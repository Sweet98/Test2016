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
// $RCSfile: db_shared/Hardware_Header_Files/iMX25/iMX25_INTVEC.h $
// $Revision: 1.5 $
// $Date: 2009/04/24 14:59:22EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx25_intvec.h
//
// Provides definitions of the Interrupt Vectors
// Refer to Chapter 3 of the iMX25 Reference Manual
//
//------------------------------------------------------------------------------

#ifndef IMX25_INTVEC_H_
#define IMX25_INTVEC_H_

//------------------------------------------------------------------------------
// REGISTER BIT FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
#define INT_CSPI3_LSH			0
#define INT_GPT4_LSH			1
#define INT_OWIRE_LSH			2
#define INT_I2C1_LSH			3
#define INT_I2C2_LSH			4
#define INT_UART4_LSH			5
#define INT_RTIC_LSH			6
#define INT_ESAI_LSH			7
#define INT_ESDHC2_LSH			8
#define INT_ESDHC1_LSH			9
#define INT_I2C3_LSH			10
#define INT_SSI2_LSH			11
#define INT_SSI1_LSH			12
#define INT_CSPI2_LSH			13
#define INT_CSPI1_LSH			14
#define INT_ATA_LSH				15
#define INT_GPIO3_LSH			16
#define INT_CSI_LSH				17
#define INT_UART3_LSH			18
#define INT_IIM_LSH				19
#define INT_SIM1_LSH			20
#define INT_SIM2_LSH			21
#define INT_RNGD_LSH			22
#define INT_GPIO4_INT			23
#define INT_KPP_LSH				24
#define INT_Dry_Ice_RTC_LSH		25	//Consolidated RTC Interrupt
#define INT_PWM_LSH				26
#define INT_EPIT2_LSH			27
#define INT_EPIT1_LSH			28
#define INT_GPT3_LSH			29
#define INT_POWER_FAIL_LSH		30	//power fail interrupt from exteranl PAD
#define INT_CRM_INT				31
#define INT_UART2_LSH			32
#define INT_NANDFC_LSH			33	//Consolidated Nand Flash Controller Interrupt
#define INT_SDMA_LSH			34	//“AND” of all 32 interrupts from all the channels
#define INT_USB_HTG_LSH			35
#define INT_PWM2_LSH			36
#define INT_USB_OTG_LSH			37
#define INT_SLCDC_LSH			38
#define INT_LCDC_LSH			39
#define INT_UART5_LSH			40
#define INT_PWM3_LSH			41
#define INT_PWM4_LSH			42
#define INT_CAN1_LSH			43
#define INT_CAN2_LSH			44
#define INT_UART1_LSH			45
#define INT_TSC_LSH				46
#define INT_Reserved_LSH		47
#define INT_ECT_LSH				48
#define INT_SCC_SCM_LSH			49	//SCM SCM interrupt
#define INT_SCC_SMN_LSH			50	//SMN SMN interrupt
#define INT_GPIO2_LSH			51	//Combined Interrupts - 1 Bit Int Or Of 32
#define INT_GPIO1_LSH			52	//Combined Interrupts - 1 Bit Int Or Of 32
#define INT_GPT2_LSH			53
#define INT_GPT1_LSH			54
#define INT_WDOG_LSH			55
#define INT_Dry_Ice_LSH			56
#define INT_FEC_LSH				57
#define INT_EXT_INT5_LSH		58	//external interrupt for Pwr Management via GPIO-1[5]
#define INT_EXT_INT4_LSH		59	//external interrupt for Temp via GPIO-1[6]
#define INT_EXT_INT3_LSH		60	//external interrupt for Sensor via GPIO-1[0]
#define INT_EXT_INT2_LSH		61	//external interrupt for Sensor via GPIO-1[1]
#define INT_EXT_INT1_LSH		62	//external interrupt for Watch-dog via GPIO-1[2]
#define INT_EXT_INT0_LSH		63	//external interrupt for TV via GPIO-1[3]

//------------------------------------------------------------------------------
// REGISTER FIELD WIDTHS
//------------------------------------------------------------------------------
#define INT_WID				1		//Width for all Vectors


typedef enum
{
    VEC_EXT_INT0_INT	=	63,
    VEC_EXT_INT1_INT	=	62,
    VEC_EXT_INT2_INT	=	61,
    VEC_EXT_INT3_INT	=	60,
    VEC_EXT_INT4_INT	=	59,
    VEC_EXT_INT5_INT	=	58,
    VEC_FEC_INT			=	57,
    VEC_Dry_Ice_INT		=	56,
    VEC_WDOG_INT		=	55,
    VEC_GPT1_INT		=	54,
    VEC_GPT2_INT		=	53,
    VEC_GPIO1_INT		=	52,
    VEC_GPIO2_INT		=	51,
    VEC_SCC_SMN_INT		=	50,
    VEC_SCC_SCM_INT		=	49,
    VEC_ECT_INT			=	48,
    VEC_Reserved_INT	=	47,
    VEC_TSC_INT			=	46,
    VEC_UART1_INT		=	45,
    VEC_CAN2_INT		=	44,
    VEC_CAN1_INT		=	43,
    VEC_PWM4_INT		=	42,
    VEC_PWM3_INT		=	41,
    VEC_UART5_INT		=	40,
    VEC_LCDC_INT		=	39,
    VEC_SLCDC_INT		=	38,
    VEC_USB_OTG_INT		=	37,
    VEC_PWM2_INT		=	36,
    VEC_USB_HTG_INT		=	35,
    VEC_SDMA_INT		=	34,
    VEC_NANDFC_INT		=	33,
    VEC_UART2_INT		=	32,
    VEC_CRM_INT			=	31,
    VEC_POWER_FAIL_INT	=	30,
    VEC_GPT3_INT		=	29,
    VEC_EPIT1_INT		=	28,
    VEC_EPIT2_INT		=	27,
    VEC_PWM_INT			=	26,
    VEC_Dry_Ice_RTC_INT	=	25,
    VEC_KPP_INT			=	24,
    VEC_GPIO4_INT		=	23,
    VEC_RNGD_INT		=	22,
    VEC_SIM2_INT		=	21,
    VEC_SIM1_INT		=	20,
    VEC_IIM_INT			=	19,
    VEC_UART3_INT		=	18,
    VEC_CSI_INT			=	17,
    VEC_GPIO3_INT		=	16,
    VEC_ATA_INT			=	15,
    VEC_CSPI1_INT		=	14,
    VEC_CSPI2_INT		=	13,
    VEC_SSI1_INT		=	12,
    VEC_SSI2_INT		=	11,
    VEC_I2C3_INT		=	10,
    VEC_ESDHC1_INT		=	9,
    VEC_ESDHC2_INT		=	8,
    VEC_ESAI_INT		=	7,
    VEC_RTIC_INT		=	6,
    VEC_UART4_INT		=	5,
    VEC_I2C2_INT		=	4,
    VEC_I2C1_INT		=	3,
    VEC_OWIRE_INT		=	2,
    VEC_GPT4_INT		=	1,
    VEC_CSPI3_INT		=	0

}vector_number_t;


#endif /*IMX25_INTVEC_H_*/


