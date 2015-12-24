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
// $RCSfile: db_shared/Hardware_Header_Files/iMX27/iMX27_esdramc.h $
// $Revision: 1.1 $
// $Date: 2008/09/22 16:35:07EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx27_esdramc.h
//
// Provides definitions for the Enhanced SDRAM Controller (ESDRAMC)
//
// Refer to Chapter 18 Page 534 of the iMX27 Reference Manual
//
//------------------------------------------------------------------------------
#ifndef IMX27_ESDRAMC_H_
#define IMX27_ESDRAMC_H_

//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------
// #########################################
// # ESDCTL                                #
// # $D800_1000 to $D800_1FFF              #
// #########################################
#define ESDCTL_BASE_ADDR         0xD8001000
#define ESD_ESDCTL0              (ESDCTL_BASE_ADDR+0x00)    // 
#define ESD_ESDCFG0              (ESDCTL_BASE_ADDR+0x04)    // 
#define ESD_ESDCTL1              (ESDCTL_BASE_ADDR+0x08)    // 
#define ESD_ESDCFG1              (ESDCTL_BASE_ADDR+0x0C)    // 
#define ESD_ESDMISC              (ESDCTL_BASE_ADDR+0x10)    // 
#define CONFIG_DATA_MDDR1        (ESDCTL_BASE_ADDR+0x20)    // 
#define CONFIG_DATA_MDDR2        (ESDCTL_BASE_ADDR+0x24)    // 
#define CONFIG_DATA_MDDR3        (ESDCTL_BASE_ADDR+0x28)    // 
#define CONFIG_DATA_MDDR4        (ESDCTL_BASE_ADDR+0x2C)    // 
#define CONFIG_DATA_MDDR5        (ESDCTL_BASE_ADDR+0x30)    // 
#define DEL_CYCL_LEN_MDDR        (ESDCTL_BASE_ADDR+0x34)    // 
#define CONFIG_DATA_MDDR6        (ESDCTL_BASE_ADDR+0x38)    //

#endif /*IMX27_ESDRAMC_H_*/


