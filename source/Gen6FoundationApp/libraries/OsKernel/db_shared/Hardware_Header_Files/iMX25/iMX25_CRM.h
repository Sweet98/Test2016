/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  HARDWARE DEFINITION FILE 
//  Date:       9/12/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: db_shared/Hardware_Header_Files/iMX25/iMX25_CRM.h $
// $Revision: 1.12 $
// $Date: 2011/07/28 21:55:28EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  iMX25_crm.h
//
// Provides definitions for the Clock & Reset controller and Power Management
//   module based on MX25.
// Refer to Chapter 48 of the iMX25 Reference Manual
//
//------------------------------------------------------------------------------

#ifndef IMX25_CRM_H_
#define IMX25_CRM_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// REGISTERS
//------------------------------------------------------------------------------
#define CRM_BASE_ADDR              0x53F80000U
#define CRM_MPCTL                  (CRM_BASE_ADDR+0x00U)       // MCU PLL Control Register
#define CRM_UPCTL                  (CRM_BASE_ADDR+0x04U)       // USB PLL Control Register
#define CRM_CCTL                   (CRM_BASE_ADDR+0x08U)       // Clock Control Register
#define CRM_CGR0                   (CRM_BASE_ADDR+0x0CU)       // Clock Gating Register 0
#define CRM_CGR1                   (CRM_BASE_ADDR+0x10U)       // Clock Gating Register 1
#define CRM_CGR2                   (CRM_BASE_ADDR+0x14U)       // Clock Gating Register 2
#define CRM_PDR0                   (CRM_BASE_ADDR+0x18U)       // perclk Divider Register 0
#define CRM_PDR1                   (CRM_BASE_ADDR+0x1CU)       // perclk Divider Register 1
#define CRM_PDR2                   (CRM_BASE_ADDR+0x20U)       // perclk Divider Register 2
#define CRM_PDR3                   (CRM_BASE_ADDR+0x24U)       // perclk Divider Register 3
#define CRM_CRSR                   (CRM_BASE_ADDR+0x28U)       // CRM Status Register
#define CRM_CRDR                   (CRM_BASE_ADDR+0x2CU)       // CRM Debug Register
#define CRM_DCVR0                  (CRM_BASE_ADDR+0x30U)       // DPTC Comparator Value Register 0
#define CRM_DCVR1                  (CRM_BASE_ADDR+0x34U)       // DPTC Comparator Value Register 1
#define CRM_DCVR2                  (CRM_BASE_ADDR+0x38U)       // DPTC Comparator Value Register 2
#define CRM_DCVR3                  (CRM_BASE_ADDR+0x3CU)       // DPTC Comparator Value Register 3
#define CRM_LTR0                   (CRM_BASE_ADDR+0x40U)       // Load Tracking Register 0
#define CRM_LTR1                   (CRM_BASE_ADDR+0x44U)       // Load Tracking Register 1
#define CRM_LTR2                   (CRM_BASE_ADDR+0x48U)       // Load Tracking Register 2
#define CRM_LTR3                   (CRM_BASE_ADDR+0x4CU)       // Load Tracking Register 3
#define CRM_LTBR0                  (CRM_BASE_ADDR+0x50U)       // Load Tracking Buffer Register 0
#define CRM_LTBR1                  (CRM_BASE_ADDR+0x54U)       // Load Tracking Buffer Register 1
#define CRM_PMCR0                  (CRM_BASE_ADDR+0x58U)       // Power Management Control Register 0
#define CRM_PMCR1                  (CRM_BASE_ADDR+0x5CU)       // Power Management Control Register 1
#define CRM_PMCR2                  (CRM_BASE_ADDR+0x60U)       // Power Management Control Register 2
#define CRM_MCR                    (CRM_BASE_ADDR+0x64U)       // CRM MCR Register
#define CRM_LPIMR0                 (CRM_BASE_ADDR+0x68U)       // Low Power Interrupt Mask Registers
#define CRM_LPIMR1                 (CRM_BASE_ADDR+0x6CU)       // Low Power Interrupt Mask Registers


//------------------------------------------------------------------------------
// REGISTER FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
// MCU PLL Control Register (MPCTL)
#define CRM_MPCTL_MFN_LSH                   0
#define CRM_MPCTL_MFI_LSH                   10
#define CRM_MPCTL_LOCK_LSH                  15
#define CRM_MPCTL_MFD_LSH                   16
#define CRM_MPCTL_PD_LSH                    26
#define CRM_MPCTL_BRMO_LSH                  31

// USB PLL Control Register (UPCTL)
#define CRM_UPCTL_MFN_LSH                   0
#define CRM_UPCTL_MFI_LSH                   10
#define CRM_UPCTL_LOCK_LSH                  15
#define CRM_UPCTL_MFD_LSH                   16
#define CRM_UPCTL_PD_LSH                    26
#define CRM_UPCTL_BRMO_LSH                  31

// Clock Control Register - CCTL
#define CRM_CCTL_ARMSRC_LSH                 14
#define CRM_CCTL_CGCTL_LSH                  15
#define CRM_CCTL_USB_DIV_LSH                16
#define CRM_CCTL_MPLL_BYPASS_LSH            22
#define CRM_CCTL_UPLLDIS_LSH                23
#define CRM_CCTL_LPCTL_LSH                  24
#define CRM_CCTL_UPLLRST_LSH                26
#define CRM_CCTL_MPLLRST_LSH                27
#define CRM_CCTL_AHBCLKDIV_LSH              28
#define CRM_CCTL_ARMCLKDIV_LSH              30

//Clock Gating Control Register 0 - CGCR0
#define CRM_CGCR0_IPG_PER_0_CSI_LSH			0		//CMOS Sensor Interface
#define CRM_CGCR0_IPG_PER_1_EPIT_LSH		1		//Enhanced Periodic Timer
#define CRM_CGCR0_IPG_PER_2_ESAI_LSH		2		//Enhanced Audio Interface
#define CRM_CGCR0_IPG_PER_3_ESDHC1_LSH		3		//Enhanced SD Host Controller 1
#define CRM_CGCR0_IPG_PER_4_ESDHC2_LSH		4		//Enhanced SD Host Controller 2
#define CRM_CGCR0_IPG_PER_5_GPT_LSH			5		//General Purpose Timer
#define CRM_CGCR0_IPG_PER_6_I2C_LSH			6		//Inter-IC Communication
#define CRM_CGCR0_IPG_PER_7_LCDC_LSH		7		//Liquid Crystal Display Controller
#define CRM_CGCR0_IPG_PER_8_NFC_LSH			8		//NAND Flash Controller 
#define CRM_CGCR0_IPG_PER_9_OWIRE_LSH		9		//One Wire
#define CRM_CGCR0_IPG_PER_10_PWM_LSH		10		//Pulse-Width Modulator
#define CRM_CGCR0_IPG_PER_11_SIM1_LSH		11		//Subscriber Identification Module 1
#define CRM_CGCR0_IPG_PER_12_SIM2_LSH		12		//Subscriber Identification Module 2
#define CRM_CGCR0_IPG_PER_13_SSI1_LSH		13		//Synchronous Serial Interface 1
#define CRM_CGCR0_IPG_PER_14_SSI2_LSH		14		//Synchronous Serial Interface 2
#define CRM_CGCR0_IPG_PER_15_UART_LSH		15		//Universal Asynchronous Receiver/Transmitters

#define CRM_CGCR0_HCLK_AHB_ATA_LSH          16		//ATA Module
#define CRM_CGCR0_HCLK_AHB_BROM_LSH         17		//??
#define CRM_CGCR0_HCLK_AHB_CSI_LSH          18		//CMOS Sensor Interface
#define CRM_CGCR0_HCLK_AHB_EMI_LSH          19		//External Memory Interface
#define CRM_CGCR0_HCLK_AHB_ESAI_LSH         20		//Enhanced Serial Audio Interface
#define CRM_CGCR0_HCLK_AHB_ESDHC1_LSH       21		//Enhanced SD Host Controller 1
#define CRM_CGCR0_HCLK_AHB_ESDHC2_LSH       22		//Enhanced SD Host Controller 2
#define CRM_CGCR0_HCLK_AHB_FEC_LSH          23		//Ethernet
#define CRM_CGCR0_HCLK_AHB_LCDC_LSH         24		//LCD Controller 
#define CRM_CGCR0_HCLK_AHB_RITC_LSH         25		//Real Time Integrity Checker 
#define CRM_CGCR0_HCLK_AHB_SDMA_LSH         26		//Smart DMA
#define CRM_CGCR0_HCLK_AHB_SLCDC_LSH        27		//Smart LCD Controller
#define CRM_CGCR0_HCLK_AHB_USBOTG_LSH       28		//USB On The Go

//Clock Gating Control Register 1 - CGCR1
#define CRM_CGCR1_IPG_CLK_AUDMUX_LSH        0		//Digital Audio Mux
#define CRM_CGCR1_IPG_CLK_ATA_LSH          	1		//Advanced Technology Attachment
#define CRM_CGCR1_IPG_CLK_CAN1_LSH          2		//Controller Area Network 1
#define CRM_CGCR1_IPG_CLK_CAN2_LSH          3		//Controller Area Network 2
#define CRM_CGCR1_IPG_CLK_CSI_LSH          	4		//CMOS Sensor Interface
#define CRM_CGCR1_IPG_CLK_CSPI1_LSH         5		//Configurable Serial Peripheral Interface 1
#define CRM_CGCR1_IPG_CLK_CSPI2_LSH         6		//Configurable Serial Peripheral Interface 2
#define CRM_CGCR1_IPG_CLK_CSPI3_LSH         7		//Configurable Serial Peripheral Interface 3
#define CRM_CGCR1_IPG_CLK_DRYICE_LSH        8		//DRY ICE
#define CRM_CGCR1_IPG_CLK_ECT_LSH          	9		//Embedded Cross Trigger
#define CRM_CGCR1_IPG_CLK_EPIT1_LSH         10		//Enhanced Periodic Timer 1
#define CRM_CGCR1_IPG_CLK_EPIT2_LSH         11		//Enhanced Periodic Timer 2
#define CRM_CGCR1_IPG_CLK_ESAI_LSH          12		//Enhanced Serial Audio Interface
#define CRM_CGCR1_IPG_CLK_ESDHC1_LSH        13		//Enhanced SD Host Controller 1
#define CRM_CGCR1_IPG_CLK_ESDHC2_LSH        14		//Enhanced SD Host Controller 2
#define CRM_CGCR1_IPG_CLK_FEC_LSH          	15		//Ethernet
#define CRM_CGCR1_IPG_CLK_GPIO1_LSH         16		//General Purpose I/O Module 1
#define CRM_CGCR1_IPG_CLK_GPIO2_LSH         17		//General Purpose I/O Module 2
#define CRM_CGCR1_IPG_CLK_GPIO3_LSH         18		//General Purpose I/O Module 3
#define CRM_CGCR1_IPG_CLK_GPT1_LSH          19		//General Purpose Timer 1
#define CRM_CGCR1_IPG_CLK_GPT2_LSH          20		//General Purpose Timer 2
#define CRM_CGCR1_IPG_CLK_GPT3_LSH          21		//General Purpose Timer 3
#define CRM_CGCR1_IPG_CLK_GPT4_LSH          22		//General Purpose Timer 4
#define CRM_CGCR1_IPG_CLK_I2C1_LSH          23		//Inter-IC Communication 1
#define CRM_CGCR1_IPG_CLK_I2C2_LSH          24		//Inter-IC Communication 2
#define CRM_CGCR1_IPG_CLK_I2C3_LSH          25		//Inter-IC Communication 3
#define CRM_CGCR1_IPG_CLK_IIM_LSH          	26		//IC Identification Module
#define CRM_CGCR1_IPG_CLK_IOMUXC_LSH        27		//I/O Multiplexer Controller 
#define CRM_CGCR1_IPG_CLK_KPP_LSH          	28		//KeyPad Port
#define CRM_CGCR1_IPG_CLK_LCDC_LSH          29		//LCD Controller
#define CRM_CGCR1_IPG_CLK_OWIRE_LSH         30		//One Wire
#define CRM_CGCR1_IPG_CLK_PWM1_LSH          31		//Pulse-Width Modulator 1

//Clock Gating Control Register 2 - CGCR2
#define CRM_CGCR2_IPG_CLK_PWM2_LSH         	0		//Pulse-Width Modulator 2
#define CRM_CGCR2_IPG_CLK_PWM3_LSH         	1		//Pulse-Width Modulator 3
#define CRM_CGCR2_IPG_CLK_PWM4_LSH         	2		//Pulse-Width Modulator 4
#define CRM_CGCR2_IPG_CLK_RNGB_LSH         	3		//Random Number Generator
#define CRM_CGCR2_IPG_CLK_RTIC_LSH         	4		//Real Time Integrity Checker 
#define CRM_CGCR2_IPG_CLK_SCC_LSH         	5		//Security Controller
#define CRM_CGCR2_IPG_CLK_SDMA_LSH         	6		//Smart DMA
#define CRM_CGCR2_IPG_CLK_SIM1_LSH         	7		//Subscriber Identification Module 1
#define CRM_CGCR2_IPG_CLK_SIM2_LSH         	8		//Subscriber Identification Module 2
#define CRM_CGCR2_IPG_CLK_SLCDC_LSH         9		//Smart LCD Controller
#define CRM_CGCR2_IPG_CLK_SPBA_LSH         	10		//Shared Peripheral Bus Arbiter
#define CRM_CGCR2_IPG_CLK_SSI1_LSH         	11		//Synchronous Serial Interface 1
#define CRM_CGCR2_IPG_CLK_SSI2_LSH         	12		//Synchronous Serial Interface 2
#define CRM_CGCR2_IPG_CLK_TCHSCRN_LSH       13		//Touch Screen Controller (TSC)? and ADC?
#define CRM_CGCR2_IPG_CLK_UART1_LSH         14		//Universal Asynchronous Receiver/Transmitter 1
#define CRM_CGCR2_IPG_CLK_UART2_LSH         15		//Universal Asynchronous Receiver/Transmitter 2
#define CRM_CGCR2_IPG_CLK_UART3_LSH         16		//Universal Asynchronous Receiver/Transmitter 3
#define CRM_CGCR2_IPG_CLK_UART4_LSH         17		//Universal Asynchronous Receiver/Transmitter 4
#define CRM_CGCR2_IPG_CLK_UART5_LSH         18		//Universal Asynchronous Receiver/Transmitter 5
#define CRM_CGCR2_IPG_CLK_WDOG_LSH         	19		//Watch Dog Timer

//Per Clock Divider Register 0 - PCDR0
#define CRM_PCDR0_PER0DIV_LSH               0
#define CRM_PCDR0_PER1DIV_LSH               8
#define CRM_PCDR0_PER2DIV_LSH               16
#define CRM_PCDR0_PER3DIV_LSH               24

//Per Clock Divider Register 1 - PCDR1
#define CRM_PCDR1_PER4DIV_LSH               0
#define CRM_PCDR1_PER5DIV_LSH               8
#define CRM_PCDR1_PER6DIV_LSH               16
#define CRM_PCDR1_PER7DIV_LSH               24

//Per Clock Divider Register 2 - PCDR2
#define CRM_PCDR2_PER8DIV_LSH               0
#define CRM_PCDR2_PER9DIV_LSH               8
#define CRM_PCDR2_PER10DIV_LSH              16
#define CRM_PCDR2_PER11DIV_LSH              24

//Per Clock Divider Register 3 - PCDR3
#define CRM_PCDR3_PER12DIV_LSH              0
#define CRM_PCDR3_PER13DIV_LSH              8
#define CRM_PCDR3_PER14DIV_LSH              16
#define CRM_PCDR3_PER15DIV_LSH              24

//CRM Status Register - CRSR (Current Reference Manual says - RCSR)
#define CRM_CRSR_RESTS_LSH               	0
#define CRM_CRSR_MLCSEL_LSH               	4
#define CRM_CRSR_EEPROMCFG_LSH              5
#define CRM_CRSR_BOOTINT_LSH                6
#define CRM_CRSR_SPARESIZE_LSH              7
#define CRM_CRSR_NFC_FMS_LSH                8
#define CRM_CRSR_NFC_4K_LSH               	9
#define CRM_CRSR_BOOTREG_LSH                10
#define CRM_CRSR_CLKSEL_LSH               	12
#define CRM_CRSR_NFC_16BIT_SEL_LSH          14
#define CRM_CRSR_SOFT_RESET_LSH             15
#define CRM_CRSR_BTRES_LSH               	16
#define CRM_CRSR_BTSRC_LSH               	20
#define CRM_CRSR_USBSRC_LSH               	22
#define CRM_CRSR_BUS_WIDTH_LSH              24
#define CRM_CRSR_PAGE_SIZE_LSH              26
#define CRM_CRSR_MEM_TYPE_LSH               28
#define CRM_CRSR_MEM_CTRL_LSH               30

//CRM Debug Register - CRDR
#define CRM_CRDR_BT_LPB_FREQ_LSH            26
#define CRM_CRDR_BT_UART_SRC_LSH            29

//DPTC Comparator Value Registers - DCVR0-DCVR3
#define CRM_DCVR0_ELV_LSH            		2
#define CRM_DCVR0_LLV_LSH            		12
#define CRM_DCVR0_ULV_LSH            		22

#define CRM_DCVR1_ELV_LSH            		2
#define CRM_DCVR1_LLV_LSH            		12
#define CRM_DCVR1_ULV_LSH            		22

#define CRM_DCVR2_ELV_LSH            		2
#define CRM_DCVR2_LLV_LSH            		12
#define CRM_DCVR2_ULV_LSH            		22

#define CRM_DCVR3_ELV_LSH            		2
#define CRM_DCVR3_LLV_LSH            		12
#define CRM_DCVR3_ULV_LSH            		22

//Load Tracking Register 0- LTR0
#define CRM_LTR0_SIGD0_LSH            		0
#define CRM_LTR0_SIGD1_LSH            		1
#define CRM_LTR0_SIGD2_LSH            		2
#define CRM_LTR0_SIGD3_LSH            		3
#define CRM_LTR0_SIGD4_LSH            		4
#define CRM_LTR0_SIGD5_LSH            		5
#define CRM_LTR0_SIGD6_LSH            		6
#define CRM_LTR0_SIGD7_LSH            		7
#define CRM_LTR0_SIGD8_LSH            		8
#define CRM_LTR0_SIGD9_LSH            		9
#define CRM_LTR0_SIGD10_LSH            		10
#define CRM_LTR0_SIGD11_LSH            		11
#define CRM_LTR0_SIGD12_LSH            		12
#define CRM_LTR0_SIGD13_LSH            		13
#define CRM_LTR0_SIGD14_LSH            		14
#define CRM_LTR0_SIGD15_LSH            		15
#define CRM_LTR0_DNTHR_LSH            		16
#define CRM_LTR0_UPTHR_LSH            		22
#define CRM_LTR0_DIV3CK_LSH            		28

//Load Tracking Register 1 - LTR1
#define CRM_LTR1_PNCTHR_LSH            		0
#define CRM_LTR1_UPCNT_LSH            		6
#define CRM_LTR1_DNCNT_LSH            		14
#define CRM_LTR1_LTBRSR_LSH            		22
#define CRM_LTR1_LTBRSH_LSH            		23

//Load Tracking Register 2 - LTR2
#define CRM_LTR2_EMAC_LSH            		0
#define CRM_LTR2_WSW9_LSH            		11
#define CRM_LTR2_WSW10_LSH            		14
#define CRM_LTR2_WSW11_LSH            		17
#define CRM_LTR2_WSW12_LSH            		20
#define CRM_LTR2_WSW13_LSH            		23
#define CRM_LTR2_WSW14_LSH            		26
#define CRM_LTR2_WSW15_LSH            		29

//Load Tracking Register 3 - LTR3
#define CRM_LTR3_WSW0_LSH       			5
#define CRM_LTR3_WSW1_LSH       			8
#define CRM_LTR3_WSW2_LSH       			11
#define CRM_LTR3_WSW3_LSH       			14
#define CRM_LTR3_WSW4_LSH       			17
#define CRM_LTR3_WSW5_LSH       			20
#define CRM_LTR3_WSW6_LSH       			23
#define CRM_LTR3_WSW7_LSH       			26
#define CRM_LTR3_WSW8_LSH       			29

//Load Tracking Buffer Register 0 - LTBR0
#define CRM_LTBR0_LTS0_LSH       			0
#define CRM_LTBR0_LTS1_LSH       			4
#define CRM_LTBR0_LTS2_LSH       			8
#define CRM_LTBR0_LTS3_LSH       			12
#define CRM_LTBR0_LTS4_LSH       			16
#define CRM_LTBR0_LTS5_LSH       			20
#define CRM_LTBR0_LTS6_LSH       			24
#define CRM_LTBR0_LTS7_LSH       			28

//Load Tracking Buffer Register 1 - LTBR1
#define CRM_LTBR1_LTS8_LSH       			0
#define CRM_LTBR1_LTS9_LSH       			4
#define CRM_LTBR1_LTS10_LSH       			8
#define CRM_LTBR1_LTS11_LSH       			12
#define CRM_LTBR1_LTS12_LSH       			16
#define CRM_LTBR1_LTS13_LSH       			20
#define CRM_LTBR1_LTS14_LSH       			24
#define CRM_LTBR1_LTS15_LSH       			28

//Power Management Control Register 0 - PMCR0
#define CRM_PMCR0_DPTEN_LSH       			0
#define CRM_PMCR0_PTVAI_LSH       			1
#define CRM_PMCR0_PTVAIM_LSH       			3
#define CRM_PMCR0_DVFEN_LSH       			4
#define CRM_PMCR0_SCR_LSH       			5
#define CRM_PMCR0_DRCE0_LSH       			6
#define CRM_PMCR0_DRCE1_LSH       			7
#define CRM_PMCR0_DRCE2_LSH       			8
#define CRM_PMCR0_DRCE3_LSH       			9
#define CRM_PMCR0_WFIM_LSH       			10
#define CRM_PMCR0_DPVV_LSH       			11
#define CRM_PMCR0_DPVCR_LSH       			12
#define CRM_PMCR0_FSVAI_LSH       			13
#define CRM_PMCR0_FSVAIM_LSH       			15
#define CRM_PMCR0_DVFS_START_LSH       		16
#define CRM_PMCR0_PTVIS_LSH       			17
#define CRM_PMCR0_LBCF_LSH       			18
#define CRM_PMCR0_LBFL_LSH       			20
#define CRM_PMCR0_LBMI_LSH       			21
#define CRM_PMCR0_DVFIS_LSH       			22
#define CRM_PMCR0_DVFEC_LSH       			23
#define CRM_PMCR0_DVFS_UPD_FINISH_LSH       24
#define CRM_PMCR0_DVSUP_LSH       			28

//Power Management Control Register 1 - PMCR1
#define CRM_PMCR1_DVGP_LSH       			0
#define CRM_PMCR1_CPSPA_LSH       			8
#define CRM_PMCR1_CPFA_LSH       			12
#define CRM_PMCR1_CPEN_LSH       			13
#define CRM_PMCR1_WBCN_LSH       			16
#define CRM_PMCR1_CPSPA_EMI_LSH       		24
#define CRM_PMCR1_CPFA_EMI_LSH       		28
#define CRM_PMCR1_CPEN_EMI_LSH       		29

//Power Management Control Register 2- PMCR2
#define CRM_PMCR2_DVFS_ACK_LSH       		0
#define CRM_PMCR2_DVFS_REQ_LSH       		1
#define CRM_PMCR2_ARM_CLKON_CNT_LSH       	4
#define CRM_PMCR2_ARM_MEMON_CNT_LSH       	8
#define CRM_PMCR2_OSC24M_DOWN_LSH       	16
#define CRM_PMCR2_VSTBY_LSH       			17
#define CRM_PMCR2_ARMMEMDWN_LSH       		18

//Misc Control Register - MCR
#define CRM_MCR_PER0_CLK_MUX_LSH       		0
#define CRM_MCR_PER1_CLK_MUX_LSH       		1
#define CRM_MCR_PER2_CLK_MUX_LSH       		2
#define CRM_MCR_PER3_CLK_MUX_LSH       		3
#define CRM_MCR_PER4_CLK_MUX_LSH       		4
#define CRM_MCR_PER5_CLK_MUX_LSH       		5
#define CRM_MCR_PER6_CLK_MUX_LSH       		6
#define CRM_MCR_PER7_CLK_MUX_LSH       		7
#define CRM_MCR_PER8_CLK_MUX_LSH       		8
#define CRM_MCR_PER9_CLK_MUX_LSH       		9
#define CRM_MCR_PER10_CLK_MUX_LSH      		10
#define CRM_MCR_PER11_CLK_MUX_LSH      		11
#define CRM_MCR_PER12_CLK_MUX_LSH      		12
#define CRM_MCR_PER13_CLK_MUX_LSH      		13
#define CRM_MCR_PER14_CLK_MUX_LSH      		14
#define CRM_MCR_PER15_CLK_MUX_LSH      		15
#define CRM_MCR_USBCLKMUX_LSH       		16
#define CRM_MCR_SSI1CLKMUX_LSH       		17
#define CRM_MCR_SSI2CLKMUX_LSH       		18
#define CRM_MCR_ESAICLKMUX_LSH       		19
#define CRM_MCR_CLKO_SEL_LSH       			20
#define CRM_MCR_CLKO_DIV_LSH       			24
#define CRM_MCR_CLKOEN_LSH       			30
#define CRM_MCR_USBXTALMUX_LSH       		31

//Low Power Interrupt Mask Registers - LPIMR0
#define CRM_LPIMR0_LPIM_LSH       			0

//Low Power Interrupt Mask Registers - LPIMR1
#define CRM_LPIMR1_LPIM_LSH       			0

//------------------------------------------------------------------------------
// REGISTER FIELD WIDTHS
//------------------------------------------------------------------------------
// MCU PLL Control Register (MPCTL)
#define CRM_MPCTL_MFN_WID 					Width_10
#define CRM_MPCTL_MFI_WID 					Width_4
#define CRM_MPCTL_LOCK_WID					Width_1
#define CRM_MPCTL_MFD_WID					Width_10
#define CRM_MPCTL_PD_WID					Width_4
#define CRM_MPCTL_BRMO_WID					Width_1

// USB PLL Control Register (UPCTL)
#define CRM_UPCTL_MFN_WID 					Width_10
#define CRM_UPCTL_MFI_WID 					Width_4
#define CRM_UPCTL_LOCK_WID					Width_1
#define CRM_UPCTL_MFD_WID 					Width_10
#define CRM_UPCTL_PD_WID					Width_4
#define CRM_UPCTL_BRMO_WID					Width_1

// Clock Control Register - CCTL
#define CRM_CCTL_ARMSRC_WID 				Width_1
#define CRM_CCTL_CGCTL_WID					Width_1
#define CRM_CCTL_USB_DIV_WID 				Width_6
#define CRM_CCTL_MPLL_BYPASS_WID			Width_1
#define CRM_CCTL_UPLLDIS_WID 				Width_1
#define CRM_CCTL_LPCTL_WID 					Width_2
#define CRM_CCTL_UPLLRST_WID 				Width_1
#define CRM_CCTL_MPLLRST_WID 				Width_1
#define CRM_CCTL_AHBCLKDIV_WID				Width_2
#define CRM_CCTL_ARMCLKDIV_WID				Width_2

//Clock Gating Control Register 0 - CGCR0
#define CRM_CGCR0_IPG_PER_0_CSI_WID			Width_1		//CMOS Sensor Interface
#define CRM_CGCR0_IPG_PER_1_EPIT_WID		Width_1		//Enhanced Periodic Timer
#define CRM_CGCR0_IPG_PER_2_ESAI_WID		Width_1		//Enhanced Audio Interface
#define CRM_CGCR0_IPG_PER_3_ESDHC1_WID		Width_1		//Enhanced SD Host Controller 1
#define CRM_CGCR0_IPG_PER_4_ESDHC2_WID		Width_1		//Enhanced SD Host Controller 2
#define CRM_CGCR0_IPG_PER_5__GPT_WID		Width_1		//General Purpose Timer
#define CRM_CGCR0_IPG_PER_6_I2C_WID			Width_1		//Inter-IC Communication
#define CRM_CGCR0_IPG_PER_7_LCDC_WID		Width_1		//Liquid Crystal Display Controller
#define CRM_CGCR0_IPG_PER_8_NFC_WID			Width_1		//NAND Flash Controller 
#define CRM_CGCR0_IPG_PER_9_OWIRE_WID		Width_1		//One Wire
#define CRM_CGCR0_IPG_PER_10_PWM_WID		Width_1		//Pulse-Width Modulator
#define CRM_CGCR0_IPG_PER_11_SIM1_WID		Width_1		//Subscriber Identification Module 1
#define CRM_CGCR0_IPG_PER_12_SIM2_WID		Width_1		//Subscriber Identification Module 2
#define CRM_CGCR0_IPG_PER_13_SSI1_WID		Width_1		//Synchronous Serial Interface 1
#define CRM_CGCR0_IPG_PER_14_SSI2_WID		Width_1		//Synchronous Serial Interface 2
#define CRM_CGCR0_IPG_PER_15_UART_WID		Width_1		//Universal Asynchronous Receiver/Transmitters

#define CRM_CGCR0_HCLK_AHB_ATA_WID          Width_1		//ATA Module
#define CRM_CGCR0_HCLK_AHB_BROM_WID         Width_1		//Boot ROM ?
#define CRM_CGCR0_HCLK_AHB_CSI_WID          Width_1		//CMOS Sensor Interface
#define CRM_CGCR0_HCLK_AHB_EMI_WID          Width_1		//External Memory Interface
#define CRM_CGCR0_HCLK_AHB_ESAI_WID         Width_1		//Enhanced Serial Audio Interface
#define CRM_CGCR0_HCLK_AHB_ESDHC1_WID       Width_1		//Enhanced SD Host Controller 1
#define CRM_CGCR0_HCLK_AHB_ESDHC2_WID       Width_1		//Enhanced SD Host Controller 2
#define CRM_CGCR0_HCLK_AHB_FEC_WID          Width_1		//Ethernet
#define CRM_CGCR0_HCLK_AHB_LCDC_WID         Width_1		//LCD Controller 
#define CRM_CGCR0_HCLK_AHB_RITC_WID         Width_1		//Real Time Integrity Checker 
#define CRM_CGCR0_HCLK_AHB_SDMA_WID         Width_1		//Smart DMA
#define CRM_CGCR0_HCLK_AHB_SLCDC_WID        Width_1		//Smart LCD Controller
#define CRM_CGCR0_HCLK_AHB_USBOTG_WID       Width_1		//USB On The Go

//Clock Gating Control Register 1 - CGCR1
#define CRM_CGCR1_IPG_CLK_AUDMUX_WID        Width_1		//Digital Audio Mux
#define CRM_CGCR1_IPG_CLK_ATA_WID          	Width_1		//Advanced Technology Attachment
#define CRM_CGCR1_IPG_CLK_CAN1_WID          Width_1		//Controller Area Network 1
#define CRM_CGCR1_IPG_CLK_CAN2_WID          Width_1		//Controller Area Network 2
#define CRM_CGCR1_IPG_CLK_CSI_WID          	Width_1		//CMOS Sensor Interface
#define CRM_CGCR1_IPG_CLK_CSPI1_WID         Width_1		//Configurable Serial Peripheral Interface 1
#define CRM_CGCR1_IPG_CLK_CSPI2_WID         Width_1		//Configurable Serial Peripheral Interface 2
#define CRM_CGCR1_IPG_CLK_CSPI3_WID         Width_1		//Configurable Serial Peripheral Interface 3
#define CRM_CGCR1_IPG_CLK_DRYICE_WID        Width_1		//DRY ICE
#define CRM_CGCR1_IPG_CLK_ECT_WID          	Width_1		//Embedded Cross Trigger
#define CRM_CGCR1_IPG_CLK_EPIT1_WID         Width_1		//Enhanced Periodic Timer 1
#define CRM_CGCR1_IPG_CLK_EPIT2_WID         Width_1		//Enhanced Periodic Timer 2
#define CRM_CGCR1_IPG_CLK_ESAI_WID          Width_1		//Enhanced Serial Audio Interface
#define CRM_CGCR1_IPG_CLK_ESDHC1_WID        Width_1		//Enhanced SD Host Controller 1
#define CRM_CGCR1_IPG_CLK_ESDHC2_WID        Width_1		//Enhanced SD Host Controller 2
#define CRM_CGCR1_IPG_CLK_FEC_WID          	Width_1		//Ethernet
#define CRM_CGCR1_IPG_CLK_GPIO1_WID         Width_1		//General Purpose I/O Module 1
#define CRM_CGCR1_IPG_CLK_GPIO2_WID         Width_1		//General Purpose I/O Module 2
#define CRM_CGCR1_IPG_CLK_GPIO3_WID         Width_1		//General Purpose I/O Module 3
#define CRM_CGCR1_IPG_CLK_GPT1_WID          Width_1		//General Purpose Timer 1
#define CRM_CGCR1_IPG_CLK_GPT2_WID          Width_1		//General Purpose Timer 2
#define CRM_CGCR1_IPG_CLK_GPT3_WID          Width_1		//General Purpose Timer 3
#define CRM_CGCR1_IPG_CLK_GPT4_WID          Width_1		//General Purpose Timer 4
#define CRM_CGCR1_IPG_CLK_I2C1_WID          Width_1		//Inter-IC Communication 1
#define CRM_CGCR1_IPG_CLK_I2C2_WID          Width_1		//Inter-IC Communication 2
#define CRM_CGCR1_IPG_CLK_I2C3_WID          Width_1		//Inter-IC Communication 3
#define CRM_CGCR1_IPG_CLK_IIM_WID          	Width_1		//IC Identification Module
#define CRM_CGCR1_IPG_CLK_IOMUXC_WID        Width_1		//I/O Multiplexer Controller 
#define CRM_CGCR1_IPG_CLK_KPP_WID          	Width_1		//KeyPad Port
#define CRM_CGCR1_IPG_CLK_LCDC_WID          Width_1		//LCD Controller
#define CRM_CGCR1_IPG_CLK_OWIRE_WID         Width_1		//One Wire
#define CRM_CGCR1_IPG_CLK_PWM1_WID          Width_1		//Pulse-Width Modulator 1

//Clock Gating Control Register 2 - CGCR2
#define CRM_CGCR2_IPG_CLK_PWM2_WID          Width_1		//Pulse-Width Modulator 2
#define CRM_CGCR2_IPG_CLK_PWM3_WID          Width_1		//Pulse-Width Modulator 3
#define CRM_CGCR2_IPG_CLK_PWM4_WID          Width_1		//Pulse-Width Modulator 4
#define CRM_CGCR2_IPG_CLK_RNGB_WID          Width_1		//Random Number Generator
#define CRM_CGCR2_IPG_CLK_RTIC_WID          Width_1		//Real Time Integrity Checker 
#define CRM_CGCR2_IPG_CLK_SCC_WID          	Width_1		//Security Controller
#define CRM_CGCR2_IPG_CLK_SDMA_WID          Width_1		//Smart DMA
#define CRM_CGCR2_IPG_CLK_SIM1_WID          Width_1		//Subscriber Identification Module 1
#define CRM_CGCR2_IPG_CLK_SIM2_WID          Width_1		//Subscriber Identification Module 2
#define CRM_CGCR2_IPG_CLK_SLCDC_WID         Width_1		//Smart LCD Controller
#define CRM_CGCR2_IPG_CLK_SPBA_WID          Width_1		//Shared Peripheral Bus Arbiter
#define CRM_CGCR2_IPG_CLK_SSI1_WID          Width_1		//Synchronous Serial Interface 1
#define CRM_CGCR2_IPG_CLK_SSI2_WID          Width_1		//Synchronous Serial Interface 2
#define CRM_CGCR2_IPG_CLK_TCHSCRN_WID       Width_1		//Touch Screen Controller (TSC)? and ADC?
#define CRM_CGCR2_IPG_CLK_UART1_WID         Width_1		//Universal Asynchronous Receiver/Transmitter 1
#define CRM_CGCR2_IPG_CLK_UART2_WID         Width_1		//Universal Asynchronous Receiver/Transmitter 2
#define CRM_CGCR2_IPG_CLK_UART3_WID         Width_1		//Universal Asynchronous Receiver/Transmitter 3
#define CRM_CGCR2_IPG_CLK_UART4_WID         Width_1		//Universal Asynchronous Receiver/Transmitter 4
#define CRM_CGCR2_IPG_CLK_UART5_WID         Width_1		//Universal Asynchronous Receiver/Transmitter 5
#define CRM_CGCR2_IPG_CLK_WDOG_WID          Width_1		//Watch Dog Timer


//Per Clock Divider Register 0 - PCDR0
#define CRM_PCDR0_PER0DIV_WID				Width_6
#define CRM_PCDR0_PER1DIV_WID			 	Width_6
#define CRM_PCDR0_PER2DIV_WID				Width_6
#define CRM_PCDR0_PER3DIV_WID				Width_6

//Per Clock Divider Register 0 - PCDR1
#define CRM_PCDR1_PER4DIV_WID				Width_6
#define CRM_PCDR1_PER5DIV_WID				Width_6
#define CRM_PCDR1_PER6DIV_WID				Width_6
#define CRM_PCDR1_PER7DIV_WID				Width_6

//Per Clock Divider Register 2 - PCDR2
#define CRM_PCDR2_PER8DIV_WID				Width_6
#define CRM_PCDR2_PER9DIV_WID				Width_6
#define CRM_PCDR2_PER10DIV_WID				Width_6
#define CRM_PCDR2_PER11DIV_WID				Width_6

//Per Clock Divider Register 3 - PCDR3
#define CRM_PCDR3_PER12DIV_WID				Width_6
#define CRM_PCDR3_PER13DIV_WID				Width_6
#define CRM_PCDR3_PER14DIV_WID				Width_6
#define CRM_PCDR3_PER15DIV_WID				Width_6

//CRM Status Register - CRSR 
#define CRM_CRSR_RESTS_WID					Width_4
#define CRM_CRSR_MLCSEL_WID					Width_1
#define CRM_CRSR_EEPROMCFG_WID				Width_1
#define CRM_CRSR_BOOTINT_WID				Width_1
#define CRM_CRSR_SPARESIZE_WID				Width_1
#define CRM_CRSR_NFC_FMS_WID				Width_1
#define CRM_CRSR_NFC_4K_WID					Width_1
#define CRM_CRSR_BOOTREG_WID				Width_2
#define CRM_CRSR_CLKSEL_WID					Width_1
#define CRM_CRSR_NFC_16BIT_SEL_WID			Width_1
#define CRM_CRSR_SOFT_RESET_WID				Width_1
#define CRM_CRSR_BTRES_WID					Width_4
#define CRM_CRSR_BTSRC_WID					Width_2
#define CRM_CRSR_USBSRC_WID					Width_2
#define CRM_CRSR_BUS_Width_WID				Width_2
#define CRM_CRSR_PAGE_SIZE_WID				Width_2
#define CRM_CRSR_MEM_TYPE_WID				Width_2
#define CRM_CRSR_MEM_CTRL_WID				Width_2

//CRM Debug Register - CRDR
#define CRM_CRDR_BT_LPB_FREQ_WID			Width_3
#define CRM_CRDR_BT_UART_SRC_WID			Width_3

//DPTC Comparator Value Registers - DCVR0-DCVR3
#define CRM_DCVR0_ELV_WID					Width_10
#define CRM_DCVR0_LLV_WID					Width_10
#define CRM_DCVR0_ULV_WID					Width_10

#define CRM_DCVR1_ELV_WID					Width_10
#define CRM_DCVR1_LLV_WID					Width_10
#define CRM_DCVR1_ULV_WID					Width_10

#define CRM_DCVR2_ELV_WID					Width_10
#define CRM_DCVR2_LLV_WID					Width_10
#define CRM_DCVR2_ULV_WID					Width_10

#define CRM_DCVR3_ELV_WID					Width_10
#define CRM_DCVR3_LLV_WID					Width_10
#define CRM_DCVR3_ULV_WID					Width_10

//Load Tracking Register 0- LTR0
#define CRM_LTR0_SIGD0_WID 					Width_1
#define CRM_LTR0_SIGD1_WID 					Width_1
#define CRM_LTR0_SIGD2_WID 					Width_1
#define CRM_LTR0_SIGD3_WID 					Width_1
#define CRM_LTR0_SIGD4_WID					Width_1
#define CRM_LTR0_SIGD5_WID 					Width_1
#define CRM_LTR0_SIGD6_WID 					Width_1
#define CRM_LTR0_SIGD7_WID 					Width_1
#define CRM_LTR0_SIGD8_WID 					Width_1
#define CRM_LTR0_SIGD9_WID 					Width_1
#define CRM_LTR0_SIGD10_WID 				Width_1
#define CRM_LTR0_SIGD11_WID 				Width_1
#define CRM_LTR0_SIGD12_WID 				Width_1
#define CRM_LTR0_SIGD13_WID 				Width_1
#define CRM_LTR0_SIGD14_WID 				Width_1
#define CRM_LTR0_SIGD15_WID 				Width_1
#define CRM_LTR0_DNTHR_WID					Width_6 		
#define CRM_LTR0_UPTHR_WID 					Width_6
#define CRM_LTR0_DIV3CK_WID 				Width_2

//Load Tracking Register 1 - LTR1
#define CRM_LTR1_PNCTHR_WID 				Width_6
#define CRM_LTR1_UPCNT_WID 					Width_8
#define CRM_LTR1_DNCNT_WID 					Width_8
#define CRM_LTR1_LTBRSR_WID 				Width_1
#define CRM_LTR1_LTBRSH_WID 				Width_1

//Load Tracking Register 2 - LTR2
#define CRM_LTR2_EMAC_WID 					Width_9
#define CRM_LTR2_WSW9_WID 					Width_3
#define CRM_LTR2_WSW10_WID 					Width_3
#define CRM_LTR2_WSW11_WID 					Width_3
#define CRM_LTR2_WSW12_WID 					Width_3
#define CRM_LTR2_WSW13_WID 					Width_3
#define CRM_LTR2_WSW14_WID 					Width_3
#define CRM_LTR2_WSW15_WID 					Width_3

//Load Tracking Register 3 - LTR3
#define CRM_LTR3_WSW0_WID 					Width_3
#define CRM_LTR3_WSW1_WID 					Width_3
#define CRM_LTR3_WSW2_WID 					Width_3
#define CRM_LTR3_WSW3_WID 					Width_3
#define CRM_LTR3_WSW4_WID 					Width_3
#define CRM_LTR3_WSW5_WID 					Width_3
#define CRM_LTR3_WSW6_WID 					Width_3
#define CRM_LTR3_WSW7_WID 					Width_3
#define CRM_LTR3_WSW8_WID 					Width_3

//Load Tracking Buffer Register 0 - LTBR0
#define CRM_LTBR0_LTS0_WID 					Width_4
#define CRM_LTBR0_LTS1_WID 					Width_4
#define CRM_LTBR0_LTS2_WID 					Width_4
#define CRM_LTBR0_LTS3_WID 					Width_4
#define CRM_LTBR0_LTS4_WID 					Width_4
#define CRM_LTBR0_LTS5_WID 					Width_4
#define CRM_LTBR0_LTS6_WID 					Width_4
#define CRM_LTBR0_LTS7_WID 					Width_4

//Load Tracking Buffer Register 1 - LTBR1
#define CRM_LTBR1_LTS8_WID       			Width_4
#define CRM_LTBR1_LTS9_WID       			Width_4
#define CRM_LTBR1_LTS10_WID       			Width_4
#define CRM_LTBR1_LTS11_WID       			Width_4
#define CRM_LTBR1_LTS12_WID       			Width_4
#define CRM_LTBR1_LTS13_WID       			Width_4
#define CRM_LTBR1_LTS14_WID       			Width_4
#define CRM_LTBR1_LTS15_WID       			Width_4

//Power Management Control Register 0 - PMCR0
#define CRM_PMCR0_DPTEN_WID       			Width_1
#define CRM_PMCR0_PTVAI_WID       			Width_2
#define CRM_PMCR0_PTVAIM_WID       			Width_1
#define CRM_PMCR0_DVFEN_WID       			Width_1
#define CRM_PMCR0_SCR_WID       			Width_1
#define CRM_PMCR0_DRCE0_WID       			Width_1
#define CRM_PMCR0_DRCE1_WID       			Width_1
#define CRM_PMCR0_DRCE2_WID       			Width_1
#define CRM_PMCR0_DRCE3_WID       			Width_1
#define CRM_PMCR0_WFIM_WID       			Width_1
#define CRM_PMCR0_DPVV_WID       			Width_1
#define CRM_PMCR0_DPVCR_WID       			Width_1
#define CRM_PMCR0_FSVAI_WID       			Width_2
#define CRM_PMCR0_FSVAIM_WID       			Width_1
#define CRM_PMCR0_DVFS_START_WID       		Width_1
#define CRM_PMCR0_PTVIS_WID       			Width_1
#define CRM_PMCR0_LBCF_WID       			Width_2
#define CRM_PMCR0_LBFL_WID       			Width_1
#define CRM_PMCR0_LBMI_WID       			Width_1
#define CRM_PMCR0_DVFIS_WID       			Width_1
#define CRM_PMCR0_DVFEC_WID       			Width_1
#define CRM_PMCR0_DVFS_UPD_FINISH_WID       Width_1
#define CRM_PMCR0_DVSUP_WID       			Width_2

//Power Management Control Register 1 - PMCR1
#define CRM_PMCR1_DVGP_WID       			Width_4
#define CRM_PMCR1_CPSPA_WID       			Width_4
#define CRM_PMCR1_CPFA_WID       			Width_1
#define CRM_PMCR1_CPEN_WID       			Width_1
#define CRM_PMCR1_WBCN_WID       			Width_8
#define CRM_PMCR1_CPSPA_EMI_WID       		Width_4
#define CRM_PMCR1_CPFA_EMI_WID       		Width_1
#define CRM_PMCR1_CPEN_EMI_WID       		Width_1

//Power Management Control Register 2- PMCR2
#define CRM_PMCR2_DVFS_ACK_WID       		Width_1
#define CRM_PMCR2_DVFS_REQ_WID       		Width_1
#define CRM_PMCR2_ARM_CLKON_CNT_WID       	Width_4
#define CRM_PMCR2_ARM_MEMON_CNT_WID       	Width_8
#define CRM_PMCR2_OSC24M_DOWN_WID       	Width_1
#define CRM_PMCR2_VSTBY_WID       			Width_1
#define CRM_PMCR2_ARMMEMDWN_WID       		Width_1

//Misc Control Register - MCR
//#define CRM_MCR_PER_CLK_MUX_WID       	Width_16
#define CRM_MCR_PER0_CLK_MUX_WID       		Width_1
#define CRM_MCR_PER1_CLK_MUX_WID      		Width_1
#define CRM_MCR_PER2_CLK_MUX_WID      		Width_1
#define CRM_MCR_PER3_CLK_MUX_WID      		Width_1
#define CRM_MCR_PER4_CLK_MUX_WID      		Width_1
#define CRM_MCR_PER5_CLK_MUX_WID      		Width_1
#define CRM_MCR_PER6_CLK_MUX_WID      		Width_1
#define CRM_MCR_PER7_CLK_MUX_WID      		Width_1
#define CRM_MCR_PER8_CLK_MUX_WID      		Width_1
#define CRM_MCR_PER9_CLK_MUX_WID      		Width_1
#define CRM_MCR_PER10_CLK_MUX_WID     		Width_1
#define CRM_MCR_PER11_CLK_MUX_WID     		Width_1
#define CRM_MCR_PER12_CLK_MUX_WID     		Width_1
#define CRM_MCR_PER13_CLK_MUX_WID     		Width_1
#define CRM_MCR_PER14_CLK_MUX_WID     		Width_1
#define CRM_MCR_PER15_CLK_MUX_WID     		Width_1
#define CRM_MCR_USBCLKMUX_WID       		Width_1
#define CRM_MCR_SSI1CLKMUX_WID       		Width_1
#define CRM_MCR_SSI2CLKMUX_WID       		Width_1
#define CRM_MCR_ESAICLKMUX_WID       		Width_1
#define CRM_MCR_CLKO_SEL_WID       			Width_4
#define CRM_MCR_CLKO_DIV_WID       			Width_6
#define CRM_MCR_CLKOEN_WID       			Width_1
#define CRM_MCR_USBXTALMUX_WID       		Width_1

//Low Power Interrupt Mask Registers - LPIMR0
#define CRM_LPIMR0_LPIM_WID       			Width_32

//Low Power Interrupt Mask Registers - LPIMR1
#define CRM_LPIMR1_LPIM_WID       			Width_32




//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------
//Settings used in APP: (Settings for boot are in the .asm file)
#define		CRM_MPCTL_SETTING	0x800BAC01
#define		CRM_UPCTL_SETTING	0x8400A800
//ewr06509
//#ifndef XENON_LITE_1500
#define		CRM_CCTL_SETTING	0x20004000
//#else
//#define		CRM_CCTL_SETTING	0x50000000
//#endif
#define		CRM_CGR0_SETTING	0x100C8561
#define		CRM_CGR1_SETTING	0x8A0F0050
#define		CRM_CGR2_SETTING	0x000AC006	
#define		CRM_PDR0_SETTING	0x00000004
#define		CRM_PDR1_SETTING	0x00000300
#define		CRM_PDR2_SETTING	0x00000000
#define		CRM_PDR3_SETTING	0x02000000		
#define		CRM_MCR_SETTING		0x00000401

	
#define		LP_CTL_RUN_Mode			0x0
#define		LP_CTL_Wait_Mode		0x1
#define		LP_CTL_Doze_Mode		0x2
#define		LP_CTL_Stop_Mode		0x3

#endif /*IMX25_CRM_H_*/






