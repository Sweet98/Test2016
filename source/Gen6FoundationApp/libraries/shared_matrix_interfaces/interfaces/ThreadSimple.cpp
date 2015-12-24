/*=================================================================================
  class: CThreadSimple

//=================================================================================
   $Source: interfaces/ThreadSimple.cpp $
   $Date: 2009/06/24 22:41:57EDT $
   $Revision: 1.1 $


//=================================================================================*/
//! \file

#include "stdInclude.h"
#include "product.h"
#include "Thread.h"
#include "ThreadSimple.h"
//#include "HwTrace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern "C" void MT_CloseDriverThread (ULONG handle);
//extern "C" void MT_DriverThreadSwap(void);
//ULONG MT_OpenDriverThread(int StackSize,void (*ThreadPtr)(void *pUniversal), void *pUniversal);

CThreadSimple::CThreadSimple(size_t stacksize)
{
	m_stacksize = stacksize;
	m_ThreadHandle = 0;
}

CThreadSimple::~CThreadSimple()
{
	Stop();
}

bool CThreadSimple::PrepareRun (int Priority,void (*std_action)(void *pUniversal), void *pUniversal)
{
	if(m_ThreadHandle == 0)		// precaution against starting it twice
	{
		m_ThreadHandle = MT_OpenDriverThread(Priority,m_stacksize, std_action, pUniversal);
	}
	return (m_ThreadHandle != 0);
}

bool CThreadSimple::PrepareRun (int Priority,CCallBackTranslator *pConnector)
{
	return PrepareRun (Priority,pConnector->GetTranslatedFunctionPointer(), pConnector->GetTranslatedThisPointer());
}

///////////////////////////////////////////////////////////////////////////////
//! Stops the thread.
/*!
 \return Always 1
*/
int CThreadSimple::Stop(void)
{
	if (m_ThreadHandle != 0)
	{
		MT_CloseDriverThread(m_ThreadHandle);
		m_ThreadHandle = 0;
	}
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
//! Give up timeslice imediately.
/*!
*/
/*
void CThreadSimple::Wait(void)
{
	MT_DriverThreadSwap();
}
*/

///////////////////////////////////////////////////////////////////////////////
//! This is deprecated.
/*!
 Use the MT_Event stuff instead.

 \param EnableThisVector A interrupt vector number
*/
/*
void CThreadSimple::WaitAndEnableInterrupt(vector_number_t EnableThisVector)
{
	// We need to avoid a race condition here.
	// The Interrupt could fire inbetween it's enable and the thread swap
	DISABLE_NORMAL_INTERRUPTS();	               // do not shut off the FIQ
	EnableInterruptNumber(EnableThisVector);
	MT_DriverThreadSwap();
	RESTORE_INTERRUPTS();
}
*/




