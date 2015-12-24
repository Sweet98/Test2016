//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/PageButton/HWPageButton.cpp $
// $Revision: 1.3 $
// $Date: 2010/04/13 11:44:54EDT $
//======================================================================================
//======================================================================================

//======================================================================================
#include "HWPageButton.h"
#include "interrupts.h"
#include "OsTimer.h"
#include "iMX25_GPIO.h"
#include "iMX25_IOMUX.h"

CHWPageButton *pCHWPaging=NULL;
void CHWPageButton::DoDebounce(void)
{
	pCHWPaging->DebouncePageButton();
}

int CHWPageButton::CreateHWPageButton(void)
{
	int ret=1;
	if (!pCHWPaging)
	{
		pCHWPaging=new CHWPageButton();
		ret =pCHWPaging->SetUpHardware();
	}
	return ret;
}
void CHWPageButton::DestroyHWPageButton(void)
{
	DISABLE_INTERRUPTS();
	UnRegisterOneMsTask(DoDebounce);
	RegisterOneMsTask(DoDebounce);
	RESTORE_INTERRUPTS();
	delete pCHWPaging;
	pCHWPaging=NULL;
}
bool CHWPageButton::IsPageButtonPulled(void)
{
	return m_PagePulled;
}
void CHWPageButton::ResetPageButton(void)
{
	 m_PagePulled=false;
}
int CHWPageButton::SetUpHardware(void)
{
	DISABLE_INTERRUPTS();
	UnRegisterOneMsTask(DoDebounce);
	RegisterOneMsTask(DoDebounce);
	*((volatile unsigned int *)IOMUXC_SW_MUX_CTL_PAD_BCLK)=MUX_MODE_5;
	*((volatile unsigned int *)GPIO4_GDIR0) &=~(1<<4);
	RESTORE_INTERRUPTS();
	return 0;
}
void CHWPageButton::PrepareForStop(void)
{
	isIntEnabled = IsInterruptSourceActive(VEC_GPIO4_INT);
	WRITE_FIELD_TO_REG(GPIO4_IMR,GPIO_BIT4_LSH,GPIO_BIT4_WID,0x1);					//Interrupt Mask Register, Turn the INT ON
	WRITE_FIELD_TO_REG(GPIO4_ICR1,GPIO_ICR4_LSH,GPIO_ICR4_WID,FALLING_EDGE_INTERRUPT);	//Configure for FALLING_EDGE_INTERRUPT
	WRITE_FIELD_TO_REG(GPIO4_ISR,GPIO_BIT4_LSH,GPIO_BIT4_WID,0x1);					//Clear ISR
	if(!isIntEnabled)
	{
		EnableInterruptNumber(VEC_GPIO4_INT);  										// Enable interrupts GPIO Port 4
	}

}
void CHWPageButton::UnPrepareForStop(void)
{
	WRITE_FIELD_TO_REG(GPIO4_IMR,GPIO_BIT4_LSH,GPIO_BIT4_WID,0x0);		//Interrupt Mask Register, Turn the INT OFF
	WRITE_FIELD_TO_REG(GPIO4_ISR,GPIO_BIT4_LSH,GPIO_BIT4_WID,0x1);		//Clear ISR
	if(!isIntEnabled)
	{
		DisableInterruptNumber(VEC_GPIO4_INT);  						// Disable interrupts GPIO Port 4
	}
}

bool CHWPageButton::IsHwPaging(void)
{

	return !(*((volatile unsigned int *)GPIO4_PSR0) &(1<<4));

}

void CHWPageButton::DebouncePageButton(void)
{

	bool read = IsHwPaging();
	if (read !=prevPagePulled)
		m_PageButtonTimer=DEBOUNCE_TIMER;
	prevPagePulled=read;
	if (m_PageButtonTimer){
		m_PageButtonTimer--;}
	else
		m_PagePulled= prevPagePulled;
}


   
