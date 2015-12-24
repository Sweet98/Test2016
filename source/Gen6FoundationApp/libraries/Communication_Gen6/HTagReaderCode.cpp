//======================================================================================
// HTagReaderCode.cpp
//======================================================================================
// $RCSfile: HTagReaderCode.cpp $
// $Revision: 1.1 $
// $Date: 2009/03/08 16:04:20EDT $
//======================================================================================

//======================================================================================


#include	"Compression.h"			// Compression interface definitions
#include	"HTagReaderCode.h"		// HTAG_READER_CODE interface definition

#include	<stdlib.h>				// Standard library functions
#include	<stdio.h>
#include	<string.h>

//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//
//  Class Name:     HTAG_READER_CODE
//
//  Class Description:
//		The HTAG_READER_CODE class provides a mechanism to transfer reader code.  Since all fields are tagged, if the
//		receiver does not yet speak the new version of HTAG_READER_CODE, it will ignore tags that it doesn't understand.
//
//---------------------------------------------------------------------------







//
// Constructor for HTAG_READER_CODE; overloaded version #1 for loading reader code later.
//      Indicate that no reader code has been loaded yet by setting Data size to zero.
//
HTAG_READER_CODE::HTAG_READER_CODE(const char *Command, HCOMM *HCommDevice, HTRANSFER_STATUS *TransferStatus,
								HCOMMAND_HANDLER *CommandHandler)
		: HTAG( HCommDevice, TransferStatus, CommandHandler, false )
{
	Data = new HSTRING(DEFAULT_READER_CODE_DATA_ALLOCATION, true);
	HTAG::Data = Data;				// Give the pointer to the base class so it can operate on the data as well.
	CommandName->Copy(Command);

// Add file name workgroup parameters.
	FileName = new HSTRING(20, true);
	AddHStringParameter(HTAG_READER_CODE_FILE_PARAMETER_NAME, FileName, "UNKNOWN", 7, true );
	AddNumericParameter(HTAG_READER_CODE_COMPRESSION_TYPE_PARAMETER_NAME, &CompressionType, NO_COMPRESSION,
					NO_COMPRESSION, INVALID_COMPRESSION_MODE - 1, true );
	AddNumericParameter( HTAG_READER_CODE_STORE_FILE_PARAMETER_NAME, &StoreFile, 1, 0, 1, true );
	AddNumericParameter( HTAG_READER_CODE_START_ADDRESS_PARAMETER_NAME, &StartAddress, 0xFFFFFFFF, true );
	AddNumericParameter( HTAG_READER_CODE_SIZE_PARAMETER_NAME, &SizeOfImage, 0xFFFFFFFF, true );
	AddNumericParameter( HTAG_READER_CODE_BOOT_MODE_FLASH_IMAGE_PARAMETER_NAME, &BootModeFlashImage, 0, 0, 1, true );
	AddNumericParameter( HTAG_READER_CODE_CRC_PARAMETER_NAME, &CRC, -1, true );
	AddNumericParameter( HTAG_READER_CODE_REBOOT_PARAMETER_NAME, &Reboot, 0, 0, 1, true );
	AddNumericParameter( HTAG_READER_CODE_OVERRIDE_APPLICATION_TYPE_CHECK_PARAMETER_NAME, &OverrideApplicationTypeCheck,
					0, 0, 1, false );
	AddNumericParameter( HTAG_READER_CODE_OVERRIDE_DEVICE_TYPE_CHECK_PARAMETER_NAME, &OverrideDeviceTypeCheck,
					0, 0, 1, false );
	AddNumericParameter( HTAG_READER_CODE_DO_INDICATORS, &DoIndicators, HTAG_READER_CODE_DO_INDICATORS_DEFAULT, 0, 1, false );
	AddNumericParameter( HTAG_READER_CODE_STOP_AFTER_DOWNLOAD, &StopAfterDownload, HTAG_READER_CODE_STOP_AFTER_DOWNLOAD_DEFAULT, 0, 1, false );
	AddNumericParameter( HTAG_READER_CODE_SAVE_SERIAL_NUMBERS, &SaveSerialNumbers, HTAG_READER_CODE_SAVE_SERIAL_NUMBERS_DEFAULT, 0, 1, false );

    return;
}
//---------------------------------------------------------------------------





//
// Destructor for HTAG_READER_CODE
//      If memory has been allocated to reader code, release it.
//
HTAG_READER_CODE::~HTAG_READER_CODE(void)
{
	delete Data;
	delete FileName;

    return;
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
//
//  Class definitions for HTAG_SCAN_ENGINE_FIRMWARE
//
//
//	The HTAG_SCAN_ENGINE_FIRMWARE class provides a mechanism to transfer scan engine firmware.  Since all fields are
//	tagged, if the receiver does not yet speak the new version of HTAG_SCAN_ENGINE_FIRMWARE, it will ignore tags that
//	it doesn't understand.
//
//---------------------------------------------------------------------------







//
// Constructor for HTAG_SCAN_ENGINE_FIRMWARE; overloaded version #1 for loading firmware later.
//      Indicate that no firmware has been loaded yet by setting Data size to zero.
//
HTAG_SCAN_ENGINE_FIRMWARE::HTAG_SCAN_ENGINE_FIRMWARE(const char *Command, HCOMM *HCommDevice, HTRANSFER_STATUS *TransferStatus,
								HCOMMAND_HANDLER *CommandHandler)
		: HTAG( HCommDevice, TransferStatus, CommandHandler, false )
{
	Data = new HSTRING(DEFAULT_READER_CODE_DATA_ALLOCATION, true);
	HTAG::Data = Data;				// Give the pointer to the base class so it can operate on the data as well.
	CommandName->Copy(Command);

// Add file name workgroup parameters.
	FileName = new HSTRING(20, true);
	AddHStringParameter( HTAG_SCAN_ENGINE_FIRMWARE_FILE_PARAMETER_NAME, FileName, "UNKNOWN", 7, true );
	AddNumericParameter( HTAG_SCAN_ENGINE_FIRMWARE_COMPRESSION_TYPE_PARAMETER_NAME, &CompressionType,
					HTAG_SCAN_ENGINE_FIRMWARE_UNCOMPRESSED_HEX, HTAG_SCAN_ENGINE_FIRMWARE_UNCOMPRESSED_HEX,
					HTAG_SCAN_ENGINE_FIRMWARE_END_OF_COMPRESSION_TYPES - 1, true );
	AddNumericParameter( HTAG_READER_CODE_CRC_PARAMETER_NAME, &CRC, -1, true );
	AddNumericParameter( HTAG_SCAN_ENGINE_FIRMWARE_STORE_FILE_PARAMETER_NAME, &StorageType,
					HTAG_SCAN_ENGINE_FIRMWARE_STORE_TO_SCAN_ENGINE_ONLY, HTAG_SCAN_ENGINE_FIRMWARE_STORE_TO_SCAN_ENGINE_ONLY,
					HTAG_SCAN_ENGINE_FIRMWARE_END_OF_STORAGE_TYPES - 1, true );
	AddNumericParameter( HTAG_SCAN_ENGINE_FIRMWARE_IGNORE_TYPE_MISMATCH_PARAMETER_NAME, &IgnoreFirmwareTypeMismatch,
					0, 0, 1, false );

    return;
}
//---------------------------------------------------------------------------





//
// Destructor for HTAG_SCAN_ENGINE_FIRMWARE
//      If memory has been allocated to reader code, release it.
//
HTAG_SCAN_ENGINE_FIRMWARE::~HTAG_SCAN_ENGINE_FIRMWARE(void)
{
	delete Data;
	delete FileName;

    return;
}
//---------------------------------------------------------------------------









