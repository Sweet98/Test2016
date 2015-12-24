//======================================================================================
// ImagingMenuSettings.cpp
//======================================================================================
// $RCSfile: ImagingMenuSettings.cpp $
// $Revision: 1.97 $
// $Date: 2011/11/11 14:49:21EST $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Module Description:
//      This file contains a routine that creates imaging menu settings as part
//		of the overall reader configuration for a Matrix device.
//
//---------------------------------------------------------------------------


#include 	"stdInclude.h"
#include	"IOMan.h"
#include	"Menu.h"
#include	"ProductConfiguration.h"
#include	"ReaderConfiguration.h"
#include 	"Hi704A.h"
#include	"ImagingMenuSettings.h"			// ImagingMenuSettings interface definition
#include 	"beep.h"
#include 	"PluginGpioInterface.h"
#include    "Flash_Illumination_Control.h"

//#include	"vars.h"
//#include	"Hal.h"
//#include	"Trigger.h"


#ifndef RFBASE
#include	"HexFile.h"
#include	"ScanEnginePSOC.h"                          //lmc   fixme
#include "startupFlashdisk.h"
#include "db_hdw.h"
#include "HardwareMacros.h"
#include "iMX25_GPIO.h"	//GPIO Defines
#include "iMX25_IOMUX.h"	//IO MUX Defines
#include 	"thread.h"
#include 	"interrupts.h"
//#include	"scandriverinterface.h"		//replaced with "scandrivermatrixinterface.h"
#else
#endif

#include    "scandrivermatrixinterface.h"
#include "HardwareToken.h"
#ifdef DEVELOPMENT_SETTINGS
#include "db_heap.h"
#include "imaging.h"
#endif


#ifndef	TRACE0
#define	TRACE0( a )
#endif
#ifndef	TRACE1
#define	TRACE1( a, b )
#endif
#ifndef	TRACE2
#define	TRACE2( a, b, c )
#endif
/*																	  //to debug - remove comment out
#include	"db_Hdw.h"
#include	"IoMan.h"
#include "db_chrio.h"        // Character I/O header prototypes
#undef	TRACE0
#define	TRACE0( a ) theStdInterface.printf( a );  wait_til_tx_is_empty()
#undef	TRACE1
#define	TRACE1( a, b ) theStdInterface.printf( a, b );  wait_til_tx_is_empty()
#undef	TRACE2
#define	TRACE2( a, b, c ) theStdInterface.printf( a, b, c );  wait_til_tx_is_empty()
*/																		//to debug - remove comment out


bool bUpdateIMGparameters = false;
#ifndef RFBASE
#define CLEAR_FLASHOUT_INT_Signal()		do{WRITE_FIELD_TO_REG(GPIO3_ISR,GPIO_BIT23_LSH,GPIO_BIT23_WID,0x1);}while(0)			//Clears the Interrupt Status Register Flag for the FLASH_OUT Signal			
#define READ_FLASHOUT_INT_Signal()		(READ_FIELD(GPIO3_ISR,GPIO_BIT23_LSH,GPIO_BIT23_WID))									//Reads the Interrupt Status Register Flag for the FLASH_OUT Signal 
#define FLASHOUT_INT_CONFIG(state)		do{WRITE_FIELD_TO_REG(GPIO3_ICR2,GPIO_ICR23_LSH,GPIO_ICR23_WID,state);}while(0)			//Sets the Interrupt Configuration Field for the FLASH_OUT Signal
#define READ_FLASHOUT_INT_CONFIG()		(READ_FIELD(GPIO3_ICR2,GPIO_ICR23_LSH,GPIO_ICR23_WID))									//Reads the Interrupt Control Field to determin rising or falling edge
#define FLASHOUT_INT_CONTROL(state)		do{WRITE_FIELD_TO_REG(GPIO3_IMR,GPIO_BIT23_LSH,GPIO_BIT23_WID,state);}while(0)			//Enable or Disables the FLASH_OUT interrupt bit
#define READ_FLASHOUT_INT_CONTROL()		(READ_FIELD(GPIO3_IMR,GPIO_BIT23_LSH,GPIO_BIT23_WID))
#define FLASHOUT_PINSTATUS()			(READ_FIELD(GPIO3_PSR0,GPIO_BIT23_LSH,GPIO_BIT23_WID))
#endif
// Routines which route diagnostic output to the correct interface (from PlugAndPlayMenuSettings.cpp).
extern void SendStringToInterface(const char *pucBuffer, int SizeOfBuffer );

void SendStringToInterface( const char *pucBuffer, int SizeOfBuffer, bool /* DoNotUseIndicators */ );
extern void LittleEndien4bytes(unsigned int x, unsigned char *ptr);
extern BOOL SpreadReadRegisters (DWORD offset, BYTE * data, BYTE cnt);
extern BOOL SpreadWriteRegisters (DWORD offset, const BYTE * data, BYTE cnt );
extern void UpdatePowerDownPin(void);
extern int PSOCBlankdownload;

//UpdatePowerDownPin
#ifndef RFBASE
void UpdatePowerDownPin(void) 
{
	bool	bExtFlo = GET_SETTING(EnConnecttoExt);
	if( bExtFlo )
	{
		bool FlashOutState = FLASHOUT_PINSTATUS();	
		if(FlashOutState)
		{
			HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT31_LSH,GPIO_BIT31_WID,GPIO_LOW);	//set data reg to LOW
		}
		else
		{	
			HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT31_LSH,GPIO_BIT31_WID,GPIO_HIGH);	//set data reg to High
		}
	}
}
#endif // endof #ifndef RFBASE
//
//
// CheckIMGVGAMode
//    Check the new IMGVGA mode against the existing mode.  If they're the same, set bUpdateIMGparameters to true so we don't reset the
//    IMGxxx values when we finish processing this command.
//
bool CheckIMGVGAMode( void )
{
	bool		Status = true;	// Assume success until we know otherwise

   if( Status )
	{
	// Convert the existing value to a string and see if it is changing.
		char	temp[ 16 ];

		sprintf( temp, "%d", GET_SETTING( ShipImagesIn640x480VGAMode ) );

		if(( strlen( temp ) == ReaderConfiguration->MenuCommandResponse->Size ) &&
            ( memcmp( temp, ReaderConfiguration->MenuCommandResponse->Char, ReaderConfiguration->MenuCommandResponse->Size ) == 0 ))
		{
         bUpdateIMGparameters = false;
		}
		else
		{
         bUpdateIMGparameters = true;
		}
	}

	return Status;
} // CheckIMGVGAMode


//
//	UpdateImageShipParameters
//		Update image ship parameters based on imager size.
//
bool UpdateImageShipParameters( void )
{
#ifndef RFBASE
	TRACE0( " Attempting to update image ship parameters \r\n" );

	/* Get from engine ID */
	if(HardwareToken.IAmOEMJade()) 
	{
		PSoC_5100_REGISTER_TABLE Registers;
		
		if(Psoc5100GetRegisters( &Registers ))
		{
			if(PSOC_5600_AIMER_IMAGER_JADE_MONOCOLOR == Registers.EngineID.b.Imager)
			{
				ReaderConfiguration->ChangeNumericList("04082A",TRANSMIT_IMAGE_FORMAT_KIM, TRANSMIT_IMAGE_FORMAT_TIF_BINARY, TRANSMIT_IMAGE_FORMAT_TIF_GRP,
							TRANSMIT_IMAGE_FORMAT_TIF_GRAYSCALE, TRANSMIT_IMAGE_FORMAT_BINARY, TRANSMIT_IMAGE_FORMAT_GRAYSCALE,
							TRANSMIT_IMAGE_FORMAT_JPEG, TRANSMIT_IMAGE_FORMAT_OUTLINED_IMAGE, TRANSMIT_IMAGE_FORMAT_BMP, TRANSMIT_IMAGE_FORMAT_LOSSLESS_HUFFMAN,
			 				TRANSMIT_IMAGE_FORMAT_TIF_COLOR_COMPRESSED, TRANSMIT_IMAGE_FORMAT_TIF_COLOR_UNCOMPRESSED,
							TRANSMIT_IMAGE_FORMAT_JPEG_COLOR, TRANSMIT_IMAGE_FORMAT_BMP_COLOR, TRANSMIT_IMAGE_FORMAT_UNCOMPRESSED_RAW);
			}

		}
		else
		{
			if(GET_SETTING(GeneralDebugOn))
			{
				TRACE0("Failed to read PSOC regs\r\n");
			}
		}
	}
	else if((GET_CONFIG_SETTING(ImageSensor) == E2V_EV76C454_Jade_Monocolor) && (GET_CONFIG_SETTING(bColorFusion)))
	{
		ReaderConfiguration->ChangeNumericList("04082A",TRANSMIT_IMAGE_FORMAT_KIM, TRANSMIT_IMAGE_FORMAT_TIF_BINARY, TRANSMIT_IMAGE_FORMAT_TIF_GRP,
							TRANSMIT_IMAGE_FORMAT_TIF_GRAYSCALE, TRANSMIT_IMAGE_FORMAT_BINARY, TRANSMIT_IMAGE_FORMAT_GRAYSCALE,
							TRANSMIT_IMAGE_FORMAT_JPEG, TRANSMIT_IMAGE_FORMAT_OUTLINED_IMAGE, TRANSMIT_IMAGE_FORMAT_BMP, TRANSMIT_IMAGE_FORMAT_LOSSLESS_HUFFMAN,
			 				TRANSMIT_IMAGE_FORMAT_TIF_COLOR_COMPRESSED, TRANSMIT_IMAGE_FORMAT_TIF_COLOR_UNCOMPRESSED,
							TRANSMIT_IMAGE_FORMAT_JPEG_COLOR, TRANSMIT_IMAGE_FORMAT_BMP_COLOR, TRANSMIT_IMAGE_FORMAT_UNCOMPRESSED_RAW);
	}


	if( bUpdateIMGparameters &&  !ReaderConfiguration->FoundValidWorkingConfigurationInFlash)
	{
     	bUpdateIMGparameters = false;
	}

// Adjust image ship parameters intellegently per scan driver loaded
	DWORD nHeight,nWidth;

	if( ReaderConfiguration->ImagerIsRunning )
	{
		if( SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&nHeight,4,NULL) &&
					SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&nWidth,4,NULL) )
		{

		// Adjust height and width back to VGA 640 x 480 if IMGVGA is enabled.
			if( GET_SETTING( ShipImagesIn640x480VGAMode ) )
			{
				if( nHeight > 480 )
					nHeight = 480;
				if( nWidth > 640 )
					nWidth = 640;
			}

		// New margin parameter maximum must be equal to one half of the lesser of height and width.  It is then moved in by 2 pixels to ensure at least
		// a 4x4 block of data.
			DWORD	MarginParameterMaximum = ( nWidth > nHeight ) ? (( nHeight / 2 ) - 2) : (( nWidth / 2 ) - 2);

			ReaderConfiguration->ChangeNumericRange("040801",nWidth,nWidth);
			ReaderConfiguration->ChangeNumericRange("040802",nHeight,nHeight);
			ReaderConfiguration->ChangeNumericRange("040803",nWidth-1,nWidth-1);
			ReaderConfiguration->ChangeNumericRange("040804",nHeight-1,nHeight-1);
			ReaderConfiguration->ChangeNumericRange("040823",0,nWidth-1,&(ReaderConfiguration->TransmitImageWindowRight));
			ReaderConfiguration->ChangeNumericRange("040824",0,&(ReaderConfiguration->TransmitImageWindowLeft),ReaderConfiguration->ImgRowNumberOfPixelsMinus1/*ABSOLUTE_IMAGE_MAX_COL-1*/);
			ReaderConfiguration->ChangeNumericRange("040825",0,&(ReaderConfiguration->TransmitImageWindowTop), ReaderConfiguration->ImgColNumberOfPixelsMinus1/*ABSOLUTE_IMAGE_MAX_ROW-1*/);
			ReaderConfiguration->ChangeNumericRange("040826",0,nHeight-1,&(ReaderConfiguration->TransmitImageWindowBottom));
			ReaderConfiguration->ChangeNumericRange("04082B",0, MarginParameterMaximum);

			ReaderConfiguration->ChangeDefault( "040824", (int) ( nWidth - 1 ), false, bUpdateIMGparameters );
			ReaderConfiguration->ChangeDefault( "040825", (int) ( nHeight - 1 ), false, bUpdateIMGparameters );
			ReaderConfiguration->ChangeDefault( "040823", 0, false, bUpdateIMGparameters );
			ReaderConfiguration->ChangeDefault( "040826", 0, false, bUpdateIMGparameters );
			ReaderConfiguration->ChangeDefault( "04082B", 0, false, bUpdateIMGparameters );
		}
	}
#endif

	return true;
} // UpdateImageShipParameters






//
//	ProcessEngineIlluminationColorChange
//		Change engine illumination color according to the parameter sent by user.
//
bool ProcessEngineIlluminationColorChange(void)
{
#ifndef RFBASE
	DWORD tags[2]={LI_ILLLUMINATION_SOURCE,HHPSD_IS_ALTERNATE};
	if(0)//( GET_SETTING( EngineIlluminationColor ) == PSOC_5100_PRIMARY_ILUMINATION )                //lmc   fixme
		tags[1]= HHPSD_IS_PRIMARY;
	if (nOpenCntxt)
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_LIGHT_SETTINGS,PBYTE(tags),(DWORD)(sizeof(tags)),0, 0, 0  );

#endif

	return true;
} // ProcessEngineIlluminationColorChange





//
//	ProcessEngineFirmwareDiagnosticModeChange
//		Turn diagnostic mode on or off in scanner depending on ENGDIA setting.
//
bool ProcessEngineFirmwareDiagnosticModeChange(void)
{
	BOOL bStatus=false;
#ifndef RFBASE
	if( ReaderConfiguration->ImagerIsRunning )
	{
		DWORD nDiagMode = (DWORD)(GET_SETTING(EngineFirmwareDiagnosticMode));
		bStatus = SCN_IOControl(nOpenCntxt, HHPSD_IOCTL_ENGINE_DIAGNOSTIC_MODE, (PBYTE)&nDiagMode, sizeof(DWORD), 0, 0, 0);
	}
#endif

	return (bStatus ? true:false);
} // ProcessEngineFirmwareDiagnosticModeChange









#ifndef RFBASE	//Not Rf Base ***********************************************************************************************

//
//	WriteEngineFirmwareFile
//		Check to see whether the engine firmware name included in the ENGFWN command is valid scan engine firmware.  If it is, try to store it.
//
bool WriteEngineFirmwareFile(void)
{
	bool		Status = false;
	
// We will not allow a NULL string.
	if( ReaderConfiguration->ExtraneousTextAfterCommand->Size == 0 )
	{
		Status = false;
	}

// Get the index of the file in the flash disk.
	else if((HardwareToken.IAmOEMJade())||(GET_CONFIG_SETTING(AimerType) == AIMER_PSOC_LASER))  
	{
		unsigned char* pContents = NULL;
		HSTRING FileName(ReaderConfiguration->ExtraneousTextAfterCommand,true);
		FileName.UnprintableFormatsToCharacters();
		int size = ReadAndCloseFile((char*) FileName.Char,0,&pContents);
    		if (size>0)
		{
			//if( Psoc5100LoadFirmwareData( FirmwareFile.Char, FirmwareFile.Size, true, false ) )
			if(Psoc5100LoadFirmwareData(pContents,size,true,false))
			{
				Status = true;
			}
			else
			{
				Status = false;
			}
		}
		else
		{
			Status = false;
		}
		if(pContents)
		{
			free(pContents);
		}
	}
	else
	{
		Status = false;
	}
	return Status;
} // WriteEngineFirmwareFile





//
//	WriteDefaultEngineFirmware
//		Write the default engine firmware to the 5000.
//
bool WriteDefaultEngineFirmware(void)
{
	if (!(HardwareToken.IAmOEMJade()||(GET_CONFIG_SETTING(AimerType) == AIMER_PSOC_LASER)||PSOCBlankdownload))  
		return 0;
	else
		return Psoc5100VerifyFirmwareVersion( true );                  //lmc   fixme
} // WriteDefaultEngineFirmware



//
//	ShowEngineFirmwareChecksum
//		Display the engine firmware checksum.
//
bool ShowEngineFirmwareChecksum(void)
{
	bool Status;
	if(HardwareToken.IAmOEMJade()||(GET_CONFIG_SETTING(AimerType) == AIMER_PSOC_LASER))  
	{
		PSoC_5100_REGISTER_TABLE	Registers;                                          //lmc   fixme
		Status = Psoc5100GetRegisters( &Registers );                          //lmc   fixme

		if( Status )
		{
			char	temp[ 80 ];                                                           //lmc   fixme
			sprintf( temp, "%08x", (int) Registers.Checksum );                            //lmc   fixme
			ReaderConfiguration->MenuCommandResponse->Append( temp );                    //lmc   fixme
		}
	}
	else
		Status = 0;
	return Status;
} // ShowEngineFirmwareChecksum



//
//	ShowEngineFirmwareFullReport
//		Display the engine firmware register table contents.
//
bool ShowEngineFirmwareFullReport(void)
{
	bool	Status;
	if(HardwareToken.IAmOEMJade()||(GET_CONFIG_SETTING(AimerType) == AIMER_PSOC_LASER))  
	{
		PSoC_5100_REGISTER_TABLE	Registers;                                   //lmc   fixme
		Status = Psoc5100GetRegisters( &Registers );                      //lmc   fixme

		if( Status )
		{                                                                        //lmc   fixme
			HSTRING		Temp( 20, true );
			HSTRING		OutputString( 20, true );
			char		cTemp[ 160 ];
			sprintf( cTemp, "Engine firmware configuration (table size %d):\r\n", sizeof( PSoC_5100_REGISTER_TABLE ) );
			OutputString.Append( cTemp );
			sprintf( cTemp, "    Engine ID:                                      %X\r\n", (int) Registers.EngineID.s );
			OutputString.Append( cTemp );
		//	sprintf( cTemp, "        Focus:                                      %X\r\n", (int) Registers.EngineID.b.Focus );
		//	OutputString.Append( cTemp );
		//	sprintf( cTemp, "        Bank3 Illumination:                     %X\r\n", (int) Registers.EngineID.b.Bank3Illumination );
		//	OutputString.Append( cTemp );
		//	sprintf( cTemp, "        Bank2 Illumination:                       %X\r\n", (int) Registers.EngineID.b.Bank2Illumination );
		//	OutputString.Append( cTemp );
		//	sprintf( cTemp, "        Bank1 Illumination:                                      %X\r\n", (int) Registers.EngineID.b.Bank1Illumination );
		//	OutputString.Append( cTemp );
		//	sprintf( cTemp, "        Aimer:                                      %X\r\n", (int) Registers.EngineID.b.Aimer );
		//	OutputString.Append( cTemp );
		//	sprintf( cTemp, "        Imager:                                     %X\r\n", (int) Registers.EngineID.b.Imager );
		//	OutputString.Append( cTemp );
		//	sprintf( cTemp, "    Spread Spectrum:                               %X\r\n", (int) Registers.EngineID.b.SpreadSpectrum );
		//	OutputString.Append( cTemp );
		//	sprintf( cTemp, "    On Board MCU:                               %X\r\n", (int) Registers.EngineID.b.OnBoardMCU );
		//	OutputString.Append( cTemp );
		//	sprintf( cTemp, "    IO Voltage:                               %X\r\n", (int) Registers.EngineID.b.IOVoltage );
		//	OutputString.Append( cTemp );
		//	sprintf( cTemp, "    Optics Focus Type:                               %X\r\n", (int) Registers.EngineID.b.OpticsFocusType );
		//	OutputString.Append( cTemp );
		//	sprintf( cTemp, "    Engine Format:                               %X\r\n", (int) Registers.EngineID.b.EngineFormat );
		//	OutputString.Append( cTemp );
			Temp.Copy( Registers.SweetcornSerialNumber, sizeof( Registers.SweetcornSerialNumber ) );
			sprintf( cTemp, "    Products Serial #:                    %s\r\n", (char *) Temp.Char );
			OutputString.Append( cTemp );
			Temp.Copy( Registers.CustomSerialNumber, sizeof( Registers.CustomSerialNumber ) );
			sprintf( cTemp, "    Custom Serial #:                                %s\r\n", (char *) Temp.Char );
			OutputString.Append( cTemp );
			sprintf( cTemp, "    Aimer X Center #:                               %d\r\n", (int) Registers.AimerXCenter );
			OutputString.Append( cTemp );
			sprintf( cTemp, "    Aimer Y Center #:                               %d\r\n", (int) Registers.AimerYCenter );
			OutputString.Append( cTemp );
			sprintf( cTemp, "    Laser Power Setting:                            %d\r\n", (int) Registers.LaserPowerSetting );
			OutputString.Append( cTemp );
		//	sprintf( cTemp, "    Primary Illumination Current Absolute Max:      %d\r\n", (int) Registers.PrimaryIlluminationDriveCurrentAbsoluteMaximum );
		//	OutputString.Append( cTemp );
		//	sprintf( cTemp, "    Boost target voltage:      %d\r\n", (int) Registers.BoostTargetVoltage);
		//	OutputString.Append(cTemp);
		//	sprintf( cTemp, "    Secondary Illumination Current Absolute Max:    %d\r\n", (int) Registers.SecondaryIlluminationDriveCurrentAbsoluteMaximum );
		//	OutputString.Append( cTemp );
		//	sprintf( cTemp, "    Boost voltage offset:      %d\r\n", (int) Registers.BoostVoltageOffset);
		//	OutputString.Append(cTemp);
			sprintf( cTemp, "    Focus Station ID:                               %X\r\n", (int) Registers.FocusStationID );
			OutputString.Append( cTemp );
		//	sprintf( cTemp, "    Aimer Current Absolute Max:                     %d\r\n", (int) Registers.AimerDriveCurrentAbsoluteMaximum );
		//	OutputString.Append( cTemp );
		//	sprintf( cTemp, "    Primary Illumination Current Operational Max:   %d\r\n", (int) Registers.PrimaryIlluminationDriveCurrentOperationalMaximum );
		//	OutputString.Append( cTemp );
		//	sprintf( cTemp, "    Secondary Illumination Current Operational Max: %d\r\n", (int) Registers.SecondaryIlluminationDriveCurrentOperationalMaximum );
		//	OutputString.Append( cTemp );
		//	sprintf( cTemp, "    Aimer Current Operational Max:                  %d\r\n", (int) Registers.AimerDriveCurrentOperationalMaximum );
		//	OutputString.Append( cTemp );
			sprintf( cTemp, "    Firmware Checksum:                              %X\r\n", (int) Registers.Checksum );
			OutputString.Append( cTemp );
			sprintf( cTemp, "    Laser Power Setting Backup:                     %d\r\n", (int) Registers.LaserPowerSettingBackup);
			OutputString.Append( cTemp );
		//	Temp.Copy( Registers.LicenseKeys, sizeof( Registers.LicenseKeys ) );
		//	Temp.BinaryToHex();
		//	sprintf( cTemp, "    Control keys:                                   %s\r\n", (char *) Temp.Char );
		//	OutputString.Append( cTemp );
			sprintf( cTemp, "    LED Control Mode:                               %d\r\n", (int) Registers.LEDControl.b.Mode );
			OutputString.Append( cTemp );
		//	sprintf( cTemp, "    LED Current:                                      %d\r\n", (int) Registers.LEDControl.b.Current );
		//	OutputString.Append( cTemp );
		//	sprintf( cTemp, "    PWM Base Frequency:                             %d\r\n", (int) Registers.PWMBaseFrequency );
		//	OutputString.Append( cTemp );
		//	sprintf( cTemp, "    Primary Illumination Current:                   %d\r\n", (int) Registers.PrimaryIlluminationDriveCurrent );
		//	OutputString.Append( cTemp );
		//	sprintf( cTemp, "    Secondary Illumination Current:                 %d\r\n", (int) Registers.SecondaryIlluminationDriveCurrent );
		//	OutputString.Append( cTemp );
		//	sprintf( cTemp, "    Aimer Current:                                  %d\r\n", (int) Registers.AimerDriveCurrent );
		//	OutputString.Append( cTemp );
		//	sprintf( cTemp, "    Pixel Clock Frequency:                          %d\r\n", (int) Registers.PixelClockFrequency );
		//	OutputString.Append( cTemp );
			sprintf( cTemp, "    Firmware Type:                                  %d\r\n", (int) Registers.FirmwareType );
			OutputString.Append( cTemp );
			sprintf( cTemp, "    Firmware Revision:                              %d\r\n", (int) Registers.FirmwareRevision );
			OutputString.Append( cTemp );
			sprintf( cTemp, "    Mfg Test Status:                                %X\r\n", (int) Registers.MfgTestStatus);
			OutputString.Append( cTemp );
			sprintf( cTemp, "    Register Checksum:                              %X\r\n", (int) Registers.RegisterChecksum );
			OutputString.Append( cTemp );
			SendStringToInterface(  (char*) OutputString.Char, OutputString.Size );                   //lmc   fixme
		}
		}
	else
		Status = 0;
	return Status;
} // ShowEngineFirmwareFullReport



//
//	Tags used by ShowEngineFirmwareSettingList to encode field values.
//
typedef enum {
	ENGINE_REGISTERS_ENGINE_ID_TAG,
	ENGINE_REGISTERS_HAND_HELD_PRODUCTS_SERIAL_NUMBER_TAG,
	ENGINE_REGISTERS_CUSTOM_SERIAL_NUMBER_TAG,
	ENGINE_REGISTERS_AIMER_X_CENTER_TAG,
	ENGINE_REGISTERS_AIMER_Y_CENTER_TAG,
	ENGINE_REGISTERS_LASER_POWER_SETTING_TAG,
	ENGINE_REGISTERS_FOCUS_STATION_ID,
//	ENGINE_REGISTERS_PWM_PULSE_WIDTH_TAG,
//	ENGINE_REGISTERS_BOOST_TARGET_VOLTAGE_TAG,
//	ENGINE_REGISTERS_BOOST_VOLTAGE_OFFSET_TAG,
	ENGINE_REGISTERS_FIRMWARE_CHECKSUM_TAG,
	ENGINE_REGISTERS_LASER_POWER_SETTING_BACKUP_TAG,
	ENGINE_REGISTERS_LED_CONTROL_MODE_TAG,
//	ENGINE_REGISTERS_FIRMWARE_TYPE_REVISION_TAG,
	
//	ENGINE_REGISTERS_LED_CURRENT_TAG,
//	ENGINE_REGISTERS_PWM_BASE_FREQUENCY_TAG,
	ENGINE_REGISTERS_FIRMWARE_TYPE_TAG,
	ENGINE_REGISTERS_FIRMWARE_REVISION_TAG,
	ENGINE_REGISTERS_MFG_TEST_STATUS_TAG,
	ENGINE_REGISTERS_REGISTER_CHECKSUM_TAG
	
} ENGINE_REGISTERS_TAGS;



//
//	ShowEngineFirmwareSettingList
//		Display the engine firmware register table contents as a tagged list.
//
bool ShowEngineFirmwareSettingList(void)
{
	bool	Status;
	if(HardwareToken.IAmOEMJade()||(GET_CONFIG_SETTING(AimerType) == AIMER_PSOC_LASER))  
	{
		PSoC_5100_REGISTER_TABLE	Registers;                                  //lmc   fixme
		Status = Psoc5100GetRegisters( &Registers );                        //lmc   fixme

		if( Status )
		{                                                                       //lmc   fixme
			char		temp[ 80 ];



		// Engine ID is tag 00
			sprintf( temp, "%02X%X", ENGINE_REGISTERS_ENGINE_ID_TAG, Registers.EngineID.s );
			ReaderConfiguration->MenuCommandResponse->Append( temp );

		// HHP serial number is tag 01
			sprintf( temp, ":%02X", ENGINE_REGISTERS_HAND_HELD_PRODUCTS_SERIAL_NUMBER_TAG );
			ReaderConfiguration->MenuCommandResponse->Append( temp );
			strncpy( temp, (char *) Registers.SweetcornSerialNumber, sizeof( Registers.SweetcornSerialNumber ) );
			temp[ sizeof( Registers.SweetcornSerialNumber ) ] = 0;
			ReaderConfiguration->MenuCommandResponse->Append( temp );

		// Custom serial number is tag 02
			sprintf( temp, ":%02X", ENGINE_REGISTERS_CUSTOM_SERIAL_NUMBER_TAG );
			ReaderConfiguration->MenuCommandResponse->Append( temp );
			strncpy( temp, (char *) Registers.CustomSerialNumber, sizeof( Registers.CustomSerialNumber ) );
			temp[ sizeof( Registers.CustomSerialNumber ) ] = 0;
			ReaderConfiguration->MenuCommandResponse->Append( temp );

		// Aimer X center is tag 03
			sprintf( temp, ":%02X%d", ENGINE_REGISTERS_AIMER_X_CENTER_TAG, (int) Registers.AimerXCenter );
			ReaderConfiguration->MenuCommandResponse->Append( temp );

		// Aimer Y center is tag 04
			sprintf( temp, ":%02X%d", ENGINE_REGISTERS_AIMER_Y_CENTER_TAG, (int) Registers.AimerYCenter );
			ReaderConfiguration->MenuCommandResponse->Append( temp );

		// Laser power setting is tag 05
			sprintf( temp, ":%02X%d", ENGINE_REGISTERS_LASER_POWER_SETTING_TAG, (int) Registers.LaserPowerSetting );
			ReaderConfiguration->MenuCommandResponse->Append( temp );

		// PWM Pulse Width is tag 06
		//	sprintf( temp, ":%02X%d", ENGINE_REGISTERS_PWM_PULSE_WIDTH_TAG,
		//				(int) Registers.PWMPulseWidth );
		//	ReaderConfiguration->MenuCommandResponse->Append( temp );

		// Boost Target Voltage is tag 07
		//	sprintf( temp, ":%02X%d", ENGINE_REGISTERS_BOOST_TARGET_VOLTAGE_TAG,
		//				(int) Registers.BoostTargetVoltage );
		//	ReaderConfiguration->MenuCommandResponse->Append( temp );

		// Boost Voltage Offset is tag 08
		//	sprintf( temp, ":%02X%d", ENGINE_REGISTERS_BOOST_VOLTAGE_OFFSET_TAG, (int) Registers.BoostVoltageOffset );
		//	ReaderConfiguration->MenuCommandResponse->Append( temp );
		
		// Focus station ID 06
			sprintf( temp, ":%02X%02X", ENGINE_REGISTERS_FOCUS_STATION_ID, (int) Registers.FocusStationID );
			ReaderConfiguration->MenuCommandResponse->Append( temp );    

		// Checksum is tag 07
			sprintf( temp, ":%02X%X", ENGINE_REGISTERS_FIRMWARE_CHECKSUM_TAG, (int) Registers.Checksum );
			ReaderConfiguration->MenuCommandResponse->Append( temp );
			
		// Laser Power Setting Backup is tag 08
			sprintf( temp, ":%02X%d", ENGINE_REGISTERS_LASER_POWER_SETTING_BACKUP_TAG, (int) Registers.LaserPowerSettingBackup );
			ReaderConfiguration->MenuCommandResponse->Append( temp );

		// LED control mode is tag 09
			sprintf( temp, ":%02X%d", ENGINE_REGISTERS_LED_CONTROL_MODE_TAG, (int) Registers.LEDControl.b.Mode );
			ReaderConfiguration->MenuCommandResponse->Append( temp );
			
		// Firmware major revision 0A
			sprintf( temp, ":%02X%d", ENGINE_REGISTERS_FIRMWARE_TYPE_TAG, (int) Registers.FirmwareType );
			ReaderConfiguration->MenuCommandResponse->Append( temp );

		// Firmware major revision 0B
			sprintf( temp, ":%02X%d", ENGINE_REGISTERS_FIRMWARE_REVISION_TAG, (int) Registers.FirmwareRevision );
			ReaderConfiguration->MenuCommandResponse->Append( temp );

		// LED Control Current is tag 0C
		//	sprintf( temp, ":%02X%d", ENGINE_REGISTERS_LED_CURRENT_TAG, (int) Registers.LEDControl.b.Current );
		//	ReaderConfiguration->MenuCommandResponse->Append( temp );

		// PWM base frequency is tag 0D
		//	sprintf( temp, ":%02X%d", ENGINE_REGISTERS_PWM_BASE_FREQUENCY_TAG, (int) Registers.PWMBaseFrequency );
		//	ReaderConfiguration->MenuCommandResponse->Append( temp );
		
		// Mfg Test Status is tag 0C
			sprintf( temp, ":%02X%X", ENGINE_REGISTERS_MFG_TEST_STATUS_TAG,
						(int) Registers.MfgTestStatus );
			ReaderConfiguration->MenuCommandResponse->Append( temp );
			// Register checksum is tag 0D
			sprintf( temp, ":%02X%X", ENGINE_REGISTERS_REGISTER_CHECKSUM_TAG, (int) Registers.RegisterChecksum );
			ReaderConfiguration->MenuCommandResponse->Append( temp );

		                          //lmc   fixme
		}
	}
	else
		Status = 0;
	return Status;
} // ShowEngineFirmwareFullReport



//
//	SetEngineLEDControlColor
//		Change engine illumination color according to the parameter sent by user.
//
bool SetEngineLEDControlColor(void)
{
	bool		Status;



// We will not allow a NULL string.
	if( ReaderConfiguration->ExtraneousTextAfterCommand->Size == 0 )
	{
		Status = false;
	}

// Get the parameter and try to change color.
	else if(HardwareToken.IAmOEMJade())
	{
		int		NumberOfCharactersUsedInConversion;
		int		Color = ReaderConfiguration->ExtraneousTextAfterCommand->ConvertToNumber( 0, &NumberOfCharactersUsedInConversion );
		if( NumberOfCharactersUsedInConversion == ReaderConfiguration->ExtraneousTextAfterCommand->Size )
		{
		// Number is good; try to use it.
			Status=Psoc5100SetIlluminationColor( Color, GET_SETTING( EngineCommandPrivilege ) );            //lmc   fixme
		}
		else
		{
		// Bad color.
			Status = false;
		}
	}
	else
		Status = false;
	return Status;
} // SetEngineLEDControlColor



//
//	EngineReadMemory
//		Dump a memory block from engine memory.
//
bool EngineReadMemory( void )
{
	bool		Status = true;		// No problems encountered yet.
	int			Address = 0;                                                       //lmc   fixme
	int			Length;
	HSTRING		Data( 20, true );


// Get the address and data from the user input string.
	if( ReaderConfiguration->ExtraneousTextAfterCommand->Size > 0 )
	{
		int		NumberOfCharactersUsedInConversion;
		int		PositionOfColon;

		bool	ColonFound = ReaderConfiguration->ExtraneousTextAfterCommand->Find( (unsigned char *) ":", 1, false, 1, &PositionOfColon );

		if(( ColonFound ) && ( PositionOfColon > 0 ) && ( PositionOfColon < ( (int) ReaderConfiguration->ExtraneousTextAfterCommand->Size - 1 ) ))
		{
		//	if(Am_I_OEM_JADE())
				Address = (int) ReaderConfiguration->ExtraneousTextAfterCommand->ConvertHexToNumber                //lmc   fixme
									( 0, PositionOfColon - 1, &NumberOfCharactersUsedInConversion );                 //lmc   fixme
				if( NumberOfCharactersUsedInConversion == PositionOfColon )
			{
				Length = (int) ReaderConfiguration->ExtraneousTextAfterCommand->ConvertHexToNumber
									( PositionOfColon + 1, ReaderConfiguration->ExtraneousTextAfterCommand->Size - 1,
														&NumberOfCharactersUsedInConversion );
				if( NumberOfCharactersUsedInConversion != ( ReaderConfiguration->ExtraneousTextAfterCommand->Size - PositionOfColon - 1 ) )
				{
				// Corrupt length; WOOF!
					Status = false;
				}
			}
			else
			{
			// Corrupt address; WOOF!
				Status = false;
			}
		}
		else
		{
			// No colon was found; WOOF!
			Status = false;
		}
	}
	else
	{
	// No address or data count found; WOOF!
		Status = false;
	}

// If OK so far, try to read from the scan engine.
	if( Status )
	{
		Data.Reallocate( Length + 2 );
		Data.Size = Length;
		if(HardwareToken.IAmOEMJade()||(GET_CONFIG_SETTING(AimerType) == AIMER_PSOC_LASER))  
			Status = Psoc5100Read( Address, (char *) Data.Char, Data.Size );                       //lmc   fixme
		else
			Status = false;
	}

// If OK so far, display the data.
	if( Status )
	{
		char	ResultString[ 2048 ];
		int		i;
		int TotalSize=0;
		for (i = 0;   i < Data.Size ;   i++)
		{
			if ((i % 16) == 0)
			{
				int Result =sprintf (&ResultString[TotalSize],"\r\n%08X -", i ) ;
				if (Result<0)
					return false;
				TotalSize+=Result;
			}
				int Result =sprintf (&ResultString[TotalSize]," %02X", ((int) Data.Char[ i ]));
				if (Result<0)
					return false;
				TotalSize+=Result;
		}

		sprintf (&ResultString[TotalSize],"\r\n");

		SendStringToInterface( (char *) ResultString,strlen(ResultString),true);
	}

	return Status;
} // EngineReadMemory

bool SpreadReadMemory(void)
{
	bool		Status = true;		// No problems encountered yet.
	int			Address = 0;                                                       //lmc   fixme
	int			Length;
	HSTRING		Data( 20, true );


// Get the address and data from the user input string.
	if( ReaderConfiguration->ExtraneousTextAfterCommand->Size > 0 )
	{
		int		NumberOfCharactersUsedInConversion;
		int		PositionOfColon;

		bool	ColonFound = ReaderConfiguration->ExtraneousTextAfterCommand->Find( (unsigned char *) ":", 1, false, 1, &PositionOfColon );

		if(( ColonFound ) && ( PositionOfColon > 0 ) && ( PositionOfColon < ( (int) ReaderConfiguration->ExtraneousTextAfterCommand->Size - 1 ) ))
		{
	//		if(Am_I_OEM_JADE())
				Address = (int) ReaderConfiguration->ExtraneousTextAfterCommand->ConvertHexToNumber                //lmc   fixme
									( 0, PositionOfColon - 1, &NumberOfCharactersUsedInConversion );                 //lmc   fixme
			if( NumberOfCharactersUsedInConversion == PositionOfColon )
			{
				Length = (int) ReaderConfiguration->ExtraneousTextAfterCommand->ConvertHexToNumber
									( PositionOfColon + 1, ReaderConfiguration->ExtraneousTextAfterCommand->Size - 1,
														&NumberOfCharactersUsedInConversion );
				if( NumberOfCharactersUsedInConversion != ( ReaderConfiguration->ExtraneousTextAfterCommand->Size - PositionOfColon - 1 ) )
				{
				// Corrupt length; WOOF!
					Status = false;
				}
			}
			else
			{
			// Corrupt address; WOOF!
				Status = false;
			}
		}
		else
		{
			// No colon was found; WOOF!
			Status = false;
		}
	}
	else
	{
	// No address or data count found; WOOF!
		Status = false;
	}

// If OK so far, try to read from the scan engine.
	if( Status )
	{
		Data.Reallocate( Length + 2 );
		Data.Size = Length;
		if(HardwareToken.IAmOEMJade())
			Status = SpreadReadRegisters( Address, (BYTE *) Data.Char, Data.Size );                       //lmc   fixme
		else
			Status = false;
	}

// If OK so far, display the data.
	if( Status )
	{
		char	ResultString[ 2048 ];
		int		i;
		int TotalSize=0;
		for (i = 0;   i < Data.Size ;   i++)
		{
			if ((i % 16) == 0)
			{
				int Result =sprintf (&ResultString[TotalSize],"\r\n%08X -", i ) ;
				if (Result<0)
					return false;
				TotalSize+=Result;
			}
				int Result =sprintf (&ResultString[TotalSize]," %02X", ((int) Data.Char[ i ]));
				if (Result<0)
					return false;
				TotalSize+=Result;
		}

		sprintf (&ResultString[TotalSize],"\r\n");

		SendStringToInterface( (char *) ResultString,strlen(ResultString),true);
	}

	return Status;
} // SpreadReadMemory
//
//	EngineWriteMemory
//		Write a memory block to engine memory from the caller specified location.
//
bool EngineWriteMemory( void )
{
	bool		Status = true;				// No problems encountered yet.                                                                         //lmc   fixme
	bool		AddressWasFound = false;	// Didn't find one yet.
	HSTRING		Data( 20, true );
	int			Address = 0;


// If ENGPRV is not enabled, WOOF!
	if( ! GET_SETTING( EngineCommandPrivilege ) )
	{
		Status = false;
	}

// Get the address and data from the user input string.
	else if( ReaderConfiguration->ExtraneousTextAfterCommand->Size > 0 )
	{
		int		NumberOfCharactersUsedInConversion = 0;
		int		PositionOfColon;

		bool	ColonFound = ReaderConfiguration->ExtraneousTextAfterCommand->Find( (unsigned char *) ":", 1, false, 1, &PositionOfColon );

		if(( ColonFound ) && ( PositionOfColon > 0 ) && ( PositionOfColon < ( (int) ReaderConfiguration->ExtraneousTextAfterCommand->Size - 1 ) ))
		{
			if(HardwareToken.IAmOEMJade()||(GET_CONFIG_SETTING(AimerType) == AIMER_PSOC_LASER))   
				Address = (int) ReaderConfiguration->ExtraneousTextAfterCommand->ConvertHexToNumber                 //lmc   fixme
									( 0, PositionOfColon - 1, &NumberOfCharactersUsedInConversion );                   //lmc   fixme
			if( NumberOfCharactersUsedInConversion == PositionOfColon )
			{
				Data.Copy( ReaderConfiguration->ExtraneousTextAfterCommand, PositionOfColon + 1,
																ReaderConfiguration->ExtraneousTextAfterCommand->Size - 1 );
				Status = Data.HexToBinary();
				AddressWasFound = true;
			}
			else
			{
			// Corrupt address; WOOF!
				Status = false;
			}
		}
		else
		{
		// No colon was found, so this must be data only.
			Data.Copy( ReaderConfiguration->ExtraneousTextAfterCommand );
			Status = Data.HexToBinary();
		}
	}
	else
	{
	// No address or data found; WOOF!
		Status = false;
	}

// If OK so far, try to write to the scan engine.
	if(( Status ))
	{
	 	if ( AddressWasFound ){
			if(HardwareToken.IAmOEMJade()||(GET_CONFIG_SETTING(AimerType) == AIMER_PSOC_LASER))   
				Status = Psoc5100Write( Address, (unsigned char *) Data.Char, Data.Size);              //lmc   fixme
			else
	 			Status = false;
			}
	 	else{
			if(HardwareToken.IAmOEMJade()||(GET_CONFIG_SETTING(AimerType) == AIMER_PSOC_LASER))   
				Status = Psoc5100Write ((char *) Data.Char, Data.Size);                               //lmc   fixme
			else
	 			Status = false;
	 		}
	}

	return Status;
} // EngineWriteMemory
//SpreadWriteMemory
bool SpreadWriteMemory(void)
{
	bool		Status = true;				// No problems encountered yet.                                                                         //lmc   fixme
	bool		AddressWasFound = false;	// Didn't find one yet.
	HSTRING		Data( 20, true );
	int			Address = 0;


// If ENGPRV is not enabled, WOOF!
	if( ! GET_SETTING( EngineCommandPrivilege ) )
	{
		Status = false;
	}

// Get the address and data from the user input string.
	else if( ReaderConfiguration->ExtraneousTextAfterCommand->Size > 0 )
	{
		int		NumberOfCharactersUsedInConversion = 0;
		int		PositionOfColon;

		bool	ColonFound = ReaderConfiguration->ExtraneousTextAfterCommand->Find( (unsigned char *) ":", 1, false, 1, &PositionOfColon );

		if(( ColonFound ) && ( PositionOfColon > 0 ) && ( PositionOfColon < ( (int) ReaderConfiguration->ExtraneousTextAfterCommand->Size - 1 ) ))
		{
			if(HardwareToken.IAmOEMJade())
				Address = (int) ReaderConfiguration->ExtraneousTextAfterCommand->ConvertHexToNumber                 //lmc   fixme
									( 0, PositionOfColon - 1, &NumberOfCharactersUsedInConversion );                   //lmc   fixme
			if( NumberOfCharactersUsedInConversion == PositionOfColon )
			{
				Data.Copy( ReaderConfiguration->ExtraneousTextAfterCommand, PositionOfColon + 1,
																ReaderConfiguration->ExtraneousTextAfterCommand->Size - 1 );
				Status = Data.HexToBinary();
				AddressWasFound = true;
			}
			else
			{
			// Corrupt address; WOOF!
				Status = false;
			}
		}
		else
		{
		// No colon was found, so this must be data only.
			Data.Copy( ReaderConfiguration->ExtraneousTextAfterCommand );
			Status = Data.HexToBinary();
		}
	}
	else
	{
	// No address or data found; WOOF!
		Status = false;
	}

// If OK so far, try to write to the scan engine.
	if(( Status ))
	{
	 	if ( AddressWasFound ){
			unsigned char SendChar[2] = {0x01, Data.Char[0]};
			if(HardwareToken.IAmOEMJade())
				Status = SpreadWriteRegisters(Address,(BYTE* ) &SendChar,2);
			else
	 			Status = false;
		}
	 	else
	 		Status = false;
	 }
	return Status;
}
//
//	EngineWriteRAMToEE
//		Write a memory block to engine memory from the caller specified location.
//
bool EngineWriteRamToEE( void )
{
	bool Status = TRUE;

	// If ENGPRV is not enabled, WOOF!
	if( ! GET_SETTING( EngineCommandPrivilege ) ) {
		Status = false;
	}
	if(HardwareToken.IAmOEMJade()||(GET_CONFIG_SETTING(AimerType) == AIMER_PSOC_LASER))  
		Status = Psoc5100SaveToEEprom();
	else
		Status = 0;//Psoc5100SaveToEEprom();                                            //lmc   fixme
	return Status;
}



//
//	GetEngineFirmwareRevisionInformation
//		If this device has an engine PSOC, return the firmware type and revision information.
//
bool GetEngineFirmwareRevisionInformation( void )
{
	bool	Status = false;		// Haven't succeeded yet.


// ENGREV only accepts a query.
	if( ReaderConfiguration->ExtraneousTextAfterCommand->Size == 1 )
	{
	// Is it a setting query?
		if(( ReaderConfiguration->ExtraneousTextAfterCommand->Compare( "?", false ) == 0 ) ||
					( ReaderConfiguration->ExtraneousTextAfterCommand->Compare( "^", false ) == 0 ))
		{
	// Only do this if there is an engine PSOC.
			if(HardwareToken.IAmOEMJade()||(GET_CONFIG_SETTING(AimerType) == AIMER_PSOC_LASER))  
			{
				unsigned int	PSOCFirmwareType;
				unsigned int	PSOCFirmwareRevision;
				if( GetEnginePSOCRevision( &PSOCFirmwareType, &PSOCFirmwareRevision ) )
				{
					char	temp[ 80 ];
					sprintf( temp, "Type: %d  Revision: %d", PSOCFirmwareType, PSOCFirmwareRevision );
					ReaderConfiguration->MenuCommandResponse->Append( temp );
					Status = true;
				}
			}
			else
		// If there is no PSOC, we'll just return a NULL string, but we will indicate success.
				Status = true;

			ReaderConfiguration->OneOrMoreQueryCommandsWereProcessed = true;
		}

	// Is it a range query?
		else if(( ReaderConfiguration->ExtraneousTextAfterCommand->Compare( "*", false ) == 0 ) ||
					( ReaderConfiguration->ExtraneousTextAfterCommand->Compare( "#", false ) == 0 ))
		{
			ReaderConfiguration->MenuCommandResponse->Append( "00-FF" );
			Status = true;
		}
	}

	return Status;
} //	GetEngineFirmwareRevisionInformation



//
//IsImagingBlockedByAllignment()
//Tests if imaging allowed on DR, must be alligned first, error beeps if not alligned
//Returns a bool, true if imaging allowed
//
bool IsImagingBlockedByAllignment( void )
{
	bool bExitImaging = false;     //Cause a menu NAK here if imaging not allowed for this menu item

	{
		bExitImaging = true;
	}

	return bExitImaging;
}



CIrqVector m_saved_gpio_vector;
static bool bGpioVectorSet = false;

static threadhandle_t GPIO3_Service_IRQ1(void *)
{
 	if( READ_FLASHOUT_INT_CONTROL() )
	{	  
		if(READ_FLASHOUT_INT_Signal())  //Check if the FlashOut Interrupt has been enabled
		{
			CLEAR_FLASHOUT_INT_Signal();
			if(false == DoesPluginHaveControlofNFRB())
			{
				bool FlashOutState = FLASHOUT_PINSTATUS();		//Only Place It is updated in Interupt or Lights Thread.

				if(FlashOutState)
				{
					HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT31_LSH,GPIO_BIT31_WID,GPIO_LOW);	//set data reg to LOW
				}
				else
				{	
					HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT31_LSH,GPIO_BIT31_WID,GPIO_HIGH);	//set data reg to High
				} //endof Flash Out Low
			}
		} //endof if Flash Out Interupt Enable
	}

	return NULL;
}	//endof GPIO3_Service_IRQ

void RemoveGpioVector(void)
{
	if(bGpioVectorSet)
	{
		SetVector( m_saved_gpio_vector, VEC_GPIO3_INT );
		bGpioVectorSet = false;
	}
}

void SetGpioVector(void)
{
	if(!bGpioVectorSet)
	{
		SetVector(GPIO3_Service_IRQ1,VEC_GPIO3_INT);
		bGpioVectorSet = true;
	}
}

bool ConnectFLOtoExternal()
{
	if(HardwareToken.IAmOEM())
	{
		bool DoConnectFlotoExternal = false;
	
		if(true == DoesPluginHaveControlofNFRB())
		{
			return false;
		}
	
		if(ReaderConfiguration->MenuCommandResponse->Char[0] == '1')
		{
			DoConnectFlotoExternal = true;
		}
	 
		if( DoConnectFlotoExternal)
		{
  			HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_ECB,MUX_MODE_5);
			HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_ECB,0x00002040);	
			HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT23_LSH,GPIO_BIT23_WID,GPIO_GDIR_INPUT);
			GetVector(m_saved_gpio_vector, VEC_GPIO3_INT);
			bGpioVectorSet = true; 
			SetVector(GPIO3_Service_IRQ1,VEC_GPIO3_INT);
			FLASHOUT_INT_CONTROL(false);								//Turn Off the Flash_Out Interrupt
			CLEAR_FLASHOUT_INT_Signal();										//Clear the Flag if it is set
			WRITE_FIELD_TO_REG(GPIO3_EDGE,GPIO_BIT23_LSH,GPIO_BIT23_WID,0x1);	//Set to INT on any EDGE, overrides the ICR2 REG SETTING
			EnableInterruptNumber(VEC_GPIO3_INT);  								// Enable interrupts GPIO Port 3
			FLASHOUT_INT_CONTROL(true);	
		}
		else
		{
			DisableInterruptNumber(VEC_GPIO3_INT);
			HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_ECB,MUX_MODE_0);
			FLASHOUT_INT_CONTROL(false);
			CLEAR_FLASHOUT_INT_Signal();
			RemoveGpioVector();
			HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT31_LSH,GPIO_BIT31_WID,GPIO_LOW);	//set data reg to LOW		   
		}
	}	//end if HardwareToken.IAmOEM()

	return true;
}


#else       //else  RF BASE   //************************************************************************************


#endif     //end else  RF BASE       //**************************************************************************






#ifdef RFBASE     //if Rf Base  //*********************************************************************************************
#else      //else not RF BASE ***********************************************************************************************


extern bool ImageSnap(void);
extern bool ImageShip(void);
extern bool RawImageShip(void);
extern bool ImageTest(void);                        //lmc   unused???
extern bool RowNoiseReduction(void);                //lmc   unused???
extern bool ImageRegisterWrite(void);               //lmc   unused???
extern bool ImageRegisterRead(void);                //lmc   unused???
extern bool TakeTwoImages(void);                   //lmc   unused???
extern bool AutoCropImage(void);
extern bool DumpImgBox(void);
//extern bool ImagerWriteExposure(void);              //lmc   fixme


bool ImagerWriteExposure(void)    { return 0; }         //lmc   remove when fixme

#endif





#if !defined( RFBASE ) && !defined( RFSCAN )
//
//	ChangeIlluminationAlwaysOnState
//		Set illumination always on state (only applies to 3800G for now).
//
bool ChangeIlluminationAlwaysOnState( void )
{
	//theHal.SetIlluminationAlwaysOnState( GET_SETTING( IlluminationAlwaysOn ));                    //lmc   fixit
	return true;
} // ChangeIlluminationAlwaysOnState



#endif     //end not RF BASE or not RF Scan ***********************************************************************************************


#ifdef BYD_SENSOR


void READER_CONFIGURATION::CreateImagingMenuSettings( int ControlMask, UINT AimerCommandModifier, UINT ImageShippingCommandModifier,
		UINT Two_DCommandModifier, UINT /* Two_DTagCommandModifier */, UINT Two_DSubtagCommandModifier, UINT HeatMitigationFixedSettingsCommandModifier )
{
	AddBoolean("040200",SUB_TAG_TABLE::VISIBLE_BY_TAG | LOCAL_SETTING , NULL, NULL, &AutoExposureReport, false, false);
	AddNumber("040201",SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP ,NULL,NULL,&MaximumGain,BYD_GAIN_MAX,BYD_GAIN_MAX,BYD_GAIN_MIN,BYD_GAIN_MAX);
	AddNumber("040202",SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP ,NULL,NULL,&AldMaximumGain,BYD_GAIN_MAX>>1,BYD_GAIN_MAX>>1,BYD_GAIN_MIN,BYD_GAIN_MAX);
	AddNumber("040203",SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP ,NULL,NULL,&MaximumExposure,BYD_INT_MAX,BYD_INT_MAX,BYD_INT_MIN,BYD_INT_MAX);		
	AddNumber("040204",SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP ,NULL,NULL,&StartingGain,BYD_GAIN_MAX,BYD_GAIN_MAX,BYD_GAIN_MIN,BYD_GAIN_MAX);
	AddNumber("040205",SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP ,NULL,NULL,&AldStartingGain,BYD_GAIN_MAX>>1,BYD_GAIN_MAX>>1,BYD_GAIN_MIN,BYD_GAIN_MAX);
	AddNumber("040206",SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP ,NULL,NULL,&StartingExposure,BYD_INT_MAX,BYD_INT_MAX,BYD_INT_MIN,BYD_INT_MAX);

	AddNumber("040207",SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP ,NULL,NULL,&TargetWhite,90,90,48,212);
	AddNumber("040208",SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP ,NULL,NULL,&AldTargetWhite,90,90,10,212);

	AddNumericList("040209",SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP ,NULL,NULL,&ExposureMethod,HHPSD_AE_METHOD_CENTER_WEIGHTED,HHPSD_AE_METHOD_CENTER_WEIGHTED,
				HHPSD_AE_METHOD_UNIFORM,HHPSD_AE_METHOD_CENTER_ONLY,HHPSD_AE_METHOD_CENTER_WEIGHTED);
	AddNumericList("04020A",SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP ,NULL,NULL,&SpecularDetection,HHPSD_SPECULAR_EXCLUSION_MODERATE,HHPSD_SPECULAR_EXCLUSION_MODERATE,
				HHPSD_SPECULAR_EXCLUSION_OFF,HHPSD_SPECULAR_EXCLUSION_MINIMAL,HHPSD_SPECULAR_EXCLUSION_MODERATE,HHPSD_SPECULAR_EXCLUSION_AGGRESSIVE);
	AddNumber("04020B",SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP ,NULL,NULL,&TargetPercentile,97,97,80,99);
	AddNumber("04020C",SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP ,NULL,NULL,&MinimumExposure,BYD_INT_MIN,BYD_INT_MIN,BYD_INT_MIN,BYD_INT_MAX);

	AddNumber("04020D",SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP, NULL, NULL, &bImageMustConform, 0,0,0,1);

	AddNumber("04020E",SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP ,NULL,NULL,&MaximumGainAuto,BYD_GAIN_MAX,BYD_GAIN_MAX,BYD_GAIN_MIN,BYD_GAIN_MAX);
	AddNumber("04020F",SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP ,NULL,NULL,&MaximumExposureAuto,BYD_INT_MAX,BYD_INT_MAX,BYD_INT_MIN,BYD_INT_MAX);


	//for manual trigggers
	AddNumericList("040210", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP , NULL,NULL,&ExposureMode,
				   HHPSD_CONTEXT_SENSITIVE_USE_HHP, HHPSD_CONTEXT_SENSITIVE_USE_HHP,
				   HHPSD_EXPOSURE_FIXED,HHPSD_AUTOEXPOSURE_USE_ONCHIP,HHPSD_AUTOEXPOSURE_USE_HHP,
				   HHPSD_AUTOEXPOSURE_HHP_PRESENTATION, HHPSD_CONTEXT_SENSITIVE_USE_HHP,
				   HHPSD_OPENLOOP_GAIN, HHPSD_CELLPHONE, HHPSD_AUTOEXPOSURE_DUAL_TRACK_USE_HHP,	HHPSD_CONTEXT_SENSITIVE_DUAL_TRACK_USE_HHP,
				   7, 8, 9, 10);
	//for streaming pres
#if (DECODER_ID)
	int ExposureModeStreamingInit = HHPSD_EXPOSURE_FIXED;
#else
	int ExposureModeStreamingInit = HHPSD_AUTOEXPOSURE_HHP_PRESENTATION;
#endif
	AddNumericList("040211", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP , NULL,NULL,&ExposureModeStreaming, ExposureModeStreamingInit, ExposureModeStreamingInit,
				   HHPSD_EXPOSURE_FIXED,HHPSD_AUTOEXPOSURE_USE_ONCHIP,HHPSD_AUTOEXPOSURE_USE_HHP,
				   HHPSD_AUTOEXPOSURE_HHP_PRESENTATION, HHPSD_CONTEXT_SENSITIVE_USE_HHP,
				   HHPSD_OPENLOOP_GAIN, HHPSD_AUTOEXPOSURE_DUAL_TRACK_USE_HHP, HHPSD_CONTEXT_SENSITIVE_DUAL_TRACK_USE_HHP );

	// for exposure of cell phone frame
	AddNumber("040212", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP , NULL,NULL, &ExposureCellphone, BYD_INT_FIXED,BYD_INT_FIXED,BYD_INT_MIN,BYD_INT_MAX);

	//for pres mode 3 low lights
	AddNumericList("040213", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP , NULL,NULL,&ExposureModePresLowLights, HHPSD_AUTOEXPOSURE_HHP_PRESENTATION, HHPSD_AUTOEXPOSURE_HHP_PRESENTATION,
				   HHPSD_EXPOSURE_FIXED,HHPSD_AUTOEXPOSURE_USE_ONCHIP,HHPSD_AUTOEXPOSURE_USE_HHP,
				   HHPSD_AUTOEXPOSURE_HHP_PRESENTATION, HHPSD_CONTEXT_SENSITIVE_USE_HHP,
				   HHPSD_OPENLOOP_GAIN, HHPSD_AUTOEXPOSURE_DUAL_TRACK_USE_HHP, HHPSD_CONTEXT_SENSITIVE_DUAL_TRACK_USE_HHP );

	//for pres mode 3 no lights
	AddNumericList("040214", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP , NULL,NULL,&ExposureModePresNoLights, HHPSD_AUTOEXPOSURE_HHP_PRESENTATION, HHPSD_AUTOEXPOSURE_HHP_PRESENTATION,
				   HHPSD_EXPOSURE_FIXED,HHPSD_AUTOEXPOSURE_USE_ONCHIP,HHPSD_AUTOEXPOSURE_USE_HHP,
				   HHPSD_AUTOEXPOSURE_HHP_PRESENTATION, HHPSD_CONTEXT_SENSITIVE_USE_HHP,
				   HHPSD_OPENLOOP_GAIN, HHPSD_AUTOEXPOSURE_DUAL_TRACK_USE_HHP, HHPSD_CONTEXT_SENSITIVE_DUAL_TRACK_USE_HHP );


	AddBoolean("040215", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &AutoExposureLightsOffAdjust,false,false);

  	AddNumber("040300",SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP ,NULL,NULL,&PresentationExposure,BYD_INT_MAX,BYD_INT_MAX,BYD_INT_MIN,BYD_INT_MAX);
	AddNumber("040301",SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP ,NULL,NULL,&PresentationGain,BYD_GAIN_MAX,BYD_GAIN_MAX,BYD_GAIN_MIN,BYD_GAIN_MAX);


	AddNumber("040216",SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP ,NULL,NULL,&FixedGain,BYD_GAIN_FIXED,BYD_GAIN_FIXED,BYD_GAIN_MIN,BYD_GAIN_MAX);
	AddNumber("040217",SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP ,NULL,NULL,&FixedExposure,BYD_INT_FIXED,BYD_INT_FIXED,BYD_INT_MIN,BYD_INT_MAX);

	AddNumber("040400", SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING , NULL,NULL, &FLODelayUserSetting, 0, 0, 0, 0);
	AddBoolean("040500", SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, &ConnectFLOtoExternal,NULL, &EnConnecttoExt, false, false);

	// Create image engine type fixed setting here and fill in the value when the scan driver is up and running.
	AddNumber("040600", SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::IGNORE_ON_STORAGE_QUERY,NULL, NULL, &ImageEngineType, -1, -1, -1, 0x7fffffff );
	AddCommand("040601", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND,&WriteEngineFirmwareFile, NULL);
	AddCommand("040602", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &WriteDefaultEngineFirmware, NULL);
	AddCommand("040603", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &ShowEngineFirmwareChecksum, NULL);
	AddCommand("040604", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &ShowEngineFirmwareFullReport, NULL);
	AddCommand("040605", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &ShowEngineFirmwareSettingList, NULL);
	AddNumber("040606",SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING , NULL, NULL, &I2CReadTimeout, 80, 80, 0, 0x7FFFFFFF);
	AddBoolean("040607", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG, NULL, ProcessEngineFirmwareDiagnosticModeChange, &EngineFirmwareDiagnosticMode, false, false);
	AddBoolean("040608", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG, NULL, NULL, &EngineFirmwareUpdateVerify, true, true );  // HIDDEN verify and re-program engine firmware on powerup

	AddBoolean("040700", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG,NULL, NULL, &PSOCVSSetting, true, true);  // Not used anymore, but is here because PSOC download tool wants to see it.

    AddCommand( "040609", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND | SUB_TAG_TABLE::PASS_QUERY_CHARACTER_TO_ENTRY_EXIT_FUNCTIONS, &GetEngineFirmwareRevisionInformation, NULL);
	AddBoolean("04060A", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG, NULL, NULL, &EngineCommandPrivilege, false, false);
	AddCommand("04060B", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND,&EngineReadMemory, NULL);
	AddCommand("04060C", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND,&EngineWriteMemory, NULL);
	AddCommand("04060D", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING,	&EngineWriteRamToEE, NULL);


	AddNumber("040800",Two_DSubtagCommandModifier | WORKGROUP, NULL, NULL, &iImagerResetTime, 60, 60,0,0xFFFF);     //REMOVE FOR RELEASE


 	AddNumber("040900", NULL | WORKGROUP, NULL, NULL, &ScanZone, 100, 100, 0, 100);
	AddNumber("040901", AimerCommandModifier | WORKGROUP,NULL,NULL,&ScanAutoAimer,0,0,0,30000);
	AddNumber("040902", Two_DCommandModifier | WORKGROUP,NULL,NULL,&ScanAimerIdleTimeout,0,0,0,240000);
	AddNumber("040903", AimerCommandModifier | WORKGROUP ,NULL,NULL,&ScanAimerDuration,0,0,0,240000);
	if( (ControlMask & MAKE_HEAT_MITIGATION_FIXED_SETTINGS_FIXED) && (ControlMask & MAKE_HEAT_MITIGATION_AVAILABLE) )  //enabled in standard build
	{
		AddNumber("040904", Two_DSubtagCommandModifier | WORKGROUP | HeatMitigationFixedSettingsCommandModifier, NULL, NULL, &ScanAimerDelay,0,0,0,4000);    //used to be AimerCommandModifier, also verizon made them   | SUB_TAG_TABLE::FIXED
		AddNumber("040905", Two_DSubtagCommandModifier | WORKGROUP | HeatMitigationFixedSettingsCommandModifier, NULL, NULL, &IllOnDuringDecode,1,1,0,1);   //removed 2 from list, HHPSD_LM_FLASH not supported in gen6		//used to be Two_DCommandModifier, also verizon made them   | SUB_TAG_TABLE::FIXED
	}
	else     //MAKE_HEAT_MITIGATION_FIXED_SETTINGS_FIXED not enabled or it is an engineering build
	{
		AddNumber("040904", AimerCommandModifier | WORKGROUP, NULL, NULL, &ScanAimerDelay,0,0,0,4000);
		AddNumber("040905", Two_DCommandModifier | WORKGROUP , NULL, NULL, &IllOnDuringDecode,1,1,0,1);      //removed 2 from list, HHPSD_LM_FLASH not supported in gen6
	}
	AddNumericList("040906", AimerCommandModifier | WORKGROUP, NULL, NULL, &ScanAimerMode, AIM_ALT, AIM_ALT, AIM_OFF, AIM_ALT, AIM_CONTINUOUS);
	AddNumericList("040302", AimerCommandModifier | WORKGROUP, NULL, NULL, &CellScanAimerMode, AIM_ALT, AIM_ALT, AIM_OFF, AIM_ALT, AIM_CONTINUOUS);
	AddBoolean( "040A00", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, ChangeIlluminationAlwaysOnState, &IlluminationAlwaysOn, false, false );
    AddBoolean("040907", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING , NULL, NULL, &bUseNonALDEngine, false, false);

	if(ControlMask & MAKE_2D_COMMANDS_AVAILABLE)        //Led intensity for Pres mode
	{
		if(HardwareToken.IAmVoyager1400())
		{
			if(HardwareToken.IAmYoujie4600())
			{
				//AddNumericList("PWRLDC", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | Two_DCommandModifier | WORKGROUP , NULL, NULL, &LedIntensityPercentage, FLASH_LED_HIGH, FLASH_LED_HIGH, FLASH_LED_OFF, FLASH_LED_LOW, FLASH_LED_HIGH);
				LedIntensityPercentage = FLASH_LED_HIGH;
				//AddNumericList("PWRNOL", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | Two_DCommandModifier | WORKGROUP , NULL, NULL, &NonOpticalTrigLedIntensityPercentage, FLASH_LED_HIGH, FLASH_LED_HIGH, FLASH_LED_LOW, FLASH_LED_HIGH);
				AddNumericList("040100", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | Two_DCommandModifier | WORKGROUP , NULL, NULL, &NonOpticalTrigLedIntensityPercentage, 150, 150, 50, 150);//Youjie request only 2 level illumination, the current is 50 and 150 mA
				Hi2dPresIdleCurrent = ILLUM_LED_LEVL3;//50;
			}
			else
			{
				//AddNumericList("PWRLDC", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | Two_DCommandModifier | WORKGROUP , NULL, NULL, &LedIntensityPercentage, FLASH_LED_HIGH, FLASH_LED_HIGH, FLASH_LED_OFF, FLASH_LED_LOW, FLASH_LED_HIGH);
				LedIntensityPercentage = FLASH_LED_HIGH;//the user from using this command and making them confused. HI2D does not support the feature
				//AddNumericList("PWRLDC", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | Two_DCommandModifier | WORKGROUP , NULL, NULL, &LedIntensityPercentage, 50, 50, 7, 15, 50, 150);
				//AddNumericList("PWRNOL", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | Two_DCommandModifier | WORKGROUP , NULL, NULL, &NonOpticalTrigLedIntensityPercentage, FLASH_LED_HIGH, FLASH_LED_HIGH, FLASH_LED_LOW, FLASH_LED_HIGH);
				AddNumericList("040100", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | Two_DCommandModifier | WORKGROUP , NULL, NULL, &NonOpticalTrigLedIntensityPercentage, ILLUM_LED_LEVL4, ILLUM_LED_LEVL4, ILLUM_LED_LEVL1, ILLUM_LED_LEVL2, ILLUM_LED_LEVL3, ILLUM_LED_LEVL4);//HI2D's change request need 4 level illumination, the current is 7,15,50 and 150 mA
				AddNumericList("040101", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | Two_DCommandModifier | WORKGROUP , NULL, NULL, &Hi2dPresIdleCurrent, ILLUM_LED_LEVL3, ILLUM_LED_LEVL3, ILLUM_LED_LEVL0, ILLUM_LED_LEVL1, ILLUM_LED_LEVL2, ILLUM_LED_LEVL3);//HI2D prese mode idle current:7 , 15, 50 mA
			}
		}
	}

	AddNumber("040801", Two_DCommandModifier | LOCAL_SETTING , NULL, NULL, &ImgRowNumberOfPixels, ABSOLUTE_IMAGE_MAX_COL, ABSOLUTE_IMAGE_MAX_COL, 0, ABSOLUTE_IMAGE_MAX_COL);
	AddNumber("040802", Two_DCommandModifier | LOCAL_SETTING, NULL, NULL, &ImgColNumberOfPixels, ABSOLUTE_IMAGE_MAX_ROW, ABSOLUTE_IMAGE_MAX_ROW, 0, ABSOLUTE_IMAGE_MAX_ROW);
   	AddNumber("040803", Two_DSubtagCommandModifier | LOCAL_SETTING , NULL, NULL, &ImgRowNumberOfPixelsMinus1, ABSOLUTE_IMAGE_MAX_COL-1, ABSOLUTE_IMAGE_MAX_COL-1, 0, ABSOLUTE_IMAGE_MAX_COL-1);
	AddNumber("040804", Two_DSubtagCommandModifier | LOCAL_SETTING , NULL, NULL, &ImgColNumberOfPixelsMinus1, ABSOLUTE_IMAGE_MAX_ROW-1, ABSOLUTE_IMAGE_MAX_ROW-1, 0, ABSOLUTE_IMAGE_MAX_ROW-1);
  	AddNumber("040805", SUB_TAG_TABLE::FIXED | Two_DCommandModifier | LOCAL_SETTING, NULL, NULL, &ImgBitsPerPixel, 8, 8, 8, 8);

	AddCommand("040806", ImageShippingCommandModifier | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &ImageSnap, &IsImagingBlockedByAllignment);
	AddCommand("040807", ImageShippingCommandModifier | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &ImageShip, &IsImagingBlockedByAllignment);
	AddCommand("040808", Two_DCommandModifier | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &AutoCropImage, &IsImagingBlockedByAllignment);
	AddCommand("040809", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &RawImageShip, NULL);

	AddCommand("04080A", Two_DCommandModifier | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &DumpImgBox, NULL);
	AddCommand("04080B", Two_DSubtagCommandModifier | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &RowNoiseReduction, NULL);
    AddNumber("04080C", Two_DCommandModifier | WORKGROUP , NULL, NULL, &PDF_SigCapRowHeightToWidth, 3, 3, 1, 12);
	AddNumber("04080D", Two_DCommandModifier | WORKGROUP , NULL, NULL, &Lin_HeightToNarrowElementWidth, 30, 30, 1, 200);
    AddCommand("04080E", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &TakeTwoImages, NULL);


	if(ControlMask & MAKE_IMAGE_SHIPPING_AVAILABLE)
	{
		AddCommand("04080F", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &ImageTest, NULL);
	}
	else
	{
		AddCommand("04080F", ImageShippingCommandModifier | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &ImageTest, NULL);
	}

	AddBoolean( "040810", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING , NULL, NULL, &ImageTestHorizontalRoll, false, false );
	AddNumber("040811", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING , NULL, NULL, &ImageTestXDimension,640, 640, 1, 8192 );
	AddNumber("040812", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, NULL, &ImageTestYDimension,480, 480, 1, 8192 );
	AddBoolean( "040813", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING , NULL, NULL, &UseMiddleOfImage, false, false);


	AddCommand("040814", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &ImageRegisterWrite, NULL);
	AddCommand("040815", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &ImageRegisterRead, NULL);
	AddNumber("040816", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &ImagerIdlePowerLevel, 1, 1, 0, 2);		// Make sure that the active value gets initialized.     //lmc   changed from default 0 to 1 for test
// Change window parameters to be generic, group includes IMGTRU,  IMGLFT, IMGRGT, IMGTOP, IMGBOT.
	deferred_reset_state = DEFERRED_RESET_NOT_ALLOWED;
	AddNumber("040817", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, deferred_reset, &ImageROILeft,0,0,0,639,&ImageROIRight);
	AddNumber("040818", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, deferred_reset, &ImageROIRight,639,639,0,&ImageROILeft,639);
	AddNumber("040819", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, deferred_reset, &ImageROITop,0,0,0,479,&ImageROIBottom);
	AddNumber("04081A", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, deferred_reset, &ImageROIBottom,479,479,0,&ImageROITop,479);
	AddNumber("04081B", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, deferred_reset, &ImageVertFlip,0,0,0,1);
	AddNumber("04081C", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, deferred_reset, &ImageHorizFlip,0,0,0,1);

	AddBoolean("04081D", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, deferred_reset, &ImageROIEnabled,false, false);
	
// Imaging commands
	AddCommand("04081E", Two_DCommandModifier, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);
	AddNumber("04081F", Two_DCommandModifier | WORKGROUP , NULL, NULL, &JPEGQualityFactor, JPEG_QUALITY_DEFAULT, JPEG_QUALITY_DEFAULT, JPEG_QUALITY_MINIMUM, JPEG_QUALITY_MAXIMUM);
	AddNumericList("040820", Two_DCommandModifier | WORKGROUP , NULL, NULL, &TransmitImageProtocol,
			TRANSMIT_IMAGE_NO_PROTOCOL, TRANSMIT_IMAGE_NO_PROTOCOL,
			TRANSMIT_IMAGE_NO_PROTOCOL_INFO_AT_END, TRANSMIT_IMAGE_DOLPHIN, TRANSMIT_IMAGE_NO_PROTOCOL,
			TRANSMIT_IMAGE_HMODEM_COMPRESSED_PROTOCOL, TRANSMIT_IMAGE_HMODEM_UNCOMPRESSED_PROTOCOL);

	AddBoolean("040821", Two_DCommandModifier | WORKGROUP , NULL, NULL, &InvertImageAboutXAxis, false, false);
	AddBoolean("040822", Two_DCommandModifier | WORKGROUP , NULL, NULL, &InvertImageAboutYAxis, false, false);


	AddNumber("040823", Two_DCommandModifier | WORKGROUP , NULL, NULL, &TransmitImageWindowLeft, 0, 0, 0, ABSOLUTE_IMAGE_MAX_COL-1, &TransmitImageWindowRight);
	AddNumber("040824", Two_DCommandModifier | WORKGROUP , NULL, NULL, &TransmitImageWindowRight, ImgRowNumberOfPixelsMinus1, ImgRowNumberOfPixelsMinus1, 0, &TransmitImageWindowLeft, ABSOLUTE_IMAGE_MAX_COL-1,&ImgRowNumberOfPixelsMinus1);
	AddNumber("040825", Two_DCommandModifier | WORKGROUP , NULL, NULL, &TransmitImageWindowBottom, ImgColNumberOfPixelsMinus1, ImgColNumberOfPixelsMinus1, 0, &TransmitImageWindowTop, ABSOLUTE_IMAGE_MAX_ROW-1,&ImgColNumberOfPixelsMinus1);
	AddNumber("040826", Two_DCommandModifier | WORKGROUP , NULL, NULL, &TransmitImageWindowTop,0, 0, 0, ABSOLUTE_IMAGE_MAX_ROW-1,&TransmitImageWindowBottom);

	AddNumber("040827", Two_DCommandModifier | WORKGROUP , NULL, NULL, &TransmitImageSubsampleIncrement, 1, 1, TRANSMIT_IMAGE_SUBSAMPLE_MIN, TRANSMIT_IMAGE_SUBSAMPLE_MAX);

	AddNumber("040828", Two_DSubtagCommandModifier | WORKGROUP , NULL, NULL, &JPEGCompressionSmoothingFactor,
				JPEG_COMPRESSION_SMOOTHING_FACTOR_DEFAULT, JPEG_COMPRESSION_SMOOTHING_FACTOR_DEFAULT,
				JPEG_COMPRESSION_SMOOTHING_FACTOR_MINIMUM, JPEG_COMPRESSION_SMOOTHING_FACTOR_MAXIMUM);

	AddBoolean("040829", Two_DSubtagCommandModifier | WORKGROUP , NULL, NULL, &JPEGDecompressionBlockSmoothing, true, true);

	AddNumericList("04082A", Two_DCommandModifier | WORKGROUP , NULL, NULL, &TransmitImageFormat,
				TRANSMIT_IMAGE_FORMAT_JPEG, TRANSMIT_IMAGE_FORMAT_JPEG,
				TRANSMIT_IMAGE_FORMAT_KIM, TRANSMIT_IMAGE_FORMAT_TIF_BINARY, TRANSMIT_IMAGE_FORMAT_TIF_GRP,
				TRANSMIT_IMAGE_FORMAT_TIF_GRAYSCALE, TRANSMIT_IMAGE_FORMAT_BINARY, TRANSMIT_IMAGE_FORMAT_GRAYSCALE,
				TRANSMIT_IMAGE_FORMAT_JPEG, TRANSMIT_IMAGE_FORMAT_OUTLINED_IMAGE, TRANSMIT_IMAGE_FORMAT_BMP,
 				TRANSMIT_IMAGE_FORMAT_LOSSLESS_HUFFMAN);


	AddNumber("04082B", Two_DCommandModifier | WORKGROUP , NULL, NULL, &TransmitImageMargin,
				0, 0, 0, ((IMAGE_MAX_COL > IMAGE_MAX_ROW) ? ((IMAGE_MAX_ROW / 2) - 2) :  ((IMAGE_MAX_COL / 2) - 2)));

	AddBoolean("04082C", Two_DSubtagCommandModifier | WORKGROUP , NULL, NULL, &TransmitImageUseFastAlgorithmForJPEG, true, true);
	AddBoolean("04082D", Two_DCommandModifier | WORKGROUP  , NULL, NULL, &ImageFlattening, false, false);
	AddBoolean("04082E", Two_DCommandModifier | WORKGROUP , NULL, NULL, &ImageStretch, false, false);
	AddNumber("04082F", Two_DCommandModifier | WORKGROUP , NULL, NULL, &ImageEdgeSharpenAmount, 0, 0, 0, 23);
	AddNumber("040830", Two_DCommandModifier | WORKGROUP , NULL, NULL, &ImageRotationValue, 0, 0, 0, 3);

	AddBoolean("040831", Two_DCommandModifier | WORKGROUP , NULL, NULL, &FlySpecks, false, false);
    AddBoolean("040832", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP , NULL, NULL, &Snazzy, false, false);
	AddBoolean("040833", Two_DSubtagCommandModifier | LOCAL_SETTING , NULL, NULL, &GlitchFix, false, false);
    AddNumber("040834", Two_DSubtagCommandModifier | WORKGROUP , NULL, NULL, &RowNoise, 1, 1, 0, 1);
    AddNumber("040835", Two_DSubtagCommandModifier | WORKGROUP , NULL, NULL, &RowNoiseTopRatio, -16, -16, -127, 128);
    AddNumber("040836", Two_DSubtagCommandModifier | WORKGROUP , NULL, NULL, &RowNoiseBottomRatio, 14, 14, -127, 128);

	AddNumber("040837", Two_DCommandModifier | WORKGROUP , NULL, NULL, &GammaCorrection, 0, 0, 0, 1000);
	AddBoolean("040838", Two_DCommandModifier | WORKGROUP , NULL, NULL, &Histogram, false, false);
	AddNumber("040839", Two_DCommandModifier | WORKGROUP , NULL, NULL, &UltraText, 0, 0, 0, 255);
	AddBoolean("04083A", Two_DCommandModifier | WORKGROUP , NULL, NULL, &AdaptiveUltraText, false, false);
	AddBoolean("04083B", Two_DCommandModifier | WORKGROUP , NULL, NULL, &InfinityFilter, false, false);
	AddBoolean("04083C", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP , NULL, NULL, &ShipRawImage, false, false);
	AddBoolean("04083D", Two_DCommandModifier | WORKGROUP , &CheckIMGVGAMode, &UpdateImageShipParameters, &ShipImagesIn640x480VGAMode, false, false );
	AddNumber("040B00", Two_DCommandModifier | WORKGROUP , NULL, NULL, &ImageSnapAimerState, 0, 0, 0, 1);
	AddNumber("040B01", Two_DCommandModifier | WORKGROUP , NULL, NULL, &ImageSnapAcceptanceDelta, 25, 25, 1, 100);
	AddNumber("040B02", Two_DCommandModifier | WORKGROUP , NULL, NULL, &ImageSnapFrameRate, 30, 30, 1, 30);

	AddNumber("040B03", Two_DCommandModifier | WORKGROUP , NULL, NULL, &ImageSnapLedState, 0, 0, 0, 2);
	AddNumber("040B040", Two_DCommandModifier | WORKGROUP , NULL, NULL, &ImageSnapStyle, 1, 1, 0, 2);
	AddNumber("040B05", Two_DCommandModifier | WORKGROUP , NULL, NULL, &ImageSnapTriggerMode, 0, 0, 0, 1);

	AddNumber("040B06", Two_DCommandModifier | WORKGROUP , NULL, NULL, &ImageSnapAttempts, 6, 6, 0 ,10);
	AddNumber("040B07", Two_DCommandModifier | WORKGROUP , NULL, NULL, &ImageSnapWhiteTarget, 90, 90, 1, 255);

	AddNumber("040B08", Two_DCommandModifier | WORKGROUP , NULL, NULL, &ImageSnapExposure, BYD_INT_FIXED,BYD_INT_FIXED,BYD_INT_MIN,BYD_INT_MAX);
	AddNumber("040B09", Two_DCommandModifier | WORKGROUP , NULL, NULL, &ImageSnapGain, BYD_GAIN_FIXED,BYD_GAIN_FIXED,BYD_GAIN_MIN,BYD_GAIN_MAX);
	AddNumber("040B0A", Two_DCommandModifier | WORKGROUP , NULL, NULL, &ImageSnapBeep, 0, 0, 0, 1);
	
	AddNumber("040B0B", Two_DCommandModifier | WORKGROUP , NULL, NULL, &ImageSnapPercent, 50, 50, 1, 99);

	AddNumber("040B0C",SUB_TAG_TABLE::VISIBLE_BY_TAG | LOCAL_SETTING, NULL, NULL, &ImageSnapMustConform, 1,1,0,1);

   	AddNumber("04083E", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP , NULL, NULL, &ImageQualityRanking, 0,0,0,1000);
   	AddNumber("04083F", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP , NULL, NULL, &ImageQualitySkip, 1,1,1,16);

   	//For Manual triggers
   	AddNumber("040840", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP , NULL, NULL, &ImageQualityMode, 1,1,0,1000);
   	//For Streaming Presentation Trigger
   	AddNumber("040303", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP , NULL, NULL, &PresentationImageQualityMode, 1,1,0,1000);
   	AddNumber("040304", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP , NULL, NULL, &IQSenseSensitivity, 14,14,0,100);
	
   	AddBoolean ("040C00", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL, NULL, &ImageQualityCellPhone, false, false);


	//Heat Mitigation
	if( (ControlMask & MAKE_HEAT_MITIGATION_FIXED_SETTINGS_FIXED) && (ControlMask & MAKE_HEAT_MITIGATION_AVAILABLE) )  //enabled in standard build
	{
		AddBoolean("040D00", Two_DCommandModifier | WORKGROUP | HeatMitigationFixedSettingsCommandModifier, NULL, NULL, &BatchedImagingMode, true, true);  	// Heat Mitigation, true == on
		AddNumber("040D01", Two_DCommandModifier | WORKGROUP, NULL, NULL, &BatchedImagingLeds, 2, 2, 0, 2);  		// Heat Mitigation--> 0 = lights off during decoding, 1 = Ill on, 2 = aimer on
	}
	else if( (ControlMask & MAKE_HEAT_MITIGATION_AVAILABLE) )   //enabled in engineering build
	{
		AddBoolean("040D00", Two_DCommandModifier | WORKGROUP, NULL, NULL, &BatchedImagingMode, false, false);  	// Heat Mitigation, true == on
		AddNumber("040D01", Two_DCommandModifier | WORKGROUP, NULL, NULL, &BatchedImagingLeds, 2, 2, 0, 2);  		// Heat Mitigation--> 0 = lights off during decoding, 1 = Ill on, 2 = aimer on
	}
	else      //disabled in product config
	{
		AddBoolean("040D00", Two_DCommandModifier | WORKGROUP | SUB_TAG_TABLE::FIXED, NULL, NULL, &BatchedImagingMode, false, false);  	// Heat Mitigation, true == on
		AddNumber("040D01", Two_DCommandModifier | WORKGROUP | SUB_TAG_TABLE::FIXED, NULL, NULL, &BatchedImagingLeds, 2, 2, 0, 2);  		// Heat Mitigation--> 0 = lights off during decoding, 1 = Ill on, 2 = aimer on
	}

	AddNumber("040E00", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | SUB_TAG_TABLE::FIXED | WORKGROUP , NULL, NULL, &ImagePresentationExposure, BYD_INT_MAX,BYD_INT_MAX,BYD_INT_MIN,BYD_INT_MAX);

   	AddBoolean("040F00", SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP , NULL, NULL, &Filter3x3Sign, false, false);
	AddNumber("040F01", SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP , NULL, NULL, &Filter3x3ShiftingCoefficient, 0,0,0,12);
	AddNumber("040F02", SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP , NULL, NULL, &Filter3x3Coefficient[0], 0,0,0,255);
	AddNumber("040F03", SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP , NULL, NULL, &Filter3x3Coefficient[1], 0,0,0,255);
	AddNumber("040F04", SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP , NULL, NULL, &Filter3x3Coefficient[2], 0,0,0,255);
	AddNumber("040F05", SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP , NULL, NULL, &Filter3x3Coefficient[3], 0,0,0,255);
	AddNumber("040F06", SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP , NULL, NULL, &Filter3x3Coefficient[4], 1,1,0,255);
	AddNumber("040F07", SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP , NULL, NULL, &Filter3x3Coefficient[5], 0,0,0,255);
	AddNumber("040F08", SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP , NULL, NULL, &Filter3x3Coefficient[6], 0,0,0,255);
	AddNumber("040F09", SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP , NULL, NULL, &Filter3x3Coefficient[7], 0,0,0,255);
	AddNumber("040F0A", SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP , NULL, NULL, &Filter3x3Coefficient[8], 0,0,0,255);
	AddNumber("040F0B", SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP , NULL, NULL, &Filter3x3Config, 0,0,0,2); // 0=DO NOT over-ride, 1=Over-ride I2C table with above values,2=Over-ride with debug

	AddBoolean("041000", SUB_TAG_TABLE::VISIBLE_BY_TAG  | WORKGROUP, NULL, NULL, &ScanDriverCellPhone, false, false);  	// Cell phone config for Scan Driver specific
	//For Manual triggers
	AddBoolean("040305", SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP, NULL, NULL, &PresentationScanDriverCellPhone, false, false);
	AddNumber("040306",SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP ,NULL,NULL,&PresentationFixedExposure,BYD_INT_FIXED,BYD_INT_FIXED,BYD_INT_MIN,BYD_INT_MAX);
	AddNumber("040307",SUB_TAG_TABLE::VISIBLE_BY_TAG | WORKGROUP ,NULL,NULL,&PresentationFixedGain,BYD_GAIN_FIXED,BYD_GAIN_FIXED,BYD_GAIN_MIN,BYD_GAIN_MAX);

    AddNumber("041100",SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP, NULL,NULL,&AimerOnTimeforCellPhoneMode,5000,5000,0,0x7fffffff);
	
	return;
}

		
#else 	// else #ifdef BYD_SENSOR

#endif



