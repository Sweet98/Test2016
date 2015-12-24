/*======================================================================
 *	"SigCaptureUT.c" -- Utilities to support Signature Capture (IQ Imaaging)
 *======================================================================
 *
 * $RCSfile: Gen6FoundationApp/FoundationApp/Imaging/sigcaptureut.c $
 * $Revision: 1.6 $
 * $Date: 2010/10/24 19:50:27EDT $
 *
 *======================================================================*/

#define DEBUG 0
#define	TRACE(x)
#define	TRACE1(x,y)
#define	TRACE2(x,y,z)
#define	CRLF

#include "sigcapture.h"
#include "decodeiface.h"
#include "db_chrio.h"
#include "DecodeIFace.h"
#include "ReaderConfiguration.h"
#include	"menu.h"
#include	"ScanDriverInterface.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//void xputstring(const unsigned char *string);

unsigned char bfr1[300];
const unsigned char *pbfr = bfr1;

void DumpLastDecodeStructure ()
{
	int i;
	char MsgBuf[120];

	sprintf (MsgBuf, "pImg %x, rows %d, cols %d, imgx %d, imgy %d, codeid %c,  ", (int) pgLastDecodeInfo->pImg, pgLastDecodeInfo->rows, pgLastDecodeInfo->cols, pgLastDecodeInfo->imgx, pgLastDecodeInfo->imgy, pgLastDecodeInfo->codeid);
	xputstring (MsgBuf);

  	// standard ints used
  	sprintf (MsgBuf, "dcorner %d, pdfrows %d, pdfcols %d \r\n", pgLastDecodeInfo->dcorner, pgLastDecodeInfo->pdfrows, pgLastDecodeInfo->pdfcols); //, TOPX, TOPY);
  	xputstring (MsgBuf);

	// corners
  	for (i=0;i<4;i++)
  	{
  		sprintf (MsgBuf, "x[%d] %d, y[%d] %d ", i, pgLastDecodeInfo->corners[i].x>>1, i, pgLastDecodeInfo->corners[i].y>>1);
  		xputstring (MsgBuf);
  	}
  	xputstring ("\r\n");

}

int GetBestSymbolCorners(tRect *NewCorners) {
	int    bStatus = NO_ERROR;
	tRect		NewRect;

	if (pgLastDecodeInfo->dcorner == 0)  return (DCORNER_IS_0);
	if( GET_SETTING( ImageEngineType ) == HHPSD_ENGINE_IMAGER_MICRON_MT9V022 ) {
		if (pgLastDecodeInfo->dcorner >= 752 || pgLastDecodeInfo->dcorner <0)	return (DCORNER_OUT_OF_RANGE);
	} else {
		if (pgLastDecodeInfo->dcorner >= 640 || pgLastDecodeInfo->dcorner <0)	return (DCORNER_OUT_OF_RANGE);
	}

	if(pgLastDecodeInfo == NULL || pgLastDecodeInfo->pImg == NULL)
	{
		bStatus = NO_IMAGE;
	}
	else if ((pgLastDecodeInfo->codeid == 'r') || (pgLastDecodeInfo->codeid == 'a') || 
			 (pgLastDecodeInfo->codeid == 'b') || (pgLastDecodeInfo->codeid == 'j') || 
			 (pgLastDecodeInfo->codeid == 'I') || (pgLastDecodeInfo->codeid == 'z') || 
			 (pgLastDecodeInfo->codeid == 'e') || (pgLastDecodeInfo->codeid == 'w')) 
	{
		// Save the original values of corners
		Point corners_bak[4];
		corners_bak[0].x = pgLastDecodeInfo->corners[0].x;
		corners_bak[0].y = pgLastDecodeInfo->corners[0].y;
		corners_bak[1].x = pgLastDecodeInfo->corners[1].x;
		corners_bak[1].y = pgLastDecodeInfo->corners[1].y;
		corners_bak[2].x = pgLastDecodeInfo->corners[2].x;
		corners_bak[2].y = pgLastDecodeInfo->corners[2].y;
		corners_bak[3].x = pgLastDecodeInfo->corners[3].x;
		corners_bak[3].y = pgLastDecodeInfo->corners[3].y;

		// ID outputs symbol corners at normal coordinates, needs to be <<1 to go into refine/define
		pgLastDecodeInfo->corners[0].x <<= 1;
		pgLastDecodeInfo->corners[0].y <<= 1;
		pgLastDecodeInfo->corners[1].x <<= 1;
		pgLastDecodeInfo->corners[1].y <<= 1;
		pgLastDecodeInfo->corners[2].x <<= 1;
		pgLastDecodeInfo->corners[2].y <<= 1;
		pgLastDecodeInfo->corners[3].x <<= 1;
		pgLastDecodeInfo->corners[3].y <<= 1;
		if ((pgLastDecodeInfo->codeid == 'r') && (pgLastDecodeInfo->pdfcols == 0)){ TRACE("No dcorner PDF "); bStatus = NO_DCORNER_PDF;}
		else {
			//---------------------------------------------------
			// Initialize our version of image corners.
			//---------------------------------------------------
	 		if ((pgLastDecodeInfo->codeid == 'a') || (pgLastDecodeInfo->codeid == 'b')||(pgLastDecodeInfo->codeid == 'j')||(pgLastDecodeInfo->codeid == 'I')||(pgLastDecodeInfo->codeid == 'e')) {
//				RefineCorners(pgLastDecodeInfo);
	 			DefineCorners(pgLastDecodeInfo);
			}
		}
 		/*Divide by 2 becuase autozoom works in the 2x world */
		NewRect.ul.x = pgLastDecodeInfo->corners[0].x >> 1;
		NewRect.ul.y = pgLastDecodeInfo->corners[0].y >> 1;
		NewRect.ur.x = pgLastDecodeInfo->corners[1].x >> 1;
		NewRect.ur.y = pgLastDecodeInfo->corners[1].y >> 1;
		NewRect.lr.x = pgLastDecodeInfo->corners[2].x >> 1;
		NewRect.lr.y = pgLastDecodeInfo->corners[2].y >> 1;
		NewRect.ll.x = pgLastDecodeInfo->corners[3].x >> 1;
		NewRect.ll.y = pgLastDecodeInfo->corners[3].y >> 1;
		/* The first thing that we're going to do is check the corners to make sure that they do not
 		* border on the edge of an image.  If they do, then we cannot be sure that a part of the bar code
 		* wasn't chopped off by the edge of thei image.*/
		if ((NewRect.ul.x > (pgLastDecodeInfo->cols-5)) || (NewRect.ul.x < 5)){ TRACE("Too Close"); bStatus = TOO_CLOSE_TO_EDGE;}
		if ((NewRect.ul.y > (pgLastDecodeInfo->rows-5)) || (NewRect.ul.y < 5)){ TRACE("Too Close"); bStatus = TOO_CLOSE_TO_EDGE;}
		if ((NewRect.ur.x > (pgLastDecodeInfo->cols-5)) || (NewRect.ur.x < 5)){ TRACE("Too Close"); bStatus = TOO_CLOSE_TO_EDGE;}
		if ((NewRect.ur.y > (pgLastDecodeInfo->rows-5)) || (NewRect.ur.y < 5)){ TRACE("Too Close"); bStatus = TOO_CLOSE_TO_EDGE;}
		if ((NewRect.lr.x > (pgLastDecodeInfo->cols-5)) || (NewRect.lr.x < 5)){ TRACE("Too Close"); bStatus = TOO_CLOSE_TO_EDGE;}
		if ((NewRect.lr.y > (pgLastDecodeInfo->rows-5)) || (NewRect.lr.y < 5)){ TRACE("Too Close"); bStatus = TOO_CLOSE_TO_EDGE;}
		if ((NewRect.ll.x > (pgLastDecodeInfo->cols-5)) || (NewRect.ll.x < 5)){ TRACE("Too Close"); bStatus = TOO_CLOSE_TO_EDGE;}
		if ((NewRect.ll.y > (pgLastDecodeInfo->rows-5)) || (NewRect.ll.y < 5)){ TRACE("Too Close"); bStatus = TOO_CLOSE_TO_EDGE;}

		if (bStatus != TOO_CLOSE_TO_EDGE) {
			int ldist,rdist;
			ldist = DISTANCE(pgLastDecodeInfo->corners[0],pgLastDecodeInfo->corners[3]);
			rdist = DISTANCE(pgLastDecodeInfo->corners[1],pgLastDecodeInfo->corners[2]);
			if (ldist > 4*rdist/3 || rdist > 4*ldist/3) {
				bStatus = TOO_SKEWED;
			}
		}
		NewCorners->ul = NewRect.ul;
		NewCorners->ur = NewRect.ur;
		NewCorners->lr = NewRect.lr;
		NewCorners->ll = NewRect.ll;

		// Restore the original values to corners
		pgLastDecodeInfo->corners[0].x = corners_bak[0].x;
		pgLastDecodeInfo->corners[0].y = corners_bak[0].y;
		pgLastDecodeInfo->corners[1].x = corners_bak[1].x;
		pgLastDecodeInfo->corners[1].y = corners_bak[1].y;
		pgLastDecodeInfo->corners[2].x = corners_bak[2].x;
		pgLastDecodeInfo->corners[2].y = corners_bak[2].y;
		pgLastDecodeInfo->corners[3].x = corners_bak[3].x;
		pgLastDecodeInfo->corners[3].y = corners_bak[3].y;
	}
	else bStatus = UNSUPPORTED_BARCODE;
	return bStatus;
}




/*----------------------------------------------------------------------*/
/*						New stuff related to Mesa decoding							*/
/*----------------------------------------------------------------------*/

SINT Thresh(LastDecodeInfo_t *pld, LONG x, LONG y, point h) {
	SINT i, s, max, min;
	LONG xm, xp, ym, yp;

#if FINE_THRESHOLDING_FOR_EDGE_CLIMBING
	xm = x + (LONG) h.x/8; ym = y + (LONG) h.y/8; xp = x - (LONG) h.x/8; yp = y - (LONG) h.y/8;
	for (i=1,min=255,max=0; i<12; i++) {
		xm -= (LONG) h.x/8; ym -= (LONG) h.y/8;
#else
	xm = x + (LONG) h.x/2; ym = y + (LONG) h.y/2; xp = x - (LONG) h.x/2; yp = y - (LONG) h.y/2;
	for (i=1,min=255,max=0; i<8; i++) {
		xm -= (LONG) h.x; ym -= (LONG) h.y;
#endif
		if (TWIX(0,(((LONG)pld->cols-2)<<8),xm) && TWIX(0,(((LONG)pld->rows-2)<<8),ym)) {
			 s = subpixel(pld,xm,ym); if (s < min) min = s; if (s > max) max = s;
		}
#if FINE_THRESHOLDING_FOR_EDGE_CLIMBING
		xp += (LONG) h.x/8; yp += (LONG) h.y/8;
#else
		xp += (LONG) h.x; yp += (LONG) h.y;
#endif
		if (TWIX(0,(((LONG)pld->cols-2)<<8),xp) && TWIX(0,(((LONG)pld->rows-2)<<8),yp)) {
			s = subpixel(pld, xp, yp); if (s < min) min = s; if (s > max) max = s;
		}
	}
/*	return ((min+max)/2); */	/* 50% */
#if INVERSE_VIDEO == 0
	return ((3*min+2*max)/5);	/* 40% */
#else /* INVERSE_VIDEO == 1 */
	return ((3*max+2*min)/5);	/* 40% */
#endif
}



SINT onEdge(LastDecodeInfo_t *pld, refcol *rc, point h) {
	SINT i, sl, sr, thr, abshx, abshy;
	LONG xl, xr, yl, yr;
	//xputstring  ((const unsigned char *)"onEdge\r\n");
#if (SHW)&&(!HIDE)
 putpixel(pld->imgx+((rc->x+128)>>8),pld->imgy+((rc->y+128)>>8),12);
#elif (CYBERANDY)
 putpixel1(((rc->x+128)>>8),((rc->y+128)>>8),DIAG_COLOR_MAGENTA);
#endif

	abshx = abs(h.x); abshy = abs(h.y);
	if ((TWIX(abshx,((LONG)(pld->cols-2)<<8)-abshx,rc->x))
	 &&(TWIX(abshy,((LONG)(pld->rows-2)<<8)-abshy,rc->y))) {

		thr = rc->thr;
		TRACE1("thr:%d ",thr);
		for (i=8; i>1; i>>=1) {
			xl = rc->x - (LONG) h.x/(LONG) i; yl = rc->y - (LONG) h.y/(LONG) i; sl = subpixel(pld, xl, yl);
			 xr = rc->x + (LONG) h.x/(LONG) i; yr = rc->y + (LONG) h.y/(LONG) i; sr = subpixel(pld, xr, yr);
			 TRACE2("sl:%d;sr:%d ",sl,sr);
			if ((sl < thr)&&(sr > thr)) {
				if (i < 8) {
					if (sr-thr > thr-sl) { rc->x -= (LONG) h.x/8; rc->y -= (LONG) h.y/8; }
					 else { rc->x += (LONG) h.x/8; rc->y += (LONG) h.y/8; }
				}
				return 2;
			} else if ((sl > thr)&&(sr < thr)) {
				if (i < 8) {
					if (sl-thr > thr-sr) { rc->x += (LONG) h.x/8; rc->y += (LONG) h.y/8; }
					 else { rc->x -= (LONG) h.x/8; rc->y -= (LONG) h.y/8; }
				}
				return 1;
			}
		}

		if (sl > thr) return 0; else return 3;

	} else return 8;
}

SINT ClimbEdge(LastDecodeInfo_t *pld, refcol *rc, point h, point v, SINT mmore) {
	int newedge=0, len, more;
	LONG xf, xl, yf, yl;

  //	xputstring  ((const unsigned char *)"ClimbEdge\r\n");
	xf = xl = rc->x; yf = yl = rc->y; len = 0;
	if (VALIDEDGE(rc->edge)) {
		more = mmore;
		while (more) {
			len++;
			rc->x += v.x; rc->y += v.y;
			newedge = onEdge(pld, rc, h); //onEdge(ws,rc,h,Image);
			if (newedge == rc->edge) {
				xl = rc->x; yl = rc->y; more = mmore;
				if ((len &0x3)==0x3)rc->thr = Thresh(pld, rc->x,rc->y,h); /* only recalculate threshold on valid edge */
			}
#if (NEWEDGE)
//			else if (newedge == 3-(rc->edge)) more = 0;
			else if (((newedge==1)&&(rc->edge==2))||((newedge==2)&&(rc->edge==1))) more = 0;
#else
			else if (newedge == -(rc->edge)) more = 0;
#endif
			else more--;
		}
		rc->thr = Thresh(pld, (xf+xl+1)/2,(yf+yl+1)/2,h);
		rc->edge = newedge;
	}
	rc->ctr.x = (xf+xl+256)/512; rc->ctr.y = (yf+yl+256)/512;
#if (SHW)&&(!HIDE)
 circle(pld->imgx+rc->ctr.x,pld->imgy+rc->ctr.y,2);
#endif
	return (len);
}







#if 1
/*********************************************************************************
 * RefineCorners - When we come into this routine, we try to verify that the
 *   corners are actually at the corner of a linear or PDF.  This should not
 *   allow the corners to get any worse than they already are, but should in
 *   fact make them better if anything, or at worst leave them alone.
 *********************************************************************************/
void RefineCorners(void *pld) {
	LastDecodeInfo_t *ld = (LastDecodeInfo_t *)pld;
	point h,v;
	refcol rc[4];
	int i,more=4;
	long vectLen,hdirx,hdiry,vdirx,vdiry;
#if DEBUG
	for (i=0;i<4;i++)  {
		TRACE2("cor%d.x:%d",i,ld->corners[i].x>>1);TRACE2(" cor%d.y:%d",i,ld->corners[i].y>>1);
	}
#endif
	/* by calculating the distance between the top corners of the barcode and dividing that by
	 * dcorner, we get a good approximation for how wide a narrow element is.  Therefore, using
	 * this information we create a scaler value vectLen which is the magnitude of this vector, or
	 * a magnitude of how many pixels a narrow element is.  We will then use corner information,
	 * top and bottom for a single side, to calculate a vertical direction we want to travel in,
	 * and go perpendicular to calculate a horizontal direction to measure the edge in.  However,
	 * this vectLen vector will give us the magnitude of the narrow element which will allow us
	 * to scale the horizontal/vertical directions of edge climbing vectors accordingly.  */
  	if(ld->pdfcols > 0)	// Don't do this if it will cause a divide by zero.
	{
		hdirx = (LONG)((LONG)_ABS_(((LONG)ld->corners[0].x<<7) - ((LONG)ld->corners[1].x<<7)))/(LONG)ld->pdfcols;
		hdiry = (LONG)((LONG)_ABS_(((LONG)ld->corners[0].y<<7) - ((LONG)ld->corners[1].y<<7)))/(LONG)ld->pdfcols;
	}
	else
	{
		hdirx = 0;
		hdiry = 0;
	}
#if DEBUG
	CRLF;
	TRACE2("((LONG)_ABS_((corner[0].y<<7) - (corner[1].y<<7))):%ld,dcorner:%ld",((LONG)_ABS_((LONG)(ld->corners[0].y<<7) - (LONG)(ld->corners[1].y<<7))),(LONG)ld->dcorner); CRLF;
	TRACE2("hdirx:%d, div256:%d ",(int)hdirx,(int)hdirx>>8); CRLF;
	TRACE2("hdiry:%d, div256:%d ",(int) hdiry,(int) hdiry>>8); CRLF;
#endif
	/* vectLen is an approximation of how many pixels that a single element width
	 * is for our bar code.  An accurate value would be    SQRT((hdirx*hdirx)+(hdiry*hdiry));
	 * Worst case is that this number is 1.414 times what it
	 * needs to be, and best case it's right on.  In any case it's always the larger
	 * than it needs to be, which is better than smaller.*/
	vectLen = hdirx+hdiry;
	for (i=0;i<2;i++) {
		/*The reference x,y coordinates of the rc structure are in real pixel coordinates, and the x,y values are in pixel coordinates*256
		 * (for 8 bit sub integer accuracy).  Since the corners are saved as pixel coordinates * 2, the reference x,y is the corner value
		 * divided by 2, and the rc.x,y values are the corners * 128.*/
		rc[i].ref.x = ld->corners[i].x>>1;
		rc[i].ref.y = ld->corners[i].y>>1;
		rc[3-i].ref.x = ld->corners[3-i].x>>1;
		rc[3-i].ref.y = ld->corners[3-i].y>>1;
		rc[i].x = (LONG)ld->corners[i].x<<7;
		rc[i].y = (LONG)ld->corners[i].y<<7;
		rc[3-i].x = (LONG)ld->corners[3-i].x<<7;
		rc[3-i].y = (LONG)ld->corners[3-i].y<<7;
		/* Here again we are trying to do component vectors.  For speed again we use the absolue value summations of
		 * the components of one vector to approximate the entire magnitude of the vector. */
		// protect against div by zero, just leave NOW!!
		if (   (_ABS_(rc[i].x-rc[3-i].x)+ _ABS_(rc[i].y-rc[3-i].y) == 0))
			return;		// leave NOW!!

		hdirx = ((rc[i].y-rc[3-i].y)*vectLen)/(_ABS_(rc[i].x-rc[3-i].x)+ _ABS_(rc[i].y-rc[3-i].y));
		hdiry = -((rc[i].x-rc[3-i].x)*vectLen)/(_ABS_(rc[i].x-rc[3-i].x)+ _ABS_(rc[i].y-rc[3-i].y));
		vdirx = ((rc[i].x-rc[3-i].x)*vectLen)/((_ABS_(rc[i].x-rc[3-i].x)+ _ABS_(rc[i].y-rc[3-i].y))*2);
		vdiry = ((rc[i].y-rc[3-i].y)*vectLen)/((_ABS_(rc[i].x-rc[3-i].x)+ _ABS_(rc[i].y-rc[3-i].y))*2);
		TRACE2("vdirx:%ld,vdiry:%ld  ",vdirx,vdiry);
		/* Up until this point we've needed long variables to do the calculations.  Now we can downscale them to
		 * SINTs (In the PC decoder SINTs are 16bit as a posed to LONGs being 32bit), because I think we are within
		 * the accuracy of 16bits */
		v.x = (SINT)vdirx;
		v.y = (SINT)vdiry;
		h.x = (SINT)hdirx;
		h.y = (SINT)hdiry;
		TRACE2("h.x:%d, h.y:%d",h.x,h.y); CRLF;
		TRACE2("v.x:%d, v.y:%d",v.x,v.y); CRLF;
		/* rc[i] represents the climbing of edges up toward the top of the barcode in both iterations, and rc[3-i] represents
		 * the climbinb edges down to the bottom of the barcode.  We start by going up.*/
		rc[i].thr = Thresh(ld,rc[i].x,rc[i].y,h); rc[i].edge = onEdge(ld,rc+i,h);
		if (VALIDEDGE(rc[i].edge)) {
			TRACE1("VALID %d  ",i);
		 	ClimbEdge(ld,rc+i,h,v,more);
			ld->corners[i].x = (((rc[i].x-(LONG)more*v.x+128)/256))*2;
			 ld->corners[i].y = (((rc[i].y-(LONG)more*v.y+128)/256))*2;
		} else {
			ld->corners[i].x = (((rc[i].x+(LONG)more*v.x+128)/256))*2;
			 ld->corners[i].y = (((rc[i].y+(LONG)more*v.y+128)/256))*2;
		}
		/*Invert the climb direction for going down. */
		v.x = -v.x;
		v.y = -v.y;
		rc[3-i].thr = Thresh(ld,rc[3-i].x,rc[3-i].y,h); rc[3-i].edge = onEdge(ld,rc+3-i,h);
		if (VALIDEDGE(rc[3-i].edge)) {
			TRACE1("VALID %d  ",3-i); CRLF;
		 	ClimbEdge(ld,rc+3-i,h,v,more);
			ld->corners[3-i].x = (((rc[3-i].x-(LONG)more*v.x+128)/256))*2;
			 ld->corners[3-i].y = (((rc[3-i].y-(LONG)more*v.y+128)/256))*2;
		} else {
			ld->corners[3-i].x = (((rc[3-i].x+(LONG)more*v.x+128)/256))*2;
			 ld->corners[3-i].y = (((rc[3-i].y+(LONG)more*v.y+128)/256))*2;
		}
	}
}
#endif




/*********************************************************************/
/*	"Intersection(a,b,c,d,*ok)" returns the point where ab crosses cd	*/
//******* redundant so it exists on both sides of the decoder ********
Point IntersectionS (Point a, Point b, Point c, Point d, int *ok) {
	Point e; LNGLNG dxab, dyab, xyab, dxcd, dycd, xycd, den;
	dxab = (LNGLNG)a.x - b.x; dyab = (LNGLNG)a.y - b.y;
	 dxcd = (LNGLNG)c.x - d.x; dycd = (LNGLNG)c.y - d.y;
	  den = dxab * dycd - dyab * dxcd;
	if (den) {
		xyab = (LNGLNG)a.x * b.y - (LNGLNG)a.y * b.x;
		xycd = (LNGLNG)c.x * d.y - (LNGLNG)c.y * d.x;
		e.x = (int)((xyab * dxcd - dxab * xycd + den/2) / den);
		e.y = (int)((xyab * dycd - dyab * xycd + den/2) / den);
	} else {
		e.x = (a.x + b.x + c.x + d.x) / 4;
		e.y = (a.y + b.y + c.y + c.y) / 4;
		*ok &= FALSE;
	}
	return (e);
}
/*
 *  DefineCorners() - Find the corners by climbing all the bars
 *		1) Search for bars between line segment defined by midpoints of corner 0,3 and 1,2
 * 	2) Search up and down each bar for endpoints
 * 	3) Reduce the number of endpoints by only accepting the longest 1/4 of them
 * 	4) Perform a linear regression on these points (both upper and lower) to give an upper and lower boundary line
 * 	5) find the lines defined by the first and last bars
 * 	6) Find the intersection points of the 4 lines to define the new corners
 *
 * 	Note: Non-linearity of the symbol, such as that of a symbol on a cylinder, will result in unsatisfactory results.
*/
void DefineCorners (void *pld) {
	LastDecodeInfo_t *ld = (LastDecodeInfo_t *)pld;
	int	IntersectionStatus;
	SINT i, imax, sum, difx, dify, dirx, diry, next, peak, delta;
	SINT hiavg, loavg, lasthi, lastlo, thresh;
	BOOL horizontal;
	UCHAR	*pix, profile[2048], *pro, *hi, *lo, *LoPix;
   Point A,B,C,D,E,F;
   UCHAR lum;
   int BarNum=0;
   int Xdelta,Ydelta;
   int temp;
	int EdgeCount= 1;
   Point PointCenter, PointBefore, PointAfter, PointBeforeFlank, PointAfterFlank, CurrentPoint, OriginPoint;
   int q;
   int j;
	UCHAR *img;
	Point NewCorners[4];
//#if DESK
#ifdef DESK
   int y;
	UCHAR    *pImgTemp;
#endif
	int UpX[TCMAX];
	int UpY[TCMAX];
	int DownX[TCMAX];
	int DownY[TCMAX];
   int Lengths[TCMAX];
   int Orientation;
   int SlopePolarity;
	line_t Line;
	WOOF;
	//
	PointBeforeFlank.x = 0;
	PointBeforeFlank.y = 0;
	PointAfterFlank.x = 0;
	PointAfterFlank.y = 0;
	A.x = (ld->corners[0].x + ld->corners[3].x ) / 4;
	A.y = (ld->corners[0].y + ld->corners[3].y ) / 4;
	B.x = (ld->corners[1].x + ld->corners[2].x ) / 4;
	B.y = (ld->corners[1].y + ld->corners[2].y ) / 4;
   Xdelta = A.x - B.x;
   Ydelta = A.y - B.y;
   if (abs(Xdelta)/2 >= abs(Ydelta)) Orientation = SORTA_HORIZONTAL;
   else if (abs(Ydelta)/2 >= abs(Xdelta)) Orientation = SORTA_VERTICAL;
   else Orientation = SORTA_DIAGONAL;
   if (Xdelta*Ydelta >= 0) SlopePolarity = 1;
   else SlopePolarity = -1;

	img=ld->pImg;
	pix = ld->pImg + A.y*ld->cols + A.x; delta = 12;
	next = peak = *pix;
   hiavg = loavg = lastlo = lasthi = 0;
   LoPix = pix;

	difx = abs(B.x-A.x); dirx = ((B.x-A.x) > 0)? 1:-1;
   dify = abs(B.y-A.y); diry = ((B.y-A.y) > 0)? 1:-1;
	if (difx > dify) { imax = difx; sum = difx>>1; horizontal = TRUE; }
   else { imax = dify; sum = dify>>1; horizontal = FALSE; }
	for (i=0,hi=lo=pro=profile; (i<imax); i++,pro++) {
		if (horizontal) {
			pix += dirx; sum += dify; if (sum >= difx) {
				sum -= difx; pix += diry*ld->cols; }
		} else {
			pix += diry*ld->cols; sum += difx; if (sum >= dify) {
				sum -= dify; pix += dirx; }
		}
		next = *pro = *pix;
		if (EdgeCount & 1) { // In a Space
			if (next > peak) { peak = next; hi = pro; }
         else {
				if (loavg) { delta = (next - loavg) / 3; if (delta < MINDEL) delta = MINDEL; }
				if (next < peak-delta) {
					lasthi = peak;
					if (lo > profile) {
						thresh = (lasthi + lastlo + hiavg + loavg)>>2;
						 if (!TWIX(lastlo,lasthi,thresh)) thresh = lasthi - 1;
						for (; (SINT)*lo<=thresh; lo++) {}
					}
					EdgeCount++;
					if (hiavg) hiavg = (3*hiavg + peak)>>2; else hiavg = peak;
					peak = next; lo = pro;
               LoPix = pix;
				}
			}
	   } else { // In a Bar
         if (*pix <*LoPix) LoPix = pix;
			if (next < peak) { peak = next; lo = pro; }
         else {
				if (loavg) { delta = (hiavg - next) / 3; if (delta < MINDEL) delta = MINDEL; }
				if (next > peak + delta) {
					lastlo = peak;
					thresh = (lasthi + lastlo + hiavg + loavg)>>2;
					 if (!TWIX(lastlo,lasthi,thresh)) thresh = lastlo + 1;
					for (; (SINT)*hi>=thresh; hi++) {}
					EdgeCount++;
					if (loavg) loavg = (3*loavg + peak)>>2; else loavg = peak;
					peak = next; hi = pro;

               OriginPoint.x = (LoPix - img) % ld->cols;
               OriginPoint.y = (LoPix - img) / ld->cols;
               CurrentPoint = OriginPoint;
					// Search up and down the bar to find the endpoints
               for (q=-1; q<2; q+=2) {
                  lum = *profile;
                  PointCenter = CurrentPoint = OriginPoint;
                  temp = 0;
                  while (CurrentPoint.x>1 && CurrentPoint.x<ld->cols-2 && CurrentPoint.y>1 && CurrentPoint.y<ld->rows-2) {
							// determine the next set of points that are candidates
                     if (Orientation == SORTA_HORIZONTAL) {
                        if (temp) {
	                        PointCenter.x = CurrentPoint.x;
   	                     PointCenter.y = CurrentPoint.y + q;
                        }
                        PointBefore.x = PointCenter.x - q;
                        PointBefore.y = PointCenter.y;
                        PointAfter.x = PointCenter.x + q;
                        PointAfter.y = PointCenter.y;
                     } else if (Orientation == SORTA_VERTICAL) {
                        if (temp) {
	                        PointCenter.x = CurrentPoint.x + q;
   	                     PointCenter.y = CurrentPoint.y;
                        }
                        PointBefore.x = PointCenter.x;
                        PointBefore.y = PointCenter.y - q;
                        PointAfter.x = PointCenter.x;
                        PointAfter.y = PointCenter.y + q;
                     } else {
                        if (temp) {
	                        PointCenter.x = CurrentPoint.x - q * SlopePolarity;
   	                     PointCenter.y = CurrentPoint.y + q;
                        }
                        PointBefore.x = PointCenter.x;
                        PointBefore.y = PointCenter.y + q;
                        PointAfter.x = PointCenter.x - q * SlopePolarity;
                        PointAfter.y = PointCenter.y;
                        PointBeforeFlank.x = PointCenter.x + q * SlopePolarity;
                        PointBeforeFlank.y = PointCenter.y + q;
                        PointAfterFlank.x = PointCenter.x - q * SlopePolarity;
                        PointAfterFlank.y = PointCenter.y - q;
                     }
							// Find the blackest of the candidate points
                     temp = 1;
                     if (PIXEL_VALUE(PointCenter) > PIXEL_VALUE(PointAfter) && PIXEL_VALUE(PointBefore) > PIXEL_VALUE(PointAfter)) {
                        CurrentPoint = PointAfter;
                     } else if (PIXEL_VALUE(PointCenter) > PIXEL_VALUE(PointBefore)) {
                        CurrentPoint = PointBefore;
                     } else {
                        CurrentPoint = PointCenter;
                     }
                     if (Orientation == SORTA_DIAGONAL) {
                        if (PIXEL_VALUE(CurrentPoint) > PIXEL_VALUE(PointBeforeFlank)) CurrentPoint = PointBeforeFlank;
                        if (PIXEL_VALUE(CurrentPoint) > PIXEL_VALUE(PointAfterFlank)) CurrentPoint = PointAfterFlank;
                     }
							// If one isn't dark enough, then we are at the end of the bar
                     if (PIXEL_VALUE(CurrentPoint) > (UCHAR)(lum + MINDEL)) break;
                     lum =  (PIXEL_VALUE(CurrentPoint)>>2) - (lum>>2) + lum;
                  }
						// Save the endpoint
                  if (q == -1) {
                     DownX[BarNum] = CurrentPoint.x;
                     DownY[BarNum] = CurrentPoint.y;
                  } else {
                     UpX[BarNum] = CurrentPoint.x;
                     UpY[BarNum] = CurrentPoint.y;
                  }
               }
               BarNum++;
				}
			}
		}
	}
	// We now have all our bars characterized. Now I'm going to find the bar that is longer than 3/4 of the bars and
	// invalidate those bars that are not within +/- 10% of that length.
   for (i=0; i<BarNum-1; i++) Lengths[i] = abs(UpX[i] - DownX[i]) + abs(UpY[i] - DownY[i]);
   for (j=0; j<BarNum-1; j++) {
      for (i=0; i<BarNum-1; i++) {
         if (Lengths[i] < Lengths[i+1]) {temp = Lengths[i]; Lengths[i] = Lengths[i+1]; Lengths[i+1] = temp;}
      }
   }
   temp = Lengths[BarNum/4];
   for (i=0; i<BarNum; i++) {
      j = abs(UpX[i] - DownX[i]) + abs(UpY[i] - DownY[i]);
      if (j > 11*temp/10 || j < 9*temp/10) {UpY[i]=-1;DownY[i]=-1;}
   }

	// Make sure we have enough bars to produce meaningful results
  	for (i=temp=0; i<BarNum; i++) if (UpY[i] != -1 && DownY[i] != -1) temp++;
	if (temp <= 5) return; // Probably can't improve corners much on so few points

	//
   i = 0; Xdelta = 0; Ydelta = 0;
	for (temp=0; temp<3; temp++) {
     	while ((UpY[i] == -1 || DownY[i] == -1) && i<BarNum) i++;
		Xdelta += (UpX[i] - DownX[i]);
		Ydelta += (UpY[i] - DownY[i]);
		i++;
	}
	Xdelta /= 3; Ydelta /= 3;
	//C,D are points on line through CORNER[0] with same slope as the first line segment used for linear regression
   C.x = A.x - Xdelta;
   C.y = A.y - Ydelta;
   D.x = A.x + Xdelta;
   D.y = A.y + Ydelta;

   i = BarNum-1; Xdelta = 0; Ydelta = 0;
	for (temp=0; temp<3; temp++) {
		while ( i>=0 ? (UpY[i] == -1 || DownY[i] == -1) : false)i--;
		Xdelta += (UpX[i] - DownX[i]);
		Ydelta += (UpY[i] - DownY[i]);
		i--;
	}
	Xdelta /= 3; Ydelta /= 3;
	//E,F are points on line through CORNER[1] with same slope as the first line segment used for linear regression
   E.x = B.x - Xdelta;
   E.y = B.y - Ydelta;
   F.x = B.x + Xdelta;
   F.y = B.y + Ydelta;

	// Compute the line of the 'top' end points
	if (Orientation != SORTA_VERTICAL) Line = linear_regression(UpX,UpY,BarNum);
	else Line = linear_regression(UpY,UpX,BarNum);
//#if DESK
#ifdef DESK
	if (Orientation != SORTA_VERTICAL) {
   	for (i=0; i<pgLastDecodeInfo->cols; i++) {
      	y = ((i*Line.slope) >> SLOPE_SHIFT)+Line.intercept;
			if (y >= 0 && y < ld->rows) *(ld->pImg+y*ld->cols+i) = 255;
   	}
	} else {
   	for (i=0; i<pgLastDecodeInfo->rows; i++) {
      	y = ((i*Line.slope) >> SLOPE_SHIFT)+Line.intercept;
			if (y >= 0 && y < ld->cols) *(ld->pImg+i*ld->cols+y) = 255;
   	}
	}
#endif
	// A,B are two points on line defined by the 'upper' endpoints.
	if (Orientation != SORTA_VERTICAL) {
		A.x = 0; A.y = Line.intercept;
		B.x = ld->cols; B.y = ((ld->cols*Line.slope) >> SLOPE_SHIFT)+Line.intercept;
	} else {
		A.y = 0; A.x = Line.intercept;
		B.y = ld->rows; B.x = ((ld->rows*Line.slope) >> SLOPE_SHIFT)+Line.intercept;
	}

	NewCorners[0] = IntersectionS(A,B,C,D,&IntersectionStatus);
	NewCorners[1] = IntersectionS(A,B,E,F,&IntersectionStatus);

	// Compute the line of the 'bottom' end points
	if (Orientation != SORTA_VERTICAL)Line = linear_regression(DownX,DownY,BarNum);
	else Line = linear_regression(DownY,DownX,BarNum);
//#if DESK
#ifdef DESK
	if (Orientation != SORTA_VERTICAL) {
   	for (i=0; i<ld->cols; i++) {
      	y = ((i*Line.slope) >> SLOPE_SHIFT)+Line.intercept;
			if (y >= 0 && y < ld->rows) *(ld->pImg+y*ld->cols+i) = 255;
   	}
	} else {
   	for (i=0; i<ld->rows; i++) {
      	y = ((i*Line.slope) >> SLOPE_SHIFT)+Line.intercept;
			if (y >= 0 && y < ld->cols) *(ld->pImg+i*ld->cols+y) = 255;
   	}
	}
#endif
	// Intersection needs points so pick 2 points on line (at edge of image)
	if (Orientation != SORTA_VERTICAL) {
		A.x = 0; A.y = Line.intercept;
		B.x = ld->cols; B.y = ((ld->cols*Line.slope) >> SLOPE_SHIFT)+Line.intercept;
	} else {
		A.y = 0; A.x = Line.intercept;
		B.y = ld->rows; B.x = ((ld->rows*Line.slope) >> SLOPE_SHIFT)+Line.intercept;
	}
	NewCorners[3] = IntersectionS(A,B,C,D,&IntersectionStatus);
	NewCorners[2] = IntersectionS(A,B,E,F,&IntersectionStatus);
//#if DESK
#ifdef DESK
	for (i=0;i<4;i++) {
		pImgTemp = ld->pImg + ((NewCorners[i].y)*ld->cols) + (NewCorners[i].x);
		for (j=-6;j<=6;j++) {
			*(pImgTemp+j)=255;
		}
		for (j=-6;j<=6;j++) {
			*(pImgTemp+(j*pgLastDecodeInfo->cols))=255;
		}
	}
#endif
	// Save the new corner values
	if ((Orientation != SORTA_VERTICAL && ld->corners[0].x < ld->corners[1].x) || (Orientation == SORTA_VERTICAL && ld->corners[0].y > ld->corners[1].y))
		for (i=0;i<4;i++) ld->corners[i] = NewCorners[3-i];
	else
		for (i=0;i<4;i++) ld->corners[i] = NewCorners[i];
	for (i=0;i<4;i++) {ld->corners[i].x *= 2; ld->corners[i].y *= 2;} // corners[] are double resolution

}

// LinearRegression() computes the line that best fits a set of points using the least-squares method.
//		Points with a negative value are ignored allowing one to knockout points from consideration.
//		If a math coprocessor is used then 'double' math might be better.
// 	Since slopes of vertical lines approach infinity, lines that are vertical need to be translated to horisontal (Swap
//		the X and Y values.)
line_t linear_regression(int *x_points, int *y_points, int max_num_points) {
	long x_avg = 0;
	long y_avg = 0;
	long  numerator = 0;
	long denominator = 0;
	long Slope,Intercept;
	int i;
   int num_points;
	line_t line;

   num_points = 0;
	for (i=0; i<max_num_points; i++) {
      if (*(x_points+i) < 0 || *(y_points+i) < 0) {
      } else {
   		x_avg += *(x_points+i);
	   	y_avg += *(y_points+i);
         num_points++;
      }
	}
   if (num_points == 0) {line.slope = 0; line.intercept = 0; return line;}
	x_avg = ((long) (((int) x_avg + num_points / 2) / num_points));
	y_avg = ((long) (((int) y_avg + num_points / 2) / num_points));

	for (i=0; i<max_num_points; i++) {
     if (!(*(x_points+i) < 0 || *(y_points+i) < 0)) {
   		numerator   += (*(x_points+i) - x_avg) * ((long) *(y_points+i) + (long) *(x_points+i) - y_avg - x_avg);
	   	denominator += (*(x_points+i) - x_avg) * (*(x_points+i) - x_avg);
     }
	}
	if (denominator == 0) denominator++;
	Slope = (numerator << SLOPE_SHIFT) / denominator - (1<<SLOPE_SHIFT);
	Intercept = y_avg - ((Slope * x_avg + (1<<(SLOPE_SHIFT-1))) >> SLOPE_SHIFT);
	line.slope = (int)Slope;
	line.intercept = (int)Intercept;
	return line;
}







/*------------------------------------------------------------------------
	subpixel() returns an interpolated grayscale value for a point in the
	grayscale image "pgLastDecodeInfo->pImg[]" at (lx,ly) where those coordinates have 8 bits
	of fractional significance.  subpix2() functions faster, from within
	LineScan(), knowing that one of the coordinates is -on- a pixel line.
------------------------------------------------------------------------*/
SINT subpixel(void *pld, LONG lx, LONG ly) {

	LastDecodeInfo_t *ld = (LastDecodeInfo_t *)pld;
	LONG px, py, dx, dy; PIXEL *grxy;
	grxy = ld->pImg + (lx>>8) + (ly>>8)*ld->cols;
	 px = lx&0xff; dx = 256-px; py = ly&0xff; dy = 256-py;
	return (((LONG)(*grxy)*dx*dy
		+ (LONG)(*(grxy+1))*px*dy
		 + (LONG)(*(grxy+ld->cols))*dx*py
		  + (LONG)(*(grxy+1+ld->cols))*px*py)>>16);
}
