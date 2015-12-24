/************************************************************************/
//
// HHP 5100 embedded firmware
//
//  5100 PSoC flashing firmware specification
//  Date:       2/17/04
//  Tools:      CodeWarrior for ARM 1.2
//  Target:     MX1
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/PSOC/ScanEnginePSOC.h $
// $Revision: 1.7 $
// $Date: 2011/08/25 01:34:21EDT $
//======================================================================================
/************************************************************************/
/*|=======================================================================
  |			MODIFICATION HISTORY
  |=======================================================================
*************************************************************************/


#ifndef PSOC_5100
#define PSOC_5100

#define DATA_RECORD 00
#define END_OF_FILE 01

#define PSOC_PROG_START				0x0040
#define PSOC_PROG_STOP				0x0A40
#define PSOC_CHECSUM_ADDR			0x0FC0

#define MPx_PSOC_PROG_START		0x0000	         //ISSP additions
#define MPx_PSOC_PROG_STOP			0x0F7E            //ISSP additions
#define MPx_PSOC_CHECSUM_ADDR		0x0F40            //ISSP additions

//-------------------------------
//PSOC Defines
//-------------------------------
#define PSOC_I2C_ADDRESS		 	0x80
#define PSOC_RECORD_SIZE			0x40
#define PSOC_VERSTR_ADDR		   0x29
#define PSOC_VERSTR_SIZE		   0x04
#define PSOC_BOOT_TRIES				0x05
#define PSOC_CMD_SIZE				10
#define PSOC_CMD_DELAY				150
#define PSOC_CMD_DELAY_INC			40
#define PSOC_CMD_DELAY_RETRY		3
#define PSOC_BOOT_COMMAND_DELAY     50
#define PSOC_ACK_CNT					10
#define PSOC_CHKSUM_B1				62
#define PSOC_CHKSUM_B2				63


//-------------------------------
//PSOC Error Codes
//-------------------------------
#define PSOC_BCOK				0x01
#define PSOC_IVERR			0x02
#define PSOC_FCERR			0x04
#define PSOC_FPERR			0x08
#define PSOC_CCERR			0x10
#define PSOC_BM				0x20
#define PSOC_IVKERR			0x40
#define PSOC_IVCERR			0x80

//-------------------------------
//PSOC Commands
//-------------------------------
#define PSOC_BOOT_MODE			0x81
#define PSOC_ENTER_BOOT			0x38
#define PSOC_FLASH_WRITE		0x39
#define PSOC_FLASH_VERIFY		0x3A
#define PSOC_VERIFY_EXIT		0x3B



//-------------------------------
//PSOC Boot Authentication Keys
//-------------------------------
#define KEY1					0xAA
#define KEY2					0x55
#define KEY3					0x33
#define KEY4					0x68
#define KEY5					0x98
#define KEY6					0x0B
#define KEY7					0x1D
#define KEY8					0xAC

//This structure is used to hold HEX file info
typedef struct
{
   UCHAR	   nRecordColon;
   UCHAR	   nRecordSize;
   USHORT	nRecordAddr;
   UCHAR    nRecordType;
   UCHAR	   nRecordChecksum;
   UCHAR	   nRecordData[PSOC_RECORD_SIZE];
} PSoC_5100_HEX_RECORD;



// Register definitions
#define		PSoC_5100_REGISTER_TABLE_TBD_1_SIZE					3
#define		PSoC_5100_REGISTER_TABLE_TBD_2_SIZE					1
#define		PSoC_5100_REGISTER_TABLE_TBD_3_SIZE					4
#define		PSoc_5100_REGISTER_TABLE_TBD_4_SIZE					2
#define		PSoc_5100_REGISTER_TABLE_TBD_5_SIZE					1
//**** Alignment note--> The struct below (and a very similar -- somewhat redundant one -- in the Scan Driver) which attempts to reliably map the 64 byte
//**** PSOC configuration memory is less than ideal.  Both structures rely on assumptions for correctly mapping this memory block -- shorts align on even
//**** boundaries naturally, etc.  This comment is put here for understanding / warming while alternatives are considered to not depend on these
//**** non obvious assumptions.


//	Engine ID definitions.
#define		PSOC_5100_FOCUS_HD									0
#define		PSOC_5100_FOCUS_SF									1
#define		PSOC_5100_FOCUS_SR									2

#define		PSOC_5100_SECONDARY_ILLUMINATION_NONE				0
#define		PSOC_5100_SECONDARY_ILLUMINATION_RED				1
#define		PSOC_5100_SECONDARY_ILLUMINATION_GREEN				2
#define		PSOC_5100_SECONDARY_ILLUMINATION_WHITE				3

#define		PSOC_5100_PRIMARY_ILLUMINATION_NONE					0
#define		PSOC_5100_PRIMARY_ILLUMINATION_GREEN				1
#define		PSOC_5100_PRIMARY_ILLUMINATION_RED					2
#define		PSOC_5100_PRIMARY_ILLUMINATION_WHITE				3

#define		PSOC_5100_SPEED_24									0
#define		PSOC_5100_SPEED_26									1

#define		PSOC_5100_AIMER_LED									0
#define		PSOC_5100_AIMER_BRIGHT_LED							1
#define		PSOC_5100_AIMER_LASER								2

#define		PSOC_5100_AIMER_IMAGER_MICRON_MT9V022_BW			0
#define		PSOC_5100_AIMER_IMAGER_MICRON_MT9V022_COLOR			1
#define		PSOC_5100_AIMER_IMAGER_STMICRO_602_BW				2
#define		PSOC_5100_AIMER_IMAGER_STMICRO_602_COLOR			3

#define		PSOC_5600_AIMER_IMAGER_JADE_BW						0
#define		PSOC_5600_AIMER_IMAGER_JADE_MONOCOLOR				1

// Specific Engine IDs
#define ENGINE_ID_5100_LED_OFF_AXIS_LIGHT_BAR_SF	0x0201
#define ENGINE_ID_5100_LED_OFF_AXIS_LIGHT_BAR_SR	0x0202
#define ENGINE_ID_5100_LED_OFF_AXIS_LIGHT_BAR_HD	0x0200
#define ENGINE_ID_JADE_FLASH_LED                                      0x4541


//Validate Revision returns
#define 	REVS_DONT_MATCH		0
#define	PASSED_VALIDATE		1
#define	PSOC_NOT_TALKATIVE	2


typedef struct {
	union {
		struct {
			unsigned int	Focus:4;
			unsigned int	LensFilter:2;				// 00-no filter 01 lens filter
			unsigned int	Bank2Illumination:2;
			unsigned int	Bank1Illumination:2;
			unsigned int	Aimer:3;
			unsigned int	Imager:3;
			unsigned int    SpreadSpectrum:2;
			unsigned int    OnBoardMCU:3;
			unsigned int    IOVoltage:2;
			unsigned int    OpticsFocusType:2;
			unsigned int	Reserved:6;
			unsigned int    EngineFormat:1;
		} b;
		unsigned int	s;
	} EngineID;

//	unsigned char	FocusStationID;
//	unsigned char	ReservedForManufacturing;
	unsigned char	SweetcornSerialNumber[ 10 ];
	unsigned char	CustomSerialNumber[ 12 ];
	unsigned short	AimerXCenter;
	unsigned short	AimerYCenter;
	unsigned char	LaserPowerSetting;
	unsigned char PWMPulseWidth;
	unsigned char BoostTargetVoltage;
	unsigned char BoostVoltageOffset;
	unsigned char TBD1[ PSoC_5100_REGISTER_TABLE_TBD_1_SIZE ];
	unsigned char FocusStationID;	
//	unsigned char	PrimaryIlluminationDriveCurrentAbsoluteMaximum;
//	unsigned char	SecondaryIlluminationDriveCurrentAbsoluteMaximum;
//	unsigned char	AimerDriveCurrentAbsoluteMaximum;
//	unsigned char	PrimaryIlluminationDriveCurrentOperationalMaximum;
//	unsigned char	SecondaryIlluminationDriveCurrentOperationalMaximum;
//	unsigned char	AimerDriveCurrentOperationalMaximum;
//	unsigned char	TBD1[ PSoC_5100_REGISTER_TABLE_TBD_1_SIZE ];
	unsigned short	Checksum;
	unsigned char  LaserPowerSettingBackup;
	unsigned char TBD2[ PSoC_5100_REGISTER_TABLE_TBD_2_SIZE ];
	unsigned char	LicenseKeys[ 8 ];

	union {
		struct {
			unsigned char	Mode:2;
			unsigned char Current:3;
			unsigned char reserved:1;
			unsigned char	LEDBank:2;
		} b;
		unsigned char	c;
	} LEDControl;

	unsigned char	PWMBaseFrequency;
	unsigned char TBD3[ PSoC_5100_REGISTER_TABLE_TBD_3_SIZE ];
	//unsigned char	PrimaryIlluminationDriveCurrent;
	//unsigned char	SecondaryIlluminationDriveCurrent;
	//unsigned char	AimerDriveCurrent;
	//unsigned char	PixelClockFrequency;
	unsigned char	FirmwareType;
	unsigned char	FirmwareRevision;
	unsigned char MfgTestStatus;
	unsigned char	TBD4[ PSoc_5100_REGISTER_TABLE_TBD_4_SIZE ];

// This next value represents a simple checksum of every byte in the register table to this point.  Earlier (pre 1.20 I believe) version of
// PSOC do not support this.  However, in earlier versions, TBD3 should be filled with zero characters, so we can use that to verify that a
// a register read worked (no I2C corruption).
	unsigned char	RegisterChecksum;
	unsigned char InvokeDefault;
	unsigned char	TBD5[ PSoc_5100_REGISTER_TABLE_TBD_5_SIZE ];
} PSoC_5100_REGISTER_TABLE;



typedef enum {
	PSOC_5100_SECONDARY_ILUMINATION = 0,
	PSOC_5100_PRIMARY_ILUMINATION,
	END_PSOC_5100_ILLUMINATION_COLORS
} PSOC_5100_ILLUMINATION_COLORS;


typedef enum {
	PSOC_5100_ILLUMINATION_CONTROL_BY_HOST__AIMER_CONTROL_BY_HOST = 0,
	PSOC_5100_ILLUMINATION_CONTROL_BY_HOST__AIMER_CONTROL_BY_SENSOR,
	PSOC_5100_ILLUMINATION_CONTROL_BY_BOTH__AIMER_CONTROL_BY_HOST,
	PSOC_5100_ILLUMINATION_CONTROL_BY_BOTH__AIMER_CONTROL_BY_BOTH,
	PSOC_5100_ILLUMINATION_CONTROL_EXTERNAL__AIMER_CONTROL_EXTERNAL,
	END_PSOC_5100_LED_CONTROL_MODES
} PSOC_5100_LED_CONTROL_MODES;


typedef enum {
	PSOC_5100_PIXEL_CLOCK_FREQUENCY_12_MHZ = 0,
	PSOC_5100_PIXEL_CLOCK_FREQUENCY_24_MHZ = 1,
	END_PSOC_5100_PIXEL_CLOCK_FREQUENCIES
} PSOC_5100_PIXEL_CLOCK_FREQUENCIES;


//  Above is Lower Cfg Block in structure overlay form -- PSOC cfg offsets 0x0 through 0x3F
//  5000 VGA has accessible but does not use Upper Cfg Block -- PSOC cfg offsets 0x40 through 0x7f
//	MPx has accessible and is going to use this Upper Cfg Block.
//	As extent / appraoch is not 'fully defined' , for now unique defines are being used here instead of typedef'ing the entire block
//  May be temp approach...
#define ROW_NOISE_TOP_CORRECTION_FACTOR_OFFSET		0x40
#define ROW_NOISE_BOTTOM_CORRECTION_FACTOR_OFFSET	0x41


//Function Headers
bool	ImageEngineHasPsoc( void );
void	Psoc5100PowerUpForRegisterProcessing( void );
void	Psoc5100PowerDownAfterRegisterProcessing( void );
UCHAR    Psoc5100ProgramBlock (USHORT nHexRecordAddress, unsigned int ExtraPsocCmdDelay);
UCHAR    Psoc5100SendCommand(UCHAR nCommandByte, bool b_GetResponse, unsigned int ExtraPsocCmdDelay);
bool	GetEnginePSOCRevision( unsigned int *FirmwareType, unsigned int *FirmwareRevision );
bool     Psoc5100FlashNewFirmware( bool OverwriteEvenIfEnginePSOCCodeIsHigherRevision, bool OverwriteEvenIfFirmwareTypesDoNotMatch, bool OverRideOldBootLoadSafety );
void     Psoc5100DelayInMilliSec(int nNumMilliseconds);
bool     Psoc5100GetHexData(const UCHAR *nHexFile, USHORT nRecordAddress, PSoC_5100_HEX_RECORD *theRecord);
bool     Psoc5100EnterBootMode(unsigned int ExtraPsocCmdDelay);
bool PsocCompareFirmwareVersion(void);
bool     Psoc5100VerifyFirmwareVersion( bool ForceUpdate = false );
void     Psoc5100SerialDownloadFirmware( void );
bool     Psoc5100CalculateDnldChecksum(USHORT usStartAddr, USHORT usStopAddr);
bool	 Psoc5100LoadFirmwareData( unsigned char *PSOCFirmware, int SizeOfPSOCFirmware, bool OverwriteEvenIfEnginePSOCCodeIsHigherRevision,
						bool OverwriteEvenIfFirmwareTypesDoNotMatch );
bool	Psoc5100Read( int Address, char *Data, int AmountOfDataToRead );
bool 	Psoc5100SaveToEEprom(void);
bool 	Psoc5100Write( int Address, unsigned char *Data, int LengthOfData);
bool	Psoc5100Write( const char *Data, int LengthOfData );
bool	Psoc5100Write( int Data );
bool	Psoc5100GetRegisters( PSoC_5100_REGISTER_TABLE *Registers );
bool	Psoc5100SetIlluminationColor( int Color, bool SetPermanently = true );
bool	Psoc5100RegisterAddressRangeIsUnprotected( unsigned int StartAddress, unsigned int SizeOfAddressRange, bool *RangeError = 0 );

bool  PsocEngineLoopTest(unsigned int diagVal);

bool GetRowNoiseCorrectionValues(signed char *pTop, signed char *pBot);

#endif
//----------------------------------------------------------------------------





