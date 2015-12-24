/*=================================================================================
  class: CWedgeEngíne

//=================================================================================
   $Source: interfaces_wedge/wedgethread.h $
   $Date: 2009/06/29 05:03:53EDT $
   $Revision: 1.5 $



//=================================================================================*/

#ifndef WEDGE_THREAD_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define WEDGE_THREAD_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "WedgeMultiKey.h"
#include "ThreadRealtime.h"
#include "thread.h"
#include "drvwedgebase.h"
class CThreadRealTime;
class CThreadedDriver;

#define baseCWedgeThread CWedgeMultiKey
class CWedgeThread : public baseCWedgeThread
{
public:
	CWedgeThread(CThreadedDriver *pDriver, const CWedgeTerminal *pTerminal);  
	virtual ~CWedgeThread();
   virtual MatrixError_t Open(interface_id_t nId, const tichar_t *pParameter);
	virtual MatrixError_t Close(void);
//	virtual IoControlRet_t DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam);

protected:
	void MT_Sleep(long uSec)								
	{ 
		MT_Delay(uSec/1000);
	}    

protected:
	//threadhandle_t mThreadhandle;								//!< Run only once
	//MT_Mailbox_t mSignalmbox;
	
};

#endif /* WEDGE_THREAD_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */




