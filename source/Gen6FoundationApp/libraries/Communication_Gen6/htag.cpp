//======================================================================================
// HTag.cpp
//======================================================================================
// $RCSfile: htag.cpp $
// $Revision: 1.2 $
// $Date: 2011/09/23 15:07:57EDT $
//======================================================================================

//======================================================================================








//---------------------------------------------------------------------------
//
//  Class Name:     HTAG
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


#include	"HTag.h"			// HTAG interface definition
#include	"Compression.h"		// Compression routine interface
#include	"HModem.h"			// HModem interface
#include	"HHPCommunicationConfig.h"

#include	<stdlib.h>		/* standard library functions				*/
#include	<stdio.h>
#include	<string.h>

//---------------------------------------------------------------------------











//
// Constructor for HTAG; overloaded version #1 for loading images later.
//      Indicate that no object has been loaded yet.
//
//      By default, there is no compression.
//
HTAG::HTAG( HCOMM *HCommDevice, HTRANSFER_STATUS *InputTransferStatus, HCOMMAND_HANDLER *InputCommandHandler,
		   bool UseResponseParameter )
{
	Device = HCommDevice;
	TransferStatus = InputTransferStatus;
	CommandHandler = InputCommandHandler;

	CommandName = new HSTRING( 20, true );

// Allocate 10 parameter table entries to start with.
	NumberOfParameters = 0;
	MaximumNumberOfParameters = NUMBER_OF_PARAMETERS_TO_ADD_ON_REALLOCATE;
	ParameterIndex = new int[MaximumNumberOfParameters];
	Parameters = new HTAG_PARAMETERS[MaximumNumberOfParameters];

// Setup protocol and response parameters, if requested.
	TransferProtocolParameterNumber = AddNumericParameter( HTAG_PROTOCOL_PARAMETER_NAME, &TransferProtocol,
					HTAG::HTAG_TRANSMIT_NO_PROTOCOL, HTAG::HTAG_TRANSMIT_NO_PROTOCOL, HTAG::HTAG_END_TRANSMIT_PROTOCOLS - 1,
					true );

	RequestedTransferProtocolParameterNumber = AddNumericParameter( HTAG_REQUESTED_PROTOCOL_PARAMETER_NAME,
					&RequestedTransferProtocol, -1, false );

	AlwaysReportAllParametersParameterNumber = AddNumericParameter( HTAG_REPORT_ALL_PARAMETER_NAME, &AlwaysReportAllParameters,
					0, 0, 1, false );

	ResponseRequiredParameterNumber = AddNumericParameter( HTAG_RESPONSE_PARAMETER_NAME, &ResponseRequired,
						UseResponseParameter ? 1 : 0, 0, 1, false );

	PreliminaryResponseRequiredParameterNumber = AddNumericParameter( HTAG_PRELIMINARY_RESPONSE_PARAMETER_NAME,
						&PreliminaryResponseRequired, 0, 0, 1, false );

	DeviceAddress = new HSTRING(20, true);
	DeviceAddressParameterNumber = AddHStringParameter( HTAG_DEVICE_ADDRESS_PARAMETER_NAME, DeviceAddress, "", 0, false );

	DeviceName = new HSTRING(20, true);
	DeviceNameParameterNumber = AddHStringParameter( HTAG_DEVICE_NAME_PARAMETER_NAME, DeviceName, "", 0, false );

// Set the Data pointer to NULL; the derived class must update it with a valid pointer.
	Data = NULL;
	HTagData = NULL;


    return;
}
//---------------------------------------------------------------------------







//
// Destructor for HTAG
//      If memory has been allocated to an object, release it.
//
HTAG::~HTAG(void)
{
	int		i;



// Delete all entries in the sub tag table.
	for(i = 0;    i < NumberOfParameters;    ++i)
	{
		delete Parameters[i].NameTag;

	// If this is a string, delete the default value.
		if(Parameters[i].Type == HTAG_STRING_PARAMETER)
		{
			delete Parameters[i].Default.s;
		}
	}

	delete DeviceAddress;
	delete DeviceName;

	delete Parameters;
	delete ParameterIndex;
	delete CommandName;

	return;
}
//---------------------------------------------------------------------------






//
//	Constructor for HTAG; overloaded version #1 for loading images later.
//		Enable / disable reporting of all parameters.
//
void HTAG::ReportAllParameters( bool Enable )
{
	AlwaysReportAllParameters = ( Enable ) ? 1 : 0;

    return;
} // ReportAllParameters







//
//	FindParameter
//		Find an item in the sorted (using the index) parameter table.  The entry number returned to the caller points to the indexed entry
//		that was found or to the first indexed entry that compares greater than the name sent by the caller.
//
bool HTAG::FindParameter(const char *Name, int *ParameterNumber, int *ParameterOrder)
{
	bool	ParameterWasFound = false;		// Didn't find setting yet.


	*ParameterOrder = 0;
	while((*ParameterOrder < NumberOfParameters) &&
			(HSTRING::hhpstricmp(Name, Parameters[ParameterIndex[*ParameterOrder]].NameTag, true) != 0))
	{
		++*ParameterOrder;
	}

// If we didn't move beyond the end of the table and the name sent by the caller matches the name of the entry last processed, then
// indicate to the caller that we found the parameter.
	if((*ParameterOrder < NumberOfParameters) &&
					(HSTRING::hhpstricmp(Name, Parameters[ParameterIndex[*ParameterOrder]].NameTag, true) == 0))
	{
		*ParameterNumber = ParameterIndex[*ParameterOrder];
		ParameterWasFound = true;
	}


	return(ParameterWasFound);
} // FindParameter





//
//	AddMoreParameterTableEntries
//		If we're out of HTAG parameters, add more.
//
void HTAG::AddMoreParameterTableEntries(void)
{
	int		i;


// Are we out of entries in the parameter table?
	if(NumberOfParameters >= MaximumNumberOfParameters)
	{
	// Allocate more space in the parameter table, copy the existing entries, and delete the old table.
		MaximumNumberOfParameters += NUMBER_OF_PARAMETERS_TO_ADD_ON_REALLOCATE;
		int				*NewParameterIndex = new int[MaximumNumberOfParameters];
		HTAG_PARAMETERS	*NewParameters = new HTAG_PARAMETERS[MaximumNumberOfParameters];
		for(i = 0;    i < NumberOfParameters;    ++i)
		{
			NewParameterIndex[i] = ParameterIndex[i];
			NewParameters[i] = Parameters[i];
		}

		delete ParameterIndex;
		delete Parameters;
		ParameterIndex = NewParameterIndex;
		Parameters = NewParameters;
	}

	return;
} // AddMoreParameterTableEntries





//
//	AddNumericParameter (overloaded version #1 with optional range)
//		Create a new numeric parameter.  If the name already exists, WOOF.
//
int HTAG::AddNumericParameter( const char *NameTag, int *Address, int Default, bool UseRange, int LowRange, int HighRange,
											bool AlwaysReport )
{
	int		NewParameterEntry = -1;			// Haven't found a valid tag entry yet.
	int		NewParameterEntryOrder = -1;	// Haven't found a valid tag entry yet.
	int		i;


// See if the parameter already exists.  If it does, WOOF!
	if((LowRange <= HighRange) &&
			(!FindParameter(NameTag, &NewParameterEntry, &NewParameterEntryOrder)))
	{
		AddMoreParameterTableEntries();		// Add more entries if needed.

	// Add the new parameter to the end of the sub-tag table.
		if(NumberOfParameters < MaximumNumberOfParameters)
		{
			Parameters[NumberOfParameters].NameTag = new char[strlen(NameTag) + 1];
			strcpy(Parameters[NumberOfParameters].NameTag, NameTag);
			Parameters[NumberOfParameters].Value.i = Address;
			Parameters[NumberOfParameters].Default.i = Default;
			*(Parameters[NumberOfParameters].Value.i) = Parameters[NumberOfParameters].Default.i;
			Parameters[NumberOfParameters].LowRange = LowRange;
			Parameters[NumberOfParameters].HighRange = HighRange;
			Parameters[NumberOfParameters].Type = HTAG_NUMERIC_PARAMETER;
			Parameters[NumberOfParameters].UseRange = UseRange;
			if( Parameters[NumberOfParameters].UseRange )
			{
				if(*(Parameters[NumberOfParameters].Value.i) < LowRange)  *(Parameters[NumberOfParameters].Value.i) = LowRange;
				if(*(Parameters[NumberOfParameters].Value.i) > HighRange)  *(Parameters[NumberOfParameters].Value.i) = HighRange;
			}
			Parameters[NumberOfParameters].AlwaysReport = AlwaysReport;
			Parameters[NumberOfParameters].Updated = false;
			++NumberOfParameters;
		}


	// NewParameterEntryOrder already points to the insertion spot in the index, so we just have to move the index values up by one and
	// set the new entry.
		for(i = NumberOfParameters - 1;    i > NewParameterEntryOrder;    --i)
		{
			ParameterIndex[i] = ParameterIndex[i-1];
		}
		ParameterIndex[NewParameterEntryOrder] = NumberOfParameters - 1;
	}

	return((NewParameterEntryOrder >= 0) ? ParameterIndex[NewParameterEntryOrder] : NewParameterEntryOrder);
} // AddParameter (overloaded version #1)





//
//	AddNumericParameter (overloaded version #2 with mandatory range)
//		Create a new numeric parameter.  If the name already exists, WOOF.
//
int HTAG::AddNumericParameter( const char *NameTag, int *Address, int Default, int LowRange, int HighRange, bool AlwaysReport )
{
	return AddNumericParameter( NameTag, Address, Default, true, LowRange, HighRange, AlwaysReport );
} // AddParameter (overloaded version #2)





//
//	AddNumericParameter (overloaded version #3 with no range)
//		Create a new numeric parameter.  If the name already exists, WOOF.
//
int HTAG::AddNumericParameter( const char *NameTag, int *Address, int Default, bool AlwaysReport )
{
	return AddNumericParameter( NameTag, Address, Default, false, 0, 0, AlwaysReport );
} // AddParameter (overloaded version #3)





//
//	AddHStringParameter (version #1 using a char* default)
//		Create a new HSTRING parameter.  If the name already exists, WOOF.
//
int HTAG::AddHStringParameter(const char *NameTag, HSTRING *Address, const char *Default, int DefaultSize, bool AlwaysReport)
{
	int		NewParameterEntry = -1;			// Haven't found a valid tag entry yet.
	int		NewParameterEntryOrder = -1;	// Haven't found a valid tag entry yet.
	int		i;


// See if the parameter already exists.  If it does, WOOF!
	if(!FindParameter(NameTag, &NewParameterEntry, &NewParameterEntryOrder))
	{
		AddMoreParameterTableEntries();		// Add more entries if needed.

	// Add the new parameter to the end of the sub-tag table.
		if(NumberOfParameters < MaximumNumberOfParameters)
		{
			Parameters[NumberOfParameters].NameTag = new char[strlen(NameTag) + 1];
			strcpy(Parameters[NumberOfParameters].NameTag, NameTag);
			Parameters[NumberOfParameters].Value.s = Address;
			Parameters[NumberOfParameters].Default.s = new HSTRING((unsigned char *) Default, DefaultSize, true);
			Parameters[NumberOfParameters].Value.s->Copy(Parameters[NumberOfParameters].Default.s);
			Parameters[NumberOfParameters].Type = HTAG_STRING_PARAMETER;
			Parameters[NumberOfParameters].AlwaysReport = AlwaysReport;
			Parameters[NumberOfParameters].Updated = false;
			++NumberOfParameters;
		}


	// NewParameterEntryOrder already points to the insertion spot in the index, so we just have to move the index values up by one and
	// set the new entry.
		for(i = NumberOfParameters - 1;    i > NewParameterEntryOrder;    --i)
		{
			ParameterIndex[i] = ParameterIndex[i-1];
		}
		ParameterIndex[NewParameterEntryOrder] = NumberOfParameters - 1;
	}

	return((NewParameterEntryOrder >= 0) ? ParameterIndex[NewParameterEntryOrder] : NewParameterEntryOrder);
} // AddHStringParameter (version #1)







//
//	AddHStringParameter (version #2 using an HSTRING* default)
//		Create a new HSTRING parameter.  If the name already exists, WOOF.
//
int HTAG::AddHStringParameter(const char *NameTag, HSTRING *Address, HSTRING *Default, bool AlwaysReport)
{
	return(AddHStringParameter(NameTag, Address, (char *) Default->Char, Default->Size, AlwaysReport ));
} // AddHStringParameter (version #2)






//
//	UpdateParameter (Overload #1)
//		Update a numeric parameter.
//		If the name doesn't exist, or the parameter is out of range, WOOF.
//
int HTAG::UpdateParameter( const char *NameTag, int NewValue )
{
	int		Status = HTAG_OK;
	int		ParameterEntry = -1;		// Haven't found a valid entry yet.
	int		ParameterEntryOrder = -1;	// Haven't found a valid entry yet.


// See if the parameter exists.  If it doesn't, WOOF!
	if( FindParameter( NameTag, &ParameterEntry, &ParameterEntryOrder ))
	{
	// See if the parameter is numeric.  If it isn't, WOOF!
		if( Parameters[ ParameterEntry ].Type == HTAG_NUMERIC_PARAMETER )
		{
		// See if the parameter is in range.  If it isn't, WOOF!
			if(( !Parameters[ ParameterEntry ].UseRange ) ||
					(( NewValue >= Parameters[ ParameterEntry ].LowRange ) &&
										( NewValue <= Parameters[ ParameterEntry ].HighRange )))
			{
				*( Parameters[ ParameterEntry ].Value.i ) = NewValue;
				Parameters[ ParameterEntry ].Updated = true;
			}
			else
			{
				Status = HTAG_NUMERIC_VALUE_OUT_OF_RANGE;
			}
		}
		else
		{
			Status = HTAG_UPDATE_PARAMETER_TYPE_MISMATCH;
		}
	}
	else
	{
		Status = HTAG_UNRECOGNIZED_PARAMETER_NAME;
	}

	return Status;
} // UpdateParameter (Overload #1)






//
//	UpdateParameter (Overload #2)
//		Update an HSTRING parameter using a char parameter, with length.
//		If the name doesn't exist, WOOF.
//
int HTAG::UpdateParameter( const char *NameTag, const char *NewValue, int NewValueLength )
{
	int		Status = HTAG_OK;
	int		ParameterEntry = -1;		// Haven't found a valid entry yet.
	int		ParameterEntryOrder = -1;	// Haven't found a valid entry yet.


// See if the parameter exists.  If it doesn't, WOOF!
	if( FindParameter( NameTag, &ParameterEntry, &ParameterEntryOrder ))
	{
	// See if the parameter is a string.  If it isn't, WOOF!
		if( Parameters[ ParameterEntry ].Type == HTAG_STRING_PARAMETER )
		{
			if( !Parameters[ ParameterEntry ].Value.s->Copy( NewValue, NewValueLength ))
			{
				Status = HTAG_INVALID_STRING_VALUE;
			}
			else
			{
				Parameters[ ParameterEntry ].Updated = true;
			}
		}
		else
		{
			Status = HTAG_UPDATE_PARAMETER_TYPE_MISMATCH;
		}
	}
	else
	{
		Status = HTAG_UNRECOGNIZED_PARAMETER_NAME;
	}

	return Status;
} // UpdateParameter (Overload #2)







//
//	UpdateParameter (Overload #3)
//		Update an HSTRING parameter using an HSTRING input parameter.
//		If the name doesn't exist, WOOF.
//
int HTAG::UpdateParameter( const char *NameTag, HSTRING *NewValue )
{
	return UpdateParameter( NameTag, (char *) NewValue->Char, NewValue->Size );
} // UpdateParameter (Overload #3)






//
//	UpdateParameter (Overload #4)
//		Update an HSTRING parameter using a char parameter.
//		If the name doesn't exist, WOOF.
//
int HTAG::UpdateParameter( const char *NameTag, const char *NewValue )
{
	return UpdateParameter( NameTag, NewValue, strlen( NewValue ) );
} // UpdateParameter (Overload #4)






//
//	SetParameterToDefault
//		Set parameter to it's default value and mark it as not having been updated.
//
int HTAG::SetParameterToDefault( const char *NameTag )
{
	int		Status = HTAG_OK;
	int		ParameterEntry = -1;		// Haven't found a valid entry yet.
	int		ParameterEntryOrder = -1;	// Haven't found a valid entry yet.


// See if the parameter exists.  If it doesn't, WOOF!
	if( FindParameter( NameTag, &ParameterEntry, &ParameterEntryOrder ))
	{
	// See if the parameter is numeric.  If it isn't, WOOF!
		if( Parameters[ ParameterEntry ].Type == HTAG_NUMERIC_PARAMETER )
		{
			*( Parameters[ ParameterEntry ].Value.i ) = Parameters[ ParameterEntry ].Default.i;
		}
		else
		{
			Parameters[ ParameterEntry ].Value.s->Copy( Parameters[ ParameterEntry ].Default.s );
		}

		Parameters[ ParameterEntry ].Updated = false;
	}
	else
	{
		Status = HTAG_UNRECOGNIZED_PARAMETER_NAME;
	}

	return Status;
} // SetParameterToDefault






//
//	ParameterHasBeenUpdated
//		Inform the caller whether or not this parameter has been updated.
//
bool HTAG::ParameterHasBeenUpdated( const char *NameTag )
{
	bool	Status = false;				// Assume parameter hasn't been updated.
	int		ParameterEntry = -1;		// Haven't found a valid entry yet.
	int		ParameterEntryOrder = -1;	// Haven't found a valid entry yet.


// See if the parameter exists.  If it doesn't, WOOF!
	if( FindParameter( NameTag, &ParameterEntry, &ParameterEntryOrder ))
	{
		Status = Parameters[ ParameterEntry ].Updated;
	}

	return Status;
} // ParameterHasBeenUpdated







//
//	SetParameterDefaults
//		Set parameters to defaults values.
//
void HTAG::SetParameterDefaults(void)
{
	int		i;


// Let's set the defaults.
	for(i = 0;    i < NumberOfParameters;    ++i)
	{
		Parameters[ i ].Updated = false;

	// Set based on type.
		if(Parameters[i].Type == HTAG_NUMERIC_PARAMETER)
		{
			*(Parameters[i].Value.i) = Parameters[i].Default.i;
		}
		else
		{
			Parameters[i].Value.s->Copy(Parameters[i].Default.s);
		}
	}


	return;
} // SetParameterDefaults






//
//	ValidateParameters
//		Validate new parameters to see if they're in bounds.
//		Return 1 if all parameters are valid, or the negated parameter number of an invalid parameter.
//
//
int HTAG::ValidateParameters(void)
{
	int		i;
	int		Status = HTAG_OK;		// No failures yet.



// Let's validate the input #s.
	for(i = 0;    (i < NumberOfParameters) && (Status == HTAG_OK);    ++i)
	{
	// Is it numeric?
		if(Parameters[i].Type == HTAG_NUMERIC_PARAMETER)
		{
			if(( Parameters[ i ].UseRange ) &&
						((*(Parameters[i].Value.i) < Parameters[i].LowRange) ||
											(*(Parameters[i].Value.i) > Parameters[i].HighRange)))
			{
				Status = -i;
			}
		}
	}

// Give a derived class the chance to do additional validation.
	if(Status == HTAG_OK)
	{
		PerformAdditionalParameterValidation(&Status);
	}


    return(Status);
} // ValidateParameters





//
//	IsValidSYNFEHeader
//		If the first character in the string we've been sent are a valid asynchronous (<SYN>FE) header, tell that to the
//		caller and also tell them the message length.
//
bool HTAG::IsValidSYNFEHeader( HSTRING *Header, int *MessageCommandLength )
{
	bool	Status = false;			// Not validated yet.


// Is there a SYN<FE> asynchronous header?
	*MessageCommandLength = -1;
	if(( Header->Size >= HTAG_HEADER_STRING_SIZE ) && ( Header->Char[ 0 ] == 0x02 ) && ( Header->Char[ 1 ] == 0xFE ) &&
				( Header->Char[ 6 ] == 0x0D ))
	{
		Header->CopyFromString( 2, 4, ((unsigned int *) MessageCommandLength));
		Status = true;
	}


	return Status;
} // IsValidSYNFEHeader






//
//	ParseHeader
//		Parse the HTAG header sent by the caller into a parameter list.
//		Note that the header may contain the asynchronous (<SYN>FE) header and the command that followed.  We'll look
//		for that at the beginning of the header and remove it if we find it.  Return HTAG_OK if all parameters are
//		valid, or the negated parameter number of an invalid parameter.
//
//		Each parameter is parsed by looking for either a number or a string (a string will start with a double quote),
//		then looking for the parameter name, finding the parameter name in our table, and seeing if the named parameter
//		matches the parameter type we just parsed.  Finally, we'll actually store the value.
//
int HTAG::ParseHeader( HSTRING *Header, int *MessageCommandLength )
{
	int		ReturnStatus=HTAG_OK;			// Status of conversion
	int		NextLocationToRead=0;     // Pointer to next place in tagged configuration from which to extract a tagged field.
	int		Number=0;                 // Extracted numeric value of a field.
	int		NumberOfCharactersUsedInConversion;
	//int		i;
	HSTRING	NameOfFormatBeingProcessed(20, true);	// Name of next format removed from the header for processing.
	int		NextParameterType;		// Parameter type currently being parsed.
	int		StartOfStringParameter=0;	// If parameter being parsed is a string, mark the start and end locations.
	int		NumberOfCharactersInStringParameter=0;




// Is there a message command to remove?  If so, return the length, else return -1.
	if( IsValidSYNFEHeader( Header, MessageCommandLength ))
	{
		NextLocationToRead += HTAG_HEADER_STRING_SIZE;
	}


// Is there a command to remove?
	if( ((Header->Size + NextLocationToRead) >= CommandName->Size ) &&
				(Header->CompareOnShorter( NextLocationToRead, CommandName, false ) == 0))
	{
		NextLocationToRead += CommandName->Size;
	}


// Indicate that none of the parameters have been updated yet during this parse.
	for( int i = 0;    i < NumberOfParameters;    ++i )
	{
		Parameters[ i ].Updated = false;
	}


// Parse through the header.
	while((NextLocationToRead < ((int) Header->Size)) &&
					((ReturnStatus == HTAG_OK) || (ReturnStatus == HTAG_UNRECOGNIZED_PARAMETER_NAME) || (ReturnStatus < 0)))
	{
	// Are we getting a number or a string?
		if(Header->Char[NextLocationToRead] == '"')
		{
		// We have a string delimiter (double quote), so we're looking for a string.  Find the trailing delimiter,
		// then store the result appropriately.
			NextParameterType = HTAG_STRING_PARAMETER;
			++NextLocationToRead;
			StartOfStringParameter = NextLocationToRead;
			NumberOfCharactersInStringParameter = 0;

			while((NextLocationToRead < ((int) Header->Size)) && (Header->Char[NextLocationToRead] != '"') &&
					((ReturnStatus == HTAG_OK) || (ReturnStatus == HTAG_UNRECOGNIZED_PARAMETER_NAME) || (ReturnStatus < 0)))
			{
				++NextLocationToRead;
				++NumberOfCharactersInStringParameter;
			}

		// If we quit because we ran out of characters, WOOF.
			if(NextLocationToRead >= ((int) Header->Size))
			{
				ReturnStatus = HTAG_INVALID_STRING_VALUE;
			}
			else
			{
			// Move to the parameter name.
				++NextLocationToRead;
			}
		}

		else
		{
		// Get the next number.
			NextParameterType = HTAG_NUMERIC_PARAMETER;

			Number = ((int) Header->ConvertToNumber(NextLocationToRead, &NumberOfCharactersUsedInConversion));
			if(NumberOfCharactersUsedInConversion < 1)
			{
			// Parameter must have at least one digit; WOOF.
				ReturnStatus = HTAG_MISSING_PARAMETER;
			}

			else
			{
			// Parameter is OK.  Move to the parameter name.
				NextLocationToRead += NumberOfCharactersUsedInConversion;
			}
		}


	// If we've succeeded so far, move to the next format.
		if((ReturnStatus == HTAG_OK) || (ReturnStatus == HTAG_UNRECOGNIZED_PARAMETER_NAME) || (ReturnStatus < 0)
													|| (ReturnStatus == HTAG_MISSING_PARAMETER))
		{
		// Remove the parameter name which is terminated by a numeric character, group separator, double quote, or end
		// of string.
			NameOfFormatBeingProcessed.Copy("");
			while((NextLocationToRead < ((int) Header->Size)) &&
							((Header->Char[NextLocationToRead] < '0') || (Header->Char[NextLocationToRead] > '9')) &&
							(Header->Char[NextLocationToRead] != '-') &&
							(Header->Char[NextLocationToRead] != '"') &&
							(Header->Char[NextLocationToRead] != 0x1D))
			{
				NameOfFormatBeingProcessed.AppendChar((int) Header->Char[NextLocationToRead]);
				++NextLocationToRead;
			}

		// If we have a format, compare it.
			if(NameOfFormatBeingProcessed.Size > 0)
			{
				int i = 0;
				while((i < NumberOfParameters) && (NameOfFormatBeingProcessed.Compare(Parameters[i].NameTag, false) != 0))
				{
					++i;
				}

			// If we didn't find an entry, indicate unrecognized format character.  Otherwise, validate and store the value.
				if(i < NumberOfParameters)
				{
				// Indicate that the parameter has been updated during this parse.
					Parameters[ i ].Updated = true;

				// If there was no parameter value, skip this step.
					if(ReturnStatus != HTAG_MISSING_PARAMETER)
					{
					// If parameter types don't match, WOOF!
						if(NextParameterType != Parameters[i].Type)
						{
							ReturnStatus = -i;
						}

					// **** We may eventually want to save parameters off to the side until we know what they are.
						else if(NextParameterType == HTAG_NUMERIC_PARAMETER)
						{
						// We're storing a number.
							if(( !Parameters[i].UseRange ) ||
										((Number >= Parameters[i].LowRange) && (Number <= Parameters[i].HighRange)))
							{
								*(Parameters[i].Value.i) = Number;
							}

							else
							{
								ReturnStatus = -i;
							}
						}

						else
						{
						// We're storing a string.  Copy it and convert HEX formats.
							Parameters[i].Value.s->Copy(&Header->Char[StartOfStringParameter], NumberOfCharactersInStringParameter);
							Parameters[i].Value.s->UnprintableFormatsToCharacters();
						}
					}

				// If we made it this far with no errors, allow a derived class to post-process the result.
				// NOTE:  We allow a missing parameter up to this point.  Post processing is responsible for
				//		clearing this error if the particular parameter doesn't require a parameter.  If not
				//		cleared by the post processor, we'll woof!
					if((ReturnStatus == HTAG_OK) || (ReturnStatus == HTAG_UNRECOGNIZED_PARAMETER_NAME) || (ReturnStatus < 0) ||
										(ReturnStatus == HTAG_MISSING_PARAMETER))
					{
						PostProcessParameterAfterParsing(Parameters[i].NameTag, &ReturnStatus);
					}
				}

				else
				{
				// Parameter name doesn't match anything in the table; WOOF!
					ReturnStatus = HTAG_UNRECOGNIZED_PARAMETER_NAME;
				}
			}

			else
			{
			// We ran out of header before finding the image format character; WOOF.
				ReturnStatus = HTAG_MISSING_PARAMETER_NAME;
			}
		}
	}


// If no errors have occurred yet, give a derived class the chance to do additional validation.
	if((ReturnStatus == HTAG_OK) || (ReturnStatus == HTAG_UNRECOGNIZED_PARAMETER_NAME) || (ReturnStatus < 0))
	{
		PerformAdditionalParameterValidation(&ReturnStatus);
	}


// If no errors have occurred yet, give a derived class the chance to do additional operations.
	if((ReturnStatus == HTAG_OK) || (ReturnStatus == HTAG_UNRECOGNIZED_PARAMETER_NAME) || (ReturnStatus < 0))
	{
		PerformAdditionalOperationsAfterParsing(&ReturnStatus);
	}


	return(ReturnStatus);
} // ParseHeader





//
//	FindAndParseTrailingHeader
//		Search for an image ship header at the end of a tagged image, then parse it into a parameter list.
//		Return HTAG_OK if all parameters are valid, or the negated parameter number of an invalid parameter.
//
int HTAG::FindAndParseTrailingHeader(HSTRING *TaggedImage, int *SizeOfHeader )
{
	int		ReturnStatus = HTAG_OK;               // No errors yet.
	int		PositionWhereHeaderWasFound;
	int		MessageCommandLength;		// If a message command is found at the beginning of the tagged image, this is set to
										//	the length in the header.


	if(( CommandName->Size ) &&
		(TaggedImage->Find( CommandName, false, -1, &PositionWhereHeaderWasFound)))
	{
	// Copy the header.
		HSTRING		Header(TaggedImage->Size - PositionWhereHeaderWasFound, true);
		Header.Copy(&(TaggedImage->Char[PositionWhereHeaderWasFound]), TaggedImage->Size - PositionWhereHeaderWasFound);
		*SizeOfHeader = Header.Size;

	// Remove the table character first.
		if((Header.Char[Header.Size - 1] == '.') ||
				(Header.Char[Header.Size - 1] == '!') ||
				(Header.Char[Header.Size - 1] == '@') ||
				(Header.Char[Header.Size - 1] == '&'))
		{
			Header.Remove(Header.Size - 1, 1);

		// Process the status character next.
			if(Header.Char[Header.Size - 1] == ACK)
			{
				Header.Remove(Header.Size - 1, 1);
			}
			else if((Header.Char[Header.Size - 1] == NAK) ||
						(Header.Char[Header.Size - 1] == ENQ))
			{
				ReturnStatus = HTAG_INVALID_FORMAT;
			}
		}

	// Now we'll process the header if no errors have occurred.
		if(ReturnStatus == HTAG_OK)
		{
			ReturnStatus = ParseHeader( &Header, &MessageCommandLength );
		}
	}

	return(ReturnStatus);
} // FindAndParseTrailingHeader








//
//	CopyParameters
//		Copy parameters from one HTAG instance to another.
//
//
int HTAG::CopyParameters( HTAG *HTAGInstanceToCopyFrom )
{
	int		i;


// If the input parameters are valid, let's do the copy.
	int		Status = HTAGInstanceToCopyFrom->ValidateParameters();

	if((Status == HTAG_OK) || (Status == HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
	{
	// If we don't have the same number of parameters, WOOF!
		if( NumberOfParameters == HTAGInstanceToCopyFrom->NumberOfParameters )
		{
			for( i = 0;    ( i < NumberOfParameters ) &&
						((Status == HTAG_OK) || (Status == HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0));    ++i )
			{
			// The type, name, and range (if applicable) must match before we do the copy.
				if( ( Parameters[ i ].Type == HTAGInstanceToCopyFrom->Parameters[ i ].Type ) &&
					( strcmp( Parameters[ i ].NameTag, HTAGInstanceToCopyFrom->Parameters[ i ].NameTag ) == 0 ) &&
					(( Parameters[ i ].Type != HTAG_NUMERIC_PARAMETER ) || 
						( !Parameters[i].UseRange ) ||
						(( Parameters[ i ].LowRange == HTAGInstanceToCopyFrom->Parameters[ i ].LowRange ) &&
							( Parameters[ i ].HighRange == HTAGInstanceToCopyFrom->Parameters[ i ].HighRange ))))
				{
				// Copy value and update flag.
					switch( Parameters[ i ].Type )
					{
						case HTAG_STRING_PARAMETER:
							Parameters[ i ].Value.s->Copy( HTAGInstanceToCopyFrom->Parameters[ i ].Value.s );
							break;

						case HTAG_NUMERIC_PARAMETER:
						default:
							*(Parameters[ i ].Value.i) = *(HTAGInstanceToCopyFrom->Parameters[ i ].Value.i);
							break;
					}

					Parameters[ i ].Updated = HTAGInstanceToCopyFrom->Parameters[ i ].Updated;
				}

				else
				{
				// Parameters aren't a match; WOOF!
					Status = HTAG_COPY_PARAMETER_MISMATCH;
				}
			}
		}
		else
		{
		// Number of parameters differs; WOOF!
			Status = HTAG_COPY_PARAMETER_MISMATCH;
		}
	}


    return Status;
} // CopyParameters





//
//	BuildTaggedImageUsingPreMatrixProtcols
//		Create a tagged image using a pre-Matrix transfer protocol (protocols 0 or 1).
//
bool HTAG::BuildTaggedImageUsingPreMatrixProtcols( HSTRING *TaggedImage, const unsigned char *Data, int SizeOfData)
{
	int		i;
	bool	ReturnStatus = true;	        // Returns false if we ran out of space; true otherwise



// We don't know how to build the old style command string, so we ask the derived class to do it for us.
	ReturnStatus = PreCreateTaggedImage( TaggedImage );

// Copy the data.  The header is already in place.
	if(ReturnStatus)
	{
	// If the protocol is Dolphin, convert ESC and EOT characters.
		if(*(Parameters[ TransferProtocolParameterNumber ].Value.i) == HTAG_TRANSMIT_DOLPHIN_PROTOCOL)
		{
		// Put an ESC in front of all ESC and EOT characters, then tack an EOT on the end.
			for(i = 0;    (i < SizeOfData) && (ReturnStatus);    ++i)
			{
				if((Data[i] == ESC) || (Data[i] == EOT))
				{
					ReturnStatus = TaggedImage->AppendChar(ESC);
				}
				ReturnStatus = TaggedImage->AppendChar(((int) (Data[i] & 0xFF)));
			}
			ReturnStatus = TaggedImage->AppendChar(EOT);
		}

	// The protocol is not Dolphin, so we'll just tack on the data to the end.
		else
		{
			ReturnStatus = TaggedImage->Append((unsigned char *) Data, SizeOfData);
		}
	}


    return (ReturnStatus);
} // BuildTaggedImageUsingPreMatrixProtcols







//
//	BuildParameterList
//		Create a tagged image parameter list, minus the protocol parameter.  The protocol parameter must be placed before this
//		list by the caller so that a receiver can identify the protocol and react accordingly.
//
//
bool HTAG::BuildParameterList( HSTRING *TaggedImageParameterList )
{
	int		i;
	bool	ReturnStatus = true;	        // Returns false if we ran out of space; true otherwise
	char	temp[80];



	for(i = 0;    (i < NumberOfParameters) && (ReturnStatus);    ++i)
	{
	// Skip if this is the protocol parameter; we do that later on.  Also skip if this parameter is not supposed to be
	// reported.
		if(( i != TransferProtocolParameterNumber ) &&
				(( Parameters[ ParameterIndex[ i ]].AlwaysReport ) ||
					( Parameters[ ParameterIndex[ i ]].Updated ) ||
					( AlwaysReportAllParameters )))
		{
		// Is this a numeric parameter?
			if(Parameters[ParameterIndex[i]].Type == HTAG_NUMERIC_PARAMETER)
			{
				sprintf(temp, "%d", *(Parameters[ParameterIndex[i]].Value.i));
				ReturnStatus = TaggedImageParameterList->Append(temp);
			}
			else
			{
			// This is a string parameter.  Convert any double quote or GS characters to HEX format, convert HEX
			// characters, surround with quotes, and store.
				ReturnStatus = TaggedImageParameterList->AppendChar('"');
				int		j;
				HSTRING	PrintableCopy(Parameters[ParameterIndex[i]].Value.s, true);
				PrintableCopy.UnprintableCharactersToFormats();
				for(j = 0;    j < (int) PrintableCopy.Size;    ++j)
				{
				// If the character is quoted, convert it to HEX format.
					if((PrintableCopy.Char[j] == '"') || (PrintableCopy.Char[j] == 0x1d))
					{
						sprintf(temp, "\\%2X", (int) PrintableCopy.Char[j]);
						ReturnStatus = TaggedImageParameterList->Append(temp);
					}
					else
					{
					// This is not a double quote or a group separator; append it as is.
						TaggedImageParameterList->AppendChar((int) PrintableCopy.Char[j]);
					}
				}

			// Append the trailing double quote.
				ReturnStatus = TaggedImageParameterList->AppendChar('"');
			}

		// Append the parameter name.
			ReturnStatus = TaggedImageParameterList->Append(Parameters[ParameterIndex[i]].NameTag);
		}
	}

// Append a group separate to delimit the end of the header.
	ReturnStatus = TaggedImageParameterList->AppendNumber(GS, 1);


    return (ReturnStatus);
} // BuildParameterList







//
//	CreateTaggedImage (version #1)
//		Create a tagged image in an empty string sent by the caller, using the data pointed at by the caller.
//		Version #1 returns two parts to the tagged image: one containing the non-protocol wrapped portion of
//		the tagged image and the other containing the protocol wrapped portion.  The tagged image is simply a
//		string that follows this format:
//
//		If this routine runs out of space in the string sent by the caller, it will return false, else it will return
//		true.
//
//		The non-protocol wrapped command string, consisting of the <SYN>FE header string, the 6 character command name, and
//		the protocol parameter, are included in NonProtocolWrappedPartOfTaggedImage, so that this command string can be sent
//		directly to the receiver.  The remainder of the tagged image is stored in ProtocolWrappedPartOfTaggedImage, and may
//		be protocol wrapped depending upon the protocol parameter.
//
bool HTAG::CreateTaggedImage( HSTRING *NonProtocolWrappedPartOfTaggedImage, HSTRING *ProtocolWrappedPartOfTaggedImage,
				const unsigned char *Data, int SizeOfData, bool IncludeAsynchronousHeader )
{
	bool	ReturnStatus = true;	        // Returns false if we ran out of space; true otherwise
	char	temp[80];



/*		NO LONGER TRUE!!!!
// Assume that tagged data was already NULLed out if necessary.  We don't do it here, because the caller may
// have already stored a command header here.
*/
	NonProtocolWrappedPartOfTaggedImage->Copy("");
	ProtocolWrappedPartOfTaggedImage->Copy("");


// Are we building the output the new (Matrix) way or the old (44XX) way?
	if(*(Parameters[ TransferProtocolParameterNumber ].Value.i) < HTAG_TRANSMIT_NO_PROTOCOL)
	{
		ReturnStatus = BuildTaggedImageUsingPreMatrixProtcols( ProtocolWrappedPartOfTaggedImage, Data, SizeOfData );
	}


	else
	{
	// Build the parameter list and add the data.
		ReturnStatus = BuildParameterList( ProtocolWrappedPartOfTaggedImage );
		ReturnStatus = ProtocolWrappedPartOfTaggedImage->Append((unsigned char *) Data, SizeOfData);

	// Create the command.  The protocol parameter must always immediately follow the command so that the host will
	// immediately know enough to change to the appropriate protocol.  All the rest of the command header and the data may
	// be protocol wrapped during transmission.
		if(ReturnStatus)
		{
			if( Parameters[ TransferProtocolParameterNumber ].AlwaysReport )
			{
				sprintf(temp, "%s%d%s", (char *) CommandName->Char, *(Parameters[ TransferProtocolParameterNumber ].Value.i),
								Parameters[ TransferProtocolParameterNumber ].NameTag);
			}
			else
			{
				sprintf( temp, "%s", (char *) CommandName->Char );
			}

			if(IncludeAsynchronousHeader)
			{
				ReturnStatus = NonProtocolWrappedPartOfTaggedImage->Copy("\x02\xFE");
				ReturnStatus = NonProtocolWrappedPartOfTaggedImage->AppendNumber
													(ProtocolWrappedPartOfTaggedImage->Size + strlen(temp), 4);
				ReturnStatus = NonProtocolWrappedPartOfTaggedImage->Append("\x0D");
				ReturnStatus = NonProtocolWrappedPartOfTaggedImage->Append(temp);
				ReturnStatus = NonProtocolWrappedPartOfTaggedImage->Append("SOLO1");
			}
			else
			{
				ReturnStatus = NonProtocolWrappedPartOfTaggedImage->Copy(temp);
			}
		}
	}


    return (ReturnStatus);
} // CreateTaggedImage (version #1)






//
//	CreateTaggedImage (version #2)
//		Create a tagged image in an empty string sent by the caller, using the data pointed at by the caller.
//		Version #2 returns a single string containing the tagged image.
//
//		If this routine runs out of space in the string sent by the caller, it will return false, else it will return
//		true.
//
//		The tagged image string consists of the <SYN>FE header string, the 6 character command name, protocol parameter,
//		all other parameters, and data.
//
bool HTAG::CreateTaggedImage( HSTRING *TaggedImage, const unsigned char *Data, int SizeOfData, bool IncludeAsynchronousHeader)
{
	bool	ReturnStatus = true;	        // Returns false if we ran out of space; true otherwise
	char	temp[80];



/*		NO LONGER TRUE!!!!
// Assume that tagged data was already NULLed out if necessary.  We don't do it here, because the caller may
// have already stored a command header here.
*/
	TaggedImage->Copy("");


// Are we building the output the new (Matrix) way or the old (44XX) way?
	if(*(Parameters[ TransferProtocolParameterNumber ].Value.i) < HTAG_TRANSMIT_NO_PROTOCOL)
	{
		ReturnStatus = BuildTaggedImageUsingPreMatrixProtcols( TaggedImage, Data, SizeOfData );
	}


	else
	{
	// Create the command.  The protocol parameter must always immediately follow the command so that the host will
	// immediately know enough to change to the appropriate protocol.  All the rest of the command header and the data may
	// be protocol wrapped during transmission.
		if( Parameters[ TransferProtocolParameterNumber ].AlwaysReport )
		{
			sprintf(temp, "%s%d%s", (char *) CommandName->Char, *(Parameters[ TransferProtocolParameterNumber ].Value.i),
								Parameters[ TransferProtocolParameterNumber ].NameTag);
		}
		else
		{
			sprintf( temp, "%s", (char *) CommandName->Char );
		}

		if(IncludeAsynchronousHeader)
		{
			ReturnStatus = TaggedImage->Copy("\x02\xFE");
			ReturnStatus = TaggedImage->AppendNumber(0, 4);
			ReturnStatus = TaggedImage->Append("\x0D");
			ReturnStatus = TaggedImage->Append(temp);
			ReturnStatus = TaggedImage->Append("SOLO2");
		}
		else
		{
			ReturnStatus = TaggedImage->Copy(temp);
		}

	// Build the parameter list and add the data.
		ReturnStatus = BuildParameterList( TaggedImage );
		ReturnStatus = TaggedImage->Append((unsigned char *) Data, SizeOfData);

	// If we included the <SYN>FE header, update the length.
		if(ReturnStatus)
		{
			if(IncludeAsynchronousHeader)
			{
				ReturnStatus = TaggedImage->InsertNumber( 2, TaggedImage->Size - HTAG_HEADER_STRING_SIZE, 4 );
			}
		}
	}


    return (ReturnStatus);
} // CreateTaggedImage (version #2)







//
//	CreateTaggedImage (version #3)
//		Create a tagged image in an empty string sent by the caller, using the local data pointer.  Version #3
//		returns two parts to the tagged image: one containing the non-protocol wrapped portion of the tagged
//		image and the other containing the protocol wrapped portion.
//
//		Version #1 is called to actually do the work.
//
bool HTAG::CreateTaggedImage( HSTRING *NonProtocolWrappedPartOfTaggedImage, HSTRING *ProtocolWrappedPartOfTaggedImage,
				bool IncludeAsynchronousHeader )
{
	unsigned char	*ucData;
	int				SizeOfData;

	if( Data != NULL )
	{
		ucData = Data->Char;
		SizeOfData = Data->Size;
	}
	else if( HTagData != NULL )
	{
		HTagData->CreateTaggedImage();
		ucData = HTagData->Char;
		SizeOfData = HTagData->Size;
	}
	else
	{
		return false;
	}

	return( CreateTaggedImage( NonProtocolWrappedPartOfTaggedImage, ProtocolWrappedPartOfTaggedImage, ucData, SizeOfData,
					IncludeAsynchronousHeader ) );
} // CreateTaggedImage (version #3)






//
//	CreateTaggedImage (version #4)
//		Create a tagged image in an empty string sent by the caller, using the local data pointer.  Version #4
//		returns a single string containing the tagged image.
//
//		Version #2 is called to actually do the work.
//
bool HTAG::CreateTaggedImage( HSTRING *TaggedImage, bool IncludeAsynchronousHeader)
{
	unsigned char	*ucData;
	int				SizeOfData;

	if( Data != NULL )
	{
		ucData = Data->Char;
		SizeOfData = Data->Size;
	}
	else if( HTagData != NULL )
	{
		HTagData->CreateTaggedImage();
		ucData = HTagData->Char;
		SizeOfData = HTagData->Size;
	}
	else
	{
		return false;
	}

	return( CreateTaggedImage( TaggedImage, ucData, SizeOfData, IncludeAsynchronousHeader ) );
} // CreateTaggedImage (version #4)






//
//	LoadTaggedImage (version #1)
//		Load the local image with a tagged image contained in the string sent by the caller.  Version #1 uses data pointed
//		at by the caller.  The tagged image is simply a string that follows this format:
//
//		The following steps are taken:
//			- Tagged fields are extracted until the end of the header is reached.
//          - The remaining data after the header is assumed to be image and is copied to local image data storage.
//
//		Returned status will reflect one of the following cases:
//			- A field was incorrectly labelled.
//			- No comma was found.
//
int HTAG::LoadTaggedImage( HSTRING *TaggedImage, unsigned char **Data, int *SizeOfData, bool SYNFEHeaderMustBePresent )
{
	int		ReturnStatus;				// Status of tagged image conversion
	int		NextLocationToRead=0;			// Pointer to next place in tagged image from which to extract a tagged field.
	int		MessageCommandLength;		// If a message command is found at the beginning of the tagged image, this is set to
										//	the length in the header.
	int		i;


// So far, indicate that we're OK.
	ReturnStatus = HTAG_OK;


// If the tagged image was processed first by the pre-load function, then skip the tagged image parsing.
	if(!PreLoadTaggedImage(TaggedImage, &ReturnStatus))
	{
	// Parse through the header.  Look for the group separator first, which marks the end of the header.
		int		PositionWhereGroupSeparatorWasFound;
		bool	GroupSeparatorWasFound = false;
		int		NextOccurrenceNumber = 1;
		bool	MissingSYNFEHeader = false;

	// A group separator character could appear in the length portion of the <SYN>FE header, so if we find one there,
	// we need to ignore it and keep on looking.
		if( IsValidSYNFEHeader( TaggedImage, &MessageCommandLength ))
		{
			do  {
				if(TaggedImage->Find(((unsigned char *) "\x1D"), 1, false, NextOccurrenceNumber,
										&PositionWhereGroupSeparatorWasFound))
				{
					GroupSeparatorWasFound = true;
					++NextOccurrenceNumber;
				}
				else
				{
					GroupSeparatorWasFound = false;
				}
			} while( ( GroupSeparatorWasFound ) && ( PositionWhereGroupSeparatorWasFound < HTAG_HEADER_STRING_SIZE ) );
		}
		else if( !SYNFEHeaderMustBePresent )
		// No SYN<FE> header, so we can search for the first occurence of <GS>.
		{
			if(TaggedImage->Find(((unsigned char *) "\x1D"), 1, false, 1, &PositionWhereGroupSeparatorWasFound))
			{
				GroupSeparatorWasFound = true;
			}
		}
		else
		{
		// No SYN<FE> header, but there was supposed to be one; woof.
			MissingSYNFEHeader = true;
		}


		if( MissingSYNFEHeader )
		// Return missing header error code.
		{
			ReturnStatus = HTAG_MISSING_SYN_FE_HEADER;
		}
		else if( GroupSeparatorWasFound )
		// If we found a <GS>, parse the header.
		{
			HSTRING		Header(PositionWhereGroupSeparatorWasFound, true);
			TaggedImage->CopyFromString(0, PositionWhereGroupSeparatorWasFound, &Header);
			ReturnStatus = ParseHeader( &Header, &MessageCommandLength );
			NextLocationToRead = PositionWhereGroupSeparatorWasFound + 1;
		}
		else
		// We didn't find a GS; WOOF!
		{
			ReturnStatus = HTAG_MISSING_GROUP_SEPARATOR_AFTER_HEADER;
		}


	// If we made it to the end without errors, store the parameters and copy the actual image data to our local buffer.
		if((ReturnStatus == HTAG_OK) || (ReturnStatus == HTAG_UNRECOGNIZED_PARAMETER_NAME) || (ReturnStatus < 0))
		{
			*Data = &TaggedImage->Char[NextLocationToRead];
			*SizeOfData = TaggedImage->Size - NextLocationToRead;
		}
	}


// If no errors have occurred and the protocol is Dolphin, remove extraneous ESC characters.
	if(((ReturnStatus == HTAG_OK) || (ReturnStatus == HTAG_UNRECOGNIZED_PARAMETER_NAME) || (ReturnStatus < 0)) &&
			(*(Parameters[ TransferProtocolParameterNumber ].Value.i) == HTAG_TRANSMIT_DOLPHIN_PROTOCOL))
	{
	// Put an ESC in front of all ESC and EOT characters, then tack an EOT on the end.
		int		j = 0;
		i = 0;
		while(i < (*SizeOfData))			// If last character is ESC, we don't care.
		{
			if(TaggedImage->Char[i + NextLocationToRead] == ESC)
			{
			// Skip this character
				++i;
			}

			TaggedImage->Char[j + NextLocationToRead] = TaggedImage->Char[i + NextLocationToRead];
			++i;
			++j;
		}

	// If the last character is not an EOT, copy that too.
		if(TaggedImage->Char[i + NextLocationToRead] != EOT)
		{
			TaggedImage->Char[j + NextLocationToRead] = TaggedImage->Char[i + NextLocationToRead];
		}

	// Resize.
		*SizeOfData = j;
	}


// If we made it this far without errors, give the derived class the opportunity to do cleanup / validation.
	if((ReturnStatus == HTAG_OK) || (ReturnStatus == HTAG_UNRECOGNIZED_PARAMETER_NAME) || (ReturnStatus < 0))
	{
		PostLoadTaggedImage( &ReturnStatus );
	}


	return(ReturnStatus);
} // LoadTaggedImage (version #1)






//
//	LoadTaggedImage (version #2)
//		Load the local image with a tagged image contained in the string sent by the caller.  Version #1 uses the local
//		data pointer.
//
//		Version #1 is called to actually do the work.
//
//		NOTE: It is possible for the caller to use the Data array as an input to conserve space.  When the copy is performed,
//			the data will be shuffled into the correct position, overwriting the parameter block at the beginning.
//
int HTAG::LoadTaggedImage( HSTRING *TaggedImage, bool SYNFEHeaderMustBePresent )
{
    unsigned char	*DataPtr;
	int		SizeOfData;

	if(( Data == NULL ) && ( HTagData == NULL ))
		return HTAG_NULL_DATA_POINTER;

	int		Status = LoadTaggedImage( TaggedImage, &DataPtr, &SizeOfData, SYNFEHeaderMustBePresent );

// If we successfully loaded, copy the data from its' location in the tagged image to Data.
	if((Status == HTAG::HTAG_OK) || (Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (Status < 0))
	{
		if( Data != NULL )
			Data->Copy(DataPtr, SizeOfData);

		if( HTagData != NULL )
		{
			HTagData->HSTRING::Copy( DataPtr, SizeOfData );
			HTagData->LoadTaggedImage();
		}
	}

	return(Status);
} // LoadTaggedImage (version #2)





//
//	ReadHTagData
//		Read data from a Matrix device in HTag format.  Any HTag compatible receives, such as HTagImage and HTagScript,
//		can use this routine to actually get the data.  If CopyOfProtocolParameterString is non-NULL, return the protcol
//		parameter that was found, since it is otherwise removed from the output stream (useful for diagnostics).
//
bool HTAG::ReadHTagData( HSTRING *HTagData,  char *Result, int NumberOfCharactersToBeReceived,
						HSTRING *CopyOfProtocolParameterString )
{
	bool		Status = true;
	HSTRING		Temp(100, true);
	int			Protocol;


// Quit immediately if no device was defined.
	if((Device == NULL) || (TransferStatus == NULL))
	{
		return(false);
	}


// Determine the protocol first.  It will be in the form "nX", where n is an integer defining the protocol and
// X is the protocol parameter name.
	bool		EndOfNumericParameterReached = false;
	bool		FoundAtLeastOneNumericDigit = false;



	Protocol = 0;

// Look first for the numeric protocol parameter.
	HSTRING		ProtocolParameterString( 20, true );	// Store the protocol parameter string as we process it.  If this
														//	turns out not to be a protocol parameter, and the protocol
														//	parameter is not mandatory, we'll want to process this as part
														//	of the regular data stream.
	bool		FailedToFindAProtocolParameter = false;
	while(( Status ) && ( !FailedToFindAProtocolParameter ) && (!EndOfNumericParameterReached))
	{
		if( HTagData->Size > 0 )
		{
			Temp.Copy( HTagData, 0, 0 );
			HTagData->Remove( 0, 1 );
		}
		else
		{
			Device->Read(&Temp, 1, Device->DefaultCharacterReadTimeoutForHTAGNoProtocol );
		}

		if(Temp.Size != 1)
		{
			FailedToFindAProtocolParameter = true;
			strcpy(Result, "Timeout while waiting for protocol\015\012");
		}
		else
		{
			ProtocolParameterString.Append( &Temp );
			--NumberOfCharactersToBeReceived;
			if((Temp.Char[0] >= '0') && (Temp.Char[0] <= '9'))
			{
				FoundAtLeastOneNumericDigit = true;
				Protocol = (Protocol * 10) + ((int) (Temp.Char[0] - '0'));
			}
			else
			{
				if(FoundAtLeastOneNumericDigit)
				{
					EndOfNumericParameterReached = true;
				}
				else
				{
					FailedToFindAProtocolParameter = true;
					strcpy(Result, "Bad protocol\015\012");
				}
			}
		}
	}

// Now look for the protocol parameter name.
	HSTRING		ParameterName(&Temp, true);
	while((Status) && ( !FailedToFindAProtocolParameter ) &&
						(ParameterName.Size < strlen( Parameters[ TransferProtocolParameterNumber ].NameTag )))
	{
		if( HTagData->Size > 0 )
		{
			Temp.Copy( HTagData, 0, 0 );
			HTagData->Remove( 0, 1 );
		}
		else
		{
			Device->Read(&Temp, 1, Device->DefaultCharacterReadTimeoutForHTAGNoProtocol );
		}

		if(Temp.Size != 1)
		{
			FailedToFindAProtocolParameter = true;
			strcpy(Result, "Timeout while waiting for protocol\015\012");
		}
		else
		{
			ProtocolParameterString.Append( &Temp );
			ParameterName.Append(&Temp);
			--NumberOfCharactersToBeReceived;
		}
	}

// If we received the parameter name, does it match what we're expecting?
	if((Status) && ( !FailedToFindAProtocolParameter ))
	{
		if(ParameterName.Compare( Parameters[ TransferProtocolParameterNumber ].NameTag, true ) != 0)
		{
			FailedToFindAProtocolParameter = true;
			strcpy(Result, "Incorrect protocol parameter name\015\012");
		}
	}


// If we failed to receive a protocol parameter, and the protocol isn't required, reinsert the data we've read into the
// input stream and start processing.
	if( Status )
	{
		if( FailedToFindAProtocolParameter )
		{
			if( Parameters[ TransferProtocolParameterNumber ].AlwaysReport )
			{
			// We didn't get a protocol parameter, but it is required; WOOF!
				Status = false;
			}
			else
			{
			// Save what we've read so far.
				Protocol = HTAG_TRANSMIT_NO_PROTOCOL;		// Default in case there is no protocol parameter.
				HTagData->Insert( &ProtocolParameterString, 0 );
				NumberOfCharactersToBeReceived += ProtocolParameterString.Size;
			}
		}
		else
		{
		// If the caller sent an address for CopyOfProtocolParameterString, do the copy now.
			if( CopyOfProtocolParameterString != 0 )
				CopyOfProtocolParameterString->Copy( &ProtocolParameterString );
		}
	}


// If protocol is HMODEM, receive via HMODEM, else receive the # of bytes sent by the caller.
	if(Status)
	{
		TransferProtocol = Protocol;

	// Assume the best for now
		strcpy(Result, "Successfully received HTag data");


		if((Protocol == HTAG::HTAG_TRANSMIT_HMODEM_HUFFMAN_COMPRESSED_PROTOCOL) ||
					(Protocol == HTAG::HTAG_TRANSMIT_HMODEM_LZ_COMPRESSED_PROTOCOL) ||
					(Protocol == HTAG::HTAG_TRANSMIT_HMODEM_UNCOMPRESSED_PROTOCOL))
		{
		// Get the HTAG data
			unsigned int		HModemStatus;

			HMODEM	*HModemSession = new HMODEM(Device, TransferStatus, CommandHandler);
		    HModemStatus = HModemSession->Receive(HTagData, HTagData->MaximumLength());
			delete HModemSession;

			switch(HModemStatus)
			{
				case HMODEM::TIMEOUT_WAITING_FOR_HMODEM_BLOCK_SIZE:
					strcpy(Result, "Timeout while waiting for HMODEM block size\015\012");
					Status = false;
					break;

				case HMODEM::TERMINATE_RECEIVED:
					strcpy(Result, "Transmitter terminated HMODEM\015\012");
					Status = false;
					break;

				case HMODEM::ILLEGAL_HMODEM_BLOCK_SIZE:
					strcpy(Result, "Illegal HMODEM block size received\015\012");
					Status = false;
					break;

				case HMODEM::TIMEOUT_WAITING_FOR_HMODEM_BLOCK_NUMBER:
					strcpy(Result, "Timeout while waiting for HMODEM block number\015\012");
					Status = false;
					break;

				case HMODEM::MISMATCHED_HMODEM_BLOCK_NUMBER:
					strcpy(Result, "Mismatched HMODEM block number\015\012");
					Status = false;
					break;

				case HMODEM::TIMEOUT_WAITING_FOR_HMODEM_INVERSE_BLOCK_NUMBER:
					strcpy(Result, "Timeout while waiting for HMODEM inverse block number\015\012");
					Status = false;
					break;

				case HMODEM::MISMATCHED_HMODEM_INVERSE_BLOCK_NUMBER:
					strcpy(Result, "Timeout while waiting for HMODEM inverse block number\015\012");
					Status = false;
					break;

				case HMODEM::TIMEOUT_WAITING_FOR_HMODEM_BLOCK:
					strcpy(Result, "Timeout while waiting for HMODEM data block\015\012");
					Status = false;
					break;

				case HMODEM::TIMEOUT_WAITING_FOR_HMODEM_CRC:
					strcpy(Result, "Timeout while waiting for HMODEM CRC\015\012");
					Status = false;
					break;

				case HMODEM::MISMATCHED_HMODEM_CRC:
					strcpy(Result, "Mismatched HMODEM CRC\015\012");
					Status = false;
					break;

				case HMODEM::TRANSMIT_DATA_TOO_LARGE:
					strcpy(Result, "Transmit HMODEM data too large\015\012");
					Status = false;
					break;

				case HMODEM::TERMINATE_BY_US:
					strcpy(Result, "Receiver terminated HMODEM\015\012");
					Status = false;
					break;

				case HMODEM::FINISH_SUCCESSFUL:
					break;

				default:
					strcpy(Result, "Error while receiving HMODEM HTag data\015\012");
					Status = false;
					break;
			}
		}


		else
		{
		// We're doing a raw receive.
			int		NumberOfCharactersToTryToRead;
			int		TotalNumberOfBytesToReceive = NumberOfCharactersToBeReceived;

			Temp.Reallocate(10000);

		// Let the instantiator know we're starting.
	        TransferStatus->SetProgressLevel( 0 );

			HTagData->Reallocate(TotalNumberOfBytesToReceive + 2);
			NumberOfCharactersToBeReceived -= HTagData->Size;
//			HTagData->Copy("");
			while((Status) && (NumberOfCharactersToBeReceived > 0))
			{
				NumberOfCharactersToTryToRead = (NumberOfCharactersToBeReceived > 10000) ? 10000 : NumberOfCharactersToBeReceived;
				bool result =Device->Read(&Temp, NumberOfCharactersToTryToRead, Device->DefaultCharacterReadTimeoutForHTAGNoProtocol );
				if(Temp.Size < 1 || !result)
				{
					Status = false;
					sprintf(Result, "Timeout while waiting for remaining %d bytes of HTag data\015\012",
							NumberOfCharactersToBeReceived);
				}
				else
				{
			        if(TransferStatus->MayContinue(Temp.Size, TotalNumberOfBytesToReceive))
					{
						NumberOfCharactersToBeReceived -= Temp.Size;
						HTagData->Append(&Temp);
					}
					else
					{
						Status = false;
					}
				}
			}
		}
	}

	return(Status);
} // ReadHTagData





//
//	WriteHTagData
//		Write data to a Matrix device in HTag format.  Any HTag compatible transmits, such as HTagImage and HTagScript,
//		can use this routine to actually send the data.
//
bool HTAG::WriteHTagData( HSTRING *Command, HSTRING *TaggedImage, char *Result, bool WaitForSecondACK )
{
	bool		Status = true;		// Assume success for now.
	bool		UseHModemProtocol;
	int			CompressionMode;





// Quit immediately if no device was defined.
	if((Device == NULL) || (TransferStatus == NULL))
	{
		return(false);
	}


// Are we using HMODEM?
	UseHModemProtocol = (( TransferProtocol == HTAG::HTAG_TRANSMIT_HMODEM_HUFFMAN_COMPRESSED_PROTOCOL) ||
						( TransferProtocol == HTAG::HTAG_TRANSMIT_HMODEM_LZ_COMPRESSED_PROTOCOL) ||
						( TransferProtocol == HTAG::HTAG_TRANSMIT_HMODEM_UNCOMPRESSED_PROTOCOL)) ? true : false;

// Should the transfer be done using compression?
	switch( TransferProtocol )
	{
		case HTAG::HTAG_TRANSMIT_HMODEM_HUFFMAN_COMPRESSED_PROTOCOL:
			CompressionMode = HUFFMAN_VERSION_2_0;
			break;

		case HTAG::HTAG_TRANSMIT_HMODEM_LZ_COMPRESSED_PROTOCOL:
			CompressionMode = LEMPEL_ZIV_VERSION_2_0;
			break;

		default:
			CompressionMode = NO_COMPRESSION;
			break;
	}


// Assume the best for now
	strcpy(Result, "Successfully transmitted HTag data");


// Transmit according to protocol setting.
	if(UseHModemProtocol)
	{
		unsigned int		HModemStatus;

		HMODEM	*HModemSession = new HMODEM(Device, TransferStatus, CommandHandler);
	    HModemStatus = HModemSession->Transmit(TaggedImage, Command, WaitForSecondACK, true, CompressionMode);
		delete HModemSession;


       	switch(HModemStatus)
   	    {
			case HMODEM::TERMINATE_RECEIVED:
				strcpy(Result, "HModem Download - Receiver terminated download\015\012");
				Status = false;
				break;

			case HMODEM::RECEIVER_NOT_RESPONSIVE_TO_FINISH:
				strcpy(Result, "HModem Download - Receiver not responding after download completion\015\012");
				Status = false;
				break;

			case HMODEM::RECEIVER_NAKED:
				strcpy(Result, "HModem Download - Receiver NAKed download operation; terminating...\015\012");
				Status = false;
				break;

			case HMODEM::RECEIVER_IS_UNRESPONSIVE:
				strcpy(Result, "HModem Download - Timeout while waiting for receiver response\015\012");
				Status = false;
				break;

			case HMODEM::RECEIVER_NOT_READY:
				strcpy(Result, "HModem Download - Receiver not ready to receive data\015\012");
				Status = false;
				break;

			case HMODEM::TERMINATE_BY_US:
				strcpy(Result, "HModem Download - Receiver terminated HMODEM\015\012");
				Status = false;
				break;

			case HMODEM::FINISH_SUCCESSFUL:
				break;

			default:
				strcpy(Result, "Error while transmitting HMODEM image\015\012");
				Status = false;
				break;
		}
	}


// Send the tagged image with no protocol
	else
	{
		int		TotalNumberOfBytesToWrite = Command->Size + TaggedImage->Size;
		int		BytesWrittenSoFar = 0;

	// Let the instantiator know we're starting.
		TransferStatus->SetProgressLevel( 0 );

		Status =Device->Write(Command);
		BytesWrittenSoFar += Command->Size;
		int			LengthOfNextCopy = BytesWrittenSoFar;	// Since we're using this to tell MayContinue how much we've
															//	written, set this initially to what we've already copied.
		int			TaggedImageBytesWritten = 0;

		while(( TransferStatus->MayContinue( LengthOfNextCopy, TotalNumberOfBytesToWrite )) &&
				( BytesWrittenSoFar < TotalNumberOfBytesToWrite ) && Status)
		{
			LengthOfNextCopy = (( TaggedImage->Size - TaggedImageBytesWritten ) >= 2048 ) ? 2048 :
										( TaggedImage->Size - TaggedImageBytesWritten );
			Status =Device->Write( TaggedImage, TaggedImageBytesWritten, LengthOfNextCopy, true );
			BytesWrittenSoFar += LengthOfNextCopy;
			TaggedImageBytesWritten += LengthOfNextCopy;
		}

	// Let the instantiator know we're done.
		TransferStatus->SetProgressLevel( TotalNumberOfBytesToWrite );
	}


// If status is good, and we're supposed to wait for a response, wait now.
	if(( Status ) && ( ResponseRequired ))
	{
// Read the header.  The command had better be HTAG_RETURN_PARAMETER_PACKET_COMMAND_NAME.

// **** SYNFECommandHandler had better return HTAG_RETURN_PARAMETER_PACKET_COMMAND_NAME when requested to do so!


//		ReadHTagData(  );
	}


	return(Status);
} // WriteHTagData






//
//	ReadTaggedImage
//		Read a tagged image from the device and load it into the local image.  If there are already enough characters in
//		OutputImage, process without receiving any more data.
//
//		If OutputImage is undefined (NULL), use Data to hold the HTAG message and the data that is pulled out of the HTAG
//		message.  A caller may do this to save buffer space on large amounts of data (such as images).
//
int HTAG::ReadTaggedImage( HSTRING *OutputImage, char *Result, int NumberOfCharactersToBeReceived)
{
// Get the script data in HTag format from the host.
	int		Status;


	if(( OutputImage != NULL ) && ( (int) OutputImage->Size >= NumberOfCharactersToBeReceived ) )
	{
		Status = LoadTaggedImage(OutputImage);
	}
	else
	{
	// Use Data if there is no output image.
		if( OutputImage == NULL )
		{
			if( Data != NULL )
				OutputImage = Data;
			else
				OutputImage = HTagData;
			OutputImage->Copy( "" );
		}

		if(ReadHTagData( OutputImage, Result, NumberOfCharactersToBeReceived ))
		{
			Status = LoadTaggedImage(OutputImage);
		}
		else
		{
			Status = HTAG_I_O_ERROR;
		}
	}

	return(Status);
} // ReadTaggedImage






//
//	WriteTaggedImage
//		Write a tagged image to the device and load it into the local image.
//
int HTAG::WriteTaggedImage(char *Result, bool WaitForSecondACK )
{
// Create the tagged image.
	HSTRING		NonProtocolWrappedPartOfTaggedImage(100, true);
	HSTRING		ProtocolWrappedPartOfTaggedImage(100, true);
	CreateTaggedImage(&NonProtocolWrappedPartOfTaggedImage, &ProtocolWrappedPartOfTaggedImage, true);

// Send the data.
	return(WriteHTagData( &NonProtocolWrappedPartOfTaggedImage, &ProtocolWrappedPartOfTaggedImage, Result, WaitForSecondACK )
					? HTAG_OK : HTAG_I_O_ERROR);
} // WriteTaggedImage





//
//	FindHeader
//		Find a <SYN>FE header in the tagged image, and return the offset to the caller.
//
int HTAG::FindHeader( HSTRING *TaggedImage )
{
	int		HeaderLocation = -1;		// Haven't found a header yet.
	int		NextOccurrenceOfSYN_FE = 1;
	int		LocationWhereSYN_FE_WasFound;
	bool	EndOfTaggedImage = false;	// Haven't reach the end of the tagged image yet.


	while( (HeaderLocation < 0) && (!EndOfTaggedImage) )
	{
		if( TaggedImage->Find( (unsigned char *) "\x02\xFE", 2, true, NextOccurrenceOfSYN_FE, &LocationWhereSYN_FE_WasFound) )
		{
			++NextOccurrenceOfSYN_FE;
			if( ((TaggedImage->Size - LocationWhereSYN_FE_WasFound) >
									(HTAG_HEADER_STRING_SIZE + HTAG_COMMAND_NAME_SIZE)) &&
						(TaggedImage->Char[LocationWhereSYN_FE_WasFound + 6] == 0x0D) &&
						(TaggedImage->CompareSubstring( LocationWhereSYN_FE_WasFound + HTAG_HEADER_STRING_SIZE,
										CommandName, false )  == 0) )
			{
				HeaderLocation = LocationWhereSYN_FE_WasFound;
			}
		}
		else
		{
			EndOfTaggedImage = true;
		}
	}

	return HeaderLocation;
} // FindHeader







//
//	ValidateDataStreamAsSYNFECommand
//		See if the data stream sent by the caller is a <SYN>FE command.  The data stream must be a valid <SYN>FE command up
//		to its' end.  We send back various condition codes depending upon how much data was processed.
//
int HTAG::ValidateDataStreamAsSYNFECommand( const char *DataStream, int SizeOfDataStream, int MaximumHTAGCommandSize )

{
	int			Status;						// Nothing processed yet.
	int			NextByteToExamine;
	int			CommandLength;



	Status = HTAG_INCOMPLETE_COMMAND_FOUND;
	NextByteToExamine = 0;

// Look for the <SYN> first.
	if(( Status == HTAG_INCOMPLETE_COMMAND_FOUND ) && ( NextByteToExamine < SizeOfDataStream ))
	{
		if( DataStream[ NextByteToExamine ] == STX )
		{
			++NextByteToExamine;
		}
		else
		{
			Status = HTAG_MISSING_SYN_CHARACTER;
		}
	}


// Look for the 0xFE.
	if(( Status == HTAG_INCOMPLETE_COMMAND_FOUND ) && ( NextByteToExamine < SizeOfDataStream ))
	{
		if( DataStream[ NextByteToExamine ] == 0xFE )
		{
			++NextByteToExamine;
		}
		else
		{
			Status = HTAG_MISSING_FE_CHARACTER;
		}
	}


// Get length.
	if(( Status == HTAG_INCOMPLETE_COMMAND_FOUND ) && ( ( NextByteToExamine + 3 ) < SizeOfDataStream ))
	{
		HSTRING		Temp( 4, true );

		Temp.Copy( &DataStream[ NextByteToExamine ], 4 );
		NextByteToExamine += 4;

	// Try to convert the number.
		Temp.CopyNumberFromString(0, 4, &CommandLength);
		if((CommandLength > MaximumHTAGCommandSize) || ( CommandLength < 0 ))
		{
			Status = HTAG_MAXIMUM_COMMAND_LENGTH_EXCEEDED;
		}
	}


// Get <CR>.
	if(( Status == HTAG_INCOMPLETE_COMMAND_FOUND ) && ( NextByteToExamine < SizeOfDataStream ))
	{
		if( DataStream[ NextByteToExamine ] == 0x0D )
		{
			++NextByteToExamine;
		}
		else
		{
			Status = HTAG_MISSING_CR;
		}
	}


// If we made it this far, verify the length.
	if( Status == HTAG_INCOMPLETE_COMMAND_FOUND )
	{
		if( ( CommandLength + HTAG_HEADER_STRING_SIZE ) > SizeOfDataStream )
		{
			Status = HTAG_COMPLETE_COMMAND_FOUND_WITH_EXTRANEOUS_DATA;
		}
		if( ( CommandLength + HTAG_HEADER_STRING_SIZE ) == SizeOfDataStream )
		{
			Status = HTAG::HTAG_OK;
		}
	}


	return(Status);
} // ValidateDataStreamAsSYNFECommand







//
//	ReceiveHTAGHeader
//		See if a <SYN>FE command has been received.  The argument to ReceiveHTAGHeader is the data that has been received
//		so far to start a <SYN>FE command.  If the string is NULL, ReceiveHTAGHeader will wait for WaitTimeForCommand
//		milliseconds to receive command data.
//
int HTAG::ReceiveHTAGHeader(char *Result, HSTRING *ReceivedData, HSTRING *CommandName, int *CommandLength,
						HCOMM *Device, HSTRING *UnusedCommandData )
{
	int			Status = HTAG_OK;		// Nothing processed yet.


// Result is OK so far.
	strcpy(Result, "Successfully received command");


// Nothing received by us so far.
	UnusedCommandData->Copy("");

// Look for the <SYN> first.
	if((Status == HTAG_OK) && (ReceivedData->Size < 1))
	{
		Device->Read( ReceivedData, 1, Device->WaitTimeForHTAGCommand );
		UnusedCommandData->Append( ReceivedData );
	}
	if(Status == HTAG_OK)
	{
		if(( ReceivedData->Size < 1 ) || ( ReceivedData->Char[0] != STX ))
		{
			Status = HTAG_MISSING_SYN_CHARACTER;
		}
	}
	ReceivedData->Remove(0, 1);


// Look for the 0xFE.
	if((Status == HTAG_OK) && (ReceivedData->Size < 1))
	{
		Device->Read( ReceivedData, 1, Device->WaitTimeForHTAGCommand );
		UnusedCommandData->Append( ReceivedData );
	}
	if(Status == HTAG_OK)
	{
		if(( ReceivedData->Size < 1 ) || ( ReceivedData->Char[0] != 0xFE ))
		{
			Status = HTAG_MISSING_FE_CHARACTER;
		}
	}
	ReceivedData->Remove(0, 1);


// Get length.
	if(( Status == HTAG_OK ) && ( ReceivedData->Size < 4 ))
	{
		HSTRING		Temp( 4, true );
		Device->Read( &Temp, 4 - ReceivedData->Size, Device->WaitTimeForHTAGCommand );
		ReceivedData->Append( &Temp );
		UnusedCommandData->Append( &Temp );
	}
	if(Status == HTAG_OK)
	{
		if(ReceivedData->Size >= 4)
		{
		// Try to convert the number.
			ReceivedData->CopyNumberFromString( 0, 4, CommandLength );
			if( *CommandLength > Device->MaximumHTAGCommandSize )
			{
				Status = HTAG_MAXIMUM_COMMAND_LENGTH_EXCEEDED;
			}
		}
		else
		{
			Status = HTAG_MISSING_LENGTH;
		}
	}
	ReceivedData->Remove(0, 4);


// Get <CR>.
	if(( Status == HTAG_OK ) && ( ReceivedData->Size < 1 ))
	{
		Device->Read( ReceivedData, 1, Device->WaitTimeForHTAGCommand );
		UnusedCommandData->Append( ReceivedData );
	}
	if(Status == HTAG_OK)
	{
		if(( ReceivedData->Size < 1 ) || ( ReceivedData->Char[0] != 0x0D ))
		{
			Status = HTAG_MISSING_CR;
		}
	}
	ReceivedData->Remove(0, 1);


// If we made it this far, get the command, which must be six characters.
	if(( Status == HTAG_OK ) && ( *CommandLength < HTAG_COMMAND_NAME_SIZE ))
	{
		Status = HTAG_MISSING_COMMAND;
	}
	if(( Status == HTAG_OK ) && ( ReceivedData->Size < HTAG_COMMAND_NAME_SIZE ))
	{
		HSTRING		Temp( HTAG_COMMAND_NAME_SIZE, true );
		Device->Read( &Temp, HTAG_COMMAND_NAME_SIZE - ReceivedData->Size, Device->WaitTimeForHTAGCommand );
		ReceivedData->Append( &Temp );
		UnusedCommandData->Append( &Temp );
	}
	CommandName->Copy( ReceivedData, 0, HTAG_COMMAND_NAME_SIZE - 1 );
	if(Status == HTAG_OK)
	{
		if(CommandName->Size != HTAG_COMMAND_NAME_SIZE)
		{
			Status = HTAG_MISSING_COMMAND;
		}
	}
	ReceivedData->Remove( 0, HTAG_COMMAND_NAME_SIZE );


	return Status;
} // ReceiveHTAGHeader








///---------------------------------------------------------------------------
//
//  Class Name:     HTAG_ARRAY
//
//  Class Description:
//
//		The HTAG_ARRAY class provides a mechanism to transfer / store arrays of HTAGS.  Since all fields are tagged, if the
//		receiver does not yet speak the new version of HTAG_ARRAY, it will ignore tags that it doesn't understand.
//
//---------------------------------------------------------------------------







//
// Constructor for HTAG_ARRAY
//
HTAG_ARRAY::HTAG_ARRAY( const char *Command, HCOMM *HCommDevice,
						HTRANSFER_STATUS *TransferStatus, HCOMMAND_HANDLER *CommandHandler)
		: HTAG( HCommDevice, TransferStatus, CommandHandler, false )
{
	Data = new HSTRING( DEFAULT_HTAG_ARRAY_DATA_ALLOCATION, true );
	HTAG::Data = Data;				// Give the pointer to the base class so it can operate on the data as well.
	CommandName->Copy(Command);

// Allocate 10 parameter table entries to start with.
	MaximumNumberOfRecordsAllocated = NUMBER_OF_RECORDS_TO_ADD_ON_REALLOCATE;
	RecordOffsets = new int[ MaximumNumberOfRecordsAllocated ];
	RecordSizes = new int[ MaximumNumberOfRecordsAllocated ];
	MaximumNumberOfRecordsAllowed = DEFAULT_MAXIMUM_NUMBER_OF_RECORDS_ALLOWED;

// Parameters with names.
	RecordName = new HSTRING(20, true);
	AddHStringParameter( HTAG_ARRAY_RECORD_NAME, RecordName, "", 0, true );

	RecordOffsetString = new HSTRING(20, true);
	AddHStringParameter( HTAG_ARRAY_RECORD_OFFSETS, RecordOffsetString, "", 0, true );

	AddNumericParameter( HTAG_ARRAY_NUMBER_OF_RECORDS, &NumberOfRecords, 0, 0, 0x7FFFFFFF, true );

    return;
}
//---------------------------------------------------------------------------





//
// Destructor for HTAG_ARRAY
//      If memory has been allocated, release it.
//
HTAG_ARRAY::~HTAG_ARRAY(void)
{
	delete Data;
	delete RecordName;
	delete RecordOffsetString;
	delete RecordOffsets;
	delete RecordSizes;

    return;
}





//
//	AddMoreRecordTableEntries
//		If we're out of record table entries, add more.
//
void HTAG_ARRAY::AddMoreRecordTableEntries( int NewRecordTableEntryCount )
{
	int		i;


// Are we out of entries in the record table?
	if( NewRecordTableEntryCount > MaximumNumberOfRecordsAllocated )
	{
	// Allocate more space in the parameter table, copy the existing entries, and delete the old table.
		MaximumNumberOfRecordsAllocated = NewRecordTableEntryCount + NUMBER_OF_RECORDS_TO_ADD_ON_REALLOCATE;
		int		*NewRecordOffsets = new int[ MaximumNumberOfRecordsAllocated ];
		int		*NewRecordSizes = new int[ MaximumNumberOfRecordsAllocated ];
		for( i = 0;    i < NumberOfRecords;    ++i )
		{
			NewRecordOffsets[i] = RecordOffsets[i];
			NewRecordSizes[i] = RecordSizes[i];
		}

		delete RecordOffsets;
		delete RecordSizes;
		RecordOffsets = NewRecordOffsets;
		RecordSizes = NewRecordSizes;
	}

	return;
} // AddMoreRecordTableEntries





//
//	ValidateRecordTrackingVariables
//		Validate that tracking variables are in sequence and within the size of the string.
//
int HTAG_ARRAY::ValidateRecordTrackingVariables( void )
{
	int		Status = HTAG_OK;			// Assume success for now.


// Are records even defined?
	if( NumberOfRecords < 1 )
	{
		Status = HTAG_ARRAY_NO_RECORDS_FOUND;
	}

	else
	{
	// There are records.  Let's validate them.
		int		i = 0;

		for( i = 0;    ( i < NumberOfRecords ) && ( Status == HTAG_OK );    ++i )
		{
			if( ( RecordOffsets[ i ] < 0 ) || ( RecordOffsets[ i ] >= (int) Data->Size ) )
			{
				Status = HTAG_ARRAY_RECORD_OFFSET_IS_OUT_OF_SEQUENCE;
			}
			else if( i > 0 )
			{
				if( RecordOffsets[ i ] < RecordOffsets[ i - 1 ] )
				{
					Status = HTAG_ARRAY_RECORD_OFFSET_IS_OUT_OF_BOUNDS;
				}
				else if( ( RecordOffsets[ i - 1 ] + RecordSizes[ i - 1 ] ) != RecordOffsets[ i ] )
				{
					Status = HTAG_ARRAY_RECORD_SIZE_MISMATCH;
				}
			}
		}

	// Check size of the last record.
		if(( Status == HTAG_OK ) &&
					(( RecordOffsets[ NumberOfRecords - 1 ] + RecordSizes[ NumberOfRecords - 1 ] ) != (int) Data->Size ))
		{
			Status = HTAG_ARRAY_RECORD_SIZE_MISMATCH;
		}
	}


	return Status;
} // ValidateRecordTrackingVariables





//
//	BuildRecordTrackingVariablesFromRecordString
//		Build record tracking variables from record and size strings.
//
int HTAG_ARRAY::BuildRecordTrackingVariablesFromRecordString( void )
{
	int		Status = HTAG_OK;			// Assume success for now.



	if( NumberOfRecords < 1 )
	{
		Status = HTAG_ARRAY_NO_RECORDS_FOUND;
	}

	else
	{
		AddMoreRecordTableEntries( NumberOfRecords );

	// Parse through the record offset string.
		int		i = 0;
		int		StartPositionOfNextRecordOffsetToProcessInRecordOffsetString = 0;
		int		EndPositionOfNextRecordOffsetToProcessInRecordOffsetString;
		int		NumberOfCharactersUsedInNumberConversion;
		while(( Status == HTAG_OK ) && ( i < NumberOfRecords ) &&
				( StartPositionOfNextRecordOffsetToProcessInRecordOffsetString < (int) Data->Size ) )
		{
		// Get the next offset.
			if( !Data->FindCharacterForward( 0x1C, StartPositionOfNextRecordOffsetToProcessInRecordOffsetString,
							&EndPositionOfNextRecordOffsetToProcessInRecordOffsetString ) )
			{
				EndPositionOfNextRecordOffsetToProcessInRecordOffsetString = Data->Size;
			}

		// Try to decode the number.
			RecordOffsets[ i ] = Data->ConvertToNumber( StartPositionOfNextRecordOffsetToProcessInRecordOffsetString,
									&NumberOfCharactersUsedInNumberConversion );

		// Validate the number.  The entire substring must be numeric, the offset must not be less than previous offsets (they
		// must be in sequence), and the offset must not be out of bounds.
			if( ( StartPositionOfNextRecordOffsetToProcessInRecordOffsetString + NumberOfCharactersUsedInNumberConversion ) !=
												EndPositionOfNextRecordOffsetToProcessInRecordOffsetString )
			{
				Status = HTAG_ARRAY_ILLEGAL_CHARACTERS_IN_RECORD_OFFSET_STRING;
			}

			StartPositionOfNextRecordOffsetToProcessInRecordOffsetString =
						EndPositionOfNextRecordOffsetToProcessInRecordOffsetString + 1;
			++i;
		}

	// Compute record sizes and validate the tracking variables.
		if( Status == HTAG_OK )
		{
			for( i = 1;    i < NumberOfRecords;    ++i )
			{
				RecordSizes[ i - 1 ] = RecordOffsets[ i ] - RecordOffsets[ i - 1 ];
			}
			RecordSizes[ NumberOfRecords - 1 ] = Data->Size - RecordOffsets[ NumberOfRecords - 1 ];

			Status = ValidateRecordTrackingVariables();
		}

	// Did we get too many records?
		if(( Status == HTAG_OK ) && ( StartPositionOfNextRecordOffsetToProcessInRecordOffsetString < (int) Data->Size ) )
		{
			Status = HTAG_ARRAY_TOO_MANY_RECORDS_IN_RECORD_OFFSET_STRING;
		}

	// Did we not get enough records?
		if(( Status == HTAG_OK ) && ( i < NumberOfRecords ) )
		{
			Status = HTAG_ARRAY_NOT_ENOUGH_RECORDS_IN_RECORD_OFFSET_STRING;
		}

	// If we have more records than are allowed, delete some.
		if(( Status == HTAG_OK ) && ( NumberOfRecords > MaximumNumberOfRecordsAllowed ))
		{
			while( NumberOfRecords > MaximumNumberOfRecordsAllowed )
			{
				RemoveRecord( NumberOfRecords - 1 );
			}
		}
	}


	return(Status);
} // BuildRecordTrackingVariablesFromRecordString





//
//	BuildRecordStringFromRecordTrackingVariables
//		Build record string from record tracking variables.
//
int HTAG_ARRAY::BuildRecordStringFromRecordTrackingVariables( void )
{
	int		Status = HTAG_OK;			// Assume success for now.


// Validate the records first.
	Status = ValidateRecordTrackingVariables();

// If records are valid, encode the string.
	int		i;
	char	temp[ 40 ];
	sprintf( temp, "%d", RecordOffsets[ 0 ] );
	RecordOffsetString->Copy( temp );

	for( i = 1;    i < NumberOfRecords;    ++i )
	{
		sprintf( temp, "%d", RecordOffsets[ i ] );
		RecordOffsetString->AppendChar( 0x1C );
		RecordOffsetString->Append( temp );
	}

	return Status;
} // BuildRecordStringFromRecordTrackingVariables








//
//	PostLoadTaggedImage
//		After loading tagged image, rebuild the record tracking variables.
//
bool HTAG_ARRAY::PostLoadTaggedImage( int *Status )
{
	*Status = this->BuildRecordTrackingVariablesFromRecordString();
	return ( *Status == HTAG_OK ) ? true : false;
} // PostLoadTaggedImage








//
//	RetrieveRecord
//		Retrieve a record from the array
//
int HTAG_ARRAY::RetrieveRecord( HSTRING *Record, int RecordNumber )
{
	int		Status = HTAG_OK;			// Assume success for now.


// Validate record number
	if(( RecordNumber < 0 ) || ( RecordNumber >= NumberOfRecords ))
	{
		Status = HTAG_ARRAY_RECORD_NUMBER_OUT_OF_RANGE;
	}
	else
	{
	// Record is valid; retrieve it.
		Record->Copy( Data, RecordOffsets[ RecordNumber ], RecordOffsets[ RecordNumber ] + RecordSizes[ RecordNumber ] - 1 );
	}

	return Status;
} // RetrieveRecord







//
//	StoreRecord
//		Store a record to the array (default is to store at end).
//
int HTAG_ARRAY::StoreRecord( HSTRING *Record, int RecordNumber )
{
	int		Status = HTAG_OK;			// Assume success for now.


// If record number is -1, store at the end.
	if( RecordNumber == -1 )
		RecordNumber = NumberOfRecords;

// Validate record number
	if(( RecordNumber < 0 ) || ( RecordNumber > NumberOfRecords ))
	{
		Status = HTAG_ARRAY_RECORD_NUMBER_OUT_OF_RANGE;
	}
	else
	{
	// Record number is valid; store it.  First, make sure there is space for the new record.
		if( NumberOfRecords >= MaximumNumberOfRecordsAllowed )
		{
			RemoveRecord( NumberOfRecords - 1 );
		}
		else
		{
			AddMoreRecordTableEntries( NumberOfRecords + 1 );
		}

	// If this is the last record, set the record offset to the size of the Data string, else use the existing record offset
	// and shift up all the entires above this record.
		if( RecordNumber == NumberOfRecords )
		{
			RecordOffsets[ RecordNumber ] = Data->Size;
		}

		else
		{
		// Shift existing record entries to make room for this one.
			int		i = NumberOfRecords;
			while( i > RecordNumber )
			{
				RecordOffsets[ i ] = RecordOffsets[ i - 1] + Record->Size;
				RecordSizes[ i ] = RecordSizes[ i - 1 ];
				--i;
			}
		}

	// Update the record table and add the record to Data.
		RecordSizes[ RecordNumber ] = Record->Size;
		Data->Insert( Record, RecordOffsets[ RecordNumber ] );
		++NumberOfRecords;
	}

// If we succeeded, update the record offset string.
	if( Status == HTAG_OK )
	{
		Status = BuildRecordStringFromRecordTrackingVariables();
	}

	return Status;
} // StoreRecord







//
//	RemoveRecord
//		Delete a record from the array
//
int HTAG_ARRAY::RemoveRecord( int RecordNumber )
{
	int		Status = HTAG_OK;			// Assume success for now.


// Validate record number
	if(( RecordNumber < 0 ) || ( RecordNumber >= NumberOfRecords ))
	{
		Status = HTAG_ARRAY_RECORD_NUMBER_OUT_OF_RANGE;
	}
	else
	{
	// Record number is valid; erase it.  First, remove it from the Data string.
		Data->Remove( RecordOffsets[ RecordNumber ], RecordSizes[ RecordNumber ] );

	// Shift existing record entries down to eliminate this record.
		int		DeletedRecordSize = RecordSizes[ RecordNumber ];
		int		i;
		for( i = RecordNumber + 1;    i < NumberOfRecords;    ++i )
		{
			RecordOffsets[ i - 1 ] = RecordOffsets[ i ] - DeletedRecordSize;
			RecordSizes[ i - 1 ] = RecordSizes[ i ];
		}

	// Update the record table count.
		--NumberOfRecords;
	}

// If we succeeded, update the record offset string.
	if( Status == HTAG_OK )
	{
		Status = BuildRecordStringFromRecordTrackingVariables();
	}

	return Status;
} // RemoveRecord






//
//	NumberOfRecordsInArray
//		Retrieve the number of records in the array
//
int HTAG_ARRAY::NumberOfRecordsInArray( void )
{
	return NumberOfRecords;
} // NumberOfRecordsInArray






//
//	SetMaximumNumberOfRecords
//		Set the maximum number of records in the array
//
void HTAG_ARRAY::SetMaximumNumberOfRecords( int NewMaximumNumberOfRecords )
{
	MaximumNumberOfRecordsAllowed = NewMaximumNumberOfRecords;
	while( NumberOfRecords > MaximumNumberOfRecordsAllowed )
	{
		RemoveRecord( NumberOfRecords - 1 );
	}

	return;
} // SetMaximumNumberOfRecords






//
//	MaximumNumberOfRecords
//		Retrieve the maximum number of records in the array
//
int HTAG_ARRAY::MaximumNumberOfRecords( void )
{
	return MaximumNumberOfRecordsAllowed;
} // MaximumNumberOfRecords







