/* ==============================================================================
   Common base class for USB drivers
//=================================================================================
   $RCSfile: interfaces_usb2/base_usb2/usb2driverbase.cpp $
   $Revision: 1.27 $
   $Date: 2011/03/31 14:34:47EDT $

//=================================================================================

//=================================================================================*/
#include "stdInclude.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "product.h"
#include "CommonDriver.h"
#include "Usb2DriverBase.h"
#include "BarcodeData.h"
#include "timeout.h"
#include "usb2.h"

#include "usb2_mx25.h"

#include "WedgeHal.h"

#if ! defined(RFBASE)
#define	TRIGGER_5_5		1
	#ifdef TRIGGER_5_5
		#include "trigger_5_5.h"                                         //lmc   fixme
	#else
		#include "trigger.h"
	#endif	//end #ifdef TRIGGER_5_5
#endif // endof #if ! defined(RFBASE)


// static variables to help with chnaging from one USB interface to another.
// We would be too fast for most PCs, so we wait 1.5 Seconds.
CTimeout CDriverUsb2Base::ms_EnsureReconnectTime(2000*MilliSec);
bool CDriverUsb2Base::ms_NeedReconnectDelay = false;
bool CDriverUsb2Base::ms_TimeoutInitialized = false;	// workaround for our missing C++ init

CDriverUsb2Base::CDriverUsb2Base(size_t BufferSizeTx, size_t BufferSizeRx) : CThreadedDriver(BufferSizeTx, BufferSizeRx)
{
	m_pUsb = NULL;
	m_FirstTime = true;

	if (!ms_TimeoutInitialized)
	{
		ms_EnsureReconnectTime.SetExpiration(2000*MilliSec);
		ms_TimeoutInitialized=true;
	}
}

CDriverUsb2Base::~CDriverUsb2Base(void)
{
}

MatrixError_t CDriverUsb2Base::Open(interface_id_t nId, const tichar_t *pParameter)
{
	if (m_FirstTime)
	{
		PrepareCableInterfaceforUSBPowerSave();
	}

	MatrixError_t RetVal = baseCDriverUsb2Base::Open(nId, pParameter);
	ASSERT(m_pUsb != NULL);

	if (m_pUsb == NULL)
	{
		return ERR_MEMORY_EXHAUSTED;
	}

	if(IsOK(RetVal))
	{
		RetVal = m_pUsb->ExtractParameters(pParameter);
	}

	if( (RetVal == ERR_NEED_RECONNECT) || ms_NeedReconnectDelay )	   	// some setting changes require a USB reconnect
	{
		if ( (!m_FirstTime))							// avoid additional startup delay
		{
			m_pUsb->Reconnect();
			ms_NeedReconnectDelay = true;
			ms_EnsureReconnectTime.Reset();	// start reconnect timer
		}
		RetVal = ERR_PASS;
	}
	if (m_FirstTime)
	{
		m_FirstTime = false;
		if(IsOK(RetVal))
		{
//			RESET_WATCHDOG_TIMER();
			RetVal = m_pUsb->SetupHardware();
//			RESET_WATCHDOG_TIMER();
		}
		if(IsOK(RetVal))
		{
			RunThread();
		}
	}
	if(IsOK(RetVal))
	{
		if (ms_NeedReconnectDelay)	      // only add delay when changing interfaces (and not at the very first startup!)
		{
#if !defined(RFBASE)
			pTrigger->SetTempManualTriggerMode();
#endif //endof !defined(RFBASE)

			while (!ms_EnsureReconnectTime.HasExpired())
			{
				RESET_WATCHDOG_TIMER();
			}
			ms_NeedReconnectDelay = false;
		}
		m_pUsb->connect_USB();
	}

	theRemInterface.DeviceIoControl(IOCTRL_REM_SET_USB_DRV, 0, (WPARAM)m_pUsb); // Tell ReM this interace's CUsb2Base pointer.  That means the ReM interface must be Open()d before the main interface.  We do this after usb is setup in case ReM needs to (some day) send data right away.
	return RetVal;
}

MatrixError_t CDriverUsb2Base::Close(void)
{
	ASSERT(m_pUsb != NULL);

	theRemInterface.DeviceIoControl(IOCTRL_REM_SET_USB_DRV, 0, (WPARAM)NULL); // Tell ReM this interace is closed (just in case there's a race between the new interface opening and ReM wanting to send data).  We do this before we start to shut usb down in case (some day) ReM needs to send all queued TX data.
	if(m_pUsb->GetEPnum() == 130)
	{
		m_pUsb->TimeoutHack(2, 10, false);      //Endpoint 2, 1s don't ACK, we'll force to disconnect
		m_pUsb->TimeoutHack(4, 10, false);       //Endpoint 4, 1s don't ACK, we'll force to disconnect
	}

	else if(m_pUsb->GetEPnum() == 131)
	{
		m_pUsb->TimeoutHack(2, 10, false);      //Endpoint 2, 1s don't ACK, we'll force to disconnect
	}
	else if((m_pUsb->GetEPnum() == 128) || (m_pUsb->GetEPnum() == 129))
	{
		m_pUsb->TimeoutHack(2, 10, false);      //Endpoint 2, 1s don't ACK, we'll force to disconnect
		m_pUsb->TimeoutHack(4, 10, false);      //Endpoint 4, 1s don't ACK, we'll force to disconnect
	}
	else if((m_pUsb->GetEPnum() == 124) || (m_pUsb->GetEPnum() == 125) || (m_pUsb->GetEPnum() == 134))
	{
		m_pUsb->TimeoutHack(2, 10, false);      //Endpoint 2, 1s don't ACK, we'll force to disconnect
		m_pUsb->TimeoutHack(4, 10, false);      //Endpoint 4, 1s don't ACK, we'll force to disconnect
	}

	m_pUsb->disconnect_USB();
	ms_NeedReconnectDelay = true;
	ms_EnsureReconnectTime.Reset();	      // start reconnect timer in case we change to another USB interface

	m_pUsb->Close();
	return baseCDriverUsb2Base::Close();
}

IoControlRet_t CDriverUsb2Base::DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam)
{
	ASSERT(m_pUsb != NULL);
	// first call base class
	IoControlRet_t ret=baseCDriverUsb2Base::DeviceIoControl(cmd, lparam, wparam);

	// then modify the answers with our knowledge
	switch(cmd)
	{
		case IOCTRL_IS_DONE:
			if(ret == IOCRET_YES)
			{
				if (m_pUsb && !m_FirstTime && (!m_pUsb->IsTxComplete()))	      // if data has not been sent, ...
					ret = IOCRET_NO;
			}
			else if(!m_pUsb->IsCtsInActive() && m_pUsb->IsEnabled_CTS())
			{
				ret = IOCRET_YES;
			}
		break;

		case IOCTRL_RETURN_TO_NORMAL_POWER_STATE:
			m_pUsb->RemoteWakeUp();
			ret = ERR_PASS;
		break;
		case IOCTRL_SHALL_I_BEEP_ON_POWERUP:
			if (m_pUsb->IsConfigured())
			{
				ret = IOCRET_YES;
			}
			else
			{
				m_pUsb->SetDeferedPowerupBeep();
				ret = IOCRET_NO;
			}
		break;
		case IOCTRL_AM_I_ENUMERATED:
			if (m_pUsb->IsConfigured())
			{
				ret = ERR_PASS;
			}
			else
			{
				ret = ERR_FAIL;
			}
		break;
		case IOCTRL_SHALL_I_BEEP_ON_GOOD_READ:
			if(m_pUsb->IsBeepEnabled())
			{
				ret = IOCRET_YES;
			}
			else
			{
				ret = IOCRET_NO;
			}
		break;
		case IOCTRL_IS_FAILSAVE_INTERFACE:
			ret = IOCRET_YES;
		break;
		case IOCTRL_IS_REMOTE_WAKEUP_EN:
			ret = m_pUsb->IsRemoteWakeUpEn();
		break;

		default:
		break;
	}

	return ret;
}

// This virtual overload is called if there is anything to send (thread loop)
int CDriverUsb2Base::StartSending(void)
{
	ASSERT(m_pUsb != NULL);
	ASSERT(AnyDataToSend());

	int ret=m_pUsb->SendLabel(GetBarcodeToSend());
	SentBarcode();
	return ret;
}

// This virtual overload is called as the very first action of the thread
void CDriverUsb2Base::InitInstance(void)
{
	m_pUsb->WaitUntilWeAreConfigured();
}

size_t CDriverUsb2Base::Write(CBarcodeData  *pLabel)
{
	return baseCDriverUsb2Base::Write(pLabel);
}

void CDriverUsb2Base::ThreadLoop(void)
{
	while (TRUE)					// This is the drivers output loop
	{
//	 	RESET_WATCHDOG_TIMER();

		if( AnyDataToSend() )		// Any characters to send in barcode data fifo?
		{
			PreSendAction();		// what needs to be done before sending           // a noop in the default implementation.
			StartSending();		        // send them
			PostSendAction();		// what needs to be done after sending            // a noop in the default implementation.
			//delay a moment
			DoInterMessageDelay();
		}
		else
		{
			WaitForAnySignal(Sig_NewBarcodeToSend);				// wait until we have data to send
			PrepareWaitForSignal(Sig_NewBarcodeToSend);			// prepare for the next packet
		}
	}
}

void CDriverUsb2Base::SetInterfaceError(int Err)
{
	mErrorUsb=Err;
}


void CDriverUsb2Base::PrepareCableInterfaceforUSBPowerSave(void)
{
	WedgeHal.PrepareCableInterfaceforUSBPower();
	return;
}


