//======================================================================================
// HImage.cpp
//======================================================================================
// $RCSfile: himage.cpp $
// $Revision: 1.7 $
// $Date: 2011/07/03 22:57:41EDT $
//======================================================================================

//======================================================================================



//---------------------------------------------------------------------------
//
//  Class Name:     HIMAGE
//
//  Class Description:
//		The HIMAGE class provides a mechanism to manipulate and compress / decompress images.  This class also
//		provides the ability to format images for transfer by tagging them in such a way as to minimize version
//		change problems.  Since all fields are tagged, if the receiver does not yet speak the new version of HIMAGE,
//		it will ignore tags that it doesn't understand.
//
//---------------------------------------------------------------------------


#include	<stdlib.h>		/* standard library functions				*/
#include	<stdio.h>
#include	<string.h>
#include	<math.h>


#include	"HHPImagingConfig.h"
#include	"HImage.h"				// HIMAGE interface definition
#include	"Compression.h"		// Compression routine interface
#include	"BmpImage.h"			// BmpImage interface definition
#include	"KimImage.h"			// KimImage interface definition
#include	"TiffImage.h"			// TiffImage interface definition

#include "HardwareToken.h"
#include "Menu.h"
#include "ProductConfiguration.h"

//---------------------------------------------------------------------------



//
//	UnformatHTagImage
//		Unformat an HTAG_IMAGE  (convert it to a raw image) according to the format setting.
bool UnformatHTagImage(HTAG_IMAGE *HTagImage, bool FastJPEG, bool JPEGDecompressionBlockSmoothing)
{
    bool        ReturnStatus = true;

// Decompress the image, returning the decompression status.
   if((HTagImage->Format < 0) || (HTagImage->Format >= HTAG_IMAGE::END_HTAG_IMAGE_FORMATS))
	{
	// Illegal format; WOOF>
		ReturnStatus = false;
	}
	else
   {
		// Convert the image.
		switch(HTagImage->Format)
		{
			case HTAG_IMAGE::HTAG_IMAGE_KIM:
				{
				BITSTRING	KimImage(HTagImage->Data->Size, true);
				KimImage.Copy(HTagImage->Data);
				bool	KimImageType;
				int		KimLeftBoundary;
				int		KimTopBoundary;
				int		KimRightBoundary;
				int		KimBottomBoundary;
				int		KimPixelIncrement;
				ReturnStatus = DecodeImageWithHeader(&KimImage, HTagImage->Data, &KimImageType, &HTagImage->XDimension, &HTagImage->YDimension, &HTagImage->Depth,
						&KimLeftBoundary, &KimTopBoundary, &KimRightBoundary, &KimBottomBoundary, &KimPixelIncrement);
				}
				break;

			case HTAG_IMAGE::HTAG_IMAGE_OUTLINED_IMAGE:
				{
				BITSTRING	KimImage(HTagImage->Data->Size, true);
				KimImage.Copy(HTagImage->Data);
				HTagImage->Depth = HTAG_IMAGE_BINARY_IMAGE_DEPTH;
				bool	KimImageType;
				int		KimLeftBoundary;
				int		KimTopBoundary;
				int		KimRightBoundary;
				int		KimBottomBoundary;
				int		KimPixelIncrement;
				ReturnStatus = DecodeImageWithHeader(&KimImage, HTagImage->Data, &KimImageType, &HTagImage->XDimension, &HTagImage->YDimension, &HTagImage->Depth,
						&KimLeftBoundary, &KimTopBoundary, &KimRightBoundary, &KimBottomBoundary, &KimPixelIncrement);
				}
				break;

			case HTAG_IMAGE::HTAG_IMAGE_TIF_BINARY:
				{
				int		TiffXDimension;
				int		TiffYDimension;
				int		TiffDepth;
				int		TiffCompression;
				HSTRING	TiffMessage(20, true);
				BITSTRING	TiffImage(HTagImage->Data->Size, true);
				TiffImage.Copy(HTagImage->Data);
				HTagImage->Depth = HTAG_IMAGE_BINARY_IMAGE_DEPTH;
				ReturnStatus = DecodeTiffImage(&TiffImage, HTagImage->Data, &TiffXDimension, &TiffYDimension, &TiffDepth, &TiffCompression,
									&TiffMessage);
				}
				break;

			case HTAG_IMAGE::HTAG_IMAGE_TIF_GRP:
				{
				int		TiffXDimension;
				int		TiffYDimension;
				int		TiffDepth;
				int		TiffCompression;
				HSTRING	TiffMessage(20, true);
				BITSTRING	TiffImage(HTagImage->Data->Size, true);
				TiffImage.Copy(HTagImage->Data);
				HTagImage->Depth = HTAG_IMAGE_BINARY_IMAGE_DEPTH;
				ReturnStatus = DecodeTiffImage(&TiffImage, HTagImage->Data, &TiffXDimension, &TiffYDimension, &TiffDepth, &TiffCompression,
									&TiffMessage);
				}
				break;

			case HTAG_IMAGE::HTAG_IMAGE_TIF_GRAYSCALE:
				{
				int		TiffXDimension;
				int		TiffYDimension;
				int		TiffDepth;
				int		TiffCompression;
				HSTRING	TiffMessage(20, true);
				BITSTRING	TiffImage(HTagImage->Data->Size, true);
				TiffImage.Copy(HTagImage->Data);
				HTagImage->Depth = HTAG_IMAGE_GRAYSCALE_IMAGE_DEPTH;
				ReturnStatus = DecodeTiffImage(&TiffImage, HTagImage->Data, &TiffXDimension, &TiffYDimension, &TiffDepth, &TiffCompression,
									&TiffMessage);
				}
				break;

			case HTAG_IMAGE::HTAG_IMAGE_UNFORMATTED_BINARY:
				{
				BITSTRING	FormattedBinaryImage(HTagImage->Data->Size, true);
				int			X, Y;
				FormattedBinaryImage.Copy(HTagImage->Data);
				HTagImage->Data->Zero();
				HTagImage->Data->Reallocate( HTagImage->XDimension * HTagImage->YDimension );

				for(Y = 0;    Y < HTagImage->YDimension;    ++Y)
				{
					for(X = 0;    X < HTagImage->XDimension;    ++X)
					{
						HTagImage->Data->AppendChar((FormattedBinaryImage.CopyNextBits(1) == 1) ? 255 : 0);
					}

				// Move to the next byte if we're not already there.
					FormattedBinaryImage.MoveToNextByteBoundary();
				}
				}
				break;

			case HTAG_IMAGE::HTAG_IMAGE_UNFORMATTED_GRAYSCALE:
			// Don't do anything, since this is the format we want the data to be in.
				break;

			case HTAG_IMAGE::HTAG_IMAGE_JPEG:
				{
				int	CompressionMode = (FastJPEG) ? JPEG_LOSSY_FAST_1_0 : JPEG_LOSSY_1_0;
			    ReturnStatus = DecompressData(HTagImage->Data, CompressionMode, JPEGDecompressionBlockSmoothing);
				}
				break;

			case HTAG_IMAGE::HTAG_IMAGE_BMP:
				{
				BITSTRING	BmpImage(HTagImage->Data->Size, true);
				BmpImage.Copy(HTagImage->Data);
				int		NewXDimension, NewYDimension, NewDepth;
				ReturnStatus = DecodeBMPImage(&BmpImage, HTagImage->Data, &NewXDimension, &NewYDimension, &NewDepth);

				if((HTagImage->XDimension != NewXDimension) || (HTagImage->YDimension != NewYDimension) || (HTagImage->Depth != NewDepth))
				{
					ReturnStatus = false;
				}
				}
				break;

			case HTAG_IMAGE::HTAG_IMAGE_HUFFMAN:
			    ReturnStatus = DecompressData(HTagImage->Data, HUFFMAN_VERSION_2_0, JPEGDecompressionBlockSmoothing);
		}	//end switch

	   if(ReturnStatus)
		{
			HTagImage->Format = HTAG_IMAGE::HTAG_IMAGE_UNFORMATTED_GRAYSCALE;
		}
    }

    return(ReturnStatus);
} // UnformatHTagImage




//
//	SetImageParameterDefaults
//		Set image parameters to defaults values.
//
void HIMAGE::SetImageParameterDefaults(void)
{
// Let's set the defaults.
	XDimension = 0;
	YDimension = 0;
	JPEGCompressionSmoothingFactor = HIMAGE_DEFAULT_JPEG_SMOOTHING_FACTOR;
	JPEGDecompressionBlockSmoothing = HIMAGE_DEFAULT_JPEG_BLOCK_SMOOTHING;
	FastJPEG = true;

// Reset the HTAG parameters.
	SetParameterDefaults();

	return;
} // SetImageParameterDefaults




//
// Constructor for HIMAGE; overloaded version #1 for loading images later.
//      Indicate that no image has been loaded yet by setting row and column
//      counts to zero.
//
//      By default, there is no compression.
//
HIMAGE::HIMAGE(void)
		: HTAG_IMAGE()
{
	Threshold = DEFAULT_THRESHOLD;
	SmoothOnSubsampling = false;

	SetImageParameterDefaults();

    return;
}
//---------------------------------------------------------------------------




//
// Constructor for HIMAGE; overloaded version #2 for loading an image at instantiation.
//      Indicate that an image has been loaded, allocating space for the input image,
//      copying it, and setting row and column counts.
//
//      By default, there is no compression.
//
HIMAGE::HIMAGE(unsigned char *ImageData, int InputXDimension, int InputYDimension)
		: HTAG_IMAGE()
{
	Threshold = DEFAULT_THRESHOLD;

	SetImageParameterDefaults();

	XDimension = InputXDimension;
	YDimension = InputYDimension;
	LeftBoundary = 0;
	RightBoundary = XDimension - 1;
	TopBoundary = 0;
	BottomBoundary = YDimension - 1;

	if(ValidateParameters() == HTAG_OK)
	{
		Data->Reallocate(XDimension * YDimension);
		Data->Size = XDimension * YDimension;
		FastMemoryCopy(Data->Char, ImageData, Data->Size);
   }

   return;
}
//---------------------------------------------------------------------------




//
// Constructor for HIMAGE; overloaded version #3 for loading an image at instantiation.
//      The difference between this and overloaded version #2 is that this overloaded version
//  	takes a ROI based on passed arguments to create the image rather than the
//		dealing in the entire image.
//
//      By default, there is no compression.
//
HIMAGE::HIMAGE(unsigned char *ImageData, int InputXDimension, int /* InputYDimension */, int x1, int x2, int y1, int y2)
		: HTAG_IMAGE()
{
   int       y;      // Index

	Threshold = DEFAULT_THRESHOLD;

	SetImageParameterDefaults();

	LeftBoundary = x1;
	RightBoundary = x2;
	TopBoundary = y1;
	BottomBoundary = y2;
	XDimension = RightBoundary - LeftBoundary + 1;
	YDimension = BottomBoundary - TopBoundary + 1;

	if((y2 <= YDimension) && (ValidateParameters() == HTAG_OK))
	{
		Data->Reallocate(XDimension * YDimension);
		Data->Size = XDimension * YDimension;

		for(y=y1; y < y2; y++)
		{
			Data->Append(&ImageData[x1 + (y * InputXDimension)], x2 - x1);
		}
	}

    return;
}
//---------------------------------------------------------------------------




//
// Constructor for HIMAGE; overloaded version #4 for loading a tagged image at instantiation.
//      Indicate that no image has been loaded yet by setting row and column
//      counts to zero.  Then, try to load the new image.
//
//      By default, there is no compression.
//
HIMAGE::HIMAGE(HSTRING *ImageData)
		: HTAG_IMAGE()
{
	Threshold = DEFAULT_THRESHOLD;

	SetImageParameterDefaults();

	LoadTaggedImage(ImageData);

    return;
}
//---------------------------------------------------------------------------




//
// Destructor for HIMAGE
//      Nothing to do.
//
HIMAGE::~HIMAGE(void)
{
    return;
}
//---------------------------------------------------------------------------





//
//	SwapDataArrays
//		Swap two data arrays.  If data addresses and size are int aligned, do an int swap for speed (swapping takes
//		better advantage of caching than copying to another buffer first); otherwise, do a byte swap.
//
void SwapDataArrays(void *Data1, void *Data2, int Size)
{
// Do a byte move if size or either data addresses are not int aligned.
	if((Size % sizeof(int)) ||
		(((int) Data1) % sizeof(int)) ||
		(((int) Data2) % sizeof(int)))
	{
		register unsigned char	TempData;
		register unsigned char	*ucData1 = (unsigned char *) Data1;
		register unsigned char	*ucData2 = (unsigned char *) Data2;
		register int	CopySize = Size;

		do
		{
			TempData = *ucData1;
			*ucData1++ = *ucData2;
			*ucData2++ = TempData;
		}  while(--CopySize);
	} // Size or addresses were not int aligned

// Do an int move since everything is int aligned.
	else
	{
		register int	TempData;
		register int	*iData1 = (int *) Data1;
		register int	*iData2 = (int *) Data2;
		register int	CopySize = Size / sizeof(int);

		do
		{
			TempData = *iData1;
			*iData1++ = *iData2;
			*iData2++ = TempData;
		}  while(--CopySize);
	} // Size or addresses were int aligned

	return;
} // SwapDataArrays




//
//	ModifyImage
//		Subsample, window, and / or invert image either over the same image or to a new image.
//		Considering all the possible combinations of changes based on the input parameters,
//		we'll try to do the modifications as fast as we can for each given case.  Note that this
//		routine assumes that input and output image are either the same or don't overlap; we woof
//		if they do overlap.
//
//		The caller can also request that smoothing be applied as data is being subsampled.  In cases where
//		the input and output image are the same, and the data is being subsampled and smoothed, the new data
//		must always be written to a buffer so that we don't overwrite original data that we need for smoothing.
//
//		Combinations of different requested and how to handle them in the most timely manner
//		is as follows:
//			Case 1:		Same Image (YES)  X Invert (NO)   Y Invert (NO)   Subsample (NO)   Window (NO)
//				Don't do anything (nothing has changed)
//			Case 2:		Same Image (NO)   X Invert (NO)   Y Invert (NO)   Subsample (NO)   Window (NO)
//				Do a FastMemoryCopy
//			Case 3:		Same Image (YES)  X Invert (NO)  Y Invert (YES)   Subsample (NO)   Window (NO)
//				For each row, swap pixels from the end working inward.
//			Case 4:		Same Image (NO)   X Invert (NO)  Y Invert (YES)   Subsample (NO)   Window (NO)
//				For each row, copy pixels in reverse.
//			Case 5:		Same Image (YES)  X Invert (YES)   Y Invert (NO)  Subsample (NO)   Window (NO)
//				For each row pair, swap the rows up to the midpoint.  If rows and row sizes are int
//				aligned, do an int swap for speed (swapping takes better advantage of caching than
//				copying to another buffer first).
//			Case 6:		Same Image (NO)   X Invert (YES)   Y Invert (NO)  Subsample (NO)   Window (NO)
//				For each row (in reverse order), FastMemoryCopy the row.
//			Case 7:		Same Image (YES)  X Invert (YES)  Y Invert (YES)  Subsample (NO)   Window (NO)
//				Swap pixels from either end of the image working inward to the midpoint.
//			Case 8:		Same Image (NO)   X Invert (YES)  Y Invert (YES)  Subsample (NO)   Window (NO)
//				Copy pixels in reverse.
//			Case 9:		Same Image (YES)  X Invert (NO)   Y Invert (NO)   Subsample (YES)  Window (NO)
//				For each row that we're not skipping, copy every S pixels.
//			Case 10:	Same Image (NO)   X Invert (NO)   Y Invert (NO)   Subsample (YES)  Window (NO)
//				For each row that we're not skipping, copy every S pixels to the output image.
//			Case 11:	Same Image (YES)  X Invert (NO)  Y Invert (YES)   Subsample (YES)  Window (NO)
//				For each row that we're not skipping, copy subsampled swapped pixels to a
//				buffer and then FastMemoryCopy the buffer into the new row location, using
//				the buffer to avoid accidentally overwriting data that has not yet been
//				swapped.
//			Case 12:	Same Image (NO)   X Invert (NO)  Y Invert (YES)   Subsample (YES)  Window (NO)
//				For each row that we're not skipping, copy subsampled swapped pixels to
//				the output image.
//			Case 13:	Same Image (YES)  X Invert (YES)   Y Invert (NO)  Subsample (YES)  Window (NO)
//				For each row that we're not skipping (in reverse order), copy all rows of
//				subsampled pixels to a buffer and then FastMemoryCopy the buffer back over
//				the input image, using the buffer to avoid accidentally overwriting data
//				that has not yet been swapped.
//			Case 14:	Same Image (NO)   X Invert (YES)   Y Invert (NO)  Subsample (YES)  Window (NO)
//				For each row that we're not skipping (in reverse order), copy subsampled
//				pixels to the output image.
//			Case 15:	Same Image (YES)  X Invert (YES)  Y Invert (YES)  Subsample (YES)  Window (NO)
//				For each row that we're not skipping (in reverse order, copy all rows of swapped
//				subsampled swapped pixels to a buffer and then FastMemoryCopy the buffer back over
//				the input image, using the buffer to avoid accidentally overwriting data
//				that has not yet been swapped.
//			Case 16:	Same Image (NO)   X Invert (YES)  Y Invert (YES)  Subsample (YES)  Window (NO)
//				For each row that we're not skipping (in reverse order), copy swapped subsampled
//				pixels to the output image.
//			Case 17:	Same Image (YES)  X Invert (NO)   Y Invert (NO)   Subsample (NO)   Window (YES)
//				For each row in the window, FastMemoryCopy the row of pixels in the window to a buffer
//				and then FastMemoryCopy the buffer to the next output row, using the buffer to avoid
//				accidentally overwriting data that has not yet been moved.
//			Case 18:	Same Image (NO)   X Invert (NO)   Y Invert (NO)   Subsample (NO)   Window (YES)
//				For each row in the window, FastMemoryCopy the row of pixels in the window to the output
//				image.
//			Case 19:	Same Image (YES)  X Invert (NO)  Y Invert (YES)   Subsample (NO)   Window (YES)
//				For each row in the window that we're not skipping, copy subsampled swapped pixels to
//				a buffer and then FastMemoryCopy the buffer into the new row location, using
//				the buffer to avoid accidentally overwriting data that has not yet been swapped.
//			Case 20:	Same Image (NO)   X Invert (NO)  Y Invert (YES)   Subsample (NO)   Window (YES)
//				For each row in the window, copy pixels in the window in reverse to the output image.
//			Case 21:	Same Image (YES)  X Invert (YES)   Y Invert (NO)  Subsample (NO)   Window (YES)
//				For each row in the window (in reverse order), copy all rows of pixels in the window
//				to a buffer and then FastMemoryCopy the buffer back over the input image, using the
//				buffer to avoid accidentally overwriting data that has not yet been swapped.
//			Case 22:	Same Image (NO)   X Invert (YES)   Y Invert (NO)  Subsample (NO)   Window (YES)
//				For each row in the window (in reverse order), FastMemoryCopy the windowed pixels in
//				that row.
//			Case 23:	Same Image (YES)  X Invert (YES)  Y Invert (YES)  Subsample (NO)   Window (YES)
//				Copy pixels in the window in reverse to a buffer and then FastMemoryCopy the buffer
//				back over the the input image, using the buffer to avoid accidentally overwriting data
//				that has not yet been swapped.
//			Case 24:	Same Image (NO)   X Invert (YES)  Y Invert (YES)  Subsample (NO)   Window (YES)
//				Copy pixels in the window in reverse to the output image.
//			Case 25:	Same Image (YES)  X Invert (NO)   Y Invert (NO)   Subsample (YES)  Window (YES)
//				For each row in the window that we're not skipping, copy every S pixels in the window.
//			Case 26:	Same Image (NO)   X Invert (NO)   Y Invert (NO)   Subsample (YES)  Window (YES)
//				For each row in the window that we're not skipping, copy every S pixels in the window
//				to the output image.
//			Case 27:	Same Image (YES)  X Invert (NO)  Y Invert (YES)   Subsample (YES)  Window (YES)
//				For each row in the window that we're not skipping, copy subsampled swapped pixels in
//				the window to a buffer and then FastMemoryCopy the buffer into the new row location,
//				using the buffer to avoid accidentally overwriting data that has not yet been swapped.
//			Case 28:	Same Image (NO)   X Invert (NO)  Y Invert (YES)   Subsample (YES)  Window (YES)
//				For each row in the window that we're not skipping, copy subsampled swapped pixels in
//				the window to the output image.
//			Case 29:	Same Image (YES)  X Invert (YES)   Y Invert (NO)  Subsample (YES)  Window (YES)
//				For each row in the window that we're not skipping (in reverse order), copy all rows of
//				subsampled pixels in the window to a buffer and then FastMemoryCopy the buffer back over
//				the input image, using the buffer to avoid accidentally overwriting data
//				that has not yet been swapped.
//			Case 30:	Same Image (NO)   X Invert (YES)   Y Invert (NO)  Subsample (YES)  Window (YES)
//				For each row in the window that we're not skipping (in reverse order), copy subsampled
//				pixels in the window to the output image.
//			Case 31:	Same Image (YES)  X Invert (YES)  Y Invert (YES)  Subsample (YES)  Window (YES)
//				For each row in the window that we're not skipping (in reverse order, copy all rows of swapped
//				subsampled swapped pixels in the window to a buffer and then FastMemoryCopy the buffer back over
//				the input image, using the buffer to avoid accidentally overwriting data that has not yet
//				been swapped.
//			Case 32:	Same Image (NO)   X Invert (YES)  Y Invert (YES)  Subsample (YES)  Window (YES)
//				For each row in the window that we're not skipping (in reverse order), copy swapped subsampled
//				pixels in the window to the output image.
//
//		Basically, I went through this exercise to cull out what might be the best approaches for each situation.
//		Having done that, I'm actually going to do the following:
//			Case 2:  Do a FastMemoryCopy.
//			Case 6:  For each row (in reverse order), FastMemoryCopy the row.
//			Case 18:  For each row in the window, FastMemoryCopy the row of pixels in the window to the output
//				image.
//			Case 22:  For each row in the window (in reverse order), FastMemoryCopy the windowed pixels in
//				that row.
//			All other even cases (output image is different from input image):  Copy row by row and pixel by pixel
//				according to the subsample, windowing, and invert parameters.  This seems to cost at most about
//				1 millisecond over the optimal approach, and keeps this code simpler.
//			Case 3:  For each row, swap pixels from the end working inward.
//			Case 5:  For each row pair, swap the rows up to the midpoint.  If rows and row sizes are int
//				aligned, do an int swap for speed (swapping takes better advantage of caching than
//				copying to another buffer first).
//			Case 7:  Swap pixels from either end of the image working inward to the midpoint.
//			Case 17:  For each row in the window, FastMemoryCopy the row of pixels in the window to a buffer
//				and then FastMemoryCopy the buffer to the next output row, using the buffer to avoid
//				accidentally overwriting data that has not yet been moved.
//			Case 21:  For each row in the window (in reverse order), copy all rows of pixels in the window
//				to a buffer and then FastMemoryCopy the buffer back over the input image, using the
//				buffer to avoid accidentally overwriting data that has not yet been swapped.
//			Cases 9 and 25:  Copy row by row and pixel by pixel according to the subsample, windowing,
//				and invert parameters.  This seems to cost at most about 1 millisecond over the optimal approach,
//				and keeps this code simpler.  What it comes down to is that as long as we're inverting in the same
//				space with no subsampling or windowing, we can simply swap bytes; if we are subsampling or windowing,
//				but not inverting, then we are simply shifting bytes to the beginning of the image and need no extra buffer.
//				If we're supposed to apply smoothing on subsampling, skip this case.
//			All other odd cases (output image and input image are the same) - Allocate an output buffer, then copy
//				row by row and pixel by pixel to that buffer according to the subsample, windowing, and invert
//				parameters.  When done, copy the output buffer over the input image with a FastMemoryCopy.  This
//				seems to cost at most about 1 millisecond over the optimal approach, and keeps this code simpler.
//
bool ModifyImage(unsigned char *InputImage, unsigned char *OutputImage, int *XDimension, int *YDimension, int *StartColorPattern, int *xOffset, int * yOffset ,bool InvertedImager,
			bool InvertAboutXAxis, bool InvertAboutYAxis, int PixelSubsampleInterval, int NewLeft, int NewRight, int NewTop,
			int NewBottom, int Margin, bool ApplySmoothing, bool Use5x5Smoothing)
{
	bool		InputIsOutput = (OutputImage == InputImage) ? true : false;
	bool		Subsampled = false;
	bool		Windowed = false;
	bool		ReturnStatus = true;			// No problems yet.

	int SubSampleSmoothingTable[11][11] =
	{
	    {  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
	    {  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
	    {  1,  1,  2,  0,  0,  0,  0,  0,  0,  0,  0 },
	    {  5,  7,  5, 17,  0,  0,  0,  0,  0,  0,  0 },
	    {  5,  8,  8,  5, 26,  0,  0,  0,  0,  0,  0 },
	    { 12, 20, 23, 20, 12, 87,  0,  0,  0,  0,  0 },
	    {  4,  7,  9,  9,  7,  4, 40,  0,  0,  0,  0 },
	    {  5,  9, 12, 13, 12,  9,  5, 65,  0,  0,  0 },
	    { 10, 19, 25, 29, 29, 25, 19, 10,166,  0,  0 },
	    { 10, 19, 26, 31, 32, 31, 26, 19, 10,204,  0 },
	    { 10, 19, 27, 32, 35, 35, 32, 27, 19, 10,246 },
	};

	int		ColumnSampleSize;
	int		RowSampleSize;
	int		Average;
	int		SubsampleSmoothingAccumulator;
	int		i, j;

	if(HardwareToken.IAmOEMJade() || GET_CONFIG_SETTING(ImageSensor) == E2V_EV76C454_Jade_Monocolor)
	{
		ApplySmoothing = false;
		Use5x5Smoothing = false;
		PixelSubsampleInterval = 1;
	}

// Only apply subsample smoothing if pixel increment is <= 10.
	if((PixelSubsampleInterval > 10) && (!Use5x5Smoothing))
	{
		ApplySmoothing = false;
	}

// If margin or box coordinates are -1, set them to default values.
	if(Margin < 0)    Margin = 0;
	if(NewLeft < 0)   NewLeft = 0;
	if(NewRight < 0)  NewRight = *XDimension - 1;
	if(NewTop < 0)    NewTop = 0;
	if(NewBottom < 0) NewBottom = *YDimension - 1;
	if(PixelSubsampleInterval < 0) PixelSubsampleInterval = 1;

// Margin and dimensional data had better be positive and within the limits of the image size.
	if((Margin >= (*XDimension / 2)) ||
			(Margin >= (*YDimension / 2)) ||
			(*XDimension < 1) ||
			(*YDimension < 1))
	{
		ReturnStatus = false;
	}

// Compute new start and end coordinates.
	if(ReturnStatus)
	{
		NewLeft += Margin;
		NewRight -= Margin;
		NewTop += Margin;
		NewBottom -= Margin;

	// Validate the coordinate parameters and adjust them if the imager is inverted.
		if((NewLeft < 0) || (NewLeft >= *XDimension) || (NewLeft > NewRight) ||
			(NewRight < 0) || (NewRight >= *XDimension) ||
			(NewTop < 0) || (NewTop >= *YDimension) || (NewTop > NewBottom) ||
			(NewBottom < 0) || (NewBottom >= *YDimension) ||
			(PixelSubsampleInterval < 1) || (PixelSubsampleInterval > (NewRight - NewLeft)) ||
					(PixelSubsampleInterval > (NewBottom - NewTop)))
		{
			ReturnStatus = false;
	   }
		else
		{
		// If image is inverted in the hardware configuration that this routine is being built for, invert automatically.
			if(InvertedImager)
			{
				InvertAboutXAxis = !InvertAboutXAxis;
				InvertAboutYAxis = !InvertAboutYAxis;

			// Adjust left, right, top, and bottom, because the user expects the differences to be applied to a correctly
			// oriented image.
				int	LeftDifference = NewLeft;
				int	RightDifference = *XDimension - NewRight - 1;
				int	TopDifference = NewTop;
				int	BottomDifference = *YDimension - NewBottom - 1;
				NewLeft = RightDifference;
				NewRight = *XDimension - LeftDifference - 1;
				NewTop = BottomDifference;
				NewBottom = *YDimension - TopDifference - 1;
			}

		// Is the image being subsampled?
			if(PixelSubsampleInterval > 1)
			{
				Subsampled = true;
			}

		// Is the image being windowed?
			if((NewLeft != 0) || (NewTop != 0) || (NewRight != (*XDimension - 1)) || (NewBottom != (*YDimension - 1)))
			{
				Windowed = true;
			}
		}
		ReturnStatus = CalculateOffsiteAndPattern(StartColorPattern, xOffset, yOffset,  InvertAboutXAxis, InvertAboutYAxis, NewLeft, NewRight, NewTop, NewBottom, Windowed, Subsampled);
	// If everything is OK, and we're actually changing the image in some way by reducing, inverting, and / or subsampling,
	// process it now (Case 1:		Same Image (YES)  X Invert (NO)   Y Invert (NO)   Subsample (NO)   Window (NO)).
		if((ReturnStatus) &&
			((Windowed) || (Subsampled) || (InvertAboutXAxis) || (InvertAboutYAxis) || (!InputIsOutput) || (ApplySmoothing)))
		{
		// New number of rows and columns, after reduction.
		    int				NewXDimension = (NewRight - NewLeft + 1) / PixelSubsampleInterval;
		    int				NewYDimension = (NewBottom - NewTop + 1) / PixelSubsampleInterval;

		// Let's make sure that the output image doesn't overlap the input image in any way if not the same image,
		// and that there is no image wrap around the end of the address space.
			unsigned int	InputImageStart = (unsigned int) InputImage;
			unsigned int	InputImageEnd = InputImageStart + (*XDimension) * (*YDimension);
			unsigned int	OutputImageStart = (unsigned int) OutputImage;
			unsigned int	OutputImageEnd = OutputImageStart + NewXDimension * NewYDimension;

			if((InputImageEnd > InputImageStart) && (OutputImageEnd > OutputImageStart) &&
				((InputIsOutput) || (OutputImageStart >= InputImageEnd) || (InputImageStart >= OutputImageEnd)))
			{
				int					StartRow = (InvertAboutXAxis) ? NewBottom : NewTop;
				int					StartCol = (InvertAboutYAxis) ? NewRight : NewLeft;
				register unsigned char		*NextColToMove;				// Next column to move
			   register unsigned char		*NextRowToMove;				// Next row to move
			   register int		x;							// Column counter
				register int		y;							// Row counter
			   register unsigned char		*MoveDestination;			// Next position in image data to move reduced image data to.
				register int		RowIncrement;				// Number and direction of rows to move.
				register int		RowIncrementPixelOffset;	// Total number of pixels between from the beginning of one
																//	row to another.
				register int		ColIncrement;				// Number and direction of columns to move.

				RowIncrement = PixelSubsampleInterval * ((InvertAboutXAxis) ? -1 : 1);
				ColIncrement = PixelSubsampleInterval * ((InvertAboutYAxis) ? -1 : 1);
				RowIncrementPixelOffset = (*XDimension) * RowIncrement;
				NextRowToMove = InputImage + StartRow * (*XDimension);
				NextRowToMove += StartCol;
				MoveDestination = OutputImage;

			// If the output image is not the same as the input, do the copy.
				if(!InputIsOutput)
				{
				// Cases 2, 6, 18, or 22 (no Y inversion or subsampling).
					if((!Subsampled) && (!InvertAboutYAxis) && (!ApplySmoothing))
					{
					// Case 2:  Do a FastMemoryCopy.
					// Treat no inversion, windowing, or subsampling as a special case for speed purposes, because we can do a straight
					// copy.
						if((!Windowed) && (!InvertAboutXAxis))
						{
							FastMemoryCopy(OutputImage, InputImage, (*XDimension) * (*YDimension));
							TRACE("Case 2\r\n");
						} // End of case 2

					// Case 6:  For each row (in reverse order), FastMemoryCopy the row.
					// Case 18:  For each row in the window, FastMemoryCopy the row of pixels in the window to the output
					//	image.
					// Case 22:  For each row in the window (in reverse order), FastMemoryCopy the windowed pixels in
					//	that row.
						else
						{
						// For all three cases, use the same logic.  Because we already set up the row and column increments,
						// we can just FastMemoryCopy each row and move the pointers will be handled automatically.
							y = NewYDimension;

							do
							{
								FastMemoryCopy(MoveDestination, NextRowToMove, NewXDimension);
								MoveDestination += NewXDimension;
								NextRowToMove += RowIncrementPixelOffset;
							}  while(--y);

							TRACE("Cases 6, 18, or 22\r\n");
						} // Cases 6, 18, or 22
					} // Cases 2, 6, 18, or 22

					else
					{
					// All other even cases (output image is different from input image):  Copy row by row and pixel by pixel
					// according to the subsample, windowing, and invert parameters.  This seems to cost at most about
					// 1 millisecond over the optimal approach, and keeps this code simpler.

					// Treat smoothing case separately.
						if(ApplySmoothing)
						{
							if(Use5x5Smoothing)
							{
								int		RowCount;
								int		ColCount;
								int		LeftAveragingLimit;
								int		RightAveragingLimit;
								int		TopAveragingLimit;
								int		BottomAveragingLimit;
								int		NumberOfAveragedPixels;
								int		PixelsFromCenter = 2;

								y = StartRow;

								for (RowCount = 0;    RowCount < NewYDimension;    ++RowCount)
								{
									if((y + PixelsFromCenter) < (*YDimension))
										BottomAveragingLimit = y + PixelsFromCenter;
									else
										BottomAveragingLimit = *YDimension - 1;

									if((y - PixelsFromCenter) >= 0)
										TopAveragingLimit = y - PixelsFromCenter;
									else
										TopAveragingLimit = 0;

									x = StartCol;
									for (ColCount = 0;    ColCount < NewXDimension;    ++ColCount)
									{
										if((x + PixelsFromCenter) < (*XDimension))
											RightAveragingLimit = x + PixelsFromCenter;
										else
											RightAveragingLimit = *XDimension - 1;

										if((x - PixelsFromCenter) >= 0)
											LeftAveragingLimit = x - PixelsFromCenter;
										else
											LeftAveragingLimit = 0;

										Average = 0;
										NumberOfAveragedPixels = 0;

							         for( i = TopAveragingLimit; i <= BottomAveragingLimit; i++ )
						            {
								      	// Average over the skip width.
						               for( j = LeftAveragingLimit; j <= RightAveragingLimit; j++ )
											{
												Average += (int) InputImage[(i * (*XDimension)) + j];
												++NumberOfAveragedPixels;
											}
										}

//										RESET_WATCHDOG_TIMER();
										*MoveDestination++ = (unsigned char)(Average / NumberOfAveragedPixels);
										x += ColIncrement;
									}

									y += RowIncrement;
								}
							}
							else
							{
								int		RowCount;
								int		ColCount;

								y = StartRow;
								for (RowCount = 0;    RowCount < NewYDimension;    ++RowCount)
								{
									RowSampleSize = ( (y + PixelSubsampleInterval) > (*YDimension) ) ?
												((*YDimension) - y) : PixelSubsampleInterval;
									x = StartCol;

									for (ColCount = 0;    ColCount < NewXDimension;    ++ColCount)
									{
										Average = 0;

							         for( i = 0; i < RowSampleSize; i++ )
						            {
								      	// Average over the skip width.
											SubsampleSmoothingAccumulator = 0;
							            ColumnSampleSize = ( (PixelSubsampleInterval + x) > (*XDimension) ) ?
												((*XDimension) - x) : PixelSubsampleInterval;

											for( j = 0; j < ColumnSampleSize; j++ )
												SubsampleSmoothingAccumulator += (int)InputImage[x + j + ((y + i) * (*XDimension))] *
													SubSampleSmoothingTable[ ColumnSampleSize ][ j ];

											Average += ((SubsampleSmoothingAccumulator / SubSampleSmoothingTable[ ColumnSampleSize ][ ColumnSampleSize ]) *
												SubSampleSmoothingTable[ RowSampleSize ][ i ]);
										}

//										RESET_WATCHDOG_TIMER();
										*MoveDestination++ = (unsigned char)(Average / SubSampleSmoothingTable[ RowSampleSize ][ RowSampleSize ]);
										x += ColIncrement;
									}

									y += RowIncrement;
								}
							}
						}
						else
						{
							y = NewYDimension;

							do
							{
								x = NewXDimension;
								NextColToMove = NextRowToMove;

								do
								{
									*MoveDestination++ = *NextColToMove;
									NextColToMove += ColIncrement;
								}  while(--x);

								NextRowToMove += RowIncrementPixelOffset;

							}  while(--y);
						}

						TRACE("All other even cases\r\n");
					} // All other even cases
				} // If writing to a different image.

				else	// Writing to the same image.
				{
				// Cases 3, 5, or 7 (inversion only).
					if((!Windowed) && (!Subsampled) && (!ApplySmoothing))
					{
					// Case 3:  For each row, swap pixels from the end working inward.
						if((!InvertAboutXAxis) && (InvertAboutYAxis))
						{
							register unsigned char	TempPixel;
						   register unsigned char	*NextDestinationRow;	// Since we're using MoveDestination, we need to save position.

							y = NewYDimension;
							NextDestinationRow = MoveDestination;

							do
							{
								x = NewXDimension / 2;
								NextColToMove = NextRowToMove;
								MoveDestination = NextDestinationRow;

								do
								{
									TempPixel = *MoveDestination;
									*MoveDestination++ = *NextColToMove;
									*NextColToMove-- = TempPixel;
								}  while(--x);

								NextRowToMove += RowIncrementPixelOffset;
								NextDestinationRow += NewXDimension;

							}  while(--y);

							TRACE("Case 3\r\n");
						} // Case 3

					// Case 5:  For each row pair, swap the rows up to the midpoint.  If rows and row sizes are int
					//		aligned, do an int swap for speed (swapping takes better advantage of caching than
					//		copying to another buffer first).
						else if((InvertAboutXAxis) && (!InvertAboutYAxis))
						{
							y = NewYDimension / 2;

							do
							{
								SwapDataArrays(NextRowToMove, MoveDestination, NewXDimension);
								MoveDestination += NewXDimension;
								NextRowToMove += RowIncrementPixelOffset;
							}  while(--y);

							TRACE("Case 5\r\n");
						} // Case 5

					// Case 7:  Swap pixels from either end of the image working inward to the midpoint.
						else if((InvertAboutXAxis) && (InvertAboutYAxis))
						{
							register unsigned char	TempPixel;

							x = (NewXDimension * NewYDimension) / 2;
							NextColToMove = NextRowToMove;

							do
							{
								TempPixel = *MoveDestination;
								*MoveDestination++ = *NextColToMove;
								*NextColToMove-- = TempPixel;
							}  while(--x);

							TRACE("Case 7\r\n");
						} // Case 7

					// Note that we're not handling the no inversion case, since we shouldn't get here (same image,
					// no inversion, no windowing, and no subsampling should have caused us to quit with no action.
					} // Cases 3, 5, or 7.


				// Cases 17 or 21 (windowing with optional Y inversion only); must be windowed, or we would have
				// stopped on cases 3, 5, or 7.
					else if((!InvertAboutYAxis) && (!Subsampled) && (!ApplySmoothing))
					{
					// Case 17:  For each row in the window, FastMemoryCopy the row of pixels in the window to a buffer
					//	and then FastMemoryCopy the buffer to the next output row, using the buffer to avoid
					//	accidentally overwriting data that has not yet been moved.
					//
					// NOTE: This could probably be consolidated with case 21, since we're probably not gaining
					//	much (if anything) in this straight row copy vs. inverse row copy.
						if(!InvertAboutXAxis)
						{
							unsigned char	*Buffer = new unsigned char[NewXDimension];

							y = NewYDimension;

							do
							{
								FastMemoryCopy(Buffer, NextRowToMove, NewXDimension);
								FastMemoryCopy(NextRowToMove, MoveDestination, NewXDimension);
								FastMemoryCopy(MoveDestination, Buffer, NewXDimension);
								MoveDestination += NewXDimension;
								NextRowToMove += RowIncrementPixelOffset;
							}  while(--y);

							delete[] Buffer;

							TRACE("Case 17\r\n");
						} // Case 17

					// Case 21:  For each row in the window (in reverse order), copy all rows of pixels in the window
					//	to a buffer and then FastMemoryCopy the buffer back over the input image, using the
					//	buffer to avoid accidentally overwriting data that has not yet been swapped.
						else
						{
							unsigned char	*Buffer = new unsigned char[NewXDimension * NewYDimension];

							y = NewYDimension;
							MoveDestination = Buffer;

							do
							{
								FastMemoryCopy(MoveDestination, NextRowToMove, NewXDimension);
								MoveDestination += NewXDimension;
								NextRowToMove += RowIncrementPixelOffset;
							}  while(--y);

							FastMemoryCopy(InputImage, Buffer, NewXDimension * NewYDimension);

							delete[] Buffer;

							TRACE("Case 21\r\n");
						} // Case 21
					} // Cases 17 or 21.


				// Cases 9 or 25:  Copy row by row and pixel by pixel according to the subsample, windowing,
				//	and invert parameters.  This seems to cost at most about 1 millisecond over the optimal approach,
				//	and keeps this code simpler.  What it comes down to is that as long as we're inverting in the same
				//	space with no subsampling or windowing, we can simply swap bytes; if we are subsampling or windowing,
				//	but not inverting, then we are simply shifting bytes to the beginning of the image and need no extra buffer.
				//	If we're supposed to apply smoothing on subsampling, skip this case.
					else if((!InvertAboutXAxis) && (!InvertAboutYAxis) && (!Subsampled) && (!ApplySmoothing))
					{
						register unsigned char	TempPixel;

						y = NewYDimension;

						do
						{
							x = NewXDimension;
							NextColToMove = NextRowToMove;

							do
							{
								TempPixel = *MoveDestination;
								*MoveDestination++ = *NextColToMove;
								*NextColToMove = TempPixel;
								NextColToMove += ColIncrement;
							}  while(--x);

							NextRowToMove += RowIncrementPixelOffset;
						}  while(--y);

						TRACE("Cases 9 or 25\r\n");
					} // Cases 9 or 25

					else
					{
					// All other odd cases (output image and input image are the same) - Allocate an output buffer, then copy
					// row by row and pixel by pixel to that buffer according to the subsample, windowing, and invert
					// parameters.  When done, copy the output buffer over the input image with a FastMemoryCopy.  This
					// seems to cost at most about 1 millisecond over the optimal approach, and keeps this code simpler.
						unsigned char	*Buffer = new unsigned char[NewXDimension * NewYDimension];

						MoveDestination = Buffer;

					// Treat smoothing case separately.
						if(ApplySmoothing)
						{
							if(Use5x5Smoothing)
							{
								int		RowCount;
								int		ColCount;
								int		LeftAveragingLimit;
								int		RightAveragingLimit;
								int		TopAveragingLimit;
								int		BottomAveragingLimit;
								int		NumberOfAveragedPixels;
								int		PixelsFromCenter = 2;

								y = StartRow;

								for (RowCount = 0;    RowCount < NewYDimension;    ++RowCount)
								{
									if((y + PixelsFromCenter) < (*YDimension))
										BottomAveragingLimit = y + PixelsFromCenter;
									else
										BottomAveragingLimit = *YDimension - 1;

									if((y - PixelsFromCenter) >= 0)
										TopAveragingLimit = y - PixelsFromCenter;
									else
										TopAveragingLimit = 0;

									x = StartCol;

									for (ColCount = 0;    ColCount < NewXDimension;    ++ColCount)
									{
										if((x + PixelsFromCenter) < (*XDimension))
											RightAveragingLimit = x + PixelsFromCenter;
										else
											RightAveragingLimit = *XDimension - 1;

										if((x - PixelsFromCenter) >= 0)
											LeftAveragingLimit = x - PixelsFromCenter;
										else
											LeftAveragingLimit = 0;

										Average = 0;
										NumberOfAveragedPixels = 0;

							         for( i = TopAveragingLimit; i <= BottomAveragingLimit; i++ )
						            {
								      	// Average over the skip width.
						               for( j = LeftAveragingLimit; j <= RightAveragingLimit; j++ )
											{
												Average += (int) InputImage[(i * (*XDimension)) + j];
												++NumberOfAveragedPixels;
											}
										}

//										RESET_WATCHDOG_TIMER();
										*MoveDestination++ = (unsigned char)(Average / NumberOfAveragedPixels);
										x += ColIncrement;
									}

									y += RowIncrement;
								}
							}
							else
							{
								int		RowCount;
								int		ColCount;

								y = StartRow;

								for (RowCount = 0;    RowCount < NewYDimension;    ++RowCount)
								{
									RowSampleSize = ( (y + PixelSubsampleInterval) > (*YDimension) ) ?
												((*YDimension) - y) : PixelSubsampleInterval;

									x = StartCol;

									for (ColCount = 0;    ColCount < NewXDimension;    ++ColCount)
									{
										Average = 0;
							         for( i = 0; i < RowSampleSize; i++ )
						            {
								      	// Average over the skip width.
											SubsampleSmoothingAccumulator = 0;
							            ColumnSampleSize = ( (PixelSubsampleInterval + x) > (*XDimension) ) ?
												((*XDimension) - x) : PixelSubsampleInterval;

						               for( j = 0; j < ColumnSampleSize; j++ )
												SubsampleSmoothingAccumulator += (int)InputImage[x + j + ((y + i) * (*XDimension))] *
													SubSampleSmoothingTable[ ColumnSampleSize ][ j ];

										   Average += ((SubsampleSmoothingAccumulator / SubSampleSmoothingTable[ ColumnSampleSize ][ ColumnSampleSize ]) *
												SubSampleSmoothingTable[ RowSampleSize ][ i ]);
										}

//										RESET_WATCHDOG_TIMER();
										*MoveDestination++ = (unsigned char)(Average / SubSampleSmoothingTable[ RowSampleSize ][ RowSampleSize ]);
										x += ColIncrement;
									}

									y += RowIncrement;
								}
							}
						}
						else
						{
							y = NewYDimension;

							do
							{
								x = NewXDimension;
								NextColToMove = NextRowToMove;

								do
								{
									*MoveDestination++ = *NextColToMove;
									NextColToMove += ColIncrement;
								}  while(--x);

								NextRowToMove += RowIncrementPixelOffset;
							}  while(--y);
						}

						FastMemoryCopy(InputImage, Buffer, NewXDimension * NewYDimension);
						delete[] Buffer;
						TRACE("All other odd cases\r\n");
					} // All other odd cases
				} // If writing to the same image.

			// If we succeeded, save the new X and Y dimensions.
				if(ReturnStatus)
				{
					*XDimension = NewXDimension;
					*YDimension = NewYDimension;
				}
			}
			else	// Image address failure - Either one of the images will wrap around the end of address space
					// OR input is supposed to be different from output, but they overlap.
			{
				ReturnStatus = false;
			} // Image address failure
		}
	}

	return(ReturnStatus);
} // ModifyImage; overloaded version #1





//
//	ModifyImage - overloaded version #2
//		Subsample, window, and / or invert image over the same image by calling ModifyImage version #1 with
//		the same input and output image.
//
bool ModifyImage(unsigned char *Image, int *XDimension, int *YDimension, int *StartColorPattern, int *xOffset, int * yOffset ,bool InvertedImager, bool InvertAboutXAxis,
		bool InvertAboutYAxis, int PixelSubsampleInterval, int NewLeft, int NewRight, int NewTop, int NewBottom, int Margin,
		bool ApplySmoothing, bool Use5x5Smoothing)
{
	return(ModifyImage(Image, Image, XDimension, YDimension, StartColorPattern, xOffset, yOffset , InvertedImager, InvertAboutXAxis, InvertAboutYAxis,
				PixelSubsampleInterval, NewLeft, NewRight, NewTop, NewBottom, Margin, ApplySmoothing, Use5x5Smoothing));
} // ModifyImage; overloaded version #2




//
//	ModifyImage - overloaded version #3
//		Subsample, window, and / or invert HSTRING image either over the same image or to a new image.
//		Considering all the possible combinations of changes based on the input parameters,
//		we'll try to do the modifications as fast as we can for each given case.  Note that this
//		routine assumes that input and output image are either the same or don't overlap; we woof
//		if they do overlap.
//
bool ModifyImage(HSTRING *InputImage, HSTRING *OutputImage, int *XDimension, int *YDimension, int *StartColorPattern, int *xOffset, int * yOffset,bool InvertedImager,
			bool InvertAboutXAxis, bool InvertAboutYAxis, int PixelSubsampleInterval, int NewLeft, int NewRight, int NewTop,
			int NewBottom, int Margin, bool ApplySmoothing, bool Use5x5Smoothing)
{
	bool	ReturnStatus = ModifyImage(InputImage->Char, OutputImage->Char, XDimension, YDimension, StartColorPattern, xOffset, yOffset, InvertedImager,
			InvertAboutXAxis, InvertAboutYAxis, PixelSubsampleInterval, NewLeft, NewRight, NewTop, NewBottom, Margin,
			ApplySmoothing, Use5x5Smoothing);

	if(ReturnStatus)
	{
		OutputImage->Size = (*XDimension) * (*YDimension);
	}

	return(ReturnStatus);
} // ModifyImage; overloaded version #3




//
//	ModifyImage - overloaded version #4
//		Subsample, window, and / or invert HSTRING image over the same image by calling ModifyImage version #1 with
//		the same input and output image.
//
bool ModifyImage(HSTRING *Image, int *XDimension, int *YDimension, int *StartColorPattern, int *xOffset, int * yOffset,bool InvertedImager, bool InvertAboutXAxis,
		bool InvertAboutYAxis, int PixelSubsampleInterval, int NewLeft, int NewRight, int NewTop, int NewBottom, int Margin,
		bool ApplySmoothing, bool Use5x5Smoothing)
{
	bool	ReturnStatus = ModifyImage(Image->Char, Image->Char, XDimension, YDimension, StartColorPattern, xOffset, yOffset, InvertedImager,
			InvertAboutXAxis, InvertAboutYAxis, PixelSubsampleInterval, NewLeft, NewRight, NewTop, NewBottom, Margin,
			ApplySmoothing, Use5x5Smoothing);

	if(ReturnStatus)
	{
		Image->Size = (*XDimension) * (*YDimension);
	}

	return(ReturnStatus);
} // ModifyImage; overloaded version #4




//#ifdef CF2D
//
bool CalculateOffsiteAndPattern(int * StartColorPattern, int * xOffset, int *yOffset,  bool InvertAboutXAxis, bool InvertAboutYAxis, int NewLeft, int NewRight, int NewTop, int NewBottom, bool Windowed, bool Subsampled)
{
	bool Status = true;
	if(Subsampled)
		Status = false;
	if( !InvertAboutYAxis && !InvertAboutXAxis)
	{
		int mxValue = NewLeft %4;
		int dxValue = NewLeft /4;
		int myValue = NewTop %4; 
		int dyValue = NewTop /4;
		int xValue  = dxValue % 2;
		int yValue  = dyValue % 2;
		if( !xValue && !yValue && !(mxValue > 2) && !(myValue > 2))    // (4n+x, 4m+y, m, n odd)  CASE 1
		{
			*xOffset = 2 - mxValue;
			*yOffset = 2 - myValue;
			*StartColorPattern = 1;  //GB
		}
		else if (!xValue && !yValue && (mxValue >2 ) && !(myValue >2)) // CASE 2
		{
			*xOffset = 3;
			*yOffset = 2 - myValue;
			*StartColorPattern = 0;        //BG
		}
		else if (!xValue && !yValue && !(mxValue > 2) && (myValue > 2)) // CASE 3
		{
			*xOffset = 2 - mxValue;
			*yOffset = 3;
			*StartColorPattern = 2;    //RG
		}
		else if (!xValue && !yValue && (mxValue > 2) && (myValue > 2)) // CASE 4
		{
			*xOffset = 3;
			*yOffset = 3;
			*StartColorPattern = 3; //GR
		}
		else if (!xValue  && yValue && !(mxValue >2 ) && !(myValue >2)) // CASE 5
		{
			*xOffset = 2 - mxValue;
			*yOffset = 2 - myValue;
			*StartColorPattern = 2 ; //RG
		}
		else if (!xValue  && yValue && (mxValue >2 ) && !(myValue >2)) // CASE 6
		{
			*xOffset = 3;
			*yOffset = 2 - myValue;
			*StartColorPattern = 3 ; //GR
		}
		else if (!xValue  && yValue && !(mxValue >2 ) && (myValue >2)) // CASE 7
		{
			*xOffset = 2 - mxValue;
			*yOffset = 3;
			*StartColorPattern = 1 ; //GB
		}
		else if (!xValue  && yValue && (mxValue >2 ) && (myValue >2))  // CASE 8
		{
			*xOffset = 3;
			*yOffset = 3;
			*StartColorPattern = 0 ; //BG
		}
		else if (xValue && !yValue && !(mxValue >2) && !(myValue > 2)) // CASE 9
		{
			*xOffset = 2 - mxValue;
			*yOffset = 2 - myValue;
			*StartColorPattern = 0; //BG
		}
		else if (xValue && !yValue && (mxValue >2) && !(myValue > 2)) // CASE 10
		{
			*xOffset = 3;
			*yOffset = 2 - myValue;
			*StartColorPattern = 1; //GB
		}
		else if (xValue && !yValue && !(mxValue >2) && (myValue > 2)) // CASE 11
		{
			*xOffset = 2 - mxValue;
			*yOffset = 3;
			*StartColorPattern = 3; //GR
		}
		else if (xValue && !yValue && !(mxValue >2) && !(myValue > 2)) // CASE 12
		{
			*xOffset = 3;
			*yOffset = 3;
			*StartColorPattern = 2; //RG
		}
		else if (xValue && yValue && !(mxValue >2) && !(myValue > 2)) // CASE 13
		{
			*xOffset = 2 - mxValue;
			*yOffset = 2 - myValue;
			*StartColorPattern = 3; //GR
		}
		else if (xValue && yValue && (mxValue >2) && !(myValue > 2))  // CASE 14
		{
			*xOffset = 3;
			*yOffset = 2 - myValue;
			*StartColorPattern = 2; //RG
		}
		else if (xValue && yValue && !(mxValue >2) && (myValue > 2)) // CASE 15
		{
			*xOffset = 2 - mxValue;
			*yOffset = 3;
			*StartColorPattern = 0; //BG
		}
		else if (xValue && yValue && (mxValue >2) && (myValue > 2))  //CASE 16
		{
			*xOffset = 3;
			*yOffset = 3;
			*StartColorPattern = 1; //GB
		}
	}
	else if (!Windowed && InvertAboutYAxis && InvertAboutXAxis )  // X inverted and Y inverted
	{
		*xOffset = 1;
		*yOffset = 1;
		*StartColorPattern = 2; // RG
	}
	else if (!Windowed && InvertAboutYAxis && !InvertAboutXAxis ) // Y inverted and not X inverted
	{
		*xOffset = 1;
		*yOffset = 2;
		*StartColorPattern = 1 ; // GB
	}
	else if ( !Windowed && !InvertAboutYAxis && InvertAboutXAxis ) // X inverted and not Y inverted
	{
		*xOffset = 2;
		*yOffset = 1;
		*StartColorPattern = 2; //RG
	} 
	else if ( Windowed && InvertAboutYAxis && InvertAboutXAxis ) // Windowed, and X inverted and Y inverted
	{
		int mxValue = NewRight %4;
		int dxValue = NewRight /4;
		int myValue = NewBottom %4; 
		int dyValue = NewBottom /4;
		int xValue  = dxValue % 2;
		int yValue  = dyValue % 2;

		if(xValue && yValue && !(mxValue <2) && !(myValue < 2))    // (4n+x, 4m+y, m, n odd)  CASE 1
		{
			*xOffset = mxValue - 2;
			*yOffset = myValue - 2;
			*StartColorPattern = 3; // GR
		}
		else if (xValue && yValue && !(mxValue <2) && (myValue < 2)) // case 2
		{
			*xOffset = mxValue - 2;
			*yOffset = myValue + 2 ;
			*StartColorPattern = 0; //BG 
		}
		else if (xValue && yValue && (mxValue <2) && !(myValue < 2)) //case 3
		{
			*xOffset = mxValue + 2;
			*yOffset = myValue - 2;
			*StartColorPattern = 2; //RG
		}
		else if (xValue && yValue && (mxValue <2) && (myValue < 2))  //case 4
		{
			*xOffset = mxValue + 2;
			*yOffset = myValue + 2;
			*StartColorPattern = 1 ; //GB
		}
		else if (xValue && !yValue && !(mxValue < 2) && !(myValue < 2)) //case 5
		{
			*xOffset = mxValue - 2;
			*yOffset = myValue - 2;
			*StartColorPattern = 0 ; //BG
		}
		else if (xValue && !yValue && !(mxValue <2) && (myValue < 2)) // case 6
		{
			*xOffset = mxValue - 2;
			*yOffset = myValue + 2 ;
			*StartColorPattern = 3; //GR
		}
		else if (xValue && !yValue && (mxValue <2) && !(myValue < 2)) // case 7
		{
			*xOffset = mxValue + 2;
			*yOffset = myValue - 2 ;
			*StartColorPattern = 1; //GB
		}
		else if (xValue && !yValue && (mxValue <2) && (myValue < 2)) // case 8
		{
			*xOffset = mxValue + 2;
			*yOffset = myValue + 2 ;
			*StartColorPattern = 2; //RG
		}
		else if (!xValue && yValue && !(mxValue <2) && !(myValue < 2)) // case 9
		{
			*xOffset = mxValue - 2;
			*yOffset = myValue - 2 ;
			*StartColorPattern = 2; //RG
		}
		else if (!xValue && yValue && !(mxValue <2) && (myValue < 2)) // case 10
		{
			*xOffset = mxValue - 2;
			*yOffset = myValue + 2 ;
			*StartColorPattern = 1; //GB
		}
		else if (!xValue && yValue && (mxValue <2) && !(myValue < 2)) // case 11
		{
			*xOffset = mxValue + 2;
			*yOffset = myValue -2 ;
			*StartColorPattern = 3; // GR
		}
		else if (!xValue && yValue && (mxValue <2) && (myValue < 2)) // case 12
		{
			*xOffset = mxValue + 2;
			*yOffset = myValue + 2 ;
			*StartColorPattern = 0; // BG
		}
		else if (!xValue && !yValue && !(mxValue <2) && !(myValue < 2)) // case 13
		{
			*xOffset = mxValue - 2;
			*yOffset = myValue - 2 ;
			*StartColorPattern = 1; // GB
		}
		else if (!xValue && !yValue && !(mxValue <2) && (myValue < 2)) // case 14
		{
			*xOffset = mxValue - 2;
			*yOffset = myValue + 2 ;
			*StartColorPattern = 2; // RG
		}
		else if (!xValue && !yValue && (mxValue <2) && !(myValue < 2)) // case 15
		{
			*xOffset = mxValue + 2;
			*yOffset = myValue - 2 ;
			*StartColorPattern = 0; // BG
		}
		else if (!xValue && !yValue && (mxValue <2) && (myValue < 2)) // case 16
		{
			*xOffset = mxValue + 2;
			*yOffset = myValue + 2 ;
			*StartColorPattern = 3; // GR
		}
	}
	else if ( Windowed && InvertAboutYAxis && !InvertAboutXAxis ) // Windowed, and X do not inverted and Y inverted
	{
		int mxValue = NewRight %4;
		int dxValue = NewRight /4;
		int myValue = NewTop %4; 
		int dyValue = NewTop /4;
		int xValue  = dxValue % 2;
		int yValue  = dyValue % 2;
		if (xValue && yValue && !(mxValue <2) && !(myValue >2)) // case 1
		{
			*xOffset = mxValue - 2;
			*yOffset = 2 - myValue;
			*StartColorPattern = 3; // GR
		}
		else if (xValue && yValue && !(mxValue <2) && (myValue >2)) //case 2
		{
			*xOffset = mxValue - 2;
			*yOffset = 3;
			*StartColorPattern = 0; //BG
		}
		else if (xValue && yValue && (mxValue < 2) && !(myValue > 2)) //case 3
		{
			*xOffset = mxValue + 2;
			*yOffset = 2 - myValue;
			*StartColorPattern = 2; //RG
		}
		else if (xValue && yValue && (mxValue < 2) && (myValue > 2))  //case 4
		{
			*xOffset = mxValue + 2;
			*yOffset = 3;
			*StartColorPattern = 1; //GB
		}
		else if (!xValue && yValue && !(mxValue < 2) && !(myValue > 2))  //case 5
		{
			*xOffset = mxValue - 2;
			*yOffset = 2 - myValue;
			*StartColorPattern = 2; //RG
		}
		else if (!xValue && yValue && !(mxValue < 2) && (myValue > 2))  //case 6
		{
			*xOffset = mxValue - 2;
			*yOffset = 3;
			*StartColorPattern = 1; //GB
		}
		else if (!xValue && yValue && (mxValue < 2) && !(myValue > 2))  //case 7
		{
			*xOffset = mxValue + 2;
			*yOffset = 2 - myValue;
			*StartColorPattern = 3; //GR
		}
		else if (!xValue && yValue && (mxValue < 2) && (myValue > 2))  //case 8    //
		{
			*xOffset = mxValue + 2;
			*yOffset = 3;
			*StartColorPattern = 0; //BG
		}
		else if( xValue && !yValue && !(mxValue < 2) && !(myValue > 2)) // case 9
		{
			*xOffset = mxValue - 2;
			*yOffset = 2 - myValue;
			*StartColorPattern = 0; //BG
		}
		else if( xValue && !yValue && !(mxValue < 2) && (myValue > 2)) // case 10
		{
			*xOffset = mxValue - 2;
			*yOffset = 3;
			*StartColorPattern = 3; //GR
		}
		else if (xValue && !yValue && (mxValue < 2) && !(myValue > 2))  //case 11
		{
			*xOffset = mxValue + 2;
			*yOffset = 2 - myValue;
			*StartColorPattern = 1; //GB
		}
		else if (xValue && !yValue && (mxValue < 2) && !(myValue > 2))  //case 12
		{
			*xOffset = mxValue + 2;
			*yOffset = 3;
			*StartColorPattern = 2; //RG
		}
		else if (!xValue && !yValue && !(mxValue < 2) && !(myValue > 2)) //case 13
		{
			*xOffset = mxValue - 2;
			*yOffset = 2 - myValue;
			*StartColorPattern = 1; //GB
		}
		else if (!xValue && !yValue && !(mxValue < 2) && (myValue > 2)) //case 14
		{
			*xOffset = mxValue - 2;
			*yOffset = 3;
			*StartColorPattern = 2; //RG
		}
		else if (!xValue && !yValue && (mxValue < 2) && !(myValue > 2)) //case 15
		{
			*xOffset = mxValue + 2;
			*yOffset = 2 - myValue;
			*StartColorPattern = 0; //BG
		}
		else if (!xValue && !yValue && (mxValue < 2) && (myValue > 2)) //case 16    ///
		{
			*xOffset = mxValue + 2;
			*yOffset = 3;
			*StartColorPattern = 3; //GR
		}
	}
	else if ( Windowed && !InvertAboutYAxis && InvertAboutXAxis ) // Windowed, and X inverted Y do not inverted
	{
		int mxValue = NewLeft %4;
		int dxValue = NewLeft /4;
		int myValue = NewBottom %4; 
		int dyValue = NewBottom /4;
		int xValue  = dxValue % 2;
		int yValue  = dyValue % 2;
		if (xValue && yValue && !(mxValue > 2) && !(myValue < 2)) //case 1
		{
			*xOffset = 2 - mxValue ;
			*yOffset = myValue - 2;
			*StartColorPattern = 3; //GR
		}
		else if (xValue && yValue && !(mxValue > 2) && (myValue < 2)) //case 2
		{
			*xOffset = 2 - mxValue ;
			*yOffset = myValue + 2;
			*StartColorPattern = 0; //BG
		}
		else if (xValue && yValue && (mxValue > 2) && !(myValue < 2)) //case 3
		{
			*xOffset = 3 ;
			*yOffset = myValue - 2;
			*StartColorPattern = 2; //RG
		}
		else if (xValue && yValue && (mxValue > 2) && (myValue < 2)) //case 4
		{
			*xOffset = 3 ;
			*yOffset = myValue +2;
			*StartColorPattern = 1; //GB
		}
		else if (!xValue && yValue && !(mxValue > 2) && !(myValue < 2)) //case 5
		{
			*xOffset = 2 - mxValue ;
			*yOffset = myValue - 2;
			*StartColorPattern = 2; //RG
		}
		else if (!xValue && yValue && !(mxValue > 2) && (myValue < 2)) //case 6
		{
			*xOffset = 2 - mxValue ;
			*yOffset = myValue + 2;
			*StartColorPattern = 1; //GB
		}
		else if (!xValue && yValue && (mxValue > 2) && !(myValue < 2)) //case 7
		{
			*xOffset = 3 ;
			*yOffset = myValue - 2;
			*StartColorPattern = 3; //GR
		}
		else if (!xValue && yValue && (mxValue > 2) && (myValue < 2)) //case 8
		{
			*xOffset = 3 ;
			*yOffset = myValue + 2;
			*StartColorPattern = 0; //BG
		}
		else if (xValue && !yValue && !(mxValue > 2) && !(myValue < 2)) //case 9
		{
			*xOffset = 2 - mxValue ;
			*yOffset = myValue - 2;
			*StartColorPattern = 0; //BG
		}
		else if (xValue && !yValue && !(mxValue > 2) && (myValue < 2)) //case 10
		{
			*xOffset = 2 - mxValue ;
			*yOffset = myValue + 2;
			*StartColorPattern = 3; //GR
		}
		else if (xValue && !yValue && (mxValue > 2) && !(myValue < 2)) //case 11
		{
			*xOffset = 3 ;
			*yOffset = myValue - 2;
			*StartColorPattern = 1; //GB
		}
		else if (xValue && !yValue && (mxValue > 2) && (myValue < 2)) //case 12
		{
			*xOffset = 3 ;
			*yOffset = myValue + 2;
			*StartColorPattern = 2; //RG
		}
		else if (!xValue && !yValue && !(mxValue > 2) && !(myValue < 2)) //case 13
		{
			*xOffset = 2 - mxValue ;
			*yOffset = myValue - 2;
			*StartColorPattern = 1; //GB
		}
		else if (!xValue && !yValue && !(mxValue > 2) && (myValue < 2)) //case 14
		{
			*xOffset = 2 - mxValue ;
			*yOffset = myValue + 2;
			*StartColorPattern = 2; //RG
		}
		else if (!xValue && !yValue && (mxValue > 2) && !(myValue < 2)) //case 15
		{
			*xOffset = 3 ;
			*yOffset = myValue - 2;
			*StartColorPattern = 0; //BG
		}
		else if (!xValue && !yValue && (mxValue > 2) && (myValue < 2)) //case 16
		{
			*xOffset = 3 ;
			*yOffset = myValue + 2;
			*StartColorPattern = 3; //GR
		}
	}
	return Status;
}




//
//	FormatImage
//		Format the resident image according to the format setting.
//
bool HIMAGE::FormatImage(int ImageFormat)
{
    bool        ReturnStatus = true;

// Only process if the format is valid.
	if((ImageFormat < 0) || (ImageFormat >= END_HTAG_IMAGE_FORMATS))
	{
		ReturnStatus = false;
	}

// Only format if the image is currently unformatted, returning the format status.
	else if(Format == HTAG_IMAGE_UNFORMATTED_GRAYSCALE)
   {
		Format = ImageFormat;

		switch(Format)
		{
			case HTAG_IMAGE_KIM:
				{
				BITSTRING	ImageCopy(Data->Size, true);

				if(Depth == HTAG_IMAGE_BINARY_IMAGE_DEPTH)
				{
					Binarize(Data, &ImageCopy, XDimension, YDimension, (BinarizationThresholdsApplied > 0) ? true : false);
				}
				else
				{
					ImageCopy.Copy(Data);
				}
				ReturnStatus = CreateKIMImage(Data, &ImageCopy, XDimension, YDimension, Depth);
				}
				break;

			case HTAG_IMAGE_TIF_BINARY:
				{
				BITSTRING	ImageCopy(Data->Size, true);
				Binarize(Data, &ImageCopy, XDimension, YDimension, (BinarizationThresholdsApplied > 0) ? true : false);
				ReturnStatus = CreateTiffImage(Data, &ImageCopy, XDimension, YDimension, false, false, "");
				}
				break;

			case HTAG_IMAGE_TIF_GRP:
				{
				BITSTRING	ImageCopy(Data->Size, true);
				Binarize(Data, &ImageCopy, XDimension, YDimension, (BinarizationThresholdsApplied > 0) ? true : false);
				ReturnStatus = CreateTiffImage(Data, &ImageCopy, XDimension, YDimension, false, true, "");
				}
				break;

			case HTAG_IMAGE_TIF_GRAYSCALE:
				{
				BITSTRING	ImageCopy(Data->Size, true);
				ImageCopy.Copy(Data);
				ReturnStatus = CreateTiffImage(Data, &ImageCopy, XDimension, YDimension, true, false, "");
				}
				break;

			case HTAG_IMAGE_UNFORMATTED_BINARY:
				{
				Binarize(Data, Data, XDimension, YDimension, (BinarizationThresholdsApplied > 0) ? true : false);
				BITSTRING	UnformattedBinaryImage(Data->Size / 8, true);
				int			i, X, Y;
				int			NextBit;
				i = 0;

				for(Y = 0;    Y < YDimension;    ++Y)
				{
					for(X = 0;    X < XDimension;    ++X)
					{
						NextBit = (Data->Char[i] >= Threshold) ? 1 : 0;
						UnformattedBinaryImage.AppendBits(NextBit, 1);
						++i;
					}

				// Move to the next byte if we're not already there.
					UnformattedBinaryImage.FillToNextByteBoundary();
				}

			// Copy back to Data.
				Data->Copy(&UnformattedBinaryImage);
				}
				break;

			case HTAG_IMAGE_UNFORMATTED_GRAYSCALE:
			// Don't do anything, since this is the format we're already in.
				break;

			case HTAG_IMAGE_JPEG:
				{
				int	CompressionMode = (FastJPEG) ? JPEG_LOSSY_FAST_1_0 : JPEG_LOSSY_1_0;
				ReturnStatus = CompressData(Data, CompressionMode, XDimension, XDimension, YDimension, JPEGQualityFactor,
						JPEGCompressionSmoothingFactor, 0);
				}
				break;

			case HTAG_IMAGE_OUTLINED_IMAGE:
				{
				BITSTRING	ImageCopy(Data->Size, true);
				OutlinedBinarize(Data, &ImageCopy, XDimension, YDimension, (BinarizationThresholdsApplied > 0) ? true : false);
				ReturnStatus = CreateKIMImage(Data, &ImageCopy, XDimension, YDimension, HTAG_IMAGE_BINARY_IMAGE_DEPTH);
				}
				break;

			case HTAG_IMAGE_BMP:
				{
					BITSTRING	ImageCopy(Data->Size, true);

					if(Depth == HTAG_IMAGE_BINARY_IMAGE_DEPTH)
				{
					Binarize(Data, &ImageCopy, XDimension, YDimension, (BinarizationThresholdsApplied > 0) ? true : false);
				}
				else
				{
					ImageCopy.Copy(Data);
				}
				ReturnStatus = CreateBMPImage(Data, &ImageCopy, XDimension, YDimension, Depth);
				}
				break;
				
			case HTAG_IMAGE_BMP_COLOR:
				{
					BITSTRING	ImageCopy(Data->Size, true);
					ImageCopy.Copy(Data);

					ReturnStatus = CreateBMPColorImage(Data, &ImageCopy, XDimension, YDimension, Depth,0);
				}
				break;

			case HTAG_IMAGE_HUFFMAN:
				ReturnStatus = CompressData(Data, HUFFMAN_VERSION_2_0, XDimension, XDimension, YDimension, JPEGQualityFactor,
						JPEGCompressionSmoothingFactor, 0);
				break;
			case HTAG_IMAGE_TIF_COLOR_UNCOMPRESSED:
				{
				BITSTRING	ImageCopy(Data->Size, true);
				ImageCopy.Copy(Data);
				ReturnStatus = CreateTiffColorImage(Data, &ImageCopy, XDimension, YDimension, false, "");
				}
				break;
			case HTAG_IMAGE_TIF_COLOR_COMPRESSED:
				{
				BITSTRING	ImageCopy(Data->Size, true);
				ImageCopy.Copy(Data);
				ReturnStatus = CreateTiffColorImage(Data, &ImageCopy, XDimension, YDimension, true, "");
				}
				break;
			case HTAG_IMAGE_JPEG_COLOR:    
				{
				int	CompressionMode = (FastJPEG) ? JPEG_LOSSY_FAST_1_0 : JPEG_LOSSY_1_0;
				ReturnStatus = CompressData(Data, CompressionMode, XDimension, XDimension, YDimension, JPEGQualityFactor,
						JPEGCompressionSmoothingFactor, 1);	
				}
				break;
			case HTAG_IMAGE_UNCOMPRESSED_COLOR:
				ReturnStatus = false;
				break;
				
			case HTAG_IMAGE_UNCOMPRESSED_RAW:
				break;
		}	//end switch
	}
   else
   {
   	ReturnStatus = false;
   }

   return(ReturnStatus);
} // FormatImage




//
//	UnformatImage
//		Unformat the resident image (convert it to a raw image) according to the format setting.
//
bool HIMAGE::UnformatImage(void)
{
	return(UnformatHTagImage(this, FastJPEG, JPEGDecompressionBlockSmoothing));
} // UnformatImage







/*
Need to still add TIFF header parse to determine actual format in Load.
Finish supporting new format.
Finish supporting new unformat.
Finish supporting different header types in Create.
Finish supporting old protocol decoding in Load.
*/
