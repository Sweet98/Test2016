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
// $RCSfile: db_shared/Hardware_Header_Files/iMX27/iMX27_pllcrc.h $
// $Revision: 1.1 $
// $Date: 2008/09/22 16:35:09EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  iMX27_pllcrc.h
//
// Provides definitions for the PLL, Clock & Reset controller module based on MX27.
// Refer to Chapter 3 Page 107 of the iMX27 Reference Manual
//
// Clock  Reset Module(CRM)
// $1002_7000 to $1002_7FFF
// Shares AIPI Slot with System Control
// System Control is defined in iMX27_sysctrl.h 
//
//------------------------------------------------------------------------------

#ifndef IMX27_PLLCRC_H_
#define IMX27_PLLCRC_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// REGISTERS
//------------------------------------------------------------------------------
#define CRM_BASE_ADDR            0x10027000
#define CRM_CSCR                 (CRM_BASE_ADDR+0x00)       // 32bit Clock Source Control Reg
#define CRM_MPCTL0               (CRM_BASE_ADDR+0x04)       // 32bit MCU PLL Control Reg
#define CRM_MPCTL1               (CRM_BASE_ADDR+0x08)       // 32bit MCU PLL
#define CRM_SPCTL0               (CRM_BASE_ADDR+0x0C)       // 32bit Serial Perpheral PLL Ctrl 0
#define CRM_SPCTL1               (CRM_BASE_ADDR+0x10)       // 32bit Serial Perpheral PLL Ctrl 1
#define CRM_OSC26MCTL            (CRM_BASE_ADDR+0x14)       // 32bit Osc 26M register
#define CRM_PCDR0                (CRM_BASE_ADDR+0x18)       // 32bit Serial Perpheral Clk Div Reg 0
#define CRM_PCDR1                (CRM_BASE_ADDR+0x1C)       // 32bit Serial Perpheral Clk Div Reg 1
#define CRM_PCCR0                (CRM_BASE_ADDR+0x20)       // 32bit Perpheral Clk Control Reg 0
#define CRM_PCCR1                (CRM_BASE_ADDR+0x24)       // 32bit Perpheral Clk Control Reg 1
#define CRM_CCSR                 (CRM_BASE_ADDR+0x28)       // 32bit Clock Control Status Reg
#define CRM_PMCTL                (CRM_BASE_ADDR+0x2C)       // 32bit PMOS Control Reg
#define CRM_PMCOUNT              (CRM_BASE_ADDR+0x30)       // 32bit PMOS Counter Reg
#define CRM_WKGDCTL              (CRM_BASE_ADDR+0x34)       // 32bit Wakeup Guard Mode Control Reg


//------------------------------------------------------------------------------
// REGISTER FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
// CSCR
#define CRM_CSCR_MPEN_LSH                    0
#define CRM_CSCR_SPEN_LSH                    1
#define CRM_CSCR_FPM_EN_LSH                  2
#define CRM_CSCR_OSC26M_DIS_LSH              3
#define CRM_CSCR_OSC26M_DIV1P5_LSH           4
#define CRM_CSCR_AHBDIV_LSH                  8
#define CRM_CSCR_ARMDIV_LSH                  12
#define CRM_CSCR_ARMSRC_LSH                  15
#define CRM_CSCR_MCU_SEL_LSH                 16
#define CRM_CSCR_SP_SEL_LSH                  17
#define CRM_CSCR_MPLL_RESTART_LSH            18
#define CRM_CSCR_SPLL_RESTART_LSH            19
#define CRM_CSCR_MSHC_SEL_LSH                20
#define CRM_CSCR_H264_SEL_LSH                21
#define CRM_CSCR_SSI1_SEL_LSH                22
#define CRM_CSCR_SSI2_SEL_LSH                23
#define CRM_CSCR_SD_CNT_LSH                  24
#define CRM_CSCR_USB_DIV_LSH                 28

// MPCTL0
#define CRM_MPCTL0_MFN_LSH                   0
#define CRM_MPCTL0_MFI_LSH                   10
#define CRM_MPCTL0_MFD_LSH                   16
#define CRM_MPCTL0_PD_LSH                    26
#define CRM_MPCTL0_CPLM_LSH                  31

// MPCTL1
#define CRM_MPCTL1_BRMO_LSH                  6
#define CRM_MPCTL1_LF_LSH                    15

// SPCTL0
#define CRM_SPCTL0_MFN_LSH                   0
#define CRM_SPCTL0_MFI_LSH                   10
#define CRM_SPCTL0_MFD_LSH                   16
#define CRM_SPCTL0_PD_LSH                    26
#define CRM_SPCTL0_CPLM_LSH                  31

// SPCTL1
#define CRM_SPCTL1_BRMO_LSH                  6
#define CRM_SPCTL1_LF_LSH                    15

// OSC26MCTL
#define CRM_OSC26MCTL_ANATEST_LSH            0
#define CRM_OSC26MCTL_AGC_LSH                8
#define CRM_OSC26MCTL_OSC26M_PEAK_LSH        16

// PCDR0
#define CRM_PCDR0_MSHCDIV_LSH                0
#define CRM_PCDR0_NFCDIV_LSH                 6
#define CRM_PCDR0_H264DIV_LSH                10
#define CRM_PCDR0_SSI1DIV_LSH                16
#define CRM_PCDR0_CLKO_DIV_LSH               22
#define CRM_PCDR0_CLKO_EN_LSH                25
#define CRM_PCDR0_SSI2DIV_LSH                26

// PCDR1
#define CRM_PCDR1_PERDIV1_LSH                0
#define CRM_PCDR1_PERDIV2_LSH                8
#define CRM_PCDR1_PERDIV3_LSH                16
#define CRM_PCDR1_PERDIV4_LSH                24

// PCCR0
#define CRM_PCCR0_SSI2_EN_LSH                0
#define CRM_PCCR0_SSI1_EN_LSH                1
#define CRM_PCCR0_SLCDC_EN_LSH               2
#define CRM_PCCR0_SDHC3_EN_LSH               3
#define CRM_PCCR0_SDHC2_EN_LSH               4
#define CRM_PCCR0_SDHC1_EN_LSH               5
#define CRM_PCCR0_SCC_EN_LSH                 6
#define CRM_PCCR0_SAHARA_EN_LSH              7
#define CRM_PCCR0_RTIC_EN_LSH                8
#define CRM_PCCR0_RTC_EN_LSH                 9
#define CRM_PCCR0_PWM_EN_LSH                 11
#define CRM_PCCR0_OWIRE_EN_LSH               12
#define CRM_PCCR0_MSHC_EN_LSH                13
#define CRM_PCCR0_LCDC_EN_LSH                14
#define CRM_PCCR0_KPP_EN_LSH                 15
#define CRM_PCCR0_IIM_EN_LSH                 16
#define CRM_PCCR0_I2C2_EN_LSH                17
#define CRM_PCCR0_I2C1_EN_LSH                18
#define CRM_PCCR0_GPT6_EN_LSH                19
#define CRM_PCCR0_GPT5_EN_LSH                20
#define CRM_PCCR0_GPT4_EN_LSH                21
#define CRM_PCCR0_GPT3_EN_LSH                22
#define CRM_PCCR0_GPT2_EN_LSH                23
#define CRM_PCCR0_GPT1_EN_LSH                24
#define CRM_PCCR0_GPIO_EN_LSH                25
#define CRM_PCCR0_FEC_EN_LSH                 26
#define CRM_PCCR0_EMMA_EN_LSH                27
#define CRM_PCCR0_DMA_EN_LSH                 28
#define CRM_PCCR0_CSPI3_EN_LSH               29
#define CRM_PCCR0_CSPI2_EN_LSH               30
#define CRM_PCCR0_CSPI1_EN_LSH               31

// PCCR1
#define CRM_PCCR1_MSHC_BAUDEN_LSH            2
#define CRM_PCCR1_NFC_BAUDEN_LSH             3
#define CRM_PCCR1_SSI2_BAUDEN_LSH            4
#define CRM_PCCR1_SSI1_BAUDEN_LSH            5
#define CRM_PCCR1_H264_BAUDEN_LSH            6
#define CRM_PCCR1_PERCLK4_EN_LSH             7
#define CRM_PCCR1_PERCLK3_EN_LSH             8
#define CRM_PCCR1_PERCLK2_EN_LSH             9
#define CRM_PCCR1_PERCLK1_EN_LSH             10
#define CRM_PCCR1_HCLK_USB_LSH               11
#define CRM_PCCR1_HCLK_SLCDC_LSH             12
#define CRM_PCCR1_HCLK_SAHARA_LSH            13
#define CRM_PCCR1_HCLK_RTIC_LSH              14
#define CRM_PCCR1_HCLK_LCDC_LSH              15
#define CRM_PCCR1_HCLK_H264_LSH              16
#define CRM_PCCR1_HCLK_FEC_LSH               17
#define CRM_PCCR1_HCLK_EMMA_LSH              18
#define CRM_PCCR1_HCLK_EMI_LSH               19
#define CRM_PCCR1_HCLK_DMA_LSH               20
#define CRM_PCCR1_HCLK_CSI_LSH               21
#define CRM_PCCR1_HCLK_BROM_LSH              22
#define CRM_PCCR1_HCLK_ATA_LSH               23
#define CRM_PCCR1_WDT_EN_LSH                 24
#define CRM_PCCR1_USB_EN_LSH                 25
#define CRM_PCCR1_UART6_EN_LSH               26
#define CRM_PCCR1_UART5_EN_LSH               27
#define CRM_PCCR1_UART4_EN_LSH               28
#define CRM_PCCR1_UART3_EN_LSH               29
#define CRM_PCCR1_UART2_EN_LSH               30
#define CRM_PCCR1_UART1_EN_LSH               31

// CCSR
#define CRM_CCSR_CLKO_SEL_LSH                0
#define CRM_CCSR_CLKMODE_LSH                 8
#define CRM_CCSR_32K_SR_LSH                  15

// WKGDCTL
#define CRM_WKGDCTL_WKGD_EN_LSH              0


//------------------------------------------------------------------------------
// REGISTER FIELD WIDTHS
//------------------------------------------------------------------------------
// CSCR
#define CRM_CSCR_MPEN_WID                    Width_1
#define CRM_CSCR_SPEN_WID                    Width_1
#define CRM_CSCR_FPM_EN_WID                  Width_1
#define CRM_CSCR_OSC26M_DIS_WID              Width_1
#define CRM_CSCR_OSC26M_DIV1P5_WID           Width_1
#define CRM_CSCR_AHBDIV_WID                  Width_2
#define CRM_CSCR_ARMDIV_WID                  Width_2
#define CRM_CSCR_ARMSRC_WID                  Width_1
#define CRM_CSCR_MCU_SEL_WID                 Width_1
#define CRM_CSCR_SP_SEL_WID                  Width_1
#define CRM_CSCR_MPLL_RESTART_WID            Width_1
#define CRM_CSCR_SPLL_RESTART_WID            Width_1
#define CRM_CSCR_MSHC_SEL_WID                Width_1
#define CRM_CSCR_H264_SEL_WID                Width_1
#define CRM_CSCR_SSI1_SEL_WID                Width_1
#define CRM_CSCR_SSI2_SEL_WID                Width_1
#define CRM_CSCR_SD_CNT_WID                  Width_2
#define CRM_CSCR_USB_DIV_WID                 Width_3

// MPCTL0
#define CRM_MPCTL0_MFN_WID                   Width_10
#define CRM_MPCTL0_MFI_WID                   Width_4
#define CRM_MPCTL0_MFD_WID                   Width_10
#define CRM_MPCTL0_PD_WID                    Width_4
#define CRM_MPCTL0_CPLM_WID                  Width_1

// MPCTL1
#define CRM_MPCTL1_BRMO_WID                  Width_1
#define CRM_MPCTL1_LF_WID                    Width_1

// SPCTL0
#define CRM_SPCTL0_MFN_WID                   Width_10
#define CRM_SPCTL0_MFI_WID                   Width_4
#define CRM_SPCTL0_MFD_WID                   Width_10
#define CRM_SPCTL0_PD_WID                    Width_4
#define CRM_SPCTL0_CPLM_WID                  Width_1

// SPCTL1
#define CRM_SPCTL1_BRMO_WID                  Width_1
#define CRM_SPCTL1_LF_WID                    Width_1

// OSC26MCTL
#define CRM_OSC26MCTL_ANATEST_WID            Width_6
#define CRM_OSC26MCTL_AGC_WID                Width_6
#define CRM_OSC26MCTL_OSC26M_PEAK_WID        Width_2

// PCDR0
#define CRM_PCDR0_MSHCDIV_WID                Width_5
#define CRM_PCDR0_NFCDIV_WID                 Width_4
#define CRM_PCDR0_H264DIV_WID                Width_6
#define CRM_PCDR0_SSI1DIV_WID                Width_6
#define CRM_PCDR0_CLKO_DIV_WID               Width_3
#define CRM_PCDR0_CLKO_EN_WID                Width_1
#define CRM_PCDR0_SSI2DIV_WID                Width_6

// PCDR1
#define CRM_PCDR1_PERDIV1_WID                Width_6
#define CRM_PCDR1_PERDIV2_WID                Width_6
#define CRM_PCDR1_PERDIV3_WID                Width_6
#define CRM_PCDR1_PERDIV4_WID                Width_6

// PCCR0
#define CRM_PCCR0_SSI2_EN_WID                Width_1
#define CRM_PCCR0_SSI1_EN_WID                Width_1
#define CRM_PCCR0_SLCDC_EN_WID               Width_1
#define CRM_PCCR0_SDHC3_EN_WID               Width_1
#define CRM_PCCR0_SDHC2_EN_WID               Width_1
#define CRM_PCCR0_SDHC1_EN_WID               Width_1
#define CRM_PCCR0_SCC_EN_WID                 Width_1
#define CRM_PCCR0_SAHARA_EN_WID              Width_1
#define CRM_PCCR0_RTIC_EN_WID                Width_1
#define CRM_PCCR0_RTC_EN_WID                 Width_1
#define CRM_PCCR0_PWM_EN_WID                 Width_1
#define CRM_PCCR0_OWIRE_EN_WID               Width_1
#define CRM_PCCR0_MSHC_EN_WID                Width_1
#define CRM_PCCR0_LCDC_EN_WID                Width_1
#define CRM_PCCR0_KPP_EN_WID                 Width_1
#define CRM_PCCR0_IIM_EN_WID                 Width_1
#define CRM_PCCR0_I2C2_EN_WID                Width_1
#define CRM_PCCR0_I2C1_EN_WID                Width_1
#define CRM_PCCR0_GPT6_EN_WID                Width_1
#define CRM_PCCR0_GPT5_EN_WID                Width_1
#define CRM_PCCR0_GPT4_EN_WID                Width_1
#define CRM_PCCR0_GPT3_EN_WID                Width_1
#define CRM_PCCR0_GPT2_EN_WID                Width_1
#define CRM_PCCR0_GPT1_EN_WID                Width_1
#define CRM_PCCR0_GPIO_EN_WID                Width_1
#define CRM_PCCR0_FEC_EN_WID                 Width_1
#define CRM_PCCR0_EMMA_EN_WID                Width_1
#define CRM_PCCR0_DMA_EN_WID                 Width_1
#define CRM_PCCR0_CSPI3_EN_WID               Width_1
#define CRM_PCCR0_CSPI2_EN_WID               Width_1
#define CRM_PCCR0_CSPI1_EN_WID               Width_1

// PCCR1
#define CRM_PCCR1_MSHC_BAUDEN_WID            Width_1
#define CRM_PCCR1_NFC_BAUDEN_WID             Width_1
#define CRM_PCCR1_SSI2_BAUDEN_WID            Width_1
#define CRM_PCCR1_SSI1_BAUDEN_WID            Width_1
#define CRM_PCCR1_H264_BAUDEN_WID            Width_1
#define CRM_PCCR1_PERCLK4_EN_WID             Width_1
#define CRM_PCCR1_PERCLK3_EN_WID             Width_1
#define CRM_PCCR1_PERCLK2_EN_WID             Width_1
#define CRM_PCCR1_PERCLK1_EN_WID             Width_1
#define CRM_PCCR1_HCLK_USB_WID               Width_1
#define CRM_PCCR1_HCLK_SLCDC_WID             Width_1
#define CRM_PCCR1_HCLK_SAHARA_WID            Width_1
#define CRM_PCCR1_HCLK_RTIC_WID              Width_1
#define CRM_PCCR1_HCLK_LCDC_WID              Width_1
#define CRM_PCCR1_HCLK_H264_WID              Width_1
#define CRM_PCCR1_HCLK_FEC_WID               Width_1
#define CRM_PCCR1_HCLK_EMMA_WID              Width_1
#define CRM_PCCR1_HCLK_EMI_WID               Width_1
#define CRM_PCCR1_HCLK_DMA_WID               Width_1
#define CRM_PCCR1_HCLK_CSI_WID               Width_1
#define CRM_PCCR1_HCLK_BROM_WID              Width_1
#define CRM_PCCR1_HCLK_ATA_WID               Width_1
#define CRM_PCCR1_WDT_EN_WID                 Width_1
#define CRM_PCCR1_USB_EN_WID                 Width_1
#define CRM_PCCR1_UART6_EN_WID               Width_1
#define CRM_PCCR1_UART5_EN_WID               Width_1
#define CRM_PCCR1_UART4_EN_WID               Width_1
#define CRM_PCCR1_UART3_EN_WID               Width_1
#define CRM_PCCR1_UART2_EN_WID               Width_1
#define CRM_PCCR1_UART1_EN_WID               Width_1

// CCSR
#define CRM_CCSR_CLKO_SEL_WID                Width_5
#define CRM_CCSR_CLKMODE_WID                 Width_2
#define CRM_CCSR_32K_SR_WID                  Width_1

// WKGDCTL
#define CRM_WKGDCTL_WKGD_EN_WID              Width_1

//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------
// CSCR
#define CRM_CSCR_MPEN_DISABLE                0
#define CRM_CSCR_MPEN_ENABLE                 1

#define CRM_CSCR_SPEN_DISABLE                0
#define CRM_CSCR_SPEN_ENABLE                 1

#define CRM_CSCR_FPM_EN_DISABLE              0
#define CRM_CSCR_FPM_EN_ENABLE               1

#define CRM_CSCR_OSC26M_DIS_ENABLE           0
#define CRM_CSCR_OSC26M_DIS_DISABLE          1

#define CRM_CSCR_OSC26M_DIV1P5_DIV1          0
#define CRM_CSCR_OSC26M_DIV1P5_DIVP5         1

#define CRM_CSCR_IPDIV_TESTONLY              0
#define CRM_CSCR_IPDIV_DIV2                  1

#define CRM_CSCR_MCU_SEL_SRC_FPM             0
#define CRM_CSCR_MCU_SEL_SRC_OSC26           1

#define CRM_CSCR_SP_SEL_SRC_FPM              0
#define CRM_CSCR_SP_SEL_SRC_OSC26            1

#define CRM_CSCR_MPLL_RESTART_NOEFFECT       0
#define CRM_CSCR_MPLL_RESTART_EN             1

#define CRM_CSCR_SPLL_RESTART_NOEFFECT       0
#define CRM_CSCR_SPLL_RESTART_EN             1

#define CRM_CSCR_MSHC_SEL_SRC_SPLL           0
#define CRM_CSCR_MSHC_SEL_SRC_MPLL           1

#define CRM_CSCR_H264_SEL_SRC_SPLL           0
#define CRM_CSCR_H264_SEL_SRC_MPLL           1

#define CRM_CSCR_SSI1_SEL_SRC_SPLL           0
#define CRM_CSCR_SSI1_SEL_SRC_MPLL           1

#define CRM_CSCR_SSI2_SEL_SRC_SPLL           0
#define CRM_CSCR_SSI2_SEL_SRC_MPLL           1

#define CRM_CSCR_SD_CNT_AFT_NEXT_EDG         0
#define CRM_CSCR_SD_CNT_AFT_2ND_EDG          1
#define CRM_CSCR_SD_CNT_AFT_3RD_EDG          2
#define CRM_CSCR_SD_CNT_AFT_4TH_EDG          3

// MPCTL0
#define CRM_MPCTL0_CPLM_MODE_FOL             0
#define CRM_MPCTL0_CPLM_MODE_FPL             1

// MPCTL1
#define CRM_MPCTL1_BRMO_1ST_ORDER            0
#define CRM_MPCTL1_BRMO_2ND_ORDER            1

#define CRM_MPCTL1_LF_MPLL_NOT_LOCKED        0
#define CRM_MPCTL1_LF_MPLL_LOCKED            1

// SPCTL0
#define CRM_SPCTL0_CPLM_MODE_FOL             0
#define CRM_SPCTL0_CPLM_MODE_FPL             1

// SPCTL1
#define CRM_SPCTL1_BRMO_1ST_ORDER            0
#define CRM_SPCTL1_BRMO_2ND_ORDER            1

#define CRM_SPCTL1_LF_SPLL_NOT_LOCKED        0
#define CRM_SPCTL1_LF_SPLL_LOCKED            1

// PCCR0 and PCCR1
#define CRM_DISABLE         				 0
#define CRM_ENABLE          				 1

// WKGDCTL
#define CRM_WKGDCTL_WKGD_EN_DISABLE          0
#define CRM_WKGDCTL_WKGD_EN_ENABLE           1


	

#endif /*IMX27_CRM_H_*/


