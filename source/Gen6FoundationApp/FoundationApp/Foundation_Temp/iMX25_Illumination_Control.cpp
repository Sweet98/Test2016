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
// $RCSfile: Gen6FoundationApp/FoundationApp/Foundation_Temp/iMX25_Illumination_Control.cpp $
// $Revision: 1.15 $
// $Date: 2009/10/09 13:46:45EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// File:  iMX25_Illumination_Control.cpp
//
//		File Contains the functions used to control the Illumination and Aimer
//		Lights with the iMX25.
//		See ds021_Illumination_Control.doc for a full description on how it works
//------------------------------------------------------------------------------
#ifndef FLASH_LEDS
#include	"iMX25_Illumination_Control.h"
#include 	"HardwareMacros.h"
#include 	"PWM_Driver.h"
#include 	"ADC_Driver.h"

#include 	"platform.h"
#include 	"language.h"
#include 	"thread.h"
#include 	"interrupts.h"

#include	"menu.h"
#include	"ProductConfiguration.h"

#include 	"iMX25_GPIO.h"
#include 	"iMX25_IOMUX.h"

#define ILLSYSSTATE_TIMEOUT 20		//Thread will wake up every 20 ms if no Event, to check current status
#define ILLCONTROL_STACK_SIZE 1000
static threadhandle_t illuminationcontrol_thread = 0;

typedef enum 
{
	RED_LIGHTBAR_ILLUMINATION,	//6 RED LEDs on the Light Bar
	WHITE_ILLUMINATION,			//1 White LED on the Light Bar
	RED_LED_ILLUMINATION,		//4-6 RED LEDs near the Imager (5000 design)
	GREEN_LED_ILLUMINATION,		//4-6 Green LEDs near the Imager, used in 5000 designs as Secondary Illumination
	RED_LED_AIMER,				//2 RED LEDs on Decoder Board
	LASER_AIMER,				//iMX25 Controlled Laser
	PSOC_LASER_AIMER,			//PSOC Controlled Laser
	BRIGHT_GREEN_LED_AIMER,		//2 Green LEDs (5000) 
	GREEN_LED_AIMER,			//2 Bright Green LEDs (5100)
	LEDS_NONE,					//Either No Aimer or No Illumination
	LEDS_OFF					//All LEDs OFF 
} ILL_CTL_LED_HW;


#define ADC_EVENT_TIMEOUT 2			//Only wait for the ADC Event for 2ms
static MT_Event_t Illumination_Event = 0;

//Hardware Pin Access
#define ILL_CTRL(state)     			do{WRITE_FIELD_TO_REG(GPIO1_DR0,GPIO_BIT5_LSH,GPIO_BIT5_WID,state);}while(0)			//GPIO - RED_ILL_CTRL - GPIO_F - GPIO1_5
#define AIM_CTRL(state)     			do{WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,state);}while(0)			//GPIO - AIM_CTRL - UART2_CTS - GPIO4_29
#define LASER_SHUTOFF(state)			do{WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT5_LSH,GPIO_BIT5_WID,!state);}while(0)			//GPIO - LASER_SHUTOFF - D15 - GPIO4_5	High = off
#define FLASHOUT_PINSTATUS()			(READ_FIELD(GPIO3_PSR0,GPIO_BIT23_LSH,GPIO_BIT23_WID))									//GPIO - FLASH_OUT - ECB - GPIO3_23

#ifdef PROTO_HW
#define LED_CTRL(state)					do{WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,state);}while(0)			//GPIO - LED_CTRL - UART2_RTS - GPIO4_28
#endif //endof PROTO_HW	

#define CLEAR_FLASHOUT_INT_Signal()		do{WRITE_FIELD_TO_REG(GPIO3_ISR,GPIO_BIT23_LSH,GPIO_BIT23_WID,0x1);}while(0)			//Clears the Interrupt Status Register Flag for the FLASH_OUT Signal			
#define READ_FLASHOUT_INT_Signal()		(READ_FIELD(GPIO3_ISR,GPIO_BIT23_LSH,GPIO_BIT23_WID))									//Reads the Interrupt Status Register Flag for the FLASH_OUT Signal 
#define FLASHOUT_INT_CONFIG(state)		do{WRITE_FIELD_TO_REG(GPIO3_ICR2,GPIO_ICR23_LSH,GPIO_ICR23_WID,state);}while(0)			//Sets the Interrupt Configuration Field for the FLASH_OUT Signal
#define READ_FLASHOUT_INT_CONFIG()		(READ_FIELD(GPIO3_ICR2,GPIO_ICR23_LSH,GPIO_ICR23_WID))									//Reads the Interrupt Control Field to determin rising or falling edge
#define FLASHOUT_INT_CONTROL(state)		do{WRITE_FIELD_TO_REG(GPIO3_IMR,GPIO_BIT23_LSH,GPIO_BIT23_WID,state);}while(0)			//Enable or Disables the FLASH_OUT interrupt bit

extern MT_Event_t ADC_EOC_Event;			//ADC End Of Conversion Event

typedef enum 
{
	LIGHTSOFF,			//All Lights OFF
	ILLUMINATIONON,		//Illumination Lights ON
	AIMERON				//Aimer Lights ON
} ILL_CTL_STATES;

//Internal Functions
void Ill_Ctl_Sys_Thread(void *);
void Ill_Ctl_Sys_Initialization(void);
void Ill_Ctl_Sys_Configure(ILL_CTL_LED_HW);
bool Ill_Ctl_Sys_Voltage_Ramp_Up(UINT);
UINT Ill_Ctl_Sys_Current_Control(UINT, UINT, UINT);
void Ill_Ctl_Sys_StateChangeToILLON(void);
void Ill_Ctl_Sys_StateChangeToAIMON(void);
void Ill_Ctl_Sys_StateChangeToOFF(void);
void Ill_Ctl_Sys_UpdateIlluminationHardware(void);
void Ill_Ctl_Sys_UpdateAimerHardware(void);
void Ill_Ctl_Sys_Init_CurrentFault(void);

//Internal Variables
static ILL_CTL_STATES PresentState;
static bool AimerState = 0;							//AIMON Signal, from Scan Driver
static bool IlluminationState = 0;					//ILLON Signal, from Scan Driver
static bool FlashOutState = 0;						//HW Line from Imager to synchronize exposure
static UINT LedControlState = 0;					//The Control State of the LEDs, 0-3, from Scan Driver 
static bool CurrentFault = 0;						//Indicator if something goes wrong, like a missing Light bar, Internal

static ILL_CTL_LED_HW IlluminationHardware = RED_LIGHTBAR_ILLUMINATION;
static ILL_CTL_LED_HW AimerHardware = RED_LED_AIMER;

static UINT IlluminationADCSetting = 0;				//ADC Setting, corresponding to the present Current Setting
static UINT AimerADCSetting = 0;
static UINT IlluminationPWM_MAXSetting = 0;			//Maximum PWM setting based on HW
static UINT AimerPWM_MAXSetting = 0;	

static UINT LastKnownGoodIlluminationDutyCycle = 0;	//The Last Illumination DutyCycle the system ramped up to that was within the current limits 
static UINT WorkingIlluminationDutyCycle = 0;		//The current Illumination DutyCycle the system is ramping too
static UINT LastKnownGoodAimerDutyCycle = 0;		//The Last Aimer DutyCycle the system ramped up to that was within the current limits 
static UINT WorkingAimerDutyCycle = 0;				//The current Aimer DutyCycle the system is ramping too

static UINT ADC_MeasurementValue = 0;				//The Last ADC Value read, Made a local variable for Engineering Testing			
static ADC_Channel ADCHWChannel = AUX0;				//Sets the Channel of the ADC
static UINT  ILL_PWM_PERIOD = 0;					//Sets the Illumination Period of the PWM
static UINT  AIM_PWM_PERIOD = 0;					//Sets the Aimer Period of the PWM


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
void SetLedControlState(UINT LCState)
{	
	LedControlState = LCState & 0x0000000F;			//This version does not support Flash Mode
	
	if(LedControlState == ILLON_AIMON_CONTROL)
	{
		DisableInterruptNumber(VEC_GPIO3_INT);	
		FLASHOUT_INT_CONTROL(ILL_OFF);				//Turn Off the Flash_Out Interrupt		
		EnableInterruptNumber(VEC_GPIO3_INT);		//Disable the FLASH Out Int but not all GPIO3 Interrupts
	}
	else
	{	
		DisableInterruptNumber(VEC_GPIO3_INT);
		CLEAR_FLASHOUT_INT_Signal();
		FLASHOUT_INT_CONTROL(ILL_ON);				//Enable the Flash_Out Interrupt
		EnableInterruptNumber(VEC_GPIO3_INT);
	}
	
	if(Illumination_Event != 0x0)
	{
		MT_SetEvent(Illumination_Event);	
	}

}	//endof SetLedControlState

UINT GetLedControlState(void)
{
	return LedControlState;
}	//endof GetLedControlState

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
//    in-out:  
//       out:  
//    return:  bool  (GET)						Value of AimerState				 	 
// -------------------------------------------------------------------------------------
void SetAIM_ONState(bool AState)
{
	if(AState != AimerState)	//Only set the Event when you actually change something
	{
		AimerState = AState;
		if(Illumination_Event != 0x0)
		{
			MT_SetEvent(Illumination_Event);	
		}
	}
}	//endof SetAIM_ONState

bool GetAIM_ONState(void)
{
	return AimerState;
}	//endof GetAIM_ONState

// ===  Function  ======================================================================
// 
//         Name:  SetILL_ONState
//				  GetILL_ONState
// 
//  Description: IlluminationState takes the place of the ILL_ON Signal.
//				SetIlluminationState - Changes the State of IlluminationState based on 
//								  input parameter.  Then signals the Illumination Event  
//								  to change the Illumination State
//				GetIlluminationState - Returns the value of IlluminationState
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in: BOOL  (SET)		IState			IlluminationState value to be set
//    in-out:  
//       out:  
//    return: BOOL  (GET)						Returned value of IlluminationState					 	 
// -------------------------------------------------------------------------------------
void SetILL_ONState(bool IState)
{
	if(IState != IlluminationState)	//Only set the Event when you actually change something
	{
		IlluminationState = IState;
		if(Illumination_Event != 0x0)
		{
			MT_SetEvent(Illumination_Event);	
		}
	}
}	//endof SetILL_ONState

bool GetILL_ONState(void)
{
	return IlluminationState;
}	//endof GetILL_ONState

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
UINT GetADCValue(void)
{
	return ADC_MeasurementValue;
}	//endof GetADCValue

// ===  Function  ======================================================================
// 
//         Name:  GetFlashOutState
// 
//  Description: Returns the current value of the FLASH_OUT HW Line
//				The Flash Out Line indicates when the imager is Exposing,
//				 It can be programmed to have an advanced notice of exposure, 
//				  of up to 2mS, we default to 1mS
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
// -------------------------------------------------------------------------------------
bool GetFlashOutState(void)
{
	return FlashOutState;
}	//endof GetFlashOutState



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
void SetAimerCurrent(UINT ACurrent)
{
	if((ACurrent <=  GET_CONFIG_SETTING(AimerMAXCurrent)) &&  (ACurrent != 0x0))
	{
		AimerADCSetting = ACurrent;
	}
	else
	{
		AimerADCSetting = GET_CONFIG_SETTING(AimerMAXCurrent);
	}
}	//endof SetAimerCurrent

UINT GetAimerCurrent(void)
{
	return AimerADCSetting;
}	//endof GetAimerCurrent

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
void SetIlluminationCurrent(UINT ICurrent)
{
	if((ICurrent <=  GET_CONFIG_SETTING(PrimaryIlluminationMAXCurrent)) &&  (ICurrent != 0x0))
	{
		IlluminationADCSetting = ICurrent;
	}
	else
	{
		IlluminationADCSetting = GET_CONFIG_SETTING(PrimaryIlluminationMAXCurrent);
	}
	
}	//endof SetIlluminationCurrent

UINT GetIlluminationCurrent(void)
{
	return IlluminationADCSetting;
}	//endof GetIlluminationCurrent

// ===  Function  ======================================================================
// 
//         Name: Ill_Ctl_Sys_UpdateIlluminationHardware
// 
//  Description: IlluminationHardware - Used to tell the system what kind of HW is configured
//					Function updates the local variable and converts it to a common enum
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:  
//       out:  
//    return:  ILL_CTL_LED_HW						Illumination Hardware Configuration					 	 
// -------------------------------------------------------------------------------------
void Ill_Ctl_Sys_UpdateIlluminationHardware(void)
{
	int IllHw;
	IllHw = GET_CONFIG_SETTING(PrimaryIlluminationType);
	switch(IllHw)
	{
		case ILLUMINATION_NONE:
		default:
			IlluminationHardware = LEDS_NONE;
			break;
			
		case ILLUMINATION_RED_LED:
			IlluminationHardware = RED_LED_ILLUMINATION;
			break;
			
		case ILLUMINATION_RED_LIGHT_BAR:
			IlluminationHardware = RED_LIGHTBAR_ILLUMINATION;
			break;
			
		case ILLUMINATION_WHITE:
			IlluminationHardware = WHITE_ILLUMINATION;
			break;
			
		case ILLUMINATION_GREEN:
			IlluminationHardware = GREEN_LED_ILLUMINATION;
			break;
	} //endof switch(IllHw)
}	//endof Ill_Ctl_Sys_UpdateIlluminationHardware

// ===  Function  ======================================================================
// 
//         Name:  Ill_Ctl_Sys_UpdateAimerHardware
// 
//  Description: AimerHardware - Used to tell the system what kind of HW is configured
//					Function updates the local variable and converts it to a common enum
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:  
//       out:  
//    return:  ILL_CTL_LED_HW						Aimer Hardware Configuration				 	 
// -------------------------------------------------------------------------------------
void Ill_Ctl_Sys_UpdateAimerHardware(void)
{
	int AimHw;
	AimHw = GET_CONFIG_SETTING(AimerType);
	switch(AimHw)
	{
		case AIMER_NONE:
		default:
			AimerHardware = LEDS_NONE;
			break;
		
		case AIMER_RED_LED:
			AimerHardware = RED_LED_AIMER;
			break;
			
		case AIMER_LASER:
			AimerHardware = LASER_AIMER;
			break;
			
		case AIMER_PSOC_LASER:
			AimerHardware = PSOC_LASER_AIMER;
			break;
			
		case AIMER_BRIGHT_GREEN_LED:
			AimerHardware = BRIGHT_GREEN_LED_AIMER;
			break;
			
		case AIMER_GREEN_LED:
			AimerHardware = GREEN_LED_AIMER;
			break;
			
	} //endof switch(AimHw)
}	//endof Ill_Ctl_Sys_UpdateAimerHardware

// ===  Function  ======================================================================
// 
//         Name:  Ill_Ctl_Sys_Init_CurrentFault
// 
//  Description: Changes the state of the system if there is a current fault.	
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:  
//       out:  
//    return:					 	 
// -------------------------------------------------------------------------------------
void Ill_Ctl_Sys_Init_CurrentFault(void)
{
	CurrentFault = true;				//Indicate a problem
	PWM_Stop(PWM3);						//Stop The PWM 
	Ill_Ctl_Sys_Configure(LEDS_OFF);	//Configure All Lights Off
	PresentState = LIGHTSOFF;	
}	//endof Ill_Ctl_Sys_Init_CurrentFault

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
void Ill_Ctl_Sys_Init_Thread(void)
{
	Ill_Ctl_Sys_Initialization();
	if ( illuminationcontrol_thread == 0x0)
	{
		illuminationcontrol_thread = MT_OpenDriverThread(ILLCONTROL_PRIORITY,ILLCONTROL_STACK_SIZE,&Ill_Ctl_Sys_Thread,NULL);
		WakeDriverThread(illuminationcontrol_thread);		//Run the Thread
	}
}	//endof Ill_Ctl_Sys_Init_Thread

// ===  Function  ======================================================================
//
//  Name:  Ill_Ctl_Sys_Thread
//
//  Description:  Main Thread that runs the Illumination System on the iMX25
//				Runs when the Illumination_Event is triggered, will change states based
//				on the inputs set before the event runs.  Will wake up every ILLSYSSTATE_TIMEOUT
//				to update the current if needed.
//				See ds021_Illumination_Control.doc for a full description	
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:
//       out:
//    return:	
// -------------------------------------------------------------------------------------
static void Ill_Ctl_Sys_Thread(void *){
	bool Eventsignal;
	UINT CapturedLedControlState = 0;
	bool CapturedAimerState = 0;
	bool CapturedIlluminationState = 0;
			
	while(TRUE)
	{
		Eventsignal = MT_WaitEvent(Illumination_Event,ILLSYSSTATE_TIMEOUT);
		if(Eventsignal)
		{
			//Freeze the Inputs incase something changes them before the event is over
			CapturedAimerState = AimerState;
			CapturedIlluminationState = IlluminationState;
			FlashOutState = FLASHOUT_PINSTATUS(); //Update FlashOutState (only place it can be updated)
			CapturedLedControlState = LedControlState;

//			Y1 = !CurrentFault && ((CapturedIlluminationState || (!CapturedIlluminationState && (!CapturedAimerState && (FlashOutState && \
//				 ((CapturedLedControlState == FLASHOUT_ILLUM_CONTROL) ||  (CapturedLedControlState == FLASHOUT_CONTROL)))))))
//
//			Y6 = !CurrentFault && ((!CapturedIlluminationState && CapturedAimerState) || (!CapturedIlluminationState && \
//				 (!CapturedAimerState && (!FlashOutState && ((CapturedLedControlState == FLASHOUT_AIMER_CONTROL) || (CapturedLedControlState == FLASHOUT_CONTROL)))))
//
//			Y4 = CurrentFault || (!CapturedIlluminationState && (!CapturedAimerState && ((CapturedLedControlState == ILLON_AIMON_CONTROL) || \
//				 ((CapturedLedControlState == FLASHOUT_AIMER_CONTROL) && FlashOutState) || ((CapturedLedControlState == FLASHOUT_ILLUM_CONTROL) && !FlashOutState)))))
//
//			Y2 = !CurrentFault && ((!CapturedIlluminationState && CapturedAimerState) || (!CapturedIlluminationState && (!CapturedAimerState && (!FlashOutState && \
//				 ((CapturedLedControlState == FLASHOUT_AIMER_CONTROL) || (CapturedLedControlState == FLASHOUT_CONTROL))))))
//
//			Y3 = CurrentFault || (!CapturedIlluminationState && (!CapturedAimerState && ((CapturedLedControlState == ILLON_AIMON_CONTROL) || \
//				 ((CapturedLedControlState == FLASHOUT_AIMER_CONTROL) && FlashOutState) || ((CapturedLedControlState = FLASHOUT_ILLUM_CONTROL) && !FlashOutState))))
//
//			Y5 = !CurrentFault && (CapturedIlluminationState || (!CapturedAimerState && (FlashOutState && ((CapturedLedControlState ==FLASHOUT_ILLUM_CONTROL) || \
//				 (CapturedLedControlState ==FLASHOUT_CONTROL)))))
						
			switch(PresentState)
			{
				case LIGHTSOFF:
					if(!CurrentFault && (CapturedIlluminationState || (!CapturedIlluminationState && (!CapturedAimerState && (FlashOutState && \
					((CapturedLedControlState == FLASHOUT_ILLUM_CONTROL) ||  (CapturedLedControlState == FLASHOUT_CONTROL)))))))//Y1
					{
						Ill_Ctl_Sys_StateChangeToILLON();
						if(!CurrentFault)
						{
							PresentState = ILLUMINATIONON;
						}
					}
					else if(!CurrentFault && ((!CapturedIlluminationState && CapturedAimerState) || (!CapturedIlluminationState && \
					(!CapturedAimerState && (!FlashOutState && ((CapturedLedControlState == FLASHOUT_AIMER_CONTROL) || (CapturedLedControlState == FLASHOUT_CONTROL)))))))//Y6
					{
						Ill_Ctl_Sys_StateChangeToAIMON();
						if(!CurrentFault)
						{
							PresentState = AIMERON;
						}
					}
					break;
					
				case ILLUMINATIONON:
					if(CurrentFault || (!CapturedIlluminationState && (!CapturedAimerState && ((CapturedLedControlState == ILLON_AIMON_CONTROL) || \
					((CapturedLedControlState == FLASHOUT_AIMER_CONTROL) && FlashOutState) || ((CapturedLedControlState == FLASHOUT_ILLUM_CONTROL) && !FlashOutState))))) //Y4
					{
						Ill_Ctl_Sys_StateChangeToOFF();
						PresentState = LIGHTSOFF;
					}
					else if(!CurrentFault && ((!CapturedIlluminationState && CapturedAimerState) || (!CapturedIlluminationState && (!CapturedAimerState && (!FlashOutState && \
					((CapturedLedControlState == FLASHOUT_AIMER_CONTROL) || (CapturedLedControlState == FLASHOUT_CONTROL)))))))//Y2
					{
						Ill_Ctl_Sys_StateChangeToAIMON();
						if(!CurrentFault)
						{
							PresentState = AIMERON;
						}
					}
					break;
					
				case AIMERON:
					if(CurrentFault || (!CapturedIlluminationState && (!CapturedAimerState && ((CapturedLedControlState == ILLON_AIMON_CONTROL) || \
					((CapturedLedControlState == FLASHOUT_AIMER_CONTROL) && FlashOutState) || ((CapturedLedControlState == FLASHOUT_ILLUM_CONTROL) && !FlashOutState)))))//Y3
					{
						Ill_Ctl_Sys_StateChangeToOFF();
						PresentState = LIGHTSOFF;
					}
					else if(!CurrentFault && (CapturedIlluminationState || (!CapturedAimerState && (FlashOutState && ((CapturedLedControlState == FLASHOUT_ILLUM_CONTROL) || \
					(CapturedLedControlState == FLASHOUT_CONTROL))))))//Y5
					{
						Ill_Ctl_Sys_StateChangeToILLON();
						if(!CurrentFault)
						{
							PresentState = ILLUMINATIONON;
						}
					}
					break;
					
			} //endof PresentState switch
		}
		else
		{
			switch(PresentState)
			{
				case LIGHTSOFF:
						//No Illumination is running at this time so do not update anything
					break;
					
				case ILLUMINATIONON:
					LastKnownGoodIlluminationDutyCycle = Ill_Ctl_Sys_Current_Control(WorkingIlluminationDutyCycle, IlluminationADCSetting, IlluminationPWM_MAXSetting);
					if(!LastKnownGoodIlluminationDutyCycle == 0x0)
					{
						WorkingIlluminationDutyCycle = LastKnownGoodIlluminationDutyCycle;
					}
					else
					{
						Ill_Ctl_Sys_Init_CurrentFault();
					}
					break;
					
				case AIMERON:
					LastKnownGoodAimerDutyCycle = Ill_Ctl_Sys_Current_Control(WorkingAimerDutyCycle, AimerADCSetting, AimerPWM_MAXSetting);
					if(!LastKnownGoodAimerDutyCycle == 0x0)
					{
						WorkingAimerDutyCycle = LastKnownGoodAimerDutyCycle;
					}
					else
					{
						Ill_Ctl_Sys_Init_CurrentFault();
					}
					break;
					
			} //endof PresentState switch
		} //endof else Eventsignal 	
	} //endof while(TRUE)
}	//endof Ill_Ctl_Sys_Thread

// ===  Function  ======================================================================
// 
//         Name:  Ill_Ctl_Sys_StateChangeToILLON
// 
//  Description: Runs all the functions needed to turn on the Illumination LEDs and change
//					the state to ILLUMINATIONON.  Will flag an Error if it detects a HW problem
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:  
//       out:  
//    return:
// -------------------------------------------------------------------------------------
static void Ill_Ctl_Sys_StateChangeToILLON(void)
{
	UINT IStatus = 0;

	PWM_Period(PWM3,ILL_PWM_PERIOD);  //Set the Period of the PWM to Illumination values
	
	do{
		Ill_Ctl_Sys_Configure(IlluminationHardware);
		IStatus += Ill_Ctl_Sys_Voltage_Ramp_Up(WorkingIlluminationDutyCycle);
		if(IStatus == 0x0)
		{
			LastKnownGoodIlluminationDutyCycle = Ill_Ctl_Sys_Current_Control(WorkingIlluminationDutyCycle, IlluminationADCSetting, IlluminationPWM_MAXSetting);
			if(LastKnownGoodIlluminationDutyCycle != 0x0)
			{
				WorkingIlluminationDutyCycle = LastKnownGoodIlluminationDutyCycle;
				IStatus = 0;
			}
			else
			{
				IStatus++;
			}
		}
		
		if(IStatus > 0x3)						//Run through this routine three times, if it fails then signal a fault
		{
			Ill_Ctl_Sys_Init_CurrentFault();	//once a fault is triggered, a reset is need to clear it
			return;
		}
	}while (IStatus != 0x0);

}	//endof Ill_Ctl_Sys_StateChangeToILLON

// ===  Function  ======================================================================
// 
//         Name:  Ill_Ctl_Sys_StateChangeToAIMON
// 
//  Description: Runs all the functions needed to turn the Aimers on and change the state
//					to AIMERON. Will flag an Error if it detects a HW problem
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:			
//    in-out:  
//       out:  
//    return:					 	 
// -------------------------------------------------------------------------------------
static void Ill_Ctl_Sys_StateChangeToAIMON(void)
{
	UINT AStatus = 0;
	
	PWM_Period(PWM3,AIM_PWM_PERIOD); //Set the Period of the PWM to Aimer values
	
	do{
		PWM_Stop(PWM3);
		Ill_Ctl_Sys_Configure(AimerHardware);
		AStatus += Ill_Ctl_Sys_Voltage_Ramp_Up(WorkingAimerDutyCycle);
		if(AStatus == 0x0)
		{
			LastKnownGoodAimerDutyCycle = Ill_Ctl_Sys_Current_Control(WorkingAimerDutyCycle, AimerADCSetting, AimerPWM_MAXSetting);
			if(LastKnownGoodIlluminationDutyCycle != 0x0)
			{
				WorkingAimerDutyCycle = LastKnownGoodAimerDutyCycle;
				AStatus = 0;
			}
			else
			{
				AStatus++;
			}
		}
		
		if(AStatus > 0x3)					//Run through this routine three times, if it fails then signal a fault
		{
			Ill_Ctl_Sys_Init_CurrentFault();	//once a fault is triggered, a reset is need to clear it
			return;
		}
	}while (AStatus != 0x0);
	
}	//endof Ill_Ctl_Sys_StateChangeToAIMON

// ===  Function  ======================================================================
// 
//         Name:  Ill_Ctl_Sys_StateChangeToOFF
// 
//  Description: Runs all the functions needed to turn the lights off and change the state
//					to LIGHTSOFF
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:			
//    in-out:  
//       out:  
//    return:
// -------------------------------------------------------------------------------------
static void Ill_Ctl_Sys_StateChangeToOFF(void)
{
//	DisableInterruptNumber(VEC_GPIO3_INT);			
//	FLASHOUT_INT_CONTROL(ILL_OFF);				//Turn Off the Flash_Out Interrupt
	PWM_Stop(PWM3);								//Stop The PWM
	Ill_Ctl_Sys_Configure(LEDS_OFF);			//Configure All Lights Off
//	EnableInterruptNumber(VEC_GPIO3_INT);		//Disable the FLASH Out Int but not all GPIO3 Interrupts
}	//endof Ill_Ctl_Sys_StateChangeToAIMON

// ===  Function  ======================================================================
//
//  Name:  Ill_Ctl_Sys_Initialization
//
//  Description:  Initial Configuration of GPIO, PWM Initialization, and ADC 
//		Initialization take place here.  Also sets up Interrupts and Events used
//		by the Illumination System
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:	
// -------------------------------------------------------------------------------------
static void Ill_Ctl_Sys_Initialization(void){

	Ill_Ctl_Sys_UpdateIlluminationHardware();
	Ill_Ctl_Sys_UpdateAimerHardware();
	
	DISABLE_INTERRUPTS();		//Disable Interrupts to write to the GPIO, used the Hazard Macros that do not disable Interrupts 
	
	//GPIO - RED_ILL_CTRL - GPIO_F - GPIO1_5
	//Turns on the Light bar, could be populated with 6 RED LEDs or 1 White LED
	HAZARD_WRITE_FIELD_TO_REG(IOMUXC_SW_MUX_CTL_PAD_GPIO_F,IOMUXC_SW_MUX_CTL_MUX_MODE_LSH,IOMUXC_SW_MUX_CTL_MUX_MODE_WID,0x0);	//MUX To GPIO set as zero b/c this is a deciated GPIO Pin (GPIO_F)
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_GPIO_F,0x00000007);	//PAD Regiser, MAX Drive Strength, Fast SLEW Rate, No Pullups
	HAZARD_WRITE_FIELD_TO_REG(GPIO1_DR0,GPIO_BIT5_LSH,GPIO_BIT5_WID,GPIO_LOW);	//Data Register, Initilize low to turn OFF
	HAZARD_WRITE_FIELD_TO_REG(GPIO1_GDIR0,GPIO_BIT5_LSH,GPIO_BIT5_WID,GPIO_GDIR_OUTPUT);	//Direction Register

	//GPIO - AIM_CTRL - UART2_CTS - GPIO4_29
	//Turns on the 2 RED Aimer LEDs
	HAZARD_WRITE_FIELD_TO_REG(IOMUXC_SW_MUX_CTL_PAD_UART2_CTS,IOMUXC_SW_MUX_CTL_MUX_MODE_LSH,IOMUXC_SW_MUX_CTL_MUX_MODE_WID,MUX_MODE_5);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART2_CTS,0x00000007);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,GPIO_GDIR_OUTPUT);

#ifdef PROTO_HW
	//GPIO - LED_CTRL - UART2_RTS - GPIO4_28
	//Master Control SW for Light bar and RED Aimers, must be ON for LEDs to be ON
	HAZARD_WRITE_FIELD_TO_REG(IOMUXC_SW_MUX_CTL_PAD_UART2_RTS,IOMUXC_SW_MUX_CTL_MUX_MODE_LSH,IOMUXC_SW_MUX_CTL_MUX_MODE_WID,MUX_MODE_5);	//MUX To GPIO
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART2_RTS,0x00000007);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,GPIO_GDIR_OUTPUT);
#endif //endof PROTO_HW	

	//GPIO - LASER_SHUTOFF - D15 - GPIO4_5
	//Turns the Laser off when iMX25 is controling it
	HAZARD_WRITE_FIELD_TO_REG(IOMUXC_SW_MUX_CTL_PAD_D15,IOMUXC_SW_MUX_CTL_MUX_MODE_LSH,IOMUXC_SW_MUX_CTL_MUX_MODE_WID,MUX_MODE_5);	//MUX To GPIO
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_D15,0x00000007);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT5_LSH,GPIO_BIT5_WID,GPIO_HIGH);		//High will turn off the laser
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT5_LSH,GPIO_BIT5_WID,GPIO_GDIR_OUTPUT);
	
	//GPIO - Reset_PSoC - KPP_ROW3 - GPIO3_0
	//Resets the PSOC
	HAZARD_WRITE_FIELD_TO_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_ROW3,IOMUXC_SW_MUX_CTL_MUX_MODE_LSH,IOMUXC_SW_MUX_CTL_MUX_MODE_WID,MUX_MODE_5);	//MUX To GPIO
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_KPP_ROW3,0x00000007);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT0_LSH,GPIO_BIT0_WID,GPIO_LOW);		//Take the PSOC out of Reset if it is
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT0_LSH,GPIO_BIT0_WID,GPIO_GDIR_OUTPUT);
	
	//GPIO - FLASH_OUT - ECB - GPIO3_23
	//Input from Imager, indicates when the LEDs should turn on
	HAZARD_WRITE_FIELD_TO_REG(IOMUXC_SW_MUX_CTL_PAD_ECB,IOMUXC_SW_MUX_CTL_MUX_MODE_LSH,IOMUXC_SW_MUX_CTL_MUX_MODE_WID,MUX_MODE_5);	//MUX To GPIO
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_ECB,0x00000000);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT5_LSH,GPIO_BIT5_WID,GPIO_GDIR_INPUT);
	
	RESTORE_INTERRUPTS();
	
	//Setup Variables
	LedControlState = 0;
	AimerState = 0;
	IlluminationState = 0;
	FlashOutState = 0;
	PresentState = LIGHTSOFF;
	
	//---------Determin Illumination Current Settings based on the HW---------
	if(IlluminationHardware == RED_LIGHTBAR_ILLUMINATION)
	{
		//Note: ADC Setting comes from Product Config, with Illumination Type
#ifndef PROTO_HW
		IlluminationPWM_MAXSetting = 0x88;	//50mA setting
#else
		IlluminationPWM_MAXSetting = 0x7A;	//50mA setting
#endif //endof PROTO_HW		
		ILL_PWM_PERIOD = 0xC6;				//Sets the Period of the PWM to 600kHz
	}
	else if (IlluminationHardware == WHITE_ILLUMINATION)
	{
		//Needs to be characterized
		//IlluminationPWM_MAXSetting = 0x68;
		//ILL_PWM_PERIOD =??;
	}
	else //No support at this time for other types of Illumination
	{
		IlluminationPWM_MAXSetting = 0x0;
		ILL_PWM_PERIOD = 0x0;
	}
	
	//---------Determin Aimer Current Settings based on the HW---------
	if(AimerHardware == RED_LED_AIMER)
	{
#ifndef PROTO_HW
		AimerPWM_MAXSetting = 	0x37;		//50mA setting
#else
		AimerPWM_MAXSetting = 	0x25;		//50mA setting
#endif //endof PROTO_HW		
		
		AIM_PWM_PERIOD = 0xC6;

	}
	else if (AimerHardware == LASER_AIMER)
	{
		AimerPWM_MAXSetting = 0x0;
		AIM_PWM_PERIOD = 0x0;  //Need to fix this
	}
	else if (AimerHardware == PSOC_LASER_AIMER)
	{
		AimerPWM_MAXSetting = 0x0;
		AIM_PWM_PERIOD = 0x0;
	}
	else
	{
		AimerPWM_MAXSetting = 0x0;
		AIM_PWM_PERIOD = 0x0;
	}
	
	//After the Max settings are determined from the HW set the run mode settings
	//Set the current control to the first known state
	IlluminationADCSetting = GET_CONFIG_SETTING(PrimaryIlluminationMAXCurrent);
	AimerADCSetting = GET_CONFIG_SETTING(AimerMAXCurrent);
	
	LastKnownGoodIlluminationDutyCycle = IlluminationPWM_MAXSetting;
	LastKnownGoodAimerDutyCycle = AimerPWM_MAXSetting;
	WorkingIlluminationDutyCycle = IlluminationPWM_MAXSetting;
	WorkingAimerDutyCycle = AimerPWM_MAXSetting;
	
	//Initilize the ADC
	ADC_Init();
	
	//Initilize the PWM
	if((IlluminationHardware == RED_LIGHTBAR_ILLUMINATION) || (IlluminationHardware == WHITE_ILLUMINATION) || (AimerHardware == RED_LED_AIMER))
	{
		ADCHWChannel = AUX0;
		PWM_Init(PWM3,FEC_TX_CLK,0x1);
		PWM_Period(PWM3,ILL_PWM_PERIOD);		//Setup for the Red Led Period, if the white turn on the this will need to change
		PWM_Sample(PWM3,0x0);	//Initilizes the DutyCycle to 0, if the PWM was started now nothing would happen.
	}
	if(AimerHardware == LASER_AIMER)
	{
		//Note: No Laser Design yet
//		PWM_Init(PWM4,CONTRAST,0x1);
//		PWM_Period(PWM4,PWM_PERIOD);
//		PWM_Sample(PWM4,0x0);	//Initilizes the DutyCycle to 0, if the PWM was started now nothing would happen.
	}
	
	if (Illumination_Event == 0x0)
	{
		Illumination_Event = MT_CreateEvent_Ex(false, true);		//Create an Auto Resetting Event, to signal a change in the Illumination System
	}

	//Setup Interrupt, GPIO for the Flash Out Signal, The GPIO3 INT is setup here but the Bit mask is not set until we start scanning
	SetVector(GPIO3_Service_IRQ,VEC_GPIO3_INT);
	CLEAR_FLASHOUT_INT_Signal();										//Clear the Flag if it is set
	WRITE_FIELD_TO_REG(GPIO3_EDGE,GPIO_BIT23_LSH,GPIO_BIT23_WID,0x1);	//Set to INT on any EDGE, overrides the ICR2 REG SETTING
	EnableInterruptNumber(VEC_GPIO3_INT);  // Enable interrupts GPIO Port 3
		
}	//endof Ill_Ctl_Sys_Initialization

// ===  Function  ======================================================================
//
//  Name:  Ill_Ctl_Sys_Configure
//
//  Description:  Configures the Hardware Lines based on which LEDs are turning on
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	ILL_CTL_LED_HW 	LED				LED to be configured
//    in-out:
//       out:
//    return:	
// -------------------------------------------------------------------------------------
static void Ill_Ctl_Sys_Configure(ILL_CTL_LED_HW LED){
	switch (LED)
	{
		case LEDS_OFF:
		default:
			AIM_CTRL(ILL_OFF);
			LASER_SHUTOFF(ILL_OFF);
			ILL_CTRL(ILL_OFF);
#ifdef PROTO_HW
			LED_CTRL(ILL_OFF);
#endif //endof PROTO_HW		
			break;
		
		case RED_LIGHTBAR_ILLUMINATION:
		case WHITE_ILLUMINATION:
			AIM_CTRL(ILL_OFF);
			LASER_SHUTOFF(ILL_OFF);
			ILL_CTRL(ILL_ON);
#ifdef PROTO_HW
			LED_CTRL(ILL_ON);
#endif //endof PROTO_HW	
			break;
		
		case RED_LED_AIMER: 
			LASER_SHUTOFF(ILL_OFF);
			ILL_CTRL(ILL_OFF);
			AIM_CTRL(ILL_ON);
#ifdef PROTO_HW
			LED_CTRL(ILL_ON);
#endif //endof PROTO_HW	
			break;
		
		case PSOC_LASER_AIMER: 
			LASER_SHUTOFF(ILL_OFF);
			ILL_CTRL(ILL_OFF);
#ifdef PROTO_HW
			LED_CTRL(ILL_OFF);
#endif //endof PROTO_HW	
			AIM_CTRL(ILL_ON);
			break;
		
		case LASER_AIMER: 
			AIM_CTRL(ILL_OFF);
			ILL_CTRL(ILL_OFF);
#ifdef PROTO_HW
			LED_CTRL(ILL_OFF);
#endif //endof PROTO_HW	
			LASER_SHUTOFF(ILL_ON);
			break;
	}//endof LED Switch

}	//endof Ill_Ctl_Sys_Configure

// ===  Function  ======================================================================
//
//  Name:  Ill_Ctl_Sys_Voltage_Ramp_Up
//
//  Description:  Ramps the PWM Duty Cycle up from zero to a pre-set point.  The 
//		pre-set point is based on how much voltage is needed to turn on the selected LEDs.
//		The Ramp-up steps the duty cycle up from 0 to the set point based on how far off the 
//		Final value is.
//	
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  UINT 		FinalPWMValue		PWM Value the Function will ramp to  
//    in-out:
//       out:
//    return: bool								Returns a 1 when an error has occured
//												Returns a 0 when the Duty Cycle = FinalPWMValue
// -------------------------------------------------------------------------------------
static bool Ill_Ctl_Sys_Voltage_Ramp_Up(UINT FinalPWMValue)
{
	UINT StopPoint = FinalPWMValue;	//The Ramp-up will ramp to the last known PWM value
	UINT DutyCycle = 0xA;
	UINT IncValue = 0;
	BOOL OkToExit = 0;

	PWM_Sample(PWM3,DutyCycle);
	PWM_Start(PWM3);
	
	ADC_Setup(ADCHWChannel, 0x6, REF_Internal);	//Make number of samples small to make the loop smaller
	ADC_Start_Convert();
	
	do{	
		if(!MT_WaitEvent(ADC_EOC_Event,ADC_EVENT_TIMEOUT))
		{
			return 1;  //The ADC Interrupt took too long to return so there must be a problem
		}
		MT_ResetEvent(ADC_EOC_Event);
		ADC_MeasurementValue = ADC_Read_FIFO();//Read and Clear the ADC buffer.

		if(DutyCycle < StopPoint)
		{
			IncValue = (StopPoint - DutyCycle)/2;
			if(IncValue < 1)
			{
				DutyCycle += 0x1;
				OkToExit = FALSE;
			}
			else
			{
				DutyCycle += IncValue;
				OkToExit = FALSE;
			}
		}
		else if(DutyCycle == StopPoint)
		{
			if(ADC_MeasurementValue == 0x0)
			{
				//This would indicate that there was no HW present 
				//because the system has reached the final PWM value 
				//but has not measured any current
				return 1;	
			}
			else
			{
				OkToExit = TRUE;
			}
		}
		else
		{
			DutyCycle -= 0x2;
			OkToExit = FALSE;
		}		
						
		if (OkToExit == FALSE)
		{
			PWM_Sample(PWM3,DutyCycle);	//Update the DutyCycle Just Calculated
			ADC_Setup(ADCHWChannel, 0x6, REF_Internal);	//Start with a small number of samples to make the loop smaller
			ADC_Start_Convert();
		}
	
	}while(OkToExit == FALSE);

	return 0;
}	//endof Ill_Ctl_Sys_Voltage_Ramp_Up

// ===  Function  ======================================================================
//
//  Name:  Ill_Ctl_Sys_Current_Control
//
//  Description:  Samples the ADC and adjusts the PWM as needed, until the current is
//					within an acceptable range defined by AcceptableWindowSize
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            		Description
// -------------------------------------------------------------------------------------
//        in:  UINT			CurrentDutyCycle			Starting DutyCycle the PWM is running at
//			   UINT 		ADCValueSetPoint			Target ADC Reading the system will reach 
//															before returning
//			   UINT 		MAXPWMValue					Used for Error Checking, the ADC 
//															should not read more then this value
//    in-out:
//       out:
//    return:  UINT										DutyCycle value used when ADC reading
//															is within the window		 
// -------------------------------------------------------------------------------------
#define AcceptableWindowSize 4
static UINT Ill_Ctl_Sys_Current_Control(UINT CurrentDutyCycle, UINT ADCValueSetPoint, UINT MAXPWMValue)
{
	UINT DutyCycle = CurrentDutyCycle;
	bool OkToExit = false;
	
	ADC_Setup(ADCHWChannel, 0x2, REF_Internal);
	ADC_Start_Convert();
	
	do{
		if(!MT_WaitEvent(ADC_EOC_Event,ADC_EVENT_TIMEOUT))
		{
			return 0;  //The ADC Event has timed out, signal an Error
		}
		else
		{
			MT_ResetEvent(ADC_EOC_Event);
			ADC_MeasurementValue = ADC_Read_FIFO();		//AUX0
		}
				
		if((ADC_MeasurementValue >= (ADCValueSetPoint - AcceptableWindowSize)) && (ADC_MeasurementValue <= (ADCValueSetPoint + AcceptableWindowSize)))
		{
			OkToExit = TRUE;
		}
		else if (ADC_MeasurementValue < (ADCValueSetPoint - AcceptableWindowSize))
		{
			if(DutyCycle > (MAXPWMValue + 0x40))
			{
				return 0;	//Something is wrong with the HW and the DC should not be increased
			}
			DutyCycle++;
			OkToExit = FALSE;
		}
		else if (ADC_MeasurementValue > (ADCValueSetPoint + AcceptableWindowSize))
		{
			DutyCycle--;
			OkToExit = FALSE;
		}
		else
		{
			OkToExit = FALSE;
			//Debug, should not get here
		}
		
		
		if (OkToExit == 0)	//Only update the DutyCycle if we have changed it.
		{
			PWM_Sample(PWM3,DutyCycle);	//Update the new DutyCycle
			ADC_Setup(ADCHWChannel, 0x2, REF_Internal);
			ADC_Start_Convert();
		}
		
	}while(OkToExit == FALSE);
	
	return DutyCycle;	//Return the Last known good value.
	
}	//endof Ill_Ctl_Sys_Current_Control

// ===  Function  ======================================================================
//
//  Name:  GPIO3_Service_IRQ
//
//  Description:  General IRQ for all GPIO3 pins. 
//				Sets an event when the Flash Out Line interrupts.  The interrupt should 
//				fire on every edge of the FlashOut signal
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	threadhandle_t
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
static threadhandle_t GPIO3_Service_IRQ(void *)
{
	DisableInterruptNumber(VEC_GPIO3_INT);
	
	if(READ_FLASHOUT_INT_Signal())  //Check if the FlashOut Interrupt has been enabled
	{
		CLEAR_FLASHOUT_INT_Signal();
		if(Illumination_Event != 0x0)
		{
			MT_SetEvent(Illumination_Event);	
		}
	}
		
	EnableInterruptNumber(VEC_GPIO3_INT);
	
	return NULL;
}	//endof GPIO3_Service_IRQ

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
void Ill_Ctl_Sys_Destroy(void)
{
	if ( !illuminationcontrol_thread == 0x0)
	{
		MT_CloseEvent(Illumination_Event);
		MT_CloseDriverThread(illuminationcontrol_thread);
	}
}	//endof Ill_Ctl_Sys_Destroy
#endif //end of FLASH_LEDS

