/************************************************************************/
//
// HHP 4480 embedded firmware
//
//  Flash management functions ported from PSC Imager 8000
//  Date:       3/12/02
//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Flash/flashapplicationcode.cpp $
// $Revision: 1.7 $
// $Date: 2010/02/16 15:12:47EST $
//======================================================================================
#if PLATFORM_CURRENT == imx25
#define SAFE_FFS_USED 1
#endif

#include <stdio.h>		/* standard I/O functions					*/
#include <stdlib.h>		/* standard library functions				*/
#include <string.h>		/* standard string functions				*/

#include "FlashApplicationCode.h"
#include "db_chrio.h"	// Serial port character output routines
#include "CheckSum.h"		// Checksum calculation definitions
#include "usage_statistics.h"
#ifndef SAFE_FFS_USED
#include "db_hdw.h"		/* artwork dependant hardware definitions	*/
#include "db_flash.h"		/* Flash ROM functions						*/
#include "db_FlashDisk.h"	//TEST ~DV
#else
#include "num_m25pxx.h"
#include "flashdrv.h"
#include "startupflashdisk.h"
#endif




#ifndef SAFE_FFS_USED

//
//	StoreApplicationCodeToFlash
//		Create entry point information for the new application code and store entry point information with code to flash.  We'll now
//		create an alternate flash disk up at the top of flash so that if the standard flash disk is lost due to power cycle, etc., the
//		boot code will at least be able to find the application.
//
//		Note that we write main application code such that it tries to reserve space to the end of the sector, so that it cannot be
//		hit by a power cycle when something else is being written to the last sector containing application code
//
bool StoreApplicationCodeToFlash(const UCHAR *ApplicationCodeAddress, int SizeOfApplicationCode, bool RunFromROM, HSTRING *)//UpdateFileContents)
{
	char	ResultString[160];
	int		WriteStatus;


	// Set application code type.
	int		ApplicationCodeType;
	if(RunFromROM)
		ApplicationCodeType = MATRIX_FLASH_DISK::FD_MAIN_APPLICATION_RUN_FROM_ROM;
	else
		ApplicationCodeType = MATRIX_FLASH_DISK::FD_MAIN_APPLICATION;

	// If 2MB of flash or more, try to create a backup copy of the main application code.
	if( ROM_SIZE > Sz_1M )
	{
		int			CopyFileIndex = FlashDisk->Find( "mainAPP" );
		if( CopyFileIndex >= 0 )
		{
			int	ExistingApplicationCodeLocation = ((int) CACHED_ROM_BASE ) + FlashDisk->MatrixFlashDiskEntries[ CopyFileIndex ].Location;
			int	ExistingApplicationCodeSize = FlashDisk->MatrixFlashDiskEntries[ CopyFileIndex ].Size;

			// If we succeeded in reading, try copying to FLASH_APPLICATION_BASE + reserved size of main app code OR FLASH_APPLICATION_BASE + size of
			// new app code (whichever is greater).
			unsigned int	FixedLocationToWriteTo =
			    ( FLASH_APPLICATION_BASE + FlashDisk->MatrixFlashDiskEntries[ CopyFileIndex ].ReservedSize ) >=
			    ( FLASH_APPLICATION_BASE + SizeOfApplicationCode ) ?
			    ( FLASH_APPLICATION_BASE + FlashDisk->MatrixFlashDiskEntries[ CopyFileIndex ].ReservedSize ) :
			    ( FLASH_APPLICATION_BASE + SizeOfApplicationCode );
			FixedLocationToWriteTo += ( FixedLocationToWriteTo % 2 );

			// Adjust the location to write to so that it falls on a sector boundary
			int				SectorNumberOfFixedLocationToWriteTo;
			unsigned int	SectorStartAddressOfFixedLocationToWriteTo;
			unsigned int	SectorSizeOfFixedLocationToWriteTo;
			bool			SectorModified;
			FlashDisk->GetSectorInformation( FixedLocationToWriteTo, &SectorNumberOfFixedLocationToWriteTo,
			                                 &SectorStartAddressOfFixedLocationToWriteTo, &SectorSizeOfFixedLocationToWriteTo, &SectorModified );
			if( FixedLocationToWriteTo != SectorStartAddressOfFixedLocationToWriteTo )
			{
				FixedLocationToWriteTo = SectorStartAddressOfFixedLocationToWriteTo + SectorSizeOfFixedLocationToWriteTo;
			}

			int CopyStatus = FlashDisk->Write( "BackupMainApplication", (char *) ExistingApplicationCodeLocation,
			                                   ExistingApplicationCodeSize, FlashDisk->MatrixFlashDiskEntries[ CopyFileIndex ].Type,
			                                   MATRIX_FLASH_DISK::FD_FORCE_TO_FIXED_LOCATION, FixedLocationToWriteTo, 0, true, true );

			if( CopyStatus >= 0 )
			{
				// If copy worked, delete the old main application.
				FlashDisk->RemoveFile( "mainAPP" );

			}
		}
	}


	// Write the new main application file.
	WriteStatus = FlashDisk->Write("mainAPP", ((char *) ApplicationCodeAddress), SizeOfApplicationCode,
	                               ApplicationCodeType, MATRIX_FLASH_DISK::FD_FORCE_TO_FIXED_LOCATION, FLASH_APPLICATION_BASE, 0,
	                               true, true );

	// If write worked, delete the backup.
	if( WriteStatus >= 0 )
		FlashDisk->RemoveFile( "BackupMainApplication" );

	// If the write succeeded, update usage statistics.
	if (WriteStatus)
	{
				UsageStatistics.TimesAppFlashed++;
				StoreUsageStatistics();
	}


	if(flash_debug_on())
	{
		sprintf(ResultString, "Application code write status = %d\r\n", WriteStatus);
		xputstring(ResultString);
	}


	return((WriteStatus >= 0) ? true : false);
} // StoreApplicationCodeToFlash
#else
//
//	StoreApplicationCodeToFlash
//		Create entry point information for the new application code and store entry point information with code to flash.  We'll now
//		create an alternate flash disk up at the top of flash so that if the standard flash disk is lost due to power cycle, etc., the
//		boot code will at least be able to find the application.
//
//		Note that we write main application code such that it tries to reserve space to the end of the sector, so that it cannot be
//		hit by a power cycle when something else is being written to the last sector containing application code
//
bool StoreApplicationCodeToFlash(const UCHAR *ApplicationCodeAddress, int SizeOfApplicationCode, bool , HSTRING *)//UpdateFileContents)
{
	int		WriteStatus=-1;
	// Write the new main application file.
	// If the write succeeded, update usage statistics.
	WriteStatus =WriteAndCloseFile( "mainAPP",ApplicationCodeAddress,SizeOfApplicationCode);
	if (WriteStatus==SizeOfApplicationCode) // safe ffs returns number of bytes written
	{
				UsageStatistics.TimesAppFlashed++;
				StoreUsageStatistics();
	}
	if(flash_debug_on())
	{
		char	ResultString[80];
		sprintf(ResultString, "Application code write status = %d\r\n", WriteStatus);
		xputstring(ResultString);
	}
	return((WriteStatus == SizeOfApplicationCode) ? true : false);
} // StoreApplicationCodeToFlash

#endif



