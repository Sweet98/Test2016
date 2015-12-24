//======================================================================================
// db_SYNFECommandHandler.cpp

//		In order to use SYN_FE_COMMAND_HANDLER, several members of the SYN_FE_COMMAND_HANDLER class must be overridden.  The
//		MATRIX_SYN_FE_COMMAND_HANDLER class accomplishes this.
//======================================================================================
// $RCSfile: MatrixSynFECommandHandler.cpp $
// $Revision: 1.58 $
// $Date: 2011/11/10 20:29:52EST $
//======================================================================================

//======================================================================================
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "MatrixSynFECommandHandler.h"			// Interface definition
#include "crc.h"
#include "db_hdw.h"
#include "db_chrio.h"
#include "thread.h"
#include "beep.h"
#include "dataedit.h"
//#include "eci.h"
#include "barcodedataman.h"
#include "IoMan.h"
#include "db_nonmenucommands.h"				// Definitions for code writing routines.
//#include "FlashProductConfiguration.h"
#include "HexFile.h"
#include "HWString.h"
//#include "main_support_functions.h"
//#include "MD5.h"
#include "readerconfiguration.h"
#include "menu.h"
#include "MenuFlash.h"
#include "ProductConfiguration.h"
#include "NonMenuCommands.h"
//#include "Interface_Support_Functions.h"

//#include "trigger.h"                                            //lmc    not needed   fixme
//#include "trigger_5_5.h"                                         //lmc   fixme
#if PLATFORM_CURRENT == imx25
#include "MenuFlash.h"
#include "MocfHandler.h"
#include "md5.h"
#include "FileHandler.h"
#endif // endof #if PLATFORM_CURRENT == imx25

#ifndef RFBASE
#include "decodeiface.h"
#include "HImage.h"
#include "HardwareToken.h"
#include "ScanEnginePSOC.h"
#include "startupFlashdisk.h"
#else
#endif



//#include	"FlashScript.h"						// Routines that store and retrieve scripts.

#ifdef PLUGIN_DEV
#include "Uchar.h"
#include "PluginMenuProcess.h"
#endif


MATRIX_SYN_FE_COMMAND_HANDLER *SYNFECommandHandler=NULL;

//---------------------------------------------------------------------------



//extern	CTrigger *pTrigger;		// Need this in order to indicate to decoder interface that we've been triggered serially.
extern bool ShowFreeHeapSpace(void);
extern void SendStringToInterface( const char *pucBuffer);
extern bool ProcessingScannedMenu;
extern void LoadWorkingConfigurationOne();
extern void LoadWorkingConfigurationZero();
extern  int AutoCable_StrIndex(unsigned char *str,unsigned char *substr);
bool ProcessingMenuFromIterator=false;
extern int PSOCBlankdownload;





//
//	Constructor
//		Nothing to do.
//
MATRIX_SYN_FE_COMMAND_HANDLER::MATRIX_SYN_FE_COMMAND_HANDLER(HCOMM *HCommDevice, HTRANSFER_STATUS *InputTransferStatus)
	    : SYN_FE_COMMAND_HANDLER(HCommDevice, InputTransferStatus, this, this, NULL, NULL, NULL, NULL, NULL, NULL),
			HTAG_POST_TRANSFER_PROCESSOR(),
			HTAG_IMAGE_POST_TRANSFER_PROCESSOR(),
			HTAG_SCRIPT_POST_TRANSFER_PROCESSOR(),
			HTAG_SCRIPT_HOST_COMMAND_POST_TRANSFER_PROCESSOR(),
			HTAG_SCRIPT_HOST_DATA_POST_TRANSFER_PROCESSOR(),
			HCOMMAND_HANDLER(),
			HTAG_FILE_POST_TRANSFER_PROCESSOR(),
			HTAG_CODE_POST_TRANSFER_PROCESSOR(),
			READER_DEVICE_EVENT_PROCESSOR()

{
	SYN_FE_COMMAND_HANDLER::ImagePostProcessor = this;
	SYN_FE_COMMAND_HANDLER::ScriptPostProcessor = this;
	SYN_FE_COMMAND_HANDLER::HostCommandProcessor = this;
	SYN_FE_COMMAND_HANDLER::HostDataProcessor = this;
	SYN_FE_COMMAND_HANDLER::FilePostProcessor = this;
	SYN_FE_COMMAND_HANDLER::CodePostProcessor = this;
	SYN_FE_COMMAND_HANDLER::EventProcessor = this;

	return;
} // MATRIX_SYN_FE_COMMAND_HANDLER constructor



//
//	Destructor
//		Nothing to do.
//
MATRIX_SYN_FE_COMMAND_HANDLER::~MATRIX_SYN_FE_COMMAND_HANDLER(void)
{
	return;
} // MATRIX_SYN_FE_COMMAND_HANDLER destructor






//
//	ProcessHTAGData
//		HTAG_POST_TRANSFER_PROCESSOR I/O implementation that currently handles Verifier data, but will eventually handle all
//
void MATRIX_SYN_FE_COMMAND_HANDLER::ProcessHTAGData( int *Status, char * /* Result */, int DataType, void * /* NewHTAGData */ )
{
	bool	DataReceivedSuccessfully =
	    (( *Status == HTAG::HTAG_OK) || ( *Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || ( *Status < 0));

	if( DataReceivedSuccessfully )
	{
		switch( DataType )
		{

		default:
			break;
		};
	}

	return;
} // ProcessHTAGData




#ifndef RFBASE

//
//	ProcessImage
//		SYN_FE_COMMAND_HANDLER override to decode an image.
//
void MATRIX_SYN_FE_COMMAND_HANDLER::ProcessImage(int *Status, char *Result, HTAG_IMAGE * NewImage)
{
	int		DecodeStartTime=0, DecodeEndTime=0;
	int		DecoderStatus = 1;
	int		WithVigor = *Status & 0x200;

	UnformatHTagImage(NewImage);

	GrayscaleImage2 ImageDescriptor;

	ImageDescriptor.imageType = 4;
	ImageDescriptor.WhiteMax		= 255;
	ImageDescriptor.WhiteClip		= 255;
	ImageDescriptor.verticalCenter = NewImage->YDimension/2;
	ImageDescriptor.horizontalCenter = NewImage->XDimension/2;
	ImageDescriptor.mirrorImage	= 0;
	ImageDescriptor.imageRotation	= 0;
	ImageDescriptor.array = NewImage->Data->Char;
	ImageDescriptor.width = NewImage->XDimension;
	ImageDescriptor.height = NewImage->YDimension;
	ImageDescriptor.arrayWidth = NewImage->XDimension;
	ImageDescriptor.whiteHigh = TRUE;
	ImageDescriptor.color = FALSE;
	ImageDescriptor.imageNumber=0;

	if (*Status & 0x400)
		DecodeDownloadedImage(&ImageDescriptor);
	else if (*Status & 0x2000)
		DecodeAndSaveImage(&ImageDescriptor);
	else if (*Status & 0x4000)
		SaveThisImage(&ImageDescriptor);
	else if (*Status & 0x8000)
		SaveDownloadedImage(&ImageDescriptor);
	else
		decode_2d_from_memory(&ImageDescriptor, WithVigor);

	*Status &= 0xfffff9ff;
	if (GET_SETTING(DebugTimeToReadReporting))
	{
		char	temp[ 80 ];
		sprintf ( temp, "\r\nCached decode in %dms    status = %d\r\n", (DecodeEndTime - DecodeStartTime)/1000, DecoderStatus);
		SendStringToInterface(temp);
	}


	if (GET_SETTING(DebugTimeToReadReporting))
	{
		char	temp[ 80 ];
		sprintf ( temp, "\r\nTransfer status = %d, message = %s\r\n", *Status, Result);
		SendStringToInterface(temp );
	}

	return;
} // ProcessImage
#else
#endif




//
//	StoreHHPLMainScript
//		SYN_FE_COMMAND_HANDLER override to store a main script.
//
void MATRIX_SYN_FE_COMMAND_HANDLER::StoreHHPLMainScript(int *Status, char *Result, HTAG_SCRIPT *NewScript)
{
#ifdef FIXME
	if(StoreMainScriptToFlash((char *) NewScript->ScriptName->Char, NewScript->Data))
	{
		// If this is supposed to be the default script, change the name.
		if(NewScript->DefaultScript)
		{
			HSTRING		Command("0C0101", true);
			Command.Append(NewScript->ScriptName);
			Command.Append(".");
			ReaderConfiguration->Menu(&Command);
			FlashWorkingChanges();
			reset_to_boot_code_hard();
		}
	}

	else
	{
		strcpy(Result, "Flash operation failed");
		*Status = SYN_FE_COMMAND_HANDLER_STORE_OPERATION_FAILED;
	}

	return;
#endif
} // StoreHHPLMainScript





//
//	RetrieveHHPLMainScript
//		SYN_FE_COMMAND_HANDLER override to retrieve a main script.
//
void MATRIX_SYN_FE_COMMAND_HANDLER::RetrieveHHPLMainScript(int *Status, char *Result, HTAG_SCRIPT* Script)
{
#ifdef FIXME
	if(!ReadMainScriptFromFlash((char *) Script->ScriptName->Char, Script->Data))
	{
		strcpy(Result, "Retrieve operation failed");
		*Status = SYN_FE_COMMAND_HANDLER_FILE_NOT_FOUND;
	}

	return;
#endif
} // RetrieveHHPLMainScript






//
//	LoadAndRunHHPLMainScript
//		SYN_FE_COMMAND_HANDLER override to load and run a main script.
//
void MATRIX_SYN_FE_COMMAND_HANDLER::LoadAndRunHHPLMainScript(int *Status, char *Result, HTAG_SCRIPT *NewScript)
{
	//	if()
	//	{
	//	}
	NewScript->TransferProtocolParameterNumber = 0;		// Stop compiler warnings by doing this dummy set!
	//	else
	//	{
	strcpy(Result, "Load and run operation failed");
	*Status = SYN_FE_COMMAND_HANDLER_LOAD_AND_RUN_OPERATION_FAILED;
	//	}

	return;
} // LoadAndRunHHPLMainScript





//
//	StoreHHPLEditScript
//		SYN_FE_COMMAND_HANDLER override to store an edit script.
//
void MATRIX_SYN_FE_COMMAND_HANDLER::StoreHHPLEditScript(int *Status, char *Result, HTAG_SCRIPT *NewScript)
{
#ifdef FIXME
	if(StoreEditScriptToFlash((char *) NewScript->ScriptName->Char, NewScript->Data))
	{
		// If this is supposed to be the default script, change the name.
		if(NewScript->DefaultScript)
		{
			HSTRING		Command("0C0102", true);
			Command.Append(NewScript->ScriptName);
			Command.Append(".");
			ReaderConfiguration->Menu(&Command);
		}
	}

	else
	{
		strcpy(Result, "Flash operation failed");
		*Status = SYN_FE_COMMAND_HANDLER_STORE_OPERATION_FAILED;
	}

	return;
#endif
} // StoreHHPLEditScript





//
//	RetrieveHHPLEditScript
//		SYN_FE_COMMAND_HANDLER override to retrieve an edit script.
//
void MATRIX_SYN_FE_COMMAND_HANDLER::RetrieveHHPLEditScript(int *Status, char *Result, HTAG_SCRIPT* Script)
{
#ifdef FIXME
	if(!ReadEditScriptFromFlash((char *) Script->ScriptName->Char, Script->Data))
	{
		strcpy(Result, "Retrieve operation failed");
		*Status = SYN_FE_COMMAND_HANDLER_FILE_NOT_FOUND;
	}

	return;
#endif
} // RetrieveHHPLEditScript





//
//	LoadAndRunHHPLEditScript
//		SYN_FE_COMMAND_HANDLER override to load and run an edit script.
//
void MATRIX_SYN_FE_COMMAND_HANDLER::LoadAndRunHHPLEditScript(int *Status, char *Result, HTAG_SCRIPT *NewScript)
{
	//	if()
	//	{
	//	}

	NewScript->TransferProtocolParameterNumber = 0;		// Stop compiler warnings by doing this dummy set!
	//	else
	//	{
	strcpy(Result, "Load and run operation failed");
	*Status = SYN_FE_COMMAND_HANDLER_LOAD_AND_RUN_OPERATION_FAILED;
	//	}

	return;
} // LoadAndRunHHPLEditScript





//
//	HHPLSendHostCommand
//		SYN_FE_COMMAND_HANDLER override to process a host command in the main script.
//
void MATRIX_SYN_FE_COMMAND_HANDLER::HHPLSendHostCommand(int *Status, char *Result, HTAG_SCRIPT_HOST_COMMAND *NewScriptHostCommand)
{
	//	if()
	//	{
	//	}

	NewScriptHostCommand->TransferProtocolParameterNumber = 0;		// Stop compiler warnings by doing this dummy set!
	//	else
	//	{
	strcpy(Result, "Host command operation failed");
	*Status = SYN_FE_COMMAND_HANDLER_HOST_COMMAND_OPERATION_FAILED;
	//	}

	return;
} // HHPLSendHostCommand





//
//	HHPLSendHostData
//		SYN_FE_COMMAND_HANDLER override to process host data in the main script.
//
void MATRIX_SYN_FE_COMMAND_HANDLER::HHPLSendHostData(int *Status, char *Result, HTAG_SCRIPT_HOST_DATA *NewScriptHostData)
{
	//	if()
	//	{
	//	}

	NewScriptHostData->TransferProtocolParameterNumber = 0;		// Stop compiler warnings by doing this dummy set!
	//	else
	//	{
	strcpy(Result, "Host data operation failed");
	*Status = SYN_FE_COMMAND_HANDLER_HOST_DATA_OPERATION_FAILED;
	//	}

	return;
} // HHPLSendHostData




#ifdef PLUGIN_DONWLOAD_OSPV
#define MAX_FILENAME_LEN	57
bool StorePlugin(const HTAG_FILE *NewFile)
{
	F_FILE *PluginFile;
	F_FIND find;
	char filename[MAX_FILENAME_LEN];

	char *pName = (char *)NewFile->FileName->Char;

	strncpy(filename, "/PluginFolder/", 15);
	strncat(filename, (const char*)pName, strlen(pName) > (MAX_FILENAME_LEN - 15) ? (MAX_FILENAME_LEN - 15) : strlen(pName));
	filename[MAX_FILENAME_LEN - 1] = 0; // make sure it's null terminated
	PLUGIN_TRACE(("File Saving...\r\n"));
	PLUGIN_TRACE((filename));
	PLUGIN_TRACE(("\r\n"));

	if (!f_findfirst("/PluginFolder",&find)){
		if (!find.attr&F_ATTR_DIR){
			f_mkdir("/PluginFolder");
		}
	} else{
		f_mkdir("/PluginFolder");
	}

	PluginFile = f_open(filename, "w");
	if(!PluginFile){
		PLUGIN_TRACE(("Cannot open file plugin\r\n"));
		return false;
	}

	if(f_write(NewFile->Data->Char, 1, NewFile->Data->Size, PluginFile) != NewFile->Data->Size){
		PLUGIN_TRACE(("Write plugin file fail\r\n"));
		return false;
	}

	PLUGIN_TRACE(("File save complete\r\n"));
	f_close(PluginFile);
	return true;
}
#endif // endof #ifdef PLUGIN_DONWLOAD_OSPV

static const char *ProkeyFileName  = "Prokey";

//
//	StoreFile
//		SYN_FE_COMMAND_HANDLER override to store a flash disk file.
//
#if PLATFORM_CURRENT == imx25
void MATRIX_SYN_FE_COMMAND_HANDLER::StoreFile( int *Status, char *Result, HTAG_FILE *NewFile )
{
	bool bIsOk;

	if ( NewFile->FileType == HTAG_FILE::HTAG_FILE_TYPE_MOCF_FILE )
	{
		bIsOk = CFileHandler::ProcessMocf(NewFile->Data->Char, NewFile->Data->Size, false, true, NewFile->ResponseRequired);
	}
	else if ( NewFile->FileType == HTAG_FILE::HTAG_FILE_TYPE_PROKEY_FILE )
	{
		bIsOk = CFileHandler::StoreFile(ProkeyFileName, NewFile->Data->Char, NewFile->Data->Size);
	}
#ifdef PLUGIN_DONWLOAD_OSPV
	else bIsOk = StorePlugin(NewFile);
#else
	else  bIsOk = false;  // Right now, I'm not sure we want to store any file without some checks (we did this in Gen5)
#endif // endof #ifdef PLUGIN_DONWLOAD_OSPV

	if ( ! bIsOk )
	{
		strcpy(Result, "File store operation failed");
		*Status = SYN_FE_COMMAND_HANDLER_STORE_OPERATION_FAILED;
	}
} // StoreFile

#else // Gen5
#endif // endof #if PLATFORM_CURRENT == imx25



void MATRIX_SYN_FE_COMMAND_HANDLER::PrepareForFileDownload(void)
{
	ForceScanDriverOff();
}


//
//	RetrieveFile
//		SYN_FE_COMMAND_HANDLER override to retrieve a flash disk file.
//
void MATRIX_SYN_FE_COMMAND_HANDLER::RetrieveFile( int *Status, char *Result, HTAG_FILE *File )
{
#ifdef FIXME
	int ReadStatus = FlashDisk->Read( (char *) File->FileName->Char, File->Data );
	if( ReadStatus < 0 )
	{
		strcpy(Result, "File retrieve operation failed");
		*Status = SYN_FE_COMMAND_HANDLER_FILE_NOT_FOUND;
	}

	return;
#endif
} // RetrieveFile





//
//	StoreApplicationCode
//		SYN_FE_COMMAND_HANDLER override to store a application code.
//
void MATRIX_SYN_FE_COMMAND_HANDLER::StoreApplicationCode(int *Status, char *Result, HTAG_READER_CODE *NewApplicationCode)
{
	if(( NewApplicationCode->CRC == -1 ) || ( NewApplicationCode->CRC == calc_crc( NewApplicationCode->Data->Char, NewApplicationCode->Data->Size )))
	{
		if( !ProcessNewApplicationCode( NewApplicationCode->Data->Char, NewApplicationCode->Data->Size,
		                                ( NewApplicationCode->OverrideApplicationTypeCheck > 0 ) ? true : false,
		                                ( NewApplicationCode->OverrideDeviceTypeCheck > 0 ) ? true : false,
		                                ( NewApplicationCode->StoreFile > 0 ) ? true : false, NewApplicationCode->ResponseRequired ? true : false ) )
		{
			strcpy(Result, "Application code write operation failed");
			*Status = SYN_FE_COMMAND_HANDLER_STORE_OPERATION_FAILED;
		}
	}
	else
	{
		strcpy(Result, "Application code CRC mismatch");
		*Status = SYN_FE_COMMAND_HANDLER_CORRUPT_IMAGE;
	}

	return;
} // StoreApplicationCode





//
//	StoreBootCode
//		SYN_FE_COMMAND_HANDLER override to store a boot code.
//
void MATRIX_SYN_FE_COMMAND_HANDLER::StoreBootCode(int *Status, char *Result, HTAG_READER_CODE *NewBootCode)
{
	if(( NewBootCode->CRC == -1 ) || ( NewBootCode->CRC == calc_crc( NewBootCode->Data->Char, NewBootCode->Data->Size )))
	{
		if( ! ProcessNewBootCode( NewBootCode->Data->Char, NewBootCode->Data->Size, ( NewBootCode->OverrideDeviceTypeCheck > 0 ) ? true : false ))
		{
			strcpy(Result, "Boot code write operation failed");
			*Status = SYN_FE_COMMAND_HANDLER_STORE_OPERATION_FAILED;
		}
	}
	else
	{
		strcpy(Result, "Boot code CRC mismatch");
		*Status = SYN_FE_COMMAND_HANDLER_CORRUPT_IMAGE;
	}

	return;
} // StoreBootCode




/*----------------------------------------------------------------------------------------
AddSetting

  Helper function fro CollectProdConfigSettings().  It queries Product Config for a setting
  and if it's there, it appends it to the HSTRING.

  Parameters:
  				pMenuStr[in]:  setting to check for (and append)

  				pSettings[in]:  HSTRING to append setting to


  Returns:  nothing.
----------------------------------------------------------------------------------------*/
static void AddSetting(const char * pMenuStr, HSTRING * pSettings)
{
	HSTRING setting(pMenuStr, true);
	ProductConfiguration->Menu(&setting);
	if ( setting.FindChar(0x06) )
	{	// This setting exists (cordless settings won't exist on a corded device & vice versa).  Replace ACK. with a ';'
		setting.ReplaceAll("\x06.", ";");
		pSettings->Append(&setting);
	}
}


/*----------------------------------------------------------------------------------------
CollectProdConfigSettings

  We need to persist Product Config settings.  To avoid a bunch of #ifdefs, this function
  queries for all settings in the MFG Product Configuration Commands document (release
  number 500025145).  Thus, some are expected to fail (for example, bluetooth related
  settings on a corded).

  Parameters:
  				pSettings[in]:  HSTRING to append setting to


  Returns:  nothing.
----------------------------------------------------------------------------------------*/
static void CollectProdConfigSettings(HSTRING * pSettings)
{
	AddSetting("0F2600?.", pSettings);
	AddSetting("0F2700?.", pSettings);
	AddSetting("0F2800?.", pSettings);
    AddSetting("0F2801?.", pSettings);
	AddSetting("0F2701?.", pSettings);
	AddSetting("0F2802?.", pSettings);
	AddSetting("0F2803?.", pSettings);
	AddSetting("0F1A04?.", pSettings);
	AddSetting("0F1A01?.", pSettings);
	AddSetting("0F1A00?.", pSettings);
	AddSetting("0F2900?.", pSettings);
	AddSetting("0F2B00?.", pSettings);
	AddSetting("0F2A00?.", pSettings);
	AddSetting("0F0802?.", pSettings);
	AddSetting("0F0801?.", pSettings);
	AddSetting("0F0800?.", pSettings);
    AddSetting("0F1901?.", pSettings);
}

//
//	StoreFlashImage
//		Store the image to flash that was sent by the caller.
//
#if PLATFORM_CURRENT == imx25
//  Gen6:  we use an MOCF System Update File.
// TODO:  we ignore the Response Required parameter (RSPREQ) and always put out a response.  This is a carryover from Gen5.  Test Engineering
// does not send RSPREQ.  When we do the next "gen 7", have them add it to the string they send us so we can be more compliant to HTAG. 
void MATRIX_SYN_FE_COMMAND_HANDLER::StoreFlashImage(int *pStatus, char *pResult, HTAG_READER_CODE *pNewSystemUpdate)
{
	if (( pNewSystemUpdate->CRC == -1 )
		|| ( pNewSystemUpdate->CRC == calc_crc( pNewSystemUpdate->Data->Char, pNewSystemUpdate->Data->Size )) )
	{


		// If we're supposed to save settings, do it now.  Note:  we are making use of the legacy pNewSystemUpdate->SaveSerialNumbers
		// which originally was intended to save just serial numbers.  Gen6 extends this to other settings as well.
		HSTRING SavedSettings(200, true);
		if( pNewSystemUpdate->SaveSerialNumbers ) CollectProdConfigSettings(&SavedSettings);
		if ( ! CFileHandler::ProcessSystemUpdateMocf(pNewSystemUpdate->Data->Char) )
		{
			strcpy(pResult, "System Update store operation failed");
			*pStatus = SYN_FE_COMMAND_HANDLER_STORE_OPERATION_FAILED;
			xputchar( NAK );
			wait_til_tx_is_empty();
		}
		else
		{	// MOCF was stored OK--send out the positive acknowledgment
			// Compute the MD5 checksum of the mocf and then store the result to product configuration setting FLSMD5.
			HSTRING PrintableMD5Checksum( 40, true );
			md5_state_t MD5State;

			md5_init( &MD5State );
			md5_append( &MD5State, (const md5_byte_t *) pNewSystemUpdate->Data->Char, pNewSystemUpdate->Data->Size );
			md5_finish( &MD5State, PrintableMD5Checksum.Char );
			PrintableMD5Checksum.Size = 16;
			PrintableMD5Checksum.BinaryToHex();

			// If we're supposed to save serial numbers, restore them now.
			if( pNewSystemUpdate->SaveSerialNumbers )
			{	// FYI:  Test Engineering burns various Product Config settings first and loads this image file last
				// Get the new product configuration from flash.
				HSTRING NewProductConfiguration( 20, true );
				if( ReadProductConfigurationFromFlash( &NewProductConfiguration ))
				{
					// If we found a new product configuration, remove trailing '.' if there is one.
					if (( NewProductConfiguration.Size > 0 )  && ( NewProductConfiguration.Char[ NewProductConfiguration.Size - 1 ] == '.' ))
						NewProductConfiguration.Remove( NewProductConfiguration.Size - 1, 1 );
				}

				// Store the MD5 checksum to product configuration setting FLSMD5.  Note:  it is unknown why FLSMD5 is only done if SaveSerialNumbers
				// is true.  But, since it's always true, it doesn't adversely affect things.
				NewProductConfiguration.Append( ";0F0A00" );
				NewProductConfiguration.Append( &PrintableMD5Checksum );
				NewProductConfiguration.Append( ";" );
				NewProductConfiguration.Append( &SavedSettings );
				NewProductConfiguration.Append(".");
				OverWriteProductConfiguration((const char *)NewProductConfiguration.Char, NewProductConfiguration.Size);
			}
			xputchar( ACK );

			xputstring( "MD5=" );
			xputstring( PrintableMD5Checksum.Char, PrintableMD5Checksum.Size );
			xputstring( "\r\n" );

			wait_til_tx_is_empty();

			// If we're supposed to wait, do indicators.
			if( pNewSystemUpdate->StopAfterDownload )
			{
				FlashImageDownloadCompleteBeep();
				while ( 1 ) RESET_WATCHDOG_TIMER();
			}
		}
	}
	else
	{
		strcpy(pResult, "Flash image CRC mismatch");
		*pStatus = SYN_FE_COMMAND_HANDLER_CORRUPT_IMAGE;
		xputchar( NAK );
		wait_til_tx_is_empty();
	}

	// Reboot if we're supposed to.
	if( pNewSystemUpdate->Reboot )
	{
		reset_to_boot_code_hard();
	}
	return;
}
#else // else it's Gen5
#endif // endof #if PLATFORM_CURRENT == imx25



//
//	RetrieveFlashImage
//		Retrieve an image from flash and return to the caller.
//
void MATRIX_SYN_FE_COMMAND_HANDLER::RetrieveFlashImage(int *Status, char *Result, HTAG_READER_CODE *FlashImage )
{
	if( FlashImage->StartAddress == 0xFFFFFFFF )
	{
		FlashImage->StartAddress = GET_SETTING( DefaultFlashImageStartAddress );
	}
	if( FlashImage->SizeOfImage == 0xFFFFFFFF )
	{
		FlashImage->SizeOfImage = FLASH_SIZE - FlashImage->StartAddress;
	}

	unsigned int	StartAddress = (unsigned int) FlashImage->StartAddress;
	unsigned int	EndAddress = StartAddress + (unsigned int) FlashImage->SizeOfImage - 1;

	if(( StartAddress < (unsigned int) ROM_SIZE ) && ( EndAddress < (unsigned int) ROM_SIZE ) && ( StartAddress <= EndAddress ))
	{
		// To save space, create pointers to flash rather than copying flash to RAM.
		delete FlashImage->Data;
		FlashImage->Data = new HSTRING( (unsigned char *) &FLASH_READ_BYTE_POINTER[ StartAddress ], FlashImage->SizeOfImage, false, true, true );
		FlashImage->HTAG::Data = FlashImage->Data;				// Give the pointer to the base class so it can operate on the data as well.

		FlashImage->CRC = calc_crc( FlashImage->Data->Char, FlashImage->Data->Size );
	}
	else
	{
		strcpy(Result, "Flash image address out of range");
		*Status = SYN_FE_COMMAND_HANDLER_ADDRESS_OUT_OF_RANGE;
	}

	return;
} // RetrieveFlashImage





#ifndef RFBASE
//
//	StoreScanEngineFirmware
//		SYN_FE_COMMAND_HANDLER override to store scan engine firmware.
//
void MATRIX_SYN_FE_COMMAND_HANDLER::StoreScanEngineFirmware( int *Status, char *Result, HTAG_SCAN_ENGINE_FIRMWARE *NewScanEngineFirmware )
{
	if(HardwareToken.IAmOEMJade()||(GET_CONFIG_SETTING(AimerType) == AIMER_PSOC_LASER)||PSOCBlankdownload)
	{
		if(( NewScanEngineFirmware->CRC == -1 ) ||
	        ( NewScanEngineFirmware->CRC == calc_crc( NewScanEngineFirmware->Data->Char, NewScanEngineFirmware->Data->Size )))
		{
			switch( NewScanEngineFirmware->StorageType )
			{
			case HTAG_SCAN_ENGINE_FIRMWARE::HTAG_SCAN_ENGINE_FIRMWARE_STORE_TO_FLASH_FILE_ONLY:
				if( WriteAndCloseFile((char *) NewScanEngineFirmware->FileName->Char, (unsigned char*)NewScanEngineFirmware->Data->Char,NewScanEngineFirmware->Data->Size) <= 0)
				//	FlashDisk->Write( (char *) NewScanEngineFirmware->FileName->Char, (char *) NewScanEngineFirmware->Data->Char,
			            //          NewScanEngineFirmware->Data->Size, MATRIX_FLASH_DISK::FD_SCAN_ENGINE_FIRMWARE, 0, 0, 0) < 0 )
				{
					*Status = SYN_FE_COMMAND_HANDLER_STORE_OPERATION_FAILED;
				}
				break;

			case HTAG_SCAN_ENGINE_FIRMWARE::HTAG_SCAN_ENGINE_FIRMWARE_STORE_TO_SCAN_ENGINE_AND_STORE_TO_FLASH_FILE:
				{
					if( WriteAndCloseFile((char *) NewScanEngineFirmware->FileName->Char, (unsigned char*)NewScanEngineFirmware->Data->Char,NewScanEngineFirmware->Data->Size) <= 0)
				//	if( FlashDisk->Write( (char *) NewScanEngineFirmware->FileName->Char, (char *) NewScanEngineFirmware->Data->Char,
				               //       NewScanEngineFirmware->Data->Size, MATRIX_FLASH_DISK::FD_SCAN_ENGINE_FIRMWARE, 0, 0, 0) < 0 )
					{
						*Status = SYN_FE_COMMAND_HANDLER_STORE_OPERATION_FAILED;
					}

					if( !Psoc5100LoadFirmwareData( NewScanEngineFirmware->Data->Char, NewScanEngineFirmware->Data->Size, true,
				                               NewScanEngineFirmware->IgnoreFirmwareTypeMismatch ? true : false ) )
					{
						*Status = SYN_FE_COMMAND_HANDLER_STORE_OPERATION_FAILED;
					}
				}
				break;

			case HTAG_SCAN_ENGINE_FIRMWARE::HTAG_SCAN_ENGINE_FIRMWARE_STORE_TO_SCAN_ENGINE_ONLY:
			default:
				{
				//	if( WriteAndCloseFile("EngineFirmware", (unsigned char*)NewScanEngineFirmware->Data->Char,NewScanEngineFirmware->Data->Size) <= 0)
					if( !Psoc5100LoadFirmwareData( NewScanEngineFirmware->Data->Char, NewScanEngineFirmware->Data->Size, true,
				          	                    true  ) ) // NewScanEngineFirmware->IgnoreFirmwareTypeMismatch ? : false
					{
						*Status = SYN_FE_COMMAND_HANDLER_STORE_OPERATION_FAILED;
					}
				}
				break;
			};
		}
		else
		{
			strcpy(Result, "Scan engine firmware CRC mismatch");
			*Status = SYN_FE_COMMAND_HANDLER_CORRUPT_IMAGE;
		}

		// Write out status.
		if( NewScanEngineFirmware->ResponseRequired )
		{
			if( *Status == HTAG::HTAG_OK )
			{
				theStdInterface.Write( (unsigned char ) ACK );
			}
			else
			{
				theStdInterface.Write( (unsigned char ) NAK );
			}
		}
	return;
	}
} // StoreScanEngineFirmware
#else
#endif


//
//	ProcessSystemInformationRequest
//		Return system information to the initiator.
//
void MATRIX_SYN_FE_COMMAND_HANDLER::ProcessSystemInformationRequest( int * /* Status */, char * /* Result */, HTAG_SYSTEM_INFORMATION *SystemInformation )
{
	SystemInformation->DeviceAddress->Copy( "" );
	SystemInformation->DeviceName->Copy( "" );

	return;
} // ProcessSystemInformationRequest




//
//	ProcessSystemInformation
//		Process system information received from the initiator.
//
void MATRIX_SYN_FE_COMMAND_HANDLER::ProcessSystemInformation( int * /* Status */, char * /* Result */, HTAG_SYSTEM_INFORMATION * /* SystemInformation */ )
{

	return;
} // ProcessSystemInformation



//
//	ProcessMenuCommand
//		Process menu command received from the initiator.  The response will be stored in Data in case the SYN FE command handler needs to
//		return the response to the initiator.  For RFBASE, the command will not be processed here if the device name or address doesn't match
//		our own, but we'll instead forward it on to the appropriate device.
//
void MATRIX_SYN_FE_COMMAND_HANDLER::ProcessMenuCommand(int * /* Status */, char * /* Result */, HTAG_MENU_COMMAND *MenuCommand)
{
#ifdef RFBASE
#else // elseof #ifdef RFBASE
#ifdef PLUGIN_DEV
	HSTRING PluginMenuCommand("",true);
	HSTRING SystemMenuCommand("",true);
	PluginMenuCommand.Copy(MenuCommand->Data);
	SystemMenuCommand.Copy(MenuCommand->Data);
					
	int PluginCmd = 0, SystemCmd = 0;

	PluginCmd = AbstractPluginMenuCommands(&PluginMenuCommand);
	SystemCmd = AbstractSystemMenuCommands(&SystemMenuCommand);

	if(PluginCmd)
	{
		//process 990 menu barcode for plugin
		PluginSerialMenuCommandProcess_Ext(&PluginMenuCommand);
		MenuCommand->Data->Copy(&PluginMenuCommand);

		if(SystemCmd){
			
			// Process system commands
			ReaderConfiguration->Menu(&SystemMenuCommand);
			// Combine the results together
			MenuCommand->Data->Remove(MenuCommand->Data->Size - 1, 1);
			MenuCommand->Data->AppendChar(';');
			MenuCommand->Data->Append(&SystemMenuCommand);
		}

	}
	else{
#endif // endof #ifdef PLUGIN_DEV


	ReaderConfiguration->Menu( MenuCommand->Data );
#ifdef PLUGIN_DEV
	}
#endif // endof #ifdef PLUGIN_DEV
#endif // endof #else // elseof #ifdef RFBASE

	return;
}



//
//	ProcessMenuCommandResponse
//		Process menu command response received from the initiator.  Only RFBASE will use this, and will send it out to the standard interface.
//
void MATRIX_SYN_FE_COMMAND_HANDLER::ProcessMenuCommandResponse(int * /* Status */, char * /* Result */, HTAG_MENU_COMMAND * /* MenuCommandResponse */ )
{

	return;
}




//
//	ProcessNonMenuCommand
//		Process non-menu command received from the initiator.  The response will be stored in Data in case the SYN FE command handler needs to
//		return the response to the initiator.  For RFBASE, the command will not be processed here if the device name or address doesn't match
//		our own, but we'll instead forward it on to the appropriate device.
//
void MATRIX_SYN_FE_COMMAND_HANDLER::ProcessNonMenuCommand(int * /* Status */, char * /* Result */, HTAG_MENU_COMMAND *NonMenuCommand)
{
#ifdef RFBASE
#else
	HSTRING		NonMenuCommandWithComma( NonMenuCommand->Data, true );
	NonMenuCommandWithComma.Insert( "N,", 0 );							// Insert a comma to make the parser happy.
	ProcessNonMenuCommands( (char *) NonMenuCommandWithComma.Char, NonMenuCommandWithComma.Size );
#endif

	return;
}




//
//	ProcessNonMenuCommandResponse
//		Process non-menu command response received from the initiator.  Only RFBASE will use this, and will send it out to the standard interface.
//
void MATRIX_SYN_FE_COMMAND_HANDLER::ProcessNonMenuCommandResponse(int * /* Status */, char * /* Result */, HTAG_MENU_COMMAND * /* NonMenuCommandResponse */)
{

	return;
}







//
//	ProcessConfigMenuCommand
//		Process menu command received from the initiator.  The response will be stored in Data in case the SYN FE command handler needs to
//		return the response to the initiator.  For RFBASE, the command will not be processed here if the device name or address doesn't match
//		our own, but we'll instead forward it on to the appropriate device.
//
void MATRIX_SYN_FE_COMMAND_HANDLER::ProcessConfigMenuCommand(int * /* Status */, char * /* Result */, HTAG_MENU_COMMAND *MenuCommand)
{			
#ifdef RFBASE
#else	
	//GFIXME
	ProductConfiguration->Menu( MenuCommand->Data );
/*	
    HSTRING SerialNum(SIZE_OF_SER_NUM, true);
    SerialNum.Copy(GET_CONFIG_SETTING(SerNum));
    SerialNum.UniCodeToSimpleAsciiFormat();

	DEBUG_puts((char*)SerialNum.Char);
*/	
#endif

	return;
}


//
//	ProcessConfigMenuCommandResponse
//		Process config menu command response received from the initiator.  Only RFBASE will use this, and will send it out to the standard interface.
//
void MATRIX_SYN_FE_COMMAND_HANDLER::ProcessConfigMenuCommandResponse(int * /* Status */, char * /* Result */, HTAG_MENU_COMMAND * /* MenuCommandResponse */ )
{

	return;
}



