/************************************************************************/
//
// HHP 4480 embedded firmware
//
//  Hardware source file -- Intent (and it is always intent) all hardware dependencies are generated here
//  Date:       2/24/02
//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: app_kernel/db_hdw.c $
// $Revision: 1.55.2.16 $
// $Date: 2011/11/10 20:40:28EST $
//======================================================================================
/************************************************************************/
/*  This module contains a collection of low level functions that are	*/
/* hardware and processor dependant.									*/
/************************************************************************/
#include  <stdio.h>  
#if PLATFORM_CURRENT == imx25
#include "HardwareMacros.h"		//Provides Macros for setting registers
#include "iMX25_GPIO.h"			//General-Purpose I/O Definitions
#include "iMX25_IOMUX.h"
#else
#error VALID PLATFORM NOT DEFINED
#endif //PLATFORM_CURRENT

#include "HardwareToken.h"

#include "db_time.h"
#include "PowerManagement.h"

#include "db_hdw.h"         	//artwork dependant hardware definitions
#include "db_chrio.h"       	//Character I/O header prototypes
#include "interrupts.h"
       	//standard I/O functions


#if defined(STREAM_CAPTURE) && defined(JEFFREY_HARPER)
#define WATCHDOG_TIMEOUT	0x3F
#else
#define WATCHDOG_TIMEOUT	0x09	//(.5*X)+.5 second time-out. So that would/should be 5s (MIN .5 Sec, MAX 64 Sec)
#endif

bool EnableWDog=FALSE;

void InitUnusedGPIOPins(void);

/************************************************************************/
/*  Pre-configuration hardware initialization. Here is where we setup	*/
/* all hardware that is not controlled by the configuration stored in	*/
/* the PEROM.  In other words, this is the place for all POI          */
/* details                                                            */
/************************************************************************/
void init_hardware (void)
{
	Initialize_Clocks();

	init_timer();		//Setup Timer1

	configure_watchdog();

	// Interrupt controller fiq/irq... make everything a irq for now
	ZeroInterruptInit();



	/***** Software Initialization *****/
	// That which we wish to assure above and beyond C init details are set...
	// should be done here

	ElapsedTimer1 = 0;
	BaselineClock = 0;
	FlushSimpleIoQueues();      /* Clear, init all queues */

//	InitUnusedGPIOPins();
}    /*  initialize_hardware */

// ===  Function  ======================================================================
//
//         Name:  configure_watchdog
//
//  Description:  Initializes the Watch Dog Timer
// 		This section configures the WCR register of the WatchDog, which is 16Bits Wide
// 		The Timeout is set in the WT (watchdog timeout) Field.
// 		When the timer expires a reset will be triggered
// 		The Timer is then Enabled with the WDE bit.
// 		During WAIT and STOP power modes the WatchDog timer is disabled,
// 		for mx27/mx25:
// 			this is controlled with the WDZST Bit.
//				Once set this bit cannot be cleared
// 			NOTE: "The WDOG module cannot be deactivated again after activation"
//
//	This is the same function as init_wdog in boot.
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void configure_watchdog (void)
{
#if PLATFORM_CURRENT == imx25
	unsigned int value;
	//Inturrupts are not enabled yet.
	//The IPG CLock for the WDog was turned on when the clocks were initilized.
	value = (FIELD_MASK(WDOG_WCR_WT_LSH,WDOG_WCR_WT_WID,WATCHDOG_TIMEOUT) |							//Set the Timeout
	         FIELD_MASK(WDOG_WCR_WDZST_LSH, WDOG_WCR_WDZST_WID, WDOG_WCR_WDZST_SUSPEND) |			//Suspend the WDOG in WAIT and STOP power modes
	         FIELD_MASK(WDOG_WCR_SRS_LSH,WDOG_WCR_SRS_WID,1) |										//Software Reset Bit
	         FIELD_MASK(WDOG_WCR_WDBG_LSH,WDOG_WCR_WDBG_WID,1));										//WDog Debug EN, supends the wdog when in debug mode
	HAZARD_WRITE_16BIT_REG(WDOG_WCR,value);
	HAZARD_WRITE_FIELD_TO_16BIT_REG(WDOG_WCR,WDOG_WCR_WDE_LSH, WDOG_WCR_WDE_WID,WDOG_WCR_WDE_ENABLE); 			//Enable the Watchdog

#endif  //endof PLATFORM_CURRENT


}	/*	configure_watchdog	*/

/************************************************************************/
/* Switch to slower drive on address data and control signals				*/
/************************************************************************/
void ChangeBusDrive(int spd)
{
}

void SwitchToSlowDrive(void)
{
	ChangeBusDrive(0);
}

/************************************************************************/
/* Setup system according to configuration.							 	*/
/************************************************************************/
void init_system (void)
{
#ifndef ETM
	SwitchToSlowDrive();
#endif
}	/*  initialize_system	*/


void Report_Boot_Version (void)
{
	if(BOOT_CODE_MAJOR_VERSION != 0xFF)
	{
		printf ("Boot code firmware version %02d.%02d \r\n", (BYTE) BOOT_CODE_MAJOR_VERSION, (BYTE) BOOT_CODE_MINOR_VERSION);
	}
	else
	{
		printf ("Boot code firmware version %s\r\n", BOOT_CODE_VERSION_STRING);
	}
}
        // Determine if we're running in ROM by checking to see if this routine is located in ROM or RAM.
//
bool running_in_ROM(void)
{
    ULONG		AddressPtr;

    AddressPtr = ((ULONG) &running_in_ROM);
    if(((AddressPtr >= (ULONG) FLASH_READ_BASE) && (AddressPtr < ((ULONG) FLASH_READ_BASE + FLASH_SIZE))) ||
            ((AddressPtr >= (ULONG) FLASH_WRITE_BASE) && (AddressPtr < ((ULONG) FLASH_WRITE_BASE + FLASH_SIZE))))
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

// ===  Function  ======================================================================
//
//         Name:  InitUnusedGPIOPins
//
//  Description: Initializes Unused GPIO Pins.  Many GPIO pins may not be used now but 
//		have traces going to future functionality.  Any pins found in this list should 
//		be removed if they are used in other modules.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void InitUnusedGPIOPins(void)
{
#ifdef PROTO_HW
#else

	DISABLE_INTERRUPTS();
	
	//--------TRACE LINES-------------------------------
	//GPIO - CS5 - TRCLK - GPIO3_21
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_CS5,MUX_MODE_5);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_CS5,0x00000000);	//Enable 100k Pull-up
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT21_LSH,GPIO_BIT21_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT21_LSH,GPIO_BIT21_WID,GPIO_GDIR_OUTPUT);
	
	//GPIO - CS4 - TRSYNC - GPIO3_20
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_CS4,MUX_MODE_5);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_CS4,0x00000000);	//Enable 100k Pull-up
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT20_LSH,GPIO_BIT20_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT20_LSH,GPIO_BIT20_WID,GPIO_GDIR_OUTPUT);

	//GPIO - NFWE_B - PIPESTAT[2] - GPIO3_26
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_NFWE_B,MUX_MODE_5);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT26_LSH,GPIO_BIT26_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT26_LSH,GPIO_BIT26_WID,GPIO_GDIR_OUTPUT);

	//GPIO - NFRE_B - PIPESTAT[1] - GPIO3_27
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_NFRE_B,MUX_MODE_5);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT27_LSH,GPIO_BIT27_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT27_LSH,GPIO_BIT27_WID,GPIO_GDIR_OUTPUT);

	//GPIO - NFALE - PIPESTAT[0] - GPIO3_28
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_NFALE,MUX_MODE_5);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT28_LSH,GPIO_BIT28_WID,GPIO_GDIR_OUTPUT);

	//GPIO - NFCLE - Trace 0   - GPIO3_29
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_NFCLE,MUX_MODE_5);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,GPIO_GDIR_OUTPUT);

	//GPIO - NFWP_B - Trace 1 - GPIO3_30
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_NFWP_B,MUX_MODE_5);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT30_LSH,GPIO_BIT30_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT30_LSH,GPIO_BIT30_WID,GPIO_GDIR_OUTPUT);

	//GPIO - NFRB - Trace 2 - GPIO3_31
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_NFRB,MUX_MODE_5);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_NFRB,0x00000000);	//Enable 100k Pull-up
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT31_LSH,GPIO_BIT31_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT31_LSH,GPIO_BIT31_WID,GPIO_GDIR_OUTPUT);

	//GPIO - CSPI1_SS0 - Trace 6 - GPIO1_16
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_CSPI1_SS0,MUX_MODE_5);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_CSPI1_SS0,0x00000000);	//Enable 100k Pull-up
	HAZARD_WRITE_FIELD_TO_REG(GPIO1_DR0,GPIO_BIT16_LSH,GPIO_BIT16_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO1_GDIR0,GPIO_BIT16_LSH,GPIO_BIT16_WID,GPIO_GDIR_OUTPUT);

	//GPIO - CSPI1_SS1 - Trace 7 - GPIO1_17
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_CSPI1_SS1,MUX_MODE_5);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_CSPI1_SS1,0x00000000);	//Enable 100k Pull-up
	HAZARD_WRITE_FIELD_TO_REG(GPIO1_DR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO1_GDIR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,GPIO_GDIR_OUTPUT);

	//GPIO - CSPI1_RDY - Trace 9 - GPIO2_22
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_CSPI1_RDY,MUX_MODE_5);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_CSPI1_RDY,0x00000000);	//Enable 100k Pull-up
	HAZARD_WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT22_LSH,GPIO_BIT22_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO2_GDIR0,GPIO_BIT22_LSH,GPIO_BIT22_WID,GPIO_GDIR_OUTPUT);

	//GPIO - SD1_DATA1 - Trace 13 - GPIO2_26
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_SD1_DATA1,MUX_MODE_5);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_SD1_DATA1,0x00000000);	//Enable 100k Pull-up
	HAZARD_WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT26_LSH,GPIO_BIT26_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO2_GDIR0,GPIO_BIT26_LSH,GPIO_BIT26_WID,GPIO_GDIR_OUTPUT);

	//GPIO - SD1_DATA2 - Trace 14 - GPIO2_27
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_SD1_DATA2,MUX_MODE_5);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_SD1_DATA2,0x00000000);	//Enable 100k Pull-up
	HAZARD_WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT27_LSH,GPIO_BIT27_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO2_GDIR0,GPIO_BIT27_LSH,GPIO_BIT27_WID,GPIO_GDIR_OUTPUT);

	RESTORE_INTERRUPTS();
	
#endif
} //endof InitUnusedGPIOPins


// ===  Function  ======================================================================
//
//         Name:  InitVuquest3310GPIOPins
//
//  Description: Initializes Vuquest 3310 GPIO Pins,
//               to reduce the current in idle after power up
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------

