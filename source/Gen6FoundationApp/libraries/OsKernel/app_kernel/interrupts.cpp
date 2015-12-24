//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: app_kernel/interrupts.cpp $
// $Revision: 1.17.1.12 $
// $Date: 2010/02/08 14:14:27EST $
//======================================================================================


#include <stdint.h>
#include "interrupts.h"
#include "db_chrio.h"
#include "platform.h"

#if PLATFORM_CURRENT == imx25
#include "iMX25_GPIO.h"			//General-Purpose I/O Definitions
#endif //endof PLATFORM Check

class CIoMan;
extern   CIoMan          		*g_pStdInterface;
extern   CIoMan          		*g_pDebugInterface;
extern   CIoMan          		*g_pRfInterface;
extern   CIoMan          		*g_pCurrentInterface;
extern void inc_count(void);
int ReturnAddr;
volatile unsigned int schedlock=0;
CIoMan * temp;

CIrqVector UserIsrVectors[64];	// The array of vector objects
#define VEC_BORDER 32

// ===  Function  ======================================================================
//
//         Name:  NullIsr
//
//  Description:  	ISR routine for unused interrupts if by chance we get here we
//					disable the offending interrupt
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  					intnum
//    in-out:
//       out:
//    return: 					threadhandle_t
// -------------------------------------------------------------------------------------
threadhandle_t NullIsr(void * intnum )
{
#ifdef DEVELOPMENT_SETTINGS
	// cheat    Reset to Boot code DOes Not work here as you are in a different mode, so quick and dirty cheat seems ok as I am bailing anyway
	temp = g_pCurrentInterface;
	g_pCurrentInterface = NULL;
	xputstring("Unhandled interrupt:Nivescr:");
	xputhex((unsigned int)pAitc->Nivecsr.f,8);
	xputstring("  :Fivescr:");
	xputhex((unsigned int)pAitc->Fivecsr,8);
	xputstring("\015\012");
	xputhex(ReturnAddr,8);
	xputstring("\015\012");
	xputhex((int)intnum ,8);
	g_pCurrentInterface=temp;


#endif //endof DEVELOPMENT_SETTINGS
char * temp2 =(char *)intnum;
	DisableInterruptNumber((vector_number_t)(temp2-(char *)0));
			return 0;
}//NullIsr


// ===  Function  ======================================================================
//
//         Name:  StartWDogDebugIrq
//
//  Description:
//
//		NOTE:		This function is only needed in the MXL build
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:					threadhandle_t
// -------------------------------------------------------------------------------------
#if defined(WDOGDEBUG) 
int WDogAddr=0xffffffff;
threadhandle_t WDogIsr(void *)
{
	g_pStdInterface = NULL; // cheat    Reset to Boot code DOes Not work here as you are in a different mode, so quick and dirty cheat seems ok as I am bailing anyway
	g_pCurrentInterface = NULL;
	xputstring("Watch Dog Timeout ");
	xputhex(WDogAddr,8);
	xputstring("\015\012");
	wait_til_tx_is_empty();
	*((unsigned short *)(WDOG_WICR))=(1<<14);
//	WATCHDOG.Wcr.b.WIE=0;				// Turn off watchdog interrupts, so that the watchdog we're about to force will cause a reset.

	//while (WATCHDOG.Wstr.b.TINT)		// Do this to read the TINT bit and clear it.  Probably doesn't need to be done, but this way we won't have
	//	RESET_WATCHDOG_TIMER();			// a lingering interrupt on startup.  Do this loop so that compiler won't optimize this read out.

	while (1)
		RESET_WATCHDOG_TIMER();

}//WDogIsr


// ===  Function  ======================================================================
//
//         Name:  StartWDogDebugIrq
//
//  Description:
//
//		NOTE:		This function is only needed in the MXL build
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void StartWDogDebugIrq(void)
{
	//WATCHDOG.Wcr.b.WDE=0;    //Disable so you can change the timeout
	SetVector(WDogIsr,VEC_WDOG_INT);
	UseFastInterrupt(VEC_WDOG_INT);
	EnableInterruptNumber(VEC_WDOG_INT);	// use int 0 for scheduler call
	*((unsigned short *)(WDOG_WICR))=(1<<15)+1;
	
	//WATCHDOG.Wcr.b.WIE=1;
	//WATCHDOG.Wcr.b.WDE=1;		// make sure it is enabled
	//WATCHDOG.Wcr.b.WHALT=0;
	
}//StartWDogDebugIrq

#endif //endof defined(WDOGDEBUG) && PLATFORM_CURRENT == mxl


// ===  CONSTRUCTOR  ======================================================================
//
//         Name:  CIrqVector::CIrqVector
//
// -------------------------------------------------------------------------------------
CIrqVector::CIrqVector(void)
{
	m_pIsrFunction	= &NullIsr;
	m_pUniversal	= NULL;
}//CIrqVector


// ===  Function  ======================================================================
//
//         Name:  Scheduler_ISR
//
//  Description:  	This Function Clears the Scheduler_ISR from the Interrupt Force Register
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:					threadhandle_t
// -------------------------------------------------------------------------------------
threadhandle_t Scheduler_ISR(void *)
{
	DISABLE_INTERRUPTS();
#if PLATFORM_CURRENT == imx25

	HAZARD_WRITE_FIELD_TO_REG(ASIC_INTFRCL,0x0,0x1,0x0); //Clear the Scheduler_ISR int from the Force Register
#endif //endof PLATFORM Check
	RESTORE_INTERRUPTS();
	return (threadhandle_t)-1;  //run the scheduler with the current que.
}//Scheduler_ISR


// ===  Function  ======================================================================
//
//         Name:  LockSheduler
//
//  Description:  	This Function Disables the Sheduler (Interrupt 0)
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void LockSheduler(void)
{
	DISABLE_INTERRUPTS();
	DisableInterruptNumber((vector_number_t)0);	// use int 0 for scheduler call
	schedlock++;
	RESTORE_INTERRUPTS();

}//LockSheduler


// ===  Function  ======================================================================
//
//         Name:  UnLockSheduler
//
//  Description:  	This Function Enables the Sheduler (Interrupt 0)
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void UnLockSheduler(void)
{
	DISABLE_INTERRUPTS();
	if (schedlock)
	{
		schedlock--;
	}
	if (schedlock==0)
	{
		EnableInterruptNumber((vector_number_t)0);	// use int 0 for scheduler call
	}
	RESTORE_INTERRUPTS();

}//UnLockSheduler


// ===  Function  ======================================================================
//
//         Name:  InterruptInit
//
//  Description:  	This Function initlizes the Interrupts,
//					it first makes sure each interrupt is turned off and then assigns
//					the sheduler to the Highest priority
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void InterruptInit(void)
{
	DISABLE_INTERRUPTS();
	schedlock=0;
	ZeroInterruptInit();

	for (int i=1;i<64;i++)             // manually call constructors because no top level init
	{
		DisableInterruptNumber((vector_number_t)i);
		UserIsrVectors[i].Set(NullIsr,(void *) i); //debugging
	}

	SetVector(Scheduler_ISR,(vector_number_t)0);
	SetNormalInterruptPriority((vector_number_t)0,15);


#ifdef WDOGDEBUG

	StartWDogDebugIrq();
#endif //endof WDOGDEBUG
}//InterruptInit


// ===  Function  ======================================================================
//
//         Name:  GetInterruptTypeAddrAndMask
//
//  Description:  	This Function returns a pointer the register Inttypel or Inttypeh
//					depending on the location of the interrupt number.  It will also
//					return the correct bit mask for the interrupt and location in the
//					register.
//
//		NOTE:		This function is only needed in the MXL build
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  vector_number_t		intnumber		Interrupt Number
//    in-out:  uint32_t				bitmask			Mask of the bit assigned to each interrupt
//			   GpioPort_t			pPort			pointer to Intfrcl or Intfrch
//       out:
//    return:
// -------------------------------------------------------------------------------------


// ===  Function  ======================================================================
//
//         Name:  UseFastInterrupt
//
//  Description:  	This Function Sets the Interrupt Type to Fast for the proper
//					interrupt, it does this by writing a one to the INTTYPEL or
//					INTTYPEH register
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  vector_number_t		intnumber		Interrupt Number
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void UseFastInterrupt(vector_number_t intnumber)
{
#if PLATFORM_CURRENT == imx25
	if(intnumber < VEC_BORDER)
	{
		WRITE_FIELD_TO_REG(ASIC_INTTYPEL,intnumber,INT_WID,0x1);
	}
	else
	{
		WRITE_FIELD_TO_REG(ASIC_INTTYPEH,(intnumber - VEC_BORDER),INT_WID,0x1);
	}

#endif //endof PLATFORM Check
}//UseFastInterrupt


// ===  Function  ======================================================================
//
//         Name:  UseNormalInterrupt
//
//  Description:  	This Function Sets the Interrupt Type to Normal for the proper
//					interrupt, it does this by writing a zero to the INTTYPEL or
//					INTTYPEH register
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  vector_number_t		intnumber		Interrupt Number
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void UseNormalInterrupt(vector_number_t intnumber)
{
#if PLATFORM_CURRENT == imx25
	if(intnumber < VEC_BORDER)
	{
		WRITE_FIELD_TO_REG(ASIC_INTTYPEL,intnumber,INT_WID,0x0);
	}
	else
	{
		WRITE_FIELD_TO_REG(ASIC_INTTYPEH,(intnumber - VEC_BORDER),INT_WID,0x0);
	}

#endif //endof PLATFORM Check
}//UseNormalInterrupt


// ===  Function  ======================================================================
//
//         Name:  GetInterruptForceAddrAndMask
//
//  Description:  	This Function returns a pointer the register Intfrcl or Intfrch
//					depending on the location of the interrupt number.  It will also
//					return the correct bit mask for the interrupt and location in the
//					register.
//
//		NOTE:		This function is only needed in the MXL build
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  vector_number_t		intnumber		Interrupt Number
//    in-out:  uint32_t				bitmask			Mask of the bit assigned to each interrupt
//			   GpioPort_t			pPort			pointer to Intfrcl or Intfrch
//       out:
//    return:
// -------------------------------------------------------------------------------------


// ===  Function  ======================================================================
//
//         Name:  ForceInterrupt
//
//  Description:  	This Function Sets the Force Interrupt Bit for the proper interrupt
//					It will set the bit within the INTFRCL or INTFRCH, depending on where
//					the interrupt is located.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  vector_number_t		intnumber		Interrupt Number
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void ForceInterrupt(vector_number_t intnumber)
{
#if PLATFORM_CURRENT == imx25
	if(intnumber < VEC_BORDER)
	{
		WRITE_FIELD_TO_REG(ASIC_INTFRCL,intnumber,INT_WID,0x1);
	}
	else
	{
		WRITE_FIELD_TO_REG(ASIC_INTFRCH,(intnumber - VEC_BORDER),INT_WID,0x1);
	}

#endif //endof PLATFORM Check
}//ForceInterrupt


// ===  Function  ======================================================================
//
//         Name:  ClrForceInterrupt
//
//  Description:  	This Function Clears the Force Interrupt Bit for the proper interrupt
//					It will clear the bit within the INTFRCL or INTFRCH, depending on where
//					the interrupt is located.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  vector_number_t		intnumber		Interrupt Number
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void ClrForceInterrupt(vector_number_t intnumber)
{
#if PLATFORM_CURRENT == imx25
	if(intnumber < VEC_BORDER)
	{
		WRITE_FIELD_TO_REG(ASIC_INTFRCL,intnumber,INT_WID,0x0);
	}
	else
	{
		WRITE_FIELD_TO_REG(ASIC_INTFRCH,(intnumber - VEC_BORDER),INT_WID,0x0);
	}

#endif //endof PLATFORM Check
}//ClrForceInterrupt


// ===  Function  ======================================================================
//
//         Name:  IsInterruptSourceActive
//
//  Description:  	This Function Reads the INTSRCL or the INTSRCH Register to determin
//					if the Interrupt Souce is Active
//					Returns a TRUE if it is or FASLE if not
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  vector_number_t		intnumber		Interrupt Number
//    in-out:
//       out:
//    return:  bool								True the Interrupt Source is Active
// -------------------------------------------------------------------------------------
bool IsInterruptSourceActive(vector_number_t intnumber)
{
#if PLATFORM_CURRENT == imx25
	if(intnumber < VEC_BORDER)
	{
		return READ_FIELD(ASIC_INTSRCL, intnumber, INT_WID) ? true : false;
	}
	else
	{
		return READ_FIELD(ASIC_INTSRCH, (intnumber - VEC_BORDER), INT_WID) ? true : false;
	}

#endif //endof PLATFORM Check
}//IsInterruptSourceActive


// ===  Function  ======================================================================
//
//         Name:  IsNormalInterruptPending
//
//  Description:  	This Function Reads the NIPNDL or the NIPNDH Register to determin
//					if a Normal Interrupt is Pending.
//					Returns a TRUE if it is or FASLE if not
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  vector_number_t		intnumber		Interrupt Number
//    in-out:
//       out:
//    return:  bool								True if a Normal Interrupt is Pending
// -------------------------------------------------------------------------------------
bool IsNormalInterruptPending(vector_number_t intnumber)
{
#if PLATFORM_CURRENT == imx25
	if(intnumber < VEC_BORDER)
	{
		return READ_FIELD(ASIC_NIPNDL, intnumber, INT_WID) ? true : false;
	}
	else
	{
		return READ_FIELD(ASIC_NIPNDH, (intnumber - VEC_BORDER), INT_WID) ? true : false;
	}

#endif //endof PLATFORM Check
}//IsNormalInterruptPending

// ===  Function  ======================================================================
//
//         Name:  AreAnyNormalInterruptPending
//
//  Description:  	This Function Reads the NIPNDL or the NIPNDH Register to determin
//					if a Any Normal Interrupt are Pending.
//					Returns a TRUE if it is or FASLE if not
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:  bool								True if a any Normal Interrupt is Pending
// -------------------------------------------------------------------------------------
bool AreAnyNormalInterruptPending()
{
#if PLATFORM_CURRENT == imx25
	return (READ_REG(ASIC_NIPNDL)||READ_REG(ASIC_NIPNDH)) ? true : false;


#endif //endof PLATFORM Check
}//AreAnyNormalInterruptPending

// ===  Function  ======================================================================
//
//         Name:  IsFastInterruptPending
//
//  Description:  	This Function Reads the FIPNDL or the FIPNDH Register to determin
//					if a Fast Interrupt is Pending.
//					Returns a TRUE if it is or FASLE if not
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  vector_number_t		intnumber		Interrupt Number
//    in-out:
//       out:
//    return:  bool								True if a Fast Interrupt is Pending
// -------------------------------------------------------------------------------------
bool IsFastInterruptPending(vector_number_t intnumber)
{
#if PLATFORM_CURRENT == imx25
	if(intnumber < VEC_BORDER)
	{
		return READ_FIELD(ASIC_FIPNDL, intnumber, INT_WID) ? true : false;
	}
	else
	{
		return READ_FIELD(ASIC_FIPNDH, (intnumber - VEC_BORDER), INT_WID) ? true : false;
	}

#endif //endof PLATFORM Check
}//IsFastInterruptPending

// ===  Function  ======================================================================
//
//         Name:  AreAnyFastInterruptPending
//
//  Description:  	This Function Reads the FIPNDL or the FIPNDH Register to determin
//					if a Any Fast Interrupt are Pending.
//					Returns a TRUE if it is or FASLE if not
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:  bool								True if a any Normal Interrupt is Pending
// -------------------------------------------------------------------------------------
bool AreAnyFastInterruptPending()
{
#if PLATFORM_CURRENT == imx25
	return (READ_REG(ASIC_FIPNDL)||READ_REG(ASIC_FIPNDH)) ? true : false;


#endif //endof PLATFORM Check
}//AreAnyFastInterruptPending

// ===  Function  ======================================================================
//
//         Name:  ZeroInterruptInit
//
//  Description:  	This Function Sets several Interrupt Registers to Zero.
//					This is done so that When the Interrupts are first setup,
//					no old interrupts are serviced
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void ZeroInterruptInit(void)
{
#if PLATFORM_CURRENT == imx25
	HAZARD_WRITE_REG(ASIC_INTCNTL,0x0);
	//	HAZARD_WRITE_FIELD_TO_REG(ASIC_INTCNTL,ASIC_INTCNTL_FIDIS_LSH,ASIC_INTCNTL_FIDIS_WID,0x1);   // let fast interrupts run and hold off dma

	HAZARD_WRITE_FIELD_TO_REG(ASIC_NIMASK,ASIC_NIMASK_NIMASK_LSH,ASIC_NIMASK_NIMASK_WID,0xFF); //Do not disable any Normal Interrupts

	HAZARD_WRITE_REG(ASIC_INTENABLEH, 0x0);				//Mask all interrupt sources
	HAZARD_WRITE_REG(ASIC_INTENABLEL, 0x0);

	HAZARD_WRITE_REG(ASIC_INTFRCH, 0x0);				//Clear any Forced interrupts
	HAZARD_WRITE_REG(ASIC_INTFRCL, 0x0);

	HAZARD_WRITE_REG(ASIC_INTTYPEH, 0x0);				//Set all interrupt sources to Normal
	HAZARD_WRITE_REG(ASIC_INTTYPEL, 0x0);

	HAZARD_WRITE_REG(GPIO1_IMR,0x0);					//Mask all GPIO Interrupts
	HAZARD_WRITE_REG(GPIO2_IMR,0x0);
	HAZARD_WRITE_REG(GPIO3_IMR,0x0);
	HAZARD_WRITE_REG(GPIO4_IMR,0x0);


	HAZARD_WRITE_REG(GPIO1_ISR,0xFFFFFFFF);				//Clear all GPIO Interrupt Status
	HAZARD_WRITE_REG(GPIO2_ISR,0xFFFFFFFF);
	HAZARD_WRITE_REG(GPIO3_ISR,0xFFFFFFFF);
	HAZARD_WRITE_REG(GPIO4_ISR,0xFFFFFFFF);


#endif //endof PLATFORM Check
}//ZeroInterruptInit



