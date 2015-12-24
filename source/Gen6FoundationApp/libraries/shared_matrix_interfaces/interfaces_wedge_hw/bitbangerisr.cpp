/*=================================================================================
  class: CBitBangerISR

//=================================================================================
   $Source: interfaces_wedge_hw/bitbangerisr.cpp $
   $Date: 2011/08/30 13:16:32EDT $
   $Revision: 2.13 $


//=================================================================================*/

#include "stdInclude.h"
#include "product.h"
#include "BitBangerISR.h"
#include "InterfacetypeDefs.h"
#include "ThreadRealtime.h"
#include "interrupts.h"
#include "thread.h"
#include "WedgeHal.h"
#include "stdio.h"
#include "HardwareMacros.h"
#include "db_chrio.h"


// Store a command to the buffer
void CBitBangCmd::Store(ULONG cmd, CPortDetails *pPort, ULONG delay)
{
	m_cmd = cmd;
	m_pPort = pPort;
	m_time_param = delay; 
}

///////////////////////////////////////////////////////////////////////////////
CBitBangerISR::CBitBangerISR(size_t size)
{
	::DisableInterruptNumber(INTERFACE_TIMER_VECTOR);
	m_repeat_time_ticks = WedgeHal.uSecsToTicks(10);
	m_size = size;
	mSignalmbox=NULL;
	m_signal = 0;
	m_pCmdBuffer = NULL;            /* need to do this so when we free this later we don't crash */
	m_pCmdBufferEnd=NULL;
	// if 0 size then multi bitbang command buffers are in use
	// derived class must initialize
	if (0 != size)
	{
		m_pCmdBuffer = new CBitBangCmd[size+1];
		m_pCmdBufferEnd =&m_pCmdBuffer[size];
	}
	WedgeHal.ITimerIrqDisable();				// safety
	SetVector(&CBitBangerISR::DecodeNextCommand, INTERFACE_TIMER_VECTOR);
	UseFastInterrupt(INTERFACE_TIMER_VECTOR);							        		// borrow timer from the thread system       dangerous  Jess
	m_pCmdBufferEnd =&m_pCmdBuffer[size];
	::EnableInterruptNumber(INTERFACE_TIMER_VECTOR);
}

CBitBangerISR::~CBitBangerISR(void)
{
	delete [] m_pCmdBuffer;
	m_pCmdBuffer = NULL;
	
	if(mSignalmbox)
		MT_DeleteMailbox(mSignalmbox);
	mSignalmbox = NULL;
}

void CBitBangerISR::CreateMailBox(void)
{
	if (!mSignalmbox)
		mSignalmbox=MT_CreateMailbox(4,sizeof(unsigned long));
}
void CBitBangerISR::Close(void)
{
	// if the thread system and the bitbanger use different timers, we must turn of interrupts from our timer.
}

inline void CBitBangerISR::SetupTimerTicks(timer_ticks_t ticks)
{
	WedgeHal.ITimerSetCompareTicksFast(ticks,1);
}

inline void CBitBangerISR::SetupTimerUSec(timer_uSec_t uSec)
{
	WedgeHal.ITimerSetCompareUSecs(uSec,1);
	WedgeHal.ITimerIrqEnable();
}


inline void CBitBangerISR::SwitchBackToSlowIrqMode(void)
{
	WedgeHal.ITimerIrqDisable();				// safety
	Signal(Sig_BigBangDone);
}

void CBitBangerISR::SwitchToFastIrqMode(void)
{
	m_return_value = ERR_PASS;
	SetupTimerUSec(10);
}

void CBitBangerISR::DoAbort(void)
{
	m_pCurrentReadLocation = &m_AbortCmd;
	m_return_value = ERR_BB_READ_Aborted;
 	m_param = m_repeat_time_ticks;
}

void CBitBangerISR::DoSetAbort(CPortDetails * /* pPort */)
{
	m_return_value = ERR_BB_READ_Aborted;
}

inline void CBitBangerISR::DoKeepCurrentCommand(void)
{
	m_pCurrentReadLocation--;	// return to current command
}

inline bool CBitBangerISR::IsBitSet(CPortDetails *pPort)
{
	return pPort->IsBitSetRaw();
}

void CBitBangerISR::DoDelay(CPortDetails * /* pPort */)
{
	// do nothing here, delay is handeled by the main function
}

void CBitBangerISR::DoSetPort(CPortDetails *pPort)
{
	pPort->DoSetPortRaw();	// set bit
}

void CBitBangerISR::DoClrPort(CPortDetails *pPort)
{
	pPort->DoClrPortRaw();	// clear bit
}

void CBitBangerISR::DoTogglePort(CPortDetails *pPort)
{
	pPort->DoTogglePortRaw();	// toggle bit
}

void CBitBangerISR::DoPreparePortCheck(CPortDetails *pPort)
{
	pPort->DoPreparePortCheckFast();
}

// check if the port is high for given amount of time, if not, then abort
void CBitBangerISR::DoCheckPortHighDuration(CPortDetails *pPort)
{
	if (pPort->IsChanged())
	{
		DoAbort();
	}
	else
	{
		if (!IsBitSet(pPort))
		{
			DoAbort();
		}
	}
}

// check if the port is low for given amount of time, if not, then abort
void CBitBangerISR::DoCheckPortLowDuration(CPortDetails *pPort)
{
	if (pPort->IsChanged())
	{
		DoAbort();
	}
	else
	{
		if (IsBitSet(pPort))
		{
			DoAbort();
		}
	}
}

void CBitBangerISR::DoCheckPortHigh(CPortDetails *pPort)
{
	if (!IsBitSet(pPort))
	{
		DoAbort();
	}
}

// check if the port is low for given amount of time, if not, then abort
void CBitBangerISR::DoCheckPortLow(CPortDetails *pPort)
{
	if (IsBitSet(pPort))
	{
		DoAbort();
	}
}

void CBitBangerISR::DoPreparePortWaitHigh(CPortDetails *pPort)
{
	ULONG dummy;
	if (IsBitSet(pPort))
	{
		ReadNextCommand(dummy, pPort);	// already high, skip next command
	}
	else
	{
		pPort->DoPreparePortWaitHighFast();
	}
}

void CBitBangerISR::DoPreparePortWaitLow(CPortDetails *pPort)
{
	ULONG dummy;
	if (!IsBitSet(pPort))
	{
		ReadNextCommand(dummy, pPort);	// already low, skip next command
	}
	else
	{
		pPort->DoPreparePortWaitLowFast();
	}
}

// wait the given time until the port is high
void CBitBangerISR::DoWaitUntilPortHigh(CPortDetails *pPort)
{
	pPort->DonePortWaitFast();
	if (!IsBitSet(pPort))	               // sanity check
	{
		DoAbort();
	}
}

// wait the given time until the port is low
void CBitBangerISR::DoWaitUntilPortLow(CPortDetails *pPort)
{
	pPort->DonePortWaitFast();
	if (IsBitSet(pPort))	               // sanity check
	{
		DoAbort();
	}
}

void CBitBangerISR::DoPreparePortRisingEdge(CPortDetails *pPort)
{
	pPort->DoPreparePortWaitRisingEdgeFast();
}

void CBitBangerISR::DoPrepareFallingEdge(CPortDetails *pPort)
{
	pPort->DoPreparePortWaitFallingEdgeFast();
}

/*
 void CBitBangerISR::DoEdgeActionHigh(CPortDetails *pPort)
{
	if (IsBitSet(m_pIrqPort))	               // sanity check
	{
		(this->*m_pEdgeAction(pPort);	         // call the command
	}
	else
	{
		DoAbort();
	}
	m_pEdgeAction = &CBitBangerISR::DoSetAbort;  // return to default action
}

void CBitBangerISR::DoEdgeActionLow(CPortDetails *pPort)
{
	if (!IsBitSet(m_pIrqPort))	               // sanity check
	{
		(this->*m_pEdgeAction(pPort);	         // call the command
	}
	else
	{
		DoAbort();
	}
	m_pEdgeAction = &CBitBangerISR::DoSetAbort;  // return to default action
}
*/
void CBitBangerISR::DoPrepareForSilence(CPortDetails *pPort)
{
	pPort->DoPreparePortCheckFast();
}

void CBitBangerISR::DoCheckForSilence(CPortDetails *pPort)
{
	pPort->DisableIrqFast();
	if (pPort->IsChanged())
	{
		DoAbort();
	}
}

void CBitBangerISR::DoPrepareRead(CPortDetails *pPort)
{
	pPort->m_received = 0;
}

void CBitBangerISR::DoReadHighShiftLeft(CPortDetails *pPort)
{
	pPort->m_received = ((IsBitSet(pPort) ? 1 : 0) | pPort->m_received)<<1;
}

void CBitBangerISR::DoReadHighShiftRight(CPortDetails *pPort)
{
	pPort->m_received = ((IsBitSet(pPort) ? 0x80000000 : 0) | pPort->m_received)>>1;
}
void CBitBangerISR::DoReadLowShiftLeft(CPortDetails *pPort)
{
	pPort->m_received = ((IsBitSet(pPort) ? 0 : 1) | pPort->m_received)<<1;
}

void CBitBangerISR::DoReadLowShiftRight(CPortDetails *pPort)
{
	pPort->m_received = ((IsBitSet(pPort) ? 0 : 0x80000000) | pPort->m_received)>>1;
}

inline void CBitBangerISR::DoJump(CPortDetails * /* pPort */)
{
	m_pCurrentReadLocation = &m_pCmdBuffer[m_param];
	m_param = 0;	// m_param was not a time
}

void CBitBangerISR::DoWhilePortHigh(CPortDetails *pPort)
{
	if (!IsBitSet(pPort))		// causes a loop if bit is set
	{
		DoJump(pPort);
	}
}

void CBitBangerISR::DoWhilePortLow(CPortDetails *pPort)
{
	if (IsBitSet(pPort))		// causes a loop if bit is clear
	{
		DoJump(pPort);
	}
}

//	-------------------------------------
// DoUserDefCommand
//	A gateway back to the specific driver to do user defined command(s)
//	Receive:  ptr to port
//	Return: nothing
// ------------------------------------------
void CBitBangerISR::DoUserDefCommand(CPortDetails *pPort)
{
	ExecuteUserDefCommand(pPort);
}

//	-----------------------------------------
//	ExecuteCommand(CPortDetails *pPort)
// execute user def command
//	return: nothing
//	input: nothing
//	-----------------------------------------
void CBitBangerISR::ExecuteUserDefCommand(CPortDetails * /* pPort */)
{
	ASSERT(0);
	return;
}


void CBitBangerISR::DoEndCommand(CPortDetails * /* pPort */)
{
	SwitchBackToSlowIrqMode();	// leave the BitBang mode
}

void CBitBangerISR::DoIllegalCmd(CPortDetails * /* pPort */)
{
	ASSERT(0);
	m_return_value = ERR_BB_ILLEGAL_CMD;
	SwitchBackToSlowIrqMode();	// leave the BitBang mode
}

// This is a table of function pointers for the bitbanger commands
const CBitBangerISR::MemberFunctionPointerCmd_t CBitBangerISR::CmdTable[NUMBER_OF_COMMANDS] =
{
// preserve the order of this table!
	&CBitBangerISR::DoIllegalCmd,
	&CBitBangerISR::DoEndCommand,
	&CBitBangerISR::DoDelay,
	&CBitBangerISR::DoJump,
	&CBitBangerISR::DoSetAbort,

	&CBitBangerISR::DoSetPort,
	&CBitBangerISR::DoClrPort,

	&CBitBangerISR::DoPreparePortCheck,
	&CBitBangerISR::DoCheckPortHighDuration,
	&CBitBangerISR::DoCheckPortLowDuration,
	&CBitBangerISR::DoCheckPortHigh,
	&CBitBangerISR::DoCheckPortLow,

	&CBitBangerISR::DoPreparePortWaitHigh,
	&CBitBangerISR::DoPreparePortWaitLow,
	&CBitBangerISR::DoWaitUntilPortHigh,
	&CBitBangerISR::DoWaitUntilPortLow,

	&CBitBangerISR::DoPreparePortRisingEdge,
	&CBitBangerISR::DoPrepareFallingEdge,
//	&CBitBangerISR::DoEdgeActionHigh,
//	&CBitBangerISR::DoEdgeActionLow,

	&CBitBangerISR::DoPrepareForSilence,
	&CBitBangerISR::DoCheckForSilence,

	&CBitBangerISR::DoPrepareRead,
	&CBitBangerISR::DoReadHighShiftLeft,
	&CBitBangerISR::DoReadHighShiftRight,
	&CBitBangerISR::DoReadLowShiftLeft,
	&CBitBangerISR::DoReadLowShiftRight,

	&CBitBangerISR::DoWhilePortHigh,
	&CBitBangerISR::DoWhilePortLow,
	&CBitBangerISR::DoTogglePort,
	&CBitBangerISR::DoUserDefCommand,

	&CBitBangerISR::DoIllegalCmd,
	&CBitBangerISR::DoIllegalCmd,
	&CBitBangerISR::DoIllegalCmd
};

// This function decodes and executes the commands in the BitBang command queue
threadhandle_t CBitBangerISR::DecodeNextCommand(void)
{	
	CPortDetails *pPort=NULL;
	ULONG cmd;
	int i=10;
	do
	{	
		i--;
		ReadNextCommand(cmd, pPort);		// decode the command
		if(cmd >= (sizeof(CmdTable)/sizeof(CmdTable[0])))  // safety net
			cmd = ILLEGAL;
		(this->*CmdTable[cmd])(pPort);	// call the decoded command
		if (cmd==END_COMMAND)
		{
		 	return 0;
		}
	}
	while(m_param==0 && i);						// repeat if time was 0.
	// some commands change the m_param if it's not a time
	// CAUTION: ENSURE that there are not more than 2 consequtive commands with time=0
	SetupTimerTicks(m_param);

	WedgeHal.ITimerGetSatusReg(); 
	WedgeHal.ITimerAcknowledgeIrqFast();	

	return 0;
}


void CBitBangerISR::Signal(ULONG SignalMask)
{
	MT_AddMailbox(mSignalmbox,&SignalMask);
}

void CBitBangerISR:: PrepareWaitForSignal(ULONG SignalMask)
{
	DISABLE_INTERRUPTS();
	m_signal &=~SignalMask;
	RESTORE_INTERRUPTS();
}
ULONG CBitBangerISR:: WaitForAnySignal(ULONG SignalMask )
{
	while ((m_signal & SignalMask) == 0)
	{
		unsigned int mbsignal=0;
		if (MT_WaitMailbox(mSignalmbox,INFINITE,&mbsignal))
		{
			DISABLE_INTERRUPTS();
			m_signal |=mbsignal;
			RESTORE_INTERRUPTS();
		}
	}
	return m_signal;
}







