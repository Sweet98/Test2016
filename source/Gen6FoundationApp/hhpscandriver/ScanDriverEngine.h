/*-------------------------------------------------------------------

 *-------------------------------------------------------------------
 * ScanDriverEngine.h -- Header file associated with engine PSOC controller
 *	which will be part of several -- first 5000 VGA and 5000 MP -- engines.
 *-------------------------------------------------------------------
 * $RCSfile: ScanDriverEngine.h $
 * $Revision: 1.17 $
 * $Date: 2010/12/13 15:11:53EST $
 *-------------------------------------------------------------------*/


#ifndef _SCAN_DRIVER_ENGINE_H_
#define _SCAN_DRIVER_ENGINE_H_

#include "ScanDriverOpenContext.h"

#ifdef BYD_SENSOR
#define HWLAYER_JADE	HWLAYER_BYD
#endif

// PSOC defines
// Memory offset / register list
#define PSOC_OFFSET_ENGINE_ID 0
#define PSOC_SIZE_ENGINE_ID 4

#define PSOC_OFFSET_HHP_SERIAL_NUMBER 4
#define PSOC_OFFSET_CUSTOM_SERIAL_NUMBER  14
#define PSOC_SIZE_SERIAL_NUMBER 12

#define PSOC_OFFSET_AIMER_CENTER_X 26
#define PSOC_OFFSET_AIMER_CENTER_Y 28
#define PSOC_SIZE_AIMER_CENTER 2

#define PSOC_OFFSET_LASER_POWER_SETTING 30
#define PSOC_SIZE_LASER_POWER_SETTING 1

#define PSOC_OFFSET_RED_ILL_MAX_CURRENT 31
#define PSOC_OFFSET_GREEN_ILL_MAX_CURRENT 32
#define PSOC_SIZE_MAX_CURRENT 1

#define PSOC_OFFSET_AIMER_DRIVE_MAX_CURRENT 33
#define PSOC_SIZE_AIMER_DRIVE_MAX_CURRENT 1

#define PSOC_OFFSET_AIMER_CURRENT_OPER_MAX 36
#define PSOC_SIZE_AIMER_CURRENT_OPER_MAX 1

#define PSOC_OFFSET_FIRMWARE_VERSION 38
#define PSOC_SIZE_FIRMWARE_VERSION 12

#define PSOC_OFFSET_LED_CONTROL_MODE_AND_ILLUMINATION_TYPE 50
#define PSOC_SIZE_LED_CONTROL_MODE_AND_ILLUMINATION_TYPE 1

#define PSOC_OFFSET_PWM_BASE_FREQUENCY 51
#define PSOC_SIZE_PWM_BASE_FREQUENCY 1

#define PSOC_OFFSET_RED_ILL_DRIVE_CURRENT 52
#define PSOC_OFFSET_GREEN_ILL_DRIVE_CURRENT 53
#define PSOC_SIZE_DRIVE_CURRENT

#define PSOC_OFFSET_AIMER_CURRENT_OPER 54         //added for testflex
#define PSOC_SIZE_AIMER_CURRENT_OPER_MAX 1        //added for testflex

#define PSOC_OFFSET_PIXEL_CLOCK_FREQUENCY 55
#define PSOC_SIZE_PIXEL_CLOCK_FREQUENCY 1

#define PSOC_ARRAY_SIZE 12
#define PSOC_CONFIG_AREA_SIZE 64

#define PSOC_FIRMWARE_VERSION_MAJOR_OFFSET 56
#define PSOC_FIRMWARE_VERSION_MINOR_OFFSET 57

#define PSOC_OFFSET_REG_CHECKSUM 61      //added for testflex
#define PSOC_SIZE_REG_CHECKSUM 1         //added for testflex

// PSOC Command list
#define PSOC_CMD_FLASH_NEW_FIRMWARE	0x81
#define PSOC_CMD_RESET_ENGINE 0x82
#define PSOC_CMD_WRITE_RAM_TO_EE 0x83  //3
//#define PSOC_CMD_SELECT_RED_ILLUMINATION 0x84
//#define PSOC_CMD_SELECT_GREEN_ILLUMINATION 0x85
//#define PSOC_CMD_LED_CONTROL 0x86
#define PSOC_CMD_UPDATE_EE 0x90

// Bit Masks to Configuration area
#define ILLUMINATION_TYPE_MASK 0x80
#define LED_CONTROL_MODE_MASK 0x03


#define PSOC_LED_REG 0x32
#define PSOC_BOOST_ENABLE_REG 0x37

//#if defined(OEM_ENGINE)
#define PSOC_RED_ILLUM_OEM		0x00
//#else
#define PSOC_RED_ILLUM			0x80
//#endif

#define PSOC_RED_OP_MAX_REG 0x22
#define PSOC_GREEN_OP_MAX_REG 0x23
#define PSOC_AIM_OP_MAX_REG 0x24

#define PSOC_RED_CURRENT_REG 0x34
#define PSOC_GREEN_CURRENT_REG 0x35
#define PSOC_AIM_CURRENT_REG 0x36

#define PSOC_FLASH_CURRENT 39


#ifdef SD_NAMESPACE
   namespace SD_NAMESPACE {
#endif

//64 byte memory overlay structure for PSOC registers
typedef struct {
	unsigned int	EngineID;										//0,1,2,3
	unsigned char	HandHeldProductSerialNumber[ 10 ];		//2-13
	unsigned char	CustomSerialNumber[ 12 ];					//14-25
	unsigned short	AimerCenterX;									//26,27
	unsigned short	AimerCenterY;									//28,29
	unsigned char	LaserPowerSetting;							//30
	unsigned char PWMPulseWidth;                                                 //31
	unsigned char BoostTargetVoltage;                                          //32
	unsigned char BoostVoltageOffset;				//33
	unsigned char TBD1[ 3 ];                                        //34,35,36
	unsigned char FocusStationID;												//37 (0x25) Open 1
	unsigned short	FirmwareChecksum;										//38,39
	unsigned char  LaserPowerSettingBackup;                                                            //40
	unsigned char TBD2;						//4//41
	unsigned char	LicenseKeys[ 8 ];                     //42,49
	unsigned char   LEDControl;                           //50                                                                   
	unsigned char	PWMBaseFrequency;								//51
	unsigned char TBD3[ 4 ];                                                                     //52,55
	unsigned char 	MajorRevision;									//56
	unsigned char 	MinorRevision;									//57
	unsigned char MfgTestStatus;                                                                   //58
	unsigned char 	TBD4[2];											//59,60

// Simple sum of sum checksum to this address non inclusive
	unsigned char	RegisterChecksum;								//61

	unsigned char 	InvokeDefault;					//62
	unsigned char 	TBD5;				//63
} HHPScanDriverPsocRegisterTable_t;


extern volatile BOOL bPsocPresent;
extern volatile HHPScanDriverPsocRegisterTable_t *gp_PsocRegisters;
extern volatile BOOL bPsocDiagnosticMode;

struct DefaultEngineConfig : public ScanEngineConfig_t
{
	DefaultEngineConfig()
		{
			nEngineId            = gp_PsocRegisters->EngineID;
//	nPixelClockFrequency = gp_PsocRegisters->PixelClockFrequency;
		//	nIlluminationType    = (DWORD)gp_PsocRegisters->LEDControl
		//		                     & ILLUMINATION_TYPE_MASK;
			nLedControlMode = (DWORD)gp_PsocRegisters->LEDControl
				                     & LED_CONTROL_MODE_MASK;
			nAimerCenterX        = gp_PsocRegisters->AimerCenterX;
			nAimerCenterY        = gp_PsocRegisters->AimerCenterY;
			nEngineFwCksum       = gp_PsocRegisters->FirmwareChecksum;
			nEngineFwVersion     = DWORD( (gp_PsocRegisters->MajorRevision<<8)
										   | gp_PsocRegisters->MinorRevision );
		}
};

// Base prototypes
BOOL PsocReadConfigArea (void );
void PsocDumpConfigArea(void);
BOOL PsocWriteBytes(BYTE *, BYTE);
BOOL PsocReadBytes(BYTE *, BYTE);
BOOL PsocWriteRegisters(DWORD, const BYTE*, BYTE);       //change from: (DWORD, BYTE*, BYTE);  before 4600r/test flex stuff added
BOOL PsocReadRegisters(DWORD, BYTE*, BYTE);
BOOL PsocUpdateRegisters(DWORD RegOffset, const BYTE * pData, BYTE DataLength);     //added for 4600r/testflex
BOOL PsocSetupDiagnosticMode(BOOL);
void PsocPreCommunication(void);

//#if defined(OEM_ENGINE)
BOOL SpreadReadRegisters (DWORD offset, BYTE * data, BYTE cnt);
BOOL SpreadWriteRegisters (DWORD offset, const BYTE * data, BYTE cnt );
//#endif

//Internal (real time Scan Driver) calling functions
BOOL PsocInternalWriteRegisters(BYTE, BYTE*, BYTE);
BOOL PsocInternalReadRegisters(BYTE, BYTE*, BYTE);
BOOL PsocInternalWriteBytes(BYTE *, BYTE);
BOOL PsocInternalReadBytes(BYTE *, BYTE);

BOOL ReportImagerProperties (const DWORD *, DWORD *, DWORD);

BOOL EngineReset(void);

//Sensor I/O functions
//BOOL SensorInternalReadRegisters(int,BYTE , void*, int);
//BOOL SensorInternalWriteRegisters(int,BYTE , void*, int);


// FIXME: these actually should use stdint
#ifndef BIG_ENDIAN
#define UINT_TO_BE(v)       (((unsigned long)(v) >> 24)| ((unsigned long)(v) >> 8 & 0x0000ff00)	| ((unsigned long)(v) << 8 & 0x00ff0000)| ((unsigned long)(v) << 24))
#define USHORT_TO_BE(v)  (((unsigned short)(v) >> 8)| ((unsigned short)(v) << 8))
#define ULONG_TO_BE(v)   (((unsigned long)(v) >> 24)| ((unsigned long)(v) >> 8 & 0x0000ff00)	| ((unsigned long)(v) << 8 & 0x00ff0000)| ((unsigned long)(v) << 24))
#else
#define USHORT_TO_BE(v)	((unsigned short)(v))
#define UINT_TO_BE(v)  ((unsigned int)(v))
#define ULONG_TO_BE(v)	((unsigned long)(v))
#endif // BIG_ENDIAN

#ifdef SD_NAMESPACE
   } // namespace SD_NAMESPACE {
#endif

#endif
