//======================================================================================
// HImageProprietary.cpp
//======================================================================================
// $RCSfile: himageProprietary.cpp $
// $Revision: 1.7 $
// $Date: 2011/06/28 21:01:22EDT $
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
#include	"HImage.h"			// HIMAGE interface definition
#include	"BmpImage.h"		// BmpImage interface definition
#include	"KimImage.h"		// KimImage interface definition
#include	"TiffImage.h"		// TiffImage interface definition

#include	"Compression.h"		// Compression routine interface
#include "db_chrio.h"
#include "ImageCore.h"
#include "scandriverdebug.h"
//---------------------------------------------------------------------------


// Flatten The Image
void HIMAGE::FlattenImage(void)    //ToRGB
{
	ImageDesc Src, Dst;
	unsigned char *ImageCopy = NULL;

	// Backup the input image data
	ImageCopy = (unsigned char*)malloc(Data->Size);
	if(!ImageCopy)
		return;
	FastMemoryCopy(ImageCopy, Data->Char, Data->Size);

	Src.ImageBlueX = 0;			// We are GRAY, ignore blue pixel		
	Src.ImageBlueY = 0;
	Src.ImageX = XDimension;
	Src.ImageY = YDimension;
	Src.Color = GRAY;				// We always use GRAY since we will first do fission before filters

	// TODO: Need consider depth 24 (Color input)
	Src.ImageDepth = 8;
	Src.ImageData = Data->Char;

	//Note!!!	Set ImageData to NULL to let ImageCore to malloc buffer
	Dst.ImageData = NULL;

	if(CompensateImage(&Src, &Dst) == 0){
		Data->Reallocate(Dst.ImageX*Dst.ImageY);
		FastMemoryCopy(Data->Char, Dst.ImageData, Dst.ImageX*Dst.ImageY);
		XDimension = Dst.ImageX;
		YDimension = Dst.ImageY;
		Data->Size = Dst.ImageX * Dst.ImageY;
		LeftBoundary = 0;
		RightBoundary = XDimension - 1;
		TopBoundary = 0;
		BottomBoundary = YDimension - 1;
	}
	else
	{	// Recover the input data if fail
		FastMemoryCopy(Data->Char, ImageCopy, Data->Size);
	}

	if(ImageCopy)
		free(ImageCopy);

	if(Dst.ImageData)
		free(Dst.ImageData);
	
}



void HIMAGE::StretchImage(void)         //ToRGB
{
	ImageDesc Src, Dst;
	unsigned char *ImageCopy = NULL;

	// Backup the input image data
	ImageCopy = (unsigned char*)malloc(Data->Size);
	if(!ImageCopy)
		return;
	FastMemoryCopy(ImageCopy, Data->Char, Data->Size);

	
	Src.ImageBlueX = 0;			// We are GRAY, ignore blue pixel		
	Src.ImageBlueY = 0;
	Src.ImageX = XDimension;
	Src.ImageY = YDimension;
	Src.Color = GRAY;				// We always use GRAY since we will first do fission before filters

	// TODO: Need to consider depth 24 (Color input)
	Src.ImageDepth = 8;
	Src.ImageData = Data->Char;

	//Note!!!	Set ImageData to NULL to let ImageCore to malloc buffer
	Dst.ImageData = NULL;

	if(HistogramStretch(&Src, &Dst, 4, 96) == 0){
		Data->Reallocate(Dst.ImageX*Dst.ImageY);
		FastMemoryCopy(Data->Char, Dst.ImageData, Dst.ImageX*Dst.ImageY);
		XDimension = Dst.ImageX;
		YDimension = Dst.ImageY;
		Data->Size = Dst.ImageX * Dst.ImageY;
		LeftBoundary = 0;
		RightBoundary = XDimension - 1;
		TopBoundary = 0;
		BottomBoundary = YDimension - 1;
	}
	else
	{	// Recover the input data if fail
		FastMemoryCopy(Data->Char, ImageCopy, Data->Size);
	}

	if(ImageCopy)
		free(ImageCopy);

	if(Dst.ImageData)
		free(Dst.ImageData);
}


void HIMAGE::GammaImage(int GammaValue)
{
	ImageDesc Src, Dst;
	unsigned char *ImageCopy = NULL;
	double Gamma = (double)(GammaValue/100);

	// Backup the input image data
	ImageCopy = (unsigned char*)malloc(Data->Size);
	if(!ImageCopy)
		return;
	FastMemoryCopy(ImageCopy, Data->Char, Data->Size);

	
	Src.ImageBlueX = 0;			// We are GRAY, ignore blue pixel		
	Src.ImageBlueY = 0;
	Src.ImageX = XDimension;
	Src.ImageY = YDimension;
	Src.Color = GRAY;				// We always use GRAY since we will first do fission before filters

	// TODO: Need to consider depth 24 (Color input)
	Src.ImageDepth = 8;
	Src.ImageData = Data->Char;

	//Note!!!	Set ImageData to NULL to let ImageCore to malloc buffer
	Dst.ImageData = NULL;

	if(GammaCorrection(&Src, &Dst, Gamma) == 0){
		Data->Reallocate(Dst.ImageX*Dst.ImageY);
		FastMemoryCopy(Data->Char, Dst.ImageData, Dst.ImageX*Dst.ImageY);
		XDimension = Dst.ImageX;
		YDimension = Dst.ImageY;
		Data->Size = Dst.ImageX * Dst.ImageY;
		LeftBoundary = 0;
		RightBoundary = XDimension - 1;
		TopBoundary = 0;
		BottomBoundary = YDimension - 1;
	}
	else
	{	// Recover the input data if fail
		FastMemoryCopy(Data->Char, ImageCopy, Data->Size);
	}

	if(ImageCopy)
		free(ImageCopy);

	if(Dst.ImageData)
		free(Dst.ImageData);
	
}


void HIMAGE::HistogramImage(void)
{
	ImageDesc Src, Dst;
	unsigned char *ImageCopy = NULL;

	// Backup the input image data
	ImageCopy = (unsigned char*)malloc(Data->Size);
	if(!ImageCopy)
		return;
	FastMemoryCopy(ImageCopy, Data->Char, Data->Size);

	
	Src.ImageBlueX = 0;			// We are GRAY, ignore blue pixel		
	Src.ImageBlueY = 0;
	Src.ImageX = XDimension;
	Src.ImageY = YDimension;
	Src.Color = GRAY;				// We always use GRAY since we will first do fission before filters

	// TODO: Need to consider depth 24 (Color input)
	Src.ImageDepth = 8;
	Src.ImageData = Data->Char;

	//Note!!!	Set ImageData to NULL to let ImageCore to malloc buffer
	Dst.ImageData = NULL;

	if(ShowHistogram(&Src, &Dst) == 0){
		Data->Reallocate(Dst.ImageX*Dst.ImageY);
		FastMemoryCopy(Data->Char, Dst.ImageData, Dst.ImageX*Dst.ImageY);
		XDimension = Dst.ImageX;
		YDimension = Dst.ImageY;
		Data->Size = Dst.ImageX * Dst.ImageY;
		LeftBoundary = 0;
		RightBoundary = XDimension - 1;
		TopBoundary = 0;
		BottomBoundary = YDimension - 1;
	}
	else
	{	// Recover the input data if fail
		FastMemoryCopy(Data->Char, ImageCopy, Data->Size);
	}

	if(ImageCopy)
		free(ImageCopy);

	if(Dst.ImageData)
		free(Dst.ImageData);
}


// Rotate the image  1=rotate right, 2=rotate 180, 3=rotate left
// Note this function may be removed after the ImageCore updating to support RGB rotation.
void HIMAGE::ImageRotateRGB(int Rotate)
{
	unsigned char *src,*dst,tempchar, tempchar1, tempchar2;
	int temp,x,y,SrcIncrement;
	BITSTRING NewImage(3*YDimension*XDimension, true);

	switch (Rotate) {
		case 1: case 3:
			FastMemoryCopy(NewImage.Char,Data->Char,3*XDimension*YDimension);
			dst = Data->Char;

			if (Rotate == 1) SrcIncrement = -3*XDimension;
			else SrcIncrement = 3*XDimension;

			for (x=0; x<XDimension; x++) {
				if (Rotate == 1) src = NewImage.Char + 3*(x + XDimension*(YDimension-1));
				else src = NewImage.Char + 3*(XDimension-x-1);
				for (y=0; y<YDimension; y++) {
					*dst = *src;
					*(dst+1) = *(src+1);
					*(dst+2) = *(src+2);
					src += SrcIncrement;
					dst += 3;
				}
			}

			temp = XDimension;
			XDimension = YDimension;
			YDimension = temp;
			BottomBoundary = YDimension-1;
			RightBoundary = XDimension-1;
			break;
		case 2:
			dst = Data->Char;
			src = Data->Char + 3*(XDimension * YDimension - 1);

			for (x=0; x<(XDimension*YDimension)>>1; x++) 
			{
				tempchar = *dst;
				tempchar1 = *(dst+1);
				tempchar2 = *(dst+2);
				*dst++ = *src;
				*src = tempchar;
				*dst++ = *(src+1);
				*(src+1) = tempchar1;
				*dst++ = *(src+2);
				*(src+2) = tempchar2;
				
				src -= 3; 
			}

			break;
	}
}


// Rotate the image  1=rotate right, 2=rotate 180, 3=rotate left
void HIMAGE::ImageRotate(int Rotate)
{
	ImageDesc Src, Dst;
	int Degrees = 0;
	unsigned char *ImageCopy = NULL;

	// Backup the input image data
	ImageCopy = (unsigned char*)malloc(Data->Size);
	if(!ImageCopy)
		return;
	FastMemoryCopy(ImageCopy, Data->Char, Data->Size);

	switch(Rotate)
	{
		case 0:
			if(ImageCopy)
				free(ImageCopy);
			return;

		case 1:
			Degrees = 90;
			break;

		case 2:
			Degrees = 180;
			break;

		case 3:
			Degrees = 270;
			break;
		default:
			if(ImageCopy)
				free(ImageCopy);
			return;
	}
	
	Src.ImageBlueX = 0;			// We are GRAY, ignore blue pixel		
	Src.ImageBlueY = 0;
	Src.ImageX = XDimension;
	Src.ImageY = YDimension;
	Src.Color = GRAY;				// We always use GRAY since we will first do fission before filters

	// TODO: Need to consider depth 24 (Color input)
	Src.ImageDepth = 8;
	Src.ImageData = Data->Char;

	//Note!!!	Set ImageData to NULL to let ImageCore to malloc buffer
	Dst.ImageData = NULL;

	if(RotateImage(&Src, &Dst, Degrees) == 0){
		Data->Reallocate(Dst.ImageX*Dst.ImageY);
		FastMemoryCopy(Data->Char, Dst.ImageData, Dst.ImageX*Dst.ImageY);
		XDimension = Dst.ImageX;
		YDimension = Dst.ImageY;
		Data->Size = Dst.ImageX * Dst.ImageY;
		LeftBoundary = 0;
		RightBoundary = XDimension - 1;
		TopBoundary = 0;
		BottomBoundary = YDimension - 1;
	}
	else
	{	// Recover the input data if fail
		FastMemoryCopy(Data->Char, ImageCopy, Data->Size);
	}

	if(ImageCopy)
		free(ImageCopy);

	if(Dst.ImageData)
		free(Dst.ImageData);
}


// UltraText filter using Adaptive convolution
void HIMAGE::AdaptiveUltraTextImage(void)
{
	ImageDesc Src, Dst;
	unsigned char *ImageCopy = NULL;

	// Backup the input image data
	ImageCopy = (unsigned char*)malloc(Data->Size);
	if(!ImageCopy)
		return;
	FastMemoryCopy(ImageCopy, Data->Char, Data->Size);

	
	Src.ImageBlueX = 0;			// We are GRAY, ignore blue pixel		
	Src.ImageBlueY = 0;
	Src.ImageX = XDimension;
	Src.ImageY = YDimension;
	Src.Color = GRAY;				// We always use GRAY since we will first do fission before filters

	// TODO: Need to consider depth 24 (Color input)
	Src.ImageDepth = 8;
	Src.ImageData = Data->Char;

	//Note!!!	Set ImageData to NULL to let ImageCore to malloc buffer
	Dst.ImageData = NULL;

	if(AdaptiveUltraText(&Src, &Dst) == 0){
		Data->Reallocate(Dst.ImageX*Dst.ImageY);
		FastMemoryCopy(Data->Char, Dst.ImageData, Dst.ImageX*Dst.ImageY);
		XDimension = Dst.ImageX;
		YDimension = Dst.ImageY;
		Data->Size = Dst.ImageX * Dst.ImageY;
		LeftBoundary = 0;
		RightBoundary = XDimension - 1;
		TopBoundary = 0;
		BottomBoundary = YDimension - 1;
	}
	else
	{	// Recover the input data if fail
		FastMemoryCopy(Data->Char, ImageCopy, Data->Size);
	}

	if(ImageCopy)
		free(ImageCopy);

	if(Dst.ImageData)
		free(Dst.ImageData);
}


void HIMAGE::Smoothing()
{
	ImageDesc Src, Dst;
	unsigned char *ImageCopy = NULL;

	// Backup the input image data
	ImageCopy = (unsigned char*)malloc(Data->Size);
	if(!ImageCopy)
		return;
	FastMemoryCopy(ImageCopy, Data->Char, Data->Size);

	
	Src.ImageBlueX = 0;			// We are GRAY, ignore blue pixel		
	Src.ImageBlueY = 0;
	Src.ImageX = XDimension;
	Src.ImageY = YDimension;
	Src.Color = GRAY;				// We always use GRAY since we will first do fission before filters

	// TODO: Need to consider depth 24 (Color input)
	Src.ImageDepth = 8;
	Src.ImageData = Data->Char;

	//Note!!!	Set ImageData to NULL to let ImageCore to malloc buffer
	Dst.ImageData = NULL;

	if(Blur(&Src, &Dst) == 0){
		Data->Reallocate(Dst.ImageX*Dst.ImageY);
		FastMemoryCopy(Data->Char, Dst.ImageData, Dst.ImageX*Dst.ImageY);
		XDimension = Dst.ImageX;
		YDimension = Dst.ImageY;
		Data->Size = Dst.ImageX * Dst.ImageY;
		LeftBoundary = 0;
		RightBoundary = XDimension - 1;
		TopBoundary = 0;
		BottomBoundary = YDimension - 1;
	}
	else
	{	// Recover the input data if fail
		FastMemoryCopy(Data->Char, ImageCopy, Data->Size);
	}

	if(ImageCopy)
		free(ImageCopy);

	if(Dst.ImageData)
		free(Dst.ImageData);
}

#if 1
void HIMAGE::UltraTextImage()
{
	ImageDesc Src, Dst;
	unsigned char *ImageCopy = NULL;

	// Backup the input image data
	ImageCopy = (unsigned char*)malloc(Data->Size);
	if(!ImageCopy)
		return;
	FastMemoryCopy(ImageCopy, Data->Char, Data->Size);

	
	Src.ImageBlueX = 0;			// We are GRAY, ignore blue pixel		
	Src.ImageBlueY = 0;
	Src.ImageX = XDimension;
	Src.ImageY = YDimension;
	Src.Color = GRAY;				// We always use GRAY since we will first do fission before filters

	// TODO: Need to consider depth 24 (Color input)
	Src.ImageDepth = 8;
	Src.ImageData = Data->Char;

	//Note!!!	Set ImageData to NULL to let ImageCore to malloc buffer
	Dst.ImageData = NULL;

	if(DocumentFilter(&Src, &Dst, UltraTextApplied) == 0){
		Data->Reallocate(Dst.ImageX*Dst.ImageY);
		FastMemoryCopy(Data->Char, Dst.ImageData, Dst.ImageX*Dst.ImageY);
		XDimension = Dst.ImageX;
		YDimension = Dst.ImageY;
		Data->Size = Dst.ImageX * Dst.ImageY;
		LeftBoundary = 0;
		RightBoundary = XDimension - 1;
		TopBoundary = 0;
		BottomBoundary = YDimension - 1;
	}
	else
	{	// Recover the input data if fail
		FastMemoryCopy(Data->Char, ImageCopy, Data->Size);
	}

	if(ImageCopy)
		free(ImageCopy);

	if(Dst.ImageData)
		free(Dst.ImageData);
}
#endif

// Sharpen the edges using convolution

void HIMAGE::EdgeSharpenImage(int Strength)  //ToRGB
{
	ImageDesc Src, Dst;
	unsigned char *ImageCopy = NULL;

	// Backup the input image data
	ImageCopy = (unsigned char*)malloc(Data->Size);
	if(!ImageCopy)
		return;
	FastMemoryCopy(ImageCopy, Data->Char, Data->Size);

	Src.ImageBlueX = 0;			// We are GRAY, ignore blue pixel		
	Src.ImageBlueY = 0;
	Src.ImageX = XDimension;
	Src.ImageY = YDimension;
	Src.Color = GRAY;				// We always use GRAY since we will first do fission before filters

	// TODO: Need to consider depth 24 (Color input)
	Src.ImageDepth = 8;
	Src.ImageData = Data->Char;

	//Note!!!	Set ImageData to NULL to let ImageCore to malloc buffer
	Dst.ImageData = NULL;

	if(SharpenEdges(&Src, &Dst, Strength) == 0){
		Data->Reallocate(Dst.ImageX*Dst.ImageY);
		FastMemoryCopy(Data->Char, Dst.ImageData, Dst.ImageX*Dst.ImageY);
		XDimension = Dst.ImageX;
		YDimension = Dst.ImageY;
		Data->Size = Dst.ImageX * Dst.ImageY;
		LeftBoundary = 0;
		RightBoundary = XDimension - 1;
		TopBoundary = 0;
		BottomBoundary = YDimension - 1;
	}
	else
	{	// Recover the input data if fail
		FastMemoryCopy(Data->Char, ImageCopy, Data->Size);
	}

	if(ImageCopy)
		free(ImageCopy);

	if(Dst.ImageData)
		free(Dst.ImageData);

}



// focus to infinity using convolution
void HIMAGE::InfinityFilterImage(void)
{
	ImageDesc Src, Dst;
	unsigned char *ImageCopy = NULL;

	// Backup the input image data
	ImageCopy = (unsigned char*)malloc(Data->Size);
	if(!ImageCopy)
		return;

	FastMemoryCopy(ImageCopy, Data->Char, Data->Size);
	
	Src.ImageBlueX = 0;			// We are GRAY, ignore blue pixel		
	Src.ImageBlueY = 0;
	Src.ImageX = XDimension;
	Src.ImageY = YDimension;
	Src.Color = GRAY;				// We always use GRAY since we will first do fission before filters

	// TODO: Need consider depth 24 (Color input)
	Src.ImageDepth = 8;
	
	Src.ImageData = Data->Char;

	//Note!!!	Set ImageData to NULL to let ImageCore to malloc buffer
	Dst.ImageData = NULL;

	if(InfinityFilter(&Src, &Dst) == 0){
		Data->Reallocate(Dst.ImageX*Dst.ImageY);
		FastMemoryCopy(Data->Char, Dst.ImageData, Dst.ImageX*Dst.ImageY);
		XDimension = Dst.ImageX;
		YDimension = Dst.ImageY;
		Data->Size = Dst.ImageX * Dst.ImageY;
		LeftBoundary = 0;
		RightBoundary = XDimension - 1;
		TopBoundary = 0;
		BottomBoundary = YDimension - 1;
	}
	else
	{	// Recover the input data if fail
		FastMemoryCopy(Data->Char, ImageCopy, Data->Size);
	}

	if(ImageCopy)
		free(ImageCopy);

	if(Dst.ImageData)
		free(Dst.ImageData);

}


void HIMAGE::ImageCrop(int Top, int Bot, int Lft, int Rgt)
{
	ImageDesc Src, Dst;
	unsigned char *ImageCopy = NULL;

	// Backup the input image data
	ImageCopy = (unsigned char*)malloc(Data->Size);
	if(!ImageCopy)
		return;
	FastMemoryCopy(ImageCopy, Data->Char, Data->Size);

	
	Src.ImageBlueX = 0;			// We are GRAY, ignore blue pixel		
	Src.ImageBlueY = 0;
	Src.ImageX = XDimension;
	Src.ImageY = YDimension;
	Src.Color = GRAY;				// We always use GRAY since we will first do fission before filters

	// TODO: Need to consider depth 24 (Color input)
	Src.ImageDepth = 8;
	Src.ImageData = Data->Char;

	//Note!!!	Set ImageData to NULL to let ImageCore to malloc buffer
	Dst.ImageData = NULL;

	if(CropImage(&Src, &Dst, Top, Bot, Lft, Rgt) == 0){
		Data->Reallocate(Dst.ImageX*Dst.ImageY);
		FastMemoryCopy(Data->Char, Dst.ImageData, Dst.ImageX*Dst.ImageY);
		XDimension = Dst.ImageX;
		YDimension = Dst.ImageY;
		Data->Size = Dst.ImageX * Dst.ImageY;
		LeftBoundary = 0;
		RightBoundary = XDimension - 1;
		TopBoundary = 0;
		BottomBoundary = YDimension - 1;
	}
	else
	{	// Recover the input data if fail
		FastMemoryCopy(Data->Char, ImageCopy, Data->Size);
	}

	if(ImageCopy)
		free(ImageCopy);
	
	if(Dst.ImageData)
		free(Dst.ImageData);
}


void HIMAGE::Snazzy(void)
{
	ImageDesc Src, Dst;
	unsigned char *ImageCopy = NULL;

	// Backup the input image data
	ImageCopy = (unsigned char*)malloc(Data->Size);
	if(!ImageCopy)
		return;
	FastMemoryCopy(ImageCopy, Data->Char, Data->Size);

	
	Src.ImageBlueX = 0;			// We are GRAY, ignore blue pixel		
	Src.ImageBlueY = 0;
	Src.ImageX = XDimension;
	Src.ImageY = YDimension;
	Src.Color = GRAY;				// We always use GRAY since we will first do fission before filters

	// TODO: Need to consider depth 24 (Color input)
	Src.ImageDepth = 8;
	Src.ImageData = Data->Char;

	//Note!!!	Set ImageData to NULL to let ImageCore to malloc buffer
	Dst.ImageData = NULL;

	if(UnsharpAndBrighten(&Src, &Dst) == 0){
		Data->Reallocate(Dst.ImageX*Dst.ImageY);
		FastMemoryCopy(Data->Char, Dst.ImageData, Dst.ImageX*Dst.ImageY);
		XDimension = Dst.ImageX;
		YDimension = Dst.ImageY;
		Data->Size = Dst.ImageX * Dst.ImageY;
		LeftBoundary = 0;
		RightBoundary = XDimension - 1;
		TopBoundary = 0;
		BottomBoundary = YDimension - 1;
	}
	else
	{	// Recover the input data if fail
		FastMemoryCopy(Data->Char, ImageCopy, Data->Size);
	}

	if(ImageCopy)
		free(ImageCopy);

	if(Dst.ImageData)
		free(Dst.ImageData);
}

void HIMAGE::SubsampleImage(int Bits)
{
	int NewImageWidth = XDimension/PixelIncrement;
	int NewImageHeight = YDimension/PixelIncrement;	
	unsigned char *tempptr = Data ->Char;
	unsigned char *datasrc = Data ->Char;
	unsigned char * NewRow = Data ->Char;

	for(int j = 0; j< NewImageHeight; j++)
	{
		if(Bits == 24)
			NewRow = NewRow+ XDimension *3 * PixelIncrement;
		else if(Bits == 8)
			NewRow = NewRow+ XDimension * PixelIncrement;

		for( int i = 0; i< NewImageWidth; i++)
		{
			if(Bits == 24)
			{
				*tempptr++ = *datasrc;
				*tempptr ++ = *(datasrc +1);
				*tempptr ++ = *(datasrc + 2);
				datasrc = datasrc + PixelIncrement * 3;
			}
			else if(Bits == 8)
			{
				*tempptr++ = *datasrc;
				datasrc = datasrc + PixelIncrement;
			}
		}
		datasrc = NewRow ;
	}
	XDimension = NewImageWidth;
	YDimension = NewImageHeight;
	if(Bits == 24)
		Data->Size = NewImageWidth * NewImageHeight *3;
	else if(Bits == 8)
		Data->Size = NewImageWidth * NewImageHeight;
		
}


void HIMAGE::FlySpeckImage(void)
{
	ImageDesc Src, Dst;
	unsigned char *ImageCopy = NULL;

	// Backup the input image data
	ImageCopy = (unsigned char*)malloc(Data->Size);
	if(!ImageCopy)
		return;
	FastMemoryCopy(ImageCopy, Data->Char, Data->Size);

	
	Src.ImageBlueX = 0;			// We are GRAY, ignore blue pixel		
	Src.ImageBlueY = 0;
	Src.ImageX = XDimension;
	Src.ImageY = YDimension;
	Src.Color = GRAY;				// We always use GRAY since we will first do fission before filters

	// TODO: Need to consider depth 24 (Color input)
	Src.ImageDepth = 8;
	Src.ImageData = Data->Char;

	//Note!!!	Set ImageData to NULL to let ImageCore to malloc buffer
	Dst.ImageData = NULL;

	if(FlySpeck(&Src, &Dst) == 0){
		Data->Reallocate(Dst.ImageX*Dst.ImageY);
		FastMemoryCopy(Data->Char, Dst.ImageData, Dst.ImageX*Dst.ImageY);
		XDimension = Dst.ImageX;
		YDimension = Dst.ImageY;
		Data->Size = Dst.ImageX * Dst.ImageY;
		LeftBoundary = 0;
		RightBoundary = XDimension - 1;
		TopBoundary = 0;
		BottomBoundary = YDimension - 1;
	}
	else
	{	// Recover the input data if fail
		FastMemoryCopy(Data->Char, ImageCopy, Data->Size);
	}

	if(ImageCopy)
		free(ImageCopy);
	
	if(Dst.ImageData)
		free(Dst.ImageData);
}


void HIMAGE::CalImageBlueXY(int * ImageX, int * ImageY)
{
	if(!StartColorPattern)
	{
		*ImageX = OffsetWidth;   
		*ImageY = OffsetHeight;  
	}
	else if(StartColorPattern == 1)
	{
		*ImageX = OffsetWidth + 4; 
		*ImageY = OffsetHeight;      
	}
	else if(StartColorPattern == 2)
	{
		*ImageX = OffsetWidth + 4;   
		*ImageY = OffsetHeight +4;  
	}
	else if(StartColorPattern == 3)
	{
		*ImageX = OffsetWidth;   
		*ImageY = OffsetHeight + 4;  
	}
}


void FixRowNoise(unsigned char *img, int xsize, int ysize, int rot, int TopRatio, int BottomRatio)
{
//   int start = BaselineClock;
   unsigned char Table[256], *pTable;
   unsigned int LastAdj = 0;
   unsigned char *ptr1, *ptr = img;
   unsigned int adj, temp;
   TopRatio = 256 + TopRatio;
   BottomRatio = 256 + BottomRatio;

   for (int y=0; y<ysize; y+=2) {
      if(rot==0) adj = (unsigned int)(TopRatio + ((BottomRatio - TopRatio)*y+ysize/2)/ysize);
      else adj = (unsigned int)(TopRatio + ((BottomRatio - TopRatio)*(ysize-y-1)+ysize/2)/ysize);
      if (adj != 256) { // if adj == 256 don't bother processing the line as no change will be done.
         if (adj > 256) {
            ptr1 = ptr + xsize * (rot ? 0 : 1);
         } else { // better to brighten rather than darken
            adj = (256 * 256 + 128) / adj;
            ptr1 = ptr + xsize * (rot ? 1 : 0);
         }
         if (adj != LastAdj) {
            LastAdj = adj;
            pTable = Table;
            for (int i=0; i<256; i++) {
               temp = (i * adj) >> 8;
               if (temp>255) temp = 255;
               *pTable++ = temp;
            }
         }
         for (int x=xsize; x!=0; x--) {
            *ptr1 = Table[*ptr1];
            ptr1++;
         }
      }
      ptr += xsize * 2;
   }
//printf("Row noise time= %dms\r\n",BaselineClock-start);
}


void HIMAGE::RowNoise(int TopRatio, int BottomRatio)
{
   FixRowNoise(Data->Char, XDimension, YDimension, 0, TopRatio, BottomRatio);
}

void HIMAGE::FissionImage(void)
{
	ImageDesc Src, Dst;
	int xShift = 0, yShift = 0;
	unsigned char *ImageCopy = NULL;

	// Backup the input image data
	ImageCopy = (unsigned char*)malloc(Data->Size);
	if(!ImageCopy)
		return;
	FastMemoryCopy(ImageCopy, Data->Char, Data->Size);

	// Calculate the first blue pixel
	CalImageBlueXY(&xShift, &yShift);

	// Get the start row and column, this may be removed after scandriver extended to support this
	if (GET_SETTING(ImageROIEnabled)){
		unsigned short StartColumn, StartRow;
		StartColumn = GET_SETTING(ImageROILeft);
		StartRow = GET_SETTING(ImageROITop);

		// Calc the new xShift and yShift
		if (xShift >= StartColumn)
			xShift = xShift - StartColumn;
		else{
			xShift = StartColumn - xShift ;
			xShift = 8 - (xShift & 7) ;
		}
		if(xShift == 8) xShift = 0;

		if (yShift >= StartRow)
			yShift = yShift - StartRow;
		else{
			yShift = StartRow - yShift ;
			yShift = 8 - (yShift & 7) ;
		}
		if(yShift == 8) yShift = 0;

	}

	Src.ImageX = XDimension;
	Src.ImageY = YDimension;
	Src.ImageBlueX = xShift;  			
	Src.ImageBlueY = yShift; 
	Src.Color = MONOCOLOR;
	Src.ImageDepth = 8;
	Src.ImageData = Data->Char;

	//Note!!!	Set ImageData to NULL to let ImageCore to malloc buffer
	Dst.ImageData = NULL;

	if(ToGray(&Src, &Dst) == 0)
	{
		Data->Reallocate(Dst.ImageX*Dst.ImageY);
		FastMemoryCopy(Data->Char, Dst.ImageData, Dst.ImageX*Dst.ImageY);
		XDimension = Dst.ImageX;
		YDimension = Dst.ImageY;
		Data->Size = Dst.ImageX * Dst.ImageY;
		LeftBoundary = 0;
		RightBoundary = XDimension - 1;
		TopBoundary = 0;
		BottomBoundary = YDimension - 1;
	}
	else
	{	// Recover the input data if fail
		FastMemoryCopy(Data->Char, ImageCopy, Data->Size);
	}

	if(ImageCopy)
		free(ImageCopy);

	if(Dst.ImageData)
		free(Dst.ImageData);

}

void HIMAGE::RGB2Gray(void)
{
	ImageDesc Src, Dst;
	int xShift = 0, yShift = 0;
	unsigned char *ImageCopy = NULL;

	// Backup the input image data
	ImageCopy = (unsigned char*)malloc(Data->Size);
	if(!ImageCopy)
		return;
	FastMemoryCopy(ImageCopy, Data->Char, Data->Size);
	
	// Calculate the first blue pixel
	CalImageBlueXY(&xShift, &yShift);

	// Note input should be 24bit RGB stored as 3 blocks of Channel R, G, B.
	Src.ImageX = XDimension;
	Src.ImageY = YDimension;
	Src.Color = RGB;
	Src.ImageDepth = 24;
	Src.ImageData = Data->Char;

	//Note!!!	Set ImageData to NULL to let ImageCore to malloc buffer
	Dst.ImageData = NULL;

	if(ToGray(&Src, &Dst) == 0)
	{
		Data->Reallocate(Dst.ImageX*Dst.ImageY);
		FastMemoryCopy(Data->Char, Dst.ImageData, Dst.ImageX*Dst.ImageY);
		XDimension = Dst.ImageX;
		YDimension = Dst.ImageY;
		Data->Size = Dst.ImageX * Dst.ImageY;
		LeftBoundary = 0;
		RightBoundary = XDimension - 1;
		TopBoundary = 0;
		BottomBoundary = YDimension - 1;
	}
	else
	{	// Recover the input data if fail
		FastMemoryCopy(Data->Char, ImageCopy, Data->Size);
	}

	if(ImageCopy)
		free(ImageCopy);

	if(Dst.ImageData)
		free(Dst.ImageData);

}
void HIMAGE::FusionImage(void)
{

	ImageDesc Src, Dst;
	int xShift = 0, yShift = 0;
	unsigned char *ImageCopy = NULL;

	// Backup the input image data
	ImageCopy = (unsigned char*)malloc(Data->Size);
	if(!ImageCopy)
		return;
	FastMemoryCopy(ImageCopy, Data->Char, Data->Size);

	//RESET_WATCHDOG_TIMER();
	
	// Calculate the first blue pixel
	CalImageBlueXY(&xShift, &yShift);

	Src.ImageX = XDimension;
	Src.ImageY = YDimension;
	Src.ImageBlueX = xShift;  			
	Src.ImageBlueY = yShift; 
	Src.Color = MONOCOLOR;
	Src.ImageDepth = 8;
	Src.ImageData = Data->Char;

	//Note!!!	Set ImageData to NULL to let ImageCore to malloc buffer
	Dst.ImageData = NULL;

	if(ToRGB(&Src, &Dst) == 0)
	{
		unsigned char* src, *dst;
		int i;
		
		XDimension = (Dst.ImageX);
		YDimension = Dst.ImageY;

		Data->Reallocate(XDimension*YDimension*3);
		src = (unsigned char*)Dst.ImageData;
		dst = Data->Char;

		// Convert to RGB
		for(i = 0; i < XDimension*YDimension; i++)
		{
			dst[3*i] = src[i];
			dst[3*i+1] = src[i+XDimension*YDimension];
			dst[3*i+2] = src[i+XDimension*YDimension*2];
		}

		Data->Size = XDimension*YDimension*3;
		LeftBoundary = 0;
		RightBoundary = XDimension - 1;
		TopBoundary = 0;
		BottomBoundary = YDimension - 1;
	}
	else
	{	// Recover the input data if fail
		FastMemoryCopy(Data->Char, ImageCopy, Data->Size);
	}

	if(ImageCopy)
		free(ImageCopy);

	if(Dst.ImageData)
		free(Dst.ImageData);
   
}

//#endif




