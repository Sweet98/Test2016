//======================================================================================
// ReaderConfigurationStorage.h
//======================================================================================
// $RCSfile: readerconfigurationstorage.h $
// $Revision: 1.13 $
// $Date: 2011/11/11 17:01:43EST $
//======================================================================================

//======================================================================================




// Process for inserting new configuration entries:
//

//---------------------------------------------------------------------------
//
//  Class definitions for ReaderConfiguration
//
//---------------------------------------------------------------------------
#ifndef ReaderConfigurationStorageH
#define ReaderConfigurationStorageH
//---------------------------------------------------------------------------


#include "HSTRING.h"		// HSTRING interface definition
#include "stricmp.h"
#include "strnicmp.h"

enum DEFERRED_RESET_STATE_t
{
	DEFERRED_RESET_NOT_ALLOWED,
	DEFERRED_RESET_ALLOWED,
	DEFERRED_RESET_PENDING,
};

extern DEFERRED_RESET_STATE_t deferred_reset_state;

bool deferred_reset();

extern bool ProcessingScannedMenu;


//	Setting validation return codes
//		These return codes are used any time we're trying to process a command string.  There are four possible conditions
//		encountered when evaluating a command string:
//		- The setting is valid, meaning that it matches all length, range, and character set requirements of a given type.
//		- The setting is not valid, because it is not complete, but the data received so far does not preclude it being
//			valid.  This is useful information when processing a barcode command where the setting may be sent piecemeal.
//		- The setting is not only valid, but if it were any longer, it would be invalid because it would exceed length or
//			range requirements.  This is useful information when processing a barcode command.
//		- The setting is not valid.

// Setting value is fixed and can't be changed.
#define		RDR_CFG_ATTEMPT_TO_CHANGE_FIXED_SETTING		-3

// Invalid value for this setting
#define		RDR_CFG_SETTING_INVALID						-2

// Value is valid, but not complete
#define		RDR_CFG_PARTIAL_SETTING_VALID				-1

// Value is valid, but not the maximum allowable length
#define		RDR_CFG_SETTING_VALID						1

// Value is valid and is the maximum allowable length
#define		RDR_CFG_SETTING_VALID_MAX_LENGTH			2

#define MENU_ERROR			0x00		// Bit clear when menuing progressed with error.
#define MENU_PASSED			0x01		// Bit set when menuing progressed without error.
#define MENU_QUERY			0x02		// Bit set when a query response is in the message buffer.



class DATA_FORMAT
{

public:	// User declarations

	#define	MAXIMUM_DATA_FORMAT_NUMBER				3
	#define	MAXIMUM_DATA_FORMAT_TERMINAL_TYPE		255
	#define	MAXIMUM_DATA_FORMAT_CODE_IDENTIFIER		255
	#define	MAXIMUM_DATA_FORMAT_LENGTH				9999


	#define	NUMBER_OF_DATA_FORMAT_ENTRIES_TO_ADD_ON_REALLOCATE		10

	typedef struct {
		int					FormatNumber;
		int					TerminalType;
		int					CodeIdentifier;
		int					Length;
		HSTRING				*Command;
	} DATA_FORMAT_ENTRY;


	DATA_FORMAT_ENTRY		*Entries;

	int						NumberOfEntries;


private:
	#define	DATA_FORMAT_FORMATTED_KEY_LENGTH			10

	int						MaximumNumberOfEntries;

// If we're out of entries, add more.
	void AddMoreEntries(void);

public:
// Validate the key.
	bool ValidateKey(int FormatNumber, int TerminalType, int CodeIdentifier, int Length);

// Parse and validate the key.
	int ParseKey(const char *Key, int *FormatNumber, int *TerminalType, int *CodeIdentifier, int *Length);

private:
// Determine the size of a binary command string based on the data formats.
	int SizeOfBinaryCommandString(int SizeOfAdditionalEntry = -1);

// Build a binary command string based on the data formats.  Every time a change is made, this must be called to update the
// binary command string.
	bool BuildBinaryCommandString(void);



private:
	char					*BinaryCommandString;
	int						MaximumBinaryCommandStringLength;


public:
// Constructor
	DATA_FORMAT(int InputMaximumBinaryCommandStringLength, const char *InputBinaryCommandString);

// Destructor
	~DATA_FORMAT(void);

// Find a data format (overloaded version #1 with a binary key value)
	int Find(int FormatNumber, int TerminalType, int CodeIdentifier, int Length, int StartIndex = 0);

// Find a data format (overloaded version #2 with a formatted key value)
	int Find(const char *Key, int StartIndex = 0);

// Find a data format and return a command string pointer (overloaded version #1 with a binary key value)
	HSTRING *FindCommandString(int FormatNumber, int TerminalType, int CodeIdentifier, int Length, int &Position);

// Find a data format and return a command string pointer (overloaded version #2 with a formatted key value)
	HSTRING *FindCommandString(const char *Key, int &Position);

// Add the format entry sent by the caller by overwriting and existing entry with the same key or creating a new entry.
	bool CreateNewEntryOrOverwriteExistingEntry(int FormatNumber, int TerminalType, int CodeIdentifier, int Length,
				HSTRING *Data);

// Process a data format command string that includes key information.
	int ProcessCommand(const char *CommandString, bool SaveTheResult = true);

// Process a binary command string.
	bool ProcessBinaryCommand(const char *BinaryCommandString, int LengthOfBinaryCommandString);

// Erase a data format (overloaded version #1, using a format number).
	bool Erase(int FormatNumber, int TerminalType, int CodeIdentifier, int Length, bool *EntryWasDeleted);

// Erase a data format (overloaded version #2, using a command string).
	bool Erase(const char *EraseCommandString, bool *EntryWasDeleted);

// Erase all data formats
	bool EraseAll(bool *EntriesWereDeleted);

// Load data formats from a binary command string.  This is the same as ProcessBinaryCommand, except that this first erases
// all existing data format entries.
	bool Load(const char *BinaryCommandString, int LengthOfBinaryCommandString);

// Build a report string based on the data formats.
	void BuildReportString(HSTRING *ReportString, bool IncludeClearAllCommand = false, bool OmitTrailingPipe = true);

private:
	DATA_FORMAT (const DATA_FORMAT&);            // declared private, but not defined, to prevent copying         
	DATA_FORMAT& operator=(const DATA_FORMAT&);  // declared private, but not defined, to prevent copying 

}; // DATA_FORMAT







class PREFIX_SUFFIX
{

public:	// User declarations

	#define	MAXIMUM_PREFIX_SUFFIX_CODE_IDENTIFIER		255


	#define	NUMBER_OF_PREFIX_SUFFIX_ENTRIES_TO_ADD_ON_REALLOCATE		10

	typedef struct {
		int					CodeIdentifier;
		HSTRING				*Command;
	} PREFIX_SUFFIX_ENTRY;


	PREFIX_SUFFIX_ENTRY		*Entries;

	int						NumberOfEntries;


private:
	#define	PREFIX_SUFFIX_FORMATTED_KEY_LENGTH			2

	int						MaximumNumberOfEntries;

// If we're out of entries, add more.
	void AddMoreEntries(void);


public:
// Validate the key.
	bool ValidateKey(int CodeIdentifier);

// Parse and validate the key.
	int ParseKey(const char *Key, int *CodeIdentifier);


private:
// Determine the size of a binary command string based on the prefix or suffix.
	int SizeOfBinaryCommandString(int CodeIdentifier = 0, int SizeOfAdditionalEntry = -1);

// Build a binary command string based on the prefix or suffix.  Every time a change is made, this must be called to update the
// binary command string.
	bool BuildBinaryCommandString(void);



private:
	char					*BinaryCommandString;
	int						MaximumBinaryCommandStringLength;


public:
// Constructor
	PREFIX_SUFFIX(int InputMaximumBinaryCommandStringLength, const char *InputBinaryCommandString);

// Destructor
	~PREFIX_SUFFIX(void);

// Find a prefix or suffix (overloaded version #1 with a binary key value)
	int Find(int CodeIdentifier, int StartIndex = 0);

// Find a prefix or suffix (overloaded version #2 with a formatted key value)
	int Find(const char *Key, int StartIndex = 0);

// Find a data format and return a command string pointer (overloaded version #1 with a binary key value)
	HSTRING *FindCommandString(int CodeIdentifier);

// Find a data format and return a command string pointer (overloaded version #2 with a formatted key value)
	HSTRING *FindCommandString(const char *Key);

// Add the prefix or suffix entry sent by the caller by overwriting and existing entry with the same key or creating a new
// entry.
	bool CreateNewEntryOrOverwriteExistingEntry(int CodeIdentifier, HSTRING *Data);

// Process a prefix or suffix command string that includes key information.
	int ProcessCommand(const char *CommandString, bool SaveTheResult = true);

// Process a binary command string.
	bool ProcessBinaryCommand(const char *BinaryCommandString, int LengthOfBinaryCommandString);

// Erase a data prefix or suffix (overloaded version #1, using a code identifier).
	bool Erase(int EraseCodeIdentifier, bool *EntryWasDeleted);

// Erase a prefix or suffix (overloaded version #2, using a command string).
	bool Erase(const char *EraseCommandString, bool *EntryWasDeleted);

// Erase all prefix or suffixes
	bool EraseAll(bool *EntriesWereDeleted);

// Load prefix or suffix from a binary command string.  This is the same as ProcessBinaryCommand, except that this first erases
// all existing entries.
	bool Load(const char *BinaryCommandString, int LengthOfBinaryCommandString);

// Build a report string based on the prefix or suffix.
	void BuildReportString(HSTRING *ReportString, bool IncludeClearAllCommand = false, bool OmitTrailingPipe = true);

private:
	PREFIX_SUFFIX (const PREFIX_SUFFIX&);            // declared private, but not defined, to prevent copying         
	PREFIX_SUFFIX& operator=(const PREFIX_SUFFIX&);  // declared private, but not defined, to prevent copying 

}; // PREFIX_SUFFIX







class SUB_TAG_TABLE
{
private:
	int		NumberOfWorkgroups;					// For workgroup settings, define the number of workgroups.


public:
	#define	SIZE_OF_SUB_TAG												3


public:

	#define	READER_CONFIGURATION_DEFAULT_STRING_ALLOCATION				20

	#define	NUMBER_OF_SUB_TAG_TABLE_ENTRIES_TO_ADD_ON_REALLOCATE		10

	#define	MAX_LIST_ITEMS											80

	#define	INVALID_LIST_ITEM										-1



	typedef struct {
		union {
			char			*c;
			bool			b;
			int				i;
			HSTRING			*BinaryString;
		};

		union {
			DATA_FORMAT		*DataFormat;
			PREFIX_SUFFIX	*PrefixSuffix;
		};

	// This flag indicates when a setting has been updated since the last storage operation.  It is up to the instantiator to clear these, via
	// "UpToDate" routines, when the instantiator has stored settings.
		bool				Updated;

	// This flag indicates when a setting has been modified one or more times since instantiation.  Their purpose is to allow a '~' query as to what
	// has been changed, so that only those settings (such as customer defaults) need to be processed, stored, etc.
		bool				ModifiedSinceInstantiation;
	} SUB_TAG_TABLE_VALUE_ENTRY;



	typedef struct {
		union {
			char			*c;
			bool			*b;
			int				*i;
			HSTRING			*BinaryString;
		};

		union {
			DATA_FORMAT		*DataFormat;
			PREFIX_SUFFIX	*PrefixSuffix;
		};

	// This flag indicates when a setting has been updated since the last storage operation.  It is up to the instantiator to clear these, via
	// "UpToDate" routines, when the instantiator has stored settings.  Not used for standard defaults.
		bool				Updated;

	// This flag indicates when a setting has been modified one or more times since instantiation.  Their purpose is to allow a '~' query as to what
	// has been changed, so that only those settings (such as customer defaults) need to be processed, stored, etc.  Not used for standard defaults.
		bool				ModifiedSinceInstantiation;
	} SUB_TAG_TABLE_INSTANTIATOR_COPY_OF_ACTIVE_VALUE;



	typedef struct {
		char											SubTag[SIZE_OF_SUB_TAG+1];
		unsigned int									TypeFlags;
		unsigned int									MaximumSizeOfActiveValue;

	// For numbers, we keep track of the low and high range.  Sometimes, though, the low or high range may be a separate
	// item rather than a constant.  For example, for a symbology character range, the min must not be greater than the current
	// max setting and the max must not be less than the current min, so we have a cross dependency and need pointers rather
	// than constant values.  Therefore we always access these values by the address.  To accomplish all this, we'll actually
	// store the full range for a setting, but we'll also store the addresses of working values.  If there were no cross
	// dependencies, then the working addresses will point to the full range
		int												LowRange;
		int												HighRange;
		int												*WorkingLowRangeAddress;
		int												*WorkingHighRangeAddress;

		int												CopyCommand;

		int												NumberOfWorkgroups;			// Define the number of workgroups in this particular setting (1
																					// if this isn't a workgroup setting) .

		SUB_TAG_TABLE_VALUE_ENTRY						StandardDefaultValue;

		SUB_TAG_TABLE_VALUE_ENTRY						DefaultValue;

		SUB_TAG_TABLE_VALUE_ENTRY						*WorkingValue;

		SUB_TAG_TABLE_VALUE_ENTRY						*ActiveValue;

		SUB_TAG_TABLE_INSTANTIATOR_COPY_OF_ACTIVE_VALUE	InstantiatorCopyOfActiveValue;	// Updated according to the active value corresponding to the
																						//	current workgroup.

		int												NumberOfEntriesInListRange;
		int												*NumericListRange;
		char											**StringListRange;

		int												SpecialCommandAction;

		bool											(*EntryFunction) (void);
		bool											(*ExitFunction) (void);
	} SUB_TAG_TABLE_ENTRY;


	SUB_TAG_TABLE_ENTRY		*Entries;
	unsigned int			*EntryIndex;

	unsigned int			NumberOfEntries;


private:
	unsigned int			MaximumNumberOfEntries;

// If we're out of sub-tag table entries, add more.
	void AddMoreEntries(void);


public:
	typedef enum {
		STRING											= 0x00000001,
		COMMAND											= 0x00000002,
		BOOLEAN											= 0x00000004,
		NUMERIC											= 0x00000008,
		NUMERIC_LIST									= 0x00000010,
		STRING_LIST										= 0x00000020,
		HEX_STRING										= 0x00000040,
		SYMBOLOGY_ENABLE_COMMAND						= 0x00000080,
		FORMAT_STRING									= 0x00000100,
		PREFIX_SUFFIX_STRING							= 0x00000200,
		TWO_DIGIT_HEX									= 0x00000400,
		RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND			= 0x00000800,
		STRING_MUST_BE_QUOTED							= 0x00001000,
		BINARY_STRING									= 0x00002000,
		UNICODE_STRING									= 0x00004000,
		BASE_ONLY_SETTING								= 0x00008000,
		WORKGROUP_SETTING								= 0x00010000,
		WORKGROUP_SELECTION								= 0x00020000,
		DEFAULTING_DISABLED								= 0x00040000,
		SCANNER_ONLY_SETTING							= 0x00080000,
		ASCII_STRING									= 0x00100000,
		PASS_QUERY_CHARACTER_TO_ENTRY_EXIT_FUNCTIONS	= 0x00200000,
		WORKTABLE_SELECTION							= 0x00400000,                             //CBLTBL
		CABLE_SETTING									= 0x00800000,                            //   CBL232,CBLUSB,CBLRTL,CBLUSB
		ONLY_AVAILABLE_WITH_DEFAULT_COMMAND_PRIVILEGE	= 0x02000000,
		FIXED_EXCEPT_ACTIVE								= 0x04000000,
		IGNORE_ON_STORAGE_QUERY							= 0x08000000,
		FULLY_VISIBLE									= 0x10000000,
		VISIBLE_BY_TAG									= 0x20000000,
		VISIBLE_BY_SUB_TAG								= 0x40000000,
		FIXED											= 0x80000000
	} CONFIGURATION_SETTING_MODIFIERS;



	typedef enum {
		NO_ACTION = 0,
		SET_ALL_WORKING_AND_ACTIVE_AND_DEFAULT_VALUES_TO_STANDARD_DEFAULTS,
		SET_ALL_WORKING_AND_ACTIVE_VALUES_TO_STANDARD_DEFAULTS,
		SET_ALL_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT,
		SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG,
		COPY_STANDARD_DEFAULTS_TO_DEFAULTS,
		COPY_WORKING_VALUES_TO_DEFAULTS,
		ENABLE_SYMBOLOGIES,
		CLEAR_FORMAT_ENTRY,
		CLEAR_ALL_FORMAT_ENTRIES,
		CLEAR_PREFIX_SUFFIX_ENTRY,
		CLEAR_ALL_PREFIX_SUFFIX_ENTRIES,
		SET_ALL_WORKING_AND_ACTIVE_AND_DEFAULT_VALUES_TO_STANDARD_DEFAULTS_FOR_ALL_TABLES,
		SET_ALL_WORKING_AND_ACTIVE_VALUES_TO_STANDARD_DEFAULTS_FOR_ALL_TABLES,
		SET_ALL_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_ALL_TABLES
	} MENU_COMMAND_ACTION;



	typedef enum {
		ADD_BASIC_INFORMATION_ILLEGAL_CHARACTERS_IN_NAME			= -1,
		ADD_BASIC_INFORMATION_NAME_ALREADY_EXISTS					= -2,
		ADD_BASIC_INFORMATION_ILLEGAL_FLAG_OR_MODIFIER_COMBINATION	= -3
	} ADD_BASIC_INFORMATION_ERROR_CODES;


// Constructor
	SUB_TAG_TABLE( int InputNumberOfWorkgroups );

// Destructor
	~SUB_TAG_TABLE(void);

// Find an item in the sub-tag table
	bool Find(const char *SettingName, unsigned int *EntryNumber, unsigned int *EntryOrder);

// Create a new sub-tag with basic information.  If the sub-tag already exists, WOOF.
	int AddBasicInformationForNewSubTag(const char *SettingName, bool (*EntryAction) (void), bool (*ExitAction) (void), unsigned int Modifier );

private:
	SUB_TAG_TABLE (const SUB_TAG_TABLE&);            // declared private, but not defined, to prevent copying         
	SUB_TAG_TABLE& operator=(const SUB_TAG_TABLE&);  // declared private, but not defined, to prevent copying 

}; // Class SUB_TAG_TABLE







class TAG_TABLE
{
private:
	int		NumberOfWorkgroups;					// For workgroup settings, define the number of workgroups.


public:
	#define	SIZE_OF_TAG												3

public:	// User declarations

	#define	NUMBER_OF_TAG_TABLE_ENTRIES_TO_ADD_ON_REALLOCATE		10


	typedef struct {
		char				Tag[SIZE_OF_TAG+1];
		SUB_TAG_TABLE		*SubTagTable;
	} TAG_TABLE_ENTRY;


	TAG_TABLE_ENTRY			*Entries;
	unsigned int			*EntryIndex;

	unsigned int			NumberOfEntries;


private:
	unsigned int			MaximumNumberOfEntries;

// If we're out of tag table entries, add more.
	void AddMoreEntries(void);


public:
// Constructor
	TAG_TABLE( int InputNumberOfWorkgroups );

// Destructor
	~TAG_TABLE(void);

// Find an item in the tag table
	bool Find(const char *SettingName, unsigned int *EntryNumber, unsigned int *EntryOrder);

// If the tag already exists, return an index.  Otherwise, create a new tag and return the index of that tag.
	int GetExistingTagOrCreateNewTag(const char *SettingName);

private:
	TAG_TABLE (const TAG_TABLE&);            // declared private, but not defined, to prevent copying         
	TAG_TABLE& operator=(const TAG_TABLE&);  // declared private, but not defined, to prevent copying 

}; // Class TAG_TABLE








class READER_CONFIGURATION_STORAGE
{
private:
	int		NumberOfWorkgroups;					// For workgroup settings, define the number of workgroups.

	int		CurrentWorkgroupNumber;

	int		ActiveWorkgroupNumber;
	int           CurrentTableNumber;
	int		 CurrentTableForScannedCommand;
	int          	ActiveTableNumber;


private:
//	These items control tag compressed format for wildcard queries.  If enabled, settings sharing the same tag will have the tag reported once followed
//	by each sub-tag and value separated by commas.
	bool	TagCompressedModeEnabledForThisCommand;
	bool	TagCompressedModeEnabled;

//	These items control reporting of base-only, common, and workgroup settings.
	bool	ReportBaseOnlySettingsForThisCommand;
	bool	ReportBaseOnlySettings;
	bool	ReportCommonSettingsForThisCommand;
	bool	ReportCommonSettings;
	bool	ReportScannerOnlySettingsForThisCommand;
	bool	ReportScannerOnlySettings;

	typedef enum {
		DO_NOT_REPORT_WORKGROUP_SETTINGS = 0,
		REPORT_AND_SET_ALL_WORKGROUP_SETTINGS_IN_ALL_WORKGROUPS_WITH_COMMON_VALUES,
		REPORT_AND_SET_ALL_WORKGROUP_SETTINGS_IN_CURRENT_WORKGROUP,
		REPORT_AND_SET_ALL_WORKGROUP_SETTINGS_IN_CURRENT_WORKGROUP_THAT_ARE_NOT_COMMON_VALUES,
		REPORT_ALL_WORKGROUP_SETTINGS_IN_ALL_WORKGROUPS_WITH_COMMON_VALUES_FIRST_THEN_WITH_UNCOMMON_VALUES,
		REPORT_ALL_WORKGROUP_SETTINGS_IN_ALL_WORKGROUPS
	} WORKGROUP_SETTINGS_HANDLING;

	WORKGROUP_SETTINGS_HANDLING		WorkgroupSettingsHandlingForThisCommand;
	WORKGROUP_SETTINGS_HANDLING		WorkgroupSettingsHandling;


//	These items control reporting of updated settings.
	typedef enum {
		SETTINGS_UPDATED_SINCE_LAST_COMMAND_NO_MODIFIERS						= 0,
		SETTINGS_UPDATED_SINCE_LAST_COMMAND_EXCLUDE_IF_WORKING_UPDATED			= 0x00000001,
		SETTINGS_UPDATED_SINCE_LAST_COMMAND_EXCLUDE_IF_ACTIVE_UPDATED			= 0x00000002,
		SETTINGS_UPDATED_SINCE_LAST_COMMAND_EXCLUDE_IF_WORKING_NOT_UPDATED		= 0x00000004,
		SETTINGS_UPDATED_SINCE_LAST_COMMAND_EXCLUDE_IF_ACTIVE_NOT_UPDATED		= 0x00000008
	} SETTINGS_UPDATED_SINCE_LAST_COMMAND_MODIFIERS;

	SETTINGS_UPDATED_SINCE_LAST_COMMAND_MODIFIERS	SettingsUpdatedSinceLastCommandModifiersForThisCommand;
	SETTINGS_UPDATED_SINCE_LAST_COMMAND_MODIFIERS	SettingsUpdatedSinceLastCommandModifiers;

	bool	ReportSettingsUpdatedSinceLastCommandForThisCommand;
	bool	ReportSettingsUpdatedSinceLastCommand;
	bool	ReportSettingsNotUpdatedSinceLastCommandForThisCommand;
	bool	ReportSettingsNotUpdatedSinceLastCommand;
	bool	ReportSettingsUpdatedSinceStartupForThisCommand;
	bool	ReportSettingsUpdatedSinceStartup;
	bool	ReportSettingsNotUpdatedSinceStartupForThisCommand;
	bool	ReportSettingsNotUpdatedSinceStartup;
	bool	ReportSettingsWhenActiveIsEqualToWorkingForThisCommand;
	bool	ReportSettingsWhenActiveIsEqualToWorking;
	bool	ReportSettingsWhenActiveIsNotEqualToWorkingForThisCommand;
	bool	ReportSettingsWhenActiveIsNotEqualToWorking;


// These item controls whether or not wildcard queries are sorted.
	bool	SortWildcardQueries;
	bool	SortWildcardQueriesForThisCommand;

// This item controls whether hidden settings are always reported or if they're reported only via '$'.
	typedef enum {
		HIDDEN_SETTINGS_REPORT_INVISIBLE			= 0x00000001,
		HIDDEN_SETTINGS_REPORT_VISIBLE_BY_SUB_TAG	= 0x00000002,
		HIDDEN_SETTINGS_REPORT_VISIBLE_BY_TAG		= 0x00000004,
		HIDDEN_SETTINGS_REPORT_FULLY_VISIBLE		= 0x00000008
	} HIDDEN_SETTINGS_REPORTING_MODIFIERS;

	int		ReportHiddenSettingsForThisCommand;
	int		ReportHiddenSettings;


// Allow updates to be turned off entirely.  This can be overridden by the OverrideFixedFlag parameter in the Menu member
// function.
	bool	SettingChangesEnabled;

public:
// Function to return working value in current workgroup if this is a workgroup setting or zero element otherwise.
	__inline SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY *WorkingValueForCurrentWorkgroup( SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntry )
	{
		if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
		{
			return &(SubTagTableEntry->WorkingValue[ CurrentWorkgroupNumber ]);
		}
		else
		{
			return &(SubTagTableEntry->WorkingValue[ 0 ]);
		}
	}

// Function to return active value in current workgroup if this is a workgroup setting or zero element otherwise.
	__inline SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY *ActiveValueForCurrentWorkgroup( SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntry )
	{
		if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
		{
			return &(SubTagTableEntry->ActiveValue[ CurrentWorkgroupNumber ]);
		}
		else
		{
			return &(SubTagTableEntry->ActiveValue[ 0 ]);
		}
	}



public:
//	Note on ULTPRV.
//		I originally created ULTPRV to be a flag word with bits set for various privileges.  It was a bad idea.  Instead I should have
//		done what I subsequently did, which is to create individual boolean settings, such as FlashTestPrivilege and
//		DirectDefaultCommandPrivilege.  Code that uses the ULTPRV flags also looks at the corresponding boolean settings (which are easier
//		for me to remember).  I can't eliminate ULTPRV yet, because it is actually being used by manufacturing and perhaps others.
//		Note that UltimatePrivilegeLevel and DirectDefaultCommandPrivilege are now part of ReaderConfigurationStorage, because we want to control
//		the processing of default commands ('@' terminator) within that class.  The reason for this is that we filtered these commands in the past
//		in the serial command handler, but now we want the query commands to go through even though privilege is off, and we want to block barcode
//		default settings, which we can't do in the serial command handler.
	#define DIRECT_DEFAULT_COMMAND_PRIVILEGE		0x20000000
	int		UltimatePrivilegeLevel;

// Defines whether or not we allow default menu settings to be changed by command.  They can (now) always be queried.  This applies to all settings
// whose terminator is '@'.
	bool	DirectDefaultCommandPrivilege;
	bool AffectedTableChangedToDefalt;
	bool CustomDefaultProgrammingMode;
	bool TwoTableUpdateSimultaneously;
	bool TableSettingUpdateSimultaneouslyForScannedCommand;
	bool  TableSettingCommandisProcessing;
	bool  FirstTimeLoadWorkingConfigurationInFlash;
	bool  ScannedCBLTBLMenu;
	bool   TableNeedChange;


// Defines whether or not we allow settings marked as DEFAULTING_DISABLED to be defaulted.  This is off by default.
	bool	OverrideDefaultingDisabled;


private:	// User declarations

	TAG_TABLE	*TagTable;

// Copy standard default value to default value.
	void CopyStandardDefaultToDefault(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToCopy);

// Copy default value to working value.
	void CopyDefaultToWorking(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToCopy, const char AffectedTable = '@');

// Copy working value to default value.
	void CopyWorkingToDefault(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToCopy);

// Copy active value to working value.
	void CopyActiveToWorking(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToCopy);

// Copy working value to active value.
	void CopyWorkingToActive(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToCopy);

// Copy standard default to default, working, and active values according to working value to active value.
	bool CopyStandardDefaultToDefaultWorkingAndActive(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToCopy,
					bool OverwriteCustomWorkingAndActiveIfModified = false );


public:
//	Bit definitions for the Menu() return byte
//These are the ReaderConfiguration->Menu return values.

	bool				AllActiveValuesWereDefaulted;
	bool				OneOrMoreActiveValuesWereDefaulted;
	bool				OneOrMoreDefaultsUpdated;
	bool				OneOrMoreWorkingValuesUpdated;
	bool				OneOrMoreActiveValuesUpdated;
	bool				OneOrMoreQueryCommandsWereProcessed;
	bool				OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated;
	bool				OneOrMoreWorkgroupValuesInActiveStorageWereUpdated;
	bool				OneOrMoreCommonValuesInWorkingStorageWereUpdated;
	bool				OneOrMoreCommonValuesInActiveStorageWereUpdated;
	bool				OneOrMoreBaseOnlyValuesInWorkingStorageWereUpdated;
	bool				OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated;
	bool				OneOrMoreScannerOnlyValuesInWorkingStorageWereUpdated;
	bool				OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated;
	bool				*OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated;
	bool				*OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated;


public:
	#define		MINIMUM_SIZE_OF_EXTRANEOUS_TEXT_AFTER_COMMAND		100
	HSTRING			*ExtraneousTextAfterCommand;


public:
	HSTRING		*MenuCommandResponse;	// Make the command response string available to entry and exit functions that want to
										//	store their response here instead of putting it out directly.
	bool		ForceExitFunctionToBeExecutedOnSettingChangeError;	// When changing a setting value, the entry function can
																	//	set this to true so that an exit function always gets
																	//	executed, even if the setting value is erroneous.  This
																	//	is always set to false before executing the entry
																	//	function.
	int			ValidateAndChangeSettingStatus;		// Let the exit function know what the status was of a setting value change.

	char		CurrentTable;						// Let the entry and exit functions see which table is being updated.

	bool		ReportResponse;						// Let the entry and exit functions determine whether or not this setting
													//	response should be reported.


public:
	bool	OmitTrailingPipeOnValueQueries;	// Determines whether or not to omit trailing pipe in prefix / suffix /
											// data format value queries.  Trailing pipe is always eliminated in storage
											// queries.
	bool	OmitTrailingPipeOnRangeQueries;	// Determines whether or not to omit trailing pipe in range commands.


public:		// User declarations

	typedef enum {
		TotalDefault = 0
	} STORAGE_COPY_COMMAND;



// Constructor for READER_CONFIGURATION_STORAGE
	READER_CONFIGURATION_STORAGE( int NumberOfWorkgroups = 1 );

// Destructor for READER_CONFIGURATION_STORAGE
	~READER_CONFIGURATION_STORAGE(void);

// Enable or disable the ability to change settings.
	void EnableSettingChanges( bool Enable );
// Get the value of SettingChangesEnabled.  
	bool GetSettingChanges(void) { return SettingChangesEnabled;}  

// Return the number of defined workgroups.
	int NumberOfAvailableWorkgroups( void );

// Return the current workgroup number to the caller.
	int CurrentWorkgroup( void );
// Return the current worktable number to the caller.
	int CurrentWorkTable(void);
// Return the current worktable number for Scanned menu.
	int CurrentWorkTableForScannedMenu(void);
// switch to the Worktable sent by the caller.
	bool SetWorkTable(int NewWorkTableNumber){CurrentTableNumber=NewWorkTableNumber;return true;}
	bool SetActiveWorkTable(int NewActiveWorkTableNumber){ActiveTableNumber=NewActiveWorkTableNumber;return true;}
	bool SetCurrentTableForScannedCommand(int NewTableNumber){CurrentTableForScannedCommand=NewTableNumber;return true;}
// Resynchronize instantiator copy of active value to the current workgroup in the active value table for the item sent by the caller.
	void MoveInstantiatorCopyOfActiveValueToCurrentWorkgroupInActiveValueTable( SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntry );

// Resynchronize current workgroup in the active value table to the instantiator copy of active value for the item sent by the caller.
	void MoveValueFromCurrentWorkgroupInActiveValueTableToInstantiatorCopyOfActiveValue( SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntry );

// Validate the workgroup number sent by the caller and switch to that workgroup if valid.  In the process of switching we save the instantiator
// copy of all active settings to our local copy for that workgroup, then we move our local copy for the new workgroup to the instantiator copy.
	bool ChangeToWorkgroup( int NewWorkgroupNumber, bool UpdateActiveWorkgroupNumber = true );
	bool ChangeToWorktable(int NewWorkcableNumber);
	bool ScannedMenuChangeToWorktable(int NewWorkcableNumber);

// Return the currently selected workgroup number.
	int GetActiveWorkgroupNumber( void )    { return ActiveWorkgroupNumber; }
	// Return the currently selected worktable number.
	int GetActiveWorktableNumber(void)	{ return ActiveTableNumber;}

// Compute maximum number of digits.
	int ComputeMaximumNumberOfDigits(int MaximumNumber, bool NumberIsHexadecimal = false);

// Copy the input setting value to the output setting value.
	void CopySettingValue( SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY *OutputSetting,
					SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY *InputSetting, unsigned int TypeFlags );

/*
After we replicate, then we'll do the "E" command.  Somehow or another, this needs to get ProcessSubTag to go through each table
only if the command is a query, and don't duplicate the base and common settings, and optionally do the "A" designator.
I think the best way to replicate is that when we encounter a query command, set B and C (and optionally "A"), and keep looping
through all the tables in Menu().  Remember to put the "AT:B+C+A", etc. into the response.
*/
	//bool UpdateNumericListWorkingValue(const char *SettingName1,const char *SettingName2);
	bool UpdateNumericListWorkingDefaltValue(const char *SettingName,int WorkingValue);
// Update the range of a numeric item in the caller specified sub-tag table location.
//		(overloaded version #1, where min and max are addresses).
	bool UpdateNumericRange(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToUpdate, int MinimumRange,
		int *MinimumRangeAddress, int MaximumRange, int *MaximumRangeAddress);

// Update the range of a numeric item in the caller specified sub-tag table location.
//		(overloaded version #2, where min is an address and max is a constant).
	bool UpdateNumericRange(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToUpdate, int MinimumRange,
		int *MinimumRangeAddress, int MaximumRange);

// Update the range of a numeric item in the caller specified sub-tag table location.
//		(overloaded version #3, where min is a constant and max is an address).
	bool UpdateNumericRange(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToUpdate, int MinimumRange,
				int MaximumRange, int *MaximumRangeAddress);

// Update the range of a numeric item in the caller specified sub-tag table location.
//		(overloaded version #4, where min and max are constants).
	bool UpdateNumericRange(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToUpdate, int MinimumRange,
				int MaximumRange);

// Update the list for a numeric list item in the caller specified sub-tag table location  (overloaded version #1,
//		with an array of list items)
	bool UpdateNumericList(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToUpdate, int *ItemList, int NumberOfItems, int MaximumDigits=-1);

// Update the list for a numeric list item in the caller specified sub-tag table location (overloaded version #2,
//		with individual list items)
	bool UpdateNumericList(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToUpdate,
		int Item1 = INVALID_LIST_ITEM, int Item2 = INVALID_LIST_ITEM, int Item3 = INVALID_LIST_ITEM, int Item4 = INVALID_LIST_ITEM,
		int Item5 = INVALID_LIST_ITEM, int Item6 = INVALID_LIST_ITEM, int Item7 = INVALID_LIST_ITEM, int Item8 = INVALID_LIST_ITEM,
		int Item9 = INVALID_LIST_ITEM, int Item10 = INVALID_LIST_ITEM, int Item11 = INVALID_LIST_ITEM, int Item12 = INVALID_LIST_ITEM,
		int Item13 = INVALID_LIST_ITEM, int Item14 = INVALID_LIST_ITEM, int Item15 = INVALID_LIST_ITEM, int Item16 = INVALID_LIST_ITEM,
		int Item17 = INVALID_LIST_ITEM, int Item18 = INVALID_LIST_ITEM, int Item19 = INVALID_LIST_ITEM, int Item20 = INVALID_LIST_ITEM,
		int Item21 = INVALID_LIST_ITEM, int Item22 = INVALID_LIST_ITEM, int Item23 = INVALID_LIST_ITEM, int Item24 = INVALID_LIST_ITEM,
		int Item25 = INVALID_LIST_ITEM, int Item26 = INVALID_LIST_ITEM, int Item27 = INVALID_LIST_ITEM, int Item28 = INVALID_LIST_ITEM,
		int Item29 = INVALID_LIST_ITEM, int Item30 = INVALID_LIST_ITEM, int Item31 = INVALID_LIST_ITEM, int Item32 = INVALID_LIST_ITEM,
		int Item33 = INVALID_LIST_ITEM, int Item34 = INVALID_LIST_ITEM, int Item35 = INVALID_LIST_ITEM, int Item36 = INVALID_LIST_ITEM,
		int Item37 = INVALID_LIST_ITEM, int Item38 = INVALID_LIST_ITEM, int Item39 = INVALID_LIST_ITEM, int Item40 = INVALID_LIST_ITEM,
		int Item41 = INVALID_LIST_ITEM, int Item42 = INVALID_LIST_ITEM, int Item43 = INVALID_LIST_ITEM, int Item44 = INVALID_LIST_ITEM,
		int Item45 = INVALID_LIST_ITEM, int Item46 = INVALID_LIST_ITEM, int Item47 = INVALID_LIST_ITEM, int Item48 = INVALID_LIST_ITEM,
		int Item49 = INVALID_LIST_ITEM, int Item50 = INVALID_LIST_ITEM, int Item51 = INVALID_LIST_ITEM, int Item52 = INVALID_LIST_ITEM,
		int Item53 = INVALID_LIST_ITEM, int Item54 = INVALID_LIST_ITEM, int Item55 = INVALID_LIST_ITEM, int Item56 = INVALID_LIST_ITEM,
		int Item57 = INVALID_LIST_ITEM, int Item58 = INVALID_LIST_ITEM, int Item59 = INVALID_LIST_ITEM, int Item60 = INVALID_LIST_ITEM);


// Update the list for a string list item in the caller specified sub-tag table location  (overloaded version #1,
//		with an array of list items)
	bool UpdateStringList(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToUpdate, const char **ItemList, int NumberOfItems );

// Update the list for a string list item in the caller specified sub-tag table location (overloaded version #2,
//		with individual list items)
	bool UpdateStringList(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToUpdate,
			const char *Item1 = 0, const char *Item2 = 0, const char *Item3 = 0, const char *Item4 = 0,
			const char *Item5 = 0, const char *Item6 = 0, const char *Item7 = 0, const char *Item8 = 0,
			const char *Item9 = 0, const char *Item10 = 0, const char *Item11 = 0, const char *Item12 = 0,
			const char *Item13 = 0, const char *Item14 = 0, const char *Item15 = 0, const char *Item16 = 0,
			const char *Item17 = 0, const char *Item18 = 0, const char *Item19 = 0, const char *Item20 = 0,
			const char *Item21 = 0, const char *Item22 = 0, const char *Item23 = 0, const char *Item24 = 0,
			const char *Item25 = 0, const char *Item26 = 0, const char *Item27 = 0, const char *Item28 = 0,
			const char *Item29 = 0, const char *Item30 = 0, const char *Item31 = 0, const char *Item32 = 0,
			const char *Item33 = 0, const char *Item34 = 0, const char *Item35 = 0, const char *Item36 = 0,
			const char *Item37 = 0, const char *Item38 = 0, const char *Item39 = 0, const char *Item40 = 0,
			const char *Item41 = 0, const char *Item42 = 0, const char *Item43 = 0, const char *Item44 = 0,
			const char *Item45 = 0, const char *Item46 = 0, const char *Item47 = 0, const char *Item48 = 0,
			const char *Item49 = 0, const char *Item50 = 0, const char *Item51 = 0, const char *Item52 = 0,
			const char *Item53 = 0, const char *Item54 = 0, const char *Item55 = 0, const char *Item56 = 0,
			const char *Item57 = 0, const char *Item58 = 0, const char *Item59 = 0, const char *Item60 = 0);

// Add a command item to the tag table.
	bool AddCommand(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void), bool (*ExitAction) (void),
					int SpecialCommandAction = SUB_TAG_TABLE::NO_ACTION);

// Add a boolean item to the tag table.
	bool AddBoolean(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
					bool (*ExitAction) (void), bool *ActiveValueLocation, bool ActiveValue, bool DefaultValue);

// Add a string item to the tag table (overloaded version #1, where active and default value lengths are explicitly specified).
	bool AddString(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), char *ActiveValueLocation, unsigned int MaximumSizeOfActiveValue, const char *ActiveValue,
			int ActiveValueLength, const char *DefaultValue, int DefaultValueLength);

// Add a string item to the tag table (overloaded version #2, where active and default value lengths are implicit via null
// termination).
	bool AddString(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), char *ActiveValueLocation, unsigned int MaximumSizeOfActiveValue, const char *ActiveValue,
			const char *DefaultValue);

// Add a unicode string item to the tag table.
	bool AddUnicodeString(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), HSTRING **ActiveValueLocation, unsigned int MaximumSizeOfActiveValue, const char *ActiveValue,
			const char *DefaultValue);

// Add a binary string item to the tag table.
	bool AddBinaryString(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), HSTRING **ActiveValueLocation, unsigned int MaximumSizeOfActiveValue, const char *ActiveValue,
			const char *DefaultValue);

// Add an ASCII HSTRING item to the tag table.  If maximum size is zero, create the strings to automatically resize
// (version #1, where maximum is determined by the caller).
	bool AddASCIIString(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), HSTRING **ActiveValueLocation, unsigned int MaximumSizeOfActiveValue, const char *ActiveValue,
			const char *DefaultValue);

// Add an ASCII HSTRING item to the tag table (version #2, where maximum string size is unlimited is determined by the caller).
	bool AddASCIIString(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), HSTRING **ActiveValueLocation, const char *ActiveValue, const char *DefaultValue);

// Add a hex string item to the tag table.
	bool AddHexString(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), HSTRING **ActiveValueLocation, unsigned int MaximumSizeOfActiveValue, const char *ActiveValue,
			const char *DefaultValue);

// Add a data format item to the tag table.
	bool AddDataFormat(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), DATA_FORMAT **ActiveValueLocation, unsigned int MaximumSizeOfActiveValue, const char *ActiveValue,
			const char *DefaultValue);

// Add a prefix or suffix item to the tag table
	bool AddPrefixSuffix(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), PREFIX_SUFFIX **ActiveValueLocation, unsigned int MaximumSizeOfActiveValue, const char *ActiveValue,
			const char *DefaultValue);

// Add a numeric item to the tag table and update the index to maintain sorting order (overloaded version #1, where
// min and max are addresses).
	bool AddNumber(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), int *ActiveValueLocation, int ActiveValue, int DefaultValue, int MinimumRange,
			int *MinimumRangeAddress, int MaximumRange, int *MaximumRangeAddress);

// Add a numeric item to the tag table and update the index to maintain sorting order (overloaded version #2, where
// min is an address and max is a constant).
	bool AddNumber(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), int *ActiveValueLocation, int ActiveValue, int DefaultValue, int MinimumRange,
			int *MinimumRangeAddress, int MaximumRange);

// Add a numeric item to the tag table and update the index to maintain sorting order (overloaded version #3, where
// min is a constant and max is an address).
	bool AddNumber(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), int *ActiveValueLocation, int ActiveValue, int DefaultValue, int MinimumRange,
			int MaximumRange, int *MaximumRangeAddress);

// Add a numeric item to the tag table and update the index to maintain sorting order (overloaded version #4, where
// min and max are constants).
	bool AddNumber(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), int *ActiveValueLocation, int ActiveValue, int DefaultValue, int MinimumRange,
			int MaximumRange);

// Add a numeric list item to the tag table and update the index to maintain sorting order.
	bool AddNumericList(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
		bool (*ExitAction) (void), int *ActiveValueLocation, int ActiveValue, int DefaultValue,
		int Item1 = INVALID_LIST_ITEM, int Item2 = INVALID_LIST_ITEM, int Item3 = INVALID_LIST_ITEM, int Item4 = INVALID_LIST_ITEM,
		int Item5 = INVALID_LIST_ITEM, int Item6 = INVALID_LIST_ITEM, int Item7 = INVALID_LIST_ITEM, int Item8 = INVALID_LIST_ITEM,
		int Item9 = INVALID_LIST_ITEM, int Item10 = INVALID_LIST_ITEM, int Item11 = INVALID_LIST_ITEM, int Item12 = INVALID_LIST_ITEM,
		int Item13 = INVALID_LIST_ITEM, int Item14 = INVALID_LIST_ITEM, int Item15 = INVALID_LIST_ITEM, int Item16 = INVALID_LIST_ITEM,
		int Item17 = INVALID_LIST_ITEM, int Item18 = INVALID_LIST_ITEM, int Item19 = INVALID_LIST_ITEM, int Item20 = INVALID_LIST_ITEM,
		int Item21 = INVALID_LIST_ITEM, int Item22 = INVALID_LIST_ITEM, int Item23 = INVALID_LIST_ITEM, int Item24 = INVALID_LIST_ITEM,
		int Item25 = INVALID_LIST_ITEM, int Item26 = INVALID_LIST_ITEM, int Item27 = INVALID_LIST_ITEM, int Item28 = INVALID_LIST_ITEM,
		int Item29 = INVALID_LIST_ITEM, int Item30 = INVALID_LIST_ITEM, int Item31 = INVALID_LIST_ITEM, int Item32 = INVALID_LIST_ITEM,
		int Item33 = INVALID_LIST_ITEM, int Item34 = INVALID_LIST_ITEM, int Item35 = INVALID_LIST_ITEM, int Item36 = INVALID_LIST_ITEM,
		int Item37 = INVALID_LIST_ITEM, int Item38 = INVALID_LIST_ITEM, int Item39 = INVALID_LIST_ITEM, int Item40 = INVALID_LIST_ITEM,
		int Item41 = INVALID_LIST_ITEM, int Item42 = INVALID_LIST_ITEM, int Item43 = INVALID_LIST_ITEM, int Item44 = INVALID_LIST_ITEM,
		int Item45 = INVALID_LIST_ITEM, int Item46 = INVALID_LIST_ITEM, int Item47 = INVALID_LIST_ITEM, int Item48 = INVALID_LIST_ITEM,
		int Item49 = INVALID_LIST_ITEM, int Item50 = INVALID_LIST_ITEM, int Item51 = INVALID_LIST_ITEM, int Item52 = INVALID_LIST_ITEM,
		int Item53 = INVALID_LIST_ITEM, int Item54 = INVALID_LIST_ITEM, int Item55 = INVALID_LIST_ITEM, int Item56 = INVALID_LIST_ITEM,
		int Item57 = INVALID_LIST_ITEM, int Item58 = INVALID_LIST_ITEM, int Item59 = INVALID_LIST_ITEM, int Item60 = INVALID_LIST_ITEM);

// Add a string list item to the tag table and update the index to maintain sorting order.
	bool AddStringList(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), char *ActiveValueLocation, unsigned int MaximumSizeOfActiveValue, const char *ActiveValue,
			const char *DefaultValue, const char *Item1 = 0, const char *Item2 = 0, const char *Item3 = 0, const char *Item4 = 0,
			const char *Item5 = 0, const char *Item6 = 0, const char *Item7 = 0, const char *Item8 = 0,
			const char *Item9 = 0, const char *Item10 = 0, const char *Item11 = 0, const char *Item12 = 0,
			const char *Item13 = 0, const char *Item14 = 0, const char *Item15 = 0, const char *Item16 = 0,
			const char *Item17 = 0, const char *Item18 = 0, const char *Item19 = 0, const char *Item20 = 0,
			const char *Item21 = 0, const char *Item22 = 0, const char *Item23 = 0, const char *Item24 = 0,
			const char *Item25 = 0, const char *Item26 = 0, const char *Item27 = 0, const char *Item28 = 0,
			const char *Item29 = 0, const char *Item30 = 0, const char *Item31 = 0, const char *Item32 = 0,
			const char *Item33 = 0, const char *Item34 = 0, const char *Item35 = 0, const char *Item36 = 0,
			const char *Item37 = 0, const char *Item38 = 0, const char *Item39 = 0, const char *Item40 = 0,
			const char *Item41 = 0, const char *Item42 = 0, const char *Item43 = 0, const char *Item44 = 0,
			const char *Item45 = 0, const char *Item46 = 0, const char *Item47 = 0, const char *Item48 = 0,
			const char *Item49 = 0, const char *Item50 = 0, const char *Item51 = 0, const char *Item52 = 0,
			const char *Item53 = 0, const char *Item54 = 0, const char *Item55 = 0, const char *Item56 = 0,
			const char *Item57 = 0, const char *Item58 = 0, const char *Item59 = 0, const char *Item60 = 0);

// Add a workgroup selection item to the tag table and update the index to maintain sorting order (overloaded version #1, where
// min and max are addresses).
	bool AddWorkgroupSelection(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), int ActiveValue, int DefaultValue );


// Process the next '=' command.
	void ProcessReportingCommand( HSTRING *ReportingCommand );


// Return the setting value in reportable form for the specified sub-tag table entry and affected table.
	void QuerySetting(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntry, const char AffectedTable, HSTRING *SettingValue,
															const char Action, bool ReturnSettableString = false);

// Return the setting range in reportable form for the specified sub-tag table entry and affected table.
	void QuerySettingRange(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntry, const char RangeAction, HSTRING *SettingValue);

// Verify that the command text for this setting is OK.  If we're to only look for part of the data, we'll also tell
// the caller if the maximum command text length has been reached.
	int ValidateAndChangeSetting(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntry, const char *CommandText,
				int CommandTextLength, const char AffectedTable, bool SaveTheResult = true, bool OverrideFixedFlag = false);

// If the setting sent by the caller is a workgroup setting, and the table(s) affected are either working or active,
// copy the current workgroup value to all the other workgroup tables.
	void ReplicateAcrossWorkgroups( SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntry, const char AffectedTable );

// See if there is any special action associated with this command and if there is, do it.
	bool DoSpecialAction(SUB_TAG_TABLE *SubTagTable, SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntry,
				const char *PrefixTag, const char AffectedTable);

// Determine if two settings are equal.
	bool CompareSettingValues( int ValueType,
				SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY *Value1, SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY *Value2 );

// Determine if two settings are equal (overload #2 where second setting is instantiator copy).
	bool CompareSettingValues( int ValueType,
				SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY *Value1,
				SUB_TAG_TABLE::SUB_TAG_TABLE_INSTANTIATOR_COPY_OF_ACTIVE_VALUE *Value2 );

// Determine if all the workgroups in the setting sent by the caller have the same value.
	bool WorkgroupSettingIsCommonValue( SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntry, char AffectedTable );


public:
// Validate the table character, remove it from the command string, and return it to the caller.
	static bool GetAffectedTable( HSTRING *MenuCommandAndResponse, char *AffectedTable );

private:

	bool	IsMenuingInProcess;		//A check to see is we have started processing tags and sub tags.

// Ensure that the menu parameter string sent by the caller only contains printable characters, and does not include a terminator character or
// colon (used for device names).
	bool ValidateMenuParameterCharacters( HSTRING *ParameterString );

// Process all the menu command sub-tags in the list sent by the caller.
	bool ProcessMenuCommandSubTags(SUB_TAG_TABLE *SubTagTable, HSTRING *MenuCommandSubTagAndResponse,
				const char *PrefixTag,char AffectedTable, bool WildcardTag, int *NumberOfSubTagsProcessed,
				int *NumberOfSubTagResponsesNotReported, bool OverrideFixedFlag = false,
				bool ForceDirectDefaultCommandPrivilege = false );

// Process the menu command string sent by the caller.  Originally, this logic was part of the Menu function, but
// we want to be able to process recursively if we're in report all workgroups mode, so this needed to be separated
// so that it could be called recursively.
	void ProcessMenuCommands( HSTRING *MenuCommandAndResponse, char AffectedTable,
								bool OverrideFixedFlag, bool ForceDirectDefaultCommandPrivilege, bool *bHadProblems );


public:
// Process a menu setting command (overloaded version #1 with HSTRING).
	unsigned int Menu(HSTRING *MenuCommandAndResponse, bool OverrideFixedFlag = false, bool ForceDirectDefaultCommandPrivilege = false );

// Process a menu setting command (overloaded version #2 with character string.
	unsigned int Menu(unsigned int *LengthOfMenuCommandAndResponse, unsigned char *MenuCommandAndResponse, bool OverrideFixedFlag = false,
				bool ForceDirectDefaultCommandPrivilege = false );

// Check to see if one or more menu settings have been updated in the customer defaults table.
	bool CustomerDefaultsUpdated(void);

// Check to see if one or more menu settings have been updated in the stored values table.
	bool WorkingValuesUpdated(void);

// Check to see if all the active values have been defaulted.
	bool ActiveValuesWereDefaulted(void);

// Check to see if one or more of the active values have been defaulted.
	bool AtLeastOneActiveValueWasDefaulted(void);

// Check to see if one or more menu settings have been updated in the active values table.
	bool ActiveValuesUpdated(void);

// Check to see if one or more workgroup menu settings have been updated in the working values table.
	bool WorkingWorkgroupValuesUpdated(void);

// Check to see if one or more workgroup menu settings have been updated in the active values table.
	bool ActiveWorkgroupValuesUpdated(void);

// Check to see if one or more workgroup menu settings have been updated in the working values table referenced by the caller.
//
	bool WorkingWorkgroupValuesInTableUpdated( int WorkgroupTableNumber );

// Check to see if one or more workgroup menu settings have been updated in the active values table referenced by the caller.
	bool ActiveWorkgroupValuesInTableUpdated( int WorkgroupTableNumber );

// Check to see if one or more base only menu settings have been updated in the working values table.
	bool WorkingBaseOnlyValuesUpdated(void);

// Check to see if one or more base only menu settings have been updated in the active values table.
	bool ActiveBaseOnlyValuesUpdated(void);

// Check to see if one or more common menu settings have been updated in the working values table.
	bool WorkingCommonValuesUpdated(void);

// Check to see if one or more common menu settings have been updated in the active values table.
	bool ActiveCommonValuesUpdated(void);

// Check to see if one or more scanner only menu settings have been updated in the working values table.
	bool WorkingScannerOnlyValuesUpdated(void);

// Check to see if one or more scanner only menu settings have been updated in the active values table.
	bool ActiveScannerOnlyValuesUpdated(void);

// Mark all menu settings in the customer defaults table as being up to date.
	void CustomerDefaultsUpToDate(void);

// Mark all menu settings in the stored values table as being up to date.
	void WorkingValuesUpToDate(void);

// Mark all menu settings in the active values table as being up to date.
	void ActiveValuesUpToDate(void);

// Mark all menu settings in the customer defaults table as unmodified since creation.
	void ClearCustomerDefaultsModifiedSinceCreationFlag(void);

// Mark all menu settings in the stored values table as unmodified since creation.
	void ClearWorkingValuesModifiedSinceCreationFlag(void);

// Mark all menu settings in the active values table as unmodified since creation.
	void ClearActiveValuesModifiedSinceCreationFlag(void);

// Copy standard defaults to defaults.
	void CopyStandardDefaultsToDefaults(void);

// Copy defaults to working and active settings.
	void CopyDefaultsToWorkingAndActive(const char AffectedTable = '@', bool DefaultAllWorkgroups = false );

// Copy working settings to defaults.
	void CopyWorkingToDefaults(void);

// Copy working setings over active settings.
	void CopyWorkingSettingsOverActiveSettings(void);

// Restore all settings to their default values.
	void RestoreAllSettingsToDefault( bool DefaultAllWorkgroups = false );

// Restore all settings to their default values.
	bool RestoreAllSettingsWithThisTagToDefault(const char *TagName, const char AffectedTable = '@');

// Restore an individual setting to its' default values.
	bool RestoreThisSettingToDefault(const char *SettingName);

// Find all symbology enable settings and turn on values in the affected table.
	bool EnableAllSymbologies(char AffectedTable);

// Find all symbology enable settings and turn off values in the affected table.
	bool DisableAllSymbologies(char AffectedTable);

// Change the range of a numeric item (overloaded version #1, where min and max are addresses).
	bool ChangeNumericRange(const char *SettingName, int MinimumRange, int *MinimumRangeAddress, int MaximumRange,
		int *MaximumRangeAddress);

// Change the range of a numeric item (overloaded version #2, where min is an address and max is a constant).
	bool ChangeNumericRange(const char *SettingName, int MinimumRange, int *MinimumRangeAddress, int MaximumRange);

// Change the range of a numeric item (overloaded version #3, where min is a constant and max is an address).
	bool ChangeNumericRange(const char *SettingName, int MinimumRange, int MaximumRange, int *MaximumRangeAddress);

// Change the range of a numeric item (overloaded version #4, where min and max are constants).
	bool ChangeNumericRange(const char *SettingName, int MinimumRange, int MaximumRange);

// Change the list for a numeric list item (overloaded version #1, with individual list items)
	bool ChangeNumericList(const char *SettingName,
		int Item1 = INVALID_LIST_ITEM, int Item2 = INVALID_LIST_ITEM, int Item3 = INVALID_LIST_ITEM, int Item4 = INVALID_LIST_ITEM,
		int Item5 = INVALID_LIST_ITEM, int Item6 = INVALID_LIST_ITEM, int Item7 = INVALID_LIST_ITEM, int Item8 = INVALID_LIST_ITEM,
		int Item9 = INVALID_LIST_ITEM, int Item10 = INVALID_LIST_ITEM, int Item11 = INVALID_LIST_ITEM, int Item12 = INVALID_LIST_ITEM,
		int Item13 = INVALID_LIST_ITEM, int Item14 = INVALID_LIST_ITEM, int Item15 = INVALID_LIST_ITEM, int Item16 = INVALID_LIST_ITEM,
		int Item17 = INVALID_LIST_ITEM, int Item18 = INVALID_LIST_ITEM, int Item19 = INVALID_LIST_ITEM, int Item20 = INVALID_LIST_ITEM,
		int Item21 = INVALID_LIST_ITEM, int Item22 = INVALID_LIST_ITEM, int Item23 = INVALID_LIST_ITEM, int Item24 = INVALID_LIST_ITEM,
		int Item25 = INVALID_LIST_ITEM, int Item26 = INVALID_LIST_ITEM, int Item27 = INVALID_LIST_ITEM, int Item28 = INVALID_LIST_ITEM,
		int Item29 = INVALID_LIST_ITEM, int Item30 = INVALID_LIST_ITEM, int Item31 = INVALID_LIST_ITEM, int Item32 = INVALID_LIST_ITEM,
		int Item33 = INVALID_LIST_ITEM, int Item34 = INVALID_LIST_ITEM, int Item35 = INVALID_LIST_ITEM, int Item36 = INVALID_LIST_ITEM,
		int Item37 = INVALID_LIST_ITEM, int Item38 = INVALID_LIST_ITEM, int Item39 = INVALID_LIST_ITEM, int Item40 = INVALID_LIST_ITEM,
		int Item41 = INVALID_LIST_ITEM, int Item42 = INVALID_LIST_ITEM, int Item43 = INVALID_LIST_ITEM, int Item44 = INVALID_LIST_ITEM,
		int Item45 = INVALID_LIST_ITEM, int Item46 = INVALID_LIST_ITEM, int Item47 = INVALID_LIST_ITEM, int Item48 = INVALID_LIST_ITEM,
		int Item49 = INVALID_LIST_ITEM, int Item50 = INVALID_LIST_ITEM, int Item51 = INVALID_LIST_ITEM, int Item52 = INVALID_LIST_ITEM,
		int Item53 = INVALID_LIST_ITEM, int Item54 = INVALID_LIST_ITEM, int Item55 = INVALID_LIST_ITEM, int Item56 = INVALID_LIST_ITEM,
		int Item57 = INVALID_LIST_ITEM, int Item58 = INVALID_LIST_ITEM, int Item59 = INVALID_LIST_ITEM, int Item60 = INVALID_LIST_ITEM);

// Change the list for a numeric list item (overloaded version #2, with an array of list items)
	bool ChangeNumericList(const char *SettingName, int *ItemList, int NumberOfItems, int MaximumDigits=-1);

// Change the list for a string list item (overloaded version #1, with individual list items)
	bool ChangeStringList(const char *SettingName,
			const char *Item1 = 0, const char *Item2 = 0, const char *Item3 = 0, const char *Item4 = 0,
			const char *Item5 = 0, const char *Item6 = 0, const char *Item7 = 0, const char *Item8 = 0,
			const char *Item9 = 0, const char *Item10 = 0, const char *Item11 = 0, const char *Item12 = 0,
			const char *Item13 = 0, const char *Item14 = 0, const char *Item15 = 0, const char *Item16 = 0,
			const char *Item17 = 0, const char *Item18 = 0, const char *Item19 = 0, const char *Item20 = 0,
			const char *Item21 = 0, const char *Item22 = 0, const char *Item23 = 0, const char *Item24 = 0,
			const char *Item25 = 0, const char *Item26 = 0, const char *Item27 = 0, const char *Item28 = 0,
			const char *Item29 = 0, const char *Item30 = 0, const char *Item31 = 0, const char *Item32 = 0,
			const char *Item33 = 0, const char *Item34 = 0, const char *Item35 = 0, const char *Item36 = 0,
			const char *Item37 = 0, const char *Item38 = 0, const char *Item39 = 0, const char *Item40 = 0,
			const char *Item41 = 0, const char *Item42 = 0, const char *Item43 = 0, const char *Item44 = 0,
			const char *Item45 = 0, const char *Item46 = 0, const char *Item47 = 0, const char *Item48 = 0,
			const char *Item49 = 0, const char *Item50 = 0, const char *Item51 = 0, const char *Item52 = 0,
			const char *Item53 = 0, const char *Item54 = 0, const char *Item55 = 0, const char *Item56 = 0,
			const char *Item57 = 0, const char *Item58 = 0, const char *Item59 = 0, const char *Item60 = 0);

// Change the list for a string list item (overloaded version #2, with an array of list items)
	bool ChangeStringList(const char *SettingName, const char **ItemList, int NumberOfItems );

// Change the type flags for the setting(s) specified by SettingName.
	bool ChangeTypeFlags(const char *SettingName, unsigned int TypeFlagsAffected, bool SetTypeFlags = true);


// Determine if there are any invalid characters in a menu command.
	bool ValidateMenuCommandCharacters(const char *Command, int CommandLength, const char Table = 0);
	bool IsTerminatorValid(char terminator);

// Determine if a partially received command is valid so far.  The complete tag and sub-tag must be included in Command.
// This will allow barcode commands to be validated on the fly.  If the command is OK, we'll also tell the caller if
// any more data would make it too big.
	bool ValidatePartialCommand(const char *Command, bool *MaximumCommandLengthLimitReached);


// Return a pointer to the sub-tag table entry corresponding to SettingName.
	bool GetSubTagTableEntry(const char *SettingName, SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY **SettingEntry);

// Return variable "IsMenuingInProcess", to check if we are processing menu tags and sub tags, set external by the menu item processed.
	bool GetIsMenuingInProcess(void);

// Set then return variable "IsMenuingInProcess", to indicate that we are processing menu tags and sub tags, set external by the menu item processed.
	bool SetIsMenuingInProcess(void);

private:
// Add a value, sent by the caller, to an existing CRC sent by the caller.
	int AddValueToCRC( int CRC, int Value );

public:
	int	LastCRC;		// CRC after last call to Menu routine.

// Do a CRC of all the settings.
	int CalculateCRC( void );

public:
// Static member function to see if there is a device name delimited by colons at the beginning of the string.
// Overload #1 takes HSTRING input.
	static bool FindAndRemoveDeviceName( HSTRING *CommandString, HSTRING *DeviceName );

// Static member function to see if there is a device name delimited by colons at the beginning of the string.
// Overload #2 takes char * input.
	static bool FindAndRemoveDeviceName( char *chCommandString, int *CommandStringLength, HSTRING *DeviceName );

// Remove commands from the input command string whose type flag matches one or more of the bits set in RemovalMask.
// This routine allows removal of certain commands, such as scanner only or base only, before sending the result on
// to another device.
	bool RemoveCommands( HSTRING *Command, int RemovalMask );

// Sets the FIXED attribute for the caller specified setting.
	bool SetFixedAttribute( const char *SettingName );

// Clears the FIXED attribute for the caller specified setting.
	bool ClearFixedAttribute( const char *SettingName );

// Sets the FIXED_EXCEPT_ACTIVE attribute for the caller specified setting.
	bool SetFixedExceptActiveAttribute( const char *SettingName );

// Clears the FIXED_EXCEPT_ACTIVE attribute for the caller specified setting.
	bool ClearFixedExceptActiveAttribute( const char *SettingName );

#ifdef READER_CONFIGURATION_STORAGE_DEBUG
// Build a list of settings that share a common active value.
	void BuildListOfSettingsWithCommonActiveValue( HSTRING *ListOfSettings );
#endif


// Set the default for a boolean item (version #1)
	bool SetDefault( SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SettingToUpdate, bool DefaultValue, bool OverrideRangeCheck = false );

// Set the default for a numeric item (version #2)
	bool SetDefault( SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SettingToUpdate, int DefaultValue, bool OverrideRangeCheck = false );

// Update the default for a boolean item (version #1)
	bool ChangeDefault( const char *SettingName, bool DefaultValue, bool OverrideRangeCheck = false,
												 bool OverwriteCustomWorkingAndActiveIfModified = false );

// Update the default for a numeric item (version #2)
	bool ChangeDefault( const char *SettingName, int DefaultValue, bool OverrideRangeCheck = false,
												 bool OverwriteCustomWorkingAndActiveIfModified = false );


protected:
	virtual void ProcessError( int /* RCSError */, const char * /* SettingName */ )    { return; }

private:
	READER_CONFIGURATION_STORAGE (const READER_CONFIGURATION_STORAGE&);            // declared private, but not defined, to prevent copying         
	READER_CONFIGURATION_STORAGE& operator=(const READER_CONFIGURATION_STORAGE&);  // declared private, but not defined, to prevent copying 

};  // Class READER_CONFIGURATION_STORAGE

#endif // ReaderConfigurationStorageH
//----------------------------------------------------------------------------




