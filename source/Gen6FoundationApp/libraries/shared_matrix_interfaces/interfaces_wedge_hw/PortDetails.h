#ifndef PORT_DETAILS_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define PORT_DETAILS_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "Interrupts.h"
#include "gpio.h"

class CPortDetails
{
	friend class CBitBangerISR;
	friend class CWedgeBase;
	friend class CWedgeHal;
	enum PortType
	{
		INPUT =0, BIDIR
	};
public:
	CPortDetails(void);
	void SetupPort(CGpio &PortBase, bit_number_t bitnumber, bool inverted, PortType type)
	{
		m_mask = CGpio::GetMaskFromNumber(bitnumber);
		m_inverted = inverted;
		m_type = type;
		m_pGpioPort = &PortBase;
		m_pOutAddr = PortBase.AddrDR();
		m_pInAddr = PortBase.AddrSSR();

	// interrupt related stuff (functions are slow, so we pre-calculate it here)
		m_pIrqMask = PortBase.AddrIMR();
		m_pIrqStatus = PortBase.AddrISR();

		m_pIrqConfig = PortBase.GetIrqConfigAddr(bitnumber);
		m_irq_edge_mask = PortBase.Get2BitMaskFromNumber(bitnumber, 2);				     // level or edge triggered
		m_irq_polarity_mask = PortBase.Get2BitMaskFromNumber(bitnumber, 1);				  // rising or falling
	}

	bool IsValid(void) 						{	return (m_pGpioPort != NULL);	}
	bool IsInverted(void)					{	return m_inverted;				}

	bool BitInfo(bool &inverted)
	{
		inverted = m_inverted;
		return IsValid();
	}

	bool BitInfoRead(GpioPort_t *&addr, ULONG &bitmask, bool &inverted)
	{
		if(IsValid())
		{
			addr = GetInPortAddr();
			bitmask = m_mask;
			inverted = m_inverted;
			return true;
		}
		return false;
	}

	bool BitInfoWrite(GpioPort_t *&addr, ULONG &bitmask, bool &inverted)
	{
		if(IsValid())
		{
			addr = GetOutPortAddr();
			bitmask = m_mask;
			inverted = m_inverted;
			return true;
		}
		return false;
	}

	bool IsBitSetRaw(void)
	{
		return ((GetInPortAddr()->f) & m_mask) ? true : false;
	}

	void DoSetPortRaw(void)
	{
		GetOutPortAddr()->f |= m_mask;	// set bit
	}

	void DoClrPortRaw(void)
	{
		GetOutPortAddr()->f &= ~m_mask;	// clear bit
	}

	void DoTogglePortRaw(void)
	{
		GetOutPortAddr()->f ^= m_mask;	// toggle bit
	}

	void DoMakeInputFast(void)
	{
		GetPortObj()->MakeInputFast(m_mask);	//
	}

	void DoMakeOutputFast(void)
	{
		GetPortObj()->MakeOutputFast(m_mask);	//
	}

	ULONG GetReceived(void)
	{
		return m_received;
	}

	void DoPreparePortCheckFast(void)        //Haven't solved by Jess
	{
		m_changed = false;
		SetIrqToAutoPolarityFast();
		SetIrqToLevelTriggeredFast();
		EnableIrqFast();
	}

	void FoundChange(void)
	{
		m_changed = true;
	}

	bool IsChanged(void)
	{
		return m_changed;
	}

	void DoPreparePortWaitLowFast(void)      //Haven/t solved by Jess
	{
		SetIrqToNegativeTriggeredFast();
		SetIrqToLevelTriggeredFast();
		EnableIrqFast();
		m_changed = false;
		m_waiting = true;
	}
	void DoPreparePortWaitHighFast(void)   //Haven/t solved by Jess
	{
		SetIrqToPositiveTriggeredFast();
		SetIrqToLevelTriggeredFast();
		EnableIrqFast();
		m_changed = false;
		m_waiting = true;
	}
	void DoPreparePortWaitFallingEdgeFast(void)   //Haven/t solved by Jess
	{
		SetIrqToNegativeTriggeredFast();
		SetIrqToEdgeTriggeredFast();
		EnableIrqFast();
		m_changed = false;
		m_waiting = true;
	}
	void DoPreparePortWaitRisingEdgeFast(void)   //Haven/t solved by Jess
	{
		SetIrqToPositiveTriggeredFast();
		SetIrqToEdgeTriggeredFast();
		EnableIrqFast();
		m_changed = false;
		m_waiting = true;
	}
	void DonePortWaitFast(void)
	{
		DisableIrqFast();
		m_waiting = false;
	}

	void DoClrPort(void);
	void DoSetPort(void);
	bool IsBitSet(void);
	void PreparePortForOutput(void);

	void SetIrqToLevelTriggeredFast(void)  //Done by Jess
	{
		m_pIrqConfig->f &= ~m_irq_edge_mask;		// set bit
	}
	void SetIrqToEdgeTriggeredFast(void)    //Done by Jess
	{
		m_pIrqConfig->f |= m_irq_edge_mask;	// clr bit
	}
	void SetIrqToNegativeTriggeredFast(void)   //Haven't solved by Jess
	{
		m_pIrqConfig->f &= ~m_irq_edge_mask;
		m_pIrqConfig->f &= ~m_irq_polarity_mask;	// set bit
	}
	void SetIrqToPositiveTriggeredFast(void)    //Haven't solved by Jess
	{
		m_pIrqConfig->f |= m_irq_polarity_mask;	// clr bit
		m_pIrqConfig->f &= ~m_irq_edge_mask;
	}
	void SetIrqToAutoPolarityFast(void)     //Haven't solved by Jess
	{
		if(IsBitSetRaw())
		{
			SetIrqToNegativeTriggeredFast();
		}
		else
		{
			SetIrqToPositiveTriggeredFast();
		}
	}
	void EnableIrqFast(void)
	{
		m_pIrqStatus->f |= m_mask;	// clr old interrupt event by writing a 1
		m_pIrqMask->f |= m_mask;	// set mask to enable
	}
	void DisableIrqFast(void)
	{
		m_pIrqMask->f &= ~m_mask;	// clr mask to disable
		m_pIrqStatus->f |= m_mask;	// clr interrupt event by writing a 1
	}
	void AckIrqFast(void)
	{
		m_pIrqStatus->f |= m_mask;	// clr interrupt event by writing a 1
	}

	bool IsIrq(void)
	{
		return (m_pIrqStatus->f & m_mask) ? true : false;	// test bit
	}
  	bool CheckIrqFast(void)
	{
		if(IsIrq())
		{
			DisableIrqFast();
			FoundChange();
			return m_waiting;
		}
		return false;
	}

	CGpio *GetPortObj(void)					{	return m_pGpioPort; 	}
	GpioPort_t *GetOutPortAddr(void)		{	return m_pOutAddr; 	}
	ULONG GetMask(void)						{	return m_mask;			}

protected:

	GpioPort_t *GetInPortAddr(void)		{	return m_pInAddr; 	}

	CGpio *m_pGpioPort;
	GpioPort_t *m_pOutAddr;
	GpioPort_t *m_pInAddr;
	ULONG m_mask;
	bool	m_inverted;

	GpioPort_t *m_pIrqMask;
	GpioPort_t *m_pIrqStatus;
	GpioPort_t *m_pIrqConfig;
	ULONG m_irq_edge_mask;				     // level or edge triggered
	ULONG m_irq_polarity_mask;				  // rising or falling

	PortType m_type;
	bool	m_changed;
	bool	m_waiting;
	ULONG	m_received;
};

#endif /* PORT_DETAILS_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */


