//======================================================================================
// SystemMenuSettings.h
//======================================================================================
// $RCSfile: SystemMenuSettings.h $
// $Revision: 1.10 $
// $Date: 2011/11/18 16:55:42EST $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Module Interface Description:
//      This file contains the interface definition to a routine that creates
//		system menu settings as part of the overall reader configuration for
//		a Matrix device.  It also contains the active value declarations of
//		system settings.
//
//---------------------------------------------------------------------------


#ifndef SystemMenuSettingsH
#define SystemMenuSettingsH
//---------------------------------------------------------------------------


#include	"language.h"					// system global definitions


int		FactoryTestMode;

#ifdef LEAN_AUTOMATION
//bool 		FactoryPCBStatus;			//RLC - 01/15/10 - Added flag indicating PCB board test pass/fail
int 		FactoryPCBStatus;			//RLC - 01/15/10 - Added flag indicating PCB board test pass/fail
#endif



bool		MenuBeep;
int			MenuTable;
bool		MenuingEnabled;		// Enable processing of menu symbols.  If disabled, decoded menu symbols will be reported as regular decode
								//	output.  Intention is that this is only visible to us and that only the active setting can be changed, so
								//	that a unit can never get into a state where it boots with this disabled.

#ifdef RFSCAN
bool		RFScannerMenuOutputOverride;	// If this mode is on, always send a menu command response in db_syncommandparse
#endif

bool		UpdatePerMenuChanges;			// Controls whether or not flash storage, interfaces, etc., are updated at the completion
											// of menu command processing.  This is used by tools to prevent updates until all settings have been
											// written, which is particularly useful for RF scanner dongle modes.

bool		AsynchronousStatusReportingModeEnabled;	// When asynchronously transferring or storing data in the background, send progress messages
													//	to the host.

bool		MatrixCommand;

// remved legacy METDES since it tied nothing
//bool		bmetroenable;//use meto command or not
// **** Note on ULTPRV and other privilege settings.
//		I originally created ULTPRV to be a flag word with bits set for various privileges.  It was a bad idea.  Instead I should have
//		done what I subsequently did, which is to create individual boolean settings, such as FlashTestPrivilege and
//		DirectDefaultCommandPrivilege.  Code that uses the ULTPRV flags also looks at the corresponding boolean settings (which are easier
//		for me to remember).  I can't eliminate ULTPRV yet, because it is actually being used by manufacturing and perhaps others.
//		Note that UltimatePrivilegeLevel and DirectDefaultCommandPrivilege are now part of ReaderConfigurationStorage, because we want to control
//		the processing of default commands ('@' terminator) within that class.  The reason for this is that we filtered these commands in the past
//		in the serial command handler, but now we want the query commands to go through even though privilege is off, and we want to block barcode
//		default settings, which we can't do in the serial command handler.
#define	FLASH_TEST_PRIVILEGE					0x40000000

bool		FlashTestPrivilege;
bool		ProductConfigurationQueryPrivilege;
bool		ProductConfigurationUpdatePrivilege;


// Maximum application and boot size.
int			MaximumApplicationSize;
int			MaximumBootSize;
int			SizeOfRAM;
int			SizeOfROM;


//	See matrix\db_app\ProjectRevision.h for definition of SOFTWARE_VERSION


#define		SIZE_OF_SOFTWARE_VERSION				40
char		ShowWARev[SIZE_OF_SOFTWARE_VERSION + 1];

#define		SIZE_OF_PROJECT_VERSION					80
char		ShowSwRev[SIZE_OF_PROJECT_VERSION + 1];

#define		SIZE_OF_CUSTOMER_CONFIGURATION		80
char		ShowCustConfig[SIZE_OF_CUSTOMER_CONFIGURATION + 1];

//char const		*ShowTimeDateSwRev;

#define		SIZE_OF_PROD_NAME				80
char		ShowProductName[SIZE_OF_PROD_NAME + 1];

char     menuEnablePassword[12 + 1];

int			MatrixApplicationType;
HSTRING		*MatrixApplicationMask;
int CompatibleProductsId;


// DWNMED (DownloadMedium) values
#define		DOWNLOAD_MEDIUM_IS_CORD			0
#define		DOWNLOAD_MEDIUM_IS_RADIO		1
#define		DOWNLOAD_MEDIUM_IS_CORD_REM		2

int			DownloadMedium;



void CreateSystemMenuSettings( int ControlMask );


#endif // SystemMenuSettingsH
//----------------------------------------------------------------------------




