/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  Date:       6/11/2009
//  Tools:      RVCT Build 739
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Imaging/ScanDriverImagingInterface.cpp $
// $Revision: 1.36 $
// $Date: 2011/09/09 14:53:21EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// File:  ScanDriverImagingInterface.cpp
//
//	Functions used to interface Imaging with the Scan Driver
//------------------------------------------------------------------------------
#include "menu.h"
#include "scandrivermatrixinterface.h"
#include "ReaderConfiguration.h"
#include "ProductConfiguration.h"
#include "ScanDriverImagingInterface.h"
#include "ioman.h"

#ifdef ENGINE_PSOC
#include "ScanEnginePSOC.h"
#include "ScanDriverEngine.h"
#endif //#ifdef ENGINE_PSOC

#define	TRIGGER_5_5		1
#ifdef TRIGGER_5_5
#include "trigger_5_5.h"                                         //lmc   fixme
#endif	//end #ifdef TRIGGER_5_5

#include "HardwareToken.h"




/*----------------------------------------------------------------------------------------
  int SetLightsCommon( int LightsState, int LightsInten, int AimerState, int AimerInten );

  The following description has been reversed engineered form code, so double check conclusions.
  
  Description: To set LED or aimer state and intensity using SCN_IOControl.
  Returns: always returns an integer of 0.
  Pass: integers for illumination state, illumination intensity(0-100), aimer state, aimer intensity(0-100)
  			Passing a -1 means do not change setting and take no action.
  			Passing > 100% with Flash Illunination will indicate high current mode for use in Pres modes
  			with exposures of less than 500 uSec max.
  
  Note:  if we call this with LightsState of 0, but with LightsInten different, we will write the
  the new LightsInten value.  Joe W. thinks this might be legacy--it may have been useful years ago.
----------------------------------------------------------------------------------------*/

void SetLightsCommon( int LightsState, int LightsInten, int AimerState, int AimerInten )
{
	if(HardwareToken.IAmOEM())   //if an oem decoder board
	{		
		if(GET_SETTING(FLODelayUserSetting) != 0)     //if any value other than 0, new value of flo will be set in jade configuration
		{
			LightsInten = 0;            //if flo is being set by user, set intensitys to zero so lights and aimer are off
			AimerInten = 0;
		}
	}

#ifdef ENGINE_PSOC
	DWORD GetEngineId[2] = {HHPSD_ENGINE_ID,0};  // 1st DWORD has tag, 2nd DWORD will be filled in with the value
#endif //#ifdef ENGINE_PSOC


															//Aimer mode                              //Lights mode
															//----------------------						//---------------------
	const int mode_map[HHPSD_LM_NUM_MODES] = {HHPSD_LM_OFF,                             //HHPSD_LM_OFF
															HHPSD_LM_AIM_ON,                          //HHPSD_LM_AIM_ON
															HHPSD_LM_ILLUM_ON,                        //HHPSD_LM_ILLUM_ON
															HHPSD_LM_ILLUM_ON_AIM_ON,                 //HHPSD_LM_ILLUM_ON_AIM_ON
															HHPSD_LM_ILLUM_ON_AIM_ON,                 //HHPSD_LM_CONCURRENT
															HHPSD_LM_ILLUM_ON_AIM_ON,                 //HHPSD_LM_INTERLACED
															HHPSD_LM_OFF,                             //HHPSD_LM_FLASH
															HHPSD_LM_ILLUM_ON};                       //HHPSD_LM_BLINK


	static int CurrentIllumInten = -1;
	static int CurrentIllumState = LED_STATE_NOT_SET;
	static int CurrentAimerState = LED_STATE_NOT_SET;
	static int CurrentAimerInten = -1;
	static int CurrentLightMode = -1;
	int i=0;
	int OrigLightMode = CurrentLightMode;

	DWORD tags[20];

#ifdef FLASH_LEDS       //if ff3 gen6 board

#include	"Flash_Illumination_Control.h"

#ifdef _SMART_SCAN_DRIVER_FLASH_INTEN_CNTRL		//pass intensity control to scan driver

	static int CurrentBoostInten = -1;

	if( HardwareToken.IAmOEM()||HardwareToken.IAmThreeLevelIllumination_NonOem() )	
	{
		if( LightsInten > 120 )	//Adjust the passed in inten for scan driver so range is now 0, 100, 120 or 150 percent, since inten only can set four modes, off, normal mode, med or high flash intensity.
		{                       //If intensity is zero, respectively, illumination or aimer will be set to off farther down.
			LightsInten = 150;        //high flash intensity boost mode
		}
		else if( LightsInten > 100 )
		{
			LightsInten = 120;       //medium flash intensity boost mode
		}
		else if( LightsInten > 0 )      //if not off, make it low lights, no boost
		{
			LightsInten = 100;         //normal lights mode
		}
	}
	else
	{
		if( LightsInten > 100)		//adjust the passed in inten for scan driver so range is now 0, 100 or 150 percent, since inten only can set three modes, off, normal, or flash intensity
		{
			LightsInten = 150;           //flash intensity boost mode
		}
		else if( LightsInten > 0 )
		{
			LightsInten = 100;         //normal lights mode
		}
	}
#else

	static int CurrentFlashIllumInten = -1;         //Save the actual flash illum inten

	//Set flash light intensity if it has been changed
	if( (LightsInten >= 0) && (CurrentFlashIllumInten != LightsInten) &&
		(!( ((CurrentFlashIllumInten > 0) && (CurrentFlashIllumInten < 101)) && ((LightsInten > 0) && (LightsInten < 101)) )) &&      //if both are in the 1 to 100 range, do not change (Both in Low lights mode)
		(!( (CurrentFlashIllumInten > 100) && (LightsInten > 100) )) )      //if both are in the 101 or above range, do not change (Both in High lights mode)
	{
		CurrentFlashIllumInten = LightsInten;                 //save new setting

		if(CurrentFlashIllumInten > 100)             //set High flash current if greater than 100 %
		{
			SetILL_FlashMode(1);
			//theStdInterface.printf(" Set High illumn inten %d \r\n", CurrentFlashIllumInten);                    //lmc debug
		}
		else                                        //else low current if 0 to 100 %
		{
			SetILL_FlashMode(0);
			//theStdInterface.printf(" Set Low illumn inten %d \r\n", CurrentFlashIllumInten);                    //lmc debug
		}
	}

	if( LightsInten > 100)		//adjust the passed in inten for scan driver so range is 0 to 100 percent, since inten only matters if it is either zero or greater than zero with Flash Illum (does not understand > 100%)
	{
		int diffLightsInten;

		diffLightsInten = LightsInten - 100;
		LightsInten = LightsInten - diffLightsInten;			//lets make it 100% if in high current mode, just to indicate lights are on
	}
#endif	//end ifdef _SMART_SCAN_DRIVER_FLASH_INTEN_CNTRL

#endif                  //end if ff3 gen6 board

#ifdef ENGINE_PSOC
	if( ImageEngineHasPsoc() )      //lets see if we have a 5100 light bar
	{
		SCN_IOControl(nOpenCntxt, HHPSD_IOCTL_GET_ENGINE_CONFIG, (PBYTE)GetEngineId, sizeof GetEngineId, NULL, 0, NULL);
	}
#endif //#ifdef ENGINE_PSOC

	if( LightsInten>=0 && (CurrentIllumInten != LightsInten) )    //set light intensity
	{
		tags[i++] = LI_ILLUMINATION_INTENSITY;
		CurrentIllumInten = LightsInten;

#ifdef _SMART_SCAN_DRIVER_FLASH_INTEN_CNTRL		//pass intensity control to scan driver
		if( CurrentIllumInten > 0)
		{
			CurrentBoostInten = CurrentIllumInten;		//save if we are curently in normal or boost flash Led mode
		}
		//Do not want to change Boost if we shut the lights off, huge delays if we need to re-enable boost flash Led mode or change from normal to boost
		tags[i++] = (LightsInten > 0)?LightsInten:CurrentBoostInten;
#else
		tags[i++] = (LightsInten>5)?LightsInten:5;
#endif
	}

	if( AimerInten>=0 && (AimerInten != CurrentAimerInten) )    //set aimer intensity
	{
		tags[i++] = LI_AIMER_INTENSITY;
		CurrentAimerInten = AimerInten;
		tags[i++] = (AimerInten>5)?AimerInten:5;
	}

	if ( LightsState>=0 ) CurrentIllumState = LightsState;    //save light mode

	if ( !CurrentIllumInten) CurrentIllumState = 0;       //if lights have no intensity, default lights mode to '0' (off)

	if ( AimerState >= 0) CurrentAimerState = AimerState;     //save aimer mode

	if ( !CurrentAimerInten) CurrentAimerState = 0;          //if aimer has no intensity, default aimer mode to '0' (off)

	//Now set Scanning Mode
	if ( CurrentAimerState>0 && CurrentIllumState>0)    //if both aimer and lights (LEDS) are to be turned on
	{
		if( ( (GET_SETTING(TriggerMode) == TRGMOD_STREAM_PRESENT) || (GET_SETTING(TriggerMode) == TRGMOD_PRESENT) )
#ifdef ENGINE_PSOC
			&& ((GetEngineId[1] == ENGINE_ID_5100_LED_OFF_AXIS_LIGHT_BAR_SF)
			|| (GetEngineId[1] == ENGINE_ID_5100_LED_OFF_AXIS_LIGHT_BAR_SR)
			|| (GetEngineId[1] == ENGINE_ID_5100_LED_OFF_AXIS_LIGHT_BAR_HD))
#endif //#ifdef ENGINE_PSOC
			)
	  	{
			CurrentLightMode = HHPSD_LM_INTERLACED;
		}
		else if (GET_SETTING(ScanAimerMode) == AIM_BOTH)              //not supported in gen 6
		{
			if(GET_SETTING(DecodeMode) != NON_OMNI_ALD) CurrentLightMode = HHPSD_LM_CONCURRENT;
			else CurrentLightMode = HHPSD_LM_ILLUM_ON_AIM_ON;
		}
		else
		{
			CurrentLightMode = HHPSD_LM_INTERLACED;
		}

		if( GET_SETTING(IllOnDuringDecode) == 2 ) CurrentLightMode = HHPSD_LM_INTERLACED;     //not supported in gen 6
	}
	else if (CurrentAimerState>0)           //if only aimer is to be turned on
	{
		if( (GET_SETTING(ScanAimerMode) == AIM_CONTINUOUS) || (CurrentAimerState == AIM_CONTINUOUS) )
		{
			CurrentLightMode = HHPSD_LM_AIM_ON;
		}		
		else
		{
			CurrentLightMode = HHPSD_LM_AIM_ONLY_NON_EXPOSURE;
		}
	}
	else if( CurrentIllumState>0 )      //if only lights (LEDS) are to be turned on
	{
		//if( (GET_SETTING(TriggerMode) == TRGMOD_AUTO)                          //lmc FIXME
			//|| (GET_SETTING(TriggerMode) == TRGMOD_STREAM_PRESENT)
			//|| (GET_SETTING(TriggerMode) == TRGMOD_PRESENT)
			//|| (GET_SETTING(TriggerMode) == TRGMOD_SSTAND)
//#ifdef ENGINE_PSOC
			//|| (GetEngineId[1] == ENGINE_ID_5100_LED_OFF_AXIS_LIGHT_BAR_SF)
			//|| (GetEngineId[1] == ENGINE_ID_5100_LED_OFF_AXIS_LIGHT_BAR_SR)
			//|| (GetEngineId[1] == ENGINE_ID_5100_LED_OFF_AXIS_LIGHT_BAR_HD)
//#endif //end ifdef ENGINE_PSOC
			//)
	  	{
				CurrentLightMode = HHPSD_LM_BLINK;
		}
		//else
		//{
		//		CurrentLightMode = HHPSD_LM_ILLUM_ON;
		//}
	}
	else
	{
		CurrentLightMode = HHPSD_LM_OFF;
	}

	//Now set Idle Mode
	if (CurrentLightMode != OrigLightMode)
	{
		tags[i++] = LI_SCAN_MODE;
		tags[i++] = CurrentLightMode;

#ifdef _SMART_SCAN_DRIVER_FLASH_INTEN_CNTRL		//pass intensity control to scan driver
		if( !(pTrigger->IsCurrentTriggerStateChangePending()) )
		{
			tags[i++] = LI_IDLE_MODE;
			tags[i++] = mode_map[CurrentLightMode];
		}
#else
		tags[i++] = LI_IDLE_MODE;
		tags[i++] = mode_map[CurrentLightMode];
#endif
	}

	// TODO:  place an ASSERT here to ensure i < (sizeof tags/4)
	if( i )
	{
#ifdef _SMART_SCAN_DRIVER_FLASH_INTEN_CNTRL		//pass intensity control to scan driver
		tags[i++] = LI_ILLUMINATION_CONTROL;

	 	if(0)//( (pTrigger->GetTriggerMode() == TRGMOD_MANUAL) )              //not doing smart mode for now
	 	{
			tags[i++] = 1;                               //scan driver has control of light intensity
			pTrigger->SetScanDriverHasFlashLedControl(true);
	 	}
	 	else
	 	{
			tags[i++] = 0;                                   //we have control of light intensity
			pTrigger->SetScanDriverHasFlashLedControl(false);
	 	}

		//theStdInterface.printf( " Set Lights Writes Sc. Dr. boostInt:%d illInt:%d aimInt:%d illState:%d aimState:%d Mode:%d \r\n",CurrentBoostInten, CurrentIllumInten, CurrentAimerInten, CurrentIllumState, CurrentAimerState, CurrentLightMode );                    //lmc debug
#endif

		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_LIGHT_SETTINGS,PBYTE(tags),(DWORD)(i*sizeof(DWORD)),0, 0, 0  );
	}
}



#ifndef _LMC_DEBUGING_SETLIGHTS
void SetLights( int LightsState, int LightsInten, int AimerState, int AimerInten )
{
	SetLightsCommon( LightsState, LightsInten, AimerState, AimerInten );
}
#else
//int _DEBUG_SetLights ( int LightsState, int LightsInten, int AimerState, int AimerInten );
//Debugging, substituted for above
void _DEBUG_SetLights( int LightsState, int LightsInten, int AimerState, int AimerInten, unsigned int iCodeLine, char *cCodeFile)
{
	SetLightsCommon( LightsState, LightsInten, AimerState, AimerInten );

	static int oldLightsInten = 0;
	static int oldLightsState = 0;
	static int oldAimerState = 0;
	//if(LightsState == 0)
	//if(LightsInten == 0)
	//if( (GET_SETTING(TriggerMode) == TRGMOD_STREAM_PRESENT) || (GET_SETTING(TriggerMode) == TRGMOD_TEMP_MANUAL) )
	if( (oldLightsInten != LightsInten) || (oldLightsState != LightsState) || (oldAimerState != AimerState) )
	{
		theStdInterface.printf("S_L Called from line %d, File %s, Lights State is %d, \r\n", iCodeLine, cCodeFile, LightsState);
		theStdInterface.printf(" and Lights Inten is %d, Aimer State is %d \r\n", LightsInten, AimerState);
	}

	oldLightsInten = LightsInten;
	oldLightsState = LightsState;
	oldAimerState = AimerState;
}
#endif



//----------------------------------------------------------------------------------------
//int GetLights ( int LightType );
//
//Description: To get LED or aimer state and/or intemsity using SCN_IOControl.
//Returns: Returns an DWORD value for item selected by LightType.
//Pass: integer for  LightType, 1 === illumination intensity, 2 == aimer intensity
//
//Tags:															Modes:		HHPSD_LM_OFF             = 0,
//			LI_SCAN_MODE = 1,													HHPSD_LM_AIM_ON          = 1,
//			LI_IDLE_MODE = 2,													HHPSD_LM_ILLUM_ON        = 2,
//			LI_ILLLUMINATION_SOURCE = 3,									HHPSD_LM_ILLUM_ON_AIM_ON = (HHPSD_LM_ILLUM_ON | HHPSD_LM_AIM_ON),
//			LI_ILLUMINAITON_INTENSITY = 4,								HHPSD_LM_CONCURRENT      = 4,
//			LI_AIMER_INTENSITY = 5											HHPSD_LM_INTERLACED      = 5,
//																					HHPSD_LM_FLASH           = 6,
//																					HHPSD_LM_BLINK           = 7,
//																					HHPSD_LM_NUM_MODES       = 8
//----------------------------------------------------------------------------------------
#define	GET_LIGHTS_DATA_SIZE		8

int GetLights( int LightType )
{
	int returnValue;


	static int iCurlightsInten = -1;
	static int iCurAimerInten = -1;
	static int iCurrentScanLightsmode = -1;
	static int iCurrentIdleLightsmode = -1;
	int i;

	DWORD inTags[GET_LIGHTS_DATA_SIZE + 1];
	DWORD outTags[GET_LIGHTS_DATA_SIZE + 1];

	i = 0;

/*	if( ImageEngineHasPsoc() )
	{
		SCN_IOControl(nOpenCntxt, HHPSD_IOCTL_GET_ENGINE_CONFIG, (PBYTE)GetEngineId, sizeof GetEngineId, NULL, 0, NULL);
	}
*/

	//Set up tags to get info from scan driver

	outTags[i] = 0xFFFFFFFF; 			inTags[i++] = LI_ILLUMINATION_INTENSITY;            //location 0 is tag
	outTags[i] = 0xFFFFFFFF;			inTags[i++] = iCurlightsInten;
	outTags[i] = 0xFFFFFFFF;			inTags[i++] = LI_AIMER_INTENSITY;                  //lacation 2 is tag
	outTags[i] = 0xFFFFFFFF;			inTags[i++] = iCurAimerInten;
	outTags[i] = 0xFFFFFFFF; 			inTags[i++] = LI_SCAN_MODE;            				//location 4 is tag
	outTags[i] = 0xFFFFFFFF;			inTags[i++] = iCurrentScanLightsmode;
	outTags[i] = 0xFFFFFFFF;			inTags[i++] = LI_IDLE_MODE;                  		//location 6 is tag
	outTags[i] = 0xFFFFFFFF;			inTags[i++] = iCurrentIdleLightsmode;

	//Get the info requested
	if( (i > 0) && (i == GET_LIGHTS_DATA_SIZE) )
	{
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_LIGHT_SETTINGS, PBYTE(inTags), (DWORD)(i*sizeof(DWORD)), PBYTE(outTags), (DWORD)(i*sizeof(DWORD)), 0  );
	}

	switch ( LightType)
	{
		case 1:
			returnValue = (int)outTags[1];
			break;

		case 2:
			returnValue = (int)outTags[3];
			break;

		case 3:
			returnValue = (int)outTags[5];
			break;

		case 4:
			returnValue = (int)outTags[7];
			break;
		default:
			returnValue = -1;
			break;
	}

	return returnValue;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Heat Mitigation --> Common location to handle Simultaneous image versus Batched/Serial processing of images
//
// NewImageAvailable, in some cases the code only wants to get an image is there is one 'waiting'.  This takes
// on a different meaning (or meaning-less) concept with the batched mode. For now, batched mode simply says there is an image available.
// Important to note that everywhere this is used, the subsequent/planned GetImage will take 2 frame times rather than be automatic.

BOOL InBatchedMode( void )
{
	BOOL result = (bool) ( (GET_CONFIG_SETTING(iOpticsPopulation)==TWO_D) && GET_SETTING(BatchedImagingMode) );
	return result;
}

//NewImageAvailable returns TRUE if new image is available, other wise it will always return a default TRUE for Batch Mode and do nothing.
BOOL NewImageAvailable( void )
{
	BOOL result = FALSE;

	if (InBatchedMode())
		result = TRUE;
	else
		result = SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_NEW_IMAGE_AVAILABLE,NULL,0,NULL,0,NULL);
	return result;
}

//CurrentlyImaging returns TRUE if scanning enabled, for all modes
BOOL CurrentlyImaging( void )
{
	BOOL result = TRUE;

	result = SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SCANNING_ENABLED, NULL,0,NULL,0,NULL);
	return result;
}

// StartImaging -- will StartScan if not in batched mode, other wise it will always return a default TRUE for Batch Mode and do nothing.
BOOL StartImaging( void )
{
	BOOL result = TRUE;

	if (!InBatchedMode())
		result = SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_START_SCANNING,NULL,0,NULL,0,NULL);

	return result;
}
// Get Image -- will request an image from the Scan Driver depending on approach.
//				If not in Batched Mode, it will start scanning if not already doing so, and then get a new scan.
//          if in Batched Mode, we stop imaging if already started, then we will get a single frame capture.
BOOL GetImage( void )
{
	BOOL result = TRUE;

	if (!InBatchedMode())
	{
		if (!CurrentlyImaging())		// if not started, start!!
			StartImaging();

		result = SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_NEW_SCAN,(UCHAR *) &P_SCANBUFFER,0,NULL,0,NULL);
	}
	else
	{
		if (CurrentlyImaging())
			StopImaging();
		result = SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SINGLE_FRAME_CAPTURE,(UCHAR *) &P_SCANBUFFER,0,NULL,0,NULL);
	}
	return result;
}

// StopImaging -- will StopScanning for all modes
// Note:  ** At first , stop will always stop because it does no harm if there should be an 'open' start **
//		  ** May consider exceptoin handling this moving forward as well
BOOL StopImaging( void )
{
	BOOL result = TRUE;
	//	if (!InBatchedMode())
	result = SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_STOP_SCANNING,NULL,0,NULL,0,NULL);

	return result;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


extern void SkipIndicatorsForThisMenuCommand( void );       //found in "decodeiface.cpp"


//
//IsImagingAllowed()
//Tests if imaging allowed on DR, must be alligned first
//Returns a bool, true if allowed
//
bool IsImagingAllowed( void )
{
	bool bAllowImaging = false;

/*stub this out for now...gen6 has no alignment but left in because we may license imaging */
#if 0
#else
	bAllowImaging = true;
#endif	//end #ifdef TRIGGER_5_5

	return bAllowImaging;
}





