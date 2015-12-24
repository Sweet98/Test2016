#ifndef SCAN_DRIVER_HW_LAYER_H_D70256A1_FE46_46E2_8F58_2E95FA005175
#define SCAN_DRIVER_HW_LAYER_H_D70256A1_FE46_46E2_8F58_2E95FA005175

#define SDHW_ERR_NONE				0           //added for testflex
#define SDHW_ERR_NOT_IMPLEMENTED	1           //added for testflex
#define SDHW_ERR_MISC				2           //added for testflex

struct HardwareLayerInterface
{
	static DWORD dummy_zero(DWORD) { return 0; }
	static DWORD dummy_zero() { return 0; }
	HardwareLayerInterface()
		: InitiateCapture(InitCapture),
		  ResetScanTransfer(ResetTransferHardware),
		  LightsLowPowerMode(dummy_zero),
		  ImagerIdlePowerLevel(dummy_zero),
		  IlluminationOn(IllumOn),
		  IlluminationOff(IllumOff),
		  IlluminationOffTimer(IllumOffTimer),
		  AimerOn(AimOn),
		  AimerOff(AimOff),
		  AimerConfig(AimConfig),
		  IlluminationConfig(IllumConfig)
		{
		}

	DWORD (*ImagerPowerOn)(DWORD);
	DWORD (*ImagerPowerOff)(DWORD);
	DWORD (*GetScanWidth)(void);
	DWORD (*GetScanHeight)(void);
	BOOL (*GetImagerProperties)(const DWORD * pBuffIn, DWORD * pBuffOut,
								DWORD dwLen  );
	DWORD (*RegisterVsyncNotification)(void (*pf)());
	DWORD (*InitializeVsyncProcessing)(void);
	DWORD (*RegisterImageLineDataNotification)(void (*pf)());
	DWORD (*DeinitializeVsyncProcessing)(void);
	DWORD (*InitializeImageLineDataProcessing)(void);
	DWORD (*DeinitializeImageLineDataProcessing)(void);
	DWORD (*RegisterSofPsocWriteNotification)(void (*pf)());


	DWORD (*GetFrequencyPerFrameOfLineDataCallback)(void);

	DWORD (*BindPhysicalMemoryToVirtualMemory)(DWORD hPhysical,void *pVirtual);
	DWORD (*UnbindPhysicalMemoryFromVirtualMemory)(DWORD hPhysical,void *pVirtual);
	DWORD (*FreePhysicalScanSpace)(DWORD hPhysical);
	DWORD (*AllocatePhysicalScanSpace)(void);

	DWORD (*InitCapture)(DWORD, DWORD dPointerPassed );
	DWORD (*&InitiateCapture)(DWORD, DWORD dPointerPassed );

	BOOL  (*LoadCaptureInfo)(void);

	void  (*ResetTransferHardware)(void);
	void  (*&ResetScanTransfer)(void);

	DWORD (*CaptureInitiated)(void);
	DWORD (*EnableVsyncProcessing)(void);
	DWORD (*DisableVsyncProcessing)(void);

	DWORD (*EnableImageLineDataProcessing)(void);
	DWORD (*DisableImageLineDataProcessing)(void);

	DWORD (*GetNumberOfValidRows)(void);
	BOOL IsPowerOffAndSuspendSupported()
		{ return ImagerPowerOffAndSuspend != 0; }
	DWORD (*ImagerPowerOffAndSuspend)(DWORD);

	bool IsPowerOnAndResumeSupported()
		{ return ImagerPowerOnAndResume != 0; }
	DWORD (*ImagerPowerOnAndResume)(DWORD);

	DWORD (*LightsLowPowerMode)(void);
	DWORD (*ImagerIdlePowerLevel)(void);

	DWORD (*IllumOn)(void);
	DWORD (*IllumOff)(void);
	DWORD (*IllumOffTimer)(void);
	DWORD (*AimOn)(void);
	DWORD (*AimOff)(void);
	BOOL	(*IllumIsOn)(void);
	BOOL	(*AimIsOn)(void);


	DWORD (*AimConfig)(DWORD);
	DWORD (*IllumConfig)(DWORD);

	DWORD (*&IlluminationOn)(void);
	DWORD (*&IlluminationOff)(void);
	DWORD (*&IlluminationOffTimer)(void);
	DWORD (*&AimerOn)(void);
	DWORD (*&AimerOff)(void);
	DWORD (*&AimerConfig)(DWORD);
	DWORD (*&IlluminationConfig)(DWORD);

//#if defined(OEM_ENGINE)
	BOOL	(*IsPsocSupportedByToken)(void);
//#else
	BOOL	IsPsocSupported()
		{ return ((WriteIIC_PSOC != 0)  && (ReadIIC_PSOC !=0)); }
//#endif

	BOOL  (*WriteIIC_PSOC)(const unsigned char*, unsigned int);
	BOOL  (*ReadIIC_PSOC)(unsigned char*, unsigned int);
	BOOL (*WriteIIC_PSOC_w_ADD)(unsigned char , const unsigned char * ,unsigned int  );


	void  (*WaitMilliseconds)(DWORD);
	DWORD (*EngineCurrentlyPowered)(void);
	BOOL  (*ImagerPowerUp)(void);
	BOOL  (*ImagerPowerDown)(void);

	DWORD  (*SetImagerModeToGrayscale)(void);      //test flex
	DWORD  (*UnSetImagerModeToGrayscale)(void);    //test flex

	BOOL  (*SynchronizedPowerDown)(void); // even though not 'common' established here as needed differentiator in code

};

#endif // #ifndef SCAN_DRIVER_HW_LAYER_H_D70256A1_FE46_46E2_8F58_2E95FA005175







