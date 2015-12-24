/*=================================================================================
  class: CWedgeEngineUsbHid

//=================================================================================
   $Source: interfaces_wedge/wedgeengineusb.cpp $
   $Date: 2010/04/13 17:25:54EDT $
   $Revision: 1.8 $



//=================================================================================*/

#include "stdInclude.h"
#include "CommonDriver.h"
#include "WedgeEngineUsb.h"
#include "Bitbanger.h"
#include "Usb2Kbd.h"
#include "WedgeHal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor
CWedgeEngineUsbHid::CWedgeEngineUsbHid(CThreadedDriver *pDriver, const CWedgeTerminal *pTerminal, CBitBanger *pBitBanger)
	:baseCWedgeEngineUsbHid(pDriver, pTerminal, pBitBanger)
{
#if USB2DBG
	USB2_TRACE("CWedgeEngineUsbHid::CWedgeEngineUsbHid\r\n");
#endif
}


// Destructor
CWedgeEngineUsbHid::~CWedgeEngineUsbHid()
{
}


MatrixError_t CWedgeEngineUsbHid::Open(interface_id_t nId, const tichar_t *pParameter)
{
#if USB2DBG
//	USB2_TRACE("CWedgeEngineUsbHid::Open\r\n");
#endif

	MatrixError_t RetVal = baseCWedgeEngineUsbHid::Open(nId, pParameter);
	if(IsOK(RetVal))
	{
#if USB2DBG
	USB2_TRACE("CWedgeEngineUsbHid::Open1\r\n");
#endif

		StoreKbdStyle(5);				// Overwrite keyboard style with KeyEmu mode!
		WdgStoreKeyEmu(true);		// We use a flag separate from the style because we enable/disable this feature at runtime
		StoreAutoKeyEmu(false);		// Ensure this is turned off!
	}
	return RetVal;
}

void CWedgeEngineUsbHid::FinalCloseLabel(void)
{
	BitBangOut(USB_EMPTY_VALUE, EX_CLOSE);  // send an "ALL Keys UP" report
}

void CWedgeEngineUsbHid::NotifyOutTermDelay(ULONG uSecDelay)
{
	ASSERT(m_pBitBang!=NULL);
	m_pBitBang->NotifyOutTermDelay(uSecDelay);
}


void CWedgeEngineUsbHid::PrepareIoForWedgeHardware(void)
{
	//WedgeHal.PrepareCableInterfaceforUSBPower();   //Configured in Cusb2driverbase, so no need to do it again here (but do not want to run PrepareIoForWedge() either (power savings in suspend)
	return;
}

void CWedgeEngineUsbHid::EnableKeyboard (void)
{
	//WedgeHal.KeyboardEnable();
	return;
}

void CWedgeEngineUsbHid::DisableKeyboard (void)
{
	//WedgeHal.KeyboardDisable();
	return;
}

/*
	This class is the front end of the wedge engin for the USB HID keyboards.
*/




