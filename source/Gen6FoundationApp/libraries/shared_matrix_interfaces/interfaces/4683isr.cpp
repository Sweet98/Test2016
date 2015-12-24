/*=============================================================================
 * Interrupt handler for the 4683 interface rx and tx.
 *=============================================================================
 */

#include "stdInclude.h"
#include "CommonDriver.h"
#include "Drvserial4683.h"
#include "def4683.h"

/*************************************************************************
 * this routine will store received data into a linear buffer
 * A full message will be passed to ProcessFrame to check for errors and
 * message type.  If all is true we will send the message to
 * ProcessRetailMessage.
 * WARNING ****** this routine does not perform buffer overflow checking
 * if we can't process a message fast enough data will be overwritten!
**************************************************************************/

/*
 *  copy from MXL for CSerial4683::Receive4683
 */
 typedef union Urxd_T
{
	struct
	{
		volatile unsigned long		 	RX_DATA		 		  	: 8;
		volatile unsigned long			Unused					: 2;
		volatile unsigned long		 	PRERR        	  		: 1;
		volatile unsigned long		 	BRK          		  	: 1;
		volatile unsigned long		 	FRMERR       		  	: 1;
		volatile unsigned long		 	OVRRUN       		  	: 1;
		volatile unsigned long		 	ERR          	   	: 1;
		volatile unsigned long		 	CHARRDY      	   	: 1;
		volatile unsigned long			unused31 	  	   	: 16;
	} b;
	volatile unsigned long f;
} URXD_T;

threadhandle_t CSerial4683::Receive4683(void)
{

    PrimaryRetailAddress = Primary4683.GetAddress();
    AuxRetailAddress = Aux4683.GetAddress();

	FrameChar_t	FrameWord;																		/* a single frame character (word) */
	URXD_T	received;
	
	m_Port.ClearParityErr();
	if(m_Port.IsRxIdleIrq())
		m_Port.ClearRxIdleIrq();

	if (m_Port.IsFrameErrIrq())
	{
		m_Port.ClearFrameErr();							 										/* Clear the error!! */
	}
	
	received.f = m_Port.ReadFifo();
	

	if (received.b.CHARRDY)
	{
		
		if (received.b.FRMERR || TxInProgress)
		{
			m_RxState=FIRST_CHAR;
			m_RxFrameLength=0;
			return 0;
		}

		FrameWord.Char.Raw = received.b.RX_DATA;
		if (m_RxFrameLength>=SIZEOF_RETAIL_BUF)
		{	
			m_RxState=FIRST_CHAR;
			m_RxFrameLength=0;
		}

		RetailRxBuf[m_RxFrameLength] = FrameWord.Char.Raw;        // Store Character in char string buffer 
		
       switch (m_RxState)
        {
            case FIRST_CHAR :
            {
				if((received.b.PRERR!= m_CParity.ParityTable[FrameWord.Char.Raw]))  // Is Address bit set?? 
                {
                    if(FrameWord.Char.Raw == PrimaryRetailAddress ||        // Is this our primary address
                       (FrameWord.Char.Raw == AuxRetailAddress && AuxRetailAddress != 0x00 ) ||            // Is it our auxilary address
                       FrameWord.Char.Raw == BROADCAST_ADDRESS)		        // or the broadcast address?
                    {
                        m_RxState = LOOKING_FOR_END_CHAR;
                        m_RxFrameLength = 1;                                //first character of this frame

                        if( FrameWord.Char.Raw == PrimaryRetailAddress )    // Set the current retail address
                        {
                            CurrentRetailAddress = PrimaryRetailAddress;    
                        }
                        else if( FrameWord.Char.Raw == AuxRetailAddress )
                        {
                            CurrentRetailAddress = AuxRetailAddress;
                        }
                        else if( FrameWord.Char.Raw == BROADCAST_ADDRESS )
                        {
                            CurrentRetailAddress = BROADCAST_ADDRESS;
                        }
                    }
                    else if(FrameWord.Char.Raw == (PrimaryRetailAddress | 0x80))    //Is this our primary poll address
                    {
                        m_RxState = LOOKING_FOR_POLL;
                        CurrentRetailAddress = PrimaryRetailAddress;
                    }
                    else if(FrameWord.Char.Raw == (AuxRetailAddress | 0x80) && AuxRetailAddress != 0x00)    // Is this our aux poll address
                    {
                       m_RxState = LOOKING_FOR_POLL;
                        CurrentRetailAddress = AuxRetailAddress;
                    }
                    else                        // Not for us 
                    {
                       m_RxFrameLength = 0;    // reset length 
                    }
                }
                break;
            }
            case LOOKING_FOR_END_CHAR :
            {
				
                if((received.b.PRERR != m_CParity.ParityTable[FrameWord.Char.Raw]))	/* Is Address bit set?? */
                {
                    if(FrameWord.Char.Raw == FRAME_END_FLAG)									/* look for end character */
                    {
                        m_RxFrameLength++;															/* and count this character */
                        receivedPoll = false;
                        m_RxState = WAITING_UNTIL_PROCESSED;
                        Signal(Sig_4683Event);	
                        m_PollResponseDelay4683.Reset();
                        m_PollResponseTimeout4683.Reset();
                    }
                    else 
                    {
                        if(FrameWord.Char.Raw == PrimaryRetailAddress || FrameWord.Char.Raw == BROADCAST_ADDRESS)
						{
                            RetailRxBuf[m_RxFrameLength = 0] = FrameWord.Char.Raw;    // Store Character in char string buffer
                            CurrentRetailAddress = PrimaryRetailAddress;
						}
                        else if( FrameWord.Char.Raw == AuxRetailAddress && AuxRetailAddress != 0x00 )
                        {
                            RetailRxBuf[m_RxFrameLength = 0] = FrameWord.Char.Raw;
                            CurrentRetailAddress = AuxRetailAddress;
                        }
                        else
                        {
                            m_RxState = FIRST_CHAR;     // set state for next frame
                            m_RxFrameLength = 0;        // reset length
						}
                    }
                }
                else
                    m_RxFrameLength++;                  // else not an address byte - count it
                break;
            }
            case LOOKING_FOR_POLL :
            {
                if((received.b.PRERR != m_CParity.ParityTable[FrameWord.Char.Raw]) &&
                    (FrameWord.Char.Raw == (CurrentRetailAddress |0x80)))				/* a second poll character */
                {
                	receivedPoll = true;
                    m_RxState = WAITING_UNTIL_PROCESSED;
                    Signal(Sig_4683Event);															/* wake the driver thread */
                    m_PollResponseDelay4683.Reset();
                    m_PollResponseTimeout4683.Reset();
                }
                else
                {
                    m_RxState = FIRST_CHAR;															/* set state for next frame */
                    m_RxFrameLength = 0;																/* reset length */
                }
                break;
            }
            case WAITING_UNTIL_PROCESSED :
                break;
            default:
                m_RxState = FIRST_CHAR;															/* set state for next frame */
                m_RxFrameLength = 0;	
        }
    }				

    		 	
    return(0);
}

/***************************************************************************
 * will transmit a char to the 4683
***************************************************************************/
threadhandle_t CSerial4683::Transmit4683(void)
{
	if(!m_PollResponseTimeout4683.HasExpired())
	{
		m_OutputCount++;													/* get next character */
		if(((m_TxFrameLength - m_OutputCount) == 0) || (m_EOP_Sent))
		{
			m_Port.TxCompleteIrqDisable();
			Tx485Disable();						/*  ...then disable the 75176 transmitter */
			m_Port.RxByteByByteIrqEnable();			/* allow receiving data from 4683 */
			Rx485Enable();						/* (enable reciever). */
			m_Port.SetPartyOdd();						/* Set parity to ODD always for receiving! */
			m_EOP_Sent = 0;
			m_OutputCount=0;
			TxInProgress=false;
			return(0);
		}
		if((m_TxFrameLength - m_OutputCount) == 1)
		{
			if(m_CParity.ParityTable[m_RetailTxBuffer[m_OutputCount]])
				m_Port.SetPartyOdd();
			else
				m_Port.SetPartyEven();

			m_Port.TxEmptyIrqDisable();
			m_Port.TxCompleteIrqEnable();
		}
		else
		{
			if(m_CParity.ParityTable[m_RetailTxBuffer[m_OutputCount]])
				m_Port.SetPartyEven();
			else
				m_Port.SetPartyOdd();
		}
		
		
		m_Port.WriteFifo( m_RetailTxBuffer[m_OutputCount] );	/* Send character */
		m_PollResponseTimeout4683.Reset();
	}
	else		/* Timed out!  Too late to send any data - QUIT complete */
	{
		m_Port.TxEmptyIrqDisable();
		m_Port.TxCompleteIrqDisable();
		Tx485Disable();						/*  ...then disable the 75176 transmitter */
		m_Port.RxByteByByteIrqEnable();			/* allow receiving data from 4683 */
		Rx485Enable();						/* (enable reciever). */
		m_Port.SetPartyOdd();						/* Set parity to ODD always for receiving! */
		m_EOP_Sent = 0;
	   m_OutputCount=0;
	   TxInProgress=false;
	}
	return(0);}

/***************************************************************************
 * will transmit EOP to the 4683
***************************************************************************/
void CSerial4683::TransmitEOP4683(void)
{
	while( !m_PollResponseDelay4683.HasExpired());
	if(!m_PollResponseTimeout4683.HasExpired())
	{
		TxInProgress=true;
		Rx485Disable();							/*  ...then disable the 75176 reciever */
		m_Port.RxByteByByteIrqDisable();				/* disable receiving data from 4683 */
		Tx485Enable();							/* (enable transmitter). */
			
		m_Port.WriteFifo(END_OF_POLL);				/* Send the <EOP> character. */
		m_EOP_Sent = 1;
		m_Port.TxCompleteIrqEnable();
	}
}

/***************************************************************************
 * will transmit the first char of a frame to the 4683
***************************************************************************/
void CSerial4683::Start4683Transmitter(void)
{
	m_OutputCount = 0;
	while( !m_PollResponseDelay4683.HasExpired());
	
	if(!m_PollResponseTimeout4683.HasExpired())
	{		
		TxInProgress=true;
		if(m_CParity.ParityTable[m_RetailTxBuffer[m_OutputCount]])
			m_Port.SetPartyOdd();
		else
			m_Port.SetPartyEven();
		Rx485Disable();                              // (disable reciever). 
		m_Port.RxByteByByteIrqDisable();                    // allow receiving data from 4683 
		m_Port.TxEnable();                                  // Enable the 75176 transmitter (disable receiver).
		Tx485Enable();                               // (disable reciever).
		m_Port.WriteFifo(m_RetailTxBuffer[m_OutputCount]);	// Send character

		m_Port.TxEmptyIrqEnable();
	}
}


/*
	Here we execute code which is too slow to be in the Receive4683 ISR.
	It runs as a separate thread
*/
void CSerial4683::ThreadLoop(void)
{
	
    ULONG signal;
    CommandHandler4683 * pRetailPort=&Primary4683;;   // Pointer to be used to select the current port as specified by Receive4683
    PrepareWaitForSignal(Sig_4683Event);
	
	while(1)
    {
        signal = WaitForAnySignal(Sig_4683Event);
    	
		if(Sig_4683Event & signal)
		{

	        PrepareWaitForSignal(Sig_4683Event);
            if( CurrentRetailAddress == AuxRetailAddress )
            {
                pRetailPort = &Aux4683;
            }

            m_RetailTxBuffer = pRetailPort->GetTxMessage(); // Set output buffer to the current port's

	        if (receivedPoll == true) // poll
			{
				pRetailPort->GotPolled();
				m_TxFrameLength = pRetailPort->GetTxFrameLength();
				if (m_TxFrameLength > 0) // Any data to send?
				{
					SendFrame();
				}
				else
				{
					TransmitEOP4683();
				}
			}
			else
	        {
                if( CheckFrame() )       // Check the CRCs
                {
                    if( CurrentRetailAddress != BROADCAST_ADDRESS ) // Have the current port handle their message
                    {
                        pRetailPort->ProcessRetailMessage(RetailRxBuf, m_RxFrameLength);
                    }
                    else if( CurrentRetailAddress == BROADCAST_ADDRESS && AuxRetailAddress != 0x00 )    // Have both ports handle a broadcast message
                    {
                        Primary4683.ProcessRetailMessage(RetailRxBuf, m_RxFrameLength);
                        Aux4683.ProcessRetailMessage(RetailRxBuf, m_RxFrameLength);
                    }
                    else // Aux port is disabled, don't have it handle the message
                    {
                        Primary4683.ProcessRetailMessage(RetailRxBuf, m_RxFrameLength);
                    }
                }	        	
	        }
	        m_RxState = FIRST_CHAR;     // set state for next frame 
	        m_RxFrameLength = 0;        // reset length                 

		}

    }    
}



/**********************************************************************************************
 * this function will determine what kind of data we have received from the 4683 terminal
 * we will only get in here if we rx the correct address.  Return TRUE if a message is good
 * and no errors.
**********************************************************************************************/
bool CSerial4683::CheckFrame(void)
{
	CSerial4683::crc_t CrcReceived, CrcCalc;
	unsigned char *Buffer = RetailRxBuf;
	int Length = m_RxFrameLength;

	CrcReceived.crclo = Buffer[Length-3];   // Store the incoming CRC
	CrcReceived.crchi = Buffer[Length-2];
	CrcCalc = CalcCrc(Buffer, Length-3);    // Calculate CRC on our end
	if(CrcCalc.crchi == CrcReceived.crchi && CrcCalc.crclo == CrcReceived.crclo)
	{
		return true;
    }
    else
    {
        return false;
    }
}







