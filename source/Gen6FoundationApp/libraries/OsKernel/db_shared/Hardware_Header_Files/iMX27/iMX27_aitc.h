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
// $RCSfile: db_shared/Hardware_Header_Files/iMX27/iMX27_aitc.h $
// $Revision: 1.1 $
// $Date: 2008/09/22 16:35:05EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx27_aitc.h
//
// Provides definitions for The ARM926EJ-S Interrupt Controller (AITC)
//
// Refer to Chapter 10 Page 335 of the iMX27 Reference Manual
//
//------------------------------------------------------------------------------

#ifndef IMX27_AITC_H_
#define IMX27_AITC_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width 
//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------
// #########################################
// # AITC                                  #
// # $1004_0000 to $1004_0FFF              #
// #########################################
#define AITC_BASE_ADDR           0x10040000
#define AITC_INTCNTL             (AITC_BASE_ADDR+0x00)      // 32bit aitc int control reg
#define AITC_NIMASK              (AITC_BASE_ADDR+0x04)      // 32bit aitc int mask reg
#define AITC_INTENNUM            (AITC_BASE_ADDR+0x08)      // 32bit aitc int enable number reg
#define AITC_INTDISNUM           (AITC_BASE_ADDR+0x0C)      // 32bit aitc int disable number reg
#define AITC_INTENABLEH          (AITC_BASE_ADDR+0x10)      // 32bit aitc int enable reg high
#define AITC_INTENABLEL          (AITC_BASE_ADDR+0x14)      // 32bit aitc int enable reg low
#define AITC_INTTYPEH            (AITC_BASE_ADDR+0x18)      // 32bit aitc int type reg high
#define AITC_INTTYPEL            (AITC_BASE_ADDR+0x1C)      // 32bit aitc int type reg low
#define AITC_NIPRIORITY7         (AITC_BASE_ADDR+0x20)      // 32bit aitc norm int priority lvl7
#define AITC_NIPRIORITY6         (AITC_BASE_ADDR+0x24)      // 32bit aitc norm int priority lvl6
#define AITC_NIPRIORITY5         (AITC_BASE_ADDR+0x28)      // 32bit aitc norm int priority lvl5
#define AITC_NIPRIORITY4         (AITC_BASE_ADDR+0x2C)      // 32bit aitc norm int priority lvl4
#define AITC_NIPRIORITY3         (AITC_BASE_ADDR+0x30)      // 32bit aitc norm int priority lvl3
#define AITC_NIPRIORITY2         (AITC_BASE_ADDR+0x34)      // 32bit aitc norm int priority lvl2
#define AITC_NIPRIORITY1         (AITC_BASE_ADDR+0x38)      // 32bit aitc norm int priority lvl1
#define AITC_NIPRIORITY0         (AITC_BASE_ADDR+0x3C)      // 32bit aitc norm int priority lvl0
#define AITC_NIVECSR             (AITC_BASE_ADDR+0x40)      // 32bit aitc norm int vector/status
#define AITC_FIVECSR             (AITC_BASE_ADDR+0x44)      // 32bit aitc fast int vector/status
#define AITC_INTSRCH             (AITC_BASE_ADDR+0x48)      // 32bit aitc int source reg high
#define AITC_INTSRCL             (AITC_BASE_ADDR+0x4C)      // 32bit aitc int source reg low
#define AITC_INTFRCH             (AITC_BASE_ADDR+0x50)      // 32bit aitc int force reg high
#define AITC_INTFRCL             (AITC_BASE_ADDR+0x54)      // 32bit aitc int force reg low
#define AITC_NIPNDH              (AITC_BASE_ADDR+0x58)      // 32bit aitc norm int pending high
#define AITC_NIPNDL              (AITC_BASE_ADDR+0x5C)      // 32bit aitc norm int pending low
#define AITC_FIPNDH              (AITC_BASE_ADDR+0x60)      // 32bit aitc fast int pending high
#define AITC_FIPNDL              (AITC_BASE_ADDR+0x64)      // 32bit aitc fast int pending low


//------------------------------------------------------------------------------
// REGISTER BIT FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------

//Interrupt Control Register (INTCNTL)
#define AITC_INTCNTL_POINTER_LSH        2
#define AITC_INTCNTL_MD_LSH             16
#define AITC_INTCNTL_FIAD_LSH           19
#define AITC_INTCNTL_NIAD_LSH           20
#define AITC_INTCNTL_FIDIS_LSH          21
#define AITC_INTCNTL_NIDIS_LSH          22

// Normal Interrupt Mask Register (NIMASK)
#define AITC_NIMASK_NIMASK_LSH          0

// Interrupt Enable Number Register (INTENNUM)
#define AITC_INTENNUM_ENNUM_LSH         0

// Interrupt Disable Number Register (INTDISNUM)
#define AITC_INTDISNUM_DISNUM_LSH       0

// Interrupt Enable Register High (INTENABLEH) and Low(INTENABLEL)
#define AITC_INTENABLEH_INTENABLE_LSH   0
#define AITC_INTENABLEL_INTENABLE_LSH   0

// Interrupt Type Register High (INTTYPEH) and Low (INTTYPEL)
#define AITC_INTTYPEH_INTTYPE_LSH       0
#define AITC_INTTYPEL_INTTYPE_LSH       0

// Normal Interrupt Vector and Status Register (NIVECSR)
#define AITC_NIVECSR_NIPRILVL_LSH       0
#define AITC_NIVECSR_NIVECTOR_LSH       16

// Fast Interrupt Vector and Status Register (FIVECSR)
#define AITC_FIVECSR_FIVECTOR_LSH       0

// Interrupt Source Register High (INTSRCH) and Low (INTSRCL)
#define AITC_INTSRCH_INTIN_LSH          0
#define AITC_INTSRCL_INTIN_LSH          0

// Interrupt Force Register High (INTFRCH) and Low (INTFRCL)
#define AITC_INTFRCH_FORCE_LSH          0
#define AITC_INTFRCL_FORCE_LSH          0

// Normal Interrupt Pending Register High (NIPNDH) and Low (NIPNDL)
#define AITC_NIPNDH_NIPEND_LSH          0
#define AITC_NIPNDL_NIPEND_LSH          0

// Fast Interrupt Pending Register High (FIPNDH) and Low (FIPNDL)
#define AITC_FIPNDH_FIPEND_LSH          0
#define AITC_FIPNDL_FIPEND_LSH          0

//------------------------------------------------------------------------------
// REGISTER FIELD WIDTHS
//------------------------------------------------------------------------------

//Interrupt Control Register (INTCNTL)
#define AITC_INTCNTL_POINTER_WID        Width_10
#define AITC_INTCNTL_MD_WID             Width_1
#define AITC_INTCNTL_FIAD_WID           Width_1
#define AITC_INTCNTL_NIAD_WID           Width_1
#define AITC_INTCNTL_FIDIS_WID          Width_1
#define AITC_INTCNTL_NIDIS_WID          Width_1

// Normal Interrupt Mask Register (NIMASK)
#define AITC_NIMASK_NIMASK_WID          Width_5

// Interrupt Enable Number Register (INTENNUM)
#define AITC_INTENNUM_ENNUM_WID         Width_6

// Interrupt Disable Number Register (INTDISNUM)
#define AITC_INTDISNUM_DISNUM_WID       Width_6

// Interrupt Enable Register High (INTENABLEH) and Low(INTENABLEL)
#define AITC_INTENABLEH_INTENABLE_WID   Width_32
#define AITC_INTENABLEL_INTENABLE_WID   Width_32

// Interrupt Type Register High (INTTYPEH) and Low (INTTYPEL)
#define AITC_INTTYPEH_INTTYPE_WID       Width_32
#define AITC_INTTYPEL_INTTYPE_WID       Width_32

// Normal Interrupt Vector and Status Register (NIVECSR)
#define AITC_NIVECSR_NIPRILVL_WID       Width_16
#define AITC_NIVECSR_NIVECTOR_WID       Width_16

// Fast Interrupt Vector and Status Register (FIVECSR)
#define AITC_FIVECSR_FIVECTOR_WID       Width_32

// Interrupt Source Register High (INTSRCH) and Low (INTSRCL)
#define AITC_INTSRCH_INTIN_WID          Width_32
#define AITC_INTSRCL_INTIN_WID          Width_32

// Interrupt Force Register High (INTFRCH) and Low (INTFRCL)
#define AITC_INTFRCH_FORCE_WID          Width_32
#define AITC_INTFRCL_FORCE_WID          Width_32

// Normal Interrupt Pending Register High (NIPNDH) and Low (NIPNDL)
#define AITC_NIPNDH_NIPEND_WID          Width_32
#define AITC_NIPNDL_NIPEND_WID          Width_32

// Fast Interrupt Pending Register High (FIPNDH) and Low (FIPNDL)
#define AITC_FIPNDH_FIPEND_WID          Width_32
#define AITC_FIPNDL_FIPEND_WID          Width_32


#endif /*IMX27_AITC_H_*/



