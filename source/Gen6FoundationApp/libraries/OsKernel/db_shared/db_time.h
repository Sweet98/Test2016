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
// $RCSfile: db_shared/db_time.h $
// $Revision: 1.1.2.5 $
// $Date: 2009/01/14 12:50:29EST $
//======================================================================================
/************************************************************************/
#ifndef __DB_TIME_H            /*  prevent redundant definitions   */
#define __DB_TIME_H  1

#include "language.h"

void set_time_limit(unsigned int);
unsigned int time_limit_expired(void);
void init_timer(void);
unsigned int translate_ticks_to_ms(unsigned int ticks);
unsigned int current_time(void);

#define	One_Second			100

/* timer prototypes */
// Return the value of BaselineClock (updated by heartbeat).
unsigned int read_ms_clock(void);

// Return the difference between the current BaselineClock and a previous reading, adjusting for wraparound.
unsigned int elapsed_ms_clock_time( unsigned int /* ReferenceMsClock */ );


void zero_timer(void);								// Zero the value of ElapsedTimer1 (updated by heartbeat)
unsigned int read_timer(void);					// Return the value of ElapsedTimer1 (updated by heartbeat)
unsigned int timer_granularity(void);			// Return the number of heartbeat counts equaling one second
unsigned int hardware_timer_granularity(void);// Return the number of TCN1 counts equaling one second
void Sleep(UINT SleepTimeInMilliseconds);
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
unsigned int translate_ms_ticks(unsigned int ms);


extern volatile UINT	ElapsedTimer1;							// 10 ms granularity timer to track elapsed time of operations
extern volatile UINT	SerialPortTimer;						// 10 ms granularity timer for serial port operations
extern volatile UINT	SerialPortIntercharacterReceiveTimer;	// 10 ms granularity timer for tracking time between serial chars received
extern volatile UINT	SleepTimer;								// 10 ms granularity timer to track process sleep time
extern volatile UINT	BaselineClock;		// a 1 ms granularity Clock for use by the Decoder (and others?!)


#endif                                  // __DB_TIME_H


