/* ==============================================================================
   USB HID for OEM POS Module Source
   ==============================================================================

   $RCSfile: interfaces_usb2/hid_usb2/pos_usb2/usb2hidpos.cpp $
   $Revision: 1.17 $
   $Date: 2011/03/31 14:38:47EDT $

//=================================================================================

//=================================================================================*/

#include "stdInclude.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "product.h"
#include "usb2.h"
#include "usb2hid.h"
#include "usb2hidpos.h"
#include "UsbRem.h"

#include "ParseHelper.h"

#define SIZE_STRING_USB2POS_INTERFACE (1+1+(7*2))
const UCHAR pxInterfaceStringDescriptorA[SIZE_STRING_USB2POS_INTERFACE] = 
{
	SIZE_STRING_USB2POS_INTERFACE,
	0x03,
	
	UNI('H'), UNI('I'), UNI('D'), UNI(' '),	UNI('P'), UNI('O'), UNI('S'),
};

const USB_DEVICE_DESCRIPTOR DeviceDescrA =
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
	USB_WORD16(USB_POS_PRODUCT_ID), //idProductL
	0x79, 0x00,                     //bcdDeviceL						
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

CUsb2HidPos::MyConfigDescriptor_t CUsb2HidPos::m_MyConfigDescriptor =
{
	//Configuration descriptors	
	{
		0x09, 		//CbLength
		0x02, 		//CbDescriptorType
		0x29,           //CwTotalLength                                     
		0x00,		
		0x01,       	//CbNumInterfaces				      
		0x01,		//CbConfigurationValue			 		
		0x03,          	//CiConfiguration		                      
		USB_BM_ATTRIBUTES,     	//CbmAttributes Bus powered + Remote Wakeup				
		USB_POWER/2 //CMaxPower
	}, 		

        //Interface Descriptor Requirement
	{
		0x09,  			//bLength
		0x04,  			//bDescriptorType
		0x00,			//bInterfaceNumber	                      				
		0x00,			//bAlternateSetting					
		0x02,                   //bNumEndpoints					
		0x03,   		//bInterfaceClass: HID Class	              
		0x00,   		//bInterfaceSubclass(boot device)    
		0x00,    		//bInterfaceProtocol(kbd: HID P19)
		0x04    		//iInterface
	},				      

	//HID descriptor(HID P58)
	{
		0x09,  			//bLength
		0x21,  			//bDescriptorType
		USB_WORD16(0x0110),     //bcdHID
		0x00,                   //bCountryCode
		0x01,   		//bNumDescriptors
		0x22,   		//bDescriptorType
		0x90,    		//wDescriptorLength(low)                        
		0x00   		//wDescriptorLength(high)
	},

        //endp 2 interrupt IN
	{
		0x07,  			        //bLenght
		0x05,  				//bDescriptorType
		0x82,				//bEndpointAddress, Endpoint 02 - IN						
		0x03,  				//bmAttributes	  INT
		64,                             //wMaxPacketSize: 64 bytes
		0x00,                           
		0x10  				//bInterval	
	},

        //endp 7 interrupt OUT
	{
		0x07,  			        //bLenght
		0x05,  				//bDescriptorType
		0x07,				//bEndpointAddress, Endpoint 07 - OUT						
		0x03,  				//bmAttributes	  INT
		64,                             //wMaxPacketSize: 64 bytes
		0x00,                           
		0x10 				//bInterval
	},
				
	CUsbRem::m_IfaceDescriptor  // NOTE:  there's a problem with the toolchain & it places all 0s here--we have to set this at runtime
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
	0x90,    		//wDescriptorLength(low)                          
	0x00     		//wDescriptorLength(high)
};

const UCHAR usb_report_descrA[] =
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

CUsb2HidPos::CUsb2HidPos(CRxFifo *pRxFifo, CThreadRealTime *pThread)
: CUsb2HidPosHelper(pRxFifo, pThread)
{
	termid_eps = 131; 
	rxMaxLenIDs = 64;
	

	m_MyConfigDescriptor.RemIfaceDesc = CUsbRem::m_IfaceDescriptor;
	m_MyConfigDescriptor.RemIfaceDesc.IfaceDesc.bInterfaceNumber = GetMaxNumIfaces() - 1; // ReM is the last interface;

	m_MyOtherSpeedConfigDescriptor = m_MyConfigDescriptor;
	m_MyOtherSpeedConfigDescriptor.ConfigDesc.bDescriptorType = USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE;

	InitUSBEndpDes();
		
	m_ucNumInterfaces = 1;
}

CUsb2HidPos::~CUsb2HidPos(void)
{
	//disable endpoints
	MX25USB2_eps_dis(2);	
	MX25USB2_eps_dis(7);
}

void CUsb2HidPos::InitUSBEndpDes(void)
{			
	AttachDevDescriptors(&DeviceDescrA, &HS_Device_QualifierA);
	// Setup PID and VID
	
	AttachConDescriptors((UCHAR *)&m_MyConfigDescriptor, 0);
	AttachConDescriptors((UCHAR *)&m_MyConfigDescriptor, 1);	
	AttachConDescriptors((UCHAR *)&m_MyOtherSpeedConfigDescriptor, 2);
	usb_descrLEN_FS = sizeof(m_MyConfigDescriptor);
	usb_descrLEN_HS = sizeof(m_MyConfigDescriptor);
	usb_othersp_descrLEN = sizeof(m_MyOtherSpeedConfigDescriptor);

	m_HidDescriptors.Add(0, usb_hid_descrA, sizeof usb_hid_descrA);
	m_ReportDescriptors.Add(0, usb_report_descrA, sizeof usb_report_descrA);
	
	AttachStringDescriptors(pxInterfaceStringDescriptorA, 0);
	pxInterfaceStrDesLEN[0] = sizeof(pxInterfaceStringDescriptorA);	

	CustomizeUsbStrings();
}

inline usb_err_t CUsb2HidPos::Ep0DataHandler(void)
{
	return ReportHandler();	
}

inline usb_err_t CUsb2HidPos::ReportHandler(void)
{	
	usb_err_t RetVal=RET_STALL;

	USHORT nInterface= ControlData.DeviceRequest.wIndex;
	switch (nInterface)
	{
	case SINGLE_INTERFACE:
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

int CUsb2HidPos::GetReportSize(DEVICE_REQUEST *)
{
	int RetVal=-1;

	USHORT nInterface= ControlData.DeviceRequest.wIndex;
	switch (nInterface)
	{
	case SINGLE_INTERFACE:
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

int CUsb2HidPos::SendLabel(CBarcodeData *pLabel)
{	
	return CUsb2HidPosHelper::HidPosSendLabel(SINGLE_INTERFACE, pLabel);
}



MatrixError_t CUsb2HidPos::ExtractParameters(const tichar_t *pParameters)
{
	MatrixError_t ret = CUsb2Enum::ExtractParameters(pParameters ); // make sure all classes get a chance to extract params
	PatchDeviceDesc((USB_DEVICE_DESCRIPTOR *)DeviceDescr);
	return ret;
}






