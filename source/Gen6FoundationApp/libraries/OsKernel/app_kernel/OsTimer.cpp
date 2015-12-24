//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: app_kernel/OsTimer.cpp $
// $Revision: 1.7 $
// $Date: 2009/06/01 14:47:23EDT $
//======================================================================================


#if PLATFORM_CURRENT == imx25
#include "HardwareMacros.h"		//Provides Macros for setting registers
#include "iMX25_GPT.h"			//General Purpose Timer (GPT) Definitions
#define OS_TIMER_IRQ_VEC VEC_GPT1_INT

#endif

#include "interrupts.h"
#include "db_time.h"
#include "OsTimer.h"
#define NUMTASKS 8

extern "C" void ServiceThreadTimers(void);	// thread system timer managment

static threadhandle_t OsTimerIrq(void *);
void (*OneMsTask[NUMTASKS+1])(void)={ServiceThreadTimers,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
volatile unsigned int ElapsedTimer1;		// 1 ms granularity timer to track elapsed time of operations
volatile unsigned int BaselineClock;		// 1 ms granularity Clock for use by the Decoder (and others?!)

// Return the value of BaselineClock (updated by heartbeat).
unsigned int read_ms_clock(void)
{
	return(BaselineClock);
} // read_ms_clock


// Return the difference between the current BaselineClock and a previous reading, adjusting for wraparound.
unsigned int elapsed_ms_clock_time( unsigned int ReferenceMsClock )
{
	unsigned int	CurrentMsClock = BaselineClock;
	return ( CurrentMsClock - ReferenceMsClock );
} // elapsed_ms_clock_time



// Zero the value of ElapsedTimer1 (updated by heartbeat).
void zero_timer(void)
{
	ElapsedTimer1 = 0;
	return;
} // read_timer


// Return the value of ElapsedTimer1 (updated by heartbeat).
unsigned int read_timer(void)
{
	return(ElapsedTimer1);
} // read_timer









void InitOsTimer(void)
{
	DisableInterruptNumber(OS_TIMER_IRQ_VEC);
#if PLATFORM_CURRENT == imx25

	WRITE_FIELD_TO_REG(GPT1_GPTIR,GPT_GPTIR_OF1IE_LSH,GPT_GPTIR_OF1IE_WID,1); //Output Compare 1 Interrupt Enable
#endif

	SetVector(OsTimerIrq,OS_TIMER_IRQ_VEC);
	BaselineClock=0;
	ElapsedTimer1=0;
	set_time_limit(1);
	EnableInterruptNumber(OS_TIMER_IRQ_VEC);
}

bool RegisterOneMsTask(void (*Task)(void))
{
	bool ret =false;
	DISABLE_INTERRUPTS();
	for (int i=0; i<NUMTASKS;i++)
	{
		if (OneMsTask[i]==NULL)
		{
			ret=true;
			OneMsTask[i]=Task;
			break;
		}
	}
	RESTORE_INTERRUPTS();
	return ret;
}

bool UnRegisterOneMsTask(void (*Task)(void))
{
	bool ret =false;
	DISABLE_INTERRUPTS();
	for (int i=0; i<NUMTASKS;i++)
	{
		if (OneMsTask[i]==Task)
		{
			ret=true;
		}
		if (ret)
		{
			OneMsTask[i]=OneMsTask[i+1];
		}
	}
	RESTORE_INTERRUPTS();
	return ret;
}

threadhandle_t OsTimerIrq(void *)
{
	set_time_limit(1);
	for (int i=0;i<NUMTASKS;i++)
	{
		if (OneMsTask[i])
		{
			OneMsTask[i]();
		}
		else
		{
			break;
		}
	}
	BaselineClock++;
	ElapsedTimer1++;
	return (threadhandle_t)0;
}
