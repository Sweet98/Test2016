/*------------------------------------------------------------------

*-------------------------------------------------------------------
* ScanDriverOpenContext.c - This file contains the functions
* associated with manipulating the ScanDriverOpenContext structure
*-------------------------------------------------------------------
* $RCSfile: scandriveropencontext.c $
* $Revision: 1.56 $
* $Date: 2011/09/02 08:21:41EDT $
*--------------------------------------------------------------------*/
#if defined(_WIN32_WCE) || defined(PALMOS) || defined(_MSC_VER)
#include <windows.h>
#else // DBALL
#include <stdlib.h>
#include "language.h"
#include "platform.h"
#endif

#include "ScanDriverOpenContext.h"
#include "ScanDriverScanManager.h"
#include "ScanDriverInterface.h"
#include "ScanDriverScannerSelect.h"
#include "ScanDriverLightControl.h"
#include "ScanDriverDebug.h"

#ifdef SD_NAMESPACE
   namespace SD_NAMESPACE {
#endif

/*
  Here, we set up global default configuration for the open context.
  This configuration works with most engines.  Then, the engine
  specific code, HHPSD.SetDefaultScannerOptions() is called to give
  each engine a chance to override the open context configuration.

  This was done to make the code easier to maintain.  It used to be
  when a new field was added to the open context, every
  SetDefaultScannerOptions function for each engine had to be modified
  to initialize this field.  Now, we can intialize the field in this
  function and override it in HHPSD.SetDefaultScannerOptions() for
  special cases.

  Because this function is new, most open context fields are not
  initialized in it yet.  Fields that existed before this function was
  created are still intialized in the HHPSD.SetDefaultScannerOptions().
  As new fields are added to open context they can be initialized here
  first.
*/
static void SetDefaultGlobalOptions(ScanDriverOpenContext_t * pOpenContext)
{
	ScanDriverLightingOptions_t & lightingOptions = pOpenContext->LightingOptions;

	lightingOptions.nScanLightMode      = HHPSD_LM_OFF;
	lightingOptions.nIdleLightMode      = HHPSD_LM_OFF;
	lightingOptions.nIlluminationSource = HHPSD_IS_PRIMARY;
}


/*-------------------------------------------------------------------
 * ScnDrv_CreateOpenContext - Allocate the memory and set to defaults
 *  the open context memory
 *-------------------------------------------------------------------*/
ScanDriverOpenContext_t *ScnDrv_CreateOpenContext(void) {
	ScanDriverOpenContext_t *pOpenContext;
	pOpenContext = (ScanDriverOpenContext_t *)zalloc(sizeof(ScanDriverOpenContext_t));
	if (pOpenContext != NULL) {
		/* This is how we will validate when a user calls with this handle that
		 * we actually created it. */
		pOpenContext->nKey = (DWORD)pOpenContext;
#ifdef PRODUCT_CONFIGURATION_APPLICATION
		pOpenContext->nAutoExposureMode = HHPSD_EXPOSURE_FIXED;
#else
		pOpenContext->nAutoExposureMode = HHPSD_AUTOEXPOSURE_USE_HHP;
#endif
		pOpenContext->pScanBuffers = NULL;
		pOpenContext->LightingOptions.nIlluminationControl = FALSE;
		SetDefaultGlobalOptions(pOpenContext);
		if (HHPSD.SetDefaultScannerOptions(pOpenContext) == FALSE) {
			zfree(pOpenContext);
			pOpenContext = NULL;
		}
	}
	return pOpenContext;
}


/*-------------------------------------------------------------------
 * ScnDrv_DeleteOpenContext - Deallocate the memory for the open
 *  context.
 *-------------------------------------------------------------------*/
BOOL ScnDrv_DeleteOpenContext(ScanDriverOpenContext_t *pOpenContext) {
	/* Make sure that we are done with imaging, and have turned off lights
	 * In all these functions, if we're doing these things these functions
	 * will disable them, if not, no harm is done. */
	ScanDriverStopScanning(pOpenContext,FALSE);
	LightControlAimerState(pOpenContext,0);
	LightControlIlluminationState(pOpenContext,0);
	/* If the open context still has buffers connected, we need to get rid of them. */
	if (pOpenContext->pScanBuffers != NULL) {
		while (pOpenContext->pScanBuffers != NULL) {
			/* This should take care of both unregistering and deleting the scan buffers themselves */
			if (UnregisterVirtualScanBuffer(pOpenContext,pOpenContext->pScanBuffers->pApplicationBuffer)==FALSE)
				return FALSE;
		}
	}
	zfree(pOpenContext);
	return TRUE;
}

/*-------------------------------------------------------------------
 * CreateNewRegisteredBuffer - This function attempts to add a
 *   buffer as a registered buffer for a given open context
 *-------------------------------------------------------------------*/
BOOL CreateNewRegisteredBuffer(ScanDriverOpenContext_t *pOpenCntxt,void *pVirtualScanBuffer) {
	ScanBufferRegistry_t *pReg;
	if (pOpenCntxt->pScanBuffers != NULL) {
		pReg = pOpenCntxt->pScanBuffers;
		/* We add the new registered buffer onto the end of the linked list */
		while (pReg->pNext != NULL) {
			/* Here we protect against somebody accidentally trying to register the same buffer
			 * multiple times. */
			if (pReg->pApplicationBuffer == pVirtualScanBuffer) {
				return FALSE;
			}
			pReg = pReg->pNext;
		}
		/* We never check the buffer whose "next" will be the new buffer for
		 * a repeat entry.  */
		if (pReg->pApplicationBuffer == pVirtualScanBuffer) {
			return FALSE;
		}
		/* Allocate memory for the new buffer */
		pReg->pNext = (ScanBufferRegistry_t *)zalloc(sizeof(ScanBufferRegistry_t));
		/* Point to our new buffer */
		pReg = pReg->pNext;
	} else {
		/* Allocate memory for the new buffer */
		pOpenCntxt->pScanBuffers = (ScanBufferRegistry_t *)zalloc(sizeof(ScanBufferRegistry_t));
		/* Point to our new buffer */
		pReg = pOpenCntxt->pScanBuffers;
	}
	/* If our buffer allocation was successful, we initialize it. */
	if (pReg != NULL) {
		pReg->pApplicationBuffer = pVirtualScanBuffer;
		pReg->pPhysicalScanBuffer = NULL;
		pReg->pNext = NULL;
		return TRUE;
	}
	return FALSE;
}
/*-------------------------------------------------------------------
 * DeleteRegisteredBuffer - Get rid of a given registered buffer
 *-------------------------------------------------------------------*/
BOOL DeleteRegisteredBuffer(ScanDriverOpenContext_t *pOpenCntxt,void *pVirtualScanBuffer) {
	ScanBufferRegistry_t *pReg,*pRegLast;
	pReg = pOpenCntxt->pScanBuffers;
	/* Make sure that there's a buffer to be pointing at. */
	if (pReg != NULL) {
		/* if the first entry in the list is the one we want to get rid of, we have to
		 * physically change the entry in the open context like we're doing here*/
		if (pReg->pApplicationBuffer == pVirtualScanBuffer) {
			/* Physically remove the entry from the list, but pReg still points to the
			 * allocated space.*/
			pOpenCntxt->pScanBuffers = pReg->pNext;
		} else {
			pRegLast = pReg;
			pReg = pReg->pNext;
			while (pReg != NULL) {
				if (pReg->pApplicationBuffer == pVirtualScanBuffer) {
					/* Physically remove the entry from the list, but pReg still points to the
					 * allocated space.*/
					pRegLast->pNext = pReg->pNext;
					break;
				}
				pRegLast = pReg;
				pReg = pReg->pNext;
			}
		}
		/* If we came out with pReg being non-NULL, it means we have something to delete.*/
		if (pReg != NULL) {
			zfree(pReg);
			return TRUE;
		}
	}
	return FALSE;
}
/*-------------------------------------------------------------------
 * GetBufferRegisterEntry - Gets the registered entry of a buffer for
 *   a given open context
 *-------------------------------------------------------------------*/
ScanBufferRegistry_t *GetBufferRegisterEntry(ScanDriverOpenContext_t *pOpenCntxt,void *pVirtualScanBuffer) {
	ScanBufferRegistry_t *pReg;
	pReg = pOpenCntxt->pScanBuffers;
	while (pReg != NULL) {
		if (pReg->pApplicationBuffer == pVirtualScanBuffer)
			break;
		pReg = pReg->pNext;
	}
	return pReg;
}
/*-------------------------------------------------------------------
 * ScnDrv_SetExposureMode - Set the autoexposure mode in the system
 *  according to the request. The options are on chip, fixed, HHP or
 *  presentation style
 *-------------------------------------------------------------------*/
BOOL ScnDrv_SetExposureMode(ScanDriverOpenContext_t *pSDOpenContext,DWORD *pBufIn) {
	DWORD dwExposureMode = *pBufIn;

	DBGM(16,) dbg_output("HHPSD_IOCTL_SET_EXPOSURE_SETTINGS = %u\r\n",
						 dwExposureMode);
	BOOL bCleanupNeeded;
	bCleanupNeeded = ScanDriverPrepareChangeConfig(pSDOpenContext);
	pSDOpenContext->nAutoExposureMode = (HHPScanDriverAutoExposureMode_t)dwExposureMode;
	if (bCleanupNeeded) ScanDriverCleanupChangeConfig(pSDOpenContext);
	return TRUE;
}
/*-------------------------------------------------------------------
 * ScnDrv_GetExposureMode - Get the mode of exposure control we're
 *   currently configured for.
 *-------------------------------------------------------------------*/
BOOL ScnDrv_GetExposureMode(ScanDriverOpenContext_t *pSDOpenContext,DWORD *pBufIn) {
	*pBufIn = pSDOpenContext->nAutoExposureMode;
	return TRUE;
}
/*-------------------------------------------------------------------
 * CheckRanges does a quick check of the values passed inton array
 * ScnDrv_SetEngineConfig to ensure they are valid. If any one
 * parameter is invalid none will be changed.
 * We are are thinking that when time permits we'll push this checking
 * to the hardware layer as the parameters may vary by camera.
 *-------------------------------------------------------------------*/
BOOL CheckRangesEngineConfig (DWORD *pBufIn, DWORD dwLenIn)
{
	DWORD Tag, x;

	for (x=0; x<((dwLenIn-sizeof(DWORD))/(sizeof(DWORD))); x+=2)
	{
		Tag = *pBufIn++;
		switch (Tag)
		{
			case HHPSD_ILLUMINATION_TYPE:
//				if ((int)*pBufIn < 0 || (int)*pBufIn > 1)
//					*pBufIn = 1;
//				break;

			default:
				break;
		}
		pBufIn++;
	}
 	return TRUE;

}

/*-------------------------------------------------------------------
 * ScnDrv_SetEngineConfig - Search through the passed in array
 * and set the appropriate parameters in the scan driver open context
 * structure.
 *-------------------------------------------------------------------*/
BOOL ScnDrv_SetEngineConfig(ScanDriverOpenContext_t *pSDOpenContext,
							   DWORD *pBufIn, DWORD dwLenIn)
{
	DWORD Tag, Value, x;
	BOOL bCleanupNeeded;
    DBG_OUTPUT("HHPSD_SetEngineConfig\r\n");

	// Do a quick range check on the settings passed in. Eventually
	// we will make this camera specific at the hardware level.
	if (!(CheckRangesEngineConfig(pBufIn, dwLenIn)))
	{
	    DBG_OUTPUT("Engine config RangeCheck() fail\r\n");
		return FALSE;
	}

	bCleanupNeeded = ScanDriverPrepareChangeConfig(pSDOpenContext);
	for (x=0; x<((dwLenIn-sizeof(DWORD))/(sizeof(DWORD))); x+=2)
	{
		Tag = *pBufIn++;	Value =  *pBufIn++;
		switch (Tag)
		{

			case HHPSD_ILLUMINATION_TYPE:
				DBG_OUTPUT3("SetEngineConfigTag=HHPSD_IllUMINATION_TYPE(%d),Value=%d(%x)\r\n",Tag,Value,Value);
				DWORD nIlluminationSource;
				if (Value)
					nIlluminationSource = HHPSD_IS_PRIMARY;
				else
					nIlluminationSource = HHPSD_IS_ALTERNATE;
				LightControlSetIlluminationSource(pSDOpenContext, nIlluminationSource);

				break;

// All other tags are Read Only so ignore
			default:
			    DBG_OUTPUT3("SetEngineConfigTag(%d), Value=%d(%x)\r\n",Tag,Value,Value);
				break;
		}
	}
	if (bCleanupNeeded) ScanDriverCleanupChangeConfig(pSDOpenContext);
 	return TRUE;
}

/*-------------------------------------------------------------------
 * ScnDrv_GetEngineConfig - Read Config items
 *-------------------------------------------------------------------*/
BOOL ScnDrv_GetEngineConfig(ScanDriverOpenContext_t *pSDOpenContext,DWORD *pBufIn, DWORD dwLenIn)
{
	DWORD x;
    DBG_OUTPUT("HHPSD_GetEngineConfig\r\n");

	for (x=0; x<((dwLenIn-sizeof(DWORD))/(sizeof(DWORD))); x+=2)
	{
		DWORD Tag = *pBufIn++;
        DWORD Value = (DWORD)-1;
		switch (Tag)
		{
			case HHPSD_ILLUMINATION_TYPE:
				if (pSDOpenContext->LightingOptions.nIlluminationSource == HHPSD_IS_PRIMARY)
					Value = 1;
				else
					Value = 0;
				DBG_OUTPUT3("Tag=HHPSD_IllUMINATION_TYPE(%x) Value=%d(%x)\r\n",Tag,Value,Value);
				break;
			case HHPSD_LED_CONTROL_MODE_OBSOLETE:
				Value = pSDOpenContext->EngineConfig.nIlluminationType;
				DBG_OUTPUT3("Tag=HHPSD_LED_CONTROL_O(%x) Value=%d(%x)\r\n",Tag,Value,Value);
				break;
			case HHPSD_PIXEL_CLOCK_FREQUENCY_OBSOLETE:
				Value = pSDOpenContext->EngineConfig.nPixelClockFrequency;
				DBG_OUTPUT3("Tag=HHPSD_PIXEL_CLOCK_FREQUENCY_O(%x) Value=%d(%x)\r\n",Tag,Value,Value);
				break;
			case HHPSD_ENGINE_ID:
				Value = pSDOpenContext->EngineConfig.nEngineId;
				DBG_OUTPUT3("Tag=HHPSD_ENGINE_ID(%x) Value =%d(%x)\r\n",Tag, Value,Value);
				break;
			case HHPSD_ENGINE_FW_CKSUM:
				Value = pSDOpenContext->EngineConfig.nEngineFwCksum;
				DBG_OUTPUT3("Tag=HHPSD_ENGINE_FW_CKSUM(%x) Value=%d(%x)\r\n",Tag,Value,Value);
				break;
			case HHPSD_ENGINE_FW_VERSION:
				Value = pSDOpenContext->EngineConfig.nEngineFwVersion;
				DBG_OUTPUT3("Tag=HHPSD_ENGINE_FW_VERSION(%x) Value=%d(%x)\r\n",Tag,Value,Value);
				break;
			case HHPSD_ENGINE_AIMER_CENTER_X:
				Value = pSDOpenContext->EngineConfig.nAimerCenterX;
				DBG_OUTPUT3("Tag=HHPSD_ENGINE_AIMER_X_CENTER(%x) Value=%d(%x)\r\n",Tag,Value,Value);
				break;
			case HHPSD_ENGINE_AIMER_CENTER_Y:
				Value = pSDOpenContext->EngineConfig.nAimerCenterY;
				DBG_OUTPUT3("Tag=HHPSD_ENGINE_AIMER_Y_CENTER(%x) Value=%d(%x)\r\n",Tag,Value,Value);
				break;
			case HHPSD_ENGINE_SENSOR_ID:
				Value = pSDOpenContext->EngineConfig.nEngineSensorIDRegister;;
				DBG_OUTPUT3("Tag=HHPSD_ENGINE_SENSOR_ID(%x) Value=%d(%x)\r\n",Tag,Value,Value);
				break;

			default:
			    DBG_OUTPUT3("Tag not found %d %d(%x)\r\n",Tag,Value,Value);
				break;
		}
        *pBufIn++ = Value;
	}
 	return TRUE;
}


/*-------------------------------------------------------------------
 * CheckRanges does a quick check of the values passed inton array
 * ScnDrv_SetExposureSettings to ensure they are valid. If any one
 * parameter is invalid none will be changed.
 * We are are thinking that when time permits we'll push this checking
 * to the hardware layer as the parameters may vary by camera.
 *-------------------------------------------------------------------*/
BOOL CheckRanges(DWORD *pBufIn, DWORD dwLenIn)
{
	DWORD Tag, x;

	for (x=0; x<((dwLenIn-sizeof(DWORD))/(sizeof(DWORD))); x+=2)
	{
		Tag = *pBufIn++;
		switch (Tag)
		{
			case ES_EXPOSURE_METHOD:
				if ((int)*pBufIn < MIN_ES_EXPOSURE_METHOD)
					*pBufIn = MIN_ES_EXPOSURE_METHOD;
				else if ((int)*pBufIn > MAX_ES_EXPOSURE_METHOD)
					*pBufIn = MAX_ES_EXPOSURE_METHOD;
//dbg_output("ES_EXPOSURE_METHOD Min:%d, Max:%d\r\n",MIN_ES_EXPOSURE_METHOD,MAX_ES_EXPOSURE_METHOD);
				break;

			case ES_TARGET_VALUE:
				if ((int)*pBufIn < MIN_ES_TARGET_VALUE)
					*pBufIn = MIN_ES_TARGET_VALUE;
				else if ((int)*pBufIn > MAX_ES_TARGET_VALUE)
					*pBufIn = MAX_ES_TARGET_VALUE;
//				if ((Value < MIN_ES_TARGET_VALUE) || (Value > MAX_ES_TARGET_VALUE))
//					return FALSE;
//dbg_output("ES_TARGET_VALUE Min:%d, Max:%d\r\n",MIN_ES_TARGET_VALUE,MAX_ES_TARGET_VALUE);
				break;
			case ES_TARGET_PERCENTILE:
				if ((int)*pBufIn < MIN_ES_TARGET_PERCENTILE)
					*pBufIn = MIN_ES_TARGET_PERCENTILE;
				else if ((int)*pBufIn > MAX_ES_TARGET_PERCENTILE)
					*pBufIn = MAX_ES_TARGET_PERCENTILE;
//				if ((Value < MIN_ES_TARGET_PERCENTILE) || (Value > MAX_ES_TARGET_PERCENTILE))
//					return FALSE;
//dbg_output("ES_TARGET_PERCENTILE Min:%d, Max:%d\r\n",MIN_ES_TARGET_PERCENTILE,MAX_ES_TARGET_PERCENTILE);
 				break;
			case ES_TARGET_ACCEPT_GAP:
				if ((int)*pBufIn < MIN_ES_TARGET_ACCEPT_GAP)
					*pBufIn = MIN_ES_TARGET_ACCEPT_GAP;
				else if ((int)*pBufIn > MAX_ES_TARGET_ACCEPT_GAP)
					*pBufIn = MAX_ES_TARGET_ACCEPT_GAP;
//				if ((Value < MIN_ES_TARGET_ACCEPT_GAP) || (Value > MAX_ES_TARGET_ACCEPT_GAP))
//					return FALSE;
//dbg_output("ES_TARGET_ACCEPT_GAP Min:%d, Max:%d\r\n",MIN_ES_TARGET_ACCEPT_GAP,MAX_ES_TARGET_ACCEPT_GAP);
				break;
			case ES_MAX_EXP:
				if ((int)*pBufIn < MIN_ES_MAX_EXP)
					*pBufIn = MIN_ES_MAX_EXP;
				else if ((int)*pBufIn > MAX_ES_MAX_EXP)
					*pBufIn = MAX_ES_MAX_EXP;
//				if ((Value < MIN_ES_MAX_EXP) || (Value > MAX_ES_MAX_EXP))
//					return FALSE;
//dbg_output("ES_MAX_EXP Min:%d, Max:%d\r\n",MIN_ES_MAX_EXP,MAX_ES_MAX_EXP);
				break;
			case ES_MAX_GAIN:
				if ((int)*pBufIn < MIN_ES_MAX_GAIN)
					*pBufIn = MIN_ES_MAX_GAIN;
				else if ((int)*pBufIn > MAX_ES_MAX_GAIN)
					*pBufIn = MAX_ES_MAX_GAIN;
//				if ((Value < MIN_ES_MAX_GAIN) || (Value > MAX_ES_MAX_GAIN))
//					return FALSE;
//dbg_output("ES_MAX_GAIN Min:%d, Max:%d\r\n",MIN_ES_MAX_GAIN,MAX_ES_MAX_GAIN);
				break;
			case ES_FRAME_RATE:
				if ((int)*pBufIn < MIN_ES_FRAME_RATE)
					*pBufIn = MIN_ES_FRAME_RATE;
				else if ((int)*pBufIn > MAX_ES_FRAME_RATE)
					*pBufIn = MAX_ES_FRAME_RATE;
//				if ((Value < MIN_ES_FRAME_RATE) || (Value > MAX_ES_FRAME_RATE))
//					return FALSE;
//dbg_output("ES_FRAME_RATE Min:%d, Max:%d\r\n",MIN_ES_FRAME_RATE,MAX_ES_FRAME_RATE);
				break;
			case ES_CONFORM_TRIES:
				if ((int)*pBufIn < MIN_ES_CONFORM_TRIES)
					*pBufIn = MIN_ES_CONFORM_TRIES;
				else if ((int)*pBufIn > MAX_ES_CONFORM_TRIES)
					*pBufIn = MAX_ES_CONFORM_TRIES;
//				if ((Value < MIN_ES_CONFORM_TRIES) || (Value > MAX_ES_CONFORM_TRIES))
//					return FALSE;
//dbg_output("ES_CONFORM_TRIES Min:%d, Max:%d\r\n",MIN_ES_CONFORM_TRIES,MAX_ES_CONFORM_TRIES);
				break;
			case ES_SPECULAR_EXCLUSION:
				if ((int)*pBufIn < MIN_ES_SPECULAR_EXCLUSION)
					*pBufIn = MIN_ES_SPECULAR_EXCLUSION;
				else if ((int)*pBufIn > MAX_ES_SPECULAR_EXCLUSION)
					*pBufIn = MAX_ES_SPECULAR_EXCLUSION;
//				if (((int)Value < MIN_ES_SPECULAR_EXCLUSION) || ((int)Value > MAX_ES_SPECULAR_EXCLUSION))
//					return FALSE;
//dbg_output("ES_SPECULAR_EXCLUSION Min:%d, Max:%d\r\n",MIN_ES_SPECULAR_EXCLUSION,MAX_ES_SPECULAR_EXCLUSION);
				break;
			case ES_SPECULAR_SAT:
				if ((int)*pBufIn < MIN_ES_SPECULAR_SAT)
					*pBufIn = MIN_ES_SPECULAR_SAT;
				else if ((int)*pBufIn > MAX_ES_SPECULAR_SAT)
					*pBufIn = MAX_ES_SPECULAR_SAT;
//				if ((Value < MIN_ES_SPECULAR_SAT) || (Value > MAX_ES_SPECULAR_SAT))
//					return FALSE;
//dbg_output("ES_SPECULAR_SAT Min:%d, Max:%d\r\n",MIN_ES_SPECULAR_SAT,MAX_ES_SPECULAR_SAT);
				break;
			case ES_SPECULAR_LIMIT:
				if ((int)*pBufIn < MIN_ES_SPECULAR_LIMIT)
					*pBufIn = MIN_ES_SPECULAR_LIMIT;
				else if ((int)*pBufIn > MAX_ES_SPECULAR_LIMIT)
					*pBufIn = MAX_ES_SPECULAR_LIMIT;
//				if ((Value < MIN_ES_SPECULAR_LIMIT) || (Value > MAX_ES_SPECULAR_LIMIT))
//					return FALSE;
//dbg_output("ES_SPECULAR_LIMIT Min:%d, Max:%d\r\n",MIN_ES_SPECULAR_LIMIT,MAX_ES_SPECULAR_LIMIT);
				break;
			case ES_FIXED_EXP:
				if ((int)*pBufIn < MIN_ES_FIXED_EXP)
					*pBufIn = MIN_ES_FIXED_EXP;
				else if ((int)*pBufIn > MAX_ES_FIXED_EXP)
					*pBufIn = MAX_ES_FIXED_EXP;
//				if ((Value < MIN_ES_FIXED_EXP) || (Value > MAX_ES_FIXED_EXP))
//					return FALSE;
//dbg_output("ES_FIXED_EXP Min:%d, Max:%d\r\n",MIN_ES_FIXED_EXP,MAX_ES_FIXED_EXP);
				break;
			case ES_FIXED_GAIN:
				if ((int)*pBufIn < MIN_ES_FIXED_GAIN)
					*pBufIn = MIN_ES_FIXED_GAIN;
				else if ((int)*pBufIn > MAX_ES_FIXED_GAIN)
					*pBufIn = MAX_ES_FIXED_GAIN;
//				if ((Value < MIN_ES_FIXED_GAIN) || (Value > MAX_ES_FIXED_GAIN))
//					return FALSE;
//dbg_output("ES_FIXED_GAIN Min:%d, Max:%d\r\n",MIN_ES_FIXED_GAIN,MAX_ES_FIXED_GAIN);
				break;
			case ES_FIXED_FRAME_RATE:
				if ((int)*pBufIn < MIN_ES_FIXED_FRAME_RATE)
					*pBufIn = MIN_ES_FIXED_FRAME_RATE;
				else if ((int)*pBufIn > MAX_ES_FIXED_FRAME_RATE)
					*pBufIn = MAX_ES_FIXED_FRAME_RATE;
//				if ((Value < MIN_ES_FIXED_FRAME_RATE) || (Value > MAX_ES_FIXED_FRAME_RATE))
//					return FALSE;
//dbg_output("ES_FIXED_FRAME_RATE Min:%d, Max:%d\r\n",MIN_ES_FIXED_FRAME_RATE,MAX_ES_FIXED_FRAME_RATE);
				break;
			/* Booleans are never really out of range.  Non-zero = TRUE, Zero = FALSE */
		    case ES_FAST_START:
                *pBufIn = *pBufIn ? TRUE : FALSE;
                break;
			case ES_ADJUST_EXP_WHEN_ILLUM_OFF:
				break;
			case ES_STARTING_EXP:
				break;
			case ES_STARTING_GAIN:
				break;
			case ES_CONFORM_IMAGE:
				break;
			default:
				break;
		}
		pBufIn++;
	}
 	return TRUE;

}


//#define SET_SETTINGS_DEBUG    1            //THESE DO NOT ALLOW ME TO DOWNLOAD MOC FILE WITH DEBUG ON, SO DO NOT ALLOW FOR RIGHT NOW //lmc debug
#ifdef  SET_SETTINGS_DEBUG
    #define SETTINGS_DBG_OUTPUT     DBG_OUTPUT
    #define SETTINGS_DBG_OUTPUT1     DBG_OUTPUT1
#else
    #define SETTINGS_DBG_OUTPUT(a);
    #define SETTINGS_DBG_OUTPUT1(a,b);
#endif

/*-------------------------------------------------------------------
 * ScnDrv_SetExposureSettings - Search through the passed in array
 * and set the appropriate parameters in the scan driver open context
 * structure.
 *-------------------------------------------------------------------*/
BOOL ScnDrv_SetExposureSettings(ScanDriverOpenContext_t *pSDOpenContext,
							   DWORD *pBufIn, DWORD dwLenIn)
{
	DWORD Tag, Value, x;
	BOOL bCleanupNeeded;
    SETTINGS_DBG_OUTPUT("HHPSD_SetExposureSettings\r\n");

	// Do a quick range check on the settings passed in. Eventually
	// we will make this camera specific at the hardware level.
	if (!(CheckRanges(pBufIn, dwLenIn)))
	{
	    SETTINGS_DBG_OUTPUT("Exposure settings RangeCheck() fail\r\n");
		return FALSE;
	}

	bCleanupNeeded = ScanDriverPrepareChangeConfig(pSDOpenContext);
	for (x=0; x<((dwLenIn-sizeof(DWORD))/(sizeof(DWORD))); x+=2)
	{
		Tag = *pBufIn++;	Value =  *pBufIn++;
		switch (Tag)
		{
			case ES_EXPOSURE_METHOD:
			    DBGM(16,) dbg_output("Tag=ES_EXPOSURE_METHOD Value=%d\r\n",Value);
				pSDOpenContext->AutoExpOptions.MethodToUse = (HHPScanDriverHHPExposureMethod)Value;
				break;
			case ES_TARGET_VALUE:
			    SETTINGS_DBG_OUTPUT1("Tag=ES_TARGET_VALUE Value=%d\r\n",Value);
				pSDOpenContext->AutoExpOptions.nTargetValue = Value;
				break;
			case ES_TARGET_PERCENTILE:
			    SETTINGS_DBG_OUTPUT1("Tag=ES_TARGET_PERCENTILE Value=%d\r\n",Value);
				pSDOpenContext->AutoExpOptions.nTargetPercentile = Value;
				break;
			case ES_TARGET_ACCEPT_GAP:
			    SETTINGS_DBG_OUTPUT1("Tag=ES_TARGET_ACCEPT_GAP Value=%d\r\n",Value);
				pSDOpenContext->AutoExpOptions.nTargetAcceptanceGap = Value;
				break;
			case ES_MAX_EXP:
				DBGM(16,) dbg_output("Tag=ES_MAX_EXP Value=%d\r\n",Value);
				pSDOpenContext->AutoExpOptions.nMaximumExposure = Value;
				break;
			case ES_MAX_GAIN:
			    DBGM(16,) dbg_output("Tag=ES_MAX_GAIN Value=%d\r\n",Value);
				pSDOpenContext->AutoExpOptions.nMaximumGain = Value;
				break;
			case ES_STARTING_EXP:
//			    DBG_OUTPUT1("Tag=ES_STARTING_EXP Value=%d\r\n",Value);
//				pSDOpenContext->AutoExpOptions.nStartingExposure = Value;
				break;
			case ES_STARTING_GAIN:
//			    DBG_OUTPUT1("Tag=ES_STARTING_GAIN Value=%d\r\n",Value);
//				pSDOpenContext->AutoExpOptions.nStartingGain = Value;
				break;
			case ES_FRAME_RATE:
		   		SETTINGS_DBG_OUTPUT1("Tag=ES_FRAME_RATE Value=%d\r\n",Value);
				pSDOpenContext->AutoExpOptions.nFrameRate = Value;
				break;
			case ES_CONFORM_IMAGE:
			    SETTINGS_DBG_OUTPUT1("Tag=ES_CONFORM_IMAGE Value=%d\r\n",Value);
				pSDOpenContext->AutoExpOptions.bImageMustConform = Value;
				break;
			case ES_CONFORM_TRIES:
			    SETTINGS_DBG_OUTPUT1("Tag=ES_CONFORM_TRIES Value=%d\r\n",Value);
				pSDOpenContext->AutoExpOptions.nTriesForConforming = Value;
				break;
			case ES_SPECULAR_EXCLUSION:
			    SETTINGS_DBG_OUTPUT1("Tag=ES_SPECULAR_EXCLUSION Value=%d\r\n",Value);
				pSDOpenContext->AutoExpOptions.nSpecularExclusion = (HHPScanDriverHHPExposureSpecularExclusion)Value;
				break;
			case ES_SPECULAR_SAT:
			    SETTINGS_DBG_OUTPUT1("Tag=ES_SPECULAR_SAT Value=%d\r\n",Value);
				pSDOpenContext->AutoExpOptions.nSpecialSpecularSaturation = Value;
				break;
			case ES_SPECULAR_LIMIT:
			    SETTINGS_DBG_OUTPUT1("Tag=ES_SPECULAR_LIMIT Value=%d\r\n",Value);
				pSDOpenContext->AutoExpOptions.nSpecialSpecularLimit = Value;
				break;
			case ES_ADJUST_EXP_WHEN_ILLUM_OFF:
			    SETTINGS_DBG_OUTPUT1("Tag=ES_ADJUST_EXP_WHEN_ILLUM_OFF=%d\r\n",Value);
				pSDOpenContext->AutoExpOptions.bAdjustExpWhenIllumOff = Value;
				break;
			case ES_FIXED_EXP:
			    DBGM(16, ) dbg_output("Tag=ES_FIXED_EXP Value=%d\r\n",Value);
				pSDOpenContext->FixedExpOptions.nFixedExposure = Value;
				break;
			case ES_FIXED_GAIN:
			    DBGM(16, ) dbg_output("Tag=ES_FIXED_GAIN Value=%d\r\n",Value);
				pSDOpenContext->FixedExpOptions.nFixedGain = Value;
				break;
			case ES_FIXED_FRAME_RATE:
			    SETTINGS_DBG_OUTPUT1("Tag=ES_FIXED_FRAME_RATE Value=%d\r\n",Value);
				pSDOpenContext->FixedExpOptions.nFrameRate = Value;
				break;
			case ES_DECODE_PARAMETER:
			    SETTINGS_DBG_OUTPUT1("Tag=ES_DECODE_PARAMETER Value=%d\r\n",Value);
				pSDOpenContext->AutoExpOptions.nDecodeParameter = Value;
				pSDOpenContext->AutoExpOptions.bDecodeParameterSet = TRUE;
				break;
			case ES_FAST_START:
				if (pSDOpenContext->AutoExpOptions.bFastStart != Value)
					DBG(== 3) dbg_output("Tag=ES_FAST_START Value=%d\r\n",Value);
				pSDOpenContext->AutoExpOptions.bFastStart = Value;
				break;
			case ES_EXPOSURE_CELL:
				if (pSDOpenContext->AutoExpOptions.bFastStart != Value)
					DBG(== 3) dbg_output("Tag=ES_EXPOSURE_CELL Value=%d\r\n",Value);
				pSDOpenContext->AutoExpOptions.nExposureCell = Value;
				break;

			default:
			    SETTINGS_DBG_OUTPUT("Tag not found\r\n");
				break;
		}
	}
	if (bCleanupNeeded) ScanDriverCleanupChangeConfig(pSDOpenContext);
 	return TRUE;
}

/*-------------------------------------------------------------------
 * ScnDrv_SetExposureSettings - Search through the passed in array
 * and set the appropriate parameters in the scan driver open context
 * structure.
 *-------------------------------------------------------------------*/
BOOL ScnDrv_GetExposureSettings(ScanDriverOpenContext_t *pSDOpenContext,DWORD *pBufIn, DWORD dwLenIn)
{
	DWORD x;
    SETTINGS_DBG_OUTPUT("HHPSD_GetExposureSettings\r\n");
	
	for (x=0; x<((dwLenIn-sizeof(DWORD))/(sizeof(DWORD))); x+=2)
	{
		DWORD Tag = *pBufIn++;
        DWORD Value = (DWORD)-1;
		switch (Tag)
		{
			case ES_EXPOSURE_METHOD:
				Value = pSDOpenContext->AutoExpOptions.MethodToUse;
			    SETTINGS_DBG_OUTPUT1("Tag=ES_EXPOSURE_METHOD Value=%d\r\n",Value);
				break;
			case ES_TARGET_VALUE:
				Value = pSDOpenContext->AutoExpOptions.nTargetValue;
			    SETTINGS_DBG_OUTPUT1("Tag=ES_TARGET_VALUE Value=%d\r\n",Value);
				break;
			case ES_TARGET_PERCENTILE:
				Value = pSDOpenContext->AutoExpOptions.nTargetPercentile;
			    SETTINGS_DBG_OUTPUT1("Tag=ES_TARGET_PERCENTILE Value=%d\r\n",Value);
				break;
			case ES_TARGET_ACCEPT_GAP:
				Value = pSDOpenContext->AutoExpOptions.nTargetAcceptanceGap;
			    SETTINGS_DBG_OUTPUT1("Tag=ES_TARGET_ACCEPT_GAP Value=%d\r\n",Value);
				break;
			case ES_MAX_EXP:
				Value = pSDOpenContext->AutoExpOptions.nMaximumExposure;
			    SETTINGS_DBG_OUTPUT1("Tag=ES_MAX_EXP Value=%d\r\n",Value);
				break;
			case ES_MAX_GAIN:
				Value = pSDOpenContext->AutoExpOptions.nMaximumGain;
			    SETTINGS_DBG_OUTPUT1("Tag=ES_MAX_GAIN Value=%d\r\n",Value);
				break;
			case ES_STARTING_EXP:
				Value = pSDOpenContext->AutoExpOptions.nStartingExposure;
			    SETTINGS_DBG_OUTPUT1("Tag=ES_STARTING_EXP Value=%d\r\n",Value);
				break;
			case ES_STARTING_GAIN:
				Value = pSDOpenContext->AutoExpOptions.nStartingGain;
			    SETTINGS_DBG_OUTPUT1("Tag=ES_STARTING_GAIN Value=%d\r\n",Value);
				break;
			case ES_FRAME_RATE:
				Value = pSDOpenContext->AutoExpOptions.nFrameRate;
			    SETTINGS_DBG_OUTPUT1("Tag=ES_FRAME_RATE Value=%d\r\n",Value);
				break;
			case ES_CONFORM_IMAGE:
				Value = pSDOpenContext->AutoExpOptions.bImageMustConform;
			    SETTINGS_DBG_OUTPUT1("Tag=ES_CONFORM_IMAGE Value=%d\r\n",Value);
				break;
			case ES_CONFORM_TRIES:
				Value = pSDOpenContext->AutoExpOptions.nTriesForConforming;
			    SETTINGS_DBG_OUTPUT1("Tag=ES_CONFORM_TRIES Value=%d\r\n",Value);
				break;
			case ES_SPECULAR_EXCLUSION:
				Value = pSDOpenContext->AutoExpOptions.nSpecularExclusion;
			    SETTINGS_DBG_OUTPUT1("Tag=ES_SPECULAR_EXCLUSION Value=%d\r\n",Value);
				break;
			case ES_SPECULAR_SAT:
				Value = pSDOpenContext->AutoExpOptions.nSpecialSpecularSaturation;
			    SETTINGS_DBG_OUTPUT1("Tag=ES_SPECULAR_SAT Value=%d\r\n",Value);
				break;
			case ES_SPECULAR_LIMIT:
				Value = pSDOpenContext->AutoExpOptions.nSpecialSpecularLimit;
			    SETTINGS_DBG_OUTPUT1("Tag=ES_SPECULAR_LIMIT Value=%d\r\n",Value);
				break;
			case ES_ADJUST_EXP_WHEN_ILLUM_OFF:
				 Value = pSDOpenContext->AutoExpOptions.bAdjustExpWhenIllumOff;
			    SETTINGS_DBG_OUTPUT1("Tag=ES_ADJUST_EXP_WHEN_ILLUM_OFF=%d\r\n",Value);
				break;
			case ES_FIXED_EXP:
				Value = pSDOpenContext->FixedExpOptions.nFixedExposure;
			    SETTINGS_DBG_OUTPUT1("Tag=ES_FIXED_EXP Value=%d\r\n",Value);
				break;
			case ES_FIXED_GAIN:
				Value = pSDOpenContext->FixedExpOptions.nFixedGain;
			    SETTINGS_DBG_OUTPUT1("Tag=ES_FIXED_GAIN Value=%d\r\n",Value);
				break;
			case ES_FIXED_FRAME_RATE:
				Value = pSDOpenContext->FixedExpOptions.nFrameRate;
			    SETTINGS_DBG_OUTPUT1("Tag=ES_FIXED_FRAME_RATE Value=%d\r\n",Value);
				break;
			case ES_EXPOSURE_CELL:
				Value = pSDOpenContext->AutoExpOptions.nExposureCell;
			    SETTINGS_DBG_OUTPUT1("Tag=ES_EXPOSURE_CELL Value=%d\r\n",Value);
				break;
			default:
			    SETTINGS_DBG_OUTPUT("Tag not found\r\n");
				break;
		}
        *pBufIn++ = Value;
	}
 	return TRUE;
}

BOOL ScnDrv_GetEngineType(ScanDriverOpenContext_t *pSDOpenContext,DWORD *pBufOut)
{
	*pBufOut = pSDOpenContext->ScanEngine;
	DBG_OUTPUT("ScnDrv_GetEngineType\r\n");
	return TRUE;
}

DWORD ScnDrv_GetImageAttributes(ScanDriverOpenContext_t *pSDOpenContext,void *pAppBuffer,DWORD *pBufOut, DWORD nLenOut) {

	DWORD i=0;
	ScanBufferRegistry_t *pReg;
	pReg = GetBufferRegisterEntry(pSDOpenContext,pAppBuffer);
	/* Make sure that the app buffer is a registered buffer */
	if (pReg != NULL) {
		/* Make sure that the registered entry, currently has a physical buffer associated with it. */
		if (pReg->pPhysicalScanBuffer != NULL) {
			/* This is kind of not needed, because the fact that the physical buffer was non-NULL tells
			 * us we have it, but we'll make sure that the physical buffer is currently considered
			 * "in use" to double check that we have it */
			if (pReg->pPhysicalScanBuffer->BufferState == SBS_IN_USE) {
				/* Make sure that nLenOut is an even number, if it's odd, cut it back to even */
				nLenOut &= 0xfffffffe;
				while (i < nLenOut) {
					switch (pBufOut[i]) {
						case 	HHPSD_IMAGE_STAT_EXPOSURE:
							pBufOut[++i] = pReg->pPhysicalScanBuffer->LastScanInfo.nExposure;
							break;
						case  HHPSD_IMAGE_STAT_GAIN:
							pBufOut[++i] = pReg->pPhysicalScanBuffer->LastScanInfo.nGain;
							break;
						case  HHPSD_IMAGE_STAT_ILLUM_VALUE:
							pBufOut[++i] = pReg->pPhysicalScanBuffer->LastScanInfo.nIllumValue;
							break;
						case HHPSD_IMAGE_STAT_ILLUM_MAXIMUM:
							pBufOut[++i] = pReg->pPhysicalScanBuffer->LastScanInfo.nIllumMaxValue;
							break;
						case HHPSD_IMAGE_STAT_ILLUM_CLIP:
							pBufOut[++i] = pReg->pPhysicalScanBuffer->LastScanInfo.nIllumClipValue;
							break;
						case HHPSD_IMAGE_STAT_HEIGHT:
							pBufOut[++i] = pReg->pPhysicalScanBuffer->LastScanInfo.nHeight;
							break;
						case HHPSD_IMAGE_STAT_WIDTH:
							pBufOut[++i] = pReg->pPhysicalScanBuffer->LastScanInfo.nWidth;
							break;
						case HHPSD_IMAGE_STAT_IMAGE_NUMBER:
							pBufOut[++i] = pReg->pPhysicalScanBuffer->LastScanInfo.nImageNumber;
							break;
						case HHPSD_IMAGE_STAT_IQ_SCORE:
							pBufOut[++i] = pReg->pPhysicalScanBuffer->LastScanInfo.nIqScore;
							break;
					    case HHPSD_IMAGE_STAT_DECODE_PARAMETER:
							//dbg_output("Get: HHPSD_IMAGE_STAT_DECODE_PARAMETER = %u\r\n", pReg->pPhysicalScanBuffer->LastScanInfo.nDecodeParameter);
							pBufOut[++i] = pReg->pPhysicalScanBuffer->LastScanInfo.nDecodeParameter;
							break;

						default:
							break;
					}
					i++;
				}
			}
		}
	}
	return i;
}

/*-----------------------------------------------------------------------
  This function looks at an open context and returns the frame rate
  the client has selected.

  INPUT:
  pOptions - Open context.

  RETURNS:
  The frame rate the client has selected.
  -----------------------------------------------------------------------*/
unsigned GetFrameRate(ScanDriverOpenContext_t * pOptions)
{
	unsigned frame_rate;
	if ((pOptions->nAutoExposureMode == HHPSD_AUTOEXPOSURE_USE_HHP)||(pOptions->nAutoExposureMode == HHPSD_AUTOEXPOSURE_HHP_PRESENTATION))
		frame_rate = pOptions->AutoExpOptions.nFrameRate;
	else if (pOptions->nAutoExposureMode == HHPSD_EXPOSURE_FIXED)
		frame_rate = pOptions->FixedExpOptions.nFrameRate;
	else
		frame_rate = 30;

	return frame_rate;
}

#ifdef SD_NAMESPACE
   } // namespace SD_NAMESPACE {
#endif
