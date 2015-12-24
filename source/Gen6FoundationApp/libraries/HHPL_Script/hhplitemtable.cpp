//======================================================================================
// HHPLItemTable.cpp
//======================================================================================
// $RCSfile: hhplitemtable.cpp $
// $Revision: 1.1 $
// $Date: 2009/06/12 16:55:19EDT $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Class Name:     HHPL_ITEM_TABLE
//
//  Class Description:
//		HHPL_ITEM_TABLE is a class that manages the tables of variable, routine, and label
//		item information that are encountered during compilation.
//
//---------------------------------------------------------------------------






#include	"HHPLItemTable.h"	// HHPL item table definitions


#include	<ctype.h>				// data type conversion functions
#include	<string.h>				// 'C' string functions
#include	<stdio.h>				// Standard I/O







//
//	Constructor #1 for HHPL_ITEM_TABLE; create with no entries.
//		Allocate and initialize storage.
//
HHPL_ITEM_TABLE::HHPL_ITEM_TABLE(void)
{
// Allocate error message
	ErrorMessage = new HWSTRING(160, true);

// Initialize table
	Initialize();
	

	return;
}  // Constructor #1






//
//	Constructor #2 for HHPL_ITEM_TABLE; create with entries copied from an existing table.
//		Allocate and initialize storage.
//
HHPL_ITEM_TABLE::HHPL_ITEM_TABLE(HHPL_ITEM_TABLE *TableToCopy)
{
// Allocate error message
	ErrorMessage = new HWSTRING(160, true);

// Initialize table
	Initialize();

// Copy existing table
	Copy(TableToCopy);
	

	return;
}  // Constructor #1






//
//	Destructor for HHPL_ITEM_TABLE
//		Free memory.
//
HHPL_ITEM_TABLE::~HHPL_ITEM_TABLE()
{
	delete ErrorMessage;

	Remove();


    return;
}  // Destructor





//
//	Initialize
//		Create a new item table on construction or after complete deletion.
//
void HHPL_ITEM_TABLE::Initialize(void)
{
	TableSize = NUMBER_OF_FREE_ELEMENTS_ON_ITEM_TABLE_REALLOCATION;
	NumberOfTableEntries = 0;
	TableEntries = new ITEM_TABLE_ENTRY[TableSize];

	return;
} // Initialize





//
//	Remove
//		Completely delete an item table.
//
void HHPL_ITEM_TABLE::Remove(void)
{
	unsigned int	i;


	for(i = 0;    i < NumberOfTableEntries;    ++i)
	{
		delete TableEntries[i].Name;
		delete TableEntries[i].ContextName;
	}
	delete TableEntries;


    return;
} // Remove




//
//	Add
//		Add a new item to the table, reallocating for more space if necessary.
//
bool HHPL_ITEM_TABLE::Add(HWSTRING *Name, unsigned int Type, unsigned int Location,
		int SizeOfEachElement, int NumberOfDimensions, int *SizeOfDimensions,
		HWSTRING *ContextName)
{
	unsigned int	i;
	bool			Status = true;		// No errors yet.



// Is the number of dimensions valid?
	if((NumberOfDimensions >= 0) && (NumberOfDimensions <= MAXIMUM_NUMBER_OF_ARRAY_DIMENSIONS))
	{
		if(NumberOfTableEntries >= TableSize)
		{
			TableSize += NUMBER_OF_FREE_ELEMENTS_ON_ITEM_TABLE_REALLOCATION;
			ITEM_TABLE_ENTRY *NewTableEntries = new ITEM_TABLE_ENTRY[TableSize];
			for(i = 0;    i < NumberOfTableEntries;    ++i)
			{
				NewTableEntries[i] = TableEntries[i];
			}
			delete TableEntries;
			TableEntries = NewTableEntries;
		}

		TableEntries[NumberOfTableEntries].Name = new HWSTRING(Name, true);
		TableEntries[NumberOfTableEntries].Type = Type;
		TableEntries[NumberOfTableEntries].Location = Location;
		TableEntries[NumberOfTableEntries].SizeOfEachElement = SizeOfEachElement;
		TableEntries[NumberOfTableEntries].NumberOfDimensions = NumberOfDimensions;
		for(i = 0;    i < ((unsigned int) NumberOfDimensions);    ++i)
		{
			TableEntries[NumberOfTableEntries].SizeOfDimensions[i] = SizeOfDimensions[i];
		}
		TableEntries[NumberOfTableEntries].ContextName = new HWSTRING(ContextName, true);
		++NumberOfTableEntries;
	}

	else
	{
	// Number of dimensions isn't legal; WOOF!
		Status = false;
	}

	return(Status);
}






//
//	SearchByName
//		Search the item table to see if we find a match with the caller specified name.
//		If found, we return the item number, else we return -1.
//
int HHPL_ITEM_TABLE::SearchByName(HWSTRING *Name)
{
	int				ItemNumber = -1;				// Haven't found it yet.
	unsigned int	NextSymbolToBeChecked;			// Index




	NextSymbolToBeChecked = 0;
	
// Keep looking until we find it or we run out of item table entries.
	while((ItemNumber < 0) && (NextSymbolToBeChecked < NumberOfTableEntries))
	{
		if(TableEntries[NextSymbolToBeChecked].Name->Compare(Name, false) == 0)
		{
			ItemNumber = NextSymbolToBeChecked;
		}
		else
		{
			++NextSymbolToBeChecked;
		}
	}


	return(ItemNumber);
} // SearchByName






//
//	SearchByNameAndType
//		Search the item table to see if we find a match with the caller specified name and type.
//		If found, we return the item number, else we return -1.
//
int HHPL_ITEM_TABLE::SearchByNameAndType(HWSTRING *Name, unsigned int Type)
{
	int				ItemNumber = -1;				// Haven't found it yet.
	unsigned int	NextSymbolToBeChecked;			// Index




	NextSymbolToBeChecked = 0;
	
// Keep looking until we find it or we run out of item table entries.
	while((ItemNumber < 0) && (NextSymbolToBeChecked < NumberOfTableEntries))
	{
		if((TableEntries[NextSymbolToBeChecked].Name->Compare(Name, false) == 0) &&
			(TableEntries[NextSymbolToBeChecked].Type & Type))
		{
			ItemNumber = NextSymbolToBeChecked;
		}
		else
		{
			++NextSymbolToBeChecked;
		}
	}


	return(ItemNumber);
} // SearchByNameAndType





//
//	NoDuplicateSymbols
//		Check to make sure that each symbol name is unique.  Note that the following rules apply:
//			1) No routines can share the same name.
//			2) No global may share a name with a routine.
//			3) Any symbol within a routine or main may use either the same or another routine's name.
//			4) Label names must be unique within their context (they cannot be shared with another
//				symbol in the same context, regardless of level).
//			5) All other symbols may be duplicated, even if one is in context at a lower level, but
//				two may not be defined at the same occurrence and nesting levels.
//		The following algorithm is used to determine if there is a conflict when a name match is found:
//
//			If names match
//				If both are routines
//					WOOF
//				else if one is a routine and the other is global in context
//					WOOF
//				else if one is a label and the other is a global
//					WOOF
//				else if one is a label (by default, it's level 1 within it's context) and the other
//										is in the same context
//					WOOF
//				else
//					if contexts match
//						WOOF
//
bool HHPL_ITEM_TABLE::NoDuplicateSymbols(void)
{
	unsigned int	NextSymbolToBeChecked;			// Index
	unsigned int	NextTableEntryToCompare;		// Index
	bool			NoDuplicatesFound = true;		// No errors yet.
	bool			DuplicateFoundInSearchIteration;// Found a duplicate in the current search iteration.
	wchar_t			temp[80];						// Error message string scratch pad


	ErrorMessage->Size = 0;

// Try to resolve all addresses and flag those that don't resolve to the user.
	for(NextSymbolToBeChecked = 0; NextSymbolToBeChecked < NumberOfTableEntries;
											++NextSymbolToBeChecked)
	{
	// Search the table for the next address to resolve.
		NextTableEntryToCompare = NextSymbolToBeChecked + 1;
		DuplicateFoundInSearchIteration = false;
		while((NextTableEntryToCompare < NumberOfTableEntries) && (!DuplicateFoundInSearchIteration))
		{
		// If they match...
			if(TableEntries[NextTableEntryToCompare].Name->Compare
						(TableEntries[NextSymbolToBeChecked].Name, false) == 0)
			{
			// If both are routines, WOOF.
				if((TableEntries[NextTableEntryToCompare].Type & ROUTINE_ITEM) &&
							(TableEntries[NextSymbolToBeChecked].Type & ROUTINE_ITEM))
				{
					DuplicateFoundInSearchIteration = true;
				}

			// If one is a routine and the other is global in context, WOOF.
				else if(((TableEntries[NextTableEntryToCompare].Type & ROUTINE_ITEM) &&
							(TableEntries[NextSymbolToBeChecked].ContextName->CompareSubstring
									(0, GLOBAL_CONTEXT_IDENTIFIER,
									wcslen(GLOBAL_CONTEXT_IDENTIFIER), false) == 0)) ||
					((TableEntries[NextTableEntryToCompare].ContextName->CompareSubstring
									(0, GLOBAL_CONTEXT_IDENTIFIER,
									wcslen(GLOBAL_CONTEXT_IDENTIFIER), false) == 0) &&
							(TableEntries[NextSymbolToBeChecked].Type & ROUTINE_ITEM)))
				{
					DuplicateFoundInSearchIteration = true;
				}

			// If one is a label and the other is global in context, WOOF.
				else if(((TableEntries[NextTableEntryToCompare].Type & LABEL_ITEM) &&
							(TableEntries[NextSymbolToBeChecked].ContextName->CompareSubstring
									(0, GLOBAL_CONTEXT_IDENTIFIER,
									wcslen(GLOBAL_CONTEXT_IDENTIFIER), false) == 0)) ||
					((TableEntries[NextTableEntryToCompare].ContextName->CompareSubstring
									(0, GLOBAL_CONTEXT_IDENTIFIER,
									wcslen(GLOBAL_CONTEXT_IDENTIFIER), false) == 0) &&
							(TableEntries[NextSymbolToBeChecked].Type & LABEL_ITEM)))
				{
					DuplicateFoundInSearchIteration = true;
				}

			// If one is a label and the contexts match, WOOF.
				else if(((TableEntries[NextTableEntryToCompare].Type & LABEL_ITEM) ||
							(TableEntries[NextSymbolToBeChecked].Type & LABEL_ITEM)) &&
					(TableEntries[NextTableEntryToCompare].ContextName->CompareOnShorter
								(0, TableEntries[NextSymbolToBeChecked].ContextName, false) == 0))
				{
					DuplicateFoundInSearchIteration = true;
				}

				else
				{
				// If contexts match...
					if(TableEntries[NextTableEntryToCompare].ContextName->Compare
									(TableEntries[NextSymbolToBeChecked].ContextName, false) == 0)
					{
						DuplicateFoundInSearchIteration = true;
					}

					else
					{
						++NextTableEntryToCompare;
					}
				}
			}
			else
			{
				++NextTableEntryToCompare;
			}
		}

	// We quit because we found two symbol declarations with the same name, WOOF!
		if(DuplicateFoundInSearchIteration)
		{
			NoDuplicatesFound = false;
			swprintf(temp,sizeof(temp), L"Duplicate of %ls\015\012",
								TableEntries[NextTableEntryToCompare].Name->WChar);
			ErrorMessage->Append(temp);
		}
	}


	return(NoDuplicatesFound);
} // NoDuplicateSymbols





// Copy this table to another table.
bool HHPL_ITEM_TABLE::Copy(HHPL_ITEM_TABLE *TableToCopy)
{
	unsigned int	i;
	bool			Status = true;		// No errors yet.



// Zero out this table first.
	Remove();
	Initialize();

// Now, do the copy.
	for(i = 0;    i < TableToCopy->NumberOfTableEntries;    ++i)
	{
		Add(TableToCopy->TableEntries[i].Name,
			TableToCopy->TableEntries[i].Type,
			TableToCopy->TableEntries[i].Location,
			TableToCopy->TableEntries[i].SizeOfEachElement,
			TableToCopy->TableEntries[i].NumberOfDimensions,
			TableToCopy->TableEntries[i].SizeOfDimensions,
			TableToCopy->TableEntries[i].ContextName);
	}

	return(Status);
}





//
//	RemoveByType
//		Remove all items matching the type mask sent by the caller.
//
void HHPL_ITEM_TABLE::RemoveByType(unsigned int TypesToRemove)
{
	unsigned int	i, j;
	unsigned int	NextCopyLocation = 0;



	TypesToRemove &= ITEM_TYPE_MASK;

// Find the entries to be deleted, squeezing them out as we go.
	for(i = 0;    i < NumberOfTableEntries;    ++i)
	{
		if(!(TableEntries[i].Type & TypesToRemove))
		{
		// If we've lost at least one entry, copy the next one.
			if(NextCopyLocation < i)
			{
				TableEntries[NextCopyLocation].Name->Copy(TableEntries[i].Name);
				TableEntries[NextCopyLocation].Type = TableEntries[i].Type;
				TableEntries[NextCopyLocation].Location = TableEntries[i].Location;
				TableEntries[NextCopyLocation].SizeOfEachElement = TableEntries[i].SizeOfEachElement;
				TableEntries[NextCopyLocation].NumberOfDimensions = TableEntries[i].NumberOfDimensions;
				for(j = 0;    j < ((unsigned int) TableEntries[i].NumberOfDimensions);    ++j)
				{
					TableEntries[NextCopyLocation].SizeOfDimensions[j] =
										TableEntries[i].SizeOfDimensions[j];
				}
				TableEntries[NextCopyLocation].ContextName->Copy(TableEntries[i].ContextName);
			}

			++NextCopyLocation;
		}
	}


// Deallocate names for the newly freed items at the end.
	for(i = NextCopyLocation;    i < NumberOfTableEntries;    ++i)
	{
		delete TableEntries[i].Name;
		delete TableEntries[i].ContextName;
	}
	NumberOfTableEntries = NextCopyLocation;


	return;
} // RemoveByType





//
//	RemoveByNonHostType
//		Remove all items matching the type mask sent by the caller that are also not marked as host.
//		Any types in this list will be removed.  Also, any types remaining that are not host
//		accessible will also be removed.
//
void HHPL_ITEM_TABLE::RemoveByNonHostType(unsigned int TypesToRemove)
{
	unsigned int	i, j;
	unsigned int	NextCopyLocation = 0;



	TypesToRemove &= ITEM_TYPE_MASK;

// Find the entries to be deleted, squeezing them out as we go.
	for(i = 0;    i < NumberOfTableEntries;    ++i)
	{
		if((!(TableEntries[i].Type & TypesToRemove)) &&
							(TableEntries[i].Type & HOST_ACCESSIBLE))
		{
		// If we've lost at least one entry, copy the next one.
			if(NextCopyLocation < i)
			{
				TableEntries[NextCopyLocation].Name->Copy(TableEntries[i].Name);
				TableEntries[NextCopyLocation].Type = TableEntries[i].Type;
				TableEntries[NextCopyLocation].Location = TableEntries[i].Location;
				TableEntries[NextCopyLocation].SizeOfEachElement = TableEntries[i].SizeOfEachElement;
				TableEntries[NextCopyLocation].NumberOfDimensions = TableEntries[i].NumberOfDimensions;
				for(j = 0;    j < ((unsigned int) TableEntries[i].NumberOfDimensions);    ++j)
				{
					TableEntries[NextCopyLocation].SizeOfDimensions[j] =
										TableEntries[i].SizeOfDimensions[j];
				}
				TableEntries[NextCopyLocation].ContextName->Copy(TableEntries[i].ContextName);
			}

			++NextCopyLocation;
		}
	}


// Deallocate names for the newly freed items at the end.
	for(i = NextCopyLocation;    i < NumberOfTableEntries;    ++i)
	{
		delete TableEntries[i].Name;
		delete TableEntries[i].ContextName;
	}
	NumberOfTableEntries = NextCopyLocation;


	return;
} // RemoveByNonHostType




//
//	RemoveItem
//		Remove the item specified by the index.
//
void HHPL_ITEM_TABLE::RemoveItem(unsigned int ItemNumberToRemove)
{
	unsigned int	i, j;
	unsigned int	NextCopyLocation = ItemNumberToRemove;



// If the item number isn't valid, return.
	if(ItemNumberToRemove >= NumberOfTableEntries)
	{
		return;
	}


// Find the entries to be deleted, squeezing them out as we go.
	for(i = ItemNumberToRemove;    i < NumberOfTableEntries;    ++i)
	{
	// Shift entries down one item number.
		TableEntries[NextCopyLocation].Name->Copy(TableEntries[i].Name);
		TableEntries[NextCopyLocation].Type = TableEntries[i].Type;
		TableEntries[NextCopyLocation].Location = TableEntries[i].Location;
		TableEntries[NextCopyLocation].SizeOfEachElement = TableEntries[i].SizeOfEachElement;
		TableEntries[NextCopyLocation].NumberOfDimensions = TableEntries[i].NumberOfDimensions;
		for(j = 0;    j < ((unsigned int) TableEntries[i].NumberOfDimensions);    ++j)
		{
			TableEntries[NextCopyLocation].SizeOfDimensions[j] =
								TableEntries[i].SizeOfDimensions[j];
		}
		TableEntries[NextCopyLocation].ContextName->Copy(TableEntries[i].ContextName);
	}


// Deallocate the name in the newly freed item at the end.
	delete TableEntries[NextCopyLocation].Name;
	delete TableEntries[NextCopyLocation].ContextName;
	NumberOfTableEntries = NextCopyLocation;


	return;
} // RemoveItem




