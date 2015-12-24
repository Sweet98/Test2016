//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/dirt_test/api.h $
// $Revision: 1.1 $
// $Date: 2010/01/25 03:56:23EST $
//======================================================================================

//======================================================================================
#ifndef HEADER_API_H
#define HEADER_API_H

#include <string.h>

// storage for dirt spot information
#define MAX_STORED_DIRT_SPOTS 1280
// maximum dirt perimeter before the algorithm should surrender
#define MAX_SPOT_PERIMETER 4000
// storage for computed convex hull points
#define MAX_CONVEX_HULL 128

// maximum allowed bad pixels (carried over from the original dirt station)
#define MAX_BAD_PIXELS 15
// dirt size criteria
#define MAX_DIRT_RADIUS 14
#define MIN_DIRT_RADIUS 4
// maximum allowed number of dirt spots within the size criteria
#define MAX_LARGE_DIRT_SPOTS 4
// maximum allowed dirt spots of any size
#define MAX_TOTAL_DIRT_SPOTS 8
// upper-left and lower-right points of a rectangle that must be clear of detected dirt spots
#define CLEAR_AREA_LEFT   188
#define CLEAR_AREA_TOP    120
#define CLEAR_AREA_RIGHT  564
#define CLEAR_AREA_BOTTOM 360

// dirt types
#define DIRT_TYPE_OTHER        0
#define DIRT_TYPE_SOFTSPOT     1
#define DIRT_TYPE_SHARPSPOT    2
#define DIRT_TYPE_SPOTWITHHALO 3
#define DIRT_TYPE_BUBBLE       4
#define DIRT_TYPE_ODDSHAPE     5

//*
/*
	Structure: DirtSpotInfo

	Coordinate and radii of dirt spot found.

	Members:

>	x
>	y
>	r

	See Also:

	<DirtTestImage>

*/
typedef struct
{
    int x;
    int y;
    int r;
}
DirtSpotInfo;

#define BADPIXEL_THRESHOLD 32
#define DISCONTINUITY_THRESHOLD 60
#define DISCONTINUITY_COUNT_BIN1 6
#define DISCONTINUITY_COUNT_BIN2 35

/***  All error judgements, and max buffers decisions have been moved up to the application layer
 These values are left here as initial starting points for these values
// return values
#define IMAGE_ERROR 0
#define IMAGE_PASS_BIN1 1
#define IMAGE_PASS_BIN2 2
#define IMAGE_FAIL_PIXEL 3
#define IMAGE_PASS_BLEMISH 4
#define IMAGE_FAIL_BLEMISH 5

// built-in tolerances
#define MINOR_BLEMISHES_ALLOWED 3
#define BADPIXEL_BIN1_ALLOWED 0
#define BADPIXEL_BIN2_ALLOWED 15
#define BADPIXEL_THRESHOLD 32
#define DISCONTINUITY_THRESHOLD 60
#define DISCONTINUITY_COUNT_BIN1 6
#define DISCONTINUITY_COUNT_BIN2 35

// maximum allowed number of various features
//#define MAX_STORAGE_BAD_PIXEL 1024
//#define MAX_STORAGE_DISCONTINUITY 8192
//#define MAX_STORAGE_MAJOR_BLEMISH 128
//#define MAX_STORAGE_MINOR_BLEMISH 128
****/

//#define BORLANDC 1


#ifdef __cplusplus
extern "C"
#endif

int SmearTest(unsigned char *image, int width, int height);
 int ComputeSNR(unsigned char *pImage, int width, int height, double *PixelAverage, double *PixelStandardDeviation, double *NoiseAverage, double *RMSNoise, double *SignalToNoise);
 int CalculateRowNoiseParameters (unsigned char *image, int width, int height, int *topRatio, int *bottomRatio);
 int ProductionTest(unsigned char *testimage, int width, int height, unsigned int config, 
                                                    int maxdis, int *ndis, int *xdis, int *ydis,
                                                    int maxbad, int *nbad, int *xbad, int *ybad,
                                                    int maxmaj, int *nmajor, int *xmajor, int *ymajor,
                                                    int maxmin, int *nminor, int *xminor, int *yminor, int *outofspec);
 int CalculateOpticalFlatness (unsigned char *image, int width, int height, int *cx, int *cy, int *ul, int *uc, int *ur, int *ml, int *mr, int *ll, int *lc, int *lr);
 int DirtTestImage(unsigned char *image, int width, int height, int *numDirtSpots, DirtSpotInfo *di);
 int HorizontalBanding(unsigned char *testimage, int width, int height, int blockXY, int gapXY, double threshold, double *value);

// DLL Revision
 int GetDIRTVersion (char *pstr);

// File Revisions:
 int GetSmearTestRevision (char *pstr);
 int GetSNRRevision (char *pstr);
 int GetRowNoiseCalculationRevision (char *pstr);
 int GetProductionTestRevision (char *pstr);
 int GetFlatnessRevision (char *pstr);
 int GetDirtTestRevision (char *pstr);
 int GetHBRevision (char *pstr);


#endif

/* -----------------------------------------------------------------------------
 * $Source: Gen6FoundationApp/FoundationApp/dirt_test/api.h $
 * $Revision: 1.1 $
 * $Date: 2010/01/25 03:56:23EST $
 *
 * ---------------------------------------------------------------------------*/

