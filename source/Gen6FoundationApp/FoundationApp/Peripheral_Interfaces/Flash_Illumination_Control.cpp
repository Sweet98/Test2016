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
// $RCSfile: Gen6FoundationApp/FoundationApp/Peripheral_Interfaces/Flash_Illumination_Control.cpp $
// $Revision: 1.57 $
// $Date: 2011/11/10 03:41:36EST $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
/* File:  Flash_Illumination_Control.cpp
//
//		File Contains the functions used to control the Flash Illumination
//		and Aimer Lights with the iMX25.  This is also responsible for some power
//		management in that it monitors the beep indicator and turns off the charging
//		of the Scan Illumination LEDs 
//		See ds021_Illumination_Control.doc for more detailed functionality.

  The initial concept was a two level illumination :  low and high.  Later, a third
  level was added (medium).  So, you will see values of 0 for low, 1 for high and
  2 for medium.  The above document was not updated for this.
//------------------------------------------------------------------------------*/
#ifdef FLASH_LEDS

#include	"Flash_Illumination_Control.h"

#include 	"HardwareMacros.h"
#include 	"iMX25_GPIO.h"
#include 	"iMX25_IOMUX.h"

#include 	"platform.h"
#include 	"language.h"
#include 	"thread.h"
#include 	"interrupts.h"

#include	"menu.h"
#include	"ProductConfiguration.h"
#include 	"ReaderConfiguration.h"
#include 	"IoMan.h"

#include 	"beep.h"
#include 	"timeout.h"
#include	"Timer3_Driver.h"
#include 	"HardwareToken.h"
#include "db_chrio.h" 

#ifdef _SMART_SCAN_DRIVER_FLASH_INTEN_CNTRL		//pass intensity control to scan driver
#include "trigger_5_5.h"
#endif

#include 	"PWM_Driver.h"//for hi2d
extern bool AimOnstatus;


//#define _LMC_DEBUGING_STATE            //un-comment out for Lights state debugging		lmc
#ifdef	_LMC_DEBUGING_STATE
	#include "ioman.h"
	#define	PRINT_DEBUG			theStdInterface.printf
#else
	static void DummyPrintLights( char* a, int b=0, int c=0, int d=0, int e=0){return;}
	#define	PRINT_DEBUG		DummyPrintLights
#endif


//#define _LMC_DEBUGING_THREAD            //un-comment out for Lights thread state machine debugging		lmc
#ifdef	_LMC_DEBUGING_THREAD
	#include "db_chrio.h"                  //lmc   debug
	#define	XPUT_STRING_DEBUG			xputstring
#else
	static void DummyXPutStringLights(char* a){return;}
	#define	XPUT_STRING_DEBUG		DummyXPutStringLights
#endif


//Internal Defines
#define ILL_HIGH_CURRENT GPIO_GDIR_OUTPUT   
#define ILL_LOW_CURRENT GPIO_GDIR_INPUT    
#define ILL_MID_CURRENT 2    //define for 3-level current. High:1, Low:0, Mid:2
#define ILLCONTROL_STACK_SIZE 1000
#define RUN_LEDCONTROL_FLAG 0x80000000


typedef enum
{
	LIGHTS_OFF,				//All Lights OFF
	ILLUMINATION_ON,		//Illumination Lights ON
	AIMER_ON				//Aimer Lights ON
} ILL_CTL_LIGHT_STATES;

// ILL_CTL_CURRENT_STATES are the state of the current level (voltage) we want to charge the cap(s) to for the Scan Illumination LEDs.  They are not to be confused
// with the actual Illumination levels which are controlled via 3 gpio lines.  Thus, BOOST_OFF is not "Scan Illumination off"--it is a low power mode we go
// into 
typedef enum
{
	BOOST_OFF,				//Boost Circuit Off, No Lights
	LOW_CURRENT,			//Circuit is configured for Low Current Mode
	HIGH_CURRENT,			//Circuit is configured for High Current Mode
	MID_CURRENT    //ewr00003
} ILL_CTL_CURRENT_STATES;

//Internal Functions
static void Ill_Ctl_Sys_Thread(void *);
static bool Ill_Ctl_Sys_StateChangeToILLON(void);
static void Ill_Ctl_Sys_StateChangeToAIMON(void);
static void Ill_Ctl_Sys_StateChangeToOFF(void);
static void Ill_Ctl_Sys_StateChangeToLOW_CURRENT(bool);
static void Ill_Ctl_Sys_StateChangeToHIGH_CURRENT(bool);  
static void Ill_Ctl_Sys_StateChangeToMID_CURRENT(bool); 
static void Ill_Ctl_Sys_StateChangeToBOOST_OFF(void);
static void Ill_HW_Pin_Initialization(void);
static void Ill_HW_Flash_Interrupt_Init(void);
static threadhandle_t GPIO3_Service_IRQ(void *);
static void Psoc_HW_Pin_Initialization(void);
static void SetFlashLedControlState( void );
void LED_LVL(int state);  

//Internal Variables
static threadhandle_t illuminationcontrol_thread = 0;
static MT_Event_t Illumination_Light_Event = 0;
static MT_Event_t Illumination_Current_Event = 0;
static MT_Event_t Illumination_FlashOut_Event = 0;
static MT_Event_t Aimer_On_Event = 0;

static ILL_CTL_LIGHT_STATES LightsPresentState = LIGHTS_OFF;	//Current Lights State Machine State
static ILL_CTL_CURRENT_STATES CurrentPresentState = BOOST_OFF;	//Current Current State Machine State
static ILL_CTL_CURRENT_STATES NextCurrentState = BOOST_OFF;		//Next State for Current Controller, used for timing
static ILL_CTL_LIGHT_STATES FlashOutHighNextStateChange; 		//Next state when the Flash Out Line Changes to High
static ILL_CTL_LIGHT_STATES FlashOutLowNextStateChange;  		//Next state when the Flash Out Line Changes to Low

static bool AimerState = 0;				//AIMON Signal, from Scan Driver
static bool IlluminationState = 0;		//ILLON Signal, from Scan Driver
static bool FlashOutState = 0;			//HW Line from Imager to synchronize exposure
static UINT LedControlState = 0;		//The Control State of the LEDs, 0-3, from Scan Driver
// NOTE:  FlashControlState's values have different meaning from LightsPresentState which uses ILL_CTL_CURRENT_STATES.
// Values for the FlashControlState variable:
#define FLASH_CTRL_ST_LOW	0
#define FLASH_CTRL_ST_HIGH	1
#define FLASH_CTRL_ST_MED	2

static UINT FlashControlState = 0;		//Flag in the Control State value, to indicate High Current Value  //ewr00003  bool
static bool Boost_Enable = 0;			//On-Off Control of the LED circuit
static bool BeeperBlocksAimer = 0;		//Beeper Blocks Aimer but keeps state.
static bool IllumBlocksAimer = 0;		//Illumination Blocks Aimer
static bool Led_Circuit_is_Enabled = 0;	//Indacates the LED Circuit pin state, according to the Illumination System
static bool Beeper_Circuit_EN = 0;		//Add extra Delay to state change to allow for full charge because Beeper took control of LED Circuit EN pin
static bool IlluminationTimerIsRunning = false;	//Flag to indicate the timer is running for control mode 4
static bool TESTIllumnFlag = false;		//Flag which indicates TSTLON is enabled. 
static UINT SavedLedControlState = 0;	//Saved version of the LED Control State to update when the timer expires 

//Hardware Access Defines
/* Note:  The Industrial schematic renamed some of these lines as they go to the PSOC:
MX25_RED_ILL_CTRL (src code calls it ILL_CTRL)  -->  PSOC_LED_CURRENT_LOW
LED_LVL_MED_DR  -->   PSOC_LED_CURRENT_MEDIUM
LED_LVL_HIGH_DR  -->  PSOC_LED_CURRENT_HIGH
*/

// The following gpio lines are meant to be in either output low or high impedance states (schematic names):  LED_CIRCUIT_EN, MX25_RED_ILL_CTRL, LED_LVL_MID, LED_LVL_HIGH, nHIGH_BOOST.
// We get high impedance by making them inputs.  The code puts a 0 in the data register and then toggles the direction register from input to output.
// LED_CIRCUIT_EN goes to STDN pin of boost converter chip
#define LED_CIRCUIT_EN(state)	do{WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT13_LSH,GPIO_BIT13_WID,state);}while(0)		//GPIO - LED_CIRCUIT_EN - FEC_TX_CLK - GPIO3_13
#define AIM_CTRL(state)			do{WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,state);}while(0)		//GPIO - AIM_CTRL - UART2_CTS - GPIO4_29
// nHIGH_BOOST adjusts the reference voltage of the boost converter chip to set its output voltage to 5V or 10V.
//ken@2011.12.15 there is no BOOST for HI2D
#ifdef HI2D_T
#define HIGH_BOOST(state)
#else
#define HIGH_BOOST(state)		do{WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,state);}while(0)	//GPIO - nHIGH_BOOST - UART2_RTS - GPIO4_28	Low = High Current, Input = Low Current
#endif
// The 3 current level gpios (MX25_RED_ILL_CTRL, LED_LVL_MID, LED_LVL_HIGH) select one resistor from a bank of 3 that sets the current level of the LEDs
#define ILL_CTRL_DIR(state)	    	do{WRITE_FIELD_TO_REG(GPIO1_GDIR0,GPIO_BIT5_LSH,GPIO_BIT5_WID,state);}while(0)		//GPIO - MX25_RED_ILL_CTRL - GPIO_F - GPIO1_5
#define ILL_CTRL_DR(state)	    	do{WRITE_FIELD_TO_REG(GPIO1_DR0,GPIO_BIT5_LSH,GPIO_BIT5_WID,state);}while(0)		//GPIO - MX25_RED_ILL_CTRL - GPIO_F - GPIO1_5
#define LED_LVL_HIGH_DIR(state)		do{WRITE_FIELD_TO_REG(GPIO2_GDIR0,GPIO_BIT30_LSH,GPIO_BIT30_WID,state);}while(0)	//GPIO - LED_LVL_HIGH - KPP_ROW1 - GPIO2_30	High = High Current, Input = Low Current
#define LED_LVL_HIGH_DR(state)		do{WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT30_LSH,GPIO_BIT30_WID,state);}while(0)		//GPIO - LED_LVL_HIGH - KPP_ROW1 - GPIO2_30
#define LED_LVL_MED_DIR(state)		do{WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT25_LSH,GPIO_BIT25_WID,state);}while(0)	//GPIO - LED_LVL_MID - RW - GPIO3_25
#define LED_LVL_MED_DR(state)		do{WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT25_LSH,GPIO_BIT25_WID,state);}while(0)		//GPIO - LED_LVL_MID - RW - GPIO3_25

//#define LED_LVL(state)			do{LED_LVL_DIR(state);	LED_LVL_DR(state);}while(0) 								//Note: Both the Output and Direction need to be changed for a high output

#define FLASHOUT_PINSTATUS()			(READ_FIELD(GPIO3_PSR0,GPIO_BIT23_LSH,GPIO_BIT23_WID))							//GPIO - FLASH_OUT - ECB - GPIO3_23
#define CLEAR_FLASHOUT_INT_Signal()		*((volatile unsigned long *)(GPIO3_ISR)) = (MOD_BIT(23)) 						//Clears the Interrupt Status Register Flag for the FLASH_OUT Signal
#define READ_FLASHOUT_INT_Signal()		(READ_FIELD(GPIO3_ISR,GPIO_BIT23_LSH,GPIO_BIT23_WID))							//Reads the Interrupt Status Register Flag for the FLASH_OUT Signal
#define FLASHOUT_INT_CONFIG(state)		do{WRITE_FIELD_TO_REG(GPIO3_ICR2,GPIO_ICR23_LSH,GPIO_ICR23_WID,state);}while(0)	//Sets the Interrupt Configuration Field for the FLASH_OUT Signal
#define READ_FLASHOUT_INT_CONFIG()		(READ_FIELD(GPIO3_ICR2,GPIO_ICR23_LSH,GPIO_ICR23_WID))							//Reads the Interrupt Control Field to determin rising or falling edge
#define FLASHOUT_INT_CONTROL(state)		do{WRITE_FIELD_TO_REG(GPIO3_IMR,GPIO_BIT23_LSH,GPIO_BIT23_WID,state);}while(0)	//Enable or Disables the FLASH_OUT interrupt bit

#define LASER_SHUTOFF(state)			do{WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT5_LSH,GPIO_BIT5_WID,!state);}while(0)	//GPIO - LASER_SHUTOFF - D15 - GPIO4_5	High = off
#define PSOC_AIM_CTRL(state)			do{WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT12_LSH,GPIO_BIT12_WID,state);}while(0)	//GPIO - SCLK_AIM_ON - FEC_RX_DEV - GPIO3_12
#define PSOC_PWR_ENA(state)				do{WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,state);}while(0)	//GPIO - PSOC_PWR_ENA - KPP_ROW0 - GPIO2_29

/* Bit field flags */
#define   MOD_BIT(a)				  ((unsigned long)0x00000001 << (a))
#define CLK_BIT										0
#define DAT_BIT										1
#define IOMUXC_SW_MUX_CTL_PAD_I2C_CLK			IOMUXC_SW_MUX_CTL_PAD_GPIO_A
#define IOMUXC_SW_MUX_CTL_PAD_I2C_DAT			IOMUXC_SW_MUX_CTL_PAD_GPIO_B
#define IOMUXC_SW_PAD_CTL_PAD_I2C_CLK			IOMUXC_SW_PAD_CTL_PAD_GPIO_A
#define IOMUXC_SW_PAD_CTL_PAD_I2C_DAT			IOMUXC_SW_PAD_CTL_PAD_GPIO_B

#define IIC_STABLE_TIME								7                             //mSec timer for psoc stable, 4.9mS total measured on scope, 3mS from psoc low power set then I2C latches low for additional 1.9 ms
#define GPIO_MUX_SETTING							((unsigned long)0x00000000)

//JADE power, set pad to cmos and 100k pull down
#define CONFIG_JADE_POWER_PAD_TO_CMOS_W_PD				*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_KPP_ROW2)) = (unsigned long)( MOD_BIT(13) | MOD_BIT(7) | MOD_BIT(6) )

//JADE power, set pad to gpio
#define CONFIG_JADE_POWER_PAD_TO_GPIO						*((volatile unsigned long *)(IOMUXC_SW_MUX_CTL_PAD_KPP_ROW2)) = 0x00000005

//set JADE power data register to 1 to turn on power (out of reset)
#define SET_JADE_POWER_DR_REG_HIGH							(*((volatile unsigned long *)(GPIO2_DR0))) |= ((unsigned long)(MOD_BIT(31)))

//set JADE power data register to 0 to turn off power (into reset)
#define SET_JADE_POWER_DR_REG_LOW							(*((volatile unsigned long *)(GPIO2_DR0))) &= ~((unsigned long)(MOD_BIT(31)))

//set JADE power direction register to output
#define SET_JADE_POWER_DATA_DIR_OUTPUT						(*((volatile unsigned long *)(GPIO2_GDIR0))) |= ((unsigned long)(MOD_BIT(31)))

//set All open drain with 47k pull up for I2C
#define ENABLE_ALL_OUTPUT_RESISTORS	\
										do{	\
										*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_I2C_CLK)) = (unsigned long)( MOD_BIT(13) | MOD_BIT(7) | MOD_BIT(6) | MOD_BIT(4) | MOD_BIT(3) );		\
	 									*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_I2C_DAT)) = (unsigned long)( MOD_BIT(13) | MOD_BIT(7) | MOD_BIT(6) | MOD_BIT(4) | MOD_BIT(3) );		\
										}while(0)

//select port daisy chain for i2c3 clock
#define SET_I2C_CLK_DAISY							SET_I2C3_CLK_DAISY
#define SET_I2C3_CLK_DAISY							*((volatile unsigned long *)(IOMUXC_I2C3_IPP_SCL_IN_SELECT_INPUT)) = ((unsigned long)0x00000001)

//select port daisy chain for i2c3 data
#define SET_I2C_DAT_DAISY							SET_I2C3_DAT_DAISY
#define SET_I2C3_DAT_DAISY							*((volatile unsigned long *)(IOMUXC_I2C3_IPP_SDA_IN_SELECT_INPUT)) = ((unsigned long)0x00000002)

//Set_Clock_and_Data_Port_for_GPIO
#define SET_CLK_DATA_PORT_MODE_GPIO		\
										do{		\
										*((volatile unsigned long *)(IOMUXC_SW_MUX_CTL_PAD_I2C_CLK)) = GPIO_MUX_SETTING;	\
	   								*((volatile unsigned long *)(IOMUXC_SW_MUX_CTL_PAD_I2C_DAT)) = GPIO_MUX_SETTING;	\
										}while(0)

//set clock and data direction to input
#define SET_CLK_DATA_DIR_INPUT		(*((volatile unsigned long *)(GPIO1_GDIR0))) &= ~((unsigned long)(MOD_BIT(CLK_BIT) | MOD_BIT(DAT_BIT)))
//Is_Clock_High
#define IS_CLK_HIGH					(( (*((volatile unsigned long *)(GPIO1_PSR0))) & ((unsigned long)(MOD_BIT(CLK_BIT))) )!=FALSE)
//Is_Data_High
#define IS_DATA_HIGH					(( (*((volatile unsigned long *)(GPIO1_PSR0))) & ((unsigned long)(MOD_BIT(DAT_BIT))) )!=FALSE)
//HI2D illmuination control struct init
static HI2D_illumination_control HI2D_illum_ctrl = {
	0
	//.HI2D_flash = NULL,
	//.HI2D_aimer = NULL,
	//.HI2D_current = NULL,
	//.scanAimerIllumMode = HI2D_AIM_ALT,
	//.HI2D_LedCtrlMode = HI2D_GPIO,
};
//parameter: ledCtrlMode need to be HI2D_GPIO or HI2D_PWM 
static void turnon_flash(ScanAimerIllumMode_t  mode, bool state )
{
	//unsigned int duty_cyc = 1000;//GET_SETTING(LedLevel);//0 is off, 1000 is high all the times
	//unsigned int current = 0;//1,2,3,4
	//enterHi2dCurrentLevel(GET_SETTING(CurrentLevel));//ken debug
	//enterHi2dCurrentLevel(GET_SETTING(NonOpticalTrigLedIntensityPercentage));
	//theStdInterface.printf("flash\r\n");//ken dbg
	LedCtrlMode_t ledCtrlMode = ((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_LedCtrlMode;
	if(HI2D_AIM_OFF == mode || HI2D_AIM_ALT == mode || HI2D_AIM_BOTH == mode)
	{
		if(state)
		{
			if (GET_SETTING(illumON)==TRUE)
			{
				if(HI2D_PWM == ledCtrlMode)
				{
					PWM_Sample(PWM3,1000);
					PWM_Start(PWM3);
					//AIM_CTRL(0);
					//xputstring("N1\r\n");
				}
				else
				{
					LED_CIRCUIT_EN(1);
					//AIM_CTRL(0);
					//xputstring("N2\r\n");
				}
			}
		}
		else
		{
			if(HI2D_PWM == ledCtrlMode)
			{
				PWM_Sample(PWM3,1000);
				PWM_Stop(PWM3);
				if(HI2D_AIM_OFF != mode)
				{
					AIM_CTRL(1);
					//xputstring("F1\r\n");
				}
			}
			else
			{
				LED_CIRCUIT_EN(0);
				if(HI2D_AIM_OFF != mode)
				{
					AIM_CTRL(1);
					//xputstring("F2\r\n");
				}
			}
		}
	}
	else
	{
		if(HI2D_PWM == ledCtrlMode)
		{
			PWM_Sample(PWM3,1000);
			PWM_Stop(PWM3);
		}
		else
		{
			LED_CIRCUIT_EN(0);
		}
	}
}
static void turnoff_flash(ScanAimerIllumMode_t mode, bool state)
{
	LedCtrlMode_t ledCtrlMode = ((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_LedCtrlMode;
	if(HI2D_PWM == ledCtrlMode)
	{
		PWM_Sample(PWM3,1000);
		PWM_Stop(PWM3);
	}
	else
	{
		LED_CIRCUIT_EN(0);
	}
}
static void turnon_aimer(ScanAimerIllumMode_t mode )
{
#if defined(SM3110) || defined(SM3100) || defined(SM5130)
	if(!((GET_SETTING(bTriggerAimerSync) == FALSE)&&(GET_SETTING(bAimerIndependOn) == TRUE)))
#endif
	{
		if( HI2D_AIM_ALT == mode || HI2D_AIM_BOTH == mode || HI2D_AIM_CONTINUOUS == mode)
			AIM_CTRL(1);
		else
			AIM_CTRL(0);
	}
}
static void turnoff_aimer(ScanAimerIllumMode_t mode )
{
#if defined(SM3110) || defined(SM3100)|| defined(SM5130)
	if(!((GET_SETTING(bTriggerAimerSync) == FALSE)&&(GET_SETTING(bAimerIndependOn) == TRUE)))
#endif
	{
		AIM_CTRL(0);
	}
}
static void enterHi2dLowCurrentMode()
{
	return;//TODO
}
static void enterHi2dHighCurrentMode()
{
	return;//TODO
}
//unsigned int current_map[5] = {150, 150, 50, 15, 7};
/*
		Hi2D illumination current level table
	-----------------------------------------	
	|Current |  nhigh_boost | nhigh_lad | pwm_lad |
	|   mA    |                    |                |               |
	-----------------------------------------
	|   150    |      Hi-z        |     Hi-z     |    high     |
	-----------------------------------------
	|    50     |      low         |     Hi-z     |    high     |
	-----------------------------------------
	|    15     |      Hi-z        |     low      |    high     |
	-----------------------------------------
	|    7       |      Hi-z        |     low      |    low      |
	-----------------------------------------
*/
static void enterHi2dCurrentLevel(unsigned int level)
{
	//unsigned int current;
	DISABLE_INTERRUPTS();
	
	switch(level)
	{
		case ILLUM_LED_LEVL4://level 1-->level4
			//DISABLE_INTERRUPTS();
			//nhigh_boost to hi-z
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,GPIO_LOW);
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,GPIO_GDIR_INPUT);//Hi-z STATE

			//nhigh_lad to hi-z
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT26_LSH,GPIO_BIT26_WID,GPIO_LOW);
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT26_LSH,GPIO_BIT26_WID,GPIO_GDIR_INPUT);//Hi-z STATE

			//pwm_lad to 1
			PWM_Sample(PWM4,1000);
			PWM_Start(PWM4);
			//RESTORE_INTERRUPTS();
			break;
		case ILLUM_LED_LEVL3://level 2-->level3
			//DISABLE_INTERRUPTS();
			//nhigh-boost to 0
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,GPIO_LOW);
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,GPIO_GDIR_OUTPUT);//to 0
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,GPIO_LOW);

			//nhigh_lad to hi-z
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT26_LSH,GPIO_BIT26_WID,GPIO_LOW);
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT26_LSH,GPIO_BIT26_WID,GPIO_GDIR_INPUT);//Hi-z STATE

			//pwm_lad to 1
			PWM_Sample(PWM4,1000);
			PWM_Start(PWM4);
			//RESTORE_INTERRUPTS();
			break;
		case ILLUM_LED_LEVL2://level 3-->level2
			//DISABLE_INTERRUPTS();
			//nhigh_boost to hi-z
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,GPIO_LOW);
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,GPIO_GDIR_INPUT);//Hi-z STATE

			//nhigh_lad to 0
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT26_LSH,GPIO_BIT26_WID,GPIO_LOW);
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT26_LSH,GPIO_BIT26_WID,GPIO_GDIR_OUTPUT);//to 0
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT26_LSH,GPIO_BIT26_WID,GPIO_LOW);
	
			//pwm_lad to 1
			PWM_Sample(PWM4,1000);
			PWM_Start(PWM4);
			//RESTORE_INTERRUPTS();
			break;
		case ILLUM_LED_LEVL1://level 4 -->level1
			//DISABLE_INTERRUPTS();
			//nhigh_boost to hi-z
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,GPIO_LOW);
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,GPIO_GDIR_INPUT);//Hi-z STATE

			//nhigh_lad to 0
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT26_LSH,GPIO_BIT26_WID,GPIO_LOW);
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT26_LSH,GPIO_BIT26_WID,GPIO_GDIR_OUTPUT);//to 0
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT26_LSH,GPIO_BIT26_WID,GPIO_LOW);

			//pwm_lad to 0
			PWM_Sample(PWM4,0);
			PWM_Start(PWM4);
			//RESTORE_INTERRUPTS();
			break;
		default://level 1
			//DISABLE_INTERRUPTS();
			//nhigh_boost to hi-z
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,GPIO_LOW);
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,GPIO_GDIR_INPUT);//Hi-z STATE

			//nhigh_lad to hi-z
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT26_LSH,GPIO_BIT26_WID,GPIO_LOW);
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT26_LSH,GPIO_BIT26_WID,GPIO_GDIR_INPUT);//Hi-z STATE

			//pwm_lad to 1
			PWM_Sample(PWM4,1000);
			PWM_Start(PWM4);
			//RESTORE_INTERRUPTS();
			break;
	}
	RESTORE_INTERRUPTS();
	
}
static void Hi2dIllumPinInit(void)
{
	DISABLE_INTERRUPTS();
	//GPIO - nHIGH_BOOST - UART2_RTS - GPIO4_28
	//Set as Input = Low Current
	//ken add @2011.12.15 pinmux to GPIO
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART2_RTS,MUX_MODE_5);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART2_RTS,0x00000007);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,GPIO_GDIR_INPUT);//Hi-z STATE

	//ken dbg: 4 level illumination for MKT request. Use PWM4, GPIO4 26, GPIO4 28 to control the current
	//pinmux Contrast to PWM4
	PWM_Init(PWM4,CONTRAST,23);
	PWM_Period(PWM4,998);//24M/(998 + 2) = 1K
	PWM_Sample(PWM4,0x0);

	//pinmux nHigh_lad set to GPIO
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART2_RXD, MUX_MODE_5);							//MUX To GPIO
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART2_RXD,0x00000007);							//PAD Regiser, MAX Drive Strength, Fast SLEW Rate, No Pullups
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT26_LSH,GPIO_BIT26_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT26_LSH,GPIO_BIT26_WID,GPIO_GDIR_INPUT);//Hi-z STATE

	//GPIO - LED_CIRCUIT_EN - FEC_TXCLK - GPIO3_13 - PWM3
	//If used for PWM to open and close the illumination, need to set the IO to GPIO3 13. Ohterwise set to PWM3
	if(HI2D_PWM == ((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_LedCtrlMode)//use for PWM
	{
		//UINT  ILL_PWM_PERIOD;
		//ILL_PWM_PERIOD = 998;	
		PWM_Init(PWM3,FEC_TX_CLK,23);//24M/24=1M
		PWM_Period(PWM3,998);//1M/(998 + 2) = 1K
		PWM_Sample(PWM3,0x0);
		
	}
	else if(HI2D_GPIO == ((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_LedCtrlMode)//use for GPIO
	{
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_FEC_TX_CLK,MUX_MODE_5);							//MUX To GPIO
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_FEC_TX_CLK,0x00000007);							//PAD Regiser, MAX Drive Strength, Fast SLEW Rate, No Pullups
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT13_LSH,GPIO_BIT13_WID,GPIO_LOW);			//Data Register, Initilize low to turn OFF
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT13_LSH,GPIO_BIT13_WID,GPIO_GDIR_OUTPUT);	//Direction Register
	}
	else
	{
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_FEC_TX_CLK,MUX_MODE_5);							//MUX To GPIO
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_FEC_TX_CLK,0x00000007);							//PAD Regiser, MAX Drive Strength, Fast SLEW Rate, No Pullups
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT13_LSH,GPIO_BIT13_WID,GPIO_LOW);			//Data Register, Initilize low to turn OFF
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT13_LSH,GPIO_BIT13_WID,GPIO_GDIR_OUTPUT);	//Direction Register
	}
	
	
	//GPIO - AIM_CTRL - UART2_CTS - GPIO4_29
	//Controls the 2 RED Aimer LEDs; High = LEDS ON, Low = LEDS OFF
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART2_CTS,MUX_MODE_5);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART2_CTS,0x00000007);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,GPIO_GDIR_OUTPUT);

	RESTORE_INTERRUPTS();
}
void *get_HI2D_illum_crl()
{
	return &HI2D_illum_ctrl;
}
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
//HI2D's illumination control is different form xenon's.
	//theStdInterface.printf("Ill_Ctl_Sys_Init_Thread:0x%x\r\n",HardwareToken.InterfaceToken());

	if(HardwareToken.IAmVoyager1400())
	{
	//init the illum control struct with the way below:
	//theStdInterface.printf("Ill_Ctl_Sys_Init_Thread\r\n");
		HI2D_illum_ctrl.HI2D_LedCtrlMode = HI2D_GPIO;//if use PWM to control the illumination pls modify it to HI2D_PWM
		HI2D_illum_ctrl.scanAimerIllumMode = HI2D_AIM_ALT;//default aimer and flash both on
		HI2D_illum_ctrl.HI2D_flash.turnon = turnon_flash;
		HI2D_illum_ctrl.HI2D_flash.turnoff = turnoff_flash;
		HI2D_illum_ctrl.HI2D_aimer.turnon = turnon_aimer;
		HI2D_illum_ctrl.HI2D_aimer.turnoff = turnoff_aimer;
		HI2D_illum_ctrl.HI2D_current.enter_high = enterHi2dHighCurrentMode;
		HI2D_illum_ctrl.HI2D_current.enter_low = enterHi2dLowCurrentMode;
		HI2D_illum_ctrl.HI2D_current.enterHi2dCurrentLevel = enterHi2dCurrentLevel;		
		Hi2dIllumPinInit();
	}
	else
	{
		Ill_HW_Pin_Initialization();
		Psoc_HW_Pin_Initialization();

		if (Illumination_Light_Event == 0x0)
		{
			Illumination_Light_Event = MT_CreateEvent_Ex(false, true);		//Create an Auto Resetting Event, to signal a change in the Illumination Controller State Machine
		}

		if (Illumination_Current_Event == 0x0)
		{
			Illumination_Current_Event = MT_CreateEvent_Ex(false, true);	//Create an Auto Resetting Event, to signal a change in the Current Controller State Machine
		}

		if (Illumination_FlashOut_Event == 0x0)
		{
			Illumination_FlashOut_Event = MT_CreateEvent_Ex(false, true);	//Create an Auto Resetting Event, to signal a change in the Flash Out Line
		}
    
		if (Aimer_On_Event == 0x0)
    		{
        		Aimer_On_Event = MT_CreateEvent_Ex(false, true);   //Create an Auto Resetting Event, to signal that the Aimer has just turned on
   		 }

		Ill_HW_Flash_Interrupt_Init();

		if ( illuminationcontrol_thread == 0x0)
		{
			illuminationcontrol_thread = MT_OpenDriverThread(ILLCONTROL_PRIORITY,ILLCONTROL_STACK_SIZE,&Ill_Ctl_Sys_Thread,NULL);
			WakeDriverThread(illuminationcontrol_thread);		//Run the Thread
		}
	}

}	//endof Ill_Ctl_Sys_Init_Thread

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
	if (illuminationcontrol_thread != 0x0)
	{
		Disable_Boost();
		while(CurrentPresentState != BOOST_OFF); 	//Wait for the HW to shutdown

		MT_CloseEvent(Illumination_Light_Event);
		MT_CloseEvent(Illumination_Current_Event);
		MT_CloseEvent(Illumination_FlashOut_Event);
        MT_CloseEvent(Aimer_On_Event);
		MT_CloseDriverThread(illuminationcontrol_thread);
	}
}	//endof Ill_Ctl_Sys_Destroy


// ===  Function  ======================================================================
//
//         Name:  SetFlashLedControlState
//
//  Description:
//				 SetFlashLedControlState - Sets the LED Control State for flash control,
//					by setting the high and low flags used in the flash interupt.
//					Also turns on LED for first frame Illumination if SDRTIM is still running.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode	   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in: 		void
//    return: 		void
// -------------------------------------------------------------------------------------
void SetFlashLedControlState( void )
{
	bool FlashOutStateLocal = 0;			//State of HW Line from Imager to synchronize exposure

	FlashOutStateLocal = FLASHOUT_PINSTATUS();		//Read exposure control line from imager

	//Led Control is FlashOut, controls Led and Aimer .
	//theStdInterface.printf("SetFlashLedControlState: %d\r\n",LedControlState);
	if( (LedControlState == FLASHOUT_CONTROL) )
	{
		FlashOutHighNextStateChange = ILLUMINATION_ON;
		FlashOutLowNextStateChange = AIMER_ON;

		if( FlashOutStateLocal )       //If High, turn the LEDs on, imager must already be running due to "SDRTIM", so we Illuminate first frame
		{
			if( Ill_Ctl_Sys_StateChangeToILLON() );
			{
				FlashOutState = FlashOutStateLocal;		//Only updated here before Interupt or Lights Thread is ran.
			}
		}
	}
	//Led Control is FlashOut, controls Led only
	else if( (LedControlState == FLASHOUT_ILLUM_CONTROL) )
	{
		FlashOutHighNextStateChange = ILLUMINATION_ON;
		FlashOutLowNextStateChange = LIGHTS_OFF;

		if( FlashOutStateLocal )       //If High, turn the LEDs on, imager must already be running due to "SDRTIM", so we Illuminate first frame
		{
			if( Ill_Ctl_Sys_StateChangeToILLON() );
			{
				FlashOutState = FlashOutStateLocal;		//Only updated here before Interupt or Lights Thread is ran.
			}
		}
	}
	//Led Control is FlashOut, controls Aimer only
	else if( LedControlState == FLASHOUT_AIMER_CONTROL )
	{
		FlashOutHighNextStateChange = LIGHTS_OFF;
		FlashOutLowNextStateChange = AIMER_ON;
	}
	//Led is Controled with FlashOut and Timer
	else if( (LedControlState == ILLUM_TIMER_CONTROL) || (LedControlState == ILLUM_AIMER_TIMER_CONTROL) )
	{
		FlashOutHighNextStateChange = LIGHTS_OFF;
		FlashOutLowNextStateChange = LIGHTS_OFF;
	}
	//Lights off
	else
	{
		FlashOutHighNextStateChange = LIGHTS_OFF;
		FlashOutLowNextStateChange = LIGHTS_OFF;
	}
}

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
/*  Note:  the following was reverse engineered from the code:
    LCState is a bitmask with the following bit assignments:
		FlashControlState (2 bits)  |  LedControlState (4 bits)

		FlashControlState values:  0, 1 (high), 2 (med)
*/
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in: UINT	(SET)		LCState			New LED Control State value
//    in-out:
//       out:
//    return: nothing
// -------------------------------------------------------------------------------------
#define BOOST_BIT 0x00000020

void SetLedControlState(UINT LCState)
{	
	if(IlluminationTimerIsRunning == false)
	{
		SavedLedControlState = LCState;
		
#ifdef _SMART_SCAN_DRIVER_FLASH_INTEN_CNTRL		//pass intensity control to scan driver
		UINT NewFlashControlState = LCState & 0x000000F0;	//Mask off the Flash Boost mode Bit

		PRINT_DEBUG(" Cont State Sets Boost %d \r\n", NewFlashControlState);
#endif

		bool bRanIlluminationCurrentEvent = false;
		LedControlState = LCState & 0x0000000F;	//Mask off the Flash Bit

		PRINT_DEBUG(" Set Cont State %d \r\n", LedControlState);
	
		if(LedControlState == ILLON_AIMON_CONTROL)          //No Flash Out control
		{
			DisableInterruptNumber(VEC_GPIO3_INT);
			FLASHOUT_INT_CONTROL(ILL_OFF);				//Turn Off the Flash_Out Interrupt
			CLEAR_FLASHOUT_INT_Signal();              //Clear the Flash_Out Interrupt
			EnableInterruptNumber(VEC_GPIO3_INT);		//Disable the FLASH Out Int but not all GPIO3 Interrupts

	                Ill_Ctl_Sys_StateChangeToOFF();
            
#ifdef _SMART_SCAN_DRIVER_FLASH_INTEN_CNTRL		//pass intensity control to scan driver
//ewr00002
#if 1
			if( pTrigger->GetScanDriverHasFlashLedControl() )	//if scan driver has control of intensity, change boost. If we have control, do not change intensity on a lights off request!			
			{				
				if (HardwareToken.IAmTwoLevelIllumination())				
				{					
					if( (NewFlashControlState & BOOST_BIT) != 0 )	//if set to boost set					
					{						
						bRanIlluminationCurrentEvent = SetILL_FlashMode(1);					
					}					
					else         //else normal lights					
					{						
						bRanIlluminationCurrentEvent = SetILL_FlashMode(0);					
					}		
				}				
				else				
				{					
					UINT new_current = (NewFlashControlState >> 4) & 3;	
//					dbg_output("set Led mode 1--%d\r\n", new_current);//ewr00003 debug
					if (new_current > 2)						
						new_current = 2;					
					bRanIlluminationCurrentEvent = SetILL_FlashMode(new_current);				
				}			
			}			
			else    //if scan driver does not have smart control of intensity,  do not change intensity to boost high on a lights off request!  We do it in trigger!			
			{				
				if (HardwareToken.IAmTwoLevelIllumination())				
				{					
					if( (NewFlashControlState & BOOST_BIT) == 0 )	//if set to boost not set				
					{						
						bRanIlluminationCurrentEvent = SetILL_FlashMode(0);				
					}				
				}				
				else				
				{					
					UINT new_current = (NewFlashControlState >> 4) & 3;				
					if (new_current >= 3)						
						new_current = 2;					
					UINT old_current = GetILL_FlashMode();
					if(new_current==1)  //ewr00005
						new_current=2;
					else if (new_current==2)  //ewr00005
						new_current=1;
//					dbg_output("set Led mode 2--%d, %d\r\n", new_current, old_current);//ewr00003 debug
					if (new_current < old_current)	//ewr00007				
					{						
						bRanIlluminationCurrentEvent = SetILL_FlashMode(new_current);					
					}				
				}			
			}
#endif
//	
#endif

			if( (Illumination_Light_Event != 0x0) && (bRanIlluminationCurrentEvent == false) )		//if not already running the thread in a Illumination_Current_Event (first event case runs all cases in thread)
			{
				MT_SetEvent(Illumination_Light_Event);
			}
		}
		else             //Flash Out control
		{
			DisableInterruptNumber(VEC_GPIO3_INT);
			SetFlashLedControlState();
			CLEAR_FLASHOUT_INT_Signal();            //Clear the Flash_Out Interrupt
			FLASHOUT_INT_CONTROL(ILL_ON);				//Enable the Flash_Out Interrupt
			EnableInterruptNumber(VEC_GPIO3_INT);

#ifdef _SMART_SCAN_DRIVER_FLASH_INTEN_CNTRL		//pass intensity control to scan driver
//ewr00002
#if 1
			if (HardwareToken.IAmTwoLevelIllumination())			
			{				
				if( (NewFlashControlState & BOOST_BIT) != 0 )	//if set to boost set				
				{					
					bRanIlluminationCurrentEvent = SetILL_FlashMode(1);			
				}			
				else      //else normal lights				
				{					
					bRanIlluminationCurrentEvent = SetILL_FlashMode(0);			
				}			
			}			
			else			
			{				
				UINT new_current = (NewFlashControlState >> 4) & 3;			
				if (new_current > 2)						
					new_current = 2;
//					dbg_output("set Led mode 3--%d\r\n", new_current);//ewr00003 debug
					if(new_current==0)   //ewr00005
						bRanIlluminationCurrentEvent = SetILL_FlashMode(0);
					else if (new_current==1)   //ewr00005
						bRanIlluminationCurrentEvent = SetILL_FlashMode(2);
					else if (new_current==2)  //ewr00005
						bRanIlluminationCurrentEvent = SetILL_FlashMode(1);
//				bRanIlluminationCurrentEvent = SetILL_FlashMode(new_current);	//ewr00005	
			}
#endif
//
#endif
		}
	}
	else
	{
		SavedLedControlState = LCState | RUN_LEDCONTROL_FLAG;
	}
}	//endof SetLedControlState


UINT GetLedControlState(void)
{
	return LedControlState;
}	//endof GetLedControlState

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
//        in: UINT  (SET)		FlashControlBit		FlashControlState value to be set
//    in-out:
//       out:
//    return: BOOL  (GET)						Returned value of FlashControlState
// -------------------------------------------------------------------------------------
bool SetILL_FlashMode(UINT FlashControlBit)     //ewr00003  bool
{
	bool bChangedState = false;
	if(HardwareToken.IAmVoyager1400())
	{
		return true;//HI2D 
	}
	else
	{
		if((FlashControlBit != FlashControlState) || (TESTIllumnFlag && (FlashControlState != 0x0)))
		{
			if((FlashControlBit==2)&&(HardwareToken.IAmTwoLevelIllumination())) //ewr00003
				bChangedState = false;			
			else       //ewr00003
			{
				if(TESTIllumnFlag)
				{
					FlashControlState = 0x0;
				}
				else
				{
					FlashControlState = FlashControlBit;
				}

				PRINT_DEBUG(" Set Inten H-L %d \r\n", FlashControlState);	

				if(Illumination_Current_Event != 0x0)
				{
					MT_SetEvent(Illumination_Current_Event);
				}

				bChangedState = true;
			}
		}

		return bChangedState;
	}
}	//endof SetILL_FlashMode

UINT GetILL_FlashMode(void)    //ewr00003
{
	return FlashControlState;
}	//endof GetILL_FlashMode

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

		PRINT_DEBUG(" Set Aim State %d \r\n", AState);

		if(Illumination_Light_Event != 0x0)
		{
			MT_SetEvent(Illumination_Light_Event);
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
//								  to change the Illumination State, this is not supported
//								  when in High Current Mode, and will not turn the LEDs ON
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

		PRINT_DEBUG(" Set Illum State %d \r\n", IState);

		if(Illumination_Light_Event != 0x0)
		{
			MT_SetEvent(Illumination_Light_Event);
		}
	}
}	//endof SetILL_ONState

bool GetILL_ONState(void)
{
	return IlluminationState;
}	//endof GetILL_ONState

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
void IllTestMode(bool TestMode, bool AimIllFunction)
{
	TESTIllumnFlag = TestMode;
	SetILL_FlashMode(ILL_OFF);
	SetILL_ONState(TestMode && AimIllFunction);
	SetAIM_ONState(TestMode && !AimIllFunction);
}	//endof IllTestMode

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
// -------------------------------------------------------------------------------------
bool GetFlashOutState(void)
{
	return FlashOutState;
}	//endof GetFlashOutState

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
void Enable_Boost(void)
{
	Boost_Enable = ILL_ON;

	PRINT_DEBUG(" Enable Boost \r\n");

	if(GET_CONFIG_SETTING(AimerType) == AIMER_PSOC_LASER)
	{
		PSOC_PWR_ENA(ILL_ON);
	}
	
	IllumBlocksAimer = 0;
	
	if(Illumination_Current_Event != 0x0)
	{
		MT_SetEvent(Illumination_Current_Event);
	}
}	//endof Enable_Boost

void Disable_Boost(void)
{
	Boost_Enable = ILL_OFF;
	NextCurrentState = BOOST_OFF;

	PRINT_DEBUG(" Disable Boost \r\n");

	if(GET_CONFIG_SETTING(AimerType) == AIMER_PSOC_LASER)
	{
		PSOC_PWR_ENA(ILL_OFF);
	}
	Ill_HW_Disable_Aimer(FUNC_ILLUM);
	
	if(Illumination_Current_Event != 0x0)
	{
		MT_SetEvent(Illumination_Current_Event);
	}
}	//endof Disable_Boost

// ===  Function  ======================================================================
//
//         Name:  IsLightCurrentStateChangedToHigh
//
//  Description: Allows other functions to wait on the Illumination System to change
//					current states.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   	Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:		bool										True if in high boost mode, false other wise
// -------------------------------------------------------------------------------------
bool IsLightCurrentStateChangedToHigh(void)
{
	bool	stateCorrect = false;

//	if( HIGH_CURRENT == CurrentPresentState )    //ewr00003
	if( (HIGH_CURRENT == CurrentPresentState)||(MID_CURRENT==CurrentPresentState) )   //ewr00003
	{
		stateCorrect = true;
	}

	return stateCorrect;
}	//endof IsLightCurrentStateChangedToHigh



/*----------------------------------------------------------------------------------------
HandleCurrenEvent

  Code for handling the EVENT_IDX_CURRENT associated with the Illumination_Current_Event
  event variable.

  Modifies these global variables:  NextCurrentState; Beeper_Circuit_EN and CurrentPresentState
  via calls to Ill_Ctl_Sys_StateChangeToHIGH_CURRENT(), Ill_Ctl_Sys_StateChangeToLOW_CURRENT(),
  and Ill_Ctl_Sys_StateChangeToMID_CURRENT().


  NOTE:  the meanings of the values of CapturedFlashControlState/FlashControlState variables differ from
  ThisCurrentPresentState!  See description of FlashControlState above.
  Parameters:  
				ThisCurrentPresentState[in]:  the present state (value) of the current level (a single read of CurrentPresentState)

				CapturedFlashControlState[in]: a single read of the FlashControlState which is the illumination level (0 - 2) SEE NOTE above!

  Returns:  true if token says we're Industrial.
----------------------------------------------------------------------------------------*/
void HandleCurrenEvent(ILL_CTL_CURRENT_STATES ThisCurrentPresentState, UINT ThisCapturedFlashControlState)
{
//				---------------- Current Controller State Machine ----------------
//				Y1 = (Boost_Enable && CapturedFlashControlState)
//
//				Y2 = (!Boost_Enable || (Boost_Enable && !CapturedFlashControlState))
//
//				Y3 = (BOOST_Enable && !CapturedFlashControlState)
//
//				Y4 = (!Boost_Enable || (Boost_Enable && CapturedFlashControlState))

	switch(ThisCurrentPresentState)
	{
		case BOOST_OFF:
			if(Boost_Enable && (ThisCapturedFlashControlState==1))//Y1    //ewr00004
			{
				NextCurrentState = HIGH_CURRENT;
				Ill_Ctl_Sys_StateChangeToHIGH_CURRENT(1);	//Enter LED high power mode
				XPUT_STRING_DEBUG( " A \r\n" );       //lmc   debug
			}
			else if(Boost_Enable && !ThisCapturedFlashControlState)//Y3
			{
				NextCurrentState = LOW_CURRENT;
				Ill_Ctl_Sys_StateChangeToLOW_CURRENT(1);	//Enter LED low power mode (delay)
				XPUT_STRING_DEBUG( " B \r\n" );       //lmc   debug
			}
			else if(Boost_Enable && (ThisCapturedFlashControlState==2))   //ewr00003 fixme
			{
				NextCurrentState = MID_CURRENT;
				Ill_Ctl_Sys_StateChangeToMID_CURRENT(1);	//Enter LED mid power mode
				XPUT_STRING_DEBUG( " A-MID \r\n" );       //lmc   debug
			}						
			else
			{
				NextCurrentState = BOOST_OFF;
				XPUT_STRING_DEBUG( " C \r\n" );       //lmc   debug
			}
			break;

		case LOW_CURRENT:
			if(!Boost_Enable || (Boost_Enable && ThisCapturedFlashControlState)) //Y4
			{
				Ill_Ctl_Sys_StateChangeToBOOST_OFF();	//turn boost off
				XPUT_STRING_DEBUG( " D \r\n" );       //lmc   debug
			}
			break;

		case HIGH_CURRENT:
			if(!Boost_Enable)//Y2
			{
				Ill_Ctl_Sys_StateChangeToBOOST_OFF();	//turn boost off
				XPUT_STRING_DEBUG( " E \r\n" );       //lmc   debug
			}
			if(Boost_Enable && !ThisCapturedFlashControlState)
			{
				NextCurrentState = LOW_CURRENT;
				Ill_Ctl_Sys_StateChangeToLOW_CURRENT(0);	//Enter LED low power mode (delay)
				XPUT_STRING_DEBUG( " F \r\n" );       //lmc   debug
			}
			if(Boost_Enable && (ThisCapturedFlashControlState==2))    //ewr00007
			{
				NextCurrentState = MID_CURRENT;
				Ill_Ctl_Sys_StateChangeToMID_CURRENT(0);	//Enter LED mid power mode
				XPUT_STRING_DEBUG( " G \r\n" );       //lmc   debug
			}
			break;
			
		case MID_CURRENT:        //ewr00003
			if(!Boost_Enable)//Y2
			{
				Ill_Ctl_Sys_StateChangeToBOOST_OFF();	//turn boost off
				XPUT_STRING_DEBUG( " E-MID \r\n" );       //lmc   debug
			}
			if(Boost_Enable && !ThisCapturedFlashControlState)    //ewr00003 fixme
			{
				NextCurrentState = LOW_CURRENT;
				Ill_Ctl_Sys_StateChangeToLOW_CURRENT(0);	//Enter LED low power mode (delay)
				XPUT_STRING_DEBUG( " F-MID \r\n" );       //lmc   debug
			}
			if(Boost_Enable && (ThisCapturedFlashControlState==1))    //ewr00007
			{
				NextCurrentState = HIGH_CURRENT;
				Ill_Ctl_Sys_StateChangeToHIGH_CURRENT(0);	//Enter LED high power mode
				XPUT_STRING_DEBUG( " J \r\n" );       //lmc   debug
			}
			break;

	} //endof switch
}


// ===  Function  ======================================================================
//
//  Name:  Ill_Ctl_Sys_Thread
//
//  Description:  Main Thread that runs the Illumination System on the iMX25
//				Runs when the one of the three events is triggered, and will change states based
//				on the inputs set before the event runs.
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
	int Eventsignal = 0;
	UINT CapturedLedControlState = 0;
	bool CapturedAimerState = 0;
	bool CapturedIlluminationState = 0;
	UINT CapturedFlashControlState = 0;   //ewr00003
	UINT CellPhoneIlluminationTime = 0;
// Event Indeces used in Illumination_System_Events 
#define EVENT_IDX_CURRENT	0
#define EVENT_IDX_ILL_LIGHT	1
#define EVENT_IDX_ILL_FLO	2
#define EVENT_IDX_AIMER		3
    
	MT_Event_t Illumination_System_Events[] = {Illumination_Current_Event,Illumination_Light_Event,Illumination_FlashOut_Event,Aimer_On_Event};

	if((GET_SETTING(CblSelNumber)==CBL_USB))
	{
		//Wait for USB to Enumerate and the beeper is finished before starting the boost
		while(ERR_FAIL == (theStdInterface.DeviceIoControl(IOCTRL_AM_I_ENUMERATED)))
		{
			MT_Delay(1);//Wait till after Enumeration to stay below 100mA
		}
		while(true == IsBeepInProgress())
		{
			MT_Delay(1);//Wait till after the beeper to stay below 500mA
		}

		Enable_Boost();

	}
	else
	{
		Enable_Boost();	//Enable the Boost on Startup.
	}

	while(TRUE)
	{
		Eventsignal = MT_WaitMultiEvents(sizeof(Illumination_System_Events)/sizeof(MT_Event_t),Illumination_System_Events,INFINITE);

		//Freeze the Inputs incase something changes them before the event is over
		CapturedAimerState = AimerState;
		CapturedIlluminationState = IlluminationState;
		CapturedLedControlState = LedControlState;
		CapturedFlashControlState = FlashControlState;

		switch(Eventsignal)
		{
			case EVENT_IDX_CURRENT: //Illumination_Current_Event
				HandleCurrenEvent(CurrentPresentState, CapturedFlashControlState);

				//Note: No break:  also updates the Light Controller State Machine

			case EVENT_IDX_ILL_LIGHT:	//Illumination_Light_Event

				//--Update System Functions --
				switch(LightsPresentState)
				{
					case LIGHTS_OFF:
						//---Update State Machine---

						//Boost set to low current and Illumnination State On
						if(!(CurrentPresentState == BOOST_OFF) && (CapturedIlluminationState && !(CurrentPresentState == HIGH_CURRENT)))
						{
							Ill_Ctl_Sys_StateChangeToILLON();
							XPUT_STRING_DEBUG( " 1 \r\n" );       //lmc   debug
						}
						//Boost set to low or high current and Illumnination State Off and Aimer State On
						else if(!(CurrentPresentState == BOOST_OFF) && (!CapturedIlluminationState && CapturedAimerState))
						{
							Ill_Ctl_Sys_StateChangeToAIMON();
							XPUT_STRING_DEBUG( " 2 \r\n" );       //lmc   debug
						}
						break;

					case ILLUMINATION_ON:
						//---Update State Machine---

						//Boost set to off or (Illumnination State off and Aimer State Off)
						if(CurrentPresentState == BOOST_OFF || (!CapturedIlluminationState && !CapturedAimerState))
						{
							Ill_Ctl_Sys_StateChangeToOFF();
							XPUT_STRING_DEBUG( " 3 \r\n" );       //lmc   debug
						}
						//Boost set to low or high current and Illumnination State Off and Aimer State On
						else if(!(CurrentPresentState == BOOST_OFF) && (!CapturedIlluminationState && CapturedAimerState))
						{
							Ill_Ctl_Sys_StateChangeToAIMON();
							XPUT_STRING_DEBUG( " 4 \r\n" );       //lmc   debug
						}
						break;

					case AIMER_ON:
						//---Update State Machine---

						//Boost set to off or (Illumnination State off and Aimer State Off)
						if((CurrentPresentState == BOOST_OFF) || (!CapturedIlluminationState && !CapturedAimerState))
						{
							Ill_Ctl_Sys_StateChangeToOFF();
							XPUT_STRING_DEBUG( " 5 \r\n" );       //lmc   debug
						}
						//Boost set to low current and Illumnination State On
						else if(!(CurrentPresentState == BOOST_OFF) && (CapturedIlluminationState && !(CurrentPresentState == HIGH_CURRENT)))
						{
							Ill_Ctl_Sys_StateChangeToILLON();
							XPUT_STRING_DEBUG( " 6 \r\n" );       //lmc   debug
						}
						break;

				} //endof LightsPresentState switch

//				---------------- Update Flags for Next Flash Out Event ----------------

				//Uses the same transition formulas as the Light Controller State Machine but
				//	broken up to decide what happens when Flash_Out goes High or Low

				//Led Control is FlashOut controls Led and Aimer .
				if( (CapturedLedControlState == FLASHOUT_CONTROL) )
				{
					FlashOutHighNextStateChange = ILLUMINATION_ON;
					FlashOutLowNextStateChange = AIMER_ON;
					XPUT_STRING_DEBUG( " a \r\n" );       //lmc   debug
				}
				//Led Control is FlashOut controls Led only
				else if( (CapturedLedControlState == FLASHOUT_ILLUM_CONTROL) )
				{
					FlashOutHighNextStateChange = ILLUMINATION_ON;
					FlashOutLowNextStateChange = LIGHTS_OFF;
					XPUT_STRING_DEBUG( " b \r\n" );       //lmc   debug
				}
				//Led Control is FlashOut controls Aimer only
				else if(CapturedLedControlState == FLASHOUT_AIMER_CONTROL)
				{
					FlashOutHighNextStateChange = LIGHTS_OFF;
					FlashOutLowNextStateChange = AIMER_ON;
					XPUT_STRING_DEBUG( " c \r\n" );       //lmc   debug
				}
				//Lights off
				else
				{
					FlashOutHighNextStateChange = LIGHTS_OFF;
					FlashOutLowNextStateChange = LIGHTS_OFF;
					XPUT_STRING_DEBUG( " d \r\n" );       //lmc   debug
				}
				
				break;
				
			case EVENT_IDX_ILL_FLO:	//Illumination_FlashOut_Event
				if(((CapturedLedControlState == ILLUM_TIMER_CONTROL) || (CapturedLedControlState == ILLUM_AIMER_TIMER_CONTROL)) && !(CurrentPresentState == BOOST_OFF))
				{   
                    //Change the on time of the illumination based on the exposure of the illumination frame
                    //This will reduce flicker in the Illumination LEDs                
                    if((pTrigger->GetTriggerMode()==TRGMOD_MANUAL) || (pTrigger->GetTriggerMode()==TRGMOD_TEMP_MANUAL))
                    {
                        CellPhoneIlluminationTime = (GET_SETTING(FixedExposure) * 125);
                    }
                    else if(pTrigger->GetTriggerMode()==TRGMOD_STREAM_PRESENT)
                    {
                        CellPhoneIlluminationTime = (GET_SETTING(PresentationFixedExposure) * 125);
                    }
                    
                    IlluminationTimerIsRunning = true;
                    Ill_Ctl_Sys_StateChangeToILLON();
					Timer3_MicroSecondDelay(CellPhoneIlluminationTime);	//Sleep
					IlluminationTimerIsRunning = false;
					
					CapturedLedControlState = LedControlState; //Get Control State again incase it was updated while we were sleeping
					
					//Determine the illumination state after the lights have turned off 			
					if(CapturedLedControlState == ILLUM_AIMER_TIMER_CONTROL)
					{
						Ill_Ctl_Sys_StateChangeToAIMON();
					}
					else if(CapturedLedControlState == ILLUM_TIMER_CONTROL)
					{
						Ill_Ctl_Sys_StateChangeToOFF();
					}
					else
					{
						if((SavedLedControlState & RUN_LEDCONTROL_FLAG) != 0x0)
						{
							SetLedControlState(SavedLedControlState);
						}
						else
						{
							if(Illumination_Light_Event != 0x0)
							{
								MT_SetEvent(Illumination_Light_Event);
							}
						}
					}
				}
				break;
			
            case EVENT_IDX_AIMER: //Aimer_On_Event
                //Only allow the Aimer to be on a specific number of mS to reduce Aimer Flicker
                 
                if(((GET_SETTING(ScanDriverCellPhone)>=1) && ((pTrigger->GetTriggerMode()==TRGMOD_MANUAL) || (pTrigger->GetTriggerMode()==TRGMOD_TEMP_MANUAL))) || ((GET_SETTING(PresentationScanDriverCellPhone)>=1) && (pTrigger->GetTriggerMode()==TRGMOD_STREAM_PRESENT)))
                {
                    int active_cell_mode=0;
                    if((pTrigger->GetTriggerMode()==TRGMOD_MANUAL || pTrigger->GetTriggerMode()==TRGMOD_TEMP_MANUAL))
                    {
                    	active_cell_mode=GET_SETTING(ScanDriverCellPhone);
                    }
                    else /* no else if because the top if only lets us fall in for manual, temp manaul or streaming pres trigger */
                    {
                    	active_cell_mode=GET_SETTING(PresentationScanDriverCellPhone);
                    }
                    switch(active_cell_mode)
                    {
                    	case 1:
                    	case 2:
                    	case 3:
                    		Timer3_MicroSecondDelay(1700); /*1.7ms, if Scandriver changes expsoure for these modes, this number needs to change!!!!!!!!*/
                    		break;
                    	case 4:
                    		Timer3_MicroSecondDelay(5000); /*5ms, if Scandriver changes expsoure for these modes, this number needs to change!!!!!!!!*/
                    		break;
                    	default:
                    		break;
                    }


                    if(LightsPresentState == AIMER_ON)
                    {
                        Ill_Ctl_Sys_StateChangeToOFF();
                    }            
                }
            	break;	
                	
			case WAIT_TIMED_OUT:
				//The Wait is INFINITE so we should not get here
				break;

			default:
				//We only get here if there was an error
				break;

		} //endof Eventsignal switch
	} //endof while(TRUE)
}	//endof Ill_Ctl_Sys_Thread

// ===  Function  ======================================================================
//
//         Name:  Ill_Ctl_Sys_StateChangeToILLON
//
//  Description: Toggles the GPIO needed to turn on the Illumination LEDs and change
//					the state to ILLUMINATIONON.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   	Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:		bool										True if Illum On was successful, false other wise
// -------------------------------------------------------------------------------------
static bool Ill_Ctl_Sys_StateChangeToILLON(void)
{
	unsigned int lightsOn = 0;

	if(GET_CONFIG_SETTING(AimerType) == AIMER_RED_LED)
	{
		if(!HardwareToken.IAmVoyager1400())//ken TODO
			AIM_CTRL(ILL_OFF);
		LightsPresentState = LIGHTS_OFF;
	}
	else if(GET_CONFIG_SETTING(AimerType) == AIMER_PSOC_LASER)
	{
		PSOC_AIM_CTRL(ILL_OFF);
		LightsPresentState = LIGHTS_OFF;
	}

#ifndef XENON_LITE_1500
	if(CurrentPresentState == HIGH_CURRENT)
	{
		lightsOn = 2;
	}
	else if(CurrentPresentState == LOW_CURRENT)
	{
		lightsOn = 1;
	}
	else if(CurrentPresentState == MID_CURRENT)
	{
		if(!(HardwareToken.IAmTwoLevelIllumination()))  
			lightsOn = 3;
		else
			lightsOn = 1; //If Mid is not supported go to Low Current	
	}
#else
	if(GET_SETTING(FactoryTestMode))
	{
		if(CurrentPresentState == HIGH_CURRENT)
		{
			lightsOn = 2;
		}
		else if(CurrentPresentState == LOW_CURRENT)
		{
			lightsOn = 1;
		}
		else if(CurrentPresentState == MID_CURRENT)
		{
			if(!(HardwareToken.IAmTwoLevelIllumination()))  
				lightsOn = 3;
			else
				lightsOn = 1; //If Mid is not supported go to Low Current	
		}
	}
	else
	{
		lightsOn = 1;
	}
#endif

	if(lightsOn)
	{
//		ILL_CTRL(ILL_ON);
		if(!(HardwareToken.IAmTwoLevelIllumination()))
		{
			if(lightsOn == 2) //HIGH_CURRENT
			{
				ILL_CTRL_DIR(ILL_OFF);
				LED_LVL_MED_DIR(ILL_OFF);
				LED_LVL_HIGH_DIR(ILL_ON);
				LED_LVL_HIGH_DR(ILL_ON);			
			}
			else if (lightsOn == 3) //MID_CURRENT
			{
				ILL_CTRL_DIR(ILL_OFF);
				LED_LVL_MED_DIR(ILL_ON);
				LED_LVL_MED_DR(ILL_ON);
				LED_LVL_HIGH_DIR(ILL_OFF);
			}
			else //LOW_CURRENT
			{
				ILL_CTRL_DIR(ILL_ON);
				ILL_CTRL_DR(ILL_ON);
				LED_LVL_MED_DIR(ILL_OFF);
				LED_LVL_HIGH_DIR(ILL_OFF);
			}		
		}
		else
		{
			if(lightsOn == 2) //HIGH_CURRENT
			{
				ILL_CTRL_DIR(ILL_ON);
				ILL_CTRL_DR(ILL_ON);
				LED_LVL_HIGH_DIR(ILL_ON);
				LED_LVL_HIGH_DR(ILL_ON);			
			}
			else //LOW_CURRENT
			{
				ILL_CTRL_DIR(ILL_ON);
				ILL_CTRL_DR(ILL_ON);
				LED_LVL_HIGH_DIR(ILL_OFF);
				LED_LVL_HIGH_DR(ILL_OFF);
			}		
		}

		LightsPresentState = ILLUMINATION_ON;
	}

	//PRINT_DEBUG(" Illum On CurPresSt:%d LiOn:%d  \r\n", CurrentPresentState, lightsOn );

	return (bool)lightsOn;
}	//endof Ill_Ctl_Sys_StateChangeToILLON

// ===  Function  ======================================================================
//
//         Name:  Ill_Ctl_Sys_StateChangeToAIMON
//
//  Description: Toggles the GPIO needed to turn on the Aimers on and change the state
//					to AIMERON.
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

	if(!(HardwareToken.IAmTwoLevelIllumination()))
	{
		ILL_CTRL_DIR(ILL_OFF);
		LED_LVL_MED_DIR(ILL_OFF);
		LED_LVL_HIGH_DIR(ILL_OFF);
	}
	else
	{
		ILL_CTRL_DR(ILL_OFF);
		ILL_CTRL_DIR(ILL_ON);
		LED_LVL_HIGH_DIR(ILL_OFF);
		LED_LVL_HIGH_DR(ILL_OFF);	
	}


	if(CurrentPresentState != BOOST_OFF)
	{
		if((BeeperBlocksAimer == 0) && (IllumBlocksAimer == 0))  //Aimer is allowed to turn on when noone is blocking it
		{
			if(GET_CONFIG_SETTING(AimerType) == AIMER_RED_LED)
			{
				AIM_CTRL(ILL_ON);
			}
			else if(GET_CONFIG_SETTING(AimerType) == AIMER_PSOC_LASER)
			{
				PSOC_AIM_CTRL(ILL_ON);
			}
            
            if(Aimer_On_Event != 0x0)
            {
                MT_SetEvent(Aimer_On_Event);
            }
		}
	LightsPresentState = AIMER_ON;
	}
}	//endof Ill_Ctl_Sys_StateChangeToAIMON


// ===  Function  ======================================================================
//
//         Name:  Ill_Ctl_Sys_StateChangeToOFF
//
//  Description: Toggles the GPIO needed to turn the lights off and change the state
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
	if(!(HardwareToken.IAmTwoLevelIllumination()))
	{
		ILL_CTRL_DIR(ILL_OFF);
		LED_LVL_MED_DIR(ILL_OFF);
		LED_LVL_HIGH_DIR(ILL_OFF);
	}
	else
	{
		ILL_CTRL_DR(ILL_OFF);
		ILL_CTRL_DIR(ILL_ON);
		LED_LVL_HIGH_DIR(ILL_OFF);
		LED_LVL_HIGH_DR(ILL_OFF);	
	}

if( (AimOnstatus!=GetAIM_ONState())||(AimOnstatus==0))  
{	
	if(GET_CONFIG_SETTING(AimerType) == AIMER_RED_LED)
	{
		AIM_CTRL(ILL_OFF);
	}
	else if(GET_CONFIG_SETTING(AimerType) == AIMER_PSOC_LASER)
	{
		PSOC_AIM_CTRL(ILL_OFF);
	}
}
	#ifdef VUQUEST3310
		EXT_ILL_CTRL(ILL_OFF);
	#endif
	if(HardwareToken.IAmVoyager1400()) 
	{
		if(HI2D_PWM == ((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_LedCtrlMode)
		{
			//unsigned int duty_cyc = 1000;//GET_SETTING(LedLevel);
			PWM_Sample(PWM3,1000);
			PWM_Stop(PWM3);
		}
		else if(HI2D_GPIO == ((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_LedCtrlMode)
		{
			LED_CIRCUIT_EN(ILL_OFF);//close when state changed, HI2D use it temporarily.TODO
		}
		else
		{
			LED_CIRCUIT_EN(ILL_OFF);
		}
	}

	LightsPresentState = LIGHTS_OFF;
}	//endof Ill_Ctl_Sys_StateChangeToAIMON

// ===  Function  ======================================================================
//
//         Name:  Ill_Ctl_Sys_StateChangeToLOW_CURRENT
//
//  Description: Toggles the GPIO needed to Enable Low Current Mode and change the state
//					to LOW_CURRENT, accounts for Hardware Timing needed to change to
//					the state
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	bool			StartupDelay		Delay to let the hardware charge the cap
//														not needed when going from high to low
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
static void Ill_Ctl_Sys_StateChangeToLOW_CURRENT(bool StartupDelay)
{
	Ill_HW_Disable_Aimer(FUNC_ILLUM);

	if(GET_CONFIG_SETTING(PrimaryIlluminationType)==0x3)
	HIGH_BOOST(ILL_HIGH_CURRENT); 
	else 
	HIGH_BOOST(ILL_LOW_CURRENT);  

	Ill_Ctl_Sys_LED_Cir_EN_CTRL(FUNC_ILLUM, ILL_ON);

	if(StartupDelay)
	{
		MT_Delay(1);
	}
	CurrentPresentState = LOW_CURRENT;
	Ill_HW_Enable_Aimer(FUNC_ILLUM);
}	//endof Ill_Ctl_Sys_StateChangeToLOW_CURRENT

// ===  Function  ======================================================================
//
//         Name:  Ill_Ctl_Sys_StateChangeToMID_CURRENT
//
//  Description: Toggles the GPIO needed to Enable Mid Current Mode and change the state
//					to MID_CURRENT if hardware token support three level current, if it not support three level current change the state to HIGH_CURRENT, 
//                                accounts for Hardware Timing needed to change to the state
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
static void Ill_Ctl_Sys_StateChangeToMID_CURRENT(bool boostDelay)     
{
//dbg_output("ChangeToMID\r\n");//ewr00003 debug
	Ill_HW_Disable_Aimer(FUNC_ILLUM);
#ifndef XENON_LITE_1500	// boost function is disabled for 1500
	HIGH_BOOST(ILL_HIGH_CURRENT);
#else
	if(GET_SETTING(FactoryTestMode))
	{
		HIGH_BOOST(ILL_HIGH_CURRENT);
	}
	else
	{
		HIGH_BOOST(ILL_LOW_CURRENT);
	}
#endif
	Ill_Ctl_Sys_LED_Cir_EN_CTRL(FUNC_ILLUM, ILL_ON);

	if(boostDelay)  
	{
		MT_Delay(30);
	
		if(Beeper_Circuit_EN ==true)	//Add addition delay to account for the beeper turning off the enable line
		{
			Beeper_Circuit_EN = false;
			MT_Delay(16);
		}
	}
	if(!(HardwareToken.IAmTwoLevelIllumination())) CurrentPresentState = MID_CURRENT;
	else CurrentPresentState = HIGH_CURRENT;	
	
	Ill_HW_Enable_Aimer(FUNC_ILLUM);
}	//endof Ill_Ctl_Sys_StateChangeToMID_CURRENT

// ===  Function  ======================================================================
//
//         Name:  Ill_Ctl_Sys_StateChangeToHIGH_CURRENT
//
//  Description: Toggles the GPIO needed to Enable High Current Mode and change the state
//					to HIGH_CURRENT, accounts for Hardware Timing needed to change to
//					the state
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
static void Ill_Ctl_Sys_StateChangeToHIGH_CURRENT(bool boostDelay)  
{
	Ill_HW_Disable_Aimer(FUNC_ILLUM);
#ifndef XENON_LITE_1500	// boost function is disabled for 1500
	HIGH_BOOST(ILL_HIGH_CURRENT);
#else
	if(GET_SETTING(FactoryTestMode))
	{
		HIGH_BOOST(ILL_HIGH_CURRENT);
	}
	else
	{
		HIGH_BOOST(ILL_LOW_CURRENT);
	}
#endif
	Ill_Ctl_Sys_LED_Cir_EN_CTRL(FUNC_ILLUM, ILL_ON);

	if(boostDelay)   
	{
		MT_Delay(30);
	
		if(Beeper_Circuit_EN ==true)	//Add addition delay to account for the beeper turning off the enable line
		{	// We were beeping while charging, thus we were disabling the charging at times (when the beeper is on) which will increase the total time of charge--account for this
			Beeper_Circuit_EN = false;
			MT_Delay(16);
		}
	}
	CurrentPresentState = HIGH_CURRENT;
	Ill_HW_Enable_Aimer(FUNC_ILLUM);
}	//endof Ill_Ctl_Sys_StateChangeToHIGH_CURRENT

// ===  Function  ======================================================================
//
//         Name:  Ill_Ctl_Sys_StateChangeToBOOST_OFF
//
//  Description: Toggles the GPIO needed to Disable the Boost and change the state
//					to BOOST_OFF, accounts for Hardware Timing needed to change to
//					the state
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
static void Ill_Ctl_Sys_StateChangeToBOOST_OFF(void)
{
	CurrentPresentState = BOOST_OFF;
	Ill_Ctl_Sys_LED_Cir_EN_CTRL(FUNC_ILLUM, ILL_OFF);

	MT_Delay(15);

	//Set the Event again to see if the state should be changed to normal or high
	if(Illumination_Current_Event != 0x0)
	{
		MT_SetEvent(Illumination_Current_Event);
	}

}	//endof Ill_Ctl_Sys_StateChangeToBOOST_OFF

// ===  Function  ======================================================================
//
//  Name:  Ill_HW_Pin_Initialization
//
//  Description:  Initial Configuration of GPIO Pins used by the Illumination System
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
static void Ill_HW_Pin_Initialization(void){

	DISABLE_INTERRUPTS();		//Disable Interrupts to write to the GPIO, used the Hazard Macros that do not disable Interrupts

	if(!(HardwareToken.IAmTwoLevelIllumination()))
	{
		//GPIO - LED_LVL_MID - RW - GPIO3_25
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_RW,MUX_MODE_5);									//MUX To GPIO
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_RW,0x00000000);									//PAD Regiser, No Pullups
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT25_LSH,GPIO_BIT25_WID,GPIO_LOW);			//Data Register, Initilize low to turn OFF
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT25_LSH,GPIO_BIT25_WID,GPIO_GDIR_INPUT);	//Direction Register
	}

	//GPIO - LED_CIRCUIT_EN - FEC_TXCLK - GPIO3_13 - PWM3
	//Enable for boost Circuit and Linear Regulator.
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_FEC_TX_CLK,MUX_MODE_5);							//MUX To GPIO
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_FEC_TX_CLK,0x00000007);							//PAD Regiser, MAX Drive Strength, Fast SLEW Rate, No Pullups
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT13_LSH,GPIO_BIT13_WID,GPIO_LOW);			//Data Register, Initilize low to turn OFF
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT13_LSH,GPIO_BIT13_WID,GPIO_GDIR_OUTPUT);	//Direction Register

	//GPIO - RED_ILL_CTRL - GPIO_F - GPIO1_5
	//Enables the Linear Regulator High = LEDS ON, Low = LEDS OFF
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_EXT_ARMCLK,MUX_MODE_0);							//MUX To GPIO set as zero b/c this is a deciated GPIO Pin (GPIO_F)
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_GPIO_F,0x00000007);								//PAD Regiser, MAX Drive Strength, Fast SLEW Rate, No Pullups
	HAZARD_WRITE_FIELD_TO_REG(GPIO1_DR0,GPIO_BIT5_LSH,GPIO_BIT5_WID,GPIO_LOW);				//Data Register, Initilize low to turn OFF
	HAZARD_WRITE_FIELD_TO_REG(GPIO1_GDIR0,GPIO_BIT5_LSH,GPIO_BIT5_WID,GPIO_GDIR_OUTPUT);	//Direction Register

	//GPIO - AIM_CTRL - UART2_CTS - GPIO4_29
	//Controls the 2 RED Aimer LEDs; High = LEDS ON, Low = LEDS OFF
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART2_CTS,MUX_MODE_5);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART2_CTS,0x00000007);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,GPIO_GDIR_OUTPUT);

	//GPIO - nHIGH_BOOST - UART2_RTS - GPIO4_28
	//Boost Circuit Output Voltage Control, Drive Low = High Current Set as Input = Low Current
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART2_RTS,MUX_MODE_5);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART2_RTS,0x00000007);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,GPIO_GDIR_INPUT);

	//GPIO - LED_LVL_HIGH - KPP_ROW1 - GPIO2_30
	//Linear Regulator Current Control, High = High Current, Input = Low Current
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_ROW1,MUX_MODE_5);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_KPP_ROW1,0x00000007);
	HAZARD_WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT30_LSH,GPIO_BIT30_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO2_GDIR0,GPIO_BIT30_LSH,GPIO_BIT30_WID,GPIO_GDIR_INPUT);

	//GPIO - FLASH_OUT - ECB - GPIO3_23
	//Input from Imager, indicates when the LEDs should turn on
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_ECB,MUX_MODE_5);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_ECB,0x00000000);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT23_LSH,GPIO_BIT23_WID,GPIO_GDIR_INPUT);

	//GPIO - LASER_SHUTOFF - D15 - GPIO4_5
	//Turns the Laser off when iMX25 is controling it
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_D15,MUX_MODE_5);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_D15,0x00000007);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT5_LSH,GPIO_BIT5_WID,GPIO_HIGH);		//High will turn off the laser
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT5_LSH,GPIO_BIT5_WID,GPIO_GDIR_OUTPUT);
	RESTORE_INTERRUPTS();

}	//endof Ill_HW_Pin_Initialization

// ===  Function  ======================================================================
//
//  Name:  Ill_HW_Flash_Interrupt_Init
//
//  Description:  Initial Setup of the Flash Out Interrupt
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
static void Ill_HW_Flash_Interrupt_Init(void){
	//Setup Interrupt, GPIO for the Flash Out Signal, The GPIO3 INT is setup here but the Bit mask is not set until we start scanning
	SetVector(GPIO3_Service_IRQ,VEC_GPIO3_INT);
	FLASHOUT_INT_CONTROL(ILL_OFF);								//Turn Off the Flash_Out Interrupt
	CLEAR_FLASHOUT_INT_Signal();										//Clear the Flag if it is set
	WRITE_FIELD_TO_REG(GPIO3_EDGE,GPIO_BIT23_LSH,GPIO_BIT23_WID,0x1);	//Set to INT on any EDGE, overrides the ICR2 REG SETTING
	EnableInterruptNumber(VEC_GPIO3_INT);  								// Enable interrupts GPIO Port 3

}	//endof Ill_HW_Flash_Interrupt_Init

// ===  Function  ======================================================================
//
//  Name:  GPIO3_Service_IRQ
//
//  Description:  General IRQ for all GPIO3 pins.
//				Changes the Light State Machine based on the Flash Flags and pin state.
//				Then sets the Illumination_FlashOut_Event to update the flags for the next event
//				The interrupt will fire on every edge of the FlashOut signal
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

	if(READ_FLASHOUT_INT_Signal())  //Check if the FlashOut Interrupt has been enabled
	{
		CLEAR_FLASHOUT_INT_Signal();
		FlashOutState = FLASHOUT_PINSTATUS();		//Only Place It is updated in Interupt or Lights Thread.

		if(FlashOutState)
		{
			if(FlashOutHighNextStateChange != LightsPresentState)
			{
				switch(FlashOutHighNextStateChange)
				{
					case LIGHTS_OFF:
						Ill_Ctl_Sys_StateChangeToOFF();
						//XPUT_STRING_DEBUG( " O \r\n" );       //lmc   debug
						break;

					case ILLUMINATION_ON:
						Ill_Ctl_Sys_StateChangeToILLON();
						//XPUT_STRING_DEBUG( " I \r\n" );       //lmc   debug
						break;

					case AIMER_ON:
						Ill_Ctl_Sys_StateChangeToAIMON();
						//XPUT_STRING_DEBUG( " Z \r\n" );       //lmc   debug
						break;

				} //endof FlashOutHighNextStateChange switch
			}
		}
		else
		{
			if(FlashOutLowNextStateChange != LightsPresentState)
			{
				switch(FlashOutLowNextStateChange)
				{
					case LIGHTS_OFF:
						Ill_Ctl_Sys_StateChangeToOFF();
						//XPUT_STRING_DEBUG( " o \r\n" );       //lmc   debug
						break;

					case ILLUMINATION_ON:
						Ill_Ctl_Sys_StateChangeToILLON();
						//XPUT_STRING_DEBUG( " i \r\n" );       //lmc   debug
						break;

					case AIMER_ON:
						Ill_Ctl_Sys_StateChangeToAIMON();
						//XPUT_STRING_DEBUG( " z \r\n" );       //lmc   debug
						break;

				} //endof FlashOutLowNextStateChange switch
			}
			
			//Update Status for ILLUM_TIMER_CONTROL and ILLUM_AIMER_TIMER_CONTROL modes
			if(Illumination_FlashOut_Event != 0x0)
			{
				MT_SetEvent(Illumination_FlashOut_Event);
			}
			
		} //endof Flash Out Low
	} //endof if Flash Out Interupt Enable

	return NULL;
}	//endof GPIO3_Service_IRQ



// ===  Function  ======================================================================
//
//  Name:  Psoc_HW_Pin_Initialization
//
//  Description:  Initial Configuration of GPIO Pins used by the Psoc for aimer control
//
//					Note:  Must be called before I2C and Scan Driver are initialized!!!!!
//
// - PARAMETERS -------------------------------------------------------------------------
//			Direction			Type             Name            Description
// -------------------------------------------------------------------------------------
//			passed:				void
//
//			return:				void
// -------------------------------------------------------------------------------------
static void Psoc_HW_Pin_Initialization(void)
{
	CTimeoutmS psoc_Timeout(IIC_STABLE_TIME);
	bool bPsocInitialize = false;
	bool bPsocStable = false;

	DISABLE_INTERRUPTS();		//Disable Interrupts to write to the GPIO, used the Hazard Macros that do not disable Interrupts

	//GPIO - Reset_PSoC/ENG_RESET - KPP_ROW3 - GPIO3_0
	//Resets the PSOC
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_ROW3,MUX_MODE_5);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_KPP_ROW3,0x00000007);		//high slew, max drive
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT0_LSH,GPIO_BIT0_WID,GPIO_HIGH);		//Put PSOC in Reset
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT0_LSH,GPIO_BIT0_WID,GPIO_GDIR_OUTPUT);

	//GPIO - PSOC_PWR_ENA - KPP_ROW0 - GPIO2_29
	// Allow PSOC to go into Low Power Mode, when set low
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_ROW0,MUX_MODE_5);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_KPP_ROW0,0x00000000);
	HAZARD_WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,GPIO_LOW);		//Set pwr ena to low power
	HAZARD_WRITE_FIELD_TO_REG(GPIO2_GDIR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,GPIO_GDIR_OUTPUT);

	//GPIO - SCLK_AIM_ON - FEC_RX_DEV - GPIO3_12
	// PSOC Aimer Control
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_FEC_RX_DV,MUX_MODE_5);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_FEC_RX_DV,0x00000000);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT12_LSH,GPIO_BIT12_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT12_LSH,GPIO_BIT12_WID,GPIO_GDIR_OUTPUT);

	//GPIO - MX25_SDATA_PSoC/ILL_ON - EXT_ARMCLK - GPIO3_15
	// PSOC Illumination Control, not used here for any design
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_EXT_ARMCLK,MUX_MODE_5);
//	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_EXT_ARMCLK,0x00000000);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT15_LSH,GPIO_BIT15_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT15_LSH,GPIO_BIT15_WID,GPIO_GDIR_OUTPUT);

	//Put jade in reset, has not been initialized yet
	CONFIG_JADE_POWER_PAD_TO_CMOS_W_PD;       //config gpio pad
	CONFIG_JADE_POWER_PAD_TO_GPIO;            //set pad to gpio
	SET_JADE_POWER_DR_REG_LOW;						//turn off power (in reset)
	SET_JADE_POWER_DATA_DIR_OUTPUT;          //configure pin as output

	//I2c not configured yet, so make it a gpio input
	SET_CLK_DATA_DIR_INPUT;			//turn off CLK and data as an output, set them to be inputs, to be pulled high
	ENABLE_ALL_OUTPUT_RESISTORS;	//enable output pull-up resistors
	SET_CLK_DATA_PORT_MODE_GPIO;	//set up clock and data ports for GPIO
	SET_I2C_CLK_DAISY;				//set pad daisy chain for i2c3 only
	SET_I2C_DAT_DAISY;           //set pad daisy chain for i2c3 only

 	//GPIO - Reset_PSoC/ENG_RESET - KPP_ROW3 - GPIO3_0
	//Resets the PSOC
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT0_LSH,GPIO_BIT0_WID,GPIO_LOW);		//Take the PSOC out of Reset
	HAZARD_WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,GPIO_HIGH);		

	RESTORE_INTERRUPTS();

//	RESET_WATCHDOG_TIMER();

	psoc_Timeout.SetExpiration( IIC_STABLE_TIME );

	//wait for psoc to run its initialization code
	do
	{
		if( psoc_Timeout.HasExpired() )
		{
			bPsocInitialize = true;
		}
	}
	while( !bPsocInitialize );

//	RESET_WATCHDOG_TIMER();

	psoc_Timeout.SetExpiration( IIC_STABLE_TIME );

	//wait for psoc to release I2C lines when it is stable
	do
	{
		if( IS_CLK_HIGH && IS_DATA_HIGH )
		{
			XPUT_STRING_DEBUG( " Psoc Stable \r\n" );       //lmc   debug
			bPsocStable = true;
		}

		if( psoc_Timeout.HasExpired() )
		{
			XPUT_STRING_DEBUG( " Psoc wait time out \r\n" );       //lmc   debug
			break;
		}
	}
	while( !bPsocStable );

	return;
}	//endof Psoc_HW_Pin_Initialization

// ===  Function  ======================================================================
//
//  Name:  Ill_HW_Disable_Aimer
//
//  Description: Forces the Aimer off.  The Aimer cannot be on while the beeper is beeping.
//			This is done to keep the Scanner Current under 500mA.  This Function turns the
//			Aimer off and blocks the State from turning it on with the BlockAimer Flags.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	bool			Function		Function calling the Ill_Ctl_Sys_LED_Cir_EN_CTRL function.
//													FUNC_ILLUM or FUNC_BEEPER
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void Ill_HW_Disable_Aimer(bool Function)
{
	if(Function == FUNC_ILLUM)
	{
		IllumBlocksAimer = true;
	}
	else
	{
		BeeperBlocksAimer = true;
	}

	if(GET_CONFIG_SETTING(AimerType) == AIMER_RED_LED)
	{
		AIM_CTRL(ILL_OFF);
	}
	else if(GET_CONFIG_SETTING(AimerType) == AIMER_PSOC_LASER)
	{
		PSOC_AIM_CTRL(ILL_OFF);
	}
}	//endof Ill_HW_Disable_Aimer

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
//        in:	bool			Function		Function calling the Ill_Ctl_Sys_LED_Cir_EN_CTRL function.
//													FUNC_ILLUM or FUNC_BEEPER
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void Ill_HW_Enable_Aimer(bool Function)
{
	if(Function == FUNC_ILLUM)
	{
		IllumBlocksAimer = false;
	}
	else
	{
		BeeperBlocksAimer = false;
	}

	if((BeeperBlocksAimer == 0) && (IllumBlocksAimer == 0))
	{
		LightsPresentState = LIGHTS_OFF;
		if(Illumination_Light_Event != 0x0)
		{
			MT_SetEvent(Illumination_Light_Event);
		}
	}
}	//endof Ill_HW_Enable_Aimer

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
void Ill_Ctl_Sys_LED_Cir_EN_CTRL (bool Function, bool EN_State)
{
	if(Function == FUNC_ILLUM)
	{
		Led_Circuit_is_Enabled = EN_State;
	}

	if(NextCurrentState != CurrentPresentState) // JimR:  I think this is overkill--we probably only need to look for the high current state (off to high)
	{
		if(EN_State == true)
		{
			if((Is_PWM_Signal_State_High() == false) && (Led_Circuit_is_Enabled == true))
			{
				LED_CIRCUIT_EN(ILL_ON);
			}
		}
		else
		{
			LED_CIRCUIT_EN(ILL_OFF);
		}

		if(Function == FUNC_BEEPER)
		{
			Beeper_Circuit_EN = true;	//Signal that time should be added to the state transition wait
		}
	}
	else
	{
		if(Function == FUNC_ILLUM)
		{
			LED_CIRCUIT_EN(EN_State);
		}
		else
		{
			LED_CIRCUIT_EN(Led_Circuit_is_Enabled);	//Don't let the beeper have control if there is no state change
		}
	}
}	//endof Ill_Ctl_Sys_LED_Cir_EN_CTRL



#endif //endof FLASH_LEDS


