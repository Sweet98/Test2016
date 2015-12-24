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
// $RCSfile: db_shared/Hardware_Header_Files/iMX25/iMX25_AIPS.h $
// $Revision: 1.2 $
// $Date: 2011/06/28 13:26:51EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx25_aips.h
//
// Provides definitions for The AHB-lite to IPS (AIPS)
// Refer to Chapter 12 of the iMX25 Reference Manual
//
//-----------------------------------------------------------------------------

#ifndef IMX25_AIPS_H_
#define IMX25_AIPS_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------
// #########################################
// # AIPS1                                 #
// #########################################
#define AIPS1_BASE_ADDR            0x43F00000U
#define AIPS1_MPROT0_7             (AIPS1_BASE_ADDR+0x00U)     // Master Privilege Reg 0-7
#define AIPS1_MPROT8_15            (AIPS1_BASE_ADDR+0x04U)     // Master Privilege Reg 8-15
#define AIPS1_PACR0_7              (AIPS1_BASE_ADDR+0x20U)     // Peripheral Access Control Reg 0-7
#define AIPS1_PACR8_15             (AIPS1_BASE_ADDR+0x24U)     // Peripheral Access Control Reg 8-15
#define AIPS1_PACR16_23            (AIPS1_BASE_ADDR+0x28U)     // Peripheral Access Control Reg 16-23
#define AIPS1_PACR24_31            (AIPS1_BASE_ADDR+0x2CU)     // Peripheral Access Control Reg 24-32
#define AIPS1_OPACR0_7             (AIPS1_BASE_ADDR+0x40U)     // Off-Platform Peri Access Ctl Reg 0-7
#define AIPS1_OPACR8_15            (AIPS1_BASE_ADDR+0x44U)     // Off-Platform Peri Access Ctl Reg 8-15
#define AIPS1_OPACR16_23           (AIPS1_BASE_ADDR+0x48U)     // Off-Platform Peri Access Ctl Reg 16-23
#define AIPS1_OPACR24_31           (AIPS1_BASE_ADDR+0x4CU)     // Off-Platform Peri Access Ctl Reg 24-31
#define AIPS1_OPACR32_33           (AIPS1_BASE_ADDR+0x50U)     // Off-Platform Peri Access Ctl Reg 32-33


// #########################################
// # AIPS2                                 #
// #########################################
#define AIPS2_BASE_ADDR            0x53F00000U
#define AIPS2_MPROT0_7             (AIPS2_BASE_ADDR+0x00U)     // Master Privilege Reg 0-7
#define AIPS2_MPROT8_15            (AIPS2_BASE_ADDR+0x04U)     // Master Privilege Reg 8-15
#define AIPS2_PACR0_7              (AIPS2_BASE_ADDR+0x20U)     // Peripheral Access Control Reg 0-7
#define AIPS2_PACR8_15             (AIPS2_BASE_ADDR+0x24U)     // Peripheral Access Control Reg 8-15
#define AIPS2_PACR16_23            (AIPS2_BASE_ADDR+0x28U)     // Peripheral Access Control Reg 16-23
#define AIPS2_PACR24_31            (AIPS2_BASE_ADDR+0x2CU)     // Peripheral Access Control Reg 24-32
#define AIPS2_OPACR0_7             (AIPS2_BASE_ADDR+0x40U)     // Off-Platform Peri Access Ctl Reg 0-7
#define AIPS2_OPACR8_15            (AIPS2_BASE_ADDR+0x44U)     // Off-Platform Peri Access Ctl Reg 8-15
#define AIPS2_OPACR16_23           (AIPS2_BASE_ADDR+0x48U)     // Off-Platform Peri Access Ctl Reg 16-23
#define AIPS2_OPACR24_31           (AIPS2_BASE_ADDR+0x4CU)     // Off-Platform Peri Access Ctl Reg 24-31
#define AIPS2_OPACR32_33           (AIPS2_BASE_ADDR+0x50U)     // Off-Platform Peri Access Ctl Reg 32-33

//------------------------------------------------------------------------------
// REGISTER FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// REGISTER FIELD WIDTHS
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------


#endif /*IMX25_AIPS_H_*/




