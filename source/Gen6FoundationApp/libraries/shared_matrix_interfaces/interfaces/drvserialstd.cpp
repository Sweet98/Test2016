/*=================================================================================
  class: CSerialDriver

//=================================================================================
   $Source: interfaces/drvserialstd.cpp $
   $Date: 2011/01/18 15:46:07EST $
   $Revision: 1.20.3.3.1.1.1.13 $


//=================================================================================*/
#include "stdInclude.h"
#include "CommonDriver.h"
#include "DrvSerialStd.h"
#include "interrupts.h"
#include "BarcodeData.h"
#include "rxfifo.h"
//ewr-02787
#include "MLFlatbedSerialParser.h"
//
//#include "beep.h"		//lmc  debug

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



// This is the class factory for this driver
CDriverBase *CreateSerialDriver(void)
{
	return (new CSerialDriver(CSerialBase::UART1));
}

CDriverBase *CreateSerialOW(void)
{
	return (new CSerialDriver(CSerialBase::UART2));
}

// Constructor
CSerialDriver::CSerialDriver(UartNumber_t UartNumber)
		: CSerialBase(UartNumber)
{
//ewr-02787
	CFlatBedParser::CreateFlatBedParser();
//
	m_xoff= 'S'-'@';
	m_xon = 'Q'-'@';
	m_CharacterTime = 1000;	               // default to 1 mSec

//ewr-02787
	m_uiPacketModeType = 0;      //flatbed packet mode type, default to 0 (off) right now, 2 is the only one currently supported
	m_bPacketModeNakEnabled = false;           //flatbed packet ACK/NAK mode enable
	m_bPacketModified = false;      //indicates the the barcode data has been modified for a flatbed packet mode
//
}

// Destructor
CSerialDriver::~CSerialDriver()
{
//ewr-02787
	CFlatBedParser::DestroyFlatBedParser();
//
}

MatrixError_t CSerialDriver::Open(interface_id_t nId, const tichar_t *pParameter)
{
	MatrixError_t ret = CSerialBase::Open(nId, pParameter);
	if(IsOK(ret))
	{
		ret = m_Port.SetupHardware();

//ewr-02787
		if(HardwareToken.IAmOEM())
			m_Port.SetRtsCtsIoPinsToGPIO();
		else
		{
			if( (IsRtsCts() == CTSMAN) || (IsRtsCts() == CTSFST) )
//
			{
				//theHal.PrepareIoForRs232WithManualCts();
				m_Port.SetCtsIoPinsToGpio();
			}
			else
			{
				m_Port.SetCtsIoPinsToRts();;
			}
		}
	}

	if(IsOK(ret))
	{
		m_CharacterTime = m_Port.GetCharacterTime();  // for better delays
		if (IsAnyCharacterDelays())	                // need to slow down?
		{
			// we work in polling mode if any InterXdelay is configured.
			// So a higher threshold is better.
			m_TxThreshold = m_Port.TX_FIFO_SIZE-2;
		}
		m_Port.SetTxTriggerLevel(m_TxThreshold);
		m_Port.SetRxTriggerLevel(24);
		SetIrqVectors();
	//	m_Port.FrameErrIrqEnable();
		m_Port.RxMinFifoIrqEnable();
		m_Port.RxTimeoutIrqEnable();
	//	m_Port.RxIdleIrqEnable();
		m_Port.EnableIrqAIC();
		RunThread();
	}
	return ret;
}

MatrixError_t CSerialDriver::Close()
{
	m_Port.Close();
	return CSerialBase::Close();
}

int CSerialDriver::StartSending(void)
{
	bool bSendData = TRUE;
	timer_mSec_t timeout = GetCtsDelay();

	m_MessageThrownOutNotSent = FALSE;

	ASSERT(AnyDataToSend());

//ewr-02787
	if( (IsRtsCts()==CTSMAN) || (IsRtsCts()==CTSFST) )      //lets wait menued delay for CTS and only send if we get it
	{                                                       //if delay expires, toss data
		m_Port.TxReadyIrqDisable();		//No need for Tx Ack/Nak handling
		m_Port.TxCompleteIrqDisable();

		if(IsRtsCts()==CTSFST)
		{
			bSendData = WaitForCts(timeout, 1);	//if ML flatbed, wait for CTS to be removed

		}
		else
		{
			bSendData = WaitForCts(timeout, 0);	//if NCR flatbed, just wait for CTS to be asserted
		}
//

		if(bSendData == FALSE)		//delay expired
		{
			m_MessageThrownOutNotSent = TRUE;
			GetBarcodeToSend();  //if we failed to get CTS, get barcode so we can throw it out
		}
	}

	if(bSendData)
	{
//ewr-02787
		if( (IsAnyCharacterDelays()) || (IsRtsCts()==CTSMAN) || (IsRtsCts()==CTSFST) )   // need to slow down?  -  must be used for flatbed handshaking
//
		{
			SendSlow();				               // send in polling mode
		}
		else
		{
			CSerialBase::StartSending();	// send at full speed
		}
	}
	return 0;
}


// send with delays
// If there are any delays configured (InterXDelay), then it makes not too much sense to send the data in interrupt mode.
// The delays would make the design very complicated and it would not buy us too much.
// The overhead using this polling mode is fairly small.
void CSerialDriver::SendSlow(void)
{
	ASSERT(AnyDataToSend());
	int signal = Sig_Resend;

//ewr-02787
	CBarcodeData * pBarcodeToSend;

	bool bSendData = TRUE;
//

	while(signal & Sig_Resend)
	{
//ewr-02787
		//GetBarcodeToSend()->GetTotalData(m_pCurrentText, m_RestSize);       //broke line up below   lmc
		pBarcodeToSend = GetBarcodeToSend();
		m_bPacketModified = false;

		if( (IsRtsCts() == CTSFST) )
		{
			m_uiPacketModeType = (unsigned int) GetPacketMode();
			m_bPacketModified = theFlatBedParser.FormatBarcodeData( pBarcodeToSend, m_uiPacketModeType );

			if( !m_bPacketModified && (m_uiPacketModeType > 0) )        //if packet not modified by parser and a packet mode is not set to off.
			{
				m_MessageThrownOutNotSent = TRUE;
				bSendData = FALSE;
			}
		}

		pBarcodeToSend->GetTotalData(m_pCurrentText, m_RestSize);

		if( bSendData )
		{
			PrepareWaitForSignal(Sig_BarcodeSent|Sig_Resend);	// prepare the wait for done
			SendWithPollingMode();            //send data
			if(IsAcknak())
			{
				signal = WaitForAnySignal(Sig_BarcodeSent|Sig_Resend);	// wait until the ISR sent them
			}
			else break;
		}
		else break;
	}   //end while
//

}

void CSerialDriver::EnsureDelay(ULONG delay)
{
	if(delay > 0)
	{
		while ( m_Port.IsTxFifoNotEmptyOrIsTxBufferNotSent() )	   // wait until all bytes left the transmitter
		{
//ewr-02787
			if( (IsRtsCts()==CTSMAN) || (IsRtsCts()==CTSFST) )
			{
				CTimeout IsTxComplete_Delay(m_CharacterTime);      // check on every byte, delay is in uS
				while (!IsTxComplete_Delay.HasExpired());
			}
			else
			{
				MT_Sleep(m_CharacterTime);   		// check on every byte
			}
//
		}

//ewr-02787
		if( (IsRtsCts()==CTSMAN) || (IsRtsCts()==CTSFST) )
		{
			CTimeout Sleep_Delay( (delay) );               // now sleep the specified time (in uS)
			while (!Sleep_Delay.HasExpired());
		}
		else
		{
			MT_Delay((delay+500)/1000);							// now sleep the specified time (pass uS, round up and convert to mS)
		}
//
	}
}

//ewr-02787
#define PACKET_RESEND_TRIES	1
//
// This is used if there are any delays configured
void CSerialDriver::SendWithPollingMode(void)
{
	UCHAR ucChar;

//ewr-02787
	size_t uiResendRestSize = 0;
	const bchar_t *pResendCurrentText = NULL;
	bool bSendData = TRUE;
	bool bWaitPacketAckNak = FALSE;
	timer_mSec_t timeout = GetCtsDelay();

	m_bPacketModeNakEnabled = (0 != IsPktNakEnabled());
	int iPacketResendTries = PACKET_RESEND_TRIES;
   timer_uSec_t timeoutNak = GetPacketAckNakDelay();

	if( m_bPacketModeNakEnabled && (IsRtsCts()==CTSFST) && m_bPacketModified )
	{                                         //lets save packet start location, if we need to resend in packet mode for a ACK/NAK failure
		uiResendRestSize = m_RestSize;
		pResendCurrentText = m_pCurrentText;
	}
//

	while (m_RestSize-- > 0)
	{
		while( m_Port.IsTxFifoNotEmptyOrIsTxBufferNotSent() )			// any room in the fifo?
		{
//ewr-02787
			if( (IsRtsCts()==CTSMAN) || (IsRtsCts()==CTSFST) )
			{
				CTimeout IsTxComplete_Delay(m_CharacterTime);     // check after every byte, in flatbed mode
				while (!IsTxComplete_Delay.HasExpired());
			}
			else
			{
				MT_Sleep(m_CharacterTime*m_TxThreshold/2);		// check after half of the fifo contents
			}
//
		}

		while(m_OutputStopped)					// is output on hold by the other side (XonXoffISR) ?
		{
//ewr-02787
			if( (IsRtsCts()==CTSMAN) || (IsRtsCts()==CTSFST) )
			{
				CTimeout OutputStopped_Delay(2000);                // check every 2000 uSec if in flatbed mode
				while (!OutputStopped_Delay.HasExpired());
			}
			else
			{
				MT_Delay(2);			         // check every 2 mSec
			}
//
		}


		if( (m_MessageThrownOutNotSent==FALSE) && ( (IsRtsCts()==CTSMAN) || (IsRtsCts()==CTSFST) ) )   //lets wait menued delay for CTS and only send if we get it
		{
			if(IsRtsCts()==CTSFST)
			{
				bSendData = WaitForCts(timeout, 2);      //if ML flatbed, disable RTS, wait for CTS to be disabled, then set RTS,
			}                                           //and finally wait for CTS to be asserted
			else
			{
				bSendData = WaitForCts(timeout, 0);			//if NCR flatbed, just wait for CTS
			}

			if(bSendData == FALSE)
			{
				m_MessageThrownOutNotSent = TRUE;
				m_RestSize = 0;
			}
		}

		if( m_bPacketModeNakEnabled && (IsRtsCts()==CTSFST) && m_bPacketModified )
		{                           //lets look for the termination char in the current packet and set flag to wait for ACK/NAK if we found it
			UCHAR ucPacketTermChar;

			ucPacketTermChar = theFlatBedParser.GetTermPacketChar();

			if( ucPacketTermChar == *m_pCurrentText )
			{
				bWaitPacketAckNak = TRUE;
			}
		}

		ucChar = *m_pCurrentText++;		//get the char to send

		if(bSendData)	//if hand shaking ok, then send data and do delays
		{
			 if( (IsRtsCts()==CTSFST) )
			{
				EnsureDelay(50);       //wait for all char to be sent(should already be), then do a small delay (in uS) after CTS set, before data sent.
			}

			m_Port.WriteFifo(ucChar);              //Lets send the Byte

			if( (IsRtsCts()==CTSFST) )
			{
				WaitForTxToComplete(50);      //Let wait for the interface to detect the emptying of the TX buffer, sampling at time passed, given in uS, before dropping RTS
				WaitForCts(0, 3);     //drop RTS immediatly here
				EnsureDelay( 300 + (3 * m_CharacterTime) );       //wait for all char to be sent(should already be), then do min intercharacter delay time (in uS) while RTS is down (1100 to 1200 uSec min)
				//Click();                            //lmc-debug
			}
		}

		if( m_bPacketModeNakEnabled && (IsRtsCts()==CTSFST) && bWaitPacketAckNak )
		{                  //if end of packet was found above, then wait for ACK/NAK before continuing with next packet (continue sending) or current packet resend
			int signal;
			PrepareWaitForSignal(Sig_BarcodeSent|Sig_Resend);	// prepare the wait for done
         	signal = WaitForAnySignal(Sig_BarcodeSent|Sig_Resend, timeoutNak);  // wait until the ISR sent them

		  	bWaitPacketAckNak = FALSE;

			if(signal & Sig_Resend)           //Got a NAK on this packet
			{
				if( iPacketResendTries == 0 )           //just throw out for now, if it is defined to 0, then no resends.
				{
					m_MessageThrownOutNotSent = TRUE;
					m_RestSize = 0;
				}
				else  //lets reset the current packet start location, if we need to resend it in packet mode for the next ACK/NAK check
				{
					iPacketResendTries--;
					m_RestSize = uiResendRestSize;
					m_pCurrentText = pResendCurrentText;
				}
			}

         	else if(signal & Sig_BarcodeSent)   //we got an ACK, move on to next packet if we have one
			{
				if( m_RestSize > 0)	//if there is another packet, then lets save the next packet start location,
				{                     // if we need to resend it in packet mode for the next ACK/NAK check
					uiResendRestSize = m_RestSize;
					pResendCurrentText = m_pCurrentText;
					iPacketResendTries = PACKET_RESEND_TRIES;
				}
			}
         	else  //Timed out without receiving ACK or NAK, clear out.
         	{
            	iPacketResendTries = 0;
            	m_MessageThrownOutNotSent = TRUE;
            	m_RestSize = 0;
         	}
		}

		EnsureDelay(GetInterCharDelay(ucChar));	//then do menued inter character delay if one is set
	}		//end  while(m_RestSize-- > 0)
//

	m_RestSize++;									// keep it at 0

//ewr-02787
	if( (IsRtsCts()!=CTSFST) )
	{
		EnsureDelay(100);                      // ensure we did sent the very last character (100 is arbitrary)
	}
//
}



void CSerialDriver::ReceiveXonXoffISR(void)
{
	if(m_Port.IsRxIdleIrq())
	{
		m_Port.ClearRxIdleIrq();
	}

	m_Port.ClearRxTimeout();

	unsigned long received = m_Port.ReadFifo();

	while (received & BIT(UART_URXD_CHARRDY_LSH))
	{
		if(!(received & BIT(UART_URXD_ERR_LSH)))
		{
			if ((received & 0xff)== m_xon)
			{
				m_OutputStopped = false;
				m_Port.TxReadyIrqEnable();
			}
			else if ((received & 0xff) == m_xoff)
			{
				m_OutputStopped = true;
				m_Port.TxReadyIrqDisable();
			}
			else
			{
				m_pRxFifo->Write(received & 0xff);
			}
		}
		else
		{
			// what to do??
		}
		received = m_Port.ReadFifo();	// read next fifo entry
	}
}


void CSerialDriver::ReceiveAckNackISR(void)
{
	if(m_Port.IsRxIdleIrq())
	{
		m_Port.ClearRxIdleIrq();
	}

	m_Port.ClearRxTimeout();

	unsigned long received = m_Port.ReadFifo();

	while (received & BIT(UART_URXD_CHARRDY_LSH))
	{
		if(!(received & BIT(UART_URXD_ERR_LSH)))
		{
			if ((received & 0xff)== ACK)
			{
				Signal(Sig_BarcodeSent);	// wake the driver thread
			}
			else if ((received & 0xff) == NAK)
			{
				Signal(Sig_Resend);			// wake the driver thread

			}
			else
			{
				m_pRxFifo->Write(received & 0xff);
			}
		}
		else
		{
			// what to do??
		}
		received = m_Port.ReadFifo();	// read next fifo entry
	}
}

void CSerialDriver::SendAckNakISR(void)
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
			m_Port.TxReadyIrqDisable();
			m_RestSize++;							// keep it at 0
			break;									// done, wait for ACK or NAK in the receive ISR
		}
	}
}



void CSerialDriver::SetIrqVectors(void)
{
	if (IsXonXoff())
	{
		pRxIsr = &CSerialDriver::ReceiveXonXoffISR;
		pTxIsr = &CSerialDriver::SimpleSendISR;
	}

//ewr-02787
	else if( IsAcknak() || IsPktNakEnabled() )
//
	{
		pRxIsr = &CSerialDriver::ReceiveAckNackISR;
		pTxIsr = &CSerialDriver::SendAckNakISR;
	}
	else
	{
		pRxIsr = &CSerialDriver::SimpleReceiveISR;
		pTxIsr = &CSerialDriver::SimpleSendISR;
	}
}


//ewr-02787
//*******************************************************************
//WaitForCts( )
//Manual mode for checking handshaking for different flatbed interfaces.
//
//iHandleRts=0 is for the NCR flatbed interface. RTS is set prior to startsending(),
//and here we wait for CTS before sending each character.
//iHandleRts=1 is used by the ML flatbed interface, and is only used before
//we start to send to check for CTS being passive, then leave and return pass.
//iHandleRts=2 is used by the ML flatbed interface, and is only used before
//we start to send, removes RTS, then checks for CTS being passive, then sets RTS, and waits for CTS.
//iHandleRts=3 is used by the ML flatbed interface, and is only used after
//we send to check for CTS being passive if timeout>0, then remove RTS and leave.
//If timeout not sent (==0), then we just drop RTS and leave.
//
//*******************************************************************
bool CSerialDriver::WaitForCts(timer_mSec_t timeout, int iHandleRts)
{
	bool bDone = FALSE;
	bool bCtsClear = FALSE;
	bool bRtsSet;

	if( (IsRtsCts()==CTSFST) && (iHandleRts==3) )		//RTS has already been set, so here we will lower it post data send,
	{                                                  //if timeout set, Check for CTS dropped then remove, else just remove RTS
		if(timeout == 0)
		{
			bCtsClear = TRUE;
		}

		bRtsSet = FALSE;
	}
	else if( (IsRtsCts()==CTSFST) && (iHandleRts==2) )		//RTS is set before calling StartSending in all modes except this one,
	{                                                  //so set RTS off, even though it should have already been done by a call to (==3) above,
		bRtsSet = FALSE;                                //then indicate that RTS is not asserted yet, do your pre send stuff below
		m_Port.ForceRtsOutPassive();
	}
	else if( (IsRtsCts()==CTSFST) && (iHandleRts==1) )		//RTS is set before calling StartSending in all modes except this one,
	{                                                     //so indicate that RTS is not assert yet, then check that CTS has been lowered below.
		bRtsSet = FALSE;
	}
	else
	{
		bRtsSet = TRUE;
	}

	if(timeout > 0)
	{
		CTimeoutmS timer(timeout);

		while(TRUE)
		{
			RESET_WATCHDOG_TIMER();   /*This is tough call here.  We might have an interface that is holding us off or really slow output and we wait
								   	* forever or for a long time, or what if the thread gets corrupted?..  I believe this is what 3800 does and it
								   	* is what it is for now */


		  	if( !bRtsSet && !bCtsClear && (IsRtsCts()==CTSFST) ) 	//In this mode only, if CTS is set , wait to it clears,
			{                                                     //before asserting RTS in next step below.
				//if( /*pSSR_C->b.Bit10 == TRUE*/ ((pUart1->Usr1)&BIT(UART_USR1_RTSS_LSH)) == FALSE /*IsCtsInActive() == TRUE*/ )
				if( m_Port.ReadCtsIoPinFromGpio() )
				{
					//if( /*pSSR_C->b.Bit10 == TRUE*/ ((pUart1->Usr1)&BIT(UART_USR1_RTSS_LSH)) == FALSE /*IsCtsInActive() == TRUE*/ )
					if( m_Port.ReadCtsIoPinFromGpio() )
					{
						bCtsClear = TRUE;
					}
				}
			}
			else if( (IsRtsCts()==CTSFST) && bCtsClear && !bRtsSet && (iHandleRts==1) )		//after confirming CTS is not there, then leave, for iHandleRts==1 only
			{
				bDone = TRUE;
				break;
			}
			else if( (IsRtsCts()==CTSFST) && bCtsClear && !bRtsSet && (iHandleRts==3) )		//after confirming CTS is not there, drop RTS, then leave, for iHandleRts==1 only
			{
				m_Port.ForceRtsOutPassive();
				bDone = TRUE;
				break;
			}                                                            //if iHandleRts==2, and rest of conditions same as two above,
			else if( (IsRtsCts()==CTSFST) && bCtsClear && !bRtsSet )		//after confirming CTS is not there, then assert RTS here, then continue to next CTS test below
			{
				m_Port.ForceRtsOutActive();
				bRtsSet = TRUE;
				timer.SetExpiration(timeout);	//we have waited for CTS to go away, now reset timer to wait for CTS to be set
			}
			else if( bRtsSet )		//if RTS has been set, then look for CTS before sending
			{
			//if(pSSR_B->b.Bit29==TRUE)          //If CTS is set , allow data to transmit
				//if( /*pSSR_C->b.Bit10 == FALSE*/ ((pUart1->Usr1)&BIT(UART_USR1_RTSS_LSH)) /*IsCtsInActive() == FALSE*/ )
				if( !m_Port.ReadCtsIoPinFromGpio() )
				{
					//if( /*pSSR_C->b.Bit10 == FALSE*/ ((pUart1->Usr1)&BIT(UART_USR1_RTSS_LSH)) /*IsCtsInActive() == FALSE*/ )
					if( !m_Port.ReadCtsIoPinFromGpio() )
					{
						bDone = TRUE;
						break;
					}
				}
			}

			if( timer.HasExpired() )          // If timer expired, throw out data
			{
				bDone = FALSE;
				break;
			}
		};		//end while(TRUE)
	}
	else		//if timeout is zero
	{
		if( (IsRtsCts()==CTSFST) && bCtsClear && !bRtsSet && (iHandleRts==3) )		//if timeout==0, drop RTS, then leave, for iHandleRts==3 only
		{
			m_Port.ForceRtsOutPassive();
		}

		bDone = TRUE;
	}

	return bDone;
}


//
//WaitForTxToComplete()
//
//waits for TX fifo to empty.
//checks status at period passed (uS)
//
void CSerialDriver::WaitForTxToComplete(ULONG delay)
{
	if(delay > 0)
	{
		while( m_Port.IsTxFifoNotEmptyOrIsTxBufferNotSent() )	   // wait until all bytes left the transmitter
		{
			if( (IsRtsCts()==CTSMAN) || (IsRtsCts()==CTSFST) )
			{
				CTimeout IsTxComplete_Delay(delay);      // check on every time interval, delay is in uS
				while (!IsTxComplete_Delay.HasExpired())
				{
					if( (IsRtsCts()==CTSFST) ) 			//In this mode only, if CTS gets cleared,
					{                                      //jump out of hear.
						//if( /*pSSR_C->b.Bit10 == TRUE*/ ((pUart1->Usr1)&BIT(UART_USR1_RTSS_LSH)) == FALSE /*IsCtsInActive() == TRUE*/ )
						if( m_Port.ReadCtsIoPinFromGpio() )
						{
							//if( /*pSSR_C->b.Bit10 == TRUE*/ ((pUart1->Usr1)&BIT(UART_USR1_RTSS_LSH)) == FALSE /*IsCtsInActive() == TRUE*/ )
							if( m_Port.ReadCtsIoPinFromGpio() )
							{
								break;
							}
						}
					}
				}	//end while !IsTxComplete_Delay.HasExpired()
			}
			else
			{
				MT_Sleep(delay);   		// check on every time interval, delay is in uS
			}
		}	//end while !m_Port.IsTxComplete()
	}	//end if(delay > 0)
}
//


void CSerialDriver::handleISRs(void)
{
	m_Port.SetAwakeOff();
	(this->*pRxIsr)();		// read any recieve data

	//if( (IsRtsCts()==CTSMAN) || (IsRtsCts()==CTSFST) || IsPktNakEnabled() )
	{
		if( m_Port.IsTxReadyIrq() && m_Port.IsTxReadyIrqEnableSet() )
		{
			(this->*pTxIsr)(); //handle sending
		}
		else if( m_Port.IsTxComplete() && m_Port.IsTxCompleteIrqEnableSet() )
		{
			(this->*pTxIsr)(); //handle sending
		}
	}
/*	else
	{
		if( m_Port.IsTxReadyIrq() )
		{
			(this->*pTxIsr)(); //handle sending
		}
	}*/
}





