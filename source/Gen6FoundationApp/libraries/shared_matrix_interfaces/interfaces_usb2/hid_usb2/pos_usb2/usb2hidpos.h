/* ==============================================================================
   USB HID for HID POS Module header
   ==============================================================================

   $RCSfile: interfaces_usb2/hid_usb2/pos_usb2/usb2hidpos.h $
   $Revision: 1.5 $
   $Date: 2011/03/31 14:38:48EDT $

//=================================================================================

//=================================================================================*/

#ifndef _USB2HID_POS_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define _USB2HID_POS_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "Usb2HidHlp.h"
#include "usbdesc.h"
#include "UsbRem.h"


class CUsb2HidPos : public CUsb2HidPosHelper
{
	
protected:
	enum
	{
		// define the interface metrics 
		SINGLE_INTERFACE = 0
	};
	
public:
	CUsb2HidPos(CRxFifo *pRxFifo, CThreadRealTime *pThread);
	virtual ~CUsb2HidPos(void);
	virtual usb_err_t ReportHandler(void);
	virtual usb_err_t Ep0DataHandler(void);

	virtual int GetReportSize(DEVICE_REQUEST *pRequest);

	virtual int SendLabel(CBarcodeData *pLabel);
	virtual MatrixError_t ExtractParameters(const tichar_t *pParameters);

protected:
	typedef struct
	{
		ConfigDescriptor_t ConfigDesc; 
		InterfaceDescriptor_t IfaceDesc;
		HIDDescriptor1_t HidDesc;
		EndpointDescriptor_t InEndpoint;
		EndpointDescriptor_t OutEndpoint;

		CUsbRem::IfaceDescriptor_t RemIfaceDesc;
	} MyConfigDescriptor_t;

	static MyConfigDescriptor_t m_MyConfigDescriptor;
	MyConfigDescriptor_t m_MyOtherSpeedConfigDescriptor;

	void InitUSBEndpDes(void);
	virtual UCHAR GetSizeOfConfigDesc(void) {return sizeof(MyConfigDescriptor_t);}  // this includes ReM
	virtual UCHAR GetMaxNumIfaces(void) {return 2;}  // this includes ReM
	virtual UCHAR GetPidOffset(bool bIsRem) {if (bIsRem) return USB_POS_REM_PID_OFFSET; else return USB_POS_PID_OFFSET;}

public:
};

#endif	/* _USB2HID_POS_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */





