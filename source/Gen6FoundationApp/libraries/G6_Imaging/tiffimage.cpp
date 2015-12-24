//======================================================================================
// TiffImage.cpp
//======================================================================================
// $RCSfile: tiffimage.cpp $
// $Revision: 1.4 $
// $Date: 2011/06/17 04:37:30EDT $
//======================================================================================

//======================================================================================



//#define NEW_SUBFILE_TYPE_TAG        0xfe
//#define SAMPLES_PER_PIXEL_TAG       0x115
//#define PLANAR_CONFIG_TAG           0x11c
//#define PREDICTOR_TAG               0x13d
//#define PALETTE_TAG                 0x140





//---------------------------------------------------------------------------
//
//  Module Name:     TiffImage
//
//---------------------------------------------------------------------------


#include	"HHPImagingConfig.h"
#include	"TiffImage.h"		// TiffImage interface definition


#ifdef TIFF_DECOMPRESSION
#include "TIFF_CSB.h"
#include "TIFF_Fax.h"
#include "TIFF_TiffCon.hpp"
#endif


//---------------------------------------------------------------------------








#include <stdlib.h>
//#include <memory.h>
#include <string.h>
#include "LZWCompressor.h"


#define White					0   // compression only works when white 0, black 1
#define Black 					1
#define RGB 					2

#define T6_FLAG_BYTE 0x20          /* bit 0 = nothing, bit 1 = do we allow uncompressed if smaller? (1 yes, 0 no) */

#define RESOLUTION_UNIT 1   /* 1 = none, 2 = inch, 3 = centimeter */


/* X and Y resolution is a rational number.  And the resolution is then that
number per resolution unit*/

#define X_RESOLUTION_NUMERATOR 0x12c
#define X_RESOLUTION_DENOMINATOR 1

#define Y_RESOLUTION_NUMERATOR 0x12c
#define Y_RESOLUTION_DENOMINATOR 1


// Header Info
#define TIFF_HEADER_SIZE			8				// Size of a TIFF header
#define TIFF_ID                     0x4949
#define TIFF_VER                    0x002a


#define TIFF_TAG_ENTRY_SIZE			12				// Size of a TIFF tag
#define	TIFF_SIZE_OF_NUMBER_OF_ENTRIES	2
#define	TIFF_SIZE_OF_END_MARKER		4
#define	TIFF_SIZE_OF_X_RESOLUTION	8
#define	TIFF_SIZE_OF_Y_RESOLUTION	8
#define	TIFF_SIZE_OF_BITS_PER_SAMPLE    6

// TagType
#define TiffBYTE                    1
#define TiffASCII                   2
#define TiffWORD                    3
#define TiffDWORD                   4
#define TiffRational                5
// Tag Numbers.
#define NEW_SUBFILE_TYPE_TAG        0xfe
#define IMAGE_WIDTH_TAG             0x100
#define IMAGE_HEIGHT_TAG            0x101
#define IMAGE_BITS_PER_PIXEL_TAG    0x102
#define IMAGE_COMPRESSION_TAG       0x103
#define PHOTOMETRIC_INTERP_TAG      0x106
#define FILL_ORDER_TAG				0x10A
#define IMAGE_DESCRIPTION_TAG		0x10E
#define IMAGE_DATA_OFFSET_TAG       0x111
#define SAMPLES_PER_PIXEL_TAG       0x115
#define ROWS_PER_STRIP_TAG          0x116
#define BYTES_PER_STRIP_TAG         0x117
#define X_RESOLUTION_TAG            0x11a
#define Y_RESOLUTION_TAG            0x11b
#define PLANAR_CONFIG_TAG           0x11c
#define	T6_FLAG_BYTE_TAG			0x125
#define RESOLUTION_UNIT_TAG         0x128
#define PREDICTOR_TAG               0x13d
#define PALETTE_TAG                 0x140
#define	CAERE_PROPRIETARY_TAG		0x857b
// Number of Tags
#define NUM_8BIT_TIFF_TAGS          16
#define NUM_1BIT_TIFF_TAGS          11
#define TIFF_OFFSET_TO_DATA         0xd0






/* Added by MDG in order have option of passing alloc responsibilities to caller */
int comp_write_init_flag;


/***********************************************************************
*   Append_Tiff_Header - This should be the first thing called when making
*	 a TIFF file.  This tells: first the byte order of multiple byte data such
*   as integers and such, second it tells the world that this is indeed a
*   TIFF file, and third it tells where the Image File Directory can be found
*   with respect to the beginning of the file.
***********************************************************************/
//
//	Tag format -
//		2 byte unsigned		TiffID
//		2 byte unsigned		TiffVersion
//		4 byte unsigned		OffsetToTags
//
void Append_Tiff_Header(BITSTRING *TiffImage)
{
	TiffImage->AppendNumber(TIFF_ID, 2);
	TiffImage->AppendNumber(TIFF_VER, 2);
	TiffImage->AppendNumber(TIFF_HEADER_SIZE, 4);
}

/*******************************************************************************************************
	Append_ImageFileDirectory -  Outputs all of the tags needed for the compression type 4 TIFF file
*******************************************************************************************************/
//
//	Tag format -
//		2 byte unsigned		Tag
//		2 byte unsigned		TagType
//		4 byte unsigned		Length
//		4 byte unsigned		Value
//

bool AppendTiffHeaderAndIFD(int XDimension, int YDimension, long nBytesInImage, bool GrayscaleImage,
					bool CompressedImage, const char *pMessage, BITSTRING *TiffImage, bool AppendCaereTag)
{

   int nNumberIFDentries=12,nCompressionType=1,xResolutionOffset,yResolutionOffset;
   int TiffStripOffset,MessageSize=0,MessageOffset;//,z=0;
   short nBitsPerSample=1,PhotometricInterpretation=White;


	if(!GrayscaleImage)
	{
		if(!CompressedImage)
		{
			nNumberIFDentries = 12;
			nCompressionType = 1;
			nBitsPerSample = 1;
		}
		else
		{
			if (AppendCaereTag)
				nNumberIFDentries = 14;
			else
				nNumberIFDentries = 13;
			nCompressionType = 4;
			nBitsPerSample = 1;
		}
	}
	else
	{
		nNumberIFDentries = 12;
		nCompressionType = 1;
		nBitsPerSample = 8;
		PhotometricInterpretation = Black;
	}

	if (pMessage != NULL)
	{
		nNumberIFDentries+=1;
		MessageSize = strlen(pMessage)+1;
	}

	xResolutionOffset = TIFF_HEADER_SIZE + TIFF_SIZE_OF_NUMBER_OF_ENTRIES + (nNumberIFDentries * TIFF_TAG_ENTRY_SIZE) +
						TIFF_SIZE_OF_END_MARKER;
	yResolutionOffset = xResolutionOffset + TIFF_SIZE_OF_X_RESOLUTION;
	MessageOffset = yResolutionOffset + TIFF_SIZE_OF_Y_RESOLUTION;

	if (MessageSize > 4)   // if the message size (including the null) is greater than four characters, we need a place to
								  // to put the message
		TiffStripOffset = MessageOffset + MessageSize;
	else                   // if the message size is less than four characters, we can fit the message right into the message
								  // tag and won't bother needing room for the message.
		TiffStripOffset = MessageOffset;


   	Append_Tiff_Header(TiffImage);

/* The first thing in the IFD is to tell how many tags there will be in the image*/
	TiffImage->AppendNumber(nNumberIFDentries, 2);

/* IMAGE WIDTH(256) */
	TiffImage->AppendNumber(IMAGE_WIDTH_TAG, 2);
	TiffImage->AppendNumber(TiffDWORD, 2);
	TiffImage->AppendNumber(1, 4);					// Only one value.
	TiffImage->AppendNumber(XDimension, 4);

/* IMAGE LENGTH(257)*/
	TiffImage->AppendNumber(IMAGE_HEIGHT_TAG, 2);
	TiffImage->AppendNumber(TiffDWORD, 2);
	TiffImage->AppendNumber(1, 4);					// Only one value.
	TiffImage->AppendNumber(YDimension, 4);

/* BITS PER SAMPLE (258) */
	TiffImage->AppendNumber(IMAGE_BITS_PER_PIXEL_TAG, 2);
	TiffImage->AppendNumber(TiffWORD, 2);
	TiffImage->AppendNumber(1, 4);					// Only one value.
	TiffImage->AppendNumber(nBitsPerSample, 4);

/* COMPRESSION(259) */
	TiffImage->AppendNumber(IMAGE_COMPRESSION_TAG, 2);
	TiffImage->AppendNumber(TiffWORD, 2);
	TiffImage->AppendNumber(1, 4);					// Only one value.
	TiffImage->AppendNumber(nCompressionType, 4);

/* PHOTOMETRIC INTERPRETATION(262) */
	TiffImage->AppendNumber(PHOTOMETRIC_INTERP_TAG, 2);
	TiffImage->AppendNumber(TiffWORD, 2);
	TiffImage->AppendNumber(1, 4);					// Only one value.
	TiffImage->AppendNumber(PhotometricInterpretation, 4);

/* FILL ORDER (266) */
	TiffImage->AppendNumber(FILL_ORDER_TAG, 2);
	TiffImage->AppendNumber(TiffWORD, 2);
	TiffImage->AppendNumber(1, 4);					// Only one value.
	TiffImage->AppendNumber(1, 4);

/* IMAGE DESCRIPTION (270) */
	if(pMessage != NULL)
	{
		TiffImage->AppendNumber(IMAGE_DESCRIPTION_TAG, 2);
		TiffImage->AppendNumber(TiffASCII, 2);
	// if the message size is less than four characters (including the null character) then the message can fit right in the
	// tag itself.  If it is longer however, we need to input an offset to the string in the tag
		if (MessageSize > 4)
		{
			TiffImage->AppendNumber(MessageSize, 4);					// Only one value.
			TiffImage->AppendNumber(MessageOffset, 4);
		}
		else
		{
			TiffImage->AppendNumber(MessageSize, 4);					// Only one value.
			TiffImage->Append(pMessage, 4);
		}
	}

/* STRIP OFFSETS(273) */
	TiffImage->AppendNumber(IMAGE_DATA_OFFSET_TAG, 2);
	TiffImage->AppendNumber(TiffDWORD, 2);
	TiffImage->AppendNumber(1, 4);					// Only one value.
	TiffImage->AppendNumber(TiffStripOffset, 4);

/* ROWS PER STRIP (278) */
	TiffImage->AppendNumber(ROWS_PER_STRIP_TAG, 2);
	TiffImage->AppendNumber(TiffDWORD, 2);
	TiffImage->AppendNumber(1, 4);					// Only one value.
	TiffImage->AppendNumber(YDimension, 4);

/* STRIP BYTE COUNTS (279) */
	TiffImage->AppendNumber(BYTES_PER_STRIP_TAG, 2);
	TiffImage->AppendNumber(TiffDWORD, 2);
	TiffImage->AppendNumber(1, 4);					// Only one value.
	TiffImage->AppendNumber( (int) nBytesInImage, 4);

/* XRESOLUTION (282) */
	TiffImage->AppendNumber(X_RESOLUTION_TAG, 2);
	TiffImage->AppendNumber(TiffRational, 2);
	TiffImage->AppendNumber(1, 4);					// Only one value.
	TiffImage->AppendNumber(xResolutionOffset, 4);

/* YRESOLUTION (283) */
	TiffImage->AppendNumber(Y_RESOLUTION_TAG, 2);
	TiffImage->AppendNumber(TiffRational, 2);
	TiffImage->AppendNumber(1, 4);					// Only one value.
	TiffImage->AppendNumber(yResolutionOffset, 4);

	if ((!GrayscaleImage) && (CompressedImage))
	{
	/* T6OPTIONS (293) */
		TiffImage->AppendNumber(T6_FLAG_BYTE_TAG, 2);
		TiffImage->AppendNumber(TiffDWORD, 2);
		TiffImage->AppendNumber(1, 4);					// Only one value.
		TiffImage->AppendNumber(T6_FLAG_BYTE, 4);
	}

/* RESOLUTION UNIT (296)  */
	TiffImage->AppendNumber(RESOLUTION_UNIT_TAG, 2);
	TiffImage->AppendNumber(TiffWORD, 2);
	TiffImage->AppendNumber(1, 4);					// Only one value.
	TiffImage->AppendNumber(RESOLUTION_UNIT, 4);

/* CAERE's PROPRIETARY TAG */
	if ((AppendCaereTag)&&((!GrayscaleImage) && (CompressedImage)))
	{
		long caere_file_size = (long) (TIFF_HEADER_SIZE + 2 + 12 * nNumberIFDentries + 4 + 8 + 8 + (int)nBytesInImage);
		if ((pMessage != NULL)&&(MessageSize > 4)) caere_file_size += MessageSize;

		TiffImage->AppendNumber(CAERE_PROPRIETARY_TAG, 2);
		TiffImage->AppendNumber(TiffDWORD, 2);
		TiffImage->AppendNumber(1, 4);					// Only one value.
		TiffImage->AppendNumber( (int) caere_file_size, 4);
	}

/* END OF IFD */
	TiffImage->AppendNumber(0, 4);

/* XRESOLUTION DATA */
	TiffImage->AppendNumber(X_RESOLUTION_NUMERATOR, 4);
	TiffImage->AppendNumber(X_RESOLUTION_DENOMINATOR, 4);

/* YRESOLUTION DATA */
	TiffImage->AppendNumber(Y_RESOLUTION_NUMERATOR, 4);
	TiffImage->AppendNumber(Y_RESOLUTION_DENOMINATOR, 4);


	if ((pMessage != NULL)&&(MessageSize > 4))  // if the message doesn't fit in the tag itself, put it here.
	{
		TiffImage->Append(pMessage, MessageSize);
	}


	return true;
}


/*******************************************************************************************************
	Append_ImageFileDirectory -  Outputs all of the tags needed for the compression type 4 TIFF file
*******************************************************************************************************/
//
//	Tag format -
//		2 byte unsigned		Tag
//		2 byte unsigned		TagType
//		4 byte unsigned		Length
//		4 byte unsigned		Value
//

bool AppendTiffHeaderAndIFDColor(int XDimension, int YDimension, long nBytesInImage,
					bool CompressedImage, const char *pMessage, BITSTRING *TiffImage)
{

   int nNumberIFDentries=12,nCompressionType=1,xResolutionOffset,yResolutionOffset, BitsPerSampleOffset;
   int TiffStripOffset,MessageSize=0,MessageOffset;//,z=0;
   short nBitsPerSample=1,PhotometricInterpretation=RGB;
   short SamplesPerPixel = 3;


	nNumberIFDentries = 12;
	nBitsPerSample = 8;
	nCompressionType = CompressedImage ? 5 : 1;	// We will use LZW algorithm
	
	if (pMessage != NULL)
	{
		nNumberIFDentries+=1;
		MessageSize = strlen(pMessage)+1;
	}

	xResolutionOffset = TIFF_HEADER_SIZE + TIFF_SIZE_OF_NUMBER_OF_ENTRIES + (nNumberIFDentries * TIFF_TAG_ENTRY_SIZE) +
						TIFF_SIZE_OF_END_MARKER;
	yResolutionOffset = xResolutionOffset + TIFF_SIZE_OF_X_RESOLUTION;
	BitsPerSampleOffset = yResolutionOffset + TIFF_SIZE_OF_Y_RESOLUTION;
	MessageOffset = BitsPerSampleOffset + TIFF_SIZE_OF_BITS_PER_SAMPLE;

	if (MessageSize > 4)   // if the message size (including the null) is greater than four characters, we need a place to
								  // to put the message
		TiffStripOffset = MessageOffset + MessageSize;
	else                   // if the message size is less than four characters, we can fit the message right into the message
								  // tag and won't bother needing room for the message.
		TiffStripOffset = MessageOffset;


   	Append_Tiff_Header(TiffImage);

/* The first thing in the IFD is to tell how many tags there will be in the image*/
	TiffImage->AppendNumber(nNumberIFDentries, 2);

/* IMAGE WIDTH(256) */
	TiffImage->AppendNumber(IMAGE_WIDTH_TAG, 2);
	TiffImage->AppendNumber(TiffDWORD, 2);
	TiffImage->AppendNumber(1, 4);					// Only one value.
	TiffImage->AppendNumber(XDimension, 4);

/* IMAGE LENGTH(257)*/
	TiffImage->AppendNumber(IMAGE_HEIGHT_TAG, 2);
	TiffImage->AppendNumber(TiffDWORD, 2);
	TiffImage->AppendNumber(1, 4);					// Only one value.
	TiffImage->AppendNumber(YDimension, 4);

/* BITS PER SAMPLE (258) */
	TiffImage->AppendNumber(IMAGE_BITS_PER_PIXEL_TAG, 2);
	TiffImage->AppendNumber(TiffWORD, 2);
	TiffImage->AppendNumber(3, 4);					// 3 values.
	TiffImage->AppendNumber(BitsPerSampleOffset, 4);

/* COMPRESSION(259) */
	TiffImage->AppendNumber(IMAGE_COMPRESSION_TAG, 2);
	TiffImage->AppendNumber(TiffWORD, 2);
	TiffImage->AppendNumber(1, 4);					// Only one value.
	TiffImage->AppendNumber(nCompressionType, 4);

/* PHOTOMETRIC INTERPRETATION(262) */
	TiffImage->AppendNumber(PHOTOMETRIC_INTERP_TAG, 2);
	TiffImage->AppendNumber(TiffWORD, 2);
	TiffImage->AppendNumber(1, 4);					// Only one value.
	TiffImage->AppendNumber(PhotometricInterpretation, 4);

/* IMAGE DESCRIPTION (270) */
	if(pMessage != NULL)
	{
		TiffImage->AppendNumber(IMAGE_DESCRIPTION_TAG, 2);
		TiffImage->AppendNumber(TiffASCII, 2);
	// if the message size is less than four characters (including the null character) then the message can fit right in the
	// tag itself.  If it is longer however, we need to input an offset to the string in the tag
		if (MessageSize > 4)
		{
			TiffImage->AppendNumber(MessageSize, 4);					// Only one value.
			TiffImage->AppendNumber(MessageOffset, 4);
		}
		else
		{
			TiffImage->AppendNumber(MessageSize, 4);					// Only one value.
			TiffImage->Append(pMessage, 4);
		}
	}

/* STRIP OFFSETS(273) */
	TiffImage->AppendNumber(IMAGE_DATA_OFFSET_TAG, 2);
	TiffImage->AppendNumber(TiffDWORD, 2);
	TiffImage->AppendNumber(1, 4);					// Only one value.
	TiffImage->AppendNumber(TiffStripOffset, 4);

/* SAMPLES PER PIXEL (277) */
	TiffImage->AppendNumber(SAMPLES_PER_PIXEL_TAG, 2);
	TiffImage->AppendNumber(TiffWORD, 2);
	TiffImage->AppendNumber(1, 4);					// Only one value.
	TiffImage->AppendNumber(SamplesPerPixel, 4);

/* ROWS PER STRIP (278) */
	TiffImage->AppendNumber(ROWS_PER_STRIP_TAG, 2);
	TiffImage->AppendNumber(TiffDWORD, 2);
	TiffImage->AppendNumber(1, 4);					// Only one value.
	TiffImage->AppendNumber(YDimension, 4);

/* STRIP BYTE COUNTS (279) */
	TiffImage->AppendNumber(BYTES_PER_STRIP_TAG, 2);
	TiffImage->AppendNumber(TiffDWORD, 2);
	TiffImage->AppendNumber(1, 4);					// Only one value.
	TiffImage->AppendNumber( (int) nBytesInImage, 4);

/* XRESOLUTION (282) */
	TiffImage->AppendNumber(X_RESOLUTION_TAG, 2);
	TiffImage->AppendNumber(TiffRational, 2);
	TiffImage->AppendNumber(1, 4);					// Only one value.
	TiffImage->AppendNumber(xResolutionOffset, 4);

/* YRESOLUTION (283) */
	TiffImage->AppendNumber(Y_RESOLUTION_TAG, 2);
	TiffImage->AppendNumber(TiffRational, 2);
	TiffImage->AppendNumber(1, 4);					// Only one value.
	TiffImage->AppendNumber(yResolutionOffset, 4);

/* RESOLUTION UNIT (296)  */
	TiffImage->AppendNumber(RESOLUTION_UNIT_TAG, 2);
	TiffImage->AppendNumber(TiffWORD, 2);
	TiffImage->AppendNumber(1, 4);					// Only one value.
	TiffImage->AppendNumber(RESOLUTION_UNIT, 4);

/* END OF IFD */
	TiffImage->AppendNumber(0, 4);

/* XRESOLUTION DATA */
	TiffImage->AppendNumber(X_RESOLUTION_NUMERATOR, 4);
	TiffImage->AppendNumber(X_RESOLUTION_DENOMINATOR, 4);

/* YRESOLUTION DATA */
	TiffImage->AppendNumber(Y_RESOLUTION_NUMERATOR, 4);
	TiffImage->AppendNumber(Y_RESOLUTION_DENOMINATOR, 4);

/* BITS PER SAMPLE */
	TiffImage->AppendNumber(nBitsPerSample, 2);
	TiffImage->AppendNumber(nBitsPerSample, 2);
	TiffImage->AppendNumber(nBitsPerSample, 2);
	
	if ((pMessage != NULL)&&(MessageSize > 4))  // if the message doesn't fit in the tag itself, put it here.
	{
		TiffImage->Append(pMessage, MessageSize);
	}


	return true;
}



//
//	CreateTiffImage
//		Create an 8 or 1 bit per pixel Tiff image from the input image.
//
bool CreateTiffImage(BITSTRING *TiffImage, BITSTRING *InputImage, int XDimension, int YDimension, bool GrayscaleImage,
					bool CompressedImage, const char *pMessage, bool AppendCaereTag, int Threshold)
{
	int				i,x,y;
	unsigned long			nUncompressedBytes,nCompressedBytes;
	bool			Successful=false;
	BITSTRING		*pUncompressedBinaryImage, *pCompressedBinaryImage;
	int				AdjustedXDimension;




//	RESET_WATCHDOG_TIMER();

	TiffImage->Zero();


	if (CompressedImage) // they want a compressed image
		AdjustedXDimension = (XDimension+7)&0xFFFFFFF8; // However, when the file is compressed, the compression algorithm
														// assumes that each row ends on a byte boundry, so we have to make sure
														// that happens. what this will do is add as many pixels as needed to the
														// end of each row to make it end on a byte boundry.  That way we know
														// that we have the whole image.
	else
		AdjustedXDimension = XDimension;





	if(!GrayscaleImage) // if this is a binary image of some type
	{
	// nUncompressedBytes is the height*width of the image divided by the number of bits in an unsigned char.  The
	// extra bytes ((x1-x0) equal to the height of the image) is for the uncompressed type of file.  Since the uncompressed
	// raster image assumes that each row starts on a new byte, we need buffer space so we have the room to assume we
	// can do this.
		nUncompressedBytes = (unsigned long) ((AdjustedXDimension * YDimension)/(sizeof(unsigned char)*8) + (AdjustedXDimension));
		pUncompressedBinaryImage = new BITSTRING(nUncompressedBytes, true);
	   	if (pUncompressedBinaryImage != NULL)
	   	{
		// Here we will extract the binary raster image from the binarized image pointed to by "di"
			i = 0;
			for (y=0;y<YDimension;y++)
			{
				for (x=0; x<XDimension; x++)
				{
					pUncompressedBinaryImage->AppendBits(((unsigned int) InputImage->Char[i++] < (unsigned int) Threshold) ? 1 : 0, 1);
				}// end for x

				pUncompressedBinaryImage->FillToNextByteBoundary(false);
			}// end for y


			if (CompressedImage) // they want a compressed image
			{
				pCompressedBinaryImage = new BITSTRING(nUncompressedBytes, true);
	      		if (pCompressedBinaryImage != NULL)// make room for the compressed
		  		{                                                                                 // image
      				grp4comp(pUncompressedBinaryImage->Char, pUncompressedBinaryImage->Size,
      							AdjustedXDimension, YDimension, pCompressedBinaryImage->Char, (long *)&nCompressedBytes); // compress the image
					delete pUncompressedBinaryImage;   // since we have the compressed image, we don't need the uncompressed one

					if (AppendTiffHeaderAndIFD(AdjustedXDimension, YDimension, nCompressedBytes, GrayscaleImage, CompressedImage,
										pMessage, TiffImage, AppendCaereTag))
					{
					// stick all of the tiff stuff on, and if buffered output, where in the buffer we are to start
						TiffImage->Append(pCompressedBinaryImage->Char, nCompressedBytes);
						Successful = true; // successful image capture
					}
					else
					{
						TRACE("Append Tiff failed");
					}
					delete pCompressedBinaryImage;  		// deallocate compressed image space
				} // end if (pCompressedBinaryImage = (unsigned char *)MALLOC....
				else
				{
					delete pUncompressedBinaryImage; /* even though we had a malloc fail...lets free the item we did get above to avoid cascading the problem */
					TRACE("Could not malloc enough memory for the compressed image \r");
				}
			} // end if filetype is compressed

			else // filetype is uncompressed binary
			{
				if (AppendTiffHeaderAndIFD(AdjustedXDimension, YDimension, pUncompressedBinaryImage->Size, GrayscaleImage,
										CompressedImage, pMessage, TiffImage, AppendCaereTag))
				{
									// stick all of the TIFF stuff on, and get back the buffer offset for the image if needed
					TiffImage->Append(pUncompressedBinaryImage);
					Successful = true;
				}
				else
				{
					TRACE("Append Tiff failed");
				}
				delete pUncompressedBinaryImage;         // deallocate this big chunk of memory
			} // end else filetype is uncompressed
		}// end if pUncompressedBinaryImage
		else
		{
			TRACE("Could not malloc enough memory for the uncompressed image \r");
		}
	}// end - if this is a binary image



	else // we have a grayscale image
	{
		nUncompressedBytes = (unsigned long) (YDimension*XDimension);    // since each pixel is a byte, the image size is just height * width

   	// get the header and Image Field Directory and if buffered output, get back where we are to start putting image
		if (AppendTiffHeaderAndIFD(XDimension, YDimension, nUncompressedBytes, GrayscaleImage, CompressedImage, pMessage,
						TiffImage, AppendCaereTag))
		{
			TiffImage->Append(InputImage->Char, nUncompressedBytes);
			Successful = true;
		}
		else
		{
			TRACE("Append Tiff failed");
		}
	}

//	RESET_WATCHDOG_TIMER();

	return Successful;
} // CreateTiffImage


//
//	CreateTiffColorImage
//		Create an 24 bit per pixel Tiff image from the input image.
//
bool CreateTiffColorImage(BITSTRING *TiffImage, BITSTRING *InputImage, int XDimension, int YDimension,
					bool CompressedImage, const char *pMessage, bool AppendCaereTag)
{
	unsigned long		nUncompressedBytes;
	bool				Successful=false;

	TiffImage->Zero();

	nUncompressedBytes = (unsigned long) (YDimension*XDimension*3);    // since each pixel is a byte, the image size is just height * width

   	// get the header and Image Field Directory and if buffered output, get back where we are to start putting image
   		if(!CompressedImage){
			if (AppendTiffHeaderAndIFDColor(XDimension, YDimension, nUncompressedBytes, CompressedImage, pMessage,
							TiffImage))
			{
				TiffImage->Append(InputImage->Char, nUncompressedBytes);
				Successful = true;
			}
			else
			{
				TRACE("Append Tiff failed");
			}
   		}else{
			if (AppendTiffHeaderAndIFDColor(XDimension, YDimension, nUncompressedBytes, CompressedImage, pMessage,
							TiffImage))
			{
				CLZWCompressor Compressor;
				Compressor.TiffCompress(TiffImage, InputImage, nUncompressedBytes);
				Successful = true;
			}
			else
			{
				TRACE("Append Tiff failed");
			}
   		}

//	RESET_WATCHDOG_TIMER();

	return Successful;
} // CreateTiffImage


/***********************************************************************
*   grp4comp is the main routine of this file.  It does pre-           *
*   liminary setup, calls routines, and does final processing.         *
************************************************************************/

/***********************************************************************
*  Arguments          						       *
*  ---------                					       *
*	Passed in:         					       *
*		   indata - buffer containing the uncompressed data.   *
*		   inbytes - the number of bytes in indata.            *
*  		   width - Width in pixels of scan line in indata.     *
*  		   height - Number of lines in indata.                 *
*	Returned:          					       *
*		   outdata - buffer containing the compressed data.    *
*		   outbytes - the number of bytes in outdata.          *
************************************************************************/

void grp4comp(unsigned char *indata,long /* inbytes */,int width,int height,unsigned char *outdata,long *outbytes)
{
   struct uncompressed_descriptor uncompressed;
   struct compressed_descriptor compressed;

   uncompressed.pixels_per_line = width;
   uncompressed.number_of_lines = height;
   uncompressed.data = indata;
   comp_write_init_flag = 1;		// TRUE
	read_uncompressed_file_into_memory( &uncompressed);
   compressed.data = outdata;
	control_compression( &uncompressed, &compressed );
   *outbytes = compressed.length_in_bytes;
}


/***************************** control_compression **************************

		calls the functions that compress the image

*****************************************************************************/
/***********************************************************************
*  Arguments          						       *
*  ---------                					       *
*	Passed in:         					       *
*  		   uncompressed	- structure containing the # of pixels *
*				  per line, the number of lines, and   *
*				  the uncompressed data.               *
*	Returned:          					       *
*  		   compressed	- structure containing the # of pixels *
*				  per line, the number of lines, and   *
*				  the compressed data.                 *
************************************************************************/
void control_compression(struct uncompressed_descriptor *uncompressed,struct compressed_descriptor *compressed)
{
	struct parameters 				 sole_parameters;
	struct parameters 				*params = &sole_parameters;

	prepare_to_compress( uncompressed, compressed, params );
	compress_image( uncompressed, compressed, params );
   FREE(params->reference_line);
   FREE(params->coding_line);
}


/************************ read_uncompressed_file_into_memory *******************

		allocates memory for the uncompressed image.

*****************************************************************************/
/***********************************************************************
*  Arguments          						       *
*  ---------                					       *
*	Passed in:         					       *
*  		   uncompressed	- structure containing the # of pixels *
*				  per line, the number of lines, and   *
*				  the uncompressed data.               *
*	Returned:          					       *
*  		   uncompressed	- structure containing the # of pixels *
*				  per line, the number of lines, and   *
*				  the compressed data.                 *
************************************************************************/
void read_uncompressed_file_into_memory(struct uncompressed_descriptor *uncompressed)
{
	if(uncompressed->data == NULL)
	{
//   	printf("\nNo memory allocated for input data!\n");
      crash_c();
   }
} /* end read_uncompressed_file_into_memory() */



/*************************** prepare_to_compress ****************************

		initializes variables in preperation for compression

*****************************************************************************/
/***********************************************************************
*  Arguments          						       *
*  ---------                					       *
*	Passed in:         					       *
*  		   uncompressed	- structure containing the # of pixels *
*				  per line, the number of lines, and   *
*				  the uncompressed data.               *
*	Returned:          					       *
*  		   compressed	- structure containing the # of pixels *
*				  per line, the number of lines, and   *
*				  the compressed data.                 *
*		   params - structure storing information needed for   *
*		   	    comparison and other tasks.		       *
************************************************************************/
void prepare_to_compress(struct uncompressed_descriptor  *uncompressed,struct compressed_descriptor 	*compressed,
struct parameters *params)
{

	params->max_pixel   	    = uncompressed->pixels_per_line;
	compressed->pixels_per_line = uncompressed->pixels_per_line;
	compressed->number_of_lines = uncompressed->number_of_lines;

	set_up_first_line_c( params );
	prepare_to_write_bits_c( compressed );

} /* end prepare_to_compress() */



/****************************** compress_image *******************************

				compresses the image

*****************************************************************************/
/***********************************************************************
*  Arguments          						       *
*  ---------                					       *
*	Passed in:         					       *
*  		   uncompressed	- structure containing the # of pixels *
*				  per line, the number of lines, and   *
*				  the uncompressed data.               *
*	Returned:          					       *
*  		   compressed	- structure containing the # of pixels *
*				  per line, the number of lines, and   *
*				  the compressed data.                 *
*		   params - structure storing information need for     *
*		   	    comparison and other tasks.		       *
************************************************************************/
void compress_image(struct uncompressed_descriptor *uncompressed,struct compressed_descriptor *compressed,
		struct parameters *params)
{
short  line;

	for(line = 0; line < uncompressed->number_of_lines; line++) {
	     make_array_of_changing_elements( params, uncompressed, line );
	     set_up_first_and_last_changing_elements_c( params );
	     compress_line( params );
	     prepare_to_compress_next_line( params );
	} /* end for each line loop */

	write_bits_c("000000000001000000000001");
	compressed->length_in_bytes = flush_buffer();
}



/************************ make_array_of_changing_elements *********************

	stores in a list pointed to by "params->coding_line" the pixel numbers
	of all the changing elements in the coding line

*****************************************************************************/
/***********************************************************************
*  Arguments          						       *
*  ---------                					       *
*	Passed in:         					       *
*  		   uncompressed	- structure containing the # of pixels *
*				  per line, the number of lines, and   *
*				  the uncompressed data.               *
*		   line_number -  the number of the line in the image  *
*	Returned:          					       *
*		   params - structure storing information need for     *
*		   	    comparison and other tasks.		       *
************************************************************************/
void make_array_of_changing_elements(struct parameters *params,struct uncompressed_descriptor *uncompressed,
		short line_number)
{
short 	bytes_per_line;
int	line_offset;
short 	byte_offset;

	bytes_per_line = params->max_pixel / Pixels_per_byte;
	line_offset = bytes_per_line * line_number;
	for(byte_offset=0; byte_offset < bytes_per_line; byte_offset++) {
	    process_char(*(uncompressed->data+line_offset+byte_offset),params);
	}

}	/* end make_array_of_changing_elements() */



/******************* set_up_first_and_last_changing_elements_c *****************

	initializes the first and last changing elements in the coding line

******************************************************************************/
/***********************************************************************
*  Arguments          						       *
*  ---------                					       *
*	Passed in:         					       *
*		   params - structure storing information need for     *
*		   	    comparison and other tasks.		       *
*	Returned:          					       *
*		   params - structure storing information need for     *
*		   	    comparison and other tasks.		       *
************************************************************************/
void set_up_first_and_last_changing_elements_c(struct parameters *params)
{
	*(params->coding_line) = Invalid;
	*(params->coding_line + ++params->index) = params->max_pixel;
	*(params->coding_line + ++params->index) = params->max_pixel;
	*(params->coding_line + ++params->index) = params->max_pixel;

	/* the previous lines may be necessary if when searching for b1, you
	skip some elements because you know that they are the wrong color */
}


/************************ prepare_to_compress_next_line ***********************

	initializes variables in preperation for compressing another line

******************************************************************************/
/***********************************************************************
*  Arguments          						       *
*  ---------                					       *
*	Passed in:         					       *
*		   params - structure storing information need for     *
*		   	    comparison and other tasks.		       *
*	Returned:          					       *
*		   params - structure storing information need for     *
*		   	    comparison and other tasks.		       *
************************************************************************/
void prepare_to_compress_next_line(struct parameters *params)
{
short *temp;

	/* swap the reference and unchanged coding lines */

	temp = params->reference_line;
	params->reference_line = params->coding_line;
	params->coding_line = temp;

	params->pixel = 0;
	params->index = 0;
	params->previous_color = White;

} /* end prepare_to_read_next_line() */



/******************************* set_up_first_line_c ***************************

	initializes variables in preperation for compressing the first line

******************************************************************************/
/***********************************************************************
*  Arguments          						       *
*  ---------                					       *
*	Passed in:         					       *
*		   params - structure storing information need for     *
*		   	    comparison and other tasks.		       *
*	Returned:          					       *
*		   params - structure storing information need for     *
*		   	    comparison and other tasks.		       *
************************************************************************/
void set_up_first_line_c(struct parameters *params)
{

	params->reference_line =
	 (short *)MALLOC( (params->max_pixel + Extra_positions) * sizeof(short) );
	params->coding_line =
	 (short *)MALLOC( (params->max_pixel + Extra_positions) * sizeof(short) );

	*(params->reference_line + 0) = Invalid;
	*(params->reference_line + 1) = params->max_pixel;
	*(params->reference_line + 2) = params->max_pixel;
	*(params->reference_line + 3) = params->max_pixel;

	/* initialize first changing element on coding line (A0 = -1) */
	*(params->coding_line) = Invalid;

	params->pixel = 0;
	params->index = 0;
	params->previous_color = White;

} /* end set_up_first_line_c() */



/*********************************** crash_c ***********************************

		forces the program to crash and create a core file

*****************************************************************************/
void crash_c()
{
//FILE *crash_program = NULL;
//	fprintf(crash_program,"This will kill the program and create a core file");
}


/***************************************************************************/
/* Originally mode.c                                                       */
/***************************************************************************/

static short	A0,		A0_color,
				A1,		a2,
				b1,		b2;



/******************************* compress_line ********************************

			compresses a single line of the image

*****************************************************************************/
void compress_line(struct parameters *params)
{

	A0 = Invalid; /* set A0 equal to imaginary first array element */
	A0_color = White;
	A1 = 1;
	initialize_b1(params);
	b2 = b1 + 1;

	do {

		if (*(params->reference_line + b2) < *(params->coding_line + A1)) {
			pass_mode_c(params);
			continue;
		} else
			if (abs(*(params->coding_line+A1)-*(params->reference_line+b1)) <=3)
			 	vertical_mode_c(params);
		    else
				horizontal_mode_c(params);
	} while( A0 < params->max_pixel);

}


/******************************* initialize_b1 ********************************

		locates b1's first position in the reference line

*****************************************************************************/
void initialize_b1(struct parameters *params)
{
	short	last_bit_of_b1;

	b1 = 1;
	last_bit_of_b1 = b1 & Last_bit_mask;

	while( ((*(params->reference_line +b1) <=A0) || (A0_color ==last_bit_of_b1))
	 && (*(params->reference_line + b1) < params->max_pixel) ){
	 	b1++;
 		last_bit_of_b1 = b1 & Last_bit_mask;
	} /* end while loop */

	return;
}


/********************************** pass_mode_c ********************************

				compresses a pass mode

*****************************************************************************/
void pass_mode_c(struct parameters *params)
{
	write_bits_c("0001");

	/*
	 * Reset the value A0 points to to a'0 (the value that b2 points to).
	 */

	A0 = *(params->reference_line + b2);

	/*
	 * Since A0 is now greater than the pixel b1 points to, both b1 and b2
	 * must be advanced twice to maintain the color difference between A0 and
	 * b1, and the positional requirement that b1 point to a pixel greater than
	 * the one A0 points to.
	 */

	 b1 += 2;
	 b2 += 2;

	 /*
	  * Note that the b's can be advanced by two positions without fear of
	  * moving them beyond the last changing element because pass_mode cannot
	  * occur if b2 is already pointing to max_pixel.
	  */

}


/****************************** vertical_mode_c ********************************

			compresses a vertical mode

*****************************************************************************/
void vertical_mode_c(struct parameters *params)
{
short difference;

	difference = *(params->coding_line + A1) - *(params->reference_line + b1);
	A0 = *(params->coding_line + A1);
	A0_color = !A0_color;
	A1++;

	switch(difference) {

	case 0:
		write_bits_c("1");
		if(*(params->reference_line + b1) != params->max_pixel ) {
		    b1++;
		    b2++;
		} /* end if b1 is not on the last changing element */
		break;

	case 1:
		write_bits_c("011");
		b1++;
		b2++;
		if((*(params->reference_line + b1) <= A0)  &&
		   (*(params->reference_line + b1) != params->max_pixel) ) {
		    b1 += 2;
		    b2 += 2;
		}
		break;

	case -1:
		write_bits_c("010");
		if(*(params->reference_line + b1) != params->max_pixel ) {
		   b1++;
		   b2++;
		} /* end if b1 is not on the last changing element */
		break;

	case 2:
		write_bits_c("000011");
		b1++;
		b2++;
		if((*(params->reference_line + b1) <= A0)  &&
		   (*(params->reference_line + b1) != params->max_pixel) ) {
		    b1 += 2;
		    b2 += 2;
		}
		break;

	case -2:
		write_bits_c("000010");
		if(*(params->reference_line + b1 - 1) > A0 ) {
		   b1--;
		   b2--;
		} else if(*(params->reference_line + b1) != params->max_pixel){
			  b1++;
			  b2++;
			}
		break;

	case 3:
		write_bits_c("0000011");
		b1++;
		b2++;
		while ((*(params->reference_line + b1) <= A0)  &&
		       (*(params->reference_line + b1) != params->max_pixel) ) {
			b1 += 2;
			b2 += 2;
		}
		break;

	case -3:
		write_bits_c("0000010");
		if(*(params->reference_line + b1 - 1) > A0 ) {
		   b1--;
		   b2--;
		} else if(*(params->reference_line + b1) != params->max_pixel){
			  b1++;
			  b2++;
			}
		break;

	default:
//		printf("ERROR in vertical_mode_c() ");
		break;
	} /* end case of difference */

}


/**************************** horizontal_mode_c ********************************

			compresses a horizontal mode

*****************************************************************************/
void horizontal_mode_c(struct parameters *params)
{
short run_length;

	a2 = A1 + 1;
	write_bits_c("001");

	if(A0 == Invalid) /* on imaginary first pixel */
	   run_length = *(params->coding_line + A1);
	else
	   run_length = *(params->coding_line + A1) - A0;
	write_run_length(run_length, A0_color );
	/* the last bit contains the color of the changing element */

	run_length = *(params->coding_line + a2) - *(params->coding_line + A1);
	write_run_length(run_length, !A0_color);

	/*
	 *  Must use !A0_color instead of A1 because in cases in which A1 occurs
	 *  on max_pixel, its color is bogus.
	 */

	/* NOTE: is the above statement true? if A1 were on max_pixel, you should
	not get horizontal mode. */


	A0 = *(params->coding_line + a2);
	A1 = a2 + 1;

	while((*(params->reference_line + b1) <= *(params->coding_line + a2)) &&
	       ( *(params->reference_line + b1) < params->max_pixel)  )
	{
   	   b1 += 2; /* must move ahead by 2 to maintain color difference with */
	   b2 += 2; /* A0, whose color does not change in this mode. */
	}

}


/***************************************************************************/
/* Originally write_bits_c.c                                               */
/***************************************************************************/

static short  bit_place_mark;

static long  byte_place_mark;

static unsigned char  *output_area;

static char const  write_one[Pixels_per_byte] =
{
	(char) 0x80,
	(char) 0x40,
	(char) 0x20,
	(char) 0x10,
	(char) 0x8,
	(char) 0x4,
	(char) 0x2,
	(char) 0x1,
};

static char const  write_zero[Pixels_per_byte] =
{
	(char) 0x7F,
	(char) 0xBF,
	(char) 0xDF,
	(char) 0xEF,
	(char) 0xF7,
	(char) 0xFB,
	(char) 0xFD,
	(char) 0xFE,
};


/*************************** prepare_to_write_bits_c **************************

	initializes variables in preperation for writing compressed images

*****************************************************************************/
void prepare_to_write_bits_c(struct compressed_descriptor *compressed)
{
    if (compressed->data == NULL){
//        printf("\nMemory allocation error for compressed output data.\n");
    	crash_c();
    }
	output_area = compressed->data;
}



/******************************** write_bits_c **********************************

	writes a variable length series of bits represented by a string of '1's
	and '0's, which it receives as a parameter

*****************************************************************************/
void write_bits_c(char const * string_ptr)
{
//	printf("%s  at byte place %ld\n",string_ptr,byte_place_mark);
      /* global switch added by Michael D. Garris 2/26/90 */
      if(comp_write_init_flag){
         bit_place_mark = 0;
         byte_place_mark = 0;
         comp_write_init_flag = 0;		// FALSE
      }
	while(*string_ptr != 0) {
	      if(*string_ptr == '1')
	  	 *(output_area + byte_place_mark) |= write_one[bit_place_mark];
	      else
		 *(output_area + byte_place_mark) &= write_zero[bit_place_mark];
	      if(bit_place_mark == Last_bit_in_a_byte) {
	 	 bit_place_mark = 0;
		 byte_place_mark++;
	      } /* end if byte is full */
	      else
		 bit_place_mark++;
	      string_ptr++;
	} /* end while */

}


/******************************** flush_buffer *******************************

	writes to memory whatever bits are left in the bit buffer followed by
	enough zero-bits to pad the compressed image out to a byte boundary.

*****************************************************************************/
unsigned long flush_buffer()
{
short i;

	if (bit_place_mark != 0) {
       	    for (i=bit_place_mark; i<Pixels_per_byte; i++)
		*(output_area + byte_place_mark) &= write_zero[i];
		/*
		 * pad the rest of the last byte with '0' bits.
		 */
		++byte_place_mark;
	}
	return byte_place_mark;
}


/***************************************************************************/
/* Originally write_run.c                                                  */
/***************************************************************************/

/******************************************************************************

	The arrays that follow contain character representations of the binary
	run length codes written during compression.

******************************************************************************/
static char const * const white_terminating_code[64] =
{
	"00110101",
	"000111",
	"0111",
	"1000",
	"1011",
	"1100",
	"1110",
	"1111",
	"10011",
	"10100",
	"00111",
	"01000",
	"001000",
	"000011",
	"110100",
	"110101",
	"101010",
	"101011",
	"0100111",
	"0001100",
	"0001000",
	"0010111",
	"0000011",
	"0000100",
	"0101000",
	"0101011",
	"0010011",
	"0100100",
	"0011000",
	"00000010",
	"00000011",
	"00011010",
	"00011011",
	"00010010",
	"00010011",
	"00010100",
	"00010101",
	"00010110",
	"00010111",
	"00101000",
	"00101001",
	"00101010",
	"00101011",
	"00101100",
	"00101101",
	"00000100",
	"00000101",
	"00001010",
	"00001011",
	"01010010",
	"01010011",
	"01010100",
	"01010101",
	"00100100",
	"00100101",
	"01011000",
	"01011001",
	"01011010",
	"01011011",
	"01001010",
	"01001011",
	"00110010",
	"00110011",
	"00110100",
};/* end array of white terminating code */


static char const * const black_terminating_code[64] =
{
	"0000110111",
	"010",
	"11",
	"10",
	"011",
	"0011",
	"0010",
	"00011",
	"000101",
	"000100",
	"0000100",
	"0000101",
	"0000111",
	"00000100",
	"00000111",
	"000011000",
	"0000010111",
	"0000011000",
	"0000001000",
	"00001100111",
	"00001101000",
	"00001101100",
	"00000110111",
	"00000101000",
	"00000010111",
	"00000011000",
	"000011001010",
	"000011001011",
	"000011001100",
	"000011001101",
	"000001101000",
	"000001101001",
	"000001101010",
	"000001101011",
	"000011010010",
	"000011010011",
	"000011010100",
	"000011010101",
	"000011010110",
	"000011010111",
	"000001101100",
	"000001101101",
	"000011011010",
	"000011011011",
	"000001010100",
	"000001010101",
	"000001010110",
	"000001010111",
	"000001100100",
	"000001100101",
	"000001010010",
	"000001010011",
	"000000100100",
	"000000110111",
	"000000111000",
	"000000100111",
	"000000101000",
	"000001011000",
	"000001011001",
	"000000101011",
	"000000101100",
	"000001011010",
	"000001100110",
	"000001100111",
}; /* end black_terminating_array */


static char const * const white_make_up_code[40] =
{
	"11011",
	"10010",
	"010111",
	"0110111",
	"00110110",
	"00110111",
	"01100100",
	"01100101",
	"01101000",
	"01100111",
	"011001100",
	"011001101",
	"011010010",
	"011010011",
	"011010100",
	"011010101",
	"011010110",
	"011010111",
	"011011000",
	"011011001",
	"011011010",
	"011011011",
	"010011000",
	"010011001",
	"010011010",
	"011000",
	"010011011",

   /*
    * from this line on, the codes are colorless and represnt runs from
    * 1792 pixels to 2560 pixels.  In other words, the longest run length
    * codes have been added onto both the white make up codes and the black
    * make up codes.  This has been done to make the procedure
    * "write_run_length()" easier to write and to understand.  No other
    * procedure in the compression algorithm is affected by this merging of
    * different types of run length codes, and the compatibility of the
    * program is in no way effected.
    */

	"00000001000",
	"00000001100",
	"00000001101",
	"000000010010",
	"000000010011",
	"000000010100",
	"000000010101",
	"000000010110",
	"000000010111",
	"000000011100",
	"000000011101",
	"000000011110",
	"000000011111",
}; /* end case of white makeup code */


static char const * const black_make_up_code[40] =
{
	"0000001111",
	"000011001000",
	"000011001001",
	"000001011011",
	"000000110011",
	"000000110100",
	"000000110101",
	"0000001101100",
	"0000001101101",
	"0000001001010",
	"0000001001011",
	"0000001001100",
	"0000001001101",
	"0000001110010",
	"0000001110011",
	"0000001110100",
	"0000001110101",
	"0000001110110",
	"0000001110111",
	"0000001010010",
	"0000001010011",
	"0000001010100",
	"0000001010101",
	"0000001011010",
	"0000001011011",
	"0000001100100",
	"0000001100101",

   /*
    * from this line on, the codes are colorless and represnt runs from
    * 1792 pixels to 2560 pixels.  In other words, the longest run length
    * codes have been added onto both the white make up codes and the black
    * make up codes.  This has been done to make the procedure
    * "write_run_length()" easier to write and to understand.  No other
    * procedure in the compression algorithm is affected by this merging of
    * different types of run length codes, and the compatibility of the
    * program is in no way compromised.
    */

	"00000001000",
	"00000001100",
	"00000001101",
	"000000010010",
	"000000010011",
	"000000010100",
	"000000010101",
	"000000010110",
	"000000010111",
	"000000011100",
	"000000011101",
	"000000011110",
	"000000011111",
}; /* end black makeup code */


char const * const largest_colorless_code =
{
	"000000011111"
};



/****************************** write_run_length() *****************************

	writes the code, or series of codes, that represent a given run length
	of a given color.

******************************************************************************/
void write_run_length(short length,short color)
{

short	multiples_of_largest_code,
		make_up_code_index,
		remainder,
		i;

	multiples_of_largest_code = length / Largest_code;
	length %= Largest_code;
	for(i=0 ; i < multiples_of_largest_code ; i++)
	    write_bits_c( largest_colorless_code );

	remainder = length % Size_of_make_up_code_increments;

   /* remainder in the range 0 - 63 */

	make_up_code_index = length / Size_of_make_up_code_increments;

   /*
    * make_up_code_index in the range 0 - 39, and represents a run length
    * of 64 times its value (i.e. 0 - 2496).  To translate this value into
    * an index into the arrays that store the bit sequence that represents
    * the appropriate run length, 1 must be subtracted from make_up_code_
    * index.  If this results in the value -1, no make up code should be
    * written.
    */

	make_up_code_index--;

	if(make_up_code_index != Invalid) {
	   if(color == White)
	      write_bits_c(white_make_up_code[make_up_code_index]);
	   else
	      write_bits_c(black_make_up_code[make_up_code_index]);
	}

	if(color == White)
	   write_bits_c(white_terminating_code[remainder]);
	else
	   write_bits_c(black_terminating_code[remainder]);

} 	/* end write run length() */


/***************************************************************************/
/* Originally table.c                                                      */
/***************************************************************************/

struct byte_descriptor {
    short pixel[9];
};


static struct byte_descriptor const table[Number_of_different_bytes] =
{
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    7,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    6,    7,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    6,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    5,    6,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    5,    6,    7,   -1,   -1,   -1,   -1,   -1,   -1,
    5,    7,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    5,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    4,    5,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    4,    5,    7,   -1,   -1,   -1,   -1,   -1,   -1,
    4,    5,    6,    7,   -1,   -1,   -1,   -1,   -1,
    4,    5,    6,   -1,   -1,   -1,   -1,   -1,   -1,
    4,    6,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    4,    6,    7,   -1,   -1,   -1,   -1,   -1,   -1,
    4,    7,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    4,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    3,    4,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    3,    4,    7,   -1,   -1,   -1,   -1,   -1,   -1,
    3,    4,    6,    7,   -1,   -1,   -1,   -1,   -1,
    3,    4,    6,   -1,   -1,   -1,   -1,   -1,   -1,
    3,    4,    5,    6,   -1,   -1,   -1,   -1,   -1,
    3,    4,    5,    6,    7,   -1,   -1,   -1,   -1,
    3,    4,    5,    7,   -1,   -1,   -1,   -1,   -1,
    3,    4,    5,   -1,   -1,   -1,   -1,   -1,   -1,
    3,    5,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    3,    5,    7,   -1,   -1,   -1,   -1,   -1,   -1,
    3,    5,    6,    7,   -1,   -1,   -1,   -1,   -1,
    3,    5,    6,   -1,   -1,   -1,   -1,   -1,   -1,
    3,    6,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    3,    6,    7,   -1,   -1,   -1,   -1,   -1,   -1,
    3,    7,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    3,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    2,    3,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    2,    3,    7,   -1,   -1,   -1,   -1,   -1,   -1,
    2,    3,    6,    7,   -1,   -1,   -1,   -1,   -1,
    2,    3,    6,   -1,   -1,   -1,   -1,   -1,   -1,
    2,    3,    5,    6,   -1,   -1,   -1,   -1,   -1,
    2,    3,    5,    6,    7,   -1,   -1,   -1,   -1,
    2,    3,    5,    7,   -1,   -1,   -1,   -1,   -1,
    2,    3,    5,   -1,   -1,   -1,   -1,   -1,   -1,
    2,    3,    4,    5,   -1,   -1,   -1,   -1,   -1,
    2,    3,    4,    5,    7,   -1,   -1,   -1,   -1,
    2,    3,    4,    5,    6,    7,   -1,   -1,   -1,
    2,    3,    4,    5,    6,   -1,   -1,   -1,   -1,
    2,    3,    4,    6,   -1,   -1,   -1,   -1,   -1,
    2,    3,    4,    6,    7,   -1,   -1,   -1,   -1,
    2,    3,    4,    7,   -1,   -1,   -1,   -1,   -1,
    2,    3,    4,   -1,   -1,   -1,   -1,   -1,   -1,
    2,    4,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    2,    4,    7,   -1,   -1,   -1,   -1,   -1,   -1,
    2,    4,    6,    7,   -1,   -1,   -1,   -1,   -1,
    2,    4,    6,   -1,   -1,   -1,   -1,   -1,   -1,
    2,    4,    5,    6,   -1,   -1,   -1,   -1,   -1,
    2,    4,    5,    6,    7,   -1,   -1,   -1,   -1,
    2,    4,    5,    7,   -1,   -1,   -1,   -1,   -1,
    2,    4,    5,   -1,   -1,   -1,   -1,   -1,   -1,
    2,    5,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    2,    5,    7,   -1,   -1,   -1,   -1,   -1,   -1,
    2,    5,    6,    7,   -1,   -1,   -1,   -1,   -1,
    2,    5,    6,   -1,   -1,   -1,   -1,   -1,   -1,
    2,    6,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    2,    6,    7,   -1,   -1,   -1,   -1,   -1,   -1,
    2,    7,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    2,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    1,    2,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    1,    2,    7,   -1,   -1,   -1,   -1,   -1,   -1,
    1,    2,    6,    7,   -1,   -1,   -1,   -1,   -1,
    1,    2,    6,   -1,   -1,   -1,   -1,   -1,   -1,
    1,    2,    5,    6,   -1,   -1,   -1,   -1,   -1,
    1,    2,    5,    6,    7,   -1,   -1,   -1,   -1,
    1,    2,    5,    7,   -1,   -1,   -1,   -1,   -1,
    1,    2,    5,   -1,   -1,   -1,   -1,   -1,   -1,
    1,    2,    4,    5,   -1,   -1,   -1,   -1,   -1,
    1,    2,    4,    5,    7,   -1,   -1,   -1,   -1,
    1,    2,    4,    5,    6,    7,   -1,   -1,   -1,
    1,    2,    4,    5,    6,   -1,   -1,   -1,   -1,
    1,    2,    4,    6,   -1,   -1,   -1,   -1,   -1,
    1,    2,    4,    6,    7,   -1,   -1,   -1,   -1,
    1,    2,    4,    7,   -1,   -1,   -1,   -1,   -1,
    1,    2,    4,   -1,   -1,   -1,   -1,   -1,   -1,
    1,    2,    3,    4,   -1,   -1,   -1,   -1,   -1,
    1,    2,    3,    4,    7,   -1,   -1,   -1,   -1,
    1,    2,    3,    4,    6,    7,   -1,   -1,   -1,
    1,    2,    3,    4,    6,   -1,   -1,   -1,   -1,
    1,    2,    3,    4,    5,    6,   -1,   -1,   -1,
    1,    2,    3,    4,    5,    6,    7,   -1,   -1,
    1,    2,    3,    4,    5,    7,   -1,   -1,   -1,
    1,    2,    3,    4,    5,   -1,   -1,   -1,   -1,
    1,    2,    3,    5,   -1,   -1,   -1,   -1,   -1,
    1,    2,    3,    5,    7,   -1,   -1,   -1,   -1,
    1,    2,    3,    5,    6,    7,   -1,   -1,   -1,
    1,    2,    3,    5,    6,   -1,   -1,   -1,   -1,
    1,    2,    3,    6,   -1,   -1,   -1,   -1,   -1,
    1,    2,    3,    6,    7,   -1,   -1,   -1,   -1,
    1,    2,    3,    7,   -1,   -1,   -1,   -1,   -1,
    1,    2,    3,   -1,   -1,   -1,   -1,   -1,   -1,
    1,    3,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    1,    3,    7,   -1,   -1,   -1,   -1,   -1,   -1,
    1,    3,    6,    7,   -1,   -1,   -1,   -1,   -1,
    1,    3,    6,   -1,   -1,   -1,   -1,   -1,   -1,
    1,    3,    5,    6,   -1,   -1,   -1,   -1,   -1,
    1,    3,    5,    6,    7,   -1,   -1,   -1,   -1,
    1,    3,    5,    7,   -1,   -1,   -1,   -1,   -1,
    1,    3,    5,   -1,   -1,   -1,   -1,   -1,   -1,
    1,    3,    4,    5,   -1,   -1,   -1,   -1,   -1,
    1,    3,    4,    5,    7,   -1,   -1,   -1,   -1,
    1,    3,    4,    5,    6,    7,   -1,   -1,   -1,
    1,    3,    4,    5,    6,   -1,   -1,   -1,   -1,
    1,    3,    4,    6,   -1,   -1,   -1,   -1,   -1,
    1,    3,    4,    6,    7,   -1,   -1,   -1,   -1,
    1,    3,    4,    7,   -1,   -1,   -1,   -1,   -1,
    1,    3,    4,   -1,   -1,   -1,   -1,   -1,   -1,
    1,    4,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    1,    4,    7,   -1,   -1,   -1,   -1,   -1,   -1,
    1,    4,    6,    7,   -1,   -1,   -1,   -1,   -1,
    1,    4,    6,   -1,   -1,   -1,   -1,   -1,   -1,
    1,    4,    5,    6,   -1,   -1,   -1,   -1,   -1,
    1,    4,    5,    6,    7,   -1,   -1,   -1,   -1,
    1,    4,    5,    7,   -1,   -1,   -1,   -1,   -1,
    1,    4,    5,   -1,   -1,   -1,   -1,   -1,   -1,
    1,    5,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    1,    5,    7,   -1,   -1,   -1,   -1,   -1,   -1,
    1,    5,    6,    7,   -1,   -1,   -1,   -1,   -1,
    1,    5,    6,   -1,   -1,   -1,   -1,   -1,   -1,
    1,    6,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    1,    6,    7,   -1,   -1,   -1,   -1,   -1,   -1,
    1,    7,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    1,    7,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    1,    6,    7,   -1,   -1,   -1,   -1,   -1,
    0,    1,    6,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    1,    5,    6,   -1,   -1,   -1,   -1,   -1,
    0,    1,    5,    6,    7,   -1,   -1,   -1,   -1,
    0,    1,    5,    7,   -1,   -1,   -1,   -1,   -1,
    0,    1,    5,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    1,    4,    5,   -1,   -1,   -1,   -1,   -1,
    0,    1,    4,    5,    7,   -1,   -1,   -1,   -1,
    0,    1,    4,    5,    6,    7,   -1,   -1,   -1,
    0,    1,    4,    5,    6,   -1,   -1,   -1,   -1,
    0,    1,    4,    6,   -1,   -1,   -1,   -1,   -1,
    0,    1,    4,    6,    7,   -1,   -1,   -1,   -1,
    0,    1,    4,    7,   -1,   -1,   -1,   -1,   -1,
    0,    1,    4,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    1,    3,    4,   -1,   -1,   -1,   -1,   -1,
    0,    1,    3,    4,    7,   -1,   -1,   -1,   -1,
    0,    1,    3,    4,    6,    7,   -1,   -1,   -1,
    0,    1,    3,    4,    6,   -1,   -1,   -1,   -1,
    0,    1,    3,    4,    5,    6,   -1,   -1,   -1,
    0,    1,    3,    4,    5,    6,    7,   -1,   -1,
    0,    1,    3,    4,    5,    7,   -1,   -1,   -1,
    0,    1,    3,    4,    5,   -1,   -1,   -1,   -1,
    0,    1,    3,    5,   -1,   -1,   -1,   -1,   -1,
    0,    1,    3,    5,    7,   -1,   -1,   -1,   -1,
    0,    1,    3,    5,    6,    7,   -1,   -1,   -1,
    0,    1,    3,    5,    6,   -1,   -1,   -1,   -1,
    0,    1,    3,    6,   -1,   -1,   -1,   -1,   -1,
    0,    1,    3,    6,    7,   -1,   -1,   -1,   -1,
    0,    1,    3,    7,   -1,   -1,   -1,   -1,   -1,
    0,    1,    3,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    1,    2,    3,   -1,   -1,   -1,   -1,   -1,
    0,    1,    2,    3,    7,   -1,   -1,   -1,   -1,
    0,    1,    2,    3,    6,    7,   -1,   -1,   -1,
    0,    1,    2,    3,    6,   -1,   -1,   -1,   -1,
    0,    1,    2,    3,    5,    6,   -1,   -1,   -1,
    0,    1,    2,    3,    5,    6,    7,   -1,   -1,
    0,    1,    2,    3,    5,    7,   -1,   -1,   -1,
    0,    1,    2,    3,    5,   -1,   -1,   -1,   -1,
    0,    1,    2,    3,    4,    5,   -1,   -1,   -1,
    0,    1,    2,    3,    4,    5,    7,   -1,   -1,
    0,    1,    2,    3,    4,    5,    6,    7,   -1,
    0,    1,    2,    3,    4,    5,    6,   -1,   -1,
    0,    1,    2,    3,    4,    6,   -1,   -1,   -1,
    0,    1,    2,    3,    4,    6,    7,   -1,   -1,
    0,    1,    2,    3,    4,    7,   -1,   -1,   -1,
    0,    1,    2,    3,    4,   -1,   -1,   -1,   -1,
    0,    1,    2,    4,   -1,   -1,   -1,   -1,   -1,
    0,    1,    2,    4,    7,   -1,   -1,   -1,   -1,
    0,    1,    2,    4,    6,    7,   -1,   -1,   -1,
    0,    1,    2,    4,    6,   -1,   -1,   -1,   -1,
    0,    1,    2,    4,    5,    6,   -1,   -1,   -1,
    0,    1,    2,    4,    5,    6,    7,   -1,   -1,
    0,    1,    2,    4,    5,    7,   -1,   -1,   -1,
    0,    1,    2,    4,    5,   -1,   -1,   -1,   -1,
    0,    1,    2,    5,   -1,   -1,   -1,   -1,   -1,
    0,    1,    2,    5,    7,   -1,   -1,   -1,   -1,
    0,    1,    2,    5,    6,    7,   -1,   -1,   -1,
    0,    1,    2,    5,    6,   -1,   -1,   -1,   -1,
    0,    1,    2,    6,   -1,   -1,   -1,   -1,   -1,
    0,    1,    2,    6,    7,   -1,   -1,   -1,   -1,
    0,    1,    2,    7,   -1,   -1,   -1,   -1,   -1,
    0,    1,    2,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    2,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    2,    7,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    2,    6,    7,   -1,   -1,   -1,   -1,   -1,
    0,    2,    6,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    2,    5,    6,   -1,   -1,   -1,   -1,   -1,
    0,    2,    5,    6,    7,   -1,   -1,   -1,   -1,
    0,    2,    5,    7,   -1,   -1,   -1,   -1,   -1,
    0,    2,    5,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    2,    4,    5,   -1,   -1,   -1,   -1,   -1,
    0,    2,    4,    5,    7,   -1,   -1,   -1,   -1,
    0,    2,    4,    5,    6,    7,   -1,   -1,   -1,
    0,    2,    4,    5,    6,   -1,   -1,   -1,   -1,
    0,    2,    4,    6,   -1,   -1,   -1,   -1,   -1,
    0,    2,    4,    6,    7,   -1,   -1,   -1,   -1,
    0,    2,    4,    7,   -1,   -1,   -1,   -1,   -1,
    0,    2,    4,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    2,    3,    4,   -1,   -1,   -1,   -1,   -1,
    0,    2,    3,    4,    7,   -1,   -1,   -1,   -1,
    0,    2,    3,    4,    6,    7,   -1,   -1,   -1,
    0,    2,    3,    4,    6,   -1,   -1,   -1,   -1,
    0,    2,    3,    4,    5,    6,   -1,   -1,   -1,
    0,    2,    3,    4,    5,    6,    7,   -1,   -1,
    0,    2,    3,    4,    5,    7,   -1,   -1,   -1,
    0,    2,    3,    4,    5,   -1,   -1,   -1,   -1,
    0,    2,    3,    5,   -1,   -1,   -1,   -1,   -1,
    0,    2,    3,    5,    7,   -1,   -1,   -1,   -1,
    0,    2,    3,    5,    6,    7,   -1,   -1,   -1,
    0,    2,    3,    5,    6,   -1,   -1,   -1,   -1,
    0,    2,    3,    6,   -1,   -1,   -1,   -1,   -1,
    0,    2,    3,    6,    7,   -1,   -1,   -1,   -1,
    0,    2,    3,    7,   -1,   -1,   -1,   -1,   -1,
    0,    2,    3,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    3,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    3,    7,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    3,    6,    7,   -1,   -1,   -1,   -1,   -1,
    0,    3,    6,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    3,    5,    6,   -1,   -1,   -1,   -1,   -1,
    0,    3,    5,    6,    7,   -1,   -1,   -1,   -1,
    0,    3,    5,    7,   -1,   -1,   -1,   -1,   -1,
    0,    3,    5,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    3,    4,    5,   -1,   -1,   -1,   -1,   -1,
    0,    3,    4,    5,    7,   -1,   -1,   -1,   -1,
    0,    3,    4,    5,    6,    7,   -1,   -1,   -1,
    0,    3,    4,    5,    6,   -1,   -1,   -1,   -1,
    0,    3,    4,    6,   -1,   -1,   -1,   -1,   -1,
    0,    3,    4,    6,    7,   -1,   -1,   -1,   -1,
    0,    3,    4,    7,   -1,   -1,   -1,   -1,   -1,
    0,    3,    4,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    4,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    4,    7,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    4,    6,    7,   -1,   -1,   -1,   -1,   -1,
    0,    4,    6,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    4,    5,    6,   -1,   -1,   -1,   -1,   -1,
    0,    4,    5,    6,    7,   -1,   -1,   -1,   -1,
    0,    4,    5,    7,   -1,   -1,   -1,   -1,   -1,
    0,    4,    5,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    5,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    5,    7,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    5,    6,    7,   -1,   -1,   -1,   -1,   -1,
    0,    5,    6,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    6,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    6,    7,   -1,   -1,   -1,   -1,   -1,   -1,
    0,    7,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    0,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
}; /* end of data for list of byte descriptors */



/****************************** process_char *********************************

	writes the pixel number of each changing element within the character
	being processed to the list "params->coding_line"

******************************************************************************/
void process_char(unsigned char data_byte,struct parameters *params)
{
static char color = 0;
short i = 0;

	color = -(data_byte & Last_bit_mask);
	data_byte ^= params->previous_color;

/* if the previous color is black - which is contrary to our assumptions -
* the bits in the byte must all be changed so that the result, when used
* as an index into the array 'bytes,' yields the correct result.  In the
* above operation, if the previous color is black (11111111b), all bits
* are changed; if the previous color is white (00000000b), no bits are
* changed. */

	while(table[data_byte].pixel[i] != Invalid)
	      *( params->coding_line + ++params->index ) =
	      params->pixel + table[data_byte].pixel[i++];

	params->pixel += Pixels_per_byte;
	params->previous_color = color;

	/* 'color' is a temporary holding place for the value of previous color */
}






#define	TIFFhowmany(x, y) ((((uint32)(x))+(((uint32)(y))-1))/((uint32)(y)))
#define	TIFFroundup(x, y) (TIFFhowmany(x,y)*((uint32)(y)))
//
//	DecodeTiffImage
//		Decode an 8 or 1 bit per pixel Tiff image into a raw output image.
//
bool DecodeTiffImage(BITSTRING *TiffImage, BITSTRING *OutputImage, int *XDimension, int *YDimension, int *Depth,
					 int *Compression, HSTRING *Message)
{
	bool	Status = true;				// Assume success until we know otherwise.
	int		NextByteToProcess = 0;		// Next byte in image to be processed.
	int		TiffID;
	int		TiffVersion;
	int		TiffHeaderSize;
	int		NumberOfTags;
	int		NumberOfTagsProcessed;
	int		PhotometricInterpretation = Black;
	int		FillOrder;
	int		MessageSize = 0;
	int		MessageOffset = 0;
	int		TiffStripOffset;
	int		RowsPerStrip = -1;
	int		BytesInImage;
	int		XResolutionOffset = -1;
	int		YResolutionOffset = -1;
	int		T6FlagByte = -1;
	int		ResolutionUnit = -1;
	int		CaereFileSize;
	int		XResolutionNumerator;
	int		XResolutionDenominator;
	int		YResolutionNumerator;
	int		YResolutionDenominator;





//	RESET_WATCHDOG_TIMER();

// Start by decoding and validating the header.  We'll only accept a TIFF image generated by us!
	Status = TiffImage->CopyNumberFromString(NextByteToProcess + 0, 2, &TiffID);
	Status = TiffImage->CopyNumberFromString(NextByteToProcess + 2, 2, &TiffVersion);
	Status = TiffImage->CopyNumberFromString(NextByteToProcess + 4, 4, &TiffHeaderSize);
	NextByteToProcess += TIFF_HEADER_SIZE;
	if((TiffID != TIFF_ID) || (TiffVersion != TIFF_VER) || (TiffHeaderSize != TIFF_HEADER_SIZE))
	{
		Status = false;
	}


// Now we'd better find the number of tags.
	if(Status)    Status = TiffImage->CopyNumberFromString(NextByteToProcess, 2, &NumberOfTags);
	NextByteToProcess += 2;



// Let's keep getting tags until we reach the end of the header.
	bool	EndOfHeader = false;
	int		Tag;
	int		TagType;
	int		TagLength;
	*XDimension = -1;
	*YDimension = -1;
	*Depth = -1;
	*Compression = -1;
	NumberOfTagsProcessed = 0;

	while((Status) && (!EndOfHeader))
	{
		Status = TiffImage->CopyNumberFromString(NextByteToProcess, 2, &Tag);
		NextByteToProcess += 2;
		Status = TiffImage->CopyNumberFromString(NextByteToProcess, 2, &TagType);
		NextByteToProcess += 2;

	// If the last four bytes were zeroes, we reached the end of the header.
		if((Tag == 0) && (TagType == 0))
		{
			EndOfHeader = true;
		}

		else
		{
			Status = TiffImage->CopyNumberFromString(NextByteToProcess, 4, &TagLength);
			NextByteToProcess += 4;

			if(Status)
			{
				switch(Tag)
				{
					case IMAGE_WIDTH_TAG:
						if((TagType == TiffDWORD) && (TagLength == 1))
							Status = TiffImage->CopyNumberFromString(NextByteToProcess, 4, XDimension);
						else
							Status = false;
						break;

					case IMAGE_HEIGHT_TAG:
						if((TagType == TiffDWORD) && (TagLength == 1))
							Status = TiffImage->CopyNumberFromString(NextByteToProcess, 4, YDimension);
						else
							Status = false;
						break;

					case IMAGE_BITS_PER_PIXEL_TAG:
						if((TagType == TiffWORD) && (TagLength == 1))
							Status = TiffImage->CopyNumberFromString(NextByteToProcess, 4, Depth);
						else
							Status = false;
						break;

					case IMAGE_COMPRESSION_TAG:
						if((TagType == TiffWORD) && (TagLength == 1))
							Status = TiffImage->CopyNumberFromString(NextByteToProcess, 4, Compression);
						else
							Status = false;
						break;

					case PHOTOMETRIC_INTERP_TAG:
						if((TagType == TiffWORD) && (TagLength == 1))
							Status = TiffImage->CopyNumberFromString(NextByteToProcess, 4, &PhotometricInterpretation);
						else
							Status = false;
						break;

					case FILL_ORDER_TAG:
						if((TagType == TiffWORD) && (TagLength == 1))
							Status = TiffImage->CopyNumberFromString(NextByteToProcess, 4, &FillOrder);
						else
							Status = false;
						break;

					case IMAGE_DESCRIPTION_TAG:
						MessageSize = TagLength;
						if(TagType == TiffASCII)
							if(TagLength <= 4)
								Message->Copy(&TiffImage->Char[NextByteToProcess], TagLength);
							else
								Status = TiffImage->CopyNumberFromString(NextByteToProcess, 4, &MessageOffset);
						else
							Status = false;
						break;

					case IMAGE_DATA_OFFSET_TAG:
						if((TagType == TiffDWORD) && (TagLength == 1))
							Status = TiffImage->CopyNumberFromString(NextByteToProcess, 4, &TiffStripOffset);
						else
							Status = false;
						break;

					case ROWS_PER_STRIP_TAG:
						if((TagType == TiffDWORD) && (TagLength == 1))
							Status = TiffImage->CopyNumberFromString(NextByteToProcess, 4, &RowsPerStrip);
						else
							Status = false;
						break;

					case BYTES_PER_STRIP_TAG:
						if((TagType == TiffDWORD) && (TagLength == 1))
							Status = TiffImage->CopyNumberFromString(NextByteToProcess, 4, &BytesInImage);
						else
							Status = false;
						break;

					case X_RESOLUTION_TAG:
						if((TagType == TiffRational) && (TagLength == 1))
							Status = TiffImage->CopyNumberFromString(NextByteToProcess, 4, &XResolutionOffset);
						else
							Status = false;
						break;

					case Y_RESOLUTION_TAG:
						if((TagType == TiffRational) && (TagLength == 1))
							Status = TiffImage->CopyNumberFromString(NextByteToProcess, 4, &YResolutionOffset);
						else
							Status = false;
						break;

					case T6_FLAG_BYTE_TAG:
						if((TagType == TiffDWORD) && (TagLength == 1))
							Status = TiffImage->CopyNumberFromString(NextByteToProcess, 4, &T6FlagByte);
						else
							Status = false;
						break;

					case RESOLUTION_UNIT_TAG:
						if((TagType == TiffWORD) && (TagLength == 1))
							Status = TiffImage->CopyNumberFromString(NextByteToProcess, 4, &ResolutionUnit);
						else
							Status = false;
						break;

					case CAERE_PROPRIETARY_TAG:
						if((TagType == TiffDWORD) && (TagLength == 1))
							Status = TiffImage->CopyNumberFromString(NextByteToProcess, 4, &CaereFileSize);
						else
							Status = false;
						break;

					default:
						break;
				}

				++NumberOfTagsProcessed;
				NextByteToProcess += 4;
			}
		}
	}



// We'd better have found the correct number of tags and we'd better have found dimensional and compression data.
	if((NumberOfTags != NumberOfTagsProcessed) ||
					(*XDimension <= 0) ||
					(*YDimension <= 0) ||
					((RowsPerStrip > 0) && (RowsPerStrip != *YDimension)) ||
					((*Depth != 1) && (*Depth != 8)) ||
					((*Compression != 1) && (*Compression != 4)) ||
					((PhotometricInterpretation != White) && (PhotometricInterpretation != Black)) ||
					(FillOrder != 1) ||
					((T6FlagByte >= 0) && (T6FlagByte != T6_FLAG_BYTE)) ||
					((ResolutionUnit >= 0) && (ResolutionUnit != RESOLUTION_UNIT)))
	{
		Status = false;
	}


// If we have any X-resolution information, process it now.
	if((Status) && (XResolutionOffset >= 0))
	{
		Status = TiffImage->CopyNumberFromString(XResolutionOffset, 4, &XResolutionNumerator);
		Status = TiffImage->CopyNumberFromString(XResolutionOffset + 4, 4, &XResolutionDenominator);

		if((XResolutionNumerator != X_RESOLUTION_NUMERATOR) || (XResolutionDenominator != X_RESOLUTION_DENOMINATOR))
			Status = false;
	}


// If we have any Y-resolution information, process it now.
	if((Status) && (YResolutionOffset >= 0))
	{
		Status = TiffImage->CopyNumberFromString(YResolutionOffset, 4, &YResolutionNumerator);
		Status = TiffImage->CopyNumberFromString(YResolutionOffset + 4, 4, &YResolutionDenominator);

		if((YResolutionNumerator != Y_RESOLUTION_NUMERATOR) || (YResolutionDenominator != Y_RESOLUTION_DENOMINATOR))
			Status = false;
	}


// If we have message data that was too large to fit in the tag, process it now.
	if((Status) && (MessageSize > 4))
	{
		if((MessageOffset + MessageSize) <= (int) TiffImage->Size)
			Message->Copy(&TiffImage->Char[MessageOffset], MessageSize);
		else
			Status = false;
	}



// If we made it this far, we're ready to process the image!
	if(Status)
	{
		if(*Depth == 8)
		{
		// 8 bit gray scale image.  Copy directly to the output image.
			Status = TiffImage->CopyFromString(TiffStripOffset, ((*XDimension) * (*YDimension)), OutputImage);
		}

		else
		{
		// 1 bit image.
			if(*Compression == 4)
			{
#ifdef TIFF_DECOMPRESSION
			// Decompress binary first.
				CSB				csb;
				int32			bytes;
				unsigned char	pixelValue[2];
				unsigned char	*bits;

				csb.bs.groupoptions = TIFF_Group3Options::G3_2DENCODING;

			// Store the vertical resolution in the codec state block.

				csb.bs.yresolution = (float) YResolutionOffset;
				csb.bs.resolutionunit = ResolutionUnit;

			// Image dimensions

				csb.bs.rowpixels = *XDimension;
				csb.bs.rows      = *YDimension;

				csb.bs.rowbytes = TIFFroundup(csb.bs.rowpixels, 8) / 8;

				uint32 nruns = 2 * TIFFroundup(csb.bs.rowpixels, 8);

				csb.ds.runs = new uint16[nruns];
//				if (csb.ds.runs == NULL)
//				{
//				}
				csb.ds.curruns = csb.ds.runs;
				csb.ds.refruns = csb.ds.runs + (nruns >> 1);

				switch (PhotometricInterpretation)
				{
					case TIFF_Photometric::MINISBLACK:
						pixelValue[0] = 0x00;
						pixelValue[1] = 0xFF;
						break;

					case TIFF_Photometric::MINISWHITE:
					default:
						pixelValue[0] = 0xFF;
						pixelValue[1] = 0x00;
						break;
				}

			// Set up the codec output pointer

				bytes = csb.bs.rows * csb.bs.rowbytes;
				bits = new unsigned char [bytes];
//				if (bits == NULL)
//				return *this;		// +++ throw exception

			// Set up the codec input array and length.
				csb.bs.rawdata = csb.bs.rawcp = &TiffImage->Char[ TiffStripOffset ];
				csb.bs.rawcc = BytesInImage;

			// Now decode the image into the bitmap
				Fax3PreDecode(&csb, TIFF_FillOrder::MSB2LSB);
				Fax4Decode(&csb, bits, bytes);

				TiffImage->Remove( TiffStripOffset, TiffImage->Size - TiffStripOffset );
				TiffImage->Append( bits, bytes );
#endif
			}

		// Build grayscale image from uncompressed binary
			if(Status)
			{
				int		X;
				int		Y;
				int		WhiteValue = (PhotometricInterpretation == White) ? 0 : 255;
				int		BlackValue = (PhotometricInterpretation == White) ? 255 : 0;

				TiffImage->ResetBitPointer();
				TiffImage->BytePointer = TiffStripOffset;
				OutputImage->Zero();
				OutputImage->Reallocate( (*XDimension) * (*YDimension) );

				for (Y = 0;    Y < *YDimension;    Y++)
				{
					for (X = 0;    X < *XDimension;    X++)
					{
						OutputImage->AppendChar((TiffImage->CopyNextBits(1) > 0) ? WhiteValue : BlackValue);
					}// end for X

					TiffImage->MoveToNextByteBoundary();
				}// end for Y
			}
		}
	}


//	RESET_WATCHDOG_TIMER();

	return Status;
} // DecodeTiffImage



