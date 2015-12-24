//======================================================================================
// HHPLProgramProfile.cpp
//======================================================================================
// $RCSfile: hhplprogramprofile.cpp $
// $Revision: 1.2 $
// $Date: 2009/07/02 17:39:11EDT $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Class Name:     HHPL_PROGRAM_PROFILE
//
//  Class Description:
//		HHPL_PROGRAM_PROFILE initializes data that keeps track of the HHPL run-time
//		profile and cleans up after the profile data is no longer needed (principally by freeing
//		memory).
//
//---------------------------------------------------------------------------


#include	"HHPLProgramProfile.h"	// HHPL program definitions







//
//	Constructor for HHPL_PROGRAM_PROFILE
//		Allocate storage for strings and set defaults.
//
HHPL_PROGRAM_PROFILE::HHPL_PROGRAM_PROFILE(const unsigned char *InputCode, unsigned int InputCodeSize,
			const wchar_t *InputDecodedData, unsigned int InputDecodedDataSize,
			const wchar_t *InputDataSourceID, unsigned int InputDataSourceIDSize,
			unsigned int InputMaximumSizeOfPrefixAndSuffix, unsigned int InputMaximumStringSize,
			int InputDataControlFlags )
{
	unsigned int		i, j;


	MaximumSizeOfPrefixAndSuffix = InputMaximumSizeOfPrefixAndSuffix;
	MaximumStringSize = InputMaximumStringSize;

// Convert the input code stream to HHPL_IMAGE_DATA format
	ParsedHHPLCodeImage = new HHPL_IMAGE_DATA( L"", InputCode, InputCodeSize );
	Size = ParsedHHPLCodeImage->Code->Size;
	Code = new unsigned char[Size + 5];

// Create our local copy of the code and set the start pointer
	ParsedHHPLCodeImage->Code->CopyFromString(Code, Size);
	NextInstruction = ParsedHHPLCodeImage->MainProgramLocation;

// Allocate data space, zero it, and initialize variables via the initialization table.
	SizeOfDataSpace = ParsedHHPLCodeImage->SizeOfDataSpace;
	DataSpace = new unsigned char[SizeOfDataSpace + 5];
	for(i = 0;    i < SizeOfDataSpace;    ++i)
	{
		DataSpace[i] = 0;
	}

	for(i = 0;    i < ParsedHHPLCodeImage->NumberOfInitializerTableEntries;    ++i)
	{
		for(j = 0;    j < ((unsigned int) ParsedHHPLCodeImage->InitializerTable[i].SizeOfInitializationValue);    ++j)
		{
			DataSpace[ParsedHHPLCodeImage->InitializerTable[i].AddressToStoreInitializationValue + j] =
					ParsedHHPLCodeImage->InitializerValueHeap->Char
						[ParsedHHPLCodeImage->InitializerTable[i].LocationInInitializerHeapOfInitializationValue + j];
		}
	}


// Load the decoded data edit buffer.
	EditBuffer = new HWSTRING(InputDecodedData, InputDecodedDataSize, true);
	EditBufferControlFlags = InputDataControlFlags;

// Load the data source ID buffer.
	DataSourceID = new HWSTRING(InputDataSourceID, InputDataSourceIDSize, true);


	Cursor = 0;
	NestingLevel = 0;
	DoWhileLoopLevel = 0;
	ForeverLoopLevel = 0;
	IfThenElseLevel = 0;
	WhileLoopLevel = 0;
	GoToEncountered = false;
	ReturnString = new HSTRING(MaximumStringSize, true);
	ReturnWideString = new HWSTRING(MaximumStringSize, true);
	ReturnNumberIsCorrupt = false;
	ReturnRange.Valid = false;
	ReturnPrefix.String = new HWSTRING(MaximumSizeOfPrefixAndSuffix, true);
	ReturnPrefix.UseIfNoData = false;
	ReturnPrefix.UseIfData = false;
	ReturnSuffix.String = new HWSTRING(MaximumSizeOfPrefixAndSuffix, true);
	ReturnSuffix.UseIfNoData = false;
	ReturnSuffix.UseIfData = false;
	DefaultPrefix.String = new HWSTRING(MaximumSizeOfPrefixAndSuffix, true);
	DefaultPrefix.UseIfNoData = false;
	DefaultPrefix.UseIfData = false;
	DefaultSuffix.String = new HWSTRING(MaximumSizeOfPrefixAndSuffix, true);
	DefaultSuffix.UseIfNoData = false;
	DefaultSuffix.UseIfData = false;
	ExitInstructionEncountered = false;
	UnrecoverableError = false;
	StringComparisonsAreCaseSensitiveByDefault = true;
	NumberOfSuppressionCharacters = 0;
	NumberOfReplacementCharacters = 0;

	return;
}  // Constructor





//
//	Destructor for HHPL_PROGRAM_PROFILE
//		Free string memory.
//
HHPL_PROGRAM_PROFILE::~HHPL_PROGRAM_PROFILE()
{
	delete [] Code;
	delete [] DataSpace;
	delete ReturnString;
	delete ReturnWideString;
	delete EditBuffer;
	delete DataSourceID;
	delete ReturnPrefix.String;
	delete ReturnSuffix.String;
	delete DefaultPrefix.String;
	delete DefaultSuffix.String;
	delete ParsedHHPLCodeImage;

    return;
}  // Destructor


