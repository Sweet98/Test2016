//======================================================================================
// SYNFECommandHandler.cpp
//======================================================================================
// $RCSfile: Matrix/synfecommandhandler.cpp $
// $Revision: 1.16 $
// $Date: 2011/09/26 16:49:22EDT $
//======================================================================================

//======================================================================================








//---------------------------------------------------------------------------
//
//  Class Name:     SYN_FE_COMMAND_HANDLER
//
//  Class Description:
//
//		The SYN_FE_COMMAND_HANDLER class provides a mechanism to receive <SYN>FE type asynchronous commands from a
//		device.  All received data is wrapped in the <SYN>FE HTAG format.  The following data types can be received:
//		1) Scripts - actual script code defined in HTAG_SCRIPT.
//		2) Host commands - HHPL host commands that are to be processed by a script, as defined in HTAG_SCRIPT_HOST_COMMAND.
//		3) Host data - HHPL host generated data that is to be processed by a script, as defined in HTAG_SCRIPT_HOST_DATA.
//
//		The principle use of this class is to call the ReceiveAndParseServer function.  The argument to ReceiveAndParseServer
//		is the data that has been received so far to start a <SYN>FE command.  If the string is NULL, ReceiveAndParseServer
//		will wait for WaitTimeForCommand milliseconds to receive command data.  Otherwise, the string contains the start of a
//		<SYN>FE command and ReceiveAndParseServer must receive and process the rest of the data.
//
//		Note that entering boot mode is required in order to store or retrieve boot code.  There is a two step process to
//		get there.  First, the host must issue a BOTMOD command with a parameter named password set to "SETBOOTCODEMODE".
//		Second, the host must issue a BOTMOD command with a parameter named password set to either "SETBOOTCODESTOREMODE" if
//		storing boot code or "SETBOOTCODERETRIEVEMODE" if retrieving boot code.  Finally, issue BOTCOD or BOTSTO to store
//		boot code, or BOTRET to retrieve it.  Note that we use BootCodeMode to track the current mode, which gets reset to idle
//		after every <SYN>FE command if it hasn't changed from the last time we processed a <SYN>FE command.  This is done to
//		force the mode commands to be contiguous, so that we never end up where the mode is enabled or partially enabled and
//		left open to receive a BOTSTO or BOTRET at a later time.
//
//		Note also that entering flash image mode is required in order to store or retrieve flash.  There is a two step process to
//		get there.  First, the host must issue a FLSMOD command with a parameter named password set to "SETFLASHIMAGEMODE".
//		Second, the host must issue a FLSMOD command with a parameter named password set to either "SETFLASHIMAGESTOREMODE" if
//		storing flash or "SETFLASHIMAGERETRIEVEMODE" if retrieving flash.  Finally, issue FLSST2 to store
//		flash, or FLSRET to retrieve it.  Note that we use FlashImageMode to track the current mode, which gets reset to idle
//		after every <SYN>FE command if it hasn't changed from the last time we processed a <SYN>FE command.  This is done to
//		force the mode commands to be contiguous, so that we never end up where the mode is enabled or partially enabled and
//		left open to receive a FLSSTO or FLSRET at a later time.
//
//---------------------------------------------------------------------------
#include "db_chrio.h"
#include	"CRC.h"
#include	"HHPCommunicationConfig.h"
#include	"HWString.h"
#include	"SYNFECommandHandler.h"			// Interface definition
#include	"HTagDataResult.h"			// Protocol wrapped command handling for commands

#include	<string.h>
#include	<stdio.h>









//
// Constructor for SYN_FE_COMMAND_HANDLER
//      Copy device and transfer status handlers.
//
SYN_FE_COMMAND_HANDLER::SYN_FE_COMMAND_HANDLER(HCOMM *HCommDevice, HTRANSFER_STATUS *InputTransferStatus,
        HCOMMAND_HANDLER *InputCommandHandler,
        HTAG_POST_TRANSFER_PROCESSOR *InputPostTransferProcessor,
        HTAG_IMAGE_POST_TRANSFER_PROCESSOR *InputImagePostProcessor,
        HTAG_SCRIPT_POST_TRANSFER_PROCESSOR *InputScriptPostProcessor,
        HTAG_SCRIPT_HOST_COMMAND_POST_TRANSFER_PROCESSOR *InputHostCommandProcessor,
        HTAG_SCRIPT_HOST_DATA_POST_TRANSFER_PROCESSOR *InputHostDataProcessor,
        READER_DEVICE_EVENT_PROCESSOR *InputEventProcessor,
        HTAG_FILE_POST_TRANSFER_PROCESSOR *InputFilePostProcessor,
        HTAG_CODE_POST_TRANSFER_PROCESSOR *InputCodePostProcessor )
{
	// To start with, we're not in boot code mode.
	LastBootCodeMode = IDLE_BOOT_CODE_MODE;
	BootCodeMode = IDLE_BOOT_CODE_MODE;

	// To start with, we're not in flash image mode.
	LastFlashImageMode = IDLE_FLASH_IMAGE_MODE;
	FlashImageMode = IDLE_FLASH_IMAGE_MODE;

	Device = HCommDevice;
	TransferStatus = InputTransferStatus;
	CommandHandler = InputCommandHandler;
	PostTransferProcessor = InputPostTransferProcessor;
	ImagePostProcessor = InputImagePostProcessor;
	FilePostProcessor = InputFilePostProcessor;
	ScriptPostProcessor = InputScriptPostProcessor;
	HostCommandProcessor = InputHostCommandProcessor;
	HostDataProcessor = InputHostDataProcessor;
	EventProcessor = InputEventProcessor;
	CodePostProcessor = InputCodePostProcessor;

	UnusedCommandData = new HSTRING( HTAG_HEADER_STRING_SIZE, true );

	return;
}
//---------------------------------------------------------------------------







//
// Destructor for SYN_FE_COMMAND_HANDLER
//      Release storage.
//
SYN_FE_COMMAND_HANDLER::~SYN_FE_COMMAND_HANDLER(void)
{
	if (UnusedCommandData)
		delete UnusedCommandData;

	return;
}
//---------------------------------------------------------------------------







//
//	ReceiveAndParseServer (overload #1, where length of input data stream is specified)
//		See if a <SYN>FE command has been received.  The argument to ReceiveAndParseServer is the data that has been received
//		so far to start a <SYN>FE command.  If the string is NULL, ReceiveAndParseServer will wait for WaitTimeForCommand
//		milliseconds to receive command data.  Otherwise, the string contains the start of a <SYN>FE command and
//		ReceiveAndParseServer must receive and process the rest of the data.
//
/* TODO:  we repeat the same code over and over, and worse yet--we're not consistent on how we read the HTAG (we do some differently
   for no obvious reason.  But, there MIGHT be a good reason.  We need to be careful about that.
   
   We need to strive for something like this:
   HTAG object;
   object.read() to read HTAG.  We can probably call HTAG::ReadTaggedImage().  Again, we do this for ALL of them;
   LoadHtag or something similar to create the proper derived HTAG (dependant on HTAG Command ID).  This should only be what's inside the series of CommandName.Compare() calls;
   Act on HTAG object data using that derived class.  This would be outside that above series of "ifs", if possible;
   return response(s);
*/
int SYN_FE_COMMAND_HANDLER::ReceiveAndParseServer(char *Result, const char *DataReceivedSoFar, int LengthOfDataReceivedSoFar )
{
	int			Status = HTAG::HTAG_NO_COMMAND_RECEIVED;		// Nothing processed yet.
	int			CommandLength;
	HSTRING		CommandName( HTAG_COMMAND_NAME_SIZE, true );
	HSTRING		ReceivedData( HTAG_HEADER_STRING_SIZE + HTAG_COMMAND_NAME_SIZE, true );
	int			StartingTransferType = TransferStatus->TransferType;
	bool bRespReq = false;


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	ReceivedData.Copy( DataReceivedSoFar, LengthOfDataReceivedSoFar );


	Status = HTAG::ReceiveHTAGHeader( Result, &ReceivedData, &CommandName, &CommandLength, Device, UnusedCommandData );
	// At this point, I think ReceivedData has nothing in it (CommandName & CommandLength have the data).

	// If we made it this far, dispatch the read routine based on the command.
	if( Status == HTAG::HTAG_OK )
	{
		// Is it a command to store a main script?
		if(CommandName.Compare("HPLSMS", false) == 0)
		{
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			TransferStatus->TransferType = HTRANSFER_STATUS::STORE_SCRIPT;
			TransferStatus->PrepareDownloadStatus();
			HTAG_SCRIPT		NewScript("HPLSMS", Device, TransferStatus, CommandHandler);
			Status = NewScript.ReadTaggedImage(&ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			bRespReq = (bool) NewScript.ResponseRequired;
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( NewScript.PreliminaryResponseRequired ) xputchar(ACK);
				if(ScriptPostProcessor != NULL) ScriptPostProcessor->StoreHHPLMainScript(&Status, Result, &NewScript);
			}
			else
			{
				if ( NewScript.PreliminaryResponseRequired ) xputchar(NAK);
			}
			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

		// Is it a command to retrieve a main script?
		else if(CommandName.Compare("HPLRMS", false) == 0)
		{
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			TransferStatus->TransferType = HTRANSFER_STATUS::RETRIEVE_SCRIPT;
			TransferStatus->PrepareDownloadStatus();
			// Get information on the script to retrieve.
			HTAG_SCRIPT		NewScript("HPLRMS", Device, TransferStatus, CommandHandler);
			Status = NewScript.ReadTaggedImage(&ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			bRespReq = (bool) NewScript.ResponseRequired;
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( NewScript.PreliminaryResponseRequired ) xputchar(ACK);
				if(ScriptPostProcessor != NULL)
				{
					ScriptPostProcessor->RetrieveHHPLMainScript(&Status, Result, &NewScript);
					if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
					{
						Status = NewScript.WriteTaggedImage(Result);
					}
				}
			}
			else
			{
				if ( NewScript.PreliminaryResponseRequired ) xputchar(NAK);
				Status = SYN_FE_COMMAND_HANDLER_FILE_NOT_FOUND;
			}
			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

		// Is it a command to load and run a main script?
		else if(CommandName.Compare("HPLLMS", false) == 0)
		{
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			TransferStatus->TransferType = HTRANSFER_STATUS::LOAD_AND_RUN_SCRIPT;
			TransferStatus->PrepareDownloadStatus();
			HTAG_SCRIPT		NewScript("HPLLMS", Device, TransferStatus, CommandHandler);
			Status = NewScript.ReadTaggedImage(&ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			bRespReq = (bool) NewScript.ResponseRequired;
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( NewScript.PreliminaryResponseRequired ) xputchar(ACK);
				if(ScriptPostProcessor != NULL)
					ScriptPostProcessor->LoadAndRunHHPLMainScript(&Status, Result, &NewScript);
			}
			else
			{
				if ( NewScript.PreliminaryResponseRequired ) xputchar(NAK);
			}
			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

		// Is it a command to store an edit script?
		else if(CommandName.Compare("HPLSES", false) == 0)
		{
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			TransferStatus->TransferType = HTRANSFER_STATUS::STORE_SCRIPT;
			TransferStatus->PrepareDownloadStatus();
			HTAG_SCRIPT		NewScript("HPLSES", Device, TransferStatus, CommandHandler);
			Status = NewScript.ReadTaggedImage(&ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			bRespReq = (bool) NewScript.ResponseRequired;
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( NewScript.PreliminaryResponseRequired ) xputchar(ACK);
				if(ScriptPostProcessor != NULL)
					ScriptPostProcessor->StoreHHPLEditScript(&Status, Result, &NewScript);
			}
			else
			{
				if ( NewScript.PreliminaryResponseRequired ) xputchar(NAK);
			}
			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

		// Is it a command to retrieve an edit script?
		else if(CommandName.Compare("HPLRES", false) == 0)
		{
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			TransferStatus->TransferType = HTRANSFER_STATUS::RETRIEVE_SCRIPT;
			TransferStatus->PrepareDownloadStatus();
			// Get information on the script to retrieve.
			HTAG_SCRIPT		NewScript("HPLRES", Device, TransferStatus, CommandHandler);
			Status = NewScript.ReadTaggedImage(&ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			bRespReq = (bool) NewScript.ResponseRequired;
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( NewScript.PreliminaryResponseRequired ) xputchar(ACK);
				if(ScriptPostProcessor != NULL)
				{
					ScriptPostProcessor->RetrieveHHPLEditScript(&Status, Result, &NewScript);
					if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
					{
						Status = NewScript.WriteTaggedImage(Result);
					}
				}
			}
			else
			{
				if ( NewScript.PreliminaryResponseRequired ) xputchar(NAK);
				Status = SYN_FE_COMMAND_HANDLER_FILE_NOT_FOUND;
			}
			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

		// Is it a command to load and run an edit script?
		else if(CommandName.Compare("HPLLES", false) == 0)
		{
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			TransferStatus->TransferType = HTRANSFER_STATUS::LOAD_AND_RUN_SCRIPT;
			TransferStatus->PrepareDownloadStatus();
			HTAG_SCRIPT		NewScript("HPLLES", Device, TransferStatus, CommandHandler);
			Status = NewScript.ReadTaggedImage(&ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			bRespReq = (bool) NewScript.ResponseRequired;
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( NewScript.PreliminaryResponseRequired ) xputchar(ACK);
				if(ScriptPostProcessor != NULL)
					ScriptPostProcessor->LoadAndRunHHPLEditScript(&Status, Result, &NewScript);
			}
			else
			{
				if ( NewScript.PreliminaryResponseRequired ) xputchar(NAK);
			}
			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

		// Is it a command to process a script host command?
		else if(CommandName.Compare("HPLSHC", false) == 0)
		{
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			TransferStatus->TransferType = HTRANSFER_STATUS::HOST_COMMAND;
			TransferStatus->PrepareDownloadStatus();
			HTAG_SCRIPT_HOST_COMMAND		NewScriptHostCommand("HPLSHC", Device, TransferStatus, CommandHandler);
			Status = NewScriptHostCommand.ReadTaggedImage(&ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			bRespReq = (bool) NewScriptHostCommand.ResponseRequired;
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( NewScriptHostCommand.PreliminaryResponseRequired ) xputchar(ACK);
				if(HostCommandProcessor != NULL)
					HostCommandProcessor->HHPLSendHostCommand(&Status, Result, &NewScriptHostCommand);
			}
			else
			{
				if ( NewScriptHostCommand.PreliminaryResponseRequired ) xputchar(NAK);
			}
			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

		// Is it a command to process a script host data?
		else if(CommandName.Compare("HPLSHD", false) == 0)
		{
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			TransferStatus->TransferType = HTRANSFER_STATUS::HOST_DATA;
			TransferStatus->PrepareDownloadStatus();
			HTAG_SCRIPT_HOST_DATA		NewScriptHostData("HPLSHD", Device, TransferStatus, CommandHandler);
			Status = NewScriptHostData.ReadTaggedImage(&ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			bRespReq = (bool) NewScriptHostData.ResponseRequired;
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( NewScriptHostData.PreliminaryResponseRequired ) xputchar(ACK);
				if(HostDataProcessor != NULL)
					HostDataProcessor->HHPLSendHostData(&Status, Result, &NewScriptHostData);
			}
			else
			{
				if ( NewScriptHostData.PreliminaryResponseRequired ) xputchar(NAK);
			}
			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

		// Is it a command to process an image?
		else if(CommandName.Compare("040807", false) == 0 || CommandName.Compare("IMGSHV", false) == 0 || CommandName.Compare("IMGSHX", false) == 0
		        || CommandName.Compare("IMGSHD", false) == 0 || CommandName.Compare("IMGSHY", false) == 0 || CommandName.Compare("IMGSHS", false) == 0)
		{
			TransferStatus->TransferType = HTRANSFER_STATUS::PROCESS_IMAGE;
			TransferStatus->PrepareDownloadStatus();
			HTAG_IMAGE		NewImage("040807", Device, TransferStatus, CommandHandler);
			// Note that we treat the imager commands a differently due to the fact that images can be so large.  We create a
			// minimum size string to receive data, letting the HTAG or HMODEM processor resize it as needed.  This is done so
			// that HMODEM can use a single buffer for compressed data by resizing OutputImage only once.  This saves lots of heap
			// space in an embedded environment.  If all the data we need has already been received, we'll use that buffer instead.
			//			if( (int) ReceivedData.Size >= ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			if( ReceivedData.Size > 0 )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
				Status = NewImage.ReadTaggedImage( &ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE );
			}
			else
			{
				Status = NewImage.ReadTaggedImage( NULL, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			}
			bRespReq = (bool) NewImage.ResponseRequired;

			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( NewImage.PreliminaryResponseRequired ) xputchar(ACK);
				if (CommandName.Compare("IMGSHV", false) == 0 )
					Status |= 0x200; // use status to pass in a flag to do vigor levels
				else if (CommandName.Compare("IMGSHX", false) == 0 )
					Status |= 0x400; // use status to pass in a flag to do vigor levels
				else if (CommandName.Compare("IMGSHD", false) == 0 )
					Status |= 0x2000; // use status to pass in a flag to do vigor levels
				else if (CommandName.Compare("IMGSHY", false) == 0 )
					Status |= 0x4000; // use status to pass in a flag to do stream analysis
				else if (CommandName.Compare("IMGSHS", false) == 0 )
					Status |= 0x8000; // use status to pass in a flag to do stream analysis

				if(ImagePostProcessor != NULL)
					ImagePostProcessor->ProcessImage(&Status, Result, &NewImage);
			}
			else
			{
				if ( NewImage.PreliminaryResponseRequired ) xputchar(NAK);
			}
			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

		// Is it a command to process a trigger event?
		else if(CommandName.Compare("TRGEVT", false) == 0)
		{
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			HTAG	TriggerEventHandler( Device, TransferStatus, CommandHandler, false );
			TriggerEventHandler.CommandName->Copy( "TRGEVT" );
			int		TriggerEventType=READER_DEVICE_EVENT_PROCESSOR::UNTRIGGERED;
			TriggerEventHandler.AddNumericParameter(HTAG_TRIGGER_EVENT_TYPE_PARAMETER_NAME, &TriggerEventType,
			                                        READER_DEVICE_EVENT_PROCESSOR::MANUALLY_TRIGGERED, READER_DEVICE_EVENT_PROCESSOR::UNTRIGGERED,
			                                        READER_DEVICE_EVENT_PROCESSOR::AUTO_TRIGGERED, true );
			unsigned char	*DataPtr;
			int		SizeOfData;
			if(TriggerEventHandler.ReadHTagData(&ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE ))
			{
				if ( TriggerEventHandler.PreliminaryResponseRequired ) xputchar(ACK);
				Status = TriggerEventHandler.LoadTaggedImage( &ReceivedData, &DataPtr, &SizeOfData );
			}
			else
			{
				if ( TriggerEventHandler.PreliminaryResponseRequired ) xputchar(NAK);
				Status = HTAG::HTAG_I_O_ERROR;
			}
			bRespReq = (bool) TriggerEventHandler.ResponseRequired;
			if(EventProcessor != NULL)
				EventProcessor->TriggerEvent(TriggerEventType);
		}

		// Is it a command to process a decode message in MSGGET form?
		else if(CommandName.Compare("091200", false) == 0)
		{
			/*NOTE:  Looking at the code below, I don't think this follows the HTAG format!  Instead, ReceivedData seems to have the following format:
			0)
			1)
			2)
			3)
			4) HHP Code ID
			5) AIM ID 1
			6) AIM ID 2
			7) Group Seperator (0x1D)
			8) Start of barcode data
			*/
			ReceivedData.Reallocate( CommandLength - HTAG_COMMAND_NAME_SIZE );
			HSTRING	Temp( CommandLength - HTAG_COMMAND_NAME_SIZE - ReceivedData.Size );
			Device->Read( &Temp, CommandLength - HTAG_COMMAND_NAME_SIZE - ReceivedData.Size, Device->WaitTimeForHTAGCommand );
			ReceivedData.Append( &Temp );
			bRespReq = false; // This is not a real HTAG, so this must be false;

			// Build the result.
			if( ReceivedData.Char[ 7 ] == GS )  // Interesting--this appears to expect exactly 7 characters in the Parameters.  WCHAR appears to be important (always reported), but below we hard code to false.
			{
				HTAG_DECODE_RESULT	DecodeResult( "091200" );
				DecodeResult.AIMID->Copy( &ReceivedData, 5, 6 );
				DecodeResult.HHPCodeID->Copy( &ReceivedData, 4, 4 );
				DecodeResult.Data->Copy( &ReceivedData, 8, ReceivedData.Size - 1 );
				DecodeResult.HostTransmitAcknowledge = false;
				DecodeResult.WideCharacterFormat = false;
				if(ImagePostProcessor != NULL)
					ImagePostProcessor->ProcessDecodeResult( &Status, Result, &DecodeResult );
			}
			else
			{
				Status = HTAG::HTAG_I_O_ERROR;
			}
		}
		// Is it a command to process a decode message?
		else if( CommandName.Compare( HTAG_DECODE_RESULT_DEFAULT_COMMAND_NAME, false ) == 0 )
		{
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			TransferStatus->TransferType = HTRANSFER_STATUS::RECEIVING_DECODE_RESULT;
			TransferStatus->PrepareDownloadStatus();
			HTAG_DECODE_RESULT	DecodeResult( HTAG_DECODE_RESULT_DEFAULT_COMMAND_NAME, Device, TransferStatus, CommandHandler );
			Status = DecodeResult.ReadTaggedImage(&ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			bRespReq = (bool) DecodeResult.ResponseRequired;
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( DecodeResult.PreliminaryResponseRequired ) xputchar(ACK);
				if(ImagePostProcessor != NULL)
					ImagePostProcessor->ProcessDecodeResult( &Status, Result, &DecodeResult );
			}
			else
			{
				if ( DecodeResult.PreliminaryResponseRequired ) xputchar(NAK);
			}
			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

		// Is it a command to process a decode message host transmit acknowledge?
		else if( CommandName.Compare( HTAG_DECODE_RESULT_HOST_TRANSMIT_ACKNOWLEDGE_COMMAND_NAME, false ) == 0 )
		{
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			TransferStatus->TransferType = HTRANSFER_STATUS::RECEIVING_DECODE_RESULT_ACKNOWLEDGE;
			TransferStatus->PrepareDownloadStatus();
			HTAG_DECODE_RESULT	DecodeResultHostTransmitAcknowledge
			( HTAG_DECODE_RESULT_HOST_TRANSMIT_ACKNOWLEDGE_COMMAND_NAME, Device, TransferStatus, CommandHandler );
			Status = DecodeResultHostTransmitAcknowledge.ReadTaggedImage
			         (&ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			bRespReq = (bool) DecodeResultHostTransmitAcknowledge.ResponseRequired;
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( DecodeResultHostTransmitAcknowledge.PreliminaryResponseRequired ) xputchar(ACK);
				if(ImagePostProcessor != NULL)
					ImagePostProcessor->ProcessDecodeResultHostTransmitAcknowledge
					( &Status, Result, &DecodeResultHostTransmitAcknowledge );
			}
			else
			{
				if ( DecodeResultHostTransmitAcknowledge.PreliminaryResponseRequired ) xputchar(NAK);
			}
			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

		// Is it a command to store a file?
		else if(CommandName.Compare("FILSTO", false) == 0)
		{
			if ( CodePostProcessor )  CodePostProcessor->PrepareForFileDownload();	
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			TransferStatus->TransferType = HTRANSFER_STATUS::STORE_FILE;
			TransferStatus->PrepareDownloadStatus();
			HTAG_FILE		NewFile("FILSTO", Device, TransferStatus, CommandHandler);
			Status = NewFile.ReadTaggedImage(&ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			bRespReq = (bool) NewFile.ResponseRequired; // Note:  if it's an MOCF, the ResponseRequired will be handled after the reboot.
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( NewFile.PreliminaryResponseRequired ) xputchar(ACK);
				if(FilePostProcessor != NULL) FilePostProcessor->StoreFile(&Status, Result, &NewFile);
				// we'll only get here if we didn't reboot (we must be an rfbase & we got an mocf for an rfscanner, or an error)
			}
			else
			{
				if ( NewFile.PreliminaryResponseRequired ) xputchar(NAK);
			}
			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

		// Is it a command to retrieve a file?
		else if(CommandName.Compare("FILRET", false) == 0)
		{
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			TransferStatus->TransferType = HTRANSFER_STATUS::RETRIEVE_FILE;
			TransferStatus->PrepareDownloadStatus();
			// Get the file name (should be the remainder of the command).
			HTAG_FILE		File("FILRET", Device, TransferStatus, CommandHandler);
			Status = File.ReadTaggedImage(&ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			bRespReq = (bool) File.ResponseRequired;
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( File.PreliminaryResponseRequired ) xputchar(ACK);
				if(FilePostProcessor != NULL)
				{
					FilePostProcessor->RetrieveFile(&Status, Result, &File);

					if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
					{
						// Change command name before sending back to the caller.
						File.CommandName->Copy( "FILSTO" );
						Status = File.WriteTaggedImage(Result);
					}
				}
			}
			else
			{
				if ( File.PreliminaryResponseRequired ) xputchar(NAK);
				Status = SYN_FE_COMMAND_HANDLER_FILE_NOT_FOUND;
			}
			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

		// Is it a command to store application code?
		else if( (CommandName.Compare("APPCOD", false) == 0) || (CommandName.Compare("APPSTO", false) == 0) )
		{
			if ( CodePostProcessor )  CodePostProcessor->PrepareForFileDownload();	
			TransferStatus->TransferType = HTRANSFER_STATUS::STORE_APPLICATION_CODE;
			TransferStatus->PrepareDownloadStatus();
			HTAG_READER_CODE		NewReaderCode( (char *) CommandName.Char , Device, TransferStatus, CommandHandler);

			// Note that we treat the code images differently due to the fact that they can be so large.  We create a
			// minimum size string to receive data, letting the HTAG or HMODEM processor resize it as needed.  This is done so
			// that HMODEM can use a single buffer for compressed data by resizing OutputImage only once.  This saves lots of heap
			// space in an embedded environment.  If all the data we need has already been received, we'll use that buffer instead.
			//			if( (int) ReceivedData.Size >= ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			if( ReceivedData.Size > 0 )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
				Status = NewReaderCode.ReadTaggedImage( &ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE );
			}
			else
			{
				Status = NewReaderCode.ReadTaggedImage( NULL /* OutputImage */, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			}
			bRespReq = (bool) NewReaderCode.ResponseRequired;

			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( NewReaderCode.PreliminaryResponseRequired ) xputchar(ACK);
				if(CodePostProcessor != NULL)
					CodePostProcessor->StoreApplicationCode(&Status, Result, &NewReaderCode);
			}
			else
			{
				if ( NewReaderCode.PreliminaryResponseRequired ) xputchar(NAK);
			}
			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

		// Is it a command to retrieve application code?
		else if(CommandName.Compare("APPRET", false) == 0)
		{
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			TransferStatus->TransferType = HTRANSFER_STATUS::RETRIEVE_APPLICATION_CODE;
			TransferStatus->PrepareDownloadStatus();
			// Get the file name (should be the remainder of the command).
			HTAG_READER_CODE		ReaderCode( (char *) CommandName.Char , Device, TransferStatus, CommandHandler);
			Status = ReaderCode.ReadTaggedImage(&ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			bRespReq = (bool) ReaderCode.ResponseRequired;
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( ReaderCode.PreliminaryResponseRequired ) xputchar(ACK);
				if( CodePostProcessor != NULL )
				{
					CodePostProcessor->RetrieveApplicationCode(&Status, Result, &ReaderCode);

					if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
					{
						// Change command name before sending back to the caller.
						ReaderCode.CommandName->Copy( "APPCOD");
						Status = ReaderCode.WriteTaggedImage(Result);
					}
				}
			}
			else
			{
				if ( ReaderCode.PreliminaryResponseRequired ) xputchar(NAK);
				Status = SYN_FE_COMMAND_HANDLER_FILE_NOT_FOUND;
			}
			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

		// Is it a command to store boot code?
		else if( (CommandName.Compare("BOTCOD", false) == 0) || (CommandName.Compare("BOTSTO", false) == 0) )
		{
			TransferStatus->TransferType = HTRANSFER_STATUS::STORE_BOOT_CODE;
			TransferStatus->PrepareDownloadStatus();
			HTAG_READER_CODE		NewReaderCode( (char *) CommandName.Char , Device, TransferStatus, CommandHandler);

			// Note that we treat the code images differently due to the fact that they can be so large.  We create a
			// minimum size string to receive data, letting the HTAG or HMODEM processor resize it as needed.  This is done so
			// that HMODEM can use a single buffer for compressed data by resizing OutputImage only once.  This saves lots of heap
			// space in an embedded environment.  If all the data we need has already been received, we'll use that buffer instead.
			//			if( (int) ReceivedData.Size >= ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			if( ReceivedData.Size > 0 )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
				Status = NewReaderCode.ReadTaggedImage( &ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE );
			}
			else
			{
				Status = NewReaderCode.ReadTaggedImage( NULL /* OutputImage */, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			}

			bRespReq = (bool) NewReaderCode.ResponseRequired;
			// Only process if we're in the right mode.
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( NewReaderCode.PreliminaryResponseRequired ) xputchar(ACK);
				if( BootCodeMode == SETBOOTCODESTOREMODE_BOOT_CODE_MODE )
				{
					if(CodePostProcessor != NULL)
						CodePostProcessor->StoreBootCode(&Status, Result, &NewReaderCode);
				}
				else
				{
					Status = SYN_FE_COMMAND_HANDLER_NOT_IN_APPROPRIATE_BOOT_MODE;
				}
			}
			else
			{
				if ( NewReaderCode.PreliminaryResponseRequired ) xputchar(NAK);
			}

			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;

			// Whatever happened, always turn off boot code mode.
			BootCodeMode = IDLE_BOOT_CODE_MODE;
		}

		// Is it a command to retrieve boot code?
		else if(CommandName.Compare("BOTRET", false) == 0)
		{
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			TransferStatus->TransferType = HTRANSFER_STATUS::RETRIEVE_BOOT_CODE;
			TransferStatus->PrepareDownloadStatus();
			// Get the file name (should be the remainder of the command).
			HTAG_READER_CODE		ReaderCode( (char *) CommandName.Char , Device, TransferStatus, CommandHandler);
			Status = ReaderCode.ReadTaggedImage(&ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			bRespReq = (bool) ReaderCode.ResponseRequired;

			// Only process if we're in the right mode.
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( ReaderCode.PreliminaryResponseRequired ) xputchar(ACK);
				if( BootCodeMode == SETBOOTCODERETRIEVEMODE_BOOT_CODE_MODE )
				{
					if( CodePostProcessor != NULL )
					{
						CodePostProcessor->RetrieveBootCode(&Status, Result, &ReaderCode);
						if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
						{
							if( TurnOnBootMode( Result, false ) )
							{
								// Change command name before sending back to the caller.
								ReaderCode.CommandName->Copy( "BOTCOD");
								Status = ReaderCode.WriteTaggedImage(Result);
							}
							else
							{
								Status = SYN_FE_COMMAND_HANDLER_MODE_SWITCH_FAILED;
							}
						}
						else
						{
							Status = SYN_FE_COMMAND_HANDLER_FILE_NOT_FOUND;
						}
					}
				}
				else
				{
					Status = SYN_FE_COMMAND_HANDLER_NOT_IN_APPROPRIATE_BOOT_MODE;
				}
			}
			else
			{
				if ( ReaderCode.PreliminaryResponseRequired ) xputchar(NAK);
			}

			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;

			// Whatever happened, always turn off boot code mode.
			BootCodeMode = IDLE_BOOT_CODE_MODE;
		}

		// Is it a command to turn on boot mode?
		else if(CommandName.Compare("BOTMOD", false) == 0)
		{
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			HTAG	BootModeEventHandler( Device, TransferStatus, CommandHandler, false );
			BootModeEventHandler.CommandName->Copy( "BOTMOD" );
			HSTRING		PasswordParameter( 20, true );
			BootModeEventHandler.AddHStringParameter( "PASSWORD", &PasswordParameter, "", 0, true );
			unsigned char	*DataPtr;
			int		SizeOfData;
			if( BootModeEventHandler.ReadHTagData( &ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE ))
			{
				Status = BootModeEventHandler.LoadTaggedImage( &ReceivedData, &DataPtr, &SizeOfData );
				bRespReq = (bool) BootModeEventHandler.ResponseRequired;
				if ( BootModeEventHandler.PreliminaryResponseRequired ) xputchar(ACK);

				// Process the boot mode.
				switch( BootCodeMode )
				{
				case IDLE_BOOT_CODE_MODE:
					// If we're in idle mode, the only valid password is "SETBOOTCODEMODE".
					if( PasswordParameter.Compare( "SETBOOTCODEMODE", true ) == 0 )
					{
						BootCodeMode = SETBOOTCODEMODE_BOOT_CODE_MODE;
					}
					break;

				case SETBOOTCODEMODE_BOOT_CODE_MODE:
					// If we've turned on boot code mode and are waiting to know whether we're to store or retrieve,
					// the only valid passwords are "SETBOOTCODESTOREMODE" or "SETBOOTCODERETRIEVEMODE".
					if( PasswordParameter.Compare( "SETBOOTCODESTOREMODE", true ) == 0 )
					{
						BootCodeMode = SETBOOTCODESTOREMODE_BOOT_CODE_MODE;
					}
					else if( PasswordParameter.Compare( "SETBOOTCODERETRIEVEMODE", true ) == 0 )
					{
						BootCodeMode = SETBOOTCODERETRIEVEMODE_BOOT_CODE_MODE;
					}
					else
					{
						BootCodeMode = IDLE_BOOT_CODE_MODE;
					}
					break;

				default:
					// If we're in any other mode and we get a request to switch mode, that's an error, because the next command
					// should be the actual "BOTSTO" or "BOTRET" command.  Turn off boot code mode.
					BootCodeMode = IDLE_BOOT_CODE_MODE;
					break;
				};
			}
			else
			{
				if ( BootModeEventHandler.PreliminaryResponseRequired ) xputchar(NAK);
				Status = HTAG::HTAG_I_O_ERROR;
			}
		}

		// This command stores a System Update File
		else if(CommandName.Compare("FLSST2", false) == 0)
		{
			if ( CodePostProcessor )  CodePostProcessor->PrepareForFileDownload();	
			int		OriginalDefaultCharacterReadTimeoutForHTAGNoProtocol = Device->DefaultCharacterReadTimeoutForHTAGNoProtocol;
			Device->DefaultCharacterReadTimeoutForHTAGNoProtocol =
			    Device->DefaultCharacterReadTimeoutForHTAGNoProtocolFlashImageTransfers;
			TransferStatus->TransferType = HTRANSFER_STATUS::RETRIEVE_FLASH_IMAGE;
			TransferStatus->PrepareDownloadStatus();
			HTAG_READER_CODE		NewFlashImage( (char *) CommandName.Char , Device, TransferStatus, CommandHandler);

			// Note that we treat the code images differently due to the fact that they can be so large.  We create a
			// minimum size string to receive data, letting the HTAG or HMODEM processor resize it as needed.  This is done so
			// that HMODEM can use a single buffer for compressed data by resizing OutputImage only once.  This saves lots of heap
			// space in an embedded environment.  If all the data we need has already been received, we'll use that buffer instead.
			//			if( (int) ReceivedData.Size >= ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			if( ReceivedData.Size > 0 )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
				Status = NewFlashImage.ReadTaggedImage( &ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			}
			else
			{
				Status = NewFlashImage.ReadTaggedImage( NULL /* OutputImage */, Result,
				                                        CommandLength - HTAG_COMMAND_NAME_SIZE);
			}
			// This command has parameters that decide if the device reboots or never returns from StoreFlashImage().  It would complicate things if we
			// tried to honor the Response Required here, so that part is done in StoreFlashImage().  Thus, we hard code bRespReq to false.
			bRespReq = false;

			// Only process if we're in the right mode.
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( NewFlashImage.PreliminaryResponseRequired ) xputchar(ACK);
				if( FlashImageMode == SETFLASHIMAGESTOREMODE_FLASH_IMAGE_MODE )
				{
					if(CodePostProcessor != NULL)
						CodePostProcessor->StoreFlashImage(&Status, Result, &NewFlashImage);
				}
				else
				{
					Status = SYN_FE_COMMAND_HANDLER_NOT_IN_APPROPRIATE_FLASH_IMAGE_MODE;
				}
			}
			else
			{
				if ( NewFlashImage.PreliminaryResponseRequired ) xputchar(NAK);
			}

			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;

			// Whatever happened, always turn off flash image mode.
			FlashImageMode = IDLE_FLASH_IMAGE_MODE;
			Device->DefaultCharacterReadTimeoutForHTAGNoProtocol = OriginalDefaultCharacterReadTimeoutForHTAGNoProtocol;
		}

		// Is it a command to turn on flash mode?
		else if(CommandName.Compare("FLSMOD", false) == 0)
		{
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			HTAG	FlashImageModeEventHandler( Device, TransferStatus, CommandHandler, false );
			FlashImageModeEventHandler.CommandName->Copy( "FLSMOD" );
			HSTRING		PasswordParameter( 20, true );
			FlashImageModeEventHandler.AddHStringParameter( "PASSWORD", &PasswordParameter, "", 0, true );
			unsigned char	*DataPtr;
			int		SizeOfData;
			if( FlashImageModeEventHandler.ReadHTagData( &ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE ))
			{
				Status = FlashImageModeEventHandler.LoadTaggedImage( &ReceivedData, &DataPtr, &SizeOfData );
				bRespReq = (bool) FlashImageModeEventHandler.ResponseRequired;
				if ( FlashImageModeEventHandler.PreliminaryResponseRequired ) xputchar(ACK);

				// Process the flash image mode.
				switch( FlashImageMode )
				{
				case IDLE_FLASH_IMAGE_MODE:
					// If we're in idle mode, the only valid password is "SETFLASHIMAGEMODE".
					if( PasswordParameter.Compare( "SETFLASHIMAGEMODE", true ) == 0 )
					{
						FlashImageMode = SETFLASHIMAGEMODE_FLASH_IMAGE_MODE;
					}
					break;

				case SETFLASHIMAGEMODE_FLASH_IMAGE_MODE:
					// If we've turned on flash mode and are waiting to know whether we're to store or retrieve,
					// the only valid passwords are "SETFLASHIMAGESTOREMODE" or "SETFLASHIMAGERETRIEVEMODE".
					if( PasswordParameter.Compare( "SETFLASHIMAGESTOREMODE", true ) == 0 )
					{
						FlashImageMode = SETFLASHIMAGESTOREMODE_FLASH_IMAGE_MODE;
					}
					else if( PasswordParameter.Compare( "SETFLASHIMAGERETRIEVEMODE", true ) == 0 )
					{
						FlashImageMode = SETFLASHIMAGERETRIEVEMODE_FLASH_IMAGE_MODE;
					}
					else
					{
						FlashImageMode = IDLE_FLASH_IMAGE_MODE;
					}
					break;

				default:
					// If we're in any other mode and we get a request to switch mode, that's an error, because the next command
					// should be the actual "FLSSTO" or "FLSRET" command.  Turn off flash image mode.
					FlashImageMode = IDLE_FLASH_IMAGE_MODE;
					break;
				};
			}
			else
			{
				if ( FlashImageModeEventHandler.PreliminaryResponseRequired ) xputchar(NAK);
				Status = HTAG::HTAG_I_O_ERROR;
			}
		}

		// Is it a command to store engine firmware?
		else if( CommandName.Compare( HTAG_SCAN_ENGINE_FIRMWARE_DEFAULT_COMMAND_NAME, false ) == 0 )
		{
			TransferStatus->TransferType = HTRANSFER_STATUS::STORE_ENGINE_FIRMWARE;
			TransferStatus->PrepareDownloadStatus();
			HTAG_SCAN_ENGINE_FIRMWARE	NewEngineFirmware( (char *) CommandName.Char , Device, TransferStatus, CommandHandler);

			// Note that we treat the code images differently due to the fact that they can be so large.  We create a
			// minimum size string to receive data, letting the HTAG or HMODEM processor resize it as needed.  This is done so
			// that HMODEM can use a single buffer for compressed data by resizing OutputImage only once.  This saves lots of heap
			// space in an embedded environment.  If all the data we need has already been received, we'll use that buffer instead.
			//			if( (int) ReceivedData.Size >= ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			if( ReceivedData.Size > 0 )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
				Status = NewEngineFirmware.ReadTaggedImage( &ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE );
			}
			else
			{
				Status = NewEngineFirmware.ReadTaggedImage( NULL /* OutputImage */, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			}
			bRespReq = (bool) NewEngineFirmware.ResponseRequired;

			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( NewEngineFirmware.PreliminaryResponseRequired ) xputchar(ACK);
				if(CodePostProcessor != NULL)
					CodePostProcessor->StoreScanEngineFirmware( &Status, Result, &NewEngineFirmware );
			}
			else
			{
				if ( NewEngineFirmware.PreliminaryResponseRequired ) xputchar(NAK);
			}
			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

		// Is it a command to process a system information request?
		else if( CommandName.Compare( HTAG_GET_SYSTEM_INFORMATION_DEFAULT_COMMAND_NAME, false ) == 0 )
		{
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			TransferStatus->TransferType = HTRANSFER_STATUS::RECEIVING_SYSTEM_INFORMATION_REQUEST;
			TransferStatus->PrepareDownloadStatus();
			HTAG_SYSTEM_INFORMATION	SystemInformation( HTAG_GET_SYSTEM_INFORMATION_DEFAULT_COMMAND_NAME, Device,
			        TransferStatus, CommandHandler );
			Status = SystemInformation.ReadTaggedImage(&ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			bRespReq = (bool) SystemInformation.ResponseRequired;

			// Only process if no bad errors.
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( SystemInformation.PreliminaryResponseRequired ) xputchar(ACK);
				if(ImagePostProcessor != NULL)
				{
					ImagePostProcessor->ProcessSystemInformationRequest( &Status, Result, &SystemInformation );
					if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
					{
						// Change command name before sending back to the caller.
						SystemInformation.CommandName->Copy( HTAG_SYSTEM_INFORMATION_DEFAULT_COMMAND_NAME );
						Status = SystemInformation.WriteTaggedImage(Result);
					}
				}
			}
			else
			{
				if ( SystemInformation.PreliminaryResponseRequired ) xputchar(NAK);
			}

			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

		// Is it a command to process system information?
		else if( CommandName.Compare( HTAG_SYSTEM_INFORMATION_DEFAULT_COMMAND_NAME, false ) == 0 )
		{
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			TransferStatus->TransferType = HTRANSFER_STATUS::RECEIVING_SYSTEM_INFORMATION;
			TransferStatus->PrepareDownloadStatus();
			HTAG_SYSTEM_INFORMATION	SystemInformation( HTAG_SYSTEM_INFORMATION_DEFAULT_COMMAND_NAME, Device,
			        TransferStatus, CommandHandler );
			Status = SystemInformation.ReadTaggedImage(&ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			bRespReq = (bool) SystemInformation.ResponseRequired;
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( SystemInformation.PreliminaryResponseRequired ) xputchar(ACK);
				if(ImagePostProcessor != NULL)
					ImagePostProcessor->ProcessSystemInformation
					( &Status, Result, &SystemInformation );
			}
			else
			{
				if ( SystemInformation.PreliminaryResponseRequired ) xputchar(NAK);
			}
			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

		// Is it a command to process a menu command?
		else if( CommandName.Compare( HTAG_MENU_COMMAND_SEND_MENU_COMMAND, false ) == 0 )
		{
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			TransferStatus->TransferType = HTRANSFER_STATUS::RECEIVING_MENU_COMMAND;
			TransferStatus->PrepareDownloadStatus();
			HTAG_MENU_COMMAND	MenuCommand( HTAG_MENU_COMMAND_SEND_MENU_COMMAND, Device,
			                               TransferStatus, CommandHandler );
			Status = MenuCommand.ReadTaggedImage(&ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			bRespReq = (bool) MenuCommand.ResponseRequired;

			// Only process if no bad errors.
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( MenuCommand.PreliminaryResponseRequired ) xputchar(ACK);
				if(ImagePostProcessor != NULL)
				{
					ImagePostProcessor->ProcessMenuCommand( &Status, Result, &MenuCommand );
					if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
					{
						// Only do the response if requested to.
						if( MenuCommand.CommandResponseRequired )
						{
							// Change command name before sending back to the caller.
							MenuCommand.CommandName->Copy( HTAG_MENU_COMMAND_MENU_COMMAND_RESPONSE );
							Status = MenuCommand.WriteTaggedImage(Result);
						}
					}
				}
			}
			else
			{
				if ( MenuCommand.PreliminaryResponseRequired ) xputchar(NAK);
			}

			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

		// Is it a command to process a menu command response?
		else if( CommandName.Compare( HTAG_MENU_COMMAND_MENU_COMMAND_RESPONSE, false ) == 0 )
		{
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			TransferStatus->TransferType = HTRANSFER_STATUS::RECEIVING_MENU_COMMAND_RESPONSE;
			TransferStatus->PrepareDownloadStatus();
			HTAG_MENU_COMMAND	MenuCommandResponse( HTAG_MENU_COMMAND_MENU_COMMAND_RESPONSE, Device,
			                                       TransferStatus, CommandHandler );
			Status = MenuCommandResponse.ReadTaggedImage(&ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			bRespReq = (bool) MenuCommandResponse.ResponseRequired;

			// Only process if no bad errors.
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( MenuCommandResponse.PreliminaryResponseRequired ) xputchar(ACK);
				if(ImagePostProcessor != NULL)
				{
					ImagePostProcessor->ProcessMenuCommandResponse( &Status, Result, &MenuCommandResponse );
				}
			}
			else
			{
				if ( MenuCommandResponse.PreliminaryResponseRequired ) xputchar(NAK);
			}

			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

	// Is it a command to process a product config menu command?
		else if( CommandName.Compare( HTAG_MENU_COMMAND_SEND_CONFIG_MENU_COMMAND, false ) == 0 )
		{
		// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
		// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			TransferStatus->TransferType = HTRANSFER_STATUS::RECEIVING_MENU_COMMAND;
			TransferStatus->PrepareDownloadStatus();
			HTAG_MENU_COMMAND	MenuCommand( HTAG_MENU_COMMAND_SEND_CONFIG_MENU_COMMAND, Device,
									TransferStatus, CommandHandler );
			Status = MenuCommand.ReadTaggedImage(&ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			bRespReq = (bool) MenuCommand.ResponseRequired;

		// Only process if no bad errors.
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( MenuCommand.PreliminaryResponseRequired ) xputchar(ACK);
				if(ImagePostProcessor != NULL)
				{
					ImagePostProcessor->ProcessConfigMenuCommand( &Status, Result, &MenuCommand );
					if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
					{
					// Only do the response if requested to.
						if( MenuCommand.CommandResponseRequired )
						{
						// Change command name before sending back to the caller.
							MenuCommand.CommandName->Copy( HTAG_MENU_COMMAND_MENU_COMMAND_RESPONSE );
							Status = MenuCommand.WriteTaggedImage(Result);
						}
					}
				}
			}
			else
			{
				if ( MenuCommand.PreliminaryResponseRequired ) xputchar(NAK);
			}

			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

	// Is it a command to process a menu command response?
		else if( CommandName.Compare( HTAG_MENU_COMMAND_CONFIG_MENU_COMMAND_RESPONSE, false ) == 0 )
		{
		// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
		// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			TransferStatus->TransferType = HTRANSFER_STATUS::RECEIVING_MENU_COMMAND_RESPONSE;
			TransferStatus->PrepareDownloadStatus();
			HTAG_MENU_COMMAND	MenuCommandResponse( HTAG_MENU_COMMAND_CONFIG_MENU_COMMAND_RESPONSE, Device,
									TransferStatus, CommandHandler );
			Status = MenuCommandResponse.ReadTaggedImage(&ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			bRespReq = (bool) MenuCommandResponse.ResponseRequired;

		// Only process if no bad errors.
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( MenuCommandResponse.PreliminaryResponseRequired ) xputchar(ACK);
				if(ImagePostProcessor != NULL)
				{
					ImagePostProcessor->ProcessConfigMenuCommandResponse( &Status, Result, &MenuCommandResponse );
				}
			}
			else
			{
				if ( MenuCommandResponse.PreliminaryResponseRequired ) xputchar(NAK);
			}

			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

		// Is it a command to process a non-menu command?
		else if( CommandName.Compare( HTAG_MENU_COMMAND_SEND_NON_MENU_COMMAND, false ) == 0 )
		{
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			TransferStatus->TransferType = HTRANSFER_STATUS::RECEIVING_NON_MENU_COMMAND;
			TransferStatus->PrepareDownloadStatus();
			HTAG_MENU_COMMAND	NonMenuCommand( HTAG_MENU_COMMAND_SEND_NON_MENU_COMMAND, Device,
			                                  TransferStatus, CommandHandler );
			Status = NonMenuCommand.ReadTaggedImage(&ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			bRespReq = (bool) NonMenuCommand.ResponseRequired;

			// Only process if no bad errors.
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( NonMenuCommand.PreliminaryResponseRequired ) xputchar(ACK);
				if(ImagePostProcessor != NULL)
				{
					ImagePostProcessor->ProcessNonMenuCommand( &Status, Result, &NonMenuCommand );
					if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
					{
						// Only do the response if requested to.
						if( NonMenuCommand.CommandResponseRequired )
						{
							// Change command name before sending back to the caller.
							NonMenuCommand.CommandName->Copy( HTAG_MENU_COMMAND_NON_MENU_COMMAND_RESPONSE );
							Status = NonMenuCommand.WriteTaggedImage(Result);
						}
					}
				}
			}
			else
			{
				if ( NonMenuCommand.PreliminaryResponseRequired ) xputchar(NAK);
			}

			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

		// Is it a command to process a non-menu command response?
		else if( CommandName.Compare( HTAG_MENU_COMMAND_NON_MENU_COMMAND_RESPONSE, false ) == 0 )
		{
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			TransferStatus->TransferType = HTRANSFER_STATUS::RECEIVING_NON_MENU_COMMAND_RESPONSE;
			TransferStatus->PrepareDownloadStatus();
			HTAG_MENU_COMMAND	NonMenuCommandResponse( HTAG_MENU_COMMAND_NON_MENU_COMMAND_RESPONSE, Device,
			        TransferStatus, CommandHandler );
			Status = NonMenuCommandResponse.ReadTaggedImage(&ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			bRespReq = (bool) NonMenuCommandResponse.ResponseRequired;

			// Only process if no bad errors.
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( NonMenuCommandResponse.PreliminaryResponseRequired ) xputchar(ACK);
				if(ImagePostProcessor != NULL)
				{
					ImagePostProcessor->ProcessNonMenuCommandResponse( &Status, Result, &NonMenuCommandResponse );
				}
			}
			else
			{
				if ( NonMenuCommandResponse.PreliminaryResponseRequired ) xputchar(NAK);
			}

			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

		// Is it a command to process an activity status command?
		else if( CommandName.Compare( HTAG_ACTIVITY_STATUS_DEFAULT_COMMAND_NAME, false ) == 0 )
		{
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			TransferStatus->TransferType = HTRANSFER_STATUS::RECEIVING_ACTIVITY_STATUS;
			TransferStatus->PrepareDownloadStatus();
			HTAG_ACTIVITY_STATUS	ActivityStatus( HTAG_ACTIVITY_STATUS_DEFAULT_COMMAND_NAME, Device,
			                                     TransferStatus, CommandHandler );
			Status = ActivityStatus.ReadTaggedImage(&ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
			bRespReq = (bool) ActivityStatus.ResponseRequired;

			// Only process if no bad errors.
			if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
			{
				if ( ActivityStatus.PreliminaryResponseRequired ) xputchar(ACK);
				if(ImagePostProcessor != NULL)
				{
					ImagePostProcessor->ProcessActivityStatus( &Status, Result, &ActivityStatus );
				}
			}
			else
			{
				if ( ActivityStatus.PreliminaryResponseRequired ) xputchar(NAK);
			}

			TransferStatus->CloseDownloadStatus();
			TransferStatus->TransferType = StartingTransferType;
		}

		// Is it a command to process Verifier calibration data?
		//		else if( HTAG_VERIFIER_CALIBRATION_DATA::ReceiveVerifierCommand( Result, &Status, &CommandName,
		//				&ReceivedData, CommandLength, Device, TransferStatus, CommandHandler, PostTransferProcessor ) )
		//		{
		//		}

		// Unrecognized command; WOOF!
		else
		{
			// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
			// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
			if( (int) ReceivedData.Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
			{
				ReceivedData.Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
			}

			// We don't recognize the command, so let's consume the remainder of the input.
			HTAG	UnknownCommandHandler( Device, TransferStatus, CommandHandler, false );
			UnknownCommandHandler.CommandName->Copy( &CommandName );

			//Make protocol parameter optional
			UnknownCommandHandler.Parameters[ UnknownCommandHandler.TransferProtocolParameterNumber ].AlwaysReport = false;

			HSTRING	CopyOfProtocolParameterString( 20, true );

			if( !UnknownCommandHandler.ReadHTagData( &ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE,
			        &CopyOfProtocolParameterString ))
			{
				bRespReq = (bool) UnknownCommandHandler.ResponseRequired;
				if ( UnknownCommandHandler.PreliminaryResponseRequired ) xputchar(NAK);
				Status = HTAG::HTAG_I_O_ERROR;
			}
			else
			{
				if ( UnknownCommandHandler.PreliminaryResponseRequired ) xputchar(ACK);
			}

			bRespReq = (bool) UnknownCommandHandler.ResponseRequired;
			// Put the header back into the beginning of the data stream.
			ReceivedData.Insert( &CopyOfProtocolParameterString, 0 );
			ReceivedData.Insert( UnusedCommandData, 0 );
			ReceivedData.Insert( DataReceivedSoFar, LengthOfDataReceivedSoFar, 0 );
			if(EventProcessor != NULL)
				EventProcessor->UnknownSYNFECommandReceived( &Status, Result, &CommandName,
				        &ReceivedData );

			strcpy(Result, "Unrecognized <STX>FE command");
			Status = HTAG::HTAG_UNRECOGNIZED_COMMAND;
		}
	}

	if ( bRespReq )
	{
		if ( (Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) ) xputchar(ACK);
		else xputchar(NAK);
	}

	// If boot code mode hasn't changed, reset it.
	if( BootCodeMode == LastBootCodeMode )
	{
		BootCodeMode = IDLE_BOOT_CODE_MODE;
	}
	LastBootCodeMode = BootCodeMode;


	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return(Status);
} // ReceiveAndParseServer (overload #1)







//
//	ReceiveAndParseServer (overload #2, where length of input data stream is implied by NULL termination).
//		See if a <SYN>FE command has been received.  The argument to ReceiveAndParseServer is the data that has been received
//		so far to start a <SYN>FE command.  If the string is NULL, ReceiveAndParseServer will wait for WaitTimeForCommand
//		milliseconds to receive command data.  Otherwise, the string contains the start of a <SYN>FE command and
//		ReceiveAndParseServer must receive and process the rest of the data.
//
int SYN_FE_COMMAND_HANDLER::ReceiveAndParseServer(char *Result, const char *DataReceivedSoFar )
{
	return ReceiveAndParseServer( Result, DataReceivedSoFar, strlen( DataReceivedSoFar ) );
} // ReceiveAndParseServer (overload #2)





//
//	WriteMatrixImageData (overloaded version #1, where HImage data sent by the caller is written)
//		Writes the image data to a Matrix device.
//
bool SYN_FE_COMMAND_HANDLER::WriteMatrixImageData(HTAG_IMAGE *ImageData, char *Result)
{
	bool		Status = true;


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	int		StartingTransferType = TransferStatus->TransferType;
	TransferStatus->TransferType = HTRANSFER_STATUS::SEND_IMAGE_FOR_PROCESSING;
	TransferStatus->PrepareDownloadStatus();

	ImageData->Device = Device;
	ImageData->TransferStatus = TransferStatus;
	ImageData->CommandHandler = CommandHandler;

	// Write the image.
	Status = (ImageData->WriteTaggedImage(Result) == HTAG::HTAG_OK) ? true : false;

	TransferStatus->CloseDownloadStatus();
	TransferStatus->TransferType = StartingTransferType;

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return(Status);
} // WriteMatrixImageData (overloaded version #1)





//
//	WriteMatrixImageData (overloaded version #2, where 8 bit image data sent by the caller is converted to an HIMAGE)
//		Writes the image data to a Matrix device.
//
bool SYN_FE_COMMAND_HANDLER::WriteMatrixImageData(const char *CommandName, HSTRING *ImageData, int XDimension, int YDimension,
        char *Result, int Protocol, int Format)
{
	bool		Status = true;


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	int		StartingTransferType = TransferStatus->TransferType;
	TransferStatus->TransferType = HTRANSFER_STATUS::SEND_IMAGE_FOR_PROCESSING;
	TransferStatus->PrepareDownloadStatus();

	// Create the HTAG_IMAGe
	HTAG_IMAGE	NewImage(CommandName, Device, TransferStatus, CommandHandler);
	NewImage.LeftBoundary = 0;
	NewImage.RightBoundary = XDimension - 1;
	NewImage.TopBoundary = 0;
	NewImage.BottomBoundary = YDimension - 1;
	NewImage.Data->Copy(ImageData);
	NewImage.TransferProtocol = Protocol;
	NewImage.Format = Format;

	// Write the image.
	Status = (NewImage.WriteTaggedImage(Result) == HTAG::HTAG_OK) ? true : false;

	TransferStatus->CloseDownloadStatus();
	TransferStatus->TransferType = StartingTransferType;

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();


	return(Status);
} // WriteMatrixImageData (overloaded version #2)





//
//	WriteScript
//		Write script data to a Matrix device in HTag format.
//
bool SYN_FE_COMMAND_HANDLER::WriteScript(const char *CommandName, const char *FileName, int WorkGroup, HSTRING *ScriptData,
        char *Result, int Protocol, int DesignateAsDefaultScript)
{
	bool		Status = true;			// Assume success for now.


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	int		StartingTransferType = TransferStatus->TransferType;
	TransferStatus->TransferType = HTRANSFER_STATUS::STORE_SCRIPT;
	TransferStatus->PrepareDownloadStatus();

	// Create the script.
	HTAG_SCRIPT		NewScript(CommandName, Device, TransferStatus, CommandHandler);
	NewScript.ScriptName->Copy(FileName);
	NewScript.TransferProtocol = Protocol;
	NewScript.ScriptWorkgroup = WorkGroup;
	NewScript.DefaultScript = DesignateAsDefaultScript;
	NewScript.Data->Copy(ScriptData);

	// Write the script.
	Status = (NewScript.WriteTaggedImage(Result) == HTAG::HTAG_OK) ? true : false;

	TransferStatus->CloseDownloadStatus();
	TransferStatus->TransferType = StartingTransferType;

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return(Status);
} // WriteScript





//
//	WriteMainScript
//		Write main script data to a Matrix device in HTag format.
//
bool SYN_FE_COMMAND_HANDLER::WriteMainScript(const char *FileName, int WorkGroup, HSTRING *ScriptData, char *Result,
        int Protocol, int DesignateAsDefaultScript)
{
	return(WriteScript("HPLSMS", FileName, WorkGroup, ScriptData, Result, Protocol, DesignateAsDefaultScript));
} // WriteMainScript





//
//	WriteEditScript
//		Write edit script data to a Matrix device in HTag format.
//
bool SYN_FE_COMMAND_HANDLER::WriteEditScript(const char *FileName, int WorkGroup, HSTRING *ScriptData, char *Result,
        int Protocol, int DesignateAsDefaultScript)
{
	return(WriteScript("HPLSES", FileName, WorkGroup, ScriptData, Result, Protocol, DesignateAsDefaultScript));
} // WriteEditScript





//
//	WriteFile
//		Write a file to a Matrix device in HTag format.
//
bool SYN_FE_COMMAND_HANDLER::WriteFile(const char *FileName, int WorkGroup, HSTRING *FileData, char *Result, int Protocol,
                                       int CompressionType, int FileType, const char *CommandName )
{
	bool		Status = true;			// Assume success for now.


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	// Create the script.
	HTAG_FILE		NewFile(CommandName, Device, TransferStatus, CommandHandler);
	NewFile.FileName->Copy(FileName);
	NewFile.TransferProtocol = Protocol;
	NewFile.FileWorkgroup = WorkGroup;
	NewFile.CompressionType = CompressionType;
	NewFile.FileType = FileType;
	NewFile.Data->Copy(FileData);

	if( NewFile.ValidateParameters() == HTAG::HTAG_OK )
	{
		int		StartingTransferType = TransferStatus->TransferType;
		TransferStatus->TransferType = HTRANSFER_STATUS::STORE_FILE;
		TransferStatus->PrepareDownloadStatus();

		// Write the file.
		Status = (NewFile.WriteTaggedImage(Result) == HTAG::HTAG_OK) ? true : false;

		TransferStatus->CloseDownloadStatus();
		TransferStatus->TransferType = StartingTransferType;
	}
	else
	{
		strcpy( Result, "WriteFile failed - Illegal file parameters" );
		Status = false;
	}

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return(Status);
} // WriteFile





//
//	SendTriggerEvent
//		Send a trigger event.
//
bool SYN_FE_COMMAND_HANDLER::SendTriggerEvent(int TriggerState)
{
	char		Result[80];				// Holds status information.
	bool		Status = true;			// Assume success for now.


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	// Create the script.
	HTAG	TriggerEventCommand( Device, TransferStatus, CommandHandler, false );
	TriggerEventCommand.CommandName->Copy( "TRGEVT" );
	HSTRING	CommandString(20, true);
	HSTRING	TaggedData(20, true);
	TriggerEventCommand.AddNumericParameter(HTAG_TRIGGER_EVENT_TYPE_PARAMETER_NAME, &TriggerState,
	                                        READER_DEVICE_EVENT_PROCESSOR::MANUALLY_TRIGGERED, READER_DEVICE_EVENT_PROCESSOR::UNTRIGGERED,
	                                        READER_DEVICE_EVENT_PROCESSOR::AUTO_TRIGGERED, true );
	TriggerEventCommand.CreateTaggedImage( &CommandString, &TaggedData, (unsigned char *) "", 0, true);
	Status = TriggerEventCommand.WriteHTagData( &CommandString, &TaggedData, Result );

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return(Status);
} // SendTriggerEvent





//
//	WriteApplicationCode
//		Write application code to a Matrix device in HTag format.
//
bool SYN_FE_COMMAND_HANDLER::WriteApplicationCode( HSTRING *ApplicationCode, char *Result, int Protocol, bool StoreFile,
        bool OverrideApplicationTypeCheck, bool OverrideDeviceTypeCheck,
        int CompressionType, const char *CommandName )
{
	bool		Status = true;			// Assume success for now.


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	// Create the script.
	HTAG_READER_CODE	CodeImage(CommandName, Device, TransferStatus, CommandHandler);
	CodeImage.FileName->Copy( "mainAPP" );
	CodeImage.CompressionType = CompressionType;
	CodeImage.Data->Copy( ApplicationCode );
	CodeImage.TransferProtocol = Protocol;
	CodeImage.StoreFile = ( StoreFile ) ? 1 : 0;
	CodeImage.UpdateParameter( HTAG_READER_CODE_OVERRIDE_APPLICATION_TYPE_CHECK_PARAMETER_NAME, OverrideApplicationTypeCheck );
	CodeImage.UpdateParameter( HTAG_READER_CODE_OVERRIDE_DEVICE_TYPE_CHECK_PARAMETER_NAME, OverrideDeviceTypeCheck );

	if( CodeImage.ValidateParameters() == HTAG::HTAG_OK )
	{
		int		StartingTransferType = TransferStatus->TransferType;
		TransferStatus->TransferType = HTRANSFER_STATUS::STORE_APPLICATION_CODE;
		TransferStatus->PrepareDownloadStatus();

		// Write the file.
		Status = ( CodeImage.WriteTaggedImage(Result) == HTAG::HTAG_OK) ? true : false;

		TransferStatus->CloseDownloadStatus();
		TransferStatus->TransferType = StartingTransferType;
	}
	else
	{
		strcpy( Result, "WriteApplicationCode failed - Illegal file parameters" );
		Status = false;
	}

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return(Status);
} // WriteApplicationCode





//
//	TurnOnBootMode
//		Send the boot mode commands to turn on boot mode.
//
bool SYN_FE_COMMAND_HANDLER::TurnOnBootMode( char *Result, bool RetrieveMode )
{
	bool		Status = true;			// Assume success for now.


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	// Turn on boot mode.
	HTAG		BootModeCommand( Device, TransferStatus, CommandHandler, false );
	HSTRING		BootModePasswordParameter( 20, true );
	BootModeCommand.CommandName->Copy( "BOTMOD" );
	HSTRING	CommandString(20, true);
	HSTRING	TaggedData(20, true);
	BootModeCommand.AddHStringParameter( "PASSWORD", &BootModePasswordParameter, "", 0, true );
	BootModePasswordParameter.Copy( "SETBOOTCODEMODE" );
	BootModeCommand.TransferProtocol = HTAG::HTAG_TRANSMIT_NO_PROTOCOL;
	BootModeCommand.CreateTaggedImage( &CommandString, &TaggedData, (unsigned char *) "", 0, true);
	Status = BootModeCommand.WriteHTagData( &CommandString, &TaggedData, Result );

	if( Status )
	{
		// Turn on boot store mode.
		if( RetrieveMode )
		{
			BootModePasswordParameter.Copy( "SETBOOTCODERETRIEVEMODE" );
		}
		else
		{
			BootModePasswordParameter.Copy( "SETBOOTCODESTOREMODE" );
		}

		BootModeCommand.CreateTaggedImage( &CommandString, &TaggedData, (unsigned char *) "", 0, true);
		Status = BootModeCommand.WriteHTagData( &CommandString, &TaggedData, Result );
	}

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return(Status);
} // TurnOnBootMode





//
//	WriteBootCode
//		Write boot code to a Matrix device in HTag format.
//
bool SYN_FE_COMMAND_HANDLER::WriteBootCode( HSTRING *BootCode, char *Result, int Protocol, bool OverrideDeviceTypeCheck,
        const char *CommandName, int Key )
{
	bool		Status = true;			// Assume success for now.


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	if( Key != SPECIAL_MODE_KEY )
	{
		Status = false;
	}

	else
	{
		Status = TurnOnBootMode( Result, false );

		if( Status )
		{
			// Create the script.
			HTAG_READER_CODE	CodeImage(CommandName, Device, TransferStatus, CommandHandler);
			CodeImage.FileName->Copy( "BootCode" );
			CodeImage.CompressionType = NO_COMPRESSION;
			CodeImage.Data->Copy( BootCode );
			CodeImage.TransferProtocol = Protocol;
			CodeImage.UpdateParameter( HTAG_READER_CODE_OVERRIDE_DEVICE_TYPE_CHECK_PARAMETER_NAME, OverrideDeviceTypeCheck );

			if( CodeImage.ValidateParameters() == HTAG::HTAG_OK )
			{
				int		StartingTransferType = TransferStatus->TransferType;
				TransferStatus->TransferType = HTRANSFER_STATUS::STORE_BOOT_CODE;
				TransferStatus->PrepareDownloadStatus();

				// Write the file.
				Status = (CodeImage.WriteTaggedImage(Result) == HTAG::HTAG_OK) ? true : false;

				TransferStatus->CloseDownloadStatus();
				TransferStatus->TransferType = StartingTransferType;
			}
			else
			{
				strcpy( Result, "WriteBootCode failed - Illegal file parameters" );
				Status = false;
			}
		}
	}

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return(Status);
} // WriteBootCode





//
//	TurnOnFlashImageMode
//		Send the flash image mode commands to turn on flash image mode.
//
bool SYN_FE_COMMAND_HANDLER::TurnOnFlashImageMode( char *Result, bool RetrieveMode )
{
	bool		Status = true;			// Assume success for now.


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	// Turn on flash image mode.
	HTAG		FlashImageModeCommand( Device, TransferStatus, CommandHandler, false );
	HSTRING		FlashImagePasswordParameter( 20, true );
	FlashImageModeCommand.CommandName->Copy( "FLSMOD" );
	HSTRING	CommandString(20, true);
	HSTRING	TaggedData(20, true);
	FlashImageModeCommand.AddHStringParameter( "PASSWORD", &FlashImagePasswordParameter, "", 0, true );
	FlashImagePasswordParameter.Copy( "SETFLASHIMAGEMODE" );
	FlashImageModeCommand.TransferProtocol = HTAG::HTAG_TRANSMIT_NO_PROTOCOL;
	FlashImageModeCommand.CreateTaggedImage( &CommandString, &TaggedData, (unsigned char *) "", 0, true);
	Status = FlashImageModeCommand.WriteHTagData( &CommandString, &TaggedData, Result );

	if( Status )
	{
		// Turn on flash image store mode.
		if( RetrieveMode )
		{
			FlashImagePasswordParameter.Copy( "SETFLASHIMAGERETRIEVEMODE" );
		}
		else
		{
			FlashImagePasswordParameter.Copy( "SETFLASHIMAGESTOREMODE" );
		}

		FlashImageModeCommand.CreateTaggedImage( &CommandString, &TaggedData, (unsigned char *) "", 0, true);
		Status = FlashImageModeCommand.WriteHTagData( &CommandString, &TaggedData, Result );
	}

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return(Status);
} // TurnOnFlashImageMode





//
//	WriteFlashImage
//		Write a flash image to the device.
//
bool SYN_FE_COMMAND_HANDLER::WriteFlashImage( HSTRING *InputFlashImage, char *Result, int Protocol, int StartAddress,
        bool Reboot, bool BootMode, int CompressionType, const char *CommandName, int Key, bool WaitForSecondACK,
        bool DoIndicators, bool StopAfterDownload, bool SaveSerialNumbers, bool PreliminaryResponseRequired )
{
	bool		Status = true;			// Assume success for now.


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	if( Key != SPECIAL_MODE_KEY )
	{
		Status = false;
	}

	else
	{
		Status = TurnOnFlashImageMode( Result, false );

		if( Status )
		{
			// Create the image.
			HTAG_READER_CODE	FlashImage(CommandName, Device, TransferStatus, CommandHandler);
			FlashImage.FileName->Copy( "FlashImage" );
			FlashImage.CompressionType = CompressionType;
			FlashImage.Data->Copy( InputFlashImage );
			FlashImage.TransferProtocol = Protocol;
			FlashImage.StartAddress = StartAddress;
			FlashImage.BootModeFlashImage = ( BootMode ) ? 1 : 0;
			FlashImage.Reboot = ( Reboot ) ? 1 : 0;
			FlashImage.CRC = (int) calc_crc( InputFlashImage->Char, InputFlashImage->Size );
			FlashImage.UpdateParameter( HTAG_READER_CODE_DO_INDICATORS, DoIndicators ? 1 : 0 );
			FlashImage.UpdateParameter( HTAG_READER_CODE_STOP_AFTER_DOWNLOAD, StopAfterDownload ? 1 : 0 );
			FlashImage.UpdateParameter( HTAG_READER_CODE_SAVE_SERIAL_NUMBERS, SaveSerialNumbers ? 1 : 0 );
			FlashImage.UpdateParameter( HTAG_PRELIMINARY_RESPONSE_PARAMETER_NAME, PreliminaryResponseRequired ? 1 : 0 );

			if( FlashImage.ValidateParameters() == HTAG::HTAG_OK )
			{
				int		StartingTransferType = TransferStatus->TransferType;
				TransferStatus->TransferType = HTRANSFER_STATUS::STORE_FLASH_IMAGE;
				TransferStatus->PrepareDownloadStatus();

				// Write the image.
				Status = ( FlashImage.WriteTaggedImage( Result, WaitForSecondACK ) == HTAG::HTAG_OK) ? true : false;

				TransferStatus->CloseDownloadStatus();
				TransferStatus->TransferType = StartingTransferType;
			}
			else
			{
				strcpy( Result, "WriteFlashImage failed - Illegal file parameters" );
				Status = false;
			}
		}
	}

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return(Status);
} // WriteFlashImage





//
//	ReadFlashImage
//		Retrieve a flash image from the device.
//
bool SYN_FE_COMMAND_HANDLER::ReadFlashImage( char *Result, int Protocol, int StartAddress, int SizeOfImage, bool Reboot,
        bool BootMode, int CompressionType, const char *CommandName, int Key )
{
	bool		Status = true;			// Assume success for now.


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	if( Key != SPECIAL_MODE_KEY )
	{
		Status = false;
	}

	else
	{
		Status = TurnOnFlashImageMode( Result, true );

		if( Status )
		{
			// Create the image.
			HTAG_READER_CODE	FlashImage(CommandName, Device, TransferStatus, CommandHandler);
			FlashImage.FileName->Copy( "FlashImage" );
			FlashImage.CompressionType = CompressionType;
			FlashImage.TransferProtocol = Protocol;
			FlashImage.StartAddress = StartAddress;
			FlashImage.SizeOfImage = SizeOfImage;
			FlashImage.Reboot = ( Reboot ) ? 1 : 0;
			FlashImage.BootModeFlashImage = (BootMode ) ? 1 : 0;

			if( FlashImage.ValidateParameters() == HTAG::HTAG_OK )
			{
				int		StartingTransferType = TransferStatus->TransferType;
				TransferStatus->TransferType = HTRANSFER_STATUS::RETRIEVE_FLASH_IMAGE;
				TransferStatus->PrepareDownloadStatus();

				// Write the image.
				Status = ( FlashImage.WriteTaggedImage( Result ) == HTAG::HTAG_OK) ? true : false;

				TransferStatus->CloseDownloadStatus();
				TransferStatus->TransferType = StartingTransferType;
			}
			else
			{
				strcpy( Result, "ReadFlashImage failed - Illegal file parameters" );
				Status = false;
			}
		}
	}

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return(Status);
} // ReadFlashImage







//
//	WriteScanEngineFirmware (overload #1, using HTAG_SCAN_ENGINE_FIRMWARE input)
//		Write scan engine firmware to a Matrix device in HTag format.
//
bool SYN_FE_COMMAND_HANDLER::WriteScanEngineFirmware( HTAG_SCAN_ENGINE_FIRMWARE *ScanEngineFirmware, char *Result )
{
	bool		Status = true;


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	int		StartingTransferType = TransferStatus->TransferType;
	TransferStatus->TransferType = HTRANSFER_STATUS::STORE_ENGINE_FIRMWARE;
	TransferStatus->PrepareDownloadStatus();

	ScanEngineFirmware->Device = Device;
	ScanEngineFirmware->TransferStatus = TransferStatus;
	ScanEngineFirmware->CommandHandler = CommandHandler;

	// Write the firmware.
	Status = ( ScanEngineFirmware->WriteTaggedImage(Result) == HTAG::HTAG_OK) ? true : false;

	TransferStatus->CloseDownloadStatus();
	TransferStatus->TransferType = StartingTransferType;

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return Status;
} // WriteScanEngineFirmware (overload #1)





//
//	WriteScanEngineFirmware (overload #2, using individual parameter input)
//		Write scan engine firmware to a Matrix device in HTag format.
//
bool SYN_FE_COMMAND_HANDLER::WriteScanEngineFirmware( HSTRING *ScanEngineFirmware, char *Result, int Protocol,
        int CompressionType, const char *CommandName, const char *FileName, int StorageType, bool ResponseRequired,
        bool IgnoreFirmwareTypeMismatch )
{
	bool		Status = true;			// Assume success for now.


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	// Create the script.
	HTAG_SCAN_ENGINE_FIRMWARE	FirmwareImage(CommandName, Device, TransferStatus, CommandHandler);
	FirmwareImage.UpdateParameter( HTAG_SCAN_ENGINE_FIRMWARE_FILE_PARAMETER_NAME, FileName );
	FirmwareImage.UpdateParameter( HTAG_SCAN_ENGINE_FIRMWARE_COMPRESSION_TYPE_PARAMETER_NAME, CompressionType );
	FirmwareImage.Data->Copy( ScanEngineFirmware );
	FirmwareImage.UpdateParameter( HTAG_PROTOCOL_PARAMETER_NAME, Protocol );
	FirmwareImage.UpdateParameter( HTAG_READER_CODE_CRC_PARAMETER_NAME,
	                               (int) calc_crc( ScanEngineFirmware->Char, ScanEngineFirmware->Size ) );
	FirmwareImage.UpdateParameter( HTAG_SCAN_ENGINE_FIRMWARE_STORE_FILE_PARAMETER_NAME, StorageType );
	FirmwareImage.UpdateParameter( HTAG_RESPONSE_PARAMETER_NAME, ResponseRequired );
	FirmwareImage.UpdateParameter( HTAG_SCAN_ENGINE_FIRMWARE_IGNORE_TYPE_MISMATCH_PARAMETER_NAME,
	                               IgnoreFirmwareTypeMismatch ? 1 : 0 );

	if( FirmwareImage.ValidateParameters() == HTAG::HTAG_OK )
	{
		int		StartingTransferType = TransferStatus->TransferType;
		TransferStatus->TransferType = HTRANSFER_STATUS::STORE_ENGINE_FIRMWARE;
		TransferStatus->PrepareDownloadStatus();

		// Write the file.
		Status = ( FirmwareImage.WriteTaggedImage(Result) == HTAG::HTAG_OK) ? true : false;

		// Wait for the response if we're supposed to.
		if( ResponseRequired )
		{
			Status = false;			// Don't assume success until we receive a NAK.
			strcpy( Result, "No response received" );

			// Wait for the response.
			unsigned int		MilliSecondsToWait = (unsigned int) Device->WaitTimeForScanEngineFirmwareDownload;
			HSTRING	Response( 20, true );
			int		i;

			bool			ACKorNAKFound = false;

			Device->ZeroElapsedTime();
			do
			{
				if( Device->Read( &Response, 1, Device->DefaultCharacterReadTimeout ) )
				{
					if( Response.Size > 0 )
					{
						// Look for an ACK or NAK
						i = 0;
						while(( i < (int) Response.Size ) && ( !ACKorNAKFound ))
						{
							if( Response.Char[ i ] == ACK )
							{
								strcpy( Result, "ACK was received" );
								ACKorNAKFound = true;
								Status = true;
							}
							else if( Response.Char[ i ] == NAK )
							{
								ACKorNAKFound = true;
								strcpy( Result, "NAK was received" );
							}
							else
							{
								PostTransferProcessor->ProcessExtraneousDataReceivedDuringOrAfterTransfer( &Response );
								++i;
							}
						}
					}
				}
			}
			while(( ( Device->ElapsedTime() * 1000 ) < MilliSecondsToWait ) && ( !ACKorNAKFound ));
		}

		TransferStatus->CloseDownloadStatus();
		TransferStatus->TransferType = StartingTransferType;
	}
	else
	{
		strcpy( Result, "WriteScanEngineFirmware failed - Illegal file parameters" );
		Status = false;
	}

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return(Status);
} // WriteScanEngineFirmware (overload #2)







//
//	SendDecodeResult
//		Send a decode result.
//
bool SYN_FE_COMMAND_HANDLER::SendDecodeResult( char *Result, int Protocol, const char *DecodeResultData,
        int SizeOfDecodeResultData, const char *HHPCodeID, const char *AIMID, bool WaitForResponse,
        bool ReturnDecodeResultHostTransmitAcknowledge, bool WideCharacterFormat, int IndicatorRequired,
        int SequenceNumber, int IntermessageDelay )
{
	bool		Status = true;			// Assume success for now.


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	// Create the decode result.
	HTAG_DECODE_RESULT	DecodeResult( HTAG_DECODE_RESULT_DEFAULT_COMMAND_NAME, Device, TransferStatus, CommandHandler );
	DecodeResult.SetDecodeResultParameters( Protocol, DecodeResultData, SizeOfDecodeResultData, HHPCodeID, AIMID,
	                                        WaitForResponse, ReturnDecodeResultHostTransmitAcknowledge, WideCharacterFormat, IndicatorRequired,
	                                        SequenceNumber, IntermessageDelay );

	int		StartingTransferType = TransferStatus->TransferType;
	TransferStatus->TransferType = HTRANSFER_STATUS::SENDING_DECODE_RESULT;
	TransferStatus->PrepareDownloadStatus();

	// Write the result.
	Status = ( DecodeResult.WriteTaggedImage( Result ) == HTAG::HTAG_OK) ? true : false;

	TransferStatus->CloseDownloadStatus();
	TransferStatus->TransferType = StartingTransferType;

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return(Status);
} // SendDecodeResult





//
//	SendWideDecodeResult
//		Send a wchar_t decode result.
//
bool SYN_FE_COMMAND_HANDLER::SendWideDecodeResult( char *Result, int Protocol, const wchar_t *DecodeResultData,
        int SizeOfDecodeResultData, const char *HHPCodeID, const char *AIMID, bool WaitForResponse,
        bool ReturnDecodeResultHostTransmitAcknowledge, int IndicatorRequired, int SequenceNumber,
        int IntermessageDelay )
{
	HWSTRING	hwstrDecodeResult( SizeOfDecodeResultData, true );
	HSTRING		hstrDecodeResult( SizeOfDecodeResultData * 2, true );
	hwstrDecodeResult.Copy( DecodeResultData, SizeOfDecodeResultData );
	hwstrDecodeResult.CopyToHString( &hstrDecodeResult );

	return SendDecodeResult( Result, Protocol, (char *) hstrDecodeResult.Char, hstrDecodeResult.Size, HHPCodeID, AIMID,
	                         WaitForResponse, ReturnDecodeResultHostTransmitAcknowledge, true, IndicatorRequired, SequenceNumber,
	                         IntermessageDelay );
} // SendWideDecodeResult





//
//	SendDecodeResultHostTransmitAcknowledge
//		Send a host transmit acknowledge for adecode result.
//
bool SYN_FE_COMMAND_HANDLER::SendDecodeResultHostTransmitAcknowledge( char *Result, int Protocol, bool WaitForResponse,
        int IndicatorRequired, int SequenceNumber )
{
	bool		Status = true;			// Assume success for now.


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	// Create the decode result host transmit acknowledge.
	HTAG_DECODE_RESULT	DecodeResultHostTransmitAcknowledge( HTAG_DECODE_RESULT_HOST_TRANSMIT_ACKNOWLEDGE_COMMAND_NAME,
	        Device, TransferStatus, CommandHandler );
	DecodeResultHostTransmitAcknowledge.SetDecodeResultHostTransmitAcknowledgeParameters( Protocol, WaitForResponse,
	        IndicatorRequired, SequenceNumber );

	int		StartingTransferType = TransferStatus->TransferType;
	TransferStatus->TransferType = HTRANSFER_STATUS::SENDING_DECODE_RESULT_ACKNOWLEDGE;
	TransferStatus->PrepareDownloadStatus();

	// Write the result.
	Status = ( DecodeResultHostTransmitAcknowledge.WriteTaggedImage( Result ) == HTAG::HTAG_OK) ? true : false;

	TransferStatus->CloseDownloadStatus();
	TransferStatus->TransferType = StartingTransferType;

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return(Status);
} // SendDecodeResultHostTransmitAcknowledge





//
//	SendSystemInformationRequest
//		Send a system information request.
//
bool SYN_FE_COMMAND_HANDLER::SendSystemInformationRequest( char *Result, int Protocol, const char *DeviceAddress,
        int SizeOfDeviceAddress, const char *DeviceName, int SizeOfDeviceName, bool WaitForResponse )
{
	bool		Status = true;			// Assume success for now.


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	// Create the system information request.
	HTAG_SYSTEM_INFORMATION	SystemInformation( HTAG_GET_SYSTEM_INFORMATION_DEFAULT_COMMAND_NAME, Device, TransferStatus,
	        CommandHandler );

	SystemInformation.SetSystemInformationRequestParameters( Protocol, DeviceAddress, SizeOfDeviceAddress, DeviceName,
	        SizeOfDeviceName, WaitForResponse );

	int		StartingTransferType = TransferStatus->TransferType;
	TransferStatus->TransferType = HTRANSFER_STATUS::SENDING_SYSTEM_INFORMATION_REQUEST;
	TransferStatus->PrepareDownloadStatus();

	// Write the result.
	Status = ( SystemInformation.WriteTaggedImage( Result ) == HTAG::HTAG_OK) ? true : false;

	TransferStatus->CloseDownloadStatus();
	TransferStatus->TransferType = StartingTransferType;

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return(Status);
} // SendSystemInformationRequest





//
//	SendSystemInformation
//		Send system information.
//
bool SYN_FE_COMMAND_HANDLER::SendSystemInformation( char *Result, int Protocol, const char *DeviceAddress,
        int SizeOfDeviceAddress, const char *DeviceName, int SizeOfDeviceName, bool WaitForResponse, int WorkgroupSelection )
{
	bool		Status = true;			// Assume success for now.


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	// Create the system information request.
	HTAG_SYSTEM_INFORMATION	SystemInformation( HTAG_SYSTEM_INFORMATION_DEFAULT_COMMAND_NAME, Device, TransferStatus,
	        CommandHandler );

	SystemInformation.SetSystemInformationParameters( Protocol, DeviceAddress, SizeOfDeviceAddress, DeviceName,
	        SizeOfDeviceName, WaitForResponse, WorkgroupSelection );

	int		StartingTransferType = TransferStatus->TransferType;
	TransferStatus->TransferType = HTRANSFER_STATUS::SENDING_SYSTEM_INFORMATION;
	TransferStatus->PrepareDownloadStatus();

	// Write the result.
	Status = ( SystemInformation.WriteTaggedImage( Result ) == HTAG::HTAG_OK) ? true : false;

	TransferStatus->CloseDownloadStatus();
	TransferStatus->TransferType = StartingTransferType;

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return(Status);
} // SendSystemInformation





//
//	SendMenuCommand
//		Send menu command.
//
bool SYN_FE_COMMAND_HANDLER::SendMenuCommand( char *Result, int Protocol, const char *DeviceName, int SizeOfDeviceName,
        const char *MenuCommand, int SizeOfMenuCommand, bool CommandResponseRequired, bool WaitForResponse,
        bool IndicatorRequired /*= false*/, const CIoMan *  pOrigInterface /*= g_pStdInterface*/ )
{
	bool		Status = true;			// Assume success for now.


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	// Create the menu command.
	HTAG_MENU_COMMAND	HTAGMenuCommand( HTAG_MENU_COMMAND_SEND_MENU_COMMAND, Device, TransferStatus,
	                                   CommandHandler );

	HTAGMenuCommand.SetMenuCommandParameters( Protocol, DeviceName, SizeOfDeviceName, MenuCommand, SizeOfMenuCommand,
	        CommandResponseRequired, WaitForResponse, IndicatorRequired, pOrigInterface );

	int		StartingTransferType = TransferStatus->TransferType;
	TransferStatus->TransferType = HTRANSFER_STATUS::SENDING_MENU_COMMAND;
	TransferStatus->PrepareDownloadStatus();

	// Write the result.
	Status = ( HTAGMenuCommand.WriteTaggedImage( Result ) == HTAG::HTAG_OK) ? true : false;

	TransferStatus->CloseDownloadStatus();
	TransferStatus->TransferType = StartingTransferType;

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return(Status);
} // SendMenuCommand





//
//	SendMenuCommandResponse
//		Send menu command response.
//
/* Not used.  If we do use this, we need to take into account ReM.
bool SYN_FE_COMMAND_HANDLER::SendMenuCommandResponse( char *Result, int Protocol, const char *DeviceName, int SizeOfDeviceName,
        const char *MenuCommandResponse, int SizeOfMenuCommandResponse, bool WaitForResponse )
{
	bool		Status = true;			// Assume success for now.


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	// Create the menu command response.
	HTAG_MENU_COMMAND	HTAGMenuCommandResponse( HTAG_MENU_COMMAND_MENU_COMMAND_RESPONSE, Device, TransferStatus,
	        CommandHandler );

	HTAGMenuCommandResponse.SetMenuCommandResponseParameters( Protocol, DeviceName, SizeOfDeviceName, MenuCommandResponse,
	        SizeOfMenuCommandResponse, WaitForResponse );

	int		StartingTransferType = TransferStatus->TransferType;
	TransferStatus->TransferType = HTRANSFER_STATUS::SENDING_MENU_COMMAND_RESPONSE;
	TransferStatus->PrepareDownloadStatus();

	// Write the result.
	Status = ( HTAGMenuCommandResponse.WriteTaggedImage( Result ) == HTAG::HTAG_OK) ? true : false;

	TransferStatus->CloseDownloadStatus();
	TransferStatus->TransferType = StartingTransferType;

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return(Status);
} // SendMenuCommandResponse
*/


//
//	SendConfigMenuCommand
//		Send Config menu command.
//
bool SYN_FE_COMMAND_HANDLER::SendConfigMenuCommand( char *Result, int Protocol, const char *DeviceName, int SizeOfDeviceName,
        const char *MenuCommand, int SizeOfMenuCommand, bool CommandResponseRequired, bool WaitForResponse,
        bool IndicatorRequired /*= false*/, const CIoMan *  pOrigInterface /*= g_pStdInterface*/  )
{
	bool		Status = true;			// Assume success for now.
	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	// Create the menu command.
	HTAG_MENU_COMMAND	HTAGMenuCommand( HTAG_MENU_COMMAND_SEND_CONFIG_MENU_COMMAND, Device, TransferStatus,
	                                   CommandHandler );

	HTAGMenuCommand.SetMenuCommandParameters( Protocol, DeviceName, SizeOfDeviceName, MenuCommand, SizeOfMenuCommand,
	        CommandResponseRequired, WaitForResponse, IndicatorRequired, pOrigInterface );

	int		StartingTransferType = TransferStatus->TransferType;
	TransferStatus->TransferType = HTRANSFER_STATUS::SENDING_MENU_COMMAND;
	TransferStatus->PrepareDownloadStatus();

	// Write the result.
	Status = ( HTAGMenuCommand.WriteTaggedImage( Result ) == HTAG::HTAG_OK) ? true : false;

	TransferStatus->CloseDownloadStatus();
	TransferStatus->TransferType = StartingTransferType;

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return(Status);
} // SendConfigMenuCommand



//
//	SendConfigMenuCommandResponse
//		Send config menu command response.
//
/* Not used.  If we do use this, we need to take into account ReM.
bool SYN_FE_COMMAND_HANDLER::SendConfigMenuCommandResponse( char *Result, int Protocol, const char *DeviceName, int SizeOfDeviceName,
        const char *MenuCommandResponse, int SizeOfMenuCommandResponse, bool WaitForResponse )
{
	bool		Status = true;			// Assume success for now.


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	// Create the menu command response.
	HTAG_MENU_COMMAND	HTAGMenuCommandResponse( HTAG_MENU_COMMAND_CONFIG_MENU_COMMAND_RESPONSE, Device, TransferStatus,
	        CommandHandler );

	HTAGMenuCommandResponse.SetMenuCommandResponseParameters( Protocol, DeviceName, SizeOfDeviceName, MenuCommandResponse,
	        SizeOfMenuCommandResponse, WaitForResponse );

	int		StartingTransferType = TransferStatus->TransferType;
	TransferStatus->TransferType = HTRANSFER_STATUS::SENDING_MENU_COMMAND_RESPONSE;
	TransferStatus->PrepareDownloadStatus();

	// Write the result.
	Status = ( HTAGMenuCommandResponse.WriteTaggedImage( Result ) == HTAG::HTAG_OK) ? true : false;

	TransferStatus->CloseDownloadStatus();
	TransferStatus->TransferType = StartingTransferType;

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return(Status);
} // SendConfigMenuCommandResponse
*/





//
//	SendNonMenuCommand
//		Send non-menu command.
//
bool SYN_FE_COMMAND_HANDLER::SendNonMenuCommand( char *Result, int Protocol, const char *DeviceName, int SizeOfDeviceName,
        const char *NonMenuCommand, int SizeOfNonMenuCommand, bool CommandResponseRequired, bool WaitForResponse )
{
	bool		Status = true;			// Assume success for now.


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	// Create the non-menu command.
	HTAG_MENU_COMMAND	HTAGNonMenuCommand( HTAG_MENU_COMMAND_SEND_NON_MENU_COMMAND, Device, TransferStatus,
	                                      CommandHandler );

	HTAGNonMenuCommand.SetNonMenuCommandParameters( Protocol, DeviceName, SizeOfDeviceName, NonMenuCommand,
	        SizeOfNonMenuCommand, CommandResponseRequired, WaitForResponse );

	int		StartingTransferType = TransferStatus->TransferType;
	TransferStatus->TransferType = HTRANSFER_STATUS::SENDING_NON_MENU_COMMAND;
	TransferStatus->PrepareDownloadStatus();

	// Write the result.
	Status = ( HTAGNonMenuCommand.WriteTaggedImage( Result ) == HTAG::HTAG_OK) ? true : false;

	TransferStatus->CloseDownloadStatus();
	TransferStatus->TransferType = StartingTransferType;

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return(Status);
} // SendNonMenuCommand





//
//	SendNonMenuCommandResponse
//		Send non-menu command response.
//
bool SYN_FE_COMMAND_HANDLER::SendNonMenuCommandResponse( char *Result, int Protocol, const char *DeviceName,
        int SizeOfDeviceName, const char *NonMenuCommandResponse, int SizeOfNonMenuCommandResponse, bool WaitForResponse )
{
	bool		Status = true;			// Assume success for now.


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	// Create the non-menu command response.
	HTAG_MENU_COMMAND	HTAGNonMenuCommandResponse( HTAG_MENU_COMMAND_NON_MENU_COMMAND_RESPONSE, Device, TransferStatus,
	        CommandHandler );

	HTAGNonMenuCommandResponse.SetNonMenuCommandResponseParameters( Protocol, DeviceName, SizeOfDeviceName, NonMenuCommandResponse,
	        SizeOfNonMenuCommandResponse, WaitForResponse );

	int		StartingTransferType = TransferStatus->TransferType;
	TransferStatus->TransferType = HTRANSFER_STATUS::SENDING_NON_MENU_COMMAND_RESPONSE;
	TransferStatus->PrepareDownloadStatus();

	// Write the result.
	Status = ( HTAGNonMenuCommandResponse.WriteTaggedImage( Result ) == HTAG::HTAG_OK) ? true : false;

	TransferStatus->CloseDownloadStatus();
	TransferStatus->TransferType = StartingTransferType;

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return(Status);
} // SendNonMenuCommandResponse



