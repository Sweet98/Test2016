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
// $RCSfile: db_shared/Hardware_Header_Files/iMX27/iMX27_GPIO.h $
// $Revision: 1.1 $
// $Date: 2008/09/22 16:35:07EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx27_GPIO.h
//
// Provides definitions for the General-Purpose I/O (GPIO)
// Refer to Chapter 6 Page 241 of the iMX27 Reference Manual
//
//-----------------------------------------------------------------------------

#ifndef IMX27_GPIO_H_
#define IMX27_GPIO_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width 
//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------

// #########################################
// # GPIO                                  #
// # $1001_5000 to $1001_5FFF              #
// #########################################

//PORT A
#define GPIOA_BASE_ADDR          0x10015000
#define GPIOA_DDIR               (GPIOA_BASE_ADDR+0x00)     // 32bit gpio pta data direction reg
#define GPIOA_OCR1               (GPIOA_BASE_ADDR+0x04)     // 32bit gpio pta output config 1 reg
#define GPIOA_OCR2               (GPIOA_BASE_ADDR+0x08)     // 32bit gpio pta output config 2 reg
#define GPIOA_ICONFA1            (GPIOA_BASE_ADDR+0x0C)     // 32bit gpio pta input config A1 reg
#define GPIOA_ICONFA2            (GPIOA_BASE_ADDR+0x10)     // 32bit gpio pta input config A2 reg
#define GPIOA_ICONFB1            (GPIOA_BASE_ADDR+0x14)     // 32bit gpio pta input config B1 reg
#define GPIOA_ICONFB2            (GPIOA_BASE_ADDR+0x18)     // 32bit gpio pta input config B2 reg
#define GPIOA_DR                 (GPIOA_BASE_ADDR+0x1C)     // 32bit gpio pta data reg
#define GPIOA_GIUS               (GPIOA_BASE_ADDR+0x20)     // 32bit gpio pta in use reg
#define GPIOA_SSR                (GPIOA_BASE_ADDR+0x24)     // 32bit gpio pta sample status reg
#define GPIOA_ICR1               (GPIOA_BASE_ADDR+0x28)     // 32bit gpio pta interrupt ctrl 1 reg
#define GPIOA_ICR2               (GPIOA_BASE_ADDR+0x2C)     // 32bit gpio pta interrupt ctrl 2 reg
#define GPIOA_IMR                (GPIOA_BASE_ADDR+0x30)     // 32bit gpio pta interrupt mask reg
#define GPIOA_ISR                (GPIOA_BASE_ADDR+0x34)     // 32bit gpio pta interrupt status reg
#define GPIOA_GPR                (GPIOA_BASE_ADDR+0x38)     // 32bit gpio pta general purpose reg
#define GPIOA_SWR                (GPIOA_BASE_ADDR+0x3C)     // 32bit gpio pta software reset reg
#define GPIOA_PUEN               (GPIOA_BASE_ADDR+0x40)     // 32bit gpio pta pull up enable reg

//PORT B
#define GPIOB_BASE_ADDR          0x10015100
#define GPIOB_DDIR               (GPIOB_BASE_ADDR+0x00)     // 32bit gpio ptb data direction reg
#define GPIOB_OCR1               (GPIOB_BASE_ADDR+0x04)     // 32bit gpio ptb output config 1 reg
#define GPIOB_OCR2               (GPIOB_BASE_ADDR+0x08)     // 32bit gpio ptb output config 2 reg
#define GPIOB_ICONFA1            (GPIOB_BASE_ADDR+0x0C)     // 32bit gpio ptb input config A1 reg
#define GPIOB_ICONFA2            (GPIOB_BASE_ADDR+0x10)     // 32bit gpio ptb input config A2 reg
#define GPIOB_ICONFB1            (GPIOB_BASE_ADDR+0x14)     // 32bit gpio ptb input config B1 reg
#define GPIOB_ICONFB2            (GPIOB_BASE_ADDR+0x18)     // 32bit gpio ptb input config B2 reg
#define GPIOB_DR                 (GPIOB_BASE_ADDR+0x1C)     // 32bit gpio ptb data reg
#define GPIOB_GIUS               (GPIOB_BASE_ADDR+0x20)     // 32bit gpio ptb in use reg
#define GPIOB_SSR                (GPIOB_BASE_ADDR+0x24)     // 32bit gpio ptb sample status reg
#define GPIOB_ICR1               (GPIOB_BASE_ADDR+0x28)     // 32bit gpio ptb interrupt ctrl 1 reg
#define GPIOB_ICR2               (GPIOB_BASE_ADDR+0x2C)     // 32bit gpio ptb interrupt ctrl 2 reg
#define GPIOB_IMR                (GPIOB_BASE_ADDR+0x30)     // 32bit gpio ptb interrupt mask reg
#define GPIOB_ISR                (GPIOB_BASE_ADDR+0x34)     // 32bit gpio ptb interrupt status reg
#define GPIOB_GPR                (GPIOB_BASE_ADDR+0x38)     // 32bit gpio ptb general purpose reg
#define GPIOB_SWR                (GPIOB_BASE_ADDR+0x3C)     // 32bit gpio ptb software reset reg
#define GPIOB_PUEN               (GPIOB_BASE_ADDR+0x40)     // 32bit gpio ptb pull up enable reg

//PORT C
#define GPIOC_BASE_ADDR          0x10015200
#define GPIOC_DDIR               (GPIOC_BASE_ADDR+0x00)     // 32bit gpio ptc data direction reg
#define GPIOC_OCR1               (GPIOC_BASE_ADDR+0x04)     // 32bit gpio ptc output config 1 reg
#define GPIOC_OCR2               (GPIOC_BASE_ADDR+0x08)     // 32bit gpio ptc output config 2 reg
#define GPIOC_ICONFA1            (GPIOC_BASE_ADDR+0x0C)     // 32bit gpio ptc input config A1 reg
#define GPIOC_ICONFA2            (GPIOC_BASE_ADDR+0x10)     // 32bit gpio ptc input config A2 reg
#define GPIOC_ICONFB1            (GPIOC_BASE_ADDR+0x14)     // 32bit gpio ptc input config B1 reg
#define GPIOC_ICONFB2            (GPIOC_BASE_ADDR+0x18)     // 32bit gpio ptc input config B2 reg
#define GPIOC_DR                 (GPIOC_BASE_ADDR+0x1C)     // 32bit gpio ptc data reg
#define GPIOC_GIUS               (GPIOC_BASE_ADDR+0x20)     // 32bit gpio ptc in use reg
#define GPIOC_SSR                (GPIOC_BASE_ADDR+0x24)     // 32bit gpio ptc sample status reg
#define GPIOC_ICR1               (GPIOC_BASE_ADDR+0x28)     // 32bit gpio ptc interrupt ctrl 1 reg
#define GPIOC_ICR2               (GPIOC_BASE_ADDR+0x2C)     // 32bit gpio ptc interrupt ctrl 2 reg
#define GPIOC_IMR                (GPIOC_BASE_ADDR+0x30)     // 32bit gpio ptc interrupt mask reg
#define GPIOC_ISR                (GPIOC_BASE_ADDR+0x34)     // 32bit gpio ptc interrupt status reg
#define GPIOC_GPR                (GPIOC_BASE_ADDR+0x38)     // 32bit gpio ptc general purpose reg
#define GPIOC_SWR                (GPIOC_BASE_ADDR+0x3C)     // 32bit gpio ptc software reset reg
#define GPIOC_PUEN               (GPIOC_BASE_ADDR+0x40)     // 32bit gpio ptc pull up enable reg

//PORT D
#define GPIOD_BASE_ADDR          0x10015300
#define GPIOD_DDIR               (GPIOD_BASE_ADDR+0x00)     // 32bit gpio ptd data direction reg
#define GPIOD_OCR1               (GPIOD_BASE_ADDR+0x04)     // 32bit gpio ptd output config 1 reg
#define GPIOD_OCR2               (GPIOD_BASE_ADDR+0x08)     // 32bit gpio ptd output config 2 reg
#define GPIOD_ICONFA1            (GPIOD_BASE_ADDR+0x0C)     // 32bit gpio ptd input config A1 reg
#define GPIOD_ICONFA2            (GPIOD_BASE_ADDR+0x10)     // 32bit gpio ptd input config A2 reg
#define GPIOD_ICONFB1            (GPIOD_BASE_ADDR+0x14)     // 32bit gpio ptd input config B1 reg
#define GPIOD_ICONFB2            (GPIOD_BASE_ADDR+0x18)     // 32bit gpio ptd input config B2 reg
#define GPIOD_DR                 (GPIOD_BASE_ADDR+0x1C)     // 32bit gpio ptd data reg
#define GPIOD_GIUS               (GPIOD_BASE_ADDR+0x20)     // 32bit gpio ptd in use reg
#define GPIOD_SSR                (GPIOD_BASE_ADDR+0x24)     // 32bit gpio ptd sample status reg
#define GPIOD_ICR1               (GPIOD_BASE_ADDR+0x28)     // 32bit gpio ptd interrupt ctrl 1 reg
#define GPIOD_ICR2               (GPIOD_BASE_ADDR+0x2C)     // 32bit gpio ptd interrupt ctrl 2 reg
#define GPIOD_IMR                (GPIOD_BASE_ADDR+0x30)     // 32bit gpio ptd interrupt mask reg
#define GPIOD_ISR                (GPIOD_BASE_ADDR+0x34)     // 32bit gpio ptd interrupt status reg
#define GPIOD_GPR                (GPIOD_BASE_ADDR+0x38)     // 32bit gpio ptd general purpose reg
#define GPIOD_SWR                (GPIOD_BASE_ADDR+0x3C)     // 32bit gpio ptd software reset reg
#define GPIOD_PUEN               (GPIOD_BASE_ADDR+0x40)     // 32bit gpio ptd pull up enable reg

//PORT E
#define GPIOE_BASE_ADDR          0x10015400
#define GPIOE_DDIR               (GPIOE_BASE_ADDR+0x00)     // 32bit gpio pte data direction reg
#define GPIOE_OCR1               (GPIOE_BASE_ADDR+0x04)     // 32bit gpio pte output config 1 reg
#define GPIOE_OCR2               (GPIOE_BASE_ADDR+0x08)     // 32bit gpio pte output config 2 reg
#define GPIOE_ICONFA1            (GPIOE_BASE_ADDR+0x0C)     // 32bit gpio pte input config A1 reg
#define GPIOE_ICONFA2            (GPIOE_BASE_ADDR+0x10)     // 32bit gpio pte input config A2 reg
#define GPIOE_ICONFB1            (GPIOE_BASE_ADDR+0x14)     // 32bit gpio pte input config B1 reg
#define GPIOE_ICONFB2            (GPIOE_BASE_ADDR+0x18)     // 32bit gpio pte input config B2 reg
#define GPIOE_DR                 (GPIOE_BASE_ADDR+0x1C)     // 32bit gpio pte data reg
#define GPIOE_GIUS               (GPIOE_BASE_ADDR+0x20)     // 32bit gpio pte in use reg
#define GPIOE_SSR                (GPIOE_BASE_ADDR+0x24)     // 32bit gpio pte sample status reg
#define GPIOE_ICR1               (GPIOE_BASE_ADDR+0x28)     // 32bit gpio pte interrupt ctrl 1 reg
#define GPIOE_ICR2               (GPIOE_BASE_ADDR+0x2C)     // 32bit gpio pte interrupt ctrl 2 reg
#define GPIOE_IMR                (GPIOE_BASE_ADDR+0x30)     // 32bit gpio pte interrupt mask reg
#define GPIOE_ISR                (GPIOE_BASE_ADDR+0x34)     // 32bit gpio pte interrupt status reg
#define GPIOE_GPR                (GPIOE_BASE_ADDR+0x38)     // 32bit gpio pte general purpose reg
#define GPIOE_SWR                (GPIOE_BASE_ADDR+0x3C)     // 32bit gpio pte software reset reg
#define GPIOE_PUEN               (GPIOE_BASE_ADDR+0x40)     // 32bit gpio pte pull up enable reg

//PORT F
#define GPIOF_BASE_ADDR          0x10015500
#define GPIOF_DDIR               (GPIOF_BASE_ADDR+0x00)     // 32bit gpio ptf data direction reg
#define GPIOF_OCR1               (GPIOF_BASE_ADDR+0x04)     // 32bit gpio ptf output config 1 reg
#define GPIOF_OCR2               (GPIOF_BASE_ADDR+0x08)     // 32bit gpio ptf output config 2 reg
#define GPIOF_ICONFA1            (GPIOF_BASE_ADDR+0x0C)     // 32bit gpio ptf input config A1 reg
#define GPIOF_ICONFA2            (GPIOF_BASE_ADDR+0x10)     // 32bit gpio ptf input config A2 reg
#define GPIOF_ICONFB1            (GPIOF_BASE_ADDR+0x14)     // 32bit gpio ptf input config B1 reg
#define GPIOF_ICONFB2            (GPIOF_BASE_ADDR+0x18)     // 32bit gpio ptf input config B2 reg
#define GPIOF_DR                 (GPIOF_BASE_ADDR+0x1C)     // 32bit gpio ptf data reg
#define GPIOF_GIUS               (GPIOF_BASE_ADDR+0x20)     // 32bit gpio ptf in use reg
#define GPIOF_SSR                (GPIOF_BASE_ADDR+0x24)     // 32bit gpio ptf sample status reg
#define GPIOF_ICR1               (GPIOF_BASE_ADDR+0x28)     // 32bit gpio ptf interrupt ctrl 1 reg
#define GPIOF_ICR2               (GPIOF_BASE_ADDR+0x2C)     // 32bit gpio ptf interrupt ctrl 2 reg
#define GPIOF_IMR                (GPIOF_BASE_ADDR+0x30)     // 32bit gpio ptf interrupt mask reg
#define GPIOF_ISR                (GPIOF_BASE_ADDR+0x34)     // 32bit gpio ptf interrupt status reg
#define GPIOF_GPR                (GPIOF_BASE_ADDR+0x38)     // 32bit gpio ptf general purpose reg
#define GPIOF_SWR                (GPIOF_BASE_ADDR+0x3C)     // 32bit gpio ptf software reset reg
#define GPIOF_PUEN               (GPIOF_BASE_ADDR+0x40)     // 32bit gpio ptf pull up enable reg

#define GPIO_REG_BASE            0x10015600
#define GPIO_PMASK               (GPIO_REG_BASE+0x00)       // 32bit gpio interrupt mask reg

//------------------------------------------------------------------------------
// REGISTER BIT FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
//DDIR
#define GPIO_DDIR_BIT0_LSH			0
#define GPIO_DDIR_BIT1_LSH			1
#define GPIO_DDIR_BIT2_LSH			2
#define GPIO_DDIR_BIT3_LSH			3
#define GPIO_DDIR_BIT4_LSH			4
#define GPIO_DDIR_BIT5_LSH			5
#define GPIO_DDIR_BIT6_LSH			6
#define GPIO_DDIR_BIT7_LSH			7
#define GPIO_DDIR_BIT8_LSH			8
#define GPIO_DDIR_BIT9_LSH			9
#define GPIO_DDIR_BIT10_LSH			10
#define GPIO_DDIR_BIT11_LSH			11
#define GPIO_DDIR_BIT12_LSH			12
#define GPIO_DDIR_BIT13_LSH			13
#define GPIO_DDIR_BIT14_LSH			14
#define GPIO_DDIR_BIT15_LSH			15
#define GPIO_DDIR_BIT16_LSH			16
#define GPIO_DDIR_BIT17_LSH			17
#define GPIO_DDIR_BIT18_LSH			18
#define GPIO_DDIR_BIT19_LSH			19
#define GPIO_DDIR_BIT20_LSH			20
#define GPIO_DDIR_BIT21_LSH			21
#define GPIO_DDIR_BIT22_LSH			22
#define GPIO_DDIR_BIT23_LSH			23
#define GPIO_DDIR_BIT24_LSH			24
#define GPIO_DDIR_BIT25_LSH			25
#define GPIO_DDIR_BIT26_LSH			26
#define GPIO_DDIR_BIT27_LSH			27
#define GPIO_DDIR_BIT28_LSH			28
#define GPIO_DDIR_BIT29_LSH			29
#define GPIO_DDIR_BIT30_LSH			30
#define GPIO_DDIR_BIT31_LSH			31

//OCR1
#define GPIO_OCR1_PIN0_LSH			0
#define GPIO_OCR1_PIN1_LSH			2
#define GPIO_OCR1_PIN2_LSH			4
#define GPIO_OCR1_PIN3_LSH			6
#define GPIO_OCR1_PIN4_LSH			8
#define GPIO_OCR1_PIN5_LSH			10
#define GPIO_OCR1_PIN6_LSH			12
#define GPIO_OCR1_PIN7_LSH			14
#define GPIO_OCR1_PIN8_LSH			16
#define GPIO_OCR1_PIN9_LSH			18
#define GPIO_OCR1_PIN10_LSH			20
#define GPIO_OCR1_PIN11_LSH			22
#define GPIO_OCR1_PIN12_LSH			24
#define GPIO_OCR1_PIN13_LSH			26
#define GPIO_OCR1_PIN14_LSH			28
#define GPIO_OCR1_PIN15_LSH			30

//OCR2	
#define GPIO_OCR2_PIN16_LSH			0
#define GPIO_OCR2_PIN17_LSH			2
#define GPIO_OCR2_PIN18_LSH			4
#define GPIO_OCR2_PIN19_LSH			6
#define GPIO_OCR2_PIN20_LSH			8
#define GPIO_OCR2_PIN21_LSH			10
#define GPIO_OCR2_PIN22_LSH			12
#define GPIO_OCR2_PIN23_LSH			14
#define GPIO_OCR2_PIN24_LSH			16
#define GPIO_OCR2_PIN25_LSH			18
#define GPIO_OCR2_PIN26_LSH			20
#define GPIO_OCR2_PIN27_LSH			22
#define GPIO_OCR2_PIN28_LSH			24
#define GPIO_OCR2_PIN29_LSH			26
#define GPIO_OCR2_PIN30_LSH			28
#define GPIO_OCR2_PIN31_LSH			30

//ICONFA1
#define GPIO_ICONFA1_PIN0_LSH		0
#define GPIO_ICONFA1_PIN1_LSH		2
#define GPIO_ICONFA1_PIN2_LSH		4
#define GPIO_ICONFA1_PIN3_LSH		6
#define GPIO_ICONFA1_PIN4_LSH		8
#define GPIO_ICONFA1_PIN5_LSH		10
#define GPIO_ICONFA1_PIN6_LSH		12
#define GPIO_ICONFA1_PIN7_LSH		14
#define GPIO_ICONFA1_PIN8_LSH		16
#define GPIO_ICONFA1_PIN9_LSH		18
#define GPIO_ICONFA1_PIN10_LSH		20
#define GPIO_ICONFA1_PIN11_LSH		22
#define GPIO_ICONFA1_PIN12_LSH		24
#define GPIO_ICONFA1_PIN13_LSH		26
#define GPIO_ICONFA1_PIN14_LSH		28
#define GPIO_ICONFA1_PIN15_LSH		30

//ICONFA2
#define GPIO_ICONFA2_PIN16_LSH		0
#define GPIO_ICONFA2_PIN17_LSH		2
#define GPIO_ICONFA2_PIN18_LSH		4
#define GPIO_ICONFA2_PIN19_LSH		6
#define GPIO_ICONFA2_PIN20_LSH		8
#define GPIO_ICONFA2_PIN21_LSH		10
#define GPIO_ICONFA2_PIN22_LSH		12
#define GPIO_ICONFA2_PIN23_LSH		14
#define GPIO_ICONFA2_PIN24_LSH		16
#define GPIO_ICONFA2_PIN25_LSH		18
#define GPIO_ICONFA2_PIN26_LSH		20
#define GPIO_ICONFA2_PIN27_LSH		22
#define GPIO_ICONFA2_PIN28_LSH		24
#define GPIO_ICONFA2_PIN29_LSH		26
#define GPIO_ICONFA2_PIN30_LSH		28
#define GPIO_ICONFA2_PIN31_LSH		30

//ICONFB1
#define GPIO_ICONFB1_PIN0_LSH		0
#define GPIO_ICONFB1_PIN1_LSH		2
#define GPIO_ICONFB1_PIN2_LSH		4
#define GPIO_ICONFB1_PIN3_LSH		6
#define GPIO_ICONFB1_PIN4_LSH		8
#define GPIO_ICONFB1_PIN5_LSH		10
#define GPIO_ICONFB1_PIN6_LSH		12
#define GPIO_ICONFB1_PIN7_LSH		14
#define GPIO_ICONFB1_PIN8_LSH		16
#define GPIO_ICONFB1_PIN9_LSH		18
#define GPIO_ICONFB1_PIN10_LSH		20
#define GPIO_ICONFB1_PIN11_LSH		22
#define GPIO_ICONFB1_PIN12_LSH		24
#define GPIO_ICONFB1_PIN13_LSH		26
#define GPIO_ICONFB1_PIN14_LSH		28
#define GPIO_ICONFB1_PIN15_LSH		30

//ICONFB2
#define GPIO_ICONFB2_PIN16_LSH		0
#define GPIO_ICONFB2_PIN17_LSH		2
#define GPIO_ICONFB2_PIN18_LSH		4
#define GPIO_ICONFB2_PIN19_LSH		6
#define GPIO_ICONFB2_PIN20_LSH		8
#define GPIO_ICONFB2_PIN21_LSH		10
#define GPIO_ICONFB2_PIN22_LSH		12
#define GPIO_ICONFB2_PIN23_LSH		14
#define GPIO_ICONFB2_PIN24_LSH		16
#define GPIO_ICONFB2_PIN25_LSH		18
#define GPIO_ICONFB2_PIN26_LSH		20
#define GPIO_ICONFB2_PIN27_LSH		22
#define GPIO_ICONFB2_PIN28_LSH		24
#define GPIO_ICONFB2_PIN29_LSH		26
#define GPIO_ICONFB2_PIN30_LSH		28
#define GPIO_ICONFB2_PIN31_LSH		30

//DR
#define GPIO_DR_BIT0_LSH			0
#define GPIO_DR_BIT1_LSH			1
#define GPIO_DR_BIT2_LSH			2
#define GPIO_DR_BIT3_LSH			3
#define GPIO_DR_BIT4_LSH			4
#define GPIO_DR_BIT5_LSH			5
#define GPIO_DR_BIT6_LSH			6
#define GPIO_DR_BIT7_LSH			7
#define GPIO_DR_BIT8_LSH			8
#define GPIO_DR_BIT9_LSH			9
#define GPIO_DR_BIT10_LSH			10
#define GPIO_DR_BIT11_LSH			11
#define GPIO_DR_BIT12_LSH			12
#define GPIO_DR_BIT13_LSH			13
#define GPIO_DR_BIT14_LSH			14
#define GPIO_DR_BIT15_LSH			15
#define GPIO_DR_BIT16_LSH			16
#define GPIO_DR_BIT17_LSH			17
#define GPIO_DR_BIT18_LSH			18
#define GPIO_DR_BIT19_LSH			19
#define GPIO_DR_BIT20_LSH			20
#define GPIO_DR_BIT21_LSH			21
#define GPIO_DR_BIT22_LSH			22
#define GPIO_DR_BIT23_LSH			23
#define GPIO_DR_BIT24_LSH			24
#define GPIO_DR_BIT25_LSH			25
#define GPIO_DR_BIT26_LSH			26
#define GPIO_DR_BIT27_LSH			27
#define GPIO_DR_BIT28_LSH			28
#define GPIO_DR_BIT29_LSH			29
#define GPIO_DR_BIT30_LSH			30
#define GPIO_DR_BIT31_LSH			31

//GIUS
//NOTE: Some of the BITs may limit Read/Write Access
//Refer to Reference Manuel for detail on each Port Register
#define GPIO_GIUS_BIT0_LSH			0
#define GPIO_GIUS_BIT1_LSH			1
#define GPIO_GIUS_BIT2_LSH			2
#define GPIO_GIUS_BIT3_LSH			3
#define GPIO_GIUS_BIT4_LSH			4
#define GPIO_GIUS_BIT5_LSH			5
#define GPIO_GIUS_BIT6_LSH			6
#define GPIO_GIUS_BIT7_LSH			7
#define GPIO_GIUS_BIT8_LSH			8
#define GPIO_GIUS_BIT9_LSH			9
#define GPIO_GIUS_BIT10_LSH			10
#define GPIO_GIUS_BIT11_LSH			11
#define GPIO_GIUS_BIT12_LSH			12
#define GPIO_GIUS_BIT13_LSH			13
#define GPIO_GIUS_BIT14_LSH			14
#define GPIO_GIUS_BIT15_LSH			15
#define GPIO_GIUS_BIT16_LSH			16
#define GPIO_GIUS_BIT17_LSH			17
#define GPIO_GIUS_BIT18_LSH			18
#define GPIO_GIUS_BIT19_LSH			19
#define GPIO_GIUS_BIT20_LSH			20
#define GPIO_GIUS_BIT21_LSH			21
#define GPIO_GIUS_BIT22_LSH			22
#define GPIO_GIUS_BIT23_LSH			23
#define GPIO_GIUS_BIT24_LSH			24
#define GPIO_GIUS_BIT25_LSH			25
#define GPIO_GIUS_BIT26_LSH			26
#define GPIO_GIUS_BIT27_LSH			27
#define GPIO_GIUS_BIT28_LSH			28
#define GPIO_GIUS_BIT29_LSH			29
#define GPIO_GIUS_BIT30_LSH			30
#define GPIO_GIUS_BIT31_LSH			31

//SSR
//NOTE: Read Only
#define GPIO_SSR_BIT0_LSH			0
#define GPIO_SSR_BIT1_LSH			1
#define GPIO_SSR_BIT2_LSH			2
#define GPIO_SSR_BIT3_LSH			3
#define GPIO_SSR_BIT4_LSH			4
#define GPIO_SSR_BIT5_LSH			5
#define GPIO_SSR_BIT6_LSH			6
#define GPIO_SSR_BIT7_LSH			7
#define GPIO_SSR_BIT8_LSH			8
#define GPIO_SSR_BIT9_LSH			9
#define GPIO_SSR_BIT10_LSH			10
#define GPIO_SSR_BIT11_LSH			11
#define GPIO_SSR_BIT12_LSH			12
#define GPIO_SSR_BIT13_LSH			13
#define GPIO_SSR_BIT14_LSH			14
#define GPIO_SSR_BIT15_LSH			15
#define GPIO_SSR_BIT16_LSH			16
#define GPIO_SSR_BIT17_LSH			17
#define GPIO_SSR_BIT18_LSH			18
#define GPIO_SSR_BIT19_LSH			19
#define GPIO_SSR_BIT20_LSH			20
#define GPIO_SSR_BIT21_LSH			21
#define GPIO_SSR_BIT22_LSH			22
#define GPIO_SSR_BIT23_LSH			23
#define GPIO_SSR_BIT24_LSH			24
#define GPIO_SSR_BIT25_LSH			25
#define GPIO_SSR_BIT26_LSH			26
#define GPIO_SSR_BIT27_LSH			27
#define GPIO_SSR_BIT28_LSH			28
#define GPIO_SSR_BIT29_LSH			29
#define GPIO_SSR_BIT30_LSH			30
#define GPIO_SSR_BIT31_LSH			31

//ICR1
#define GPIO_ICR1_PIN0_LSH			0
#define GPIO_ICR1_PIN1_LSH			2
#define GPIO_ICR1_PIN2_LSH			4
#define GPIO_ICR1_PIN3_LSH			6
#define GPIO_ICR1_PIN4_LSH			8
#define GPIO_ICR1_PIN5_LSH			10
#define GPIO_ICR1_PIN6_LSH			12
#define GPIO_ICR1_PIN7_LSH			14
#define GPIO_ICR1_PIN8_LSH			16
#define GPIO_ICR1_PIN9_LSH			18
#define GPIO_ICR1_PIN10_LSH			20
#define GPIO_ICR1_PIN11_LSH			22
#define GPIO_ICR1_PIN12_LSH			24
#define GPIO_ICR1_PIN13_LSH			26
#define GPIO_ICR1_PIN14_LSH			28
#define GPIO_ICR1_PIN15_LSH			30

//ICR2
#define GPIO_ICR2_PIN16_LSH			0
#define GPIO_ICR2_PIN17_LSH			2
#define GPIO_ICR2_PIN18_LSH			4
#define GPIO_ICR2_PIN19_LSH			6
#define GPIO_ICR2_PIN20_LSH			8
#define GPIO_ICR2_PIN21_LSH			10
#define GPIO_ICR2_PIN22_LSH			12
#define GPIO_ICR2_PIN23_LSH			14
#define GPIO_ICR2_PIN24_LSH			16
#define GPIO_ICR2_PIN25_LSH			18
#define GPIO_ICR2_PIN26_LSH			20
#define GPIO_ICR2_PIN27_LSH			22
#define GPIO_ICR2_PIN28_LSH			24
#define GPIO_ICR2_PIN29_LSH			26
#define GPIO_ICR2_PIN30_LSH			28
#define GPIO_ICR2_PIN31_LSH			30

//IMR
//NOTE: Read Only
#define GPIO_IMR_BIT0_LSH			0
#define GPIO_IMR_BIT1_LSH			1
#define GPIO_IMR_BIT2_LSH			2
#define GPIO_IMR_BIT3_LSH			3
#define GPIO_IMR_BIT4_LSH			4
#define GPIO_IMR_BIT5_LSH			5
#define GPIO_IMR_BIT6_LSH			6
#define GPIO_IMR_BIT7_LSH			7
#define GPIO_IMR_BIT8_LSH			8
#define GPIO_IMR_BIT9_LSH			9
#define GPIO_IMR_BIT10_LSH			10
#define GPIO_IMR_BIT11_LSH			11
#define GPIO_IMR_BIT12_LSH			12
#define GPIO_IMR_BIT13_LSH			13
#define GPIO_IMR_BIT14_LSH			14
#define GPIO_IMR_BIT15_LSH			15
#define GPIO_IMR_BIT16_LSH			16
#define GPIO_IMR_BIT17_LSH			17
#define GPIO_IMR_BIT18_LSH			18
#define GPIO_IMR_BIT19_LSH			19
#define GPIO_IMR_BIT20_LSH			20
#define GPIO_IMR_BIT21_LSH			21
#define GPIO_IMR_BIT22_LSH			22
#define GPIO_IMR_BIT23_LSH			23
#define GPIO_IMR_BIT24_LSH			24
#define GPIO_IMR_BIT25_LSH			25
#define GPIO_IMR_BIT26_LSH			26
#define GPIO_IMR_BIT27_LSH			27
#define GPIO_IMR_BIT28_LSH			28
#define GPIO_IMR_BIT29_LSH			29
#define GPIO_IMR_BIT30_LSH			30
#define GPIO_IMR_BIT31_LSH			31

//ISR
#define GPIO_ISR_BIT0_LSH			0
#define GPIO_ISR_BIT1_LSH			1
#define GPIO_ISR_BIT2_LSH			2
#define GPIO_ISR_BIT3_LSH			3
#define GPIO_ISR_BIT4_LSH			4
#define GPIO_ISR_BIT5_LSH			5
#define GPIO_ISR_BIT6_LSH			6
#define GPIO_ISR_BIT7_LSH			7
#define GPIO_ISR_BIT8_LSH			8
#define GPIO_ISR_BIT9_LSH			9
#define GPIO_ISR_BIT10_LSH			10
#define GPIO_ISR_BIT11_LSH			11
#define GPIO_ISR_BIT12_LSH			12
#define GPIO_ISR_BIT13_LSH			13
#define GPIO_ISR_BIT14_LSH			14
#define GPIO_ISR_BIT15_LSH			15
#define GPIO_ISR_BIT16_LSH			16
#define GPIO_ISR_BIT17_LSH			17
#define GPIO_ISR_BIT18_LSH			18
#define GPIO_ISR_BIT19_LSH			19
#define GPIO_ISR_BIT20_LSH			20
#define GPIO_ISR_BIT21_LSH			21
#define GPIO_ISR_BIT22_LSH			22
#define GPIO_ISR_BIT23_LSH			23
#define GPIO_ISR_BIT24_LSH			24
#define GPIO_ISR_BIT25_LSH			25
#define GPIO_ISR_BIT26_LSH			26
#define GPIO_ISR_BIT27_LSH			27
#define GPIO_ISR_BIT28_LSH			28
#define GPIO_ISR_BIT29_LSH			29
#define GPIO_ISR_BIT30_LSH			30
#define GPIO_ISR_BIT31_LSH			31

//GPR
#define GPIO_GPR_BIT0_LSH			0
#define GPIO_GPR_BIT1_LSH			1
#define GPIO_GPR_BIT2_LSH			2
#define GPIO_GPR_BIT3_LSH			3
#define GPIO_GPR_BIT4_LSH			4
#define GPIO_GPR_BIT5_LSH			5
#define GPIO_GPR_BIT6_LSH			6
#define GPIO_GPR_BIT7_LSH			7
#define GPIO_GPR_BIT8_LSH			8
#define GPIO_GPR_BIT9_LSH			9
#define GPIO_GPR_BIT10_LSH			10
#define GPIO_GPR_BIT11_LSH			11
#define GPIO_GPR_BIT12_LSH			12
#define GPIO_GPR_BIT13_LSH			13
#define GPIO_GPR_BIT14_LSH			14
#define GPIO_GPR_BIT15_LSH			15
#define GPIO_GPR_BIT16_LSH			16
#define GPIO_GPR_BIT17_LSH			17
#define GPIO_GPR_BIT18_LSH			18
#define GPIO_GPR_BIT19_LSH			19
#define GPIO_GPR_BIT20_LSH			20
#define GPIO_GPR_BIT21_LSH			21
#define GPIO_GPR_BIT22_LSH			22
#define GPIO_GPR_BIT23_LSH			23
#define GPIO_GPR_BIT24_LSH			24
#define GPIO_GPR_BIT25_LSH			25
#define GPIO_GPR_BIT26_LSH			26
#define GPIO_GPR_BIT27_LSH			27
#define GPIO_GPR_BIT28_LSH			28
#define GPIO_GPR_BIT29_LSH			29
#define GPIO_GPR_BIT30_LSH			30
#define GPIO_GPR_BIT31_LSH			31

//SWR
#define GPIO_SWR_LSH				0

//PUEN
#define GPIO_PUEN_BIT0_LSH			0
#define GPIO_PUEN_BIT1_LSH			1
#define GPIO_PUEN_BIT2_LSH			2
#define GPIO_PUEN_BIT3_LSH			3
#define GPIO_PUEN_BIT4_LSH			4
#define GPIO_PUEN_BIT5_LSH			5
#define GPIO_PUEN_BIT6_LSH			6
#define GPIO_PUEN_BIT7_LSH			7
#define GPIO_PUEN_BIT8_LSH			8
#define GPIO_PUEN_BIT9_LSH			9
#define GPIO_PUEN_BIT10_LSH			10
#define GPIO_PUEN_BIT11_LSH			11
#define GPIO_PUEN_BIT12_LSH			12
#define GPIO_PUEN_BIT13_LSH			13
#define GPIO_PUEN_BIT14_LSH			14
#define GPIO_PUEN_BIT15_LSH			15
#define GPIO_PUEN_BIT16_LSH			16
#define GPIO_PUEN_BIT17_LSH			17
#define GPIO_PUEN_BIT18_LSH			18
#define GPIO_PUEN_BIT19_LSH			19
#define GPIO_PUEN_BIT20_LSH			20
#define GPIO_PUEN_BIT21_LSH			21
#define GPIO_PUEN_BIT22_LSH			22
#define GPIO_PUEN_BIT23_LSH			23
#define GPIO_PUEN_BIT24_LSH			24
#define GPIO_PUEN_BIT25_LSH			25
#define GPIO_PUEN_BIT26_LSH			26
#define GPIO_PUEN_BIT27_LSH			27
#define GPIO_PUEN_BIT28_LSH			28
#define GPIO_PUEN_BIT29_LSH			29
#define GPIO_PUEN_BIT30_LSH			30
#define GPIO_PUEN_BIT31_LSH			31

//PMASK
#define GPIO_PMASK_PTA_LSH			0
#define GPIO_PMASK_PTB_LSH			1
#define GPIO_PMASK_PTC_LSH			2
#define GPIO_PMASK_PTD_LSH			3
#define GPIO_PMASK_PTE_LSH			4
#define GPIO_PMASK_PTF_LSH			5

//------------------------------------------------------------------------------
// REGISTER FIELD WIDTHS
//------------------------------------------------------------------------------
//DDIR
#define GPIO_DDIR_BIT0_WID			Width_1
#define GPIO_DDIR_BIT1_WID			Width_1
#define GPIO_DDIR_BIT2_WID			Width_1
#define GPIO_DDIR_BIT3_WID			Width_1
#define GPIO_DDIR_BIT4_WID			Width_1
#define GPIO_DDIR_BIT5_WID			Width_1
#define GPIO_DDIR_BIT6_WID			Width_1
#define GPIO_DDIR_BIT7_WID			Width_1
#define GPIO_DDIR_BIT8_WID			Width_1
#define GPIO_DDIR_BIT9_WID			Width_1
#define GPIO_DDIR_BIT10_WID			Width_1
#define GPIO_DDIR_BIT11_WID			Width_1
#define GPIO_DDIR_BIT12_WID			Width_1
#define GPIO_DDIR_BIT13_WID			Width_1
#define GPIO_DDIR_BIT14_WID			Width_1
#define GPIO_DDIR_BIT15_WID			Width_1
#define GPIO_DDIR_BIT16_WID			Width_1
#define GPIO_DDIR_BIT17_WID			Width_1
#define GPIO_DDIR_BIT18_WID			Width_1
#define GPIO_DDIR_BIT19_WID			Width_1
#define GPIO_DDIR_BIT20_WID			Width_1
#define GPIO_DDIR_BIT21_WID			Width_1
#define GPIO_DDIR_BIT22_WID			Width_1
#define GPIO_DDIR_BIT23_WID			Width_1
#define GPIO_DDIR_BIT24_WID			Width_1
#define GPIO_DDIR_BIT25_WID			Width_1
#define GPIO_DDIR_BIT26_WID			Width_1
#define GPIO_DDIR_BIT27_WID			Width_1
#define GPIO_DDIR_BIT28_WID			Width_1
#define GPIO_DDIR_BIT29_WID			Width_1
#define GPIO_DDIR_BIT30_WID			Width_1
#define GPIO_DDIR_BIT31_WID			Width_1

//OCR1
#define GPIO_OCR1_PIN0_WID			Width_2
#define GPIO_OCR1_PIN1_WID			Width_2
#define GPIO_OCR1_PIN2_WID			Width_2
#define GPIO_OCR1_PIN3_WID			Width_2
#define GPIO_OCR1_PIN4_WID			Width_2
#define GPIO_OCR1_PIN5_WID			Width_2
#define GPIO_OCR1_PIN6_WID			Width_2
#define GPIO_OCR1_PIN7_WID			Width_2
#define GPIO_OCR1_PIN8_WID			Width_2
#define GPIO_OCR1_PIN9_WID			Width_2
#define GPIO_OCR1_PIN10_WID			Width_2
#define GPIO_OCR1_PIN11_WID			Width_2
#define GPIO_OCR1_PIN12_WID			Width_2
#define GPIO_OCR1_PIN13_WID			Width_2
#define GPIO_OCR1_PIN14_WID			Width_2
#define GPIO_OCR1_PIN15_WID			Width_2

//OCR2
#define GPIO_OCR2_PIN16_WID			Width_2
#define GPIO_OCR2_PIN17_WID			Width_2
#define GPIO_OCR2_PIN18_WID			Width_2
#define GPIO_OCR2_PIN19_WID			Width_2
#define GPIO_OCR2_PIN20_WID			Width_2
#define GPIO_OCR2_PIN21_WID			Width_2
#define GPIO_OCR2_PIN22_WID			Width_2
#define GPIO_OCR2_PIN23_WID			Width_2
#define GPIO_OCR2_PIN24_WID			Width_2
#define GPIO_OCR2_PIN25_WID			Width_2
#define GPIO_OCR2_PIN26_WID			Width_2
#define GPIO_OCR2_PIN27_WID			Width_2
#define GPIO_OCR2_PIN28_WID			Width_2
#define GPIO_OCR2_PIN29_WID			Width_2
#define GPIO_OCR2_PIN30_WID			Width_2
#define GPIO_OCR2_PIN31_WID			Width_2

//ICONFA1
#define GPIO_ICONFA1_PIN0_WID		Width_2
#define GPIO_ICONFA1_PIN1_WID		Width_2
#define GPIO_ICONFA1_PIN2_WID		Width_2
#define GPIO_ICONFA1_PIN3_WID		Width_2
#define GPIO_ICONFA1_PIN4_WID		Width_2
#define GPIO_ICONFA1_PIN5_WID		Width_2
#define GPIO_ICONFA1_PIN6_WID		Width_2
#define GPIO_ICONFA1_PIN7_WID		Width_2
#define GPIO_ICONFA1_PIN8_WID		Width_2
#define GPIO_ICONFA1_PIN9_WID		Width_2
#define GPIO_ICONFA1_PIN10_WID		Width_2
#define GPIO_ICONFA1_PIN11_WID		Width_2
#define GPIO_ICONFA1_PIN12_WID		Width_2
#define GPIO_ICONFA1_PIN13_WID		Width_2
#define GPIO_ICONFA1_PIN14_WID		Width_2
#define GPIO_ICONFA1_PIN15_WID		Width_2

//ICONFA2
#define GPIO_ICONFA2_PIN16_WID		Width_2
#define GPIO_ICONFA2_PIN17_WID		Width_2
#define GPIO_ICONFA2_PIN18_WID		Width_2
#define GPIO_ICONFA2_PIN19_WID		Width_2
#define GPIO_ICONFA2_PIN20_WID		Width_2
#define GPIO_ICONFA2_PIN21_WID		Width_2
#define GPIO_ICONFA2_PIN22_WID		Width_2
#define GPIO_ICONFA2_PIN23_WID		Width_2
#define GPIO_ICONFA2_PIN24_WID		Width_2
#define GPIO_ICONFA2_PIN25_WID		Width_2
#define GPIO_ICONFA2_PIN26_WID		Width_2
#define GPIO_ICONFA2_PIN27_WID		Width_2
#define GPIO_ICONFA2_PIN28_WID		Width_2
#define GPIO_ICONFA2_PIN29_WID		Width_2
#define GPIO_ICONFA2_PIN30_WID		Width_2
#define GPIO_ICONFA2_PIN31_WID		Width_2

//ICONFB1
#define GPIO_ICONFB1_PIN0_WID		Width_2
#define GPIO_ICONFB1_PIN1_WID		Width_2
#define GPIO_ICONFB1_PIN2_WID		Width_2
#define GPIO_ICONFB1_PIN3_WID		Width_2
#define GPIO_ICONFB1_PIN4_WID		Width_2
#define GPIO_ICONFB1_PIN5_WID		Width_2
#define GPIO_ICONFB1_PIN6_WID		Width_2
#define GPIO_ICONFB1_PIN7_WID		Width_2
#define GPIO_ICONFB1_PIN8_WID		Width_2
#define GPIO_ICONFB1_PIN9_WID		Width_2
#define GPIO_ICONFB1_PIN10_WID		Width_2
#define GPIO_ICONFB1_PIN11_WID		Width_2
#define GPIO_ICONFB1_PIN12_WID		Width_2
#define GPIO_ICONFB1_PIN13_WID		Width_2
#define GPIO_ICONFB1_PIN14_WID		Width_2
#define GPIO_ICONFB1_PIN15_WID		Width_2

//ICONFB2
#define GPIO_ICONFB2_PIN16_WID		Width_2
#define GPIO_ICONFB2_PIN17_WID		Width_2
#define GPIO_ICONFB2_PIN18_WID		Width_2
#define GPIO_ICONFB2_PIN19_WID		Width_2
#define GPIO_ICONFB2_PIN20_WID		Width_2
#define GPIO_ICONFB2_PIN21_WID		Width_2
#define GPIO_ICONFB2_PIN22_WID		Width_2
#define GPIO_ICONFB2_PIN23_WID		Width_2
#define GPIO_ICONFB2_PIN24_WID		Width_2
#define GPIO_ICONFB2_PIN25_WID		Width_2
#define GPIO_ICONFB2_PIN26_WID		Width_2
#define GPIO_ICONFB2_PIN27_WID		Width_2
#define GPIO_ICONFB2_PIN28_WID		Width_2
#define GPIO_ICONFB2_PIN29_WID		Width_2
#define GPIO_ICONFB2_PIN30_WID		Width_2
#define GPIO_ICONFB2_PIN31_WID		Width_2

//DR
#define GPIO_DR_BIT0_WID			Width_1
#define GPIO_DR_BIT1_WID			Width_1
#define GPIO_DR_BIT2_WID			Width_1
#define GPIO_DR_BIT3_WID			Width_1
#define GPIO_DR_BIT4_WID			Width_1
#define GPIO_DR_BIT5_WID			Width_1
#define GPIO_DR_BIT6_WID			Width_1
#define GPIO_DR_BIT7_WID			Width_1
#define GPIO_DR_BIT8_WID			Width_1
#define GPIO_DR_BIT9_WID			Width_1
#define GPIO_DR_BIT10_WID			Width_1
#define GPIO_DR_BIT11_WID			Width_1
#define GPIO_DR_BIT12_WID			Width_1
#define GPIO_DR_BIT13_WID			Width_1
#define GPIO_DR_BIT14_WID			Width_1
#define GPIO_DR_BIT15_WID			Width_1
#define GPIO_DR_BIT16_WID			Width_1
#define GPIO_DR_BIT17_WID			Width_1
#define GPIO_DR_BIT18_WID			Width_1
#define GPIO_DR_BIT19_WID			Width_1
#define GPIO_DR_BIT20_WID			Width_1
#define GPIO_DR_BIT21_WID			Width_1
#define GPIO_DR_BIT22_WID			Width_1
#define GPIO_DR_BIT23_WID			Width_1
#define GPIO_DR_BIT24_WID			Width_1
#define GPIO_DR_BIT25_WID			Width_1
#define GPIO_DR_BIT26_WID			Width_1
#define GPIO_DR_BIT27_WID			Width_1
#define GPIO_DR_BIT28_WID			Width_1
#define GPIO_DR_BIT29_WID			Width_1
#define GPIO_DR_BIT30_WID			Width_1
#define GPIO_DR_BIT31_WID			Width_1

//GIUS
//NOTE: Some of the BITs may limit Read/Write Access
//Refer to Reference Manuel for detail on each Port Register
#define GPIO_GIUS_BIT0_WID			Width_1
#define GPIO_GIUS_BIT1_WID			Width_1
#define GPIO_GIUS_BIT2_WID			Width_1
#define GPIO_GIUS_BIT3_WID			Width_1
#define GPIO_GIUS_BIT4_WID			Width_1
#define GPIO_GIUS_BIT5_WID			Width_1
#define GPIO_GIUS_BIT6_WID			Width_1
#define GPIO_GIUS_BIT7_WID			Width_1
#define GPIO_GIUS_BIT8_WID			Width_1
#define GPIO_GIUS_BIT9_WID			Width_1
#define GPIO_GIUS_BIT10_WID			Width_1
#define GPIO_GIUS_BIT11_WID			Width_1
#define GPIO_GIUS_BIT12_WID			Width_1
#define GPIO_GIUS_BIT13_WID			Width_1
#define GPIO_GIUS_BIT14_WID			Width_1
#define GPIO_GIUS_BIT15_WID			Width_1
#define GPIO_GIUS_BIT16_WID			Width_1
#define GPIO_GIUS_BIT17_WID			Width_1
#define GPIO_GIUS_BIT18_WID			Width_1
#define GPIO_GIUS_BIT19_WID			Width_1
#define GPIO_GIUS_BIT20_WID			Width_1
#define GPIO_GIUS_BIT21_WID			Width_1
#define GPIO_GIUS_BIT22_WID			Width_1
#define GPIO_GIUS_BIT23_WID			Width_1
#define GPIO_GIUS_BIT24_WID			Width_1
#define GPIO_GIUS_BIT25_WID			Width_1
#define GPIO_GIUS_BIT26_WID			Width_1
#define GPIO_GIUS_BIT27_WID			Width_1
#define GPIO_GIUS_BIT28_WID			Width_1
#define GPIO_GIUS_BIT29_WID			Width_1
#define GPIO_GIUS_BIT30_WID			Width_1
#define GPIO_GIUS_BIT31_WID			Width_1

//SSR
//NOTE: Read Only
#define GPIO_SSR_BIT0_WID			Width_1
#define GPIO_SSR_BIT1_WID			Width_1
#define GPIO_SSR_BIT2_WID			Width_1
#define GPIO_SSR_BIT3_WID			Width_1
#define GPIO_SSR_BIT4_WID			Width_1
#define GPIO_SSR_BIT5_WID			Width_1
#define GPIO_SSR_BIT6_WID			Width_1
#define GPIO_SSR_BIT7_WID			Width_1
#define GPIO_SSR_BIT8_WID			Width_1
#define GPIO_SSR_BIT9_WID			Width_1
#define GPIO_SSR_BIT10_WID			Width_1
#define GPIO_SSR_BIT11_WID			Width_1
#define GPIO_SSR_BIT12_WID			Width_1
#define GPIO_SSR_BIT13_WID			Width_1
#define GPIO_SSR_BIT14_WID			Width_1
#define GPIO_SSR_BIT15_WID			Width_1
#define GPIO_SSR_BIT16_WID			Width_1
#define GPIO_SSR_BIT17_WID			Width_1
#define GPIO_SSR_BIT18_WID			Width_1
#define GPIO_SSR_BIT19_WID			Width_1
#define GPIO_SSR_BIT20_WID			Width_1
#define GPIO_SSR_BIT21_WID			Width_1
#define GPIO_SSR_BIT22_WID			Width_1
#define GPIO_SSR_BIT23_WID			Width_1
#define GPIO_SSR_BIT24_WID			Width_1
#define GPIO_SSR_BIT25_WID			Width_1
#define GPIO_SSR_BIT26_WID			Width_1
#define GPIO_SSR_BIT27_WID			Width_1
#define GPIO_SSR_BIT28_WID			Width_1
#define GPIO_SSR_BIT29_WID			Width_1
#define GPIO_SSR_BIT30_WID			Width_1
#define GPIO_SSR_BIT31_WID			Width_1

//ICR1
#define GPIO_ICR1_PIN0_WID			Width_2
#define GPIO_ICR1_PIN1_WID			Width_2
#define GPIO_ICR1_PIN2_WID			Width_2
#define GPIO_ICR1_PIN3_WID			Width_2
#define GPIO_ICR1_PIN4_WID			Width_2
#define GPIO_ICR1_PIN5_WID			Width_2
#define GPIO_ICR1_PIN6_WID			Width_2
#define GPIO_ICR1_PIN7_WID			Width_2
#define GPIO_ICR1_PIN8_WID			Width_2
#define GPIO_ICR1_PIN9_WID			Width_2
#define GPIO_ICR1_PIN10_WID			Width_2
#define GPIO_ICR1_PIN11_WID			Width_2
#define GPIO_ICR1_PIN12_WID			Width_2
#define GPIO_ICR1_PIN13_WID			Width_2
#define GPIO_ICR1_PIN14_WID			Width_2
#define GPIO_ICR1_PIN15_WID			Width_2

//ICR2
#define GPIO_ICR2_PIN16_WID			Width_2
#define GPIO_ICR2_PIN17_WID			Width_2
#define GPIO_ICR2_PIN18_WID			Width_2
#define GPIO_ICR2_PIN19_WID			Width_2
#define GPIO_ICR2_PIN20_WID			Width_2
#define GPIO_ICR2_PIN21_WID			Width_2
#define GPIO_ICR2_PIN22_WID			Width_2
#define GPIO_ICR2_PIN23_WID			Width_2
#define GPIO_ICR2_PIN24_WID			Width_2
#define GPIO_ICR2_PIN25_WID			Width_2
#define GPIO_ICR2_PIN26_WID			Width_2
#define GPIO_ICR2_PIN27_WID			Width_2
#define GPIO_ICR2_PIN28_WID			Width_2
#define GPIO_ICR2_PIN29_WID			Width_2
#define GPIO_ICR2_PIN30_WID			Width_2
#define GPIO_ICR2_PIN31_WID			Width_2

//IMR
#define GPIO_IMR_BIT0_WID			Width_1
#define GPIO_IMR_BIT1_WID			Width_1
#define GPIO_IMR_BIT2_WID			Width_1
#define GPIO_IMR_BIT3_WID			Width_1
#define GPIO_IMR_BIT4_WID			Width_1
#define GPIO_IMR_BIT5_WID			Width_1
#define GPIO_IMR_BIT6_WID			Width_1
#define GPIO_IMR_BIT7_WID			Width_1
#define GPIO_IMR_BIT8_WID			Width_1
#define GPIO_IMR_BIT9_WID			Width_1
#define GPIO_IMR_BIT10_WID			Width_1
#define GPIO_IMR_BIT11_WID			Width_1
#define GPIO_IMR_BIT12_WID			Width_1
#define GPIO_IMR_BIT13_WID			Width_1
#define GPIO_IMR_BIT14_WID			Width_1
#define GPIO_IMR_BIT15_WID			Width_1
#define GPIO_IMR_BIT16_WID			Width_1
#define GPIO_IMR_BIT17_WID			Width_1
#define GPIO_IMR_BIT18_WID			Width_1
#define GPIO_IMR_BIT19_WID			Width_1
#define GPIO_IMR_BIT20_WID			Width_1
#define GPIO_IMR_BIT21_WID			Width_1
#define GPIO_IMR_BIT22_WID			Width_1
#define GPIO_IMR_BIT23_WID			Width_1
#define GPIO_IMR_BIT24_WID			Width_1
#define GPIO_IMR_BIT25_WID			Width_1
#define GPIO_IMR_BIT26_WID			Width_1
#define GPIO_IMR_BIT27_WID			Width_1
#define GPIO_IMR_BIT28_WID			Width_1
#define GPIO_IMR_BIT29_WID			Width_1
#define GPIO_IMR_BIT30_WID			Width_1
#define GPIO_IMR_BIT31_WID			Width_1

//ISR
#define GPIO_ISR_BIT0_WID			Width_1
#define GPIO_ISR_BIT1_WID			Width_1
#define GPIO_ISR_BIT2_WID			Width_1
#define GPIO_ISR_BIT3_WID			Width_1
#define GPIO_ISR_BIT4_WID			Width_1
#define GPIO_ISR_BIT5_WID			Width_1
#define GPIO_ISR_BIT6_WID			Width_1
#define GPIO_ISR_BIT7_WID			Width_1
#define GPIO_ISR_BIT8_WID			Width_1
#define GPIO_ISR_BIT9_WID			Width_1
#define GPIO_ISR_BIT10_WID			Width_1
#define GPIO_ISR_BIT11_WID			Width_1
#define GPIO_ISR_BIT12_WID			Width_1
#define GPIO_ISR_BIT13_WID			Width_1
#define GPIO_ISR_BIT14_WID			Width_1
#define GPIO_ISR_BIT15_WID			Width_1
#define GPIO_ISR_BIT16_WID			Width_1
#define GPIO_ISR_BIT17_WID			Width_1
#define GPIO_ISR_BIT18_WID			Width_1
#define GPIO_ISR_BIT19_WID			Width_1
#define GPIO_ISR_BIT20_WID			Width_1
#define GPIO_ISR_BIT21_WID			Width_1
#define GPIO_ISR_BIT22_WID			Width_1
#define GPIO_ISR_BIT23_WID			Width_1
#define GPIO_ISR_BIT24_WID			Width_1
#define GPIO_ISR_BIT25_WID			Width_1
#define GPIO_ISR_BIT26_WID			Width_1
#define GPIO_ISR_BIT27_WID			Width_1
#define GPIO_ISR_BIT28_WID			Width_1
#define GPIO_ISR_BIT29_WID			Width_1
#define GPIO_ISR_BIT30_WID			Width_1
#define GPIO_ISR_BIT31_WID			Width_1

//GPR
#define GPIO_GPR_BIT0_WID			Width_1
#define GPIO_GPR_BIT1_WID			Width_1
#define GPIO_GPR_BIT2_WID			Width_1
#define GPIO_GPR_BIT3_WID			Width_1
#define GPIO_GPR_BIT4_WID			Width_1
#define GPIO_GPR_BIT5_WID			Width_1
#define GPIO_GPR_BIT6_WID			Width_1
#define GPIO_GPR_BIT7_WID			Width_1
#define GPIO_GPR_BIT8_WID			Width_1
#define GPIO_GPR_BIT9_WID			Width_1
#define GPIO_GPR_BIT10_WID			Width_1
#define GPIO_GPR_BIT11_WID			Width_1
#define GPIO_GPR_BIT12_WID			Width_1
#define GPIO_GPR_BIT13_WID			Width_1
#define GPIO_GPR_BIT14_WID			Width_1
#define GPIO_GPR_BIT15_WID			Width_1
#define GPIO_GPR_BIT16_WID			Width_1
#define GPIO_GPR_BIT17_WID			Width_1
#define GPIO_GPR_BIT18_WID			Width_1
#define GPIO_GPR_BIT19_WID			Width_1
#define GPIO_GPR_BIT20_WID			Width_1
#define GPIO_GPR_BIT21_WID			Width_1
#define GPIO_GPR_BIT22_WID			Width_1
#define GPIO_GPR_BIT23_WID			Width_1
#define GPIO_GPR_BIT24_WID			Width_1
#define GPIO_GPR_BIT25_WID			Width_1
#define GPIO_GPR_BIT26_WID			Width_1
#define GPIO_GPR_BIT27_WID			Width_1
#define GPIO_GPR_BIT28_WID			Width_1
#define GPIO_GPR_BIT29_WID			Width_1
#define GPIO_GPR_BIT30_WID			Width_1
#define GPIO_GPR_BIT31_WID			Width_1

//SWR
#define GPIO_SWR_WID				Width_1

//PUEN
#define GPIO_PUEN_BIT0_WID			Width_1
#define GPIO_PUEN_BIT1_WID			Width_1
#define GPIO_PUEN_BIT2_WID			Width_1
#define GPIO_PUEN_BIT3_WID			Width_1
#define GPIO_PUEN_BIT4_WID			Width_1
#define GPIO_PUEN_BIT5_WID			Width_1
#define GPIO_PUEN_BIT6_WID			Width_1
#define GPIO_PUEN_BIT7_WID			Width_1
#define GPIO_PUEN_BIT8_WID			Width_1
#define GPIO_PUEN_BIT9_WID			Width_1
#define GPIO_PUEN_BIT10_WID			Width_1
#define GPIO_PUEN_BIT11_WID			Width_1
#define GPIO_PUEN_BIT12_WID			Width_1
#define GPIO_PUEN_BIT13_WID			Width_1
#define GPIO_PUEN_BIT14_WID			Width_1
#define GPIO_PUEN_BIT15_WID			Width_1
#define GPIO_PUEN_BIT16_WID			Width_1
#define GPIO_PUEN_BIT17_WID			Width_1
#define GPIO_PUEN_BIT18_WID			Width_1
#define GPIO_PUEN_BIT19_WID			Width_1
#define GPIO_PUEN_BIT20_WID			Width_1
#define GPIO_PUEN_BIT21_WID			Width_1
#define GPIO_PUEN_BIT22_WID			Width_1
#define GPIO_PUEN_BIT23_WID			Width_1
#define GPIO_PUEN_BIT24_WID			Width_1
#define GPIO_PUEN_BIT25_WID			Width_1
#define GPIO_PUEN_BIT26_WID			Width_1
#define GPIO_PUEN_BIT27_WID			Width_1
#define GPIO_PUEN_BIT28_WID			Width_1
#define GPIO_PUEN_BIT29_WID			Width_1
#define GPIO_PUEN_BIT30_WID			Width_1
#define GPIO_PUEN_BIT31_WID			Width_1

//PMASK
#define GPIO_PMASK_PTA_WID			Width_1
#define GPIO_PMASK_PTB_WID			Width_1
#define GPIO_PMASK_PTC_WID			Width_1
#define GPIO_PMASK_PTD_WID			Width_1
#define GPIO_PMASK_PTE_WID			Width_1
#define GPIO_PMASK_PTF_WID			Width_1


//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------
#define GPIO_DDIR_INPUT             0 // GPIO pin is input
#define GPIO_DDIR_OUTPUT            1 // GPIO pin is output

#define GPIO_OCR_AIN                0 // Output selected is Input A_IN[i]
#define GPIO_OCR_BIN                1 // Output selected is Input B_IN[i]
#define GPIO_OCR_CIN                2 // Output selected is Input C_IN[i]
#define GPIO_OCR_DATA               3 // Output selected is data register[i]

#define GPIO_ICONFA_GPIO            0 // A_OUT[i] is GPIO_In[i]
#define GPIO_ICONFA_ISR             1 // A_OUT[i] is Interrupt Status register[i]
#define GPIO_ICONFA_0               2 // A_OUT[i] is 0
#define GPIO_ICONFA_1               3 // A_OUT[i] is 1

#define GPIO_ICONFB_GPIO            0 // B_OUT[i] is GPIO_In[i]
#define GPIO_ICONFB_ISR             1 // B_OUT[i] is Interrupt Status register[i]
#define GPIO_ICONFB_0               2 // B_OUT[i] is 0
#define GPIO_ICONFB_1               3 // B_OUT[i] is 1

#define GPIO_DR_LOW             	0 // GPIO pin is set LOW
#define GPIO_DR_HIGH              	1 // GPIO pin is set HIGH

#define GPIO_GIUS_MUX               0 // GPIO pin is utilized for multiplexed function
#define GPIO_GIUS_GPIO              1 // GPIO pin is utilized for GPIO function

#define GPIO_ICR_RISE_EDGE          0 // Interrupt is rising edge
#define GPIO_ICR_FALL_EDGE          1 // Interrupt is falling edge
#define GPIO_ICR_HIGH_LEVEL         2 // Interrupt is high-level
#define GPIO_ICR_LOW_LEVEL          3 // Interrupt is low-level

#define GPIO_IMR_MASKED             0 // Interrupt is masked
#define GPIO_IMR_UNMASKED           1 // Interrupt is unmasked

#define GPIO_GPR_PRI                0 // Select primary pin function
#define GPIO_GPR_ALT                1 // Select alternate pin function

#define GPIO_SWR_NOEFFECT           0 // No reset
#define GPIO_SWR_RESET              1 // GPIO circuitry for Port X reset

#define GPIO_PUEN_TRISTATE          0 // Pin [i] is tri-stated when not driven
#define GPIO_PUEN_HIGH              1 // Pin [i] is pulled high1 when not driven

#define GPIO_PMASK_MASKED           0 // Port interrupt is masked
#define GPIO_PMASK_UNMASKED         1 // Port interrupt is unmasked



#endif /*IMX27_GPIO_H_*/



