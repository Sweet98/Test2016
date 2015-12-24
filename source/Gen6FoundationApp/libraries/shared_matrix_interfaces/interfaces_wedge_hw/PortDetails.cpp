/*=================================================================================
  class: CPortDetails

//=================================================================================
   $Source: interfaces_wedge_hw/PortDetails.cpp $
   $Date: 2009/06/29 05:11:03EDT $
   $Revision:


//=================================================================================*/

#include "stdInclude.h"
#include "Product.h"
#include "PortDetails.h"
#include "gpio.h"

CPortDetails::CPortDetails(void)
{
	m_pGpioPort = NULL;
	m_mask = 0;
	m_inverted = 0;
	m_changed = false;
	m_waiting = false;
	m_received = 0;
	m_type = INPUT;

	m_pIrqMask = NULL;
	m_pIrqStatus = NULL;
	m_pIrqConfig = NULL;
	m_irq_edge_mask = 0;
	m_irq_polarity_mask = 0;
}

void CPortDetails::DoSetPort(void)
{
	if(!m_inverted)
		DoSetPortRaw();
	else
		DoClrPortRaw();
}

void CPortDetails::DoClrPort(void)
{
	if(m_inverted)
		DoSetPortRaw();
	else
		DoClrPortRaw();
}

bool CPortDetails::IsBitSet(void)
{
	return IsBitSetRaw() ^ m_inverted;
}

void CPortDetails::PreparePortForOutput(void)        //Haven't  solved by Jess
{
	if(IsValid())
	{
		if(m_type==BIDIR)
		{
			m_pGpioPort->MakeOutput(m_mask);
		}
	}
}


