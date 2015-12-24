/*=================================================================================
  class: CUsb2ComEmu

//=================================================================================
   $Source: interfaces_usb2/cdcacm_usb2/drvusb2comemu.h $
   $Revision: 1.4 $
   $Date: 2009/07/06 23:16:48EDT $

//=================================================================================

//=================================================================================*/

#ifndef _DRV_USB_2_COM_EMU_H_INCLUDED
#define _DRV_USB_2_COM_EMU_H_INCLUDED

#include "Usb2DriverBase.h"

class CBarcodeData;
class CUsb2CdcAcm;


#define baseCUsb2Driver CDriverUsb2Base

class CUsb2ComEmu : public baseCUsb2Driver
{
public:
	CUsb2ComEmu(void);
	virtual ~CUsb2ComEmu();

protected:
public:
};

#endif	//_DRV_USB_2_COM_EMU_H_INCLUDED

   
