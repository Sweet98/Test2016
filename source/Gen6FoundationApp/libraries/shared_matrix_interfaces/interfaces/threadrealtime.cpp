/* ==============================================================================
   Simulation for the driver thread Module Source
   ==============================================================================
 
   $RCSfile: interfaces/threadrealtime.cpp $
   $Revision: 1.16.2.4 $
   $Date: 2009/11/09 16:39:10EST $
 
   ==============================================================================

   ============================================================================== */

#include "stdInclude.h"
#include "product.h"
#include "ThreadRealtime.h"
#include "Timeout.h"
#include "thread.h"
#include "CallbackHelper.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CThreadRealTime::CThreadRealTime(size_t stacksize)
{
	m_signal = 0;
	m_StackSize =stacksize;
	mThreadHandle=0;
	mSignalmbox=0;
}
int CThreadRealTime::Stop(void)
{
	if (mSignalmbox)
	{
		MT_DeleteMailbox(mSignalmbox);
		mSignalmbox=NULL;
	}
	if (mThreadHandle != 0)
	{
		MT_CloseDriverThread(mThreadHandle);
		mThreadHandle = 0;
	}
	return 1;
}


CThreadRealTime::~CThreadRealTime()
{
	Stop();
}

bool CThreadRealTime::PrepareRun (int Priority,void (*std_action)(void *pUniversal), void *pUniversal,bool CreateSignal)
{
	if (!mSignalmbox && CreateSignal)
	{
		mSignalmbox=MT_CreateMailbox(16,sizeof(unsigned long));
		if (!mSignalmbox)
		{
			return false;
		}
	}
	if (!mThreadHandle)
	{
		mThreadHandle = MT_OpenDriverThread( Priority,m_StackSize,std_action,pUniversal);
	}
	if (!mThreadHandle)
	{
		return false;
	}
	return true;
}

bool CThreadRealTime::PrepareRun (int Priority,CCallBackTranslator *pConnector,bool CreateSignal)
{
	return PrepareRun (Priority,pConnector->GetTranslatedFunctionPointer(), pConnector->GetTranslatedThisPointer(),CreateSignal);
}

bool CThreadRealTime::Run (int Priority,CCallBackTranslator *pConnector,bool CreateSignal)
{
	if (!PrepareRun (Priority,pConnector,CreateSignal))
	{
		return false;
	}
	WakeDriverThread(mThreadHandle);
	return true;
}

bool CThreadRealTime::Run (int Priority,void (*std_action)(void *pUniversal), void *pUniversal,bool CreateSignal)
{
	if (!PrepareRun(Priority,std_action,pUniversal,CreateSignal))
	{
		return false;
	}
	WakeDriverThread(mThreadHandle);
	return true;
}



void CThreadRealTime::Sleep(timer_uSec_t uSec)
{
	MT_Delay(uSec/1000);
}


void CThreadRealTime::PrepareWaitForSignal(ULONG SignalMask)
{
	DISABLE_INTERRUPTS();
	m_signal &= ~SignalMask;
	RESTORE_INTERRUPTS();
}


ULONG CThreadRealTime::WaitForAnySignal(ULONG SignalMask)
{
	while ((m_signal & SignalMask) == 0)
	{
		unsigned int mbsignal=0;
		if (MT_WaitMailbox(mSignalmbox,INFINITE,&mbsignal))
		{
			DISABLE_INTERRUPTS();
			m_signal |=mbsignal;
			RESTORE_INTERRUPTS();
		}
	}
	return m_signal;
}

// with a timeout
ULONG CThreadRealTime::WaitForAnySignal(ULONG SignalMask, ULONG uSec)
{
	unsigned int mbsignal=0;
	CTimeoutmS timeout(uSec/1000);
	while(!(m_signal & SignalMask))	// any bits in SignalMask must be set in Signal
	{
		if (MT_WaitMailbox(mSignalmbox,timeout.GetRestTime(),&mbsignal))
		{
			DISABLE_INTERRUPTS();
			m_signal |=mbsignal;
			RESTORE_INTERRUPTS();
		}
		if (timeout.HasExpired())			// abort if timeout
		{
			break;
		}
	}
	return m_signal;
}

// This function might get called from inside the thread, but most likely it gets called from an ISR
// See also the Fast version (inline, no disable IRQs)
void CThreadRealTime::Signal(ULONG SignalMask)
{
	MT_AddMailbox(mSignalmbox,&SignalMask);
}






