//======================================================================================
// HModem.h
//======================================================================================
// $RCSfile: hmodem.h $
// $Revision: 1.2 $
// $Date: 2011/11/09 15:24:21EST $
//======================================================================================

//======================================================================================






//---------------------------------------------------------------------------
//
//  Class definitions for HMODEM
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
#ifndef HMODEMH
#define HMODEMH
//---------------------------------------------------------------------------

// Set this flag so that when we include HModemTransmitReceive within the class definition,
// the global variables defined in that file will be incorporated as member variables here.
#define		HMODEM_TRANSMIT_RECEIVE_INCLUDE_VARIABLE_DEFINITIONS		1


#include	"Compression.h"
#include	"HComm.h"
#include	"HCommandHandler.h"
#include	"HString.h"
#include	"HTransferStatus.h"


//---------------------------------------------------------------------------
class HMODEM
{
public:

// Include the global control variables for HModem transfers here as public member variables.
#include	"HModemTransmitReceive.h"

private:
	HCOMM				*Device;			// Device that we're talking to.
	HTRANSFER_STATUS	*TransferStatus;	// Mechanism for status feedback
	HCOMMAND_HANDLER	*CommandHandler;	// Mechanism for issuing wakeup commands.

    


public:		// User declarations




	HSTRING		*StatusMessage;





// Contstructor for HMODEM.
//  Prepare for an HMODEM transmit or receive.
	HMODEM(HCOMM *HCommDevice, HTRANSFER_STATUS *InputTransferStatus, HCOMMAND_HANDLER *InputCommandHandler);

// Destructor for HMODEM.
    ~HMODEM(void);

// Request transmit of data to a remote receiver via HModem protocol (overloaded version #1 using HSTRING input).
	unsigned int Transmit(HSTRING *Data, HSTRING *InitiatingCommand, bool WaitForSecondACK);

// Request transmit of data to a remote receiver via HModem protocol (overloaded version #2 using unsigned char* input).
	unsigned int Transmit(unsigned char *Data, unsigned int DataLength, HSTRING *InitiatingCommand, bool WaitForSecondACK);

// Request transmit of data to a remote receiver via HModem protocol using HSTRING input, sending the
// buffer size as part of the transfer based on caller preference (overloaded version #3).
	unsigned int Transmit(HSTRING *Data, HSTRING *InitiatingCommand, bool WaitForSecondACK, bool TransmitHeader);

// Request transmit of data to a remote receiver via HModem protocol using unsigned char* input, sending the
// buffer size as part of the transfer based on caller preference (overloaded version #4).
	unsigned int Transmit(unsigned char *Data, unsigned int DataLength, HSTRING *InitiatingCommand, bool WaitForSecondACK, bool TransmitHeader);

// Request transmit of data to a remote receiver via HModem protocol (overloaded version #5 using HSTRING input and
// caller requested compression mode).
	unsigned int Transmit(HSTRING *Data, HSTRING *InitiatingCommand, bool WaitForSecondACK, unsigned int CompressionMode);

// Request transmit of data to a remote receiver via HModem protocol (overloaded version #6 using unsigned char* input and
// caller requested compression mode).
	unsigned int Transmit(unsigned char *Data, unsigned int DataLength, HSTRING *InitiatingCommand, bool WaitForSecondACK, unsigned int CompressionMode);

// Request transmit of data to a remote receiver via HModem protocol using HSTRING input, sending the
// buffer size as part of the transfer based on caller preference (overloaded version #7, using
// caller requested compression mode).
	unsigned int Transmit(HSTRING *Data, HSTRING *InitiatingCommand, bool WaitForSecondACK, bool TransmitHeader,
                                                                        unsigned int CompressionMode);

// Request transmit of data to a remote receiver via HModem protocol using unsigned char* input, sending the
// buffer size as part of the transfer based on caller preference (overloaded version #8, using
// caller requested compression mode).
	unsigned int Transmit(unsigned char *Data, unsigned int DataLength, HSTRING *InitiatingCommand, bool WaitForSecondACK, bool TransmitHeader,
                                                                        unsigned int CompressionMode);

// Set the maximum number of retries to a caller specified value.
    void SetMaximumNumberOfRetries(unsigned int NewMaximumNumberOfRetries);

// Set the number of seconds to wait for an initial response from the other side.
    void SetInitialResponseWaitPeriod(unsigned int NewInitialResponseWaitPeriod);

// Request receive data from a remote transmitter via HModem protocol (overloaded version #1 using return data callback).
    unsigned int Receive(unsigned int MaximumNumberOfBytesToReceive,
                                        bool (*ReceivedDataCharacters) (HSTRING *));

// Request receive data from a remote transmitter via HModem protocol (overloaded version #2 using HSTRING return buffer).
	unsigned int Receive(HSTRING *ReceiveBuffer, unsigned int MaximumNumberOfBytesToReceive);

// Request receive data from a remote transmitter via HModem protocol (overloaded version #3 using unsigned char* return buffer).
	unsigned int Receive(unsigned char *ReceiveBuffer, unsigned int *NumberOfBytesInReceiveBuffer, unsigned int MaximumNumberOfBytesToReceive);

// Set receive timeout in milliseconds.
	void SetReceiveTimeout(unsigned int NewReceiveTimeout);

// Set receive timeout based on baud rate.
	void SetReceiveTimeoutByBaudRate(unsigned int BaudRate);

// Set time to wait for acknowledgement of a packet.
	void SetWaitForPacketACKTimeout(unsigned int NewWaitForPacketACKTimeout);

// Set time to wait for a final ACK response at the end of the transfer.
	void SetWaitForFinalACKTimeout(unsigned int NewWaitForFinalACKTimeout);
	
private:
	HMODEM (const HMODEM&);            // declared private, but not defined, to prevent copying         
	HMODEM& operator=(const HMODEM&);  // declared private, but not defined, to prevent copying 
	
};
//---------------------------------------------------------------------------
#endif





