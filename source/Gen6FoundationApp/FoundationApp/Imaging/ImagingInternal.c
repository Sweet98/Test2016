/************************************************************************/
//		This module performs Photo taking functions
//
// $RCSfile: Gen6FoundationApp/FoundationApp/Imaging/ImagingInternal.c $
// $Revision: 1.7 $
// $Date: 2010/11/17 15:42:18EST $
//======================================================================================

#include <math.h>
#include <stdlib.h>

#include "product.h"
#include "db_chrio.h"
#include "db_hdw.h"
#include "menu.h"
#include "scandrivermatrixinterface.h"
#include "HImage.h"
#include "beep.h"
#include "ioman.h"
#include "db_time.h"
#include "imaging.h"
#include "readerconfiguration.h"

void showSudoku(unsigned char *ImageBuffer, int wid, int hgt, char *BcMessage);

//********************************************************************************
#ifdef GEN6_DEVELOPMENT
//********************************************************************************

int ComputeImageScore(unsigned char *img, int wid, int hgt, int wrap, int skip)
{
   int energy, hEnergy=0, vEnergy=0;

	if (GET_SETTING(ImageQualityRanking) & 2)
	{
   	int i, j;
   	unsigned char* p;

   	int xStart = wid/6;  int xGrid = 2*wid/15;
   	int yStart = hgt/6; int yGrid = 2*hgt/15;

   	for(i=5; i>=0; i--)
		{
      	unsigned char a,b;
      	int yDelta = wrap*skip;
      	//horizontally
      	energy = 0;
      	p = img + (yStart+ yGrid*i)*wrap + xStart;
      	for (j=(wid*2/3)/skip; j>0; j--) {
         	a = *p; b = *(p+skip); if (a>b) energy += (int)(a-b); else energy += (int)(b-a); p+=skip;
      	}
      	if (energy > hEnergy) hEnergy = energy;
      	//vertically
      	energy = 0;
      	p = img + yStart*wrap + xStart + xGrid*i;
      	for(j=(hgt*2/3)/skip; j>0; j--) {
         	a = *p; b = *(p-yDelta); if (a>b) energy += (int)(a-b); else energy += (int)(b-a); p+=yDelta;
      	}
      	if (energy > vEnergy) vEnergy = energy;
   	}
	}
	else
	{
   	int i, j, tmp;
   	unsigned char* p;

   	int xStart = wid/6;  int xGrid = 2*wid/15;
   	int yStart = hgt/6; int yGrid = 2*hgt/15;

   	for(i=5; i>=0; i--)
		{
      	//horizontally
      	energy = 0;
      	p =  img + (yStart+ yGrid*i)*wid + xStart;
      	for(j=wid*2/3; j>0; j--) {
         	tmp =  (int)( *(p+1) - *p);
         	energy += abs(tmp);
         	p++;
      	}
      	if (energy > hEnergy) hEnergy = energy;

      	//vertically
      	energy = 0;
      	p =  img + yStart*wid + xStart + xGrid*i;
      	for(j=hgt*2/3; j>0; j--) {
         	tmp =  (int)(*(p) - *(p-wid));
         	energy +=  abs(tmp);   //tmp*tmp;
         	p += wid;
      	}
      	if (energy > vEnergy) vEnergy = energy;

      	//energy >>= 6;
      	//sum += energy;
   	}
	}
   energy = (vEnergy+hEnergy);
   if (energy < 5)  energy = 5;
   return energy;
}
//********************************************************************************
#endif			// end #if GEN6_DEVELOPMENT
//********************************************************************************


typedef struct
{
	S32 x;	// x-coordinate
	S32 y;	// y-coordinate
} Point;


static const int SincWgt[17][4] =
{
	{  0,256,  0,  0, }, { -8,255, 10, -1, }, {-14,249, 23, -2, }, {-17,238, 39, -4, },
	{-19,225, 56, -6, }, {-20,208, 76, -8, }, {-19,188, 98,-11, }, {-17,166,120,-13, },
	{-15,143,143,-15, }, {-13,120,166,-17, }, {-11, 98,188,-19, }, { -8, 76,208,-20, },
	{ -6, 56,225,-19, }, { -4, 39,238,-17, }, { -2, 23,249,-14, }, { -1, 10,255, -8, },
	{  0,  0,256,  0, }
};


int BiCubicInterpolation (unsigned char *GIMG, int X, int Y, int ZOOM, int Xsize, int Ysize)
{
	int i, j, *wgt, *w, sum, pxl[4], rx, ry, mask = (1<<ZOOM) - 1;
	unsigned char *c;
	rx = X & mask; ry = Y & mask; X >>= ZOOM; Y >>= ZOOM;
   if (X < 1) X = 1;
   if (Y < 1) Y = 1;
   if (X > Xsize-3) X = Xsize-3;
   if (Y > Ysize-3) Y = Ysize-3;
	i = ZOOM - 4; if (i > 0) { j = i >> 1; rx = (rx + j) >> i; ry = (ry + j) >> i; }
	 else if (i < 0) { rx <<= -i; ry <<= -i; }
	c = GIMG + (Y-1) * Xsize + X-1; wgt = (int*)SincWgt + (rx << 2);
	for (i=0; i<4; i++) {
		for (j=sum=0,w=wgt; j<4; j++) sum += (*(w++)) * ((int)*(c++));
		 pxl[i] = sum; c += Xsize - 4;
	}
	for (i=sum=0,w=pxl,wgt=(int*)SincWgt+(ry<<2); i<4; i++) sum += (*(w++)) * (*(wgt++));
   if (sum < 0) return 0;
   if (sum > (255<<16)) return 255;
	return (sum>>16);
}


int subpixel(unsigned char *Img, int Wrap, long lx, long ly)
{
	long px, py, dx, dy; unsigned char *grxy;
	grxy = Img + (lx>>8) + (ly>>8)*Wrap;
	 px = lx&0xff; dx = 256-px; py = ly&0xff; dy = 256-py;
	return (((long)(*grxy)*dx*dy
		+ (long)(*(grxy+1))*px*dy
		 + (long)(*(grxy+Wrap))*dx*py
		  + (long)(*(grxy+1+Wrap))*px*py)>>16);
}


void BiLinearInterpolation( unsigned char *SrcImg, unsigned char *DstImg,
                            int SrcWrap, int DstWidth, int DstHeight,
                            int ulx, int uly, int urx, int ury,
                            int lrx, int lry, int llx, int lly )
{
	int x, y, ix, iy;
	long x0, x1, y0, y1, xp, yp;
   for (y=0; y<DstHeight; y++) {
      // Here we set up the starting and ending points of the line that we are
      // looking at sampling for our new image
      x0 = (long)((y*(llx - ulx)<<8)/DstHeight + (ulx<<8));
      y0 = (long)((y*(lly - uly)<<8)/DstHeight + (uly<<8));
      x1 = (long)((y*(lrx - urx)<<8)/DstHeight + (urx<<8));
      y1 = (long)((y*(lry - ury)<<8)/DstHeight + (ury<<8));
      // Run through the line
      for (x=0; x<DstWidth; x++) {
         // Create pixel x,y coordinates down to sub pixel accuracy according
         // to how many rows and columns that there are.
         xp = ((long)x*(x1 - x0)/DstWidth) + x0; ix = xp >>8;
         yp = ((long)x*(y1 - y0)/DstWidth) + y0; iy = yp >>8;
         // Make sure the pixel is in range before we take it.
         if ((ix>=0)&&(ix<SrcWrap-1)&&(iy>=0)&&(iy<SrcWrap-1))
            *(DstImg++) = subpixel(SrcImg,SrcWrap,xp,yp);
         else
            *(DstImg++) = 64;
      }
   }
}


void Transmute(unsigned char *SrcImg, unsigned char *DstImg, int SrcX, int SrcY, int DstWidth, int DstHeight,
             int ulx, int uly, int urx, int ury, int lrx, int lry, int llx, int lly, int InterpolationType)
{
	int x, y, ix, iy;
	LONG x0, x1, y0, y1, xp, yp;
   for (y=0; y<DstHeight; y++) {
//      RESET_WATCHDOG_TIMER();
      // Here we set up the starting and ending points of the line that we are looking at sampling for our new image
      x0 = (LONG)((y*(llx - ulx)<<8)/DstHeight + (ulx<<8));
      y0 = (LONG)((y*(lly - uly)<<8)/DstHeight + (uly<<8));
      x1 = (LONG)((y*(lrx - urx)<<8)/DstHeight + (urx<<8));
      y1 = (LONG)((y*(lry - ury)<<8)/DstHeight + (ury<<8));
      // Run through the line
      for (x=0; x<DstWidth; x++) {
         // Create pixel x,y coordinates down to sub pixel accuracy according to how many rows and columns that there are.
         xp = ((LONG)x*(x1 - x0)/DstWidth) + x0; ix = xp >>8;
         yp = ((LONG)x*(y1 - y0)/DstWidth) + y0; iy = yp >>8;
         // Make sure the pixel is in range before we take it.
         if ((ix>=0)&&(ix<SrcX-1)&&(iy>=0)&&(iy<SrcY-1))
            if (InterpolationType == 0) *(DstImg++) = BiCubicInterpolation(SrcImg,xp>>4,yp>>4,4,SrcX, SrcY);
            else if (InterpolationType == 1) *(DstImg++) = subpixel(SrcImg,SrcX,xp,yp);
            else *(DstImg++) = *(SrcImg + iy * SrcX + ix);
         else *(DstImg++) = 64;
      }
   }
}


extern "C" void   putpixel(UINT x, UINT y, UCHAR color);
extern "C" char   BcbPause(void);
#ifdef BCB
#define Point point
#include "Bcb.h"
#else
#define SHOWWORK 0
#endif
typedef unsigned char MxPixel;

#define POINT(P,C) putpixel(P.x,P.y,C)
#define CIRCLE(P,R,C) { setcolor(C); circle(P.x,P.y,R); }
#define DRAWLINE(A,B,W,C) putline(A,B,W,C)
#define PAUSE BcbPause()

void xputstring (const unsigned char *string, int StringLength);
#include <string.h>
#define PRINTF(a,b) { char s[20]; sprintf(s,a,b); xputstring((const unsigned char*)s,strlen(s)); }
#define PINT(i) PRINTF(" %4d",i)
#define PCHR(d) PRINTF("%c",d)
#define PSPC xputstring((const unsigned char*)" ",1)
#define PHEX(h) PRINTF(" %04x",h)
#define PSTR(a) xputstring((const unsigned char*)a,strlen(a))
#define PPNT(p) PRINTF(" (%d,",(int)p.x); PRINTF("%d)",(int)p.y)
#define PPTR(p) PRINTF(" %p",p)
#define CRLF xputstring((const unsigned char*)"\r\n",2)

// Some basic utility functions
static int Larger (int a, int b) { if (a > b) return (a); else return (b); }
static int Smaller (int a, int b) { if (a < b) return (a); else return (b); }


// Transform() performs a rotational transformation !!
static Point Transform (Point p, int dx, int dy)
{
   float c = sqrt((float)(dx*dx + dy*dy)); Point q;
   q.x = (int)((float)(dx * p.x + dy * p.y) / c); q.y = (int)((float)(dx * p.y - dy * p.x) / c); return (q);
}

//*****************************************************************************************************

#define UCHAR unsigned char
#define U32 unsigned long
typedef struct { Point A, B; int dir; } line;

// Here's a "workspace" structure
typedef struct
{
   UCHAR *img; int topx, topy, wrap;
   U32 *bimg, *binPtr, *mimg, *mskPtr, Mask; Point binPnt; char *binDone; int binDir, bcols, brows;
   UCHAR *timg, *eimg; int ttopx, ttopy;
   line *Lin; int nLin;
} Workspace;


#define IMG ws->img
#define TOPX ws->topx
#define TOPY ws->topy
#define WRAP ws->wrap
#define BTOP(a) ((a+31)>>5)
#define BIMG ws->bimg
#define BPTR ws->binPtr
#define MIMG ws->mimg
#define MPTR ws->mskPtr
#define MASK ws->Mask
#define BPNT ws->binPnt
#define BDONE ws->binDone
#define BDIR ws->binDir
#define BROWS ws->brows
#define BCOLS ws->bcols
#define TTOP(a) (((a+15)>>4)+1)
#define TTOPX ws->ttopx
#define TTOPY ws->ttopy
#define TIMG ws->timg
#define EIMG ws->eimg
#define LIN ws->Lin
#define NLIN ws->nLin
#define NLINES 256


static Workspace* EstablishWorkspace (int imgwid, int imghgt)
{
   Workspace *ws = (Workspace*)malloc(sizeof(Workspace));
   if (ws) {
      TOPX = imgwid; TOPY = imghgt; BROWS = BTOP(imghgt); BCOLS = BTOP(imgwid);
      int siz = BCOLS * imghgt; BIMG = (unsigned long*)malloc(sizeof(unsigned long) * siz << 1);
      if (BIMG) { MIMG = BIMG + siz; BDONE = (char*)malloc(sizeof(char) * BROWS * BCOLS);
         if (BDONE) { TTOPX = TTOP(imgwid); TTOPY = TTOP(imghgt);
            siz = TTOPX * TTOPY; TIMG = (UCHAR*)malloc(sizeof(UCHAR) * siz << 1);
            if (TIMG) { EIMG = TIMG + siz;
               LIN = (line*)malloc(sizeof(line)*NLINES);
               if (LIN) {
                  NLIN = 0; return (ws);
               } free(TIMG);
            } free(BDONE);
         } free (BIMG);
      } free(ws);
   } return (0);
}


static void FreeWorkspace (Workspace *ws)
{
   free(LIN); free(TIMG); free(BDONE); free(BIMG); free(ws);
}


#define HI_LO_SPREAD(spl,hi,lo)    if (spl > hi)hi = spl; else if (spl < lo)lo = spl
#define HI_LO_SUM_ODD(h,spl,sm,hi,lo) {                           \
   sm += spl = (h[0]&0x00ff0000)>>16;  HI_LO_SPREAD(spl,hi,lo);   \
   sm += spl = (h[0]&0x000000ff);      HI_LO_SPREAD(spl,hi,lo);   \
   sm += spl = (h[1]&0x00ff0000)>>16;  HI_LO_SPREAD(spl,hi,lo);   \
   sm += spl = (h[1]&0x000000ff);      HI_LO_SPREAD(spl,hi,lo);   \
   sm += spl = (h[2]&0x00ff0000)>>16;  HI_LO_SPREAD(spl,hi,lo);   \
   sm += spl = (h[2]&0x000000ff);      HI_LO_SPREAD(spl,hi,lo);   \
   sm += spl = (h[3]&0x00ff0000)>>16;  HI_LO_SPREAD(spl,hi,lo);   \
   sm += spl = (h[3]&0x000000ff);      HI_LO_SPREAD(spl,hi,lo);   }

#define HI_LO_SUM_EVEN(h,spl,sm,hi,lo) {                          \
   sm += spl = (h[0]&0xff000000)>>24;  HI_LO_SPREAD(spl,hi,lo);   \
   sm += spl = (h[0]&0x0000ff00)>>8;   HI_LO_SPREAD(spl,hi,lo);   \
   sm += spl = (h[1]&0xff000000)>>24;  HI_LO_SPREAD(spl,hi,lo);   \
   sm += spl = (h[1]&0x0000ff00)>>8;   HI_LO_SPREAD(spl,hi,lo);   \
   sm += spl = (h[2]&0xff000000)>>24;  HI_LO_SPREAD(spl,hi,lo);   \
   sm += spl = (h[2]&0x0000ff00)>>8;   HI_LO_SPREAD(spl,hi,lo);   \
   sm += spl = (h[3]&0xff000000)>>24;  HI_LO_SPREAD(spl,hi,lo);   \
   sm += spl = (h[3]&0x0000ff00)>>8;   HI_LO_SPREAD(spl,hi,lo);   }


static char SampleImage (Workspace *ws)
{
   int x, y, hi, lo, sum, sample, *LO, *HI, *SUM, *EG, *tlo, *thi, *tsum, *tdes;
   int *loStart, *hiStart, *sumStart, delta;
   int ttopx = (TOPX+7)>>4, ttopy = (TOPY+7)>>4, tskip = TTOPX-ttopx+1;
   UCHAR *gRow;
   unsigned int *pxl0, *pxl1, *pxl2, *pxl3, *pxl4, *pxl5, *pxl6, *pxl7;
   unsigned int *pxl8, *pxl9, *pxlA, *pxlB, *pxlC, *pxlD, *pxlE, *pxlF;
   unsigned int nHold[4];

   int siz = TTOPX * TTOPY;
   LO = (int*)malloc(sizeof(int) * siz << 2); if (!LO) return FALSE;
   HI = LO + siz; SUM = HI + siz; EG = SUM + siz;

//PSTR(" Sample!"); CRLF;
// Compute the Minima & Maxima around each Threshold point, Staying comfortably INSIDE the Image's edge...
   tlo = LO + TTOPX + 1; thi = HI + TTOPX + 1; tsum = SUM + TTOPX + 1; gRow = ws->img + ((WRAP +1) << 4); // Skip the top/left rims
   for (y=ttopy-1; y>0; y--) {
      pxl0 = (unsigned int *)(gRow - 8*WRAP - 8); pxl1 = (unsigned int *)(gRow - 7*WRAP - 8);   // NB: odd offsets make this "color proof"!
      pxl2 = (unsigned int *)(gRow - 6*WRAP - 8); pxl3 = (unsigned int *)(gRow - 5*WRAP - 8);
      pxl4 = (unsigned int *)(gRow - 4*WRAP - 8); pxl5 = (unsigned int *)(gRow - 3*WRAP - 8);
      pxl6 = (unsigned int *)(gRow - 2*WRAP - 8); pxl7 = (unsigned int *)(gRow - WRAP - 8);
      pxl8 = (unsigned int *)(gRow - 8);              pxl9 = (unsigned int *)(gRow + WRAP - 8);
      pxlA = (unsigned int *)(gRow + 2*WRAP - 8); pxlB = (unsigned int *)(gRow + 3*WRAP - 8);
      pxlC = (unsigned int *)(gRow + 4*WRAP - 8); pxlD = (unsigned int *)(gRow + 5*WRAP - 8);
      pxlE = (unsigned int *)(gRow + 6*WRAP - 8); pxlF = (unsigned int *)(gRow + 7*WRAP - 8);

      for (x=ttopx-1; x>0; x--) {
         nHold[0] = *pxl0++; nHold[1] = *pxl0++; nHold[2] = *pxl0++; nHold[3] = *pxl0++;
          lo = hi = nHold[0]&0x000000ff; sum = 0; HI_LO_SUM_EVEN(nHold,sample,sum,hi,lo);
         nHold[0] = *pxl1++; nHold[1] = *pxl1++; nHold[2] = *pxl1++; nHold[3] = *pxl1++; HI_LO_SUM_ODD(nHold,sample,sum,hi,lo);
         nHold[0] = *pxl2++; nHold[1] = *pxl2++; nHold[2] = *pxl2++; nHold[3] = *pxl2++; HI_LO_SUM_EVEN(nHold,sample,sum,hi,lo);
         nHold[0] = *pxl3++; nHold[1] = *pxl3++; nHold[2] = *pxl3++; nHold[3] = *pxl3++; HI_LO_SUM_ODD(nHold,sample,sum,hi,lo);
         nHold[0] = *pxl4++; nHold[1] = *pxl4++; nHold[2] = *pxl4++; nHold[3] = *pxl4++; HI_LO_SUM_EVEN(nHold,sample,sum,hi,lo);
         nHold[0] = *pxl5++; nHold[1] = *pxl5++; nHold[2] = *pxl5++; nHold[3] = *pxl5++; HI_LO_SUM_ODD(nHold,sample,sum,hi,lo);
         nHold[0] = *pxl6++; nHold[1] = *pxl6++; nHold[2] = *pxl6++; nHold[3] = *pxl6++; HI_LO_SUM_EVEN(nHold,sample,sum,hi,lo);
         nHold[0] = *pxl7++; nHold[1] = *pxl7++; nHold[2] = *pxl7++; nHold[3] = *pxl7++; HI_LO_SUM_ODD(nHold,sample,sum,hi,lo);
         nHold[0] = *pxl8++; nHold[1] = *pxl8++; nHold[2] = *pxl8++; nHold[3] = *pxl8++; HI_LO_SUM_EVEN(nHold,sample,sum,hi,lo);
         nHold[0] = *pxl9++; nHold[1] = *pxl9++; nHold[2] = *pxl9++; nHold[3] = *pxl9++; HI_LO_SUM_ODD(nHold,sample,sum,hi,lo);
         nHold[0] = *pxlA++; nHold[1] = *pxlA++; nHold[2] = *pxlA++; nHold[3] = *pxlA++; HI_LO_SUM_EVEN(nHold,sample,sum,hi,lo);
         nHold[0] = *pxlB++; nHold[1] = *pxlB++; nHold[2] = *pxlB++; nHold[3] = *pxlB++; HI_LO_SUM_ODD(nHold,sample,sum,hi,lo);
         nHold[0] = *pxlC++; nHold[1] = *pxlC++; nHold[2] = *pxlC++; nHold[3] = *pxlC++; HI_LO_SUM_EVEN(nHold,sample,sum,hi,lo);
         nHold[0] = *pxlD++; nHold[1] = *pxlD++; nHold[2] = *pxlD++; nHold[3] = *pxlD++; HI_LO_SUM_ODD(nHold,sample,sum,hi,lo);
         nHold[0] = *pxlE++; nHold[1] = *pxlE++; nHold[2] = *pxlE++; nHold[3] = *pxlE++; HI_LO_SUM_EVEN(nHold,sample,sum,hi,lo);
         nHold[0] = *pxlF++; nHold[1] = *pxlF++; nHold[2] = *pxlF++; nHold[3] = *pxlF++; HI_LO_SUM_ODD(nHold,sample,sum,hi,lo);
         *(tlo++) = (UCHAR)lo; *(thi++) = (UCHAR)hi; *(tsum++) = (sum + 4)>>3;
      }
      tlo += tskip; thi += tskip; tsum += tskip; // ... & skip the bottom/right rims by two!
      gRow += WRAP << 4;
   }

// & then load proper values along the Rims!
   for (y=1,tlo=LO+TTOPX,thi=HI+TTOPX,tsum=SUM+TTOPX; y<TTOPY-1; y++,tlo+=TTOPX,thi+=TTOPX,tsum+=TTOPX) {
      *tlo = *(tlo+1); *thi = *(thi+1); *tsum = *(tsum+1);
   }
   for (y--,tlo--,thi--,tsum--; y; y--,tlo-=TTOPX,thi-=TTOPX,tsum-=TTOPX) {
      if (ttopx < TTOPX-1) { *(tlo-1) = *(tlo-2); *(thi-1) = *(thi-2); *(tsum-1) = *(tsum-2); }
      *tlo = *(tlo-1); *thi = *(thi-1); *tsum = *(tsum-1);
   }
   for (x=0,tlo=LO,thi=HI,tsum=SUM; x<TTOPX; x++,tlo++,thi++,tsum++) {
      *tlo = *(tlo+TTOPX); *thi = *(thi+TTOPX); *tsum = *(tsum+TTOPX);
   }
   y = TTOPX * (TTOPY-2);
   for (x=0,tlo=LO+y,thi=HI+y,tsum=SUM+y; x<TTOPX; x++,tlo++,thi++,tsum++) {
      if (ttopy < TTOPY-1) { *tlo = *(tlo-TTOPX); *thi = *(thi-TTOPX); *tsum = *(tsum-TTOPX); }
      *(tlo+TTOPX) = *tlo ; *(thi+TTOPX) = *thi; *(tsum+TTOPX) = *tsum;
   }
// Now HI, LO, & SUM are all filled!
//for (y=0,thi=HI; y<TTOPY; y++) { for (x=0; x<TTOPX; x++,thi++) PINT(*thi); CRLF; } CRLF;
//for (y=0,thi=LO; y<TTOPY; y++) { for (x=0; x<TTOPX; x++,thi++) PINT(*thi); CRLF; } CRLF;
//for (y=0,thi=SUM; y<TTOPY; y++) { for (x=0; x<TTOPX; x++,thi++) PINT(*thi); CRLF; } CRLF;

// Load up EG, finding the Highest value
   for (y=TTOPY,thi=HI,tlo=LO,tdes=EG,hi=*HI-*LO; y; y--) {
      for (x=TTOPX; x; x--,tdes++) { *tdes = *(thi++) - *(tlo++); if (*tdes > hi) hi = *tdes; }
   } hi >>= 2;
//for (y=0,thi=EG; y<TTOPY; y++) { for (x=0; x<TTOPX; x++,thi++) PINT(*thi); CRLF; } CRLF;
// & then fill EIMG
   for (y=TTOPY,thi=EG,gRow=EIMG; y; y--) { for (x=TTOPX; x; x--) *(gRow++) = (*(thi++) > hi)? TRUE:FALSE; }
//for (y=0,gRow=EIMG; y<TTOPY; y++) { for (x=0; x<TTOPX; x++,gRow++) { PCHR(*gRow+'0'); PSPC; } CRLF; } CRLF; PAUSE;

// Lastly, Smooth the measured values & then fill the Threshold Array

#define DROOPFACTOR 4
// Smooth Row by Row
   loStart = LO; hiStart = HI; sumStart = SUM;
   for (y=0; y<TTOPY; y++) {
      tlo = loStart;
       thi = hiStart;
        tsum = sumStart;
         lo = *tlo;
          hi = *thi;
           sum = *tsum;
      for (x=1; x<TTOPX; x++) {
         delta = ((*thi++)-(*tlo++))>>DROOPFACTOR; // NB: droop rate is proportional to separation!
         lo += delta; if (lo < *tlo) *tlo = lo; else lo = *tlo;
         hi -= delta; if (hi > *thi) *thi = hi; else hi = *thi;
         tsum++; *tsum = sum = (sum + *tsum)>>1;
      }
      for (x-=2; x>=0; x--) {
         delta = ((*thi--)-(*tlo--))>>DROOPFACTOR;
         lo += delta; if (lo < *tlo) *tlo = lo; else lo = *tlo;
         hi -= delta; if (hi > *thi) *thi = hi; else hi = *thi;
         tsum--; *tsum = sum = (sum + *tsum)>>1;
      }
      loStart += TTOPX; hiStart += TTOPX; sumStart += TTOPX;
   }
// ... then Column by Column
   loStart = LO; hiStart = HI; sumStart = SUM;
   for (x=0; x<TTOPX; x++) {
      tlo = loStart; thi = hiStart; tsum = sumStart; lo = *tlo; hi = *thi; sum = *tsum;
      for (y=1; y<TTOPY; y++) {
         delta = ((*thi)-(*tlo))>>DROOPFACTOR; tlo += TTOPX; thi += TTOPX;
         lo += delta; if (lo < *tlo) *tlo = lo; else lo = *tlo;
         hi -= delta; if (hi > *thi) *thi = hi; else hi = *thi;
         tsum += TTOPX; *tsum = sum = (sum + *tsum)>>1;
      }
      for (y-=2; y>=0; y--) {
         delta = ((*thi)-(*tlo))>>DROOPFACTOR; tlo -= TTOPX; thi -= TTOPX;
         lo += delta; if (lo < *tlo) *tlo = lo; else lo = *tlo;
         hi -= delta; if (hi > *thi) *thi = hi; else hi = *thi;
         tsum -= TTOPX; *tsum = sum = (sum + *tsum)>>1;
      }
      loStart++; hiStart++; sumStart++;
   }


#define TOPFACTOR 32 // out of 48
// ... & store this "Average" in TSMOOTH
   for (y=siz,gRow=TIMG,thi=HI,tlo=LO,tsum=SUM; y; y--) {
      *(gRow++) = ((*thi++)*TOPFACTOR + (*tlo++)*(48-TOPFACTOR) + *(tsum++) + 32) >> 6;
   }
//for (y=0,gRow=TIMG; y<TTOPY; y++) { for (x=0; x<TTOPX; x++,gRow++) PINT(*gRow); CRLF; } CRLF;

   free(LO); return (TRUE);
}


// Point utilities
static Point pSum (Point a, Point b) { a.x += b.x; a.y += b.y; return a; }
static Point pDif (Point a, Point b) { a.x -= b.x; a.y -= b.y; return a; }
static Point pTimes (Point a, S32 b) { a.x *= b; a.y *= b; return a; }


static Point pDiv (Point a, S32 b)
{
   S32 bh = b >> 1;
   if (a.x >= 0) a.x += bh; else a.x -= bh; a.x /= b;
   if (a.y >= 0) a.y += bh; else a.y -= bh; a.y /= b;
   return (a);
}


static Point pRotate (Point a, S32 b) { Point c; c.x = -b*a.y; c.y = b*a.x; return c; }
// pTrim returns a Point the direction of dir & the length of len, assuming(!) both are close in direction


#define pNeg(a) pTimes(a,-1)
#define pAvg(a,b) pDiv(pSum(a,b),2)
#define pFrac(p,mul,div) pDiv(pTimes(p,mul),div)
#define pEQ(a,b) ((a.x == b.x)&&(a.y == b.y))
#define pNEQ(a,b) ((a.x != b.x)||(a.y != b.y))
#define pPLUS(p,a) (p).x += (a).x; (p).y += (a).y
#define pMINUS(p,a) (p).x -= (a).x; (p).y -= (a).y
#define pTIMES(p,a) (p).x *= (a); (p).y *= (a)
#define pSCALE(p,a) (p).x <<= (a); (p).y <<= (a)
#define pSHRINK(p,a) (p).x >>= (a); (p).y >>= (a)
#define pDIV(p,a) (p).x /= (a); (p).y /= (a)
#define pDIVR(p,a) (p).x = ((p).x+((a)>>1))/(a); (p).y = ((p).y+((a)>>1))/(a)

#define TWIX(a,b,c) ((a <= c)&&(c <= b))


static void PointSpread (Point *pnt1, Point *pnt2, S32 l, S32 m, S32 r)
{
	Point dif;
	dif.x = pnt2->x - pnt1->x; dif.y = pnt2->y - pnt1->y;
	if (m) {
		pnt1->x -= dif.x * l/m; pnt1->y -= dif.y * l/m;
		pnt2->x += dif.x * r/m; pnt2->y += dif.y * r/m;
	}
}


static BOOL InFrame (Workspace *ws, Point a, int margin)
{
   return ((TWIX(margin,(TOPX-1-margin),a.x))&&(TWIX(margin,(TOPY-1-margin),a.y)))? TRUE : FALSE;
}


static int CrawlEglEdge (Workspace *ws, UCHAR **e, Point *p, int *dir)
{
   int ntry = 4; (*dir)--;
   switch ((*dir)&0x3) {
loop: case 0: if ((p->y>1)&&(*((*e)-TTOPX))) { *e -= TTOPX; p->y--; break; } else (*dir)++; if (!(--ntry)) break; //Try North
      case 1: if ((p->x<TTOPX-2)&&(*((*e)+1))) { (*e)++; p->x++; break; } else (*dir)++; if (!(--ntry)) break; //Try East
      case 2: if ((p->y<TTOPY-2)&&(*((*e)+TTOPX))) { *e += TTOPX; p->y++; break; } else (*dir)++; if (!(--ntry)) break; //Try South
      case 3: if ((p->x>1)&&(*((*e)-1))) { (*e)--; p->x--; break; } else (*dir)++; if (--ntry) goto loop; break; //Try West
   } return (ntry);
}


static void EglOutLine (Workspace *ws, Point ptrue, Point pfalse)
{
   UCHAR *e = EIMG + ptrue.y * TTOPX + ptrue.x;

   if (!((*e)&4))
	{
      int dir;
      pMINUS(pfalse,ptrue);

      switch (3*pfalse.y+pfalse.x)
		{
         case 2: case -1:  dir = 0; break;
         case -3: case -4: dir = 1; break;
         case 1: case -2:  dir = 2; break;
         case 4: case 3: default:   dir = 3; break;
      }

		if (CrawlEglEdge(ws,&e,&ptrue,&dir))
		{
         Point *QP = (Point*)MALLOC(sizeof(Point)<<6);

			if (QP)
			{
            int *ss = (int*)MALLOC(sizeof(int)<<6);

				if (ss)
				{
               int i, xdir, d[4], sum, pix, lastpix[32], lastlen = 0, gotone = 0, peakpix, peaklen;
					UCHAR *x;

					peakpix = 0;
					peaklen = 0;

               memset(lastpix,0,sizeof(int)<<5);

               for (pix=sum=0; pix<4; pix++)
					{
                  CrawlEglEdge(ws,&e,&ptrue,&dir); *e |= 4; ss[pix] = sum += d[pix] = dir; QP[pix] = ptrue;
               }

					x = e; xdir = dir&3;

					for (i=0; i<32; i++) lastpix[i] = pix;

               do
					{
                  int lastsum = sum, len;
                  CrawlEglEdge(ws,&e,&ptrue,&dir); *e |= 4;
                  ss[pix&0x3f] = sum += dir - d[pix&3]; d[pix&3] = dir; lastpix[sum&0xf] = pix;
//CIRCLE(pTimes(ptrue,16),8,13); //PAUSE;
                  if (sum > lastsum+1) for (i=sum-1; i>lastsum; i--) lastpix[i&0xf] = pix;
                   else if (sum < lastsum-1) for (i=sum+1; i<lastsum; i++) lastpix[i&0xf] = pix;
//                len = pix - Larger(lastpix[(sum-2)&0xf],lastpix[(sum+2)&0xf]);
                  len = pix - Smaller(Larger(lastpix[(sum-2)&0xf],lastpix[(sum+1)&0xf]),Larger(lastpix[(sum-1)&0xf],lastpix[(sum+2)&0xf]));
//               len = pix - Smaller(Smaller(Larger(lastpix[(sum-3)&0xf],lastpix[(sum+1)&0xf]),
//                      Larger(lastpix[(sum-2)&0xf],lastpix[(sum+2)&0xf])),Larger(lastpix[(sum-1)&0xf],lastpix[(sum+3)&0xf]));
#define ECRIT 5
                  if ((lastlen > ECRIT)&&(len <= ECRIT)) {
//CIRCLE(pTimes(ptrue,16),13,13); //PAUSE;
                     if (!gotone) { x = e; xdir = dir&3; gotone = 1; } else {
                        int sum, history = Smaller(peaklen,52); Point min, max, d;
								for (i=peakpix-history,sum=0; i<peakpix; i++) sum += ss[i&0x3f];
                        sum = ((sum<<5)/history)&0x1ff; d.y = 128 - abs(sum-256);
                        sum = (sum+128)&0x1ff; d.x = 128 - abs(sum-256);
                        min = max = Transform(pTimes(QP[(peakpix-2)&0x3f],16),d.x,d.y);
                        for (i=peakpix-history-3; i<peakpix-1; i++) {
                           Point q = Transform(pTimes(QP[i&0x3f],16),d.x,d.y);
//CIRCLE(pTimes(QP[i&0x3f],16),4,15);
                           if (q.x < min.x) min.x = q.x; else if (q.x > max.x) max.x = q.x;
                           if (q.y < min.y) min.y = q.y; else if (q.y > max.y) max.y = q.y;
                        } max.y = min.y; //(min.y + max.y)>>1;
                        if (NLIN < NLINES) {
                           LIN[NLIN].B = Transform(max,d.x,-d.y);
                           LIN[NLIN].A = pDif(LIN[NLIN].B,pFrac(d,peaklen<<4,abs(d.x)+abs(d.y)));
									PointSpread(&LIN[NLIN].A,&LIN[NLIN].B,1,peaklen,1);
                           if (InFrame(ws,pAvg(LIN[NLIN].A,LIN[NLIN].B),24)) NLIN++;
#if (SHOWWORK)
DRAWLINE(LIN[NLIN-1].A,LIN[NLIN-1].B,3,15); //PAUSE;
#endif
                        } gotone = 2;
                     }
                  } else if (len > lastlen) { peaklen = len; peakpix = pix+1; }
                  lastlen = len; QP[(pix++)&0x3f] = ptrue;
               } while ((((e != x)||((dir&3) != xdir)))||(gotone == 1));

               FREE(ss);
            }
            FREE(QP);
         }
      }
   }
}


/***********************************************************************/
/***           ORDINARY (One-to-One) Binary Image Processing         ***/
/***********************************************************************/
// Key to DIR: 0 = North(-y), 1 = East(+x), 2 = South(+y), 3 = West(-x)

// "LRNORTH etc. check the state of adjacent LoRes pixels
#define LRNORTH (*(BPTR-1)&MASK)
#define LRSOUTH (*(BPTR+1)&MASK)
#define LRWEST ((MASK == 0x80000000)? *(BPTR-TOPY)&1 : *BPTR&(MASK<<1))
#define LREAST ((MASK == 1)? *(BPTR+TOPY)&0x80000000 : *BPTR&(MASK>>1))

#define LRNW ((MASK == 0x80000000)? *(BPTR-1-TOPY)&1 : *(BPTR-1)&(MASK<<1))
#define LRNE ((MASK == 1)? *(BPTR-1+TOPY)&0x80000000 : *(BPTR-1)&(MASK>>1))
#define LRSE ((MASK == 1)? *(BPTR+1+TOPY)&0x80000000 : *(BPTR+1)&(MASK>>1))
#define LRSW ((MASK == 0x80000000)? *(BPTR+1-TOPY)&1 : *(BPTR+1)&(MASK<<1))

// "GOLRWEST" etc. move to neighboring LoRes pixels
#define GOLRWESTA BPNT.x--; BPTR -= TOPY; MPTR -= TOPY; MASK = 1;
#define GOLRWESTB BPNT.x--; MASK <<= 1
#define GOLRWEST if (MASK==0x80000000) { GOLRWESTA; } else { GOLRWESTB; }
#define GOLRNORTH BPNT.y--; BPTR--; MPTR--
#define GOLREASTA BPNT.x++; BPTR += TOPY; MPTR += TOPY; MASK = 0x80000000;
#define GOLREASTB BPNT.x++; MASK >>= 1
#define GOLREAST if (MASK==1) { GOLREASTA; } else { GOLREASTB; }
#define GOLRSOUTH BPNT.y++; BPTR++; MPTR++


static void LoResBinarize (Workspace *ws, int X, int Y)          // ACTUAL(!) Image Coordinates
{
   X &= -32; Y &= -32;  // Find the Upper Left Corner coordinates
   if ((TWIX(0,TOPX-1,X)) && (TWIX(0,TOPY-1,Y))) { // Inside the Image?!
      int offset = ((U32)Y * BCOLS + X) >> 5;
      if (!(*(BDONE+offset))) {
         int thresh, target; UCHAR *th, *thLeft = TIMG + ((Y * TTOPX + X) >> 4); // Find the corresponding Threshold location,
         MxPixel *img = IMG + Y * WRAP + X;  // the corresponding Image location,
         U32 word, *bimg = BIMG + (X>>5) * TOPY + Y, *bimg2 = bimg;  // ...& the corresponding Binary Image location!

         int x, y, xhi, yhi, lx, rx, ty, by, dir, dif, sum;

         if ((X>>5) < BCOLS-1) xhi = 32; else xhi = ((TOPX-1) & 0x1f);
          if ((Y>>5) < BROWS-1) yhi = 32; else yhi = ((TOPY-1) & 0x1f);

         for (y=ty=0,by=16; y<yhi; y++,ty++,by--) {
            if (!by) { ty = 0; by = 16; thLeft += TTOPX; }
            th = thLeft; thresh = (by * (*th) + ty * (*(th+TTOPX)) + 8) >> 4;
            th++; target = (by * (*th) + ty * (*(th+TTOPX)) + 8) >> 4;
            if (target > thresh) { dir = 1; dif = target-thresh; } else { dir = -1; dif = thresh-target; }
            for (x=lx=word=0,rx=16,sum=8; x<xhi; x++,lx++,rx--) {
               if (!rx) {
                  lx = 0; rx = 16; th++; target = (by * (*th) + ty * (*(th+TTOPX)) + 8) >> 4;
                  if (target > thresh) { dir = 1; dif = target-thresh; } else { dir = -1; dif = thresh-target; }
               }
               word <<= 1; if (*(img++) > (MxPixel)thresh) word |= 1;
//putpixel(x+X,y+Y,(word&1)?9:3);
               sum += dif; while (sum > 15) { sum -= 16; thresh += dir; }
            } for (; x<32; x++) word <<= 1;
            *(bimg++) = word; img += (WRAP-xhi);
         }

         if (!Y) *bimg2 = 0;
          else if ((Y>>5) == BROWS-1) *bimg = 0;
         if (!X) {
            for (y=0; y<yhi; y++,bimg2++) *bimg2 &= 0x7fffffff;
         } else if ((X>>5) == BCOLS-1) {
            word = (S32)(0xffffffff << (32 - ((TOPX-1)&0x1f)));
            for (y=0; y<yhi; y++,bimg2++) *bimg2 &= word;
         }

         *(BDONE+offset) = TRUE;
      }
   }
}


/***********************************************************************/
// EdgeCheck (ws,P) checks if P is on a Cell boundary, Binarizing adjacent Cells if so
static void LoResEdgeCheck (Workspace *ws, Point P)
{
   int x = P.x & 0x1f, y = P.y & 0x1f;
   if (!x) { LoResBinarize(ws,P.x-1,P.y);
      if (!y) LoResBinarize(ws,P.x-1,P.y-1);
       else if (y == 0x1f) LoResBinarize(ws,P.x-1,P.y+1);
   } else if (x == 0x1f) { LoResBinarize(ws,P.x+1,P.y);
      if (!y) LoResBinarize(ws,P.x+1,P.y-1);
       else if (y == 0x1f) LoResBinarize(ws,P.x+1,P.y+1);
   }
   if (!y) LoResBinarize(ws,P.x,P.y-1);
    else if (y == 0x1f) LoResBinarize(ws,P.x,P.y+1);
}


/***********************************************************************/
static int LoResLeftAttach (Workspace *ws, Point Light, Point Dark)
{
   int tries = 4;
   BPNT = Light; LoResEdgeCheck(ws,BPNT);
   BPTR = BIMG + (Light.x>>5)*TOPY + Light.y;
   MPTR = MIMG + (Light.x>>5)*TOPY + Light.y; MASK = (S32)(0x80000000>>(Light.x&0x1f));

   Dark.x -= Light.x; Dark.y -= Light.y;
   switch (4 + 3*Dark.y + Dark.x) {
      case 0: /* NW */
      case 1: /* N */ goto TryEast;
      case 2: /* NE */
      case 5: /* E */ goto TrySouth;
      case 3: /* W */
      case 6: /* SW */ goto TryNorth;
      case 7: /* S */
      case 8: /* SE */ goto TryWest;
      default: return 0;
   }

TryNorth:
   if ((LRSOUTH)&&(!LRWEST||!LRSW)) { BDIR = 0; return 1; }
    if (--tries == 0) return 0;
TryEast:
   if ((LRWEST)&&(!LRNORTH||!LRNW)) { BDIR = 1; return 1; }
    if (--tries == 0) return 0;
TrySouth:
   if ((LRNORTH)&&(!LREAST||!LRNE)) { BDIR = 2; return 1; }
    if (--tries == 0) return 0;
TryWest:
   if ((LREAST)&&(!LRSOUTH||!LRSE)) { BDIR = 3; return 1; }
    if (--tries) goto TryNorth; return 0;
}


/***********************************************************************/
static void LoResLeftHug (Workspace *ws)
{
   int turns = -1;
   LoResEdgeCheck(ws,BPNT);
   switch (BDIR & 0x3) {
      case 0:  goto TryWest;
      case 1:  goto TryNorth;
      case 2:  goto TryEast;
      case 3:  goto TrySouth;
   }
TryNorth:
   if (LRNORTH) { GOLRNORTH; goto HugDun; }
   if ((++turns) == 3) goto HugDun;
TryEast:
   if (MASK == 1) {
      if (*(BPTR+TOPY)&0x80000000) { GOLREASTA; goto HugDun; }
   } else {
      if (*BPTR&(MASK>>1)) { GOLREASTB; goto HugDun; }
   }
   if ((++turns) == 3) goto HugDun;
TrySouth:
   if (LRSOUTH) { GOLRSOUTH; goto HugDun; }
   if ((++turns) == 3) goto HugDun;
TryWest:
   if (MASK == 0x80000000) {
      if (*(BPTR-TOPY)&1) { GOLRWESTA; goto HugDun; }
   } else {
      if (*BPTR&(MASK<<1)) { GOLRWESTB; goto HugDun; }
   }
   if ((++turns) < 3) goto TryNorth;
HugDun:
   BDIR += turns;
}


// LoResBit(ws,X,Y) samples the binarized image using (X,Y) values,
// returning "0" for a Light pixel & "1" for a Dark pixel
static int LoResBit (Workspace *ws, int X, int Y)
{
   if (TWIX(0,TOPX-1,X) && TWIX(0,TOPY-1,Y)) {
      LoResBinarize(ws,X,Y);
      if (BIMG[(U32)(X>>5)*TOPY+Y]&(0x80000000L>>(X&0x1f))) return 0; else return 1;
   } else return 7;  // ..if outside the Image
}


/* returns direction from "a" to "b", with North = 0 & East = "ninety"  */
static int Angle (Point a, Point b, int ninety)
{
   int A = -1; long dx, dy;
   dx = (long)(b.x - a.x); dy = (long)(b.y - a.y);
   if ((dx) || (dy)) {
      if (abs(dx) > abs(dy)) {
         if (dx > 0) /* Eastward */ A = (dx*ninety + dy*ninety/2 + dx/2)/dx;
          else /* Westward */ A = (dx*3*ninety + dy*ninety/2 + dx/2)/dx;
      } else {
         if (dy > 0) /* Southward */ A = (dy*2*ninety - dx*ninety/2 + dy/2)/dy;
          else /* Northward */ A = (dy*4*ninety - dx*ninety/2 + dy/2)/dy;
      }
   }
   if (A >= 4*ninety) A -= 4*ninety;
   return (A);
}


/***********************************************************************/
static void ImgOutLine (Workspace *ws, Point light, Point dark)
{
   MPTR = MIMG + (light.x>>5)*TOPY + light.y; MASK = (S32)(0x80000000>>(light.x&0x1f));
   if ((!(*MPTR & MASK))&&(LoResLeftAttach(ws,light,dark))) {
      Point *QP = (Point*)MALLOC(sizeof(Point)<<8); if (QP) {
         int *ss = (int*)MALLOC(sizeof(int)<<8); if (ss) {
            Point xp; int i, xDIR, sum, dir[16], pix, lastpix[64], lastlen = 0, gotone = 0;
//CIRCLE(light,10,13);
// Find an initial sum
            for (pix=sum=0; pix<16; pix++) {
               LoResLeftHug(ws); *MPTR |= MASK; ss[pix] = sum += dir[pix] = BDIR; QP[pix] = BPNT;
            } xp = BPNT; xDIR = BDIR&3; for (i=0; i<64; i++) lastpix[i] = pix;
// Hug further Leftward until the Outline closes on itself
            do {
               int lastsum = sum, len;
               LoResLeftHug(ws); *MPTR |= MASK; ss[pix&0xff] = sum += BDIR-dir[pix&0xf]; dir[pix&0xf] = BDIR;
//POINT(BPNT,14);
               lastpix[sum&0x3f] = pix;
                if (sum > lastsum+1) for (i=sum-1; i>lastsum; i--) lastpix[i&0x3f] = pix;
                 else if (sum < lastsum-1) for (i=sum+1; i<lastsum; i++) lastpix[i&0x3f] = pix;
//             len = pix - Larger(lastpix[(sum-2)&0x3f],lastpix[(sum+2)&0x3f]);
//             len = pix - Smaller(Larger(lastpix[(sum-2)&0x3f],lastpix[(sum+1)&0x3f]),Larger(lastpix[(sum-1)&0x3f],lastpix[(sum+2)&0x3f]));
               len = pix - Smaller(Smaller(Larger(lastpix[(sum-3)&0x3f],lastpix[(sum+1)&0x3f]),
                     Larger(lastpix[(sum-2)&0x3f],lastpix[(sum+2)&0x3f])),Larger(lastpix[(sum-1)&0x3f],lastpix[(sum+3)&0x3f]));
#define ICRIT 32
               if ((lastlen > ICRIT)&&(len <= ICRIT)) {
                  if (!gotone) { xp = BPNT; xDIR = BDIR&3; gotone = 1; } else {
                     int sum, history = Smaller(lastlen,246); Point min, max, d;
                     for (i=pix-history,sum=0; i<pix; i++) sum += ss[i&0xff];
                     sum = ((sum<<4)/history)&0x3ff; d.y = 256 - abs(sum-512);
                     sum = (sum+256)&0x3ff; d.x = 256 - abs(sum-512);
                     min = max = Transform(QP[(pix-2)&0xff],d.x,d.y);
                     for (i=pix-history-7; i<pix-1; i++) {
                        Point q = Transform(QP[i&0xff],d.x,d.y);
                        if (q.x < min.x) min.x = q.x; else if (q.x > max.x) max.x = q.x;
                        if (q.y < min.y) min.y = q.y; else if (q.y > max.y) max.y = q.y;
                     } max.y = (min.y + max.y)>>1;
                     if (NLIN < NLINES) {
                        LIN[NLIN].B = Transform(max,d.x,-d.y);
                        LIN[NLIN].A = pDif(LIN[NLIN].B,pFrac(d,lastlen+5,abs(d.x)+abs(d.y)));
                        if (InFrame(ws,pAvg(LIN[NLIN].A,LIN[NLIN].B),20)) { NLIN++;
#if (SHOWWORK)
DRAWLINE(LIN[NLIN-1].A,LIN[NLIN-1].B,3,13); //PAUSE;
#endif
                        }
                     } gotone = 2;
                  }
               }
               lastlen = len; QP[(pix++)&0xff] = BPNT;
            } while (((pNEQ(xp,BPNT)||(xDIR!=(BDIR&3))))||(gotone == 1));
            FREE(ss);
         }
         FREE(QP);
      }
   }
}


/***********************************************************************/
// Some Rail routines from Mx2dUtil.h & .c	//
// a "Rail" is a structure with Position and a Direction along which it steps in a straight line
typedef struct
{
	Point pnt;
	S32 sum;
	Point dif, sgn;
	int hori;
} Rail;


// "SetRail()" establishes a Rail from A to B starting in position A
static int SetRail (Rail *r, Point A, Point B)
{
	if (B.x >= A.x) { r->dif.x = B.x - A.x; r->sgn.x = 1; } else { r->dif.x = A.x - B.x; r->sgn.x = -1; }
	if (B.y >= A.y) { r->dif.y = B.y - A.y; r->sgn.y = 1; } else { r->dif.y = A.y - B.y; r->sgn.y = -1; }
	r->hori = (r->dif.x >= r->dif.y)? TRUE : FALSE;
	r->pnt = A; r->sum = (r->hori)? r->dif.x>>1 : r->dif.y>>1;
	if ((r->dif.x == 0)&&(r->dif.y == 0)) r->dif.x = r->dif.y = 1;  // Protecting / Indicating when point A == point B
	return (Larger(r->dif.x,r->dif.y));
}


// "Ahead(r)" & "Back(r)" move one position along Rail "r"
static void Ahead (Rail *r)
{
	if (r->hori) {
		r->pnt.x += r->sgn.x; r->sum += r->dif.y;
		if (r->sum >= r->dif.x) { r->sum -= r->dif.x; r->pnt.y += r->sgn.y; }
	} else {
		r->pnt.y += r->sgn.y; r->sum += r->dif.x;
		if (r->sum >= r->dif.y) { r->sum -= r->dif.y; r->pnt.x += r->sgn.x; }
	}
}


static void Back (Rail *r)
{
	if (r->hori) {
		r->pnt.x -= r->sgn.x; r->sum -= r->dif.y;
		if (r->sum < 0) { r->sum += r->dif.x; r->pnt.y -= r->sgn.y; }
	} else {
		r->pnt.y -= r->sgn.y; r->sum -= r->dif.x;
		if (r->sum <= 0) { r->sum += r->dif.y; r->pnt.x -= r->sgn.x; }
	}
}


#define DISQ(x,y) ((S32)(x)*(x)+(S32)(y)*(y))
#define PDSQ(a,b) (DISQ(a.x-b.x,a.y-b.y))
#define DISTANCE(a,b) ((int)sqrt((double)PDSQ(a,b)))


static bool ClosestPnt (Rail *r, Point a, Point b, Point p)
{
	Point ref = r->pnt; int d = PDSQ(r->pnt,p), dlast = d;
	while ((pNEQ(r->pnt,a))&&(d <= dlast)) { dlast = d; Back(r); d = PDSQ(r->pnt,p); } if (d > dlast) { Ahead(r); d = dlast; }
	while ((pNEQ(r->pnt,b))&&(d <= dlast)) { dlast = d; Ahead(r); d = PDSQ(r->pnt,p); } if (d > dlast) Back(r);
	return (pEQ(r->pnt,ref)) ? false : true;
}


static int NearestPnts (Point a, Point b, Point A, Point B)
{
	Rail r, R; bool q, Q;
//DRAWLINE(a,b,3,13); DRAWLINE(A,B,3,14);
//CIRCLE(a,7,14); CIRCLE(b,5,14); CIRCLE(A,7,15); CIRCLE(B,5,15); //PAUSE;
	if (SetRail(&r,a,b) && SetRail(&R,A,B)) {
		do { q = ClosestPnt(&r,a,b,R.pnt); Q = ClosestPnt(&R,A,B,r.pnt); } while (q || Q);
	}
//CIRCLE(r.pnt,15,13); CIRCLE(R.pnt,15,13); //PINT(DISTANCE(r.pnt,R.pnt)); PAUSE; CRLF;
	return (DISTANCE(r.pnt,R.pnt));
}


/***********************************************************************/
// Here's the Auto-Cropping function itself
void DoAutoCrop (UCHAR *pImg, int wid, int hgt, int wrap, Point *ul, Point *ur, Point *ll, Point *lr)
{
   Workspace *ws = EstablishWorkspace(wid,hgt);
   if (ws) { ws->img = pImg; WRAP = wrap;
      if (SampleImage(ws))
		{

#define HSLICE 14
#define VSLICE 20	// must be >= HSLICE !
			//int i, j, n, V[VSLICE], *v, hist[64]; UCHAR *E[VSLICE], **e; Point P[VSLICE], *p, q, dir, min, max, c[4];

         int 	i = 0, j = 0, n = 0;
			int	V[VSLICE], *v, hist[64];
			UCHAR	*E[VSLICE], **e;
			Point P[VSLICE], *p, c[4];
			Point	q, dir, min, max;

			min.x = 0;
			min.y = 0;
			max.x = 0;
			max.y = 0;

//UCHAR *z = EIMG; Point qq; for (qq.y=0; qq.y<TTOPY; qq.y++) for (qq.x=0; qq.x<TTOPX; qq.x++,z++) { CIRCLE(pTimes(qq,16),3,(*z)?13:14); } PAUSE;

// Vertical E-Slices
         for (i=1,p=P,e=E; i<VSLICE+1; i++,p++,e++) { p->x = i * TTOPX / (VSLICE+1); p->y = 0; *e = EIMG + p->x; }
         for (n=TTOPY-1; n; n--) {
            for (i=0,p=P,e=E; i<VSLICE; i++,p++,e++) {
               UCHAR last = **e; Point plast = *p; *e += TTOPX; p->y++; //CIRCLE(pTimes(*p,16),5,15);
               if (!last) { if (**e) EglOutLine(ws,*p,plast); } else { if (!(**e)) EglOutLine(ws,plast,*p); }
            }
         }
// Horizontal E-Slices
         for (i=1,p=P,e=E; i<HSLICE+1; i++,p++,e++) { p->x = 0; p->y = i * TTOPY / (HSLICE+1); *e = EIMG + p->y * TTOPX; }
         for (n=TTOPX-1; n; n--) {
            for (i=0,p=P,e=E; i<HSLICE; i++,p++,e++) {
               UCHAR last = **e; Point plast = *p; (*e)++; p->x++; //CIRCLE(pTimes(*p,16),10,13);
               if (!last) { if (**e) EglOutLine(ws,*p,plast); } else { if (!(**e)) EglOutLine(ws,plast,*p); }
            }
         }
//PINT(NLIN); PAUSE;

         memset(MIMG,0,sizeof(U32)*BCOLS*TOPY); // Clear out the Mask Array
         memset(BDONE,FALSE,sizeof(char)*BCOLS*BROWS);

//{ Point p; for (p.y=0; p.y<TOPY; p.y++) for (p.x=0; p.x<TOPX; p.x++) { int v = (LoResBit(ws,p.x,p.y))? 2:10; POINT(p,v); } } PAUSE;

// Vertical I-Slices
         for (i=1,p=P,v=V; i<VSLICE+1; i++,p++,v++) { p->x = i * TOPX / (VSLICE+1); p->y = 0; *v = LoResBit(ws,p->x,p->y); }
         for (n=TOPY-1; n; n--) {
            for (i=0,p=P,v=V; i<VSLICE; i++,p++,v++) {
               UCHAR last = *v; Point plast = *p; (p->y)++; *v = LoResBit(ws,p->x,p->y); //POINT((*p),15);
               if (*v) { if (!last) ImgOutLine(ws,plast,*p); } else { if (last) ImgOutLine(ws,*p,plast); }
            }
         }
// Horizontal I-Slices
         for (i=1,p=P,v=V; i<HSLICE+1; i++,p++,v++) { p->x = 0; p->y =  i * TOPY / (HSLICE+1); *v = LoResBit(ws,p->x,p->y); }
         for (n=TOPX-1; n; n--) {
            for (i=0,p=P,v=V; i<HSLICE; i++,p++,v++) {
               UCHAR last = *v; Point plast = *p; (p->x)++; *v = LoResBit(ws,p->x,p->y); //POINT((*p),15);
               if (*v) { if (!last) ImgOutLine(ws,plast,*p); } else { if (last) ImgOutLine(ws,*p,plast); }
            }
         }
//PINT(NLIN); PAUSE;

			ul->x = lr->x = 0; ur->x = ll->x = TOPX-1; ul->y = ur->y = 0; lr->y = ll->y = TOPY-1;
			if (NLIN > 1)
			{
				int k, group, size, groupmax, sizemax, lineGroup[NLINES]; bool done;

				groupmax = 0;

//				int pllProx = (TOPX+TOPY)>>3, nmlProx = pllProx>>3;... needs DPI indication !!
#define pllProx 160		// assumes about 120 DPI
#define nmlProx 20
				memset(lineGroup,0,sizeof(int)*NLINES);

				for (i=0; i<NLIN; i++) LIN[i].dir = Angle(LIN[i].A,LIN[i].B,32);	// calculate line "dir"ections to 128 points!

//ClearDebugPane();
				for (i=sizemax=group=0; i<NLIN; i++) if (!lineGroup[i]) { group++; lineGroup[i] = group+1; size = 0;
//PINT(group); PCHR(':');
					do { done = true;
						for (j=0; j<NLIN; j++) if (lineGroup[j] == group+1) { size++; lineGroup[j]--;  //PINT(j);
#if (SHOWWORK)
 DRAWLINE(LIN[j].A,LIN[j].B,3,12+group);
#endif
							for (k=0; k<NLIN; k++) if (!lineGroup[k]) {
								int angle = (128+16+LIN[j].dir-LIN[k].dir)&63;
								if (((TWIX(12,20,angle))&&(NearestPnts(LIN[j].A,LIN[j].B,LIN[k].A,LIN[k].B) < pllProx)) // about parallel
										||((TWIX(44,52,angle))&&(NearestPnts(LIN[j].A,LIN[j].B,LIN[k].A,LIN[k].B) < nmlProx))) { // about normal
									lineGroup[k] = group+1; done = false;
								}
							}
						}
					} while (!done);
					if (size > sizemax) { sizemax = size; groupmax = group; } //PSTR("  size:");  PINT(size); CRLF;
				}
//PSTR("# Groups:"); PINT(group); CRLF;
//PSTR("Largest Group:"); PINT(groupmax); PSTR("  has"); PINT(sizemax); PSTR("  members"); CRLF; //PAUSE;

				if ((sizemax<<2) > NLIN) {	// is this Largest Group at least 1/4 all the lines?
					memset(hist,0,sizeof(int)<<6);
   		      for (i=0; i<NLIN; i++) if (lineGroup[i] == groupmax) {
						int a = LIN[i].dir;
         		   hist[a&0x3f] += 3; hist[(a-1)&0x3f] += 2; hist[(a+1)&0x3f] += 2; hist[(a-2)&0x3f]++; hist[(a+2)&0x3f]++;
        			}
//for (i=0; i<64; i++) PINT(hist[i]); CRLF; PAUSE;
		         for (i=j=0; i<64; i++) if (hist[i] > j) { j = hist[i]; n = i; } //PINT(n);
// "n" is the most prominent angle
   		      n += 16; // now at a 45 degree angle!
      		   for (i=dir.x=dir.y=0; i<NLIN; i++) if (lineGroup[i] == groupmax) {
         		   j = LIN[i].dir - n;
            		q = pDif(LIN[i].B,LIN[i].A);
               	if (!(j & 0x20)) q = pRotate(q,1);
		            if (!(j & 0x40)) q = pNeg(q);
   		           pPLUS(dir,q); LIN[i].dir = TRUE;
      	   	}
	      	   for (i=j=0; i<NLIN; i++) if (lineGroup[i] == groupmax) {
     	      	   q = Transform(LIN[i].A,dir.x,dir.y); if (!j) { min = max = q; j++; } else {
        	      	   if (q.x < min.x) min.x = q.x; else if (q.x > max.x) max.x = q.x;
           	      	if (q.y < min.y) min.y = q.y; else if (q.y > max.y) max.y = q.y;
	              	}
   	  	   		q = Transform(LIN[i].B,dir.x,dir.y);
      	  	      if (q.x < min.x) min.x = q.x; else if (q.x > max.x) max.x = q.x;
        		      if (q.y < min.y) min.y = q.y; else if (q.y > max.y) max.y = q.y;
  	         	}
	         	c[0] = c[1] = min; c[2] = c[3] = max; c[1].x = max.x; c[3].x = min.x;


					if ((pNEQ(c[0],c[1]))&&(pNEQ(c[1],c[2]))) {

   		      	for (i=0; i<4; i++) c[i] = Transform(c[i],dir.x,-dir.y);

//	for (i=0; i<2; i++) PointSpread(c+i,c+3-i,1,14,1); for (i=0; i<4; i+=2) PointSpread(c+i,c+i+1,1,14,1);	// Adding some cushion!

#if (SHOWWORK)
for (i=0; i<4; i++) { CIRCLE(c[i],13,14); DRAWLINE(c[i],c[(i+1)&3],1,14); } PAUSE;
#endif

      		   	*ul = c[0]; *ur = c[1]; *lr = c[3]; *ll = c[2];  //... ahem!

					}
				}
			}

      } FreeWorkspace(ws);
   }
}


void FixRowNoise(unsigned char *img, int xsize, int ysize, int rot, int TopRatio, int BottomRatio);


bool AutoCropImage(void)
{
   Point ul,ur,ll,lr;
   int ncols,nrows;
   int a,b,c,d,e;
   bool ReturnStatus = false;
   unsigned char *subimg, *newimg;
   DWORD dwHeight, dwWidth;

#if PLATFORM_CURRENT == mxl
	if( !IsImagingAllowed() )	return true;   //We will NAK menu command in the exit function 
#endif

	if (P_SCANBUFFER == NULL)	return(ReturnStatus); // Trigger has not been pulled yet, no image to crop.

	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&dwHeight,4,NULL);
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&dwWidth,4,NULL);
   if (dwWidth == 1280 && dwHeight == 960) {
      newimg = (unsigned char *) malloc(dwWidth*dwHeight);
      memcpy(newimg, P_SCANBUFFER, dwWidth*dwHeight);
      FixRowNoise(newimg, dwWidth, dwHeight, 0, GET_SETTING(RowNoiseTopRatio), GET_SETTING(RowNoiseBottomRatio));
   } else newimg = P_SCANBUFFER;
   if (newimg) {
      DoAutoCrop(newimg, (int)dwWidth, (int)dwHeight, (int)dwWidth, &ul, &ur, &lr, &ll);
      ncols = (int)sqrt((double)((ur.x-ul.x)*(ur.x-ul.x)+(ur.y-ul.y)*(ur.y-ul.y)));
      nrows = (int)sqrt((double)((ll.x-ul.x)*(ll.x-ul.x)+(ll.y-ul.y)*(ll.y-ul.y)));
//char rr[128];
//sprintf(rr,"\r\n %d,%d %d,%d %d,%d %d,%d   %d %d \r\n",ul.x,ul.y,ur.x,ur.y,lr.x,lr.y,ll.x,ll.y,ncols,nrows);
//xputstring(rr);
      if (nrows <= 50 || ncols <= 50 || (nrows >15*dwHeight/16 && ncols > 15*dwWidth/16)) {
         if (dwWidth == 1280 && dwHeight == 960) free(newimg);
         ReturnStatus = ProcessImageShipStyleCommand(P_SCANBUFFER, (int)dwWidth, (int)dwHeight, false);
       } else {
         subimg = (unsigned char *) malloc(ncols*nrows);
         if (subimg) {
            a = ReaderConfiguration->TransmitImageWindowLeft;
            b = ReaderConfiguration->TransmitImageWindowRight;
            c = ReaderConfiguration->TransmitImageWindowTop;
            d = ReaderConfiguration->TransmitImageWindowBottom;
            e = ReaderConfiguration->RowNoise;
            ReaderConfiguration->RowNoise = 0;
            ReaderConfiguration->TransmitImageWindowLeft = 0;
            ReaderConfiguration->TransmitImageWindowRight = ncols-1;
            ReaderConfiguration->TransmitImageWindowTop = 0;
            ReaderConfiguration->TransmitImageWindowBottom = nrows-1;
            Transmute(newimg, subimg, (int)dwWidth, (int)dwHeight, ncols, nrows, ul.x, ul.y, ur.x, ur.y, lr.x, lr.y, ll.x, ll.y, 0);
            if (dwWidth == 1280 && dwHeight == 960) free(newimg);
            ReturnStatus = ProcessImageShipStyleCommand(subimg, ncols, nrows, true);
            ReaderConfiguration->TransmitImageWindowLeft = a;
            ReaderConfiguration->TransmitImageWindowRight = b;
            ReaderConfiguration->TransmitImageWindowTop = c;
            ReaderConfiguration->TransmitImageWindowBottom = d;
            ReaderConfiguration->RowNoise = e;
            free(subimg);
         }
      }
   }
   return ReturnStatus;
}


//********************************************************************************
#if 1
//********************************************************************************


// SubDatum - This function performs bicubic interpolation

static int SubDatum (unsigned char *GIMG, int Wrap, LONG X, LONG Y)
{
	int i, j, *wgt, *w, sum, pxl[4], rx, ry;
	unsigned char *c;
	rx = X & 0x0f; ry = Y & 0x0f; X >>= 4; Y >>= 4;
   c = GIMG + (Y-1) * Wrap + X-1; wgt = (int*)SincWgt + (rx << 2);
	for (i=0; i<4; i++) {
		for (j=sum=0,w=wgt; j<4; j++) sum += (*(w++)) * ((int)*(c++));
      pxl[i] = sum; c += Wrap - 4;
	}
	for (i=sum=0,w=pxl,wgt=(int*)SincWgt+(ry<<2); i<4; i++) sum += (*(w++)) * (*(wgt++));
	if (sum < 0) return 0;
	if (sum > (255<<16)) return 255;
	return (sum>>16);
}


// subpixelxx - This function performs bilinear interpolation
static int subpixelxx(unsigned char *Img, int Wrap, LONG lx, LONG ly)
{
	LONG px, py, dx, dy; unsigned char *grxy;
	grxy = Img + (lx>>4) + (ly>>4)*Wrap;
	 px = lx&0xf; dx = 16-px; py = ly&0xf; dy = 16-py;
	return (((LONG)(*grxy)*dx*dy
		+ (LONG)(*(grxy+1))*px*dy
		 + (LONG)(*(grxy+Wrap))*dx*py
		  + (LONG)(*(grxy+1+Wrap))*px*py)>>8);
}


// ClosestPixel - This function performs nearest pixel sampling (fast but aliased)
static int ClosestPixel(unsigned char *Img, int Wrap, LONG lx, LONG ly)
{
	return *(Img + (ly>>4) * Wrap + (lx>>4));
}


// Transmute - This function maps a quadrilateral area in the source image into the
// rectangular region of the destination image using either bilinear or bicubic interpolation.
static void Transmutexx(unsigned char *SrcImg, unsigned char *DstImg, int SrcX, int SrcY, int DstWidth, int DstHeight,
             int ulx, int uly, int urx, int ury, int lrx, int lry, int llx, int lly, int InterpolationType)
{
	int x, y, ix, iy;
	int x0, x1, y0, y1, xp, yp;
	int (*InterpolationFunction)(unsigned char *, int, LONG, LONG);
   if (InterpolationType == 1) InterpolationFunction = subpixelxx;
	else if (InterpolationType == 2) InterpolationFunction = ClosestPixel;
	else InterpolationFunction = SubDatum;
	for (y=0; y<DstHeight; y++) {
		// Here we set up the starting and ending points of the line that we are looking at sampling for our new image
		x0 = (y*(llx - ulx)<<4)/DstHeight + (ulx<<4);
		y0 = (y*(lly - uly)<<4)/DstHeight + (uly<<4);
		x1 = (y*(lrx - urx)<<4)/DstHeight + (urx<<4);
		y1 = (y*(lry - ury)<<4)/DstHeight + (ury<<4);
		// Run through the line
		for (x=0; x<DstWidth; x++) {
			// Create pixel x,y coordinates down to sub pixel accuracy according to how many rows and columns that there are.
			xp = (x*(x1 - x0)/DstWidth) + x0; ix = xp >>4;
			yp = (x*(y1 - y0)/DstWidth) + y0; iy = yp >>4;
			// Make sure the pixel is in range before we take it.
			if ((ix>0)&&(ix<SrcX-2)&&(iy>0)&&(iy<SrcY-2)) *(DstImg++) = InterpolationFunction(SrcImg,SrcX,xp,yp);
			else *(DstImg++) = ClosestPixel(SrcImg,SrcX,xp,yp);
		}
	}
}


char font[9][192] ={{  // 1
0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,1,1,0,0,0,0,
0,0,0,0,0,0,1,1,0,0,0,0,
0,0,0,0,0,1,1,1,0,0,0,0,
0,0,1,1,1,1,1,1,0,0,0,0,
0,0,1,1,1,0,1,1,0,0,0,0,
0,0,0,0,0,0,1,1,0,0,0,0,
0,0,0,0,0,0,1,1,0,0,0,0,
0,0,0,0,0,0,1,1,0,0,0,0,
0,0,0,0,0,0,1,1,0,0,0,0,
0,0,0,0,0,0,1,1,0,0,0,0,
0,0,0,0,0,0,1,1,0,0,0,0,
0,0,0,0,0,0,1,1,0,0,0,0,
0,0,0,0,0,0,1,1,0,0,0,0,
0,0,0,0,0,0,1,1,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,
},{  // 2
0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,1,1,1,1,1,0,0,0,0,
0,0,1,1,1,1,1,1,1,0,0,0,
0,1,1,1,0,0,0,1,1,1,0,0,
0,1,1,0,0,0,0,0,1,1,0,0,
0,0,0,0,0,0,0,0,1,1,0,0,
0,0,0,0,0,0,0,0,1,1,0,0,
0,0,0,0,0,0,0,1,1,0,0,0,
0,0,0,0,0,0,1,1,0,0,0,0,
0,0,0,0,0,1,1,0,0,0,0,0,
0,0,0,0,1,1,0,0,0,0,0,0,
0,0,0,1,1,0,0,0,0,0,0,0,
0,0,1,1,0,0,0,0,0,0,0,0,
0,1,1,1,1,1,1,1,1,1,0,0,
0,1,1,1,1,1,1,1,1,1,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,
},{  // 3
0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,1,1,1,1,0,0,0,0,0,
0,0,1,1,1,1,1,1,0,0,0,0,
0,1,1,1,0,0,0,1,1,0,0,0,
0,1,1,0,0,0,0,1,1,0,0,0,
0,0,0,0,0,0,0,1,1,0,0,0,
0,0,0,0,1,1,1,1,0,0,0,0,
0,0,0,0,1,1,1,1,1,0,0,0,
0,0,0,0,0,0,0,1,1,1,0,0,
0,0,0,0,0,0,0,0,1,1,0,0,
0,0,0,0,0,0,0,0,1,1,0,0,
0,1,1,0,0,0,0,0,1,1,0,0,
0,1,1,1,0,0,0,1,1,1,0,0,
0,0,1,1,1,1,1,1,1,0,0,0,
0,0,0,1,1,1,1,1,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,
},{  // 4
0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,1,0,0,0,
0,0,0,0,0,0,0,1,1,0,0,0,
0,0,0,0,0,0,1,1,1,0,0,0,
0,0,0,0,0,1,1,1,1,0,0,0,
0,0,0,0,1,1,0,1,1,0,0,0,
0,0,0,0,1,1,0,1,1,0,0,0,
0,0,0,1,1,0,0,1,1,0,0,0,
0,0,1,1,0,0,0,1,1,0,0,0,
0,1,1,0,0,0,0,1,1,0,0,0,
0,1,1,1,1,1,1,1,1,1,1,0,
0,1,1,1,1,1,1,1,1,1,1,0,
0,0,0,0,0,0,0,1,1,0,0,0,
0,0,0,0,0,0,0,1,1,0,0,0,
0,0,0,0,0,0,0,1,1,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,
},{  // 5
0,0,0,0,0,0,0,0,0,0,0,0,
0,0,1,1,1,1,1,1,1,0,0,0,
0,0,1,1,1,1,1,1,1,0,0,0,
0,0,1,1,0,0,0,0,0,0,0,0,
0,0,1,1,0,0,0,0,0,0,0,0,
0,1,1,0,0,0,0,0,0,0,0,0,
0,1,1,0,1,1,1,1,0,0,0,0,
0,1,1,1,1,1,1,1,1,0,0,0,
0,1,1,0,0,0,0,0,1,1,0,0,
0,0,0,0,0,0,0,0,1,1,0,0,
0,0,0,0,0,0,0,0,1,1,0,0,
0,1,1,0,0,0,0,0,1,1,0,0,
0,1,1,1,0,0,0,1,1,1,0,0,
0,0,1,1,1,1,1,1,1,0,0,0,
0,0,0,1,1,1,1,1,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,
},{  // 6
0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,1,1,1,1,0,0,0,0,
0,0,0,1,1,1,1,1,1,0,0,0,
0,0,1,1,0,0,0,1,1,1,0,0,
0,1,1,0,0,0,0,0,1,1,0,0,
0,1,1,0,0,0,0,0,0,0,0,0,
0,1,1,0,1,1,1,1,0,0,0,0,
0,1,1,1,1,1,1,1,1,0,0,0,
0,1,1,1,0,0,0,1,1,1,0,0,
0,1,1,0,0,0,0,0,1,1,0,0,
0,1,1,0,0,0,0,0,1,1,0,0,
0,1,1,0,0,0,0,0,1,1,0,0,
0,0,1,1,0,0,0,1,1,1,0,0,
0,0,1,1,1,1,1,1,1,0,0,0,
0,0,0,0,1,1,1,1,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,
},{  // 7
0,0,0,0,0,0,0,0,0,0,0,0,
0,1,1,1,1,1,1,1,1,1,0,0,
0,1,1,1,1,1,1,1,1,1,0,0,
0,0,0,0,0,0,0,0,1,0,0,0,
0,0,0,0,0,0,0,1,1,0,0,0,
0,0,0,0,0,0,1,1,0,0,0,0,
0,0,0,0,0,0,1,0,0,0,0,0,
0,0,0,0,0,1,1,0,0,0,0,0,
0,0,0,0,0,1,1,0,0,0,0,0,
0,0,0,0,1,1,0,0,0,0,0,0,
0,0,0,0,1,1,0,0,0,0,0,0,
0,0,0,0,1,1,0,0,0,0,0,0,
0,0,0,1,1,0,0,0,0,0,0,0,
0,0,0,1,1,0,0,0,0,0,0,0,
0,0,0,1,1,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,
},{  // 8
0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,1,1,1,1,1,0,0,0,0,
0,0,1,1,1,1,1,1,1,0,0,0,
0,1,1,1,0,0,0,1,1,1,0,0,
0,1,1,0,0,0,0,0,1,1,0,0,
0,1,1,0,0,0,0,0,1,1,0,0,
0,1,1,1,0,0,0,1,1,1,0,0,
0,0,1,1,1,1,1,1,1,0,0,0,
0,0,1,1,1,1,1,1,1,0,0,0,
0,1,1,1,0,0,0,1,1,1,0,0,
0,1,1,0,0,0,0,0,1,1,0,0,
0,1,1,0,0,0,0,0,1,1,0,0,
0,1,1,1,0,0,0,1,1,1,0,0,
0,0,1,1,1,1,1,1,1,0,0,0,
0,0,0,1,1,1,1,1,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,
},{  // 9
0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,1,1,1,1,0,0,0,0,0,
0,0,1,1,1,1,1,1,1,0,0,0,
0,1,1,1,0,0,0,1,1,0,0,0,
0,1,1,0,0,0,0,0,1,1,0,0,
0,1,1,0,0,0,0,0,1,1,0,0,
0,1,1,0,0,0,0,0,1,1,0,0,
0,1,1,1,0,0,0,1,1,1,0,0,
0,0,1,1,1,1,1,1,1,1,0,0,
0,0,0,1,1,1,1,0,1,1,0,0,
0,0,0,0,0,0,0,0,1,1,0,0,
0,1,1,0,0,0,0,0,1,1,0,0,
0,1,1,1,0,0,0,1,1,0,0,0,
0,0,1,1,1,1,1,1,1,0,0,0,
0,0,0,1,1,1,1,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,
}};


void HistogramStretch(unsigned char *SrcImg,int wid, int hgt, int BlackPercent, int WhitePercent)
{
	int histogram[256],i,j,k,nWhiteToUse,nBlackToUse,delta,pixel,ExtremeLimit;
	unsigned char *ptr;
	k=hgt*wid;
	for (i=0; i<256; i++) histogram[i]=0;
	nBlackToUse=255; nWhiteToUse=0;
	ptr = SrcImg;
	for (i=0; i<k; i++) histogram[*ptr++]++;
	ExtremeLimit = BlackPercent*k/100;
	j=0;
	for (i=0; i<256; i++) {
		j += histogram[i];
		if (j > ExtremeLimit) {
			nBlackToUse = i;
			break;
		}
	}
	ExtremeLimit = (100-WhitePercent)*k/100;
	j=0;
	for (i=255; i>=0; i--) {
		j += histogram[i];
		if (j > ExtremeLimit) {
			nWhiteToUse = i;
			break;
		}
	}
	delta = nWhiteToUse - nBlackToUse;
	if (delta == 0) delta = 1;
	ptr = SrcImg;
	for (i=0; i<k; i++) {
      pixel = ((*ptr - nBlackToUse)*255)/(delta);
		if (pixel > 255) pixel = 255;
		if (pixel < 0) pixel = 0;
		*ptr++ = pixel;
   }
}


void PasteChar(int index, unsigned char *ptr, int wrap, unsigned char color)
{
	char *pfont = &font[index][0];
	int x,y;

	for (y=0; y<16; y++) {
		for (x=0; x<12; x++) {
			if (*pfont++) *ptr = color;
			ptr++;
		}
		ptr += wrap-12;
	}
}


void showSudoku(unsigned char *ImageBuffer, int wid, int hgt, char *BcMessage)
{
   //int Size;                             //not used
	int i;
   int x, y;
   unsigned char *ptr;
   Point corners[4];
   int Height = 200,Width = 200;
   unsigned char *pNewImg = (unsigned char *)malloc(Height*Width);

   //Size = Width*Height;                         //not used
   sscanf((const char*)BcMessage+108,"%d,%d %d,%d %d,%d %d,%d",&corners[0].x,&corners[0].y,&corners[1].x,&corners[1].y,&corners[2].x,&corners[2].y,&corners[3].x,&corners[3].y);
   PointSpread(&corners[0], &corners[1], 1,18,1);
   PointSpread(&corners[3], &corners[2], 1,18,1);
   PointSpread(&corners[0], &corners[3], 1,18,1);
   PointSpread(&corners[1], &corners[2], 1,18,1);
   Transmutexx(ImageBuffer, pNewImg, wid, hgt, Width, Height, corners[0].x, corners[0].y, corners[1].x, corners[1].y, corners[2].x, corners[2].y, corners[3].x, corners[3].y, 0);
   HistogramStretch(pNewImg, 200, 200, 1, 99);
   for (y=0; y<9; y++) {
      for (x=0; x<9; x++) {
         i = BcMessage[y*11+x];
         if (i>=81 && i<=89) {
            i = i-81;
				ptr = pNewImg + (y*20+13)*Width + x*20+15;
				PasteChar(i,ptr-Width-1,Width,0); PasteChar(i,ptr-Width,Width,0); PasteChar(i,ptr-Width+1,Width,0);
				 PasteChar(i,ptr+Width-1,Width,0); PasteChar(i,ptr+Width+1,Width,0); PasteChar(i,ptr,Width,255);
         }
      }
   }
   int a,b,c,d,e,f;
   a = ReaderConfiguration->TransmitImageWindowLeft;
   b = ReaderConfiguration->TransmitImageWindowRight;
   c = ReaderConfiguration->TransmitImageWindowTop;
   d = ReaderConfiguration->TransmitImageWindowBottom;
   e = ReaderConfiguration->TransmitImageFormat;
   f = ReaderConfiguration->ImageRotationValue;
   ReaderConfiguration->TransmitImageWindowLeft = 0;
   ReaderConfiguration->TransmitImageWindowRight = Width-1;
   ReaderConfiguration->TransmitImageWindowTop = 0;
   ReaderConfiguration->TransmitImageWindowBottom = Height-1;
   ReaderConfiguration->TransmitImageFormat = 8;
   ReaderConfiguration->ImageRotationValue = (2 + ReaderConfiguration->ImageRotationValue) % 4;
   ProcessImageShipStyleCommand(pNewImg, Width, Height, true);
   ReaderConfiguration->TransmitImageWindowLeft = a;
   ReaderConfiguration->TransmitImageWindowRight = b;
   ReaderConfiguration->TransmitImageWindowTop = c;
   ReaderConfiguration->TransmitImageWindowBottom = d;
   ReaderConfiguration->TransmitImageFormat = e;
   ReaderConfiguration->ImageRotationValue = f;
   free(pNewImg);
}


//********************************************************************************
#else
//********************************************************************************


void showSudoku(unsigned char *ImageBuffer, int wid, int hgt, char *BcMessage)
{
 //Dummy
}


//********************************************************************************
 #endif    //end #if
//********************************************************************************


