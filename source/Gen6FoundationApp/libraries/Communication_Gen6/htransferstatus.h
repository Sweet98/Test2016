//======================================================================================
// HTransferStatus.h
//======================================================================================
// $RCSfile: htransferstatus.h $
// $Revision: 1.2 $
// $Date: 2011/11/11 17:01:42EST $
//======================================================================================

//======================================================================================






//---------------------------------------------------------------------------
//
//  Class definitions for HTRANSFER_STATUS
//
//---------------------------------------------------------------------------
#ifndef HTRANSFER_STATUSH
#define HTRANSFER_STATUSH
//---------------------------------------------------------------------------

#include	"HWString.h"

#ifndef HTRANSFER_STATUS_API	            // Only if it has not been defined yet
#ifdef WIN32
//#if (WIN32 || _WIN32_WCE)
#ifdef WINAPP
	    #define HTRANSFER_STATUS_API
	#else
		#if (_USRDLL || DLL)
		  #define HTRANSFER_STATUS_API __declspec(dllexport)
		#else
		  #define HTRANSFER_STATUS_API __declspec(dllimport)
		#endif
	#endif
#else
	#ifdef _WIN32_WCE
	    #if (_USRDLL || DLL)
    	    #define HTRANSFER_STATUS_API __declspec(dllexport)
	    #else
    	    #define HTRANSFER_STATUS_API __declspec(dllimport)
	    #endif
	#else
	    #define HTRANSFER_STATUS_API
	#endif
#endif
#endif	                                 // ifndef HTRANSFER_STATUS_API

//---------------------------------------------------------------------------
class HTRANSFER_STATUS_API HTRANSFER_STATUS
{
public:


	// When a data transfer is about to take place, TransferType will be loaded with the appropriate TRANSFER_TYPE value so that the
	// instantiator can tell what is being sent or received.  After the data transfer is complete, TransferType will be set to
	// IDLE.

	typedef enum {
	    IDLE = 0,
	    STORE_APPLICATION_CODE,
	    RETRIEVE_APPLICATION_CODE,
	    STORE_BOOT_CODE,
	    RETRIEVE_BOOT_CODE,
	    PROCESS_IMAGE,
	    SEND_IMAGE_FOR_PROCESSING,
	    STORE_SCRIPT,
	    RETRIEVE_SCRIPT,
	    LOAD_AND_RUN_SCRIPT,
	    HOST_COMMAND,
	    HOST_DATA,
	    STORE_FILE,
	    RETRIEVE_FILE,
	    STORE_FLASH_IMAGE,
	    RETRIEVE_FLASH_IMAGE,
	    PROCESS_JPEG_IMAGE,
	    SENDING_DECODE_RESULT,
	    RECEIVING_DECODE_RESULT,
	    SENDING_DECODE_RESULT_ACKNOWLEDGE,
	    RECEIVING_DECODE_RESULT_ACKNOWLEDGE,
	    SENDING_SYSTEM_INFORMATION_REQUEST,
	    RECEIVING_SYSTEM_INFORMATION_REQUEST,
	    SENDING_SYSTEM_INFORMATION,
	    RECEIVING_SYSTEM_INFORMATION,
	    SENDING_MENU_COMMAND,
	    RECEIVING_MENU_COMMAND,
	    SENDING_MENU_COMMAND_RESPONSE,
	    RECEIVING_MENU_COMMAND_RESPONSE,
	    SENDING_NON_MENU_COMMAND,
	    RECEIVING_NON_MENU_COMMAND,
	    SENDING_NON_MENU_COMMAND_RESPONSE,
	    RECEIVING_NON_MENU_COMMAND_RESPONSE,
	    SENDING_ACTIVITY_STATUS,
	    RECEIVING_ACTIVITY_STATUS,
	    FLASHING_APPLICATION_CODE,
	    SENDING_APPLICATION_CODE_TO_RF_SCANNER,
	    STORE_ENGINE_FIRMWARE,
	    SENDING_VERIFIER_CALIBRATION_DATA,
	    RECEIVING_VERIFIER_CALIBRATION_DATA
	} TRANSFER_TYPE;

	int		TransferType;

	HSTRING	*DeviceName;


public:
	// The following functions are optional in order to support HTRANSFER_STATUS.  It is the responsibility of the
	// instantiator to override these, which requires that they use this class as a base class.

	// Instantiator may need to prepare a dialog or other status information prior to a data transfer.  This routine will be called
	// by a data transfer routine in the communication library as it is about to begin a transfer.  The TransferType variable will
	// be loaded with the appropriate TRANSFER_TYPE value.
	virtual void PrepareDownloadStatus( HWSTRING * /* DeviceName */ )
	{
		return;
	};
	virtual void PrepareDownloadStatus(void)
	{
		HWSTRING	DeviceName( 2, true );
		PrepareDownloadStatus( &DeviceName );
		return;
	};
	virtual void UpdateDownloadStatus( HWSTRING * /* DeviceName */ )
	{
		return;
	};

	// This routine is called by the data transfer routine to let the instantiator know that the data transfer is completed, and if
	// there is any cleanup to do, such as closing a dialog box, then the instantiator can do it here.  The TransferType variable
	// will still be loaded with the appropriate TRANSFER_TYPE value; it will be cleared by the data transfer routine after this
	// this routine is called, but before the data transfer routine exits.
	virtual void CloseDownloadStatus(void)
	{
		return;
	};

	// This routine is called by the data transfer routine throughout the data transfer.  It tells the instantiator how much progress
	// has been made.  TotalBytesToBeSent is the total number of bytes to be sent.  NumberOfBytesSentSinceLastUpdate is the number
	// of bytes that have been sent since the last call of MayContinue by the data transfer routine.  When this routine is called
	// by the data transfer routine, the instantiator should return true if the data transfer should be allowed to continue or return
	// false if the data transfer should be terminated, due to user cancellation, errors, or any other reason that the instantiator
	// may decide to terminate the data transfer.  Note that TotalBytesToBeSent may change from the value that was sent in the first
	// call to MayContinue, particularly when receiving data, where the first call may contain the number of bytes we expect to
	// receive and some subsequent call may contain a corrected number specified by the transmitter.  Note that because the progress
	// is reported as the number of bytes since the last call, it is the responsibility of the instantiator to keep track of the
	// total number sent, which may be corrected by SetProgressLevel (see definition of that routine below).  The reason for this is
	// that MayContinue is called at times when we're waiting for status information from the device; in this situation, no new data
	// will be reported and by looking at NumberOfBytesSentSinceLastUpdate and seeing that it is zero, the instantiator can skip
	// an update of the progress indicator, which otherwise will be flashing with lots of updates that aren't really updates.  If I
	// had it to do over again, I would have reported total bytes sent instead and left it up to the instantiator to determine if
	// there was any change that needed to be reported, but this is used all over the place now, so I'm not changing it.
	//
	// If CheckForTerminateConditionOnly to true, the instantiator implementation of MayContinue should not update the UI, but
	// should only indicate whether or not it is OK to continue.
	virtual bool MayContinue(unsigned int /* NumberOfBytesSentSinceLastUpdate */, unsigned int /* TotalBytesToBeSent */,
	                         bool /* CheckForTerminateConditionOnly */ )
	{
		return(true);
	}
	virtual bool MayContinue(unsigned int NumberOfBytesSentSinceLastUpdate, unsigned int TotalBytesToBeSent )
	{
		return( MayContinue( NumberOfBytesSentSinceLastUpdate, TotalBytesToBeSent, false ));
	}

	// DownloadStartingHandler is called by a data transfer routine immediately before it is to begin a protocol transfer, such as
	// HModem.  DownloadFinishHandler is called immediately after the conclusion of a protocol transfer.  The intended purpose of
	// these is to let the instantiator know when the data transfer routine is actually talking to a device as part of the protocol
	// transfer, so that the instantiator, if it is monitoring device communication traffic asynchronously, it will know that the
	// message traffic is associated with the protocol data transfer.  If, for example, the instantiator normally assumes that all
	// data received is barcode data and is therefore displayed in the background to a text window, it will know enough to ignore
	// data received during this time.
	virtual void DownloadStartingHandler(void)
	{
		return;
	}
	virtual void DownloadFinishHandler(void)
	{
		return;
	}

	// During a protocol data transfer, the instantiator may wish to "see" message traffic from the device for debugging or other
	// purposes.  This routine is called during data transfer to feed the latest received data to the instantiator.
	virtual void ReceivedProtocolCharacters(HSTRING * /* ReceivedCharacters */)
	{
		return;
	}

	// During a protocol data transfer, the instantiator may wish to get feedback when a retry has occurred.  This routine is called
	// during data transfer to indicate that a packet retyr has occurred and a status string is provided with specific information
	// about the nature of the retry.
	virtual void RetryWarning(HSTRING * /* RetryStatusMessage */)
	{
		return;
	}

	// Certain operations, such as the HHPReaderDevice class WriteMatrixApplication and WriteMatrixBoot, call this function to let
	// the instantiator know what action is currently taking place (validating file, waiting for completion, etc.).  See the
	// definition of the particular operation, such as WriteMatrixApplication and WriteMatrixBoot, to see if this function is used.
	virtual void DownloadStep(int /* DownloadStepNumber */)
	{
		return;
	};

	// When menu or non-menu commands are sent to a device and the command handler is waiting for a response, WaitingForResponse is
	// called at the outset and FinishedWaitingForResponse is called after a response is received or an error has occurred.  The
	// purpose of these is to allow the instantiator to display a dialog, wait cursor, etc. and to lock out user input if desired.
	virtual void WaitingForResponse(void)
	{
		return;
	};
	virtual void FinishedWaitingForResponse(void)
	{
		return;
	};

	// This routine is called by the data transfer routine whenever the number of bytes received needs to be corrected, due to having
	// to back up because of retries, etc.  If the instantiator is using MayContinue to report progress, then it also needs to
	// implement this routine in order to correct the number of bytes sent.  Note in the first method that if TotalBytesToBeSent is
	// -1, then the range hasn't changed and if DeviceName is NULL, it is unused.
	virtual void SetProgressLevel( int /* ProgressLevel */, int /* TotalBytesToBeSent */ )
	{
		return;
	}
	virtual void SetProgressLevel(int ProgressLevel)
	{
		SetProgressLevel( ProgressLevel, -1 );
		return;
	}


	// HHPReaderDevice supports, with the help of the external device handler, the ability to read asynchronous data and to
	// determine if it is text, image, etc.  The external device handler needs to be told during certain activities, such as
	// sending commands and waiting for responses, that it should not try to read the data asynchronously, as the data might
	// be read before the command handler has a chance to look at it.  This semaphore allows the command handlers and other entities
	// to tell an external device handler to ignore (not read) data received asynchronously.
	int		ActivityCounter;

	// Indicate that there is something else wanting to grab received data in place of ProcessReceivedData, then
	// tell the instantiator that the semaphore changed.
	void IncrementProcessReceivedDataLockSemaphore(void);

	// Indicate that there is one less process wanting to grab received data in place of ProcessReceivedData, then
	// tell the instantiator that the semaphore changed.
	void DecrementProcessReceivedDataLockSemaphore(void);

	// Return the current value of ProcessReceivedDataLockSemaphore to the caller.
	int GetProcessReceivedDataLockSemaphore(void);
	int GetActivityCounter(void);

	// Virtual function that may be overloaded by the instantiator so that it is told when the state of the
	// ProcessReceivedDataLockSemaphore has been changed.
	virtual void ReceivedDataLockSemaphoreWasChanged( void )
	{
		return;
	};


	// This optional function is called when we've reidentified a device.
	virtual void DeviceDetected( void )
	{
		return;
	}




public:		// User declarations

	// Contstructor for HTRANSFER_STATUS.
	//  Prepare for an HTRANSFER_STATUS transfer.
	HTRANSFER_STATUS(void);

	// Destructor for HTRANSFER_STATUS.
	~HTRANSFER_STATUS(void);
	
private:
	HTRANSFER_STATUS (const HTRANSFER_STATUS&);            // declared private, but not defined, to prevent copying         
	HTRANSFER_STATUS& operator=(const HTRANSFER_STATUS&);  // declared private, but not defined, to prevent copying 

};
//---------------------------------------------------------------------------



#endif // HTRANSFER_STATUSH





