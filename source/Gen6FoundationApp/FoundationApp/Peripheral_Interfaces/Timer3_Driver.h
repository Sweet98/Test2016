/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  Date:       1/30/2010
//  Tools:      RVCT Build 862
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Peripheral_Interfaces/Timer3_Driver.h $
// $Revision: 1.2 $
// $Date: 2010/02/10 09:57:46EST $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// HEADER:  Timer3_Driver.h
//
// 
//------------------------------------------------------------------------------
#ifndef TIMER3_DRIVER_H_
#define TIMER3_DRIVER_H_

#include "thread.h"

// ===  Function  ======================================================================
//
//         Name:  Timer3_Init
//
//  Description: Resets the Timer then configures the Control Register, events, and 
//					Interrupt
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void Timer3_Init(void);

// ===  Function  ======================================================================
//
//         Name:  MicroSecondDelay
//
//  Description:  Delays a specified number of microseconds.  
//				  This is a non blocking delay and will sleep the calling thread during 
//					the delay.
//
//				  Each timer has three compare channels to delay from.  This function 
//				   	chooses the first channel not in use for its delay.  When the channel 
//				   	is configured the function waits for the interrupt to flag the event.  
//
//				  If no channels are available the function will not delay.  If a channel 
//				    is available but the event is not triggered the wait event will time  
//					out at the next largest microsecond.  In ether failure a 0 will be 
//					returned
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  unsigned int		uS_Delay_Time	The time to be loaded into the register
//												This time is in us
//    in-out:
//       out:
//    return:  bool								1 = Successful delay
//												0 = Failed to delay at all or delayed longer then intended.
// -------------------------------------------------------------------------------------
bool Timer3_MicroSecondDelay(unsigned int);

// ===  Function  ======================================================================
//
//         Name:  Timer3_Get_Current_Time
//
//  Description:  Reads the GPT3 Counter Value Register
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:  32bit					 	 GPT3 Counter Value Register
// -------------------------------------------------------------------------------------
unsigned int Timer3_Get_Current_Time(void);

#endif /*TIMER3_DRIVER_H_*/

