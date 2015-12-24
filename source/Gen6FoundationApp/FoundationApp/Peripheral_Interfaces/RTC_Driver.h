/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  Date:       7/22/2009
//  Tools:      RVCT Build 739
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Peripheral_Interfaces/RTC_Driver.h $
// $Revision: 1.1 $
// $Date: 2009/07/24 16:31:58EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// HEADER:  RTC_Driver.h
//
// Real Time Clock Functions are defined here
//------------------------------------------------------------------------------
#ifndef RTC_DRIVER_H_
#define RTC_DRIVER_H_

// ===  Function  ======================================================================
//
//  Name:  ZeroRTC
//
//  Description: Zeros all registers.
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:
//       out:
//    return:	
// -------------------------------------------------------------------------------------
void ZeroRTC(void);

// ===  Function  ======================================================================
//
//  Name:  PrepareRTC
//
//  Description: Initializes the Real Time Clock
//		Setup and start the real time clock to wake us when the power down timeout 
//		would have expired
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  unsigned long   time				The Time to wait while in Stop Mode (Seconds)
//    in-out:
//       out:
//    return:	
// -------------------------------------------------------------------------------------
void PrepareRTC(unsigned long);

// ===  Function  ======================================================================
//
//  Name:  ClearRTC
//
//  Description: 
// 		This routine is also called by IOMAN whenever there is IO activity 
// 		Resets both timers regardless of which triggermode we are actually using	
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:
//       out:
//    return:	
// -------------------------------------------------------------------------------------
void ClearRTC(void);


// ===  Function  ======================================================================
//
//  Name:  GetRTCSeconds
//
//  Description: Returns the Current Time in Seconds
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:
//       out:
//    return:  int								Current time in Seconds
// -------------------------------------------------------------------------------------
int GetRTCSeconds(void);

// ===  Function  ======================================================================
//
//  Name:  GetRTC_ALM_ISR_Status
//
//  Description: Returns the Status bit of the Alarm ISR.
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:
//       out:
//    return:  bool								Alarm ISR Status
// -------------------------------------------------------------------------------------
bool GetRTC_ALM_ISR_Status(void);

#endif /*RTC_DRIVER_H_*/

