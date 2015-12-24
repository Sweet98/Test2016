/**************************************************************************************/
//
// HIM 48X0G GENERATION 6 SOFTWARE
//
//  HARDWARE DEFINITION FILE 
//  Date:       5/09/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX27
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: db_shared/Hardware_Header_Files/iMX27/iMX27_pwm.h $
// $Revision: 1.1 $
// $Date: 2008/09/22 16:35:09EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx27_pwm.h
//
// Provides definitions for the Pulse-Width Modulator (PWM)
// Refer to Chapter 32 Page 1218 of the iMX27 Reference Manual
//
//------------------------------------------------------------------------------
#ifndef IMX27_PWM_H_
#define IMX27_PWM_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------
// #########################################
// # PWM                                   #
// # $1000_6000 to $1000_6FFF              #
// #########################################
#define PWM_BASE_ADDR            0x10006000
#define PWM_PWMC                 (PWM_BASE_ADDR+0x00)       // 32bit pwm control reg
#define PWM_PWMS                 (PWM_BASE_ADDR+0x04)       // 32bit pwm sample reg
#define PWM_PWMI                 (PWM_BASE_ADDR+0x08)       // 32bit pwm interrupt reg
#define PWM_PWMSA                (PWM_BASE_ADDR+0x0C)       // 32bit pwm sample reg
#define PWM_PWMP                 (PWM_BASE_ADDR+0x10)       // 32bit pwm period reg
#define PWM_PWMCNT               (PWM_BASE_ADDR+0x14)       // 32bit pwm counter reg


//------------------------------------------------------------------------------
// REGISTER FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
#define   PWM_CR_EN_LSH             0
#define   PWM_CR_REPEAT_LSH         1
#define   PWM_CR_SWR_LSH            3
#define   PWM_CR_PRESCALER_LSH      4
#define   PWM_CR_CLKSRC_LSH         16
#define   PWM_CR_POUTC_LSH          18
#define   PWM_CR_HCTR_LSH           20
#define   PWM_CR_BCTR_LSH           21
#define   PWM_CR_DBGEN_LSH          22
#define   PWM_CR_WAITEN_LSH         23
#define   PWM_CR_DOZEN_LSH          24
#define   PWM_CR_STOPEN_LSH         25
#define   PWM_CR_FWM_LSH            26

#define   PWM_SR_FIFOAV_LSH         0
#define   PWM_SR_FE_LSH             3
#define   PWM_SR_ROV_LSH            4
#define   PWM_SR_CMP_LSH            5

#define  PWM_IR_FIE_LSH             0
#define  PWM_IR_RIE_LSH             1
#define  PWM_IR_CIE_LSH             2



//------------------------------------------------------------------------------
// REGISTER FIELD WIDTHS
//------------------------------------------------------------------------------

#define   PWM_CR_EN_WID             Width_1
#define   PWM_CR_REPEAT_WID         Width_2
#define   PWM_CR_SWR_WID            Width_1
#define   PWM_CR_PRESCALER_WID      Width_12
#define   PWM_CR_CLKSRC_WID         Width_2
#define   PWM_CR_POUTC_WID          Width_2
#define   PWM_CR_HCTR_WID           Width_1
#define   PWM_CR_BCTR_WID           Width_1
#define   PWM_CR_DBGEN_WID          Width_1
#define   PWM_CR_WAITEN_WID         Width_1
#define   PWM_CR_DOZEN_WID          Width_1
#define   PWM_CR_STOPEN_WID         Width_1
#define   PWM_CR_FWM_WID            Width_2

#define   PWM_SR_FIFOAV_WID         Width_3
#define   PWM_SR_FE_WID             Width_1
#define   PWM_SR_ROV_WID            Width_1
#define   PWM_SR_CMP_WID            Width_1

#define   PWM_IR_FIE_WID            Width_1
#define   PWM_IR_RIE_WID            Width_1
#define   PWM_IR_CIE_WID            Width_1


//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------
#define   PWM_CR_EN_ENABLE          1
#define   PWM_CR_EN_DISABLE         0

#define   PWM_CR_SWR_RESET          1
#define   PWM_CR_SWR_NORESET        0

#define  PWM_CR_HCTR_ENABLE         1
#define  PWM_CR_HCTR_DISABLE        0

#define   PWM_CR_BCTR_ENABLE        1
#define   PWM_CR_BCTR_DISABLE       0

#define  PWM_CR_DBGEN_ACTIVE        1
#define  PWM_CR_DBGEN_INACTIVE      0

#define  PWM_CR_WAITEN_ACTIVE       1
#define  PWM_CR_WAITEN_INACTIVE     0

#define  PWM_CR_DOZEN_ACTIVE        1
#define  PWM_CR_DOZEN_INACTIVE      0

#define  PWM_CR_STOPEN_ACTIVE       1
#define  PWM_CR_STOPEN_INACTIVE     0

#define PWM_IR_CIE_ENABLE           1
#define PWM_IR_CIE_DISABLE          0
#define PWM_IR_RIE_ENABLE           1
#define PWM_IR_RIE_DISABLE          0
#define PWM_IR_FIE_ENABLE           1
#define PWM_IR_FIE_DISABLE          0

#define PWM_SR_CMP_STATUS_CLEAR     1  //  status clear
#define PWM_SR_ROV_STATUS_CLEAR     1  //  status clear
#define PWM_SR_FE_STATUS_CLEAR      1  //  status clear


#endif /*IMX27_PWM_H_*/


