//======================================================================================
// HTagData.h
//======================================================================================
// $RCSfile: HTagData.h $
// $Revision: 1.2 $
// $Date: 2011/11/08 18:02:47EST $
//======================================================================================

//======================================================================================






//---------------------------------------------------------------------------
//
//  Class definitions for HTAG_DATA
//
//---------------------------------------------------------------------------
#ifndef HTAG_DATA_H
#define HTAG_DATA_H
//---------------------------------------------------------------------------

//
//	The HTAG_DATA class provides a mechanism to manage and tag data in a single binary string for storage and
//	transfer.  Each data item must have a unique tag name.  When encoded, each data field consists of a tag name
//	(any character can be used in the name except a space character), a space character, data length (expressed as
//	a formatted decimal number, a space character, and the data.  When an encoded string is decoded, all tag names
//	that are understood will be stored.  Tag names that are not understood will be discarded, and it will be assumed
//	that they are newer.
//


#include	"HStringArray.h"








class HTAG_DATA : public HSTRING
{
public:
// Status codes
	typedef enum {
		HTAG_DATA_OK = 1,
		HTAG_DATA_TOO_LONG,
		HTAG_DATA_UNRECOGNIZED_ITEM_NAME,
		HTAG_DATA_COPY_ITEM_NAME_MISMATCH,
		HTAG_DATA_COPY_ITEM_COUNT_MISMATCH,
		HTAG_DATA_ZERO_CHARACTER_NAME,
		HTAG_DATA_ZERO_CHARACTER_SIZE,
		HTAG_DATA_SIZE_FIELD_CONTAINS_NON_NUMERIC_CHARACTER,
		HTAG_DATA_MISSING_DATA_IN_TAGGED_IMAGE,
		HTAG_DATA_LOAD_IMAGE_TOO_MANY_RECORDS,
		HTAG_DATA_RECORD_OUT_OF_BOUNDS,
		HTAG_DATA_USER_DEFINED_STATUS_BEGIN = 10000,
		HTAG_DATA_SECOND_USER_DEFINED_STATUS_BEGIN = 20000,
		HTAG_DATA_THIRD_USER_DEFINED_STATUS_BEGIN = 30000
	} HTAG_DATA_STATUS;



public:
	#define	NUMBER_OF_DATA_ITEMS_TO_ADD_ON_REALLOCATE		10


	typedef struct {
		HSTRING				*DataNameTag;

		HSTRINGARRAY		*Data;

		bool				AlwaysReport;
		bool				*Updated;
	} HTAG_DATA_ITEMS;


	HTAG_DATA_ITEMS			*DataItems;

	int						NumberOfRecords;
	int						*RecordOrder;

	int						NumberOfDataItems;
	int						MaximumNumberOfDataItems;

	bool					AlwaysReportAllDataItems;

	bool					AlwaysReportAllRecords;
	int						RecordNumberToReport;		// If AlwaysReportAllRecords is true, ignore this number.  Otherwise,
														//	report the record indicated by this number.


// Find an item in the sorted (using the index) data item table.  The entry number returned to the caller points to the indexed entry
// that was found or to the first indexed entry that compares greater than the name sent by the caller.
	bool Find( const char *Name, int *DataItemNumber );

// If we're out of data items, add more.
	void AddMoreDataItemEntries(void);


public:
// Create a new data item.
	int Add( const char *NameTag, HSTRINGARRAY *Address, bool AlwaysReport = false );


public:
// Update an item.
	int Update( const char *NameTag, const char *NewValue, int NewValueLength, int RecordNumber = 0 );
	int Update( const char *NameTag, HSTRING *NewValue, int RecordNumber = 0 );

// Inform the caller whether or not this data item has been updated.
	bool DataItemHasBeenUpdated( const char *NameTag, int RecordNumber );


public:
// Constructor for HTAG_DATA
    HTAG_DATA( int SizeOfDataString = 0, bool AutoResize = false, int InputNumberOfRecords = 1 );

// Destructor for HTAG_DATA
    ~HTAG_DATA( void );

// Enable / disable reporting of all data items.
	void ReportAllDataItems( bool Enable = true );

// Return number of records.
	int GetNumberOfRecords( void );

// Copy a record from one HTAG_DATA instance to another.
	int CopyRecord( HTAG_DATA *HTAG_DATAInstanceToCopyFrom, int RecordNumberToCopyFrom, int RecordNumberToCopyTo );

// Delete a record.
	int DeleteRecord( int RecordNumberToDelete );

// Insert a record by copying a record from another HTAG_DATA instance.
	int InsertRecord( HTAG_DATA *HTAG_DATAInstanceToCopyFrom, int RecordNumberToCopyFrom, int RecordNumberToInsert,
						  int DeleteRecordNumber = -1 );

// Copy all records from one HTAG_DATA instance to another.
	int Copy( HTAG_DATA *HTAG_DATAInstanceToCopyFrom );


// Create a tagged image in an empty string sent by the caller.  If TaggedImage is NULL, use the local string.
	bool CreateTaggedImage( HSTRING *TaggedImage = 0 );


// Load the local image with a tagged image contained in the string sent by the caller.  If TaggedImage is NULL, use the local
// string.
    int LoadTaggedImage( HSTRING *TaggedImage = 0 );
    
private:
	HTAG_DATA (const HTAG_DATA&);            // declared private, but not defined, to prevent copying         
	HTAG_DATA& operator=(const HTAG_DATA&);  // declared private, but not defined, to prevent copying 


};
//----------------------------------------------------------------------------




#endif // HTAG_DATA_H



