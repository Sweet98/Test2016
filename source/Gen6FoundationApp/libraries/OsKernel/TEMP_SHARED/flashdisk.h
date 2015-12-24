/************************************************************************/
//
// HHP 4480 embedded firmware
// Flash disk management definitions and interfaces
//
//  Date:       3/20/02
//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: TEMP_SHARED/flashdisk.h $
// $Revision: 1.1 $
// $Date: 2008/04/09 10:27:06EDT $
//======================================================================================


//---------------------------------------------------------------------------
//
//  Class definitions for MATRIX_FLASH_DISK
//		Please see FlashDisk.cpp for a detailed description of this class.
//
//---------------------------------------------------------------------------
#ifndef FlashDiskH
#define FlashDiskH
//---------------------------------------------------------------------------


class HSTRING;



// Reserved entries are areas encompassed by the flash disk that should never be written to.  We keep track of these by making
// a file named "RESERVED" and marked as FD_RESERVED_SPACE.
	typedef struct {
		unsigned int		Location;
		unsigned int		Size;
	} FD_AREA;
	
	#define	FD_RESERVED_AREA		FD_AREA


// Description of the main application (used by boot) and the flash disk directory file.  The information pertaining to the main application
// is stored in this format in flash so that the boot code, which isn't sophisticated enough to understand flash disk organization, can
// at least determine how to load and run the main application.  Also, the flash disk directory file size and CRC are stored here so that
// the main application can determine if there is any corruption on the flash disk.
#define	RUN_MAIN_APPLICATION_FROM_ROM_FLAG_VALUE		0xFDB9ECA8	// If main application is to be run from ROM, this value is stored in
															//
	typedef struct {
		unsigned int		MainApplicationRunFromROMFlagSetting;
		unsigned int		MainApplicationChecksum;
		unsigned int		MainApplicationLocation;
		unsigned int		MainApplicationSize;
		unsigned int		MainApplicationCRC;
		unsigned int		FlashDirectorySize;
		unsigned int		FlashDirectoryCRC;
    } ENTRY_POINT_INFORMATION;




class FLASH_ACCESS
{
public:
// The following functions are required in order to support flash disk reads and writes.  It is the responsibility of the
// instantiator to override these, which requires that they use this class as a base class.
	virtual bool ReadFromFlash(char * /*Data*/, int /*AmountOfDataToRead*/, unsigned int /*FlashAddressToReadFrom*/)   { return(false); }
	virtual bool WriteToFlash(const char * /*Data*/, int /*AmountOfDataToWrite*/, unsigned int /*FlashAddressToWriteTo*/)   { return(false); }
	
public:
// Constructor / destructor
	FLASH_ACCESS( void );
	~FLASH_ACCESS( void );
};




//
//	The purpose of the SECTOR_INFORMATION class is to facilitate temporary storage of various data to be flashed and then do it all at once,
//	making most efficient use of time by writing data in shared sectors only once.
//
class SECTOR_INFORMATION
{
private:
	FLASH_ACCESS	*FlashAccess;		// Need a pointer to flash write routines.

public:
// Keep track of which sectors have been modified and the data that is in them.
	typedef struct {
		unsigned int	Address;
		int				Size;
		bool			Modified;
		HSTRING			*Data;
	} SECTOR_DATA;


	unsigned int	SizeOfWritableFlash;


	SECTOR_DATA		*SectorData;
	int				NumberOfSectors;

	int				*SectorUpdateOrder;
	int				NumberOfSectorsThatHaveBeenUpdated;

// Constructor / destructor
public:
	SECTOR_INFORMATION( FLASH_ACCESS *FlashAccess, unsigned int AvailableFlash, int *SectorAddresses = 0, int *SectorSizes = 0, int NumberOfSectors = 0 );
	~SECTOR_INFORMATION( void );

// Free all allocated data space.
	void FreeDataSpace( void );

// Find the sector referenced by the caller specified address.
	int FindSectorNumber( unsigned int Location );

// Write data to the buffers.
	bool Write(const char *Data, int AmountOfDataToWrite, unsigned int FlashAddressToWriteTo );

// Flush buffers to flash
	bool FlushBuffersToFlash( void );
};





class MATRIX_FLASH_DISK : public FLASH_ACCESS
{
public:
	FD_RESERVED_AREA	*ReservedAreas;
	int					NumberOfReservedAreas;

public:
	FD_AREA				*RestrictedAreas;
	int					NumberOfRestrictedAreas;

public:
// Keep global load status flag up to date in case this is called via instantiation where we can't return the status directly.
	int		StatusOfLastLoadOperation;
	bool	FlashDiskWasNotWhereWeExpectedToFindIt;


// Alignment of each entry.
	#define	DEFAULT_ALIGNMENT_BIT		1

	int		AlignOnBit;

public:
	#define	FD_DEFAULT_NUMBER_OF_WRITE_RETRIES		3
	int		NumberOfRetries;


private:
	SECTOR_INFORMATION	*SectorInformation;


public:		// User declarations

// Status return codes for various flash disk operations.
	enum {
		FD_OK = 0,
		FD_FILE_NOT_FOUND = -1,
		FD_RESERVED_NAME = -2,
		FD_FIXED_LOCATION_OUTSIDE_RANGE = -3,
		FD_FILE_NAME_TOO_LONG = -4,
		FD_FLASH_WRITE_FAILURE = -5,
		FD_CONFLICT_ADDING_FIXED_FILE_OVER_FIXED_FILE = -6,
		FD_DISK_SPACE_UNAVAILABLE = -7,
		FD_CORRUPT_FLASH_DIRECTORY = -8,
		FD_CREATE_FLASH_DIRECTORY_MEMORY_ERROR = -9,
		FD_CORRUPT_FILE = -10,
		FD_FIXED_LOCATION_WRITE_FAILURE = -11,
		FD_RESERVED_ENTRY_FALLS_IN_FLASH_DISK_SECTOR = -12,
		FD_FIXED_LOCATION_IS_NOT_ALIGNED = -13,
		FD_FILE_NAME_TOO_SHORT = -14,
		FD_FILE_NAME_NOT_ALPHANUMERIC = -15,
		FD_ATTEMPT_TO_OVERWRITE_FILE_OF_DIFFERENT_TYPE = -16,
		FD_WRITE_ATTEMPTED_WITH_ILLEGAL_FILE_TYPE = -17,
		FD_FILE_ALREADY_EXISTS = -18,
		FD_BAD_FILE_NAME = -19,
		FD_RESTRICTED_ENTRY_FALLS_IN_FLASH_DISK_SECTOR = -20
	} MATRIX_FLASH_DISK_OPERATION_STATUS;


// Flash disk file types.
//		NOTE:	As types are added, if they should not be allowed to be written, make sure you add them to the list of forbidden types in
//				the write routine.
	enum {
		FD_FREE_SPACE = 0,
		FD_FLASH_DISK_DIRECTORY,
		FD_RESERVED,
		FD_MAIN_APPLICATION,
		FD_MAIN_APPLICATION_RUN_FROM_ROM,
		FD_WORKING_READER_CONFIGURATION,
		FD_DEFAULT_READER_CONFIGURATION,
		FD_MAIN_SCRIPT,
		FD_EDIT_SCRIPT,
		FD_OTHER,
		FD_RESERVED_SPACE,
		FD_PROKEY_FILE,
		FD_RF_SCANNER_TABLE,
		FD_SCAN_ENGINE_FIRMWARE,
		FD_RESTRICTED_SPACE,
		FD_MAXIMUM_FLASH_DISK_ENTRY_TYPES
	} MATRIX_FLASH_DISK_ENTRY_TYPE;


// Special file placement flags
	enum {
		FD_NO_SPECIAL_PLACEMENT = 0,
		FD_FORCE_TO_FLASH_DISK_SECTOR_IF_AVAILABLE,
		FD_FORCE_TO_FIXED_LOCATION,
		FD_FORCE_TO_FIXED_LOCATION_IF_AVAILABLE,
		FD_FORCE_TO_FIXED_LOCATION_IN_RESTRICTED_SPACE,
		FD_FORCE_TO_RESTRICTED_SPACE,
		FD_FORCE_TO_RESTRICTED_SPACE_IF_AVAILABLE
	} MATRIX_FLASH_DISK_ENTRY_PLACEMENT_CODES;


// Modifier flags
	enum {
		FD_NO_MODIFIERS								= 0,
		FD_MODIFIER_FLAG_HIDDEN_FILE				= 0x00000001
	} MATRIX_FLASH_DISK_MODIFIER_FLAGS;


	#define		MAXIMUM_FLASH_DISK_NAME_SIZE	40


// Flash disk file entry definition
	typedef struct {
		int				Type;				// MATRIX_FLASH_DISK_ENTRY_TYPE
		int				Placement;			// Fixed position information.
		int				Modifier;			// Modifier flags
		unsigned int	Size;				// Actual size of the file.
		unsigned int	ReservedSize;		// Size taken up in flash to store the file (must be >= Size).
		unsigned int	CRC;				// CRC of the file (# bytes used in the CRC is specified by Size).
		unsigned int	Checksum;			// Checksum of the file (# bytes used in the checksum is specified by Size).
		unsigned int	Location;			// Location of the file in flash (relative to the start of flash).
		unsigned int	OriginalLocation;	// Used when this entry is being moved; when the updated entries are being
											//	written back to flash, this indicates the current location of the file in
											//	flash where we'll read from and Location now points to it's new location
											//	in flash.
		char			Name[MAXIMUM_FLASH_DISK_NAME_SIZE+1];
		bool			Updated;			// Set to true when an entry needs to be written or rewritten to flash, either
											//	because it's changed location or the contents are new.
	} MATRIX_FLASH_DISK_ENTRY;


// General information about the flash we're operating with.
	unsigned int		FlashOffsetInMappedMemory;	// Start of flash in mapped physical memory.
	unsigned int		SizeOfWritableFlash;
	unsigned int		FlashDiskBaseAddress;
	unsigned int		SizeOfFlashDiskSector;
	int					FlashDiskFilePlacement;


// Description of the main application (used by boot) and the flash disk directory file.
	ENTRY_POINT_INFORMATION		EntryPointInformation;


	#define		NUMBER_OF_MATRIX_FLASH_DISK_ENTRIES_TO_ADD_ON_REALLOCATE		10
	MATRIX_FLASH_DISK_ENTRY		*MatrixFlashDiskEntries;
	int							NumberOfMatrixFlashDiskEntries;
	int							MaximumNumberOfMatrixFlashDiskEntries;

	#define		FLASH_DISK_BOUNDARY_ON_REALLOCATE								512

// Constructors
	void Initialize(unsigned int FlashOffset, unsigned int AvailableFlash, unsigned int DiskBaseAddress, unsigned int SizeOfDiskSector,
						int NumberOfReservedEntries, FD_RESERVED_AREA *InputReservedAreas, int *SectorAddresses = 0, int *SectorSizes = 0,
						int NumberOfSectors = 0, int NumberOfRestrictedEntries = 0, FD_AREA *InputRestrictedAreas = 0,
						int InputFlashDiskFilePlacement = FD_FORCE_TO_FIXED_LOCATION );
	MATRIX_FLASH_DISK(unsigned int FlashOffset, unsigned int AvailableFlash, unsigned int DiskBaseAddress, unsigned int SizeOfDiskSector,
						int NumberOfReservedEntries, FD_RESERVED_AREA *InputReservedAreas, int *SectorAddresses = 0, int *SectorSizes = 0,
						int NumberOfSectors = 0, int NumberOfRestrictedEntries = 0, FD_AREA *InputRestrictedAreas = 0,
						int InputFlashDiskFilePlacement = FD_FORCE_TO_FIXED_LOCATION );
	MATRIX_FLASH_DISK(unsigned int FlashOffset, unsigned int AvailableFlash, unsigned int DiskBaseAddress, unsigned int SizeOfDiskSector,
						const char *ExistingFlashDisk, int NumberOfReservedEntries, FD_RESERVED_AREA *InputReservedAreas, int *SectorAddresses = 0,
						int *SectorSizes = 0, int NumberOfSectors = 0, int NumberOfRestrictedEntries = 0, FD_AREA *InputRestrictedAreas = 0,
						int InputFlashDiskFilePlacement = FD_FORCE_TO_FIXED_LOCATION );

// Destructor
	~MATRIX_FLASH_DISK(void);

// Check the file name to see if the length and content are OK.
	int ValidateFileName( const char *Name, bool OverrideCharacterRangeCheck = false );

// Find the index of the entry at the specified address.
	int FindEntryAtAddress(unsigned int Address);

// Find a flash disk file.
	int Find(const char *Name);

// Read a flash disk file.
	int Read(const char *Name, HSTRING *FileContents);
	int Read(const char *Name, const unsigned char *&pContents, unsigned int &Size);
	int Read(const char *Name, const char *&pContents, int &Size);

// Compute the reserved size such that the file fills to the remainder of it's ending sector.
	int SetReservedSizeToEndSectorBoundary( unsigned int Location, unsigned int FileSize );

// Create a new entry such that all the sectors it resides in are reserved exclusively for it.
	int CreateNewEntryAndReserveEntireSectors(unsigned int *NewFileLocationInFlash, unsigned int *ReservedSize);
	
public:
//	FLASH_DISK_WRITE_FILE_LIST contains information for a file that is to be written to flash disk.
	typedef struct {
		const char		*Name;
		const char		*FileContents;
		int				FileSize;
		int				Type;
		int				Placement;
		int				Modifier;
		unsigned int	FixedLocation;
		unsigned int	ReservedSize;
		bool			ReserveToSectorBoundary;
		int				FileIndex;
	} FLASH_DISK_WRITE_FILE_LIST;

// Write a list of flash disk files.
	int WriteFiles( FLASH_DISK_WRITE_FILE_LIST *WriteFileList, int NumberOfFilesToWrite, bool UpdateBackupFlashDisk = false );

// Write a flash disk file.
	int Write( const char *Name, const char *FileContents, int FileSize, int Type, int Placement, unsigned int FixedLocation = 0, unsigned int ReservedSize = 0,
										bool UpdateBackupFlashDisk = false, bool ReserveToSectorBoundary = false, int Modifier = 0 );

public:
// Remove a flash disk file
	int RemoveFile( const char *Name );

// Rename a flash disk file
	int RenameFile( const char *OldName, const char *NewName, bool UpdateFlash = true );

// Parse the flash disk file sent by the caller and create a local flash disk directory.
	int ValidateFlashDiskFile(const char *ExistingFlashDisk, bool UnknownMainApplicationLocation,
										ENTRY_POINT_INFORMATION *EntryPointInformationForMainApplication, bool ValidateOnly = false);

// Parse the flash disk file sent by the caller and create a local flash disk directory.  If not valid, search through flash for a valid flash disk file.
	int LoadFlashDiskFile(const char *ExistingFlashDisk, bool UnknownMainApplicationLocation = true);

// Create a flash disk file from the local flash disk directory.
	int CreateFlashDiskFile(HSTRING *NewFlashDisk);

// Create a formatted directory report that can be displayed or printed.
	void CreateFormattedDirectoryReport(HSTRING *Report, bool ReportHiddenSettings = false );
	
// Create a formatted information dump, including directory information, that can be displayed or printed.
	void CreateFormattedInformationDump(HSTRING *Report);
	
// Indicate to the caller whether the current main application is RAM based (true) or ROM based (false).
	bool MainApplicationIsRAMBased( void );

// Check the application type (ROM or RAM based) against what the caller says it should be and if it differs,
// update it here and flush the updated flash disk out to flash.
	int ValidateMainApplicationType( bool MainApplicationIsROMBased );
	
// Return sector information for the sector that the Location, sent by the caller, falls in.
	bool GetSectorInformation( unsigned int Location, int *SectorNumber, unsigned int *StartAddress, unsigned int *Size,
										bool *Modified );



private:	// User declarations

//
// Tag Definitions
//
//	Tags consist of 32 bits where the first 16 bits are the tag number and the second 16 bits are a length field.
//	Tag groups (such as size fields), should be grouped on boundaries of 100.  The length field represents the
//	number of bytes in the field.
//
// Define sizes
    #define     SIZE_OF_FLASH_DIRECTORY_TAG_NUMBER					2
    #define     SIZE_OF_FLASH_DIRECTORY_TAG_LENGTH					2

// General Information Group

// Directory Entries Group
	#define		FD_StartOfNewDirectoryEntryMarker					100
	#define		FD_DirectoryEntryTypeTag							101
	#define		FD_DirectoryEntryPlacementTag						102
	#define		FD_DirectoryEntrySizeTag							103
	#define		FD_DirectoryEntryReservedSizeTag					104
	#define		FD_DirectoryEntryCRCTag								105
	#define		FD_DirectoryEntryChecksumTag						106
	#define		FD_DirectoryEntryLocationTag						107
	#define		FD_DirectoryEntryNameTag							108
	#define		FD_DirectoryEntryModifierTag						109


// Zero out an entry by setting all parameters to defaults.
	void ZeroEntry( int FileIndex );

// Force a reserved space entry to the location specified by the caller.
	int MakeReservedEntry(unsigned int Location, unsigned int Size);

// Force a restricted space entry to the location specified by the caller.
	int MakeRestrictedEntry(unsigned int Location, unsigned int Size);

// Add space for new entries in the directory.
	void AddFlashDiskEntries(int NumberOfEntriesToAdd);

// Insert unused entries in the directory.
	void InsertUnusedDirectoryEntries(int FileIndexToInsertAt, int NumberOfEntriesToInsert);

// See if the two address ranges referenced by the caller overlap.
	typedef enum
	{
		FD_RANGE_CHECK_ERROR = 0,
		FD_RANGE_CHECK_RANGE1_BELOW_RANGE2,
		FD_RANGE_CHECK_RANGE1_STARTS_BELOW_RANGE2_AND_ENDS_INSIDE_RANGE2,
		FD_RANGE_CHECK_RANGE1_EQUALS_RANGE2,
		FD_RANGE_CHECK_RANGE1_IS_INSIDE_RANGE2,
		FD_RANGE_CHECK_RANGE2_IS_INSIDE_RANGE1,
		FD_RANGE_CHECK_RANGE1_STARTS_INSIDE_RANGE2_AND_ENDS_ABOVE_RANGE2,
		FD_RANGE_CHECK_RANGE1_ABOVE_RANGE2
	} FD_RANGE_CHECK_RETURN_CODES;

	bool CompareAddressRanges( unsigned int StartAddress1ToCheck, unsigned int Size1ToCheck,
													unsigned int StartAddress2ToCheck, unsigned int Size2ToCheck,
													int *ReturnCode );

// See if the two address ranges referenced by the caller overlap.
	bool DoesAddressRangeFallInRestrictedSpace( unsigned int StartAddressToCheck, unsigned int SizeToCheck,
							int *ReturnCode, unsigned int *StartAddressOfOverlappingRestrictedSpace, unsigned int *SizeOfOverlappingRestrictedSpace );

// Mark a directory entry as empty.
	void MarkEntryAsEmpty(int FileIndex, unsigned int Location, unsigned int Size );

// Insert an entry in the directory for a new file.  Note that it is up to the caller to have verified that there is space for the entry
// at the caller specified location.
	int InsertDirectoryEntryForNewFile(unsigned int FixedLocation, unsigned int ReservedSize,
													bool AllowEntryToBeInRestrictedSpace = false );

// Find the best fit for the amount of space requested by the caller.  Avoid getting space from the flash disk sector unless space cannot
// be obtained anywhere else.
	int FindBestFit(unsigned int *LocationOfBestFitEntry, unsigned int ReservedSize);

// Coalesce all empty entries to defragment flash.
	void RecoverFragments(void);

// Create a new entry in the flash disk sector, if possible.
	int CreateNewEntryInFlashDiskSector(unsigned int *NewFileLocationInFlash, unsigned int ReservedSize);

// Create a new entry in the best fit location in flash.
	int CreateNewEntry(unsigned int *NewFileLocationInFlash, unsigned int ReservedSize);

// Create a new entry in the directory at a fixed location.
	int CreateNewEntryAtFixedLocation(unsigned int FixedLocation, unsigned int ReservedSize,
						bool ForceThisFileOverAnotherFixedFile = false, bool AllowEntryToBeInRestrictedSpace = false );

// Create a new flash disk file entry in the flash disk, but don't update flash.
	int CreateNewFileEntry( const char *Name, const char *FileContents, int FileSize, int Type, int Placement, unsigned int FixedLocation,
										unsigned int *ReservedSize, bool ReserveToSectorBoundary, int Modifier );

// For all files that are marked as updated, read the originals from flash and then write them back to their new locations.
	int WriteUpdatedEntries( bool UpdateBackupFlashDisk = false, FLASH_DISK_WRITE_FILE_LIST *WriteFileList = 0, int NumberOfFilesToWrite = 0 );

};
#endif



