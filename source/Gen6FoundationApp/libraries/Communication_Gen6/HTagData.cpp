//======================================================================================
// HTagData.cpp
//======================================================================================
// $RCSfile: HTagData.cpp $
// $Revision: 1.2 $
// $Date: 2009/04/28 15:21:41EDT $
//======================================================================================

//======================================================================================








//---------------------------------------------------------------------------
//
//  Class Name:     HTAG_DATA
//
//  Class Description:
//		The HTAG class provides a basic mechanism to compress / decompress objects and to tag them with parameters.
//		This class also provides the ability to format those objects for transfer by tagging them in such a way as
//		to minimize version change problems.  Since all fields are tagged, if the receiver does not yet speak the
//		new version of HTAG, it will ignore tags that it doesn't understand.
//
//
//	Notes from application support training:
//		� Started out as a means of wrapping decode data and image data for the SDK, driven by MSGGET and IMGSHP.
//		� Expanded to other types of data for commonality
//		� Provides a mechanism to do the following:
//			A. Offer a means of triggering a receiver to receive data via a 7 byte header
//			B. Tell the receiver exactly how much data is to be sent.
//			C. Specify a protocol for transfer (if any)
//			D. Specify the type of data or action via a 6 character command
//			E. Include option text parameters to control the interpretation / processing of the data stream at the other end
//			F. Optionally compress the data.
//			G. If sent with no protocol, and a file is already encoded in HTAG format, the file can be sent as a byte stream
//				to the receiver with no special handling.
//		� Format is as follows:
//			A. Header consists of <SYN><FE>data stream length<CR>, where data stream length is the four byte little endian
//				length of the data stream that follows this 7 character header.
//			B. 6 character command specifying the type of data.  Examples are IMGSHP for images, APPCOD for application code
//				(not fully implemented yet), and MSGGET for decoded data.
//			C. Protocol parameter (eventually this can appear among other parameters, but must be the first parameter for now)
//			D. Other parameters specific to the command.  Each parameter is either numeric or string, where the data appears
//				first followed by the parameter name.  A string parameter must be in double quotes and use \XX hex format for
//				unprintable characters; if a double quote character is to be part of the string, it must also be specified in
//				hex format.
//			E. Group separator character to delineate the end of the parameter block and the beginning of data.
//			F. Data
//
//---------------------------------------------------------------------------


#include	"HTagData.h"		// HTAG_DATA interface definition

#include	<stdio.h>
#include	<string.h>


//---------------------------------------------------------------------------











//
// Constructor for HTAG_DATA.
//      Indicate that no object has been loaded yet.
//
HTAG_DATA::HTAG_DATA( int SizeOfDataString, bool AutoResize, int InputNumberOfRecords ) :
			HSTRING( SizeOfDataString, AutoResize )
{
	AlwaysReportAllDataItems = false;

// Allocate data item entries to start with.
	NumberOfDataItems = 0;
	MaximumNumberOfDataItems = NUMBER_OF_DATA_ITEMS_TO_ADD_ON_REALLOCATE;
	DataItems = new HTAG_DATA_ITEMS[ MaximumNumberOfDataItems ];

	NumberOfRecords = InputNumberOfRecords;
	RecordOrder = new int[ NumberOfRecords ];
	AlwaysReportAllRecords = true;
	RecordNumberToReport = 0;

	int		i;
	for( i = 0;    i < NumberOfRecords;    ++i )
		RecordOrder[ i ] = i;

    return;
}
//---------------------------------------------------------------------------







//
// Destructor for HTAG_DATA
//      If memory has been allocated to an object, release it.
//
HTAG_DATA::~HTAG_DATA(void)
{
	int		i;



// Delete all entries.
	for(i = 0;    i < NumberOfDataItems;    ++i)
	{
		delete DataItems[ i ].DataNameTag;
		delete DataItems[ i ].Updated;
	}

	delete [] DataItems;

	delete [] RecordOrder;

	return;
}
//---------------------------------------------------------------------------






//
//	ReportAllDataItems
//		Enable / disable reporting of all data items.
//
void HTAG_DATA::ReportAllDataItems( bool Enable )
{
	AlwaysReportAllDataItems = Enable;

    return;
} // ReportAllDataItems







//
//	GetNumberOfRecords
//		Return number of records.
//
int HTAG_DATA::GetNumberOfRecords( void )
{
    return NumberOfRecords;
} // GetNumberOfRecords







//
//	Find
//		Find an item in the sorted (using the index) data item table.  The entry number returned to the caller points to the
//		indexed entry that was found or to the first indexed entry that compares greater than the name sent by the caller.
//
bool HTAG_DATA::Find( const char *Name, int *DataItemNumber )
{
	bool	DataItemWasFound = false;		// Didn't find it yet.


	*DataItemNumber = 0;
	while(( *DataItemNumber < NumberOfDataItems ) && ( !DataItemWasFound ))
	{
		if( DataItems[ *DataItemNumber ].DataNameTag->Compare( Name, false ) == 0 )
		{
			DataItemWasFound = true;
		}
		else
		{
			++(*DataItemNumber);
		}
	}

	return DataItemWasFound;
} // Find





//
//	AddMoreDataItemEntries
//		If we're out of data items, add more.
//
void HTAG_DATA::AddMoreDataItemEntries(void)
{
	int		i;


// Are we out of entries in the parameter table?
	if( NumberOfDataItems >= MaximumNumberOfDataItems )
	{
	// Allocate more space in the parameter table, copy the existing entries, and delete the old table.
		MaximumNumberOfDataItems += NUMBER_OF_DATA_ITEMS_TO_ADD_ON_REALLOCATE;
		HTAG_DATA_ITEMS		*NewDataItems = new HTAG_DATA_ITEMS[ MaximumNumberOfDataItems ];
		for(i = 0;    i < NumberOfDataItems;    ++i)
		{
			NewDataItems[ i ] = DataItems[ i ];
		}

		delete DataItems;
		DataItems = NewDataItems;
	}

	return;
} // AddMoreDataItemEntries





//
//	Add
//		Create a new data item.
//
int HTAG_DATA::Add( const char *NameTag, HSTRINGARRAY *Address, bool AlwaysReport )
{
	int		ExistingItemEntry;			// Unused, but required by Find.
	int		NewItemEntry = -1;			// Haven't found a valid tag entry yet.
	int		i;


// See if the item already exists or if the name contains a space.  If it does, WOOF!
	HSTRING	hstrNameTag( (unsigned char *) NameTag, strlen( NameTag ), false, true, true );
	int		Position;
	if(( !hstrNameTag.Find( (unsigned char *) " ", 1, false, 1, &Position ) ) && ( !Find( NameTag, &ExistingItemEntry ) ))
	{
		AddMoreDataItemEntries();		// Add more entries if needed.

	// Add the new item to the end of the sub-tag table.
		if( NumberOfDataItems < MaximumNumberOfDataItems )
		{
			DataItems[ NumberOfDataItems ].DataNameTag = new HSTRING( NameTag );
			DataItems[ NumberOfDataItems ].Data = Address;
			DataItems[ NumberOfDataItems ].AlwaysReport = AlwaysReport;
			DataItems[ NumberOfDataItems ].Updated = new bool[ NumberOfRecords ];
			for( i = 0;    i < NumberOfRecords;    ++i )
				DataItems[ NumberOfDataItems ].Updated[ i ] = false;
			NewItemEntry = NumberOfDataItems;
			++NumberOfDataItems;
		}
	}

	return NewItemEntry;
} // Add






//
//	Update (overload #1)
//		Update an item.
//
int HTAG_DATA::Update( const char *NameTag, const char *NewValue, int NewValueLength, int RecordNumber )
{
	int		Status = HTAG_DATA_OK;
	int		ItemEntry = -1;			// Haven't found a valid entry yet.


// See if the item exists.  If it doesn't, WOOF!
	if( ( RecordNumber < 0 ) || ( RecordNumber >= NumberOfRecords ))
	{
		Status = HTAG_DATA_RECORD_OUT_OF_BOUNDS;
	}
	else if( Find( NameTag, &ItemEntry ) )
	{
		if( !(DataItems[ ItemEntry ].Data->ElementAt( RecordOrder[ RecordNumber ] )->Copy( NewValue, NewValueLength )) )
		{
			Status = HTAG_DATA_TOO_LONG;
		}
		else
		{
			DataItems[ ItemEntry ].Updated[ RecordOrder[ RecordNumber ] ] = true;
		}
	}
	else
	{
		Status = HTAG_DATA_UNRECOGNIZED_ITEM_NAME;
	}

	return Status;
} // Update (overload #1)







//
//	Update (overload #2)
//		Update an item.
//
int HTAG_DATA::Update( const char *NameTag, HSTRING *NewValue, int RecordNumber )
{
	return Update( NameTag, (char *) NewValue->Char, NewValue->Size, RecordNumber );
} // Update (overload #2)







//
//	DataItemHasBeenUpdated
//		Inform the caller whether or not this data item has been updated.
//
bool HTAG_DATA::DataItemHasBeenUpdated( const char *NameTag, int RecordNumber )
{
	bool	Status = false;			// Assume item hasn't been updated.
	int		ItemEntry = -1;			// Haven't found a valid entry yet.


// See if the item exists.  If it doesn't, WOOF!
	if( ( RecordNumber >= 0 ) && ( RecordNumber < NumberOfRecords ))
	{
		if( Find( NameTag, &ItemEntry ))
		{
			Status = DataItems[ ItemEntry ].Updated[ RecordOrder[ RecordNumber ] ];
		}
	}

	return Status;
} // DataItemHasBeenUpdated






//
//	CopyRecord
//		Copy a record from one HTAG_DATA instance to another.
//
int HTAG_DATA::CopyRecord( HTAG_DATA *HTAG_DATAInstanceToCopyFrom, int RecordNumberToCopyFrom, int RecordNumberToCopyTo )
{
	int		Status = HTAG_DATA_OK;		// Assume success until we know otherwise.
	int		i;


// If we don't have the same number of items, WOOF!
	if( NumberOfDataItems != HTAG_DATAInstanceToCopyFrom->NumberOfDataItems )
	{
	// Number of parameters differs; WOOF!
		Status = HTAG_DATA_COPY_ITEM_COUNT_MISMATCH;
	}
	else if(( RecordNumberToCopyFrom < 0 ) || ( RecordNumberToCopyFrom >= HTAG_DATAInstanceToCopyFrom->GetNumberOfRecords() ) ||
				( RecordNumberToCopyTo < 0 ) || ( RecordNumberToCopyTo >= NumberOfRecords ))
	{
	// Record number is out of range; WOOF.
		Status = HTAG_DATA_RECORD_OUT_OF_BOUNDS;
	}
	else
	{
		for( i = 0;    ( i < NumberOfDataItems ) &&
						((Status == HTAG_DATA_OK) || ( Status == HTAG_DATA_UNRECOGNIZED_ITEM_NAME ) || (Status < 0));    ++i )
		{
		// The name must match before we do the copy.
			if( DataItems[ i ].DataNameTag->Compare( HTAG_DATAInstanceToCopyFrom->DataItems[ i ].DataNameTag, false ) == 0 )
			{
			// Copy value and update flag.
				DataItems[ i ].Data->ElementAt( RecordOrder[ RecordNumberToCopyTo ] )->Copy
						( HTAG_DATAInstanceToCopyFrom->DataItems[ i ].Data->ElementAt
									( HTAG_DATAInstanceToCopyFrom->RecordOrder[ RecordNumberToCopyFrom ] ) );
				DataItems[ i ].Updated[ RecordOrder[ RecordNumberToCopyTo ] ] =
						HTAG_DATAInstanceToCopyFrom->DataItems[ i ].Updated[ HTAG_DATAInstanceToCopyFrom->
										RecordOrder[ RecordNumberToCopyFrom ] ];
			}

			else
			{
			// Names don't match; WOOF!
				Status = HTAG_DATA_COPY_ITEM_NAME_MISMATCH;
			}
		}
	}


    return Status;
} // CopyRecord










//
//	DeleteRecord
//		Delete a record.
//
int HTAG_DATA::DeleteRecord( int RecordNumberToDelete )
{
	int		Status = HTAG_DATA_OK;		// Assume success until we know otherwise.
	int		i;


	if(( RecordNumberToDelete < 0 ) || ( RecordNumberToDelete >= NumberOfRecords ))
	{
	// Record number is out of range; WOOF.
		Status = HTAG_DATA_RECORD_OUT_OF_BOUNDS;
	}
	else
	{
	// Delete the requested record.
		int		RecordOrderPointerToDeletedRecord = RecordOrder[ RecordNumberToDelete ];
		for( i = RecordNumberToDelete;    i < ( NumberOfRecords - 1 );    ++i )
		{
			RecordOrder[ i ] = RecordOrder[ i + 1 ];
		}
		RecordOrder[ NumberOfRecords - 1 ] = RecordOrderPointerToDeletedRecord;

		for( i = 0;    i < NumberOfDataItems;    ++i )
		{
			DataItems[ i ].Data->ElementAt( RecordOrderPointerToDeletedRecord )->Copy( "" );
			DataItems[ i ].Updated[ RecordOrderPointerToDeletedRecord ] = false;
		}
	}


    return Status;
} // DeleteRecord







//
//	InsertRecord
//		Insert a record by copying a record from another HTAG_DATA instance.
//
int HTAG_DATA::InsertRecord( HTAG_DATA *HTAG_DATAInstanceToCopyFrom, int RecordNumberToCopyFrom, int RecordNumberToInsert,
							int DeleteRecordNumber )
{
	int		Status = HTAG_DATA_OK;		// Assume success until we know otherwise.
	int		i;


	if(( RecordNumberToCopyFrom < 0 ) || ( RecordNumberToCopyFrom >= HTAG_DATAInstanceToCopyFrom->GetNumberOfRecords() ) ||
				( RecordNumberToInsert < 0 ) || ( RecordNumberToInsert >= NumberOfRecords ) ||
				(( DeleteRecordNumber != -1 ) &&
						(( DeleteRecordNumber < 0 ) || ( DeleteRecordNumber >= NumberOfRecords ))))
	{
	// Record number is out of range; WOOF.
		Status = HTAG_DATA_RECORD_OUT_OF_BOUNDS;
	}
	else
	{
	// Delete the requested record.
		if( DeleteRecordNumber != -1 )
		{
			Status = DeleteRecord( DeleteRecordNumber );
		}

	// Make space for the new record.
		if((Status == HTAG_DATA_OK) || ( Status == HTAG_DATA_UNRECOGNIZED_ITEM_NAME ) || (Status < 0))
		{
			int		RecordOrderPointerToLastRecord = RecordOrder[ NumberOfRecords - 1 ];
			for( i = NumberOfRecords - 1;    i > RecordNumberToInsert;    --i )
			{
				RecordOrder[ i ] = RecordOrder[ i - 1 ];
			}
			RecordOrder[ RecordNumberToInsert ] = RecordOrderPointerToLastRecord;

		// Copy the record.
			Status = CopyRecord( HTAG_DATAInstanceToCopyFrom, RecordNumberToCopyFrom, RecordNumberToInsert );
		}
	}


    return Status;
} // InsertRecord






//
//	Copy
//		Copy all records from one HTAG_DATA instance to another.
//
int HTAG_DATA::Copy( HTAG_DATA *HTAG_DATAInstanceToCopyFrom )
{
	int		Status = HTAG_DATA_OK;		// Assume success until we know otherwise.
	int		i, j;


// If we don't have the same number of items, WOOF!
	if( NumberOfDataItems == HTAG_DATAInstanceToCopyFrom->NumberOfDataItems )
	{
	// Copy record order.  Do this before we do the record by record copy.
		for( i = 0;    i < NumberOfRecords;    ++i )
			RecordOrder[ i ] = HTAG_DATAInstanceToCopyFrom->RecordOrder[ i ];

		for( i = 0;    ( i < NumberOfDataItems ) &&
						((Status == HTAG_DATA_OK) || ( Status == HTAG_DATA_UNRECOGNIZED_ITEM_NAME ) || (Status < 0));    ++i )
		{
		// The name must match before we do the copy.
			if( DataItems[ i ].DataNameTag->Compare( HTAG_DATAInstanceToCopyFrom->DataItems[ i ].DataNameTag, false ) == 0 )
			{
			// Copy value and update flag.
				for( j = 0;    j < NumberOfRecords;    ++j )
				{
					DataItems[ i ].Data->ElementAt( j )->Copy( HTAG_DATAInstanceToCopyFrom->DataItems[ i ].Data->ElementAt( j ) );
					DataItems[ i ].Updated[ j ] = HTAG_DATAInstanceToCopyFrom->DataItems[ i ].Updated[ j ];
				}
			}

			else
			{
			// Names don't match; WOOF!
				Status = HTAG_DATA_COPY_ITEM_NAME_MISMATCH;
			}
		}
	}
	else
	{
	// Number of parameters differs; WOOF!
		Status = HTAG_DATA_COPY_ITEM_COUNT_MISMATCH;
	}


    return Status;
} // CopyParameters







//
//	CreateTaggedImage
//		Create a tagged image in an empty string sent by the caller.  If TaggedImage is NULL, use the local string.
//
bool HTAG_DATA::CreateTaggedImage( HSTRING *TaggedImage )
{
	bool	ReturnStatus = true;	        // Returns false if we ran out of space; true otherwise
	int		NextDataItem;
	int		NextRecordNumber;
	char	temp[80];



// If TaggedImage is NULL, use the local string.
	if( TaggedImage == 0 )
		TaggedImage = this;

// Start with a clean copy.
	TaggedImage->Copy("");

// Add each item to the list.
	for( NextRecordNumber = 0;    NextRecordNumber < NumberOfRecords;    ++NextRecordNumber )
	{
	// If this isn't the first record, add an <RS>.
		if( NextRecordNumber != 0 )
			TaggedImage->AppendChar( 0x1E );

		for( NextDataItem = 0;    NextDataItem < NumberOfDataItems;    ++NextDataItem )
		{
			if(( DataItems[ NextDataItem ].Updated[ RecordOrder[ NextRecordNumber ] ] ) ||
							( DataItems[ NextDataItem ].AlwaysReport ) || ( AlwaysReportAllDataItems ))
			{
				TaggedImage->Append( DataItems[ NextDataItem ].DataNameTag );
				TaggedImage->Append( " " );
				sprintf( temp, "%d ", DataItems[ NextDataItem ].Data->ElementAt( RecordOrder[ NextRecordNumber ] )->Size );
				TaggedImage->Append( temp );
				TaggedImage->Append( DataItems[ NextDataItem ].Data->ElementAt( RecordOrder[ NextRecordNumber ] ) );
			}
		}
	}

    return ReturnStatus;
} // CreateTaggedImage






//
//	LoadTaggedImage
//		Load the local image with a tagged image contained in the string sent by the caller.  If TaggedImage is NULL, use the
//		local string.
//
int HTAG_DATA::LoadTaggedImage( HSTRING *TaggedImage )
{
// Parse the string.  Each item is parsed by looking for the item name (terminated with a space character), the item length
// in formatted decimal (terminated with a space character), and the actual data.
//
	int		ReturnStatus;			// Status of conversion
	int		NextLocationToRead;     // Pointer to next place in tagged configuration from which to extract a tagged field.
	int		i;
	HSTRING	NameOfItemBeingProcessed(20, true);	// Name of next item removed from the header for processing.
	HSTRING	SizeOfItemBeingProcessed(20, true);	// Size of next item removed from the header for processing.
	int		iSizeOfItemBeingProcessed;
	int		StartPositionOfName;				// Start position of name for next item.
	int		StartPositionOfSize;				// Start position of size for next item.
	int		ItemNumber;							// Item number in DataItems corresponding to the item in TaggedImage that we're
												//	currently processing.
	int		NextRecordNumber;					// Next record number to process.



// If TaggedImage is NULL, use the local string.
	if( TaggedImage == 0 )
		TaggedImage = this;

// So far, indicate that we're OK.
	ReturnStatus = HTAG_DATA_OK;
	NextLocationToRead = 0;


// Indicate that none of the items have been updated yet during this parse.  Zero out the entire data array.
	for( NextRecordNumber = 0;    NextRecordNumber < NumberOfRecords;    ++NextRecordNumber )
	{
		for( i = 0;    i < NumberOfDataItems;    ++i )
		{
			DataItems[ i ].Data->ElementAt( RecordOrder[ NextRecordNumber ] )->Copy( "" );
			DataItems[ i ].Updated[ RecordOrder[ NextRecordNumber ] ] = false;
		}

		RecordOrder[ NextRecordNumber ] = NextRecordNumber;
	}


// Parse through the header.  Start at record 0 and increment every time we hit an <RS> character.
	NextRecordNumber = 0;		
	while(( NextLocationToRead < ((int) TaggedImage->Size )) &&
			(( ReturnStatus == HTAG_DATA_OK ) || ( ReturnStatus == HTAG_DATA_UNRECOGNIZED_ITEM_NAME ) || ( ReturnStatus < 0 )))
	{
	// If next character is <RS>, move to the next record.
		if( TaggedImage->Char[ NextLocationToRead ] == 0x1E )
		{
			++NextLocationToRead;
			++NextRecordNumber;
			if( NextRecordNumber >= NumberOfRecords )
			{
			// We didn't find a copy of this item.
				ReturnStatus = HTAG_DATA_LOAD_IMAGE_TOO_MANY_RECORDS;
			}
		}

		else
		{
		// Find the next space character.  The name must be at least one character in length.
			StartPositionOfName = NextLocationToRead;
			while(( NextLocationToRead < (int) TaggedImage->Size ) && ( TaggedImage->Char[ NextLocationToRead ] != ' ' ))
				++NextLocationToRead;

			if( NextLocationToRead > StartPositionOfName )
			{
				NameOfItemBeingProcessed.Copy( TaggedImage, StartPositionOfName, NextLocationToRead - 1 );

				++NextLocationToRead;		// Move past space.

			//  Look for the length field.
				StartPositionOfSize = NextLocationToRead;
				while(( NextLocationToRead < (int) TaggedImage->Size ) && ( TaggedImage->Char[ NextLocationToRead ] != ' ' ))
					++NextLocationToRead;

				if( NextLocationToRead > StartPositionOfSize )
				{
					SizeOfItemBeingProcessed.Copy( TaggedImage, StartPositionOfSize, NextLocationToRead - 1 );

				// Convert the length field.
					if( SizeOfItemBeingProcessed.RangeCheck( '0', '9' ) )
					{
						int		NumberOfCharactersUsedInConversion;
						iSizeOfItemBeingProcessed = SizeOfItemBeingProcessed.ConvertToNumber( 0,
										&NumberOfCharactersUsedInConversion );
						if( NumberOfCharactersUsedInConversion == (int) SizeOfItemBeingProcessed.Size )
						{
						// The length field is valid.  Move past the space and get the data.
							++NextLocationToRead;		// Move past space.
							if( ( NextLocationToRead + iSizeOfItemBeingProcessed ) <= (int) TaggedImage->Size )
							{
							// See if the name exists.  If yes, copy it.  If not, note that.
								if( Find( (char *) NameOfItemBeingProcessed.Char, &ItemNumber ))
								{
									if( iSizeOfItemBeingProcessed > 0 )
									{
										DataItems[ ItemNumber ].Data->ElementAt( NextRecordNumber )->Copy( TaggedImage,
													NextLocationToRead, NextLocationToRead + iSizeOfItemBeingProcessed - 1 );
									}
									else
									{
										DataItems[ ItemNumber ].Data->ElementAt( NextRecordNumber )->Copy( "" );
									}

									DataItems[ ItemNumber ].Updated[ NextRecordNumber ] = true;
								}
								else
								{
								// We didn't find a copy of this item.
									ReturnStatus = HTAG_DATA_UNRECOGNIZED_ITEM_NAME;
								}

								NextLocationToRead += iSizeOfItemBeingProcessed;
							}

							else
							{
							// Missing data for this item; WOOF.
								ReturnStatus = HTAG_DATA_MISSING_DATA_IN_TAGGED_IMAGE;
							}
						}

						else
						{
						// Non-numeric character; WOOF.
						ReturnStatus = HTAG_DATA_SIZE_FIELD_CONTAINS_NON_NUMERIC_CHARACTER;
						}
					}

					else
					{
					// Non-numeric character; WOOF.
						ReturnStatus = HTAG_DATA_SIZE_FIELD_CONTAINS_NON_NUMERIC_CHARACTER;
					}
				}
				else
				{
				// Name had no characters; WOOF.
					ReturnStatus = HTAG_DATA_ZERO_CHARACTER_SIZE;
				}
			}
			else
			{
			// Name had no characters; WOOF.
				ReturnStatus = HTAG_DATA_ZERO_CHARACTER_NAME;
			}
		}
	}


	return ReturnStatus;
} // LoadTaggedImage







