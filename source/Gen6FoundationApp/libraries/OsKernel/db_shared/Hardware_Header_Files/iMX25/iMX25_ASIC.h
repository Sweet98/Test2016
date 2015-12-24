/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  HARDWARE DEFINITION FILE 
//  Date:       9/17/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: db_shared/Hardware_Header_Files/iMX25/iMX25_ASIC.h $
// $Revision: 1.2 $
// $Date: 2011/06/28 13:26:40EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx25_asic.h
//
// Provides definitions for The ARM Simple Interrupt Controller (ASIC) 
//
// Note from Section 10.4.5:
//		NOTE: The ASIC module is the same one used on the ARM11 
//		platform (ARM926EJ-S Vectored Interrupt Controller AVIC) 
//		with the vectoring feature removed for use with an ARM9. 
//		This is Freescale IP.
//
// Refer to Chapter 13 of the iMX25 Reference Manual
//
//------------------------------------------------------------------------------

#ifndef IMX25_ASIC_H_
#define IMX25_ASIC_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width 
//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------
// #########################################
// # ASIC                                  #
// #########################################
#define ASIC_BASE_ADDR             0x68000000U
#define ASIC_INTCNTL               (ASIC_BASE_ADDR+0x00U)      // 32bit ASIC int control reg
#define ASIC_NIMASK                (ASIC_BASE_ADDR+0x04U)      // 32bit ASIC int mask reg
#define ASIC_INTENNUM              (ASIC_BASE_ADDR+0x08U)      // 32bit ASIC int enable number reg
#define ASIC_INTDISNUM             (ASIC_BASE_ADDR+0x0CU)      // 32bit ASIC int disable number reg
#define ASIC_INTENABLEH            (ASIC_BASE_ADDR+0x10U)      // 32bit ASIC int enable reg high
#define ASIC_INTENABLEL            (ASIC_BASE_ADDR+0x14U)      // 32bit ASIC int enable reg low
#define ASIC_INTTYPEH              (ASIC_BASE_ADDR+0x18U)      // 32bit ASIC int type reg high
#define ASIC_INTTYPEL              (ASIC_BASE_ADDR+0x1CU)      // 32bit ASIC int type reg low
#define ASIC_NIPRIORITY7           (ASIC_BASE_ADDR+0x20U)      // 32bit ASIC norm int priority lvl7
#define ASIC_NIPRIORITY6           (ASIC_BASE_ADDR+0x24U)      // 32bit ASIC norm int priority lvl6
#define ASIC_NIPRIORITY5           (ASIC_BASE_ADDR+0x28U)      // 32bit ASIC norm int priority lvl5
#define ASIC_NIPRIORITY4           (ASIC_BASE_ADDR+0x2CU)      // 32bit ASIC norm int priority lvl4
#define ASIC_NIPRIORITY3           (ASIC_BASE_ADDR+0x30U)      // 32bit ASIC norm int priority lvl3
#define ASIC_NIPRIORITY2           (ASIC_BASE_ADDR+0x34U)      // 32bit ASIC norm int priority lvl2
#define ASIC_NIPRIORITY1           (ASIC_BASE_ADDR+0x38U)      // 32bit ASIC norm int priority lvl1
#define ASIC_NIPRIORITY0           (ASIC_BASE_ADDR+0x3CU)      // 32bit ASIC norm int priority lvl0
#define ASIC_NIVECSR               (ASIC_BASE_ADDR+0x40U)      // 32bit ASIC norm int vector/status
#define ASIC_FIVECSR               (ASIC_BASE_ADDR+0x44U)      // 32bit ASIC fast int vector/status
#define ASIC_INTSRCH               (ASIC_BASE_ADDR+0x48U)      // 32bit ASIC int source reg high
#define ASIC_INTSRCL               (ASIC_BASE_ADDR+0x4CU)      // 32bit ASIC int source reg low
#define ASIC_INTFRCH               (ASIC_BASE_ADDR+0x50U)      // 32bit ASIC int force reg high
#define ASIC_INTFRCL               (ASIC_BASE_ADDR+0x54U)      // 32bit ASIC int force reg low
#define ASIC_NIPNDH                (ASIC_BASE_ADDR+0x58U)      // 32bit ASIC norm int pending high
#define ASIC_NIPNDL                (ASIC_BASE_ADDR+0x5CU)      // 32bit ASIC norm int pending low
#define ASIC_FIPNDH                (ASIC_BASE_ADDR+0x60U)      // 32bit ASIC fast int pending high
#define ASIC_FIPNDL                (ASIC_BASE_ADDR+0x64U)      // 32bit ASIC fast int pending low

//------------------------------------------------------------------------------
// REGISTER BIT FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------

//Interrupt Control Register (INTCNTL)
#define ASIC_INTCNTL_FIDIS_LSH			21
#define ASIC_INTCNTL_NIDIS_LSH			22
#define ASIC_INTCNTL_BYP_EN_LSH			23


// Normal Interrupt Mask Register (NIMASK)
#define ASIC_NIMASK_NIMASK_LSH          0

// Interrupt Enable Number Register (INTENNUM)
#define ASIC_INTENNUM_ENNUM_LSH         0

// Interrupt Disable Number Register (INTDISNUM)
#define ASIC_INTDISNUM_DISNUM_LSH       0

// Interrupt Enable Register High (INTENABLEH) and Low(INTENABLEL)
#define ASIC_INTENABLEH_INTENABLE_LSH   0
#define ASIC_INTENABLEL_INTENABLE_LSH   0

// Interrupt Type Register High (INTTYPEH) and Low (INTTYPEL)
#define ASIC_INTTYPEH_INTTYPE_LSH       0
#define ASIC_INTTYPEL_INTTYPE_LSH       0

// Normal Interrupt Vector and Status Register (NIVECSR)
#define ASIC_NIVECSR_NIPRILVL_LSH       0
#define ASIC_NIVECSR_NIVECTOR_LSH       16

// Fast Interrupt Vector and Status Register (FIVECSR)
#define ASIC_FIVECSR_FIVECTOR_LSH       0

// Interrupt Source Register High (INTSRCH) and Low (INTSRCL)
#define ASIC_INTSRCH_INTIN_LSH          0
#define ASIC_INTSRCL_INTIN_LSH          0

// Interrupt Force Register High (INTFRCH) and Low (INTFRCL)
#define ASIC_INTFRCH_FORCE_LSH          0
#define ASIC_INTFRCL_FORCE_LSH          0

// Normal Interrupt Pending Register High (NIPNDH) and Low (NIPNDL)
#define ASIC_NIPNDH_NIPEND_LSH          0
#define ASIC_NIPNDL_NIPEND_LSH          0

// Fast Interrupt Pending Register High (FIPNDH) and Low (FIPNDL)
#define ASIC_FIPNDH_FIPEND_LSH          0
#define ASIC_FIPNDL_FIPEND_LSH          0

//------------------------------------------------------------------------------
// REGISTER FIELD WIDTHS
//------------------------------------------------------------------------------

//Interrupt Control Register (INTCNTL)
#define ASIC_INTCNTL_FIDIS_WID          Width_1
#define ASIC_INTCNTL_NIDIS_WID          Width_1
#define ASIC_INTCNTL_BYP_EN_WID         Width_1

// Normal Interrupt Mask Register (NIMASK)
#define ASIC_NIMASK_NIMASK_WID          Width_5

// Interrupt Enable Number Register (INTENNUM)
#define ASIC_INTENNUM_ENNUM_WID         Width_6

// Interrupt Disable Number Register (INTDISNUM)
#define ASIC_INTDISNUM_DISNUM_WID       Width_6

// Interrupt Enable Register High (INTENABLEH) and Low(INTENABLEL)
#define ASIC_INTENABLEH_INTENABLE_WID   Width_32
#define ASIC_INTENABLEL_INTENABLE_WID   Width_32

// Interrupt Type Register High (INTTYPEH) and Low (INTTYPEL)
#define ASIC_INTTYPEH_INTTYPE_WID       Width_32
#define ASIC_INTTYPEL_INTTYPE_WID       Width_32

// Normal Interrupt Vector and Status Register (NIVECSR)
#define ASIC_NIVECSR_NIPRILVL_WID       Width_16
#define ASIC_NIVECSR_NIVECTOR_WID       Width_16

// Fast Interrupt Vector and Status Register (FIVECSR)
#define ASIC_FIVECSR_FIVECTOR_WID       Width_32

// Interrupt Source Register High (INTSRCH) and Low (INTSRCL)
#define ASIC_INTSRCH_INTIN_WID          Width_32
#define ASIC_INTSRCL_INTIN_WID          Width_32

// Interrupt Force Register High (INTFRCH) and Low (INTFRCL)
#define ASIC_INTFRCH_FORCE_WID          Width_32
#define ASIC_INTFRCL_FORCE_WID          Width_32

// Normal Interrupt Pending Register High (NIPNDH) and Low (NIPNDL)
#define ASIC_NIPNDH_NIPEND_WID          Width_32
#define ASIC_NIPNDL_NIPEND_WID          Width_32

// Fast Interrupt Pending Register High (FIPNDH) and Low (FIPNDL)
#define ASIC_FIPNDH_FIPEND_WID          Width_32
#define ASIC_FIPNDL_FIPEND_WID          Width_32


#endif /*IMX25_ASIC_H_*/



