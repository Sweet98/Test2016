/*=================================================================================
  class: CSerialSimpleNoThread
 
//=================================================================================
   $Source: interfaces/drvsimpleserialnothread.cpp $
   $Date: 2009/03/01 23:47:06EST $
   $Revision: 1.10.1.3 $
 

//=================================================================================*/
#include "stdInclude.h"
#include "CommonDriver.h"
#include "DrvSimpleSerialNoThread.h"
#include "BarcodeData.h"
#include "rxfifo.h"
#include "interrupts.h"
#include "timeout.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// This is the class factory for this driver
#if PLATFORM_CURRENT == imx25

CDriverBase *CreateSerialSimpleNoThread(void)
{
	return (new CSerialSimpleNoThread(pUart1, VEC_UART1_INT));
}


CDriverBase *CreateSerialSimpleNoThread2(void)
{
	return (new CSerialSimpleNoThread(pUart2, VEC_UART2_INT));
}

// Constructor
CSerialSimpleNoThread::CSerialSimpleNoThread(Uart_T *pUart, vector_number_t RxVectorNumber): m_Port(pUart)
{
	m_RxVectorNumber = RxVectorNumber;
	msspUart=pUart;
	// precaution
	DisableInterruptNumber(m_RxVectorNumber);
	// save vectors
	::GetVector(m_saved_rx_vector, m_RxVectorNumber);

	m_TxThreshold = 2;
	m_RestSize = 0;
	m_TxInProgress = false;
}

// Destructor
CSerialSimpleNoThread::~CSerialSimpleNoThread()
{
	// restore old vectors
	::SetVector(m_saved_rx_vector, m_RxVectorNumber);
}

MatrixError_t CSerialSimpleNoThread::Open(interface_id_t nId, const tichar_t *pParameter)
{
	MatrixError_t ret = baseCSerialSimpleNoThread::Open(nId, pParameter);


	if(IsOK(ret))
	{
		ret = m_Port.ExtractParameters(pParameter);
	}

	if(IsOK(ret))
	{
		DisableInterruptNumber(m_RxVectorNumber);		// safety
		m_Port.SetIoPins();
		ret = m_Port.SetupHardware();
	}

	if(IsOK(ret))
	{
		m_Port.SetTxTriggerLevel(m_TxThreshold);
		m_Port.SetRxTriggerLevel(24);

		SetIrqVectors();

		m_Port.RxMinFifoIrqEnable();
		//m_Port.RxIdleIrqEnable();
		m_Port.RxTimeoutIrqEnable();
		//m_Port.FrameErrIrqEnable();

	}

	return ret;
}

MatrixError_t CSerialSimpleNoThread::Close()
{
	// precaution
	DisableInterruptNumber(m_RxVectorNumber);

	m_Port.Close();
	return baseCSerialSimpleNoThread::Close();
}
void CSerialSimpleNoThread::SetIrqVectors(void)
{
	DisableInterruptNumber(m_RxVectorNumber);		// safety
	SetVector(&CSerialSimpleNoThread::SimpleRxTxISR,m_RxVectorNumber);
	EnableInterruptNumber(m_RxVectorNumber);
}

threadhandle_t CSerialSimpleNoThread::SimpleRxTxISR(void)
{
	SimpleReceiveISR();		// read any recieve data
	if (m_Port.IsTxReadyIrq())
		SimpleSendISR(); //handle sending
	return false;		// no thread switch
}
#else
	#error VALID PLATFORM NOT DEFINED
#endif //endof PLATFORM Check



/// Use for non standard communication from you application to the driver
IoControlRet_t CSerialSimpleNoThread::DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam)
{
	IoControlRet_t ret=IOCRET_DEFAULT;
	ret = baseCSerialSimpleNoThread::DeviceIoControl(cmd, lparam, wparam);  // ask our base class to be sure
	switch(cmd)
	{
	case IOCTRL_IS_DONE:
		if(ret == IOCRET_YES)
		{
			if (!m_Port.IsTxComplete())	      // if uart fifo is not empty, ...
				ret = IOCRET_NO;
		}
		break;
	default:
		break;
	}
	return ret;
}

size_t CSerialSimpleNoThread::Write(CBarcodeData  *pLabel)
{
	size_t ret = baseCSerialSimpleNoThread::Write(pLabel);
	if(!IsTxRunning())
		StartSending();
	return ret;
}

bool CSerialSimpleNoThread::IsTxRunning(void)
{
	return m_TxInProgress;
}

void CSerialSimpleNoThread::SetTxRunning(void)
{
	DISABLE_INTERRUPTS();
	m_TxInProgress = true;
	RESTORE_INTERRUPTS();
}

void CSerialSimpleNoThread::ClearTxRunning(void)
{
	DISABLE_INTERRUPTS();
	m_TxInProgress = false;
	RESTORE_INTERRUPTS();
}

void CSerialSimpleNoThread::StartSending(void)
{
	if (AnyDataToSend())
	{
		GetBarcodeToSend()->GetTotalData(m_pCurrentText, m_RestSize);
		SetTxRunning();
		m_Port.TxReadyIrqEnable();
	}
	else
	{
		ClearTxRunning();
	}
}

// CAUTION: This ISR takes longer than 10uSec (at the end of TX)!
threadhandle_t CSerialSimpleNoThread::SimpleSendISR(void)
{
	size_t block = CUart::TX_FIFO_SIZE - m_TxThreshold;
	while(block-- > 0)
	{
		if(m_RestSize-- > 0)
		{
			m_Port.WriteFifo(*m_pCurrentText++);
		}
		else
		{
			m_RestSize++;							// keep it at 0
			m_Port.TxReadyIrqDisable();
			SentBarcode();
			StartSending();						// try next barcode
			break;									// done
		}
	};
	return false;				// do not activate the drivers thread
}

threadhandle_t CSerialSimpleNoThread::SimpleReceiveISR(void)
{
	m_Port.ClearRxIdleIrq();
	m_Port.ClearRxTimeout();

	unsigned long received = m_Port.ReadFifo();

	while (received & BIT(UART_URXD_CHARRDY_LSH))
	{
		if(!(received & BIT(UART_URXD_ERR_LSH)))
		{
			m_pRxFifo->Write(received & 0xff);
		}
		else
		{
			// what to do??
		}
		received = m_Port.ReadFifo();	// read next fifo entry
	}
	return false;				// do not activae the drivers thread
}





