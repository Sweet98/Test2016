/*=================================================================================
  class: CSerial4683

===================================================================================

==================================================================================*/

#ifndef FULL_SERIAL_4683_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define FULL_SERIAL_4683_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#include "Uart.h"
#include "ThreadedDriver.h"
#include "interrupts.h"
#include "Parity.h"
#include "Timeout.h"
#include "iocontrol.h"
#include "RetailAppFlash.h"
#include "CommandHandler4683.h"

class CRxFifo;
class CBarcodeData;

//Frame Type
#define	SEQUENCED_FRAME		0x04
#define	NON_SEQUENCED_FRAME	0x08
#define SIZEOF_RETAIL_BUF	500
#define	FRAME_SIZE			264

#define baseCSerial4683 CThreadedDriver
class CSerial4683: public baseCSerial4683
{
public:
	CSerial4683(Uart_T *pUart);
	virtual ~CSerial4683();
	virtual MatrixError_t Open(interface_id_t nId, const tichar_t *pParameter);
	virtual MatrixError_t Close();
	virtual void ThreadLoop(void);
	virtual IoControlRet_t DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam);
	virtual size_t Write(CBarcodeData  *pLabel);
	virtual size_t Write(const bchar_t *pBin, size_t nSize);
	virtual size_t Write(const wchar_t *pText, size_t nSize);

protected:
	CUart m_Port;
	CParity m_CParity;
	CTimeout m_PollResponseDelay4683; // The time to wait for the master to turn off its 485 drive before responding.
	CTimeout m_PollResponseTimeout4683; // The time	within which we must respond to a poll before the master assumes we're not there.
	CommandHandler4683 Primary4683;
	CommandHandler4683 Aux4683;

	char CurrentRetailAddress; // The current retail address primary/aux
	char PrimaryRetailAddress; // Primary Port's address
	char AuxRetailAddress; // Auxilary Port's address

	volatile char m_RxState;
	volatile int m_RxFrameLength; /* the received frame length */
	volatile BOOL m_EOP_Sent; /* EOP was last character sent */
	int m_ProtocolFormat;

	int m_TxThreshold;
	int m_RxThreshold;

	vector_number_t m_RxVectorNumber;

	bool TxInProgress;

public:

	struct FrameChar_t 					/* The FRAME_CHAR structure provides 2 ways to      */
	{ 									/* read & write a buffered frame character:         */
		union CHAR 						/*      8        7      6   5   4   3   2   1   0   */
		{ 								/*+----------+-------------------------------------+*/
			unsigned char Raw :8; 		/*|bMP(Tx/Rx)|          R A W    C h a r           |*/
										/*+----------+-------+-----------------------------+*/
			struct FLAG_POLL 			/*|bMP(Tx/Rx)| bType |           C h               |*/
			{ 							/*+----------+-------+-----------------------------+*/
				unsigned char Ch :7;
				unsigned char bType :1; /* 1 = Poll char., 0 = Addr/Flag */
			} FlagPoll;

			struct
			{
				unsigned char NotUsed :1;
				unsigned char SendNum :3;
				unsigned char AlwaysZero :1;
				unsigned char RecNum :3;
			} SeqInfo;
		} Char;

		union
		{
			unsigned char Raw :8;

			struct BITS
			{
				unsigned char bMpTx :1; 		/* 1 = Frame char., 0 = Data char. (R/W) */
				unsigned char bMpRx :1; 		/* 1 = Frame char., 0 = Data char. (R-O) */
				unsigned char bTxEnd :1; 		/* (R-O) */
				unsigned char bParityErr :1; 	/* (R/[W only 0]) */
				unsigned char bFrameErr :1; 	/* (R/[W only 0]) */
				unsigned char bOverrunErr :1; 	/* (R/[W only 0]) */
				unsigned char bRxFull :1; 		/* (R/[W only 0]) */
				unsigned char bTxEmpty :1; 		/* (R/[W only 0]) */
			} Bits;
		} Byte1;
	};


	volatile int m_OutputCount; // Output counter
	volatile int m_TxFrameLength; // Length of message to transmit
	unsigned char RetailRxBuf[SIZEOF_RETAIL_BUF + 4]; /* receive buffer */
	bool receivedPoll;
	unsigned char *m_RetailTxBuffer; // TxBuffer points to port's output buffer!


private:
	typedef struct
	{
		unsigned char crchi;
		unsigned char crclo; /* high and low order cyclic redundancy check bytes.*/
	} crc_t;

public:
	CREATE_SETVECTOR_MEMBER(CSerial4683);
	threadhandle_t Transmit4683(void);
	threadhandle_t Receive4683(void);
	threadhandle_t SimpleRxTxISR(void);
	crc_t CalcCrc(unsigned char *cmd_str, int lenght);
	void SendFrame(void);

protected:
	/* function prototypes */
	void Rx485Enable(void)
	{
		m_Port.ForceRtsOutActive();
	}

	void Rx485Disable(void)
	{
		m_Port.ForceRtsOutPassive();
	}

	void Tx485Enable(void)
	{
		m_Port.ForceRtsOutPassive();
	}

	void Tx485Disable(void)
	{
		m_Port.ForceRtsOutActive();
	}

	void PrepareIoFor4683(void);
	void SetIrqVectors(void);
	void TransmitEOP4683(void);
	void Start4683Transmitter(void);
	bool CheckFrame(void);
	MatrixError_t IoControlForceClose();

};

#endif // FULL_SERIAL_4683_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_





