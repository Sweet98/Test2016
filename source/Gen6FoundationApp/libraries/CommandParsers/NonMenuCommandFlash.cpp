//======================================================================================
// 
//======================================================================================
// $RCSfile: NonMenuCommandFlash.cpp $
// $Revision: 1.16 $
// $Date: 2010/03/26 14:57:15EDT $
//======================================================================================

//======================================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "db_chrio.h"
#include "db_sharedtable.h"
#include "db_NonMenuCommands.h"
#include "MatrixHeader.h"
#include "HSTRING.h"
#include "Matrixnonmenuhmodem.h"
#include "FlashApplicationCode.h"
#ifdef SAFE_FFS_USED
#include "StartupFlashdisk.h"
#endif
#include "thread.h"
#include "menu.h"
#include "ReaderConfiguration.h"
#include "beep.h"
#include "Interface_Support_Functions.h" // bVisMnuOverride, VisMnuPtr
#include "hab_header.h" // SpiFlashBootCode
#include "FileHandler.h"

#define OUTPUT_DEBUG_MESSAGE theCurrentInterface.printf




volatile bool 	ProcessingAppCode = false;




//
//	ProcessNewApplicationCode
//		Determine compatibility of new application code and either store it to flash and reboot OR run it from RAM.
//
bool ProcessNewApplicationCode(const unsigned char *ApplicationCode, int SizeOfApplicationCode, bool SkipApplicationTypeCheck, bool SkipDeviceTypeCheck,
                                bool FlashTheNewCode, bool SendACKIfSuccessful, bool ResetAfterFlash )
{
	bool bIsOk;
	if ( (ApplicationCode[0] == 'M') && (ApplicationCode[1] == 'O') && (ApplicationCode[2] == 'C') && (ApplicationCode[3] == 'F') )
	{	// It looks like an MOCF.
		// FYI:  only "APPCOD" can set SendACKIfSuccessful to true &  ResetAfterFlash to false.
		// FYI:  only "NEWAPP" can set SkipApplicationTypeCheck to false (another non Menu Command has to be sent first).
		bool bCheckCompatible = true;
		if ( SkipApplicationTypeCheck || SkipDeviceTypeCheck ) bCheckCompatible = false;
		bIsOk = CFileHandler::ProcessMocf(ApplicationCode, SizeOfApplicationCode, ! FlashTheNewCode, bCheckCompatible);
	}
	else bIsOk = false;
	// The only way we ACK is if we're an RFBASE and we routed the MOCF to the scanners.
	if ( bIsOk ) xputchar(ACK);
	else xputchar(NAK);

	return bIsOk;
}



//
//	ProcessLegacyBootCode
//		Determine compatibility of new boot code store it to flash if OK.
//
bool ProcessLegacyBootCode( unsigned char *BootCode, int SizeOfBootCode, bool SkipDeviceTypeCheck )
{
	// Wait for all host writes to cease, then disable IRQs.
	wait_til_tx_is_empty();
	if(( GetDeviceType( ((char *) BootCode), SizeOfBootCode ) ==
	        GetDeviceType(((char *) CACHED_FLASH_BASE), FLASH_MAX_BOOT_CODE_LENGTH)) || ( SkipDeviceTypeCheck ))
	{
		if(SpiFlashBootCode(BootCode,SizeOfBootCode)==0)                          
		{
			HSTRING * FlashDiskData = new HSTRING(100,true);

			// For RF scanner builds, if the current interface is not theStdInterface, then we must have received code via the radio link.  In
			// all other instances, it must have been by the cord.
#ifdef RFSCAN
#else

			RAM_SET_ACTUAL( DownloadMedium, DOWNLOAD_MEDIUM_IS_CORD );                         
#endif

#ifdef RFSCAN

#else

			FlashDiskData->Copy("060702$;060703$;060704$;060705$;060706$;060707$;060708$;060709$;06070A$;06070B$;0D1200$;060500$!");
#endif

			ReaderConfiguration->Menu(FlashDiskData);
#ifdef SAFE_FFS_USED
				WriteAndCloseFile(FILE_FW_UPDATE,FlashDiskData->Char,FlashDiskData->Size);
#else
				FlashDisk->Write(FILE_FW_UPDATE,(char *)FlashDiskData->Char,FlashDiskData->Size,MATRIX_FLASH_DISK::FD_OTHER, 0, 0, 0);
#endif
			xputchar(ACK);
			wait_til_tx_is_empty();
			reset_to_boot_code_hard();
		}
		else
		{
			xputchar(NAK);
		}
	}
	else
	{
		xputchar(NAK);

		if (GET_SETTING(GeneralDebugOn))
		{
			OUTPUT_DEBUG_MESSAGE("Illegal boot code type for device\r\n");
		}
	}
	// We should never get here if the code is OK, so return false to indicate that something went wrong.
	return(false);
} // ProcessLegacyBootCode



//	ProcessNewBootCode
//		Determine compatibility of new boot code store it to flash if OK.
bool ProcessNewBootCode( unsigned char * pBootCode, int SizeOfBootCode, bool SkipDeviceTypeCheck )
{
	if ( (pBootCode[0] == 'M') && (pBootCode[1] == 'O') && (pBootCode[2] == 'C') && (pBootCode[3] == 'F') )
	{	// It looks like an MOCF.
		// FYI:  only "BOTCOD" can set SkipDeviceTypeCheck to true.
		// FYI:  only "NEWBOOT" can set SkipDeviceTypeCheck to false (another non Menu Command has to be sent first).
		bool bIsOk = CFileHandler::ProcessMocf(pBootCode, SizeOfBootCode, false, ! SkipDeviceTypeCheck);
		// The only way we ACK is if we're an RFBASE and we routed the MOCF to the scanners.
		if ( bIsOk ) xputchar(ACK);
		else xputchar(NAK);
		
		return bIsOk;
	}
	else return ProcessLegacyBootCode(pBootCode, SizeOfBootCode, SkipDeviceTypeCheck);
}



//
//	DownloadBootCode
//		The host has requested that we download new boot code.
//
bool DownloadBootCode( bool SkipDeviceTypeCheck )
{
	UCHAR	*Buffer;					// Pointer to receive buffer on heap.
	int		NumberOfBytesTransferred = 0;	// Number of bytes downloaded from host.
	char	TempStr[160];
	if(BootCodeDownloadMode < 1)
	{
		xputchar(NAK);
		xputstring("\025Boot download not enabled; download terminated\015\012");
		return(false);
	}


	// Attempt to malloc some space for the buffer, allocating enough space to include the copy_and_execute routine.
	// If it fails, return.
	Buffer = ((BYTE *) malloc(FLASH_MAX_BOOT_CODE_LENGTH + 40000));
	if(Buffer == NULL)
	{
		xputchar(NAK);
		xputstring("\025Not enough memory for download\015\012");
		return(false);
	}

	// Do the HModem receive.
	NonMenuHModem *hm = new NonMenuHModem();
	hm->SetReceiveTimeout(500);

	UINT HModemStatus = hm->Receive(Buffer, ((UINT *) &NumberOfBytesTransferred), FLASH_MAX_BOOT_CODE_LENGTH);
	delete hm;

	//If an error occurred, log it if normal debug mode is on and reboot.
	if (HModemStatus != HMODEM::FINISH_SUCCESSFUL)
	{
		if ((GET_SETTING(GeneralDebugOn)) && (HModemStatus != HMODEM::TERMINATE_BY_US))
		{
			MT_Delay(SLEEP_AFTER_TERMINATE * 2);
			ProcessHmodemError(HModemStatus);
		}
	}
	else
	{
		if (GET_SETTING(GeneralDebugOn))
		{
			OUTPUT_DEBUG_MESSAGE("\015\012");		// Move to a new line.
			sprintf(TempStr, "Downloading %d bytes of new boot code...\015\012", NumberOfBytesTransferred);
			OUTPUT_DEBUG_MESSAGE(TempStr);
		}

		ProcessNewBootCode( Buffer, NumberOfBytesTransferred, SkipDeviceTypeCheck );
	}
	free(Buffer);
	return(false);
} // DownloadBootCode




//
//	DownloadApplicationCode
//		The host has requested that we download new applcation code.
//
bool DownloadApplicationCode( bool SkipApplicationTypeCheck, bool SkipDeviceTypeCheck, bool FlashTheNewCode)
{
	HSTRING	Buffer(FLASH_MAX_APPLICATION_AREA_LENGTH,true);			// Receive buffer on heap.
	int		NumberOfBytesTransferred;	// Number of bytes downloaded from host.
	char	TempStr[160];

	// Do the HModem receive.
	NonMenuHModem *hm = new NonMenuHModem();

//LinNing for ReM1.0: POSTMO
	hm->SetReceiveTimeout(GET_SETTING(HModemDownAppReceiveTimeOut));

	UINT HModemStatus = hm->Receive(&Buffer, FLASH_MAX_APPLICATION_AREA_LENGTH);
	NumberOfBytesTransferred = Buffer.Size;
	delete hm;

	// If an error occurred, log it if normal debug mode is on and reboot.
	if(HModemStatus != HMODEM::FINISH_SUCCESSFUL)
	{
		if ((GET_SETTING(GeneralDebugOn)) && (HModemStatus != HMODEM::TERMINATE_BY_US) ) 
		{
			MT_Delay(SLEEP_AFTER_TERMINATE * 2);
			ProcessHmodemError(HModemStatus);
		}

		SoundErrorBeep();
		MT_Delay(SLEEP_AFTER_TERMINATE * 2);
		reset_to_boot_code_hard();
	}
	else
	{
		if (GET_SETTING(GeneralDebugOn))
		{
			OUTPUT_DEBUG_MESSAGE("\015\012");		// Move to a new line.

			sprintf(TempStr, "Downloading %d bytes of new application code...\015\012", NumberOfBytesTransferred);
			OUTPUT_DEBUG_MESSAGE(TempStr);
		}
		wait_til_tx_is_empty();
		ProcessNewApplicationCode( Buffer.Char, NumberOfBytesTransferred, SkipApplicationTypeCheck, SkipDeviceTypeCheck, FlashTheNewCode );
	}
	return(false);
} // DownloadApplicationCode


void ProcessHmodemError(unsigned int HModemStatus)
{
	char Result[160];

	switch(HModemStatus)
	{
	case HMODEM::TIMEOUT_WAITING_FOR_HMODEM_BLOCK_SIZE:
		OUTPUT_DEBUG_MESSAGE("Timeout while waiting for HMODEM block size\015\012");
		break;
	case HMODEM::TERMINATE_RECEIVED:
		OUTPUT_DEBUG_MESSAGE("Transmitter terminated HMODEM\015\012");
		break;
	case HMODEM::ILLEGAL_HMODEM_BLOCK_SIZE:
		OUTPUT_DEBUG_MESSAGE("Illegal HMODEM block size received\015\012");
		break;
	case HMODEM::TIMEOUT_WAITING_FOR_HMODEM_BLOCK_NUMBER:
		OUTPUT_DEBUG_MESSAGE("Timeout while waiting for HMODEM block number\015\012");
		break;
	case HMODEM::MISMATCHED_HMODEM_BLOCK_NUMBER:
		OUTPUT_DEBUG_MESSAGE("Mismatched HMODEM block number\015\012");
		break;
	case HMODEM::TIMEOUT_WAITING_FOR_HMODEM_INVERSE_BLOCK_NUMBER:
		OUTPUT_DEBUG_MESSAGE("Timeout while waiting for HMODEM inverse block number\015\012");
		break;
	case HMODEM::MISMATCHED_HMODEM_INVERSE_BLOCK_NUMBER:
		OUTPUT_DEBUG_MESSAGE("Timeout while waiting for HMODEM inverse block number\015\012");
		break;
	case HMODEM::TIMEOUT_WAITING_FOR_HMODEM_BLOCK:
		OUTPUT_DEBUG_MESSAGE("Timeout while waiting for HMODEM data block\015\012");
		break;
	case HMODEM::TIMEOUT_WAITING_FOR_HMODEM_CRC:
		OUTPUT_DEBUG_MESSAGE("Timeout while waiting for HMODEM CRC\015\012");
		break;
	case HMODEM::MISMATCHED_HMODEM_CRC:
		OUTPUT_DEBUG_MESSAGE("Mismatched HMODEM CRC\015\012");
		break;
	default:
		sprintf( Result, "Error %d while transmitting HMODEM image\015\012", HModemStatus );
		OUTPUT_DEBUG_MESSAGE( Result );
		break;
	}
}



