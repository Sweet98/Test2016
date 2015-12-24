/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  Date:       10/31/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Foundation_Temp/PowerManagement.h $
// $Revision: 1.21 $
// $Date: 2011/11/10 03:22:25EST $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// HEADER:  PowerManagement.h
//
//  This module contains a collection of low level functions that control the
//		power management for the processor and scanner.
//
//------------------------------------------------------------------------------
#ifndef POWERMANAGEMENT_H_
#define POWERMANAGEMENT_H_

extern volatile bool ProcessingAppCode;
extern volatile bool gOkToPowerDown;
extern "C" void WaitForIRQ(void);

typedef enum {FULL,DOZE,DOZE_LOW,STOP,POWERDOWN,STOPTHENPOWERDOWN}power_t;

void ResetMinimumPowerUp(void);
void ResetPowerTimeOuts(void);
void EnterPowerState(power_t);
void ManageSystemPower(void);
void EnterStopMode(void);
void Prepare_System_for_Suspend(void);
void Restore_System_from_Suspend(void);
void PSMODE_Control(bool);
void Init_PowerPins(void);
void SystemSuspendState(bool);

#if defined(VUQUEST3310) || defined (HI2D_T)
//add by kehe@2011.08.29 for ESD issue
void reset_res_count_and_timout();
bool is_ignore_suspend();
#endif
#endif /*POWERMANAGEMENT_H_*/


