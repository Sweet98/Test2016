//======================================================================================
// SystemMenuSettings.cpp
//======================================================================================
// $RCSfile: SystemMenuSettings.cpp $
// $Revision: 1.44 $
// $Date: 2011/11/18 16:55:42EST $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Module Description:
//      This file contains a routine that creates system menu settings as part
//		of the overall reader configuration for an NG2D device.
//
//---------------------------------------------------------------------------



#include "stdInclude.h"
#include "db_sharedtable.h"
#include "MenuFlash.h"	// Need this to get access to DeleteDefaultConfigurationFromFlash routine that zeroes out default
#include "MatrixHeader.h"
#include "Menu.h"
#include "ProjectRevision.h"
#include "ReaderConfiguration.h"
#include "SystemMenuSettings.h"			// SystemMenuSettings interface definition
#include "ProductConfiguration.h"
#include "MocfTags.h"
#include "PinfoCreator.h"
#include "HardwareToken.h"
#include "AutoCableSelect.h"


#if defined(MANUFACTURING_IMAGER_TEST)
// NOTE:  The following header file redefines the RevProject, RevCustConfig, and SOFTWARE_VERSION macros from MatrixFW_mfg_Test.pj
#include "MfgTestProjectRevision.h"
#endif


// Compatible Product ID for MOCF

#if defined(HI2D_T)
#ifdef YJ2D_BASED_ON_HI2D
#define COMP_PROD_ID	cpYoujie4600AppFw
#else
#define COMP_PROD_ID	cpVoyager1400AppFw
#endif
#endif // endof #if defined(VUQUEST3310)




//
//	ProcessProductConfigurationQueryPrivilegeChange
//		If PCFQRY changed state, change visibility of settings.  If PCFUPD and PCFQRY are enabled, allow product configuration settings to be
//		modified if they aren't marked as fixed.
//
bool ProcessProductConfigurationQueryPrivilegeChange( void )
{

	if(GET_SETTING(ProductConfigurationQueryPrivilege))
		ProductConfiguration->ChangeTypeFlags("",SUB_TAG_TABLE::FULLY_VISIBLE, true);
	else
		ProductConfiguration->ChangeTypeFlags("",SUB_TAG_TABLE::FULLY_VISIBLE, false);

	if( GET_SETTING( ProductConfigurationQueryPrivilege ) && GET_SETTING( ProductConfigurationUpdatePrivilege ))
	{
		ProductConfiguration->EnableSettingChanges( true );
	}
	else
	{
		ProductConfiguration->EnableSettingChanges( false );
	}

	return true;
} //	ProcessProductConfigurationQueryPrivilegeChange



//
//	ProcessProductConfigurationUpdatePrivilegeChange
//		If PCFUPD and PCFQRY are enabled, allow product configuration settings to be modified if they aren't marked as fixed.
//
bool ProcessProductConfigurationUpdatePrivilegeChange( void )
{

	if( GET_SETTING( ProductConfigurationQueryPrivilege ) && GET_SETTING( ProductConfigurationUpdatePrivilege ))
	{
		ProductConfiguration->EnableSettingChanges( true ); 
	}
	else
	{
		ProductConfiguration->EnableSettingChanges( false );
	}

	return true;
} //	ProcessProductConfigurationUpdatePrivilegeChange

bool NextCommandAppliedToDefaltSetting(void)
{
	if(ProcessingScannedMenu)
		ReaderConfiguration->AffectedTableChangedToDefalt=true;
	return true;
}

bool SetCustomDefaultProgrammingModeOn( void )
{
	ReaderConfiguration->CustomDefaultProgrammingMode = true;
	return true;
}

bool SetCustomDefaultProgrammingModeOff( void )
{
	ReaderConfiguration->CustomDefaultProgrammingMode = false;
	return true;
}

//
// CheckPassword() function.
//
bool CheckPassword( void)
{
	bool  status = false;      // Haven't succeeded yet.

	if( ReaderConfiguration->MenuCommandResponse->Size < 1 )
	{
		status = true;
	}
	else if( ReaderConfiguration->MenuCommandResponse->Char[0] == '1'  &&
	         ReaderConfiguration->MenuCommandResponse->RangeCheck( 'A','Z', 'a','z', '0','9' ) )
	{
		status = true;
	}
   else if( (GET_SETTING(TerminalID) == 0 ||
             GET_SETTING(TerminalID) == 50 ||
             GET_SETTING(TerminalID) == 130) &&
            ReaderConfiguration->MenuCommandResponse->RangeCheck( 'A','Z', 'a','z', '0','9' ) )
   {
      status = true;

	}
	return status;
}



/*************************************************************************************************
*  ValidatePassword() function.
*
*  This MNUENA entry function checks if a valid password is set.
*
*  Menu Commands:
*   PASWRD - sets up to a 12 alphanumeric (letters and numbers only) password to lock/unlock barcode menuing capability.
*            The standard default is no password.
*   MNUENAx[password] - disables/enables menuing.
*   High security option only works in bidirectional interfaces(termID=0,50,130) to prevent scanner
*   from being permanently locked.
*  (see "matrix password.doc" for usage explanation.)
*
*   Inputs:
*     MNUENA data.
*
*   Outputs:
*     Returns FAIL(false) if password requirement not met.
*     Returns PASS(true) if valid password.
*****************************************************************************************************/
bool ValidatePassword(void)
{
	bool	Status = false;		// Haven't succeeded yet.
	char pucBuffer[15];
	strcpy(pucBuffer, GET_SETTING(menuEnablePassword));

	if( !strcmp(pucBuffer,"") )
	{
		Status = true;
	}
   else if(ReaderConfiguration->MenuCommandResponse->Char[0] == '0' )
   {
		if(
			GET_SETTING(TerminalID) == 0 ||
			GET_SETTING(TerminalID) == 50 ||
			GET_SETTING(TerminalID) == 130 ||
			ReaderConfiguration->MenuCommandResponse->Char[1] == '1' )
		{

			ReaderConfiguration->MenuCommandResponse->Remove(0,1);
			if( ReaderConfiguration->MenuCommandResponse->Compare( pucBuffer, false ) == 0 )
			{
				ReaderConfiguration->MenuCommandResponse->Copy("0");
				Status = true;
			}
		}
	}
	else if(( ReaderConfiguration->MenuCommandResponse->Compare( pucBuffer, false ) == 0 ))
	{
		ReaderConfiguration->MenuCommandResponse->Copy("1");
		Status = true;
	}
	return Status;
} // ValidatePassword




/*----------------------------------------------------------------------------------------
CableNumToPinfoNum

  Helper function for DisplayPinfo().  It converts our cable number to a number used by
  the PINFO_NAME_CBL value.

  Parameters:  none

  Returns:  PINFO_NAME_CBL value.
----------------------------------------------------------------------------------------*/
int CableNumToPinfoNum(void)
{
	int num;
	if ( PresentInterface == Interface_232_Group ) num = PINFO_CABLE_RS232;
	else if ( PresentInterface == Interface_Fac232_Group ) num = PINFO_CABLE_RS232;
	else if ( PresentInterface == Interface_USB_Group ) num = PINFO_CABLE_USB;
	else if ( PresentInterface == Interface_485_Group ) num = PINFO_CABLE_RETAIL;
	else if ( PresentInterface == Interface_KBDWDG_Group ) num = PINFO_CABLE_KBD_WDG;
	else num = PINFO_CABLE_UNKNOWN;

return num;
}




/*----------------------------------------------------------------------------------------
DisplayPinfo

  Creates and displays the P_INFO data structure.

  Parameters:  none

  Returns:  true.
----------------------------------------------------------------------------------------*/
bool DisplayPinfo(void)
{
	HSTRING PinfoStr;
	HSTRING val;
	PinfoCreator pinfo;
	char BootSwRev[50];
	
	pinfo.AddItem(PINFO_NAME_MODE, PINFO_STR_VAL_APP);  // at this time the P_INFO spec hasn't been updated, but emails say the value is either "app" or "boot", not "normal" or "boot".
	pinfo.AddItem(PINFO_NAME_APP_PRES, true);
	pinfo.AddItem(PINFO_NAME_BOOT_PRES, true);
	val.Copy(GET_CONFIG_SETTING(SerNum));
	val.UniCodeToSimpleAsciiFormat();
	pinfo.AddItem(PINFO_NAME_SN, (const char *)val.Char);
	pinfo.AddItemHex(PINFO_NAME_HW_REV, HardwareToken.GetHardwareToken());  // Note:  the P_INFO spec is vague about what's valid for this value.  Emails didn't clarify, so I went with this.
	pinfo.AddItem(PINFO_NAME_CBL, CableNumToPinfoNum());
	pinfo.AddItem(PINFO_NAME_APP_REV, GET_SETTING(ShowWARev) + 2);  // ShowWARev string has a ": " prepended to the number--we need to remove that
	if ( BOOT_CODE_MINOR_VERSION == 1 ) strcpy(BootSwRev, PINFO_STR_VAL_UNKNOWN);
	else
	{	// to avoid a pesky klockwork warning, we'll use strncpy() & ensure it's NULL terminated
		strncpy(BootSwRev, BOOT_CODE_VERSION_STRING, sizeof BootSwRev - 1); 
		BootSwRev[sizeof BootSwRev - 1] = 0;
	}
	pinfo.AddItem(PINFO_NAME_BOOT_REV, BootSwRev);
	pinfo.AddItem(PINFO_NAME_FLASH, 3);
	pinfo.AddItem(PINFO_NAME_APP_PNAME_MENU, (const char *)GET_SETTING(ShowProductName));
	pinfo.AddItem(PINFO_NAME_BOOT_PNAME_MENU, (const char *)GET_SETTING(ShowProductName));
	val.Copy(GET_CONFIG_SETTING(ProductName));
	val.UniCodeToSimpleAsciiFormat();
	pinfo.AddItem(PINFO_NAME_APP_PNAME_CONFIG, (const char *)val.Char);
	pinfo.AddItem(PINFO_NAME_BOOT_PNAME_CONFIG, (const char *)val.Char);
	
	if ( BOOT_CODE_MINOR_VERSION < 3 )
	{
		pinfo.AddItem(PINFO_NAME_BOOT_SW_TIME, PINFO_STR_VAL_UNKNOWN);
		pinfo.AddItem(PINFO_NAME_BOOT_SW_DATE, PINFO_STR_VAL_UNKNOWN);
	}
	else
	{
		val.Copy(BOOT_TIME_STR);
		pinfo.AddItem(PINFO_NAME_BOOT_SW_TIME, (const char *)val.Char);
		val.Copy(BOOT_DATE_STR);
		pinfo.AddItem(PINFO_NAME_BOOT_SW_DATE, (const char *)val.Char);
	}
	pinfo.AddItem(PINFO_NAME_APP_SW_TIME, g_TimeStamp);
	pinfo.AddItem(PINFO_NAME_APP_SW_DATE, g_DateStamp);


	pinfo.CreateStr(&PinfoStr);
	ReaderConfiguration->MenuCommandResponse->Append(&PinfoStr);
	return true;
}



void READER_CONFIGURATION::CreateSystemMenuSettings( int /* ControlMask */ )
{

	AddCommand("0D0100", SUB_TAG_TABLE::FULLY_VISIBLE, NULL, NULL, SUB_TAG_TABLE::SET_ALL_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT);

	//	"FACTON" is used by manufacturing to put a unit in 'Semi-permanent' factory test mode...
	//	 when enabled on power-up, PlugAndPlayFactoryTest() is called during initialization.
#ifdef LEAN_AUTOMATION
	AddNumber("0D0300", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, NULL, &FactoryTestMode, 1,1,0,2);
#else
	AddNumber("0D0300", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, NULL, &FactoryTestMode, 0,0,0,2);
#endif
	//	AddCommand("DEFCUS", SUB_TAG_TABLE::FULLY_VISIBLE, NULL, NULL,  SUB_TAG_TABLE::COPY_WORKING_VALUES_TO_DEFAULTS);
	AddCommand("0D0101", SUB_TAG_TABLE::FULLY_VISIBLE, NULL, NULL,  SUB_TAG_TABLE::SET_ALL_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT);
	AddCommand("0D0400",SUB_TAG_TABLE::FULLY_VISIBLE,NULL,NULL);
	AddCommand("0D0500",SUB_TAG_TABLE::FULLY_VISIBLE,NULL,NULL);

	AddBoolean("0D0600", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &MenuBeep, true, true);
	AddNumber("0D0601", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &MenuTable, 0, 0, 0, 0);

	AddBoolean("0D0602", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG, &ValidatePassword, NULL, &MenuingEnabled, true, true);
	AddString("0D0700", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &CheckPassword,NULL, menuEnablePassword, 12,  "", "");


	AddBoolean("0D0604", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | SUB_TAG_TABLE::FIXED_EXCEPT_ACTIVE | LOCAL_SETTING, NULL, NULL,
	           &UpdatePerMenuChanges, true, true );

	AddBoolean("0D0800", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | SUB_TAG_TABLE::FIXED_EXCEPT_ACTIVE | LOCAL_SETTING, NULL, NULL,
	           &AsynchronousStatusReportingModeEnabled, false, false);

	AddCommand("0D0102", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::ONLY_AVAILABLE_WITH_DEFAULT_COMMAND_PRIVILEGE, NULL,
	           &DeleteDefaultConfigurationFromFlash, SUB_TAG_TABLE::COPY_STANDARD_DEFAULTS_TO_DEFAULTS);

	// Matrix ID command
	AddBoolean("0D0900", SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::IGNORE_ON_STORAGE_QUERY,
	           NULL, NULL, &MatrixCommand, true, true);


	// Special privilege commands.
	// **** See note for this setting in ReaderConfiguration.h.
	AddNumber("0D0A00", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG, NULL, NULL, &UltimatePrivilegeLevel, 0, 0, 0, 0x7FFFFFFF);

	AddBoolean("0D0B00", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG, NULL, ProcessProductConfigurationQueryPrivilegeChange,
	           &ProductConfigurationQueryPrivilege, false, false);

	AddBoolean("0D0C00", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG, NULL, NULL, &FlashTestPrivilege, false, false);
	AddBoolean("0D0D00", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG, NULL, NULL, &DirectDefaultCommandPrivilege, true, true);
	AddBoolean("0D0B01", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | SUB_TAG_TABLE::FIXED_EXCEPT_ACTIVE , NULL,
	           ProcessProductConfigurationUpdatePrivilegeChange, &ProductConfigurationUpdatePrivilege,
	           false, false);
	//command for the programming barcode to applied to custom defalt table.
	AddCommand("0D0605",SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG,&NextCommandAppliedToDefaltSetting,NULL);
	// comamnds for new custom default programming modes: MNUCDP turns programming on, MNUCDS turns programming off
	AddCommand("0D0606",SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG,&SetCustomDefaultProgrammingModeOn,NULL);
	AddCommand("0D0607",SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG,&SetCustomDefaultProgrammingModeOff,NULL);

#ifndef WINSIM
	// Maximum application and boot size query
	AddNumber("0D0E00", SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::IGNORE_ON_STORAGE_QUERY,
	          NULL, NULL, &MaximumApplicationSize, FLASH_MAX_APPLICATION_AREA_LENGTH, FLASH_MAX_APPLICATION_AREA_LENGTH,
	          FLASH_MAX_APPLICATION_AREA_LENGTH, FLASH_MAX_APPLICATION_AREA_LENGTH );
	AddNumber("0D0E01", SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::IGNORE_ON_STORAGE_QUERY,
	          NULL, NULL, &MaximumBootSize, FLASH_MAX_BOOT_CODE_LENGTH, FLASH_MAX_BOOT_CODE_LENGTH,
	          FLASH_MAX_BOOT_CODE_LENGTH, FLASH_MAX_BOOT_CODE_LENGTH );
	AddNumber("0D0F00", SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::IGNORE_ON_STORAGE_QUERY,
	          NULL, NULL, &SizeOfRAM, MEMORY_SIZE, MEMORY_SIZE, MEMORY_SIZE, MEMORY_SIZE );
	AddNumber("0D0F01", SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::IGNORE_ON_STORAGE_QUERY,
	          NULL, NULL, &SizeOfROM, ROM_SIZE, ROM_SIZE, ROM_SIZE, ROM_SIZE );

	// Show application type and mask.
	int		iDownloadMask;
	iDownloadMask = 0;
	MatrixApplicationType = 0;
	char	chDownloadMask[ 10 ];
	sprintf( chDownloadMask, "%08X", iDownloadMask );
	AddNumber("0D1000", SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::IGNORE_ON_STORAGE_QUERY,
	          NULL, NULL, &MatrixApplicationType, MatrixApplicationType, MatrixApplicationType, MatrixApplicationType, MatrixApplicationType );
	AddHexString("0D1001", SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::IGNORE_ON_STORAGE_QUERY,
	             NULL, NULL, &MatrixApplicationMask, strlen( chDownloadMask ), chDownloadMask, chDownloadMask );
	AddNumber("0D1100", SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::IGNORE_ON_STORAGE_QUERY,
	          NULL, NULL, &CompatibleProductsId, COMP_PROD_ID, COMP_PROD_ID, COMP_PROD_ID, COMP_PROD_ID);


	// The DWNMED setting is used to indicate what medimum (interface) was used for the last download.  For example, if application code is downloaded
	// via the RF link, DWNMED will be set to DOWNLOAD_MEDIUM_IS_RADIO on the next restart so the ACK can go out the correct interface.
	AddNumericList( "0D1200", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | SUB_TAG_TABLE::FIXED_EXCEPT_ACTIVE | LOCAL_SETTING,
	                NULL, NULL, &DownloadMedium, DOWNLOAD_MEDIUM_IS_CORD, DOWNLOAD_MEDIUM_IS_CORD, DOWNLOAD_MEDIUM_IS_CORD, DOWNLOAD_MEDIUM_IS_CORD_REM );
#endif

	//Custom Configuration ID, please leave it at the end of the menu table
	AddString("0D1300", SUB_TAG_TABLE::STRING_MUST_BE_QUOTED | SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, ShowCustConfig,
	          SIZE_OF_CUSTOMER_CONFIGURATION, RevCustConfig, RevCustConfig);

	// Revision Information.
	AddString("0D1301", SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::FULLY_VISIBLE, NULL, NULL, ShowSwRev,
	          SIZE_OF_PROJECT_VERSION, RevProject, RevProject);
	char SoftwareVersion[SIZE_OF_SOFTWARE_VERSION];
	ReadSoftwareVersion(SoftwareVersion, SIZE_OF_SOFTWARE_VERSION);
	AddString("0D1302", SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::FULLY_VISIBLE, NULL, NULL, ShowWARev,
	          SIZE_OF_SOFTWARE_VERSION, SoftwareVersion, SoftwareVersion);
	int TimeDateRevStrLen = strlen(TimeDateRevisionString);
	char * pTimeDateRevStrStorageArea = new char[TimeDateRevStrLen + 1]; // we need the +1 because AddString() will memcpy(length +1) to get the NULL
	AddString("0D1303", SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::FULLY_VISIBLE, NULL, NULL, pTimeDateRevStrStorageArea,
	          TimeDateRevStrLen /* don't include the NULL */ , TimeDateRevisionString, TimeDateRevisionString);

#ifdef LEAN_AUTOMATION		
    //RLC - 01/15/10 - Added menu command to get/set PCB test status
//	AddBoolean("MFGPCB", SUB_TAG_TABLE::FULLY_VISIBLE, NULL, NULL, &FactoryPCBStatus, false, false);
	AddNumber("0D1400", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::DEFAULTING_DISABLED, NULL, NULL, &FactoryPCBStatus, 0,0,0,9);

#endif

	// remved legacy METDES since it tied nothing
	//AddBoolean("METDES",NULL,NULL,NULL,& bmetroenable,false,false);
	AddCommand("0D1500", SUB_TAG_TABLE::FULLY_VISIBLE, &DisplayPinfo, NULL);
	return;
}


