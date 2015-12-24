//======================================================================================
// HTagImage.h
//======================================================================================
// $RCSfile: htagimage.h $
// $Revision: 1.4 $
// $Date: 2011/11/11 17:01:41EST $
//======================================================================================

//======================================================================================



//---------------------------------------------------------------------------
#ifndef HTAG_IMAGE_H
#define HTAG_IMAGE_H
//---------------------------------------------------------------------------


#include	"BitString.h"
#include	"HTag.h"




//---------------------------------------------------------------------------
//
//  Class definitions for HTAG_IMAGE
//
//---------------------------------------------------------------------------

//
//	The HTAG_IMAGE class provides a mechanism to transfer images.  Since all fields are tagged, if the
//	receiver does not yet speak the new version of HTAG_IMAGE, it will ignore tags that it doesn't understand.
//



class HTAG_IMAGE : public HTAG
{
public:
// Status codes
	typedef enum {
		HTAG_IMAGE_FILE_NAME = HTAG::HTAG_USER_DEFINED_STATUS_BEGIN,
		HTAG_IMAGE_INVALID_BOUNDARY_COORDINATES,
		HTAG_IMAGE_INVALID_DEPTH,
		HTAG_IMAGE_INVALID_MARGIN,
		HTAG_IMAGE_INVALID_PIXEL_INCREMENT,
		HTAG_IMAGE_PARSE_BOUNDARY_COORDINATES_AND_MARGIN_OUT_OF_RANGE,
		HTAG_IMAGE_CORRUPT_IMAGE
	} HTAG_IMAGE_STATUS;


// Maximum size of image.
	#define	HTAG_IMAGE_MINIMUM_IMAGE_BOUNDARY				0
   #define  HTAG_IMAGE_MAXIMUM_IMAGE_BOUNDARY            10000

// Pixel increment range.
	#define	HTAG_IMAGE_MINIMUM_PIXEL_INCREMENT				1
	#define	HTAG_IMAGE_MAXIMUM_PIXEL_INCREMENT				10


// JPEG quality factor.
	#define	HTAG_IMAGE_MINIMUM_JPEG_QUALITY_FACTOR			0
	#define	HTAG_IMAGE_MAXIMUM_JPEG_QUALITY_FACTOR			100
	#define	HTAG_IMAGE_DEFAULT_JPEG_QUALITY_FACTOR			80


// Image formats
	#define	HTAG_IMAGE_BINARY_IMAGE_DEPTH					1
	#define	HTAG_IMAGE_GRAYSCALE_IMAGE_DEPTH				8

	typedef enum {
		HTAG_IMAGE_KIM = 0,
		HTAG_IMAGE_TIF_BINARY,
		HTAG_IMAGE_TIF_GRP,
		HTAG_IMAGE_TIF_GRAYSCALE,
		HTAG_IMAGE_UNFORMATTED_BINARY,
		HTAG_IMAGE_UNFORMATTED_GRAYSCALE,
		HTAG_IMAGE_JPEG,
		HTAG_IMAGE_OUTLINED_IMAGE,
		HTAG_IMAGE_BMP,
		HTAG_IMAGE_HUFFMAN,
		HTAG_IMAGE_TIF_COLOR_COMPRESSED,  //   10
		HTAG_IMAGE_TIF_COLOR_UNCOMPRESSED,  // 11
		HTAG_IMAGE_JPEG_COLOR,  //    12
		HTAG_IMAGE_UNCOMPRESSED_COLOR,  //13
		HTAG_IMAGE_BMP_COLOR,  //14
		HTAG_IMAGE_UNCOMPRESSED_RAW,			//15	
		END_HTAG_IMAGE_FORMATS
	} HTAG_IMAGE_FORMATS;




// Parameters with names.
	#define	HTAG_IMAGE_FILE_PARAMETER_NAME						"FILE"
	HSTRING		*ImageName;

	// Boundary coordinates in a captured frame where this image was acquired.
	#define	HTAG_IMAGE_LEFT_BOUNDARY_PARAMETER_NAME				"L"
	int		LeftBoundary;
	#define	HTAG_IMAGE_RIGHT_BOUNDARY_PARAMETER_NAME			"R"
	int		RightBoundary;
	#define	HTAG_IMAGE_BOTTOM_BOUNDARY_PARAMETER_NAME			"B"
	int		BottomBoundary;
	#define	HTAG_IMAGE_TOP_BOUNDARY_PARAMETER_NAME				"T"
	int		TopBoundary;

	#define	HTAG_IMAGE_MARGIN_PARAMETER_NAME					"M"
	int		Margin;							// Pixel reduction from edge of image

	#define	HTAG_IMAGE_DEPTH_PARAMETER_NAME						"D"
	int		Depth;							// Pixel depth in bits.

	#define	HTAG_IMAGE_PIXEL_INCREMENT_PARAMETER_NAME			"S"
	int		PixelIncrement;

	#define	HTAG_IMAGE_FLATTENING_PARAMETER_NAME				"C"
	int		ImageFlatteningApplied;
	#define	HTAG_IMAGE_EDGE_ENHANCEMENT_PARAMETER_NAME			"E"
	int		EdgeEnhancementApplied;
	#define	HTAG_IMAGE_HISTOGRAM_STRETCH_PARAMETER_NAME			"H"
	int		HistogramStretchApplied;
	#define	HTAG_IMAGE_X_AND_Y_AXIS_INVERSION_PARAMETER_NAME	"I"
	int		CompleteInversionApplied;
	#define	HTAG_IMAGE_X_AXIS_INVERSION_PARAMETER_NAME			"IX"
	int		XAxisInversionApplied;
	#define	HTAG_IMAGE_Y_AXIS_INVERSION_PARAMETER_NAME			"IY"
	int		YAxisInversionApplied;
	#define	HTAG_IMAGE_SHIP_ROTATION_PARAMETER_NAME				"IR"
	int		RotationApplied;
	#define	HTAG_IMAGE_HISTOGRAM_EQUALIZATION_PARAMETER_NAME	"Q"
	int		HistogramEqualizationApplied;
	#define	HTAG_IMAGE_SMOOTHING_PARAMETER_NAME					"V"
	int		SmoothingApplied;
	#define	HTAG_IMAGE_BINARIZATION_THRESHOLDS_PARAMETER_NAME	"Z"
	int		BinarizationThresholdsApplied;

	#define	HTAG_IMAGE_FORMAT_PARAMETER_NAME					"F"
	int		Format;				            // Image format / compression definitions

	#define	HTAG_IMAGE_JPEG_QUALITY_FACTOR_PARAMETER_NAME		"J"
	int		JPEGQualityFactor;

	#define	HTAG_IMAGE_GAIN_PARAMETER_NAME						"G"
	int		GainCapturedAt;					// Gain value used to capture this image
	#define	HTAG_IMAGE_EXPOSURE_PARAMETER_NAME					"X"
	int		ExposureCapturedAt;			// Exposure time used to capture this image
	#define	HTAG_IMAGE_OBSERVED_WHITE_VALUE_PARAMETER_NAME		"O"
	int		ObservedWhiteValue;
	#define	HTAG_IMAGE_FRAME_RATE_PARAMETER_NAME				"Y"
	int		FrameRateCaptureAt;
   #define  HTAG_IMAGE_ULTRATEXT_PARAMETER_NAME             "U"
	int		UltraTextApplied;
   #define  HTAG_IMAGE_ADAPTIVEULTRATEXT_PARAMETER_NAME             "IU"
   int      AdaptiveUltraTextApplied;
   #define  HTAG_IMAGE_INFINITY_FILTER_PARAMETER_NAME             "A"
	int		InfinityFilterApplied;
	#define	HTAG_IMAGE_GAMMA_PARAMETER_NAME					"K"
	int		GammaCorrectionApplied;
	#define	HTAG_IMAGE_SHIP_HISTOGRAM_PARAMETER_NAME					"W"
	int		HistogramApplied;
	#define	HTAG_IMAGE_SHIP_FLYSPECK_PARAMETER_NAME					"IF"
	int		FlySpeckApplied;
   #define  HTAG_IMAGE_SHIP_SNAZZY_PARAMETER_NAME                 "IS"
   int      SnazzyApplied;
   #define  HTAG_IMAGE_SHIP_DESPECKLE_PARAMETER_NAME              "ID"
	int		DeSpeckleApplied;
	#define	HTAG_IMAGE_SHIP_GLITCHFIX_PARAMETER_NAME					"IG"
	int		GlitchFixApplied;
	#define	HTAG_IMAGE_SHIP_ROWNOISE_PARAMETER_NAME					"~"
	int		RowNoiseApplied;
	#define   HTAG_IMAGE_SHIP_COLOR_PATTERN_PARAMETER_NAME                       "IP"
	int        StartColorPattern;
	#define   HTAG_IMAGE_COLOR_X_OFFSET_PARAMETER_NAME                     "OX"
	int        OffsetWidth;
	#define   HTAG_IMAGE_COLOR_Y_OFFSET_PARAMETER_NAME                     "OY"
	int        OffsetHeight;


public:
	int		XDimension;
	int		YDimension;


public:


// Pointer to the actual image.
	#define	DEFAULT_IMAGE_DATA_ALLOCATION				200
	BITSTRING	*Data;




// Constructor for HTAG_IMAGE; overloaded version #1 for loading images later.
    HTAG_IMAGE(const char *Command = "040807", HCOMM *HCommDevice = 0, HTRANSFER_STATUS *TransferStatus = 0,
								HCOMMAND_HANDLER *CommandHandler = 0);

// Destructor for HTAG_IMAGE
    ~HTAG_IMAGE(void);

// Validate new parameters to see if they're in bounds.
	virtual void PerformAdditionalParameterValidation(int *Status);

// Process each parameter as it's parsed.
	virtual bool PostProcessParameterAfterParsing(const char *NameTag, int *Status);

// After parameters have been parsed by the base class, set XDimension and YDimension.
	virtual void PerformAdditionalOperationsAfterParsing(int *Status);

// Pre-create a tagged image if the protocol is such that HTAG can't do it for us.
    virtual bool PreCreateTaggedImage( HSTRING *TaggedImage );

// Load the local image with a tagged image contained in the string sent by the caller.
	virtual bool PreLoadTaggedImage(HSTRING *TaggedImage, int *Status);


private:
	HTAG_IMAGE (const HTAG_IMAGE&);            // declared private, but not defined, to prevent copying         
	HTAG_IMAGE& operator=(const HTAG_IMAGE&);  // declared private, but not defined, to prevent copying 

};

//----------------------------------------------------------------------------




#endif // HTAG_IMAGE_H



