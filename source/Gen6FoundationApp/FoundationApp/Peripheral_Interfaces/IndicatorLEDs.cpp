/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  Date:       7/22/2009
//  Tools:      RVCT Build 739
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Peripheral_Interfaces/IndicatorLEDs.cpp $
// $Revision: 1.17 $
// $Date: 2010/11/23 02:46:45EST $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// File:  IndicatorLEDs.cpp
//
//		File Contains the functions used to control the Good Read and Error LEDs.
//
//------------------------------------------------------------------------------
#include "IndicatorLEDs.h"

#if PLATFORM_CURRENT == imx25
#include "HardwareMacros.h"
#include "iMX25_GPIO.h"	//GPIO Defines
#include "iMX25_IOMUX.h"	//IO MUX Defines
#endif

#include	"ReaderConfiguration.h"
#include "menu.h"
#include "HardwareToken.h"
#include "PluginGpioInterface.h"

#if defined(SM4100)
	#define RED_LED(state)
#else	
	#define RED_LED(state)		 do{WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT18_LSH,GPIO_BIT18_WID,state);}while(0)
#endif

static void Green_Led(bool);

//Flags to indicate the operational state of the LEDs.  
//This allows the beeper code to interlace the PWM and 
//LED function.  This is done to keep the scanner current 
//under 500mA
static bool GoodReadLEDState = 0;
static bool ErrorLEDState = 0;
static bool KeepLightsOff = 0;

// ===  Function  ======================================================================
//
//  Name:  Init_IndicatorLEDs
//
//  Description: Set-up of the GPIO pins to control both sets of LEDs
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void Init_IndicatorLEDs(void)
{
	DISABLE_INTERRUPTS();
	
#if PLATFORM_CURRENT == imx25

	//Configure the GREEN_LED -GPIO3_17
	HAZARD_WRITE_FIELD_TO_REG(IOMUXC_SW_MUX_CTL_PAD_VSTBY_REQ,IOMUXC_SW_MUX_CTL_MUX_MODE_LSH,IOMUXC_SW_MUX_CTL_MUX_MODE_WID,MUX_MODE_5);	//MUX To GPIO
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_VSTBY_REQ,0x00002007);	//PAD Regiser Drive Strength
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,GPIO_GDIR_OUTPUT);	//Direction Register

	//Configure the RED_LED -GPIO3_18
	HAZARD_WRITE_FIELD_TO_REG(IOMUXC_SW_MUX_CTL_PAD_VSTBY_ACK,IOMUXC_SW_MUX_CTL_MUX_MODE_LSH,IOMUXC_SW_MUX_CTL_MUX_MODE_WID,MUX_MODE_5);	//MUX To GPIO
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_VSTBY_ACK,0x00002007);	//PAD Regiser Drive Strength

	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT18_LSH,GPIO_BIT18_WID,GPIO_GDIR_OUTPUT);	//Direction Register

#endif //endof PLATFORM_CURRENT == imx25

	RESTORE_INTERRUPTS();
	
} //endof Init_IndicatorLEDs

// ===  Function  ======================================================================
//
//  Name:  GoodReadLedOff
//
//  Description: Turns the Good Read LED Off
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void GoodReadLedOff(void)
{
	GoodReadLEDState = 0;
	Green_Led(0);
}	//endof GoodReadLedOff

// ===  Function  ======================================================================
//
//  Name:  ErrorLedOff
//
//  Description: Turns the Error LED Off
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void ErrorLedOff(void)
{
	ErrorLEDState = 0;
	if(!(HardwareToken.IAmOEM()))
		RED_LED(0);
	
}	//endof ErrorLedOff

// ===  Function  ======================================================================
//
//  Name:  GoodReadLedOn
//
//  Description: Turns the Good Read LED on
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void GoodReadLedOn(void)
{
	GoodReadLEDState = 1;
	if(!KeepLightsOff)
	{
		Green_Led(1);
	}
}	//endof GoodReadLedOn

// ===  Function  ======================================================================
//
//  Name:  ErrorLedOn
//
//  Description: Turns the Error LED on
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void ErrorLedOn(void)
{
	ErrorLEDState = 1;
	if(!KeepLightsOff)
	{
		if(!(HardwareToken.IAmOEM()))
			RED_LED(1);

	}
}	//endof ErrorLedOn

// ===  Function  ======================================================================
//
//  Name:  IsErrorLedOn
//
//  Description: Checks the ErrorLEDState flag to see if the beeper can turn on the Error LED 
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
bool IsErrorLedOn(void)
{
	return ErrorLEDState;
}	//endof ErrorLedOn

// ===  Function  ======================================================================
//
//  Name:  IsGoodReadOn
//
//  Description: Checks the GoodReadLEDState flag to see if the beeper can turn on the 
//			Good Read LED
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
bool IsGoodReadOn(void)
{
	return GoodReadLEDState;
}	//endof GoodReadOn

// ===  Function  ======================================================================
//
//  Name:  Force_Green_LED
//
//  Description:  DO NOT USE THIS FUNCTION
//		This allows synchronization between the indicator LEDs and the Beeper
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void Force_Green_LED(bool GRstate)
{
	Green_Led(GRstate);
}	//endof Force_Green_LED

// ===  Function  ======================================================================
//
//  Name:  Force_Red_LED
//
//  Description: DO NOT USE THIS FUNCTION
//		This allows synchronization between the indicator LEDs and the Beeper
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void Force_Red_LED(bool Rstate)
{
	if(!(HardwareToken.IAmOEM()))
		RED_LED(Rstate);
}	//endof Force_Red_LED

// ===  Function  ======================================================================
//
//  Name:  BeeperLockLEDS
//
//  Description:  DO NOT USE THIS FUNCTION
//		This allows the Beeper to take control of the LEDS and interlace it with the beep
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void BeeperLockLEDS(void)
{
	KeepLightsOff = 1;
}	//endof BeeperLockLEDS

// ===  Function  ======================================================================
//
//  Name:  BeeperUnLockLEDS
//
//  Description:   DO NOT USE THIS FUNCTION
//		This allows the Beeper to release control of the LEDS
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void BeeperUnLockLEDS(void)
{
	KeepLightsOff = 0;
}	//endof Force_Red_LED

// ===  Function  ======================================================================
//
//  Name:  Green_Led
//
//  Description:   Hardware Control of the Green LED
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in: bool 				LineState		State of the Green LED Line 1 = ON, 0 = OFF	
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
static void Green_Led(bool state)
{
#if defined(SM4100)	// TODO: need test
		WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,~state);
#else
		WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,state);
#endif
}	//endof Force_Red_LED


