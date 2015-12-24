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
// $RCSfile: Gen6FoundationApp/FoundationApp/Peripheral_Interfaces/RTC_Driver.cpp $
// $Revision: 1.4 $
// $Date: 2010/10/13 10:59:12EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// File:  RTC_Driver.cpp
//
//		File Contains the functions used to control the Real Time Clock.
//			The Real Time Clock is used in the Manual Low Power trigger 
//			mode by Power Management to determine when to shut down the 
//			scanner.
//------------------------------------------------------------------------------

#include "RTC_Driver.h"

#if PLATFORM_CURRENT == imx25
	#include "HardwareMacros.h"
	#include "iMX25_DRYICE.h"
	#include "iMX25_INTVEC.h"
	#include "interrupts.h"
	#include "iMX25_CRM.h"
	#include "product.h"
//	#include "IndicatorLEDs.h"		//Debug

#else
	#error The Real Time Clock is not used for this chip
#endif

static threadhandle_t RTC_Alarm_IRQ(void *);

static void reg32_dryice32kdomain_write(unsigned int, unsigned int);
//static bool LedTestState = 0; //Debug


// ===  Function  ======================================================================
//
//  Name:  ZeroRTC
//
//  Description: Zeros all registers.
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:
//       out:
//    return:	
// -------------------------------------------------------------------------------------
void ZeroRTC()
{
#if PLATFORM_CURRENT == imx25
	reg32_dryice32kdomain_write(DRYICE_DCR,0x8000);		//Disable security to allow any register writes
	reg32_dryice32kdomain_write(DRYICE_DTCMR,0x0);		//Clear the RTC
#endif
} //endof ZeroRTC

// ===  Function  ======================================================================
//
//  Name:  PrepareRTC
//
//  Description: Initializes the Real Time Clock
//		Setup and start the real time clock to wake us when the power down timeout 
//		would have expired
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:
//       out:
//    return:	
// -------------------------------------------------------------------------------------

void PrepareRTC(unsigned long time)
{
#if PLATFORM_CURRENT == imx25

	DISABLE_INTERRUPTS();		
	HAZARD_WRITE_FIELD_TO_REG(CRM_CGR1,CRM_CGCR1_IPG_CLK_DRYICE_LSH,CRM_CGCR1_IPG_CLK_DRYICE_WID,0x1); //Turn On IPG CLK
	HAZARD_WRITE_REG(DRYICE_DIER, 0x00000300);
	reg32_dryice32kdomain_write(DRYICE_DCR,0x00008000);		//Disable security to allow any register writes
	reg32_dryice32kdomain_write(DRYICE_DSR,0x00000003);		//Clear any security errors
	reg32_dryice32kdomain_write(DRYICE_DCAMR,time);			//Set the Alarm Time
	reg32_dryice32kdomain_write(DRYICE_DTCMR,0x0);			//Clear the RTC, need to write to the RTCMR to get RTC started
	HAZARD_WRITE_FIELD_TO_REG(DRYICE_DIER,DRYICE_DIER_CAIE_LSH,DRYICE_DIER_CAIE_WID,0x1); //Enable the Clock-alarm interrupt
	reg32_dryice32kdomain_write(DRYICE_DCR,0x8008);			//Enable the RTC
	SetVector(RTC_Alarm_IRQ,VEC_Dry_Ice_RTC_INT);			//Assign the RTC interrupt to a function
	EnableInterruptNumber(VEC_Dry_Ice_RTC_INT);				//Enable the RTC interrupt
	RESTORE_INTERRUPTS();	

#endif
} //endof PrepareRTC

// ===  Function  ======================================================================
//
//  Name:  ClearRTC
//
//  Description: 
//			Disables the RTC interrupt	
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:
//       out:
//    return:	
// -------------------------------------------------------------------------------------
void ClearRTC()
{
#if PLATFORM_CURRENT == imx25
	reg32_dryice32kdomain_write(DRYICE_DTCMR,0x0);		//Clear the RTC, need to write to the RTCMR to get RTC started
	HAZARD_WRITE_FIELD_TO_REG(DRYICE_DIER,DRYICE_DSR_CAF_LSH,DRYICE_DSR_CAF_WID,0x1); //Disable the Clock-alarm interrupt
	HAZARD_WRITE_FIELD_TO_REG(DRYICE_DIER,DRYICE_DIER_CAIE_LSH,DRYICE_DIER_CAIE_WID,0x0); //Disable the Clock-alarm interrupt
	DisableInterruptNumber(VEC_Dry_Ice_RTC_INT);
#endif
} //endof ClearRTC

// ===  Function  ======================================================================
//
//  Name:  GetRTCSeconds
//
//  Description: Returns the Current Time in Seconds
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:
//       out:
//    return:  int								Current time in Seconds
// -------------------------------------------------------------------------------------
int GetRTCSeconds()
{
#if PLATFORM_CURRENT == imx25
	return READ_REG(DRYICE_DTCMR);
#endif
} //endof GetRTCSeconds

// ===  Function  ======================================================================
//
//  Name:  GetRTC_ALM_ISR_Status
//
//  Description: Returns the Status bit of the Alarm ISR.
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:
//       out:
//    return:  int								Current time in Seconds
// -------------------------------------------------------------------------------------
bool GetRTC_ALM_ISR_Status()
{
#if PLATFORM_CURRENT == imx25
	return READ_FIELD(DRYICE_DSR,DRYICE_DSR_CAF_LSH,DRYICE_DSR_CAF_WID) ? true : false;
#endif
} //endof GetRTCSeconds

// ===  Function  ======================================================================
//
//  Name:  RTC_Alarm_IRQ
//
//  Description: IRQ of the Alarm Interrupt.
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:
//       out:
//    return:	
// -------------------------------------------------------------------------------------
static threadhandle_t RTC_Alarm_IRQ(void *)
{
	DisableInterruptNumber(VEC_Dry_Ice_RTC_INT);

//RTC Debug Test:	
//	WRITE_FIELD_TO_REG(DRYICE_DSR,DRYICE_DSR_CAF_LSH,DRYICE_DSR_CAF_WID,0x1);	//Clear Alarm Flag
//	ZeroRTC();
//	
//	
//	if (LedTestState == 1)
//	{
//		ErrorLedOn();
//		GoodReadLedOff();
//		LedTestState = 0;
//	}
//	else
//	{
//		ErrorLedOff();
//		GoodReadLedOn();
//		LedTestState = 1;
//	}		

	return NULL;
} //endof ADC_Service_IRQ

// ===  Function  ======================================================================
//
//  Name:  reg32_dryice32kdomain_write
//
//  Description: Used to write registers of the RTC because they operate off the 32K Hz
//				  clock it takes them longer to write.  This function waits for the data
//				  to be written before returning.
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	unsigned int 	addr			Address of register to be written
//				unsigned int 	wdata			Data to be written
//    in-out:
//       out:
//    return:	
// -------------------------------------------------------------------------------------
//========Use the following to write to registers on 32kHz clock domain============
void reg32_dryice32kdomain_write(unsigned int addr , unsigned int wdata) 
{
	HAZARD_WRITE_REG(addr, wdata);
	while (READ_FIELD(DRYICE_DSR,DRYICE_DSR_WNF_LSH,DRYICE_DSR_WNF_WID) == 0x0)		//Wait for the write to compleate
	{
		//Note: System will lock up if there is a clock problem. See Chip Errata ENGcm07872
	}
	while (READ_FIELD(DRYICE_DSR,DRYICE_DSR_WCF_LSH,DRYICE_DSR_WCF_WID) == 0x0)	//Wait before you can write the next reg
	{

	}
} //endof reg32_dryice32kdomain_write



