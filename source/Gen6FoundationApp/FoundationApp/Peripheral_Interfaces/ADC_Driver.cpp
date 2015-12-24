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
// $RCSfile: Gen6FoundationApp/FoundationApp/Peripheral_Interfaces/ADC_Driver.cpp $
// $Revision: 1.7 $
// $Date: 2009/11/19 09:38:51EST $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// File:  ADC_Driver.cpp
//
//	The ADC_Driver is responsible for all A-D data.  Only the GCC FIFO is used.
//------------------------------------------------------------------------------
#if PLATFORM_CURRENT == imx25
	#include "ADC_Driver.h"
	#include "HardwareMacros.h"
	#include "iMX25_CRM.h"
	#include "imx25_adc-tsc.h"
	#include "interrupts.h"
	#include "thread.h"
	#include "language.h"

#else
	#error The ADC Driver is not used for this chip
#endif

#define  ENABLE_GCQ_EOQ_INTERUPT()	do{WRITE_FIELD_TO_REG(ADC_GCQMR,ADC_CQMR_EOQ_IRQ_MASK_LSH,ADC_CQMR_EOQ_IRQ_MASK_WID,0x0);}while(0)
#define DISABLE_GCQ_EOQ_INTERUPT()	do{WRITE_FIELD_TO_REG(ADC_GCQMR,ADC_CQMR_EOQ_IRQ_MASK_LSH,ADC_CQMR_EOQ_IRQ_MASK_WID,0x1);}while(0)

threadhandle_t ADC_Service_IRQ(void *);		//ADC Interrupt Handler

MT_Event_t ADC_EOC_Event = 0;		//ADC End of Conversion Event
static MT_Mutex_t accessADC_HW = NULL;

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
//    return:	bool			return			returns a one when the funcion has compleated
// -------------------------------------------------------------------------------------
bool ADC_Init(void)
{
#if PLATFORM_CURRENT == imx25

	WRITE_FIELD_TO_REG(CRM_CGR2,CRM_CGCR2_IPG_CLK_TCHSCRN_LSH,CRM_CGCR2_IPG_CLK_TCHSCRN_WID,0x1); //Turn On IPG CLK

	if (!READ_FIELD(ADC_TGCR,ADC_TGCR_IPG_CLK_EN_LSH,ADC_TGCR_IPG_CLK_EN_WID)) //Only Init if the ADC is not enabled
	{
		WRITE_FIELD_TO_REG(ADC_TGCR,ADC_TGCR_IPG_CLK_EN_LSH,ADC_TGCR_IPG_CLK_EN_WID,0x1); //Enable the ADC,
	
		WRITE_FIELD_TO_REG(ADC_TGCR,ADC_TGCR_TSC_RST_LSH,ADC_TGCR_TSC_RST_WID,0x1);//Reset the TSC
		while(READ_FIELD(ADC_TGCR,ADC_TGCR_TSC_RST_LSH,ADC_TGCR_TSC_RST_WID))
		{
			//Wait for the TSC reset to complete
		}
	
		WRITE_REG(ADC_TGCR,0x00130701);				//Set CLK to 1.66Mhz (600ns), Keep it Powered ON, Turn on the Internal Ref
		WRITE_FIELD_TO_REG(ADC_GCQCR,ADC_CQCR_QSM_LSH,ADC_CQCR_QSM_WID,0x2);	//Set Start Queue to FQS
		WRITE_REG(ADC_GCQSR, 0x00000073);	//Clear Flags		
	}
	
	//Setup Interrupt, Will not start until ADC_Start_Convert is called
	if(ADC_EOC_Event == 0x0)
	{
		ADC_EOC_Event = MT_CreateEvent(false);
		SetVector(ADC_Service_IRQ,VEC_TSC_INT);
		EnableInterruptNumber(VEC_TSC_INT);	// Enable interrupts from the TSC
		ENABLE_GCQ_EOQ_INTERUPT();
	}	
	if(accessADC_HW == NULL)
	{
		accessADC_HW = MT_CreateMutex(false);
	}	
	return 1;
	
#endif
}//endof ADC_Init

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
bool ADC_Start_Convert(void)
{
#if PLATFORM_CURRENT == imx25
	WRITE_REG(ADC_GCQSR, 0x00000073);	//Clear Flags
	WRITE_FIELD_TO_REG(ADC_GCQCR,ADC_CQCR_QSM_LSH,ADC_CQCR_QSM_WID,0x2);	//FQS will Start the Conversion
	WRITE_FIELD_TO_REG(ADC_GCQCR,ADC_CQCR_FQS_LSH,ADC_CQCR_FQS_WID,0x1); //Start Conversion by setting the FQS Bit
	EnableInterruptNumber(VEC_TSC_INT);
	return 1;
#endif
}//endof ADC_Start_Convert

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
bool ADC_Setup(ADC_Channel Channel, int NumberofSamples, ADC_Reference Reference)
{
#if PLATFORM_CURRENT == imx25
				
	WRITE_REG(ADC_GCC0, 0x0010160C);			//Asign Pin PRE Charge, External Ref, and Settling Time
	WRITE_FIELD_TO_REG(ADC_GCC0,ADC_xCCx_SELIN_LSH,ADC_xCCx_SELIN_WID,Channel);
	WRITE_FIELD_TO_REG(ADC_GCC0,ADC_xCCx_SELREFP_LSH,ADC_xCCx_SELREFP_WID,Reference);
	
	WRITE_REG(ADC_GCC1, 0x0000160C);
	WRITE_FIELD_TO_REG(ADC_GCC1,ADC_xCCx_SELIN_LSH,ADC_xCCx_SELIN_WID,Channel);
	WRITE_FIELD_TO_REG(ADC_GCC1,ADC_xCCx_NOS_LSH,ADC_xCCx_NOS_WID,NumberofSamples - 1); //sub 1 to account for the 0
	WRITE_FIELD_TO_REG(ADC_GCC1,ADC_xCCx_SELREFP_LSH,ADC_xCCx_SELREFP_WID,Reference);
		
	//Item 0 = Channel Precharge, Item 1 = Channel 
	WRITE_REG(ADC_GCQITEM_7_0, 0x00000010);		//Item Numbers
	WRITE_REG(ADC_GCQITEM_15_8, 0x00000000);	//Item Numbers
	WRITE_FIELD_TO_REG(ADC_GCQCR,ADC_CQCR_LAST_ITEM_ID_LSH,ADC_CQCR_LAST_ITEM_ID_WID,0x1);
	
	return 1;
#endif
}//endof ADC_Setup

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
//    return:
// -------------------------------------------------------------------------------------
int ADC_Read_FIFO(void) //ADC_Channel Channel
{
#if PLATFORM_CURRENT == imx25
	int AvgData = 0;
	int Items_Read = 0;
	
	WRITE_FIELD_TO_REG(ADC_GCQCR,ADC_CQCR_QSM_LSH,ADC_CQCR_QSM_WID,0x0);	//STOP
	WRITE_FIELD_TO_REG(ADC_GCQCR,ADC_CQCR_FQS_LSH,ADC_CQCR_FQS_WID,0x0);	//Clear FQS
	WRITE_FIELD_TO_REG(ADC_GCQSR,ADC_CQSR_EOQ_LSH,ADC_CQSR_EOQ_WID,0x1);	//Clear EOQ
	
	while(READ_FIELD(ADC_GCQSR,ADC_CQSR_EMPT_LSH,ADC_CQSR_EMPT_WID)==0) //Do this while FIFO is not Empty
	{
		AvgData += READ_FIELD(ADC_GCQFIFO,ADC_CQFIFO_ADCDATAOUT_LSH,ADC_CQFIFO_ADCDATAOUT_WID);
		Items_Read++;
	}
	
	if (Items_Read != 0)
	{
		return (AvgData/Items_Read);
	}
	else
	{
		return 0;
	}

#endif
}//endof ADC_Read_FIFO

// ===  Function  ======================================================================
//
//  Name:  ADC_Service_IRQ
//
//  Description: ADC interrupt handler.  Clears interrupt flags, sets the ADC_EOC event
//					then exits.  It does not re-enable the Interrupt or start a new 
//					conversion, ADC_Start_Convert needs to be called again to restart 
//					the conversion
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
static threadhandle_t ADC_Service_IRQ(void *)
{
	DisableInterruptNumber(VEC_TSC_INT);
	WRITE_FIELD_TO_REG(ADC_GCQCR,ADC_CQCR_QSM_LSH,ADC_CQCR_QSM_WID,0x0);	//STOP
	WRITE_FIELD_TO_REG(ADC_GCQCR,ADC_CQCR_FQS_LSH,ADC_CQCR_FQS_WID,0x0);	//Clear FQS
	WRITE_FIELD_TO_REG(ADC_GCQSR,ADC_CQSR_EOQ_LSH,ADC_CQSR_EOQ_WID,0x1);	//Clear EOQ

	if(ADC_EOC_Event != 0x0)
	{
		MT_SetEvent(ADC_EOC_Event);			//Set the Event so someone knows it happend
	}
	WRITE_REG(ADC_GCQSR, 0x00000073);		//Clear All Flags (FER, FUR, FOR, EOQ, PD)

	return NULL;
}//endof ADC_Service_IRQ

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
int ReadADCChannel(ADC_Channel Channel, int Samples)
{
	UINT ADC_MeasurementValue = 0;

	ADC_Init();		//Initilize the ADC

	MT_WaitMutex(accessADC_HW,INFINITE);

	ADC_Setup(Channel, Samples, REF_External);
	ADC_Start_Convert();
	
	if(MT_WaitEvent(ADC_EOC_Event,0x3))
	{
		MT_ResetEvent(ADC_EOC_Event);
		ADC_MeasurementValue = ADC_Read_FIFO();		//AUX0
	}
	else
	{
		ADC_MeasurementValue = 0;  //The ADC Event has timed out so signal an Error
	}
	
	MT_ReleaseMutex(accessADC_HW);
	
	return ADC_MeasurementValue;
}	//endof ReadADCChannel	


