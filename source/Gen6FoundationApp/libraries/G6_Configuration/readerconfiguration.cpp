//======================================================================================
// ReaderConfiguration.cpp
//======================================================================================
// $RCSfile: readerconfiguration.cpp $
// $Revision: 1.34 $
// $Date: 2011/11/18 06:51:31EST $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Class Name:     READER_CONFIGURATION
//
//  Class Description:
//      READER_CONFIGURATION contains the data fields and methods needed to manage
//      HHP reader configuration data.
//
//		Note that some definitions are surrounded with #if(DLL < 1).  The purpose of this is
//		to prevent that code from being included in a Windows build of this class, since the
//		external stuff required for the #if blocked code will not be present for a Windows build.
//
//---------------------------------------------------------------------------


#include	<stdio.h>		/* standard I/O functions					*/
#include	<string.h>		/* string functions							*/

#include	"stdInclude.h"
#include	"Beep.h"
#include	"db_chrio.h"
#include	"ReaderConfiguration.h"			// READER_CONFIGURATION interface definition
#include	"ProductConfiguration.h"
#include	"menu.h"
#include	"db_hdw.h"

#ifndef RFBASE
#include	"scandrivermatrixinterface.h"
#include "ScanEnginePSOC.h"
#include "MenuFlash.h"

#include "hardwaretoken.h"
#include "Interface_Support_Functions.h"

//#include	"ScanEnginePsoc.h"
//#include	"ScanDriverEngine.h"  // for PSOC register locations
#else
#endif




extern void FillInProductName(void);
extern volatile unsigned nOpenCntxt; 							// "open context" for the scan driver" opened in main called elswhere.  Declared in vars.c.
extern bool UpdateImageShipParameters( void );					// Declared in ImagingMenuSettings.cpp.
extern bool ProcessEngineIlluminationColorChange( void );		// Declared in ImagingMenuSettings.cpp.
//extern bool ProcessEngineFirmwareDiagnosticModeChange( void );	// Declared in ImagingMenuSettings.cpp.
//extern void FillInProductName(void);


#if !defined( RFBASE ) && !defined( RFSCAN )
extern bool ChangeIlluminationAlwaysOnState( void );                       // Declared in ImagingMenuSettings.cpp.
#endif

//
//	Interface to routines that create various menu settings groups.
//
#include "DecoderMenuSettings.h"
#include "DiagnosticAndDebugMenuSettings.h"
#include "ImagingMenuSettings.h"
#include "IndicatorMenuSettings.h"
#include "InterfaceMenuSettings.h"
#include "OutputMenuSettings.h"
#include "PlugAndPlayMenuSettings.h"
#include "ScriptingMenuSettings.h"
#include "SystemMenuSettings.h"

DEFERRED_RESET_STATE_t deferred_reset_state;

bool deferred_reset()
{
	if (deferred_reset_state == DEFERRED_RESET_ALLOWED)
		deferred_reset_state = DEFERRED_RESET_PENDING;
	return true;
}



#ifdef PRODUCT_CONFIGURATION_APPLICATION

RDR_CFG_FOR_PRODUCT_CFG		*ReaderConfiguration;
#else

READER_CONFIGURATION		*ReaderConfiguration;
#endif




//
//	Initialize
//      Initialize all menu settings.
//
void READER_CONFIGURATION::Initialize(int ControlMask1)
{
	// Setup control mask information
	UINT LinearSymbologyCommandModifier;
	UINT CodeZSymbologyCommandModifier;
	UINT VericodeSymbologyCommandModifier;
	UINT BC412SymbologyCommandModifier;
	UINT SecureCodeSymbologyCommandModifier;
	UINT Two_DCommandModifier;
	UINT Two_DTagCommandModifier;
	UINT Two_DSubtagCommandModifier;
	UINT ImageShippingCommandModifier;
	UINT ScriptingCommandModifier;                                                 
	UINT AimerCommandModifier;
	UINT DpcCirrusSymbologyCommandModifier;
	UINT GmxMatrixSymbologyCommandModifier;
	UINT HeatMitigationFixedSettingsCommandModifier;

	
	LinearSymbologyCommandModifier = (ControlMask1 & MAKE_LINEAR_SYMBOLOGIES_AVAILABLE) ? SUB_TAG_TABLE::FULLY_VISIBLE : NULL;
	CodeZSymbologyCommandModifier = (ControlMask1 & MAKE_CODE_Z_SYMBOLOGY_AVAILABLE) ? SUB_TAG_TABLE::FULLY_VISIBLE : NULL;
	VericodeSymbologyCommandModifier = (ControlMask1 & MAKE_VERICODE_SYMBOLOGY_AVAILABLE) ? SUB_TAG_TABLE::FULLY_VISIBLE : NULL;
	BC412SymbologyCommandModifier = (ControlMask1 & MAKE_BC_412_SYMBOLOGY_AVAILABLE) ? SUB_TAG_TABLE::FULLY_VISIBLE: NULL;
	SecureCodeSymbologyCommandModifier = (ControlMask1 & MAKE_SECURECODE_SYMBOLOGY_AVAILABLE) ? SUB_TAG_TABLE::FULLY_VISIBLE: NULL;
	DpcCirrusSymbologyCommandModifier = (ControlMask1 & MAKE_DPC_CIRRUS_SYMBOLOGY_AVAILABLE) ? SUB_TAG_TABLE::FULLY_VISIBLE : NULL;
	GmxMatrixSymbologyCommandModifier = (ControlMask1 & MAKE_GRID_MATRIX_SYMBOLOGY_AVAILABLE) ? SUB_TAG_TABLE::FULLY_VISIBLE : NULL;

	Two_DCommandModifier = (ControlMask1 & MAKE_2D_COMMANDS_AVAILABLE) ? SUB_TAG_TABLE::FULLY_VISIBLE : NULL;
	Two_DTagCommandModifier = (ControlMask1 & MAKE_2D_COMMANDS_AVAILABLE) ? SUB_TAG_TABLE::VISIBLE_BY_TAG : NULL;
	Two_DSubtagCommandModifier = (ControlMask1 & MAKE_2D_COMMANDS_AVAILABLE) ? SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG : NULL;
	ImageShippingCommandModifier = (ControlMask1 & MAKE_IMAGE_SHIPPING_AVAILABLE) ? SUB_TAG_TABLE::FULLY_VISIBLE : NULL;
	ScriptingCommandModifier = (ControlMask1 & MAKE_SCRIPTING_AVAILABLE) ? SUB_TAG_TABLE::FULLY_VISIBLE : SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG;                //lmc   fixme
	AimerCommandModifier = (ControlMask1 & MAKE_AIMER_COMMANDS_AVAILABLE) ? SUB_TAG_TABLE::FULLY_VISIBLE : NULL;

	HeatMitigationFixedSettingsCommandModifier = (ControlMask1 & MAKE_HEAT_MITIGATION_FIXED_SETTINGS_FIXED) ? ( SUB_TAG_TABLE::FIXED ) : NULL;

	//----------------------------------- Start Menu Entries -------------------------------------------
	CreateDecoderMenuSettings( ControlMask1, BC412SymbologyCommandModifier, CodeZSymbologyCommandModifier, DpcCirrusSymbologyCommandModifier,
			SecureCodeSymbologyCommandModifier,	Two_DCommandModifier, VericodeSymbologyCommandModifier, GmxMatrixSymbologyCommandModifier,
			LinearSymbologyCommandModifier );

	CreateCPUPowerSpeedMenuSettings( ControlMask1 );

	CreateDiagnosticAndDebugMenuSettings( ControlMask1 );

	CreateImagingMenuSettings( ControlMask1, AimerCommandModifier, ImageShippingCommandModifier, Two_DCommandModifier, Two_DTagCommandModifier,
							Two_DSubtagCommandModifier, HeatMitigationFixedSettingsCommandModifier);

	CreateIndicatorMenuSettings( ControlMask1 );
	CreateInterfaceMenuSettings( ControlMask1 );
	CreateOutputMenuSettings( ControlMask1 );
	CreatePlugAndPlayMenuSettings( ControlMask1 );


	CreateScriptingMenuSettings( ControlMask1, ScriptingCommandModifier );
	CreateSystemMenuSettings( ControlMask1 );
	CreateLicenseMenuSettings( ControlMask1 );
	CreateTriggeringMenuSettings( ControlMask1, Two_DCommandModifier, HeatMitigationFixedSettingsCommandModifier );


#ifdef PLUGIN_DEV
	CreatePluginMenuSettings(ControlMask1);
#endif

	return;
} // Initialize



//
// Constructor for READER_CONFIGURATION
//      Initialize the default edit format and copy defaults to start with.
//		Zero out application code version numbers to indicate that they haven't
//		been set yet.  Since they're reader code specific, we will make no
//		assumptions about what they should be.  Set the configuration version as well.
//		Indicate that there are no known manufacturing settings yet.
//
//		We use the control mask sent by the caller to enable / disable symbologies.
//
READER_CONFIGURATION::READER_CONFIGURATION(int ControlMask1)
		: READER_CONFIGURATION_STORAGE( NUMBER_OF_WORKGROUPS )
{
	FoundValidWorkingConfigurationInFlash = false;
	FoundValidWorkingConfiguration0InFlash =false;
	FoundValidWorkingConfiguration1InFlash = false;
	ImagerIsRunning = false;		// Let entry / exit functions know that it is not yet safe to access the imager.

	Initialize( ControlMask1 );

	return;
} // Constructor





//
// Destructor for READER_CONFIGURATION
//      Nothing to do at this point.
//
READER_CONFIGURATION::~READER_CONFIGURATION(void)
{

	return;
} // Destructor






//
// ReconcileSettingsAfterHardwareInitializationOrDefaultCommand
//      Once hardware is initialized (especially the imager), or settings have been defaulted, update settings whose ranges or values need
//		to be modified based on hardware configuration or are dependent upon other settings.
//
void READER_CONFIGURATION::ReconcileSettingsAfterHardwareInitializationOrDefaultCommand( void )
{
#ifndef RFBASE

	// Update image ship parameters.
	UpdateImageShipParameters();

	if(HardwareToken.IAmOEM())
	{
		UpdateImageTransferSettings();	// Update image transfer settings based on interface type.
	}

	// Update engine illumination color.
	ProcessEngineIlluminationColorChange();

	// Update engine diagnostic mode.
	//ProcessEngineFirmwareDiagnosticModeChange();




#if !defined( RFSCAN )
	// In case the unit was defaulted, update ILLAON state.
	ChangeIlluminationAlwaysOnState();
#endif

#endif		//end #ifndef RFBASE

	return;
} // ReconcileSettingsAfterHardwareInitializationOrDefaultCommand




#define EXPEST_MT9V022	60





//
// ReconcileSettingsAfterHardwareInitialization
//      Once hardware is initialized (especially the imager), update settings whose ranges or values need to be modified based on hardware
//		configuration.
//
void READER_CONFIGURATION::ReconcileSettingsAfterHardwareInitialization( void )
{
#ifndef RFBASE

	ImagerIsRunning = true;		// Let entry / exit functions know that it is safe to access the imager.

	int nEngineType,nSize;
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_ENGINE_TYPE,NULL,0,(PBYTE)&nEngineType,0,(PDWORD)&nSize);

	bool bHaveLightBar = false;
	//DWORD GetEngineId[2] = {HHPSD_ENGINE_ID,0};  // 1st DWORD has tag, 2nd DWORD will be filled in with the value        //lmc   fixme

	if ( nOpenCntxt )
	{
		// Fix the range on the IMGCTR and IMGCTX settings.
		DWORD nHeight,nWidth;

		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&nHeight,4,NULL);
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&nWidth,4,NULL);
		ProductConfiguration->ChangeNumericRange("0F1A00", 0, nHeight);
		ProductConfiguration->ChangeNumericRange("0F1A01", 0, nWidth);

			// If IAmOEM, update the aimer center
		if( HardwareToken.IAmOEM() && ImageEngineHasPsoc() ) {
			DWORD	GetImagerPropertiesRequest[ 4 ] = { IP_AIMER_X_OFFSET, 0, IP_AIMER_Y_OFFSET, 0 };

			SCN_IOControl( nOpenCntxt, HHPSD_IOCTL_GET_IMAGER_PROPERTIES, (PBYTE) GetImagerPropertiesRequest, sizeof( GetImagerPropertiesRequest ),
				(PBYTE) GetImagerPropertiesRequest, sizeof( GetImagerPropertiesRequest ), NULL ); 

			if((GetImagerPropertiesRequest[1] != GET_CONFIG_SETTING(iImageXCenter)) )
			{
				if(!GET_SETTING( ProductConfigurationUpdatePrivilege ))
					ProductConfiguration->EnableSettingChanges( true );
				char CenterStr[16];
				HSTRING menuc("0F1A01",true);
				sprintf(CenterStr, "%d", GetImagerPropertiesRequest[1]);
				menuc.Append(CenterStr, strlen(CenterStr));
				menuc.AppendChar('.');
				ProductConfiguration->Menu(&menuc);
				StoreProductConfigurationToFlash( true );
				ProductConfiguration->WorkingValuesUpToDate();
				if(!GET_SETTING( ProductConfigurationUpdatePrivilege ))
					ProductConfiguration->EnableSettingChanges(false);
			}

			if((GetImagerPropertiesRequest[3] != GET_CONFIG_SETTING(iImageYCenter)))
			{
				if(!GET_SETTING( ProductConfigurationUpdatePrivilege ))
					ProductConfiguration->EnableSettingChanges( true );
				char CenterStr[16];
				HSTRING menuc("0F1A00",true);
				sprintf(CenterStr, "%d", GetImagerPropertiesRequest[3]);
				menuc.Append(CenterStr, strlen(CenterStr));
				menuc.AppendChar('.');
				ProductConfiguration->Menu(&menuc);
				StoreProductConfigurationToFlash( true );
				ProductConfiguration->WorkingValuesUpToDate();
				if(!GET_SETTING( ProductConfigurationUpdatePrivilege ))
					ProductConfiguration->EnableSettingChanges(false);
			}

		}


		// Update ENGTYP with image engine type.  If this is a 5000 VGA, update the defaults for some "EXP" settings and flash if we're supposed to.
		ReaderConfiguration->ChangeNumericRange("0F2201", nEngineType, nEngineType );
	}


	if( GET_SETTING( ImageEngineType ) == HHPSD_ENGINE_IMAGER_MICRON_MT9V022 )
	{
		ReaderConfiguration->ChangeDefault( "040201", 2, false, !FoundValidWorkingConfigurationInFlash );
		ReaderConfiguration->ChangeDefault( "040203", 420, false, !FoundValidWorkingConfigurationInFlash );
		ReaderConfiguration->ChangeDefault( "040204", 2, false, !FoundValidWorkingConfigurationInFlash );
		ReaderConfiguration->ChangeDefault( "EXPDGN", 1, false, !FoundValidWorkingConfigurationInFlash );
		ReaderConfiguration->ChangeDefault( "040206", EXPEST_MT9V022, false, !FoundValidWorkingConfigurationInFlash );
		ReaderConfiguration->ChangeDefault( "040207", 90, false, !FoundValidWorkingConfigurationInFlash );

		ReaderConfiguration->ChangeDefault( "040906", AIM_ALT, false, !FoundValidWorkingConfigurationInFlash );
		//	if (!(CHal::WhatBoardAmI() >= IT5080_2_TOKEN && CHal::WhatBoardAmI() <IT4080_LAST_TOKEN))
		//		ReaderConfiguration->ChangeDefault( "IMGGFX", true, false, !FoundValidWorkingConfigurationInFlash );

//		RESET_WATCHDOG_TIMER();

		// Adjust for 5100 with light bar
		if(0) //( ImageEngineHasPsoc() )                                       //lmc   fixme
		{	// We have a PSOC, check its engine ID to see if it's one of interest
		/*                                                                          //lmc   fixme
			if ( SCN_IOControl(nOpenCntxt, HHPSD_IOCTL_GET_ENGINE_CONFIG, (PBYTE)GetEngineId, sizeof GetEngineId, NULL, 0, NULL) )
			{
				if ( (GetEngineId[1] == ENGINE_ID_5100_LED_OFF_AXIS_LIGHT_BAR_SF) ||
				        (GetEngineId[1] == ENGINE_ID_5100_LED_OFF_AXIS_LIGHT_BAR_SR) ||
				        (GetEngineId[1] == ENGINE_ID_5100_LED_OFF_AXIS_LIGHT_BAR_HD) )
				{
					bHaveLightBar = true;

					extern void EngineHasOffAxisIllum(BOOL bHasIt);
					EngineHasOffAxisIllum(TRUE);
					// Concurrent aimer & illumination not allowed.
					int nListScnAimLightBar[] = {AIM_OFF, AIM_ALT};
					ReaderConfiguration->ChangeDefault("SCNAIM", AIM_ALT, false, ! FoundValidWorkingConfigurationInFlash);
					ReaderConfiguration->ChangeNumericList("SCNAIM", nListScnAimLightBar, sizeof nListScnAimLightBar/sizeof(int) ,1);

					// Limit exposer to 8ms. (more efficient LEDs)
					const int MaxExposure = 63;  // 8 ms
					// Since the default EXPEST setting for HHPSD_ENGINE_IMAGER_MICRON_MT9V022 was changed to a suitable value (see above), we'll use this value as well
					ReaderConfiguration->ChangeDefault("EXPEST", EXPEST_MT9V022, false, ! FoundValidWorkingConfigurationInFlash);
					ReaderConfiguration->ChangeNumericRange("EXPEST", 1, MaxExposure);
					ReaderConfiguration->ChangeNumericRange("EXPEMX", 1, MaxExposure);

					// in cases where the illumination is off, we wanted SNPEXP to be configurable up to the true maximum of 1 second, so we exception handle the
					// maximum exposure at the time when image capture is initiated.
					//				ReaderConfiguration->ChangeDefault("SNPEXP", MaxExposure, false, ! FoundValidWorkingConfigurationInFlash);
					//				ReaderConfiguration->ChangeNumericRange("SNPEXP", 1, MaxExposure);
				}
			}
		*/                                                        //lmc   fixme
		}   //END Adjust for 5100 with light bar

		if(!bHaveLightBar)
		{
			int nList[] = {TRGMOD_MANUAL, TRGMOD_MANUAL_LO, TRGMOD_PRESENT,TRGMOD_SSTAND, TRGMOD_NOTIFY_HOST, TRGMOD_SNAP_AND_SHIP};
			ReaderConfiguration->ChangeNumericList("0E0100",&nList[0],sizeof(nList)/sizeof(int),1);
		}
	}
	else if( (GET_SETTING( ImageEngineType ) == HHPSD_ENGINE_IMAGER_STM_VC602) || (GET_SETTING( ImageEngineType ) == HHPSD_ENGINE_IMAGER_STM_VC700) )
	{
		ReaderConfiguration->ChangeNumericList("040201",1,2,4,6,8,16,32,64);
		ReaderConfiguration->ChangeNumericList("040204",1,2,4,6,8,16,32,64);
		ReaderConfiguration->ChangeDefault( "040201", 6, false, !FoundValidWorkingConfigurationInFlash );
		ReaderConfiguration->ChangeDefault( "040204", 6, false, !FoundValidWorkingConfigurationInFlash );
		ReaderConfiguration->ChangeDefault( "040205", 8, false, !FoundValidWorkingConfigurationInFlash );
		ReaderConfiguration->ChangeDefault( "040202", 8, false, !FoundValidWorkingConfigurationInFlash );
		ReaderConfiguration->ChangeDefault( "040203", 400, false, !FoundValidWorkingConfigurationInFlash );
		ReaderConfiguration->ChangeDefault( "040207", 90, false, !FoundValidWorkingConfigurationInFlash );
		ReaderConfiguration->ChangeDefault( "024D16", 1, false, !FoundValidWorkingConfigurationInFlash );

		//signed char t,b;                                                              //lmc   fixme

		if(0)// (GetRowNoiseCorrectionValues(&t,&b))                                      //lmc   fixme
		{
			//ReaderConfiguration->ChangeDefault ("IMGRNT",t,true,true);                           //lmc   fixme
			ReaderConfiguration->SetFixedAttribute ("040835");
			//ReaderConfiguration->ChangeDefault ("IMGRNB",b,true,true);                          //lmc   fixme
			ReaderConfiguration->SetFixedAttribute ("040836");
		}
	}

	if(0) //( (GetEngineId[1] != ENGINE_ID_5100_LED_OFF_AXIS_LIGHT_BAR_SF) &&                           //lmc   fixme
	        //(GetEngineId[1] != ENGINE_ID_5100_LED_OFF_AXIS_LIGHT_BAR_SR) &&
	        //(GetEngineId[1] != ENGINE_ID_5100_LED_OFF_AXIS_LIGHT_BAR_HD) &&
	        //(nHeight > 1) )
	{
#if defined(RFSCAN)
#else

		int nList[] = {TRGMOD_MANUAL, TRGMOD_MANUAL_LO, TRGMOD_PRESENT,TRGMOD_SSTAND, TRGMOD_NOTIFY_HOST, TRGMOD_SNAP_AND_SHIP};
#endif

		ReaderConfiguration->ChangeNumericList("0E0100",&nList[0],sizeof(nList)/sizeof(int),1);
	}

	// Update the working values for IMGCTR and IMGCTX based on scan driver configuration.  What actually happens here is that the scan driver
	// uses the product configuration for aimer center by default, but if the engine has a PSOC, and if those register values are valid, then
	// the scan driver uses the engine PSOC aimer center settings instead, which we'll use to update the product configuration.

	// For now, only do this if this is a 5000.  The ICMEDIA and 5000 scan drivers are the only ones that support this request.
	if(0)//(ImageEngineHasPsoc())                                              //lmc   fixme
	{
		DWORD	GetImagerPropertiesRequest[ 4 ] = { IP_AIMER_X_OFFSET, 0, IP_AIMER_Y_OFFSET, 0 };
		DWORD	GetImagerPropertiesReturnedInformation[ 4 ];

		if( SCN_IOControl( nOpenCntxt, HHPSD_IOCTL_GET_IMAGER_PROPERTIES, (PBYTE) GetImagerPropertiesRequest, sizeof( GetImagerPropertiesRequest ),
		                   (PBYTE) GetImagerPropertiesReturnedInformation, sizeof( GetImagerPropertiesReturnedInformation ), NULL ) )
		{
			// Save the state of the ModifiedSinceInstantiation field for IMGCTX and IMGCTR.  If these haven't been modified already, we want to mark them
			// as unmodified when we're done with this update, so that they are not stored permanently to flash if and when a product configuration write
			// to flash is performed.  In other words, we don't want these values stored to flash if they weren't already in flash.  Keep in mind that
			// when flashing product configuration, we only flash the settings that were actually changed by the user.
			SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY			*AimerCenterXSettingEntry;
			SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*AimerCenterXWorkingValue = 0;	// Indicate that this doesn't point to anything yet.
			SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY			*AimerCenterYSettingEntry;
			SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*AimerCenterYWorkingValue = 0;

			bool		AimerCenterXHasBeenModifiedSinceInstantiation = false;
			bool		AimerCenterYHasBeenModifiedSinceInstantiation = false;

			if( ProductConfiguration->GetSubTagTableEntry( "0F1A01", &AimerCenterXSettingEntry ) )
			{
				AimerCenterXWorkingValue = WorkingValueForCurrentWorkgroup( AimerCenterXSettingEntry );
				AimerCenterXHasBeenModifiedSinceInstantiation = AimerCenterXWorkingValue->ModifiedSinceInstantiation;
			}

			if( ProductConfiguration->GetSubTagTableEntry( "0F1A00", &AimerCenterYSettingEntry ) )
			{
				AimerCenterYWorkingValue = WorkingValueForCurrentWorkgroup( AimerCenterYSettingEntry );
				AimerCenterYHasBeenModifiedSinceInstantiation = AimerCenterYWorkingValue->ModifiedSinceInstantiation;
			}

			HSTRING		AimerCenterCommandString( 80, true );
			sprintf( (char *) AimerCenterCommandString.Char, "0F1A01%d;0F1A00%d.", (int) GetImagerPropertiesReturnedInformation[ 1 ],
			         (int) GetImagerPropertiesReturnedInformation[ 3 ] );
			AimerCenterCommandString.Size = strlen( (char *) AimerCenterCommandString.Char );
			ProductConfiguration->Menu( &AimerCenterCommandString, true );

			// Restore ModifiedSinceInstantiation fields if we found an entry for them.
			if( AimerCenterXWorkingValue )
				AimerCenterXWorkingValue->ModifiedSinceInstantiation = AimerCenterXHasBeenModifiedSinceInstantiation;

			if( AimerCenterYWorkingValue )
				AimerCenterYWorkingValue->ModifiedSinceInstantiation = AimerCenterYHasBeenModifiedSinceInstantiation;

			ProductConfiguration->WorkingValuesUpToDate();
		}
	}


	// If scan engine has PSOC and product configuration SERNUM is default (XXXXXX), use the serial number in the engine PSOC.
	if(0)//( ImageEngineHasPsoc() )                                               //lmc   fixme
	{
		//PSoC_5100_REGISTER_TABLE	Registers;                                       //lmc   fixme

		if(0)//( Psoc5100GetRegisters( &Registers ) )                                 //lmc   fixme
		{
			HSTRING		ASCIIUSBSerialNumber( GET_SETTING( USBSerNum ), true );           //The USB serial number
			ASCIIUSBSerialNumber.UniCodeToSimpleAsciiFormat();

			HSTRING		ASCIIConfigSerialNumber( GET_CONFIG_SETTING( SerNum ), true );    //The Product config serial number
			ASCIIConfigSerialNumber.UniCodeToSimpleAsciiFormat();

			//If ( SER_NUM (=="XXXXXX") is equiv to USB serial number ) OR
			//( Config ser number equiv to SER_NUM (=="XXXXXX") AND USB ser num not equiv to to imager serial num )
			if(0)//( (ASCIIUSBSerialNumber.Compare( SER_NUM, true ) == 0) || ((ASCIIConfigSerialNumber.Compare( SER_NUM, true ) == 0) && (ASCIIUSBSerialNumber.Compare( &hsImagerSerialNumber, true ) != 0)) )         //lmc   fixme
			{
				HSTRING		SerialNumberCommandString( 80, true );
				SerialNumberCommandString.Copy( "060A07" );
				SerialNumberCommandString.Append( "." );
				Menu( &SerialNumberCommandString );
			}
		}
	}

	//SetFixedAttribute( "USBSRN" );         //must lock this setting in all cases                        //lmc   fixme




	// If scan engine has PSOC, adjust ENGILL range if no secondary illumination is present.
	if(0)//( ImageEngineHasPsoc() )                                                  //lmc   fixme
	{
		//PSoC_5100_REGISTER_TABLE	Registers;                                         //lmc   fixme

		if(0)//( Psoc5100GetRegisters( &Registers ) )                                      //lmc   fixme
		{
			if(0)//( Registers.EngineID.b.SecondaryIllumination == PSOC_5100_SECONDARY_ILLUMINATION_NONE )
			{
				//ReaderConfiguration->ChangeNumericList( "ENGILL", PSOC_5100_PRIMARY_ILUMINATION );             //lmc   fixme
			}
		}
	}


	// Update IMGVAR default to be true for 5000 VGA.
	if( GET_SETTING( ImageEngineType ) == HHPSD_ENGINE_IMAGER_MICRON_MT9V022 )
	{
		ReaderConfiguration->ChangeDefault( "04083D", true, false, !FoundValidWorkingConfigurationInFlash );
//		RESET_WATCHDOG_TIMER();
	}

	// Reconcile settings that also have to be updated whenever a DEFALT or a tag DFT is performed.
	ReconcileSettingsAfterHardwareInitializationOrDefaultCommand();

#endif	//not defined rf base

	// Now that we know engine type, fill in the product name.
	FillInProductName();

	return;
} // ReconcileSettingsAfterHardwareInitialization






//
//	ProcessError
//		Handle RCS errors through the virtual function ProcessError.
//
void READER_CONFIGURATION::ProcessError( int RCSError, const char *SettingName )
{
	char	temp[ 100 ];

	switch( RCSError )
	{
	case SUB_TAG_TABLE::ADD_BASIC_INFORMATION_ILLEGAL_CHARACTERS_IN_NAME:
		sprintf( temp, "Setting name %s contains illegal characters and was not created\r\n", SettingName );
		break;

	case SUB_TAG_TABLE::ADD_BASIC_INFORMATION_NAME_ALREADY_EXISTS:
		sprintf( temp, "Setting name %s already exists and was not created\r\n", SettingName );
		break;

	case SUB_TAG_TABLE::ADD_BASIC_INFORMATION_ILLEGAL_FLAG_OR_MODIFIER_COMBINATION:
		sprintf( temp, "Setting name %s creation contains illegal modifier combination (such as both base and scanner only)\r\n", SettingName );
		break;

	default:
		sprintf( temp, "Unable to create setting %s\r\n", SettingName );
		break;
	};

#ifndef EMERALD_VERIFIER

	//xputstring( temp );
	//		Beep( true, 100, 1000 );
#endif

	return;
} // ProcessError













