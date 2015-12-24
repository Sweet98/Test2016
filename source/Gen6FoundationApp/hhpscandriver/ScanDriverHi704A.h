#ifndef SCANDRIVER_BYD_H
#define SCANDRIVER_BYD_H

#include "ScanDriverDebug.h"
#include "Hi704A.h"

#ifdef UNDER_CE
#define FUNCTION __FUNCTION
#else
#define FUNCTION __func__
#endif

#define UNIMPL                                                      \
{                                                                   \
   static bool has_run_D0E525F3DC276C82 = false;                    \
   DBG(== 3) if (has_run_D0E525F3DC276C82 == false)					\
   {                                                                \
	   has_run_D0E525F3DC276C82 = true;                             \
	   dbg_output("Function, %s is not implemented\r\n", FUNCTION); \
   }                                                                \
}                                                                   \
return


#define BYD_EXP_EVERY_TWO_FRAMES	1


class Exposure_control;
class Pixel_sampler_abs;
class Auto_exposure_accumulator;
class Context_sensitive_accumulator;
template <class T> class Pixel_sampler;
class BYDConfig_t;

typedef enum
{
	POWER_DOWN,
	POWER_UP,
	STANDBY,
	TRIGGER,
	IDLE,
	CLEAR
}OperatingModesBYD_t;


struct DefaultAutoExposure : public ScanDriverHHPAutoExposureOptions_t
{
	DefaultAutoExposure()
		{
			MethodToUse                 = HHPSD_AE_METHOD_CENTER_WEIGHTED;
			nTargetValue                = 90;
			nTargetPercentile           = 97;
			nTargetAcceptanceGap        = 40;
			nMaximumExposure            = BYD_INT_MAX;
			nMaximumGain                = BYD_GAIN_MAX;
			nStartingExposure           = BYD_INT_FIXED;
			nStartingGain               = BYD_GAIN_FIXED;
			nFrameRate                  = BYD_FRAME_RATE;
			bImageMustConform           = FALSE;
			nTriesForConforming         = 6;
			nSpecularExclusion          = HHPSD_SPECULAR_EXCLUSION_MODERATE;
			nSpecialSpecularSaturation  = 240;
			nSpecialSpecularLimit       = 3;
			bAdjustExpWhenIllumOff      = TRUE;
			bFastStart                  = FALSE;
			nExposureCell	            = 100;
		}
};



struct DefaultFixedExposure : public ScanDriverHHPFixedExposureOptions_t
{
	DefaultFixedExposure()
		{
			nFixedExposure = BYD_INT_FIXED;
			nFixedGain     = BYD_GAIN_FIXED;
			nFrameRate     = BYD_FRAME_RATE;
		};
};

struct DefaultLighting : public ScanDriverLightingOptions
{
	DefaultLighting()
		{
			nIlluminationIntensity       = LIGHTS_CONTROL_MAXIMUM_ILLUM_SETTING;
			bAllowIllumIntensityToAdjust = FALSE;
			bAdjustIllumIntensityDown    = TRUE;
			nAimerIntensity              = LIGHTS_CONTROL_MAXIMUM_ILLUM_SETTING;
			bAllowAimerIntensityToAdjust = FALSE;
			bAdjustAimerIntensityDown    = TRUE;
			bSoftwareControlledAimer     = FALSE;
			nScanLightMode               = HHPSD_LM_OFF;
			nIdleLightMode               = HHPSD_LM_OFF;
			nIlluminationSource          = HHPSD_IS_PRIMARY;
		}
};


class HHPSD_BYD : public HHPSD_interface
{
public:
	HHPSD_BYD();
	virtual void ScanManagerAutoExpLineSample(void *pImage,DWORD nRowsComplete);
	virtual BOOL ScanManagerImageIsAcceptable(ScanDriverLastScanInfo_t *pStats);
	virtual DWORD ScanManagerEngineSaysOkToCaptureNextFrame(void);
	virtual DWORD ScanEngineFaultOccurred(void);
	virtual void ScanManagerAutoExpCalc(void);
	virtual DWORD ScanEngineRestart(ScanDriverOpenContext_t *);
	virtual void ScanManagerFrameEventAutoExposure(void);
	virtual void ExposureSettingsUpdate(ScanDriverOpenContext_t *pOptions);
	virtual void LightSettingsUpdate(const ScanDriverOpenContext_t *pOptions);
	virtual DWORD ResetEngine(void);
	virtual DWORD InitializeScanEngine(ScanDriverOpenContext_t * p, BOOL b);
	virtual void  ScanEngineStart(void);
	virtual DWORD DeinitializeScanEngine(ScanDriverOpenContext_t * p, BOOL b);
	virtual DWORD ScanEngineSaveStateAndPowerDown(void);
	virtual DWORD ScanEnginePowerUpAndRestoreState(void);
	virtual DWORD ScanEnginePowerUpAndRestoreRegs(void);
	virtual DWORD SetDefaultScannerOptions(ScanDriverOpenContext_t * p);
		   

	bool is_autoexposure() const;
	bool is_context_sensitive() const;
	bool is_presentation() const;
	bool is_dual_track() const;
	bool is_fixed() const;
	bool is_onchip() const;
	bool is_openloop_gain() const;
	bool is_cellphone() const;
	
	DWORD iq_score() const;
	

	// copy and assignement are not supported.
	HHPSD_BYD(const HHPSD_BYD &);
	void operator = (const HHPSD_BYD &);

	void loadOpenContext(ScanDriverOpenContext_t * p);
	void resetAEStats();
	void InitializeAutoExposure(ScanDriverOpenContext_t * pOptions);

	BOOL IsAcceptablePresentation(ScanDriverLastScanInfo_t *pStats);
	BOOL IsAcceptableFixed(ScanDriverLastScanInfo_t *pStats);
	BOOL IsAcceptableNth(ScanDriverLastScanInfo_t *pStats);

	void AutoExpCalcFixed();
	void AutoExpCalcCellphone();
	
	BOOL CellphoneDetectionViaImaging(unsigned char * img, unsigned int StartRow, unsigned int EndRow);

	struct ExpSet
	{
		unsigned short Exp;
		unsigned short Gain;
	} ;
	ExpSet PresentationExp(unsigned idx);
	void AutoExpCalcPresentation();

	unsigned long nCapturedImages;
	DWORD frame_cnt; ///< Number of frames since start scannning.

	friend struct BYDConfig_t;
	ScanDriverOpenContext_t options;


	Pixel_sampler<Auto_exposure_accumulator>     * p_auto_exposure_sampler;
	Pixel_sampler<Context_sensitive_accumulator> * p_context_sensitive_sampler;

	static const int PRES_EXP_FORCE_CHANGE = 0xdd;
	int PresExpState;
	int PresExpStateLast;
};

void AutoAimControlHighPowerBYD(DWORD /*nLineInts*/);                        //lmc
void AutoAimControlLowPowerBYD(DWORD /*nLineInts*/);                         //lmc

static BOOL set_exp_gain(WORD exp, WORD gain );
static BOOL read_exp_gain(WORD *p_exp, WORD *p_gain);

HHPSD_BYD * get_hhpsd_BYD();
BOOL SetSensorMode (OperatingModesBYD_t nMode);
static Exposure_control * get_exposure_control(HHPSD_BYD * BYD, HHPScanDriverAutoExposureMode_t mode);

void cumulativeHistogram(  DWORD (&cumulative)[256], const DWORD (&histogram)[256]  );
unsigned percentile(DWORD (&cum)[256], unsigned percentile);


extern unsigned stamp_image_cnt;

#endif // #ifndef SCANDRIVER_BYD_H

