/*=================================================================================
   USB HID Keyboard
//=================================================================================
   $Source: interfaces_usb2/hid_usb2/kbdpc_usb2/drvusb2kbd.h $
   $Date: 2011/01/17 09:51:44EST $
   $Revision: 1.5 $

//=================================================================================

//=================================================================================*/
//! \file

#ifndef _DRV_USB2_KEYBOARD_H_CDD7933B_4209_4291_B69D_1788BB71CCDC__INCLUDED
#define _DRV_USB2_KEYBOARD_H_CDD7933B_4209_4291_B69D_1788BB71CCDC__INCLUDED

#include "Usb2DriverBase.h"
#include "usb2.h"

class CBarcodeData;
class CUsb2HidKbd;

class CWedgeTerminal;
class CWedgeEngineUsbHid;
class CBitBangUsb2Kbd;


#define baseCDriverUsb2Kbd CDriverUsb2Base

class CDriverUsb2Kbd : public baseCDriverUsb2Kbd
{
public:
	CDriverUsb2Kbd(const CWedgeTerminal *pTerminal, UCHAR kbdmod);
	virtual ~CDriverUsb2Kbd();

	virtual MatrixError_t Open(interface_id_t nId, const tichar_t *pParameter);
	virtual MatrixError_t Close();
	virtual IoControlRet_t DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam);

protected:
	virtual void ThreadLoop(void);
	virtual int StartSending(void);

	virtual void PrepareCableInterfaceforUSBPowerSave(void);

	void HidKeyboardIdleAction(void);

	CUsb2HidKbd	*m_pUsb2Kbd;

	CWedgeEngineUsbHid *m_pWedge;
	CBitBangUsb2Kbd *m_pConnector;
	bool m_TurboMode;									//!< Support for a much faster output rate
};
#endif	// _DRV_USB2_KEYBOARD_H_CDD7933B_4209_4291_B69D_1788BB71CCDC__INCLUDED


