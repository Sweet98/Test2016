//======================================================================================
// HTagReaderCode.h
//======================================================================================
// $RCSfile: HTagReaderCode.h $
// $Revision: 1.3 $
// $Date: 2011/11/09 15:24:22EST $
//======================================================================================

//======================================================================================



//---------------------------------------------------------------------------
#ifndef HTAG_READER_CODE_H
#define HTAG_READER_CODE_H
//---------------------------------------------------------------------------


#include "HTag.h"




//---------------------------------------------------------------------------
//
//  Class definitions for HTAG_READER_CODE
//
//---------------------------------------------------------------------------

//
//	The HTAG_READER_CODE class provides a mechanism to transfer reader code.  Since all fields are tagged, if the
//	receiver does not yet speak the new version of HTAG_READER_CODE, it will ignore tags that it doesn't understand.
//



class HTAG_READER_CODE : public HTAG
{
public:
// Status codes
//	typedef enum {
//		HTAG_READER_CODE_FILE_NAME = HTAG::HTAG_USER_DEFINED_STATUS_BEGIN,
//		HTAG_READER_CODE_WORKGROUP
//	} HTAG_READER_CODE_STATUS;


// Information about the script.
	#define	HTAG_READER_CODE_FILE_PARAMETER_NAME								"FILE"
	#define	HTAG_READER_CODE_COMPRESSION_TYPE_PARAMETER_NAME					"CMP"
	#define	HTAG_READER_CODE_STORE_FILE_PARAMETER_NAME							"STORE"
	#define	HTAG_READER_CODE_START_ADDRESS_PARAMETER_NAME						"STARTADDRESS"
	#define	HTAG_READER_CODE_SIZE_PARAMETER_NAME								"SIZE"
	#define	HTAG_READER_CODE_BOOT_MODE_FLASH_IMAGE_PARAMETER_NAME				"BOOTMODEFLASHIMAGE"
	#define	HTAG_READER_CODE_CRC_PARAMETER_NAME									"CRC"
	#define	HTAG_READER_CODE_REBOOT_PARAMETER_NAME								"REBOOT"
	#define	HTAG_READER_CODE_OVERRIDE_APPLICATION_TYPE_CHECK_PARAMETER_NAME		"OVERRIDEAPPTYPE"
	#define	HTAG_READER_CODE_OVERRIDE_DEVICE_TYPE_CHECK_PARAMETER_NAME			"OVERRIDEDEVTYPE"
	#define	HTAG_READER_CODE_DO_INDICATORS										"INDICATORS"
	#define	HTAG_READER_CODE_STOP_AFTER_DOWNLOAD								"STOPAFTERDOWNLOAD"
	#define	HTAG_READER_CODE_SAVE_SERIAL_NUMBERS								"SAVESERIALNUMBERS"

	HSTRING		*FileName;
	int			CompressionType;
	int			StoreFile;			// 1 to store code file in device, 0 to run only
	int			StartAddress;		// Specifies the start address of a flash image
	int			SizeOfImage;		// Specifies the size of a flash image to be retrieved
	int			BootModeFlashImage;	// Specifies whether or not this image may overwrite all of flash, including boot area.
	int			CRC;				// Specifies the CRC of the image (-1 indicates that it is unused).
	int			Reboot;				// Specified whether or not to reboot after the operation.
	int			OverrideApplicationTypeCheck;
	int			OverrideDeviceTypeCheck;
	int			DoIndicators;		// For flash image writes, use indicators to notify user when download is in process and
									//	when it is finished.
	int			StopAfterDownload;	// When a flash image write has completed, sit in a loop until the user power cycles the
									//	unit.
	int			SaveSerialNumbers;	// Save product configuration serial #s before a flash image write and restore them when
									//	the write has completed.

	#define	HTAG_READER_CODE_DO_INDICATORS_DEFAULT			0
	#define	HTAG_READER_CODE_STOP_AFTER_DOWNLOAD_DEFAULT	0
	#define	HTAG_READER_CODE_SAVE_SERIAL_NUMBERS_DEFAULT	0


public:


// Pointer to the actual reader code.
	#define	DEFAULT_READER_CODE_DATA_ALLOCATION				200
	HSTRING		*Data;




// Constructor for HTAG_READER_CODE; overloaded version #1 for loading reader code later.
    HTAG_READER_CODE(const char *Command, HCOMM *HCommDevice = 0, HTRANSFER_STATUS *TransferStatus = 0,
								HCOMMAND_HANDLER *CommandHandler = 0);

// Destructor for HTAG_READER_CODE
    ~HTAG_READER_CODE(void);
    
private:
	HTAG_READER_CODE (const HTAG_READER_CODE&);            // declared private, but not defined, to prevent copying         
	HTAG_READER_CODE& operator=(const HTAG_READER_CODE&);  // declared private, but not defined, to prevent copying 

};
//----------------------------------------------------------------------------




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



class HTAG_SCAN_ENGINE_FIRMWARE : public HTAG
{
public:
	#define	HTAG_SCAN_ENGINE_FIRMWARE_DEFAULT_COMMAND_NAME						"ENGSTO"

// Status codes
//	typedef enum {
//		HTAG_SCAN_ENGINE_FIRMWARE_FILE_NAME = HTAG::HTAG_USER_DEFINED_STATUS_BEGIN,
//		HTAG_SCAN_ENGINE_FIRMWARE_WORKGROUP
//	} HTAG_SCAN_ENGINE_FIRMWARE_STATUS;


// Information about the firmware.
	#define	HTAG_SCAN_ENGINE_FIRMWARE_FILE_PARAMETER_NAME					"FILE"
	#define	HTAG_SCAN_ENGINE_FIRMWARE_COMPRESSION_TYPE_PARAMETER_NAME		"CMP"
	#define	HTAG_SCAN_ENGINE_FIRMWARE_CRC_PARAMETER_NAME					"CRC"
	#define	HTAG_SCAN_ENGINE_FIRMWARE_STORE_FILE_PARAMETER_NAME				"STORE"
	#define	HTAG_SCAN_ENGINE_FIRMWARE_IGNORE_TYPE_MISMATCH_PARAMETER_NAME	"IGNTYP"

	HSTRING		*FileName;

	typedef enum {
		HTAG_SCAN_ENGINE_FIRMWARE_UNCOMPRESSED_HEX = 0,
		HTAG_SCAN_ENGINE_FIRMWARE_COMPRESSED_HEX,
		HTAG_SCAN_ENGINE_FIRMWARE_END_OF_COMPRESSION_TYPES
	} HTAG_SCAN_ENGINE_FIRMWARE_COMPRESSION_TYPE;

	int			CompressionType;
	int			CRC;				// Specifies the CRC of the image (-1 indicates that it is unused).

	typedef enum {
		HTAG_SCAN_ENGINE_FIRMWARE_STORE_TO_SCAN_ENGINE_ONLY = 0,
		HTAG_SCAN_ENGINE_FIRMWARE_STORE_TO_FLASH_FILE_ONLY,
		HTAG_SCAN_ENGINE_FIRMWARE_STORE_TO_SCAN_ENGINE_AND_STORE_TO_FLASH_FILE,
		HTAG_SCAN_ENGINE_FIRMWARE_END_OF_STORAGE_TYPES
	} HTAG_SCAN_ENGINE_FIRMWARE_STORAGE_TYPE;

	int			StorageType;

	int			IgnoreFirmwareTypeMismatch;


public:


// Pointer to the actual firmware.
	#define	DEFAULT_PSOC_FIRMWARE_DATA_ALLOCATION				200
	HSTRING		*Data;




// Constructor for HTAG_SCAN_ENGINE_FIRMWARE; overloaded version #1 for loading firmware later.
    HTAG_SCAN_ENGINE_FIRMWARE(const char *Command, HCOMM *HCommDevice = 0, HTRANSFER_STATUS *TransferStatus = 0,
								HCOMMAND_HANDLER *CommandHandler = 0);

// Destructor for HTAG_SCAN_ENGINE_FIRMWARE
    ~HTAG_SCAN_ENGINE_FIRMWARE(void);
    
private:
	HTAG_SCAN_ENGINE_FIRMWARE (const HTAG_SCAN_ENGINE_FIRMWARE&);            // declared private, but not defined, to prevent copying         
	HTAG_SCAN_ENGINE_FIRMWARE& operator=(const HTAG_SCAN_ENGINE_FIRMWARE&);  // declared private, but not defined, to prevent copying 

};
//----------------------------------------------------------------------------




#endif // HTAG_READER_CODE_H



