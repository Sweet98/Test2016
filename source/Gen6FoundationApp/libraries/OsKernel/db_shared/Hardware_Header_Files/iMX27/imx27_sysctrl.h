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
// $RCSfile: db_shared/Hardware_Header_Files/iMX27/imx27_sysctrl.h $
// $Revision: 1.1 $
// $Date: 2008/09/22 16:35:10EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx27_sysctrl.h
//
// Provides definitions for the System Control Registers:
//		• Chip ID
//		• Multiplexing of I/O signals
//		• I/O Drive Strength
//		• I/O Pull Enable Control
//		• Well Bias Control
//		• System boot mode selection
//		• DPTC Control
// Refer to Chapter 4 Page 143 of the iMX27 Reference Manual
//
// System Control
// $1002_7000 to $1002_7FFF
// Shares AIPI Slot with the Clock and Reset Modual (CRM)
// Clock Control is defined in iMX27_PLLCRC.h 
//------------------------------------------------------------------------------
#ifndef IMX27_SYSCTRL_H_
#define IMX27_SYSCTRL_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// REGISTERS
//------------------------------------------------------------------------------
#define SYS_BASE_ADDR            0x10027800
#define SYS_FMCR                 (SYS_BASE_ADDR+0x14)       // Functional Muxing Control Reg
#define SYS_GPCR                 (SYS_BASE_ADDR+0x18)       // Global Peripheral Control Reg
#define SYS_WBCR                 (SYS_BASE_ADDR+0x1C)       // Well Bias Control Reg
#define SYS_DSCR1                (SYS_BASE_ADDR+0x20)       // Drive Strength Crtl Reg 1
#define SYS_DSCR2                (SYS_BASE_ADDR+0x24)       // Drive Strength Crtl Reg 2
#define SYS_DSCR3                (SYS_BASE_ADDR+0x28)       // Drive Strength Crtl Reg 3
#define SYS_DSCR4                (SYS_BASE_ADDR+0x2C)       // Drive Strength Crtl Reg 4
#define SYS_DSCR5                (SYS_BASE_ADDR+0x30)       // Drive Strength Crtl Reg 5
#define SYS_DSCR6                (SYS_BASE_ADDR+0x34)       // Drive Strength Crtl Reg 6
#define SYS_DSCR7                (SYS_BASE_ADDR+0x38)       // Drive Strength Crtl Reg 7
#define SYS_DSCR8                (SYS_BASE_ADDR+0x3C)       // Drive Strength Crtl Reg 8
#define SYS_DSCR9                (SYS_BASE_ADDR+0x40)       // Drive Strength Crtl Reg 9
#define SYS_DSCR10               (SYS_BASE_ADDR+0x44)       // Drive Strength Crtl Reg 10
#define SYS_DSCR11               (SYS_BASE_ADDR+0x48)       // Drive Strength Crtl Reg 11
#define SYS_DSCR12               (SYS_BASE_ADDR+0x4c)       // Drive Strength Crtl Reg 12
#define SYS_DSCR13               (SYS_BASE_ADDR+0x50)       // Drive Strength Crtl Reg 13
#define SYS_PSCR                 (SYS_BASE_ADDR+0x54)       // Pull Strength Control Reg
#define SYS_PCSR                 (SYS_BASE_ADDR+0x58)       // Priority Control/select Reg
#define SYS_PMCR                 (SYS_BASE_ADDR+0x60)       // Power Management Control Reg
#define SYS_DCVR0                (SYS_BASE_ADDR+0x64)       // DPTC Comparator Value Reg 0
#define SYS_DCVR1                (SYS_BASE_ADDR+0x68)       // DPTC Comparator Value Reg 1
#define SYS_DCVR2                (SYS_BASE_ADDR+0x6C)       // DPTC Comparator Value Reg 2
#define SYS_DCVR3                (SYS_BASE_ADDR+0x70)       // DPTC Comparator Value Reg 3


//------------------------------------------------------------------------------
// REGISTER BIT FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
// CID
#define SYSCTRL_CID_MANUFACTURE_ID_LSH          0
#define SYSCTRL_CID_PART_NUMBER_LSH             12
#define SYSCTRL_CID_VERSION_ID_LSH              28

// FMCR
#define SYSCTRL_FMCR_SDCS0_SEL_LSH              0
#define SYSCTRL_FMCR_SDCS1_SEL_LSH              1
#define SYSCTRL_FMCR_SLCDC_SEL_LSH              2
#define SYSCTRL_FMCR_NF_16BIT_SEL_LSH           4
#define SYSCTRL_FMCR_NF_FMS_LSH                 5
#define SYSCTRL_FMCR_IOIS16_CTL_LSH             8
#define SYSCTRL_FMCR_PC_BVD2_CTL_LSH            9
#define SYSCTRL_FMCR_PC_BVD1_CTL_LSH            10
#define SYSCTRL_FMCR_PC_VS2_CTL_LSH             11
#define SYSCTRL_FMCR_PC_VS1_CTL_LSH             12
#define SYSCTRL_FMCR_PC_READY_CTL_LSH           13
#define SYSCTRL_FMCR_PC_WAIT_B_CTL_LSH          14
#define SYSCTRL_FMCR_KP_ROW6_CTL_LSH            16
#define SYSCTRL_FMCR_KP_ROW7_CTL_LSH            17
#define SYSCTRL_FMCR_KP_COL6_CTL_LSH            18
#define SYSCTRL_FMCR_UART4_RTS_CTL_LSH          24
#define SYSCTRL_FMCR_UART4_RXD_CTL_LSH          25

// GPCR
#define SYSCTRL_GPCR_CLK_DDR_MODE_LSH           1
#define SYSCTRL_GPCR_DDR_MODE_LSH               2
#define SYSCTRL_GPCR_CLOCK_GATING_EN_LSH        3
#define SYSCTRL_GPCR_DMA_BURST_OVERRIDE_LSH     8
#define SYSCTRL_GPCR_PP_BURST_OVERRIDE_LSH      9
#define SYSCTRL_GPCR_USB_BURST_OVERRIDE_LSH     10
#define SYSCTRL_GPCR_ETM9_PAD_EN_LSH            11
#define SYSCTRL_GPCR_BOOT_LSH                   16

// WBCR
#define SYSCTRL_WBCR_CRM_WBM_LSH                0
#define SYSCTRL_WBCR_CRM_WBFA_LSH               3
#define SYSCTRL_WBCR_CRM_SPA_BIT01_LSH          8
#define SYSCTRL_WBCR_CRM_SPA_BIT23_LSH          10
#define SYSCTRL_WBCR_CRM_WBM_EMI_LSH            16
#define SYSCTRL_WBCR_CRM_WBFA_EMI_LSH           19
#define SYSCTRL_WBCR_CRM_SPA_EMI_BIT01_LSH      24
#define SYSCTRL_WBCR_CRM_SPA_EMI_BIT23_LSH      26

//DSCR1
#define SYSCTRL_DSCR1_DS_SLOW11_DVS_PMIC_LSH    20
#define SYSCTRL_DSCR1_DS_SLOW10_SDHC1_CSPI3_LSH 18
#define SYSCTRL_DSCR1_DS_SLOW9_JTAG_LSH     	16
#define SYSCTRL_DSCR1_DS_SLOW8_PWM_LSH 	 		14
#define SYSCTRL_DSCR1_DS_SLOW7_CSPI1_LSH    	12
#define SYSCTRL_DSCR1_DS_SLOW6_SSI1_LSH       	10
#define SYSCTRL_DSCR1_DS_SLOW5_GPT1_LSH      	8
#define SYSCTRL_DSCR1_DS_SLOW4_USBH1_LSH    	6
#define SYSCTRL_DSCR1_DS_SLOW3_CSI_LSH          4
#define SYSCTRL_DSCR1_DS_SLOW2_SDHC2_MSHC_LSH  	2
#define SYSCTRL_DSCR1_DS_SLOW1_LCDC_LSH     	0

//PSCR
#define SYSCTRL_PSCR_SD2_D0_PUENCR_LSH		14
#define SYSCTRL_PSCR_SD2_D1_PUENCR_LSH		12
#define SYSCTRL_PSCR_SD2_D2_PUENCR_LSH		10
#define SYSCTRL_PSCR_SD2_D3_PUENCR_LSH		8
#define SYSCTRL_PSCR_SD2_CMD_PUENCR_LSH		6
#define SYSCTRL_PSCR_SD2_CLK_PUENCR_LSH		4
#define SYSCTRL_PSCR_SD1_D3_PUENCR_LSH		2
#define SYSCTRL_PSCR_ATA_DAT3_PUENCR_LSH	0

// PCSR
#define SYSCTRL_PCSR_S0_AMPR_SEL_LSH            16
#define SYSCTRL_PCSR_S1_AMPR_SEL_LSH            17
#define SYSCTRL_PCSR_S2_AMPR_SEL_LSH            18
#define SYSCTRL_PCSR_S3_AMPR_SEL_LSH            19

// PMCR
#define SYSCTRL_PMCR_DPTEN_LSH                  0
#define SYSCTRL_PMCR_DIE_LSH                    1
#define SYSCTRL_PMCR_DIM_LSH                    2
#define SYSCTRL_PMCR_DRCE0_LSH                  4
#define SYSCTRL_PMCR_DRCE1_LSH                  5
#define SYSCTRL_PMCR_DRCE2_LSH                  6
#define SYSCTRL_PMCR_DRCE3_LSH                  7
#define SYSCTRL_PMCR_RCLKON_LSH                 8
#define SYSCTRL_PMCR_DCR_LSH                    9
#define SYSCTRL_PMCR_REFCOUNTER_LSH             16
#define SYSCTRL_PMCR_LO_LSH                     28
#define SYSCTRL_PMCR_UP_LSH                     29
#define SYSCTRL_PMCR_EM_LSH                     30
#define SYSCTRL_PMCR_MC_LSH                     31

// DCVR
#define SYSCTRL_DCVR_ELV_LSH                    0
#define SYSCTRL_DCVR_LLV_LSH                    10
#define SYSCTRL_DCVR_ULV_LSH                    21


//------------------------------------------------------------------------------
// REGISTER BIT FIELD WIDTHS
//------------------------------------------------------------------------------
// CID
#define SYSCTRL_CID_MANUFACTURE_ID_WID          Width_12
#define SYSCTRL_CID_PART_NUMBER_WID             Width_16
#define SYSCTRL_CID_VERSION_ID_WID              Width_4

// FMCR
#define SYSCTRL_FMCR_SDCS0_SEL_WID              Width_1
#define SYSCTRL_FMCR_SDCS1_SEL_WID              Width_1
#define SYSCTRL_FMCR_SLCDC_SEL_WID              Width_1
#define SYSCTRL_FMCR_NF_16BIT_SEL_WID           Width_1
#define SYSCTRL_FMCR_NF_FMS_WID                 Width_1
#define SYSCTRL_FMCR_IOIS16_CTL_WID             Width_1
#define SYSCTRL_FMCR_PC_BVD2_CTL_WID            Width_1
#define SYSCTRL_FMCR_PC_BVD1_CTL_WID            Width_1
#define SYSCTRL_FMCR_PC_VS2_CTL_WID             Width_1
#define SYSCTRL_FMCR_PC_VS1_CTL_WID             Width_1
#define SYSCTRL_FMCR_PC_READY_CTL_WID           Width_1
#define SYSCTRL_FMCR_PC_WAIT_B_CTL_WID          Width_1
#define SYSCTRL_FMCR_KP_ROW6_CTL_WID            Width_1
#define SYSCTRL_FMCR_KP_ROW7_CTL_WID            Width_1
#define SYSCTRL_FMCR_KP_COL6_CTL_WID            Width_1
#define SYSCTRL_FMCR_UART4_RTS_CTL_WID          Width_1
#define SYSCTRL_FMCR_UART4_RXD_CTL_WID          Width_1

// GPCR
#define SYSCTRL_GPCR_CLK_DDR_MODE_WID           Width_1
#define SYSCTRL_GPCR_DDR_MODE_WID               Width_1
#define SYSCTRL_GPCR_CLOCK_GATING_EN_WID        Width_1
#define SYSCTRL_GPCR_DMA_BURST_OVERRIDE_WID     Width_1
#define SYSCTRL_GPCR_PP_BURST_OVERRIDE_WID      Width_1
#define SYSCTRL_GPCR_USB_BURST_OVERRIDE_WID     Width_1
#define SYSCTRL_GPCR_ETM9_PAD_EN_WID            Width_1
#define SYSCTRL_GPCR_BOOT_WID                   Width_4

// WBCR
#define SYSCTRL_WBCR_CRM_WBM_WID                Width_3
#define SYSCTRL_WBCR_CRM_WBFA_WID               Width_1
#define SYSCTRL_WBCR_CRM_SPA_BIT01_WID          Width_2
#define SYSCTRL_WBCR_CRM_SPA_BIT23_WID          Width_2
#define SYSCTRL_WBCR_CRM_WBM_EMI_WID            Width_3
#define SYSCTRL_WBCR_CRM_WBFA_EMI_WID           Width_1
#define SYSCTRL_WBCR_CRM_SPA_EMI_BIT01_WID      Width_2
#define SYSCTRL_WBCR_CRM_SPA_EMI_BIT23_WID      Width_2

//DSCR1
#define SYSCTRL_DSCR1_DS_SLOW11_DVS_PMIC_WID 	Width_2
#define SYSCTRL_DSCR1_DS_SLOW10_SDHC1_CSPI3_WID Width_2
#define SYSCTRL_DSCR1_DS_SLOW9_JTAG_WID 		Width_2
#define SYSCTRL_DSCR1_DS_SLOW8_PWM_WID 			Width_2
#define SYSCTRL_DSCR1_DS_SLOW7_CSPI1_WID 		Width_2
#define SYSCTRL_DSCR1_DS_SLOW6_SSI1_WID 		Width_2
#define SYSCTRL_DSCR1_DS_SLOW5_GPT1_WID 		Width_2
#define SYSCTRL_DSCR1_DS_SLOW4_USBH1_WID 		Width_2
#define SYSCTRL_DSCR1_DS_SLOW3_CSI_WID 			Width_2
#define SYSCTRL_DSCR1_DS_SLOW2_SDHC2_MSHC_WID 	Width_2
#define SYSCTRL_DSCR1_DS_SLOW1_LCDC_WID 		Width_2

//PSCR
#define SYSCTRL_PSCR_SD2_D0_PUENCR_WID			Width_2
#define SYSCTRL_PSCR_SD2_D1_PUENCR_WID			Width_2
#define SYSCTRL_PSCR_SD2_D2_PUENCR_WID			Width_2
#define SYSCTRL_PSCR_SD2_D3_PUENCR_WID			Width_2
#define SYSCTRL_PSCR_SD2_CMD_PUENCR_WID			Width_2
#define SYSCTRL_PSCR_SD2_CLK_PUENCR_WID			Width_2
#define SYSCTRL_PSCR_SD1_D3_PUENCR_WID			Width_2
#define SYSCTRL_PSCR_ATA_DAT3_PUENCR_WID		Width_2

// PCSR
#define SYSCTRL_PCSR_S0_AMPR_SEL_WID            Width_1
#define SYSCTRL_PCSR_S1_AMPR_SEL_WID            Width_1
#define SYSCTRL_PCSR_S2_AMPR_SEL_WID            Width_1
#define SYSCTRL_PCSR_S3_AMPR_SEL_WID            Width_1

// PMCR
#define SYSCTRL_PMCR_DPTEN_WID                  Width_1
#define SYSCTRL_PMCR_DIE_WID                    Width_1
#define SYSCTRL_PMCR_DIM_WID                    Width_2
#define SYSCTRL_PMCR_DRCE0_WID                  Width_1
#define SYSCTRL_PMCR_DRCE1_WID                  Width_1
#define SYSCTRL_PMCR_DRCE2_WID                  Width_1
#define SYSCTRL_PMCR_DRCE3_WID                  Width_1
#define SYSCTRL_PMCR_RCLKON_WID                 Width_1
#define SYSCTRL_PMCR_DCR_WID                    Width_1
#define SYSCTRL_PMCR_REFCOUNTER_WID             Width_11
#define SYSCTRL_PMCR_LO_WID                     Width_1
#define SYSCTRL_PMCR_UP_WID                     Width_1
#define SYSCTRL_PMCR_EM_WID                     Width_1
#define SYSCTRL_PMCR_MC_WID                     Width_1

//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------
// FMCR
#define SYSCTRL_FMCR_SDCS0_SEL_USE_CS2          0
#define SYSCTRL_FMCR_SDCS0_SEL_USE_CSD0         1

#define SYSCTRL_FMCR_SDCS1_SEL_USE_CS3          0
#define SYSCTRL_FMCR_SDCS1_SEL_USE_CSD1         1

#define SYSCTRL_FMCR_SLCDC_SEL_SLCDC            0
#define SYSCTRL_FMCR_SLCDC_SEL_BB               1

#define SYSCTRL_FMCR_NF_16BIT_SEL_8BIT          0
#define SYSCTRL_FMCR_NF_16BIT_SEL_16BIT         1

#define SYSCTRL_FMCR_NF_FMS_512B_PAGE           0
#define SYSCTRL_FMCR_NF_FMS_2KB_PAGE            1

#define SYSCTRL_FMCR_IOIS16_CTL_PC17_BOUT       0
#define SYSCTRL_FMCR_IOIS16_CTL_IOIS16          1

#define SYSCTRL_FMCR_PC_BVD2_CTL_PC18_BOUT      0
#define SYSCTRL_FMCR_PC_BVD2_CTL_PC_BVD2        1

#define SYSCTRL_FMCR_PC_BVD1_CTL_PC19_BOUT      0
#define SYSCTRL_FMCR_PC_BVD1_CTL_PC_BVD1        1

#define SYSCTRL_FMCR_PC_VS2_CTL_PC28_BOUT       0
#define SYSCTRL_FMCR_PC_VS2_CTL_PC_VS2          1

#define SYSCTRL_FMCR_PC_VS1_CTL_PC29_BOUT       0
#define SYSCTRL_FMCR_PC_VS1_CTL_PC_VS1          1

#define SYSCTRL_FMCR_PC_READY_CTL_PC30_BOUT     0
#define SYSCTRL_FMCR_PC_READY_CTL_PC_READY      1

#define SYSCTRL_FMCR_PC_WAIT_B_CTL_PC31_BOUT    0
#define SYSCTRL_FMCR_PC_WAIT_B_CTL_PC_WAIT_B    1

#define SYSCTRL_FMCR_KP_ROW6_CTL_PE1_ALT        0
#define SYSCTRL_FMCR_KP_ROW6_CTL_PE7_ALT        1

#define SYSCTRL_FMCR_KP_ROW7_CTL_PE2_ALT        0
#define SYSCTRL_FMCR_KP_ROW7_CTL_PE4_ALT        1

#define SYSCTRL_FMCR_KP_COL6_CTL_PE0_ALT        0
#define SYSCTRL_FMCR_KP_COL6_CTL_PE6_ALT        1

#define SYSCTRL_FMCR_UART4_RTS_CTL_PB31_AOUT    0
#define SYSCTRL_FMCR_UART4_RTS_CTL_PB26_ALT     1

#define SYSCTRL_FMCR_UART4_RXD_CTL_PB29_AOUT    0
#define SYSCTRL_FMCR_UART4_RXD_CTL_PB31_ALT     1

// GPCR
#define SYSCTRL_GPCR_CLK_DDR_MODE_IPP_DSE01     0
#define SYSCTRL_GPCR_CLK_DDR_MODE_SSTL_18       1

#define SYSCTRL_GPCR_DDR_MODE_IPP_DSE01         0
#define SYSCTRL_GPCR_DDR_MODE_SSTL_18           1

#define SYSCTRL_GPCR_CLOCK_GATING_EN_DISABLE    0
#define SYSCTRL_GPCR_CLOCK_GATING_EN_ENABLE     1

#define SYSCTRL_GPCR_DMA_BURST_OVERRIDE_BYPASS  0
#define SYSCTRL_GPCR_DMA_BURST_OVERRIDE_INCR    1

#define SYSCTRL_GPCR_PP_BURST_OVERRIDE_BYPASS   0
#define SYSCTRL_GPCR_PP_BURST_OVERRIDE_INCR     1

#define SYSCTRL_GPCR_USB_BURST_OVERRIDE_BYPASS  0
#define SYSCTRL_GPCR_USB_BURST_OVERRIDE_INCR    1

#define SYSCTRL_GPCR_ETM9_PAD_EN_DISABLE        0
#define SYSCTRL_GPCR_ETM9_PAD_EN_ENABLE         1

#define SYSCTRL_GPCR_BOOT_UART_USB1             0
#define SYSCTRL_GPCR_BOOT_UART_USB2             1
#define SYSCTRL_GPCR_BOOT_NF_8BIT_2KB_PAGE      2
#define SYSCTRL_GPCR_BOOT_NF_16BIT_2KB_PAGE     3
#define SYSCTRL_GPCR_BOOT_NF_16BIT_512B_PAGE    4
#define SYSCTRL_GPCR_BOOT_16BIT_CS0             5
#define SYSCTRL_GPCR_BOOT_32BIT_CS0             6
#define SYSCTRL_GPCR_BOOT_NF_8BIT_512B_PAGE     7

//DSCR
#define SYSCTRL_DSCR_DRIVING_STRENGTH_NORMAL 	0
#define SYSCTRL_DSCR_DRIVING_STRENGTH_HIGH 		1
#define SYSCTRL_DSCR_DRIVING_STRENGTH_MAX 		3

//PSCR
#define SYSCTRL_PSCR_100K_PD		0
#define SYSCTRL_PSCR_100K_PU		1
#define SYSCTRL_PSCR_47K_PU			2
#define SYSCTRL_PSCR_22K_PU			3

// PMCR
#define SYSCTRL_PMCR_DPTEN_DPTC_DISABLE         0
#define SYSCTRL_PMCR_DPTEN_DPTC_ENABLE          1

#define SYSCTRL_PMCR_DIE_DPTC_DISABLE           0
#define SYSCTRL_PMCR_DIE_DPTC_ENABLE            1

#define SYSCTRL_PMCR_DIM_DPTC_ALL               0
#define SYSCTRL_PMCR_DIM_DPTC_LOWER             1
#define SYSCTRL_PMCR_DIM_DPTC_UPPER             0
#define SYSCTRL_PMCR_DIM_DPTC_EMER              1

#define SYSCTRL_PMCR_DRCE_DISABLE               0
#define SYSCTRL_PMCR_DRCE_ENABLE                1

#define SYSCTRL_PMCR_RCLKON_DPTC_NORMAL         0
#define SYSCTRL_PMCR_RCLKON_DPTC_ALWAYS         1

#define SYSCTRL_PMCR_DCR_128_CLOCKS             0
#define SYSCTRL_PMCR_DCR_256_CLOCKS             1


#endif /*IMX27_SYSCTRL_H_*/


