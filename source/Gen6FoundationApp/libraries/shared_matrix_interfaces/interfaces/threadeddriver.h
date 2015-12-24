/*=================================================================================
  class: CThreadedDriver
 
//=================================================================================
   $Source: interfaces/threadeddriver.h $
   $Date: 2009/12/17 14:38:12EST $
   $Revision: 1.12.1.5 $
 

//=================================================================================*/
//! \file

#ifndef THREADED_DRIVER_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define THREADED_DRIVER_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#include "thread.h"
#include "driverbase.h"



///////////////////////////////////////////////////////////////////////////////
/// Base class for all interface drivers that require a thread.
/*! Derive your driver class from this.
*/

class CThreadedDriver : public CDriverBase
{
public:
	CThreadedDriver(size_t BufferSizeTx = DEFAULT_BUFFER_SIZE_TX, size_t BufferSizeRx = DEFAULT_BUFFER_SIZE_RX);
	virtual ~CThreadedDriver();
	virtual size_t Write(CBarcodeData  *pLabel);
	virtual size_t Write(const bchar_t *pBin, size_t nSize)
	{
		return CDriverBase::Write(pBin,nSize);
	};
	virtual size_t Write(const wchar_t *pText, size_t nSize)
	{
		return CDriverBase::Write(pText,nSize);
	};

	virtual MatrixError_t Open(interface_id_t nId, const tichar_t *pParameter);
	virtual MatrixError_t Close(void);
	virtual IoControlRet_t DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam);
	void Signal(ULONG SignalMask);
	
protected:
	void RunThread(void);
	virtual void PreSendAction(void);
	virtual int StartSending(void);
	virtual void PostSendAction(void);
	virtual void InitInstance(void);
	virtual void ThreadLoop(void);    	   // old name is SendLabel, but this was a bad name
	virtual void WaitUntilCurrentTransmittionIsComplete(void);
	ULONG m_signal;
	void DoInterMessageDelay(void);
	void MT_Sleep(timer_uSec_t uSec)
	{
		MT_Delay(uSec/1000);
	}
	void PrepareWaitForSignal(ULONG SignalMask);
	ULONG WaitForAnySignal(ULONG );
	ULONG WaitForAnySignal(ULONG , ULONG uSec);
	
private:
	void DriverThread(void);

protected:
	threadhandle_t mThreadhandle;
	bool m_Running;										//!< Run only once
	MT_Mailbox_t mSignalmbox;
};


#endif /* THREADED_DRIVER_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */





