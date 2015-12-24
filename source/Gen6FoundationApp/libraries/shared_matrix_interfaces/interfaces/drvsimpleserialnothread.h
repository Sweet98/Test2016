/*=================================================================================
  class: CSerialSimpleNoThread
 
//=================================================================================
   $Source: interfaces/drvsimpleserialnothread.h $
   $Date: 2009/03/06 16:43:43EST $
   $Revision: 1.5.2.3 $
 

//=================================================================================*/

#ifndef SIMPLE_SERIAL_NO_THREAD_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define SIMPLE_SERIAL_NO_THREAD_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#include "Uart.h"
#include "Driverbase.h"
#include "interrupts.h"

class CBarcodeData;


#define baseCSerialSimpleNoThread CDriverBase
class CSerialSimpleNoThread : public baseCSerialSimpleNoThread
{
#if PLATFORM_CURRENT == imx25
public:
	CSerialSimpleNoThread(Uart_T * const pUart, vector_number_t RxVectorNumber);
protected:
	CIrqVector m_saved_rx_vector;
	vector_number_t m_RxVectorNumber;
	threadhandle_t SimpleRxTxISR(void);

#elif	PLATFORM_CURRENT == mxl
public:
	CSerialSimpleNoThread(Uart_T * const pUart, vector_number_t RxVectorNumber, vector_number_t TxVectorNumber, vector_number_t Pf_VectorNumber);

protected:
	CIrqVector m_saved_rx_vector;
	CIrqVector m_saved_tx_vector;
	CIrqVector m_saved_pf_vector;
	vector_number_t m_RxVectorNumber;
	vector_number_t m_TxVectorNumber;
	vector_number_t m_PfVectorNumber;
#else
	#error VALID PLATFORM NOT DEFINED
#endif
public:
	virtual ~CSerialSimpleNoThread();

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
	virtual MatrixError_t Close();
	virtual IoControlRet_t DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam);
	// more as we need it

protected:
	Uart_T *msspUart;
	ULONG GetBaudRate(void)
	{
		return m_Port.GetBaudRate();
	}
	RtsTypes IsRtsCts(void)
	{
		return m_Port.IsRtsCts();
	}
	BOOL IsXonXoff(void)
	{
		return m_Port.IsXonXoff();
	}
	BOOL IsAcknak(void)
	{
		return m_Port.IsAcknak();
	}
	int GetParity(void)
	{
		return m_Port.GetParity();
	}
	int GetStopBits(void)
	{
		return m_Port.GetStopBits();
	}
	int GetDataBits(void)
	{
		return m_Port.GetDataBits();
	}

	void StartSending(void);
	threadhandle_t SimpleSendISR(void);
	threadhandle_t SimpleReceiveISR(void);

public:
	bool IsTxRunning(void);
	void ClearTxRunning(void);
	void SetTxRunning(void);

protected:
	CUart m_Port;
	int m_TxThreshold;

	const bchar_t *m_pCurrentText;
	size_t m_RestSize;

	volatile bool m_TxInProgress;


	void SetIrqVectors(void);

public:
	CREATE_SETVECTOR_MEMBER(CSerialSimpleNoThread);
};

#endif // SIMPLE_SERIAL_NO_THREAD_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_





