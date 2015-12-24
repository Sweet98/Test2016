//======================================================================================
// HTransferProcessing.h
//======================================================================================
// $RCSfile: htransferprocessing.h $
// $Revision: 1.3 $
// $Date: 2010/03/19 17:39:58EDT $
//======================================================================================

//======================================================================================






#ifndef HTRANSFER_PROCESSINGH
#define HTRANSFER_PROCESSINGH
//---------------------------------------------------------------------------

#include	"HTagDataResult.h"
#include	"HTagFile.h"
#include	"HTagImage.h"
#include	"HTagReaderCode.h"
#include	"HTagScript.h"
#include	"HString.h"



//	IMPORTANT NOTE:
//		In general, we will use ProcessHTAGData for all new data types, and we may migrate existing types to use
//		this function, thereby eliminating many of the functions below.  The reason for this is two-fold: there
//		are too many classes needing to be overloaded AND sometimes the data type class wants to reference the
//		transfer processor, but it can't do that if the transfer processor classes in turn reference it.  The
//		original intent of using several classes was that the application using this code may want to provide an
//		overload for images in one place, scripts in another, etc.  This can still be achieved by having the
//		application provide it's own overload layer on top of this.



//---------------------------------------------------------------------------
class HTAG_POST_TRANSFER_PROCESSOR
{
public:
// Data types that may be returned via this class after an HTAG receive.
	typedef enum {
		VERIFIER_CALIBRATION_DATA = 0
	} HTAG_POST_TRANSFER_PROCESSOR_DATA_TYPES;

// The following functions are optional in order to support processing of data after transfer using a Matrix device.  It is
// the responsibility of the instantiator to override these, which requires that they use this class as a base class.
// Note that in general, we will use ProcessHTAGData for all new data types, and we may migrate existing types to use
// this function, thereby eliminating many of the functions below.
	virtual void ProcessHTAGData( int * /* Status */, char * /* Result */, int /* DataType */, void * /* NewHTAGData */)   { return; }

// Allow extraneous data to be processed.  This is primarily used when a transfer is completed and we're waiting for a final
// status byte, but would like to display any debug messages that occur while we're waiting.
	virtual void ProcessExtraneousDataReceivedDuringOrAfterTransfer( HSTRING * /* ExtraneousData */ )   { return; }



public:		// User declarations

// Contstructor for HTAG_POST_TRANSFER_PROCESSOR.
//  Prepare for post-transfer data processing.
	HTAG_POST_TRANSFER_PROCESSOR(void);

// Destructor for HTAG_POST_TRANSFER_PROCESSOR.
    ~HTAG_POST_TRANSFER_PROCESSOR(void);
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
class HTAG_IMAGE_POST_TRANSFER_PROCESSOR
{
public:
// The following functions are optional in order to support processing of an image after transfer using a Matrix device.  It is
// the responsibility of the instantiator to override these, which requires that they use this class as a base class.
	virtual void ProcessImage(int * /* Status */, char * /* Result */, HTAG_IMAGE * /* NewImage */)   { return; }
	virtual void ProcessText(int * /* Status */, char * /* Result */, HSTRING * /* NewText */)   { return; }
	virtual void ProcessDecodeResult(int * /* Status */, char * /* Result */, HTAG_DECODE_RESULT * /* DecodeResult */)   { return; }
	virtual void ProcessDecodeResultHostTransmitAcknowledge
					(int * /* Status */, char * /* Result */, HTAG_DECODE_RESULT * /* DecodeResult */)   { return; }
	virtual void ProcessSystemInformationRequest(int * /* Status */, char * /* Result */,
					HTAG_SYSTEM_INFORMATION * /* SystemInformation */)   { return; }
	virtual void ProcessSystemInformation(int * /* Status */, char * /* Result */,
					HTAG_SYSTEM_INFORMATION * /* SystemInformation */)   { return; }
	virtual void ProcessMenuCommand(int * /* Status */, char * /* Result */,
					HTAG_MENU_COMMAND * /* MenuCommand */)   { return; }
	virtual void ProcessMenuCommandResponse(int * /* Status */, char * /* Result */,
					HTAG_MENU_COMMAND * /* MenuCommandResponse */)   { return; }
	virtual void ProcessConfigMenuCommand(int * /* Status */, char * /* Result */,
					HTAG_MENU_COMMAND * /* MenuCommand */)   { return; }
	virtual void ProcessConfigMenuCommandResponse(int * /* Status */, char * /* Result */,
					HTAG_MENU_COMMAND * /* MenuCommandResponse */)   { return; }
					
	virtual void ProcessNonMenuCommand(int * /* Status */, char * /* Result */,
					HTAG_MENU_COMMAND * /* NonMenuCommand */)   { return; }
	virtual void ProcessNonMenuCommandResponse(int * /* Status */, char * /* Result */,
					HTAG_MENU_COMMAND * /* NonMenuCommandResponse */)   { return; }
	virtual void ProcessActivityStatus(int * /* Status */, char * /* Result */,
					HTAG_ACTIVITY_STATUS * /* ActivityStatus */)   { return; }
	virtual void ProcessUnknownSYNFECommand( int * /* Status */, char * /* Result */, HSTRING * /* NameOfUnknownCommand */,
					HSTRING * /* OriginalData */, HSTRING * /* FormattedData */ )    { return; }	



public:		// User declarations

// Contstructor for HTAG_IMAGE_POST_TRANSFER_PROCESSOR.
//  Prepare for post-transfer image processing.
	HTAG_IMAGE_POST_TRANSFER_PROCESSOR(void);

// Destructor for HTAG_IMAGE_POST_TRANSFER_PROCESSOR.
    ~HTAG_IMAGE_POST_TRANSFER_PROCESSOR(void);
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
class HTAG_FILE_POST_TRANSFER_PROCESSOR
{
public:
// The following functions are optional in order to support processing a file after transfer using a Matrix device.  It is
// the responsibility of the instantiator to override these, which requires that they use this class as a base class.
	virtual void StoreFile(int * /* Status */, char * /* Result */, HTAG_FILE * /* NewFile */)   { return; }
	virtual void RetrieveFile(int * /* Status */, char * /* Result */, HTAG_FILE * /* File */)   { return; }



public:		// User declarations

// Contstructor for HTAG_FILE_POST_TRANSFER_PROCESSOR.
//  Prepare for post-transfer file processing.
	HTAG_FILE_POST_TRANSFER_PROCESSOR(void);

// Destructor for HTAG_FILE_POST_TRANSFER_PROCESSOR.
    ~HTAG_FILE_POST_TRANSFER_PROCESSOR(void);
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
class HTAG_SCRIPT_POST_TRANSFER_PROCESSOR
{
public:
// The following functions are optional in order to support processing a script after transfer using a Matrix device.  It is
// the responsibility of the instantiator to override these, which requires that they use this class as a base class.
	virtual void StoreHHPLMainScript(int * /* Status */, char * /* Result */, HTAG_SCRIPT * /* NewScript */)   { return; }
	virtual void RetrieveHHPLMainScript(int * /* Status */, char * /* Result */, HTAG_SCRIPT * /* Script */)   { return; }
	virtual void LoadAndRunHHPLMainScript(int * /* Status */, char * /* Result */, HTAG_SCRIPT * /* NewScript */)   { return; }
	virtual void StoreHHPLEditScript(int * /* Status */, char * /* Result */, HTAG_SCRIPT * /* NewScript */)   { return; }
	virtual void RetrieveHHPLEditScript(int * /* Status */, char * /* Result */, HTAG_SCRIPT * /* Script */)   { return; }
	virtual void LoadAndRunHHPLEditScript(int * /* Status */, char * /* Result */, HTAG_SCRIPT * /* NewScript */)   { return; }
	virtual	void PrepareForFileDownload(void) {return;}


public:		// User declarations

// Contstructor for HTAG_SCRIPT_POST_TRANSFER_PROCESSOR.
//  Prepare for post-transfer script processing.
	HTAG_SCRIPT_POST_TRANSFER_PROCESSOR(void);

// Destructor for HTAG_SCRIPT_POST_TRANSFER_PROCESSOR.
    ~HTAG_SCRIPT_POST_TRANSFER_PROCESSOR(void);
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
class HTAG_SCRIPT_HOST_COMMAND_POST_TRANSFER_PROCESSOR
{
public:
// The following functions are optional in order to support processing a script host command after transfer using a Matrix
// device.  It is the responsibility of the instantiator to override these, which requires that they use this class as a base
// class.
	virtual void HHPLSendHostCommand(int * /* Status */, char * /* Result */, HTAG_SCRIPT_HOST_COMMAND * /* NewScriptHostCommand */)   { return; }



public:		// User declarations

// Contstructor for HTAG_SCRIPT_HOST_COMMAND_POST_TRANSFER_PROCESSOR.
//  Prepare for post-transfer script host command processing.
	HTAG_SCRIPT_HOST_COMMAND_POST_TRANSFER_PROCESSOR(void);

// Destructor for HTAG_SCRIPT_HOST_COMMAND_POST_TRANSFER_PROCESSOR.
    ~HTAG_SCRIPT_HOST_COMMAND_POST_TRANSFER_PROCESSOR(void);
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
class HTAG_SCRIPT_HOST_DATA_POST_TRANSFER_PROCESSOR
{
public:
// The following functions are optional in order to support processing script host data after transfer using a Matrix
// device.  It is the responsibility of the instantiator to override these, which requires that they use this class as a base
// class.
	virtual void HHPLSendHostData(int * /* Status */, char * /* Result */, HTAG_SCRIPT_HOST_DATA * /* NewScriptHostData */)   { return; }



public:		// User declarations

// Contstructor for HTAG_SCRIPT_HOST_DATA_POST_TRANSFER_PROCESSOR.
//  Prepare for post-transfer script host data processing.
	HTAG_SCRIPT_HOST_DATA_POST_TRANSFER_PROCESSOR(void);

// Destructor for HTAG_SCRIPT_HOST_DATA_POST_TRANSFER_PROCESSOR.
    ~HTAG_SCRIPT_HOST_DATA_POST_TRANSFER_PROCESSOR(void);
};
//---------------------------------------------------------------------------





class READER_DEVICE_EVENT_PROCESSOR
{
public:
// The following functions are optional in order to support processing of various reader events that we are notified of
// asynchronously.

	typedef enum {
		UNTRIGGERED = 0,
		MANUALLY_TRIGGERED,
		SERIALLY_TRIGGERED,
		AUTO_TRIGGERED,
		END_OF_TRIGGER_STATES
	} TRIGGER_STATE;

// This optional function is called when a trigger event is received from the reader device.  It specifies the type of trigger
// event that has been received (TRIGGER_EVENT). 
	virtual void TriggerEvent(int /* TriggeredState */ )  { return; }	

// This optional function is called when an unrecognized <SYN>FE command is received.
	virtual void UnknownSYNFECommandReceived( int * /* Status */, char * /* Result */, HSTRING * /* NameOfUnknownCommand */,
					HSTRING * /* ReceivedData */ )  { return; }	

public:		// User declarations

// Contstructor for READER_DEVICE_EVENT_PROCESSOR.
//  Prepare for reader device event processing.
	READER_DEVICE_EVENT_PROCESSOR(void);

// Destructor for READER_DEVICE_EVENT_PROCESSOR.
    ~READER_DEVICE_EVENT_PROCESSOR(void);
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
class HTAG_CODE_POST_TRANSFER_PROCESSOR
{
public:
// The following functions are optional in order to support processing of code after transfer using a Matrix device.  It is
// the responsibility of the instantiator to override these, which requires that they use this class as a base class.
	virtual void StoreApplicationCode(int * /* Status */, char * /* Result */, HTAG_READER_CODE * /* NewApplicationCode */)   { return; }
	virtual void RetrieveApplicationCode(int * /* Status */, char * /* Result */, HTAG_READER_CODE * /* ApplicationCode */)   { return; }
	virtual void StoreBootCode(int * /* Status */, char * /* Result */, HTAG_READER_CODE * /* NewBootCode */)   { return; }
	virtual void RetrieveBootCode(int * /* Status */, char * /* Result */, HTAG_READER_CODE * /* BootCode */)   { return; }
	virtual void StoreFlashImage(int * /* Status */, char * /* Result */, HTAG_READER_CODE * /* NewFlashImage */)   { return; }
	virtual void RetrieveFlashImage(int * /* Status */, char * /* Result */, HTAG_READER_CODE * /* FlashImage */)   { return; }
	virtual void StoreScanEngineFirmware(int * /* Status */, char * /* Result */,
				HTAG_SCAN_ENGINE_FIRMWARE * /* NewScanEngineFirmware */)   { return; }

	virtual	void PrepareForFileDownload(void) {return;} // Called when we know the incoming SYN FE command involves a file download


public:		// User declarations

// Contstructor for HTAG_CODE_POST_TRANSFER_PROCESSOR.
//  Prepare for post-transfer code processing.
	HTAG_CODE_POST_TRANSFER_PROCESSOR(void);

// Destructor for HTAG_CODE_POST_TRANSFER_PROCESSOR.
    ~HTAG_CODE_POST_TRANSFER_PROCESSOR(void);
};
//---------------------------------------------------------------------------



#endif // HTRANSFER_PROCESSINGH





