/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  Date:       8/10/2009
//  Tools:      RVCT Build 739
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Peripheral_Interfaces/IndicatorLEDs.h $
// $Revision: 1.3 $
// $Date: 2010/01/05 15:56:49EST $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// HEADER:  IndicatorLEDs.h
//
// Indicator LED Functions are defined here
//------------------------------------------------------------------------------
#ifndef INDICATORLEDS_H_
#define INDICATORLEDS_H_

void Init_IndicatorLEDs(void);
void GoodReadLedOff(void);
void ErrorLedOff(void);
void GoodReadLedOn(void);
void ErrorLedOn(void);
bool IsErrorLedOn(void);
bool IsGoodReadOn(void);

//These allow synchronization between the indicator LEDs and the Beeper
void Force_Green_LED(bool);  //DO NOT USE THIS FUNCTION
void Force_Red_LED(bool);    //DO NOT USE THIS FUNCTION
void BeeperUnLockLEDS(void); //DO NOT USE THIS FUNCTION
void BeeperLockLEDS(void);	 //DO NOT USE THIS FUNCTION

#endif /*INDICATORLEDS_H_*/

