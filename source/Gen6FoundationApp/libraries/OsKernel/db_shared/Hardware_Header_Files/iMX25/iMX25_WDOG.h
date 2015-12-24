/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  HARDWARE DEFINITION FILE 
//  Date:       9/12/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: db_shared/Hardware_Header_Files/iMX25/iMX25_WDOG.h $
// $Revision: 1.3 $
// $Date: 2011/06/28 13:26:51EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  iMX25_wdog.h
//
// Provides definitions for The Watchdog (WDOG) timer
// Refer to Chapter 47 of the iMX25 Reference Manual
//
//------------------------------------------------------------------------------
#ifndef IMX25_WDOG_H_
#define IMX25_WDOG_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------
// #########################################
// # WDOG                                  #
// # $1000_2000 to $1000_2FFF              #
// #########################################
#define WDOG_BASE_ADDR             0x53FDC000U
#define WDOG_WCR                   (WDOG_BASE_ADDR+0x00U)      // 16bit Watchdog Control Register (WCR)
#define WDOG_WSR                   (WDOG_BASE_ADDR+0x02U)      // 16bit Watchdog Service Register (WSR)
#define WDOG_WRSR                  (WDOG_BASE_ADDR+0x04U)      // 16bit Watchdog Reset Status Register (WRSR)
#define WDOG_WICR                  (WDOG_BASE_ADDR+0x06U)      // 16bit Watchdog Interrupt Control Register (WICR)
#define WDOG_WMCR                  (WDOG_BASE_ADDR+0x08U)      // 16bit Watchdog Miscellaneous Control Register (WMCR)

//------------------------------------------------------------------------------
// REGISTER BIT FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
//Watchdog Control Register (WCR)
#define WDOG_WCR_WDZST_LSH     0
#define WDOG_WCR_WDBG_LSH      1
#define WDOG_WCR_WDE_LSH       2
#define WDOG_WCR_WDT_LSH       3
#define WDOG_WCR_SRS_LSH       4
#define WDOG_WCR_WDA_LSH       5
#define WDOG_WCR_WDW_LSH       7
#define WDOG_WCR_WT_LSH        8

//Watchdog Service Register (WSR)
#define WDOG_WSR_WSR_LSH       0

//Watchdog Reset Status Register (WRSR)
#define WDOG_WRSR_SFTW_LSH     0
#define WDOG_WRSR_TOUT_LSH     1

//Watchdog Interrupt Control Register (WICR)
#define WDOG_WICR_WICT_LSH     0
#define WDOG_WICR_WTIS_LSH     14
#define WDOG_WICR_WIE_LSH      15

//Watchdog Miscellaneous Control Register (WMCR)
#define WDOG_WMCR_PDE_LSH     0

//------------------------------------------------------------------------------
// REGISTER BIT FIELD WIDTHS
//------------------------------------------------------------------------------
//Watchdog Control Register (WCR)
#define WDOG_WCR_WDZST_WID     Bit16_Width_1
#define WDOG_WCR_WDBG_WID      Bit16_Width_1
#define WDOG_WCR_WDE_WID       Bit16_Width_1
#define WDOG_WCR_WDT_WID       Bit16_Width_1
#define WDOG_WCR_SRS_WID       Bit16_Width_1
#define WDOG_WCR_WDA_WID       Bit16_Width_1
#define WDOG_WCR_WDW_WID       Bit16_Width_1
#define WDOG_WCR_WT_WID        Bit16_Width_8

//Watchdog Service Register (WSR)
#define WDOG_WSR_WSR_WID       Bit16_Width_16

//Watchdog Reset Status Register (WRSR)
#define WDOG_WRSR_SFTW_WID     Bit16_Width_1
#define WDOG_WRSR_TOUT_WID     Bit16_Width_1

//Watchdog Interrupt Control Register (WICR)
#define WDOG_WICR_WICT_WID     Bit16_Width_8
#define WDOG_WICR_WTIS_WID     Bit16_Width_1
#define WDOG_WICR_WIE_WID      Bit16_Width_1

//Watchdog Miscellaneous Control Register (WMCR)
#define WDOG_WMCR_PDE_WID      Bit16_Width_1



//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------

//Watchdog Control Register (WCR)
#define WDOG_WCR_WDZST_CONTINUE    0
#define WDOG_WCR_WDZST_SUSPEND     1

#define WDOG_WCR_WDBG_CONTINUE     0
#define WDOG_WCR_WDBG_SUSPEND      1

#define WDOG_WCR_WDE_DISABLE       0
#define WDOG_WCR_WDE_ENABLE        1

#define WDOG_WCR_WDT_NOEFFECT      0
#define WDOG_WCR_WDT_DISABLE       1

#define WDOG_WCR_SRS_NOEFFECT      1
#define WDOG_WCR_SRS_ASSERT_RESET  0

#define WDOG_WCR_WDA_ASSERT_TIMEOUT 0
#define WDOG_WCR_WDA_NOEFFECT      1

#define WDOG_WCR_WDW_CONTINUE	   0
#define WDOG_WCR_WDW_SUSAPEND      1

//Watchdog Service Register (WSR)
#define WDOG_WSR_WSR_RELOAD1       0x5555
#define WDOG_WSR_WSR_RELOAD2       0xAAAA

//Watchdog Reset Status Register (WRSR)
#define WDOG_WRSR_SFTW_NORESET     0
#define WDOG_WRSR_SFTW_RESET       1

#define WDOG_WRSR_TOUT_NORESET     0
#define WDOG_WRSR_TOUT_RESET       1

#define WDOG_WRSR_EXT_NORESET      0
#define WDOG_WRSR_EXT_RESET        1

#define WDOG_WRSR_PWR_NORESET      0
#define WDOG_WRSR_PWR_RESET        1

//Watchdog Interrupt Control Register (WICR)
#define WDOG_WIE_WIE_DISABLE       0
#define WDOG_WIE_WIE_ENABLE        1

//Watchdog Miscellaneous Control Register (WMCR)
#define WDOG_WMCR_PDE_DISABLE      0
#define WDOG_WMCR_PDE_ENABLE       1


#endif /*IMX25_WDOG_H_*/



