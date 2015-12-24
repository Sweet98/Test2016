/*=============================================================================
 * This file contains a function that will get a character string and convert
 * it to a retail message.

 *=============================================================================
 * $RCSfile: interfaces/4683tx.cpp $
 * $Revision: 2.4.1.9.1.2.1.2 $
 * $Date: 2010/08/20 13:09:04EDT $
 *============================================================================*/

#include "stdInclude.h"
#include "CommonDriver.h"
#include "Drvserial4683.h"
#include "def4683.h"

bool IBM4683RetailPDFEnable;
int IBM4683SurePosMaxPacketSize;
extern bool ExecuteRetailPDF(void);

/*********************************************************************************************
 * This function serves as the main routine for sending out any framed message.  The only
 * command that it does not send is a EOP(that is handled at a lower level).
 * Needs *Message: The message you want to send is here.
 * Needs MessageLength: put length of message here. If no message then set to zero.
 * Needs ControlChar: this is the control character that is needed in the frame.
 * This function will put the message in the proper format and send it.
**********************************************************************************************/

// Send Frame will append the CRCs and Flag chracter before transmitting
void CSerial4683::SendFrame(void)
{
	CSerial4683::crc_t      Crc1Crc2;
	unsigned char           *TempBufPtr = m_RetailTxBuffer;
	
    TempBufPtr += m_TxFrameLength;

	Crc1Crc2 = CalcCrc(m_RetailTxBuffer, m_TxFrameLength);     // get CRC 

	*TempBufPtr++ = Crc1Crc2.crclo;     // insert CRC1 
	*TempBufPtr++ = Crc1Crc2.crchi;     // insert CRC2 
	*TempBufPtr++ = FRAME_END_FLAG;     // last insert the frame end char 

	m_TxFrameLength = TempBufPtr - m_RetailTxBuffer;   // get full message length

	Start4683Transmitter();             // now send the frame
}

/****************************************************************************
 *
 *  Function:   ClearTxBuffer
 *              Clears the lengths of the transmit buffer
 *
 * **************************************************************************/
void CommandHandler4683::ClearTxBuffer(void)
{
	TxFrameLength = 0;
	TxDataLength = 0;   // set length to zero to clear tx buffer
}

/****************************************************************************
 *
 *  Function:   ClearStatusBits
 *              Clear the status bits - generally after sending a frame
 *
 * **************************************************************************/
void CommandHandler4683::ClearStatusBits(void)
{
	if(retailPort == PORT_5B_ID)
	{
		Status.Port5BControl.Status.Byte1.bECLevelFollows = 0;		// clear ec level status 
		Status.Port5BControl.Status.Byte1.bScannerHWResetAck = 0;
		Status.Port5BControl.Status.Byte1.bPort2Bit2 = 0;
		Status.Port5BControl.Status.Byte2.bPort0WriteAck = 0;
		Status.Port5BControl.Status.Byte3 = 0;
	}
	else if(retailPort == PORT_9B1_ID || retailPort == PORT_9B2_ID)
	{
		Status.Port9BControl.Status.Byte1.bReadDataInResponse = 0;	// clear data in response bit 
		Status.Port9BControl.Status.Byte1.bAckResponse = 0;			// clear ack bit default 
		Status.Port9BControl.Status.Byte1.bTestEndResponse = 0;		// clear test bit 
		Status.Port9BControl.Status.Byte1.bStatusResponse = 0;		// clear status bit 
		Status.Port9BControl.Status.Byte1.bECReadoutResponse = 0; 	// clear EC bit 
		Status.Port9BControl.Status.Byte1.bModeDataInResponse = 0;	// clear mode data bit 
	}
	else if(retailPort == PORT_17_ID)
	{
		Status.Port17Control.Status.Byte0.bECLevelFollows = 0;        // clear ec level 
		Status.Port17Control.Status.Byte1.bCommandReject = 0;         // clear command reject 
	}
    else if(retailPort == PORT_64_ID || retailPort == PORT_65_ID || retailPort == PORT_68_ID || retailPort == PORT_69_ID)
    {
        Status.Port6XControl.Status.Byte1.bRxBuffer1Empty = 0;        //clear rx buffer 1 empty
        Status.Port6XControl.Status.Byte1.bTimerOverflow = 0;         //clear timer overflow
        Status.Port6XControl.Status.Byte1.bRxBuffer1Full = 0;         //clear rx buffer 1 full
        Status.Port6XControl.Status.Byte2.bCommandError = 0;          //clear rx command error
        Status.Port6XControl.Status.Byte2.bECLevelFollows = 0;        //clear ec level
   }
}

extern int SurePosMaxPacketSize;
/****************************************************************************
 *
 *  Function:   CanYouSendThisBarcode
 *              Recieve a pointer to a barcode object, and if you are able to
 *              send, setup a frame and call SendRetailFrame
 *
 *  Arguments:  CBarcodeData *  pBarcode    - Pointer to ready to send barcode
 *
 *  Returns:    True if you can send the data, false if you can't
 *
 * **************************************************************************/
bool CommandHandler4683::CanYouSendThisBarcode(CBarcodeData *pBarcode)
{
	const bchar_t *pCurrentText = NULL;
	size_t Size = 0;
	int leftToSend = 0;
	bool retVal = true;

	const char Code39SuffixHEX[3] = { 0x00, 0x0a, 0x0b };

	ExecuteRetailPDF(); //update RTLPDF value

	if ((WA_CODELETTER_MENU_RESPONSE != pBarcode->GetHHPId() && !scannerStatus.ScannerDisabled)	|| (CanITransmit6x()))
	{
		pBarcode->GetTotalData(pCurrentText, Size);
		//Yang
		if (!IBM4683RetailPDFEnable) //Menu Command RTLPDF disable
		{
			if (244 > Size) // 4680 system is limited to messages < 256 characters including the overhead.
			{
				TxDataLength = (unsigned char) Size;
				memcpy((char*) &outputBuffer[0], (const char*) pCurrentText, Size);
				if (retailPort == PORT_9B1_ID || retailPort == PORT_9B2_ID) // if 9b 1 or 2
				{
					Status.Port9BControl.Status.Byte1.bReadDataInResponse = 1; // set data in response bit
				}
				controlChar = S_R_Count.Char.Raw;
				SendRetailFrame(outputBuffer);
				outputState = NO_MESSAGE;
				dataWasSent = TRUE;
				retVal = true;
			}
			else if (Size >= 244 && CanITransmit6x()) // Sending long barcodes or menu responses over Port 6x
			{
				if (tempBarcodePtr == NULL) // haven't started sending yet
				{
					tempBarcodePtr = pCurrentText;
					memcpy((char*) &outputBuffer[0],(const char*) tempBarcodePtr, 244);
					// Send the first 244 bytes
					tempBarcodePtr += 244;
					TxDataLength = 244;
					retVal = false;
				}
				else // already started sending - send the next chunk
				{
					leftToSend = Size - (tempBarcodePtr - pCurrentText);
					if (leftToSend > 244)
					{
						memcpy((char*) &outputBuffer[0],(const char*) tempBarcodePtr, 244);
						TxDataLength = 244;
						tempBarcodePtr += 244;
						retVal = false;
					}
					else // Sending the last chunk of data set pointer to null
					{
						memcpy((char*) &outputBuffer[0],(const char*) tempBarcodePtr, leftToSend);
						TxDataLength = leftToSend;
						tempBarcodePtr = NULL;
						retVal = true;
					}
				}
				controlChar = S_R_Count.Char.Raw;
				SendRetailFrame(outputBuffer);
				outputState = NO_MESSAGE;
				dataWasSent = TRUE;
			}

		}
		else //Menu Command RTLPDF enable
		{
			if (IBM4683SurePosMaxPacketSize >= (Size - 3) && ('r' == pBarcode->GetHHPId())) // 4680 system is limited to messages < 256 characters including the overhead.
			{
				TxDataLength = (unsigned char) Size;
				memcpy((char*) &outputBuffer[0], (const char*) pCurrentText, Size - 3);
				memcpy((char*) &outputBuffer[Size - 3], Code39SuffixHEX, 3); //add code 39 suffix
				if (retailPort == PORT_9B1_ID || retailPort == PORT_9B2_ID) // if 9b 1 or 2
				{
					Status.Port9BControl.Status.Byte1.bReadDataInResponse = 1; // set data in response bit
				}
				controlChar = S_R_Count.Char.Raw;
				SendRetailFrame(outputBuffer);
				outputState = NO_MESSAGE;
				dataWasSent = TRUE;
				retVal = true;
			}
			else if (((Size - 3) > IBM4683SurePosMaxPacketSize) && ('r' == pBarcode->GetHHPId())) // Sending long barcodes
			{
				if (startofsegment) // haven't started sending yet
				{
					PDFleftToSend = Size;
					memcpy((char*) &outputBuffer[0],(const char*) pCurrentText,	IBM4683SurePosMaxPacketSize);
					// Send the first XX bytes

					memcpy((char*) &outputBuffer[IBM4683SurePosMaxPacketSize], Code39SuffixHEX, 3); //add code 39 suffix
					TxDataLength = IBM4683SurePosMaxPacketSize + 3;
					retVal = false;
					startofsegment = false;
					PDFleftToSend = Size - IBM4683SurePosMaxPacketSize;
					dataWasSent = FALSE;
				}
				else // already started sending - send the next chunk
				{
					if ((PDFleftToSend - 3) > IBM4683SurePosMaxPacketSize)
					{
						pCurrentText = pCurrentText + (Size - PDFleftToSend);
						memcpy((char*) &outputBuffer[0],(const char*) pCurrentText, IBM4683SurePosMaxPacketSize);
						memcpy((char*) &outputBuffer[IBM4683SurePosMaxPacketSize], Code39SuffixHEX, 3); //add code 39 suffix
						TxDataLength = IBM4683SurePosMaxPacketSize + 3;
						PDFleftToSend = PDFleftToSend - IBM4683SurePosMaxPacketSize;
						dataWasSent = FALSE;
						retVal = false;
					}
					else // Sending the last chunk of data set pointer to null
					{
						pCurrentText = pCurrentText + (Size - PDFleftToSend);
						memcpy((char*) &outputBuffer[0],(const char*) pCurrentText, (PDFleftToSend - 3));
						memcpy((char*) &outputBuffer[PDFleftToSend - 3],Code39SuffixHEX, 3); //add code39 suffix at last
						TxDataLength = PDFleftToSend;
						startofsegment = true;
						PDFleftToSend = 0;
						dataWasSent = TRUE;
						retVal = true;
					}
				}
				controlChar = S_R_Count.Char.Raw;
				SendRetailFrame(outputBuffer);
				outputState = NO_MESSAGE;
			}
			else if ((244 > Size) && ('r' != pBarcode->GetHHPId())) // 4680 system is limited to messages < 256 characters including the overhead.
			{
				TxDataLength = (unsigned char) Size;
				memcpy((char*) &outputBuffer[0], (const char*) pCurrentText, Size);
				if (retailPort == PORT_9B1_ID || retailPort == PORT_9B2_ID) // if 9b 1 or 2
				{
					Status.Port9BControl.Status.Byte1.bReadDataInResponse = 1; // set data in response bit
				}
				controlChar = S_R_Count.Char.Raw;
				SendRetailFrame(outputBuffer);
				outputState = NO_MESSAGE;
				dataWasSent = TRUE;
				retVal = true;
			}
			else if (Size >= 244 && CanITransmit6x()) // Sending long barcodes or menu responses over Port 6x
			{
				if (tempBarcodePtr == NULL) // haven't started sending yet
				{
					tempBarcodePtr = pCurrentText;
					memcpy((char*) &outputBuffer[0],(const char*) tempBarcodePtr, 244);
					// Send the first 244 bytes
					tempBarcodePtr += 244;
					TxDataLength = 244;
					retVal = false;
				}
				else // already started sending - send the next chunk
				{
					leftToSend = Size - (tempBarcodePtr - pCurrentText);
					if (leftToSend > 244)
					{
						memcpy((char*) &outputBuffer[0],(const char*) tempBarcodePtr, 244);
						TxDataLength = 244;
						tempBarcodePtr += 244;
						retVal = false;
					}
					else // Sending the last chunk of data set pointer to null
					{
						memcpy((char*) &outputBuffer[0],(const char*) tempBarcodePtr, leftToSend);
						TxDataLength = leftToSend;
						tempBarcodePtr = NULL;
						retVal = true;
					}
				}
				controlChar = S_R_Count.Char.Raw;
				SendRetailFrame(outputBuffer);
				outputState = NO_MESSAGE;
				dataWasSent = TRUE;
			}
		}
	}
	return retVal;
}

