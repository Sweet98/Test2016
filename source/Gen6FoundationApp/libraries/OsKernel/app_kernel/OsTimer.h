#ifndef OSTIMER_H_
#define OSTIMER_H_
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: app_kernel/OsTimer.h $
// $Revision: 1.1 $
// $Date: 2009/04/05 12:06:12EDT $
//======================================================================================
// Return the value of ElapsedTimer1 (updated by heartbeat).
unsigned int read_timer(void);


// Return the value of BaselineClock (updated by heartbeat).
unsigned int read_ms_clock(void);

// Zero the value of ElapsedTimer1 (updated by heartbeat). 
void zero_timer(void);

// Return the difference between the current BaselineClock and a previous reading, adjusting for wraparound.
unsigned int elapsed_ms_clock_time( unsigned int ReferenceMsClock );

//initialize os timer and set up irq etc
void InitOsTimer(void);

// register a task to be run every 1 mS this task runs in irq mode with limited stack 
// 7 tasks are available returns false if no more available
bool RegisterOneMsTask(void(*)(void));

// unregister a task registered above returns false if not in list
bool UnRegisterOneMsTask(void(*)(void));

#endif
