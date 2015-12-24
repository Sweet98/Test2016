#ifndef HWTRIGGER_H_
#define HWTRIGGER_H_
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Trigger/HWTrigger.h $
// $Revision: 1.6 $
// $Date: 2011/10/25 13:35:03EDT $
//======================================================================================
//======================================================================================

//======================================================================================
#include "HardwareToken.h"


class CHWTrigger
{
public:
	static int CreateHWTrigger(void);		// call once in main to create this class
	static void DestroyHWTrigger(void);		// call when destroying this class (never usually)

	virtual bool IsTriggerPulled(void); 	// trigger pressed debounced true = pressed
	virtual bool ReadReedSwitch(void); 		// check state of the Reed Switch
	virtual void PrepareForStop(void);  	// set up irq to wake up from stop mode
	virtual void UnPrepareForStop(void); 	// shutdown irq after waking up from stop
protected:
	CHWTrigger(unsigned int MyDebounceTimeMs) : m_MyDebounceTimeMs(MyDebounceTimeMs)
	{
		m_bPrevTrigger=false;
		m_Triggered=false;
		m_TriggerButtonTimer = MyDebounceTimeMs;
		bExtFlo = false;
#if defined(SM4100)
		m_start_time = 0;
#endif
	};
	virtual ~CHWTrigger(){};

	const unsigned int m_MyDebounceTimeMs;
	static void DoDebounce(void); // wrapper for irq function
	virtual bool IsHwTrigger(void); // raw not debounced trigger
	virtual int SetUpHardware(void); // called by CreateHWTrigger to set up hw pins

	virtual void DebounceTriggerButton(void);
	bool m_bPrevTrigger;
	bool m_Triggered;
	volatile unsigned int m_TriggerButtonTimer;
#if defined(SM4100)
	int	 m_start_time;
#endif
	bool isIntEnabled;
	bool bExtFlo;
};
extern CHWTrigger *pCHWTrigger;


#endif /*HWTRIGGER_H_*/
