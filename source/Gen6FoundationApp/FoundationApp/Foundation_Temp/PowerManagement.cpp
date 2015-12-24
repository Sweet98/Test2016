/**************************************************************************************/
//
// HIM 48X0G GENERATION 6 SOFTWARE
//
//  Date:       10/29/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Foundation_Temp/PowerManagement.cpp $
// $Revision: 1.80 $
// $Date: 2011/11/10 03:20:06EST $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// File:  PowerManagement.cpp
//
//  This module contains a collection of low level functions that control the
//		power management for the processor and scanner.
//
//
// This file replaces the power.c file for the MXL processor
//
//------------------------------------------------------------------------------
//#pragma O0		//Debug

#include "PowerManagement.h"
#include "HardwareMacros.h"		//Provides Macros for setting registers
#include "iMX25_CRM.h"			//Clocks Control
#include "imx25_armclkctl.h"
#include "db_time.h"
#include "iMX25_IOMUX.h"
#include "iMX25_GPIO.h"
#include "iMX25_USBOTG.h"
#include "menu.h"
#include "ReaderConfiguration.h"
#include "beep.h"
#include "ioman.h"
#include "RTC_Driver.h"
#include "interrupts.h"
#include "usage_statistics.h"
#include "timeout.h"
#include "mainhhplproc.h"
#include "IndicatorLEDs.h"
#include "HardwareToken.h"
#include "PluginGpioInterface.h"
#include "IndicatorTfb.h"
#include "iMX25_PWM.h"
#if defined(RFBASE)
#else

	#include "ScanDriverImagingInterface.h"
	#include "HWTrigger.h"
	
	#define	TRIGGER_5_5		1
	#ifdef TRIGGER_5_5
		#include "trigger_5_5.h"                                         //lmc   fixme
	#else
		#include "trigger.h"
	#endif	//end #ifdef TRIGGER_5_5

	#ifdef FLASH_LEDS
		#include	"Flash_Illumination_Control.h"
	#endif	//endof FLASH_LEDS

#endif // endof #if defined(RFBASE)

#if defined( RFSCAN )
#else
CTimeoutmS InterfaceLowPowerTimeout(GET_SETTING(RS232LowPowerTimeOut)*1000L);
#endif//endof defined( RFSCAN )

//Internal Variables
static int PMSavedIRQ = 0;
static int PMStopModeIRQ = 0;
static int Saved_INT_EN_Low = 0;
static int Saved_INT_EN_High = 0;
static bool bResetPwrDownTime = true;
static bool bSystemInSuspendMode = false;
static bool bSystemSuspended = false;

static ULONG SaveCRM_CGR0Reg = 0;
static ULONG SaveCRM_CGR1Reg = 0;
static ULONG SaveCRM_CGR2Reg = 0;
	
//Save GPIO Settings
static ULONG SaveGPIO1_DR0Reg = 0;
static ULONG SaveGPIO1_GDIR0Reg = 0;
static ULONG SaveGPIO2_DR0Reg = 0;
static ULONG SaveGPIO2_GDIR0Reg = 0;
static ULONG SaveGPIO3_DR0Reg = 0;
static ULONG SaveGPIO3_GDIR0Reg = 0;
static ULONG SaveGPIO4_DR0Reg = 0;
static ULONG SaveGPIO4_GDIR0Reg = 0;
#ifdef HI2D_T
static ULONG SaveIOMUXC_SW_MUX_CTL_PAD_GPIO_A = 0;
static ULONG SaveIOMUXC_SW_MUX_CTL_PAD_GPIO_B = 0;
static ULONG SaveIOMUXC_SW_MUX_CTL_PAD_CSI_MCLK = 0;
#endif
static CTimeout StopModeTimer;

#if defined(RFBASE)
#else
static int SavedTriggeringEnSetting = 0;
static int SavedMenuEnSetting = 0;
static int SavedImagerPowerSetting = 0;
static int SavedScanDriverTimeoutSetting = 0;
static int SavedShotgunSetting = 0;
static int SavedPreferredSetting = 0;
static int SavedSequenceSetting = 0;
static int SavedTrigmodeSetting = 0;
#endif //endof defined(RFBASE)

//Internal Defines
//LowPower Settings
#define		CRM_CGR0_LP_SETTING		0x0A0A0000
#define		CRM_CGR1_LP_SETTING		0x0400000C
#define		CRM_CGR2_LP_SETTING		0x00000450

#define		CRM_PMCR0_LP_SETTING	0x002C8028 		//Disable DVFS and DPTC
#define		CRM_PMCR0_RUN_SETTING	0x002C8028
#define		CRM_PMCR1_LP_SETTING	0x2FA02F00		//EMI Well Bias Enabled
#define		CRM_PMCR1_RUN_SETTING	0x0FA00F40		//Disable Well Bias
#define		CRM_PMCR2_LP_SETTING	0x0005A030		//Turn off 24 MHz OSC, VSTBY OFF, ARM Memory Off (0x0007A030 TEST CODE Turn on VSTBY)
#define		CRM_PMCR2_RUN_SETTING	0x0000A030		//Keep ARM MEM on in Doze Mode

//Allow These Interrupts to wake the processor
#define		INTENL_OEM_LP_SETTING	0x02010000		//Trigger (GPIO3), RTC
#define		INTENH_OEM_LP_SETTING	0x00002020		//USB, UART1
#define		INTENL_LP_SETTING		0x00800000		//Trigger (GPIO4)
#define		INTENH_LP_SETTING		0x00000020		//USB

//Hardware Defines
#define PS_MODE_CTRL(state)     	do{WRITE_FIELD_TO_REG(GPIO1_DR0,GPIO_BIT4_LSH,GPIO_BIT4_WID,state);}while(0)		//GPIO - PS_MODE - GPIO_E - GPIO1_4
#define SHUTDOWN_CTRL(state)		do{WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT11_LSH,GPIO_BIT11_WID,state);}while(0)		//GPIO - SHUTDOWN - FEC_RDATA1 - GPIO3_11
#define AIM_IN_STATE()		     	(READ_FIELD(GPIO2_PSR0,GPIO_BIT30_LSH,GPIO_BIT30_WID))		//GPIO - AIM_IN - KPP_ROW1 - GPIO2_30	

//Function Prototypes
static void GoInToPowerDownMode(void);
static power_t DetermineNextPowerMode(void);
static void Wake_Ctrl(bool LineState);


#if !defined(RFBASE)
bool IsWakeInputActive(void);
static void Imager_Shutdown_for_Suspend(void);
static void Imager_Startup_from_Suspend(void);
#endif //endof !defined(RFBASE)

// ===  Function  ======================================================================
//
//         Name:  IsWakeInputActive(void)
//
//  Description:
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
bool IsWakeInputActive(void)
{
	if(HardwareToken.IAmOEM())
	{
		if(true == DoesPluginHaveControlofKPP_ROW1())
		{
			return false;
		}
		else
		{
			//some simple debouncing and protection against noise
			int nWake = 0;
			const int NumTests =10;
			for (int i = 0; i<NumTests; i++)
			{
				if(!AIM_IN_STATE())      //we neeed to invert because the AIM definition defines the AIM, not the nWake.
				{
					nWake++;
				}
			}
	
			return (nWake == NumTests);
		}
	}
	else
	{
		return false;
	}
}

// ===  Function  ======================================================================
//
//         Name:  ResetMinimumPowerUp()
//
//  Description:
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void ResetMinimumPowerUp(void)
{
} //endof ResetMinimumPowerUp


// ===  Function  ======================================================================
//
//         Name:  ResetPowerTimeOuts()
//
//  Description:  This routine is also called by IOMAN whenever there is IO activity
// 				Resets both timers regardless of which triggermode we are actually
//				using
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void ResetPowerTimeOuts(void)
{

#if ! defined(RFBASE)
	if (pTrigger)
	{
		pTrigger->ResetLowPowerTimeout();	/* Only used in LOW_POWER trigger mode */
	}
#endif // endof #if ! defined(RFBASE)
#if defined( RFSCAN )
#else

	int lowpowerimeout = GET_SETTING(RS232LowPowerTimeOut)*1000L;

#endif
	
	if (!lowpowerimeout)
	{
		lowpowerimeout=100;								// 100 ms
	}
	
	InterfaceLowPowerTimeout.SetExpiration(lowpowerimeout);	/* only used in MANUAL_TRIGGER mode */
} //endof ResetPowerTimeOuts



// ===  Function  ======================================================================
//
//         Name:  ManageSystemPower()
//
//  Description:  Main system power managment function called from main loop
// controls which power state to enter and calls EnterPowerState
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void ManageSystemPower(void)
{
	bResetPwrDownTime = TRUE;
	
	power_t NextPowerSate = FULL;

	DisableInterrupts(PMSavedIRQ);  /*We want to disable interrupts before we check our system stuff. Most of these things are changed
							via an interrupt so we want them masked until we stop to avoid race condition */

	NextPowerSate = DetermineNextPowerMode();

	if(NextPowerSate != FULL)
	{
		if(NextPowerSate == DOZE)
		{
			EnterPowerState(NextPowerSate);	//Change the Power State Here
		}
		else
		{
			RestoreInterrupts(PMSavedIRQ);

			EnterPowerState(NextPowerSate);	//Change the Power State Here

			DisableInterrupts(PMSavedIRQ);
		}
	}

	if(bResetPwrDownTime)
	{
		ResetPowerTimeOuts();
	}

	RestoreInterrupts(PMSavedIRQ);

	return;
} //endof ManageSystemPower


// ===  Function  ======================================================================
//
//         Name:  DetermineNextPowerMode
//
//  Description: Each Version of Hardware has its own function please update for all versions
//		
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in: 
//    in-out:
//       out:
//    return: power_t							Lowest Power State the System can enter
// -------------------------------------------------------------------------------------
//------------------------------------------------
// Corded - DetermineNextPowerMode
//------------------------------------------------
#if defined( CORDED )
static power_t DetermineNextPowerMode(void)
{
	power_t ReturnedNextPowerSate = FULL;
	
	if (gOkToPowerDown && pTrigger && ProcessingAppCode == false)  /*the variable the script uses to tell us it is doing something processing intensive */
	{
		int trigmode = pTrigger->GetTriggerMode();

 		/*Don't go to STOP or PowerDown below if any of the following is true..Instead Go to Doze */
 		if ( pTrigger->IsHWorSerialTrig()
 				|| !(theStdInterface.IsLowPowerModeAllowed()) /* Does programmed interface allow power mode. Never changes for a given interface */
 				|| !theStdInterface.IsAllSent()
 				|| !IsBeepQueEmpty()  /*or if we have beeps if in the que */
				|| (trigmode != TRGMOD_MANUAL && trigmode != TRGMOD_MANUAL_LO)
 				|| GET_SETTING( KeepPowerOn )
 				||(AIM_IN_STATE()!=0)
			)
		{
			ReturnedNextPowerSate = DOZE;
		}
		else
		{
			bResetPwrDownTime = false;
			
			/*We do these check here instead of lumping it in above as these calls are not supper quick like the others.
			We only want to check these if all the other conditions above allow it */
			bool Is232lptActive =((theStdInterface.GetTerminalID() == TRM_ASYNC_ASCII)&& (GET_SETTING(RS232LowPowerTimeOut)!=0));
			bool lptdone = (Is232lptActive && InterfaceLowPowerTimeout.HasExpired());

			if( trigmode == TRGMOD_MANUAL_LO )
			{
				if (lptdone ||(theStdInterface.GetTerminalID() != TRM_ASYNC_ASCII))
				{
					ReturnedNextPowerSate = STOPTHENPOWERDOWN;
				}
				else
				{
					if (pTrigger->HasLowPowerTimeoutExpired() && !Is232lptActive)
					{
						ReturnedNextPowerSate = POWERDOWN;
					}
					else
					{
						ReturnedNextPowerSate = DOZE;
					}
				}
			}
			else if (lptdone || (theStdInterface.GetTerminalID() != TRM_ASYNC_ASCII))
			{
				ReturnedNextPowerSate = STOP;
			}
			else
			{
				ReturnedNextPowerSate = DOZE;
			}
		}
	}//endof if (gOkToPowerDown && pTrigger && ProcessingAppCode == false)
	
	return ReturnedNextPowerSate;
	
}	//endof CORDED DetermineNextPowerMode
//------------------------------------------------
// RF_SCAN - DetermineNextPowerMode
//------------------------------------------------
#endif

// ===  Function  ======================================================================
//
//         Name:  EnterPowerState(power_t PowerSate)
//
//  Description:  Sets the system in different power states
//
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  power_t             power        	Power state to enter
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void EnterPowerState(power_t PowerSate)
{
	switch (PowerSate)
	{
		case FULL:
			//Do nothing
			break;

		case DOZE:
		case DOZE_LOW:
			if ((*(volatile unsigned int *)ASIC_NIPNDH) | 
				(*(volatile unsigned int *)ASIC_NIPNDL) |  
				(*(volatile unsigned int *)ASIC_FIPNDH) | 
				(*(volatile unsigned int *)ASIC_FIPNDL) )
				{
					break;
				}
				
			WRITE_FIELD_TO_REG(CRM_CCTL,CRM_CCTL_LPCTL_LSH,CRM_CCTL_LPCTL_WID,LP_CTL_Wait_Mode);	//tell the CRM to enter wait mode when WFI has been issued
			WaitForIRQ();  											//Tell the Core to wait for interrupt
			break;

		case POWERDOWN:
			theStdInterface.DeviceIoControl(IOCTRL_PREPARE_FOR_LOW_POWER_STATE); // This is also done in POWERDOWN In case of fallback: STOP mode

			GoInToPowerDownMode();

			theStdInterface.DeviceIoControl(IOCTRL_RETURN_TO_NORMAL_POWER_STATE);
			ResetPowerTimeOuts();
			//NOTE: no break just enter STOP Mode

		case STOP:
			Prepare_System_for_Suspend();
			EnterStopMode();
			Restore_System_from_Suspend();
			break;

		case STOPTHENPOWERDOWN:
#if !defined(RFBASE)
			ResetPowerTimeOuts();
			PrepareRTC((pTrigger->GetLowPowerTimeoutRestTime())/1000);// Number of seconds to remain in stop mode
			Prepare_System_for_Suspend();
			EnterStopMode();	//Stop the PLL's and stuff. Wake up from irqs.
			Restore_System_from_Suspend();		
			bool bRtc = GetRTC_ALM_ISR_Status();	// Save RTC status, so we can clean it up now
			ClearRTC();	// Turn off all RTC irqs in case we fall into STOP mode. It would wake us up.
			ZeroRTC();			
			if (bRtc)	               // If we woke up by the RTC, it is time to turn off the power
			{
				GoInToPowerDownMode();
			}

#endif //endof !defined(RFBASE)
			break;
	} //endof switch (PowerSate)
}	//endof EnterPowerState

// ===  Function  ======================================================================
//
//         Name:  EnterStopMode
//
//  Description: Places the Processor in STOP Mode and restores the system when STOP 
//		mode is exited.  This function is not static because of USB Suspend.  Other 
//		functions should call EnterPowerState and NOT this function
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void EnterStopMode(void)
{
	DisableInterrupts(PMSavedIRQ);
	
	
	//Turn off Indicators
	ErrorLedOff();
	GoodReadLedOff();
	BeepOff();
	IndicatorTfb::CondOff();
	//ken @2011.11.30
	if(HardwareToken.IAmVoyager1400())//make sure illumination control mode is init
	{
		WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT13_LSH,GPIO_BIT13_WID,0);
		WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,0);
		//((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_flash.turnoff(HI2D_AIM_ALT, true/*, ((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_LedCtrlMode*/);
		//((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_aimer.turnoff(HI2D_AIM_ALT);
	}
	if(theStdInterface.DeviceIoControl(IOCTRL_IS_REMOTE_WAKEUP_EN))
	{
		
#if defined(RFBASE)
#else
		if(false == DoesPluginHaveControlofKPP_COL2())
		{
			if(pCHWTrigger != NULL)
				pCHWTrigger->PrepareForStop();
		}
#endif

	}	//endof IOCTRL_IS_REMOTE_WAKEUP_EN
	
	//Save Clock Settings
	SaveCRM_CGR0Reg = READ_REG(CRM_CGR0);
	SaveCRM_CGR1Reg = READ_REG(CRM_CGR1);
	SaveCRM_CGR2Reg = READ_REG(CRM_CGR2);

	//Save GPIO Settings
	SaveGPIO1_DR0Reg = READ_REG(GPIO1_DR0);
	SaveGPIO1_GDIR0Reg = READ_REG(GPIO1_GDIR0);
	SaveGPIO2_DR0Reg = READ_REG(GPIO2_DR0);
	SaveGPIO2_GDIR0Reg = READ_REG(GPIO2_GDIR0);
	SaveGPIO3_DR0Reg = READ_REG(GPIO3_DR0);
	SaveGPIO3_GDIR0Reg = READ_REG(GPIO3_GDIR0);
	SaveGPIO4_DR0Reg = READ_REG(GPIO4_DR0);
	SaveGPIO4_GDIR0Reg = READ_REG(GPIO4_GDIR0);
#ifdef HI2D_T
	SaveIOMUXC_SW_MUX_CTL_PAD_GPIO_A = READ_REG(IOMUXC_SW_MUX_CTL_PAD_GPIO_A);
	SaveIOMUXC_SW_MUX_CTL_PAD_GPIO_B = READ_REG(IOMUXC_SW_MUX_CTL_PAD_GPIO_B);
	SaveIOMUXC_SW_MUX_CTL_PAD_CSI_MCLK= READ_REG(IOMUXC_SW_MUX_CTL_PAD_CSI_MCLK);
#endif
//	HAZARD_WRITE_FIELD_TO_REG(IOMUXC_SW_MUX_CTL_PAD_VSTBY_REQ,IOMUXC_SW_MUX_CTL_MUX_MODE_LSH,IOMUXC_SW_MUX_CTL_MUX_MODE_WID,MUX_MODE_0);	//TEST CODE turn on VSTBY

	//--------------Control GPIO-----------------
	//Making the following GPIO Changes will reduce the total board Current
	//BOOT_PWR_SELECT - Set GPIO as an input
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_GPIO_C,MUX_MODE_0);
	HAZARD_WRITE_FIELD_TO_REG(GPIO1_GDIR0,GPIO_BIT2_LSH,GPIO_BIT2_WID,GPIO_GDIR_INPUT);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_GPIO_C,0x00000000);
	
	if(HardwareToken.IAmOEM())
	{
		if(!(HardwareToken.IAmOEMPrototype()))
		{
			//Set 232INV pin to proper state.
			HAZARD_WRITE_FIELD_TO_REG(IOMUXC_SW_MUX_CTL_PAD_UART2_CTS,IOMUXC_SW_MUX_CTL_MUX_MODE_LSH,IOMUXC_SW_MUX_CTL_MUX_MODE_WID,MUX_MODE_5);
			HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART2_CTS,0x000000E0);  //100k pullup
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,GPIO_HIGH);
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,GPIO_GDIR_OUTPUT);
			
			//PSOC_PWR_ENA - KPP_ROW2 - GPIO2_31
			// Allow PSOC to go into Low Power Mode, when set low
			HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_ROW2,MUX_MODE_5);
			HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_KPP_ROW2,0x00000000);
			HAZARD_WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT31_LSH,GPIO_BIT31_WID,GPIO_LOW);
			HAZARD_WRITE_FIELD_TO_REG(GPIO2_GDIR0,GPIO_BIT31_LSH,GPIO_BIT31_WID,GPIO_GDIR_OUTPUT);
			
			StopModeTimer.SetExpiration(1000);		//Wait 1mS for the PSOC to turn the lights off.
			while(!StopModeTimer.HasExpired()){};
		
			HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT19_LSH,GPIO_BIT19_WID,GPIO_HIGH); //GPIO - POWER_FAIL - PS_MODE_BUCK_BOOST - GPIO3_19
		}
	}
	else
	{
		//ENG_RESET - GPIO3_0 - Take the PSOC out of reset, allow it to put itself into Low Power Mode
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_ROW3,MUX_MODE_5);
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT0_LSH,GPIO_BIT0_WID,GPIO_LOW);
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT0_LSH,GPIO_BIT0_WID,GPIO_GDIR_OUTPUT);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_KPP_ROW3,0x00000000);

		//Controls Laser Aimer PSOC on 1900 and 1902
		//PSOC_PWR_ENA - KPP_ROW0 - GPIO2_29
		// Allow PSOC to go into Low Power Mode, when set low
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_ROW0,MUX_MODE_5);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_KPP_ROW0,0x00000000);
		HAZARD_WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,GPIO_LOW);
		HAZARD_WRITE_FIELD_TO_REG(GPIO2_GDIR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,GPIO_GDIR_OUTPUT);
	}
	
	if(HardwareToken.IAmVoyager1400())
	{
		// POWER_SWITCH - OE - GPIO2_14
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_OE,MUX_MODE_5);
		HAZARD_WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT14_LSH,GPIO_BIT14_WID,GPIO_LOW);
		HAZARD_WRITE_FIELD_TO_REG(GPIO2_GDIR0,GPIO_BIT14_LSH,GPIO_BIT14_WID,GPIO_GDIR_OUTPUT);
		//CSP type
		//Init D11 gpio4_9 to control VDD
		//Ken@2011.11.29 Here must be sure the sensor has been sleep.
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_D11,MUX_MODE_5);
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT9_LSH,GPIO_BIT9_WID,GPIO_LOW);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_D11,0x00000000);
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT9_LSH,GPIO_BIT9_WID,GPIO_GDIR_OUTPUT);
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT9_LSH,GPIO_BIT9_WID,GPIO_LOW);//down the VDD

		//low-high current pin set hi-z state
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART2_RTS,MUX_MODE_5);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART2_RTS,0x00000007);
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,GPIO_LOW);
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,GPIO_GDIR_INPUT);//Hi-z STATE

		//nhigh_lad to hi-z
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART2_RXD, MUX_MODE_5);							//MUX To GPIO
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART2_RXD,0x00000007);	
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT26_LSH,GPIO_BIT26_WID,GPIO_LOW);
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT26_LSH,GPIO_BIT26_WID,GPIO_GDIR_INPUT);//Hi-z STATE

		//pwm_lad to 0
		//PWM_Sample(PWM4,0);
		//PWM_Stop(PWM4);
		HAZARD_WRITE_FIELD_TO_REG(PWM4_PWMCR,PWM_CR_POUTC_LSH,PWM_CR_POUTC_WID,0x2);
		HAZARD_WRITE_FIELD_TO_REG(PWM4_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID,0x0);
	}
	//Place USB PHY into Low Power Suspend
	HAZARD_WRITE_FIELD_TO_REG(USB_H2_PORTSC1,23,1,1);		//We can do the Host Core directly becasue we do not use it
	if(HardwareToken.IAmVoyager1400())
	{	
		//Config I2C3_SCL/I2C3_SDA/CSI_MCLK to be input GPIO
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_GPIO_A,MUX_MODE_0);
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_GPIO_B,MUX_MODE_0);
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_CSI_MCLK,MUX_MODE_5);
		
		HAZARD_WRITE_FIELD_TO_REG(GPIO1_GDIR0,GPIO_BIT0_LSH,GPIO_BIT0_WID,GPIO_GDIR_INPUT);
		HAZARD_WRITE_FIELD_TO_REG(GPIO1_GDIR0,GPIO_BIT1_LSH,GPIO_BIT1_WID,GPIO_GDIR_INPUT);
		HAZARD_WRITE_FIELD_TO_REG(GPIO1_GDIR0,GPIO_BIT8_LSH,GPIO_BIT8_WID,GPIO_GDIR_INPUT);
		//only in usb suspend mode
		HAZARD_WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT12_LSH,GPIO_BIT12_WID,GPIO_LOW);
		
	}	
	//Save Interrupts 
	Saved_INT_EN_Low = READ_REG(ASIC_INTENABLEL);
	Saved_INT_EN_High = READ_REG(ASIC_INTENABLEH);

	//Only Enable the Interrupts that can wake us up, this prevents others from running	
	if(theStdInterface.DeviceIoControl(IOCTRL_IS_REMOTE_WAKEUP_EN))
	{
		//Allow Interrupts to wake up processor in LOW Power
		if(HardwareToken.IAmOEM())
		{
			HAZARD_WRITE_REG(CRM_LPIMR0, ~INTENL_OEM_LP_SETTING);	
			HAZARD_WRITE_REG(CRM_LPIMR1, ~INTENH_OEM_LP_SETTING);
			HAZARD_WRITE_REG(ASIC_INTENABLEL,INTENL_OEM_LP_SETTING);
			HAZARD_WRITE_REG(ASIC_INTENABLEH,INTENH_OEM_LP_SETTING);
		}
		else
		{
			HAZARD_WRITE_REG(CRM_LPIMR0, ~INTENL_LP_SETTING);
			HAZARD_WRITE_REG(CRM_LPIMR1, ~INTENH_LP_SETTING);
			HAZARD_WRITE_REG(ASIC_INTENABLEL,INTENL_LP_SETTING);
			HAZARD_WRITE_REG(ASIC_INTENABLEH,INTENH_LP_SETTING);
		}		
	}
	else
	{
		//Do not allow the Trigger to wake the processor
		if(HardwareToken.IAmOEM())
		{		
			HAZARD_WRITE_REG(CRM_LPIMR0, (~INTENL_OEM_LP_SETTING || (1<<INT_GPIO4_INT)));
			HAZARD_WRITE_REG(CRM_LPIMR1, ~INTENH_OEM_LP_SETTING);
			HAZARD_WRITE_REG(ASIC_INTENABLEL,(INTENL_OEM_LP_SETTING && ~(1<<INT_GPIO4_INT)));
			HAZARD_WRITE_REG(ASIC_INTENABLEH,INTENH_OEM_LP_SETTING);
		}
		else
		{		
			HAZARD_WRITE_REG(CRM_LPIMR0, (~INTENL_LP_SETTING || (1<<INT_GPIO3_LSH)));
			HAZARD_WRITE_REG(CRM_LPIMR1, ~INTENH_LP_SETTING);
			HAZARD_WRITE_REG(ASIC_INTENABLEL,(INTENL_LP_SETTING && ~(1<<INT_GPIO3_LSH)));
			HAZARD_WRITE_REG(ASIC_INTENABLEH,INTENH_LP_SETTING);
		}
	}
	
	//Set Clock Gates
	HAZARD_WRITE_REG(CRM_CGR0, CRM_CGR0_LP_SETTING);		//Turn On Minimum number of Modules
	HAZARD_WRITE_REG(CRM_CGR1, CRM_CGR1_LP_SETTING);		//Turn On Minimum number of Modules
	HAZARD_WRITE_REG(CRM_CGR2, CRM_CGR2_LP_SETTING);		//Turn On Minimum number of Modules

	//Set Low Power Registers
	HAZARD_WRITE_REG(CRM_PMCR0, CRM_PMCR0_LP_SETTING);		//Low Power Register Setting
	HAZARD_WRITE_REG(CRM_PMCR1, CRM_PMCR1_LP_SETTING);		//Low Power Register Setting
	HAZARD_WRITE_REG(CRM_PMCR2, CRM_PMCR2_LP_SETTING);		//Low Power Register Setting
	HAZARD_WRITE_REG(CTL_GP_SER,0x00000080);				//Set Bit 7 of CTL_GP_CTRL
	
	//Enter Low Power Mode
	HAZARD_WRITE_FIELD_TO_REG(CRM_CCTL,CRM_CCTL_LPCTL_LSH,CRM_CCTL_LPCTL_WID,LP_CTL_Stop_Mode);	//tell the CRM to enter stop mode when WFI has been issued
	PMStopModeIRQ = 0;
	RestoreInterrupts(PMStopModeIRQ);		//Interrupts must be enabled to come out of stop mode
											//But do not enable any pending ones now

	//************************************
	//#ifdef VUQUEST3310
	#if defined(VUQUEST3310) || defined (HI2D_T)
	//add by kenhe @2011.08.26
	if(!is_ignore_suspend())
		WaitForIRQ();  	//Tell the Core to wait for interrupt, you are now in STOP MODE
	reset_res_count_and_timout();//set usb resume irq count to zero
	#else
	WaitForIRQ();  	//Tell the Core to wait for interrupt, you are now in STOP MODE
	#endif
	//************************************

	//NOTE: The waking interrupt will run before we return here 
		
	//Exit Low Power Mode
	PMStopModeIRQ = 0;
	DisableInterrupts(PMStopModeIRQ);

#if defined(SM3110)|| defined(SM5130)
// fix bug: when wake up from stop mode, scanner transmit error code to PC. This is because MCU's PLL locked error frequency
		{
			int i;
			for(i=50000;i>0;i--)
			{
				;
			}
			for(i=50000;i>0;i--)
			{
				;
			}
		}

	
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

	{
		int i;
		for(i=5000;i>0;i--)
		{
			;
		}
	}
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
#endif

	//Wait for PLLs to LOCK before restarting, should be done in HW but just check it
	while(READ_FIELD(CRM_MPCTL,CRM_MPCTL_LOCK_LSH,CRM_MPCTL_LOCK_WID) == 0)
	{
		//Wait for the MCU PLL to LOCK
	}
	while(READ_FIELD(CRM_UPCTL,CRM_UPCTL_LOCK_LSH,CRM_UPCTL_LOCK_WID) == 0)
	{
		//Wait for the USB PLL to LOCK
	}

	//Restore Low Power Settings
	HAZARD_WRITE_REG(CRM_PMCR0, CRM_PMCR0_RUN_SETTING);		//Low Power Register Setting in Normal Run Mode
	HAZARD_WRITE_REG(CRM_PMCR1, CRM_PMCR1_RUN_SETTING);		//Low Power Register Setting in Normal Run Mode
	HAZARD_WRITE_REG(CRM_PMCR2, CRM_PMCR2_RUN_SETTING);		//Low Power Register Setting in Normal Run Mode
	
	//Restore Clocks
	HAZARD_WRITE_REG(CRM_CGR0,SaveCRM_CGR0Reg);
	HAZARD_WRITE_REG(CRM_CGR1,SaveCRM_CGR1Reg);
	HAZARD_WRITE_REG(CRM_CGR2,SaveCRM_CGR2Reg);
	
	HAZARD_WRITE_REG(GPIO1_DR0,SaveGPIO1_DR0Reg);
	HAZARD_WRITE_REG(GPIO1_GDIR0,SaveGPIO1_GDIR0Reg);
	HAZARD_WRITE_REG(GPIO2_DR0,SaveGPIO2_DR0Reg);
	HAZARD_WRITE_REG(GPIO2_GDIR0,SaveGPIO2_GDIR0Reg);
	HAZARD_WRITE_REG(GPIO3_DR0,SaveGPIO3_DR0Reg);
	HAZARD_WRITE_REG(GPIO3_GDIR0,SaveGPIO3_GDIR0Reg);
	HAZARD_WRITE_REG(GPIO4_DR0,SaveGPIO4_DR0Reg);
	HAZARD_WRITE_REG(GPIO4_GDIR0,SaveGPIO4_GDIR0Reg);
#ifdef HI2D_T
	//restore I2C3_SCL/I2C3_SDA/CSI_MCLK
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_GPIO_A,SaveIOMUXC_SW_MUX_CTL_PAD_GPIO_A);
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_GPIO_B,SaveIOMUXC_SW_MUX_CTL_PAD_GPIO_B);
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_CSI_MCLK,SaveIOMUXC_SW_MUX_CTL_PAD_CSI_MCLK);
#endif	
	//---Restore GPIO---
	if(HardwareToken.IAmOEM())
	{
		if(!(HardwareToken.IAmOEMPrototype()))
		{
			StopModeTimer.SetExpiration(1000);	//Wait 1mS for the hardware to stabilize before letting the Power Supplies go into high current mode
			while(!StopModeTimer.HasExpired()){};
		
			HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT19_LSH,GPIO_BIT19_WID,GPIO_LOW); //GPIO - POWER_FAIL - PS_MODE_BUCK_BOOST - GPIO3_19
		}
	}
	else
	{
		HAZARD_WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,GPIO_HIGH); //SET PSOC_PWR_ENA to high
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT0_LSH,GPIO_BIT0_WID,GPIO_LOW); //ENG_RESET for Laser Aimer  
	}

	Wake_Ctrl(1);         //Ensure to hold our power back on. (In case we were called called from POWERDOWN below)

#if defined(RFBASE)
#else
	if(pCHWTrigger != NULL)
		pCHWTrigger->UnPrepareForStop();
#endif //endof defined(RFBASE)
	
	//Re enable all interrupts 
	HAZARD_WRITE_REG(ASIC_INTENABLEL,Saved_INT_EN_Low);
	HAZARD_WRITE_REG(ASIC_INTENABLEH,Saved_INT_EN_High);
	

	RestoreInterrupts(PMSavedIRQ);	//Restore Interrupts incase we were not called from Manage System power.
	
}	//endof EnterStopMode

// ===  Function  ======================================================================
//
//         Name:  GoInToPowerDownMode
//
//  Description: Powers Off the system by releasing the Wake Line and forcing the 
//		Shutdown pin.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
static void GoInToPowerDownMode(void)
{
	ULONG SaveIrqMaskL = GetHighInterruptEnableRegister();  // Save interrupt enable	 registers
	ULONG SaveIrqMaskH = GetLowInterruptEnableRegister();

	UsageStatistics.PowerDownCycles++;
	StoreUsageStatistics();

	Wake_Ctrl(0);  /*Allow the Hardware to begin shutdown.
					Hardware takes 2 seconds for this to go down so
					we will go to STOP and if we wake up we will restore
					power to device	and move on with our lives */

	if(!(HardwareToken.IAmOEM())) //Shutdown pin not available in OEM
	{
		SHUTDOWN_CTRL(1); //Set Shutdown high to discharge capacitors more rapidly.  
	}
	
	DisableInterrupts(PMSavedIRQ);

	// It is best to run with full power until the power goes away.
	// This shall improve behavior on some hardware that did cause a reset while shutting down.
	CTimeout PowerDownTimeout(6*Second);	// 5080 takes about 4 seconds to turn off power.

	while(!PowerDownTimeout.HasExpired())
	{
		//Once we start the shutdown there is no turning back, unless it fails
		RESET_WATCHDOG_TIMER();
		if(IsWakeInputActive())  
		{                  
			break;
		}
	} //endof while(!timeout.HasExpired())

	// If poweroff works, we will never reach this point
	SetLowInterruptEnableRegister(SaveIrqMaskL);  	// restore the interrupt enable registers
	SetLowInterruptEnableRegister(SaveIrqMaskH);
	RestoreInterrupts(PMSavedIRQ);
	
}	//endof GoInToPowerDownMode

// ===  Function  ======================================================================
//
//         Name:  Prepare_System_for_Suspend
//
//  Description:  Performs a Shutdown of major system components while the interrupts 
//		are enabled.  This is mainly used for the Base because some components are 
//		Shutdown but the processor does not suspend.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in: 
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void Prepare_System_for_Suspend(void)
{
	if(bSystemInSuspendMode == false)
	{
		bSystemInSuspendMode = true;
		theStdInterface.DeviceIoControl(IOCTRL_PREPARE_FOR_LOW_POWER_STATE);
#if defined(RFBASE)
		
#else

		Imager_Shutdown_for_Suspend();
	
			
#ifdef FLASH_LEDS
		if(!(HardwareToken.IAmOEM()))
		{
			Disable_Boost();		//ShutDown the Illumination Boost Circuit
		}
#endif //endof FLASH_LEDS

#endif // endof #if defined(RFBASE)
	}
}	//endof Prepare_System_for_Suspend

// ===  Function  ======================================================================
//
//         Name:  Restore_System_from_Suspend
//
//  Description:  Restores the components that were shutdown in the 
//		Prepare_System_for_Suspend function
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in: 
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void Restore_System_from_Suspend(void)
{
	if((bSystemInSuspendMode == true) && (bSystemSuspended == false) && theStdInterface.DeviceIoControl(IOCTRL_AM_I_ENUMERATED))
	{
//		theStdInterface.DeviceIoControl(IOCTRL_RETURN_TO_NORMAL_POWER_STATE);
		ResetPowerTimeOuts();
		
#if defined(RFBASE)


#else
		Imager_Startup_from_Suspend();


#ifdef FLASH_LEDS
		if(!(HardwareToken.IAmOEM()))
		{
			Enable_Boost();		//Start the Illumination Boost Circuit	
		}
#endif //endof FLASH_LEDS

#endif // endof #if defined(RFBASE)

		bSystemInSuspendMode = false; //Successfully exiting Suspend mode
	}
	
}	//endof Restore_System_from_Suspend

#if !defined(RFBASE)
// ===  Function  ======================================================================
//
//         Name:  Imager_Shutdown_for_Suspend
//
//  Description:  Safely shuts down the imager and stops scanning.  The imager should 
//		be shutdown in less then 8mS to meet the USB suspend time.  It states the system 
//		must enter suspend within 10ms.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
static void Imager_Shutdown_for_Suspend(void)
{
	//Block Triggering
	SavedTriggeringEnSetting = GET_SETTING(bIgnoreTrigger);
	RAM_SET_ACTUAL(bIgnoreTrigger,1);
	
	//Disable Menuing
	SavedMenuEnSetting = GET_SETTING(MenuingEnabled);
	RAM_SET_ACTUAL(MenuingEnabled,0);
	
	//Turn the Imager Power Off when Idle
	SavedImagerPowerSetting = GET_SETTING(ImagerIdlePowerLevel);
	RAM_SET_ACTUAL(ImagerIdlePowerLevel,0);
	
	//Turn the Scan Driver off after imaging
	SavedScanDriverTimeoutSetting = GET_SETTING(ScanDriverTimeout);
	RAM_SET_ACTUAL(ScanDriverTimeout,1);
	
	//Turn Shotgun mode off
	SavedShotgunSetting = GET_SETTING(Shotgun);
	RAM_SET_ACTUAL(Shotgun,0);
	
	//Turn Preferred mode off
	SavedPreferredSetting = GET_SETTING(PrioritySequence);
	RAM_SET_ACTUAL(PrioritySequence,0);

	//Turn Sequence mode off
	SavedSequenceSetting = GET_SETTING(SequenceEnabled);
	RAM_SET_ACTUAL(SequenceEnabled,0);
	
	//Change to Manual Trigger Mode
	SavedTrigmodeSetting = GET_SETTING(TriggerMode);
	RAM_SET_ACTUAL(TriggerMode,0);
	
	
#ifdef TRIGGER_5_5
	if(pTrigger != 0x0)
	{
    	pTrigger->OnChangedConfiguration();
    	pTrigger->SignalScanningFinished();
	}
#else
	if(g_pTriggerHandler != 0x0)
	{
    	g_pTriggerHandler->OnChangedConfiguration();
    	g_pTriggerHandler->SignalScanningFinished();
	}
#endif	//end #ifdef TRIGGER_5_5
	
	StopImaging();

}	//endof Imager_Shutdown_for_Suspend

// ===  Function  ======================================================================
//
//         Name:  Imager_Startup_from_Suspend
//
//  Description:  
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
static void Imager_Startup_from_Suspend(void)
{
	// flush the scan driver to sync up the statistics array
	StartImaging();
	MT_Delay(100);		// change the delay from 60ms to 100ms to work-around the sensor power down issue, which happened at scanner power up when use USB cable.
	StopImaging();

	//restore settings
	RAM_SET_ACTUAL(SequenceEnabled,SavedSequenceSetting);
	RAM_SET_ACTUAL(PrioritySequence,SavedPreferredSetting);
	RAM_SET_ACTUAL(Shotgun,SavedShotgunSetting);
	RAM_SET_ACTUAL(ScanDriverTimeout,SavedScanDriverTimeoutSetting);
	RAM_SET_ACTUAL(ImagerIdlePowerLevel,SavedImagerPowerSetting);
	RAM_SET_ACTUAL(MenuingEnabled,SavedMenuEnSetting);
	RAM_SET_ACTUAL(TriggerMode,SavedTrigmodeSetting);
	RAM_SET_ACTUAL(bIgnoreTrigger,SavedTriggeringEnSetting);

	if(g_pScriptInterpreter != NULL)
	{
		g_pScriptInterpreter->OnChangedConfiguration();	// For shotgun and sequence modes
	}
#ifdef TRIGGER_5_5
	if(pTrigger != 0x0)
	{
    	pTrigger->OnChangedConfiguration();
	}
#else
	if(g_pTriggerHandler != 0x0)
	{
    	g_pTriggerHandler->OnChangedConfiguration();
	}
#endif	//end #ifdef TRIGGER_5_5

}	//endof Imager_Startup_from_Suspend

#endif // endof #if ! defined(RFBASE)

// ===  Function  ======================================================================
//
//         Name:  SystemSuspendState
//
//  Description:  
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void SystemSuspendState(bool Suspend)
{
	bSystemSuspended = Suspend;
}	//endof SystemSuspendState

// ===  Function  ======================================================================
//
//         Name:  PSMODE_Control
//
//  Description:  A high output allows the regulators to automatically enter into
//					a higher efficiency pfm mode.  A low output forces the regualtors
//					into pwm mode.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  BOOL				LineState		State of the GPIO Output
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void PSMODE_Control(bool LineState)
{
#ifndef PROTO_HW
	PS_MODE_CTRL(LineState);
#endif //endof PROTO_HW
}	//endof PSMODE_Control

// ===  Function  ======================================================================
//
//         Name:  Init_PowerPins
//
//  Description:  Initialization of uP_WAKE and PS_MODE pins
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void Init_PowerPins(void)
{
	DISABLE_INTERRUPTS();

#if defined(PROTO_HW) && defined(RFSCAN)
#else
	if(HardwareToken.IAmOEM() && !(HardwareToken.IAmOEMPrototype()))
	{
		//GPIO - uP_WAKE - FEC_RDATA1 - GPIO3_11
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_FEC_RDATA1,MUX_MODE_5);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_FEC_RDATA1,0x000000C6);	//MAX Drive, 100k Pull-Down 
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT11_LSH,GPIO_BIT11_WID,GPIO_HIGH); //Set High to keep the unit from shutting down
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT11_LSH,GPIO_BIT11_WID,GPIO_GDIR_OUTPUT);
	}
	else
	{
		//GPIO - uP_WAKE - DE_B	- GPIO2_20
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_DE_B,MUX_MODE_5);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_DE_B,0x00000006);	//MAX Drive, No Pullups
		HAZARD_WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT20_LSH,GPIO_BIT20_WID,GPIO_HIGH); //Set High to keep the unit from shutting down
		HAZARD_WRITE_FIELD_TO_REG(GPIO2_GDIR0,GPIO_BIT20_LSH,GPIO_BIT20_WID,GPIO_GDIR_OUTPUT);
	}
	
#endif //endof defined(PROTO_HW) && defined(RFSCAN)

#ifndef PROTO_HW
	//GPIO - PS_MODE - GPIO_E - GPIO1_4
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_GPIO_E,MUX_MODE_0);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_GPIO_E,0x00000006);	//MAX Drive, No Pullups
	HAZARD_WRITE_FIELD_TO_REG(GPIO1_DR0,GPIO_BIT4_LSH,GPIO_BIT4_WID,GPIO_HIGH); //Set High to allow Power Supplies to regulate themselves
	HAZARD_WRITE_FIELD_TO_REG(GPIO1_GDIR0,GPIO_BIT4_LSH,GPIO_BIT4_WID,GPIO_GDIR_OUTPUT);
	
	if(HardwareToken.IAmOEM())
	{
		if(HardwareToken.IAmOEMPrototype())
		{
			//GPIO - UART2_RTS - PS_MODE_BUCK_BOOST - GPIO4_28
			HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART2_RTS,MUX_MODE_5);
			HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART2_RTS,0x00000006);	//MAX Drive, No Pullups
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,GPIO_LOW); //Set LOW to allow Power Supplies to regulate themselves
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,GPIO_GDIR_OUTPUT);
			
			//GPIO -PWRDY/nREADY - GPIO2_12
			HAZARD_WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT12_LSH,GPIO_BIT12_WID,GPIO_LOW);	//set data reg to LOW
			HAZARD_WRITE_FIELD_TO_REG(GPIO2_GDIR0,GPIO_BIT12_LSH,GPIO_BIT12_WID,GPIO_GDIR_OUTPUT);	//set direction to output
			HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_EB0,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO
			HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_EB0,0x00002040);	
		}
		else
		{
			//GPIO - POWER_FAIL - PS_MODE_BUCK_BOOST - GPIO3_19
			HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_POWER_FAIL,MUX_MODE_5);
			HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_POWER_FAIL,0x00000006);	//MAX Drive, No Pullups
			HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT19_LSH,GPIO_BIT19_WID,GPIO_LOW); //Set LOW to allow Power Supplies to regulate themselves
			HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT19_LSH,GPIO_BIT19_WID,GPIO_GDIR_OUTPUT);
			
			//GPIO - NFRB - PWRDY/nREADY - GPIO3_31
			HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT31_LSH,GPIO_BIT31_WID,GPIO_LOW);	//set data reg to LOW
			HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT31_LSH,GPIO_BIT31_WID,GPIO_GDIR_OUTPUT);	//set direction to output
			HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_NFRB,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO
			HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_NFRB,0x00002040);	
		}

		//GPIO - AIM_IN - GPIO2_30
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_ROW1,MUX_MODE_5);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_KPP_ROW1,0x000000A0);	//MAX Drive, 100K Pullups
		HAZARD_WRITE_FIELD_TO_REG(GPIO2_GDIR0,GPIO_BIT30_LSH,GPIO_BIT30_WID,GPIO_GDIR_INPUT);

	}
	else
	{
		//GPIO - SHUTDOWN - FEC_RDATA1 - GPIO3_11
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_FEC_RDATA1,MUX_MODE_5);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_FEC_RDATA1,0x00000001);	//No Pullups, Fast Slew Rate
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT11_LSH,GPIO_BIT11_WID,GPIO_LOW); //Set Low to not discharge the battery
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT11_LSH,GPIO_BIT11_WID,GPIO_GDIR_OUTPUT);
	}
	
#endif //endof PROTO_HW


	if(HardwareToken.IAmVoyager1400())
	{
		//TO DO set GPIO2_14 to high ken @2011.12.14
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_OE,MUX_MODE_5);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_OE,0x00000000);
		HAZARD_WRITE_FIELD_TO_REG(GPIO2_GDIR0,GPIO_BIT14_LSH,GPIO_BIT14_WID,GPIO_GDIR_OUTPUT);
		HAZARD_WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT14_LSH,GPIO_BIT14_WID,GPIO_HIGH);
		//HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT31_LSH,GPIO_BIT31_WID,GPIO_LOW);		//High will turn off the laser
		//HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT31_LSH,GPIO_BIT31_WID,GPIO_GDIR_OUTPUT);
	}
	

	RESTORE_INTERRUPTS();

}	//endof Init_PowerPins

// ===  Function  ======================================================================
//
//         Name:  Wake_Ctrl
//
//  Description:  A high output keeps power to the device
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  BOOL				LineState		State of the GPIO Output
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
static void Wake_Ctrl(bool LineState)
{
#if defined(PROTO_HW) && defined(RFSCAN)
#else
	if(HardwareToken.IAmOEM() && !(HardwareToken.IAmOEMPrototype()))
	{
		WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT11_LSH,GPIO_BIT11_WID,LineState);		//GPIO - uP_WAKE - FEC_RDATA1 - GPIO3_11
	}
	else
	{
		WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT20_LSH,GPIO_BIT20_WID,LineState);		//GPIO - uP_WAKE - DE_B - GPIO2_20
	}
#endif //endof defined(PROTO_HW) && defined(RFSCAN)

}	//endof PSMODE_Control


