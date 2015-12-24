/* ==============================================================================
   USB HID for SurePos POS Module Source
   ==============================================================================

   $RCSfile: interfaces_usb2/hid_usb2/surepos_usb2/usb2hidsurepos.cpp $
   $Revision: 1.21.1.3 $
   $Date: 2010/09/27 14:59:33EDT $

//=================================================================================

//=================================================================================*/
//! \file

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "stdInclude.h"
#include "interfacetypedefs.h"
#include "usb2.h"
#include "usb2chip.h"
#include "usb2hidsurepos.h"
#include "ParseHelper.h"
#include "barcodedata.h"
#include "usb2pos.h"
#include "DirectIoParser.h"

#include "timeout.h"
#include "barcodedataman.h"

#include "threadeddriver.h"

#define SIZE_STRING_SUREPOS_INTERFACE 1+1+(53*2)
const UCHAR pxInterfaceStringDescriptorA[SIZE_STRING_SUREPOS_INTERFACE] = 
{
	SIZE_STRING_SUREPOS_INTERFACE,
	0x03,

	UNI('H'), UNI('a'), UNI('n'), UNI('d'), UNI('-'), UNI('h'), UNI('e'), UNI('l'),	UNI('d'), UNI(' '),
	UNI('S'), UNI('c'), UNI('a'), UNI('n'),	UNI('n'), UNI('e'), UNI('r'), UNI(' '),	UNI('('), UNI('U'),
	UNI('s'), UNI('a'), UNI('g'), UNI('e'),	UNI(' '), UNI('='), UNI(' '), UNI('4'),	UNI('B'), UNI('0'),
	UNI('0'), UNI('h'), UNI(','), UNI(' '),	UNI('U'), UNI('s'), UNI('a'), UNI('g'),	UNI('e'), UNI(' '),
	UNI('P'), UNI('a'), UNI('g'), UNI('e'),	UNI(' '), UNI('='), UNI(' '), UNI('F'),	UNI('F'), UNI('4'),
	UNI('5'), UNI('h'), UNI(')'),
};

#define SIZE_STRING_SUREPOS_FLASH 1+1+(48*2)
const UCHAR pxInterfaceStringDescriptorB[SIZE_STRING_SUREPOS_FLASH] = 
{
	SIZE_STRING_SUREPOS_FLASH,
	0x03,

	UNI('F'), UNI('l'), UNI('a'), UNI('s'),	UNI('h'), UNI(' '), UNI('U'), UNI('p'), UNI('d'), UNI('a'),
	UNI('t'), UNI('e'), UNI(' '), UNI('('),	UNI('U'), UNI('s'), UNI('a'), UNI('g'),	UNI('e'), UNI(' '),
	UNI('='), UNI(' '), UNI('A'), UNI('0'),	UNI('0'), UNI('0'), UNI('h'), UNI(','),	UNI(' '), UNI('U'),
	UNI('s'), UNI('a'), UNI('g'), UNI('e'),	UNI(' '), UNI('P'), UNI('a'), UNI('g'),	UNI('e'), UNI(' '),
	UNI('='), UNI(' '), UNI('F'), UNI('F'),	UNI('4'), UNI('5'), UNI('h'), UNI(')'),
};

//surePosHH
const USB_DEVICE_DESCRIPTOR DeviceDescrHH =
{
	0x12, 				//bLength = 12H
	0x01, 				//bDescriptorType= 01H
//	0x00,0x02,  			//bcdUSBL= usb 2.0
	0x10,0x01,  			//bcdUSBL= usb 1.1
	0x00,    			//bDeviceClass             //Change for HID
	0x00,                           //bDeviceSubclass
	0x00,				//bDeviceProtocol				
	USB_MAX_CTRL_PAYLOAD,  //bMaxPacketSize0 EP0 PACKET SIZE,					
	0x36, 0x05,                     //idVendorL						
	USB_WORD16(USB_SUREPOS_HH_PRODUCT_ID), //idProductL
	USB_WORD16(USB_DEVICE_ID),      //bcdDeviceL						
	0x01,                           //iManufacturer
	0x02,                           //iProduct
	0x08,	      			//SerialNumber			
	0x01				//bNumConfigs								
};

const HS_DEVICE_QUALIFIER HS_Device_QualifierA =
{	
	0x0a,				// 0AH
	0x06,                           // USB DEVICE QUALIFIER DESCRIPTOR TYPE		
//	0x00,0x02,			// USB 2.0 version
	0x10,0x01,			// USB 1.1 version
	0x00,                           // Change for HID			
	0x00,										
	0x00, 										
	0x40,										
	0x01,		
	0x00																					
};	

CUsb2HidSurePos::MyConfigDescriptor_t CUsb2HidSurePos::m_MyConfigDescriptor =
{
	//Configuration descriptors	
	0x09, 		//CbLength
	0x02, 		//CbDescriptorType
	0x42,           //CwTotalLength                                     
	0x00,		
	0x02,       	//CbNumInterfaces				      
	0x01,		//CbConfigurationValue			 		
	0x03,          	//CiConfiguration		                      
	USB_BM_ATTRIBUTES,     	//CbmAttributes Bus powered + Remote Wakeup				
	USB_POWER/2, 		//CMaxPower

        //Interface Descriptor Requirement
	0x09,  			//bLength
	0x04,  			//bDescriptorType
	0x00,			//bInterfaceNumber	                      				
	0x00,			//bAlternateSetting					
	0x01,                   //bNumEndpoints					
	0x03,   		//bInterfaceClass: HID Class	              
	0x00,   		//bInterfaceSubclass(boot device)    
	0x00,    		//bInterfaceProtocol(kbd: HID P19)
	0x04,    	        //iInterface

	//HID descriptor(HID P58)
	0x09,  			//bLength
	0x21,  			//bDescriptorType
	USB_WORD16(0x0110),     //bcdHID
	0x00,                   //bCountryCode
	0x01,   		//bNumDescriptors
	0x22,   		//bDescriptorType
	0x1e,    		//wDescriptorLength(low)                          
	0x00,   		//wDescriptorLength(high)

        //endp 2 interrupt IN
	0x07,  			        //bLenght
	0x05,  				//bDescriptorType
	0x82,				//bEndpointAddress, Endpoint 02 - IN						
	0x03,  				//bmAttributes	  INT
	64,                             //wMaxPacketSize: 64 bytes
	0x00,                           
	0x02,  				//bInterval	

        //Interface Descriptor Requirement
	0x09,  			//bLength
	0x04,  			//bDescriptorType
	0x01,			//bInterfaceNumber	                      				
	0x00,			//bAlternateSetting					
	0x02,                   //bNumEndpoints					
	0x03,   		//bInterfaceClass: HID Class	              
	0x00,   		//bInterfaceSubclass(boot device)    
	0x00,    		//bInterfaceProtocol(kbd: HID P19)
	0x05,    	        //iInterface(index string descriptor of this interface)  				      

	//HID descriptor(HID P58)
	0x09,  			//bLength
	0x21,  			//bDescriptorType
	USB_WORD16(0x0110),     //bcdHID
	0x00,                   //bCountryCode
	0x01,   		//bNumDescriptors
	0x22,   		//bDescriptorType
	0x21,    		//wDescriptorLength(low)                          
	0x00,   		//wDescriptorLength(high)

        //endp 4 interrupt IN
	0x07,  			        //bLenght
	0x05,  				//bDescriptorType
	0x84,				//bEndpointAddress, Endpoint 04 - IN						
	0x03,  				//bmAttributes	  INT
	64,                             //wMaxPacketSize: 64 bytes
	0x00,                           
	0x04,  				//bInterval	

        //endp 7 interrupt OUT
	0x07,  			        //bLenght
	0x05,  				//bDescriptorType
	0x07,				//bEndpointAddress, Endpoint 07 - OUT						
	0x03,  				//bmAttributes	  INT
	64,                             //wMaxPacketSize: 64 bytes
	0x00,                           
	0x01				//bInterval			
};



const UCHAR usb_hid_descrA[] =
{
	//HID descriptor(HID P58)
	0x09,  			//bLength
	0x21,  			//bDescriptorType
	USB_WORD16(0x0110),     //bcdHID
	0x00,                   //bCountryCode
	0x01,   		//bNumDescriptors
	0x22,   		//bDescriptorType
	0x1e,    		//wDescriptorLength(low)                      
	0x00     		//wDescriptorLength(high)
};

const UCHAR usb_hid_descrB[] =
{
	//HID descriptor(HID P58)
	0x09,  			//bLength
	0x21,  			//bDescriptorType
	USB_WORD16(0x0110),     //bcdHID
	0x00,                   //bCountryCode
	0x01,   		//bNumDescriptors
	0x22,   		//bDescriptorType
	0x21,    		//wDescriptorLength(low)                       
	0x00     		//wDescriptorLength(high)
};


const UCHAR usb_report_descrA[] =
{
    0x06, 0x45, 0xff,              // USAGE_PAGE (Vendor Defined Page (OEMPOS))
    0x0a, 0x00, 0x4b,              // USAGE (Hand-held Scanner)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x0a, 0x01, 0x4a,              // USAGE (Scanner Command)
    0x75, 0x08,                    // REPORT_SIZE (8)
    0x95, 0x0b,                    // REPORT_COUNT (11)
    0x15, 0x00,                    // LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              // LOGICAL_MAXIMUM (255)
    0x91, 0x02,                    // OUTPUT (Data,Var,Abs)
    0x0a, 0x02, 0x4a,              // USAGE (Scanner Status)
    0x95, 0x40,                    // REPORT_COUNT (64)
    0x81, 0x02,                    // INPUT (Data,Var,Abs)
    0xc0                           // END_COLLECTION
};

const UCHAR usb_report_descrB[] =
{
    0x06, 0x45, 0xff,              // USAGE_PAGE (Vendor Defined Page (SurePos))
    0x0a, 0x00, 0xa0,              // USAGE (Flash Report)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x75, 0x08,                    // REPORT_SIZE (8)
    0x96, 0x07, 0x01,              // REPORT_COUNT (263)
    0x15, 0x00,                    // LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              // LOGICAL_MAXIMUM (255)
    0x0a, 0x01, 0xa0,              // USAGE (Flash Commands and associated data)
    0x91, 0x02,                    // OUTPUT (Data,Var,Abs)
    0x75, 0x08,                    // REPORT_SIZE (8)
    0x95, 0x02,                    // REPORT_COUNT (2)
    0x0a, 0x02, 0xa0,              // USAGE (Flash Status)
    0x81, 0x02,                    // INPUT (Data,Var,Abs)
    0xc0                           // END_COLLECTION
};

//surePosTT
const UCHAR pxInterfaceStringDescriptorATT[SIZE_STRING_SUREPOS_INTERFACE] = 
{
	SIZE_STRING_SUREPOS_INTERFACE,
	0x03,

	UNI('T'), UNI('a'), UNI('b'), UNI('l'), UNI('e'), UNI('-'), UNI('t'), UNI('o'),	UNI('p'), UNI(' '),
	UNI('S'), UNI('c'), UNI('a'), UNI('n'),	UNI('n'), UNI('e'), UNI('r'), UNI(' '),	UNI('('), UNI('U'),
	UNI('s'), UNI('a'), UNI('g'), UNI('e'),	UNI(' '), UNI('='), UNI(' '), UNI('4'),	UNI('A'), UNI('0'),
	UNI('0'), UNI('h'), UNI(','), UNI(' '),	UNI('U'), UNI('s'), UNI('a'), UNI('g'),	UNI('e'), UNI(' '),
	UNI('P'), UNI('a'), UNI('g'), UNI('e'), UNI(' '), UNI('='), UNI(' '), UNI('F'),	UNI('F'), UNI('4'),
	UNI('5'), UNI('h'), UNI(')'),
};

const USB_DEVICE_DESCRIPTOR DeviceDescrTT =
{
	0x12, 				//bLength = 12H
	0x01, 				//bDescriptorType= 01H
//	0x00,0x02,  			//bcdUSBL= usb 2.0
	0x10,0x01,  			//bcdUSBL= usb 1.1
	0x00,    			//bDeviceClass             //Change for HID
	0x00,                           //bDeviceSubclass
	0x00,				//bDeviceProtocol				
	USB_MAX_CTRL_PAYLOAD,  //bMaxPacketSize0 EP0 PACKET SIZE,					
	0x36, 0x05,                     //idVendorL						
	USB_WORD16(USB_SUREPOS_TT_PRODUCT_ID), //idProductL
	USB_WORD16(USB_DEVICE_ID),      //bcdDeviceL						
	0x01,                           //iManufacturer
	0x02,                           //iProduct
	0x08,	      			//SerialNumber			
	0x01				//bNumConfigs								
};

const UCHAR usb_report_descrATT[] =
{
    0x06, 0x45, 0xff,              // USAGE_PAGE (Vendor Defined Page (OEMPOS))
    0x0a, 0x00, 0x4a,              // USAGE (Table Top Scanner)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x0a, 0x01, 0x4a,              // USAGE (Scanner Command)
    0x75, 0x08,                    // REPORT_SIZE (8)
    0x95, 0x0b,                    // REPORT_COUNT (11)
    0x15, 0x00,                    // LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              // LOGICAL_MAXIMUM (255)
    0x91, 0x02,                    // OUTPUT (Data,Var,Abs)
    0x0a, 0x02, 0x4a,              // USAGE (Scanner Status)
    0x95, 0x40,                    // REPORT_COUNT (64)
    0x81, 0x02,                    // INPUT (Data,Var,Abs)
    0xc0                           // END_COLLECTION
};

bool IBM4690RetailPDFEnable;
int IBM4690SurePosMaxPacketSize;
extern bool ExecuteRetailPDF(void);
CUsb2HidSurePos::CUsb2HidSurePos(CRxFifo *pRxFifo, CThreadRealTime *pThread, bool TableTop)
: CUsb2Hid(pRxFifo, pThread)
{
	int i;

	m_bIsTableTop = TableTop;
	//variable init		
	m_IgnoreConfigFlag	= false;
	m_IgnoreDisableFlag	= false;
	m_IgnoreBeepFlag  	= false;
	m_SelectOCRFlag		= false;
	m_UseBCDFlag		= false;

	m_ucStatus0 = OST0_GOOD_READ_BEEP_ENABLED;
	m_ucStatus1 = OST1_SCANNER_ALIVE | OST1_SCANNER_ENABLED;

	m_ucFlashStatus0 = FST0_DEFAULT;
	m_bFlashResetStatus0 = FALSE;

	m_ucNumInterfaces = 1;

	for (i=0; i<=ConfigSize; i++)	// initialize to something
	{                                 //10 bytes total, 9 configuration bytes (cfg 0-8) plus one direct/IO config byte
		m_CurrentConfig[i] = 0x0;
	}

	for (i=0;i<TWO_LABEL_CONFIG_SIZE;i++)
		m_ucTwoLabelFlagConfig[i] = 0;

	totalDEP = 0;
	rxMaxLenIDs = 263; //RXMAXSIZE;
	
	if(TableTop)
	{
		termid_eps = 129;   //UsbSurePosTT
	}
	else
	{
		termid_eps = 128;   //UsbSurePosHH
	}

	m_MyConfigDescriptor.RemIfaceDesc = CUsbRem::m_IfaceDescriptor;
	m_MyConfigDescriptor.RemIfaceDesc.IfaceDesc.bInterfaceNumber = GetMaxNumIfaces() - 1; // ReM is the last interface;

	m_MyOtherSpeedConfigDescriptor = m_MyConfigDescriptor;
	m_MyOtherSpeedConfigDescriptor.ConfigDesc.bDescriptorType = USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE;
	m_MyOtherSpeedConfigDescriptor.ScannerInEndpoint.bEndpointAddress = 0x84;  // IN, 4
	m_MyOtherSpeedConfigDescriptor.FlashUpdateInEndpoint.bEndpointAddress = 0x82; // IN, 2

	InitUSBEndpDes(TableTop);

	if(!IBM4690RetailPDFEnable)
		IBM4690SurePosMaxPacketSize = 64;
		
	DisableTx();		// CVS Only - come up disabled until we get an enable
	dioParser = new DirectIoParser(pRxFifo);
}

CUsb2HidSurePos::~CUsb2HidSurePos(void)
{
	//disable endpoints
	MX25USB2_eps_dis(2);	
	MX25USB2_eps_dis(4);
	MX25USB2_eps_dis(7);
	delete dioParser;
	dioParser = NULL;
}

void CUsb2HidSurePos::InitUSBEndpDes(bool TableTop)
{		
	if(TableTop)
	{
		m_bIsTableTop = true;
		AttachDevDescriptors(&DeviceDescrTT, &HS_Device_QualifierA);
	}
	else
	{
		m_bIsTableTop = false;
		AttachDevDescriptors(&DeviceDescrHH, &HS_Device_QualifierA);
	}
	// Setup PID and VID
	
	AttachConDescriptors((UCHAR *)&m_MyConfigDescriptor, 0);
	AttachConDescriptors((UCHAR *)&m_MyConfigDescriptor, 1);	
	AttachConDescriptors((UCHAR *)&m_MyOtherSpeedConfigDescriptor, 2);
	usb_descrLEN_FS = sizeof(m_MyConfigDescriptor);
	usb_descrLEN_HS = sizeof(m_MyConfigDescriptor);
	usb_othersp_descrLEN = sizeof(m_MyOtherSpeedConfigDescriptor);

	m_HidDescriptors.Add(0, usb_hid_descrA, sizeof usb_hid_descrA);
	m_HidDescriptors.Add(1, usb_hid_descrB, sizeof usb_hid_descrB);
	
	if(TableTop) m_ReportDescriptors.Add(0, usb_report_descrATT, sizeof usb_report_descrATT);
	else m_ReportDescriptors.Add(0, usb_report_descrA, sizeof usb_report_descrA);
	m_ReportDescriptors.Add(1, usb_report_descrB, sizeof usb_report_descrB);
	

	if(TableTop)
	{
		AttachStringDescriptors(pxInterfaceStringDescriptorATT, 0);
		pxInterfaceStrDesLEN[0] = sizeof(pxInterfaceStringDescriptorATT);	
	}	
	else
	{
		AttachStringDescriptors(pxInterfaceStringDescriptorA, 0);
		pxInterfaceStrDesLEN[0] = sizeof(pxInterfaceStringDescriptorA);	
	}	
		
	AttachStringDescriptors(pxInterfaceStringDescriptorB, 1);		
	pxInterfaceStrDesLEN[1] = sizeof(pxInterfaceStringDescriptorB);	
	
	CustomizeUsbStrings();
}

/*----------------------------------------------------------------------------------------
ExtractParameters

  Extracts parameters (typcially Menu Settings).  It will call it's base class' (not to be
  confused with CUsbBase) ExtractParameters() method to enusure all classes have the
  chance to extract params.  Any error code or ERR_NEED_RECONNECT from the base class will
  be returned if there's no (other) errors.

  Parameters:  
  			pParameters[in]:  comma delimited parameters

  Returns:  ERR_PASS if OK, ERR_NEED_RECONNECT if we need to re-enumerate, or an error.
----------------------------------------------------------------------------------------*/
MatrixError_t CUsb2HidSurePos::ExtractParameters(const tichar_t *pParameters )
{
	bool ret=PASS;
	MatrixError_t RetVal = CUsb2Hid::ExtractParameters(pParameters); // make sure all classes get a chance to extract params
	PatchDeviceDesc((USB_DEVICE_DESCRIPTOR *)DeviceDescr);
		
	ret &= CParseHelper::Extract(pParameters, _TI("060F03"), m_IgnoreConfigFlag);
	ret &= CParseHelper::Extract(pParameters, _TI("060F07"), m_UseBCDFlag);
	ret &= CParseHelper::Extract(pParameters, _TI("060F04"), m_IgnoreDisableFlag);
	ret &= CParseHelper::Extract(pParameters, _TI("060F05"), m_IgnoreBeepFlag);
	ret &= CParseHelper::Extract(pParameters, _TI("060F06"), m_SelectOCRFlag);

	if ( ! ret ) RetVal = ERR_BADPARAM;
	return RetVal;
}



UCHAR CUsb2HidSurePos::GetPidOffset(bool bIsRem)
{
	UCHAR PidOffset;

	if ( m_bIsTableTop )
	{
		if ( bIsRem ) PidOffset = USB_SUREPOS_TT_REM_PID_OFFSET;
		else PidOffset = USB_SUREPOS_TT_PID_OFFSET;
	}
	else
	{
		if ( bIsRem ) PidOffset = USB_SUREPOS_HH_REM_PID_OFFSET;
		else PidOffset = USB_SUREPOS_HH_PID_OFFSET;
	}
	return PidOffset;
}


inline usb_err_t CUsb2HidSurePos::Ep0DataHandler(void)
{
	return ReportHandler();	
}

// A helper to search through the report table and call the required handler
// It gets called by the HID framework for any get/set report request
inline usb_err_t CUsb2HidSurePos::ReportHandler(void)
{
	usb_err_t RetVal=RET_STALL;

	USHORT nInterface= ControlData.DeviceRequest.wIndex;

	switch (nInterface)
	{
	case DATA_INTERFACE:
		{
			const THidReportTable<CUsb2HidSurePos> *pPosTable = ms_DataReports;
			RetVal = FindAndCallReportHandler (pPosTable);
		}
		break;
	case FLASH_INTERFACE:
		{
			const THidReportTable<CUsb2HidSurePos> *pPosTable = ms_FlashReports;
			RetVal = FindAndCallReportHandler (pPosTable);
		}
		break;
	default:
		break;
	}
	
	return RetVal;
}

int CUsb2HidSurePos::GetReportSize(DEVICE_REQUEST *  /*pRequest*/)
{
	int RetVal=-1;	

	USHORT nInterface= ControlData.DeviceRequest.wIndex;

	switch (nInterface)
	{
	case DATA_INTERFACE:
		{
			const THidReportTable<CUsb2HidSurePos> *pPosTable = ms_DataReports;
			RetVal = CUsb2Hid::GetReportSize (pPosTable);
		}
		break;
	case FLASH_INTERFACE:
		{
			const THidReportTable<CUsb2HidSurePos> *pPosTable = ms_FlashReports;
			RetVal = CUsb2Hid::GetReportSize (pPosTable);
		}
		break;
	default:
		break;
	}
	return RetVal;	
}

usb_err_t CUsb2HidSurePos::EP0_InEmptyHandler(void)
{
	SurePosInReportBuffer_t *pReport = (SurePosInReportBuffer_t*) dataToEP;
	// send an "all keys up" here 
	pReport->Clear();
	// send an empty report here
	pReport->u.report.ucLength = 4;
	pReport->u.report.ucStatus0 = m_ucStatus0;
	pReport->u.report.ucStatus1 = m_ucStatus1;
	pReport->u.report.ucStatus2 = 0;

	code_transmit((UCHAR *)dataToEP, pReport->GetBufferSize());
	
	return RET_ACK;
}

// gets called when we gotthe report via EP0
usb_err_t CUsb2HidSurePos::OutReportHandler(void)
{
	m_ParentDriver->Signal(Sig_ByteReceived);
	return RET_NOTHING;
}

/* This table lists all known HID reports */
const THidReportTable<CUsb2HidSurePos> CUsb2HidSurePos::ms_DataReports[] =
{
	// type,			report ID,	size,				Handler
	// IN 
	USB_HID_REPORT_IN,		 0,		HID_SURE_POS_IN_REPORT_SIZE,	&CUsb2HidSurePos::EP0_InEmptyHandler,
	// OUT
	USB_HID_REPORT_OUT,		 0,		HID_SURE_POS_OUT_REPORT_SIZE,	&CUsb2HidSurePos::OutReportHandler,
	// END 
	USB_HID_REPORT_TABLE_END, 	 0,		0,				NULL
};

// A table to speed up the translation to BCD
static const char ValTable[256] =
{
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  0,  0,  0,  0,  0,  0,
	0,  0xA,0xB,0xC,0xD,0xE,0xF,0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0xA,0xB,0xC,0xD,0xE,0xF,0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,

	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};

// All lables but C39, C93, C128, I25, unknown are send as BCD
//	We use the postambles to distinguish. See the formats in the pnp section.
void CUsb2HidSurePos::TranslateIntoBCD (SurePosInReportBuffer_t *pInReport, CBarcodeData * /*pLabel*/)
{
	UCHAR *pSourceBuffer = pInReport->GetDataPointer();
	size_t iNumberOfBytes = pInReport->GetLength();

	int i;
	int iLastByte = iNumberOfBytes-1;
	int iLastLabel = -1;
	if ( (pSourceBuffer[iLastByte] == 0x0C)	
		||(pSourceBuffer[iLastByte] == 0x16)	
		||(pSourceBuffer[iLastByte] == 0x0D)	
		||(pSourceBuffer[iLastByte] == 0x0A)	
		)
	{
		iLastLabel = iLastByte-1;	// Needs BCD
	}
	else if (pSourceBuffer[iLastByte] == 0x0B)
	{
		if ( (pSourceBuffer[iLastByte-1] == 0x1D)		
			||	( (pSourceBuffer[iLastByte-1] >= 0x11)	
				&&(pSourceBuffer[iLastByte-1] <= 0x17)	
				)
			)
		{
			iLastLabel = iLastByte-3;	// Needs BCD 
		}
	}

	for (i=0; i<=iLastLabel; i++)
	{
		*pSourceBuffer = ValTable[*pSourceBuffer];
		pSourceBuffer++;
	}
}

// This function's purpose has changed: PDF417, Maxicode, and RSS Expanded label identifiers are now done with the data formatter.  
// This function will now break up large packets and append the correct label identifier to each chunk of data
// and it will not modify the final pieces of the barcodes when they have the correct identifier
int CUsb2HidSurePos::SendLargeLabel(CBarcodeData *pLabel)
{	
	bool bResetInterface = FALSE;

	bool bIsMoreDataFlag = TRUE;
	const int SUFFIX_SIZE = 3;
	int BlockSize = IBM4690SurePosMaxPacketSize - 4 -SUFFIX_SIZE;
	bchar_t ucThisHHPId;
	size_t uiCurrentPacketSize = 0;
	size_t uiLength = 0;
	const UCHAR *pCurrentText = NULL;  

	pLabel->GetTotalData(pCurrentText, uiLength);
	ucThisHHPId = pLabel->GetHHPId();

	SurePosInReportBuffer_t *pInReport = (SurePosInReportBuffer_t *)dataToEP;

	while( uiLength > 0 )
	{
		pInReport->Clear();

		uiCurrentPacketSize = uiLength;
		bIsMoreDataFlag = FALSE;
		// fill the "more data to follow" flag
		if( uiCurrentPacketSize > BlockSize )
		{
			uiCurrentPacketSize = BlockSize;
			bIsMoreDataFlag = TRUE;
		}

		pInReport->ClearData();

		// fill in status
		pInReport->u.report.ucStatus0 = m_ucStatus0;
		pInReport->u.report.ucStatus1 = m_ucStatus1;
		pInReport->u.report.ucStatus2 = 0;
		// copy data
		pInReport->CopyData(pCurrentText, uiCurrentPacketSize);

		// set data editing size info
		pInReport->SetLength( (UCHAR)(uiCurrentPacketSize) );

		if(bIsMoreDataFlag)	// if this is not the final piece of data in the code
		{
			if( ucThisHHPId == 'r' )	// if PDF add first 2 bytes
			{
				if(IBM4690RetailPDFEnable)
				{
					pInReport->AddByte((UCHAR)PDF_FIRST_DATA_CONTINUATION_BYTE);
					pInReport->AddByte((UCHAR)(ID_BYTE_ONE_PDF_NEW4690OS));
				}
				else
				{
					pInReport->AddByte((UCHAR)MORE_DATA_CONTINUATION_BYTE);
					pInReport->AddByte((UCHAR)ID_BYTE_ONE_PDF);
				}
			}
			else	// Not PDF
			{
				pInReport->AddByte((UCHAR)MORE_DATA_CONTINUATION_BYTE);	// Add continuation byte
				
				// Get the next byte from data formatter if it exists, else make it unknown
				if(pCurrentText[uiLength-3] == LAST_DATA_CONTINUATION_BYTE && pCurrentText[uiLength-1] == ID_BYTE_TWO_ALL)
				{
					pInReport->AddByte(pCurrentText[uiLength-2]);
				}
				else
				{
					pInReport->AddByte((UCHAR)ID_BYTE_ONE_UNKNOWN);
				}
			}
			// add third byte label identifier
			pInReport->AddByte((UCHAR)ID_BYTE_TWO_ALL);

			// fill in actual packet size info
			pInReport->SetLength( (UCHAR)(uiCurrentPacketSize + 4 + SUFFIX_SIZE) );
			
			pCurrentText += uiCurrentPacketSize;	// adjust location in string
		}
		else	// final piece of barcode data
		{
			// fill in actual packet size info
			pInReport->SetLength( (UCHAR)(uiCurrentPacketSize + 4 ) );
		}
		uiLength -= uiCurrentPacketSize;	// adjust size

		//convert to Binary coded decimal if menu selected
		if (m_UseBCDFlag)
		{
			TranslateIntoBCD(pInReport, pLabel);
			// some older versions of the OS4690 (< 2.4) need the EAN/UPC as BCD 
		}
		SendReportWithTest (DATA_INTERFACE, HID_USBPOS_IN_REPORT_SIZE, (UCHAR *)pInReport);
	}	// repeat while loop

	return bResetInterface?-1:0;
}

// Send the current barcode to the host
int CUsb2HidSurePos::SendLabel(CBarcodeData *pLabel)
{
	bool bResetInterface = FALSE;

	if(IsTxEnabled())
	{
		size_t Length = 0;
		const UCHAR *pCurrentText = NULL;  

		pLabel->GetTotalData(pCurrentText, Length);

		ExecuteRetailPDF(); //update RTLPDF value after repower.
		if(!IBM4690RetailPDFEnable)
			IBM4690SurePosMaxPacketSize = 64;	

		// If the code is longer than the max packet size and requires special code identifiers call Send Large Label 
		if( (Length + 4) > IBM4690SurePosMaxPacketSize)
		{
			SendLargeLabel(pLabel);
		}
		else
		{
			SurePosInReportBuffer_t *pInReport = (SurePosInReportBuffer_t *)dataToEP;

			pInReport->SetLength((UCHAR) Length + 4 );
			pInReport->u.report.ucStatus0 = m_ucStatus0;
			pInReport->u.report.ucStatus1 = m_ucStatus1;
			pInReport->u.report.ucStatus2 = 0;
			pInReport->ClearData();
			pInReport->CopyData(pCurrentText, Length);

			if (m_UseBCDFlag)
			{
				TranslateIntoBCD(pInReport, pLabel);
				// some older versions of the OS4690 (< 2.4) need the EAN/UPC as BCD 
			}
			SendReportWithTest (DATA_INTERFACE, HID_USBPOS_IN_REPORT_SIZE, (UCHAR *)pInReport);
		}
	}
	
	return bResetInterface?-1:0;
}

///////////////////////////////////////////////////////////////////////////////
// specific functions for the firmware flash
///////////////////////////////////////////////////////////////////////////////
// Firmware flash is done with a second HID interface.

// This table lists all known HID reports
const THidReportTable<CUsb2HidSurePos> CUsb2HidSurePos::ms_FlashReports[] =
{
	//type,				report ID,	size,					Handler 
	// IN 
	USB_HID_REPORT_IN,		0,		HID_SURE_POS_FLASH_IN_REPORT_SIZE,	&CUsb2HidSurePos::EP0_FlashInStatusHandler,
	// OUT 
	USB_HID_REPORT_OUT,		0,		HID_SURE_POS_FLASH_OUT_REPORT_SIZE,	&CUsb2HidSurePos::EP0_FlashOutReportHandler,
	// END
	USB_HID_REPORT_TABLE_END, 	0,		0,												NULL
};

// gets called when we got the report via EP0
usb_err_t CUsb2HidSurePos::EP0_FlashOutReportHandler(void)
{	
	memcpy((UCHAR *)dataForHandler, (UCHAR *)ControlData.dataBuffer, ControlData.wLength);

	m_ParentDriver->Signal(Sig_FlashCommandReceived);
	return RET_NOTHING;
}

// Callback for receiving from the EP 
inline void CUsb2HidSurePos::EPRxHandleReceivedData(UINT pEp)
{
	if(m_dataBufCtrl)
	{	
		m_dataBufCtrl = false;
		datalenEP = MX25USB2_ReadEndpoint_Data(pEp, rxMaxLenIDs, false, dataFromEPsec);  //endpoint 7	
        if(datalenEP > 0)  
        {
			memcpy((UCHAR *)dataForHandler+totalDEP, (UCHAR *)dataFromEP, datalenEP);
			totalDEP += datalenEP;		
		}
	}		
	else
	{
		m_dataBufCtrl = true;
		datalenEP = MX25USB2_ReadEndpoint_Data(pEp, rxMaxLenIDs, false, dataFromEP);  //endpoint 7	
        if(datalenEP > 0)  
        {
			memcpy((UCHAR *)dataForHandler+totalDEP, (UCHAR *)dataFromEPsec, datalenEP);
			totalDEP += datalenEP;		
		}
	}
	
	if (((datalenEP <= 11) && (datalenEP > 0)) || (totalDEP >= HID_SURE_POS_FLASH_OUT_REPORT_SIZE))
	{	
		totalDEP = 0;
		m_ParentDriver->Signal(Sig_FlashCommandReceived);
	}	
}


///////////////////////////////////////////////////////////////////////////////
// specific functions for the firmware flash
///////////////////////////////////////////////////////////////////////////////
// Firmware flash is done with a second HID interface.


// gets called when we got the request via EP0
usb_err_t  CUsb2HidSurePos::EP0_FlashInStatusHandler(void)
{
	SendFlashStatus(0, true);
	return RET_ACK;
}

void CUsb2HidSurePos::SendFlashStatus(UCHAR FlashStatus, bool RequestCameFromEP0)
{
	SurePosFlashResponseReportBuffer_t *pInReport = (SurePosFlashResponseReportBuffer_t *) dataToEP;

	pInReport->u.report.ucStatus0 = m_ucFlashStatus0|FlashStatus;
	pInReport->u.report.ucStatus1 = 0;
	if( RequestCameFromEP0 )	// send the response to EP0 if that's where the request came from
	{
		code_transmit((UCHAR *)dataToEP, ControlData.DeviceRequest.wLength);
	}
	else	// otherwise send to the flash endpoint
	{
		SendReportWithTest (FLASH_INTERFACE, HID_SURE_POS_FLASH_IN_REPORT_SIZE, (UCHAR *)pInReport);
	}
}

void CUsb2HidSurePos::FlashCommandDispatcher(void)
{	
	const SurePosFlashOutReportBuffer_t *pReport = (const SurePosFlashOutReportBuffer_t *) dataForHandler;
	
	switch(pReport->u.report.Cmd1)
	{
		case  SYSTEM_COMMAND:	// 0x00 
			FlashSystemCommandDispatcher(pReport);
			break;
		case  START_FLASH:		// 0x01
			m_ucStatus0 	  |= OST0_FLASH_IN_PROGRESS;
			m_ucFlashStatus0 |= FST0_FLASH_IN_PROGRESS;
	  		SendFlashStatus (FST0_COMMAND_COMPLETE, false);
			break;
		case  WRITE_FLASH_RECORD:	// 0x02
			if(pReport->u.report.DataCount != 0)
			{				
				size_t written;
				do
				{
					written = StuffInputBuffer( pReport->u.report.Data, pReport->u.report.DataCount );
					if(written!=pReport->u.report.DataCount)
					{
						MT_Sleep(1000);
					}
				}while(written!=pReport->u.report.DataCount);
			}
			else
			{
				// HTAG format automatically starts the flashwrite followed by a reboot
				m_bFlashResetStatus0 = TRUE;
			}
	  		SendFlashStatus (FST0_COMMAND_COMPLETE, false);
			break;
		default:
			SendFlashStatus (FST0_COMMAND_REJECT, false);  // reject
			m_bFlashResetStatus0 = FALSE;
			break;
	}	
}

void CUsb2HidSurePos::FlashSystemCommandDispatcher(const SurePosFlashOutReportBuffer_t *pOutReport)
{
	switch (pOutReport->u.report.Cmd2)		// Sort the scanner system command 
	{
		case TEST:
		{
	  		SendFlashStatus (FST0_COMMAND_COMPLETE, false);
	  		break;
		}
		case STATUS_REQUEST:
		{
	  		SendFlashStatus (FST0_COMMAND_COMPLETE, false);
			break;
		}
		case RESET_SCANNER:
		{
			if( !((m_ucFlashStatus0 & FST0_FLASH_IN_PROGRESS) && m_bFlashResetStatus0) )      //We reset anyway , so do not reset during flash   lmc
			{
				ResetDevice();
			}
			break;
		}
		default:
			SendFlashStatus (FST0_COMMAND_REJECT, false);  // reject
			break;
	}
}

void CUsb2HidSurePos::StoreMenuResponse(CBarcodeData *pMenuResponse)
{
	// send to dio parser
	if( dioParser != NULL && dioParser->GetExpectingMenuResponse() )
	{
		dioParser->StoreMenuResponse(pMenuResponse);
	}
	else // keep for yourself
	{
		CUsb2Base::StoreMenuResponse(pMenuResponse);
	}
}




