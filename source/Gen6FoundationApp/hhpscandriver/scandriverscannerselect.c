/*-------------------------------------------------------------------

 *-------------------------------------------------------------------
 * ScanDriverScannerSelect.c - This is the source file for the
 *   software which will select which scanner/imager is connected and
 *   configure the hardware/software accordingly.  This file also
 *   contains global function variables which will be linked to the
 *   functions associated with the appropriate scanner/imager
 *-------------------------------------------------------------------
 * $RCSfile: scandriverscannerselect.c $
 * $Revision: 1.77.1.21 $
 * $Date: 2011/05/25 16:28:49EDT $
 *-------------------------------------------------------------------*/
#ifdef _WIN32_WCE

#include <windows.h>
//#include <pkfuncs.h>

extern "C" HINSTANCE LoadDriver(LPCWSTR lpszFileName);

#else // DBALL
#include <stdio.h>
#include <string.h>
#include "language.h"
#include "Hi704A.h"//Ken confused: Is it to say the Hi704A.c is part of Scan driver??? But other files such as psoc use Hi704A.c 's function too. How to make a building block?
#include "db_chrio.h"
#include "HardwareToken.h" 

#endif

#include "ScanDriverOpenContext.h"
#include "ScanDriverScannerSelect.h"
#include "ScanDriverScanManager.h"
#include "ScanDriverLightControl.h"
#include "ScanDriverGlobalConfig.h"
#ifdef HI2D_T
#include "scandriverHi704A.h"//ken fixme for HI2D KENTODO
#else
#endif
#include "ScanDriverDebug.h"

static DWORD dummy_zero()                                { return 0; }
static DWORD dummy_zero(DWORD)                           { return 0; }
static DWORD dummy_zero(ScanDriverOpenContext_t *)                          { return 0; }
static DWORD dummy_one()                                 { return 1; }
static BOOL  dummy_FALSE()                               { return FALSE; }
static BOOL  dummy_FALSE(ScanDriverOpenContext_t *, BOOL)                   { return FALSE; }
static BOOL  dummy_FALSE(ScanDriverOpenContext_t *)       { return FALSE; }
static void  dummy_void()                                { }
static void  dummy_void(const ScanDriverOpenContext_t *) { }
//static DWORD ErrNotImplemented() {return SDHW_ERR_NOT_IMPLEMENTED;}         //added for testflex                //lmc

#ifdef DOXYGEN
/* Defines of global function pointers used when we attach for a specific
 * scanner */
static BOOL HHPSD_ScanEngineFaultOccurred(void){}
static BOOL HHPSD_ScanEngineRestart(ScanDriverOpenContext_t *){}
static BOOL HHPSD_ResetEngine(void){}
static BOOL HHPSD_InitializeScanEngine(ScanDriverOpenContext_t *,BOOL){}
static void HHPSD_ScanEngineStart(void){}
static BOOL HHPSD_DeinitializeScanEngine(ScanDriverOpenContext_t *,BOOL){}
static BOOL HHPSD_ScanEngineSaveStateAndPowerDown(void){}
static BOOL HHPSD_ScanEnginePowerUpAndRestoreState(void){}
static DWORD HHPSD_ScanEnginePowerUpAndRestoreRegs(void){}
static DWORD HHPSD_SetDefaultScannerOptions(ScanDriverOpenContext_t *){}

#ifdef _WIN32_WCE
static DWORD HHPSD_GetHardwareDllRevision(void){}
static void  HHPSD_UnloadHWLayer(void){}
BOOL  HHPSD_InitHWLayer(void){}
#endif

DWORD HHPSD_GetPowerOffIdleMode(void){}
DWORD HHPSD_ScanEngineWaitMs(DWORD){}
DWORD HHPSD_ScanEnginePowerDown(void){}


void HHPSD_ScanManagerAutoExpLineSample(void *pImage,DWORD nRowsComplete){}
void HHPSD_ScanManagerAutoExpCalc(void){}

/*#if defined(OEM_ENGINE)
void HHPSD_ScanManagerAutoPSOCCurrentCalc(void){};
#endif*/

BOOL HHPSD_ScanManagerImageIsAcceptable(ScanDriverLastScanInfo_t *pStats){}
void HHPSD_ScanManagerFrameEventAutoExposure(void){}
DWORD HHPSD_ScanManagerEngineSaysOkToCaptureNextFrame(void){}
void HHPSD_ExposureSettingsUpdate(ScanDriverOpenContext_t *pOptions){}
void HHPSD_LightSettingsUpdate(const ScanDriverOpenContext_t *pOptions){}

#else	//else ifdef DOXYGEN

/* Defines of global function pointers used when we attach for a specific
 * scanner */
static BOOL (*HHPSD_ScanEngineFaultOccurred)(void)			= dummy_FALSE;
static BOOL (*HHPSD_ScanEngineRestart)(ScanDriverOpenContext_t *)				= dummy_FALSE;
static BOOL (*HHPSD_ResetEngine)(void)							= dummy_FALSE;
static BOOL (*HHPSD_InitializeScanEngine)(ScanDriverOpenContext_t *,BOOL)            = dummy_FALSE;
static void (*HHPSD_ScanEngineStart)(void)					= dummy_void;
static BOOL (*HHPSD_DeinitializeScanEngine)(ScanDriverOpenContext_t *,BOOL)          = dummy_FALSE;
static BOOL (*HHPSD_ScanEngineSaveStateAndPowerDown)(void)	= dummy_FALSE;
static BOOL (*HHPSD_ScanEnginePowerUpAndRestoreState)(void)	= dummy_FALSE;
static DWORD (*HHPSD_ScanEnginePowerUpAndRestoreRegs)(void)    = dummy_zero;
static DWORD (*HHPSD_SetDefaultScannerOptions)(ScanDriverOpenContext_t *)			= dummy_zero;
static void  (*HHPSD_FrameBoundary)()			= dummy_void;

#ifdef _WIN32_WCE
static DWORD (*HHPSD_GetHardwareDllRevision)(void)				= dummy_zero;
static void  (*HHPSD_UnloadHWLayer)(void) = dummy_void;
BOOL  (*HHPSD_InitHWLayer)(void)                 = 0;
#endif

DWORD (*HHPSD_GetPowerOffIdleMode)(void) = dummy_zero;
DWORD (*HHPSD_ScanEngineWaitMs)(DWORD) = dummy_zero;
DWORD (*HHPSD_ScanEnginePowerDown)(void) = dummy_zero;

void (*HHPSD_ScanManagerAutoExpLineSample)(void *pImage,DWORD nRowsComplete);
void (*HHPSD_ScanManagerAutoExpCalc)(void);

/*#if defined(OEM_ENGINE)
void (*HHPSD_ScanManagerAutoPSOCCurrentCalc)(void);
#endif*/

BOOL (*HHPSD_ScanManagerImageIsAcceptable)(ScanDriverLastScanInfo_t *pStats);
void (*HHPSD_ScanManagerFrameEventAutoExposure)(void) = dummy_void;
DWORD (*HHPSD_ScanManagerEngineSaysOkToCaptureNextFrame)(void);
void (*HHPSD_ExposureSettingsUpdate)(ScanDriverOpenContext_t *pOptions);
void (*HHPSD_LightSettingsUpdate)(const ScanDriverOpenContext_t *pOptions) = dummy_void;

#endif	//end ifdef DOXYGEN


class HHPSD_Transition : public HHPSD_interface
{
public:
	virtual void ScanManagerAutoExpLineSample(void *pImage,DWORD nRowsComplete)
		{ HHPSD_ScanManagerAutoExpLineSample(pImage, nRowsComplete); }
	virtual BOOL ScanManagerImageIsAcceptable(ScanDriverLastScanInfo_t *pStats)
		{ return HHPSD_ScanManagerImageIsAcceptable(pStats); }
	virtual DWORD ScanManagerEngineSaysOkToCaptureNextFrame(void)
		{ return HHPSD_ScanManagerEngineSaysOkToCaptureNextFrame(); }
	virtual DWORD ScanEngineFaultOccurred(void)
		{ return HHPSD_ScanEngineFaultOccurred(); }
	virtual void ScanManagerAutoExpCalc(void)
		{ HHPSD_ScanManagerAutoExpCalc(); }

/*#if defined(OEM_ENGINE)
	virtual void ScanManagerAutoPSOCCurrentCalc(void)
		{ HHPSD_ScanManagerAutoPSOCCurrentCalc();  }
#endif*/

	virtual DWORD ScanEngineRestart(ScanDriverOpenContext_t * p)
		{ return HHPSD_ScanEngineRestart(p); }
	virtual void ScanManagerFrameEventAutoExposure(void)
		{ HHPSD_ScanManagerFrameEventAutoExposure(); }
	virtual void ExposureSettingsUpdate(ScanDriverOpenContext_t *pOptions)
		{ HHPSD_ExposureSettingsUpdate(pOptions); }
	virtual void LightSettingsUpdate(const ScanDriverOpenContext_t *pOptions)
		{ HHPSD_LightSettingsUpdate(pOptions); }
	virtual DWORD ResetEngine(void)
		{ return HHPSD_ResetEngine(); }
	virtual DWORD InitializeScanEngine(ScanDriverOpenContext_t * p, BOOL b)
		{ return HHPSD_InitializeScanEngine(p, b); }
	virtual void  ScanEngineStart(void)
		{ HHPSD_ScanEngineStart(); }
	virtual DWORD DeinitializeScanEngine(ScanDriverOpenContext_t * p, BOOL b)
		{ return HHPSD_DeinitializeScanEngine(p, b); }
	virtual DWORD ScanEngineSaveStateAndPowerDown(void)
		{ return HHPSD_ScanEngineSaveStateAndPowerDown(); }
	virtual DWORD ScanEnginePowerUpAndRestoreState(void)
		{ return HHPSD_ScanEnginePowerUpAndRestoreState(); }
	virtual DWORD ScanEnginePowerUpAndRestoreRegs(void)
		{ return HHPSD_ScanEnginePowerUpAndRestoreRegs(); }
	virtual DWORD SetDefaultScannerOptions(ScanDriverOpenContext_t * p)
		{ return HHPSD_SetDefaultScannerOptions(p); }
	virtual void FrameBoundary()
	    { return HHPSD_FrameBoundary(); }
};

static HHPSD_Transition hhpsd_transition;
HHPSD_interface * p_hhpsd_interface = &hhpsd_transition;

HardwareLayerInterface * p_hardware_layer_interface;
//ICMediaHardwareLayerInterface * p_icmedia_hardware_layer_interface;                      //lmc
//OV7120HardwareLayerInterface * p_ov7120_hardware_layer_interface;                        //lmc
//MT9V022HardwareLayerInterface *p_mt9v022_hardware_layer_interface;                      //lmc
#ifdef HI2D_T
#ifdef BYD_SENSOR
BYDHardwareLayerInterface *p_BYD_hardware_layer_interface;
#else
Hi704AHardwareLayerInterface *p_Hi704A_hardware_layer_interface;
#endif	//end #ifdef BYD_SENSOR
#else
JadeHardwareLayerInterface *p_jade_hardware_layer_interface;
#endif
//DWORD (*fGetNumberOfValidPixelsToshiba)(void);                                         //lmc
//DWORD (*fChangeExposureToshiba)(DWORD);                                                //lmc
//DWORD	(*fImagerPowerUpToshiba)(void);
//DWORD	(*fImagerPowerDownToshiba)(void);
//DWORD (*fGetReferenceBlackValueToshiba)(void);                                         //lmc
//DWORD (*fSetDCOffsetToshiba)(DWORD val);                                               //lmc
//DWORD (*fRequestAimToshiba)(DWORD nState);                                             //lmc
//DWORD (*fRequestIllumToshiba)(DWORD nState);                                           //lmc
//DWORD (*fGetNumberOfTotalLineCallbacks)(void); // we know that HHPSD_GetFrequencyPerFrameOfLineDataCallback is only the ones ending on a row.     //lmc

//BOOL	(*fReadIIC_STM_VC700)(unsigned char, unsigned char *, unsigned);                //lmc
//BOOL	(*fWriteIIC_STM_VC700)(unsigned char, const unsigned char *, unsigned);         //lmc
//BOOL	(*fImagerPowerUpSTM_VC700)(void);
//BOOL	(*fImagerPowerDownSTM_VC700)(void);
//void	(*fWaitMillisecondsSTM_VC700)(DWORD);
//DWORD	(*fGetRegisterSettingsForHHPExposureControlSTM_VC700)(void *);                    //lmc
//DWORD (*fGetRegisterSettingsForHHPExposureControlSTM_VC602)(void *);                      //lmc
//DWORD  (*fVC700EngineCurrentlyPowered)(void);

/* HW DLL revision 2 assumes poweron/poweroff functionality, and lights control */
#define HHPSD_HWDLL_REVISION_2	2

static void initialize()
{
	p_hardware_layer_interface         = new HardwareLayerInterface();
	//p_icmedia_hardware_layer_interface = new ICMediaHardwareLayerInterface();               //lmc
	//p_ov7120_hardware_layer_interface  = new OV7120HardwareLayerInterface();                 //lmc
	//p_mt9v022_hardware_layer_interface = new MT9V022HardwareLayerInterface();                //lmc
#ifdef HI2D_T
#ifdef BYD_SENSOR
	p_BYD_hardware_layer_interface    = new BYDHardwareLayerInterface();	
#else
	p_Hi704A_hardware_layer_interface    = new Hi704AHardwareLayerInterface();//
#endif //end #ifdef BYD_SENSOR
#else
	p_jade_hardware_layer_interface    = new JadeHardwareLayerInterface();
#endif
}


#ifdef _WIN32_WCE		//*****************************************************************************


static HINSTANCE hLib;
static BOOL  dummy_FALSE(const DWORD *, DWORD *, DWORD ) { return FALSE; }


BOOL AutoDetectScanEngine(void) {
	initialize();

	p_hhpsd_interface = new HHPSD_Transition();
	p_hhpsd_interface->setLightController(new HW_light_control_primitive());

	BOOL bStatus = FALSE;
	DWORD nDllRevision = 0;
	/* If the DLL exists out there somewhere, then we prove it by trying to load it.*/
	hLib = LoadDriver(TEXT("HHPOV7120HW.DLL"));
	if (hLib != NULL) {
		bStatus = TRUE;
		DBG_OUTPUT("---AutoDetectScanEngine-Found HHPOV7120HW.DLL\n");
		HHPSD_GetHardwareDllRevision = (DWORD (*)(void))GetProcAddress(hLib,TEXT("GetHardwareDllRevisionOV7120"));
		if (HHPSD_GetHardwareDllRevision == NULL) bStatus = FALSE;
		else nDllRevision = HHPSD_GetHardwareDllRevision();
		HWLAYER_OV7120.ReadIIC = (BOOL(*)(unsigned char,unsigned char *,unsigned))GetProcAddress(hLib,TEXT("ReadIIC_OV7120"));
		if (HWLAYER_OV7120.ReadIIC == NULL) bStatus = FALSE;
		HWLAYER.ImagerPowerUp = (BOOL(*)(void))GetProcAddress(hLib,TEXT("ImagerPowerUpOV7120"));
		if (HWLAYER.ImagerPowerUp == NULL) bStatus = FALSE;
		HWLAYER.ImagerPowerDown = (BOOL(*)(void))GetProcAddress(hLib,TEXT("ImagerPowerDownOV7120"));
		if (HWLAYER.ImagerPowerDown == NULL) bStatus = FALSE;
		HWLAYER.WaitMilliseconds = (void(*)(DWORD))GetProcAddress(hLib,TEXT("WaitMillisecondsOV7120"));
		if (HWLAYER.WaitMilliseconds == NULL) bStatus = FALSE;
		HWLAYER.EngineCurrentlyPowered					= (DWORD(*)(void))GetProcAddress(hLib,TEXT("ImagerIsPoweredOV7120"));
		if (HWLAYER.EngineCurrentlyPowered == NULL) bStatus = FALSE;
		if (nDllRevision >= HHPSD_HWDLL_REVISION_2) {
            HWLAYER.ImagerPowerOn = (DWORD(*)(DWORD))GetProcAddress(hLib,TEXT("ImagerPowerOnOV7120"));
            // If you un-comment the next line you need to add support in scandriverov7120.c and the hardware dll
            HWLAYER.ImagerPowerOnAndResume = (DWORD(*)(DWORD))GetProcAddress(hLib,TEXT("ImagerPowerOnAndResumeOV7120"));
            HWLAYER.ImagerPowerOnAndResume = NULL;
            if (HWLAYER.ImagerPowerOn == NULL && HWLAYER.ImagerPowerOnAndResume == NULL) bStatus = FALSE;

            HWLAYER.ImagerPowerOff = (DWORD(*)(DWORD))GetProcAddress(hLib,TEXT("ImagerPowerOffOV7120"));
            // If you un-comment the next line you need to add support in scandriverov7120.c and the hardware dll
            HWLAYER.ImagerPowerOffAndSuspend = (DWORD(*)(DWORD))GetProcAddress(hLib,TEXT("ImagerPowerOffAndSuspendOV7120"));
            HWLAYER.ImagerPowerOffAndSuspend = NULL;
            if (HWLAYER.ImagerPowerOff == NULL && HWLAYER.ImagerPowerOffAndSuspend == NULL) bStatus = FALSE;

            // If either one of these functions are exported, then both must be
            // exported.
            if (!HWLAYER.ImagerPowerOffAndSuspend != !HWLAYER.ImagerPowerOnAndResume)
                bStatus = FALSE;

			HWLAYER.WriteIIC_PSOC = NULL;
			HWLAYER.ReadIIC_PSOC = NULL;
			HWLAYER.WriteIIC_PSOC_w_ADD = NULL;


      }
      HWLAYER.SynchronizedPowerDown = NULL; 	// necessary differentiator
		HWLAYER.SetImagerModeToGrayscale = ErrNotImplemented;		//added for testflex
		HWLAYER.UnSetImagerModeToGrayscale = ErrNotImplemented;	//added for testflex

		if (bStatus == TRUE) {
			/* Now we use find out if the scanner is an OV7120 imager.  If it is
			 * then we set up our driver to work with one. */
			if (ScanEngineIsOV7120()) {
				DBG_OUTPUT("---AutoDetectScanEngine-Found OV7120 Imager!!\n");
				/* Overload the DLL functions specific to the camera....*/
				HWLAYER_OV7120.WriteIIC = (BOOL(*)(unsigned char, const unsigned char *, unsigned))GetProcAddress(hLib,TEXT("WriteIIC_OV7120"));
				if (HWLAYER_OV7120.WriteIIC == NULL) bStatus = FALSE;
				HWLAYER_OV7120.InDownTime = (BOOL(*)(void))GetProcAddress(hLib,TEXT("InDownTimeOV7120"));
				if (HWLAYER_OV7120.InDownTime == NULL) bStatus = FALSE;
				HWLAYER_OV7120.GetRegisterSettingsForHHPExposureControl = (DWORD(*)(const OV7120RegisterEntry_t**))GetProcAddress(hLib,TEXT("GetRegisterSettingsForHHPExposureControlOV7120"));
				if (HWLAYER_OV7120.GetRegisterSettingsForHHPExposureControl == NULL) bStatus = FALSE;
				HWLAYER_OV7120.GetRegisterSettingsForOnChipAutoExposure = (DWORD(*)(const OV7120RegisterEntry_t**))GetProcAddress(hLib,TEXT("GetRegisterSettingsForOnChipAutoExposureOV7120"));
				if (HWLAYER_OV7120.GetRegisterSettingsForOnChipAutoExposure == NULL) bStatus = FALSE;
				HWLAYER_OV7120.AimIsOn = (BOOL(*)(void))GetProcAddress(hLib,TEXT("AimIsOnOV7120"));
				if (HWLAYER_OV7120.AimIsOn == NULL) bStatus = FALSE;
				HWLAYER_OV7120.IllumIsOn = (BOOL(*)(void))GetProcAddress(hLib,TEXT("IllumIsOnOV7120"));
				if (HWLAYER_OV7120.IllumIsOn == NULL) bStatus = FALSE;
				/* ...then the DLL functions which are generic to the driver... */
				HWLAYER.GetScanWidth								= (DWORD (*)(void))GetProcAddress(hLib,TEXT("GetScanWidthOV7120"));
				if (HWLAYER.GetScanWidth == NULL) bStatus = FALSE;
				HWLAYER.GetScanHeight								= (DWORD (*)(void))GetProcAddress(hLib,TEXT("GetScanHeightOV7120"));
				if (HWLAYER.GetScanHeight == NULL) bStatus = FALSE;
				HWLAYER.AllocatePhysicalScanSpace					= (DWORD (*)(void))GetProcAddress(hLib,TEXT("AllocatePhysicalScanSpaceOV7120"));
				if (HWLAYER.AllocatePhysicalScanSpace == NULL) bStatus = FALSE;
				HWLAYER.FreePhysicalScanSpace						= (DWORD(*)(DWORD))GetProcAddress(hLib,TEXT("FreePhysicalScanSpaceOV7120"));
				if (HWLAYER.FreePhysicalScanSpace == NULL) bStatus = FALSE;
				HWLAYER.BindPhysicalMemoryToVirtualMemory			= (DWORD(*)(DWORD,void *))GetProcAddress(hLib,TEXT("BindPhysicalMemoryToVirtualMemoryOV7120"));
				if (HWLAYER.BindPhysicalMemoryToVirtualMemory == NULL) bStatus = FALSE;
				HWLAYER.UnbindPhysicalMemoryFromVirtualMemory		= (DWORD(*)(DWORD,void *))GetProcAddress(hLib,TEXT("UnbindPhysicalMemoryFromVirtualMemoryOV7120"));
				if (HWLAYER.UnbindPhysicalMemoryFromVirtualMemory == NULL) bStatus = FALSE;
//				HWLAYER.EngineCurrentlyPowered					= (DWORD(*)(void))GetProcAddress(hLib,TEXT("ImagerIsPoweredOV7120"));
//				if (HWLAYER.EngineCurrentlyPowered == NULL) bStatus = FALSE;
				HWLAYER.IlluminationOn							= (DWORD(*)(void))GetProcAddress(hLib,TEXT("IllumOnOV7120"));
				if (HWLAYER.IllumOn == NULL) bStatus = FALSE;
				HWLAYER.IllumOff							= (DWORD(*)(void))GetProcAddress(hLib,TEXT("IllumOffOV7120"));
				if (HWLAYER.IllumOff == NULL) bStatus = FALSE;
				HWLAYER.AimOn									= (DWORD(*)(void))GetProcAddress(hLib,TEXT("AimOnOV7120"));
				if (HWLAYER.AimOn == NULL) bStatus = FALSE;
				HWLAYER.AimOff									= (DWORD(*)(void))GetProcAddress(hLib,TEXT("AimOffOV7120"));
				if (HWLAYER.AimOff == NULL) bStatus = FALSE;
				HWLAYER.EnableVsyncProcessing						= (DWORD(*)(void))GetProcAddress(hLib,TEXT("EnableVsyncProcessingOV7120"));
				if (HWLAYER.EnableVsyncProcessing == NULL) bStatus = FALSE;
				HWLAYER.DisableVsyncProcessing					= (DWORD(*)(void))GetProcAddress(hLib,TEXT("DisableVsyncProcessingOV7120"));
				if (HWLAYER.DisableVsyncProcessing == NULL) bStatus = FALSE;
				HWLAYER.EnableImageLineDataProcessing				= (DWORD(*)(void))GetProcAddress(hLib,TEXT("EnableImageLineDataProcessingOV7120"));
				if (HWLAYER.EnableImageLineDataProcessing == NULL) bStatus = FALSE;
				HWLAYER.DisableImageLineDataProcessing			= (DWORD(*)(void))GetProcAddress(hLib,TEXT("DisableImageLineDataProcessingOV7120"));
				if (HWLAYER.DisableImageLineDataProcessing == NULL) bStatus = FALSE;
				HWLAYER.InitCapture							= (DWORD(*)(DWORD))GetProcAddress(hLib,TEXT("InitCaptureOV7120"));
				if (HWLAYER.InitCapture == NULL) bStatus = FALSE;
				HWLAYER.LoadCaptureInfo												= dummy_FALSE;
				HWLAYER.RegisterVsyncNotification					= (DWORD (*)(void (*)()))GetProcAddress(hLib,TEXT("RegisterVsyncNotificationOV7120"));
				if (HWLAYER.RegisterVsyncNotification == NULL) bStatus = FALSE;
				HWLAYER.InitializeVsyncProcessing					= (DWORD(*)(void))GetProcAddress(hLib,TEXT("InitializeVsyncProcessingOV7120"));
				if (HWLAYER.InitializeVsyncProcessing == NULL) bStatus = FALSE;
				HWLAYER.RegisterImageLineDataNotification			= (DWORD (*)(void (*)()))GetProcAddress(hLib,TEXT("RegisterImageLineDataNotificationOV7120"));
				if (HWLAYER.RegisterImageLineDataNotification == NULL) bStatus = FALSE;
				HWLAYER.InitializeImageLineDataProcessing			= (DWORD(*)(void))GetProcAddress(hLib,TEXT("InitializeImageLineDataProcessingOV7120"));
				if (HWLAYER.InitializeImageLineDataProcessing == NULL) bStatus = FALSE;
				HWLAYER.DeinitializeVsyncProcessing				= (DWORD(*)(void))GetProcAddress(hLib,TEXT("DeinitializeVsyncProcessingOV7120"));
				if (HWLAYER.DeinitializeVsyncProcessing == NULL) bStatus = FALSE;
				HWLAYER.DeinitializeImageLineDataProcessing		= (DWORD(*)(void))GetProcAddress(hLib,TEXT("DeinitializeImageLineDataProcessingOV7120"));
				if (HWLAYER.DeinitializeImageLineDataProcessing == NULL) bStatus = FALSE;
				HWLAYER.GetFrequencyPerFrameOfLineDataCallback			= (DWORD(*)(void))GetProcAddress(hLib,TEXT("GetFrequencyPerFrameOfLineDataCallbackOV7120"));
				if (HWLAYER.GetFrequencyPerFrameOfLineDataCallback == NULL) bStatus = FALSE;
				HWLAYER.GetNumberOfValidRows						= (DWORD(*)(void))GetProcAddress(hLib,TEXT("GetNumberOfValidRowsOV7120"));
				if (HWLAYER.GetNumberOfValidRows == NULL) bStatus = FALSE;
				HWLAYER.ResetTransferHardware							= (void(*)(void))GetProcAddress(hLib,TEXT("ResetTransferHardwareOV7120"));
				if (HWLAYER.ResetTransferHardware == NULL) bStatus = FALSE;
                HWLAYER.CaptureInitiated                          = (DWORD(*)(void))GetProcAddress(hLib,TEXT("CaptureInitiatedOV7120"));
                if (HWLAYER.CaptureInitiated == NULL) { HWLAYER.CaptureInitiated = dummy_one;}

				/* ...then the scanner specific functions which are overloaded within the driver itself */
				HHPSD_InitializeScanEngine						= InitializeScannerOV7120;
				HHPSD_DeinitializeScanEngine					= DeinitializeScannerOV7120;
				HHPSD_SetDefaultScannerOptions					= SetDefaultScannerOptionsOV7120;
				HHPSD_ScanEngineFaultOccurred					= ScanEngineFaultOccurredOV7120;
				HHPSD_ScanEngineRestart							= RestartScannerOV7120;
				HHPSD_ScanEngineSaveStateAndPowerDown			= ScanEngineSaveStateOV7120;
				HHPSD_ScanEnginePowerUpAndRestoreState			= ScanEngineRestoreStateOV7120;
				HHPSD_ExposureSettingsUpdate							= (void (__cdecl *)(ScanDriverOpenContext_t *))ExposureSettingsUpdateOV7120;
                HHPSD_ScanManagerEngineSaysOkToCaptureNextFrame = dummy_one;
			} else {
				bStatus = FALSE;
				DBG_OUTPUT("---AutoDetectScanEngine- Engine is not OV7120\n");
			}
		}
		if (bStatus != TRUE)
			UnloadHardwareLibrary();
	}
	if (bStatus != TRUE) {
		hLib = LoadDriver(TEXT("HHPMT9V022HW.DLL"));
		if (hLib != NULL) {
			p_hardware_layer_interface = p_mt9v022_hardware_layer_interface;
			HHPSD_InitHWLayer = (BOOL (*)(void))GetProcAddress(hLib,TEXT("InitHWLayerMT9V022"));

			// If InitHWLayer() is defined and it returns false, unload the library
			if (HHPSD_InitHWLayer != 0 && HHPSD_InitHWLayer() == FALSE)
			{
				// If HHPSD_InitHWLayer() fails, it should leave the HW layer
				// in a state where no other cleanup is required.  So, we
				// call FreeLibrary() instead of calling the UnloadHWLayer
				// funciton.
				DBG_OUTPUT (" --- AutoDetectScanEngine -- HWLayer faileD!!\r\n");
				FreeLibrary(hLib);
			}
			else
			{
				DBG_OUTPUT("---AutoDetectScanEngine-Found HHPMT9V022HW.DLL\n");
				bStatus = TRUE;
				HHPSD_GetHardwareDllRevision = (DWORD (*)(void))GetProcAddress(hLib,TEXT("GetHardwareDllRevisionMT9V022"));
				if (HHPSD_GetHardwareDllRevision == NULL) bStatus = FALSE;
				else nDllRevision = HHPSD_GetHardwareDllRevision();
				HWLAYER_MT9V022.ReadIIC = (BOOL(*)(unsigned char,unsigned short *,unsigned))GetProcAddress(hLib,TEXT("ReadIIC_MT9V022"));
				if (HWLAYER_MT9V022.ReadIIC == NULL) bStatus = FALSE;
				HWLAYER_MT9V022.WriteIIC = (BOOL(*)(unsigned char,const unsigned short *,unsigned))GetProcAddress(hLib,TEXT("WriteIIC_MT9V022"));
				if (HWLAYER_MT9V022.WriteIIC == NULL) bStatus = FALSE;
				HWLAYER.ImagerPowerUp = (BOOL(*)(void))GetProcAddress(hLib,TEXT("ImagerPowerUpMT9V022"));
				if (HWLAYER.ImagerPowerUp == NULL) bStatus = FALSE;
				HWLAYER.ImagerPowerDown = (BOOL(*)(void))GetProcAddress(hLib,TEXT("ImagerPowerDownMT9V022"));
				if (HWLAYER.ImagerPowerDown == NULL) bStatus = FALSE;
				HWLAYER.WaitMilliseconds = (void(*)(DWORD))GetProcAddress(hLib,TEXT("WaitMillisecondsMT9V022"));
				if (HWLAYER.WaitMilliseconds == NULL) bStatus = FALSE;
				HWLAYER.EngineCurrentlyPowered					= (DWORD(*)(void))GetProcAddress(hLib,TEXT("ImagerIsPoweredMT9V022"));
				if (HWLAYER.EngineCurrentlyPowered == NULL) {DBG_OUTPUT("ImagerIsPowered\r\n");bStatus = FALSE;}

				if (nDllRevision >= HHPSD_HWDLL_REVISION_2) {
					HWLAYER.ImagerPowerOn = (DWORD(*)(DWORD))GetProcAddress(hLib,TEXT("ImagerPowerOnMT9V022"));
					HWLAYER.ImagerPowerOnAndResume = (DWORD(*)(DWORD))GetProcAddress(hLib,TEXT("ImagerPowerOnAndResumeMT9V022"));
					if (HWLAYER.ImagerPowerOn == NULL && HWLAYER.ImagerPowerOnAndResume == NULL) bStatus = FALSE;
					HWLAYER.ImagerPowerOff = (DWORD(*)(DWORD))GetProcAddress(hLib,TEXT("ImagerPowerOffMT9V022"));
					HWLAYER.ImagerPowerOffAndSuspend = (DWORD(*)(DWORD))GetProcAddress(hLib,TEXT("ImagerPowerOffAndSuspendMT9V022"));
					if (HWLAYER.ImagerPowerOff == NULL && HWLAYER.ImagerPowerOffAndSuspend == NULL) bStatus = FALSE;

					// If either one of these functions are exported, then both must be
					// exported.
					if (!HWLAYER.ImagerPowerOffAndSuspend != !HWLAYER.ImagerPowerOnAndResume)
						bStatus = FALSE;
					HWLAYER.WriteIIC_PSOC = (BOOL(*)(const unsigned char *, unsigned))GetProcAddress(hLib,TEXT("WriteIIC_PSOC_MT9V022"));
					HWLAYER.ReadIIC_PSOC = (BOOL(*)(unsigned char *,unsigned))GetProcAddress(hLib,TEXT("ReadIIC_PSOC_MT9V022"));
					HWLAYER.WriteIIC_PSOC_w_ADD = NULL;

					HHPSD_UnloadHWLayer = (void (*)(void))GetProcAddress(hLib,TEXT("UnloadHWLayerMT9V022"));

				}
				// The new powerup sequence requires this to be done before the autodetection routine.
				HWLAYER_MT9V022.InDownTime = (BOOL(*)(void))GetProcAddress(hLib,TEXT("InDownTimeMT9V022"));
				if (HWLAYER_MT9V022.InDownTime == NULL)
				{
					DBG_OUTPUT("fInDownTime\r\n");
					bStatus = FALSE;
				}

				HWLAYER.SynchronizedPowerDown = (BOOL(*)(void))GetProcAddress(hLib,TEXT("SynchronizedPowerDownMT9V022"));
				if (HWLAYER.SynchronizedPowerDown == NULL) {DBG_OUTPUT("SyncPowerDown\r\n");bStatus = FALSE;}
				if (bStatus == TRUE) {
					if (ScanEngineIsMT9V022()) {
						p_hhpsd_interface->setLightController(new HW_light_control_MT9V022());

						DBG_OUTPUT("---AutoDetectScanEngine-Found MT9V022 Imager!!\n");
						HWLAYER_MT9V022.GetRegisterSettingsForHHPExposureControl = (DWORD(*)(const MicronRegisterEntry_t **))GetProcAddress(hLib,TEXT("GetRegisterSettingsForHHPExposureControlMT9V022"));
						if (HWLAYER_MT9V022.GetRegisterSettingsForHHPExposureControl == NULL)
						{DBG_OUTPUT("fGetRegisterSettings\r\n");bStatus = FALSE;}
						HWLAYER_MT9V022.AimIsOn = (BOOL(*)(void))GetProcAddress(hLib,TEXT("AimIsOnMT9V022"));
						if (HWLAYER_MT9V022.AimIsOn == NULL) {DBG_OUTPUT("AimIsOn\r\n");bStatus = FALSE;}
						HWLAYER_MT9V022.IllumIsOn = (BOOL(*)(void))GetProcAddress(hLib,TEXT("IllumIsOnMT9V022"));
						if (HWLAYER_MT9V022.IllumIsOn == NULL) {DBG_OUTPUT("IllumIsOn\r\n");bStatus = FALSE;}
						HWLAYER_MT9V022.WaitForVsync = (BOOL(*)(void))GetProcAddress(hLib,TEXT("WaitForVsyncMT9V022"));
						if (HWLAYER_MT9V022.WaitForVsync == NULL) {DBG_OUTPUT("WaitForVsync\r\n");bStatus = FALSE;}
						HWLAYER_MT9V022.RequestSynchronizedPowerDown = (void(*)(void))GetProcAddress(hLib,TEXT("RequestSynchronizedPowerDownMT9V022"));
						if (HWLAYER_MT9V022.RequestSynchronizedPowerDown == NULL) {DBG_OUTPUT("RequestSyncPowerDown\r\n");bStatus = FALSE;}
						/* ...then the DLL functions which are generic to the driver... */
						HWLAYER.GetScanWidth								= (DWORD (*)(void))GetProcAddress(hLib,TEXT("GetScanWidthMT9V022"));
						if (HWLAYER.GetScanWidth == NULL) {DBG_OUTPUT("GetScanWidth\r\n");bStatus = FALSE;}
						HWLAYER.GetScanHeight								= (DWORD (*)(void))GetProcAddress(hLib,TEXT("GetScanHeightMT9V022"));
						if (HWLAYER.GetScanHeight == NULL) {DBG_OUTPUT("GetScanHeight\r\n");bStatus = FALSE;}
						HWLAYER.AllocatePhysicalScanSpace					= (DWORD (*)(void))GetProcAddress(hLib,TEXT("AllocatePhysicalScanSpaceMT9V022"));
						if (HWLAYER.AllocatePhysicalScanSpace == NULL) {DBG_OUTPUT("Allocate\r\n");bStatus = FALSE;}
						HWLAYER.FreePhysicalScanSpace						= (DWORD(*)(DWORD))GetProcAddress(hLib,TEXT("FreePhysicalScanSpaceMT9V022"));
						if (HWLAYER.FreePhysicalScanSpace == NULL) {DBG_OUTPUT("FreePhysical\r\n");bStatus = FALSE;}
						HWLAYER.BindPhysicalMemoryToVirtualMemory			= (DWORD(*)(DWORD,void *))GetProcAddress(hLib,TEXT("BindPhysicalMemoryToVirtualMemoryMT9V022"));
						if (HWLAYER.BindPhysicalMemoryToVirtualMemory == NULL) {DBG_OUTPUT("Bind\r\n");bStatus = FALSE;}
						HWLAYER.UnbindPhysicalMemoryFromVirtualMemory		= (DWORD(*)(DWORD,void *))GetProcAddress(hLib,TEXT("UnbindPhysicalMemoryFromVirtualMemoryMT9V022"));
						if (HWLAYER.UnbindPhysicalMemoryFromVirtualMemory == NULL) {DBG_OUTPUT("Unbind\r\n");bStatus = FALSE;}
//						HWLAYER.EngineCurrentlyPowered					= (DWORD(*)(void))GetProcAddress(hLib,TEXT("ImagerIsPoweredMT9V022"));
//						if (HWLAYER.EngineCurrentlyPowered == NULL) {DBG_OUTPUT("ImagerIsPowered\r\n");bStatus = FALSE;}
						HWLAYER.IllumOn							= (DWORD(*)(void))GetProcAddress(hLib,TEXT("IllumOnMT9V022"));
						if (HWLAYER.IllumOn == NULL) {DBG_OUTPUT("IllumOn\r\n");bStatus = FALSE;}
						HWLAYER.IllumOff							= (DWORD(*)(void))GetProcAddress(hLib,TEXT("IllumOffMT9V022"));
						if (HWLAYER.IllumOff == NULL) {DBG_OUTPUT("IllumOff\r\n");bStatus = FALSE;}
						HWLAYER.AimOn									= (DWORD(*)(void))GetProcAddress(hLib,TEXT("AimOnMT9V022"));
						if (HWLAYER.AimOn == NULL) {DBG_OUTPUT("AimOn\r\n");bStatus = FALSE;}
						HWLAYER.AimOff									= (DWORD(*)(void))GetProcAddress(hLib,TEXT("AimOffMT9V022"));
						if (HWLAYER.AimOff == NULL) {DBG_OUTPUT("AimOff\r\n");bStatus = FALSE;}
						HWLAYER.EnableVsyncProcessing						= (DWORD(*)(void))GetProcAddress(hLib,TEXT("EnableVsyncProcessingMT9V022"));
						if (HWLAYER.EnableVsyncProcessing == NULL) {DBG_OUTPUT("EnableVsyncProc\r\n");bStatus = FALSE;}
						HWLAYER.DisableVsyncProcessing					= (DWORD(*)(void))GetProcAddress(hLib,TEXT("DisableVsyncProcessingMT9V022"));
						if (HWLAYER.DisableVsyncProcessing == NULL) {DBG_OUTPUT("DisableVsyncProc\r\n");bStatus = FALSE;}
						HWLAYER.EnableImageLineDataProcessing				= (DWORD(*)(void))GetProcAddress(hLib,TEXT("EnableImageLineDataProcessingMT9V022"));
						if (HWLAYER.EnableImageLineDataProcessing == NULL) {DBG_OUTPUT("EnableImageLine\r\n");bStatus = FALSE;}
						HWLAYER.DisableImageLineDataProcessing			= (DWORD(*)(void))GetProcAddress(hLib,TEXT("DisableImageLineDataProcessingMT9V022"));
						if (HWLAYER.DisableImageLineDataProcessing == NULL) {DBG_OUTPUT("DisableImageLineData\r\n");bStatus = FALSE;}
						HWLAYER.InitCapture							= (DWORD(*)(DWORD))GetProcAddress(hLib,TEXT("InitCaptureMT9V022"));
						if (HWLAYER.InitCapture == NULL) {DBG_OUTPUT("InitCapture\r\n");bStatus = FALSE;}
						HWLAYER.LoadCaptureInfo												= dummy_FALSE;
						HWLAYER.RegisterVsyncNotification					= (DWORD(*)(void (*)()))GetProcAddress(hLib,TEXT("RegisterVsyncNotificationMT9V022"));
						if (HWLAYER.RegisterVsyncNotification == NULL) {DBG_OUTPUT("RegisterVsync\r\n");bStatus = FALSE;}
						HWLAYER.InitializeVsyncProcessing					= (DWORD(*)(void))GetProcAddress(hLib,TEXT("InitializeVsyncProcessingMT9V022"));
						if (HWLAYER.InitializeVsyncProcessing == NULL) {DBG_OUTPUT("InitVsyncProc\r\n");bStatus = FALSE;}
						HWLAYER.RegisterImageLineDataNotification			= (DWORD(*)(void (*pf)()))GetProcAddress(hLib,TEXT("RegisterImageLineDataNotificationMT9V022"));
						if (HWLAYER.RegisterImageLineDataNotification == NULL) {DBG_OUTPUT("RegisterImageLineData\r\n");bStatus = FALSE;}
						HWLAYER.InitializeImageLineDataProcessing			= (DWORD(*)(void))GetProcAddress(hLib,TEXT("InitializeImageLineDataProcessingMT9V022"));
						if (HWLAYER.InitializeImageLineDataProcessing == NULL) {DBG_OUTPUT("InitializeIMageLineData\r\n");bStatus = FALSE;}
						HWLAYER.DeinitializeVsyncProcessing				= (DWORD(*)(void))GetProcAddress(hLib,TEXT("DeinitializeVsyncProcessingMT9V022"));
						if (HWLAYER.DeinitializeVsyncProcessing == NULL) {DBG_OUTPUT("DeinitializeVsyncProc\r\n");bStatus = FALSE;}
						HWLAYER.DeinitializeImageLineDataProcessing		= (DWORD(*)(void))GetProcAddress(hLib,TEXT("DeinitializeImageLineDataProcessingMT9V022"));
						if (HWLAYER.DeinitializeImageLineDataProcessing == NULL) {DBG_OUTPUT("DeinitImageLineData\r\n");bStatus = FALSE;}
						HWLAYER.GetFrequencyPerFrameOfLineDataCallback			= (DWORD(*)(void))GetProcAddress(hLib,TEXT("GetFrequencyPerFrameOfLineDataCallbackMT9V022"));
						if (HWLAYER.GetFrequencyPerFrameOfLineDataCallback == NULL) {DBG_OUTPUT("GetFrequencyPer\r\n");bStatus = FALSE;}
						HWLAYER.GetNumberOfValidRows						= (DWORD(*)(void))GetProcAddress(hLib,TEXT("GetNumberOfValidRowsMT9V022"));
						if (HWLAYER.GetNumberOfValidRows == NULL) {DBG_OUTPUT("GetNumberOfValidRows\r\n");bStatus = FALSE;}
						HWLAYER.ResetTransferHardware							= (void(*)(void))GetProcAddress(hLib,TEXT("ResetTransferHardwareMT9V022"));
						if (HWLAYER.ResetTransferHardware == NULL) {DBG_OUTPUT("ResetTransferHardware\r\n");bStatus = FALSE;}
						HWLAYER.CaptureInitiated                          = (DWORD(*)(void))GetProcAddress(hLib,TEXT("CaptureInitiatedMT9V022"));
						if (HWLAYER.CaptureInitiated == NULL) { HWLAYER.CaptureInitiated = dummy_one;}
						HWLAYER.GetImagerProperties                       = (BOOL (*)(const DWORD *, DWORD *, DWORD))GetProcAddress(hLib,TEXT("GetImagerPropertiesMT9V022"));
						if (HWLAYER.GetImagerProperties == NULL) { HWLAYER.GetImagerProperties = dummy_FALSE; }

						//spd
						// !! Do we need this?
						//HWLAYER.NotifyHdwLayer				= (DWORD (*)(DWORD,void *))GetProcAddress(hLib,TEXT("NotifyHdwLayer"));
						//if (HWLAYER.NotifyHdwLayer == NULL) { /*NKDbgPrintfW(TEXT("NoNotify")); */HWLAYER.NotifyHdwLayer = DwordVoidPtrArgumentDwordReturn_Dummy;; }

						/* ...then the scanner specific functions which are overloaded within the driver itself */
						HHPSD_InitializeScanEngine						= InitializeScanEngineMT9V022;
						HHPSD_DeinitializeScanEngine					= DeinitializeScanEngineMT9V022;
						HHPSD_SetDefaultScannerOptions					= SetDefaultScannerOptionsMT9V022;
						HHPSD_ScanEngineFaultOccurred					= ScanEngineFaultOccurredMT9V022;
						HHPSD_ScanEngineRestart							= ScanEngineRestartMT9V022;
						HHPSD_ScanEngineSaveStateAndPowerDown			= ScanEngineSaveStateAndPowerDownMT9V022;

						HHPSD_ScanEnginePowerUpAndRestoreState			= ScanEnginePowerUpAndRestoreStateMT9V022;
						HHPSD_ScanEnginePowerUpAndRestoreRegs           = ScanEnginePowerUpAndRestoreRegsMT9V022;



						HHPSD_ExposureSettingsUpdate							= ExposureSettingsUpdateMT9V022;
						HHPSD_LightSettingsUpdate							= LightSettingsUpdateMT9V022;
//                    HHPSD_ScanManagerEngineSaysOkToCaptureNextFrame = ScanEngineOkToCaptureNextFrameMT9V022;
						HHPSD_ScanManagerEngineSaysOkToCaptureNextFrame = dummy_one;
// spd added for power control
						HHPSD_ScanEngineWaitMs = ScanEngineWaitMsMT9V022;
						HHPSD_ScanEnginePowerDown = ScanEnginePowerDownMT9V022;
						HHPSD_FrameBoundary = FrameBoundaryMT9V022;

					} else {
						DBG_OUTPUT("---AutoDetectScanEngine- Engine is not MT9V022\n");
						bStatus = FALSE;
					}
				}
				if (bStatus != TRUE)
				{
					DBG_OUTPUT("--- Unloading Libary- bStatus != TRUE\r\n");
					UnloadHardwareLibrary();
				}
			}
        } else {
            DBG_OUTPUT("---AutoDetectScanEngine-Couldn't find HHPMT9V022HW.DLL\n");
        }
    }
	if (bStatus != TRUE) {
		hLib = LoadDriver(TEXT("HHPICMEDIAHW.DLL"));
		if (hLib != NULL) {
			p_hardware_layer_interface = p_icmedia_hardware_layer_interface;
			HHPSD_InitHWLayer = (BOOL (*)(void))GetProcAddress(hLib,TEXT("InitHWLayerICMedia"));

			// If InitHWLayer() is defined and it returns false, unload the library
			if (HHPSD_InitHWLayer != 0 && HHPSD_InitHWLayer() == FALSE)
			{
				// If HHPSD_InitHWLayer() fails, it should leave the HW layer
				// in a state where no other cleanup is required.  So, we
				// call FreeLibrary() instead of calling the UnloadHWLayer
				// funciton.
				FreeLibrary(hLib);
			}
			else
			{
				DBG_OUTPUT("---AutoDetectScanEngine-Found HHPICMEDIAHW.DLL\n");
				bStatus = TRUE;
				HHPSD_GetHardwareDllRevision = (DWORD (*)(void))GetProcAddress(hLib,TEXT("GetHardwareDllRevisionICMedia"));
				if (HHPSD_GetHardwareDllRevision == NULL) bStatus = FALSE;
				else nDllRevision = HHPSD_GetHardwareDllRevision();
				HWLAYER_ICMEDIA.ReadIIC = (BOOL(*)(unsigned char,unsigned char *))GetProcAddress(hLib,TEXT("ReadIIC_ICMedia"));
				if (HWLAYER_ICMEDIA.ReadIIC == NULL) bStatus = FALSE;
				HWLAYER_ICMEDIA.WriteIIC = (BOOL(*)(unsigned char,const unsigned char *))GetProcAddress(hLib,TEXT("WriteIIC_ICMedia"));
				if (HWLAYER_ICMEDIA.WriteIIC == NULL) bStatus = FALSE;
				HWLAYER.ImagerPowerUp = (BOOL(*)(void))GetProcAddress(hLib,TEXT("ImagerPowerUpICMedia"));
				if (HWLAYER.ImagerPowerUp == NULL) bStatus = FALSE;
				HWLAYER.ImagerPowerDown = (BOOL(*)(void))GetProcAddress(hLib,TEXT("ImagerPowerDownICMedia"));
				if (HWLAYER.ImagerPowerDown == NULL) bStatus = FALSE;
				HWLAYER.WaitMilliseconds = (void(*)(DWORD))GetProcAddress(hLib,TEXT("WaitMillisecondsICMedia"));
				if (HWLAYER.WaitMilliseconds == NULL) bStatus = FALSE;
				HWLAYER.EngineCurrentlyPowered					= (DWORD(*)(void))GetProcAddress(hLib,TEXT("ImagerIsPoweredICMedia"));
				if (HWLAYER.EngineCurrentlyPowered == NULL) bStatus = FALSE;
				if (nDllRevision >= HHPSD_HWDLL_REVISION_2) {
					HWLAYER.ImagerPowerOn = (DWORD(*)(DWORD))GetProcAddress(hLib,TEXT("ImagerPowerOnICMedia"));
					HWLAYER.ImagerPowerOnAndResume = (DWORD(*)(DWORD))GetProcAddress(hLib,TEXT("ImagerPowerOnAndResumeICMedia"));
					if (HWLAYER.ImagerPowerOn == NULL && HWLAYER.ImagerPowerOnAndResume == NULL) bStatus = FALSE;
					HWLAYER.ImagerPowerOff = (DWORD(*)(DWORD))GetProcAddress(hLib,TEXT("ImagerPowerOffICMedia"));
					HWLAYER.ImagerPowerOffAndSuspend = (DWORD(*)(DWORD))GetProcAddress(hLib,TEXT("ImagerPowerOffAndSuspendICMedia"));
					if (HWLAYER.ImagerPowerOff == NULL && HWLAYER.ImagerPowerOffAndSuspend == NULL) bStatus = FALSE;

					// If either one of these functions are exported, then both must be
					// exported.
					if (!HWLAYER.ImagerPowerOffAndSuspend != !HWLAYER.ImagerPowerOnAndResume)
						bStatus = FALSE;

					HHPSD_UnloadHWLayer = (void (*)(void))GetProcAddress(hLib,TEXT("UnloadHWLayerICMedia"));

					HWLAYER.WriteIIC_PSOC = NULL;
			   		HWLAYER.ReadIIC_PSOC = NULL;
			   		HWLAYER.WriteIIC_PSOC_w_ADD = NULL;

				}
				HWLAYER.SynchronizedPowerDown = (BOOL(*)(void))GetProcAddress(hLib,TEXT("SynchronizedPowerDownICMedia"));
				if (HWLAYER.SynchronizedPowerDown == NULL) bStatus = FALSE;
				if (bStatus == TRUE) {
					if (ScanEngineIsICMedia()) {
						DBG_OUTPUT("---AutoDetectScanEngine-Found IC Media Imager!!\n");
						HWLAYER_ICMEDIA.InDownTime = (BOOL(*)(void))GetProcAddress(hLib,TEXT("InDownTimeICMedia"));
						if (HWLAYER_ICMEDIA.InDownTime == NULL) bStatus = FALSE;
						HWLAYER_ICMEDIA.GetRegisterSettingsForHHPExposureControl = (DWORD(*)(const ICMediaRegisterEntry_t**))GetProcAddress(hLib,TEXT("GetRegisterSettingsForHHPExposureControlICMedia"));
						if (HWLAYER_ICMEDIA.GetRegisterSettingsForHHPExposureControl == NULL) bStatus = FALSE;
						HWLAYER_ICMEDIA.AimIsOn = (BOOL(*)(void))GetProcAddress(hLib,TEXT("AimIsOnICMedia"));
						if (HWLAYER_ICMEDIA.AimIsOn == NULL) bStatus = FALSE;
						HWLAYER_ICMEDIA.IllumIsOn = (BOOL(*)(void))GetProcAddress(hLib,TEXT("IllumIsOnICMedia"));
						if (HWLAYER_ICMEDIA.IllumIsOn == NULL) bStatus = FALSE;
						HWLAYER_ICMEDIA.WaitForVsync = (BOOL(*)(void))GetProcAddress(hLib,TEXT("WaitForVsyncICMedia"));
						if (HWLAYER_ICMEDIA.WaitForVsync == NULL) bStatus = FALSE;
						HWLAYER_ICMEDIA.RequestSynchronizedPowerDown = (void(*)(void))GetProcAddress(hLib,TEXT("RequestSynchronizedPowerDownICMedia"));
						if (HWLAYER_ICMEDIA.RequestSynchronizedPowerDown == NULL) bStatus = FALSE;
						/* ...then the DLL functions which are generic to the driver... */
						HWLAYER.GetScanWidth								= (DWORD (*)(void))GetProcAddress(hLib,TEXT("GetScanWidthICMedia"));
						if (HWLAYER.GetScanWidth == NULL) bStatus = FALSE;
						HWLAYER.GetScanHeight								= (DWORD (*)(void))GetProcAddress(hLib,TEXT("GetScanHeightICMedia"));
						if (HWLAYER.GetScanHeight == NULL) bStatus = FALSE;
						HWLAYER.AllocatePhysicalScanSpace					= (DWORD (*)(void))GetProcAddress(hLib,TEXT("AllocatePhysicalScanSpaceICMedia"));
						if (HWLAYER.AllocatePhysicalScanSpace == NULL) bStatus = FALSE;
						HWLAYER.FreePhysicalScanSpace						= (DWORD(*)(DWORD))GetProcAddress(hLib,TEXT("FreePhysicalScanSpaceICMedia"));
						if (HWLAYER.FreePhysicalScanSpace == NULL) bStatus = FALSE;
						HWLAYER.BindPhysicalMemoryToVirtualMemory			= (DWORD(*)(DWORD,void *))GetProcAddress(hLib,TEXT("BindPhysicalMemoryToVirtualMemoryICMedia"));
						if (HWLAYER.BindPhysicalMemoryToVirtualMemory == NULL) bStatus = FALSE;
						HWLAYER.UnbindPhysicalMemoryFromVirtualMemory		= (DWORD(*)(DWORD,void *))GetProcAddress(hLib,TEXT("UnbindPhysicalMemoryFromVirtualMemoryICMedia"));
						if (HWLAYER.UnbindPhysicalMemoryFromVirtualMemory == NULL) bStatus = FALSE;
//						HWLAYER.EngineCurrentlyPowered					= (DWORD(*)(void))GetProcAddress(hLib,TEXT("ImagerIsPoweredICMedia"));
//						if (HWLAYER.EngineCurrentlyPowered == NULL) bStatus = FALSE;
						HWLAYER.IllumOn							= (DWORD(*)(void))GetProcAddress(hLib,TEXT("IllumOnICMedia"));
						if (HWLAYER.IllumOn == NULL) bStatus = FALSE;
						HWLAYER.IllumOff							= (DWORD(*)(void))GetProcAddress(hLib,TEXT("IllumOffICMedia"));
						if (HWLAYER.IllumOff == NULL) bStatus = FALSE;
						HWLAYER.AimOn									= (DWORD(*)(void))GetProcAddress(hLib,TEXT("AimOnICMedia"));
						if (HWLAYER.AimOn == NULL) bStatus = FALSE;
						HWLAYER.AimOff									= (DWORD(*)(void))GetProcAddress(hLib,TEXT("AimOffICMedia"));
						if (HWLAYER.AimOff == NULL) bStatus = FALSE;
						HWLAYER.EnableVsyncProcessing						= (DWORD(*)(void))GetProcAddress(hLib,TEXT("EnableVsyncProcessingICMedia"));
						if (HWLAYER.EnableVsyncProcessing == NULL) bStatus = FALSE;
						HWLAYER.DisableVsyncProcessing					= (DWORD(*)(void))GetProcAddress(hLib,TEXT("DisableVsyncProcessingICMedia"));
						if (HWLAYER.DisableVsyncProcessing == NULL) bStatus = FALSE;
						HWLAYER.EnableImageLineDataProcessing				= (DWORD(*)(void))GetProcAddress(hLib,TEXT("EnableImageLineDataProcessingICMedia"));
						if (HWLAYER.EnableImageLineDataProcessing == NULL) bStatus = FALSE;
						HWLAYER.DisableImageLineDataProcessing			= (DWORD(*)(void))GetProcAddress(hLib,TEXT("DisableImageLineDataProcessingICMedia"));
						if (HWLAYER.DisableImageLineDataProcessing == NULL) bStatus = FALSE;
						HWLAYER.InitCapture							= (DWORD(*)(DWORD))GetProcAddress(hLib,TEXT("InitCaptureICMedia"));
						if (HWLAYER.InitCapture == NULL) bStatus = FALSE;
						HWLAYER.LoadCaptureInfo												= dummy_FALSE;
						HWLAYER.RegisterVsyncNotification					= (DWORD (*)(void (*)()))GetProcAddress(hLib,TEXT("RegisterVsyncNotificationICMedia"));
						if (HWLAYER.RegisterVsyncNotification == NULL) bStatus = FALSE;
						HWLAYER.InitializeVsyncProcessing					= (DWORD(*)(void))GetProcAddress(hLib,TEXT("InitializeVsyncProcessingICMedia"));
						if (HWLAYER.InitializeVsyncProcessing == NULL) bStatus = FALSE;
						HWLAYER.RegisterImageLineDataNotification			= (DWORD (*)(void (*)()))GetProcAddress(hLib,TEXT("RegisterImageLineDataNotificationICMedia"));
						if (HWLAYER.RegisterImageLineDataNotification == NULL) bStatus = FALSE;
						HWLAYER.InitializeImageLineDataProcessing			= (DWORD(*)(void))GetProcAddress(hLib,TEXT("InitializeImageLineDataProcessingICMedia"));
						if (HWLAYER.InitializeImageLineDataProcessing == NULL) bStatus = FALSE;
						HWLAYER.DeinitializeVsyncProcessing				= (DWORD(*)(void))GetProcAddress(hLib,TEXT("DeinitializeVsyncProcessingICMedia"));
						if (HWLAYER.DeinitializeVsyncProcessing == NULL) bStatus = FALSE;
						HWLAYER.DeinitializeImageLineDataProcessing		= (DWORD(*)(void))GetProcAddress(hLib,TEXT("DeinitializeImageLineDataProcessingICMedia"));
						if (HWLAYER.DeinitializeImageLineDataProcessing == NULL) bStatus = FALSE;
						HWLAYER.GetFrequencyPerFrameOfLineDataCallback			= (DWORD(*)(void))GetProcAddress(hLib,TEXT("GetFrequencyPerFrameOfLineDataCallbackICMedia"));
						if (HWLAYER.GetFrequencyPerFrameOfLineDataCallback == NULL) bStatus = FALSE;
						HWLAYER.GetNumberOfValidRows						= (DWORD(*)(void))GetProcAddress(hLib,TEXT("GetNumberOfValidRowsICMedia"));
						if (HWLAYER.GetNumberOfValidRows == NULL) bStatus = FALSE;
						HWLAYER.ResetTransferHardware							= (void(*)(void))GetProcAddress(hLib,TEXT("ResetTransferHardwareICMedia"));
						if (HWLAYER.ResetTransferHardware == NULL) bStatus = FALSE;
						HWLAYER.CaptureInitiated                          = (DWORD(*)(void))GetProcAddress(hLib,TEXT("CaptureInitiatedICMedia"));
						if (HWLAYER.CaptureInitiated == NULL) { HWLAYER.CaptureInitiated = dummy_one;}
						HWLAYER.GetImagerProperties                       = (BOOL (*)(const DWORD *, DWORD *, DWORD))GetProcAddress(hLib,TEXT("GetImagerPropertiesICMedia"));
						if (HWLAYER.GetImagerProperties == NULL) { HWLAYER.GetImagerProperties = dummy_FALSE; }

						/* ...then the scanner specific functions which are overloaded within the driver itself */
						HHPSD_InitializeScanEngine						= InitializeScannerICMedia;
						HHPSD_DeinitializeScanEngine					= DeinitializeScannerICMedia;
						HHPSD_SetDefaultScannerOptions					= SetDefaultScannerOptionsICMedia;
						HHPSD_ScanEngineFaultOccurred					= ScanEngineFaultOccurredICMedia;
						HHPSD_ScanEngineRestart							= RestartScannerICMedia;
						HHPSD_ScanEngineSaveStateAndPowerDown			= ScanEngineSaveStateICMedia;
						HHPSD_ScanEnginePowerUpAndRestoreState			= ScanEngineRestoreStateICMedia;
						HHPSD_ScanEnginePowerUpAndRestoreRegs           = ScanEngineRestoreRegsICMedia;
						HHPSD_ExposureSettingsUpdate							= (void (*)(struct ScanDriverOpenContext_t *))ExposureSettingsUpdateICMedia;
//                    HHPSD_ScanManagerEngineSaysOkToCaptureNextFrame = ScanEngineOkToCaptureNextFrameICMedia;
						HHPSD_ScanManagerEngineSaysOkToCaptureNextFrame = dummy_one;
					} else {
						DBG_OUTPUT("---AutoDetectScanEngine- Engine is not IC Media\n");
						bStatus = FALSE;
					}
				}
				if (bStatus != TRUE)
				{
					DBG_OUTPUT("--- Unloading Libary- bStatus != TRUE\r\n");
					UnloadHardwareLibrary();
				}
			}
        } else {
            DBG_OUTPUT("---AutoDetectScanEngine-Couldn't find HHPICMEDIAHW.DLL\n");
        }
    }
	return bStatus;
}

void UnloadHardwareLibrary(void) {
	if (HHPSD_UnloadHWLayer)
		HHPSD_UnloadHWLayer();
	FreeLibrary(hLib);
}


#else // DBALL			//*****************************************************************************



#ifdef HI2D_T//KENTODO remove it 
#ifdef BYD_SENSOR

extern BOOL ScanEngineIsBYD(void);

BOOL AutoDetectBYD(void)
{
	HHPSD_interface * save_interface = p_hhpsd_interface;
	
	p_hhpsd_interface = get_hhpsd_BYD();
	p_hardware_layer_interface = p_BYD_hardware_layer_interface;
	
	HWLAYER.ImagerPowerOnAndResume	= NULL;
	HWLAYER.ImagerPowerOffAndSuspend= NULL;
	HWLAYER.ImagerPowerOn        	= dummy_zero;
	HWLAYER.ImagerPowerOff       	= dummy_zero;
	
	HWLAYER.ImagerIdlePowerLevel = ImagerIdlePowerLevelBYD;

	HWLAYER_BYD.ReadIIC 	= ReadIIC_BYD;
	HWLAYER_BYD.WriteIIC 	= WriteIIC_BYD;

	HWLAYER.IsPsocSupportedByToken = IsPsocInterfaceSupportedBYD;

	//Kyle???TODO:
	if(HWLAYER.IsPsocSupportedByToken())
	{
		p_hhpsd_interface->setLightController(new HW_light_control_OEM_Jade());
	}
	else
	{	   
		p_hhpsd_interface->setLightController(new HW_light_control_MT9V022());	
	}

	if( HWLAYER.IsPsocSupportedByToken() )
	{
		HWLAYER.WriteIIC_PSOC 		= NULL;
		HWLAYER.ReadIIC_PSOC 		= NULL;
		HWLAYER.WriteIIC_PSOC_w_ADD = NULL;
	}
	else
	{
		HWLAYER.WriteIIC_PSOC       = NULL;
		HWLAYER.ReadIIC_PSOC        = NULL;
		HWLAYER.WriteIIC_PSOC_w_ADD = NULL;
	}

	HWLAYER_BYD.WriteIIC_SpreadSpec 		= NULL;
	HWLAYER_BYD.ReadIIC_SpreadSpec 			= NULL;
	HWLAYER_BYD.WriteIIC_SpreadSpec_w_ADD 	= NULL;
	HWLAYER_BYD.InDownTime = NULL;
	
	HWLAYER.ImagerPowerUp 					= ImagerPowerUpBYD;
	HWLAYER.ImagerPowerDown 				= ImagerPowerDownBYD;
	HWLAYER.WaitMilliseconds 				= WaitMillisecondsBYD;
	HWLAYER.EngineCurrentlyPowered			= ImagerIsPoweredBYD;
	HWLAYER.SynchronizedPowerDown 			= NULL;
	
	if (ScanEngineIsBYD()) {
		HWLAYER_BYD.GetRegisterSettingsForHHPExposureControl 	= GetRegisterSettingsForHHPExposureControlBYD;
		HWLAYER_BYD.AimIsOn 				= AimIsOnBYD;
		HWLAYER_BYD.IllumIsOn 				= IllumIsOnBYD;
		HWLAYER_BYD.ImagerIdlePowerLevel 	= ImagerIdlePowerLevelBYD;
		HWLAYER_BYD.WaitForVsync 			= WaitForVsyncBYD;
		HWLAYER_BYD.RequestSynchronizedPowerDown				= RequestSynchronizedPowerDownBYD;

		HWLAYER.GetScanWidth 				= GetScanWidthBYD;
		HWLAYER.GetScanHeight				= GetScanHeightBYD;
		HWLAYER.AllocatePhysicalScanSpace	= AllocatePhysicalScanSpaceBYD;
		HWLAYER.FreePhysicalScanSpace		= FreePhysicalScanSpaceBYD;
		HWLAYER.BindPhysicalMemoryToVirtualMemory		= BindPhysicalMemoryToVirtualMemoryBYD;
		HWLAYER.UnbindPhysicalMemoryFromVirtualMemory	= UnbindPhysicalMemoryFromVirtualMemoryBYD;
		HWLAYER.IllumOn						= IllumOnBYD;
		HWLAYER.IllumConfig					= IllumConfigBYD;
		HWLAYER.IllumOff					= IllumOffBYD;
		HWLAYER.IllumOffTimer				= IllumOffTimerBYD;
		HWLAYER.AimOn						= AimOnBYD;
		HWLAYER.AimConfig					= AimConfigBYD;
		HWLAYER.AimOff						= AimOffBYD;
		HWLAYER.EnableVsyncProcessing		= EnableVsyncProcessingBYD;
		HWLAYER.DisableVsyncProcessing		= DisableVsyncProcessingBYD;
		HWLAYER.EnableImageLineDataProcessing			= EnableImageLineDataProcessingBYD;
		HWLAYER.DisableImageLineDataProcessing			= DisableImageLineDataProcessingBYD;
		HWLAYER.InitCapture					= InitCaptureBYD;
		HWLAYER.LoadCaptureInfo				= LoadCaptureInfoBYD;
		HWLAYER.RegisterVsyncNotification	= RegisterVsyncNotificationBYD;
		HWLAYER.InitializeVsyncProcessing	= InitializeVsyncProcessingBYD;
		HWLAYER.RegisterImageLineDataNotification		= RegisterImageLineDataNotificationBYD;
		HWLAYER.InitializeImageLineDataProcessing		= InitializeImageLineDataProcessingBYD;
		HWLAYER.DeinitializeVsyncProcessing 			= DeinitializeVsyncProcessingBYD;
		HWLAYER.DeinitializeImageLineDataProcessing		= DeinitializeImageLineDataProcessingBYD;
		HWLAYER.GetFrequencyPerFrameOfLineDataCallback	= GetFrequencyPerFrameOfLineDataCallbackBYD;
		HWLAYER.GetNumberOfValidRows 		= GetNumberOfValidRowsBYD;
		HWLAYER.ResetTransferHardware		= ResetTransferHardwareBYD;
		HWLAYER.LightsLowPowerMode			= LightsLowPowerModeBYD;
		HWLAYER.ImagerPowerOffAndSuspend 	= NULL;
		HWLAYER.CaptureInitiated 			= dummy_one;
		HWLAYER.GetImagerProperties			= GetImagerPropertiesBYD;
		HWLAYER.RegisterSofPsocWriteNotification		= RegisterSofPsocWriteNotificationBYD;

		SetupAutoAimControlFunctions( AutoAimControlHighPowerBYD,             //lmc
									  AutoAimControlLowPowerBYD );                     //lmc

		HHPSD_ScanManagerEngineSaysOkToCaptureNextFrame = dummy_one;
		HWLAYER.SetImagerModeToGrayscale	= SetImagerModeToGrayscaleBYD;			//added for testflex
		HWLAYER.UnSetImagerModeToGrayscale 	= UnSetImagerModeToGrayscaleBYD;		//added for testflex
		return TRUE;
	}
	p_hhpsd_interface = save_interface;

	return FALSE;
}

#else

extern BOOL ScanEngineIsHi704A(void);
#define PASTE(a,b) a##b
#define ASGN_HWLAYER(name) PASTE(HWLAYER., name) = PASTE(name, Hi704A)
#define ASGN_HI704A(name)    PASTE(HWLAYER_JADE., name) = PASTE(name, Hi704A)
#define ASGN_HI704A_(name)    PASTE(HWLAYER_JADE., name) = PASTE(name, _Hi704A)
BOOL AutoDetectHi704A(void)
{
	HHPSD_interface * save_interface = p_hhpsd_interface;
	p_hhpsd_interface = get_hhpsd_hi704a();
	p_hardware_layer_interface = p_Hi704A_hardware_layer_interface;
	HWLAYER.ImagerPowerOnAndResume= NULL;
	HWLAYER.ImagerPowerOffAndSuspend= NULL;
	HWLAYER.ImagerPowerOn        	= dummy_zero;
	HWLAYER.ImagerPowerOff       	= dummy_zero;
	ASGN_HWLAYER(ImagerIdlePowerLevel);
	ASGN_HI704A_(ReadIIC);
	ASGN_HI704A_(WriteIIC);

	//HWLAYER.IsPsocSupportedByToken = IsPsocSupportedByTokenJade;
//	ASGN_HWLAYER(IsPsocSupportedByToken);
	HWLAYER.IsPsocSupportedByToken = IsPsocInterfaceSupportedHi704A;

	//	p_hhpsd_interface->setLightController(new HW_light_control_primitive()); //lmc   was instance of 'HW_light_control_MT9V022()', no psoc, so make it primitive class instance
	
	if(HWLAYER.IsPsocSupportedByToken())
	{
		p_hhpsd_interface->setLightController(new HW_light_control_OEM_Jade());
	}
	else
	{	   
		p_hhpsd_interface->setLightController(new HW_light_control_MT9V022());	
	}

	if( HWLAYER.IsPsocSupportedByToken() )
	{
		HWLAYER.WriteIIC_PSOC = NULL;//WriteIIC_PSOC_Jade;
		HWLAYER.ReadIIC_PSOC = NULL;//ReadIIC_PSOC_Jade;
		HWLAYER.WriteIIC_PSOC_w_ADD = NULL;//WriteIIC_PSOC_w_ADD_Jade;
	}
	else
	{
		HWLAYER.WriteIIC_PSOC       = NULL;
		HWLAYER.ReadIIC_PSOC        = NULL;
		HWLAYER.WriteIIC_PSOC_w_ADD = NULL;
	}	
	ASGN_HI704A_(WriteIIC_SpreadSpec);
	ASGN_HI704A_(ReadIIC_SpreadSpec);
	ASGN_HI704A_(WriteIIC_SpreadSpec_w_ADD);


	ASGN_HWLAYER(ImagerPowerUp);
	ASGN_HWLAYER(ImagerPowerDown);
	ASGN_HWLAYER(WaitMilliseconds);
	HWLAYER.EngineCurrentlyPowered	= ImagerIsPoweredHi704A;
	ASGN_HI704A(InDownTime);
	HWLAYER.SynchronizedPowerDown = NULL; //Important for exception handling in ScanDriverEngine.c ** //ASGN_HWLAYER(SynchronizedPowerDown);
	
	if (ScanEngineIsHi704A()) {
//		p_hhpsd_interface->setLightController(new HW_light_control_MT9V022());
		ASGN_HI704A(GetRegisterSettingsForHHPExposureControl);
		ASGN_HI704A(AimIsOn);
		ASGN_HI704A(IllumIsOn);
//		HWLAYER.EngineCurrentlyPowered							= ImagerIsPoweredMT9V022;
		ASGN_HI704A(ImagerIdlePowerLevel);
		ASGN_HI704A(WaitForVsync);
		ASGN_HI704A(RequestSynchronizedPowerDown);

		ASGN_HWLAYER(GetScanWidth);
		ASGN_HWLAYER(GetScanHeight);
		ASGN_HWLAYER(AllocatePhysicalScanSpace);
		ASGN_HWLAYER(FreePhysicalScanSpace);
		ASGN_HWLAYER(BindPhysicalMemoryToVirtualMemory);
		ASGN_HWLAYER(UnbindPhysicalMemoryFromVirtualMemory);
		ASGN_HWLAYER(IllumOn);
		ASGN_HWLAYER(IllumConfig);
		ASGN_HWLAYER(IllumOff);
		ASGN_HWLAYER(IllumOffTimer);
		ASGN_HWLAYER(AimOn);
		ASGN_HWLAYER(AimConfig);
		ASGN_HWLAYER(AimOff);
		ASGN_HWLAYER(EnableVsyncProcessing);
		ASGN_HWLAYER(DisableVsyncProcessing);
		ASGN_HWLAYER(EnableImageLineDataProcessing);
		ASGN_HWLAYER(DisableImageLineDataProcessing);
		ASGN_HWLAYER(InitCapture);
		ASGN_HWLAYER(LoadCaptureInfo); //	= dummy_FALSE;
		ASGN_HWLAYER(RegisterVsyncNotification);
		ASGN_HWLAYER(InitializeVsyncProcessing);
		ASGN_HWLAYER(RegisterImageLineDataNotification);
		ASGN_HWLAYER(InitializeImageLineDataProcessing);
		ASGN_HWLAYER(DeinitializeVsyncProcessing);
		ASGN_HWLAYER(DeinitializeImageLineDataProcessing);
		ASGN_HWLAYER(GetFrequencyPerFrameOfLineDataCallback);
		ASGN_HWLAYER(GetNumberOfValidRows);
		ASGN_HWLAYER(ResetTransferHardware);
		ASGN_HWLAYER(LightsLowPowerMode);
		HWLAYER.ImagerPowerOffAndSuspend = NULL;
		HWLAYER.CaptureInitiated = dummy_one;
		ASGN_HWLAYER(GetImagerProperties);
		ASGN_HWLAYER(RegisterSofPsocWriteNotification);

		//SetupAutoAimControlFunctions( AutoAimControlHighPowerMT9V022,             //lmc
		//							  AutoAimControlLowPowerMT9V022 );                     //lmc
		SetupAutoAimControlFunctions( AutoAimControlHighPowerHi704A,             //lmc
									  AutoAimControlLowPowerHi704A );                     //lmc

		HHPSD_ScanManagerEngineSaysOkToCaptureNextFrame = dummy_one;
		HWLAYER.SetImagerModeToGrayscale									= SetImagerModeToGrayscaleHi704A;			//added for testflex
		HWLAYER.UnSetImagerModeToGrayscale 								= UnSetImagerModeToGrayscaleHi704A;		//added for testflex
		return TRUE;
	}
	p_hhpsd_interface = save_interface;

	return FALSE;
}
#endif //end #ifdef BYD_SENSOR
#else
extern BOOL ScanEngineIsJade(void);
BOOL AutoDetectJade(void)
{
#define PASTE(a,b) a##b
#define ASGN_HWLAYER(name) PASTE(HWLAYER., name) = PASTE(name, Jade)
#define ASGN_JADE(name)    PASTE(HWLAYER_JADE., name) = PASTE(name, Jade)
#define ASGN_JADE_(name)    PASTE(HWLAYER_JADE., name) = PASTE(name, _Jade)

	HHPSD_interface * save_interface = p_hhpsd_interface;
	p_hhpsd_interface = get_hhpsd_jade();

	p_hardware_layer_interface = p_jade_hardware_layer_interface;
	HWLAYER.ImagerPowerOnAndResume= NULL;
	HWLAYER.ImagerPowerOffAndSuspend= NULL;
	HWLAYER.ImagerPowerOn        	= dummy_zero;
	HWLAYER.ImagerPowerOff       	= dummy_zero;
	ASGN_HWLAYER(ImagerIdlePowerLevel);
	ASGN_JADE_(ReadIIC);
	ASGN_JADE_(WriteIIC);

	//HWLAYER.IsPsocSupportedByToken = IsPsocSupportedByTokenJade;
//	ASGN_HWLAYER(IsPsocSupportedByToken);
	HWLAYER.IsPsocSupportedByToken = IsPsocInterfaceSupportedJade;

	//	p_hhpsd_interface->setLightController(new HW_light_control_primitive()); //lmc   was instance of 'HW_light_control_MT9V022()', no psoc, so make it primitive class instance
	
	if(HWLAYER.IsPsocSupportedByToken())
	{
		p_hhpsd_interface->setLightController(new HW_light_control_OEM_Jade());
	}
	else
	{	   
		p_hhpsd_interface->setLightController(new HW_light_control_MT9V022());	
	}

	if( HWLAYER.IsPsocSupportedByToken() )
	{
		HWLAYER.WriteIIC_PSOC = WriteIIC_PSOC_Jade;
		HWLAYER.ReadIIC_PSOC = ReadIIC_PSOC_Jade;
		HWLAYER.WriteIIC_PSOC_w_ADD = WriteIIC_PSOC_w_ADD_Jade;
	}
	else
	{
		HWLAYER.WriteIIC_PSOC       = NULL;
		HWLAYER.ReadIIC_PSOC        = NULL;
		HWLAYER.WriteIIC_PSOC_w_ADD = NULL;
	}	
	ASGN_JADE_(WriteIIC_SpreadSpec);
	ASGN_JADE_(ReadIIC_SpreadSpec);
	ASGN_JADE_(WriteIIC_SpreadSpec_w_ADD);


	ASGN_HWLAYER(ImagerPowerUp);
	ASGN_HWLAYER(ImagerPowerDown);
	ASGN_HWLAYER(WaitMilliseconds);
	HWLAYER.EngineCurrentlyPowered	= ImagerIsPoweredJade;
	ASGN_JADE(InDownTime);
	HWLAYER.SynchronizedPowerDown = NULL; //Important for exception handling in ScanDriverEngine.c ** //ASGN_HWLAYER(SynchronizedPowerDown);
	
	if (ScanEngineIsJade()) {
//		p_hhpsd_interface->setLightController(new HW_light_control_MT9V022());
		ASGN_JADE(GetRegisterSettingsForHHPExposureControl);
		ASGN_JADE(AimIsOn);
		ASGN_JADE(IllumIsOn);
//		HWLAYER.EngineCurrentlyPowered							= ImagerIsPoweredMT9V022;
		ASGN_JADE(ImagerIdlePowerLevel);
		ASGN_JADE(WaitForVsync);
		ASGN_JADE(RequestSynchronizedPowerDown);

		ASGN_HWLAYER(GetScanWidth);
		ASGN_HWLAYER(GetScanHeight);
		ASGN_HWLAYER(AllocatePhysicalScanSpace);
		ASGN_HWLAYER(FreePhysicalScanSpace);
		ASGN_HWLAYER(BindPhysicalMemoryToVirtualMemory);
		ASGN_HWLAYER(UnbindPhysicalMemoryFromVirtualMemory);
		ASGN_HWLAYER(IllumOn);
		ASGN_HWLAYER(IllumConfig);
		ASGN_HWLAYER(IllumOff);
		ASGN_HWLAYER(IllumOffTimer);
		ASGN_HWLAYER(AimOn);
		ASGN_HWLAYER(AimConfig);
		ASGN_HWLAYER(AimOff);
		ASGN_HWLAYER(EnableVsyncProcessing);
		ASGN_HWLAYER(DisableVsyncProcessing);
		ASGN_HWLAYER(EnableImageLineDataProcessing);
		ASGN_HWLAYER(DisableImageLineDataProcessing);
		ASGN_HWLAYER(InitCapture);
		ASGN_HWLAYER(LoadCaptureInfo); //	= dummy_FALSE;
		ASGN_HWLAYER(RegisterVsyncNotification);
		ASGN_HWLAYER(InitializeVsyncProcessing);
		ASGN_HWLAYER(RegisterImageLineDataNotification);
		ASGN_HWLAYER(InitializeImageLineDataProcessing);
		ASGN_HWLAYER(DeinitializeVsyncProcessing);
		ASGN_HWLAYER(DeinitializeImageLineDataProcessing);
		ASGN_HWLAYER(GetFrequencyPerFrameOfLineDataCallback);
		ASGN_HWLAYER(GetNumberOfValidRows);
		ASGN_HWLAYER(ResetTransferHardware);
		ASGN_HWLAYER(LightsLowPowerMode);
		HWLAYER.ImagerPowerOffAndSuspend = NULL;
		HWLAYER.CaptureInitiated = dummy_one;
		ASGN_HWLAYER(GetImagerProperties);
		ASGN_HWLAYER(RegisterSofPsocWriteNotification);

		//SetupAutoAimControlFunctions( AutoAimControlHighPowerMT9V022,             //lmc
		//							  AutoAimControlLowPowerMT9V022 );                     //lmc
		SetupAutoAimControlFunctions( AutoAimControlHighPowerJade,             //lmc
									  AutoAimControlLowPowerJade );                     //lmc

		HHPSD_ScanManagerEngineSaysOkToCaptureNextFrame = dummy_one;
		HWLAYER.SetImagerModeToGrayscale									= SetImagerModeToGrayscaleJade;			//added for testflex
		HWLAYER.UnSetImagerModeToGrayscale 								= UnSetImagerModeToGrayscaleJade;		//added for testflex
		return TRUE;
	}
	p_hhpsd_interface = save_interface;

	return FALSE;
}
#endif

// At this point Autodetection will simply be the loading of the linear functions
BOOL AutoDetectScanEngine(void)
{
	initialize();
#ifdef HI2D_T//TODO auto detect the sensor type according to hardware token???
#ifdef BYD_SENSOR
	if (AutoDetectBYD())
		return TRUE;
	else
		return FALSE; 
#else
	if (AutoDetectHi704A())
		return TRUE;
	else
		return FALSE;  
#endif //#ifdef BYD_SENSOR
#else
	if (AutoDetectJade())
		return TRUE;
	else
		return FALSE;                                       //lmc -  only working with Jade for now
#endif

}
void UnloadHardwareLibrary(void) {
}
#endif
