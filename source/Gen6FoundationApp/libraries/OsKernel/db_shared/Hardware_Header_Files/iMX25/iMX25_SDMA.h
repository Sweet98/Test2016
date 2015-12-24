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
// $RCSfile: db_shared/Hardware_Header_Files/iMX25/iMX25_SDMA.h $
// $Revision: 1.2 $
// $Date: 2011/06/28 13:26:49EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx25_dma.h
//
// Provides definitions for the The Smart Direct Memory Access (SDMA) Controller
// Refer to Chapter 53 of the iMX25 Reference Manual
//
//-----------------------------------------------------------------------------

#ifndef IMX25_SDMA_H_
#define IMX25_SDMA_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------
// #########################################
// # SDMA                                  #
// #########################################
#define SDMA_BASE_ADDR             0x53FD4000U
#define SDMA_COPTR                 (SDMA_BASE_ADDR+0x000U)    
#define SDMA_INTR                  (SDMA_BASE_ADDR+0x004U)    
#define SDMA_STOP                  (SDMA_BASE_ADDR+0x008U)    
#define SDMA_START                 (SDMA_BASE_ADDR+0x00CU)    
#define SDMA_EVTOVR                (SDMA_BASE_ADDR+0x010U)    
#define SDMA_DSPOVR                (SDMA_BASE_ADDR+0x014U)    
#define SDMA_HOSTOVR               (SDMA_BASE_ADDR+0x018U)    
#define SDMA_EVTPEND               (SDMA_BASE_ADDR+0x01CU)    
#define SDMA_DSPENBL               (SDMA_BASE_ADDR+0x020U)    
#define SDMA_RESET                 (SDMA_BASE_ADDR+0x024U)    
#define SDMA_EVTERR                (SDMA_BASE_ADDR+0x028U)    
#define SDMA_INTRMASK              (SDMA_BASE_ADDR+0x02CU)    
#define SDMA_PSW                   (SDMA_BASE_ADDR+0x030U)    
#define SDMA_EVTERRDBG             (SDMA_BASE_ADDR+0x034U)    
#define SDMA_CONFIG                (SDMA_BASE_ADDR+0x038U)    
#define SDMA_ONCE_ENB              (SDMA_BASE_ADDR+0x040U)    
#define SDMA_ONCE_DATA             (SDMA_BASE_ADDR+0x044U)    
#define SDMA_ONCE_INSTR            (SDMA_BASE_ADDR+0x048U)    
#define SDMA_ONCE_STAT             (SDMA_BASE_ADDR+0x04CU)    
#define SDMA_ONCE_CMD              (SDMA_BASE_ADDR+0x050U)    
#define SDMA_MIRROR                (SDMA_BASE_ADDR+0x054U)    
#define SDMA_ILLINSTADDR           (SDMA_BASE_ADDR+0x058U)    
#define SDMA_CHN0ADDR              (SDMA_BASE_ADDR+0x05CU)    
#define SDMA_XTRIG_CONF1           (SDMA_BASE_ADDR+0x070U)    
#define SDMA_XTRIG_CONF2           (SDMA_BASE_ADDR+0x074U)    
#define SDMA_CHNPRI_0              (SDMA_BASE_ADDR+0x100U)    
#define SDMA_CHNPRI_1              (SDMA_BASE_ADDR+0x104U)    
#define SDMA_CHNPRI_2              (SDMA_BASE_ADDR+0x108U)    
#define SDMA_CHNPRI_3              (SDMA_BASE_ADDR+0x10CU)    
#define SDMA_CHNPRI_4              (SDMA_BASE_ADDR+0x110U)    
#define SDMA_CHNPRI_5              (SDMA_BASE_ADDR+0x114U)    
#define SDMA_CHNPRI_6              (SDMA_BASE_ADDR+0x118U)    
#define SDMA_CHNPRI_7              (SDMA_BASE_ADDR+0x11CU)    
#define SDMA_CHNPRI_8              (SDMA_BASE_ADDR+0x120U)    
#define SDMA_CHNPRI_9              (SDMA_BASE_ADDR+0x124U)    
#define SDMA_CHNPRI_10             (SDMA_BASE_ADDR+0x128U)    
#define SDMA_CHNPRI_11             (SDMA_BASE_ADDR+0x12CU)    
#define SDMA_CHNPRI_12             (SDMA_BASE_ADDR+0x130U)    
#define SDMA_CHNPRI_13             (SDMA_BASE_ADDR+0x134U)    
#define SDMA_CHNPRI_14             (SDMA_BASE_ADDR+0x138U)    
#define SDMA_CHNPRI_15             (SDMA_BASE_ADDR+0x13CU)    
#define SDMA_CHNPRI_16             (SDMA_BASE_ADDR+0x140U)    
#define SDMA_CHNPRI_17             (SDMA_BASE_ADDR+0x144U)    
#define SDMA_CHNPRI_18             (SDMA_BASE_ADDR+0x148U)    
#define SDMA_CHNPRI_19             (SDMA_BASE_ADDR+0x14CU)    
#define SDMA_CHNPRI_20             (SDMA_BASE_ADDR+0x150U)    
#define SDMA_CHNPRI_21             (SDMA_BASE_ADDR+0x154U)    
#define SDMA_CHNPRI_22             (SDMA_BASE_ADDR+0x158U)    
#define SDMA_CHNPRI_23             (SDMA_BASE_ADDR+0x15CU)    
#define SDMA_CHNPRI_24             (SDMA_BASE_ADDR+0x160U)    
#define SDMA_CHNPRI_25             (SDMA_BASE_ADDR+0x164U)    
#define SDMA_CHNPRI_26             (SDMA_BASE_ADDR+0x168U)    
#define SDMA_CHNPRI_27             (SDMA_BASE_ADDR+0x16CU)    
#define SDMA_CHNPRI_28             (SDMA_BASE_ADDR+0x170U)    
#define SDMA_CHNPRI_29             (SDMA_BASE_ADDR+0x174U)    
#define SDMA_CHNPRI_30             (SDMA_BASE_ADDR+0x178U)    
#define SDMA_CHNPRI_31             (SDMA_BASE_ADDR+0x17CU)    
#define SDMA_CHENBL_0              (SDMA_BASE_ADDR+0x200U)    
#define SDMA_CHENBL_1              (SDMA_BASE_ADDR+0x204U)    
#define SDMA_CHENBL_2              (SDMA_BASE_ADDR+0x208U)    
#define SDMA_CHENBL_3              (SDMA_BASE_ADDR+0x20CU)    
#define SDMA_CHENBL_4              (SDMA_BASE_ADDR+0x210U)    
#define SDMA_CHENBL_5              (SDMA_BASE_ADDR+0x214U)    
#define SDMA_CHENBL_6              (SDMA_BASE_ADDR+0x218U)    
#define SDMA_CHENBL_7              (SDMA_BASE_ADDR+0x21CU)    
#define SDMA_CHENBL_8              (SDMA_BASE_ADDR+0x220U)    
#define SDMA_CHENBL_9              (SDMA_BASE_ADDR+0x224U)    
#define SDMA_CHENBL_10             (SDMA_BASE_ADDR+0x228U)    
#define SDMA_CHENBL_11             (SDMA_BASE_ADDR+0x22CU)    
#define SDMA_CHENBL_12             (SDMA_BASE_ADDR+0x230U)    
#define SDMA_CHENBL_13             (SDMA_BASE_ADDR+0x234U)    
#define SDMA_CHENBL_14             (SDMA_BASE_ADDR+0x238U)    
#define SDMA_CHENBL_15             (SDMA_BASE_ADDR+0x23CU)    
#define SDMA_CHENBL_16             (SDMA_BASE_ADDR+0x240U)    
#define SDMA_CHENBL_17             (SDMA_BASE_ADDR+0x244U)    
#define SDMA_CHENBL_18             (SDMA_BASE_ADDR+0x248U)    
#define SDMA_CHENBL_19             (SDMA_BASE_ADDR+0x24CU)    
#define SDMA_CHENBL_20             (SDMA_BASE_ADDR+0x250U)    
#define SDMA_CHENBL_21             (SDMA_BASE_ADDR+0x254U)    
#define SDMA_CHENBL_22             (SDMA_BASE_ADDR+0x258U)    
#define SDMA_CHENBL_23             (SDMA_BASE_ADDR+0x25CU)    
#define SDMA_CHENBL_24             (SDMA_BASE_ADDR+0x260U)    
#define SDMA_CHENBL_25             (SDMA_BASE_ADDR+0x264U)    
#define SDMA_CHENBL_26             (SDMA_BASE_ADDR+0x268U)    
#define SDMA_CHENBL_27             (SDMA_BASE_ADDR+0x26CU)    
#define SDMA_CHENBL_28             (SDMA_BASE_ADDR+0x270U)    
#define SDMA_CHENBL_29             (SDMA_BASE_ADDR+0x274U)    
#define SDMA_CHENBL_30             (SDMA_BASE_ADDR+0x278U)    
#define SDMA_CHENBL_31             (SDMA_BASE_ADDR+0x27CU)    
#define SDMA_CHENBL_32             (SDMA_BASE_ADDR+0x280U)    
#define SDMA_CHENBL_33             (SDMA_BASE_ADDR+0x284U)    
#define SDMA_CHENBL_34             (SDMA_BASE_ADDR+0x288U)    
#define SDMA_CHENBL_35             (SDMA_BASE_ADDR+0x28CU)    
#define SDMA_CHENBL_36             (SDMA_BASE_ADDR+0x290U)    
#define SDMA_CHENBL_37             (SDMA_BASE_ADDR+0x294U)    
#define SDMA_CHENBL_38             (SDMA_BASE_ADDR+0x298U)    
#define SDMA_CHENBL_39             (SDMA_BASE_ADDR+0x29CU)    
#define SDMA_CHENBL_40             (SDMA_BASE_ADDR+0x2A0U)    
#define SDMA_CHENBL_41             (SDMA_BASE_ADDR+0x2A4U)    
#define SDMA_CHENBL_42             (SDMA_BASE_ADDR+0x2A8U)    
#define SDMA_CHENBL_43             (SDMA_BASE_ADDR+0x2ACU)    
#define SDMA_CHENBL_44             (SDMA_BASE_ADDR+0x2B0U)    
#define SDMA_CHENBL_45             (SDMA_BASE_ADDR+0x2B4U)    
#define SDMA_CHENBL_46             (SDMA_BASE_ADDR+0x2B8U)    
#define SDMA_CHENBL_47             (SDMA_BASE_ADDR+0x2BCU)  
//------------------------------------------------------------------------------
// REGISTER BIT FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
// Channel Priority Registers (CHNPRIn)
#define DMA_CHNPRIn_CHNPRIn_LSH            0

//------------------------------------------------------------------------------
// REGISTER BIT FIELD WIDTHS
//------------------------------------------------------------------------------
// Channel Priority Registers (CHNPRIn)
#define DMA_CHNPRIn_CHNPRIn_WID            Width_3

//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------
// Channel Priority Registers (CHNPRIn)
#define DMA_CHNPRIn_CHNPRIn_PRIORITY_0		0x0
#define DMA_CHNPRIn_CHNPRIn_PRIORITY_1		0x1
#define DMA_CHNPRIn_CHNPRIn_PRIORITY_2		0x2
#define DMA_CHNPRIn_CHNPRIn_PRIORITY_3		0x3
#define DMA_CHNPRIn_CHNPRIn_PRIORITY_4		0x4
#define DMA_CHNPRIn_CHNPRIn_PRIORITY_5		0x5
#define DMA_CHNPRIn_CHNPRIn_PRIORITY_6		0x6
#define DMA_CHNPRIn_CHNPRIn_PRIORITY_7		0x7



#endif /*IMX25_SDMA_H_*/


