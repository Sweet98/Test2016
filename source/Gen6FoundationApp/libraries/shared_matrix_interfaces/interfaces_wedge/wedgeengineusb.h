/*=================================================================================
  class: CWedgeEngineUsbHid

//=================================================================================
   $Source: interfaces_wedge/wedgeengineusb.h $
   $Date: 2010/04/13 17:26:30EDT $
   $Revision: 1.7 $



//=================================================================================*/

#ifndef WDGE_ENGINE_USB_HID_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define WDGE_ENGINE_USB_HID_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "WedgeEngine.h"


#define baseCWedgeEngineUsbHid CWedgeEngine
class CWedgeEngineUsbHid : public baseCWedgeEngineUsbHid
{
public:
	CWedgeEngineUsbHid(CThreadedDriver *pDriver, const CWedgeTerminal *pTerminal, CBitBanger *pBitBanger);
	virtual ~CWedgeEngineUsbHid();
//	virtual MatrixError_t DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam);
   virtual MatrixError_t Open(interface_id_t nId, const tichar_t *pParameter);
	virtual void NotifyOutTermDelay(ULONG uSecDelay);

protected:
//	virtual void OpenLabel(void);
	virtual void FinalCloseLabel(void);

	virtual void PrepareIoForWedgeHardware(void);
	virtual void EnableKeyboard(void);
	virtual void DisableKeyboard(void);

};

#endif /* WDGE_ENGINE_USB_HID_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */


