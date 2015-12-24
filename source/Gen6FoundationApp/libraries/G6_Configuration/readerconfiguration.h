//======================================================================================
// ReaderConfiguration.h
//======================================================================================
// $RCSfile: readerconfiguration.h $
// $Revision: 1.11 $
// $Date: 2011/02/18 11:45:04EST $
//======================================================================================

//======================================================================================




// Process for inserting new configuration entries:
//

//---------------------------------------------------------------------------
//
//  Class definitions for ReaderConfiguration
//
//---------------------------------------------------------------------------
#ifndef ReaderConfigurationH
#define ReaderConfigurationH
//---------------------------------------------------------------------------


#include	"language.h"					// system global definitions
#include	"ReaderConfigurationStorage.h"


//
//	Interface to various menu settings groups.
//



#define	NUMBER_OF_RF_WORKGROUPS			7

#ifdef RFBASE
#define	WORKGROUP				SUB_TAG_TABLE::WORKGROUP_SETTING
#define	BASE_ONLY				SUB_TAG_TABLE::BASE_ONLY_SETTING
#define	NUMBER_OF_WORKGROUPS			NUMBER_OF_RF_WORKGROUPS
#else
#define	WORKGROUP				0
#define	BASE_ONLY				0
#define	NUMBER_OF_WORKGROUPS			1
#endif


#ifdef RFBASE
#define	LOCAL_SETTING				SUB_TAG_TABLE::BASE_ONLY_SETTING
#else
	#ifdef RFSCAN
	#define	LOCAL_SETTING			SUB_TAG_TABLE::SCANNER_ONLY_SETTING
	#else
	#define	LOCAL_SETTING			0
	#endif
#endif





#define	MAKE_HIDDEN_UNTIL_LICENSED					SUB_TAG_TABLE::FULLY_VISIBLE
//#define	MAKE_HIDDEN_UNTIL_LICENSED					0x00000000
//#define	MAKE_MATRIX_SYMBOLOGIES_AVAILABLE			0x00000001
//#define	MAKE_POSTAL_SYMBOLOGIES_AVAILABLE			0x00000002
//#define	MAKE_STACKED_LINEAR_SYMBOLOGIES_AVAILABLE	0x00000004
#define	MAKE_LINEAR_SYMBOLOGIES_AVAILABLE			0x00000008
#define	MAKE_CODE_Z_SYMBOLOGY_AVAILABLE				0x00000010
#define	MAKE_VERICODE_SYMBOLOGY_AVAILABLE			0x00000020
#define	MAKE_BC_412_SYMBOLOGY_AVAILABLE				0x00000040
//#define	MAKE_OCR_AVAILABLE							0x00000080
#define	MAKE_2D_COMMANDS_AVAILABLE					0x00000100
#define	MAKE_KB_WEDGE_DEFAULT						0x00000200
#define	MAKE_IMAGE_SHIPPING_AVAILABLE		  		0x00000400
#define	MAKE_SECURECODE_SYMBOLOGY_AVAILABLE			0x00000800
#define MAKE_SCRIPTING_AVAILABLE					0x00001000
#define	MAKE_DPC_CIRRUS_SYMBOLOGY_AVAILABLE			0x00002000
#define	MAKE_AIMER_COMMANDS_AVAILABLE				0x00004000
#define	MAKE_USB_KB_WEDGE_DEFAULT					0x00008000
#define	MAKE_USB_COMM_EMULATION_DEFAULT				0x00010000
#define	MAKE_HEAT_MITIGATION_FIXED_SETTINGS_FIXED	0x00020000
#define	MAKE_HEAT_MITIGATION_AVAILABLE		 		0x00040000
#define	MAKE_GRID_MATRIX_SYMBOLOGY_AVAILABLE		0x00080000
//#define	MAKE_PDF_SYMBOLOGY_AVAILABLE				0x00100000


#ifdef PRODUCT_CONFIGURATION_APPLICATION
#include "RdrCfgForProductCfg.h"
#else
class READER_CONFIGURATION : public READER_CONFIGURATION_STORAGE
{
public:

#include	"DecoderMenuSettings.h"

#include	"CPUPowerSpeedMenuSettings.h"
#include	"DiagnosticAndDebugMenuSettings.h"
#include	"ImagingMenuSettings.h"
#include	"IndicatorMenuSettings.h"
#include	"InterfaceMenuSettings.h"
#include	"OutputMenuSettings.h"
#include	"PlugAndPlayMenuSettings.h"

#if defined(RFBASE) || defined(RFSCAN)
#include	"RFMenuSettings.h"
#endif

#include	"ScriptingMenuSettings.h"
#include	"SystemMenuSettings.h"
#include 	"LicenseControlMenuSettings.h"
#include	"TriggeringMenuSettings.h"

#ifdef VERIFIER
#include	"VerifierMenuSettings.h"
#endif

#ifdef PLUGIN_DEV
#include "PluginMenuSettings.h"
#endif


public:
	bool		FoundValidWorkingConfigurationInFlash;
	bool		FoundValidWorkingConfiguration0InFlash;
	bool        FoundValidWorkingConfiguration1InFlash;
	bool		ImagerIsRunning;


public:		// User declarations
// Initialize all menu settings.
	void Initialize(int ControlMask1);

// Constructor for READER_CONFIGURATION
	READER_CONFIGURATION(int ControlMask1 =	MAKE_LINEAR_SYMBOLOGIES_AVAILABLE);

// Destructor for READER_CONFIGURATION
	~READER_CONFIGURATION(void);

// Once hardware is initialized (especially the imager), or settings have been defaulted, update settings whose ranges or values need
// to be modified based on hardware configuration or are dependent upon other settings.
	void ReconcileSettingsAfterHardwareInitializationOrDefaultCommand( void );

// Reconcile menu setting defaults and ranges for settings dependent upon hardware configuration after hardware has been initialized.
	void ReconcileSettingsAfterHardwareInitialization( void );


protected:
	virtual void ProcessError( int RCSError, const char *SettingName );

};  // Class READER_CONFIGURATION







#ifdef RFBASE
// Pass an imaging command on through the RF base to the default RF imaging device.
bool SendMenuCommandToDefaultImagingDevice( const char *cCommand, const char *cTable );
#endif

	extern READER_CONFIGURATION		*ReaderConfiguration;
#endif

#endif // ReaderConfigurationH
//----------------------------------------------------------------------------




