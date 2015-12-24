//======================================================================================
// HHPLItemTable.h
//======================================================================================
// $RCSfile: hhplitemtable.h $
// $Revision: 1.2 $
// $Date: 2011/11/14 17:05:03EST $
//======================================================================================

//======================================================================================



#ifndef __HHPL_ITEM_TABLE_H
#define __HHPL_ITEM_TABLE_H



#include	"HSTRING.h"					// HSTRING interface definition
#include	"HWSTRING.h"				// HWSTRING interface definition

#include	"CRC.h"						// CRC generator for protecting script code



class HHPL_ITEM_TABLE
{
public:

	#define	GLOBAL_CONTEXT_IDENTIFIER		L"GLOBAL$"
	#define	MAIN_CONTEXT_IDENTIFIER			L"MAIN$"
	#define	ROUTINE_CONTEXT_IDENTIFIER		L"ROUTINE$"
	#define	CONTEXT_LEVEL_SEPARATOR			L"$"


	#define	MAXIMUM_NUMBER_OF_ARRAY_DIMENSIONS		5


	HWSTRING	*ErrorMessage;		// Holds error messages generated during address resolution.


	#define NUMBER_OF_FREE_ELEMENTS_ON_ITEM_TABLE_REALLOCATION 100


	enum {									// Defines entry types in ADDRESS_TABLE
		ROUTINE_ITEM		=0x00000001,	// Subroutine declaration or reference
		LABEL_ITEM			=0x00000002,	// Label statement declaration or reference
		BOOLEAN_ITEM		=0x00000004,	// Boolean variable declaration or reference
		INTEGER_ITEM		=0x00000008,	// Integer variable declaration or reference
		BYTE_STRING_ITEM	=0x00000010,	// Byte string variable declaration or reference
		STRING_ITEM			=0x00000020,	// String variable declaration or reference
		ITEM_TYPE_MASK		=0x0000003F,
		HOST_ACCESSIBLE		=0x80000000,	// Subroutine declaration that may be executed from an outside host
		NUMBER_OF_ITEM_TYPES
	} ADDRESS_TABLE_ITEM_TYPES;

	

	typedef struct {
		HWSTRING		*Name;		// For symbol table, this is the name of a routine, label, or
									//	variable declaration; for address resolution table, this is
									//	the name of a reference to a routine, label, or variable.
		unsigned int	Type;		// This is the type (routine, label, or variable type) of item
									//	that is being referenced in this symbol table or address
									//	resolution table slot.
		unsigned int	Location;	// For symbol table, this is the location in the code or data
									//	space that the symbol points to; for address resolution table,
									//	this is the location in the code of a reference to be resolved.
		int				SizeOfEachElement;	// Size of each element, if item is an array.
		int				NumberOfDimensions;	// Number of dimension, if this is an array.
		int				SizeOfDimensions[MAXIMUM_NUMBER_OF_ARRAY_DIMENSIONS];	// Size of each dimension,
																			//	if this is an array.
		HWSTRING	*ContextName;			// Name of context for this item.
	} ITEM_TABLE_ENTRY;

	unsigned int		TableSize;
	unsigned int		NumberOfTableEntries;
	ITEM_TABLE_ENTRY	*TableEntries;
	


	




//	Constructor #1 for HHPL_ITEM_TABLE; create with no entries.
	HHPL_ITEM_TABLE(void);

//	Constructor #2 for HHPL_ITEM_TABLE; create with entries copied from an existing table.
	HHPL_ITEM_TABLE(HHPL_ITEM_TABLE *TableToCopy);

//	Destructor for HHPL_ITEM_TABLE
	~HHPL_ITEM_TABLE(void);

// Create a new item table on construction or after complete deletion.
	void Initialize(void);

// Completely delete an item table.
	void Remove(void);

//	Add a new item.
	bool Add(HWSTRING *Name, unsigned int Type, unsigned int Location,
		int SizeOfEachElement, int NumberOfDimensions, int *SizeOfDimensions,
		HWSTRING *ContextName);

// Search the item table to see if we find a match with the caller specified name.
	int SearchByName(HWSTRING *Name);

// Search the item table to see if we find a match with the caller specified name and type.
	int SearchByNameAndType(HWSTRING *Name, unsigned int Type);


// Check to make sure that each item name is unique.
	bool NoDuplicateSymbols(void);


// Copy another table to this table.
	bool Copy(HHPL_ITEM_TABLE *TableToCopy);


// Remove all items matching the type mask sent by the caller.
	void RemoveByType(unsigned int TypesToRemove);


// Remove the item specified by the index.
	void RemoveItem(unsigned int ItemNumberToRemove);


// Remove all items matching the type mask sent by the caller that are also not marked as host.
// Any types in this list will be removed.  Also, any types remaining that are not host
// accessible will also be removed.
	void RemoveByNonHostType(unsigned int TypesToRemove);


private:
	HHPL_ITEM_TABLE (const HHPL_ITEM_TABLE&);            // declared private, but not defined, to prevent copying         
	HHPL_ITEM_TABLE& operator=(const HHPL_ITEM_TABLE&);  // declared private, but not defined, to prevent copying 



};
#endif		//	__HHPL_ITEM_TABLE_H


