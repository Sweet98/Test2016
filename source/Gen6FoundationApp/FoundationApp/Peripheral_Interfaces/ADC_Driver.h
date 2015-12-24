/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  Date:       2/2/2009
//  Tools:      RVCT Build 739
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Peripheral_Interfaces/ADC_Driver.h $
// $Revision: 1.4 $
// $Date: 2009/05/27 16:46:00EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// HEADER:  ADC_Driver.h
//
// The ADC_Driver is responsible for all A-D data.  Only the GCC FIFO is used.
//------------------------------------------------------------------------------
#ifndef ADC_DRIVER_H_
#define ADC_DRIVER_H_

#include "thread.h"

typedef enum 
{
	XP   = 0,
	YP   = 1,
	XN   = 2,
	YN   = 3,
	W    = 4,
	AUX0 = 5,
	AUX1 = 6,
	AUX2 = 7,
} ADC_Channel; 	//Channel names available on the iMX25

typedef enum 
{
	REF_External = 2, 
	REF_Internal = 3, 
	REF_XP = 0, 		//Note: Gen6 HW does not support this
	REF_YP = 1			//Note: Gen6 HW does not support this
} ADC_Reference;	//Volatage References

// ===  Function  ======================================================================
//
//  Name:  ADC_Init
//
//  Description: Turns on the Clocks for the ADC.  Resets the ADC then sets the
//					QSM to FQS and turns on interrupt masks 
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:			
//    in-out:
//       out:
//    return:	bool							returns a one when the funcion has compleated
// -------------------------------------------------------------------------------------
bool ADC_Init(void);

// ===  Function  ======================================================================
//
//  Name:  ADC_Start_Convert
//
//  Description: Starts the ADC Conversion, by setting the FQS bit, Enables the Interrupt
//					once the Convert is finished the interrupt will fire and the EOC event
//					will be signaled (see ADC_Service_IRQ)
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:			
//    in-out:
//       out:
//    return:	bool							returns a one when finished
// -------------------------------------------------------------------------------------
bool ADC_Start_Convert(void);

// ===  Function  ======================================================================
//
//  Name:  ADC_Setup
//
//  Description: Configures the channel and number of samples to fill the FIFO.
//			If SYSTEM is used for the channel 4 channels will be sampled each with 
//			4 samples.  when other channels are passed in the NumberofSamples is used
//			to fill the FIFO.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	ADC_Config		Channel			Channel to get data from
//				int				NumberofSamples	Number of Samples sent to the FIFO. 
//    in-out:
//       out:
//    return:  bool								Returns a 1 when completed 
// -------------------------------------------------------------------------------------
bool ADC_Setup(ADC_Channel, int, ADC_Reference);

// ===  Function  ======================================================================
//
//  Name:  ADC_Read_FIFO
//
//  Description: Reads the FIFO and returns the Average of each of the IDs, Only one ID
//					should be in the FIFO at one time.
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:
//       out:
//    return:  INT								Averaged ADC Data
// -------------------------------------------------------------------------------------
int  ADC_Read_FIFO(void);

// ===  Function  ======================================================================
//
//  Name:  ReadADCChannel
//
//  Description: Function to Configure the ADC and return a measurment of the ADC Channel
//			Will sleep the thread until the ADC has returned with a value.
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	ADC_Channel		Channel			Channel To Read
//				UINT			Samples			Number of Samples to take
//    in-out:
//       out:
//    return:	UINT							ADC Measurment
// -------------------------------------------------------------------------------------
int ReadADCChannel(ADC_Channel, int);

#endif /*ADC_DRIVER_H_*/

