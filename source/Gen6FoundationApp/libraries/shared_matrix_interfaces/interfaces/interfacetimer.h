/*=================================================================================
  class: TTimer

//=================================================================================
   $Source: interfaces/interfacetimer.h $
   $Date: 2011/08/30 13:09:29EDT $
   $Revision: 1.8.1.4 $


//=================================================================================*/
//! \file

#ifndef INTERFACE_TIMER_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define INTERFACE_TIMER_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "timer.h"
#include "stdio.h"
#include "string.h"
#include "HardwareMacros.h"	
#include "iMX25_GPT.h"

///////////////////////////////////////////////////////////////////////////////
//! A template class to create a timer for the interface drivers.
/*! Due to hardware restrictions (the MX1 has only two timers), this class is only used once in the CHal.
*/

class TInterfaceTimer : public TTimer 
{
	//! Constants
	enum
	{
		TICKS_PER_USEC	= 17,
		PRESCALER	= 1,
	};
public:
	//! Constructor reqires address of the timer.
	TInterfaceTimer(void):TTimer(pGPTimer2)
	{
		unsigned long ConfigTimerValue;
		DISABLE_INTERRUPTS();
		HAZARD_WRITE_FIELD_TO_REG(GPT2_GPTCR, GPT_GPTCTL_SWR_LSH, GPT_GPTCTL_SWR_WID,1);							
		while(READ_FIELD(GPT2_GPTCR, GPT_GPTCTL_SWR_LSH, GPT_GPTCTL_SWR_WID))
		{
			
		}
		HAZARD_WRITE_FIELD_TO_REG(GPT2_GPTPR, GPT_GPTPR_PRESCALER_LSH,GPT_GPTPR_PRESCALER_WID,1);			
		ConfigTimerValue = (FIELD_MASK(GPT_GPTCTL_FRR_LSH, GPT_GPTCTL_FRR_WID,1)|								//Run Free
	                  	    FIELD_MASK(GPT_GPTCTL_CLKSOURCE_LSH, GPT_GPTCTL_CLKSOURCE_WID,0x2)|					//Clock Source HighFreq - PERCLK (33.25MHz)
	                    	FIELD_MASK(GPT_GPTCTL_EN_LSH,GPT_GPTCTL_EN_WID,1) |
	                    	FIELD_MASK(GPT_GPTCTL_WAITEN_LSH,GPT_GPTCTL_WAITEN_WID,1) );					
		HAZARD_WRITE_REG(GPT2_GPTCR,ConfigTimerValue);
		RESTORE_INTERRUPTS();
	}

	

	//! Static helper to translate uSec into timer ticks.
	timer_ticks_t uSecsToTicks(timer_uSec_t uSec)
	{
		return uSec*TICKS_PER_USEC;
	}
	//! Configures the timer compare register.
	void	SetCompareUSecs(timer_uSec_t uSec,int num)
	{	
		DISABLE_INTERRUPTS();
		AddCompareTicksFast(uSecsToTicks(uSec),num);
		RESTORE_INTERRUPTS();
	}
	//! Static helper to translate timer ticks into uSec.
	timer_ticks_t TicksTouSecs(timer_uSec_t uSec)
	{
		return uSec/TICKS_PER_USEC;
	}
	
};


#endif /* INTERFACE_TIMER_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */




