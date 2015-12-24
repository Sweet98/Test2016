/*===================================================================
=====================================================================
   $Source: Gen6FoundationApp/FoundationApp/menu/MenuFlash.cpp $
   $Date: 2010/04/23 10:04:59EDT $
   $Revision: 1.10 $


===================================================================*/
#if PLATFORM_CURRENT == imx25
#define SAFE_FFS_USED 1
#endif
#ifdef SAFE_FFS_USED
#include <string.h>
#include <stdlib.h>
#include "startupFlashdisk.h"
#endif
#include "db_hdw.h"		// watchdog and max config size
#include "menu.h"
#include "readerconfiguration.h"
#include "db_chrio.h"
#include "ProductConfiguration.h"
#include "MenuFlash.h"




static bool StoreConfigToFlashIfChanged(const char * ConfigName,unsigned char *WorkingConfigurationAddress, int SizeOfWorkingConfiguration)
{
	int		WriteStatus = 0;		// Success so far.
#ifdef SAFE_FFS_USED

	unsigned char *ExistingConfig=NULL;
	WriteStatus=ReadAndCloseFile(ConfigName,0,&ExistingConfig);
/*	if(WriteStatus>0)
	{
		int k=0;
		theStdInterface.printf("Read Value is :\r\n");
		for(k=0;k<WriteStatus;k++)
		{
			theStdInterface.printf("%c",*ExistingConfig++);
		}
		theStdInterface.printf("\r\n");
	}
*/	
	if ((WriteStatus!=SizeOfWorkingConfiguration) || memcmp(ExistingConfig,WorkingConfigurationAddress,SizeOfWorkingConfiguration))
	{
		//theStdInterface.printf("StoreConfiguration:%d\r\n",SizeOfWorkingConfiguration);
		EncodeBuffer(ConfigName,WorkingConfigurationAddress,SizeOfWorkingConfiguration);
		WriteStatus=WriteAndCloseFile(ConfigName,WorkingConfigurationAddress,SizeOfWorkingConfiguration);
	}
	if (ExistingConfig)
	{
		free(ExistingConfig);
	}
#else
	return false;
#endif

	return((WriteStatus >= 0) ? true : false);
}


//
//	StoreWorkingConfigurationToFlash
//
bool StoreWorkingConfigurationToFlash(unsigned char *WorkingConfigurationAddress, int SizeOfWorkingConfiguration)
{
#ifdef SAFE_FFS_USED
#ifndef RFSCAN
	if(ReaderConfiguration->GetActiveWorktableNumber())
		return StoreConfigToFlashIfChanged(FILENAME_WORKING_CONFIG_1,WorkingConfigurationAddress,SizeOfWorkingConfiguration);
	else
		return StoreConfigToFlashIfChanged(FILENAME_WORKING_CONFIG_0,WorkingConfigurationAddress,SizeOfWorkingConfiguration);
#else
#endif
#else

	int		WriteStatus = 0;		// Success so far.

	char	ResultString[160];
	bool	NewAndExistingConfigurationsAreEqual = false;



	// If the new configuration is equal to what's already in flash, don't store it.
	HSTRING	ExistingConfiguration( 20, true );
	if( FlashDisk->Read( FILENAME_WORKING_CONFIG, &ExistingConfiguration ) >= 0 )
	{
		HSTRING	NewConfiguration( 20, true );
		NewConfiguration.Copy( WorkingConfigurationAddress, SizeOfWorkingConfiguration );
		if( NewConfiguration.Compare( &ExistingConfiguration, true ) == 0 )
		{
			NewAndExistingConfigurationsAreEqual = true;
		}
	}

	if( NewAndExistingConfigurationsAreEqual )
	{
		if(flash_debug_on())
		{
			sprintf(ResultString, "New working configuration is the same as stored configuration\r\n" );
			xputstring(ResultString);
		}
	}
	else
	{
		UsageStatistics.TimesWorkingFlashed++;

		MATRIX_FLASH_DISK::FLASH_DISK_WRITE_FILE_LIST	WriteFileList[ 2 ];

		WriteFileList[ 0 ].Name = FILENAME_WORKING_CONFIG;
		WriteFileList[ 0 ].FileContents = ((char *) WorkingConfigurationAddress);
		WriteFileList[ 0 ].FileSize = SizeOfWorkingConfiguration;
		WriteFileList[ 0 ].Type = MATRIX_FLASH_DISK::FD_WORKING_READER_CONFIGURATION;
		WriteFileList[ 0 ].Placement = MATRIX_FLASH_DISK::FD_FORCE_TO_FIXED_LOCATION_IF_AVAILABLE;
		WriteFileList[ 0 ].Modifier = 0;
		WriteFileList[ 0 ].FixedLocation = FLASH_WORKING_CONFIGURATION_BASE;
		WriteFileList[ 0 ].ReservedSize = FLASH_MAX_CONFIGURATION_LENGTH;
		WriteFileList[ 0 ].ReserveToSectorBoundary = false;

		WriteFileList[ 1 ].Name = "BackupWorkingConfiguration";
		WriteFileList[ 1 ].FileContents = ((char *) WorkingConfigurationAddress);
		WriteFileList[ 1 ].FileSize = SizeOfWorkingConfiguration;
		WriteFileList[ 1 ].Type = MATRIX_FLASH_DISK::FD_WORKING_READER_CONFIGURATION;
		WriteFileList[ 1 ].Placement = MATRIX_FLASH_DISK::FD_FORCE_TO_FIXED_LOCATION_IF_AVAILABLE;
		WriteFileList[ 1 ].Modifier = 0;
		WriteFileList[ 1 ].FixedLocation = FLASH_BACKUP_WORKING_CONFIGURATION_BASE;
		WriteFileList[ 1 ].ReservedSize = FLASH_MAX_CONFIGURATION_LENGTH;
		WriteFileList[ 1 ].ReserveToSectorBoundary = false;

		// Write a list of flash disk files.
		WriteStatus = FlashDisk->WriteFiles( WriteFileList, 2 );

		if(flash_debug_on())
		{
			sprintf(ResultString, "Working configuration write status = %d\r\n", WriteStatus);
			xputstring(ResultString);
		}
	}

	return((WriteStatus >= 0) ? true : false);
#endif
} // StoreWorkingConfigurationToFlash

void SaveSettingChangeToTableZero(void)
{
	if((ReaderConfiguration->WorkingValuesUpdated()) && ( GET_SETTING( UpdatePerMenuChanges ) ))
	{
		if(GET_SETTING(GeneralDebugOn))
		{
			xputstring("Writing new working settings to WorkingConfig0\r\n");
		}
		
		HSTRING MenuCommandAndResponse("=at:ae;$.",true);

		ReaderConfiguration->Menu(&MenuCommandAndResponse);
		StoreConfigToFlashIfChanged(FILENAME_WORKING_CONFIG_0,MenuCommandAndResponse.Char,MenuCommandAndResponse.Size);
//		RESET_WATCHDOG_TIMER();
	}
}

void SaveSettingChangeToTableOne(void)
{
	if((ReaderConfiguration->WorkingValuesUpdated()) && ( GET_SETTING( UpdatePerMenuChanges ) ))
	{
		if(GET_SETTING(GeneralDebugOn))
		{
			xputstring("Writing new working settings to WorkingConfig1\r\n");
		}
		HSTRING MenuCommandAndResponse("=at:ae;$.",true);

		ReaderConfiguration->Menu(&MenuCommandAndResponse);
		StoreConfigToFlashIfChanged(FILENAME_WORKING_CONFIG_1,MenuCommandAndResponse.Char,MenuCommandAndResponse.Size);
//		RESET_WATCHDOG_TIMER();
	}
}
//
//	StoreDefaultConfigurationToFlash
//
bool StoreDefaultConfigurationToFlash(unsigned char *DefaultConfigurationAddress, int SizeOfDefaultConfiguration, bool WriteBackupDefaultConfiguration)
{

#ifdef SAFE_FFS_USED
	return StoreConfigToFlashIfChanged(FILENAME_DEFAULT_CONFIG,DefaultConfigurationAddress,SizeOfDefaultConfiguration);
#else

	char	ResultString[160];
	int		WriteStatus = 0;		// Success so far.
	bool	NewAndExistingConfigurationsAreEqual = false;



	UsageStatistics.TimesDefaultFlashed++;

	// If the new configuration is equal to what's already in flash, don't store it.
	HSTRING	ExistingConfiguration( 20, true );
	if( FlashDisk->Read( "DefaultConfiguration", &ExistingConfiguration ) >= 0 )
	{
		HSTRING	NewConfiguration( 20, true );
		NewConfiguration.Copy( DefaultConfigurationAddress, SizeOfDefaultConfiguration );
		if( NewConfiguration.Compare( &ExistingConfiguration, true ) == 0 )
		{
			NewAndExistingConfigurationsAreEqual = true;
		}
	}

	if( NewAndExistingConfigurationsAreEqual )
	{
		if(flash_debug_on())
		{
			sprintf(ResultString, "New default configuration is the same as stored configuration\r\n" );
			xputstring(ResultString);
		}
	}
	else
	{
		// Write the backup copy first.  If this fails, at least the flash disk sector won't be corrupted.
		MATRIX_FLASH_DISK::FLASH_DISK_WRITE_FILE_LIST	WriteFileList[ 2 ];
		int												NumberOfFilesToWrite = 0;

		if( WriteBackupDefaultConfiguration )
		{
			WriteFileList[ NumberOfFilesToWrite ].Name = "BackupDefaultConfiguration";
			WriteFileList[ NumberOfFilesToWrite ].FileContents = ((char *) DefaultConfigurationAddress);
			WriteFileList[ NumberOfFilesToWrite ].FileSize = SizeOfDefaultConfiguration;
			WriteFileList[ NumberOfFilesToWrite ].Type = MATRIX_FLASH_DISK::FD_DEFAULT_READER_CONFIGURATION;
			WriteFileList[ NumberOfFilesToWrite ].Placement = 0;
			WriteFileList[ NumberOfFilesToWrite ].Modifier = 0;
			WriteFileList[ NumberOfFilesToWrite ].FixedLocation = 0;
			WriteFileList[ NumberOfFilesToWrite ].ReservedSize = 0;
			WriteFileList[ NumberOfFilesToWrite ].ReserveToSectorBoundary = false;
			++NumberOfFilesToWrite;
		}

		WriteFileList[ NumberOfFilesToWrite ].Name = "DefaultConfiguration";
		WriteFileList[ NumberOfFilesToWrite ].FileContents = ((char *) DefaultConfigurationAddress);
		WriteFileList[ NumberOfFilesToWrite ].FileSize = SizeOfDefaultConfiguration;
		WriteFileList[ NumberOfFilesToWrite ].Type = MATRIX_FLASH_DISK::FD_DEFAULT_READER_CONFIGURATION;
		WriteFileList[ NumberOfFilesToWrite ].Placement = MATRIX_FLASH_DISK::FD_FORCE_TO_FIXED_LOCATION_IF_AVAILABLE;
		WriteFileList[ NumberOfFilesToWrite ].Modifier = 0;
		WriteFileList[ NumberOfFilesToWrite ].FixedLocation = FLASH_DEFAULT_CONFIGURATION_BASE;
		WriteFileList[ NumberOfFilesToWrite ].ReservedSize = FLASH_MAX_CONFIGURATION_LENGTH;
		WriteFileList[ NumberOfFilesToWrite ].ReserveToSectorBoundary = false;
		++NumberOfFilesToWrite;

		// Write a list of flash disk files.
		WriteStatus = FlashDisk->WriteFiles( WriteFileList, NumberOfFilesToWrite, true );

		if(flash_debug_on())
		{
			sprintf(ResultString, "Default configuration write status = %d\r\n", WriteStatus);
			xputstring(ResultString);
		}
	}

	return((WriteStatus >= 0) ? true : false);
#endif
} // StoreDefaultConfigurationToFlash

bool StoreProductConfigurationToFlash(bool)
{
	HSTRING MenuCommandAndResponse("$.",true);

	ProductConfiguration->Menu(&MenuCommandAndResponse);
	return StoreConfigToFlashIfChanged("ProductConfig",MenuCommandAndResponse.Char,MenuCommandAndResponse.Size);
}



/*----------------------------------------------------------------------------------------
ReadProductConfigurationFromFlash

  Reads the data from Product Config.  This tries keep the number of heap allocations
  to a minimum by resizing the HSTRING (it'll only malloc if needed) and reading
  the data directly into the HSTRING.  We have to remember to terminate the string though.

  Parameters:  
		ProductConfiguration[out]:  pointer to an HSTRING to hold the data

  Returns:  true if file exists and we read the data, false otherwise
----------------------------------------------------------------------------------------*/
bool ReadProductConfigurationFromFlash(HSTRING * pProdConfigData)
{
	int FileLength = GetFileLength(FILENAME_PRODUCT_CONFIG);
	if ( FileLength < 0 ) return false;
	pProdConfigData->Resize(FileLength);
	int NumRead = ReadAndCloseFile(FILENAME_PRODUCT_CONFIG, FileLength, &pProdConfigData->Char);
	if ( ! NumRead ) return false;
	pProdConfigData->Char[FileLength] = 0; 
	return true;
}




//
//	FlashProductConfigurationChanges
//		If any product configuration settings have been updated in the working table, and update flags are set, store them to flash.
//
void FlashProductConfigurationChanges( bool bOverRidePrivilege )
{
	if( bOverRidePrivilege ||( ProductConfiguration->WorkingValuesUpdated() ) &&
	        GET_SETTING( ProductConfigurationQueryPrivilege ) && GET_SETTING( ProductConfigurationUpdatePrivilege ))
	{
		if(GET_SETTING(GeneralDebugOn))
		{
			xputstring("Writing new product configuration settings to flash\r\n");
		}

		StoreProductConfigurationToFlash( true );
//		RESET_WATCHDOG_TIMER();

		ProductConfiguration->WorkingValuesUpToDate();
	}

	return;
} // FlashProductConfigurationChanges



void FlashAndReSynchCustomerDefaults(void)
{
	if((ReaderConfiguration->CustomerDefaultsUpdated()) && ( GET_SETTING( UpdatePerMenuChanges ) ))
	{
		if(GET_SETTING(GeneralDebugOn))
		{
			xputstring("Writing new default settings to flash\r\n");
		}

		HSTRING MenuCommandAndResponse("~@",true);
		
		ReaderConfiguration->Menu(&MenuCommandAndResponse);
		StoreDefaultConfigurationToFlash(MenuCommandAndResponse.Char,MenuCommandAndResponse.Size,true); // create backup if old flashdisk
//		RESET_WATCHDOG_TIMER();
		ReaderConfiguration->CustomerDefaultsUpToDate();
	}
}
void FlashWorkingChanges(void)
{
	if((ReaderConfiguration->WorkingValuesUpdated()) && ( GET_SETTING( UpdatePerMenuChanges ) ))
	{
		if(GET_SETTING(GeneralDebugOn))
		{
			xputstring("Writing new working settings to flash\r\n");
		}
		
		HSTRING MenuCommandAndResponse("=at:ae;$.",true);
		
		ReaderConfiguration->Menu(&MenuCommandAndResponse);
		StoreWorkingConfigurationToFlash(MenuCommandAndResponse.Char,MenuCommandAndResponse.Size);

	}
}


//
//	DeleteDefaultConfigurationFromFlash
//		Erase the default and backup default configuration files from flash.  First zero out the space before removing the files from
//		flash disk, lest we find the default configuration file when we call ValidateMenuCommandCharacters in
//		ReadDefaultConfigurationFromFlash.
//
bool DeleteDefaultConfigurationFromFlash(void)
{
#ifdef SAFE_FFS_USED
	return EraseFile(FILENAME_DEFAULT_CONFIG)==0;
#else

	bool		Status = true;		// No errors yet.


	// Zero out DefaultConfiguration.
	HSTRING		ZeroedData( FLASH_MAX_CONFIGURATION_LENGTH, true );

	Status = StoreDefaultConfigurationToFlash( ZeroedData.Char, ZeroedData.Size, false );


	// Erase the files from the flash disk.
	FlashDisk->RemoveFile( "BackupDefaultConfiguration" );

	// Indicate that customer defaults are up to date so that they don't get rewritten to flash.
	ReaderConfiguration->CustomerDefaultsUpToDate();
	return Status;
#endif

} // DeleteDefaultConfigurationFromFlash



bool OverWriteProductConfiguration(const char * pStr, int StrLength)
{
	EncodeBuffer(FILENAME_PRODUCT_CONFIG,(unsigned char *)pStr,StrLength);
	return WriteAndCloseFile(FILENAME_PRODUCT_CONFIG, (const unsigned char *)pStr, StrLength);
}


bool OverWriteCustomDefaultConfiguration(const char * pStr, int StrLength)
{
	EncodeBuffer(FILENAME_DEFAULT_CONFIG,(unsigned char *)pStr,StrLength);

	return WriteAndCloseFile(FILENAME_DEFAULT_CONFIG, (const unsigned char *)pStr, StrLength);
}


bool OverWriteCurrentWorkingConfiguration(const char * pStr, int StrLength)
{
	const char * pWhichConfig;
	if ( ReaderConfiguration->GetActiveWorktableNumber() ) pWhichConfig = FILENAME_WORKING_CONFIG_1;
	else pWhichConfig = FILENAME_WORKING_CONFIG_0;
	
	EncodeBuffer(pWhichConfig,(unsigned char *)pStr,StrLength);
	return WriteAndCloseFile(pWhichConfig, (const unsigned char *)pStr, StrLength);
}




