//======================================================================================
// KimImage.cpp
//======================================================================================
// $RCSfile: kimimage.cpp $
// $Revision: 1.2 $
// $Date: 2010/01/06 18:04:27EST $
//======================================================================================

//======================================================================================








//---------------------------------------------------------------------------
//
//  Module Name:     KimImage
//
//---------------------------------------------------------------------------


#include	"HHPImagingConfig.h"
#include	"KimImage.h"		// KimImage interface definition

#include	<stdio.h>


//---------------------------------------------------------------------------






//
//	CreateKIMImage
//		Create an 8 or 1 bit per pixel KIM image from the input image.
//
bool CreateKIMImage(BITSTRING *KimImage, BITSTRING *InputImage, int XDimension, int YDimension, int NumberOfBits,
					int Threshold)
{
	int		i;
	unsigned int	LastPixelValue = 1000;
	unsigned int	PixelValue;
	int		Difference;
	bool	Status = true;			// Assume success until we know otherwise.


//	RESET_WATCHDOG_TIMER();

// Only create if X and Y dimensions are > 0 and their product equals the size of the input image.
	if((XDimension > 0) && (YDimension > 0) && ((XDimension * YDimension) == ((int) InputImage->Size)) &&
			((NumberOfBits == 8) || (NumberOfBits == 1)))
	{
		KimImage->Zero();
//		KimImage->MinimumResizeQuantity = (XDimension * YDimension) / 10;


	// Is it 8 bits per pixel?
		if(NumberOfBits == 8)
		{
			KimImage->Reallocate( ( 3 * XDimension * YDimension ) / 2 );
			for(i = 0;    i < (XDimension * YDimension);    ++i)
			{
				PixelValue = ((unsigned int) InputImage->Char[i]);
				Difference = ((int) PixelValue) - ((int) LastPixelValue);
				if((Difference > -8) && (Difference < 8))
				{
					KimImage->AppendBits(Difference, 4);
				}
				else
				{
					KimImage->AppendBits(8, 4);
					KimImage->AppendBits(((unsigned long) PixelValue) / 16, 4);
					KimImage->AppendBits(((unsigned long) PixelValue) % 16, 4);
				}

				LastPixelValue = PixelValue;
//				RESET_WATCHDOG_TIMER();
			}
			if(KimImage->BitPointer != 0x80)
			{
				KimImage->AppendBits(8, 0);
			}
			KimImage->AppendBits(13, 8);
			KimImage->AppendBits(10, 8);
		}

		else
		{
		// 1 bit per pixel
			bool	ColorIsWhite = true;
			int		NextRLECount = 0;
			int		NextPixel = 0;

		// First byte may specify color.
			if(InputImage->Char[0] < Threshold)
			{
				KimImage->AppendNumber(1, 1);
				ColorIsWhite = false;
			}

		// Treat a white pixel followed by a black pixel as a special case.  This would normally result in a white pixel count
		// of one, but since one in the first byte is used to change color, a white pixel count of 1 will confuse the host, so
		// we'll go ahead and set the first byte to one to indicate a black pixel and increment the pixel counts, effectively
		// making the first pixel black even though it was white (but at least we won't confuse the host).
			else if((InputImage->Size > 1) && (InputImage->Char[0] >= Threshold) && (InputImage->Char[1] < Threshold))
			{
				KimImage->AppendNumber(1, 1);
				++NextRLECount;
				++NextPixel;
			}

			while(NextPixel < ((int) InputImage->Size))
			{
			// If we're still processing the same color, increment the counter.
				if((InputImage->Char[NextPixel] >= Threshold) == ColorIsWhite)
				{
					++NextRLECount;
					++NextPixel;
				}

				else
				{
				// We switch colors, so we need to write out the length of the current color and start the counters for the
				// new color.
					while(NextRLECount > 255)
					{
						KimImage->AppendNumber(0, 1);
						NextRLECount -= 255;
					}
					KimImage->AppendNumber(NextRLECount, 1);

					ColorIsWhite = !ColorIsWhite;
					NextRLECount = 1;
					++NextPixel;
				}
//				RESET_WATCHDOG_TIMER();
			}

		// Finish up outputting the final count.
			while(NextRLECount > 255)
			{
				KimImage->AppendNumber(0, 1);
				NextRLECount -= 255;
			}
			KimImage->AppendNumber(NextRLECount, 1);
		}
	}

	else
	{
	// Dimensions are zero; WOOF!
		Status = false;
	}

//	RESET_WATCHDOG_TIMER();

	return(Status);
} // CreateKIMImage






//
//	CreateImageWithHeader
//		Create an 8 or 1 bit per pixel KIM or raw image from the input image, with a header on the beginning of it.
//
bool CreateImageWithHeader(BITSTRING *OutputImage, BITSTRING *InputImage, bool KimImage, int XDimension, int YDimension, int NumberOfBits,
			int Threshold, int LeftBoundary, int TopBoundary, int RightBoundary, int BottomBoundary, int PixelIncrement)
{
	bool	Status = true;			// Assume success until we know otherwise.


//	RESET_WATCHDOG_TIMER();

// Number of bits had better be 1 or 8.
	if((NumberOfBits != 1) && (NumberOfBits != 8))
	{
		Status = false;
	}

// Do the KIM compression if it is a KIM image.
	else if(KimImage)
	{
		Status = CreateKIMImage(OutputImage, InputImage, XDimension, YDimension, NumberOfBits, Threshold);
	}

// If raw binary image, convert to bitstream here.
	else if(NumberOfBits == 1)
	{
		OutputImage->ResetBitPointer();
		int			i, X, Y;
		int			NextBit;
		i = 0;
		for(Y = 0;    Y < YDimension;    ++Y)
		{
			for(X = 0;    X < XDimension;    ++X)
			{
				NextBit = (InputImage->Char[i] >= Threshold) ? 1 : 0;
				OutputImage->AppendBits(NextBit, 1);
				++i;
			}

		// Move to the next byte if we're not already there.
			OutputImage->FillToNextByteBoundary();
		}
	}

// Copy raw image.
	else
	{
		OutputImage->Copy(InputImage);
	}


	if(Status)
	{
	// Prepend the header.
		char	temp[80];

		sprintf(temp,"%04d%04d%04d%04d%04d", LeftBoundary, TopBoundary, RightBoundary, BottomBoundary, PixelIncrement);
		OutputImage->Insert(temp, 0);

		if(KimImage)
		{
			sprintf(temp,"]!!)%dbT(%04d%04d", NumberOfBits, XDimension, YDimension);
			OutputImage->Insert(temp, 0);
		}
		else
		{
			sprintf(temp,"]!!)%dbU(%04d%04d", NumberOfBits, XDimension, YDimension);
			OutputImage->Insert(temp, 0);
		}

	}

//	RESET_WATCHDOG_TIMER();

	return(Status);
} // CreateImageWithHeader





//
//	DecodeKIMImage
//		Decode an 8 or 1 bit per pixel KIM image into a raw output image.
//
bool DecodeKIMImage(BITSTRING *KimImage, BITSTRING *OutputImage, int XDimension, int YDimension, int NumberOfBits, int OffsetIntoInputImage)
{
	unsigned int	LastPixelValue = 0;
	unsigned int	PixelValue;
	int		Difference;
	bool	Status = true;			// Assume success until we know otherwise.


//	RESET_WATCHDOG_TIMER();

// Only create if X and Y dimensions are > 0.
	if((XDimension > 0) && (YDimension > 0) && (KimImage->Size > 0) && ((NumberOfBits == 8) || (NumberOfBits == 1)))
	{
		OutputImage->Zero();
		OutputImage->Reallocate( XDimension * YDimension );
		KimImage->ResetBitPointer();
		KimImage->BytePointer = OffsetIntoInputImage;

	// Is it 8 bits per pixel?
		if(NumberOfBits == 8)
		{
			while((!KimImage->EndReached) && (((int) OutputImage->Size) < (XDimension * YDimension)))
			{
				Difference = KimImage->CopyNextBits(4);

				if(Difference < 8)
				{
					PixelValue = Difference + LastPixelValue;
				}
				else if(Difference > 8)
				{
					PixelValue = (Difference - 16) + LastPixelValue;
				}
				else
				{
					PixelValue = KimImage->CopyNextBits(4) * 16;
					PixelValue += KimImage->CopyNextBits(4);
				}

				OutputImage->AppendNumber(PixelValue, 1);
				LastPixelValue = PixelValue;
			}
		}



		else
		{
		// 1 bit per pixel.  Default color is white, unless first byte is equal to one.
			int		Color = 255;
			int		NextInputByte = 0;

		// If the first byte is one, set the color to black.
			if(KimImage->Char[0] == 1)
			{
				Color = 0;
				++NextInputByte;
			}

		// Process the input image.
			while((NextInputByte < ((int) KimImage->Size)) && (((int) OutputImage->Size) < (XDimension * YDimension)))
			{
			// If we haven't reached the end of the RLE counter, fill in 255 bytes.
				if(KimImage->Char[NextInputByte] == 0)
				{
					OutputImage->AppendFillChar(Color, 255);
					++NextInputByte;
				}

			// We've reached the end of the RLE counter, so we need to fill in the # of bytes specified by the counter and
			// then switch colors.
				else
				{
					OutputImage->AppendFillChar(Color, ((unsigned int) KimImage->Char[NextInputByte]));
					Color = 255 - Color;
					++NextInputByte;
				}
			}
		}
	}

	else
	{
	// Dimensions are zero; WOOF!
		Status = false;
	}


// If we made it this far, then the output image size better be equal to the product of the dimensions.
	if((Status) && ((XDimension * YDimension) != ((int) OutputImage->Size)))
	{
		Status = false;
	}

//	RESET_WATCHDOG_TIMER();

	return(Status);
} // DecodeKIMImage






//
//	DecodeImageWithHeader
//		Decode an 8 or 1 bit per pixel KIM or raw image with header into a raw output image.  If no header is present,
//		treat it as a KIM image (the dimensions and number of bits must have been set by the caller).
//
bool DecodeImageWithHeader(BITSTRING *OutputImage, BITSTRING *InputImage, bool *KimImage, int *XDimension, int *YDimension, int *NumberOfBits,
			int *LeftBoundary, int *TopBoundary, int *RightBoundary, int *BottomBoundary, int *PixelIncrement)
{
	int		NextByteToProcess = 0;				// Next point in input image to process.
	int		NumberOfCharactersUsedInConversion;	// Number of bytes used when converting a numeric header parameter.
	bool	Status = true;			// Assume success until we know otherwise.



//	RESET_WATCHDOG_TIMER();

// If there aren't enough header bytes, WOOF!
	if(InputImage->Size < KIM_IMAGE_HEADER_SIZE)
		return false;


// Get 4 byte header.
	if(InputImage->CompareOnShorter(NextByteToProcess, (unsigned char *) "]!!)", 4, true) == 0)
	{
		NextByteToProcess += 4;

	// Get depth byte.
		if(Status)
		{
			if((InputImage->Char[NextByteToProcess] == '1') || (InputImage->Char[NextByteToProcess] == '8'))
			{
				*NumberOfBits = (int) (InputImage->Char[NextByteToProcess] - '0');

				if((*NumberOfBits == 8) || (*NumberOfBits == 1))
					++NextByteToProcess;
				else
					Status = false;
			}
			else
				Status = false;
		}
		

	// Get depth byte tag.
		if(Status)
		{
			if(InputImage->Char[NextByteToProcess] == 'b')
				++NextByteToProcess;
			else
				Status = false;
		}
	

	// Get image type tag.
		if(Status)
		{
			if(InputImage->Char[NextByteToProcess] == 'T')
			{
				*KimImage = true;
				++NextByteToProcess;
			}
			else if(InputImage->Char[NextByteToProcess] == 'U')
			{
				*KimImage = false;
				++NextByteToProcess;
			}
			else
				Status = false;
		}
		

	// Get closing parenthesis.
		if(Status)
		{
			if(InputImage->Char[NextByteToProcess] == '(')
				++NextByteToProcess;
			else
				Status = false;
		}
		

	// Get X-Dimension.
		if(Status)
		{
			*XDimension = InputImage->ConvertToNumber(NextByteToProcess, NextByteToProcess + 3, &NumberOfCharactersUsedInConversion);

			if(*XDimension > KIM_IMAGE_MAXIMUM_COLUMNS)
				Status = false;
			else if(NumberOfCharactersUsedInConversion == 4)
				NextByteToProcess += 4;
			else
				Status = false;
		}


	// Get Y-Dimension.
		if(Status)
		{
			*YDimension = InputImage->ConvertToNumber(NextByteToProcess, NextByteToProcess + 3, &NumberOfCharactersUsedInConversion);

			if(*YDimension > KIM_IMAGE_MAXIMUM_ROWS)
				Status = false;
			else if(NumberOfCharactersUsedInConversion == 4)
				NextByteToProcess += 4;
			else
				Status = false;
		}


	// Get left boundary.
		if(Status)
		{
			*LeftBoundary = InputImage->ConvertToNumber(NextByteToProcess, NextByteToProcess + 3, &NumberOfCharactersUsedInConversion);

			if(NumberOfCharactersUsedInConversion == 4)
				NextByteToProcess += 4;
			else
				Status = false;
		}


	// Get top boundary.
		if(Status)
		{
			*TopBoundary = InputImage->ConvertToNumber(NextByteToProcess, NextByteToProcess + 3, &NumberOfCharactersUsedInConversion);

			if(NumberOfCharactersUsedInConversion == 4)
				NextByteToProcess += 4;
			else
				Status = false;
		}


	// Get right boundary.
		if(Status)
		{
			*RightBoundary = InputImage->ConvertToNumber(NextByteToProcess, NextByteToProcess + 3, &NumberOfCharactersUsedInConversion);

			if(NumberOfCharactersUsedInConversion == 4)
				NextByteToProcess += 4;
			else
				Status = false;
		}


	// Get bottom boundary.
		if(Status)
		{
			*BottomBoundary = InputImage->ConvertToNumber(NextByteToProcess, NextByteToProcess + 3, &NumberOfCharactersUsedInConversion);

			if(NumberOfCharactersUsedInConversion == 4)
				NextByteToProcess += 4;
			else
				Status = false;
		}


	// Get pixel increment.
		if(Status)
		{
			*PixelIncrement = InputImage->ConvertToNumber(NextByteToProcess, NextByteToProcess + 3, &NumberOfCharactersUsedInConversion);

			if(NumberOfCharactersUsedInConversion == 4)
				NextByteToProcess += 4;
			else
				Status = false;
		}


	// If we've succeeded so far, decode the image.
		if(Status)
		{
			if(*KimImage)
			{
				Status = DecodeKIMImage(OutputImage, InputImage, *XDimension, *YDimension, *NumberOfBits, NextByteToProcess);
			}

			else
			{
			// Process raw data.
				if(*NumberOfBits == 1)
				{
				// Convert from binary.
					InputImage->ResetBitPointer();
					InputImage->BytePointer = NextByteToProcess;
					int			X, Y;
					OutputImage->Zero();
					OutputImage->Reallocate( (*XDimension) * (*YDimension) );
					for(Y = 0;    (Y < *YDimension) && (!InputImage->EndReached);    ++Y)
					{
						for(X = 0;    (X < *XDimension) && (!InputImage->EndReached);    ++X)
						{
							OutputImage->AppendChar((InputImage->CopyNextBits(1) == 1) ? 255 : 0);
						}

					// Move to the next byte if we're not already there.
						InputImage->MoveToNextByteBoundary();
					}

				// Make sure we didn't prematurely run out of bytes by checking the size of the output image.
					if((*XDimension * *YDimension) != (int) OutputImage->Size)
						Status = false;
				}
				else
				// Just do a copy if there are enough bytes to copy.
				{
					if((*XDimension * *YDimension) <= ( (int) InputImage->Size - NextByteToProcess ) )
						OutputImage->Copy(InputImage, NextByteToProcess, (*XDimension * *YDimension) +  NextByteToProcess - 1);
					else
						Status = false;
				}
			}
		}
	}

	else
	{
	// No header is present; decode as a KIM image.
		Status = DecodeKIMImage(OutputImage, InputImage, *XDimension, *YDimension, *NumberOfBits);
	}

//	RESET_WATCHDOG_TIMER();

	return(Status);
} // DecodeImageWithHeader





//
//	CreateKIMFile
//		Create an 8 bit per pixel KIM file from the input image.
//
bool CreateKIMFile( BITSTRING *KimFile, BITSTRING *InputImage, int XDimension, int YDimension )
{
	bool	Status = true;		// Assume success until we know otherwise.

	Status = CreateKIMImage( KimFile, InputImage, XDimension, YDimension );
	if(Status)
	{
		char	temp[80];
		sprintf( temp, "%04d%04d", XDimension, YDimension );
		KimFile->Insert( temp, 0 );
	}

	return Status;
} // CreateKIMFile



