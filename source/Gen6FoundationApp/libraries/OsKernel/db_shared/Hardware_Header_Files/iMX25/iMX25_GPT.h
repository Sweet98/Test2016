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
// $RCSfile: db_shared/Hardware_Header_Files/iMX25/iMX25_GPT.h $
// $Revision: 1.3 $
// $Date: 2011/06/28 13:26:49EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx25_gpt.h
//
// Provides definitions for the 6 general-purpose timers (GPT)
// Refer to Chapter 46 of the iMX25 Reference Manual
//
//------------------------------------------------------------------------------

#ifndef IMX25_GPT_H_
#define IMX25_GPT_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// REGISTERS
//------------------------------------------------------------------------------

// #########################################
// # GPT1                                  #
// #########################################
#define GPT1_BASE_ADDR             0x53F90000U
#define GPT1_GPTCR                (GPT1_BASE_ADDR+0x00U)     // GPT Control Register (GPTCR)
#define GPT1_GPTPR                (GPT1_BASE_ADDR+0x04U)     // GPT Prescaler Register (GPTPR)
#define GPT1_GPTSR                (GPT1_BASE_ADDR+0x08U)     // GPT Status Register (GPTSR)
#define GPT1_GPTIR                (GPT1_BASE_ADDR+0x0CU)     // GPT Interrupt Register (GPTIR)
#define GPT1_GPTOCR1              (GPT1_BASE_ADDR+0x10U)     // GPT Compare Register 1 (GPTOCR1)
#define GPT1_GPTOCR2              (GPT1_BASE_ADDR+0x14U)     // GPT Compare Register 2 (GPTOCR2)
#define GPT1_GPTOCR3              (GPT1_BASE_ADDR+0x18U)     // GPT Compare Register 3 (GPTOCR3)
#define GPT1_GPTICR1              (GPT1_BASE_ADDR+0x1CU)     // GPT Input capture Register 1 (GPTICR1)
#define GPT1_GPTICR2              (GPT1_BASE_ADDR+0x20U)     // GPT Input capture Register 2 (GPTICR2)
#define GPT1_GPTCNT               (GPT1_BASE_ADDR+0x24U)     // GPT Counter Register (GPTCNT)

// #########################################
// # GPT2                                  #
// #########################################
#define GPT2_BASE_ADDR             0x53F8C000U
#define GPT2_GPTCR                (GPT2_BASE_ADDR+0x00U)     
#define GPT2_GPTPR                (GPT2_BASE_ADDR+0x04U)     
#define GPT2_GPTSR                (GPT2_BASE_ADDR+0x08U)     
#define GPT2_GPTIR                (GPT2_BASE_ADDR+0x0CU)
#define GPT2_GPTOCR1              (GPT2_BASE_ADDR+0x10U)
#define GPT2_GPTOCR2              (GPT2_BASE_ADDR+0x14U)
#define GPT2_GPTOCR3              (GPT2_BASE_ADDR+0x18U)
#define GPT2_GPTICR1              (GPT2_BASE_ADDR+0x1CU)
#define GPT2_GPTICR2              (GPT2_BASE_ADDR+0x20U)
#define GPT2_GPTCNT               (GPT2_BASE_ADDR+0x24U)

// #########################################
// # GPT3                                  #
// #########################################
#define GPT3_BASE_ADDR             0x53F88000U
#define GPT3_GPTCR                (GPT3_BASE_ADDR+0x00U)     
#define GPT3_GPTPR                (GPT3_BASE_ADDR+0x04U)     
#define GPT3_GPTSR                (GPT3_BASE_ADDR+0x08U)     
#define GPT3_GPTIR                (GPT3_BASE_ADDR+0x0CU)     
#define GPT3_GPTOCR1              (GPT3_BASE_ADDR+0x10U)     
#define GPT3_GPTOCR2              (GPT3_BASE_ADDR+0x14U)     
#define GPT3_GPTOCR3              (GPT3_BASE_ADDR+0x18U)     
#define GPT3_GPTICR1              (GPT3_BASE_ADDR+0x1CU)     
#define GPT3_GPTICR2              (GPT3_BASE_ADDR+0x20U)     
#define GPT3_GPTCNT               (GPT3_BASE_ADDR+0x24U)

// #########################################
// # GPT4                                  #
// #########################################
#define GPT4_BASE_ADDR             0x53F84000U
#define GPT4_GPTCR                (GPT4_BASE_ADDR+0x00U)     
#define GPT4_GPTPR                (GPT4_BASE_ADDR+0x04U)     
#define GPT4_GPTSR                (GPT4_BASE_ADDR+0x08U)     
#define GPT4_GPTIR                (GPT4_BASE_ADDR+0x0CU)     
#define GPT4_GPTOCR1              (GPT4_BASE_ADDR+0x10U)     
#define GPT4_GPTOCR2              (GPT4_BASE_ADDR+0x14U)     
#define GPT4_GPTOCR3              (GPT4_BASE_ADDR+0x18U)     
#define GPT4_GPTICR1              (GPT4_BASE_ADDR+0x1CU)     
#define GPT4_GPTICR2              (GPT4_BASE_ADDR+0x20U)     
#define GPT4_GPTCNT               (GPT4_BASE_ADDR+0x24U) 



//------------------------------------------------------------------------------
// REGISTER BIT FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
// GPT Control Register (GPTCR)
#define GPT_GPTCTL_EN_LSH        	0
#define GPT_GPTCTL_ENMOD_LSH     	1
#define GPT_GPTCTL_DBGEN_LSH     	2
#define GPT_GPTCTL_WAITEN_LSH    	3
#define GPT_GPTCTL_STOPEN_LSH    	5
#define GPT_GPTCTL_CLKSOURCE_LSH 	6
#define GPT_GPTCTL_FRR_LSH       	9
#define GPT_GPTCTL_SWR_LSH       	15
#define GPT_GPTCTL_IM1_LSH       	16
#define GPT_GPTCTL_IM2_LSH      	18
#define GPT_GPTCTL_OM1_LSH      	20
#define GPT_GPTCTL_OM2_LSH      	23
#define GPT_GPTCTL_OM3_LSH      	26
#define GPT_GPTCTL_FO1_LSH      	29
#define GPT_GPTCTL_FO2_LSH      	30
#define GPT_GPTCTL_FO3_LSH      	31

// GPT Prescaler Register (GPTPR)
#define GPT_GPTPR_PRESCALER_LSH 	0

// GPT Status Register (GPTSR)
#define GPT_GPTSR_OF1_LSH 			0
#define GPT_GPTSR_OF2_LSH 			1
#define GPT_GPTSR_OF3_LSH 			2
#define GPT_GPTSR_IF1_LSH 			3
#define GPT_GPTSR_IF2_LSH 			4
#define GPT_GPTSR_ROV_LSH 			5

// GPT Interrupt Register (GPTIR)
#define GPT_GPTIR_OF1IE_LSH			0
#define GPT_GPTIR_OF2IE_LSH			1
#define GPT_GPTIR_OF3IE_LSH			2
#define GPT_GPTIR_IF1IE_LSH			3
#define GPT_GPTIR_IF2IE_LSH			4
#define GPT_GPTIR_ROVIE_LSH			5

// GPT Compare Register 1 (GPTOCR1)
#define GPT_GPTOCR1_COMP_LSH    	0

// GPT Compare Register 2 (GPTOCR2)
#define GPT_GPTOCR2_COMP_LSH    	0

// GPT Compare Register 3 (GPTOCR3)
#define GPT_GPTOCR3_COMP_LSH    	0

// GPT Input capture Register 1 (GPTICR1)
#define GPT_GPTICR1_CAPT_LSH    	0

// GPT Input capture Register 2 (GPTICR2)
#define GPT_GPTICR2_CAPT_LSH    	0

// GPT Counter Register (GPTCNT)
#define GPT_GPTCNT_COUNT_LSH        0



//------------------------------------------------------------------------------
// REGISTER BIT FIELD WIDTHS
//------------------------------------------------------------------------------
// GPT Control Register (GPTCR)
#define GPT_GPTCTL_EN_WID        	Width_1
#define GPT_GPTCTL_ENMOD_WID     	Width_1
#define GPT_GPTCTL_DBGEN_WID     	Width_1
#define GPT_GPTCTL_WAITEN_WID    	Width_1
#define GPT_GPTCTL_STOPEN_WID    	Width_1
#define GPT_GPTCTL_CLKSOURCE_WID 	Width_3
#define GPT_GPTCTL_FRR_WID       	Width_1
#define GPT_GPTCTL_SWR_WID       	Width_1
#define GPT_GPTCTL_IM1_WID       	Width_2
#define GPT_GPTCTL_IM2_WID      	Width_2
#define GPT_GPTCTL_OM1_WID      	Width_3
#define GPT_GPTCTL_OM2_WID      	Width_3
#define GPT_GPTCTL_OM3_WID      	Width_3
#define GPT_GPTCTL_FO1_WID      	Width_1
#define GPT_GPTCTL_FO2_WID      	Width_1
#define GPT_GPTCTL_FO3_WID      	Width_1

// GPT Prescaler Register (GPTPR)
#define GPT_GPTPR_PRESCALER_WID 	Width_12

// GPT Status Register (GPTSR)
#define GPT_GPTSR_OF1_WID 			Width_1
#define GPT_GPTSR_OF2_WID 			Width_1
#define GPT_GPTSR_OF3_WID 			Width_1
#define GPT_GPTSR_IF1_WID 			Width_1
#define GPT_GPTSR_IF2_WID 			Width_1
#define GPT_GPTSR_ROV_WID 			Width_1

// GPT Interrupt Register (GPTIR)
#define GPT_GPTIR_OF1IE_WID			Width_1
#define GPT_GPTIR_OF2IE_WID			Width_1
#define GPT_GPTIR_OF3IE_WID			Width_1
#define GPT_GPTIR_IF1IE_WID			Width_1
#define GPT_GPTIR_IF2IE_WID			Width_1
#define GPT_GPTIR_ROVIE_WID			Width_1

// GPT Compare Register 1 (GPTOCR1)
#define GPT_GPTOCR1_COMP_WID    	Width_32

// GPT Compare Register 2 (GPTOCR2)
#define GPT_GPTOCR2_COMP_WID    	Width_32

// GPT Compare Register 3 (GPTOCR3)
#define GPT_GPTOCR3_COMP_WID    	Width_32

// GPT Input capture Register 1 (GPTICR1)
#define GPT_GPTICR1_CAPT_WID    	Width_32

// GPT Input capture Register 2 (GPTICR2)
#define GPT_GPTICR2_CAPT_WID    	Width_32

// GPT Counter Register (GPTCNT)
#define GPT_GPTCNT_COUNT_WID    	Width_32


//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------

// GPT Control Register (GPTCR)
#define GPT_GPTCTL_EN_ENABLE              1 // GPT enabled
#define GPT_GPTCTL_EN_DISABLE             0 // GPT disabled

#define GPT_GPTCTL_ENMOD_ENABLE           1 //GPT counter retain its value when it is disabled
#define GPT_GPTCTL_ENMOD_DISABLE          0 //GPT counter value is reset to 0 when it is disabled

#define GPT_GPTCTL_DBGEN_ENABLE           1 //GPT is enabled in debug mode
#define GPT_GPTCTL_DBGEN_DISABLE          0 //GPT is disabled in debug mode

#define GPT_GPTCTL_WAITEN_ENABLE          1 //GPT is enabled in wait mode
#define GPT_GPTCTL_WAITEN_DISABLE         0 //GPT is disabled in wait mode

#define GPT_GPTCTL_STOPEN_ENABLE          1 //GPT is enabled in stop mode
#define GPT_GPTCTL_STOPEN_DISABLE         0 //GPT is disabled in stop mode

#define GPT_GPTCTL_CLKSOURCE_DISABLE      0 // clock disabled
#define GPT_GPTCTL_CLKSOURCE_PERCLK1      1 // ipg_clk
#define GPT_GPTCTL_CLKSOURCE_PERCLK1DIV4  2 // ipg_clk_highfreq
#define GPT_GPTCTL_CLKSOURCE_TIN          3 // ipp_ind_clkin (external clock from pad)
#define GPT_GPTCTL_CLKSOURCE_32KCLK       7 // ipg_clk_32k

#define GPT_GPTCTL_FRR_FREERUN            1 // Freerun mode (counter continues after compare)
#define GPT_GPTCTL_FRR_RESTART            0 // Restart mode (counter set to zero after compare)

#define GPT_GPTCTL_SWR_RESET              1 // Self-clearing software reset
#define GPT_GPTCTL_SWR_NORESET            0 // Do not activate software reset

#define GPT_GPTCTL_IM1_DISABLE            0 // Capture function disabled
#define GPT_GPTCTL_IM1_RISING             1 // Capture on rising edge
#define GPT_GPTCTL_IM1_FALLING            2 // Capture on falling edge
#define GPT_GPTCTL_IM1_BOTH               3 // Capture on both edges

#define GPT_GPTCTL_IM2_DISABLE            0 // Capture function disabled
#define GPT_GPTCTL_IM2_RISING             1 // Capture on rising edge
#define GPT_GPTCTL_IM2_FALLING            2 // Capture on falling edge
#define GPT_GPTCTL_IM2_BOTH               3 // Capture on both edges

#define GPT_GPTCTL_OM1_DISABLE            0 // 000 Output disconnected. No response on pin
#define GPT_GPTCTL_OM1_TOGGLE             1 // 001 Toggle output pin
#define GPT_GPTCTL_OM1_CLEAR              2 // 010 Clear output pin
#define GPT_GPTCTL_OM1_SET                3 // 011 Set output pin
#define GPT_GPTCTL_OM1_PULSE              4 // 1xx Generate a active low pulse of one input clock wide on output

#define GPT_GPTCTL_OM2_DISABLE            0 // 000 Output disconnected. No response on pin
#define GPT_GPTCTL_OM2_TOGGLE             1 // 001 Toggle output pin
#define GPT_GPTCTL_OM2_CLEAR              2 // 010 Clear output pin
#define GPT_GPTCTL_OM2_SET                3 // 011 Set output pin
#define GPT_GPTCTL_OM2_PULSE              4 // 1xx Generate a active low pulse of one input clock wide on output

#define GPT_GPTCTL_OM3_DISABLE            0 // 000 Output disconnected. No response on pin
#define GPT_GPTCTL_OM3_TOGGLE             1 // 001 Toggle output pin
#define GPT_GPTCTL_OM3_CLEAR              2 // 010 Clear output pin
#define GPT_GPTCTL_OM3_SET                3 // 011 Set output pin
#define GPT_GPTCTL_OM3_PULSE              4 // 1xx Generate a active low pulse of one input clock wide on output

#define GPT_GPTCTL_FO1_COMPARE            1 // Causes pin action on timer output compare 2 pin, OF2 flag is not set, Writing a 0 has no effect
#define GPT_GPTCTL_FO2_COMPARE            1 // Causes pin action on timer output compare 2 pin, OF2 flag is not set, Writing a 0 has no effect
#define GPT_GPTCTL_FO3_COMPARE            1 // Causes pin action on timer output compare 2 pin, OF2 flag is not set, Writing a 0 has no effect

// GPT Status Register (GPTSR)
#define GPT_GPTSR_OF1_OCCURRED			  1 //Compare event occurred
#define GPT_GPTSR_OF1_NOTOCCURRED		  0 //Compare event not occurred

#define GPT_GPTSR_OF2_OCCURRED			  1 //Compare event occurred
#define GPT_GPTSR_OF2_NOTOCCURRED		  0 //Compare event not occurred

#define GPT_GPTSR_OF3_OCCURRED			  1 //Compare event occurred
#define GPT_GPTSR_OF3_NOTOCCURRED		  0 //Compare event not occurred

#define GPT_GPTSR_IF1_OCCURRED			  1 //Capture event occurred
#define GPT_GPTSR_IF1_NOTOCCURRED		  0 //Capture event not occurred

#define GPT_GPTSR_IF2_OCCURRED			  1 //Capture event occurred
#define GPT_GPTSR_IF2_NOTOCCURRED		  0 //Capture event not occurred

#define GPT_GPTSR_ROV_OCCURRED			  1 //Rollover event occurred
#define GPT_GPTSR_ROV_NOTOCCURRED		  0 //Rollover event not occurred

// GPT Interrupt Register (GPTIR)
#define GPT_GPTIR_OF1IE_ENABLE            1 //Interrupt enabled
#define GPT_GPTIR_OF1IE_DISABLE           0 //Interrupt disabled

#define GPT_GPTIR_OF2IE_ENABLE            1 //Interrupt enabled
#define GPT_GPTIR_OF2IE_DISABLE           0 //Interrupt disabled

#define GPT_GPTIR_OF3IE_ENABLE         	  1 //Interrupt enabled
#define GPT_GPTIR_OF3IE_DISABLE           0 //Interrupt disabled

#define GPT_GPTIR_IF1IE_ENABLE            1 //Interrupt enabled
#define GPT_GPTIR_IF1IE_DISABLE           0 //Interrupt disabled

#define GPT_GPTIR_IF2IE_ENABLE            1 //Interrupt enabled
#define GPT_GPTIR_IF2IE_DISABLE           0 //Interrupt disabled

#define GPT_GPTIR_ROVIE_ENABLE            1 //Interrupt enabled
#define GPT_GPTIR_ROVIE_DISABLE           0 //Interrupt disabled

#endif /*IMX25_GPT_H_*/







