/*=================================================================================
  class: CSerialDriver

//=================================================================================
   $Source: interfaces/drvserialstd.h $
   $Date: 2009/12/17 02:41:09EST $
   $Revision: 1.9.2.3 $


//=================================================================================*/

#ifndef FULL_SERIAL_DRIVER_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define FULL_SERIAL_DRIVER_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#include "SerialBase.h"
#include "ThreadedDriver.h"
#include "timeout.h"

class CSerialDriver : public CSerialBase
{
public:
	CSerialDriver(UartNumber_t UartNumber);
	virtual ~CSerialDriver();

	virtual MatrixError_t Open(interface_id_t nId, const tichar_t *pParameter);
	virtual MatrixError_t Close();

protected:
	virtual int StartSending(void);
	void SendSlow(void);
	void SendWithPollingMode(void);
	void EnsureDelay(ULONG delay);

//ewr-02787
	bool WaitForCts(timer_mSec_t timeout, int iHandleRts );
	void WaitForTxToComplete(ULONG delay);
//

	void SetIrqVectors(void);
	void ReceiveXonXoffISR(void);
	void ReceiveAckNackISR(void);
	void SendAckNakISR(void);
	void (CSerialDriver::*pRxIsr)(void);
	void (CSerialDriver::*pTxIsr)(void);
	virtual void handleISRs(void);


	UCHAR	m_xoff;
	UCHAR	m_xon;
	ULONG m_CharacterTime;		            // the time required to send on byte (for InterXDelays)

//ewr-02787
private:

unsigned int m_uiPacketModeType;      //flatbed packet mode type
bool m_bPacketModeNakEnabled;           //flatbed packet ACK/NAK mode enable
bool m_bPacketModified;              //indicates the the barcode data has been modified for a flatbed packet mode
//

public:
};

#endif // FULL_SERIAL_DRIVER_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_





