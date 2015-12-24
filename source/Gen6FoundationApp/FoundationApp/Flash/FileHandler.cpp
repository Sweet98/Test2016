/*===================================================================
  The class will eventually contain processing for files to be
  stored.  We may or may not add reading & processing of already
  stored files--it depends on how big this class gets (TBD).

=====================================================================
   $Source: Gen6FoundationApp/FoundationApp/Flash/FileHandler.cpp $
   $Date: 2011/09/23 15:03:40EDT $
   $Revision: 1.9 $


===================================================================*/
#include "FileHandler.h"
#include "MocfHandler.h"
#include "MocfTags.h"
#include "HSTRING.h"
#include "PowerManagement.h"
#include "menu.h"
#include "ReaderConfiguration.h"
#include "db_chrio.h"
#include "db_hdw.h"
#include "hab_header.h"
#include "Interface_Support_Functions.h" // bVisMnuOverride, VisMnuPtr
#include "StartupFlashdisk.h"




//#define INC_MOCF_DEBUG // uncomment this to include debug prints to std interface



#ifdef INC_MOCF_DEBUG
#include "ioman.h"
// FYI:  the "## in the macro expansion handles the case MOCF_DEBUG("only text, no variables")
#define MOCF_DEBUG(format, ...) theStdInterface.printf(format, ## __VA_ARGS__)
#else
#define MOCF_DEBUG(format, ...)
#endif


// Use with caution to avoid security leaks!
// For now the code is copied from StoreUserFile. Later we can remove some duplications. (We are in the end phase before a release)
bool CFileHandler::StoreFile(const char * pFileName, const unsigned char * pData, int DataLength)
{
	int NumWritten = WriteAndCloseFile(pFileName, pData, DataLength);
	if ( NumWritten != DataLength ) return false;

	return true;
}

bool CFileHandler::StoreUserFile(const char * pFileName, const unsigned char * pData, int DataLength)
{
	HSTRING filename(100, true);

	filename.Copy(USER_FOLDER);
	filename.Append("/");
	filename.Append(pFileName);

	int NumWritten = WriteAndCloseFile((const char *) filename.Char, pData, DataLength);
	if ( NumWritten != DataLength ) return false;

	return true;
}



bool CFileHandler::ProcessSystemUpdateMocf(const unsigned char * pMocf)
{
	CGen6MocfHandler MocfHandler((CTlv::Header_t *) pMocf);
	const CTlv::Header_t * pRecord;

	MOCF_DEBUG("Checking to make sure it's valid\n");
	if ( ! MocfHandler.IsValid() ) return false;
	MOCF_DEBUG("Checking to make sure it's a System Update MOCF\n");
	if ( ! MocfHandler.IsSystemUpdateMocf() ) return false;
	MOCF_DEBUG("Checking to make sure we have required Object Records\n");
	if ( ! MocfHandler.FindRecord(Gen6::tagBootFirmware, &pRecord) ) return FALSE;
	if ( ! MocfHandler.FindRecord(Gen6::tagAppFirmware, &pRecord) ) return FALSE;
	if ( ! MocfHandler.FindRecord(Gen6::tagMenuSettings, &pRecord) ) return FALSE;  // We must have at least a FACTON1 setting.
	if ( ! MocfHandler.FindRecord(Gen6::tagSoftwareKeys, &pRecord) ) return FALSE;  // We must have at least a FIMPNM setting.
	MOCF_DEBUG("Checking to make sure there's room\n");
	if ( ! MocfHandler.IsThereRoom() ) return false;

	MOCF_DEBUG("Formatting filesystem\n");
	if ( FormatVolume(0) < 0 ) return false;
	CreateFolders();
	MOCF_DEBUG("Storing Objects\n");
	if ( ! MocfHandler.StoreObjects() ) return false;

	return true;
}



/*----------------------------------------------------------------------------------------
ProcessMocf

  This will process an MOCF.

  Some reverse engineering notes:
	  Legacy code had an option to send an ACK if OK, but there was no code that ever enabled it.  So, it looks
	  like we send a NAK on error (and never an ACK).  The ACK for OK comes out later after we reboot and apply the Menu Settings
	  in the UpdateInformation file (see SaveInterfaceMenuSettings()).

  Parameters:
  				pMocf[in]:  pointer to the MOCF.

  				MocfLength[in]:  length of the MOCF.

  				bRunAppFirmwareFromRam[in]:  This is used to download Application Firmware into RAM and run it (not store it to flash).
  											 No external customer application can set this to true.

  				bDoCompatibleCheck[in]:  This tells us to do the compatibility check.  No external customer application can set this to false.

  				bSendResponseAfterReboot[in]:  This tells us to send the ACK after reboot.  This was added to support HTAG's Response Required parameter.

  Returns:  false if the MOCF does not contain an Application Firmware.
----------------------------------------------------------------------------------------*/
bool CFileHandler::ProcessMocf(const unsigned char * pMocf, int MocfLength, bool bRunAppFirmwareFromRam /*= false*/, bool bDoCompatibleCheck /*= true*/, bool bSendResponseAfterReboot/* = true*/)
{
	CGen6MocfHandler MocfHandler((CTlv::Header_t *) pMocf);
	bool bIsOk;


	ProcessingAppCode = true;  // this is a global variable used in power management
	bIsOk = MocfHandler.IsValid();
	if ( bIsOk && MocfHandler.IsSystemUpdateMocf() ) bIsOk = false;
	if ( bIsOk && bDoCompatibleCheck ) bIsOk = MocfHandler.IsCompatible(GET_SETTING(CompatibleProductsId));

	if ( bIsOk && bRunAppFirmwareFromRam ) bIsOk = RunFromRam(pMocf); // RunFromRam() will only return with a false

	if ( bIsOk ) bIsOk = MocfHandler.IsThereRoom();
	if ( bIsOk ) bIsOk = MocfHandler.StoreObjects();
	if ( bIsOk )
	{
		if ( bSendResponseAfterReboot ) SaveInterfaceMenuSettings();
		reset_to_boot_code_hard();
	}

	// If we get here, there was some sort of problem (bIsOk must be false)
	ProcessingAppCode = false;
	return bIsOk;
}







/*----------------------------------------------------------------------------------------
RunFromRam

  This will extract the Application Firmware binary and load it into RAM.  If all goes
  well, it will not return.  It ignorea all other Records in the MOCF.

  NOTE:  RelocateAndRun() is broken, so we'll always return false;


  Parameters:
  				pMocf[in]:  pointer to the MOCF containing the firmware binary.

  Returns:  false if the MOCF does not contain an Application Firmware.
----------------------------------------------------------------------------------------*/
bool CFileHandler::RunFromRam(const unsigned char * pMocf)
{
	/*
	CGen6MocfHandler MocfHandler((CTlv::Header_t *) pMocf);
	const CTlv::Header_t * pRecord;

	if ( MocfHandler.FindRecord(Gen6::tagAppFirmware, &pRecord) )
	{
		CFirmwareParser FwParser(pRecord);
		const unsigned char * pFirmwareBin;
		DWORD FwLength;

		if ( ! FwParser.GetFwData(&pFirmwareBin, &FwLength) ) return false;
		// we now have a pointer to the raw binary firmware
		wait_til_tx_is_empty();
		RESET_WATCHDOG_TIMER();  // In case the watchdog keeps running, kick the timer
		RelocateAndRun(pFirmwareBin, ((BYTE *) (&RAM_RESET)), FwLength);  // go run the code
	}
	else return false;
	*/
	return false;  // we'll never get here.  if we do, something went wrong.
}


/*----------------------------------------------------------------------------------------
SaveInterfaceMenuSettings

  The desktop application may change our interface setting so it can download firmware
  (for example, if our interface was keyboard wedge).  For some reason, it relies on the
  firmware to keep track of things (instead of changing it back after the download).  So,
  we store the settings in a file here and restore them after we boot.  FYI:  the final
  ACK comes out after we restore the settings.

  Parameters:  none

  Returns:  nothing
----------------------------------------------------------------------------------------*/
void CFileHandler::SaveInterfaceMenuSettings(void)
{
	HSTRING FlashDiskData(100, true);
	if ( bVisMnuOverride )
	{
		FlashDiskData.Copy(VisMnuPtr);
	}
	else
	{
		// For RF scanner builds, if the current interface is not theStdInterface, then we must have received code via the radio link.  In
		// all other instances, it must have been by the cord.
		int DownloadMediumVal;
		
		if ( GetCurrentInterface() == g_pStdInterface ) DownloadMediumVal = DOWNLOAD_MEDIUM_IS_CORD;
		else  DownloadMediumVal = DOWNLOAD_MEDIUM_IS_CORD_REM;
		RAM_SET_ACTUAL(DownloadMedium, DownloadMediumVal);
		FlashDiskData.Copy("060702$;060703$;060704$;060705$;060706$;060707$;060708$;060709$;06070A$;06070B$;0D1200$;060500$!");
		ReaderConfiguration->Menu(&FlashDiskData);
	}
	WriteAndCloseFile(FILE_FW_UPDATE, FlashDiskData.Char, FlashDiskData.Size);
}


