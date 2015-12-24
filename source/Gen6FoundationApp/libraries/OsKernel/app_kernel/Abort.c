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
// $RCSfile: app_kernel/Abort.c $
// $Revision: 1.8 $
// $Date: 2010/06/03 16:53:20EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// HEADER:  Abort.h
//
//	This file contains reset and abort handling functions
//
//------------------------------------------------------------------------------
#include <stdio.h>           /* standard I/O functions                         */
#include <stddef.h>

#if PLATFORM_CURRENT == imx25
#include "HardwareMacros.h"		//Provides Macros for setting registers
#include "iMX25_GPIO.h"			//General-Purpose I/O Definitions
#include "iMX25_WDOG.h"			//WatchDog Definitions
#else
#error VALID PLATFORM NOT DEFINED
#endif

#include "Abort.h"
#include "stdinclude.h"
#include "db_hdw.h"          /* artwork dependant hardware definitions    */
#include "db_chrio.h"        /* Character I/O header prototypes */
#include "interrupts.h"
#include "thread.h"
//#include "imaging.h"

/***** Private variables *****/
unsigned int register_save[16];
unsigned int savedpsr;


#define DUMP_IMAGE 0		//dumps the current P_SCANBUFFER
#define DUMP_ACUITY 0		//abort debug for acuity
#define RESET_ON_ABORT 0	//reset to boot code instead of holding at while(1)


//  low level dump internal registers routine
void DumpAbortRegisterState(void)
{
	xputstring("Saved PSR: ");
	xputhex(savedpsr,8);
	xputstring("\r\n");

	DumpRegisters((unsigned int *) register_save, "");
	xputstring("\r\nMemory dump at Top of Stack:\r\n");
	DumpMemory((unsigned int *) (register_save[13] & ~0xF), 256);
	xputstring("\r\n");
	print_thread_info();
} //DumpAbortRegisterState




void DumpMemory(const unsigned int * base, unsigned int word_count)
{
	char			bfr[16];
	int 			j = 0;

	for  (unsigned  i = 0;  i < word_count;  i++)
	{
		if  (j == 0)
		{
			snprintf(bfr, sizeof(bfr), "%8.8x: ", (unsigned int) (&base[i]));
			xputstring(bfr);
		}
		if  (j == 4)
			xputstring("  ");
		snprintf(bfr, sizeof(bfr), "  %8.8x", base[i]);
		xputstring(bfr);
		j++;
		if  (j > 7)
		{
			xputstring("\r\n");
			j = 0;
		}
	}
	if  (j != 0)
		xputstring("\r\n");
} //DumpMemory




//  low level processor register dump routine
void DumpRegisters(const unsigned int * save_base, const char * line_indent)
{
	char			bfr[24];
	const char *	nm;

	for  (int  i = 0;  i < 16;  i++)
	{
		if (i % 4 == 0)
		{
			xputstring(line_indent);
		}
		switch  (i)
		{
		case  13:
			nm = "sp";
			break;
		case  14:
			nm = "lr";
			break;
		case  15:
			nm = "pc";
			break;
		default:
			nm = "  ";
		}
		snprintf(bfr, sizeof(bfr), "   r%-2d %s %08x", i, nm, save_base[i]);
		xputstring(bfr);
		if (i % 4 == 3)
		{
			xputstring("\r\n");
		}
	}
} //DumpRegisters

#ifdef DUMP_DECODER
class HIMAGE;
bool SendImage(HIMAGE *ImageParameters, bool UseInversionSetting, bool OverrideShipImagesIn640x480VGAMode );
bool DumpDecoderSetup(void);
bool DumpDecoderData(void);
#endif

#if (DUMP_IMAGE)
void SendChasteImage(int X, int Y, unsigned char *img);
#endif

#ifdef SHOW_ABORTS
extern "C" unsigned int Get_SP(void);


void PrintAbortMessageandReset(unsigned int InstructionAddress,const char * str )
{

#if (DUMP_ACUITY)
	//Used with Acuity to store the debug report
	xputstring("\x02\xFE\x07\x00\x00\x00\rDEBUGS\x1d",14);
	wait_til_tx_is_empty();
#endif

#if defined( theHal ) && ( !defined( SIMPLE_IO_ONLY ) )

	if  (g_pHal)
	{				// ensure debug to serial port even if using USB
		theHal.PrepareIoForRs232();
	}
	pUart1->Ubir.b.WORD = 1151;
	pUart1->Ubmr.b.WORD = 9999;
#endif

#ifdef theCurrentInterface

	g_pCurrentInterface = NULL;
#endif

	DEBUG_puts(str);
	xputhex(InstructionAddress, 8);
	DEBUG_puts("\r\n");
	DumpAbortRegisterState();
	wait_til_tx_is_empty();

#ifdef DUMP_DECODER
	DumpDecoderSetup();
	xputstring("\r\n");
	DumpDecoderData();
	SendImage(NULL, true, true);
	wait_til_tx_is_empty();
#endif

#if (DUMP_IMAGE)
	SendChasteImage(0,0,0);
	wait_til_tx_is_empty();
#endif	

#if (DUMP_ACUITY)	  
	xputstring("\x02\xFE\x07\x00\x00\x00\rDEBUGE\x1d",14);
	wait_til_tx_is_empty();
#endif

#if (RESET_ON_ABORT)
	reset_to_boot_code_hard();
#endif

	while (1)
		RESET_WATCHDOG_TIMER();

#else		   //ELSE NOT #ifdef SHOW_ABORTS

	void PrintAbortMessageandReset(unsigned int /* InstructionAddress */,const char * /* str */ )
	{
		reset_to_boot_code_hard();

#endif     //END #ifdef SHOW_ABORTS


	}	//PrintAbortMessageandReset

	// General undefined handler
	// Undefined is a standard call, so no special ((keyword)) handling of handler
	extern "C" void UndefinedHandler (unsigned int InstructionAddress)
	{
		PrintAbortMessageandReset(InstructionAddress,"Application Code Undefined Instruction Exception caused by instruction at ");
	} /* UndefinedHandler */


	// General software interrupt handler
	// potentially very powerful, it does nothing now for 2 reasons
	// try to run without taking advantage AND Angel uses SWI predominantly,
	// if I have to link it in, this will have to be removed
#ifdef DEVELOPMENT_SETTINGS
	extern "C" void C_SWI_Handler (unsigned int swi_number, unsigned int /*InstructionAddress*/)
	{
		g_pStdInterface = NULL; // cheat
		g_pCurrentInterface = NULL;
		xputstring("Application Code SWI # ");
		xputhex(swi_number, 8);
		xputstring("\015\012");
		wait_til_tx_is_empty();
#else

		extern "C" void C_SWI_Handler (unsigned int /* swi_number */, unsigned int /*InstructionAddress*/)
		{
#endif
			reset_to_boot_code_hard();

		} /* SWIHandler */


		// General prefetch error handler
		// although the intent is that you might be able to soft-recover by adding code
		// to the op code handler, this routine will always reset to boot code
		extern "C" void PrefetchHandler (unsigned int InstructionAddress)
		{
			PrintAbortMessageandReset(InstructionAddress,"Application Code Prefetch Instruction Exception caused by instruction at ");
		}


#ifdef PRODUCT_CONFIGURATION_APPLICATION   /*only allow this code in product config.  When we get data aborts that means something bad happenend. Something
		bad could include corrupting the MemoryCheck variable so we stay in the data abort. (Not likely now as it was changed from a boolean to a specific value ) but
		we could also get our PC out of whack and run the function to enable it.  That could get us stuck as well.  So product config only one to use it... only allow
		it in product config */
		//
		//	Allow the use of the data abort handler to check for the existence of memory or devices.
		//
#define DO_IT 0xbeefdaad
#define DO_NOT_DO_IT ~(DO_IT)

		int	MemoryCheckInProgress = DO_NOT_DO_IT;  /*use a strange pattern here instead of a boolean true false to pretty much prevent this
																			varaiable space from getting whacked by some rogue code and getting us stuck in the abort handler looping forever */
		int		NumberOfDataAborts = 0;

		void EnableMemoryCheck( void )
		{
			MemoryCheckInProgress = DO_IT;
			return;
		}

		void DisableMemoryCheck( void )
		{
			MemoryCheckInProgress = DO_NOT_DO_IT;
			return;
		}

		int DataAbortCount( void )
		{
			return NumberOfDataAborts;
		}

		void ResetDataAbortCount( void )
		{
			NumberOfDataAborts = 0;
			return;
		}
#endif

		//

		// General data error handler
		// this also will always cause a reset as well
		extern "C" void AbortHandler (unsigned int InstructionAddress)
		{

			//RestoreMode(PROCESSOR_MODE_USER);
			// cable noise and capacitive coupling results in these messages often being sent due to a
			// power on or power off instance.  Therefore these relatively verbose messages were changed to
			// be a single recognizeable (for those of us who might want to recognize them) character
			// data abort is 0xfd
#ifdef PRODUCT_CONFIGURATION_APPLICATION
			if( MemoryCheckInProgress == DO_IT )
			{
				++NumberOfDataAborts;
			}
			else
#endif

			{
				PrintAbortMessageandReset(InstructionAddress,"Application Code Data Abort Exception caused by instruction at ");
			}
		} /* AbortHandler */

#ifdef SHOW_LIB_TRAPS
		extern "C"void __rt_raise(int signal,int type)
		{
			unsigned int		stackloc = Get_SP();
			static char bfr[60];

			//used with Acuity to store debug reports
			xputstring("\x02\xFE\x07\x00\x00\x00\rDEBUGS\x1d",14);
			wait_til_tx_is_empty();

			sprintf(bfr, "\r\nError code: %d type:%d  \r\n", signal, type);
			xputstring(bfr);
			sprintf(bfr, "\r\nStack pointer %8.8x:\r\n", (unsigned int) stackloc);
			xputstring(bfr);
			DumpMemory((unsigned int *) (stackloc & ~0xF), 256);
			xputstring("\r\n");
			wait_til_tx_is_empty();
#ifdef DUMP_DECODER

			DumpDecoderSetup();
			xputstring("\r\n");
			DumpDecoderData();
			RawImageShip();
#endif

#if (DUMP_IMAGE)
			SendChasteImage(0,0,0);
#endif
			wait_til_tx_is_empty();
			xputstring("\x02\xFE\x07\x00\x00\x00\rDEBUGE\x1d",14);
			wait_til_tx_is_empty();

#if (RESET_ON_ABORT)
			reset_to_boot_code_hard();
#endif
			while (1)
				RESET_WATCHDOG_TIMER();
		}

#else		//ELSE NOT #ifdef SHOW_LIB_TRAPS

		extern "C"void __rt_raise(int,int)
		{
			reset_to_boot_code_hard();
		}
#endif		//END #ifdef SHOW_LIB_TRAPS

		// Software Reset handler
		// minmimal clean up, let watchdog take care of the reset
		// this should be an improvement over the "jump to zero" approach that was originally done here
		// as we believe it will more consistently deal with needed RESET conditions of the MX1, MMU, etc

		// To allow for a save call even in early stertup process,
		// we check the existence of the HAL and other object before we use them.
		// If any of these objects is not instanciated yet, there is no reason why we would
		// do the accociated action anyway.

		/*we call this one when something bad happenend. ESD issue for example.  Don't want to do anything crazy. Just reset ASAP */
#ifdef SHOW_RESET_TO_BOOT
		void _reset_to_boot_code_hard(int line)
		{
			char temp[80];
			// Before we are going to do suizide, we must ensure that any USB hosts do not want to talk to us.
			// The best way is to turn off the D+ pullup of the USB. This has the same effect as unplugging.
			/*    Works for any matrix we have in the field. */
			// If only simple I/O is supported, then there is no HAL, so we'll set the GPIO configuration manually to turn off D+, which may be wrong, because
			// not all devices work this way.  If the HAL is supported and is initialized, do the disconnect via the HAL function.
#if defined( theHal ) && ( !defined( SIMPLE_IO_ONLY ) )

			if( g_pHal != 0 )
			{
				theHal.DisconnectUsb();

			}
			else //( g_pHal != 0 )
			{
#endif //defined( theHal ) && ( !defined( SIMPLE_IO_ONLY ) )

#if PLATFORM_CURRENT == imx25
				/*Disconnect USB ??*/                                                    //fixme


#endif //PLATFORM_CURRENT

#if defined( theHal ) && ( !defined( SIMPLE_IO_ONLY ) )

			}//( g_pHal != 0 )
#endif //defined( theHal ) && ( !defined( SIMPLE_IO_ONLY ) )

			// Zero out the shared table
			unsigned char	*SharedTablePtr = (unsigned char *) ACTUAL_TABLE_LOCATION;
			while( SharedTablePtr != (unsigned char *) SHARED_TABLE_END )
			{
				*SharedTablePtr++ = 0;
			}

			sprintf(temp,"reset to boot line %d\r\n",line);
			DEBUG_puts(temp);

			RESET_WATCHDOG_TIMER();
			wait_til_tx_is_empty();

			// Now we can proceed to the reset
#if PLATFORM_CURRENT == imx25

			HAZARD_WRITE_FIELD_TO_16BIT_REG(WDOG_WICR,WDOG_WICR_WIE_LSH,WDOG_WICR_WIE_WID,0); 	//Disable watchdog interrupt
			HAZARD_WRITE_FIELD_TO_16BIT_REG(WDOG_WCR,WDOG_WCR_SRS_LSH,WDOG_WCR_SRS_WID,0);		//Reset

#endif //PLATFORM_CURRENT

			DISABLE_INTERRUPTS();
			while (1);			// wait til it happens, the rest should take care of itself
		}
#else //SHOW_RESET_TO_BOOT
		void reset_to_boot_code_hard(void)
		{
			// Before we are going to do suizide, we must ensure that any USB hosts do not want to talk to us.
			// The best way is to turn off the D+ pullup of the USB. This has the same effect as unplugging.
			/*    Works for any matrix we have in the field. */
			// If only simple I/O is supported, then there is no HAL, so we'll set the GPIO configuration manually to turn off D+, which may be wrong, because
			// not all devices work this way.  If the HAL is supported and is initialized, do the disconnect via the HAL function.
#if defined( theHal ) && ( !defined( SIMPLE_IO_ONLY ) )
			if( g_pHal != 0 )
			{

				theHal.DisconnectUsb();

			}

			else
			{
#endif //defined( theHal ) && ( !defined( SIMPLE_IO_ONLY ) )
#if PLATFORM_CURRENT == imx25
				/*Disconnect USB ??*/                                                    //fixme
				HAZARD_WRITE_FIELD_TO_16BIT_REG(WDOG_WICR,WDOG_WICR_WIE_LSH,WDOG_WICR_WIE_WID,0); 	//Disable watchdog interrupt
				HAZARD_WRITE_FIELD_TO_16BIT_REG(WDOG_WCR,WDOG_WCR_SRS_LSH,WDOG_WCR_SRS_WID,0);		//Reset
#endif //PLATFORM_CURRENT

#if defined( theHal ) && ( !defined( SIMPLE_IO_ONLY ) )

			}
#endif //defined( theHal ) && ( !defined( SIMPLE_IO_ONLY ) )

			// Zero out the shared table
			unsigned char	*SharedTablePtr = (unsigned char *) ACTUAL_TABLE_LOCATION;
			while( SharedTablePtr != (unsigned char *) SHARED_TABLE_END )
			{
				*SharedTablePtr++ = 0;
			}

			// Now we can proceed to the reset
#if PLATFORM_CURRENT == imx25
			HAZARD_WRITE_FIELD_TO_16BIT_REG(WDOG_WICR,WDOG_WICR_WIE_LSH,WDOG_WICR_WIE_WID,0); 	//Disable watchdog interrupt
			HAZARD_WRITE_FIELD_TO_16BIT_REG(WDOG_WCR,WDOG_WCR_SRS_LSH,WDOG_WCR_SRS_WID,0);		//Reset

#endif //PLATFORM_CURRENT

			while (1)
				; // wait til it happens, the rest should take care of itself
		}
#endif //SHOW_RESET_TO_BOOT
