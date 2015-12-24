//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Trigger/HWTrigger.cpp $
// $Revision: 1.19 $
// $Date: 2011/11/10 03:53:06EST $
//======================================================================================
//======================================================================================

//======================================================================================


#include "HWTrigger.h"
#include "interrupts.h"
#include "OsTimer.h"
#include "iMX25_GPIO.h"
#include "iMX25_IOMUX.h"
#include "HardwareToken.h"
#include "PluginGpioInterface.h"
#include "menu.h"
#include "ReaderConfiguration.h"

#include "db_chrio.h"        // Character I/O header prototypes
#if defined(SM4100)
#include "db_time.h"
#endif

#pragma thumb

#define DEBOUNCE_TIME_MS 4
#define DEBOUNCE_TIME_IND_MS 6


extern void TrigClick(void);
CHWTrigger *pCHWTrigger=NULL;


/*----------------------------------------------------------------------------------------
DoDebounce

  This method gets called every 1 ms via an IRQ.

  Parameters:  none

  Returns:  nothing
----------------------------------------------------------------------------------------*/
void CHWTrigger::DoDebounce(void)
{
	pCHWTrigger->DebounceTriggerButton();
}

int CHWTrigger::CreateHWTrigger(void)
{
	int ret=1;
	if (!pCHWTrigger)
	{
		if ( HardwareToken.IAmIndustrial() ) pCHWTrigger = new CHWTrigger(DEBOUNCE_TIME_IND_MS);
		else pCHWTrigger = new CHWTrigger(DEBOUNCE_TIME_MS);
		ret =pCHWTrigger->SetUpHardware();
	}
	return ret;
}

void CHWTrigger::DestroyHWTrigger(void)
{
	DISABLE_INTERRUPTS();
	UnRegisterOneMsTask(DoDebounce);
	RegisterOneMsTask(DoDebounce);
	RESTORE_INTERRUPTS();
	delete pCHWTrigger;
	pCHWTrigger=NULL;
}

bool CHWTrigger::IsTriggerPulled(void)
{
	return m_Triggered;
}

int CHWTrigger::SetUpHardware(void)
{
	DISABLE_INTERRUPTS();
	UnRegisterOneMsTask(DoDebounce);
	RegisterOneMsTask(DoDebounce);
#if defined(SM4100)
	m_start_time = current_time();
#endif
#if (defined(RFSCAN) && defined(PROTO_HW))//old cordless scanners have trigger in different location
#else	
	if(HardwareToken.IAmOEM())
	{
		//GPIO - nTRIG_uP - KPP_COL2 - GPIO3_3 
		*((volatile unsigned int *)IOMUXC_SW_MUX_CTL_PAD_KPP_COL2 )=MUX_MODE_5;
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_KPP_COL2,0x000000A0);		//Disable Pull-up
		*((volatile unsigned int *)GPIO3_GDIR0) &=~(1<<3);
		*((volatile unsigned int *)IOMUXC_SW_PAD_CTL_PAD_LBA) = 0x00000000; //disable "keeper" on Reed Switch
		
		//GPIO - nSW_TRIG - KPP_ROW0 - GPIO2_29
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_ROW0,MUX_MODE_5);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_KPP_ROW0,0x000000A0);
		HAZARD_WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,GPIO_HIGH);		
		HAZARD_WRITE_FIELD_TO_REG(GPIO2_GDIR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,GPIO_GDIR_OUTPUT);	
	}
	else
	{
		*((volatile unsigned int *)IOMUXC_SW_MUX_CTL_PAD_BCLK)=MUX_MODE_5;
		*((volatile unsigned int *)GPIO4_GDIR0) &=~(1<<4);
		*((volatile unsigned int *)IOMUXC_SW_PAD_CTL_PAD_LBA) = 0x00000000; //disable "keeper" on Reed Switch
	}
#endif
	RESTORE_INTERRUPTS();
	return 0;
}

#define FLASHOUT_INT_CONTROL(state)		do{WRITE_FIELD_TO_REG(GPIO3_IMR,GPIO_BIT23_LSH,GPIO_BIT23_WID,state);}while(0)			//Enable or Disables the FLASH_OUT interrupt bit
#define CLEAR_FLASHOUT_INT_Signal()		do{WRITE_FIELD_TO_REG(GPIO3_ISR,GPIO_BIT23_LSH,GPIO_BIT23_WID,0x1);}while(0)			//Clears the Interrupt Status Register Flag for the FLASH_OUT Signal
extern void RemoveGpioVector(void);
extern void SetGpioVector(void);

void CHWTrigger::PrepareForStop(void)
{
#if (defined(RFSCAN) && defined(PROTO_HW))
#else
	if(HardwareToken.IAmOEM())
	{
		bExtFlo = GET_SETTING(EnConnecttoExt);

		if( bExtFlo )
		{
			DisableInterruptNumber(VEC_GPIO3_INT);  						// Disable interrupts GPIO Port 3
			FLASHOUT_INT_CONTROL(false);			   
			CLEAR_FLASHOUT_INT_Signal();
			RemoveGpioVector();										//Clear the Flag if it is set
			HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT31_LSH,GPIO_BIT31_WID,GPIO_LOW);	//set data reg to LOW						   
			//DEBUG_puts("N");       
		}   //else{ DEBUG_puts("A"); }

		isIntEnabled = IsInterruptSourceActive(VEC_GPIO3_INT);
		WRITE_FIELD_TO_REG(GPIO3_IMR,GPIO_BIT3_LSH,GPIO_BIT3_WID,0x1);					//Interrupt Mask Register, Turn the INT ON
		WRITE_FIELD_TO_REG(GPIO3_ICR1,GPIO_ICR3_LSH,GPIO_ICR3_WID,FALLING_EDGE_INTERRUPT);	//Configure for LOW_LEVEL_INTERRUPT
		WRITE_FIELD_TO_REG(GPIO3_ISR,GPIO_BIT3_LSH,GPIO_BIT3_WID,0x1);					//Clear ISR
		
		if(!isIntEnabled)
		{
			EnableInterruptNumber(VEC_GPIO3_INT);  										// Enable interrupts GPIO Port 3
		}
			
	}
	else
	{
		isIntEnabled = IsInterruptSourceActive(VEC_GPIO4_INT);
		WRITE_FIELD_TO_REG(GPIO4_IMR,GPIO_BIT4_LSH,GPIO_BIT4_WID,0x1);						//Interrupt Mask Register, Turn the INT ON
		WRITE_FIELD_TO_REG(GPIO4_ICR1,GPIO_ICR4_LSH,GPIO_ICR4_WID,FALLING_EDGE_INTERRUPT);	//Configure for FALLING_EDGE_INTERRUPT so the interrupt only fires once when someone holds the trigger, it is only needed to bring us out of suspend.
		WRITE_FIELD_TO_REG(GPIO4_ISR,GPIO_BIT4_LSH,GPIO_BIT4_WID,0x1);						//Clear ISR	
		if(!isIntEnabled)
		{
			EnableInterruptNumber(VEC_GPIO4_INT);  										// Enable interrupts GPIO Port 4
		}
	}
#endif	
}

void CHWTrigger::UnPrepareForStop(void)
{
#if (defined(RFSCAN) && defined(PROTO_HW))
#else
	if(HardwareToken.IAmOEM())
	{
		WRITE_FIELD_TO_REG(GPIO3_IMR,GPIO_BIT3_LSH,GPIO_BIT3_WID,0x0);		//Interrupt Mask Register, Turn the INT OFF
		WRITE_FIELD_TO_REG(GPIO3_ISR,GPIO_BIT3_LSH,GPIO_BIT3_WID,0x1);		//Clear ISR
		
		if(!isIntEnabled)
		{
			DisableInterruptNumber(VEC_GPIO3_INT);  						// Disable interrupts GPIO Port 3
		}

		if( bExtFlo )
		{
			SetGpioVector();			   
			CLEAR_FLASHOUT_INT_Signal();										//Clear the Flag if it is set
			FLASHOUT_INT_CONTROL(true);
			EnableInterruptNumber(VEC_GPIO3_INT);  										// Enable interrupts GPIO Port 3   
		}
	}
	else
	{
		WRITE_FIELD_TO_REG(GPIO4_IMR,GPIO_BIT4_LSH,GPIO_BIT4_WID,0x0);		//Interrupt Mask Register, Turn the INT OFF
		WRITE_FIELD_TO_REG(GPIO4_ISR,GPIO_BIT4_LSH,GPIO_BIT4_WID,0x1);		//Clear ISR
		if(!isIntEnabled)
		{
			DisableInterruptNumber(VEC_GPIO4_INT);  						// Disable interrupts GPIO Port 4
		}
		
	}
#endif
}

bool CHWTrigger::IsHwTrigger(void)
{
#if (defined(RFSCAN) && defined(PROTO_HW))
#else
	if(HardwareToken.IAmOEM())
	{
		if(true == DoesPluginHaveControlofKPP_COL2())
		{
			return false;
		}
		else
		{
			return !(*((volatile unsigned int *)GPIO3_PSR0) &(1<<3));
		}
	}
	else
	{
		return !(*((volatile unsigned int *)GPIO4_PSR0) &(1<<4));
	}
#endif
}

bool CHWTrigger::ReadReedSwitch(void)
{
	return !(*((volatile unsigned int *)GPIO3_PSR0) &(1<<24));
}

void CHWTrigger::DebounceTriggerButton(void)
{
#if defined(SM4100)
	bool bIsTriggered = IsHwTrigger();
	//static int start_time = 0;
	if (GET_SETTING(IRorTrigger))
	{
		if ((bIsTriggered != m_bPrevTrigger)&&(bIsTriggered == 1))
		{
			m_TriggerButtonTimer = m_MyDebounceTimeMs;
		}
		else if ((bIsTriggered != m_bPrevTrigger)&&(bIsTriggered == 0))
		{
			if ((current_time()-m_start_time)/1000 < GET_SETTING(TriggerReleaseDebounceTimer))
			{
				m_TriggerButtonTimer = GET_SETTING(TriggerReleaseDebounceTimer)-((current_time()-m_start_time)/1000)+ m_MyDebounceTimeMs;
			}
			else
			{
				m_TriggerButtonTimer = m_MyDebounceTimeMs;
			}
		}
	}
	else 
	{
		if(bIsTriggered != m_bPrevTrigger)
			m_TriggerButtonTimer = m_MyDebounceTimeMs;
	}
		
	m_bPrevTrigger = bIsTriggered;
	if (m_TriggerButtonTimer)
	{
		if (m_TriggerButtonTimer == 1) TrigClick();
		m_TriggerButtonTimer--;
	}
	else
	{
		m_Triggered = m_bPrevTrigger;
		
		if (m_Triggered == 1)
		{
			m_start_time = current_time();
		}
	}
#else
	bool bIsTriggered = IsHwTrigger();
	if (bIsTriggered != m_bPrevTrigger)	m_TriggerButtonTimer = m_MyDebounceTimeMs;
	m_bPrevTrigger = bIsTriggered;
	if (m_TriggerButtonTimer)
	{
		if (m_TriggerButtonTimer == 1) TrigClick();
		m_TriggerButtonTimer--;
	}
	else 
	{
		m_Triggered = m_bPrevTrigger;
	}
#endif
}

