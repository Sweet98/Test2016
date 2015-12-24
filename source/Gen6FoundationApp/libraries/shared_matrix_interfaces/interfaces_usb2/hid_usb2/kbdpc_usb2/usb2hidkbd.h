/* ==============================================================================
   USB HID for HID Keyboard Module header
   ==============================================================================

   $RCSfile: interfaces_usb2/hid_usb2/kbdpc_usb2/usb2hidkbd.h $
   $Revision: 1.7 $
   $Date: 2011/03/31 14:38:48EDT $

//=================================================================================

//=================================================================================*/
//! \file


#ifndef _USB2HID_KBD_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define _USB2HID_KBD_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "Usb2HidHlp.h"
#include "Usb2Kbd.h"
#include "usbdesc.h"
#include "UsbRem.h"

class CWedgeEngine;

class CUsb2HidKbd : public CUsb2HidPosHelper
{	
protected:
	enum
	{
		// define the interface metrics
		KBD_INTERFACE = 0,
		POS_INTERFACE = 1
	};		
	
public:
	CUsb2HidKbd(CRxFifo *pRxFifo, CThreadRealTime *pThread, UCHAR kbdmod);
	virtual ~CUsb2HidKbd(void);

	virtual usb_err_t ReportHandler(void);
	virtual usb_err_t Ep0DataHandler(void);
		
	virtual int GetReportSize(DEVICE_REQUEST *pRequest);

	virtual int SendLabel(CBarcodeData *pLabel);
	virtual MatrixError_t ExtractParameters(const tichar_t *pParameters );

	ULONG	GetKbdIdleSteps(void)		{ return GetInterface(KBD_INTERFACE)->GetIdle(0);}
	void SendAllKeysUp(void);
	bool IsOutputToKeyboard(void)		{ return (GetDestinationInterface() == KBD_INTERFACE);}

	int SendLabelToKeyboard(CBarcodeData *pLabel);

	void Attach(CWedgeEngine *pWedge)					{ m_pWedge = pWedge;	}
	KbdInReportBuffer_t *GetWritableKbdReportBuffer(void);

	void SetPollingInterval(UCHAR kbdmod, bool kbdTurboMode);

#ifdef USB_DEBUG_KBD
	void SendKbdReport(bool bWait=true)
	{
		SendTestReport (POS_INTERFACE, HID_KEYBOARD_REPORT_SIZE);
	}
	void SendTestReport(int interface, size_t size);
#else
	void SendKbdReport(bool bWait=true);
#endif

protected:
	void InitUSBEndpDes(UCHAR kbdmode);
	virtual UCHAR GetSizeOfConfigDesc(void) {return sizeof(CUsb2HidKbd::MyConfigDescriptor_t);}  // this includes ReM
	virtual UCHAR GetMaxNumIfaces(void) {return 3;}  // this includes ReM
	virtual UCHAR GetPidOffset(bool bIsRem);

	virtual void ReceiveKbdData(UINT pEp);

	usb_err_t EP0_InReportHandler(void);
	usb_err_t EP0_OutReportHandler(void);

	void StoreLEDInfo(UCHAR ucLED);

// data members
	bool m_bIsMacKeyboard;
protected:
public:
	typedef struct
	{
		ConfigDescriptor_t ConfigDesc; 
		InterfaceDescriptor_t KeyboardIfaceDesc;
		HIDDescriptor1_t KeyboardHidDesc;
		EndpointDescriptor_t KeyboardInEndpoint;
		EndpointDescriptor_t KeyboardOutEndpoint;
		InterfaceDescriptor_t HidPosIfaceDesc;
		HIDDescriptor1_t HidPosHidDesc;
		EndpointDescriptor_t HidPosInEndpoint;
		EndpointDescriptor_t HidPosOutEndpoint;

		CUsbRem::IfaceDescriptor_t RemIfaceDesc;
	} MyConfigDescriptor_t;

	static MyConfigDescriptor_t m_MyConfigDescriptor;
	MyConfigDescriptor_t m_MyOtherSpeedConfigDescriptor;
	CWedgeEngine *m_pWedge;
	ULONG m_OutputFormat;
	bool m_KbdTurboMode;
	const static THidReportTable<CUsb2HidKbd> ms_KbdReports[];
};

#endif	/* _USB2HID_KBD_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */





