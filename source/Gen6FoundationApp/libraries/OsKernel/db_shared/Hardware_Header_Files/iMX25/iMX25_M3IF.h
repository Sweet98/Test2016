
/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  Date:       6/1/2009
//  Tools:      RVCT Build 739
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: db_shared/Hardware_Header_Files/iMX25/iMX25_M3IF.h $
// $Revision: 1.2 $
// $Date: 2011/06/28 13:26:44EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// HEADER:  iMX25_M3IF.h
//
//  Provides definitions for The Multi-Master Memory Interface (M3IF)
//
//------------------------------------------------------------------------------
#ifndef IMX25_M3IF_H_
#define IMX25_M3IF_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------
// #########################################
// # M3IF                                  #
// #########################################
#define M3IF_BASE_ADDR             0xB8003000U
#define M3IF_CTL                   (M3IF_BASE_ADDR+0x00U)     
#define M3IF_WCFG0                 (M3IF_BASE_ADDR+0x04U)     
#define M3IF_WCFG1                 (M3IF_BASE_ADDR+0x08U)     
#define M3IF_WCFG2                 (M3IF_BASE_ADDR+0x0CU)     
#define M3IF_WCFG3                 (M3IF_BASE_ADDR+0x10U)     
#define M3IF_WCFG4                 (M3IF_BASE_ADDR+0x14U)     
#define M3IF_WCFG5                 (M3IF_BASE_ADDR+0x18U)     
#define M3IF_WCFG6                 (M3IF_BASE_ADDR+0x1CU)     
#define M3IF_WCFG7                 (M3IF_BASE_ADDR+0x20U)     
#define M3IF_WCSR                  (M3IF_BASE_ADDR+0x24U)     
#define M3IF_SCFG0                 (M3IF_BASE_ADDR+0x28U)     
#define M3IF_SCFG1                 (M3IF_BASE_ADDR+0x2CU)     
#define M3IF_SCFG2                 (M3IF_BASE_ADDR+0x30U)     
#define M3IF_SSR0                  (M3IF_BASE_ADDR+0x34U)     
#define M3IF_SSR1                  (M3IF_BASE_ADDR+0x38U)     

//------------------------------------------------------------------------------
// REGISTER FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// REGISTER FIELD WIDTHS
//------------------------------------------------------------------------------
		

//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------


#endif /*IMX25_M3IF_H_*/

