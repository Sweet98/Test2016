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
// $RCSfile: db_boot/db_hdw.c $
// $Revision: 1.11.1.34 $
// $Date: 2011/11/10 20:40:55EST $
//======================================================================================
/************************************************************************/
/*  This module contains a collection of low level functions that are	*/
/* hardware and processor dependant.									*/
/************************************************************************/
#include	<string.h>

#include "db_hdw.h"          /* artwork dependant hardware definitions    */
#include "db_chrio.h"        /* Character I/O header prototypes */
#include "db_time.h"
#include "db_sharedtable.h"
#include "db_syncommandparse.h"
#include "boot_main.h"

#if PLATFORM_CURRENT == imx25
//Gen 6 Hardware (MX25)
#include "HardwareMacros.h"		//Provides Macros for setting registers
#include "iMX25_CRM.h"			//Clock & Reset controller Definitions
#include "iMX25_WDOG.h"			//WatchDog Definitions
#include "iMX25_IOMUX.h"		//IO MUX Controller
#include "iMX25_GPIO.h"			//GPIO controller
#include "iMX25_UART.h"			//UART Definitions, Offsets for UARTs 1-6
#include "ProjectRevisionBoot.h"
#endif	//endof PLATFORM_CURRENT

#define WATCHDOG_TIMEOUT	0x03	//.5*(X+1) second time-out. So that would/should be 3s (MIN .5 Sec, MAX 128 Sec)


// I believe this version info is legacy.  We're gonna use this to get around a sticky problem with the shared table limitation (length was not set & no version information).
#define BootMajorVersion ((BYTE) 0xFF)  // note:  there's some legacy code in app that looks for 0xff which means version info (MKS project rev) is present in the shared table.
#define BootMinorVersion ((BYTE) 0x03)  // note:  we increment this when we add new items to the table after product has been released so it can handle old/new versions of boot.


/***** External defines ****/

extern "C"	void MMU_reset();
extern "C"     void Run_boot_code(void) ;
extern "C"	unsigned int QueryFlashType ( void *FlashBaseAddress, void *Address );
extern "C"	void GetSizeAndAddressOfQueryFlashType (unsigned int *Address, unsigned int *Size);
extern "C"	void MMU_CleanAndInvalidate( void );



// ===  Function  ======================================================================
// 
//         Name:  init_hardware
// 
//  Description:  Initializes Hardware, calls init_timer(), calls init_wdog.
//					it then sets up some initial shard table values 
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:  
//       out:  
//    return:					 	 
// -------------------------------------------------------------------------------------
void init_hardware (void)
{

	init_timer();			// initialize timer 1 about 1uS tick rate
	init_wdog();			// Start the Watchdog
	

// Some sharedtable things need to be set up.  TODO:  thisn't really hw related, so consider moving this somewhere else (perhaps boot_main.c).
	BOOT_CODE_VERSION_STRING = PROJ_REV_BOOT;
	BOOT_CODE_MINOR_VERSION = BootMinorVersion;
	BOOT_CODE_MAJOR_VERSION = BootMajorVersion;
	BOOT_COMPAT_PROD_ID = COMPAT_PROD_ID;
	BOOT_DATE_STR = g_DateStamp;
	BOOT_TIME_STR = g_TimeStamp;
	SIZE_OF_SHARED_TABLE = SHARED_TABLE_LAST - ACTUAL_TABLE_LOCATION;
// setup table entries for app code should you know
// wnat this to be a function call when we are done here, but for now just set what is necessary to set
	SIZE_OF_OP_CODE_TO_STORE =    0;      // should I hand over to the tell  handed over app code to store / or at least look to see if it should immediately upon run
	COMPRESSED_APPLICATION_LOCATION = 0;
	COMPRESSED_APPLICATION_SIZE = 0;
	APPLICATION_COMPRESSOR_LOCATION = 0;
	COMPRESSOR_AND_COMPRESSED_APPLICATION_SIZE = 0;
	FLASH_DISK_LOCATION = ( unsigned char *)0xFFFFFFFF;
	INVERSE_FLASH_DISK_LOCATION = ( unsigned char *)0xFFFFFFFF;

     /* Clear, init all queues */
	FlushSimpleIoQueues();
// do whatever is necessary to take peripheral pins out of sleep state

}	/*  initialize_hardware */



// ===  Function  ======================================================================
// 
//         Name:  init_wdog
// 
//  Description:  Initializes the Watch Dog Timer 
// 		This section configures the WCR register of the WatchDog, which is 16Bits Wide
// 		The Timeout is set in the WT (watchdog timeout) Field.
// 		When the timer expires a reset will be triggered
// 		The Timer is then Enabled with the WDE bit.
// 		During WAIT and STOP power modes the WatchDog timer is disabled,
// 		for mx27:  
// 			this is controlled with the WDZST Bit.
//				Once set this bit cannot be cleared
// 			NOTE: "The WDOG module cannot be deactivated again after activation"

// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:  
//       out:  
//    return:					 	 
// -------------------------------------------------------------------------------------
void init_wdog (void)
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

#endif  //Endof PLATFORM_CURRENT

}//End init_wdog


/************************************************************************/
/* Setup system according to configuration.							 	*/
/************************************************************************/
void init_system (void)
{
	//This does nothing
}	/*  initialize_system	*/



/*************************************/
/************************************/
/***********************************/
/**********************************/
/*********************************/
/********************************/
// General IRQ handler

// even though IRQs don't really apply to boot code, for completeness this is being improved in coordination with the
// added setup assembly code initially.  This setup does what is necessary that the __irq would normally do.
//extern "C" __irq void IRQHandler (void)
extern "C" void IRQHandler (void)
{

	reset_to_boot_code_hard();
// 	return;
} /* IRQHandler */


// General undefined handler
// Undefined is a standard call, so no special ((keyword)) handling of handler
extern "C" void UndefinedHandler (unsigned int /*InstructionAddress*/)
{

//      xputstring("Boot Code Undefined Instruction Exception caused by instruction at ");
 //     xputhex(InstructionAddress, 8);
  //    xputstring("\015\012");
		reset_to_boot_code_hard();
} /* UndefinedHandler */


extern "C" void C_SWI_Handler (unsigned int /*i_number */,unsigned int /* InstructionAddress */)
{
		reset_to_boot_code_hard();
} /* SWIHandler */



// General prefetch error handler
// although the intent is that you might be able to soft-recover by adding code
// to the op code handler, this routine will always reset to boot code
extern "C" void PrefetchHandler (unsigned int /* InstructionAddress */)
{
 //     xputstring("Boot Code Prefetch Instruction Exception caused by instruction at ");
  //    xputhex(InstructionAddress, 8);
   //   xputstring("\015\012");
		reset_to_boot_code_hard();
} /* PrefetchHandler */



// General data error handler
// this also will always cause a reset as well
extern "C" void AbortHandler (unsigned int /* InstructionAddress */)
{

	{
 //     xputstring("Boot Code Data Abort Exception caused by instruction at ");
  //    xputhex(InstructionAddress, 8);
 //     xputstring("\015\012");
		reset_to_boot_code_hard();
	}
} /* AbortHandler */



// General FIQ handler
// For now, I will just undo all the potential masks and return
//... later I will at least move the IRQs here.

// same comment to change as the IRQ above
//extern "C" __irq void FIQHandler ()
extern "C" void FIQHandler ()
{

	reset_to_boot_code_hard();
} /* FIQHandler */


// Software reset handler...  Got something real bad we don't want to do too much.  Just the minimum.
//ESD is one of those bad things.
void reset_to_boot_code_hard(void)
{
#if ((PLATFORM_CURRENT == imx27) || (PLATFORM_CURRENT == imx25))	
		HAZARD_WRITE_FIELD_TO_16BIT_REG(WDOG_WCR,WDOG_WCR_SRS_LSH,WDOG_WCR_SRS_WID,0);
		
#endif  //endof PLATFORM_CURRENT
	while (1); // wait til it happens, the rest should take care of itself
}


//
//	DetermineROMSize
//		Query the ROM for it's type, then return the size based on that type.
//			Type codes are
//					0 for AMD29LV800BB
//					1 for Intel
//					2 for AMD29LV160DB
//					3 for ST_29W800DB
//					4 for MX_29LV800BTC
//					5 for INTEL_1MB_TOP_BOOT
//					6 for INTEL_1MB_BOTTOM_BOOT
//					7 for INTEL_2MB_TOP_BOOT
//					8 for INTEL_2MB_BOTTOM_BOOT
//					9 for INTEL_4MB_TOP_BOOT
//					10 for INTEL_4MB_BOTTOM_BOOT
//					11 for INTEL_8MB_TOP_BOOT
//					12 for INTEL_8MB_BOTTOM_BOOT
//					13 spansion 32M bit uniform sector
//		Note that we take the flash base address as an argument.  This is because it needs to be the uncached mapped base
//		address for flash if MMU is enabled, but needs to be what would normally
extern "C" int DetermineROMSize( void *FlashBase )
{
#if PLATFORM_CURRENT == imx25 // mx25 has spi flash for now just say 0 since is no flash on cs0
	return 0;
#endif
} // DetermineROMSize


extern unsigned long getFlashDiskBase(void)
{
	static const unsigned int fdiskBase[] =
	{
			0x8000,
			0x8000,
			0x8000,
			0x8000,
			0x8000,
			0x100000-0x8000,
			0x8000,
			0x200000-0x8000,
			0x8000,
			0x400000-0x8000,
			0x8000,
			0x800000-0x8000,
			0x8000,
			0x20000
		};
		
	return fdiskBase[SHARED_TABLE_FLASHTYPE];
}




