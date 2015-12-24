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
// $RCSfile: db_shared/Hardware_Header_Files/iMX27/iMX27_wdog.h $
// $Revision: 1.1 $
// $Date: 2008/09/22 16:35:12EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  iMX27_wdog.h
//
// Provides definitions for The Watchdog (WDOG) timer
// Refer to Chapter 34 Page 1254 of the iMX27 Reference Manual
//
//------------------------------------------------------------------------------
#ifndef IMX27_WDOG_H_
#define IMX27_WDOG_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------
// #########################################
// # WDOG                                  #
// # $1000_2000 to $1000_2FFF              #
// #########################################
#define WDOG_BASE_ADDR           0x10002000
#define WDOG_WCR                 (WDOG_BASE_ADDR+0x00)      // 16bit watchdog control reg
#define WDOG_WSR                 (WDOG_BASE_ADDR+0x02)      // 16bit watchdog service reg
#define WDOG_WRSR                (WDOG_BASE_ADDR+0x04)      // 16bit watchdog reset status reg


//------------------------------------------------------------------------------
// REGISTER BIT FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
#define WDOG_WCR_WDZST_LSH     0
#define WDOG_WCR_WDBG_LSH      1
#define WDOG_WCR_WDE_LSH       2
#define WDOG_WCR_WRE_LSH       3
#define WDOG_WCR_SRS_LSH       4
#define WDOG_WCR_WDA_LSH       5
#define WDOG_WCR_WT_LSH        8

#define WDOG_WSR_WSR_LSH       0

#define WDOG_WRSR_SFTW_LSH     0
#define WDOG_WRSR_TOUT_LSH     1
#define WDOG_WRSR_EXT_LSH      3
#define WDOG_WRSR_PWR_LSH      4



//------------------------------------------------------------------------------
// REGISTER BIT FIELD WIDTHS
//------------------------------------------------------------------------------
#define WDOG_WCR_WDZST_WID     Bit16_Width_1
#define WDOG_WCR_WDBG_WID      Bit16_Width_1
#define WDOG_WCR_WDE_WID       Bit16_Width_1
#define WDOG_WCR_WRE_WID       Bit16_Width_1
#define WDOG_WCR_SRS_WID       Bit16_Width_1
#define WDOG_WCR_WDA_WID       Bit16_Width_1
#define WDOG_WCR_WT_WID        Bit16_Width_8

#define WDOG_WSR_WSR_WID       Bit16_Width_16

#define WDOG_WRSR_SFTW_WID     Bit16_Width_1
#define WDOG_WRSR_TOUT_WID     Bit16_Width_1
#define WDOG_WRSR_EXT_WID      Bit16_Width_1
#define WDOG_WRSR_PWR_WID      Bit16_Width_1

//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------

#define WDOG_WCR_WDZST_CONTINUE    0
#define WDOG_WCR_WDZST_SUSPEND     1

#define WDOG_WCR_WDBG_CONTINUE     0
#define WDOG_WCR_WDBG_SUSPEND      1

#define WDOG_WCR_WDE_DISABLE       0
#define WDOG_WCR_WDE_ENABLE        1

#define WDOG_WCR_WRE_SIG_RESET     0
#define WDOG_WCR_WRE_SIG_TIMEOUT   1

#define WDOG_WCR_SRS_NOEFFECT      1
#define WDOG_WCR_SRS_ASSERT_RESET  0

#define WDOG_WCR_WDA_ASSERT_TIMEOUT 0
#define WDOG_WCR_WDA_NOEFFECT      1

#define WDOG_WSR_WSR_RELOAD1       0x5555
#define WDOG_WSR_WSR_RELOAD2       0xAAAA

#define WDOG_WRSR_SFTW_NORESET     0
#define WDOG_WRSR_SFTW_RESET       1

#define WDOG_WRSR_TOUT_NORESET     0
#define WDOG_WRSR_TOUT_RESET       1

#define WDOG_WRSR_EXT_NORESET      0
#define WDOG_WRSR_EXT_RESET        1

#define WDOG_WRSR_PWR_NORESET      0
#define WDOG_WRSR_PWR_RESET        1

#define WDOG_WCR_WT_MASK           0xFF


#endif /*IMX27_WDOG_H_*/




