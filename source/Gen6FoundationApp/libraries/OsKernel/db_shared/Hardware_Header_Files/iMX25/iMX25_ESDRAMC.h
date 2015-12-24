/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  HARDWARE DEFINITION FILE 
//  Date:       9/15/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: db_shared/Hardware_Header_Files/iMX25/iMX25_ESDRAMC.h $
// $Revision: 1.3 $
// $Date: 2011/06/28 13:26:52EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx25_esdramc.h
//
// Provides definitions for the Enhanced SDRAM Controller (ESDRAMC)
// As well as the NAND Flash Controller (NFC)
// And the Wireless External Interface Module (WEIM)
//
// Refer to Chapter 26 of the iMX25 Reference Manual
//
//------------------------------------------------------------------------------
#ifndef IMX25_ESDRAMC_H_
#define IMX25_ESDRAMC_H_

//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------
// #########################################
// # NFC                                   #
// #########################################
#define NFC_BASE_ADDR              0xBB000000U
#define NFC_REG_BASE_ADDR          (NFC_BASE_ADDR+0x1E00U)    
#define NFC_RAM_BUF_ADDR           (NFC_REG_BASE_ADDR+0x04U)  
#define NFC_NAND_FLASH_ADDR        (NFC_REG_BASE_ADDR+0x06U)  
#define NFC_NAND_FLASH_CMD         (NFC_REG_BASE_ADDR+0x08U)  
#define NFC_CONFIG                 (NFC_REG_BASE_ADDR+0x0AU)  
#define NFC_ECC_STAT_RSLT1         (NFC_REG_BASE_ADDR+0x0CU)  
#define NFC_ECC_STAT_RSLT2         (NFC_REG_BASE_ADDR+0x0EU)  
#define NFC_ECC_SPAS               (NFC_REG_BASE_ADDR+0x10U)  
#define NFC_NF_WR_PROT             (NFC_REG_BASE_ADDR+0x12U)  
#define NFC_NF_WR_PROT_STAT        (NFC_REG_BASE_ADDR+0x18U)  
#define NFC_NF_CONFIG1             (NFC_REG_BASE_ADDR+0x1AU)  
#define NFC_NF_CONFIG2             (NFC_REG_BASE_ADDR+0x1CU)  
#define NFC_ULOCK_START_BLK0       (NFC_REG_BASE_ADDR+0x20U)  
#define NFC_ULOCK_END_BLK0         (NFC_REG_BASE_ADDR+0x22U)  
#define NFC_ULOCK_START_BLK1       (NFC_REG_BASE_ADDR+0x24U)  
#define NFC_ULOCK_END_BLK1         (NFC_REG_BASE_ADDR+0x26U)  
#define NFC_ULOCK_START_BLK2       (NFC_REG_BASE_ADDR+0x28U)  
#define NFC_ULOCK_END_BLK2         (NFC_REG_BASE_ADDR+0x2AU)  
#define NFC_ULOCK_START_BLK3       (NFC_REG_BASE_ADDR+0x2CU)  
#define NFC_ULOCK_END_BLK3         (NFC_REG_BASE_ADDR+0x2EU) 

// #########################################
// # ESDCTL                                #
// #########################################
#define ESDCTL_BASE_ADDR           0xB8001000U
#define ESD_ESDCTL0                (ESDCTL_BASE_ADDR+0x00U)   
#define ESD_ESDCFG0                (ESDCTL_BASE_ADDR+0x04U)   
#define ESD_ESDCTL1                (ESDCTL_BASE_ADDR+0x08U)   
#define ESD_ESDCFG1                (ESDCTL_BASE_ADDR+0x0CU)   
#define ESD_ESDMISC                (ESDCTL_BASE_ADDR+0x10U)   
#define CONFIG_DATA_MDDR1          (ESDCTL_BASE_ADDR+0x20U)   
#define CONFIG_DATA_MDDR2          (ESDCTL_BASE_ADDR+0x24U)   
#define CONFIG_DATA_MDDR3          (ESDCTL_BASE_ADDR+0x28U)   
#define CONFIG_DATA_MDDR4          (ESDCTL_BASE_ADDR+0x2CU)   
#define CONFIG_DATA_MDDR5          (ESDCTL_BASE_ADDR+0x30U)   
#define DEL_CYCL_LEN_MDDR          (ESDCTL_BASE_ADDR+0x34U)   
#define CONFIG_DATA_MDDR6          (ESDCTL_BASE_ADDR+0x38U)  

// #########################################
// # WEIM                                  #
// #########################################
#define WEIM_BASE_ADDR             0xB8002000U
#define WEIM_CS0U                  (WEIM_BASE_ADDR+0x00U)     
#define WEIM_CS0L                  (WEIM_BASE_ADDR+0x04U)     
#define WEIM_CS0A                  (WEIM_BASE_ADDR+0x08U)     
#define WEIM_CS1U                  (WEIM_BASE_ADDR+0x10U)     
#define WEIM_CS1L                  (WEIM_BASE_ADDR+0x14U)     
#define WEIM_CS1A                  (WEIM_BASE_ADDR+0x18U)     
#define WEIM_CS2U                  (WEIM_BASE_ADDR+0x20U)     
#define WEIM_CS2L                  (WEIM_BASE_ADDR+0x24U)     
#define WEIM_CS2A                  (WEIM_BASE_ADDR+0x28U)     
#define WEIM_CS3U                  (WEIM_BASE_ADDR+0x30U)     
#define WEIM_CS3L                  (WEIM_BASE_ADDR+0x34U)     
#define WEIM_CS3A                  (WEIM_BASE_ADDR+0x38U)     
#define WEIM_CS4U                  (WEIM_BASE_ADDR+0x40U)     
#define WEIM_CS4L                  (WEIM_BASE_ADDR+0x44U)     
#define WEIM_CS4A                  (WEIM_BASE_ADDR+0x48U)     
#define WEIM_CS5U                  (WEIM_BASE_ADDR+0x50U)     
#define WEIM_CS5L                  (WEIM_BASE_ADDR+0x54U)     
#define WEIM_CS5A                  (WEIM_BASE_ADDR+0x58U)     
#define WEIM_EIM                   (WEIM_BASE_ADDR+0x60U)  

#endif /*IMX25_ESDRAMC_H_*/






