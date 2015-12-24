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
// $RCSfile: db_shared/Hardware_Header_Files/iMX27/iMX27_timers.h $
// $Revision: 1.1 $
// $Date: 2008/09/22 16:35:10EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx27_timers.h
//
// Provides definitions for the 6 general-purpose timers (GPT)
// Refer to Chapter 31 Page 1204 of the iMX27 Reference Manual
//
//------------------------------------------------------------------------------

#ifndef IMX27_TIMERS_H_
#define IMX27_TIMERS_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// REGISTERS
//------------------------------------------------------------------------------

// #########################################
// # GPT1                                  #
// # $1000_3000 to $1000_3FFF              #
// #########################################
#define GPT1_BASE_ADDR          0x10003000
#define GPT1_TCTL               (GPT1_BASE_ADDR+0x00)      // 32bit timer 1 control reg
#define GPT1_TPRER              (GPT1_BASE_ADDR+0x04)      // 32bit timer 1 prescaler reg
#define GPT1_TCMP               (GPT1_BASE_ADDR+0x08)      // 32bit timer 1 compare reg
#define GPT1_TCR                (GPT1_BASE_ADDR+0x0C)      // 32bit timer 1 capture reg
#define GPT1_TCN                (GPT1_BASE_ADDR+0x10)      // 32bit timer 1 counter reg
#define GPT1_TSTAT              (GPT1_BASE_ADDR+0x14)      // 32bit timer 1 status reg

// #########################################
// # GPT2                                  #
// # $1000_4000 to $1000_4FFF              #
// #########################################
#define GPT2_BASE_ADDR          0x10004000
#define GPT2_TCTL               (GPT2_BASE_ADDR+0x00)      // 32bit timer 2 control reg
#define GPT2_TPRER              (GPT2_BASE_ADDR+0x04)      // 32bit timer 2 prescaler reg
#define GPT2_TCMP               (GPT2_BASE_ADDR+0x08)      // 32bit timer 2 compare reg
#define GPT2_TCR                (GPT2_BASE_ADDR+0x0C)      // 32bit timer 2 capture reg
#define GPT2_TCN                (GPT2_BASE_ADDR+0x10)      // 32bit timer 2 counter reg
#define GPT2_TSTAT              (GPT2_BASE_ADDR+0x14)      // 32bit timer 2 status reg

// #########################################
// # GPT3                                  #
// # $1000_5000 to $1000_5FFF              #
// #########################################
#define GPT3_BASE_ADDR          0x10005000
#define GPT3_TCTL               (GPT3_BASE_ADDR+0x00)      // 32bit timer 3 control reg
#define GPT3_TPRER              (GPT3_BASE_ADDR+0x04)      // 32bit timer 3 prescaler reg
#define GPT3_TCMP               (GPT3_BASE_ADDR+0x08)      // 32bit timer 3 compare reg
#define GPT3_TCR                (GPT3_BASE_ADDR+0x0C)      // 32bit timer 3 capture reg
#define GPT3_TCN                (GPT3_BASE_ADDR+0x10)      // 32bit timer 3 counter reg
#define GPT3_TSTAT              (GPT3_BASE_ADDR+0x14)      // 32bit timer 3 status reg

// #########################################
// # GPT4                                  #
// # $1001_9000 to $1001_9FFF              #
// #########################################
#define GPT4_BASE_ADDR          0x10019000
#define GPT4_TCTL               (GPT4_BASE_ADDR+0x00)      // 32bit timer 4 control reg
#define GPT4_TPRER              (GPT4_BASE_ADDR+0x04)      // 32bit timer 4 prescaler reg
#define GPT4_TCMP               (GPT4_BASE_ADDR+0x08)      // 32bit timer 4 compare reg
#define GPT4_TCR                (GPT4_BASE_ADDR+0x0C)      // 32bit timer 4 capture reg
#define GPT4_TCN                (GPT4_BASE_ADDR+0x10)      // 32bit timer 4 counter reg
#define GPT4_TSTAT              (GPT4_BASE_ADDR+0x14)      // 32bit timer 4 status reg

// #########################################
// # GPT5                                  #
// # $1001_A000 to $1001_AFFF              #
// #########################################
#define GPT5_BASE_ADDR          0x1001A000
#define GPT5_TCTL               (GPT5_BASE_ADDR+0x00)      // 32bit timer 5 control reg
#define GPT5_TPRER              (GPT5_BASE_ADDR+0x04)      // 32bit timer 5 prescaler reg
#define GPT5_TCMP               (GPT5_BASE_ADDR+0x08)      // 32bit timer 5 compare reg
#define GPT5_TCR                (GPT5_BASE_ADDR+0x0C)      // 32bit timer 5 capture reg
#define GPT5_TCN                (GPT5_BASE_ADDR+0x10)      // 32bit timer 5 counter reg
#define GPT5_TSTAT              (GPT5_BASE_ADDR+0x14)      // 32bit timer 5 status reg

// #########################################
// # GPT6                                  #
// # $1001_F000 to $1001_FFFF              #
// #########################################
#define GPT6_BASE_ADDR          0x1001F000
#define GPT6_TCTL               (GPT6_BASE_ADDR+0x00)      // 32bit timer 6 control reg
#define GPT6_TPRER              (GPT6_BASE_ADDR+0x04)      // 32bit timer 6 prescaler reg
#define GPT6_TCMP               (GPT6_BASE_ADDR+0x08)      // 32bit timer 6 compare reg
#define GPT6_TCR                (GPT6_BASE_ADDR+0x0C)      // 32bit timer 6 capture reg
#define GPT6_TCN                (GPT6_BASE_ADDR+0x10)      // 32bit timer 6 counter reg
#define GPT6_TSTAT              (GPT6_BASE_ADDR+0x14)      // 32bit timer 6 status reg


//------------------------------------------------------------------------------
// REGISTER BIT FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
#define GPT_TCTL_TEN_LSH        0
#define GPT_TCTL_CLKSOURCE_LSH  1
#define GPT_TCTL_COMPEN_LSH     4
#define GPT_TCTL_CAPTEN_LSH     5
#define GPT_TCTL_CAP_LSH        6
#define GPT_TCTL_FRR_LSH        8
#define GPT_TCTL_OM_LSH         9
#define GPT_TCTL_CC_LSH         10
#define GPT_TCTL_SWR_LSH        15

#define GPT_TPRER_PRESCALER_LSH 0

#define GPT_TCMP_COMPARE_LSH    0

#define GPT_TCR_CAPTURE_LSH     0

#define GPT_TCN_COUNT_LSH       0

#define GPT_TSTAT_COMP_LSH      0
#define GPT_TSTAT_CAPT_LSH      1


//------------------------------------------------------------------------------
// REGISTER BIT FIELD WIDTHS
//------------------------------------------------------------------------------
#define GPT_TCTL_TEN_WID        Width_1
#define GPT_TCTL_CLKSOURCE_WID  Width_3
#define GPT_TCTL_COMPEN_WID     Width_1
#define GPT_TCTL_CAPTEN_WID     Width_1
#define GPT_TCTL_CAP_WID        Width_2
#define GPT_TCTL_FRR_WID        Width_1
#define GPT_TCTL_OM_WID         Width_1
#define GPT_TCTL_CC_WID         Width_1
#define GPT_TCTL_SWR_WID        Width_1

#define GPT_TPRER_PRESCALER_WID Width_10

#define GPT_TCMP_COMPARE_WID    Width_32

#define GPT_TCR_CAPTURE_WID     Width_32

#define GPT_TCN_COUNT_WID       Width_32

#define GPT_TSTAT_COMP_WID      Width_1
#define GPT_TSTAT_CAPT_WID      Width_1


//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------

// GPTCR
#define GPT_TCTL_TEN_ENABLE             1 // GPT enabled
#define GPT_TCTL_TEN_DISABLE            0 // GPT disabled

#define GPT_TCTL_CLKSOURCE_DISABLE      0 // clock disabled
#define GPT_TCTL_CLKSOURCE_PERCLK1      1 // PERCLK1 to prescaler
#define GPT_TCTL_CLKSOURCE_PERCLK1DIV4  2 // PERCLK1 divided by 4 to prescaler
#define GPT_TCTL_CLKSOURCE_TIN          3 // TIN to prescaler
#define GPT_TCTL_CLKSOURCE_32KCLK       7 // 32kHz clock to prescaler

#define GPT_TCTL_COMPEN_ENABLE          1 // Compare interrupt enabled
#define GPT_TCTL_COMPEN_DISABLE         0 // Compare interrupt disabled

#define GPT_TCTL_CAPTEN_ENABLE          1 // Capture interrupt enabled
#define GPT_TCTL_CAPTEN_DISABLE         0 // Capture interrupt disabled

#define GPT_TCTL_CAP_DISABLE            0 // Capture function disabled
#define GPT_TCTL_CAP_RISING             1 // Capture on rising edge
#define GPT_TCTL_CAP_FALLING            1 // Capture on falling edge
#define GPT_TCTL_CAP_BOTH               0 // Capture on both edges

#define GPT_TCTL_FRR_FREERUN            1 // Freerun mode (counter continues after compare)
#define GPT_TCTL_FRR_RESTART            0 // Restart mode (counter set to zero after compare)

#define GPT_TCTL_OM_ACTIVELOW           0 // Output active low pulse for 1 clock
#define GPT_TCTL_OM_TOGGLE              1 // Toggle output

#define GPT_TCTL_CC_ENABLE              1 // Counter reset when TEN = 0
#define GPT_TCTL_CC_DISABLE             0 // Counter halted at current count when TEN = 0

#define GPT_TCTL_SWR_RESET              1 // Self-clearing software reset
#define GPT_TCTL_SWR_NORESET            0 // Do not activate software reset

// TPRER
#define GPT_TPRER_PRESCALER_MAX         0x7FF

// TCN
#define GPT_TCN_COUNT_MAX               0xFFFFFFFF

// TSTAT
#define GPT_TSTAT_COMP_NOEVENT          0 // No compare event
#define GPT_TSTAT_COMP_EVENT            1 // A compare event has occurred
#define GPT_TSTAT_COMP_NORESET          0 // No reset
#define GPT_TSTAT_COMP_RESET            1 // Reset


#define GPT_TSTAT_CAPT_NOEVENT          0 // No capture event
#define GPT_TSTAT_CAPT_EVENT            1 // A capture event has occurred
#define GPT_TSTAT_CAPT_NORESET          0 // No reset
#define GPT_TSTAT_CAPT_RESET            1 // Reset

#endif /*IMX27_TIMERS_H_*/


