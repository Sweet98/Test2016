/************************************************************************/
//
// HHP 4480 embedded firmware
//
//  Timing related things
//  Date:       2/24/02
//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: db_shared/db_time.c $
// $Revision: 1.1.2.18 $
// $Date: 2009/06/10 11:22:28EDT $
//======================================================================================
/************************************************************************/


#if PLATFORM_CURRENT == imx25
#include "HardwareMacros.h"		//Provides Macros for setting registers
#include "iMX25_GPT.h"			//General Purpose Timer (GPT) Definitions

	#ifdef RAMBASEDBOOT		//Boot Compiler SW that defines a fully iRAM based Boot for HW testing
		#define TICKS_PER_MS	1		//For the Ram Based Boot leave the time in uS and not mS
	#else
		#define TICKS_PER_MS	1024
	#endif //endof RAMBASEDBOOT 


#endif
#include "db_time.h"



// ===  Function  ======================================================================
//
//         Name:  init_timer
//
//  Description: Initializes GPT1
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void init_timer(void)
{
	//***** Configure and Start the General Purpose Timer (GTP1)*****
#if PLATFORM_CURRENT == imx25
	unsigned long ConfigTimerValue;

	//Reset and initialize Timer1
	HAZARD_WRITE_FIELD_TO_REG(GPT1_GPTCR, GPT_GPTCTL_SWR_LSH, GPT_GPTCTL_SWR_WID,1);							//Reset Timer 1
	while(READ_FIELD(GPT1_GPTCR, GPT_GPTCTL_SWR_LSH, GPT_GPTCTL_SWR_WID))
	{
		//Wait for the timer reset to complete This should take 5 clock cycles
	}
	HAZARD_WRITE_FIELD_TO_REG(GPT1_GPTPR, GPT_GPTPR_PRESCALER_LSH,GPT_GPTPR_PRESCALER_WID,32);		//Divide the input clock by 33 to run the timer at (133MHz/4/33) = 1.007575MHz.
	ConfigTimerValue = (FIELD_MASK(GPT_GPTCTL_FRR_LSH, GPT_GPTCTL_FRR_WID,1)|						//Run Free
	                    FIELD_MASK(GPT_GPTCTL_CLKSOURCE_LSH, GPT_GPTCTL_CLKSOURCE_WID,0x2)|			//Clock Source HighFreq - PERCLK (33.25MHz)
	                    FIELD_MASK(GPT_GPTCTL_WAITEN_LSH, GPT_GPTCTL_WAITEN_WID,0x1)|				//Allow the Timer to run in Wait Mode
	                    FIELD_MASK(GPT_GPTCTL_EN_LSH,GPT_GPTCTL_EN_WID,1));					//Enable IRQ
	HAZARD_WRITE_REG(GPT1_GPTCR,ConfigTimerValue);

#endif //endof PLATFORM_CURRENT
}

// ===  Function  ======================================================================
//
//         Name:  current_time
//
//  Description:  Reads the GPT1 Counter Value Register
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:  32bit					 	 GPT1 Counter Value Register
// -------------------------------------------------------------------------------------
unsigned int current_time(void)
{
#if PLATFORM_CURRENT == imx25
	return ((unsigned int) READ_REG(GPT1_GPTCNT)); // Timer 1 counter read is used to return a status on the current time TRUE;
#endif //endof PLATFORM_CURRENT == imx27
}//current_time


// ===  Function  ======================================================================
//
//         Name:  set_time_limit
//
//  Description:  Sets the Compare Value of the GPT1
//				  This is used to indicate how much time has pased for an event and
//					is needed because the GPT1 runs all the time.
//				  The input time is ms.  the function converts this time to "us"
//				  by doing a simple left shift (*1024 which is close to 1000) and places
//				  the time into the compare register of timer 1.  and clears any satus bits
//	Example:  set_time_limit(200) will set a time of about 200ms
//				Actual time is 203.26 with left shift
//				Actual time is 198.50 with *1000
//	Example:  set_time_limit(1) will set a time of about 1ms
//				Actual time is 1.018 with left shift
//				Actual time is 0.994 with *1000
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  unsigned int		time			The time to be loaded into the register
//												This time is in ms
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void set_time_limit(unsigned int time)
{
	unsigned int temp= (time * TICKS_PER_MS) + current_time();		//Convert input time to us add it to the current time value
#if PLATFORM_CURRENT == imx25

	HAZARD_WRITE_REG(GPT1_GPTOCR1,temp);
	HAZARD_WRITE_REG(GPT1_GPTSR,0x1);			//Clear any Status Bits (must write a 1 to clear it)
#endif //endof PLATFORM_CURRENT == imx27
}//set_time_limit



// ===  Function  ======================================================================
//
//         Name:  time_limit_expired
//
//  Description:  Checks to see if a compare event has occured by checking the GPT1 Flag
//				  This is used to indicate how much time has pased for an event
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:  unsigned int		TRUE/FALSE		Indicates if the compare even (flag) has happened
// -------------------------------------------------------------------------------------
unsigned int time_limit_expired(void)
{
#if PLATFORM_CURRENT == imx25
	if (READ_FIELD(GPT1_GPTSR,GPT_GPTSR_OF1_LSH,GPT_GPTSR_OF1_WID))
	{
		return true;
	}
	else
	{
		return false;
	}
#endif		//endof PLATFORM_CURRENT == imx27
}//time_limit_expired

// ===  Function  ======================================================================
//
//         Name:  translate_ticks_ms_to_ticks
//
//  Description:  	translates  number of mS given into timer 1 ticks
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  unsigned int		ms			# of ms
//    in-out:
//       out:
//    return:  unsigned int						# of ticks
// -------------------------------------------------------------------------------------
unsigned int translate_ms_ticks(unsigned int ms)
{
	return (TICKS_PER_MS*ms);          // should be in ms
}

// ===  Function  ======================================================================
//
//         Name:  translate_ticks_to_ms
//
//  Description:  	function to return the relative time in ms per a tick count
// 					tick count is based on 1MHz clock, so the tick
// 					1 tick = 1us, so 1 tick in ms = tick/1000
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  unsigned int		ticks			# of ticks
//    in-out:
//       out:
//    return:  unsigned int						# of ms
// -------------------------------------------------------------------------------------
unsigned int translate_ticks_to_ms(unsigned int ticks)
{
	return (ticks/TICKS_PER_MS);          // should be in ms
}
// ===  Function  ======================================================================
//
//         Name:  Sleep(SleepTimeInMilliseconds)
//
//  Description:  	Sleep for SleepTime milliseconds.
// 					This function should not be called!!
//					Use a thread safe function instad
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            			Description
// -------------------------------------------------------------------------------------
//        in:  unsigned int		SleepTimeInMilliseconds			Time to Sleep (in ms)
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
//void Sleep(UINT SleepTimeInMilliseconds)
//{
//// Convert to heart beat counts.
//	SleepTimer = (SleepTimeInMilliseconds * One_Second) / 1000;
//
//	while(SleepTimer)
//	{
//	// These operations are not needed, but they fool the optimizer into thinking that something is really going on.
//		if(kbhit())
//		{
//			++SleepTimeInMilliseconds;
//		}
//	}
//
//	return;
//}//Sleep

// THIS IS CALLED IN DBXM1K.C but does nothing !!!!
// not sure  dbmx1k.c is used anymore
void reset_heartbeat_timer(void)
{
	return;
}
// Return the number of heartbeat counts that equal one second.
unsigned int timer_granularity(void)
{
	return(1000);
} // timer_granularity


// Return the number of TCN1 counts that equal one second.
unsigned int hardware_timer_granularity(void)
{
	return(TICKS_PER_MS*1000);
} // hardware_timer_granularity


