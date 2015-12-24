//======================================================================================
// ReaderConfigurationStorage.cpp
//======================================================================================
// $RCSfile: readerconfigurationstorage.cpp $
// $Revision: 1.20 $
// $Date: 2011/03/30 15:01:01EDT $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Class Name:     READER_CONFIGURATION_STORAGE
//
//  Class Description:
//      READER_CONFIGURATION_STORAGE contains the data fields and methods needed to manage
//      HHP reader configuration data.
//
//		In order to support RF, the general approach used in the past has been employed, where workgroups are used to change configuration context.
//		Three types of settings are supported:
//			Base only settings - these are only defined on the base, and need to be marked as base only so that they are not included amongst the
//					settings communicated to readers.
//			Common settings - these are defined in the base and the readers and are common to all workgroups.  These are stored in the base and
//					pushed to each readers' active table as each reader associates with the base.  The readers maintain a working storage copy of
//					these settings which may be updated via direct link.  These will be used by the reader as active settings at boot time, but
//					they will always be overridden by the base when association is made.  If one of these settings is scanned as a menu code by a
//					particular reader, that reader will not store the setting to its' active or working table, but will forward it on to the base,
//					which will be responsible for sending it to all readers of the same workgroup, including the one that read the symbol.  These
//					settings will be visible by tag only in the reader.
//			Workgroup settings - these are defined in the base and are specific to each workgroup.  That is, each workgroup settings has a different
//					possible setting for each workgroup.  A workgroup table consists of all workgroup setting values for a specific workgroup number.
//					There is, however, only one default value for each setting.  These are stored in the base.  According to the selected workgroup
//					in a particular reader, the settings from that workgroup table are pushed to the readers' active table as each reader associates
//					with the base.  The readers maintain a working storage copy of these settings which may be updated via direct link, although
//					they are not indexed by workgroup.  These will be used by the reader as active settings at boot time, but they will always be
//					overridden by the base when association is made.  If one of these settings is scanned as a menu code by a particular reader,
//					that reader will not store the setting to its' active or working table, but will forward it on to the base, which will be
//					responsible for sending it to all readers of the same workgroup, including the one that read the symbol.  These settings will
//					be visible by tag only in the reader.
//			Reader only settings - these are only defined in the reader and can only be updated via barcode menu commands or direct link.  These
//					settings are fully visible, and are the only ones that will be reported in configuration tools, such as Visual Menu.  No design
//					changes are necessary to support this.
//
//
//		There are several unique implementation requirements for these types of settings:
//			Flash Storage / Retrieval - Base and common settings should only be stored once in flash.  In order to avoid storing every workgroup
//					table value for each workgroup setting, those settings with common values (all workgroup table values are the same) should be
//					stored once, since there are usually few settings with differences across workgroup tables.  The balance of the workgroup
//					settings will be stored with each workgroup table value.

//					Flash storage / retrieval in Matrix devices is done by querying READER_CONFIGURATION_STORAGE with a "$." command, and storing the
//					resulting response to flash.  At startup, this string is retrieved from flash and sent as a command to READER_CONFIGURATION_STORAGE.
//					The same approach will be employed for RF, requiring some new commands to accomplish this.  In addition, global query commands
//					(querying all settings) result in the tag/sub-tag being provided for every setting.  In order to save more storage, a new command
//					modifier is required to allow global queries to return all settings for a particular tag in compressed format, where the first
//					setting returned includes the tag and the remainder only include the sub-tag, each setting being separated by commas.
//			The command approach needs to be backward compatible from a host perspective.  A global query to the base should return all base only
//					settings, all common settings, and all settings in the workgroup table selected by the CURTBL menu setting.  When modifying
//					workgroup settings, CURTBL will also control which workgroup table gets updated for a particular setting.  When defaulting
//					workgroup settings, the workgroup table selected by CURTBL will be updated.
//			When association is made, the base must send, as a command string, all common settings to a reader along with all settings in the
//					workgroup table corresponding to the reader selected workgroup number.
//			In the base software, each workgroup setting needs to have an array of active and working values, as opposed to a single value.
//					Because of the fact that the active values table is made up of actual variable instances supplied by the instantiator (so that
//					the instantiator can see the active value directly), this class will continue to work with the active setting variables supplied
//					by the caller, but will maintain a local copy for each workgroup.  What will happen is that whenever the current workgroup number
//					is changed, the instantiator supplied active setting variables will be stored to the local copy of active settings for the current
//					workgroup, then the instantiator supplied active setting variables will be loaded with the local copy of active settings for the
//					new workgroup.
//
//
//		To support the needs of flash storage / retrieval and menu uploading to readers, a new command, '=',  is being added to
//		READER_CONFIGURATION_STORAGE.  The '=' command may be used anywhere in the command string, and must be separated by semi-colons from other
//		menu commands.  The format of the '=' command consists of an '=' in the first position followed by any combination of the following
//		parameters, separated by commas:
//			"CMP" or "UNCMP" - indicates that wildcard queries are to be returned in tag compressed format ("CMP") or tag uncompressed format
//					("UNCMP").  Default is "UNCMP".
//			"SORTALPHA" or "SORTCREATE" - controls whether the output from wildcard queries is sorted alphabetically or in the
//					order in which setting tags and subtags within tags were created.  Default is "SORTCREATE".
//			"HIDDEN[:hf]" - affects the manner in which wildcard queries return hidden settings.  If no optional flags (hf) are
//					specified, all settings are reported, regardless of visibility.  If specified the flags must be preceded with
//					a colon and may be a combination of one or more of the following separated by '+' characters:
//						"N" - report settings that are not visible at all.
//						"S" - report settings that are visible by sub-tag.
//						"T" - report settings that are visible by tag.
//						"F" - report settings that are fully visible.
//					Each of the flag characters may be followed by an optional '>' character, which specifies that all settings
//					of higher visibility will be reported as well.  For example, "S>" is the same as specifying "S+T+F" and "N>"
//					is the same as specifying "N+S+T+F".  Default is "NOHIDDEN" (see below).
//			"NOHIDDEN" - indicates that wildcard queries are to skip hidden settings ("NOHIDDEN").  Default is "NOHIDDEN".
//			"AT:ats" - changes the affected table for reporting / setting, according to the affected table specifier (ats), which consists of one or
//					more fields separated by '+' characters:
//				"B" - report base only settings.  This does not affect base only settings that are explicitly referenced by tag / sub-tag.
//				"C" - report common settings.  This does not affect common settings that are explicitly referenced by tag / sub-tag.
//				"S" - report scanner only settings.  This does not affect scanner only settings that are explicitly referenced by tag / sub-tag.
//				one of the following workgroup table selections:
//					"A" - Report workgroup settings with common values across all workgroup tables.  Setting changes affect all workgroup tables.
//							This is most useful for fulfilling the flash storage / retrieval requirements.
//					["U"]"D" - Report workgroup settings in the workgroup table referenced by CURTBL.  Setting changes affect the workgroup table
//							referenced by CURTBL.  The optional "U" designator indicates that only those workgroup settings that should be reported
//							are those with at least one different value in one of the workgroup tables.  The "U" has no effect when changing
//							settings and is most useful for fulfilling the flash storage / retrieval requirements.
//					["U"]["P"]# -  Report workgroup settings in the workgroup table number specified by #.  Setting changes affect the workgroup
//							table referenced by #.  The optional "U" designator indicates that only those workgroup settings that should be
//							reported are those with at least one different value in one of the workgroup tables.  The "U" has no effect when
//							changing settings and is most useful for fulfilling the flash storage / retrieval requirements.  The optional "P"
//							parameter causes the current table selection, referenced by CURTBL, to be permanently updated.
//					["A"]"E" - Report every workgroup value for every workgroup setting.  This results in a command response that sets the workgroup
//							table temporarily to 0, then reports all workgroup table 0 settings, then sets the workgroup table temporarily to 1, etc.,
//							until all workgroups have been reported.  The # command is used in the command response to indicate the workgroup table
//							being reported.  The optional "A" designator indicates that workgroup settings with common values across all workgroup
//							tables will be reported first, followed by uncommon values in table 0, uncommon values in table 1, etc.  The initial use
//							of this command will be to issue "=AT:AE;$.", which will return a response string that can be stored to flash and later
//							used as a command string to restore all workgroup settings at startup.  Setting changes affect all workgroup tables.
//			"UP:uss" - changes how updated settings are reported, according to the updated setting specifier (uss), which consists of one or
//					more fields separated by '+' characters:
//				"A" - report all settings, without regard to update status.  This is the default.
//				["N"]"SLC" - Report settings that have been updated since the last menu command was sent.  The optional "N" designator specifies to
//					report those settings which have NOT been updated since the last menu command was sent.    If the optional SLC modifier (slcm)
//					is used, the condition specified must also be true.  The SLC modifier can be one of the following:
//					"WU"  - When processing active or workgroup settings, only report settings if the working table has been
//							modified.
//					"AU"  - When processing active or workgroup settings, only report settings if the active table has been
//							modified.
//					"NWU" - When processing active or workgroup settings, only report settings if the working table has not been
//							modified.
//					"NAU" - When processing active or workgroup settings, only report settings if the active table has not been
//							modified.
//					This command is only for internal application usage, when we're trying to determine what settings have changed before we
//					reinitialize interfaces and store them.  The result of the "SLC" command outside the application code should always be NULL.
//				["N"]"SS" - Report settings that have been modified since startup.  The optional "N" designator specifies to report those settings
//					which have NOT been updated since startup.
//				["N"]"AEW" - When processing active or workgroup settings, report a setting if it's active and working values are equal.  The optional
//					"N" designator specifies to report the setting if it's active and working values are NOT equal.  This only takes effect when
//					the affected table is '.' or '!'.
//
//		The effect of the '=' command on selected workgroup table expires at the end of the command string processing or when
//		another '=' command is encountered that alters updated setting reporting, whichever comes first.  Note also that the
//		"SLC" and "NSLC" parameters to the "UP" command are mutually exclusive.  As soon as you specify one, you exclude the
//		other, and the last one specified in a "UP" command takes precedence.  For example, "=UP:SLC+NSLC;?." will cause all
//		settings not updated since the last command to be reported, and the effect of the "SLC" parameter will be voided by
//		the subsequent "NSLC" parameter.
//
//		In the future, the '=' command may be allowed to follow a setting command, and it will only apply to that command.  For example,
//		"CBRENA1=AT:3;C39ENA1." causes CBRENA in workgroup table 3 to be updated, and C39ENA will be updated in the default workgroup.
//
//
//		To support these different types of settings, READER_CONFIGURATION_STORAGE will also need to do the following:
//			Add routines will support additional flags that designate a setting as base only or as a workgroup setting.  If no flag is specified,
//					the setting will be common.
//			Maintain a multidimensional array of values for each workgroup setting.
//			Change default and command logic to operate on the selected workgroup table.
//
//---------------------------------------------------------------------------


#include	<stdio.h>							// standard I/O functions
#include	<stdlib.h>
#include	<string.h>							// string functions

#include	"HHPReaderConfiguration_Config.h"	//
#include	"ReaderConfigurationStorage.h"		// READER_CONFIGURATION_STORAGE interface definition
#include	"crc.h"								// Interface to CRC generator


//---------------------------------------------------------------------------




#ifndef	TRACE0
#define	TRACE0( a )
#endif
#ifndef	TRACE1
#define	TRACE1( a, b )
#endif
#ifndef	TRACE2
#define	TRACE2( a, b, c )
#endif
/*																	  //to debug - remove comment out
#include	"db_Hdw.h"
#include	"IoMan.h"
#include "db_chrio.h"        // Character I/O header prototypes
#undef	TRACE0
#define	TRACE0( a ) theStdInterface.printf( a );  wait_til_tx_is_empty()
#undef	TRACE1
#define	TRACE1( a, b ) theStdInterface.printf( a, b );  wait_til_tx_is_empty()
#undef	TRACE2
#define	TRACE2( a, b, c ) theStdInterface.printf( a, b, c );  wait_til_tx_is_empty()
*/																		//to debug - remove comment out



#define NOT_FOUD ((unsigned int)-1)
//
//	ReaderConfigurationQSortCompareOfNumericListItems
//		Use by qsort routine to compare numeric list items as they are sorted into ascending order.
//
/*
int ReaderConfigurationQSortCompareOfNumericListItems( const void *arg1, const void *arg2 )
{
	return *( (int *) arg1 ) - *( (int *) arg2 );
} // ReaderConfigurationQSortCompareOfNumericListItems
*/



//
//	Constructor for DATA_FORMAT
//		Create a minimum number of empty entries, then add the entries sent by the caller.
//
DATA_FORMAT::DATA_FORMAT(int InputMaximumBinaryCommandStringLength, const char *InputCommandString)
{
// Allocate 10 entries to start with.
	NumberOfEntries	= 0;
	MaximumNumberOfEntries = NUMBER_OF_DATA_FORMAT_ENTRIES_TO_ADD_ON_REALLOCATE;
	Entries = new DATA_FORMAT_ENTRY[MaximumNumberOfEntries];

// Save binary command string parameters.
	MaximumBinaryCommandStringLength = InputMaximumBinaryCommandStringLength;
	BinaryCommandString = new char [MaximumBinaryCommandStringLength + 1];

// Load entries sent by caller.
	ProcessCommand(InputCommandString);

	return;
} // Constructor





//
//	Destructor for DATA_FORMAT
//		Delete each entry, then delete the entry table.
//
DATA_FORMAT::~DATA_FORMAT(void)
{
	bool	EntryWasDeleted;			// Return status for data format and prefix / suffix erasures (we don't use it).


// Delete all entries.
	EraseAll(&EntryWasDeleted);

// Delete the table.
	delete [] Entries;
	Entries=NULL;
	delete BinaryCommandString;

	return;
} // Destructor





//
//	AddMoreEntries
//		If we're out of entries, add more.
//
void DATA_FORMAT::AddMoreEntries(void)
{
	int		i;


// Are we out of entries in the tag table?
	if(NumberOfEntries >= MaximumNumberOfEntries)
	{
	// Allocate more space in the table, copy the existing entries, and delete the old table.
		MaximumNumberOfEntries += NUMBER_OF_DATA_FORMAT_ENTRIES_TO_ADD_ON_REALLOCATE;
		DATA_FORMAT_ENTRY	*NewEntries = new DATA_FORMAT_ENTRY[MaximumNumberOfEntries];
		for(i = 0;    i < NumberOfEntries;    ++i)
		{
			NewEntries[i] = Entries[i];
		}

		delete [] Entries;
		Entries = NewEntries;
	}

	return;
}






//
//	ValidateKey
//		Validate the key.
//
bool DATA_FORMAT::ValidateKey(int FormatNumber, int TerminalType, int CodeIdentifier, int Length)
{
	bool	Status = true;			// Assume success.


	if((FormatNumber < 0) || (FormatNumber > MAXIMUM_DATA_FORMAT_NUMBER))
	{
		Status = false;
	}

	if((TerminalType < 0) || (TerminalType > MAXIMUM_DATA_FORMAT_TERMINAL_TYPE))
	{
		Status = false;
	}

	if((CodeIdentifier < 0) || (CodeIdentifier > MAXIMUM_DATA_FORMAT_CODE_IDENTIFIER))
	{
		Status = false;
	}

	if((Length < 0) || (Length > MAXIMUM_DATA_FORMAT_LENGTH))
	{
		Status = false;
	}


	return(Status);
} // ValidateKey





//
//	ParseKey
//		Parse and validate the key.  Note that we may only have a partial key.
//
int DATA_FORMAT::ParseKey(const char *Key, int *FormatNumber, int *TerminalType, int *CodeIdentifier, int *Length)
{
	int		Status = RDR_CFG_SETTING_VALID;			// Assume success.
	int		NumberOfCharactersUsedInConversion;
	HSTRING	ParsingKey(Key, true);
	int		ExpectedSizeOfNextItem;


// If the key length is not at least DATA_FORMAT_FORMATTED_KEY_LENGTH, WOOF!
	if(ParsingKey.Size < DATA_FORMAT_FORMATTED_KEY_LENGTH)
	{
		Status = RDR_CFG_PARTIAL_SETTING_VALID;
	}

	*FormatNumber = 0;
	*TerminalType = 0;
	*CodeIdentifier = 0;
	*Length = 0;

// Convert the format number.
	if((Status != RDR_CFG_SETTING_INVALID) && (ParsingKey.Size))
	{
		ExpectedSizeOfNextItem = (ParsingKey.Size < 1) ? ParsingKey.Size : 1;
		*FormatNumber = ((int) ParsingKey.ConvertToNumber(0, ExpectedSizeOfNextItem - 1, &NumberOfCharactersUsedInConversion));
		if(NumberOfCharactersUsedInConversion == ExpectedSizeOfNextItem)
		{
			ParsingKey.Remove(0, ExpectedSizeOfNextItem);
		}
		else
		{
			Status = RDR_CFG_SETTING_INVALID;
		}
	}

// Convert the terminal type
	if((Status != RDR_CFG_SETTING_INVALID) && (ParsingKey.Size))
	{
		ExpectedSizeOfNextItem = (ParsingKey.Size < 3) ? ParsingKey.Size : 3;
		*TerminalType = ((int) ParsingKey.ConvertToNumber(0, ExpectedSizeOfNextItem - 1, &NumberOfCharactersUsedInConversion));
		if(NumberOfCharactersUsedInConversion == ExpectedSizeOfNextItem)
		{
			ParsingKey.Remove(0, ExpectedSizeOfNextItem);
		}
		else
		{
			Status = RDR_CFG_SETTING_INVALID;
		}
	}

// Convert the code identifier
	if((Status != RDR_CFG_SETTING_INVALID) && (ParsingKey.Size))
	{
		ExpectedSizeOfNextItem = (ParsingKey.Size < 2) ? ParsingKey.Size : 2;
		*CodeIdentifier = ((int) ParsingKey.ConvertHexToNumber(0, ExpectedSizeOfNextItem - 1, &NumberOfCharactersUsedInConversion));
		if(NumberOfCharactersUsedInConversion == ExpectedSizeOfNextItem)
		{
			ParsingKey.Remove(0, ExpectedSizeOfNextItem);
		}
		else
		{
			Status = RDR_CFG_SETTING_INVALID;
		}
	}

// Convert the length
	if((Status != RDR_CFG_SETTING_INVALID) && (ParsingKey.Size))
	{
		ExpectedSizeOfNextItem = (ParsingKey.Size < 4) ? ParsingKey.Size : 4;
		*Length = ((int) ParsingKey.ConvertToNumber(0, ExpectedSizeOfNextItem - 1, &NumberOfCharactersUsedInConversion));
		if(NumberOfCharactersUsedInConversion == ExpectedSizeOfNextItem)
		{
			ParsingKey.Remove(0, ExpectedSizeOfNextItem);
		}
		else
		{
			Status = RDR_CFG_SETTING_INVALID;
		}
	}

// Validate the result
	if((Status != RDR_CFG_SETTING_INVALID) && !ValidateKey(*FormatNumber, *TerminalType, *CodeIdentifier, *Length))
	{
		Status = RDR_CFG_SETTING_INVALID;
	}

	return(Status);
} // ParseKey






//
//	SizeOfBinaryCommandString
//		Determine the size of a binary command string based on the data formats.
//
int DATA_FORMAT::SizeOfBinaryCommandString(int SizeOfAdditionalEntry)
{
	int		Size;
	int		i;					// Next entry to process.


	i = 0;
	Size = 0;

// Process each entry until all are completed or we encounter an error.
	while(i < NumberOfEntries)
	{
		Size += Entries[i].Command->Size + 7;
		++i;
	}

	Size += 2;

// If requested to do so (non-negative input size), determine how much the caller specified format size will add.
	if(SizeOfAdditionalEntry >= 0)
	{
		Size += SizeOfAdditionalEntry + 7;
	}


	return(Size);
} // SizeOfBinaryCommandString






//
//	BuildBinaryCommandString
//		Build a binary command string based on the data formats.  Every time a change is made, this must be called to update
//		the binary command string.
//
bool DATA_FORMAT::BuildBinaryCommandString(void)
{
	bool	Status = true;		// Assume success until we know otherwise.
	int		i;					// Next entry to process.
	int		p;					// Next position in binary command string.
	int		BinaryEntryLength;	// Length of next entry in binary form.



// If new string will be too big, WOOF!
	if(SizeOfBinaryCommandString() > MaximumBinaryCommandStringLength)
	{
		return(false);
	}

	i = 0;
	p = 0;


// Process each entry until all are completed or we encounter an error.
	while((Status) && (i < NumberOfEntries))
	{
		BinaryEntryLength = Entries[i].Command->Size + 5;
		if((p + BinaryEntryLength  + 2) <= MaximumBinaryCommandStringLength)
		{
			BinaryCommandString[p] = ((unsigned char) ((BinaryEntryLength / 256) & 0xFF));
			++p;
			BinaryCommandString[p] = ((unsigned char) (BinaryEntryLength & 0xFF));
			++p;
			BinaryCommandString[p] = ((unsigned char) (Entries[i].FormatNumber & 0xFF));
			++p;
			BinaryCommandString[p] = ((unsigned char) (Entries[i].TerminalType & 0xFF));
			++p;
			BinaryCommandString[p] = ((unsigned char) (Entries[i].CodeIdentifier & 0xFF));
			++p;
			BinaryCommandString[p] = ((unsigned char) ((Entries[i].Length / 256) & 0xFF));
			++p;
			BinaryCommandString[p] = ((unsigned char) (Entries[i].Length & 0xFF));
			++p;
			Entries[i].Command->CopyFromString(((unsigned char *) &BinaryCommandString[p]), Entries[i].Command->Size);
			p += Entries[i].Command->Size;
		}
		else
		{
			Status = false;
		}

		++i;
	}


// At the end, tack on two NULLs.
	if(Status)
	{
		if((p + 2) <= MaximumBinaryCommandStringLength)
		{
			BinaryCommandString[p] = 0;
			++p;
			BinaryCommandString[p] = 0;
		}
		else
		{
			Status = false;
		}
	}

	return(Status);
} // BuildBinaryCommandString







//
//	Find (overloaded version #1 with a binary key value)
//		Find an item in the table that matches the input key.
//
int DATA_FORMAT::Find(int FormatNumber, int TerminalType, int CodeIdentifier, int Length, int StartIndex)
{
	int		Entry = -1;				// No entry found yet.
	int		i;


	i = StartIndex;
	while((i < NumberOfEntries) && (Entry < 0))
	{
		if((Entries[i].FormatNumber == FormatNumber) && (Entries[i].TerminalType == TerminalType) &&
					(Entries[i].CodeIdentifier == CodeIdentifier) && (Entries[i].Length == Length))
		{
			Entry = i;
		}
		else
		{
			++i;
		}
	}

	return(Entry);
} // Find (overloaded version #1)






//
//	Find (overloaded version #2 with a formatted key value)
//		Find an item in the table that matches the input key.
//
int DATA_FORMAT::Find(const char *Key, int StartIndex)
{
	int		FormatNumber;
	int		TerminalType;
	int		CodeIdentifier;
	int		Length;
	int		Entry = -1;				// No entry found yet.


	if(ParseKey(Key, &FormatNumber, &TerminalType, &CodeIdentifier, &Length) > 0)
	{
		Entry = Find(FormatNumber, TerminalType, CodeIdentifier, Length, StartIndex);
	}

	return(Entry);
} // Find (overloaded version #2)







//
//	FindCommandString (overloaded version #1 with a binary key value)
//		Find an item in the table that matches the input key and return the location and size of the command string.
//
HSTRING * DATA_FORMAT::FindCommandString(int FormatNumber, int TerminalType, int CodeIdentifier, int Length, int &Position)
{
	HSTRING		*CommandString = 0;

	if(Position < 0)	                     // allow start values <0 to allow same logic as many MFC functions use
		Position = 0;

	Position = Find(FormatNumber, TerminalType, CodeIdentifier, Length, Position);
	if(Position >= 0)
	{
		CommandString = Entries[Position].Command;
	}

	return(CommandString);
} // FindCommandString (overloaded version #1)






//
//	FindCommandString (overloaded version #2 with a formatted key value)
//		Find an item in the table that matches the input key and return the location and size of the command string.
//
HSTRING * DATA_FORMAT::FindCommandString(const char *Key, int &Position)
{
	int			FormatNumber;
	int			TerminalType;
	int			CodeIdentifier;
	int			Length;
	HSTRING		*CommandString = 0;


	if(ParseKey(Key, &FormatNumber, &TerminalType, &CodeIdentifier, &Length) > 0)
	{
		CommandString = FindCommandString(FormatNumber, TerminalType, CodeIdentifier, Length, Position);
	}

	return(CommandString);
} // FindCommandString (overloaded version #2)






//
//	CreateNewEntryOrOverwriteExistingEntry
//		Add the format entry sent by the caller by overwriting an existing entry with the same key or creating a new
//		entry.
//
bool DATA_FORMAT::CreateNewEntryOrOverwriteExistingEntry(int FormatNumber, int TerminalType, int CodeIdentifier, int Length,
				HSTRING *Data)
{
	bool	Status = true;			// Assume success.
	int		NewEntryNumber;



// First, let's validate the input.
	Status = ValidateKey(FormatNumber, TerminalType, CodeIdentifier, Length);


// Next, let's see if an entry already exists.
	if(Status)
	{
		NewEntryNumber = Find(FormatNumber, TerminalType, CodeIdentifier, Length);
		if((NewEntryNumber >= 0) && (Entries[NewEntryNumber].Command->Compare(Data, true) == 0))
		{
		// Overwrite the existing entry
			Entries[NewEntryNumber].CodeIdentifier = CodeIdentifier;
			Entries[NewEntryNumber].FormatNumber = FormatNumber;
			Entries[NewEntryNumber].Length = Length;
			Entries[NewEntryNumber].TerminalType = TerminalType;
			Entries[NewEntryNumber].Command->Copy(Data);
		}
		else
		{
		// Create a new entry.
			AddMoreEntries();
			NewEntryNumber = NumberOfEntries;
			Entries[NewEntryNumber].CodeIdentifier = CodeIdentifier;
			Entries[NewEntryNumber].FormatNumber = FormatNumber;
			Entries[NewEntryNumber].Length = Length;
			Entries[NewEntryNumber].TerminalType = TerminalType;
			Entries[NewEntryNumber].Command = new HSTRING(Data, true);
			++NumberOfEntries;
		}
	}

	return(Status);
} // CreateNewEntryOrOverwriteExistingEntry





//
//	ProcessCommand
//		Process a data format command string that includes key information.
//
int DATA_FORMAT::ProcessCommand(const char *CommandString, bool SaveTheResult)
{
	HSTRING		OriginalCommand(CommandString, true);
	HSTRING		NextCommand(strlen(CommandString), true);	// Container for the next command to be processed
															//	(max size is the original command).
	bool			ProcessAnotherCommand = true;		// We haven't run out of commands yet.
	int			PositionOfNextCommand;
	int			FormatNumber;
	int			TerminalType;
	int			CodeIdentifier;
	int			Length;
	int			Status = RDR_CFG_SETTING_VALID;		// Assume success.



// Parse out and process commands separated by '|'.
	do  {
		ProcessAnotherCommand = OriginalCommand.FindCharacterForward('|', 0, &PositionOfNextCommand);

	// If another command follows the next one, then only copy up to the command beyond the next one; otherwise, copy the whole buffer.
		if(ProcessAnotherCommand)
		{
			OriginalCommand.CopyFromString(0, PositionOfNextCommand, &NextCommand);
			OriginalCommand.Remove(0, PositionOfNextCommand + 1);
		}
		else
		{
			NextCommand.Copy(&OriginalCommand);
		}


	// There had better be either a command and / or a format.
		if(NextCommand.Size > 0)
		{
		// Check for a command first.
			if(NextCommand.Char[0] == '%')
			{
				NextCommand.Remove(0, 1);

			// Only allowable command at this point is 'C' (clear).
				if((NextCommand.Size > 0) && ((NextCommand.Char[0] == 'C') || (NextCommand.Char[0] == 'c')))
				{
					bool	EntriesWereDeleted;
					EraseAll(&EntriesWereDeleted);
					NextCommand.Remove(0, 1);
				}

				else
				{
				// Bad command; WOOF!
					Status = RDR_CFG_SETTING_INVALID;
				}
			}
		}
		else
		{
		// No text; WOOF!
			Status = RDR_CFG_PARTIAL_SETTING_VALID;
		}


		if((NextCommand.Size) && (Status != RDR_CFG_SETTING_INVALID))
		{
		// First, let's parse the key
			Status = ParseKey((char *) NextCommand.Char, &FormatNumber, &TerminalType, &CodeIdentifier, &Length);


		// If the key is valid, let's convert the command.
			if(Status != RDR_CFG_SETTING_INVALID)
			{
				if(NextCommand.Size >= DATA_FORMAT_FORMATTED_KEY_LENGTH)
				{
					NextCommand.Remove(0, DATA_FORMAT_FORMATTED_KEY_LENGTH);

				// There must be an even number of HEX characters else this can be a partial command at best.
					if((NextCommand.Size % 2) != 0)
					{
						Status = RDR_CFG_PARTIAL_SETTING_VALID;
					}

					int			i;				// Next character to convert.
					int			NextCharacter;
					int			NumberOfCharactersUsedInConversion;
					HSTRING		ConvertedString((NextCommand.Size / 2), true);
					int			ExpectedSizeOfNextItem;

				// Convert all the 2 digit hex numbers (last # may be 1 digit if only a partial command).  If the conversion
				// ever fails, WOOF!
					i = 0;
					while((Status != RDR_CFG_SETTING_INVALID) && (i < ((int) NextCommand.Size)))
					{
						ExpectedSizeOfNextItem = ((i + 1) < ((int) NextCommand.Size)) ? 2 : 1;
						NextCharacter = NextCommand.ConvertHexToNumber(i, i + ExpectedSizeOfNextItem - 1,
												&NumberOfCharactersUsedInConversion);
						ConvertedString.AppendNumber(NextCharacter, 1);
						if(NumberOfCharactersUsedInConversion != ExpectedSizeOfNextItem)
						{
							Status = RDR_CFG_SETTING_INVALID;
						}

						i += ExpectedSizeOfNextItem;
					}


					if(Status != RDR_CFG_SETTING_INVALID)
					{
					// If length is going to cause us to exceed the size of the binary command string, WOOF!
						int	SizeOfBinaryCommand = SizeOfBinaryCommandString(ConvertedString.Size);
						if(SizeOfBinaryCommand > MaximumBinaryCommandStringLength)
						{
							Status = RDR_CFG_SETTING_INVALID;
						}
						else if((Status != RDR_CFG_PARTIAL_SETTING_VALID) && (SizeOfBinaryCommand == MaximumBinaryCommandStringLength))
						{
						// If we're at maximum length for binary command, and partial setting status is not set
						// (which indicates that we received only one byte of two), indicate that we've reached
						// maximum length.
							Status = RDR_CFG_SETTING_VALID_MAX_LENGTH;
						}
					}


				// If we successfully converted the string, build an entry for the new format.
					if((Status > 0) && (SaveTheResult))
					{
						Status = (CreateNewEntryOrOverwriteExistingEntry
							(FormatNumber, TerminalType, CodeIdentifier, Length, &ConvertedString)) ? Status : RDR_CFG_SETTING_INVALID;
					}
				}
			}
		}
	} while((ProcessAnotherCommand) && (Status > 0));


// At the end, update the binary command string.
	if((Status > 0) && (SaveTheResult))
	{
		Status = (BuildBinaryCommandString()) ? Status : RDR_CFG_SETTING_INVALID;
	}

	return(Status);
} // ProcessCommand





//
//	ProcessBinaryCommand
//		Process a binary command string.
//
bool DATA_FORMAT::ProcessBinaryCommand(const char *BinaryCommandString, int LengthOfBinaryCommandString)
{
	bool	Status = true;		// Assume success.



// There have to be at least DATA_FORMAT_FORMATTED_KEY_LENGTH characters in the input string.
	if(LengthOfBinaryCommandString >= DATA_FORMAT_FORMATTED_KEY_LENGTH)
	{
		int	FormatNumber = ((int) (BinaryCommandString[0] & 0xFF));
		int	TerminalType = ((int) (BinaryCommandString[1] & 0xFF));
		int	CodeIdentifier = ((int) (BinaryCommandString[2] & 0xFF));
		int	Length = ((int) (BinaryCommandString[3] & 0xFF)) * 256 + ((int) (BinaryCommandString[4] & 0xFF));
		HSTRING	Data(((unsigned char *) &BinaryCommandString[LengthOfBinaryCommandString - 5]),
						((unsigned int) (LengthOfBinaryCommandString - 5)), true);


	// If length is going to cause us to exceed the size of the binary command string, WOOF!
		if(SizeOfBinaryCommandString(Data.Size) > MaximumBinaryCommandStringLength)
		{
			Status = false;
		}
		else
		{
			Status = CreateNewEntryOrOverwriteExistingEntry(FormatNumber, TerminalType, CodeIdentifier, Length, &Data);
		}
	}

	else
	{
	// Not enough characters; WOOF!
		Status = false;
	}


// At the end, update the binary command string.
	if(Status)
	{
		Status = BuildBinaryCommandString();
	}

	return(Status);
} // ProcessBinaryCommand






//
//	Erase (overloaded version #1, using a format number).
//		Erase a data format.
//
bool DATA_FORMAT::Erase(int FormatNumber, int TerminalType, int CodeIdentifier, int Length, bool *EntryWasDeleted)
{
	int		Entry;				// Next entry to delete.
	bool	Status = true;		// Assume success.




// Haven't found an entry yet.
	*EntryWasDeleted = false;


// First, let's validate the input.
	Status = ValidateKey(FormatNumber, TerminalType, CodeIdentifier, Length);


// Next, let's see if an entry already exists.
	if(Status)
	{
	// Try to find the format.
		Entry = 0;
		while(Entry >= 0)
		{
			Entry = Find(FormatNumber, TerminalType, CodeIdentifier, Length, Entry);
			if(Entry >= 0)
			{
			// We found the format; erase it.
				*EntryWasDeleted = true;
				delete Entries[Entry].Command;
				int		i = Entry + 1;
				while(i < NumberOfEntries)
				{
					Entries[i - 1] = Entries[i];
					++i;
				}

				--NumberOfEntries;
			}
		}
	}
	else
	{
	// Format number is out of range.
		Status = false;
	}


// At the end, update the binary command string.
	if(Status)
	{
		Status = BuildBinaryCommandString();
	}

	return(Status);
} // Erase (overloaded version #1)






//
//	Erase (overloaded version #2, using a command string).
//		Erase a data format.
//
bool DATA_FORMAT::Erase(const char *EraseCommandString, bool *EntryWasDeleted)
{
	bool	Status = true;		// Assume success.
	int		FormatNumber;
	int		TerminalType;
	int		CodeIdentifier;
	int		Length;




// Haven't found an entry yet.
	*EntryWasDeleted = false;


// Parse and validate the key
	if((strlen(EraseCommandString) == DATA_FORMAT_FORMATTED_KEY_LENGTH) &&
				(ParseKey(EraseCommandString, &FormatNumber, &TerminalType, &CodeIdentifier, &Length) > 0))
	{
	// Erase this entry
		Status = Erase(FormatNumber, TerminalType, CodeIdentifier, Length, EntryWasDeleted);
	}
	else
	{
	// Key is bad.
		Status = false;
	}

	return(Status);
} // Erase (overloaded version #2)






//
//	EraseAll
//		Erase all data formats
//
bool DATA_FORMAT::EraseAll(bool *EntriesWereDeleted)
{
	bool	Status = true;		// Assume success.
	int		i;


// Indicate whether or not there were any entries to erase.
	if(NumberOfEntries)
	{
		*EntriesWereDeleted = true;
	}
	else
	{
		*EntriesWereDeleted = false;
	}


// Delete all entries.
	for(i = 0;    i < NumberOfEntries;    ++i)
	{
	// Delete the next entry.
		delete Entries[i].Command;
	}

	NumberOfEntries = 0;


// At the end, update the binary command string.
	if(Status)
	{
		Status = BuildBinaryCommandString();
	}


	return(Status);
} // EraseAll





//
//	Load
//		Load data formats from a binary command string.  This is the same as ProcessBinaryCommand, except that this first
//		erases all existing data format entries.
bool DATA_FORMAT::Load(const char *BinaryCommandString, int LengthOfBinaryCommandString)
{
	bool	Status = true;		// Assume success.
	bool	EntryWasDeleted;	// Return status for data format and prefix / suffix erasures (we don't use it).
	int		i;
	int		Length;


	Status = EraseAll(&EntryWasDeleted);
	i = 0;
	while((i < LengthOfBinaryCommandString) && (Status))
	{
	// Get the length of the next command
		if((i + 2) <= LengthOfBinaryCommandString)
		{
			Length = ((int) (BinaryCommandString[i] & 0xFF)) * 256 + ((int) (BinaryCommandString[i + 1] & 0xFF));
			i += 2;
			if((i + Length) <= LengthOfBinaryCommandString)
			{
			// If the length is non-zero, process the command
				if(Length)
				{
					Status = ProcessBinaryCommand(&BinaryCommandString[i], Length);
				}
				i += Length;
			}
			else
			{
			// Ran out of string; WOOF!
				Status = false;
			}
		}
		else
		{
		// Ran out of string while looking for length; WOOF!
			Status = false;
		}
	}

// If we failed, erase everything.
	if(!Status)
	{
		EraseAll(&EntryWasDeleted);
	}

	return(Status);
} // Load






//
//	BuildReportString
//		Build a report string based on the data formats.
//
void DATA_FORMAT::BuildReportString(HSTRING *ReportString, bool IncludeClearAllCommand, bool OmitTrailingPipe)
{
	int		i;		// Next entry to process.
	int		j;
	char	temp[40];


	ReportString->Copy("");


	if(IncludeClearAllCommand)
	{
		ReportString->Append("%C");
	}

	for(i = 0;    i < NumberOfEntries;    ++i)
	{
		sprintf(temp, "%01u%03u%02X%04u", Entries[i].FormatNumber, Entries[i].TerminalType, Entries[i].CodeIdentifier,
					Entries[i].Length);
		ReportString->Append(temp);
		for(j = 0;    j < ((int) Entries[i].Command->Size);    ++j)
		{
			sprintf(temp, "%02X", ((int) (Entries[i].Command->Char[j] & 0xFF)));
			ReportString->Append(temp);
		}

		if(((i + 1) < NumberOfEntries) || (!OmitTrailingPipe))
		{
			ReportString->Append("|");
		}
	}

	return;
} // BuildReportString
















//
//	Constructor for PREFIX_SUFFIX
//		Create a minimum number of empty entries, then add the entries sent by the caller.
//
PREFIX_SUFFIX::PREFIX_SUFFIX(int InputMaximumBinaryCommandStringLength, const char *InputCommandString)
{
// Allocate 10 entries to start with.
	NumberOfEntries	= 0;
	MaximumNumberOfEntries = NUMBER_OF_PREFIX_SUFFIX_ENTRIES_TO_ADD_ON_REALLOCATE;
	Entries = new PREFIX_SUFFIX_ENTRY[MaximumNumberOfEntries];

// Save binary command string parameters.
	MaximumBinaryCommandStringLength = InputMaximumBinaryCommandStringLength;
	BinaryCommandString = new char [MaximumBinaryCommandStringLength + 1];

// Load entries sent by caller.
	ProcessCommand(InputCommandString);

	return;
} //





//
//	Destructor for PREFIX_SUFFIX
//		Delete each entry, then delete the entry table.
//
PREFIX_SUFFIX::~PREFIX_SUFFIX(void)
{
	bool	EntryWasDeleted;			// Return status for data format and prefix / suffix erasures (we don't use it).



// Delete all entries.
	EraseAll(&EntryWasDeleted);

// Delete the table.
	delete [] Entries;

	delete [] BinaryCommandString;

	return;
} // Destructor





//
//	AddMoreEntries
//		If we're out of entries, add more.
//
void PREFIX_SUFFIX::AddMoreEntries(void)
{
	int		i;


// Are we out of entries in the tag table?
	if(NumberOfEntries >= MaximumNumberOfEntries)
	{
	// Allocate more space in the table, copy the existing entries, and delete the old table.
		MaximumNumberOfEntries += NUMBER_OF_PREFIX_SUFFIX_ENTRIES_TO_ADD_ON_REALLOCATE;
		PREFIX_SUFFIX_ENTRY	*NewEntries = new PREFIX_SUFFIX_ENTRY[MaximumNumberOfEntries];
		for(i = 0;    i < NumberOfEntries;    ++i)
		{
			NewEntries[i] = Entries[i];
		}

		delete [] Entries;
		Entries = NewEntries;
	}

	return;
}






//
//	ValidateKey
//		Validate the key.
//
bool PREFIX_SUFFIX::ValidateKey(int CodeIdentifier)
{
	bool	Status = true;			// Assume success.


	if((CodeIdentifier < 0) || (CodeIdentifier > MAXIMUM_PREFIX_SUFFIX_CODE_IDENTIFIER))
	{
		Status = false;
	}


	return(Status);
} // ValidateKey





//
//	ParseKey
//		Parse and validate the key.
//
int PREFIX_SUFFIX::ParseKey(const char *Key, int *CodeIdentifier)
{
	int		Status = RDR_CFG_SETTING_VALID;			// Assume success.
	int		NumberOfCharactersUsedInConversion;
	HSTRING	ParsingKey(Key, true);
	int		ExpectedSizeOfNextItem;



// If the key length is not at least PREFIX_SUFFIX_FORMATTED_KEY_LENGTH, WOOF!
	if(ParsingKey.Size < PREFIX_SUFFIX_FORMATTED_KEY_LENGTH)
	{
		Status = RDR_CFG_PARTIAL_SETTING_VALID;
	}

	*CodeIdentifier = 0;

// Convert the code identifier
	if((Status != RDR_CFG_SETTING_INVALID) && (ParsingKey.Size))
	{
		ExpectedSizeOfNextItem = (ParsingKey.Size < 2) ? ParsingKey.Size : 2;
		*CodeIdentifier = ((int) ParsingKey.ConvertHexToNumber(0, ExpectedSizeOfNextItem - 1, &NumberOfCharactersUsedInConversion));
		if(NumberOfCharactersUsedInConversion == ExpectedSizeOfNextItem)
		{
			ParsingKey.Remove(0, ExpectedSizeOfNextItem);
		}
		else
		{
			Status = RDR_CFG_SETTING_INVALID;
		}
	}


// Validate the result
	if((Status != RDR_CFG_SETTING_INVALID) && !ValidateKey(*CodeIdentifier))
	{
		Status = RDR_CFG_SETTING_INVALID;
	}

	return(Status);
} // ParseKey






//
//	SizeOfBinaryCommandString
//		Determine the size of a binary command string based on the prefix or suffix.
//
int PREFIX_SUFFIX::SizeOfBinaryCommandString(int CodeIdentifier, int SizeOfAdditionalEntry)
{
	int		Size;
	int		i;					// Next entry to process.


	i = 0;
	Size = 0;

// Process each entry until all are completed or we encounter an error.
	while(i < NumberOfEntries)
	{
		if(CodeIdentifier != Entries[i].CodeIdentifier)
		{
			Size += Entries[i].Command->Size + 3;
		}
		++i;
	}

	Size += 2;

// If requested to do so (non-negative input size), determine how much the caller specified prefix or suffix size will add.
	if(SizeOfAdditionalEntry >= 0)
	{
		Size += SizeOfAdditionalEntry + 3;
	}


	return(Size);
} // SizeOfBinaryCommandString






//
//	BuildBinaryCommandString
//		Build a binary command string based on the prefix or suffix.  Every time a change is made, this must be called to update
//		the binary command string.
//
bool PREFIX_SUFFIX::BuildBinaryCommandString(void)
{
	bool	Status = true;		// Assume success until we know otherwise.
	int		i;					// Next entry to process.
	int		p;					// Next position in binary command string.
	int		BinaryEntryLength;	// Length of next entry in binary form.



// If new string will be too big, WOOF!
	if(SizeOfBinaryCommandString() > MaximumBinaryCommandStringLength)
	{
		return(false);
	}

	i = 0;
	p = 0;


// Process each entry until all are completed or we encounter an error.
	while((Status) && (i < NumberOfEntries))
	{
		BinaryEntryLength = Entries[i].Command->Size + 1;
		if((p + BinaryEntryLength  + 2) <= MaximumBinaryCommandStringLength)
		{
			BinaryCommandString[p] = ((unsigned char) ((BinaryEntryLength / 256) & 0xFF));
			++p;
			BinaryCommandString[p] = ((unsigned char) (BinaryEntryLength & 0xFF));
			++p;
			BinaryCommandString[p] = ((unsigned char) (Entries[i].CodeIdentifier & 0xFF));
			++p;
			Entries[i].Command->CopyFromString(((unsigned char *) &BinaryCommandString[p]), Entries[i].Command->Size);
			p += Entries[i].Command->Size;
		}
		else
		{
			Status = false;
		}

		++i;
	}


// At the end, tack on two NULLs.
	if(Status)
	{
		if((p + 2) <= MaximumBinaryCommandStringLength)
		{
			BinaryCommandString[p] = 0;
			++p;
			BinaryCommandString[p] = 0;
		}
		else
		{
			Status = false;
		}
	}

	return(Status);
} // BuildBinaryCommandString







//
//	Find (overloaded version #1 with a binary key value)
//		Find an item in the table that matches the input key.
//
int PREFIX_SUFFIX::Find(int CodeIdentifier, int StartIndex)
{
	int		Entry = -1;				// No entry found yet.
	int		i;


	i = StartIndex;
	while((i < NumberOfEntries) && (Entry < 0))
	{
		if(Entries[i].CodeIdentifier == CodeIdentifier)
		{
			Entry = i;
		}
		else
		{
			++i;
		}
	}

	return(Entry);
} // Find (overloaded version #1)






//
//	Find (overloaded version #2 with a formatted key value)
//		Find an item in the table that matches the input key.
//
int PREFIX_SUFFIX::Find(const char *Key, int StartIndex)
{
	int		CodeIdentifier;
	int		Entry = -1;				// No entry found yet.


	if(ParseKey(Key, &CodeIdentifier) > 0)
	{
		Entry = Find(CodeIdentifier, StartIndex);
	}

	return(Entry);
} // Find (overloaded version #2)







//
//	FindCommandString (overloaded version #1 with a binary key value)
//		Find an item in the table that matches the input key and return the location and size of the command string.
//
HSTRING * PREFIX_SUFFIX::FindCommandString(int CodeIdentifier)
{
	HSTRING		*CommandString = 0;
	int IndexOfEntryFound;
	IndexOfEntryFound = Find(CodeIdentifier, 0);
	if(IndexOfEntryFound >= 0)
	{
		CommandString = Entries[IndexOfEntryFound].Command;
	}

	return(CommandString);
} // FindCommandString (overloaded version #1)






//
//	FindCommandString (overloaded version #2 with a formatted key value)
//		Find an item in the table that matches the input key and return the location and size of the command string.
//
HSTRING * PREFIX_SUFFIX::FindCommandString(const char *Key)
{
	int			CodeIdentifier;
	HSTRING		*CommandString = 0;


	if(ParseKey(Key, &CodeIdentifier) > 0)
	{
		CommandString = FindCommandString(CodeIdentifier);
	}

	return(CommandString);
} // FindCommandString (overloaded version #2)






//
//	CreateNewEntryOrOverwriteExistingEntry
//		Add the prefix or suffix entry sent by the caller by overwriting and existing entry with the same key or creating a new
//		entry.
//
bool PREFIX_SUFFIX::CreateNewEntryOrOverwriteExistingEntry(int CodeIdentifier, HSTRING *Data)
{
	bool	Status = true;			// Assume success.
	int		NewEntryNumber;



// First, let's validate the input.
	Status = ValidateKey(CodeIdentifier);


// Next, let's see if an entry already exists.
	if(Status)
	{
		NewEntryNumber = Find(CodeIdentifier);
//		if((NewEntryNumber >= 0) && (Entries[NewEntryNumber].Command->Compare(Data, true) == 0))
		if(NewEntryNumber >= 0)
		{
		// Overwrite the existing entry
			Entries[NewEntryNumber].CodeIdentifier = CodeIdentifier;
			Entries[NewEntryNumber].Command->Copy(Data);
		}
		else
		{
		// Create a new entry.
			AddMoreEntries();
			NewEntryNumber = NumberOfEntries;
			Entries[NewEntryNumber].CodeIdentifier = CodeIdentifier;
			Entries[NewEntryNumber].Command = new HSTRING(Data, true);
			++NumberOfEntries;
		}
	}

	return(Status);
} // CreateNewEntryOrOverwriteExistingEntry





//
//	ProcessCommand
//		Process a prefix or suffix command string that includes key information.
//
int PREFIX_SUFFIX::ProcessCommand(const char *CommandString, bool SaveTheResult)
{
	HSTRING		OriginalCommand(CommandString, true);
	HSTRING		NextCommand(strlen(CommandString), true);	// Container for the next command to be processed
															//	(max size is the original command).
	bool			ProcessAnotherCommand = true;		// We haven't run out of commands yet.
	int			PositionOfNextCommand;
	int			CodeIdentifier;
	int			Status = RDR_CFG_SETTING_VALID;		// Assume success.



// Parse out and process commands separated by '|'.
	do  {
		ProcessAnotherCommand = OriginalCommand.FindCharacterForward('|', 0, &PositionOfNextCommand);

	// If another command follows the next one, then only copy up to the command beyond the next one; otherwise, copy the whole buffer.
		if(ProcessAnotherCommand)
		{
			OriginalCommand.CopyFromString(0, PositionOfNextCommand, &NextCommand);
			OriginalCommand.Remove(0, PositionOfNextCommand + 1);
		}
		else
		{
			NextCommand.Copy(&OriginalCommand);
		}


	// There had better be either a command and / or a prefix / suffix.
		if(NextCommand.Size > 0)
		{
		// Check for a command first.
			if(NextCommand.Char[0] == '%')
			{
				NextCommand.Remove(0, 1);

			// Only allowable command at this point is 'C' (clear).
				if((NextCommand.Size > 0) && ((NextCommand.Char[0] == 'C') || (NextCommand.Char[0] == 'c')))
				{
					bool	EntriesWereDeleted;
					EraseAll(&EntriesWereDeleted);
					NextCommand.Remove(0, 1);
				}

				else
				{
				// Bad command; WOOF!
					Status = RDR_CFG_SETTING_INVALID;
				}
			}
		}
		else
		{
		// No text; WOOF!
			Status = RDR_CFG_PARTIAL_SETTING_VALID;
		}


		if((NextCommand.Size) && (Status != RDR_CFG_SETTING_INVALID))
		{
		// First, let's parse the key
			Status = ParseKey((char *) NextCommand.Char, &CodeIdentifier);


		// If the key is valid, let's convert the command.
			if(Status != RDR_CFG_SETTING_INVALID)
			{
				if(NextCommand.Size >= PREFIX_SUFFIX_FORMATTED_KEY_LENGTH)
				{
					NextCommand.Remove(0, PREFIX_SUFFIX_FORMATTED_KEY_LENGTH);

				// There must be an even number of HEX characters else this can be a partial command at best.
					if((NextCommand.Size % 2) != 0)
					{
						Status = RDR_CFG_PARTIAL_SETTING_VALID;
					}

					int			i;				// Next character to convert.
					int			NextCharacter;
					int			NumberOfCharactersUsedInConversion;
					HSTRING		ConvertedString((NextCommand.Size / 2), true);
					int			ExpectedSizeOfNextItem;

				// Convert all the 2 digit hex numbers (last # may be 1 digit if only a partial command).  If the conversion
				// ever fails, WOOF!
					i = 0;
					while((Status != RDR_CFG_SETTING_INVALID) && (i < ((int) NextCommand.Size)))
					{
						ExpectedSizeOfNextItem = ((i + 1) < ((int) NextCommand.Size)) ? 2 : 1;
						NextCharacter = NextCommand.ConvertHexToNumber(i, i + ExpectedSizeOfNextItem - 1,
												&NumberOfCharactersUsedInConversion);
						ConvertedString.AppendNumber(NextCharacter, 1);
						if(NumberOfCharactersUsedInConversion != ExpectedSizeOfNextItem)
						{
							Status = RDR_CFG_SETTING_INVALID;
						}

						i += ExpectedSizeOfNextItem;
					}

					if(Status != RDR_CFG_SETTING_INVALID)
					{
					// If length is going to cause us to exceed the size of the binary command string, WOOF!
						int	SizeOfBinaryCommand = SizeOfBinaryCommandString(CodeIdentifier, ConvertedString.Size);
						if(SizeOfBinaryCommand > MaximumBinaryCommandStringLength)
						{
							Status = RDR_CFG_SETTING_INVALID;
						}
						else if((Status != RDR_CFG_PARTIAL_SETTING_VALID) && (SizeOfBinaryCommand == MaximumBinaryCommandStringLength))
						{
						// If we're at maximum length for binary command, and partial setting status is not set
						// (which indicates that we received only one byte of two), indicate that we've reached
						// maximum length.
							Status = RDR_CFG_SETTING_VALID_MAX_LENGTH;
						}
					}


				// If we successfully converted the string, build an entry for the new prefix or suffix .
					if((Status > 0) && (SaveTheResult))
					{
						Status = (CreateNewEntryOrOverwriteExistingEntry
									(CodeIdentifier, &ConvertedString)) ? Status : RDR_CFG_SETTING_INVALID;
					}
				}
			}
		}
	} while((ProcessAnotherCommand) && (Status));


// At the end, update the binary command string.
	if((Status > 0) && (SaveTheResult))
	{
		Status = (BuildBinaryCommandString()) ? Status : RDR_CFG_SETTING_INVALID;
	}

	return(Status);
} // ProcessCommand





//
//	ProcessBinaryCommand
//		Process a binary command string.
//
bool PREFIX_SUFFIX::ProcessBinaryCommand(const char *BinaryCommandString, int LengthOfBinaryCommandString)
{
	bool	Status = true;		// Assume success.



// There have to be at least PREFIX_SUFFIX_FORMATTED_KEY_LENGTH characters in the input string.
	if(LengthOfBinaryCommandString >= PREFIX_SUFFIX_FORMATTED_KEY_LENGTH)
	{
		int		CodeIdentifier = ((int) (BinaryCommandString[0] & 0xFF));
		HSTRING	Data(((unsigned char *) &BinaryCommandString[1]), ((unsigned int) (LengthOfBinaryCommandString - 1)), true);


	// If length is going to cause us to exceed the size of the binary command string, WOOF!
		if(SizeOfBinaryCommandString(CodeIdentifier, Data.Size) > MaximumBinaryCommandStringLength)
		{
			Status = false;
		}
		else
		{
			Status = CreateNewEntryOrOverwriteExistingEntry(CodeIdentifier, &Data);
		}
	}

	else
	{
	// Not enough characters; WOOF!
		Status = false;
	}


// At the end, update the binary command string.
	if(Status)
	{
		Status = BuildBinaryCommandString();
	}

	return(Status);
} // ProcessBinaryCommand






//
//	Erase (overloaded version #1, using a code identifier).
//		Erase a prefix or suffix .
//
bool PREFIX_SUFFIX::Erase(int EraseCodeIdentifier, bool *EntryWasDeleted)
{
	int		Entry;				// Next entry to delete.
	bool	Status = true;		// Assume success.




// Haven't found an entry yet.
	*EntryWasDeleted = false;


// Validate the code identifier.
	if((EraseCodeIdentifier >= 0) && (EraseCodeIdentifier <= MAXIMUM_PREFIX_SUFFIX_CODE_IDENTIFIER))
	{
	// Try to find the prefix or suffix .
		Entry = 0;
		while(Entry >= 0)
		{
			Entry = Find(EraseCodeIdentifier);
			if(Entry >= 0)
			{
			// We found the format; erase it.
				*EntryWasDeleted = true;
				delete Entries[Entry].Command;
				int		i = Entry + 1;
				while(i < NumberOfEntries)
				{
					Entries[i - 1] = Entries[i];
					++i;
				}

				--NumberOfEntries;
			}
		}
	}
	else
	{
	// Code identifier number is out of range.
		Status = false;
	}


// At the end, update the binary command string.
	if(Status)
	{
		Status = BuildBinaryCommandString();
	}

	return(Status);
} // Erase (overloaded version #1)






//
//	Erase (overloaded version #2, using a command string).
//		Erase a prefix or suffix .
//
bool PREFIX_SUFFIX::Erase(const char *EraseCommandString, bool *EntryWasDeleted)
{
	bool	Status = true;		// Assume success.
	int		CodeIdentifier;



// Haven't found an entry yet.
	*EntryWasDeleted = false;


// Parse and validate the key
	if((strlen(EraseCommandString) == PREFIX_SUFFIX_FORMATTED_KEY_LENGTH) &&
						(ParseKey(EraseCommandString, &CodeIdentifier) > 0))
	{
	// Erase this entry
		Status = Erase(CodeIdentifier, EntryWasDeleted);
	}
	else
	{
	// Code identifier is bad.
		Status = false;
	}

	return(Status);
} // Erase (overloaded version #2)






//
//	EraseAll
//		Erase all prefix or suffixes
//
bool PREFIX_SUFFIX::EraseAll(bool *EntriesWereDeleted)
{
	bool	Status = true;		// Assume success.
	int		i;


// Indicate whether or not there were any entries to erase.
	if(NumberOfEntries)
	{
		*EntriesWereDeleted = true;
	}
	else
	{
		*EntriesWereDeleted = false;
	}


// Delete all entries.
	for(i = 0;    i < NumberOfEntries;    ++i)
	{
	// Delete the next entry.
		delete Entries[i].Command;
	}

	NumberOfEntries = 0;


// At the end, update the binary command string.
	if(Status)
	{
		Status = BuildBinaryCommandString();
	}


	return(Status);
} // EraseAll





//
//	Load
//		Load prefix or suffixes from a binary command string.  This is the same as ProcessBinaryCommand, except that this first
//		erases all existing prefix or suffix entries.
bool PREFIX_SUFFIX::Load(const char *BinaryCommandString, int LengthOfBinaryCommandString)
{
	bool	Status = true;		// Assume success.
	bool	EntryWasDeleted;	// Return status for data format and prefix / suffix erasures (we don't use it).
	int		i;
	int		Length;


	Status = EraseAll(&EntryWasDeleted);
	i = 0;
	while((i < LengthOfBinaryCommandString) && (Status))
	{
	// Get the length of the next command
		if((i + 2) <= LengthOfBinaryCommandString)
		{
			Length = ((int) (BinaryCommandString[i] & 0xFF)) * 256 + ((int) (BinaryCommandString[i + 1] & 0xFF));
			i += 2;
			if((i + Length) <= LengthOfBinaryCommandString)
			{
			// If the length is non-zero, process the command
				if(Length)
				{
					Status = ProcessBinaryCommand(&BinaryCommandString[i], Length);
				}
				i += Length;
			}
			else
			{
			// Ran out of string; WOOF!
				Status = false;
			}
		}
		else
		{
		// Ran out of string while looking for length; WOOF!
			Status = false;
		}
	}

// If we failed, erase everything.
	if(!Status)
	{
		EraseAll(&EntryWasDeleted);
	}

	return(Status);
} // Load






//
//	BuildReportString
//		Build a report string based on the prefix or suffixes.
//
void PREFIX_SUFFIX::BuildReportString(HSTRING *ReportString, bool IncludeClearAllCommand, bool OmitTrailingPipe)
{
	int		i;		// Next entry to process.
	int		j;
	char	temp[40];


	ReportString->Copy("");


	if(IncludeClearAllCommand)
	{
		ReportString->Append("%C");
	}

	for(i = 0;    i < NumberOfEntries;    ++i)
	{
		sprintf(temp, "%02X", Entries[i].CodeIdentifier);
		ReportString->Append(temp);
		for(j = 0;    j < ((int) (Entries[i].Command->Size & 0xFF));    ++j)
		{
			sprintf(temp, "%02X", ((int) (Entries[i].Command->Char[j] & 0xFF)));
			ReportString->Append(temp);
		}

		if(((i + 1) < NumberOfEntries) || (!OmitTrailingPipe))
		{
			ReportString->Append("|");
		}
	}

	return;
} // BuildReportString


















//
//	Constructor for SUB_TAG_TABLE
//		Create a minimum number of empty entries.
//
SUB_TAG_TABLE::SUB_TAG_TABLE( int InputNumberOfWorkgroups )
{
	NumberOfWorkgroups = InputNumberOfWorkgroups;

// Allocate 10 sub tag table entries to start with.
	NumberOfEntries	= 0;
	MaximumNumberOfEntries = NUMBER_OF_SUB_TAG_TABLE_ENTRIES_TO_ADD_ON_REALLOCATE;
	EntryIndex = new unsigned int[MaximumNumberOfEntries];
	Entries = new SUB_TAG_TABLE_ENTRY[MaximumNumberOfEntries];

	return;
}





//
//	Destructor for SUB_TAG_TABLE
//		Delete each entry, then delete the entry and entry index tables.
//
SUB_TAG_TABLE::~SUB_TAG_TABLE(void)
{
	unsigned int	i;
	int				j;
	int				k;


// Delete all entries in the sub tag table.
	for(i = 0;    i < NumberOfEntries;    ++i)
	{
	// Delete the default and stored string values.
		if(Entries[i].TypeFlags & STRING)
		{
		// If the next entry is a string, delete as a char pointer.
			delete Entries[i].StandardDefaultValue.c;
			delete Entries[i].DefaultValue.c;

			for( k = 0;    k < Entries[i].NumberOfWorkgroups;    ++k )
			{
				delete Entries[i].WorkingValue[ k ].c;
				delete Entries[i].ActiveValue[ k ].c;
			}
		}

	// Delete the default and stored binary string values.
		else if((Entries[i].TypeFlags & BINARY_STRING) || (Entries[i].TypeFlags & ASCII_STRING) ||
				(Entries[i].TypeFlags & HEX_STRING) || (Entries[i].TypeFlags & UNICODE_STRING))
		{
		// If the next entry is a string, delete as a char pointer.
			delete Entries[i].StandardDefaultValue.BinaryString;
			delete Entries[i].DefaultValue.BinaryString;

			for( k = 0;    k < Entries[i].NumberOfWorkgroups;    ++k )
			{
				delete Entries[i].WorkingValue[ k ].BinaryString;
				delete Entries[i].ActiveValue[ k ].BinaryString;
			}

			delete Entries[i].InstantiatorCopyOfActiveValue.BinaryString;
		}

	// Delete the default and stored string values, string list values, and string list for string list items.
		else if(Entries[i].TypeFlags & STRING_LIST)
		{
		// If the next entry is a string, delete as a char pointer.
			delete Entries[i].StandardDefaultValue.c;
			delete Entries[i].DefaultValue.c;

			for( k = 0;    k < Entries[i].NumberOfWorkgroups;    ++k )
			{
				delete Entries[i].WorkingValue[ k ].c;
				delete Entries[i].ActiveValue[ k ].c;
			}

			if(Entries[i].NumberOfEntriesInListRange > 0)
			{
				for(j = 0;    j < Entries[i].NumberOfEntriesInListRange;    ++j)
				{
					delete Entries[i].StringListRange[j];
				}

				delete Entries[i].StringListRange;
			}
		}

	// Delete numeric list range for numeric list items
		else if((Entries[i].TypeFlags & NUMERIC_LIST) && (Entries[i].NumberOfEntriesInListRange > 0))
		{
			delete Entries[i].NumericListRange;
		}

	// Delete data format
		else if(Entries[i].TypeFlags & FORMAT_STRING)
		{
			delete Entries[i].StandardDefaultValue.DataFormat;
			delete Entries[i].DefaultValue.DataFormat;

			for( k = 0;    k < Entries[i].NumberOfWorkgroups;    ++k )
			{
				delete Entries[i].WorkingValue[ k ].DataFormat;
				delete Entries[i].ActiveValue[ k ].DataFormat;
			}

			delete Entries[i].InstantiatorCopyOfActiveValue.DataFormat;
		}

	// Delete prefix or suffix
		else if(Entries[i].TypeFlags & PREFIX_SUFFIX_STRING)
		{
			delete Entries[i].StandardDefaultValue.PrefixSuffix;
			delete Entries[i].DefaultValue.PrefixSuffix;

			for( k = 0;    k < Entries[i].NumberOfWorkgroups;    ++k )
			{
				delete Entries[i].WorkingValue[ k ].PrefixSuffix;
				delete Entries[i].ActiveValue[ k ].PrefixSuffix;
			}

			delete Entries[i].InstantiatorCopyOfActiveValue.PrefixSuffix;
		}

		delete Entries[i].WorkingValue;
		delete Entries[i].ActiveValue;
	}

// Delete the sub-tag table and its' index.
	delete [] Entries;
	Entries=NULL;
	delete [] EntryIndex;

	return;
}





//
//	Find
//		Find an item in the sorted (using the index) sub-tag table.  The entry number returned to the caller points to the indexed entry
//		that was found or to the first indexed entry that compares greater than the name sent by the caller.
//
bool SUB_TAG_TABLE::Find(const char *SettingName, unsigned int *EntryNumber, unsigned int *EntryOrder)
{
	bool	SettingWasFound = false;		// Didn't find setting yet.


/*
	*EntryOrder = 0;
	while((*EntryOrder < NumberOfEntries) && (strnicmp(SettingName, Entries[EntryIndex[*EntryOrder]].SubTag, SIZE_OF_SUB_TAG) < 0))
	{
		++*EntryOrder;
	}

// If we didn't move beyond the end of the table and the name sent by the caller matches the name of the entry last processed, then
// indicate to the caller that we found the setting.
	if((*EntryOrder < NumberOfEntries) && (strnicmp(SettingName, Entries[EntryIndex[*EntryOrder]].SubTag, SIZE_OF_SUB_TAG) == 0))
	{
		*EntryNumber = EntryIndex[*EntryOrder];
		SettingWasFound = true;
	}
*/
	int			Lower = 0;
	int			Upper = (int) NumberOfEntries - 1;
	int			Result = -1;
	while(Lower <= Upper)
	{
		*EntryOrder = ((unsigned int) ((Lower + Upper) / 2));
		Result = strnicmp(SettingName, Entries[EntryIndex[*EntryOrder]].SubTag, SIZE_OF_SUB_TAG);
		if(Result == 0)
		{
			break;
		}
		else if(Result < 0)
		{
			Upper = (int) *EntryOrder - 1;
		}
		else
		{
			Lower = (int) *EntryOrder + 1;
		}
	}

	if(Result == 0)
	{
		*EntryNumber = EntryIndex[*EntryOrder];
		SettingWasFound = true;
	}
	else
	{
		*EntryOrder = 0;
		while((*EntryOrder < NumberOfEntries) && (strnicmp(SettingName, Entries[EntryIndex[*EntryOrder]].SubTag, SIZE_OF_SUB_TAG) > 0))
		{
			++*EntryOrder;
		}
	}


	return(SettingWasFound);
}





//
//	AddMoreEntries
//		If we're out of sub-tag table entries, add more.
//
void SUB_TAG_TABLE::AddMoreEntries(void)
{
	unsigned int	i;


// Are we out of entries in the sub-tag table?
	if(NumberOfEntries >= MaximumNumberOfEntries)
	{
	// Allocate more space in the sub-tag table, copy the existing entries, and delete the old table.
		MaximumNumberOfEntries += NUMBER_OF_SUB_TAG_TABLE_ENTRIES_TO_ADD_ON_REALLOCATE;
		unsigned int				*NewEntryIndex = new unsigned int[MaximumNumberOfEntries];
		SUB_TAG_TABLE_ENTRY	*NewEntries = new SUB_TAG_TABLE_ENTRY[MaximumNumberOfEntries];
		for(i = 0;    i < NumberOfEntries;    ++i)
		{
			NewEntryIndex[i] = EntryIndex[i];
			NewEntries[i] = Entries[i];
			if(&(Entries[i].LowRange) == Entries[i].WorkingLowRangeAddress)
			{
				NewEntries[i].WorkingLowRangeAddress = &(NewEntries[i].LowRange);
			}
			if(&(Entries[i].HighRange) == Entries[i].WorkingHighRangeAddress)
			{
				NewEntries[i].WorkingHighRangeAddress = &(NewEntries[i].HighRange);
			}
		}

		delete [] EntryIndex;
		delete [] Entries;
		EntryIndex = NewEntryIndex;
		Entries = NewEntries;
	}

	return;
}





//
//	AddBasicInformationForNewSubTag
//		Create a new sub-tag with basic information.  If the sub-tag already exists, WOOF.
//		Return -1 for bad range, -2 for
//
int SUB_TAG_TABLE::AddBasicInformationForNewSubTag(const char *SettingName, bool (*EntryFunction) (void),
																bool (*ExitFunction) (void), unsigned int Modifier )
{
	unsigned int	NewSubTagTableEntry = NOT_FOUD;		// Haven't found a valid tag entry yet.
	unsigned int	NewSubTagTableEntryOrder = NOT_FOUD;	// Haven't found a valid tag entry yet.
	int				i;
	int				ReturnCode;



// Make sure that the name is valid and see if the sub-tag already exists.  If not, WOOF!
	HSTRING		hstrSettingName( ( char * ) SettingName, false, true, true );
	if( hstrSettingName.RangeCheck( 'A', 'Z', 'a', 'z', '0', '9', '_', '_' ) )
	{
		if( !Find( SettingName, &NewSubTagTableEntry, &NewSubTagTableEntryOrder ) )
		{
		// If this setting is designated as more than one of the following, WOOF, since these are mutually exclusive: base only,
		// scanner only, or workgroup.
			int		ExclusionFlagCount = 0;
			if( Modifier & BASE_ONLY_SETTING )
			{
				++ExclusionFlagCount;
			}
			if( Modifier & SCANNER_ONLY_SETTING )
			{
				++ExclusionFlagCount;
			}
			if( Modifier & WORKGROUP_SETTING )
			{
				++ExclusionFlagCount;
			}

			if( ExclusionFlagCount <= 1 )
			{
				int		NumberOfWorkgroupsInThisSetting = ( Modifier & WORKGROUP_SETTING ) ? NumberOfWorkgroups : 1;

				AddMoreEntries();		// Add more entries if needed.

			// Add the new sub-tag to the end of the sub-tag table.
				strncpy(Entries[NumberOfEntries].SubTag, SettingName, SIZE_OF_SUB_TAG);
				Entries[NumberOfEntries].SubTag[SIZE_OF_SUB_TAG] = 0;
				Entries[NumberOfEntries].EntryFunction = EntryFunction;
				Entries[NumberOfEntries].ExitFunction = ExitFunction;

				Entries[NumberOfEntries].StandardDefaultValue.Updated = false;						// These should never be
				Entries[NumberOfEntries].StandardDefaultValue.ModifiedSinceInstantiation = false;	//	changed, but let's
																									//	initialize themanyway.

				Entries[NumberOfEntries].DefaultValue.Updated = false;
				Entries[NumberOfEntries].DefaultValue.ModifiedSinceInstantiation = false;

				Entries[NumberOfEntries].WorkingValue = new SUB_TAG_TABLE_VALUE_ENTRY[ NumberOfWorkgroupsInThisSetting ];
				Entries[NumberOfEntries].ActiveValue = new SUB_TAG_TABLE_VALUE_ENTRY[ NumberOfWorkgroupsInThisSetting ];

				for( i = 0;    i < NumberOfWorkgroupsInThisSetting;    ++i )
				{
					Entries[NumberOfEntries].WorkingValue[ i ].Updated = false;
					Entries[NumberOfEntries].WorkingValue[ i ].ModifiedSinceInstantiation = false;
					Entries[NumberOfEntries].ActiveValue[ i ].Updated = false;
					Entries[NumberOfEntries].ActiveValue[ i ].ModifiedSinceInstantiation = false;
				}

				Entries[NumberOfEntries].InstantiatorCopyOfActiveValue.Updated = false;
				Entries[NumberOfEntries].InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = false;

				Entries[NumberOfEntries].SpecialCommandAction = NO_ACTION;
				Entries[NumberOfEntries].NumberOfWorkgroups = NumberOfWorkgroupsInThisSetting;
				++NumberOfEntries;


			// NewSubTagTableEntry already points to the insertion spot in the index, so we just have to move the index values up by one and
			// set the new entry.
				for(i = NumberOfEntries - 1;    i > (int) NewSubTagTableEntryOrder;    --i)
				{
					EntryIndex[i] = EntryIndex[i-1];
				}
				EntryIndex[NewSubTagTableEntryOrder] = NumberOfEntries - 1;

				ReturnCode = EntryIndex[NewSubTagTableEntryOrder];
			}
			else
			{
				ReturnCode = ADD_BASIC_INFORMATION_ILLEGAL_FLAG_OR_MODIFIER_COMBINATION;
			}
		}
		else
		{
			ReturnCode = ADD_BASIC_INFORMATION_NAME_ALREADY_EXISTS;
		}
	}
	else
	{
		ReturnCode = ADD_BASIC_INFORMATION_ILLEGAL_CHARACTERS_IN_NAME;
	}

	return ReturnCode;
} // AddBasicInformationForNewSubTag








//
//	Constructor for TAG_TABLE
//		Create a minimum number of empty entries.
//
TAG_TABLE::TAG_TABLE( int InputNumberOfWorkgroups )
{
	NumberOfWorkgroups = InputNumberOfWorkgroups;

// Allocate 10 tag table entries to start with.
	NumberOfEntries	= 0;
	MaximumNumberOfEntries = NUMBER_OF_TAG_TABLE_ENTRIES_TO_ADD_ON_REALLOCATE;
	EntryIndex = new unsigned int[MaximumNumberOfEntries];
	Entries = new TAG_TABLE_ENTRY[MaximumNumberOfEntries];

	return;
}





//
//	Destructor for TAG_TABLE
//		Delete each entry, then delete the entry and entry index tables.
//
TAG_TABLE::~TAG_TABLE(void)
{
	unsigned int	i;


// Delete all entries in the tag table.
	for(i = 0;    i < NumberOfEntries;    ++i)
	{
	// Delete the sub-tag table for this tag.
		delete Entries[i].SubTagTable;
	}

// Delete the tag table and its' index.
	delete [] Entries;
	delete [] EntryIndex;

	return;
}





//
//	Find
//		Find an item in the sorted (using the index) tag table.  The entry number returned to the caller points to the indexed entry
//		that was found or to the first indexed entry that compares greater than the name sent by the caller.
//
bool TAG_TABLE::Find(const char *SettingName, unsigned int *EntryNumber, unsigned int *EntryOrder)
{
	bool	SettingWasFound = false;		// Didn't find setting yet.


/*
	*EntryOrder = 0;
	while((*EntryOrder < NumberOfEntries) && (strnicmp(SettingName, Entries[EntryIndex[*EntryOrder]].Tag, SIZE_OF_TAG) < 0))
	{
		++*EntryOrder;
	}

// If we didn't move beyond the end of the table and the name sent by the caller matches the name of the entry last processed, then
// indicate to the caller that we found the setting.
	if((*EntryOrder < NumberOfEntries) && (strnicmp(SettingName, Entries[EntryIndex[*EntryOrder]].Tag, SIZE_OF_TAG) == 0))
	{
		*EntryNumber = EntryIndex[*EntryOrder];
		SettingWasFound = true;
	}
*/
	int			Lower = 0;
	int			Upper = (int) NumberOfEntries - 1;
	int			Result = -1;
	while(Lower <= Upper)
	{
		*EntryOrder = ((unsigned int) ((Lower + Upper) / 2));
		Result = strnicmp(SettingName, Entries[EntryIndex[*EntryOrder]].Tag, SIZE_OF_TAG);
		if(Result == 0)
		{
			break;
		}
		else if(Result < 0)
		{
			Upper = (int) *EntryOrder - 1;
		}
		else
		{
			Lower = (int) *EntryOrder + 1;
		}
	}

	if(Result == 0)
	{
		*EntryNumber = EntryIndex[*EntryOrder];
		SettingWasFound = true;
	}
	else
	{
		*EntryOrder = 0;
		while((*EntryOrder < NumberOfEntries) && (strnicmp(SettingName, Entries[EntryIndex[*EntryOrder]].Tag, SIZE_OF_TAG) > 0))
		{
			++*EntryOrder;
		}
	}


	return(SettingWasFound);
}





//
//	AddMoreEntries
//		If we're out of tag table entries, add more.
//
void TAG_TABLE::AddMoreEntries(void)
{
	unsigned int	i;


// Are we out of entries in the tag table?
	if(NumberOfEntries >= MaximumNumberOfEntries)
	{
	// Allocate more space in the tag table, copy the existing entries, and delete the old table.
		MaximumNumberOfEntries += NUMBER_OF_TAG_TABLE_ENTRIES_TO_ADD_ON_REALLOCATE;
		unsigned int			*NewEntryIndex = new unsigned int[MaximumNumberOfEntries];
		TAG_TABLE_ENTRY	*NewEntries = new TAG_TABLE_ENTRY[MaximumNumberOfEntries];
		for(i = 0;    i < NumberOfEntries;    ++i)
		{
			NewEntryIndex[i] = EntryIndex[i];
			NewEntries[i] = Entries[i];
		}

		delete [] EntryIndex;
		delete [] Entries;
		EntryIndex = NewEntryIndex;
		Entries = NewEntries;
	}

	return;
}





//
//	GetExistingTagOrCreateNewTag
//		If the tag already exists, return an index.  Otherwise, create a new tag and return the index of that tag.
//
int TAG_TABLE::GetExistingTagOrCreateNewTag(const char *SettingName)
{
	unsigned int	NewTagTableEntry = NOT_FOUD;			// Haven't found a valid tag entry yet.
	unsigned int	NewTagTableEntryOrder = NOT_FOUD;		// Haven't found a valid tag entry yet.
	int				i;


// See if the tag already exists.  If it doesn't, add it!
	if(!Find(SettingName, &NewTagTableEntry, &NewTagTableEntryOrder))
	{
		AddMoreEntries();		// Add more entries if needed.


	// Add the new sub-tag to the end of the sub-tag table.
		strncpy(Entries[NumberOfEntries].Tag, SettingName, SIZE_OF_TAG);
		Entries[NumberOfEntries].Tag[SIZE_OF_SUB_TAG] = 0;
		Entries[NumberOfEntries].SubTagTable = new SUB_TAG_TABLE( NumberOfWorkgroups );
		++NumberOfEntries;


	// NewTagTableEntry already points to the insertion spot in the index, so we just have to move the index values up by one and
	// set the new entry.
		for(i = NumberOfEntries - 1;    i > (int) NewTagTableEntryOrder;    --i)
		{
			EntryIndex[i] = EntryIndex[i-1];
		}
		EntryIndex[NewTagTableEntryOrder] = NumberOfEntries - 1;
	}

	return(EntryIndex[NewTagTableEntryOrder]);
}







bool EntryWoof(void)
{
//	xputstring("Entry...\r\n");
	return(true);
}





bool ExitWoof(void)
{
//	xputstring("Exit...\r\n");
	return(true);
}






//
// Constructor for READER_CONFIGURATION_STORAGE
//      Initialize the default edit format and copy defaults to start with.
//		Zero out application code version numbers to indicate that they haven't
//		been set yet.  Since they're reader code specific, we will make no
//		assumptions about what they should be.  Set the configuration version as well.
//		Indicate that there are no known manufacturing settings yet.
//
READER_CONFIGURATION_STORAGE::READER_CONFIGURATION_STORAGE( int NumberOfWorkgroups )
{
	int			i;


	SettingChangesEnabled = true;
	FirstTimeLoadWorkingConfigurationInFlash=false;
	UltimatePrivilegeLevel = 0;
	DirectDefaultCommandPrivilege = false;
	AffectedTableChangedToDefalt=false;
	CustomDefaultProgrammingMode = false;
	OverrideDefaultingDisabled = false;
	TableNeedChange=false;
	TableSettingCommandisProcessing=false;
	CurrentTableForScannedCommand=0;
	TableSettingUpdateSimultaneouslyForScannedCommand=false;
	CurrentTableNumber=0;
	ActiveTableNumber=0;
	TwoTableUpdateSimultaneously=false;
	ScannedCBLTBLMenu=false;


	this->NumberOfWorkgroups = NumberOfWorkgroups;
	CurrentWorkgroupNumber = 0;
	ActiveWorkgroupNumber = 0;

	TagCompressedModeEnabled = false;

	ReportBaseOnlySettings = true;
	ReportScannerOnlySettings = true;
	ReportCommonSettings = true;
	WorkgroupSettingsHandling = REPORT_AND_SET_ALL_WORKGROUP_SETTINGS_IN_CURRENT_WORKGROUP;


	ReportSettingsUpdatedSinceLastCommand = true;
	ReportSettingsNotUpdatedSinceLastCommand = true;
	ReportSettingsUpdatedSinceStartup = true;
	ReportSettingsNotUpdatedSinceStartup = true;
	ReportSettingsWhenActiveIsEqualToWorking = true;
	ReportSettingsWhenActiveIsNotEqualToWorking = true;
	SettingsUpdatedSinceLastCommandModifiers = SETTINGS_UPDATED_SINCE_LAST_COMMAND_NO_MODIFIERS;


	SortWildcardQueries = false;


	ReportHiddenSettings = 0;

	IsMenuingInProcess = false;

// By default, we'll omit trailing pipe on all storage and range queries.
	OmitTrailingPipeOnValueQueries = true;
	OmitTrailingPipeOnRangeQueries = true;

// Create the tag table.
	TagTable = new TAG_TABLE( NumberOfWorkgroups );

// Nothing has been updated yet.
	AllActiveValuesWereDefaulted = false;
	OneOrMoreActiveValuesWereDefaulted = false;
	OneOrMoreDefaultsUpdated = false;
	OneOrMoreWorkingValuesUpdated = false;
	OneOrMoreActiveValuesUpdated = false;
	OneOrMoreQueryCommandsWereProcessed = false;
	OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated = false;
	OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = false;
	OneOrMoreCommonValuesInWorkingStorageWereUpdated = false;
	OneOrMoreCommonValuesInActiveStorageWereUpdated = false;
	OneOrMoreBaseOnlyValuesInWorkingStorageWereUpdated = false;
	OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = false;
	OneOrMoreScannerOnlyValuesInWorkingStorageWereUpdated = false;
	OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = false;
	OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated = new bool[ NumberOfWorkgroups ];
	OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated = new bool[ NumberOfWorkgroups ];
	for( i = 0;    i < NumberOfWorkgroups;    ++i )
	{
		OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated[ i ] = false;
		OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ i ] = false;
	}

// Allocate space for extraneous text that follows a command
	ExtraneousTextAfterCommand = new HSTRING(MINIMUM_SIZE_OF_EXTRANEOUS_TEXT_AFTER_COMMAND, true);

	return;
} // Constructor





//
// Destructor for READER_CONFIGURATION_STORAGE
//      Delete all tag and sub-tag table entries.
//
READER_CONFIGURATION_STORAGE::~READER_CONFIGURATION_STORAGE(void)
{
	delete TagTable;
	delete ExtraneousTextAfterCommand;

	delete [] OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated;
	delete [] OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated;

	return;
} // Destructor





//
// EnableSettingChanges
//		Enable or disable the ability to change settings.
//
void READER_CONFIGURATION_STORAGE::EnableSettingChanges( bool Enable )
{
	SettingChangesEnabled = Enable;
	return;
} // EnableSettingChanges







//
// NumberOfAvailableWorkgroups
//		Return the number of defined workgroups.
//
int READER_CONFIGURATION_STORAGE::NumberOfAvailableWorkgroups( void )
{
	return NumberOfWorkgroups;
} // NumberOfAvailableWorkgroups







//
// CurrentWorkgroup
//      Return the current workgroup number to the caller.
//
int READER_CONFIGURATION_STORAGE::CurrentWorkgroup( void )
{
	return CurrentWorkgroupNumber;
} // CurrentWorkgroup

//
// CurrentWorkTable
//    Return the current worktable number to the caller
int READER_CONFIGURATION_STORAGE::CurrentWorkTable(void)
{
	return CurrentTableNumber;
}// CurrentWorktable


// CurrentWorkTableForScannedMenu
// Return the current worktable number for scanned menu to the caller.
int READER_CONFIGURATION_STORAGE::CurrentWorkTableForScannedMenu(void)
{
	return CurrentTableForScannedCommand;
}





//
// ValidateMenuParameterCharacters
//		Ensure that the menu parameter string sent by the caller only contains printable characters, and does not include a terminator character or
//		colon (used for device names).
//
bool READER_CONFIGURATION_STORAGE::ValidateMenuParameterCharacters( HSTRING *ParameterString )
{
	return ParameterString->RangeCheck( 0x20, 0x20, 0x22, 0x25, 0x27, 0x2D, 0x2F, 0x39, 0x3B, 0x3F, 0x41, 0x7E );
} // ValidateMenuParameterCharacters







//
// MoveInstantiatorCopyOfActiveValueToCurrentWorkgroupInActiveValueTable
//      Resynchronize instantiator copy of active value to the current workgroup in the active value table for the item sent by the caller.
//
void READER_CONFIGURATION_STORAGE::MoveInstantiatorCopyOfActiveValueToCurrentWorkgroupInActiveValueTable
					( SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntry )
{
	SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*ActiveValue = ActiveValueForCurrentWorkgroup( SubTagTableEntry );

// Update the next sub-tag table item.
	if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BOOLEAN )
	{
		ActiveValue->b = *(SubTagTableEntry->InstantiatorCopyOfActiveValue.b);
	}

	else if(( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING ) ||
					( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING_LIST ))
	{
		strcpy( ActiveValue->c, SubTagTableEntry->InstantiatorCopyOfActiveValue.c );
	}

	else if(( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BINARY_STRING ) ||
				( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::ASCII_STRING ) ||
				( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::HEX_STRING ) ||
				( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::UNICODE_STRING ))
	{
		ActiveValue->BinaryString->Copy( SubTagTableEntry->InstantiatorCopyOfActiveValue.BinaryString );
	}

	else if(( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::NUMERIC ) ||
							( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::NUMERIC_LIST ))
	{
		ActiveValue->i = *(SubTagTableEntry->InstantiatorCopyOfActiveValue.i);
	}

	else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::FORMAT_STRING )
	{
		HSTRING		Temp(100, true);
		bool		EntriesWereDeleted;
		ActiveValue->DataFormat->EraseAll(&EntriesWereDeleted);
		SubTagTableEntry->InstantiatorCopyOfActiveValue.DataFormat->BuildReportString(&Temp, true, true);
		ActiveValue->DataFormat->ProcessCommand((char *) Temp.Char, true);
	}

	else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::PREFIX_SUFFIX_STRING )
	{
		HSTRING		Temp(100, true);
		bool		EntriesWereDeleted;
		ActiveValue->PrefixSuffix->EraseAll(&EntriesWereDeleted);
		SubTagTableEntry->InstantiatorCopyOfActiveValue.PrefixSuffix->BuildReportString(&Temp, true, true);
		ActiveValue->PrefixSuffix->ProcessCommand((char *) Temp.Char, true);
	}

// Copy the flags.
	ActiveValue->Updated = SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated;
	ActiveValue->ModifiedSinceInstantiation = SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation;

	return;
} // MoveInstantiatorCopyOfActiveValueToCurrentWorkgroupInActiveValueTable





//
// MoveValueFromCurrentWorkgroupInActiveValueTableToInstantiatorCopyOfActiveValue
//      Resynchronize current workgroup in the active value table to the instantiator copy of active value for the item sent by the caller.
//
void READER_CONFIGURATION_STORAGE::MoveValueFromCurrentWorkgroupInActiveValueTableToInstantiatorCopyOfActiveValue
					( SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntry )
{
	SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*ActiveValue = ActiveValueForCurrentWorkgroup( SubTagTableEntry );

// Update the next sub-tag table item.
	if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BOOLEAN )
	{
		*(SubTagTableEntry->InstantiatorCopyOfActiveValue.b) = ActiveValue->b;
	}

	else if(( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING ) ||
					( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING_LIST ))
	{
		strcpy( SubTagTableEntry->InstantiatorCopyOfActiveValue.c, ActiveValue->c );
	}

	else if(( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BINARY_STRING ) ||
				( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::ASCII_STRING ) ||
				( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::HEX_STRING ) ||
				( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::UNICODE_STRING ))
	{
		SubTagTableEntry->InstantiatorCopyOfActiveValue.BinaryString->Copy( ActiveValue->BinaryString );
	}

	else if(( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::NUMERIC ) ||
					( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::NUMERIC_LIST ))
	{
		*(SubTagTableEntry->InstantiatorCopyOfActiveValue.i) = ActiveValue->i;
	}

	else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::FORMAT_STRING )
	{
		HSTRING		Temp(100, true);
		bool		EntriesWereDeleted;
		SubTagTableEntry->InstantiatorCopyOfActiveValue.DataFormat->EraseAll(&EntriesWereDeleted);
		ActiveValue->DataFormat->BuildReportString(&Temp, true, true);
		SubTagTableEntry->InstantiatorCopyOfActiveValue.DataFormat->ProcessCommand((char *) Temp.Char, true);
	}

	else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::PREFIX_SUFFIX_STRING )
	{
		HSTRING		Temp(100, true);
		bool		EntriesWereDeleted;
		SubTagTableEntry->InstantiatorCopyOfActiveValue.PrefixSuffix->EraseAll(&EntriesWereDeleted);
		ActiveValue->PrefixSuffix->BuildReportString(&Temp, true, true);
		SubTagTableEntry->InstantiatorCopyOfActiveValue.PrefixSuffix->ProcessCommand((char *) Temp.Char, true);
	}

// Copy the flags.
	SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated = ActiveValue->Updated;
	SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = ActiveValue->ModifiedSinceInstantiation;

	return;
} // MoveValueFromCurrentWorkgroupInActiveValueTableToInstantiatorCopyOfActiveValue





//
// ChangeToWorkgroup
//      Validate the workgroup number sent by the caller and switch to that workgroup if valid.  In the process of switching we save the instantiator
//		copy of all active settings to our local copy for that workgroup, then we move our local copy for the new workgroup to the instantiator copy.
//
bool READER_CONFIGURATION_STORAGE::ChangeToWorkgroup( int NewWorkgroupNumber, bool UpdateActiveWorkgroupNumber )
{
	bool		Status = false;			// Haven't succeeded yet.


	if(( NewWorkgroupNumber < 0 ) || ( NewWorkgroupNumber >= NumberOfWorkgroups ))
	{
	// Caller requested bad table; WOOF!
		Status = false;
	}

	else if( NewWorkgroupNumber == CurrentWorkgroupNumber )
	{
	// No need to do table swap, because we aren't changing tables.
		Status = true;
	}

	else
	{
		Status = true;

	// Move the instantiator copy of active settings to our local copy for the current workgroup, then change to the new workgroup and move our local
	// copy of active settings for the new workgroup to the instantiator copy.
		unsigned int		TagTableEntry;								// Entry table index.
		unsigned int		SubTagTableEntry;							// Entry table index.


	// Save instantiator copy of active settings to the current table.
	// Process all entries in the tag table.
		for(TagTableEntry = 0;   TagTableEntry < TagTable->NumberOfEntries;    ++TagTableEntry)
		{
		// Process all entries in the sub-tag table.
			for(SubTagTableEntry = 0;   SubTagTableEntry < TagTable->Entries[TagTableEntry].SubTagTable->NumberOfEntries;
																				++SubTagTableEntry)
			{
				SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY			*CurrentEntry = &TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry];
				if(!(CurrentEntry->TypeFlags & SUB_TAG_TABLE::WORKTABLE_SELECTION ||CurrentEntry->TypeFlags & SUB_TAG_TABLE::CABLE_SETTING))
					MoveInstantiatorCopyOfActiveValueToCurrentWorkgroupInActiveValueTable( CurrentEntry );
			}
		}



	// Change to the new workgroup.
		CurrentWorkgroupNumber = NewWorkgroupNumber;
		if( UpdateActiveWorkgroupNumber )
		{
			ActiveWorkgroupNumber = NewWorkgroupNumber;
		}


	// Load instantiator copy of active settings from the new table.
	// Process all entries in the tag table.
		for(TagTableEntry = 0;   TagTableEntry < TagTable->NumberOfEntries;    ++TagTableEntry)
		{
		// Process all entries in the sub-tag table.
			for(SubTagTableEntry = 0;   SubTagTableEntry < TagTable->Entries[TagTableEntry].SubTagTable->NumberOfEntries;
																				++SubTagTableEntry)
			{
				SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY			*CurrentEntry = &TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry];
			if(!(CurrentEntry->TypeFlags & SUB_TAG_TABLE::WORKTABLE_SELECTION ||CurrentEntry->TypeFlags & SUB_TAG_TABLE::CABLE_SETTING))
				MoveValueFromCurrentWorkgroupInActiveValueTableToInstantiatorCopyOfActiveValue( CurrentEntry );
			}
		}
	}

	return Status;
} // ChangeToWorkgroup

bool READER_CONFIGURATION_STORAGE::ChangeToWorktable(int NewWorkcableNumber)
{
	bool Status=false;
	int NewWorkTableNumber;
	if(!((NewWorkcableNumber==0)||(NewWorkcableNumber==1)||(NewWorkcableNumber==2)||(NewWorkcableNumber==3)||(NewWorkcableNumber==9999)))
	{    
	//Caller bad cable; WOOF!
		Status=false;
	}
	else
	{
		Status=true;
		SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY		*SubTagTableEntryForThisCommand;
		switch(NewWorkcableNumber){
			case 0:
					if(GetSubTagTableEntry("060302",&SubTagTableEntryForThisCommand))
					{
						NewWorkTableNumber=SubTagTableEntryForThisCommand->WorkingValue[CurrentWorkgroupNumber].i;
						CurrentTableNumber=NewWorkTableNumber;
					}
					break;
			case 1:
					if(GetSubTagTableEntry("060303",&SubTagTableEntryForThisCommand))
					{	
						NewWorkTableNumber=SubTagTableEntryForThisCommand->WorkingValue[CurrentWorkgroupNumber].i;
						CurrentTableNumber=NewWorkTableNumber;
					}
					break;
			case 2:
					if(GetSubTagTableEntry("060301",&SubTagTableEntryForThisCommand))
					{	
						NewWorkTableNumber=SubTagTableEntryForThisCommand->WorkingValue[CurrentWorkgroupNumber].i;
						CurrentTableNumber=NewWorkTableNumber;
					}
					break;
			case 3:
					if(GetSubTagTableEntry("060304",&SubTagTableEntryForThisCommand))
					{	
						NewWorkTableNumber=SubTagTableEntryForThisCommand->WorkingValue[CurrentWorkgroupNumber].i;
						CurrentTableNumber=NewWorkTableNumber;
					}
					break;
			case 9999:
					TwoTableUpdateSimultaneously=true;
					break;
			default:
					break;
			}	
	}
	return Status;
}

bool READER_CONFIGURATION_STORAGE::ScannedMenuChangeToWorktable(int NewWorkcableNumber)
{
	bool Status=false;
	int NewWorkTableNumber;
	if(!((NewWorkcableNumber==0)||(NewWorkcableNumber==1)||(NewWorkcableNumber==2)||(NewWorkcableNumber==3)||(NewWorkcableNumber==9999)))
	{    
	//Caller bad cable; WOOF!
		Status=false;
	}
	else
	{
		Status=true;
		SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY		*SubTagTableEntryForThisCommand;
		switch(NewWorkcableNumber){
			case 0:
					if(GetSubTagTableEntry("060302",&SubTagTableEntryForThisCommand))
					{
						NewWorkTableNumber=SubTagTableEntryForThisCommand->WorkingValue[CurrentWorkgroupNumber].i;
						CurrentTableForScannedCommand=NewWorkTableNumber;
					}
					break;
			case 1:
					if(GetSubTagTableEntry("060303",&SubTagTableEntryForThisCommand))
					{	
						NewWorkTableNumber=SubTagTableEntryForThisCommand->WorkingValue[CurrentWorkgroupNumber].i;
						CurrentTableForScannedCommand=NewWorkTableNumber;
					}
					break;
			case 2:
					if(GetSubTagTableEntry("060301",&SubTagTableEntryForThisCommand))
					{	
						NewWorkTableNumber=SubTagTableEntryForThisCommand->WorkingValue[CurrentWorkgroupNumber].i;
						CurrentTableForScannedCommand=NewWorkTableNumber;
					}
					break;
			case 3:
					if(GetSubTagTableEntry("060304",&SubTagTableEntryForThisCommand))
					{	
						NewWorkTableNumber=SubTagTableEntryForThisCommand->WorkingValue[CurrentWorkgroupNumber].i;
						CurrentTableForScannedCommand=NewWorkTableNumber;
					}
					break;
			case 9999:
					TableSettingUpdateSimultaneouslyForScannedCommand=true;
					break;
			default:
					break;
			}	
	}
	return Status;
}



//
//	ComputeMaximumNumberOfDigits
//		Compute maximum number of digits allowed based on the maximum number.
//
int READER_CONFIGURATION_STORAGE::ComputeMaximumNumberOfDigits(int MaximumNumber, bool NumberIsHexadecimal)
{
	int			NextDigit;
	int			MaximumNumberOfDigits;


	if(NumberIsHexadecimal)
	{
		MaximumNumberOfDigits = 1;
		NextDigit = 0x10;
		while(( MaximumNumberOfDigits < 8 ) && ( MaximumNumber > ( NextDigit - 1 )))
		{
			++MaximumNumberOfDigits;
			NextDigit *= 0x10;
		}
	}

	else
	{
		MaximumNumberOfDigits = 1;
		NextDigit = 10;
		while(( MaximumNumberOfDigits < 10 ) && ( MaximumNumber > ( NextDigit - 1 )))
		{
			++MaximumNumberOfDigits;
			NextDigit *= 10;
		}
	}


	return(MaximumNumberOfDigits);
} // ComputeMaximumNumberOfDigits






//
//	CopySettingValue
//		Copy the input setting value to the output setting value.
//
void READER_CONFIGURATION_STORAGE::CopySettingValue( SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY *OutputSetting,
					SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY *InputSetting, unsigned int TypeFlags )
{
// Copy the sub-tag table item default value to the working value.
	if(TypeFlags & SUB_TAG_TABLE::BOOLEAN)
	{
		OutputSetting->b = InputSetting->b;
	}

	else if((TypeFlags & SUB_TAG_TABLE::STRING) ||
				(TypeFlags & SUB_TAG_TABLE::STRING_LIST))
	{
		strcpy( OutputSetting->c, InputSetting->c);
	}

	else if((TypeFlags & SUB_TAG_TABLE::BINARY_STRING) ||
			(TypeFlags & SUB_TAG_TABLE::ASCII_STRING ) ||
			(TypeFlags & SUB_TAG_TABLE::HEX_STRING) ||
			(TypeFlags & SUB_TAG_TABLE::UNICODE_STRING))
	{
		OutputSetting->BinaryString->Copy(InputSetting->BinaryString);
	}

	else if((TypeFlags & SUB_TAG_TABLE::NUMERIC) ||
				(TypeFlags & SUB_TAG_TABLE::NUMERIC_LIST) ||
				(TypeFlags & SUB_TAG_TABLE::WORKGROUP_SELECTION))
	{
		OutputSetting->i = InputSetting->i;
	}

	else if(TypeFlags & SUB_TAG_TABLE::FORMAT_STRING)
	{
		HSTRING		Temp(100, true);
		bool		EntriesWereDeleted;
		OutputSetting->DataFormat->EraseAll(&EntriesWereDeleted);

		InputSetting->DataFormat->BuildReportString(&Temp, true, true);

		OutputSetting->DataFormat->ProcessCommand((char *) Temp.Char, true);
	}

	else if(TypeFlags & SUB_TAG_TABLE::PREFIX_SUFFIX_STRING)
	{
		HSTRING		Temp(100, true);
		bool		EntriesWereDeleted;
		OutputSetting->PrefixSuffix->EraseAll(&EntriesWereDeleted);

		InputSetting->PrefixSuffix->BuildReportString(&Temp, true, true);

		OutputSetting->PrefixSuffix->ProcessCommand((char *) Temp.Char, true);
	}
} // CopySettingValue






// Copy standard default value to default value.
void READER_CONFIGURATION_STORAGE::CopyStandardDefaultToDefault(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToCopy)
{
// Copy the sub-tag table item standard default value to the default.
	if(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::BOOLEAN)
	{
		SubTagTableEntryToCopy->DefaultValue.b = SubTagTableEntryToCopy->StandardDefaultValue.b;
	}

	else if((SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::STRING) ||
				(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::STRING_LIST))
	{
		strcpy(SubTagTableEntryToCopy->DefaultValue.c, SubTagTableEntryToCopy->StandardDefaultValue.c);
	}

	else if((SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::BINARY_STRING) ||
			(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::ASCII_STRING ) ||
			(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::HEX_STRING) ||
			(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::UNICODE_STRING))
	{
		SubTagTableEntryToCopy->DefaultValue.BinaryString->Copy(SubTagTableEntryToCopy->StandardDefaultValue.BinaryString);
	}

	else if((SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::NUMERIC) ||
				(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::NUMERIC_LIST) ||
				(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SELECTION))
	{
		SubTagTableEntryToCopy->DefaultValue.i = SubTagTableEntryToCopy->StandardDefaultValue.i;
	}

	else if(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::FORMAT_STRING)
	{
		HSTRING		Temp(100, true);
		bool		EntriesWereDeleted;
		SubTagTableEntryToCopy->DefaultValue.DataFormat->EraseAll(&EntriesWereDeleted);
		SubTagTableEntryToCopy->StandardDefaultValue.DataFormat->BuildReportString(&Temp, true, true);
		SubTagTableEntryToCopy->DefaultValue.DataFormat->ProcessCommand((char *) Temp.Char, true);
	}

	else if(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::PREFIX_SUFFIX_STRING)
	{
		HSTRING		Temp(100, true);
		bool		EntriesWereDeleted;
		SubTagTableEntryToCopy->DefaultValue.PrefixSuffix->EraseAll(&EntriesWereDeleted);
		SubTagTableEntryToCopy->StandardDefaultValue.PrefixSuffix->BuildReportString(&Temp, true, true);
		SubTagTableEntryToCopy->DefaultValue.PrefixSuffix->ProcessCommand((char *) Temp.Char, true);
	}

	SubTagTableEntryToCopy->DefaultValue.ModifiedSinceInstantiation = false;
	SubTagTableEntryToCopy->DefaultValue.Updated = true;
	OneOrMoreDefaultsUpdated = true;

	return;
}



// Copy default value to working value.
void READER_CONFIGURATION_STORAGE::CopyDefaultToWorking(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToCopy,
														const char AffectedTable)
{
	if(( OverrideDefaultingDisabled ) || ( !( SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::DEFAULTING_DISABLED )))
	{
		SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*WorkingValue = WorkingValueForCurrentWorkgroup( SubTagTableEntryToCopy );

	// Copy the sub-tag table item default value to the working value.
		if(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::BOOLEAN)
		{
		// Point to the correct table.
			if(AffectedTable == '&')
			{
				WorkingValue->b = SubTagTableEntryToCopy->StandardDefaultValue.b;
			}
			else
			{
				WorkingValue->b = SubTagTableEntryToCopy->DefaultValue.b;
			}
		}

		else if((SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::STRING) ||
				(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::STRING_LIST))
		{
			if(AffectedTable == '&')
			{
				strcpy(WorkingValue->c, SubTagTableEntryToCopy->StandardDefaultValue.c);
			}
			else
			{
				strcpy(WorkingValue->c, SubTagTableEntryToCopy->DefaultValue.c);
			}
		}

		else if((SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::BINARY_STRING) ||
				(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::ASCII_STRING ) ||
				(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::HEX_STRING) ||
				(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::UNICODE_STRING))
		{
			if(AffectedTable == '&')
			{
				WorkingValue->BinaryString->Copy(SubTagTableEntryToCopy->StandardDefaultValue.BinaryString);
			}
			else
			{
				WorkingValue->BinaryString->Copy(SubTagTableEntryToCopy->DefaultValue.BinaryString);
			}
		}

		else if((SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::NUMERIC) ||
					(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::NUMERIC_LIST) ||
					(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SELECTION))
		{
			if(AffectedTable == '&')
			{
				WorkingValue->i = SubTagTableEntryToCopy->StandardDefaultValue.i;
			}
			else
			{
				WorkingValue->i = SubTagTableEntryToCopy->DefaultValue.i;
			}
		}

		else if(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::FORMAT_STRING)
		{
			HSTRING		Temp(100, true);
			bool		EntriesWereDeleted;
			WorkingValue->DataFormat->EraseAll(&EntriesWereDeleted);

			if(AffectedTable == '&')
			{
				SubTagTableEntryToCopy->StandardDefaultValue.DataFormat->BuildReportString(&Temp, true, true);
			}
			else
			{
				SubTagTableEntryToCopy->DefaultValue.DataFormat->BuildReportString(&Temp, true, true);
			}

			WorkingValue->DataFormat->ProcessCommand((char *) Temp.Char, true);
		}

		else if(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::PREFIX_SUFFIX_STRING)
		{
			HSTRING		Temp(100, true);
			bool		EntriesWereDeleted;
			WorkingValue->PrefixSuffix->EraseAll(&EntriesWereDeleted);

			if(AffectedTable == '&')
			{
				SubTagTableEntryToCopy->StandardDefaultValue.PrefixSuffix->BuildReportString(&Temp, true, true);
			}
			else
			{
				SubTagTableEntryToCopy->DefaultValue.PrefixSuffix->BuildReportString(&Temp, true, true);
			}

			WorkingValue->PrefixSuffix->ProcessCommand((char *) Temp.Char, true);
		}

		WorkingValue->Updated = true;
		WorkingValue->ModifiedSinceInstantiation = true;
		OneOrMoreWorkingValuesUpdated = true;

		if( SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
		{
			OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated = true;
			OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
		}
		else if( SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
		{
			OneOrMoreBaseOnlyValuesInWorkingStorageWereUpdated = true;
		}
		else if( SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
		{
			OneOrMoreScannerOnlyValuesInWorkingStorageWereUpdated = true;
		}
		else
		{
			OneOrMoreCommonValuesInWorkingStorageWereUpdated = true;
		}
	}

	return;
}






// Copy working value to default value.
void READER_CONFIGURATION_STORAGE::CopyWorkingToDefault(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToCopy)
{
	SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*WorkingValue = WorkingValueForCurrentWorkgroup( SubTagTableEntryToCopy );

// Copy the sub-tag table item working value to the default value.
	if(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::BOOLEAN)
	{
		SubTagTableEntryToCopy->DefaultValue.b = WorkingValue->b;
	}

	else if((SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::STRING) ||
					(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::STRING_LIST))
	{
		strcpy(SubTagTableEntryToCopy->DefaultValue.c, WorkingValue->c);
	}

	else if((SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::BINARY_STRING) ||
			(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::ASCII_STRING ) ||
			(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::HEX_STRING) ||
			(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::UNICODE_STRING))
	{
		SubTagTableEntryToCopy->DefaultValue.BinaryString->Copy(WorkingValue->BinaryString);
	}

	else if((SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::NUMERIC) ||
				(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::NUMERIC_LIST) ||
				(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SELECTION))
	{
		SubTagTableEntryToCopy->DefaultValue.i = WorkingValue->i;
	}

	else if(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::FORMAT_STRING)
	{
		HSTRING		Temp(100, true);
		bool		EntriesWereDeleted;
		SubTagTableEntryToCopy->DefaultValue.DataFormat->EraseAll(&EntriesWereDeleted);
		WorkingValue->DataFormat->BuildReportString(&Temp, true, true);
		SubTagTableEntryToCopy->DefaultValue.DataFormat->ProcessCommand((char *) Temp.Char, true);
	}

	else if(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::PREFIX_SUFFIX_STRING)
	{
		HSTRING		Temp(100, true);
		bool		EntriesWereDeleted;
		SubTagTableEntryToCopy->DefaultValue.PrefixSuffix->EraseAll(&EntriesWereDeleted);
		WorkingValue->PrefixSuffix->BuildReportString(&Temp, true, true);
		SubTagTableEntryToCopy->DefaultValue.PrefixSuffix->ProcessCommand((char *) Temp.Char, true);
	}

	SubTagTableEntryToCopy->DefaultValue.Updated = true;
	SubTagTableEntryToCopy->DefaultValue.ModifiedSinceInstantiation = true;
	OneOrMoreDefaultsUpdated = true;

	return;
}






// Copy active value to working value.
void READER_CONFIGURATION_STORAGE::CopyActiveToWorking(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToCopy)
{
	SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*WorkingValue = WorkingValueForCurrentWorkgroup( SubTagTableEntryToCopy );

// Copy the sub-tag table item active value to the working value.
	if(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::BOOLEAN)
	{
		WorkingValue->b = *(SubTagTableEntryToCopy->InstantiatorCopyOfActiveValue.b);
	}

	else if((SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::STRING) ||
					(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::STRING_LIST))
	{
		strcpy(WorkingValue->c, SubTagTableEntryToCopy->InstantiatorCopyOfActiveValue.c);
	}

	else if((SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::BINARY_STRING) ||
			(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::ASCII_STRING ) ||
			(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::HEX_STRING) ||
			(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::UNICODE_STRING))
	{
		WorkingValue->BinaryString->Copy(SubTagTableEntryToCopy->InstantiatorCopyOfActiveValue.BinaryString);
	}

	else if((SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::NUMERIC) ||
				(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::NUMERIC_LIST) ||
				(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SELECTION))
	{
		WorkingValue->i = *(SubTagTableEntryToCopy->InstantiatorCopyOfActiveValue.i);
	}

	else if(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::FORMAT_STRING)
	{
		HSTRING		Temp(100, true);
		bool		EntriesWereDeleted;
		WorkingValue->DataFormat->EraseAll(&EntriesWereDeleted);
		SubTagTableEntryToCopy->InstantiatorCopyOfActiveValue.DataFormat->BuildReportString(&Temp, true, true);
		WorkingValue->DataFormat->ProcessCommand((char *) Temp.Char, true);
	}

	else if(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::PREFIX_SUFFIX_STRING)
	{
		HSTRING		Temp(100, true);
		bool		EntriesWereDeleted;
		WorkingValue->PrefixSuffix->EraseAll(&EntriesWereDeleted);
		SubTagTableEntryToCopy->InstantiatorCopyOfActiveValue.PrefixSuffix->BuildReportString(&Temp, true, true);
		WorkingValue->PrefixSuffix->ProcessCommand((char *) Temp.Char, true);
	}

	WorkingValue->Updated = true;
	WorkingValue->ModifiedSinceInstantiation = true;
	OneOrMoreWorkingValuesUpdated = true;

	if( SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
	{
		OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated = true;
		OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
	}
	else if( SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
	{
		OneOrMoreBaseOnlyValuesInWorkingStorageWereUpdated = true;
	}
	else if( SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
	{
		OneOrMoreScannerOnlyValuesInWorkingStorageWereUpdated = true;
	}
	else
	{
		OneOrMoreCommonValuesInWorkingStorageWereUpdated = true;
	}

	return;
}






// Copy working value to active value.
void READER_CONFIGURATION_STORAGE::CopyWorkingToActive(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToCopy)
{
	SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*WorkingValue = WorkingValueForCurrentWorkgroup( SubTagTableEntryToCopy );
	SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*ActiveValue = ActiveValueForCurrentWorkgroup( SubTagTableEntryToCopy );

// Copy the sub-tag table item working value to the active value.
	if(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::BOOLEAN)
	{
		ActiveValue->b = WorkingValue->b;
		*(SubTagTableEntryToCopy->InstantiatorCopyOfActiveValue.b) = WorkingValue->b;
	}

	else if((SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::STRING) ||
					(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::STRING_LIST))
	{
		strcpy(ActiveValue->c, WorkingValue->c);
		strcpy(SubTagTableEntryToCopy->InstantiatorCopyOfActiveValue.c, WorkingValue->c);
	}

	else if((SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::BINARY_STRING) ||
			(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::ASCII_STRING ) ||
			(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::HEX_STRING) ||
			(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::UNICODE_STRING))
	{
		ActiveValue->BinaryString->Copy(WorkingValue->BinaryString);
		SubTagTableEntryToCopy->InstantiatorCopyOfActiveValue.BinaryString->Copy(WorkingValue->BinaryString);
	}

	else if((SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::NUMERIC) ||
				(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::NUMERIC_LIST) ||
				(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SELECTION))
	{
		ActiveValue->i = WorkingValue->i;
		*(SubTagTableEntryToCopy->InstantiatorCopyOfActiveValue.i) = WorkingValue->i;
	}

	else if(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::FORMAT_STRING)
	{
		HSTRING		Temp(100, true);
		bool		EntriesWereDeleted;
		ActiveValue->DataFormat->EraseAll(&EntriesWereDeleted);
		SubTagTableEntryToCopy->InstantiatorCopyOfActiveValue.DataFormat->EraseAll(&EntriesWereDeleted);
		WorkingValue->DataFormat->BuildReportString(&Temp, true, true);
		ActiveValue->DataFormat->ProcessCommand((char *) Temp.Char, true);
		SubTagTableEntryToCopy->InstantiatorCopyOfActiveValue.DataFormat->ProcessCommand((char *) Temp.Char, true);
	}

	else if(SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::PREFIX_SUFFIX_STRING)
	{
		HSTRING		Temp(100, true);
		bool		EntriesWereDeleted;
		ActiveValue->PrefixSuffix->EraseAll(&EntriesWereDeleted);
		SubTagTableEntryToCopy->InstantiatorCopyOfActiveValue.PrefixSuffix->EraseAll(&EntriesWereDeleted);
		WorkingValue->PrefixSuffix->BuildReportString(&Temp, true, true);
		ActiveValue->PrefixSuffix->ProcessCommand((char *) Temp.Char, true);
		SubTagTableEntryToCopy->InstantiatorCopyOfActiveValue.PrefixSuffix->ProcessCommand((char *) Temp.Char, true);
	}

	SubTagTableEntryToCopy->InstantiatorCopyOfActiveValue.Updated = true;
	SubTagTableEntryToCopy->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
	OneOrMoreActiveValuesUpdated = true;

	if( SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
	{
		OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
		OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
	}
	else if( SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
	{
		OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
	}
	else if( SubTagTableEntryToCopy->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
	{
		OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
	}
	else
	{
		OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
	}

	return;
}






//
//	CopyStandardDefaultToDefaultWorkingAndActive
//		Copy standard default value to other tables, provided that either a) the value hasn't been modified since startup
//		or b) the overwrite flag is on.
//
bool READER_CONFIGURATION_STORAGE::CopyStandardDefaultToDefaultWorkingAndActive
			( SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SettingEntry, bool OverwriteCustomWorkingAndActiveIfModified )
{
	bool	Status = true;		// No problems yet.


// If default value hasn't been modified since instantiation, copy standard default now.
	if(( OverwriteCustomWorkingAndActiveIfModified ) || ( !SettingEntry->DefaultValue.ModifiedSinceInstantiation ))
	{
		CopyStandardDefaultToDefault( SettingEntry );
		TRACE0( "Default is being updated to new standard default\r\n" );
	}
	else
	{
		Status = false;
	}

// If working value hasn't been modified since instantiation, copy over working and active now, then replicate across
// workgroups.  Note that we only check the ModifiedSinceInstantiation flag for the current workgroup table value.
	SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*WorkingValue = WorkingValueForCurrentWorkgroup( SettingEntry );

	if(( OverwriteCustomWorkingAndActiveIfModified ) || ( !WorkingValue->ModifiedSinceInstantiation ))
	{
		CopyDefaultToWorking( SettingEntry, '&' );
		CopyWorkingToActive( SettingEntry );
		ReplicateAcrossWorkgroups( SettingEntry, '.' );
		TRACE0( "All working and active values are being updated to new standard default\r\n" );
	}
	else
	{
		Status = false;
	}


	return Status;
} // CopyStandardDefaultToDefaultWorkingAndActive


/*
//copy interger menu item 2 to integer menu item one 
bool READER_CONFIGURATION_STORAGE::UpdateNumericListWorkingValue(const char *SettingName1,const char*SettingName2)
{
	bool								Succeeded = true;			// Assume this operation works until we know otherwise.
	SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY	*SettingEntry1;
	SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY	*SettingEntry2;	

	Succeeded = GetSubTagTableEntry( SettingName1, &SettingEntry1 );

	if( Succeeded )
	{
		Succeeded = GetSubTagTableEntry( SettingName2, &SettingEntry2 );

		if(Succeeded)
		{
			int WorkgroupNumber;
			 
			for(WorkgroupNumber = 0;    WorkgroupNumber < SettingEntry1->NumberOfWorkgroups;    ++WorkgroupNumber )	//copy setting two to setting one, only works for integer menu items
			{
				if( SettingEntry1->WorkingValue[WorkgroupNumber].i != SettingEntry2->WorkingValue[WorkgroupNumber].i )
				{
				 	SettingEntry1->WorkingValue[WorkgroupNumber].i = SettingEntry2->WorkingValue[WorkgroupNumber].i;
					SettingEntry1->WorkingValue[WorkgroupNumber].Updated = true;	//termid changed, lets indicate it needs to be flashed
					*(SettingEntry1->InstantiatorCopyOfActiveValue.i) = *(SettingEntry2->InstantiatorCopyOfActiveValue.i);				
				 }

				if( SettingEntry1->ActiveValue[WorkgroupNumber].i != SettingEntry2->ActiveValue[WorkgroupNumber].i )
				{
				 	SettingEntry1->ActiveValue[WorkgroupNumber].i = SettingEntry2->ActiveValue[WorkgroupNumber].i;
					SettingEntry1->ActiveValue[WorkgroupNumber].Updated = true;	//termid changed	 
				}				
			}

			if( *(SettingEntry1->InstantiatorCopyOfActiveValue.i) != *(SettingEntry2->InstantiatorCopyOfActiveValue.i) )
			{
				*(SettingEntry1->InstantiatorCopyOfActiveValue.i) = *(SettingEntry2->InstantiatorCopyOfActiveValue.i);
				SettingEntry1->InstantiatorCopyOfActiveValue.Updated = true;	//termid changed
			}
 		}
	}
	return Succeeded;
}*/


bool READER_CONFIGURATION_STORAGE::UpdateNumericListWorkingDefaltValue(const char *SettingName,int WorkingValue)
{
	bool								Succeeded = true;			// Assume this operation works until we know otherwise.
	SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY	*SettingEntry;

	Succeeded = GetSubTagTableEntry( SettingName, &SettingEntry );
	if( Succeeded )
	{
		int WorkgroupNumber; 
		for( WorkgroupNumber = 0;    WorkgroupNumber < SettingEntry->NumberOfWorkgroups;    ++WorkgroupNumber )
		{ 
			if( SettingEntry->WorkingValue[WorkgroupNumber].i != WorkingValue)
			{
				SettingEntry->WorkingValue[WorkgroupNumber].i = WorkingValue;
				*(SettingEntry->InstantiatorCopyOfActiveValue.i) =WorkingValue;
			}
			if( SettingEntry->ActiveValue[WorkgroupNumber].i != WorkingValue)
			{
				SettingEntry->ActiveValue[WorkgroupNumber].i = WorkingValue;
			}
			if( *(SettingEntry->InstantiatorCopyOfActiveValue.i) != WorkingValue)
				*(SettingEntry->InstantiatorCopyOfActiveValue.i) = WorkingValue;
			SettingEntry->DefaultValue.i = WorkingValue;
			SettingEntry->StandardDefaultValue.i = WorkingValue;
		}
		
	}
	return Succeeded;
}

//
//	UpdateNumericRange	(overloaded version #1, where min and max are addresses).
//		Update the range of a numeric item in the caller specified sub-tag table location.
//
bool READER_CONFIGURATION_STORAGE::UpdateNumericRange(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToUpdate,
				int MinimumRange, int *MinimumRangeAddress, int MaximumRange, int *MaximumRangeAddress)
{
	bool	UpdateSucceeded = true;			// Assume this operation works until we know otherwise.


// Make sure that the entry specified by the caller is actually a numeric item.
	if(( SubTagTableEntryToUpdate->TypeFlags & SUB_TAG_TABLE::NUMERIC ) ||
					( SubTagTableEntryToUpdate->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SELECTION ))
	{
	// Update the range information for this item.
		SubTagTableEntryToUpdate->LowRange = MinimumRange;
		SubTagTableEntryToUpdate->HighRange = MaximumRange;
		SubTagTableEntryToUpdate->WorkingLowRangeAddress = MinimumRangeAddress;
		SubTagTableEntryToUpdate->WorkingHighRangeAddress = MaximumRangeAddress;
		SubTagTableEntryToUpdate->MaximumSizeOfActiveValue = ComputeMaximumNumberOfDigits(MaximumRange);

	// If the range change causes any values to be out of bounds, fix it now.  Note that we check against the absolute (low and
	// high) range rather than the working range, because the working range may reference dependency variables that have not
	// yet been initialized.  Note that the order of checking is important, because an invalid active values is loaded with
	// working, an invalid working value is loaded with default, etc.
		if(SubTagTableEntryToUpdate->StandardDefaultValue.i < MinimumRange)
			SubTagTableEntryToUpdate->StandardDefaultValue.i = MinimumRange;
		if(SubTagTableEntryToUpdate->StandardDefaultValue.i > MaximumRange)
			SubTagTableEntryToUpdate->StandardDefaultValue.i = MaximumRange;
		if(SubTagTableEntryToUpdate->DefaultValue.i < MinimumRange)
			SubTagTableEntryToUpdate->DefaultValue.i = SubTagTableEntryToUpdate->StandardDefaultValue.i;
		if(SubTagTableEntryToUpdate->DefaultValue.i > MaximumRange)
			SubTagTableEntryToUpdate->DefaultValue.i = SubTagTableEntryToUpdate->StandardDefaultValue.i;

		int		i;

		for( i = 0;    i < SubTagTableEntryToUpdate->NumberOfWorkgroups;    ++i )
		{
			if(SubTagTableEntryToUpdate->WorkingValue[ i ].i < MinimumRange)
				SubTagTableEntryToUpdate->WorkingValue[ i ].i = SubTagTableEntryToUpdate->DefaultValue.i;
			if(SubTagTableEntryToUpdate->WorkingValue[ i ].i > MaximumRange)
				SubTagTableEntryToUpdate->WorkingValue[ i ].i = SubTagTableEntryToUpdate->DefaultValue.i;
			if(SubTagTableEntryToUpdate->ActiveValue[ i ].i < MinimumRange)
				SubTagTableEntryToUpdate->ActiveValue[ i ].i = SubTagTableEntryToUpdate->WorkingValue[ i ].i;
			if(SubTagTableEntryToUpdate->ActiveValue[ i ].i > MaximumRange)
				SubTagTableEntryToUpdate->ActiveValue[ i ].i = SubTagTableEntryToUpdate->WorkingValue[ i ].i;
		}

		SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*WorkingValue = WorkingValueForCurrentWorkgroup( SubTagTableEntryToUpdate );
		if(*(SubTagTableEntryToUpdate->InstantiatorCopyOfActiveValue.i) < MinimumRange)
			*(SubTagTableEntryToUpdate->InstantiatorCopyOfActiveValue.i) = WorkingValue->i;
		if(*(SubTagTableEntryToUpdate->InstantiatorCopyOfActiveValue.i) > MaximumRange)
			*(SubTagTableEntryToUpdate->InstantiatorCopyOfActiveValue.i) = WorkingValue->i;
	}
	else
	{
		UpdateSucceeded = false;
	}

	return(UpdateSucceeded);
} // UpdateNumericRange (overloaded version #1)






//
//	UpdateNumericRange	(overloaded version #2, where min is an address and max is a constant).
//		Update the range of a numeric item in the caller specified sub-tag table location.
//
bool READER_CONFIGURATION_STORAGE::UpdateNumericRange(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToUpdate,
				int MinimumRange, int *MinimumRangeAddress, int MaximumRange)
{
	bool	UpdateSucceeded = true;			// Assume this operation works until we know otherwise.


// Make sure that the entry specified by the caller is actually a numeric item.
	if(( SubTagTableEntryToUpdate->TypeFlags & SUB_TAG_TABLE::NUMERIC ) ||
					( SubTagTableEntryToUpdate->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SELECTION ))
	{
	// Update the range information for this item.
		SubTagTableEntryToUpdate->LowRange = MinimumRange;
		SubTagTableEntryToUpdate->WorkingLowRangeAddress = MinimumRangeAddress;
		SubTagTableEntryToUpdate->HighRange = MaximumRange;
		SubTagTableEntryToUpdate->WorkingHighRangeAddress = &(SubTagTableEntryToUpdate->HighRange);
		SubTagTableEntryToUpdate->MaximumSizeOfActiveValue = ComputeMaximumNumberOfDigits(MaximumRange);

	// If the range change causes any values to be out of bounds, fix it now.  Note that we check against the absolute (low and
	// high) range rather than the working range, because the working range may reference dependency variables that have not
	// yet been initialized.  Note that the order of checking is important, because an invalid active values is loaded with
	// working, an invalid working value is loaded with default, etc.
		if(SubTagTableEntryToUpdate->StandardDefaultValue.i < MinimumRange)
			SubTagTableEntryToUpdate->StandardDefaultValue.i = MinimumRange;
		if(SubTagTableEntryToUpdate->StandardDefaultValue.i > MaximumRange)
			SubTagTableEntryToUpdate->StandardDefaultValue.i = MaximumRange;
		if(SubTagTableEntryToUpdate->DefaultValue.i < MinimumRange)
			SubTagTableEntryToUpdate->DefaultValue.i = SubTagTableEntryToUpdate->StandardDefaultValue.i;
		if(SubTagTableEntryToUpdate->DefaultValue.i > MaximumRange)
			SubTagTableEntryToUpdate->DefaultValue.i = SubTagTableEntryToUpdate->StandardDefaultValue.i;

		int		i;


		for( i = 0;    i < SubTagTableEntryToUpdate->NumberOfWorkgroups;    ++i )
		{
			if(SubTagTableEntryToUpdate->WorkingValue[ i ].i < MinimumRange)
				SubTagTableEntryToUpdate->WorkingValue[ i ].i = SubTagTableEntryToUpdate->DefaultValue.i;
			if(SubTagTableEntryToUpdate->WorkingValue[ i ].i > MaximumRange)
				SubTagTableEntryToUpdate->WorkingValue[ i ].i = SubTagTableEntryToUpdate->DefaultValue.i;
			if(SubTagTableEntryToUpdate->ActiveValue[ i ].i < MinimumRange)
				SubTagTableEntryToUpdate->ActiveValue[ i ].i = SubTagTableEntryToUpdate->WorkingValue[ i ].i;
			if(SubTagTableEntryToUpdate->ActiveValue[ i ].i > MaximumRange)
				SubTagTableEntryToUpdate->ActiveValue[ i ].i = SubTagTableEntryToUpdate->WorkingValue[ i ].i;
		}

		SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*WorkingValue = WorkingValueForCurrentWorkgroup( SubTagTableEntryToUpdate );
		if(*(SubTagTableEntryToUpdate->InstantiatorCopyOfActiveValue.i) < MinimumRange)
			*(SubTagTableEntryToUpdate->InstantiatorCopyOfActiveValue.i) = WorkingValue->i;
		if(*(SubTagTableEntryToUpdate->InstantiatorCopyOfActiveValue.i) > MaximumRange)
			*(SubTagTableEntryToUpdate->InstantiatorCopyOfActiveValue.i) = WorkingValue->i;
	}
	else
	{
		UpdateSucceeded = false;
	}

	return(UpdateSucceeded);
} // UpdateNumericRange (overloaded version #2)






//
//	UpdateNumericRange	(overloaded version #3, where min is a constant and max is an address).
//		Update the range of a numeric item in the caller specified sub-tag table location.
//
bool READER_CONFIGURATION_STORAGE::UpdateNumericRange(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToUpdate,
				int MinimumRange, int MaximumRange, int *MaximumRangeAddress)
{
	bool	UpdateSucceeded = true;			// Assume this operation works until we know otherwise.


// Make sure that the entry specified by the caller is actually a numeric item.
	if(( SubTagTableEntryToUpdate->TypeFlags & SUB_TAG_TABLE::NUMERIC ) ||
					( SubTagTableEntryToUpdate->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SELECTION ))
	{
	// Update the range information for this item.
		SubTagTableEntryToUpdate->LowRange = MinimumRange;
		SubTagTableEntryToUpdate->WorkingLowRangeAddress = &(SubTagTableEntryToUpdate->LowRange);
		SubTagTableEntryToUpdate->HighRange = MaximumRange;
		SubTagTableEntryToUpdate->WorkingHighRangeAddress = MaximumRangeAddress;
		SubTagTableEntryToUpdate->MaximumSizeOfActiveValue = ComputeMaximumNumberOfDigits(MaximumRange);

	// If the range change causes any values to be out of bounds, fix it now.  Note that we check against the absolute (low and
	// high) range rather than the working range, because the working range may reference dependency variables that have not
	// yet been initialized.  Note that the order of checking is important, because an invalid active values is loaded with
	// working, an invalid working value is loaded with default, etc.
		if(SubTagTableEntryToUpdate->StandardDefaultValue.i < MinimumRange)
			SubTagTableEntryToUpdate->StandardDefaultValue.i = MinimumRange;
		if(SubTagTableEntryToUpdate->StandardDefaultValue.i > MaximumRange)
			SubTagTableEntryToUpdate->StandardDefaultValue.i = MaximumRange;
		if(SubTagTableEntryToUpdate->DefaultValue.i < MinimumRange)
			SubTagTableEntryToUpdate->DefaultValue.i = SubTagTableEntryToUpdate->StandardDefaultValue.i;
		if(SubTagTableEntryToUpdate->DefaultValue.i > MaximumRange)
			SubTagTableEntryToUpdate->DefaultValue.i = SubTagTableEntryToUpdate->StandardDefaultValue.i;

		int		i;

		for( i = 0;    i < SubTagTableEntryToUpdate->NumberOfWorkgroups;    ++i )
		{
			if(SubTagTableEntryToUpdate->WorkingValue[ i ].i < MinimumRange)
				SubTagTableEntryToUpdate->WorkingValue[ i ].i = SubTagTableEntryToUpdate->DefaultValue.i;
			if(SubTagTableEntryToUpdate->WorkingValue[ i ].i > MaximumRange)
				SubTagTableEntryToUpdate->WorkingValue[ i ].i = SubTagTableEntryToUpdate->DefaultValue.i;
			if(SubTagTableEntryToUpdate->ActiveValue[ i ].i < MinimumRange)
				SubTagTableEntryToUpdate->ActiveValue[ i ].i = SubTagTableEntryToUpdate->WorkingValue[ i ].i;
			if(SubTagTableEntryToUpdate->ActiveValue[ i ].i > MaximumRange)
				SubTagTableEntryToUpdate->ActiveValue[ i ].i = SubTagTableEntryToUpdate->WorkingValue[ i ].i;
		}

		SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*WorkingValue = WorkingValueForCurrentWorkgroup( SubTagTableEntryToUpdate );
		if(*(SubTagTableEntryToUpdate->InstantiatorCopyOfActiveValue.i) < MinimumRange)
			*(SubTagTableEntryToUpdate->InstantiatorCopyOfActiveValue.i) = WorkingValue->i;
		if(*(SubTagTableEntryToUpdate->InstantiatorCopyOfActiveValue.i) > MaximumRange)
			*(SubTagTableEntryToUpdate->InstantiatorCopyOfActiveValue.i) = WorkingValue->i;
	}
	else
	{
		UpdateSucceeded = false;
	}

	return(UpdateSucceeded);
} // UpdateNumericRange (overloaded version #3)






//
//	UpdateNumericRange	(overloaded version #4, where min and max are constants).
//		Update the range of a numeric item in the caller specified sub-tag table location.
//
bool READER_CONFIGURATION_STORAGE::UpdateNumericRange(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToUpdate,
				int MinimumRange, int MaximumRange)
{
	bool	UpdateSucceeded = true;			// Assume this operation works until we know otherwise.


// Make sure that the entry specified by the caller is actually a numeric item.
	if(( SubTagTableEntryToUpdate->TypeFlags & SUB_TAG_TABLE::NUMERIC ) ||
					( SubTagTableEntryToUpdate->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SELECTION ))
	{
	// Update the range information for this item.
		SubTagTableEntryToUpdate->LowRange = MinimumRange;
		SubTagTableEntryToUpdate->HighRange = MaximumRange;
		SubTagTableEntryToUpdate->WorkingLowRangeAddress = &(SubTagTableEntryToUpdate->LowRange);
		SubTagTableEntryToUpdate->WorkingHighRangeAddress = &(SubTagTableEntryToUpdate->HighRange);
		SubTagTableEntryToUpdate->MaximumSizeOfActiveValue = ComputeMaximumNumberOfDigits(MaximumRange);

	// If the range change causes any values to be out of bounds, fix it now.  Note that we check against the absolute (low and
	// high) range rather than the working range, because the working range may reference dependency variables that have not
	// yet been initialized.  Note that the order of checking is important, because an invalid active values is loaded with
	// working, an invalid working value is loaded with default, etc.
		if(SubTagTableEntryToUpdate->StandardDefaultValue.i < MinimumRange)
			SubTagTableEntryToUpdate->StandardDefaultValue.i = MinimumRange;
		if(SubTagTableEntryToUpdate->StandardDefaultValue.i > MaximumRange)
			SubTagTableEntryToUpdate->StandardDefaultValue.i = MaximumRange;
		if(SubTagTableEntryToUpdate->DefaultValue.i < MinimumRange)
			SubTagTableEntryToUpdate->DefaultValue.i = SubTagTableEntryToUpdate->StandardDefaultValue.i;
		if(SubTagTableEntryToUpdate->DefaultValue.i > MaximumRange)
			SubTagTableEntryToUpdate->DefaultValue.i = SubTagTableEntryToUpdate->StandardDefaultValue.i;

		int		i;

		for( i = 0;    i < SubTagTableEntryToUpdate->NumberOfWorkgroups;    ++i )
		{
			if(SubTagTableEntryToUpdate->WorkingValue[ i ].i < MinimumRange)
				SubTagTableEntryToUpdate->WorkingValue[ i ].i = SubTagTableEntryToUpdate->DefaultValue.i;
			if(SubTagTableEntryToUpdate->WorkingValue[ i ].i > MaximumRange)
				SubTagTableEntryToUpdate->WorkingValue[ i ].i = SubTagTableEntryToUpdate->DefaultValue.i;
			if(SubTagTableEntryToUpdate->ActiveValue[ i ].i < MinimumRange)
				SubTagTableEntryToUpdate->ActiveValue[ i ].i = SubTagTableEntryToUpdate->WorkingValue[ i ].i;
			if(SubTagTableEntryToUpdate->ActiveValue[ i ].i > MaximumRange)
				SubTagTableEntryToUpdate->ActiveValue[ i ].i = SubTagTableEntryToUpdate->WorkingValue[ i ].i;
		}

		SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*WorkingValue = WorkingValueForCurrentWorkgroup( SubTagTableEntryToUpdate );
		if(*(SubTagTableEntryToUpdate->InstantiatorCopyOfActiveValue.i) < MinimumRange)
			*(SubTagTableEntryToUpdate->InstantiatorCopyOfActiveValue.i) = WorkingValue->i;
		if(*(SubTagTableEntryToUpdate->InstantiatorCopyOfActiveValue.i) > MaximumRange)
			*(SubTagTableEntryToUpdate->InstantiatorCopyOfActiveValue.i) = WorkingValue->i;
	}
	else
	{
		UpdateSucceeded = false;
	}

	return(UpdateSucceeded);
} // UpdateNumericRange (overloaded version #4)







//
//	UpdateNumericList (overloaded version #1, with an array of list items)
//		Update the list for a numeric list item in the caller specified sub-tag table location
//
bool READER_CONFIGURATION_STORAGE::UpdateNumericList(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToUpdate,
		int *ItemList, int NumberOfItems, int MaximumDigits)
{
	bool	UpdateSucceeded = true;			// Assume this operation works until we know otherwise.
	int		i;


// Make sure that the entry specified by the caller is actually a numeric list item.
	if(SubTagTableEntryToUpdate->TypeFlags & SUB_TAG_TABLE::NUMERIC_LIST)
	{
	// Get rid of any existing list.
		if(SubTagTableEntryToUpdate->NumberOfEntriesInListRange > 0)
		{
			delete SubTagTableEntryToUpdate->NumericListRange;
		}

	// Make sure that the number of items is valid.
		if((NumberOfItems < 0) || (NumberOfItems > MAX_LIST_ITEMS))
		{
			UpdateSucceeded = false;
		}

		else
		{
		// Update the list information for this item.
		// Set the end of the item list.
			SubTagTableEntryToUpdate->NumberOfEntriesInListRange = NumberOfItems;

		// Create the item list and copy the entries.
			if(SubTagTableEntryToUpdate->NumberOfEntriesInListRange > 0)
			{
				SubTagTableEntryToUpdate->NumericListRange = new int[SubTagTableEntryToUpdate->NumberOfEntriesInListRange];
				for(i = 0;    i < SubTagTableEntryToUpdate->NumberOfEntriesInListRange;    ++i)
				{
					SubTagTableEntryToUpdate->NumericListRange[i] = ItemList[i];
				}

			// Sort the items in ascending order.
/*
				qsort( SubTagTableEntryToUpdate->NumericListRange, SubTagTableEntryToUpdate->NumberOfEntriesInListRange,
								sizeof( int ), ReaderConfigurationQSortCompareOfNumericListItems );
*/

			// Remove duplicate items and find the largest item.  While we're searching for duplicate items, we'll
			// also check each list item against the current standard default, default, working, and active values
			// to see if they match.  If we don't find a match, then we'll have to reset the value to a valid item.
				int		j;
				int		k;
				int		LargestItem = -2147483647;
				bool	StandardDefaultValueFound = false;
				bool	DefaultValueFound = false;
				bool	*WorkingValueFound = new bool [ SubTagTableEntryToUpdate->NumberOfWorkgroups ];
				bool	*ActiveValueFound = new bool [ SubTagTableEntryToUpdate->NumberOfWorkgroups ];
				bool	InstantiatorCopyOfActiveValueFound = false;
				int		WorkgroupNumber;

				for( WorkgroupNumber = 0;    WorkgroupNumber < SubTagTableEntryToUpdate->NumberOfWorkgroups;    ++WorkgroupNumber )
				{
					WorkingValueFound[ WorkgroupNumber ] = false;
					ActiveValueFound[ WorkgroupNumber ] = false;
				}

				i = 0;
				while(i < SubTagTableEntryToUpdate->NumberOfEntriesInListRange)
				{
				// If this is the largest item so far, note it.
					if(SubTagTableEntryToUpdate->NumericListRange[i] > LargestItem)
					{
						LargestItem = SubTagTableEntryToUpdate->NumericListRange[i];
					}

				// Check each value to see if it matches this item.
					if(SubTagTableEntryToUpdate->NumericListRange[i] == SubTagTableEntryToUpdate->StandardDefaultValue.i)
					{
						StandardDefaultValueFound = true;
					}
					if(SubTagTableEntryToUpdate->NumericListRange[i] == SubTagTableEntryToUpdate->DefaultValue.i)
					{
						DefaultValueFound = true;
					}

					for( WorkgroupNumber = 0;    WorkgroupNumber < SubTagTableEntryToUpdate->NumberOfWorkgroups;    ++WorkgroupNumber )
					{
						if(SubTagTableEntryToUpdate->NumericListRange[i] == SubTagTableEntryToUpdate->WorkingValue[ WorkgroupNumber ].i )
						{
							WorkingValueFound[ WorkgroupNumber ] = true;
						}
						if(SubTagTableEntryToUpdate->NumericListRange[i] == SubTagTableEntryToUpdate->ActiveValue[ WorkgroupNumber ].i )
						{
							ActiveValueFound[ WorkgroupNumber ] = true;
						}
					}

					if(SubTagTableEntryToUpdate->NumericListRange[i] == *(SubTagTableEntryToUpdate->InstantiatorCopyOfActiveValue.i))
					{
						InstantiatorCopyOfActiveValueFound = true;
					}

					j = i + 1;
					k = i + 1;

					while(j < SubTagTableEntryToUpdate->NumberOfEntriesInListRange)
					{
						if(SubTagTableEntryToUpdate->NumericListRange[i] != SubTagTableEntryToUpdate->NumericListRange[j])
						{
							SubTagTableEntryToUpdate->NumericListRange[k] = SubTagTableEntryToUpdate->NumericListRange[j];
							++k;
						}

						++j;
					}

					SubTagTableEntryToUpdate->NumberOfEntriesInListRange = k;
					++i;
				}



			// If the list update causes any values to be out of bounds, fix it now.  Note that the order of checking is
			// important, because an invalid active values is loaded with working, an invalid working value is loaded with
			// default, etc.
				if(!StandardDefaultValueFound)
				{
					SubTagTableEntryToUpdate->StandardDefaultValue.i = SubTagTableEntryToUpdate->NumericListRange[0];
				}
				if(!DefaultValueFound)
				{
					SubTagTableEntryToUpdate->DefaultValue.i = SubTagTableEntryToUpdate->StandardDefaultValue.i;
				}

				for( WorkgroupNumber = 0;    WorkgroupNumber < SubTagTableEntryToUpdate->NumberOfWorkgroups;    ++WorkgroupNumber )
				{
					if(!WorkingValueFound[ WorkgroupNumber ] )
					{
						SubTagTableEntryToUpdate->WorkingValue[ WorkgroupNumber ].i = SubTagTableEntryToUpdate->DefaultValue.i;
					}
					if(!ActiveValueFound[ WorkgroupNumber ] )
					{
						SubTagTableEntryToUpdate->ActiveValue[ WorkgroupNumber ].i = SubTagTableEntryToUpdate->WorkingValue[ WorkgroupNumber ].i;
					}
				}

				SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*WorkingValue = WorkingValueForCurrentWorkgroup( SubTagTableEntryToUpdate );
				if(!InstantiatorCopyOfActiveValueFound)
				{
					*(SubTagTableEntryToUpdate->InstantiatorCopyOfActiveValue.i) = WorkingValue->i;
				}


			// Update the maximum number of digits based on the largest item.
				if(MaximumDigits>(-1))
					SubTagTableEntryToUpdate->MaximumSizeOfActiveValue = MaximumDigits;
				else
					SubTagTableEntryToUpdate->MaximumSizeOfActiveValue = ComputeMaximumNumberOfDigits(LargestItem);


				delete [] WorkingValueFound;
				delete [] ActiveValueFound;
			}
		}
	}

	else
	{
		UpdateSucceeded = false;
	}

	return(UpdateSucceeded);
} // UpdateNumericList (overloaded version #1)







//
//	UpdateNumericList (overloaded version #2, with individual list items)
//		Update the list for a numeric list item in the caller specified sub-tag table location
//
bool READER_CONFIGURATION_STORAGE::UpdateNumericList(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToUpdate,
		int Item1, int Item2, int Item3, int Item4, int Item5, int Item6, int Item7, int Item8, int Item9, int Item10,
		int Item11, int Item12, int Item13, int Item14, int Item15, int Item16, int Item17, int Item18, int Item19, int Item20,
		int Item21, int Item22, int Item23, int Item24, int Item25, int Item26, int Item27, int Item28, int Item29, int Item30,
		int Item31, int Item32, int Item33, int Item34, int Item35, int Item36, int Item37, int Item38, int Item39, int Item40,
		int Item41, int Item42, int Item43, int Item44, int Item45, int Item46, int Item47, int Item48, int Item49, int Item50,
		int Item51, int Item52, int Item53, int Item54, int Item55, int Item56, int Item57, int Item58, int Item59, int Item60)
{
	int		ItemList[MAX_LIST_ITEMS];
	int		NumberOfItems;


// Create pointers to the items.
	ItemList[0] = Item1;  ItemList[1] = Item2;  ItemList[2] = Item3;  ItemList[3] = Item4;  ItemList[4] = Item5;
	ItemList[5] = Item6;  ItemList[6] = Item7;  ItemList[7] = Item8;  ItemList[8] = Item9;  ItemList[9] = Item10;
	ItemList[10] = Item11;  ItemList[11] = Item12;  ItemList[12] = Item13;  ItemList[13] = Item14;  ItemList[14] = Item15;
	ItemList[15] = Item16;  ItemList[16] = Item17;  ItemList[17] = Item18;  ItemList[18] = Item19;  ItemList[19] = Item20;
	ItemList[20] = Item21;  ItemList[21] = Item22;  ItemList[22] = Item23;  ItemList[23] = Item24;  ItemList[24] = Item25;
	ItemList[25] = Item26;  ItemList[26] = Item27;  ItemList[27] = Item28;  ItemList[28] = Item29;  ItemList[29] = Item30;
	ItemList[30] = Item31;  ItemList[31] = Item32;  ItemList[32] = Item33;  ItemList[33] = Item34;  ItemList[34] = Item35;
	ItemList[35] = Item36;  ItemList[36] = Item37;  ItemList[37] = Item38;  ItemList[38] = Item39;  ItemList[39] = Item40;
	ItemList[40] = Item41;  ItemList[41] = Item42;  ItemList[42] = Item43;  ItemList[43] = Item44;  ItemList[44] = Item45;
	ItemList[45] = Item46;  ItemList[46] = Item47;  ItemList[47] = Item48;  ItemList[48] = Item49;  ItemList[49] = Item50;
	ItemList[50] = Item51;  ItemList[51] = Item52;  ItemList[52] = Item53;  ItemList[53] = Item54;  ItemList[54] = Item55;
	ItemList[55] = Item56;  ItemList[56] = Item57;  ItemList[57] = Item58;  ItemList[58] = Item59;  ItemList[59] = Item60;

// Find the end of the item list.
	NumberOfItems = 0;
	while((NumberOfItems < MAX_LIST_ITEMS) && (NumberOfItems < 60) && (ItemList[NumberOfItems] != INVALID_LIST_ITEM))
	{
		++NumberOfItems;
	}

	return(UpdateNumericList(SubTagTableEntryToUpdate, ItemList, NumberOfItems));
} // UpdateNumericList (overloaded version #2)









//
//	UpdateStringList (overloaded version #1, with an array of list items)
//		Update the list for a string list item in the caller specified sub-tag table location
//
bool READER_CONFIGURATION_STORAGE::UpdateStringList(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToUpdate,
			const char **ItemList, int NumberOfItems )
{
	bool		UpdateSucceeded = true;			// Assume this operation works until we know otherwise.
	int			i;


// Make sure that the entry specified by the caller is actually a string list item.
	if(SubTagTableEntryToUpdate->TypeFlags & SUB_TAG_TABLE::STRING_LIST)
	{
	// Get rid of any existing list.
		if(SubTagTableEntryToUpdate->NumberOfEntriesInListRange > 0)
		{
			for(i = 0;    i < SubTagTableEntryToUpdate->NumberOfEntriesInListRange;    ++i)
			{
				delete SubTagTableEntryToUpdate->StringListRange[i];
			}

			delete SubTagTableEntryToUpdate->StringListRange;
		}

	// Make sure that the number of items is valid.
		if((NumberOfItems < 0) || (NumberOfItems > MAX_LIST_ITEMS))
		{
			UpdateSucceeded = false;
		}

	// Update the list information for this item.
		else
		{
		// Set the end of the item list.
			SubTagTableEntryToUpdate->NumberOfEntriesInListRange = NumberOfItems;

		// Now check to see that the lengths in the item list will not exceed the maximum size of the actual value.
			i = 0;
			while((i < SubTagTableEntryToUpdate->NumberOfEntriesInListRange) && (UpdateSucceeded))
			{
				if(strlen(ItemList[i]) > SubTagTableEntryToUpdate->MaximumSizeOfActiveValue)
				{
					UpdateSucceeded = false;
				}
				else
				{
					++i;
				}
			}

		// Create the item list and copy the entries if no errors have occurred and if any list items were found.  While we're
		// creating the item list, we'll also check each list item against the current standard default, default, working, and
		// active values to see if they match.  If we don't find a match, then we'll have to reset the value to a valid item.
			if((UpdateSucceeded) && (SubTagTableEntryToUpdate->NumberOfEntriesInListRange > 0))
			{
				bool	StandardDefaultValueFound = false;
				bool	DefaultValueFound = false;

				bool	*WorkingValueFound = new bool [ SubTagTableEntryToUpdate->NumberOfWorkgroups ];
				bool	*ActiveValueFound = new bool [ SubTagTableEntryToUpdate->NumberOfWorkgroups ];
				bool	InstantiatorCopyOfActiveValueFound = false;
				int		WorkgroupNumber;

				for( WorkgroupNumber = 0;    WorkgroupNumber < SubTagTableEntryToUpdate->NumberOfWorkgroups;    ++WorkgroupNumber )
				{
					WorkingValueFound[ WorkgroupNumber ] = false;
					ActiveValueFound[ WorkgroupNumber ] = false;
				}

				SubTagTableEntryToUpdate->StringListRange = new char *[SubTagTableEntryToUpdate->NumberOfEntriesInListRange];
				for(i = 0;    i < SubTagTableEntryToUpdate->NumberOfEntriesInListRange;    ++i)
				{
				// Check each value to see if it matches this item.
					if(stricmp(SubTagTableEntryToUpdate->StandardDefaultValue.c, ItemList[i]) == 0)
					{
						StandardDefaultValueFound = true;
					}
					if(stricmp(SubTagTableEntryToUpdate->DefaultValue.c, ItemList[i]) == 0)
					{
						DefaultValueFound = true;
					}

					for( WorkgroupNumber = 0;    WorkgroupNumber < SubTagTableEntryToUpdate->NumberOfWorkgroups;    ++WorkgroupNumber )
					{
						if(stricmp(SubTagTableEntryToUpdate->WorkingValue[ WorkgroupNumber ].c, ItemList[i]) == 0)
						{
							WorkingValueFound[ WorkgroupNumber ] = true;
						}
						if(stricmp(SubTagTableEntryToUpdate->ActiveValue[ WorkgroupNumber ].c, ItemList[i]) == 0)
						{
							ActiveValueFound[ WorkgroupNumber ] = true;
						}
					}

					if(stricmp(SubTagTableEntryToUpdate->InstantiatorCopyOfActiveValue.c, ItemList[i]) == 0)

					SubTagTableEntryToUpdate->StringListRange[i] = new char [strlen(ItemList[i] + 1)];
					strcpy(SubTagTableEntryToUpdate->StringListRange[i], ItemList[i]);
				}


			// If the list update causes any values to be out of bounds, fix it now.  Note that the order of checking is
			// important, because an invalid active values is loaded with working, an invalid working value is loaded with
			// default, etc.
				if(!StandardDefaultValueFound)
				{
					strcpy(SubTagTableEntryToUpdate->StandardDefaultValue.c, SubTagTableEntryToUpdate->StringListRange[0]);
				}
				if(!DefaultValueFound)
				{
					strcpy(SubTagTableEntryToUpdate->DefaultValue.c, SubTagTableEntryToUpdate->StandardDefaultValue.c);
				}

				for( WorkgroupNumber = 0;    WorkgroupNumber < SubTagTableEntryToUpdate->NumberOfWorkgroups;    ++WorkgroupNumber )
				{
					if(!WorkingValueFound[ WorkgroupNumber ] )
					{
						strcpy(SubTagTableEntryToUpdate->WorkingValue[ WorkgroupNumber ].c, SubTagTableEntryToUpdate->DefaultValue.c);
					}
					if(!ActiveValueFound[ WorkgroupNumber ] )
					{
						strcpy(SubTagTableEntryToUpdate->ActiveValue[ WorkgroupNumber ].c, SubTagTableEntryToUpdate->WorkingValue[ WorkgroupNumber ].c);
					}
				}

				SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*WorkingValue = WorkingValueForCurrentWorkgroup( SubTagTableEntryToUpdate );
				if(!InstantiatorCopyOfActiveValueFound)
				{
					strcpy(SubTagTableEntryToUpdate->InstantiatorCopyOfActiveValue.c, WorkingValue->c);
				}

				delete [] WorkingValueFound;
				delete [] ActiveValueFound;
			}
		}
	}

	else
	{
		UpdateSucceeded = false;
	}

	return(UpdateSucceeded);
} // UpdateStringList (overloaded version #1)





//
//	UpdateStringList (overloaded version #2, with individual list items)
//		Update the list for a string list item in the caller specified sub-tag table location
//
bool READER_CONFIGURATION_STORAGE::UpdateStringList(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntryToUpdate,
			const char *Item1, const char *Item2, const char *Item3, const char *Item4, const char *Item5,
			const char *Item6, const char *Item7, const char *Item8, const char *Item9, const char *Item10,
			const char *Item11, const char *Item12, const char *Item13, const char *Item14, const char *Item15,
			const char *Item16, const char *Item17, const char *Item18, const char *Item19, const char *Item20,
			const char *Item21, const char *Item22, const char *Item23, const char *Item24, const char *Item25,
			const char *Item26, const char *Item27, const char *Item28, const char *Item29, const char *Item30,
			const char *Item31, const char *Item32, const char *Item33, const char *Item34, const char *Item35,
			const char *Item36, const char *Item37, const char *Item38, const char *Item39, const char *Item40,
			const char *Item41, const char *Item42, const char *Item43, const char *Item44, const char *Item45,
			const char *Item46, const char *Item47, const char *Item48, const char *Item49, const char *Item50,
			const char *Item51, const char *Item52, const char *Item53, const char *Item54, const char *Item55,
			const char *Item56, const char *Item57, const char *Item58, const char *Item59, const char *Item60)
{
	const char	*ItemList[MAX_LIST_ITEMS];
	int			NumberOfItems;


// Create pointers to the items.
	ItemList[0] = Item1;  ItemList[1] = Item2;  ItemList[2] = Item3;  ItemList[3] = Item4;  ItemList[4] = Item5;
	ItemList[5] = Item6;  ItemList[6] = Item7;  ItemList[7] = Item8;  ItemList[8] = Item9;  ItemList[9] = Item10;
	ItemList[10] = Item11;  ItemList[11] = Item12;  ItemList[12] = Item13;  ItemList[13] = Item14;  ItemList[14] = Item15;
	ItemList[15] = Item16;  ItemList[16] = Item17;  ItemList[17] = Item18;  ItemList[18] = Item19;  ItemList[19] = Item20;
	ItemList[20] = Item21;  ItemList[21] = Item22;  ItemList[22] = Item23;  ItemList[23] = Item24;  ItemList[24] = Item25;
	ItemList[25] = Item26;  ItemList[26] = Item27;  ItemList[27] = Item28;  ItemList[28] = Item29;  ItemList[29] = Item30;
	ItemList[30] = Item31;  ItemList[31] = Item32;  ItemList[32] = Item33;  ItemList[33] = Item34;  ItemList[34] = Item35;
	ItemList[35] = Item36;  ItemList[36] = Item37;  ItemList[37] = Item38;  ItemList[38] = Item39;  ItemList[39] = Item40;
	ItemList[40] = Item41;  ItemList[41] = Item42;  ItemList[42] = Item43;  ItemList[43] = Item44;  ItemList[44] = Item45;
	ItemList[45] = Item46;  ItemList[46] = Item47;  ItemList[47] = Item48;  ItemList[48] = Item49;  ItemList[49] = Item50;
	ItemList[50] = Item51;  ItemList[51] = Item52;  ItemList[52] = Item53;  ItemList[53] = Item54;  ItemList[54] = Item55;
	ItemList[55] = Item56;  ItemList[56] = Item57;  ItemList[57] = Item58;  ItemList[58] = Item59;  ItemList[59] = Item60;

// Find the end of the item list.
	NumberOfItems = 0;
	while((NumberOfItems < MAX_LIST_ITEMS) && (NumberOfItems < 60) && (ItemList[NumberOfItems] != 0))
	{
		++NumberOfItems;
	}

	return(UpdateStringList(SubTagTableEntryToUpdate, ItemList, NumberOfItems));
} // UpdateStringList (overloaded version #2)





//
//	AddCommand
//		Add a command item to the tag and sub-tag tables, then update the index to maintain sorting order.
//
bool READER_CONFIGURATION_STORAGE::AddCommand(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
							bool (*ExitAction) (void), int SpecialCommandAction)
{
	bool	AddSucceeded = true;			// Assume this operation works until we know otherwise.
	int		NewTagTableEntry;


	NewTagTableEntry = TagTable->GetExistingTagOrCreateNewTag(SettingName);
	if(NewTagTableEntry >= 0)
	{
		int				NewSubTagTableEntry;
		SUB_TAG_TABLE	*SubTagTable = TagTable->Entries[NewTagTableEntry].SubTagTable;


		NewSubTagTableEntry = SubTagTable->AddBasicInformationForNewSubTag(&SettingName[SIZE_OF_TAG], EntryAction, ExitAction, Modifier);
		if(NewSubTagTableEntry >= 0)
		{
		// Add the remaining information for the new sub-tag.
			SubTagTable->Entries[NewSubTagTableEntry].TypeFlags = Modifier | SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::COMMAND;
			SubTagTable->Entries[NewSubTagTableEntry].SpecialCommandAction = SpecialCommandAction;
		}

		else
		{
		// Sub tag entry already exists; WOOF!
			AddSucceeded = false;
			ProcessError( NewSubTagTableEntry, SettingName );
		}
	}
	else
	{
		AddSucceeded = false;
	}


	return(AddSucceeded);
}






//
//	AddBoolean
//		Add a boolean item to the tag and sub-tag tables, then update the index to maintain sorting order.
//
bool READER_CONFIGURATION_STORAGE::AddBoolean(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
				  bool (*ExitAction) (void), bool *ActiveValueLocation, bool ActiveValue, bool DefaultValue)
{
	bool	AddSucceeded = true;			// Assume this operation works until we know otherwise.
	int		NewTagTableEntry;


	NewTagTableEntry = TagTable->GetExistingTagOrCreateNewTag(SettingName);
	if(NewTagTableEntry >= 0)
	{
		int				NewSubTagTableEntry;
		SUB_TAG_TABLE	*SubTagTable = TagTable->Entries[NewTagTableEntry].SubTagTable;


		NewSubTagTableEntry = SubTagTable->AddBasicInformationForNewSubTag(&SettingName[SIZE_OF_TAG], EntryAction, ExitAction, Modifier);
		if(NewSubTagTableEntry >= 0)
		{
		// Add the remaining information for the new sub-tag.
			SubTagTable->Entries[NewSubTagTableEntry].TypeFlags = Modifier | SUB_TAG_TABLE::BOOLEAN;
			SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.b = DefaultValue;
			SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.b = DefaultValue;

			int		WorkgroupNumber;
			for( WorkgroupNumber = 0;    WorkgroupNumber < SubTagTable->Entries[NewSubTagTableEntry].NumberOfWorkgroups;    ++WorkgroupNumber )
			{
				SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ WorkgroupNumber ].b = ActiveValue;
				SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ WorkgroupNumber ].b = ActiveValue;
			}

			SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.b = ActiveValueLocation;
			*(SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.b) = ActiveValue;
		}

		else
		{
		// Sub tag entry already exists; WOOF!
			AddSucceeded = false;
			ProcessError( NewSubTagTableEntry, SettingName );
		}
	}
	else
	{
		AddSucceeded = false;
	}

	return(AddSucceeded);
}






//
//	AddString  (overloaded version #1, where active and default value lengths are explicitly specified).
//		Add a string item to the tag and sub-tag tables, then update the index to maintain sorting order.
//
bool READER_CONFIGURATION_STORAGE::AddString(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), char *ActiveValueLocation, unsigned int MaximumSizeOfActiveValue, const char *ActiveValue,
			int ActiveValueLength, const char *DefaultValue, int DefaultValueLength)
{
	bool	AddSucceeded = true;			// Assume this operation works until we know otherwise.
	int		NewTagTableEntry;


// If the default value exceeds the string length, WOOF!
	if((DefaultValueLength > ((int) MaximumSizeOfActiveValue)) || (ActiveValueLength > ((int) MaximumSizeOfActiveValue)))
	{
		AddSucceeded = false;
	}


	else
	{
		NewTagTableEntry = TagTable->GetExistingTagOrCreateNewTag(SettingName);
		if(NewTagTableEntry >= 0)
		{
			int				NewSubTagTableEntry;
			SUB_TAG_TABLE	*SubTagTable = TagTable->Entries[NewTagTableEntry].SubTagTable;


			NewSubTagTableEntry = SubTagTable->AddBasicInformationForNewSubTag(&SettingName[SIZE_OF_TAG], EntryAction, ExitAction, Modifier);
			if(NewSubTagTableEntry >= 0)
			{
			// Add the remaining information for the new sub-tag.
				SubTagTable->Entries[NewSubTagTableEntry].TypeFlags = Modifier | SUB_TAG_TABLE::STRING;
				SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.c = new char[MaximumSizeOfActiveValue + 1];
				memcpy(SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.c, DefaultValue, DefaultValueLength + 1 );
				SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.c = new char[MaximumSizeOfActiveValue + 1];
				memcpy(SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.c, DefaultValue, DefaultValueLength + 1 );

				int		WorkgroupNumber;
				for( WorkgroupNumber = 0;    WorkgroupNumber < SubTagTable->Entries[NewSubTagTableEntry].NumberOfWorkgroups;    ++WorkgroupNumber )
				{
					SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ WorkgroupNumber ].c = new char[MaximumSizeOfActiveValue + 1];
					memcpy(SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ WorkgroupNumber ].c, ActiveValue, ActiveValueLength + 1 );
					SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ WorkgroupNumber ].c = new char[MaximumSizeOfActiveValue + 1];
					memcpy(SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ WorkgroupNumber ].c, ActiveValue, ActiveValueLength + 1 );
				}

				SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.c = ActiveValueLocation;
				memcpy(SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.c, ActiveValue, ActiveValueLength + 1 );
				SubTagTable->Entries[NewSubTagTableEntry].MaximumSizeOfActiveValue = MaximumSizeOfActiveValue;
			}

			else
			{
			// Sub tag entry already exists; WOOF!
				AddSucceeded = false;
				ProcessError( NewSubTagTableEntry, SettingName );
			}
		}
		else
		{
			AddSucceeded = false;
		}
	}

	return(AddSucceeded);
} // AddString (overloaded version #1)






//
//	AddString  (overloaded version #2, where active and default value lengths are implicit via null termination).
//		Add a string item to the tag and sub-tag tables, then update the index to maintain sorting order.
//
bool READER_CONFIGURATION_STORAGE::AddString(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), char *ActiveValueLocation, unsigned int MaximumSizeOfActiveValue, const char *ActiveValue,
			const char *DefaultValue)
{
	return(AddString(SettingName, Modifier, EntryAction, ExitAction, ActiveValueLocation, MaximumSizeOfActiveValue,
			ActiveValue,  strlen(ActiveValue), DefaultValue, strlen(DefaultValue)));
//			ActiveValue,  strlen(ActiveValue) + 1, DefaultValue, strlen(DefaultValue) + 1));
} // AddString (overloaded version #2)







//
//	AddUnicodeString
//		Add a unicode string item to the tag table.
//
bool READER_CONFIGURATION_STORAGE::AddUnicodeString(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), HSTRING **ActiveValueLocation, unsigned int MaximumSizeOfActiveValue, const char *ActiveValue,
			const char *DefaultValue)
{
	bool	AddSucceeded = true;			// Assume this operation works until we know otherwise.
	int		NewTagTableEntry;


// If the default value exceeds the string length, WOOF!
	if((strlen(DefaultValue) > ((int) MaximumSizeOfActiveValue)) || (strlen(ActiveValue) > ((int) MaximumSizeOfActiveValue)))
	{
		AddSucceeded = false;
	}

	else
	{
		NewTagTableEntry = TagTable->GetExistingTagOrCreateNewTag(SettingName);
		if(NewTagTableEntry >= 0)
		{
			int				NewSubTagTableEntry;
			SUB_TAG_TABLE	*SubTagTable = TagTable->Entries[NewTagTableEntry].SubTagTable;

			NewSubTagTableEntry = SubTagTable->AddBasicInformationForNewSubTag(&SettingName[SIZE_OF_TAG], EntryAction, ExitAction, Modifier);
			if(NewSubTagTableEntry >= 0)
			{
			// Add the remaining information for the new sub-tag.
				SubTagTable->Entries[NewSubTagTableEntry].TypeFlags = Modifier | SUB_TAG_TABLE::UNICODE_STRING;
				SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.BinaryString = new HSTRING(MaximumSizeOfActiveValue + 1, true);
				SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.BinaryString->Copy(DefaultValue);
				SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.BinaryString->SimpleAsciiToUniCodeFormat();

				SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.BinaryString = new HSTRING(MaximumSizeOfActiveValue + 1, true);
				SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.BinaryString->Copy(DefaultValue);
				SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.BinaryString->SimpleAsciiToUniCodeFormat();

				int		WorkgroupNumber;
				for( WorkgroupNumber = 0;    WorkgroupNumber < SubTagTable->Entries[NewSubTagTableEntry].NumberOfWorkgroups;    ++WorkgroupNumber )
				{
					SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ WorkgroupNumber ].BinaryString = new HSTRING(MaximumSizeOfActiveValue + 1, true);
					SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ WorkgroupNumber ].BinaryString->Copy(ActiveValue);
					SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ WorkgroupNumber ].BinaryString->SimpleAsciiToUniCodeFormat();

					SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ WorkgroupNumber ].BinaryString = new HSTRING(MaximumSizeOfActiveValue + 1, true);
					SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ WorkgroupNumber ].BinaryString->Copy(ActiveValue);
					SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ WorkgroupNumber ].BinaryString->SimpleAsciiToUniCodeFormat();
				}

				SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.BinaryString = new HSTRING(MaximumSizeOfActiveValue + 1, true);
				SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.BinaryString->Copy(ActiveValue);
				SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.BinaryString->SimpleAsciiToUniCodeFormat();
				*ActiveValueLocation = SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.BinaryString;

				SubTagTable->Entries[NewSubTagTableEntry].MaximumSizeOfActiveValue = MaximumSizeOfActiveValue;
			}

			else
			{
			// Sub tag entry already exists; WOOF!
				AddSucceeded = false;
				ProcessError( NewSubTagTableEntry, SettingName );
			}
		}
		else
		{
			AddSucceeded = false;
		}
	}

	return(AddSucceeded);
} // AddUnicodeString





//
//	AddBinaryString
//		Add a binary string item to the tag table.
//
bool READER_CONFIGURATION_STORAGE::AddBinaryString(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), HSTRING **ActiveValueLocation, unsigned int MaximumSizeOfActiveValue, const char *ActiveValue,
			const char *DefaultValue)
{
	bool	AddSucceeded = true;			// Assume this operation works until we know otherwise.
	int		NewTagTableEntry;


// If the default value exceeds the string length, WOOF!
	if((strlen(DefaultValue) > ((int) MaximumSizeOfActiveValue)) || (strlen(ActiveValue) > ((int) MaximumSizeOfActiveValue)))
	{
		AddSucceeded = false;
	}


	else
	{
		NewTagTableEntry = TagTable->GetExistingTagOrCreateNewTag(SettingName);
		if(NewTagTableEntry >= 0)
		{
			int				NewSubTagTableEntry;
			SUB_TAG_TABLE	*SubTagTable = TagTable->Entries[NewTagTableEntry].SubTagTable;


			NewSubTagTableEntry = SubTagTable->AddBasicInformationForNewSubTag(&SettingName[SIZE_OF_TAG], EntryAction, ExitAction, Modifier);
			if(NewSubTagTableEntry >= 0)
			{
			// Add the remaining information for the new sub-tag.
				SubTagTable->Entries[NewSubTagTableEntry].TypeFlags = Modifier | SUB_TAG_TABLE::BINARY_STRING;
				SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.BinaryString = new HSTRING(MaximumSizeOfActiveValue + 1, true);
				SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.BinaryString->Copy(DefaultValue);
				SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.BinaryString->UnprintableFormatsToCharacters();

				SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.BinaryString = new HSTRING(MaximumSizeOfActiveValue + 1, true);
				SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.BinaryString->Copy(DefaultValue);
				SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.BinaryString->UnprintableFormatsToCharacters();

				int		WorkgroupNumber;
				for( WorkgroupNumber = 0;    WorkgroupNumber < SubTagTable->Entries[NewSubTagTableEntry].NumberOfWorkgroups;    ++WorkgroupNumber )
				{
					SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ WorkgroupNumber ].BinaryString = new HSTRING(MaximumSizeOfActiveValue + 1, true);
					SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ WorkgroupNumber ].BinaryString->Copy(ActiveValue);
					SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ WorkgroupNumber ].BinaryString->UnprintableFormatsToCharacters();

					SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ WorkgroupNumber ].BinaryString = new HSTRING(MaximumSizeOfActiveValue + 1, true);
					SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ WorkgroupNumber ].BinaryString->Copy(ActiveValue);
					SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ WorkgroupNumber ].BinaryString->UnprintableFormatsToCharacters();
				}

				SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.BinaryString = new HSTRING(MaximumSizeOfActiveValue + 1, true);
				SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.BinaryString->Copy(ActiveValue);
				SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.BinaryString->UnprintableFormatsToCharacters();
				*ActiveValueLocation = SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.BinaryString;

				SubTagTable->Entries[NewSubTagTableEntry].MaximumSizeOfActiveValue = MaximumSizeOfActiveValue;
			}

			else
			{
			// Sub tag entry already exists; WOOF!
				AddSucceeded = false;
				ProcessError( NewSubTagTableEntry, SettingName );
			}
		}
		else
		{
			AddSucceeded = false;
		}
	}

	return(AddSucceeded);
} // AddBinaryString





//
//	AddASCIIString	(overloaded version #1, where maximum is determined by the caller.).
//		Add an ASCII HSTRING item to the tag table.  If maximum size is zero, create the strings to automatically resize.
//
bool READER_CONFIGURATION_STORAGE::AddASCIIString(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), HSTRING **ActiveValueLocation, unsigned int MaximumSizeOfActiveValue, const char *ActiveValue,
			const char *DefaultValue)
{
	bool	AddSucceeded = true;			// Assume this operation works until we know otherwise.
	int		NewTagTableEntry;


// If the default value exceeds the string length, WOOF!
	if((strlen(DefaultValue) > ((int) MaximumSizeOfActiveValue)) || (strlen(ActiveValue) > ((int) MaximumSizeOfActiveValue)))
	{
		AddSucceeded = false;
	}


	else
	{
		NewTagTableEntry = TagTable->GetExistingTagOrCreateNewTag(SettingName);
		if(NewTagTableEntry >= 0)
		{
			int				NewSubTagTableEntry;
			SUB_TAG_TABLE	*SubTagTable = TagTable->Entries[NewTagTableEntry].SubTagTable;


			NewSubTagTableEntry = SubTagTable->AddBasicInformationForNewSubTag(&SettingName[SIZE_OF_TAG], EntryAction, ExitAction, Modifier);
			if(NewSubTagTableEntry >= 0)
			{
			// Add the remaining information for the new sub-tag.
				SubTagTable->Entries[NewSubTagTableEntry].TypeFlags = Modifier | SUB_TAG_TABLE::ASCII_STRING;
				SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.BinaryString =
						( MaximumSizeOfActiveValue != 0 ) ? ( new HSTRING(MaximumSizeOfActiveValue, false ) ) :
														( new HSTRING( READER_CONFIGURATION_DEFAULT_STRING_ALLOCATION, true) );
				SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.BinaryString->Copy(DefaultValue);

				SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.BinaryString =
						( MaximumSizeOfActiveValue != 0 ) ? ( new HSTRING(MaximumSizeOfActiveValue, false ) ) :
														( new HSTRING( READER_CONFIGURATION_DEFAULT_STRING_ALLOCATION, true) );
				SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.BinaryString->Copy(DefaultValue);

				int		WorkgroupNumber;
				for( WorkgroupNumber = 0;    WorkgroupNumber < SubTagTable->Entries[NewSubTagTableEntry].NumberOfWorkgroups;    ++WorkgroupNumber )
				{
					SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ WorkgroupNumber ].BinaryString =
						( MaximumSizeOfActiveValue != 0 ) ? ( new HSTRING(MaximumSizeOfActiveValue, false ) ) :
														( new HSTRING( READER_CONFIGURATION_DEFAULT_STRING_ALLOCATION, true) );
					SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ WorkgroupNumber ].BinaryString->Copy(ActiveValue);

					SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ WorkgroupNumber ].BinaryString =
						( MaximumSizeOfActiveValue != 0 ) ? ( new HSTRING(MaximumSizeOfActiveValue, false ) ) :
														( new HSTRING( READER_CONFIGURATION_DEFAULT_STRING_ALLOCATION, true) );
					SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ WorkgroupNumber ].BinaryString->Copy(ActiveValue);
				}

				SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.BinaryString =
						( MaximumSizeOfActiveValue != 0 ) ? ( new HSTRING(MaximumSizeOfActiveValue, false ) ) :
														( new HSTRING( READER_CONFIGURATION_DEFAULT_STRING_ALLOCATION, true) );
				SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.BinaryString->Copy(ActiveValue);
				*ActiveValueLocation = SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.BinaryString;

				SubTagTable->Entries[NewSubTagTableEntry].MaximumSizeOfActiveValue = MaximumSizeOfActiveValue;
			}

			else
			{
			// Sub tag entry already exists; WOOF!
				AddSucceeded = false;
				ProcessError( NewSubTagTableEntry, SettingName );
			}
		}
		else
		{
			AddSucceeded = false;
		}
	}

	return(AddSucceeded);
} // AddASCIIString (version #1)





//
//	AddASCIIString	(overloaded version #2, where maximum string size is unlimited is determined by the caller.).
//		Add an ASCII HSTRING item to the tag table.
//
bool READER_CONFIGURATION_STORAGE::AddASCIIString(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), HSTRING **ActiveValueLocation, const char *ActiveValue, const char *DefaultValue)
{
	return AddASCIIString( SettingName, Modifier, EntryAction, ExitAction, ActiveValueLocation, 0, ActiveValue, DefaultValue );
} // AddASCIIString (version #2)





//
//	AddHexString
//		Add a hexadecimal string item to the tag table.
//
bool READER_CONFIGURATION_STORAGE::AddHexString(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), HSTRING **ActiveValueLocation, unsigned int MaximumSizeOfActiveValue, const char *ActiveValue,
			const char *DefaultValue)
{
	bool	AddSucceeded = true;			// Assume this operation works until we know otherwise.
	int		NewTagTableEntry;


// If the default value exceeds the string length, WOOF!
	if((strlen(DefaultValue) > ((int) MaximumSizeOfActiveValue)) || (strlen(ActiveValue) > ((int) MaximumSizeOfActiveValue)))
	{
		AddSucceeded = false;
	}


	else
	{
		NewTagTableEntry = TagTable->GetExistingTagOrCreateNewTag(SettingName);
		if(NewTagTableEntry >= 0)
		{
			int				NewSubTagTableEntry;
			SUB_TAG_TABLE	*SubTagTable = TagTable->Entries[NewTagTableEntry].SubTagTable;


			NewSubTagTableEntry = SubTagTable->AddBasicInformationForNewSubTag(&SettingName[SIZE_OF_TAG], EntryAction, ExitAction, Modifier);
			if(NewSubTagTableEntry >= 0)
			{
			// Add the remaining information for the new sub-tag.
				SubTagTable->Entries[NewSubTagTableEntry].TypeFlags = Modifier | SUB_TAG_TABLE::HEX_STRING;
				SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.BinaryString = new HSTRING(MaximumSizeOfActiveValue + 1, true);
				SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.BinaryString->Copy(DefaultValue);
				if(!SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.BinaryString->HexToBinary())
				{
					SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.BinaryString->Copy("");
				}

				SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.BinaryString = new HSTRING(MaximumSizeOfActiveValue + 1, true);
				SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.BinaryString->Copy(DefaultValue);
				if(!SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.BinaryString->HexToBinary())
				{
					SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.BinaryString->Copy("");
				}

				int		WorkgroupNumber;
				for( WorkgroupNumber = 0;    WorkgroupNumber < SubTagTable->Entries[NewSubTagTableEntry].NumberOfWorkgroups;    ++WorkgroupNumber )
				{
					SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ WorkgroupNumber ].BinaryString = new HSTRING(MaximumSizeOfActiveValue + 1, true);
					SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ WorkgroupNumber ].BinaryString->Copy(ActiveValue);
					if(!SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ WorkgroupNumber ].BinaryString->HexToBinary())
					{
						SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ WorkgroupNumber ].BinaryString->Copy("");
					}

					SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ WorkgroupNumber ].BinaryString = new HSTRING(MaximumSizeOfActiveValue + 1, true);
					SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ WorkgroupNumber ].BinaryString->Copy(ActiveValue);
					if(!SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ WorkgroupNumber ].BinaryString->HexToBinary())
					{
						SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ WorkgroupNumber ].BinaryString->Copy("");
					}
				}

				SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.BinaryString = new HSTRING(MaximumSizeOfActiveValue + 1, true);
				SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.BinaryString->Copy(ActiveValue);
				if(!SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.BinaryString->HexToBinary())
				{
					SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.BinaryString->Copy("");
				}
				*ActiveValueLocation = SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.BinaryString;

				SubTagTable->Entries[NewSubTagTableEntry].MaximumSizeOfActiveValue = MaximumSizeOfActiveValue;
			}

			else
			{
			// Sub tag entry already exists; WOOF!
				AddSucceeded = false;
				ProcessError( NewSubTagTableEntry, SettingName );
			}
		}
		else
		{
			AddSucceeded = false;
		}
	}

	return(AddSucceeded);
} // AddHexString






//
//	AddDataFormat
//		Add a data format item to the tag and sub-tag tables, then update the index to maintain sorting order.
//
bool READER_CONFIGURATION_STORAGE::AddDataFormat(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), DATA_FORMAT **ActiveValueLocation, unsigned int MaximumSizeOfActiveValue, const char *ActiveValue,
			const char *DefaultValue)
{
	bool	AddSucceeded = true;			// Assume this operation works until we know otherwise.
	int		NewTagTableEntry;


// If the default value exceeds the string length, WOOF!
	if((strlen(DefaultValue) > ((int) MaximumSizeOfActiveValue)) || (strlen(ActiveValue) > ((int) MaximumSizeOfActiveValue)))
	{
		AddSucceeded = false;
	}


	else
	{
		NewTagTableEntry = TagTable->GetExistingTagOrCreateNewTag(SettingName);
		if(NewTagTableEntry >= 0)
		{
			int				NewSubTagTableEntry;
			SUB_TAG_TABLE	*SubTagTable = TagTable->Entries[NewTagTableEntry].SubTagTable;


			NewSubTagTableEntry = SubTagTable->AddBasicInformationForNewSubTag(&SettingName[SIZE_OF_TAG], EntryAction, ExitAction, Modifier);
			if(NewSubTagTableEntry >= 0)
			{
			// Add the remaining information for the new sub-tag.
				SubTagTable->Entries[NewSubTagTableEntry].TypeFlags = Modifier | SUB_TAG_TABLE::FORMAT_STRING;
				SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.DataFormat =
							new DATA_FORMAT(MaximumSizeOfActiveValue, DefaultValue);
				SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.DataFormat =
							new DATA_FORMAT(MaximumSizeOfActiveValue, DefaultValue);

				int		WorkgroupNumber;
				for( WorkgroupNumber = 0;    WorkgroupNumber < SubTagTable->Entries[NewSubTagTableEntry].NumberOfWorkgroups;    ++WorkgroupNumber )
				{
					SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ WorkgroupNumber ].DataFormat =
							new DATA_FORMAT(MaximumSizeOfActiveValue, ActiveValue);

					SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ WorkgroupNumber ].DataFormat =
							new DATA_FORMAT(MaximumSizeOfActiveValue, ActiveValue);
				}

				SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.DataFormat =
							new DATA_FORMAT(MaximumSizeOfActiveValue, ActiveValue);
				*ActiveValueLocation = SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.DataFormat;
			}

			else
			{
			// Sub tag entry already exists; WOOF!
				AddSucceeded = false;
				ProcessError( NewSubTagTableEntry, SettingName );
			}
		}
		else
		{
			AddSucceeded = false;
		}
	}

	return(AddSucceeded);
} // AddDataFormat






//
//	AddPrefixSuffix
//		Add a prefix or suffix item to the tag and sub-tag tables, then update the index to maintain sorting order.
//
bool READER_CONFIGURATION_STORAGE::AddPrefixSuffix(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), PREFIX_SUFFIX **ActiveValueLocation, unsigned int MaximumSizeOfActiveValue, const char *ActiveValue,
			const char *DefaultValue)
{
	bool	AddSucceeded = true;			// Assume this operation works until we know otherwise.
	int		NewTagTableEntry;


// If the default value exceeds the string length, WOOF!
	if((strlen(DefaultValue) > ((int) MaximumSizeOfActiveValue)) || (strlen(ActiveValue) > ((int) MaximumSizeOfActiveValue)))
	{
		AddSucceeded = false;
	}


	else
	{
		NewTagTableEntry = TagTable->GetExistingTagOrCreateNewTag(SettingName);
		if(NewTagTableEntry >= 0)
		{
			int				NewSubTagTableEntry;
			SUB_TAG_TABLE	*SubTagTable = TagTable->Entries[NewTagTableEntry].SubTagTable;


			NewSubTagTableEntry = SubTagTable->AddBasicInformationForNewSubTag(&SettingName[SIZE_OF_TAG], EntryAction, ExitAction, Modifier);
			if(NewSubTagTableEntry >= 0)
			{
			// Add the remaining information for the new sub-tag.
				SubTagTable->Entries[NewSubTagTableEntry].TypeFlags = Modifier | SUB_TAG_TABLE::PREFIX_SUFFIX_STRING;
				SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.PrefixSuffix =
							new PREFIX_SUFFIX(MaximumSizeOfActiveValue, DefaultValue);
				SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.PrefixSuffix =
							new PREFIX_SUFFIX(MaximumSizeOfActiveValue, DefaultValue);

				int		WorkgroupNumber;
				for( WorkgroupNumber = 0;    WorkgroupNumber < SubTagTable->Entries[NewSubTagTableEntry].NumberOfWorkgroups;    ++WorkgroupNumber )
				{
					SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ WorkgroupNumber ].PrefixSuffix =
							new PREFIX_SUFFIX(MaximumSizeOfActiveValue, ActiveValue);

					SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ WorkgroupNumber ].PrefixSuffix =
							new PREFIX_SUFFIX(MaximumSizeOfActiveValue, ActiveValue);
				}

				SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.PrefixSuffix =
							new PREFIX_SUFFIX(MaximumSizeOfActiveValue, ActiveValue);
				*ActiveValueLocation = SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.PrefixSuffix;
			}

			else
			{
			// Sub tag entry already exists; WOOF!
				AddSucceeded = false;
				ProcessError( NewSubTagTableEntry, SettingName );
			}
		}
		else
		{
			AddSucceeded = false;
		}
	}

	return(AddSucceeded);
} // AddPrefixSuffix







//
//	AddNumber	(overloaded version #1, where min and max are addresses).
//		Add a numeric item to the tag and sub-tag tables, then update the index to maintain sorting order.
//
bool READER_CONFIGURATION_STORAGE::AddNumber(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
		bool (*ExitAction) (void), int *ActiveValueLocation, int ActiveValue, int DefaultValue, int MinimumRange,
		int *MinimumRangeAddress, int MaximumRange, int *MaximumRangeAddress)
{
	bool	AddSucceeded = true;			// Assume this operation works until we know otherwise.
	int		NewTagTableEntry;


	NewTagTableEntry = TagTable->GetExistingTagOrCreateNewTag(SettingName);
	if(NewTagTableEntry >= 0)
	{
		int				NewSubTagTableEntry;
		SUB_TAG_TABLE	*SubTagTable = TagTable->Entries[NewTagTableEntry].SubTagTable;


		NewSubTagTableEntry = SubTagTable->AddBasicInformationForNewSubTag(&SettingName[SIZE_OF_TAG], EntryAction, ExitAction, Modifier);
		if(NewSubTagTableEntry >= 0)
		{
		// Add the remaining information for the new sub-tag.
			SubTagTable->Entries[NewSubTagTableEntry].TypeFlags = Modifier | SUB_TAG_TABLE::NUMERIC;
			SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.i = DefaultValue;
			SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.i = DefaultValue;

			int		WorkgroupNumber;
			for( WorkgroupNumber = 0;    WorkgroupNumber < SubTagTable->Entries[NewSubTagTableEntry].NumberOfWorkgroups;    ++WorkgroupNumber )
			{
				SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ WorkgroupNumber ].i = ActiveValue;
				SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ WorkgroupNumber ].i = ActiveValue;
			}

			SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.i = ActiveValueLocation;
			*(SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.i) = ActiveValue;

			AddSucceeded = UpdateNumericRange(&(SubTagTable->Entries[NewSubTagTableEntry]), MinimumRange, MinimumRangeAddress,
					MaximumRange, MaximumRangeAddress);
		}

		else
		{
		// Sub tag entry already exists; WOOF!
			AddSucceeded = false;
			ProcessError( NewSubTagTableEntry, SettingName );
		}
	}
	else
	{
		AddSucceeded = false;
	}

	return(AddSucceeded);
}






//
//	AddNumber	(overloaded version #2, where min is an address and max is a constant).
//		Add a numeric item to the tag and sub-tag tables, then update the index to maintain sorting order.
//
bool READER_CONFIGURATION_STORAGE::AddNumber(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
		bool (*ExitAction) (void), int *ActiveValueLocation, int ActiveValue, int DefaultValue, int MinimumRange,
		int *MinimumRangeAddress, int MaximumRange)
{
	bool	AddSucceeded = true;			// Assume this operation works until we know otherwise.
	int		NewTagTableEntry;


	NewTagTableEntry = TagTable->GetExistingTagOrCreateNewTag(SettingName);
	if(NewTagTableEntry >= 0)
	{
		int				NewSubTagTableEntry;
		SUB_TAG_TABLE	*SubTagTable = TagTable->Entries[NewTagTableEntry].SubTagTable;


		NewSubTagTableEntry = SubTagTable->AddBasicInformationForNewSubTag(&SettingName[SIZE_OF_TAG], EntryAction, ExitAction, Modifier);
		if(NewSubTagTableEntry >= 0)
		{
		// Add the remaining information for the new sub-tag.
			SubTagTable->Entries[NewSubTagTableEntry].TypeFlags = Modifier | SUB_TAG_TABLE::NUMERIC;
			SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.i = DefaultValue;
			SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.i = DefaultValue;

			int		WorkgroupNumber;
			for( WorkgroupNumber = 0;    WorkgroupNumber < SubTagTable->Entries[NewSubTagTableEntry].NumberOfWorkgroups;    ++WorkgroupNumber )
			{
				SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ WorkgroupNumber ].i = ActiveValue;
				SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ WorkgroupNumber ].i = ActiveValue;
			}

			SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.i = ActiveValueLocation;
			*(SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.i) = ActiveValue;

			AddSucceeded = UpdateNumericRange(&(SubTagTable->Entries[NewSubTagTableEntry]), MinimumRange, MinimumRangeAddress,
					MaximumRange);
		}

		else
		{
		// Sub tag entry already exists; WOOF!
			AddSucceeded = false;
			ProcessError( NewSubTagTableEntry, SettingName );
		}
	}
	else
	{
		AddSucceeded = false;
	}

	return(AddSucceeded);
}






//
//	AddNumber	(overloaded version #3, where min is a constant and max is an address).
//		Add a numeric item to the tag and sub-tag tables, then update the index to maintain sorting order.
//
bool READER_CONFIGURATION_STORAGE::AddNumber(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
		bool (*ExitAction) (void), int *ActiveValueLocation, int ActiveValue, int DefaultValue, int MinimumRange,
		int MaximumRange, int *MaximumRangeAddress)
{
	bool	AddSucceeded = true;			// Assume this operation works until we know otherwise.
	int		NewTagTableEntry;


	NewTagTableEntry = TagTable->GetExistingTagOrCreateNewTag(SettingName);
	if(NewTagTableEntry >= 0)
	{
		int				NewSubTagTableEntry;
		SUB_TAG_TABLE	*SubTagTable = TagTable->Entries[NewTagTableEntry].SubTagTable;


		NewSubTagTableEntry = SubTagTable->AddBasicInformationForNewSubTag(&SettingName[SIZE_OF_TAG], EntryAction, ExitAction, Modifier);
		if(NewSubTagTableEntry >= 0)
		{
		// Add the remaining information for the new sub-tag.
			SubTagTable->Entries[NewSubTagTableEntry].TypeFlags = Modifier | SUB_TAG_TABLE::NUMERIC;
			SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.i = DefaultValue;
			SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.i = DefaultValue;

			int		WorkgroupNumber;
			for( WorkgroupNumber = 0;    WorkgroupNumber < SubTagTable->Entries[NewSubTagTableEntry].NumberOfWorkgroups;    ++WorkgroupNumber )
			{
				SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ WorkgroupNumber ].i = ActiveValue;
				SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ WorkgroupNumber ].i = ActiveValue;
			}

			SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.i = ActiveValueLocation;
			*(SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.i) = ActiveValue;

			AddSucceeded = UpdateNumericRange(&(SubTagTable->Entries[NewSubTagTableEntry]), MinimumRange, MaximumRange,
					MaximumRangeAddress);
		}

		else
		{
		// Sub tag entry already exists; WOOF!
			AddSucceeded = false;
			ProcessError( NewSubTagTableEntry, SettingName );
		}
	}
	else
	{
		AddSucceeded = false;
	}

	return(AddSucceeded);
}






//
//	AddNumber	(overloaded version #4, where min and max are constants).
//		Add a numeric item to the tag and sub-tag tables, then update the index to maintain sorting order.
//
bool READER_CONFIGURATION_STORAGE::AddNumber(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
		bool (*ExitAction) (void), int *ActiveValueLocation, int ActiveValue, int DefaultValue, int MinimumRange,
		int MaximumRange)
{
	bool	AddSucceeded = true;			// Assume this operation works until we know otherwise.
	int		NewTagTableEntry;


	NewTagTableEntry = TagTable->GetExistingTagOrCreateNewTag(SettingName);
	if(NewTagTableEntry >= 0)
	{
		int				NewSubTagTableEntry;
		SUB_TAG_TABLE	*SubTagTable = TagTable->Entries[NewTagTableEntry].SubTagTable;


		NewSubTagTableEntry = SubTagTable->AddBasicInformationForNewSubTag(&SettingName[SIZE_OF_TAG], EntryAction, ExitAction, Modifier);
		if(NewSubTagTableEntry >= 0)
		{
		// Add the remaining information for the new sub-tag.
			SubTagTable->Entries[NewSubTagTableEntry].TypeFlags = Modifier | SUB_TAG_TABLE::NUMERIC;
			SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.i = DefaultValue;
			SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.i = DefaultValue;

			int		WorkgroupNumber;
			for( WorkgroupNumber = 0;    WorkgroupNumber < SubTagTable->Entries[NewSubTagTableEntry].NumberOfWorkgroups;    ++WorkgroupNumber )
			{
				SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ WorkgroupNumber ].i = ActiveValue;
				SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ WorkgroupNumber ].i = ActiveValue;
			}

			SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.i = ActiveValueLocation;
			*(SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.i) = ActiveValue;

			AddSucceeded = UpdateNumericRange(&(SubTagTable->Entries[NewSubTagTableEntry]), MinimumRange, MaximumRange);
		}

		else
		{
		// Sub tag entry already exists; WOOF!
			AddSucceeded = false;
			ProcessError( NewSubTagTableEntry, SettingName );
		}
	}
	else
	{
		AddSucceeded = false;
	}

	return(AddSucceeded);
}







//
//	AddNumericList
//		Add a numeric list item to the tag and sub-tag tables, then update the index to maintain sorting order.
//
bool READER_CONFIGURATION_STORAGE::AddNumericList(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
		bool (*ExitAction) (void), int *ActiveValueLocation, int ActiveValue, int DefaultValue,
		int Item1, int Item2, int Item3, int Item4, int Item5, int Item6, int Item7, int Item8, int Item9, int Item10,
		int Item11, int Item12, int Item13, int Item14, int Item15, int Item16, int Item17, int Item18, int Item19, int Item20,
		int Item21, int Item22, int Item23, int Item24, int Item25, int Item26, int Item27, int Item28, int Item29, int Item30,
		int Item31, int Item32, int Item33, int Item34, int Item35, int Item36, int Item37, int Item38, int Item39, int Item40,
		int Item41, int Item42, int Item43, int Item44, int Item45, int Item46, int Item47, int Item48, int Item49, int Item50,
		int Item51, int Item52, int Item53, int Item54, int Item55, int Item56, int Item57, int Item58, int Item59, int Item60)
{
	bool	AddSucceeded = true;			// Assume this operation works until we know otherwise.
	int		NewTagTableEntry;


	NewTagTableEntry = TagTable->GetExistingTagOrCreateNewTag(SettingName);
	if(NewTagTableEntry >= 0)
	{
		int				NewSubTagTableEntry;
		SUB_TAG_TABLE	*SubTagTable = TagTable->Entries[NewTagTableEntry].SubTagTable;

		NewSubTagTableEntry = SubTagTable->AddBasicInformationForNewSubTag(&SettingName[SIZE_OF_TAG], EntryAction, ExitAction, Modifier);
		if(NewSubTagTableEntry >= 0)
		{
		// Add the remaining information for the new sub-tag.
			SubTagTable->Entries[NewSubTagTableEntry].TypeFlags = Modifier | SUB_TAG_TABLE::NUMERIC_LIST;
			SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.i = DefaultValue;
			SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.i = DefaultValue;

			int		WorkgroupNumber;
			for( WorkgroupNumber = 0;    WorkgroupNumber < SubTagTable->Entries[NewSubTagTableEntry].NumberOfWorkgroups;    ++WorkgroupNumber )
			{
				SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ WorkgroupNumber ].i = ActiveValue;
				SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ WorkgroupNumber ].i = ActiveValue;
			}

			SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.i = ActiveValueLocation;
			*(SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.i) = ActiveValue;

		// Create the item list and copy the entries.
			SubTagTable->Entries[NewSubTagTableEntry].NumberOfEntriesInListRange = 0;
			AddSucceeded = UpdateNumericList(&(SubTagTable->Entries[NewSubTagTableEntry]),
					Item1, Item2, Item3, Item4, Item5, Item6, Item7, Item8, Item9, Item10,
					Item11, Item12, Item13, Item14, Item15, Item16, Item17, Item18, Item19, Item20,
					Item21, Item22, Item23, Item24, Item25, Item26, Item27, Item28, Item29, Item30,
					Item31, Item32, Item33, Item34, Item35, Item36, Item37, Item38, Item39, Item40,
					Item41, Item42, Item43, Item44, Item45, Item46, Item47, Item48, Item49, Item50,
					Item51, Item52, Item53, Item54, Item55, Item56, Item57, Item58, Item59, Item60);
		}

		else
		{
		// Sub tag entry already exists; WOOF!
			AddSucceeded = false;
			ProcessError( NewSubTagTableEntry, SettingName );
		}
	}
	else
	{
		AddSucceeded = false;
	}

	return(AddSucceeded);
}







//
//	AddStringList
//		Add a numeric list item to the tag and sub-tag tables, then update the index to maintain sorting order.
//
bool READER_CONFIGURATION_STORAGE::AddStringList(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), char *ActiveValueLocation, unsigned int MaximumSizeOfActiveValue, const char *ActiveValue,
			const char *DefaultValue, const char *Item1, const char *Item2, const char *Item3, const char *Item4,
			const char *Item5, const char *Item6, const char *Item7, const char *Item8, const char *Item9, const char *Item10,
			const char *Item11, const char *Item12, const char *Item13, const char *Item14, const char *Item15,
			const char *Item16, const char *Item17, const char *Item18, const char *Item19, const char *Item20,
			const char *Item21, const char *Item22, const char *Item23, const char *Item24, const char *Item25,
			const char *Item26, const char *Item27, const char *Item28, const char *Item29, const char *Item30,
			const char *Item31, const char *Item32, const char *Item33, const char *Item34, const char *Item35,
			const char *Item36, const char *Item37, const char *Item38, const char *Item39, const char *Item40,
			const char *Item41, const char *Item42, const char *Item43, const char *Item44, const char *Item45,
			const char *Item46, const char *Item47, const char *Item48, const char *Item49, const char *Item50,
			const char *Item51, const char *Item52, const char *Item53, const char *Item54, const char *Item55,
			const char *Item56, const char *Item57, const char *Item58, const char *Item59, const char *Item60)
{
	bool	AddSucceeded = true;			// Assume this operation works until we know otherwise.
	int		NewTagTableEntry;




// If the default value exceeds the string length, WOOF!
	if((strlen(DefaultValue) > MaximumSizeOfActiveValue) || (strlen(ActiveValue) > MaximumSizeOfActiveValue))
	{
		AddSucceeded = false;
	}


	else
	{
		NewTagTableEntry = TagTable->GetExistingTagOrCreateNewTag(SettingName);
		if(NewTagTableEntry >= 0)
		{
			int				NewSubTagTableEntry;
			SUB_TAG_TABLE	*SubTagTable = TagTable->Entries[NewTagTableEntry].SubTagTable;


			NewSubTagTableEntry = SubTagTable->AddBasicInformationForNewSubTag(&SettingName[SIZE_OF_TAG], EntryAction, ExitAction, Modifier);
			if(NewSubTagTableEntry >= 0)
			{
			// Add the remaining information for the new sub-tag.
				SubTagTable->Entries[NewSubTagTableEntry].TypeFlags = Modifier | SUB_TAG_TABLE::STRING_LIST;

				SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.c = new char[MaximumSizeOfActiveValue + 1];
				strcpy(SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.c, DefaultValue);
				SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.c = new char[MaximumSizeOfActiveValue + 1];
				strcpy(SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.c, DefaultValue);

				int		WorkgroupNumber;
				for( WorkgroupNumber = 0;    WorkgroupNumber < SubTagTable->Entries[NewSubTagTableEntry].NumberOfWorkgroups;    ++WorkgroupNumber )
				{
					SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ WorkgroupNumber ].c = new char[MaximumSizeOfActiveValue + 1];
					strcpy(SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ WorkgroupNumber ].c, ActiveValue);

					SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ WorkgroupNumber ].c = new char[MaximumSizeOfActiveValue + 1];
					strcpy(SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ WorkgroupNumber ].c, ActiveValue);
				}

				SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.c = ActiveValueLocation;
				strcpy(SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.c, ActiveValue);
				SubTagTable->Entries[NewSubTagTableEntry].MaximumSizeOfActiveValue = MaximumSizeOfActiveValue;

			// Create the item list and copy the entries if no errors have occurred and if any list items were found.
				SubTagTable->Entries[NewSubTagTableEntry].NumberOfEntriesInListRange = 0;
				AddSucceeded = UpdateStringList(&(SubTagTable->Entries[NewSubTagTableEntry]),
					Item1, Item2, Item3, Item4, Item5, Item6, Item7, Item8, Item9, Item10,
					Item11, Item12, Item13, Item14, Item15, Item16, Item17, Item18, Item19, Item20,
					Item21, Item22, Item23, Item24, Item25, Item26, Item27, Item28, Item29, Item30,
					Item31, Item32, Item33, Item34, Item35, Item36, Item37, Item38, Item39, Item40,
					Item41, Item42, Item43, Item44, Item45, Item46, Item47, Item48, Item49, Item50,
					Item51, Item52, Item53, Item54, Item55, Item56, Item57, Item58, Item59, Item60);
			}

			else
			{
			// Sub tag entry already exists; WOOF!
				AddSucceeded = false;
				ProcessError( NewSubTagTableEntry, SettingName );
			}
		}
		else
		{
			AddSucceeded = false;
		}
	}

	return(AddSucceeded);
}







//
//	AddWorkgroupSelection
//		Add a workgroup selection item to the tag table and update the index to maintain sorting order (overloaded version #1, where
//		min and max are addresses).
//
bool READER_CONFIGURATION_STORAGE::AddWorkgroupSelection(const char *SettingName, unsigned int Modifier, bool (*EntryAction) (void),
			bool (*ExitAction) (void), int ActiveValue, int DefaultValue )
{
	bool	AddSucceeded = true;			// Assume this operation works until we know otherwise.
	int		NewTagTableEntry;


// Cannot be a workgroup setting, even if user specified otherwise.
	Modifier &= ~SUB_TAG_TABLE::WORKGROUP_SETTING;


// Active and default values must be in range 0 to NumberOfWorkgroups - 1.
	if( ( ActiveValue >= 0 ) && ( ActiveValue < NumberOfWorkgroups ) && ( DefaultValue >= 0 ) && ( DefaultValue < NumberOfWorkgroups ) )
	{
		NewTagTableEntry = TagTable->GetExistingTagOrCreateNewTag(SettingName);
		if(NewTagTableEntry >= 0)
		{
			int				NewSubTagTableEntry;
			SUB_TAG_TABLE	*SubTagTable = TagTable->Entries[NewTagTableEntry].SubTagTable;


			NewSubTagTableEntry = SubTagTable->AddBasicInformationForNewSubTag(&SettingName[SIZE_OF_TAG], EntryAction, ExitAction, Modifier);
			if(NewSubTagTableEntry >= 0)
			{
			// Add the remaining information for the new sub-tag.
				SubTagTable->Entries[NewSubTagTableEntry].TypeFlags = Modifier | SUB_TAG_TABLE::WORKGROUP_SELECTION;
				SubTagTable->Entries[NewSubTagTableEntry].StandardDefaultValue.i = DefaultValue;
				SubTagTable->Entries[NewSubTagTableEntry].DefaultValue.i = DefaultValue;

				SubTagTable->Entries[NewSubTagTableEntry].WorkingValue[ 0 ].i = ActiveValue;
				SubTagTable->Entries[NewSubTagTableEntry].ActiveValue[ 0 ].i = ActiveValue;

				SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.i = &ActiveWorkgroupNumber;
				*(SubTagTable->Entries[NewSubTagTableEntry].InstantiatorCopyOfActiveValue.i) = ActiveValue;

				AddSucceeded = UpdateNumericRange(&(SubTagTable->Entries[NewSubTagTableEntry]), 0, NumberOfWorkgroups - 1);
			}

			else
			{
			// Sub tag entry already exists; WOOF!
				AddSucceeded = false;
				ProcessError( NewSubTagTableEntry, SettingName );
			}
		}
		else
		{
		// Failed to create tag.
			AddSucceeded = false;
		}
	}
	else
	{
	// Active and / or default value is out of range.
		AddSucceeded = false;
	}

	return(AddSucceeded);
} // AddWorkgroupSelection






//
//	ProcessReportingCommand
//		Process the next '=' command.
//
void READER_CONFIGURATION_STORAGE::ProcessReportingCommand( HSTRING *ReportingCommand )
{
	HSTRING		OriginalReportingCommand( ReportingCommand, true );
	bool		ErrorsWereEncountered = false;							// No failures yet.



// Check for '='.
	if(( ReportingCommand->Size >= 1 ) && ( ReportingCommand->Char[ 0 ] == '=' ))
	{
		ReportingCommand->Remove( 0, 1 );
	}
	else
	{
		ErrorsWereEncountered = true;
	}


// Look for parameters.
	bool	ProcessAnotherParameter = ( ReportingCommand->Size >= 1 );
	while( ( !ErrorsWereEncountered ) && ( ProcessAnotherParameter ) )
	{
	// Is the next parameter a "CMP"?
		if( ReportingCommand->CompareSubstring( "CMP", 3, false ) == 0 )
		{
			TagCompressedModeEnabledForThisCommand = true;
			ReportingCommand->Remove( 0, 3 );
		}

	// Is the next parameter a "UNCMP"?
		else if( ReportingCommand->CompareSubstring( "UNCMP", 5, false ) == 0 )
		{
			TagCompressedModeEnabledForThisCommand = false;
			ReportingCommand->Remove( 0, 5 );
		}

	// Is the next parameter a "SORTALPHA"?
		else if( ReportingCommand->CompareSubstring( "SORTALPHA", 9, false ) == 0 )
		{
			SortWildcardQueriesForThisCommand = true;
			ReportingCommand->Remove( 0, 9 );
		}

	// Is the next parameter a "SORTCREATE"?
		else if( ReportingCommand->CompareSubstring( "SORTCREATE", 10, false ) == 0 )
		{
			SortWildcardQueriesForThisCommand = false;
			ReportingCommand->Remove( 0, 10 );
		}

	// Is the next parameter an "AT"?
		else if( ReportingCommand->CompareSubstring( "AT:", 3, false ) == 0 )
		{
		// Process an "AT" parameter, which itself has some parameters.  Since the "AT" has been specified, we'll turn off all reporting at the outset.
			ReportingCommand->Remove( 0, 3 );
			bool	ProcessAnotherATParameter = ( ReportingCommand->Size >= 1 );

			ReportBaseOnlySettingsForThisCommand = false;
			ReportScannerOnlySettingsForThisCommand = false;
			ReportCommonSettingsForThisCommand = false;
			WorkgroupSettingsHandlingForThisCommand = DO_NOT_REPORT_WORKGROUP_SETTINGS;

		// Look for AT parameters.
			while( ( !ErrorsWereEncountered ) && ( ProcessAnotherATParameter ) )
			{
			// Is the next AT parameter a "B"?
				if( ReportingCommand->CompareSubstring( "B", 1, false ) == 0 )
				{
					ReportBaseOnlySettingsForThisCommand = true;
					ReportingCommand->Remove( 0, 1 );
				}

			// Is the next AT parameter an "S"?
				else if( ReportingCommand->CompareSubstring( "S", 1, false ) == 0 )
				{
					ReportScannerOnlySettingsForThisCommand = true;
					ReportingCommand->Remove( 0, 1 );
				}

			// Is the next AT parameter a "C"?
				else if( ReportingCommand->CompareSubstring( "C", 1, false ) == 0 )
				{
					ReportCommonSettingsForThisCommand = true;
					ReportingCommand->Remove( 0, 1 );
				}

			// Is the next AT parameter an "E"?
				else if( ReportingCommand->CompareSubstring( "E", 1, false ) == 0 )
				{
					WorkgroupSettingsHandlingForThisCommand = REPORT_ALL_WORKGROUP_SETTINGS_IN_ALL_WORKGROUPS;
					ReportingCommand->Remove( 0, 1 );
				}

			// Is the next AT parameter an "AE"?
				else if( ReportingCommand->CompareSubstring( "AE", 2, false ) == 0 )
				{
					WorkgroupSettingsHandlingForThisCommand = REPORT_ALL_WORKGROUP_SETTINGS_IN_ALL_WORKGROUPS_WITH_COMMON_VALUES_FIRST_THEN_WITH_UNCOMMON_VALUES;
					ReportingCommand->Remove( 0, 2 );
				}

			// Is the next AT parameter an "A"?
				else if( ReportingCommand->CompareSubstring( "A", 1, false ) == 0 )
				{
					WorkgroupSettingsHandlingForThisCommand = REPORT_AND_SET_ALL_WORKGROUP_SETTINGS_IN_ALL_WORKGROUPS_WITH_COMMON_VALUES;
					ReportingCommand->Remove( 0, 1 );
				}

			// Is the next AT parameter a "D"?
				else if( ReportingCommand->CompareSubstring( "D", 1, false ) == 0 )
				{
					WorkgroupSettingsHandlingForThisCommand = REPORT_AND_SET_ALL_WORKGROUP_SETTINGS_IN_CURRENT_WORKGROUP;
					ChangeToWorkgroup( ActiveWorkgroupNumber, false );
					ReportingCommand->Remove( 0, 1 );
				}

			// Is the next AT parameter a "UD"?
				else if( ReportingCommand->CompareSubstring( "UD", 2, false ) == 0 )
				{
					WorkgroupSettingsHandlingForThisCommand = REPORT_AND_SET_ALL_WORKGROUP_SETTINGS_IN_CURRENT_WORKGROUP_THAT_ARE_NOT_COMMON_VALUES;
					ChangeToWorkgroup( ActiveWorkgroupNumber, false );
					ReportingCommand->Remove( 0, 2 );
				}


			// This must be a workgroup number.
				else
				{
					bool	MakeThisWorkgroupNumberPermanent = false;		// Haven't been told to make it permanent yet.
					WorkgroupSettingsHandlingForThisCommand = REPORT_AND_SET_ALL_WORKGROUP_SETTINGS_IN_CURRENT_WORKGROUP;

				// Is the workgroup number preceded by an optional "U"?
					if( ReportingCommand->CompareSubstring( "U", 1, false ) == 0 )
					{
						WorkgroupSettingsHandlingForThisCommand = REPORT_AND_SET_ALL_WORKGROUP_SETTINGS_IN_CURRENT_WORKGROUP_THAT_ARE_NOT_COMMON_VALUES;
						ReportingCommand->Remove( 0, 1 );
					}

				// Is the workgroup number preceded by an optional "P"?
					if( ReportingCommand->CompareSubstring( "P", 1, false ) == 0 )
					{
						MakeThisWorkgroupNumberPermanent = true;
						ReportingCommand->Remove( 0, 1 );
					}

				// Get the workgroup.
					int		NumberOfCharactersUsedInConversion;
					int		NewWorkgroupNumber;

					NewWorkgroupNumber = (int) ReportingCommand->ConvertToNumber(0, &NumberOfCharactersUsedInConversion);

					if( NumberOfCharactersUsedInConversion >= 1 )
					{
						if( ChangeToWorkgroup( NewWorkgroupNumber, MakeThisWorkgroupNumberPermanent ) )
						{
							ReportingCommand->Remove( 0, NumberOfCharactersUsedInConversion );
						}
						else
						{
							ErrorsWereEncountered = true;
						}
					}
					else
					{
						ErrorsWereEncountered = true;
					}
				}


			// If no errors were encountered and the next character is a '+', process another AT parameter.
				if(( !ErrorsWereEncountered ) && ( ReportingCommand->Size >= 1 ))
				{
					if( ReportingCommand->Char[ 0 ] == '+' )
					{
						ReportingCommand->Remove( 0, 1 );
					}
					else
					{
					// We have extraneous text without a '+'; WOOF!
						ErrorsWereEncountered = true;
					}
				}
				else
				{
					ProcessAnotherATParameter = false;
				}
			}
		}


	// Is the next parameter an "UP"?
		else if( ReportingCommand->CompareSubstring( "UP:", 3, false ) == 0 )
		{
		// Process a "UP" parameter, which itself has some parameters.  Since the "UP" has been specified, we'll turn off all reporting at the outset.
			ReportingCommand->Remove( 0, 3 );
			bool	ProcessAnotherUPParameter = ( ReportingCommand->Size >= 1 );

			ReportSettingsUpdatedSinceLastCommandForThisCommand = true;
			ReportSettingsNotUpdatedSinceLastCommandForThisCommand = true;
			ReportSettingsUpdatedSinceStartupForThisCommand = true;
			ReportSettingsNotUpdatedSinceStartupForThisCommand = true;
			ReportSettingsWhenActiveIsEqualToWorkingForThisCommand = true;
			ReportSettingsWhenActiveIsNotEqualToWorkingForThisCommand = true;
			SettingsUpdatedSinceLastCommandModifiersForThisCommand = SETTINGS_UPDATED_SINCE_LAST_COMMAND_NO_MODIFIERS;

		// Look for UP parameters.
			while( ( !ErrorsWereEncountered ) && ( ProcessAnotherUPParameter ) )
			{
			    // Is the next UP parameter an "SLC" or "NSLC"?
				if(( ReportingCommand->CompareSubstring( "SLC", 3, false ) == 0 ) ||
						( ReportingCommand->CompareSubstring( "NSLC", 4, false ) == 0 ))
				{
					SettingsUpdatedSinceLastCommandModifiersForThisCommand = SETTINGS_UPDATED_SINCE_LAST_COMMAND_NO_MODIFIERS;
					if( ReportingCommand->CompareSubstring( "N", 1, false ) == 0 )
					{
						ReportSettingsUpdatedSinceLastCommandForThisCommand = false;
						ReportSettingsNotUpdatedSinceLastCommandForThisCommand = true;
						ReportingCommand->Remove( 0, 1 );
					}
					else
					{
						ReportSettingsUpdatedSinceLastCommandForThisCommand = true;
						ReportSettingsNotUpdatedSinceLastCommandForThisCommand = false;
					}
					ReportingCommand->Remove( 0, 3 );

				// Are there any modifiers?
					if( ReportingCommand->CompareSubstring( "WU", 2, false ) == 0 )
					{
						SettingsUpdatedSinceLastCommandModifiersForThisCommand =
										SETTINGS_UPDATED_SINCE_LAST_COMMAND_EXCLUDE_IF_WORKING_NOT_UPDATED;
						ReportingCommand->Remove( 0, 2 );
					}
					else if( ReportingCommand->CompareSubstring( "AU", 2, false ) == 0 )
					{
						SettingsUpdatedSinceLastCommandModifiersForThisCommand =
										SETTINGS_UPDATED_SINCE_LAST_COMMAND_EXCLUDE_IF_ACTIVE_NOT_UPDATED;
						ReportingCommand->Remove( 0, 2 );
					}
					else if( ReportingCommand->CompareSubstring( "NWU", 3, false ) == 0 )
					{
						SettingsUpdatedSinceLastCommandModifiersForThisCommand =
										SETTINGS_UPDATED_SINCE_LAST_COMMAND_EXCLUDE_IF_WORKING_UPDATED;
						ReportingCommand->Remove( 0, 3 );
					}
					else if( ReportingCommand->CompareSubstring( "NAU", 3, false ) == 0 )
					{
						SettingsUpdatedSinceLastCommandModifiersForThisCommand =
										SETTINGS_UPDATED_SINCE_LAST_COMMAND_EXCLUDE_IF_ACTIVE_UPDATED;
						ReportingCommand->Remove( 0, 3 );
					}
				}

			// Is the next UP parameter an "SS"?
				else if( ReportingCommand->CompareSubstring( "SS", 2, false ) == 0 )
				{
					ReportSettingsUpdatedSinceStartupForThisCommand = true;
					ReportSettingsNotUpdatedSinceStartupForThisCommand = false;
					ReportingCommand->Remove( 0, 2 );
				}

			// Is the next UP parameter an "NSS"?
				else if( ReportingCommand->CompareSubstring( "NSS", 3, false ) == 0 )
				{
					ReportSettingsUpdatedSinceStartupForThisCommand = false;
					ReportSettingsNotUpdatedSinceStartupForThisCommand = true;
					ReportingCommand->Remove( 0, 3 );
				}

			// Is the next UP parameter an "AEW"?
				else if( ReportingCommand->CompareSubstring( "AEW", 3, false ) == 0 )
				{
					ReportSettingsWhenActiveIsEqualToWorkingForThisCommand = true;
					ReportSettingsWhenActiveIsNotEqualToWorkingForThisCommand = false;
					ReportingCommand->Remove( 0, 3 );
				}

			// Is the next UP parameter an "NAEW"?
				else if( ReportingCommand->CompareSubstring( "NAEW", 4, false ) == 0 )
				{
					ReportSettingsWhenActiveIsEqualToWorkingForThisCommand = false;
					ReportSettingsWhenActiveIsNotEqualToWorkingForThisCommand = true;
					ReportingCommand->Remove( 0, 4 );
				}
				/* Is the next UP parameter a "A"?   Important, this has to be last, so it does not get in the way of the
				 * AEW command above. Move it down to fix bug where AEW did not work because first A in AEW as being treated
				 * as an A below, move it down to bottom and that is not an issue...Talked to Barry on this*/
				else if( ReportingCommand->CompareSubstring( "A", 1, false ) == 0 )
				{
					ReportSettingsUpdatedSinceLastCommandForThisCommand = true;
					ReportSettingsNotUpdatedSinceLastCommandForThisCommand = true;
					ReportSettingsUpdatedSinceStartupForThisCommand = true;
					ReportSettingsNotUpdatedSinceStartupForThisCommand = true;
					ReportSettingsWhenActiveIsEqualToWorkingForThisCommand = true;
					ReportSettingsWhenActiveIsNotEqualToWorkingForThisCommand = true;
					SettingsUpdatedSinceLastCommandModifiersForThisCommand = SETTINGS_UPDATED_SINCE_LAST_COMMAND_NO_MODIFIERS;
					ReportingCommand->Remove( 0, 1 );
				}
			// We don't recognize the parameter; WOOF.
				else
				{
					ErrorsWereEncountered = true;
				}


			// If no errors were encountered and the next character is a '+', process another AT parameter.
				if(( !ErrorsWereEncountered ) && ( ReportingCommand->Size >= 1 ))
				{
					if( ReportingCommand->Char[ 0 ] == '+' )
					{
						ReportingCommand->Remove( 0, 1 );
					}
					else
					{
					// We have extraneous text without a '+'; WOOF!
						ErrorsWereEncountered = true;
					}
				}
				else
				{
					ProcessAnotherUPParameter = false;
				}
			}
		}

	// Is the next parameter "HIDDEN"?
		else if( ReportingCommand->CompareSubstring( "HIDDEN", 6, false ) == 0 )
		{
			ReportingCommand->Remove( 0, 6 );
//****Fix this, since we're not pulling commands out properly.  May want to store command up to ',' to ReportingCommand at the
//****beginning of each cycle through the loop.
			bool	ProcessAnotherHiddenParameter = false;
			int		HiddenSettingClasssesAboveThisOne;

			if( ReportingCommand->CompareSubstring( ":", 1, false ) == 0 )
			{
				ReportingCommand->Remove( 0, 1 );
				ProcessAnotherHiddenParameter = true;
			}

		// If there are no parameters, turn everything on, else parse the parameters.
			if( ProcessAnotherHiddenParameter )
			{
				ReportHiddenSettingsForThisCommand = 0;

			// Look for HIDDEN parameters.
				while( ( !ErrorsWereEncountered ) && ( ProcessAnotherHiddenParameter ) )
				{
				// Is the next HIDDEN parameter an "N"?
					if( ReportingCommand->CompareSubstring( "N", 1, false ) == 0 )
					{
						ReportingCommand->Remove( 0, 1 );
						ReportHiddenSettingsForThisCommand |= HIDDEN_SETTINGS_REPORT_INVISIBLE;
						HiddenSettingClasssesAboveThisOne = HIDDEN_SETTINGS_REPORT_VISIBLE_BY_SUB_TAG |
															HIDDEN_SETTINGS_REPORT_VISIBLE_BY_TAG |
															HIDDEN_SETTINGS_REPORT_FULLY_VISIBLE;
					}

				// Is the next HIDDEN parameter an "S"?
					else if( ReportingCommand->CompareSubstring( "S", 1, false ) == 0 )
					{
						ReportingCommand->Remove( 0, 1 );
						ReportHiddenSettingsForThisCommand |= HIDDEN_SETTINGS_REPORT_VISIBLE_BY_SUB_TAG;
						HiddenSettingClasssesAboveThisOne = HIDDEN_SETTINGS_REPORT_VISIBLE_BY_TAG |
															HIDDEN_SETTINGS_REPORT_FULLY_VISIBLE;
					}

				// Is the next HIDDEN parameter a "T"?
					else if( ReportingCommand->CompareSubstring( "T", 1, false ) == 0 )
					{
						ReportingCommand->Remove( 0, 1 );
						ReportHiddenSettingsForThisCommand |= HIDDEN_SETTINGS_REPORT_VISIBLE_BY_TAG;
						HiddenSettingClasssesAboveThisOne = HIDDEN_SETTINGS_REPORT_FULLY_VISIBLE;
					}

				// Is the next HIDDEN parameter an "F"?
					else if( ReportingCommand->CompareSubstring( "F", 1, false ) == 0 )
					{
						ReportingCommand->Remove( 0, 1 );
						ReportHiddenSettingsForThisCommand |= HIDDEN_SETTINGS_REPORT_FULLY_VISIBLE;
						HiddenSettingClasssesAboveThisOne = 0;
					}

				// We don't recognize the parameter; WOOF.
					else
					{
						ErrorsWereEncountered = true;
					}


				// If no errors were encountered and the next character is a '>', OR HiddenSettingClasssesAboveThisOne.
					if(( !ErrorsWereEncountered ) && ( ReportingCommand->Size >= 1 ))
					{
						if( ReportingCommand->Char[ 0 ] == '>' )
						{
							ReportingCommand->Remove( 0, 1 );
							ReportHiddenSettingsForThisCommand |= HiddenSettingClasssesAboveThisOne;
						}
					}


				// If no errors were encountered and the next character is a '+', process another AT parameter.
					if(( !ErrorsWereEncountered ) && ( ReportingCommand->Size >= 1 ))
					{
						if( ReportingCommand->Char[ 0 ] == '+' )
						{
							ReportingCommand->Remove( 0, 1 );
						}
						else
						{
						// We have extraneous text without a '+'; WOOF!
							ErrorsWereEncountered = true;
						}
					}
					else
					{
						ProcessAnotherHiddenParameter = false;
					}
				}
			}

			else
			{
				ReportHiddenSettingsForThisCommand = 0xFFFFFFFF;
			}
		}

	// Is the next parameter "NOHIDDEN"?
		else if( ReportingCommand->CompareSubstring( "NOHIDDEN", 8, false ) == 0 )
		{
			ReportHiddenSettingsForThisCommand = 0;
			ReportingCommand->Remove( 0, 8 );
		}

	// Unrecognized parameter; WOOF!
		else
		{
			ErrorsWereEncountered = true;
		}


	// If no errors were encountered and the next character is a ',', process another parameter.
		if(( !ErrorsWereEncountered ) && ( ReportingCommand->Size >= 1 ))
		{
			if( ReportingCommand->Char[ 0 ] == ',' )
			{
				ReportingCommand->Remove( 0, 1 );
			}
			else
			{
			// We have extraneous text without a comma; WOOF!
				ErrorsWereEncountered = true;
			}
		}
		else
		{
			ProcessAnotherParameter = false;
		}
	}


// Copy the original reporting command back as a response.  If any errors were encountered, append a NAK.
	ReportingCommand->Copy( &OriginalReportingCommand);
	if( ErrorsWereEncountered )
	{
		ReportingCommand->AppendNumber(((unsigned int) NAK), 1);
	}

	return;
} // ProcessReportingCommand






//
//	QuerySetting
//		Return the setting value in reportable form for the specified sub-tag table entry and affected table.
//
void READER_CONFIGURATION_STORAGE::QuerySetting(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntry,
							const char AffectedTable, HSTRING *SettingValue, const char Action, bool ReturnSettableString )
{
	char	temp[80];


// Return either the customer default value, stored value, or active value depending on the AffectedTable field
// sent by the caller.
	switch(AffectedTable)
	{
		case '.':
			{
			SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*WorkingValue = WorkingValueForCurrentWorkgroup( SubTagTableEntry );

		// Return the stored value, depending upon the type of setting we're looking at.
			if((SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING) ||
					(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING_LIST))
			{
				SettingValue->Copy("");
				if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING_MUST_BE_QUOTED)
				{
					SettingValue->AppendChar('"');
				}
				SettingValue->Append( WorkingValue->c);
				if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING_MUST_BE_QUOTED)
				{
					SettingValue->AppendChar('"');
				}
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BINARY_STRING)
			{
				SettingValue->Copy("");
				SettingValue->Append( WorkingValue->BinaryString);
				SettingValue->UnprintableCharactersToFormats();
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::ASCII_STRING)
			{
				SettingValue->Copy("");
				SettingValue->Append( WorkingValue->BinaryString);
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::UNICODE_STRING)
			{
				SettingValue->Copy("");
				SettingValue->Append( WorkingValue->BinaryString->Char, WorkingValue->BinaryString->Size);
				SettingValue->UniCodeToSimpleAsciiFormat();
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::HEX_STRING)
			{
				SettingValue->Copy("");
				SettingValue->Append( WorkingValue->BinaryString);
				SettingValue->BinaryToHex();
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::FORMAT_STRING)
			{
				WorkingValue->DataFormat->BuildReportString(SettingValue, ReturnSettableString,
									ReturnSettableString ? true : OmitTrailingPipeOnValueQueries);
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::PREFIX_SUFFIX_STRING)
			{
				WorkingValue->PrefixSuffix->BuildReportString(SettingValue, ReturnSettableString,
									ReturnSettableString ? true : OmitTrailingPipeOnValueQueries);
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BOOLEAN)
			{
				if(WorkingValue->b)
				{
					SettingValue->Copy("1");
				}
				else
				{
					SettingValue->Copy("0");
				}
			}

			else if((SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::NUMERIC) ||
						(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::NUMERIC_LIST) ||
						(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SELECTION))
			{
				if(( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::TWO_DIGIT_HEX ) &&
						(( Action == '$' ) || ( Action == '~' )))
				{
					sprintf(temp, "%02X", WorkingValue->i);
				}
				else
				{
					sprintf(temp, "%d", WorkingValue->i);
				}

				SettingValue->Copy(temp);
			}
			}
			break;


		case '^':
		case '@':
		// Return the default value, depending upon the type of setting we're looking at.
			if((SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING) ||
					(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING_LIST))
			{
				SettingValue->Copy("");
				if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING_MUST_BE_QUOTED)
				{
					SettingValue->AppendChar('"');
				}
				SettingValue->Append(SubTagTableEntry->DefaultValue.c);
				if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING_MUST_BE_QUOTED)
				{
					SettingValue->AppendChar('"');
				}
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BINARY_STRING)
			{
				SettingValue->Copy("");
				SettingValue->Append(SubTagTableEntry->DefaultValue.BinaryString);
				SettingValue->UnprintableCharactersToFormats();
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::ASCII_STRING)
			{
				SettingValue->Copy("");
				SettingValue->Append(SubTagTableEntry->DefaultValue.BinaryString);
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::UNICODE_STRING)
			{
				SettingValue->Copy("");
				SettingValue->Append(SubTagTableEntry->DefaultValue.BinaryString->Char,SubTagTableEntry->DefaultValue.BinaryString->Size);
				SettingValue->UniCodeToSimpleAsciiFormat();
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::HEX_STRING)
			{
				SettingValue->Copy("");
				SettingValue->Append(SubTagTableEntry->DefaultValue.BinaryString);
				SettingValue->BinaryToHex();
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::FORMAT_STRING)
			{
				SubTagTableEntry->DefaultValue.DataFormat->BuildReportString(SettingValue, ReturnSettableString,
									ReturnSettableString ? true : OmitTrailingPipeOnValueQueries);
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::PREFIX_SUFFIX_STRING)
			{
				SubTagTableEntry->DefaultValue.PrefixSuffix->BuildReportString(SettingValue, ReturnSettableString,
									ReturnSettableString ? true : OmitTrailingPipeOnValueQueries);
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BOOLEAN)
			{
				if(SubTagTableEntry->DefaultValue.b)
				{
					SettingValue->Copy("1");
				}
				else
				{
					SettingValue->Copy("0");
				}
			}

			else if((SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::NUMERIC) ||
						(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::NUMERIC_LIST) ||
						(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SELECTION))
			{
				if(( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::TWO_DIGIT_HEX ) &&
						(( Action == '$' ) || ( Action == '~' )))
				{
					sprintf(temp, "%02X", SubTagTableEntry->DefaultValue.i);
				}
				else
				{
					sprintf(temp, "%d", SubTagTableEntry->DefaultValue.i);
				}

				SettingValue->Copy(temp);
			}
			break;


		case '&':
		// Return the standard default value, depending upon the type of setting we're looking at.
			if((SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING) ||
					(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING_LIST))
			{
				SettingValue->Copy("");
				if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING_MUST_BE_QUOTED)
				{
					SettingValue->AppendChar('"');
				}
				SettingValue->Append(SubTagTableEntry->StandardDefaultValue.c);
				if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING_MUST_BE_QUOTED)
				{
					SettingValue->AppendChar('"');
				}
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BINARY_STRING)
			{
				SettingValue->Copy("");
				SettingValue->Append(SubTagTableEntry->StandardDefaultValue.BinaryString);
				SettingValue->UnprintableCharactersToFormats();
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::ASCII_STRING)
			{
				SettingValue->Copy("");
				SettingValue->Append(SubTagTableEntry->StandardDefaultValue.BinaryString);
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::UNICODE_STRING)
			{
				SettingValue->Copy("");
				SettingValue->Append(SubTagTableEntry->StandardDefaultValue.BinaryString->Char,SubTagTableEntry->StandardDefaultValue.BinaryString->Size);
				SettingValue->UniCodeToSimpleAsciiFormat();
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::HEX_STRING)
			{
				SettingValue->Copy("");
				SettingValue->Append(SubTagTableEntry->StandardDefaultValue.BinaryString);
				SettingValue->BinaryToHex();
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::FORMAT_STRING)
			{
				SubTagTableEntry->StandardDefaultValue.DataFormat->BuildReportString(SettingValue, ReturnSettableString,
									ReturnSettableString ? true : OmitTrailingPipeOnValueQueries);
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::PREFIX_SUFFIX_STRING)
			{
				SubTagTableEntry->StandardDefaultValue.PrefixSuffix->BuildReportString(SettingValue, ReturnSettableString,
									ReturnSettableString ? true : OmitTrailingPipeOnValueQueries);
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BOOLEAN)
			{
				if(SubTagTableEntry->StandardDefaultValue.b)
				{
					SettingValue->Copy("1");
				}
				else
				{
					SettingValue->Copy("0");
				}
			}

			else if((SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::NUMERIC) ||
						(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::NUMERIC_LIST) ||
						(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SELECTION))
			{
				if(( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::TWO_DIGIT_HEX ) &&
						(( Action == '$' ) || ( Action == '~' )))
				{
					sprintf(temp, "%02X", SubTagTableEntry->StandardDefaultValue.i);
				}
				else
				{
					sprintf(temp, "%d", SubTagTableEntry->StandardDefaultValue.i);
				}

				SettingValue->Copy(temp);
			}

			break;

		case '!':
		default:
		// Return the active value, depending upon the type of setting we're looking at.
			if((SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING) ||
					(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING_LIST))
			{
				SettingValue->Copy("");
				if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING_MUST_BE_QUOTED)
				{
					SettingValue->AppendChar('"');
				}
				SettingValue->Append(SubTagTableEntry->InstantiatorCopyOfActiveValue.c);
				if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING_MUST_BE_QUOTED)
				{
					SettingValue->AppendChar('"');
				}
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BINARY_STRING)
			{
				SettingValue->Copy("");
				SettingValue->Append(SubTagTableEntry->InstantiatorCopyOfActiveValue.BinaryString);
				SettingValue->UnprintableCharactersToFormats();
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::ASCII_STRING)
			{
				SettingValue->Copy("");
				SettingValue->Append(SubTagTableEntry->InstantiatorCopyOfActiveValue.BinaryString);
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::UNICODE_STRING)
			{
				SettingValue->Copy("");
				SettingValue->Append(SubTagTableEntry->InstantiatorCopyOfActiveValue.BinaryString->Char,SubTagTableEntry->InstantiatorCopyOfActiveValue.BinaryString->Size);
				SettingValue->UniCodeToSimpleAsciiFormat();
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::HEX_STRING)
			{
				SettingValue->Copy("");
				SettingValue->Append(SubTagTableEntry->InstantiatorCopyOfActiveValue.BinaryString);
				SettingValue->BinaryToHex();
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::FORMAT_STRING)
			{
				SubTagTableEntry->InstantiatorCopyOfActiveValue.DataFormat->BuildReportString(SettingValue, ReturnSettableString,
									ReturnSettableString ? true : OmitTrailingPipeOnValueQueries);
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::PREFIX_SUFFIX_STRING)
			{
				SubTagTableEntry->InstantiatorCopyOfActiveValue.PrefixSuffix->BuildReportString(SettingValue, ReturnSettableString,
									ReturnSettableString ? true : OmitTrailingPipeOnValueQueries);
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BOOLEAN)
			{
				if(*(SubTagTableEntry->InstantiatorCopyOfActiveValue.b))
				{
					SettingValue->Copy("1");
				}
				else
				{
					SettingValue->Copy("0");
				}
			}

			else if((SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::NUMERIC) ||
						(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::NUMERIC_LIST) ||
						(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SELECTION))
			{
				if(( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::TWO_DIGIT_HEX ) &&
						(( Action == '$' ) || ( Action == '~' )))
				{
					sprintf(temp, "%02X", *(SubTagTableEntry->InstantiatorCopyOfActiveValue.i));
				}
				else
				{
					sprintf(temp, "%d", *(SubTagTableEntry->InstantiatorCopyOfActiveValue.i));
				}

				SettingValue->Copy(temp);
			}
			break;
	}


	return;
} // QuerySetting






//
//	QuerySettingRange
//		Return the setting range in reportable form for the specified sub-tag table entry and affected table.
//
void READER_CONFIGURATION_STORAGE::QuerySettingRange(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntry,
							const char RangeAction, HSTRING *RangeSettingValue)
{
	int		i;
	char	temp[80];


// Return the range, depending upon the type of setting we're looking at.
	if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING)
	{
	// Return 00-FF for string.
		RangeSettingValue->Copy("00-FF");
	}

	else if((SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BINARY_STRING) ||
			   (SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::ASCII_STRING) ||
			   (SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::HEX_STRING) ||
				 (SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::UNICODE_STRING))
	{
	// Return 00-FF for string.
		RangeSettingValue->Copy("00-FF");
	}

	else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::FORMAT_STRING)
	{
	// Return 00-FF for string.
		RangeSettingValue->Copy("00-FF");
	}

	else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::PREFIX_SUFFIX_STRING)
	{
	// Return 00-FF for string.
		RangeSettingValue->Copy("00-FF");
	}

	else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BOOLEAN)
	{
	// Return 0-1 for boolean.
		RangeSettingValue->Copy("0-1");
	}

	else if((SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::NUMERIC) ||
			(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SELECTION))
	{
	// Return range for numeric.
		if(RangeAction == '*')
		{
			sprintf(temp, "%d-%d", SubTagTableEntry->LowRange, SubTagTableEntry->HighRange);
		}
		else
		{
		// For minimum, display the greater of the absolute minimum or the minimum cross dependency.
		// For maximum, display the lesser of the absolute maximum or the maximum cross dependency.
			sprintf(temp, "%d-%d",
					(*(SubTagTableEntry->WorkingLowRangeAddress) > SubTagTableEntry->LowRange)
									? *(SubTagTableEntry->WorkingLowRangeAddress) : SubTagTableEntry->LowRange,
					(*(SubTagTableEntry->WorkingHighRangeAddress) < SubTagTableEntry->HighRange)
									? *(SubTagTableEntry->WorkingHighRangeAddress) : SubTagTableEntry->HighRange);
		}
		RangeSettingValue->Copy(temp);
	}

	else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::NUMERIC_LIST)
	{
	// Return range for list.
		if(SubTagTableEntry->NumberOfEntriesInListRange > 0)
		{
			sprintf(temp, "%d", SubTagTableEntry->NumericListRange[0]);
			RangeSettingValue->Copy(temp);

			for(i = 1;    i < SubTagTableEntry->NumberOfEntriesInListRange;    ++i)
			{
				sprintf(temp, "|%d", SubTagTableEntry->NumericListRange[i]);
				RangeSettingValue->Append(temp);
			}

			if((RangeAction == '*') && (!OmitTrailingPipeOnRangeQueries))
			{
				RangeSettingValue->Append("|");
			}
		}
	}

	else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING_LIST)
	{
	// Return range for list.
		if(SubTagTableEntry->NumberOfEntriesInListRange > 0)
		{
			RangeSettingValue->CopyChar( '"' );
			RangeSettingValue->Append(SubTagTableEntry->StringListRange[0]);
			RangeSettingValue->AppendChar( '"' );

			for(i = 1;    i < SubTagTableEntry->NumberOfEntriesInListRange;    ++i)
			{
				RangeSettingValue->Append("|");
				RangeSettingValue->AppendChar( '"' );
				RangeSettingValue->Append(SubTagTableEntry->StringListRange[i]);
				RangeSettingValue->AppendChar( '"' );
			}

			if((RangeAction == '*') && (!OmitTrailingPipeOnRangeQueries))
			{
				RangeSettingValue->Append("|");
			}
		}
	}


	return;
} // QuerySettingRange








//
//	ValidateAndChangeSetting
//		Verify that the command text for this setting is OK.  If we're to only look for part of the data, we'll also tell
//		the caller if the maximum command text length has been reached.
//
int READER_CONFIGURATION_STORAGE::ValidateAndChangeSetting(SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntry,
		const char *CommandText, int CommandTextLength, const char AffectedTable, bool SaveTheResult, bool OverrideFixedFlag)
{
	int			Status = RDR_CFG_SETTING_INVALID;		// Assume it's invalid until we know otherwise.
	HSTRING		CommandTextCopy(((unsigned char *) CommandText), CommandTextLength, true);
	int			i;



	SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*WorkingValue = WorkingValueForCurrentWorkgroup( SubTagTableEntry );

// If the table character is invalid, WOOF!.
	if((AffectedTable != '.') && (AffectedTable != '!') && (AffectedTable != '@') && (AffectedTable != '^'))
	{
		return(RDR_CFG_SETTING_INVALID);
	}


// Process the command text.
// Don't process this sub-tag if it is not marked visible and we're not supposed to show hidden entries, if the
// action is a '$' query and this sub-tag is a command, or if the action is a '~' and this is a command.
	if((OverrideFixedFlag) ||
				(( SettingChangesEnabled ) &&
					((SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::FULLY_VISIBLE) ||
					(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::VISIBLE_BY_TAG) ||
					(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG))))
	{
	// Don't process a fixed value unless it is a command.
		if((SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::COMMAND) ||
			(OverrideFixedFlag) ||
			(!(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::FIXED) &&
				(!(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::FIXED_EXCEPT_ACTIVE) || (AffectedTable == '!'))))
		{
		// We're supposed to set the value with the remainder of the command string.
			if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING)
			{
			// No text yet means that it may be a partial.
				if(CommandTextCopy.Size == 0)
				{
					if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING_MUST_BE_QUOTED)
					{
//						Status = RDR_CFG_PARTIAL_SETTING_VALID;
						Status = RDR_CFG_SETTING_VALID;			// Quotes are no longer required, since there could be HEX numbers outside
																//	the quotes.
					}
					else if(CommandTextCopy.Size == SubTagTableEntry->MaximumSizeOfActiveValue)
					{
						Status = RDR_CFG_SETTING_VALID_MAX_LENGTH;
					}
					else
					{
						Status = RDR_CFG_SETTING_VALID;
					}
				}

			// Handle quoted strings as a special case.
				else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING_MUST_BE_QUOTED)
				{
					bool	IncompleteString;

					if( CommandTextCopy.QuotedHEXStringToBinary( &IncompleteString ))
					{
					// If the size of the new data is too large, return an error.
						if( CommandTextCopy.Size <= ( SubTagTableEntry->MaximumSizeOfActiveValue ))
						{
						// If we haven't exceeded the maximum number of digits, and the result was incomplete, report it.
							if( IncompleteString )
							{
								Status = RDR_CFG_PARTIAL_SETTING_VALID;
							}

							else
							{
							// Set the valid status depending on whether or not we've reached the most characters we can receive.
								if( CommandTextCopy.Size == SubTagTableEntry->MaximumSizeOfActiveValue )
								{
									Status = RDR_CFG_SETTING_VALID_MAX_LENGTH;
								}
								else
								{
									Status = RDR_CFG_SETTING_VALID;
								}
							}
						}
					}
/*				// If the size of the new data is too large, return an error.
					if(CommandTextCopy.Size <= (SubTagTableEntry->MaximumSizeOfActiveValue + 2))
					{
					// If we have quotes at either end, we're done.
						if((CommandTextCopy.Size >= 2) && (CommandTextCopy.Char[0] == '"') &&
														(CommandTextCopy.Char[CommandTextCopy.Size - 1] == '"'))
						{
						// Set the valid status depending on whether or not we've reached the most characters we can receive.
							if(CommandTextCopy.Size == (SubTagTableEntry->MaximumSizeOfActiveValue + 2))
							{
								Status = RDR_CFG_SETTING_VALID_MAX_LENGTH;
							}
							else
							{
								Status = RDR_CFG_SETTING_VALID;
							}
						}

					// If we haven't reached the maximum number of digits, we haven't passed quote test, and we find a quote
					// at the beginning of the string, we'll indicate that the command is partially complete.
						else if((CommandTextCopy.Size != (SubTagTableEntry->MaximumSizeOfActiveValue + 2)) &&
														(CommandTextCopy.Char[0] == '"'))
						{
							Status = RDR_CFG_PARTIAL_SETTING_VALID;
						}
					}
*/
				}

			// Unquoted string is always OK, so long as it doesn't exceed maximum length.
				else
				{
				// Set the valid status depending on whether or not we've reached the most characters we can receive.
					if(CommandTextCopy.Size == SubTagTableEntry->MaximumSizeOfActiveValue )
					{
						Status = RDR_CFG_SETTING_VALID_MAX_LENGTH;
					}
					else if(CommandTextCopy.Size < SubTagTableEntry->MaximumSizeOfActiveValue )
					{
						Status = RDR_CFG_SETTING_VALID;
					}
				}


			// If we have a valid setting and we're supposed to save it, do it now.
				if((Status > 0) && (SaveTheResult))
				{
				// If the string is quoted, remove the quotes first.  NO LONGER NECESSARY, AS THEY WILL ALREADY HAVE BEEN REMOVED.
/*					if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING_MUST_BE_QUOTED)
					{
						CommandTextCopy.Remove(CommandTextCopy.Size - 1, 1);
						CommandTextCopy.Remove(0, 1);
					}
*/

				// Store the string value to the appropriate table and return an ACK.
					switch(AffectedTable)
					{
						case '.':
						// For stored value update, change the active value as well.
							CommandTextCopy.CopyFromString
											(((unsigned char *) WorkingValue->c), CommandTextCopy.Size + 1);
							CommandTextCopy.CopyFromString
											(((unsigned char *) SubTagTableEntry->InstantiatorCopyOfActiveValue.c), CommandTextCopy.Size + 1);
							WorkingValue->Updated = true;
							WorkingValue->ModifiedSinceInstantiation = true;
							SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated = true;
							SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
							OneOrMoreWorkingValuesUpdated = true;
							OneOrMoreActiveValuesUpdated = true;

							if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
							{
								OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated = true;
								OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
								OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
								OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
							{
								OneOrMoreBaseOnlyValuesInWorkingStorageWereUpdated = true;
								OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
							{
								OneOrMoreScannerOnlyValuesInWorkingStorageWereUpdated = true;
								OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
							}
							else
							{
								OneOrMoreCommonValuesInWorkingStorageWereUpdated = true;
								OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
							}
							break;

						case '^':
						case '@':
							CommandTextCopy.CopyFromString
											(((unsigned char *) SubTagTableEntry->DefaultValue.c), CommandTextCopy.Size + 1);
							SubTagTableEntry->DefaultValue.Updated = true;
							SubTagTableEntry->DefaultValue.ModifiedSinceInstantiation = true;
							OneOrMoreDefaultsUpdated = true;
							break;

						case '!':
							CommandTextCopy.CopyFromString
											(((unsigned char *) SubTagTableEntry->InstantiatorCopyOfActiveValue.c), CommandTextCopy.Size + 1);
							SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated = true;
							SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
							OneOrMoreActiveValuesUpdated = true;

							if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
							{
								OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
								OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
							{
								OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
							{
								OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
							}
							else
							{
								OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
							}
							break;

						default:
							Status = RDR_CFG_SETTING_INVALID;
							break;
					}
				}
			}





			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BINARY_STRING)
			{
			// No text yet means that it may be a partial.
				if(CommandTextCopy.Size == 0)
				{
					Status = RDR_CFG_SETTING_VALID;
				}

			// Always OK, so long as it doesn't exceed maximum length.
				else
				{
					CommandTextCopy.UnprintableFormatsToCharacters();
				// Set the valid status depending on whether or not we've reached the most characters we can receive.
					if(CommandTextCopy.Size == SubTagTableEntry->MaximumSizeOfActiveValue )
					{
						Status = RDR_CFG_SETTING_VALID_MAX_LENGTH;
					}
					else if(CommandTextCopy.Size < SubTagTableEntry->MaximumSizeOfActiveValue )
					{
						Status = RDR_CFG_SETTING_VALID;
					}
				}


			// If we have a valid setting and we're supposed to save it, do it now.
				if((Status > 0) && (SaveTheResult))
				{
				// Store the string value to the appropriate table and return an ACK.
					switch(AffectedTable)
					{
						case '.':
						// For stored value update, change the active value as well.
							WorkingValue->BinaryString->Copy(&CommandTextCopy);
							SubTagTableEntry->InstantiatorCopyOfActiveValue.BinaryString->Copy(&CommandTextCopy);
							WorkingValue->Updated = true;
							WorkingValue->ModifiedSinceInstantiation = true;
							SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated = true;
							SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
							OneOrMoreWorkingValuesUpdated = true;
							OneOrMoreActiveValuesUpdated = true;

							if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
							{
								OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated = true;
								OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
								OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
								OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
							{
								OneOrMoreBaseOnlyValuesInWorkingStorageWereUpdated = true;
								OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
							{
								OneOrMoreScannerOnlyValuesInWorkingStorageWereUpdated = true;
								OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
							}
							else
							{
								OneOrMoreCommonValuesInWorkingStorageWereUpdated = true;
								OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
							}
							break;

						case '^':
						case '@':
							SubTagTableEntry->DefaultValue.BinaryString->Copy(&CommandTextCopy);
							SubTagTableEntry->DefaultValue.Updated = true;
							SubTagTableEntry->DefaultValue.ModifiedSinceInstantiation = true;
							OneOrMoreDefaultsUpdated = true;
							break;

						case '!':
							SubTagTableEntry->InstantiatorCopyOfActiveValue.BinaryString->Copy(&CommandTextCopy);
							SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated = true;
							SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
							OneOrMoreActiveValuesUpdated = true;

							if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
							{
								OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
								OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
							{
								OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
							{
								OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
							}
							else
							{
								OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
							}
							break;

						default:
							Status = RDR_CFG_SETTING_INVALID;
							break;
					}
				}
			}





			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::ASCII_STRING)
			{
			// No text yet means that it may be a partial.
				if(CommandTextCopy.Size == 0)
				{
					Status = RDR_CFG_SETTING_VALID;
				}

			// Always OK, so long as it doesn't exceed maximum length.
				else
				{
				// Set the valid status depending on whether or not we've reached the most characters we can receive.
					if( SubTagTableEntry->MaximumSizeOfActiveValue == 0 )
					{
						Status = RDR_CFG_SETTING_VALID;
					}
					else if(CommandTextCopy.Size == SubTagTableEntry->MaximumSizeOfActiveValue )
					{
						Status = RDR_CFG_SETTING_VALID_MAX_LENGTH;
					}
					else if(CommandTextCopy.Size < SubTagTableEntry->MaximumSizeOfActiveValue )
					{
						Status = RDR_CFG_SETTING_VALID;
					}
				}


			// If we have a valid setting and we're supposed to save it, do it now.
				if((Status > 0) && (SaveTheResult))
				{
				// Store the string value to the appropriate table and return an ACK.
					switch(AffectedTable)
					{
						case '.':
						// For stored value update, change the active value as well.
							WorkingValue->BinaryString->Copy(&CommandTextCopy);
							SubTagTableEntry->InstantiatorCopyOfActiveValue.BinaryString->Copy(&CommandTextCopy);
							WorkingValue->Updated = true;
							WorkingValue->ModifiedSinceInstantiation = true;
							SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated = true;
							SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
							OneOrMoreWorkingValuesUpdated = true;
							OneOrMoreActiveValuesUpdated = true;

							if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
							{
								OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated = true;
								OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
								OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
								OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
							{
								OneOrMoreBaseOnlyValuesInWorkingStorageWereUpdated = true;
								OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
							{
								OneOrMoreScannerOnlyValuesInWorkingStorageWereUpdated = true;
								OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
							}
							else
							{
								OneOrMoreCommonValuesInWorkingStorageWereUpdated = true;
								OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
							}
							break;

						case '^':
						case '@':
							SubTagTableEntry->DefaultValue.BinaryString->Copy(&CommandTextCopy);
							SubTagTableEntry->DefaultValue.Updated = true;
							SubTagTableEntry->DefaultValue.ModifiedSinceInstantiation = true;
							OneOrMoreDefaultsUpdated = true;
							break;

						case '!':
							SubTagTableEntry->InstantiatorCopyOfActiveValue.BinaryString->Copy(&CommandTextCopy);
							SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated = true;
							SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
							OneOrMoreActiveValuesUpdated = true;

							if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
							{
								OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
								OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
							{
								OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
							{
								OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
							}
							else
							{
								OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
							}
							break;

						default:
							Status = RDR_CFG_SETTING_INVALID;
							break;
					}
				}
			}

			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::UNICODE_STRING)
			{
			// No text yet means that it may be a partial.
				if(CommandTextCopy.Size == 0)
					Status = RDR_CFG_SETTING_VALID;

			// Always OK, so long as it doesn't exceed maximum length.
				else
				{
					CommandTextCopy.SimpleAsciiToUniCodeFormat();
				// Set the valid status depending on whether or not we've reached the most characters we can receive.
					if(CommandTextCopy.Size == SubTagTableEntry->MaximumSizeOfActiveValue )
						Status = RDR_CFG_SETTING_VALID_MAX_LENGTH;
					else if(CommandTextCopy.Size < SubTagTableEntry->MaximumSizeOfActiveValue )
						Status = RDR_CFG_SETTING_VALID;
				}


			// If we have a valid setting and we're supposed to save it, do it now.
				if((Status > 0) && (SaveTheResult))
				{
				// Store the string value to the appropriate table and return an ACK.
					switch(AffectedTable)
					{
						case '.':
						// For stored value update, change the active value as well.
							WorkingValue->BinaryString->Copy(&CommandTextCopy);
							SubTagTableEntry->InstantiatorCopyOfActiveValue.BinaryString->Copy(&CommandTextCopy);
							WorkingValue->Updated = true;
							WorkingValue->ModifiedSinceInstantiation = true;
							SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated = true;
							SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
							OneOrMoreWorkingValuesUpdated = true;
							OneOrMoreActiveValuesUpdated = true;

							if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
							{
								OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated = true;
								OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
								OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
								OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
							{
								OneOrMoreBaseOnlyValuesInWorkingStorageWereUpdated = true;
								OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
							{
								OneOrMoreScannerOnlyValuesInWorkingStorageWereUpdated = true;
								OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
							}
							else
							{
								OneOrMoreCommonValuesInWorkingStorageWereUpdated = true;
								OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
							}
							break;

						case '^':
						case '@':
							SubTagTableEntry->DefaultValue.BinaryString->Copy(&CommandTextCopy);
							SubTagTableEntry->DefaultValue.Updated = true;
							SubTagTableEntry->DefaultValue.ModifiedSinceInstantiation = true;
							OneOrMoreDefaultsUpdated = true;
							break;

						case '!':
							SubTagTableEntry->InstantiatorCopyOfActiveValue.BinaryString->Copy(&CommandTextCopy);
							SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated = true;
							SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
							OneOrMoreActiveValuesUpdated = true;

							if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
							{
								OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
								OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
							{
								OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
							{
								OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
							}
							else
							{
								OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
							}
							break;

						default:
							Status = RDR_CFG_SETTING_INVALID;
							break;
					}
				}
			}






			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::HEX_STRING)
			{
			// No text yet means that it may be a partial.
				if(CommandTextCopy.Size == 0)
				{
					Status = RDR_CFG_SETTING_VALID;
				}

			// Always OK, so long as it doesn't exceed maximum length.
				else
				{
					if(CommandTextCopy.HexToBinary())
					{
					// Set the valid status depending on whether or not we've reached the most characters we can receive.
						if(CommandTextCopy.Size == SubTagTableEntry->MaximumSizeOfActiveValue )
						{
							Status = RDR_CFG_SETTING_VALID_MAX_LENGTH;
						}
						else if(CommandTextCopy.Size < SubTagTableEntry->MaximumSizeOfActiveValue )
						{
							Status = RDR_CFG_SETTING_VALID;
						}

					// If we don't have an even number of bytes, then it may be a partial.
						if((Status > 0) && ((CommandTextLength % 2) != 0))
						{
							Status = RDR_CFG_PARTIAL_SETTING_VALID;
						}
					}
				}


			// If we have a valid setting and we're supposed to save it, do it now.
				if((Status > 0) && (SaveTheResult))
				{
				// Store the string value to the appropriate table and return an ACK.
					switch(AffectedTable)
					{
						case '.':
						// For stored value update, change the active value as well.
							WorkingValue->BinaryString->Copy(&CommandTextCopy);
							SubTagTableEntry->InstantiatorCopyOfActiveValue.BinaryString->Copy(&CommandTextCopy);
							WorkingValue->Updated = true;
							WorkingValue->ModifiedSinceInstantiation = true;
							SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated = true;
							SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
							OneOrMoreWorkingValuesUpdated = true;
							OneOrMoreActiveValuesUpdated = true;

							if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
							{
								OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated = true;
								OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
								OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
								OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
							{
								OneOrMoreBaseOnlyValuesInWorkingStorageWereUpdated = true;
								OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
							{
								OneOrMoreScannerOnlyValuesInWorkingStorageWereUpdated = true;
								OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
							}
							else
							{
								OneOrMoreCommonValuesInWorkingStorageWereUpdated = true;
								OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
							}
							break;

						case '^':
						case '@':
							SubTagTableEntry->DefaultValue.BinaryString->Copy(&CommandTextCopy);
							SubTagTableEntry->DefaultValue.Updated = true;
							SubTagTableEntry->DefaultValue.ModifiedSinceInstantiation = true;
							OneOrMoreDefaultsUpdated = true;
							break;

						case '!':
							SubTagTableEntry->InstantiatorCopyOfActiveValue.BinaryString->Copy(&CommandTextCopy);
							SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated = true;
							SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
							OneOrMoreActiveValuesUpdated = true;

							if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
							{
								OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
								OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
							{
								OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
							{
								OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
							}
							else
							{
								OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
							}
							break;

						default:
							Status = RDR_CFG_SETTING_INVALID;
							break;
					}
				}
			}





			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::FORMAT_STRING)
			{
				switch(AffectedTable)
				{
					case '.':
						Status = WorkingValue->DataFormat->ProcessCommand((char *) CommandTextCopy.Char,
											SaveTheResult);
						Status = SubTagTableEntry->InstantiatorCopyOfActiveValue.DataFormat->ProcessCommand((char *) CommandTextCopy.Char,
											SaveTheResult);
						if((Status > 0) && (SaveTheResult))
						{
							WorkingValue->Updated = true;
							WorkingValue->ModifiedSinceInstantiation = true;
							SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated = true;
							SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
							OneOrMoreWorkingValuesUpdated = true;
							OneOrMoreActiveValuesUpdated = true;

							if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
							{
								OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated = true;
								OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
								OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
								OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
							{
								OneOrMoreBaseOnlyValuesInWorkingStorageWereUpdated = true;
								OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
							{
								OneOrMoreScannerOnlyValuesInWorkingStorageWereUpdated = true;
								OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
							}
							else
							{
								OneOrMoreCommonValuesInWorkingStorageWereUpdated = true;
								OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
							}
						}
						break;

					case '!':
						Status = SubTagTableEntry->InstantiatorCopyOfActiveValue.DataFormat->ProcessCommand((char *) CommandTextCopy.Char,
											SaveTheResult);
						if((Status > 0) && (SaveTheResult))
						{
							SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated = true;
							SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
							OneOrMoreActiveValuesUpdated = true;

							if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
							{
								OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
								OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
							{
								OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
							{
								OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
							}
							else
							{
								OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
							}
						}
						break;

					case '@':
					case '^':
						Status = SubTagTableEntry->DefaultValue.DataFormat->ProcessCommand((char *) CommandTextCopy.Char,
											SaveTheResult);
						if((Status > 0) && (SaveTheResult))
						{
							SubTagTableEntry->DefaultValue.Updated = true;
							SubTagTableEntry->DefaultValue.ModifiedSinceInstantiation = true;
							OneOrMoreDefaultsUpdated = true;
						}
						break;

					default:
						Status = RDR_CFG_SETTING_INVALID;
						break;
				}
			}





			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::PREFIX_SUFFIX_STRING)
			{
				switch(AffectedTable)
				{
					case '.':
						Status = WorkingValue->PrefixSuffix->ProcessCommand((char *) CommandTextCopy.Char,
											SaveTheResult);
						Status = SubTagTableEntry->InstantiatorCopyOfActiveValue.PrefixSuffix->ProcessCommand((char *) CommandTextCopy.Char,
											SaveTheResult);
						if((Status > 0) && (SaveTheResult))
						{
							WorkingValue->Updated = true;
							WorkingValue->ModifiedSinceInstantiation = true;
							SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated = true;
							SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
							OneOrMoreWorkingValuesUpdated = true;
							OneOrMoreActiveValuesUpdated = true;

							if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
							{
								OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated = true;
								OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
								OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
								OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
							{
								OneOrMoreBaseOnlyValuesInWorkingStorageWereUpdated = true;
								OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
							{
								OneOrMoreScannerOnlyValuesInWorkingStorageWereUpdated = true;
								OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
							}
							else
							{
								OneOrMoreCommonValuesInWorkingStorageWereUpdated = true;
								OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
							}
						}
						break;

					case '!':
						Status = SubTagTableEntry->InstantiatorCopyOfActiveValue.PrefixSuffix->ProcessCommand((char *) CommandTextCopy.Char,
											SaveTheResult);
						if((Status > 0) && (SaveTheResult))
						{
							SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated = true;
							SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
							OneOrMoreActiveValuesUpdated = true;

							if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
							{
								OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
								OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
							{
								OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
							}
							else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
							{
								OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
							}
							else
							{
								OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
							}
						}
						break;

					case '@':
					case '^':
						Status = SubTagTableEntry->DefaultValue.PrefixSuffix->ProcessCommand((char *) CommandTextCopy.Char,
											SaveTheResult);
						if((Status > 0) && (SaveTheResult))
						{
							SubTagTableEntry->DefaultValue.Updated = true;
							SubTagTableEntry->DefaultValue.ModifiedSinceInstantiation = true;
							OneOrMoreDefaultsUpdated = true;
						}
						break;

					default:
						Status = RDR_CFG_SETTING_INVALID;
						break;
				}
			}




			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BOOLEAN)
			{
			// Is there a single 0 or 1?  If yes, process, else NAK.
				if((CommandTextCopy.Size == 1) &&
							((CommandTextCopy.Char[0] == '0') || (CommandTextCopy.Char[0] == '1')))
				{
					Status = RDR_CFG_SETTING_VALID_MAX_LENGTH;

				// If we have a valid setting and we're supposed to save it, do it now.
					if((Status > 0) && (SaveTheResult))
					{
					// If Store the boolean value to the appropriate table and return an ACK.
						switch(AffectedTable)
						{
							case '.':
							// For stored value update, change the active value as well.
								WorkingValue->b =
													(CommandTextCopy.Char[0] == '1') ? true : false;
								*(SubTagTableEntry->InstantiatorCopyOfActiveValue.b) =
													(CommandTextCopy.Char[0] == '1') ? true : false;
								WorkingValue->Updated = true;
								WorkingValue->ModifiedSinceInstantiation = true;
								SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated = true;
								SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
								OneOrMoreWorkingValuesUpdated = true;
								OneOrMoreActiveValuesUpdated = true;

								if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
								{
									OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated = true;
									OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
									OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
									OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
								}
								else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
								{
									OneOrMoreBaseOnlyValuesInWorkingStorageWereUpdated = true;
									OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
								}
								else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
								{
									OneOrMoreScannerOnlyValuesInWorkingStorageWereUpdated = true;
									OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
								}
								else
								{
									OneOrMoreCommonValuesInWorkingStorageWereUpdated = true;
									OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
								}
								break;

							case '^':
							case '@':
								SubTagTableEntry->DefaultValue.b =
													(CommandTextCopy.Char[0] == '1') ? true : false;
								SubTagTableEntry->DefaultValue.Updated = true;
								SubTagTableEntry->DefaultValue.ModifiedSinceInstantiation = true;
								OneOrMoreDefaultsUpdated = true;
								break;

							case '!':
								*(SubTagTableEntry->InstantiatorCopyOfActiveValue.b) =
													(CommandTextCopy.Char[0] == '1') ? true : false;
								SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated = true;
								SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
								OneOrMoreActiveValuesUpdated = true;

								if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
								{
									OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
									OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
								}
								else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
								{
									OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
								}
								else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
								{
									OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
								}
								else
								{
									OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
								}
								break;

							default:
								Status = RDR_CFG_SETTING_INVALID;
								break;
						}
					}
				}

// The boolean value should be included with this setting.
//				else if(CommandTextCopy.Size == 0)
//				{
//					Status = RDR_CFG_PARTIAL_SETTING_VALID;
//				}
			}



			else if((SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::NUMERIC) ||
					(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SELECTION))
			{
			// Is there a single # and is it in range?  If yes, process, else NAK.
				int		NumberOfCharactersUsedInConversion;
				int		NumericSetting;

			// No text yet means that it may be a partial.
				if(CommandTextCopy.Size == 0)
				{
					Status = RDR_CFG_PARTIAL_SETTING_VALID;
				}

			// If the size of the new data is too large, return an error.
				else if(CommandTextCopy.Size <= SubTagTableEntry->MaximumSizeOfActiveValue)
				{
					if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::TWO_DIGIT_HEX)
					{
						NumericSetting = (int) CommandTextCopy.ConvertHexToNumber
														(0, &NumberOfCharactersUsedInConversion);
					}
					else
					{
						NumericSetting = (int) CommandTextCopy.ConvertToNumber
															(0, &NumberOfCharactersUsedInConversion);
					}



					if((CommandTextCopy.Size == ((unsigned int) NumberOfCharactersUsedInConversion)) &&
									(NumericSetting <= SubTagTableEntry->HighRange) &&
									(NumericSetting <= *(SubTagTableEntry->WorkingHighRangeAddress)))
					{
						if((NumericSetting >= SubTagTableEntry->LowRange) &&
									(NumericSetting >= *(SubTagTableEntry->WorkingLowRangeAddress)) &&
									(!(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::TWO_DIGIT_HEX) ||
													(NumberOfCharactersUsedInConversion == 2)))
						{
						// Set the valid status depending on whether or not we've reached the most characters we can receive.
							if(CommandTextCopy.Size == SubTagTableEntry->MaximumSizeOfActiveValue)
							{
								Status = RDR_CFG_SETTING_VALID_MAX_LENGTH;
							}
							else
							{
								Status = RDR_CFG_SETTING_VALID;
							}

						// We have a valid setting; if we're supposed to save it, do it now.
							if(SaveTheResult)
							{
							// Store the numeric value to the appropriate table and return an ACK.
								switch(AffectedTable)
								{
									case '.':
									// For stored value update, change the active value as well.
										WorkingValue->i = NumericSetting;
										*(SubTagTableEntry->InstantiatorCopyOfActiveValue.i) = NumericSetting;
										WorkingValue->Updated = true;
										WorkingValue->ModifiedSinceInstantiation = true;
										SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated = true;
										SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
										OneOrMoreWorkingValuesUpdated = true;
										OneOrMoreActiveValuesUpdated = true;

										if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
										{
											OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated = true;
											OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
											OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
											OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
										}
										else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
										{
											OneOrMoreBaseOnlyValuesInWorkingStorageWereUpdated = true;
											OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
										}
										else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
										{
											OneOrMoreScannerOnlyValuesInWorkingStorageWereUpdated = true;
											OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
										}
										else
										{
											OneOrMoreCommonValuesInWorkingStorageWereUpdated = true;
											OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
										}
										break;

									case '^':
									case '@':
										SubTagTableEntry->DefaultValue.i = NumericSetting;
										SubTagTableEntry->DefaultValue.Updated = true;
										SubTagTableEntry->DefaultValue.ModifiedSinceInstantiation = true;
										OneOrMoreDefaultsUpdated = true;
										break;

									case '!':
										*(SubTagTableEntry->InstantiatorCopyOfActiveValue.i) = NumericSetting;
										SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated = true;
										SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
										OneOrMoreActiveValuesUpdated = true;

										if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
										{
											OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
											OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
										}
										else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
										{
											OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
										}
										else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
										{
											OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
										}
										else
										{
											OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
										}
										break;

									default:
										Status = RDR_CFG_SETTING_INVALID;
										break;
								}

							// If this was a workgroup selection, we need to switch workgroups.
								if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SELECTION)
								{
									ChangeToWorkgroup( NumericSetting, true );
								}
								if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::CABLE_SETTING)
								{
									if(TableNeedChange)
									{
										TableNeedChange = false;
										CurrentTableNumber = NumericSetting;
										CurrentTableForScannedCommand = NumericSetting;
										ActiveTableNumber = NumericSetting;
									}
								}
							}
						}

					// If we've reached the maximum number of digits, but we haven't passed the low range / two digit hex
					// validation, we'll return an error, else we'll indicate that the command is partially complete.
						else if(CommandTextCopy.Size != SubTagTableEntry->MaximumSizeOfActiveValue)
						{
							Status = RDR_CFG_PARTIAL_SETTING_VALID;
						}
					}
				}
			}



			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::NUMERIC_LIST)
			{
			// Is there a single # and is it in range?  If yes, process, else NAK.
				int		NumberOfCharactersUsedInConversion;
				int		NumericSetting;


			// No text yet means that it may be a partial.
				if(CommandTextCopy.Size == 0)
				{
					Status = RDR_CFG_PARTIAL_SETTING_VALID;
				}

			// If the size of the new data is too large, return an error.
				else if(CommandTextCopy.Size <= SubTagTableEntry->MaximumSizeOfActiveValue)
				{
					if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::TWO_DIGIT_HEX)
					{
						NumericSetting = (int) CommandTextCopy.ConvertHexToNumber
														(0, &NumberOfCharactersUsedInConversion);
					}
					else
					{
						NumericSetting = (int) CommandTextCopy.ConvertToNumber
															(0, &NumberOfCharactersUsedInConversion);
					}



				// All characters must be numeric.
					if((CommandTextCopy.Size == ((unsigned int) NumberOfCharactersUsedInConversion)) &&
									(!(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::TWO_DIGIT_HEX) ||
													(NumberOfCharactersUsedInConversion == 2)))
					{
					// See if we find the number in the list.
						i = 0;
						while((i < SubTagTableEntry->NumberOfEntriesInListRange) &&
												(SubTagTableEntry->NumericListRange[i] != NumericSetting))
						{
							++i;
						}

					// If we found a list entry...
						if(i < SubTagTableEntry->NumberOfEntriesInListRange)
						{
						// Set the valid status depending on whether or not we've reached the most characters we can receive.
							if(CommandTextCopy.Size == SubTagTableEntry->MaximumSizeOfActiveValue)
							{
								Status = RDR_CFG_SETTING_VALID_MAX_LENGTH;
							}
							else
							{
								Status = RDR_CFG_SETTING_VALID;
							}

						// We have a valid setting; if we're supposed to save it, do it now.
							if(SaveTheResult)
							{
							// Store the numeric value to the appropriate table and return an ACK.
							if(!(SubTagTableEntry->TypeFlags &SUB_TAG_TABLE::WORKTABLE_SELECTION))
							{
								switch(AffectedTable)
								{
									case '.':
									// For stored value update, change the active value as well.
										WorkingValue->i = NumericSetting;
										*(SubTagTableEntry->InstantiatorCopyOfActiveValue.i) = NumericSetting;
										WorkingValue->Updated = true;
										WorkingValue->ModifiedSinceInstantiation = true;
										SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated = true;
										SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
										OneOrMoreWorkingValuesUpdated = true;
										OneOrMoreActiveValuesUpdated = true;

										if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
										{
											OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated = true;
											OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
											OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
											OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
										}
										else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
										{
											OneOrMoreBaseOnlyValuesInWorkingStorageWereUpdated = true;
											OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
										}
										else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
										{
											OneOrMoreScannerOnlyValuesInWorkingStorageWereUpdated = true;
											OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
										}
										else
										{
											OneOrMoreCommonValuesInWorkingStorageWereUpdated = true;
											OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
										}
										break;

									case '^':
									case '@':
										SubTagTableEntry->DefaultValue.i = NumericSetting;
										SubTagTableEntry->DefaultValue.Updated = true;
										SubTagTableEntry->DefaultValue.ModifiedSinceInstantiation = true;
										OneOrMoreDefaultsUpdated = true;
										break;

									case '!':
										*(SubTagTableEntry->InstantiatorCopyOfActiveValue.i) = NumericSetting;
										SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated = true;
										SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
										OneOrMoreActiveValuesUpdated = true;

										if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
										{
											OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
											OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
										}
										else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
										{
											OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
										}
										else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
										{
											OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
										}
										else
										{
											OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
										}
										break;

									default:
										Status = RDR_CFG_SETTING_INVALID;
										break;
								}
								}
							else
								{
									if(!ProcessingScannedMenu && !FirstTimeLoadWorkingConfigurationInFlash)
									{
										ChangeToWorktable(NumericSetting);
										TableSettingCommandisProcessing=true;
									}
									else if(ProcessingScannedMenu)
									{
										ScannedCBLTBLMenu=true;
										CurrentTableForScannedCommand=ActiveTableNumber;
									 	TableSettingUpdateSimultaneouslyForScannedCommand=false;
										ScannedMenuChangeToWorktable(NumericSetting);
									}
									
								}
							}
						}

					// If we've reached the maximum number of digits, but we didn't find the number in the list,
					// we'll return an error, else we'll indicate that the command is partially complete.
						else if(CommandTextCopy.Size != SubTagTableEntry->MaximumSizeOfActiveValue)
						{
							Status = RDR_CFG_PARTIAL_SETTING_VALID;
						}
					}
				}
			}



		// We're supposed to set the value with the remainder of the command string.
			else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING_LIST)
			{
			// No text yet means that it may be a partial.
				if(CommandTextCopy.Size == 0)
				{
					Status = RDR_CFG_PARTIAL_SETTING_VALID;
				}

			// Handle quoted strings as a special case.
				else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING_MUST_BE_QUOTED)
				{
					bool	IncompleteString;

					if( CommandTextCopy.QuotedHEXStringToBinary( &IncompleteString ))
					{
					// If the size of the new data is too large, return an error.
						if( CommandTextCopy.Size <= ( SubTagTableEntry->MaximumSizeOfActiveValue ))
						{
						// If we haven't exceeded the maximum number of digits, and the result was incomplete, report it.
							if( IncompleteString )
							{
								Status = RDR_CFG_PARTIAL_SETTING_VALID;
							}

							else
							{
							// Set the valid status depending on whether or not we've reached the most characters we can receive.
								if( CommandTextCopy.Size == SubTagTableEntry->MaximumSizeOfActiveValue )
								{
									Status = RDR_CFG_SETTING_VALID_MAX_LENGTH;
								}
								else
								{
									Status = RDR_CFG_SETTING_VALID;
								}
							}
						}
					}
/*				// If the size of the new data is too large, return an error.
					if(CommandTextCopy.Size <= (SubTagTableEntry->MaximumSizeOfActiveValue + 2))
					{
					// If we have quotes at either end, we're done.
						if((CommandTextCopy.Size >= 2) && (CommandTextCopy.Char[0] == '"') &&
														(CommandTextCopy.Char[CommandTextCopy.Size - 1] == '"'))
						{
						// Set the valid status depending on whether or not we've reached the most characters we can receive.
							if(CommandTextCopy.Size == (SubTagTableEntry->MaximumSizeOfActiveValue + 2))
							{
								Status = RDR_CFG_SETTING_VALID_MAX_LENGTH;
							}
							else
							{
								Status = RDR_CFG_SETTING_VALID;
							}
						}

					// If we haven't reached the maximum number of digits, we haven't passed quote test, and we find a quote
					// at the beginning of the string, we'll indicate that the command is partially complete.
						else if((CommandTextCopy.Size != (SubTagTableEntry->MaximumSizeOfActiveValue + 2)) &&
														(CommandTextCopy.Char[0] == '"'))
						{
							Status = RDR_CFG_PARTIAL_SETTING_VALID;
						}
					}
*/
				}


			// Unquoted string is always OK, so long as it doesn't exceed maximum length.
				else
				{
				// Set the valid status depending on whether or not we've reached the most characters we can receive.
					if(CommandTextCopy.Size == SubTagTableEntry->MaximumSizeOfActiveValue )
					{
						Status = RDR_CFG_SETTING_VALID_MAX_LENGTH;
					}
					else if(CommandTextCopy.Size < SubTagTableEntry->MaximumSizeOfActiveValue )
					{
						Status = RDR_CFG_SETTING_VALID;
					}
				}


			// If the string is OK, we now need to see if it is in the list.
				if(Status > 0)
				{
				// If the string is quoted, remove the quotes first.  NO LONGER NECESSARY, AS THEY WILL ALREADY HAVE BEEN REMOVED.
/*					if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING_MUST_BE_QUOTED)
					{
						CommandTextCopy.Remove(CommandTextCopy.Size - 1, 1);
						CommandTextCopy.Remove(0, 1);
					}
*/

				// Search for the new setting in the string list range.
					int		LengthOfLongestStringListItem = -2147483647;

					i = 0;
					while((i < SubTagTableEntry->NumberOfEntriesInListRange) &&
								(CommandTextCopy.Compare(SubTagTableEntry->StringListRange[i], false) != 0))
					{
						if(((int) strlen(SubTagTableEntry->StringListRange[i])) > LengthOfLongestStringListItem)
						{
							LengthOfLongestStringListItem = strlen(SubTagTableEntry->StringListRange[i]);
						}
						++i;
					}


				// If we found a list entry...
					if(i < SubTagTableEntry->NumberOfEntriesInListRange)
					{
					// Set the valid status depending on whether or not we've reached the most characters we can receive.
						if(CommandTextCopy.Size == SubTagTableEntry->MaximumSizeOfActiveValue)
						{
							Status = RDR_CFG_SETTING_VALID_MAX_LENGTH;
						}
						else
						{
							Status = RDR_CFG_SETTING_VALID;
						}

					// We have a valid setting; if we're supposed to save it, do it now.
						if(SaveTheResult)
						{
						// Store the string value to the appropriate table and return an ACK.
							switch(AffectedTable)
							{
								case '.':
								// For stored value update, change the active value as well.
									strcpy(WorkingValue->c, SubTagTableEntry->StringListRange[i]);
									strcpy(SubTagTableEntry->InstantiatorCopyOfActiveValue.c, SubTagTableEntry->StringListRange[i]);
									WorkingValue->Updated = true;
									WorkingValue->ModifiedSinceInstantiation = true;
									SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated = true;
									SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
									OneOrMoreWorkingValuesUpdated = true;
									OneOrMoreActiveValuesUpdated = true;

									if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
									{
										OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated = true;
										OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
										OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
										OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
									}
									else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
									{
										OneOrMoreBaseOnlyValuesInWorkingStorageWereUpdated = true;
										OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
									}
									else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
									{
										OneOrMoreScannerOnlyValuesInWorkingStorageWereUpdated = true;
										OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
									}
									else
									{
										OneOrMoreCommonValuesInWorkingStorageWereUpdated = true;
										OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
									}
									break;

								case '^':
								case '@':
									strcpy(SubTagTableEntry->DefaultValue.c, SubTagTableEntry->StringListRange[i]);
									SubTagTableEntry->DefaultValue.Updated = true;
									SubTagTableEntry->DefaultValue.ModifiedSinceInstantiation = true;
									OneOrMoreDefaultsUpdated = true;
									break;

								case '!':
									strcpy(SubTagTableEntry->InstantiatorCopyOfActiveValue.c, SubTagTableEntry->StringListRange[i]);
									SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated = true;
									SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
									OneOrMoreActiveValuesUpdated = true;

									if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
									{
										OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
										OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
									}
									else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
									{
										OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
									}
									else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
									{
										OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
									}
									else
									{
										OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
									}
									break;

								default:
									Status = RDR_CFG_SETTING_INVALID;
									break;
							}
						}
					}
				}


			// If we've reached the maximum number of characters, but we haven't found the string in the list,
			// we'll return an error, else we'll indicate that the command is partially complete.
//Fix STRING_LIST to check maximum length of longest item in list
				else if(CommandTextCopy.Size < SubTagTableEntry->MaximumSizeOfActiveValue)
				{
					Status = RDR_CFG_PARTIAL_SETTING_VALID;
				}

				else
				{
					Status = RDR_CFG_SETTING_INVALID;
				}
			}




		// If this is a command, check for valid extraneous data.  Note that all we'll do is check; we won't actually process
		// anything, even if the SaveTheResult flag is set.
			if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::COMMAND)
			{
			// If this entry takes extraneous data, indicate that this is valid, but not complete.
				if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND)
				{
					Status = RDR_CFG_SETTING_VALID;

				// If there is a special action that uses extraneous text, try to process it.
					if(SubTagTableEntry->SpecialCommandAction == SUB_TAG_TABLE::ENABLE_SYMBOLOGIES)
					{
					// There should either be no extraneous text, a '0', or a '1'.
						if(ExtraneousTextAfterCommand->Size > 1)
						{
							Status = RDR_CFG_SETTING_INVALID;
						}
						else if((ExtraneousTextAfterCommand->Size == 1) &&
									((ExtraneousTextAfterCommand->Char[0] == '1') ||
									 (ExtraneousTextAfterCommand->Char[0] == '0')))
						{
							Status = RDR_CFG_SETTING_VALID_MAX_LENGTH;
						}
					}

					else if(SubTagTableEntry->SpecialCommandAction == SUB_TAG_TABLE::CLEAR_FORMAT_ENTRY)
					{
					// The extraneous text should parse to a valid format key.  Since we just need to use the ParseKey
					// method and we're not storing data, we'll use the ParseKey method associated with the active value.
						if(ExtraneousTextAfterCommand->Size > 1)
						{
							int		FormatNumber;		// We aren't using the parsed parameters, since we're just trying to
							int		TerminalType;		//	validate the key.
							int		CodeIdentifier;
							int		Length;

							Status = SubTagTableEntry->InstantiatorCopyOfActiveValue.DataFormat->ParseKey
									(((char *) ExtraneousTextAfterCommand->Char),
											&FormatNumber, &TerminalType, &CodeIdentifier, &Length);
						}
					}

					else if(SubTagTableEntry->SpecialCommandAction == SUB_TAG_TABLE::CLEAR_PREFIX_SUFFIX_ENTRY)
					{
					// The extraneous text should parse to a valid prefix / suffix key.  Since we just need to use the ParseKey
					// method and we're not storing data, we'll use the ParseKey method associated with the active value.
						if(ExtraneousTextAfterCommand->Size > 1)
						{
							int		CodeIdentifier;		// We aren't using the parsed parameters, since we're just trying to
														//	validate the key.

							Status = SubTagTableEntry->InstantiatorCopyOfActiveValue.PrefixSuffix->ParseKey
									(((char *) ExtraneousTextAfterCommand->Char), &CodeIdentifier);
						}
					}
				}

				else
				{
				// There isn't supposed to be any extraneous text.  If there is any, WOOF!
					if(CommandTextCopy.Size == 0)
					{
						Status = RDR_CFG_SETTING_VALID_MAX_LENGTH;
					}
					else
					{
						Status = RDR_CFG_SETTING_INVALID;
					}
				}



			// If there is a special action and it requires data, validate it.
				if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND)
				{
					Status = RDR_CFG_SETTING_VALID;
				}

			}
		}


		else
		{
		// We tried to alter a fixed setting, but it wasn't a command; WOOF!
			Status = RDR_CFG_ATTEMPT_TO_CHANGE_FIXED_SETTING;
		}
	}


	return(Status);
} // ValidateAndChangeSetting








//
//	ReplicateAcrossWorkgroups
//		If the setting sent by the caller is a workgroup setting, and the table(s) affected are either working or active,
//		copy the current workgroup value to all the other workgroup tables.
//
void READER_CONFIGURATION_STORAGE::ReplicateAcrossWorkgroups( SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntry,
											const char AffectedTable )
{
// If table is not '.' or '!', then there are no workgroups to deal with; return.
	if((AffectedTable != '.') && (AffectedTable != '!'))
	{
		return;
	}


// If this is not a workgroup setting, return.
	if( !( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING ) )
	{
		return;
	}


// Since the table was either '!' or '.', and in either case, the instantiator copy of the active value was updated,
// save the instantiator copy to the current active value table entry.
	MoveInstantiatorCopyOfActiveValueToCurrentWorkgroupInActiveValueTable( SubTagTableEntry );


// Replicate the lower entries.
	int		i;

	i = 0;
	while( i < CurrentWorkgroupNumber )
	{
		CopySettingValue( &( SubTagTableEntry->ActiveValue[ i ] ), &( SubTagTableEntry->ActiveValue[ CurrentWorkgroupNumber ] ),
					SubTagTableEntry->TypeFlags );

		SubTagTableEntry->ActiveValue[ i ].Updated = true;
		SubTagTableEntry->ActiveValue[ i ].ModifiedSinceInstantiation = true;
		OneOrMoreActiveValuesUpdated = true;
		OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
		OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ i ] = true;

		if( AffectedTable == '.' )
		{
			CopySettingValue( &( SubTagTableEntry->WorkingValue[ i ] ), &( SubTagTableEntry->WorkingValue[ CurrentWorkgroupNumber ] ),
						SubTagTableEntry->TypeFlags );

			SubTagTableEntry->WorkingValue[ i ].Updated = true;
			SubTagTableEntry->WorkingValue[ i ].ModifiedSinceInstantiation = true;
			OneOrMoreWorkingValuesUpdated = true;
			OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated = true;
			OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated[ i ] = true;
		}

		++i;
	}


// Replicate the upper entries.
	i = CurrentWorkgroupNumber + 1;
	while( i < NumberOfWorkgroups )
	{
		CopySettingValue( &( SubTagTableEntry->ActiveValue[ i ] ), &( SubTagTableEntry->ActiveValue[ CurrentWorkgroupNumber ] ),
					SubTagTableEntry->TypeFlags );

		SubTagTableEntry->ActiveValue[ i ].Updated = true;
		SubTagTableEntry->ActiveValue[ i ].ModifiedSinceInstantiation = true;
		OneOrMoreActiveValuesUpdated = true;
		OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
		OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ i ] = true;

		if( AffectedTable == '.' )
		{
			CopySettingValue( &( SubTagTableEntry->WorkingValue[ i ] ), &( SubTagTableEntry->WorkingValue[ CurrentWorkgroupNumber ] ),
						SubTagTableEntry->TypeFlags );

			SubTagTableEntry->WorkingValue[ i ].Updated = true;
			SubTagTableEntry->WorkingValue[ i ].ModifiedSinceInstantiation = true;
			OneOrMoreWorkingValuesUpdated = true;
			OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated = true;
			OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated[ i ] = true;
		}

		++i;
	}



	return;
} // ReplicateAcrossWorkgroups








//
//	DoSpecialAction
//		See if there is any special action associated with this command and if there is, do it.
//
bool READER_CONFIGURATION_STORAGE::DoSpecialAction(SUB_TAG_TABLE *SubTagTable,
					SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntry, const char *PrefixTag, const char AffectedTable)
{
	int		i;
	bool	EntryWasDeleted;			// Return status for data format and prefix / suffix erasures (we don't use it).
	bool	CommandStatus = true;		// Assume success until we know otherwise.


	switch(SubTagTableEntry->SpecialCommandAction)
	{
		case SUB_TAG_TABLE::SET_ALL_WORKING_AND_ACTIVE_AND_DEFAULT_VALUES_TO_STANDARD_DEFAULTS:
			CopyStandardDefaultsToDefaults();
			CopyDefaultsToWorkingAndActive();
			AllActiveValuesWereDefaulted = true;
			OneOrMoreActiveValuesWereDefaulted = true;
			break;

		case SUB_TAG_TABLE::SET_ALL_WORKING_AND_ACTIVE_AND_DEFAULT_VALUES_TO_STANDARD_DEFAULTS_FOR_ALL_TABLES:
			CopyStandardDefaultsToDefaults();
			CopyDefaultsToWorkingAndActive( '&', true );
			AllActiveValuesWereDefaulted = true;
			OneOrMoreActiveValuesWereDefaulted = true;
			break;

		case SUB_TAG_TABLE::SET_ALL_WORKING_AND_ACTIVE_VALUES_TO_STANDARD_DEFAULTS:
			CopyDefaultsToWorkingAndActive('&');
			AllActiveValuesWereDefaulted = true;
			OneOrMoreActiveValuesWereDefaulted = true;
			break;

		case SUB_TAG_TABLE::SET_ALL_WORKING_AND_ACTIVE_VALUES_TO_STANDARD_DEFAULTS_FOR_ALL_TABLES:
			CopyDefaultsToWorkingAndActive( '&', true );
			AllActiveValuesWereDefaulted = true;
			OneOrMoreActiveValuesWereDefaulted = true;
			break;

		case SUB_TAG_TABLE::SET_ALL_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT:
			CopyDefaultsToWorkingAndActive(AffectedTable);
			AllActiveValuesWereDefaulted = true;
			OneOrMoreActiveValuesWereDefaulted = true;
			break;

		case SUB_TAG_TABLE::SET_ALL_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_ALL_TABLES:
			CopyDefaultsToWorkingAndActive( AffectedTable, true );
			AllActiveValuesWereDefaulted = true;
			OneOrMoreActiveValuesWereDefaulted = true;
			break;

		case SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG:
			RestoreAllSettingsWithThisTagToDefault(PrefixTag, AffectedTable);
			OneOrMoreActiveValuesWereDefaulted = true;
			break;

		case SUB_TAG_TABLE::COPY_STANDARD_DEFAULTS_TO_DEFAULTS:
			CopyStandardDefaultsToDefaults();
			break;

		case SUB_TAG_TABLE::COPY_WORKING_VALUES_TO_DEFAULTS:
			CopyWorkingToDefaults();
			break;

		case SUB_TAG_TABLE::ENABLE_SYMBOLOGIES:
		// If there is no extraneous character, enable all symbologies.
			if(ExtraneousTextAfterCommand->Size == 0)
			{
				CommandStatus = EnableAllSymbologies(AffectedTable);
			}

		// If there is a single extraneous character and it is '1', enable all symbologies.
			else if((ExtraneousTextAfterCommand->Size == 1) &&
												(ExtraneousTextAfterCommand->Char[0] == '1'))
			{
				CommandStatus = EnableAllSymbologies(AffectedTable);
			}

		// If there is a single extraneous character and it is '0', disable all symbologies.
			else if((ExtraneousTextAfterCommand->Size == 1) &&
												(ExtraneousTextAfterCommand->Char[0] == '0'))
			{
				CommandStatus = DisableAllSymbologies(AffectedTable);
			}

			else
			{
			// There is an extraneous character, but it is invalid; WOOF!
				CommandStatus = false;
			}
			break;

		case SUB_TAG_TABLE::CLEAR_FORMAT_ENTRY:
		// Clear specified format entries sharing this tag.
			i = 0;
			CommandStatus = false;		// Don't set to true until we find at least one format entry.

			while(i < ((int) SubTagTable->NumberOfEntries))
			{
				SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY			*CurrentEntry = &SubTagTable->Entries[i];
				SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*WorkingValue = WorkingValueForCurrentWorkgroup( CurrentEntry );

				if(CurrentEntry->TypeFlags & SUB_TAG_TABLE::FORMAT_STRING)
				{
					switch(AffectedTable)
					{
						case '.':
							CommandStatus = WorkingValue->DataFormat->
													Erase(((char *) ExtraneousTextAfterCommand->Char), &EntryWasDeleted);
							CommandStatus = CurrentEntry->InstantiatorCopyOfActiveValue.DataFormat->
													Erase(((char *) ExtraneousTextAfterCommand->Char), &EntryWasDeleted);
							if(CommandStatus)
							{
								WorkingValue->Updated = true;
								WorkingValue->ModifiedSinceInstantiation = true;
								CurrentEntry->InstantiatorCopyOfActiveValue.Updated = true;
								CurrentEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
								OneOrMoreWorkingValuesUpdated = true;
								OneOrMoreActiveValuesUpdated = true;

								if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
								{
									OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated = true;
									OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
									OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
									OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
								}
								else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
								{
									OneOrMoreBaseOnlyValuesInWorkingStorageWereUpdated = true;
									OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
								}
								else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
								{
									OneOrMoreScannerOnlyValuesInWorkingStorageWereUpdated = true;
									OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
								}
								else
								{
									OneOrMoreCommonValuesInWorkingStorageWereUpdated = true;
									OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
								}
							}
							break;

						case '^':
						case '@':
							CommandStatus = CurrentEntry->DefaultValue.DataFormat->
													Erase(((char *) ExtraneousTextAfterCommand->Char), &EntryWasDeleted);
							if(CommandStatus)
							{
								CurrentEntry->DefaultValue.Updated = true;
								CurrentEntry->DefaultValue.ModifiedSinceInstantiation = true;
								OneOrMoreDefaultsUpdated = true;
							}
							break;

						case '!':
							CommandStatus = CurrentEntry->InstantiatorCopyOfActiveValue.DataFormat->
													Erase(((char *) ExtraneousTextAfterCommand->Char), &EntryWasDeleted);
							if(CommandStatus)
							{
								CurrentEntry->InstantiatorCopyOfActiveValue.Updated = true;
								CurrentEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
								OneOrMoreActiveValuesUpdated = true;

								if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
								{
									OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
									OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
								}
								else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
								{
									OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
								}
								else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
								{
									OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
								}
								else
								{
									OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
								}
							}
							break;

						default:
							CommandStatus = false;
							break;
					};
				}

				++i;
			}
			break;

		case SUB_TAG_TABLE::CLEAR_ALL_FORMAT_ENTRIES:
		// Clear all format entries sharing this tag.
			i = 0;
			CommandStatus = false;		// Don't set to true until we find at least one format entry.

			while(i < ((int) SubTagTable->NumberOfEntries))
			{
				SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY			*CurrentEntry = &SubTagTable->Entries[i];
				SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*WorkingValue = WorkingValueForCurrentWorkgroup( CurrentEntry );

				if(CurrentEntry->TypeFlags & SUB_TAG_TABLE::FORMAT_STRING)
				{
					switch(AffectedTable)
					{
						case '.':
							CommandStatus = WorkingValue->DataFormat->EraseAll(&EntryWasDeleted);
							CommandStatus = CurrentEntry->InstantiatorCopyOfActiveValue.DataFormat->EraseAll(&EntryWasDeleted);
							if(CommandStatus)
							{
								WorkingValue->Updated = true;
								WorkingValue->ModifiedSinceInstantiation = true;
								CurrentEntry->InstantiatorCopyOfActiveValue.Updated = true;
								CurrentEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
								OneOrMoreWorkingValuesUpdated = true;
								OneOrMoreActiveValuesUpdated = true;

								if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
								{
									OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated = true;
									OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
									OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
									OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
								}
								else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
								{
									OneOrMoreBaseOnlyValuesInWorkingStorageWereUpdated = true;
									OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
								}
								else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
								{
									OneOrMoreScannerOnlyValuesInWorkingStorageWereUpdated = true;
									OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
								}
								else
								{
									OneOrMoreCommonValuesInWorkingStorageWereUpdated = true;
									OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
								}
							}
							break;

						case '^':
						case '@':
							CommandStatus = CurrentEntry->DefaultValue.DataFormat->EraseAll(&EntryWasDeleted);
							if(CommandStatus)
							{
								CurrentEntry->DefaultValue.Updated = true;
								CurrentEntry->DefaultValue.ModifiedSinceInstantiation = true;
								OneOrMoreDefaultsUpdated = true;
							}
							break;

						case '!':
							CommandStatus = CurrentEntry->InstantiatorCopyOfActiveValue.DataFormat->EraseAll(&EntryWasDeleted);
							if(CommandStatus)
							{
								CurrentEntry->InstantiatorCopyOfActiveValue.Updated = true;
								CurrentEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
								OneOrMoreActiveValuesUpdated = true;

								if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
								{
									OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
									OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
								}
								else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
								{
									OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
								}
								else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
								{
									OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
								}
								else
								{
									OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
								}
							}
							break;

						default:
							CommandStatus = false;
							break;
					};
				}

				++i;
			}
			break;

		case SUB_TAG_TABLE::CLEAR_PREFIX_SUFFIX_ENTRY:
		// Clear specified prefix / suffix entries sharing this tag.
			i = 0;
			CommandStatus = false;		// Don't set to true until we find at least one prefix / suffix.

			while(i < ((int) SubTagTable->NumberOfEntries))
			{
				SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY			*CurrentEntry = &SubTagTable->Entries[i];
				SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*WorkingValue = WorkingValueForCurrentWorkgroup( CurrentEntry );

				if(CurrentEntry->TypeFlags & SUB_TAG_TABLE::PREFIX_SUFFIX_STRING)
				{
					switch(AffectedTable)
					{
						case '.':
							CommandStatus = WorkingValue->PrefixSuffix->
													Erase(((char *) ExtraneousTextAfterCommand->Char), &EntryWasDeleted);
							CommandStatus = CurrentEntry->InstantiatorCopyOfActiveValue.PrefixSuffix->
													Erase(((char *) ExtraneousTextAfterCommand->Char), &EntryWasDeleted);
							if(CommandStatus)
							{
								WorkingValue->Updated = true;
								WorkingValue->ModifiedSinceInstantiation = true;
								CurrentEntry->InstantiatorCopyOfActiveValue.Updated = true;
								CurrentEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
								OneOrMoreWorkingValuesUpdated = true;
								OneOrMoreActiveValuesUpdated = true;

								if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
								{
									OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated = true;
									OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
									OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
									OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
								}
								else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
								{
									OneOrMoreBaseOnlyValuesInWorkingStorageWereUpdated = true;
									OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
								}
								else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
								{
									OneOrMoreScannerOnlyValuesInWorkingStorageWereUpdated = true;
									OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
								}
								else
								{
									OneOrMoreCommonValuesInWorkingStorageWereUpdated = true;
									OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
								}
							}
							break;

						case '^':
						case '@':
							CommandStatus = CurrentEntry->DefaultValue.PrefixSuffix->
													Erase(((char *) ExtraneousTextAfterCommand->Char), &EntryWasDeleted);
							if(CommandStatus)
							{
								CurrentEntry->DefaultValue.Updated = true;
								CurrentEntry->DefaultValue.ModifiedSinceInstantiation = true;
								OneOrMoreDefaultsUpdated = true;
							}
							break;

						case '!':
							CommandStatus = CurrentEntry->InstantiatorCopyOfActiveValue.PrefixSuffix->
													Erase(((char *) ExtraneousTextAfterCommand->Char), &EntryWasDeleted);
							if(CommandStatus)
							{
								CurrentEntry->InstantiatorCopyOfActiveValue.Updated = true;
								CurrentEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
								OneOrMoreActiveValuesUpdated = true;

								if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
								{
									OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
									OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
								}
								else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
								{
									OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
								}
								else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
								{
									OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
								}
								else
								{
									OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
								}
							}
							break;

						default:
							CommandStatus = false;
							break;
					};
				}

				++i;
			}
			break;

		case SUB_TAG_TABLE::CLEAR_ALL_PREFIX_SUFFIX_ENTRIES:
		// Clear all prefix / suffix entries sharing this tag.
			i = 0;
			CommandStatus = false;		// Don't set to true until we find at least one prefix / suffix.

			while(i < ((int) SubTagTable->NumberOfEntries))
			{
				SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY			*CurrentEntry = &SubTagTable->Entries[i];
				SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*WorkingValue = WorkingValueForCurrentWorkgroup( CurrentEntry );

				if(CurrentEntry->TypeFlags & SUB_TAG_TABLE::PREFIX_SUFFIX_STRING)
				{
					switch(AffectedTable)
					{
						case '.':
							CommandStatus = WorkingValue->PrefixSuffix->EraseAll(&EntryWasDeleted);
							CommandStatus = CurrentEntry->InstantiatorCopyOfActiveValue.PrefixSuffix->EraseAll(&EntryWasDeleted);
							if(CommandStatus)
							{
								WorkingValue->Updated = true;
								WorkingValue->ModifiedSinceInstantiation = true;
								CurrentEntry->InstantiatorCopyOfActiveValue.Updated = true;
								CurrentEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
								OneOrMoreWorkingValuesUpdated = true;
								OneOrMoreActiveValuesUpdated = true;

								if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
								{
									OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated = true;
									OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
									OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
									OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
								}
								else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
								{
									OneOrMoreBaseOnlyValuesInWorkingStorageWereUpdated = true;
									OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
								}
								else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
								{
									OneOrMoreScannerOnlyValuesInWorkingStorageWereUpdated = true;
									OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
								}
								else
								{
									OneOrMoreCommonValuesInWorkingStorageWereUpdated = true;
									OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
								}
							}
							break;

						case '^':
						case '@':
							CommandStatus = CurrentEntry->DefaultValue.PrefixSuffix->EraseAll(&EntryWasDeleted);
							if(CommandStatus)
							{
								CurrentEntry->DefaultValue.Updated = true;
								CurrentEntry->DefaultValue.ModifiedSinceInstantiation = true;
								OneOrMoreDefaultsUpdated = true;
							}
							break;

						case '!':
							CommandStatus = CurrentEntry->InstantiatorCopyOfActiveValue.PrefixSuffix->EraseAll(&EntryWasDeleted);
							if(CommandStatus)
							{
								CurrentEntry->InstantiatorCopyOfActiveValue.Updated = true;
								CurrentEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
								OneOrMoreActiveValuesUpdated = true;

								if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING )
								{
									OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
									OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
								}
								else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING )
								{
									OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
								}
								else if( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
								{
									OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
								}
								else
								{
									OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
								}
							}
							break;

						default:
							CommandStatus = false;
							break;
					};
				}

				++i;
			}
			break;

		default:
			break;
	};


	return(CommandStatus);
} // DoSpecialAction






//
//	CompareSettingValues
//		Determine if two settings are equal.
//
bool READER_CONFIGURATION_STORAGE::CompareSettingValues( int ValueType,
				SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY *Value1, SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY *Value2 )
{
	bool	SettingValuesAreEqual;


// Compare the values.
	if( ValueType & SUB_TAG_TABLE::BOOLEAN )
	{
		SettingValuesAreEqual = ( Value1->b == Value2->b );
	}

	else if(( ValueType & SUB_TAG_TABLE::STRING ) || ( ValueType & SUB_TAG_TABLE::STRING_LIST ))
	{
		SettingValuesAreEqual = ( strcmp( Value1->c, Value2->c ) == 0 );
	}

	else if(( ValueType & SUB_TAG_TABLE::BINARY_STRING ) ||
					( ValueType & SUB_TAG_TABLE::ASCII_STRING ) ||
					( ValueType & SUB_TAG_TABLE::HEX_STRING ) ||
					( ValueType & SUB_TAG_TABLE::UNICODE_STRING ))
	{
		SettingValuesAreEqual = ( Value1->BinaryString->Compare( Value2->BinaryString, true ) == 0 );
	}

	else if(( ValueType & SUB_TAG_TABLE::NUMERIC ) || ( ValueType & SUB_TAG_TABLE::NUMERIC_LIST ))
	{
		SettingValuesAreEqual = ( Value1->i == Value2->i );
	}

	else if( ValueType & SUB_TAG_TABLE::FORMAT_STRING )
	{
		HSTRING		Temp1(100, true);
		HSTRING		Temp2(100, true);
		Value1->DataFormat->BuildReportString(&Temp1, true, true);
		Value2->DataFormat->BuildReportString(&Temp2, true, true);
		SettingValuesAreEqual = ( Temp1.Compare( &Temp2, true ) == 0 );
	}

	else if( ValueType & SUB_TAG_TABLE::PREFIX_SUFFIX_STRING )
	{
		HSTRING		Temp1(100, true);
		HSTRING		Temp2(100, true);
		Value1->PrefixSuffix->BuildReportString(&Temp1, true, true);
		Value2->PrefixSuffix->BuildReportString(&Temp2, true, true);
		SettingValuesAreEqual = ( Temp1.Compare( &Temp2, true ) == 0 );
	}

	else
	{
	// Unrecognized type; woof!
		SettingValuesAreEqual = true;
	}


	return SettingValuesAreEqual;
} // CompareSettingValues






//
//	CompareSettingValues (overload #2 where second setting is instantiator copy)
//		Determine if two settings are equal.
//
bool READER_CONFIGURATION_STORAGE::CompareSettingValues( int ValueType,
				SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY *Value1,
				SUB_TAG_TABLE::SUB_TAG_TABLE_INSTANTIATOR_COPY_OF_ACTIVE_VALUE *Value2 )
{
	bool	SettingValuesAreEqual;


// Compare the values.
	if( ValueType & SUB_TAG_TABLE::BOOLEAN )
	{
		SettingValuesAreEqual = ( Value1->b == *( Value2->b ) );
	}

	else if(( ValueType & SUB_TAG_TABLE::STRING ) || ( ValueType & SUB_TAG_TABLE::STRING_LIST ))
	{
		SettingValuesAreEqual = ( strcmp( Value1->c, Value2->c ) == 0 );
	}

	else if(( ValueType & SUB_TAG_TABLE::BINARY_STRING ) ||
					( ValueType & SUB_TAG_TABLE::ASCII_STRING ) ||
					( ValueType & SUB_TAG_TABLE::HEX_STRING ) ||
					( ValueType & SUB_TAG_TABLE::UNICODE_STRING ))
	{
		SettingValuesAreEqual = ( Value1->BinaryString->Compare( Value2->BinaryString, true ) == 0 );
	}

	else if(( ValueType & SUB_TAG_TABLE::NUMERIC ) || ( ValueType & SUB_TAG_TABLE::NUMERIC_LIST ))
	{
		SettingValuesAreEqual = ( Value1->i == *( Value2->i ) );
	}

	else if( ValueType & SUB_TAG_TABLE::FORMAT_STRING )
	{
		HSTRING		Temp1(100, true);
		HSTRING		Temp2(100, true);
		Value1->DataFormat->BuildReportString(&Temp1, true, true);
		Value2->DataFormat->BuildReportString(&Temp2, true, true);
		SettingValuesAreEqual = ( Temp1.Compare( &Temp2, true ) == 0 );
	}

	else if( ValueType & SUB_TAG_TABLE::PREFIX_SUFFIX_STRING )
	{
		HSTRING		Temp1(100, true);
		HSTRING		Temp2(100, true);
		Value1->PrefixSuffix->BuildReportString(&Temp1, true, true);
		Value2->PrefixSuffix->BuildReportString(&Temp2, true, true);
		SettingValuesAreEqual = ( Temp1.Compare( &Temp2, true ) == 0 );
	}

	else
	{
	// Unrecognized type; woof!
		SettingValuesAreEqual = true;
	}


	return SettingValuesAreEqual;
} // CompareSettingValues (overload #2 where second setting is instantiator copy)






//
//	WorkgroupSettingIsCommonValue
//		Determine if all the workgroups in the setting sent by the caller have the same value.
//
bool READER_CONFIGURATION_STORAGE::WorkgroupSettingIsCommonValue( SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SubTagTableEntry, char AffectedTable )
{
	bool	SettingIsCommonValue = false;


// If there's only one value, it must be common.
	if( SubTagTableEntry->NumberOfWorkgroups == 1 )
	{
		SettingIsCommonValue = true;
	}

	else if(( AffectedTable == '.' ) || ( AffectedTable == '!' ))
	{
		SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY						*ValueToCheck;

	// If this is an active value, let's first resynchronize the instantiator copy to the active table.
		if( AffectedTable == '!' )
		{
			MoveInstantiatorCopyOfActiveValueToCurrentWorkgroupInActiveValueTable( SubTagTableEntry );
			ValueToCheck = SubTagTableEntry->ActiveValue;
		}
		else
		{
			ValueToCheck = SubTagTableEntry->WorkingValue;
		}


	// Now, compare the values.
		SettingIsCommonValue = true;
		int		NextWorkgroupToCompare = 1;
		while(( SettingIsCommonValue ) && ( NextWorkgroupToCompare < SubTagTableEntry->NumberOfWorkgroups ) )
		{
		// Check the next workgroup value.
			SettingIsCommonValue = CompareSettingValues( SubTagTableEntry->TypeFlags,
							&( ValueToCheck[ 0 ] ), &( ValueToCheck[ NextWorkgroupToCompare ] ) );

			++NextWorkgroupToCompare;
		}
	}

	else
	{
	// This must be a default, and defaults are always common, since there's only one value.
		SettingIsCommonValue = true;
	}


	return SettingIsCommonValue;
} // WorkgroupSettingIsCommonValue





//
//	ProcessMenuCommandSubTags
//		Process all the menu command sub-tags in the list sent by the caller.
//
bool READER_CONFIGURATION_STORAGE::ProcessMenuCommandSubTags(SUB_TAG_TABLE *SubTagTable, HSTRING *MenuCommandSubTagAndResponse,
					const char *PrefixTag, char AffectedTable, bool WildcardTag, int *NumberOfSubTagsProcessed,
					int *NumberOfSubTagResponsesNotReported, bool OverrideFixedFlag, bool ForceDirectDefaultCommandPrivilege )
{
	HSTRING		OriginalCommandSubTag(MenuCommandSubTagAndResponse);
	bool		WildcardSubTag;							// Indicates whether or not the sub-tag is a wildcard.
	bool		SubTagOK;
	bool		CommonValueForWorkgroup;



	*NumberOfSubTagsProcessed = 0;
	*NumberOfSubTagResponsesNotReported = 0;


// Let the entry and exit functions see which table is being updated.
	CurrentTable = AffectedTable;

// Clear extraneous text from last command.
	ExtraneousTextAfterCommand->Copy("");

// Nothing in the response yet.
	MenuCommandSubTagAndResponse->Copy("");


// Keep getting subtags until we reach the end of the list.
	HSTRING		NextCommandSubTag(OriginalCommandSubTag.Size, true);// Container for the next command sub tag to be processed (max size is
																						//	the original command sub tag.
	int		PositionOfNextCommandSubTag;

	bool		ProcessAnotherCommandSubTag = true;		// We haven't run out of command sub-tags yet.

	do  {
		SubTagOK = true;
		ProcessAnotherCommandSubTag = OriginalCommandSubTag.FindCharacterForward(',', 0, &PositionOfNextCommandSubTag);

	// If another command sub tag follows the next one, then only copy up to the command sub tag beyond the next one; otherwise, copy
	// the whole buffer.
		if(ProcessAnotherCommandSubTag)
		{
			OriginalCommandSubTag.CopyFromString(0, PositionOfNextCommandSubTag, &NextCommandSubTag);
			OriginalCommandSubTag.Remove(0, PositionOfNextCommandSubTag + 1);
		}
		else
		{
			NextCommandSubTag.Copy(&OriginalCommandSubTag);
		}


	// Try to process the command.
		unsigned int	LowerRangeOfSubTagsToProcess;
		unsigned int	OrderOfSubTagToProcess;
		unsigned int	UpperRangeOfSubTagsToProcess;


	// If there are 4 or less characters left and the last character is a "^" (get default), "*" (get
	// range), '#' (get working range, which may be limited by cross dependency to another variable),
	// "?" (get current value), "$" (get current value without status), or "~" (get current value if
	// modified since instantiation without status), save the character to Action and remove the character
	// from the end of the command
		char	Action=' ';

		if((NextCommandSubTag.Size <= (SIZE_OF_SUB_TAG + 1)) &&
						((NextCommandSubTag.Char[NextCommandSubTag.Size - 1] == '^') ||
								(NextCommandSubTag.Char[NextCommandSubTag.Size - 1] == '*') ||
								(NextCommandSubTag.Char[NextCommandSubTag.Size - 1] == '#') ||
								(NextCommandSubTag.Char[NextCommandSubTag.Size - 1] == '?') ||
								(NextCommandSubTag.Char[NextCommandSubTag.Size - 1] == '$') ||
								(NextCommandSubTag.Char[NextCommandSubTag.Size - 1] == '~')))
		{
			Action = NextCommandSubTag.Char[NextCommandSubTag.Size - 1];
			NextCommandSubTag.Remove(NextCommandSubTag.Size - 1, 1);
		}


	// If we have a sub-tag , try to lookup the sub-tag, else cycle through all the sub-tags.
		if(NextCommandSubTag.Size > 0)
		{
		// We have a tag.  Try to do a lookup on it.
			if(SubTagTable->Find(((char *) NextCommandSubTag.Char), &LowerRangeOfSubTagsToProcess, &OrderOfSubTagToProcess))
			{
			// Remove the sub-tag.
				NextCommandSubTag.Remove(0, SIZE_OF_SUB_TAG);


			// Set range.
				UpperRangeOfSubTagsToProcess = LowerRangeOfSubTagsToProcess;

				WildcardSubTag = false;
			}

			else
			{
			// This sub-tag didn't match; return the command with an ENQ.
				NextCommandSubTag.UpperCase();
				MenuCommandSubTagAndResponse->Append(&NextCommandSubTag);
				MenuCommandSubTagAndResponse->AppendNumber(((unsigned int) ENQ), 1);
				if(ProcessAnotherCommandSubTag)
				{
					MenuCommandSubTagAndResponse->Append(",");
				}
				SubTagOK = false;
			}
		}

		else if(Action != ' ')
		{
		// We have no explicit sub tag, so we need to loop through all of them.  Note that this is only valid when we're doing
		// some type of query command.
			LowerRangeOfSubTagsToProcess = 0;
			UpperRangeOfSubTagsToProcess = SubTagTable->NumberOfEntries - 1;

			WildcardSubTag = true;
		}

		else
		{
		// We have no explicit sub tag and we have no query command; WOOF!
			MenuCommandSubTagAndResponse->AppendNumber(((unsigned int) ENQ), 1);
			if(ProcessAnotherCommandSubTag)
			{
				MenuCommandSubTagAndResponse->Append(",");
			}
			SubTagOK = false;
		}


	// Now let's process the sub-tags.
		if(SubTagOK)
		{
			unsigned int			SubTagTableEntryIndex;	// Index of next entry in the sub-tag table corresponding to the command sub tag.
			SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY
							*SubTagTableEntry;		// Next entry in the sub-tag table corresponding to the command sub tag.
			SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*WorkingValue=NULL;



			for(SubTagTableEntryIndex = LowerRangeOfSubTagsToProcess;    SubTagTableEntryIndex <= UpperRangeOfSubTagsToProcess;
													++SubTagTableEntryIndex)
			{
				++(*NumberOfSubTagsProcessed);
				ReportResponse = true;

			// Are we sorting in order
				if(( SortWildcardQueriesForThisCommand ) && ( LowerRangeOfSubTagsToProcess != UpperRangeOfSubTagsToProcess ))
				{
					SubTagTableEntry = &(SubTagTable->Entries[ SubTagTable->EntryIndex[ SubTagTableEntryIndex ]]);
				}
				else
				{
					SubTagTableEntry = &(SubTagTable->Entries[SubTagTableEntryIndex]);
				}

				bool		ValueModifiedSinceInstantiation;
				bool		ValueUpdatedSinceLastCommand;
				bool		ActiveValueIsEqualToWorkingValue;
				bool		WorkingValueUpdatedSinceLastCommand = false;
				bool		ActiveValueUpdatedSinceLastCommand = false;

			// Point to the working value.
				if(( AffectedTable == '.' ) || ( AffectedTable == '!' ))
				{
					WorkingValue = WorkingValueForCurrentWorkgroup( SubTagTableEntry );
					WorkingValueUpdatedSinceLastCommand = WorkingValue->Updated;
					ActiveValueUpdatedSinceLastCommand = SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated;
				}


			// In case the query is a '~', determine if the value has been modified since instantiation, according to the affected table.
			// In case update reporting parameters apply, check to see if this setting has been updated.
				switch ( AffectedTable )
				{
					case '@':
						ValueModifiedSinceInstantiation = SubTagTableEntry->DefaultValue.ModifiedSinceInstantiation;
						ValueUpdatedSinceLastCommand = SubTagTableEntry->DefaultValue.Updated;
						ActiveValueIsEqualToWorkingValue = true;			// Don't care if it's not a '.' or '!' query.
						break;

					case '.':
						ValueModifiedSinceInstantiation = WorkingValue->ModifiedSinceInstantiation;
						ValueUpdatedSinceLastCommand = WorkingValue->Updated;

					// Only do this if we're supposed to be excluding based on whether or not active and working are equal.
					// If we don't need to know, don't waste time on it.
						if(( !ReportSettingsWhenActiveIsEqualToWorkingForThisCommand ) ||
							( !ReportSettingsWhenActiveIsNotEqualToWorkingForThisCommand ) ||
							( SettingsUpdatedSinceLastCommandModifiersForThisCommand !=
											SETTINGS_UPDATED_SINCE_LAST_COMMAND_NO_MODIFIERS ))
						{
							ActiveValueIsEqualToWorkingValue = CompareSettingValues( SubTagTableEntry->TypeFlags,
										WorkingValue, &( SubTagTableEntry->InstantiatorCopyOfActiveValue ) );
						}
						else
						{
							ActiveValueIsEqualToWorkingValue = true;
						}
						break;

					case '!':
						ValueModifiedSinceInstantiation = SubTagTableEntry->InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation;
						ValueUpdatedSinceLastCommand = SubTagTableEntry->InstantiatorCopyOfActiveValue.Updated;

					// Only do this if we're supposed to be excluding based on whether or not active and working are equal.
					// If we don't need to know, don't waste time on it.
						if(( !ReportSettingsWhenActiveIsEqualToWorkingForThisCommand ) ||
							( !ReportSettingsWhenActiveIsNotEqualToWorkingForThisCommand ) ||
							( SettingsUpdatedSinceLastCommandModifiersForThisCommand !=
											SETTINGS_UPDATED_SINCE_LAST_COMMAND_NO_MODIFIERS ))
						{
							ActiveValueIsEqualToWorkingValue = CompareSettingValues( SubTagTableEntry->TypeFlags,
										WorkingValue, &( SubTagTableEntry->InstantiatorCopyOfActiveValue ) );
						}
						else
						{
							ActiveValueIsEqualToWorkingValue = true;
						}
						break;

					default:
						ValueModifiedSinceInstantiation = false;
						ValueUpdatedSinceLastCommand = false;
						ActiveValueIsEqualToWorkingValue = true;			// Don't care if it's not a '.' or '!' query.
						break;
				};

			// Don't process this sub-tag if it is not marked visible and we're not supposed to show hidden entries, if the
			// action is a '$' query and this sub-tag is a command, if the action is a '~' query and this sub-tag is a command,
			// if the action is not a query (Action = ' ') and the affected table is '@' but we don't have privilege to do this,
			// or if this item only available when default command privilege is active.

			// In case this is a query of a workgroup setting, check to see if all workgroups for this settings have a common value.
				CommonValueForWorkgroup = WorkgroupSettingIsCommonValue( SubTagTableEntry, AffectedTable );

				if(
				// See if this setting is visible.  Note that if this not a command, and the action is '$' or '~', then it is
				// considered visible.  This test could appear to conflict with the test further on for similar conditions, but
				// that second test will stop reporting of a command if the action is '$' or '~', where this wants to cause
				// reporting if not a command, but the action is '$' or '~'.
					(((!(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::COMMAND)) &&
																				( ( Action == '$' ) || ( Action == '~' ) ) ) ||
						(OverrideFixedFlag) ||

					// If hidden value is set, we don't want to pay attention to the visibility flag (we'll mask that below).
						( ReportHiddenSettingsForThisCommand != 0 ) ||

						(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::FULLY_VISIBLE) ||
						((SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::VISIBLE_BY_TAG) && (!WildcardTag)) ||
						((SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG) && (!WildcardSubTag))) &&

				// If there are flags set for reporting hidden values, mask out this setting if the flags don't correspond.
					( ( ReportHiddenSettingsForThisCommand == 0 ) ||
						(( ReportHiddenSettingsForThisCommand & HIDDEN_SETTINGS_REPORT_INVISIBLE ) &&
								!( ( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::FULLY_VISIBLE ) ||
									( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::VISIBLE_BY_TAG ) ||
									( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG ) ) )  ||
						(( ReportHiddenSettingsForThisCommand & HIDDEN_SETTINGS_REPORT_VISIBLE_BY_SUB_TAG ) &&
									( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG ) )  ||
						(( ReportHiddenSettingsForThisCommand & HIDDEN_SETTINGS_REPORT_VISIBLE_BY_TAG ) &&
									( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::VISIBLE_BY_TAG ) )  ||
						(( ReportHiddenSettingsForThisCommand & HIDDEN_SETTINGS_REPORT_FULLY_VISIBLE ) &&
									( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::FULLY_VISIBLE ) ) ) &&

				// If we're doing a '~' query, don't report if the value hasn't been modified since instantiation.
					( ( Action != '~' ) || ( ValueModifiedSinceInstantiation )) &&

				// If the action is '$' or '~', and this is a command, don't report.
					(( ( Action != '$' ) && ( Action != '~' ) ) || (!(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::COMMAND))) &&

				// If the action is '$' or '~', and we're supposed to ignore on storage query, don't report.
					(( ( Action != '$' ) && ( Action != '~' ) ) || (!(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::IGNORE_ON_STORAGE_QUERY))) &&

				// If this is a query of the defaults table, and neither direct default command privilege is on nor are we being
				// forced to that mode by the caller, don't report.
					( ( Action != ' ' ) || ( AffectedTable != '@' ) || ( UltimatePrivilegeLevel & DIRECT_DEFAULT_COMMAND_PRIVILEGE ) ||
						( DirectDefaultCommandPrivilege ) || ( ForceDirectDefaultCommandPrivilege )) &&

				// If this setting is only supposed to be available when direct default command privilege is enabled, and
				// neither direct default command privilege is on nor are we being forced to that mode by the caller, don't
				// report.
					( (!( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::ONLY_AVAILABLE_WITH_DEFAULT_COMMAND_PRIVILEGE )) ||
						( UltimatePrivilegeLevel & DIRECT_DEFAULT_COMMAND_PRIVILEGE ) || ( DirectDefaultCommandPrivilege ) || ( ForceDirectDefaultCommandPrivilege ) ) &&

				// If this is some type of query (Action is non-null), then see if update reporting excludes the setting from being
				//	reported.
					(( Action == ' ' ) ||
					// This is a query, but we're not excluding anything.  We're doing this check for the benefit of those
					// who aren't excluding anything, since this is faster and is less logic error prone than what follows.
						( ReportSettingsUpdatedSinceLastCommandForThisCommand &&
							ReportSettingsNotUpdatedSinceLastCommandForThisCommand &&
							ReportSettingsUpdatedSinceStartupForThisCommand &&
							ReportSettingsNotUpdatedSinceStartupForThisCommand &&
							ReportSettingsWhenActiveIsEqualToWorkingForThisCommand &&
							ReportSettingsWhenActiveIsNotEqualToWorkingForThisCommand &&
							( SettingsUpdatedSinceLastCommandModifiersForThisCommand ==
														SETTINGS_UPDATED_SINCE_LAST_COMMAND_NO_MODIFIERS )) ||
					// We're excluding something, so we better determine what it is.
						(((( ReportSettingsUpdatedSinceLastCommandForThisCommand && ValueUpdatedSinceLastCommand ) ||
								( ReportSettingsNotUpdatedSinceLastCommandForThisCommand && ( !ValueUpdatedSinceLastCommand ))) &&
							( ( !WorkingValueUpdatedSinceLastCommand ) ||
									( !( SettingsUpdatedSinceLastCommandModifiersForThisCommand &
												SETTINGS_UPDATED_SINCE_LAST_COMMAND_EXCLUDE_IF_WORKING_UPDATED ))) &&
							( WorkingValueUpdatedSinceLastCommand ||
									( !( SettingsUpdatedSinceLastCommandModifiersForThisCommand &
												SETTINGS_UPDATED_SINCE_LAST_COMMAND_EXCLUDE_IF_WORKING_NOT_UPDATED ))) &&
							( ( !ActiveValueUpdatedSinceLastCommand ) ||
									( !( SettingsUpdatedSinceLastCommandModifiersForThisCommand &
												SETTINGS_UPDATED_SINCE_LAST_COMMAND_EXCLUDE_IF_ACTIVE_UPDATED ))) &&
							( ActiveValueUpdatedSinceLastCommand ||
									( !( SettingsUpdatedSinceLastCommandModifiersForThisCommand &
												SETTINGS_UPDATED_SINCE_LAST_COMMAND_EXCLUDE_IF_ACTIVE_NOT_UPDATED )))) &&
						 (( ReportSettingsUpdatedSinceStartupForThisCommand && ValueModifiedSinceInstantiation ) ||
							( ReportSettingsNotUpdatedSinceStartupForThisCommand && ( !ValueModifiedSinceInstantiation ))) &&
						 (( ReportSettingsWhenActiveIsEqualToWorkingForThisCommand && ActiveValueIsEqualToWorkingValue ) ||
							( ReportSettingsWhenActiveIsNotEqualToWorkingForThisCommand && ( !ActiveValueIsEqualToWorkingValue ))))) &&

				// If this is some type of query (Action is non-null), then see if the current setting type (base only, common, or workgroup) is allowed to be
				// reported.
					(( Action == ' ' ) ||
						( ReportBaseOnlySettingsForThisCommand && ( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING ) )  ||
						( ReportScannerOnlySettingsForThisCommand && ( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING ) )  ||
						( ReportCommonSettingsForThisCommand && ( !( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BASE_ONLY_SETTING ) ) &&
									( !( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::SCANNER_ONLY_SETTING ) ) &&
									( ! (SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING ) ) )  ||
						( (SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SETTING ) &&
							( ( WorkgroupSettingsHandlingForThisCommand == REPORT_AND_SET_ALL_WORKGROUP_SETTINGS_IN_CURRENT_WORKGROUP ) ||
								( ( WorkgroupSettingsHandlingForThisCommand == REPORT_AND_SET_ALL_WORKGROUP_SETTINGS_IN_ALL_WORKGROUPS_WITH_COMMON_VALUES ) &&
											CommonValueForWorkgroup ) ||
								( ( WorkgroupSettingsHandlingForThisCommand == REPORT_AND_SET_ALL_WORKGROUP_SETTINGS_IN_CURRENT_WORKGROUP_THAT_ARE_NOT_COMMON_VALUES ) &&
											( !CommonValueForWorkgroup ) ) ) ) ) )
				{
				// Add the tag to the response if the tag is a wildcard and we're not in compressed mode.
					if(( WildcardTag ) && ( !TagCompressedModeEnabledForThisCommand ))
					{
						MenuCommandSubTagAndResponse->Append(PrefixTag);
					}

				// Add the sub-tag to the response.
					int		StartOfResponse = MenuCommandSubTagAndResponse->Size;
					MenuCommandSubTagAndResponse->Append(SubTagTableEntry->SubTag, SIZE_OF_SUB_TAG);


				// If the sub-tag contains data, then process it, else ACK the command.
					if((SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING) ||
								(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BINARY_STRING) ||
								(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::ASCII_STRING) ||
								(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::HEX_STRING) ||
								(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::UNICODE_STRING) ||
								(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::STRING_LIST) ||
								(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::FORMAT_STRING) ||
								(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::PREFIX_SUFFIX_STRING) ||
								(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::BOOLEAN) ||
								(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::NUMERIC_LIST) ||
								(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::NUMERIC) ||
								(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SELECTION))
					{
					// Process according to the requested action.
						if(Action == '^')
						{
							OneOrMoreQueryCommandsWereProcessed = true;

							HSTRING		SettingValue(40, true);

							QuerySetting(SubTagTableEntry, '^', &SettingValue, Action, false);
							MenuCommandSubTagAndResponse->Append(&SettingValue);

							MenuCommandSubTagAndResponse->AppendNumber(((unsigned int) ACK), 1);
						}

						else if((Action == '*') || (Action == '#'))
						{
							HSTRING		RangeSettingValue(40, true);

							QuerySettingRange(SubTagTableEntry, Action, &RangeSettingValue);
							MenuCommandSubTagAndResponse->Append(&RangeSettingValue);

							MenuCommandSubTagAndResponse->AppendNumber(((unsigned int) ACK), 1);
						}

						else if((Action == '?') || (Action == '$'))
						{
							OneOrMoreQueryCommandsWereProcessed = true;

							HSTRING		SettingValue(40, true);

							QuerySetting(SubTagTableEntry, AffectedTable, &SettingValue, Action, (Action == '$'));
							MenuCommandSubTagAndResponse->Append(&SettingValue);

							if(Action == '?')
							{
								MenuCommandSubTagAndResponse->AppendNumber(((unsigned int) ACK), 1);
							}
						}

						else if( Action == '~' )
						{
							OneOrMoreQueryCommandsWereProcessed = true;

							HSTRING		SettingValue(40, true);

							QuerySetting(SubTagTableEntry, AffectedTable, &SettingValue, Action, true);
							MenuCommandSubTagAndResponse->Append(&SettingValue);
						}


					// We're trying to set a value.
						else
						{
						// Make the command response string available to entry and exit functions that want to store their
						// response here instead of putting it out directly.
							MenuCommandResponse = &NextCommandSubTag;
							CurrentTable = AffectedTable;
							ReportResponse = true;

						// By default, we won't process the exit function if we had a problem changing the setting.  The entry
						// function can force the exit function to be run.
							ForceExitFunctionToBeExecutedOnSettingChangeError = false;

						// Execute the entry function (if there is one).
							bool	EntryFunctionStatus = true;		// Assume success.
							if(SubTagTableEntry->EntryFunction != 0)
							{
								EntryFunctionStatus = SubTagTableEntry->EntryFunction();
							}


						// Set this again in case the entry function called Menu to do some processing.
							MenuCommandResponse = &NextCommandSubTag;
							CurrentTable = AffectedTable;


						// Validate the parameter string, if no errors occurred in the entry function.  Note that we're giving the entry function
						// a chance to clean up any invalid characters.
							bool	ParameterStringIsValid = ValidateMenuParameterCharacters( MenuCommandResponse );


							if(( EntryFunctionStatus ) && ( ParameterStringIsValid ))
							{
								char CustomAffectedTable = AffectedTable;
								if( CustomDefaultProgrammingMode && CustomAffectedTable == '.' )
								{
									CustomAffectedTable = '@';
								}
							// Try to make the setting if the entry function status is OK.
								ValidateAndChangeSettingStatus =
										ValidateAndChangeSetting(SubTagTableEntry, ((char *) NextCommandSubTag.Char),
												NextCommandSubTag.Size, CustomAffectedTable, true, OverrideFixedFlag);

							// Add a status character to the response string according to the result of the validate and set.
								if(ValidateAndChangeSettingStatus == RDR_CFG_ATTEMPT_TO_CHANGE_FIXED_SETTING)
								{
								// Copy the setting text to the response string.
									MenuCommandSubTagAndResponse->Append(&NextCommandSubTag);
									MenuCommandSubTagAndResponse->AppendNumber(((unsigned int) ENQ), 1);

									if( ForceExitFunctionToBeExecutedOnSettingChangeError )
									{
										if(SubTagTableEntry->ExitFunction != 0)
										{
											SubTagTableEntry->ExitFunction();
										}
									}
								}

								else if((ValidateAndChangeSettingStatus == RDR_CFG_SETTING_VALID) ||
												(ValidateAndChangeSettingStatus == RDR_CFG_SETTING_VALID_MAX_LENGTH))
								{
								// Execute the exit function (if there is one).
									if(SubTagTableEntry->ExitFunction != 0)
									{
										if(SubTagTableEntry->ExitFunction())
										{
										// Exit function worked; ACK.
										// Copy the setting text to the response string.
											MenuCommandSubTagAndResponse->Append(&NextCommandSubTag);
											MenuCommandSubTagAndResponse->AppendNumber(((unsigned int) ACK), 1);
										}
										else
										{
										// Exit function failed; NAK.
										// Copy the setting text to the response string.
											MenuCommandSubTagAndResponse->Append(&NextCommandSubTag);
											MenuCommandSubTagAndResponse->AppendNumber(((unsigned int) NAK), 1);
										}
									}
									else
									{
									// No exit function; ACK.
									// Copy the setting text to the response string.
										MenuCommandSubTagAndResponse->Append(&NextCommandSubTag);
										MenuCommandSubTagAndResponse->AppendNumber(((unsigned int) ACK), 1);
									}

								// If this is a workgroup setting, and we're supposed to update all workgroup tables, do a replicate from the current
								// table to all the others.  Note that we don't want to do this operation before calling the exit function, because
								// the exit function may update values, and we want those updates to be replicated.
									if(( WorkgroupSettingsHandlingForThisCommand ==
												REPORT_AND_SET_ALL_WORKGROUP_SETTINGS_IN_ALL_WORKGROUPS_WITH_COMMON_VALUES ) ||
										( WorkgroupSettingsHandlingForThisCommand ==
												REPORT_ALL_WORKGROUP_SETTINGS_IN_ALL_WORKGROUPS_WITH_COMMON_VALUES_FIRST_THEN_WITH_UNCOMMON_VALUES ) ||
										( WorkgroupSettingsHandlingForThisCommand ==
												REPORT_ALL_WORKGROUP_SETTINGS_IN_ALL_WORKGROUPS ))
									{
										ReplicateAcrossWorkgroups( SubTagTableEntry, AffectedTable );
									}
								}

								else
								{
								// Copy the setting text to the response string.
									MenuCommandSubTagAndResponse->Append(&NextCommandSubTag);
									MenuCommandSubTagAndResponse->AppendNumber(((unsigned int) NAK), 1);

									if( ForceExitFunctionToBeExecutedOnSettingChangeError )
									{
										if(SubTagTableEntry->ExitFunction != 0)
										{
											SubTagTableEntry->ExitFunction();
										}
									}
								}
							}

							else
							{
							// Entry function failed, or parameter string was bad; NAK.
							// Copy the setting text to the response string.
								MenuCommandSubTagAndResponse->Append(&NextCommandSubTag);
								MenuCommandSubTagAndResponse->AppendNumber(((unsigned int) NAK), 1);
							}
						}
					}

				// Note that as we process a command, we'll only execute it if there was no specified reporting
				// action; otherwise, we'll just report ACK.
					else if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::COMMAND)
					{
					// Clear extraneous text from last command.
						ExtraneousTextAfterCommand->Copy("");

						bool	CommandStatus = true;		// Assume it worked until we know otherwise.


					// Append any extraneous text to the output.  Even if it's invalid, we'll still want to report it back to
					// the user.
						MenuCommandSubTagAndResponse->Append(&NextCommandSubTag);

					// Check for extraneous text after the command, which will be used (if allowed) by the command handler.
						if(NextCommandSubTag.Size > 0)
						{
							if(SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND)
							{
							// If text is allowed, store it for use by the command handler.
								ExtraneousTextAfterCommand->Copy(&NextCommandSubTag);
							}
							else
							{
							// Text is not allowed, so we need to return an error.
								CommandStatus = false;
							}
						}

					// If the action is some type of query (non-space character), append it to the extraneous text so that
					// the entry or exit function can see it.
						if(( Action != ' ' ) &&
									( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::PASS_QUERY_CHARACTER_TO_ENTRY_EXIT_FUNCTIONS ))
							ExtraneousTextAfterCommand->AppendChar( (unsigned int) Action );

					// Make the command response string available to entry and exit functions that want to store their
					// response here instead of putting it out directly.
						MenuCommandResponse = MenuCommandSubTagAndResponse;
						CurrentTable = AffectedTable;
						ReportResponse = true;


					// Execute the entry function if there is one.
						if((CommandStatus) && (SubTagTableEntry->EntryFunction != 0) &&
								(( Action == ' ' ) ||
									( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::PASS_QUERY_CHARACTER_TO_ENTRY_EXIT_FUNCTIONS )))
						{
						// Make a copy of extraneous text, in case the entry function calls Menu and modifies
						// the extraneous text as a result.
							HSTRING		CopyOfExtraneousTextAfterCommand(ExtraneousTextAfterCommand);
							CommandStatus = SubTagTableEntry->EntryFunction();
							ExtraneousTextAfterCommand->Copy(&CopyOfExtraneousTextAfterCommand);
						}

					// Set this again in case the entry function called Menu to do some processing.
						MenuCommandResponse = MenuCommandSubTagAndResponse;
						CurrentTable = AffectedTable;

					// Do any specified action.
						if((CommandStatus) && (Action == ' '))
						{
							CommandStatus = DoSpecialAction(SubTagTable, SubTagTableEntry, PrefixTag, AffectedTable);
						}

					// Execute the exit function if there is one and if no errors have occurred.
						if((SubTagTableEntry->ExitFunction != 0) && (CommandStatus) &&
								(( Action == ' ' ) ||
									( SubTagTableEntry->TypeFlags & SUB_TAG_TABLE::PASS_QUERY_CHARACTER_TO_ENTRY_EXIT_FUNCTIONS )))
						{
							CommandStatus = SubTagTableEntry->ExitFunction();
						}

					// There is no data associated with this sub-tag.  We'll tack on an ACK if the command succeeded, else
					// we'll tack on a NAK.
						if(CommandStatus)
						{
							MenuCommandSubTagAndResponse->AppendNumber(((unsigned int) ACK), 1);
						}
						else
						{
							MenuCommandSubTagAndResponse->AppendNumber(((unsigned int) NAK), 1);
						}
					}


				// If there is another wildcard sub tag to process, or there is another command sub tag to process
				// after this one, add a ";" to the end of the response if we also prefixed with a tag,
				// else add a ",".
					if( ReportResponse )
					{
						if((SubTagTableEntryIndex < UpperRangeOfSubTagsToProcess) || (ProcessAnotherCommandSubTag))
						{
						// Add the ";" to the response if the tag is a wildcard and we're not in compressed mode.
							if(( WildcardTag ) && ( !TagCompressedModeEnabledForThisCommand ))
							{
								MenuCommandSubTagAndResponse->Append(";");
							}
							else
							{
								MenuCommandSubTagAndResponse->Append(",");
							}
						}
					}
					else
					{
						++(*NumberOfSubTagResponsesNotReported);

					// We're not supposed to report the result, so we need to back it out of the response.
						MenuCommandSubTagAndResponse->Remove
											( StartOfResponse, MenuCommandSubTagAndResponse->Size - StartOfResponse );
					}
				}

				else
				{
				// If we got here because of a hidden entry, add the text with an ENQ.
					if(!WildcardSubTag)
					{
					// Add the sub-tag to the response.
						MenuCommandSubTagAndResponse->Append(SubTagTableEntry->SubTag, SIZE_OF_SUB_TAG);
						MenuCommandSubTagAndResponse->Append(&NextCommandSubTag);
						MenuCommandSubTagAndResponse->AppendNumber(((unsigned int) ENQ), 1);
						if(ProcessAnotherCommandSubTag)
						{
							MenuCommandSubTagAndResponse->Append(",");
						}
					}

				// If this is a wildcard sub tag, but not a wildcard tag, this is the last entry for the tag,
				// and we've had no results because they were supposed to be hidden, send an ENQ.
					else if(( !WildcardTag ) &&
								(SubTagTableEntryIndex == UpperRangeOfSubTagsToProcess) &&
								(MenuCommandSubTagAndResponse->Size == 0))
					{
						MenuCommandSubTagAndResponse->Append(&NextCommandSubTag);
						MenuCommandSubTagAndResponse->AppendNumber(((unsigned int) ENQ), 1);
						if(ProcessAnotherCommandSubTag)
						{
							MenuCommandSubTagAndResponse->Append(",");
						}
					}
				}


/*				else
				{
				// This last entry was hidden.  If the last character in the response string is a ";" or ",",
				// remove it.
					if((MenuCommandSubTagAndResponse->Size != 0) && (!ProcessAnotherCommandSubTag) &&
						((MenuCommandSubTagAndResponse->Char[MenuCommandSubTagAndResponse->Size - 1] == ',') ||
							(MenuCommandSubTagAndResponse->Char[MenuCommandSubTagAndResponse->Size - 1] == ';')))
					{
						MenuCommandSubTagAndResponse->Remove(MenuCommandSubTagAndResponse->Size - 1, 1);
					}
				}
*/
			}
		}
	} while(ProcessAnotherCommandSubTag);

// If the last character in the response string is a ";" or ",", remove it.
	if((MenuCommandSubTagAndResponse->Size != 0) &&
						((MenuCommandSubTagAndResponse->Char[MenuCommandSubTagAndResponse->Size - 1] == ',') ||
							(MenuCommandSubTagAndResponse->Char[MenuCommandSubTagAndResponse->Size - 1] == ';')))
	{
		MenuCommandSubTagAndResponse->Remove(MenuCommandSubTagAndResponse->Size - 1, 1);
	}


	return(SubTagOK);
} // ProcessMenuCommandSubTags







//
//	GetAffectedTable
//		Validate the table character, remove it from the command string, and return it to the caller.
//
bool READER_CONFIGURATION_STORAGE::GetAffectedTable( HSTRING *MenuCommandAndResponse, char *AffectedTable )
{
	bool	Status = true;		// No errors yet.


// There needs to be at least one character.
	if( MenuCommandAndResponse->Size >= 1 )
	{
	// Get the affected table.
		char	TableCharacter = MenuCommandAndResponse->Char[ MenuCommandAndResponse->Size - 1 ];

	// If the affected table character is not ".", "!", "^", "@", or "&", quit.
		if(( TableCharacter != '.' ) && ( TableCharacter != '!' ) && ( TableCharacter != '&' ) && ( TableCharacter != '^' ) &&
					 ( TableCharacter != '@' ))
		{
			Status = false;
		}
		else
		{
			*AffectedTable = TableCharacter;
			MenuCommandAndResponse->Remove( MenuCommandAndResponse->Size - 1, 1 );
		}
	}
	else
	{
	// Empty string; WOOF.
		Status = false;
	}

	return Status;
} // GetAffectedTable





//
//	ProcessMenuCommands
//		Process the menu command string sent by the caller.  Originally, this logic was part of the Menu function, but
//		we want to be able to process recursively if we're in report all workgroups mode, so this needed to be separated
//		so that it could be called recursively.
//
void READER_CONFIGURATION_STORAGE::ProcessMenuCommands( HSTRING *MenuCommandAndResponse, char AffectedTable,
								bool OverrideFixedFlag, bool ForceDirectDefaultCommandPrivilege, bool *bHadProblems )
{
	HSTRING		OriginalCommand(MenuCommandAndResponse);
	int			NumberOfSubTagsProcessed;
	int			NumberOfSubTagResponsesNotReported;




// Nothing in the response yet.
	MenuCommandAndResponse->Copy("");


// If there aren't any characters, return immediately.
	if(OriginalCommand.Size < 1)
	{
		*bHadProblems = true;
		return;
	}


// Keep getting commands until we reach the end of the list.
	HSTRING			NextCommand(OriginalCommand.Size, true);// Container for the next command to be processed (max size is the original command.

	int				PositionOfNextCommand;
	unsigned int	TagTableEntry;							// Entry in the tag table corresponding to the command.
	unsigned int	TagTableEntryOrder;						// Sorted entry index in the tag table corresponding to the command.

	bool				ProcessAnotherCommandTag = true;			// We haven't run out of commands yet.

	do
	{
		ProcessAnotherCommandTag = OriginalCommand.FindCharacterForward(';', 0, &PositionOfNextCommand);

	// If another command follows the next one, then only copy up to the command beyond the next one; otherwise, copy the whole buffer.
		if(ProcessAnotherCommandTag)
		{
			OriginalCommand.CopyFromString(0, PositionOfNextCommand, &NextCommand);
			OriginalCommand.Remove(0, PositionOfNextCommand + 1);
		}
		else
		{
			NextCommand.Copy(&OriginalCommand);
		}

	// Try to process the command.

	// If there are no characters left in the command, send an ENQ response.
		if(NextCommand.Size < 1)
		{
			MenuCommandAndResponse->AppendNumber(((unsigned int) ENQ), 1);
			*bHadProblems = true;
		}

		else
		{
		// Is this a reporting command?
			if( NextCommand.Char[0] == '=' )
			{
				ProcessReportingCommand( &NextCommand );

				if(NextCommand.Size)
				{
					MenuCommandAndResponse->Append(&NextCommand);
				}
			}

		// If this is a ?, $, or ~ query, the affected table is . or !, and the reporting mode is all workgroups, we need to
		// string together a command that causes all the appropriate reporting to be done, then call this routine recursively
		// to accomplish the job.
			else if((( NextCommand.Char[ NextCommand.Size - 1 ] == '?' ) ||
									( NextCommand.Char[ NextCommand.Size - 1 ] == '$' ) ||
									( NextCommand.Char[ NextCommand.Size - 1 ] == '~' )) &&
					(( AffectedTable == '!' ) || ( AffectedTable == '.' )) &&
					(( WorkgroupSettingsHandlingForThisCommand ==
											REPORT_ALL_WORKGROUP_SETTINGS_IN_ALL_WORKGROUPS_WITH_COMMON_VALUES_FIRST_THEN_WITH_UNCOMMON_VALUES ) ||
									( WorkgroupSettingsHandlingForThisCommand ==
																			REPORT_ALL_WORKGROUP_SETTINGS_IN_ALL_WORKGROUPS )))
			{
				HSTRING							RevisedCommand( 20, true );
				int								OriginalCurrentWorkgroupNumber = CurrentWorkgroupNumber;
				bool							OriginalReportBaseOnlySettingsForThisCommand = ReportBaseOnlySettingsForThisCommand;
				bool							OriginalReportScannerOnlySettingsForThisCommand = ReportScannerOnlySettingsForThisCommand;
				bool							OriginalReportCommonSettingsForThisCommand = ReportCommonSettingsForThisCommand;
				WORKGROUP_SETTINGS_HANDLING		OriginalWorkgroupSettingsHandlingForThisCommand = WorkgroupSettingsHandlingForThisCommand;
				SETTINGS_UPDATED_SINCE_LAST_COMMAND_MODIFIERS
												OriginalSettingsUpdatedSinceLastCommandModifiersForThisCommand =
															SettingsUpdatedSinceLastCommandModifiersForThisCommand;
				bool							OriginalReportSettingsUpdatedSinceLastCommandForThisCommand =
															ReportSettingsUpdatedSinceLastCommandForThisCommand;
				bool							OriginalReportSettingsNotUpdatedSinceLastCommandForThisCommand =
															ReportSettingsNotUpdatedSinceLastCommandForThisCommand;
				bool							OriginalReportSettingsUpdatedSinceStartupForThisCommand =
															ReportSettingsUpdatedSinceStartupForThisCommand;
				bool							OriginalReportSettingsNotUpdatedSinceStartupForThisCommand =
															ReportSettingsNotUpdatedSinceStartupForThisCommand;
				bool							OriginalReportSettingsWhenActiveIsEqualToWorkingForThisCommand =
															ReportSettingsWhenActiveIsEqualToWorkingForThisCommand;
				bool							OriginalReportSettingsWhenActiveIsNotEqualToWorkingForThisCommand =
															ReportSettingsWhenActiveIsNotEqualToWorkingForThisCommand;
				char							Temp[ 80 ];
				int								i;


				RevisedCommand.Append( "=AT:B+C+S" );
				if( WorkgroupSettingsHandlingForThisCommand ==
							REPORT_ALL_WORKGROUP_SETTINGS_IN_ALL_WORKGROUPS_WITH_COMMON_VALUES_FIRST_THEN_WITH_UNCOMMON_VALUES )
				{
					RevisedCommand.Append( "+A" );
				}
				RevisedCommand.Append( ";" );
				RevisedCommand.Append( &NextCommand );

				for( i = 0;    i < NumberOfWorkgroups;    ++i )
				{
					RevisedCommand.Append( ";=AT:" );
					if( WorkgroupSettingsHandlingForThisCommand ==
							REPORT_ALL_WORKGROUP_SETTINGS_IN_ALL_WORKGROUPS_WITH_COMMON_VALUES_FIRST_THEN_WITH_UNCOMMON_VALUES )
					{
						RevisedCommand.Append( "U" );
					}

					sprintf( Temp, "%d;", i );
					RevisedCommand.Append( Temp );
					RevisedCommand.Append( &NextCommand );
				}

				ProcessMenuCommands( &RevisedCommand, AffectedTable, OverrideFixedFlag, ForceDirectDefaultCommandPrivilege,
							bHadProblems );

				MenuCommandAndResponse->Append( &RevisedCommand );



			// Switch back to original workgroup and reporting before returning.
				if( CurrentWorkgroupNumber != OriginalCurrentWorkgroupNumber )
				{
					ChangeToWorkgroup( OriginalCurrentWorkgroupNumber, true );
				}

				ReportBaseOnlySettingsForThisCommand = OriginalReportBaseOnlySettingsForThisCommand;
				ReportScannerOnlySettingsForThisCommand = OriginalReportScannerOnlySettingsForThisCommand;
				ReportCommonSettingsForThisCommand = OriginalReportCommonSettingsForThisCommand;
				WorkgroupSettingsHandlingForThisCommand = OriginalWorkgroupSettingsHandlingForThisCommand;
				SettingsUpdatedSinceLastCommandModifiersForThisCommand =
												OriginalSettingsUpdatedSinceLastCommandModifiersForThisCommand;
				ReportSettingsUpdatedSinceLastCommandForThisCommand =
															OriginalReportSettingsUpdatedSinceLastCommandForThisCommand;
				ReportSettingsNotUpdatedSinceLastCommandForThisCommand =
															OriginalReportSettingsNotUpdatedSinceLastCommandForThisCommand;
				ReportSettingsUpdatedSinceStartupForThisCommand =
															OriginalReportSettingsUpdatedSinceStartupForThisCommand;
				ReportSettingsNotUpdatedSinceStartupForThisCommand =
															OriginalReportSettingsNotUpdatedSinceStartupForThisCommand;
				ReportSettingsWhenActiveIsEqualToWorkingForThisCommand =
															OriginalReportSettingsWhenActiveIsEqualToWorkingForThisCommand;
				ReportSettingsWhenActiveIsNotEqualToWorkingForThisCommand =
															OriginalReportSettingsWhenActiveIsNotEqualToWorkingForThisCommand;
			}


		// Get the tag for the next command.  If we have a tag (the next character is alphanumeric), try to lookup the tag, else
		// cycle through all the tags.
			else if(((NextCommand.Char[0] >= '0') && (NextCommand.Char[0] <= '9')) ||
					((NextCommand.Char[0] >= 'A') && (NextCommand.Char[0] <= 'Z')) ||
					((NextCommand.Char[0] >= 'a') && (NextCommand.Char[0] <= 'z')))
			{
			// We have a tag.  Try to do a lookup on it.
				if(TagTable->Find(((char *) NextCommand.Char), &TagTableEntry, &TagTableEntryOrder))
				{
				// Remove the tag and pass this on to the sub-tag command processor.
					NextCommand.Remove(0, SIZE_OF_TAG);


					ProcessMenuCommandSubTags(TagTable->Entries[TagTableEntry].SubTagTable,
								&NextCommand, TagTable->Entries[TagTableEntry].Tag, AffectedTable, false,
								&NumberOfSubTagsProcessed, &NumberOfSubTagResponsesNotReported,
								OverrideFixedFlag, ForceDirectDefaultCommandPrivilege);

					if(( NumberOfSubTagsProcessed == 0 ) || ( NumberOfSubTagsProcessed != NumberOfSubTagResponsesNotReported ))
					{
						MenuCommandAndResponse->Append(TagTable->Entries[TagTableEntry].Tag, SIZE_OF_TAG);

						if(NextCommand.Size)
						{
							MenuCommandAndResponse->Append(&NextCommand);
						}
						else
						{
						// We didn't find any entries for this tag, because they're probably all hidden, so we'll ENQ it.
							MenuCommandAndResponse->AppendNumber(((unsigned int) ENQ), 1);
							*bHadProblems=true;
						}
					}
				}

				else
				{
				// This tag didn't match; return the command with an ENQ.  Since the tag didn't match, and there
				// may have been sub-tags separated by commas, search for every comma and insert an ENQ before it.
					NextCommand.UpperCase();
					int		NextCommaPosition = 0;
					bool	CommaWasFound = true;
					char	ENQString[2]={ENQ, 0};
					while(CommaWasFound)
					{
						CommaWasFound = NextCommand.FindCharacterForward(',', (unsigned int) NextCommaPosition,
													&NextCommaPosition);
						if(CommaWasFound)
						{
						// Insert an ENQ.
							NextCommand.Insert(ENQString, NextCommaPosition);
							NextCommaPosition += 2;
						}
					}
					MenuCommandAndResponse->Append(&NextCommand);
					MenuCommandAndResponse->AppendNumber(((unsigned int) ENQ), 1);
					*bHadProblems=true;
				}
			}

			else
			{
			// This must be some type of query.
				HSTRING		UnalteredCommand(&NextCommand);
				bool		CommandStatus = true;		// Keep going unless we encounter a command error.
				int			TagTableEntryIndex;			// Actual index to look at next (may be sorted).

			// We have no explicit tag, so we need to loop through all of them.
				for(TagTableEntry = 0;    (TagTableEntry < TagTable->NumberOfEntries) && CommandStatus;    ++TagTableEntry)
				{
				// Are we sorting in order
					if( SortWildcardQueriesForThisCommand )
					{
						TagTableEntryIndex = TagTable->EntryIndex[ TagTableEntry ];
					}
					else
					{
						TagTableEntryIndex = TagTableEntry;
					}

				// Remove the tag and pass this on to the sub-tag command processor.
					CommandStatus = ProcessMenuCommandSubTags(TagTable->Entries[TagTableEntryIndex].SubTagTable, &NextCommand,
									TagTable->Entries[TagTableEntryIndex].Tag, AffectedTable, true,
									&NumberOfSubTagsProcessed, &NumberOfSubTagResponsesNotReported,
									OverrideFixedFlag, ForceDirectDefaultCommandPrivilege);

					if(( NumberOfSubTagsProcessed == 0 ) || ( NumberOfSubTagsProcessed != NumberOfSubTagResponsesNotReported ))
					{
						if( NextCommand.Size > 0 )
						{
							if( TagCompressedModeEnabledForThisCommand )
							{
								MenuCommandAndResponse->Append(TagTable->Entries[TagTableEntryIndex].Tag, SIZE_OF_TAG);
							}

							MenuCommandAndResponse->Append(&NextCommand);
						}
					}

				// If there is another tag to process, add a ";" to the end of the response.
					if(((TagTableEntry + 1) < TagTable->NumberOfEntries) &&  (NextCommand.Size > 0))
					{
						MenuCommandAndResponse->Append(";");
					}

					NextCommand.Copy(&UnalteredCommand);
				}

			// If the last character in the response string is a ";" (due to a hidden entry being skipped), remove it.
				if((MenuCommandAndResponse->Size != 0) &&
							(MenuCommandAndResponse->Char[MenuCommandAndResponse->Size - 1] == ';'))
				{
					MenuCommandAndResponse->Remove(MenuCommandAndResponse->Size - 1, 1);
				}
			}


		// If there is another command to process, add a ";" to the end of the response.
			if((ProcessAnotherCommandTag) && (NextCommand.Size > 0))
			{
				MenuCommandAndResponse->Append(";");
			}
		}
	} while(ProcessAnotherCommandTag);


	return;
} // ProcessMenuCommands





//
//	Menu (overloaded version #1 with HSTRING).
//		Process a menu setting command.
//
unsigned int READER_CONFIGURATION_STORAGE::Menu(HSTRING *MenuCommandAndResponse, bool OverrideFixedFlag, bool ForceDirectDefaultCommandPrivilege)
{
	char		AffectedTable;						// Either ".", "!", "^", or "&"
	bool		bHadProblems=false;



	OneOrMoreQueryCommandsWereProcessed = false;




// Initialize reporting controls.
	TagCompressedModeEnabledForThisCommand = TagCompressedModeEnabled;
	ReportBaseOnlySettingsForThisCommand = ReportBaseOnlySettings;
	ReportScannerOnlySettingsForThisCommand = ReportScannerOnlySettings;
	ReportCommonSettingsForThisCommand = ReportCommonSettings;
	WorkgroupSettingsHandlingForThisCommand = WorkgroupSettingsHandling;

	SettingsUpdatedSinceLastCommandModifiersForThisCommand = SettingsUpdatedSinceLastCommandModifiers;
	ReportSettingsUpdatedSinceLastCommandForThisCommand = ReportSettingsUpdatedSinceLastCommand;
	ReportSettingsNotUpdatedSinceLastCommandForThisCommand = ReportSettingsNotUpdatedSinceLastCommand;
	ReportSettingsUpdatedSinceStartupForThisCommand = ReportSettingsUpdatedSinceStartup;
	ReportSettingsNotUpdatedSinceStartupForThisCommand = ReportSettingsNotUpdatedSinceStartup;
	ReportSettingsWhenActiveIsEqualToWorkingForThisCommand = ReportSettingsWhenActiveIsEqualToWorking;
	ReportSettingsWhenActiveIsNotEqualToWorkingForThisCommand = ReportSettingsWhenActiveIsNotEqualToWorking;

	SortWildcardQueriesForThisCommand = SortWildcardQueries;

	ReportHiddenSettingsForThisCommand = ReportHiddenSettings;


// If there aren't any characters, return immediately.
	if(MenuCommandAndResponse->Size < 1)
	{
		return(MENU_ERROR);
	}


// Get the affected table.
	if( !GetAffectedTable( MenuCommandAndResponse, &AffectedTable ))
	{
		return(MENU_ERROR);
	}

	IsMenuingInProcess = false;
	ProcessMenuCommands( MenuCommandAndResponse, AffectedTable, OverrideFixedFlag, ForceDirectDefaultCommandPrivilege,
						&bHadProblems );
	IsMenuingInProcess = false;

// If the last character in the response string is a ";" (due to a hidden entry being skipped), remove it.
	if((MenuCommandAndResponse->Size != 0) &&
							(MenuCommandAndResponse->Char[MenuCommandAndResponse->Size - 1] == ';'))
	{
		MenuCommandAndResponse->Remove(MenuCommandAndResponse->Size - 1, 1);
	}


// Before we return, tack the affected table character onto the end of the response.
	if( MenuCommandAndResponse->Size > 0 )
	{
		MenuCommandAndResponse->AppendNumber(AffectedTable, 1);
	}


// Before we return, switch back to the default workgroup number if we're not already there.
	if( CurrentWorkgroupNumber != ActiveWorkgroupNumber )
	{
		ChangeToWorkgroup( ActiveWorkgroupNumber, true );
	}


// Before we return, restore reporting controls in case anyone else is going to be using them and doesn't think to initialize them.
	TagCompressedModeEnabledForThisCommand = TagCompressedModeEnabled;
	ReportBaseOnlySettingsForThisCommand = ReportBaseOnlySettings;
	ReportScannerOnlySettingsForThisCommand = ReportScannerOnlySettings;
	ReportCommonSettingsForThisCommand = ReportCommonSettings;
	WorkgroupSettingsHandlingForThisCommand = WorkgroupSettingsHandling;

	SettingsUpdatedSinceLastCommandModifiersForThisCommand = SettingsUpdatedSinceLastCommandModifiers;
	ReportSettingsUpdatedSinceLastCommandForThisCommand = ReportSettingsUpdatedSinceLastCommand;
	ReportSettingsNotUpdatedSinceLastCommandForThisCommand = ReportSettingsNotUpdatedSinceLastCommand;
	ReportSettingsUpdatedSinceStartupForThisCommand = ReportSettingsUpdatedSinceStartup;
	ReportSettingsNotUpdatedSinceStartupForThisCommand = ReportSettingsNotUpdatedSinceStartup;
	ReportSettingsWhenActiveIsEqualToWorkingForThisCommand = ReportSettingsWhenActiveIsEqualToWorking;
	ReportSettingsWhenActiveIsNotEqualToWorkingForThisCommand = ReportSettingsWhenActiveIsNotEqualToWorking;

	SortWildcardQueriesForThisCommand = SortWildcardQueries;

	ReportHiddenSettingsForThisCommand = ReportHiddenSettings;
	
// Recompute CRC
//	LastCRC = CalculateCRC();

	if( bHadProblems )
		return(MENU_ERROR);
	else if(OneOrMoreQueryCommandsWereProcessed)
		return(MENU_QUERY);
	else
		return(MENU_PASSED);

} // Menu (overloaded version #1)



//
//	Menu (overloaded version #2 with character string.
//		Process a menu setting command.
//
unsigned int READER_CONFIGURATION_STORAGE::Menu(unsigned int *LengthOfMenuCommandAndResponse, unsigned char *MenuCommandAndResponse,
															bool OverrideFixedFlag, bool ForceDirectDefaultCommandPrivilege)
{
	HSTRING			OriginalCommand(MenuCommandAndResponse, *LengthOfMenuCommandAndResponse, true);
	unsigned int	Status;

	Status = Menu(&OriginalCommand, OverrideFixedFlag, ForceDirectDefaultCommandPrivilege);
	OriginalCommand.CopyFromString(MenuCommandAndResponse, OriginalCommand.Size + 1);
	*LengthOfMenuCommandAndResponse = OriginalCommand.Size;
	MenuCommandAndResponse[*LengthOfMenuCommandAndResponse] = 0;

	return(Status);
} // Menu (overloaded version #2)






//
//	CustomerDefaultsUpdated
//		Check to see if one or more menu settings have been updated in the customer defaults table.
//
bool READER_CONFIGURATION_STORAGE::CustomerDefaultsUpdated(void)
{
/*
	unsigned int	TagTableEntry;								// Entry table index.
	unsigned int	SubTagTableEntry;							// Entry table index.
	bool			OneOrMoreCustomerDefaultsUpdated = false;	// No updated entries found yet.


// Keep going until we find updated entries or we reach the end of the tag table.
	TagTableEntry = 0;
	while((TagTableEntry < TagTable->NumberOfEntries) && (!OneOrMoreCustomerDefaultsUpdated))
	{
	// Check the next sub-tag table.
		SubTagTableEntry = 0;
		while((SubTagTableEntry < TagTable->Entries[TagTableEntry].SubTagTable->NumberOfEntries) && (!OneOrMoreCustomerDefaultsUpdated))
		{
		// Check the next sub-tag table item.
			OneOrMoreCustomerDefaultsUpdated = TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].DefaultUpdated;
			++SubTagTableEntry;
		}
		++TagTableEntry;
	}

	return(OneOrMoreCustomerDefaultsUpdated);
*/
	return(OneOrMoreDefaultsUpdated);
} // CustomerDefaultsUpdated





//
//	WorkingValuesUpdated
//		Check to see if one or more menu settings have been updated in the stored values table.
//
bool READER_CONFIGURATION_STORAGE::WorkingValuesUpdated(void)
{
/*
	unsigned int	TagTableEntry;								// Entry table index.
	unsigned int	SubTagTableEntry;							// Entry table index.
	bool			OneOrMoreWorkingValuesUpdated = false;		// No updated entries found yet.


// Keep going until we find updated entries or we reach the end of the tag table.
	TagTableEntry = 0;
	while((TagTableEntry < TagTable->NumberOfEntries) && (!OneOrMoreWorkingValuesUpdated))
	{
	// Check the next sub-tag table.
		SubTagTableEntry = 0;
		while((SubTagTableEntry < TagTable->Entries[TagTableEntry].SubTagTable->NumberOfEntries) && (!OneOrMoreWorkingValuesUpdated))
		{
		// Check the next sub-tag table item.
			OneOrMoreWorkingValuesUpdated = TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].WorkingValueUpdated;
			++SubTagTableEntry;
		}
		++TagTableEntry;
	}

	return(OneOrMoreWorkingValuesUpdated);
*/
	return(OneOrMoreWorkingValuesUpdated);
} // WorkingValuesUpdated





//
//	ActiveValuesWereDefaulted
//		Check to see if all the active values have been defaulted.
//
bool READER_CONFIGURATION_STORAGE::ActiveValuesWereDefaulted(void)
{
	return AllActiveValuesWereDefaulted;
} // ActiveValuesWereDefaulted





//
//	AtLeastOneActiveValueWasDefaulted
//		Check to see if one or more of the active values have been defaulted.
//
bool READER_CONFIGURATION_STORAGE::AtLeastOneActiveValueWasDefaulted(void)
{
	return OneOrMoreActiveValuesWereDefaulted;
} // AtLeastOneActiveValueWasDefaulted





//
//	ActiveValuesUpdated
//		Check to see if one or more menu settings have been updated in the active values table.
//
bool READER_CONFIGURATION_STORAGE::ActiveValuesUpdated(void)
{
/*
	unsigned int	TagTableEntry;								// Entry table index.
	unsigned int	SubTagTableEntry;							// Entry table index.
	bool			OneOrMoreActiveValuesUpdated = false;		// No updated entries found yet.


// Keep going until we find updated entries or we reach the end of the tag table.
	TagTableEntry = 0;
	while((TagTableEntry < TagTable->NumberOfEntries) && (!OneOrMoreActiveValuesUpdated))
	{
	// Check the next sub-tag table.
		SubTagTableEntry = 0;
		while((SubTagTableEntry < TagTable->Entries[TagTableEntry].SubTagTable->NumberOfEntries) && (!OneOrMoreActiveValuesUpdated))
		{
		// Check the next sub-tag table item.
			OneOrMoreActiveValuesUpdated = TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].ActiveValueUpdated;
			++SubTagTableEntry;
		}
		++TagTableEntry;
	}

	return(OneOrMoreActiveValuesUpdated);
*/
	return(OneOrMoreActiveValuesUpdated);
} // ActiveValuesUpdated





//
//	WorkingWorkgroupValuesUpdated
//		Check to see if one or more workgroup menu settings have been updated in the working values table.
//
bool READER_CONFIGURATION_STORAGE::WorkingWorkgroupValuesUpdated(void)
{
	return OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated;
} // WorkingWorkgroupValuesUpdated





//
//	ActiveWorkgroupValuesUpdated
//		Check to see if one or more workgroup menu settings have been updated in the active values table.
//
bool READER_CONFIGURATION_STORAGE::ActiveWorkgroupValuesUpdated(void)
{
	return OneOrMoreWorkgroupValuesInActiveStorageWereUpdated;
} // ActiveWorkgroupValuesUpdated





//
//	WorkingWorkgroupValuesInTableUpdated
//		Check to see if one or more workgroup menu settings have been updated in the working values table referenced by the
//		caller.
//
bool READER_CONFIGURATION_STORAGE::WorkingWorkgroupValuesInTableUpdated( int WorkgroupTableNumber )
{
	if( ( WorkgroupTableNumber >= 0 ) && ( WorkgroupTableNumber < NumberOfWorkgroups ) )
	{
		return OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated[ WorkgroupTableNumber ];
	}
	else
	{
		return false;
	}
} // WorkingWorkgroupValuesInTableUpdated





//
//	ActiveWorkgroupValuesInTableUpdated
//		Check to see if one or more workgroup menu settings have been updated in the active values table referenced by the
//		caller.
//
bool READER_CONFIGURATION_STORAGE::ActiveWorkgroupValuesInTableUpdated( int WorkgroupTableNumber )
{
	if( ( WorkgroupTableNumber >= 0 ) && ( WorkgroupTableNumber < NumberOfWorkgroups ) )
	{
		return OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ WorkgroupTableNumber ];
	}
	else
	{
		return false;
	}
} // ActiveWorkgroupValuesInTableUpdated





//
//	WorkingBaseOnlyValuesUpdated
//		Check to see if one or more base only menu settings have been updated in the working values table.
//
bool READER_CONFIGURATION_STORAGE::WorkingBaseOnlyValuesUpdated(void)
{
	return OneOrMoreBaseOnlyValuesInWorkingStorageWereUpdated;
} // WorkingBaseOnlyValuesUpdated





//
//	ActiveBaseOnlyValuesUpdated
//		Check to see if one or more base only menu settings have been updated in the active values table.
//
bool READER_CONFIGURATION_STORAGE::ActiveBaseOnlyValuesUpdated(void)
{
	return OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated;
} // ActiveBaseOnlyValuesUpdated





//
//	WorkingScannerOnlyValuesUpdated
//		Check to see if one or more scanner only menu settings have been updated in the working values table.
//
bool READER_CONFIGURATION_STORAGE::WorkingScannerOnlyValuesUpdated(void)
{
	return OneOrMoreScannerOnlyValuesInWorkingStorageWereUpdated;
} // WorkingScannerOnlyValuesUpdated





//
//	ActiveScannerOnlyValuesUpdated
//		Check to see if one or more scanner only menu settings have been updated in the active values table.
//
bool READER_CONFIGURATION_STORAGE::ActiveScannerOnlyValuesUpdated(void)
{
	return OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated;
} // ActiveScannerOnlyValuesUpdated





//
//	WorkingCommonValuesUpdated
//		Check to see if one or more common menu settings have been updated in the working values table.
//
bool READER_CONFIGURATION_STORAGE::WorkingCommonValuesUpdated(void)
{
	return OneOrMoreCommonValuesInWorkingStorageWereUpdated;
} // WorkingCommonValuesUpdated





//
//	ActiveCommonValuesUpdated
//		Check to see if one or more common menu settings have been updated in the active values table.
//
bool READER_CONFIGURATION_STORAGE::ActiveCommonValuesUpdated(void)
{
	return OneOrMoreCommonValuesInActiveStorageWereUpdated;
} // ActiveCommonValuesUpdated





//
//	CustomerDefaultsUpToDate
//		Mark all menu settings in the customer defaults table as being up to date.
//
void READER_CONFIGURATION_STORAGE::CustomerDefaultsUpToDate(void)
{
	unsigned int		TagTableEntry;								// Entry table index.
	unsigned int		SubTagTableEntry;							// Entry table index.


// Process all entries in the tag table.
	for(TagTableEntry = 0;   TagTableEntry < TagTable->NumberOfEntries;    ++TagTableEntry)
	{
	// Process all entries in the sub-tag table.
		for(SubTagTableEntry = 0;   SubTagTableEntry < TagTable->Entries[TagTableEntry].SubTagTable->NumberOfEntries;
																				++SubTagTableEntry)
		{
		// Update the next sub-tag table item.
			TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].DefaultValue.Updated = false;
		}
	}

	OneOrMoreDefaultsUpdated = false;

	return;
} // CustomerDefaultsUpToDate




//
//	WorkingValuesUpToDate
//		Mark all menu settings in the stored values table as being up to date.
//
void READER_CONFIGURATION_STORAGE::WorkingValuesUpToDate(void)
{
	unsigned int		TagTableEntry;								// Entry table index.
	unsigned int		SubTagTableEntry;							// Entry table index.
	int					i;

	SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*WorkingValue;

// Process all entries in the tag table.
	for(TagTableEntry = 0;   TagTableEntry < TagTable->NumberOfEntries;    ++TagTableEntry)
	{
	// Process all entries in the sub-tag table.
		for(SubTagTableEntry = 0;   SubTagTableEntry < TagTable->Entries[TagTableEntry].SubTagTable->NumberOfEntries;
																				++SubTagTableEntry)
		{
		// Update the next sub-tag table item.
			WorkingValue = WorkingValueForCurrentWorkgroup( &TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry] );
			WorkingValue->Updated = false;
		}
	}

	OneOrMoreWorkingValuesUpdated = false;
	OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated = false;
	OneOrMoreBaseOnlyValuesInWorkingStorageWereUpdated = false;
	OneOrMoreScannerOnlyValuesInWorkingStorageWereUpdated = false;
	OneOrMoreCommonValuesInWorkingStorageWereUpdated = false;
	for( i = 0;    i < NumberOfWorkgroups;    ++i )
	{
		OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated[ i ] = false;
	}

	return;
} // WorkingValuesUpToDate




//
//	ActiveValuesUpToDate
//		Mark all menu settings in the active values table as being up to date.
//
void READER_CONFIGURATION_STORAGE::ActiveValuesUpToDate(void)
{
	unsigned int		TagTableEntry;								// Entry table index.
	unsigned int		SubTagTableEntry;							// Entry table index.
	int					i;


// Process all entries in the tag table.
	for(TagTableEntry = 0;   TagTableEntry < TagTable->NumberOfEntries;    ++TagTableEntry)
	{
	// Process all entries in the sub-tag table.
		for(SubTagTableEntry = 0;   SubTagTableEntry < TagTable->Entries[TagTableEntry].SubTagTable->NumberOfEntries;
																				++SubTagTableEntry)
		{
		// Update the next sub-tag table item.
			TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].InstantiatorCopyOfActiveValue.Updated = false;
		}
	}

	AllActiveValuesWereDefaulted = false;
	OneOrMoreActiveValuesWereDefaulted = false;
	OneOrMoreActiveValuesUpdated = false;
	OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = false;
	OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = false;
	OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = false;
	OneOrMoreCommonValuesInActiveStorageWereUpdated = false;
	for( i = 0;    i < NumberOfWorkgroups;    ++i )
	{
		OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ i ] = false;
	}

	return;
} // ActiveValuesUpToDate





//
//	ClearCustomerDefaultsModifiedSinceCreationFlag
//		Mark all menu settings in the customer defaults table as unmodified since creation.
//
void READER_CONFIGURATION_STORAGE::ClearCustomerDefaultsModifiedSinceCreationFlag(void)
{
	unsigned int		TagTableEntry;								// Entry table index.
	unsigned int		SubTagTableEntry;							// Entry table index.


// Process all entries in the tag table.
	for(TagTableEntry = 0;   TagTableEntry < TagTable->NumberOfEntries;    ++TagTableEntry)
	{
	// Process all entries in the sub-tag table.
		for(SubTagTableEntry = 0;   SubTagTableEntry < TagTable->Entries[TagTableEntry].SubTagTable->NumberOfEntries;
																				++SubTagTableEntry)
		{
		// Update the next sub-tag table item.
			TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].DefaultValue.Updated = false;
		}
	}

	return;
} // ClearCustomerDefaultsModifiedSinceCreationFlag




//
//	ClearWorkingValuesModifiedSinceCreationFlag
//		Mark all menu settings in the stored values table as unmodified since creation.
//
void READER_CONFIGURATION_STORAGE::ClearWorkingValuesModifiedSinceCreationFlag(void)
{
	unsigned int		TagTableEntry;								// Entry table index.
	unsigned int		SubTagTableEntry;							// Entry table index.

	SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*WorkingValue;

// Process all entries in the tag table.
	for(TagTableEntry = 0;   TagTableEntry < TagTable->NumberOfEntries;    ++TagTableEntry)
	{
	// Process all entries in the sub-tag table.
		for(SubTagTableEntry = 0;   SubTagTableEntry < TagTable->Entries[TagTableEntry].SubTagTable->NumberOfEntries;
																				++SubTagTableEntry)
		{
		// Update the next sub-tag table item.
			WorkingValue = WorkingValueForCurrentWorkgroup( &TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry] );
			WorkingValue->Updated = false;
		}
	}
	return;
} // ClearWorkingValuesModifiedSinceCreationFlag




//
//	ClearActiveValuesModifiedSinceCreationFlag
//		Mark all menu settings in the active values table as unmodified since creation.
//
void READER_CONFIGURATION_STORAGE::ClearActiveValuesModifiedSinceCreationFlag(void)
{
	unsigned int		TagTableEntry;								// Entry table index.
	unsigned int		SubTagTableEntry;							// Entry table index.


// Process all entries in the tag table.
	for(TagTableEntry = 0;   TagTableEntry < TagTable->NumberOfEntries;    ++TagTableEntry)
	{
	// Process all entries in the sub-tag table.
		for(SubTagTableEntry = 0;   SubTagTableEntry < TagTable->Entries[TagTableEntry].SubTagTable->NumberOfEntries;
																				++SubTagTableEntry)
		{
		// Update the next sub-tag table item.
			TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].InstantiatorCopyOfActiveValue.Updated = false;
		}
	}

	return;
} // ClearActiveValuesModifiedSinceCreationFlag






// Copy standard defaults to defaults.
void READER_CONFIGURATION_STORAGE::CopyStandardDefaultsToDefaults(void)
{
	unsigned int		TagTableEntry;								// Entry table index.
	unsigned int		SubTagTableEntry;							// Entry table index.


// Process all entries in the tag table.
	for(TagTableEntry = 0;   TagTableEntry < TagTable->NumberOfEntries;    ++TagTableEntry)
	{
	// Process all entries in the sub-tag table.
		for(SubTagTableEntry = 0;   SubTagTableEntry < TagTable->Entries[TagTableEntry].SubTagTable->NumberOfEntries;
																				++SubTagTableEntry)
		{
		// Copy the next sub-tag table item.
			CopyStandardDefaultToDefault(&(TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry]));
		}
	}

	return;
}






// Copy defaults to working and active settings.
void READER_CONFIGURATION_STORAGE::CopyDefaultsToWorkingAndActive( const char AffectedTable, bool DefaultAllWorkgroups )
{
	unsigned int		TagTableEntry;								// Entry table index.
	unsigned int		SubTagTableEntry;							// Entry table index.


// Process all entries in the tag table.
	for(TagTableEntry = 0;   TagTableEntry < TagTable->NumberOfEntries;    ++TagTableEntry)
	{
	// Process all entries in the sub-tag table.
		for(SubTagTableEntry = 0;   SubTagTableEntry < TagTable->Entries[TagTableEntry].SubTagTable->NumberOfEntries;
																				++SubTagTableEntry)
		{
			if(( OverrideDefaultingDisabled ) ||
				( !( TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].TypeFlags & SUB_TAG_TABLE::DEFAULTING_DISABLED )))
			{
			// Copy the next sub-tag table item.
				CopyDefaultToWorking(&(TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry]), AffectedTable);
				CopyWorkingToActive(&(TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry]));
			}

			if( DefaultAllWorkgroups )
			{
				ReplicateAcrossWorkgroups( &(TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry]), '.' );
			}
		}
	}

	return;
}





// Copy working settings to defaults.
void READER_CONFIGURATION_STORAGE::CopyWorkingToDefaults(void)
{
	unsigned int		TagTableEntry;								// Entry table index.
	unsigned int		SubTagTableEntry;							// Entry table index.


// Process all entries in the tag table.
	for(TagTableEntry = 0;   TagTableEntry < TagTable->NumberOfEntries;    ++TagTableEntry)
	{
	// Process all entries in the sub-tag table.
		for(SubTagTableEntry = 0;   SubTagTableEntry < TagTable->Entries[TagTableEntry].SubTagTable->NumberOfEntries;
																				++SubTagTableEntry)
		{
		// Copy the next sub-tag table item.
			CopyWorkingToDefault(&(TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry]));
		}
	}

	return;
}






// Copy working setings over active settings.
void READER_CONFIGURATION_STORAGE::CopyWorkingSettingsOverActiveSettings(void)
{
	unsigned int		TagTableEntry;								// Entry table index.
	unsigned int		SubTagTableEntry;							// Entry table index.


// Process all entries in the tag table.
	for(TagTableEntry = 0;   TagTableEntry < TagTable->NumberOfEntries;    ++TagTableEntry)
	{
	// Process all entries in the sub-tag table.
		for(SubTagTableEntry = 0;   SubTagTableEntry < TagTable->Entries[TagTableEntry].SubTagTable->NumberOfEntries;
																				++SubTagTableEntry)
		{
		// Copy the next sub-tag table item.
			CopyWorkingToActive(&(TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry]));
		}
	}

	return;
}






// Restore all settings to their default values.
void READER_CONFIGURATION_STORAGE::RestoreAllSettingsToDefault( bool DefaultAllWorkgroups )
{
	CopyDefaultsToWorkingAndActive( DefaultAllWorkgroups );
}





// Restore all settings to their default values.
bool READER_CONFIGURATION_STORAGE::RestoreAllSettingsWithThisTagToDefault(const char *TagName, const char AffectedTable)
{
	bool			Status = true;		// Assume success until we know otherwise.
	unsigned int	TagTableEntryNumberToRestore;
	unsigned int	OrderOfTagTableEntryNumberToRestore;


	Status = TagTable->Find(TagName, &TagTableEntryNumberToRestore, &OrderOfTagTableEntryNumberToRestore);
	if(Status)
	{
		unsigned int		SubTagTableEntry;							// Entry table index.


	// Process all entries in the sub-tag table.
		for(SubTagTableEntry = 0;    SubTagTableEntry < TagTable->Entries[TagTableEntryNumberToRestore].SubTagTable->NumberOfEntries;
																				++SubTagTableEntry)
		{
			if(( OverrideDefaultingDisabled ) ||
				( !( TagTable->Entries[TagTableEntryNumberToRestore].SubTagTable->Entries[SubTagTableEntry].TypeFlags & SUB_TAG_TABLE::DEFAULTING_DISABLED )))
			{
			// Copy the next sub-tag table item.
				CopyDefaultToWorking(&(TagTable->Entries[TagTableEntryNumberToRestore].SubTagTable->Entries[SubTagTableEntry]), AffectedTable);
				CopyWorkingToActive(&(TagTable->Entries[TagTableEntryNumberToRestore].SubTagTable->Entries[SubTagTableEntry]));
			}
		}
	}

	return(Status);
}





// Restore an individual setting to its' default values.
bool READER_CONFIGURATION_STORAGE::RestoreThisSettingToDefault(const char *SettingName)
{
	bool			Status = true;		// Assume success until we know otherwise.
	unsigned int	TagTableEntryNumberToRestore;
	unsigned int	OrderOfTagTableEntryNumberToRestore;
	unsigned int	SubTagTableEntryNumberToRestore;
	unsigned int	OrderOfSubTagTableEntryNumberToRestore;


	Status = TagTable->Find(SettingName, &TagTableEntryNumberToRestore, &OrderOfTagTableEntryNumberToRestore);
	if(Status)
	{
		Status = TagTable->Entries[TagTableEntryNumberToRestore].SubTagTable->Find(&SettingName[SIZE_OF_TAG],
										&SubTagTableEntryNumberToRestore, &OrderOfSubTagTableEntryNumberToRestore);
		if(Status)
		{
			if(( OverrideDefaultingDisabled ) ||
				( !( TagTable->Entries[TagTableEntryNumberToRestore].SubTagTable->Entries[SubTagTableEntryNumberToRestore].TypeFlags & SUB_TAG_TABLE::DEFAULTING_DISABLED )))
			{
			// Copy the next sub-tag table item.
				CopyDefaultToWorking
					(&(TagTable->Entries[TagTableEntryNumberToRestore].SubTagTable->Entries[SubTagTableEntryNumberToRestore]));
				CopyWorkingToActive
					(&(TagTable->Entries[TagTableEntryNumberToRestore].SubTagTable->Entries[SubTagTableEntryNumberToRestore]));
			}
		}
	}

	return(Status);
}





//
//	EnableAllSymbologies
//		Find all symbology enable settings and turn on values in the affected table.
//
bool READER_CONFIGURATION_STORAGE::EnableAllSymbologies(char AffectedTable)
{
	unsigned int	TagTableEntry;								// Entry table index.
	unsigned int	SubTagTableEntry;							// Entry table index.
	bool			Status = true;								// Assume success until we know otherwise.
	SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*WorkingValue;



// Is the table valid?
	if((AffectedTable == '!') || (AffectedTable == '.') || (AffectedTable == '@') || (AffectedTable == '^'))
	{
	// Process all entries in the tag table.
		for(TagTableEntry = 0;   TagTableEntry < TagTable->NumberOfEntries;    ++TagTableEntry)
		{
		// Process all entries in the sub-tag table.
			for(SubTagTableEntry = 0;   SubTagTableEntry < TagTable->Entries[TagTableEntry].SubTagTable->NumberOfEntries;
																				++SubTagTableEntry)
			{
			// If this setting is a boolean and a symbology enable setting, turn it on.
				if((TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].TypeFlags & SUB_TAG_TABLE::BOOLEAN) &&
					(TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].TypeFlags & SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND))
				{
					if((AffectedTable == '!') || (AffectedTable == '.'))
					{
						*(TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].InstantiatorCopyOfActiveValue.b) = true;
						TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].InstantiatorCopyOfActiveValue.Updated = true;
						TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
						OneOrMoreActiveValuesUpdated = true;

						if( TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].TypeFlags &
											SUB_TAG_TABLE::WORKGROUP_SETTING )
						{
							OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
							OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
						}
						else if( TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].TypeFlags &
											SUB_TAG_TABLE::BASE_ONLY_SETTING )
						{
							OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
						}
						else if( TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].TypeFlags &
											SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
						{
							OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
						}
						else
						{
							OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
						}
					}

					if(AffectedTable == '.')
					{
						WorkingValue = WorkingValueForCurrentWorkgroup( &TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry] );
						WorkingValue->b = true;
						WorkingValue->Updated = true;
						WorkingValue->ModifiedSinceInstantiation = true;
						OneOrMoreWorkingValuesUpdated = true;

						if( TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].TypeFlags &
											SUB_TAG_TABLE::WORKGROUP_SETTING )
						{
							OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated = true;
							OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
						}
						else if( TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].TypeFlags &
											SUB_TAG_TABLE::BASE_ONLY_SETTING )
						{
							OneOrMoreBaseOnlyValuesInWorkingStorageWereUpdated = true;
						}
						else if( TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].TypeFlags &
											SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
						{
							OneOrMoreScannerOnlyValuesInWorkingStorageWereUpdated = true;
						}
						else
						{
							OneOrMoreCommonValuesInWorkingStorageWereUpdated = true;
						}
					}

					if((AffectedTable == '@') || (AffectedTable == '^'))
					{
						TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].DefaultValue.b = true;
						TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].DefaultValue.Updated = true;
						TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].DefaultValue.ModifiedSinceInstantiation = true;
						OneOrMoreDefaultsUpdated = true;
					}
				}
			}
		}
	}

	else
	{
	// Table is invalid; WOOF;
		Status = false;
	}


	return(Status);
} // EnableAllSymbologies





//
//	DisableAllSymbologies
//		Find all symbology enable settings and turn off values in the affected table.
//
bool READER_CONFIGURATION_STORAGE::DisableAllSymbologies(char AffectedTable)
{
	unsigned int	TagTableEntry;								// Entry table index.
	unsigned int	SubTagTableEntry;							// Entry table index.
	bool			Status = true;								// Assume success until we know otherwise.
	SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*WorkingValue;


// Is the table valid?
	if((AffectedTable == '!') || (AffectedTable == '.') || (AffectedTable == '@') || (AffectedTable == '^'))
	{
	// Process all entries in the tag table.
		for(TagTableEntry = 0;   TagTableEntry < TagTable->NumberOfEntries;    ++TagTableEntry)
		{
		// Process all entries in the sub-tag table.
			for(SubTagTableEntry = 0;   SubTagTableEntry < TagTable->Entries[TagTableEntry].SubTagTable->NumberOfEntries;
																				++SubTagTableEntry)
			{
			// If this setting is a boolean and a symbology enable setting, turn it off.
				if((TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].TypeFlags & SUB_TAG_TABLE::BOOLEAN) &&
					(TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].TypeFlags & SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND))
				{
					if((AffectedTable == '!') || (AffectedTable == '.'))
					{
						*(TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].InstantiatorCopyOfActiveValue.b) = false;
						TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].InstantiatorCopyOfActiveValue.Updated = true;
						TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].InstantiatorCopyOfActiveValue.ModifiedSinceInstantiation = true;
						OneOrMoreActiveValuesUpdated = true;

						if( TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].TypeFlags &
											SUB_TAG_TABLE::WORKGROUP_SETTING )
						{
							OneOrMoreWorkgroupValuesInActiveStorageWereUpdated = true;
							OneOrMoreWorkgroupValuesInTableInActiveStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
						}
						else if( TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].TypeFlags &
											SUB_TAG_TABLE::BASE_ONLY_SETTING )
						{
							OneOrMoreBaseOnlyValuesInActiveStorageWereUpdated = true;
						}
						else if( TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].TypeFlags &
											SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
						{
							OneOrMoreScannerOnlyValuesInActiveStorageWereUpdated = true;
						}
						else
						{
							OneOrMoreCommonValuesInActiveStorageWereUpdated = true;
						}
					}

					if(AffectedTable == '.')
					{
						WorkingValue = WorkingValueForCurrentWorkgroup( &TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry] );
						WorkingValue->b = false;
						WorkingValue->Updated = true;
						WorkingValue->ModifiedSinceInstantiation = true;
						OneOrMoreWorkingValuesUpdated = true;

						if( TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].TypeFlags &
											SUB_TAG_TABLE::WORKGROUP_SETTING )
						{
							OneOrMoreWorkgroupValuesInWorkingStorageWereUpdated = true;
							OneOrMoreWorkgroupValuesInTableInWorkingStorageWereUpdated[ CurrentWorkgroupNumber ] = true;
						}
						else if( TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].TypeFlags &
											SUB_TAG_TABLE::BASE_ONLY_SETTING )
						{
							OneOrMoreBaseOnlyValuesInWorkingStorageWereUpdated = true;
						}
						else if( TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].TypeFlags &
											SUB_TAG_TABLE::SCANNER_ONLY_SETTING )
						{
							OneOrMoreScannerOnlyValuesInWorkingStorageWereUpdated = true;
						}
						else
						{
							OneOrMoreCommonValuesInWorkingStorageWereUpdated = true;
						}
					}

					if((AffectedTable == '@') || (AffectedTable == '^'))
					{
						TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].DefaultValue.b = false;
						TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].DefaultValue.Updated = true;
						TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry].DefaultValue.ModifiedSinceInstantiation = true;
						OneOrMoreDefaultsUpdated = true;
					}
				}
			}
		}
	}

	else
	{
	// Table is invalid; WOOF;
		Status = false;
	}


	return(Status);
} // DisableAllSymbologies





//
//	ChangeNumericRange (overloaded version #1, where min and max are addresses)
//		Change the range of a numeric item.
//
bool READER_CONFIGURATION_STORAGE::ChangeNumericRange(const char *SettingName, int MinimumRange, int *MinimumRangeAddress,
			int MaximumRange, int *MaximumRangeAddress)
{
	bool			Status = true;		// Assume success until we know otherwise.
	unsigned int	TagTableEntry;
	unsigned int	OrderOfTagTableEntry;


	Status = TagTable->Find(SettingName, &TagTableEntry, &OrderOfTagTableEntry);
	if(Status)
	{
		unsigned int		SubTagTableEntry;							// Entry table index.
		unsigned int		OrderOfSubTagTableEntry;					// Order of entry

		Status = TagTable->Entries[TagTableEntry].SubTagTable->Find(&SettingName[SIZE_OF_TAG], &SubTagTableEntry, &OrderOfSubTagTableEntry);

		if(Status)
		{
			Status = UpdateNumericRange(&(TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry]),
							MinimumRange, MinimumRangeAddress, MaximumRange, MaximumRangeAddress);
		}
	}

	return(Status);
} // ChangeNumericRange (overloaded version #1)





//
//	ChangeNumericRange (overloaded version #2, where min is an address and max is a constant)
//		Change the range of a numeric item.
//
bool READER_CONFIGURATION_STORAGE::ChangeNumericRange(const char *SettingName, int MinimumRange, int *MinimumRangeAddress,
			int MaximumRange)
{
	bool			Status = true;		// Assume success until we know otherwise.
	unsigned int	TagTableEntry;
	unsigned int	OrderOfTagTableEntry;

	TRACE2( "Attempting to change numeric range, max range values to new standard default %d for setting %s\r\n",
					MaximumRange, SettingName );

	Status = TagTable->Find(SettingName, &TagTableEntry, &OrderOfTagTableEntry);
	if(Status)
	{
		unsigned int		SubTagTableEntry;							// Entry table index.
		unsigned int		OrderOfSubTagTableEntry;					// Order of entry

		Status = TagTable->Entries[TagTableEntry].SubTagTable->Find(&SettingName[SIZE_OF_TAG], &SubTagTableEntry, &OrderOfSubTagTableEntry);

		if(Status)
		{
			Status = UpdateNumericRange(&(TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry]),
							MinimumRange, MinimumRangeAddress, MaximumRange);
		}
	}

	return(Status);
} // ChangeNumericRange (overloaded version #2)





//
//	ChangeNumericRange (overloaded version #3, where min is a constant and max is an address)
//		Change the range of a numeric item.
//
bool READER_CONFIGURATION_STORAGE::ChangeNumericRange(const char *SettingName, int MinimumRange, int MaximumRange,
			int *MaximumRangeAddress)
{
	bool			Status = true;		// Assume success until we know otherwise.
	unsigned int	TagTableEntry;
	unsigned int	OrderOfTagTableEntry;


	Status = TagTable->Find(SettingName, &TagTableEntry, &OrderOfTagTableEntry);
	if(Status)
	{
		unsigned int		SubTagTableEntry;							// Entry table index.
		unsigned int		OrderOfSubTagTableEntry;					// Order of entry

		Status = TagTable->Entries[TagTableEntry].SubTagTable->Find(&SettingName[SIZE_OF_TAG], &SubTagTableEntry, &OrderOfSubTagTableEntry);

		if(Status)
		{
			Status = UpdateNumericRange(&(TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry]),
							MinimumRange, MaximumRange, MaximumRangeAddress);
		}
	}

	return(Status);
} // ChangeNumericRange (overloaded version #3)





//
//	ChangeNumericRange (overloaded version #4, where min and max are constants)
//		Change the range of a numeric item.
//
bool READER_CONFIGURATION_STORAGE::ChangeNumericRange(const char *SettingName, int MinimumRange, int MaximumRange)
{
	bool			Status = true;		// Assume success until we know otherwise.
	unsigned int	TagTableEntry;
	unsigned int	OrderOfTagTableEntry;


	Status = TagTable->Find(SettingName, &TagTableEntry, &OrderOfTagTableEntry);
	if(Status)
	{
		unsigned int		SubTagTableEntry;							// Entry table index.
		unsigned int		OrderOfSubTagTableEntry;					// Order of entry

		Status = TagTable->Entries[TagTableEntry].SubTagTable->Find(&SettingName[SIZE_OF_TAG], &SubTagTableEntry, &OrderOfSubTagTableEntry);

		if(Status)
		{
			Status = UpdateNumericRange(&(TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry]),
							MinimumRange, MaximumRange);
		}
	}

	return(Status);
} // ChangeNumericRange (overloaded version #4)







//
//	ChangeNumericList (overloaded version #1, with individual list items)
//		Change the list for a numeric list item
//
bool READER_CONFIGURATION_STORAGE::ChangeNumericList(const char *SettingName,
		int Item1, int Item2, int Item3, int Item4, int Item5, int Item6, int Item7, int Item8, int Item9, int Item10,
		int Item11, int Item12, int Item13, int Item14, int Item15, int Item16, int Item17, int Item18, int Item19, int Item20,
		int Item21, int Item22, int Item23, int Item24, int Item25, int Item26, int Item27, int Item28, int Item29, int Item30,
		int Item31, int Item32, int Item33, int Item34, int Item35, int Item36, int Item37, int Item38, int Item39, int Item40,
		int Item41, int Item42, int Item43, int Item44, int Item45, int Item46, int Item47, int Item48, int Item49, int Item50,
		int Item51, int Item52, int Item53, int Item54, int Item55, int Item56, int Item57, int Item58, int Item59, int Item60)
{
	bool			Status = true;		// Assume success until we know otherwise.
	unsigned int	TagTableEntry;
	unsigned int	OrderOfTagTableEntry;


	Status = TagTable->Find(SettingName, &TagTableEntry, &OrderOfTagTableEntry);
	if(Status)
	{
		unsigned int		SubTagTableEntry;							// Entry table index.
		unsigned int		OrderOfSubTagTableEntry;					// Order of entry

		Status = TagTable->Entries[TagTableEntry].SubTagTable->Find(&SettingName[SIZE_OF_TAG], &SubTagTableEntry, &OrderOfSubTagTableEntry);

		if(Status)
		{
			Status = UpdateNumericList(&(TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry]),
					Item1, Item2, Item3, Item4, Item5, Item6, Item7, Item8, Item9, Item10,
					Item11, Item12, Item13, Item14, Item15, Item16, Item17, Item18, Item19, Item20,
					Item21, Item22, Item23, Item24, Item25, Item26, Item27, Item28, Item29, Item30,
					Item31, Item32, Item33, Item34, Item35, Item36, Item37, Item38, Item39, Item40,
					Item41, Item42, Item43, Item44, Item45, Item46, Item47, Item48, Item49, Item50,
					Item51, Item52, Item53, Item54, Item55, Item56, Item57, Item58, Item59, Item60);
		}
	}

	return(Status);
} // ChangeNumericList (overloaded version #1)







//
//	ChangeNumericList (overloaded version #2, with an array of list items)
//		Change the list for a numeric list item
//
bool READER_CONFIGURATION_STORAGE::ChangeNumericList(const char *SettingName, int *ItemList, int NumberOfItems, int MaximumDigits)
{
	bool			Status = true;		// Assume success until we know otherwise.
	unsigned int	TagTableEntry;
	unsigned int	OrderOfTagTableEntry;


	Status = TagTable->Find(SettingName, &TagTableEntry, &OrderOfTagTableEntry);
	if(Status)
	{
		unsigned int		SubTagTableEntry;							// Entry table index.
		unsigned int		OrderOfSubTagTableEntry;					// Order of entry

		Status = TagTable->Entries[TagTableEntry].SubTagTable->Find(&SettingName[SIZE_OF_TAG], &SubTagTableEntry, &OrderOfSubTagTableEntry);

		if(Status)
		{
			Status = UpdateNumericList(&(TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry]),
					ItemList, NumberOfItems, MaximumDigits);
		}
	}

	return(Status);
} // ChangeNumericList (overloaded version #2)








//
//	ChangeStringList (overloaded version #1, with individual list items)
//		Change the list for a string list item
//
bool READER_CONFIGURATION_STORAGE::ChangeStringList(const char *SettingName,
			const char *Item1, const char *Item2, const char *Item3, const char *Item4, const char *Item5,
			const char *Item6, const char *Item7, const char *Item8, const char *Item9, const char *Item10,
			const char *Item11, const char *Item12, const char *Item13, const char *Item14, const char *Item15,
			const char *Item16, const char *Item17, const char *Item18, const char *Item19, const char *Item20,
			const char *Item21, const char *Item22, const char *Item23, const char *Item24, const char *Item25,
			const char *Item26, const char *Item27, const char *Item28, const char *Item29, const char *Item30,
			const char *Item31, const char *Item32, const char *Item33, const char *Item34, const char *Item35,
			const char *Item36, const char *Item37, const char *Item38, const char *Item39, const char *Item40,
			const char *Item41, const char *Item42, const char *Item43, const char *Item44, const char *Item45,
			const char *Item46, const char *Item47, const char *Item48, const char *Item49, const char *Item50,
			const char *Item51, const char *Item52, const char *Item53, const char *Item54, const char *Item55,
			const char *Item56, const char *Item57, const char *Item58, const char *Item59, const char *Item60)
{
	bool			Status = true;		// Assume success until we know otherwise.
	unsigned int	TagTableEntry;
	unsigned int	OrderOfTagTableEntry;


	Status = TagTable->Find(SettingName, &TagTableEntry, &OrderOfTagTableEntry);
	if(Status)
	{
		unsigned int		SubTagTableEntry;							// Entry table index.
		unsigned int		OrderOfSubTagTableEntry;					// Order of entry

		Status = TagTable->Entries[TagTableEntry].SubTagTable->Find(&SettingName[SIZE_OF_TAG], &SubTagTableEntry, &OrderOfSubTagTableEntry);

		if(Status)
		{
			Status = UpdateStringList(&(TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry]),
					Item1, Item2, Item3, Item4, Item5, Item6, Item7, Item8, Item9, Item10,
					Item11, Item12, Item13, Item14, Item15, Item16, Item17, Item18, Item19, Item20,
					Item21, Item22, Item23, Item24, Item25, Item26, Item27, Item28, Item29, Item30,
					Item31, Item32, Item33, Item34, Item35, Item36, Item37, Item38, Item39, Item40,
					Item41, Item42, Item43, Item44, Item45, Item46, Item47, Item48, Item49, Item50,
					Item51, Item52, Item53, Item54, Item55, Item56, Item57, Item58, Item59, Item60);
		}
	}

	return(Status);
} // ChangeStringList (overloaded version #1, with individual list items)





//
//	ChangeStringList (overloaded version #2, with an array of list items)
//		Change the list for a string list item
//
bool READER_CONFIGURATION_STORAGE::ChangeStringList(const char *SettingName, const char **ItemList, int NumberOfItems )
{
	bool			Status = true;		// Assume success until we know otherwise.
	unsigned int	TagTableEntry;
	unsigned int	OrderOfTagTableEntry;


	Status = TagTable->Find(SettingName, &TagTableEntry, &OrderOfTagTableEntry);
	if(Status)
	{
		unsigned int		SubTagTableEntry;							// Entry table index.
		unsigned int		OrderOfSubTagTableEntry;					// Order of entry

		Status = TagTable->Entries[TagTableEntry].SubTagTable->Find(&SettingName[SIZE_OF_TAG], &SubTagTableEntry, &OrderOfSubTagTableEntry);

		if(Status)
		{
			Status = UpdateStringList( &(TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry]),
					ItemList, NumberOfItems );
		}
	}

	return(Status);
} // ChangeStringList (overloaded version #2)








//
//	ChangeTypeFlags
//		Change the type flags for the setting(s) specified by SettingName.
//
bool READER_CONFIGURATION_STORAGE::ChangeTypeFlags(const char *SettingName, unsigned int TypeFlagsAffected, bool SetTypeFlags)
{
	bool		FoundASetting = true;			// Assume success until we know otherwise.
	bool		WildcardSubTag = false;
	int			StartingTagToProcess;
	int			NumberOfTagsToProcess;
	int			StartingSubTagToProcess;
	int			NumberOfSubTagsToProcess=0;




// Setup the search range dependent upon the number of characters in the setting name.
	switch(strlen(SettingName))
	{
		case 0:		// No characters means that we want to modify type flags for every setting.
			WildcardSubTag = true;
			StartingTagToProcess = 0;
			NumberOfTagsToProcess = TagTable->NumberOfEntries;
			break;

		case SIZE_OF_TAG:		// Wildcard sub-tag; check to be sure that tag is valid.
			{
			unsigned int		TagTableEntryOrder;			// Sorted entry index in the tag table corresponding to the command.
			WildcardSubTag = true;
			if(TagTable->Find(SettingName, (unsigned int *) &StartingTagToProcess, &TagTableEntryOrder))
			{
				NumberOfTagsToProcess = 1;
			}
			else
			{
				FoundASetting = false;
			}
			}
			break;

		case SIZE_OF_TAG + SIZE_OF_SUB_TAG:		// Full setting name has been specified; check to be sure that it is valid.
			{
			unsigned int		TagTableEntryOrder;			// Sorted entry index in the tag table corresponding to the command.
			unsigned int		SubTagTableEntryOrder;			// Sorted entry index in the tag table corresponding to the command.
			WildcardSubTag = false;
			if(TagTable->Find(SettingName, (unsigned int *) &StartingTagToProcess, &TagTableEntryOrder))
			{
				NumberOfTagsToProcess = 1;

				if(TagTable->Entries[StartingTagToProcess].SubTagTable->Find
									(&SettingName[SIZE_OF_SUB_TAG], (unsigned int *) &StartingSubTagToProcess, &SubTagTableEntryOrder))
				{
					NumberOfSubTagsToProcess = 1;
				}
				else
				{
					FoundASetting = false;
				}
			}
			else
			{
				FoundASetting = false;
			}
			}
			break;

		default:		// Invalid number of characters; WOOF.
			FoundASetting = false;
			break;
	}



// Process the settings if no errors have occurred.
	if(FoundASetting)
	{
		int		Tag;
		int		SubTag;
		for(Tag = StartingTagToProcess;    Tag < (StartingTagToProcess + NumberOfTagsToProcess);    ++Tag)
		{
		// If this is a wildcard, then update all sub-tags, else only update the sub-tag specified in the setting name sent
		// by the caller (those indices have already been setup).
			if(WildcardSubTag)
			{
				StartingSubTagToProcess = 0;
				NumberOfSubTagsToProcess = TagTable->Entries[Tag].SubTagTable->NumberOfEntries;
			}

			for(SubTag = StartingSubTagToProcess;    SubTag < (StartingSubTagToProcess + NumberOfSubTagsToProcess);    ++SubTag)
			{
				if(SetTypeFlags)
				{
					TagTable->Entries[Tag].SubTagTable->Entries[SubTag].TypeFlags |= TypeFlagsAffected;
				}
				else
				{
					TagTable->Entries[Tag].SubTagTable->Entries[SubTag].TypeFlags &= ~TypeFlagsAffected;
				}
			}
		}
	}


	return(FoundASetting);

} // ChangeTypeFlags





//
//	ValidateMenuCommandCharacters
//		Determine if there are any invalid characters in a menu command.  The command must be printable text terminated with
//		a valid table character, optionally followed by nulls.
//
bool READER_CONFIGURATION_STORAGE::ValidateMenuCommandCharacters(const char *Command, int CommandLength, const char Table)
{
	bool	Status = true;		// Assume success until we know otherwise.
	int		i;


// Find the first non-null character at the end of the buffer (where the text command ends).
	i = CommandLength - 1;
	while((i >= 0) && (Command[i] == 0))
	{
		--i;
	}

// We'd better find a valid table character.  If Table is NULL, then any table character is valid; otherwise, only the table
// character sent by the caller is valid.
	if((i >= 0) &&
		((( Table == 0 ) &&
				(IsTerminatorValid(Command[i]))) ||
		( Table == Command[i] )))
	{
	// Ensure that the characters between the beginning and the table character are valid text.
		while((i >= 0) && (Status))
		{
			if((Command[i] < 32) || (Command[i] > 126))
			{
				Status = false;
			}
			--i;
		}
	}
	else
	{
	// We didn't find a valid table character; WOOF!
		Status = false;
	}


	return(Status);
}



bool READER_CONFIGURATION_STORAGE::IsTerminatorValid(char terminator)
{
	if ( (terminator == '.') || (terminator == '!') || (terminator == '^') || (terminator == '@') || (terminator == '&') ) return true;
	return false;
}



//
//	ValidatePartialCommand
//		Determine if a partially received command is valid so far.  The complete tag and sub-tag must be included in Command.
//		This will allow barcode commands to be validated on the fly.  If the command is OK, we'll also tell the caller if
//		any more data would make it too big.
//
bool READER_CONFIGURATION_STORAGE::ValidatePartialCommand(const char *Command, bool *MaximumCommandLengthLimitReached)
{
	bool			Status = true;		// Assume success until we know otherwise.
	unsigned int	TagTableEntry;
	unsigned int	OrderOfTagTableEntry;


	*MaximumCommandLengthLimitReached = false;


// If command is special, such as a query (first character is non-alphanumeric), then no more text should be needed and
// we'll return true.
	if((strlen(Command) > 0) &&
		(((Command[0] < 'A') || (Command[0] > 'Z')) &&
		 ((Command[0] < 'a') || (Command[0] > 'z')) &&
		 ((Command[0] < '0') || (Command[0] > '9'))))
	{
		*MaximumCommandLengthLimitReached = true;
		return(true);
	}


// Quit if the command isn't long enough to contain a tag, sub-tag, and table.
	if(strlen(Command) < (SIZE_OF_TAG + SIZE_OF_SUB_TAG))
	{
		return(false);
	}


// Find the setting.
	Status = TagTable->Find(Command, &TagTableEntry, &OrderOfTagTableEntry);
	if(Status)
	{
		unsigned int		SubTagTableEntry;							// Entry table index.
		unsigned int		OrderOfSubTagTableEntry;					// Order of entry


		Status = TagTable->Entries[TagTableEntry].SubTagTable->Find(&Command[SIZE_OF_TAG], &SubTagTableEntry, &OrderOfSubTagTableEntry);

		if(Status)
		{
		// If we found the setting, validate it.  A partial setting is OK and we don't want to make the change if the setting
		// is valid.
			int		ParameterLength = strlen(Command) - (SIZE_OF_TAG + SIZE_OF_SUB_TAG);

			int ValidateStatus = ValidateAndChangeSetting
						(&(TagTable->Entries[ TagTableEntry ].SubTagTable->Entries[SubTagTableEntry]),
							&Command[ SIZE_OF_TAG + SIZE_OF_SUB_TAG ], ParameterLength, '.', false, false);
			if((ValidateStatus == RDR_CFG_PARTIAL_SETTING_VALID) || (ValidateStatus > 0))
			{
				Status = true;
			}
			else
			{
				Status = false;
			}
			if(ValidateStatus == RDR_CFG_SETTING_VALID_MAX_LENGTH)
			{
				*MaximumCommandLengthLimitReached = true;
			}
		}
	}

	return(Status);
} // ValidatePartialCommand







//
//	GetSubTagTableEntry
//		Return a pointer to the sub-tag table entry corresponding to SettingName.
//
//	Input Parameters:
//		SettingName - name of setting that we're supposed to get a pointer to.
//
//	Output Parameters:
//		SettingEntry - pointer to the sub-tag table entry corresponding to SettingName.
//
//	Returns true if we found the setting, false otherwise.
//
bool READER_CONFIGURATION_STORAGE::GetSubTagTableEntry(const char *SettingName, SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY **SettingEntry)
{
	bool			Status = true;		// Assume success until we know otherwise.
	unsigned int	TagTableEntry;
	unsigned int	OrderOfTagTableEntry;


	Status = TagTable->Find(SettingName, &TagTableEntry, &OrderOfTagTableEntry);
	if(Status)
	{
		unsigned int		SubTagTableEntry;							// Entry table index.
		unsigned int		OrderOfSubTagTableEntry;					// Order of entry

		Status = TagTable->Entries[TagTableEntry].SubTagTable->Find(&SettingName[SIZE_OF_TAG], &SubTagTableEntry, &OrderOfSubTagTableEntry);

		if(Status)
		{
			*SettingEntry = &(TagTable->Entries[TagTableEntry].SubTagTable->Entries[SubTagTableEntry]);
		}
	}

	return(Status);
} // GetSubTagTableEntry







//
//	AddValueToCRC
//		Add a value, sent by the caller, to an existing CRC sent by the caller.
//
int READER_CONFIGURATION_STORAGE::AddValueToCRC( int CRC, int Value )
{
	int		i;


	CRC = CRC ^ Value << 8;
	for(i = 0; i < 8; ++i)
		if(CRC & 0x8000)
			CRC = CRC << 1 ^ 0x1021;
		else
			CRC = CRC << 1;

	return CRC;
} // AddValueToCRC




//
//	CalculateCRC
//		Do a CRC of all the settings.
//
int READER_CONFIGURATION_STORAGE::CalculateCRC( void )
{
	unsigned int							TagTableEntry;								// Entry table index.
	unsigned int							SubTagTableEntry;							// Entry table index.
	int										CRC = 0;
	unsigned int							i;
	SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY		*NextEntry;



// Process all entries in the tag table.
	for(TagTableEntry = 0;   TagTableEntry < TagTable->NumberOfEntries;    ++TagTableEntry)
	{
	// Process all entries in the sub-tag table.
		for( SubTagTableEntry = 0;   SubTagTableEntry < TagTable->Entries[TagTableEntry].SubTagTable->NumberOfEntries;
																				++SubTagTableEntry)
		{
			NextEntry = &TagTable->Entries[ TagTableEntry ].SubTagTable->Entries[ SubTagTableEntry ];

		// Copy the sub-tag table item default value to the working value.
			if( NextEntry->TypeFlags & SUB_TAG_TABLE::BOOLEAN)
			{
				CRC = AddValueToCRC( CRC, (( *( NextEntry->InstantiatorCopyOfActiveValue.b ) ) ? 1 : 0 ) );
				CRC = AddValueToCRC( CRC, (( NextEntry->StandardDefaultValue.b ) ? 1 : 0 ) );
				CRC = AddValueToCRC( CRC, (( NextEntry->DefaultValue.b ) ? 1 : 0 ) );
				CRC = AddValueToCRC( CRC, (( NextEntry->WorkingValue[ 0 ].b ) ? 1 : 0 ) );
			}

			else if(( NextEntry->TypeFlags & SUB_TAG_TABLE::STRING) ||
				( NextEntry->TypeFlags & SUB_TAG_TABLE::STRING_LIST))
			{
				for( i = 0;    i < strlen( NextEntry->InstantiatorCopyOfActiveValue.c );    ++i )
				{
					CRC = AddValueToCRC( CRC, (int) NextEntry->InstantiatorCopyOfActiveValue.c[ i ] );
				}
				for( i = 0;    i < strlen( NextEntry->StandardDefaultValue.c );    ++i )
				{
					CRC = AddValueToCRC( CRC, (int) NextEntry->StandardDefaultValue.c[ i ] );
				}
				for( i = 0;    i < strlen( NextEntry->DefaultValue.c );    ++i )
				{
					CRC = AddValueToCRC( CRC, (int) NextEntry->DefaultValue.c[ i ] );
				}
				for( i = 0;    i < strlen( NextEntry->WorkingValue[ 0 ].c );    ++i )
				{
					CRC = AddValueToCRC( CRC, (int) NextEntry->WorkingValue[ 0 ].c[ i ] );
				}
			}

			else if(( NextEntry->TypeFlags & SUB_TAG_TABLE::BINARY_STRING) ||
					( NextEntry->TypeFlags & SUB_TAG_TABLE::ASCII_STRING) ||
					( NextEntry->TypeFlags & SUB_TAG_TABLE::HEX_STRING) ||
					( NextEntry->TypeFlags & SUB_TAG_TABLE::UNICODE_STRING))
			{
				for( i = 0;    i < NextEntry->InstantiatorCopyOfActiveValue.BinaryString->Size;    ++i )
				{
					CRC = AddValueToCRC( CRC, (int) NextEntry->InstantiatorCopyOfActiveValue.BinaryString->Char[ i ] );
				}
				for( i = 0;    i < NextEntry->StandardDefaultValue.BinaryString->Size;    ++i )
				{
					CRC = AddValueToCRC( CRC, (int) NextEntry->StandardDefaultValue.BinaryString->Char[ i ] );
				}
				for( i = 0;    i < NextEntry->DefaultValue.BinaryString->Size;    ++i )
				{
					CRC = AddValueToCRC( CRC, (int) NextEntry->DefaultValue.BinaryString->Char[ i ] );
				}
				for( i = 0;    i < NextEntry->WorkingValue[ 0 ].BinaryString->Size;    ++i )
				{
					CRC = AddValueToCRC( CRC, (int) NextEntry->WorkingValue[ 0 ].BinaryString->Char[ i ] );
				}
			}

			else if(( NextEntry->TypeFlags & SUB_TAG_TABLE::NUMERIC) ||
						( NextEntry->TypeFlags & SUB_TAG_TABLE::NUMERIC_LIST) ||
						( NextEntry->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SELECTION))
			{
				CRC = AddValueToCRC( CRC, *( NextEntry->InstantiatorCopyOfActiveValue.i ) );
				CRC = AddValueToCRC( CRC, NextEntry->StandardDefaultValue.i );
				CRC = AddValueToCRC( CRC, NextEntry->DefaultValue.i );
				CRC = AddValueToCRC( CRC, NextEntry->WorkingValue[ 0 ].i );
			}

			else if( NextEntry->TypeFlags & SUB_TAG_TABLE::FORMAT_STRING)
			{
				HSTRING		Temp(100, true);
				NextEntry->InstantiatorCopyOfActiveValue.DataFormat->BuildReportString(&Temp, true, true);
				for( i = 0;    i < Temp.Size;    ++i )
				{
					CRC = AddValueToCRC( CRC, (int) Temp.Char[ i ] );
				}
				NextEntry->StandardDefaultValue.DataFormat->BuildReportString(&Temp, true, true);
				for( i = 0;    i < Temp.Size;    ++i )
				{
					CRC = AddValueToCRC( CRC, (int) Temp.Char[ i ] );
				}
				NextEntry->DefaultValue.DataFormat->BuildReportString(&Temp, true, true);
				for( i = 0;    i < Temp.Size;    ++i )
				{
					CRC = AddValueToCRC( CRC, (int) Temp.Char[ i ] );
				}
				NextEntry->WorkingValue[ 0 ].DataFormat->BuildReportString(&Temp, true, true);
				for( i = 0;    i < Temp.Size;    ++i )
				{
					CRC = AddValueToCRC( CRC, (int) Temp.Char[ i ] );
				}
			}

			else if( NextEntry->TypeFlags & SUB_TAG_TABLE::PREFIX_SUFFIX_STRING)
			{
				HSTRING		Temp(100, true);
				NextEntry->InstantiatorCopyOfActiveValue.PrefixSuffix->BuildReportString(&Temp, true, true);
				for( i = 0;    i < Temp.Size;    ++i )
				{
					CRC = AddValueToCRC( CRC, (int) Temp.Char[ i ] );
				}
				NextEntry->StandardDefaultValue.PrefixSuffix->BuildReportString(&Temp, true, true);
				for( i = 0;    i < Temp.Size;    ++i )
				{
					CRC = AddValueToCRC( CRC, (int) Temp.Char[ i ] );
				}
				NextEntry->DefaultValue.PrefixSuffix->BuildReportString(&Temp, true, true);
				for( i = 0;    i < Temp.Size;    ++i )
				{
					CRC = AddValueToCRC( CRC, (int) Temp.Char[ i ] );
				}
				NextEntry->WorkingValue[ 0 ].PrefixSuffix->BuildReportString(&Temp, true, true);
				for( i = 0;    i < Temp.Size;    ++i )
				{
					CRC = AddValueToCRC( CRC, (int) Temp.Char[ i ] );
				}
			}
		}
	}

	return CRC;
} // CalculateCRC





//
//	FindAndRemoveDeviceName (overload #1 with HSTRING input)
//		Static member function to see if there is a device name delimited by colons at the beginning of the string.  If so,
//		remove it and store it in DeviceName.
//
bool READER_CONFIGURATION_STORAGE::FindAndRemoveDeviceName( HSTRING *CommandString, HSTRING *DeviceName )
{
	int		PositionWhereColonWasFound;
	bool	Status = false;		// Didn't find a device name yet.


	DeviceName->Copy( "" );

	if(( CommandString->Size > 0 ) && ( CommandString->Char[ 0 ] == ':' ))
	{
		if( CommandString->FindCharacterForward( ':', 1, &PositionWhereColonWasFound ) )
		{
			DeviceName->Copy( CommandString, 1, PositionWhereColonWasFound - 1 );
		// Include entire printable range except exclamation, ampersand, period, colon, and at sign.
			if( DeviceName->RangeCheck( 0x20, 0x20, 0x22, 0x25, 0x27, 0x2D, 0x2F, 0x39, 0x3B, 0x3F, 0x41, 0x7E ) )
			{
				DeviceName->UnprintableFormatsToCharacters();
				CommandString->Remove( 0, PositionWhereColonWasFound + 1 );
				Status = true;
			}
			else
			{
				DeviceName->Copy( "" );
			}
		}
	}

	return Status;
} // FindAndRemoveDeviceName (overload #1)





//
//	FindAndRemoveDeviceName (overload #2 with character string input)
//		Static member function to see if there is a device name delimited by colons at the beginning of the string.  If so,
//		remove it and store it in DeviceName.
//
bool READER_CONFIGURATION_STORAGE::FindAndRemoveDeviceName( char *chCommandString, int *CommandStringLength,
														HSTRING *DeviceName )
{
	bool		Status = false;		// Haven't found a device name yet.

	HSTRING	CommandString( (unsigned char *) chCommandString, (unsigned int) *CommandStringLength, false, false, true );

	Status = FindAndRemoveDeviceName( &CommandString, DeviceName );
	*CommandStringLength = CommandString.Size;

	return Status;
} // FindAndRemoveDeviceName (overload #2)





//
//	RemoveCommands
//		Remove commands from the input command string whose type flag matches one or more of the bits set in RemovalMask.
//		This routine allows removal of certain commands, such as scanner only or base only, before sending the result on
//		to another device.
//
bool READER_CONFIGURATION_STORAGE::RemoveCommands( HSTRING *Command, int RemovalMask )
{
	bool		Status = false;						// Haven't succeeded yet
	HSTRING		OriginalCommand( Command, true );
	char		AffectedTable;						// Either ".", "!", "^", or "&"



// If there aren't any characters, return immediately.
	if(( OriginalCommand.Size >= 1 ) && ( GetAffectedTable( &OriginalCommand, &AffectedTable )))
	{
		Status = true;
		Command->Copy( "" );


	// Keep getting commands until we reach the end of the list.
		HSTRING			NextCommand(OriginalCommand.Size, true);// Container for the next command to be processed (max size is the original command.
		bool			ProcessAnotherCommand = true;			// We haven't run out of commands yet.
		int				PositionOfNextCommand;
		bool			NextCommandWasPassedOn;
		unsigned int	TagTableEntry;							// Entry in the tag table corresponding to the command.
		unsigned int	TagTableEntryOrder;						// Sorted entry index in the tag table corresponding to the command.

		do  {
			ProcessAnotherCommand = OriginalCommand.FindCharacterForward(';', 0, &PositionOfNextCommand);
			NextCommandWasPassedOn = false;

		// If another command follows the next one, then only copy up to the command beyond the next one; otherwise, copy the whole buffer.
			if(ProcessAnotherCommand)
			{
				OriginalCommand.CopyFromString(0, PositionOfNextCommand, &NextCommand);
				OriginalCommand.Remove(0, PositionOfNextCommand + 1);
			}
			else
			{
				NextCommand.Copy(&OriginalCommand);
			}

		// Determine if the next command should be included.

		// Is this a reporting command?  If yes, include it.
			if( NextCommand.CompareSubstring( 0, "=", 1, false ) == 0 )
			{
				NextCommandWasPassedOn = true;
			}


		// Get the tag for the next command.  If we have a tag (the next character is alphanumeric), try to lookup the tag, else
		// cycle through all the tags.
			else if(((NextCommand.Char[0] >= '0') && (NextCommand.Char[0] <= '9')) ||
					((NextCommand.Char[0] >= 'A') && (NextCommand.Char[0] <= 'Z')) ||
					((NextCommand.Char[0] >= 'a') && (NextCommand.Char[0] <= 'z')))
			{
			// We have a tag.  Try to do a lookup on it.
				if(TagTable->Find(((char *) NextCommand.Char), &TagTableEntry, &TagTableEntryOrder))
				{
				// Keep getting commands until we reach the end of the list.
					HSTRING			CommandTag( 3, true );
					CommandTag.Copy( &NextCommand, 0, 2 );
					NextCommand.Remove( 0, 3 );
					HSTRING			OriginalSubTagCommand( &NextCommand, true );
					HSTRING			NewSubTagCommand( OriginalSubTagCommand.Size, true );
					HSTRING			NextSubTagCommand(OriginalSubTagCommand.Size, true);
																			// Container for the next command to be processed
																			//	(max size is the original command.
					bool			ProcessAnotherSubTagCommand = true;		// We haven't run out of commands yet.
					int				PositionOfSubTagNextCommand;
					bool			NextSubTagCommandWasPassedOn;
					unsigned int	SubTagTableEntry;						// Entry in the sub-tag table corresponding to the command.
					unsigned int	SubTagTableEntryOrder;					// Sorted entry index in the sub-tag table corresponding to the command.

					do  {
						ProcessAnotherSubTagCommand = OriginalSubTagCommand.FindCharacterForward( ',', 0, &PositionOfSubTagNextCommand);
						NextSubTagCommandWasPassedOn = false;

					// If another sub-tag command follows the next one, then only copy up to the command beyond the next one; otherwise, copy the whole buffer.
						if( ProcessAnotherSubTagCommand )
						{
							OriginalSubTagCommand.CopyFromString(0, PositionOfSubTagNextCommand, &NextSubTagCommand);
							OriginalSubTagCommand.Remove(0, PositionOfSubTagNextCommand+ 1);
						}
						else
						{
							NextSubTagCommand.Copy(&OriginalSubTagCommand);
						}

					// Determine if the next command should be included.

					// Get the sub- tag for the next command.  If we have a sub-tag (the next character is alphanumeric), try to lookup the sub-tag,
					// else it must be a wildcard operation.
                  if( ((NextSubTagCommand.Char[0] >= '0') && (NextSubTagCommand.Char[0] <= '9')) ||
								((NextSubTagCommand.Char[0] >= 'A') && (NextSubTagCommand.Char[0] <= 'Z')) ||
                        ((NextSubTagCommand.Char[0] >= 'a') && (NextSubTagCommand.Char[0] <= 'z')) ||
                        (NextSubTagCommand.Char[0] >= '_') )
						{
						// We have a sub-tag.  Try to do a lookup on it.
							if( TagTable->Entries[TagTableEntry].SubTagTable->Find
										(((char *) NextSubTagCommand.Char), &SubTagTableEntry, &SubTagTableEntryOrder))
							{
							// If no type match with the removal mask, pass it on.
								if( ( TagTable->Entries[TagTableEntry].SubTagTable->Entries[ SubTagTableEntry ].TypeFlags &
											RemovalMask ) == 0 )
								{
									NextSubTagCommandWasPassedOn = true;
								}
							}

							else
							{
							// This sub-tag didn't match; include the command.
								NextSubTagCommandWasPassedOn = true;
							}
						}

						else
						{
						// This must be some type of query; include the command.
							NextSubTagCommandWasPassedOn = true;
						}


					// If this command was passed on, add it to the result.  Add a ',' to separate a previous result.
						if( NextSubTagCommandWasPassedOn )
						{
							if( NewSubTagCommand.Size )
							{
								NewSubTagCommand.Append( "," );
							}
							NewSubTagCommand.Append( &NextSubTagCommand );
						}
					} while( ProcessAnotherSubTagCommand );

				// If there were one or more commands, include it.
					if( NewSubTagCommand.Size )
					{
						NextCommand.Copy( &CommandTag );
						NextCommand.Append( &NewSubTagCommand );
						NextCommandWasPassedOn = true;
					}
				}

				else
				{
				// This tag didn't match; include the command.
					NextCommandWasPassedOn = true;
				}
			}

			else
			{
			// This must be some type of query; include the command.
				NextCommandWasPassedOn = true;
			}


		// If this command was passed on, add it to the result.  Add a ';' to separate a previous result.
			if( NextCommandWasPassedOn )
			{
				if( Command->Size )
				{
					Command->Append( ";" );
				}

				Command->Append( &NextCommand );
			}
		} while( ProcessAnotherCommand );

	// If anything made it through filtering, append the table character.
		if( Command->Size )
		{
			Command->AppendChar( (int) AffectedTable );
		}
	}

	return Status;
} // RemoveCommands





//
//	SetFixedAttribute
//		Sets the FIXED attribute for the caller specified setting.
//
bool READER_CONFIGURATION_STORAGE::SetFixedAttribute( const char *SettingName )
{
	SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY		*SettingEntryToChange;
	bool									Status = false;			// Haven't succeeded yet.


	Status = GetSubTagTableEntry( SettingName, &SettingEntryToChange );
	if( Status )
	{
		SettingEntryToChange->TypeFlags |= SUB_TAG_TABLE::FIXED;
	}

	return Status;
} // SetFixedAttribute





//
//	ClearFixedAttribute
//		Clears the FIXED attribute for the caller specified setting.
//
bool READER_CONFIGURATION_STORAGE::ClearFixedAttribute( const char *SettingName )
{
	SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY		*SettingEntryToChange;
	bool									Status = false;			// Haven't succeeded yet.


	Status = GetSubTagTableEntry( SettingName, &SettingEntryToChange );
	if( Status )
	{
		SettingEntryToChange->TypeFlags &= ~SUB_TAG_TABLE::FIXED;
	}

	return Status;
} // ClearFixedAttribute





//
//	SetFixedExceptActiveAttribute
//		Sets the FIXED_EXCEPT_ACTIVE attribute for the caller specified setting.
//
bool READER_CONFIGURATION_STORAGE::SetFixedExceptActiveAttribute( const char *SettingName )
{
	SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY		*SettingEntryToChange;
	bool									Status = false;			// Haven't succeeded yet.


	Status = GetSubTagTableEntry( SettingName, &SettingEntryToChange );
	if( Status )
	{
		SettingEntryToChange->TypeFlags |= SUB_TAG_TABLE::FIXED_EXCEPT_ACTIVE;
	}

	return Status;
} // SetFixedExceptActiveAttribute





//
//	ClearFixedExceptActiveAttribute
//		Clears the FIXED_EXCEPT_ACTIVE attribute for the caller specified setting.
//
bool READER_CONFIGURATION_STORAGE::ClearFixedExceptActiveAttribute( const char *SettingName )
{
	SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY		*SettingEntryToChange;
	bool									Status = false;			// Haven't succeeded yet.


	Status = GetSubTagTableEntry( SettingName, &SettingEntryToChange );
	if( Status )
	{
		SettingEntryToChange->TypeFlags &= ~SUB_TAG_TABLE::FIXED_EXCEPT_ACTIVE;
	}

	return Status;
} // ClearFixedExceptActiveAttribute





#ifdef READER_CONFIGURATION_STORAGE_DEBUG
//
//	BuildListOfSettingsWithCommonActiveValue
//		Build a list of settings that share a common active value.
//
void READER_CONFIGURATION_STORAGE::BuildListOfSettingsWithCommonActiveValue( HSTRING *ListOfSettings )
{
	HSTRING									NextCommandNameToProcess( 6, true );
	HSTRING									NextCommandNameList( 20, true );
	SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY		*NextCommandToProcess;
	SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY		*NextCommandToCompare;
	void									*NextActiveValueAddressToProcess;
	void									*NextActiveValueAddressToCompare;
	unsigned int							TagTableEntry;								// Entry table index.
	unsigned int							SubTagTableEntry;							// Entry table index.
	int										i;



// Clear the list.
	ListOfSettings->Copy( "" );

// Determine the number of entries to be processed.
	int		NumberOfEntriesToBeProcessed = 0;
	for(TagTableEntry = 0;   TagTableEntry < TagTable->NumberOfEntries;    ++TagTableEntry)
	{
		NumberOfEntriesToBeProcessed += TagTable->Entries[TagTableEntry].SubTagTable->NumberOfEntries;
	}

// Build a list of all the entries.
	TAG_TABLE::TAG_TABLE_ENTRY	**TagTableEntries = new TAG_TABLE::TAG_TABLE_ENTRY * [ NumberOfEntriesToBeProcessed ];
	SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY	**SubTagTableEntries =
						new SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY * [ NumberOfEntriesToBeProcessed ];
	i = 0;
	for(TagTableEntry = 0;   TagTableEntry < TagTable->NumberOfEntries;    ++TagTableEntry)
	{
	// Process all entries in the sub-tag table.
		for( SubTagTableEntry = 0;   SubTagTableEntry < TagTable->Entries[TagTableEntry].SubTagTable->NumberOfEntries;
																				++SubTagTableEntry)
		{
			TagTableEntries[ i ] = &TagTable->Entries[ TagTableEntry ];
			SubTagTableEntries[ i ] = &TagTable->Entries[ TagTableEntry ].SubTagTable->Entries[ SubTagTableEntry ];
			++i;
		}
	}


// Process all entries in the tag table.
	while( NumberOfEntriesToBeProcessed > 1 )
	{
		NextCommandToProcess = SubTagTableEntries[ NumberOfEntriesToBeProcessed - 1 ];
		NextCommandNameToProcess.Copy( TagTableEntries[ NumberOfEntriesToBeProcessed - 1 ]->Tag );
		NextCommandNameToProcess.Append( SubTagTableEntries[ NumberOfEntriesToBeProcessed - 1 ]->SubTag );
		NextCommandNameList.Copy( "" );
		--NumberOfEntriesToBeProcessed;
		i = NumberOfEntriesToBeProcessed - 1;
		NextActiveValueAddressToProcess = (void *) 0xFFFFFFFF;
		if( NextCommandToProcess->TypeFlags & SUB_TAG_TABLE::BOOLEAN )
		{
			NextActiveValueAddressToProcess = NextCommandToProcess->InstantiatorCopyOfActiveValue.b;
		}
		else if(( NextCommandToProcess->TypeFlags & SUB_TAG_TABLE::STRING ) ||
					( NextCommandToProcess->TypeFlags & SUB_TAG_TABLE::STRING_LIST ))
		{
			NextActiveValueAddressToProcess = NextCommandToProcess->InstantiatorCopyOfActiveValue.c;
		}
		else if(( NextCommandToProcess->TypeFlags & SUB_TAG_TABLE::BINARY_STRING ) ||
				( NextCommandToProcess->TypeFlags & SUB_TAG_TABLE::ASCII_STRING ) ||
				( NextCommandToProcess->TypeFlags & SUB_TAG_TABLE::HEX_STRING ) ||
				( NextCommandToProcess->TypeFlags & SUB_TAG_TABLE::UNICODE_STRING ))
		{
			NextActiveValueAddressToProcess = NextCommandToProcess->InstantiatorCopyOfActiveValue.BinaryString;
		}
		else if(( NextCommandToProcess->TypeFlags & SUB_TAG_TABLE::NUMERIC ) ||
							( NextCommandToProcess->TypeFlags & SUB_TAG_TABLE::NUMERIC_LIST ))
		{
			NextActiveValueAddressToProcess = NextCommandToProcess->InstantiatorCopyOfActiveValue.i;
		}
		else if( NextCommandToProcess->TypeFlags & SUB_TAG_TABLE::FORMAT_STRING )
		{
			NextActiveValueAddressToProcess = NextCommandToProcess->InstantiatorCopyOfActiveValue.DataFormat;
		}
		else if( NextCommandToProcess->TypeFlags & SUB_TAG_TABLE::PREFIX_SUFFIX_STRING )
		{
			NextActiveValueAddressToProcess = NextCommandToProcess->InstantiatorCopyOfActiveValue.PrefixSuffix;
		}

		while( i >= 0 )
		{
		// Compare the next entry.
			NextCommandToCompare = SubTagTableEntries[ i ];

			NextActiveValueAddressToCompare = (void *) 0xFFFFFFFE;
			if( NextCommandToCompare->TypeFlags & SUB_TAG_TABLE::BOOLEAN )
			{
				NextActiveValueAddressToCompare = NextCommandToCompare->InstantiatorCopyOfActiveValue.b;
			}
			else if(( NextCommandToCompare->TypeFlags & SUB_TAG_TABLE::STRING ) ||
					( NextCommandToCompare->TypeFlags & SUB_TAG_TABLE::STRING_LIST ))
			{
				NextActiveValueAddressToCompare = NextCommandToCompare->InstantiatorCopyOfActiveValue.c;
			}
			else if(( NextCommandToCompare->TypeFlags & SUB_TAG_TABLE::BINARY_STRING ) ||
				( NextCommandToCompare->TypeFlags & SUB_TAG_TABLE::ASCII_STRING ) ||
				( NextCommandToCompare->TypeFlags & SUB_TAG_TABLE::HEX_STRING ) ||
				( NextCommandToCompare->TypeFlags & SUB_TAG_TABLE::UNICODE_STRING ))
			{
				NextActiveValueAddressToCompare = NextCommandToCompare->InstantiatorCopyOfActiveValue.BinaryString;
			}
			else if(( NextCommandToCompare->TypeFlags & SUB_TAG_TABLE::NUMERIC ) ||
							( NextCommandToCompare->TypeFlags & SUB_TAG_TABLE::NUMERIC_LIST ))
			{
				NextActiveValueAddressToCompare = NextCommandToCompare->InstantiatorCopyOfActiveValue.i;
			}
			else if( NextCommandToCompare->TypeFlags & SUB_TAG_TABLE::FORMAT_STRING )
			{
				NextActiveValueAddressToCompare = NextCommandToCompare->InstantiatorCopyOfActiveValue.DataFormat;
			}
			else if( NextCommandToCompare->TypeFlags & SUB_TAG_TABLE::PREFIX_SUFFIX_STRING )
			{
				NextActiveValueAddressToCompare = NextCommandToCompare->InstantiatorCopyOfActiveValue.PrefixSuffix;
			}

			if( NextActiveValueAddressToProcess == NextActiveValueAddressToCompare )
			{
			// If the active value addresses are equal, log the value and remove this value from the list.
				if( NextCommandNameList.Size )
				{
				// If there is already at least one entry in the input list, append a dash before adding this new entry.
					NextCommandNameList.Append( "-" );
				}
				else
				{
				// This is the first entry, so we need to add the command name we're processing
					NextCommandNameList.Copy( &NextCommandNameToProcess );
					NextCommandNameList.Append( "-" );
				}

				NextCommandNameList.Append( TagTableEntries[ i ]->Tag );
				NextCommandNameList.Append( SubTagTableEntries[ i ]->SubTag );

				if( i != ( NumberOfEntriesToBeProcessed - 1 ))
				{
				// This isn't the last item in the list, so we'll swap it with the last item.
					TagTableEntries[ i ] = TagTableEntries[ NumberOfEntriesToBeProcessed - 1 ];
					SubTagTableEntries[ i ] = SubTagTableEntries[ NumberOfEntriesToBeProcessed - 1 ];
				}
				--NumberOfEntriesToBeProcessed;
			}

			--i;
		}

		if( NextCommandNameList.Size )
		{
		// If we found any matches, copy to the input list.
			if( ListOfSettings->Size )
			{
			// If there is already at least one entry in the input list, append a semi-colon before adding this new entry.
				ListOfSettings->Append( ";" );
			}

			ListOfSettings->Append( &NextCommandNameList );
		}
	}

// Free space.
	delete TagTableEntries;
	delete SubTagTableEntries;

	return;
} // BuildListOfSettingsWithCommonActiveValue
#endif






//
//	SetDefault (version #1)
//		Set the default for a boolean item.
//
bool READER_CONFIGURATION_STORAGE::SetDefault( SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SettingToUpdate, bool DefaultValue,
												bool /* OverrideRangeCheck */ )
{
	bool		Succeeded = true;			// Assume this operation works until we know otherwise.


	if( SettingToUpdate->TypeFlags & SUB_TAG_TABLE::BOOLEAN )
	{
		SettingToUpdate->StandardDefaultValue.b = DefaultValue;
	}
	else
	{
		Succeeded = false;
	}

	return Succeeded;
} // SetDefault (version #1)






//
//	SetDefault (version #2)
//		Set the default for a numeric item.
//
bool READER_CONFIGURATION_STORAGE::SetDefault( SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SettingToUpdate, int DefaultValue,
												bool OverrideRangeCheck )
{
	bool		Succeeded = true;			// Assume this operation works until we know otherwise.


	if((( SettingToUpdate->TypeFlags & SUB_TAG_TABLE::NUMERIC ) ||
			( SettingToUpdate->TypeFlags & SUB_TAG_TABLE::WORKGROUP_SELECTION )) &&
		(( OverrideRangeCheck ) ||
			(( DefaultValue >= SettingToUpdate->LowRange ) && ( DefaultValue <= SettingToUpdate->HighRange )) )  )
	{
		SettingToUpdate->StandardDefaultValue.i = DefaultValue;
	}

	else if( SettingToUpdate->TypeFlags & SUB_TAG_TABLE::NUMERIC_LIST )
	{
	// See if we find the number in the list.
		int		i;
		i = 0;
		while(( i < SettingToUpdate->NumberOfEntriesInListRange ) &&
												( SettingToUpdate->NumericListRange[i] != DefaultValue ))
		{
			++i;
		}

	// If we found the number in the list, or we're going to override range check anyway, update the default.
		if(( OverrideRangeCheck ) || ( i < SettingToUpdate->NumberOfEntriesInListRange ))
		{
			SettingToUpdate->StandardDefaultValue.i = DefaultValue;
		}
	}

	else
	{
		Succeeded = false;
	}

	return Succeeded;
} // SetDefault (version #2)





//
//	ChangeDefault (version #1)
//		Update the default for a boolean item.
//
bool READER_CONFIGURATION_STORAGE::ChangeDefault( const char *SettingName, bool DefaultValue, bool OverrideRangeCheck,
												 bool OverwriteCustomWorkingAndActiveIfModified )
{
	bool								Succeeded = true;			// Assume this operation works until we know otherwise.
	SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY	*SettingEntry;


	TRACE2( "Attempting to update default, all working, and all active values to new standard default %s for setting %s\r\n",
					DefaultValue ? "true" : "false", SettingName );

	Succeeded = GetSubTagTableEntry( SettingName, &SettingEntry );
	if( Succeeded )
	{
		Succeeded = SetDefault( SettingEntry, DefaultValue, OverrideRangeCheck );
	}

	if( Succeeded )
	{
		Succeeded = CopyStandardDefaultToDefaultWorkingAndActive( SettingEntry, OverwriteCustomWorkingAndActiveIfModified );
	}

	return Succeeded;
} // ChangeDefault (version #1)





//
//	ChangeDefault (version #2)
//		Update the default for a numeric item.
//
bool READER_CONFIGURATION_STORAGE::ChangeDefault( const char *SettingName, int DefaultValue, bool OverrideRangeCheck,
												 bool OverwriteCustomWorkingAndActiveIfModified )
{
	bool								Succeeded = true;			// Assume this operation works until we know otherwise.
	SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY	*SettingEntry;


	TRACE2( "Attempting to update default, all working, and all active values to new standard default %d for setting %s\r\n",
					DefaultValue, SettingName );

	Succeeded = GetSubTagTableEntry( SettingName, &SettingEntry );
	if( Succeeded )
	{
		Succeeded = SetDefault( SettingEntry, DefaultValue, OverrideRangeCheck );
	}

	if( Succeeded )
	{
		Succeeded = CopyStandardDefaultToDefaultWorkingAndActive( SettingEntry, OverwriteCustomWorkingAndActiveIfModified );
	}

	return Succeeded;
} // ChangeDefault (version #2)



//
// GetIsMenuingInProcess()
// Return variable "IsMenuingInProcess", to check if we are processing menu tags and sub tags, set external by the menu item processed
//
bool READER_CONFIGURATION_STORAGE::GetIsMenuingInProcess(void)
{
	return IsMenuingInProcess;
}


//
// SetIsMenuingInProcess()
// Set then return variable "IsMenuingInProcess", to indicate that we are processing menu tags and sub tags, set external by the menu item processed
//
bool READER_CONFIGURATION_STORAGE::SetIsMenuingInProcess(void)
{
	IsMenuingInProcess = true;
	return IsMenuingInProcess;
}



