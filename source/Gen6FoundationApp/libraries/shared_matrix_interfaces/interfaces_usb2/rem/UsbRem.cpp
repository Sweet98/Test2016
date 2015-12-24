/*===================================================================
  Holds all the descriptors for the ReM interface.  Since ReM can
  come and go with REMIFC Menu Setting, we put the interface
  descriptor at the end of all other interfaces' Config Descritors.
  This necessitate adjusting their Config Descriptor's length at
  runtime. 

  See drvUsbRem.cpp for details on how things interact with the system.
=====================================================================
   $Source: interfaces_usb2/rem/UsbRem.cpp $
   $Date: 2011/04/18 15:59:40EDT $
   $Revision: 1.4 $


===================================================================*/
#include <assert.h>
#include "UsbRem.h"
#include "usb2.h"
#include "usb2pos.h"
#include "usb2hid.h"


#define HID_REPORT_TYPE_INPUT	0x01

// The Report Descriptor is a subset of HID POS.
// Note:  if we ever need to shorten this, we could simply say the "Scanned Data Report" only consists of a 64 B array (don't describe the various bytes & bits in the data).
const unsigned char CUsbRem::m_ReportDescriptor[] = 
{
    0x06, 0x8c, 0xff,              // USAGE_PAGE (Vendor Specific Page (REM))
    0x09, 0x02,                    // USAGE (Bar Code Scanner)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x12,                    //   USAGE (Scanned Data Report)
    0xa1, 0x02,                    //   COLLECTION (Logical)
    0x85, 0x02,                    //     REPORT_ID (2)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //     LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x3b,                    //     USAGE (Byte Count)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x05, 0x8c,                    //     USAGE_PAGE (Barcode Page (POS))
    0x09, 0xfb,                    //     USAGE (Symbology Identifier 1)
    0x09, 0xfc,                    //     USAGE (Symbology Identifier 2)
    0x09, 0xfd,                    //     USAGE (Symbology Identifier 3)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x95, 0x38,                    //     REPORT_COUNT (56)
    0x09, 0xfe,                    //     USAGE (Decoded Data)
    0x82, 0x02, 0x01,              //     INPUT (Data,Var,Abs,Buf)
    0x06, 0x66, 0xff,              //     USAGE_PAGE (Vendor Defined Page (HHP))
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x09, 0x04,                    //     USAGE (Code ID)
    0x09, 0x00,                    //     USAGE (Undefined)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x05, 0x8c,                    //     USAGE_PAGE (Barcode Page (POS))
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x95, 0x08,                    //     REPORT_COUNT (8)
    0x09, 0xff,                    //     USAGE (Decode Data Continued)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xc0,                          //   END_COLLECTION
    
   0x06, 0x66, 0xff,              //   USAGE_PAGE (Vendor Defined Page (HHP))
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
/* we don't need this part of HID POS
    0x09, 0x03,                    //   USAGE (Output Redirection)
    0xa1, 0x02,                    //   COLLECTION (Logical)
    0x85, 0xfe,                    //     REPORT_ID (254)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x09, 0x03,                    //     USAGE (Output Redirection)
    0xb2, 0x82, 0x01,              //     FEATURE (Data,Var,Abs,Vol,Buf)
    0xc0,                          //   END_COLLECTION
*/
    0x09, 0x01,                    //   USAGE (Serial Data Stream)
    0xa1, 0x02,                    //   COLLECTION (Logical)
    0x85, 0xfd,                    //     REPORT_ID (253)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x09, 0x21,                    //     USAGE (Length)
    0x91, 0x02,                    //     OUTPUT (Data,Var,Abs)
    0x95, 0x3e,                    //     REPORT_COUNT (62)
    0x09, 0x22,                    //     USAGE (CP1252 Data)
    0x92, 0x82, 0x01,              //     OUTPUT (Data,Var,Abs,Vol,Buf)
    0xc0,                          //   END_COLLECTION
    0xc0                           // END_COLLECTION
};



const HIDDescriptor1_t CUsbRem::m_HidDescriptor = 
{
	0x09,  //bLength
	USB_HID_DESCR,  //bDescriptorType
	USB_WORD16(0x0110),     //bcdHID, NOTE:  this is version 1.10 and 1.11 is out.
	0x00,  //bCountryCode
	0x01, //bNumDescriptors
	USB_REPORT_DESCR, //bDescriptorType
	sizeof m_ReportDescriptor, //wDescriptorLength(low)
	0x00, //wDescriptorLength(high)
};


// This needs to be the last interface.
CUsbRem::IfaceDescriptor_t CUsbRem::m_IfaceDescriptor = 
{
	// IfaceDescr
	0x09,  //bLength
	0x04,  //bDescriptorType
	0x00,  //bInterfaceNumber--this will change depending on which TERMID were at.
	0x00,  //bAlternateSetting
	0x02,  //bNumEndpoints
	USB_HID_CLASS,  //bInterfaceClass: HID Class
	USB_NOSUBCLASS,  //bInterfaceSubclass: no subclass
	USB_NOPROTOCOL,  //bInterfaceProtocol:  none
	REM_INTERFACE_IDX,  //iInterface
	
	// HID descriptor.
	m_HidDescriptor, // Note:  this gets populated with 0s for some reason, so we explicitly set it at runtime
	
	// Endpoint descriptors for HID.  Per USB HID spec version 1.11, section 7.1, IN endpoint first, OUT next
	// Endpoint IN
	0x07, //bLength
	0x05, //bDescriptorType
	EP_IN(MyInEndpoint), //bEndpointAddress
	0x03, //bmAttributes	  INT
	64,   //wMaxPacketSize: 64 bytes
	0x00,
	1,    //bInterval
	
	// Endpoint OUT
	0x07, //bLength
	0x05, //bDescriptorType
	EP_OUT(MyOutEndpoint), //bEndpointAddress
	0x03, //bmAttributes	  INT
	64,   //wMaxPacketSize: 64 bytes
	0x00,
	3     //bInterval
};

#define SIZE_STRING_REM_INTERFACE (1+1+(3*2))
const CUsbRem::IfaceStrDescriptor_t CUsbRem::m_RemInterfaceStrDesc =
{
	sizeof(CUsbRem::IfaceStrDescriptor_t),
	0x03,
	UNI('R'), UNI('E'), UNI('M')
};




/*----------------------------------------------------------------------------------------
Init

  The toolchain wouldn't populate m_IfaceDescriptor struct correctly (it put 0s), so we
  fix it here.  Because of the way each class is dependent (inherited from) on one another,
  there were compile issues using a constructor, so we use this static method instead.

  Parameters:  none

  Returns:  nothing.
----------------------------------------------------------------------------------------*/
void CUsbRem::Init(void)
{
	m_IfaceDescriptor.HidDesc = m_HidDescriptor;
}



/*----------------------------------------------------------------------------------------
SendData

  Sends data out the ReM interface/endpoint.

  Parameters:  
  			pUsb2Chip[in]:  access to CUsb2Chip so we can write to the IN endpoint

  			pData[in]:  data to send
  			
  			length[in]:  length of data to send

  Returns:  nothing.
----------------------------------------------------------------------------------------*/
void CUsbRem::SendData(CUsb2Chip * pUsb2Chip, const unsigned char * pData, int length)
{
		UsbPosInReportBuffer_t HidPosReport;
		int NumToSend;
	

		while ( length )
		{
			assert( length >= 0 );
			NumToSend = length;
			HidPosReport.Clear();
			HidPosReport.SetReportId(HID_USBPOS_IN_REPORT_ID);
			if ( NumToSend > HID_USBPOS_LABEL_SIZE)
			{	// fill the "more data to follow" flag
				NumToSend = HID_USBPOS_LABEL_SIZE;
				HidPosReport.SetPadding(1);  // This indicates "more data to follow"
			}
			// fill size info
			HidPosReport.SetSize(NumToSend);
			// don't bother to fill barcode info
			// now copy the data
			memcpy(HidPosReport.GetContentBuffer(), pData, NumToSend);
			pUsb2Chip->MX25USB2_WriteEndpoint(MyInEndpoint, HID_USBPOS_IN_REPORT_SIZE, (const unsigned char *) &HidPosReport);

			pData += NumToSend;
			length -= NumToSend;
		}
}



/*----------------------------------------------------------------------------------------
HandleRxData

  Handles incoming data from the ReM OUT endpoint.  Technically, we don't need the length
  parameter (it's always 64 bytes), but it comes in handy for debugging.

  Parameters:  
  			pBuffer[in]:  buffer of data read
  			
  			length[in]:  length of data read

  			pRxFifo[in]:  pointer to CDriverUsbRem's RX fifo

  Returns:  nothing.
----------------------------------------------------------------------------------------*/
void CUsbRem::HandleRxData(const unsigned char * pBuffer, int length, CRxFifo * pRxFifo)
{
	UsbCommonHidReportBuffer_t * pReport = (UsbCommonHidReportBuffer_t *) pBuffer;
	if ( pReport->GetReportId() == HID_POS_OUT_1252_REPORT_ID )
	{	// we only handle this report--ignore all others
		pRxFifo->Write(pReport->GetContentBuffer(), pReport->GetContentLength());
	}
}


/*----------------------------------------------------------------------------------------
OnClassRequest

  Handles incoming Setup packets on the control endpoint.  Currently this handles none, so
  all incoming Setup packets will get STALLed.
  
  Note:  per HID spec (section 7.2 of HID 1.1), we are supposed to handle GET_REPORTs 
  (and we can stall everything else).  The only report that makes sense to send is an IN
  report (i.e. data).  Since no one is supposed to be using the control endpoint for doing
  transfers (ReM uses 2 interrupt endpoints for data transfer), just send a report with
  no data.

  Parameters:  
  			pDevReq[in]:  pointer to incoming device request

  			pPacketToSend[out]:  pointer to a buffer to hold a HID packet

  			pNumToSend[out]:  how much data to send

  			pbSendPacket[out]:  pointer to tell the caller to send a HID packet

  Returns:  true if handled, false otherwise (caller will send STALL response).
----------------------------------------------------------------------------------------*/
bool CUsbRem::OnClassRequest(const volatile DEVICE_REQUEST * pDevReq, unsigned char * pPacketToSend, int * pNumToSend, bool * pbSendPacket)
{
	*pbSendPacket = false;
	if ( (pDevReq->bmRequestType == USB_GET_CLASS_INTERFACE ) && (pDevReq->bRequest == USB_HID_GET_REPORT) )
	{	// we know wIndex means this interface, so we know this is a HID GET_REPORT for ReM.  The only report we understand is an Output report.
		unsigned char ReportType  = pDevReq->wValue >> 8;
		unsigned char ReportId  = pDevReq->wValue & 0xff;
		if ( (ReportType == HID_REPORT_TYPE_INPUT) && (ReportId == HID_USBPOS_IN_REPORT_ID) )
		{	// It's a request for an output report.  Just send nothing (a response with 0 length).
			UsbPosInReportBuffer_t HidPosReport;
			HidPosReport.Clear();
			HidPosReport.SetReportId(HID_USBPOS_IN_REPORT_ID);
			HidPosReport.SetSize(0);
			int NumToSend = pDevReq->wLength;
			// Just send a small packet (USB spec allows us to send less than requested)--only the 1st two bytes are really necessary to tell the reader there's no data.
			// Note:  I saw an issue if we tried to send a full 64B packet and an application asked for 65 or more bytes--the app hung, expecting more data.
			// A USB trace showed an incomplete transaction--the host was waiting for more data.  We might have an issue with how we terminate control enpoint transfers using
			// code_transmit()--it doesn't transmit a 0 length packet.
			*pNumToSend = 8;  
			if ( NumToSend < 8 ) *pNumToSend = NumToSend;
			memcpy(pPacketToSend, (const unsigned char *) &HidPosReport, 8);
			*pbSendPacket = true;
			return true;
		}
	}
	return false;
}



