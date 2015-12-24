/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  Date:       3/25/2009
//  Tools:      RVCT Build 739
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Foundation_Temp/iMX25_Illumination_Control.h $
// $Revision: 1.7 $
// $Date: 2009/10/09 13:46:44EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// HEADER:  iMX25_Illumination_Control.h
//
// 
//------------------------------------------------------------------------------
#ifndef FLASH_LEDS
#ifndef IMX25_ILLUMINATION_CONTROL_H_
#define IMX25_ILLUMINATION_CONTROL_H_

#include 	"language.h"

#define ILL_OFF	0x0
#define ILL_ON	0x1

//AIMER Options see Product Config for defines
//#define AIMER_NONE 					0
//#define AIMER_GREEN_LED 				1
//#define AIMER_BRIGHT_GREEN_LED		2
//#define AIMER_PSOC_LASER 				3
//#define AIMER_LASER 					4
//#define AIMER_RED_LED 				5
//
////ILLUMINATION Options see Product Config for defines
//#define ILLUMINATION_NONE 			0
//#define ILLUMINATION_RED_LED 			1
//#define ILLUMINATION_RED_LIGHT_BAR 	2
//#define ILLUMINATION_WHITE 			3
//#define ILLUMINATION_GREEN 			4

//NOTE: in any state, ILLON and AIMON has overriding control of the LEDs
typedef enum 
{
	ILLON_AIMON_CONTROL = 0x0,		//Flash Out Does not control eather signal
	FLASHOUT_AIMER_CONTROL = 0x1,	//Only The Aimers follows the Flash Out signal
	FLASHOUT_ILLUM_CONTROL = 0x2,	//Only Illumination follows the Flash Out signal 
	FLASHOUT_CONTROL = 0x3			//Both the Aimer and Illumination Follow the Flash Out Signal
} ILL_CTL_LEDControlStates;

threadhandle_t GPIO3_Service_IRQ(void *);

//External API Functions:
// ===  Function  ======================================================================
// 
//         Name:  Ill_Ctl_Sys_Init_Thread
// 
//  Description: Perform illumination initialization sequence in the calling thread  
//					then creates and runs the Illuminatoin Thread	
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:  
//       out:  
//    return:					 	 
// -------------------------------------------------------------------------------------
void Ill_Ctl_Sys_Init_Thread(void);

// ===  Function  ======================================================================
// 
//         Name:  Ill_Ctl_Sys_Destroy
// 
//  Description: Kills the ILL_Ctl_Sys Thread and any events assoitated with it
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:  
//       out:  
//    return:					 	 
// -------------------------------------------------------------------------------------
void Ill_Ctl_Sys_Destroy(void);

// ===  Function  ======================================================================
// 
//         Name:  SetLedControlState
//				  GetLedControlState
//
//  Description: See Illumination Doc for explanation on the different Control States
//				 SetLedControlState - Sets the LED Control State, Enables the FLASH_OUT
//				  interrupt for modes that sync to the imager.  Triggers the Illumination
//				  event to change Illumination States if needed
//				 SetLedControlState - returns the value of LedControlState
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in: UINT	(SET)		LCState			
//    in-out:  
//       out:  
//    return: UINT	(GET)						value of present LED Control State LedControlState
// -------------------------------------------------------------------------------------
void SetLedControlState(UINT);
UINT GetLedControlState(void);

// ===  Function  ======================================================================
// 
//         Name:  SetAIM_ONtate
//				  GetAIM_ONState
// 
//  Description: AimerState takes the place of the AIM_ON Hardware Signal.
//				SetAIM_ONState - Changes the State of AimerState based on input parameter
//								  Then signals the Illumination Event to change the 
//								  Illumination State
//				GetAIM_ONState - Returns the value of AimerState 
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  bool  (SET)		AState			AimerState value to be set
//    in-out:  
//       out:  
//    return:  bool  (GET)						Value of AimerState				 	 
// -------------------------------------------------------------------------------------
void SetAIM_ONState(bool);
bool GetAIM_ONState(void);

// ===  Function  ======================================================================
// 
//         Name:  SetILL_ONState
//				  GetILL_ONState
// 
//  Description: IlluminationState takes the place of the ILL_ON Signal.
//				SetILL_ONState - Changes the State of IlluminationState based on 
//								  input parameter.  Then signals the Illumination Event  
//								  to change the Illumination State
//				GetILL_ONState - Returns the value of IlluminationState
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in: BOOL  (SET)		IState			IlluminationState value to be set
//    in-out:  
//       out:  
//    return: BOOL  (GET)						Returned value of IlluminationState					 	 
// -------------------------------------------------------------------------------------
void SetILL_ONState(bool);
bool GetILL_ONState(void);

// ===  Function  ======================================================================
// 
//         Name:  GetADCValue
// 
//  Description: Returns the last value the ADC measured, does not matter if it was 
//					Aimer or Illumination
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:  
//       out:  
//    return: UINT								returned value of ADC_MeasurementValue			 	 
// -------------------------------------------------------------------------------------
UINT GetADCValue(void);

// ===  Function  ======================================================================
// 
//         Name:  GetFlashOutState
// 
//  Description: Returns the current value of the FLASH_OUT HW Line
//				Imager Exposing = 1, Imager not Exposing = 0
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:  
//       out:  
//    return: BOOL								returned value of FlashOutState			 	 
// -------------------------------------------------------------------------------------
bool GetFlashOutState(void);

// ===  Function  ======================================================================
// 
//         Name:  SetAimerCurrent
//				  GetAimerCurrent
// 
//  Description: AimerADCSetting - Is the Current value the Aimer will be set to when 
//					running.  Changing this value will lower or raise the Current through
//					The Aimer.  When changing this value the Startup time of the Aimer will
//					increase.
//				SetAimerCurrent - Checks that the new current is not above the MAX then sets
//								AimerADCSetting
//				GetAimerCurrent - Returns the value of AimerADCSetting
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  UINT	 (SET)	  ACurrent			New ADC Setting for Aimer Current
//    in-out:  
//       out:  
//    return:  UINT  (GET)						Value of AimerADCSetting
// -------------------------------------------------------------------------------------
void SetAimerCurrent(UINT);
UINT GetAimerCurrent(void);

// ===  Function  ======================================================================
// 
//         Name:  SetIlluminationCurrent
//				  GetIlluminationCurrent
// 
//  Description: IlluminationADCSetting - ADC Setting of the Illumination Current the system
//					will run the LEDs at.
//				SetIlluminationCurrent - Checks the value to make sure its not above the MAX
//					then sets IlluminationADCSetting
//				GetIlluminationCurrent - returns the IlluminationADCSetting value
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  UINT	(SET)		ICurrent		New ADC Setting for Illumination Current
//    in-out:  
//       out:  
//    return:  UINT (GET)						Value of IlluminationADCSetting
// -------------------------------------------------------------------------------------
void SetIlluminationCurrent(UINT);
UINT GetIlluminationCurrent(void);

#endif //endof FLASH_LEDS
#endif /*IMX25_ILLUMINATION_CONTROL_H_*/

