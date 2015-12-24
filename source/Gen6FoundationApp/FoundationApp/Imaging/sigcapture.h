/*======================================================================
 *	"SigCapture.h" -- Signature Capture (IQ Imaaging)
 *======================================================================
 *
 * $RCSfile: Gen6FoundationApp/FoundationApp/Imaging/sigcapture.h $
 * $Revision: 1.1 $
 * $Date: 2009/12/02 10:16:44EST $
 *
 *======================================================================*/


#ifndef SIGCAPTURE_H
#define SIGCAPTURE_H

// Reasons for IMGBOX to fail  -- which is frustrating because it is treated as a bool 1 = true, 0 = false, which means none of this gets back!!
#define 	NO_ERROR 				0
#define 	UNSPECIFIED_ERROR 	1
#define 	TOO_CLOSE_TO_EDGE 	2
#define 	TOO_SKEWED 				3
#define 	IMAGE_OVERFLOW 		4
#define 	UNSUPPORTED_BARCODE 	5
#define 	NO_DCORNER_PDF 		6
#define	NO_IMAGE					7
#define 	DCORNER_IS_0 			88
#define 	DCORNER_OUT_OF_RANGE 89

#ifndef LONG
	#define		LONG	long
#endif

#define 	FINE_THRESHOLDING_FOR_EDGE_CLIMBING 	1
#define	INVERSE_VIDEO									0
#define	SHW												0
#define	NEWEDGE											0
#define	HIDE												0


// Macros defines on the decode side, for now redundantly being put here
// Ideally these might be included in mxbakdor.h or mxapi.h to be used by the system, but no harm in them being defined here again
#include <math.h>
#define TWIX(a,b,c) 		((a <= c)&&(c <= b))
#define _ABS_(d) 			(((d)>=0)?(d):(-(d)))
#define VALIDEDGE(e) 	((e==1)||(e==2))
#define DISQ(x,y) 		((S32)(x)*(x)+(S32)(y)*(y))
#define PDSQ(a,b) 		(DISQ(a.x-b.x,a.y-b.y))
#define DISTANCE(a,b) 	((int)sqrt((double)PDSQ(a,b)))

#define PIXEL_VALUE(p) (*(ld->pImg+p.y*ld->cols+p.x))
//These next #defines describe the orientation of the barcode
#define SORTA_HORIZONTAL 1
#define SORTA_VERTICAL 2
#define SORTA_DIAGONAL 3
typedef struct {
	int slope, intercept;
} line_t;
line_t linear_regression(int *x_points, int *y_points, int num_points);
#define	SLOPE_SHIFT				7			/* binary point location for the slope */
#define 	MINDEL 15   // TPH was 2; we need to do better
#define	TCMAX	512


//Still WRONG place for this -- these want and need to be returned from the standard mxapi.h
typedef struct {
	//int x, y;
	signed int x, y;
} point;

typedef struct tagRect {
	point	ul;
	point	ur;
	point	lr;
	point ll;
}	tRect;

typedef struct {
	int thr, edge;
	point ref, ctr, del;
	LONG x, y;
} refcol;

signed int subpixel(void *pld, long lx, long ly); //, void *Image);
int GetBestSymbolCorners(tRect *NewCorners); //, char *pMyImage);
void RefineCorners(void *pld);
void DefineCorners(void *pld);

//debug
void DrawPoint (unsigned char *Img, point p, int NumOfColsPerRow ) ;
void DrawBox (unsigned char *Img, point ul, point ur, point ll, point lr);
void DrawBox(unsigned char *Img, tRect Rect);
void DrawVerticalLine (unsigned char *Img, point a, point b, int NumOfColsPerRow);
void DrawHorizontalLine(unsigned char *Img, point a, point b, int NumOfColsPerRow);


#endif
