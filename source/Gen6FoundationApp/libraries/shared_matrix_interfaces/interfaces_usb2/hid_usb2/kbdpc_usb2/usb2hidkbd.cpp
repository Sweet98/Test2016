/* ==============================================================================
   USB HID for HID Keyboard Module Source
   ==============================================================================

   $RCSfile: interfaces_usb2/hid_usb2/kbdpc_usb2/usb2hidkbd.cpp $
   $Revision: 1.22 $
   $Date: 2011/05/16 11:32:25EDT $

//=================================================================================

//=================================================================================*/
//! \file
//#pragma O0		//Debug

#include "stdInclude.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "product.h"
#include "CommonDriver.h"
#include "usb2.h"
#include "usb2chip.h"
#include "usb2hid.h"
#include "Usb2HidKbd.h"
#include "WedgeEngine.h"
#include "TransCodeEx.h"
#include "ParseHelper.h"

#include "timeout.h"
#include "barcodedataman.h"

#include "menu.h"
#include "ReaderConfiguration.h"
#include "InterfaceMenuSettings.h"

#define SIZE_STRING_HIDKBD 1+1+(22*2)
const UCHAR pxInterfaceStringDescriptorA[SIZE_STRING_HIDKBD] =
{
	SIZE_STRING_HIDKBD,
	0x03,

	UNI('H'), UNI('I'), UNI('D'), UNI(' '),	UNI('K'), UNI('e'), UNI('y'), UNI('b'),	UNI('o'), UNI('a'),
	UNI('r'), UNI('d'), UNI(' '), UNI('E'),	UNI('m'), UNI('u'), UNI('l'), UNI('a'),	UNI('t'), UNI('i'),
	UNI('o'), UNI('n'),
};

#define SIZE_STRING_USB2POS_INTERFACE (1+1+(7*2))
const UCHAR pxInterfaceStringDescriptorB[SIZE_STRING_USB2POS_INTERFACE] =
{
	SIZE_STRING_USB2POS_INTERFACE,
	0x03,

	UNI('H'), UNI('I'), UNI('D'), UNI(' '),	UNI('P'), UNI('O'), UNI('S'),
};

//PC,JAP
const USB_DEVICE_DESCRIPTOR DeviceDescrAPCJap =
{
	0x12, 				//bLength = 12H
	0x01, 				//bDescriptorType= 01H
//	0x00,0x02,  			//bcdUSBL= usb 2.0
	0x10,0x01,  			//bcdUSBL= usb 1.1
	0x00,    			//bDeviceClass             //Change for HID
	0x00,                           //bDeviceSubclass
	0x00,				//bDeviceProtocol
	USB_MAX_CTRL_PAYLOAD,   //bMaxPacketSize0 EP0 PACKET SIZE,
	0x36, 0x05,                     //idVendorL
	USB_WORD16(USB_KBD_PC_PRODUCT_ID),  //idProductL
	0x79, 0x00,                     //bcdDeviceL	  //Fylood
	0x01,                           //iManufacturer
	0x02,                           //iProduct
	0x08,	      			//SerialNumber
	0x01				//bNumConfigs
};

const USB_DEVICE_DESCRIPTOR DeviceDescrAMac =
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
	USB_WORD16(USB_KBD_MAC_PRODUCT_ID),  //idProductL
	0x79, 0x00,                     //bcdDeviceL	  //Fylood
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
	0x00,                           //Change for HID
	0x00,
	0x00,
	0x40,
	0x01,
	0x00
};

#define Size_KbdReportDescriptor 65



CUsb2HidKbd::MyConfigDescriptor_t CUsb2HidKbd::m_MyConfigDescriptor =
{
	//Configuration descriptors
	0x09, 		//CbLength
	0x02, 		//CbDescriptorType
	0x49,           //CwTotalLength
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
	0x02,                   //bNumEndpoints
	0x03,   		//bInterfaceClass: HID Class
	0x01,   		//bInterfaceSubclass(boot device)
	0x01,    		//bInterfaceProtocol(kbd: HID P19)
	0x04,    		//iInterface string

	//HID descriptor(HID P58)
	0x09,  			//bLength
	0x21,  			//bDescriptorType
	USB_WORD16(0x0110),     //bcdHID
	0x00,                   //bCountryCode
	0x01,   		//bNumDescriptors
	0x22,   		//bDescriptorType
	Size_KbdReportDescriptor, //wDescriptorLength(low)
	0x00,   		//wDescriptorLength(high)

        //endp 4 interrupt IN
	0x07,  			        //bLenght
	0x05,  				//bDescriptorType
	0x84,				//bEndpointAddress, Endpoint 04 - IN
	 0x03,  				//bmAttributes	  INT
	64,                             //wMaxPacketSize: 64 bytes
	0x00,
	8,  				//bInterval

        //endp 5 interrupt OUT
	0x07,  			        //bLenght
	0x05,  				//bDescriptorType
	0x05,				//bEndpointAddress, Endpoint 05 - OUT
	0x03,  				//bmAttributes	  INT
	64,                             //wMaxPacketSize: 64 bytes
	0x00,
	8, 				//bInterval

        //Interface Descriptor Requirement
	0x09,  			//bLength
	0x04,  			//bDescriptorType
	0x01,			//bInterfaceNumber
	0x00,			//bAlternateSetting
	0x02,                   //bNumEndpoints
	0x03,   		//bInterfaceClass: HID Class
	0x00,   		//bInterfaceSubclass(no boot device)
	0x00,    		//bInterfaceProtocol
	0x05,   		//iInterface string

	//HID descriptor(HID P58)
	0x09,  			//bLength
	0x21,  			//bDescriptorType
	USB_WORD16(0x0110),     //bcdHID
	0x00,                   //bCountryCode
	0x01,   		//bNumDescriptors
	0x22,   		//bDescriptorType
	0x90,    		//wDescriptorLength(low)
	0x00,   		//wDescriptorLength(high)

        //endp 2 interrupt IN
	0x07,  			        //bLenght
	0x05,  				//bDescriptorType
	0x82,				//bEndpointAddress, Endpoint 02 - IN
	0x03,  				//bmAttributes	  INT
	64,                             //wMaxPacketSize: 64 bytes
	0x00,
	8,  				//bInterval

        //endp 7 interrupt OUT
	0x07,  			        //bLenght
	0x05,  				//bDescriptorType
	0x07,				//bEndpointAddress, Endpoint 07 - OUT
	0x03,  				//bmAttributes	  INT
	64,                             //wMaxPacketSize: 64 bytes
	0x00,
	1  				//bInterval

	//CUsbRem::m_IfaceDescriptor  // NOTE:  there's a problem with the toolchain & it places all 0s here--we have to set this at runtime
};


const UCHAR usb_hid_descrAKbd[] =
{
	//HID descriptor(HID P58)
	0x09,  			//bLength
	0x21,  			//bDescriptorType
	USB_WORD16(0x0110),     //bcdHID
	0x00,                   //bCountryCode
	0x01,   		//bNumDescriptors
	0x22,   		//bDescriptorType
	Size_KbdReportDescriptor, //wDescriptorLength(low)
	0x00,   		//wDescriptorLength(high)
};

const UCHAR usb_hid_descrBPCMacJap[] =
{
	//HID descriptor(HID P58)
	0x09,  			//bLength
	0x21,  			//bDescriptorType
	USB_WORD16(0x0110),     //bcdHID
	0x00,                   //bCountryCode
	0x01,   		//bNumDescriptors
	0x22,   		//bDescriptorType
	0x90,	                //wDescriptorLength(low)
	0x00,   		//wDescriptorLength(high)
};

const UCHAR usb_report_descrAKbd[Size_KbdReportDescriptor] =
{
	//Report descriptor
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    0x95, 0x05,                    //   REPORT_COUNT (5)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x05, 0x08,                    //   USAGE_PAGE (LEDs)
    0x19, 0x01,                    //   USAGE_MINIMUM (Num Lock)
    0x29, 0x05,                    //   USAGE_MAXIMUM (Kana)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x03,                    //   REPORT_SIZE (3)
    0x91, 0x03,                    //   OUTPUT (Cnst,Var,Abs)
    0x95, 0x06,                    //   REPORT_COUNT (6)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xFF,0x00,               //   LOGICAL_MAXIMUM (255)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
    0x2A, 0xFF,0x00,               //   USAGE_MAXIMUM (Max Keyboard usage)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0xc0                           // END_COLLECTION
};

const UCHAR usb_report_descrBPCMacJap[] =
{
	0x05, 0x8c,                    // USAGE_PAGE (Barcode Page (POS))
	0x09, 0x02,                    // USAGE (Bar Code Scanner)
	0xa1, 0x01,                    // COLLECTION (Application)
	0x09, 0x12,                    // USAGE (Scanned Data Report)
	0xa1, 0x02,                    // COLLECTION (Logical)
	0x85, 0x02,                    // REPORT_ID (2)
	0x15, 0x00,                    // LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,              // LOGICAL_MAXIMUM (255)
	0x75, 0x08,                    // REPORT_SIZE (8)
	0x95, 0x01,                    // REPORT_COUNT (1)
	0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
	0x09, 0x3b,                    // USAGE (Byte Count)
	0x81, 0x02,                    // INPUT (Data,Var,Abs)
	0x95, 0x03,                    // REPORT_COUNT (3)
	0x05, 0x8c,                    // USAGE_PAGE (Barcode Page (POS))
	0x09, 0xfb,                    // USAGE (Symbology Identifier 1)
	0x09, 0xfc,                    // USAGE (Symbology Identifier 2)
	0x09, 0xfd,                    // USAGE (Symbology Identifier 3)
	0x81, 0x02,                    // INPUT (Data,Var,Abs)
	0x95, 0x38,                    // REPORT_COUNT (56)
	0x09, 0xfe,                    // USAGE (Decoded Data)
	0x82, 0x02, 0x01,              // INPUT (Data,Var,Abs,Buf)
	0x06, 0x66, 0xff,              // USAGE_PAGE (Vendor Defined Page (HHP))
	0x95, 0x02,                    // REPORT_COUNT (2)
	0x09, 0x04,                    // USAGE (Code ID)
	0x09, 0x00,                    // USAGE (Undefined)
	0x81, 0x02,                    // INPUT (Data,Var,Abs)
	0x05, 0x8c,                    // USAGE_PAGE (Barcode Page (POS))
	0x25, 0x01,                    // LOGICAL_MAXIMUM (1)
	0x75, 0x01,                    // REPORT_SIZE (1)
	0x95, 0x08,                    // REPORT_COUNT (8)
	0x09, 0xff,                    // USAGE (Decode Data Continued)
	0x81, 0x02,                    // INPUT (Data,Var,Abs)
	0xc0,                          // END_COLLECTION
	0x09, 0x14,                    // USAGE (Trigger Report)
	0xa1, 0x02,                    // COLLECTION (Logical)
	0x85, 0x04,                    // REPORT_ID (4)
	0x15, 0x00,                    // LOGICAL_MINIMUM (0)
	0x25, 0x01,                    // LOGICAL_MAXIMUM (1)
	0x75, 0x01,                    // REPORT_SIZE (1)
	0x95, 0x08,                    // REPORT_COUNT (8)
	0x09, 0x00,                    // USAGE (Undefined)
	0x09, 0x5f,                    // USAGE (Prevent Read of Barcodes)
	0x09, 0x60,                    // USAGE (Initiate Barcode Read)
	0x09, 0x00,                    // USAGE (Undefined)
	0x09, 0x00,                    // USAGE (Undefined)
	0x09, 0x85,                    // USAGE (Sound Error Beep)
	0x09, 0x86,                    // USAGE (Sound Good Read Beep)
	0x91, 0x86,                    // OUTPUT (Data,Var,Rel,Vol)
	0xc0,                          // END_COLLECTION
	0x06, 0x66, 0xff,              // USAGE_PAGE (Vendor Defined Page (HHP))
	0x75, 0x08,                    // REPORT_SIZE (8)
	0x26, 0xff, 0x00,              // LOGICAL_MAXIMUM (255)
	0x09, 0x03,                    // USAGE (Output Redirection
	0xa1, 0x02,                    // COLLECTION (Logical)
	0x85, 0xfe,                    // REPORT_ID (254)
	0x95, 0x01,                    // REPORT_COUNT (1)
	0x09, 0x03,                    // USAGE (Output Redirection)
	0xb2, 0x82, 0x01,              // FEATURE (Data,Var,Abs,Vol,Buf)
	0xc0,                          // END_COLLECTION
	0x09, 0x01,                    // USAGE (Serial Data Stream)
	0xa1, 0x02,                    // COLLECTION (Logical)
	0x85, 0xfd,                    // REPORT_ID (253)
	0x95, 0x01,                    // REPORT_COUNT (1)
	0x09, 0x21,                    // USAGE (Length)
	0x91, 0x02,                    // OUTPUT (Data,Var,Abs)
	0x95, 0x3e,                    // REPORT_COUNT (62)
	0x09, 0x22,                    // USAGE (CP1252 Data)
	0x92, 0x82, 0x01,              // OUTPUT (Data,Var,Abs,Vol,Buf)
	0xc0,                          // END_COLLECTION
	0xc0                           // END_COLLECTION
};

CUsb2HidKbd::CUsb2HidKbd(CRxFifo *pRxFifo, CThreadRealTime *pThread, UCHAR kbdmod)
: CUsb2HidPosHelper(pRxFifo, pThread)
{
	m_pWedge = NULL;
	m_KbdTurboMode = false;
	switch(kbdmod)
	{
	case 124:
		termid_eps = 124;
	break;
	case 125:
		termid_eps = 125;
	break;
	case 134:
		termid_eps = 134;
	break;
	default:
		break;
	}
	rxMaxLenIDs = 64;
	GetInterface(KBD_INTERFACE)->SetIdleSupported(0);

	m_MyConfigDescriptor.RemIfaceDesc = CUsbRem::m_IfaceDescriptor;
	m_MyConfigDescriptor.RemIfaceDesc.IfaceDesc.bInterfaceNumber = GetMaxNumIfaces() - 1; // ReM is the last interface;
	m_MyOtherSpeedConfigDescriptor.RemIfaceDesc = CUsbRem::m_IfaceDescriptor;

	m_MyOtherSpeedConfigDescriptor = m_MyConfigDescriptor;
	m_MyOtherSpeedConfigDescriptor.ConfigDesc.bDescriptorType = USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE;
	m_MyOtherSpeedConfigDescriptor.KeyboardInEndpoint.bInterval = 16;
	m_MyOtherSpeedConfigDescriptor.KeyboardOutEndpoint.bInterval = 16;
	m_MyOtherSpeedConfigDescriptor.HidPosInEndpoint.bInterval = 16;
	m_MyOtherSpeedConfigDescriptor.HidPosOutEndpoint.bInterval = 16;
	InitUSBEndpDes(kbdmod);
}

CUsb2HidKbd::~CUsb2HidKbd(void)
{
	//disable endpoints
	MX25USB2_eps_dis(2);
	MX25USB2_eps_dis(4);
	MX25USB2_eps_dis(5);
	MX25USB2_eps_dis(7);
}

void CUsb2HidKbd::SetPollingInterval(UCHAR kbdmod, bool kbdTurboMode)
{
	if((kbdmod == 124) || (kbdmod == 125))
	{
		//ewr-04771, Turbo mode.
		if(kbdTurboMode)
		{
			m_MyConfigDescriptor.KeyboardInEndpoint.bInterval = 1;
		}
		else
		{
			m_MyConfigDescriptor.KeyboardInEndpoint.bInterval = 8;
		}
	}
}

void CUsb2HidKbd::InitUSBEndpDes(UCHAR kbdmod)
{
	if((kbdmod == 124) || (kbdmod == 134))
	{
		m_bIsMacKeyboard = false;
		AttachDevDescriptors(&DeviceDescrAPCJap, &HS_Device_QualifierA);
	}
	else
	{
		m_bIsMacKeyboard = true;
		AttachDevDescriptors(&DeviceDescrAMac, &HS_Device_QualifierA);
	}


	AttachConDescriptors((UCHAR *)&m_MyConfigDescriptor, 0);
	AttachConDescriptors((UCHAR *)&m_MyConfigDescriptor, 1);
	usb_descrLEN_FS = sizeof(m_MyConfigDescriptor);
	usb_descrLEN_HS = sizeof(m_MyConfigDescriptor);

	AttachConDescriptors((UCHAR *)&m_MyOtherSpeedConfigDescriptor, 2);
	usb_othersp_descrLEN = sizeof(m_MyOtherSpeedConfigDescriptor);

	m_HidDescriptors.Add(0, usb_hid_descrAKbd, sizeof usb_hid_descrAKbd);
	m_ReportDescriptors.Add(0, usb_report_descrAKbd, sizeof usb_report_descrAKbd);

	m_HidDescriptors.Add(1, usb_hid_descrBPCMacJap, sizeof usb_hid_descrBPCMacJap);
	m_ReportDescriptors.Add(1, usb_report_descrBPCMacJap, sizeof usb_report_descrBPCMacJap);

	AttachStringDescriptors(pxInterfaceStringDescriptorA, 0);
	AttachStringDescriptors(pxInterfaceStringDescriptorB, 1);
	pxInterfaceStrDesLEN[0] = sizeof(pxInterfaceStringDescriptorA);
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
MatrixError_t CUsb2HidKbd::ExtractParameters(const tichar_t *pParameters)
{
	MatrixError_t RetVal = CUsb2HidPosHelper::ExtractParameters(pParameters ); // make sure all classes get a chance to extract params
	PatchDeviceDesc((USB_DEVICE_DESCRIPTOR *)DeviceDescr); // TODO:  We call this in every class--maybe we can call it once in usb2base.  Also, we should remove the const in all Device Descriptor arrays and in DeviceDesc pointer as it's obvious they're not const.
	bool SaveKbdTurboMode = m_KbdTurboMode;

	if ( IsOK(RetVal) || (RetVal == ERR_NEED_RECONNECT) )
	{
		CParseHelper::Extract(pParameters, _TI("EciOut"), m_OutputFormat);
		CParseHelper::Extract(pParameters, _TI("Turbo"), m_KbdTurboMode);
	}

	if(SaveKbdTurboMode != m_KbdTurboMode)
	{
		RetVal = ERR_NEED_RECONNECT; 	         // and ensure we simulate a unplug/replug situation
	}

	return RetVal;
}




UCHAR CUsb2HidKbd::GetPidOffset(bool bIsRem)
{
	UCHAR PidOffset;

	if ( m_bIsMacKeyboard )
	{
		if ( bIsRem ) PidOffset = USB_KBD_MAC_REM_PID_OFFSET;
		else PidOffset = USB_KBD_MAC_PID_OFFSET;
	}
	else
	{
		if ( bIsRem ) PidOffset = USB_KBD_PC_REM_PID_OFFSET;
		else PidOffset = USB_KBD_PC_PID_OFFSET;
	}
	return PidOffset;
}


inline usb_err_t CUsb2HidKbd::ReportHandler(void)
{
	usb_err_t RetVal=RET_STALL;

	USHORT nInterface= ControlData.DeviceRequest.wIndex;
	switch (nInterface)
	{
	case KBD_INTERFACE:
		{
			const THidReportTable<CUsb2HidKbd> *pKbdTable = ms_KbdReports;
			RetVal = FindAndCallReportHandler (pKbdTable);
		}
		break;
	case POS_INTERFACE:
		{
			const THidReportTable<CUsb2HidPosHelper> *pPosTable = ms_PosReports;
			RetVal = FindAndCallReportHandler (pPosTable);
		}
		break;
	default:
		break;
	}

	return RetVal;
}

int CUsb2HidKbd::GetReportSize(DEVICE_REQUEST * /*pRequest*/)
{
	int RetVal=-1;

	USHORT nInterface= ControlData.DeviceRequest.wIndex;
	switch (nInterface)
	{
	case KBD_INTERFACE:
		{
			const THidReportTable<CUsb2HidKbd> *pKbdTable = ms_KbdReports;
			RetVal = CUsb2Hid::GetReportSize (pKbdTable);
		}
		break;
	case POS_INTERFACE:
		{
			const THidReportTable<CUsb2HidPosHelper> *pPosTable = ms_PosReports;
			RetVal = CUsb2Hid::GetReportSize (pPosTable);
		}
		break;
	default:
		break;
	}

	return RetVal;
}

/* This table lists all known HID reports */
const THidReportTable<CUsb2HidKbd> CUsb2HidKbd::ms_KbdReports[] =
{
	USB_HID_REPORT_IN,		0,		8,		&CUsb2HidKbd::EP0_InReportHandler,
	USB_HID_REPORT_OUT,		0,		1,		&CUsb2HidKbd::EP0_OutReportHandler,
	USB_HID_REPORT_TABLE_END,	0,		0,		NULL
};

/* Report handler for all known HIDPOS reports */
usb_err_t CUsb2HidKbd::EP0_InReportHandler(void)
{
	KbdInReportBuffer_t *pReport = (KbdInReportBuffer_t*)dataToEP;
	// send an "all keys up" here
	pReport->Clear();
	code_transmit((UCHAR *)dataToEP, pReport->GetBufferSize());

	return RET_ACK;
}

// This function is used if there is no out endpoint defined of by Win98
usb_err_t CUsb2HidKbd::EP0_OutReportHandler(void)
{
	memcpy((UCHAR *)dataForHandler, (UCHAR *)ControlData.dataBuffer, ControlData.wCount);
	const KbdOutReportBuffer_t *pReport = (const KbdOutReportBuffer_t*) dataForHandler;
	StoreLEDInfo(pReport->GetLedInfo());

	return RET_ACK;
}

void CUsb2HidKbd::ReceiveKbdData(UINT pEp)
{
	if(m_datakbdBufCtrl)
	{
		m_datakbdBufCtrl = false;
		datalenEP = MX25USB2_ReadEndpoint_Data(pEp, rxMaxLenIDs, false, datakbdFromEPsec);  //endpoint 5

		if(datalenEP > 0)
		{
			const KbdOutReportBuffer_t *pReport = (const KbdOutReportBuffer_t*) datakbdFromEP;
			StoreLEDInfo(pReport->GetLedInfo());
		}
 	}
	else
	{
		m_datakbdBufCtrl = true;
		datalenEP = MX25USB2_ReadEndpoint_Data(pEp, rxMaxLenIDs, false, datakbdFromEP);  //endpoint 7
		if(datalenEP > 0)
		{
			const KbdOutReportBuffer_t *pReport = (const KbdOutReportBuffer_t*) datakbdFromEPsec;
			StoreLEDInfo(pReport->GetLedInfo());
		}
	}
}

inline usb_err_t CUsb2HidKbd::Ep0DataHandler(void)
{
	return ReportHandler();
}

// send the current keyboard report to the host and wait for done if bWait is set.
// Note from Dieter Fauth: The delays below were required in early days of Matrix devices.
// Later (~2006) the SendReportWithTest was changed to wait until the report is gone.
// So in Matrix we do not need the delay anymore. Not sure about gen6.
void CUsb2HidKbd::SendKbdReport(bool bWait)
{
	SendReportWithTest(KBD_INTERFACE, HID_KEYBOARD_REPORT_SIZE, (UCHAR *)dataToEP);
	if( bWait )
	{
		//ewr-04771 Turbo remove MT_Delay(4) ,speed should same with MS4920
		if(!m_KbdTurboMode)
		{
			MT_Delay(4);	         // this improves the interX delays. Host polls every 8mSec.
		}
		// Without that delay above, the interchar delay would not work as expected. We would get the
		// sequence (in mSec) 8,16,8,16.. if we program the device for 10mSec delay.
	}
}

KbdInReportBuffer_t *CUsb2HidKbd::GetWritableKbdReportBuffer(void)
{
	return (KbdInReportBuffer_t*) dataToEP;
}

void CUsb2HidKbd::SendAllKeysUp(void)
{
	KbdInReportBuffer_t *pReport = GetWritableKbdReportBuffer();
	pReport->Clear();
	SendKbdReport();
}

void CUsb2HidKbd::StoreLEDInfo(UCHAR ucLED)
{
	if(m_pWedge != NULL)
	{
		// decode the LED status
		m_pWedge->WdgStoreCapsLock		(ucLED & bHIDCapsLock	? true : false);
		m_pWedge->WdgStoreNumLock		(ucLED & bHIDNumLock		? true : false);
		m_pWedge->WdgStoreScrollLock	(ucLED & bHIDScrollLock	? true : false);
		m_pWedge->WdgStoreLED_Valid	(true);
	}
}

int CUsb2HidKbd::SendLabel(CBarcodeData *pLabel)  // to HidPos
{
	if (!pLabel)
		return 0;

	theTransCode.AutoTranslate(pLabel, m_OutputFormat);
	CUsb2HidPosHelper::HidPosSendLabel(POS_INTERFACE, pLabel);

	return 0;
}

int CUsb2HidKbd::SendLabelToKeyboard(CBarcodeData *pLabel)  // to keyboard
{
	const wchar_t  * pDataToReSend;
	size_t iSizeDataToReSend = 0;
	int ret = -1;

	pLabel->GetTotalData(pDataToReSend, iSizeDataToReSend);

	theTransCode.AutoTranslate(pLabel, WEDGE_DATATYPE);
	ret =m_pWedge->SendLabel();

	return ret;
}


#ifdef USB_DEBUG_KBD
#include "DebugTrace.h"


void CUsb2HidKbd::SendTestReport(int nInterface, size_t size)
{
/*
	For debug
*/
}
#endif



