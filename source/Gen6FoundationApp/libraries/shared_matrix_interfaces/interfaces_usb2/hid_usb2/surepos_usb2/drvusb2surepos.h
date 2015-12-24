/* ==============================================================================
   USB SurePos Module
//=================================================================================
   $Source: interfaces_usb2/hid_usb2/surepos_usb2/drvusb2surepos.h $
   $Revision: 1.3 $
   $Date: 2009/07/06 23:17:20EDT $

//=================================================================================

//=================================================================================*/
//! \file

#ifndef _DRV_USB2_SUREPOS_H_9A9BF9A1_C396_4DB5_8D17_39E154DF4FBA__INCLUDED
#define _DRV_USB2_SUREPOS_H_9A9BF9A1_C396_4DB5_8D17_39E154DF4FBA__INCLUDED

class CBarcodeData;

#include "Usb2DriverBase.h"

class CBarcodeData;
class CUsb2HidSurePos;

#define baseCDriverUsb2SurePos CDriverUsb2Base
class CDriverUsb2SurePos : public baseCDriverUsb2SurePos
{
public:
	CDriverUsb2SurePos(bool TableTop);
	virtual ~CDriverUsb2SurePos();

	virtual size_t Write(CBarcodeData  *pLabel);	//default writes to a fifo
	virtual size_t Write(const bchar_t *pBin, size_t nSize) // add these to remove warnings
	{
		return CDriverBase::Write(pBin,nSize);
	};
	virtual size_t Write(const wchar_t *pText, size_t nSize)
	{
		return CDriverBase::Write(pText,nSize);
	};

protected:
	virtual void InitInstance(void);
	virtual void ThreadLoop(void);

	CUsb2HidSurePos* GetSurePosObject(void);
};

#endif	// _DRV_USB2_SUREPOS_H_9A9BF9A1_C396_4DB5_8D17_39E154DF4FBA__INCLUDED

