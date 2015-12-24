//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/dirt_test/Api.c $
// $Revision: 1.2 $
// $Date: 2010/01/25 03:55:52EST $
//======================================================================================

//======================================================================================
#include "api.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "platform.h"
//Port from SmearTest.c
static int compiAA(int N, double* iAA)
{
  int n, r, cc;
  double a, b, c, d, e, f, g, h, i, Mnn, Mrn;
  double D9[9]={1,2,6,4,30,12,42,24,90};
  double N9[9] ;
  double M[5][10]={
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1}};
  N9[0] = 0.0+N;
  for (n=1; n<9; n++)
    N9[n]  = N9[n-1] / N;
    i = 1*N9[0]/D9[0];
    h = (1*N9[0] + 1*N9[1])/D9[1];
    g = (2*N9[0] + 3*N9[1] + 1*N9[2])/D9[2];
    f = (1*N9[0] + 2*N9[1] + 1*N9[2]+ 0*N9[3])/D9[3];
    e = (6*N9[0] + 15*N9[1] + 10*N9[2] + 0*N9[3] - 1*N9[4])/D9[4];
    d = (2*N9[0] + 6*N9[1] + 5*N9[2] + 0*N9[3] - 1*N9[4] + 0*N9[5])/D9[5];
    c = (6*N9[0] + 21*N9[1] + 21*N9[2] + 0*N9[3] - 7*N9[4] + 0*N9[5] + 1*N9[6])/D9[6];
    b = (3*N9[0] + 12*N9[1] + 14*N9[2] + 0*N9[3] - 7*N9[4] + 0*N9[5] + 2*N9[6] + 0*N9[7])/D9[7];
    a = (10*N9[0] + 45*N9[1] + 60*N9[2] + 0*N9[3] - 42*N9[4] + 0*N9[5] + 20*N9[6] + 0*N9[7] - 3*N9[8])/D9[8];
    M[0][0] = a;
    M[0][1] = M[1][0] = b;
    M[0][2] = M[1][1] = M[2][0] = c;
    M[0][3] = M[1][2] = M[2][1] = M[3][0] = d;
    M[0][4] = M[1][3] = M[2][2] = M[3][1] = M[4][0] = e;
    M[1][4] = M[2][3] = M[3][2] = M[4][1] = f;
    M[2][4] = M[3][3] = M[4][2] = g;
    M[3][4] = M[4][3] = h;
    M[4][4] = i;

    //M=[M,eye(5)];

    for (n=0; n<5; n++) {
      Mnn=M[n][n];
      for (cc=0; cc<10; cc++)
        M[n][cc] /= Mnn;

    for (r=n+1; r<5; r++) {
      Mrn=M[r][n];
      for (cc=n; cc<10; cc++)
        M[r][cc] -= M[n][cc]*Mrn;
    }
  }

  for (n=4; n>0; n--)
    for (r=n-1; r>=0; r--) {
      Mrn=M[r][n];
      for (cc=n; cc<10; cc++)
        M[r][cc] -= M[n][cc]*Mrn;
    }
  for (n=0; n<5; n++) {
    for (r=0; r<5; r++)
      iAA[n*5+r]=M[n][r+5];
  }
  return 0;
}


// order 4-polyfit

//using the least square minimization method
static int polyfit(int* v, int N, double* iAA, int* fitv)
{
// v is the input vector for regression
// N is the length of the vector v,
// iAA is a 5x5 matrix depends on N, which is pre-computed for speeding
// fitv is the output, the polyfit vector of v -->> change: // (new fitv)=(v-(polyfit of v)*100
// mathematcally, fitv=A*inv(A'*A)*A'*v
// where A=[B.^4, B.^3, B.^2, B, ones(N,1)]; and B=[1:N]'/N;
//Our order of computing is:
//1. v1=A'*v
//2. v2=iAA*v1
//3. fitv=A*v2
//4. output = (v- (fitv from step 3))*100

  int  n, p;
  double tem, v1[5], v2[5];
  double pbyN;// pbyNm;

//compute v1=A'*v
  v1[0] = v1[1] = v1[2] = v1[3] = v1[4] = 0;
  for (p=1; p<N+1; p++) {
    tem = v[p-1];
    v1[4] += tem;
    pbyN=1.0*p/N;
    tem *= pbyN;
    v1[3] += tem;
    tem *= pbyN;
    v1[2] += tem;
    tem *= pbyN;
    v1[1] += tem;
    tem *= pbyN;
    v1[0] += tem;
    }

//compute v2=iAA*v1
  v2[0] = iAA[0] * v1[0] + iAA[1] * v1[1] + iAA[2] * v1[2] + iAA[3] * v1[3] + iAA[4] * v1[4];
  v2[1] = iAA[5] * v1[0] + iAA[6] * v1[1] + iAA[7] * v1[2] + iAA[8] * v1[3] + iAA[9] * v1[4];
  v2[2] = iAA[10] * v1[0] + iAA[11] * v1[1] + iAA[12] * v1[2] + iAA[13] * v1[3] + iAA[14] * v1[4];
  v2[3] = iAA[15] * v1[0] + iAA[16] * v1[1] + iAA[17] * v1[2] + iAA[18] * v1[3] + iAA[19] * v1[4];
  v2[4] = iAA[20] * v1[0] + iAA[21] * v1[1] + iAA[22] * v1[2] + iAA[23] * v1[3] + iAA[24] * v1[4];

//compute fitv=A*v2 ( = v2[4]+v2[3]*x+v2[2]*x^2+v2[1]*x^3+v2[0]*x^4 , where x=p/N, and p=1...,N)
for (p=1; p<N+1; p++){
    tem = v2[0];
    pbyN = 1.0*p/N;
    for (n=1; n<=4; n++)
       tem=(tem*pbyN)+v2[n];
    /* old code
    tem += v2[3]*pbyN;
    pbyNm = pbyN*pbyN;
    tem += v2[2]*pbyNm;
    pbyNm = pbyNm*pbyN;
    tem += v2[1]*pbyNm;
    pbyNm = pbyNm*pbyN;
    tem += v2[0]*pbyNm; //in fact, this is the value of fitv
    */
    fitv[p-1] = (int)((v[p-1] - tem)*100); //here, we use the variable fitv to hold the value of df=100(v-fitv)
    }
    return 0;
}

/*
	Function: SmearTest
	
	Determines if a Smear anomoly is present in standard white image.
	
	Parameters:

	pImage -- pointer to Input image.

	width -- # of pixels in x dimension of Input image.

	height -- # of pixels in y dimension of Input image.
 
	Returns:
	
	int - Smear score

	See Also:

*/
 int SmearTest(unsigned char *image,int width, int height)
{
  unsigned char *Pimage;
  int score, rn, cn;
  int r3[1284]; //1284 is the largest width of images I have
  int cr3[1284];
  int df[1284];
  int m=0;
  int s=0;
  double iAA[25] ={35.168135038510,  -70.392359606598,   45.300362228479,  -10.084150475807,   0.506419518958,
  -70.392359606598,   143.129862472856,  -94.024956856597,   21.526781437066,  -1.125825423591,
   45.300362228479,  -94.024956856597,   63.527292101211,  -15.148333959315,   0.844706140530,
  -10.084150475807,   21.526781437066,  -15.148333959315,   03.851925213811,  -0.241441199458,
   0.506419518958,  -1.125825423591,   0.844706140530,  -0.241441199458,   0.020128204391};//N=1254,
  int N=1254; //the normal size of the row vector sent for regression,
// FIXME int width2=2*width;
  int width3=3*width;

  //if the real width is not as the pre-computed case (width=1284 i.e., N=1254), re-compute iAA
  if (width!=N+30) {
    N=width-30;
    compiAA(N, iAA);
    //to reduce the computing error, since the result matrix should be symmetric
    for (rn=0;rn<5;rn++)
      for (cn=rn+1;cn<5;cn++) {
        iAA[rn*5+cn] += iAA[cn*5+rn];
        iAA[rn*5+cn] /= 2;
        iAA[cn*5+rn] = iAA[rn*5+cn];
      }
  }

  // r3 is the mean of 3 rows
  //r3=img(1,:)+img(2,:)+img(3,:);
  Pimage =  image+width;
  for (cn=0; cn<width; cn++) {
    r3[cn]=*(Pimage-width) + *(Pimage) + *(Pimage+width);
    Pimage++;
    }

  //process line by line, count the sum m and the smear count s
  for (rn=1; rn<height-2; rn++) {
    //cr3 is equal to one row in the result of conv2(image, ones(3,31), 'valid')
    cr3[0]=0;
    for (cn=0; cn<31; cn++) 
      cr3[0] += r3[cn];
    Pimage =  image+rn*width+15;
    for (cn=1; cn<N; cn++) {
       m += *(++Pimage);
       cr3[cn] = cr3[cn-1] - r3[cn-1] + r3[cn+30];
    }
    //4th order polyfitting of cr3, df is the error array
    polyfit(cr3, N,  iAA, df);

    //s=s+sum(abs(df)>60);
    for (cn=0; cn<N; cn++)
      if (df[cn]>6000) //value of df was enlarged 100 times, so did the threshold
        s++;
    //update r3 for next row
    //r3=r3-img(rn-1,:)+img(rn+2,:);
    Pimage =  image+(rn-1)*width;
    for (cn=0; cn<width; cn++)
      r3[cn] += (*((Pimage)+width3)-*(Pimage++));
  }
  score = (int)(1.0e5*s/m);

  return score;
}

static const char *SmearfileVerStr = "Smear Test $Revision: 1.2 $";
/*
	Function: GetSmearTestRevision

	Reports the MKS file version of SmearTest.c ($Revision: 1.2 $)

	Parameters:

	pstr -- pointer to rev string

	Returns:

	int - Non zero return indicates sucess

	See Also:
*/
 int GetSmearTestRevision (char *pstr)
{
 	strcpy (pstr,SmearfileVerStr);
	return 1;
}
// Port from ProductionTest.c

// returns sum of 8 points surrounding x,y
static int EightNeighborTotal(unsigned char *image, int width, int height, int x, int y)
{
    int total;
    if ((x < 1) || (y < 1) || (x > (width-2)) || (y > (height-2)))
        return 0;
    y *= width;
    total = image[y+x-1] + image[y+x+1];
    y -= width;
    total += image[y+x-1] + image[y+x] + image[y+x+1];
    y += width + width;
    total += image[y+x-1] + image[y+x] + image[y+x+1];
    return total;
}

// returns sum of 9 points (3x3 area) centered at x,y
static int NineNeighborTotal(unsigned char *image, int width, int height, int x, int y)
{
    int total;
    if ((x < 1) || (y < 1) || (x > (width-2)) || (y > (height-2)))
        return 0;
    y *= width;
    total = image[y+x-1] + image[y+x] + image[y+x+1];
    y -= width;
    total += image[y+x-1] + image[y+x] + image[y+x+1];
    y += width + width;
    total += image[y+x-1] + image[y+x] + image[y+x+1];
    return total;
}
// returns sum of 25 points (5x5 area) centered at x,y
static int TwentyFiveNeighborTotal(unsigned char *image, int width, int height, int x, int y)
{
	int total;
	if ((x < 3) || (y < 3) || (x > (width-4)) || (y > (height-4)))
		return 0;
	y *= width;
	total = image[y+x-2] + image[y+x-1] + image[y+x] + image[y+x+1] + image[y+x+2];
	y -= width;
	total += image[y+x-2] + image[y+x-1] + image[y+x] + image[y+x+1] + image[y+x+2];
	y -= width;
	total += image[y+x-2] + image[y+x-1] + image[y+x] + image[y+x+1] + image[y+x+2];

	y += width + width + width;
	total += image[y+x-2] + image[y+x-1] + image[y+x] + image[y+x+1] + image[y+x+2];

	y += width;
	total += image[y+x-2] + image[y+x-1] + image[y+x] + image[y+x+1] + image[y+x+2];
	return total;
}
// returns average of 49 points (7x7 area) centered at x,y
static int FortyNineNeighborAverage(unsigned char *image, int width, int height, int x, int y)
{
    int average, i, j;
    if ((x < 3) || (y < 3) || (x > (width-4)) || (y > (height-4)))
        return 0;
    for (average = 0, j = -3; j <= 3; j++)
        for (i = -3; i <= 3; i++)
            average += image[(y+j)*width+x+i];
    return (average*2 + 49) / 98;
}

// alternative to avg -- to deal with fly speck differences on Mpx -- is to use Median.  Borrowed (stole) median computation from flatness operation.
// (Better thing to do is make the Compute Median take the kernel as input... but no time!!)
#define KERNAL3 3  /*code below will not work correctly unless this is defined to 3 */
static unsigned int ComputeMedian3by3(unsigned char *pImg,int width, int height, int x,int y) {
   unsigned int data[KERNAL3*KERNAL3], *dptr, temp;
   unsigned char *ptr;
   int i,j;
   dptr = data;
   ptr = pImg + (y-1)*width + x-1;  /*places curser to upper left of 3x3 matrix */
   for (i=0; i<KERNAL3; i++) {
      for (j=0; j<KERNAL3; j++) *dptr++ = (unsigned int)*ptr++;
      ptr+=width-KERNAL3;
   }
   for (i=0; i<KERNAL3*KERNAL3; i++) {
      dptr = data;
      for (j=0; j<KERNAL3*KERNAL3-1; j++) {
         if (*dptr > *(dptr+1)) {temp = *dptr; *dptr = *(dptr+1); *(dptr+1) = temp;}
         dptr++;
      }
   }
   return data[KERNAL3*KERNAL3/2];
}
// returns average of 8 points surrounding x,y
static int QualifiedNeighborAverage(unsigned char *image, int width, int height, int x, int y)
{
    int i, j;
    int count = 0;
    int qualifiedAverage = 0;
    int average = EightNeighborTotal(image, width, height, x, y) / 8;
    for (j = -1; j <= 1; j++)
    {
        for (i = -1; i <= 1; i++)
        {
            if ((i != 0) || (j != 0))
            {
                if (abs((double)(image[(y+j)*width+x+i] - average)) < 16)
                {
                    qualifiedAverage += image[(y+j)*width+x+i];
                    count++;
                }
            }
        }
    }
    return (count < 5) ? average : (qualifiedAverage*2 + count) / (count*2);
}

// returns number of bad pixels in image, storing coordinates of each in xp[],yp[]
static int FindBadPixels(unsigned char *image, int width, int height, unsigned int config, int max,  int *xp, int *yp)
{
    int x, y;
    int coarseThreshold = 20;
    int fineThreshold = BADPIXEL_THRESHOLD;
    int r = 0;
	unsigned int flag = config;		//&1;  // future, if we get this specific, for now, non zero is enough to denote

	if (flag)
	{
		for (y = 1;( y < height - 1) && (r<max); y++)
		{
			for (x = 1; (x < width - 1) && (r<max); x++)
			{
				if ((abs((double)(image[y*width+x] - image[y*width+x+1])) > coarseThreshold) || (abs((double)(image[y*width+x] - image[(y+1)*width+x])) > coarseThreshold))
				{
					if (abs((double)(image[y*width+x] - ComputeMedian3by3(image, width, height, x, y))) > fineThreshold)
					{
						*xp++ = x;
						*yp++ = y;
						r++;
					}
				}
			}
		}
	}
	else
	{
		for (y = 1;( y < height - 1) && (r<max); y++)
		{
			for (x = 1; (x < width - 1) && (r<max); x++)
			{
				if ((abs((double)(image[y*width+x] - image[y*width+x+1])) > coarseThreshold) || (abs((double)(image[y*width+x] - image[(y+1)*width+x])) > coarseThreshold))
				{
					if (abs((double)(image[y*width+x] - QualifiedNeighborAverage(image, width, height, x, y))) > fineThreshold)
					{
						*xp++ = x;
						*yp++ = y;
						r++;
					}
				}
			}
		}
	}
    return r;
}

// replaces bad pixels in image at xp[np],yp[np] with a qualified value
static void RemoveBadPixels(unsigned char *image, int width, int height, unsigned int config, int np, int *xp, int *yp)
{
    int i;
	unsigned int flag = config;		//&1;  // future, if we get this specific, for now, non zero is enough to denote

	if (flag)
	{
		for (i = 0; i < np; i++)
		{
			if ((xp[i] >= 3) && (xp[i] < width - 4) && (yp[i] >= 3) && (yp[i] < height - 4))
			{
				image[yp[i]*width+xp[i]] = (unsigned char)ComputeMedian3by3(image, width, height, xp[i], yp[i]);
			}
		}
	}
	else
	{
		for (i = 0; i < np; i++)
		{
			if ((xp[i] >= 3) && (xp[i] < width - 4) && (yp[i] >= 3) && (yp[i] < height - 4))
			{
				image[yp[i]*width+xp[i]] = (unsigned char)QualifiedNeighborAverage(image, width, height, xp[i], yp[i]);
			}
		}
	}
}

// returns number of discontinuity pixels in image, storing coordinates of each in xd[],yd[]
static int FindDiscontinuitiesFlatMode(unsigned char *image, int width, int height, unsigned int config,int max, int *xd, int *yd)
{
	int x, y;
	int region1, region2, region3;
	int space,boxdim,startvalue;
	int flatThreshold = DISCONTINUITY_THRESHOLD;
	int r = 0;
	unsigned int flag = config;		//&1;  // future, if we get this specific, for now, non zero is enough to denote

	if(flag)
	{
		space = 7;
		boxdim = 5;
	}
	else
	{
		space = 2;
		boxdim = 3;
	}
	startvalue = (boxdim >> 1) + 1;
	for (y = startvalue; (y < height - 2*boxdim - space) && (r<max); y++)
	{
		for (x = startvalue; (x < width - 2*boxdim - space) && (r<max); x++)
		{
			if(flag)
			{
				region1 = TwentyFiveNeighborTotal(image, width, height, x, y);
				region2 = TwentyFiveNeighborTotal(image, width, height, x + boxdim + space, y);
				region3 = TwentyFiveNeighborTotal(image, width, height, x, y + boxdim + space);
				if(region1 > 150 * 25)
					flatThreshold = 110; //  3% difference  Need to be this agressive at higher target to cath everything.
				else if(region1 > 110 * 25 )
					flatThreshold = 130;   //  4.7% difference  Need to desenstize it at lower signals so we don't pick  up fakes.
				else
					flatThreshold = 150;  // 5.8% difference. Need to desenstize it at low signals so we don't pick  up fakes.
			}
			else
			{
				region1 = NineNeighborTotal(image, width, height, x, y);
				region2 = NineNeighborTotal(image, width, height, x + boxdim + space, y);
				region3 = NineNeighborTotal(image, width, height, x, y + boxdim + space);
			}

			if ((abs((double)(region1 - region2)) > flatThreshold) || (abs((double)(region1 - region3)) > flatThreshold))
			{
				*xd++ = x;
				*yd++ = y;
				r++;
			}
		}
	}
	return r;
}
// returns the number of points in xa[na],ya[na] that are in a dxd area centered at x,y
static int NeighborCount(int x, int y, int d, int na, int *xa, int *ya)
{
    int i;
    int r = 0;
    for (i = 0; i < na; i++)
        if ((xa[i] >= 0) && (ya[i] >= 0))
            if ((abs((double)(x - xa[i])) <= d) && (abs((double)(y - ya[i])) <= d))
                r++;
    return r;
}

// sets to invalid values any point in xa[na],ya[na] that are within +/-d of x,y
static void MakeInvalid(int x, int y, int d, int na, int *xa, int *ya)
{
    int i;
    d *= d;
    for (i = 0; i < na; i++)
        if ((xa[i] >= 0) && (ya[i] >= 0))
            if (((x - xa[i])*(x - xa[i]) + (y - ya[i])*(y - ya[i])) < d)
                xa[i] = ya[i] = -1;
}

// given image with discontinuity points at xd[nd],yd[nd], finds & stores major blemishes in xmajor[],ymajor[] and minor blemishes in xminor[],yminor[]
// returns number of out of spec blemishes
static int FindBlemishes(unsigned char *image, int width, int height, int nd, int *xd, int *yd, int maxmaj, int *nmajor, int *xmajor, int *ymajor, int maxmin, int *nminor, int *xminor, int *yminor)
{
    int i, j, k;
    int outofspec = 0;
    int distance = 20;
    int bin1 = DISCONTINUITY_COUNT_BIN1;
    int bin2 = DISCONTINUITY_COUNT_BIN2;
    *nmajor = 0;
    for (k = 0; (k < nd) && (*nmajor<maxmaj); k++)
    {
        if (NeighborCount(xd[k], yd[k], 3, nd, xd, yd) > bin2)
        {
            int average;
            if ((xd[k] < 0) || (yd[k] < 0))
                continue;
            // major blemish
            if ((xd[k] - 4 - distance) > 0)
                average = FortyNineNeighborAverage(image, width, height, xd[k] - distance, yd[k]);
            else if ((yd[k] - 4 - distance) > 0)
                average = FortyNineNeighborAverage(image, width, height, xd[k], yd[k] - distance);
            else
                average = FortyNineNeighborAverage(image, width, height ,xd[k] + distance, yd[k]);
            for (i = 0; i <= 20; i++)
            {
                for (j = 0; j <= 20; j++)
                {
                    if (((xd[k] + i + 3) < width) && (yd[k] + j + 3) < height)
                    {
                        int average2 = FortyNineNeighborAverage(image, width, height, xd[k] + i, yd[k] + j);
                        if ((average2 < (average - 22)) || (average2 > (average + 12)))
                            outofspec++;
                    }
                }
            }
            *xmajor++ = xd[k];
            *ymajor++ = yd[k];
            (*nmajor)++;
            MakeInvalid(xd[k], yd[k], 30, nd, xd, yd);
        }
    }
    *nminor = 0;
    for (k = 0; (k < nd) && (*nminor<maxmin); k++)
    {
        if (NeighborCount(xd[k], yd[k], 3, nd, xd, yd) > bin1)
        {
            *xminor++ = xd[k];
            *yminor++ = yd[k];
            (*nminor)++;
            MakeInvalid(xd[k], yd[k], 30, nd, xd, yd);
        }
    }
    return outofspec;
}

// this function implements the equivalent of the original VB dirt test program
// operates only as FLAT_MODE_ONE_BLEMISH
// returns a special pass/fail code

/*
	Function: ProductionTest

	Implements the equivalnet of the original VB dirt test program.  Operates only as FLAT_MODE_ONE_BLEMISH.

	Parameters:

	testimage -- pointer to Input image.

	width -- # of pixels in x dimension of Input image.

	height -- # of pixels in y dimension of Input image.

	config -- config flag.   Imagined bit field for other controls, bit 0: 0, filter=avg(5000 Vga), 1, filter=median(Mpx)

	maxdis -- maximum # of discontinuities allowed.  Should be the array sizes passed in.

	ndis -- pointer to number of discontinuities
	
	xdis -- pointer to array of X coordinates for discontinuities

	ydis -- pointer to array of Y coordinates for discontinuities

	maxbad -- maximum # of bad pixels allowed.  Should be array size passed in. 
	
	nbad -- pointer to number of bad pixels

	xbad -- pointer to array of X coordinates of bad

	ybad -- pointer to array of Y coordinates of bad

	maxmaj -- maximum number of major blemishes.  Should be the array sizes passed in.
	
	nmajor -- pointer to number of major blemishes

	xmajor -- pointer to array of X coordinates of major blemishes

	ymajor -- pointer to array of Y coordinates of major blemishes

	maxminor -- maximum number of minor blemishes.  Should be the array sizes passed in. 

	nminor -- pointer to number of minor

	xminor -- pointer to array of X coordinates of minor blemishes

	yminor -- pointer to array of Y coordinates of minor blemishes

	outofspec -- pointer to number of blemishes

	Returns:

	int - non zero value means success.  0 return value implies one of the internal test reached their max.

	See Also:

*/
 int ProductionTest(unsigned char *testimage, int width, int height, unsigned int config, 
                                                    int maxdis, int *ndis, int *xdis, int *ydis,
                                                    int maxbad, int *nbad, int *xbad, int *ybad,
                                                    int maxmaj, int *nmajor, int *xmajor, int *ymajor,
                                                    int maxmin, int *nminor, int *xminor, int *yminor, int *outofspec)
{
    int result = 1;
	unsigned int flag = config&1; // keep future plan for other bit field controls already viable

    // image processing steps
    *nbad = FindBadPixels(testimage, width, height, flag, maxbad, xbad, ybad);
    if (*nbad)
        RemoveBadPixels(testimage, width, height, flag, *nbad, xbad, ybad);
    *ndis = FindDiscontinuitiesFlatMode(testimage, width, height, flag, maxdis, xdis, ydis);
	RESET_WATCHDOG_TIMER();
    *outofspec = FindBlemishes(testimage, width, height, *ndis, xdis, ydis, maxmaj, nmajor, xmajor, ymajor, maxmaj, nminor, xminor, yminor);

	if (*ndis >= maxdis || *nmajor >= maxmaj || *nminor >= maxmin || *nbad >= maxbad)
		result = 0;

    return result;
}

static const char *ProductfileVerStr = "ProductionTest $Revision: 1.2 $";
/*
	Function: GetProductionTestRevision

	Reports the MKS file version of ProductionTest.c ($Revision: 1.2 $)

	Parameters:

	pstr -- pointer to rev string

	Returns:

	int - Non zero return indicates sucess

	See Also:
*/
 int GetProductionTestRevision (char *pstr)
{
 	strcpy (pstr,ProductfileVerStr);
	return 1;
}

// port from detect.c
// distance between 3x3 sample areas (a dirt station test parameter)
#define GAP 2

// return 1 if x,y is within distance r of xc,yc, 0 otherwise
static int Within(int x, int y, int xc, int yc, int r)
{
    return (((x-xc)*(x-xc) + (y-yc)*(y-yc)) <= r*r) ? 1 : 0;
}

// compute the centroid and area of a polygon of na vertices at xa[],ya[]
// assumes na >= 3
static void Centroid(int na, int *xa, int *ya, int *xctr, int *yctr, int *area)
{
    int i, j, ai, ac = 0, xc = 0, yc = 0;
    for (i = na - 1, j = 0; j < na; i = j, j++)
    {
        ai = xa[i]*ya[j] - xa[j]*ya[i];
        ac += ai;
        xc += (xa[j] + xa[i]) * ai;
        yc += (ya[j] + ya[i]) * ai;
    }
    *area = ac / 2;
    if (ac != 0)
    {
        *xctr = xc / (3 * ac);
        *yctr = yc / (3 * ac);
    }
    else
    {
        *xctr = xa[0];
        *yctr = ya[0];
    }
}

// returns sum of 9 points (3x3 area) centered at x,y
static int Sum9Pixels(unsigned char *image, int width, int height, int x, int y)
{
    int total;
    if ((x < 1) || (y < 1) || (x > (width-2)/*750*/) || (y > (height-2)/*478*/))
        return 0;
    y *= width;
    total = image[y+x-1] + image[y+x] + image[y+x+1];
    y -= width;
    total += image[y+x-1] + image[y+x] + image[y+x+1];
    y += width + width;
    total += image[y+x-1] + image[y+x] + image[y+x+1];
    return total;
}

// look for a discontinuity (using discontinuity criteria from the dirt station test program)
static int Discontinuity(unsigned char *image, int width, int height, int x, int y)
{
    int dc, dn, ds, de, dw;
    int threshold = 60;
    if ((x < 3-GAP) || (y < 3-GAP) || (x > (width-4)/*748*/-GAP) || (y > (height-4)/*476*/-GAP))
        return 0;
    dc = Sum9Pixels(image, width, height, x, y);
    dn = Sum9Pixels(image, width, height, x, y-2-GAP);
    ds = Sum9Pixels(image, width, height, x, y+2+GAP);
    dw = Sum9Pixels(image, width, height, x-2-GAP, y);
    de = Sum9Pixels(image, width, height, x+2+GAP, y);
//    if ((abs(dn - dc) > threshold) || (abs(ds - dc) > threshold) || (abs(dw - dc) > threshold) || (abs(de - dc) > threshold))
    if (((dn - dc) > threshold) || ((ds - dc) > threshold) || ((dw - dc) > threshold) || ((de - dc) > threshold))
        return 1;
    return 0;
}

/*
 Hugging a wall (adapted from mxbinimg.c):

 find a point below the top edge - first check E (or harmlessly first check N)

 if last dir is N then first check W
 if last dir is E then first check N
 if last dir is S then first check E
 if last dir is W then first check S

  (check up to 4 directions)
N:
 check (x,y-1) - if an edge then last dir is N & keep the point
E:
 check (x+1,y) - if an edge then last dir is E & keep the point
S:
 check (x,y+1) - if an edge then last dir is S & keep the point
W:
 check (x-1,y) - if an edge then last dir is W & keep the point
 goto N:
*/

// steps along a wall of discontinuity points from x,y and stores the next step in xa[],ya[]
// this function finds all points along a dirt spot in the image given initial point x0,y0
// returns the number of points stored in xp[],yx[]
int LocateSpot(unsigned char *image, int width, int height, int x0, int y0, int *xp, int *yp)
{
    int np, x, y, tx, ty, dir, done;

    xp[0] = x0;
    yp[0] = y0;
    np = 1;
    tx = x0;
    ty = y0;
    dir = 2;
    // find all the points along the spot (up to a reasonable maximum)
    do
    {
        // take the next step along the discontinuity
        // direction in which to step is 0,1,2,3 = N,E,S,W
        done = --dir + 4;
        x = tx;
        y = ty;
        while (dir != done)
        {
            switch (dir & 3)
            {
                case 0:
                    tx = x;
                    ty = y - 1;
                    break;
                case 1:
                    tx = x + 1;
                    ty = y;
                    break;
                case 2:
                    tx = x;
                    ty = y + 1;
                    break;
                //case 3:
                default:
                    tx = x - 1;
                    ty = y;
                    break;
            }
            if (Discontinuity(image, width, height, tx, ty))
            {
                xp[np] = tx;
                yp[np] = ty;
                np++;
                break;
            }
            dir++;
        }
        // stop the loop when the original starting point is reached
        if ((xp[np-1] == x0) && (yp[np-1] == y0))
            break;
    } while (np < MAX_SPOT_PERIMETER);

    return np;
}

// compute the vector cross product of P1-P0 and P2-P0
static int CrossProduct(int x0, int y0, int x1, int y1, int x2, int y2)
{
    return (x1-x0)*(y2-y0)-(x2-x0)*(y1-y0);
}

// convex hull storage
static int nh, xh[MAX_CONVEX_HULL], yh[MAX_CONVEX_HULL];

// compute the convex hull of a set of na points at xa[],ya[]
// (assumes the points describe a polygon in clockwise order)
static void ConvexHull(int na, int *xa, int *ya)
{
    int i;
    // assume 1st 2 points are on the hull
    xh[0] = xa[0];
    yh[0] = ya[0];
    xh[1] = xa[1];
    yh[1] = ya[1];
    nh = 2;
    // the set of points should only make "right" turns;
    // whenever a left is made, then the previous point (& possibly more!) is not on the hull
    for (i = nh; i < na; i++)
    {
        while (CrossProduct(xh[nh-2], yh[nh-2], xh[nh-1], yh[nh-1], xa[i], ya[i]) <= 0)
        {
            nh--;
            if (nh <= 2)
                break;
        }
        xh[nh] = xa[i];
        yh[nh] = ya[i];
        nh++;
        if (nh == MAX_CONVEX_HULL)
            break;
    }
}

// find the center and radius of the dirt spot described by na points at xa[],ya[]
// (first & last point are the same)
static void ProcessSpot(int na, int *xa, int *ya, int *xc, int *yc, int *r)
{
    int i, t;
    int area;// avgx, avgy;
    int rmin, rmax;

    // nothing to process, really
    if (na <= 3)
    {
        *xc = xa[0];
        *yc = ya[0];
        *r = 0;
        return;
    }
    else if (na >= MAX_SPOT_PERIMETER)
    {
        *xc = xa[0];
        *yc = ya[0];
        *r = 1000;
        return;
    }

    // remove the last point, which is the same as the first
    na--;

    // reduce the set of point to only those on the outer perimeter
    ConvexHull(na, xa, ya);
    // use the convex hull points as the dirt spot definition
    for (na = 0; na < nh; na++)
    {
        xa[na] = xh[na];
        ya[na] = yh[na];
    }

    // choose a method to find the shape's center
    // one method also finds area but area is not used
#if 1
    // find the center and area of the spot
    Centroid(na, xa, ya, xc, yc, &area);

#endif

    // find the radius of the enclosing circle
    *r = 0;
    rmax = 0;
    rmin = 10000;
    for (i = 0; i < na; i++)
    {
        t = (*xc - xa[i])*(*xc - xa[i]) + (*yc - ya[i])*(*yc - ya[i]);
        if (t > *r)
            *r = t;
        if (t > rmax)
            rmax = t;
        if (t < rmin)
            rmin = t;
    }
    rmax = (int)(sqrt((double)rmax) + 0.5);
    rmin = (int)(sqrt((double)rmin) + 0.5);
    *r = (int)(sqrt((double)*r) + 0.5);

    // determine dirt type
    // use profile to determine type

/*
    // save edge points (for debugging)
    for (i = 0; i < na; i++, nedge++)
    {
        xedge[nedge] = xa[i];
        yedge[nedge] = ya[i];
    }
    xedge[nedge] = -1;
    yedge[nedge] = -1;
    nedge++;
*/
}

// points along the rim of a dirt spot
static int nrim, xrim[4096], yrim[4096];

// find dirt spots in a given image
// returns the number of spots and stores their centers in xc[],yc[] and radii in rad[]
//static int FindDirt(unsigned char *image, int width, int height, int *xc, int *yc, int *rad)
static int FindDirtSpots(unsigned char *image, int width, int height, DirtSpotInfo *di)
{
    int x, y, i;
 //   int dc, dn, ds, de, dw;
    int n = 0;
    for (y = 3 + GAP; y < height - 3 - GAP; y++)
    {
        for (x = 3 + GAP; x < width - 3 - GAP; x++)
        {
            // if this point is within an existing dirt spot then skip it
            for (i = 0; i < n; i++)
                if (Within(x, y, di[i].x, di[i].y, di[i].r + 1))
                    break;
            if (i < n)
                continue;
            // look for a first discontinuous point
            if (Discontinuity(image, width, height, x, y))
            {
                // find all the points along the spot
                nrim = LocateSpot(image, width, height, x, y, xrim, yrim);
                // add this spot to the list
                ProcessSpot(nrim, xrim, yrim, &di[n].x, &di[n].y, &di[n].r);
                n++;
                if (n == MAX_STORED_DIRT_SPOTS)
                    return n;
            }
        }
    }
    return n;
}

// all the points along dirt spot boundaries (for debugging)
int nedge, xedge[16384], yedge[16384];

// find dirt spots in a given image and return whether the imager passes or fails
// dirt spot information is stored in the given locations
/*
	Function: DirtTestImage

	Locates dirts spots in image and provides coordinate information for them.

	Parameters:

	image -- pointer to Input image.

	width -- # of pixels in x dimension of Input image.

	height -- # of pixels in y dimension of Input image.

	numDirtSpots -- number of dirt spots

	di -- coordinates

	Returns:

	int - Non zero indicates success

	See Also:

	<DirtSpotInfo>

*/
 int DirtTestImage(unsigned char *image, int width, int height, int *numDirtSpots, DirtSpotInfo *di)
{
    int dist[MAX_DIRT_RADIUS+2];
    int i;
    int r = 1;
    int numClearAreaSpots = 0;
    int numLargeDirtSpots = 0;

    // for debugging
    nedge = 0;

    // derive all dirt spot information
    *numDirtSpots = FindDirtSpots(image, width, height, di);
    r = *numDirtSpots;

    // test the dirt exclusion area
    for (i = 0; i < r; i++)
    {
        if ((di[i].r > 0) &&
            (di[i].x >= CLEAR_AREA_LEFT) && (di[i].x <= CLEAR_AREA_RIGHT) &&
            (di[i].y >= CLEAR_AREA_TOP) && (di[i].y <= CLEAR_AREA_BOTTOM))
            numClearAreaSpots++;
    }

    // gather a distribution of dirt radii
    for (i = 0; i < MAX_DIRT_RADIUS+2; i++)
        dist[i] = 0;
    for (i = 0; i < r; i++)
    {
        if (di[i].r <= MAX_DIRT_RADIUS)
            dist[di[i].r]++;
        else
            dist[MAX_DIRT_RADIUS+1]++;
    }

    // count the number of large spots
    for (i = MIN_DIRT_RADIUS; i <= MAX_DIRT_RADIUS; i++)
        numLargeDirtSpots += dist[i];

    // set a pass/fail return flag
    r = 1;
    if ((numLargeDirtSpots > MAX_LARGE_DIRT_SPOTS) || (numClearAreaSpots > 0) ||
        (*numDirtSpots > MAX_TOTAL_DIRT_SPOTS) || (dist[MAX_DIRT_RADIUS+1] > 0))
        r = 0;

    return r;

}

static const char *DirttestfileVerStr = "Dirt Test $Revision: 1.2 $";
/*
	Function: GetDirtTestRevision 

	Reports the MKS file version of Detect.c ($Revision: 1.2 $)

	Parameters:

	pstr -- pointer to rev string

	Returns:

	int - Non zero return indicates sucess

	See Also:
*/
 int GetDirtTestRevision (char *pstr)
{
 	strcpy (pstr,DirttestfileVerStr);
	return 1;
}

//port from RowNoise.c
/*
Straight from Tom's "Fish Egg Cacophony" email: assume identical to what's in Acuity!!

Row Noise, for this case, is defined as rhe relationship of the
grayscale value of any pixel in an image with the grayscale value of the
pixel directly above or below it. Row noise is apparent in the MegaPixel
imager. It is caused by the unequal area of each sensor in the odd and even
rows. The microlenses do much to attenuate the effect but is still relatively
strong at the top and bottom of the images. While there will be a degradation
of decoding effectiveness, we have decided that we can live with it since the
center of the image has only negligible noise. An image that is shipped to the
user, however, must be corrected to eliminate the row noise.

Assumptions:
All MegaPixel images will have row noise.
Row noise is consistent across any two rows, Meaning the effect is the same in
the middle of a row as it is on the edges.

Process:
Take an image of a flatly illuminated, featureless field of medium gray level.
Compute the average grayscale value of each row. Compute the ratio of each
Even/Odd line pair. Best fit the line that describes this data set. Return the
adjusted correction ratio for the first and last line.

Results:
Two values are returned. These are used to compute the Correction Ratios for
the top and bottom of the image. To conserve memory space in the PSOC, Each
value is a single byte. The Top Value TV is used to compute the top Correction
Ratio TR like this: TR = (TV + 256)/ 256.
Similarily for the bottom: BR = (BV + 256) / 256.


SOURCE CODE FOR COMPUTING ROW NOISE CORRECTION VALUES
*/

typedef struct {
        float slope, intercept;
} line_t;

// LinearRegression() computes the line that best fits a set of points using the least-squares method.
//      Since slopes of vertical lines approach infinity, lines that are vertical need to be translated to horisontal (Swap
//              the X and Y values.)
static line_t linear_regression(int *x_points, int *y_points, int num_points) {
        long x_avg = 0;
        long y_avg = 0;
        long  numerator = 0;
        long denominator = 0;
        int i;
        line_t line;
        float a, b;

        for (i=0; i<num_points; i++) {
      x_avg += *(x_points+i);
      y_avg += *(y_points+i);
        }
        x_avg = ((long) (((int) x_avg + num_points / 2) / num_points));
        y_avg = ((long) (((int) y_avg + num_points / 2) / num_points));

        for (i=0; i<num_points; i++) {
      numerator   += (*(x_points+i) - x_avg) * ((long) *(y_points+i) - y_avg);
      denominator += (*(x_points+i) - x_avg) * (*(x_points+i) - x_avg);
        }
        if (denominator == 0) denominator++;
  b = (float)numerator/(float)(denominator);
  a =  (float)y_avg - (b * (float)x_avg);
        line.slope = b;
        line.intercept = a;
        return line;
}

//This routine computes the Row Noise Correction values.
// Assumptions: Image is uniformly illuminated, not saturated, featureless, target white 160 (+/- 30)
//              Row noise changes linearly through the image, and is uniform horizontally.
// Process: Each row of the image is averaged. The ratio of each even/odd row pair is generated. The ratios are then line fit
//          to produce the correction ratio for each line pair. The result is the ratios of the top and bottom line pairs.
//          The ratio R is expressed as 256*R-256
int RowNoiseReduction (unsigned char *img, int w, int h, int *topRatio, int *bottomRatio) {
// Original function bool RowNoiseReduction(void) {
   // Get image height and width from scan driver.
 //       DWORD dwHeight, dwWidth;
 //       SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&dwHeight,4,NULL);
 //       SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&dwWidth,4,NULL);
        int             ImageXDimension = w; //(int) dwWidth;
        int             ImageYDimension = h; //(int) dwHeight;

   unsigned char *ptr0, *ptr1;
   int a, b, *sums, *indexes;
   int x, y;
   line_t line;
   sums = (int *) malloc(ImageYDimension*sizeof(int));
   indexes = (int *) malloc(ImageYDimension*sizeof(int));
   for (y=0; y<ImageYDimension; y+=2) {
      ptr0 = img + ImageXDimension * y; // Even rows
      ptr1 = ptr0 + ImageXDimension; // Odd rows
      a = b = 0;
      for (x=0; x<ImageXDimension; x++) {
         a += (int)((unsigned int)*ptr0++);
         b += (int)((unsigned int)*ptr1++);
      }
      sums[y>>1] = (int)(.5 + (float)a * 256.0 / (float)b);
      indexes[y>>1] = y;
   }
   line = linear_regression(indexes,sums,ImageYDimension/2);
	*topRatio = (int)(line.intercept+.5) - 256;
	*bottomRatio = (int)(line.slope *(959.0) + line.intercept + .5) - 256;
//   printf("\r\nTopRatio=%d Put in IMGRNT\r\nBottomRatio=%d Put in IMGRNB\r\n",(int)(line.intercept+.5) - 256,(int)(line.slope *(959.0) + line.intercept + .5) - 256);
   free(indexes);
   free(sums);
        return (1);
}

void RowNoiseCalibrateB (unsigned char *img, int w, int h, int *topRatio, int *bottomRatio) {
	int      ImageXDimension = w; //(int)NumXBytes;
	int      ImageYDimension = h; //(int)NumYBytes;
	unsigned char *pImg = img;	

	unsigned char *ptr0, *ptr1;
	int a, b, *sums, *indexes;
	int x, y;
	line_t line;
	sums = (int *) malloc(ImageYDimension*sizeof(int));
	indexes = (int *) malloc(ImageYDimension*sizeof(int));
	for (y=0; y<ImageYDimension; y+=2) {
		ptr0 = pImg + ImageXDimension * y + ImageXDimension/4;
		ptr1 = ptr0 + ImageXDimension;
		a = b = 0;
		for (x=0; x<ImageXDimension/2; x++) {
			a += (int)((unsigned int)*ptr0++);
			b += (int)((unsigned int)*ptr1++);
		}
		sums[y>>1] = (int)(.5 + (float)a * 256.0 / (float)b);
		indexes[y>>1] = y;
	}
	
	line = linear_regression(&indexes[ImageYDimension/8],&sums[ImageYDimension/8],3*ImageYDimension/4/2);
	*topRatio = (int)(line.intercept+.5) - 256;
	*bottomRatio = (int)(line.slope *(float)(ImageYDimension-1) + line.intercept + .5) - 256;

	free(indexes);
	free(sums);
}

/*
	Function: CalculateRowNoiseParameters

	Calculates top and bottom ratio to be stored in PSOC.

	Parameters:

	pImage -- pointer to Input image.

	width -- # of pixels in x dimension of Input image.

	height -- # of pixels in y dimension of Input image.

	topRatio -- parameter to be stored in PSOC for particular unit to be applied on top 1/2 in Row Noise correction

	bottomRatio -- parameter to be stored in PSOC for particular unit to be applied on bottom 1/2 in Row Noise correction

	Returns:

	int - Non zero return indicates sucess

	See Also:
*/
 int CalculateRowNoiseParameters (unsigned char *image, int width, int height, int *topRatio, int *bottomRatio)
{
	RowNoiseCalibrateB (image,width,height,topRatio,bottomRatio);
	return 1;
}

static const char *RowNoisefileVerStr = "Row Noise Calculation $Revision: 1.2 $";
/*
	Function: GetRowNoiseCalculationRevision

	Reports the MKS file version of RowNoise.c ($Revision: 1.2 $)

	Parameters:

	pstr -- pointer to rev string

	Returns:

	int - Non zero return indicates sucess

	See Also:
*/
 int GetRowNoiseCalculationRevision (char *pstr)
{
 	strcpy (pstr,RowNoisefileVerStr);
	return 1;
}

//Port from SignalToNoise.c
/*
	Function: ComputeSNR

	Calculates SNR of image.

	Parameters:

	pImage -- pointer to Input image

	width -- # of pixels in x dimension of Input image

	height -- # of pixels in y dimension of Input image

	PixelAverage -- pointer to mean pixel value

	PixelStandardDeviation -- pointer to SD of pixels in Input image

	NoiseAverage -- pointer to output of noise average

	RMSNoise -- pointer to output of RMS noise

	SignalToNoise -- pointer to resulting S/N ratio
	 
	Returns:

	int - Non zero return indicates sucess

	See Also:
*/
 int ComputeSNR(unsigned char *pImage, int width, int height, double *PixelAverage, double *PixelStandardDeviation, double *NoiseAverage, double *RMSNoise, double *SignalToNoise)
{
   unsigned char *pPixel;
   int x,y;
   int NumXBytes, NumYBytes;
   long NumberOfPixels, TotalOfPixels=0;
   long dif, NumberOfComparisons, TotalOfDiffBetweenPixels, TotalOfSquaredDiffBetweenPixels;
   	   double diff;
   	double SumOfPixelDiffsSquared=0;

   NumXBytes =width; NumYBytes =height;
	//*******************************************
	// scan once to get the total of the pixels,
	//*******************************************
 	NumberOfPixels=0; TotalOfPixels=0;
   	pPixel=pImage;
	   for ( y=0 ; y<NumYBytes ; y++) {  	  // for each row
  		   for (x=0 ; x<NumXBytes ; x++){ 	 	  // for each pixel
			   TotalOfPixels += *pPixel++;          // add together all pixel values
   		}
	   }
    	NumberOfPixels = NumXBytes*NumYBytes;
	   *PixelAverage = (double)TotalOfPixels/NumberOfPixels;

   	//*******************************************
	   // scan once to get the total of the diffs between the pixels and
   	//*******************************************
  //	   long   dif;
   	NumberOfComparisons =0; TotalOfDiffBetweenPixels=0; TotalOfSquaredDiffBetweenPixels=0;
   	pPixel=pImage;
	   for ( y=0 ; y<NumYBytes ; y++) {  	  // for each row
  		   for (x=0 ; x<NumXBytes-1 ; x++){ 	 	  // for each pixel
			   dif = abs(*pPixel - *(pPixel+1)); // absolute value of horizontal difference
   			TotalOfDiffBetweenPixels+=dif;
	   		TotalOfSquaredDiffBetweenPixels+=(dif*dif);
		   	pPixel++;
   		}
	   	pPixel++; // skip last pixel in each row
   	}
	   pPixel=pImage;
   	for ( y=0 ; y<NumYBytes-1 ; y++) {  	  // for each row
  	   	for (x=0 ; x<NumXBytes ; x++){ 	 	  // for each pixel
		   	dif = abs(*pPixel - *(pPixel+NumXBytes)); // absolute value of vertical difference
			   TotalOfDiffBetweenPixels+=dif;
   			TotalOfSquaredDiffBetweenPixels+=(dif*dif);
	   		pPixel++;
		   }
   	}
	   NumberOfComparisons = 2*NumXBytes*NumYBytes-NumXBytes-NumYBytes;
   	*NoiseAverage = (double)TotalOfDiffBetweenPixels/NumberOfComparisons;
	   if (*NoiseAverage==0)
		   *SignalToNoise = -1;
   	else
	   	*SignalToNoise = *PixelAverage / *NoiseAverage;
   	if (*SignalToNoise>999.99) *SignalToNoise=999.99;
	   *RMSNoise=pow(TotalOfSquaredDiffBetweenPixels/(NumberOfComparisons),0.5);

   	//*******************************************
	   // scan once to get the sum of squares of Pixel-PixelAverage
   	//*******************************************
	   diff=0; SumOfPixelDiffsSquared=0;
	   pPixel=pImage;
   	for ( y=0 ; y<NumYBytes ; y++) {  	  // for each row
  	   	for (x=0 ; x<NumXBytes ; x++){ 	 	  // for each pixel
		   	diff=*pPixel - *PixelAverage;
			   SumOfPixelDiffsSquared+=(diff)*(diff);
   			pPixel++;
	   	}
   	}
	   *PixelStandardDeviation=pow(SumOfPixelDiffsSquared/(NumberOfPixels-1),0.5);
   
	   
	return 1;
}

static const char *SNRfileVerStr = "Signal To Noise $Revision: 1.2 $";
/*
	Function: GetSNRRevision

	Reports the MKS file version of SignalToNoise.c ($Revision: 1.2 $)

	Parameters:

	pstr -- pointer to rev string

	Returns:

	int - Non zero return indicates sucess

	See Also:
*/
 int GetSNRRevision (char *pstr)
{
 	strcpy (pstr,SNRfileVerStr);
	return 1;
}
//port from flatness.c
#define KERNAL 5
unsigned int ComputeMedian(unsigned char *pImg,int width, int height, int x,int y) {
   unsigned int data[KERNAL*KERNAL], *dptr, temp;
   unsigned char *ptr;
   int i,j;
   dptr = data;
   ptr = pImg + y*width + x;
   for (i=0; i<KERNAL; i++) {
      for (j=0; j<KERNAL; j++) *dptr++ = (unsigned int)*ptr++;
      ptr+=width-KERNAL;
   }
   for (i=0; i<KERNAL*KERNAL; i++) {
      dptr = data;
      for (j=0; j<KERNAL*KERNAL-1; j++) {
         if (*dptr > *(dptr+1)) {temp = *dptr; *dptr = *(dptr+1); *(dptr+1) = temp;}
         dptr++;
      }
   }
   return data[KERNAL*KERNAL/2];
}

/*
	Function: CalculateOpticalRolloff

	Calculates rolloff at 8 center points on edge of image.

	Parameters:

	pImage -- pointer to Input image.

	width -- # of pixels in x dimension of Input image.

	height -- # of pixels in y dimension of Input image.

	cx, cy -- pointers to center coordinates of optical field calculated.

	ul,uc,ur,ml,mr,ll,lc,lr -- pointers to percentage rolloff at that location from center.
>
>
>		     |-----------------------|
>		     |ul        uc        ur |
>		     |                       |
>		     |ml                  mr |
>		     |                       |
>		     |ll        lc        lr |
>		     |-----------------------|

	Returns:

	int - Non zero return indicates sucess

	See Also:

*/
 int CalculateOpticalFlatness (unsigned char *image, int width, int height, int *cx, int *cy, int *ul, int *uc, int *ur, int *ml, int *mr, int *ll, int *lc, int *lr)
{
  // char text[64];
   unsigned int value, max;
   int maxx=0,maxy=0,x,y;
   int NumXBytes = width;
   int NumYBytes = height;

   max = 0;
   for (x=0; x<NumXBytes-KERNAL; x++) {
      for (y=0; y<NumYBytes-KERNAL; y++) {
         value = ComputeMedian(image,width, height, x,y);
         if (value > max) {
            max = value;
            maxx = x;
            maxy = y;
         }
      }
	  RESET_WATCHDOG_TIMER();	
   }
	
   *cx = maxx;
   *cy = maxy;
	*ul = ComputeMedian(image,width, height, 0,0)*100/max;
	*ur = ComputeMedian(image,width, height, NumXBytes-KERNAL,0)*100/max;
	*ll = ComputeMedian(image,width, height, 0,NumYBytes-KERNAL)*100/max;
	*lr = ComputeMedian(image,width, height, NumXBytes-KERNAL,NumYBytes-KERNAL)*100/max;
	*uc = ComputeMedian(image,width, height, (NumXBytes-KERNAL)/2,0)*100/max;
	*lc = ComputeMedian(image,width, height, (NumXBytes-KERNAL)/2,NumYBytes-KERNAL)*100/max;
	*ml = ComputeMedian(image,width, height, 0,(NumYBytes-KERNAL)/2)*100/max;
	*mr = ComputeMedian(image,width, height, NumXBytes-KERNAL,(NumYBytes-KERNAL)/2)*100/max;

	return 1;
}

static const char *FlatnessfileVerStr = "Flatness $Revision: 1.2 $";
/*
	Function: GetFlatnessRevision

	Reports the MKS file version of Flatness.c ($Revision: 1.2 $)

	Parameters:

	pstr -- pointer to rev string

	Returns:

	int - Non zero return indicates sucess

	See Also:
*/
 int GetFlatnessRevision (char *pstr)
{
 	strcpy (pstr,FlatnessfileVerStr);
	return 1;
}
static const char  *verStr = "dist_test $Revision: 1.2 $";
/*
	Function: GetDLLVersion
	
	Reports the MKS version of the DLL ($ProjectRevision: 1.194 $)

	Parameters:

	pstr -- pointer to rev string

	Returns:

	int - Non zero return indicates sucess

	See Also:
*/
 int GetDIRTVersion (char *pstr)
{
 	strcpy (pstr,verStr);
	return 1;
}

   
