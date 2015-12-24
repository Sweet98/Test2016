/*=================================================================================
  class: CBitBanger

//=================================================================================
   $Source: interfaces_wedge_hw/bitbanger.h $
   $Date: 2009/06/29 05:04:58EDT $
   $Revision: 2.2 $


//=================================================================================*/

#ifndef BIT_BANGER_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define BIT_BANGER_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "BitBangerISR.h"

class CBitBanger : public CBitBangerISR
{
public:
	CBitBanger(size_t size);
	virtual ~CBitBanger(void);

// interface
	virtual MatrixError_t BitBangOutTerm(unsigned int Data, unsigned int exinfo)=0;
	virtual MatrixError_t BitBangInTerm(unsigned int &uiReturnData);
	virtual MatrixError_t BitBangOutKbd(unsigned int Data, unsigned int exinfo);
	virtual MatrixError_t BitBangInKbd(unsigned int &uiReturnData);
	virtual void NotifyOutTermDelay(ULONG uSecDelay);

protected:
	void KbdClk_Output(ULONG delaytime);					// make it an output
	void KbdClk_Low(ULONG delaytime);						// set output to low	(as seen at the 10 pin modular connector)
	void KbdClk_High(ULONG delaytime);					// set output to high
	void KbdClk_HighOc(ULONG delaytime);					// edge improved open collector (open drain)
	void KbdClk_HighZ(ULONG delaytime);					// High impedance
	void KbdClk_CheckHigh(ULONG delaytime);				// Check for high, abort if not
	void KbdClk_CheckLow(ULONG delaytime);				// Check for low, abort if not
	void KbdClk_WaitHigh(ULONG delaytime);		// Check for high, wait if not
	void KbdClk_WaitLow(ULONG delaytime);		// Check for low, wait if not

	void KbdDat_Output(ULONG delaytime);
	void KbdDat_Low(ULONG delaytime);
	void KbdDat_High(ULONG delaytime);
	void KbdDat_HighOc(ULONG delaytime);
	void KbdDat_HighZ(ULONG delaytime);
	void KbdDat_CheckHigh(ULONG delaytime);
	void KbdDat_CheckLow(ULONG delaytime);
	void KbdDat_WaitHigh(ULONG delaytime);
	void KbdDat_WaitLow(ULONG delaytime);
	void KbdDat_ReadShiftLeft(ULONG delaytime);
	void KbdDat_ReadShiftRight(ULONG delaytime);

	void TermClk_Output(ULONG delaytime);
	void TermClk_Low(ULONG delaytime);
	void TermClk_High(ULONG delaytime);
	void TermClk_HighOc(ULONG delaytime);
	void TermClk_HighZ(ULONG delaytime);
	void TermClk_CheckHigh(ULONG delaytime);
	void TermClk_CheckLow(ULONG delaytime);
	void TermClk_WaitHigh(ULONG delaytime);
	void TermClk_WaitLow(ULONG delaytime);

	void TermDat_Output(ULONG delaytime);
	void TermDat_Low(ULONG delaytime);
	void TermDat_High(ULONG delaytime);
	void TermDat_HighOc(ULONG delaytime);
	void TermDat_HighZ(ULONG delaytime);
	void TermDat_CheckHigh(ULONG delaytime);
	void TermDat_CheckLow(ULONG delaytime);
	void TermDat_WaitHigh(ULONG delaytime);
	void TermDat_WaitLow(ULONG delaytime);
	void TermDat_ReadShiftLeft(ULONG delaytime);
	void TermDat_ReadShiftRight(ULONG delaytime);

	void PrepareWaitForSilence(CWedgeHal::Ports Port, ULONG delaytime);
	void CheckForSilence(CWedgeHal::Ports Port, ULONG delaytime);
	void PrepareWaitForSilence(ULONG delaytime);
	void CheckForSilence(ULONG delaytime);

	void TermDat_PrepareWaitForSilence(ULONG delaytime);
	void TermClk_PrepareWaitForSilence(ULONG delaytime);
	void TermDat_CheckForSilence(ULONG delaytime);
	void TermClk_CheckForSilence(ULONG delaytime);
	void KbdDat_PrepareWaitForSilence(ULONG delaytime);
	void KbdClk_PrepareWaitForSilence(ULONG delaytime);
	void KbdDat_CheckForSilence(ULONG delaytime);
	void KbdClk_CheckForSilence(ULONG delaytime);

	void AddUserDefCommand(ULONG delaytime);

	void DisableKeyboard(ULONG delaytime);
	void EnableKeyboard(ULONG delaytime);
	void Done(void);

	void OnAbortResumeHere(void);
	void OnGoodExitResumeHere(void);

	void WhilePortLow(CWedgeHal::Ports Port);
	void WhilePortHigh(CWedgeHal::Ports Port);
	void EndWhile(void);
	void SetAbort(void);

	MatrixError_t RunBigBanger(void);
	static ULONG GetReceivedPortData(CWedgeHal::Ports Port);
	bool CheckParity(ULONG input);

protected:
	static ULONG TranslateIntoTimerTicks(ULONG uSec);
	void WriteIllegalCommand(void);
	void ResetCommands(void);
	void AddCommand(Commands Cmd, ULONG port_id, ULONG param2);
	void AddTimeCommand(Commands Cmd, ULONG port_id, ULONG delaytime);
	void SetBit(CWedgeHal::Ports Port, ULONG delaytime);
	void ClrBit(CWedgeHal::Ports Port, ULONG delaytime);
	void CheckBitHigh(CWedgeHal::Ports Port, ULONG delaytime);
	void CheckBitLow(CWedgeHal::Ports Port, ULONG delaytime);
	void CheckBitHighDuration(CWedgeHal::Ports Port, ULONG delaytime);
	void CheckBitLowDuration(CWedgeHal::Ports Port, ULONG delaytime);
	void WaitForBitHigh(CWedgeHal::Ports Port, ULONG delaytime);
	void WaitForBitLow(CWedgeHal::Ports Port, ULONG delaytime);
	void WaitForRisingEdge(CWedgeHal::Ports Port, ULONG delaytime);
	void WaitForFallingEdge(CWedgeHal::Ports Port, ULONG delaytime);

	inline void ReadPortShiftLeft(CWedgeHal::Ports Port, ULONG delaytime);
	inline void ReadPortShiftRight(CWedgeHal::Ports Port, ULONG delaytime);

	void KeepWhilePosition(void);		// helper for creating the while loop
	void ResolvePosition(void);		// helper for creating the while loop

	bool CheckOddParity(ULONG val, int numbits);
	bool CheckEvenParity(ULONG val, int numbits)
	{
		return !CheckOddParity(val, numbits);
	}
protected:

	enum
	{
		OFFSET_CMDS = 0
	};

	size_t m_CurrentPosition;
	size_t m_WhilePosition;	// a one element stack for the while loop. Later we might add a real stack.
	CBitBangCmd *m_pCurrentFillLocation;
	ULONG m_min_time;
};

#endif /* BIT_BANGER_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */




