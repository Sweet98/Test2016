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
// $RCSfile: db_shared/Hardware_Header_Files/iMX27/iMX27_aipi.h $
// $Revision: 1.1 $
// $Date: 2008/09/22 16:35:05EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  iMX27_aipi.h
//
// Provides definitions for the AHB-Lite to IP bus interface (AIPI)
// Refer to Chapter 35 Page 1267 of the iMX27 Reference Manual
// VRAM is also defined in this file.
//
//------------------------------------------------------------------------------

#ifndef IMX27_AIPI_H_
#define IMX27_AIPI_H_

//------------------------------------------------------------------------------
// REGISTERS
//------------------------------------------------------------------------------

// #########################################
// # AIPI1                                 #
// # $1000_0000 to $1000_0FFF              #
// #########################################
#define AIPI1_BASE_ADDR          0x10000000
#define AIPI1_PSR0               (AIPI1_BASE_ADDR+0x00)     // 32bit Peripheral Size Reg 0
#define AIPI1_PSR1               (AIPI1_BASE_ADDR+0x04)     // 32bit Peripheral Size Reg 1
#define AIPI1_PAR                (AIPI1_BASE_ADDR+0x08)     // 32bit Peripheral Access Reg

// #########################################
// # AIPI2                                 #
// # $1002_0000 to $1002_0FFF              #
// #########################################
#define AIPI2_BASE_ADDR          0x10020000
#define AIPI2_PSR0               (AIPI2_BASE_ADDR+0x00)     // 32bit Peripheral Size Reg 0
#define AIPI2_PSR1               (AIPI2_BASE_ADDR+0x04)     // 32bit Peripheral Size Reg 1
#define AIPI2_PAR                (AIPI2_BASE_ADDR+0x08)     // 32bit Peripheral Access Reg

// #########################################
// # VRAM                                  #
// # $FFFF_4C00 to $FFFF_FFFF              #
// #########################################
#define VRAM_BASE_ADDR           0xFFFF4C00

#endif /*IMX27_AIPI_H_*/



