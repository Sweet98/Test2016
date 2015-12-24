/*=================================================================================
  class: CWedgeHal

//=================================================================================
   $Source: interfaces_wedge_hw/WedgeHal.h $
   $Date: 2011/11/14 13:33:51EST $
   $Revision: 1.5 $


//=================================================================================*/

#ifndef THE_HAL__H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define THE_HAL__H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "Interrupts.h"
#include "InterfaceTimer.h"
#include "Gpio.h"
#include "PortDetails.h"
#include "stdint.h"

class CWedgeHal
{
	friend class CWedgeDriver;
public:
	CWedgeHal(void);
	 ~CWedgeHal(void);
	enum Ports
	{
		ILLEGAL_PORT_ID=0,
		// Place all input ports here
		FIRST_PORT_ID,
		KBD_CLK_IN = FIRST_PORT_ID,
		KBD_DATA_IN,
		TERM_CLK_IN,
		TERM_DATA_IN,
		LAST_IN_PORT_ID = TERM_DATA_IN,

		// Place all output and bidirectional ports below
		KBD_CLK_OUT,
		KBD_CLK_DIR,

		KBD_DATA_OUT,
		KBD_DATA_DIR,

		TERM_CLK_OUT,
		TERM_CLK_DIR,

		TERM_DATA_OUT,
		TERM_DATA_DIR,

		KBD_ENABLE,

		MAX_PORT_ID              //Must be at end, this sets the max number of handles allowed
	};

	enum
	{
		INVERTED		= true,
		NOT_INVERTED= false,
	};


	static void CreateWedgeHal(void);
	virtual void MakeInterfaceIoBenign(void);

	virtual void PrepareIoForWedge(void);
	void PrepareIoIrqForWedge(void);

	virtual void SetupPort(Ports port, CGpio &PortAddr, bit_number_t bitnumber, bool inverted);
	bool BitInfo(Ports port, bool &inverted);

	void KeyboardEnable (void);
	void KeyboardDisable (void);

	timer_ticks_t uSecsToTicks(timer_uSec_t uSec)	{ return m_InterfaceTimer->uSecsToTicks(uSec);}
	void ITimerSetCompareTicksFast(timer_ticks_t ticks,int num)	{ m_InterfaceTimer->SetCompareTicksFast(ticks,num);	}
	void ITimerSetCompareUSecs(timer_uSec_t uSec,int num)	{ m_InterfaceTimer->SetCompareUSecs(uSec,num);}
	void ITimerIrqEnable(void)								{ m_InterfaceTimer->IrqEnable();}
	void ITimerIrqDisable(void)								{ m_InterfaceTimer->IrqDisable();}
	unsigned long ITimerGetCounter(void){return m_InterfaceTimer->GetCounter();}
	unsigned long ITimerGetSatusReg(void){	return m_InterfaceTimer->GetSatusReg();}	// another name
	void ITimerAcknowledgeIrqFast(void)
	{
		CGpio::ClrPortFast(m_mTimerInterruptForceBitmask, m_pTimerInteruptForceRegister);  // Clr force bit
		m_InterfaceTimer->AcknowledgeTimerIrq();
	}

	void ITimerIrqForceFast(void)
	{
		CGpio::SetPortFast(m_mTimerInterruptForceBitmask, m_pTimerInteruptForceRegister);  // Set force bit
	}

	void DoClrPortFast(Ports port_id);
	void DoSetPortFast(Ports port_id);
	void DoClrPort(Ports port_id);
	void DoSetPort(Ports port_id);

	CPortDetails *GetPort(Ports port_id)
	{
		return &m_PortDetails[port_id];
	}

	void GetPort(ULONG port_id, CPortDetails *&pPort)
	{
		pPort = &m_PortDetails[port_id];
	}

	bool ReadBit(Ports port);


	// ===  Function  ======================================================================
	//
	//         Name:  PrepareCableInterfaceforUSBPower
	//
	//  Description:  Disables the Output Enables and 232 Communication pins.  This is done
	//					to draw the least amount of power in USB Suspend mode.
	//
	// - PARAMETER -------------------------------------------------------------------------
	//      Mode   Type             Name            Description
	// -------------------------------------------------------------------------------------
	//        in:
	//    in-out:
	//       out:
	//    return:
	// -------------------------------------------------------------------------------------
	virtual void PrepareCableInterfaceforUSBPower(void);

public:
	CGpio	m_Port1;
	CGpio	m_Port2;
	CGpio	m_Port3;
	CGpio	m_Port4;

protected:
	unsigned int m_mTimerInterruptForceBitmask;			 // to speed up we precalculate the IRQ force register
	GpioPort_t *m_pTimerInteruptForceRegister;

	TInterfaceTimer * m_InterfaceTimer;

	CPortDetails m_PortDetails[MAX_PORT_ID];

private:
	CWedgeHal (const CWedgeHal&);            // declared private, but not defined, to prevent copying         
	CWedgeHal& operator=(const CWedgeHal&);  // declared private, but not defined, to prevent copying 

};

#define INTERFACE_TIMER_VECTOR VEC_GPT2_INT

extern CWedgeHal           		*g_pWedgeHal;
#define  WedgeHal					(*g_pWedgeHal)


#endif



