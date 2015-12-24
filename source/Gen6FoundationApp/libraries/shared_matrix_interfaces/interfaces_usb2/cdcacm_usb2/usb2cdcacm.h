/*=================================================================================
  Class declaration for a COM port emulation with the CDC ACM.
	(Communication Device Class - Abstract Control Model)
//=================================================================================
   $Source: interfaces_usb2/cdcacm_usb2/usb2cdcacm.h $
   $Revision: 1.15 $
   $Date: 2011/10/11 11:11:48EDT $

//=================================================================================

//=================================================================================*/
//! \file

#ifndef _USB2CDC_ACM_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define _USB2CDC_ACM_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "usb2enum.h"
#include "usbdesc.h"
#include "UsbRem.h"

class CBarcodeData;

class CUsb2CdcAcm : public CUsb2Enum
{
public:
	CUsb2CdcAcm(CRxFifo *pRxFifo, CThreadRealTime *pThread, bool UseAlternativPID=false);
	virtual ~CUsb2CdcAcm(void);

	// Send the contents of a barcode
	virtual int SendLabel(CBarcodeData *pLabel);

	// called from the Open function of the driver
	virtual MatrixError_t ExtractParameters(const tichar_t *pParameters );

	virtual usb_err_t Ep0DataHandler(void);

protected:
	virtual void EPRxHandleReceivedData(UINT pEp);
	virtual void OnClassRequest(void);
	void OnSetClassInterfaceReq(void);
	void OnGetClassInterfacReq(void);
	virtual void OnVendorRequest(void);
	virtual bool IsTxComplete(void);
	virtual bool IsCtsInActive(void);
	virtual bool IsEnabled_CTS(void);

	volatile int mMessageLen;

	void SendToDataInterface(UCHAR *buffer, size_t Size);  // a interface to the low level code
	virtual void SendToRem(const unsigned char * pData, int length);
	virtual bool IsRemTxComplete(void);
	void SendResponseAvailable(size_t Size);
	virtual void OnChangeBandwithSavingsMode(bool bSavingsMode);
	void InitUSBEndpDes(void);
	virtual UCHAR GetSizeOfConfigDesc(void) {return sizeof(MyConfigDescriptor_t);}  // this includes ReM
	virtual UCHAR GetMaxNumIfaces(void) {return 3;}  // this includes ReM
	virtual UCHAR GetPidOffset(bool bIsRem) {if (bIsRem) return USB_MODEM_ACM_REM_PID_OFFSET;else return USB_MODEM_ACM_PID_OFFSET;}
	void EpReconnectOnTimeout(int EpNum);

protected:

// Class-Specific Request Codes (CDC)
	enum
	{
		SEND_ENCAPSULATED_COMMAND = 0x00,
		GET_ENCAPSULATED_RESPONSE = 0x01,
		SET_COMM_FEATURE = 0x02,
		GET_COMM_FEATURE = 0x03,
		CLEAR_COMM_FEATURE = 0x04,
		SET_AUX_LINE_STATE = 0x10,
		SET_HOOK_STATE = 0x11,
		PULSE_SETUP = 0x12,
		SEND_PULSE = 0x13,
		SET_PULSE_TIME = 0x14,
		RING_AUX_JACK = 0x15,
		SET_LINE_CODING = 0x20,
		GET_LINE_CODING = 0x21,
		SET_CONTROL_LINE_STATE = 0x22,
		SEND_BREAK = 0x23,
		SET_RINGER_PARMS = 0x30,
		GET_RINGER_PARMS = 0x31,
		SET_OPERATION_PARMS = 0x32,
		GET_OPERATION_PARMS = 0x33,
		SET_LINE_PARMS = 0x34,
		GET_LINE_PARMS = 0x35,
		DIAL_DIGITS = 0x36,
		SET_UNIT_PARAMETER = 0x37,
		GET_UNIT_PARAMETER = 0x38,
		CLEAR_UNIT_PARAMETER = 0x39,
		GET_PROFILE = 0x3A,
		SET_ETHERNET_MULTICAST_FILTERS = 0x40,
		SET_ETHERNET_POWER_MANAGEMENT_PATTERN = 0x41,
		GET_ETHERNET_POWER_MANAGEMENT_PATTERN = 0x42,
		SET_ETHERNET_PACKET_FILTER = 0x43,
		GET_ETHERNET_STATISTIC = 0x44
	};

	typedef struct
	{
		ConfigDescriptor_t ConfigDesc; 
		IadDescriptor_t IadDesc;
		InterfaceDescriptor_t CommClassIfaceDesc;
		CDC_HeaderDescriptor_t HeaderFunctionalDesc;
		CDC_AC_ManagmentDescriptor_t AcManagFunctionalDesc;
		CDC_UnionFunctionalDescriptor_t UnionFunctionalDesc;
		CDC_CallManagmentDescriptor_t CallManagFunctionalDesc;
		EndpointDescriptor_t ControlInEndpoint;
		InterfaceDescriptor_t DataIfaceDesc;
		EndpointDescriptor_t DataOutEndpoint;
		EndpointDescriptor_t DataInEndpoint;

		CUsbRem::IfaceDescriptor_t RemIfaceDesc;
	} MyConfigDescriptor_t;

	static USB_DEVICE_DESCRIPTOR m_DeviceDescr;
	static HS_DEVICE_QUALIFIER m_DeviceQualifier;
	static MyConfigDescriptor_t m_MyConfigDescriptor_FS;
	MyConfigDescriptor_t m_MyConfigDescriptor_HS;
	// TODO:  rather than have these copies (the only thing different is bDescriptorType), we could adjust the appropriate bDescriptorType when we have to reply to the GET_DESCRIPTOR.
	MyConfigDescriptor_t m_MyOtherSpeedConfigDescriptor_FS;
	MyConfigDescriptor_t m_MyOtherSpeedConfigDescriptor_HS;

// Class-Specific Notification Codes (CDC)
	enum
	{
		SERIAL_STATE 			= 0x20
	};

// How the host tells us info about baudrate etc. (dummies for us)
	struct LineCoding_t
	{
		ULONG dwDTERate;	// Number Data terminal rate, in bits per second.
		UCHAR	bCharFormat;	// Number stop bits
									// 0 - 1 Stop bit
									// 1 - 1.5 Stop bits
									// 2 - 2 Stop bits
		UCHAR bParityType;	// Number Parity
									// 0 - None
									// 1 - Odd
									// 2 - Even
									// 3 - Mark
									// 4 - Space
		UCHAR bDataBits;		// Number Data bits (5, 6, 7, 8 or 16).  // we do not support 16!
	};

	void SendCommNotification (int size);

	void HandlePreSendActions(void);
	void HandlePostSendActions(void);
	void WaitUntilAllDataSent(void);
	void WaitUntilNotificationSent(void);

	// class specific responses/notifications
	bool IsEnabled_ResponseAvailable(void);
	bool IsEnabled_ActivateRTSWhileData(void);
//	bool IsEnabled_CTS(void);
	void SendSerialState(const bool rts, const bool dtr);
//	usb_err_t GetEncapsuledResponse(void);

	void DefaultModemSimulation(void);

	// emulate old protocols for the sake of compatibility
	void HandleAckNakModeRX(UCHAR *pBuffer, UINT Size);
	usb_err_t HandleAckNakModeTX(void);

	volatile bool m_AckNakEnabled;
	volatile bool m_CtsModeEnabled;
	volatile bool m_RtsModeEnabled;
	volatile bool m_DtrStatus;
//	bool m_XonEnabled;
//	UCHAR	m_xoff;
//	UCHAR	m_xon;

	volatile bool m_GotNak;
	volatile bool m_WaitingForAckNak;
	bool m_ShowSerialNumber;

	// class specific data members
	LineCoding_t m_linecoding;

};

#endif	/* _USBCDC_ACM_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */


