//======================================================================================
// HModemTransmitReceive.h
//======================================================================================
// $RCSfile: HModemTransmitReceive.h $
// $Revision: 1.2 $
// $Date: 2009/04/24 17:29:42EDT $
//======================================================================================

//======================================================================================






//---------------------------------------------------------------------------
//
//  Interface definitions for HMODEM transmit and receive routines
//
//---------------------------------------------------------------------------

#ifndef HMODEMTRANSMITRECEIVEH
#define HMODEMTRANSMITRECEIVEH
//---------------------------------------------------------------------------

#include	"Compression.h"
#include	"HString.h"



// Default timing definitions
	#define RECEIVE_TIMEOUT                    200				// Default number of milliseconds to wait for data on the input
                                                                		//  port.
	#define MAXIMUM_RECEIVE_TIMEOUT            2000                 // Maximum number of milliseconds to wait for data on the input
                                                                				//  port.
	#define	SLEEP_AFTER_TERMINATE				RECEIVE_TIMEOUT * 2 // Number of milliseconds to wait after terminating
                                                                			//  download prematurely.

	#define DEFAULT_MAXIMUM_NUMBER_OF_RETRIES   3		// Default maximum number of times to try to receive an xmodem packet

	#define DEFAULT_INITIAL_RESPONSE_TIME       10      // Default number of seconds to wait for response from the other side.

	#define	DEFAULT_WAIT_FOR_PACKET_ACK			6		// Number of seconds to wait for a packet ACK response.

	#define	DEFAULT_WAIT_FOR_FINAL_ACK_TIMEOUT	60		// Number of seconds to wait for a final ACK response at the
																					//	end of the transfer.


#ifdef	HMODEM_TRANSMIT_RECEIVE_INCLUDE_VARIABLE_DEFINITIONS
// Only define transmit status / tracking variables if this is being included in the actual HMODEM source code.  No one else
// needs to see these.
	bool	TransmitPacketAhead;						// Always transmit one packet ahead of the one we're waiting for a response on.
	bool	CancelTransmitPacketAheadOnRetry;			// If a retry occurs during transmission, cancel TransmitPacketAhead mode.
	bool	Transmit128BytePacketsOnly;					// Force packet size to 128 bytes.
	bool	SwitchTo128BytePacketsOnRetry;				// If a retry occurs during transmission, force to 128 byte packets.


    unsigned int	ReceiveTimeout;                         // Receive timeout used while trying to read data.



    unsigned int	MaximumNumberOfRetries;                             // Initiator configurable maximum number of retries
                                                                				//  (configured via SetMaximumNumberOfRetries).

    unsigned int	SecondsToWaitForInitialResponse;            // Time, in seconds, to wait for an initial response from the
                                                        					//  other side.

    unsigned int	NumberOfBytesToBeSentByTransmitter;         // Number of bytes to expect from the transmitter.

	unsigned int	WaitForPacketACKTimeout;					// Time, in seconds, to wait for acknowledgement of a packet.

	unsigned int	WaitForFinalACKTimeout;						// Time, in seconds, to wait for a final ACK response at the
																					//	end of the transfer.
#endif


// HMODEM routine interface definitions.

// Setup HMODEM defaults.
	void InitializeHModemSession( void );


// Transmit data to a remote receiver via HModem protocol.
	unsigned int TransmitProcessor(unsigned char *ByteData, unsigned int ByteDataLength, unsigned char *InitiatingCommand,
											unsigned int InitiatingCommandSize,	bool WaitForSecondACK, bool TransmitHeader, unsigned int CompressionMode);

// Receive data from a remote transmitter via HModem protocol.
    unsigned int ReceiveProcessor(unsigned char *ByteBuffer, unsigned int *ByteBufferLength, HSTRING *HstringBuffer,
											unsigned int MaximumNumberOfBytesToReceive, bool (*ReceivedDataCharacters) (HSTRING *));



    void TerminateHModemSession(void);


    unsigned int WaitForACK(unsigned int NumberOfSecondsToWait, unsigned int NumberOfBytesBeingTransmitted);



// Feature flags
	typedef enum {
		SUPPORT_OF_DOUBLE_EOT		= 1
	} FEATURE_FLAGS;

#ifdef	HMODEM_TRANSMIT_RECEIVE_INCLUDE_VARIABLE_DEFINITIONS
// Only define packet tags if this is being included in the actual HMODEM source code.  No one else needs to see these.

//
// Tag Definitions
//
//	Tags consist of 32 bits where the first 16 bits are the tag number and the second 16 bits are a length field.
//	Tag groups (such as size fields), should be grouped on boundaries of 100.  The length field represents the
//	number of bytes in the field.
//
//  NOTE: Avoid tag zero (0).  Packets may be zero padded, so using a tag of zero would be a mistake.
//
// Define sizes
    #define     SIZE_OF_TAG_NUMBER                      2
    #define     SIZE_OF_TAG_LENGTH                      2
//		Size Group
    #define		TransmitBufferSizeTag                   100
    #define     SizeOfUncompressedDataTag               101
//
//		Compression Group
    #define		CompressionModeTag  					200
//
//		Feature Group
    #define		FeatureFlagTag  						300



// Block size / type definitions
    #define     HEADER_BLOCK_128_BYTE                   0
    #define     DATA_BLOCK_128_BYTE                     1
    #define     DATA_BLOCK_1024_BYTE                    2
    #define     RECOVERY_HEADER_BLOCK_BYTE              3

#endif



// HMODEM transmit and receive return status codes
    typedef enum  {
		BAD_INPUT_DATA_POINTER=0,
        NO_DATA_RECEIVED,
        DATA_RECEIVED,
        FINISH_RECEIVED,
		FINISH_SUCCESSFUL,
		RECEIVER_NOT_RESPONSIVE_TO_FINISH,
		RECEIVER_NAKED,
		RECEIVER_IS_UNRESPONSIVE,
		RECEIVER_NOT_READY,
        TERMINATE_BY_TRANSMITTER,
        TERMINATE_RECEIVED,
        TERMINATE_BY_US,
        TIMEOUT_WAITING_FOR_HMODEM_BLOCK_NUMBER,
        MISMATCHED_HMODEM_BLOCK_NUMBER,
        TIMEOUT_WAITING_FOR_HMODEM_INVERSE_BLOCK_NUMBER,
        MISMATCHED_HMODEM_INVERSE_BLOCK_NUMBER,
        TIMEOUT_WAITING_FOR_HMODEM_BLOCK_SIZE,
        ILLEGAL_HMODEM_BLOCK_SIZE,
        TIMEOUT_WAITING_FOR_HMODEM_BLOCK,
        TIMEOUT_WAITING_FOR_HMODEM_CRC,
        MISMATCHED_HMODEM_CRC,
        UNRECOGNIZED_COMPRESSION_MODE,
        TRANSMIT_DATA_TOO_LARGE,
        HEADER_NOT_ACKNOWLEDGED,
        ACK_RECEIVED,
        NAK_RECEIVED,
        TIMEOUT_WAITING_FOR_ACK
    } PACKET_RETURN_STATUS;

//---------------------------------------------------------------------------
#endif // HMODEMTRANSMITRECEIVEH





