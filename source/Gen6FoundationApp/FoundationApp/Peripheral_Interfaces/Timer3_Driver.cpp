/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  Date:       1/30/2010
//  Tools:      RVCT Build 862
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Peripheral_Interfaces/Timer3_Driver.cpp $
// $Revision: 1.4 $
// $Date: 2010/07/01 16:08:40EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// File:  Timer3_Driver.cpp
//
//------------------------------------------------------------------------------
#include "Timer3_Driver.h"
#if PLATFORM_CURRENT == imx25
	#include "HardwareMacros.h"
	#include "iMX25_IOMUX.h"
	#include "iMX25_GPIO.h"
	#include "iMX25_GPT.h"			//General Purpose Timer (GPT) Definitions
#else
	#error The Timer3 Driver is not used for this chip
#endif

#include "interrupts.h"
#include "thread.h"

//Internal Variables
static MT_Event_t Timer3_Output_Compare_1_Event = 0;
static MT_Event_t Timer3_Output_Compare_2_Event = 0;
static MT_Event_t Timer3_Output_Compare_3_Event = 0;

//Internal Functions
static void Timer3_Output_Compare_1_Setup(unsigned int);
static void Timer3_Output_Compare_2_Setup(unsigned int);
static void Timer3_Output_Compare_3_Setup(unsigned int);
static threadhandle_t Timer3_Service_IRQ(void *);

//Hardware Access Defines
#define CLEAR_OUTPUT_COMPARE_FLAG_1()		do{WRITE_FIELD_TO_REG(GPT3_GPTSR,GPT_GPTSR_OF1_LSH,GPT_GPTSR_OF1_WID,1);}while(0)		//Output compare 1 flag. This bit indicates that a compare event has occurred on output compare channel 1.
#define CLEAR_OUTPUT_COMPARE_FLAG_2()		do{WRITE_FIELD_TO_REG(GPT3_GPTSR,GPT_GPTSR_OF2_LSH,GPT_GPTSR_OF2_WID,1);}while(0)			//Write 1 to clear
#define CLEAR_OUTPUT_COMPARE_FLAG_3()		do{WRITE_FIELD_TO_REG(GPT3_GPTSR,GPT_GPTSR_OF3_LSH,GPT_GPTSR_OF3_WID,1);}while(0)

#define READ_OUTPUT_COMPARE_FLAG_1()		(READ_FIELD(GPT3_GPTSR,GPT_GPTSR_OF1_LSH,GPT_GPTSR_OF1_WID))
#define READ_OUTPUT_COMPARE_FLAG_2()		(READ_FIELD(GPT3_GPTSR,GPT_GPTSR_OF2_LSH,GPT_GPTSR_OF2_WID))
#define READ_OUTPUT_COMPARE_FLAG_3()		(READ_FIELD(GPT3_GPTSR,GPT_GPTSR_OF3_LSH,GPT_GPTSR_OF3_WID))

#define OUTPUT_COMPARE_1_INT_EN(state)			do{WRITE_FIELD_TO_REG(GPT3_GPTIR,GPT_GPTIR_OF1IE_LSH,GPT_GPTIR_OF1IE_WID,state);}while(0)	//Output Compare Interrupt Enable
#define OUTPUT_COMPARE_2_INT_EN(state)			do{WRITE_FIELD_TO_REG(GPT3_GPTIR,GPT_GPTIR_OF2IE_LSH,GPT_GPTIR_OF2IE_WID,state);}while(0)
#define OUTPUT_COMPARE_3_INT_EN(state)			do{WRITE_FIELD_TO_REG(GPT3_GPTIR,GPT_GPTIR_OF3IE_LSH,GPT_GPTIR_OF3IE_WID,state);}while(0)

#define READ_OUTPUT_COMPARE_1_INT_EN()		(READ_FIELD(GPT3_GPTIR,GPT_GPTIR_OF1IE_LSH,GPT_GPTIR_OF1IE_WID))
#define READ_OUTPUT_COMPARE_2_INT_EN()		(READ_FIELD(GPT3_GPTIR,GPT_GPTIR_OF2IE_LSH,GPT_GPTIR_OF2IE_WID))
#define READ_OUTPUT_COMPARE_3_INT_EN()		(READ_FIELD(GPT3_GPTIR,GPT_GPTIR_OF3IE_LSH,GPT_GPTIR_OF3IE_WID))

// ===  Function  ======================================================================
//
//         Name:  Timer3_Init
//
//  Description: Resets the Timer then configures the Control Register, events, and 
//					Interrupt
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void Timer3_Init(void)
{
	//***** Configure and Start the General Purpose Timer (GTP3)*****
	unsigned long ConfigTimerValue;
	if(READ_FIELD(GPT3_GPTCR,GPT_GPTCTL_EN_LSH,GPT_GPTCTL_EN_WID)==0)	//Only Init the timer if it is not running
	{
		DisableInterruptNumber(VEC_GPT3_INT);
	
		DISABLE_INTERRUPTS();		//Disable Interrupts to write to the GPIO, used the Hazard Macros that do not disable Interrupts 
		
		//Reset and initialize Timer3
		HAZARD_WRITE_FIELD_TO_REG(GPT3_GPTCR, GPT_GPTCTL_SWR_LSH, GPT_GPTCTL_SWR_WID,1);							//Reset Timer 1
		while(READ_FIELD(GPT3_GPTCR, GPT_GPTCTL_SWR_LSH, GPT_GPTCTL_SWR_WID))
		{
			//Wait for the timer reset to complete This should take 5 clock cycles
		}
		HAZARD_WRITE_FIELD_TO_REG(GPT3_GPTPR, GPT_GPTPR_PRESCALER_LSH,GPT_GPTPR_PRESCALER_WID,32);		//Divide the input clock by 33 to run the timer at (133MHz/4/33) = 1.007575MHz.
		ConfigTimerValue = (FIELD_MASK(GPT_GPTCTL_FRR_LSH, GPT_GPTCTL_FRR_WID,1)|						//Run Free
	                    	FIELD_MASK(GPT_GPTCTL_CLKSOURCE_LSH, GPT_GPTCTL_CLKSOURCE_WID,0x2)|			//Clock Source HighFreq - PERCLK (33.25MHz)
	                    	FIELD_MASK(GPT_GPTCTL_WAITEN_LSH, GPT_GPTCTL_WAITEN_WID,0x1)|				//Allow the Timer to run in Wait Mode
	                    	FIELD_MASK(GPT_GPTCTL_EN_LSH,GPT_GPTCTL_EN_WID,1));							//Enable Timer
		HAZARD_WRITE_REG(GPT3_GPTCR,ConfigTimerValue);
		
		RESTORE_INTERRUPTS();
		
		
		SetVector(Timer3_Service_IRQ,VEC_GPT3_INT);	//Setup Interrupt
		EnableInterruptNumber(VEC_GPT3_INT);
		
		//Setup Events
		if (Timer3_Output_Compare_1_Event == 0x0)
		{
			Timer3_Output_Compare_1_Event = MT_CreateEvent_Ex(false, true);		//Create an Auto Resetting Event, to signal a change in the Output Compare 1 Interrupt has fired
		}
		
		if (Timer3_Output_Compare_2_Event == 0x0)
		{
			Timer3_Output_Compare_2_Event = MT_CreateEvent_Ex(false, true);		//Create an Auto Resetting Event, to signal a change in the Output Compare 2 Interrupt has fired
		}
		
		if (Timer3_Output_Compare_3_Event == 0x0)
		{
			Timer3_Output_Compare_3_Event = MT_CreateEvent_Ex(false, true);		//Create an Auto Resetting Event, to signal a change in the Output Compare 3 Interrupt has fired
		}
		
		//Clear 
		OUTPUT_COMPARE_1_INT_EN(0);
		OUTPUT_COMPARE_2_INT_EN(0);
		OUTPUT_COMPARE_3_INT_EN(0);
	}
	
} //endof Timer3_Init

// ===  Function  ======================================================================
//
//         Name:  MicroSecondDelay
//
//  Description:  Delays a specified number of microseconds.  
//				  This is a non blocking delay and will sleep the calling thread during 
//					the delay.
//
//				  Each timer has three compare channels to delay from.  This function 
//				   	chooses the first channel not in use for its delay.  When the channel 
//				   	is configured the function waits for the interrupt to flag the event.  
//
//				  If no channels are available the function will not delay.  If a channel 
//				    is available but the event is not triggered the wait event will time  
//					out at the next largest microsecond.  In ether failure a 0 will be 
//					returned
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  unsigned int		uS_Delay_Time	The time to be loaded into the register
//												This time is in us
//    in-out:
//       out:
//    return:  bool								1 = Successful delay
//												0 = Failed to delay at all or delayed longer then intended.
// -------------------------------------------------------------------------------------
bool Timer3_MicroSecondDelay(unsigned int uSDelay)
{
	unsigned int Timeout = (uSDelay/1000)+2;
	bool Successful_Delay = false;
	
    if(uSDelay == 0x0)
    {
        return true;
    }
    
	DISABLE_INTERRUPTS();		//Disable Interrupts to prevent other threads from stepping on the timer channel
								//Each channel will enable the interrupts after it is configured.
								 
	if(READ_OUTPUT_COMPARE_1_INT_EN() == false)
	{
		Timer3_Output_Compare_1_Setup(uSDelay);
		RESTORE_INTERRUPTS();
		if(MT_WaitEvent(Timer3_Output_Compare_1_Event,Timeout))
		{
			Successful_Delay = true;
		}
		else
		{
			Successful_Delay = false;
            CLEAR_OUTPUT_COMPARE_FLAG_1();
            OUTPUT_COMPARE_1_INT_EN(0);
		}
	}
	else if(READ_OUTPUT_COMPARE_2_INT_EN() == false)
	{
		Timer3_Output_Compare_2_Setup(uSDelay);
		RESTORE_INTERRUPTS();
		if(MT_WaitEvent(Timer3_Output_Compare_2_Event,Timeout))
		{
			Successful_Delay = true;
		}
		else
		{
			Successful_Delay = false;
            CLEAR_OUTPUT_COMPARE_FLAG_2();
            OUTPUT_COMPARE_2_INT_EN(0);
		}
	} 
	else if(READ_OUTPUT_COMPARE_3_INT_EN() == false)
	{
		Timer3_Output_Compare_3_Setup(uSDelay);
		RESTORE_INTERRUPTS();
		if(MT_WaitEvent(Timer3_Output_Compare_3_Event,Timeout))
		{
			Successful_Delay = true;
		}
		else
		{
			Successful_Delay = false;
            CLEAR_OUTPUT_COMPARE_FLAG_3();
            OUTPUT_COMPARE_3_INT_EN(0);
		}
	}
	else
	{
		Successful_Delay = false;
		RESTORE_INTERRUPTS();
	}
	
	return Successful_Delay;

} //endof Timer3_MicroSecondDelay

// ===  Function  ======================================================================
//
//         Name:  Timer3_Output_Compare_1_Setup
//				  Timer3_Output_Compare_2_Setup
//				  Timer3_Output_Compare_3_Setup
//
//  Description:  Sets the Compare Value of the GPT3
//				  This is used to indicate how much time has pased for an event and
//					is needed because the GPT3 runs all the time.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  unsigned int		time			The time to be loaded into the register
//												This time is in us
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void Timer3_Output_Compare_1_Setup(unsigned int time)
{
	unsigned int temp= time + Timer3_Get_Current_Time();		//Convert input time to us add it to the current time value
	HAZARD_WRITE_REG(GPT3_GPTOCR1,temp);
	CLEAR_OUTPUT_COMPARE_FLAG_1();
	OUTPUT_COMPARE_1_INT_EN(1);
} //endof Timer3_Output_Compare_1_Setup

void Timer3_Output_Compare_2_Setup(unsigned int time)
{
	unsigned int temp= time + Timer3_Get_Current_Time();
	HAZARD_WRITE_REG(GPT3_GPTOCR2,temp);
	CLEAR_OUTPUT_COMPARE_FLAG_2();
	OUTPUT_COMPARE_2_INT_EN(1);
} //endof Timer3_Output_Compare_2_Setup

void Timer3_Output_Compare_3_Setup(unsigned int time)
{
	unsigned int temp= time + Timer3_Get_Current_Time();
	HAZARD_WRITE_REG(GPT3_GPTOCR3,temp);
	CLEAR_OUTPUT_COMPARE_FLAG_3();
	OUTPUT_COMPARE_3_INT_EN(1);
} //endof Timer3_Output_Compare_3_Setup

// ===  Function  ======================================================================
//
//         Name:  Timer3_Service_IRQ
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
static threadhandle_t Timer3_Service_IRQ(void *)
{
	if(READ_OUTPUT_COMPARE_FLAG_1() == true)
	{
		CLEAR_OUTPUT_COMPARE_FLAG_1();
		OUTPUT_COMPARE_1_INT_EN(0);
		if(Timer3_Output_Compare_1_Event != 0x0)
		{
			MT_SetEvent(Timer3_Output_Compare_1_Event);
		}
	}
	else if(READ_OUTPUT_COMPARE_FLAG_2() == true)
	{
		CLEAR_OUTPUT_COMPARE_FLAG_2();
		OUTPUT_COMPARE_2_INT_EN(0);
		if(Timer3_Output_Compare_2_Event != 0x0)
		{
			MT_SetEvent(Timer3_Output_Compare_2_Event);
		}
	} 
	else if(READ_OUTPUT_COMPARE_FLAG_3() == true)
	{
		CLEAR_OUTPUT_COMPARE_FLAG_3();
		OUTPUT_COMPARE_3_INT_EN(0);
		if(Timer3_Output_Compare_3_Event != 0x0)
		{
			MT_SetEvent(Timer3_Output_Compare_3_Event);
		}
	}

	return NULL;
} //endof Timer3_Service_IRQ

// ===  Function  ======================================================================
//
//         Name:  Timer3_Get_Current_Time
//
//  Description:  Reads the GPT3 Counter Value Register
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:  32bit					 	 GPT3 Counter Value Register
// -------------------------------------------------------------------------------------
unsigned int Timer3_Get_Current_Time(void)
{
	return ((unsigned int) READ_REG(GPT3_GPTCNT)); // Timer 3 counter value;
} //endof current_time


