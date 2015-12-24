/*=================================================================================
  class: CWedgeEngíne

//=================================================================================
   $Source: interfaces_wedge/wedgeengine.h $
   $Date: 2011/11/14 16:48:54EST $
   $Revision: 1.5 $



//=================================================================================*/

#ifndef WDGE_ENGINE_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define WDGE_ENGINE_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "WedgeLabel.h"

class CThreadRealTime;
class CBitBanger;

#define baseCWedgeEngine CWedgeLabel
class CWedgeEngine : public baseCWedgeEngine
{
public:
	CWedgeEngine(CThreadedDriver *pDriver, const CWedgeTerminal *pTerminal, CBitBanger *pBitBanger);
	virtual ~CWedgeEngine();
//	virtual MatrixError_t DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam);
	virtual MatrixError_t Close(void);

protected:
	virtual MatrixError_t BitBangOut(unsigned int ucData, unsigned int exinfo);
	virtual MatrixError_t BitBangIn(unsigned int &uiReturnData);
	virtual MatrixError_t OpenKeyboardEmulation(void);
	virtual MatrixError_t CheckKeyboardEmulation(void);

	CBitBanger *m_pBitBang;
	
private:
	CWedgeEngine (const CWedgeEngine&);            // declared private, but not defined, to prevent copying         
	CWedgeEngine& operator=(const CWedgeEngine&);  // declared private, but not defined, to prevent copying 

};

#endif /* WDGE_ENGINE_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */




