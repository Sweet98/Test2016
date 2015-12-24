//======================================================================================
// HModem.cpp
//======================================================================================
// $RCSfile: hmodem.cpp $
// $Revision: 1.4 $
// $Date: 2009/04/27 11:12:23EDT $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Class name:     HMODEM
//
//  Class description:
//
//      This class provides support for transmitting or receiving HMODEM wrapped
//      data from a remote device.  The instantiator of this class is referred to
//      as the initiator, since it decided to do an HModem transmit or receive.
//
//		Originally, all the logic to accomplish the transmit or receive resided in this
//		file, where the I/O functionality was provided by HCOMM and transfer
//		status reporting was provided by HTRANSFER_STATUS, but in order to make
//		integration easier for others, the bulk of the logic has been moved to
//		HModemTransmitReceive.cpp, which uses macros, to be defined by the
//		integrator, to accomplish I/O and transfer status reporting.  This file
//		will still be used to create a class wrapper, allowing us to easily support
//		multiple HMODEM sessions.  Also, we still use HCOMM and HTRANSFER_STATUS by
//		simply redefining the macros used by HModemTransmitReceive.cpp to point to
//		the corresponding HCOMM and HTRANSFER_STATUS functions.
//
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------



//#include <stdio.h>					// debug
//#include <string.h>					// debug
//#include "db_chrio.h"					//lmc   debug

#include	"Crc.h"
#include	"HModem.h"
#include	"HHPCommunicationConfig.h"




//---------------------------------------------------------------------------


// Incorporate the routines in HModemTransmitReceive as class members.
#define		INITIALIZE_HMODEM_SESSION				HMODEM::InitializeHModemSession
#define		TERMINATE_HMODEM_SESSION				HMODEM::TerminateHModemSession
#define		WAIT_FOR_ACK							HMODEM::WaitForACK
#define		TRANSMIT_PROCESSOR						HMODEM::TransmitProcessor
#define		RECEIVE_PROCESSOR						HMODEM::ReceiveProcessor

// Status message to be used by HModemTransmitReceive.
#define		STATUS_MESSAGE_COPY( msg )				StatusMessage->Copy( msg )

// I/O functions to be used by HModemTransmitReceive.
#define		READ( buffername, size, millisecondstosleep ) \
													Device->Read( buffername, size, millisecondstosleep)
#define		WRITE( buffername )						Device->Write( buffername )
#define		FLUSH_READ_BUFFER()						Device->FlushReadBuffer()
#define		FLUSH_WRITE_BUFFER()					Device->FlushWriteBuffer()
#define		BYTES_IN_WRITE_BUFFER()					Device->BytesInWriteBuffer()
#define		WAKEUP_DEVICE()							CommandHandler->WakeupDevice()


// Timing functions to be used by HModemTransmitReceive.
#define		ZERO_ELAPSED_TIME()						Device->ZeroElapsedTime()
#define		ELAPSED_TIME()							Device->ElapsedTime()
#define		SLEEP( sleeptimeinmilliseconds )        Device->HCommSleep( sleeptimeinmilliseconds );

// Status feedback functions to be used by HModemTransmitReceive.
#define		MAY_CONTINUE( NumberOfNewBytesReceived, TotalBytesToReceive ) \
													TransferStatus->MayContinue( NumberOfNewBytesReceived, TotalBytesToReceive )
#define		RECEIVED_PROTOCOL_CHARACTERS( buffername ) \
													TransferStatus->ReceivedProtocolCharacters( buffername )
#define		DOWNLOAD_STARTING_HANDLER()				TransferStatus->DownloadStartingHandler()
#define		SET_PROGRESS_LEVEL( NumberOfBytesSentSoFar ) \
													TransferStatus->SetProgressLevel( NumberOfBytesSentSoFar )
#define		DOWNLOAD_FINISH_HANDLER()				TransferStatus->DownloadFinishHandler()
#define		RETRY_WARNING( message )				TransferStatus->RetryWarning( message )


// Compression functions to be used by HModemTransmitReceive.
#define	COMPRESSION_SUPPORTED true
#define	COMPRESS_DATA( Data,CompressMode) CompressData(Data,CompressMode)
#define	DECOMPRESS_DATA( Data, CompressMode,BlockSmoothing,SzUncompressedData) DecompressData( Data,CompressMode, BlockSmoothing,SzUncompressedData)


//
//  Constructor for class HMODEM.
//      - Indicate that no transmit start and stop handlers have been defined
//          yet.
//      - By default, the maximum number of retries is 10.
//
HMODEM::HMODEM(HCOMM *HCommDevice, HTRANSFER_STATUS *InputTransferStatus, HCOMMAND_HANDLER *InputCommandHandler)
{
	Device = HCommDevice;
	TransferStatus = InputTransferStatus;
	CommandHandler = InputCommandHandler;

	InitializeHModemSession();

	StatusMessage = new HSTRING(160L, true);

	return;
}
//---------------------------------------------------------------------------;





//
//  Destructor for class HMODEM.
//      - Free memory associated with the error message.
//
HMODEM::~HMODEM(void)
{
	delete StatusMessage;

	return;
}
//---------------------------------------------------------------------------;






//
//  SetMaximumNumberOfRetries
//      Set the maximum number of retries to a caller specified value.  This retry
//      limit applies to all HModem operations that may be retried.
//
void HMODEM::SetMaximumNumberOfRetries(unsigned int NewMaximumNumberOfRetries)
{
	MaximumNumberOfRetries = NewMaximumNumberOfRetries;
}
//---------------------------------------------------------------------------;






//
//  SetInitialResponseWaitPeriod
//      Set the number of seconds to wait for an initial response from the other side.
//
void HMODEM::SetInitialResponseWaitPeriod(unsigned int NewInitialResponseWaitPeriod)
{
	SecondsToWaitForInitialResponse = NewInitialResponseWaitPeriod;
}
//---------------------------------------------------------------------------;






//
//  SetReceiveTimeout
//      Set the number of milliseconds to wait for data from the remote HMODEM device
//
void HMODEM::SetReceiveTimeout(unsigned int NewReceiveTimeout)
{
	ReceiveTimeout = NewReceiveTimeout;

	return;
}
//---------------------------------------------------------------------------;






//
//  SetReceiveTimeout
//      Set receive timeout based on baud rate.
//
void HMODEM::SetReceiveTimeoutByBaudRate(unsigned int BaudRate)
{
	ReceiveTimeout = (115200 * RECEIVE_TIMEOUT) / BaudRate;
	if(ReceiveTimeout < RECEIVE_TIMEOUT)
		ReceiveTimeout = RECEIVE_TIMEOUT;
	if(ReceiveTimeout > MAXIMUM_RECEIVE_TIMEOUT)
		ReceiveTimeout = MAXIMUM_RECEIVE_TIMEOUT;

	return;
}
//---------------------------------------------------------------------------;






//
//  SetWaitForPacketACKTimeout
//		Set time to wait for acknowledgement of a packet.
//
void HMODEM::SetWaitForPacketACKTimeout(unsigned int NewWaitForPacketACKTimeout)
{
	WaitForPacketACKTimeout = NewWaitForPacketACKTimeout;

	return;
}
//---------------------------------------------------------------------------;






//
//  SetWaitForFinalACKTimeout
//      Set time to wait for a final ACK response at the end of the transfer.
//
void HMODEM::SetWaitForFinalACKTimeout(unsigned int NewWaitForFinalACKTimeout)
{
	WaitForFinalACKTimeout = NewWaitForFinalACKTimeout;

	return;
}
//---------------------------------------------------------------------------;




// Include the actual HModem routines in this source.  Because of the macros defined above for each
// of the routines, they will be incorporated here as class member functions.
#include	"HModemTransmitReceive.cpp"





//
//	Transmit (overloaded version #1)
//		Request transmit of data to a remote receiver via HModem protocol using HSTRING input.
//
unsigned int HMODEM::Transmit(HSTRING *Data, HSTRING *InitiatingCommand, bool WaitForSecondACK)
{
	return (TransmitProcessor(Data->Char, Data->Size, InitiatingCommand->Char, InitiatingCommand->Size, WaitForSecondACK, true, NO_COMPRESSION));
}
//---------------------------------------------------------------------------;





//
//	Transmit (overloaded version #2)
//		Request transmit of data to a remote receiver via HModem protocol using unsigned char* input.
//
unsigned int HMODEM::Transmit(unsigned char *Data, unsigned int DataLength, HSTRING *InitiatingCommand, bool WaitForSecondACK)
{
	return (TransmitProcessor(Data, DataLength, InitiatingCommand->Char, InitiatingCommand->Size, WaitForSecondACK, true, NO_COMPRESSION));
}
//---------------------------------------------------------------------------;





//
//	Transmit (overloaded version #3)
//		Request transmit of data to a remote receiver via HModem protocol using HSTRING input, sending the
//		buffer size as part of the transfer based on caller preference.
//
unsigned int HMODEM::Transmit(HSTRING *Data, HSTRING *InitiatingCommand, bool WaitForSecondACK, bool TransmitHeader)
{
	return (TransmitProcessor(Data->Char, Data->Size, InitiatingCommand->Char, InitiatingCommand->Size, WaitForSecondACK, TransmitHeader, NO_COMPRESSION));
}
//---------------------------------------------------------------------------;





//
//	Transmit (overloaded version #4)
//		Request transmit of data to a remote receiver via HModem protocol using unsigned char* input, sending the
//		buffer size as part of the transfer based on caller preference.
//
unsigned int HMODEM::Transmit(unsigned char *Data, unsigned int DataLength, HSTRING *InitiatingCommand, bool WaitForSecondACK,
                              bool TransmitHeader)
{
	return (TransmitProcessor(Data, DataLength, InitiatingCommand->Char, InitiatingCommand->Size, WaitForSecondACK, TransmitHeader, NO_COMPRESSION));
}
//---------------------------------------------------------------------------;





//
//	Transmit (overloaded version #5)
//		Request transmit of data to a remote receiver via HModem protocol using HSTRING input and
//      caller requested compression mode.
//
unsigned int HMODEM::Transmit(HSTRING *Data, HSTRING *InitiatingCommand, bool WaitForSecondACK, unsigned int CompressionMode)
{
	return (TransmitProcessor(Data->Char, Data->Size, InitiatingCommand->Char, InitiatingCommand->Size, WaitForSecondACK, true, CompressionMode));
}
//---------------------------------------------------------------------------;





//
//	Transmit (overloaded version #6)
//		Request transmit of data to a remote receiver via HModem protocol using unsigned char* input and
//      caller requested compression mode.
//
unsigned int HMODEM::Transmit(unsigned char *Data, unsigned int DataLength, HSTRING *InitiatingCommand, bool WaitForSecondACK, unsigned int CompressionMode)
{
	return (TransmitProcessor(Data, DataLength, InitiatingCommand->Char, InitiatingCommand->Size, WaitForSecondACK, true, CompressionMode));
}
//---------------------------------------------------------------------------;





//
//	Transmit (overloaded version #7)
//		Request transmit of data to a remote receiver via HModem protocol using HSTRING input, sending the
//		buffer size as part of the transfer based on caller preference and using caller requested compression mode.
//
unsigned int HMODEM::Transmit(HSTRING *Data, HSTRING *InitiatingCommand, bool WaitForSecondACK, bool TransmitHeader,
                              unsigned int CompressionMode)
{
	//char	MsgBuf[80];                                           //lmc   debug
	//sprintf (MsgBuf," Transmit DataSize:%d CommandSize:%d Header:%u CompMode:%d \r\n", Data->Size, InitiatingCommand->Size, TransmitHeader, CompressionMode ); //lmc   debug
	//xputstring (MsgBuf);                                        //lmc   debug

	return (TransmitProcessor(Data->Char, Data->Size, InitiatingCommand->Char, InitiatingCommand->Size, WaitForSecondACK, TransmitHeader, CompressionMode));
}
//---------------------------------------------------------------------------;





//
//	Transmit (overloaded version #8)
//		Request transmit of data to a remote receiver via HModem protocol using unsigned char* input, sending the
//		buffer size as part of the transfer based on caller preference and using caller requested compression mode.
//
unsigned int HMODEM::Transmit(unsigned char *Data, unsigned int DataLength, HSTRING *InitiatingCommand, bool WaitForSecondACK,
                              bool TransmitHeader, unsigned int CompressionMode)
{
	return (TransmitProcessor(Data, DataLength, InitiatingCommand->Char, InitiatingCommand->Size, WaitForSecondACK, TransmitHeader, CompressionMode));
}
//---------------------------------------------------------------------------;





//
//	Receive (overloaded version #1)
//		Request receive data from a remote transmitter via HModem protocol using return data callback.
//
unsigned int HMODEM::Receive(unsigned int MaximumNumberOfBytesToReceive,
                             bool (*ReceivedDataCharacters) (HSTRING *))
{
	return (ReceiveProcessor(NULL, NULL, NULL, MaximumNumberOfBytesToReceive, ReceivedDataCharacters));
}
//---------------------------------------------------------------------------;






//
//	Receive (overloaded version #2)
//		Request receive data from a remote transmitter via HModem protocol using an HSTRING return buffer.
//
unsigned int HMODEM::Receive(HSTRING *ReceiveBuffer, unsigned int MaximumNumberOfBytesToReceive)
{
	return (ReceiveProcessor(NULL, NULL, ReceiveBuffer, MaximumNumberOfBytesToReceive, NULL));
}
//---------------------------------------------------------------------------;





//
//	Receive (overloaded version #3)
//		Request receive data from a remote transmitter via HModem protocol using a unsigned char* return buffer.
//
unsigned int HMODEM::Receive(unsigned char *ReceiveBuffer, unsigned int *NumberOfBytesInReceiveBuffer, unsigned int MaximumNumberOfBytesToReceive)
{
	return (ReceiveProcessor(ReceiveBuffer, NumberOfBytesInReceiveBuffer, NULL, MaximumNumberOfBytesToReceive, NULL));
}
//---------------------------------------------------------------------------;






