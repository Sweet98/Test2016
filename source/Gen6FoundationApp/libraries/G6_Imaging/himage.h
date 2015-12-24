//======================================================================================
// HImage.h
//======================================================================================
// $RCSfile: himage.h $
// $Revision: 1.4 $
// $Date: 2011/06/17 04:37:02EDT $
//======================================================================================

//======================================================================================



//---------------------------------------------------------------------------
//
//  Class definitions for HIMAGE
//
//---------------------------------------------------------------------------

#ifndef HIMAGE_H
#define HIMAGE_H

//---------------------------------------------------------------------------


//
//	The HIMAGE class provides a mechanism to manipulate and compress / decompress images.  This class also
//	provides the ability to format images for transfer by tagging them in such a way as to minimize version
//	change problems.  Since all fields are tagged, if the receiver does not yet speak the new version of HIMAGE,
//	it will ignore tags that it doesn't understand.
//



#include	"BitString.h"			// BITSTRING interface definition
#include	"HTagImage.h"			// HTAG_IMAGE interface definition



// Unformat an HTAG_IMAGE  (convert it to a raw image) according to the format setting.
bool UnformatHTagImage(HTAG_IMAGE *HTagImage, bool FastJPEG = true, bool JPEGDecompressionBlockSmoothing = true);



class HIMAGE : public HTAG_IMAGE
{
public:
// Default JPEG smoothing.
	#define	HIMAGE_DEFAULT_JPEG_SMOOTHING_FACTOR		0
	#define	HIMAGE_DEFAULT_JPEG_BLOCK_SMOOTHING			true

	int		JPEGCompressionSmoothingFactor;
	bool	JPEGDecompressionBlockSmoothing;
	bool	FastJPEG;						// Use fast JPEG algorithm


	#define	DEFAULT_THRESHOLD		128
	int		Threshold;



public:
	bool	SmoothOnSubsampling;


protected:
// The following functions are optional in order to support binarization.  It is the responsibility of the
// instantiator to override these, which requires that they use this class as a base class.
	virtual bool Binarize(BITSTRING *InputImage, BITSTRING *OutputImage, int /* XDimension */, int /*YDimension*/,	bool /*UseDecoderThresholds*/)
	{
		int		i;
		bool	Status = true;		// Assume success until we know otherwise.

		OutputImage->Copy("");
		for(i = 0;    i < ((int) InputImage->Size);    ++i)
		{
			Status = OutputImage->AppendNumber((InputImage->Char[i] >= Threshold) ? 255 : 0, 1);
		}
		return(Status);
	};


	virtual bool OutlinedBinarize(BITSTRING *InputImage, BITSTRING *OutputImage, int XDimension, int YDimension, bool UseDecoderThresholds)
	{
		return(Binarize(InputImage, OutputImage, XDimension, YDimension, UseDecoderThresholds));
	};


public:


// Set image parameters to defaults values.
	void SetImageParameterDefaults(void);

// Constructor for HIMAGE; overloaded version #1 for loading images later.
    HIMAGE(void);

// Constructor for HIMAGE; overloaded version #2 for loading an image at instantiation.
    HIMAGE(unsigned char *ImageData, int InputXDimension, int InputYDimension);

// Constructor HIMAGE; overloaded version #3 for loading an image ROI internal original image at instantiation.
	HIMAGE(unsigned char *ImageData, int InputXDimension, int InputYDimension, int x1, int x2, int y1, int y2);

// Constructor for HIMAGE; overloaded version #4 for loading a tagged image at instantiation.
    HIMAGE(HSTRING *ImageData);

// Destructor for HIMAGE
    ~HIMAGE(void);


// Edge Sharpen Image
	void EdgeSharpenImage(int EdgeSharpenValue);

// Format the resident image according to the format setting.
    bool FormatImage(int ImageFormat);


// Unformat the resident image (convert it to a raw image) according to the format setting.
    bool UnformatImage(void);

// Smoothing
	void Smoothing();

	void SubsampleImage(int Bits);

// Rotate the image
	void ImageRotate(int rotate);
	void ImageRotateRGB(int Rotate);

// Remove stuck/dead/hot/cold pixels
	void FlySpeckImage(void);

// Crop image
	void ImageCrop(int Top, int Bot, int Lft, int Rgt);

// Unsharpen and lighten
   void Snazzy(void);


// Flatten the image
	void FlattenImage();

// Stretch the image
	void StretchImage(void);

// unSharp using convolution
	void UltraTextImage();

// Adaptive UltraText using convolution
   void AdaptiveUltraTextImage(void);

// InfinityFilter using convolution
	void InfinityFilterImage(void);

// Gamma Correct the image
	void GammaImage(int GammaValue);

// Ship Histogram of the image
	void HistogramImage(void);

// Remove Row Noise
	void RowNoise(int TopRatio, int BottomRatio);

// Convert Monocolor image to Monochrome
	void FissionImage(void);

	void RGB2Gray(void);

// Convert Monocolor imge to full color.
	void FusionImage(void);

	void CalImageBlueXY(int * ImageX, int * ImageY);  //ToRGB
protected:


};
//----------------------------------------------------------------------------


// Swap two data arrays.
void SwapDataArrays(void *Data1, void *Data2, int Size);


// Subsample, window, and / or invert image either over the same image or to a new image.
// Considering all the possible combinations of changes based on the input parameters,
// we'll try to do the modifications as fast as we can for each given case.  Note that this
// routine assumes that input and output image are either the same or don't overlap; we woof
// if they do overlap.
bool ModifyImage(unsigned char *InputImage, unsigned char *OutputImage, int *XDimension, int *YDimension, int *StartColorPattern, int *xOffset, int * yOffset, bool InvertedImager,
			bool InvertAboutXAxis, bool InvertAboutYAxis, int PixelSubsampleInterval = -1, int NewLeft = -1, int NewRight = -1,
			int NewTop = -1, int NewBottom = -1, int Margin = -1, bool ApplySmoothing = false, bool Use5x5Smoothing = true);


// Subsample, window, and / or invert image over the same image by calling ModifyImage version #1 with
// the same input and output image.
bool ModifyImage(unsigned char *Image, int *XDimension, int *YDimension, int *StartColorPattern, int *xOffset, int * yOffset, bool InvertAboutXAxis, bool InvertedImager,
			bool InvertAboutYAxis, int PixelSubsampleInterval = -1, int NewLeft = -1, int NewRight = -1, int NewTop = -1,
			int NewBottom = -1, int Margin = -1, bool ApplySmoothing = false, bool Use5x5Smoothing = true);


// Subsample, window, and / or invert HSTRING image either over the same image or to a new image.
// Considering all the possible combinations of changes based on the input parameters,
// we'll try to do the modifications as fast as we can for each given case.  Note that this
// routine assumes that input and output image are either the same or don't overlap; we woof
// if they do overlap.
bool ModifyImage(HSTRING *InputImage, HSTRING *OutputImage, int *XDimension, int *YDimension, int *StartColorPattern, int *xOffset, int * yOffset, bool InvertedImager,
			bool InvertAboutXAxis, bool InvertAboutYAxis, int PixelSubsampleInterval = -1, int NewLeft = -1, int NewRight = -1,
			int NewTop = -1, int NewBottom = -1, int Margin = -1, bool ApplySmoothing = false, bool Use5x5Smoothing = true);


// Subsample, window, and / or invert HSTRING image over the same image by calling ModifyImage version #1 with
// the same input and output image.
bool ModifyImage(HSTRING *Image, int *XDimension, int *YDimension, int *StartColorPattern, int *xOffset, int * yOffset, bool InvertAboutXAxis, bool InvertedImager,
			bool InvertAboutYAxis, int PixelSubsampleInterval = -1, int NewLeft = -1, int NewRight = -1, int NewTop = -1,
			int NewBottom = -1, int Margin = -1, bool ApplySmoothing = false, bool Use5x5Smoothing = true);
bool CalculateOffsiteAndPattern(int * StartColorPattern, int * xOffset, int *yOffset,  bool InvertAboutXAxis, bool InvertAboutYAxis, int NewLeft, int NewRight, int NewTop, int NewBottom, bool Windowed, bool Subsampled);
#endif // HIMAGE_H



