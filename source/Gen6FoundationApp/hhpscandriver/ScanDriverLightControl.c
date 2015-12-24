/*------------------------------------------------------------------

*-------------------------------------------------------------------
* ScanDriverLightControl.c - This file contains the functions
* associated with controlling the illumination/aimer of a given
* imaging engine
*-------------------------------------------------------------------
* $RCSfile: ScanDriverLightControl.c $
* $Revision: 1.45.1.41.2.2 $
* $Date: 2011/08/25 01:34:15EDT $
*--------------------------------------------------------------------
*--------------------------------------------------------------------*/
#if defined(_WIN32_WCE) || defined(PALMOS)
#include <windows.h>
#else // DBALL

#include <stdlib.h>
#include "language.h"
#include "platform.h"

#endif

#include "ScanDriverInterface.h"
#include "ScanDriverLightControl.h"
#include "ScanDriverScannerSelect.h"
#include "ScanDriverScanManager.h"
#include "ScanDriverGlobalConfig.h"
#include "ScanDriverEngine.h"
#include "ScanDriverDebug.h"
#include "ScanDriverMisc.h"

#include "HardwareToken.h"

/// @todo Remove this #include "Flash_illumination_control.h".  It was
/// added to import SetILL_FlashMode() prototype.  However, we
/// shouldn't call this directly.  Should go through the HW layer to
/// adjust flash mode.
#include "Flash_illumination_control.h" 


#define PSOCLESS 1

#define ELEMENTSOF(x) (sizeof(x)/sizeof(*(x)))
#define ELEMENT_OFFSET(t,e) (size_t(&((t *)0)->e))
#define ELEMENT_POINTER(type,pointer,offset) \
               ((type *)((char *)(pointer) + (offset)))

static BOOL  volatile bCurrentIlluminationState			= FALSE;
// bRequestedAimerState describes the requested state of the aimer.
// bCurrentAimerState describes the current state of the aimer.
// Most of the time these two varibles have the same value.  If
// gDriverConfigs.bPreventSimultaneousIlluminationAndAimer is TRUE, it
// is possible for application to request that the aimer is on but
// this module may override the request to insure that aimers and
// illumination are not on at the same time.  In this case,
// bRequestedAimerState will be TRUE, and bCurrentAimerState will be
// FALSE.
static BOOL  volatile bCurrentAimerState					= FALSE;
static BOOL  volatile bRequestedAimerState					= FALSE;
static DWORD nCurrentIlluminationIntensity		= 100;
static DWORD nCurrentAimerIntensity				= 100;
static BOOL	 bCurrentAllowIllumIntensityToAdjust= FALSE;
static BOOL	 bCurrentAllowAimerIntensityToAdjust= FALSE;
static BOOL	 bCurrentIllumIntensityToAdjustDown = TRUE;
static BOOL	 bCurrentAimerIntensityToAdjustDown = TRUE;
static bool g_bEnableFlashCurrent = false;
static bool g_cellphone_flag = false;
extern bool AimOnstatus=0;



static void (*pfAutoAimControlHighPower)(DWORD) = NULL;
static void (*pfAutoAimControlLowPower)(DWORD)  = NULL;

struct ModeMap_t
{
	DWORD origMode;
	DWORD newMode;
};

static DWORD mapModeFunction(DWORD origMode, const ModeMap_t modeMap[],
							 unsigned cnt)
{
	const ModeMap_t * end = modeMap + cnt;
	while (modeMap < end)
	{
		if (modeMap->origMode == origMode)
			return modeMap->newMode;
		++modeMap;
	}
	return origMode;
}

#define mapMode(origMode,modeMap) \
   mapModeFunction((origMode),(modeMap),sizeof(modeMap)/sizeof((modeMap)[0]))

void SetupAutoAimControlFunctions(void (*pHigh)(DWORD),void (*pLow)(DWORD)) {
	pfAutoAimControlHighPower = pHigh;
	pfAutoAimControlLowPower = pLow;
}

void LightControlInitialize(void) {
	HWLAYER.AimerConfig(nCurrentAimerIntensity);
	HWLAYER.IlluminationConfig(nCurrentIlluminationIntensity);
	HWLAYER.AimerOff();
  	HWLAYER.IlluminationOff();
}

void GetCurrentLightsStates(BOOL *pIllState,BOOL *pAimState) {
	*pIllState = bCurrentIlluminationState;
	*pAimState = bRequestedAimerState;
}

void LightControlAutomaticAimerControl(DWORD nLines)
{
	ScanDriverOpenContext_t * pMasterContext = ScanDriverScanningMasterMode();

	// This function should not be called unless someone is scanning and
	// we are in master mode.
	SDASSERT(pMasterContext);

	if (pMasterContext == 0)
		return;

	if (pMasterContext->LightingOptions.bSoftwareControlledAimer == FALSE)
		return;
	if (bCurrentIlluminationState == 0)
		return;
	if (bRequestedAimerState == 0)
		return;

	if (   pMasterContext->LightingOptions.nScanLightMode != HHPSD_LM_CONCURRENT
		   || gDriverConfigs.bPreventSimultaneousIlluminationAndAimer  )
	{
		pfAutoAimControlLowPower(nLines);
	}
	else
		pfAutoAimControlHighPower(nLines);
}


// Translates an open context lighting mode to a mode that obey's
// gDriverConfigs.bPreventSimultaneousIlluminationAndAimer.
// If gDriverConfigs.bPreventSimultaneousIlluminationAndAimer is true
// the platform does not support turning the aimers and illumination on
// at the same time.
static DWORD TranslateMode(DWORD lightMode)
{
//	DWORD lightMode = pOpenContext->LightingOptions.nScanLightMode;
	if (gDriverConfigs.bPreventSimultaneousIlluminationAndAimer)
	{
		ModeMap_t preventSimultaneousModeMap[] =
			{
				{ HHPSD_LM_ILLUM_ON_AIM_ON, HHPSD_LM_ILLUM_ON },
				{ HHPSD_LM_CONCURRENT, HHPSD_LM_INTERLACED },
			};
		
		lightMode = mapMode(lightMode, preventSimultaneousModeMap);
	}
	
	return lightMode;
}

// This function is called when the lighting options in the open
// context changes, or when the master open context changes.  It will
// make the changes necessary to make the lighting conform to the open
// context settings.

void LightControlOpenContextChanged(ScanDriverOpenContext_t *pOpenContext)
{
	ScanDriverOpenContext_t * pMasterContext = ScanDriverScanningMasterMode();
	if (pOpenContext != 0 && pOpenContext == pMasterContext)
	{
		ScanDriverLightingOptions_t &lightOptions 
			= pOpenContext->LightingOptions;
		BOOL bAimReallyOn = lightOptions.bAimerOn();
		if (   lightOptions.bIlluminationOn()
			   && gDriverConfigs.bPreventSimultaneousIlluminationAndAimer   )
		{
			bAimReallyOn = FALSE;
		}
		if (lightOptions.bSoftwareControlledAimer)
			bAimReallyOn = FALSE;

		DWORD lightMode = TranslateMode(lightOptions.nScanLightMode);
		DWORD illumIntensity = lightOptions.nIlluminationIntensity;

		if (lightOptions.nIlluminationControl)
		{
			HW_light_control * p_lc = HHPSD.getLightController();

			if (p_lc)
			{
//#if defined(OEM_ENGINE)
				if(HWLAYER.IsPsocSupported())
				{
					HW_light_control_OEM_Jade * p_lc_oem_jade;
					p_lc_oem_jade = dynamic_cast<HW_light_control_OEM_Jade *>(p_lc);

					if (p_lc_oem_jade)
					{
						if( p_lc_oem_jade->CurrentBoostIntensityLevel() == 2 )
						{
						 	illumIntensity = 150;
						}
						else if( p_lc_oem_jade->CurrentBoostIntensityLevel() == 1 )
						{
						 	illumIntensity = 120;
						}
						else
						{
							illumIntensity = 100;
						}						 						
					}
				}
				else
//#endif
				{ 					   
					HW_light_control_MT9V022 * p_lc_mt9v022;
					p_lc_mt9v022 = dynamic_cast<HW_light_control_MT9V022 *>(p_lc);

					if (p_lc_mt9v022)
					{					
						illumIntensity = p_lc_mt9v022->boostOn() ? 150 : 100;
					}
				}
			}
		}
		DBG(== 3) dbg_output("g_bEnableFlashCurrent:0x%x,  illumIntensity: %d\r\n",g_bEnableFlashCurrent, illumIntensity);

		if (! g_bEnableFlashCurrent)
		{
			illumIntensity = min<DWORD>(100, illumIntensity);
		}
		else
		{
		 	if( HWLAYER.IsPsocSupported() && (illumIntensity > lightOptions.nIlluminationIntensity) )   //if PSOC , do not allow setting above ioctrl set value
			{
				illumIntensity = lightOptions.nIlluminationIntensity;
			}
		}
				
		DBG(== 3) dbg_output("setHwLightMode from %u\r\n", __LINE__);
		HHPSD.setHwLightMode(  lightMode,
							   illumIntensity,
							   lightOptions.nAimerIntensity,
							   lightOptions.nIlluminationSource  );

		bCurrentIlluminationState = lightOptions.bIlluminationOn();
		bRequestedAimerState	  = lightOptions.bAimerOn();
		bCurrentAimerState        = bAimReallyOn;

		HHPSD.LightSettingsUpdate(pOpenContext);
	}
	else if (pMasterContext == 0)
		LightControlEnableGlobalControl();
}


/* Maps the old-style OpenContext light control options and
   bPreventSimultaneiousIlluminationAndAimer into the appropriate
   lighting mode.

       IlluminationState
       |       AimerState
       |	   |       AutomaticAimerControl
       |	   |	   |      bPreventSimultaneousIlluminationAndAimer
       |       |       |      |
       |       |       |      |     Description
    ------- ------- ------- ------- ---------------------------------------
    FALSE   FALSE   DC      DC      A. Illumination and aimers are off.
    FALSE   TRUE    DC      DC      B. Illumination off, aimers on.
    TRUE    FALSE   DC      DC      C. Illumination on, aimers off.
    TRUE    DC      FALSE   TRUE    C. Illumination on, aimers off.
    TRUE    TRUE    FALSE   FALSE   D. Illumination and aimers are on.
    TRUE    TRUE    TRUE    FALSE   E. Concurrent mode.
    TRUE    TRUE    TRUE    TRUE    F. Interlaced mode.
*/

BOOL LightControlAutomaticAimerControl(  ScanDriverOpenContext_t *pOpenContext,
										 BOOL bState  )
{
	DWORD &nScanLightMode = pOpenContext->LightingOptions.nScanLightMode;
	DWORD &nIdleLightMode = pOpenContext->LightingOptions.nIdleLightMode;
	BOOL  &bSoftwareControlledAimer = pOpenContext->LightingOptions.bSoftwareControlledAimer;

	bSoftwareControlledAimer = !!bState;
	static ModeMap_t softwareControllTrueModeMap[] =
	{
		{ HHPSD_LM_ILLUM_ON_AIM_ON, HHPSD_LM_CONCURRENT },
	};

	static ModeMap_t softwareControlFalseModeMap[] =
	{
		{ HHPSD_LM_CONCURRENT, HHPSD_LM_ILLUM_ON_AIM_ON },
		{ HHPSD_LM_INTERLACED, HHPSD_LM_ILLUM_ON_AIM_ON },
		/* { HHPSD_LM_FLASH,      HHPSD_LM_ILLUM_ON_AIM_ON }, */
	};

	if (bState)
		nScanLightMode = mapMode(nScanLightMode, softwareControllTrueModeMap);
	else
		nScanLightMode = mapMode(nScanLightMode, softwareControlFalseModeMap);

	nIdleLightMode = nScanLightMode;
	if (nIdleLightMode > HHPSD_LM_ILLUM_ON_AIM_ON)
		nIdleLightMode = HHPSD_LM_ILLUM_ON_AIM_ON;

//	LM_ASSERT(pOpenContext);

	LightControlOpenContextChanged(pOpenContext);

	return TRUE;
}

BOOL LightControlIlluminationState(ScanDriverOpenContext_t *pOpenContext,
								   BOOL bState)
{
	DWORD &nScanLightMode = pOpenContext->LightingOptions.nScanLightMode;
	DWORD &nIdleLightMode = pOpenContext->LightingOptions.nIdleLightMode;
	BOOL  &bSoftwareControlledAimer = pOpenContext->LightingOptions.bSoftwareControlledAimer;

	static ModeMap_t illuminationTrueModeMap[] =
	{
		{ HHPSD_LM_OFF,    HHPSD_LM_ILLUM_ON },
		{ HHPSD_LM_AIM_ON, HHPSD_LM_ILLUM_ON_AIM_ON },
	};
	static ModeMap_t illuminationFalseModeMap[] =
	{
		{ HHPSD_LM_ILLUM_ON,        HHPSD_LM_OFF },
		{ HHPSD_LM_ILLUM_ON_AIM_ON, HHPSD_LM_AIM_ON },
		{ HHPSD_LM_CONCURRENT,      HHPSD_LM_AIM_ON },
		{ HHPSD_LM_INTERLACED,      HHPSD_LM_AIM_ON },
		/* { HHPSD_LM_FLASH,           HHPSD_LM_AIM_ON }, */
	};

	if (bState)
		nScanLightMode = mapMode(nScanLightMode, illuminationTrueModeMap);
	else
		nScanLightMode = mapMode(nScanLightMode, illuminationFalseModeMap);

	if (nScanLightMode == HHPSD_LM_ILLUM_ON_AIM_ON && bSoftwareControlledAimer)
		nScanLightMode = HHPSD_LM_CONCURRENT;

	nIdleLightMode = nScanLightMode;
	if (nIdleLightMode > HHPSD_LM_ILLUM_ON_AIM_ON)
		nIdleLightMode = HHPSD_LM_ILLUM_ON_AIM_ON;

//	LM_ASSERT(pOpenContext);
	SDASSERT(!!bState == pOpenContext->LightingOptions.bIlluminationOn());

	LightControlOpenContextChanged(pOpenContext);
	return TRUE;
}

BOOL LightControlAimerState(ScanDriverOpenContext_t *pOpenContext,BOOL bState)
{
	DWORD &nScanLightMode = pOpenContext->LightingOptions.nScanLightMode;
	DWORD &nIdleLightMode = pOpenContext->LightingOptions.nIdleLightMode;
	BOOL  &bSoftwareControlledAimer = pOpenContext->LightingOptions.bSoftwareControlledAimer;

	static ModeMap_t aimerTrueModeMap[] =
	{
		{ HHPSD_LM_OFF,      HHPSD_LM_AIM_ON },
		{ HHPSD_LM_ILLUM_ON, HHPSD_LM_ILLUM_ON_AIM_ON },
	};
	static ModeMap_t aimerFalseModeMap[] =
	{
		{ HHPSD_LM_AIM_ON,          HHPSD_LM_OFF },
		{ HHPSD_LM_ILLUM_ON_AIM_ON, HHPSD_LM_ILLUM_ON },
		{ HHPSD_LM_CONCURRENT,      HHPSD_LM_ILLUM_ON },
		{ HHPSD_LM_INTERLACED,      HHPSD_LM_ILLUM_ON },
		/* { HHPSD_LM_FLASH,           HHPSD_LM_ILLUM_ON }, */
	};

	if (bState)
		nScanLightMode = mapMode(nScanLightMode, aimerTrueModeMap);
	else
		nScanLightMode = mapMode(nScanLightMode, aimerFalseModeMap);

	if (nScanLightMode == HHPSD_LM_ILLUM_ON_AIM_ON && bSoftwareControlledAimer)
		nScanLightMode = HHPSD_LM_CONCURRENT;

	nIdleLightMode = nScanLightMode;
	if (nIdleLightMode > HHPSD_LM_ILLUM_ON_AIM_ON)
		nIdleLightMode = HHPSD_LM_ILLUM_ON_AIM_ON;

//	LM_ASSERT(pOpenContext);
	SDASSERT(!!bState == pOpenContext->LightingOptions.bAimerOn());

	LightControlOpenContextChanged(pOpenContext);

	return TRUE;
}

static bool SetScanMode(ScanDriverOpenContext_t *pOpenContext,DWORD mode,
				   bool *p_changed)
{
	DWORD &nScanLightMode = pOpenContext->LightingOptions.nScanLightMode;
	BOOL  &bSoftwareControlledAimer
		= pOpenContext->LightingOptions.bSoftwareControlledAimer;

	if (mode >= HHPSD_LM_NUM_MODES)
		return FALSE;

	BOOL bNewSoftwareControlledAimer = (mode > HHPSD_LM_ILLUM_ON_AIM_ON);

	if (  nScanLightMode != mode
		  || bSoftwareControlledAimer != bNewSoftwareControlledAimer  )
	{
		*p_changed = true;
	}

	nScanLightMode = mode;
	bSoftwareControlledAimer = bNewSoftwareControlledAimer;

	return TRUE;
}

static bool SetIdleMode(ScanDriverOpenContext_t *pOpenContext,DWORD mode,
				   bool *p_changed)
{
	DWORD &nIdleLightMode = pOpenContext->LightingOptions.nIdleLightMode;

	if (mode > HHPSD_LM_ILLUM_ON_AIM_ON)
		return FALSE;

	if (nIdleLightMode != mode)
		*p_changed = true;
	nIdleLightMode = mode;
//	LM_ASSERT(pOpenContext);

	return TRUE;
}

BOOL LightControlSetIlluminationSource(ScanDriverOpenContext_t *pOpenContext, DWORD source)
{
	DWORD &nIlluminationSource = pOpenContext->LightingOptions.nIlluminationSource;

	nIlluminationSource = source;
//	LM_ASSERT(pOpenContext);

	LightControlOpenContextChanged(pOpenContext);

	return TRUE;
}

BOOL LightControlIlluminationIntensity(  ScanDriverOpenContext_t *pOpenContext,
										 DWORD nValue  )
{
	pOpenContext->LightingOptions.nIlluminationIntensity = nValue;
	LightControlOpenContextChanged(pOpenContext);
	return TRUE;
}

BOOL LightControlAimerIntensity(  ScanDriverOpenContext_t *pOpenContext,
								  DWORD nValue  )
{
	pOpenContext->LightingOptions.nAimerIntensity = nValue;
	LightControlOpenContextChanged(pOpenContext);
	return TRUE;
}


/*
  This is called to notify the light control module if the exposure
  is low enough to safely turn on the flash mode current.

  bEnableFlashCurrent - If true, it indicates that the exposure is low
                        enough for flash mode.  If false, it
                        indicatest the exposure is too high.
*/
BOOL LightControlFlashCurrent( bool bFlashExposure, int tint, bool bAllowDeferredBoost )
{
	bool bEnableFlashCurrent = bFlashExposure;
	DBG(== 3) dbg_output("LightControlFlashCurrent( %u, %d) (En,Int) \r\n", bEnableFlashCurrent, tint);

	g_bEnableFlashCurrent = bEnableFlashCurrent;

	HW_light_control * p_lc_base = HHPSD.getLightController();
	SDASSERT(p_lc_base);

	//HW_light_control_OEM_Jade * p_lc = 0;
	HW_light_control_OEM_Jade * p_lc_oem_jade = 0;
	HW_light_control_MT9V022 * p_lc_mt9v022 = 0;

	if (p_lc_base)
	{
		//#if defined(OEM_ENGINE)		//If we are OEM
		if(HWLAYER.IsPsocSupported())
		{
			p_lc_oem_jade = dynamic_cast<HW_light_control_OEM_Jade *>(p_lc_base);
			SDASSERT(p_lc_oem_jade);
		}
		else	  
		//#endif 	   	
		{	
			p_lc_mt9v022 = dynamic_cast<HW_light_control_MT9V022 *>(p_lc_base);
			SDASSERT(p_lc_mt9v022);
		}
	}

	ScanDriverOpenContext_t * pOpenContext = ScanDriverScanningMasterMode();
	SDASSERT(pOpenContext);

	if (pOpenContext == 0)
		return false;

	ScanDriverLightingOptions & lightOptions = pOpenContext->LightingOptions;

	if (bFlashExposure)
	{
		// If we're in a lights mode that doesn't support boost current, 
		// then force boost current off.
		if (  lightOptions.nScanLightMode == HHPSD_LM_ILLUM_ON
			  || lightOptions.nScanLightMode == HHPSD_LM_ILLUM_ON_AIM_ON )
		{
			bEnableFlashCurrent   = FALSE;
			g_bEnableFlashCurrent = FALSE;
		}
		
		// If caller is saying that we can turn on boost, but the 
		// scan driver hasn't been configured to control boost, then
		// there is nothing to do, so return
		//#if defined(OEM_ENGINE)	//If not an OEM

		if(!HWLAYER.IsPsocSupported())
		//#endif
		{
			if( bEnableFlashCurrent && ! lightOptions.nIlluminationControl ) 
			{
				return true;
			}
		}
	}
	else
	{
		// If the exposure is too long for flash, but the lights are off, 
		// then, enable flash.
		if (! lightOptions.bIlluminationOn())
			g_bEnableFlashCurrent = bEnableFlashCurrent = TRUE;
	}

	//#if defined(OEM_ENGINE)	//If not an OEM

	if( (!HWLAYER.IsPsocSupported()) && p_lc_mt9v022 )
	{
	//#endif
		// If flash current already set correctly, return early.
		if (p_lc_mt9v022 && !bEnableFlashCurrent == !p_lc_mt9v022->boostOn())
			return true;

		DWORD lightMode = TranslateMode(lightOptions.nScanLightMode);

		DWORD illumIntensity = 100;

		// If the flash current is off (boostOn() return false), this function
		// can't turn flash current back on.
		if (bEnableFlashCurrent && p_lc_mt9v022 && p_lc_mt9v022->boostOn())
			illumIntensity = 150;
		
		DBG(== 3) dbg_output("setHwLightMode from %u\r\n", __LINE__);
		return p_lc_mt9v022->setHwLightMode(lightMode,
								illumIntensity,
								lightOptions.nAimerIntensity,
								lightOptions.nIlluminationSource  );

	//#if defined(OEM_ENGINE)		//else if we are OEM
	}
	else if( (HWLAYER.IsPsocSupported()) && p_lc_oem_jade )
	{
		int currentBoost = p_lc_oem_jade->CurrentBoostIntensityLevel();
		int newBoost = p_lc_oem_jade->SelectBoostIntensityLevel( tint );
		int currentBoostPercent = p_lc_oem_jade->CurrentBoostIntensityPercentLevel();

		// If flash current already set correctly, return early.
		if( currentBoost == newBoost )
		{
			return true;
		}

		// If the flash current is already low ( 100% ), this function
		// can't turn flash current back on.
		if( (currentBoost == 0) )
		{
			return true;
		}

		DWORD lightMode = TranslateMode(lightOptions.nScanLightMode);

		DWORD illumIntensity = 100;    // when exposure >2ms, we switch to low current.       //lmc was 3.5ms, now 2 for heat mit

		if( newBoost == 2 )            //switching from Medium Current mode back to High Current mode , or set high current for exposure < 500us
		{
		 	illumIntensity = 150;
			
			if( (tint >= 16) && (currentBoost == 1) )    //if already in medium boost, and if not setting a real low exposure, keep meduim boost 
			{                           					//This adds histerisis so we do not flicker between boost modes                  
				illumIntensity = 120;	//stay in meduim boost
				return true;
			}				   
		}			
		else if( newBoost == 1 )       // when exposure>0.5ms WE switch to medium current
		{
			illumIntensity = 120;	
		}

		if (! g_bEnableFlashCurrent)                 //if boost not allowed
		{
			illumIntensity = min<DWORD>(100, illumIntensity);
		}
		else
		{
		 	if( (illumIntensity > lightOptions.nIlluminationIntensity) )   //if PSOC , do not allow setting above ioctrl set value
			{				 
				illumIntensity = lightOptions.nIlluminationIntensity;
				int limitBoost = p_lc_oem_jade->FindBoostIntensityLevel( illumIntensity );

				// If flash current already set correctly, return early.
				if( currentBoost == limitBoost )
				{
					return true;
				}
			}
		}

		if( ((illumIntensity > currentBoostPercent) || (currentBoostPercent == 150)) && bAllowDeferredBoost )	//if we are boosting higher, or at highest boost (150%), do not do it till we are in vert sync, because do not want it changed in current frame.
		{
			p_lc_oem_jade->SetBoostDeferred();				   
		}
		
		DBG(== 3) dbg_output("LightControlFlashCurrent( %u, %d) (En,Int) \r\n", bEnableFlashCurrent, tint);
		DBG(== 3) dbg_output("setHwLightMode from %u\r\n", __LINE__);

		return p_lc_oem_jade->setHwLightMode(lightMode,
								illumIntensity,
								lightOptions.nAimerIntensity,
								lightOptions.nIlluminationSource  );		

		//return p_lc_oem_jade->flashCurrent( bEnableFlashCurrent, tint );
	}
	//#endif
	return false;
}


BOOL LightControlEnableGlobalControl(void) {
	DWORD nTempIll=0,nTempAim=0;
	DWORD nIllSource = HHPSD_IS_ALTERNATE;
	BOOL bAim = FALSE, bIll = FALSE;
	ScanDriverOpenContext_t *pTempOC=NULL;

	/* The light settings when we are in global control mode is that we
	 * use the highest of all of the open contexts.  Therefore if one open
	 * context has the illumination on, and another does not, it will be
	 * turned on.  Also, if one context has the aimer on at 50% and another
	 * has it on at 70%, we'll put it on at 70%. */
	pTempOC = GetOpenContextEntry(pTempOC);
	ScanDriverLightingOptions_t &lightOptions = pTempOC->LightingOptions;
	
	while (pTempOC != NULL)
	{
		DWORD nIdleLightMode = lightOptions.nIdleLightMode;

		if (nIdleLightMode & HHPSD_LM_AIM_ON)
		{
			bAim = TRUE;
			nTempAim = max(nTempAim, lightOptions.nAimerIntensity);
		}

		if (lightOptions.nIlluminationControl == TRUE)
		{
			nTempIll = 150;
		}
		else 
		{			
			nTempIll = max(nTempIll, lightOptions.nIlluminationIntensity);

			if (nIdleLightMode & HHPSD_LM_ILLUM_ON)
			{
				bIll = TRUE;

				if (lightOptions.nIlluminationSource == HHPSD_IS_PRIMARY)
					nIllSource = HHPSD_IS_PRIMARY;
			}
		}
		pTempOC = GetOpenContextEntry(pTempOC);
	}

	if (bIll == FALSE)
		nIllSource = HHPSD_IS_PRIMARY;

	//  spd per power OFF functionalities.  Current concept is that only those things that will talk to
	// sensor will power device back up if needed.  Currently, this SHOULD only be extraneous lights control (so, here)
	// and request of images, which all go through common initializer.
	if ((bIll || bAim) && IsScanDriverPoweredOff())
	{
		if (bIll)
			bCurrentIlluminationState = FALSE;
		if (bAim)
			bRequestedAimerState = FALSE;

		ScanDriverPowerOn();
	}

	BOOL bAimReallyOn = bAim;
	DWORD lightMode = HHPSD_LM_OFF;

	if (bIll && bAim && gDriverConfigs.bPreventSimultaneousIlluminationAndAimer)
	{
		bAimReallyOn = FALSE;
		lightMode = HHPSD_LM_ILLUM_ON;
	}
	else
	{
		if (bIll)
			lightMode = lightMode | HHPSD_LM_ILLUM_ON;
		if (bAim)
			lightMode = lightMode | HHPSD_LM_AIM_ON;
	}

	if( HWLAYER.IsPsocSupported() )   
	{
		if (! g_bEnableFlashCurrent )
		{
			nTempIll = min<DWORD>(100, nTempIll);
		}
		else
		{
		 	if( (nTempIll > lightOptions.nIlluminationIntensity) )   //if PSOC , do not allow setting above ioctrl set value
			{
				nTempIll = lightOptions.nIlluminationIntensity;
			}
		}
	}

	DBG(== 3) dbg_output("setHwLightMode from %u\r\n", __LINE__);
	HHPSD.setHwLightMode(lightMode, nTempIll, nTempAim, nIllSource);

	bCurrentIlluminationState = bIll;
	bRequestedAimerState	  = bAim;
	bCurrentAimerState        = bAimReallyOn;


	bCurrentAllowIllumIntensityToAdjust	= FALSE;
	bCurrentAllowAimerIntensityToAdjust	= FALSE;
	return 1;
}


BOOL LightControlEnableIndividualControl(ScanDriverOpenContext_t *pOpenContext)
{
	LightControlOpenContextChanged(pOpenContext);

	bCurrentAllowIllumIntensityToAdjust	= pOpenContext->LightingOptions.bAllowIllumIntensityToAdjust;
	bCurrentAllowAimerIntensityToAdjust	= pOpenContext->LightingOptions.bAllowAimerIntensityToAdjust;
	bCurrentIllumIntensityToAdjustDown	= pOpenContext->LightingOptions.bAdjustIllumIntensityDown;
	bCurrentAimerIntensityToAdjustDown	= pOpenContext->LightingOptions.bAdjustAimerIntensityDown;
	return 1;
}


BOOL LightControlAutomaticIlluminationAdjustment(DWORD nNewIllumValue) {
	BOOL bStatus = FALSE;
	if (bCurrentIlluminationState && bCurrentAllowIllumIntensityToAdjust) {
		if (bCurrentIllumIntensityToAdjustDown == TRUE) {
			if ((nNewIllumValue >= LIGHTS_CONTROL_MINIMUM_ILLUM_SETTING)&&
				(nNewIllumValue <= nCurrentIlluminationIntensity)) {
				HWLAYER.IlluminationConfig(nNewIllumValue);
				bStatus = TRUE;
			}
		} else {
			if ((nNewIllumValue <= LIGHTS_CONTROL_MAXIMUM_ILLUM_SETTING)&&
				(nNewIllumValue >= nCurrentIlluminationIntensity)) {
				HWLAYER.IlluminationConfig(nNewIllumValue);
				bStatus = TRUE;
			}
		}
	}
	return bStatus;
}


BOOL LightControlAutomaticAimerAdjustment(DWORD nNewAimerValue) {
	BOOL bStatus = FALSE;
	if (bRequestedAimerState && bCurrentAllowAimerIntensityToAdjust) {
		if (bCurrentAimerIntensityToAdjustDown == TRUE) {
			if ((nNewAimerValue >= LIGHTS_CONTROL_MINIMUM_ILLUM_SETTING)&&
				(nNewAimerValue <= nCurrentAimerIntensity)) {
				HWLAYER.AimerConfig(nNewAimerValue);
				bStatus = TRUE;
			}
		} else {
			if ((nNewAimerValue <= LIGHTS_CONTROL_MAXIMUM_ILLUM_SETTING)&&
				(nNewAimerValue >= nCurrentAimerIntensity)) {
				HWLAYER.AimerConfig(nNewAimerValue);
				bStatus = TRUE;
			}
		}
	}
	return bStatus;
}


#ifndef PALMOS

#define TAG_ENTRY(t,e,fp) { #t, (t), ELEMENT_OFFSET(ScanDriverLightingOptions_t,e), fp },

const struct tag_entry_t
{
	const char * name;
	DWORD tag;
	size_t offset;
	bool (*fp_change)(ScanDriverOpenContext_t *pOpenContext,
					  DWORD value, bool *p_changed);
} tag_table[] =
{
	TAG_ENTRY (LI_SCAN_MODE,              nScanLightMode,         SetScanMode)
	TAG_ENTRY (LI_IDLE_MODE,              nIdleLightMode,         SetIdleMode)
	TAG_ENTRY (LI_ILLLUMINATION_SOURCE,   nIlluminationSource,    0)
	TAG_ENTRY (LI_ILLUMINATION_INTENSITY, nIlluminationIntensity, 0)
	TAG_ENTRY (LI_AIMER_INTENSITY,        nAimerIntensity,        0)
	TAG_ENTRY (LI_ILLUMINATION_CONTROL,   nIlluminationControl,   0)
};
#else

struct tag_entry_t
{
	const char * name;
	DWORD tag;
	size_t offset;
	bool (*fp_change)(ScanDriverOpenContext_t *pOpenContext,
					  DWORD value, bool *p_changed);
} tag_table[5];


void InitLightControl(void)
{
#define TAG_ENTRY(t,e,fp)												\
	{ tag_table[n].name = #t;											\
		tag_table[n].tag = (t);											\
		tag_table[n].offset = ELEMENT_OFFSET(ScanDriverLightingOptions_t,e); \
		tag_table[n++].fp_change = fp;									\
	}

	int n = 0;
	TAG_ENTRY (LI_SCAN_MODE,              nScanLightMode,         SetScanMode)
	TAG_ENTRY (LI_IDLE_MODE,              nIdleLightMode,         SetIdleMode)
	TAG_ENTRY (LI_ILLLUMINATION_SOURCE,   nIlluminationSource,    0)
	TAG_ENTRY (LI_ILLUMINAITON_INTENSITY, nIlluminationIntensity, 0)
	TAG_ENTRY (LI_AIMER_INTENSITY,        nAimerIntensity,        0)	
	TAG_ENTRY (LI_ILLUMINATION_CONTROL,   nIlluminationControl,   0)
}
#endif


static const tag_entry_t * find_tag_entry(DWORD tag)
{
	for (unsigned i = 0; i < ELEMENTSOF(tag_table); ++i)
		if (tag_table[i].tag == tag)
			return &tag_table[i];
	return 0;
}


BOOL LightControlSetTags(  ScanDriverOpenContext_t *pOpenContext,
							const DWORD * pBuffIn, DWORD buffSize  )
{
//	dbg_output(  "LightControlSetTags:(0x%X, 0x%X, %u)\r\n",  
//				 pOpenContext, pBuffIn, buffSize         );
	BOOL bStatus = TRUE;
	bool change = false;
	ScanDriverLightingOptions_t * pLighting = &pOpenContext->LightingOptions;
	buffSize /= sizeof(DWORD) * 2;


	for (unsigned i = 0; i < buffSize; ++i)
	{
		DWORD tag = *pBuffIn++;
		DWORD value = *pBuffIn++;

		const tag_entry_t * pEntry = find_tag_entry(tag);

		if (pEntry)
		{
			DBGM(16,) dbg_output("   %s = %u\r\n", pEntry->name, value);
			
			if (pEntry->fp_change == 0)
			{
				DWORD *pElement = ELEMENT_POINTER(  DWORD, pLighting,
													pEntry->offset  );
				if (*pElement != value)
				{
					change = true;
					*pElement = value;
				}
			}
			else
			{
				pEntry->fp_change(pOpenContext, value, &change);
			}
		}
		else
			bStatus = FALSE;
	}

	if (change)
		LightControlOpenContextChanged(pOpenContext);

	return bStatus;
}


BOOL LightControlGetTags(  ScanDriverOpenContext_t *pOpenContext,
						   const DWORD * pBuffIn,
						   DWORD *pBuffOut,
						   DWORD buffSize  )
{
	BOOL bStatus = TRUE;
	ScanDriverLightingOptions_t * pLighting = &pOpenContext->LightingOptions;
	buffSize /= sizeof(DWORD) * 2;


	for (unsigned i = 0; i < buffSize; ++i)
	{
		DWORD tag = *pBuffIn++;
		*pBuffOut++ = tag;

		const tag_entry_t * pEntry = find_tag_entry(tag);

		if (pEntry)
		{
			DWORD *pElement = ELEMENT_POINTER(  DWORD, pLighting,
											  pEntry->offset  );
			DBG_OUTPUT3("Get tag %s at %u to %u\r\n",
					   pEntry->name, pEntry->offset, *pElement);
			*pBuffOut = *pElement;
		}
		else
		{
			*pBuffOut = *pBuffIn;
			bStatus = FALSE;
		}
		++pBuffOut;
		++pBuffIn;
	}

	return bStatus;
}


BOOL
HW_light_control_primitive ::
setHwLightMode(DWORD lm, DWORD nIllVal, DWORD nAimVal, DWORD /*illumSource*/)
{
	BOOL bIllOn = FALSE, bAimOn = FALSE, bAimReallyOn = FALSE;

	switch(lm)
	{
	case HHPSD_LM_AIM_ONLY_NON_EXPOSURE:
	case HHPSD_LM_AIM_ON:
		bAimOn = TRUE;
		bAimReallyOn = TRUE;
		break;
	case HHPSD_LM_BLINK:
	case HHPSD_LM_ILLUM_ON:
		bIllOn = TRUE;
		break;
	case HHPSD_LM_ILLUM_ON_AIM_ON:
		bAimOn = TRUE;
		bAimReallyOn = TRUE;
		bIllOn = TRUE;
		break;
	case HHPSD_LM_INTERLACED:
	case HHPSD_LM_CONCURRENT:
		bAimOn = TRUE;
		bIllOn = TRUE;
		break;
	};


	if (bIllOn && nIllVal != nCurrentIlluminationIntensity)
	{
		HWLAYER.IlluminationConfig(nIllVal);
		nCurrentIlluminationIntensity = nIllVal;
	}
	if (bAimOn && nAimVal != nCurrentAimerIntensity)
	{
		HWLAYER.AimerConfig(nAimVal);
		nCurrentAimerIntensity = nAimVal;
	}
	if (bIllOn && !bCurrentIlluminationState)
		HWLAYER.IlluminationOn();
	if (bAimReallyOn && !bCurrentAimerState)
		HWLAYER.AimerOn();
	if (!bIllOn && (bCurrentIlluminationState || HWLAYER.IllumIsOn()))
		HWLAYER.IlluminationOff();
	if (!bAimReallyOn && (bCurrentAimerState || HWLAYER.AimIsOn()))
		HWLAYER.AimerOff();

	return TRUE;
}


HW_light_control_MT9V022 :: HW_light_control_MT9V022()
	: illumState(0), aimState(0), psocLedModeReg(0),
	  current_mode(HHPSD_LM_OFF),
	  red_current(PSOC_RED_CURRENT_REG, PSOC_RED_OP_MAX_REG, PSOC_FLASH_CURRENT),
	  green_current(PSOC_GREEN_CURRENT_REG, PSOC_GREEN_OP_MAX_REG),
	  aim_current(PSOC_AIM_CURRENT_REG, PSOC_AIM_OP_MAX_REG)
{
//	dbg_output("HW_light_control_MT9V022 :: HW_light_control_MT9V022()\r\n");
	unsigned char *p_regs = (unsigned char *)gp_PsocRegisters;
	psocLedModeReg = p_regs[PSOC_LED_REG];
//	dbg_output("Initial psocLedModeReg = 0x%X\r\n", psocLedModeReg);
}

// GEN6_TODO: No need to include this here.
#include "Hi704A.h"


BOOL 
HW_light_control_MT9V022 ::
boostOn() const
{
	return GetILL_FlashMode(); /// @todo After Larry's changes, check
							   /// bits in psocLedModeReg;
}


BOOL
HW_light_control_MT9V022 ::
setHwLightMode(DWORD lm, DWORD nIllVal, DWORD nAimVal, DWORD illumSource)
{
	DBG(== 3) dbg_output(  "setHwLightMode(%u, %u, %u, %u)\r\n", 
						  lm, nIllVal, nAimVal, illumSource    );

	bool bIllOn = FALSE, bAimOn = FALSE;
	BYTE psocLedMode = 0;
	BYTE psocLedModeMask = PSOC_RED_ILLUM;
	psocLedModeMask=~psocLedModeMask;
	// We don't support flash mode with the green aimers.
/*	if (lm == HHPSD_LM_FLASH && illumSource != HHPSD_IS_PRIMARY)
	lm = HHPSD_LM_INTERLACED; */

	if (illumSource == HHPSD_IS_PRIMARY)
		psocLedMode |= PSOC_RED_ILLUM;

	switch(lm)
	{
	case HHPSD_LM_AIM_ON:
		bAimOn = TRUE;
		break;
	case HHPSD_LM_ILLUM_ON:
		bIllOn = TRUE;
		psocLedModeMask |= PSOC_RED_ILLUM;
		break;
	case HHPSD_LM_ILLUM_ON_AIM_ON:
		bAimOn = TRUE;
		bIllOn = TRUE;
		psocLedModeMask |= PSOC_RED_ILLUM;
		break;
/*	case HHPSD_LM_FLASH:
		psocLedMode |= 0x43;
		psocLedModeMask |= PSOC_RED_ILLUM;
		break; */
	case HHPSD_LM_AIM_ONLY_NON_EXPOSURE:
		psocLedMode |= 0x1;
		psocLedModeMask |= PSOC_RED_ILLUM;
		break;
	case HHPSD_LM_INTERLACED:
		psocLedMode |= 0x3;
		psocLedModeMask |= PSOC_RED_ILLUM;
		break;
	case HHPSD_LM_CONCURRENT:
		psocLedMode |= 0x1;
		bIllOn = TRUE;
		psocLedModeMask |= PSOC_RED_ILLUM;
		break;
	case HHPSD_LM_BLINK:
		psocLedMode |= 0x2;
		psocLedModeMask |= PSOC_RED_ILLUM;
		break;
	};

	if (nIllVal == 0)
	{
		bIllOn = FALSE;
		psocLedMode &= ~2;
	}
	else if (nIllVal > 100)
	{
#ifndef _SMART_SCAN_DRIVER_FLASH_INTEN_CNTRL
		SetILL_FlashMode(1); /// @todo After Larry's changes, set bit on psocLedMode instead.
#else
		psocLedMode |= 0x20;
#endif
	}
	else
	{
#ifndef _SMART_SCAN_DRIVER_FLASH_INTEN_CNTRL
		SetILL_FlashMode(0);
#endif
	}

	if (lm == HHPSD_LM_ILLUM_ON || lm == HHPSD_LM_ILLUM_ON_AIM_ON
		|| lm == HHPSD_LM_INTERLACED || lm == HHPSD_LM_CONCURRENT || lm== HHPSD_LM_BLINK)
	{
		if (illumSource == HHPSD_IS_PRIMARY)
			red_current.setIntensity(nIllVal);
		else
			green_current.setIntensity(nIllVal);
	}
	/* else if (lm == HHPSD_LM_FLASH && current_mode != lm)
	   red_current.setIntensity(100); */

	if (lm == HHPSD_LM_AIM_ON || lm == HHPSD_LM_ILLUM_ON_AIM_ON
		|| lm == HHPSD_LM_INTERLACED || lm == HHPSD_LM_CONCURRENT
		|| lm== HHPSD_LM_AIM_ONLY_NON_EXPOSURE                     )
	{
		aim_current.setIntensity(nAimVal);
	}
	
	if (g_cellphone_flag && (0x2 & psocLedMode & psocLedModeMask) == 0x2)
	{
		psocLedMode     &= ~0x2;
		psocLedMode     |= 0x4;
		psocLedModeMask |=  0x7;
	}
	setLedMode(psocLedMode, psocLedModeMask);
	DBGM(4,) dbg_output("setIllum(%u) setAim(%u)\r\n", !!bIllOn, !!bAimOn);
	setIllum(!!bIllOn);
	setAim(!!bAimOn);

	current_mode = lm;

	// GEN6_TODO: Remove current hack below.
#ifdef HI2D_T
#ifdef BYD_SENSOR
	AimConfigBYD(100);
	IllumConfigBYD(100);	
#else
	AimConfigHi704A(100);
	IllumConfigHi704A(100);
#endif	//end #ifdef BYD_SENSOR
#else
	AimConfigJade(100);
	IllumConfigJade(100);
#endif

	return TRUE;
}


static bool g_led_control_initialized = false;
static BYTE g_led_control_cached_value = 0;

/** Temporarily turn illumination off for cell phone reading.
	Exposure control code calls this function if it thinks there is a
	cellphone and it is necesaary to temporarily turn the illumination
	LEDs off to read the cell phone.

	cellphone_flag - True, turns the illumination off for cellphone reading.  
	                 Otherwise, restore the illumination. 
*/
void ForceCellphone(bool cellphone_flag)
{
	if (g_cellphone_flag == cellphone_flag)
		return;

	g_cellphone_flag = cellphone_flag;
	ScanDriverOpenContext_t * pMasterContext = ScanDriverScanningMasterMode();
	if (pMasterContext)
		LightControlOpenContextChanged(pMasterContext);
}


bool GetForceCellphone()
{
	return g_cellphone_flag;
}


BOOL LedControl(BYTE value, bool forced = false)
{
	{
		//dbg_output("Value = %d, DebugScanDriver = %d\r\n",value, GET_SETTING(DebugScanDriver));
	}
	if (GET_SETTING(DebugScanDriver) & 128)
	{
		value |= 0x20;
	}

	bool &initialized = g_led_control_initialized;
	BYTE &cached_value = g_led_control_cached_value;

	if (!initialized || forced || value != cached_value)
	{
		initialized = true;
		cached_value = value;
		DBG(== 3) dbg_output("LedControl(%u, %u)\r\n", value, forced);                    //lmc debug

#ifdef PSOCLESS
		// GEN6_TODO: Setup up LedControlStateJade in HW layer interface in 
	    // scandriverscannerselect.c and use the HW layer interface here.
		//#if defined(OEM_ENGINE)		//if not an OEM
	    
		if(!HWLAYER.IsPsocSupported())
	    {
		//#endif
#ifdef HI2D_T
#ifdef BYD_SENSOR
			LedControlStateBYD(value);		
#else
			LedControlStateHi704A(value);
#endif	//end #ifdef BYD_SENSOR
#else
			LedControlStateJade(value);
#endif
			return TRUE;

		//#if defined(OEM_ENGINE)		//else we are an OEM
    	}
		else
		{
			return PsocInternalWriteRegisters (PSOC_LED_REG, &value, 1);
		}		   
		//#endif

#else		//else ifdef PSOCLESS

		return PsocInternalWriteRegisters (PSOC_LED_REG, &value, 1);

#endif     //end ifdef PSOCLESS
	}

	return TRUE;
}



void HW_light_control_MT9V022 :: resetLights()
{
	LedControl(psocLedModeReg, true);
	//PsocInternalWriteRegisters (PSOC_LED_REG, &psocLedModeReg, 1);

	if (illumState)
		HWLAYER.IlluminationOn();
	else
		HWLAYER.IlluminationOff();

	if (aimState)
		HWLAYER.AimerOn();
	else
		HWLAYER.AimerOff();
}

void HW_light_control_MT9V022 :: setIllum(bool bOn)
{
// GEN6_TODO: Add this back in
//	if (illumState == bOn)
//		return;

	illumState = bOn;
	if (illumState)
		HWLAYER.IlluminationOn();
	else
		HWLAYER.IlluminationOff();
}

void HW_light_control_MT9V022 :: setAim(bool bOn)
{
// GEN6_TODO: Add this back in
//	if (aimState == bOn)
//		return;

	aimState = bOn;
	if (aimState)
		HWLAYER.AimerOn();
	else
		HWLAYER.AimerOff();
}


void HW_light_control_MT9V022 :: setLedMode(unsigned ledMode, unsigned ledModeMask)
{
	DBG(== 3) dbg_output("setLedMode(0x%X, 0x%X)\r\n", ledMode, ledModeMask);        

	ledMode = (ledMode & ledModeMask) | (psocLedModeReg & ~ledModeMask);
// GEN6_TODO: Add this back in.
//	if (psocLedModeReg == ledMode)
//		return;


	psocLedModeReg = ledMode;
	LedControl(psocLedModeReg);
//	PsocInternalWriteRegisters (PSOC_LED_REG, &psocLedModeReg, 1);
}



/*
There is a suspend / resume issue with
PSOC_current_reg::setIntensity.  This code keeps
track of the operational current value stored in the PSOC and avoids
updates to the PSOC unless the value in the PSOC needs to change.
However, across suspend / resume, the operational current in the PSOC
is reset.  setIntensity doesn't know about the reset and would become
confused.

This issue isn't a problem because PDT products do not support changes
to intensity.
 
See bug report 1999 for more details
http://hhpvbug01.welchallyn.com/show_bug.cgi?id=1999
*/

void
PSOC_current_reg ::
setIntensity(unsigned intensity)
{
	if (intensity > 100)
		intensity = 100;

	if (! is_initialized)
	{
		unsigned char *p_regs = (unsigned char *)gp_PsocRegisters;
		max_current = p_regs[max_current_reg];
		op_current  = p_regs[op_current_reg];
		
		// !! This is a hack for development only.  Some PSOCs do not
		// have the operational max current set (it is zero).  For
		// these units, we use the default operational current
		// instead.  We need to remove this hack before we deploy.
		if (max_current == 0)
			max_current = op_current;


		is_initialized = true;
	}

	BYTE new_op_current = (max_current * intensity + 50) / 100;
	if (op_current != new_op_current)
	{
// GEN6_TODO:  Implement intensity control
//		if (PsocInternalWriteRegisters(op_current_reg, &new_op_current, 1) == TRUE)
			op_current = new_op_current;
	}
}

void PSOC_current_reg_with_flash :: setFlashIntensity()
{
	if (op_current != flash_current)
	{
		op_current = flash_current;
		
// GEN6_TODO:  No flash on GEN6
//		PsocInternalWriteRegisters(op_current_reg, &op_current, 1);
	}
}


void HW_light_control_MT9V022 :: setRedIllumIntensity(unsigned intensity)
{
	red_current.setIntensity(intensity);
}

void HW_light_control_MT9V022 :: setGreenIllumIntensity(unsigned intensity)
{
	green_current.setIntensity(intensity);
}

void HW_light_control_MT9V022 :: setAimIntensity(unsigned intensity)
{
	aim_current.setIntensity(intensity);
}


/* This function turns on the flash mode current if
   bEnableFlashCurrent is true.  It goes to normal current if
   bEnableFlashCurrent is false.  The flash mode bit in reg. 0x32
   should already be set.  This function only adjusts the illumination
   operational current register.
  */
bool HW_light_control_MT9V022 :: flashCurrent(bool bEnableFlashCurrent)
{
	if (bEnableFlashCurrent)
		red_current.setFlashIntensity();
	else
		setRedIllumIntensity(100);

	return true;
}


/* This is called following a suspend / resume cycle to reset the LED mode
   register to it's default value. */
void HW_light_control_MT9V022 :: restoreLedMode()
{
	//dbg_output("resetLedMode() setting led mode to 0x%X\r\n", psocLedModeReg);
	LedControl(psocLedModeReg, true);
//	PsocInternalWriteRegisters (PSOC_LED_REG, &psocLedModeReg, 1);
}

HW_light_control_STMVC700 :: HW_light_control_STMVC700()
	: illum_current(PSOC_RED_CURRENT_REG, PSOC_RED_OP_MAX_REG)
{
}

BOOL 
HW_light_control_STMVC700 :: 
setHwLightMode(  DWORD lm, DWORD nIllVal, DWORD nAimVal, 
				 DWORD /* illumSource */ )
{
	BOOL bIllOn = FALSE, bAimOn = FALSE, bAimReallyOn = FALSE;

	switch(lm)
	{
	case HHPSD_LM_AIM_ON:
		bAimOn = TRUE;
		bAimReallyOn = TRUE;
		break;
	case HHPSD_LM_BLINK:
	case HHPSD_LM_ILLUM_ON:
		bIllOn = TRUE;
		break;
	case HHPSD_LM_ILLUM_ON_AIM_ON:
		bAimOn = TRUE;
		bAimReallyOn = TRUE;
		bIllOn = TRUE;
		break;
	case HHPSD_LM_INTERLACED:
	case HHPSD_LM_CONCURRENT:
		bAimOn = TRUE;
		bIllOn = TRUE;
		break;
	};


	if (bIllOn && nIllVal != nCurrentIlluminationIntensity)
	{
		HWLAYER.IlluminationConfig(nIllVal);
		nCurrentIlluminationIntensity = nIllVal;
	}
	if (bAimOn && nAimVal != nCurrentAimerIntensity)
	{
		HWLAYER.AimerConfig(nAimVal);
		nCurrentAimerIntensity = nAimVal;
	}
	if (bIllOn && !bCurrentIlluminationState)
		HWLAYER.IlluminationOn();
	if (bAimReallyOn && !bCurrentAimerState)
		HWLAYER.AimerOn();
	if (!bIllOn && (bCurrentIlluminationState || HWLAYER.IllumIsOn()))
		HWLAYER.IlluminationOff();
	if (!bAimReallyOn && (bCurrentAimerState || HWLAYER.AimIsOn()))
		HWLAYER.AimerOff();

	if (lm == HHPSD_LM_ILLUM_ON || lm == HHPSD_LM_ILLUM_ON_AIM_ON
		|| lm == HHPSD_LM_INTERLACED || lm == HHPSD_LM_CONCURRENT || lm== HHPSD_LM_BLINK)
	{
		illum_current.setIntensity(nIllVal);
	}
	
	return TRUE;
}

void HW_light_control_STMVC700 :: setRedIllumIntensity(unsigned intensity)
{
	illum_current.setIntensity(intensity);
}



static DWORD lastExposureSet = 0;

//void SaveLastExposureSet( DWORD tint )
//Save the last exposue writen to imager
// 
void SaveLastExposureSetForFlashControl( DWORD tint )
{
	lastExposureSet = tint;
}



//void PsocWriteFlashCurrent( DWORD nLineISTnum )
//Write flash current that is boosting during vertical sync, so it does not effect currrent image.
//Selects which platform to talk to.
// 
void PsocWriteFlashCurrent( DWORD nLineISTnum )
{
	if( HWLAYER.IsPsocSupported() )
	{
		HW_light_control * p_lc_base = HHPSD.getLightController();
		SDASSERT(p_lc_base);
	
		HW_light_control_OEM_Jade * p_lc_oem_jade = 0;		

		if( p_lc_base )
		{			
			p_lc_oem_jade = dynamic_cast<HW_light_control_OEM_Jade *>(p_lc_base);
			SDASSERT(p_lc_oem_jade);

			if( p_lc_oem_jade )
			{
				DBG(== 4) dbg_output("PsocWriteFlashCurrent(nLineISTnum 0x%X)\r\n", nLineISTnum);

				p_lc_oem_jade->WriteFlashCurrent( nLineISTnum );
			}
		}			
	}
}


//void OemHandleCellPhoneLights( DWORD nLineISTnum )
//Set up lights control for cell phone on oem psoc boards
//Selects which platform to talk to.
// 
void OemHandleCellPhoneLights( DWORD nLineISTnum )
{
	if( HWLAYER.IsPsocSupported() )
	{
		HW_light_control * p_lc_base = HHPSD.getLightController();
		SDASSERT(p_lc_base);
	
		HW_light_control_OEM_Jade * p_lc_oem_jade = 0;		

		if( p_lc_base )
		{			
			p_lc_oem_jade = dynamic_cast<HW_light_control_OEM_Jade *>(p_lc_base);
			SDASSERT(p_lc_oem_jade);

			if( p_lc_oem_jade )
			{
				p_lc_oem_jade->HandleCellPhoneLights( nLineISTnum );
			}
		}			
	}
}


//#if defined(OEM_ENGINE)		//OEM decoder board support of imager with PSOC light control


HW_light_control_OEM_Jade :: HW_light_control_OEM_Jade()
	: illumState(0), aimState(0), psocLedModeReg(0),
	  current_mode(HHPSD_LM_OFF),
	  red_current(PSOC_RED_CURRENT_REG, PSOC_RED_OP_MAX_REG, PSOC_FLASH_CURRENT),
	  green_current(PSOC_GREEN_CURRENT_REG, PSOC_GREEN_OP_MAX_REG),
	  aim_current(PSOC_AIM_CURRENT_REG, PSOC_AIM_OP_MAX_REG)
{
//	dbg_output("HW_light_control_OEM_Jade :: HW_light_control_OEM_Jade()\r\n");
	unsigned char *p_regs = (unsigned char *)gp_PsocRegisters;
	psocLedModeReg = p_regs[PSOC_LED_REG];
	boostUp = false;
	boostUpPending = false;
	psocLedModeRegNext = 0;
	psocLedModeRegCurrent = 0;
	m_cellPhoneFlagMode = 0;
	m_lightMode = 0; 
	m_IllVal = 0;
	m_AimVal = 0;
	m_nLineISTnum = 0;
//	dbg_output("Initial psocLedModeReg = 0x%X\r\n", psocLedModeReg);
}



int 
HW_light_control_OEM_Jade ::
boostOn() const
{
	if( HWLAYER.IsPsocSupported() )
	{
		return (psocLedModeReg & 0x08) >> 3;
	}
	else
	{
		return GetILL_FlashMode(); /// @todo After Larry's changes, check bits in psocLedModeReg;
	}	 							   
}


int 
HW_light_control_OEM_Jade ::
CurrentBoostIntensityLevel() const
{
	int intensityLevel = -1;

	if( HWLAYER.IsPsocSupported() )
	{
		intensityLevel = ((psocLedModeReg & 0x0c) >> 2);
	}
	
	return intensityLevel;	 		   //return 0 for 100%, 1 for 120%, and 2 for 150%, 3 is invalid					   
}



int 
HW_light_control_OEM_Jade ::
CurrentBoostIntensityPercentLevel() const
{
	int intensityLevel = -1;
	int intensityPercentLevel = -1;

	if( HWLAYER.IsPsocSupported() )
	{
		intensityLevel = ((psocLedModeReg & 0x0c) >> 2);

		if( (intensityLevel == 2) )
		{
		 	intensityPercentLevel = 150;                     //150%
		}			
		else if( (intensityLevel == 1) )
		{
		 	intensityPercentLevel = 120;                  //120%
		}
		else if( (intensityLevel == 0) )
		{
		 	intensityPercentLevel = 100;                //100%
		}		
	}
	
	return intensityPercentLevel;	 		   //return 100%, 120%, or 150%					   
}



int 
HW_light_control_OEM_Jade ::
SelectBoostIntensityLevel( int tint ) 
{
	int intensityLevel = -1;

	if( HWLAYER.IsPsocSupported() )
	{
		if( (tint <= 29) )
		{
		 	intensityLevel = 2;                     //150%
		}			
		else if( (tint > 29)  && (tint <= 121) )                        //lmc    Was 209, changed for heat mit
		{
		 	intensityLevel = 1;                  //120%
		}
		else if( (tint > 121) )                                         //lmc    Was 209, changed for heat mit
		{
		 	intensityLevel = 0;                //100%
		}
		else
		{
		 	intensityLevel = 0;                //100%              //lowest is safest as the default
		}
	}
	
	return intensityLevel;	 							   
}



int 
HW_light_control_OEM_Jade ::
FindBoostIntensityLevel( int intens ) 
{
	int intensityLevel = -1;

	if( HWLAYER.IsPsocSupported() )
	{
		if( (intens > 120) && (intens <= 150) )
		{
		 	intensityLevel = 2;                     //150%
		}			
		else if( (intens > 100) && (intens <= 120) )
		{
		 	intensityLevel = 1;                  //120%
		}
		else if( (intens > 0) && (intens <= 100) )
		{
		 	intensityLevel = 0;                //100%
		}
		else
		{
		 	intensityLevel = 0;                //100%              //lowest is safest as the default
		}
	}
	
	return intensityLevel;	 							   
}



int
HW_light_control_OEM_Jade ::
SelectHighBoostIntensityLevel( int tint ) 
{
	int intensityLevel = -1;

	if( HWLAYER.IsPsocSupported() )
	{
		if( (tint <= 12) )                     //less than 200 uS
		{
		 	intensityLevel = 1;                     //128 uS (actual 98)
		}			
		else if( (tint > 12)  && (tint <= 18) )                        //less than 300 uS
		{
		 	intensityLevel = 2;                  //256 uS (actual 221)
		}
		else if( (tint > 18)  && (tint <= 24) )                        //less than 400 uS
		{
		 	intensityLevel = 3;                  //384 uS (actual 369)
		}
		else if( (tint > 24)  && (tint <= 33) )                        //less than 550 uS
		{
		 	intensityLevel = 4;                  //512 uS (actual 484)
		}		
	}
	
	return intensityLevel;	 							   
}



//if not using a psoc with a fixed pulse at high boost current
//#define NOT_USING_PSOC_WITH_HIGH_CURRENT_PULSE_TIMER	1

extern int gScanDriverCellPhone;		//FIXME TODO definitely do not want this to stay


BOOL
HW_light_control_OEM_Jade ::
setHwLightMode(DWORD lm, DWORD nIllVal, DWORD nAimVal, DWORD illumSource)
{
	DBG(== 3) dbg_output(  "setHwLightMode(%u, %u, %u, %u)\r\n", 
						  lm, nIllVal, nAimVal, illumSource    );

	bool bIllOn = FALSE, bAimOn = FALSE;
	BYTE psocLedMode = 0;

	BYTE psocLedModeMask = PSOC_RED_ILLUM_OEM;
	psocLedModeMask = ~psocLedModeMask;                       //invert mask

	bool bIllOnBeSetOn = false; 

	// We don't support flash mode with the green aimers.
/*	if (lm == HHPSD_LM_FLASH && illumSource != HHPSD_IS_PRIMARY)
	lm = HHPSD_LM_INTERLACED; */

	if (!HWLAYER.IsPsocSupported())	
	{
		psocLedModeMask = PSOC_RED_ILLUM;
		psocLedModeMask = ~psocLedModeMask;                       //invert mask

		if (illumSource == HHPSD_IS_PRIMARY)
			psocLedMode |= PSOC_RED_ILLUM;

		switch(lm)
		{
		case HHPSD_LM_AIM_ON:
			bAimOn = TRUE;
			break;
		case HHPSD_LM_ILLUM_ON:
			bIllOn = TRUE;
			psocLedModeMask |= PSOC_RED_ILLUM;
			break;
		case HHPSD_LM_ILLUM_ON_AIM_ON:
			bAimOn = TRUE;
			bIllOn = TRUE;
			psocLedModeMask |= PSOC_RED_ILLUM;
			break;
	/*	case HHPSD_LM_FLASH:
			psocLedMode |= 0x43;
			psocLedModeMask |= PSOC_RED_ILLUM;
			break; */
		case HHPSD_LM_AIM_ONLY_NON_EXPOSURE:
			psocLedMode |= 0x1;
			psocLedModeMask |= PSOC_RED_ILLUM;
			break;
		case HHPSD_LM_INTERLACED:
			psocLedMode |= 0x3;
			psocLedModeMask |= PSOC_RED_ILLUM;
			break;
		case HHPSD_LM_CONCURRENT:
			psocLedMode |= 0x1;
			bIllOn = TRUE;
			psocLedModeMask |= PSOC_RED_ILLUM;
			break;
		case HHPSD_LM_BLINK:
			psocLedMode |= 0x2;
			psocLedModeMask |= PSOC_RED_ILLUM;
			break;
		};
	
	
		if (nIllVal == 0)
		{
			bIllOn = FALSE;
			psocLedMode &= ~2;
		}
		else if (nIllVal > 100)
		{
			#ifndef _SMART_SCAN_DRIVER_FLASH_INTEN_CNTRL
				SetILL_FlashMode(1); /// @todo After Larry's changes, set bit on psocLedMode instead.
			#else
				psocLedMode |= 0x20;
			#endif
		}
		else
		{
			#ifndef _SMART_SCAN_DRIVER_FLASH_INTEN_CNTRL
				SetILL_FlashMode(0);
			#endif
		}
	}	
	else	//We are OEM                               //else if HWLAYER.IsPsocSupported()
	{
		int psocModeBits = 0;         //set to indicate mode 0

		switch(lm)
		{
		case HHPSD_LM_AIM_ON:
			psocLedMode &= 0xfc;              //clear mode bits
			bAimOn = TRUE;
			break;
		case HHPSD_LM_ILLUM_ON:
			psocLedMode &= 0xfc;             //clear mode bits
			bIllOn = TRUE;			
			break;
		case HHPSD_LM_ILLUM_ON_AIM_ON:
			psocLedMode &= 0xfc;            //clear mode bits
			bAimOn = TRUE;
			bIllOn = TRUE;			
			break;
	/*	case HHPSD_LM_FLASH:
			psocLedMode |= 0x43;
			psocLedModeMask |= PSOC_RED_ILLUM_OEM;
			break; */
		case HHPSD_LM_AIM_ONLY_NON_EXPOSURE:
			psocLedMode |= 0x1;
			psocModeBits = 1;			   
			break;
		case HHPSD_LM_INTERLACED:
			psocLedMode |= 0x3;
			psocModeBits = 3;			   
			break;
		case HHPSD_LM_CONCURRENT:
			psocLedMode |= 0x1;
			psocModeBits = 1;
			bIllOn = TRUE;			
			break;
		case HHPSD_LM_BLINK:
			psocLedMode |= 0x2;
			psocModeBits = 2;			   
			break;
		};
		
		psocLedMode &= 0xf3;            //clear flash bits			   

		if(nAimVal == 0)                //turn off the aimer if no intensity set (0)
		{
			bAimOn = FALSE;
			psocLedMode &= ~0x01;              //clear aimer mode bit (bit 0)

			if( psocModeBits == 1 )
			{
			 	psocModeBits = 0;
			}
			else if( psocModeBits == 3 )
			{
			 	psocModeBits = 2;
			}
		}

		if(nIllVal == 0)                 //turn off the leds if no intensity set (0)
		{
			bIllOn = FALSE;
			psocLedMode &= ~0x02;              //clear Led mode bit (bit 1)

			if( psocModeBits == 2 )
			{
			 	psocModeBits = 0;
			}
			else if( psocModeBits == 3 )
			{
			 	psocModeBits = 1;
			}
		}

		if( (psocModeBits == 3) || (psocModeBits == 2) )          //if flo controls the leds, allow boost
		{
			if( (nIllVal > 0) && (nIllVal <= 100) )                       //100%
			{
				psocLedMode &= 0xf3;            //clear flash bits	
			}
			else if( (nIllVal > 100) && (nIllVal <= 120) )               //120%
			{			
					psocLedMode |= 0x04;			
			}
			else if( (nIllVal > 120) && (nIllVal <= 150) )               //150%
			{			   
					psocLedMode |= 0x08;			
			}
		}
		else    //if leds are not flo controled, then illumination can only be set to a max of 100
		{
			if( nIllVal > 100 )
			{
		 		nIllVal = 100;
			}
		}
	}		//end if HWLAYER.IsPsocSupported()				  // end OEM               

	
	if (lm == HHPSD_LM_ILLUM_ON || lm == HHPSD_LM_ILLUM_ON_AIM_ON
		|| lm == HHPSD_LM_INTERLACED || lm == HHPSD_LM_CONCURRENT || lm== HHPSD_LM_BLINK)
	{
		if (illumSource == HHPSD_IS_PRIMARY)
		{
			red_current.setIntensity(nIllVal);
		}
		else
		{
			green_current.setIntensity(nIllVal);
		}
	}
	/* else if (lm == HHPSD_LM_FLASH && current_mode != lm)
	   red_current.setIntensity(100); */

	if (lm == HHPSD_LM_AIM_ON || lm == HHPSD_LM_ILLUM_ON_AIM_ON
		|| lm == HHPSD_LM_INTERLACED || lm == HHPSD_LM_CONCURRENT
		|| lm== HHPSD_LM_AIM_ONLY_NON_EXPOSURE                     )
	{
		aim_current.setIntensity(nAimVal);
	}
	
	if(!HardwareToken.IAmOEMJade())	
	{
		if (g_cellphone_flag && (0x2 & psocLedMode & psocLedModeMask) == 0x2)
		{
			psocLedMode     &= ~0x2;
			psocLedMode     |= 0x20;
			psocLedModeMask |= 0x23;
		}	
	}
	else
	{
		bool cellphone = !!(gScanDriverCellPhone);         //temporary, should not be using extern here,  fixme

		psocLedMode &= ~0x20;            //clear mode 0 timer bit (#5), pulse = 128 uS
		psocLedMode &= ~0x40;            //clear mode 0 timer bit (#6), pulse = 128 uS

		if( cellphone )
		{			 
			if( g_cellphone_flag && ((0x2 & psocLedMode) == 0x2) && (m_cellPhoneFlagMode == 0) )   //if in mode 2 or mode 3, make it mode 0, no lights or aimer  .
			{
				bIllOnBeSetOn = SetUpCellPhoneLights( 2, lm, nIllVal, nAimVal );

				if( !bIllOnBeSetOn && (m_cellPhoneFlagMode == 2) )
				{
					bIllOn = false;
					psocLedMode &= ~0x3;             //set mode 0
					psocLedMode &= 0xf3;            //clear flash bits, illum = 100% 					
				}									   
			}
			else if( ((0x2 & psocLedMode) == 0x2) && (m_cellPhoneFlagMode == 0) )        //if in mode 2 or mode 3, make it mode 0, no lights or aimer  .
			{
				bIllOnBeSetOn = SetUpCellPhoneLights( 1, lm, nIllVal, nAimVal );

				if( bIllOnBeSetOn  && (m_cellPhoneFlagMode == 1) )
				{
				 	bIllOn = true;
					psocLedMode &= ~0x3;             //set mode 0
					psocLedMode &= 0xf3;            //clear flash bits, illum = 100%
					psocLedMode |= 0x08;            //set flash bits, illum = 150%
					 
					int CurrentHighBoostExposure = SelectHighBoostIntensityLevel( lastExposureSet );

					DBGM(4,) dbg_output(" BoostExp(%d) \r\n", CurrentHighBoostExposure);
					
					if( CurrentHighBoostExposure == 2 )      //note: if == 0, default is then pulse = 128 uS (actual 98) 
					{
						psocLedMode |= 0x20;            //set mode 0 timer bit (#5), pulse = 256 uS (actual 221)	
					}
					else if( CurrentHighBoostExposure == 3 ) 
					{
						psocLedMode |= 0x40;            //set mode 0 timer bit (#6), pulse = 384 uS (actual 369)	
					}
					else if( CurrentHighBoostExposure == 4 ) 
					{
						psocLedMode |= 0x20;            //set mode 0 timer bit (#5), pulse = 512 uS (actual 484)
						psocLedMode |= 0x40;            //set mode 0 timer bit (#6), pulse = 512 uS	
					}												   
				}				
			}
			else if( ((0x3 & psocLedMode) == 0) && bIllOn )        //if in mode 0, make higher intensity not possible  .
			{
				psocLedMode &= 0xf3;            //clear flash bits, illum = 100%
			}			   
		}
		else
		{
			SetUpCellPhoneLights( 0, lm, nIllVal, nAimVal );            //clear all
		}
	}	         
	
	// If the lights are off, enable boost current.
	if (!bIllOn && (psocLedModeMask & 2) && (psocLedMode & 2) == 0)
		psocLedMode |= 0x08;

	AimOnstatus=bAimOn;

	setLedMode(psocLedMode, psocLedModeMask);       //psocLedModeReg = (ledMode & ledModeMask) | (psocLedModeReg & ~ledModeMask);	

	DBGM(4,) dbg_output("setIllum(%u) setAim(%u)\r\n", !!bIllOn, !!bAimOn);
	setIllum(!!bIllOn);
	setAim(!!bAimOn);

	if( HWLAYER.IsPsocSupported() && bIllOnBeSetOn )	 
	{
#ifdef NOT_USING_PSOC_WITH_HIGH_CURRENT_PULSE_TIMER		
		HWLAYER.IlluminationOffTimer();			//set timer to turn Led off
#endif		
		
		bIllOnBeSetOn = false;
	}	 

	current_mode = lm;

	// GEN6_TODO: Remove current hack below.

	if(!HWLAYER.IsPsocSupported())
	{
#ifdef HI2D_T
#ifdef BYD_SENSOR
		AimConfigBYD(100);
		IllumConfigBYD(100);	
#else
		AimConfigHi704A(100);
		IllumConfigHi704A(100);
#endif	//end #ifdef BYD_SENSOR
#else	//else #ifdef HI2D_T
		AimConfigJade(100);
		IllumConfigJade(100);
#endif
	}

	return TRUE;
}


void HW_light_control_OEM_Jade :: resetLights()
{
	LedControl(psocLedModeReg, true);
	//PsocInternalWriteRegisters (PSOC_LED_REG, &psocLedModeReg, 1);

	if (illumState)
		HWLAYER.IlluminationOn();
	else
		HWLAYER.IlluminationOff();

	if (aimState)
		HWLAYER.AimerOn();
	else
		HWLAYER.AimerOff();
}


void HW_light_control_OEM_Jade :: setIllum(bool bOn)
{
// GEN6_TODO: Add this back in
//	if (illumState == bOn)
//		return;

	illumState = bOn;
	if (illumState)
		HWLAYER.IlluminationOn();
	else
		HWLAYER.IlluminationOff();
}


void HW_light_control_OEM_Jade :: setAim(bool bOn)
{
// GEN6_TODO: Add this back in
//	if (aimState == bOn)
//		return;

	aimState = bOn;
	if (aimState)
		HWLAYER.AimerOn();
	else
		HWLAYER.AimerOff();
}


void HW_light_control_OEM_Jade :: setLedMode(unsigned ledMode, unsigned ledModeMask)
{
	DBG(== 4) dbg_output("setLedMode( mode 0x%X, mask 0x%X)\r\n", ledMode, ledModeMask);            

	ledMode = (ledMode & ledModeMask) | (psocLedModeReg & ~ledModeMask);
// GEN6_TODO: Add this back in.
//	if (psocLedModeReg == ledMode)
//		return; 	

	if(!HWLAYER.IsPsocSupported())
	{
		psocLedModeReg = ledMode;

		DBG(== 3) dbg_output("setLedMode(mode reg 0x%X)\r\n", psocLedModeReg);

		LedControl(psocLedModeReg);
	}
	else
	{
		if( boostUpPending )     //if another command comes in, we will throw away the pending boost and handle the new command immediately
		{		 	
			ClearBoostDeferred();
		}

		if( boostUp )
		{
		 	psocLedModeRegNext = ledMode;   //save  boost setting to be writen in vertical blanking latter
			boostUpPending = true;               
		}
		else
		{
			psocLedModeReg = ledMode;

	   		DBG(== 3) dbg_output("setLedMode(mode reg 0x%X)\r\n", psocLedModeReg);

			ClearBoostDeferred();
			PsocInternalWriteRegisters (PSOC_LED_REG, &psocLedModeReg, 1);
		}
	}
}


void HW_light_control_OEM_Jade :: setRedIllumIntensity(unsigned intensity)
{
	red_current.setIntensity(intensity);
}


void HW_light_control_OEM_Jade :: setGreenIllumIntensity(unsigned intensity)
{
	green_current.setIntensity(intensity);
}


void HW_light_control_OEM_Jade :: setAimIntensity(unsigned intensity)
{
	aim_current.setIntensity(intensity);
}


/* This function turns on the flash mode current if
   bEnableFlashCurrent is true.  It goes to normal current if
   bEnableFlashCurrent is false.  The flash mode bit in reg. 0x32
   should already be set.  This function only adjusts the illumination
   operational current register.
  */

bool HW_light_control_OEM_Jade :: flashCurrent(bool bEnableFlashCurrent, int tint)
{
	if(!HWLAYER.IsPsocSupported())
	{
		if (bEnableFlashCurrent)
			red_current.setFlashIntensity();
		else
			setRedIllumIntensity(100);

		return true;
	}
	else
	{
/*		BYTE LedDataReg;

		int newBoost = SelectBoostIntensityLevel( tint );

		PsocInternalReadRegisters(PSOC_LED_REG, &LedDataReg, 1);

		if( (newBoost == 2) && ((LedDataReg & 0x03) == 0x03 ) && ((LedDataReg & 0x0c) == 0x00) )        //can not go back to high power
		{
			return true;
		}
		else if( (newBoost == 1) && ((LedDataReg & 0x03) == 0x03 ) && ((LedDataReg & 0x0c) == 0x00) )   //can not go back to high power
		{
			return true;
		}
		else if( (newBoost == 0) && ((LedDataReg & 0x03) == 0x03 ) && ((LedDataReg & 0x0c) == 0x00) )   //already at low power, so stay there
		{
			return true;
		}
		else
		{
			if( newBoost == 2 )
			{
				psocLedModeReg &= 0xf3;          //switching from Medium Current mode back to High Current mode , or set high current for exposure < 500us
				psocLedModeReg |= 0x08;				
			}			
			else if( newBoost == 1 )
			{
				psocLedModeReg &= 0xf3;    // when exposure>0.5ms WE switch to medium current
				psocLedModeReg |= 0x04;
			}
			else if( newBoost == 0 ) 
			{
				psocLedModeReg &= 0xf3;   // when exposure >3.5ms, we switch to low current.
			}
			else  
			{
				psocLedModeReg &= 0xf3;   // as a default, we switch to low current.
			}

			DBG(== 3) dbg_output(" flash current 0x%X \r\n", psocLedModeReg);

			return PsocInternalWriteRegisters (PSOC_LED_REG, &psocLedModeReg, 1);			
		}*/
		return false;
	}		
}


/* This is called following a suspend / resume cycle to reset the LED mode
   register to it's default value. */
void HW_light_control_OEM_Jade :: restoreLedMode()
{
//	dbg_output("resetLedMode() setting led mode to 0x%X\r\n", psocLedModeReg);
	LedControl(psocLedModeReg, true);
//	PsocInternalWriteRegisters (PSOC_LED_REG, &psocLedModeReg, 1);
}


//void HW_light_control_OEM_Jade :: WriteFlashCurrent( DWORD nLineISTnum )
//Write flash current that is boosting during vertical sync interupt or SOF interupt (both have nLineISTnum == 0), so it does not effect currrent image
// 
void HW_light_control_OEM_Jade :: WriteFlashCurrent( DWORD nLineISTnum )
{
	if( HWLAYER.IsPsocSupported() && ( nLineISTnum == 0) )
	{
	 	if( boostUp )
		{
		 	boostUp = false;

			if( psocLedModeRegCurrent == psocLedModeReg )         //if no one has changed lights since we saved this in line interupt 11
			{
				psocLedModeReg = psocLedModeRegNext;             //lets write the new boost level to the psoc

				DBG(== 3) dbg_output("WriteFlashCurrent(mode reg 0x%X)\r\n", psocLedModeReg);

				PsocInternalWriteRegisters( PSOC_LED_REG, &psocLedModeReg, 1 );
			}
		}

		ClearBoostDeferred();	
	}
}



extern BOOL Write_Frame_Blank_Time(WORD value);
#define TWAIT_TIME_OEM_CELL_SHORT					0x0038	         //56 * 16.7 uS = 935 ms
#define TWAIT_TIME_OEM_CELL_LONG					0x012C	         //300 * 16.7 uS = 5010 ms



//void HW_light_control_OEM_Jade :: HandleCellPhoneLights( DWORD nLineISTnum )
//Set cell phone lights in sync with jade interupts
// Zero is vertical, 1 through 12 is stat fifo line progression
//Note: g_cellphone_flag is updated before "nLineISTnumLocal == 2" is serviced 
// 
void HW_light_control_OEM_Jade :: HandleCellPhoneLights( DWORD nLineISTnum )
{
	bool cellphone = !!(gScanDriverCellPhone);         //temporary, should not be using extern here,  fixme
	m_nLineISTnum = nLineISTnum;                

	if( (HWLAYER.IsPsocSupported()) && cellphone )
	{
		//static bool cellphone_flag_local = 0;        //lmc debug

		DWORD cellphoneFlagMode = m_cellPhoneFlagMode;             // 0 is flo mode, 1 is manual lights not in image
		DWORD nLineISTnumLocal = m_nLineISTnum;
		bool writePsoc = false;
		bool clearSettings = false;
		DWORD lightMode = m_lightMode; 
		DWORD nIllVal = m_IllVal;
		DWORD nAimVal = m_AimVal;		   
		
		switch ( nLineISTnumLocal ) 
		{
			case 0 :
				clearSettings = true;				   
				//dbg_output("Vert:%d CpF:%d \r\n", nLineISTnum, g_cellphone_flag); //lmc debug
				//if(cellphone_flag_local != g_cellphone_flag) dbg_output("Line:%d CpF:%d \r\n", nLineISTnum, g_cellphone_flag); //lmc debug
				break;
			case 1 :
				 
				//if(cellphone_flag_local != g_cellphone_flag) dbg_output("Line:%d CpF:%d \r\n", nLineISTnum, g_cellphone_flag); //lmc debug
				break;
			case 2 :								   
				
				if( nAimVal )           //lights off and aimer on in any mode if aimer enabled
				{
					setIllum(0);			//turn off gpio pin for mode 0 Led control before writing Psoc
				 	writePsoc = true;
					lightMode = 1;
				}
				else if( cellphoneFlagMode == 1 )    //lights off, aimer off, in mode 1, the mode with the Leds in the frame
				{
					setIllum(0);			//turn off gpio pin for mode 0 Led control before writing Psoc
				 	writePsoc = true;
					lightMode = 0;
				}												   

				//if(cellphone_flag_local != g_cellphone_flag) dbg_output("Line:%d CpF:%d \r\n", nLineISTnum, g_cellphone_flag); //lmc debug
				break;
			case 3 :

				if( cellphoneFlagMode == 1 )
				{
				 	writePsoc = true;
					lightMode = 7;					
				}
				 else if( nAimVal )           //lights off and aimer off in any mode if aimer enabled
				{
				 	writePsoc = true;
					lightMode = 0;
				}								

			 	//if(cellphone_flag_local != g_cellphone_flag) dbg_output("Line:%d CpF:%d \r\n", nLineISTnum, g_cellphone_flag); //lmc debug
				break;
			case 4 :
				if( cellphoneFlagMode == 2 )
				{
					Write_Frame_Blank_Time( (WORD) TWAIT_TIME_OEM_CELL_LONG );
				}
				else if( cellphoneFlagMode == 1 )    
				{
					Write_Frame_Blank_Time( (WORD) TWAIT_TIME_OEM_CELL_SHORT );
				}

				clearSettings = true; 
				//if(cellphone_flag_local != g_cellphone_flag) dbg_output("Line:%d CpF:%d \r\n", nLineISTnum, g_cellphone_flag); //lmc debug
				break;
			default:

		 		//if(cellphone_flag_local != g_cellphone_flag) dbg_output("Line:%d CpF:%d \r\n", nLineISTnum, g_cellphone_flag); //lmc debug	
				break;			   
		}

		if( writePsoc )         //set lights mode
		{
			DBG(== 3) dbg_output("WriteCellMode(mode %d illum %d)\r\n", lightMode, nIllVal);

			setHwLightMode( lightMode, nIllVal, 100, 0);
		}

		if( clearSettings )         //clear settings
		{
			m_cellPhoneFlagMode = 0;
			m_lightMode = 0; 
			m_IllVal = 0;
			m_AimVal = 0;
		}

		//cellphone_flag_local = g_cellphone_flag;            //lmc debug
	}
}



//void HW_light_control_OEM_Jade :: SetUpCellPhoneLights( DWORD cellphone_flag, DWORD lightMode, DWORD nIllVal, DWORD nAimVal )
//Configure cell phone lights
//cellphone_flag == true is when lights are off in frame and exposure is long, for reading the cell phone
//ledMode is the psoc mode register settings 
// 
bool HW_light_control_OEM_Jade :: SetUpCellPhoneLights( DWORD cellphone_flag, DWORD lightMode, DWORD nIllVal, DWORD nAimVal )
{
	bool cellphone = !!(gScanDriverCellPhone);         //temporary, should not be using extern here,  fixme
	bool turnIllumOn = false;

	if( (HWLAYER.IsPsocSupported()) && cellphone )
	{
		DWORD nLineISTnum = m_nLineISTnum;

		m_lightMode = lightMode; 
   		m_IllVal = nIllVal;
		m_AimVal = 0;		   
		m_cellPhoneFlagMode = 0;

		if( (cellphone_flag == 2) && (nLineISTnum == 1) && m_IllVal && ((m_lightMode == 5) || (m_lightMode == 7)) )     //if the lights off frame
		{
			turnIllumOn = false;
			m_cellPhoneFlagMode = cellphone_flag;			   
		}
		else if( (cellphone_flag == 1) && (nLineISTnum == 1) && m_IllVal && ((m_lightMode == 5) || (m_lightMode == 7)) )     //if the lights sync with flo frame
		{
			turnIllumOn = true;
			m_cellPhoneFlagMode = cellphone_flag;
		}
		else
		{
			m_cellPhoneFlagMode = 0;
			m_lightMode = 0; 
			m_IllVal = 0;
		}

		if( (m_cellPhoneFlagMode > 0) && (nLineISTnum == 1) && nAimVal && ((m_lightMode == 5) || (m_lightMode == 8)) )     //if the lights off frame
		{
			m_AimVal = nAimVal;			   //allow aimer on in "m_nLineISTnum == 2" 
		}
		
		//dbg_output("Line:%d CpF:%d-%d Md:%d Int:%d \r\n", nLineISTnum, m_cellPhoneFlagMode, cellphone_flag, m_lightMode, m_IllVal); //lmc debug		   
	    DBG(== 3) dbg_output("Line:%d CpF:%d-%d Md:%d Int:%d \r\n", nLineISTnum, m_cellPhoneFlagMode, cellphone_flag, m_lightMode, m_IllVal); //lmc debug
	}
	else
	{
	 	m_cellPhoneFlagMode = 0;
		m_lightMode = 0; 
		m_IllVal = 0;
		m_AimVal = 0;
	}

	return turnIllumOn;
}



//#endif		//end if defined(OEM_ENGINE)
