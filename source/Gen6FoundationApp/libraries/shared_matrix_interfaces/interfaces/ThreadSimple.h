/*=================================================================================
  class: CThreadSimple
	This class is to abstract the low level threading code while still be a thin wrapper.
	It shall be easier to use than the low level stuff.
	Use it to create some kind of a defered procedure call mechanism.
 	If you need timing features, then consider to use the CThreadRealTime class.
	See the usage example at the need of this file.
//=================================================================================
   $Source: interfaces/ThreadSimple.h $
   $Date: 2009/06/24 22:41:58EDT $
   $Revision: 1.1 $


//=================================================================================*/

#ifndef THREAD_SIMPLE_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define THREAD_SIMPLE_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

//! \file
#include "interrupts.h"
#include "CallBackhelper.h"
#include "thread.h"
///////////////////////////////////////////////////////////////////////////////
//! This class is to abstract the low level threading code while still be a thin wrapper.
/*
	It shall be easier to use than the low level stuff.
	Use it to create some kind of a defered procedure call mechanism.
 	If you need timing features, then consider to use the CThreadRealTime class.
*/
class CThreadSimple
{
public:
	CThreadSimple(size_t stacksize=2000);
	~CThreadSimple();

	// some overloaded run functions
	bool PrepareRun (int Priority,void (*std_action)(void *pUniversal), void *pUniversal);
	bool PrepareRun (int Priority,CCallBackTranslator *pConnector);
	bool PrepareRun (int Priority,CCallBackTranslator &Connector)
	{
		return PrepareRun (Priority,&Connector);
	}

	int Stop(void);

//	void Wait(void);
//	void WaitAndEnableInterrupt(vector_number_t EnableThisVector);

	threadhandle_t GetThreadHandle(void)
	{
		return m_ThreadHandle;
	}
protected:
public:
	CREATE_SETVECTOR_MEMBER(CThreadSimple);

protected:
	size_t m_stacksize;
	threadhandle_t m_ThreadHandle;
};

/*! \class  CThreadSimple
\verbatim
class YourClass
{
 ....

	CThreadSimple m_Thread;
};

void YourClass::DeferedProcedureCall(void)
{
	while (TRUE)					// This is the thread loop
	{
		// Do some work and then sleep again
		m_Thread.Wait();			// sleep until our ISR wakes us up
	}
}

HANDLE YourClass::ISR(void)
{
	....
	return m_Thread.GetThreadHandle();	// switch context to run DeferedProcedureCall()
}
\endverbatim

CAUTION: It is your responsibility that the interrupt does not
 			fire while you are in the DeferedProcedureCall !!
*/

#endif /* THREAD_SIMPLE_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */





