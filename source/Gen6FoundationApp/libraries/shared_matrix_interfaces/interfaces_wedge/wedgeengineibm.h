/*=================================================================================
  class: CWedgeEngineIBM

//=================================================================================
   $Source: interfaces_wedge/wedgeengineibm.h $
   $Date: 2010/04/13 17:25:08EDT $
   $Revision: 1.7 $



//=================================================================================*/

#ifndef WDGE_ENGINE_IBM_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define WDGE_ENGINE_IBM_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "WedgeEngine.h"

class CThreadRealTime;
class CBitBanger;

#define baseCWedgeEngineIBM CWedgeEngine
class CWedgeEngineIBM : public baseCWedgeEngineIBM
{
public:
	CWedgeEngineIBM(CThreadedDriver *pDriver, const CWedgeTerminal *pTerminal, UINT ScancodeSet, CBitBanger *pBitBanger);
	virtual ~CWedgeEngineIBM();
//	virtual MatrixError_t DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam);

protected:
	virtual MatrixError_t OpenKeyboardEmulation(void);
	virtual MatrixError_t CheckKeyboardEmulation(void);
//	virtual void OpenLabel(void);
//	virtual void FinalCloseLabel(void);

	virtual void PrepareIoForWedgeHardware(void);
	virtual void EnableKeyboard(void);
	virtual void DisableKeyboard(void);

	MatrixError_t BitBangOutKbd(unsigned int ucData, unsigned int exinfo);
	MatrixError_t BitBangInKbd(unsigned int &uiReturnData);

	UINT GetScancodeSet(void)			{	return m_ScancodeSet; }
	void SetScancodeSet(UINT val)	{	m_ScancodeSet = val;  }
	void SetScancodeTable(const CScancodes *pScancode)	{	m_pScancodes = pScancode;  }
	MatrixError_t GetNextByte(unsigned char *Data );

	bool WaitForClkDataHigh(UINT mSec);
	MatrixError_t Write(unsigned int scancode);
	void DoAskForResend(void);
	void DoSendAck(void);
	void DoEcho(void);
	void DoResend(void);
	void DoEnable(void);
	void DoReset(void);
	void DoTypematic(void);
	void DoScancodeSet(void);
	void DoId(void);
	void DoStatusIndicators(void);


	UINT m_ScancodeSet;

};

#endif /* WDGE_ENGINE_IBM_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */




