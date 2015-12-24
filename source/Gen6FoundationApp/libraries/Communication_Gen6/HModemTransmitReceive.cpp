//======================================================================================
// HModemTransmitReceive.cpp
//======================================================================================
// $RCSfile: HModemTransmitReceive.cpp $
// $Revision: 1.2 $
// $Date: 2009/04/24 17:28:09EDT $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Module name:     HMODEM transmit receive
//
//  Module description:
//
//      This module provides support for transmitting or receiving HMODEM wrapped
//      data from a remote device.  It is assumed that the communication to the
//      remote device is RS232, but this is not strictly necessary, since the
//		  integrator supplies input / output capabilities via the macros defined below,
//		  thereby relieving HMODEM of any knowledge about the actual communication medium.
//
//      HModem transmit will send HSTRING data specified by the initiator to the
//      remote device, first sending initiator specified HSTRING command text to
//      the remote device to tell them that we want to do an HModem transmit.  If
//      the command text is null, no command will be sent.  The initiator may request
//      that, upon completion, transmit waits for a second ACK before returning, the
//      first ACK having been sent in response to us sending an EOT to signal that
//      transmission is completed.  The purpose of this second ACK is for when the
//      remote device, having received all the data, needs to perform some action
//      and then inform us, via an ACK, that the action is completed.  An example would
//		  be in embedded devices that send a second ACK after successfully flashing the
//		  downloaded data.
//
//      HModem receive will read HModem wrapped data from a remote device, not to
//      exceed the number of characters specified by the initiator.  There are two basic
//		  methods of receiving: (1) receiving the entire data stream before making it
//		  available to the integrator and (2) making each packet available.  If using the
//		  second method, the initiator must also supply a callback that will return data as
//		  each packet is received.  The reason that the second receive method is used is that
//		  it allows the integrator to limit the amount of memory allocated since data can
//      be processed and disposed of on the fly.
//
//      Both transmit and receive require the following capabilities via I/O function
//		  overloading:
//          - A means of zeroing and reading an elapsed timer with one second
//              granularity, in order to see if we've waited too long for data
//              or for an acknowledgement.
//          - A means of flushing the communications device read buffer when we've
//              received bad data, when we're starting a transfer, and when we're
//              ready to exit.
//          - A means of flushing the communications device write buffer when the
//              remote device has told us to quit sending.  This flush operation
//              will probably have no impact at higher baud rates, but at slow
//              (such as 300) baud rates, the ability to flush data prevents data
//              from trickling in to the remote device over a long (up to 2 minute)
//              period.
//          - A means of receiving data from and sending data to the communications
//              device.  The read function also includes a timeout capability,
//              expressed in milliseconds.  If we set the timeout to zero, then
//              the function must wait indefinitely for data to be received, up to
//              the maximum we specify; otherwise, the read function should return
//              when the timeout has been reached.
//
//      Both transmit and receive will use the following capabilities via I/O function
//		  overloading if supplied by the initiator:
//          - A method of sleeping (remaining idle) for some time period, where the
//              granularity is 1 millisecond.  This function is not required, because
//              it is used to catch extraneous transmitted on error or exit conditions.
//          - A means of determining if the initiator wants us to cancel the transfer.
//              We will supply the initiator with a progress count on the number of
//              bytes received so far and if we are able, we'll also tell them
//              the maximum amount of data to be transferred.
//
//      When no data or incorrect data is received, packet receive and transmit operations
//      are retried up to the maximum configured number of retries.  By default, this
//      maximum is ten, but can be changed by the initiator.
//
//      Transmit will use the following capabilities via I/O function overloading if
//		  supplied by the initiator:
//          - A handler to be called when transmission has been initiated and a
//              handler to be called when the remote device has been informed of
//              transfer completion.  The primary purpose of these functions is
//              anticipated to be that the HModem transfer initiator will know
//              exactly when to ignore data coming over the communication link,
//              because it is part of an HModem transfer.
//
//      Receive will use the following capabilities via I/O function overloading if
//		  supplied by the initiator:
//          - A handler to be called whenever an data or protocol characters are
//              received.  In this way, the initiator can look at all received
//              characters for diagnostic purposes and to discern that data
//              is still being received.
//          - A handler to be called when a retry is being attempted.  The handler
//              is supplied with a text string describing the retry condition.
//
//
//	Class integration:
//		This code was originally included in a class called HMODEM.  The purpose of
//		including it in a class was to allow multiple HMODEM session to be operating
//		simultaneously in one program, each with it's own workspace.  This module
//		can still be incorporated into a class wrapper by including this file within
//		the C++ class source and including the associated header file within the
//		class definition header file.  The routines used in this code are defined
//		with macro names, so that the names can be redefined to be class member
//		names.
//
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------



#ifndef HMODEMTRANSMITRECEIVECPP
#define HMODEMTRANSMITRECEIVECPP
//---------------------------------------------------------------------------


#include	<stdio.h>
#include	<string.h>


// Indicate that this is HMODEM code source, so that we get all the session variable definitions in the include file.
#define		HMODEM_TRANSMIT_RECEIVE_INCLUDE_VARIABLE_DEFINITIONS		1


#include	"Crc.h"
#include	"HHPCommunicationConfig.h"
#include	"HModemTransmitReceive.h"


//
//	The following are wrappers for the HMODEM functions.  These may be defined so that the routine is part of a class and / or
//	so that the calling convention can be forced.
//
#ifndef		INITIALIZE_HMODEM_SESSION
#define		INITIALIZE_HMODEM_SESSION				InitializeHModemSession
#endif

#ifndef		TERMINATE_HMODEM_SESSION
#define		TERMINATE_HMODEM_SESSION				TerminateHModemSession
#endif

#ifndef		WAIT_FOR_ACK
#define		WAIT_FOR_ACK							WaitForACK
#endif

#ifndef		TRANSMIT_PROCESSOR
#define		TRANSMIT_PROCESSOR					TransmitProcessor
#endif

#ifndef		RECEIVE_PROCESSOR
#define		RECEIVE_PROCESSOR						ReceiveProcessor
#endif



//
//	The following are macros for functions that HMODEM uses to accomplish I/O and reporting.  Some are optional and some are
//	mandatory.
//

//
//	Optional function to store a character string status message for debugging / tracing purposes.
//
#ifndef		STATUS_MESSAGE_COPY
#define		STATUS_MESSAGE_COPY( msg )
#endif



//
//	Mandatory read function that takes an HSTRING buffer, and integer maximum size to read, and milliseconds to wait
//	for data.  It returns boolean true unless the read failed because of a hardware error or because the communication
//	port is not available.  Failure to read anything before the timeout expires is not an error.
//
#ifndef		READ
#define		READ( buffer, maximumsize, millisecondstowait )
#endif


//
//	Mandatory write function that writes the contents of an HSTRING buffer to the communication port.  Boolean true is
//	returned if the write succeeded, false otherwise.
//
#ifndef		WRITE
#define		WRITE( buffer )
#endif


//
//	Mandatory flush read buffer queue function that clears all input data from the communication port input queue.
//	Boolean true if no errors occurred, false otherwise.
//
#ifndef		FLUSH_READ_BUFFER
#define		FLUSH_READ_BUFFER()
#endif


//
//	Optional flush write buffer queue function that clears all pending output data from the communication port output queue.
//	Boolean true if no errors occurred, false otherwise.  This function is not mandatory, but helps speed up error recovery
//	during packet transmission.
//
#ifndef		FLUSH_WRITE_BUFFER
#define		FLUSH_WRITE_BUFFER()
#endif


//
//	Optional function to return the number of bytes in the communication port output queue.  This function is not mandatory,
//	but helps speed up error recovery during packet transmission.
//
#ifndef		BYTES_IN_WRITE_BUFFER
#define		BYTES_IN_WRITE_BUFFER()					0
#endif


//
//	Optional function to send a wakeup command to the device before starting transmission.  Currently unused, but may be
//	applicable to future products.
//
#ifndef		WAKEUP_DEVICE
#define		WAKEUP_DEVICE()
#endif


//
//	ZERO_ELAPSED_TIME and ELAPSED_TIME are mandatory fuctions to allow HMODEM routines to keep track of elapsed time during
//	transfers.  The granularity is in seconds.  ZERO_ELAPSED_TIME must reset an elapsed time counter, and ELAPSED_TIME
//	returns the amount of time elapsed, in seconds, since the last call to ZERO_ELAPSED_TIME.
//
#ifndef		ZERO_ELAPSED_TIME
#define		ZERO_ELAPSED_TIME()
#endif

#ifndef		ELAPSED_TIME
#define		ELAPSED_TIME()							0
#endif


//
//	Optional sleep function that allows HMODEM error recovery routines to sleep for the specified number of milliseconds.
//	This routine is not required, but is strongly recommended to provide timely error recovery.
//
#ifndef		SLEEP
#define		SLEEP( sleeptimeinmilliseconds )
#endif


//
//	Optional function for embedded applications that are operating with a watchdog timer.  This routine is called by HMODEM
//	to indicate that it is still processing and is not hung.
#ifndef		RESET_WATCHDOG_TIMER
#define		RESET_WATCHDOG_TIMER()
#endif


//
//	Optional callback function that HMODEM transmit and receive routines call to let the application know when new data has
//	been received and / or to check if it is OK to continue.  If the transfer process should be discontinued, return false,
//	else return true to allow the transfer to continue.
#ifndef		MAY_CONTINUE
#define		MAY_CONTINUE( NumberOfNewBytesReceived, TotalBytesToReceive )	true
#endif


//
//	Optional callback function the HMODEM uses to report all characters received from the host.  Useful only for diagnostic
//	purposes to see message traffic.
//
#ifndef		RECEIVED_PROTOCOL_CHARACTERS
#define		RECEIVED_PROTOCOL_CHARACTERS( buffername )
#endif


//
//	Optional callback used by HMODEM to tell the application that it is about to start a transfer, in case the application needs
//	to do something such as updating a status information box.
//
#ifndef		DOWNLOAD_STARTING_HANDLER
#define		DOWNLOAD_STARTING_HANDLER()
#endif


//
//	Optional callback used by HMODEM to tell the application how much data it has received so far.  This is similar to
//	MAY_CONTINUE, but differs in that it specifies the actual number of data bytes received so far, instead of how many new
//	bytes have been received since the last call to MAY_CONTINUE.  If doing progress tracking, this function should be
//	implemented, because if HMODEM has to back up a packet during error recovery, the amount of data received so far will be
//	adjusted downward.
//
#ifndef		SET_PROGRESS_LEVEL
#define		SET_PROGRESS_LEVEL( NumberOfBytesToBeTransmitted )
#endif


//
//	Optional callback used by HMODEM to tell the application that it is about to end a transfer, in case the application needs
//	to do something such as updating a status information box.
//
#ifndef		DOWNLOAD_FINISH_HANDLER
#define		DOWNLOAD_FINISH_HANDLER()
#endif


//
//	Optional callback function the HMODEM uses to report retries by sending an HSTRING status message.  Useful only for
//	diagnostic purposes.
//
#ifndef		RETRY_WARNING
#define		RETRY_WARNING( message )
#endif


//
//	Indicate whether or not compression routines, defined below, exist.  Leave false for now.
//
#ifndef		COMPRESSION_SUPPORTED
#define		COMPRESSION_SUPPORTED					false
#endif

#ifndef		COMPRESS_DATA
#define		COMPRESS_DATA( Data, CompressionMode )	false
#endif

#ifndef		DECOMPRESS_DATA
#define		DECOMPRESS_DATA( Data, CompressionMode, BlockSmoothing, SizeOfUncompressedData )	false
#endif



//
//  INITIALIZE_HMODEM_SESSION
//      Setup HMODEM defaults.
//
void INITIALIZE_HMODEM_SESSION( void )
{
	MaximumNumberOfRetries = DEFAULT_MAXIMUM_NUMBER_OF_RETRIES;

	SecondsToWaitForInitialResponse = DEFAULT_INITIAL_RESPONSE_TIME;

	ReceiveTimeout = RECEIVE_TIMEOUT;

	TransmitPacketAhead = true;
	CancelTransmitPacketAheadOnRetry = false;
	Transmit128BytePacketsOnly = false;
	SwitchTo128BytePacketsOnRetry = false;

	WaitForPacketACKTimeout = DEFAULT_WAIT_FOR_PACKET_ACK;

	WaitForFinalACKTimeout = DEFAULT_WAIT_FOR_FINAL_ACK_TIMEOUT;

	NumberOfBytesToBeSentByTransmitter = 0;		// Don't know how much data to expect yet.

	return;
}




void TERMINATE_HMODEM_SESSION(void)
{
	HSTRING             *Buffer;                // Buffer to write ESC from.

	Buffer = new HSTRING(150);

	FLUSH_READ_BUFFER();
	FLUSH_WRITE_BUFFER();

	do
	{
		RESET_WATCHDOG_TIMER();
		READ( Buffer, 1, SLEEP_AFTER_TERMINATE );
	}
	while(Buffer->Size != 0);

	READ( Buffer, 1, SLEEP_AFTER_TERMINATE );

	// Write ESC to tell the host that we're quitting.  Write it twice so that receiver knows that
	// this is really an ESC and not a transmission error.  Clear the NAK that the receiver will send
	// in between the two writes.  See ReceiveProcessor ESC handling for more information.
	Buffer->CopyChar(ESC);
	WRITE(Buffer);
	READ( Buffer, 1, ReceiveTimeout );
	Buffer->CopyChar(ESC);
	WRITE(Buffer);

	delete Buffer;

	return;
}



//
//	WAIT_FOR_ACK
//
unsigned int WAIT_FOR_ACK(unsigned int NumberOfSecondsToWait, unsigned int NumberOfBytesBeingTransmitted)
{
	bool						ACKReceived=false;			// Indicates that receiver ACKed.
	bool                	NAKReceived=false;			// Indicates that receiver NAKed.
	bool                	ESCReceived=false;			// Indicates that receiver wants to ESC.
	bool                	UserHalt=false;				// Indicates that user stopped download.
	unsigned int			ReturnStatus=0;				// Indicates what the terminating action was.
	HSTRING             *InputBuffer;					// Buffer to receive ACK

	ZERO_ELAPSED_TIME();

	InputBuffer = new HSTRING(100);

	do
	{
		RESET_WATCHDOG_TIMER();
		READ(InputBuffer, 1, ReceiveTimeout);
		RECEIVED_PROTOCOL_CHARACTERS( InputBuffer );

		// If the receiver ACKed, keep going if this isn't the last packet.  Otherwise...
		if(InputBuffer->RemoveChar(ACK))
		{
			ACKReceived = true;
			ReturnStatus = ACK_RECEIVED;
		}

		// If the receiver NAKed, notify the user and stop the current download request.
		else if(InputBuffer->RemoveChar(NAK))
		{
			NAKReceived = true;
			ReturnStatus = NAK_RECEIVED;
			FLUSH_WRITE_BUFFER();
		}

		// If the receiver wants to terminate (ESC), notify the user and stop the current
		// download request.
		else if(InputBuffer->RemoveChar(ESC))
		{
			ESCReceived = true;
			ReturnStatus = TERMINATE_RECEIVED;
			FLUSH_WRITE_BUFFER();
		}

		// If there are still bytes to be written, zero the elapsed time.
		if( BYTES_IN_WRITE_BUFFER() )
		{
			ZERO_ELAPSED_TIME();
		}

		// Tell the initiator that we're still going and see if it is OK to continue.
		if(!MAY_CONTINUE( 0, NumberOfBytesBeingTransmitted ))
		{
			UserHalt = true;
			ReturnStatus = TERMINATE_BY_US;
			TERMINATE_HMODEM_SESSION();
		}
	}
	while( (!ESCReceived) && (!UserHalt) && (!ACKReceived) && (!NAKReceived) &&
	        (ELAPSED_TIME() < (int) NumberOfSecondsToWait) );

	if(ELAPSED_TIME() >= (int) NumberOfSecondsToWait)
	{
		ReturnStatus = TIMEOUT_WAITING_FOR_ACK;
	}

	delete InputBuffer;

	return(ReturnStatus);
}



//
//  TRANSMIT_PROCESSOR
//      Purpose:
//          To transmit data, sent by the caller, to a receiver via HModem protocol.
//
//      Main loop logic:
//          Send the download command, waiting for a 'C'.  If received...
//              While there are bytes remaining to be transferred and the
//              download process hasn't been terminated...
//                  Build the next packet, padding the packet with zeroes
//                  if this is the last packet.  The length is normally
//                  1024, but the last packet can be 128 bytes in
//                  length if there are 128 or less bytes remaining.  The
//                  block consists of a block size, block number, data, and CRC.
//
//                  Send the block and wait for a reply.  If an ACK is
//                  not received, terminate the download.  Otherwise...
//                      If there is more data to process, continue.
//                      Otherwise, send a finish character to the
//                      scanner and wait for an ACK.  If we're supposed to do
//                      so, wait for a second ACK before
//                      continuing.  Note that a single port
//                      read may contain both ACKs.
//
//
unsigned int TRANSMIT_PROCESSOR(unsigned char *ByteData, unsigned int ByteDataLength, unsigned char *InitiatingCommand,
                                unsigned int InitiatingCommandSize,
                                bool WaitForSecondACK, bool TransmitHeader, unsigned int CompressionMode)
{
	bool                ReceiverReady;								// Flag indicating that receiver responded to
																						//  the download request and is ready to receive.
	bool                FatalDownloadError;     					// Fatal error occurred while trying to send
																						//  data to the receiver.
	HSTRING					*InputBuffer;								// Read buffer for processing data
																						//  sent by the receiver.
	HSTRING					*OutputBuffer;								// Write buffer, for writing data
																						//  packets to the receiver.
	unsigned int				BlockNumToSend;         			// Next packet to be sent to the receiver.
	unsigned int				BlockNumToBeACKed;      			// Next packet to be ACKed by the receiver.
	unsigned int				crc;                    			// Computed CRC of a packet to be sent to
																						//  the receiver.
	unsigned int				NumberOfRetries;        			// Number of packet send retries.
	unsigned int				BytesRemainingToBeSent; 			// Number of file bytes remaining to be
																						//  sent to the receiver.
	unsigned int				BytesRemainingToBeACKed;			// Number of file bytes remaining to be
																						//  ACKed by the receiver, including those bytes not yet sent.
	//	unsigned int				i;                      		// Indices
	unsigned int				j;
	unsigned int				OffsetOfNextFileByteToBeSent;   // Offset of next byte in data being transmitted
																						//	to the receiver as part of the next packet.
	unsigned int				OffsetOfNextFileByteToBeACKed;  // Offset of next packet in data that has yet to be ACKed.
	unsigned int				NumberOfBytesToBeTransmitted;   // Total number of data bytes to be transmitted
																						//  to the receiver.  This can be larger than
																						//  the actual file size, because this number
																						//  includes the zero pad bytes at the end
																						//  of the last packet.
	bool                DownloadTerminatedByReceiver;   		// Indicates that the receiver has sent an ESC
																						//  to request download termination.
	unsigned int				ReturnStatus;							// Status of download operation.
	HSTRING						*Data;									// Pointer to HSTRING representation of data sent by the caller.  If the
																						//	data doesn't need to be compressed, then this instance is simply a
																						//	reference to the input buffer; otherwise, it is a copy of the input
																						//	buffer, since we don't want to alter the data sent by the caller.
	unsigned int				NumberOfDataBytesInNextPacket=0;	// Number of bytes of actual data being transmitted in the next
																						//	packet.
	unsigned int				ACKWaitStatus;          			// Status returned from ACK wait routine
	unsigned int				WaitStatus;								//
	bool				TransmitPacketAheadDuringThisSession = TransmitPacketAhead;		// Transmit the next packet while waiting for
																													//	the current packet if this is the default.
																													//	Note that this will get turned off if the
																													//	receiver speaks minimal XModem.
	bool				XModemOnly = false;								// The receiver only recognizes XModem.
	unsigned int	FeatureFlag = SUPPORT_OF_DOUBLE_EOT;		// Defines features supported by this version of HModem.
	bool				Transmit128BytePacketMode = Transmit128BytePacketsOnly;	// We'll configure 128 byte packet mode
																											//	based for the moment if we're forced
																											//	to, but we may revert to it anyway
																											//	if certain errors occur.
	unsigned char		*OriginalData;								// Pointer to original data.
	int					SizeOfOriginalData;						// Size of original data, before compression.


	STATUS_MESSAGE_COPY("");	// No status message yet.


	// Set up pointer to data and size of data.
	if(ByteData != 0)
	{
		OriginalData = ByteData;
		SizeOfOriginalData = ByteDataLength;
	}
	else
	{
		return(BAD_INPUT_DATA_POINTER);
	}

	// If compression type is invalid, return an error.
	if( !COMPRESSION_SUPPORTED )
	{
		CompressionMode = NO_COMPRESSION;
	}

	if(CompressionMode >= INVALID_COMPRESSION_MODE)
	{
		ReturnStatus = UNRECOGNIZED_COMPRESSION_MODE;
		STATUS_MESSAGE_COPY("Unrecognized HModem compression mode");
		return(ReturnStatus);
	}

	// If no compression, create a reference to the original data.  Otherwise, create a copy so that the copy can be compressed.
	if( CompressionMode == NO_COMPRESSION )
		Data = new HSTRING( OriginalData, SizeOfOriginalData, false, true, true );
	else
		Data = new HSTRING( OriginalData, SizeOfOriginalData, true, false, false );

	RESET_WATCHDOG_TIMER();

	// Allocate string space.
	InputBuffer = new HSTRING(1100);
	OutputBuffer = new HSTRING(1100);

	// No errors have occurred yet as part of the download process.
	FatalDownloadError = false;
	DownloadTerminatedByReceiver = false;
	ReturnStatus = FINISH_SUCCESSFUL;                 // No errors yet.

	// If we're cleared to transmit a header, compress the data.  Otherwise, leave it uncompressed, because
	// we won't be able to tell the receiver what type of compression was used.  The reason that we compress
	// here is that we don't want to take a long time after issuing the command to the reader, else the reader
	// will time out.
	if(TransmitHeader)
	{
		// Compress the image
		if( CompressionMode != NO_COMPRESSION )
		{
			bool	CompressionStatus = COMPRESS_DATA( Data, CompressionMode );

			// If the compressed data size is larger than the uncompressed size, leave the data uncompressed
			if(( !CompressionStatus ) || ( Data->Size >= (unsigned int) SizeOfOriginalData ))
			{
				Data->Copy( OriginalData, SizeOfOriginalData );
				CompressionMode = NO_COMPRESSION;
			}
		}
	}

	RESET_WATCHDOG_TIMER();

	// Wake up the device, clear the read buffer, and send the command
	WAKEUP_DEVICE();
	FLUSH_READ_BUFFER();
	HSTRING		strInitiatingCommand( InitiatingCommand, InitiatingCommandSize, false );
	WRITE( &strInitiatingCommand );

	// Start the timer and wait for the initial response.
	ZERO_ELAPSED_TIME();
	ReceiverReady = false;

	// Even though this will be adjusted to be on a packet boundary, set it for now so that
	// it may be properly reported by the initiator.
	NumberOfBytesToBeTransmitted = SizeOfOriginalData;


	// Wait for a response from the receiver.  'C' means that the receiver is
	// ready to receive data and ESC means that the receiver wants to quit.
	// Any other condition is an error.
	while((ELAPSED_TIME() < (int) SecondsToWaitForInitialResponse) && (!ReceiverReady) && (!DownloadTerminatedByReceiver) &&
	        (!FatalDownloadError))
	{
		RESET_WATCHDOG_TIMER();
		//        READ(InputBuffer, 80, ReceiveTimeout);
		READ(InputBuffer, 1, ReceiveTimeout);
		RECEIVED_PROTOCOL_CHARACTERS(InputBuffer);
		//        for(i=0; i<InputBuffer->Size; ++i)
		if( InputBuffer->Size == 1)
		{
			//            if(InputBuffer->Char[i] == 'C')
			if(InputBuffer->Char[0] == 'C')
			{
				ReceiverReady = true;
				break;
			}
			//            else if(InputBuffer->Char[i] == ESC)
			else if(InputBuffer->Char[0] == ESC)
			{
				ReturnStatus = TERMINATE_RECEIVED;
				STATUS_MESSAGE_COPY("Receiver terminated HMODEM");
				DownloadTerminatedByReceiver = true;
				FatalDownloadError = true;
				FLUSH_WRITE_BUFFER();
				break;
			}
		}

		if(!MAY_CONTINUE(0, NumberOfBytesToBeTransmitted))
		{
			FatalDownloadError = true;
			ReturnStatus = TERMINATE_BY_US;
			STATUS_MESSAGE_COPY("We (transmitter) terminated HMODEM");
			TerminateHModemSession();
		}
	}

	// If the receiver said it's ready to receive, start sending packets.
	if(ReceiverReady)
	{
		DOWNLOAD_STARTING_HANDLER();

		// Determine the total number of bytes to be transmitted, depending
		// on whether the last packet can be 128 bytes in length or not.
		if(Transmit128BytePacketMode)
		{
			switch( Data->Size % 128)
			{
			case 0:
				NumberOfBytesToBeTransmitted = Data->Size;
				break;

			default:
				NumberOfBytesToBeTransmitted = (Data->Size & 0xFFFFFF80) + 128;
				break;
			}
		}
		else
		{
			switch( Data->Size % 1024)
			{
			case 128:
			case 0:
				NumberOfBytesToBeTransmitted = Data->Size;
				break;

			default:
				NumberOfBytesToBeTransmitted = ((Data->Size % 1024) < 128) ?
				                               (Data->Size & 0xFFFFFF80) + 128 : (Data->Size & 0xFFFFFC00) + 1024;
				break;
			}
		}

		// If we're cleared to transmit a header, build a header and write it.  Note that this is
		// compatible with previous HMODEM revisions; the receive processor won't know what to do with this data
		// and will NAK the header block.  We'll then get ready for an uncompressed send and recalculate transfer
		// sizes.
		if(TransmitHeader)
		{
			// Build a 128 byte header to send to the receiver
			OutputBuffer->CopyChar(HEADER_BLOCK_128_BYTE);    // Size of header block is 128

			// Write the buffer
			FLUSH_READ_BUFFER();
			WRITE(OutputBuffer);

			RESET_WATCHDOG_TIMER();

			// Now, we'll wait for an ACK.  If we don't get one, we're going to assume that the receiver can't
			// handle a header or our compression request, which will mean that we'll have to transmit uncompressed data.
			WaitStatus = WaitForACK(WaitForPacketACKTimeout, NumberOfBytesToBeTransmitted);

			if(WaitStatus == TERMINATE_RECEIVED)
			{
				DownloadTerminatedByReceiver = true;
				ReturnStatus = TERMINATE_RECEIVED;
				STATUS_MESSAGE_COPY("Receiver terminated HMODEM");
			}
			else if(WaitStatus != ACK_RECEIVED)
			{
				//        	else if(WaitForACK(WaitForPacketACKTimeout, NumberOfBytesToBeTransmitted) != ACK_RECEIVED)
				//          {
				if( CompressionMode != NO_COMPRESSION )
				{
					Data->Copy( OriginalData, SizeOfOriginalData );
				}

				// Determine the total number of bytes to be transmitted, depending
				// on whether the last packet can be 128 bytes in length or not.
				if(Transmit128BytePacketMode)
				{
					switch( Data->Size % 128 )
					{
					case 0:
						NumberOfBytesToBeTransmitted = Data->Size;
						break;

					default:
						NumberOfBytesToBeTransmitted = (Data->Size & 0xFFFFFF80) + 128;
						break;
					}
				}
				else
				{
					switch( Data->Size % 1024 )
					{
					case 128:
					case 0:
						NumberOfBytesToBeTransmitted = Data->Size;
						break;

					default:
						NumberOfBytesToBeTransmitted = ((Data->Size % 1024) < 128) ?
						                               (Data->Size & 0xFFFFFF80) + 128 : (Data->Size & 0xFFFFFC00) + 1024;
						break;
					}
				}

				// Flush the read buffer
				do
				{
					RESET_WATCHDOG_TIMER();
					//                    READ(InputBuffer, 80, ReceiveTimeout);
					READ(InputBuffer, 1, ReceiveTimeout);
					RECEIVED_PROTOCOL_CHARACTERS(InputBuffer);
				}
				while( InputBuffer->Size );

				// Turn off packet ahead transmission.
				TransmitPacketAheadDuringThisSession = false;
				XModemOnly = true;
			}
			else
			{
				// Append the buffer size with tag
				OutputBuffer->Size = 0;
				OutputBuffer->AppendNumber(((unsigned short) TransmitBufferSizeTag), SIZE_OF_TAG_NUMBER);
				OutputBuffer->AppendNumber(((unsigned short) sizeof(NumberOfBytesToBeTransmitted)), SIZE_OF_TAG_LENGTH);
				OutputBuffer->AppendNumber(NumberOfBytesToBeTransmitted, sizeof(NumberOfBytesToBeTransmitted));

				// Append the size of the data prior to compression, with tag
				OutputBuffer->AppendNumber(((unsigned short) SizeOfUncompressedDataTag), SIZE_OF_TAG_NUMBER);
				OutputBuffer->AppendNumber(((unsigned short) sizeof( SizeOfOriginalData )), SIZE_OF_TAG_LENGTH);
				OutputBuffer->AppendNumber( SizeOfOriginalData, sizeof( SizeOfOriginalData ));

				// Append compression mode
				OutputBuffer->AppendNumber(((unsigned short) CompressionModeTag), SIZE_OF_TAG_NUMBER);
				OutputBuffer->AppendNumber(((unsigned short) sizeof(CompressionMode)), SIZE_OF_TAG_LENGTH);
				OutputBuffer->AppendNumber(CompressionMode, sizeof(CompressionMode));

				// Append feature flag
				OutputBuffer->AppendNumber(((unsigned short) FeatureFlagTag), SIZE_OF_TAG_NUMBER);
				OutputBuffer->AppendNumber(((unsigned short) sizeof(FeatureFlag)), SIZE_OF_TAG_LENGTH);
				OutputBuffer->AppendNumber(FeatureFlag, sizeof(FeatureFlag));

				// Zero out the remainder of the 128 byte block
				OutputBuffer->AppendFillChar(0, 128 - OutputBuffer->Size);

				// Build the CRC
				crc = ((unsigned int) calc_crc(OutputBuffer->Char, 128));

				// Store the CRC to the output buffer.
				OutputBuffer->AppendChar((crc / 256) & 0xff);
				OutputBuffer->AppendChar(crc & 0xff);

				// Write the buffer
				FLUSH_READ_BUFFER();
				WRITE(OutputBuffer);

				RESET_WATCHDOG_TIMER();

				// Now, we'll wait for an ACK.  If we don't get one, we're going to assume that the receiver can't
				// handle a header or our compression request, which will mean that we'll have to transmit uncompressed data.
				WaitStatus = WaitForACK(WaitForPacketACKTimeout, NumberOfBytesToBeTransmitted);

				if(WaitStatus == TERMINATE_RECEIVED)
				{
					DownloadTerminatedByReceiver = true;
					ReturnStatus = TERMINATE_RECEIVED;
					STATUS_MESSAGE_COPY("Receiver terminated HMODEM");
				}
				else if(WaitStatus != ACK_RECEIVED)
				{

					//            	if(WaitForACK(WaitForPacketACKTimeout, NumberOfBytesToBeTransmitted) != ACK_RECEIVED)
					//               {
					if( CompressionMode != NO_COMPRESSION )
					{
						Data->Copy( OriginalData, SizeOfOriginalData );
					}

					// Determine the total number of bytes to be transmitted, depending
					// on whether the last packet can be 128 bytes in length or not.
					if(Transmit128BytePacketMode)
					{
						switch( Data->Size % 128 )
						{
						case 0:
							NumberOfBytesToBeTransmitted = Data->Size;
							break;

						default:
							NumberOfBytesToBeTransmitted = ( Data->Size & 0xFFFFFF80 ) + 128;
							break;
						}
					}
					else
					{
						switch( Data->Size % 1024 )
						{
						case 128:
						case 0:
							NumberOfBytesToBeTransmitted = Data->Size;
							break;

						default:
							NumberOfBytesToBeTransmitted = (( Data->Size % 1024 ) < 128) ?
							                               ( Data->Size & 0xFFFFFF80 ) + 128 : ( Data->Size & 0xFFFFFC00 ) + 1024;
							break;
						}
					}

					FLUSH_WRITE_BUFFER();
					SLEEP(SLEEP_AFTER_TERMINATE);

					RESET_WATCHDOG_TIMER();

					ReturnStatus = HEADER_NOT_ACKNOWLEDGED;
					STATUS_MESSAGE_COPY("HMODEM header not acknowledged by receiver");

					// Turn off packet ahead transmission.
					TransmitPacketAheadDuringThisSession = false;
					XModemOnly = true;
				}
			}
		}

		// First packet being sent.
		BlockNumToSend = 1;
		BlockNumToBeACKed = 1;
		OffsetOfNextFileByteToBeSent = 0;
		OffsetOfNextFileByteToBeACKed = 0;
		NumberOfRetries = 0;

		// Number of bytes remaining to be sent is the entire data buffer (we haven't sent anything yet).
		BytesRemainingToBeSent = Data->Size;
		BytesRemainingToBeACKed = Data->Size;

		// While there is still data to send and we haven't encountered a
		// termination condition...
		while((BytesRemainingToBeACKed) && (!DownloadTerminatedByReceiver) && (!FatalDownloadError))
		{
			RESET_WATCHDOG_TIMER();

			// Only write the next block if there is anything to be sent.
			if(BytesRemainingToBeSent)
			{
				// Build next output buffer

				// If this is the last block, determine if it should be 128 or 1024 bytes
				// long.  The block has already been padded out with an extra 1024 zeros.
				// Even so, why send 1024 if 128 will do?  The block must be 128 or 1024
				// bytes in length for the receiver
				if(BytesRemainingToBeSent <= 128)
				{
					OutputBuffer->CopyChar(DATA_BLOCK_128_BYTE);	// Size of block is 128
					BytesRemainingToBeSent = 128;
					NumberOfDataBytesInNextPacket = 128;
				}
				else
				{
					if(Transmit128BytePacketMode)
					{
						OutputBuffer->CopyChar(DATA_BLOCK_128_BYTE);	// Size of block is 128
						NumberOfDataBytesInNextPacket = 128;
					}

					else
					{
						// We have enough data to do a 1024 byte packet.
						OutputBuffer->CopyChar(DATA_BLOCK_1024_BYTE);	// Size of block is 1024

						if(BytesRemainingToBeSent < 1024)
						{
							BytesRemainingToBeSent = 1024;
						}

						NumberOfDataBytesInNextPacket = 1024;
					}
				}

				// Put the block number into the buffer
				OutputBuffer->AppendChar(BlockNumToSend & 0xFF);
				OutputBuffer->AppendChar((~BlockNumToSend) & 0xFF);

				j = 0;

				while((j < NumberOfDataBytesInNextPacket) && ((OffsetOfNextFileByteToBeSent + j) < Data->Size ))
				{
					OutputBuffer->AppendChar(((int) ( Data->Char[OffsetOfNextFileByteToBeSent+j] )));
					++j;
				}

				// If we finished the data, we may need to fill the balance of the packet with
				// NULLs.
				while(j < NumberOfDataBytesInNextPacket)
				{
					OutputBuffer->AppendChar(0);
					++j;
				}

				// Build the CRC, using the length indicated by index "i" from the copy operation.
				crc = ((unsigned int) calc_crc(&(OutputBuffer->Char[3]), NumberOfDataBytesInNextPacket));

				// Store the CRC to the output buffer.
				OutputBuffer->AppendChar((crc / 256) & 0xff);
				OutputBuffer->AppendChar(crc & 0xff);

				// Write the buffer
				WRITE(OutputBuffer);
			}

			RESET_WATCHDOG_TIMER();

			// Tell the initiator that we're still going and see if it is OK to continue.
			if(!MAY_CONTINUE(0, NumberOfBytesToBeTransmitted))
			{
				FatalDownloadError = true;
				TerminateHModemSession();
				ReturnStatus = TERMINATE_BY_US;
				STATUS_MESSAGE_COPY("We (transmitter) terminated HMODEM");
			}
			else
			{
				// Wait for receiver ACK on a sent block if we've already sent the next block OR if
				// we just sent a block as a retry OR if we've sent all the bytes (in other words, we're
				// finished sending and we're waiting for the final packet status).
				if((BlockNumToSend > BlockNumToBeACKed) || (!TransmitPacketAheadDuringThisSession) ||
				        (NumberOfRetries) || (!BytesRemainingToBeSent))
				{

					// Wait for a receiver response until the write queue is empty.
					ACKWaitStatus = WaitForACK(WaitForPacketACKTimeout, NumberOfBytesToBeTransmitted);

					RESET_WATCHDOG_TIMER();

					switch(ACKWaitStatus)
					{
					case ACK_RECEIVED:
						// If the receiver ACKed, keep going if this isn't the last packet.  Otherwise...
						// Update the number of bytes remaining
						if(Transmit128BytePacketMode)
						{
							if(BytesRemainingToBeACKed >= 128)
							{
								BytesRemainingToBeACKed -= 128;
							}
							else
							{
								BytesRemainingToBeACKed = 0;
							}
						}
						else
						{
							if(BytesRemainingToBeACKed >= 1024)
							{
								BytesRemainingToBeACKed -= 1024;
							}
							else
							{
								BytesRemainingToBeACKed = 0;
							}
						}

						// Update to next packet number
						++BlockNumToBeACKed;
						if(BlockNumToSend <= BlockNumToBeACKed)
						{
							BytesRemainingToBeSent -= NumberOfDataBytesInNextPacket;
							++BlockNumToSend;

							// Point to next block
							if(Transmit128BytePacketMode)
							{
								OffsetOfNextFileByteToBeACKed += 128;
							}
							else
							{
								OffsetOfNextFileByteToBeACKed += 1024;
							}
							OffsetOfNextFileByteToBeSent += NumberOfDataBytesInNextPacket;
						}

						// Reset the retry counter, since we just had a success.
						NumberOfRetries = 0;

						// We successfully transferred.
						ReturnStatus = FINISH_SUCCESSFUL;

						// Since the current data packet has been successfully sent, update the initiator with the
						// progress made and see if it is OK to continue.
						if(!MAY_CONTINUE(NumberOfDataBytesInNextPacket, NumberOfBytesToBeTransmitted))
						{
							FatalDownloadError = true;
							TerminateHModemSession();
							ReturnStatus = TERMINATE_BY_US;
							STATUS_MESSAGE_COPY("We (transmitter) terminated HMODEM");
						}

						else if(!BytesRemainingToBeACKed)
						{
							// This is the LAST PACKET!!
							// Write ctrl-D (EOT) to tell the host that we're done.  Write it twice so that receiver knows that
							// this is really an EOT and not a transmission error.  Clear the NAK that the receiver will send
							// in between the two writes.  See ReceiveProcessor EOT handling for more information.
							OutputBuffer->CopyChar(EOT);
							WRITE(OutputBuffer);

							// Indicate that we're done.
							SET_PROGRESS_LEVEL(NumberOfBytesToBeTransmitted);

							// Wait for a receiver response until the write queue is empty.
							ACKWaitStatus = WaitForACK(((WaitForSecondACK) ? WaitForFinalACKTimeout : WaitForPacketACKTimeout), NumberOfBytesToBeTransmitted);

							// If we get a NAK, we'll send the second EOT and wait again for a response.
							if(ACKWaitStatus == NAK_RECEIVED)
							{
								WRITE(OutputBuffer);
								ACKWaitStatus = WaitForACK(((WaitForSecondACK) ? WaitForFinalACKTimeout : WaitForPacketACKTimeout), NumberOfBytesToBeTransmitted);
							}

							DOWNLOAD_FINISH_HANDLER();

							RESET_WATCHDOG_TIMER();

							switch(ACKWaitStatus)
							{
							case ACK_RECEIVED:
								// If we're supposed to get a second ACK, wait for an ACK; if it
								// doesn't appear, return an error
								if(WaitForSecondACK)
								{
									ACKWaitStatus = WaitForACK(WaitForFinalACKTimeout, NumberOfBytesToBeTransmitted);

									RESET_WATCHDOG_TIMER();

									switch(ACKWaitStatus)
									{
									case ACK_RECEIVED:
										ReturnStatus = FINISH_SUCCESSFUL;
										break;

									default:
										// Any other response but an ACK is an error.  Notify the user and stop
										// the current download request.
										sprintf(((char *) InputBuffer->Char),
										        "Receiver is unresponsive to finish request");
										InputBuffer->Size = strlen(((char *) InputBuffer->Char));
										ReturnStatus = RECEIVER_IS_UNRESPONSIVE;
										STATUS_MESSAGE_COPY("Receiver not responding after download completion");
										FLUSH_WRITE_BUFFER();
										break;
									}
								}

								break;

							default:
								// Any other response but an ACK is an error.  Notify the user and stop the current
								// download request.
								sprintf(((char *) InputBuffer->Char), "Receiver is unresponsive to finish request");
								InputBuffer->Size = strlen(((char *) InputBuffer->Char));
								ReturnStatus = RECEIVER_IS_UNRESPONSIVE;
								STATUS_MESSAGE_COPY("Receiver is unresponsive to finish request");
								FLUSH_WRITE_BUFFER();
								break;
							}
						}

						break;

					case NAK_RECEIVED:
						// If the receiver NAKed, notify the user and stop the current download request.
						++NumberOfRetries;
						sprintf(((char *) InputBuffer->Char), "Receiver NAKed; retry #%d\015\012", NumberOfRetries);
						InputBuffer->Size = strlen(((char *) InputBuffer->Char));
						STATUS_MESSAGE_COPY("Receiver NAKed");
						ReturnStatus = ACKWaitStatus;
						break;

					case TERMINATE_RECEIVED:
						// If the receiver wants to terminate (ESC), notify the user and stop the current
						// download request.
						++NumberOfRetries;
						DownloadTerminatedByReceiver = true;
						FatalDownloadError = true;
						STATUS_MESSAGE_COPY("Receiver terminated HMODEM");
						ReturnStatus = ACKWaitStatus;
						break;

					case TERMINATE_BY_US:
						FatalDownloadError = true;
						ReturnStatus = ACKWaitStatus;
						STATUS_MESSAGE_COPY("We (transmitter) terminated HMODEM");
						break;

					default:
					case TIMEOUT_WAITING_FOR_ACK:
						// If we timed out wait for an ACK, notify the user and stop the current
						// download request.
						++NumberOfRetries;
						sprintf(((char *) InputBuffer->Char), "Receiver is unresponsive; retry #%d\015\012",
						        NumberOfRetries);
						InputBuffer->Size = strlen(((char *) InputBuffer->Char));
						STATUS_MESSAGE_COPY("HModem receiver is unresponsive");
						ReturnStatus = RECEIVER_IS_UNRESPONSIVE;
						FLUSH_WRITE_BUFFER();
						break;
					}


					// If we need to retry, return a diagnostic message if we're configured to
					// do so and keep transmitting data until we've gone at least 500 ms with no
					// incoming data.  Then, if we have exceeded the retry count, an ESC will be
					// sent and the download operation will be cancelled.
					if(NumberOfRetries)
					{
						// Cancel transmit packet ahead if we're configured to do so.
						if(CancelTransmitPacketAheadOnRetry)
						{
							TransmitPacketAheadDuringThisSession = false;
						}

						BytesRemainingToBeSent = BytesRemainingToBeACKed;
						BlockNumToSend = BlockNumToBeACKed;
						OffsetOfNextFileByteToBeSent = OffsetOfNextFileByteToBeACKed;

						RETRY_WARNING(InputBuffer);

						if(NumberOfRetries >= MaximumNumberOfRetries)
						{
							// Write ESC to tell the host that we're quitting.  Write it twice so that receiver knows that
							// this is really an ESC and not a transmission error.  Clear the NAK that the receiver will send
							// in between the two writes.  See ReceiveProcessor ESC handling for more information.
							OutputBuffer->CopyChar(ESC);
							WRITE(OutputBuffer);
							READ(InputBuffer, 1, ReceiveTimeout);
							WRITE(OutputBuffer);
							FatalDownloadError = true;
						}
						else
						{
							// If we're about to run out of retries and we're configured to switch to 128 byte packet
							// mode, AND we're not already in that mode, change now.
							if(((NumberOfRetries + 1) >= MaximumNumberOfRetries) &&
							        (SwitchTo128BytePacketsOnRetry) && (!Transmit128BytePacketMode))
							{
								Transmit128BytePacketMode = true;

								// Give ourselves a little recovery room, so long as we indicate at least one retry.
								if(NumberOfRetries > 1)
								{
									--NumberOfRetries;
								}
							}


							unsigned int	RecoveryRetryCount = 0;

							if(XModemOnly)
							{
								do
								{
									RESET_WATCHDOG_TIMER();
									++RecoveryRetryCount;
									WaitStatus = WaitForACK(WaitForPacketACKTimeout, NumberOfBytesToBeTransmitted);
								}
								while((WaitStatus != ACK_RECEIVED) && (WaitStatus != TERMINATE_RECEIVED) &&
								        (WaitStatus != NAK_RECEIVED) &&
								        (RecoveryRetryCount <= MaximumNumberOfRetries));
							}
							else
							{
								do
								{
									RESET_WATCHDOG_TIMER();
									FLUSH_READ_BUFFER();

									OutputBuffer->CopyChar(RECOVERY_HEADER_BLOCK_BYTE);
									WRITE(OutputBuffer);
									++RecoveryRetryCount;
									WaitStatus = WaitForACK(WaitForPacketACKTimeout, NumberOfBytesToBeTransmitted);
								}
								while((WaitStatus != ACK_RECEIVED) && (WaitStatus != TERMINATE_RECEIVED) &&
								        (WaitStatus != TIMEOUT_WAITING_FOR_ACK) &&
								        (RecoveryRetryCount <= MaximumNumberOfRetries));
							}

							if((WaitStatus != ACK_RECEIVED) && (WaitStatus != NAK_RECEIVED))
							{
								// Write ESC to tell the host that we're quitting.  Write it twice so that receiver knows that
								// this is really an ESC and not a transmission error.  Clear the NAK that the receiver will send
								// in between the two writes.  See ReceiveProcessor ESC handling for more information.
								OutputBuffer->CopyChar(ESC);
								WRITE(OutputBuffer);
								READ(InputBuffer, 1, ReceiveTimeout);
								WRITE(OutputBuffer);
								FatalDownloadError = true;
							}
						}
					}
				}
				else
				{
					BytesRemainingToBeSent -= NumberOfDataBytesInNextPacket;
					BlockNumToSend++;
					// Point to next block
					OffsetOfNextFileByteToBeSent += NumberOfDataBytesInNextPacket;

				}
			}
		}
	}

	// The receiver wasn't ready to do a download.  Notify the user and stop the current download
	// request.
	else
	{
		ReturnStatus = RECEIVER_NOT_READY;
		STATUS_MESSAGE_COPY("HModem receiver is unresponsive");
		FatalDownloadError = true;
		FLUSH_WRITE_BUFFER();
		SLEEP(SLEEP_AFTER_TERMINATE);
	}

	// We're through downloading the current file.  Get rid of the buffers and exit.
	delete InputBuffer;
	delete OutputBuffer;
	delete Data;

	return(ReturnStatus);
} // TRANSMIT_PROCESSOR
//---------------------------------------------------------------------------;



//
//  RECEIVE_PROCESSOR
//      Purpose:
//          To receive data from a transmitter via HModem protocol, returning the
//          received data to the initiator.
//
//      Main loop logic:
//          Send a 'C', telling the transmitter that we're ready.
//
//          While the initiator hasn't told us to quit, no errors or termination has been
//          received from the transmitter, and we haven't overrun the character limit made
//          by the initiator...
//              Get the first packet character.  If EOT, then we've successfully received
//                  all the data.  If ESC, the transmitter wants to quit prematurely.  If
//                  we received a 1 or 2 (block size of 128 or 1024), then...
//                      Get the block number and inverse block number.  If they're OK, then...
//                          Get the data packet (either 128 or 1024 bytes).  If no errors
//                          errors occurred, then...
//                              Get the CRC.  If the transmitted CRC matches the computed
//                              CRC on the received data, send the data to the initiator
//                              and prepare to receive the next block.
//  NOTE:
//      All of the above loops will be retried up to the maximum configured number of
//      retries.  If either not enough data is received or the wrong data is received, the
//      the transmitter will be NAKed and we'll wait for them to try again.  At every step,
//      we'll check to be sure that the initiator is not telling us to quit.
//
//
unsigned int RECEIVE_PROCESSOR(unsigned char *ByteBuffer, unsigned int *ByteBufferLength, HSTRING *HstringBuffer,
                               unsigned int MaximumNumberOfBytesToReceive, bool (*ReceivedDataCharacters) (HSTRING *))
{
	bool                Cancelled;						// Due to error or user intervention, the receive is cancelled
	//  data to the receiver.
	HSTRING				*InputBuffer;					// Read buffer for processing data
	//  sent by the transmitter.
	HSTRING				*DataPacket;					// Actual data packet sent by caller.
	HSTRING				*OutputBuffer;					// Write buffer, for writing to the transmitter.
	unsigned int				BlockNumber;					// Next packet to be received from the transmitter.
	unsigned int				CRC;							// Computed CRC of a packet received from the transmitter.
	unsigned int				NumberOfRetries;				// Number of packet receive retries.
	unsigned int				i;								// Indices
	unsigned int				NumberOfBytesReceived;			// Total number of data bytes received so far.
	unsigned int				ReturnStatus=0;					// Status of download operation.
	unsigned int				BlockSize;						// Size of next block.
	unsigned short                Tag;							// Tag number of the field currently being extracted from header block.
	unsigned short                Length;							// Length of the field currently being extracted from header block.
	unsigned int	            Number;							// Extracted numeric value of a field in a header block.
	unsigned int				SizeOfUncompressedData;			// If data is sent compressed from the transmitter, this is the size of
	//  the data before it was compressed.  We can use it to know if we'll
	//	have enough space in the output data stream for the uncompressed
	//	data.
	unsigned int				CompressionMode;				// If data is sent compressed from the transmitter, this is the
	//	compression mode that will have to be used to decompress the data.
	unsigned int				SizeOfSecondToLastPacket;		// In case we have to back up two packet when receiving in packet ahead
	//	mode, we need to know the size of the second to last packet
	//	successfully received.
	unsigned int				SizeOfLastPacket;				// In case we have to back up two packet when receiving in packet ahead
	//	mode, we need to know the size of the last packet successfully
	//	received.
	unsigned int				FeatureFlag = 0;				// Defines features supported by transmitter (none by default).




	//	ReturnStatus = FINISH_SUCCESSFUL;		// No errors yet.

	STATUS_MESSAGE_COPY("");	// No status message yet.

	NumberOfBytesToBeSentByTransmitter = 0;		// Don't know how much data to expect yet.
	SizeOfUncompressedData = 0;

	CompressionMode = NO_COMPRESSION;			// No compression requested yet.


	// Zero out number of bytes received, if it's defined
	if(ByteBufferLength != NULL)
	{
		*ByteBufferLength = 0;
	}

	// Zero out HSTRING buffer, if it's defined
	if(HstringBuffer != NULL)
	{
		HstringBuffer->Size = 0;

		if( MaximumNumberOfBytesToReceive > (unsigned int) HstringBuffer->MaximumLength() )
		{
			MaximumNumberOfBytesToReceive = (unsigned int) HstringBuffer->MaximumLength();
		}
	}



	// Allocate string space.
	InputBuffer = new HSTRING(1100);
	DataPacket = new HSTRING(1100);
	OutputBuffer = new HSTRING(1100);

	// No errors have occurred yet as part of the download process.
	Cancelled = false;



	// Send 'C' to tell the transmitter that we're ready to receive data.
	NumberOfRetries = 0;            // No retries yet on trying to synchronize with transmitter.
	do
	{
		RESET_WATCHDOG_TIMER();
		OutputBuffer->CopyChar('C');
		WRITE(OutputBuffer);

		READ(InputBuffer, 1, ReceiveTimeout);
		RECEIVED_PROTOCOL_CHARACTERS(InputBuffer);

		if(InputBuffer->Size == 1)
		{
			// If we received an EOT, send an ACK to the transmitter and quit with success.
			if(InputBuffer->Char[0] == EOT)
			{
				OutputBuffer->CopyChar(ACK);
				WRITE(OutputBuffer);

				Cancelled = true;
				ReturnStatus = FINISH_SUCCESSFUL;
				break;
			}

			// If we received an ESC, quit with TERMINATE_RECEIVED.
			else if(InputBuffer->Char[0] == ESC)
			{
				Cancelled = true;
				ReturnStatus = TERMINATE_RECEIVED;
				STATUS_MESSAGE_COPY("Transmitter terminated HMODEM");
				break;
			}
		}

		if((InputBuffer->Size != 1) ||
		        ((InputBuffer->Char[0] != DATA_BLOCK_128_BYTE) && (InputBuffer->Char[0] != DATA_BLOCK_1024_BYTE) &&
		         (InputBuffer->Char[0] != HEADER_BLOCK_128_BYTE)))
		{
			++NumberOfRetries;
		}



		if(NumberOfRetries >= MaximumNumberOfRetries)
		{
			OutputBuffer->CopyChar(ESC);
			WRITE(OutputBuffer);
			ReturnStatus = ILLEGAL_HMODEM_BLOCK_SIZE;
			STATUS_MESSAGE_COPY("Illegal HModem block size received for first block");

			Cancelled = true;
		}
	}
	while((!Cancelled) && ((InputBuffer->Size != 1) ||
	                       ((InputBuffer->Char[0] != DATA_BLOCK_128_BYTE) && (InputBuffer->Char[0] != DATA_BLOCK_1024_BYTE) &&
	                        (InputBuffer->Char[0] != HEADER_BLOCK_128_BYTE))));



	// If we're getting a header, receive and process it.
	if((InputBuffer->Size == 1) && (InputBuffer->Char[0] == HEADER_BLOCK_128_BYTE))
	{
		// Send an ACK right away to tell the transmitter that we're ready for the header block.
		OutputBuffer->CopyChar(ACK);
		WRITE(OutputBuffer);

		BlockSize = 130;        // Tagged header + CRC
		InputBuffer->Size = 0;
		DataPacket->Size = 0;
		NumberOfBytesReceived = 0;      // Nothing received yet.

		while((DataPacket->Size < BlockSize) && MAY_CONTINUE(0L, NumberOfBytesToBeSentByTransmitter))
		{
			RESET_WATCHDOG_TIMER();
			READ(InputBuffer, BlockSize - DataPacket->Size, ReceiveTimeout);
			DataPacket->Append(InputBuffer);
			RECEIVED_PROTOCOL_CHARACTERS(InputBuffer);
			NumberOfBytesReceived += InputBuffer->Size;
			if(InputBuffer->Size == 0)
			{
				break;
			}
		}

		// If we didn't receive the entire data packet, flag an error to the host
		if(MAY_CONTINUE(0L, NumberOfBytesToBeSentByTransmitter))
		{
			// If we didn't get the correct sized header with CRC, flag an error and return a NAK, but don't try again.
			if(DataPacket->Size != BlockSize)
			{
				sprintf(((char *) InputBuffer->Char), "Failed to read full header : bytes read = %d\015\012", DataPacket->Size);
				InputBuffer->Size = strlen(((char *) InputBuffer->Char));

				ReturnStatus = TIMEOUT_WAITING_FOR_HMODEM_BLOCK;
				STATUS_MESSAGE_COPY("Receiver timeout while waiting for first block to be transmitted");

				RETRY_WARNING(InputBuffer);

				do
				{
					RESET_WATCHDOG_TIMER();
					READ(InputBuffer, 1024, ReceiveTimeout);
					RECEIVED_PROTOCOL_CHARACTERS(InputBuffer);
				}
				while(InputBuffer->Size);

				OutputBuffer->CopyChar(NAK);
				WRITE(OutputBuffer);
			}

			// Build CRC and compare.
			else
			{
				CRC = ((unsigned int) calc_crc(((unsigned char *) (DataPacket->Char)), BlockSize - 2));
				if(CRC != ((unsigned int) (DataPacket->Char[BlockSize-1] + DataPacket->Char[BlockSize-2] * 256)))
				{
					sprintf(((char *) InputBuffer->Char), "CRC mismatch (%X sent vs. %X) on header block\015\012",
					        ((unsigned int) (DataPacket->Char[BlockSize-1] + DataPacket->Char[BlockSize-2] * 256)), CRC);
					InputBuffer->Size = strlen(((char *) InputBuffer->Char));

					ReturnStatus = MISMATCHED_HMODEM_CRC;
					STATUS_MESSAGE_COPY("Receiver CRC mismatch on first block transmitted");

					RETRY_WARNING(InputBuffer);

					OutputBuffer->CopyChar(NAK);
					WRITE(OutputBuffer);
				}

				// We received the header correctly
				else
				{
					// Parse tags in the header packet.
					i = 0;

					// Decode header block tags until we've reached the end of the header
					while(i < DataPacket->Size - 2)
					{
						RESET_WATCHDOG_TIMER();
						DataPacket->CopyFromString(i, SIZE_OF_TAG_NUMBER, &Tag);
						i += SIZE_OF_TAG_NUMBER;
						DataPacket->CopyFromString(i, SIZE_OF_TAG_LENGTH, &Length);
						i += SIZE_OF_TAG_LENGTH;

						Number = 0;

						switch(Tag)
						{
							// Buffer size
						case TransmitBufferSizeTag:
							DataPacket->CopyFromString(i, Length, &Number);
							i += Length;
							NumberOfBytesToBeSentByTransmitter = Number;
							break;

							// Size of data prior to compression
						case SizeOfUncompressedDataTag:
							DataPacket->CopyFromString(i, Length, &Number);
							i += Length;
							SizeOfUncompressedData = Number;
							break;

							// Compression mode
						case CompressionModeTag:
							DataPacket->CopyFromString(i, Length, &Number);
							i += Length;
							CompressionMode = Number;
							break;

							// Feature flag
						case FeatureFlagTag:
							DataPacket->CopyFromString(i, Length, &Number);
							i += Length;
							FeatureFlag = Number;
							break;

						default:
							i += Length;
							break;
						}
					}


					// If number of bytes to be sent by transmitter, when decompressed, will be too large, reallocate if this is
					// a resizable HSTRING, else quit with an error.  Note that even if we can't process compressed data, we'll
					// still use the uncompressed size for resizing, because the transmitter will get our NAK and will have to send
					// uncompressed anyway.
					if( ( NumberOfBytesToBeSentByTransmitter > MaximumNumberOfBytesToReceive ) ||
					        ( SizeOfUncompressedData > MaximumNumberOfBytesToReceive ) )
					{
						bool	ReallocateStatus = false;		// Haven't successfully reallocated yet.
						unsigned int	TotalStorageNeeded = ( NumberOfBytesToBeSentByTransmitter > SizeOfUncompressedData ) ?
						                                  NumberOfBytesToBeSentByTransmitter : SizeOfUncompressedData;

						if( ( HstringBuffer != NULL ) && ( HstringBuffer->AutoResizing() ) )
						{
							ReallocateStatus = HstringBuffer->Reallocate( TotalStorageNeeded + 1024 );
							MaximumNumberOfBytesToReceive = TotalStorageNeeded + 1024;
						}

						if( !ReallocateStatus )
						{
							Cancelled = true;
							sprintf(((char *) InputBuffer->Char),
							        "%d bytes of decompressed data too large for %d byte buffer\015\012",
							        TotalStorageNeeded, MaximumNumberOfBytesToReceive);
							InputBuffer->Size = strlen(((char *) InputBuffer->Char));
							RETRY_WARNING(InputBuffer);

							ReturnStatus = TRANSMIT_DATA_TOO_LARGE;
							STATUS_MESSAGE_COPY("Receiver refusing data from transmitter (too large)");

							OutputBuffer->CopyChar(ESC);
							WRITE(OutputBuffer);
						}

						/*
												else
												{
							                    // Send an ACK
								                    OutputBuffer->CopyChar(ACK);
									                WRITE(OutputBuffer);
												}
						*/
					}

					// If compression is being attempted, but we don't support it, retry.
					if( !Cancelled )

					{
						if(( CompressionMode != NO_COMPRESSION ) && ( !COMPRESSION_SUPPORTED ))
						{
							if( NumberOfBytesToBeSentByTransmitter < SizeOfUncompressedData )
							{
								NumberOfBytesToBeSentByTransmitter = SizeOfUncompressedData;
							}

							CompressionMode = NO_COMPRESSION;			// Reset to no compression.

							sprintf(((char *) InputBuffer->Char),
							        "Compression mode %d not supported by receiver\015\012", CompressionMode);
							InputBuffer->Size = strlen(((char *) InputBuffer->Char));

							ReturnStatus = UNRECOGNIZED_COMPRESSION_MODE;
							STATUS_MESSAGE_COPY("Receiver doesn't support compression");

							RETRY_WARNING(InputBuffer);

							OutputBuffer->CopyChar(NAK);
							WRITE(OutputBuffer);
						}

						// If compression mode is no good, retry.
						else if(CompressionMode >= INVALID_COMPRESSION_MODE)
						{
							if( NumberOfBytesToBeSentByTransmitter < SizeOfUncompressedData )
							{
								NumberOfBytesToBeSentByTransmitter = SizeOfUncompressedData;
							}

							CompressionMode = NO_COMPRESSION;			// Reset to no compression.

							sprintf(((char *) InputBuffer->Char),
							        "Compression mode %d not recognized by receiver\015\012", CompressionMode);
							InputBuffer->Size = strlen(((char *) InputBuffer->Char));

							ReturnStatus = UNRECOGNIZED_COMPRESSION_MODE;
							STATUS_MESSAGE_COPY("Receiver doesn't recognize compression mode of data sent by transmitter");

							RETRY_WARNING(InputBuffer);

							OutputBuffer->CopyChar(NAK);
							WRITE(OutputBuffer);
						}

						// The header is OK; send an ACK to the transmitter.
						else
						{
							// Send an ACK
							OutputBuffer->CopyChar(ACK);
							WRITE(OutputBuffer);
						}
					}
				}
			}
		}

		else
		{
			do
			{
				RESET_WATCHDOG_TIMER();
				READ(InputBuffer, 1024, ReceiveTimeout);
				RECEIVED_PROTOCOL_CHARACTERS(InputBuffer);
			}
			while(InputBuffer->Size);

			OutputBuffer->CopyChar(ESC);
			WRITE(OutputBuffer);
		}

		// Zero out so that receive loop won't think that there is a block size byte to start with.
		InputBuffer->Size = 0;
	}




	NumberOfRetries = 0;            // No retries yet on data packet transmission.

	NumberOfBytesReceived = 0;      // Nothing received yet.

	BlockNumber = 1;                // Starting with first block.



	// Get data packets until full image (plus 5 AGC bytes) is received, text window is terminated, or user cancelled.
	SizeOfSecondToLastPacket = 0;
	SizeOfLastPacket = 0;

	while((MAY_CONTINUE(0L, NumberOfBytesToBeSentByTransmitter)) &&
	        (!Cancelled))
	{
		RESET_WATCHDOG_TIMER();

		// Get next block size, or EOT if the transmitter is finished..
		if(InputBuffer->Size == 0)
		{
			READ(InputBuffer, 1, ReceiveTimeout);
			RECEIVED_PROTOCOL_CHARACTERS(InputBuffer);
		}

		RESET_WATCHDOG_TIMER();

		// If we didn't get a block size, flag an error and quit if number of retries is exceeded, else try again.
		if(InputBuffer->Size != 1)
		{
			++NumberOfRetries;
			sprintf(((char *) InputBuffer->Char), "Failed to read block size (%d bytes); retry #%d : block #%d\015\012",
			        InputBuffer->Size, NumberOfRetries, BlockNumber);
			InputBuffer->Size = strlen(((char *) InputBuffer->Char));

			ReturnStatus = TIMEOUT_WAITING_FOR_HMODEM_BLOCK_SIZE;
			STATUS_MESSAGE_COPY("Receiver timed out while waiting for HModem block size");
		}

		// If we received an EOT, send an ACK to the transmitter and quit with success.
		else if(InputBuffer->Char[0] == EOT)
		{
			// We now NAK the first EOT byte and ACK the second one, so long as it is received immediately following.
			// The reason for this is that we want to make sure that this is actually an EOT byte and not part of a
			// data packet.  We were seeing a situation where the packet size byte didn't appear and the first packet byte we saw
			// was the block number, which happened to be equal to the recovery header byte, and things never recovered, because
			// we ACKed the recovery header byte and the transmitter thought we were ACKing the packet.  Now, we NAK the first
			// recovery header byte and ACK the second one, so long as it is received immediately following and as a safeguard,
			// we're doing the same for EOT.
			if(FeatureFlag & SUPPORT_OF_DOUBLE_EOT)
			{
				do
				{
					RESET_WATCHDOG_TIMER();
					READ(InputBuffer, 1024, 1); //ReceiveTimeout);
					RECEIVED_PROTOCOL_CHARACTERS(InputBuffer);
				}
				while(InputBuffer->Size);

				OutputBuffer->CopyChar(NAK);
				WRITE(OutputBuffer);

				READ(InputBuffer, 1, ReceiveTimeout * 2);

				if((InputBuffer->Size == 1) && (InputBuffer->Char[0] == EOT))
				{
					OutputBuffer->CopyChar(ACK);
					WRITE(OutputBuffer);

					Cancelled = true;
					ReturnStatus = FINISH_SUCCESSFUL;
				}
				else
				{
					++NumberOfRetries;		// For now, let's just cancel and assume that there was no error.
				}
			}
			else
			{
				OutputBuffer->CopyChar(ACK);
				WRITE(OutputBuffer);

				Cancelled = true;
				ReturnStatus = FINISH_SUCCESSFUL;
			}
		}


		// If we received an ESC, quit with TERMINATE_RECEIVED.
		else if(InputBuffer->Char[0] == ESC)
		{
			// We now NAK the first ESC byte and ACK the second one, so long as it is received immediately following.
			// The reason for this is that we want to make sure that this is actually an ESC byte and not part of a
			// data packet.  We were seeing a situation where the packet size byte didn't appear and the first packet byte we saw
			// was the block number, which happened to be equal to the ESC byte, which caused us to quit, thinking that the
			// transmitter wanted to terminate.  Now, we NAK the first ESC and ACK the second one, so long as it is received
			// immediately following.
			do
			{
				RESET_WATCHDOG_TIMER();
				READ(InputBuffer, 1024, 1); //ReceiveTimeout);
				RECEIVED_PROTOCOL_CHARACTERS(InputBuffer);
			}
			while(InputBuffer->Size);

			OutputBuffer->CopyChar(NAK);
			WRITE(OutputBuffer);

			READ(InputBuffer, 1, ReceiveTimeout * 2);

			if(InputBuffer->Size == 1)
			{
				if(InputBuffer->Char[0] == ESC)
				{
					Cancelled = true;
					ReturnStatus = TERMINATE_RECEIVED;
					STATUS_MESSAGE_COPY("Transmitter terminated HMODEM");
				}
				else
				{
					++NumberOfRetries;
				}
			}
			else
			{
				Cancelled = true;
				ReturnStatus = TERMINATE_RECEIVED;
				STATUS_MESSAGE_COPY("Transmitter terminated HMODEM");
			}
		}

		// If we're receiving a header recovery byte, ACK and continue!
		else if(InputBuffer->Char[0] == RECOVERY_HEADER_BLOCK_BYTE)
		{
			sprintf(((char *) InputBuffer->Char), "Received recovery byte; retry #%d : block #%d\015\012",
			        NumberOfRetries, BlockNumber);
			InputBuffer->Size = strlen(((char *) InputBuffer->Char));
			RETRY_WARNING(InputBuffer);

			InputBuffer->Size = 0;
			NumberOfRetries = 0;


			// We now NAK the first recovery header byte and ACK the second one, so long as it is received immediately following.
			// The reason for this is that we want to make sure that this is actually a recovery header byte and not part of a
			// data packet.  We were seeing a situation where the packet size byte didn't appear and the first packet byte we saw
			// was the block number, which happened to be equal to the recovery header byte, and things never recovered, because
			// we ACKed the recovery header byte and the transmitter thought we were ACKing the packet.  Now, we NAK the first
			// recovery header byte and ACK the second one, so long as it is received immediately following.
			do
			{
				RESET_WATCHDOG_TIMER();
				READ(InputBuffer, 1024, 1); //ReceiveTimeout);
				RECEIVED_PROTOCOL_CHARACTERS(InputBuffer);
			}
			while(InputBuffer->Size);

			OutputBuffer->CopyChar(NAK);
			WRITE(OutputBuffer);

			READ(InputBuffer, 1, ReceiveTimeout * 2);

			if((InputBuffer->Size == 1) && (InputBuffer->Char[0] == RECOVERY_HEADER_BLOCK_BYTE))
			{
				OutputBuffer->CopyChar(ACK);
				WRITE(OutputBuffer);
			}
			else
			{
				++NumberOfRetries;
			}
		}

		// If we didn't get the right block size (1 or 2), flag an error and quit if number of retries is exceeded, else try again.
		else if((InputBuffer->Char[0] != DATA_BLOCK_128_BYTE) && (InputBuffer->Char[0] != DATA_BLOCK_1024_BYTE))
		{
			++NumberOfRetries;
			sprintf(((char *) InputBuffer->Char), "Bad block size; retry #%d : block #%d\015\012",
			        NumberOfRetries, BlockNumber);
			InputBuffer->Size = strlen(((char *) InputBuffer->Char));

			ReturnStatus = ILLEGAL_HMODEM_BLOCK_SIZE;
			STATUS_MESSAGE_COPY("Illegal HModem block size received");
		}

		// We'd better not be over the limit for receive characters
		// **** May want to get rid of this altogether.
		else if(NumberOfBytesReceived >= MaximumNumberOfBytesToReceive)
		{
			Cancelled = true;
			ReturnStatus = TRANSMIT_DATA_TOO_LARGE;
			STATUS_MESSAGE_COPY("Exceeded maximum data to be received");
		}

		// Block size was correct, and it's OK to continue.  Get the block number.
		else if(MAY_CONTINUE(0L, NumberOfBytesToBeSentByTransmitter))
		{
			if(InputBuffer->Char[0] == DATA_BLOCK_128_BYTE)
			{
				BlockSize = 128;
			}
			else
			{
				BlockSize = 1024;
			}


			RESET_WATCHDOG_TIMER();

			// Get block number.
			READ(InputBuffer, 1, ReceiveTimeout);
			RECEIVED_PROTOCOL_CHARACTERS(InputBuffer);

			// If the block size is going to push us over the limit for receive characters, WOOF!.
			if((NumberOfBytesReceived + BlockSize) > MaximumNumberOfBytesToReceive)
			{
				Cancelled = true;
				ReturnStatus = TRANSMIT_DATA_TOO_LARGE;
				STATUS_MESSAGE_COPY("Exceeded maximum data to be received");
			}

			// If we didn't get a block number, flag an error and quit if number of retries is exceeded, else try again.
			else if(InputBuffer->Size != 1)
			{
				++NumberOfRetries;
				sprintf(((char *) InputBuffer->Char), "Failed to read block number; retry #%d : block #%d\015\012",
				        NumberOfRetries, BlockNumber);
				InputBuffer->Size = strlen(((char *) InputBuffer->Char));

				ReturnStatus = TIMEOUT_WAITING_FOR_HMODEM_BLOCK_NUMBER;
				STATUS_MESSAGE_COPY("Receiver timed out while waiting for HModem block number");
			}

			// If we didn't get the right block number (equal to our next or previous block number), flag an error
			// and quit if number of retries is exceeded, else try again.
			//            else if(((BlockNumber & 0xFF) != ((unsigned int) InputBuffer->Char[0])) &&
			//                    ((BlockNumber == 1) ||
			//                                ((BlockNumber & 0xFF) != ((((unsigned int) InputBuffer->Char[0]) + 1) & 0xff))))
			else if(((BlockNumber & 0xFF) != (((unsigned int) InputBuffer->Char[0]) & 0xFF)) &&
			        ((BlockNumber & 0xFF) != ((((unsigned int) InputBuffer->Char[0]) + 1) & 0xFF)) &&
			        ((BlockNumber & 0xFF) != ((((unsigned int) InputBuffer->Char[0]) + 2) & 0xFF)))
			{
				++NumberOfRetries;
				sprintf(((char *) InputBuffer->Char), "Bad block number %d; retry #%d : expecting block #%d\015\012",
				        ((unsigned int) InputBuffer->Char[0]), NumberOfRetries, BlockNumber);
				InputBuffer->Size = strlen(((char *) InputBuffer->Char));

				ReturnStatus = MISMATCHED_HMODEM_BLOCK_NUMBER;
				STATUS_MESSAGE_COPY("Mismatched HModem block number received");
			}

			// Block numbers match, and it's OK to continue.  Get the inverse block number.
			else if(MAY_CONTINUE(0L, NumberOfBytesToBeSentByTransmitter))
			{
				RESET_WATCHDOG_TIMER();

				// If we are receiving the previous block due to a communication error (even though we received it
				// OK, let's back up
				if((BlockNumber & 0xFF) == ((((unsigned int) InputBuffer->Char[0]) + 1) & 0xFF))
				{
					sprintf(((char *) InputBuffer->Char), "Lower block number %d; retry #%d : expecting block #%d\015\012",
					        ((unsigned int) InputBuffer->Char[0]), NumberOfRetries, BlockNumber);
					InputBuffer->Size = strlen(((char *) InputBuffer->Char));
					RETRY_WARNING(InputBuffer);
					InputBuffer->Size = 0;

					--BlockNumber;

					NumberOfBytesReceived -= SizeOfLastPacket;
					SET_PROGRESS_LEVEL(NumberOfBytesReceived);

					if(ByteBuffer != NULL)
					{
						*ByteBufferLength -= SizeOfLastPacket;
					}

					if(HstringBuffer != NULL)
					{
						HstringBuffer->Size -= SizeOfLastPacket;
					}
				}

				if((BlockNumber & 0xFF) == ((((unsigned int) InputBuffer->Char[0]) + 2) & 0xFF))
				{
					sprintf(((char *) InputBuffer->Char), "Lower block number %d; retry #%d : expecting block #%d\015\012",
					        ((unsigned int) InputBuffer->Char[0]), NumberOfRetries, BlockNumber);
					InputBuffer->Size = strlen(((char *) InputBuffer->Char));
					RETRY_WARNING(InputBuffer);
					InputBuffer->Size = 0;

					BlockNumber -= 2;

					NumberOfBytesReceived -= (SizeOfSecondToLastPacket + SizeOfLastPacket);
					SET_PROGRESS_LEVEL(NumberOfBytesReceived);

					if(ByteBuffer != NULL)
					{
						*ByteBufferLength -= (SizeOfSecondToLastPacket + SizeOfLastPacket);
					}

					if(HstringBuffer != NULL)
					{
						HstringBuffer->Size -= (SizeOfSecondToLastPacket + SizeOfLastPacket);
					}
				}

				RESET_WATCHDOG_TIMER();

				// Get inverse block number.
				READ(InputBuffer, 1, ReceiveTimeout);
				RECEIVED_PROTOCOL_CHARACTERS(InputBuffer);


				// If we didn't get an inverse block number, flag an error and quit if number of retries is exceeded, else try again.
				if(InputBuffer->Size != 1)
				{
					++NumberOfRetries;
					sprintf(((char *) InputBuffer->Char), "Failed to read inverse block number; retry #%d : block #%d\015\012",
					        NumberOfRetries, BlockNumber);
					InputBuffer->Size = strlen(((char *) InputBuffer->Char));

					ReturnStatus = TIMEOUT_WAITING_FOR_HMODEM_INVERSE_BLOCK_NUMBER;
					STATUS_MESSAGE_COPY("Receiver timed out while waiting for HModem inverse block number");
				}

				// If we didn't get the right inverse block number (equal to complement of our block number), flag an error
				// and quit if number of retries is exceeded, else try again.
				else if(((~BlockNumber) & 0xFF) != ((unsigned int) InputBuffer->Char[0]))
				{
					++NumberOfRetries;
					sprintf(((char *) InputBuffer->Char), "Bad inverse block number; retry #%d : block #%d\015\012",
					        NumberOfRetries, BlockNumber);
					InputBuffer->Size = strlen(((char *) InputBuffer->Char));

					ReturnStatus = MISMATCHED_HMODEM_INVERSE_BLOCK_NUMBER;
					STATUS_MESSAGE_COPY("Mismatched HModem inverse block number received");
				}

				// Inverse block numbers match, and it's OK to continue.  Get the block.
				else if(MAY_CONTINUE(0L, NumberOfBytesToBeSentByTransmitter))
				{
					// Read the data block.
					InputBuffer->Size = 0;
					DataPacket->Size = 0;
					while((DataPacket->Size < BlockSize) && MAY_CONTINUE(0L, NumberOfBytesToBeSentByTransmitter))
					{
						RESET_WATCHDOG_TIMER();
						READ(InputBuffer, BlockSize - DataPacket->Size, ReceiveTimeout);
						DataPacket->Append(InputBuffer);
						RECEIVED_PROTOCOL_CHARACTERS(InputBuffer);
						//                        NumberOfBytesReceived += InputBuffer->Size;
						if(InputBuffer->Size == 0)
						{
							break;
						}
					}


					// It's OK to continue.  Check the block size.
					if(MAY_CONTINUE(0L, NumberOfBytesToBeSentByTransmitter))
					{
						// If we didn't get the correct sized block, flag an error and quit if number of retries is exceeded,
						// else try again.
						if(DataPacket->Size != BlockSize)
						{
							++NumberOfRetries;
							sprintf(((char *) InputBuffer->Char), "Failed to read data packet; retry #%d : block #%d\015\012",
							        NumberOfRetries, BlockNumber);
							InputBuffer->Size = strlen(((char *) InputBuffer->Char));

							ReturnStatus = TIMEOUT_WAITING_FOR_HMODEM_BLOCK;
							STATUS_MESSAGE_COPY("Receiver timed out while waiting for HModem block data");
						}

						// Block size is OK.  Get the CRC if operation not cancelled.
						else
						{
							READ(InputBuffer, 1, ReceiveTimeout);
							RECEIVED_PROTOCOL_CHARACTERS(InputBuffer);
							READ(OutputBuffer, 1, ReceiveTimeout);
							RECEIVED_PROTOCOL_CHARACTERS(OutputBuffer);
							InputBuffer->Append(OutputBuffer);

							RESET_WATCHDOG_TIMER();

							// If we didn't get a 2 byte CRC, flag an error and quit if number of retries is exceeded, else try again.
							if(InputBuffer->Size != 2)
							{
								++NumberOfRetries;
								sprintf(((char *) InputBuffer->Char), "Failed to read CRC; retry #%d : block #%d\015\012",
								        NumberOfRetries, BlockNumber);
								InputBuffer->Size = strlen(((char *) InputBuffer->Char));

								ReturnStatus = TIMEOUT_WAITING_FOR_HMODEM_CRC;
								STATUS_MESSAGE_COPY("Receiver timed out while waiting for HModem block CRC");
							}

							// 2 CRC bytes were received, and it's OK to continue.
							else if(MAY_CONTINUE(0L, NumberOfBytesToBeSentByTransmitter))
							{
								// Build the CRC on the data packet we just received.  The CRC sent
								// in the packet must match this calculated CRC.
								CRC = ((unsigned int) calc_crc(((unsigned char *) (DataPacket->Char)), BlockSize));

								// If they don't match, flag an error and quit if number of retries is exceeded, else try again.
								if(CRC != ((unsigned int) (InputBuffer->Char[1] + InputBuffer->Char[0] * 256)))
								{
									++NumberOfRetries;
									sprintf(((char *) InputBuffer->Char), "CRC mismatch (%X sent vs. %X); retry #%d : block # %d\015\012",
									        ((unsigned int) (InputBuffer->Char[1] + InputBuffer->Char[0] * 256)),
									        CRC, NumberOfRetries, BlockNumber);
									InputBuffer->Size = strlen(((char *) InputBuffer->Char));

									ReturnStatus = MISMATCHED_HMODEM_CRC;
									STATUS_MESSAGE_COPY("Mismatched HModem block CRC received");
								}

								// We received the block correctly, and it's OK to continue.
								else if(MAY_CONTINUE(DataPacket->Size, NumberOfBytesToBeSentByTransmitter))
								{
									// Since we received correctly, update the last packet size variables.
									SizeOfSecondToLastPacket = SizeOfLastPacket;
									SizeOfLastPacket = BlockSize;
									NumberOfBytesReceived += BlockSize;

									NumberOfRetries = 0;            // Reset retry counter

									// Send an ACK
									OutputBuffer->CopyChar(ACK);
									WRITE(OutputBuffer);

									// Get ready for the next block number
									++BlockNumber;

									// Add data to the byte buffer, if it's defined.
									if(ByteBuffer != NULL)
									{
										for(i=0; i<DataPacket->Size; ++i)
										{
											ByteBuffer[(*ByteBufferLength)++] = DataPacket->Char[i];
										}
									}

									// Add data to the Hstring buffer, if it's defined.
									if(HstringBuffer != NULL)
									{
										HstringBuffer->Append(DataPacket);
									}

									// Return the data, if a handler is defined; cancel if the receive returns false.
									if(ReceivedDataCharacters != NULL)
									{
										if(!ReceivedDataCharacters(DataPacket))
										{
											Cancelled = true;
										}
									}
								}

								// We received the block correctly, but it's NOT OK to continue.
								else
								{
									OutputBuffer->CopyChar(ESC);
									WRITE(OutputBuffer);
									SLEEP(SLEEP_AFTER_TERMINATE);
									FLUSH_READ_BUFFER();
								}
							}

							// 2 CRC bytes were received, but it's NOT OK to continue.
							else
							{
								OutputBuffer->CopyChar(ESC);
								WRITE(OutputBuffer);
								SLEEP(SLEEP_AFTER_TERMINATE);
								FLUSH_READ_BUFFER();
							}
						}
					}

					// Received block size was OK, but it's NOT OK to continue.
					else
					{
						OutputBuffer->CopyChar(ESC);
						WRITE(OutputBuffer);
						SLEEP(SLEEP_AFTER_TERMINATE);
						FLUSH_READ_BUFFER();
					}
				}

				// Inverse block numbers match, but it's NOT OK to continue.
				else
				{
					OutputBuffer->CopyChar(ESC);
					WRITE(OutputBuffer);
					SLEEP(SLEEP_AFTER_TERMINATE);
					FLUSH_READ_BUFFER();
				}
			}

			// Block numbers match, but it's NOT OK to continue.
			else
			{
				OutputBuffer->CopyChar(ESC);
				WRITE(OutputBuffer);
				SLEEP(SLEEP_AFTER_TERMINATE);
				FLUSH_READ_BUFFER();
			}
		}

		// Block size was correct, but it's NOT OK to continue.
		else
		{
			OutputBuffer->CopyChar(ESC);
			WRITE(OutputBuffer);
			SLEEP(SLEEP_AFTER_TERMINATE);
			FLUSH_READ_BUFFER();
		}


		// If we need to retry, return a diagnostic message if we're configured to
		// do so and keep receiving data until we've gone at least 500 ms with no
		// incoming data.  Then, if we haven't exceeded the retry count, a NAK will be
		// sent to the transmitter; otherwise, an ESC will be sent and the download
		// operation will be cancelled.
		if(NumberOfRetries)
		{
			RETRY_WARNING(InputBuffer);

			do
			{
				RESET_WATCHDOG_TIMER();
				READ(InputBuffer, 1024, ReceiveTimeout);
				RECEIVED_PROTOCOL_CHARACTERS(InputBuffer);
			}
			while(InputBuffer->Size);

			if(NumberOfRetries >= MaximumNumberOfRetries)
			{
				OutputBuffer->CopyChar(ESC);
				WRITE(OutputBuffer);

				Cancelled = true;
			}
			else
			{
				OutputBuffer->CopyChar(NAK);
				WRITE(OutputBuffer);
			}
		}

		InputBuffer->Size = 0;      // Zero out so that we don't try to read another block size.
	}


	// If we terminated the transfer, reflect that fact in the return status.
	if(!MAY_CONTINUE(0L, NumberOfBytesToBeSentByTransmitter))
	{
		ReturnStatus = TERMINATE_BY_US;
		STATUS_MESSAGE_COPY("We (receiver) terminated HMODEM");
	}



	// Decompress data to byte and/or HSTRING buffers
	if(ByteBuffer != NULL)
	{
		RESET_WATCHDOG_TIMER();

		if(CompressionMode != NO_COMPRESSION)
		{
			HSTRING *CompressedData;

			if(*ByteBufferLength < SizeOfUncompressedData)
			{
				CompressedData = new HSTRING(SizeOfUncompressedData+5, true);
			}
			else
			{
				CompressedData = new HSTRING((*ByteBufferLength)+5, true);
			}

			CompressedData->Copy(ByteBuffer, *ByteBufferLength);
			DECOMPRESS_DATA(CompressedData, CompressionMode, true, SizeOfUncompressedData);
			CompressedData->CopyFromString(0L, CompressedData->Size, ByteBuffer);
			*ByteBufferLength = CompressedData->Size;
			delete CompressedData;
		}
		else
		{
			*ByteBufferLength = SizeOfUncompressedData;
		}
	}

	if(HstringBuffer != NULL)
	{
		RESET_WATCHDOG_TIMER();

		if(CompressionMode != 0)
		{
			DECOMPRESS_DATA(HstringBuffer, CompressionMode, true, SizeOfUncompressedData);
		}
		else if(HstringBuffer->Size >= SizeOfUncompressedData)
		{
			HstringBuffer->Size = SizeOfUncompressedData;
		}
	}


	// We're through downloading the current file.  Get rid of the buffers and exit.
	delete InputBuffer;
	delete DataPacket;
	delete OutputBuffer;

	RESET_WATCHDOG_TIMER();

	return(ReturnStatus);
} // RECEIVE_PROCESSOR
//---------------------------------------------------------------------------;



//---------------------------------------------------------------------------
#endif // HMODEMTRANSMITRECEIVECPP




