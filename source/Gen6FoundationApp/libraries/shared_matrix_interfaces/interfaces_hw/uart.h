/*=================================================================================
  class: CUart

//=================================================================================
   $Source: interfaces_hw/uart.h $
   $Date: 2010/11/09 21:12:09EST $
   $Revision:


//=================================================================================*/

#ifndef SERIAL_DRIVER_BASE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define SERIAL_DRIVER_BASE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

/*
CAUTION:
	The MX1 datasheets use a different naming scheme (DCE) for the SR232 signals as we do (DTE).
	In simple words the RTS and CTS are exchanged compared to our scheme.
	So please do not fix. the "error"....

*/
#include "SerialParameter.h"
#include "iMX25_UART.h"
#include "iMX25_GPIO.h"
#include "iMX25_IOMUX.h"
#include "HardwareMacros.h"
#include "HardwareToken.h"
//#include "db_mx1.h"
class CBarcodeData;

class CUart
{
public:
	CUart(Uart_T *pUart);
	virtual ~CUart();

	//	virtual size_t Write(CBarcodeData  *pLabel);	//default writes to a fifo
	//	virtual size_t Read(tichar_t *pDestination, size_t size);  //defaults reads from a fifo
	//   virtual MatrixError_t Open(interface_id_t nId, const tichar_t *pParameter);
	virtual MatrixError_t Close();
	//	virtual MatrixError_t DeviceIoControl(LPARAM lparam, WPARAM wparam);
	// more as we need it


	MatrixError_t ExtractParameters(const tichar_t *pParameters );
	MatrixError_t SetupHardware(void);
	MatrixError_t SetupBaudrate(void);
	void EnableIrqAIC(void);
	void DisableIrqAIC(void);

	ULONG GetBaudRate(void)
	{
		return m_setting.GetBaudRate();
	}

	void SetBaudRate(ULONG n)
	{
		m_setting.SetBaudRate(n);
	}

	RtsTypes IsRtsCts(void)
	{
		return m_setting.IsRtsCts();
	}

	bool IsXonXoff(void)
	{
		return m_setting.IsXonXoff();
	}

	bool IsAcknak(void)
	{
		return m_setting.IsAcknak();
	}

	int GetParity(void)
	{
		return m_setting.GetParity();
	}

	int GetStopBits(void)
	{
		return m_setting.GetStopBits();
	}

	int GetDataBits(void)
	{
		return m_setting.GetDataBits();
	}

	timer_uSec_t GetCharacterTime(void)
	{
		return m_setting.GetCharacterTime();
	}

	ULONG GetCtsDelay(void)
	{
		return m_setting.GetCtsDelay();
	}

	// ewr-02787
	PktModeTypes GetPacketMode(void)		{ return m_setting.GetPacketMode(); }
	BOOL IsPktNakEnabled(void)				{ return m_setting.IsPktNakEnabled(); }
	ULONG GetPacketAckNakDelay(void) { return m_setting.GetPacketAckNakDelay(); }
//

	// interrupts
	// Indicates the TX fifo is below a threshold
	void TxReadyIrqEnable(void)
	{
		m_pUart->Ucr1 |=BIT(UART_UCR1_TRDYEN_LSH);
	}

	void TxReadyIrqDisable(void)
	{
		m_pUart->Ucr1 &=~BIT(UART_UCR1_TRDYEN_LSH);
	}

	// Indicates the TX ready interupt enable is set
	bool IsTxReadyIrqEnableSet(void)
	{
		return ( (m_pUart->Ucr1 & BIT(UART_UCR1_TRDYEN_LSH)) != 0 );
	}

	// Indicates the TX fifo is empty
	void TxEmptyIrqEnable(void)
	{
		m_pUart->Ucr1 |=BIT(UART_UCR1_TXMPTYEN_LSH);
	}

	void TxEmptyIrqDisable(void)
	{
		m_pUart->Ucr1 &=~BIT(UART_UCR1_TXMPTYEN_LSH);
	}

	// Indicates the last byte has been send
	void TxCompleteIrqEnable(void)
	{
		m_pUart->Ucr4 |=BIT(UART_UCR4_TCEN_LSH);
	}

	void TxCompleteIrqDisable(void)
	{
		m_pUart->Ucr4 &=~BIT(UART_UCR4_TCEN_LSH);
	}

	// Indicates the Tx complete Irq interupt has been enabled
	bool IsTxCompleteIrqEnableSet(void)
	{
		return ( (m_pUart->Ucr4 & BIT(UART_UCR4_TCEN_LSH)) != 0 );
	}

	// Indicates that the RxFIFO data level is above the threshold
	void RxMinFifoIrqEnable(void)
	{
		//		m_pUart->Ucr1.b.RRDYENAB = 1;
		m_pUart->Ucr1 |=BIT(UART_UCR1_RRDYEN_LSH);
	}

	void RxMinFifoIrqDisable(void)
	{
		//		m_pUart->Ucr1.b.RRDYENAB = 0;
		m_pUart->Ucr1 &=~BIT(UART_UCR1_RRDYEN_LSH);
	}

	// Indicates that an idle condition has existed for more than a programmed amount frame
	void RxIdleIrqEnable(void)
	{
		//		m_pUart->Ucr1.b.IDENAB = 1;
		m_pUart->Ucr1 |=BIT(UART_UCR1_IDEN_LSH);

	}

	void RxIdleIrqDisable(void)
	{
		//		m_pUart->Ucr1.b.IDENAB = 0;
		m_pUart->Ucr1 &=~BIT(UART_UCR1_IDEN_LSH);
	}

	// Indicates that data in the RxFIFO has been idle for a time of 8 character lengths
	void RxTimeoutIrqEnable(void)
	{
		m_pUart->Ucr2 |= BIT(UART_UCR2_ATEN_LSH);
	}

	void RxTimeoutIrqDisable(void)
	{
		m_pUart->Ucr2 &= ~BIT(UART_UCR2_ATEN_LSH);
	}

	// Indicates that at least 1 character is received and written to the RxFIFO.
	void RxByteByByteIrqEnable(void)
	{
		m_pUart->Ucr4 |= BIT(UART_UCR4_DREN_LSH);
	}

	void RxByteByByteIrqDisable(void)
	{
		m_pUart->Ucr4 &=~BIT(UART_UCR4_DREN_LSH);
	}

	// Indicates that the receiver state machine is in an IDLE state,
	// the next state is IDLE, and the receive pin is high.
	void RxStatusIrqEnable(void)
	{
		m_pUart->Ucr3 |=BIT(UART_UCR3_RXDSEN_LSH);
	}

	void RxStatusIrqDisable(void)
	{
		m_pUart->Ucr3 &=~BIT(UART_UCR3_RXDSEN_LSH);
	}

	void CtsIrqEnable(void)
	{
		m_pUart->Ucr1 |=BIT(UART_UCR1_RTSDEN_LSH);
	}	// (MX1 calls it RTS)

	void CtsIrqDisable(void)
	{
		m_pUart->Ucr1 &=~BIT(UART_UCR1_RTSDEN_LSH);
	}  // (MX1 calls it RTS)

	void ParityErrIrqEnable(void)
	{
		m_pUart->Ucr3 |=BIT(UART_UCR3_PARERREN_LSH);
	}

	void ParityErrIrqDisable(void)
	{
		m_pUart->Ucr3 &=~BIT(UART_UCR3_PARERREN_LSH);
	}

	void FrameErrIrqEnable(void)
	{
		m_pUart->Ucr3 |= BIT(UART_UCR3_FRAERREN_LSH);
	}

	void FrameErrIrqDisable(void)
	{
		//		m_pUart->Ucr3.f= m_pUart->Ucr3.f &(~BIT11);
		m_pUart->Ucr3 &= ~BIT(UART_UCR3_FRAERREN_LSH);
	}

	// other control
	void TxEnable(void)
	{
		m_pUart->Ucr2 |= BIT(UART_UCR2_TXEN_LSH);
	}

	void TxDisable(void)
	{
		m_pUart->Ucr2 &= ~BIT(UART_UCR2_TXEN_LSH);

		//	m_pUart->Ucr2.b.TXENAB = 0;
	}

	void RxEnable(void)
	{
		m_pUart->Ucr2 |= BIT(UART_UCR2_RXEN_LSH);

		//		m_pUart->Ucr2.b.RXENAB = 1;
	}

	void RxDisable(void)
	{
		m_pUart->Ucr2 &= ~BIT(UART_UCR2_RXEN_LSH);

		//		m_pUart->Ucr2.b.RXENAB = 0;
	}

	void UartEnable(void)
	{
		m_pUart->Ucr1 |=BIT(UART_UCR1_UARTEN_LSH);
	}

	void UartDisable(void)
	{
		m_pUart->Ucr1 &=~BIT(UART_UCR1_UARTEN_LSH);
	}

	void PartyEnable(void)
	{
		m_pUart->Ucr2 |=BIT(UART_UCR2_PREN_LSH);
	}

	void PartyDisable(void)
	{
		m_pUart->Ucr2&=~BIT(UART_UCR2_PREN_LSH);
	}

	void SetPartyEven(void)
	{
		m_pUart->Ucr2 &=~BIT(UART_UCR2_PROE_LSH);
	}

	void SetPartyOdd(void)
	{
		m_pUart->Ucr2 |=BIT(UART_UCR2_PROE_LSH);
	}

	void Set7BitWords(void)
	{
		m_pUart->Ucr2 &=~BIT(UART_UCR2_WS_LSH);
	}

	void Set8BitWords(void)
	{
		m_pUart->Ucr2 |=BIT(UART_UCR2_WS_LSH);
	}

	void Set1StopBits(void)
	{
		m_pUart->Ucr2 &=~BIT(UART_UCR2_STPB_LSH);
	}

	void Set2StopBits(void)
	{
		m_pUart->Ucr2 |=BIT(UART_UCR2_STPB_LSH);
	}

	void IgnoreCtsInput(void)
	{
		m_pUart->Ucr2 |=BIT(UART_UCR2_IRTS_LSH);
	}			// (MX1 calls it RTS)

	void UseCtsInput(void)
	{
		m_pUart->Ucr2 &=~BIT(UART_UCR2_IRTS_LSH);
	}			// (MX1 calls it RTS)

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	void SetCtsIrqEdgeFall(void)
	{
		ClearCtsIrqEdgeBoth();
		m_pUart->Ucr2 |=BIT(UART_UCR2_RTEC_LSH);
	}			// (MX1 calls it RTS)

	void SetCtsIrqEdgeRise(void)
	{
		ClearCtsIrqEdgeBoth();
		m_pUart->Ucr2 &=~BIT(UART_UCR2_RTEC_LSH);
	}			// (MX1 calls it RTS)

	void ClearCtsIrqEdgeBoth(void)
	{
		m_pUart->Ucr2 &= ~(2 << UART_UCR2_RTEC_LSH);
	}

	void SetCtsIrqEdgeBoth(void)
	{
		m_pUart->Ucr2 |= (2 << UART_UCR2_RTEC_LSH);
	}

	void RtsManualMode(void)		// line controlled manually
	{
		m_pUart->Ucr2 &=~BIT(UART_UCR2_CTSC_LSH);
	}

	void RtsAutolMode(void)	// line controlled by rx buffer level
	{
		m_pUart->Ucr2 |= BIT(UART_UCR2_CTSC_LSH);
	}			// (MX1 calls it RTS)

	// Sets the CTS Trigger Level
	void SetRtsAutoLevel(int level)
	{
		int temp = BIT(UART_UCR4_CTSTL_LSH)-1;
		temp &=m_pUart->Ucr4;
		temp |= (level<<UART_UCR4_CTSTL_LSH);
		m_pUart->Ucr4=temp;
	}

	void SetAwakeOn(void)
	{
		m_pUart->Ucr3 |= BIT( UART_UCR3_AWAKEN_LSH);
	}		// set uart awake on

	void SetAwakeOff(void)
	{
		m_pUart->Ucr3 &= ~BIT( UART_UCR3_AWAKEN_LSH);
	}		// set uart awake on

	void EnableRtsDelta(void)
	{
		m_pUart->Ucr1 |= BIT(UART_UCR1_RTSDEN_LSH);
	}

	void DisableRtsDelta(void)
	{
		m_pUart->Ucr1 &= ~BIT(UART_UCR1_RTSDEN_LSH);
	}

	void CTSIrqEnable(void)
	{
		m_pUart->Ucr2 |= BIT(UART_UCR2_RTSEN_LSH);
	}		// (MX1 calls it RTS)

	void CTSIrqDisable(void)
	{
		m_pUart->Ucr2 &= ~BIT(UART_UCR2_RTSEN_LSH);
	}		// (MX1 calls it RTS)

//ewr-02787
	void SetRtsOutActive(void)			{	if ( (IsRtsCts()!=FLOW) && (IsRtsCts()!=CTSFST) ) 	//  if flow control on hardware controls line
														ForceRtsOutActive();		}	// Sets the RTS output (MX1 calls it CTS)
	void SetRtsOutPassive(void)		{	if ( (IsRtsCts()==HNDSHK) || (IsRtsCts()==CTSMAN) || (IsRtsCts()==CTSFST) )
														ForceRtsOutPassive();}					//if in handshaking mode we control the pin to frame data

	void ForceRtsOutPassive(void)
	{
		if(HardwareToken.IAmOEM())
		{
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT24_LSH,GPIO_BIT24_WID,1);
		}
		else
		{
			m_pUart->Ucr2 &= ~BIT(UART_UCR2_CTS_LSH);
		}
	}					//if in handshaking mode we control the pin to frame data

	void ForceRtsOutActive(void)
	{
		if(HardwareToken.IAmOEM())
		{
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT24_LSH,GPIO_BIT24_WID,0);
		}
		else
		{
			m_pUart->Ucr2 |= BIT(UART_UCR2_CTS_LSH);
		}
		
	}					//if in handshaking mode we control the pin to frame data

	void SetTxTriggerLevel(int num)
	{
		unsigned int temp =m_pUart->Ufcr & (BIT(UART_UFCR_TXTL_LSH)-1);
		temp |= (num<< UART_UFCR_TXTL_LSH);
		m_pUart->Ufcr=temp;
	}	// less than num characters generate an IRQ

	void SetRxTriggerLevel(int num)
	{
		unsigned int temp = m_pUart->Ufcr & ~(0x1f);
		temp |= num;
		m_pUart->Ufcr=temp;
	}	// more than num characters generate an IRQ

	void SetRefDiv(int n)
	{
		unsigned int temp =m_pUart->Ufcr & ~(BIT(7)|BIT(8)|BIT(9));
		temp |= n<< 7;
		m_pUart->Ufcr=temp;
	}

	void LoadBaudIncRegint(int n)
	{
		m_pUart->Ubir = n;
	}

	void LoadBaudModRegint(int n)
	{
		m_pUart->Ubmr = n;
	}

	// other status
	bool	IsCtsInActive(void)
	{
		if(HardwareToken.IAmOEM())
		{
			return( !READ_FIELD(GPIO4_DR0,GPIO_BIT25_LSH,GPIO_BIT25_WID));	
		}
		else
		{
			return 	(m_pUart->Usr1 & BIT(UART_USR1_RTSS_LSH));
		}
	}

	bool	IsCtsChangeIrq(void)
	{
		return 	(m_pUart->Usr2 & BIT(UART_USR2_RTSF_LSH));
	}  // (MX1 calls it RTS)

	bool	IsParityErrIrq(void)
	{
		return 	(m_pUart->Usr1 & BIT(UART_USR1_PARITYERR_LSH));
	}

	bool	IsTxReadyIrq(void)
	{
		return 	(m_pUart->Usr1 & BIT(UART_USR1_TRDY_LSH));
	}

	bool	IsCtsChanged(void)
	{
		return 	(m_pUart->Usr1 & BIT(UART_USR1_RTSD_LSH));
	}

	bool	IsFrameErrIrq(void)
	{
		return 	(m_pUart->Usr1 & BIT(UART_USR1_FRAMERR_LSH));
	}

	bool	IsRxReadyIrq(void)	// more than threshold characters in fifo
	{
		return 	(m_pUart->Usr1 & BIT(UART_USR1_RRDY_LSH));
	}

	bool	IsTxComplete(void)	// the last character has been sent.  NOTE: Only indicates that tx buffer is empty, does not indicate state of TX fifo below, even though reference manual says it does.
	{
		return 	(m_pUart->Usr2 & BIT(UART_USR2_TXDC_LSH));
	}

	bool	IsTxFifoEmpty(void)	// the Tx fifo has been emptied
	{
		return 	(m_pUart->Usr2 & BIT(UART_USR2_TXFE_LSH));
	}

	// Checks for !IsTxFifoEmpty() OR !IsTxFifoEmpty(), to indicate a complete data send.  Note: Need to check both above, to make sure data has been sent
	bool	IsTxFifoNotEmptyOrIsTxBufferNotSent(void)	// Indicates all bytes left the transmitter
	{
		return ( !IsTxComplete() || !IsTxFifoEmpty() );
	}

	bool	IsRxData(void)		// at least one character in fifo
	{
		return 	(m_pUart->Usr2 & BIT(UART_USR2_RDR_LSH));
	}

	bool	IsRxIdleIrq(void)
	{
		return 	m_pUart->Usr1 & BIT(UART_USR1_RXDS_LSH);
	}


	//bool	IsRxTimeoutIrq(void)			{ return 	m_pUart->Usr1.b.TIMEOUT;}


#if 0
	bool	IsTestTxFifoEmpty(void)
	{
		return 	m_pUart->Uts.b.TXEMPTY;
	}
	bool	IsTestRxFifoEmpty(void)
	{
		return 	m_pUart->Uts.b.RXEMPTY;
	}
	bool	IsTestTxFifoFull(void)
	{
		return 	m_pUart->Uts.b.TXFULL;
	}
	bool	IsTestRxFifoFull(void)
	{
		return 	m_pUart->Uts.b.RXFULL;
	}
	bool	IsTestReset(void)
	{
		return 	m_pUart->Uts.b.SOFTRST;
	}
#endif

	void	AckCtsChanged(void)
	{
		m_pUart->Usr1=BIT(UART_USR1_RTSD_LSH);
	}

	void 	AckCtsChangeIrq(void)
	{
		m_pUart->Usr2= BIT(UART_USR2_RTSF_LSH);
	}	// RTSF = 1 (MX1 calls it RTS)

	void	ClearParityErr(void)
	{
		m_pUart->Usr1= BIT(UART_USR1_PARITYERR_LSH);
	}

	void	ClearFrameErr(void)
	{
		m_pUart->Usr1=BIT(UART_USR1_FRAMERR_LSH);
	}

	void	ClearORunErr(void)
	{
		m_pUart->Usr2=BIT(UART_USR2_ORE_LSH);
	}

	void	ClearRxTimeout(void)
	{
		m_pUart->Usr1=BIT(UART_USR1_AGTIM_LSH);
	}

	void	ClearRxIdleIrq(void)
	{
		m_pUart->Usr2=BIT(UART_USR2_IDLE_LSH);
	}

	void	ClearAwake(void)
	{
		m_pUart->Usr1=BIT(UART_USR1_AWAKE_LSH);
	}

	void	ClearRtsDelta(void)
	{
		m_pUart->Usr1=BIT(UART_USR1_RTSD_LSH);
	}

	void SetCTSMode(int mode);


	// data
	void WriteFifo(UCHAR n)
	{
		m_pUart->Utxd[0]= n;
	}

	unsigned long ReadFifo(void)
	{
		return m_pUart->Urxd[0];
	}

	void SetIoPins();	// set up gpioinuse /pad registers for use as uart
	void SetCtsIoPinsToGpio();	// set up gpio in use / pad registers for use in cts manual mode
	void SetRtsCtsIoPinsToGPIO();
	void SetCtsIoPinsToRts();		// set up gpio in use / pad registers for use in CTS uart controled mode
	unsigned long ReadCtsIoPinFromGpio();		//Read GPIO for manual Cts state

	enum
	{
	    TX_FIFO_SIZE = 32,
	    RX_FIFO_SIZE = 32
	};

protected:
	CSerParm m_setting;
	float m_RefFrequency;
	Uart_T *m_pUart;
	int mUartnum;
};

#endif // SERIAL_DRIVER_BASE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_




