//======================================================================================
// HHPLProgramProfile.h
//======================================================================================
// $RCSfile: hhplprogramprofile.h $
// $Revision: 1.2 $
// $Date: 2011/11/11 17:01:39EST $
//======================================================================================

//======================================================================================



#ifndef __HHPL_PROG_PROF_H
#define __HHPL_PROG_PROF_H



#include	"HSTRING.h"				// HSTRING interface definition
#include	"HWSTRING.h"			// HWSTRING interface definition

#include	"HHPLImageData.h"		// HHPL code storage definitions

#include	"HHPLInstructionSet.h"		// HHPL instruction set definitions



class HHPL_PROGRAM_PROFILE
{
private:
// In non-C++ environments, we can't don't expand the strings dynamically, so we need to set
// maximums for such operations as InputString.  In the C++ environment, these parameters reflect
// the minimum size of strings, prefixes, and suffixes whereas in the non-C++ environment, they
// reflect the maximum sizes.
	unsigned int	MaximumSizeOfPrefixAndSuffix;
	unsigned int	MaximumStringSize;




public:

#define	MAXIMUM_NUMBER_OF_SUPPRESSION_CHARACTERS			15
#define	MAXIMUM_NUMBER_OF_REPLACEMENT_CHARACTERS			15



//	Constructor for HHPL_PROGRAM_PROFILE
	HHPL_PROGRAM_PROFILE(const unsigned char *InputCode, unsigned int InputCodeSize, const wchar_t *InputDecodedData,
			unsigned int InputDecodedDataSize, const wchar_t *InputDataSourceID, unsigned int InputDataSourceIDSize,
			unsigned int InputMaximumSizeOfPrefixAndSuffix, unsigned int InputMaximumStringSize,
			int InputDataControlFlags = HHPL_INSTRUCTION_SET::HHPL_INPUT_AND_SEND_TO_HOST_DEFAULT );


//	Destructor for HHPL_PROGRAM_PROFILE
	~HHPL_PROGRAM_PROFILE(void);




	HHPL_IMAGE_DATA		*ParsedHHPLCodeImage;	// Pointer to parsed code image.

	unsigned char		*Code;				// HHPL program to be executed
    unsigned int		Size;				// Size of HHPL program to be executed
	unsigned char		*DataSpace;			// Storage for run-time variables
	unsigned int		SizeOfDataSpace;	// Size of storage for run-time variables
    unsigned int		NextInstruction;	// Instruction pointer
	unsigned int		Cursor;				// Cursor position in the edit buffer data stream
	unsigned int		NestingLevel;		// Nesting level in the program; some traditional
											//	data format instructions quit on range or no data
											//	errors, yet we don't want them to if we're using
											//	them as part of an IF, WHILE, or DO WHILE, so we
											//	use this variable to track if we're inside another
											//	instruction (such as an IF, WHILE, or DO WHILE)
											//	and won't terminate data format instructions that
											//	fail if we are inside.
	unsigned int		DoWhileLoopLevel;	// Number of DO WHILE loops that we're currently in
	unsigned int		ForeverLoopLevel;	// Number of FOREVER loops that we're currently in
	unsigned int		IfThenElseLevel;	// Number of IF THEN ELSE that we're currently in
	unsigned int		WhileLoopLevel;		// Number of WHILE loops that we're currently in
	bool				GoToEncountered;	// Indicates if we're trying to do a jump to a fixed location.
	unsigned int		GoToAddress;		// Address pointed at by GoTo.
	HSTRING				*ReturnString;		// Contains a string returned by an instruction
	HWSTRING			*ReturnWideString;	// Contains a wide string returned by an instruction
	int					ReturnNumber;		// Contains a number returned by an instruction
	bool				ReturnNumberIsCorrupt;	// Intended to indicate that an instruction
												//	returned a number, but that it is corrupt
												//	for some reason; not used yet.
	bool				ReturnBool;			// Contains a boolean value returned by an instruction
	HHPL_INSTRUCTION_SET::RANGE
						ReturnRange;		// Contains a range value returned by an instruction.
	HHPL_INSTRUCTION_SET::PREFIX_SUFFIX
						ReturnPrefix;		// Contains a prefix value returned by an instruction
	HHPL_INSTRUCTION_SET::PREFIX_SUFFIX
						ReturnSuffix;		// Contains a suffix value returned by an instruction
	HHPL_INSTRUCTION_SET::PREFIX_SUFFIX
						DefaultPrefix;		// Default prefix to be applied to a SEND instruction
	HHPL_INSTRUCTION_SET::PREFIX_SUFFIX
						DefaultSuffix;		// Default suffix to be applied to a SEND instruction

	HWSTRING			*EditBuffer;		// Contains the current contents of the decoded data
											//	data edit buffer

	int					EditBufferControlFlags;	// Contains the HHPL_INPUT_AND_SEND_TO_HOST_FLAGS flag settings
												//	associated with EditBuffer

	HWSTRING			*DataSourceID;		// Contains the current contents of the data source ID
											//	buffer

	bool				ExitInstructionEncountered;	// Indicates that we encountered an EXIT
													//	instruction while running the script; used
													//	to stop execution of routines and the main
													//	program, returning control to the caller.
	bool				UnrecoverableError;			// Indicates that an unrecoverable execution
													//	error has occurred and that we should
													//	terminate the program.
	bool				StringComparisonsAreCaseSensitiveByDefault;	// Determines whether or not
																	//	string comparisons are
																	//	case sensitive by default;
																	//	unless otherwise explicitly
																	//	set by the program, all
																	//	comparisons are case sensitive.
// Characters that should be suppressed when being sent to the host
	unsigned int		NumberOfSuppressionCharacters;
	wchar_t				SuppressionCharacters[MAXIMUM_NUMBER_OF_SUPPRESSION_CHARACTERS];
// Characters that should be replaced when being sent to the host; the first character in a pair
// is the character to be replaced and the second is the replacement.
	unsigned int		NumberOfReplacementCharacters;
	wchar_t				ReplacementCharacters[MAXIMUM_NUMBER_OF_REPLACEMENT_CHARACTERS * 2];

private:
	HHPL_PROGRAM_PROFILE (const HHPL_PROGRAM_PROFILE&);            // declared private, but not defined, to prevent copying         
	HHPL_PROGRAM_PROFILE& operator=(const HHPL_PROGRAM_PROFILE&);  // declared private, but not defined, to prevent copying 

};
#endif      /*  __HHPL_PROG_PROF_H   */


