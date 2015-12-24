//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Trigger/Trigger_5_5.h $
// $Date: 2011/11/08 18:02:47EST $
// $Revision: 1.32 $
//======================================================================================

//======================================================================================


/*-----------------5/30/2002 8:26AM-----------------
 * trigger_5_5.H
 * Implements CTrigger class providing manual and serial trigger
 * as well as auto trigger eventually
 * --------------------------------------------------*/
#ifndef TRIGGER_INCLUDED
#define TRIGGER_INCLUDED
#include "product.h"
#include "platform.h"
#include "timeout.h"
#include "thread.h"

#define MANUAL_TRIGGER 						0
#define AUTOMATIC_TRIGGER 					1
#define LOW_POWER 							2
#define PRESENTATION_TRIGGER 				3
#define SCANSTAND_TRIGGER					4
#define NOTIFY_HOST							5
#define SNAP_AND_SHIP_TRIGGER	 			6
#define TEMP_MANUAL_TRIG 					7
#define STREAMING_PRESENTATION_TRIGGER		8

#ifdef EMERALD_VERIFIER
#define AimerOffStateVrf 	0
#define AimerState1Vrf 		1
#define AimerState2Vrf 		2
#define AimerState3Vrf 		3
#endif

#define GOODREAD_DELAY0 0               /* GoodReadDelay = (4.1mS)(GOODREAD_DELAY) = 0 */
#define GOODREAD_DELAY1 500             /* GoodReadDelay = (4.1mS)(GOODREAD_DELAY) = 500mS */
#define GOODREAD_DELAY2 1000             /* GoodReadDelay = (4.1mS)(GOODREAD_DELAY) = 1000mS */
#define GOODREAD_DELAY3 1500             /* GoodReadDelay = (4.1mS)(GOODREAD_DELAY) = 1500mS */

#define REREAD_DELAY0 175              	 /* ReReadDelay = (4.1mS)(REREAD_DELAY) = 175mS */
#define REREAD_DELAY1 450             	 /* ReReadDelay = (4.1mS)(REREAD_DELAY) = 450mS */
#define REREAD_DELAY2 1000             	 /* ReReadDelay = (4.1mS)(REREAD_DELAY) = 1000mS */
#define REREAD_DELAY3 2000            	 /* ReReadDelay = (4.1mS)(REREAD_DELAY) = 2000mS */


class CTrigger
{
 	enum SCAN_DRIVER_STATE
 	{
 		FINISHED_AND_OFF,
 		FINISHED,
 		ACTIVE
 	};



	public:

		CTrigger( void );
		~CTrigger(void);

		/*-----------------5/30/2002 8:30AM-----------------
		 *  TriggerSerial set serial trigger on
		 * --------------------------------------------------*/
	void TriggerSerial(void);

		inline bool  IsTriggeredSerial(void){return bSerialTrigger==TRUE;}

		/*-----------------5/30/2002 8:32AM-----------------
		 * UnTriggerSerial set serial trigger state off
		 * --------------------------------------------------*/
		void UnTriggerSerial(void);

	private:

		//************************* 4800dr allignment mode functionality *************************************
		bool bAllignmentModeLightsForced;
		bool bAllignmentModeForceSerialTrigger;
		unsigned int uiNoLightCount;
		unsigned int uiTriggerTimeoutCount;

		unsigned int ForceReadTimersForAllignmentMode( unsigned int uiForcedVariable );
		void ResetAllTimersForAllignmentMode( void );

	public:

		bool bAllignmentModeEnabled;			//enable document allignment mode for 4800dr

		void TriggerAllignmentMode(void);
		void UnTriggerAllignmentMode(void);
		void SetAllignmentModeTriggerTimeout(void);
		bool HasAllignmentModeTriggerTimeoutExpired(void);
		void SetMaxAllignmentModeTriggerTimeout(void);
		bool HasMaxAllignmentModeTriggerTimeoutExpired(void);
		void SearchForAllignmentModeBarcode(void);
		bool CheckIfAllignmentModeAllowed( void );
		//************************* end the 4800dr allignment mode functionality *****************************



		//*************************Light Bar stuff****************************************
		bool AreWeLightBarEngine(void);
		inline bool GetScanDriverHasFlashLedControl(void){return mbScanDriverHasFlashLedControl;}
		inline void SetScanDriverHasFlashLedControl(bool bControlState){ mbScanDriverHasFlashLedControl = bControlState; }
		//********************************************************************************


		/*-----------------5/30/2002 8:31AM-----------------
		 *GetTriggerMode return the current trigger mode  see above
		 * --------------------------------------------------*/
		inline unsigned GetTriggerMode(void){return mTriggerMode; }

      	inline unsigned int GetTrgModeModFlag(void){return mTriggerModifierFlags;}
		void DebugIt(void);

		void OnChangedConfiguration(void);
		void SetTriggerMode(void);		    // get setting from menu
		inline bool HasSkipDecodeTimeoutExpired(void){return (mSkipDecodeTimeout==0)?true:SkipDecodeTimeout.HasExpired();}
		inline bool HasReReadTimeoutExpired(void){return (mReReadTimeout==0)?true:ReReadTimeout.HasExpired();}
      	inline bool HasReReadTimeout2DExpired(void){return (mReReadTimeout2D==0)?true:ReReadTimeout2D.HasExpired();}
		inline bool HasGoodReadTimeoutExpired(void){return (mGoodReadTimeout==0)?true:GoodReadTimeout.HasExpired();}

		void SetGoodReadTimeout(int timeout);
		void SetGoodReadTimeout(void);   // get setting from menu

		void SetReReadTimeout(int timeout);
		void SetReReadTimeout(void);               // get setting from menu

		void SetReReadTimeout2D(int timeout);
		void SetReReadTimeout2D(void);

		void SetSkipDecodeTimeout(int timeout);
		void SetSkipDecodeTimeout(void);               // get setting from menu

      	void SetLowPwrTimeout(int timeout);
      	void SetLowPwrTimeout(void);
		inline void ResetLowPowerTimeout(void) { pLowPwrTimeout->Reset(); }
		inline bool HasLowPowerTimeoutExpired(void) { return pLowPwrTimeout->HasExpired();}
		inline int GetLowPowerTimeoutRestTime(void) { return pLowPwrTimeout->GetRestTime();}

		void WaitForScanStand(void);
		bool IsWaitingForScanStand(void);
		void GetLowPowPixVal(UINT *GetPixelValue);               //lets get some pixel values for low power scan stand

		void SetSerialTriggerTimeout(int timeout);
		void SetSerialTriggerTimeout(void);                // get setting from menu

		bool IsLowpowerTriggerMode(void);
		inline void SignalScanningFinished(void) {if (this) FinishedScanning=FINISHED_AND_OFF;}
		inline bool IsScanDriverRunning(void) {return (FinishedScanning != FINISHED_AND_OFF);}
		inline int  GetScanDriverState(void) {return (int)FinishedScanning;}
		inline void SetScanDriverState(int iScanDriverState) {FinishedScanning = (SCAN_DRIVER_STATE)iScanDriverState;}

		/*-----------------5/30/2002 9:38AM-----------------
		*IsUnitTriggered returns true if unit is triggered false
		* otherwise.
		* --------------------------------------------------*/
		bool IsUnitTriggered(void);
	   	bool IsHWorSerialTrig(void);

		void UpdateTriggerStatus( void );

		void OpticalTrigger(void);

		inline bool GetBlockDecoderFromExecuting( void )	{ return BlockDecoderFromExecuting; }
		inline void SetBlockDecoderFromExecuting( bool bBlockDecoderFromExecuting )	{ BlockDecoderFromExecuting = bBlockDecoderFromExecuting; }

		void ForceAimerOn(void);		//force aimers on, to override normal aimer state, on indefinitely till power cycle!!!
		void ForceAimerOff(void);		// undo force aimers on
		void ForceLEDsOn(void);	 		//force lights on, to override normal LED state, on indefinitely till power cycle!!!
		void ForceLEDsOff(void);    	// undo force Lights on

		/* these two functions control illumination and start/stop scanning based on trigger mode*/
		void ResetGoodReadTime(void) {GoodReadTimeout.SetExpiration(mGoodReadTimeout);}
		void ResetReReadTime(void) {ReReadTimeout.SetExpiration(mReReadTimeout);}

		inline void ResetReReadTime2D(void) {ReReadTimeout.SetExpiration(mReReadTimeout2D);}

		void ClearReReadTimeouts(void) {ReReadTimeout.SetExpiration(0); ReReadTimeout2D.SetExpiration(0); }
		void ClearGoodReadTimeout(void) {GoodReadTimeout.SetExpiration(0);}

		//Scanning Functions
		void PrepareForScanning(void);
		void FinishScanning(void);
		BOOL GetNextScan(DWORD nOpenCntxt,PBYTE *ppScanBuffer);
		void PreferredSequenceShotgunModeFinishScanning(void);

		inline bool IsCurrentTriggerStateChangePending(void){return mbCurrentTriggerStateChangePending;}

		CTimeoutmS		*pLowPwrTimeout;

		bool				mAimersForcedOn;
		bool				mLightsForcedOn;

		void PrsModesFoundPDF(int bGotOne);
		bool PrsModesPDFdetected(void);

		static int GetShotgunMode( void );

		//************** Heat mitigation functionality ***********************************
		bool HeatMitigationHasDecodeOutput; //in pres mode, set true after a decode, to disable heat mitigation (dose), otherwise to false when triggered   //Heat Mitigation variables - start

		bool AreWeUsbTermId( void );
		void SetTempManualTriggerMode( void );
		void RestoreTriggerModeForUSB( void );
		//ewr04791 Codegate
		bool IsPresentationTypeTrigger(void);
		inline unsigned GetCodeGateMode(void){return mCodeGateMode;}
		//Serial D/E mode.
		inline bool IsSerialDisable(void){return mSerialDisableEnable;}

	private:
		CTrigger (const CTrigger&);            // declared private, but not defined, to prevent copying         
		CTrigger& operator=(const CTrigger&);  // declared private, but not defined, to prevent copying 
		void SetTriggerMode(int triggermode);             //for class use only, such as to set temp manual trigger

		bool bAllowHeatMitigation;	// Set from the product config command HETMIT
		bool bHeatMitigationHasDecoded; //in scan stand, set true after a decode, to enable heat mitigation , otherwise to false
		bool AllowHeatMitigationCoolDown;				//if true, allows heat mitigation for the given HeatMitigationCoolDownTime
		unsigned int HeatMitigationCoolDownTime;
		int GoNumbAfterDecode;		//these two variables were local variables in optical trigger, change of scope for H.M.
		int DelayForPreAiming;         																											//Heat Mitigation variables - end

		DWORD GetLastExposure(void);
		void WaitForHeatMitigationCoolDown( unsigned int uiAllowLowPower = 0, unsigned int uiMultiplier = 1 );
		void SetHeatMitigationScanStand(int iLightsBefore, int iAimerBefore, bool bLightsAfter, bool bAimerAfter, bool bAllowMitigation);
		//************** End Heat mitigation functionality *******************************


		//************** Optical Trigger Variables*****************************
		int BeLessSensitive;
		int NextFrameSensitivity;
		int LedDutyCycle;
		int IdleMe;
		UINT NoChangeClock, DimClock;
		UINT TimeClock;
		int ReducedImaging;
		int RampDownComplete;
		int LastLedDutyCycle;
		int DevalueNextScan;
		int OldDecodeCount;
		int NumberOfTriggersWithoutDecode;
		DWORD NormalImagingDataToRestore[12];
		bool BlockDecoderFromExecuting;
		int ReducedExposure;
		//************** End Optical Trigger Variables *****************************


		//************** optical trigger methods *****************************
		void ResetOpticalTrigger(void);
		void GoToReducedImaging(void);
		void GoToNormalImaging(void);
		void GoToReducedExposureInStreaming(void);
		void GoToNormalExposureInStreaming(void);
		//************** end optical trigger methods *****************************

		//*************************Light Bar stuff****************************************
		bool bWeAreLightBarEngine;
		bool bPresModeLightBarLowLightOn;
		void AdjustAimerCurrentsForStreamingPresentationMode(void);
		bool UpdatePsocRegisters(int Address, unsigned char *Data, int LengthOfData);
		bool ProcessChangeToFromTempManualTrigger(void);
		bool mbScanDriverHasFlashLedControl;
		bool NeedToHandleScanStandSwitch(void);


		struct
		{
			bool bAreSaved;
			BYTE OperMax;
			BYTE Operational;
		} m_SavedAimerCurrentVals;
		//************************************************************************************

		//************** variables for  methods: PrepareForScanning, FinishScanning, GetNextScan *********************
		int bFirstTime;
		int nStartTime;
		bool mbUseBoostMode;
		bool mbCurrentTriggerStateChangePending;
		int mStartingExposureManualTrigger;
		//************** end variables for  methods: PrepareForScanning, FinishScanning, GetNextScan *********************

		void Set_nAimerState(int NEW_nAimerState);
		void _DEBUG_Set_nAimerState(int NEW_nAimerState, unsigned int iCodeLine);      //used to debug aimer state
		void Set_nLightsState(int NEW_nLightsState);
		void _DEBUG_Set_nLightsState(int NEW_nLightsState, unsigned int iCodeLine);    //used to debug lights state
		void Set_AllDutyCycle(int NEW_AllDutyCycle);
		void _DEBUG_Set_AllDutyCycle(int NEW_AllDutyCycle, unsigned int iCodeLine);    //used to debug light Intensity
		void Set_LedDutyCycle(int NEW_LedDutyCycle);
		void _DEBUG_Set_LedDutyCycle(int NEW_LedDutyCycle, unsigned int iCodeLine);    //used to debug Optical trigger light Intensity
		bool NeedToChangeToManual(void);
		int nLightsState;                    //used to be nLights
		int nAimerState;
		int AllDutyCycle;
		bool TriggeredInAutoOrPresMode;

		//*****************Shotgun Mode Stuff***********************
		int	mShotgunMode;
		void SetShotgunMode(int ShotgunSetting);
		// also includes GetShotgunMode( void ) which is public
		//**********************************************************

		int 				PMpix[3645+1];		// size of 1 d image
		int 				PMmin[3645+1];
		int 				PMmax[3645+1];

		CTimeoutmS		GoodReadTimeout;

		CTimeoutmS 		TriggeredTimeout;
		CTimeoutmS		SkipDecodeTimeout;
		CTimeoutmS		ReReadTimeout;
      	CTimeoutmS    	ReReadTimeout2D;
		CTimeoutmS 		PresentationModeTimeout;
		CTimeoutmS 		SerialTriggerTimeout;
		CTimeoutmS		mEmiTimeout;
		CTimeoutmS		pSSTimeout;
		CTimeoutmS		pSSHMTimeout;
		CTimeoutmS		pImagerIdleTimeout;
		CTimeoutmS		pAimerPreDelayTimeout;
		CTimeoutmS		pmScanAimerIdleTimeout;
		CTimeoutmS		pAimerDurationTimout;
		CTimeoutmS 		AllignmentModeTriggerTimeout;
		CTimeoutmS 		MaxAllignmentModeTriggerTimeout;
		CTimeoutmS		USBRecoveryTimeout;

		unsigned			mAllignmentModeTriggerTimeout;
		unsigned			mMaxAllignmentModeTriggerTimeout;
		unsigned			mSerialTriggerTimeout;
		unsigned 			mGoodReadTimeout;
		unsigned 			mReReadTimeout;
      	unsigned       		mReReadTimeout2D;
		unsigned 			mSkipDecodeTimeout;
		unsigned   			mLowPwrTimeout;
		bool				bSerialTrigger;
		bool 				bCurrentTriggerState;                   //used to be named AutoTriggerState
		int					mTriggerMode;
		bool 				mEmiTrigger;
		unsigned			ScanStandState;
		unsigned int 		nSSlights;
		unsigned int		xChk[10];
		int					TriggerThreshold;
		int					ChangeThreshold;
      	unsigned int   		mTriggerModifierFlags;
		INT32 				lightsduty;
		bool 				validscanpending;
		int					imagerpwrdntime;
		int					AimerPreDelay;
		int					mScanAimerIdleTimeout;
		int			   		mScanAimerDuration;
		bool				EmiTriggerState;
		volatile SCAN_DRIVER_STATE	FinishedScanning;
		DWORD				mAimerInImage;

		bool				bGetScanStandExp;                            //find out exposure found by auto exposure
		bool 				bUseScanStandSymbLowPow;                     //if we have found a scan stand symbol, set flag
		int 				iCounter;                     					//counter for low power scan stand
		int					mAutoaimertimeout;
  		int 				iGetStableImages;             				   //flag and counter for got image in scan stand low power
		unsigned int 		nSSlightsStart;
		unsigned int 		nSSlightsStop;

		int iImageWidth;
		int iImageHeight;
		int iFractionImageWidth;                                    //fraction of image width
		int iFractionImageHeight;                                   //fraction of image height
		int iBoxLeftCollPixel;                                     //left collumn of box
		int iBoxCenterCollPixel;                                   //center collumn of box
		int iBoxRightCollPixel;                                    //right collumn of box
		int iBoxUpperRowPixel;                 							//upper row of box
		int iBoxMiddleRowPixel;                 							//middle row of box
		int iBoxLowerRowPixel;                 							//lower row of box

		//trigger specific cell phone settings
		int mScanDriverCellPhone;
		int mPresentationScanDriverCellPhone;
		int	mScanAimerMode;
		int	mCellScanAimerMode;
		int	mCurrentScanAimerMode;
		int mIllOnDuringDecode;

		bool mPreferredMode;
		int mSequenceMode;

		DWORD ssMaxGain,ssGain,ssRestoreGain,ssRestoreGainTemp,ssMaxExp,ssExp,ssRestoreExp,ssRestoreExpTemp,ssExpTimesGain,ssMaxExpTimesGain;

		unsigned int yChk[10];                                 //temporary storage for low power pixel test
		int mLastTriggerMode;
		int mSavedTriggerMode;
		bool mSavedTriggerModeNotSet;

		int CountEdges(int  P1x, int P2x,unsigned char * pImg);

		enum SCANSTANDSTATES
		{
			WAITFORSCANSTAND,
			GOTSCANSTAND,
			SCANSTANDTRIGGERED,
			SCANSTANDUNTRIGGERED,
			SCANSTANDLOWPOWER,
			SCANSTANDLOWPOWERSTART
		};

		// this is used to notify drivers that we just un/triggered (HHLC)
		enum IoControlStates_t
		{
			IOC_NONE=0,
			IOC_TRIGGERED,
			IOC_UNTRIGGERED
		};

		IoControlStates_t	m_IoControlState;
		MT_Mutex_t TriggerMutex;

		bool m_PrsModesPDFdetected;
		bool mUseSSSwitch;
		bool mBlockTempManualOnTriggerChange;

		bool mCodeGateMode;
		bool mSerialDisableEnable; 	
};

extern CTrigger *pTrigger;


#endif
