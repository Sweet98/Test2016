/*=================================================================================
  class: TTimer

//=================================================================================
   $Source: interfaces_wedge_hw/timer.h $
   $Date: 2009/06/29 05:11:44EDT $
   $Revision: 1.2 $


//=================================================================================*/

#ifndef TIMER_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define TIMER_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "interrupts.h"
#include "product.h"
#include "stdio.h"
#include "string.h"



struct GPTimerControl_t
{
	union 
	{
      struct 
		{
			volatile unsigned long			EN					: 1;
			volatile unsigned long			ENMOD				: 1;
			volatile unsigned long			unused2				: 1;
			volatile unsigned long			WAITEN				: 1;
			volatile unsigned long			unused4				: 1;
			volatile unsigned long			STOPEN				: 1;
			volatile unsigned long			CLKSRC				: 3;
			volatile unsigned long			FRR					: 1;
			volatile unsigned long			unused10			: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			SWR					: 1;
			volatile unsigned long			IM1					: 2;
			volatile unsigned long			IM2					: 2;
			volatile unsigned long			OM1					: 3;
			volatile unsigned long			OM2					: 3;
			volatile unsigned long			OM3					: 3;
			volatile unsigned long			FO1					: 1;
			volatile unsigned long			FO2					: 1;
			volatile unsigned long			FO3					: 1;
  		} b;
  		volatile unsigned long f;
	}u;
};


typedef struct GPTimers_t
{

/* Timer 1 Control Register */

	GPTimerControl_t  Tctl;

/* Timer Prescaler Register */

	union
	{
      struct
		{
			volatile unsigned long			PRESCALER		: 12;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
  		} b;
  		volatile unsigned long f;
	} Tprer;

/* Timer 1 Status Register */

	union
	{
      struct
		{
			volatile unsigned long			OF1				: 1;
			volatile unsigned long			OF2				: 1;
			volatile unsigned long			OF3				: 1;
			volatile unsigned long			IF1				: 1;
			volatile unsigned long			IF2				: 1;
			volatile unsigned long			ROV				: 1;
			volatile unsigned long			unused6			: 1;
			volatile unsigned long			unused7			: 1;
			volatile unsigned long			unused8			: 1;
			volatile unsigned long			unused9			: 1;
			volatile unsigned long			unused10			: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
  		} b;
  		volatile unsigned long f;
	} Tstat;
	
/* Timer Interrupt Register */

	union
	{
      struct
		{
			volatile unsigned long			OF1IE				: 1;
			volatile unsigned long			OF2IE				: 1;
			volatile unsigned long			OF3IE			: 1;
			volatile unsigned long			IF1IE			: 1;
			volatile unsigned long			IF2IE			: 1;
			volatile unsigned long			ROVIE			: 1;
			volatile unsigned long			unused6			: 1;
			volatile unsigned long			unused7			: 1;
			volatile unsigned long			unused8			: 1;
			volatile unsigned long			unused9			: 1;
			volatile unsigned long			unused10			: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
  		} b;
  		volatile unsigned long f;
	} Tintr;
	
	
/* Timer Compare1 Register */

	volatile unsigned long Tcmp1;
	
/* Timer Compare2 Register */

	volatile unsigned long Tcmp2;
	
/* Timer Compare3 Register */

	volatile unsigned long Tcmp3;

/* Timer Capture1 Register */

	volatile unsigned long Tcr1;
	
/* Timer Capture1 Register */

	volatile unsigned long Tcr2;

/* Timer 1 Counter Register */

	volatile unsigned long Tcn;



} GPTIMERS_T;

#define pGPTimer1		((GPTimers_t *) 0x53F90000)
#define pGPTimer2		((GPTimers_t *) 0x53F8C000)
#define pGPTimer3		((GPTimers_t *) 0x53F88000)
#define pGPTimer4		((GPTimers_t *) 0x53F84000)


void GpTimerSoftwareReset(GPTimers_t *pTimer);

typedef ULONG timer_ticks_t;
typedef ULONG timer_uSec_t;
typedef ULONG timer_mSec_t;

class TTimer
{
public:
	TTimer(GPTimers_t *m_ADDR)
	{
		ADDR=m_ADDR;
		
	}
	~TTimer(void)
	{
	
		ADDR->Tintr.f &= ~(0x07);           // warning removal we accsess ADDR here to prevent waning
	//	IrqDisable();
		TimerDisable();

	}
	 timer_ticks_t GetCounter(void)	{ return ADDR->Tcn;		}
	 timer_ticks_t GetCompare(int num)	
	{
		switch(num)
			{
				case 1:
					return ADDR->Tcmp1;	
				case 2:
					return ADDR->Tcmp2;	
				case 3:
					return ADDR->Tcmp3;	
				default:
					return ADDR->Tcmp1;	
			}
					
	}
	 GPTimers_t* GetAddress(void)		{ return ADDR; 			}

	 void	SetCompareTicksFast(timer_ticks_t ticks,int num)	// use this only from inside a FISR
	{
		AddCompareTicksFast(ticks,num);
	}
	
	 void	SetCompareTicks(timer_ticks_t ticks,int num)
	{
		DISABLE_INTERRUPTS();
		AddCompareTicksFast(ticks,num);
		RESTORE_INTERRUPTS();
	}
	

	// Only use this function if you exactly know what you do. It is somewhat dangerous.
	// Add ticks to the current value of the compare register.
	// So it could be that the timer already passed this time
	// and we set the compare register to a value which is now in the past.
	// That means we will fire after a full turn of the timer.
	 void	AddCompareTicksFast(timer_ticks_t ticks,int num)	// use this only from inside a FISR
	{
		ticks =(ticks <17)?17:ticks;
		switch(num)
			{
				case 1:
					ADDR->Tcmp1 =ADDR->Tcn+ ticks;	// as fast as possible	
					break;
				case 2:
					ADDR->Tcmp2 =ADDR->Tcn+ ticks;	// as fast as possible
					break;
				case 3:
					ADDR->Tcmp3 =ADDR->Tcn+ ticks;	// as fast as possible
					break;
				default:
					ADDR->Tcmp1 =ADDR->Tcn+ ticks;
					break;
			}
		
	}
	
	 void	SetCompareOutputToggleFast(int num)	// use this only from inside a FISR
	{
		GPTimerControl_t Temp = ADDR->Tctl;
		switch(num)
			{
				case 1:
					Temp.u.b.IM1= 0;								// turns off the input capture
					Temp.u.b.IM2= 0;
					Temp.u.b.OM1= 1;		
					break;
				case 2:
					Temp.u.b.IM1= 0;								// turns off the input capture
					Temp.u.b.IM2= 0;
					Temp.u.b.OM2= 1;	
					break;
				case 3:
					Temp.u.b.IM1= 0;								// turns off the input capture
					Temp.u.b.IM2= 0;
					Temp.u.b.OM3= 1;	
					break;
				default:
					Temp.u.b.IM1= 0;								// turns off the input capture
					Temp.u.b.IM2= 0;
					Temp.u.b.OM1= 1;	
					break;
			}		            // toggle
		ADDR->Tctl = Temp;
	}

	 void	SetCompareOutputPulseFast(int num)	// use this only from inside a FISR
	{
		GPTimerControl_t Temp = ADDR->Tctl;
		switch(num)
			{
				case 1:
					Temp.u.b.IM1= 0;								// turns off the input capture
					Temp.u.b.IM2= 0;
					Temp.u.b.OM1= 0x04;		
					break;
				case 2:
					Temp.u.b.IM1= 0;								// turns off the input capture
					Temp.u.b.IM2= 0;
					Temp.u.b.OM2= 0x04;	
					break;
				case 3:
					Temp.u.b.IM1= 0;								// turns off the input capture
					Temp.u.b.IM2= 0;
					Temp.u.b.OM3= 0x04;	
					break;
				default:
					Temp.u.b.IM1= 0;								// turns off the input capture
					Temp.u.b.IM2= 0;
					Temp.u.b.OM1= 0x04;	
					break;
			}		            // toggle
		ADDR->Tctl = Temp;
	}

	 void	SetCompareOutputLowFast(int num)	// use this only from inside a FISR
	{
		GPTimerControl_t Temp = ADDR->Tctl;
		switch(num)
			{
				case 1:
					Temp.u.b.IM1= 0;								// turns off the input capture
					Temp.u.b.IM2= 0;
					Temp.u.b.OM1= 0x02;		
					break;
				case 2:
					Temp.u.b.IM1= 0;								// turns off the input capture
					Temp.u.b.IM2= 0;
					Temp.u.b.OM2= 0x02;	
					break;
				case 3:
					Temp.u.b.IM1= 0;								// turns off the input capture
					Temp.u.b.IM2= 0;
					Temp.u.b.OM3= 0x02;	
					break;
				default:
					Temp.u.b.IM1= 0;								// turns off the input capture
					Temp.u.b.IM2= 0;
					Temp.u.b.OM1= 0x02;	
					break;
			}		            // toggle
		ADDR->Tctl = Temp;
	}

	 void	SetCompareOutputHighFast(int num)	// use this only from inside a FISR
	{
		GPTimerControl_t Temp = ADDR->Tctl;
		switch(num)
			{
				case 1:
					Temp.u.b.IM1= 0;								// turns off the input capture
					Temp.u.b.IM2= 0;
					Temp.u.b.OM1= 0x03;		
					break;
				case 2:
					Temp.u.b.IM1= 0;								// turns off the input capture
					Temp.u.b.IM2= 0;
					Temp.u.b.OM2= 0x03;	
					break;
				case 3:
					Temp.u.b.IM1= 0;								// turns off the input capture
					Temp.u.b.IM2= 0;
					Temp.u.b.OM3= 0x03;	
					break;
				default:
					Temp.u.b.IM1= 0;								// turns off the input capture
					Temp.u.b.IM2= 0;
					Temp.u.b.OM1= 0x03;	
					break;
			}		            // toggle
		ADDR->Tctl = Temp;
	}


	 void	SetCompareOutputToggle(int num)
	{
 //		GPTimers_t *pTemp = ADDR;	         // this dummy pointer helps the compiler to create better code
		DISABLE_INTERRUPTS();
		SetCompareOutputToggleFast(num);
		RESTORE_INTERRUPTS();
	}

	 void	SetCompareOutputPulse(int num)
	{
 //		GPTimers_t *pTemp = ADDR;	         // this dummy pointer helps the compiler to create better code
		DISABLE_INTERRUPTS();
		SetCompareOutputPulseFast(num);
		RESTORE_INTERRUPTS();
	}

 void	SetCompareOutputLow(int num)
	{
 //		GPTimers_t *pTemp = ADDR;	         // this dummy pointer helps the compiler to create better code
		DISABLE_INTERRUPTS();
		SetCompareOutputLowFast(num);
		RESTORE_INTERRUPTS();
	}

 void	SetCompareOutputHigh(int num)
	{
 //		GPTimers_t *pTemp = ADDR;	         // this dummy pointer helps the compiler to create better code
		DISABLE_INTERRUPTS();
		SetCompareOutputHighFast(num);
		RESTORE_INTERRUPTS();
	}


	// input cature functions
	enum CaptureType
	{
		CaptureOff=0,
		CaptureRisingEdge=1,
		CaptureFallingEdge=2,
		CaptureBothEdges=3
	};

	 void	SetCaptureTypeFast(CaptureType Type,int num)	// use this only from inside a FISR
	{
		switch(num)
			{
				case 1:
					ADDR->Tctl.u.b.IM1= Type;
					break;
				case 2:
					ADDR->Tctl.u.b.IM2= Type;
					break;
				default:
					ADDR->Tctl.u.b.IM1= Type;
					break;
			}
	}

	 void	SetCaptureOffFast(int num)				// use this only from inside a FISR
	{
		switch(num)
			{
				case 1:
					ADDR->Tctl.u.b.IM1= CaptureOff;
					break;
				case 2:
					ADDR->Tctl.u.b.IM2= CaptureOff;
					break;
				default:
					ADDR->Tctl.u.b.IM1= CaptureOff;
					break;
			}
	}
	 void	SetCaptureRisingEdgeFast(int num)	// use this only from inside a FISR
	{
		switch(num)
			{
				case 1:
					ADDR->Tctl.u.b.IM1= CaptureRisingEdge;
					break;
				case 2:
					ADDR->Tctl.u.b.IM2= CaptureRisingEdge;
					break;
				default:
					ADDR->Tctl.u.b.IM1= CaptureRisingEdge;
					break;
			}
	}
	 void	SetCaptureFallingEdgeFast(int num)	// use this only from inside a FISR
	{
		switch(num)
			{
				case 1:
					ADDR->Tctl.u.b.IM1= CaptureFallingEdge;
					break;
				case 2:
					ADDR->Tctl.u.b.IM2= CaptureFallingEdge;
					break;
				default:
					ADDR->Tctl.u.b.IM1= CaptureFallingEdge;
					break;
			}
	}
	
	 void	SetCaptureBothEdgesFast(int num)		// use this only from inside a FISR
	{
		switch(num)
			{
				case 1:
					ADDR->Tctl.u.b.IM1= CaptureBothEdges;
					break;
				case 2:
					ADDR->Tctl.u.b.IM2= CaptureBothEdges;
					break;
				default:
					ADDR->Tctl.u.b.IM1= CaptureBothEdges;
					break;
			}
	}

	 void	SetCaptureType(CaptureType Type,int num)
	{
		DISABLE_INTERRUPTS();
		SetCaptureTypeFast(Type,num);
		RESTORE_INTERRUPTS();
	}

	 void	SetCaptureOff(int num)
	{
 //		GPTimers_t *pTemp = ADDR;	         // this dummy pointer helps the compiler to create better code
		DISABLE_INTERRUPTS();
		SetCaptureOffFast(num);
		RESTORE_INTERRUPTS();
	}
	 void	SetCaptureRisingEdge(int num)
	{
 //		GPTimers_t *pTemp = ADDR;	         // this dummy pointer helps the compiler to create better code
		DISABLE_INTERRUPTS();
		SetCaptureRisingEdgeFast(num);
		RESTORE_INTERRUPTS();
	}
	 void	SetCaptureFallingEdge(int num)
	{
 //		GPTimers_t *pTemp = ADDR;	         // this dummy pointer helps the compiler to create better code
		DISABLE_INTERRUPTS();
		SetCaptureFallingEdgeFast(num);
		RESTORE_INTERRUPTS();
	}
	 void	SetCaptureBothEdges(int num)
	{
 //		GPTimers_t *pTemp = ADDR;	         // this dummy pointer helps the compiler to create better code
		DISABLE_INTERRUPTS();
		SetCaptureBothEdgesFast(num);
		RESTORE_INTERRUPTS();
	}
	 unsigned int GetCaptureRegister(int num)
	{
		switch(num)
			{
				case 1:
					return ADDR->Tcr1;
				case 2:
					return ADDR->Tcr2;
				default:
					return ADDR->Tcr1;
			}
	}
// interupts
	 void IrqEnable(void)			{ ADDR->Tintr.f |= 0x01;  }   //Just Enable Compare1 
	 void IrqDisable(void)			{ ADDR->Tintr.f &= ~(0x07);  }
	 bool IsIrqDisabled(void)		{ return (ADDR->Tintr.b.OF1IE |ADDR->Tintr.b.OF2IE |ADDR->Tintr.b.OF3IE )  ? true : false;	}

	 void TimerEnable(void)			{ ADDR->Tctl.u.b.EN = 1;	}
	 void TimerDisable(void)		{ ADDR->Tctl.u.b.EN = 0;	}

	 bool HasTimerElapsed(void)	{ return (ADDR->Tstat.b.OF1/* |ADDR->Tstat.b.OF2 |ADDR->Tstat.b.OF3 */)  ? true : false;	}
	 bool IsTimerCapture(void)	{ return (ADDR->Tstat.b.IF1 |ADDR->Tstat.b.IF2 ) ? true : false;	}
	 unsigned long GetSatusReg(void){	return ADDR->Tstat.f;}	// another name

	// be careful here, you could kill a pending one doing it wrong!

	 void AcknowledgeTimerIrq(void){	ADDR->Tstat.f= 0x3F;}	// another name


	enum ClkSources
	{
		GPTCTL_CLKSOURCE_DISABLE=0,     		
		GPTCTL_CLKSOURCE_PERCLK1 =1,    		
		GPTCTL_CLKSOURCE_PERCLK1DIV4 =2,
		GPTCTL_CLKSOURCE_TIN =3,       	 	
		GPTCTL_CLKSOURCE_32KCLK = 7 		
	};
protected:
	 void DefaultControlRegister(void)		{ ADDR->Tctl.u.f = 0;	}
	 void SetFreeRunning(void)					{ ADDR->Tctl.u.b.FRR = 1;	}

	 void SoftwareReset(void)					{ GpTimerSoftwareReset(ADDR);		}

	 void SetClockSource(ClkSources num)	{ ADDR->Tctl.u.b.CLKSRC = num;	}
	 void SetPrescaler(int divide)			{ ADDR->Tprer.b.PRESCALER = divide-1;	}
protected:
	 GPTimers_t *ADDR;
};

#endif /* TIMER_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */


