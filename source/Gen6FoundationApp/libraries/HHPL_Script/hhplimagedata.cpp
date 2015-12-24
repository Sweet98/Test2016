//======================================================================================
// HHPLImageData.cpp
//======================================================================================
// $RCSfile: hhplimagedata.cpp $
// $Revision: 1.1 $
// $Date: 2009/06/12 16:55:17EDT $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Class Name:     HHPL_IMAGE_DATA
//
//  Class Description:
//		HHPL_IMAGE_DATA is a class that manages the symbol tables, addresses, and actual
//		HPPL code.
//
//---------------------------------------------------------------------------






#include	"Compression.h"			// Compression definitions
#include	"HHPLImageData.h"		// HHPL code storage definitions
#include	"HHPLInstructionSet.h"	// HHPL basic type and instruction definitions.


#include	<ctype.h>				// data type conversion functions
#include	<string.h>				// 'C' string functions
#include	<stdio.h>				// Standard I/O







//
//	Constructor #1 for HHPL_IMAGE_DATA
//		Allocate and initialize storage.
//
HHPL_IMAGE_DATA::HHPL_IMAGE_DATA(const wchar_t *CompilerVersion, bool InputFeatureProtectionOverride )
{
	FeatureProtectionOverride = InputFeatureProtectionOverride;

	// Allocate error message
	ErrorMessage = new HWSTRING(160, true);

	// Initialize tables
	InitializeTables();


	// Initialize code storage
	Code = new HSTRING(20, true);

	SizeOfDataSpace = 0;

	// Create version strings and load default string.
	HHPLCompilerVersion = new HWSTRING(CompilerVersion, true);
	HHPLCompilerVersionInLoadedImage = new HWSTRING(20, true);

	return;
}  // Constructor #1






//
//	Constructor #2 for HHPL_IMAGE_DATA
//		Load tagged HHPL code sent by caller and store locally.
//
HHPL_IMAGE_DATA::HHPL_IMAGE_DATA(const wchar_t *CompilerVersion, HSTRING *ExistingHHPLCode, bool InputFeatureProtectionOverride )
{
	FeatureProtectionOverride = InputFeatureProtectionOverride;

	// Allocate error message
	ErrorMessage = new HWSTRING(160, true);

	// Initialize tables
	InitializeTables();


	// Initialize code storage
	Code = new HSTRING(20, true);

	SizeOfDataSpace = 0;

	// Create version strings and load default string.
	HHPLCompilerVersion = new HWSTRING(CompilerVersion, true);
	HHPLCompilerVersionInLoadedImage = new HWSTRING(20, true);

	// Load the tagged code
	LoadImage(ExistingHHPLCode);


	return;
}  // Constructor #2






//
//	Constructor #3 for HHPL_IMAGE_DATA
//		Load tagged HHPL code sent by caller and store locally.
//
HHPL_IMAGE_DATA::HHPL_IMAGE_DATA(const wchar_t *CompilerVersion, const unsigned char *ExistingHHPLCode,
                                 unsigned int SizeOfExistingHHPLCode, bool InputFeatureProtectionOverride )
{
	FeatureProtectionOverride = InputFeatureProtectionOverride;

	// Allocate error message
	ErrorMessage = new HWSTRING(160, true);

	// Initialize tables
	InitializeTables();


	// Initialize code storage
	Code = new HSTRING(20, true);

	SizeOfDataSpace = 0;

	// Create version strings and load default string.
	HHPLCompilerVersion = new HWSTRING(CompilerVersion, true);
	HHPLCompilerVersionInLoadedImage = new HWSTRING(20, true);

	// Load the tagged code
	HSTRING		Temp(ExistingHHPLCode, SizeOfExistingHHPLCode);
	LoadImage(&Temp);


	return;
}  // Constructor #3





//
//	Destructor for HHPL_IMAGE_DATA
//		Free memory.
//
HHPL_IMAGE_DATA::~HHPL_IMAGE_DATA()
{
	delete ErrorMessage;

	delete Code;

	delete HHPLCompilerVersion;
	delete HHPLCompilerVersionInLoadedImage;

	RemoveTables();

	return;
}  // Destructor






//
//	InitializeTables
//		Allocate and initialize symbol, address, and variable initializer tables.
//
void HHPL_IMAGE_DATA::InitializeTables(void)
{
	// Initialize symbol table
	SymbolTable = new HHPL_ITEM_TABLE();

	// Initialize address resolution table
	AddressResolutionTable = new HHPL_ITEM_TABLE();


	// Initializer variable initializer table
	SizeOfInitializerTable = NUMBER_OF_FREE_ELEMENTS_ON_TABLE_REALLOCATION;
	NumberOfInitializerTableEntries = 0;
	InitializerTable = new INITIALIZER_TABLE[SizeOfInitializerTable];
	InitializerValueHeap = new HSTRING(200, true);


	return;
}  // InitializeTables





//
//	RemoveTables
//		Free symbol, address, and variable initializer table memory.
//
void HHPL_IMAGE_DATA::RemoveTables(void)
{
	delete SymbolTable;

	delete AddressResolutionTable;


	delete InitializerTable;
	delete InitializerValueHeap;


	return;
}  // RemoveTables





//
//	AddVariableInitializer
//		Add a variable initializer entry to the initializer table
//
void HHPL_IMAGE_DATA::AddVariableInitializer(void *InitializerData, unsigned int SizeOfInitializerData,
        unsigned int DataSpaceLocationWhereInitializerWillGo)
{
	if(NumberOfInitializerTableEntries >= SizeOfInitializerTable)
	{
		SizeOfInitializerTable += NUMBER_OF_FREE_ELEMENTS_ON_TABLE_REALLOCATION;
		INITIALIZER_TABLE *NewInitializerTable = new INITIALIZER_TABLE[SizeOfInitializerTable];
		unsigned int		i;
		for(i = 0;    i < NumberOfInitializerTableEntries;    ++i)
		{
			NewInitializerTable[i] = InitializerTable[i];
		}
		delete InitializerTable;
		InitializerTable = NewInitializerTable;
	}

	InitializerTable[NumberOfInitializerTableEntries].AddressToStoreInitializationValue =
	    DataSpaceLocationWhereInitializerWillGo;
	InitializerTable[NumberOfInitializerTableEntries].SizeOfInitializationValue =
	    SizeOfInitializerData;
	InitializerTable[NumberOfInitializerTableEntries].LocationInInitializerHeapOfInitializationValue =
	    InitializerValueHeap->Size;
	InitializerValueHeap->Append((char *) InitializerData, SizeOfInitializerData);
	++NumberOfInitializerTableEntries;

	return;
} // AddVariableInitializer




//
//	AddRoutineLocation
//		Add a new routine location to the symbol table, reallocating for more space if necessary.
//
void HHPL_IMAGE_DATA::AddRoutineLocation(HWSTRING *Name, unsigned int Location, HWSTRING *ContextName)
{
	SymbolTable->Add(Name, HHPL_ITEM_TABLE::ROUTINE_ITEM, Location, 0, 0, 0, ContextName);

	return;
}




//
//	AddHostRoutineLocation
//		Add a new routine location to the symbol table, marking it as host accessible and reallocating
//		for more space if necessary.
//
void HHPL_IMAGE_DATA::AddHostRoutineLocation(HWSTRING *Name, unsigned int Location, HWSTRING *ContextName)
{
	SymbolTable->Add(Name, HHPL_ITEM_TABLE::ROUTINE_ITEM | HHPL_ITEM_TABLE::HOST_ACCESSIBLE, Location, 0, 0, 0, ContextName);

	return;
}




//
//	AddStatementLabelLocation
//		Add a new label location to the symbol table, reallocating for more space if necessary.
//
void HHPL_IMAGE_DATA::AddStatementLabelLocation(HWSTRING *Name, unsigned int Location, HWSTRING *ContextName)
{
	SymbolTable->Add(Name, HHPL_ITEM_TABLE::LABEL_ITEM, Location, 0, 0, 0, ContextName);

	return;
}






//
//	AddBooleanLocation
//		Add a new boolean variable location to the symbol table, reallocating for more space if
//		necessary.
//
void HHPL_IMAGE_DATA::AddBooleanLocation(HWSTRING *Name, unsigned int Location, unsigned int SizeOfNewData,
        int TotalNumberOfArrayElements, int NumberOfDimensions, int *SizeOfDimensions,
        HWSTRING *ContextName)
{
	// For the time being, we'll force all variables to be global context.
	HWSTRING		DummyContextName(ContextName);		// Keep MW compiler happy by making it think ContextName is being used.
	HWSTRING		GlobalContextName(GLOBAL_CONTEXT_IDENTIFIER);
	SymbolTable->Add(Name, HHPL_ITEM_TABLE::BOOLEAN_ITEM, Location, SizeOfNewData, NumberOfDimensions,
	                 SizeOfDimensions, &GlobalContextName /* ContextName */);
	SizeOfDataSpace += (SizeOfNewData * ((unsigned int) TotalNumberOfArrayElements));

	return;
}




//
//	AddIntegerLocation
//		Add a new integer variable location to the symbol table, reallocating for more space if
//		necessary.
//
void HHPL_IMAGE_DATA::AddIntegerLocation(HWSTRING *Name, unsigned int Location, unsigned int SizeOfNewData,
        int TotalNumberOfArrayElements, int NumberOfDimensions, int *SizeOfDimensions,
        HWSTRING *ContextName)
{
	// For the time being, we'll force all variables to be global context.
	HWSTRING		DummyContextName(ContextName);		// Keep MW compiler happy by making it think ContextName is being used.
	HWSTRING		GlobalContextName(GLOBAL_CONTEXT_IDENTIFIER);
	SymbolTable->Add(Name, HHPL_ITEM_TABLE::INTEGER_ITEM, Location, SizeOfNewData, NumberOfDimensions,
	                 SizeOfDimensions, &GlobalContextName /* ContextName */);
	SizeOfDataSpace += (SizeOfNewData * ((unsigned int) TotalNumberOfArrayElements));

	return;
}




//
//	AddStringLocation
//		Add a new string variable location to the symbol table, reallocating for more space if
//		necessary.  Note that the caller specifies the size of the string, to which we add 4
//		bytes:  2 bytes for storing the maximum size and 2 bytes for keeping track of the number
//		of bytes in use at any given time.
//
void HHPL_IMAGE_DATA::AddStringLocation(HWSTRING *Name, unsigned int Location, unsigned int SizeOfNewData,
                                        int TotalNumberOfArrayElements, int NumberOfDimensions, int *SizeOfDimensions,
                                        HWSTRING *ContextName)
{
	int				i;
	unsigned char	MaximumSize[2];
	unsigned int	CumulativeDataSpaceRequirements = 0;


	for(i = 0;    i < TotalNumberOfArrayElements;    ++i)
	{
		// We have to store the maximum size in the initializer table.
		MaximumSize[0] = ((unsigned char) (SizeOfNewData & 0xFF));
		MaximumSize[1] = ((unsigned char) ((SizeOfNewData / 256) & 0xFF));
		AddVariableInitializer(MaximumSize, 2, Location + CumulativeDataSpaceRequirements);
		CumulativeDataSpaceRequirements += ( SizeOfNewData * 2 ) + 4;
	}

	// Allocate data space for the string variable, adding 2 bytes for the maximum size and 2 bytes
	// for the number of bytes used.
	// For the time being, we'll force all variables to be global context.
	HWSTRING		DummyContextName(ContextName);		// Keep MW compiler happy by making it think ContextName is being used.
	HWSTRING		GlobalContextName(GLOBAL_CONTEXT_IDENTIFIER);
	SymbolTable->Add(Name, HHPL_ITEM_TABLE::STRING_ITEM, Location, (((int) SizeOfNewData) * 2) + 4, NumberOfDimensions,
	                 SizeOfDimensions, &GlobalContextName /* ContextName */);
	SizeOfDataSpace += CumulativeDataSpaceRequirements;

	return;
}




//
//	AddByteStringLocation
//		Add a new byte string variable location to the symbol table, reallocating for more space if
//		necessary.  Note that the caller specifies the size of the string, to which we add 4
//		bytes:  2 bytes for storing the maximum size and 2 bytes for keeping track of the number
//		of bytes in use at any given time.
//
void HHPL_IMAGE_DATA::AddByteStringLocation(HWSTRING *Name, unsigned int Location, unsigned int SizeOfNewData,
        int TotalNumberOfArrayElements, int NumberOfDimensions, int *SizeOfDimensions,
        HWSTRING *ContextName)
{
	int				i;
	unsigned char	MaximumSize[2];
	unsigned int	CumulativeDataSpaceRequirements = 0;


	for(i = 0;    i < TotalNumberOfArrayElements;    ++i)
	{
		// We have to store the maximum size in the initializer table.
		MaximumSize[0] = ((unsigned char) (SizeOfNewData & 0xFF));
		MaximumSize[1] = ((unsigned char) ((SizeOfNewData / 256) & 0xFF));
		AddVariableInitializer(MaximumSize, 2, Location + CumulativeDataSpaceRequirements);
		CumulativeDataSpaceRequirements += SizeOfNewData + 4;
	}

	// Allocate data space for the string variable, adding 2 bytes for the maximum size and 2 bytes
	// for the number of bytes used.
	// For the time being, we'll force all variables to be global context.
	HWSTRING		DummyContextName(ContextName);		// Keep MW compiler happy by making it think ContextName is being used.
	HWSTRING		GlobalContextName(GLOBAL_CONTEXT_IDENTIFIER);
	SymbolTable->Add(Name, HHPL_ITEM_TABLE::BYTE_STRING_ITEM, Location, ((int) SizeOfNewData) + 4, NumberOfDimensions,
	                 SizeOfDimensions, &GlobalContextName /* ContextName */);
	SizeOfDataSpace += CumulativeDataSpaceRequirements;

	return;
}




//
//	AddRoutineAddressRequestLocation
//		Add a new routine address request to the address resolution table, reallocating for more
//		space if necessary.
//
void HHPL_IMAGE_DATA::AddRoutineAddressRequestLocation(HWSTRING *Name, unsigned int Location,
        HWSTRING *ContextName)
{
	AddressResolutionTable->Add(Name, HHPL_ITEM_TABLE::ROUTINE_ITEM, Location, 0, 0, 0, ContextName);

	return;
}




//
//	AddStatementLabelAddressRequestLocation
//		Add a new label address request to the address resolution table, reallocating for more
//		space if necessary.
//
void HHPL_IMAGE_DATA::AddStatementLabelAddressRequestLocation(HWSTRING *Name, unsigned int Location,
        HWSTRING *ContextName)
{
	AddressResolutionTable->Add(Name, HHPL_ITEM_TABLE::LABEL_ITEM, Location, 0, 0, 0, ContextName);

	return;
}




//
//	AddBooleanAddressRequestLocation
//		Add a new boolean variable address request to the address resolution table, reallocating
//		for more space if necessary.
//
void HHPL_IMAGE_DATA::AddBooleanAddressRequestLocation(HWSTRING *Name, unsigned int Location,
        HWSTRING *ContextName)
{
	// For the time being, we'll force all variables to be global context.
	HWSTRING		DummyContextName(ContextName);		// Keep MW compiler happy by making it think ContextName is being used.
	HWSTRING		GlobalContextName(GLOBAL_CONTEXT_IDENTIFIER);
	AddressResolutionTable->Add(Name, HHPL_ITEM_TABLE::BOOLEAN_ITEM, Location, 0, 0, 0, &GlobalContextName /* ContextName */);

	return;
}




//
//	AddIntegerAddressRequestLocation
//		Add a new integer variable address request to the address resolution table, reallocating
//		for more space if necessary.
//
void HHPL_IMAGE_DATA::AddIntegerAddressRequestLocation(HWSTRING *Name, unsigned int Location,
        HWSTRING *ContextName)
{
	// For the time being, we'll force all variables to be global context.
	HWSTRING		DummyContextName(ContextName);		// Keep MW compiler happy by making it think ContextName is being used.
	HWSTRING		GlobalContextName(GLOBAL_CONTEXT_IDENTIFIER);
	AddressResolutionTable->Add(Name, HHPL_ITEM_TABLE::INTEGER_ITEM, Location, 0, 0, 0, &GlobalContextName /* ContextName */);

	return;
}




//
//	AddStringAddressRequestLocation
//		Add a new string variable address request to the address resolution table, reallocating
//		more space if necessary.
//
void HHPL_IMAGE_DATA::AddStringAddressRequestLocation(HWSTRING *Name, unsigned int Location,
        HWSTRING *ContextName)
{
	// For the time being, we'll force all variables to be global context.
	HWSTRING		DummyContextName(ContextName);		// Keep MW compiler happy by making it think ContextName is being used.
	HWSTRING		GlobalContextName(GLOBAL_CONTEXT_IDENTIFIER);
	AddressResolutionTable->Add(Name, HHPL_ITEM_TABLE::STRING_ITEM, Location, 0, 0, 0, &GlobalContextName /* ContextName */);

	return;
}




//
//	AddByteStringAddressRequestLocation
//		Add a new byte string variable address request to the address resolution table, reallocating
//		more space if necessary.
//
void HHPL_IMAGE_DATA::AddByteStringAddressRequestLocation(HWSTRING *Name, unsigned int Location,
        HWSTRING *ContextName)
{
	// For the time being, we'll force all variables to be global context.
	HWSTRING		DummyContextName(ContextName);		// Keep MW compiler happy by making it think ContextName is being used.
	HWSTRING		GlobalContextName(GLOBAL_CONTEXT_IDENTIFIER);
	AddressResolutionTable->Add(Name, HHPL_ITEM_TABLE::BYTE_STRING_ITEM, Location, 0, 0, 0, &GlobalContextName /* ContextName */);

	return;
}





//
//	NameIsRoutine
//		Search the symbol table to see if we find a routine with the caller specified name.
//
bool HHPL_IMAGE_DATA::NameIsRoutine(HWSTRING *Name)
{
	return((SymbolTable->SearchByNameAndType(Name, HHPL_ITEM_TABLE::ROUTINE_ITEM) >= 0) ? true : false);
}





//
//	NameIsStatementLabel
//		Search the symbol table to see if we find a label with the caller specified name.
//
bool HHPL_IMAGE_DATA::NameIsStatementLabel(HWSTRING *Name)
{
	return((SymbolTable->SearchByNameAndType(Name, HHPL_ITEM_TABLE::LABEL_ITEM) >= 0) ? true : false);
} // NameIsStatementLabel





//
//	NameIsIntegerVariable
//		Search the symbol table to see if we find an integer with the caller specified name.
//
bool HHPL_IMAGE_DATA::NameIsIntegerVariable(HWSTRING *Name)
{
	return((SymbolTable->SearchByNameAndType(Name, HHPL_ITEM_TABLE::INTEGER_ITEM) >= 0) ? true : false);
} // NameIsIntegerVariable





//
//	NameIsBooleanVariable
//		Search the symbol table to see if we find a boolean with the caller specified name.
//
bool HHPL_IMAGE_DATA::NameIsBooleanVariable(HWSTRING *Name)
{
	return((SymbolTable->SearchByNameAndType(Name, HHPL_ITEM_TABLE::BOOLEAN_ITEM) >= 0) ? true : false);
} // NameIsBooleanVariable





//
//	NameIsStringVariable
//		Search the symbol table to see if we find a string with the caller specified name.
//
bool HHPL_IMAGE_DATA::NameIsStringVariable(HWSTRING *Name)
{
	return((SymbolTable->SearchByNameAndType(Name, HHPL_ITEM_TABLE::STRING_ITEM) >= 0) ? true : false);
} // NameIsStringVariable





//
//	NameIsByteStringVariable
//		Search the symbol table to see if we find a byte string with the caller specified name.
//
bool HHPL_IMAGE_DATA::NameIsByteStringVariable(HWSTRING *Name)
{
	return((SymbolTable->SearchByNameAndType(Name, HHPL_ITEM_TABLE::BYTE_STRING_ITEM) >= 0) ? true : false);
} // NameIsByteStringVariable





//
//	SetMainProgramLocation
//		Set the main program location and update the tagged table
//
void HHPL_IMAGE_DATA::SetMainProgramLocation(unsigned int Location)
{
	MainProgramLocation = Location;

	return;
}





//
//	NoDuplicateSymbols
//		Check to make sure that each symbol name is unique.
//
bool HHPL_IMAGE_DATA::NoDuplicateSymbols(void)
{
	bool	Status = true;					// No errors yet.


	if(!SymbolTable->NoDuplicateSymbols())
	{
		ErrorMessage->Copy(SymbolTable->ErrorMessage);

		Status = false;
	}


	return(Status);
}





//
//	ResolveAddresses
//		Resolve address in the address resolution table against the symbol table.  Notify the
//		user of any errors via ErrorMessage.
//
bool HHPL_IMAGE_DATA::ResolveAddresses(void)
{
	unsigned int	NextAddressToBeResolved;		// Index
	unsigned int	NextSymbolTableEntryToCompare;	// Index
	bool			Status = true;					// No errors yet.
	wchar_t			temp[80];						// Error message string scratch pad
	bool			MatchOccurred;					// Indicates match between symbol to be resolved and an
	//	item in the symbol table.


	if(NoDuplicateSymbols())
	{
		ErrorMessage->Size = 0;

		// Try to resolve all addresses and flag those that don't resolve to the user.
		for(NextAddressToBeResolved = 0;
		        NextAddressToBeResolved < AddressResolutionTable->NumberOfTableEntries;
		        ++NextAddressToBeResolved)
		{
			// Search the symbol table for the next address to resolve.
			NextSymbolTableEntryToCompare = 0;
			MatchOccurred = false;
			while((NextSymbolTableEntryToCompare < SymbolTable->NumberOfTableEntries) &&
			        (!MatchOccurred))
			{
				// Do the names match?
				if(SymbolTable->TableEntries[NextSymbolTableEntryToCompare].Name->Compare
				        (AddressResolutionTable->TableEntries[NextAddressToBeResolved].Name, false) == 0)
				{
					// If the types match...
					if((AddressResolutionTable->TableEntries[NextAddressToBeResolved].Type & HHPL_ITEM_TABLE::ITEM_TYPE_MASK)==
					        (SymbolTable->TableEntries[NextSymbolTableEntryToCompare].Type & HHPL_ITEM_TABLE::ITEM_TYPE_MASK))
					{
						// If we're dealing with a label type, then the symbol table context must be
						// the base context for the reference.
						if(AddressResolutionTable->TableEntries[NextAddressToBeResolved].Type & HHPL_ITEM_TABLE::LABEL_ITEM)
						{
							// Do the contexts match?
							if(AddressResolutionTable->TableEntries[NextAddressToBeResolved].ContextName->CompareSubstring
							        (SymbolTable->TableEntries[NextSymbolTableEntryToCompare].ContextName,
							         false) == 0)
							{
								// The label reference falls within the defined label context.
								MatchOccurred = true;
							}
							else
							{
								++NextSymbolTableEntryToCompare;
							}
						}
						else
						{
							// For the time being, we don't match context on anything else.
							MatchOccurred = true;
						}
					}
					else
					{
						++NextSymbolTableEntryToCompare;
					}
				}
				else
				{
					++NextSymbolTableEntryToCompare;
				}
			}

			// We didn't find the symbol to be resolved in the symbol table; WOOF!
			if(!MatchOccurred)
			{
				// Make a string descriptor for the type of variable we failed to find.
				Status = false;
				ErrorMessage->Append( L"Failed to find " );

				if(AddressResolutionTable->TableEntries[NextAddressToBeResolved].Type & HHPL_ITEM_TABLE::ROUTINE_ITEM)
				{
					ErrorMessage->Append( L"routine" );
				}
				else if(AddressResolutionTable->TableEntries[NextAddressToBeResolved].Type & HHPL_ITEM_TABLE::LABEL_ITEM)
				{
					ErrorMessage->Append( L"statement label" );
				}
				else if(AddressResolutionTable->TableEntries[NextAddressToBeResolved].Type & HHPL_ITEM_TABLE::BOOLEAN_ITEM)
				{
					ErrorMessage->Append( L"boolean variable" );
				}
				else if(AddressResolutionTable->TableEntries[NextAddressToBeResolved].Type & HHPL_ITEM_TABLE::INTEGER_ITEM)
				{
					ErrorMessage->Append( L"integer variable" );
				}
				else if(AddressResolutionTable->TableEntries[NextAddressToBeResolved].Type & HHPL_ITEM_TABLE::STRING_ITEM)
				{
					ErrorMessage->Append( L"string variable" );
				}
				else if(AddressResolutionTable->TableEntries[NextAddressToBeResolved].Type & HHPL_ITEM_TABLE::BYTE_STRING_ITEM)
				{
					ErrorMessage->Append( L"byte string variable" );
				}

				swprintf( temp,sizeof(temp), L" %ls in symbol table\015\012",
				          AddressResolutionTable->TableEntries[NextAddressToBeResolved].Name->WChar );
				ErrorMessage->Append(temp);
			}

			// We found the symbol.  Is the type correct?
			else
			{
				// The types match.  Update it's location.
				Code->Char[AddressResolutionTable->TableEntries[NextAddressToBeResolved].Location] =
				    ((unsigned char) (SymbolTable->TableEntries[NextSymbolTableEntryToCompare].Location & 0xFF));
				Code->Char[AddressResolutionTable->TableEntries[NextAddressToBeResolved].Location + 1] =
				    ((unsigned char) ((SymbolTable->TableEntries[NextSymbolTableEntryToCompare].Location / 256) & 0xFF));
				Code->Char[AddressResolutionTable->TableEntries[NextAddressToBeResolved].Location + 2] =
				    ((unsigned char) ((SymbolTable->TableEntries[NextSymbolTableEntryToCompare].Location / 65536) & 0xFF));
				Code->Char[AddressResolutionTable->TableEntries[NextAddressToBeResolved].Location + 3] =
				    ((unsigned char) ((SymbolTable->TableEntries[NextSymbolTableEntryToCompare].Location / 16777216) & 0xFF));
			}
		}
	}

	else
	{
		// We found duplicate symbols; WOOF!
		Status = false;
	}


	return(Status);
}






//
//	CreateImage
//		Create tagged HHPL code to an empty string sent by the caller.  The tagged code is
//		simply a string with concatenated tagged fields.
//
//		If this routine runs out of space in the string sent by the caller, it will return false,
//		else it will return true.
//
//		Version #1 - caller specifies whether or not to strip out symbol table information and
//			whether or not to compress image
//
bool HHPL_IMAGE_DATA::CreateImage(HSTRING *OutputCodeAndTable, int CreateImageFlags)
{
	unsigned int	i, j;
	bool			ReturnStatus;		        // Returns false if we ran out of space; true otherwise
	HSTRING			Temp(20, true);				// Temporary storage for intermediate strings
	int				CrcPointer;					// Pointer to the CRC
	int				SizePointer;				// Pointer to the size
	HWSTRING		HHPLImageDataVersion( HHPL_IMAGE_DATA_VERSION, true );



	// Start out with an empty string.
	OutputCodeAndTable->Size = 0;



	// Reserve space for CRC and size
	ReturnStatus = OutputCodeAndTable->Copy(((unsigned char *) ("            ")),
	                                        2 + VERSACODE_BYTES_IN_NUMBER );


	// Tack on "HHPL" as an identifier
	ReturnStatus = OutputCodeAndTable->Append("HHPL");


	// Tack on "UNCMPRES" to indicate that image is not compressed
	ReturnStatus = OutputCodeAndTable->Append("UNCMPRES");


	// Load General Information Group
	ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) ImageVersionTag), SIZE_OF_HHPL_TAG_NUMBER);
	ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) HHPLImageDataVersion.Size * 2), SIZE_OF_HHPL_TAG_LENGTH);
	ReturnStatus = HHPLImageDataVersion.AppendToHString( OutputCodeAndTable );

	ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) MainProgramLocationTag), SIZE_OF_HHPL_TAG_NUMBER);
	ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) sizeof(MainProgramLocation)), SIZE_OF_HHPL_TAG_LENGTH);
	ReturnStatus = OutputCodeAndTable->AppendNumber(MainProgramLocation, sizeof(MainProgramLocation));

	ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) HHPLCodeTag), SIZE_OF_HHPL_TAG_NUMBER);
	ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) Code->Size), SIZE_OF_HHPL_TAG_LENGTH);
	ReturnStatus = OutputCodeAndTable->Append(Code);

	ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) SizeOfDataSpaceTag), SIZE_OF_HHPL_TAG_NUMBER);
	ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) sizeof(SizeOfDataSpace)), SIZE_OF_HHPL_TAG_LENGTH);
	ReturnStatus = OutputCodeAndTable->AppendNumber(SizeOfDataSpace, sizeof(SizeOfDataSpace));

	ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) CompilerVersionTag), SIZE_OF_HHPL_TAG_NUMBER);
	ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) HHPLCompilerVersion->Size) * 2, SIZE_OF_HHPL_TAG_LENGTH);
	ReturnStatus = HHPLCompilerVersion->AppendToHString( OutputCodeAndTable );

	ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) FeatureProtectionOverrideTag), SIZE_OF_HHPL_TAG_NUMBER);
	ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) sizeof( int ) ), SIZE_OF_HHPL_TAG_LENGTH);
	ReturnStatus = OutputCodeAndTable->AppendNumber( (int) (( FeatureProtectionOverride ) ? 1 : 0 ), sizeof( int ) );



	// We'll create a copy of the symbol table, then remove any entries that shouldn't be there according
	// to the mask.  If there is anything left, we'll tag it.
	HHPL_ITEM_TABLE		RevisedSymbolTable(SymbolTable);

	if((!(CreateImageFlags & INCLUDE_ENTIRE_SYMBOL_TABLE)) &&
	        (!(CreateImageFlags & INCLUDE_ALL)))
	{
		unsigned int	ItemTypesToRemove = HHPL_ITEM_TABLE::LABEL_ITEM |
		                                 HHPL_ITEM_TABLE::BOOLEAN_ITEM |
		                                 HHPL_ITEM_TABLE::INTEGER_ITEM |
		                                 HHPL_ITEM_TABLE::STRING_ITEM |
		                                 HHPL_ITEM_TABLE::BYTE_STRING_ITEM;

		if(CreateImageFlags & INCLUDE_ROUTINE_ENTRIES)
		{
			RevisedSymbolTable.RemoveByType(ItemTypesToRemove);
		}
		else if(CreateImageFlags & INCLUDE_HOST_ROUTINE_ENTRIES)
		{
			RevisedSymbolTable.RemoveByNonHostType(ItemTypesToRemove);
		}
		else
		{
			ItemTypesToRemove |= HHPL_ITEM_TABLE::ROUTINE_ITEM;
			RevisedSymbolTable.RemoveByType(ItemTypesToRemove);
		}
	}

	if(RevisedSymbolTable.NumberOfTableEntries)
	{
		// Load Symbol Table Group
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) NumberOfSymbolTableEntriesTag), SIZE_OF_HHPL_TAG_NUMBER);
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) sizeof(RevisedSymbolTable.NumberOfTableEntries)), SIZE_OF_HHPL_TAG_LENGTH);
		ReturnStatus = OutputCodeAndTable->AppendNumber(RevisedSymbolTable.NumberOfTableEntries, sizeof(RevisedSymbolTable.NumberOfTableEntries));

		// Pack the symbol table names (size of string and string for each name) to a temporary
		// string first.
		Temp.Size = 0;
		for(i = 0;    i < RevisedSymbolTable.NumberOfTableEntries;    ++i)
		{
			Temp.AppendNumber(RevisedSymbolTable.TableEntries[i].Name->Size * 2, SIZE_OF_HHPL_TAG_LENGTH);
			RevisedSymbolTable.TableEntries[i].Name->AppendToHString( &Temp );
		}
		// Now, load the tag, size, and packed symbol table names
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) SymbolTableNamesTag), SIZE_OF_HHPL_TAG_NUMBER);
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) Temp.Size), SIZE_OF_HHPL_TAG_LENGTH);
		ReturnStatus = OutputCodeAndTable->Append(&Temp);

		// Pack the symbol table locations (size of location and location value for each name)
		// to a temporary string first.
		Temp.Size = 0;
		Temp.AppendNumber(sizeof(RevisedSymbolTable.TableEntries[0].Location), SIZE_OF_HHPL_TAG_LENGTH);	// Append field size first.
		for(i = 0;    i < RevisedSymbolTable.NumberOfTableEntries;    ++i)
		{
			Temp.AppendNumber(RevisedSymbolTable.TableEntries[i].Location,
			                  sizeof(RevisedSymbolTable.TableEntries[i].Location));
		}
		// Now, load the tag, size, and packed symbol table locations
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) SymbolTableLocationsTag), SIZE_OF_HHPL_TAG_NUMBER);
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) Temp.Size), SIZE_OF_HHPL_TAG_LENGTH);
		ReturnStatus = OutputCodeAndTable->Append(&Temp);

		// Pack the symbol table types (size of type and type value for each name)
		// to a temporary string first.
		Temp.Size = 0;
		Temp.AppendNumber(sizeof(RevisedSymbolTable.TableEntries[0].Type), SIZE_OF_HHPL_TAG_LENGTH);	// Append field size first.
		for(i = 0;    i < RevisedSymbolTable.NumberOfTableEntries;    ++i)
		{
			Temp.AppendNumber(RevisedSymbolTable.TableEntries[i].Type,
			                  sizeof(RevisedSymbolTable.TableEntries[i].Type));
		}
		// Now, load the tag, size, and packed symbol table types
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) SymbolTableTypesTag), SIZE_OF_HHPL_TAG_NUMBER);
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) Temp.Size), SIZE_OF_HHPL_TAG_LENGTH);
		ReturnStatus = OutputCodeAndTable->Append(&Temp);


		// Pack the symbol table array element sizes (size and value for each array element size)
		// to a temporary string first.
		Temp.Size = 0;
		Temp.AppendNumber(sizeof(RevisedSymbolTable.TableEntries[0].SizeOfEachElement), SIZE_OF_HHPL_TAG_LENGTH);	// Append field size first.
		for(i = 0;    i < RevisedSymbolTable.NumberOfTableEntries;    ++i)
		{
			Temp.AppendNumber(RevisedSymbolTable.TableEntries[i].SizeOfEachElement,
			                  sizeof(RevisedSymbolTable.TableEntries[i].SizeOfEachElement));
		}
		// Now, load the tag, size, and packed symbol table array element sizes
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) SymbolTableArrayElementSizesTag), SIZE_OF_HHPL_TAG_NUMBER);
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) Temp.Size), SIZE_OF_HHPL_TAG_LENGTH);
		ReturnStatus = OutputCodeAndTable->Append(&Temp);


		// Pack the symbol table number of array dimensions (size and value for each # of dimensions)
		// to a temporary string first.
		Temp.Size = 0;
		Temp.AppendNumber(sizeof(RevisedSymbolTable.TableEntries[0].NumberOfDimensions), SIZE_OF_HHPL_TAG_LENGTH);	// Append field size first.
		for(i = 0;    i < RevisedSymbolTable.NumberOfTableEntries;    ++i)
		{
			Temp.AppendNumber(RevisedSymbolTable.TableEntries[i].NumberOfDimensions,
			                  sizeof(RevisedSymbolTable.TableEntries[i].NumberOfDimensions));
		}
		// Now, load the tag, size, and packed symbol table number of dimensions
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) SymbolTableNumberOfArrayDimensionsTag), SIZE_OF_HHPL_TAG_NUMBER);
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) Temp.Size), SIZE_OF_HHPL_TAG_LENGTH);
		ReturnStatus = OutputCodeAndTable->Append(&Temp);


		// Pack the symbol table array dimension sizes (size and value for each array dimension size)
		// to a temporary string first.
		Temp.Size = 0;
		Temp.AppendNumber(sizeof(RevisedSymbolTable.TableEntries[0].SizeOfDimensions[0]), SIZE_OF_HHPL_TAG_LENGTH);	// Append field size first.
		for(i = 0;    i < RevisedSymbolTable.NumberOfTableEntries;    ++i)
		{
			for(j = 0;    j < ((unsigned int) RevisedSymbolTable.TableEntries[i].NumberOfDimensions);    ++j)
			{
				Temp.AppendNumber(RevisedSymbolTable.TableEntries[i].SizeOfDimensions[j],
				                  sizeof(RevisedSymbolTable.TableEntries[i].SizeOfDimensions[j]));
			}
		}
		// Now, load the tag, size, and packed symbol table array dimension sizes
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) SymbolTableSizeOfArrayDimensionsTag), SIZE_OF_HHPL_TAG_NUMBER);
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) Temp.Size), SIZE_OF_HHPL_TAG_LENGTH);
		ReturnStatus = OutputCodeAndTable->Append(&Temp);


		// Pack the symbol table context names (size of string and string for each name) to a temporary
		// string first.
		Temp.Size = 0;
		for(i = 0;    i < RevisedSymbolTable.NumberOfTableEntries;    ++i)
		{
			Temp.AppendNumber(RevisedSymbolTable.TableEntries[i].ContextName->Size * 2, SIZE_OF_HHPL_TAG_LENGTH);
			RevisedSymbolTable.TableEntries[i].ContextName->AppendToHString( &Temp );
		}
		// Now, load the tag, size, and packed symbol table context names
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) SymbolTableContextNameTag), SIZE_OF_HHPL_TAG_NUMBER);
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) Temp.Size), SIZE_OF_HHPL_TAG_LENGTH);
		ReturnStatus = OutputCodeAndTable->Append(&Temp);
	}

	else
	{
		// We've been told to not include any symbol table entries, so we'd better at
		// least indicate that the table is zero length
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) NumberOfSymbolTableEntriesTag), SIZE_OF_HHPL_TAG_NUMBER);
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) sizeof(RevisedSymbolTable.NumberOfTableEntries)), SIZE_OF_HHPL_TAG_LENGTH);
		ReturnStatus = OutputCodeAndTable->AppendNumber(0, sizeof(RevisedSymbolTable.NumberOfTableEntries));
	}



	if((CreateImageFlags & INCLUDE_ENTIRE_ADDRESS_RESOLUTION_TABLE) || (CreateImageFlags & INCLUDE_ALL))
	{
		// Load Address ResolutionTable Group
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) NumberOfAddressResolutionTableEntriesTag), SIZE_OF_HHPL_TAG_NUMBER);
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) sizeof(AddressResolutionTable->NumberOfTableEntries)), SIZE_OF_HHPL_TAG_LENGTH);
		ReturnStatus = OutputCodeAndTable->AppendNumber(AddressResolutionTable->NumberOfTableEntries, sizeof(AddressResolutionTable->NumberOfTableEntries));

		// Pack the address resolution table names (size of string and string for each name) to a temporary
		// string first.
		Temp.Size = 0;
		for(i = 0;    i < AddressResolutionTable->NumberOfTableEntries;    ++i)
		{
			Temp.AppendNumber(AddressResolutionTable->TableEntries[i].Name->Size * 2, SIZE_OF_HHPL_TAG_LENGTH);
			AddressResolutionTable->TableEntries[i].Name->AppendToHString( &Temp );
		}
		// Now, load the tag, size, and packed address resolution table names
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) AddressResolutionTableNamesTag), SIZE_OF_HHPL_TAG_NUMBER);
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) Temp.Size), SIZE_OF_HHPL_TAG_LENGTH);
		ReturnStatus = OutputCodeAndTable->Append(&Temp);

		// Pack the address resolution table locations (size of location and location value for each name)
		// to a temporary string first.
		Temp.Size = 0;
		Temp.AppendNumber(sizeof(AddressResolutionTable->TableEntries[0].Location), SIZE_OF_HHPL_TAG_LENGTH);	// Append field size first.
		for(i = 0;    i < AddressResolutionTable->NumberOfTableEntries;    ++i)
		{
			Temp.AppendNumber(AddressResolutionTable->TableEntries[i].Location,
			                  sizeof(AddressResolutionTable->TableEntries[i].Location));
		}
		// Now, load the tag, size, and packed address resolution table locations
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) AddressResolutionTableLocationsTag), SIZE_OF_HHPL_TAG_NUMBER);
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) Temp.Size), SIZE_OF_HHPL_TAG_LENGTH);
		ReturnStatus = OutputCodeAndTable->Append(&Temp);

		// Pack the address resolution table types (size of type and type value for each name)
		// to a temporary string first.
		Temp.Size = 0;
		Temp.AppendNumber(sizeof(AddressResolutionTable->TableEntries[0].Type), SIZE_OF_HHPL_TAG_LENGTH);	// Append field size first.
		for(i = 0;    i < AddressResolutionTable->NumberOfTableEntries;    ++i)
		{
			Temp.AppendNumber(AddressResolutionTable->TableEntries[i].Type,
			                  sizeof(AddressResolutionTable->TableEntries[i].Type));
		}
		// Now, load the tag, size, and packed address resolution table types
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) AddressResolutionTableTypesTag), SIZE_OF_HHPL_TAG_NUMBER);
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) Temp.Size), SIZE_OF_HHPL_TAG_LENGTH);
		ReturnStatus = OutputCodeAndTable->Append(&Temp);


		// Pack the address resolution table context names (size of string and string for each name) to a temporary
		// string first.
		Temp.Size = 0;
		for(i = 0;    i < AddressResolutionTable->NumberOfTableEntries;    ++i)
		{
			Temp.AppendNumber(AddressResolutionTable->TableEntries[i].ContextName->Size * 2, SIZE_OF_HHPL_TAG_LENGTH);
			AddressResolutionTable->TableEntries[i].ContextName->AppendToHString( &Temp );
		}
		// Now, load the tag, size, and packed address resolution table context names
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) AddressResolutionTableContextNameTag), SIZE_OF_HHPL_TAG_NUMBER);
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) Temp.Size), SIZE_OF_HHPL_TAG_LENGTH);
		ReturnStatus = OutputCodeAndTable->Append(&Temp);
	}

	else
	{
		// We've been told to not include any address resolution table entries, so we'd better at
		// least indicate that the table is zero length
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) NumberOfAddressResolutionTableEntriesTag), SIZE_OF_HHPL_TAG_NUMBER);
		ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) sizeof(AddressResolutionTable->NumberOfTableEntries)), SIZE_OF_HHPL_TAG_LENGTH);
		ReturnStatus = OutputCodeAndTable->AppendNumber(0, sizeof(AddressResolutionTable->NumberOfTableEntries));
	}




	// Load Initializer Table Group
	ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) NumberOfInitializerTableEntriesTag), SIZE_OF_HHPL_TAG_NUMBER);
	ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) sizeof(NumberOfInitializerTableEntries)), SIZE_OF_HHPL_TAG_LENGTH);
	ReturnStatus = OutputCodeAndTable->AppendNumber(NumberOfInitializerTableEntries, sizeof(NumberOfInitializerTableEntries));

	// Pack the initializer table item storage addresses to a temporary string first.
	Temp.Size = 0;
	for(i = 0;    i < NumberOfInitializerTableEntries;    ++i)
	{
		Temp.AppendNumber(InitializerTable[i].AddressToStoreInitializationValue, VERSACODE_BYTES_IN_NUMBER );
	}
	// Now, load the tag, size, and packed initializer item storage addresses
	ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) InitializerItemStorageAddressTableTag), SIZE_OF_HHPL_TAG_NUMBER);
	ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) Temp.Size), SIZE_OF_HHPL_TAG_LENGTH);
	ReturnStatus = OutputCodeAndTable->Append(&Temp);

	// Pack the initializer table item sizes to a temporary string first.
	Temp.Size = 0;
	for(i = 0;    i < NumberOfInitializerTableEntries;    ++i)
	{
		Temp.AppendNumber(InitializerTable[i].SizeOfInitializationValue, VERSACODE_BYTES_IN_NUMBER );
	}
	// Now, load the tag, size, and packed initializer item sizes
	ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) InitializerItemSizeTableTag), SIZE_OF_HHPL_TAG_NUMBER);
	ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) Temp.Size), SIZE_OF_HHPL_TAG_LENGTH);
	ReturnStatus = OutputCodeAndTable->Append(&Temp);

	// Pack the initializer table item heap locations to a temporary string first.
	Temp.Size = 0;
	for(i = 0;    i < NumberOfInitializerTableEntries;    ++i)
	{
		Temp.AppendNumber(InitializerTable[i].LocationInInitializerHeapOfInitializationValue, VERSACODE_BYTES_IN_NUMBER );
	}
	// Now, load the tag, size, and packed initializer item heap locations
	ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) InitializerItemHeapLocationTableTag), SIZE_OF_HHPL_TAG_NUMBER);
	ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) Temp.Size), SIZE_OF_HHPL_TAG_LENGTH);
	ReturnStatus = OutputCodeAndTable->Append(&Temp);

	// Load the initializer heap
	ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned short) InitializerHeapTag), SIZE_OF_HHPL_TAG_NUMBER);
	ReturnStatus = OutputCodeAndTable->AppendNumber(((unsigned int) InitializerValueHeap->Size), SIZE_OF_HHPL_TAG_LENGTH);
	ReturnStatus = OutputCodeAndTable->Append(InitializerValueHeap);



	// Now save the size and CRC
	if(ReturnStatus)
	{
		CrcPointer = 0;
		SizePointer = 2;

		OutputCodeAndTable->InsertNumber(SizePointer, ((int) (OutputCodeAndTable->Size)), 4);
		OutputCodeAndTable->InsertNumber(CrcPointer,
		                                 ((unsigned short) (calc_crc(((unsigned char *) &(OutputCodeAndTable->Char[2])), OutputCodeAndTable->Size - 2))), 2);
	}


	// If we're supposed to compress, do it now.
	if((ReturnStatus) &&
	        ((CreateImageFlags & COMPRESS_IMAGE_LEMPEL_ZIV_2) || (CreateImageFlags & COMPRESS_IMAGE_HUFFMAN_1)))
	{
		// Create a string to hold the compressed result.
		HSTRING	CompressedImage(OutputCodeAndTable, true);

		// Create a HEX representation of the original image size.
		char	OriginalImageSize[10];
		sprintf(OriginalImageSize, "%08X", ((int) OutputCodeAndTable->Size));

		// Compress the image.
		if(CreateImageFlags & COMPRESS_IMAGE_LEMPEL_ZIV_2)
		{
			if(CompressData(&CompressedImage, LEMPEL_ZIV_VERSION_2_0))
			{
				// Reserve space for CRC and size
				ReturnStatus = OutputCodeAndTable->Copy(((unsigned char *) ("            ")), 2 + VERSACODE_BYTES_IN_NUMBER );
				// Tack on "HHPL" as an identifier
				ReturnStatus = OutputCodeAndTable->Append("HHPL");

				// Tack on "LEMPZIV " to indicate that image is Huffman compressed
				ReturnStatus = OutputCodeAndTable->Append("LEMPZIV ");

				// Embed the size of the uncompressed image.
				ReturnStatus = OutputCodeAndTable->Append(OriginalImageSize, 8);

				// Copy compressed image
				ReturnStatus = OutputCodeAndTable->Append(&CompressedImage);

				// Update CRC and pointer.
				if(ReturnStatus)
				{
					CrcPointer = 0;
					SizePointer = 2;

					OutputCodeAndTable->InsertNumber(SizePointer, ((int) (OutputCodeAndTable->Size)), 4);
					OutputCodeAndTable->InsertNumber(CrcPointer,
					                                 ((unsigned short) (calc_crc(((unsigned char *) &(OutputCodeAndTable->Char[2])), OutputCodeAndTable->Size - 2))), 2);
				}
			}

			else
			{
				// Compression error; WOOF!
				ReturnStatus = false;
			}
		}

		else
		{
			if(HuffmanEncode(&CompressedImage, 1))
			{
				// Reserve space for CRC and size
				ReturnStatus = OutputCodeAndTable->Copy(((unsigned char *) ("            ")), 2 + VERSACODE_BYTES_IN_NUMBER );
				// Tack on "HHPL" as an identifier
				ReturnStatus = OutputCodeAndTable->Append("HHPL");

				// Tack on "HUFFMAN " to indicate that image is Huffman compressed
				ReturnStatus = OutputCodeAndTable->Append("HUFFMAN ");

				// Embed the size of the uncompressed image.
				ReturnStatus = OutputCodeAndTable->Append(OriginalImageSize, 8);

				// Copy compressed image
				ReturnStatus = OutputCodeAndTable->Append(&CompressedImage);

				// Update CRC and pointer.
				if(ReturnStatus)
				{
					CrcPointer = 0;
					SizePointer = 2;

					OutputCodeAndTable->InsertNumber(SizePointer, ((int) (OutputCodeAndTable->Size)), 4);
					OutputCodeAndTable->InsertNumber(CrcPointer,
					                                 ((unsigned short) (calc_crc(((unsigned char *) &(OutputCodeAndTable->Char[2])), OutputCodeAndTable->Size - 2))), 2);
				}
			}

			else
			{
				// Compression error; WOOF!
				ReturnStatus = false;
			}
		}
	}


	return (ReturnStatus);
}






//
//	CreateImage
//		Create tagged HHPL code to an empty string sent by the caller.  The tagged code is
//		simply a string with concatenated tagged fields.
//
//		If this routine runs out of space in the string sent by the caller, it will return false,
//		else it will return true.
//
//		Version #2 - automatically include symbol table information, but does not compress image.
//
bool HHPL_IMAGE_DATA::CreateImage(HSTRING *OutputCodeAndTable)
{
	return(CreateImage(OutputCodeAndTable, INCLUDE_ENTIRE_SYMBOL_TABLE));
}






//
//	CreateStrippedImage
//		Create tagged HHPL code to an empty string sent by the caller, leaving out the symbol
//		table information.  The image will not be compressed.  The tagged code is simply a
//		string with concatenated tagged fields.
//
//		If this routine runs out of space in the string sent by the caller, it will return false,
//		else it will return true.
//
bool HHPL_IMAGE_DATA::CreateStrippedImage(HSTRING *OutputCodeAndTable)
{
	return(CreateImage(OutputCodeAndTable, MINIMUM_IMAGE));
}





//
//	LoadImage
//		Load the local HHPL code storage with the tagged HHPL code sent by the caller.
//
//		Returned status will reflect any exceptions encountered during the load.
//
int HHPL_IMAGE_DATA::LoadImage(HSTRING *ExistingHHPLCode)
{
	int				i;
	unsigned long	ReturnStatus;               // Status of tagged image conversion
	unsigned int	NextLocationToRead;     // Pointer to next place in tagged configuration from which to extract a tagged field.
	unsigned short	Tag;                    // Tag number of the field currently being extracted.
	unsigned int	Length;                 // Length of the field currently being extracted.
	unsigned int	Number;                 // Extracted numeric value of a field.
	int				CrcPointer;			// Pointer to the CRC
	int				SizePointer;			// Pointer to the size
	unsigned short	ImageCrc;
	int				ImageSize;

	HSTRING	SymbolTableNames(20, true);					// Temporary storage for symbol table names
	HSTRING	SymbolTableLocations(20, true);				// Temporary storage for symbol table locations
	HSTRING	SymbolTableTypes(20, true);					// Temporary storage for symbol table types
	HSTRING	SymbolTableArrayElementSizes(20, true);		// Temporary storage for symbol table array element sizes
	HSTRING	SymbolTableNumberOfArrayDimensions(20, true);// Temporary storage for symbol table number of array dimensions
	HSTRING	SymbolTableSizeOfArrayDimensions(20, true);	// Temporary storage for symbol table array dimension sizes
	HSTRING	SymbolTableContextNames(20,true);			// Temporary storage for symbol table context names.
	HSTRING	AddressResolutionTableNames(20, true);		// Temporary storage for address resolution table names
	HSTRING	AddressResolutionTableLocations(20, true);	// Temporary storage for address resolution table locations
	HSTRING	AddressResolutionTableTypes(20, true);		// Temporary storage for address resolution table types
	HSTRING	AddressResolutionTableContextNames(20,true);// Temporary storage for address resolution table context names.
	HSTRING	InitializerItemStorageAddresses(20, true);	// Temporary storage for initializer item storage addresses
	HSTRING	InitializerItemSizes(20, true);				// Temporary storage for initializer item sizes
	HSTRING	InitializerItemHeapLocations(20, true);		// Temporary storage for initializer item heap locations
	HSTRING	InitializerHeapStorage(20, true);			// Temporary storage for initializer heap
	HSTRING	StoredCode(20, true);						// Temporary storage for HHPL code.
	unsigned int	StoredMainLocation=0;							// Temporary storage for main program location
	unsigned int	StoredNumberOfSymbolTableEntries=0;			// Temporary storage for stored # of symbol table entries.
	unsigned int	StoredNumberOfAddressResolutionTableEntries=0;// Temporary storage for stored # of address resolution table entries.
	unsigned int	StoredNumberOfInitializerTableEntries=0;		// Temporary storage for stored # of initializer table entries.

	bool	MainLocationFound;								// We found the main program location, which is mandatory.
	bool	NumberOfSymbolTableEntriesFound;				// We found the number of symbol table entries
	bool	NumberOfAddressResolutionTableEntriesFound;		// We found the number of symbol table entries
	bool	NumberOfInitializerTableEntriesFound=false;			// We found the number of initializer table entries

	HSTRING		TempStr(20, true);							// Scratch space for a string
	HWSTRING	TempWStr(20, true);							// Scratch space for a widestring

	unsigned int	TempNum;									// Scratch space for a number
	unsigned int	TempType;									// Scratch space for a type
	int				TempArrayElementSize;						// Scratch space for array element size
	int				TempNumberOfDimensions;						// Scratch space for number of array dimensions
	int				TempSizeOfDimensions[MAXIMUM_NUMBER_OF_ARRAY_DIMENSIONS];
	HWSTRING		TempContextName(20, true);					// Scratch space for context names


	// So far, indicate that we're OK.
	ReturnStatus = HHPL_IMAGE_DECODED_OK;


	// Initialize the local stuff to indicate that no code is stored.
	RemoveTables();
	InitializeTables();
	SizeOfDataSpace = 0;
	FeatureProtectionOverride = false;


	// The image sent to us may be an HHPL code image or it may be raw code (such as an old-style data format program),
	// which contains no header information.  Store the input image directly to the code location and set the main
	// program location to zero so that if we don't find a header, we'll assume that we received raw code.
	Code->Copy(ExistingHHPLCode);
	MainProgramLocation = 0;


	// Get size and CRC
	CrcPointer = 0;
	SizePointer = 2;

	ExistingHHPLCode->CopyNumberFromString(CrcPointer, 2, &ImageCrc);
	ExistingHHPLCode->CopyNumberFromString(SizePointer, 4, &ImageSize);

	// Start reading after the CRC and size.
	NextLocationToRead = 2 + VERSACODE_BYTES_IN_NUMBER;


	// If the size sent in the code indicates that we should have more data than was sent to us,
	// WOOF!!!!
	if(ExistingHHPLCode->Size < ((unsigned long) ImageSize))
	{
		return HHPL_IMAGE_SIZE_MISMATCH;
	}

	// If CRC of data sent to us doesn't match computed CRC, WOOF.
	if(ImageCrc != ((unsigned short) (calc_crc(((unsigned char *) &(ExistingHHPLCode->Char[SizePointer])), ExistingHHPLCode->Size - 2))))
	{
		return HHPL_IMAGE_CRC_MISMATCH;
	}



	// Verify that the "HHPL" identifier is present
	ExistingHHPLCode->CopyFromString(NextLocationToRead, 4, &TempStr);
	NextLocationToRead += 4;
	if(TempStr.Compare("HHPL", true) != 0)
	{
		return HHPL_IMAGE_MISSING_IDENTIFIER;
	}


	// If we made it this far, we must have a valid HHPL image, meaning that the code sent to us is not raw and the
	// image needs to be parsed; don't treat the image we received as actual code.
	Code->Size = 0;


	// If the image was compressed, decompress it and call this routine with the result.
	ExistingHHPLCode->CopyFromString(NextLocationToRead, 8, &TempStr);
	if(TempStr.Compare("HUFFMAN ", true) == 0)
	{
		NextLocationToRead += 8;

		// Get the HEX representation of the original image size.
		ExistingHHPLCode->CopyFromString(NextLocationToRead, 8, &TempStr);
		TempStr.Char[8] = 0;
		NextLocationToRead += 8;
		unsigned int	OriginalImageSize;
		sscanf(((char *) TempStr.Char), "%08X", &OriginalImageSize);

		// Create a string to hold the decompressed result, then copy the compressed image.
		HSTRING	DecompressedImage(OriginalImageSize, true);
		ExistingHHPLCode->CopyFromString(NextLocationToRead,
		                                 ExistingHHPLCode->Size - NextLocationToRead, &DecompressedImage);

		// Decompress the image.
		if(HuffmanDecode(&DecompressedImage))
		{
			return(LoadImage(&DecompressedImage));
		}
		else
		{
			return HHPL_IMAGE_DECOMPRESSION_ERROR;
		}
	}



	else if(TempStr.Compare("LEMPZIV ", true) == 0)
	{
		NextLocationToRead += 8;

		// Get the HEX representation of the original image size.
		ExistingHHPLCode->CopyFromString(NextLocationToRead, 8, &TempStr);
		TempStr.Char[8] = 0;
		NextLocationToRead += 8;
		unsigned int	OriginalImageSize;
		sscanf(((char *) TempStr.Char), "%08X", &OriginalImageSize);

		// Create a string to hold the decompressed result, then copy the compressed image.
		HSTRING	DecompressedImage(OriginalImageSize, true);
		ExistingHHPLCode->CopyFromString(NextLocationToRead,
		                                 ExistingHHPLCode->Size - NextLocationToRead, &DecompressedImage);

		// Decompress the image.
		if(DecompressData(&DecompressedImage, LEMPEL_ZIV_VERSION_2_0, false, OriginalImageSize))
		{
			return(LoadImage(&DecompressedImage));
		}
		else
		{
			return HHPL_IMAGE_DECOMPRESSION_ERROR;
		}
	}



	else
	{
		if(TempStr.Compare("UNCMPRES", true) == 0)
		{
			NextLocationToRead += 8;
		}

		// Image was uncompressed; continue parsing and loading it!
		MainLocationFound = false;
		NumberOfSymbolTableEntriesFound = false;
		NumberOfAddressResolutionTableEntriesFound = false;


		// Decode tags until we've reached the end of the header
		while(NextLocationToRead < ExistingHHPLCode->Size)
		{
			ExistingHHPLCode->CopyNumberFromString(NextLocationToRead, SIZE_OF_HHPL_TAG_NUMBER, &Tag);
			NextLocationToRead += SIZE_OF_HHPL_TAG_NUMBER;
			ExistingHHPLCode->CopyNumberFromString(NextLocationToRead, SIZE_OF_HHPL_TAG_LENGTH, &Length);
			NextLocationToRead += SIZE_OF_HHPL_TAG_LENGTH;

			Number = 0;

			switch(Tag)
			{
				// General Information Group
			case ImageVersionTag:
				NextLocationToRead += Length;			// Currently, we're discarding the image version.
				break;


			case MainProgramLocationTag:
				ExistingHHPLCode->CopyNumberFromString(NextLocationToRead, Length, &Number);
				NextLocationToRead += Length;
				StoredMainLocation = Number;
				MainLocationFound = true;
				break;


			case HHPLCodeTag:
				ExistingHHPLCode->CopyFromString(NextLocationToRead, Length, &StoredCode);
				NextLocationToRead += Length;
				break;


			case SizeOfDataSpaceTag:
				ExistingHHPLCode->CopyNumberFromString(NextLocationToRead, Length, &Number);
				NextLocationToRead += Length;
				SizeOfDataSpace = Number;
				break;

			case CompilerVersionTag:
				HHPLCompilerVersion->CopyFromHString( ExistingHHPLCode, NextLocationToRead, NextLocationToRead + Length - 1 );
				NextLocationToRead += Length;
				break;


			case FeatureProtectionOverrideTag:
				ExistingHHPLCode->CopyNumberFromString(NextLocationToRead, Length, &Number);
				NextLocationToRead += Length;
				FeatureProtectionOverride = ( Number != 0 ) ? true : false;
				break;




				// Symbol Table Group
			case NumberOfSymbolTableEntriesTag:
				ExistingHHPLCode->CopyNumberFromString(NextLocationToRead, Length, &Number);
				NextLocationToRead += Length;
				StoredNumberOfSymbolTableEntries = Number;
				NumberOfSymbolTableEntriesFound = true;
				break;


			case SymbolTableNamesTag:
				// Get the packed symbol table names; we'll unpack them later.
				ExistingHHPLCode->CopyFromString(NextLocationToRead, Length, &SymbolTableNames);
				NextLocationToRead += Length;
				break;


			case SymbolTableLocationsTag:
				// Get the packed symbol table locations; we'll unpack them later.
				ExistingHHPLCode->CopyFromString(NextLocationToRead, Length, &SymbolTableLocations);
				NextLocationToRead += Length;
				break;


			case SymbolTableTypesTag:
				// Get the packed symbol table types; we'll unpack them later.
				ExistingHHPLCode->CopyFromString(NextLocationToRead, Length, &SymbolTableTypes);
				NextLocationToRead += Length;
				break;


			case SymbolTableArrayElementSizesTag:
				// Get the packed symbol table array element sizes; we'll unpack them later.
				ExistingHHPLCode->CopyFromString(NextLocationToRead, Length, &SymbolTableArrayElementSizes);
				NextLocationToRead += Length;
				break;


			case SymbolTableNumberOfArrayDimensionsTag:
				// Get the packed symbol table number of array dimensions; we'll unpack them later.
				ExistingHHPLCode->CopyFromString(NextLocationToRead, Length, &SymbolTableNumberOfArrayDimensions);
				NextLocationToRead += Length;
				break;


			case SymbolTableSizeOfArrayDimensionsTag:
				// Get the packed symbol table array dimension sizes; we'll unpack them later.
				ExistingHHPLCode->CopyFromString(NextLocationToRead, Length, &SymbolTableSizeOfArrayDimensions);
				NextLocationToRead += Length;
				break;


			case SymbolTableContextNameTag:
				// Get the packed symbol table array dimension sizes; we'll unpack them later.
				ExistingHHPLCode->CopyFromString(NextLocationToRead, Length, &SymbolTableContextNames);
				NextLocationToRead += Length;
				break;





				// Address Resolution Table Group
			case NumberOfAddressResolutionTableEntriesTag:
				ExistingHHPLCode->CopyNumberFromString(NextLocationToRead, Length, &Number);
				NextLocationToRead += Length;
				StoredNumberOfAddressResolutionTableEntries = Number;
				NumberOfAddressResolutionTableEntriesFound = true;
				break;


			case AddressResolutionTableNamesTag:
				// Get the packed address resolution table names; we'll unpack them later.
				ExistingHHPLCode->CopyFromString(NextLocationToRead, Length, &AddressResolutionTableNames);
				NextLocationToRead += Length;
				break;


			case AddressResolutionTableLocationsTag:
				// Get the packed address resolution table locations; we'll unpack them later.
				ExistingHHPLCode->CopyFromString(NextLocationToRead, Length, &AddressResolutionTableLocations);
				NextLocationToRead += Length;
				break;


			case AddressResolutionTableTypesTag:
				// Get the packed address resolution table types; we'll unpack them later.
				ExistingHHPLCode->CopyFromString(NextLocationToRead, Length, &AddressResolutionTableTypes);
				NextLocationToRead += Length;
				break;


			case AddressResolutionTableContextNameTag:
				// Get the packed symbol table array dimension sizes; we'll unpack them later.
				ExistingHHPLCode->CopyFromString(NextLocationToRead, Length, &AddressResolutionTableContextNames);
				NextLocationToRead += Length;
				break;




				// Initializer Table Group
			case NumberOfInitializerTableEntriesTag:
				ExistingHHPLCode->CopyNumberFromString(NextLocationToRead, Length, &Number);
				NextLocationToRead += Length;
				StoredNumberOfInitializerTableEntries = Number;
				NumberOfInitializerTableEntriesFound = true;
				break;


			case InitializerItemStorageAddressTableTag:
				// Get the packed initializer item storage addresses; we'll unpack them later.
				ExistingHHPLCode->CopyFromString(NextLocationToRead, Length, &InitializerItemStorageAddresses);
				NextLocationToRead += Length;
				break;


			case InitializerItemSizeTableTag:
				// Get the packed initializer table item sizes ; we'll unpack them later.
				ExistingHHPLCode->CopyFromString(NextLocationToRead, Length, &InitializerItemSizes);
				NextLocationToRead += Length;
				break;


			case InitializerItemHeapLocationTableTag:
				// Get the packed initializer table item heap locations; we'll unpack them later.
				ExistingHHPLCode->CopyFromString(NextLocationToRead, Length, &InitializerItemHeapLocations);
				NextLocationToRead += Length;
				break;


			case InitializerHeapTag:
				// Get the initializer heap data.
				ExistingHHPLCode->CopyFromString(NextLocationToRead, Length, &InitializerHeapStorage);
				NextLocationToRead += Length;
				break;


			default:
				ReturnStatus |= HHPL_IMAGE_FIELD_NOT_RECOGNIZED;
				NextLocationToRead += Length;
				break;
			}
		}


		// Let's log any errors that we know of.  We'll check the integrity of the symbol and address
		// resolution tables along the way.
		if(!MainLocationFound)
		{
			ReturnStatus |= HHPL_IMAGE_MISSING_MAIN_PROGRAM_LOCATION;
		}

		if(!NumberOfSymbolTableEntriesFound)
		{
			ReturnStatus |= HHPL_IMAGE_MISSING_NUMBER_OF_SYMBOL_TABLE_ENTRIES;
		}

		if(!NumberOfAddressResolutionTableEntriesFound)
		{
			ReturnStatus |= HHPL_IMAGE_MISSING_NUMBER_OF_ADDRESS_RESOLUTION_TABLE_ENTRIES;
		}

		if(!NumberOfInitializerTableEntriesFound)
		{
			ReturnStatus |= HHPL_IMAGE_MISSING_NUMBER_OF_INITIALIZER_TABLE_ENTRIES;
		}




		// If we received symbol table data, let's unpack it and check its' integrity
		if((NumberOfSymbolTableEntriesFound) && (StoredNumberOfSymbolTableEntries))
		{
			unsigned int	NextPositionInNameTableToRead = 0;
			unsigned int	NextPositionInLocationTableToRead = 0;
			unsigned int	NextPositionInTypeTableToRead = 0;
			unsigned int	NextPositionInArrayElementSizesTableToRead = 0;
			unsigned int	NextPositionInNumberOfArrayDimensionsTableToRead = 0;
			unsigned int	NextPositionInArrayDimensionSizesTableToRead = 0;
			unsigned int	NextPositionInContextNamesTableToRead = 0;

			unsigned int	NumberOfSymbolTableNamesFound=0;
			unsigned int	NumberOfSymbolTableLocationsFound=0;
			unsigned int	NumberOfSymbolTableTypesFound=0;
			unsigned int	NumberOfSymbolTableArrayElementSizesFound = 0;
			unsigned int	NumberOfSymbolTableNumberOfArrayDimensionsFound = 0;
			unsigned int	NumberOfSymbolTableArrayDimensionSizesFound = 0;
			unsigned int	NumberOfSymbolTableContextNamesFound = 0;
			unsigned int	LengthOfEachLocationEntry = 0;
			unsigned int	LengthOfEachTypeEntry = 0;
			unsigned int	LengthOfEachArrayElementSizesEntry = 0;
			unsigned int	LengthOfEachNumberOfArrayDimensionsEntry = 0;
			unsigned int	LengthOfEachArrayDimensionSizesEntry = 0;

			// Get the size of each location entry first
			SymbolTableLocations.CopyNumberFromString(NextPositionInLocationTableToRead,
			        SIZE_OF_HHPL_TAG_LENGTH, &LengthOfEachLocationEntry);
			NextPositionInLocationTableToRead += SIZE_OF_HHPL_TAG_LENGTH;

			SymbolTableTypes.CopyNumberFromString(NextPositionInTypeTableToRead,
			                                      SIZE_OF_HHPL_TAG_LENGTH, &LengthOfEachTypeEntry);
			NextPositionInTypeTableToRead += SIZE_OF_HHPL_TAG_LENGTH;

			SymbolTableArrayElementSizes.CopyNumberFromString(NextPositionInArrayElementSizesTableToRead,
			        SIZE_OF_HHPL_TAG_LENGTH, &LengthOfEachArrayElementSizesEntry);
			NextPositionInArrayElementSizesTableToRead += SIZE_OF_HHPL_TAG_LENGTH;

			SymbolTableNumberOfArrayDimensions.CopyNumberFromString(NextPositionInNumberOfArrayDimensionsTableToRead,
			        SIZE_OF_HHPL_TAG_LENGTH, &LengthOfEachNumberOfArrayDimensionsEntry);
			NextPositionInNumberOfArrayDimensionsTableToRead += SIZE_OF_HHPL_TAG_LENGTH;

			SymbolTableSizeOfArrayDimensions.CopyNumberFromString(NextPositionInArrayDimensionSizesTableToRead,
			        SIZE_OF_HHPL_TAG_LENGTH, &LengthOfEachArrayDimensionSizesEntry);
			NextPositionInArrayDimensionSizesTableToRead += SIZE_OF_HHPL_TAG_LENGTH;




			while((NextPositionInNameTableToRead < SymbolTableNames.Size) &&
			        (NumberOfSymbolTableNamesFound < StoredNumberOfSymbolTableEntries) &&
			        (NextPositionInLocationTableToRead < SymbolTableLocations.Size) &&
			        (NumberOfSymbolTableLocationsFound < StoredNumberOfSymbolTableEntries) &&
			        (NextPositionInTypeTableToRead < SymbolTableTypes.Size) &&
			        (NumberOfSymbolTableTypesFound < StoredNumberOfSymbolTableEntries) &&
			        (NextPositionInArrayElementSizesTableToRead < SymbolTableArrayElementSizes.Size) &&
			        (NumberOfSymbolTableArrayElementSizesFound < StoredNumberOfSymbolTableEntries) &&
			        (NextPositionInNumberOfArrayDimensionsTableToRead < SymbolTableNumberOfArrayDimensions.Size) &&
			        (NumberOfSymbolTableNumberOfArrayDimensionsFound < StoredNumberOfSymbolTableEntries) &&
			        (NextPositionInArrayDimensionSizesTableToRead <= SymbolTableSizeOfArrayDimensions.Size) &&
			        (NumberOfSymbolTableArrayDimensionSizesFound < StoredNumberOfSymbolTableEntries) &&
			        (NextPositionInContextNamesTableToRead < SymbolTableContextNames.Size) &&
			        (NumberOfSymbolTableContextNamesFound < StoredNumberOfSymbolTableEntries))
			{
				SymbolTableNames.CopyNumberFromString(NextPositionInNameTableToRead,
				                                      SIZE_OF_HHPL_TAG_LENGTH, &Length);
				NextPositionInNameTableToRead += SIZE_OF_HHPL_TAG_LENGTH;
				TempWStr.CopyFromHString( &SymbolTableNames, NextPositionInNameTableToRead,
				                          NextPositionInNameTableToRead + Length - 1 );
				NextPositionInNameTableToRead += Length;
				++NumberOfSymbolTableNamesFound;

				SymbolTableLocations.CopyNumberFromString(NextPositionInLocationTableToRead,
				        LengthOfEachLocationEntry, &TempNum);
				NextPositionInLocationTableToRead += LengthOfEachLocationEntry;
				++NumberOfSymbolTableLocationsFound;

				SymbolTableTypes.CopyNumberFromString(NextPositionInTypeTableToRead,
				                                      LengthOfEachTypeEntry, &TempType);
				NextPositionInTypeTableToRead += LengthOfEachTypeEntry;
				++NumberOfSymbolTableTypesFound;



				SymbolTableArrayElementSizes.CopyNumberFromString(NextPositionInArrayElementSizesTableToRead,
				        LengthOfEachArrayElementSizesEntry, &TempArrayElementSize);
				NextPositionInArrayElementSizesTableToRead += LengthOfEachArrayElementSizesEntry;
				++NumberOfSymbolTableArrayElementSizesFound;


				SymbolTableNumberOfArrayDimensions.CopyNumberFromString(NextPositionInNumberOfArrayDimensionsTableToRead,
				        LengthOfEachNumberOfArrayDimensionsEntry, &TempNumberOfDimensions);
				NextPositionInNumberOfArrayDimensionsTableToRead += LengthOfEachNumberOfArrayDimensionsEntry;
				++NumberOfSymbolTableNumberOfArrayDimensionsFound;

				// Get the number of dimension sizes based on the number of dimensions we just found above.
				for(i = 0;    i < TempNumberOfDimensions;    ++i)
				{
					SymbolTableSizeOfArrayDimensions.CopyNumberFromString(NextPositionInArrayDimensionSizesTableToRead,
					        LengthOfEachArrayDimensionSizesEntry, &TempSizeOfDimensions[i]);
					NextPositionInArrayDimensionSizesTableToRead += LengthOfEachArrayDimensionSizesEntry;
				}
				++NumberOfSymbolTableArrayDimensionSizesFound;


				SymbolTableContextNames.CopyNumberFromString(NextPositionInContextNamesTableToRead,
				        SIZE_OF_HHPL_TAG_LENGTH, &Length);
				NextPositionInContextNamesTableToRead += SIZE_OF_HHPL_TAG_LENGTH;
				TempContextName.CopyFromHString( &SymbolTableContextNames, NextPositionInContextNamesTableToRead,
				                                 NextPositionInContextNamesTableToRead + Length - 1 );
				NextPositionInContextNamesTableToRead += Length;
				++NumberOfSymbolTableContextNamesFound;


				SymbolTable->Add(&TempWStr, TempType, TempNum, TempArrayElementSize,
				                 TempNumberOfDimensions, TempSizeOfDimensions, &TempContextName);
			}

			// If we didn't get enough table entries, or we still have data left over, WOOF.
			if((NumberOfSymbolTableNamesFound != StoredNumberOfSymbolTableEntries) ||
			        (NextPositionInNameTableToRead < SymbolTableNames.Size))
			{
				ReturnStatus |= HHPL_IMAGE_NUMBER_OF_SYMBOL_TABLE_NAMES_IS_INCORRECT;
			}

			if((NumberOfSymbolTableLocationsFound != StoredNumberOfSymbolTableEntries) ||
			        (NextPositionInLocationTableToRead < SymbolTableLocations.Size))
			{
				ReturnStatus |= HHPL_IMAGE_NUMBER_OF_SYMBOL_TABLE_LOCATIONS_IS_INCORRECT;
			}

			if((NumberOfSymbolTableTypesFound != StoredNumberOfSymbolTableEntries) ||
			        (NextPositionInTypeTableToRead < SymbolTableLocations.Size))
			{
				ReturnStatus |= HHPL_IMAGE_NUMBER_OF_SYMBOL_TABLE_TYPES_IS_INCORRECT;
			}

			if((NumberOfSymbolTableArrayElementSizesFound != StoredNumberOfSymbolTableEntries) ||
			        (NextPositionInArrayElementSizesTableToRead < SymbolTableArrayElementSizes.Size))
			{
				ReturnStatus |= HHPL_IMAGE_NUMBER_OF_SYMBOL_TABLE_ELEMENT_SIZES_IS_INCORRECT;
			}

			if((NumberOfSymbolTableNumberOfArrayDimensionsFound != StoredNumberOfSymbolTableEntries) ||
			        (NextPositionInNumberOfArrayDimensionsTableToRead < SymbolTableNumberOfArrayDimensions.Size))
			{
				ReturnStatus |= HHPL_IMAGE_NUMBER_OF_SYMBOL_TABLE_NUMBER_OF_DIMENSIONS_IS_INCORRECT;
			}

			if((NumberOfSymbolTableArrayDimensionSizesFound != StoredNumberOfSymbolTableEntries) ||
			        (NextPositionInArrayDimensionSizesTableToRead < SymbolTableSizeOfArrayDimensions.Size))
			{
				ReturnStatus |= HHPL_IMAGE_NUMBER_OF_SYMBOL_TABLE_SIZE_OF_DIMENSIONS_IS_INCORRECT;
			}

			if((NumberOfSymbolTableContextNamesFound != StoredNumberOfSymbolTableEntries) ||
			        (NextPositionInContextNamesTableToRead < SymbolTableContextNames.Size))
			{
				ReturnStatus |= HHPL_IMAGE_NUMBER_OF_SYMBOL_TABLE_CONTEXT_NAMES_IS_INCORRECT;
			}
		}




		// If we received address resolution table data, let's unpack it and check its' integrity
		if((NumberOfAddressResolutionTableEntriesFound) && (StoredNumberOfAddressResolutionTableEntries))
		{
			unsigned int	NextPositionInNameTableToRead = 0;
			unsigned int	NextPositionInLocationTableToRead = 0;
			unsigned int	NextPositionInTypeTableToRead = 0;
			unsigned int	NextPositionInContextNamesTableToRead = 0;

			unsigned int	NumberOfAddressResolutionTableNamesFound=0;
			unsigned int	NumberOfAddressResolutionTableLocationsFound=0;
			unsigned int	NumberOfAddressResolutionTableTypesFound=0;
			unsigned int	NumberOfAddressResolutionTableContextNamesFound = 0;
			unsigned int	LengthOfEachLocationEntry = 0;
			unsigned int	LengthOfEachTypeEntry = 0;

			// Get the size of each location entry first
			AddressResolutionTableLocations.CopyNumberFromString(NextPositionInLocationTableToRead,
			        SIZE_OF_HHPL_TAG_LENGTH, &LengthOfEachLocationEntry);
			NextPositionInLocationTableToRead += SIZE_OF_HHPL_TAG_LENGTH;

			AddressResolutionTableTypes.CopyNumberFromString(NextPositionInTypeTableToRead,
			        SIZE_OF_HHPL_TAG_LENGTH, &LengthOfEachTypeEntry);
			NextPositionInTypeTableToRead += SIZE_OF_HHPL_TAG_LENGTH;

			while((NextPositionInNameTableToRead < AddressResolutionTableNames.Size) &&
			        (NumberOfAddressResolutionTableNamesFound <
			         StoredNumberOfAddressResolutionTableEntries) &&
			        (NextPositionInLocationTableToRead < AddressResolutionTableLocations.Size) &&
			        (NumberOfAddressResolutionTableLocationsFound <
			         StoredNumberOfAddressResolutionTableEntries) &&
			        (NextPositionInTypeTableToRead < AddressResolutionTableTypes.Size) &&
			        (NumberOfAddressResolutionTableTypesFound <
			         StoredNumberOfAddressResolutionTableEntries) &&
			        (NextPositionInContextNamesTableToRead < AddressResolutionTableContextNames.Size) &&
			        (NumberOfAddressResolutionTableContextNamesFound < StoredNumberOfAddressResolutionTableEntries))
			{
				AddressResolutionTableNames.CopyNumberFromString(NextPositionInNameTableToRead,
				        SIZE_OF_HHPL_TAG_LENGTH, &Length);
				NextPositionInNameTableToRead += SIZE_OF_HHPL_TAG_LENGTH;
				TempWStr.CopyFromHString( &AddressResolutionTableNames, NextPositionInNameTableToRead,
				                          NextPositionInNameTableToRead + Length - 1 );
				NextPositionInNameTableToRead += Length;
				++NumberOfAddressResolutionTableNamesFound;

				AddressResolutionTableLocations.CopyNumberFromString
				(NextPositionInLocationTableToRead, LengthOfEachLocationEntry, &TempNum);
				NextPositionInLocationTableToRead += LengthOfEachLocationEntry;
				++NumberOfAddressResolutionTableLocationsFound;

				AddressResolutionTableTypes.CopyNumberFromString
				(NextPositionInTypeTableToRead, LengthOfEachTypeEntry, &TempType);
				NextPositionInTypeTableToRead += LengthOfEachTypeEntry;
				++NumberOfAddressResolutionTableTypesFound;


				AddressResolutionTableContextNames.CopyNumberFromString(NextPositionInContextNamesTableToRead,
				        SIZE_OF_HHPL_TAG_LENGTH, &Length);
				NextPositionInContextNamesTableToRead += SIZE_OF_HHPL_TAG_LENGTH;
				TempContextName.CopyFromHString( &AddressResolutionTableContextNames, NextPositionInContextNamesTableToRead,
				                                 NextPositionInContextNamesTableToRead + Length - 1 );
				NextPositionInContextNamesTableToRead += Length;
				++NumberOfAddressResolutionTableContextNamesFound;


				AddressResolutionTable->Add(&TempWStr, TempType, TempNum, 0, 0, 0, &TempContextName);
			}


			// If we didn't get enough table entries, or we still have data left over, WOOF.
			if((NumberOfAddressResolutionTableNamesFound != StoredNumberOfAddressResolutionTableEntries) ||
			        (NextLocationToRead < AddressResolutionTableNames.Size))
			{
				ReturnStatus |= HHPL_IMAGE_NUMBER_OF_ADDRESS_RESOLUTION_TABLE_NAMES_IS_INCORRECT;
			}

			if((NumberOfAddressResolutionTableLocationsFound != StoredNumberOfAddressResolutionTableEntries) ||
			        (NextPositionInLocationTableToRead < AddressResolutionTableLocations.Size))
			{
				ReturnStatus |= HHPL_IMAGE_NUMBER_OF_ADDRESS_RESOLUTION_TABLE_LOCATIONS_IS_INCORRECT;
			}

			if((NumberOfAddressResolutionTableTypesFound != StoredNumberOfAddressResolutionTableEntries) ||
			        (NextPositionInTypeTableToRead < AddressResolutionTableTypes.Size))
			{
				ReturnStatus |= HHPL_IMAGE_NUMBER_OF_ADDRESS_RESOLUTION_TABLE_TYPES_IS_INCORRECT;
			}

			if((NumberOfAddressResolutionTableContextNamesFound != StoredNumberOfAddressResolutionTableEntries) ||
			        (NextPositionInContextNamesTableToRead < AddressResolutionTableContextNames.Size))
			{
				ReturnStatus |= HHPL_IMAGE_NUMBER_OF_ADDRESS_RESOLUTION_TABLE_CONTEXT_NAMES_IS_INCORRECT;
			}
		}




		// If we received initializer table data, let's unpack it and check its' integrity
		if((NumberOfInitializerTableEntriesFound) && (StoredNumberOfInitializerTableEntries))
		{
			unsigned int	NextPositionInStorageAddressTableToRead = 0;
			unsigned int	NextPositionInSizeTableToRead = 0;
			unsigned int	NextPositionInHeapLocationTableToRead = 0;

			unsigned int	NumberOfStorageAddressesFound=0;
			unsigned int	NumberOfSizesFound=0;
			unsigned int	NumberOfHeapLocationsFound=0;

			int		NextItemStorageAddress;
			int		NextItemSize;
			int		NextItemHeapLocation;


			// Get the size of each location entry first
			while((NextPositionInStorageAddressTableToRead < InitializerItemStorageAddresses.Size) &&
			        (NumberOfStorageAddressesFound < StoredNumberOfInitializerTableEntries) &&
			        (NextPositionInSizeTableToRead < InitializerItemSizes.Size) &&
			        (NumberOfSizesFound < StoredNumberOfInitializerTableEntries) &&
			        (NextPositionInHeapLocationTableToRead < InitializerItemHeapLocations.Size) &&
			        (NumberOfHeapLocationsFound < StoredNumberOfInitializerTableEntries))
			{
				InitializerItemStorageAddresses.CopyNumberFromString(NextPositionInStorageAddressTableToRead,
				        VERSACODE_BYTES_IN_NUMBER, &NextItemStorageAddress);
				NextPositionInStorageAddressTableToRead += VERSACODE_BYTES_IN_NUMBER;
				++NumberOfStorageAddressesFound;

				InitializerItemSizes.CopyNumberFromString(NextPositionInSizeTableToRead,
				        VERSACODE_BYTES_IN_NUMBER, &NextItemSize);
				NextPositionInSizeTableToRead += VERSACODE_BYTES_IN_NUMBER;
				++NumberOfSizesFound;

				InitializerItemHeapLocations.CopyNumberFromString(NextPositionInHeapLocationTableToRead,
				        VERSACODE_BYTES_IN_NUMBER, &NextItemHeapLocation);
				NextPositionInHeapLocationTableToRead += VERSACODE_BYTES_IN_NUMBER;
				++NumberOfHeapLocationsFound;

				AddVariableInitializer(&(InitializerHeapStorage.Char[NextItemHeapLocation]),
				                       ((unsigned int) NextItemSize), ((unsigned int) NextItemStorageAddress));
			}

			// If we didn't get enough table entries, or we still have data left over, WOOF.
			if((NumberOfStorageAddressesFound != StoredNumberOfInitializerTableEntries) ||
			        (NextPositionInStorageAddressTableToRead < InitializerItemStorageAddresses.Size))
			{
				ReturnStatus |= HHPL_IMAGE_NUMBER_OF_INITIALIZER_ITEM_STORAGE_ADDRESSES_IS_INCORRECT;
			}

			if((NumberOfSizesFound != StoredNumberOfInitializerTableEntries) ||
			        (NextPositionInSizeTableToRead < InitializerItemSizes.Size))
			{
				ReturnStatus |= HHPL_IMAGE_NUMBER_OF_INITIALIZER_ITEM_SIZES_IS_INCORRECT;
			}

			if((NumberOfHeapLocationsFound != StoredNumberOfInitializerTableEntries) ||
			        (NextPositionInHeapLocationTableToRead < InitializerItemHeapLocations.Size))
			{
				ReturnStatus |= HHPL_IMAGE_NUMBER_OF_INITIALIZER_ITEM_HEAP_LOCATIONS_IS_INCORRECT;
			}
		}




		// If we succeeded (the only allowable error is that we ran across a field we didn't understand,
		// let's finish updating the local data.
		if((ReturnStatus & (~HHPL_IMAGE_FIELD_NOT_RECOGNIZED)) == HHPL_IMAGE_DECODED_OK)
		{
			MainProgramLocation = StoredMainLocation;

			Code->Copy(&StoredCode);
		}

		// We had at least one serious error.  Let's reinitialize the symbol and address resolution tables.
		else
		{
			RemoveTables();
			InitializeTables();
		}
	}



	return (ReturnStatus);
}






//
//	Insert
//		Insert instructions into HHPL code.  Adjust the symbol table and address resolution
//		table accordingly.
//
bool HHPL_IMAGE_DATA::Insert(HSTRING *NewInstructions, int InsertLocation)
{
	bool	InsertIsSuccessful = true;		// Assume it worked until we know otherwise.
	int		i;								// Index




	// Insert the code itself.
	Code->Insert(NewInstructions, ((unsigned int) InsertLocation));

	// Update the main program location if it is above the insertion area
	if(MainProgramLocation >= ((unsigned int) InsertLocation))
	{
		MainProgramLocation += NewInstructions->Size;
	}

	// If the code insert worked, let's do the symbol table.
	for(i = 0;    i < (int) SymbolTable->NumberOfTableEntries;    ++i)
	{
		// Only alter symbols that are in code space
		if(((SymbolTable->TableEntries[i].Type & HHPL_ITEM_TABLE::ROUTINE_ITEM) ||
		        (SymbolTable->TableEntries[i].Type & HHPL_ITEM_TABLE::LABEL_ITEM)) &&
		        (SymbolTable->TableEntries[i].Location >= ((unsigned int) InsertLocation)))
		{
			SymbolTable->TableEntries[i].Location += NewInstructions->Size;
		}
	}

	// Now let's do the address request table.
	for(i = 0;    i < (int) AddressResolutionTable->NumberOfTableEntries;    ++i)
	{
		if(AddressResolutionTable->TableEntries[i].Location >= ((unsigned int) InsertLocation))
		{
			AddressResolutionTable->TableEntries[i].Location += NewInstructions->Size;
		}
	}


	return (InsertIsSuccessful);
} // Insert






//
//	Remove (version #1 with size argument)
//		Remove code and symbol information above the caller specified address.
//
bool HHPL_IMAGE_DATA::Remove(int RemovalStartLocation, int SizeOfAreaToRemove)
{
	bool			RemoveIsSuccessful = true;		// Assume it worked until we know otherwise.
	unsigned int	i;								// Index




	// Is the removal range OK?
	if((RemovalStartLocation < ((int) Code->Size)) && (SizeOfAreaToRemove <= ((int) Code->Size)) &&
	        ((RemovalStartLocation + SizeOfAreaToRemove) <= ((int) Code->Size)))
	{
		// Remove the code itself.
		RemoveIsSuccessful = Code->Remove(((unsigned int) RemovalStartLocation), ((unsigned int) SizeOfAreaToRemove));
		if(RemoveIsSuccessful)
		{
			// If the code remove worked, let's do the symbol table.
			i = 0;
			while(i < SymbolTable->NumberOfTableEntries)
			{
				// Only remove symbols that are in code space
				if(((SymbolTable->TableEntries[i].Type & HHPL_ITEM_TABLE::ROUTINE_ITEM) ||
				        (SymbolTable->TableEntries[i].Type & HHPL_ITEM_TABLE::LABEL_ITEM)) &&
				        (SymbolTable->TableEntries[i].Location >= ((unsigned int) RemovalStartLocation)))
				{
					if(SymbolTable->TableEntries[i].Location < ((unsigned int) (RemovalStartLocation + SizeOfAreaToRemove)))
					{
						// Symbol is in removal range; squeeze it out.
						SymbolTable->RemoveItem(i);
					}
					else
					{
						// Symbol is above removal range; adjust the address
						SymbolTable->TableEntries[i].Location -= ((unsigned int) SizeOfAreaToRemove);
						++i;
					}
				}
				else
				{
					++i;
				}
			}

			// Now let's do the address request table.
			i = 0;
			while(i < AddressResolutionTable->NumberOfTableEntries)
			{
				if(AddressResolutionTable->TableEntries[i].Location >= ((unsigned int) RemovalStartLocation))
				{
					if(AddressResolutionTable->TableEntries[i].Location <
					        ((unsigned int) (RemovalStartLocation + SizeOfAreaToRemove)))
					{
						// Symbol is in removal range; squeeze it out.
						AddressResolutionTable->RemoveItem(i);
					}
					else
					{
						// Symbol is above removal range; adjust the address
						AddressResolutionTable->TableEntries[i].Location -= ((unsigned int) SizeOfAreaToRemove);
						++i;
					}
				}
				else
				{
					++i;
				}
			}
		}
	}

	else
	{
		// The removal range was bad; WOOF!
		RemoveIsSuccessful = false;
	}


	return (RemoveIsSuccessful);
} // Remove (version #1)






//
//	Remove (version #2 that removes everything after the caller specified address)
//		Remove code and symbol information above the caller specified address.
//
bool HHPL_IMAGE_DATA::Remove(int RemovalStartLocation)
{
	return(Remove(RemovalStartLocation, Code->Size - RemovalStartLocation));
} // Remove (version #2)






//
//	GetArrayInformation
//		Get array information.
//
bool HHPL_IMAGE_DATA::GetArrayInformation(HWSTRING *VariableName, int *SizeOfEachElement,
        int *NumberOfDimensions, int *SizeOfDimensions)
{
	bool			NameFound=false;				// Haven't found it yet.
	unsigned int	NextSymbolToBeChecked;			// Index
	int				i;




	NextSymbolToBeChecked = 0;

	// Keep looking until we find it or we run out of symbol table entries.
	while((!NameFound) && (NextSymbolToBeChecked < SymbolTable->NumberOfTableEntries))
	{
		if(SymbolTable->TableEntries[NextSymbolToBeChecked].Name->Compare(VariableName, false) == 0)
		{
			NameFound = true;

			*SizeOfEachElement = SymbolTable->TableEntries[NextSymbolToBeChecked].SizeOfEachElement;
			*NumberOfDimensions = SymbolTable->TableEntries[NextSymbolToBeChecked].NumberOfDimensions;
			for(i = 0;    i < MAXIMUM_NUMBER_OF_ARRAY_DIMENSIONS;     ++i)
			{
				SizeOfDimensions[i] = SymbolTable->TableEntries[NextSymbolToBeChecked].SizeOfDimensions[i];
			}
		}
		else
		{
			++NextSymbolToBeChecked;
		}
	}


	return(NameFound);
} // GetArrayInformation




