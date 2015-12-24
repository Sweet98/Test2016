#ifndef IMAGECORE_H_
#define IMAGECORE_H_
// =============================================================================
// $Source: ImageCore.h $
// $Revision: 1.7 $
// $Date: 2011/10/03 17:24:22EDT $
//

/*
 * ImageCore is a collection of image enhancement routines using a common interface. This is an attempt to standardize our
 * processing across all our platforms. This can be linked in directly, compiled into a object file or a library, or built
 * into a DLL.
 *
 * Each function typically takes an input image descriptor, an output image descriptor and (optionally) function paramenters.
 * The input image descriptor describes the original (source) image.
 * The output image descriptor describes the resultant (destination) image. If the ImageData parameter of the output image
 * descriptor points at the same memory as the input image descriptor's ImageData parameter then the funtion will overwrite
 * the original image with the new image. If the output image Descriptor's Image data is non-zero but doesn't point at the
 * same memory as the input image, then the new image will be written here. If the input image descriptor's ImageData
 * parameter is zero (NULL pointer) The ImageCore will malloc the needed memory and return the pointer to the caller. The
 * caller *must* free the memory later or else there will be a memory leak.
*/

#ifdef WIN32
	#include <stdlib.h>
	#include <string.h> // for memcpy below. can be removed if a different method is used.
	#include <windows.h>
	#define DllExport   __declspec( dllexport ) __stdcall
	#define WOOF
#else
	#define DllExport
#endif
//#include "MonoColorAPI.h"
#ifndef MALLOC
	#define MALLOC(t) malloc(t)
#endif
#ifndef FREE
	#define FREE(t) free(t)
#endif
#ifndef WOOF
	#define WOOF
#endif
#define BLOCK_COPY(D,S,B) memcpy(D,S,B);
#define MAXLABELS 32768

//#define RESET_WATCHDOG_TIMER()
#if 0
// watchdog specifics
#define WDOG_WSR_WSR_RELOAD1       0x5555
#define WDOG_WSR_WSR_RELOAD2       0xAAAA
#define WDOG_BASE_ADDR             0x53FDC000U
#define WDOG_WCR                   (WDOG_BASE_ADDR+0x00U)      // 16bit Watchdog Control Register (WCR)
#define WDOG_WSR                   (WDOG_BASE_ADDR+0x02U)      // 16bit Watchdog Service Register (WSR)
#define WDOG_WRSR                  (WDOG_BASE_ADDR+0x04U)      // 16bit Watchdog Reset Status Register (WRSR)
#define WDOG_WICR                  (WDOG_BASE_ADDR+0x06U)      // 16bit Watchdog Interrupt Control Register (WICR)
#define WDOG_WMCR                  (WDOG_BASE_ADDR+0x08U)      // 16bit Watchdog Miscellaneous Control Register (WMCR)
#define HAZARD_WRITE_16BIT_REG(address,value) (*((volatile unsigned short *)(address)) = (value))

#if (PLATFORM_CURRENT == imx27) | (PLATFORM_CURRENT == imx25)
	#define 	RESET_WATCHDOG_TIMER() do {HAZARD_WRITE_16BIT_REG(WDOG_WSR,WDOG_WSR_WSR_RELOAD1); HAZARD_WRITE_16BIT_REG(WDOG_WSR,WDOG_WSR_WSR_RELOAD2);} while (0)			//Feed the Dog
#elif PLATFORM_CURRENT	== mxl
	#define 	RESET_WATCHDOG_TIMER() do {WATCHDOG.Wsr.f = 0x5555; WATCHDOG.Wsr.f = 0xAAAA;} while (0)
#if (defined(ETM)) || (defined(PRODUCT_CONFIGURATION_APPLICATION))
	#define 	ENABLE_WATCHDOG() WATCHDOG.Wcr.b.WDE=FALSE						//Enable watchdog
#else //ETM - PRODUCT_CONFIGURATION_APPLICATION
	#define 	ENABLE_WATCHDOG() WATCHDOG.Wcr.b.WDE=EnableWDog						//Enable watchdog
#endif //endof ETM - PRODUCT_CONFIGURATION_APPLICATION
	#define 	DISABLE_WATCHDOG() WATCHDOG.Wcr.b.WDE=FALSE						//Disable watchdog
#endif //endof PLATFORM_CURRENT
#endif

// Defines used by GetProcAddress. Needed because we use the NONAME attribute in the
// ImageCore DEF file so nefariuos characters can't see the function names in the
// ImageCore DLL via a dumpbin.

#define GETIMAGECOREREVISION MAKEINTRESOURCE(1)
#define ADAPTIVEDOCUMENT    MAKEINTRESOURCE(2)
#define AUTOCROP			MAKEINTRESOURCE(3)
#define BLUR		        MAKEINTRESOURCE(4)
#define COMPENSATEIMAGE     MAKEINTRESOURCE(5)
#define DOCUMENTFILTER      MAKEINTRESOURCE(6)
#define CROPIMAGE           MAKEINTRESOURCE(7)
#define FLIPIMAGE			MAKEINTRESOURCE(8)
#define DESPECKLE			MAKEINTRESOURCE(9)
#define GAMMACORRECTION     MAKEINTRESOURCE(10)
#define HISTOGRAMSTRETCH    MAKEINTRESOURCE(11)
#define INFINITYFILTER      MAKEINTRESOURCE(12)
#define MIRRORIMAGE			MAKEINTRESOURCE(13)
#define PIXELSKIP			MAKEINTRESOURCE(14)
#define RESIZEIMAGE			MAKEINTRESOURCE(15)
#define ROTATEIMAGE			MAKEINTRESOURCE(16)
#define SHARPENEDGES		MAKEINTRESOURCE(17)
#define UNSHARPANDBRIGHTEN  MAKEINTRESOURCE(18)
#define AVERAGE				MAKEINTRESOURCE(19)
#define COMPUTWHITEVALUE	MAKEINTRESOURCE(20)
//#define DIFFERENCE			MAKEINTRESOURCE(21)
#define GAUSSIANBLUR		MAKEINTRESOURCE(22)
#define HISTOGRAMEQUALIZE   MAKEINTRESOURCE(23)
#define SEMIGAUSS			MAKEINTRESOURCE(24)
#define SHOWHISTOGRAM		MAKEINTRESOURCE(25)
#define RANK				MAKEINTRESOURCE(26)
#define SOBEL				MAKEINTRESOURCE(27)
#define THRESHOLD			MAKEINTRESOURCE(28)
#define TWOTONE				MAKEINTRESOURCE(29)
#define UNSHARPMASK			MAKEINTRESOURCE(30)


// DllExport is defined to nothing at this point for a Windows build because we are
// exporting function names via a .DEF with the NONAME attribute so we can hide the names.
// If we need to go back to exporting via declspec we just change around the lines below
//#ifdef WIN32
//    #define DllExport   __declspec( dllexport )
	//#define DllExport
//#else
//    #define DllExport
//#endif

//int SizeX,SizeY,Depth,ImgType;
#if 0
enum ImageStatus {
	IMAGE_SUCCESS = 0,
	BAD_INPUT_PARAMETER,
	MALLOC_FAILURE,
	NO_ACTION_TAKEN
} ImageStatus;

enum Shapes { DEFAULT, PLUS, BORDER } ;
#endif

typedef struct {
   int x;   // x-coordinate
   int y;   // y-coordinate
} point;
#define Point point

typedef enum ColorFormat {
	BINARY,
	GRAY,
	MONOCOLOR,
	RGB,
} ColorFormat;

typedef struct ImgDesc {
	int Key;			// This should have (in bytes) the size of this structure. Used for extensibility.
	int ImageX;			// The number of pixels in the width of the image
	int ImageY;			// The number of pixels in the height of the image
	int Padding;		// The number of pixels of padding in each row (typically zero)
	int ImageDepth;		// Color depth: 1=Binary, 8=256 colors(grayscale), 24=RGB
	void *ImageData;	// Pointer to the image data array. assumes no padding at end of each line
						//  0 -> we will malloc and return this filled out structure (user must free)
	int ImageBlueX;		// Coordinates of the first blue pixel, used only for monocolor case fission, fusion
	int ImageBlueY;

	ColorFormat Color;
} ImageDesc;

typedef struct
{
  int label;
  int area;
  int perimeter;
  int minx;
  int minxy;
  int maxx;
  int maxxy;
  int miny;
  int minyx;
  int maxy;
  int maxyx;
  int height;
  int width;
  double rectfill;
  double compact;
  int parent;
} measurements;

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* NOTE: Most of these functions require an input and an output Image Descriptor
 * Type 1: Resultant image has same dimensions as the input and can overwrite the input image,
 * Type 2: Resultant image has different but smaller dimensions than the input image and can overwrite the input image
 * Type 3: Resultant image has different and potentially larger dimensions than the input image. This means the destination
 *  image cannot be the same as the input image. If the destination image pointer is NULL then ImageCore will malloc space
 * for the resultant image. It is left to the caller to free the memory when it is no longer needed.
*/

DllExport int GetImageCoreRevision(char *RevisionString);
DllExport int ToGray(ImageDesc *SrcImg, ImageDesc *DstImg);
DllExport int ToRGB(ImageDesc *SrcImg, ImageDesc *DstImg);

#if 1
DllExport int AdaptiveUltraText(ImageDesc *SrcImg, ImageDesc *DstImg);
DllExport int AutoCrop(ImageDesc *SrcImg, ImageDesc *DstImg, double Scale);
DllExport int Average(ImageDesc *SrcImg, ImageDesc *DstImg);
DllExport int Blur( ImageDesc *SrcImg, ImageDesc *DstImg);
DllExport int CompensateImage(ImageDesc *SrcImg, ImageDesc *DstImg);
DllExport int ComputeWhiteValue(ImageDesc *SrcImg, int PercentSetPoint, int *WhiteValue);
DllExport int DocumentFilter(ImageDesc *SrcImg, ImageDesc *DstImg, int UltraTextThreshold);
DllExport int CropImage(ImageDesc *SrcImg, ImageDesc *DstImg, int Top, int Bot, int Lft, int Rgt);
DllExport int FlySpeck(ImageDesc *SrcImg, ImageDesc *DstImg);
DllExport int Difference(ImageDesc *SrcImg, ImageDesc *DstImg);
DllExport int FlipImage(ImageDesc *SrcImg, ImageDesc *DstImg);
/* GammaValue: Strength. Useful range: 0.1 (lightens the dark regions) to 9.99 (darkens the light regions). Typical: ~.6 */
DllExport int GammaCorrection(ImageDesc *SrcImg, ImageDesc *DstImg, double GammaValue);
DllExport int GaussianBlur( ImageDesc *SrcImg, ImageDesc *DstImg, int rows, int cols, double sd);
DllExport int HistogramEqualize(ImageDesc *SrcImg, ImageDesc *DstImg);
DllExport int HistogramStretch(ImageDesc *SrcImg, ImageDesc *DstImg, int BlackPercent, int WhitePercent);
DllExport int InfinityFilter(ImageDesc *SrcImg, ImageDesc *DstImg);
DllExport int MirrorImage(ImageDesc *SrcImg, ImageDesc *DstImg);
DllExport int PixelSkip(ImageDesc *SrcImg, ImageDesc *DstImg, int Skip);
DllExport int Rank(ImageDesc *SrcImg, ImageDesc *DstImg, int rank);
/* factor: Zoom amount. 1.0 = no zoom. Less than 1.0 shrinks, more than 1.0 enlarges. Limited by amount of system memory.
 * Eg: a factor of 2.0 would result in an image 4 times larger. */
DllExport int ResizeImage(ImageDesc *SrcImg, ImageDesc *DstImg, double factor);
/* Degrees: Angle to rotate image. Currently only 90, 180, and 270 are available. */
DllExport int RotateImage(ImageDesc *SrcImg, ImageDesc *DstImg, int Degrees);
DllExport int SemiGauss(ImageDesc *SrcImg, ImageDesc *DstImg, double radius, int maxdelta);
DllExport int SharpenEdges(ImageDesc *SrcImg, ImageDesc *DstImg, int Strength);
DllExport int ShowHistogram(ImageDesc *SrcImg, ImageDesc *DstImg);
DllExport int Sobel(ImageDesc *SrcImg, ImageDesc *DstImg);
DllExport int Threshold(ImageDesc *SrcImg, ImageDesc *DstImg, int threshold);
/* PrintWeight: Broadens or thins black lines. Range: 0 (Broaden) to 8 (thin). Nominal: 3. */
//DllExport int TwoTone(ImageDesc *SrcImg, ImageDesc *DstImg, int PrintWeight);
DllExport int TwoTone(ImageDesc *SrcImg, ImageDesc *DstImg, int PrintWeight, int magicX);
DllExport int UnsharpAndBrighten(ImageDesc *SrcImg, ImageDesc *DstImg);
DllExport int UnsharpMask(ImageDesc *SrcImg, ImageDesc *DstImg, double Radius, double Amount, int Threshold);
DllExport int autoTwoTone(ImageDesc *SrcImg, ImageDesc *DstImg, int PrintWeight);
DllExport int Vignette(ImageDesc *SrcImg, ImageDesc *DstImg, int CenterX, int CenterY, int Amount);
DllExport int Distortion(ImageDesc *SrcImg, ImageDesc *DstImg, int cx, int cy, int k, int InterpolationType);
DllExport int UnSharp(ImageDesc *SrcImg, ImageDesc *DstImg);
DllExport int  Magic(ImageDesc *SrcImg, ImageDesc *DstImg, int X, int Y, int Z);
#endif


#ifdef __cplusplus
}


#endif  /* __cplusplus */


#endif /*IMAGECORE_H_*/
