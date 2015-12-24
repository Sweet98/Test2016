//#include <windows.h>
#include "sampling.h"
#include "auto_exposure_accumulator.h"

#define elements(x) (sizeof(x)/sizeof(*x))

void 
Auto_exposure_accumulator ::
specular_handler(DWORD nPixelGroup)
{
    DWORD nMaxVal,nMinVal,nPerceivedSpecularSamples,i,nCurrentPixel;
    nMaxVal = nMinVal = (DWORD)(nPixelGroup&0xff);
    /* First we sample the pixel group for what we consider specular.*/
    for (i=4,nPerceivedSpecularSamples=0;i>0;i--,nPixelGroup>>=8) {
	nCurrentPixel = (nPixelGroup&0xff);
	if (nCurrentPixel > nMaxVal)
	    nMaxVal = nCurrentPixel;
	if (nCurrentPixel < nMinVal)
	    nMinVal = nCurrentPixel;
	if (nCurrentPixel >= nSaturationValue) 
	    nPerceivedSpecularSamples++;
    }
    /* Now, we determine if those samples contain specular information.  If they do, then all of the
     * samples in the group are marked as specular.  We consider specular to mean if all 4 pixels were
     * above the saturation value, or if atleast one of the pixels was above the saturation value, but
     * all of the pixels were not too far behind. */
    if (nPerceivedSpecularSamples == 4) {
	specular_cnt += 4;
    } else if (nPerceivedSpecularSamples > 0) {
	nextToSpecular += 4;
	if (specularMinSample > nMinVal)
	    specularMinSample = nMinVal;
	if (specularMaxSample < nMinVal)
	    specularMaxSample = nMinVal;
    }
}

void 
Auto_exposure_accumulator ::
accumulate(DWORD pixelGroup)
{
    unsigned j;
    specular_handler(pixelGroup);
    for (j = 0; j < 4; ++j)
    {
		nHistogram[pixelGroup&0xff]++;
		pixelGroup >>= 8;
    }
    sample_cnt += 4;
}

void
Auto_exposure_accumulator ::
reset()
{
    specular_cnt = 0;
    specularMinSample = 255;
    specularMaxSample = 0;
    nextToSpecular = 0;
    sample_cnt = 0;
	
    DWORD *p = &nHistogram[0];
    DWORD *e = &nHistogram[elements(nHistogram)];
    while (p < e)
		*p++ = 0;
}
