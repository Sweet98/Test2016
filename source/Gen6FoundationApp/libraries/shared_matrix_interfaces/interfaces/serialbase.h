/*=================================================================================
  class: CSerialBase
 
//=================================================================================
   $Source: interfaces/serialbase.h $
   $Date: 2009/12/17 02:41:23EST $
   $Revision: 1.14.3.4 $
 

//=================================================================================*/

#ifndef SERIAL_BASE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define SERIAL_BASE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#include "Uart.h"
#include "ThreadedDriver.h"
#include "interrupts.h"
class CBarcodeData;

class CSerialBase : public CThreadedDriver
{
public:
	enum UartNumber_t
	{
	    UART1 = 0,
	    UART2 = 1
	};
	CSerialBase(UartNumber_t UartNumber);

	virtual ~CSerialBase();

	virtual MatrixError_t Open(interface_id_t nId, const tichar_t *pParameter);
	virtual MatrixError_t Close();
	virtual IoControlRet_t DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam);
	// more as we need it

protected:
	ULONG GetBaudRate(void)
	{
		return m_Port.GetBaudRate();
	}
	RtsTypes IsRtsCts(void)
	{
		return m_Port.IsRtsCts();
	}
	BOOL IsCtsInActive(void)
	{
		return m_Port.IsCtsInActive();
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
	ULONG GetCtsDelay(void)
	{
		return m_Port.GetCtsDelay();
	}
	virtual void 	handleISRs(void){};
	threadhandle_t SimpleRxTxISR(void);
	//ewr-02787
	PktModeTypes GetPacketMode(void)		{ return m_Port.GetPacketMode(); }
	BOOL IsPktNakEnabled(void)				{ return m_Port.IsPktNakEnabled(); }
   ULONG GetPacketAckNakDelay(void) { return m_Port.GetPacketAckNakDelay(); }
//

	virtual void PreSendAction(void);
	virtual int StartSending(void);
	virtual void PostSendAction(void);
	void SimpleSendISR(void);
	void SimpleReceiveISR(void);
	void CtsISR(void);
	void PfISR(void);

	void SetIrqVectors(void);
	int Set_OE_ForRxTxRtsCts(int portnum);
	MatrixError_t PrepareForLowPowerSleep(void);
	MatrixError_t ReturnFromLowPowerSleep(void);
public:

protected:
	CUart m_Port;
	int m_TxThreshold;

	const bchar_t *m_pCurrentText;
	size_t m_RestSize;
	volatile bool sendABResponse;
	volatile bool m_OutputStopped;
	int m_autobaudstate;
	volatile bool m_MessageThrownOutNotSent;


#if PLATFORM_CURRENT == imx25

	vector_number_t m_VectorNumber;
#elif PLATFORM_CURRENT == mxl

	vector_number_t m_RxVectorNumber;
	vector_number_t m_TxVectorNumber;
	vector_number_t m_PfVectorNumber;
	vector_number_t m_CtsVectorNumber;
	vector_number_t m_Status_VectorNumber;
#else
#error VALID PLATFORM NOT DEFINED
#endif
public:
	CREATE_SETVECTOR_MEMBER(CSerialBase);

};

#endif // SERIAL_BASE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_





