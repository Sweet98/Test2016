/*=================================================================================
  class: CThreadedDriver
 
//=================================================================================
   $Source: interfaces/threadeddriver.cpp $
   $Date: 2011/01/18 14:52:13EST $
   $Revision: 1.16.1.9 $
 

//=================================================================================*/

#include "stdInclude.h"
#include "CommonDriver.h"
#include "ThreadedDriver.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef void (CThreadedDriver::*ThreadMemberFunctionPointer_t)(void);


/*
	CThreadedDriver is a base class for drivers requiring a second thread of execution.
	Derive you driver class from CThreadedDriver rather from CDriverBase to use it.
 
	Some of the driver (keyboard wedge, ..) need it's own way of threading,
	but they can use the same model and ideas.
 
*/
#define DRIVER_STACK_SIZE 5000

// Constructor
CThreadedDriver::CThreadedDriver(size_t BufferSizeTx/*=DEFAULT_BUFFER_SIZE_TX*/, size_t BufferSizeRx/*=DEFAULT_BUFFER_SIZE_RX*/) : CDriverBase(BufferSizeTx, BufferSizeRx)
{
	mThreadhandle=0;
	mSignalmbox=NULL;
	m_Running = false;
	m_signal = 0;
}

// Destructor
CThreadedDriver::~CThreadedDriver()
{}

void CThreadedDriver:: PrepareWaitForSignal(ULONG SignalMask)
{
	DISABLE_INTERRUPTS();
	m_signal &=~SignalMask;
	RESTORE_INTERRUPTS();
}
ULONG CThreadedDriver:: WaitForAnySignal(ULONG SignalMask )
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

ULONG CThreadedDriver:: WaitForAnySignal(ULONG , ULONG uSec)
{
	unsigned int mbsignal=0;
	MT_WaitMailbox(mSignalmbox,uSec/1000,&mbsignal);
	return mbsignal;
}


/*----------------------------------------------------------------------------------------
Signal

  Disclaimer:  this paragraph of information was acquired by reverse engineering the code.
  There was a change from Gen5 to Gen6 in the Signal implementation.  Gen5 used no 
  mailboxes, just the m_signal mask of events.  This part was simpler, but there were known
  races and it appears to have this awkward thread sharing (see SignalFast & WakeFast).  
  I believe this new mailbox approach was done to alleviate these two issues.  However, 
  there are a couple of side affects:
    1.  Under the right conditions (sending small amounts of data fast), we would
	queue up several events in the mailbox.  Once data is sent, the thread could
	think there is more to send (when there isn't).

	2.  More importantly, the mailbox has a buffer with a limited amount of space.
	If you fill that mailbox, subsequent MT_AddMailbox() calls would fail and you could
	miss an important signal (event).  Unfortunately, we can't do a simple fix and
	spin in a loop waiting for MT_AddMailbox() to pass.  One, once it fails (which means
	it's full, we could get an IRQ and that will fail to Signal().  Two, Signal()
	is used inside an IRQ and we can't spin and wait in an IRQ.

  To avoid these issues, Signal() no longer pushes the event mask onto the mailbox
  buffer if those events are already set.  This more closely follows the original intent
  of Signals and m_signal in the Gen5 code.

  Parameters:  
		SignalMask[in]:  the description

  Returns:  nothing
----------------------------------------------------------------------------------------*/
void CThreadedDriver::Signal(ULONG SignalMask)
{
	DISABLE_INTERRUPTS();
	if ( (m_signal & SignalMask) != SignalMask ) MT_AddMailbox(mSignalmbox,&SignalMask);
	RESTORE_INTERRUPTS();
}



MatrixError_t CThreadedDriver::Open(interface_id_t nId, const tichar_t *pParameter)
{
	if (!mSignalmbox)
	{
		mSignalmbox=MT_CreateMailbox(16,sizeof(unsigned long));
	}
	if (!mThreadhandle)
	{
		// this code non portably (based on compiler)
		// allows accsess to a member function i 'c' code
		union dual_t
		{
			ThreadMemberFunctionPointer_t mfp;
			void (*fp)(void *);
		};
		dual_t DualPointer;
		DualPointer.mfp  = &CThreadedDriver::DriverThread;
		mThreadhandle = MT_OpenDriverThread( IFACE_PRIORITY,DRIVER_STACK_SIZE,DualPointer.fp,this);
	}

	MatrixError_t ret = CDriverBase::Open(nId, pParameter);
	return ret;
}

MatrixError_t CThreadedDriver::Close(void)
{
	MatrixError_t ret =CDriverBase::Close();
	if (mThreadhandle)
	{
		MT_CloseDriverThread(mThreadhandle);
	}
	if (mSignalmbox)
	{
		MT_DeleteMailbox(mSignalmbox);
		mSignalmbox=NULL;	
	}
	mThreadhandle=0;
	return ret;

}

IoControlRet_t CThreadedDriver::DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam)
{
	// first call base class
	IoControlRet_t RetVal=CDriverBase::DeviceIoControl(cmd, lparam, wparam);
	switch(cmd)
	{
	case IOCTRL_PAUSE_THREAD:
		//	m_pThread->Disable();
		RetVal=IOCRET_PASS;
		break;
	case IOCTRL_UNPAUSE_THREAD:
		//m_pThread->Enable();
		RetVal=IOCRET_PASS;
		break;
	default:
		break;
	}
	return RetVal;
}

size_t CThreadedDriver::Write(CBarcodeData  *pLabel)
{
	size_t ret = CDriverBase::Write(pLabel);
	Signal(Sig_NewBarcodeToSend);	// wake the driver thread
	return ret;
}


void CThreadedDriver::RunThread(void)
{
	if(!m_Running && mThreadhandle)	                     // precaution against starting it twice
	{
		m_Running = true;
		WakeDriverThread(mThreadhandle);
	}
}

/*
 This function runs as a second thread of execution.
 It's the startup for the thread.
*/
void CThreadedDriver::DriverThread(void)
{
	PrepareWaitForSignal(Sig_NewBarcodeToSend);			// prepare for the next packet
	InitInstance();                        				// optional initializer
	ThreadLoop();	                        			// call virtual function
}

// an optional place for initializations
void CThreadedDriver::InitInstance(void)
{
	// a no-op
}

/*
 Default Run loop.
 This function runs as a second thread of execution.
 It's the drivers output loop.
*/
void CThreadedDriver::ThreadLoop(void)
{
	while (TRUE)					// This is the drivers output loop
	{
		RESET_WATCHDOG_TIMER();
		if (AnyDataToSend())		// Any characters to send?
		{
			PreSendAction();		// what needs to be done before sending
			StartSending();			// send them
			PostSendAction();		// what needs to be done after sending
			DoInterMessageDelay();
		}
		else
		{
			WaitForAnySignal(Sig_NewBarcodeToSend);				// wait until we have data to send
			PrepareWaitForSignal(Sig_NewBarcodeToSend);			// prepare for the next packet
		}
	}
}

// You must overload this function with a special version if you want to use the
// default ThreadLoop loop (see above)
int CThreadedDriver::StartSending(void)
{
	ASSERT(0);	// you must overload this function with a special version
	return-99;	//error
}

// This function gets called from the driver ThreadLoop() before the StartSending.
void CThreadedDriver::PreSendAction(void)
{
	// a noop in the default implementation.
}

// This function gets called from the driver ThreadLoop() after the StartSending.
void CThreadedDriver::PostSendAction(void)
{
	// a noop in the default implementation.
}

void CThreadedDriver::DoInterMessageDelay(void)
{
	int delay = (GetInterMessageDelay()+500)/1000;
	if(delay > 0)
	{
		WaitUntilCurrentTransmittionIsComplete();
		MT_Delay(delay);
	}
}

// This function gets called from the driver ThreadLoop() after the StartSending.
// It must ensure that all data has left the device before we begin the intermessage delay.
// Most drivers take care of this themselfs, so the default implementation is a noop.
void CThreadedDriver::WaitUntilCurrentTransmittionIsComplete(void)
{
	// a noop in the default implementation.
	// You must overwrite it if your StartSending() returns before everything has been sent.
}







