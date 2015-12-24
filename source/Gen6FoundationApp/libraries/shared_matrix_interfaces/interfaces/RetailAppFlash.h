/*=================================================================================
  file: RetailAppFlash.h

===================================================================================
   $Source: interfaces/RetailAppFlash.h $
   $Date: 2010/10/05 14:08:29EDT $
   $Revision: 1.5.1.5 $


==================================================================================*/
#ifndef RETAILPPPFLASH_H
#define RETAILPPPFLASH_H

typedef enum
{
	NOT_FLASH_MODE = 0,
	FLASH_MODE,
	WAIT_FOR_CRC,
	WAIT_FOR_ERASE,
	DOWNLOADING_4683,
	WAIT_FOR_UPDATE,
	READY_TO_FLASH_SCANNER,
	SCANNER_FLASHED,
	READY_TO_FLASH_BASE,
	BASE_FLASHED,
	ERROR_WHILE_FLASHING,
	READY_TO_RESET,
	TF_FLASH_MODE,
	TF_PAUSED,
	TF_COMPLETE,
	TF_FLASH_TRANSFER_COMPLETE,
	TF_ACTIVATE
} flashStatus_t;

#define MAXIMUM_FLASH_SECTIONS   35;
#define SECTIONLEN_1         0x0010;
#define TIMER_RESET_VALUE   1000000;
#define DIO_CONFIGURATION_LENGTH 12

typedef struct
{
	flashStatus_t		flashStatus;		// Status of trickle feed flash
	unsigned char		*flashBuffer;		// Buffer to store the firmware file in - close to 2MB
	unsigned int		offset;				// Offset/current length
	unsigned int		ulCRC;				// Calculated CRC32 using NCR's CRC utility
	unsigned int		headerLength;		// Length of the header section
	unsigned int		scanLength;			// Length of scanner firmware
	unsigned int		baseLength;			// Length of base firmware
	unsigned int		fileLength;			// The total file length defined in first section
	unsigned int		fileCRC;			// CRC32 of the full file
	unsigned int		calcFileCRC;		// CRC32 calculated - kept for debugging purposes
	unsigned int		errorCondition;		// Error Conditions
}tfData_t;

typedef struct
{
	flashStatus_t       flashStatus;        // Status for flashing over 4683 tailgagte
	unsigned char       *flashBuffer;       // Pointer to beginning of scanner firmware
	unsigned char       *tempFlashPtr;      // Moving pointer for current incoming data position
	unsigned char       *endFlashPtr;       // Pointer to the end of the scanner firmware
	int                 flashBufferLength;  // Length of flash buffer
	int                 sectionLength;      // Length of current section (used for CRCs)
	int                 sections[35];       // Array of lengths of each section
	int                 currentSection;     // Current Section being downloaded to
	int                 bytesReceived;      // Number of bytes received in this section OFFSET
	int                 offset;             // Current offset from section beginning
	int                 bytesToExpect;      // Number of bytes to expect in this section OFFSET
	unsigned short      uiCRC;              // Calculated CRC16 using NCR's CRC utility
	unsigned long       ulCRC;              // Calculated CRC32 using NCR's CRC utility
	unsigned int		scanLength;         // Length of scanner firmware
	unsigned int		baseLength;         // Length of base firmware
	unsigned int		fileLength;         // The total file length defined in first section
	bool                scanFlashed;        // Signifies that the scanner has been flashed or not
	bool                baseFlashed;        // Signifies that the base has been flashed or not
	int                 flashTimeout;       // Timer that will be decremented every base cycle, and reset every 30-command
	char        baseFirmwareNum[16];        // Base's firmware number
	char        baseSerialNum[16];          // Base's serial number
	char        baseProductName[16];         // Base's assigned P_NAME (2020)
	char        scannerFirmwareNum[16];
	char        scannerSerialNum[16];
	char        scannerProductName[16];     // Scanner's assigned P_NAME (5620)
	char		DIOconfiguration[DIO_CONFIGURATION_LENGTH+1];		// Array of configuration bytes for NCR
	unsigned int		errorCondition;		// Error Conditions
}flash4683_t;

volatile extern flash4683_t     m_flash4683;
volatile extern tfData_t		m_tfData;

void TF_ResetValues(void);

// byte 2 part of system_command
#define NON_FLASH_COMMAND				0x30
#define FLASH_COMMAND                   0x1D
#define FLASH_DATA                      0x1B
#define IBM_DIO_RESERVED				0xFF

// IBM reserved commands:
#define IBM_DIO_CONFIG_SYM				0xFF
#define IBM_DIO_DEV_INFO				0xFE

// Flash scan defines
#define ENTER_FLASH_MODE                0x00
#define REQUEST_PART_NUMBER             0x01
#define REQUEST_PERIPHERAL_ID           0x02
#define SELECT_FLASH_SECTION            0x03
#define GET_CRC_16                      0x04
#define ERASE_SELECTED_SECTION          0x05
#define SELECT_FLASH_OFFSET             0x06
#define UPDATE_DEVICE                   0x07
#define RESET_DEVICE                    0x09
#define GET_CRC_32                      0x0A
#define INCREMENT_FIRMWARE_FLASH_COUNTER 0x0B
#define REQUEST_SCANNER_FIRMWARE_VERSION 0x0C

// Trickle Feed defines
#define ENTER_TRICKLE_FEED				0x01
#define GET_DOWNLOAD_OFFSET				0x02
#define TRICKLE_FEED_COMPLETE			0x04
#define PAUSE_TRICKLE_FEED				0x05
#define TERMINATE_TRICKLE_FEED			0x06
#define ACTIVATE_NEW_FIRMWARE			0x07
#define WALMART_BUFFER					0x1F0000

#define DIO_4683                        0x41
#define DIO_A_SERIES					0x41
#define DIO_F_SERIES                    0x46
#define SPECIAL_4683                    0x46
#define DIO_t_SERIES					0x74
#define CONFIG_GET                      0xA3
#define CONFIG_SET                      0xA2

#define FULL_UNIT_ID                    0x30
#define SEND_CONFIGURATION				0x32
#define REPORT_CONFIGURATION			0x33
#define FW_NEW_VERSION_REPORT           0x34
#define FW_VERSION_REPORT               0x36
#define SERIAL_NUMBER_REPORT            0x35
#define BEEP_4683                       0x32
#define DIO_0H0_COMMAND                 0x30
#define SEND_CONFIGURATION_A			0x41
#define SEND_CONFIGURATION_B			0x42
#define DIO_H_SERIES					0x48

#define DIO_CHAR1	0
#define DIO_CHAR2	1
#define DIO_CHAR3	2
#define DIO_CHAR4	3
#define DIO_CHAR5	4

// For IBM enable/disable commands
#define SYM_0	3
#define SYM_1	4

// For NCR/Walmart direct IO configuration
#define BYTE_A				4
#define BYTE_B				5
#define BYTE_C				6
#define BYTE_D				7
#define BYTE_E				8
#define BYTE_F				9
#define BYTE_G				10
#define BYTE_H				4
#define BYTE_I				5
#define BYTE_J				6
#define BYTE_K				7
#define BYTE_L				8
#define BYTE_M				9
#define I25_LENGTH_MASK		0x3F

#endif

