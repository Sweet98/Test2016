/* ==============================================================================
   USB HID POS communication declarations
//=================================================================================
   $Source: interfaces_usb2/hid_usb2/pos_usb2/drvusb2pos.h $
   $Date: 2010/10/15 14:54:33EDT $
   $Revision: 1.5 $

//=================================================================================

//=================================================================================*/

#ifndef _DRV_USB2_HIDPOS_H_CDD7933B_4209_4291_B69D_1788BB71CCDC__INCLUDED
#define _DRV_USB2_HIDPOS_H_CDD7933B_4209_4291_B69D_1788BB71CCDC__INCLUDED

#if(0)
/* All the USB HID POS specific variables in one place */
typedef struct tag_USB_PosVariables
{
	USB_HidVariables_t hid;		/* must be the very first variable! */
	UCHAR ucStatus0;		/* Status byte */
	UCHAR ucStatus1;		/* Status byte */
	UsbPosInReportBuffer_t	InReportBuffer;
	BOOL	IgnoreConfigFlag;

}USB_PosVariables_t;
#endif

class CBarcodeData;

#include "Usb2DriverBase.h"

class CBarcodeData;
class CUsb2HidPos;

#define baseCDriverUsb2Pos CDriverUsb2Base
class CDriverUsb2Pos : public baseCDriverUsb2Pos
{
public:
	CDriverUsb2Pos();
	virtual ~CDriverUsb2Pos();
	virtual IoControlRet_t DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam);
};

#endif	// _DRV_USB2_HIDPOS_H_CDD7933B_4209_4291_B69D_1788BB71CCDC__INCLUDED

