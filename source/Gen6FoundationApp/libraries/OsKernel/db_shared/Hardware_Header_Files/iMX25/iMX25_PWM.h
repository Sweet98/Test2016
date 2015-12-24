/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  HARDWARE DEFINITION FILE 
//  Date:       9/16/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: db_shared/Hardware_Header_Files/iMX25/iMX25_PWM.h $
// $Revision: 1.3 $
// $Date: 2011/06/28 13:26:43EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx25_pwm.h
//
// Provides definitions for the Pulse-Width Modulator (PWM)
// Refer to Chapter 40 of the iMX25 Reference Manual
//
//------------------------------------------------------------------------------
#ifndef IMX25_PWM_H_
#define IMX25_PWM_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------
// #########################################
// # PWM1                                  #
// #########################################
#define PWM1_BASE_ADDR             0x53FE0000U
#define PWM1_PWMCR                 (PWM1_BASE_ADDR+0x00U)      // 32bit pwm control reg
#define PWM1_PWMSR                 (PWM1_BASE_ADDR+0x04U)      // 32bit pwm status reg
#define PWM1_PWMIR                 (PWM1_BASE_ADDR+0x08U)      // 32bit pwm interrupt reg
#define PWM1_PWMSAR                (PWM1_BASE_ADDR+0x0CU)      // 32bit pwm sample reg
#define PWM1_PWMPR                 (PWM1_BASE_ADDR+0x10U)      // 32bit pwm period reg
#define PWM1_PWMCNR                (PWM1_BASE_ADDR+0x14U)      // 32bit pwm counter reg

// #########################################
// # PWM2                                  #
// #########################################
#define PWM2_BASE_ADDR             0x53FA0000U
#define PWM2_PWMCR                 (PWM2_BASE_ADDR+0x00U)      // 32bit pwm control reg
#define PWM2_PWMSR                 (PWM2_BASE_ADDR+0x04U)      // 32bit pwm status reg
#define PWM2_PWMIR                 (PWM2_BASE_ADDR+0x08U)      // 32bit pwm interrupt reg
#define PWM2_PWMSAR                (PWM2_BASE_ADDR+0x0CU)      // 32bit pwm sample reg
#define PWM2_PWMPR                 (PWM2_BASE_ADDR+0x10U)      // 32bit pwm period reg
#define PWM2_PWMCNR                (PWM2_BASE_ADDR+0x14U)      // 32bit pwm counter reg

// #########################################
// # PWM3                                  #
// #########################################
#define PWM3_BASE_ADDR             0x53FA8000U
#define PWM3_PWMCR                 (PWM3_BASE_ADDR+0x00U)      // 32bit pwm control reg
#define PWM3_PWMSR                 (PWM3_BASE_ADDR+0x04U)      // 32bit pwm status reg
#define PWM3_PWMIR                 (PWM3_BASE_ADDR+0x08U)      // 32bit pwm interrupt reg
#define PWM3_PWMSAR                (PWM3_BASE_ADDR+0x0CU)      // 32bit pwm sample reg
#define PWM3_PWMPR                 (PWM3_BASE_ADDR+0x10U)      // 32bit pwm period reg
#define PWM3_PWMCNR                (PWM3_BASE_ADDR+0x14U)      // 32bit pwm counter reg

// #########################################
// # PWM4                                  #
// #########################################
#define PWM4_BASE_ADDR             0x53FC8000U
#define PWM4_PWMCR                 (PWM4_BASE_ADDR+0x00U)      // 32bit pwm control reg
#define PWM4_PWMSR                 (PWM4_BASE_ADDR+0x04U)      // 32bit pwm status reg
#define PWM4_PWMIR                 (PWM4_BASE_ADDR+0x08U)      // 32bit pwm interrupt reg
#define PWM4_PWMSAR                (PWM4_BASE_ADDR+0x0CU)      // 32bit pwm sample reg
#define PWM4_PWMPR                 (PWM4_BASE_ADDR+0x10U)      // 32bit pwm period reg
#define PWM4_PWMCNR                (PWM4_BASE_ADDR+0x14U)      // 32bit pwm counter reg

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
#define   PWM_SR_FWE_LSH            5

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
#define   PWM_SR_FWE_WID            Width_1

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


#endif /*IMX25_PWM_H_*/


