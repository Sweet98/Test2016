/*=================================================================================
  class: CWedgeDriver

//=================================================================================
   $Source: interfaces_wedge/drvwedgebase.h $
   $Date: 2011/11/14 16:48:53EST $
   $Revision: 1.7 $



//=================================================================================*/

#ifndef WDGE_DRIVER_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define WDGE_DRIVER_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

class CWedgeEngine;
class CWedgeTerminal;
class CBitBanger;

#include "ThreadedDriver.h"


#define baseCWedgeDriver CThreadedDriver
class CWedgeDriver : public baseCWedgeDriver
{
public:
	CWedgeDriver(void);
	CWedgeDriver(const CWedgeTerminal *pTerminal, CBitBanger *pBitBanger);
	virtual ~CWedgeDriver();

   virtual MatrixError_t Open(interface_id_t nId, const tichar_t *pParameter);
	virtual MatrixError_t Close(void);
	virtual IoControlRet_t DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam);
// more as we need it

protected:
	virtual void ThreadLoop(void);

protected:
	CWedgeEngine *m_pWedge;
	
private:
	CWedgeDriver (const CWedgeDriver&);            // declared private, but not defined, to prevent copying         
	CWedgeDriver& operator=(const CWedgeDriver&);  // declared private, but not defined, to prevent copying 

};

#endif /* WDGE_DRIVER_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */



