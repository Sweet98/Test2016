/*=================================================================================
  Class implementation for a COM port emulation with the CDC ACM.
	(Communication Device Class - Abstract Control Model, see usbcdc11.pdf )

  We're using the new USB 2.0 IAD to avoid an issue with ReM.  Unfortunately, we 
  need to stay at USB 1.1 because marketing doesn't like it if you plug us into a 
  1.1 (full speed) only hub and you get the "this device might perform faster if 
  plugged into a high speed hub" message.  To avoid two config descriptors, we 
  place the IAD at the end, right before the ReM descriptor and thus include it
  even when ReM is not turned on.  If the OS understands it, it shouldn't hurt 
  anything since the data in it is correct.  If the OS doesn't understand it, it 
  should ignore it.
//=================================================================================
   $Source: interfaces_usb2/cdcacm_usb2/usb2cdcacm.cpp $
   $Revision: 1.56 $
   $Date: 2011/10/11 11:11:47EDT $

//=================================================================================

//=================================================================================*/
#include <stdio.h>
#include "stdInclude.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "product.h"
#include "CommonDriver.h"
#include "usb2.h"
#include "Usb2CdcAcm.h"
#include "UsbRem.h"
#include "ParseHelper.h"
#include "timeout.h"
#include "HardwareToken.h"
#include "Menu.h"
#include "ReaderConfiguration.h"

#include "barcodedataman.h"

#include "beep.h"

const int EpCommIn = 4;

/* Line coding - 115200 baud, N-8-1 */
UCHAR pxLineCoding[] = { 0x00, 0xC2, 0x01, 0x00, 0x00, 0x00, 0x08 };

/* Line coding - 921600 baud, N-8-1 */
//static const UCHAR pxLineCoding[] = { 0x00, 0x10, 0x0E, 0x00, 0x00, 0x00, 0x08 };

#define SIZE_STRING_CDCACM_COMM_INTERFACE 1+1+(12*2)
const UCHAR pxInterfaceStringDescriptorA[SIZE_STRING_CDCACM_COMM_INTERFACE] =
{
	SIZE_STRING_CDCACM_COMM_INTERFACE,
	0x03,

	UNI('C'), UNI('D'), UNI('C'), UNI('-'),	UNI('A'), UNI('C'), UNI('M'), UNI(' '),	UNI('C'), UNI('o'),
	UNI('m'), UNI('m'),
};

#define SIZE_STRING_CDCACM_DATA_INTERFACE 1+1+(12*2)
const UCHAR pxInterfaceStringDescriptorB[SIZE_STRING_CDCACM_DATA_INTERFACE] =
{
	SIZE_STRING_CDCACM_DATA_INTERFACE,
	0x03,

	UNI('C'), UNI('D'), UNI('C'), UNI('-'),	UNI('A'), UNI('C'), UNI('M'), UNI(' '),	UNI('D'), UNI('a'),
	UNI('t'), UNI('a'),
};

USB_DEVICE_DESCRIPTOR CUsb2CdcAcm::m_DeviceDescr =
{
	0x12, 				//bLength = 12H
	0x01, 				//bDescriptorType= 01H
	0x10,0x01,  			//bcdUSBL= usb 1.1
	0x02,    			//bDeviceClass
	0x00,                           //bDeviceSubclass
	0x00,				//bDeviceProtocol
	USB_MAX_CTRL_PAYLOAD,    //bMaxPacketSize0 EP0 PACKET SIZE,
	0x36, 0x05,                     //idVendorL
	USB_WORD16(USB_MODEM_ACM_PRODUCT_ID),   //idProductL
	0x10, 0x01,                     //bcdDeviceL
	0x01,                           //iManufacturer
	0x02,                           //iProduct
	0x08,   	      		//SerialNumber
	0x01				//bNumConfigs
};


HS_DEVICE_QUALIFIER CUsb2CdcAcm::m_DeviceQualifier =
{
	0x0a,				// 0AH
	0x06,                           // USB DEVICE QUALIFIER DESCRIPTOR TYPE
	0x10,0x01,			// USB 1.1 version
	0x02,			// bDeviceClass
	0x00,			//bDeviceSubclass
	0x00,			//bDeviceProtocol
	0x40,			//bMaxPacketSize0
	0x01,			//bNumConfigs
	0x00			// bReserved
};


CUsb2CdcAcm::MyConfigDescriptor_t CUsb2CdcAcm::m_MyConfigDescriptor_FS =
{
	/* Configuration 1 descriptor
	Here we define two interfaces (0 and 1) and a total of 3 endpoints.
	Interface 0 is a CDC Abstract Control Model interface with one interrupt-in endpoint.
	Interface 1 is a CDC Data Interface class, with a bulk-in and bulk-out endpoint.
	Endpoint 0 gets used as the CDC management element.
	*/

	0x09,				//CbLength
	0x02,				//CbDescriptorType
	0x00,				//CwTotalLength--will be set by CUsb2Base::AdjustThisConfigDescriptorForRem()
	0x00,
	0x02,				//CbNumInterfaces
	0x01,				//CbConfigurationValue
	0x03,				//CiConfiguration
	USB_BM_ATTRIBUTES,	//CbmAttributes Bus powered + Remote Wakeup
	USB_POWER/2,		//CMaxPower

	// IAD Descriptor
	0x08,  			//bLength
	0x0b,  			//bDescriptorType
	0x00,			//bFirstInterface
	0x02,			//bInterfaceCount
	0x02,           //bFunctionClass (Comm Interface Class)
	0x02,   		//bFunctionSubClass (ACM)
	0x01,   		//bFunctionProtocol (Common AT Commands)
	0x00,    		//iFunction/**/
	
        //Communication Class Interface Descriptor Requirement
	0x09,  			//bLength
	0x04,  			//bDescriptorType
	0x00,			//bInterfaceNumber
	0x00,			//bAlternateSetting
	0x01,                   //bNumEndpoints
	0x02,   		//bInterfaceClass: Comm Interface Class
	0x02,   		//bInterfaceSubclass: Abstract Control Model
	0x01,    		//bInterfaceProtocol(0x01: v25)
	0x04,   		//iInterface

        //Header Functional Descriptor
	0x05,				// bLength
	0x24,				// bDescriptor type: CS_INTERFACE
	0x00,				// bDescriptor subtype: Header Func Desc
	0x10,
	0x01,			        // bcdCDC:1.1


        //ACM Functional Descriptor
	0x04,				// bFunctionLength
	0x24,				// bDescriptor type: CS_INTERFACE
	0x02,				// bDescriptor subtype: ACM Func Desc
	0x06,				// bmCapabilities: Supporting device management of RTS/CTS
        //Union Functional Descriptor
	0x05,				// bFunctionLength
	0x24,				// bDescriptor type: CS_INTERFACE
	0x06,				// bDescriptor subtype: Union Func Desc
	0x00,				// bMasterInterface: CDC Interface
	0x01,				// bSlaveInterface0: Data Class Interface

        //Call Management Functional Descriptor
        //0 in D1 and D0 indicates that device does not handle call management*/
	0x05,				// bFunctionLength
	0x24,				// bDescriptor type: CS_INTERFACE
	0x01,				// bDescriptor subtype: Call Management Func
	0x03, //0x00,				// bmCapabilities: D1 + D0
	0x01,				// bDataInterface: Data Class Interface 1

        //endp 4 interrupt IN
	0x07,  			        //bLength
	0x05,  				//bDescriptorType
	0x84,				//bEndpointAddress, Endpoint 04 - IN
	0x03,  				//bmAttributes	  INT
	64,                 //wMaxPacketSize: 64 bytes
	0x00,
	0x10,  				//bInterval

        //Data Class Interface Descriptor Requirement
	0x09, 			        //bLength
	0x04, 				//bDescriptorType
	0x01,				//bInterfaceNumber
	0x00,				//bAlternateSetting
	0x02,   			//bNumEndPoints
	0x0A,   			//bInterfaceClass
	0x00,   			//bInterfaceSubclass
	0x00,   		        //bInterfaceProtocol
	0x05,   			//iInterface string

        //endp 7 Bulk OUT
	0x07,  			        //bLength
	0x05,  				//bDescriptorType
	0x07,				//bEndPointAddress, Endpoint 07 - OUT
	0x02,  				//bmAttributes BULK
	64,                             //wMaxPacketSize
	0x00,
	0x00,				//bInterval

        //endp 2 Bulk IN
	0x07,  			        //bLength
	0x05,  				//bDescriptorType
	0x82,				//bEndPointAddress, Endpoint 02 - IN
	0x02,  				//bmAttributes BULK
	64,                             //wMaxPacketSize
	0x00,
	0x00,			        //bInterval

	CUsbRem::m_IfaceDescriptor  // NOTE:  there's a problem with the toolchain & it places all 0s here--we have to set this at runtime
};





CUsb2CdcAcm::CUsb2CdcAcm(CRxFifo *pRxFifo, CThreadRealTime *pThread, bool /*UseAlternativPID*/)
: CUsb2Enum(pRxFifo, pThread)
{
	termid_eps = 130;
	rxMaxLenIDs = RXMAXSIZE;

	mMessageLen = 0;
	m_AckNakEnabled = false;
	m_CtsModeEnabled = false;
	m_RtsModeEnabled = false;
	m_DtrStatus = true;	      // for now we always rise DTR, this is what legacy RS232 apps expect

//	m_XonEnabled = false;

	m_WaitingForAckNak = false;
	m_GotNak = false;

	m_ShowSerialNumber = true;

	if ( HardwareToken.IAmOEM() )
	{	// USB 2.0 High Speed
		m_DeviceDescr.bcdUSB0 = 0x00;
		m_DeviceDescr.bcdUSB1 = 0x02;

		m_DeviceQualifier.wVersion0 = 0x00;
		m_DeviceQualifier.wVersion1 = 0x02;
	}
	m_MyConfigDescriptor_FS.RemIfaceDesc = CUsbRem::m_IfaceDescriptor;
	m_MyConfigDescriptor_FS.RemIfaceDesc.IfaceDesc.bInterfaceNumber = GetMaxNumIfaces() - 1; // ReM is the last interface

	m_MyConfigDescriptor_HS = m_MyConfigDescriptor_FS;
	// The High Speed speed config descriptor is set to the Full Speed config descriptor when app is loaded.  We need to adjust a few things.
	m_MyConfigDescriptor_HS.DataOutEndpoint.wMaxPacketSizeLow = 512%0x100;
	m_MyConfigDescriptor_HS.DataOutEndpoint.wMaxPacketSizeHigh = 512/0x100;
	m_MyConfigDescriptor_HS.DataInEndpoint.wMaxPacketSizeLow = 512%0x100;
	m_MyConfigDescriptor_HS.DataInEndpoint.wMaxPacketSizeHigh = 512/0x100;

	m_MyOtherSpeedConfigDescriptor_FS = m_MyConfigDescriptor_HS;
	m_MyOtherSpeedConfigDescriptor_FS.ConfigDesc.bDescriptorType = USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE;

	m_MyOtherSpeedConfigDescriptor_HS = m_MyConfigDescriptor_FS;
	m_MyOtherSpeedConfigDescriptor_HS.ConfigDesc.bDescriptorType = USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE;

	InitUSBEndpDes();

	DefaultModemSimulation();
}

CUsb2CdcAcm::~CUsb2CdcAcm(void)
{
	//disable endpoints
	MX25USB2_eps_dis(2);
	MX25USB2_eps_dis(4);
	MX25USB2_eps_dis(7);
}

void CUsb2CdcAcm::InitUSBEndpDes(void)
{	
	AttachDevDescriptors(&m_DeviceDescr, &m_DeviceQualifier);
	// Setup PID and VID

	AttachConDescriptors((UCHAR *)&m_MyConfigDescriptor_FS, 0);
	AttachConDescriptors((UCHAR *)&m_MyConfigDescriptor_HS, 1);
	usb_descrLEN_FS = sizeof(m_MyConfigDescriptor_FS);
	usb_descrLEN_HS = sizeof(m_MyConfigDescriptor_HS);
	if((HardwareToken.IAmOEM()) && (GET_SETTING(UsbHighspEnable)))
	{
		usb_othersp_descrLEN = sizeof(m_MyOtherSpeedConfigDescriptor_HS);
		AttachConDescriptors((UCHAR *)&m_MyOtherSpeedConfigDescriptor_HS, 2);
	}
	else
	{
		usb_othersp_descrLEN = sizeof(m_MyOtherSpeedConfigDescriptor_FS);
		AttachConDescriptors((UCHAR *)&m_MyOtherSpeedConfigDescriptor_FS, 2);
	}

	AttachStringDescriptors(pxInterfaceStringDescriptorA, 0);
	pxInterfaceStrDesLEN[0] = sizeof(pxInterfaceStringDescriptorA);

	AttachStringDescriptors(pxInterfaceStringDescriptorB, 1);
	pxInterfaceStrDesLEN[1] = sizeof(pxInterfaceStringDescriptorB);

	CustomizeUsbStrings();
}

void CUsb2CdcAcm::OnClassRequest(void)
{
	switch(ControlData.DeviceRequest.bmRequestType)
	{
		case USB_SET_CLASS_INTERFACE:
		{
			OnSetClassInterfaceReq();
		}
		break;

		case USB_GET_CLASS_INTERFACE:
		{
			OnGetClassInterfacReq();
		}
		break;

		default:
			stall_ep0();	//add
		break;
	}
}


void CUsb2CdcAcm::OnSetClassInterfaceReq(void)
{
	switch( ControlData.DeviceRequest.bRequest )
	{
		//0x20
		case SET_LINE_CODING:
			UINT cnt_n;
			for(cnt_n=0; cnt_n<7; cnt_n++) pxLineCoding[cnt_n] = ControlData.dataBuffer[cnt_n];

			StatusAcknowledge(1);  //send empty package
			break;
		//0x22
		case SET_CONTROL_LINE_STATE:
			// D0: 1=DTR, 0=No DTR,  D1: 1=Activate Carrier, 0=Deactivate carrier (RTS, half-duplex)
			ucControlState = ControlData.DeviceRequest.wValue;
	    	m_cts_active = (ucControlState & BIT1) ? true : false;

			StatusAcknowledge(1);  //send empty package
			break;

		default:
			stall_ep0();
			break;
	}  //endof switch( ControlData.DeviceRequest.bRequest )
}


void CUsb2CdcAcm::OnGetClassInterfacReq(void)
{
	switch( ControlData.DeviceRequest.bRequest )
	{
		case 0x01:
			stall_ep0();
			break;

		//0x21
		case 0x21:
			// get line coding
			if(  ControlData.DeviceRequest.wLength > sizeof( pxLineCoding ) )
			{
				ControlData.DeviceRequest.wLength = sizeof( pxLineCoding );
			}
			//see p.58
			code_transmit((UCHAR *)pxLineCoding, ControlData.DeviceRequest.wLength);
			break;

		default:
			stall_ep0();
			break;
	}
}




void CUsb2CdcAcm::OnVendorRequest(void)
{
	switch(ControlData.DeviceRequest.bmRequestType)
	{
		case 0x41:
			switch(ControlData.DeviceRequest.bRequest)
			{
				case 0x81: 	// wValue.D0 = save bandwith mode
					StatusAcknowledge(1);  //send empty package
					OnChangeBandwithSavingsMode((ControlData.DeviceRequest.wValue & BIT0) ? true : false);
					break;
				default:
					stall_ep0();	//add
					break;
			}
		break;
		default:
			stall_ep0();	//add
			break;
	}
}

inline void CUsb2CdcAcm::EPRxHandleReceivedData(UINT pEp)
{

	if(m_dataBufCtrl)
	{
		m_dataBufCtrl = false;
		datalenEP = MX25USB2_ReadEndpoint_Data(pEp, rxMaxLenIDs, false, dataFromEPsec);  //endpoint 7

		HandleAckNakModeRX((unsigned char *)dataFromEP, datalenEP);     //give the AckNak handler a change to see the protocol characters
		if (datalenEP > 0)
		{
			StuffInputBuffer((unsigned char *)dataFromEP, datalenEP);
		}
	}
	else
	{
		m_dataBufCtrl = true;
		datalenEP = MX25USB2_ReadEndpoint_Data(pEp, rxMaxLenIDs, false, dataFromEP);  //endpoint 7

		HandleAckNakModeRX((unsigned char *)dataFromEPsec, datalenEP);     //give the AckNak handler a change to see the protocol characters
		if (datalenEP > 0)
		{
			StuffInputBuffer((unsigned char *)dataFromEPsec, datalenEP);
		}
	}
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
MatrixError_t CUsb2CdcAcm::ExtractParameters(const tichar_t *pParameters )
{
	MatrixError_t ret = CUsb2Enum::ExtractParameters(pParameters ); // make sure all classes get a chance to extract params

	if ( m_bRemIfaceEnabled )
	{	// NOTE:  We're using an IAD here.  Yes, it's meant for USB 2.0 (however, it does seem to work for Windows), but IADs are only supported for WinXP sp2 or later.  However, the Jungo driver won't work if we don't.
		m_DeviceDescr.bDeviceClass = IAD_DEV_CLASS;
		m_DeviceDescr.bDeviceSubClass = IAD_DEV_SUBCLASS;
		m_DeviceDescr.bDeviceProtocol = IAD_DEV_PROTOCOL;
	}
	else
	{
		m_DeviceDescr.bDeviceClass = USB_CDC_CLASS;
		m_DeviceDescr.bDeviceSubClass = 0x00;
		m_DeviceDescr.bDeviceProtocol = 0x00;
	}

	PatchDeviceDesc((USB_DEVICE_DESCRIPTOR *)DeviceDescr);
	bool OldRtsModeEnabled=m_RtsModeEnabled;
	// this is optional for now
	CParseHelper::Extract(pParameters, _TI("ACK"), m_AckNakEnabled);
	CParseHelper::Extract(pParameters, _TI("CTS"), m_CtsModeEnabled);
	CParseHelper::Extract(pParameters, _TI("CTS"), m_RtsModeEnabled);	// for now until we change the parameters

	// We can turn off the visibility of the USB serial number.
	bool SaveSerialNumberFlag = m_ShowSerialNumber;
	CParseHelper::Extract(pParameters, _TI("SER"), m_ShowSerialNumber);

	if(SaveSerialNumberFlag != m_ShowSerialNumber)
	{
// Need to implement a way of disabling the serial number in the descriptor chain
// Original USB driver has the following function which is commented out
		EnableUsbSerialNumber((USB_DEVICE_DESCRIPTOR *)DeviceDescr, m_ShowSerialNumber);

		ret = ERR_NEED_RECONNECT; 	         // and ensure we simulate a unplug/replug situation
	}
	else
	{	// If we're not going to reconnect, if the user changed CTS handling, send the state out.
		if(IsConfigured() && (OldRtsModeEnabled!=m_RtsModeEnabled)) SendSerialState(!IsEnabled_ActivateRTSWhileData(), m_DtrStatus); // Send the RTS/DTR state
	}

	return ret;
}

inline void CUsb2CdcAcm::OnChangeBandwithSavingsMode(bool bSavingsMode)
{
	m_BandwithSaveMode = bSavingsMode;
	if(bSavingsMode)
	{
		SendResponseAvailable(mMessageLen);
	}
}

void CUsb2CdcAcm::SendResponseAvailable(size_t Size)
{
	const UINT NotificationSize = 12;
	UCHAR pBuffer[NotificationSize];
	pBuffer[0] = USB_GET_VENDOR_INTERFACE;    					// bmRequestType
	pBuffer[1] = 0x01;          							// bNotification
	pBuffer[2] = 0;									// wValueL
	pBuffer[3] = 0;									// wValueH
	pBuffer[4] = 0;									// wIndexL: interface
	pBuffer[5] = 0;									// wIndexH: interface
	pBuffer[6] = 4;									// wLenghtL
	pBuffer[7] = 0;									// wLenghtH

	pBuffer[8]  = Size&0xFF;
	pBuffer[9]  = (Size>>8)&0xFF;
	pBuffer[10] = (Size>>16)&0xFF;
	pBuffer[11] = (Size>>24)&0xFF;

	MX25USB2_WriteEndpoint(EpCommIn, NotificationSize, pBuffer);
}

/// This is for the communication interface.
void CUsb2CdcAcm::SendSerialState(const bool rts, const bool dtr)
{
	const UINT NotificationSize = 10;
	UCHAR pBuffer[NotificationSize];

	pBuffer[0] = USB_GET_CLASS_INTERFACE;                                           // bmRequestType
	pBuffer[1] = SERIAL_STATE;          		                                // bNotification
	pBuffer[2] = 0;									// wValueL
	pBuffer[3] = 0;									// wValueH
	pBuffer[4] = 0;									// wIndexL: interface
	pBuffer[5] = 0;									// wIndexH: interface
	pBuffer[6] = 2;									// wLenghtL
	pBuffer[7] = 0;									// wLenghtH

	pBuffer[8] = (rts << 7)|(dtr << 1);
	pBuffer[9] = 0;									// reserved

	MX25USB2_WriteEndpoint(EpCommIn, NotificationSize, pBuffer);
}


/// Sending ResponseAvailable notifications is not 100% the right thing, but our host driver will use it.
/// So we have a "hidden" way to turn it on.
inline bool CUsb2CdcAcm::IsEnabled_ResponseAvailable(void)
{
	return m_BandwithSaveMode;
}

inline bool CUsb2CdcAcm::IsEnabled_ActivateRTSWhileData(void)
{
	return m_RtsModeEnabled;
}

inline bool CUsb2CdcAcm::IsEnabled_CTS(void)
{
	return m_CtsModeEnabled;
}

/// Here we collect all legacy protocoll handling.
void CUsb2CdcAcm::HandlePreSendActions(void)
{
	// shall we activate the RTS output before sending?
	if(IsEnabled_ActivateRTSWhileData())
	{
		WaitUntilAllDataSent();      //add
		SendSerialState(true, m_DtrStatus);			   // rise RTS output
		// For the Jungo driver we need to wait until the notification has been really delivered to the host
		// Without this, the order of data and notification would be wrong
		WaitUntilNotificationSent();
	}

	// shall we wait until the CTS input is active?
	while( IsEnabled_CTS() && (!m_cts_active) )
	{
		MT_Delay(1);	         	// try again later, data has not been sent yet
	}

	if(m_AckNakEnabled)
	{
		m_WaitingForAckNak = true;
		m_GotNak = false;
	}
}

/// Here we collect all legacy protocoll handling.
inline void CUsb2CdcAcm::HandlePostSendActions(void)
{
	if(IsEnabled_ActivateRTSWhileData())
	{
		// We must wait until all data has been send. Then we can turn off RTS
		// this also provides nessisary delay if write is called twice quickly
		// there must be 200 nS between writing buffer and checking for space (while loop below)

		EpReconnectOnTimeout(2);	//endpoint 2, reconnect after timeout set by mUsbRecoveryTimeout

//		WaitUntilAllDataSent();                    //don't use this because it havn't timeout reconnect handler
		// Dieter Fauth: without the above function we might send the notification too early!!
		// This must be fixed, but I don't understand the exact reason why it has been removed.
		// Tests with the USB-Analyzer showed that we send the notification about 15 ms after the last data.
		// I have seen such delays in other places with the Xenon, so be aware if we fix the delays
		// (I think there is a bug in the low level driver! 4600 is much faster here)
		SendSerialState(false, m_DtrStatus);     	   // lower RTS output
		// For the Jungo driver we need to wait until the notification has been really delivered to the host
		// Without this, the order of data and notification would be wrong
		WaitUntilNotificationSent();
	}
	else
	{
		EpReconnectOnTimeout(2);	//endpoint 2, reconnect after timeout set by mUsbRecoveryTimeout
	}
}

// This function will call TimeoutHack which reconnects the endpoint if the host stops polling the endpoint 
// within a configurable amount of time. 
// - mUsbRecoveryTimeout should be set via USBETO
// - TimeoutHack sets a timer for RECONNECT_TEST_MS (100ms), you set the number of timeouts
void CUsb2CdcAcm::EpReconnectOnTimeout(int EpNum)
{
	if( mUsbRecoveryTimeout > 0 )
	{
		// Number of timeouts is 1 if timeout is less than RECONNECT_TEST_TIMEOUT, it's a multiple of RECONNECT_TEST_TIMEOUT otherwise
		int NumberOfTimeouts = (mUsbRecoveryTimeout > RECONNECT_TEST_MS ? mUsbRecoveryTimeout/RECONNECT_TEST_MS : 1);
		TimeoutHack(EpNum, NumberOfTimeouts, true);
	}
}

/// The TX side of ACK NAK mode.
/// Here we check if we got any response. If it was an ACK, then we are done.
/// A NAK tells us to resend the barcode (return RET_STALL from this function)
usb_err_t CUsb2CdcAcm::HandleAckNakModeTX(void)
{
	while (m_AckNakEnabled && m_WaitingForAckNak)
	{
		MT_Delay(1);	      	// wait for data received
	}

	usb_err_t RetVal=RET_ACK;           // assume we've got an ACK or we turned off the AckNak mode

	if(m_GotNak)
	{
		m_WaitingForAckNak = true;			// prepare for the resend
		m_GotNak = false;
		RetVal = RET_STALL;            	// RET_STALL means resend
	}

	return RetVal;
}

/// The RX side of the ACK NAK mode.
/// Here we receive the response and parse it.
/// While waiting for ACK or NAK we ignore any other characters.
void CUsb2CdcAcm::HandleAckNakModeRX(UCHAR *pBuffer, UINT Size)
{
	if(m_AckNakEnabled && m_WaitingForAckNak)
	{
		for(size_t i=0; i<Size; i++)
		{
			UCHAR temp = pBuffer[i];	         // get the character
			if (temp == NAK)
			{
				m_GotNak = true;
				m_WaitingForAckNak = false;
				break;
			}
			else if (temp == ACK)
			{
				m_WaitingForAckNak = false;
				break;
			}
		}
		Size = 0;	// eat all incomming characters until we see either ACK or NAK
	}
}

const UCHAR * pCurrentText;

/// This is for the data interface
/// Here we send the data to the host.
int CUsb2CdcAcm::SendLabel(CBarcodeData *pLabel)
{
	if (!pLabel)
		return 0;

	usb_err_t RetVal;

	size_t Size = 0;

	pLabel->GetTotalData(pCurrentText, Size);   //CBarcodeData class

	if(Size > 0)
	{
		HandlePreSendActions();    //Raise RTS
		do
		{
			SendToDataInterface((UCHAR *)pCurrentText, Size);  		// send data
			RetVal = HandleAckNakModeTX();	// resend if we've got a NAK
		}while(RetVal != RET_ACK);

		HandlePostSendActions();   //Low RTS
	}

	return 0;
}

void CUsb2CdcAcm::SendToDataInterface(UCHAR *buffer, size_t Size)
{
	if(IsEnabled_ResponseAvailable())
	{
		SendResponseAvailable(Size);
	}
	else
	{
		mMessageLen=Size;
	}

	MX25USB2_WriteEndpoint(2, Size, buffer);
}



void CUsb2CdcAcm::SendToRem(const unsigned char * pData, int length)
{
	CUsbRem rem;  // there's no stack space used by this object.
	rem.SendData(this, pData, length);
}


bool CUsb2CdcAcm::IsRemTxComplete(void)
{
	bool bIsDone = true;
	if ( m_bRemIfaceEnabled ) bIsDone = ! EpHasData(CUsbRem::MyInEndpoint);
	return bIsDone;
}




/// Fill the dummy data with usefull values.
void CUsb2CdcAcm::DefaultModemSimulation(void)
{
	m_linecoding.dwDTERate	= 115200;
	m_linecoding.bCharFormat= 0;
	m_linecoding.bParityType= 0;
	m_linecoding.bDataBits	= 8;
}

inline usb_err_t CUsb2CdcAcm::Ep0DataHandler(void)
{
	usb_err_t RetVal=RET_ACK;

	return RetVal;
}


bool CUsb2CdcAcm::IsCtsInActive(void)
{
	return m_cts_active;
}

bool CUsb2CdcAcm::IsTxComplete(void)
{
	return !EpHasData(2);
}

inline void CUsb2CdcAcm::WaitUntilAllDataSent(void)
{
	CTimeoutmS To(10000);		//wait 10 seconds
	while (!To.HasExpired())
	{
		if (IsTxComplete())
			return;
		MT_Delay(1);
	}
	//flush any data ???
	// Write 1s to the Flush register
	// Erratta BID2053 about flush issue
	// (Removed for NG2D12)
//	UOG_ENDPTFLUSH = 0xFFFFFFFF;
}

/// This ensures we really delivered a notification to the host.
/// Used to guarantie the right order of RTS active, DATA, RTS passiv
void CUsb2CdcAcm::WaitUntilNotificationSent(void)
{
	CTimeoutmS To(10000);		//wait 10 seconds
	while (!To.HasExpired())
	{
		if (!EpHasData(EpCommIn))
			break;
		MT_Delay(1);	// try again later, notification has not been sent yet
	}
}




