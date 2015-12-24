/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  Date:       9/24/2009
//  Tools:      RVCT Build 836
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Peripheral_Interfaces/Flash_Illumination_Control.h $
// $Revision: 1.14 $
// $Date: 2011/07/18 16:31:49EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// HEADER:  iMX25_Flash_Illumination_Control.h
//
//
//------------------------------------------------------------------------------
#ifndef IMX25_FLASH_ILLUMINATION_CONTROL_H_
#define IMX25_FLASH_ILLUMINATION_CONTROL_H_

#ifdef FLASH_LEDS

#include 	"language.h"

#define _SMART_SCAN_DRIVER_FLASH_INTEN_CNTRL	1				//un-comment out for scan driver intensity control		lmc

#define ILL_OFF	0x0
#define ILL_ON	0x1
#define FUNC_BEEPER 1
#define FUNC_ILLUM  0

typedef enum
{
	ILLON_AIMON_CONTROL = 0x0,			//Flash Out Does not control eather signal
	FLASHOUT_AIMER_CONTROL = 0x1,		//Only The Aimers follows the Flash Out signal
	FLASHOUT_ILLUM_CONTROL = 0x2,		//Only Illumination follows the Flash Out signal
	FLASHOUT_CONTROL = 0x3,				//Both the Aimer and Illumination Follow the Flash Out Signal
	ILLUM_TIMER_CONTROL = 0x4,			//Flash Out signals the timer to start, with no Aimer
	ILLUM_AIMER_TIMER_CONTROL = 0x5,	//Flash Out signals the timer to start, Aimer on when Flashout and Illumination are not on
} ILL_CTL_LEDControlStates;

//External API Functions:
// ===  Function  ======================================================================
//
//         Name:  Ill_Ctl_Sys_Init_Thread
//
//  Description: Perform illumination initialization sequence in the calling thread
//					then creates and runs the Illumination Thread
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
//				  event to change Illumination States if needed, Controls Flash Mode which
//				  sets the Current Level.
//				 SetLedControlState - returns the value of LedControlState
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in: UINT	(SET)		LCState			New LED Control State value:
//												ILLON_AIMON_CONTROL = 0x0,		//Flash Out Does not control eather signal
//												FLASHOUT_AIMER_CONTROL = 0x1,	//Only The Aimers follows the Flash Out signal
//												FLASHOUT_ILLUM_CONTROL = 0x2,	//Only Illumination follows the Flash Out signal
//												FLASHOUT_CONTROL = 0x3			//Both the Aimer and Illumination Follow the Flash Out Signal
//    in-out:
//       out:
//    return: UINT	(GET)						value of present LED Control State LedControlState
// -------------------------------------------------------------------------------------
void SetLedControlState(UINT);
UINT GetLedControlState(void);

// ===  Function  ======================================================================
//
//         Name:  SetILL_FlashMode
//				  GetILL_FlashMode
//
//  Description: Changes FlashControlState flag which changes the LED current mode
//					When Set High the Current Controller will change to High Current Mode
//					When set Low the Current Controller will change to Low Current Mode
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in: BOOL  (SET)		FlashControlBit		FlashControlState value to be set:
//													0x1 == High Current
//													0x0 == Low Current
//			return: BOOL  (SET)						Returned true if state has been changed, false if unchanged
//
//    in-out:
//       out:
//    return: BOOL  (GET)						Returned value of FlashControlState
// -------------------------------------------------------------------------------------
bool SetILL_FlashMode(UINT);
UINT GetILL_FlashMode(void);

// ===  Function  ======================================================================
//
//         Name:  SetAIM_ONState
//				  GetAIM_ONState
//
//  Description: AimerState takes the place of the AIM_ON Signal.
//				SetAIM_ONState - Changes the State of AimerState based on input parameter
//								  Then signals the Illumination Event to change the
//								  Illumination State
//				GetAIM_ONState - Returns the value of AimerState
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  bool  (SET)		AState			AimerState value to be set
//													0x1 == AimerState ON
//													0x0 == AimerState OFF
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
//				SetIlluminationState - Changes the State of IlluminationState based on
//								  input parameter.  Then signals the Illumination Event
//								  to change the Illumination State, this is not supported
//								  when in High Current Mode, and will not turn the LEDs ON
//				GetIlluminationState - Returns the value of IlluminationState
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in: BOOL  (SET)		IState			IlluminationState value to be set
//													0x1 == IlluminationState ON
//													0x0 == IlluminationState OFF
//    in-out:
//       out:
//    return: BOOL  (GET)						Returned value of IlluminationState
// -------------------------------------------------------------------------------------
void SetILL_ONState(bool);
bool GetILL_ONState(void);

// ===  Function  ======================================================================
//
//         Name:  IllTestMode
//
//  Description: Illumination Test Mode.  Called from TSTLON/TSTAON to make sure other 
//		functions cannot change the test mode.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in: BOOL				TestMode		Debug State, 1 = Debug ON, 0 = Debug OFF
//			  BOOL				AimIllFunction  Enable For Aimer (0x0) or Illumination (0x1)
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void IllTestMode(bool,bool);

// ===  Function  ======================================================================
//
//         Name:  GetFlashOutState
//
//  Description: Returns the current value of the FLASH_OUT HW Line (FLO)
//				The Flash Out Line indicates when the imager is Exposing,
//				 It can be programmed to have an advanced notice of exposure,
//				  of up to 2mS, we default to 1mS, For Flash LEDs its set a zero.
//				Imager Exposing (or in pre exposure) = 1,
//				Imager not Exposing = 0
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return: BOOL								returned value of FlashOutState
//													0x1 == FlashOutState ON
//													0x0 == FlashOutState OFF
// -------------------------------------------------------------------------------------
bool GetFlashOutState(void);

// ===  Function  ======================================================================
//
//         Name:  Enable_Boost
//				  Disable_Boost
//
//  Description: Turns on and off the Boost Circuit and there for the Illumination System
//				 This is needed for the Low Power Mode: STOP.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void Enable_Boost(void);
void Disable_Boost(void);

// ===  Function  ======================================================================
//
//         Name:  IsLightCurrentStateChangedToHigh
//
//  Description: Allows other functions to wait on the Illumination System to change
//					current states to boost high.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   	Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:		bool										True if in high boost mode, false other wise
// -------------------------------------------------------------------------------------
bool IsLightCurrentStateChangedToHigh(void);

// ===  Function  ======================================================================
//
//  Name:  Ill_HW_Disable_Aimer
//
//  Description: Forces the Aimer off.  The Aimer cannot be on while the beeper is beeping.
//			This is done to keep the Scanner Current under 500mA.  This Function turns the
//			Aimer off and blocks the State from turning it on with the AllowAimer Flag.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void Ill_HW_Disable_Aimer(bool);

// ===  Function  ======================================================================
//
//  Name:  Ill_HW_Enable_Aimer
//
//  Description:  Allows the Aimer to be turned back on.  Removes the Aimer
//			block (AllowAimer Flag).  Tricks the state machine into thinking
//			the Lights are off.  They actually are off but the driver does not
//			know that.  The thread is allowed to run again to turn the Aimer on
//			if it needs to be.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void Ill_HW_Enable_Aimer(bool);

// ===  Function  ======================================================================
//
//         Name:  Ill_Ctl_Sys_LED_Cir_EN_CTRL
//
//  Description:  This Function determines the state of the LED Circuit EN Pin based on 
//		whether the beeper is beeping and there is a state change.  When the illumination 
//		system changes states the boost enable is interlaced with the beeper.  This keeps 
//		the USB current under 500mA.  The interlace enable will only occur when the 
//		illumination system is changing current states and the beeper is beeping.
//
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	bool			Function		Function calling the Ill_Ctl_Sys_LED_Cir_EN_CTRL function.  
//													FUNC_ILLUM or FUNC_BEEPER	
//				bool			EN_State		State the calling function intends the LED Circuit EN pin to be.
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void Ill_Ctl_Sys_LED_Cir_EN_CTRL (bool, bool);

//Ken@2012.02.21 create the illumination subsystem.
//Scandriver's light control still use the old interface.
//the subsystem only used for triggered on and trigger off light control.
//NOTICE: Must match 
//#define AIM_OFF 	0
//#define AIM_BOTH	1
//#define AIM_ALT		2
//#define AIM_CONTINUOUS	3
typedef enum {
	HI2D_AIM_OFF,//flash on, aimer off
	HI2D_AIM_BOTH,//flash on, aimer on
	HI2D_AIM_ALT,//flash on,aimer on
	HI2D_AIM_CONTINUOUS,//flash off, aimer on
}ScanAimerIllumMode_t;

#if defined(HI2D_T)
#define ILLUM_LED_LEVL0			0
#define ILLUM_LED_LEVL1			1//150 -->7
#define ILLUM_LED_LEVL2         2//	50-->15
#define ILLUM_LED_LEVL3			3//15-->50
#define ILLUM_LED_LEVL4			4//7-->150
#else
#define ILLUM_LED_LEVL0			0
#define ILLUM_LED_LEVL1			7
#define ILLUM_LED_LEVL2         15
#define ILLUM_LED_LEVL3			50
#define ILLUM_LED_LEVL4			150
#endif

typedef enum {
	HI2D_LED_CURRENT_LEVL1,//flash on, aimer off
	HI2D_LED_CURRENT_LEVL2,//flash on, aimer on
	HI2D_LED_CURRENT_LEVL3,//flash on,aimer on
	HI2D_LED_CURRENT_LEVL4,//flash off, aimer on
}ScanIllumLevel_t;
typedef enum {
	HI2D_GPIO,//use the gpio to open and close the LED
	HI2D_PWM,//use the PWM to open and close the LED
}LedCtrlMode_t;
//define the flash led control component
typedef struct Led_ctrl_component_t
{
	void (* turnon)(ScanAimerIllumMode_t, bool/*, LedCtrlMode_t*/);//NOTICE the LedCtrlMode_t need user to set. TODO remove the parameter
	void (* turnoff)(ScanAimerIllumMode_t, bool/*, LedCtrlMode_t*/);//NOTICE the LedCtrlMode_t need user to set. TODO remove the parameter
	unsigned int reserved0;
	unsigned int reserved1;
}Led_ctrl_component;

//define the aimer control component
typedef struct Aimer_ctrl_component_t
{
	void (* turnon)(ScanAimerIllumMode_t);
	void (* turnoff)(ScanAimerIllumMode_t);
	unsigned int reserved0;
	unsigned int reserved1;
}Aimer_ctrl_component;

//define the current control component
typedef struct Current_ctrl_component_t
{
	void (* enter_high)(void);
	void (* enter_low)(void);
	void (* enterHi2dCurrentLevel)(unsigned int);
	unsigned int reserved0;
	unsigned int reserved1;
}Current_ctrl_component;

//define the illumination control subsystem
typedef struct HI2D_illumination_control_t
{
	Led_ctrl_component HI2D_flash;
	Aimer_ctrl_component HI2D_aimer;
	Current_ctrl_component HI2D_current;
	ScanAimerIllumMode_t scanAimerIllumMode;
	LedCtrlMode_t HI2D_LedCtrlMode;
	unsigned int reserved0;
	unsigned int reserved1;
}HI2D_illumination_control;
//Get the HI2D illumination control handle
void *get_HI2D_illum_crl( void );
#endif //endof FLASH_LEDS

#endif /*IMX25_FLASH_ILLUMINATION_CONTROL_H_*/

