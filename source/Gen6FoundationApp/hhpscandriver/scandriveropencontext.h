/*------------------------------------------------------------------

*-------------------------------------------------------------------
* Scandriveropencontext.h - This is the file which contains the
* engine configuration variables used by the driver to control the
* scan engine.
*-------------------------------------------------------------------
* $RCSfile: scandriveropencontext.h $
* $Revision: 1.44.1.5 $
* $Date: 2011/09/02 08:23:31EDT $
*-------------------------------------------------------------------*/

#ifndef _SCAN_DRIVER_OPEN_CONTEXT_H_
#define _SCAN_DRIVER_OPEN_CONTEXT_H_

#include "ScanDriverInterface.h"

#ifdef BYD_SENSOR
#include "Hi704A.h"
#endif

#define MIN_ES_EXPOSURE_METHOD	HHPSD_AE_METHOD_UNIFORM
#define MAX_ES_EXPOSURE_METHOD	HHPSD_AE_METHOD_CENTER_WEIGHTED

#define MIN_ES_TARGET_VALUE	1
#define MAX_ES_TARGET_VALUE	255

#define	MIN_ES_TARGET_PERCENTILE 1
#define	MAX_ES_TARGET_PERCENTILE 99

#define MIN_ES_TARGET_ACCEPT_GAP 1
#define MAX_ES_TARGET_ACCEPT_GAP 50

#ifdef BYD_SENSOR

#define	MIN_ES_MAX_EXP 			BYD_INT_MIN
#define	MAX_ES_MAX_EXP 			BYD_INT_MAX

#define MIN_ES_MAX_GAIN 		BYD_GAIN_MIN
#define MAX_ES_MAX_GAIN 		BYD_GAIN_MAX

#define	MIN_ES_FRAME_RATE 		BYD_FRAME_RATE
#define	MAX_ES_FRAME_RATE 		BYD_FRAME_RATE

#else	//else #ifdef BYD_SENSOR

#define	MIN_ES_MAX_EXP 1
#define	MAX_ES_MAX_EXP 7874

#define MIN_ES_MAX_GAIN 1
#define MAX_ES_MAX_GAIN 64

#define	MIN_ES_FRAME_RATE 1
#define	MAX_ES_FRAME_RATE 30

#endif	//end #ifdef SENSOR

#define	MIN_ES_CONFORM_TRIES 1
#define	MAX_ES_CONFORM_TRIES 100

#define MIN_ES_SPECULAR_EXCLUSION 0
#define MAX_ES_SPECULAR_EXCLUSION 4

#define	MIN_ES_SPECULAR_SAT	200
#define	MAX_ES_SPECULAR_SAT	255

#define MIN_ES_SPECULAR_LIMIT 1
#define MAX_ES_SPECULAR_LIMIT 5

#ifdef HI2D_T//Hi704A gain and exposure time is different from Jade, TODO reuse Jade's.

#ifdef BYD_SENSOR

#define	MIN_ES_FIXED_EXP 		BYD_INT_MIN
#define	MAX_ES_FIXED_EXP 		BYD_INT_MAX
#define MIN_ES_FIXED_GAIN 		BYD_GAIN_MIN
#define MAX_ES_FIXED_GAIN 		BYD_GAIN_MAX
#define	MIN_ES_FIXED_FRAME_RATE BYD_FRAME_RATE
#define	MAX_ES_FIXED_FRAME_RATE BYD_FRAME_RATE

#else	//else #ifdef BYD_SENSOR

#define	MIN_ES_FIXED_EXP 300//step is 0.67 us, min 300 is 0.2ms
#define	MAX_ES_FIXED_EXP 13400//max is 9ms
#define MIN_ES_FIXED_GAIN 40//real gain = 0.5 + (g - 16)/32  so min is 1.25
#define MAX_ES_FIXED_GAIN 96//max is 3
#define	MIN_ES_FIXED_FRAME_RATE 1
#define	MAX_ES_FIXED_FRAME_RATE 30

#endif	//end #ifdef BYD_SENSOR

#else	//else #ifdef HI2D_T

#define	MIN_ES_FIXED_EXP 1
#define	MAX_ES_FIXED_EXP 7874
#define MIN_ES_FIXED_GAIN 1
#define MAX_ES_FIXED_GAIN 64
#define	MIN_ES_FIXED_FRAME_RATE 1
#define	MAX_ES_FIXED_FRAME_RATE 30

#endif	//end #ifdef HI2D_T


#if 0
#define MAX_ES_CELL_RED_WIN 120
#define MAX_ES_CELL_CLEAR_WIN 24			// 3ms
#define MAX_ES_CELL_TRI_FILTER 39			// 5ms
#else
#define MAX_ES_CELL_RED_WIN 120		// 15.2ms
#define MAX_ES_CELL_CLEAR_WIN 79		// 8ms
#define MAX_ES_CELL_TRI_FILTER 120		// 15.2ms
#endif

#ifdef SD_NAMESPACE
   namespace SD_NAMESPACE {
#endif



typedef struct {
	HHPScanDriverHHPExposureMethod MethodToUse; /* How should we do this */
	DWORD nTargetValue;			/* This is the target pixel count we shoot for */
	DWORD nTargetPercentile;	/* This goes along with Value, the target value should be at this percentile */
	DWORD nTargetAcceptanceGap;	/* How close to the target value must we be */
	DWORD nMaximumExposure;		/* This is the maximum exposure we're allowed to use */
	DWORD nMaximumGain;			/* This is the maximum gain we're allowed to use */
	DWORD nStartingExposure;	/* When scanning starts, this is the first exposure that we try (updated when scanning stops)*/
	DWORD nStartingGain;		/* When scanning starts, this is the first gain that we try (updated when scanning stops)*/
	DWORD nFrameRate;
	DWORD nLightsConfig;		/* This is the lights configuration on a scale of 1-100*/
	DWORD nAimerConfig;		/* This is the aimer configuration on a scale of 1-100*/
	DWORD  bImageMustConform;	/* The image must conform to the auto-exposure requirements, if not, it's rejected. */
	DWORD nTriesForConforming;	/* If bImageMustConform is set, this is the number of images that will be taken before we give up
								 * trying to conform.  We don't want to lock up the system trying to get a conforming image*/
	HHPScanDriverHHPExposureSpecularExclusion  nSpecularExclusion;	/* Do we exclude specular? to what degree? */
	DWORD nSpecialSpecularSaturation; /* These next two only come into play if somebody specifies nSpecularExclusion*/
	DWORD nSpecialSpecularLimit;	  /* to be HHPSD_SPECULAR_EXCLUSION_SPECIAL. */
	DWORD bAdjustExpWhenIllumOff; /* If the illumination is being turned off and on during a scanning session, do we want the exposure adjusted
											 * when the illumination is off */

	BOOL  bDecodeParameterSet;
	DWORD nDecodeParameter;
	
	BOOL bFastStart;
	DWORD nExposureCell;
	
} ScanDriverHHPAutoExposureOptions_t;

typedef struct {
	DWORD nFixedExposure;
	DWORD nFixedGain;
	DWORD nFrameRate;
} ScanDriverHHPFixedExposureOptions_t;

typedef struct {
	DWORD nExposure;
	DWORD nGain;
	DWORD nIllumValue;
	DWORD nIllumMaxValue;
	DWORD nIllumClipValue;
	DWORD nHeight;
	DWORD nWidth;
	DWORD nImageNumber;
	DWORD nIqScore;
	DWORD nDecodeParameter;
} ScanDriverLastScanInfo_t;

#define HHP_DEFAULT_OPTIONS_EXPOSURE_TYPE		HHPSD_AUTOEXPOSURE_DISABLE
#define HHP_DEFAULT_OPTIONS_SCANNING_ILLUM		HHPSD_ILLUM_ONLY_ON
#define HHP_DEFAULT_OPTIONS_NOT_SCANNING_ILLUM	HHPSD_ILLUM_AIMER_OFF

typedef enum{
	SBS_NOBUFFER,
	SBS_INVALID,
	SBS_FILLING,
	SBS_WAITING,
	SBS_IN_USE
} ScanBufferState_t;


typedef struct ScanBufferInfo{
	volatile DWORD hPhysicalScanBuffer;
	void * volatile pDriverBuffer;
	void * volatile pDriverBufferCheckLocation;
	volatile ScanBufferState_t BufferState;
	/* this carries the information on the last scan captured */
	volatile ScanDriverLastScanInfo_t LastScanInfo;
	/* this will be a future expansion which will allow multiple app buffers to
	 * be connected to the same physical buffer. */
	volatile int AppBuffersCurrentlyBound;
	struct ScanBufferInfo * volatile pNext;
}ScanBufferInfo_t;

typedef struct ScanBufferRegistry{
	void *pApplicationBuffer;
	ScanBufferInfo_t  *pPhysicalScanBuffer;
	struct ScanBufferRegistry *pNext;
}ScanBufferRegistry_t;

typedef struct ScanDriverLightingOptions{
	DWORD		nIlluminationIntensity;
	BOOL    	bAllowIllumIntensityToAdjust;
	BOOL		bAdjustIllumIntensityDown;
	DWORD		nAimerIntensity;
	BOOL    	bAllowAimerIntensityToAdjust;
	BOOL		bAdjustAimerIntensityDown;
	BOOL		bSoftwareControlledAimer;
	DWORD       nScanLightMode;
	DWORD       nIdleLightMode;
	DWORD       nIlluminationSource;
	DWORD       nIlluminationControl;

	BOOL bIlluminationOn() const
		{
			return nScanLightMode == HHPSD_LM_ILLUM_ON
				|| nScanLightMode == HHPSD_LM_ILLUM_ON_AIM_ON
				|| nScanLightMode == HHPSD_LM_CONCURRENT
				|| nScanLightMode == HHPSD_LM_INTERLACED
				/* || nScanLightMode == HHPSD_LM_FLASH */
				|| nScanLightMode == HHPSD_LM_BLINK;
		}

	BOOL bAimerOn() const
		{
			return nScanLightMode == HHPSD_LM_AIM_ON
				|| nScanLightMode == HHPSD_LM_ILLUM_ON_AIM_ON
				|| nScanLightMode == HHPSD_LM_CONCURRENT
				|| nScanLightMode == HHPSD_LM_INTERLACED
				|| nScanLightMode == HHPSD_LM_AIM_ONLY_NON_EXPOSURE;

				/*|| nScanLightMode == HHPSD_LM_FLASH*/  ;
		}
}ScanDriverLightingOptions_t;

typedef struct ScanEngineConfig {
	DWORD	nEngineId;
	DWORD	nIlluminationType;
	DWORD	nPixelClockFrequency;
	DWORD	nAimerCenterX;
	DWORD	nAimerCenterY;
	DWORD	nLedControlMode;
	DWORD	nEngineFwCksum;
	DWORD	nEngineFwVersion;
	DWORD	nEngineSensorIDRegister;
} ScanEngineConfig_t;

struct ScanDriverOpenContext_t {
	/* This variable is verification that somebody is using a real scan
	 * options structure, and not just calling the Driver arbitrarily. */
	DWORD nKey;
	HHPScanDriverEngineType_t ScanEngine;	/* What type of engine is connected */
	/* This determines whether we use autoexposure and shoot for a
	 * white value, or just fix the exposure and gain*/
	HHPScanDriverAutoExposureMode_t nAutoExposureMode;
	/* This structure contains the option settings for fixed exposure/gain */
	ScanDriverHHPFixedExposureOptions_t FixedExpOptions;
	/* This structure contains the option settings for running HHP autoexposure */
	ScanDriverHHPAutoExposureOptions_t AutoExpOptions;
	/* this determines the illumination state while scanning */
	ScanDriverLightingOptions_t LightingOptions;
	/* this will hold our array to the scan buffers */
	ScanBufferRegistry_t *pScanBuffers;
	/* with the addition of the PSOC , this will track configuration changes to the PSOC */
	ScanEngineConfig_t EngineConfig;
};

ScanDriverOpenContext_t *ScnDrv_CreateOpenContext(void);
BOOL ScnDrv_DeleteOpenContext(ScanDriverOpenContext_t *pOpenContext);
BOOL ScnDrv_SetExposureSettings(ScanDriverOpenContext_t *,DWORD *, DWORD);
BOOL ScnDrv_GetExposureSettings(ScanDriverOpenContext_t *,DWORD *, DWORD);
BOOL ScnDrv_GetEngineType(ScanDriverOpenContext_t *,DWORD *);
BOOL ScnDrv_SetExposureMode(ScanDriverOpenContext_t *,DWORD *);
BOOL ScnDrv_GetExposureMode(ScanDriverOpenContext_t *,DWORD *);
/* Buffer Registration Functions*/
BOOL CreateNewRegisteredBuffer(ScanDriverOpenContext_t *pOpenCntxt,void *pVirtualScanBuffer);
BOOL DeleteRegisteredBuffer(ScanDriverOpenContext_t *pOpenCntxt,void *pVirtualScanBuffer);
ScanBufferRegistry_t *GetBufferRegisterEntry(ScanDriverOpenContext_t *pOpenCntxt,void *pVirtualScanBuffer);
void DEBUG_DecrementEntries(ScanDriverOpenContext_t *pOpenCntxt,int nInfoIndex);
DWORD ScnDrv_GetImageAttributes(ScanDriverOpenContext_t *pSDOpenContext,void *pAppBuffer,DWORD *pBufOut, DWORD nLenOut);
BOOL ScnDrv_SetEngineConfig(ScanDriverOpenContext_t *pSDOpenContext,DWORD *pBufIn, DWORD dwLenIn);
BOOL ScnDrv_GetEngineConfig(ScanDriverOpenContext_t *pSDOpenContext,DWORD *pBufIn, DWORD dwLenIn);
unsigned GetFrameRate(ScanDriverOpenContext_t * pOptions);

#ifdef SD_NAMESPACE
   } // namespace SD_NAMESPACE {
#endif

#endif
