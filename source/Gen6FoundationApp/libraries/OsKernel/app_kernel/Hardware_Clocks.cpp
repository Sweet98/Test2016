/**************************************************************************************/
//
// HIM 48X0G GENERATION 6 SOFTWARE
//
//  Date:       10/31/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: app_kernel/Hardware_Clocks.cpp $
// $Revision: 1.4 $
// $Date: 2009/04/29 09:00:49EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// File:  Hardware_Clocks.cpp
//
//  This module contains a collection of low level functions that control the
//		speed of the clocks, for power management.
//
//------------------------------------------------------------------------------
#if PLATFORM_CURRENT == imx25
#include "HardwareMacros.h"		//Provides Macros for setting registers
#include "iMX25_CRM.h"			//Clock and Reset control
#else
#error VALID PLATFORM NOT DEFINED
#endif

#include "Hardware_Clocks.h"
#include "stdinclude.h"

// ===  Function  ======================================================================
//
//         Name:  Initialize_Clocks()
//
//  Description:  Initializes the clocks for app.
//					ARM				= 399,000,000
//					USB				= 240,000,000
//					AHB				= 133,000,000
//					IPG	 			= 66,500,000
//					PER0  CSI		= 66,500,000
//					PER1  EPIT		= 66,500,000
//					PER2  ESAI		= Disabled
//					PER3  ESDHC1	= Disabled
//					PER4  ESDHC2	= Disabled
//					PER5  GPT		= 66,500,000
//					PER6  I2C		= 66,500,000
//					PER7  LCDC		= Disabled
//					PER8  NFC		= 66,500,000
//					PER9  OWIRE		= Disabled
//					PER10 PWM		= 66,500,000
//					PER11 SIM1		= Disabled
//					PER12 SIM2		= Disabled
//					PER13 SSI1		= Disabled
//					PER14 SSI2		= Disabled
//					PER15 UART		= 33,250,000
//					CLK OUT			= Disabled
//	Settings were generated from Excel Doc "ds018_iMX25_Clock_Calculator.xls"
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void Initialize_Clocks(void)
{
#if PLATFORM_CURRENT == imx25
	HAZARD_WRITE_REG(CRM_MPCTL,CRM_MPCTL_SETTING);									//Load the MCU PLL with the correct frequency
	HAZARD_WRITE_FIELD_TO_REG(CRM_CCTL,CRM_CCTL_MPLLRST_LSH,CRM_CCTL_MPLLRST_WID,0x1);  //RESET the PLL
	while(READ_FIELD(CRM_MPCTL,CRM_MPCTL_LOCK_LSH,CRM_MPCTL_LOCK_WID) == 0)
	{
		//Wait for the MCU PLL RESETS
	}

	HAZARD_WRITE_REG(CRM_UPCTL,CRM_UPCTL_SETTING);									//Load the USB PLL with the correct frequency
	HAZARD_WRITE_FIELD_TO_REG(CRM_CCTL,CRM_CCTL_MPLLRST_LSH,CRM_CCTL_MPLLRST_WID,0x1);  //RESET the PLL
	while(READ_FIELD(CRM_UPCTL,CRM_UPCTL_LOCK_LSH,CRM_UPCTL_LOCK_WID) == 0)
	{
		//Wait for the USB PLL RESETS
	}

	HAZARD_WRITE_REG(CRM_CCTL,CRM_CCTL_SETTING);
	HAZARD_WRITE_REG(CRM_CGR0,CRM_CGR0_SETTING);
	HAZARD_WRITE_REG(CRM_CGR1,CRM_CGR1_SETTING);
	HAZARD_WRITE_REG(CRM_CGR2,CRM_CGR2_SETTING);
	HAZARD_WRITE_REG(CRM_PDR0,CRM_PDR0_SETTING);
	HAZARD_WRITE_REG(CRM_PDR1,CRM_PDR1_SETTING);
	HAZARD_WRITE_REG(CRM_PDR2,CRM_PDR2_SETTING);
	HAZARD_WRITE_REG(CRM_PDR3,CRM_PDR3_SETTING);
	HAZARD_WRITE_REG(CRM_MCR,CRM_MCR_SETTING);

#endif //ETM

}//Initialize_Clocks



#define ARM_CLK_DIV_1		1
#define ARM_SCLK_399			399

/* THis function will put us at our normal operating values for our clocks.
This should not be called unit after USB has been enumarated. */
void crank_up_clocks(int MaxMCUSpeed,int BClckDivisor)
{
#if PLATFORM_CURRENT == imx25

if( MaxMCUSpeed == ARM_SCLK_399 ){};

if( BClckDivisor == ARM_CLK_DIV_1 ){};


#endif //PLATFORM_CURRENT == mxl
}


