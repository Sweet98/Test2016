//======================================================================================
// MatrixHeader.h
//======================================================================================
// $RCSfile: TEMP_SHARED/MatrixHeader.h $
// $Revision: 1.1 $
// $Date: 2008/04/09 10:27:11EDT $
//======================================================================================

//======================================================================================




#ifndef	_MATRIX_HEADER_H
#define	_MATRIX_HEADER_H



#define	SIZE_OF_MATRIX_BOOT_HEADER				24
#define	SIZE_OF_MATRIX_APPLICATION_HEADER		24



// Find the Matrix boot header in the code sent by the caller.
	int FindMatrixBootHeader(const char *Code, int SizeOfCode);

// Find the Matrix application header in the code sent by the caller.
	int FindMatrixApplicationHeader(const char *Code, int SizeOfCode);

// Determine the device type by finding the Matrix boot header in the boot code and the device type that immediately follows.
	int GetDeviceType(const char *Code, int SizeOfCode);

// Match the device type mask to device type in code sent by the caller.
	bool MatchApplicationType(const char *CodeToMatch, int SizeOfCodeToMatch, const char *CodeContainingDeviceType, int SizeOfCodeContainingDeviceType);

// Search for a text flag (case insensitive).
	bool SearchForTextFlag(const char *Code, int SizeOfCode, const char *TextFlag, int *LocationWhereFlagWasFound);

// Search for a text flag according to it's index.
	bool SearchForTextFlagByIndex( const char *Code, int SizeOfCode, int TextFlagIndex, int *LocationWhereFlagWasFound );

// Determine if the code pointed to by the caller is RAM only and should not be written to flash.
	bool RAMOnly(const char *Code, int SizeOfCode);

// Determine if the code pointed to by the caller is to be run from ROM.
	bool RunFromROM(const char *Code, int SizeOfCode);

// Determine if the code pointed to by the caller will ACK properly after being downloaded over boot code.
	bool ApplicationACKsProperlyAfterDownloadOverBootCode( const char *Code, int SizeOfCode );


	#define	MATRIX_APPLICATION_TYPE_UNKNOWN									0
	#define	MATRIX_APPLICATION_TYPE_CORDED									1
	#define	MATRIX_APPLICATION_TYPE_RFBASE									2
	#define	MATRIX_APPLICATION_TYPE_RFSCANNER								3
	#define	MATRIX_APPLICATION_TYPE_PRODUCT_CONFIGURATION					4
	#define	MATRIX_APPLICATION_TYPE_ASIAN_FRIENDLY_CORDED					5
	#define	MATRIX_APPLICATION_TYPE_ASIAN_FRIENDLY_RFBASE					6
	#define	MATRIX_APPLICATION_TYPE_ASIAN_FRIENDLY_RFSCANNER				7

	#define	MATRIX_APPLICATION_TYPE_UNKNOWN_MASK							0xFFFFFFFF
	#define	MATRIX_APPLICATION_TYPE_CORDED_MASK								0x00000011
	#define	MATRIX_APPLICATION_TYPE_RFBASE_MASK								0x00000022
	#define	MATRIX_APPLICATION_TYPE_RFSCANNER_MASK							0x00000044
	#define	MATRIX_APPLICATION_TYPE_PRODUCT_CONFIGURATION_MASK				0xFFFFFFFF
	#define	MATRIX_APPLICATION_TYPE_ASIAN_FRIENDLY_CORDED_MASK				0x00000011
	#define	MATRIX_APPLICATION_TYPE_ASIAN_FRIENDLY_RFBASE_MASK				0x00000022
	#define	MATRIX_APPLICATION_TYPE_ASIAN_FRIENDLY_RFSCANNER_MASK			0x00000044

	#define	MATRIX_APPLICATION_TYPE_UNKNOWN_DOWNLOAD_MASK					0xFFFFFFFF
	#define	MATRIX_APPLICATION_TYPE_CORDED_DOWNLOAD_MASK					0x00000011
	#define	MATRIX_APPLICATION_TYPE_RFBASE_DOWNLOAD_MASK					0x00000066
	#define	MATRIX_APPLICATION_TYPE_RFSCANNER_DOWNLOAD_MASK					0x00000044
	#define	MATRIX_APPLICATION_TYPE_PRODUCT_CONFIGURATION_DOWNLOAD_MASK		0xFFFFFFFF
	#define	MATRIX_APPLICATION_TYPE_ASIAN_FRIENDLY_CORDED_DOWNLOAD_MASK		0x00000011
	#define	MATRIX_APPLICATION_TYPE_ASIAN_FRIENDLY_RFBASE_DOWNLOAD_MASK		0x00000066
	#define	MATRIX_APPLICATION_TYPE_ASIAN_FRIENDLY_RFSCANNER_DOWNLOAD_MASK	0x00000044

// Determine application type based on embedded flags.  If no type is present, return MATRIX_APPLICATION_TYPE_UNKNOWN.
	bool DetermineApplicationType( const char *Code, int SizeOfCode, int *ApplicationType, int *ApplicationMask,
					int *DownloadMask );


#endif



