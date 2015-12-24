/*=============================================================================
 * This file contains a init function used on power up and a process function for
 * Processing a received message.

 *=============================================================================
 * $RCSfile: interfaces/4683pro.cpp $
 * $Revision: 2.19.1.10.1.5 $
 * $Date: 2010/09/27 15:23:47EDT $
 *============================================================================*/

#include "stdInclude.h"
#include "CommandHandler4683.h"
#include "def4683.h"
#include "RetailAppFlash.h"
#include "readerconfiguration.h"
#include "productconfiguration.h"
#include "DirectIoParser.h"
#include "BarcodeFifo.h"
#include "Beep.h"
#include "db_hdw.h"          /* artwork dependant hardware definitions    */

#if defined(SHOW_RESET_TO_BOOT) && !defined(MATRIX_BOOT_CODE)
#define reset_to_boot_code_hard() _reset_to_boot_code_hard(__LINE__)
void _reset_to_boot_code_hard(int);
#else
void reset_to_boot_code_hard(void);
#endif

/* #define Debug 1*/									/* use this for development only */

/****************************************************************************
 *
 *  Function:   GotPolled
 *              Handles a poll based on the current state of the scanner.
 *              This function should setup the output frame type, the control
 *              character, and in some cases the TxDataLength.
 *
 * **************************************************************************/
void CommandHandler4683::GotPolled()
{
    CBarcodeData    *pBarcode;

    switch(retailState)
    {
    case WAIT_POLL_OR_NS_FRAME:
        SendROL();
        sendStatus=true;
        break;
    case COMMAND_REJECT_MODE:
        SendMessage();
        break;
    case SEND_I_FRAME:
    case NORMAL_RESPONSE_MODE:

        if(dataWasSent)                                             // Resend last message
        {
            //ReSendFrame();    // Don't need to do anything, just leave lengths alone
        }
        else if(outputState == MESSAGE_READY)                       // Send ready message
        {
            SendMessage();
        }
        else if (outputState == SEND_STATUS || sendStatus == true || HasStatusChanged() == true)  // Send Status
        {
            sendStatus = false;
            ControlType.Char.All = 0;
            ControlType.Char.Bits.bSI_Frame = 1;
            controlChar = S_R_Count.Char.Raw;
            TxDataLength = -1;
            SendMessage();                      // no data in response status only
        }
        else if ( BarcodeFifo->AnyData() || pCurrentBarcode != NULL )    // Send barcodes
        {
            pBarcode = GetBarcodeToSend();
            if( CanYouSendThisBarcode( pBarcode ) )
            {
                SentBarcode();
            }
        }
        else if (retailState==SEND_I_FRAME)                         // if we dont have data and need to ack a frame send RR
        {
            SendRRFrame();
        }
        else
        {
            SendEOP();
        }
        retailState = NORMAL_RESPONSE_MODE;
        break;
    case WAIT_POLL_SEND_NS_RESPONSE:
        if( outputState == NO_MESSAGE )
        {
            retailState = NORMAL_RESPONSE_MODE;
            SendNsaFrame();
        }
    default:
        break;
    }
}

/****************************************************************************
 *
 *  Function:   ProcessRetailMessage
 *              Process the incoming message.  Start by copying the message
 *              from the link layer's input buffer because it may get over-
 *              written.  Make sure to handle a reset or SNRM first, then
 *              process the incoming message based on our state.
 *              This function should modify the state, status, and will
 *              create a response to the incoming message.
 *
 *  Arguments:  unsigned char * buffer  - Pointer to the incoming message
 *              int             length  - Incoming message length
 *
 * **************************************************************************/
void CommandHandler4683::ProcessRetailMessage( unsigned char* buffer, int length )
{
    RxFrameLength = length;
    memcpy( RetailRxBuf, buffer, RxFrameLength );

    RetailRxMessage_t   *pRetailRxType = &RetailRxType;
    unsigned char *Message = RetailRxBuf;
    //int MessageLength = RxFrameLength;
    unsigned char executeResult = 0;
    unsigned char Ack = ACK;

    if(Message[SYSTEM_CHAR1] == 0x00 && Message[SYSTEM_CHAR2] == 0x40 && RxFrameLength == 0x07)
    {
        SimulateScannerReset();
        return;
    }
    if (!CheckRetailFrame())	// Fill in RetailRxType
    {
        return; // unknown or bad frame type
    }
    if(pRetailRxType->Char.Bits.bSNRM_Frame)
    {
        retailState = WAIT_POLL_SEND_NS_RESPONSE;
        ResetSendReceiveCounts();       // Clear send receive counts
        ClearTxBuffer();                // clear tx buffer
        InitStatusBytes();              // Reset status bytes
        return;
    }

//    RESET_WATCHDOG_TIMER();
    switch(retailState)
    {
    case SEND_I_FRAME:
    case NORMAL_RESPONSE_MODE:
        if( pRetailRxType->Char.Bits.bNSI_Frame )
        {
            executeResult = ExecuteCommand( Message, RxFrameLength );
            if( executeResult == GOOD_COMMAND )
            {
                retailState = WAIT_POLL_SEND_NS_RESPONSE;
            }
            else if( executeResult == BAD_COMMAND )
            {
                retailState = COMMAND_REJECT_MODE;
            }
            // Last case is BAD_COMMAND_BROADCAST_ADDRESS
        }
        else if( pRetailRxType->Char.Bits.bRR_Frame )
        {
            retailState = NORMAL_RESPONSE_MODE;
            if( dataWasSent )
            {
                Translate4680Command(&Ack); // Receive Ack - calls ClearTxBuffer()
                dataWasSent = false;        // Turn flag off... can be in a better format/function
            }
            ClearTxBuffer();
        }
        else if( pRetailRxType->Char.Bits.bSI_Frame )
        {
            if( dataWasSent )
            {
                Translate4680Command(&Ack); // Receive Ack - calls ClearTxBuffer()
                dataWasSent = false;        // Turn flag off... can be in a better format/function
            }
            ClearTxBuffer();
            executeResult = ExecuteCommand( Message, RxFrameLength );
            if( executeResult == GOOD_COMMAND )
            {
                retailState = SEND_I_FRAME;
            }
            else if( executeResult == BAD_COMMAND )
            {
                retailState = COMMAND_REJECT_MODE;
            }
            // Last case is BAD_COMMAND_BROADCAST_ADDRESS
        }
        else if( pRetailRxType->Char.Bits.bErrorFrame )
        {
            retailState = SEND_I_FRAME;
            ControlType.Char.All = 0;
            ControlType.Char.Bits.bNSI_Frame = 1;
            controlChar = ((S_R_Count.Char.Raw & 0xE0)|0x01);
            outputState = MESSAGE_READY;
        }
        break;
    case WAIT_POLL_OR_NS_FRAME:
    case COMMAND_REJECT_MODE:
    case WAIT_POLL_SEND_NS_RESPONSE:
    default:
        break;
    }
}

/****************************************************************************
 *
 *  Function:   SendNSAFrame
 *              Setup the port to send a Non-sequential ack frame
 *
 * **************************************************************************/
void CommandHandler4683::SendNsaFrame(void)
{
    ControlType.Char.All =0;
    ControlType.Char.Bits.bNSI_Frame =1;
    controlChar = NON_SEQUENCED_ACK;
    TxDataLength = -1;              // no data in response 
    SendRetailFrame(TxMessage);     // send message 
}

/****************************************************************************
 *
 *  Function:   SendRRFrame
 *              Setup the port to send a Receiver Ready frame
 *
 * **************************************************************************/
void CommandHandler4683::SendRRFrame(void)
{
    ControlType.Char.All =0;
    ControlType.Char.Bits.bNSI_Frame =1;
    controlChar = ((S_R_Count.Char.Raw & 0xE0) | 0x01);
    TxDataLength = -1;              // no data in response 
    SendRetailFrame(TxMessage);     // send message 
}

/****************************************************************************
 *
 *  Function:   SendROL
 *              Setup the port to send a Request Online
 *
 * **************************************************************************/
void CommandHandler4683::SendROL(void)
{
	ControlType.Char.All = 0;
	ControlType.Char.Bits.bNSI_Frame =1;
	controlChar = REQUEST_ON_LINE;
	TxDataLength = -1;              // no data in response 
	SendRetailFrame(TxMessage);     // send message 
}

/****************************************************************************
 *
 *  Function:   SendEOP
 *              Setup the port to send a End of poll 
 *
 * **************************************************************************/
void CommandHandler4683::SendEOP(void)
{
	TxFrameLength = 0;
}

/****************************************************************************
 *
 *  Function:   ResetSendReceiveCounts
 *              Resets the set and receive counts to zero
 *
 * **************************************************************************/
void CommandHandler4683::ResetSendReceiveCounts(void)
{
    S_R_Count.Char.Raw = 0; // Make the ALL bits in the Receive/Send count ZERO! 
}

/****************************************************************************
 *
 *  Function:   ExecuteCommand
 *              This function will process the received frame.  In some cases
 *              it will build a response to be sent on the next poll.  It may
 *              modify the status, TxMessage, and control type and character
 *
 *  Arguments:  unsigned char * Message - Pointer to the incoming frame
 *              int             Length  - Lenght of the incoming frame
 *
 *  Returns:    int:    0 = Command Recognized - send proper response
 *                      1 = Command Not Recognized - send command reject
 *                      2 = Broadcast Command Not Recognized - do nothing
 *
 * **************************************************************************/
char CommandHandler4683::ExecuteCommand(unsigned char * Message, int Length)
{
	char	i;
	RetailRxMessage_t	*pRetailRxType = &RetailRxType;
	TxDataLength = 0;												 					/* reset the length of the data */
	if(retailPort == PORT_9B1_ID || retailPort == PORT_9B2_ID)
	{
		ControlType.Char.All = 0;
		ControlType = *pRetailRxType;										/* this is the default case! */
		controlChar = S_R_Count.Char.Raw;							/* default */
		outputState = MESSAGE_READY;											/* default */
		Status.Port9BControl.Status.Byte1.bAckResponse = 1;						/* set ack bit default */
		statusPtr[2] = 0;
		if(Message[SYSTEM_CHAR1] == SCANNER_ENABLE)
		{
			Status.Port9BControl.Status.Byte2.bReaderEnable = 1; 				/* perform function */
		}
		else if(Message[SYSTEM_CHAR1] == SCANNER_DISABLE)
		{
			Status.Port9BControl.Status.Byte2.bReaderEnable = 0; 				/* perform function */
		}
		else if(Message[SYSTEM_CHAR1] ==  BEEPER_ENABLE)
		{
			Status.Port9BControl.Status.Byte2.bBeeperEnable = 1; 				/* perform function */
		}
		else if(Message[SYSTEM_CHAR1] == BEEPER_DISABLE)
		{
			Status.Port9BControl.Status.Byte2.bBeeperEnable = 0; 				/* perform function */
		}
		else if(Message[SYSTEM_CHAR1] == FLASH_30_COMMAND)
		{
			statusPtr[2] = 0x20;	// Direct IO command undefined
			TxDataLength = 0;
		}
		else if(Message[SYSTEM_CHAR1] == MODE_SELECT)
		{
			if(Length == 11)																/* if length is 9 then we need to skip past comand byte2 */
			{
				Message[SYSTEM_CHAR2] = Message[SYSTEM_CHAR3];					/* skip past char we don't need */
				Status.Port9BControl.Config.Char.ModeByte = Message[SYSTEM_CHAR2];	/* save the mode in case we get asked later */
			}
			else if(Length == 10)														/* else if message is 8 then don't skip */
			{
				Status.Port9BControl.Config.Char.ModeByte = Message[SYSTEM_CHAR2];	/* save the mode in case we get asked later */
			}
			else
			{
				return(AcceptCommand(Message));
			}
		}
		else if(Message[SYSTEM_CHAR1] == LENGTH_LIMIT)
		{
			if(Length == 8)
			{
				Message[SYSTEM_CHAR2] = Message[SYSTEM_CHAR3];					/* Move the character where we can use it */
				Status.Port9BControl.Config.nLengthLimit = Message[SYSTEM_CHAR2];	/* store length limit */
			}
			else if(Length == 7)
			{
				Status.Port9BControl.Config.nLengthLimit = Message[SYSTEM_CHAR2];	/* store length limit */
			}
			else
			{
				return(AcceptCommand(Message));
			}
		}
		else if(Message[SYSTEM_CHAR1] == READ_MODE_STATUS)
		{
			TxDataLength=0;
			Status.Port9BControl.Status.Byte1.bAckResponse = 0;							/* clear ack bit */
			Status.Port9BControl.Status.Byte1.bModeDataInResponse = 1;
			TxMessage[TxDataLength++] = Status.Port9BControl.Config.Char.ModeByte;		/* Put mode data in response */
			TxMessage[TxDataLength++] = 0x00;
			TxMessage[TxDataLength++] = 0x00;												/* fill the rest with zeros */
			TxMessage[TxDataLength++] = 0x00;
		}
		else if(Message[SYSTEM_CHAR1] == SCANNER_INIT)
		{
         Status.Port9BControl.Status.Byte1.bAckResponse = 1;                 /* set ack bit default */
		}
		else if(Message[SYSTEM_CHAR1] == SYSTEM_COMMAND)
		{
			Status.Port9BControl.Status.Byte1.bAckResponse = 0;							/* clear ack bit */
			if(Message[SYSTEM_CHAR2] == TEST)
			{
				Status.Port9BControl.Status.Byte1.bTestEndResponse = 1;					/* set test bit */
			}
			else if(Message[SYSTEM_CHAR2] == STATUS_REQUEST)
			{
				Status.Port9BControl.Status.Byte1.bStatusResponse = 1;					/* set status bit */
			}
			else if(Message[SYSTEM_CHAR2] == POR && Length == 0x07)
			{
				return(SimulateScannerReset());
			}
			else if(Message[SYSTEM_CHAR2] == EC_READOUT)
			{
				TxDataLength = 02;
				TxMessage[0] = 0x00;
				TxMessage[1] = EC_READOUT_BYTE;
				Status.Port9BControl.Status.Byte1.bECReadoutResponse = 1;
				ControlType.Char.Bits.bSI_Frame = 1;
			}
			else
			{
				return(AcceptCommand(Message));
			}
		}
		else
		{
			return(AcceptCommand(Message));
		}
	}
//5B ****************
	else if(retailPort == PORT_5B_ID)
	{
		ControlType.Char.All = 0;
		ControlType = *pRetailRxType;									/* this is the default case! */
		controlChar = S_R_Count.Char.Raw;								/* default */
		outputState = MESSAGE_READY;										/* default */
		if(Message[SYSTEM_CHAR1] == SCANNER_ENABLE)
		{
			Status.Port5BControl.Status.Byte2.bScannerEnabled = 1;
		}
		else if(Message[SYSTEM_CHAR1] == SCANNER_DISABLE)
		{
			Status.Port5BControl.Status.Byte2.bScannerEnabled = 0;
		}
		else if(Message[SYSTEM_CHAR1] == BEEPER_ENABLE)
		{
			Status.Port5BControl.Status.Byte1.bScannerMainBeepEnabled = 1;
		}
		else if(Message[SYSTEM_CHAR1] == BEEPER_DISABLE)
		{
			Status.Port5BControl.Status.Byte1.bScannerMainBeepEnabled = 0;
		}
		else if(Message[SYSTEM_CHAR1] == BEEPER2_ENABLE)
		{
			Status.Port5BControl.Status.Byte1.bScannerHeadBeepDisabled = 0;
		}
		else if(Message[SYSTEM_CHAR1] == BEEPER2_DISABLE)
		{
			Status.Port5BControl.Status.Byte1.bScannerHeadBeepDisabled = 1;
		}
		else if(Message[SYSTEM_CHAR1] == P2_2_ON)
		{
			Status.Port5BControl.Status.Byte1.bPort2Bit2 = 1;
		}
		else if(Message[SYSTEM_CHAR1] == P2_2_OFF)
		{
			Status.Port5BControl.Status.Byte1.bPort2Bit2 = 0;
		}
		else if(Message[SYSTEM_CHAR1] == P2_0_ON)
		{
			Status.Port5BControl.Status.Byte1.bPort2Bit0 = 1;
		}
		else if(Message[SYSTEM_CHAR1] == FLASH_30_COMMAND ||
				Message[SYSTEM_CHAR1] == LONG_DIRECT_IO_COMMAND)
		{
			// Send length - 2 to compensate for cutting off the first two bytes
			// Subtract 3 more for the CRC and EOP flag so USB and RS485 send equivalent messages
			int dioResult = dioParser->ExecuteDirectIOCommand(&Message[SYSTEM_CHAR1], Length-5);
			if( dioResult == DIO_UNDEFINED )
			{
				DirectIOCommandUndefined();
			}
			else if( dioResult == DIO_ACCEPTED )
			{
				DirectIOCommandAccepted(false); // no data to return
			}
			else if( dioResult == DIO_ACCEPTED_WITH_DATA )
			{
				DirectIOCommandAccepted(true); // has data to return
				unsigned char *returnMsg;
				TxDataLength = dioParser->GetResponse( &returnMsg );				
				for(int i = 0; i < TxDataLength; i++)
				{
					TxMessage[i] = returnMsg[i];
				}
			}
			else if( dioResult == DIO_NOT_ALLOWED )
			{
				DirectIOCommandNotAllowed();
			}
		} 
		else if(Message[SYSTEM_CHAR1] == STATUS_OUT)
		{
			Status.Port5BControl.Status.Byte2.bPort0WriteAck = 1;
		//	m_IBM4680. = Message[DATA_BYTE_1];				  			/* save the Status Out Byte */
		}
		else if(Message[SYSTEM_CHAR1] == RESET_SCANNER)
		{
			Status.Port5BControl.Status.Byte1.bScannerHWResetAck = 1;
		}
		else if(Message[SYSTEM_CHAR1] == SYSTEM_COMMAND)
		{
			if(Message[SYSTEM_CHAR2] == POR && Length == 0x07)
			{
				return(SimulateScannerReset());
			}
			else if(Message[SYSTEM_CHAR2] == STATUS_REQUEST)
			{
				/* do the default ?? */
			}
			else if(Message[SYSTEM_CHAR2] == EC_READOUT)
			{
				Status.Port5BControl.Status.Byte1.bECLevelFollows = 1;		/* ec level folows status */
				TxMessage[0] = EC_READOUT_BYTE;
				TxDataLength = 1;
			}
			else if(Message[SYSTEM_CHAR2] == TEST)
			{
				/* do the default ?? */
			}
			else
			{
				return(AcceptCommand(Message));
			}
		}
		else
		{
			return(AcceptCommand(Message));
		}
	}
//17 ****************
	else if(retailPort == PORT_17_ID)
	{
		ControlType.Char.All = 0;
		ControlType = *pRetailRxType;										/* this is the default case! */
		controlChar = S_R_Count.Char.Raw;							/* default */
		outputState = MESSAGE_READY;											/* default */
		statusPtr[2] = 0;
		if(Message[SYSTEM_CHAR1] == SCANNER_ENABLE)
		{
			Status.Port17Control.Status.Byte1.bScannerEnabledAndLaserOn = 1;
		}
		else if(Message[SYSTEM_CHAR1] == SCANNER_DISABLE)
		{
			Status.Port17Control.Status.Byte1.bScannerEnabledAndLaserOn = 0;
		}
		else if(Message[SYSTEM_CHAR1] == BEEPER_ENABLE)
		{
			Status.Port17Control.Status.Byte0.bScannerBeepEnabled = 1;
		}
		else if(Message[SYSTEM_CHAR1] == BEEPER_DISABLE)
		{
			Status.Port17Control.Status.Byte0.bScannerBeepEnabled = 0;
		}
		else if(Message[SYSTEM_CHAR1] == FLASH_30_COMMAND)
		{
			statusPtr[2] = 0x20;	// Direct IO command undefined
			TxDataLength = 0;
		}
		else if(Message[SYSTEM_CHAR1] == CONFIGURE_SCANNER)
		{
			for(i=0;i<8;i++)
			{
				Status.Port17Control.Config.ConfigAll[i] = Message[i+DATA_BYTE_1];
			}
			Status.Port17Control.Status.Byte2.bConfigureScannerCommandAccepted = 1;
		}
		else if(Message[SYSTEM_CHAR1] == SYSTEM_COMMAND)
		{
			if(Message[SYSTEM_CHAR2] == POR && Length == 0x07)
			{
				return(SimulateScannerReset());
			}
			else if(Message[SYSTEM_CHAR2] == STATUS_REQUEST)
			{
				/* do the default ?? */
			}
			else if(Message[SYSTEM_CHAR2] == EC_READOUT)
			{
				Status.Port17Control.Status.Byte0.bECLevelFollows = 1;		/* ec level folows status */
				TxMessage[0] = EC_READOUT_BYTE_17;
				TxDataLength = 1;
			}
			else if(Message[SYSTEM_CHAR2] == TEST)
			{
				/* do the default ?? */
			}
			else
			{
				return(AcceptCommand(Message));
			}
		}
		else
		{
		 	return(AcceptCommand(Message));
		}
	}
    else if(AmIPort6x())
    {
        int TempTimeout = 0;
        unsigned int* puchStatus;
        ControlType.Char.All = 0;
		ControlType = *pRetailRxType;									/* this is the default case! */
		controlChar = S_R_Count.Char.Raw;								/* default */
		outputState = MESSAGE_READY;										/* default */

        // Indicate that out receive buffer is not empty
        Status.Port6XControl.Status.Byte2.bTxBufferEmpty = 1;

		if(Message[SYSTEM_CHAR1] == IBM_SETUP)
		{
            /* SETUP will initialize a channel.  SETUP will clear any data
             * in the TRANSMIT and RECEIVE buffers, reset the channel, then 
             * reinitialize to the indicated state. */
            puchStatus = (unsigned int *)&Status.Port6XControl;
            puchStatus[0] = 0x0;
            puchStatus[1] = 0x46;    // RTS | DTR | INITIALIZED
            /*Bytes 3 through 8 contain the following configuration data:
             * [3]      BAUDRATE                No action required
             * [4]      CHANNEL CONFIGURATION   No action required
             * [5]      CHANNEL STATUS          Store Status   
             * [6,7,8]  RECEIVE TIMEOUT         Define timeout
             */
            port6xChannelStatus.Char.All = Message[5];

            TempTimeout =  Message[7];  // The first 2 bytes represent the 2's complement of some fixed unit
            TempTimeout *= 256;         // of time in microseconds, they're sent least significant byte first
            TempTimeout += Message[6];                          // The 3rd byte is a number (max 255) that
            port6xTimeout = 0x10000 - TempTimeout;    // represents the number of defined time periods
            port6xTimeout *= Message[8];              // that expire before a timeout occurs. 
            if( port6xChannelStatus.Char.Bits.CTS )
            {
                scannerStatus.ScannerDisabled = 0;
            }
        }
		else if(Message[SYSTEM_CHAR1] == IBM_CHANNEL_CONTROL)
		{
            port6xChannelStatus.Char.All = Message[SYSTEM_CHAR2];
		}
		else if(Message[SYSTEM_CHAR1] == IBM_READ_STATUS)
		{
            sendStatus = true;
		}
		else if(Message[SYSTEM_CHAR1] == IBM_TRANSMIT)
		{
            const char* SavedMessage;       // RxFifo->Write requires a const char*
            int SavedMessageLength = 0;     // Incoming message length

            // already performed a memcpy on this data, another memcpy occurs in the Rxfifo->Write
            SavedMessage = (const char*)&Message[3];
            SavedMessageLength = Length - 6;

            RxFifo->Write(SavedMessage,(size_t)SavedMessageLength);
		}
		else if(Message[SYSTEM_CHAR1] == IBM_RECEIVE_BUFFER)
		{
            // outputState is set above to Message Ready - will automatically send
            // pending data as long as CTS is enabled
		}
		else if(Message[SYSTEM_CHAR1] == IBM_WRAP)
		{
            // If the wrap executes correctly, a standard status
            // message will be sent that does not indicate an error
            sendStatus = true;
		}
		else if(Message[SYSTEM_CHAR1] == IBM_SYSTEM_COMMAND)			// system commands start here
		{
            Status.Port6XControl.Status.Byte2.bTxBufferEmpty = 0;   // Do not indicate data received on system commands
            if(Message[SYSTEM_CHAR2] == IBM_RESET)
            {
                return(SimulateScannerReset());
            }
			else if(Message[SYSTEM_CHAR2] == IBM_STATUS_REQUEST)
			{
                sendStatus = true;
			}
			else if(Message[SYSTEM_CHAR2] == IBM_TEST)
			{
                sendStatus = true;
			}
			else if(Message[SYSTEM_CHAR2] == IBM_EC_LEVEL_REQUEST)
			{
                Status.Port6XControl.Status.Byte2.bECLevelFollows = 1;     /* ec level folows status */
                TxMessage[0] = EC_LEVEL;
                TxDataLength = 1;
                ControlType.Char.Bits.bSI_Frame = 1;
			}
			else
			{
				return(AcceptCommand(Message));
			}
        }
        else 
        {
            return(AcceptCommand(Message));
        }
    }
	Translate4680Command(&Message[SYSTEM_CHAR1]);  			/* send message to host */
	return GOOD_COMMAND;
}

/****************************************************************************
 *
 *  Function:   CheckRetailFrame
 *              Check the frame for its type and send and receive counts.  The
 *              link layer has already verified the CRCs.  Set the incoming
 *              frame type and 
 *
 *  Arguments:  unsigned char * buffer  - Pointer to the incoming message
 *              int             length  - Incoming message length
 *
 *  Returns:    bool        True if it is a correctly formatted frame
 *
 * **************************************************************************/
bool CommandHandler4683::CheckRetailFrame(void)
{
	RetailRxMessage_t	*pRetailRxType = &RetailRxType;
	unsigned char *Buffer = RetailRxBuf;
	pRetailRxType->Char.All = 0;    // set this to zero 

    if((Buffer[CONTROL_CHAR] & NON_SEQUENCED_MASK) == NON_SEQUENCED_TYPE)		// is it a Non Sequenced Frame (NSF) 
    {
        if(Buffer[CONTROL_CHAR] == SEND_NORMAL_RESPONSE_MODE)							// is it a snrm 
        {
            pRetailRxType->Char.Bits.bSNRM_Frame = 1;
        }
        else if(Buffer[CONTROL_CHAR] == NON_SEQUENCED_TYPE)							// is it a NSI 
        {
            pRetailRxType->Char.Bits.bNSI_Frame = 1;
        }
        else
        {
            return false;		// not a valid type -> by doing nothing we will return false 
        }
    }
    else if((Buffer[CONTROL_CHAR] & SEQUENCED_MASK) == SEQUENCED_TYPE)			// is it a Sequenced Frame (SF) 
    {
        if(((FrameChar_t *)(&Buffer[CONTROL_CHAR]))->Char.SeqInfo.RecNum == S_R_Count.Char.SeqInfo.SendNum)   // does the send count = receive count 
        {
            // now check that this message is not a duplicate of the last 
            S_R_Count.Char.SeqInfo.RecNum++;      // yes then update rx count 
            pRetailRxType->Char.Bits.bSI_Frame = 1;
        }
        else
        {
            pRetailRxType->Char.Bits.bErrorFrame = 1;
        }
    }
    else if((Buffer[CONTROL_CHAR] & SUPERVISORY_MASK) == SUPERVISORY_TYPE)		// or a Supervisory Frame
    {
        if(((FrameChar_t *)(&Buffer[CONTROL_CHAR]))->Char.SeqInfo.RecNum == S_R_Count.Char.SeqInfo.SendNum)   // does the send count = receive count
        {
            pRetailRxType->Char.Bits.bRR_Frame = 1;
        }
        else
        {
            return false;		/* not a valid type -> by doing nothing we will return false */
        }
    }
	else
    {
        return false;		/* we don't know this type -> by doing nothing we will return false */
    }
    return true;    // Message handled and is a good type
}

/****************************************************************************
 *
 *  Function:   AmIPort6x
 *              Determines if the retailPort is Ports 64, 65, 68, or 69 this 
 *              was commonly used code so it was placed in a function
 *
 *  Returns:    True if the retailPort is Port 64, 65, 68, or 69
 *
 * **************************************************************************/
bool CommandHandler4683::AmIPort6x(void)
{
    if(retailPort == PORT_64_ID || retailPort == PORT_65_ID || 
       retailPort == PORT_68_ID || retailPort == PORT_69_ID)
    {
        return true;
    }
    else 
    {
        return false;
    }
}

/****************************************************************************
 *  Function:   CanITransmit6x
 *              This will determine if we're able to transmit barcode/menu data 
 *              over port 6x.  The criteria include: Are we port 6x then is the 
 *              CTS line enabled, or has the transmit timeout expired, or has 
 *              the controller explicitly told us to transmit, or is Txbuffer full?
 *
 *  Returns:    True if we are one of the 6x ports and are able to transmit
 *
 * **************************************************************************/
bool CommandHandler4683::CanITransmit6x(void)
{
    if(AmIPort6x())
    {
        if( port6xChannelStatus.Char.Bits.CTS )
            return true;   
        //else if( m_IBM4680.port6xTimer.HasExpired() && port6xTimeout != 0 )
            //return true;
        else if( Status.Port6XControl.Status.Byte1.bRxBuffer1Full == 1 ||
            Status.Port6XControl.Status.Byte1.bRxBuffer2Full == 1 )
            return true;
        else 
            return false;  
    }
    else 
    {
        return false;
    }
}

/****************************************************************************
 *
 *  Function:   SimulateScannerReset
 *              Perform a 'Power-On' reset by clearing buffers and resetting
 *              status
 *
 *  Retrns:     int     BAD_COMMAND_BROADCAST_ADDRESS
 *
 * **************************************************************************/
int CommandHandler4683::SimulateScannerReset(void)
{
	if(IsBeepQueEmpty()==TRUE)
    {
		SoundPowerupBeep();
    }

	DISABLE_INTERRUPTS();
	ClearTxBuffer();
    Initialize();
    ResetSendReceiveCounts();
	RESTORE_INTERRUPTS();

	return BAD_COMMAND_BROADCAST_ADDRESS;
}

/****************************************************************************
 *
 *  Function:   SendMessage
 *              Tells the port to create a retail frame using the TxBuffer
 *              Any buffer can be sent as an argument to SendRetailFrame
 *
 * **************************************************************************/
void CommandHandler4683::SendMessage(void)
{
	SendRetailFrame(TxMessage);   // Create retail frame with our current buffer	
}

/****************************************************************************
 *
 *  Function:   SendRetailFrame
 *              This function serves as the main routine for sending out any
 *              message in a frame format.  This routine will automatically 
 *              append the retailAddress and control character, so those should
 *              be setup before calling this routine.  The argument is appended
 *              in the data section of a frame.  TxDataLength will be used to 
 *              determine how much data to append so make sure that is setup
 *              before calling this function as well.
 *
 *  Arguments:  unsigned char * Message - Data that will be appended to the frame
 *                              Its length will be determined by TxDataLength
 *
 * **************************************************************************/
void CommandHandler4683::SendRetailFrame(unsigned char* Message)
{
	unsigned char   *TempBufPtr	= RetailTxBuf;

	*TempBufPtr++ = retailAddress;              // insert 4683 address
	*TempBufPtr++ = controlChar;

	if(ControlType.Char.Bits.bSI_Frame == 1)
	{
		*TempBufPtr++ = statusPtr[0];
		*TempBufPtr++ = statusPtr[1];           // insert the status bytes 
		dataWasSent=true;                       // this frame sequenced resend until acked
		if(retailPort == PORT_5B_ID || retailPort == PORT_17_ID)
		{
			*TempBufPtr++ = statusPtr[2];       // insert the status bytes
		}
	}

	for(int i = 0; i < TxDataLength; i++)
	{
		*TempBufPtr++ = Message[i];
	}
    TxFrameLength = TempBufPtr - RetailTxBuf;
	ClearStatusBits();                          // clear the status bits 
	if(ControlType.Char.Bits.bSI_Frame == 1)
	{
		(S_R_Count.Char.SeqInfo.SendNum)++;     // update send counts
	}
	outputState = NO_MESSAGE;                   // update output state 
}

/****************************************************************************
 *
 *  Function:   AcceptCommand
 *              Sets status bits in the event of an unknown command sent to 
 *              our address.  If its a broadcast command - do nothing.
 *
 *  Arguments:  unsigned char * Message - Pointer to the incoming frame
 *
 *  Returns:    BAD_COMMAND                     if its our address
 *              BAD_COMMAND_BROADCAST_ADDRESS   if broadcast message
 *
 * **************************************************************************/
char CommandHandler4683::AcceptCommand(unsigned char * Message)
{
	RetailRxMessage_t	*pRetailRxType = &RetailRxType;
	if(retailPort == PORT_9B1_ID || retailPort == PORT_9B2_ID)
		Status.Port9BControl.Status.Byte1.bAckResponse = 0;							/* clear ack bit */

	if(BROADCAST_ADDRESS != Message[0])
	{
		if(pRetailRxType->Char.Bits.bSI_Frame)
		{
			// Send a sequenced command reject
			if(retailPort == PORT_9B1_ID || retailPort == PORT_9B2_ID)
				Status.Port9BControl.Status.Byte2.bCommandRejected = 1;				/* command reject */
			else if(retailPort == PORT_5B_ID)
				Status.Port5BControl.Status.Byte2.bCommandReject = 1;					/* command reject */
			else if(retailPort == PORT_17_ID)
				Status.Port17Control.Status.Byte1.bCommandReject = 1;					/* command reject */
            else if(AmIPort6x())
                Status.Port6XControl.Status.Byte2.bCommandError = 1;                  /* command error/reject */
		}
		else
		{
			// Send a non sequenced command reject
			ControlType.Char.All = 0;
			ControlType.Char.Bits.bNSI_Frame = 1;								/* no status */
			controlChar = COMMAND_REJECT;
		}
		return BAD_COMMAND;
	}
	else
	{
		outputState = NO_MESSAGE;										/* don't reject a broadcast message that we don't know */
		return BAD_COMMAND_BROADCAST_ADDRESS;
	}
}

/****************************************************************************
 *
 *  Function:   HasStatusChanged
 *              This function will check and update the scanner's status bits
 *              based on the current status.  Port 6x specifically states that 
 *              each time the status changes that must be conveyed to the host. 
 *
 *  Returns:    True if the status has changed, false otherwise
 *
 * **************************************************************************/
bool CommandHandler4683::HasStatusChanged()
{
    bool returnValue = false;
    if(AmIPort6x())
    {
        if( Status.Port6XControl.Status.Byte2.bTxBufferEmpty == 1 )
        {
            if( RxFifo->IsEmpty() )
            {
                Status.Port6XControl.Status.Byte2.bTxBufferEmpty = 0;
                returnValue = true;
            }
        }
    }
    return returnValue;
}



