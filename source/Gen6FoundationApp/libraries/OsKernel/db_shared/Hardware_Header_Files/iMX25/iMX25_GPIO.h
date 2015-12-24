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
// $RCSfile: db_shared/Hardware_Header_Files/iMX25/iMX25_GPIO.h $
// $Revision: 1.6 $
// $Date: 2011/06/28 13:26:50EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx25_GPIO.h
//
// Provides definitions for the General-Purpose I/O (GPIO)
// Refer to Chapter 37 of the iMX25 Reference Manual
//
//-----------------------------------------------------------------------------

#ifndef IMX25_GPIO_H_
#define IMX25_GPIO_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width 
//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------
// #########################################
// # GPIO1                                 #
// #########################################
#define GPIO1_BASE_ADDR            0x53FCC000U
#define GPIO1_DR0                  (GPIO1_BASE_ADDR+0x00U)    //DATA Register
#define GPIO1_GDIR0                (GPIO1_BASE_ADDR+0x04U)    //Direction Register
#define GPIO1_PSR0                 (GPIO1_BASE_ADDR+0x08U)    //PAD Status
#define GPIO1_ICR1                 (GPIO1_BASE_ADDR+0x0CU)    //Interrupt Configuration Register1 
#define GPIO1_ICR2                 (GPIO1_BASE_ADDR+0x10U)    //Interrupt Configuration Register2
#define GPIO1_IMR                  (GPIO1_BASE_ADDR+0x14U)    //Interrupt Mask Register
#define GPIO1_ISR                  (GPIO1_BASE_ADDR+0x18U)    //Interrupt Status Register
#define GPIO1_EDGE                 (GPIO1_BASE_ADDR+0x1CU)    //Edge Select Register

// #########################################
// # GPIO2                                 #
// #########################################
#define GPIO2_BASE_ADDR            0x53FD0000U
#define GPIO2_DR0                  (GPIO2_BASE_ADDR+0x00U)    
#define GPIO2_GDIR0                (GPIO2_BASE_ADDR+0x04U)    
#define GPIO2_PSR0                 (GPIO2_BASE_ADDR+0x08U)    
#define GPIO2_ICR1                 (GPIO2_BASE_ADDR+0x0CU)    
#define GPIO2_ICR2                 (GPIO2_BASE_ADDR+0x10U)    
#define GPIO2_IMR                  (GPIO2_BASE_ADDR+0x14U)    
#define GPIO2_ISR                  (GPIO2_BASE_ADDR+0x18U) 
#define GPIO2_EDGE                 (GPIO2_BASE_ADDR+0x1CU)

// #########################################
// # GPIO3                                 #
// #########################################
#define GPIO3_BASE_ADDR            0x53FA4000U
#define GPIO3_DR0                  (GPIO3_BASE_ADDR+0x00U)    
#define GPIO3_GDIR0                (GPIO3_BASE_ADDR+0x04U)    
#define GPIO3_PSR0                 (GPIO3_BASE_ADDR+0x08U)    
#define GPIO3_ICR1                 (GPIO3_BASE_ADDR+0x0CU)    
#define GPIO3_ICR2                 (GPIO3_BASE_ADDR+0x10U)    
#define GPIO3_IMR                  (GPIO3_BASE_ADDR+0x14U)    
#define GPIO3_ISR                  (GPIO3_BASE_ADDR+0x18U)
#define GPIO3_EDGE                 (GPIO3_BASE_ADDR+0x1CU)

// #########################################
// # GPIO4                                 #
// #########################################
#define GPIO4_BASE_ADDR            0x53F9C000U
#define GPIO4_DR0                  (GPIO4_BASE_ADDR+0x00U)    
#define GPIO4_GDIR0                (GPIO4_BASE_ADDR+0x04U)    
#define GPIO4_PSR0                 (GPIO4_BASE_ADDR+0x08U)    
#define GPIO4_ICR1                 (GPIO4_BASE_ADDR+0x0CU)    
#define GPIO4_ICR2                 (GPIO4_BASE_ADDR+0x10U)    
#define GPIO4_IMR                  (GPIO4_BASE_ADDR+0x14U)    
#define GPIO4_ISR                  (GPIO4_BASE_ADDR+0x18U)  
#define GPIO4_EDGE                 (GPIO4_BASE_ADDR+0x1CU)

//------------------------------------------------------------------------------
// REGISTER BIT FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
//GPIO Data Register (DR)
#define GPIO_BIT0_LSH			0
#define GPIO_BIT1_LSH			1
#define GPIO_BIT2_LSH			2
#define GPIO_BIT3_LSH			3
#define GPIO_BIT4_LSH			4
#define GPIO_BIT5_LSH			5
#define GPIO_BIT6_LSH			6
#define GPIO_BIT7_LSH			7
#define GPIO_BIT8_LSH			8
#define GPIO_BIT9_LSH			9
#define GPIO_BIT10_LSH			10
#define GPIO_BIT11_LSH			11
#define GPIO_BIT12_LSH			12
#define GPIO_BIT13_LSH			13
#define GPIO_BIT14_LSH			14
#define GPIO_BIT15_LSH			15
#define GPIO_BIT16_LSH			16
#define GPIO_BIT17_LSH			17
#define GPIO_BIT18_LSH			18
#define GPIO_BIT19_LSH			19
#define GPIO_BIT20_LSH			20
#define GPIO_BIT21_LSH			21
#define GPIO_BIT22_LSH			22
#define GPIO_BIT23_LSH			23
#define GPIO_BIT24_LSH			24
#define GPIO_BIT25_LSH			25
#define GPIO_BIT26_LSH			26
#define GPIO_BIT27_LSH			27
#define GPIO_BIT28_LSH			28
#define GPIO_BIT29_LSH			29
#define GPIO_BIT30_LSH			30
#define GPIO_BIT31_LSH			31

#define GPIO_ICR0_LSH			0
#define GPIO_ICR1_LSH			2
#define GPIO_ICR2_LSH			4
#define GPIO_ICR3_LSH			6
#define GPIO_ICR4_LSH			8
#define GPIO_ICR5_LSH			10
#define GPIO_ICR6_LSH			12
#define GPIO_ICR7_LSH			14
#define GPIO_ICR8_LSH			16
#define GPIO_ICR9_LSH			18
#define GPIO_ICR10_LSH			20
#define GPIO_ICR11_LSH			22
#define GPIO_ICR12_LSH			24
#define GPIO_ICR13_LSH			26
#define GPIO_ICR14_LSH			28
#define GPIO_ICR15_LSH			30

#define GPIO_ICR16_LSH			0
#define GPIO_ICR17_LSH			2
#define GPIO_ICR18_LSH			4
#define GPIO_ICR19_LSH			6
#define GPIO_ICR20_LSH			8
#define GPIO_ICR21_LSH			10
#define GPIO_ICR22_LSH			12
#define GPIO_ICR23_LSH			14
#define GPIO_ICR24_LSH			16
#define GPIO_ICR25_LSH			18
#define GPIO_ICR26_LSH			20
#define GPIO_ICR27_LSH			22
#define GPIO_ICR28_LSH			24
#define GPIO_ICR29_LSH			26
#define GPIO_ICR30_LSH			28
#define GPIO_ICR31_LSH			30

//------------------------------------------------------------------------------
// REGISTER FIELD 1THS
//------------------------------------------------------------------------------
#define GPIO_BIT0_WID			Width_1
#define GPIO_BIT1_WID			Width_1
#define GPIO_BIT2_WID			Width_1
#define GPIO_BIT3_WID			Width_1
#define GPIO_BIT4_WID			Width_1
#define GPIO_BIT5_WID			Width_1
#define GPIO_BIT6_WID			Width_1
#define GPIO_BIT7_WID			Width_1
#define GPIO_BIT8_WID			Width_1
#define GPIO_BIT9_WID			Width_1
#define GPIO_BIT10_WID			Width_1
#define GPIO_BIT11_WID			Width_1
#define GPIO_BIT12_WID			Width_1
#define GPIO_BIT13_WID			Width_1
#define GPIO_BIT14_WID			Width_1
#define GPIO_BIT15_WID			Width_1
#define GPIO_BIT16_WID			Width_1
#define GPIO_BIT17_WID			Width_1
#define GPIO_BIT18_WID			Width_1
#define GPIO_BIT19_WID			Width_1
#define GPIO_BIT20_WID			Width_1
#define GPIO_BIT21_WID			Width_1
#define GPIO_BIT22_WID			Width_1
#define GPIO_BIT23_WID			Width_1
#define GPIO_BIT24_WID			Width_1
#define GPIO_BIT25_WID			Width_1
#define GPIO_BIT26_WID			Width_1
#define GPIO_BIT27_WID			Width_1
#define GPIO_BIT28_WID			Width_1
#define GPIO_BIT29_WID			Width_1
#define GPIO_BIT30_WID			Width_1
#define GPIO_BIT31_WID			Width_1


#define GPIO_ICR0_WID			Width_2
#define GPIO_ICR1_WID			Width_2
#define GPIO_ICR2_WID			Width_2
#define GPIO_ICR3_WID			Width_2
#define GPIO_ICR4_WID			Width_2
#define GPIO_ICR5_WID			Width_2
#define GPIO_ICR6_WID			Width_2
#define GPIO_ICR7_WID			Width_2
#define GPIO_ICR8_WID			Width_2
#define GPIO_ICR9_WID			Width_2
#define GPIO_ICR10_WID			Width_2
#define GPIO_ICR11_WID			Width_2
#define GPIO_ICR12_WID			Width_2
#define GPIO_ICR13_WID			Width_2
#define GPIO_ICR14_WID			Width_2
#define GPIO_ICR15_WID			Width_2

#define GPIO_ICR16_WID			Width_2
#define GPIO_ICR17_WID			Width_2
#define GPIO_ICR18_WID			Width_2
#define GPIO_ICR19_WID			Width_2
#define GPIO_ICR20_WID			Width_2
#define GPIO_ICR21_WID			Width_2
#define GPIO_ICR22_WID			Width_2
#define GPIO_ICR23_WID			Width_2
#define GPIO_ICR24_WID			Width_2
#define GPIO_ICR25_WID			Width_2
#define GPIO_ICR26_WID			Width_2
#define GPIO_ICR27_WID			Width_2
#define GPIO_ICR28_WID			Width_2
#define GPIO_ICR29_WID			Width_2
#define GPIO_ICR30_WID			Width_2
#define GPIO_ICR31_WID			Width_2


//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------
#define GPIO_LOW             		0 // GPIO pin is set LOW
#define GPIO_HIGH              		1 // GPIO pin is set HIGH

#define GPIO_GDIR_INPUT             0 // GPIO pin is input
#define GPIO_GDIR_OUTPUT            1 // GPIO pin is output

#define LOW_LEVEL_INTERRUPT			0x0
#define HIGH_LEVEL_INTERRUPT		0x1
#define RISING_EDGE_INTERRUPT		0x2
#define FALLING_EDGE_INTERRUPT		0x3



#endif /*IMX25_GPIO_H_*/

