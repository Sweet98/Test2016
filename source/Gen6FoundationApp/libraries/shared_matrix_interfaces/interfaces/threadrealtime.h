/*=================================================================================
  class: CThreadRealTime
	This class is to abstract the low level threading code while adding timing features.
	If you only need some kind of a defered procedure call mechanism, then you can
 	probably use the CThreadSimple class.
 
//=================================================================================
   $RCSfile: interfaces/threadrealtime.h $
   $Date: 2011/11/14 13:33:47EST $
   $Revision: 1.14.2.4 $
 

//=================================================================================*/

#ifndef THREAD_REALTIME_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define THREAD_REALTIME_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include <stddef.h>
#include "thread.h"
class CCallBackTranslator;
class CThreadRealTime
{
public:
	CThreadRealTime(size_t stacksize);
	~CThreadRealTime();
	threadhandle_t GetThreadHandle(void)
	{
		return mThreadHandle;
	}
	bool PrepareRun (int Priority,void (*std_action)(void *pUniversal), void *pUniversal,bool CreateSignal=true);
	bool PrepareRun (int Priority,CCallBackTranslator *pConnector,bool CreateSignal=true);
	bool PrepareRun (int Priority,CCallBackTranslator &Connector,bool CreateSignal=true)
	{
		return Run (Priority,&Connector,CreateSignal);
	}

	bool Run (int Priority,void (*std_action)(void *pUniversal), void *pUniversal,bool CreateSignal=true);
	bool Run (int Priority,CCallBackTranslator *pConnector,bool CreateSignal=true);
	bool Run (int Priority,CCallBackTranslator &Connector,bool CreateSignal=true)
	{
		return Run (Priority,&Connector,CreateSignal);
	}

	int Stop(void);


	void Sleep(timer_uSec_t uSec);

	void PrepareWaitForSignal(unsigned long SignalMask);
	unsigned long WaitForAnySignal(unsigned long SignalMask);
	unsigned long WaitForAnySignal(unsigned long SignalMask, unsigned long uSec);
	unsigned long GetSignal(void)
	{
		return m_signal;
	}
	void Signal(unsigned long SignalMask);



protected:
	volatile unsigned long	 m_signal;
	threadhandle_t mThreadHandle;
	size_t m_StackSize;
	MT_Mailbox_t mSignalmbox;

	CThreadRealTime (const CThreadRealTime&);            // declared private, but not defined, to prevent copying         
	CThreadRealTime& operator=(const CThreadRealTime&);  // declared private, but not defined, to prevent copying 

};

#endif /* THREAD_REALTIME_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */





