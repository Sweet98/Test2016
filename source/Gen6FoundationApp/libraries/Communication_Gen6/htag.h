//======================================================================================
// HTag.h
//======================================================================================
// $RCSfile: htag.h $
// $Revision: 1.3 $
// $Date: 2011/11/11 17:01:40EST $
//======================================================================================

//======================================================================================






//---------------------------------------------------------------------------
//
//  Class definitions for HTAG
//
//---------------------------------------------------------------------------
#ifndef HTAG_H
#define HTAG_H
//---------------------------------------------------------------------------

//
//	The HTAG class provides a mechanism to manipulate and compress / decompress objects.  This class also
//	provides the ability to format objects for transfer by tagging them in such a way as to minimize version
//	change problems.  Since all fields are tagged, if the receiver does not yet speak the new version of HTAG,
//	it will ignore tags that it doesn't understand.
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



#include	"HComm.h"
#include	"HCommandHandler.h"
#include	"HString.h"
#include	"HTagData.h"
#include	"HTransferStatus.h"








class HTAG
{
public:
	#define	HTAG_HEADER_STRING_SIZE					7
	#define	HTAG_COMMAND_NAME_SIZE					6




// Status codes
	typedef enum {
		HTAG_OK = 1,
		HTAG_INVALID_FORMAT,
		HTAG_UNRECOGNIZED_PARAMETER_NAME,
		HTAG_MISSING_PARAMETER_NAME,
		HTAG_MISSING_PARAMETER,
		HTAG_MISSING_GROUP_SEPARATOR_AFTER_HEADER,
		HTAG_INVALID_TRANSFER_PROTOCOL,
		HTAG_INVALID_STRING_VALUE,
		HTAG_I_O_ERROR,
		HTAG_NULL_DATA_POINTER,
		HTAG_COPY_PARAMETER_MISMATCH,
		HTAG_PROCESS_FAILURE,
		HTAG_NUMERIC_VALUE_OUT_OF_RANGE,
		HTAG_UPDATE_PARAMETER_TYPE_MISMATCH,
		HTAG_INCOMPLETE_COMMAND_FOUND,
		HTAG_MISSING_SYN_CHARACTER,
		HTAG_MISSING_FE_CHARACTER,
		HTAG_MAXIMUM_COMMAND_LENGTH_EXCEEDED,
		HTAG_MISSING_CR,
		HTAG_COMPLETE_COMMAND_FOUND_WITH_EXTRANEOUS_DATA,
		HTAG_NO_COMMAND_RECEIVED,
		HTAG_MISSING_LENGTH,
		HTAG_MISSING_COMMAND,
		HTAG_UNRECOGNIZED_COMMAND,
		HTAG_MISSING_SYN_FE_HEADER,
		HTAG_USER_DEFINED_STATUS_BEGIN = 10000,
		HTAG_SECOND_USER_DEFINED_STATUS_BEGIN = 20000,
		HTAG_THIRD_USER_DEFINED_STATUS_BEGIN = 30000
	} HTAG_STATUS;



	typedef enum {
		HTAG_NUMERIC_PARAMETER = 0,
		HTAG_STRING_PARAMETER,
		HTAG_END_PARAMETER_TYPES
	} HTAG_PARAMETER_TYPE;




public:
// Reserved parameters.  It is the responsibility of each derived HTAG type class to actually incorporate these
// parameters.
	typedef enum  {
		HTAG_NO_RESPONSE = 0,
		HTAG_RESPONSE_REQUIRED,
		HTAG_END_RESPONSE_VALUE
	} HTAG_RESPONSE_PARAMETER_VALUES;


	typedef enum {
		HTAG_TRANSMIT_NO_PROTOCOL_HEADER_AT_END = 0,
		HTAG_TRANSMIT_DOLPHIN_PROTOCOL,
		HTAG_TRANSMIT_NO_PROTOCOL,
		HTAG_TRANSMIT_HMODEM_COMPRESSED_PROTOCOL,
		HTAG_TRANSMIT_HMODEM_HUFFMAN_COMPRESSED_PROTOCOL = HTAG_TRANSMIT_HMODEM_COMPRESSED_PROTOCOL,
		HTAG_TRANSMIT_HMODEM_UNCOMPRESSED_PROTOCOL,
		HTAG_TRANSMIT_HMODEM_LZ_COMPRESSED_PROTOCOL,
		HTAG_END_TRANSMIT_PROTOCOLS
	} HTAG_TRANSMIT_PROTOCOLS;



public:
// Optional default parameters.
	#define	HTAG_PROTOCOL_PARAMETER_NAME					"P"
	int		TransferProtocol;								// Protocol used for transfer.
	int		TransferProtocolParameterNumber;				// Index of transfer protocol parameter in HTAG

	#define	HTAG_REQUESTED_PROTOCOL_PARAMETER_NAME			"REQP"
	int		RequestedTransferProtocol;						// Requested protocol to be used for transfer.  This should normally
															//	be set to -1 indicating that TransferProtocol is what was
															//	what was requested, but in case the requested protocol is not
															//	supported for some reason, it is stored in this parameter so that
															//	the recipient will know what happened.
	int		RequestedTransferProtocolParameterNumber;		// Index of requested transfer protocol parameter in HTAG

	#define	HTAG_REPORT_ALL_PARAMETER_NAME					"CR"
	int		AlwaysReportAllParameters;						// Indicates whether or not to report all parameters
	int		AlwaysReportAllParametersParameterNumber;		// Index of report all parameter in HTAG

	#define	HTAG_RESPONSE_PARAMETER_NAME					"RSPREQ"
	int		ResponseRequired;								// 1 for response required, 0 otherwise
	int		ResponseRequiredParameterNumber;				// Index of response required parameter in HTAG

	#define	HTAG_PRELIMINARY_RESPONSE_PARAMETER_NAME		"PRSPREQ"
	int		PreliminaryResponseRequired;					// 1 for preliminary response required, 0 otherwise
	int		PreliminaryResponseRequiredParameterNumber;				// Index of preliminary response required parameter in HTAG

	#define	HTAG_DEVICE_ADDRESS_PARAMETER_NAME				"DEVADDR"
	HSTRING	*DeviceAddress;
	int		DeviceAddressParameterNumber;					// Index of device address parameter in HTAG

	#define	HTAG_DEVICE_NAME_PARAMETER_NAME					"DEVNAME"
	HSTRING	*DeviceName;
	int		DeviceNameParameterNumber;						// Index of device name parameter in HTAG



// Return parameter packet command name.
	#define	HTAG_RETURN_PARAMETER_PACKET_COMMAND_NAME		"HTGRSP"


// Menu command associated with this data.
	HSTRING		*CommandName;



public:
// There are two data pointers: one for plain HSTRING data and one for HTAG_DATA.  Only one should be used.
// These point to the data in a derived class and allows the use of CreateTaggedImage, LoadTaggedImage, ReadTaggedImage, and
// WriteTaggedImage functions that need to directly access the data.  This pointer is initialized to NULL and will not be used
// if the derived class hasn't changed it.  Note that the derived class data must either be HSTRING / HTAG_DATA or derived from
// HSTRING / HTAG_DATA in order for this to work.
	HSTRING		*Data;
	HTAG_DATA	*HTagData;



public:
	HCOMM				*Device;
	HTRANSFER_STATUS	*TransferStatus;
	HCOMMAND_HANDLER	*CommandHandler;

public:
	#define	NUMBER_OF_PARAMETERS_TO_ADD_ON_REALLOCATE		10


	typedef struct {
		char				*NameTag;

		int					Type;

		union {
			int				i;
			HSTRING			*s;
		} Default;

		int					LowRange;
		int					HighRange;

		union {
			int				*i;
			HSTRING			*s;
		} Value;

		bool				UseRange;
		bool				AlwaysReport;
		bool				Updated;
	} HTAG_PARAMETERS;


	HTAG_PARAMETERS			*Parameters;
	int						*ParameterIndex;

	int						NumberOfParameters;
	int						MaximumNumberOfParameters;



// Find an item in the sorted (using the index) parameter table.  The entry number returned to the caller points to the indexed entry
// that was found or to the first indexed entry that compares greater than the name sent by the caller.
	bool FindParameter(const char *Name, int *ParameterNumber, int *ParameterOrder);

// If we're out of HTAG parameters, add more.
	void AddMoreParameterTableEntries(void);


public:
// Create a new numeric parameter.   (overloaded version #1 with optional range)
	int AddNumericParameter( const char *NameTag, int *Address, int Default, bool UseRange, int LowRange, int HighRange,
					bool AlwaysReport = false );

// Create a new numeric parameter (overloaded version #2 with mandatory range)
	int AddNumericParameter( const char *NameTag, int *Address, int Default, int LowRange, int HighRange,
					bool AlwaysReport = false );

// Create a new numeric parameter (overloaded version #3 with no range)
	int AddNumericParameter( const char *NameTag, int *Address, int Default, bool AlwaysReport = false );

// Create a new HSTRING parameter.  If the name already exists, WOOF.   (version #1 using a char* default)
	int AddHStringParameter(const char *NameTag, HSTRING *Address, const char *Default, int DefaultSize,
					bool AlwaysReport = false );

// Create a new HSTRING parameter.  If the name already exists, WOOF.   (version #2 using an HSTRING* default)
	int AddHStringParameter(const char *NameTag, HSTRING *Address, HSTRING *Default, bool AlwaysReport = false );


public:
// Update a parameter.  Overload #1 takes a numeric parameter.
	int UpdateParameter( const char *NameTag, int NewValue );

// Update a parameter.  Overload #2 takes a char parameter, with length, that is to be put into an HSTRING parameter.
	int UpdateParameter( const char *NameTag, const char *NewValue, int NewValueLength );

// Update a parameter.  Overload #3 takes an HSTRING parameter.
	int UpdateParameter( const char *NameTag, HSTRING *NewValue );

// Update a parameter.  Overload #4 takes a char parameter that is to be put into an HSTRING parameter.
	int UpdateParameter( const char *NameTag, const char *NewValue );

// Set parameter to it's default value and mark it as not having been updated.
	int SetParameterToDefault( const char *NameTag );

// Inform the caller whether or not this parameter has been updated.
	bool ParameterHasBeenUpdated( const char *NameTag );


protected:
// Allow a derived class to do their own additional validation, post parse processing, and pre-tagged image load processing,
// pre-create tagged image processing, and process each parameter as it is parsed.
	virtual void PerformAdditionalParameterValidation(int * /* Status */)    { return; }
	virtual void PerformAdditionalOperationsAfterParsing(int * /* Status */)    { return; }
	virtual bool PreLoadTaggedImage(HSTRING * /* TaggedImage */, int * /* Status */)    { return(false); }
	virtual bool PostLoadTaggedImage( int * /* Status */ )    { return(false); }
	virtual bool PreCreateTaggedImage(HSTRING * /* TaggedImage */)    { return(false); }

// NOTE:  We will allow PostProcessParameterAfterParsing to process a parameter with a missing value.  Post processing is
//		responsible for clearing this error if the particular parameter doesn't require a parameter.  If not cleared by
//		the post processor, we'll woof!
	virtual bool PostProcessParameterAfterParsing(const char * /* NameTag */, int * /* Status */)    { return(true); }


public:


// Constructor for HTAG; overloaded version #1 for loading images later.
    HTAG( HCOMM *HCommDevice, HTRANSFER_STATUS *InputTransferStatus, HCOMMAND_HANDLER *InputCommandHandler,
		   bool UseResponseParameter = false );

// Destructor for HTAG
    ~HTAG(void);

// Enable / disable reporting of all parameters.
	void ReportAllParameters( bool Enable = true );

// Set image parameters to defaults values.
	void SetParameterDefaults(void);

// Validate new parameters to see if they're in bounds.
	int ValidateParameters(void);

// If the first character in the string we've been sent are a valid asynchronous (<SYN>FE) header, tell that to the
// caller and also tell them the message length.
	bool IsValidSYNFEHeader( HSTRING *Header, int *MessageCommandLength );

// Parse the HTAG header sent by the caller into a parameter list.
	int ParseHeader( HSTRING *Header, int *MessageCommandLength );

// Search for a header at the end of a tagged image, then parse it into a parameter list.
	int FindAndParseTrailingHeader( HSTRING *TaggedImage, int *SizeOfHeader );

// Copy parameters from one HTAG instance to another.
	int CopyParameters( HTAG *HTAGInstanceToCopyFrom );


// Create a tagged image in an empty string sent by the caller, using the data pointed at by the caller.
// Version #1 returns two parts to the tagged image: one containing the non-protocol wrapped portion of
// the tagged image and the other containing the protocol wrapped portion.
	bool CreateTaggedImage( HSTRING *NonProtocolWrappedPartOfTaggedImage, HSTRING *ProtocolWrappedPartOfTaggedImage,
					const unsigned char *Data, int SizeOfData, bool IncludeAsynchronousHeader );


// Create a tagged image in an empty string sent by the caller, using the data pointed at by the caller.
// Version #2 returns a single string containing the tagged image.
	bool CreateTaggedImage( HSTRING *TaggedImage, const unsigned char *Data, int SizeOfData, bool IncludeAsynchronousHeader);


// Create a tagged image in an empty string sent by the caller, using the local data pointer.  Version #3
// returns two parts to the tagged image: one containing the non-protocol wrapped portion of the tagged
// image and the other containing the protocol wrapped portion.
	bool CreateTaggedImage( HSTRING *NonProtocolWrappedPartOfTaggedImage, HSTRING *ProtocolWrappedPartOfTaggedImage,
					bool IncludeAsynchronousHeader );


// Create a tagged image in an empty string sent by the caller, using the local data pointer.  Version #4
// returns a single string containing the tagged image.
	bool CreateTaggedImage( HSTRING *TaggedImage, bool IncludeAsynchronousHeader);


// Load the local image with a tagged image contained in the string sent by the caller.  Version #1 uses data pointed
// at by the caller.
    int LoadTaggedImage( HSTRING *TaggedImage, unsigned char **Data, int *SizeOfData, bool SYNFEHeaderMustBePresent = false );

// Load the local image with a tagged image contained in the string sent by the caller.  Version #1 uses the local
// data pointer.
    int LoadTaggedImage( HSTRING *TaggedImage, bool SYNFEHeaderMustBePresent = false );

// Read data from a Matrix device in HTag format.  Any HTag compatible receives, such as HTagImage and HTagScript,
// can use this routine to actually get the data.
	bool ReadHTagData( HSTRING *HTagData,  char *Result, int NumberOfCharactersToBeReceived,
			HSTRING *CopyOfProtocolParameterString = 0 );

// Write data to a Matrix device in HTag format.  Any HTag compatible transmits, such as HTagImage and HTagScript,
// can use this routine to actually send the data.
	bool WriteHTagData( HSTRING *Command, HSTRING *TaggedImage, char *Result, bool WaitForSecondACK = false );

// Read a tagged image from the device and load it into the local image.
	int ReadTaggedImage(HSTRING *OutputImage, char *Result, int NumberOfCharactersToBeReceived);

// Write a tagged image to the device and load it into the local image.
	int WriteTaggedImage( char *Result, bool WaitForSecondACK = false );

//	Find a <SYN>FE header in the tagged image, and return the offset to the caller.
	int FindHeader( HSTRING *TaggedImage );

// See if the data stream sent by the caller is a <SYN>FE command.  The data stream must be a valid <SYN>FE command up
// to its' end.  We send back various condition codes depending upon how much data was processed.
	static int ValidateDataStreamAsSYNFECommand( const char *DataStream, int SizeOfDataStream,
			int MaximumHTAGCommandSize = HTAG_DEFAULT_MAXIMUM_COMMAND_SIZE );

// See if a <SYN>FE command has been received.
	static int ReceiveHTAGHeader(char *Result, HSTRING *ReceivedData, HSTRING *CommandName, int *CommandLength,
						HCOMM *Device, HSTRING *UnusedCommandData );


private:
// Create a tagged image using a pre-Matrix transfer protocol (protocols 0 or 1).
	bool BuildTaggedImageUsingPreMatrixProtcols( HSTRING *TaggedImage, const unsigned char *Data, int SizeOfData);

// Create a tagged image parameter list, minus the protocol parameter.
	bool BuildParameterList( HSTRING *TaggedImageParameterList );


private:
	HTAG (const HTAG&);            // declared private, but not defined, to prevent copying         
	HTAG& operator=(const HTAG&);  // declared private, but not defined, to prevent copying 

};
//----------------------------------------------------------------------------









//---------------------------------------------------------------------------
//
//  Class definitions for HTAG_ARRAY
//
//---------------------------------------------------------------------------

//
//	The HTAG_ARRAY class provides a mechanism to transfer / store arrays of HTAGS.  Since all fields are tagged, if the
//	receiver does not yet speak the new version of HTAG_ARRAY, it will ignore tags that it doesn't understand.
//



class HTAG_ARRAY : public HTAG
{
public:
	#define	HTAG_ARRAY_DEFAULT_COMMAND_NAME						"HTGARY"

// Status codes
	typedef enum {
		HTAG_ARRAY_BUFFER_ALLOCATION_FAILURE = HTAG::HTAG_USER_DEFINED_STATUS_BEGIN,
		HTAG_ARRAY_ILLEGAL_RECORD_COUNT,
		HTAG_ARRAY_ILLEGAL_CHARACTERS_IN_RECORD_OFFSET_STRING,
		HTAG_ARRAY_TOO_MANY_RECORDS_IN_RECORD_OFFSET_STRING,
		HTAG_ARRAY_NOT_ENOUGH_RECORDS_IN_RECORD_OFFSET_STRING,
		HTAG_ARRAY_RECORD_OFFSET_IS_OUT_OF_SEQUENCE,
		HTAG_ARRAY_RECORD_OFFSET_IS_OUT_OF_BOUNDS,
		HTAG_ARRAY_RECORD_SIZE_MISMATCH,
		HTAG_ARRAY_NO_RECORDS_FOUND,
		HTAG_ARRAY_RECORD_NUMBER_OUT_OF_RANGE
	} HTAG_ARRAY_STATUS;



// Parameters with names.
	#define	HTAG_ARRAY_RECORD_NAME								"RNAM"
	HSTRING		*RecordName;

	#define	HTAG_ARRAY_RECORD_OFFSETS							"ROFF"
	HSTRING		*RecordOffsetString;

	#define	HTAG_ARRAY_NUMBER_OF_RECORDS						"RNUM"
	int			NumberOfRecords;


private:
// Offset and size of each record in Data member, retrieved from RecordOffsetString.
	#define	NUMBER_OF_RECORDS_TO_ADD_ON_REALLOCATE		10
	#define	DEFAULT_MAXIMUM_NUMBER_OF_RECORDS_ALLOWED	1000
	int			MaximumNumberOfRecordsAllowed;
	int			MaximumNumberOfRecordsAllocated;
	int			*RecordOffsets;
	int			*RecordSizes;
	

public:


// Pointer to the actual array data.
	#define	DEFAULT_HTAG_ARRAY_DATA_ALLOCATION					40
	HSTRING		*Data;


private:
// If we're out of record table entries, add more.
	void AddMoreRecordTableEntries( int NewRecordTableEntryCount );

// Validate that tracking variables are in sequence and within the size of the string.
	int ValidateRecordTrackingVariables( void );

// Build record tracking variables from record and size strings.
	int BuildRecordTrackingVariablesFromRecordString( void );

// Build record string from record tracking variables.
	int BuildRecordStringFromRecordTrackingVariables( void );


protected:
// After loading tagged image, rebuild the record tracking variables.
	virtual bool PostLoadTaggedImage( int *Status );


public:
// Constructor for HTAG_ARRAY.
    HTAG_ARRAY(const char *Command = HTAG_ARRAY_DEFAULT_COMMAND_NAME, HCOMM *HCommDevice = 0,
						HTRANSFER_STATUS *TransferStatus = 0, HCOMMAND_HANDLER *CommandHandler = 0);

// Destructor for HTAG_ARRAY
    ~HTAG_ARRAY(void);

// Retrieve a record from the array
	int RetrieveRecord( HSTRING *Record, int RecordNumber = 0 );

// Store a record to the array (default is to store at end).
	int StoreRecord( HSTRING *Record, int RecordNumber = -1 );

// Delete a record from the array
	int RemoveRecord( int RecordNumber );

// Retrieve the number of records in the array
	int NumberOfRecordsInArray( void );

// Set the maximum number of records in the array
	void SetMaximumNumberOfRecords( int NewMaximumNumberOfRecords );

// Retrieve the maximum number of records in the array
	int MaximumNumberOfRecords( void );

private:
	HTAG_ARRAY (const HTAG_ARRAY&);            // declared private, but not defined, to prevent copying         
	HTAG_ARRAY& operator=(const HTAG_ARRAY&);  // declared private, but not defined, to prevent copying 

};
//----------------------------------------------------------------------------





#endif // HTAG_H



