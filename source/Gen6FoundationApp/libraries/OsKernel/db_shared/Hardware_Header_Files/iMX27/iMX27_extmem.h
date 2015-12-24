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
// $RCSfile: db_shared/Hardware_Header_Files/iMX27/iMX27_extmem.h $
// $Revision: 1.1 $
// $Date: 2008/09/22 16:35:07EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx27_extmem.h
//
// Provides definitions for the External Memory Locations
// Refer to Chapter 2 Page 64 of the iMX27 Reference Manual
//
//-----------------------------------------------------------------------------

#ifndef IMX27_EXTMEM_H_
#define IMX27_EXTMEM_H_

// #########################################
// # External Memory CSD0 CSD1             #
// # $A000_0000 to $AFFF_FFFF (CSD0)       #
// # $B000_0000 to $BFFF_FFFF (CSD1)       #
// #########################################
#define CSD0_BASE_ADDR           0xA0000000
#define CSD0_END_ADDR            (CSD0_BASE_ADDR+0x0FFFFFFF) // 
#define CSD1_BASE_ADDR           0xB0000000
#define CSD1_END_ADDR            (CSD1_BASE_ADDR+0x0FFFFFFF) // 

// #########################################
// # External Memory CS0 - CS5             #
// # $C000_0000 to $C7FF_FFFF (CS0)        #
// # $C800_0000 to $CFFF_FFFF (CS1)        #
// # $D000_0000 to $D1FF_FFFF (CS2)        #
// # $D200_0000 to $D3FF_FFFF (CS3)        #
// # $D400_0000 to $D5FF_FFFF (CS4)        #
// # $D600_0000 to $D7FF_FFFF (CS5)        #
// #########################################
#define CS0_BASE_ADDR            0xC0000000
#define CS0_END_ADDR             (CS0_BASE_ADDR+0x7FFFFFF)  // 
#define CS1_BASE_ADDR            0xC8000000
#define CS1_END_ADDR             (CS1_BASE_ADDR+0x7FFFFFF)  // 
#define CS2_BASE_ADDR            0xD0000000
#define CS2_END_ADDR             (CS2_BASE_ADDR+0x1FFFFFF)  // 
#define CS3_BASE_ADDR            0xD2000000
#define CS3_END_ADDR             (CS3_BASE_ADDR+0x1FFFFFF)  // 
#define CS4_BASE_ADDR            0xD4000000
#define CS4_END_ADDR             (CS4_BASE_ADDR+0x1FFFFFF)  // 
#define CS5_BASE_ADDR            0xD6000000
#define CS5_END_ADDR             (CS5_BASE_ADDR+0x1FFFFFF)  // 


#endif /*IMX27_EXTMEM_H_*/


