/* ==============================================================================
   USB HID declarations
   ==============================================================================

   $RCSfile: interfaces_usb2/hid_usb2/hidbase_usb2/usb2hidhlp.h $
   $Revision: 1.3 $
   $Date: 2009/07/06 23:16:46EDT $

//=================================================================================

//=================================================================================*/
//! \file

#ifndef _USB2HID_HELPER_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define _USB2HID_HELPER_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "Usb2Hid.h"
#include "Usb2Pos.h"

class CBarcodeData;

class CUsb2HidPosHelper : public CUsb2Hid
{
public:
	CUsb2HidPosHelper(CRxFifo *pRxFifo, CThreadRealTime *pThread);
	virtual ~CUsb2HidPosHelper(void);

	int HidPosSendLabel(int nInterface, CBarcodeData *pLabel);
	void HidPosSend0Label(int nInterface);

protected:
	virtual void EPRxHandleReceivedData(UINT pEp);
		
	usb_err_t FeatureInRedirectHandler(void);
	usb_err_t FeatureOutRedirectHandler(void);
	usb_err_t EP0_OutTriggerHandler(void);
	usb_err_t EP0_Out1252Handler(void);

	usb_err_t OutTriggerHandler(UINT pEp);
	usb_err_t Out1252Handler(UINT pEp);
	
	usb_err_t EP0_PosInEmptyHandler(void);

	void Beep(void);
	void ErrorBeep(void);
	const static THidReportTable<CUsb2HidPosHelper> ms_PosReports[];
	
private:	
	volatile UINT totalDEP;	
};

#endif


