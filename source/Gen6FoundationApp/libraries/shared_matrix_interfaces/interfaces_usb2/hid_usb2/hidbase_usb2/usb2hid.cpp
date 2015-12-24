/* ==============================================================================
   USB HID common functions
   ==============================================================================

   $RCSfile: interfaces_usb2/hid_usb2/hidbase_usb2/usb2hid.cpp $
   $Revision: 1.16 $
   $Date: 2011/03/31 14:38:48EDT $

//=================================================================================

//=================================================================================*/
//! \file

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "stdInclude.h"
#include "product.h"
#include "usb2.h"
//#include "usb2desc.h"
#include "usb2enum.h"
#include "usb2chip.h"
#include "usb2hid.h"
#include "UsbRem.h"

#include "timeout.h"


CUsb2Hid::CUsb2Hid(CRxFifo *pRxFifo, CThreadRealTime *pThread, size_t /*InSize*/, size_t /*OutSize*/ )
: CUsb2Enum(pRxFifo, pThread)
{	
	m_ucNumInterfaces = NUM_HID_INTERFACES;	
	m_ucDestinationInterface = 0;
	memset((UCHAR *)dataForHandler, 0, 263);

	for(int i=0; i<NUM_HID_INTERFACES; i++)
	{
		m_pInterface[i] = new CHidInterface();
	}
	m_ScannerStatus.ScannerDisabled = false;
}

CUsb2Hid::~CUsb2Hid(void)
{
	for(int i=0; i<NUM_HID_INTERFACES; i++)
	{
		delete m_pInterface[i];
	}
}

// A generic SendReport function with ack 
usb_err_t CUsb2Hid::SendReportWithAck(UCHAR EpNum, int size, UCHAR *buffer)
{
	usb_err_t RetVal;

	RetVal = MX25USB2_WriteEndpoint(EpNum, size, buffer);	
		
	return RetVal;
}

// Send the current barcode to the host
void CUsb2Hid::SendReportWithTest(int nInterface, int size, UCHAR *buffer)
{
	UCHAR EpNum;
		
	if((termid_eps == 124) || (termid_eps == 125) || (termid_eps == 134))
	{
		if(nInterface == 1)  
			EpNum = 2;
		else //if(nInterface == 0)
			EpNum = 4;		
	}
	else
	{
		if(nInterface == 1)  //For surePos Flash interface
			EpNum = 4;
		else //if(nInterface == 0)
			EpNum = 2;
	}
	
	SendReportWithAck(EpNum, size, buffer);

	//Timeout
//	TimeoutHack(EpNum, 20, true);	//endpoint EpNum, 20s timeout		
}


void CUsb2Hid::SendToRem(const unsigned char * pData, int length)
{
	CUsbRem rem;  // there's no stack space used by this object.
	rem.SendData(this, pData, length);
}


bool CUsb2Hid::IsRemTxComplete(void)
{
	bool bIsDone = true;
	if ( m_bRemIfaceEnabled ) bIsDone = ! EpHasData(CUsbRem::MyInEndpoint);
	return bIsDone;
}



void CUsb2Hid::SetIdle(void)
{	
	UCHAR bDuration = MSB(ControlData.DeviceRequest.wValue);     //High;
	UCHAR bReportID = ControlData.DeviceRequest.wValue & 0xff;   //Low;
	USHORT nInterface= ControlData.DeviceRequest.wIndex; 
		
	if ( (nInterface < m_ucNumInterfaces) )
	{
		if(GetInterface(nInterface)->SetIdle(bReportID, bDuration) == RET_ACK)
		{
			StatusAcknowledge(1);	
		}	
		else
			stall_ep0();
	}
	else
		stall_ep0();
}

void CUsb2Hid::GetIdle(void)
{
	UCHAR bReportID = ControlData.DeviceRequest.wValue & 0xff;
	USHORT nInterface= ControlData.DeviceRequest.wIndex;

	if ( (nInterface < m_ucNumInterfaces) && (ControlData.DeviceRequest.wLength==1) )
	{	
		if ( GetInterface(nInterface)->IsIdleSupported(bReportID) )
		{			
			code_transmit(GetInterface(nInterface)->GetIdleAddr(bReportID), 1);    
		}
		else	
			stall_ep0();   //add for pass HID Class test
	}
	else
		stall_ep0();
}

void CUsb2Hid::SetProtocol(void)
{
	USHORT nInterface= ControlData.DeviceRequest.wIndex;
	
	if (nInterface < m_ucNumInterfaces)
	{
		// we currently ignore the protocol (we have no difference between boot and report)
		GetInterface(nInterface)->SetProtocol(ControlData.DeviceRequest.wValue & 0xff);
			
		StatusAcknowledge(1);			
	}
	else
		stall_ep0();
}

void CUsb2Hid::GetProtocol(void)
{
	USHORT nInterface= ControlData.DeviceRequest.wIndex;

	if ((nInterface < m_ucNumInterfaces) && (ControlData.DeviceRequest.wLength==1))
	{
		code_transmit(GetInterface(nInterface)->GetProtocolAddr(), 1);    
	}
	else
		stall_ep0();
}

void CUsb2Hid::SetReport(void)
{
	// find and call the approriate handler
	int size = GetReportSize((DEVICE_REQUEST *)&ControlData.DeviceRequest);

	if(size > 0)
	{
	}
	else 
		stall_ep0();	//?
}

// find and call the approriate handler
usb_err_t CUsb2Hid::GetReport(void)
{
	// find and call the approriate handler
	return ReportHandler();
}


/*----------------------------------------------------------------------------------------
OnSetClassInterfaceReq

  Handles the HID USB_SET_CLASS_INTERFACE request from the control endpoint.  Makes use
  of the ControlData member variable.

  Parameters:  none

  Returns:  nothing.
----------------------------------------------------------------------------------------*/
void CUsb2Hid::OnSetClassInterfaceReq(void)
{
	switch(ControlData.DeviceRequest.bRequest)
	{
		//0x09
		case USB_HID_SET_REPORT:
			SetReport();
		break;
		//0xa
		case USB_HID_SET_IDLE:
			SetIdle();	

			if(m_ConTimes == 0)
			{
				m_dataBufCtrl = true;						
				MX25USB2_ReadEndpoint(7, rxMaxLenIDs, false, dataFromEP);  //Rx data from endpoint 7
				if((termid_eps == 124) || (termid_eps == 125) || (termid_eps == 134))
				{
					m_datakbdBufCtrl = true;
					MX25USB2_ReadEndpoint(5, rxMaxLenIDs, false, datakbdFromEP);  //Rx data from endpoint 5
				}
			}
			
			m_ConTimes += 1;
							
		break;
		//0xb
		case USB_HID_SET_PROTOCOL:				
			SetProtocol();
		break;

		default:
			stall_ep0();
		break;
	}
}


/*----------------------------------------------------------------------------------------
OnGetClassInterfaceReq

  Handles the HID USB_GET_CLASS_INTERFACE request from the control endpoint.  Makes use
  of the ControlData member variable.

  Parameters:  none

  Returns:  nothing.
----------------------------------------------------------------------------------------*/
void CUsb2Hid::OnGetClassInterfaceReq(void)
{
	switch(ControlData.DeviceRequest.bRequest)
	{
		case USB_HID_GET_REPORT:				
			GetReport();
		break;
		case USB_HID_GET_IDLE:				
			GetIdle();
		break;
		case USB_HID_GET_PROTOCOL:				
			GetProtocol();
		break;

		default:
			stall_ep0();
		break;
	}
}



// this gets called by the USB framework for any class request we did receive on EP0
void CUsb2Hid::OnClassRequest(void)
{
	switch(ControlData.DeviceRequest.bmRequestType)
	{				
		//0x21
		case USB_SET_CLASS_INTERFACE:
			OnSetClassInterfaceReq();
		break;

		case USB_GET_CLASS_INTERFACE:
			OnGetClassInterfaceReq();
		break;

		default:
			stall_ep0();
		break;
	}
}

// Return true if the scanner should be disabled
// Return false if the scanner should be enabled
bool CUsb2Hid::GetScannerDisableStatus(void)
{
	if( m_ScannerStatus.ScannerDisabled )
	{
		return true;
	}
	else
	{
		return false;
	}
}





