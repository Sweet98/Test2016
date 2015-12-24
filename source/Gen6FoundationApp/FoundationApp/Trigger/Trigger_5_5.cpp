//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Trigger/Trigger_5_5.cpp $
// $Date: 2011/11/18 03:39:04EST $
// $Revision: 1.95 $
//======================================================================================

//======================================================================================


#include "stdInclude.h"
#include "trigger_5_5.h"
#include "HWTrigger.h"
#include "ScanDriverImagingInterface.h"

//#include "hal.h"                             //not used
//#include "Vars.h"                             //not used
//#include "ScanDriverInterface.h"			//changed to "scandrivermatrixinterface.h"
#include "scandrivermatrixinterface.h"
#include "db_chrio.h"
#include "menu.h"
#include "ioman.h"
#include "decodeiface.h"
#include "mainhhplproc.h"
#include "usage_statistics.h"
#include "db_time.h"
#include "productconfiguration.h"
#include "ReaderConfiguration.h"
#include "imaging.h"                 //added for Heat Mitigation
//#include "power.h"                   //added for Heat Mitigation

#include "ScanEnginePSOC.h"
#include "HardwareToken.h"

#include "ScanDriverEngine.h"  // for PSOC register locations
#include "PowerManagement.h"
#include "Interface_Support_Functions.h"


#ifdef FLASH_LEDS       //if ff3 gen6 board
#include	"Flash_Illumination_Control.h"
#endif

//ewr05430
#include "OutputModes.h"

#ifndef XENON_LITE_1500
#ifndef RFBASE
#include "DecoderInterface.h"   //NG2D875
#endif
extern void SetImageParameters(int hgt, int wid, int wrap, int cenX, int cenY);  //NG2D875
#endif

CTrigger *pTrigger;



//#define _LMC_DEBUGING_AIMER            //un-comment out for aimer state debugging		lmc
#ifdef	_LMC_DEBUGING_AIMER
	#define Set_nAimerState( NEW_nAimerState )		_DEBUG_Set_nAimerState( NEW_nAimerState, __LINE__)   //lmc_debug    For aimer set tracking by line number
#endif


//#define _LMC_DEBUGING_LIGHTS            //un-comment out for Lights state debugging		lmc
#ifdef	_LMC_DEBUGING_LIGHTS
	#define Set_nLightsState( NEW_nLightsState )		_DEBUG_Set_nLightsState( NEW_nLightsState, __LINE__)   //lmc_debug    For lights set tracking by line number
#endif



extern bool g_dmaNotDoneBeforeSof;
int gScanDriverCellPhone = 0;

#ifdef HI2D_T
//#define HI2D_DBG
#ifdef HI2D_DBG
#define HI2D_PRINT_DBG theStdInterface.printf
#else
static void DummyPrintStateHI2D( char* a, int b=0, int c=0, int d=0, int e=0){return;}
#define HI2D_PRINT_DBG DummyPrintStateHI2D
#endif
#endif

//#define _LMC_DEBUGING_OPT_STATE            //un-comment out for optical trigger state debugging		lmc
#ifdef	_LMC_DEBUGING_OPT_STATE
	#define	PRINT_DEBUG_S			theStdInterface.printf
#else
	static void DummyPrintState( char* a, int b=0, int c=0, int d=0, int e=0){return;}
	#define	PRINT_DEBUG_S		DummyPrintState
#endif


//#define _LMC_DEBUGING_OPT_THRES            //un-comment out for optical trigger threshold debugging		lmc
#ifdef	_LMC_DEBUGING_OPT_THRES
	#define	PRINT_DEBUG_T			theStdInterface.printf
#else
	static void DummyPrintThreshold( char* a, int b=0, int c=0, int d=0, int e=0){return;}
	#define	PRINT_DEBUG_T		DummyPrintThreshold
#endif


//#define _LMC_DEBUGING_LIGHT_INTENSITY            //un-comment out for Lights intensity debugging		lmc
#ifdef	_LMC_DEBUGING_LIGHT_INTENSITY
	#define Set_AllDutyCycle( NEW_AllDutyCycle )		_DEBUG_Set_AllDutyCycle( NEW_AllDutyCycle, __LINE__)   //lmc_debug    For lights set tracking by line number
#endif


//#define _LMC_DEBUGING_LIGHT_INTENSITY_OPT_TRIG            //un-comment out for optical trigger Lights intensity debugging		lmc
#ifdef	_LMC_DEBUGING_LIGHT_INTENSITY_OPT_TRIG
	#define Set_LedDutyCycle( NEW_LedDutyCycle )		_DEBUG_Set_LedDutyCycle( NEW_LedDutyCycle, __LINE__)   //lmc_debug    For lights set tracking by line number
#endif

extern volatile UINT BaselineClock;	// a 1 ms granularity Timer for use by the Decoder (and others?!)

extern void SendStringToInterface( const char *pucBuffer, int SizeOfBuffer, bool DoNotUseIndicators );

#define STREAMING_PRESENT_MODE_AIMER_CURRENT_VALS	5	// Lower aimer current values when we're in streaming presentation mode


//************************ Scan Stand defines **********************************************************************
#define	SSTANDTRIGGERTHRESHOLD 10  // below this check value we are triggered (no longer see scan stand symbol)
#define	SSTANDLIGHTSTHRESHOLD  25  // above this checkbe value we try to lower illumination (we are sure we see the scan stand symbol)
#define	SCANSTANDLPTO 60000        //scan stand timer im ms
#define	SCAN_STAND_HEAT_TIMEOUT 6000        //scan stand heat mitigation timer im ms
#define	SCAN_STAND_MAX_NUM_IMAGES_FOR_LOW_POWER	4          //MINIMUM SETTING IS 3
#define	SCAN_STAND_HEAT_DECODED_PASS_COUNT	20


#define USB_RECOVERY_TIME 20000

extern bool WasLastSymbolScanStand(void);


/*-----------------5/30/2002 8:26AM-----------------
 * trigger.cpp
 * Implements CTrigger class providing manual and serial trigger
 * as well as auto trigger eventually
 * --------------------------------------------------*/
CTrigger:: CTrigger( void )
{
	TriggerMutex = MT_CreateMutex(false);
	mTriggerMode = -1;
	bSerialTrigger = FALSE;
	bCurrentTriggerState = FALSE;
	FinishedScanning = FINISHED_AND_OFF;
	ReReadTimeout.SetExpiration(0);
   	ReReadTimeout2D.SetExpiration(0);
	TriggeredTimeout.SetExpiration(0);
	PresentationModeTimeout.SetExpiration(1500);
    USBRecoveryTimeout.SetExpiration(USB_RECOVERY_TIME);
	SkipDecodeTimeout.SetExpiration(0);
	GoodReadTimeout.SetExpiration(0);
	pLowPwrTimeout = new CTimeoutmS(0);
	ResetPowerTimeOuts();
	bUseScanStandSymbLowPow = FALSE;            //scan stand found and use scan stand pixel locations
	TriggerThreshold=6;                         //scan stand low power thresholds, number of pixels that must change, max 8
	ChangeThreshold=4;                          //scan stand low power thresholds, value the pixel must change
	ssRestoreExp = ssRestoreExpTemp = 0;        //scan stand low power saved exposure value
	ssRestoreGain = ssRestoreGainTemp = 0;      //scan stand low power saved gain value
	iGetStableImages = 0;                       //scan stand stable image sample counter
	bGetScanStandExp = FALSE;                   //scan stand get low power exposure
	ssMaxExpTimesGain = ssExpTimesGain = 0;     //scan stand best low power image qualifier
	mAimersForcedOn = false;
	mLightsForcedOn = false;
	m_IoControlState=IOC_NONE;
	m_SavedAimerCurrentVals.bAreSaved = false;
	m_PrsModesPDFdetected = FALSE;
	bWeAreLightBarEngine = false;                   //added for streaming
	bPresModeLightBarLowLightOn = false;            //added for streaming
   	TriggeredInAutoOrPresMode = FALSE;
	mBlockTempManualOnTriggerChange = false;
	nAimerState = FALSE;
	nLightsState = FALSE;
	AllDutyCycle = 100;

	bAllignmentModeEnabled = false;
	bAllignmentModeLightsForced = false;
	bAllignmentModeForceSerialTrigger = false;
	uiNoLightCount = 0;
	uiTriggerTimeoutCount = 0;

	mbScanDriverHasFlashLedControl = false;

	lightsduty = 100;

	//************** Optical Trigger Variables*****************************
	BeLessSensitive=0;
	NextFrameSensitivity = 0;

#ifdef FLASH_LEDS
	LedDutyCycle = ( 150 );
#else
	LedDutyCycle = ( 100 );
#endif

	IdleMe=0;
	NoChangeClock = 0;
	DimClock = 0;
	TimeClock = 0;
	ReducedImaging = FALSE;
	RampDownComplete = FALSE;
	LastLedDutyCycle = -1;
	DevalueNextScan = 0;
	OldDecodeCount = 0;
	NumberOfTriggersWithoutDecode = 0;
	BlockDecoderFromExecuting = FALSE;
	ReducedExposure = FALSE;
	//************** End Optical Trigger Variables*****************************

	//************** Heat mitigation functionality ***********************************
	bAllowHeatMitigation = GET_CONFIG_SETTING(HeatMitigationFixedSettings);
	bHeatMitigationHasDecoded = false; //in scan stand, set true after a decode, to enable heat mitigation , otherwise to false
	HeatMitigationHasDecodeOutput = false; //in pres mode, set true after a decode, to disable heat mitigation (dose), otherwise to false when triggered   //Heat Mitigation variables - start
	AllowHeatMitigationCoolDown = false;  //if true, allows heat mitigation for the given HeatMitigationCoolDownTime
	HeatMitigationCoolDownTime = 0;
	GoNumbAfterDecode = 0;   //these next two variables were local variables in optical trigger, change of scope for H.M.
	DelayForPreAiming = 0;         																														//Heat Mitigation - end
	//************** End Heat mitigation functionality *******************************

	//************** variables for  methods: PrepareForScanning, FinishScanning, GetNextScan *********************
	bFirstTime = 0;
	nStartTime = 0;
	mbUseBoostMode = false;
	mbCurrentTriggerStateChangePending = false;
	mStartingExposureManualTrigger = EXPEST_JADE;       //make starting exposure 8 ms 
	//************** end variables for  methods: PrepareForScanning, FinishScanning, GetNextScan *********************

	mUseSSSwitch=false;		// Variable for using magnetic stand switch

	mSavedTriggerMode = MANUAL_TRIGGER;
	mLastTriggerMode = MANUAL_TRIGGER;
	mSavedTriggerModeNotSet = false;

	//trigger specific cell phone settings
	mScanDriverCellPhone = 0;
	mPresentationScanDriverCellPhone = 0;
	mScanAimerMode = 0;
	mCellScanAimerMode = 0;
	mCurrentScanAimerMode = 0;
	mIllOnDuringDecode = -1;

	mPreferredMode = 0;
	mSequenceMode = 0;
	mShotgunMode = 0;	
	mCodeGateMode = false;
	mSerialDisableEnable = false; 	//Serial D/E mode.
}


CTrigger:: ~CTrigger( void )
{
	MT_CloseMutex(TriggerMutex);
	delete pLowPwrTimeout;
}


void CTrigger::OnChangedConfiguration(void)
{
	int CurrentTriggerMode = -1;
	bool bRunSetTriggerMode = false;

	CurrentTriggerMode = GET_SETTING(TriggerMode);

	if( (mLastTriggerMode != CurrentTriggerMode) && (mTriggerMode == TEMP_MANUAL_TRIG) &&
		((mLastTriggerMode == STREAMING_PRESENTATION_TRIGGER) || (mLastTriggerMode == PRESENTATION_TRIGGER)) )
	{
		bRunSetTriggerMode = true;
	}

	if( mLastTriggerMode != CurrentTriggerMode )      //if trigger mode has changed
	{
		mSavedTriggerMode = CurrentTriggerMode;
	}
	else if( mTriggerMode != TEMP_MANUAL_TRIG )     //if trigger mode has not changed, and it is not in temp manual trigger, save the current configured mode
	{
		mSavedTriggerMode = mTriggerMode;
	}

	mLastTriggerMode = CurrentTriggerMode;

	if (!MT_WaitMutex(TriggerMutex,INFINITE))	return;

	if( bAllignmentModeEnabled )
	{
		UnTriggerAllignmentMode();     //clear 4800dr allignment mode if running, leave this line of code at beginning of this method, after MUTEX
	}

	DWORD ModeToSend;

	lightsduty =					GET_SETTING(NonOpticalTrigLedIntensityPercentage);
	mEmiTrigger =					GET_SETTING(EMITestReRead);
	AimerPreDelay =					GET_SETTING(ScanAimerDelay);
	mScanAimerIdleTimeout =			GET_SETTING(ScanAimerIdleTimeout);
	mScanAimerDuration =			GET_SETTING(ScanAimerDuration);
	imagerpwrdntime =				GET_SETTING(ScanDriverTimeout);
//ewr04791 Codegate
	mCodeGateMode = GET_SETTING(bAimerOutOfStandCodeGate);
	mSerialDisableEnable = GET_SETTING(RS232SerialDisableEnable); 
	mAutoaimertimeout =     		GET_SETTING(ScanAutoAimer);
   	mTriggerModifierFlags =   		GET_SETTING(TriggerModeModifierFlag);
	EmiTriggerState = false;
	mUseSSSwitch =					GET_SETTING(ScanStandSwitchEn);
	mStartingExposureManualTrigger = GET_SETTING(StartingExposure);

	//trigger specific cell phone settings
	mScanDriverCellPhone = GET_SETTING(ScanDriverCellPhone);      //enable cell phone for manual trigger
	mPresentationScanDriverCellPhone = GET_SETTING(PresentationScanDriverCellPhone);      //enable cell phone for stream pres trigger
	mScanAimerMode = GET_SETTING(ScanAimerMode);                 //for all triggers except stream pres with cell phone exposure mode
	mCellScanAimerMode = GET_SETTING(CellScanAimerMode);         //for trigger stream pres with cell phone exposure mode emnabled

	mCurrentScanAimerMode = mScanAimerMode;                      //current aimer mode selected, for when we switch to temp manual trigger

	if (mTriggerMode == STREAMING_PRESENTATION_TRIGGER)
	{
		gScanDriverCellPhone = mPresentationScanDriverCellPhone;

		if( mPresentationScanDriverCellPhone )
		{
			mCurrentScanAimerMode = mCellScanAimerMode;
		}

		if( HardwareToken.IAmOEM() ) //in oem, streaming can not have aimer on
		{
			mCurrentScanAimerMode = 0; 
		}
	}
	else
	{
		gScanDriverCellPhone = mScanDriverCellPhone;
	}


#ifdef FLASH_LEDS       //if ff3 gen6 board
#ifndef _SMART_SCAN_DRIVER_FLASH_INTEN_CNTRL		//pass intensity control to scan driver

	if( lightsduty > 100 )     //scan driver does not have control, so no high boost
	{
		lightsduty = 100;
	}
#endif
#endif

	//if we are now disabling prefered mode, sequence mode, or shotgun, and any of them were currently enabled
	if( (mPreferredMode && !GET_SETTING( PrioritySequence )) || (mSequenceMode && !GET_SETTING( SequenceEnabled )) || (mShotgunMode && !GET_SETTING( Shotgun )) )
	{
		PreferredSequenceShotgunModeFinishScanning();                              //Then stop scanning
	}

	//limit light to low power if doing priority sequence in manual trigger
	mPreferredMode = GET_SETTING( PrioritySequence );
	mSequenceMode = GET_SETTING( SequenceEnabled );
	SetShotgunMode(GET_SETTING( Shotgun ));	// also called in SetTriggerMode when changing modes to disable in pres modes

	// no high boost in prefered, sequence, or shotgun manual trigger
	if( (lightsduty > 100) && ( mPreferredMode || mSequenceMode || mShotgunMode ) )
	{
		lightsduty = 100;
	}

	// no high boost in prefered, sequence, or shotgun manual trigger, so starting exposure must be greater than 500us
	if( (mPreferredMode || mSequenceMode || mShotgunMode) )
	{
		mStartingExposureManualTrigger = EXPEST_JADE;       //make exposure 8 ms so we start in low lights
	}

	if (mAutoaimertimeout)
	{
		mScanAimerIdleTimeout=mAutoaimertimeout;
		imagerpwrdntime=0;
	}

	 // Check for 5100 with light bar and also set class variable bWeAreLightBarEngine.
	 AreWeLightBarEngine();

	if( GET_SETTING(PresModeMitigationLowLights) > 0 )       //If > 0, then lights do not go off in pres mode
	{
	 	bPresModeLightBarLowLightOn = true;	//if low lights on, we configure pres mode differently, so lights only dim
	}
	else
	{
	 	bPresModeLightBarLowLightOn = false;
	}

	//check led state so if lights are off, we can still see with a higher exposure set
	int iLastIllOnDuringDecode = mIllOnDuringDecode;
	mIllOnDuringDecode = GET_SETTING(IllOnDuringDecode);         //for all triggers which allow LEDS to be off while scanning
	//ken @2012.03.19 for SCNLED menu command in trgmod3
	if(HardwareToken.IAmVoyager1400() && 1 == mIllOnDuringDecode && mTriggerMode == PRESENTATION_TRIGGER)
	{
		//This code make the HI2D can respond the SCNLED1 in trgmod3 when the device works in SCNLED0 mode.
		((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_flash.turnon((ScanAimerIllumMode_t)GET_SETTING(ScanAimerMode), mIllOnDuringDecode/*, ((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_LedCtrlMode*/);
	}
	if( (mIllOnDuringDecode != iLastIllOnDuringDecode) && (iLastIllOnDuringDecode != -1) )        //if the lights state has changed
	{
		DWORD	DataToSend[6];
		DataToSend[0] = ES_MAX_EXP;
		DataToSend[2] = ES_FIXED_EXP;

		if( mIllOnDuringDecode ||GET_SETTING(EnConnecttoExt))      //lights are being turned on
		{
			DataToSend[3] = GET_SETTING(FixedExposure);

			// If we are manual triggered, then we want to start with our "starting" exposure and gain, and if
			// needed, we will allow it to expand out to the maximum exposure and gain latter.
			switch (mTriggerMode)
			{
				case MANUAL_TRIGGER:
				case LOW_POWER:
				case TEMP_MANUAL_TRIG:
				case NOTIFY_HOST:
				case SNAP_AND_SHIP_TRIGGER:
					DataToSend[1] = mStartingExposureManualTrigger;
					break;
				case SCANSTAND_TRIGGER:
					DataToSend[1] = GET_SETTING(MaximumExposureAuto);
					break;
				case STREAMING_PRESENTATION_TRIGGER:
					DataToSend[1] = GET_SETTING(PresentationExposure);
					DataToSend[3] = GET_SETTING(PresentationFixedExposure);
					break;
				case PRESENTATION_TRIGGER:
					if( bWeAreLightBarEngine )  // Adjust for 5100 with light bar
					{	// We have a PSOC for the 5100 engine with off axis light bar--adjust settings for it.
						DataToSend[1] = GET_SETTING(PresentationExposure);          //exposure
					}
					else
					{
						DataToSend[1] = GET_SETTING(MaximumExposureAuto);                          //exposure
					}
			   		break;
				default:
					DataToSend[1] = GET_SETTING(MaximumExposure);
					break;
			}
			//xputstring(" L_ON \r\n");                                //debug lmc
		}
		else		//else lights are being turned off
		{
			SetLights(0,-1,-1,-1);                  //lights off
			DataToSend[1] = EXPEMX_JADE_MANUAL;     //12ms exposure for seeing in the dark
			DataToSend[3] = EXPEMX_JADE_MANUAL;
			//xputstring(" L_OFF \r\n");                                //debug lmc
		}
		//
		//Set Exposure Settings
		//
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)DataToSend,sizeof(DataToSend),NULL,0,NULL);
		//xputstring(" L_SET \r\n");                                //debug lmc
	}

	// Check product config setting to see if heat mitigation is enabled
	bAllowHeatMitigation = GET_CONFIG_SETTING(HeatMitigationFixedSettings);

	//Adjust light bar engine exposure mode if in streaming pres mode
	if( bWeAreLightBarEngine && (mTriggerMode == STREAMING_PRESENTATION_TRIGGER) )
	{
		 ModeToSend = GET_SETTING( ExposureModeStreaming );	//HHPSD_AUTOEXPOSURE_HHP_PRESENTATION;
		 SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_MODE,(PBYTE)&ModeToSend,1*sizeof(DWORD),NULL,0,NULL);
	}

	//Adjust light bar engine exposure mode if in pres mode
	else if( bWeAreLightBarEngine && (mTriggerMode == PRESENTATION_TRIGGER) )
	{
#if defined(HI2D_T)
		if(GET_SETTING(PresentationScanDriverCellPhone))
		{
			ModeToSend = HHPSD_CELLPHONE;
		}
		else
		{
			if( bPresModeLightBarLowLightOn )     //if low lights are allowed on in Pres mode
			{
				ModeToSend = GET_SETTING( ExposureModePresLowLights );	//HHPSD_AUTOEXPOSURE_HHP_PRESENTATION;;
			}
			else		                               //no lights in pres mode idle
			{
				//flash Illum always uses limited exposure mode of less than 500uS
				#ifdef FLASH_LEDS       //if ff3 gen6 board
				ModeToSend = GET_SETTING( ExposureModePresNoLights );	//HHPSD_AUTOEXPOSURE_HHP_PRESENTATION;
				#else
				ModeToSend = HHPSD_AUTOEXPOSURE_USE_HHP;
				#endif                  //end if ff3 gen6 board
			}
		}
#else
	 	if( bPresModeLightBarLowLightOn )     //if low lights are allowed on in Pres mode
		{
			ModeToSend = GET_SETTING( ExposureModePresLowLights );	//HHPSD_AUTOEXPOSURE_HHP_PRESENTATION;;
		}
		else		                               //no lights in pres mode idle
		{
			//flash Illum always uses limited exposure mode of less than 500uS
			#ifdef FLASH_LEDS       //if ff3 gen6 board
				ModeToSend = GET_SETTING( ExposureModePresNoLights );	//HHPSD_AUTOEXPOSURE_HHP_PRESENTATION;
			#else
				ModeToSend = HHPSD_AUTOEXPOSURE_USE_HHP;
			#endif                  //end if ff3 gen6 board
		}
#endif

		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_MODE,(PBYTE)&ModeToSend,1*sizeof(DWORD),NULL,0,NULL);
	}
	else		//not a pres mode
	{
#if defined(HI2D_T)
		if(GET_SETTING(PresentationScanDriverCellPhone))
		{
			ModeToSend = HHPSD_CELLPHONE;
		//	theStdInterface.printf("Xu\r\n");
		}
		else
		{
			ModeToSend = GET_SETTING(ExposureMode);
		}
#else
			ModeToSend = GET_SETTING(ExposureMode);
#endif
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_MODE,(PBYTE)&ModeToSend,1*sizeof(DWORD),NULL,0,NULL);
	}

	// Adjust for engine with light bar (the fact we're in STREAMING_PRESENTATION_TRIGGER means we have a light bar)
    if( bWeAreLightBarEngine && ((GET_SETTING(TriggerMode)==STREAMING_PRESENTATION_TRIGGER) || (GET_SETTING(TriggerMode)==PRESENTATION_TRIGGER)) )
	{
	    AdjustAimerCurrentsForStreamingPresentationMode();      //lower aimer currents if in these trigger modes
	}

	SetGoodReadTimeout();
	SetReReadTimeout();
	SetReReadTimeout2D();
	SetSkipDecodeTimeout();
	SetLowPwrTimeout();
	SetSerialTriggerTimeout();
	mEmiTimeout.SetExpiration((mSerialTriggerTimeout)?mSerialTriggerTimeout:1000);
	TriggeredTimeout.SetExpiration(GET_SETTING(PresModeTimeOut));

	if( (mTriggerMode != TEMP_MANUAL_TRIG) || bRunSetTriggerMode )
	{
		SetTriggerMode();
	}

	MT_ReleaseMutex(TriggerMutex);
}

// Disable shotgun mode while in presentation modes (NG2D-1013)
// Re-enable shotgun mode when we are in manual trigger modes
void CTrigger::SetShotgunMode(int ShotgunSetting)
{
	switch(mTriggerMode)
	{
		case SCANSTAND_TRIGGER:
		case STREAMING_PRESENTATION_TRIGGER:
		case PRESENTATION_TRIGGER:
			mShotgunMode = 0;
			break;
		default:
			mShotgunMode = ShotgunSetting;
			break;
	};
}

int CTrigger::GetShotgunMode( void )
{
	if( pTrigger != NULL )
		return pTrigger->mShotgunMode;
	else
		return 0;
}

void CTrigger::SetSerialTriggerTimeout(int timeout)
{

	if( bAllignmentModeForceSerialTrigger )
	{
		mSerialTriggerTimeout = 0;
	}
	else
	{
		mSerialTriggerTimeout = (timeout==0 || timeout>250)?timeout:250;
	}

	SerialTriggerTimeout.SetExpiration(mSerialTriggerTimeout);
}


void CTrigger::SetGoodReadTimeout(int timeout)
{
	if( bAllignmentModeForceSerialTrigger )
	{
		mGoodReadTimeout = 0;
	}
	else
	{
		mGoodReadTimeout = timeout;
	}

	GoodReadTimeout.SetExpiration(0);
}


void CTrigger::SetReReadTimeout(int timeout)
{
	mReReadTimeout = ForceReadTimersForAllignmentMode(timeout);
	ReReadTimeout.SetExpiration(mReReadTimeout);
}


/*--------------------------------------------------
 * The 2D reread delay will work slightly differently than the original reread delay.
 * -If the 2d reread delay is set to the minimum (0ms) that signifies that it is disabled
 *   and the original (1D) reread delay should be used instead.
 * -If the 2D reread delay is set to anything greater than 0ms then use that delay for 2D codes
 * -By default the 2D reread delay is set to 0ms
 * --------------------------------------------------*/
void CTrigger::SetReReadTimeout2D(void)
{
    if( GET_SETTING(ReReadDly2D) != 0 )
    {
        SetReReadTimeout2D(GET_SETTING(ReReadDly2D));
    }
    else    // If the 2D delay is set to zero use the standard delay
    {
        SetReReadTimeout2D(GET_SETTING(ReReadDly));
    }
}


void CTrigger::SetReReadTimeout2D(int timeout)
{
    mReReadTimeout2D = ForceReadTimersForAllignmentMode(timeout);
    ReReadTimeout2D.SetExpiration(mReReadTimeout2D);
}


void CTrigger::SetSkipDecodeTimeout(int timeout)
{
	mSkipDecodeTimeout=timeout;
	SkipDecodeTimeout.SetExpiration(mSkipDecodeTimeout);
}


void CTrigger::SetLowPwrTimeout(int timeout)
{
	mLowPwrTimeout=timeout;
	pLowPwrTimeout->SetExpiration(mLowPwrTimeout*1000L);
}


void CTrigger::SetSerialTriggerTimeout(void)
{
		SetSerialTriggerTimeout(GET_SETTING(SerialTrigTimeOut));
}


void CTrigger::SetReReadTimeout(void)
{
	SetReReadTimeout(GET_SETTING(ReReadDly));
}


void CTrigger::SetSkipDecodeTimeout(void)
{
	SetSkipDecodeTimeout(GET_SETTING(SkipDecodingTimeout));
}


void CTrigger::SetGoodReadTimeout(void)
{
	SetGoodReadTimeout(GET_SETTING(GoodReadDly));
}


void CTrigger::SetLowPwrTimeout(void)
{
	SetLowPwrTimeout(GET_SETTING(LowPowerTimeOut));
}



/* set trigger mode to user specified trigger mode */
void CTrigger::SetTriggerMode(int triggermode)
{
	int oldtriggermode = mTriggerMode;
	DWORD	DataToSend[24];
	DWORD	ModeToSend;
	bool bReconfigureTriggerMode = false;

	if( bAllignmentModeEnabled )
	{
		UnTriggerAllignmentMode();      //clear 4800dr allignment mode if running, leave this line of code at beginning of this method
		bReconfigureTriggerMode = true;
	}

	bCurrentTriggerState = FALSE;
	mbCurrentTriggerStateChangePending = false;
	bSerialTrigger = false;
	mTriggerMode = triggermode;
	Set_AllDutyCycle( lightsduty );                       //set LED duty cycle from menu value
	validscanpending = false;

	ScanStandState = WAITFORSCANSTAND;
	bUseScanStandSymbLowPow = FALSE;                              //scan stand found and use scan stand pixel locations
	ssRestoreExp = ssRestoreExpTemp = 0;                           //scan stand low power saved exposure value
	ssRestoreGain = ssRestoreGainTemp = 0;                          //scan stand low power saved gain value
	iGetStableImages = 0;                                               //scan stand stable image sample counter
	bGetScanStandExp = FALSE;                                        //scan stand get low power exposure
	ssMaxExpTimesGain = ssExpTimesGain = 0;                         //scan stand best low power image qualifier

	BlockDecoderFromExecuting = FALSE;
	mBlockTempManualOnTriggerChange = false;
	pSSTimeout.SetExpiration(SCANSTANDLPTO);
	pSSHMTimeout.SetExpiration(SCAN_STAND_HEAT_TIMEOUT);
	pmScanAimerIdleTimeout.SetExpiration(mScanAimerIdleTimeout);
	SetSerialTriggerTimeout();
	pAimerDurationTimout.SetExpiration(mScanAimerDuration);
	pImagerIdleTimeout.SetExpiration(imagerpwrdntime);
	SkipDecodeTimeout.SetExpiration(0);

	ResetOpticalTrigger();       //pres mode reset settings
	GoToNormalImaging(); //pres mode normal mode setup,  Note: not sure why needed since Reset Optical trigger resets ReducedImaging, so this function does nothing

	DataToSend[0]=ES_TARGET_VALUE;       											// will try to lock a value above target white in scan stand
	DataToSend[2]=ES_MAX_EXP;
	DataToSend[4]=ES_MAX_GAIN;
	DataToSend[6]=ES_EXPOSURE_METHOD;
	DataToSend[8]=ES_SPECULAR_EXCLUSION;
	DataToSend[10]=ES_TARGET_PERCENTILE;
	DataToSend[12]=ES_CONFORM_IMAGE;
	DataToSend[14]=ES_ADJUST_EXP_WHEN_ILLUM_OFF;
	DataToSend[16]=ES_CONFORM_TRIES;
	DataToSend[18]=ES_FIXED_EXP;
	DataToSend[20]=ES_FIXED_GAIN;
	DataToSend[22]=ES_EXPOSURE_CELL;

		/*set the exposure settings */
	DataToSend[1]=GET_SETTING(TargetWhite);

	if ((GET_CONFIG_SETTING(iOpticsPopulation)==TWO_D) && (GET_SETTING(DecodeMode) == 2))
		DataToSend[1] = GET_SETTING(AldTargetWhite);

	DataToSend[7]=GET_SETTING(ExposureMethod);
	DataToSend[9]=GET_SETTING(SpecularDetection);
	DataToSend[11]=GET_SETTING(TargetPercentile);
	DataToSend[19] = GET_SETTING(FixedExposure);
	DataToSend[21] = GET_SETTING(FixedGain);
	if(HardwareToken.IAmOEMJade())		// for OEM read Engine ID to tell the len filter
	{
		if(GET_CONFIG_SETTING(iWindowType) == WINDOW_CLEAR)
		{
				PSoC_5100_REGISTER_TABLE Registers;
		
			if(Psoc5100GetRegisters( &Registers ))
			{
				if(0x01 == Registers.EngineID.b.LensFilter)		// we have a filter
					// 5ms exposre
					DataToSend[23] = (MAX_ES_CELL_TRI_FILTER * GET_SETTING(ExposureCellphone))/100;
				else										// no filter
					// 3ms
					DataToSend[23] = (MAX_ES_CELL_CLEAR_WIN * GET_SETTING(ExposureCellphone)) /100;
			}
			else
			{
				if(GET_SETTING(GeneralDebugOn))
				{
					theStdInterface.printf("Failed to read PSOC regs\r\n");
				}
			}
		}
		else if(GET_CONFIG_SETTING(iWindowType) == WINDOW_RED)
		{
			// Max 15.2 ms exposure
				DataToSend[23] = (MAX_ES_CELL_RED_WIN * GET_SETTING(ExposureCellphone)) /100;
		}
	}
	else
	{
		if(GET_CONFIG_SETTING(iWindowType) == WINDOW_CLEAR)
		{
			if(GET_CONFIG_SETTING(iLenFilterType) == LEN_GLASS_TRIBAND)
				// 5ms exposre
				DataToSend[23] = (MAX_ES_CELL_TRI_FILTER * GET_SETTING(ExposureCellphone))/100;
			else
				// 3ms
				DataToSend[23] = (MAX_ES_CELL_CLEAR_WIN * GET_SETTING(ExposureCellphone)) /100;
		}
		else if(GET_CONFIG_SETTING(iWindowType) == WINDOW_RED)
		{
			// Max 15.2 ms exposure
				DataToSend[23] = (MAX_ES_CELL_RED_WIN * GET_SETTING(ExposureCellphone)) /100;
		}
	}
	

	 AreWeLightBarEngine();

	 if( GET_SETTING(PresModeMitigationLowLights) > 0 )            //If > 0, then lights do not go off in pres mode
	 {
		bPresModeLightBarLowLightOn = true;	//if low lights on, we configure pres mode differently, so lights only dim
	 }
	 else
	 {
	 	bPresModeLightBarLowLightOn = false;
	 }

	 gScanDriverCellPhone = mScanDriverCellPhone;
	 mCurrentScanAimerMode = mScanAimerMode;

	// If we are manual triggered, then we want to start with our "starting" exposure and gain, and if
	// needed we will allow it to expand out to the maximum exposure and gain.
	switch (mTriggerMode)
	{
		case MANUAL_TRIGGER:
		case LOW_POWER:
		case TEMP_MANUAL_TRIG:
		case NOTIFY_HOST:
		case SNAP_AND_SHIP_TRIGGER:

				DataToSend[3] = mStartingExposureManualTrigger;

				DataToSend[5]=GET_SETTING(StartingGain);

				if( GET_CONFIG_SETTING(iOpticsPopulation)==TWO_D && GET_SETTING(DecodeMode) == 2 )
				{
					DataToSend[5]=GET_SETTING(AldStartingGain);
				}

				DataToSend[13]=GET_SETTING(bImageMustConform);
				DataToSend[15] = ( (!GET_SETTING(AutoExposureLightsOffAdjust)) && ( mIllOnDuringDecode && (GET_SETTING(NonOpticalTrigLedIntensityPercentage) > 0) )?(FALSE):(TRUE) );
				DataToSend[17]=GET_SETTING(ImageSnapAttempts);
				//theStdInterface.printf("changed to manual mode\r\n");
				//turnon_aimer();//ken @2012.01.19?????
				if ( HardwareToken.IAmVoyager1400() )
				{	
					((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_current.enter_high();
					((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_current.enterHi2dCurrentLevel(GET_SETTING(NonOpticalTrigLedIntensityPercentage));
				}
				break;

		case SCANSTAND_TRIGGER:
				DataToSend[3]=GET_SETTING(MaximumExposureAuto);
				DataToSend[5]=GET_SETTING(MaximumGainAuto);
				DataToSend[13]=TRUE;                     //SCAN STAND IMAGE MUST CONFORM TO MINIMUM WHITE
				DataToSend[15]=TRUE;                     		//ADJUST_EXP_WHEN_ILLUM_OFF
				DataToSend[17]=5;            		//we will try to conform 5 times, if not conform, scan driver will return a non conform image before trying to conform again (if gain is max, it will always return an image
				break;
		case STREAMING_PRESENTATION_TRIGGER:
				DataToSend[1]=GET_SETTING(TargetWhite);
				if (m_PrsModesPDFdetected)
				{
					#ifdef FLASH_LEDS
						DataToSend[3]=GET_SETTING(PresentationExposure);
						DataToSend[5]=GET_SETTING(PresentationGain);
						DataToSend[19] = GET_SETTING(PresentationFixedExposure);
						DataToSend[21] = GET_SETTING(PresentationFixedGain);
					#else
						DataToSend[3]=GET_SETTING(MaximumExposureAuto);
						DataToSend[5]=GET_SETTING(MaximumGainAuto);
						DataToSend[19] = GET_SETTING(FixedExposure);
						DataToSend[21] = GET_SETTING(FixedGain);
					#endif
				}
				else
				{
					DataToSend[3]=GET_SETTING(PresentationExposure);
					DataToSend[5]=GET_SETTING(PresentationGain);
					DataToSend[19] = GET_SETTING(PresentationFixedExposure);
					DataToSend[21] = GET_SETTING(PresentationFixedGain);
				}

				DataToSend[13]=FALSE;
				DataToSend[15]=FALSE;
				DataToSend[17]=5;                          //we will try to conform 5 times, dont think this is needed but want to initialize it to known value

				gScanDriverCellPhone = mPresentationScanDriverCellPhone;

				if( mPresentationScanDriverCellPhone )
				{
					mCurrentScanAimerMode = mCellScanAimerMode;
				}

				if( HardwareToken.IAmOEM() ) //in oem, streaming can not have aimer on
				{
					mCurrentScanAimerMode = 0; 
				}
				break;
		case PRESENTATION_TRIGGER:
				if( bWeAreLightBarEngine )  // Adjust for 5100 with light bar
				{	// We have a PSOC for the 5100 engine with off axis light bar--adjust settings for it.
					DataToSend[3]=GET_SETTING(PresentationExposure);          //exposure
					DataToSend[5]=GET_SETTING(PresentationGain);              //gain
					DataToSend[13]=true;                      					//conform
					DataToSend[17]=4;                      						//conform tries
					DataToSend[15]=true;                                    //AutoExposureLightsOffAdjust
				}
				else
				{
					DataToSend[3]=GET_SETTING(MaximumExposureAuto);                          //exposure
					DataToSend[5]=GET_SETTING(MaximumGainAuto);                              //gain
					DataToSend[13]=GET_SETTING(bImageMustConform);                      //conform
					DataToSend[17]=GET_SETTING(ImageSnapAttempts);                      //conform tries
					DataToSend[15]=true;                                                //AutoExposureLightsOffAdjust
				}
				if ( HardwareToken.IAmVoyager1400() )
				{	
					((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_current.enter_high();
					((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_current.enterHi2dCurrentLevel(ILLUM_LED_LEVL4);
				}

				break;
		default:
				DataToSend[3]=GET_SETTING(MaximumExposure);
				DataToSend[5]=GET_SETTING(MaximumGain);
				DataToSend[13]=GET_SETTING(bImageMustConform);
				DataToSend[15]=TRUE;
				DataToSend[17]=GET_SETTING(ImageSnapAttempts);
				break;
	}

	if( (!mIllOnDuringDecode)&&(!GET_SETTING(EnConnecttoExt)) )      //lights are off  
	{
		if( DataToSend[3] < EXPEMX_JADE_MANUAL )
		{
			DataToSend[3] = EXPEMX_JADE_MANUAL;     //12ms exposure for seeing in the dark
		}

		if( DataToSend[19] < EXPEMX_JADE_MANUAL )
		{
			DataToSend[19] = EXPEMX_JADE_MANUAL;     //12ms exposure for seeing in the dark
		}
	}

	//
	//Set Exposure Settings
	//
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)DataToSend,sizeof(DataToSend),NULL,0,NULL);

	
	//
	//If trigger mode has changed
	//
	if( (oldtriggermode != mTriggerMode) || bReconfigureTriggerMode )
	{
		mAimersForcedOn = FALSE;           //set the test mode for aimers forced on to the off state
		mLightsForcedOn = FALSE;           //set the test mode for LEDs forced on to the off state

		if( bWeAreLightBarEngine && ((mTriggerMode==STREAMING_PRESENTATION_TRIGGER) || (mTriggerMode==PRESENTATION_TRIGGER)) )
		{                                                                 // For 5100 with light bars.  Save original aimer current vals,
			AdjustAimerCurrentsForStreamingPresentationMode();
		}
		else if( bWeAreLightBarEngine && ((oldtriggermode==STREAMING_PRESENTATION_TRIGGER) || (oldtriggermode==PRESENTATION_TRIGGER)) && m_SavedAimerCurrentVals.bAreSaved )
		{                        // restore original aimer current vals that were saved when we went into STREAMING_PRESENTATION_TRIGGER or regular pres using light bar
			//UpdatePsocRegisters(PSOC_OFFSET_AIMER_CURRENT_OPER_MAX, &m_SavedAimerCurrentVals.OperMax, 1);      //lmc   fixme
			//UpdatePsocRegisters(PSOC_OFFSET_AIMER_CURRENT_OPER, &m_SavedAimerCurrentVals.Operational, 1);       //lmc   fixme
			m_SavedAimerCurrentVals.bAreSaved = false;  // set to false to indicate we've restored them
		}
		
		Set_nLightsState( false );
		Set_nAimerState( false );

		bHeatMitigationHasDecoded = false; //in scan stand, set true after a decode, to enable heat mitigation , otherwise to false
		HeatMitigationHasDecodeOutput = false; //in pres mode, set true after a decode, to disable heat mitigation (dose), otherwise to false when triggered   //Heat Mitigation variables - start
		AllowHeatMitigationCoolDown = false;  //if true, allows heat mitigation for the given HeatMitigationCoolDownTime
		HeatMitigationCoolDownTime = 0;

		SetShotgunMode(GET_SETTING(Shotgun));	// Set shotgun mode here, it needs to be disabled in presentation modes

		switch (mTriggerMode)
		{
			case SCANSTAND_TRIGGER:
				OldDecodeCount = NumberOfDecodes;        //update our decode counter					//added for Heat Mitigation
			case AUTOMATIC_TRIGGER:
			case PRESENTATION_TRIGGER:
			case STREAMING_PRESENTATION_TRIGGER:
				  	Set_nLightsState( mIllOnDuringDecode );					
					if ( HardwareToken.IAmVoyager1400() )
					{	
						((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_current.enter_high();
						((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_current.enterHi2dCurrentLevel(ILLUM_LED_LEVL4);
					}
					if( oldtriggermode == MANUAL_TRIGGER ) //if changing from manual to auto trigger, we do not want a double read of menu barcode
					{
						mBlockTempManualOnTriggerChange = true;    //if we just changed to this mode, we do not want to go into temp manual if we are still holding trigger after menuing
						BlockDecoderFromExecuting = true;
					}

					if (FinishedScanning != ACTIVE)
					{
						bool wasoff = (FinishedScanning==FINISHED_AND_OFF);
				  		FinishedScanning = ACTIVE;
				  		if (wasoff)
						{							   
							//theStdInterface.printf(" Start Scan \r\n");              //lmc debug	
							StartImaging(); //Former SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_START_SCANNING,NULL,0,NULL,0,NULL); //Heat Mitigation
							//theStdInterface.printf(" Start Scan... \r\n");              //lmc debug	
							if(HardwareToken.IAmVoyager1400())
							{
							 	((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_flash.turnon((ScanAimerIllumMode_t)GET_SETTING(ScanAimerMode), GET_SETTING(IllOnDuringDecode)/*, ((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_LedCtrlMode*/);
								((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_aimer.turnon((ScanAimerIllumMode_t)GET_SETTING(ScanAimerMode));
							}
							if( HardwareToken.IAmOEM() )        //in oem, need to set lights before starting imager, or we miss lights in first frame
							{
								if( !AreWeUsbTermId() || (AreWeUsbTermId() && theStdInterface.DeviceIoControl(IOCTRL_AM_I_ENUMERATED)) )
								{
									SetLights( (mLightsForcedOn)?true:nLightsState, AllDutyCycle, (mAimersForcedOn)?true:nAimerState, (mAimersForcedOn)?100:AllDutyCycle);
								}									
			    			}
						}
					}
					break;
			case TEMP_MANUAL_TRIG:
				theScriptInterpreter.ResetScanningState();
				TriggeredTimeout.Reset();

			case MANUAL_TRIGGER:
				if ( HardwareToken.IAmVoyager1400() )
				{	
					((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_current.enter_high();
					((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_current.enterHi2dCurrentLevel(GET_SETTING(NonOpticalTrigLedIntensityPercentage));
				}
				if( HardwareToken.IAmOEM() )
				{
					SetLights( (mLightsForcedOn)?true:nLightsState, AllDutyCycle, (mAimersForcedOn)?true:nAimerState, (mAimersForcedOn)?100:AllDutyCycle);
			    }
			case LOW_POWER:
				if (FinishedScanning != FINISHED_AND_OFF)
				{					
					FinishedScanning = FINISHED_AND_OFF; 					
					StopImaging(); // Former SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_STOP_SCANNING,NULL,0,NULL,0,NULL);    //Heat Mitigation
				}	
			case SNAP_AND_SHIP_TRIGGER:
			case NOTIFY_HOST:
					break;

		}
	}


	//
	//Set up unique exposure modes according to the trigger mode selected
	//
	if( bWeAreLightBarEngine && (mTriggerMode == STREAMING_PRESENTATION_TRIGGER) )
	{
		//if (m_PrsModesPDFdetected) ModeToSend = HHPSD_AUTOEXPOSURE_USE_HHP;
		//else ModeToSend = HHPSD_AUTOEXPOSURE_HHP_PRESENTATION;

		if (m_PrsModesPDFdetected)
		{
			//flash Illum always uses limited exposure mode of less than 500uS
			#ifdef FLASH_LEDS       //if ff3 gen6 board
	  			ModeToSend = GET_SETTING( ExposureModeStreaming );	//HHPSD_AUTOEXPOSURE_HHP_PRESENTATION;
	  		#else
	  			ModeToSend = HHPSD_AUTOEXPOSURE_USE_HHP;
	  		#endif                  //end if ff3 gen6 board
		}
		else
		{
			ModeToSend = GET_SETTING( ExposureModeStreaming );	//HHPSD_AUTOEXPOSURE_HHP_PRESENTATION;
		}

		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_MODE,(PBYTE)&ModeToSend,1*sizeof(DWORD),NULL,0,NULL);
	}
	else if( bWeAreLightBarEngine && (mTriggerMode == PRESENTATION_TRIGGER) )
	{
#if defined(HI2D_T)
		if(GET_SETTING(PresentationScanDriverCellPhone))
		{
			ModeToSend = HHPSD_CELLPHONE;
			//theStdInterface.printf("Xu\r\n");
		}
		else
		{
			if( bPresModeLightBarLowLightOn )         //If Pres mode Low lights
			{
				ModeToSend = GET_SETTING( ExposureModePresLowLights );	//HHPSD_AUTOEXPOSURE_HHP_PRESENTATION;
			}
			else                             //If Pres mode No lights
			{
				//flash Illum always uses limited exposure mode of less than 500uS
#ifdef FLASH_LEDS       //if ff3 gen6 board
				ModeToSend = GET_SETTING( ExposureModePresNoLights );	//HHPSD_AUTOEXPOSURE_HHP_PRESENTATION;
#else
				ModeToSend = HHPSD_AUTOEXPOSURE_USE_HHP;
#endif                  //end if ff3 gen6 board
			}
		}
#else
		if( bPresModeLightBarLowLightOn )         //If Pres mode Low lights
		{
			ModeToSend = GET_SETTING( ExposureModePresLowLights );	//HHPSD_AUTOEXPOSURE_HHP_PRESENTATION;
		}
		else                             //If Pres mode No lights
		{
			//flash Illum always uses limited exposure mode of less than 500uS
			#ifdef FLASH_LEDS       //if ff3 gen6 board
				ModeToSend = GET_SETTING( ExposureModePresNoLights );	//HHPSD_AUTOEXPOSURE_HHP_PRESENTATION;
			#else
				ModeToSend = HHPSD_AUTOEXPOSURE_USE_HHP;
			#endif                  //end if ff3 gen6 board
		}
#endif

		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_MODE,(PBYTE)&ModeToSend,1*sizeof(DWORD),NULL,0,NULL);
	}
	else
	{
#if defined(HI2D_T)
		if(GET_SETTING(PresentationScanDriverCellPhone))
		{
			ModeToSend = HHPSD_CELLPHONE;
			//theStdInterface.printf("Xu\r\n");
		}
		else
		{
			ModeToSend = GET_SETTING(ExposureMode);
		}
#else
		ModeToSend = GET_SETTING(ExposureMode);
#endif
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_MODE,(PBYTE)&ModeToSend,1*sizeof(DWORD),NULL,0,NULL);
	}
}


/* set trigger mode from menu  */
void CTrigger::SetTriggerMode(void)
{
	if ((GET_SETTING(TriggerMode) == SNAP_AND_SHIP_TRIGGER || GET_SETTING(TriggerMode) == NOTIFY_HOST)
	  		&& (theStdInterface.GetTerminalID() != TRM_ASYNC_ASCII) && (theStdInterface.GetTerminalID() != TRM_USB_COMEMU) )
	{
		SetTriggerMode(0);
		mSavedTriggerMode = 0;
		mLastTriggerMode = 0;
		RAM_SET_ACTUAL(TriggerMode,0);
	}
	else
	{
		SetTriggerMode(GET_SETTING(TriggerMode));
	}
}


void CTrigger::PrepareForScanning(void)
{
	if( ((MANUAL_TRIGGER==mTriggerMode) || (LOW_POWER==mTriggerMode) || (TEMP_MANUAL_TRIG==mTriggerMode)) && (FinishedScanning != ACTIVE) )
	{
		bool wasoff = (FinishedScanning == FINISHED_AND_OFF);
		bool hi2d_state = (FinishedScanning == FINISHED);//scan driver is not timeout.
		FinishedScanning=ACTIVE;
		nStartTime = current_time();
		bFirstTime = 1;
		mbUseBoostMode = false;
		if(hi2d_state && HardwareToken.IAmVoyager1400())//the scan driver still not time out.
		{
			((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_flash.turnon((ScanAimerIllumMode_t)GET_SETTING(ScanAimerMode), GET_SETTING(IllOnDuringDecode)/*, ((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_LedCtrlMode*/);
			((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_aimer.turnon((ScanAimerIllumMode_t)GET_SETTING(ScanAimerMode));
		}
		if (wasoff)
		{
		#ifndef HI2D_T//for hi2d no boost to charge
			if( (mStartingExposureManualTrigger <= 4) && (lightsduty > 100) )       //go to high boost if starting exposure is <= 4, so we start next frame in high
			{
				mbUseBoostMode = true;

				#ifdef FLASH_LEDS       //if ff3 gen6 board				
				if(!(HardwareToken.IAmOEM()))
				{				
					CTimeoutmS		BoostWaitTimeout(100);
					bool isHigh = false;

					if( !(GetILL_FlashMode()) )
					{
						SetILL_FlashMode(1);
					}

					while( !BoostWaitTimeout.HasExpired() && !isHigh )       //wait for boost to charge
					{
						isHigh = IsLightCurrentStateChangedToHigh();
						MT_Delay(1);	//sleep thread for 1 ms
					}

					//theStdInterface.printf( " Wait for high lights done:%d \r\n", isHigh);
				
				}									   
				#endif
			}
		#endif
			 //theStdInterface.printf("START IMG\r\n");
			 //open the illumination before start imaging
			if(HardwareToken.IAmVoyager1400())
			{
				((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_flash.turnon((ScanAimerIllumMode_t)GET_SETTING(ScanAimerMode), GET_SETTING(IllOnDuringDecode)/*, ((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_LedCtrlMode*/);
				((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_aimer.turnon((ScanAimerIllumMode_t)GET_SETTING(ScanAimerMode));
			}
			StartImaging(); // Former SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_START_SCANNING,NULL,0,NULL,0,NULL);    //Heat Mitigation
		}
	}
	else if(mCodeGateMode ||  0 == imagerpwrdntime)//add by ken for code gate HI2D @2012.03.09
	{
		if(HardwareToken.IAmVoyager1400())
		{
			((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_flash.turnon((ScanAimerIllumMode_t)GET_SETTING(ScanAimerMode), GET_SETTING(IllOnDuringDecode)/*, ((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_LedCtrlMode*/);
			((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_aimer.turnon((ScanAimerIllumMode_t)GET_SETTING(ScanAimerMode));
		}
	}
	else
	{
		//theStdInterface.printf("%d   %d\r\n", mCodeGateMode, imagerpwrdntime);
	}
}


void CTrigger::FinishScanning(void)
{
	DWORD ExpVals[] = {ES_MAX_EXP,0,ES_MAX_GAIN,0,ES_TARGET_VALUE,0};
	
	if( (FinishedScanning == ACTIVE) && ( (MANUAL_TRIGGER==mTriggerMode) || (LOW_POWER==mTriggerMode) || (TEMP_MANUAL_TRIG==mTriggerMode) ) )
	{
		if( imagerpwrdntime == 1 )          //if "SDRTIM" == 1, scanning off immediately (note "0" means on forever, > 1 is using a timer, in mSec)
		{
			if( (mPreferredMode || mSequenceMode || mShotgunMode) && ((MANUAL_TRIGGER==mTriggerMode) || (LOW_POWER==mTriggerMode)) )
			{
				FinishedScanning = FINISHED;
				//theStdInterface.printf(" TrigFin \r\n");          //lmc debug
			}
			else
			{
				FinishedScanning = FINISHED_AND_OFF;  /*prevents us from call this stop scanning over and over and over */
				StopImaging(); // Former SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_STOP_SCANNING,NULL,0,NULL,0,NULL);    //Heat Mitigation
				Set_nLightsState( false );
				Set_nAimerState( false );
				//theStdInterface.printf(" TrigFinOff \r\n");              //lmc debug
			}

			if( (mStartingExposureManualTrigger <= 4) && (lightsduty > 100) && mbUseBoostMode )       //go to high boost if starting exposure is <= 4, so we start next frame in high
			{
				#ifdef FLASH_LEDS       //if ff3 gen6 board				  
					if( !(HardwareToken.IAmOEM()) )
				    {
						SetILL_FlashMode(1);		//early charge up for next frame
				    }
					else if( HardwareToken.IAmOEMJade() )
					{
						MT_Delay(16);           //if we are OEM with psoc, we need 16 ms to recharge boost before we retrigger.
				    }
				#endif
			}
		}
		else if( imagerpwrdntime == 0 )		//if "SDRTIM" == 1, scanning off immediately (note "0" means on forever, > 1 is using a timer, in mSec)
		{
			Set_nLightsState( false );
			//ewr04791 Codegate
			Set_nAimerState(true);         //aimer on
			//ken@2012.03.09 for code gate
			if(HardwareToken.IAmVoyager1400())
			{
				((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_flash.turnoff((ScanAimerIllumMode_t)GET_SETTING(ScanAimerMode), true/*, ((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_LedCtrlMode*/);
				((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_aimer.turnon((ScanAimerIllumMode_t)GET_SETTING(ScanAimerMode));
			}
		}
		else			//Active scan driver state, cuttently running with lights on (if enabled), leave SD running for sdrtim time out with the lights off
		{
			FinishedScanning = FINISHED;	//Finished is scan driver running but lights have been turned off,
			Set_nLightsState( false );
			Set_nAimerState( false );
		}											//Finished and Off has both items off

		mbUseBoostMode = false;

		ExpVals[1] = mStartingExposureManualTrigger;
		ExpVals[3] = GET_SETTING(StartingGain);
		ExpVals[5] = GET_SETTING(TargetWhite);

		if (GET_CONFIG_SETTING(iOpticsPopulation)==TWO_D && GET_SETTING(DecodeMode) == 2)
		{
				ExpVals[3] = GET_SETTING(AldStartingGain);
				ExpVals[5] = GET_SETTING(AldTargetWhite);
		}

		if(( !mIllOnDuringDecode) &&(!GET_SETTING(EnConnecttoExt)))      //lights are off
		{
			if( ExpVals[1] < EXPEMX_JADE_MANUAL )
			{
				ExpVals[1] = EXPEMX_JADE_MANUAL;     //12ms exposure for seeing in the dark
			}
		}

		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)ExpVals,sizeof(ExpVals),NULL,0,NULL);
	}
}


void CTrigger::PreferredSequenceShotgunModeFinishScanning(void)
{
	if( ( (MANUAL_TRIGGER==mTriggerMode) || (LOW_POWER==mTriggerMode) || (TEMP_MANUAL_TRIG==mTriggerMode) ) )
	{
		if( ( mPreferredMode || mSequenceMode || mShotgunMode )  && (FinishedScanning != FINISHED_AND_OFF) )
		{
			bool PreferredModeRestore = mPreferredMode;    //save settings
			int SequenceModeRestore = mSequenceMode;
			int ShotgunModeRestore = mShotgunMode;

			mPreferredMode = false;                 //disable settings to fake out FinishScanning()
			mSequenceMode = 0;
			mShotgunMode = 0;

			if( imagerpwrdntime == 1 )          //if "SDRTIM" == 1, scanning off immediately (note "0" means on forever, > 1 is using a timer, in mSec)
			{
				FinishedScanning = ACTIVE;
			}

			FinishScanning();

			mPreferredMode = PreferredModeRestore;          //restore settings
			mSequenceMode = SequenceModeRestore;
			mShotgunMode = ShotgunModeRestore;
		}
	}
}

//#define HI2D_TEST 1//ken @2011.10.18 for HI2D auto exposure
#ifdef HI2D_T//for HI2D
BOOL CTrigger::GetNextScan(DWORD nOpenCntxt,PBYTE *ppScanBuffer)
{
//	theStdInterface.printf("GetNextScan\r\n");
	BOOL ret = FALSE;
	if (!bFirstTime) nStartTime = current_time(); else bFirstTime = FALSE;

	if( (MANUAL_TRIGGER==mTriggerMode) || (LOW_POWER==mTriggerMode) || (TEMP_MANUAL_TRIG==mTriggerMode) )     // || (InBatchedMode()&&!BlockDecoderFromExecuting)) // tried to make this more intuitive for batched but never resolved all the side affects!! spd
	{
		validscanpending=false;
	}

	if (validscanpending)
	{
		validscanpending=false;
		*ppScanBuffer = P_SCANBUFFER;
		return TRUE;
	}
	else
	{
//		theStdInterface.printf("Get trigger:%d\r\n",current_time());
		//delay 50 ms
//		MT_Delay(50);
//		theStdInterface.printf("Before Get 1 Image:%d\r\n",current_time());
		//ret = GetImage();
		if (GET_CONFIG_SETTING(iOpticsPopulation)==TWO_D)
		{
			ret = GetBestNewImage(ppScanBuffer);//for HI2D
		}
//		theStdInterface.printf("Get 1 Image:%d\r\n",current_time());
		//if (ret)
		return ret;
	}
}

#else
BOOL CTrigger::GetNextScan(DWORD nOpenCntxt,PBYTE *ppScanBuffer)
{
	DWORD Stuff[] = {HHPSD_IMAGE_STAT_EXPOSURE,0,HHPSD_IMAGE_STAT_GAIN,0,HHPSD_IMAGE_STAT_ILLUM_VALUE,0};
	DWORD ExpVals[] = {ES_MAX_EXP,0,ES_MAX_GAIN,0};
	DWORD nSize;
	BOOL ret;

	if (!bFirstTime) nStartTime = current_time(); else bFirstTime = FALSE;

	if( (MANUAL_TRIGGER==mTriggerMode) || (LOW_POWER==mTriggerMode) || (TEMP_MANUAL_TRIG==mTriggerMode) )     // || (InBatchedMode()&&!BlockDecoderFromExecuting)) // tried to make this more intuitive for batched but never resolved all the side affects!! spd
	{
		validscanpending=false;
	}

	if (validscanpending)
	{
		validscanpending=false;
		*ppScanBuffer = P_SCANBUFFER;
		return TRUE;
	}
	else
	{
		if (GET_CONFIG_SETTING(iOpticsPopulation)==TWO_D)
		{
			/*Here is what this does.  We leave scandriver on now when untriggered!!!  When we trigger we turn
			lights on.  First image will be dark but might read depending on ambient conditions.  Second image might
			be half dark depending on when lights were turned on. Third image will have had lights on for the hole exposure of
			the frame..
			So we check images to see if they meet some mimumum illumination level before returning them to the world.
			We did this because the decoder can sometimes take a long time ( 300ms) to give up trying to decode it.
			This is a quick way to qualify the images.  We only will throw away a max number of 2 images here. We don't
			want an endless loop here and in the situation I talk about above 2 should be the max you would ever throw
			away. */

#ifdef GEN6_DEVELOPMENT
		ret = GetBestNewImage(ppScanBuffer);
#else //!GEN6
			ret = GetImage();		// Former (ret = SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_NEW_SCAN,(UCHAR *)ppScanBuffer,0,NULL,0,NULL)))    //Heat Mitigation
#endif //END ifdef GEN6_DEVELOPMENT

			if(TRUE == ret)
			{
				/*Next get me the characteristics of this image */
				SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_IMAGE_ATTRIBUTES,(UCHAR *)ppScanBuffer,0,(PBYTE) Stuff,sizeof(Stuff)/sizeof(DWORD),&nSize);
				SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_EXPOSURE_SETTINGS,(PBYTE)ExpVals,sizeof(ExpVals),NULL,0,NULL);

				/*If first image has a value of >60 except it.Ambient is pretty good othwise go get the second image
				and make sure that value is above 90. Numbers came about with some testing. Or we have more than two images */
				if( (mTriggerMode != AUTOMATIC_TRIGGER) && (mTriggerMode != STREAMING_PRESENTATION_TRIGGER) &&
					(mTriggerMode != PRESENTATION_TRIGGER) || (!bWeAreLightBarEngine && (mTriggerMode == PRESENTATION_TRIGGER)) )
				{
					if (GET_SETTING(DecodeMode) != 2)
					{
						bool lowIllum = Stuff[5] < GET_SETTING(TargetWhite);
						bool expMaxed = Stuff[1] == ExpVals[1];
						bool gainMaxed = Stuff[3] >= ExpVals[3];

						// On the MT9V022, the possible gains jump from
						// 5.33 to 8.00.  If the max gain is set to 6, the
						// the scan driver will really max at 5.33 and the
						// exposure report will round that down to 5.
						if (  GET_SETTING( ImageEngineType )
							  == HHPSD_ENGINE_IMAGER_STM_VC602  )
						{
							if (Stuff[3] == 5 && ExpVals[3] == 6)
								gainMaxed = true;
						}

						// If the illumination is too low, but the exposure
						// and gain are maxed out, allow a higher exposure.
 						if (lowIllum && expMaxed && gainMaxed)
						{
							if( (ExpVals[1] < GET_SETTING(MaximumExposure)) )
							{
								ExpVals[1] = GET_SETTING(MaximumExposure);

								if( (!mIllOnDuringDecode)&&(!GET_SETTING(EnConnecttoExt)) )      //lights are off
								{
									if( ExpVals[1] < EXPEMX_JADE_MANUAL )
									{
										ExpVals[1] = EXPEMX_JADE_MANUAL;     //12ms exposure for seeing in the dark
									}
								}

								SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)ExpVals,sizeof(ExpVals),NULL,0,NULL);
							}
							else
							{
								if (ExpVals[3] < GET_SETTING(MaximumGain))
								{
									ExpVals[3] = GET_SETTING(MaximumGain);

									if( (!mIllOnDuringDecode)&&(!GET_SETTING(EnConnecttoExt)) )      //lights are off 
									{
										if( ExpVals[1] < EXPEMX_JADE_MANUAL )
										{
											ExpVals[1] = EXPEMX_JADE_MANUAL;     //12ms exposure for seeing in the dark
										}
									}

									SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)ExpVals,sizeof(ExpVals),NULL,0,NULL);
								}
							}
						}
					}
					else
					{
						if ((Stuff[5] < GET_SETTING(AldTargetWhite)) && (Stuff[1] == ExpVals[1]) && (Stuff[3] >= ExpVals[3]))
						{
							if( (ExpVals[1] < GET_SETTING(MaximumExposure)) )
							{
								ExpVals[1] = GET_SETTING(MaximumExposure);

								if( (!mIllOnDuringDecode)&&(!GET_SETTING(EnConnecttoExt)) )      //lights are off 
								{
									if( ExpVals[1] < EXPEMX_JADE_MANUAL )
									{
										ExpVals[1] = EXPEMX_JADE_MANUAL;     //12ms exposure for seeing in the dark
									}
								}

								SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)ExpVals,sizeof(ExpVals),NULL,0,NULL);
							}
							else
							{
								if (ExpVals[3] < GET_SETTING(AldMaximumGain))
								{
									ExpVals[3] = GET_SETTING(AldMaximumGain);

									if( (!mIllOnDuringDecode)&&(!GET_SETTING(EnConnecttoExt)) )      //lights are off
									{
										if( ExpVals[1] < EXPEMX_JADE_MANUAL )
										{
											ExpVals[1] = EXPEMX_JADE_MANUAL;     //12ms exposure for seeing in the dark
										}
									}

									SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)ExpVals,sizeof(ExpVals),NULL,0,NULL);
								}
							}
						}
					}
				}
			} //end if(TRUE == ret)

	      if(ret==TRUE)
	      {
				if(GET_SETTING(ShowSnappyTime))
				{
					char	temp[ 80 ];
					sprintf ( temp, "Time to get Next Scan = %d ms\r\n",(current_time() - nStartTime)/1000);
					SendStringToInterface( (const char *) temp, strlen( temp ), true );
				}
				if(GET_SETTING(AutoExposureReport))
				{
					/*send out the attributes of the image we are returning. */
					theStdInterface.printf("Exp:%d,Gain:%d,Illum:%d\r\n",Stuff[1],Stuff[3],Stuff[5]);
					//theStdInterface.printf(" ExpMax:%d \r\n",ExpVals[1]);         //lmc debug
				}
			}
			return ret;
		}
		else     //else if not 2D
		{
			ret = GetImage(); //Former SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_NEW_SCAN,(UCHAR *)ppScanBuffer,0,NULL,0,NULL);    //Heat Mitigation

			if (ret==TRUE)
			{
				SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_IMAGE_ATTRIBUTES,(UCHAR *)ppScanBuffer,0,(PBYTE) Stuff,sizeof(Stuff)/sizeof(DWORD),&nSize);
				SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_EXPOSURE_SETTINGS,(PBYTE)ExpVals,sizeof(ExpVals),NULL,0,NULL);

				if(GET_SETTING(ShowSnappyTime))
				{
					theStdInterface.printf("tNS = %d ms\r\n",(current_time() - nStartTime)/1000);
				}
				if(GET_SETTING(AutoExposureReport))
				{
				/*send out the attributes of the image we are returning. */
					theStdInterface.printf("e%d Bv%d i%d\r\n",Stuff[1],Stuff[3],Stuff[5]);
				}
			}
			return ret;
		}
	}
}
#endif

	/*-----------------5/30/2002 8:30AM-----------------
	*  TriggerSerial set serial trigger on
	* --------------------------------------------------*/
	void CTrigger::TriggerSerial(void)
	{
		bSerialTrigger = true;
		SerialTriggerTimeout.Reset();
	}


	/*-----------------5/30/2002 8:32AM-----------------
	* UnTriggerSerial set serial trigger state off
	* --------------------------------------------------*/
	void CTrigger::UnTriggerSerial(void)
	{
		if( !bAllignmentModeEnabled )         //do not untrigger in in 4800dr allignment mode
		{
			bSerialTrigger = false;
			ScanStandState=SCANSTANDUNTRIGGERED;
		}
	}


/*added this function below as compared to IsUnitTriggered Function as the IsUnittrigger fuction will
return FALSE if aimer delay is going even if we are serial triggered or HW triggered and we would go
to sleep which was bad */

bool CTrigger::IsHWorSerialTrig(void)
{
	if( (bSerialTrigger) || pCHWTrigger->IsTriggerPulled() )
		return 	TRUE;
	else
		return 	FALSE;
}


bool CTrigger::IsUnitTriggered( void )
{


	if (GET_SETTING(bIgnoreTrigger)==TRUE)
	{
		return FALSE;
	}


	if( ((GET_SETTING(CblSelNumber)==CBL_USB) || (AreWeUsbTermId() && HardwareToken.IAmOEM())) )
	{
		if(!theStdInterface.DeviceIoControl(IOCTRL_AM_I_ENUMERATED))
		{
			return FALSE;
		}
	}

		switch (mTriggerMode)
			{
			case MANUAL_TRIGGER:
			case LOW_POWER:
			case TEMP_MANUAL_TRIG:
				UpdateTriggerStatus();
	 		case AUTOMATIC_TRIGGER:
			case PRESENTATION_TRIGGER:
			case SCANSTAND_TRIGGER:
			case STREAMING_PRESENTATION_TRIGGER:
				return ( ScanStandState==SCANSTANDTRIGGERED || bCurrentTriggerState );

		// For this case, the host wants to know when the physical trigger is pressed.
			case NOTIFY_HOST:
				case SNAP_AND_SHIP_TRIGGER:
				if(pCHWTrigger->IsTriggerPulled())
					return TRUE ;
				else
					return FALSE ;
			}
		return FALSE;
}



//#define MINIMUM_DIFFERENCE		RAM_ACTUAL(PresentationMinimumDiff)
#define MINIMUM_DIFFERENCE GET_SETTING(AutoTriggergreyband) // what is the minimum difference we'll allow between the max and min curves so
										  // that noise will not be allowed to effect maximums and minimums
#define GET_PIXEL_VAL(x,y) (pImg[x])
#define PIXEL_ADDRESS(x,y) &(pImg[x])
#define PIXEL_SKIP 0 /* 0 means it gets every pixel, 1 means every other, 2 means every 4th, 3: every 8th, and so on (powers of 2)*/



int CTrigger::CountEdges(int  P1x, int P2x,unsigned char * pImg)
{
	int nSignx,nDeltax,nIncx,nSteps,i,*pPix = PMpix,*pMax = PMmax,*pMin = PMmin;
	int  nTop,nBot,nEdges=0,MinimumDifference = MINIMUM_DIFFERENCE;
	bool bHigh;
	unsigned char  *pRefImg;

//	RESET_WATCHDOG_TIMER();
	/* Make sure points are within image, if not, we put them within the image */
	if (P1x >= 3645) P1x = 3645-1;
	if (P2x >= 3645) P2x = 3645-1;
	/* determine the direction of the scan: positive or negative, X direction Y direction, etc.*/
	if( P1x > P2x ) {nSignx = -1; nDeltax = P1x - P2x;}
	else {nSignx = 1; nDeltax = P2x - P1x;}

	/*-----SAMPLE THE IMAGE------
	 * In the image sampling step here below, not only do we sample the pixels but we also do the first step
	 *   of mapping the maximums and minimums
	 *---------------------------*/

		pRefImg = PIXEL_ADDRESS(P1x,P1y);
		*pPix = nTop = nBot = *pMax = *pMin = (int)*pRefImg;
		nIncx = nSignx<<PIXEL_SKIP;
		nSteps = nDeltax>>PIXEL_SKIP;
		for (i=nSteps-1;i>=0;i--)
		{
			pPix++; pMin++; pMax++; pRefImg+=nIncx;
			*pPix = (int)*pRefImg;
			if ((i&0x3)==0x3){ ((--nTop)<(nBot + MinimumDifference + 1))?(nBot = nTop-MinimumDifference):(nBot++); }
			if (*pPix > nTop)nTop = *pMax = *pPix; else *pMax = nTop;
			if (*pPix < nBot)nBot = *pMin = *pPix; else *pMin = nBot;
		}
	/* -------COUNT THE EDGES-------
	 * We do the second and final step of mapping the maximums and minimums, and immediately use them to determine if
	 * we've seen an edge or not.  We detect an edge by looking at where the pixels fall between the maximum and minimum
	 * line.  If we see that a pixel falls in the upper 1/8 of the space between the maximum and minimum line, we determine that
	 * as being a "high", and if a pixel falls in the lower 1/8 of the space between the maximum and minimum line, we determine that
	 * to be a "low".  And if we see the pixel values go from a "high" to a "low" or visa versa, that's counted as an "edge".
	 * ---------------------------*/
	*pMax -= (nTop-nBot)>>3;
	*pMin += (nTop-nBot)>>3;
	if (*pPix > ((*pMax+*pMin)>>1)) bHigh=TRUE; else bHigh = FALSE;
	for (i=nSteps-1;i>=0;i--)
	{
		pPix--;pMin--;pMax--;
		if ((i&0x3)==0x3){ ((--nTop)<(nBot + MinimumDifference + 1))?(nBot = nTop-MinimumDifference):(nBot++); }
		if (*pMax > nTop)nTop = *pMax; else *pMax = nTop;
		if (*pMin < nBot)nBot = *pMin; else *pMin = nBot;
		*pMax -= (nTop-nBot)>>3;
		*pMin += (nTop-nBot)>>3;
		if ((*pPix > *pMax)&&(!bHigh)) {nEdges++; bHigh = TRUE;}
		else if ((*pPix < *pMin)&&(bHigh)) {nEdges++; bHigh = FALSE;}
	}
	return nEdges;
}



#define	EDGEDEPTH	20
#define	EDGE_HISTERESIS	6

bool ShallIUntriggerBasedOnEdgeHistory(int iCurrentEdgeCount, bool bAddThisToAverage, bool bForceUpdateNextTime)
{
	static int iCount=1;									//initialize count to this first value
	static int iEdgeAverage=iCurrentEdgeCount;	//initialize the edge average to this first value
	static bool bResetAverage=FALSE;							//if set, this means drastic change expected.  reset average.
	int iEdgeMin = iEdgeAverage-EDGE_HISTERESIS;	//Get the MIN we will accept for the average
	int iEdgeMax = iEdgeAverage+EDGE_HISTERESIS; //Get the MAX we will accept for the average
	if(iEdgeMin<0)
		iEdgeMin=0;
	if(bResetAverage==TRUE) //Is this a reset request from previous cycle?
	{
		iEdgeAverage = 0;
		iCount=EDGEDEPTH;
		bResetAverage=FALSE;
		return TRUE;
	}
	if(bForceUpdateNextTime)
	{
		bResetAverage=TRUE;
		return FALSE;
	}
	if((iCurrentEdgeCount<(iEdgeMin)) || (iCurrentEdgeCount>(iEdgeMax)))
	{
		//theStdInterface.printf("{%d %d %d %d}",iEdgeMin,iEdgeAverage,iCurrentEdgeCount,iEdgeMax);
		if(bAddThisToAverage) //Am I supposed to be adjusting the average?
		{
			iEdgeAverage = iCurrentEdgeCount;
			iCount=1;
		}
		return FALSE;
	}
	if(bAddThisToAverage==FALSE) // If I'm not doing averaging, just return.
		return TRUE;
	if(iCount==1)
		iEdgeAverage=iCurrentEdgeCount;	//first edge in the accumulation of data.
	else
		iEdgeAverage=((iEdgeAverage*iCount+iCurrentEdgeCount)/(iCount+1)); // else throw the new value into the mix.
	if(iCount<EDGEDEPTH)
		iCount+=1;
	if(iCount==EDGEDEPTH)
		return TRUE;
	else
		return FALSE;
}

//GetLastExposure() gets last exposure and modifies it for use as timer setting for Heat Mitigation cool down
DWORD CTrigger::GetLastExposure(void)                     														//added for Heat Mitigation
{
	DWORD nSize, Result, Stuff[] = {HHPSD_IMAGE_STAT_EXPOSURE,0,HHPSD_IMAGE_STAT_GAIN,0,HHPSD_IMAGE_STAT_ILLUM_VALUE,0};
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_IMAGE_ATTRIBUTES,(UCHAR *)&P_SCANBUFFER,0,(PBYTE) Stuff,sizeof(Stuff)/sizeof(DWORD),&nSize);

	//printf ("stuff1 %d\r\n", Stuff[1]);
	Result = ((Stuff[1] * 127) / 1000);  // this should be exposure time in ~ ms
	Result *= 3;  // adjust for 3 frame concept per image acquire
	if (Result <= 50)
		return 50;
	else
		return Result;
}

//Method: WaitForHeatMitigationCoolDown() uses exposure to set cool down time before we can grab another image
//Pass parameter: Pass value to allow to enter low power, such as the menu item : GET_SETTING(OpticalTriggerLowPower), defaulted to zero.
//A 'one' on allow low power causes a doze low, a 'two' allows manage system power to select power mode. Zero does not allow low power.
//The multiplier allows extra cool down time for scan stand. Defaulted to 1.
void CTrigger::WaitForHeatMitigationCoolDown( unsigned int uiAllowLowPower, unsigned int uiMultiplier )         														//added for Heat Mitigation
{
	if(!bAllowHeatMitigation)
		return;
	//*********  Heat Mitigation!! ******************
	if (GET_CONFIG_SETTING(iOpticsPopulation)==TWO_D)
	{
  		if (AllowHeatMitigationCoolDown)
		{
      		if (!InBatchedMode())
      			SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_STOP_SCANNING,NULL,0,NULL,0,NULL);

			unsigned int lastExp = (unsigned int)GetLastExposure();
			lastExp = lastExp * uiMultiplier;
			HeatMitigationCoolDownTime = BaselineClock + lastExp;

			while (HeatMitigationCoolDownTime > BaselineClock && HeatMitigationCoolDownTime - BaselineClock <= lastExp)
			{
//				RESET_WATCHDOG_TIMER();

	         	if( uiAllowLowPower == 1 )
					EnterPowerState(DOZE_LOW);
				else if( uiAllowLowPower == 2)
					ManageSystemPower();
    	  	}

      		AllowHeatMitigationCoolDown = false;

      		if (!InBatchedMode())
      			SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_START_SCANNING,NULL,0,NULL,0,NULL);

			if( uiAllowLowPower == 1 )
				EnterPowerState(DOZE_LOW);
		}
	}
}
//SetHeatMitigationScanStand() is used to control heat mitigation in scan stand
//Pass the state you want for lights before and after heat mitigation, along with and enable mitigation parameter
void CTrigger::SetHeatMitigationScanStand(int iLightsBefore, int iAimerBefore, bool bLightsAfter, bool bAimerAfter, bool bAllowMitigation)
{
	if( bAllowMitigation && bAllowHeatMitigation )
	{
		AllowHeatMitigationCoolDown = true;
		//Lets shut the lights off for heat mitigation
		SetLights( (mLightsForcedOn)?true:iLightsBefore, (mLightsForcedOn)?100:-1, (mAimersForcedOn)?true:iAimerBefore, (mAimersForcedOn)?100:-1);

		WaitForHeatMitigationCoolDown( 2, 2 );
		Set_nAimerState( bAimerAfter );                                      //turn aimer on

		if(bLightsAfter)
		{
			Set_nLightsState( mIllOnDuringDecode );              //turn lights on if enabled
		}
	}
}
	//*************  End of Heat Mitigation *************

#if defined(IR_TRIGGER)
#include "IRTrigger.h"
#endif
#include "beep.h"
void CTrigger::UpdateTriggerStatus( void )
{
	int edgecount;
	bool bPreAimerDlyRunning=false;
	DWORD  DataToSend[8], DataToRestore[8], Stuff[4], nSize;
	//char	TempStr[160];                  //debug		lmc

	//---- things are sent in groups of 2s, the first is the tag, and the second is the value----
	DataToSend[0] = ES_FIXED_EXP;
	DataToSend[1] = GET_SETTING( MaximumExposureAuto );
	DataToSend[2] = ES_FIXED_GAIN;               //settings for scan stand low power
	DataToSend[3] = GET_SETTING( MaximumGainAuto );
	DataToSend[4] = ES_CONFORM_IMAGE;
	DataToSend[5] = TRUE;
	DataToSend[6] = ES_CONFORM_TRIES;
	DataToSend[7] = 4;

   	DataToRestore[0] = ES_FIXED_EXP;
	DataToRestore[1] = GET_SETTING( MaximumExposureAuto );
	DataToRestore[2] = ES_FIXED_GAIN;    			//settings to restore for normal scan stand
	DataToRestore[3] = GET_SETTING( MaximumGainAuto );
	DataToRestore[4] = ES_CONFORM_IMAGE;
	DataToRestore[5] = TRUE;
	DataToRestore[6] = ES_CONFORM_TRIES;
	DataToRestore[7] = 5;

	Stuff[0] = HHPSD_IMAGE_STAT_EXPOSURE;
	Stuff[1] = 80;
	Stuff[2] = HHPSD_IMAGE_STAT_GAIN;
	Stuff[3] = 1;
//	RESET_WATCHDOG_TIMER();

	RestoreTriggerModeForUSB();

	if (!MT_WaitMutex(TriggerMutex,1))
		return;
	
#if defined(IR_TRIGGER)
	UpdateIrTriggerConfig();
#endif

	do
	{
		Set_nAimerState( DONT_CHANGE );	// Don't change the aimer state yet

		if( (mTriggerMode != PRESENTATION_TRIGGER) && (!mBlockTempManualOnTriggerChange) )	BlockDecoderFromExecuting = FALSE;
		/*
		Ken@2012.05.09 to support SCNDLY menu command.
		When the scan delay enabled, and trigger is not actived. Make sure the aimer is closed.
		TODO:Now every loop to operate the IO, it need to be modified.
		*/
		if(HardwareToken.IAmVoyager1400())
		{
#if defined(IR_TRIGGER)
			if( AimerPreDelay && (mTriggerMode == MANUAL_TRIGGER) && !pCHWTrigger->IsTriggerPulled() && ( (GET_SETTING(IrTriggerEnable) == IR_TRG_OFF) || !IsIrTriggered() ))
#else
			if( AimerPreDelay && (mTriggerMode == MANUAL_TRIGGER) && !pCHWTrigger->IsTriggerPulled())
#endif
			{
				((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_aimer.turnoff((ScanAimerIllumMode_t)GET_SETTING(ScanAimerMode));
			}
		}

		if (ProcessChangeToFromTempManualTrigger())
		{
			continue;
		}

		if( NeedToHandleScanStandSwitch() )
		{
			//ewr04791 Codegate
			FillDecoderWindowCenteringMenu();
			continue;
		}

#ifdef FLASH_LEDS       //if ff3 gen6 board
		static bool bJustChangedCurrentTriggerState = true;		// 'true' indicate scanner was not triggerred.
#endif

		switch (mTriggerMode)
		{
			case TEMP_MANUAL_TRIG:
			case MANUAL_TRIGGER:
			case LOW_POWER:
				if (mEmiTrigger && mEmiTimeout.HasExpired())
				{
					mEmiTimeout.Reset();
					EmiTriggerState=!EmiTriggerState;
				}

				//if we are not triggered while in manual trigger mode
				if (!bSerialTrigger && (!pCHWTrigger->IsTriggerPulled()) && !(mEmiTrigger && EmiTriggerState)
					&& (IOCRET_YES == theStdInterface.DeviceIoControl(IOCTRL_SHALL_I_TRIGGER)?!theStdInterface.IsAllSent():true)
#if defined(IR_TRIGGER)
					&& ( (GET_SETTING(IrTriggerEnable) == IR_TRG_OFF) || !IsIrTriggered() )
#endif
					)  // not triggered
				{
					// notify drivers that we just untriggered, but only send it once. (used by HHLC)
					if(m_IoControlState!=IOC_UNTRIGGERED  )
					{
						m_IoControlState=IOC_UNTRIGGERED;
						theStdInterface.DeviceIoControl(IOCTRL_UNTRIGGERED);
					}

					if(UsageStatistics.StartTriggerTime!=-1)
					{
						UsageStatistics.AccumTrigTime+=current_time()-UsageStatistics.StartTriggerTime;
						UsageStatistics.StartTriggerTime=-1;
					}

					//if( !bJustChangedCurrentTriggerState)	   theStdInterface.printf(" Changing State Untrig %d \r\n", bJustChangedCurrentTriggerState);    //lmc debug

					bCurrentTriggerState=FALSE;
					mbCurrentTriggerStateChangePending = false;
					       
#ifdef FLASH_LEDS       //if ff3 gen6 board
					bJustChangedCurrentTriggerState = true;  //set true, so next time we pull trigger we know we are changing to being triggered
#endif
					USBRecoveryTimeout.SetExpiration(USB_RECOVERY_TIME);
					SetSerialTriggerTimeout();
					pAimerPreDelayTimeout.SetExpiration(AimerPreDelay);
					Set_nLightsState( FALSE );

					if( (mScanAimerIdleTimeout !=0) && (!pmScanAimerIdleTimeout.HasExpired()) )
					{
						if( FinishedScanning != FINISHED_AND_OFF )
						{
							Set_nAimerState(2);                             //aimer on while imaging during non exposure time
						}
						else
						{
							Set_nAimerState(3);                             //aimer on continuous while not imaging
						}
					}
					//ewr04791 Codegate
					else if(!imagerpwrdntime)
					{
						Set_nAimerState(true); 		//aimer on
						if(HardwareToken.IAmVoyager1400())
							((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_aimer.turnon((ScanAimerIllumMode_t)GET_SETTING(ScanAimerMode));
					}
					else
					{
						Set_nAimerState(0);                                //aimer off
					}

					if( !imagerpwrdntime && (FinishedScanning == FINISHED_AND_OFF) )
					{
						//ewr04791 Codegate
						Set_nAimerState(true); 		//aimer on
						StartImaging(); // Former SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_START_SCANNING,NULL,0,NULL,0,NULL);  //Heat mitigation
						FinishedScanning =FINISHED;
						mbUseBoostMode = false;
					}
					else if( ( ((imagerpwrdntime>1) && pImagerIdleTimeout.HasExpired()) || mPreferredMode || mSequenceMode || mShotgunMode ) && (FinishedScanning == FINISHED) )
					{
						StopImaging(); // Former SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_STOP_SCANNING,NULL,0,NULL,0,NULL);     //Heat mitigation
						FinishedScanning = FINISHED_AND_OFF;  /*prevents us from call this stop scanning over and over and over */

						if( mPreferredMode || mSequenceMode || mShotgunMode )
						{
							Set_nLightsState( false );
							Set_nAimerState( false );
						}

						if( (mStartingExposureManualTrigger <= 4) && (lightsduty > 100) && mbUseBoostMode )       //go to high boost if starting exposure is <= 4, so we start next frame in high
						{
							#ifdef FLASH_LEDS       //if ff3 gen6 board
								if(!(HardwareToken.IAmOEM()))
								{
									SetILL_FlashMode(1);		//early charge up for next frame
								}
								else if(HardwareToken.IAmOEMJade())
								{	
									MT_Delay(16);           //if we are OEM with psoc, we need 16 ms to recharge boost before we retrigger.
								}	
							#endif
						}

						mbUseBoostMode = false;
					}

					if( mAutoaimertimeout && NewImageAvailable() || BlockDecoderFromExecuting )      //was SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_NEW_IMAGE_AVAILABLE,NULL,0,NULL,0,NULL), now NewImageAvailable(), for Heat Mitigation
					{
						if( !GoNumbAfterDecode )         //added for Heat Mitigation
						{
	                     	if( GetImage() == 0 )    //was (SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_NEW_SCAN,(UCHAR *)&P_SCANBUFFER,0,NULL,0,NULL)==0), for heat mitigation
								break; // scan driver error dont change anything and exit

							validscanpending=true;
						}

						Set_nAimerState( false );
						OpticalTrigger();         //wake up the aimer if sdrtim is running and auto aimer timeout is greater than zero

						if( nAimerState )	pmScanAimerIdleTimeout.SetExpiration(mScanAimerIdleTimeout);
					}
				}
				else     //else if triggered
				{	//triggered
					//if( bJustChangedCurrentTriggerState)	theStdInterface.printf(" Changing State trig %d \r\n", bJustChangedCurrentTriggerState);    //lmc debug

					// notify drivers that we just triggered, but only send it once. (used by HHLC)
					if(m_IoControlState!=IOC_TRIGGERED)
					{
						m_IoControlState=IOC_TRIGGERED;
						theStdInterface.DeviceIoControl(IOCTRL_TRIGGERED);
					}

					if(UsageStatistics.StartTriggerTime==-1)
					{
					 	UsageStatistics.StartTriggerTime=current_time();

						if (pCHWTrigger->IsTriggerPulled())
						{
							UsageStatistics.HardwareTriggers++;
						}

						if( bSerialTrigger && !bAllignmentModeEnabled )  UsageStatistics.SerialTriggers++;
					}

					pImagerIdleTimeout.SetExpiration(imagerpwrdntime);
					pmScanAimerIdleTimeout.SetExpiration(mScanAimerIdleTimeout);

					if (mSerialTriggerTimeout!=0 && SerialTriggerTimeout.HasExpired())
					{
						if( bCurrentTriggerState==true )  UsageStatistics.TriggerTimeouts++;
						
						bCurrentTriggerState = false;               //we are no longer triggered
						mbCurrentTriggerStateChangePending = false;
						//Serial D/E mode.
						if(mSerialDisableEnable)
						{
      						theScriptInterpreter.DisableOPOS();
						}
					}
					else
					{
						bCurrentTriggerState = true;                 //we are triggered
					}

					if (USBRecoveryTimeout.HasExpired())
					{
						USBRecoveryTimeout.SetExpiration(USB_RECOVERY_TIME);
#if !defined(DISABLE_AUTO_CBL)
						if( (GET_SETTING(CblSelNumber)==CBL_USB) && (!theStdInterface.DeviceIoControl(IOCTRL_AM_I_ENUMERATED)) )
						{
							HSTRING		MenuCommandText( 15, true );

							MenuCommandText.Copy( "060802124." );

							ReaderConfiguration->Menu( &MenuCommandText );
							ReconfigureInterface();
						}
#else
						if( (GET_SETTING(InterfaceType) == 1) && (!theStdInterface.DeviceIoControl(IOCTRL_AM_I_ENUMERATED)) )
						{
							HSTRING		MenuCommandText( 26, true );

							MenuCommandText.Copy( "0606000;0605000;060802124." );

							ReaderConfiguration->Menu( &MenuCommandText );
							ReconfigureInterface();
						}
#endif
					}

					if( mSerialTriggerTimeout && SerialTriggerTimeout.HasExpired() )
					{
						mbCurrentTriggerStateChangePending = false;
						Set_nAimerState( FALSE );
						
						if( InBatchedMode() || HardwareToken.IAmOEM() ) 	   			//Heat Mitigation
						{                              //if oem, we need to turn off lights after serial trigger time out, if entering temp manual trigger, especially if using TRGMMF_REREAD_FOV in pres mode 
							Set_nLightsState( FALSE );
						}
						//Serial D/E mode.
						if(mSerialDisableEnable)
						{
      						theScriptInterpreter.DisableOPOS();
						}
					}
					else if( FinishedScanning != ACTIVE )    // FIXME? done to resolve issue caused by FinishedScanning not ACTIVE at the start of Temp, so in batched first image and idle were with ONLY illum on     //Heat Mitigation
					{

#ifdef FLASH_LEDS       //if ff3 gen6 board
						if( (bCurrentTriggerState && bJustChangedCurrentTriggerState) )		//if just triggered, set aimer mode
						{                                              //we need to keep aimer on before scan driver enabled, need for new scan driver
							Set_nAimerState( mScanAimerMode );
							mbCurrentTriggerStateChangePending = true;  //this will keep aimer on until scan driver starts (active mode), does not set lights mode idle state
						}
						else if( !mbCurrentTriggerStateChangePending )			//if we are done witing for active mode
						{
							Set_nAimerState( FALSE );
						}
#else
						Set_nAimerState( FALSE );
#endif
					}
					else		//if FinishedScanning is ACTIVE
					{
						mbCurrentTriggerStateChangePending = false;
						Set_nAimerState( mScanAimerMode );
					}

					bSerialTrigger &= bCurrentTriggerState;      //clear serial trigger state if we are no longer triggered

					//if SCNDLY is set, start aimer when triggered and wait time set before allowing lights to come on
					if( AimerPreDelay && !pAimerPreDelayTimeout.HasExpired() && bCurrentTriggerState )
					{
						/*ken@2012.05.09 to support the SCNDLY menu command.
						   Open the aimer here and change the trigger state to false.
						*/
						if(HardwareToken.IAmVoyager1400())
						{
							bCurrentTriggerState = FALSE;   //indicate that we are not triggered till delay ends
							((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_aimer.turnon((ScanAimerIllumMode_t)GET_SETTING(ScanAimerMode));
						}
						else
						{
							bPreAimerDlyRunning = TRUE;    //run continuous aimer
							bCurrentTriggerState = FALSE;   //indicate that we are not triggered till delay ends
							mbCurrentTriggerStateChangePending = false;		//indicate that trigger state is not in the process of changing, so we set Idle lights mode
							Set_nAimerState( 3 );        //set aimer on continuous
						}
					}
					else
					{
						bPreAimerDlyRunning = FALSE;	//run imager synced aimer
					}

#ifdef FLASH_LEDS       //if ff3 gen6 board
					//if scanning active or if just became triggered
					if( (bCurrentTriggerState && FinishedScanning==ACTIVE) )
					{	
					 	Set_nLightsState( mIllOnDuringDecode );
					}
					else if( (FinishedScanning != ACTIVE) && (bCurrentTriggerState && bJustChangedCurrentTriggerState && mbCurrentTriggerStateChangePending) )      //pre scanning light state set
					{
						Set_nLightsState( mIllOnDuringDecode );
						//theStdInterface.printf(" trigState %d FinScan %d JustChangedState %d ChangePending %d \r\n", bCurrentTriggerState, FinishedScanning, bJustChangedCurrentTriggerState, mbCurrentTriggerStateChangePending);       //lmc debug
					}
					bJustChangedCurrentTriggerState = false;
#else
					if( bCurrentTriggerState && FinishedScanning==ACTIVE )
					{
					 	Set_nLightsState( mIllOnDuringDecode );
					}
#endif
			 	}     //end else (triggered)

				if( bCurrentTriggerState )
				{
				 	if( mScanAimerDuration && pAimerDurationTimout.HasExpired() ) Set_nAimerState( FALSE );
				}
				else
				{
					pAimerDurationTimout.SetExpiration(mScanAimerDuration);
				}

				break;
			case AUTOMATIC_TRIGGER:
			case PRESENTATION_TRIGGER:
			case STREAMING_PRESENTATION_TRIGGER:
			{
				PRINT_DEBUG_S(" UpdatePres \r\n"); //lmc debug

				if( NeedToChangeToManual() )                       //are we triggered
				{
					TriggeredTimeout.Reset();
					TriggeredInAutoOrPresMode = true;
					continue;
				}

				USBRecoveryTimeout.SetExpiration(USB_RECOVERY_TIME);

				if (HeatMitigationHasDecodeOutput)		AllowHeatMitigationCoolDown = false;    							//added for Heat Mitigation

				unsigned int uiOptTrigPowMode = 0;       												//disable low power in heat mitigation
				if( GET_SETTING(OpticalTriggerLowPower) )		uiOptTrigPowMode = 1;		//allow low power mode if so menued

				WaitForHeatMitigationCoolDown( uiOptTrigPowMode, 1 );		//enabled by variable AllowHeatMitigationCoolDown		//added for Heat Mitigation

				if( g_dmaNotDoneBeforeSof )
				{
					FinishedScanning = FINISHED_AND_OFF;
					StopImaging(); // Former SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_STOP_SCANNING,NULL,0,NULL,0,NULL);    //Heat Mitigation
					g_dmaNotDoneBeforeSof = false; //reset flag while off
					StartImaging(); // Former SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_START_SCANNING,NULL,0,NULL,0,NULL);    //Heat Mitigation
					FinishedScanning = ACTIVE;
				}

				if( NewImageAvailable() || BlockDecoderFromExecuting )   //was ( SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_NEW_IMAGE_AVAILABLE,NULL,0,NULL,0,NULL) || BlockDecoderFromExecuting), changed to NewImageAvailable()    //Heat Mitigation
				{
					if (!GoNumbAfterDecode)        //added for Heat Mitigation
					{
						if( GetImage() == 0 )    //used to be (SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_NEW_SCAN,(UCHAR *)&P_SCANBUFFER,0,NULL,0,NULL)==0) break;    //Heat Mitigation change
							break;         // scan driver error dont change anything and exit

						validscanpending=true;
					}

					if(GET_SETTING(AutoExposureReport))
					{
	 					DWORD AutoExpStuff[] = {HHPSD_IMAGE_STAT_EXPOSURE,0,HHPSD_IMAGE_STAT_GAIN,0,HHPSD_IMAGE_STAT_ILLUM_VALUE,0};
						SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_IMAGE_ATTRIBUTES,(UCHAR *)&P_SCANBUFFER,0,(PBYTE) AutoExpStuff,sizeof(AutoExpStuff)/sizeof(DWORD),&nSize);
						/*send out the attributes of the image we are returning. */
						theStdInterface.printf("Exp:%d,Gain:%d,Illum:%d\r\n",AutoExpStuff[1],AutoExpStuff[3],AutoExpStuff[5]);
					}

					//validscanpending=true;		//Moved to above, should be set if we get a new image
				}
				else if( P_SCANBUFFER == NULL )
					break;

				if (GET_CONFIG_SETTING(iOpticsPopulation)==TWO_D)
				{
					if( (mTriggerMode == STREAMING_PRESENTATION_TRIGGER) && (mPresentationScanDriverCellPhone) )  //if streaming pres cell phone mode
					{
						if( mCellScanAimerMode == 0 )
						{
							Set_nAimerState( FALSE );	//Turn the aimer off, assume we're idled if in PRES., aimer is always on or off in cell, so if menued off, leave it off
						}
					}
					else
					{
						Set_nAimerState( FALSE );	//Turn the aimer off, assume we're idled if in PRES. OpticalTrigger will turn it
					}

					if (mTriggerMode != AUTOMATIC_TRIGGER)
						OpticalTrigger();
					else if( ( mCurrentScanAimerMode !=0 ) && ( pTrigger->HasSkipDecodeTimeoutExpired() ) )
						Set_nAimerState( TRUE );			//In AutoTrigger so go see if we want aimers to be on.

					bCurrentTriggerState = TRUE;
					if ( (mTriggerMode == STREAMING_PRESENTATION_TRIGGER) )
						Set_nLightsState( TRUE );
					else
						Set_nLightsState( FALSE );

					if( pTrigger->HasSkipDecodeTimeoutExpired() )		Set_nLightsState( mIllOnDuringDecode );

				}
				else				//1d auto trigger
				{
					edgecount=CountEdges(0,3645,(unsigned char *)P_SCANBUFFER);
					Set_nAimerState( FALSE );

					if( bCurrentTriggerState )  // see if we need to untrigger
					{
						if((ShallIUntriggerBasedOnEdgeHistory(edgecount,TRUE,FALSE)))
						{//Go do test of edges with averaging
							ShallIUntriggerBasedOnEdgeHistory(0,FALSE,TRUE);
							bCurrentTriggerState = FALSE ;
						}
					}
					else		// see if we need to trigger
					{
						if( (AUTOMATIC_TRIGGER==mTriggerMode)||(STREAMING_PRESENTATION_TRIGGER==mTriggerMode) )
						{
							Set_nLightsState( FALSE );
							/* If trigger is pulled and its dark turn ScanLEDs on */
							if( pTrigger->HasSkipDecodeTimeoutExpired() )		Set_nLightsState( mIllOnDuringDecode );
						}
						else if (PresentationModeTimeout.HasExpired())
						{
							/* If there are no edges and the time delay has expired turn ScanLEDs off */
							Set_nLightsState( FALSE );
						}

						if (ShallIUntriggerBasedOnEdgeHistory(edgecount,FALSE,FALSE)==FALSE)
						{
							bCurrentTriggerState = TRUE;
							Set_nLightsState( FALSE );
							PresentationModeTimeout.Reset();
							if( HasSkipDecodeTimeoutExpired() )		Set_nLightsState( mIllOnDuringDecode );
						}
					}
				}
				break;
			}
			case SCANSTAND_TRIGGER:
			{
				static int iDecodedPassCount = 0;
				if (NeedToChangeToManual())
				{
					TriggeredTimeout.Reset();
					TriggeredInAutoOrPresMode = true;
					continue;
				}

				// No new imag available, dont change anything and exit (note: always returns true if in batch mode)
				if( !NewImageAvailable() )  break;			 //used to be (!SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_NEW_IMAGE_AVAILABLE,NULL,0,NULL,0,NULL)) break;       //Heat Mitigation

				if( (iDecodedPassCount < 1) || (iDecodedPassCount > (SCAN_STAND_HEAT_DECODED_PASS_COUNT-1)) ) //single frame capture takes a lot of time compared to streaming, thus slows down data output, and is why this is done
				{
					// scan driver error, dont change anything and exit
					if( GetImage() == 0 )  break;	 								//used to be (SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_NEW_SCAN,(UCHAR *)&P_SCANBUFFER,0,NULL,0,NULL)==0) break;      //Heat Mitigation
					validscanpending = true;                //store old image if you have a new one in GetNextScan
				}

				switch (ScanStandState)
				{
					case WAITFORSCANSTAND:				// call decoder and look for scan stand symbol
					{
						bUseScanStandSymbLowPow = FALSE;
						Set_nAimerState( FALSE );                         //turn off aimer
						ClearDecoderWorkspace();		/*in this test mode we want to clear the decoder on each attempt */                                                    //turn aimer off

						if( (iDecodedPassCount < 1) || (iDecodedPassCount > (SCAN_STAND_HEAT_DECODED_PASS_COUNT-1)) )	//single frame capture takes a lot of time compared to streaming, thus slows down data output, and is why this is done
						{
							bCurrentTriggerState = TRUE;                       //set trigger state to true before we call decoder
							DecodeForMenuing(P_SCANBUFFER);                                     //call decoder
							bCurrentTriggerState = FALSE;      			                               //we are untriggered
						}
						else
						{
							pSSTimeout.SetExpiration(SCANSTANDLPTO);                      //reset timers if we just decoded and heat mitigation, until iDecodedPassCount elapses
							pSSHMTimeout.SetExpiration(SCAN_STAND_HEAT_TIMEOUT);
						}
						if( WasLastSymbolScanStand() )		//if we decoded the scan stand symbol, now wait till we do not
						{
							pSSTimeout.SetExpiration(SCANSTANDLPTO);
							pSSHMTimeout.SetExpiration(SCAN_STAND_HEAT_TIMEOUT);
							ScanStandState=GOTSCANSTAND;
							bUseScanStandSymbLowPow = TRUE;
						}
						else	if (pSSTimeout.HasExpired()) // timed out looking for symbol, enter low power mode
						{
							iGetStableImages = 0;
							ssMaxExpTimesGain = 0;
							bGetScanStandExp = TRUE;
							ScanStandState=SCANSTANDLOWPOWERSTART;
						}
						else if( InBatchedMode() )	 			//did we find a barcode?            //added for Heat Mitigation
						{
							//sprintf(TempStr, "W DC %d OldDC %d Count %d;;;",NumberOfDecodes,OldDecodeCount,iDecodedPassCount);      //debug lmc
							//xputstring(TempStr);                               //debug lmc

							if( (NumberOfDecodes != OldDecodeCount) )	//NumberOfDecodes has been incremented in decodeiface, so we have decoded
							{
								bHeatMitigationHasDecoded = true;	//got a decode, so enable heat mitigation for the next pass through (do not mitigate this pass)
								OldDecodeCount = NumberOfDecodes;                   //update our decode counter
								iDecodedPassCount = 0;
							}

							if( bHeatMitigationHasDecoded && (iDecodedPassCount < SCAN_STAND_HEAT_DECODED_PASS_COUNT) )
							{
								iDecodedPassCount++;
							}
							else if( !bHeatMitigationHasDecoded )
							{
								iDecodedPassCount = 0;
							}

							SetHeatMitigationScanStand(0, 0, true, false, (iDecodedPassCount==SCAN_STAND_HEAT_DECODED_PASS_COUNT) || pSSHMTimeout.HasExpired());
						}
						else
						{
							iDecodedPassCount = 0;
							bHeatMitigationHasDecoded = false;
						}
						break;
					}

					case GOTSCANSTAND:		// saw the SS symbol, lower lights as much as we can if we still see SS symbol, else trigger
					{
						bHeatMitigationHasDecoded = false;
						Set_nAimerState( FALSE );                									 									//turn aimer off
						bCurrentTriggerState = FALSE;                                                            //we are untriggered

						int ssvalue = CheckForScanStand(P_SCANBUFFER, yChk);    //get large black/large white element difference to indicate if it is a scan stand symbol
						if (ssvalue <SSTANDTRIGGERTHRESHOLD)	// we no longer see the scan stand symbol, so we are triggered and trying to read barcode
						{
							Set_AllDutyCycle( lightsduty );							//set LED duty cycle from menu value
							pSSTimeout.SetExpiration(SCANSTANDLPTO);
							pSSHMTimeout.SetExpiration(SCAN_STAND_HEAT_TIMEOUT);
							ScanStandState=SCANSTANDTRIGGERED;                               //also indicates that we are triggered
							if( mScanAimerMode != 0 )		Set_nAimerState( TRUE );                    //set aimer state from menu value (on/off)
							break;
						}
						else
						{
							if (ssvalue > SSTANDLIGHTSTHRESHOLD) //we are sure we see the scan stand symbol, so lower lights slowly, hysterisis is in the difference between SSTANDLIGHTSTHRESHOLD and SSTANDTRIGGERTHRESHOLD
							{
								Set_AllDutyCycle( (AllDutyCycle>1)?AllDutyCycle-1:0 );                                         //was>10)?  :10;
							}
							//if(pSSHMTimeout.HasExpired())                                                //debug lmc
							//{
							//	sprintf(TempStr, "G DC %d OldDC %d;;;",NumberOfDecodes,OldDecodeCount);      //debug lmc
							//	xputstring(TempStr);                                                             //debug lmc
							//}
							OldDecodeCount = NumberOfDecodes;                   //update our decode counter
							SetHeatMitigationScanStand(-1, -1, false, false, pSSHMTimeout.HasExpired());     //added for Heat Mitigation
						}
						if( (pSSTimeout.HasExpired()) && (AllDutyCycle>33) )	// timed out trying to decode, go to "low power", only if LEDS are too high
						{
							iGetStableImages = 0;
							ssMaxExpTimesGain = 0;
							bGetScanStandExp = TRUE;
							ScanStandState=SCANSTANDLOWPOWERSTART;
						}
						break;
					}

					case SCANSTANDTRIGGERED:	  	// we are triggered and have called the decoder, now see what we got, if any thing
					{
						bHeatMitigationHasDecoded = false;
						Set_nAimerState( true );                                      //turn aimer on

						if (WasLastSymbolScanStand())			// if we see SS symbol again, start over
						{
							ScanStandState=GOTSCANSTAND;
							pSSTimeout.SetExpiration(SCANSTANDLPTO);
							pSSHMTimeout.SetExpiration(SCAN_STAND_HEAT_TIMEOUT);

							if( mScanAimerMode !=0 )		Set_nAimerState( false );               //turn aimer off
						}
						else	if (pSSTimeout.HasExpired())		// timed out trying to decode go to "low power"
						{
							iGetStableImages = 0;
							ssMaxExpTimesGain = 0;
							bGetScanStandExp = TRUE;
							ScanStandState=SCANSTANDLOWPOWERSTART;
						}
						else												//did we find a barcode?            //added for Heat Mitigation
						{
							//sprintf(TempStr, "TR DC %d OldDC %d;;;",NumberOfDecodes,OldDecodeCount);      //debug lmc
							//xputstring(TempStr);                                                              //debug lmc
							OldDecodeCount = NumberOfDecodes;                   //update our decode counter
							SetHeatMitigationScanStand(0, 0, true, true, pSSHMTimeout.HasExpired());
						}

						break;
					}

					case SCANSTANDUNTRIGGERED:
					{
						bHeatMitigationHasDecoded = false;
						Set_nAimerState( FALSE );                                 //turn aimer off
						break;
					}

					case SCANSTANDLOWPOWER:
					{
						bHeatMitigationHasDecoded = false;
						OldDecodeCount = NumberOfDecodes;                   //update our decode counter
						Set_nAimerState( FALSE );
						bCurrentTriggerState = FALSE;                                              //we are untriggered
						GetLowPowPixVal( yChk );                                //lets get some pixel data from the present image
						if(bUseScanStandSymbLowPow)
							iCounter = 8;
					  	else
							iCounter = 9;

						int nChanged = 0;
						/* compare present image pixels against reference pixels in xChk */
						for (int i=0;i<iCounter;i++)
						{
							if ( (abs( (int)(xChk[i] - yChk[i]) )) > ChangeThreshold )
								nChanged++;

							//sprintf(TempStr, "%d,%d;;", xChk[i],yChk[i]);   //debug    lmc
							//xputstring(TempStr);                            //debug    lmc
						}

						/* if enough pixels changed, bring up the lights and the reading loop in "main" will attempt a decode to find the
						scan stand symbol*/
						if( nChanged > TriggerThreshold )
						{
							DataToRestore[1] = ssRestoreExp;
							DataToRestore[3] = ssRestoreGain;
							SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)DataToRestore,sizeof(DataToRestore),NULL,0,NULL);
							Set_AllDutyCycle( lightsduty );
							pSSTimeout.SetExpiration(SCANSTANDLPTO);
							pSSHMTimeout.SetExpiration(SCAN_STAND_HEAT_TIMEOUT);
							ScanStandState = WAITFORSCANSTAND;
						}
						else
						{
							//if(pSSHMTimeout.HasExpired())                                                //debug lmc
							//{
							//	sprintf(TempStr, "LP DC %d OldDC %d;;;",NumberOfDecodes,OldDecodeCount);      //debug lmc
							//	xputstring(TempStr);                                                             //debug lmc
							//}

							SetHeatMitigationScanStand(-1, -1, false, false, pSSHMTimeout.HasExpired());     //added for Heat Mitigation
						}
						break;
					}

					case SCANSTANDLOWPOWERSTART:
					{
						bHeatMitigationHasDecoded = false;
						//xputstring("IN:");                                //debug lmc
//						RESET_WATCHDOG_TIMER();
						Set_nAimerState( FALSE );                       //turn aimer off
						if( InBatchedMode() )   								   			//Heat Mitigation
						{
							Set_AllDutyCycle( (10>GET_SETTING(NonOpticalTrigLedIntensityPercentage))?lightsduty:10 );      //LED duty cycle set to 10% or less in batched mode for heat
						}
						else
						{
							Set_AllDutyCycle( (20>GET_SETTING(NonOpticalTrigLedIntensityPercentage))?lightsduty:20 );      //LED duty cycle set to 20% or less otherwise
						}
						bCurrentTriggerState = FALSE;                                       //we are untriggered
						if(iGetStableImages < SCAN_STAND_MAX_NUM_IMAGES_FOR_LOW_POWER)   //get a few more images to settle things down
						{
							iGetStableImages++;
							//xputstring("COUNT:");                                //debug lmc
							if( bGetScanStandExp && (iGetStableImages > 2))
							{
								//xputstring("CAPTURE:");                                //debug lmc
								SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_EXPOSURE_SETTINGS,(PBYTE)DataToRestore,sizeof(DataToRestore),NULL,0,NULL);
								ssRestoreExpTemp = DataToRestore[1];
								ssRestoreGainTemp = DataToRestore[3];

								SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_IMAGE_ATTRIBUTES,(UCHAR *)&P_SCANBUFFER,0,(PBYTE) Stuff,4,&nSize);
								ssExp = Stuff[1];
								ssGain = Stuff[3];
								ssExpTimesGain = ssExp * ssGain;

								if(ssExpTimesGain > ssMaxExpTimesGain)
								{
									ssMaxExpTimesGain = ssExpTimesGain;
									ssMaxExp = ssExp;
									ssMaxGain = ssGain;

									if( InBatchedMode() )
									{
										ssRestoreExp = GET_SETTING( MaximumExposureAuto );
										ssRestoreGain = GET_SETTING( MaximumGainAuto );
									}
									else
									{
										ssRestoreGain = ssRestoreGainTemp;
										ssRestoreExp = ssRestoreExpTemp;
									}

									//sprintf(TempStr, "Found:%lu,%lu,%lu,%lu;;", ssMaxExp,ssMaxGain,ssRestoreExpTemp,ssRestoreGainTemp);      //debug lmc
									//xputstring(TempStr);                               //debug lmc
									//sprintf(TempStr, "Restore:%lu,%lu,%lu,%lu;;", ssMaxExp,ssMaxGain,ssRestoreExp,ssRestoreGain);      //debug lmc
									//xputstring(TempStr);                               //debug lmc
								}

								DataToSend[1] = ssMaxExp;
								DataToSend[3] = ssMaxGain;

								if( iGetStableImages == SCAN_STAND_MAX_NUM_IMAGES_FOR_LOW_POWER )
								{
									SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)DataToSend,sizeof(DataToSend),NULL,0,NULL);
									bGetScanStandExp = FALSE;
									//xputstring("SET:");                                //debug lmc
								}
							}
							break;   //Lets go get another image
						}

						GetLowPowPixVal( yChk );                            //lets get some pixel data from the image for reference

						//sprintf(TempStr, "%d,%d;", iImageWidth,iImageHeight);           //debug    lmc
						//xputstring(TempStr);															//debug    lmc

						if(bUseScanStandSymbLowPow)
							iCounter = 8;                      //8 pixel samples from scan stand symbol location
					  	else
							iCounter = 9;							 //9 pixel samples from image center, if have not yet found a scan stand symbol

						for(int i=0;i<iCounter;i++)
						{
							xChk[i] = yChk[i];     //lets store and save the reference values

							//sprintf(TempStr, "%d,%d;", xChk[i],yChk[i]);           //debug    lmc
							//xputstring(TempStr);                               //debug    lmc
						}

						pSSHMTimeout.SetExpiration(SCAN_STAND_HEAT_TIMEOUT);
						ScanStandState = SCANSTANDLOWPOWER;
						break;
					}
				}	// switch scan stand state
			}  // end scanstand trigger

		// For the mode where the host is to be notified of trigger events, there is nothing to do.
			case NOTIFY_HOST:
			case SNAP_AND_SHIP_TRIGGER:
					break;
		} //end switch trigger mode

		break;	// if we made it here exit while loop
	} while (true);


	if( bAllignmentModeEnabled )     //if 4800dr allignment mode
	{
		if( bAllignmentModeLightsForced )
		{
			Set_nLightsState( true );
		}
		else
		{
			Set_nLightsState( false );
		}
	}

	//theStdInterface.printf("TrigState:%d\r\n",mTriggerMode);	

	if( HardwareToken.IAmOEM() )        //in oem, need to block lights if not enumerated   			
	{
		if( !AreWeUsbTermId() || (AreWeUsbTermId() && theStdInterface.DeviceIoControl(IOCTRL_AM_I_ENUMERATED)) || (AreWeUsbTermId() && (mTriggerMode!=PRESENTATION_TRIGGER) && (mTriggerMode!=STREAMING_PRESENTATION_TRIGGER)) )
		{
			SetLights( (mLightsForcedOn)?true:nLightsState, (bPreAimerDlyRunning)?100:AllDutyCycle, (mAimersForcedOn)?true:nAimerState, (mAimersForcedOn)?100:AllDutyCycle);
		}									
	}
	else
	{
		SetLights( (mLightsForcedOn)?true:nLightsState, (bPreAimerDlyRunning)?100:AllDutyCycle, (mAimersForcedOn)?true:nAimerState, (mAimersForcedOn)?100:AllDutyCycle);
	}

	MT_ReleaseMutex(TriggerMutex);
}


void CTrigger::PrsModesFoundPDF(int bGotOne)
{
	static int nPDFDownCount=0;
	static bool nPDFtrigger=FALSE;

	if ((mTriggerMode != STREAMING_PRESENTATION_TRIGGER) && (mTriggerMode != PRESENTATION_TRIGGER)) return;

	switch (bGotOne)
	{
		// No PDF seen in this image
		case 0:
				if (m_PrsModesPDFdetected || nPDFtrigger) {
					nPDFDownCount--;
					if (nPDFDownCount <= 0) {
						if (m_PrsModesPDFdetected){m_PrsModesPDFdetected = 0; if (mTriggerMode == STREAMING_PRESENTATION_TRIGGER) SetTriggerMode();}
						nPDFtrigger = FALSE;
					}
				}
				break;
		// A PDF was seen in this image
		case 1:
				// if there's a change we have to set the trigger mode, but if not we just reset the down count;
				if (!m_PrsModesPDFdetected) {
					nPDFDownCount++;
					if ((nPDFDownCount >= 1)&&(nPDFtrigger == FALSE)){
						m_PrsModesPDFdetected = 1;
						if (mTriggerMode == STREAMING_PRESENTATION_TRIGGER)
							SetTriggerMode();
						nPDFDownCount = 1;
					} else if (nPDFDownCount >= 6) nPDFDownCount = 6;
				}

				theScriptInterpreter.SequencePriorityTimeoutRefresh(); //if we're doing prioritized sequencing, and preferred is a PDF
				break;
		// A decode was gotten in this image, therefore set an amount of images before you can go into PDF mode
		case 2: if (m_PrsModesPDFdetected != 0) { m_PrsModesPDFdetected = 0; if (mTriggerMode == STREAMING_PRESENTATION_TRIGGER) SetTriggerMode();}
				nPDFtrigger = TRUE;
				nPDFDownCount = 6;
			   break;
	}
}


bool CTrigger::PrsModesPDFdetected(void)
{
	// if we're not using a presentation mode, the presentation modes' PDF detected stuff is irrelavent
	if ((mTriggerMode != STREAMING_PRESENTATION_TRIGGER) && (mTriggerMode != PRESENTATION_TRIGGER))
		return 0;
	else
	return m_PrsModesPDFdetected;
}

/*----------------------------------------------------------------------------------------
AdjustAimerCurrentsForStreamingPresentationMode

  Adjusts the aimer currents when we're in streaming presentation mode.  If the original
  values are read OK, they are saved and aimer currents are changed.

  Parameters:  none

  Returns:  nothing
----------------------------------------------------------------------------------------*/
void CTrigger ::AdjustAimerCurrentsForStreamingPresentationMode(void)
{
	char OldValOperMax, OldValOperational;
	bool bIsOk;

	if ( m_SavedAimerCurrentVals.bAreSaved ) return;  // Aimer currents have already been adjusted (we've been called once before)
	bIsOk = 0;//Psoc5100Read(PSOC_OFFSET_AIMER_CURRENT_OPER_MAX, &OldValOperMax, 1);          //lmc   fixme
	if ( bIsOk ) bIsOk = 0;//Psoc5100Read(PSOC_OFFSET_AIMER_CURRENT_OPER, &OldValOperational, 1);            //lmc   fixme
	if ( bIsOk )
	{	// both vals read OK, save them, then update the regs
		//BYTE NewVal = STREAMING_PRESENT_MODE_AIMER_CURRENT_VALS;  // We want a lower value for the current drive to lower the heat the unit produces, but we have to bump it back up for other trigger modes or else the aimer is too dim.
		m_SavedAimerCurrentVals.OperMax = OldValOperMax;
		m_SavedAimerCurrentVals.Operational = OldValOperational;
		m_SavedAimerCurrentVals.bAreSaved = true;
		//UpdatePsocRegisters(PSOC_OFFSET_AIMER_CURRENT_OPER_MAX, &NewVal, 1);                //lmc   fixme
		//UpdatePsocRegisters(PSOC_OFFSET_AIMER_CURRENT_OPER, &NewVal, 1);                    //lmc   fixme
	}
}




/*----------------------------------------------------------------------------------------
UpdatePsocRegisters

  Helper function for using the IOCTL to update the psoc registers

  Parameters:
				Address[in]:  Starting address (offset) of registers to update

				Data[in]:  pointer to data for update.  Note, it can't be const 'cause of SCN_IOControl()

				LengthOfData[in]:  length of data to update

  Returns:  true if IOCTL is successful, false otherwise
----------------------------------------------------------------------------------------*/
bool CTrigger ::UpdatePsocRegisters(int Address, unsigned char *Data, int LengthOfData)
{
	if ( SCN_IOControl(nOpenCntxt, HHPSD_IOCTL_UPDATE_PSOC_REG, (PBYTE)&Address,
						NULL, (PBYTE)Data, (DWORD)LengthOfData, NULL) ) return true;
	else return false;
}


//override light control with this method - aimer on
void CTrigger ::ForceAimerOn(void)
{
	mAimersForcedOn = true;
}

//override light control with this method - aimer off
void CTrigger ::ForceAimerOff(void)
{
	mAimersForcedOn = false;
}

//override light control with this method - LEDs on
void CTrigger ::ForceLEDsOn(void)
{
	mLightsForcedOn = true;
}

//override light control with this method - LEDs off
void CTrigger ::ForceLEDsOff(void)
{
	mLightsForcedOn = false;
}

void CTrigger ::DebugIt(void)
{
	theStdInterface.printf("A %d F%d \r", bCurrentTriggerState, FinishedScanning);
	printf("SerialTRG: %d\r\n",bSerialTrigger);
}

void CTrigger::WaitForScanStand(void)
{
	pSSTimeout.SetExpiration(SCANSTANDLPTO);
	pSSHMTimeout.SetExpiration(SCAN_STAND_HEAT_TIMEOUT);
	ScanStandState=WAITFORSCANSTAND;
}

bool CTrigger::IsWaitingForScanStand(void)
{
	return  (mTriggerMode==SCANSTAND_TRIGGER && ScanStandState==WAITFORSCANSTAND);        //trigger state and scan stand state
}


//********* Scan Stand low power pixel value check ***********************
// returns 9 pixels in center of image if scan stand barcode has not yet been found, otherwise,
// returns 8 pixels from scan stand barcode's location, found by decoder
void CTrigger::GetLowPowPixVal(UINT *GetPixelValue)      //returns 9 pixel values from image in P_SCANBUFFER
{
									  		// 1/2 inch by 1/2 inch pixel box, centered, measured at approx 6 inch depth of field
	iImageWidth = GetImageWidth();
	iImageHeight = GetImageHeight();
	iFractionImageWidth = (iImageWidth / 20);                                //1/20 of width
	iFractionImageHeight = (iImageHeight / 12);                  				//1/12 of height
	iBoxLeftCollPixel   = (11 * iFractionImageWidth);                                    //left collumn of box
	iBoxCenterCollPixel = (10 * iFractionImageWidth);                                    //center collumn of box
	iBoxRightCollPixel  = (9 * iFractionImageWidth);                                     //right collumn of box
	iBoxUpperRowPixel  = iImageWidth * (5 * iFractionImageHeight);                 //upper row of box
	iBoxMiddleRowPixel = iImageWidth * (6 * iFractionImageHeight);                 //middle row of box
	iBoxLowerRowPixel  = iImageWidth * (7 * iFractionImageHeight);                 //lower row of box

	if(bUseScanStandSymbLowPow)                  //if we found a scan stand symbol, we can use its 8 pixel locations
	{
		CheckForScanStand(P_SCANBUFFER, GetPixelValue);
	}
	else
	{
		//lets make a half inch square pixel box in the center of image, calc. box size is approx. at 6 inch depth of field   (we use 9 pixel locations here)

		GetPixelValue[0] = (UINT) (P_SCANBUFFER[ (iBoxMiddleRowPixel - iBoxLeftCollPixel) ]) ;
		GetPixelValue[1] = (UINT) (P_SCANBUFFER[ (iBoxMiddleRowPixel - iBoxCenterCollPixel) ]) ;
		GetPixelValue[2] = (UINT) (P_SCANBUFFER[ (iBoxMiddleRowPixel - iBoxRightCollPixel) ]) ;
		GetPixelValue[3] = (UINT) (P_SCANBUFFER[ (iBoxUpperRowPixel - iBoxLeftCollPixel) ]) ;
		GetPixelValue[4] = (UINT) (P_SCANBUFFER[ (iBoxUpperRowPixel - iBoxCenterCollPixel) ]) ;
		GetPixelValue[5] = (UINT) (P_SCANBUFFER[ (iBoxUpperRowPixel - iBoxRightCollPixel) ]) ;
		GetPixelValue[6] = (UINT) (P_SCANBUFFER[ (iBoxLowerRowPixel - iBoxLeftCollPixel) ]) ;
		GetPixelValue[7] = (UINT) (P_SCANBUFFER[ (iBoxLowerRowPixel - iBoxCenterCollPixel) ]) ;
		GetPixelValue[8] = (UINT) (P_SCANBUFFER[ (iBoxLowerRowPixel - iBoxRightCollPixel) ]) ;
	}
	return;
}


#ifndef _LMC_DEBUGING_AIMER
//CTrigger::Set_nAimerState sets state of aimer in common method
void CTrigger::Set_nAimerState( int NEW_nAimerState )
{
	nAimerState = NEW_nAimerState;
}

#else
//CTrigger::_DEBUG_Set_nAimerState sets state of aimer in common method with debug, substituted for above
void CTrigger::_DEBUG_Set_nAimerState( int NEW_nAimerState, unsigned int iCodeLine )
{
	static int nSaveAimerState = -1;

	nAimerState = NEW_nAimerState;

	//if( pCHWTrigger->IsTriggerPulled() )
	if( (nAimerState >= 0) )
	{
		if( nSaveAimerState != nAimerState )
		{
			theStdInterface.printf("Called from line %d, Aimer State is %d \r\n", iCodeLine, nAimerState);
		}

		nSaveAimerState = nAimerState;
	}
}
#endif


#ifndef _LMC_DEBUGING_LIGHTS
//CTrigger::Set_nLightsState sets state of lights in common method
void CTrigger::Set_nLightsState( int NEW_nLightsState )
{
	nLightsState = NEW_nLightsState;
}

#else
//CTrigger::_DEBUG_Set_nLightsState sets state of lights in common method with debug, substituted for above
void CTrigger::_DEBUG_Set_nLightsState( int NEW_nLightsState, unsigned int iCodeLine)
{
	static int nSaveLightsState = -1;

	nLightsState = NEW_nLightsState;

	//if (pCHWTrigger->IsTriggerPulled())
	if( (nSaveLightsState != nLightsState) )
	{
		theStdInterface.printf("Called from line %d, Lights State is %d \r\n", iCodeLine, nLightsState);
	}

	nSaveLightsState = nLightsState;
}
#endif



#ifndef _LMC_DEBUGING_LIGHT_INTENSITY
//CTrigger::Set_AllDutyCycle sets light intensity in a common method
void CTrigger::Set_AllDutyCycle( int NEW_AllDutyCycle )
{
	AllDutyCycle = NEW_AllDutyCycle;
}

#else
//CTrigger::_DEBUG_Set_AllDutyCycle sets light intensity in common method with debug, substituted for above
void CTrigger::_DEBUG_Set_AllDutyCycle( int NEW_AllDutyCycle, unsigned int iCodeLine)
{
	AllDutyCycle = NEW_AllDutyCycle;

	//if (pCHWTrigger->IsTriggerPulled())
	{
		theStdInterface.printf("Called from line %d, Light Inten is %d \r\n", iCodeLine, AllDutyCycle);
	}
}
#endif



#ifndef _LMC_DEBUGING_LIGHT_INTENSITY_OPT_TRIG
//CTrigger::Set_LedDutyCycle sets optical trigger light intensity in a common method
void CTrigger::Set_LedDutyCycle( int NEW_LedDutyCycle )
{
	LedDutyCycle = NEW_LedDutyCycle;
}

#else
//CTrigger::_DEBUG_Set_LedDutyCycle sets optical trigger light intensity in common method with debug, substituted for above
void CTrigger::_DEBUG_Set_LedDutyCycle( int NEW_LedDutyCycle, unsigned int iCodeLine)
{
	LedDutyCycle = NEW_LedDutyCycle;

	//if (pCHWTrigger->IsTriggerPulled())
	{
		theStdInterface.printf("Called from line %d, Opt Trig Light Inten is %d \r\n", iCodeLine, LedDutyCycle);
	}
}
#endif



bool CTrigger::NeedToChangeToManual(void)
{
	bool TriggeredOrNot = pCHWTrigger->IsTriggerPulled();

	if( mBlockTempManualOnTriggerChange )
	{
		if( !pCHWTrigger->IsTriggerPulled() )
		{
			mBlockTempManualOnTriggerChange = false;
			BlockDecoderFromExecuting = false;
		}

		return false;
	}
///ewr05430
	else if( (theOutputModes.IsSequenceModeEnabled()) && (theOutputModes.IsSequenceStarted()) && (mSerialTriggerTimeout!=0 && SerialTriggerTimeout.HasExpired()))
	{
		TriggeredTimeout.SetExpiration(500);
		return true;
	}
	else
	{
		TriggeredTimeout.SetExpiration(GET_SETTING(PresModeTimeOut));
		return pCHWTrigger->IsTriggerPulled();
	}
}


//**********************************************************************************
//AreWeLightBarEngine() checks for PSOC on 5100 engines and then checks to see is we have a light bar.
//Returns: bool of true or false and also sets the class variable bWeAreLightBarEngine.
bool CTrigger ::AreWeLightBarEngine(void)
{
	//bWeAreLightBarEngine = false;           //lmc   fixme
	bWeAreLightBarEngine = true;

	// Check for 5100 with light bar
	if(0)// ( ImageEngineHasPsoc() )
	{	// We have a PSOC, check its engine ID to see if it's one of interest
		DWORD GetEngineId[2];  // 1st DWORD has tag, 2nd DWORD will be filled in with the value
		GetEngineId[0] = HHPSD_ENGINE_ID;
		if ( SCN_IOControl(nOpenCntxt, HHPSD_IOCTL_GET_ENGINE_CONFIG, (PBYTE)GetEngineId, sizeof GetEngineId, NULL, 0, NULL) )
		{
         if(1)// ( (GetEngineId[1] == ENGINE_ID_5100_LED_OFF_AXIS_LIGHT_BAR_SF) ||
              //(GetEngineId[1] == ENGINE_ID_5100_LED_OFF_AXIS_LIGHT_BAR_SR) ||
              //(GetEngineId[1] == ENGINE_ID_5100_LED_OFF_AXIS_LIGHT_BAR_HD) )
			{	// We have a PSOC for the 5100 engine with off axis light bar--set class variable.
				bWeAreLightBarEngine = true;
			}
		}
	}
	return bWeAreLightBarEngine;
}



//************** Optical Trigger Defines *****************************

#ifdef FLASH_LEDS
	#define LONGEST_EXPO_LIGHTBAR	(4) // 5100 with off axis light bar for Normal Pres mode
	#define LONGEST_EXPO_LIGHTBAR_STREAMING_PRES	(4) // 5100 with off axis light bar for Streaming Pres mode
	#define REDUCED_EXPO_LIGHTBAR_STREAMING_PRES	(1) // 5100 with off axis light bar for Streaming Pres mode
#else
	#define LONGEST_EXPO_LIGHTBAR	(8) // 5100 with off axis light bar for Normal Pres mode
	#define LONGEST_EXPO_LIGHTBAR_STREAMING_PRES	63 // 5100 with off axis light bar for Streaming Pres mode
	#define REDUCED_EXPO_LIGHTBAR_STREAMING_PRES	(1) // 5100 with off axis light bar for Streaming Pres mode
#endif

#define LONGEST_EXPO_DEFAULT 2624 // 3 frames per second
#define LONGEST_EXPO_MICRON 7874 // ? frames per second
#define LONGEST_EXPO_JADE	 1969 // ? frames per second

#define NEXT_FRAME_SENSIIVITY		5				//Reduce sensitivity with every loop through idle, allow S.D. to stabilize

static bool DimCurrentLedDutyCycle = false;



//optical trigger routine used to go to reduced exposure in streaming pres mode, for oem
void CTrigger::GoToReducedExposureInStreaming(void)
{
	DWORD DataToSend[4];
	//---- things are sent in groups of 2s, the first is the tag, and the second is the value----
	DataToSend[0] = ES_FIXED_EXP;
	DataToSend[1] = REDUCED_EXPO_LIGHTBAR_STREAMING_PRES;
	
	NormalImagingDataToRestore[0] = ES_FIXED_EXP;
	NormalImagingDataToRestore[2] = ES_FIXED_GAIN;
	
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_EXPOSURE_SETTINGS,(PBYTE)NormalImagingDataToRestore,4*sizeof(DWORD),NULL,0,NULL);
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)DataToSend,sizeof(DataToSend),NULL,0,NULL);

	NextFrameSensitivity = 10;		//decrease sensitivity after we change exposure to give us a chance to adjust 
	ReducedExposure = TRUE;
}



//optical trigger routine used to return to normal exposure in streaming pres mode
void CTrigger ::GoToNormalExposureInStreaming(void)
{
	if (ReducedExposure)
	{
		if( (NormalImagingDataToRestore[0]==ES_FIXED_EXP) && (NormalImagingDataToRestore[2]==ES_FIXED_GAIN) )
		{
			SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)NormalImagingDataToRestore,4*sizeof(DWORD),NULL,0,NULL);
		}

		ReducedExposure = FALSE;
	}
}



//optical trigger routine use to reset optical trigger
void CTrigger::ResetOpticalTrigger(void)
{
	IdleMe=0;
	ReducedImaging = FALSE;
	BeLessSensitive=0;
	NextFrameSensitivity = 0;
	ReducedExposure = FALSE;

#ifdef FLASH_LEDS
	Set_LedDutyCycle( 150 );
#else
	Set_LedDutyCycle( 100 );
#endif

	RampDownComplete = FALSE;
	LastLedDutyCycle = -1;
	DevalueNextScan = 0;
	OldDecodeCount = 0;
	NumberOfTriggersWithoutDecode = 0;
	DimCurrentLedDutyCycle = false;
	NoChangeClock = DimClock = TimeClock = BaselineClock;
}

//optical trigger routine used to go to reduced imaging
void CTrigger::GoToReducedImaging(void)
{
	DWORD DataToSend[12];
	//---- things are sent in groups of 2s, the first is the tag, and the second is the value----
	DataToSend[0] = ES_MAX_EXP;

	if( GET_SETTING( ImageEngineType ) == HHPSD_ENGINE_IMAGER_MICRON_MT9V022 )
	{
		DataToSend[1] = LONGEST_EXPO_MICRON;
	}
	else
	{
		DataToSend[1] = ( GET_SETTING(ImagePresentationExposure) );				//was:  else DataToSend[1] = LONGEST_EXPO_DEFAULT;
	}

	DataToSend[2] = ES_MAX_GAIN;
	DataToSend[3] = 1;
	DataToSend[4] = ES_TARGET_VALUE;
	DataToSend[5] = GET_SETTING(PresModeLowLightTarget);
	DataToSend[6] = ES_CONFORM_IMAGE;
	DataToSend[7] = 0;
	DataToSend[8] = ES_CONFORM_TRIES;
	DataToSend[9] = 1;
	DataToSend[10] = ES_ADJUST_EXP_WHEN_ILLUM_OFF;
	DataToSend[11] = TRUE;

	// Adjust for engine with light bar
	if( AreWeLightBarEngine() )
	{	// We have a PSOC, check its engine ID to see if it's one of interest
		if( (STREAMING_PRESENTATION_TRIGGER != mTriggerMode) )    //If regular presentation mode mode
		{
			if( bPresModeLightBarLowLightOn )          //if low lights are enabled in idle
			{
				DataToSend[1] = GET_SETTING(PresentationExposure);      //exposure (default is 4, equiv to 0.5 ms actual exposure)
				DataToSend[3] = GET_SETTING(PresentationGain);          //gain     (default is 2 x, not to excede 4 x due to noise in image)
				DataToSend[7] = true;                                   //must conform
				DataToSend[9] = 5;                                      //conform tries
			}
			else  //lights are off
			{
				DataToSend[7] = true;                                   //must conform
				DataToSend[9] = 3;                                      //conform tries

				#ifdef FLASH_LEDS
					DataToSend[1] = LONGEST_EXPO_JADE;                  //exposure, with lights off, let it run long
					DataToSend[3] = 2;							           //gain     (set to 2 x, default is 2 x also, not to excede 4 x due to noise in image)
				#endif
			}
		}
		else		//else we are in streaming pres mode
		{
			DataToSend[1] = LONGEST_EXPO_LIGHTBAR_STREAMING_PRES;                  //exposure
		}
	}

	NormalImagingDataToRestore[0] = ES_MAX_EXP;
	NormalImagingDataToRestore[2] = ES_MAX_GAIN;
	NormalImagingDataToRestore[4] = ES_TARGET_VALUE;
	NormalImagingDataToRestore[6] = ES_CONFORM_IMAGE;
	NormalImagingDataToRestore[8] = ES_CONFORM_TRIES;
	NormalImagingDataToRestore[10] = ES_ADJUST_EXP_WHEN_ILLUM_OFF;
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_EXPOSURE_SETTINGS,(PBYTE)NormalImagingDataToRestore,sizeof(NormalImagingDataToRestore),NULL,0,NULL);
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)DataToSend,sizeof(DataToSend),NULL,0,NULL);
	NextFrameSensitivity = 1;
	//NextFrameSensitivity = NEXT_FRAME_SENSIIVITY;
	ReducedImaging = TRUE;
	Set_AllDutyCycle( LedDutyCycle );
	#ifdef HI2D_T
	if(ILLUM_LED_LEVL0 == GET_SETTING(Hi2dPresIdleCurrent))
	{
		((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_flash.turnoff(HI2D_AIM_ALT, true);
    }
	else
	{
		((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_current.enterHi2dCurrentLevel(GET_SETTING(Hi2dPresIdleCurrent));
    }
	//turnoff_illum();//ken @2012.01.05 in idle mode illumination close
	//((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_flash.turnon((ScanAimerIllumMode_t)GET_SETTING(ScanAimerMode));
	((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_aimer.turnoff(HI2D_AIM_ALT);
	((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_current.enter_low();
	//((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_current.enterHi2dCurrentLevel(GET_SETTING(LedIntensityPercentage));
	#endif
}


//optical trigger routine used to return to normal imaging
void CTrigger ::GoToNormalImaging(void)
{
	if (ReducedImaging)
	{
		#ifdef HI2D_T
		//turnon_illum();//ken@2012.01.05 when triggered open the illumination
		//theStdInterface.printf("normal\r\n");
		//((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_flash.turnon((ScanAimerIllumMode_t)GET_SETTING(ScanAimerMode));
		((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_flash.turnon((ScanAimerIllumMode_t)GET_SETTING(ScanAimerMode), GET_SETTING(IllOnDuringDecode));
		((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_aimer.turnon((ScanAimerIllumMode_t)GET_SETTING(ScanAimerMode));
		((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_current.enter_high();
		((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_current.enterHi2dCurrentLevel(ILLUM_LED_LEVL4);
		#endif
		Set_LedDutyCycle( GET_SETTING(LedIntensityPercentage) );
		//If we are a light bar engine and have lights off during idle, then wait for short exposure image before allowing lights
		if( !bPresModeLightBarLowLightOn && bWeAreLightBarEngine )  //This will keep user from being blinded
		{
			Set_LedDutyCycle( 0 );
			DimCurrentLedDutyCycle = true;

#ifdef FLASH_LEDS
			BlockDecoderFromExecuting = TRUE;
#endif
		}
		else
		{
#ifdef FLASH_LEDS
			BlockDecoderFromExecuting = FALSE;
#endif
		}

		Set_AllDutyCycle( LedDutyCycle );
		Set_nAimerState( mCurrentScanAimerMode > 0 );

		if( (NormalImagingDataToRestore[0]==ES_MAX_EXP) && (NormalImagingDataToRestore[2]==ES_MAX_GAIN) &&
			(NormalImagingDataToRestore[4]==ES_TARGET_VALUE) && (NormalImagingDataToRestore[6]==ES_CONFORM_IMAGE) &&
			(NormalImagingDataToRestore[8]==ES_CONFORM_TRIES) && (NormalImagingDataToRestore[10]==ES_ADJUST_EXP_WHEN_ILLUM_OFF) )
		{
			SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)NormalImagingDataToRestore,sizeof(NormalImagingDataToRestore),NULL,0,NULL);
		}

		IdleMe = FALSE;

#ifndef FLASH_LEDS
		BlockDecoderFromExecuting = FALSE;
#endif

		//NextFrameSensitivity = 0;
		NoChangeClock = DimClock = TimeClock = BaselineClock;
	}
}

#define TO_PRESENT_MODE_MS			5000
#define TO_STREAM_PRESENT_MODE_MS	20000
//----------------------------------------------------------------------------------------																										 /*----------------------------------------------------------------------------------------
//OpticalTrigger
//
//  NOTE:  these comments were obtained by reverse engineering the source.  This function
//  has two primary "modes" determined by the global variable IdleMe.  If TRUE, the function
//  is looking for a scene change by comparing the difference between areas of one image
//  with that of the previous image.  If FALSE, it does a quicker, less robust algorithm
//  for detecting scene changes.  It also manages timeouts and looks at decodes to determine
//  when to set IdleMe to TRUE so it can do a more robust check,
//
//  This function also controls the LEDs depending on the checks for scene changes.  If IdleMe
//  is TRUE, the LEDs are set to their "off" state.  If FALSE, the LEDs are set to their menu'd
//  (normal) state.
//
//  Parameters:  none
//
//  Returns:  nothing
//----------------------------------------------------------------------------------------
//
#ifdef HI2D_T//TODO for HI2D temporarily
#define DIM 32
#define SAMPLE_NUMBER 300

void CTrigger::OpticalTrigger()
{
	int LevelCount = 0, EdgeCount = 0;
	int x,y;
	int OldValue,NewValue;
	int OldLevel,NewLevel;
	UCHAR *y_ptr;
	UCHAR *ptr;
	int i;
	static int History[DIM][DIM];
	static int Levels[SAMPLE_NUMBER];
	int mean=0;
	unsigned int NoiseDelta = 0;
	unsigned int NoiseMean = 0;
	unsigned int NoiseCount = 0;
	unsigned int NoiseFloorHigh = 0;
	unsigned int NoiseFloorLow = 0;
	unsigned int NoiseMeanHigh = 0;
	unsigned int NoiseCountHigh = 0;
	unsigned int NoiseMeanLow = 0;
	unsigned int NoiseCountLow = 0;
	unsigned int Histogram[256];
	unsigned int Maxi,Mini;
	unsigned int North,South,East,West,Center;
	int Threshold;
	static UINT NumbClock = 0, PreAimerClock = 0;
	//static int GoNumbAfterDecode = 0, DelayForPreAiming = 0;    //comment out for Heat Mitigation
	int a,b;
	int ThisImageExposure;
	unsigned int Height,Width;

	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&Height,4,NULL);
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&Width,4,NULL);

	//theStdInterface.printf("h:%d w:%d\r\n", Height, Width);//ken for HI2D
	/*HI2D sensor is 640*480 but color space is YUV and format is YUYV, so the width is 640*2 = 1280
	Only the Y is useful, Ken@2011.10.25 is modifying the code for HI2D.
	*/

	if( STREAMING_PRESENTATION_TRIGGER != mTriggerMode ) 	// Ignore this part for streaming presentation mode --
	{                                                     // it's specific to "regular" presentation mode
		if( GoNumbAfterDecode )
		{
			PRINT_DEBUG_S(" Numb \r\n"); //lmc debug

			if( BaselineClock-NumbClock > GET_SETTING(PresModeNumbTimeout) )
			{
				GoNumbAfterDecode = FALSE;
				DevalueNextScan = 1;       //add for Heat Mitigation
				PRINT_DEBUG_S(" Numb Done \r\n"); //lmc debug
			}
			else           //used to be "else return;"  //Heat Mitigation
			{
				AllowHeatMitigationCoolDown = true;
			}
			return;
		}

		if( DelayForPreAiming )
		{
			PRINT_DEBUG_S(" Delay \r\n"); //lmc debug

			if (BaselineClock-PreAimerClock > GET_SETTING(PresModePreAimerDelay))
			{
				DelayForPreAiming = FALSE;
				GoToNormalImaging();
				NoChangeClock = DimClock = TimeClock = BaselineClock;
				if (GET_SETTING(GeneralDebugOn)) xputstring("Off ");
				NumberOfTriggersWithoutDecode++;
				ForceAimerOff();                          //allow return to normal aimer state and set force aimer to off
				Set_nAimerState( FALSE );
				PRINT_DEBUG_S(" Delay Done \r\n"); //lmc debug
			}
			else return;
		}
	}		//end if not STREAMING_PRESENTATION_TRIGGER

	NextFrameSensitivity = 0;		//now decrementing below , to allow for less sensitivity as we change modes
	//if( NextFrameSensitivity ) NextFrameSensitivity--;

	DWORD Stuff[] = {HHPSD_IMAGE_STAT_EXPOSURE,0,HHPSD_IMAGE_STAT_GAIN,0,HHPSD_IMAGE_STAT_ILLUM_VALUE,0};
	//DWORD nSize;
	//Ken @2011.10.28 for HI2D
	//SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_IMAGE_ATTRIBUTES,(UCHAR *)&P_SCANBUFFER,0,(PBYTE) Stuff,6,&nSize);

	//Current Image stat exposure
	ThisImageExposure = Stuff[1];
	//Current Image stat Illum value
	mean = Stuff[5];
	//char rr[33];sprintf(rr,"E%d,G%d,I%d ",Stuff[1],Stuff[3],Stuff[5]); xputstring(rr);

	// for when we are in idle mode: We have lots of time so be thorough.
	if (IdleMe)
	{
		PRINT_DEBUG_S(" Idle \r\n"); //lmc debug

		if (DevalueNextScan)         //Erase Bin History
		{
			for (y=0; y<DIM; y++) for (x=0; x<DIM; x++)  History[x][y] = 0;        //clear history
			DevalueNextScan = 0;
		}

		// Characterize noise floor
		for (i=0; i<256; i++)Histogram[i]=0;           //clear histogram

		NoiseMean = 0;
		NoiseCount = 0;

		//Find the max and min in the image sample blocks
		for (y=1;y<Height;y+=3)                     //move through image y axis
		{
			//ptr = P_SCANBUFFER+y*Width;
			y_ptr = P_SCANBUFFER+y*Width;

			for (x=1; x<Width; x+=3)                    //move through image x axis
			{
				ptr = y_ptr + x;
				Mini=255; Maxi = 0;

				Center = 0xff & (unsigned int)(*ptr);
				North = 0xff & (unsigned int)(*(ptr-Width));
				South = 0xff & (unsigned int)(*(ptr+Width));
				East = 0xff & (unsigned int)(*(ptr+1));
				West = 0xff & (unsigned int)(*(ptr-1));

				if (Center<Mini) Mini=Center; if (Center>Maxi) Maxi=Center;
				if (North<Mini) Mini=North; if (North>Maxi) Maxi=North;
				if (South<Mini) Mini=South; if (South>Maxi) Maxi=South;
				if (East<Mini) Mini=East; if (East>Maxi) Maxi=East;
				if (West<Mini) Mini=West; if (West>Maxi) Maxi=West;

				Histogram[Maxi-Mini]++;                 //record number of edge differences of each magnitude found in image, for all sample blocks, to make histogram
				NoiseCount++;
				NoiseMean = NoiseMean + (Maxi-Mini);
			}
		}

		//Find total Mean of magnitudes, this should seperate noise floor from signal
		NoiseMean = NoiseMean / NoiseCount;

		Maxi = 0;		//Saves the largest count (amount for most common contrast) found between all the edge difference above mean , 'i' will indicate histogram location for noise floor, which is also edge difference value
		Mini = 0;     //Saves the next largest count (amount for most common contrast) found between all the edge difference below mean
		NoiseDelta = 0;		//A weighted value used in calculation to judge if we return to normal
		NoiseFloorHigh = 0;	//Saves location of Maxi, which is the contrast value
		NoiseFloorLow = 0;	//Saves location of Mini, which is the contrast value
		NoiseMeanHigh = 0;     //Mean of data Above the total mean
		NoiseCountHigh = 0;
		NoiseMeanLow = 0;      //Mean of data below the total mean
		NoiseCountLow = 0;

		//  Determine the noise floor
		for (i=0;i<256;i++)
		{
			if( i > NoiseMean )
			{
				if( Histogram[i] > Maxi )	//Find the largest edge difference count (contrast) above total mean in the Histogram of the image
				{
					Maxi = Histogram[i];
					NoiseFloorHigh = i;
				}

				NoiseMeanHigh = NoiseMeanHigh + ( Histogram[i] * i );
				NoiseCountHigh = NoiseCountHigh + Histogram[i];

			}
			else if( i <= NoiseMean )
			{
				if( Histogram[i] > Mini )          //Find the largest edge difference count (contrast) below total mean in the Histogram of the image
				{
					Mini = Histogram[i];
					NoiseFloorLow = i;
				}

				NoiseMeanLow = NoiseMeanLow + ( Histogram[i] * i );
				NoiseCountLow = NoiseCountLow + Histogram[i];
			}
		}

		//Calculate the mean of the signal above the total mean
		if( NoiseCountHigh == 0 )	NoiseMeanHigh = 1;
		else NoiseMeanHigh = NoiseMeanHigh / NoiseCountHigh;
		PRINT_DEBUG_T("NoiseSignal: %d , Peaks %d , Cnt %d \r\n", NoiseMeanHigh, NoiseFloorHigh, NoiseCountHigh ); //lmc debug

		//Calculate the mean of the signal below the total mean, the noise floor
		if( NoiseCountLow == 0 )	NoiseMeanLow = 1;
		else NoiseMeanLow = NoiseMeanLow / NoiseCountLow;
		PRINT_DEBUG_T("NoiseFloor: %d , Peaks %d , Cnt %d \r\n", NoiseMeanLow, NoiseFloorLow, NoiseCountLow ); //lmc debug

		//Set Noise delta according to signal to noise
		if( NoiseMeanLow == 0 )	NoiseDelta = NoiseMeanHigh;
		else NoiseDelta = NoiseMeanHigh / NoiseMeanLow;

		if( NoiseDelta >= 30)		//at least 30 db down noise floor
		{
			NoiseDelta = 4;
		}
		else if( NoiseDelta >= 10)		//at least 20 db down noise floor
		{
			NoiseDelta = 3;
		}
		else if( NoiseDelta >= 6)		//at least 15 db down noise floor
		{
			NoiseDelta = 2;
		}
		else if( NoiseDelta >= 3)		//at least 10 db down noise floor
		{
			NoiseDelta = 1;
		}
		else if( NoiseDelta >= 2)		//at least 6 db down noise floor
		{
			NoiseDelta = 0;
		}
		else
		{
			NoiseDelta = 0;
		}
		PRINT_DEBUG_T("NoiseDelta: %d \r\n", NoiseDelta); //lmc debug
		HI2D_PRINT_DBG("NoiseDelta: %d \r\n", NoiseDelta); 

		//  Determine the Arithmetic Mean for brightness
		//ken here if the image is yuyv the brightness method should be changed for HI2D
		ptr = P_SCANBUFFER;
		for (i=0;i<Width*Height; i+=55) {mean+=*ptr; ptr+=55;}
		mean = mean/(Width*Height/55);
		PRINT_DEBUG_T("Mean: %d \r\n", mean); //lmc debug
		HI2D_PRINT_DBG("Mean: %d \r\n", mean);//white is 255 black is 0
		// Manage the LEDs
		if( STREAMING_PRESENTATION_TRIGGER != mTriggerMode )		// We don't want to ramp the lights down
		{                                                       //if we're in streaming presentation mode
			int iLowLights = 0;

			if( bWeAreLightBarEngine && bPresModeLightBarLowLightOn )        // Adjust for engine with light bar and low lights mode
			{	// We have a engine with off axis light bar--adjust for it.
			 	iLowLights = GET_SETTING(PresModeMitigationLowLights);
			}
			else                                                    //lights off mode
			{
				iLowLights = GET_SETTING(PresModeLowLights);
			}

			if( LedDutyCycle != iLowLights )
			{
				if( bWeAreLightBarEngine && bPresModeLightBarLowLightOn && (LedDutyCycle>=20) )     //if low lights on, we configure pres mode differently, so lights only dim
				{
					#ifdef FLASH_LEDS       //if ff3 gen6 board
						if(LedDutyCycle>100)		Set_LedDutyCycle( 100 );
					#else
						Set_LedDutyCycle( LedDutyCycle - 10 );
					#endif                  //end if ff3 gen6 board
				}
				else if( bWeAreLightBarEngine && bPresModeLightBarLowLightOn )    //if low lights on, we configure pres mode differently, so lights only dim
				{
					#ifdef FLASH_LEDS       //if ff3 gen6 board
						if(LedDutyCycle>100)		Set_LedDutyCycle( 100 );
					#else
						Set_LedDutyCycle( LedDutyCycle - 1 );
					#endif                  //end if ff3 gen6 board
				}
				else if( bWeAreLightBarEngine && !bPresModeLightBarLowLightOn && (LedDutyCycle>=20) )         //no lights mode
				{
					#ifdef FLASH_LEDS       //if ff3 gen6 board
						if(LedDutyCycle>0)		Set_LedDutyCycle( 0 );
					#else
						Set_LedDutyCycle( LedDutyCycle - 10 );
					#endif                  //end if ff3 gen6 board
				}
				else if( bWeAreLightBarEngine && !bPresModeLightBarLowLightOn )                    //no lights mode
				{
					#ifdef FLASH_LEDS       //if ff3 gen6 board
						if(LedDutyCycle>0)		Set_LedDutyCycle( 0 );
					#else
						Set_LedDutyCycle( LedDutyCycle - 1 );
					#endif                  //end if ff3 gen6 board
				}
				else if( GET_SETTING( ImageEngineType ) == HHPSD_ENGINE_IMAGER_MICRON_MT9V022 )		Set_LedDutyCycle( LedDutyCycle - 10 );
				else Set_LedDutyCycle( LedDutyCycle - 20 );

				NextFrameSensitivity = 1;
				//NextFrameSensitivity = NEXT_FRAME_SENSIIVITY;
			}

			//Correct LedDutyCycle to menu values
			if (LedDutyCycle<iLowLights)		Set_LedDutyCycle( iLowLights );
  			if (LedDutyCycle>GET_SETTING(LedIntensityPercentage))		Set_LedDutyCycle( GET_SETTING(LedIntensityPercentage) );

			PRINT_DEBUG_T(" LedCycle: %d LedLow %d\r\n", LedDutyCycle, iLowLights); //lmc debug

			//Is LED ramp down completed
			if( (LedDutyCycle == iLowLights) && !RampDownComplete )
			{
				RampDownComplete = TRUE;
				GoToReducedImaging();
				DevalueNextScan = 1;
			}
		}		//end if not STREAMING_PRESENTATION_TRIGGER

		// be more rigorous when already idling to detect image change to return to normal
		// divide the image into bins
		int Delta = (10 + NoiseDelta) * (1+GET_SETTING(PresModeSensitivity) / 5);
		PRINT_DEBUG_T("Delta: %d \r\n", Delta); //lmc debug
		HI2D_PRINT_DBG("Delta: %d \r\n", Delta); 

  		for (y=0; y<DIM; y++)
		{
  			for (x=0; x<DIM; x++)
			{
				// measure each bin - compare adjacent areas for contrast
  				ptr = (P_SCANBUFFER + (Height/(2*DIM) + y*(Height/DIM)) * Width + x*Width/DIM);
				NewValue=0;
				a= *ptr + *(ptr+1) + *(ptr-1) + *(ptr-Width) + *(ptr+Width);                   //Bin close to its center sum
				b= *(ptr+10) + *(ptr+11) + *(ptr+9) + *(ptr+10-Width) + *(ptr+10+Width);       //Bin outside edge sum

				if (a>1250 || b>1250);     //= 0
				else if (a > b + Delta) NewValue = 1;
				else if (a < b - Delta) NewValue = 2;

				OldValue = History[x][y];            //Get Old History
				History[x][y] = NewValue;            //Store New History

				//If values are different from history and non zero for this Image bin
				if (OldValue != NewValue)
				{
					if (NewValue != 0)
					{
						if (OldValue != 0) EdgeCount+=1;               //Then Increment Edgecount for this image
					}
				}
			}
		}
		PRINT_DEBUG_T("EdgeCnt: %d \r\n", EdgeCount); //lmc debug
		HI2D_PRINT_DBG("EdgeCnt: %d \r\n", EdgeCount); //lmc debug

		Threshold = 20 + 40 * BeLessSensitive + 25 * ((NumberOfTriggersWithoutDecode > 10) ? 1:0);
		PRINT_DEBUG_T("ThresA: %d, LessSens: %d, NumTrig: %d \r\n", Threshold, BeLessSensitive, NumberOfTriggersWithoutDecode); //lmc debug
		Threshold = GET_SETTING(PresModeSensitivity)*(1+Threshold/5);
		PRINT_DEBUG_T("ThresB: %d \r\n", Threshold); //lmc debug
		{
			#ifdef FLASH_LEDS       //if ff3 gen6 board
				if (ThisImageExposure == LONGEST_EXPO_JADE)
				{
					Threshold += 20;
				}
				else if (ThisImageExposure == (GET_SETTING(PresentationExposure)))
				{
					if( bPresModeLightBarLowLightOn )          //if low lights are enabled in idle
					{
						Threshold += 1;	//if a light bar unit in normal Pres mode, lights on during idle
					}
					else                               //if lights off in idle
					{
						Threshold += 10;	//if a light bar unit in normal Pres mode, lights off during idle
					}
				}

				PRINT_DEBUG_T("ThresC: %d \r\n", Threshold); //lmc debug
				HI2D_PRINT_DBG("ThresC: %d \r\n", Threshold); //for HI2D
			#else
				if (ThisImageExposure == LONGEST_EXPO_DEFAULT) Threshold += 50;
			#endif                  //end if ff3 gen6 board
		}

		if( BeLessSensitive )		BeLessSensitive--;			//Every time we loop through idle, lets be less sensitive

		if (RampDownComplete)		AllowHeatMitigationCoolDown = true;   //added for Heat Mitigation

		 //Compare EdgeCount for this image to threshold to see if we can return to normal imaging
		if (EdgeCount > Threshold)
		{                                        // We don't want to ramp the lights on if we're in streaming presentation mode
			if( GET_SETTING(PresModePreAimerDelay) && (STREAMING_PRESENTATION_TRIGGER != mTriggerMode) )
			{
				DelayForPreAiming = TRUE;
				Set_nAimerState( TRUE );
				PreAimerClock = BaselineClock;
				SetLights(0,0,true,100);
				ForceAimerOn();                //override normal aimer state and force aimer on
				return;
			}
			else
			{

				GoToNormalImaging();
				NoChangeClock = DimClock = TimeClock = BaselineClock;
				if (GET_SETTING(GeneralDebugOn)) xputstring("Off ");
				NumberOfTriggersWithoutDecode++;
			}
		}
		//else if are cell and we decoded, because we only use histogram to test short illum frame, not the long no lights cell frame
		else if( mPresentationScanDriverCellPhone && (STREAMING_PRESENTATION_TRIGGER == mTriggerMode) && (HardwareToken.IAmOEM())&& (NumberOfDecodes != OldDecodeCount) )
		{	
			// We need to "reset" the state 'cause it won't happen in GotoNormalImaging() because we never go into "ReducedImaging"
			IdleMe = FALSE;  // This puts us back to the "start" state

			if( mCurrentScanAimerMode > 0 ) 
				Set_nAimerState( TRUE );
			else 
				Set_nAimerState( FALSE );

			GoToNormalExposureInStreaming();
			GoToNormalImaging();
			NoChangeClock = DimClock = TimeClock = BaselineClock;
			if (GET_SETTING(GeneralDebugOn)) 
				xputstring("Off ");			
		}
	}
	else       // LEDs on and Decoding, not in idle mode - Test to see if we can go to idle state
	{
		PRINT_DEBUG_S(" Norm \r\n"); //lmc debug

		//if we have a new decode then reset counters
		if( (mCurrentScanAimerMode != 0) )
		{
			Set_nAimerState( TRUE );	//We're not currently idles, so go see if we want aimers to be on.
		}

		//If we are a light bar  engine and have lights off during idle, then wait for short exposure image before allowing lights
		if( DimCurrentLedDutyCycle && !bPresModeLightBarLowLightOn && bWeAreLightBarEngine )  //This will keep user from being blinded
		{
			PRINT_DEBUG_S(" Dim \r\n"); //lmc debug

			if( (ThisImageExposure > (GET_SETTING(PresentationExposure))) )    //if exposure is above our "Normal" max exposure,
			{                                                                  //then shut lights off so we do not get a bright LED burst when in the dark
				Set_LedDutyCycle( 0 );           				//shut light off until exposure settles below max limit (allow time for long exposures in idle to get short again)
			}
			else
			{
				Set_LedDutyCycle( GET_SETTING(LedIntensityPercentage) );          //restore duty cycle when we get a "Normal" exposure
				DimCurrentLedDutyCycle = false;

#ifdef FLASH_LEDS
				BlockDecoderFromExecuting = FALSE;
#endif
			}
		}
		else
		{
			DimCurrentLedDutyCycle = false;
		}

		if (NumberOfDecodes != OldDecodeCount)   		//NumberOfDecodes has been incremented in decodeiface, so we have decoded
		{
			PRINT_DEBUG_S(" Decode \r\n"); //lmc debug

			HeatMitigationHasDecodeOutput = true;      //got a decode, so disable heat mitigation          //added for Heat Mitigation
			OldDecodeCount = NumberOfDecodes;                                 //update our decode counter
         	TimeClock = DimClock = BaselineClock;
			NumberOfTriggersWithoutDecode = 0;       //added for Heat Mitigation

			if( (GET_SETTING(PresModePriceChecker) == FALSE) )
			{
				GoNumbAfterDecode = TRUE;
				NumbClock = BaselineClock;
				IdleMe = TRUE;
 				DevalueNextScan = 1;
				BlockDecoderFromExecuting = TRUE;
				RampDownComplete = TRUE;          		//changed for Heat Mitigation from false to true

				if( bWeAreLightBarEngine && bPresModeLightBarLowLightOn )        // Adjust for engine with light bar
				{
			 		Set_LedDutyCycle( GET_SETTING(PresModeMitigationLowLights) );
					LastLedDutyCycle = LedDutyCycle;        //save current intensity, so we do not blink aimer when we go to idle mode next
				}
				else if( bWeAreLightBarEngine )
				{
					Set_LedDutyCycle( GET_SETTING(PresModeLowLights) );
				}
				else
				{
					Set_LedDutyCycle( 0 );                       //added for Heat Mitigation
				}

				GoToReducedImaging();                 //added for Heat Mitigation
            			DevalueNextScan = 1;                  //added for Heat Mitigation
            			AllowHeatMitigationCoolDown = true;   //added for Heat Mitigation
            			return;
			}
			//NumberOfTriggersWithoutDecode = 0;     //removed for Heat Mitigation
		}

		// Test for changes from the last frame
		ptr = P_SCANBUFFER + DIM;
		b = 0;
		int *LvlPtr = Levels;
		int NextSampleOffset = Width*Height/SAMPLE_NUMBER;
		NextSampleOffset &=0xfffffffe;//force to even

		for (y=0; y<SAMPLE_NUMBER; y++)
		{
			OldLevel = *LvlPtr;
			a= *ptr + *(ptr+2) + *(ptr-2);
			ptr+=NextSampleOffset;
			*LvlPtr++ = NewLevel = a;
			if( OldLevel > NewLevel + 50 )		LevelCount+=1;
			else if( OldLevel < NewLevel - 50 )		LevelCount+=1;
		}

		if( (int)mean > 16 )		  // if image is very dark then bail soon
		{
			PRINT_DEBUG_S(" MeanTest:%d \r\n", (int)mean); //lmc debug
			DimClock = BaselineClock;
		}

		if( LevelCount > 80 )
		{
			PRINT_DEBUG_S(" LevelTest:%d \r\n", LevelCount); //lmc debug
			NoChangeClock = BaselineClock;
		}

		//int PMtimeout = 5000;           //removed for Heat Mitigation
		int PMtimeout = GET_SETTING(PresModeUpTimeout);    //added for Heat Mitigation

		if( STREAMING_PRESENTATION_TRIGGER == mTriggerMode )		PMtimeout = TO_STREAM_PRESENT_MODE_MS;

		if( BaselineClock-TimeClock > PMtimeout || BaselineClock-DimClock > PMtimeout>>1 || BaselineClock-NoChangeClock > PMtimeout>>1 )
		{
			PRINT_DEBUG_S(" TimeTestDone \r\n"); //lmc debug
			BlockDecoderFromExecuting = TRUE;
			IdleMe = TRUE;
			RampDownComplete = FALSE;
			//if( STREAMING_PRESENTATION_TRIGGER != mTriggerMode )		NextFrameSensitivity = NEXT_FRAME_SENSIIVITY;
			if( HardwareToken.IAmOEM() && (STREAMING_PRESENTATION_TRIGGER == mTriggerMode) ) //in oem, need to reduce exposure when idle
			{
			 	GoToReducedExposureInStreaming();
			}

			DevalueNextScan = 1;
			if( GET_SETTING(GeneralDebugOn) )  xputstring("On ");
			if( !GET_SETTING(PresModeLEDRampDown) && bAllowHeatMitigation )		Set_LedDutyCycle( 0 );       //added for Heat Mitigation
		}
	}

	if( LastLedDutyCycle != LedDutyCycle )             //If intensity has changed
	{
		Set_AllDutyCycle( LedDutyCycle );                          //set global lights intensity
		Set_nAimerState( mCurrentScanAimerMode > 0 );
		LastLedDutyCycle = LedDutyCycle;                          //save current intensity
	}

	//char rr[33];sprintf(rr,"%d ",LedDutyCycle); xputstring(rr);

	if( NextFrameSensitivity )  BeLessSensitive = NextFrameSensitivity;
	//if( NextFrameSensitivity )  BeLessSensitive = 1;

	//char rr[33];sprintf(rr,"%d ",BaselineClock-timer); xputstring(rr);
	return;
}
//----------------------------------------------------------------------------------------																										 /*----------------------------------------------------------------------------------------
//End OpticalTrigger
//----------------------------------------------------------------------------------------
#else
#define DIM 32
#define SAMPLE_NUMBER 300

void CTrigger::OpticalTrigger()
{
	int LevelCount = 0, EdgeCount = 0;
	int x,y;
	int OldValue,NewValue;
	int OldLevel,NewLevel;
	UCHAR *y_ptr;
	UCHAR *ptr;
	int i;
	static int History[DIM][DIM];
	static int Levels[SAMPLE_NUMBER];
	int mean=0;
	unsigned int NoiseDelta = 0;
	unsigned int NoiseMean = 0;
	unsigned int NoiseCount = 0;
	unsigned int NoiseFloorHigh = 0;
	unsigned int NoiseFloorLow = 0;
	unsigned int NoiseMeanHigh = 0;
	unsigned int NoiseCountHigh = 0;
	unsigned int NoiseMeanLow = 0;
	unsigned int NoiseCountLow = 0;
	unsigned int Histogram[256];
	unsigned int Maxi,Mini;
	unsigned int North,South,East,West,Center;
	int Threshold;
	static UINT NumbClock = 0, PreAimerClock = 0;
	//static int GoNumbAfterDecode = 0, DelayForPreAiming = 0;    //comment out for Heat Mitigation
	int a,b;
	int ThisImageExposure;
	unsigned int Height,Width;

	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&Height,4,NULL);
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&Width,4,NULL);


	if( STREAMING_PRESENTATION_TRIGGER != mTriggerMode ) 	// Ignore this part for streaming presentation mode --
	{                                                     // it's specific to "regular" presentation mode
		if( GoNumbAfterDecode )
		{
			PRINT_DEBUG_S(" Numb \r\n"); //lmc debug

			if( BaselineClock-NumbClock > GET_SETTING(PresModeNumbTimeout) )
			{
				GoNumbAfterDecode = FALSE;
				DevalueNextScan = 1;       //add for Heat Mitigation
				PRINT_DEBUG_S(" Numb Done \r\n"); //lmc debug
			}
			else           //used to be "else return;"  //Heat Mitigation
			{
				AllowHeatMitigationCoolDown = true;
			}
			return;
		}

		if( DelayForPreAiming )
		{
			PRINT_DEBUG_S(" Delay \r\n"); //lmc debug

			if (BaselineClock-PreAimerClock > GET_SETTING(PresModePreAimerDelay))
			{
				DelayForPreAiming = FALSE;
				GoToNormalImaging();
				NoChangeClock = DimClock = TimeClock = BaselineClock;
				if (GET_SETTING(GeneralDebugOn)) xputstring("Off ");
				NumberOfTriggersWithoutDecode++;
				ForceAimerOff();                          //allow return to normal aimer state and set force aimer to off
				Set_nAimerState( FALSE );
				PRINT_DEBUG_S(" Delay Done \r\n"); //lmc debug
			}
			else return;
		}
	}		//end if not STREAMING_PRESENTATION_TRIGGER

	NextFrameSensitivity = 0;		//now decrementing below , to allow for less sensitivity as we change modes
	//if( NextFrameSensitivity ) NextFrameSensitivity--;

	DWORD Stuff[] = {HHPSD_IMAGE_STAT_EXPOSURE,0,HHPSD_IMAGE_STAT_GAIN,0,HHPSD_IMAGE_STAT_ILLUM_VALUE,0};
	DWORD nSize;
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_IMAGE_ATTRIBUTES,(UCHAR *)&P_SCANBUFFER,0,(PBYTE) Stuff,6,&nSize);

	//Current Image stat exposure
	ThisImageExposure = Stuff[1];
	//Current Image stat Illum value
	mean = Stuff[5];
	//char rr[33];sprintf(rr,"E%d,G%d,I%d ",Stuff[1],Stuff[3],Stuff[5]); xputstring(rr);

	// for when we are in idle mode: We have lots of time so be thorough.
	if (IdleMe)
	{
		PRINT_DEBUG_S(" Idle \r\n"); //lmc debug

		if (DevalueNextScan)         //Erase Bin History
		{
			for (y=0; y<DIM; y++) for (x=0; x<DIM; x++)  History[x][y] = 0;        //clear history
			DevalueNextScan = 0;
		}

		// Characterize noise floor
		for (i=0; i<256; i++)Histogram[i]=0;           //clear histogram

		NoiseMean = 0;
		NoiseCount = 0;

		//Find the max and min in the image sample blocks
		for (y=1;y<Height;y+=3)                     //move through image y axis
		{
			//ptr = P_SCANBUFFER+y*Width;
			y_ptr = P_SCANBUFFER+y*Width;

			for (x=1; x<Width; x+=3)                    //move through image x axis
			{
				ptr = y_ptr + x;
				Mini=255; Maxi = 0;

				Center = 0xff & (unsigned int)(*ptr);
				North = 0xff & (unsigned int)(*(ptr-Width));
				South = 0xff & (unsigned int)(*(ptr+Width));
				East = 0xff & (unsigned int)(*(ptr+1));
				West = 0xff & (unsigned int)(*(ptr-1));

				if (Center<Mini) Mini=Center; if (Center>Maxi) Maxi=Center;
				if (North<Mini) Mini=North; if (North>Maxi) Maxi=North;
				if (South<Mini) Mini=South; if (South>Maxi) Maxi=South;
				if (East<Mini) Mini=East; if (East>Maxi) Maxi=East;
				if (West<Mini) Mini=West; if (West>Maxi) Maxi=West;

				Histogram[Maxi-Mini]++;                 //record number of edge differences of each magnitude found in image, for all sample blocks, to make histogram
				NoiseCount++;
				NoiseMean = NoiseMean + (Maxi-Mini);
			}
		}

		//Find total Mean of magnitudes, this should seperate noise floor from signal
		NoiseMean = NoiseMean / NoiseCount;

		Maxi = 0;		//Saves the largest count (amount for most common contrast) found between all the edge difference above mean , 'i' will indicate histogram location for noise floor, which is also edge difference value
		Mini = 0;     //Saves the next largest count (amount for most common contrast) found between all the edge difference below mean
		NoiseDelta = 0;		//A weighted value used in calculation to judge if we return to normal
		NoiseFloorHigh = 0;	//Saves location of Maxi, which is the contrast value
		NoiseFloorLow = 0;	//Saves location of Mini, which is the contrast value
		NoiseMeanHigh = 0;     //Mean of data Above the total mean
		NoiseCountHigh = 0;
		NoiseMeanLow = 0;      //Mean of data below the total mean
		NoiseCountLow = 0;

		//  Determine the noise floor
		for (i=0;i<256;i++)
		{
			if( i > NoiseMean )
			{
				if( Histogram[i] > Maxi )	//Find the largest edge difference count (contrast) above total mean in the Histogram of the image
				{
					Maxi = Histogram[i];
					NoiseFloorHigh = i;
				}

				NoiseMeanHigh = NoiseMeanHigh + ( Histogram[i] * i );
				NoiseCountHigh = NoiseCountHigh + Histogram[i];

			}
			else if( i <= NoiseMean )
			{
				if( Histogram[i] > Mini )          //Find the largest edge difference count (contrast) below total mean in the Histogram of the image
				{
					Mini = Histogram[i];
					NoiseFloorLow = i;
				}

				NoiseMeanLow = NoiseMeanLow + ( Histogram[i] * i );
				NoiseCountLow = NoiseCountLow + Histogram[i];
			}
		}

		//Calculate the mean of the signal above the total mean
		if( NoiseCountHigh == 0 )	NoiseMeanHigh = 1;
		else NoiseMeanHigh = NoiseMeanHigh / NoiseCountHigh;
		PRINT_DEBUG_T("NoiseSignal: %d , Peaks %d , Cnt %d \r\n", NoiseMeanHigh, NoiseFloorHigh, NoiseCountHigh ); //lmc debug

		//Calculate the mean of the signal below the total mean, the noise floor
		if( NoiseCountLow == 0 )	NoiseMeanLow = 1;
		else NoiseMeanLow = NoiseMeanLow / NoiseCountLow;
		PRINT_DEBUG_T("NoiseFloor: %d , Peaks %d , Cnt %d \r\n", NoiseMeanLow, NoiseFloorLow, NoiseCountLow ); //lmc debug

		//Set Noise delta according to signal to noise
		if( NoiseMeanLow == 0 )	NoiseDelta = NoiseMeanHigh;
		else NoiseDelta = NoiseMeanHigh / NoiseMeanLow;

		if( NoiseDelta >= 30)		//at least 30 db down noise floor
		{
			NoiseDelta = 4;
		}
		else if( NoiseDelta >= 10)		//at least 20 db down noise floor
		{
			NoiseDelta = 3;
		}
		else if( NoiseDelta >= 6)		//at least 15 db down noise floor
		{
			NoiseDelta = 2;
		}
		else if( NoiseDelta >= 3)		//at least 10 db down noise floor
		{
			NoiseDelta = 1;
		}
		else if( NoiseDelta >= 2)		//at least 6 db down noise floor
		{
			NoiseDelta = 0;
		}
		else
		{
			NoiseDelta = 0;
		}
		PRINT_DEBUG_T("NoiseDelta: %d \r\n", NoiseDelta); //lmc debug

		//  Determine the Arithmetic Mean for brightness
		//ken here if the image is yuyv the brightness method should be changed for HI2D
		ptr = P_SCANBUFFER;
		for (i=0;i<Width*Height; i+=55) {mean+=*ptr; ptr+=55;}
		mean = mean/(Width*Height/55);
		PRINT_DEBUG_T("Mean: %d \r\n", mean); //lmc debug

		// Manage the LEDs
		if( STREAMING_PRESENTATION_TRIGGER != mTriggerMode )		// We don't want to ramp the lights down
		{                                                       //if we're in streaming presentation mode
			int iLowLights = 0;

			if( bWeAreLightBarEngine && bPresModeLightBarLowLightOn )        // Adjust for engine with light bar and low lights mode
			{	// We have a engine with off axis light bar--adjust for it.
			 	iLowLights = GET_SETTING(PresModeMitigationLowLights);
			}
			else                                                    //lights off mode
			{
				iLowLights = GET_SETTING(PresModeLowLights);
			}

			if( LedDutyCycle != iLowLights )
			{
				if( bWeAreLightBarEngine && bPresModeLightBarLowLightOn && (LedDutyCycle>=20) )     //if low lights on, we configure pres mode differently, so lights only dim
				{
					#ifdef FLASH_LEDS       //if ff3 gen6 board
						if(LedDutyCycle>100)		Set_LedDutyCycle( 100 );
					#else
						Set_LedDutyCycle( LedDutyCycle - 10 );
					#endif                  //end if ff3 gen6 board
				}
				else if( bWeAreLightBarEngine && bPresModeLightBarLowLightOn )    //if low lights on, we configure pres mode differently, so lights only dim
				{
					#ifdef FLASH_LEDS       //if ff3 gen6 board
						if(LedDutyCycle>100)		Set_LedDutyCycle( 100 );
					#else
						Set_LedDutyCycle( LedDutyCycle - 1 );
					#endif                  //end if ff3 gen6 board
				}
				else if( bWeAreLightBarEngine && !bPresModeLightBarLowLightOn && (LedDutyCycle>=20) )         //no lights mode
				{
					#ifdef FLASH_LEDS       //if ff3 gen6 board
						if(LedDutyCycle>0)		Set_LedDutyCycle( 0 );
					#else
						Set_LedDutyCycle( LedDutyCycle - 10 );
					#endif                  //end if ff3 gen6 board
				}
				else if( bWeAreLightBarEngine && !bPresModeLightBarLowLightOn )                    //no lights mode
				{
					#ifdef FLASH_LEDS       //if ff3 gen6 board
						if(LedDutyCycle>0)		Set_LedDutyCycle( 0 );
					#else
						Set_LedDutyCycle( LedDutyCycle - 1 );
					#endif                  //end if ff3 gen6 board
				}
				else if( GET_SETTING( ImageEngineType ) == HHPSD_ENGINE_IMAGER_MICRON_MT9V022 )		Set_LedDutyCycle( LedDutyCycle - 10 );
				else Set_LedDutyCycle( LedDutyCycle - 20 );

				NextFrameSensitivity = 1;
				//NextFrameSensitivity = NEXT_FRAME_SENSIIVITY;
			}

			//Correct LedDutyCycle to menu values
			if (LedDutyCycle<iLowLights)		Set_LedDutyCycle( iLowLights );
  			if (LedDutyCycle>GET_SETTING(LedIntensityPercentage))		Set_LedDutyCycle( GET_SETTING(LedIntensityPercentage) );

			PRINT_DEBUG_T(" LedCycle: %d LedLow %d\r\n", LedDutyCycle, iLowLights); //lmc debug

			//Is LED ramp down completed
			if( (LedDutyCycle == iLowLights) && !RampDownComplete )
			{
				RampDownComplete = TRUE;
				GoToReducedImaging();
				DevalueNextScan = 1;
			}
		}		//end if not STREAMING_PRESENTATION_TRIGGER

		// be more rigorous when already idling to detect image change to return to normal
		// divide the image into bins
		int Delta = (10 + NoiseDelta) * (1+GET_SETTING(PresModeSensitivity) / 5);
		PRINT_DEBUG_T("Delta: %d \r\n", Delta); //lmc debug

  		for (y=0; y<DIM; y++)
		{
  			for (x=0; x<DIM; x++)
			{
				// measure each bin - compare adjacent areas for contrast
  				ptr = (P_SCANBUFFER + (Height/(2*DIM) + y*(Height/DIM)) * Width + x*Width/DIM);
				NewValue=0;
				a= *ptr + *(ptr+1) + *(ptr-1) + *(ptr-Width) + *(ptr+Width);                   //Bin close to its center sum
				b= *(ptr+10) + *(ptr+11) + *(ptr+9) + *(ptr+10-Width) + *(ptr+10+Width);       //Bin outside edge sum

				if (a>1250 || b>1250);     //= 0
				else if (a > b + Delta) NewValue = 1;
				else if (a < b - Delta) NewValue = 2;

				OldValue = History[x][y];            //Get Old History
				History[x][y] = NewValue;            //Store New History

				//If values are different from history and non zero for this Image bin
				if (OldValue != NewValue)
				{
					if (NewValue != 0)
					{
						if (OldValue != 0) EdgeCount+=1;               //Then Increment Edgecount for this image
					}
				}
			}
		}
		PRINT_DEBUG_T("EdgeCnt: %d \r\n", EdgeCount); //lmc debug

		Threshold = 20 + 40 * BeLessSensitive + 25 * ((NumberOfTriggersWithoutDecode > 10) ? 1:0);
		PRINT_DEBUG_T("ThresA: %d, LessSens: %d, NumTrig: %d \r\n", Threshold, BeLessSensitive, NumberOfTriggersWithoutDecode); //lmc debug
		Threshold = GET_SETTING(PresModeSensitivity)*(1+Threshold/5);
		PRINT_DEBUG_T("ThresB: %d \r\n", Threshold); //lmc debug

		if( GET_SETTING( ImageEngineType ) == HHPSD_ENGINE_IMAGER_MICRON_MT9V022 )
		{
			if (ThisImageExposure == LONGEST_EXPO_MICRON)
			{
				Threshold += 50;
			}
			else if (ThisImageExposure == (GET_SETTING(PresentationExposure)))
			{
				if( bPresModeLightBarLowLightOn )          //if low lights are enabled in idle
				{
					Threshold -= 2;	//if a 5100 light bar unit in normal Pres mode, lights on during idle
				}
				else                               //if lights off in idle
				{
					Threshold += 25;	//if a 5100 light bar unit in normal Pres mode, lights off during idle
				}
			}
		}
		else if( STREAMING_PRESENTATION_TRIGGER == mTriggerMode )        //if streaming pres, adjust threshold for aimer
		{
			if( HardwareToken.IAmOEM() ) //in oem, need to wait for new exposure
			{
				if( (ThisImageExposure > REDUCED_EXPO_LIGHTBAR_STREAMING_PRES) && ReducedExposure && (ThisImageExposure <= LONGEST_EXPO_LIGHTBAR_STREAMING_PRES) )
				{//only do this when illum frames that are not yet to the reduced exposure and are still in the high intensity lights exposure range
					NextFrameSensitivity = 10;		//decrease sensitivity after we change exposure to give us a chance to adjust
					DevalueNextScan = 1;
					PRINT_DEBUG_T("StrPrWaitForIdleExp \r\n"); //lmc debug
				}

				if( mPresentationScanDriverCellPhone )	//if cell streaming pres
				{
					if( ThisImageExposure <= LONGEST_EXPO_LIGHTBAR_STREAMING_PRES )	//high lights short exposure
					{
						Threshold += 25;
					}
					else	//long exposure
					{
					 	Threshold += 35;
					}
				}
				else	//if just streaming pres oem
				{				
					Threshold += 25;
				}
			}
			else
			{
				Threshold += 15;
			}

			PRINT_DEBUG_T("ThresC: %d \r\n", Threshold); //lmc debug
		}
		else		//if not a HHPSD_ENGINE_IMAGER_MICRON_MT9V022
		{
			#ifdef FLASH_LEDS       //if ff3 gen6 board
				if (ThisImageExposure == LONGEST_EXPO_JADE)
				{
					Threshold += 20;
				}
				else if (ThisImageExposure == (GET_SETTING(PresentationExposure)))
				{
					if( bPresModeLightBarLowLightOn )          //if low lights are enabled in idle
					{
						Threshold += 1;	//if a light bar unit in normal Pres mode, lights on during idle
					}
					else                               //if lights off in idle
					{
						Threshold += 10;	//if a light bar unit in normal Pres mode, lights off during idle
					}
				}

				PRINT_DEBUG_T("ThresC: %d \r\n", Threshold); //lmc debug
			#else
				if (ThisImageExposure == LONGEST_EXPO_DEFAULT) Threshold += 50;
			#endif                  //end if ff3 gen6 board
		}

		if( BeLessSensitive )		BeLessSensitive--;			//Every time we loop through idle, lets be less sensitive

		if (RampDownComplete)		AllowHeatMitigationCoolDown = true;   //added for Heat Mitigation

		 //Compare EdgeCount for this image to threshold to see if we can return to normal imaging
		if (EdgeCount > Threshold)
		{                                        // We don't want to ramp the lights on if we're in streaming presentation mode
			if( GET_SETTING(PresModePreAimerDelay) && (STREAMING_PRESENTATION_TRIGGER != mTriggerMode) )
			{
				DelayForPreAiming = TRUE;
				Set_nAimerState( TRUE );
				PreAimerClock = BaselineClock;
				SetLights(0,0,true,100);
				ForceAimerOn();                //override normal aimer state and force aimer on
				return;
			}
			else
			{
				if ( STREAMING_PRESENTATION_TRIGGER == mTriggerMode )
				{	// We need to "reset" the state 'cause it won't happen in GotoNormalImaging() because we never go into "ReducedImaging"
					IdleMe = FALSE;  // This puts us back to the "start" state

					if( mCurrentScanAimerMode > 0 ) Set_nAimerState( TRUE );
					else Set_nAimerState( FALSE );

					if( HardwareToken.IAmOEM() ) //in oem, need to restore exposure when leaving idle
					{
			 			GoToNormalExposureInStreaming();
					}
				}

				GoToNormalImaging();
				NoChangeClock = DimClock = TimeClock = BaselineClock;
				if (GET_SETTING(GeneralDebugOn)) xputstring("Off ");
				NumberOfTriggersWithoutDecode++;
			}
		}
		//else if are cell and we decoded, because we only use histogram to test short illum frame, not the long no lights cell frame
		else if( mPresentationScanDriverCellPhone && (STREAMING_PRESENTATION_TRIGGER == mTriggerMode) && (HardwareToken.IAmOEM())&& (NumberOfDecodes != OldDecodeCount) )
		{	
			// We need to "reset" the state 'cause it won't happen in GotoNormalImaging() because we never go into "ReducedImaging"
			IdleMe = FALSE;  // This puts us back to the "start" state

			if( mCurrentScanAimerMode > 0 ) 
				Set_nAimerState( TRUE );
			else 
				Set_nAimerState( FALSE );

			GoToNormalExposureInStreaming();
			GoToNormalImaging();
			NoChangeClock = DimClock = TimeClock = BaselineClock;
			if (GET_SETTING(GeneralDebugOn)) 
				xputstring("Off ");			
		}
	}
	else       // LEDs on and Decoding, not in idle mode - Test to see if we can go to idle state
	{
		PRINT_DEBUG_S(" Norm \r\n"); //lmc debug

		//if we have a new decode then reset counters
		if( (mCurrentScanAimerMode != 0) )
		{
			Set_nAimerState( TRUE );	//We're not currently idles, so go see if we want aimers to be on.
		}

		//If we are a light bar  engine and have lights off during idle, then wait for short exposure image before allowing lights
		if( DimCurrentLedDutyCycle && !bPresModeLightBarLowLightOn && bWeAreLightBarEngine )  //This will keep user from being blinded
		{
			PRINT_DEBUG_S(" Dim \r\n"); //lmc debug

			if( (ThisImageExposure > (GET_SETTING(PresentationExposure))) )    //if exposure is above our "Normal" max exposure,
			{                                                                  //then shut lights off so we do not get a bright LED burst when in the dark
				Set_LedDutyCycle( 0 );           				//shut light off until exposure settles below max limit (allow time for long exposures in idle to get short again)
			}
			else
			{
				Set_LedDutyCycle( GET_SETTING(LedIntensityPercentage) );          //restore duty cycle when we get a "Normal" exposure
				DimCurrentLedDutyCycle = false;

#ifdef FLASH_LEDS
				BlockDecoderFromExecuting = FALSE;
#endif
			}
		}
		else
		{
			DimCurrentLedDutyCycle = false;
		}

		if (NumberOfDecodes != OldDecodeCount)   		//NumberOfDecodes has been incremented in decodeiface, so we have decoded
		{
			PRINT_DEBUG_S(" Decode \r\n"); //lmc debug

			HeatMitigationHasDecodeOutput = true;      //got a decode, so disable heat mitigation          //added for Heat Mitigation
			OldDecodeCount = NumberOfDecodes;                                 //update our decode counter
         	TimeClock = DimClock = BaselineClock;
			NumberOfTriggersWithoutDecode = 0;       //added for Heat Mitigation

			if( (GET_SETTING(PresModePriceChecker) == FALSE) && (STREAMING_PRESENTATION_TRIGGER != mTriggerMode) )
			{
				GoNumbAfterDecode = TRUE;
				NumbClock = BaselineClock;
				IdleMe = TRUE;
 				DevalueNextScan = 1;
				BlockDecoderFromExecuting = TRUE;
				RampDownComplete = TRUE;          		//changed for Heat Mitigation from false to true

				if( bWeAreLightBarEngine && bPresModeLightBarLowLightOn )        // Adjust for engine with light bar
				{
			 		Set_LedDutyCycle( GET_SETTING(PresModeMitigationLowLights) );
					LastLedDutyCycle = LedDutyCycle;        //save current intensity, so we do not blink aimer when we go to idle mode next
				}
				else if( bWeAreLightBarEngine )
				{
					Set_LedDutyCycle( GET_SETTING(PresModeLowLights) );
				}
				else
				{
					Set_LedDutyCycle( 0 );                       //added for Heat Mitigation
				}

				GoToReducedImaging();                 //added for Heat Mitigation
            	DevalueNextScan = 1;                  //added for Heat Mitigation
            	AllowHeatMitigationCoolDown = true;   //added for Heat Mitigation
            	return;
			}
			//NumberOfTriggersWithoutDecode = 0;     //removed for Heat Mitigation
		}

		// Test for changes from the last frame
		ptr = P_SCANBUFFER + DIM/2;
		b = 0;
		int *LvlPtr = Levels;
		int NextSampleOffset = Width*Height/SAMPLE_NUMBER;

		for (y=0; y<SAMPLE_NUMBER; y++)
		{
			OldLevel = *LvlPtr;
			a= *ptr + *(ptr+1) + *(ptr-1);
			ptr+=NextSampleOffset;
			*LvlPtr++ = NewLevel = a;
			if( OldLevel > NewLevel + 50 )		LevelCount+=1;
			else if( OldLevel < NewLevel - 50 )		LevelCount+=1;
		}

		if( (int)mean > 16 )		  // if image is very dark then bail soon
		{
			PRINT_DEBUG_S(" MeanTest:%d \r\n", (int)mean); //lmc debug
			DimClock = BaselineClock;
		}

		if( LevelCount > 80 )
		{
			PRINT_DEBUG_S(" LevelTest:%d \r\n", LevelCount); //lmc debug
			NoChangeClock = BaselineClock;
		}

		//int PMtimeout = 5000;           //removed for Heat Mitigation
		int PMtimeout = GET_SETTING(PresModeUpTimeout);    //added for Heat Mitigation

		if( STREAMING_PRESENTATION_TRIGGER == mTriggerMode )		PMtimeout = TO_STREAM_PRESENT_MODE_MS;

		if( BaselineClock-TimeClock > PMtimeout || BaselineClock-DimClock > PMtimeout>>1 || BaselineClock-NoChangeClock > PMtimeout>>1 )
		{
			PRINT_DEBUG_S(" TimeTestDone \r\n"); //lmc debug
			BlockDecoderFromExecuting = TRUE;
			IdleMe = TRUE;
			RampDownComplete = FALSE;
			//if( STREAMING_PRESENTATION_TRIGGER != mTriggerMode )		NextFrameSensitivity = NEXT_FRAME_SENSIIVITY;
			if( HardwareToken.IAmOEM() && (STREAMING_PRESENTATION_TRIGGER == mTriggerMode) ) //in oem, need to reduce exposure when idle
			{
			 	GoToReducedExposureInStreaming();
			}

			DevalueNextScan = 1;
			if( GET_SETTING(GeneralDebugOn) )  xputstring("On ");
			if( !GET_SETTING(PresModeLEDRampDown) && bAllowHeatMitigation )		Set_LedDutyCycle( 0 );       //added for Heat Mitigation
		}
	}

	if( LastLedDutyCycle != LedDutyCycle )             //If intensity has changed
	{
		Set_AllDutyCycle( LedDutyCycle );                          //set global lights intensity
		Set_nAimerState( mCurrentScanAimerMode > 0 );
		LastLedDutyCycle = LedDutyCycle;                          //save current intensity
	}

	//char rr[33];sprintf(rr,"%d ",LedDutyCycle); xputstring(rr);

	if( NextFrameSensitivity )  BeLessSensitive = NextFrameSensitivity;
	//if( NextFrameSensitivity )  BeLessSensitive = 1;

	//char rr[33];sprintf(rr,"%d ",BaselineClock-timer); xputstring(rr);
	return;
}
//----------------------------------------------------------------------------------------																										 /*----------------------------------------------------------------------------------------
//End OpticalTrigger
//----------------------------------------------------------------------------------------

#endif


bool CTrigger::ProcessChangeToFromTempManualTrigger(void)
{
#ifndef XENON_LITE_1500
	int dh, dw, dr, dx, dy; //NG2D875
#endif

	if (TriggeredInAutoOrPresMode)
	{
		TriggeredTimeout.Reset();
		SetTriggerMode(TEMP_MANUAL_TRIG);
		theScriptInterpreter.ResetScanningState();
		TriggeredInAutoOrPresMode = false;		
		ResetIqScoring();
#ifndef XENON_LITE_1500
		DecodeGet(DEC_IMAGE_HEIGHT, (void *)&dh);
		DecodeGet(DEC_IMAGE_WIDTH, (void *)&dw);
		DecodeGet(DEC_IMAGE_LINE_DELTA, (void *)&dr);
		DecodeGet(DEC_IMAGE_CENTER_X, (void *)&dx);
		DecodeGet(DEC_IMAGE_CENTER_Y, (void *)&dy);
		SetImageParameters(dh, dw, dr, dx, dy);//NG2D875
#endif
		return false;
	}

	if (mTriggerMode == TEMP_MANUAL_TRIG)
	{
		if( pCHWTrigger->IsTriggerPulled() )
		{
			TriggeredTimeout.Reset();
		}
		else if( TriggeredTimeout.HasExpired() )
		{
			SetTriggerMode();		// return to nominal mode
#ifndef XENON_LITE_1500
			DecodeGet(DEC_IMAGE_HEIGHT, (void *)&dh);
			DecodeGet(DEC_IMAGE_WIDTH, (void *)&dw);
			DecodeGet(DEC_IMAGE_LINE_DELTA, (void *)&dr);
			DecodeGet(DEC_IMAGE_CENTER_X, (void *)&dx);
			DecodeGet(DEC_IMAGE_CENTER_Y, (void *)&dy);
			SetImageParameters(dh, dw, dr, dx, dy);//NG2D875
#endif
			return true;
		}
	}
	return false;
}


bool CTrigger::NeedToHandleScanStandSwitch(void)
{
	bool bTriggerChanged = false;
	int CurrentMenuedTriggerMode = GET_SETTING(TriggerMode);
	bool bBlockWhileNotEnum = false;

	if( ((GET_SETTING(CblSelNumber)==CBL_USB) || (AreWeUsbTermId() && HardwareToken.IAmOEM())) && (!theStdInterface.DeviceIoControl(IOCTRL_AM_I_ENUMERATED)) )      //if usb cable and enumerated
	{
		bBlockWhileNotEnum = true;
	}

	if( mUseSSSwitch && !bBlockWhileNotEnum )         //if menued to allow use of scan stand switch
	{
		if( pCHWTrigger->ReadReedSwitch() )               //if we are in the stand, use streaming pres
		{
		#ifdef HI2D_T
			if( (mTriggerMode != PRESENTATION_TRIGGER) && (mTriggerMode != TEMP_MANUAL_TRIG) )     //wait for temp manual timer to complete before changing to stream pres in stand
			{
				//theStdInterface.printf("********************\r\n");
				SetTriggerMode(PRESENTATION_TRIGGER);
				mSavedTriggerMode = PRESENTATION_TRIGGER;		//always save state if we are not setting to the menued setting
				bTriggerChanged = true;
			}
		#else
			if( (mTriggerMode != STREAMING_PRESENTATION_TRIGGER) && (mTriggerMode != TEMP_MANUAL_TRIG) )     //wait for temp manual timer to complete before changing to stream pres in stand
			{
				SetTriggerMode(STREAMING_PRESENTATION_TRIGGER);
				mSavedTriggerMode = STREAMING_PRESENTATION_TRIGGER;		//always save state if we are not setting to the menued setting
				bTriggerChanged = true;
			}
		#endif
		}
		else if( (CurrentMenuedTriggerMode != mTriggerMode) && (mTriggerMode != TEMP_MANUAL_TRIG) )		//else out of stand, and running trigger mode does not match menued trigger mode
		{
			SetTriggerMode();		// return to nominal mode
			mSavedTriggerMode = CurrentMenuedTriggerMode;		//restore menued setting as the saved value
			bTriggerChanged = true;
		}
	}

	return bTriggerChanged;
}



/* -------------------------------------------------
*  IsLowpowerTriggerMode checks if OK for low power
* --------------------------------------------------*/
bool CTrigger::IsLowpowerTriggerMode(void)
{
	bool bLowPowerOK;
	bLowPowerOK = ( !bAllignmentModeEnabled && (mTriggerMode != TEMP_MANUAL_TRIG) );
	return bLowPowerOK;
}


/* -------------------------------------------------
*  TriggerAllignmentMode sets allignment mode on
* --------------------------------------------------*/
	void CTrigger::TriggerAllignmentMode(void)
	{
		if( TriggeredInAutoOrPresMode )
		{
			SetTriggerMode(MANUAL_TRIGGER);
			theScriptInterpreter.ResetScanningState();
			TriggeredInAutoOrPresMode = false;
		}
		//set allignment variables
		bAllignmentModeEnabled = true;                //Enable Allignment mode after setting the trigger mode above
		bAllignmentModeForceSerialTrigger = true;     //make timeout always set to zero when SetSerialTriggerTimeout called
		//set timers
		SetSerialTriggerTimeout(0);
		SetGoodReadTimeout();
		SetReReadTimeout();
		SetReReadTimeout2D();
		SetAllignmentModeTriggerTimeout();
		//trigger it
		bSerialTrigger = true;
		BlockDecoderFromExecuting = true;
		//set light controls
		ForceAimerOn();
		bAllignmentModeLightsForced = false;
		//reset counts
		uiNoLightCount = 0;
		uiTriggerTimeoutCount = 0;
		//reset timers before we leave
		ResetAllTimersForAllignmentMode();
	}

/*--------------------------------------------------
* 	UnTriggerAllignmentMode sets allignment mode off
* --------------------------------------------------*/
	void CTrigger::UnTriggerAllignmentMode(void)
	{
		//reset counts
		uiNoLightCount = 0;
		uiTriggerTimeoutCount = 0;
		//set allignment variables to their default off state
		bAllignmentModeEnabled = false;
		bAllignmentModeLightsForced = false;
		bAllignmentModeForceSerialTrigger = false;
		BlockDecoderFromExecuting = false;
		//lets untrigger
		bSerialTrigger = false;
		ScanStandState = SCANSTANDUNTRIGGERED;
		//set timers back
		SetGoodReadTimeout();
		SetReReadTimeout();
		SetReReadTimeout2D();
		//back to menued trigger state
		ForceAimerOff();                        //allow return to normal aimer state and set force aimer to off
		SetSerialTriggerTimeout();
	}

	/*--------------------------------------------------
	* 	SetAllignmentModeTriggerTimeout sets allignment mode timer's timeout
	* --------------------------------------------------*/
	#define	ALLIGNMENT_MODE_TIMEOUT	250

	void CTrigger::SetAllignmentModeTriggerTimeout(void)
	{
		mAllignmentModeTriggerTimeout = ALLIGNMENT_MODE_TIMEOUT;
		AllignmentModeTriggerTimeout.SetExpiration( mAllignmentModeTriggerTimeout );
	}


	/*--------------------------------------------------
	* 	HasAllignmentModeTriggerTimeoutExpired checks allignment mode timer's timeout
	* --------------------------------------------------*/
	bool CTrigger::HasAllignmentModeTriggerTimeoutExpired(void)
	{
		bool bReturn;
		bReturn = AllignmentModeTriggerTimeout.HasExpired();
		return bReturn;
	}


	/*--------------------------------------------------
	* 	SetMaxAllignmentModeTriggerTimeout sets allignment mode timer's max time on
	* --------------------------------------------------*/
	#define	MAX_ALLIGNMENT_MODE_TIMEOUT	30000                      //30 seconds

	void CTrigger::SetMaxAllignmentModeTriggerTimeout(void)
	{
		mMaxAllignmentModeTriggerTimeout = MAX_ALLIGNMENT_MODE_TIMEOUT;
		MaxAllignmentModeTriggerTimeout.SetExpiration( mMaxAllignmentModeTriggerTimeout );
	}


	/*--------------------------------------------------
	* 	HasMaxAllignmentModeTriggerTimeoutExpired checks allignment mode timer's Max timeout
	* --------------------------------------------------*/
	bool CTrigger::HasMaxAllignmentModeTriggerTimeoutExpired(void)
	{
		bool bReturn;
		bReturn = MaxAllignmentModeTriggerTimeout.HasExpired();
		return bReturn;
	}


	/*--------------------------------------------------
	* 	ForceReadTimersForAllignmentMode sets ReRead variables for allignment mode
	* --------------------------------------------------*/
	unsigned int CTrigger::ForceReadTimersForAllignmentMode( unsigned int uiForcedVariable )
	{
		unsigned int uiReturnedVariable;

		uiReturnedVariable = uiForcedVariable;

		if( bAllignmentModeEnabled )
		{
			uiReturnedVariable = ALLIGNMENT_MODE_TIMEOUT - 50;
		}

		return uiReturnedVariable;
	}


	/*--------------------------------------------------
	* 	ResetAllTimersForAllignmentMode resets timers needed for this mode
	* --------------------------------------------------*/
	void CTrigger::ResetAllTimersForAllignmentMode( void )
	{
		ResetReReadTime();
		ResetReReadTime2D();
		AllignmentModeTriggerTimeout.Reset();
	}


	/*--------------------------------------------------
	* 	SearchForAllignmentModeBarcode Handles barcode search while this mode is enabled, ends mode when needed
	* --------------------------------------------------*/
	#define	NUMBER_OF_DECODER_CALL_TRIES		15
	#define	NUMBER_OF_TRIGGER_TIMEOUTS			4

	void CTrigger::SearchForAllignmentModeBarcode(void)
	{
		if( HasAllignmentModeTriggerTimeoutExpired() )    //only search when timer has expired
		{
			if( (uiTriggerTimeoutCount == 0) )    					//try looking for target with no lights first, do not want flicker
			{                                     									//do so only at set timer intervals for heat reduction
				if( uiNoLightCount < NUMBER_OF_DECODER_CALL_TRIES )  //judge search time by decoder call attempts
				{
					bAllignmentModeLightsForced = false;              		//will not use lights for this time interval
					BlockDecoderFromExecuting = false;
					uiNoLightCount++;
				}
				else                               //turn off lights and reset timer
				{
					uiNoLightCount = 0;
					uiTriggerTimeoutCount++;
					bAllignmentModeLightsForced = false;  //lights forced off during interval
					BlockDecoderFromExecuting = true;
					ResetAllTimersForAllignmentMode();
				}

				//theStdInterface.printf(" Decode no lights: %d \r\n", uiNoLightCount );         //lmc-debug
			}
			else if( (uiTriggerTimeoutCount < NUMBER_OF_TRIGGER_TIMEOUTS) )    //need to wait for target removal and replacement during allignment lock down
			{
				if( uiNoLightCount < NUMBER_OF_DECODER_CALL_TRIES )  //judge search time by decoder call attempts
				{
					bAllignmentModeLightsForced = true;              		//will use lights for this time interval
					BlockDecoderFromExecuting = false;
					uiNoLightCount++;
				}
				else                               	//turn off lights and reset timer
				{
					uiNoLightCount = 0;
					uiTriggerTimeoutCount++;
					bAllignmentModeLightsForced = false;  //lights forced off during interval
					BlockDecoderFromExecuting = true;
					ResetAllTimersForAllignmentMode();
				}

				//theStdInterface.printf(" Decode w lights: %d, %d \r\n", uiTriggerTimeoutCount,  uiNoLightCount);         //lmc-debug
			}
			else												    //times up, lets leave allignment mode
			{
				uiNoLightCount = 0;
				uiTriggerTimeoutCount = 0;
				bAllignmentModeLightsForced = false;  //lights forced off
				BlockDecoderFromExecuting = false;    //allow decoder calls
				UnTriggerAllignmentMode();
				ReaderConfiguration->AllignmentFlagSet();            //last, set the image block allignment done flag
				SetTriggerMode();
				UpdateTriggerStatus();
			}

			//theStdInterface.printf(" Done: \r\n");         //lmc-debug
		}
		else   //else if between search intervals
		{
			bAllignmentModeLightsForced = false;  //lights forced off between intervals
			BlockDecoderFromExecuting = true;     //block decoder from being called for heat issues
		}
	}


	/*--------------------------------------------------
	* 	CheckIfAllignmentModeAllowed checks to see if this mode is allowed
	* --------------------------------------------------*/
	bool CTrigger::CheckIfAllignmentModeAllowed( void )
	{
		bool bIsModeAllowed = false;
		//bIsModeAllowed = true; DR option

		return bIsModeAllowed;
	}



	bool CTrigger::AreWeUsbTermId( void )
	{
		bool bWeAreUsbTermId = false;
		int termid = 0; 
		
		termid = (GET_SETTING(TerminalID));

		if( (termid >= MINIMUN_TERMINAL_USB_ID) && (termid <= MAXIMUM_TERMINAL_USB_ID) )
		{
		 	bWeAreUsbTermId = true;
		}

		return bWeAreUsbTermId;
	}
 


	void CTrigger::SetTempManualTriggerMode( void )
	{
		mSavedTriggerModeNotSet = false;		
		 
		if( (GET_SETTING(CblSelNumber)==CBL_USB) || (AreWeUsbTermId() && HardwareToken.IAmOEM()) )            //if usb cable           (GET_SETTING(TerminalID)==130)
		{
			if( mSavedTriggerMode != MANUAL_TRIGGER )      //if not currenly manual trigger, make it so during enumeration
			{  
				SetTriggerMode(MANUAL_TRIGGER);
				mSavedTriggerModeNotSet = true;        //indicate that we changed trigger mode from the one that is currently set while enumerating
			}
		}
	}

	void CTrigger::RestoreTriggerModeForUSB( void )
	{
		if( ( (GET_SETTING(CblSelNumber)==CBL_USB) || (AreWeUsbTermId() && HardwareToken.IAmOEM()) ) && (theStdInterface.DeviceIoControl(IOCTRL_AM_I_ENUMERATED)) )      //if usb cable and enumerated
		{
			if( mSavedTriggerModeNotSet )            //if waiting for enumeration
			{
				if( (mSavedTriggerMode != mTriggerMode) )        //if saved trigger mode is not the current trigger mode
				{
					SetTriggerMode(mSavedTriggerMode);
				}

				mSavedTriggerModeNotSet = false;
			}
		}
	}

	//ewr04791 Codegate
	// IsPresentationTypeTrigger():
	// Checks which trigger mode the scanner is in for use in setting the decoder window centering parameters.
	bool CTrigger::IsPresentationTypeTrigger(void)
	{
		bool status = false;

//		if( mUseSSSwitch && pCHWTrigger->ReadReedSwitch() )
	    if ((GET_SETTING(ForcePresentationStyleDecode)) || (mTriggerMode == SCANSTAND_TRIGGER) || ((!PrsModesPDFdetected()) && ((mTriggerMode == STREAMING_PRESENTATION_TRIGGER) || (mTriggerMode == PRESENTATION_TRIGGER))))
		{
			status = true;
		}
		return status;
	}
