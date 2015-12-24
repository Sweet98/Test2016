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
// $RCSfile: db_shared/Hardware_Header_Files/iMX25/iMX25_IIM.h $
// $Revision: 1.2 $
// $Date: 2011/06/28 13:26:53EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx25_iim.h
//
// Provides definitions for The IC Identification Module (IIM)
// Refer to Chapter 20 of the iMX25 Reference Manual
//
//-----------------------------------------------------------------------------

#ifndef IMX25_IIM_H_
#define IMX25_IIM_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------
// #########################################
// # IIM                                   #
// #########################################
#define IIM_BASE_ADDR              0x53FF0000U
#define IIM_STAT                   (IIM_BASE_ADDR+0x0000U)    
#define IIM_STATM                  (IIM_BASE_ADDR+0x0004U)    
#define IIM_ERR                    (IIM_BASE_ADDR+0x0008U)    
#define IIM_EMASK                  (IIM_BASE_ADDR+0x000CU)    
#define IIM_FCTL                   (IIM_BASE_ADDR+0x0010U)    
#define IIM_UA                     (IIM_BASE_ADDR+0x0014U)    
#define IIM_LA                     (IIM_BASE_ADDR+0x0018U)    
#define IIM_SDAT                   (IIM_BASE_ADDR+0x001CU)    
#define IIM_PREV                   (IIM_BASE_ADDR+0x0020U)    
#define IIM_SREV                   (IIM_BASE_ADDR+0x0024U)    
#define IIM_PRG_P                  (IIM_BASE_ADDR+0x0028U)    
#define IIM_SCS0                   (IIM_BASE_ADDR+0x002CU)    
#define IIM_SCS1                   (IIM_BASE_ADDR+0x0030U)    
#define IIM_SCS2                   (IIM_BASE_ADDR+0x0034U)    
#define IIM_SCS3                   (IIM_BASE_ADDR+0x0038U)    
#define IIM_FBAC0                  (IIM_BASE_ADDR+0x0800U)    
#define IIM_JAC_HWV0               (IIM_BASE_ADDR+0x0804U)    
#define IIM_HWV1                   (IIM_BASE_ADDR+0x0808U)    
#define IIM_HWV2                   (IIM_BASE_ADDR+0x080CU)    
#define IIM_HAB0                   (IIM_BASE_ADDR+0x0810U)    
#define IIM_HAB1                   (IIM_BASE_ADDR+0x0814U)    
#define IIM_PREV_FUSE              (IIM_BASE_ADDR+0x0818U)    
#define IIM_SREV_FUSE              (IIM_BASE_ADDR+0x081CU)    
#define IIM_SJC_CHALL_0            (IIM_BASE_ADDR+0x0820U)    
#define IIM_SJC_CHALL_1            (IIM_BASE_ADDR+0x0824U)    
#define IIM_SJC_CHALL_2            (IIM_BASE_ADDR+0x0828U)    
#define IIM_SJC_CHALL_3            (IIM_BASE_ADDR+0x082CU)    
#define IIM_SJC_CHALL_4            (IIM_BASE_ADDR+0x0830U)    
#define IIM_SJC_CHALL_5            (IIM_BASE_ADDR+0x0834U)    
#define IIM_SJC_CHALL_6            (IIM_BASE_ADDR+0x0838U)    
#define IIM_SJC_CHALL_7            (IIM_BASE_ADDR+0x083CU)    
#define IIM_FB0UC17                (IIM_BASE_ADDR+0x0840U)    
#define IIM_FB0UC255               (IIM_BASE_ADDR+0x0BFCU)    
#define IIM_FBAC1                  (IIM_BASE_ADDR+0x0C00U)    
#define IIM_KEY0                   (IIM_BASE_ADDR+0x0C04U)    
#define IIM_KEY1                   (IIM_BASE_ADDR+0x0C08U)    
#define IIM_KEY2                   (IIM_BASE_ADDR+0x0C0CU)    
#define IIM_KEY3                   (IIM_BASE_ADDR+0x0C10U)    
#define IIM_KEY4                   (IIM_BASE_ADDR+0x0C14U)    
#define IIM_KEY5                   (IIM_BASE_ADDR+0x0C18U)    
#define IIM_KEY6                   (IIM_BASE_ADDR+0x0C1CU)    
#define IIM_KEY7                   (IIM_BASE_ADDR+0x0C20U)    
#define IIM_KEY8                   (IIM_BASE_ADDR+0x0C24U)    
#define IIM_KEY9                   (IIM_BASE_ADDR+0x0C28U)    
#define IIM_KEY10                  (IIM_BASE_ADDR+0x0C2CU)    
#define IIM_KEY11                  (IIM_BASE_ADDR+0x0C30U)    
#define IIM_KEY12                  (IIM_BASE_ADDR+0x0C34U)    
#define IIM_KEY13                  (IIM_BASE_ADDR+0x0C38U)    
#define IIM_KEY14                  (IIM_BASE_ADDR+0x0C3CU)    
#define IIM_KEY15                  (IIM_BASE_ADDR+0x0C40U)    
#define IIM_KEY16                  (IIM_BASE_ADDR+0x0C44U)    
#define IIM_KEY17                  (IIM_BASE_ADDR+0x0C48U)    
#define IIM_KEY18                  (IIM_BASE_ADDR+0x0C4CU)    
#define IIM_KEY19                  (IIM_BASE_ADDR+0x0C50U)    
#define IIM_KEY20                  (IIM_BASE_ADDR+0x0C54U)    
#define IIM_SJC_RESP_0             (IIM_BASE_ADDR+0x0C58U)    
#define IIM_SJC_RESP_1             (IIM_BASE_ADDR+0x0C5CU)    
#define IIM_SJC_RESP_2             (IIM_BASE_ADDR+0x0C60U)    
#define IIM_SJC_RESP_3             (IIM_BASE_ADDR+0x0C64U)    
#define IIM_SJC_RESP_4             (IIM_BASE_ADDR+0x0C68U)    
#define IIM_SJC_RESP_5             (IIM_BASE_ADDR+0x0C6CU)    
#define IIM_SJC_RESP_6             (IIM_BASE_ADDR+0x0C70U)    
#define IIM_SJC_RESP_7             (IIM_BASE_ADDR+0x0C74U)    
#define IIM_FB2UC30                (IIM_BASE_ADDR+0x0C78U)    
#define IIM_FB2UC255               (IIM_BASE_ADDR+0x0FFCU)    
#define IIM_FBAC2                  (IIM_BASE_ADDR+0x1000U)    
#define IIM_XCORD                  (IIM_BASE_ADDR+0x1004U)    
#define IIM_YCORD                  (IIM_BASE_ADDR+0x1008U)    
#define IIM_FAB                    (IIM_BASE_ADDR+0x100CU)    
#define IIM_WAFER                  (IIM_BASE_ADDR+0x1010U)    
#define IIM_LOT0                   (IIM_BASE_ADDR+0x1014U)    
#define IIM_LOT1                   (IIM_BASE_ADDR+0x1018U)    
#define IIM_LOT2                   (IIM_BASE_ADDR+0x101CU)    
#define IIM_PROB                   (IIM_BASE_ADDR+0x1020U)    
#define IIM_FT                     (IIM_BASE_ADDR+0x1024U)    
#define IIM_FB1UC10                (IIM_BASE_ADDR+0x1028U)    
#define IIM_FB1UC255               (IIM_BASE_ADDR+0x13FCU)    
#define IIM_FBAC3                  (IIM_BASE_ADDR+0x1400U)    
#define IIM_FB3UC1                 (IIM_BASE_ADDR+0x1404U)    
#define IIM_FB3UC255               (IIM_BASE_ADDR+0x17FCU) 


//------------------------------------------------------------------------------
// REGISTER FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// REGISTER FIELD WIDTHS
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------


#endif /*IMX25_IIM_H_*/


