/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  HARDWARE DEFINITION FILE 
//  Date:       9/18/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: db_shared/Hardware_Header_Files/iMX25/iMX25_EXTMEM.h $
// $Revision: 1.2 $
// $Date: 2011/06/28 13:26:41EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx25_extmem.h
//
// Provides definitions for External and Internal Memory
// Refer to Chapter 2 of the iMX25 Reference Manual
//
//-----------------------------------------------------------------------------

#ifndef IMX25_EXTMEM_H_
#define IMX25_EXTMEM_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------
// #########################################
// # External Memory CSD0 CSD1             #
// #########################################
#define CSD0_BASE_ADDR             0x80000000U
#define CSD0_END_ADDR              0x8FFFFFFFU
#define CSD1_BASE_ADDR             0x90000000U
#define CSD1_END_ADDR              0x9FFFFFFFU

// #########################################
// # External Memory CS0 - CS5             #
// # $A000_0000 to $A7FF_FFFF (CS0)        #
// # $A800_0000 to $AFFF_FFFF (CS1)        #
// # $B000_0000 to $B1FF_FFFF (CS2)        #
// # $B200_0000 to $B3FF_FFFF (CS3)        #
// # $B400_0000 to $B5FF_FFFF (CS4)        #
// # $B600_0000 to $B7FF_FFFF (CS5)        #
// #########################################
#define CS0_BASE_ADDR              0xA0000000U
#define CS0_END_ADDR               (CS0_BASE_ADDR+0x7FFFFFFU) 
#define CS1_BASE_ADDR              0xA8000000U
#define CS1_END_ADDR               (CS1_BASE_ADDR+0x7FFFFFFU) 
#define CS2_BASE_ADDR              0xB0000000U
#define CS2_END_ADDR               (CS2_BASE_ADDR+0x1FFFFFFU) 
#define CS3_BASE_ADDR              0xB2000000U
#define CS3_END_ADDR               (CS3_BASE_ADDR+0x1FFFFFFU) 
#define CS4_BASE_ADDR              0xB4000000U
#define CS4_END_ADDR               (CS4_BASE_ADDR+0x1FFFFFFU) 
#define CS5_BASE_ADDR              0xB6000000U
#define CS5_END_ADDR               (CS5_BASE_ADDR+0x1FFFFFFU) 

// #########################################
// # RAM (128KB)                           #
// # $7800_0000 to $7801_FFFF              #
// #########################################
#define RAM_BASE_ADDR              0x78000000U


//------------------------------------------------------------------------------
// REGISTER FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
//#if PLATFORM_CURRENT == imx27
//#define SDRAMBase    0xA0000000          /* DRAM */
//#elif PLATFORM_CURRENT == mxl
//#define SDRAMBase    0x08000000          /* DRAM */
//#endif

//------------------------------------------------------------------------------
// REGISTER FIELD WIDTHS
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------


#endif /*IMX25_EXTMEM_H_*/


