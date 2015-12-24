//======================================================================================
// HTagImage.cpp
//======================================================================================
// $RCSfile: htagimage.cpp $
// $Revision: 1.3 $
// $Date: 2011/06/17 04:39:55EDT $
//======================================================================================

//======================================================================================


#include	"HHPCommunicationConfig.h"

#include	"HTagImage.h"		// HTAG_IMAGE interface definition

#include	<stdlib.h>		/* standard library functions				*/
#include	<stdio.h>
#include	<string.h>

//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//
//  Class Name:     HTAG_IMAGE
//
//  Class Description:
//		The HTAG_IMAGE class provides a mechanism to transfer images.  Since all fields are tagged, if the
//		receiver does not yet speak the new version of HTAG_IMAGE, it will ignore tags that it doesn't understand.
//
//---------------------------------------------------------------------------







//
// Constructor for HTAG_IMAGE; overloaded version #1 for loading images later.
//      Indicate that no image has been loaded yet by setting Data size to zero.
//
HTAG_IMAGE::HTAG_IMAGE(const char *Command, HCOMM *HCommDevice, HTRANSFER_STATUS *TransferStatus,
								HCOMMAND_HANDLER *CommandHandler)
		: HTAG( HCommDevice, TransferStatus, CommandHandler, false )
{
	Data = new BITSTRING(DEFAULT_IMAGE_DATA_ALLOCATION, true);
	HTAG::Data = Data;				// Give the pointer to the base class so it can operate on the data as well.
	CommandName->Copy(Command);


// Adjust minimum transfer protocol parameter value, created by HTAG, to be 0.
	Parameters[ TransferProtocolParameterNumber ].LowRange = 0;


	ImageName = new HSTRING(20, true);
//	AddHStringParameter(HTAG_IMAGE_FILE_PARAMETER_NAME, ImageName, "UNKNOWN", 7, false );
	AddNumericParameter(HTAG_IMAGE_LEFT_BOUNDARY_PARAMETER_NAME, &LeftBoundary, HTAG_IMAGE_MINIMUM_IMAGE_BOUNDARY,
					HTAG_IMAGE_MINIMUM_IMAGE_BOUNDARY, HTAG_IMAGE_MAXIMUM_IMAGE_BOUNDARY, true );
	AddNumericParameter(HTAG_IMAGE_RIGHT_BOUNDARY_PARAMETER_NAME, &RightBoundary, HTAG_IMAGE_MINIMUM_IMAGE_BOUNDARY,
					HTAG_IMAGE_MINIMUM_IMAGE_BOUNDARY, HTAG_IMAGE_MAXIMUM_IMAGE_BOUNDARY, true );
	AddNumericParameter(HTAG_IMAGE_BOTTOM_BOUNDARY_PARAMETER_NAME, &BottomBoundary, HTAG_IMAGE_MAXIMUM_IMAGE_BOUNDARY,
					HTAG_IMAGE_MINIMUM_IMAGE_BOUNDARY, HTAG_IMAGE_MAXIMUM_IMAGE_BOUNDARY, true );
	AddNumericParameter(HTAG_IMAGE_TOP_BOUNDARY_PARAMETER_NAME, &TopBoundary, HTAG_IMAGE_MINIMUM_IMAGE_BOUNDARY,
					HTAG_IMAGE_MINIMUM_IMAGE_BOUNDARY, HTAG_IMAGE_MAXIMUM_IMAGE_BOUNDARY, true );

	AddNumericParameter(HTAG_IMAGE_MARGIN_PARAMETER_NAME, &Margin, HTAG_IMAGE_MINIMUM_IMAGE_BOUNDARY,
					HTAG_IMAGE_MINIMUM_IMAGE_BOUNDARY, HTAG_IMAGE_MAXIMUM_IMAGE_BOUNDARY, true );

	AddNumericParameter(HTAG_IMAGE_DEPTH_PARAMETER_NAME, &Depth, HTAG_IMAGE_GRAYSCALE_IMAGE_DEPTH,
					HTAG_IMAGE_BINARY_IMAGE_DEPTH, HTAG_IMAGE_GRAYSCALE_IMAGE_DEPTH, true );

	AddNumericParameter(HTAG_IMAGE_PIXEL_INCREMENT_PARAMETER_NAME, &PixelIncrement, HTAG_IMAGE_MINIMUM_PIXEL_INCREMENT,
					HTAG_IMAGE_MINIMUM_PIXEL_INCREMENT, HTAG_IMAGE_MAXIMUM_PIXEL_INCREMENT, true );

	AddNumericParameter(HTAG_IMAGE_FLATTENING_PARAMETER_NAME, &ImageFlatteningApplied, 0, 0, 1, false );
	AddNumericParameter(HTAG_IMAGE_EDGE_ENHANCEMENT_PARAMETER_NAME, &EdgeEnhancementApplied, 0, 0, 0x7FFFFFFF, false );
	AddNumericParameter(HTAG_IMAGE_SHIP_ROTATION_PARAMETER_NAME, &RotationApplied, 0, 0, 3, false );
	AddNumericParameter(HTAG_IMAGE_HISTOGRAM_STRETCH_PARAMETER_NAME, &HistogramStretchApplied, 0, 0, 1, false );

// While HTAG_IMAGE_X_AND_Y_AXIS_INVERSION_PARAMETER_NAME directly affects CompleteInversionApplied, the intent is for it to
// control XAxisInversionApplied and YAxisInversionApplied simultaneously, but since HTAG doesn't allow us to control more than
// one variable per parameter, we'll use the post process routine to specifically handle this parameter.
	AddNumericParameter(HTAG_IMAGE_X_AND_Y_AXIS_INVERSION_PARAMETER_NAME, &CompleteInversionApplied, 0, 0, 1, false );

	AddNumericParameter(HTAG_IMAGE_X_AXIS_INVERSION_PARAMETER_NAME, &XAxisInversionApplied, 0, 0, 1, false );
	AddNumericParameter(HTAG_IMAGE_Y_AXIS_INVERSION_PARAMETER_NAME, &YAxisInversionApplied, 0, 0, 1, false );
	AddNumericParameter(HTAG_IMAGE_HISTOGRAM_EQUALIZATION_PARAMETER_NAME, &HistogramEqualizationApplied, 0, 0, 1, false );
	AddNumericParameter(HTAG_IMAGE_SMOOTHING_PARAMETER_NAME, &SmoothingApplied, 0, 0, 1, false );
	AddNumericParameter(HTAG_IMAGE_BINARIZATION_THRESHOLDS_PARAMETER_NAME, &BinarizationThresholdsApplied, 0, 0, 1, false );
   AddNumericParameter(HTAG_IMAGE_ULTRATEXT_PARAMETER_NAME, &UltraTextApplied, 0, 0, 255, false );
   AddNumericParameter(HTAG_IMAGE_ADAPTIVEULTRATEXT_PARAMETER_NAME, &AdaptiveUltraTextApplied, 0, 0, 255, false );
	AddNumericParameter(HTAG_IMAGE_INFINITY_FILTER_PARAMETER_NAME, &InfinityFilterApplied, 0, 0, 1, false );
	AddNumericParameter(HTAG_IMAGE_GAMMA_PARAMETER_NAME, &GammaCorrectionApplied, 0, 0, 1000, false );
	AddNumericParameter(HTAG_IMAGE_SHIP_HISTOGRAM_PARAMETER_NAME, &HistogramApplied, 0, 0, 1, false );

	AddNumericParameter(HTAG_IMAGE_FORMAT_PARAMETER_NAME, &Format, HTAG_IMAGE_UNFORMATTED_GRAYSCALE, 0,
					END_HTAG_IMAGE_FORMATS - 1, true );

	AddNumericParameter(HTAG_IMAGE_JPEG_QUALITY_FACTOR_PARAMETER_NAME, &JPEGQualityFactor,
					HTAG_IMAGE_DEFAULT_JPEG_QUALITY_FACTOR, HTAG_IMAGE_MINIMUM_JPEG_QUALITY_FACTOR,
					HTAG_IMAGE_MAXIMUM_JPEG_QUALITY_FACTOR, false );

	AddNumericParameter(HTAG_IMAGE_GAIN_PARAMETER_NAME, &GainCapturedAt, 0, 0, 0x7FFFFFFF, false );
	AddNumericParameter(HTAG_IMAGE_EXPOSURE_PARAMETER_NAME, &ExposureCapturedAt, 0, 0, 0x7FFFFFFF, false );
	AddNumericParameter(HTAG_IMAGE_OBSERVED_WHITE_VALUE_PARAMETER_NAME, &ObservedWhiteValue, 0, 0, 0x7FFFFFFF, false );
	AddNumericParameter(HTAG_IMAGE_FRAME_RATE_PARAMETER_NAME, &FrameRateCaptureAt, 0, 0, 0x7FFFFFFF, false );

	AddNumericParameter(HTAG_IMAGE_SHIP_FLYSPECK_PARAMETER_NAME, &FlySpeckApplied, 0, 0, 9999, false );
   AddNumericParameter(HTAG_IMAGE_SHIP_SNAZZY_PARAMETER_NAME, &SnazzyApplied, 0, 0, 9999, false );
	AddNumericParameter(HTAG_IMAGE_SHIP_GLITCHFIX_PARAMETER_NAME, &GlitchFixApplied, 0, 0, 9999, false );
	AddNumericParameter(HTAG_IMAGE_SHIP_ROWNOISE_PARAMETER_NAME, &RowNoiseApplied, 0, 0, 1, false );

	XDimension = 0;
	YDimension = 0;

    return;
}
//---------------------------------------------------------------------------





//
// Destructor for HTAG_IMAGE
//      If memory has been allocated to an image, release it.
//
HTAG_IMAGE::~HTAG_IMAGE(void)
{
	delete Data;
	delete ImageName;

    return;
}
//---------------------------------------------------------------------------





//
//	PerformAdditionalParameterValidation
//		Validate new parameters to see if they're in bounds.
//
void HTAG_IMAGE::PerformAdditionalParameterValidation(int *Status)
{
// Boundaries must be left <= right and top <= bottom (assumes origin is upper left).
	if((LeftBoundary > RightBoundary) || (TopBoundary > BottomBoundary))
	{
		*Status = HTAG_IMAGE_INVALID_BOUNDARY_COORDINATES;
	}

// Check image depth
	else if((Depth != HTAG_IMAGE_BINARY_IMAGE_DEPTH) && (Depth != HTAG_IMAGE_GRAYSCALE_IMAGE_DEPTH))
	{
		*Status = HTAG_IMAGE_INVALID_DEPTH;
	}

// Check margin (must not be greater than the distance to center from any edge).
	else if((Margin < 0) || (Margin > ((RightBoundary - LeftBoundary) / 2)) ||
				(Margin > ((BottomBoundary - TopBoundary) / 2)))
	{
		*Status = HTAG_IMAGE_INVALID_MARGIN;
	}

    return;
} // PerformAdditionalParameterValidation







//
//	PostProcessParameterAfterParsing
//		Process each parameter as it's parsed.
//
bool HTAG_IMAGE::PostProcessParameterAfterParsing(const char *NameTag, int * /* Status */)
{
	bool	ReturnStatus = true;


// We're only going to post process one process: "I" (invert both axes).  If this parameter was specified, copy it's value to
// both the X and Y axis inversion parameters.
	if(HSTRING::hhpstricmp(NameTag, "I", true) == 0)
	{
		XAxisInversionApplied = CompleteInversionApplied;
		YAxisInversionApplied = CompleteInversionApplied;
	}


    return(ReturnStatus);
} // PostProcessParameterAfterParsing





//
//	PerformAdditionalOperationsAfterParsing
//		After parameters have been parsed by the base class, set XDimension and YDimension.
//
void HTAG_IMAGE::PerformAdditionalOperationsAfterParsing(int *Status)
{
	if((*Status == HTAG_OK) || (*Status == HTAG_UNRECOGNIZED_PARAMETER_NAME) || (*Status < 0))
//	if(*Status >= HTAG::HTAG_OK)
	{
		XDimension = (RightBoundary - LeftBoundary - (2 * Margin) + 1) / PixelIncrement;
		YDimension = (BottomBoundary - TopBoundary - (2 * Margin) + 1) / PixelIncrement;

		if((XDimension < 1) || (YDimension < 1))
		{
			*Status = HTAG_IMAGE_PARSE_BOUNDARY_COORDINATES_AND_MARGIN_OUT_OF_RANGE;
		}
	}

	return;
} // PerformAdditionalOperationsAfterParsing





//
//	PreCreateTaggedImage
//		Create a tagged image in an empty string sent by the caller if the base class can't create the tagged image itself
//		because of implementation specific protocol (TransferProtocol < HTAG_TRANSMIT_NO_PROTOCOL).
//
//		If this routine runs out of space in the string sent by the caller, it will return false, else it will return
//		true.
//
bool HTAG_IMAGE::PreCreateTaggedImage( HSTRING *TaggedImage )
{
	bool	ReturnStatus = true;	        // Returns false if we ran out of space; true otherwise
	char	temp[80];


// Null out the tagged data.
	TaggedImage->Copy("");


// Are we building the output the new (Matrix) way or the old (44XX) way?
	if(TransferProtocol < HTAG_TRANSMIT_NO_PROTOCOL)
	{
	// Build the old style
		switch(Format)
		{
			case HTAG_IMAGE_KIM:
			case HTAG_IMAGE_OUTLINED_IMAGE:
				sprintf(temp,"]!!)%dbT(%04d%04d", Depth, XDimension, YDimension);
				TaggedImage->Append(temp);
				sprintf(temp,"%04d%04d%04d%04d%04d", LeftBoundary, TopBoundary,
									RightBoundary, BottomBoundary, PixelIncrement);
				TaggedImage->Append(temp);
				break;

			case HTAG_IMAGE_UNFORMATTED_BINARY:
				sprintf(temp,"]!!)1bU(%04d%04d", XDimension, YDimension);
				TaggedImage->Append(temp);
				sprintf(temp,"%04d%04d%04d%04d%04d", LeftBoundary, TopBoundary,
							RightBoundary, BottomBoundary, PixelIncrement);
				TaggedImage->Append(temp);
				break;

			case HTAG_IMAGE_UNFORMATTED_GRAYSCALE:
				sprintf(temp,"]!!)8bU(%04d%04d", XDimension, YDimension);
				TaggedImage->Append(temp);
				sprintf(temp,"%04d%04d%04d%04d%04d", LeftBoundary, TopBoundary,
							RightBoundary, BottomBoundary, PixelIncrement);
				TaggedImage->Append(temp);
				break;

			case HTAG_IMAGE_TIF_BINARY:
			case HTAG_IMAGE_TIF_GRP:
			case HTAG_IMAGE_TIF_GRAYSCALE:
			case HTAG_IMAGE_JPEG:
			case HTAG_IMAGE_BMP:
			case HTAG_IMAGE_HUFFMAN:
				break;

			default:
				ReturnStatus = false;
				break;
		}
	}


	else
	{
	// Return false, because this should only be done for old protocol.
		ReturnStatus = false;
	}


    return (ReturnStatus);
} // PreCreateTaggedImage






//
//	PreLoadTaggedImage
//		Load the local image with a tagged image contained in the string sent by the caller.  The tagged image is simply a string
//		that follows this format:
//
//		The following steps are taken:
//			- Tagged fields are extracted until the end of the header is reached.
//          - The remaining data after the header is assumed to be image and is copied to local image data storage.
//
//		Returned status will reflect one of the following cases:
//			- A field was incorrectly labelled.
//			- No comma was found.
//
bool HTAG_IMAGE::PreLoadTaggedImage(HSTRING *TaggedImage, int *Status)
{
	bool	ReturnStatus = true;		// Indicate that pre-processing was performed until we know otherwise.
	int		NextLocationToRead;			// Pointer to next place in tagged image from which to extract a tagged field.
	int		i;


// Indicate that the format is unknown.
	Format = END_HTAG_IMAGE_FORMATS;

// Are we processing an old protocol KIM, uncompressed, or outlined image?
	if((TaggedImage->Size > 36) &&
				(TaggedImage->CompareOnShorter(((unsigned char *) "]!!)"), 4, false) == 0) &&
				((TaggedImage->Char[4] == '1') || (TaggedImage->Char[4] == '8')) &&
				(TaggedImage->Char[5] == 'b') &&
				((TaggedImage->Char[6] == 'T') || (TaggedImage->Char[6] == 'U')) &&
				(TaggedImage->Char[7] == '('))
	{
	// This is an old protocol KIM, uncompressed, or outlined image.
		Depth = ((int) (TaggedImage->Char[4] - '0'));
		if(TaggedImage->Char[6] == 'T')
		{
		// There is room for ambiguity here.  A binary compressed image could be KIM or outlined.  The only way to know for
		// sure is when we get the command parameters at the end.
			Format = HTAG_IMAGE_KIM;
		}
		else if(Depth == 1)
		{
			Format = HTAG_IMAGE_UNFORMATTED_BINARY;
		}
		else
		{
			Format = HTAG_IMAGE_UNFORMATTED_GRAYSCALE;
		}

	// Remove the old protocol header.
		NextLocationToRead = 8;

	// Let's set the protocol to non-Dolphin, until we parse the IMGHSHP header.
		TransferProtocol = HTAG_TRANSMIT_NO_PROTOCOL_HEADER_AT_END;

	// Now we'll decode the parameters sent with the header.  We'll first check to be sure that the entire block is numeric.
		i = 0;
		while((i < 28) && (TaggedImage->Char[i + NextLocationToRead] >= '0') &&
						(TaggedImage->Char[i + NextLocationToRead] <= '9'))
		{
			++i;
		}

		if(i < 28)
		{
		// If the entire parameter block was not numeric, WOOF!
			*Status = HTAG_INVALID_FORMAT;
		}

		else
		{
		// Decode the parameter block.
			sscanf(((char *) &(TaggedImage->Char[NextLocationToRead])), "%04d%04d%04d%04d%04d%04d%04d",
						&XDimension,
						&YDimension,
						&LeftBoundary,
						&TopBoundary,
						&RightBoundary,
						&BottomBoundary,
						&PixelIncrement);
			NextLocationToRead += 28;


		// Validate X and Y dimensions.
			if((XDimension == ((RightBoundary - LeftBoundary + 1) / PixelIncrement)) &&
							(YDimension == ((BottomBoundary - TopBoundary + 1) / PixelIncrement)))
			{
			// Next, let's find the command header (if there is one) and parse it.
				int		SizeOfHeader = 0;
				*Status = FindAndParseTrailingHeader(TaggedImage, &SizeOfHeader);


			// If there haven't been any errors, let's store the data.
				if((*Status == HTAG_OK) || (*Status == HTAG_UNRECOGNIZED_PARAMETER_NAME) || (*Status < 0))
				{
					Data->Reallocate(XDimension * YDimension);
					Data->Copy(&TaggedImage->Char[NextLocationToRead], TaggedImage->Size - NextLocationToRead - SizeOfHeader);
				}
			}
			else
			{
			// X and Y dimensions are wrong; WOOF!
				*Status = HTAG_IMAGE_INVALID_BOUNDARY_COORDINATES;
			}
		}
	}


// Are we processing an old protocol JPEG image?
	else if((TaggedImage->Size > 4) &&
				(TaggedImage->CompareOnShorter(((unsigned char *) "\xFF\xD8\xFF\xE0"), 4, false) == 0))
	{
	// This is an old protocol JPEG image.
		Depth = 8;
		Format = HTAG_IMAGE_JPEG;

	// By default, the protocol is non-Dolphin.
		TransferProtocol = HTAG_TRANSMIT_NO_PROTOCOL_HEADER_AT_END;

	// Next, let's find the command header (if there is one) and parse it.
		int		SizeOfHeader = 0;
		*Status = FindAndParseTrailingHeader(TaggedImage, &SizeOfHeader);


	// If there haven't been any errors, let's store the data.
		if((*Status == HTAG_OK) || (*Status == HTAG_UNRECOGNIZED_PARAMETER_NAME) || (*Status < 0))
		{
			Data->Reallocate(XDimension * YDimension);
			Data->Copy(TaggedImage->Char, TaggedImage->Size - SizeOfHeader);
		}
	}


// Are we processing an old protocol BMP image?
	else if((TaggedImage->Size > 2) &&
				(TaggedImage->CompareOnShorter(((unsigned char *) "BM"), 2, false) == 0))
	{
	// This is an old protocol BMP image.
		Depth = 8;
		Format = HTAG_IMAGE_BMP;

	// By default, the protocol is non-Dolphin.
		TransferProtocol = HTAG_TRANSMIT_NO_PROTOCOL_HEADER_AT_END;

	// Next, let's find the command header (if there is one) and parse it.
		int		SizeOfHeader = 0;
		*Status = FindAndParseTrailingHeader(TaggedImage, &SizeOfHeader);


	// If there haven't been any errors, let's store the data.
		if((*Status == HTAG_OK) || (*Status == HTAG_UNRECOGNIZED_PARAMETER_NAME) || (*Status < 0))
		{
			Data->Reallocate(XDimension * YDimension);
			Data->Copy(TaggedImage->Char, TaggedImage->Size - SizeOfHeader);
		}
	}


// Are we processing an old protocol TIFF image?
	else if((TaggedImage->Size > 2) &&
				(TaggedImage->CompareOnShorter(((unsigned char *) "\x49\x49"), 2, false) == 0))
	{
	// This is an old protocol TIFF image.  We need to parse the image header to determine the TIFF image type.
		Depth = 8;

// ******** Need to decode parameters here to determine
		Format = HTAG_IMAGE_TIF_GRAYSCALE;

	// By default, the protocol is non-Dolphin.
		TransferProtocol = HTAG_TRANSMIT_NO_PROTOCOL_HEADER_AT_END;

	// Next, let's find the command header (if there is one) and parse it.
		int		SizeOfHeader = 0;
		*Status = FindAndParseTrailingHeader(TaggedImage, &SizeOfHeader);


	// If there haven't been any errors, let's store the data.
		if((*Status == HTAG_OK) || (*Status == HTAG_UNRECOGNIZED_PARAMETER_NAME) || (*Status < 0))
		{
			Data->Reallocate(XDimension * YDimension);
			Data->Copy(TaggedImage->Char, TaggedImage->Size - SizeOfHeader);
		}
	}



// We're processing a new protocol header (>= 2).  Return false to indicate that we did no processing.
	else
	{
		ReturnStatus = false;
	}


	return(ReturnStatus);
} // PreLoadTaggedImage




