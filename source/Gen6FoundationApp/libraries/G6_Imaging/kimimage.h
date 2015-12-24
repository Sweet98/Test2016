//======================================================================================
// KimImage.h
//======================================================================================
// $RCSfile: kimimage.h $
// $Revision: 1.1 $
// $Date: 2009/04/03 15:08:45EDT $
//======================================================================================

//======================================================================================






//---------------------------------------------------------------------------
//
//  Definitions for KimImage
//
//---------------------------------------------------------------------------
#ifndef KIM_IMAGE_H
#define KIM_IMAGE_H
//---------------------------------------------------------------------------




#include "BitString.h"			// BITSTRING interface definition


#define		KIM_IMAGE_HEADER_SIZE		36
#define		KIM_IMAGE_MAXIMUM_COLUMNS	2048
#define		KIM_IMAGE_MAXIMUM_ROWS		2048

// Create an 8 or 1 bit per pixel KIM image from the input image.
bool CreateKIMImage(BITSTRING *KimImage, BITSTRING *InputImage, int XDimension, int YDimension, int NumberOfBits = 8,
					int Threshold = 128);

// Create an 8 or 1 bit per pixel KIM or raw image from the input image, with a header on the beginning of it.
bool CreateImageWithHeader(BITSTRING *OutputImage, BITSTRING *InputImage, bool KimImage, int XDimension, int YDimension, int NumberOfBits,
			int Threshold, int LeftBoundary, int TopBoundary, int RightBoundary, int BottomBoundary, int PixelIncrement);

// Decode an 8 or 1 bit per pixel KIM image into a raw output image.
bool DecodeKIMImage(BITSTRING *KimImage, BITSTRING *OutputImage, int XDimension, int YDimension, int NumberOfBits = 8, int OffsetIntoInputImage = 0);

// Decode an 8 or 1 bit per pixel KIM or raw image with header into a raw output image.  If no header is present,
// treat it as a KIM image (the dimensions and number of bits must have been set by the caller).
bool DecodeImageWithHeader(BITSTRING *OutputImage, BITSTRING *InputImage, bool *KimImage, int *XDimension, int *YDimension, int *NumberOfBits,
			int *LeftBoundary, int *TopBoundary, int *RightBoundary, int *BottomBoundary, int *PixelIncrement);

// Create an 8 bit per pixel KIM file from the input image.
bool CreateKIMFile(BITSTRING *KimFile, BITSTRING *InputImage, int XDimension, int YDimension);

//----------------------------------------------------------------------------
#endif // KIM_IMAGE_H



