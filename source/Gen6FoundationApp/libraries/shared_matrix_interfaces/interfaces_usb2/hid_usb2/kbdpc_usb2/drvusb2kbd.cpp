/*=================================================================================
	USB keyboard dsriver for PC and MAC

//=================================================================================
   $Source: interfaces_usb2/hid_usb2/kbdpc_usb2/drvusb2kbd.cpp $
   $Date: 2011/08/25 09:15:49EDT $
   $Revision: 1.12 $

//=================================================================================

//=================================================================================*/
//! \file

#include "stdInclude.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "product.h"
#include "CommonDriver.h"
#include "Usb2HidKbd.h"
#include "BarcodeData.h"
#include "drvusb2kbd.h"
#include "Usb2Kbdbase.h"
#include "WedgeEngineUsb.h"
#include "ParseHelper.h"

extern const CWedgeTerminal USB_EPC_Keyboard_TERM;
extern const CWedgeTerminal APPLE_USB_TERM;
extern const CWedgeTerminal USB_JAP_106_TERM;

// This is the class factory for this driver
// There are two flavors for USB keyboard. The difference is the layout of the keys. The rest is the same.
CDriverBase *CreateUsbKbdPc(void)
{
	return (new CDriverUsb2Kbd(&USB_EPC_Keyboard_TERM, 124));
}

// Apple Mac has a different layout compared to the PC keyboards (in some countries).
CDriverBase *CreateUsbKbdMac(void)
{
	return (new CDriverUsb2Kbd(&APPLE_USB_TERM, 125));
}

CDriverBase *CreateUsbJapKbd(void)
{
	return (new CDriverUsb2Kbd(&USB_JAP_106_TERM, 134));
}

CDriverUsb2Kbd::CDriverUsb2Kbd(const CWedgeTerminal *pTerminal, UCHAR kbdmod)
//: m_TurboMode(false)
{
	ASSERT(pTerminal!=NULL);

	m_pUsb2Kbd = new CUsb2HidKbd(m_pRxFifo, NULL, kbdmod);	// create the USB component

	ASSERT(m_pUsb2Kbd != NULL);
	m_pUsb = m_pUsb2Kbd;

	m_pConnector = new CBitBangUsb2Kbd(pTerminal, m_pUsb2Kbd);  	// create the bitbanger as a connector
	ASSERT(m_pConnector != NULL);

	m_pWedge = new CWedgeEngineUsbHid(this, pTerminal, m_pConnector);  // create the wedge engine

	m_pUsb2Kbd->Attach(m_pWedge);	// connect USB to wedge for storing the LED info

	m_OutputFormat = IO_DATATYPE_UNKNOWN;  // Switch to "no translation" in the base class.
												// We have two interfaces, so we handle translation ourselves.
	m_TurboMode = false;
}

// Destructor
CDriverUsb2Kbd::~CDriverUsb2Kbd()
{
	delete m_pWedge;
	m_pWedge = NULL;

	delete m_pUsb2Kbd;
	m_pUsb2Kbd = NULL;

	// The bitbanger component (m_pConnector) is deleted by the WedgeEngine (m_pWedge)
}


MatrixError_t CDriverUsb2Kbd::Open(interface_id_t nId, const tichar_t *pParameter)
{
	ASSERT(m_pUsb2Kbd != NULL);
	ASSERT(m_pWedge != NULL);
	ASSERT(m_pConnector != NULL);

	// Check for turbo mode.
	// We must do it before we call the Open function of the modules because we need to patch the USB descriptors.
	// If we already opened the USB before we must ensure to cause a new enumeration phase.
	bool OldTurboMode = m_TurboMode;
	bool ret = CParseHelper::Extract(pParameter, _TI("Turbo"), m_TurboMode);

	if(ret && (OldTurboMode != m_TurboMode))
	{
		// Set the poll interval in the EP descriptor to 8 or 1 ms
		m_pUsb2Kbd->SetPollingInterval(nId, m_TurboMode);

		// Tell our base class that we need a re-enumeration since we changed the descriptors.
		// But at the very first open, this is superflows.
		if(!m_FirstTime)
		{
			ms_NeedReconnectDelay = true;
		}
	}


	MatrixError_t RetVal = m_pWedge->Open(nId, pParameter);

	if(IsOK(RetVal))
	{
		// We next open the base class (and USB component), after the  wedge engine.
		// This way we guaranty that paramters (delays) are handled correctly.
   		RetVal = baseCDriverUsb2Kbd::Open(nId, pParameter);
	}

	if(IsOK(RetVal))	                     // sanity
	{
		if ((m_pUsb2Kbd == NULL) || (m_pWedge == NULL) || (m_pConnector == NULL))
			RetVal = ERR_MEMORY_EXHAUSTED;
	}
	return RetVal;
}

MatrixError_t CDriverUsb2Kbd::Close(void)
{
	m_pWedge->Close();
	baseCDriverUsb2Kbd::Close();
	return ERR_PASS;
}

IoControlRet_t CDriverUsb2Kbd::DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam)
{
	IoControlRet_t RetVal=baseCDriverUsb2Kbd::DeviceIoControl(cmd, lparam, wparam);
	switch(cmd)
	{
		case IOCTRL_IS_INTERPRET_DATA:
			RetVal = IOCRET_YES;
		break;
		case IOCTRL_SHALL_I_DELAY:
			RetVal = IOCRET_YES;
		break;
		case IOCTRL_NEED_PRESUFFIX_ESC:
			if(m_pWedge->NeedsPreSuffixEsc())
			{
				RetVal = IOCRET_YES;
			}
			break;
		default:
		break;
	}

	return RetVal;
}

void CDriverUsb2Kbd::ThreadLoop(void)
{
	while (TRUE)					// This is the drivers output loop
	{
		if (AnyDataToSend())		// Any characters to send?
		{
			StartSending();		// Yes, send them
		}
		else
		{
			HidKeyboardIdleAction();
		}
	}
}

int CDriverUsb2Kbd::StartSending(void)
{
	ASSERT(AnyDataToSend());
	int Ret =0;

	if(m_pUsb2Kbd->IsOutputToKeyboard())
	{
		Ret =m_pUsb2Kbd->SendLabelToKeyboard(GetBarcodeToSend());  				// send via the keyboard interface
	}
	else
	{
		Ret=m_pUsb2Kbd->SendLabel(GetBarcodeToSend());  	// send via the HidPos interface
		SentBarcode();
	}

	return Ret;
}

void CDriverUsb2Kbd::HidKeyboardIdleAction(void)
{
	timer_uSec_t IdleTime = m_pUsb2Kbd->GetKbdIdleSteps();
	if (IdleTime > 0)
	{
		IdleTime *= 4*MilliSec;	// make uSecs (USB HID defines one step to be 4 mSec)
		ULONG WakeReason = WaitForAnySignal(Sig_NewBarcodeToSend, IdleTime);	// wait until we have data to send or timeout
		if(WakeReason & Sig_NewBarcodeToSend)
		{
			PrepareWaitForSignal(Sig_NewBarcodeToSend);			// prepare for the next packet
		}
		else
		{
			m_pUsb2Kbd->SendAllKeysUp();
		}
	}
	else
	{
		WaitForAnySignal(Sig_NewBarcodeToSend);				// wait until we have data to send
		PrepareWaitForSignal(Sig_NewBarcodeToSend);			// prepare for the next packet
	}
}


void CDriverUsb2Kbd::PrepareCableInterfaceforUSBPowerSave(void)
{
	WedgeHal.PrepareCableInterfaceforUSBPower();                //test case
	return;
}



