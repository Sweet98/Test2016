/*=================================================================================
  class:

//=================================================================================
   $Source: interfaces_usb2/base_usb2/usb2base.cpp $
   $Revision: 1.40 $
   $Date: 2011/03/31 14:33:41EDT $

//=================================================================================

//=================================================================================*/
#include <stdio.h>
#include "stdInclude.h"
#include "Usb2Base.h"
#include "UsbRem.h"
#include "MenuGlueDriver.h"
#include "rxfifo.h"
#include "ParseHelper.h"
#include "barcodedata.h"
#include "CommonDriver.h"
#include "usb2resu.h"
#include "threadeddriver.h"
#include "db_sharedtable.h"
#include "HardwareToken.h"



extern const UCHAR pxProductStringDescriptorA[USB_SIZE_STRING_PRODUCT];
extern const UCHAR pxSerialStringDescriptorA[USB_SIZE_STRING_SERIAL];
 
CUsb2Base::CUsb2Base(CRxFifo *pRxFifo, CThreadRealTime *pThread)
{
	m_ParentDriver = NULL;
	termid_eps = 130; 
	m_ConTimes = 0;
			
	ucControlState = 0;
	ucControlStateLine = 0;
	m_BusState =USB_UNPLUGGED;
	m_BandwithSaveMode = false;
	m_cts_active = true;
	m_BeepEnabled = true;
	m_SpdHighEnabled = false;
	m_dataBufCtrl = true;
	mUsbRecoveryTimeout = 0;
	m_bRemIfaceEnabled = false;
	m_bSimpleInterfaceEnabled = false;
	CUsbRem::Init();

	memset((void *)&ControlData,0,sizeof(CONTROL_XFER));
	ControlData.dataBuffer = new unsigned char [MAX_CONTROLDATA_SIZE+64];
	
	rxMaxLenIDs = RXMAXSIZE;	
	datalenEP = 0;	
	maxTxdata = EP_MAX_LENGTH_TRANSFER*DATABSIZE;   //(16k - 1)*DATABSIZE
	dataFromEP = new unsigned char [RXMAXSIZE];
	dataFromEPsec = new unsigned char [RXMAXSIZE];
	dataToEP = new unsigned char [TXMAXSIZE];
	memset((UCHAR *)dataFromEP, 0, RXMAXSIZE);	
	memset((UCHAR *)dataFromEPsec, 0, RXMAXSIZE);		
	memset((UCHAR *)dataToEP, 0, TXMAXSIZE);		

	//For kbd
	m_datakbdBufCtrl = true;
	datakbdFromEPsec = new unsigned char [64];
	datakbdFromEP = new unsigned char [64];
	memset((UCHAR *)datakbdFromEPsec, 0, 64);		
	memset((UCHAR *)datakbdFromEP, 0, 64);				

	pxProductStrDesLEN = 0;
	pxSerialStrDesLEN = 0;
	pxInterfaceStrDesLEN[0] = 0;
	pxInterfaceStrDesLEN[1] = 0;
	
	usb_descr_FS = NULL;
	usb_descr_HS = NULL;
	usb_othersp_descr = NULL;
	usb_descrLEN_FS = 0;
	usb_descrLEN_HS = 0;
	usb_othersp_descrLEN = 0;
		
	m_TxEnabled=true;
	m_usb2DeferedPowerUpBeep = false;
	
	m_pThread = pThread;
	m_pRxFifo = pRxFifo;
	m_pRemRxFifo = NULL; // drvUsbRem will set this when it opens ReM interface.
	m_pMenuResponse = NULL;	
	
	DeviceDescr = NULL;
	HS_Device_Qualifier = NULL;
	bEPPflags.value = 0;
	
	usbd_qh_basemem = NULL;
	usbd_qh_base = NULL;
	
	MenuResponseEvent = MT_CreateEvent(false); 
	SetQueuesToNonCached(); 
}

CUsb2Base::~CUsb2Base(void)
{
	delete [] dataFromEP;
	delete [] dataFromEPsec;
	delete [] dataToEP;
	delete [] datakbdFromEPsec;
	delete [] datakbdFromEP;
	delete [] ControlData.dataBuffer;
	MT_CloseEvent(MenuResponseEvent);
}

size_t CUsb2Base::StuffInputBuffer( const UCHAR* pcInputData, size_t size )
{	
	ASSERT(m_pRxFifo != NULL);
	return m_pRxFifo->Write(pcInputData, size);
}

size_t CUsb2Base::StuffInputBuffer( const UCHAR* pcInputData )
{
	return StuffInputBuffer( pcInputData, strlen((const char*)pcInputData) );
}

/*----------------------------------------------------------------------------------------
ExtractParameters

  Extracts parameters (typcially Menu Settings).

  Note:  if we have each derived class call this LAST instead of FIRST, we can probably
  avoid the need for some of the virtual methods like GetMaxNumIfaces().  We could just
  get the count in the apppropriate CDescriptorQueue and use that.

  Parameters:  
  			pParameters[in]:  comma delimited parameters

  Returns:  ERR_PASS, if OK, ERR_NEED_RECONNECT if we need to re-enumerat, or an error.
----------------------------------------------------------------------------------------*/
MatrixError_t CUsb2Base::ExtractParameters(const tichar_t *  pParameters )
{
	MatrixError_t status = ERR_PASS;
	bool TmpRemIfc;
	bool TmpUsbSimpleInterface = false;
	static bool bFirstTime = true; // TODO:  make this a member variable so derived classes can use this (they will need to call the base class' ExtractParameters() last, instead of first)

	CParseHelper::Extract(pParameters, _TI("SPD"), m_SpdHighEnabled);
	bool ret =CParseHelper::Extract(pParameters, _TI("RCVYTIM"),mUsbRecoveryTimeout);
	if (!ret) mUsbRecoveryTimeout=0;

	if( CParseHelper::Extract(pParameters, _TI("USBSMP"), TmpUsbSimpleInterface) )
	{
		if( m_bSimpleInterfaceEnabled != TmpUsbSimpleInterface )
		{
			m_bSimpleInterfaceEnabled = TmpUsbSimpleInterface;
			if ( ! bFirstTime ) status = ERR_NEED_RECONNECT;
		}
		AdjustAllDescriptorsForSimpleInterface();
	}

	// USB Simple Interface precludes REM from being enabled
	if ( !m_bSimpleInterfaceEnabled )
	{
		if( CParseHelper::Extract(pParameters, _TI("060B00"), TmpRemIfc) )
		{
			if ( m_bRemIfaceEnabled != TmpRemIfc )
			{
				m_bRemIfaceEnabled = TmpRemIfc;
				if ( ! bFirstTime ) status = ERR_NEED_RECONNECT;  // just in case our default for m_bRemIfaceEnabled doesn't match the MenuSetting, we don't want to re-enumerate after bootup.
			}
			int RemIfaceNum = GetMaxNumIfaces() - 1;  // ReM is always the last interface.
			int RemReportDescLen = CUsbRem::m_IfaceDescriptor.HidDesc.bDescriptorLengthLow;  // for some reason, the compiler won't let me use sizeof CUsbRem::m_IfaceDescriptor
			if ( m_bRemIfaceEnabled )
			{
				m_HidDescriptors.Add(RemIfaceNum, (const unsigned char *)&CUsbRem::m_HidDescriptor, sizeof CUsbRem::m_HidDescriptor);
				m_ReportDescriptors.Add(RemIfaceNum, CUsbRem::m_ReportDescriptor, RemReportDescLen);
			}
			else
			{
				m_HidDescriptors.Remove(RemIfaceNum);
				m_ReportDescriptors.Remove(RemIfaceNum);
			}
		}
		AdjustAllConfigDescriptorsForRem();
	}

	if ( bFirstTime ) bFirstTime = false;
	return status;
}




void CUsb2Base::SetQueuesToNonCached(void)
{
	// Ensure that qh_base points to non-cached memory to properly communicate with the peripheral
	unsigned char * pTemp;

	pTemp = (unsigned char *)ControlData.dataBuffer;
	pTemp += ( NON_CACHED_MEMORY_BASE - CACHED_MEMORY_BASE );
	ControlData.dataBuffer = pTemp;

	pTemp = (unsigned char *)dataFromEP;
	pTemp += ( NON_CACHED_MEMORY_BASE - CACHED_MEMORY_BASE );
	dataFromEP = pTemp;

	pTemp = (unsigned char *)dataFromEPsec;
	pTemp += ( NON_CACHED_MEMORY_BASE - CACHED_MEMORY_BASE );
	dataFromEPsec = pTemp;

	pTemp = (unsigned char *)dataToEP;
	pTemp += ( NON_CACHED_MEMORY_BASE - CACHED_MEMORY_BASE );
	dataToEP = pTemp;

	pTemp = (unsigned char *)datakbdFromEPsec;
	pTemp += ( NON_CACHED_MEMORY_BASE - CACHED_MEMORY_BASE );
	datakbdFromEPsec = pTemp;

	pTemp = (unsigned char *)datakbdFromEP;
	pTemp += ( NON_CACHED_MEMORY_BASE - CACHED_MEMORY_BASE );
	datakbdFromEP = pTemp;
}

void CUsb2Base::RemoteWakeUp(void)
{
	if(bEPPflags.bits.remote_wakeup == 1)
	{
		MX25USB2_resume();
	}
}

bool CUsb2Base::IsRemoteWakeUpEn(void)
{
	if(bEPPflags.bits.remote_wakeup == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}
//datapump part end

void CUsb2Base::AttachDevDescriptors(const USB_DEVICE_DESCRIPTOR *pChain, const HS_DEVICE_QUALIFIER *pChainQ)
{	
		DeviceDescr = pChain;		
		HS_Device_Qualifier = pChainQ;
}


// index:  0 is FS, 1 is HS, 2 is Other Speed
void CUsb2Base::AttachConDescriptors(UCHAR *pChain, UINT index)
{

	PatchConfigDesc((USB_CONFIG_DESCRIPTOR*)pChain);

		
	if(index == 1)
		usb_descr_HS = pChain;
	else if(index == 2)
		usb_othersp_descr = pChain;
	else
		usb_descr_FS = pChain;
}

void CUsb2Base::AttachStringDescriptors(const UCHAR *pChain, UINT index) 
{
	if (index > 1)
		pxInterfaceStringDescriptor[0] = pChain;
	else
		pxInterfaceStringDescriptor[index] = pChain;
}

// 
// In the base class implementation we disable the simple interface feature
// Classes that override this feature should allow m_bSimpleInterfaceEnabled to
// be set to true. They should adjust all descriptors to remove additional 
// interfaces. 
void CUsb2Base::AdjustAllDescriptorsForSimpleInterface(void)
{
	m_bSimpleInterfaceEnabled = false;
}

void CUsb2Base::AdjustAllConfigDescriptorsForRem(void)
{
	if ( usb_descr_FS ) AdjustThisConfigDescriptorForRem((ConfigDescriptor_t *) usb_descr_FS, &usb_descrLEN_FS);
	if ( usb_descr_HS ) AdjustThisConfigDescriptorForRem((ConfigDescriptor_t *) usb_descr_HS, &usb_descrLEN_HS);
	if ( usb_othersp_descr ) AdjustThisConfigDescriptorForRem((ConfigDescriptor_t *) usb_othersp_descr, &usb_othersp_descrLEN);
}



void CUsb2Base::AdjustThisConfigDescriptorForRem(ConfigDescriptor_t * pConfigDescr, volatile UINT * pDescrLength)
{
	if ( m_bRemIfaceEnabled )
	{
		pConfigDescr->wTotalLengthLow = GetSizeOfConfigDesc() & 0xff;
		//pConfigDescr->wTotalLengthHigh = GetSizeOfConfigDesc() >> 8;  // all of our descriptors are < 0x00ff in length
		pConfigDescr->bNumInterfaces = GetMaxNumIfaces();
	}
	else
	{
		pConfigDescr->wTotalLengthLow = (GetSizeOfConfigDesc() - sizeof(CUsbRem::IfaceDescriptor_t)) & 0xff;
		//pConfigDescr->wTotalLengthHigh = GetSizeOfConfigDesc() >> 8;  // all of our descriptors are < 0x00ff in length
		pConfigDescr->bNumInterfaces = GetMaxNumIfaces() - 1;
	}
	*pDescrLength = pConfigDescr->wTotalLengthLow;  // all of our descriptors are < 0xff in length
}

void CUsb2Base::EnableUsbSerialNumber(USB_DEVICE_DESCRIPTOR *pDeviceDesc, bool enable)
{
	if (enable)
	{
		pDeviceDesc->iSerialNumber = SERIAL_IDX;		
	}
	else
	{
		pDeviceDesc->iSerialNumber = 0;  	// and index of 0 turns of the serial number
	}
}

void CUsb2Base::PatchDeviceDesc(USB_DEVICE_DESCRIPTOR *pDeviceDesc)
{	
	USHORT VID = CMenuGlueDriver::GetUsbVID();
	USHORT PID = CMenuGlueDriver::GetUsbPID();  // This is the base number
	
	pDeviceDesc->idVendor0 = (VID & 0xFF);
	pDeviceDesc->idVendor1 = (VID >> 8);
	
	USHORT usUsbGeneration = CMenuGlueDriver::GetUsbGen();
	USHORT usUsbLevel = CMenuGlueDriver::GetUsbLev();
	
	pDeviceDesc->bcdDevice1 = usUsbGeneration;
	pDeviceDesc->bcdDevice0 = usUsbLevel;
	
	PID += GetPidOffset(m_bRemIfaceEnabled);
	pDeviceDesc->idProduct0 = PID & 0xFF;
	pDeviceDesc->idProduct1 = PID >> 8;
}

void CUsb2Base::PatchConfigDesc(USB_CONFIG_DESCRIPTOR *pConfigDesc)
{
	if ( HardwareToken.IAmIndustrial() )
	{
		pConfigDesc->MaxPower = 500/2; // MaxPower is in 2 mA units
	}
}

// A helper to copy a new contents to the USB strings
void CUsb2Base::PatchString(UCHAR *pDest, HSTRING *pSource)
{
	wchar_t *pUniDest   = (wchar_t*) &pDest[2];  	// skip the size and destriptor type
	wchar_t *pUniSource = (wchar_t*) pSource->Char; // get the text to copy
	
	int MaxSize = pSource->Size/sizeof(wchar_t);    // get num characters
	
	if (MaxSize > CUSTOMIZABLE_STRING_LENGTH)  // safty net
	{
		MaxSize = CUSTOMIZABLE_STRING_LENGTH; //must less than 32
	}
	if(MaxSize==0)	                           // safety net agains a zero lenght string
	{
		return;
	}
	
	// Copy the string to the USB descriptor. Maximum string size is 32 characters w/o the trailing 0.
	for(int size=0;  size<MaxSize; size++)
	{
		*pUniDest++ = *pUniSource++;        // store the character
	} 

	*pDest = (UCHAR) (MaxSize+1)*2;			// store the size info into the descriptor
}

// here we patch the default strings with values from the model config block
void CUsb2Base::CustomizeUsbStrings(void)
{		
	//PatchString(UsbStringManufacturer, CMenuGlueDriver::GetVendorName());
	PatchString((UCHAR *)pxProductStringDescriptorA, CMenuGlueDriver::GetProductName());
	pxProductStrDesLEN = pxProductStringDescriptorA[0];

	//Some operating systems do not allow character values less than 0x20, greater than 0x7F or equal to 0x2C
	//in the Serial Number descriptor.
	PatchString((UCHAR *)pxSerialStringDescriptorA, CMenuGlueDriver::GetSerNumString());
	pxSerialStrDesLEN = pxSerialStringDescriptorA[0];			
}

void CUsb2Base::StuffInputBufferAndWaitForMenuResponse(const bchar_t *pMenuString)
{
	MT_ResetEvent(MenuResponseEvent);	
	StuffInputBuffer( pMenuString, strlen((const char*)pMenuString));
	MT_WaitEvent(MenuResponseEvent,INFINITE);
	MT_ResetEvent(MenuResponseEvent);
}

// Can be called in the write finction of a driver to bypass the TxFifo.
// See SurePos driver for an example.
void CUsb2Base::StoreMenuResponse(CBarcodeData *pMenuResponse)
{
	// Here we must ensure that there are no old responses. This could happen if we get an unsolicided menu response.
	// The Free() can be savely called with a NULL pointer!
	m_pMenuResponse->Free();	            // Avoid a memory leak

	m_pMenuResponse = pMenuResponse;
	MT_SetEvent(MenuResponseEvent);        // wakeup our thread (see TalkToReaderConfig())
}

// Must be called after the menu response is no longer used.
void CUsb2Base::FreeMenuResponse(void)
{
	m_pMenuResponse->Free();	                  // Avoid a memory leak
	m_pMenuResponse = NULL;
}

void CUsb2Base::WaitUntilWeAreConfigured(void)
{
	while ( !IsConfigured() )
	{
		MT_Delay(1);
	};
}

// Allow driver class to set the parent driver pointer so we can
// signal the parent thread. First used in Surepos to signal the
// interface thread that we have a SurePOS command to translate.
void CUsb2Base::SetParentDriver( CThreadedDriver *parentDriver )
{
	m_ParentDriver = parentDriver;
}


   

