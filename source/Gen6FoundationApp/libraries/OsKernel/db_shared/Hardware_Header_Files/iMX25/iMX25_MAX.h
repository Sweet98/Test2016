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
// $RCSfile: db_shared/Hardware_Header_Files/iMX25/iMX25_MAX.h $
// $Revision: 1.2 $
// $Date: 2011/06/28 13:26:42EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx25_max.h
//
// Provides definitions for the MAX (Multi-Layer AHB Crossbar Switch)
// Refer to Chapter 16 of the iMX25 Reference Manual
//
//------------------------------------------------------------------------------

#ifndef IMX25_MAX_H_
#define IMX25_MAX_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// REGISTERS
//------------------------------------------------------------------------------
// #########################################
// # MAX                                   #
// #########################################
#define MAX_BASE_ADDR              0x43F04000U
#define MAX_SLV0_BASE              (MAX_BASE_ADDR+0x000U)      // base location for slave 0
#define MAX_SLV1_BASE              (MAX_BASE_ADDR+0x100U)      // base location for slave 1
#define MAX_SLV2_BASE              (MAX_BASE_ADDR+0x200U)      // base location for slave 2
#define MAX_SLV3_BASE              (MAX_BASE_ADDR+0x300U)      // base location for slave 3
#define MAX_SLV4_BASE              (MAX_BASE_ADDR+0x400U)      // base location for slave 4

#define MAX_SLV0_MPR0              (MAX_SLV0_BASE+0x00U)       // 32bit max slv0 master priority reg
#define MAX_SLV0_SGPCR0            (MAX_SLV0_BASE+0x10U)       // 32bit max slv0 general ctrl reg

#define MAX_SLV1_MPR1              (MAX_SLV1_BASE+0x00U)       // 32bit max slv1 master priority reg
#define MAX_SLV1_SGPCR1            (MAX_SLV1_BASE+0x10U)       // 32bit max slv1 general ctrl reg

#define MAX_SLV2_MPR2              (MAX_SLV2_BASE+0x00U)       // 32bit max slv2 master priority reg
#define MAX_SLV2_SGPCR2            (MAX_SLV2_BASE+0x10U)       // 32bit max slv2 general ctrl reg

#define MAX_SLV3_MPR3              (MAX_SLV3_BASE+0x00U)       // 32bit max slv3 master priority reg
#define MAX_SLV3_SGPCR3            (MAX_SLV3_BASE+0x10U)       // 32bit max slv3 general ctrl reg

#define MAX_SLV4_MPR4              (MAX_SLV4_BASE+0x00U)       // 32bit max slv4 master priority reg
#define MAX_SLV4_SGPCR4            (MAX_SLV4_BASE+0x10U)       // 32bit max slv4 general ctrl reg

#define MAX_MST0_MGPCR0            (MAX_BASE_ADDR+0x800U)      // 32bit max mst0 general ctrl reg
#define MAX_MST1_MGPCR1            (MAX_BASE_ADDR+0x900U)      // 32bit max mst1 general ctrl reg
#define MAX_MST2_MGPCR2            (MAX_BASE_ADDR+0xA00U)      // 32bit max mst2 general ctrl reg
#define MAX_MST3_MGPCR3            (MAX_BASE_ADDR+0xB00U)      // 32bit max mst3 general ctrl reg
#define MAX_MST4_MGPCR4            (MAX_BASE_ADDR+0xC00U)      // 32bit max mst4 general ctrl reg


//------------------------------------------------------------------------------
// REGISTER BIT FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
//Master Priority Register (MPRn)
#define MAX_MPR_MSTR_0_LSH       0
#define MAX_MPR_MSTR_1_LSH       4
#define MAX_MPR_MSTR_2_LSH       8
#define MAX_MPR_MSTR_3_LSH       12
#define MAX_MPR_MSTR_4_LSH       16

//Slave General Purpose Control Register (SGPCR)
#define MAX_SGPCR_PARK_LSH       0
#define MAX_SGPCR_PCTL_LSH       4
#define MAX_SGPCR_ARB_LSH        8
#define MAX_SGPCR_HLP_LSH        30
#define MAX_SGPCR_RO_LSH         31

//Master General Purpose Control Register (MGPCR)
#define MAX_MGPCR_AULB_LSH       0


//------------------------------------------------------------------------------
// REGISTER FIELD WIDTHS
//------------------------------------------------------------------------------
//Master Priority Register (MPRn)
#define MAX_MPR_MSTR_0_WID      Width_3
#define MAX_MPR_MSTR_1_WID      Width_3
#define MAX_MPR_MSTR_2_WID      Width_3
#define MAX_MPR_MSTR_3_WID      Width_3
#define MAX_MPR_MSTR_4_WID      Width_3

//Slave General Purpose Control Register (SGPCR)
#define MAX_SGPCR_PARK_WID      Width_3
#define MAX_SGPCR_PCTL_WID      Width_2
#define MAX_SGPCR_ARB_WID       Width_2
#define MAX_SGPCR_HLP_WID       Width_1
#define MAX_SGPCR_RO_WID       	Width_1

//Master General Purpose Control Register (MGPCR)
#define MAX_MGPCR_AULB_WID      Width_3

//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------

#endif /*IMX25_MAX_H_*/


