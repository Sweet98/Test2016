//======================================================================================
// MatrixHeader.cpp
//======================================================================================
// $RCSfile: TEMP_SHARED/MatrixHeader.cpp $
// $Revision: 1.3 $
// $Date: 2009/02/25 12:32:37EST $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Description:
//		MatrixHeader.cpp contains the routines needed to find and access Matrix
//		application and boot header information in application and boot code
//		respectively.  The boot header information contains identifiers
//		("MATRIXBOOT" and "DEVICETYPE") followed by a device ID.  The application
//		header information contains identifiers ("MATRIXAPP" and "DEVICETYPE")
//		followed by a device mask consisting of the number of bytes in the mask
//		and the mask itself.  Whenever new boot code is downloaded, the
//		application uses routines in this module to find the header in the
//		existing boot code, find the header in the new boot code, and compare the
//		device types to make sure that they match.  Whenever new application code
//		is downloaded, the application uses routines in this module to find the
//		header in the existing boot code, find the header in the new application
//		code, and makes sure that the device mask bit corresponding to the
//		device type in the boot header is set in the new application code.
//
//---------------------------------------------------------------------------

#define SAFE_FFS_USED 1

#ifndef	MATRIX_BOOT_CODE
#include "HString.h"
#endif
#include "MatrixHeader.h"
#include "db_sharedtable.h"


//
//	FindMatrixBootHeader
//		Find the Matrix boot header in the code sent by the caller.
//
int FindMatrixBootHeader(const char *Code, int SizeOfCode)
{
	int		OffsetBeyondMatrixBootHeader = -1;
	int		i;


	// Find the header.
	i = 0;
	while((i < (SizeOfCode - SIZE_OF_MATRIX_BOOT_HEADER)) && (OffsetBeyondMatrixBootHeader < 0))
	{
		//		if(( strnicmp( &Code[ i ], "MATRIXBOOT", 10 ) == 0 ) &&
		//							( strnicmp( &Code[ i + 12 ], "DEVICETYPE", 10 ) == 0 ) &&
		//							(Code[ i + 10 ] == 0) && (Code[ i + 11 ] == 0) &&
		//							(Code[ i + 22 ] == 0) && (Code[ i + 23 ] == 0))
		if((Code[i + 0] == 'M') && (Code[i + 1] == 'A') && (Code[i + 2] == 'T') &&
		        (Code[i + 3] == 'R') && (Code[i + 4] == 'I') && (Code[i + 5] == 'X') &&
		        (Code[i + 6] == 'B') && (Code[i + 7] == 'O') && (Code[i + 8] == 'O') &&
		        (Code[i + 9] == 'T') && (Code[i + 10] == 0) && (Code[i + 11] == 0) &&
		        (Code[i + 12] == 'D') && (Code[i + 13] == 'E') && (Code[i + 14] == 'V') &&
		        (Code[i + 15] == 'I') && (Code[i + 16] == 'C') && (Code[i + 17] == 'E') &&
		        (Code[i + 18] == 'T') && (Code[i + 19] == 'Y') && (Code[i + 20] == 'P') &&
		        (Code[i + 21] == 'E') && (Code[i + 22] == 0) && (Code[i + 23] == 0))
		{
			OffsetBeyondMatrixBootHeader = i + SIZE_OF_MATRIX_BOOT_HEADER;
		}
		else
		{
			++i;
		}
	}


	return(OffsetBeyondMatrixBootHeader);
} // FindMatrixBootHeader






//
//	FindMatrixApplicationHeader
//		Find the Matrix application header in the code sent by the caller.
//
int FindMatrixApplicationHeader(const char *Code, int SizeOfCode)
{
	int		OffsetBeyondMatrixApplicationHeader = -1;
	int		i;


	// Find the header.
	i = 0;
	while((i < (SizeOfCode - SIZE_OF_MATRIX_APPLICATION_HEADER)) && (OffsetBeyondMatrixApplicationHeader < 0))
	{
		//		if(( strnicmp( &Code[ i ], "MATRIXAPP", 9 ) == 0 ) &&
		//							( strnicmp( &Code[ i + 12 ], "DEVICETYPE", 10 ) == 0 ) &&
		//							(Code[ i + 9 ] == 0) && (Code[ i + 10 ] == 0) && (Code[ i + 11 ] == 0) &&
		//							(Code[ i + 22 ] == 0) && (Code[ i + 23 ] == 0))
		if((Code[i + 0] == 'M') && (Code[i + 1] == 'A') && (Code[i + 2] == 'T') &&
		        (Code[i + 3] == 'R') && (Code[i + 4] == 'I') && (Code[i + 5] == 'X') &&
		        (Code[i + 6] == 'A') && (Code[i + 7] == 'P') && (Code[i + 8] == 'P') &&
		        (Code[i + 9] == 0) && (Code[i + 10] == 0) && (Code[i + 11] == 0) &&
		        (Code[i + 12] == 'D') && (Code[i + 13] == 'E') && (Code[i + 14] == 'V') &&
		        (Code[i + 15] == 'I') && (Code[i + 16] == 'C') && (Code[i + 17] == 'E') &&
		        (Code[i + 18] == 'T') && (Code[i + 19] == 'Y') && (Code[i + 20] == 'P') &&
		        (Code[i + 21] == 'E') && (Code[i + 22] == 0) && (Code[i + 23] == 0))
		{
			OffsetBeyondMatrixApplicationHeader = i + SIZE_OF_MATRIX_APPLICATION_HEADER;
		}
		else
		{
			++i;
		}
	}


	return(OffsetBeyondMatrixApplicationHeader);
} // FindMatrixApplicationHeader







//
//	GetDeviceType
//		Determine the device type by finding the Matrix boot header in the boot code and the device type that immediately
//		follows.
//
int GetDeviceType(const char *, int )
{
#ifdef ETM
	return(1);
#else

	return(DEVICE_TYPE_ST);
#endif
} // GetDeviceType






//
//	MatchApplicationType
//		Match the device type mask to device type in code sent by the caller.
//
bool MatchApplicationType(const char *CodeToMatch, int SizeOfCodeToMatch, const char *, int )
{
	int		OffsetToDeviceTypeMask;
	int		SizeOfDeviceTypeMask;
	bool	DeviceTypesMatch = false;


	// Get offsets to device type and device type mask.
	int		DeviceType = GetDeviceType(NULL,0);
	OffsetToDeviceTypeMask = FindMatrixApplicationHeader(CodeToMatch, SizeOfCodeToMatch);

	if(OffsetToDeviceTypeMask >= 0)
	{
		SizeOfDeviceTypeMask = *((int *) &CodeToMatch[OffsetToDeviceTypeMask]);

		// If the mask fits inside the code, find and compare the bit specified by DeviceType.
		if(((OffsetToDeviceTypeMask + (int) sizeof(int) + SizeOfDeviceTypeMask) <= SizeOfCodeToMatch) &&
		        ((SizeOfDeviceTypeMask * 8) >= DeviceType))
		{
			// Look at the specific bit in the device mask (referenced by DeviceType) to see if they match.
			if(CodeToMatch[OffsetToDeviceTypeMask + sizeof(int) + ((DeviceType - 1) / 8)] &
			        (1 << ((DeviceType - 1) % 8)))
			{
				DeviceTypesMatch = true;
			}
		}
	}

	return(DeviceTypesMatch);

} // MatchApplicationType






// *********************************************************************************************************************************
// This file is shared by boot code, but boot code does not require every operation here, and we don't want unnecessary code include
// because of boot code space limitations.  Everything beyond this point is not needed by boot code.
#ifndef	MATRIX_BOOT_CODE

//
//	SearchForTextFlag
//		Search for a text flag (case insensitive).
//
bool SearchForTextFlag(const char *Code, int SizeOfCode, const char *TextFlag, int *LocationWhereFlagWasFound)
{
	int		OffsetToDeviceTypeMask;
	int		SizeOfDeviceTypeMask;
	int		NumberOfTextFlagBlockEntries;
	int		SizeOfNextTextFlagBlockEntry;
	bool	TextFlagFound = false;



	// Get offsets to device type and device type mask.
	OffsetToDeviceTypeMask = FindMatrixApplicationHeader(Code, SizeOfCode);

	if(OffsetToDeviceTypeMask >= 0)
	{
		SizeOfDeviceTypeMask = *((int *) &Code[OffsetToDeviceTypeMask]);

		// If the mask fits inside the code, check to see if the text flag block header (0xFFFFFFFF) and # of entries are present.
		if(((OffsetToDeviceTypeMask + (3 * (int) sizeof(int)) + SizeOfDeviceTypeMask) <= SizeOfCode) &&
		        (*((int *) &Code[OffsetToDeviceTypeMask + sizeof(int) + SizeOfDeviceTypeMask]) == 0xFFFFFFFF))
		{
			NumberOfTextFlagBlockEntries = *((int *) &Code[OffsetToDeviceTypeMask + (2 * sizeof(int)) + SizeOfDeviceTypeMask]);

			// Search all entries until we find a match or we run out of entries.
			*LocationWhereFlagWasFound = OffsetToDeviceTypeMask + (3 * sizeof(int)) + SizeOfDeviceTypeMask;
			while((NumberOfTextFlagBlockEntries) && (!TextFlagFound))
			{
				SizeOfNextTextFlagBlockEntry = *((int *) &Code[*LocationWhereFlagWasFound]);
				*LocationWhereFlagWasFound += sizeof(int);
				if(HSTRING::hhpstricmp(TextFlag, &Code[*LocationWhereFlagWasFound], true) == 0)
				{
					TextFlagFound = true;
				}
				else
				{
					*LocationWhereFlagWasFound += SizeOfNextTextFlagBlockEntry;
					--NumberOfTextFlagBlockEntries;
				}
			}
		}
	}


	return(TextFlagFound);
} // SearchForTextFlag





//
//	SearchForTextFlagByIndex
//		Search for a text flag according to it's index.
//
bool SearchForTextFlagByIndex(const char *Code, int SizeOfCode, int TextFlagIndex, int *LocationWhereFlagWasFound)
{
	int		OffsetToDeviceTypeMask;
	int		SizeOfDeviceTypeMask;
	int		NumberOfTextFlagBlockEntries;
	int		SizeOfNextTextFlagBlockEntry;
	bool	TextFlagFound = false;



	// Get offsets to device type and device type mask.
	OffsetToDeviceTypeMask = FindMatrixApplicationHeader(Code, SizeOfCode);

	if(OffsetToDeviceTypeMask >= 0)
	{
		SizeOfDeviceTypeMask = *((int *) &Code[OffsetToDeviceTypeMask]);

		// If the mask fits inside the code, check to see if the text flag block header (0xFFFFFFFF) and # of entries are present.
		if(((OffsetToDeviceTypeMask + (3 * (int) sizeof(int)) + SizeOfDeviceTypeMask) <= SizeOfCode) &&
		        (*((int *) &Code[OffsetToDeviceTypeMask + sizeof(int) + SizeOfDeviceTypeMask]) == 0xFFFFFFFF))
		{
			NumberOfTextFlagBlockEntries = *((int *) &Code[OffsetToDeviceTypeMask + (2 * sizeof(int)) + SizeOfDeviceTypeMask]);

			// Search all entries until we find a match or we run out of entries.
			*LocationWhereFlagWasFound = OffsetToDeviceTypeMask + (3 * sizeof(int)) + SizeOfDeviceTypeMask;
			int		i = 0;
			while((NumberOfTextFlagBlockEntries) && (!TextFlagFound))
			{
				SizeOfNextTextFlagBlockEntry = *((int *) &Code[*LocationWhereFlagWasFound]);
				*LocationWhereFlagWasFound += sizeof(int);
				if( i == TextFlagIndex )
				{
					TextFlagFound = true;
				}
				else
				{
					*LocationWhereFlagWasFound += SizeOfNextTextFlagBlockEntry;
					--NumberOfTextFlagBlockEntries;
					++i;
				}
			}
		}
	}


	return(TextFlagFound);
} // SearchForTextFlagByIndex





//
//	RAMOnly
//		Determine if the code pointed to by the caller is RAM only and should not be written to flash.
//
bool RAMOnly(const char *Code, int SizeOfCode)
{
#ifdef ETM
	return(true);
#else

	int		LocationWhereRAMOnlyFlagWasFound;

	return(SearchForTextFlag(Code, SizeOfCode, "RAMONLY", &LocationWhereRAMOnlyFlagWasFound));
#endif
} // RAMOnly





//
//	RunFromROM
//		Determine if the code pointed to by the caller is to be run from ROM.
//
bool RunFromROM(const char *Code, int SizeOfCode)
{
#ifdef ETM
	return(false);
#else

	int		LocationWhereRunFromROMFlagWasFound;

	return(SearchForTextFlag(Code, SizeOfCode, "RUNFROMROM", &LocationWhereRunFromROMFlagWasFound));
#endif
} // RunFromROM





//
//	ApplicationACKsProperlyAfterDownloadOverBootCode
//		Determine if the code pointed to by the caller will ACK properly after being downloaded over boot code.
//
bool ApplicationACKsProperlyAfterDownloadOverBootCode(const char *Code, int SizeOfCode)
{
	int		LocationWhereBootACKFlagWasFound;

	return(SearchForTextFlag(Code, SizeOfCode, "BOOTACK", &LocationWhereBootACKFlagWasFound));
} // RunFromROM





//
//	DetermineApplicationType
//		Determine application type based on embedded flags.  If no type is present, return MATRIX_APPLICATION_TYPE_UNKNOWN.
//
bool DetermineApplicationType( const char *Code, int SizeOfCode, int *ApplicationType, int *ApplicationMask,
                               int *DownloadMask )
{
	bool	FoundAHeader = true;				// Assume success until we know otherwise.
	int		LocationWhereFlagWasFound;

	if( FindMatrixApplicationHeader( Code, SizeOfCode ) < 0 )
	{
		FoundAHeader = false;
	}
	else if( SearchForTextFlag( Code, SizeOfCode, "CORDED", &LocationWhereFlagWasFound ) )
	{
		*ApplicationType = MATRIX_APPLICATION_TYPE_CORDED;
		*ApplicationMask = MATRIX_APPLICATION_TYPE_CORDED_MASK;
		*DownloadMask = MATRIX_APPLICATION_TYPE_CORDED_DOWNLOAD_MASK;
	}
	else if( SearchForTextFlag( Code, SizeOfCode, "RFBASE", &LocationWhereFlagWasFound ) )
	{
		*ApplicationType = MATRIX_APPLICATION_TYPE_RFBASE;
		*ApplicationMask = MATRIX_APPLICATION_TYPE_RFBASE_MASK;
		*DownloadMask = MATRIX_APPLICATION_TYPE_RFBASE_DOWNLOAD_MASK;
	}
	else if( SearchForTextFlag( Code, SizeOfCode, "RFSCAN", &LocationWhereFlagWasFound ) )
	{
		*ApplicationType = MATRIX_APPLICATION_TYPE_RFSCANNER;
		*ApplicationMask = MATRIX_APPLICATION_TYPE_RFSCANNER_MASK;
		*DownloadMask = MATRIX_APPLICATION_TYPE_RFSCANNER_DOWNLOAD_MASK;
	}
	else if( SearchForTextFlag( Code, SizeOfCode, "PRODCFG", &LocationWhereFlagWasFound ) )
	{
		*ApplicationType = MATRIX_APPLICATION_TYPE_PRODUCT_CONFIGURATION;
		*ApplicationMask = MATRIX_APPLICATION_TYPE_PRODUCT_CONFIGURATION_MASK;
		*DownloadMask = MATRIX_APPLICATION_TYPE_PRODUCT_CONFIGURATION_DOWNLOAD_MASK;
	}
	else if( SearchForTextFlag( Code, SizeOfCode, "AFCORDED", &LocationWhereFlagWasFound ) )
	{
		*ApplicationType = MATRIX_APPLICATION_TYPE_ASIAN_FRIENDLY_CORDED;
		*ApplicationMask = MATRIX_APPLICATION_TYPE_ASIAN_FRIENDLY_CORDED_MASK;
		*DownloadMask = MATRIX_APPLICATION_TYPE_ASIAN_FRIENDLY_CORDED_DOWNLOAD_MASK;
	}
	else if( SearchForTextFlag( Code, SizeOfCode, "AFRFBASE", &LocationWhereFlagWasFound ) )
	{
		*ApplicationType = MATRIX_APPLICATION_TYPE_ASIAN_FRIENDLY_RFBASE;
		*ApplicationMask = MATRIX_APPLICATION_TYPE_ASIAN_FRIENDLY_RFBASE_MASK;
		*DownloadMask = MATRIX_APPLICATION_TYPE_ASIAN_FRIENDLY_RFBASE_DOWNLOAD_MASK;
	}
	else if( SearchForTextFlag( Code, SizeOfCode, "AFRFSCAN", &LocationWhereFlagWasFound ) )
	{
		*ApplicationType = MATRIX_APPLICATION_TYPE_ASIAN_FRIENDLY_RFSCANNER;
		*ApplicationMask = MATRIX_APPLICATION_TYPE_ASIAN_FRIENDLY_RFSCANNER_MASK;
		*DownloadMask = MATRIX_APPLICATION_TYPE_ASIAN_FRIENDLY_RFSCANNER_DOWNLOAD_MASK;
	}
	else
	{
		*ApplicationType = MATRIX_APPLICATION_TYPE_UNKNOWN;
		*ApplicationMask = MATRIX_APPLICATION_TYPE_UNKNOWN_MASK;
		*DownloadMask = MATRIX_APPLICATION_TYPE_UNKNOWN_DOWNLOAD_MASK;
	}

	return FoundAHeader;
} // DetermineApplicationType

#endif	// end MATRIX_BOOT_CODE
// *********************************************************************************************************************************




