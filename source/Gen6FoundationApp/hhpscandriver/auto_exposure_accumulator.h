#ifndef AUTO_EXPOSURE_ACCUMULATOR_8cd46372_a084_11de_b811_001eecfc0375
#define AUTO_EXPOSURE_ACCUMULATOR_8cd46372_a084_11de_b811_001eecfc0375

#if defined(_WIN32_WCE) || defined(PALMOS) || defined(_MSC_VER)
#include <windows.h>
#else
#include "language.h"
#include "platform.h"

#include <string.h>
#include <stdio.h>

#endif

class Auto_exposure_accumulator
{
  public:
	Auto_exposure_accumulator()
		: nSaturationValue(0)
    {
		reset();
    }
	

    DWORD specular_cnt;
    DWORD specularMinSample;
    DWORD specularMaxSample;
    DWORD nextToSpecular;
    DWORD sample_cnt;
    DWORD nHistogram[256];

    DWORD nSaturationValue;

    void reset();
	void setSaturationValue(DWORD _nSaturationValue) 
		{ 
			nSaturationValue = _nSaturationValue; 
		}
    void specular_handler(DWORD nPixelGroup);
    void accumulate(DWORD pixelGroup);
};

#endif // #ifndef AUTO_EXPOSURE_ACCUMULATOR_8cd46372_a084_11de_b811_001eecfc0375
