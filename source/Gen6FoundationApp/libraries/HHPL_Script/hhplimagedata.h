//======================================================================================
// HHPLImageData.h
//======================================================================================
// $RCSfile: hhplimagedata.h $
// $Revision: 1.2 $
// $Date: 2011/11/11 17:01:40EST $
//======================================================================================

//======================================================================================



#ifndef __HHPL_IMAGE_DATA_H
#define __HHPL_IMAGE_DATA_H



#include	"HSTRING.h"					// HSTRING interface definition
#include	"HWSTRING.h"				// HWSTRING interface definition

#include	"HHPLItemTable.h"

#include	"CRC.h"						// CRC generator for protecting script code



class HHPL_IMAGE_DATA
{
public:


	HWSTRING	*ErrorMessage;		// Holds error messages generated during address resolution.


	#define		HHPL_IMAGE_DATA_VERSION				L"1.1"


	#define NUMBER_OF_FREE_ELEMENTS_ON_TABLE_REALLOCATION 100


	HHPL_ITEM_TABLE		*SymbolTable;
	HHPL_ITEM_TABLE		*AddressResolutionTable;




//
// Initialization Table Description
//	In order to support run-time initialization of data, we need to keep track of initialization
//	information.  We actually store the initialization values on a heap.  We reference those
//	values from an initializer table, specifying the pointer in the heap to a value, the size of
//	the value, and where the value is to be written to in data space.
//
	typedef struct {
		int			AddressToStoreInitializationValue;
		int			SizeOfInitializationValue;
		int			LocationInInitializerHeapOfInitializationValue;
	} INITIALIZER_TABLE;

	unsigned int		SizeOfInitializerTable;
	unsigned int		NumberOfInitializerTableEntries;
	INITIALIZER_TABLE	*InitializerTable;

	HSTRING				*InitializerValueHeap;



	unsigned int		MainProgramLocation;


	bool				FeatureProtectionOverride;	// If set, the target system should allow this script to run if scripting
													//	is disabled because it is an upgrade option.


	HSTRING				*Code;


	unsigned int		SizeOfDataSpace;


	HWSTRING			*HHPLCompilerVersion;
	HWSTRING			*HHPLCompilerVersionInLoadedImage;


// Return codes for script image decoding.
	enum {
		HHPL_IMAGE_DECODED_OK														= 0x00000000,
        HHPL_IMAGE_FIELD_NOT_RECOGNIZED												= 0x00000001,
        HHPL_IMAGE_MISSING_NUMBER_OF_SYMBOL_TABLE_ENTRIES							= 0x00000002,
        HHPL_IMAGE_NUMBER_OF_SYMBOL_TABLE_NAMES_IS_INCORRECT						= 0x00000004,
        HHPL_IMAGE_NUMBER_OF_SYMBOL_TABLE_LOCATIONS_IS_INCORRECT					= 0x00000008,
        HHPL_IMAGE_NUMBER_OF_SYMBOL_TABLE_TYPES_IS_INCORRECT						= 0x00000010,
        HHPL_IMAGE_NUMBER_OF_SYMBOL_TABLE_ELEMENT_SIZES_IS_INCORRECT				= 0x00000020,
        HHPL_IMAGE_NUMBER_OF_SYMBOL_TABLE_NUMBER_OF_DIMENSIONS_IS_INCORRECT			= 0x00000040,
        HHPL_IMAGE_NUMBER_OF_SYMBOL_TABLE_SIZE_OF_DIMENSIONS_IS_INCORRECT			= 0x00000080,
        HHPL_IMAGE_MISSING_NUMBER_OF_ADDRESS_RESOLUTION_TABLE_ENTRIES				= 0x00000100,
        HHPL_IMAGE_NUMBER_OF_ADDRESS_RESOLUTION_TABLE_NAMES_IS_INCORRECT			= 0x00000200,
        HHPL_IMAGE_NUMBER_OF_ADDRESS_RESOLUTION_TABLE_LOCATIONS_IS_INCORRECT		= 0x00000400,
        HHPL_IMAGE_NUMBER_OF_ADDRESS_RESOLUTION_TABLE_TYPES_IS_INCORRECT			= 0x00000800,
		HHPL_IMAGE_MISSING_MAIN_PROGRAM_LOCATION									= 0x00001000,
		HHPL_IMAGE_NUMBER_OF_INITIALIZER_ITEM_STORAGE_ADDRESSES_IS_INCORRECT		= 0x00002000,
		HHPL_IMAGE_NUMBER_OF_INITIALIZER_ITEM_SIZES_IS_INCORRECT					= 0x00004000,
		HHPL_IMAGE_NUMBER_OF_INITIALIZER_ITEM_HEAP_LOCATIONS_IS_INCORRECT			= 0x00008000,
		HHPL_IMAGE_MISSING_NUMBER_OF_INITIALIZER_TABLE_ENTRIES						= 0x00010000,
    	HHPL_IMAGE_SIZE_MISMATCH													= 0x00020000,
    	HHPL_IMAGE_CRC_MISMATCH														= 0x00040000,
		HHPL_IMAGE_MISSING_IDENTIFIER												= 0x00080000,
		HHPL_IMAGE_DECOMPRESSION_ERROR												= 0x00100000,
		HHPL_IMAGE_UNCOMPRESSED														= 0x00200000,
		HHPL_IMAGE_NUMBER_OF_SYMBOL_TABLE_CONTEXT_NAMES_IS_INCORRECT				= 0x00400000,
		HHPL_IMAGE_NUMBER_OF_ADDRESS_RESOLUTION_TABLE_CONTEXT_NAMES_IS_INCORRECT	= 0x00800000,
		HHPL_IMAGE_NO_MEMORY_AVAILABLE												= 0x01000000
	};



// Code image creation flags
	enum {
		MINIMUM_IMAGE							= 0x00000000,
		INCLUDE_HOST_ROUTINE_ENTRIES			= 0x00000001,
		INCLUDE_ROUTINE_ENTRIES					= 0x00000002,
		INCLUDE_ENTIRE_SYMBOL_TABLE				= 0x00000004,
		INCLUDE_ENTIRE_ADDRESS_RESOLUTION_TABLE	= 0x00000008,
		INCLUDE_ALL								= 0x00000010,
		INCLUDE_TABLE_MASK						= 0x0000001F,
		COMPRESS_IMAGE_LEMPEL_ZIV_2				= 0x40000000,
		COMPRESS_IMAGE_HUFFMAN_1				= 0x80000000
	} CODE_IMAGE_CREATION_FLAGS;



//	Constructor #1 for HHPL_IMAGE_DATA; no initial code
	HHPL_IMAGE_DATA(const wchar_t *CompilerVersion, bool InputFeatureProtectionOverride = false );


//	Constructor #2 for HHPL_IMAGE_DATA; load tagged HHPL code sent by caller and store locally.
	HHPL_IMAGE_DATA(const wchar_t *CompilerVersion, HSTRING *ExistingHHPLCode, bool InputFeatureProtectionOverride = false );


//	Constructor #3 for HHPL_IMAGE_DATA; load tagged HHPL code sent by caller and store locally.
	HHPL_IMAGE_DATA(const wchar_t *CompilerVersion, const unsigned char *ExistingHHPLCode,
					unsigned int SizeOfExistingHHPLCode, bool InputFeatureProtectionOverride = false);


//	Destructor for HHPL_IMAGE_DATA
	~HHPL_IMAGE_DATA(void);

//	Add a new routine location to the symbol table, reallocating for more space if necessary.
	void AddRoutineLocation(HWSTRING *Name, unsigned int Location, HWSTRING *ContextName);

//	Add a new routine location to the symbol table, marking it as host accessible and reallocating
//	for more space if necessary.
	void AddHostRoutineLocation(HWSTRING *Name, unsigned int Location, HWSTRING *ContextName);

//	Add a new label location to the symbol table, reallocating for more space if necessary.
	void AddStatementLabelLocation(HWSTRING *Name, unsigned int Location, HWSTRING *ContextName);

//	Add a new boolean variable location to the symbol table, reallocating for more space if
//	necessary.
	void AddBooleanLocation(HWSTRING *Name, unsigned int Location, unsigned int SizeOfNewData,
			int TotalNumberOfArrayElements, int NumberOfDimensions, int *SizeOfDimensions,
			HWSTRING *ContextName);

//	Add a new integer variable location to the symbol table, reallocating for more space if
//	necessary.
	void AddIntegerLocation(HWSTRING *Name, unsigned int Location, unsigned int SizeOfNewData,
			int TotalNumberOfArrayElements, int NumberOfDimensions, int *SizeOfDimensions,
			HWSTRING *ContextName);

//	Add a new string variable location to the symbol table, reallocating for more space if
//	necessary.
	void AddStringLocation(HWSTRING *Name, unsigned int Location, unsigned int SizeOfNewData,
			int TotalNumberOfArrayElements, int NumberOfDimensions, int *SizeOfDimensions,
			HWSTRING *ContextName);

//	Add a new byte string variable location to the symbol table, reallocating for more space if
//	necessary.
	void AddByteStringLocation(HWSTRING *Name, unsigned int Location, unsigned int SizeOfNewData,
			int TotalNumberOfArrayElements, int NumberOfDimensions, int *SizeOfDimensions,
			HWSTRING *ContextName);


//	Add a new routine address request to the address resolution table, reallocating for more space
//	if necessary.
	void AddRoutineAddressRequestLocation(HWSTRING *Name, unsigned int Location, HWSTRING *ContextName);

//	Add a new label address request to the address resolution table, reallocating for more space
//	if necessary.
	void AddStatementLabelAddressRequestLocation(HWSTRING *Name, unsigned int Location, HWSTRING *ContextName);

//	Add a new boolean variable address request to the address resolution table, reallocating for
//	more space if necessary.
	void AddBooleanAddressRequestLocation(HWSTRING *Name, unsigned int Location, HWSTRING *ContextName);

//	Add a new integer variable address request to the address resolution table, reallocating for
//	more space if necessary.
	void AddIntegerAddressRequestLocation(HWSTRING *Name, unsigned int Location, HWSTRING *ContextName);

//	Add a new string variable address request to the address resolution table, reallocating for
//	more space if necessary.
	void AddStringAddressRequestLocation(HWSTRING *Name, unsigned int Location, HWSTRING *ContextName);

//	Add a new byte string variable address request to the address resolution table, reallocating for
//	more space if necessary.
	void AddByteStringAddressRequestLocation(HWSTRING *Name, unsigned int Location, HWSTRING *ContextName);



// Search the symbol table to see if we find a routine with the caller specified name.
	bool NameIsRoutine(HWSTRING *Name);


// Search the symbol table to see if we find a label with the caller specified name.
	bool NameIsStatementLabel(HWSTRING *Name);


// Search the symbol table to see if we find an integer with the caller specified name.
	bool NameIsIntegerVariable(HWSTRING *Name);


// Search the symbol table to see if we find a boolean with the caller specified name.
	bool NameIsBooleanVariable(HWSTRING *Name);


// Search the symbol table to see if we find a string with the caller specified name.
	bool NameIsStringVariable(HWSTRING *Name);


// Search the symbol table to see if we find a byte string with the caller specified name.
	bool NameIsByteStringVariable(HWSTRING *Name);



//	Set the main program location and update the tagged table
//
	void SetMainProgramLocation(unsigned int Location);


// Check to make sure that each symbol name is unique.
	bool NoDuplicateSymbols(void);


//	Resolve address in the address resolution table against the symbol table.  Notify the
//	user of any errors via ErrorMessage.
	bool ResolveAddresses(void);


//	Create HHPL code image to an empty string sent by the caller.  The tagged code is
//	simply a string with concatenated tagged fields.  Version #1 - specify whether or not to
//	strip out symbol table information.
	bool CreateImage(HSTRING *OutputCodeAndTable, int CreateImageFlags);


//	Create HHPL code image to an empty string sent by the caller.  The tagged code is
//	simply a string with concatenated tagged fields.  Version #2 - automatically include
//	symbol table information and create uncompressed.
	bool CreateImage(HSTRING *OutputCodeAndTable);


//	Create HHPL code image to an empty string sent by the caller.  The tagged code is
//	simply a string with concatenated tagged fields.  The symbol table information will be
//	stripped out (not included) in the image and the image will be uncompressed.
	bool CreateStrippedImage(HSTRING *OutputCodeAndTable);


//	Load the local HHPL code storage with the tagged HHPL code sent by the caller.
	int LoadImage(HSTRING *ExistingHHPLCode);


// Insert instructions into HHPL code.  Adjust the symbol table and address resolution
// table accordingly.
	bool Insert(HSTRING *NewInstructions, int InsertLocation);


// Remove (version #1 with size argument)
	bool Remove(int RemovalStartLocation, int SizeOfAreaToRemove);


// Remove (version #2 that removes everything after the caller specified address)
	bool Remove(int RemovalStartLocation);


// Get array information.
	bool GetArrayInformation(HWSTRING *VariableName, int *SizeOfEachElement,
									int *NumberOfDimensions, int *SizeOfDimensions);


private:


//
// Tag Definitions
//
//	Tags consist of 48 bits where the first 16 bits are the tag number and the second 32 bits are a length field.
//	Tag groups (such as size fields), should be grouped on boundaries of 100.  The length field represents the
//	number of bytes in the field.
//
// Define sizes
    #define     SIZE_OF_HHPL_TAG_NUMBER								2
    #define     SIZE_OF_HHPL_TAG_LENGTH								4

// General Information Group
	#define		ImageVersionTag									0
	#define		MainProgramLocationTag							1
	#define		HHPLCodeTag										2
	#define		SizeOfDataSpaceTag								3
	#define		CompilerVersionTag								4
	#define		FeatureProtectionOverrideTag					5

// Symbol Table Group
	#define		NumberOfSymbolTableEntriesTag					100
	#define		SymbolTableNamesTag								101
	#define		SymbolTableLocationsTag							102
	#define		SymbolTableTypesTag								103
	#define		SymbolTableArrayElementSizesTag					104
	#define		SymbolTableNumberOfArrayDimensionsTag			105
	#define		SymbolTableSizeOfArrayDimensionsTag				106
	#define		SymbolTableContextNameTag						107

// Address Resolution Table Group
	#define		NumberOfAddressResolutionTableEntriesTag		200
	#define		AddressResolutionTableNamesTag					201
	#define		AddressResolutionTableLocationsTag				202
	#define		AddressResolutionTableTypesTag					203
	#define		AddressResolutionTableContextNameTag			204

// Initializer Table Group
	#define		NumberOfInitializerTableEntriesTag				300
	#define		InitializerItemStorageAddressTableTag			301
	#define		InitializerItemSizeTableTag						302
	#define		InitializerItemHeapLocationTableTag				303
	#define		InitializerHeapTag								304



// Add a variable initializer entry to the initializer table
	void AddVariableInitializer(void *InitializerData, unsigned int SizeOfInitializerData,
										   unsigned int DataSpaceLocationWhereInitializerWillGo);


// Allocate and initialize symbol, address, and variable initializer tables.
	void InitializeTables(void);


//	Free symbol, address, and variable initializer table memory.
	void RemoveTables(void);

private:
	HHPL_IMAGE_DATA (const HHPL_IMAGE_DATA&);            // declared private, but not defined, to prevent copying         
	HHPL_IMAGE_DATA& operator=(const HHPL_IMAGE_DATA&);  // declared private, but not defined, to prevent copying 
	
};
#endif		//	__HHPL_IMAGE_DATA_H


