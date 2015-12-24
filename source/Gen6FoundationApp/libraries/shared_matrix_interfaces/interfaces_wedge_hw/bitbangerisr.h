/*=================================================================================
  class: CBitBangerISR

//=================================================================================
   $Source: interfaces_wedge_hw/bitbangerisr.h $
   $Date: 2011/11/14 13:33:49EST $
   $Revision: 2.6 $


//=================================================================================*/

#ifndef BIT_BANGER_ISR_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define BIT_BANGER_ISR_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "WedgeHal.h"
#include "thread.h"
#include <stddef.h>

class CBitBangCmd
{
public:
//	CBitBangCmd(void);
	void Store(ULONG cmd, CPortDetails *pPort, ULONG delay);
	void StoreParm(ULONG delay)
	{
		m_time_param = delay;
	}
	void Get(ULONG &cmd, CPortDetails *&pPort, ULONG &delay)
	{
		cmd = m_cmd;
		pPort = m_pPort;
		delay = m_time_param;
	}

protected:
	ULONG m_cmd;
	ULONG m_time_param;
	CPortDetails *m_pPort;
};

class CThreadRealTime;

class CBitBangerISR
{
public:
	CBitBangerISR(size_t size);
	virtual ~CBitBangerISR(void);
	//void Attach(MT_Mailbox_t &m_Signalmbox);
	void CreateMailBox(void);
	virtual void ExecuteUserDefCommand(CPortDetails *pPort);  // use with care, watch the speed!
	virtual void Close(void);

	enum Commands
	{
		ILLEGAL			= 0x00000000,
		END_COMMAND,
		DELAY,
		JUMP,
		SET_ABORT,							// store info that we did abort

		SET_PORT,							// set register to high
		CLR_PORT,							// set register to low

		PREPARE_PORT_CHECK,				// Prepare a wait for port
		CHECK_PORT_HIGH_DURATION,		// Check bit for a given time duration and abort if wrong
		CHECK_PORT_LOW_DURATION,		// Check bit for a given time duration and abort if wrong
		CHECK_PORT_HIGH,					// Check bit now and abort if wrong
		CHECK_PORT_LOW,					// Check bit now and abort if wrong

		PREPARE_PORT_WAIT_HIGH,			// Prepare a wait for port
		PREPARE_PORT_WAIT_LOW,			// Prepare a wait for port
		WAIT_PORT_HIGH,					// Check bit and wait if wrong
		WAIT_PORT_LOW,						// Check bit and wait if wrong

		PREPARE_PORT_WAIT_RISING, 		// Prepare a wait for a rising edge at port
		PREPARE_PORT_WAIT_FALLING,		// Prepare a wait for a falling edge at port

		PREPARE_WAIT_FOR_SILENCE,		// no external transfer must occure
		CHECK_FOR_SILENCE,				// no external transfer must occure

		PREPARE_PORT_READ,				// Prepare the port bit read (reset the redeive buffer)
		READ_PORT_HIGH_LEFT_SHIFT,		// Read the port bit a shift left  (used for non-inverted ports)
		READ_PORT_HIGH_RIGHT_SHIFT,	// Read the port bit a shift right (used for non-inverted ports)
		READ_PORT_LOW_LEFT_SHIFT,		// Read the port bit a shift left  (used for inverted ports)
		READ_PORT_LOW_RIGHT_SHIFT,		// Read the port bit a shift right (used for inverted ports)

		WHILE_PORT_HIGH,
		WHILE_PORT_LOW,
		TOGGLE_PORT,
		USERDEF_COMMAND,					// a place holder to allow for variations outside of the normal scheme
												// place it in the buffer, overload the ExecuteCommand() and
												// use it as you wish in your derived class
		FIRST_LOCAL_COMMAND				// Use it to start your own local command range
	};
	enum
	{
		NUMBER_OF_COMMANDS = 32
	};

	threadhandle_t DecodeNextCommand(void);
	CREATE_SETVECTOR_MEMBER(CBitBangerISR);
protected:
	void SwitchToFastIrqMode(void);
	inline void SwitchBackToSlowIrqMode(void);
	static inline void SetupTimerTicks(timer_ticks_t ticks);
	static inline void SetupTimerUSec(timer_uSec_t uSec);
	void DoAbort(void);
	inline void DoKeepCurrentCommand(void);
	static inline bool IsBitSet(CPortDetails *pPort);

	// read it from the command buffer
	void ReadNextCommand(ULONG &cmd, CPortDetails *&pPort)
	{
		if (m_pCurrentReadLocation<=m_pCmdBufferEnd)
			(m_pCurrentReadLocation++)->Get(cmd, pPort, m_param);	// decode the command
		else cmd= ILLEGAL;
	}

	void DoIllegalCmd(CPortDetails *pPort);
	void DoEndCommand(CPortDetails *pPort);
	void DoDelay(CPortDetails *pPort);
	inline void DoJump(CPortDetails *pPort);
	void DoSetAbort(CPortDetails *pPort);

	void DoSetPort(CPortDetails *pPort);
	void DoClrPort(CPortDetails *pPort);
	void DoTogglePort(CPortDetails *pPort);

	void DoPreparePortCheck(CPortDetails *pPort);
	void DoCheckPortHighDuration(CPortDetails *pPort);
	void DoCheckPortLowDuration(CPortDetails *pPort);
	void DoCheckPortHigh(CPortDetails *pPort);
	void DoCheckPortLow(CPortDetails *pPort);

	void DoPreparePortWaitHigh(CPortDetails *pPort);
	void DoPreparePortWaitLow(CPortDetails *pPort);
	void DoWaitUntilPortHigh(CPortDetails *pPort);
	void DoWaitUntilPortLow(CPortDetails *pPort);

	void DoPreparePortRisingEdge(CPortDetails *pPort);
	void DoPrepareFallingEdge(CPortDetails *pPort);

	void DoPrepareForSilence(CPortDetails *pPort);
	void DoCheckForSilence(CPortDetails *pPort);

	void DoPrepareRead(CPortDetails *pPort);
	void DoReadHighShiftLeft(CPortDetails *pPort);
	void DoReadHighShiftRight(CPortDetails *pPort);
	void DoReadLowShiftLeft(CPortDetails *pPort);
	void DoReadLowShiftRight(CPortDetails *pPort);

	void DoWhilePortHigh(CPortDetails *pPort);
	void DoWhilePortLow(CPortDetails *pPort);

	void DoUserDefCommand(CPortDetails *pPort);				// a wide open command place holder
	void Signal(ULONG SignalMask);
	void PrepareWaitForSignal(ULONG SignalMask);
	ULONG WaitForAnySignal(ULONG SignalMask );


	typedef void (CBitBangerISR::*MemberFunctionPointerCmd_t)(CPortDetails *pPort);
	static const MemberFunctionPointerCmd_t CmdTable[NUMBER_OF_COMMANDS];


	static void GetPort(ULONG port_id, CPortDetails *&pPort)
	{
		WedgeHal.GetPort(port_id, pPort);
	}

protected:

	size_t m_size;
	ULONG m_param;
	ULONG m_return_value;
	ULONG m_repeat_time_ticks;
	CBitBangCmd	*m_pCmdBuffer;
	CBitBangCmd	*m_pCmdBufferEnd;
	CBitBangCmd *m_pCurrentReadLocation;

	CBitBangCmd	m_AbortCmd;
	CBitBangCmd	m_GoodExitCmd;

	CIrqVector m_saved_timer_vector;
	//threadhandle_t mThreadhandle;								//!< Run only once
	MT_Mailbox_t mSignalmbox;
	ULONG m_signal;

private:
	CBitBangerISR (const CBitBangerISR&);            // declared private, but not defined, to prevent copying         
	CBitBangerISR& operator=(const CBitBangerISR&);  // declared private, but not defined, to prevent copying 


};


#endif /* BIT_BANGER_ISR_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */



