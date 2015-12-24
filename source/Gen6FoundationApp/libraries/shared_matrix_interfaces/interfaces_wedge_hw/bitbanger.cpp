/*=================================================================================
  class: CBitBanger

//=================================================================================
   $Source: interfaces_wedge_hw/bitbanger.cpp $
   $Date: 2009/06/29 05:04:57EDT $
   $Revision: 2.2 $


//=================================================================================*/

#include "stdInclude.h"
#include "product.h"
#include "BitBanger.h"
#include "ThreadRealtime.h"
#include "InterfacetypeDefs.h"
#include "WedgeHal.h"
//#include "HwTrace.h"

#define ILLEGAL_PORT NULL

CBitBanger::CBitBanger(size_t size)
: CBitBangerISR(size)
{
	// a 0 size indicates a multibuffer system of bit bang commands
	// the derived class must initialize the command buffers
	if (0 != size)
		ResetCommands();

	m_min_time = 0;
}

CBitBanger::~CBitBanger(void)
{
}

bool CBitBanger::CheckOddParity(ULONG val, int numbits)
{
	int Parity=0;
	ULONG BitMask = 1;
	for( ; numbits>0; numbits-- , (BitMask <<= 1))
	{
		Parity += (BitMask&val) ? 1 : 0;
	}
	return (Parity&1) ? true : false;
}


MatrixError_t CBitBanger::BitBangInTerm(unsigned int &  /* uiReturnData */)
{
	return ERR_BB_READ_NoData;
}
MatrixError_t CBitBanger::BitBangInKbd(unsigned int & /* uiReturnData */)
{
	return ERR_BB_READ_NoData;
}

MatrixError_t CBitBanger::BitBangOutKbd(unsigned int /* ucData */, unsigned int /* exinfo */)
{
	return ERR_PASS;
}

void CBitBanger::NotifyOutTermDelay(ULONG /* uSecDelay */)  // just a dummy for most interfaces (used by USB)
{
}

void CBitBanger::ResetCommands(void)
{
	m_pCurrentFillLocation = &m_pCmdBuffer[OFFSET_CMDS];
	m_CurrentPosition=OFFSET_CMDS;

	m_AbortCmd.Store(END_COMMAND, ILLEGAL_PORT, 0);		// safety net
	m_GoodExitCmd.Store(END_COMMAND, ILLEGAL_PORT, 0);	// safety net
	m_pCmdBuffer[OFFSET_CMDS].Store(ILLEGAL, ILLEGAL_PORT,  0);				// safety net
}

MatrixError_t CBitBanger::RunBigBanger(void)
{

	Done();	// add the endcommand

	PrepareWaitForSignal(Sig_BigBangDone);

	m_pCurrentReadLocation = &m_pCmdBuffer[OFFSET_CMDS];
	SwitchToFastIrqMode();					  // now the bits get transfered in fast interrupt mode

	WaitForAnySignal(Sig_BigBangDone);  

	return m_return_value;
}


ULONG CBitBanger::GetReceivedPortData(CWedgeHal::Ports Port)
{
	CPortDetails *pPort;
	WedgeHal.GetPort(Port, pPort);
	ASSERT(pPort!=NULL);
	return pPort->GetReceived();
}

ULONG CBitBanger::TranslateIntoTimerTicks(ULONG uSec)
{
	return WedgeHal.uSecsToTicks(uSec);
}

// This is used to write an illegal command into th command queue as a safety net
void CBitBanger::WriteIllegalCommand(void)
{
	m_pCmdBuffer[m_CurrentPosition].Store(ILLEGAL, ILLEGAL_PORT, 0);	// safety net
}

void CBitBanger::AddCommand(Commands Cmd, ULONG port_id, ULONG param2)
{
	CPortDetails *pPort;
	if(port_id == CWedgeHal::ILLEGAL_PORT_ID)
		pPort = ILLEGAL_PORT;
	else
		GetPort(port_id, pPort);

	m_pCurrentFillLocation->Store(Cmd, pPort, param2);
	m_pCurrentFillLocation++;
	if(++m_CurrentPosition >= m_size)	// avoid overflow
	{
		ASSERT(0);
		m_pCurrentFillLocation--;
		m_CurrentPosition--;
	}
}

void CBitBanger::AddTimeCommand(Commands Cmd, ULONG port_id, ULONG delaytime)
{
	delaytime = TranslateIntoTimerTicks(delaytime);

	AddCommand(Cmd, port_id, delaytime);
}

void CBitBanger::OnAbortResumeHere(void)
{
	size_t destination = m_CurrentPosition;
	m_AbortCmd.Store(JUMP, ILLEGAL_PORT, destination);

	WriteIllegalCommand();	// safety net
}

void CBitBanger::OnGoodExitResumeHere(void)
{
	size_t destination = m_CurrentPosition;
	m_GoodExitCmd.Store(JUMP, ILLEGAL_PORT, destination);

	WriteIllegalCommand();	// safety net
}


void CBitBanger::Done(void)
{
	AddTimeCommand(END_COMMAND, 0, 1);	  // need a time != 0 to avoid a endless loop at the end
}

void CBitBanger::PrepareWaitForSilence(CWedgeHal::Ports Port, ULONG delaytime)
{
	AddTimeCommand(PREPARE_WAIT_FOR_SILENCE, Port, delaytime);
}

void CBitBanger::CheckForSilence(CWedgeHal::Ports Port, ULONG delaytime)
{
	AddTimeCommand(CHECK_FOR_SILENCE, Port, delaytime);
}

void CBitBanger::SetBit(CWedgeHal::Ports Port, ULONG delaytime)
{
	bool inverted;
	Commands Cmd;
	WedgeHal.BitInfo(Port, inverted);

	Cmd = inverted ? CLR_PORT : SET_PORT;
	AddTimeCommand(Cmd, Port, delaytime);
}

void CBitBanger::ClrBit(CWedgeHal::Ports Port, ULONG delaytime)
{
	bool inverted;
	Commands Cmd;
	WedgeHal.BitInfo(Port, inverted);

	Cmd = inverted ? SET_PORT : CLR_PORT;
	AddTimeCommand(Cmd, Port, delaytime);
}

void CBitBanger::CheckBitHighDuration(CWedgeHal::Ports Port, ULONG delaytime)
{
	bool inverted;
	Commands Cmd;
	WedgeHal.BitInfo(Port, inverted);

	AddTimeCommand(PREPARE_PORT_CHECK, Port, delaytime);

	Cmd = inverted ? CHECK_PORT_LOW_DURATION : CHECK_PORT_HIGH_DURATION;
	AddTimeCommand(Cmd, Port, m_min_time);
}

void CBitBanger::CheckBitLowDuration(CWedgeHal::Ports Port, ULONG delaytime)
{
	bool inverted;
	Commands Cmd;
	WedgeHal.BitInfo(Port, inverted);

	AddTimeCommand(PREPARE_PORT_CHECK, Port, delaytime);

	Cmd = inverted ? CHECK_PORT_HIGH_DURATION : CHECK_PORT_LOW_DURATION;
	AddTimeCommand(Cmd, Port, m_min_time);
}

void CBitBanger::CheckBitHigh(CWedgeHal::Ports Port, ULONG delaytime)
{
	bool inverted;
	Commands Cmd;
	WedgeHal.BitInfo(Port, inverted);

	Cmd = inverted ? CHECK_PORT_LOW : CHECK_PORT_HIGH;
	AddTimeCommand(Cmd, Port, delaytime);
}

void CBitBanger::CheckBitLow(CWedgeHal::Ports Port, ULONG delaytime)
{
	bool inverted;
	Commands Cmd;
	WedgeHal.BitInfo(Port, inverted);

	Cmd = inverted ? CHECK_PORT_HIGH : CHECK_PORT_LOW;
	AddTimeCommand(Cmd, Port, delaytime);
}

void CBitBanger::WaitForBitHigh(CWedgeHal::Ports Port, ULONG delaytime)
{
	bool inverted;
	Commands Cmd;
	WedgeHal.BitInfo(Port, inverted);

	Cmd = inverted ? PREPARE_PORT_WAIT_LOW : PREPARE_PORT_WAIT_HIGH;
	AddTimeCommand(Cmd, Port, delaytime);

	Cmd = inverted ? WAIT_PORT_LOW : WAIT_PORT_HIGH;
	AddTimeCommand(Cmd, Port, 5);
}

void CBitBanger::WaitForBitLow(CWedgeHal::Ports Port, ULONG delaytime)
{
	bool inverted;
	Commands Cmd;
	WedgeHal.BitInfo(Port, inverted);

	Cmd = inverted ? PREPARE_PORT_WAIT_HIGH : PREPARE_PORT_WAIT_LOW;
	AddTimeCommand(Cmd, Port, delaytime);

	Cmd = inverted ? WAIT_PORT_HIGH : WAIT_PORT_LOW;
	AddTimeCommand(Cmd, Port, 5);
}

void CBitBanger::WaitForRisingEdge(CWedgeHal::Ports Port, ULONG delaytime)
{
	bool inverted;
	Commands Cmd;
	WedgeHal.BitInfo(Port, inverted);

	Cmd = inverted ? PREPARE_PORT_WAIT_FALLING : PREPARE_PORT_WAIT_RISING;
	AddTimeCommand(Cmd, Port, delaytime);

	Cmd = inverted ? WAIT_PORT_LOW : WAIT_PORT_HIGH;
	AddTimeCommand(Cmd, Port, 0);
}

void CBitBanger::WaitForFallingEdge(CWedgeHal::Ports Port, ULONG delaytime)
{
	bool inverted;
	Commands Cmd;
	WedgeHal.BitInfo(Port, inverted);

	Cmd = inverted ? PREPARE_PORT_WAIT_RISING : PREPARE_PORT_WAIT_FALLING;
	AddTimeCommand(Cmd, Port, delaytime);

	Cmd = inverted ? WAIT_PORT_HIGH : WAIT_PORT_LOW;
	AddTimeCommand(Cmd, Port, 0);
}

void CBitBanger::KeepWhilePosition(void)
{
	m_WhilePosition = m_CurrentPosition;
}

void CBitBanger::ResolvePosition(void)
{
	m_pCmdBuffer[m_WhilePosition].StoreParm(m_CurrentPosition);
//	m_WhilePosition = BAD_ENTRY;
	WriteIllegalCommand();	               // a safety net
}

void CBitBanger::WhilePortLow(CWedgeHal::Ports Port)
{
	bool inverted;
	Commands Cmd;
	WedgeHal.BitInfo(Port, inverted);

	KeepWhilePosition();
	Cmd = inverted ? WHILE_PORT_HIGH : WHILE_PORT_LOW;
	AddCommand(Cmd, Port, 0);
}

void CBitBanger::WhilePortHigh(CWedgeHal::Ports Port)
{
	bool inverted;
	Commands Cmd;
	WedgeHal.BitInfo(Port, inverted);

	KeepWhilePosition();
	Cmd = inverted ? WHILE_PORT_LOW : WHILE_PORT_HIGH;
	AddCommand(Cmd, Port, 0);
}

void CBitBanger::EndWhile(void)
{
	AddCommand(JUMP, CWedgeHal::ILLEGAL_PORT_ID, m_WhilePosition);	// add the jump back to the beginning of the loop
	ResolvePosition();	// no fix the forward jump to here
}

void CBitBanger::SetAbort(void)
{
	AddTimeCommand(SET_ABORT, CWedgeHal::ILLEGAL_PORT_ID, 0);
}

inline void CBitBanger::ReadPortShiftLeft(CWedgeHal::Ports Port, ULONG delaytime)
{
	bool inverted;
	Commands Cmd;
	WedgeHal.BitInfo(Port, inverted);

	Cmd = inverted ? READ_PORT_LOW_LEFT_SHIFT : READ_PORT_HIGH_LEFT_SHIFT;
	AddTimeCommand(Cmd, Port, delaytime);
}

inline void CBitBanger::ReadPortShiftRight(CWedgeHal::Ports Port, ULONG delaytime)
{
	bool inverted;
	Commands Cmd;
	WedgeHal.BitInfo(Port, inverted);

	Cmd = inverted ? READ_PORT_LOW_RIGHT_SHIFT : READ_PORT_HIGH_RIGHT_SHIFT;
	AddTimeCommand(Cmd, Port, delaytime);
}

void CBitBanger::KbdDat_ReadShiftLeft(ULONG delaytime)
{
	ReadPortShiftLeft(CWedgeHal::KBD_DATA_IN, delaytime);
}
void CBitBanger::KbdDat_ReadShiftRight(ULONG delaytime)
{
	ReadPortShiftRight(CWedgeHal::KBD_DATA_IN, delaytime);
}
void CBitBanger::TermDat_ReadShiftLeft(ULONG delaytime)
{
	ReadPortShiftLeft(CWedgeHal::TERM_DATA_IN, delaytime);
}
void CBitBanger::TermDat_ReadShiftRight(ULONG delaytime)
{
	ReadPortShiftRight(CWedgeHal::TERM_DATA_IN, delaytime);
}

void CBitBanger::DisableKeyboard(ULONG delaytime)
{
	ClrBit(CWedgeHal::KBD_ENABLE, delaytime);
}

void CBitBanger::EnableKeyboard(ULONG delaytime)
{
	SetBit(CWedgeHal::KBD_ENABLE, delaytime);
}

void CBitBanger::TermClk_Output(ULONG delaytime)
{
	SetBit(CWedgeHal::TERM_CLK_DIR, delaytime);
}

void CBitBanger::TermClk_Low(ULONG delaytime)
{
	ClrBit(CWedgeHal::TERM_CLK_OUT, delaytime);
}

void CBitBanger::TermClk_High(ULONG delaytime)
{
	SetBit(CWedgeHal::TERM_CLK_OUT, delaytime);
}

void CBitBanger::TermClk_HighOc(ULONG delaytime)
{
	TermClk_High(0);
	TermClk_HighZ(delaytime);
}

void CBitBanger::TermClk_HighZ(ULONG delaytime)
{
	ClrBit(CWedgeHal::TERM_CLK_DIR, delaytime);
}

void CBitBanger::TermClk_CheckHigh(ULONG delaytime)
{
	CheckBitHigh(CWedgeHal::TERM_CLK_IN, delaytime);
}

void CBitBanger::TermClk_CheckLow(ULONG delaytime)
{
	CheckBitLow(CWedgeHal::TERM_CLK_IN, delaytime);
}

void CBitBanger::TermClk_WaitHigh(ULONG delaytime)
{
	WaitForBitHigh(CWedgeHal::TERM_CLK_IN, delaytime);
}

void CBitBanger::TermClk_WaitLow(ULONG delaytime)
{
	WaitForBitLow(CWedgeHal::TERM_CLK_IN, delaytime);
}

void CBitBanger::TermDat_Output(ULONG delaytime)
{
	SetBit(CWedgeHal::TERM_DATA_DIR, delaytime);
}

void CBitBanger::TermDat_Low(ULONG delaytime)
{
	ClrBit(CWedgeHal::TERM_DATA_OUT, delaytime);
}

void CBitBanger::TermDat_High(ULONG delaytime)
{
	SetBit(CWedgeHal::TERM_DATA_OUT, delaytime);
}

void CBitBanger::TermDat_HighOc(ULONG delaytime)
{
	TermDat_High(0);
	TermDat_HighZ(delaytime);
}

void CBitBanger::TermDat_HighZ(ULONG delaytime)
{
	ClrBit(CWedgeHal::TERM_DATA_DIR, delaytime);
}

void CBitBanger::TermDat_CheckHigh(ULONG delaytime)
{
	CheckBitHigh(CWedgeHal::TERM_DATA_IN, delaytime);
}

void CBitBanger::TermDat_CheckLow(ULONG delaytime)
{
	CheckBitLow(CWedgeHal::TERM_DATA_IN, delaytime);
}

void CBitBanger::TermDat_WaitHigh(ULONG delaytime)
{
	WaitForBitHigh(CWedgeHal::TERM_DATA_IN, delaytime);
}

void CBitBanger::TermDat_WaitLow(ULONG delaytime)
{
	WaitForBitLow(CWedgeHal::TERM_DATA_IN, delaytime);
}

void CBitBanger::TermDat_PrepareWaitForSilence(ULONG delaytime)
{
	PrepareWaitForSilence(CWedgeHal::TERM_DATA_IN, delaytime);
}

void CBitBanger::TermClk_PrepareWaitForSilence(ULONG delaytime)
{
	PrepareWaitForSilence(CWedgeHal::TERM_CLK_IN, delaytime);
}

void CBitBanger::TermDat_CheckForSilence(ULONG delaytime)
{
	CheckForSilence(CWedgeHal::TERM_DATA_IN, delaytime);
}

void CBitBanger::TermClk_CheckForSilence(ULONG delaytime)
{
	CheckForSilence(CWedgeHal::TERM_CLK_IN, delaytime);
}

void CBitBanger::PrepareWaitForSilence(ULONG delaytime)
{
	PrepareWaitForSilence(CWedgeHal::TERM_DATA_IN, 10);
	PrepareWaitForSilence(CWedgeHal::TERM_CLK_IN, delaytime);
}

void CBitBanger::CheckForSilence(ULONG delaytime)
{
	CheckForSilence(CWedgeHal::TERM_DATA_IN, 10);
	CheckForSilence(CWedgeHal::TERM_CLK_IN, delaytime);
}

///////////////////////////////////////////////////////////////////////////////
void CBitBanger::KbdClk_Output(ULONG delaytime)
{
	SetBit(CWedgeHal::KBD_CLK_DIR, delaytime);
}

void CBitBanger::KbdClk_Low(ULONG delaytime)
{
	ClrBit(CWedgeHal::KBD_CLK_OUT, delaytime);
}

void CBitBanger::KbdClk_High(ULONG delaytime)
{
	SetBit(CWedgeHal::KBD_CLK_OUT, delaytime);
}

void CBitBanger::KbdClk_HighOc(ULONG delaytime)
{
	KbdClk_High(0);
	KbdClk_HighZ(delaytime);
}

void CBitBanger::KbdClk_HighZ(ULONG delaytime)
{
	ClrBit(CWedgeHal::KBD_CLK_DIR, delaytime);
}

void CBitBanger::KbdClk_CheckHigh(ULONG delaytime)
{
	CheckBitHigh(CWedgeHal::KBD_CLK_IN, delaytime);
}

void CBitBanger::KbdClk_CheckLow(ULONG delaytime)
{
	CheckBitLow(CWedgeHal::KBD_CLK_IN, delaytime);
}

void CBitBanger::KbdClk_WaitHigh(ULONG delaytime)
{
	WaitForBitHigh(CWedgeHal::KBD_CLK_IN, delaytime);
}

void CBitBanger::KbdClk_WaitLow(ULONG delaytime)
{
	WaitForBitLow(CWedgeHal::KBD_CLK_IN, delaytime);
}

void CBitBanger::KbdDat_Output(ULONG delaytime)
{
	SetBit(CWedgeHal::KBD_DATA_DIR, delaytime);
}

void CBitBanger::KbdDat_Low(ULONG delaytime)
{
	ClrBit(CWedgeHal::KBD_DATA_OUT, delaytime);
}

void CBitBanger::KbdDat_High(ULONG delaytime)
{
	SetBit(CWedgeHal::KBD_DATA_OUT, delaytime);
}

void CBitBanger::KbdDat_HighOc(ULONG delaytime)
{
	KbdDat_High(0);
	KbdDat_HighZ(delaytime);
}

void CBitBanger::KbdDat_HighZ(ULONG delaytime)
{
	ClrBit(CWedgeHal::KBD_DATA_DIR, delaytime);
}

void CBitBanger::KbdDat_CheckHigh(ULONG delaytime)
{
	CheckBitHigh(CWedgeHal::KBD_DATA_IN, delaytime);
}

void CBitBanger::KbdDat_CheckLow(ULONG delaytime)
{
	CheckBitLow(CWedgeHal::KBD_DATA_IN, delaytime);
}

void CBitBanger::KbdDat_WaitHigh(ULONG delaytime)
{
	WaitForBitHigh(CWedgeHal::KBD_DATA_IN, delaytime);
}

void CBitBanger::KbdDat_WaitLow(ULONG delaytime)
{
	WaitForBitLow(CWedgeHal::KBD_DATA_IN, delaytime);
}

void CBitBanger::KbdDat_PrepareWaitForSilence(ULONG delaytime)
{
	PrepareWaitForSilence(CWedgeHal::KBD_DATA_IN, delaytime);
}

void CBitBanger::KbdClk_PrepareWaitForSilence(ULONG delaytime)
{
	PrepareWaitForSilence(CWedgeHal::KBD_CLK_IN, delaytime);
}

void CBitBanger::KbdDat_CheckForSilence(ULONG delaytime)
{
	CheckForSilence(CWedgeHal::KBD_DATA_IN, delaytime);
}

void CBitBanger::KbdClk_CheckForSilence(ULONG delaytime)
{
	CheckForSilence(CWedgeHal::KBD_CLK_IN, delaytime);
}

void CBitBanger::AddUserDefCommand(ULONG delaytime)
{
	AddCommand(USERDEF_COMMAND, CWedgeHal::TERM_DATA_OUT, delaytime);
}




