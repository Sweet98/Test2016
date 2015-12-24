//======================================================================================
// SYNFECommandHandler.h
//======================================================================================
// $RCSfile: Matrix/synfecommandhandler.h $
// $Revision: 1.5 $
// $Date: 2011/11/09 15:24:23EST $
//======================================================================================

//======================================================================================






//---------------------------------------------------------------------------
//
//  Class definitions for SYN_FE_COMMAND_HANDLER
//
//---------------------------------------------------------------------------
#ifndef SYN_FE_COMMAND_HANDLER_H
#define SYN_FE_COMMAND_HANDLER_H
//---------------------------------------------------------------------------

//
//	The SYN_FE_COMMAND_HANDLER class provides a mechanism to receive <SYN>FE type asynchronous commands from a
//	device.  All received data is wrapped in the <SYN>FE HTAG format.  The following data types can be received:
//		1) Scripts - actual script code defined in HTAG_SCRIPT.
//		2) Host commands - HHPL host commands that are to be processed by a script, as defined in HTAG_SCRIPT_HOST_COMMAND.
//		3) Host data - HHPL host generated data that is to be processed by a script, as defined in HTAG_SCRIPT_HOST_DATA.
//
//	The principle use of this class is to call the ReceiveAndParse function.  The argument to ReceiveAndParse is the
//	data that has been received so far to start a <SYN>FE command.  If the string is NULL, ReceiveAndParse will wait
//	for WaitTimeForCommand milliseconds to receive command data.  Otherwise, the string contains the start of a
//	<SYN>FE command and ReceiveAndParse must receive and process the rest of the data.
//



#include	"Compression.h"				// Compression type definitions
#include	"HComm.h"					// Abstraction for device communication
#include	"HTag.h"					// General protocol wrapped command handling.
#include	"HTagFile.h"				// Protocol wrapped command handling for files.
#include	"HTagImage.h"				// Protocol wrapped command handling for images
#include	"HTagReaderCode.h"			// Protocol wrapped command handling for code
#include	"HTagScript.h"				// Protocol wrapped command handling for scripts
#include	"HTransferProcessing.h"		// Abstraction for protocol wrapped image and script transfer post-processing.
#include	"HTransferStatus.h"			// Abstraction for protocol wrapped command status feedback



#include	"wchar.h"









class SYN_FE_COMMAND_HANDLER
{
public:	// User declarations
	HCOMM												*Device;				// Device that we're talking to.
	HTRANSFER_STATUS									*TransferStatus;		// Mechanism for status feedback
	HCOMMAND_HANDLER									*CommandHandler;		// Mechanism for sending initating <SYN>M and <SYN>N commands.
	HTAG_POST_TRANSFER_PROCESSOR						*PostTransferProcessor;	// Mechanism for post-processing transferred HTAG data.
	HTAG_IMAGE_POST_TRANSFER_PROCESSOR					*ImagePostProcessor;	// Mechanism for post-processing transferred images.
	HTAG_FILE_POST_TRANSFER_PROCESSOR					*FilePostProcessor;		// Mechanism for post-processing transferred files.
	HTAG_SCRIPT_POST_TRANSFER_PROCESSOR					*ScriptPostProcessor;	// Mechanism for post-processing transferred scripts.
	HTAG_SCRIPT_HOST_COMMAND_POST_TRANSFER_PROCESSOR	*HostCommandProcessor;	// Mechanism for post-processing host commands.
	HTAG_SCRIPT_HOST_DATA_POST_TRANSFER_PROCESSOR		*HostDataProcessor;		// Mechanism for post-processing host data.
	READER_DEVICE_EVENT_PROCESSOR						*EventProcessor;		// Mechanism for post-processing reader events.
	HTAG_CODE_POST_TRANSFER_PROCESSOR					*CodePostProcessor;		// Mechanism for post-processing transferred code.


public:

// Status codes
	typedef enum {
		SYN_FE_COMMAND_HANDLER_STORE_OPERATION_FAILED = HTAG::HTAG_USER_DEFINED_STATUS_BEGIN,
		SYN_FE_COMMAND_HANDLER_LOAD_AND_RUN_OPERATION_FAILED,
		SYN_FE_COMMAND_HANDLER_HOST_COMMAND_OPERATION_FAILED,
		SYN_FE_COMMAND_HANDLER_HOST_DATA_OPERATION_FAILED,
		SYN_FE_COMMAND_HANDLER_MISSING_OR_INCOMPLETE_FILE_NAME,
		SYN_FE_COMMAND_HANDLER_FILE_NOT_FOUND,
		SYN_FE_COMMAND_HANDLER_NOT_IN_APPROPRIATE_BOOT_MODE,
		SYN_FE_COMMAND_HANDLER_NOT_IN_APPROPRIATE_FLASH_IMAGE_MODE,
		SYN_FE_COMMAND_HANDLER_CORRUPT_IMAGE,
		SYN_FE_COMMAND_HANDLER_ADDRESS_OUT_OF_RANGE,
		SYN_FE_COMMAND_HANDLER_MODE_SWITCH_FAILED
	} SYN_FE_COMMAND_HANDLER_STATUS;


public:
	#define	HTAG_TRIGGER_EVENT_TYPE_PARAMETER_NAME		"T"


public:
	HSTRING		*UnusedCommandData;		// When we think we're receiving a <SYN>FE command, because we've received
										//	the <SYN> and 0xFE characters, we won't know for sure until we get the
										//	<CR> character after the length.  In case this wasn't really a <SYN>FE
										//	command, we want to save any data we read to a buffer so that a caller
										//	to the receive handler can see it.



private:
	#define	SPECIAL_MODE_KEY					-1891871527

private:
	typedef enum {
		IDLE_BOOT_CODE_MODE = 0,
		SETBOOTCODEMODE_BOOT_CODE_MODE,
		SETBOOTCODESTOREMODE_BOOT_CODE_MODE,
		SETBOOTCODERETRIEVEMODE_BOOT_CODE_MODE
	} BOOT_CODE_MODE;

	int			BootCodeMode;			// Per the explanation at the beginning of the code file (.CPP), we track the mode
	int			LastBootCodeMode;		//	between commands and reset it if it hasn't changed.



private:
	typedef enum {
		IDLE_FLASH_IMAGE_MODE = 0,
		SETFLASHIMAGEMODE_FLASH_IMAGE_MODE,
		SETFLASHIMAGESTOREMODE_FLASH_IMAGE_MODE,
		SETFLASHIMAGERETRIEVEMODE_FLASH_IMAGE_MODE
	} FLASH_IMAGE_MODE;

	int			FlashImageMode;			// Per the explanation at the beginning of the code file (.CPP), we track the mode
	int			LastFlashImageMode;		//	between commands and reset it if it hasn't changed.



private:
// Send the boot mode commands to turn on boot mode.
	bool TurnOnBootMode( char *Result, bool RetrieveMode );

// Send the flash image mode commands to turn on flash image mode.
	bool TurnOnFlashImageMode( char *Result, bool RetrieveMode );


public:
// Constructor for SYN_FE_COMMAND_HANDLER
    SYN_FE_COMMAND_HANDLER(HCOMM *HCommDevice, HTRANSFER_STATUS *InputTransferStatus,
						HCOMMAND_HANDLER *InputCommandHandler,
						HTAG_POST_TRANSFER_PROCESSOR *InputPostTransferProcessor,
						HTAG_IMAGE_POST_TRANSFER_PROCESSOR *InputImagePostProcessor,
						HTAG_SCRIPT_POST_TRANSFER_PROCESSOR *InputScriptPostProcessor,
						HTAG_SCRIPT_HOST_COMMAND_POST_TRANSFER_PROCESSOR *InputHostCommandProcessor,
						HTAG_SCRIPT_HOST_DATA_POST_TRANSFER_PROCESSOR *InputHostDataProcessor,
						READER_DEVICE_EVENT_PROCESSOR *InputEventProcessor,
						HTAG_FILE_POST_TRANSFER_PROCESSOR *InputFilePostProcessor,
						HTAG_CODE_POST_TRANSFER_PROCESSOR *InputCodePostProcessor = 0 );

// Destructor for SYN_FE_COMMAND_HANDLER
    ~SYN_FE_COMMAND_HANDLER(void);

// See if a <SYN>FE command has been received ( overload #1 where length of input is specified).
	int ReceiveAndParseServer(char *Result, const char *DataReceivedSoFar, int LengthOfDataReceivedSoFar );

// See if a <SYN>FE command has been received (overload #2, where length of input data stream is implied by NULL termination).
	int ReceiveAndParseServer(char *Result, const char *DataReceivedSoFar = "");

// Writes the HTAG_IMAGE data to a Matrix device.
	bool WriteMatrixImageData(HTAG_IMAGE *ImageData, char *Result);

// WriteMatrixImageData (overloaded version #2, where 8 bit image data sent by the caller is converted to an HTAG_IMAGE)
	bool WriteMatrixImageData(const char *CommandName, HSTRING *ImageData, int XDimension, int YDimension,
												char *Result, int Protocol, int Format);

// Write script data to a Matrix device in HTag format.
	bool WriteScript(const char *CommandName, const char *FileName, int WorkGroup, HSTRING *ScriptData,
											char *Result, int Protocol, int DesignateAsDefaultScript);

// Write main script data to a Matrix device in HTag format.
	bool WriteMainScript(const char *FileName, int WorkGroup, HSTRING *ScriptData, char *Result,
											int Protocol, int DesignateAsDefaultScript);

// Write edit script data to a Matrix device in HTag format.
	bool WriteEditScript(const char *FileName, int WorkGroup, HSTRING *ScriptData, char *Result,
											int Protocol, int DesignateAsDefaultScript);

// Write a file to a Matrix device in HTag format.
	bool WriteFile(const char *FileName, int WorkGroup, HSTRING *FileData, char *Result, int Protocol,
									   int CompressionType, int FileType, const char *CommandName = "FILSTO" );

// Send a trigger event.
	bool SendTriggerEvent(int TriggerState);


// Write application code to a Matrix device in HTag format.
	bool WriteApplicationCode( HSTRING *ApplicationCode, char *Result, int Protocol, bool StoreFile = true,
					bool OverrideApplicationTypeCheck = false, bool OverrideDeviceTypeCheck = false,
					int CompressionType = NO_COMPRESSION, const char *CommandName = "APPCOD" );


// Write boot code to a Matrix device in HTag format.
	bool WriteBootCode( HSTRING *BootCode, char *Result, int Protocol, bool OverrideDeviceTypeCheck = false,
					const char *CommandName = "BOTCOD", int Key = 0 );


// Write a flash image to the device.
	bool WriteFlashImage( HSTRING *InputFlashImage, char *Result, int Protocol, int StartAddress, bool Reboot, bool BootMode,
			int CompressionType, const char *CommandName = "FLSSTO", int Key = 0, bool WaitForSecondACK = false,
			bool DoIndicators = false, bool StopAfterDownload = false, bool SaveSerialNumbers = false,
			bool PreliminaryResponseRequired = false );


// Retrieve a flash image from the device.
	bool ReadFlashImage( char *Result, int Protocol, int StartAddress, int SizeOfImage, bool Reboot, bool BootMode,
			int CompressionType, const char *CommandName = "FLSRET", int Key = 0 );


// Write scan engine firmware to a Matrix device in HTag format.
	bool WriteScanEngineFirmware( HTAG_SCAN_ENGINE_FIRMWARE *ScanEngineFirmware, char *Result );


// Write scan engine firmware to a Matrix device in HTag format.
	bool WriteScanEngineFirmware( HSTRING *ScanEngineFirmware, char *Result, int Protocol,
						int CompressionType = HTAG_SCAN_ENGINE_FIRMWARE::HTAG_SCAN_ENGINE_FIRMWARE_UNCOMPRESSED_HEX,
						const char *CommandName = HTAG_SCAN_ENGINE_FIRMWARE_DEFAULT_COMMAND_NAME,
						const char *FileName = "EngineFirmware",
						int StorageType = HTAG_SCAN_ENGINE_FIRMWARE::HTAG_SCAN_ENGINE_FIRMWARE_STORE_TO_SCAN_ENGINE_ONLY,
						bool ResponseRequired = false,
						bool IgnoreFirmwareTypeMismatch = false );


// Send a decode result.
	bool SendDecodeResult( char *Result, int Protocol, const char *DecodeResultData, int SizeOfDecodeResultData,
			const char *HHPCodeID, const char *AIMID, bool WaitForResponse, bool ReturnDecodeResultHostTransmitAcknowledge,
			bool WideCharacterFormat, int IndicatorRequired = HTAG_DECODE_RESULT_INDICATOR_REQUIRED_DEFAULT,
			int SequenceNumber = HTAG_DECODE_RESULT_SEQUENCE_NUMBER_DEFAULT,
			int IntermessageDelay = HTAG_DECODE_RESULT_INTERMESSAGE_DELAY_DEFAULT );


// Send a wchar_t decode result.
	bool SendWideDecodeResult( char *Result, int Protocol, const wchar_t *DecodeResultData, int SizeOfDecodeResultData,
			const char *HHPCodeID, const char *AIMID, bool WaitForResponse, bool ReturnDecodeResultHostTransmitAcknowledge,
			int IndicatorRequired = HTAG_DECODE_RESULT_INDICATOR_REQUIRED_DEFAULT,
			int SequenceNumber = HTAG_DECODE_RESULT_SEQUENCE_NUMBER_DEFAULT,
			int IntermessageDelay = HTAG_DECODE_RESULT_INTERMESSAGE_DELAY_DEFAULT );


// Send a host transmit acknowledge for adecode result.
	bool SendDecodeResultHostTransmitAcknowledge( char *Result, int Protocol, bool WaitForResponse,
			int IndicatorRequired = HTAG_DECODE_RESULT_INDICATOR_REQUIRED_DEFAULT,
			int SequenceNumber = HTAG_DECODE_RESULT_SEQUENCE_NUMBER_DEFAULT );


// Send a system information request.
	bool SendSystemInformationRequest( char *Result, int Protocol, const char *DeviceAddress,
			int SizeOfDeviceAddress, const char *DeviceName, int SizeOfDeviceName, bool WaitForResponse );


// Send system information.
	bool SendSystemInformation( char *Result, int Protocol, const char *DeviceAddress,
			int SizeOfDeviceAddress, const char *DeviceName, int SizeOfDeviceName, bool WaitForResponse, int WorkgroupSelection );


// Send menu command.
	bool SendMenuCommand( char *Result, int Protocol, const char *DeviceName, int SizeOfDeviceName, const char *MenuCommand,
		int SizeOfMenuCommand, bool CommandResponseRequired, bool WaitForResponse, bool IndicatorRequired = false, const CIoMan *  pOrigInterface = g_pStdInterface );

// Send menu command response.
///	bool SendMenuCommandResponse( char *Result, int Protocol, const char *DeviceName, int SizeOfDeviceName,
///			const char *MenuCommandResponse, int SizeOfMenuCommandResponse, bool WaitForResponse );

// Send Config menu command.
	bool SendConfigMenuCommand( char *Result, int Protocol, const char *DeviceName, int SizeOfDeviceName, const char *MenuCommand,
			int SizeOfMenuCommand, bool CommandResponseRequired, bool WaitForResponse, bool IndicatorRequired = false, const CIoMan *  pOrigInterface = g_pStdInterface );

// Send config menu command response.
///	bool SendConfigMenuCommandResponse( char *Result, int Protocol, const char *DeviceName, int SizeOfDeviceName,
///			const char *MenuCommandResponse, int SizeOfMenuCommandResponse, bool WaitForResponse );

// Send non-menu command.
	bool SendNonMenuCommand( char *Result, int Protocol, const char *DeviceName, int SizeOfDeviceName,
			const char *NonMenuCommand, int SizeOfNonMenuCommand, bool CommandResponseRequired, bool WaitForResponse );


// Send non-menu command response.
	bool SendNonMenuCommandResponse( char *Result, int Protocol, const char *DeviceName, int SizeOfDeviceName,
			const char *NonMenuCommandResponse, int SizeOfNonMenuCommandResponse, bool WaitForResponse );
	
private:
	SYN_FE_COMMAND_HANDLER (const SYN_FE_COMMAND_HANDLER&);            // declared private, but not defined, to prevent copying         
	SYN_FE_COMMAND_HANDLER& operator=(const SYN_FE_COMMAND_HANDLER&);  // declared private, but not defined, to prevent copying 

};
//----------------------------------------------------------------------------



#endif // SYN_FE_COMMAND_HANDLER_H



