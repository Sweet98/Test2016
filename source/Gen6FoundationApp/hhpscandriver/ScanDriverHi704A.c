#if defined(_WIN32_WCE) || defined(PALMOS)
#include <windows.h>
#else // DBALL
extern "C" {
	int abs(int);
}
#include "language.h"
#include "platform.h"

#include <string.h>
#include <stdio.h>
#include <math.h>

#include "db_time.h"
#include "menu.h" //FIXME TODO definitely do not want this to stay
#include "ReaderConfiguration.h" //FIXME TODO definitely do not want this to stay

#include "ioman.h"

#endif


#include "ScanDriverOpenContext.h"
#include "ScanDriverScannerSelect.h"
#include "ScanDriverScanManager.h"
#include "ScanDriverGlobalConfig.h"
#include "ScanDriverDebug.h"
#include "ScanDriverLightControl.h"
#include "ScanDriverHi704A.h"
#include "ScanDriverEngine.h"
#include "ScanDriverMisc.h"

#include "exposure.h"
#include "context_sensitive_accumulator.h"
#include "auto_exposure_accumulator.h"
#include "context_sensitive_accumulator.cpp"
#include "auto_exposure_accumulator.cpp"
#include "History_queue.h"

/// @todo Remove this #include "Flash_illumination_control.h".  It was
/// added to import SetILL_FlashMode() prototype.  However, we
/// shouldn't call this directly.  Should go through the HW layer to
/// adjust flash mode.
#include "Flash_illumination_control.h"


#include "thread.h" /// @todo remove this.
#include "iMX25_Illumination_Control.h"  /// @todo remove this.
#include "iMX25_GPIO.h"  /// @todo remove this.

#include "productconfiguration.h"
#include "HardwareToken.h"



HHPSD_BYD::
HHPSD_BYD(): 	
		nCapturedImages(0),
		PresExpState(0),
		PresExpStateLast(0)
{
	p_auto_exposure_sampler = new Pixel_sampler<Auto_exposure_accumulator>;
	p_context_sensitive_sampler = new Pixel_sampler<Context_sensitive_accumulator>;
}

inline bool
HHPSD_BYD::
is_presentation() const
{
	return ::is_presentation(options.nAutoExposureMode);
}

inline bool
HHPSD_BYD::
is_fixed() const
{
	return ::is_fixed(options.nAutoExposureMode);
}

inline bool
HHPSD_BYD::
is_openloop_gain() const
{
	return ::is_openloop_gain(options.nAutoExposureMode);
}

inline bool
HHPSD_BYD::
is_autoexposure() const
{
	return ::is_autoexposure(options.nAutoExposureMode);
}

inline bool
HHPSD_BYD::
is_context_sensitive() const
{
	return ::is_context_sensitive(options.nAutoExposureMode);
}

inline bool
HHPSD_BYD::
is_dual_track() const
{
	return ::is_dual_track(options.nAutoExposureMode);
}

inline bool
HHPSD_BYD::
is_onchip() const
{
	return ::is_onchip(options.nAutoExposureMode);
}

inline bool
HHPSD_BYD::
is_cellphone() const
{
	return (options.nAutoExposureMode == HHPSD_CELLPHONE);
}

DWORD
HHPSD_BYD::
iq_score() const
{
	if (is_context_sensitive())
		return p_context_sensitive_sampler->get_iq_score();
	else
		return 0xFFFF;

}

class IIC_Shadow
{
 public:
	template <unsigned n>
	explicit IIC_Shadow(BYTE const (&reg_no_ary)[n])
		{
			initialize (reg_no_ary, n);
		}
	IIC_Shadow(BYTE const reg_no_ary[], unsigned n)
		{ initialize (reg_no_ary, n); }
	BOOL  WriteIIC(unsigned char, unsigned short);
	BOOL  ReadIIC(unsigned char, unsigned short *);
	void reset();



 private:
	void initialize(BYTE const reg_no_ary[], unsigned n);

	unsigned n_state;
	struct Shadow_state
	{
		BYTE  reg_no;
		bool  initialized;
		WORD  value;
	};
	Shadow_state * pState;

	Shadow_state * find_state(BYTE reg_no);
};

IIC_Shadow * p_iic_shadow = 0;
#define iic_shadow (*p_iic_shadow)

void IIC_Shadow :: reset()
{
	for (unsigned i = 0; i < n_state; ++i)
		pState[i].initialized = false;
}

IIC_Shadow::Shadow_state *
IIC_Shadow ::
find_state(BYTE reg_no)
{
	for (unsigned i = 0; i < n_state; ++i)
		if (pState[i].reg_no == reg_no)
			return &pState[i];
	return 0;
}

BOOL IIC_Shadow :: WriteIIC(unsigned char nReg, const WORD value)
{
	BOOL result = HWLAYER_BYD.WriteIIC(nReg, &value, 1);
	
	 if (!result)
	{
		return result;
	}

	Shadow_state * pstate = find_state(nReg);
	if (pstate)
	{
		pstate->initialized = true;
		pstate->value       = value;
	}
	return TRUE;
}

BOOL  IIC_Shadow :: ReadIIC(BYTE nReg, WORD * value)
{
	Shadow_state * pstate = find_state(nReg);
	if (pstate && pstate->initialized)
	{
		*value = pstate->value;
		return TRUE;
	}
	BOOL result = HWLAYER_BYD.ReadIIC(nReg, value, 1);

	if (result)
	{
//		DBGM(4,) dbg_output("ReadIIC(0x%X) = 0x%X\r\n", nReg, *value);
	}
	else
	{
//		DBGM(4,) dbg_output("ReadIIC(0x%X) failed\r\n", nReg);
	}

	if (pstate && result)
	{
		pstate->initialized = true;
		pstate->value       = *value;
	}
	return result;
}

void IIC_Shadow :: initialize(BYTE const reg_no_ary[], unsigned n)
{
	pState = new Shadow_state[n];
	for (unsigned i = 0; i < n; ++i)
	{
		pState[i].reg_no      = reg_no_ary[i];
		pState[i].initialized = false;
		pState[i].value       = 0;
//		dbg_output("Shadowing reg 0x%X\r\n", pState[i].reg_no);
	}
	n_state = n;
}


/** Contains exposure control information.

    @todo The references that point into hhpsd_BYD.options need to be
    removed.  Code that reliese on the references should be reworked
    to access options directly.

*/
struct BYDConfig_t
{
	BYDConfig_t() :
		nRows(0),
		nColumns(0),
		nMaximumGain(get_hhpsd_BYD()->options.AutoExpOptions.nMaximumGain),
		nTargetValue(get_hhpsd_BYD()->options.AutoExpOptions.nTargetValue),
		nTargetAcceptanceGap(get_hhpsd_BYD()->options.AutoExpOptions.nTargetAcceptanceGap),
		bImageMustConform(get_hhpsd_BYD()->options.AutoExpOptions.bImageMustConform),
		nTriesForConforming(get_hhpsd_BYD()->options.AutoExpOptions.nTriesForConforming),
		bShutdownOnGoodCapture(0),
		bUseSpecularExclusion(0),
		nSpecularExclusionLimit(0)
		{
		}
 	DWORD nRows;
 	DWORD nColumns;
	DWORD nFrameRate;
 	DWORD nMaximumGain;
	DWORD nMinimumGain;
	DWORD nMaximumExposure;
	DWORD nMinimumExposure;
	DWORD nFixedExposure;
	DWORD nFixedGain;
	DWORD nStartingExposure;
	DWORD nStartingGain;
 	DWORD nTargetValue;
	DWORD nTargetPercentile;
 	DWORD nTargetAcceptanceGap;
	DWORD bImageMustConform;
	DWORD nTriesForConforming;
 	DWORD bShutdownOnGoodCapture;
 	DWORD bUseSpecularExclusion;
	DWORD nSpecularExclusionLimit;
};

typedef struct
{
	unsigned int CellphoneIllumToggle;

	WORD last_exp;
	WORD current_exp;
	WORD next_exp;
	WORD last_gain;
	WORD current_gain;
	WORD next_gain;
	WORD saved_exp;
	WORD saved_gain;

 	DWORD nIlluminationValue;
 	DWORD nIlluminationMaximum;
 	DWORD nIlluminationClipValue;
 	DWORD nConsecutiveNonconformingImages;
 	DWORD nLastImageConformed;

}BYDAutoExposureStats_t;

static BYDConfig_t *pConfigs;
#define Configs (*pConfigs)
static BYDAutoExposureStats_t AEStats;



class Exposure_control
{
public:

	bool abort()	//Kyle???TODO: remove it, not used any more for BYD
		{
			return true;
		}
	bool set_trigger(bool trigger)//Kyle???TODO: remove it, not used any more for BYD
		{
			return true;
		}

	virtual void sample(  void *pImage, DWORD nRows,
						  HHPScanDriverHHPExposureMethod method  )
	{
		Pixel_sampler_abs * sampler = get_pixel_sampler();
		if (sampler == 0)
			return;

		void *pImage2 = sampler->getImage();
		if (pImage2 == 0)
			sampler->setImage(pImage);
		else
			SDASSERT_EQ(pImage, pImage2);

		switch (method)
		{
			case HHPSD_AE_METHOD_UNIFORM:
				sampler->uniform_sample(nRows);
				break;
			case HHPSD_AE_METHOD_CENTER_ONLY:
				sampler->center_sample(nRows);
				break;
			case HHPSD_AE_METHOD_CENTER_WEIGHTED:
			default:
				sampler->center_weight_sample(nRows);
				break;
		}

	}

	virtual Pixel_sampler_abs * get_pixel_sampler()
		{
			DBGM(32,)
				dbg_output("get_pixel_sampler()\r\n");
			return 0;
		}
	virtual DWORD initialize(ScanDriverOpenContext_t * pOptions,
							 BOOL bAutoShutdown, DWORD & start_exposure,
							 unsigned & start_gain, bool &byebye)
		{
			start_exposure = BYD_INT_FIXED;
			start_gain = BYD_GAIN_FIXED;

			return true;
		}
	virtual DWORD deinitialize(ScanDriverOpenContext_t * pOptions,
							   BOOL bAutoShutdown)
		{
//			dbg_output("deinitialize()\r\n");
			return true;
		}
	virtual bool is_acceptable(ScanDriverLastScanInfo_t * pStats)
		{
			return TRUE;
		}
	virtual void calc_exposure()
		{
			DBGM(32,)
				dbg_output("calc_exposure\r\n");
		}
	virtual void frame_event()
		{
			DBGM(32,)
				dbg_output("frame_event\r\n");
		}
};



class Exposure_control_cellphone : public Exposure_control
{
public:

	Exposure_control_cellphone(HHPSD_BYD * _BYD)
		: BYD(_BYD)
	{
	}
private:
	
#define BYD_CELLPHONE_IDLE 0		// illumination on, normal exposure setting
#define BYD_CELLPHONE_ILL_OFF 1		// illumination off, cell phone exposure setting
#define BYD_CELLPHONE_ILL_ON 2		// illumination on, exposure setting as idle

	HHPSD_BYD * BYD;

	virtual Pixel_sampler_abs * get_pixel_sampler()
		{
			return BYD->p_auto_exposure_sampler;
		}

	virtual bool is_acceptable(ScanDriverLastScanInfo_t * pStats);

	virtual void calc_exposure();

};

bool
Exposure_control_cellphone::
is_acceptable(ScanDriverLastScanInfo_t * pStats)
{
	pStats->nIllumValue = AEStats.nIlluminationValue;
	pStats->nIllumMaxValue = AEStats.nIlluminationMaximum;
	pStats->nIllumClipValue = AEStats.nIlluminationClipValue;
	pStats->nHeight	= Configs.nRows;
	pStats->nWidth    = Configs.nColumns;
	pStats->nImageNumber = ++BYD->nCapturedImages;
	pStats->nIqScore = BYD->iq_score();
	pStats->nDecodeParameter = AEStats.last_exp * AEStats.last_gain;
	return true; //Kyle???TODO:
}

void
Exposure_control_cellphone::
calc_exposure()
{
	static unsigned int CellphoneFrameCounter = 0;
	static int BYDCellphoneState = BYD_CELLPHONE_ILL_ON;
	BOOL skipAdjustContrast = FALSE;


	CellphoneFrameCounter ++;
	//dbg_output("Exposure_control_cellphone: CellphoneFrameCounter:0x%d BYDCellphoneState:0x%d\r\n",CellphoneFrameCounter,BYDCellphoneState);

	switch(BYDCellphoneState)
	{
		case BYD_CELLPHONE_ILL_ON:
			//if(AEStats.CellphoneIllumToggle)
			if(CellphoneFrameCounter%4==2)
			{
				BYDCellphoneState = BYD_CELLPHONE_ILL_OFF;
				//CellphoneFrameCounter = 0;	

				//turnoff_illum();
				((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_flash.turnoff(HI2D_AIM_ALT, true);
				((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_aimer.turnoff(HI2D_AIM_ALT);

				set_exp_gain(Configs.nMaximumExposure, Configs.nMaximumGain);
				skipAdjustContrast = TRUE;

				DBGM(32,)
				{
					dbg_output("------------------cellphone on, illum off, set max exp\r\n");
				}
			}
			break;
		case BYD_CELLPHONE_ILL_OFF:
			if(CellphoneFrameCounter%4 == 0)
			{
				BYDCellphoneState = BYD_CELLPHONE_ILL_ON;
				CellphoneFrameCounter = 0;

				//turnon_illum();
				((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_flash.turnon((ScanAimerIllumMode_t)GET_SETTING(ScanAimerMode), GET_SETTING(IllOnDuringDecode));
				((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_aimer.turnon((ScanAimerIllumMode_t)GET_SETTING(ScanAimerMode));

				set_exp_gain(Configs.nFixedExposure >> 1, Configs.nFixedGain >> 1);
				skipAdjustContrast = TRUE;

				DBGM(32,)
				{
					dbg_output("************************BYD_CELLPHONE_ILL_OFF -> illum on, set fixed exp\r\n");
				}
			}
			break;
			
//		case BYD_CELLPHONE_ILL_ON:
//			if(CellphoneFrameCounter >= 2)
//			{
//				BYDCellphoneState = BYD_CELLPHONE_IDLE;	
//				CellphoneFrameCounter = 0;
//				DBGM(32,)
//				{
//					dbg_output("***********************BYD_CELLPHONE_ILL_ON -> BYD_CELLPHONE_IDLE\r\n");
//				}
//			}
//			break;
	}

	if(skipAdjustContrast)
		return;


	DWORD (*ppHistogram)[256] = 0;
	DWORD cumulative[256];
	unsigned sample_cnt = 0;
	WORD exp_set, gain_set;

	DWORD &illum = AEStats.nIlluminationValue;
	
#if !ASSERT_FLAG
	sample_cnt++;                 //fix compiler warning
#endif

	ppHistogram = &(BYD->p_auto_exposure_sampler->nHistogram);


	cumulativeHistogram(cumulative, *ppHistogram);


	sample_cnt = cumulative[elements(cumulative)-1];

	illum = percentile(cumulative, Configs.nTargetPercentile);

	AEStats.nIlluminationClipValue	= 255;
	AEStats.nIlluminationMaximum	= 255;
	
	if (sample_cnt == 0)
	{
		DBGM(32,)
			dbg_output("sample_cnt = %u\r\n", sample_cnt);
		return;
	}
	
	DBGM(32,)
		dbg_output ("illum =%d,sample_cnt = %u\r\n",illum,sample_cnt);
	
	read_exp_gain(&exp_set,&gain_set);

	SDASSERT(exp_set == AEStats.current_exp);
	SDASSERT(gain_set == AEStats.current_gain);

	//BYD sensor's exp setting will work in next next frame.
	//so we ajust exp every two frames
#if BYD_EXP_EVERY_TWO_FRAMES
	if(AEStats.last_exp != AEStats.current_exp || AEStats.last_gain != AEStats.current_gain)
		return;
#else
	if(AEStats.last_exp == 0 && AEStats.last_gain == 0)
		return;
#endif

	if(AEStats.last_exp == 0)
		AEStats.last_exp = 1;
	if(AEStats.last_gain == 0)
		AEStats.last_gain = 1;
	
	DWORD last_contr   = AEStats.last_exp * AEStats.last_gain;
	DWORD target_contr = last_contr * Configs.nTargetValue / illum;

	// Don't allow the target_contr to increase more than 16 times of the last
	// contrast.  This prevents an overshoot that may occur if the illumination
	// value is extremely low (i.e. < 5)
	target_contr = min(target_contr, last_contr * 16);

	//must make sure last_exp and BYD_GAIN_MAX not zero
	AEStats.next_gain = ((target_contr + (AEStats.last_exp>>1)) / AEStats.last_exp);
	AEStats.next_gain = bound<WORD>(Configs.nMinimumGain, AEStats.next_gain, Configs.nMaximumGain);

		
	AEStats.next_exp = ((target_contr + (Configs.nMaximumGain>>1)) / Configs.nMaximumGain);
	AEStats.next_exp = bound<WORD>(Configs.nMinimumExposure, AEStats.next_exp, Configs.nMaximumExposure);

	set_exp_gain(AEStats.next_exp, AEStats.next_gain);	
}


class Auto_exposure_control : public Exposure_control
{
private:

	HHPSD_BYD * BYD;

public:
	Auto_exposure_control(HHPSD_BYD * _BYD)
		: BYD(_BYD)
	{
	}
	virtual Pixel_sampler_abs * get_pixel_sampler()
		{
			if (BYD->is_autoexposure() || BYD->is_presentation())
				return BYD->p_auto_exposure_sampler;
			if (BYD->is_context_sensitive())
				return BYD->p_context_sensitive_sampler;
			return 0;
		}

	virtual DWORD deinitialize(ScanDriverOpenContext_t * pOptions,
							   BOOL bAutoShutdown);
	virtual bool is_acceptable(ScanDriverLastScanInfo_t * pStats);
	virtual void calc_exposure();
	
};


DWORD
Auto_exposure_control::
deinitialize(ScanDriverOpenContext_t * pOptions, BOOL bAutoShutdown)
{
	//Kyle:???TODO: save contrast and it can be used for next starting
	return TRUE;
}


bool
Auto_exposure_control::
is_acceptable(ScanDriverLastScanInfo_t * pStats)
{
//	const ScanDriverHHPAutoExposureOptions_t & autoExp = BYD->autoExp;
	BOOL bStatus;
	UCHAR accept;
	
	pStats->nIllumValue = AEStats.nIlluminationValue;
	pStats->nIllumMaxValue = AEStats.nIlluminationMaximum;
	pStats->nIllumClipValue = AEStats.nIlluminationClipValue;
	pStats->nHeight	= Configs.nRows;
	pStats->nWidth    = Configs.nColumns;
	pStats->nImageNumber = ++BYD->nCapturedImages;
	pStats->nIqScore = BYD->iq_score();
	pStats->nDecodeParameter = AEStats.last_exp * AEStats.last_gain;
	
	accept = 0;
	if(Configs.bImageMustConform == FALSE)
	{
		accept |= 1;
	}
	else
	{
		if((DWORD)abs(int(Configs.nTargetValue - AEStats.nIlluminationValue)) <= Configs.nTargetAcceptanceGap)
			accept |= 2;
		if((AEStats.nIlluminationValue < Configs.nTargetValue) && 
			(AEStats.last_exp >= Configs.nMaximumExposure) &&  
			(AEStats.last_gain >= Configs.nMaximumGain))
			accept |= 4;
		if((AEStats.nIlluminationValue > Configs.nTargetValue) && 
			(AEStats.last_exp <= Configs.nMinimumExposure) && 
			(AEStats.last_gain <= Configs.nMinimumGain))
			accept |= 8;
		if(AEStats.nConsecutiveNonconformingImages >= (Configs.nTriesForConforming -1))
			accept |= 0x10;
	}
	
	if(accept)
	{
		DBGM(32,)
		{
			dbg_output ("Conforms is_acceptable : %x\r\n",!!(accept & 2),accept);
		}
		AEStats.nConsecutiveNonconformingImages = 0;
        AEStats.nLastImageConformed = TRUE;
		bStatus = TRUE;
	} 
	else 
	{
		++AEStats.nConsecutiveNonconformingImages;
		DBGM(32,)
			dbg_output ("Does not conform %d\r\n",AEStats.nConsecutiveNonconformingImages);
		AEStats.nLastImageConformed = FALSE;

		bStatus = FALSE;
	}
	return bStatus;
}


void 
Auto_exposure_control::
calc_exposure()
{
	DWORD (*ppHistogram)[256] = 0;
	DWORD cumulative[256];
	unsigned sample_cnt = 0;
	WORD exp_set, gain_set;
	DWORD &illum = AEStats.nIlluminationValue;

static unsigned int AutoFrameCounter = 0;
static int AimerState = 1;


AutoFrameCounter ++;
//dbg_output("Exposure_control_cellphone: CellphoneFrameCounter:0x%d BYDCellphoneState:0x%d\r\n",CellphoneFrameCounter,BYDCellphoneState);

switch(AimerState)
{
	case 1:
		//if(AEStats.CellphoneIllumToggle)
		if(AutoFrameCounter%5==1)
		{
			AimerState = 0; //aimer off

			//turnoff_illum();
			//((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_flash.turnoff(HI2D_AIM_ALT, true);
			((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_aimer.turnoff(HI2D_AIM_ALT);


			DBGM(32,)
			{
				dbg_output("------------------cellphone on, illum off, set max exp\r\n");
			}
		}
		break;
	case 0:
		if(AutoFrameCounter%5 == 0)
		{
			AimerState = 1;//aimer on
			AutoFrameCounter = 0;
			//turnon_illum();
			((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_aimer.turnon((ScanAimerIllumMode_t)GET_SETTING(ScanAimerMode));

			DBGM(32,)
			{
				dbg_output("************************BYD_CELLPHONE_ILL_OFF -> illum on, set fixed exp\r\n");
			}
		}
		break;
}


	
#if !ASSERT_FLAG
	sample_cnt++;                 //fix compiler warning
#endif

	if (BYD->is_autoexposure())
		ppHistogram = &(BYD->p_auto_exposure_sampler->nHistogram);
	else if (BYD->is_context_sensitive())
		ppHistogram = &(BYD->p_context_sensitive_sampler->nHistogram);
	else
	{
		SDASSERT(0);
		return;
	}

	cumulativeHistogram(cumulative, *ppHistogram);


	if (BYD->is_autoexposure())
	{
   		sample_cnt = cumulative[elements(cumulative)-1];
		{
			// TODO: Replace all of this with full auto exposure code.
			illum = percentile(cumulative, Configs.nTargetPercentile);

			AEStats.nIlluminationClipValue	= 255;
			AEStats.nIlluminationMaximum	= 255;
		}
    }
	else if (BYD->is_context_sensitive())
	{
		sample_cnt = BYD->p_context_sensitive_sampler->n_buffered;

		if (sample_cnt == 0)
		{
			DBGM(32,)
				dbg_output("sample_cnt = %u\r\n", sample_cnt);
			return;
		}
		
		SDASSERT(cumulative[elements(cumulative)-1] == sample_cnt);

		illum  = BYD->p_context_sensitive_sampler->get_white_value();
		

		AEStats.nIlluminationClipValue	= 255;
		AEStats.nIlluminationMaximum	= 255;
	}
	else
	{
		SDASSERT(0);
	}


	
	
	DWORD illum3, illum50, illum97;

	illum3  = percentile(cumulative, 3);
	illum50 = percentile(cumulative, 50);
	illum97 = percentile(cumulative, 97);
	
	DBGM(32,)
	{
		dbg_output ("illum =%d,target = %d iV=%d, TP=%d sample_cnt = %u\r\n",illum, Configs.nTargetValue, AEStats.nIlluminationValue, Configs.nTargetPercentile, sample_cnt);
	}
	
	if (illum == 0)
		illum = 1;

	read_exp_gain(&exp_set,&gain_set);

	SDASSERT(exp_set == AEStats.current_exp);
	SDASSERT(gain_set == AEStats.current_gain);

	//BYD sensor's exp setting will work in next next frame.
	//so we ajust exp every two frames
#if BYD_EXP_EVERY_TWO_FRAMES
	if(AEStats.last_exp != AEStats.current_exp || AEStats.last_gain != AEStats.current_gain)
		return;
#else
	if(AEStats.last_exp == 0 && AEStats.last_gain == 0)
		return;
#endif

	if(AEStats.last_exp == 0)
		AEStats.last_exp = 1;
	if(AEStats.last_gain == 0)
		AEStats.last_gain = 1;
	
	DWORD last_contr   = AEStats.last_exp * AEStats.last_gain;
	DWORD target_contr = last_contr * Configs.nTargetValue / illum;

	// Don't allow the target_contr to increase more than 16 times of the last
	// contrast.  This prevents an overshoot that may occur if the illumination
	// value is extremely low (i.e. < 5)
	target_contr = min(target_contr, last_contr * 16);
	
	// If we're way over exposed, this code drops the exposure
	// rapidly.  If the last_contr or nTragetExposure are very low, we may
	// be getting into an area where ADC ref. adjust kicks in.  We don't
	// want to do the rapid exposure drop in combiniation with ref. ADC
	// adjust
	// Conditions:
	// a) illumination value > 255
	// b) illumination_value > nTargetValue * 120% and in context
	//    sensitive mode and illum97 == 255.
	// c) last_contr > 32 and target_contr > 16
	// We do the rapid drop if condition (c) is true and either
	// condition (a) or (b) is true.

	if((  illum >= 255 ||  (  illum > ((Configs.nTargetValue * 307 + 128) >> 8) && BYD->is_context_sensitive() && illum97 >= 255))
		  && last_contr > (Configs.nMinimumExposure * Configs.nMinimumGain << 1) 
		  && target_contr > (Configs.nMinimumExposure * Configs.nMinimumGain))
	{
		// If the 3rd percentile illumination is high, drop exposure
		// by 1/8th.  If median illumination is high, drop by 1/4th
		// Otherwise, drop by 1/2.
		if (  BYD->is_context_sensitive()
			  && BYD->p_context_sensitive_sampler->get_cs_pixel_cnt() < 15
			  && illum50 >= 250)
		{
			DBGM(32,)
				dbg_output("16 adjust\r\n");
			target_contr = last_contr / 16;
		}
		else if (illum3 >= 220)
		{
			DBGM(32,)
				dbg_output("8 adjust\r\n");
			target_contr = last_contr / 8;
		}
		else if (illum50 >= 250)
		{
			DBGM(32,)
				dbg_output("4 adjust\r\n");
			target_contr = last_contr / 4;
		}
		else
		{
			DBGM(32,)
				dbg_output("2 adjust\r\n");
			target_contr = min<DWORD>(target_contr, last_contr/2);
		}

		target_contr = max(target_contr, DWORD((Configs.nMinimumExposure * Configs.nMinimumGain)));
	}

	//must make sure last_exp and BYD_GAIN_MAX not zero
	AEStats.next_gain = ((target_contr + (AEStats.last_exp>>1)) / AEStats.last_exp);
	AEStats.next_gain = bound<WORD>(Configs.nMinimumGain, AEStats.next_gain, Configs.nMaximumGain);

		
	AEStats.next_exp = ((target_contr + (Configs.nMaximumGain>>1)) / Configs.nMaximumGain);
	AEStats.next_exp = bound<WORD>(Configs.nMinimumExposure, AEStats.next_exp, Configs.nMaximumExposure);

	set_exp_gain(AEStats.next_exp, AEStats.next_gain);
	
}


DWORD
HHPSD_BYD ::
SetDefaultScannerOptions(ScanDriverOpenContext_t * pOpenContext)
{
	//Kyle???TODO: default settings
	pOpenContext->ScanEngine	  = HHPSD_ENGINE_IMAGER_E2V_BYD;
	pOpenContext->FixedExpOptions = DefaultFixedExposure();
	pOpenContext->AutoExpOptions  = DefaultAutoExposure();
	pOpenContext->LightingOptions = DefaultLighting();	//currently HI2D using another mothod to control light and aimer
	pOpenContext->EngineConfig    = DefaultEngineConfig();
	pOpenContext->LightingOptions.nIlluminationControl = FALSE;

	
	pOpenContext->EngineConfig.nEngineSensorIDRegister = (unsigned short)(gp_PsocRegisters->EngineID);

	return TRUE;
}

void
HHPSD_BYD ::
resetAEStats()
{
	p_auto_exposure_sampler->reset(  HWLAYER.GetScanWidth(),
									 HWLAYER.GetScanHeight()  );
	p_context_sensitive_sampler->reset(  HWLAYER.GetScanWidth(),
										 HWLAYER.GetScanHeight()  );
}



void
HHPSD_BYD ::
loadOpenContext(ScanDriverOpenContext_t * pOptions)
{
	options = *pOptions;

	Configs.nRows				= HWLAYER.GetScanHeight();
	Configs.nColumns			= HWLAYER.GetScanWidth();
	Configs.nFrameRate 			= options.AutoExpOptions.nFrameRate;

	Configs.bImageMustConform 	= options.AutoExpOptions.bImageMustConform;
	Configs.nTriesForConforming = options.AutoExpOptions.nTriesForConforming;

	Configs.nTargetValue 		= options.AutoExpOptions.nTargetValue;
	Configs.nTargetAcceptanceGap= options.AutoExpOptions.nTargetAcceptanceGap;

	Configs.nTargetPercentile 	= options.AutoExpOptions.nTargetPercentile;

	Configs.nFixedExposure 		= options.FixedExpOptions.nFixedExposure;
	Configs.nFixedGain 			= options.FixedExpOptions.nFixedGain;
	Configs.nMaximumExposure 	= options.AutoExpOptions.nMaximumExposure;
	Configs.nMinimumExposure	= BYD_INT_MIN;
	Configs.nMaximumGain		= options.AutoExpOptions.nMaximumGain;
	Configs.nMinimumGain		= BYD_GAIN_MIN;
	Configs.nStartingExposure 	= options.AutoExpOptions.nStartingExposure;
	Configs.nStartingGain		= options.AutoExpOptions.nStartingGain;


	Configs.bUseSpecularExclusion = TRUE;

	DWORD nSaturationValue = 245;
	switch(pOptions->AutoExpOptions.nSpecularExclusion )
	{
		case HHPSD_SPECULAR_EXCLUSION_OFF:
			Configs.bUseSpecularExclusion	= FALSE;
			break;
		case HHPSD_SPECULAR_EXCLUSION_MINIMAL:
			nSaturationValue			= 250;
			Configs.nSpecularExclusionLimit		= 2;
			break;
		case HHPSD_SPECULAR_EXCLUSION_MODERATE:
			nSaturationValue			= 245;
			Configs.nSpecularExclusionLimit		= 3;
			break;
		case HHPSD_SPECULAR_EXCLUSION_AGGRESSIVE:
			nSaturationValue			= 240;
			Configs.nSpecularExclusionLimit		= 4;
			break;
		case HHPSD_SPECULAR_EXCLUSION_SPECIAL:
			nSaturationValue = pOptions->AutoExpOptions.nSpecialSpecularSaturation;
			Configs.nSpecularExclusionLimit
				= pOptions->AutoExpOptions.nSpecialSpecularLimit;
			break;
	}
	p_auto_exposure_sampler->setSaturationValue(nSaturationValue);

	DBGM(64,)
	{
		dbg_output("Configs.nRows 				= %d,\r\n",Configs.nRows);
		dbg_output("Configs.nColumns 			= %d,\r\n",Configs.nColumns);
		dbg_output("Configs.nFrameRate 			= %d,\r\n",Configs.nFrameRate);
		dbg_output("Configs.bImageMustConform 	= %d,\r\n",Configs.bImageMustConform);
		dbg_output("Configs.nTriesForConforming = %d,\r\n",Configs.nTriesForConforming);
		dbg_output("Configs.nTargetValue 		= %d,\r\n",Configs.nTargetValue);
		dbg_output("Configs.nTargetAcceptanceGap= %d,\r\n",Configs.nTargetAcceptanceGap);
		dbg_output("Configs.nFixedExposure		= 0x%x,\r\n",Configs.nFixedExposure);
		dbg_output("Configs.nFixedGain			= 0x%x,\r\n",Configs.nFixedGain);
		dbg_output("Configs.nMaximumExposure 	= 0x%x,\r\n",Configs.nMaximumExposure);
		dbg_output("Configs.nMinimumExposure 	= 0x%x,\r\n",Configs.nMinimumExposure);
		dbg_output("Configs.nMaximumGain 		= 0x%x,\r\n",Configs.nMaximumGain);
		dbg_output("Configs.nMinimumGain 		= 0x%x,\r\n",Configs.nMinimumGain);
		dbg_output("Configs.nStartingExposure 	= 0x%x,\r\n",Configs.nStartingExposure);
		dbg_output("Configs.nStartingGain 		= 0x%x,\r\n",Configs.nStartingGain);
		dbg_output("Configs.bUseSpecularExclusion 	= %d,\r\n",Configs.bUseSpecularExclusion);
		dbg_output("Configs.nSpecularExclusionLimit = %d,\r\n",Configs.nSpecularExclusionLimit);
	}
}

void
HHPSD_BYD ::
InitializeAutoExposure(ScanDriverOpenContext_t * pOptions)
{
	loadOpenContext(pOptions);
	resetAEStats();
}


DWORD
HHPSD_BYD ::
InitializeScanEngine( ScanDriverOpenContext_t * pOptions, BOOL bAutoShutdown  )
{
	BOOL bStatus;
	
	frame_cnt = 0;

	// flush iic cache.
	iic_shadow.reset();

	if (gDriverConfigs.nImagerIdlePowerLevel != IMAGER_POWER_RUN)
	{
		Configs.bShutdownOnGoodCapture = bAutoShutdown;
	}

	InitializeAutoExposure(pOptions);

	AEStats.nIlluminationMaximum=AEStats.nIlluminationClipValue=255; // FIXME:  But suffice to say this is critical to report non-zero value to the decoder!!

	SetSensorMode (POWER_UP);

	read_exp_gain(&AEStats.next_exp, &AEStats.next_gain);

	AEStats.last_exp = 0;
	AEStats.last_gain= 0;
	AEStats.current_exp = 0;
	AEStats.current_gain = 0;

	DBGM(32,)
	{
		dbg_output("init exp  0x%x, gain 0x%x\r\n", AEStats.next_exp, AEStats.next_gain);
		dbg_output("last exp : 0x%x, current_exp : 0x%x, next_exp : 0x%x\r\n", AEStats.last_exp, AEStats.current_exp, AEStats.next_exp);
		dbg_output("last gain: 0x%x, current_gain: 0x%x, next_gain: 0x%x\r\n", AEStats.last_gain, AEStats.current_gain, AEStats.next_gain);
	}
	bStatus = TRUE;
	
	return bStatus;
}


VOID
HHPSD_BYD ::
ScanEngineStart (void)
{
	SetSensorMode(TRIGGER);
}


DWORD
HHPSD_BYD ::
DeinitializeScanEngine( ScanDriverOpenContext_t * pOptions, BOOL bAutoShutdown )
{
	stamp_image_cnt = 0;
	frame_cnt = 0;
	bool bAutoShutDownBYD = FALSE;

	set_exp_gain(Configs.nStartingExposure, Configs.nStartingGain);

	bAutoShutDownBYD = bAutoShutdown;

	if( bAutoShutDownBYD )//HI2D run here
	{
		/* get the register value for the mode */
		unsigned short miscel_ctrl = 0x00;

		//a register value that is not valid in BYD, to indicate power down mode IMAGER_POWER_OFF (0xF0)
		unsigned char miscel_ctrl_invalid_address = 0xF0;

		
		if (gDriverConfigs.nImagerIdlePowerLevel == IMAGER_POWER_OFF)
		{
			DBGM(32,)
				dbg_output("***auto,IMAGER_POWER_OFF\r\n");
			HWLAYER_BYD.RequestSynchronizedPowerDown( miscel_ctrl_invalid_address, &miscel_ctrl, 1 );

			SetSensorMode(POWER_DOWN);
		}
		else
		{
			DBGM(32,)
				dbg_output("***auto,IMAGER_POWER_IDLE\r\n");
			SetSensorMode(CLEAR); 

			//Kyle???TODO: we shouldn't use direct reg address, 0x09 should be define in hal0.h file
			//make hal1 here same api;
			if(iic_shadow.ReadIIC (0x09, &miscel_ctrl))
			{
				miscel_ctrl |= (1 << 4);
				HWLAYER_BYD.RequestSynchronizedPowerDown( (unsigned char) (0x09), &miscel_ctrl, 1 );
			}
		}
	}
	else
	{
		if (gDriverConfigs.nImagerIdlePowerLevel == IMAGER_POWER_OFF)
		{
			DBGM(32,)
				dbg_output("***bAutoShutDownBYD false,IMAGER_POWER_OFF\r\n");
			SetSensorMode(POWER_DOWN);
		}
		else
		{
			DBGM(32,)
				dbg_output("***bAutoShutDownBYD false,IMAGER_POWER_DLE\r\n");
			SetSensorMode(STANDBY);	
		}
	}

	DWORD ret = ( get_exposure_control( this, options.nAutoExposureMode )->deinitialize( pOptions, bAutoShutdown ) );
	ForceCellphone(false);
	return ret;
}


DWORD
HHPSD_BYD ::
ScanManagerEngineSaysOkToCaptureNextFrame(void)
{
	UNIMPL TRUE;
}



DWORD
HHPSD_BYD ::
ScanEngineRestart(ScanDriverOpenContext_t *)
{
	UNIMPL TRUE;
}


DWORD
HHPSD_BYD ::
ScanEngineSaveStateAndPowerDown()
{
	UNIMPL TRUE;
}

DWORD
HHPSD_BYD ::
ScanEnginePowerUpAndRestoreRegs()
{
	UNIMPL TRUE;
}

DWORD
HHPSD_BYD ::
ScanEnginePowerUpAndRestoreState(void)
{
	UNIMPL TRUE;
}


DWORD
HHPSD_BYD ::
ResetEngine(void)
{
	UNIMPL TRUE;
}

void
HHPSD_BYD ::
ExposureSettingsUpdate(ScanDriverOpenContext_t *pOptions)
{
	loadOpenContext(pOptions);
	PresExpState = PRES_EXP_FORCE_CHANGE;
	PresExpStateLast = 0;
}

void
HHPSD_BYD ::
LightSettingsUpdate(const struct ScanDriverOpenContext_t * pOptions)
{
	DBG(== 3) dbg_output("LightSettingsUpdate(0x%X)\r\n", pOptions);
}

void
HHPSD_BYD ::
ScanManagerAutoExpLineSample(  void *pImage, DWORD nRows  )
{

	if (  !is_fixed() && !is_presentation() )
	{
		get_exposure_control(this, options.nAutoExposureMode)
			->sample(pImage, nRows, options.AutoExpOptions.MethodToUse);
	}
	else
	{
		Pixel_sampler_abs * sampler = p_auto_exposure_sampler;
		if (sampler == 0)
			return;

		void *pImage2 = sampler->getImage();
		if (pImage2 == 0)
			sampler->setImage(pImage);
		else
			SDASSERT_EQ(pImage, pImage2);

		switch (options.AutoExpOptions.MethodToUse)
		{
			case HHPSD_AE_METHOD_UNIFORM:
				sampler->uniform_sample(nRows);
				break;
			case HHPSD_AE_METHOD_CENTER_ONLY:
				sampler->center_sample(nRows);
				break;
			case HHPSD_AE_METHOD_CENTER_WEIGHTED:
			default:
				sampler->center_weight_sample(nRows);
				break;
		}
	}

	//Kyle???TODO: can we use current sampling method specular_handler() ?
	if(is_cellphone())
	{
		//dbg_output("is_cellphone nRows(%d)\r\n", nRows);
		if(nRows >= 400)
		{
			AEStats.CellphoneIllumToggle = CellphoneDetectionViaImaging((unsigned char *)pImage, 60, 400);
		}
		//dbg_output("is_cellphone CellphoneIllumToggle(%d)\r\n", AEStats.CellphoneIllumToggle);
	}

}


/** Returns TRUE if the image is acceptable.
 */
BOOL
HHPSD_BYD ::
IsAcceptablePresentation(ScanDriverLastScanInfo_t *pStats)
{
	pStats->nIllumValue = AEStats.nIlluminationValue;
	pStats->nIllumMaxValue = AEStats.nIlluminationMaximum;
	pStats->nIllumClipValue = AEStats.nIlluminationClipValue;
	pStats->nHeight	= Configs.nRows;
	pStats->nWidth    = Configs.nColumns;

	pStats->nIqScore = iq_score();
	pStats->nDecodeParameter = 0;

	return TRUE;
}

BOOL
HHPSD_BYD ::
IsAcceptableFixed(ScanDriverLastScanInfo_t *pStats)
{
	pStats->nIllumValue = 0xffff;
	pStats->nIllumMaxValue = 0xff;
	pStats->nIllumClipValue = 0xff;
	pStats->nHeight	= Configs.nRows;
	pStats->nWidth    = Configs.nColumns;
	pStats->nIqScore = iq_score();
	pStats->nDecodeParameter = 0;

	return TRUE;
}


/** This version of is acceptable is used for diagnostics.  It always
    returns the AEStats.nConsecutiveNonconformingImages frame since the
	last power up.  This routine is called when bImageMustConform == 2.
  */

BOOL
HHPSD_BYD ::
IsAcceptableNth(ScanDriverLastScanInfo_t *pStats)
{
	pStats->nGain		= AEStats.last_gain;
	pStats->nIllumValue = AEStats.nIlluminationValue;
	pStats->nIllumMaxValue = AEStats.nIlluminationMaximum;
	pStats->nIllumClipValue = AEStats.nIlluminationClipValue;
	pStats->nHeight	= Configs.nRows;
	pStats->nWidth    = Configs.nColumns;
	pStats->nIqScore = iq_score();
	pStats->nDecodeParameter = 0;

	if (frame_cnt >= Configs.nTriesForConforming)
		return TRUE;
	return FALSE;
}

BOOL
HHPSD_BYD ::
ScanManagerImageIsAcceptable (ScanDriverLastScanInfo_t * pStats)
{
	BOOL ret = FALSE;

	pStats->nImageNumber = ++nCapturedImages;
	++frame_cnt;

	pStats->nExposure   = AEStats.last_exp;
	pStats->nGain		= AEStats.last_gain;

	if (Configs.bImageMustConform == 2)
		ret = IsAcceptableNth(pStats);
	else
	{
		if (is_fixed())
			ret = IsAcceptableFixed(pStats);
		else if (is_onchip())
			ret = TRUE;
		else if (is_presentation())
			ret = IsAcceptablePresentation(pStats);
		else
			ret = get_exposure_control(this, options.nAutoExposureMode)
				->is_acceptable(pStats);
	}

	if (ret)
	{
		if (Configs.bShutdownOnGoodCapture)
		{
			unsigned short miscel_ctrl = 0x00;
			unsigned char miscel_ctrl_invalid_address = 0x09;
			HWLAYER_BYD.RequestSynchronizedPowerDown( miscel_ctrl_invalid_address, &miscel_ctrl, 1 );
			set_exp_gain(BYD_INT_FIXED, BYD_GAIN_FIXED);
		}
	}

	return ret;
}

/** @todo For now, ScanEngineFaultOccurred is going to return FALSE.
	However, it needs to be modified to detect an incomplete frame transfer.
*/
DWORD
HHPSD_BYD ::
ScanEngineFaultOccurred()
{
	UNIMPL FALSE;
}

void
HHPSD_BYD ::
AutoExpCalcFixed()
{
	unsigned int exp = Configs.nFixedExposure;
	unsigned int gain = Configs.nFixedGain;

	ForceCellphone(false); 		//Force Cellphone mode off so the lights will be synced with exposure when switching out of cellphone mode.

	AEStats.next_gain = exp;
	AEStats.next_gain = bound<WORD>(Configs.nMinimumGain, AEStats.next_gain, Configs.nMaximumGain);

		
	AEStats.next_exp = gain;
	AEStats.next_exp = bound<WORD>(Configs.nMinimumExposure, AEStats.next_exp, Configs.nMaximumExposure);
	
	set_exp_gain(exp, gain);
	
	DBGM(32,)
		dbg_output("fixed exp  0x%x, gain 0x%x\r\n", exp, gain);
}


#define MAX_PRESENTATION_EXP 3

HHPSD_BYD::ExpSet
HHPSD_BYD::
PresentationExp(unsigned idx)
{
	ExpSet ret;

	switch(idx)
	{
		case 0:
			ret.Exp  = Configs.nMaximumExposure;
			ret.Gain = Configs.nMaximumGain;
			break;

		case 1:
			ret.Exp  = Configs.nMaximumExposure >> 3;	
			ret.Gain = Configs.nMaximumGain; 
			break;

		case 2:
			ret.Exp  = Configs.nMaximumExposure >> 3;
			ret.Gain = Configs.nMaximumGain >> 2;
			break;

		default:
			SDASSERT(0);
			// Fall trough intentional.

		case 3:
			ret.Exp   = Configs.nMaximumExposure >> 4;
			ret.Gain = Configs.nMaximumGain >> 4;
			break;
	}
	return ret;
}


void
HHPSD_BYD ::
AutoExpCalcPresentation()
{
	DWORD (*ppHistogram)[256] = 0;
	DWORD cumulative[256];
	DWORD nSamples;
	DWORD &illum_value = AEStats.nIlluminationValue;;
	WORD exp_set, gain_set;
	

    ForceCellphone(false);  //Force Cellphone mode off so the lights will be synced with exposure when switching out of cellphone mode.


	if (is_presentation())
		ppHistogram = &(p_auto_exposure_sampler->nHistogram);
	else
	{
		SDASSERT(0);
		return;
	}
	
	nSamples = p_auto_exposure_sampler->sample_cnt;
	/* No data to work with */
	if (nSamples == 0) {
		SDASSERT(0);
		return;
	}

	cumulativeHistogram(cumulative, *ppHistogram);
	
	if (is_presentation())
	{
   		nSamples = cumulative[elements(cumulative)-1];
		illum_value = percentile(cumulative, Configs.nTargetPercentile);
		DBGM(128,)
		{
			dbg_output("Configs.nTargetPercentile %d\r\n", Configs.nTargetPercentile);
		}

		AEStats.nIlluminationClipValue	= 255;
		AEStats.nIlluminationMaximum	= 255;
    }
	else
	{
		SDASSERT(0);
		return;
	}
	

	DBGM(32,)
	{
		dbg_output("illum = %d, status = %d\r\n", illum_value, PresExpState);
	}

	read_exp_gain(&exp_set,&gain_set);

	SDASSERT(exp_set == AEStats.current_exp);
	SDASSERT(gain_set == AEStats.current_gain);

	//BYD sensor's exp setting will work in next next frame.
	//so we ajust exp every two frames
#if BYD_EXP_EVERY_TWO_FRAMES
	if(AEStats.last_exp != AEStats.current_exp || AEStats.last_gain != AEStats.current_gain)
		return;
#else
	if(AEStats.last_exp == 0 && AEStats.last_gain == 0)
		return;
#endif


	AEStats.nIlluminationClipValue = AEStats.nIlluminationMaximum = 255;

	if (PresExpState == PRES_EXP_FORCE_CHANGE)
	{
		PresExpStateLast = PRES_EXP_FORCE_CHANGE;
		PresExpState = 0;
	}
	else if (PresExpState == PresExpStateLast)
	{
		if (  PresExpState < MAX_PRESENTATION_EXP
			  && (illum_value > 200))
		{
			PresExpState++;
		}
		else if ((PresExpState > 0) && (illum_value < 85))
		{
			PresExpState--;
		}
	} 
	else 
	{
		PresExpStateLast = PresExpState;
	}

	if (PresExpState != PresExpStateLast)
	{
		struct ExpSet expSet = PresentationExp(PresExpState);
		AEStats.next_gain = expSet.Gain;
		AEStats.next_exp = expSet.Exp;

		AEStats.next_gain = expSet.Gain;
		AEStats.next_gain = bound<WORD>(Configs.nMinimumGain, AEStats.next_gain, Configs.nMaximumGain);

		AEStats.next_exp = expSet.Exp;
		AEStats.next_exp = bound<WORD>(Configs.nMinimumExposure, AEStats.next_exp, Configs.nMaximumExposure);

		set_exp_gain(AEStats.next_exp, AEStats.next_gain);
	}
}


BOOL
HHPSD_BYD::
CellphoneDetectionViaImaging(unsigned char * img, unsigned int StartRow, unsigned int EndRow)
{
	unsigned int Histogram4SpecularDetect[256];
	unsigned char *rowbase;
	int i, r;
	int SaturatedPixels = 0, DarkPixels = 0, SpecularThre, SaturatedThre;
	//t1 = current_time();
	SpecularThre = 30;		//GET_SETTING(SpecularThreshold);
	SaturatedThre = 240;	//GET_SETTING(StaturatedThreshold);

	for(r = StartRow; r <= EndRow; r+=20)
	{
		rowbase = (img + r*640);

		for(i = SaturatedThre; i < 256; i++)
		{
			Histogram4SpecularDetect[i] = 0;
		}

		for(i = 0; i < 50; i++)
		{
			Histogram4SpecularDetect[i] = 0;
		}

		for(i=0; i<640; i++)
		{
			Histogram4SpecularDetect[*(rowbase + i)]++;
		}

		SaturatedPixels = 0;
		for(i = SaturatedThre; i < 256; i++)
		{
			SaturatedPixels += Histogram4SpecularDetect[i];
		}

		DarkPixels = 0;
		for(i = 0; i < 50; i++)
		{
			DarkPixels += Histogram4SpecularDetect[i];
		}

		if((SaturatedPixels > SpecularThre) && (DarkPixels > 200))
		{
			DBGM(32,)
			{
				dbg_output("SaturatedPixels = %d, DarkPixels = %d\r\n",SaturatedPixels, DarkPixels);
			}
			return true;
		}
	}
	DBGM(32,)
	{
		dbg_output("SaturatedPixels = %d, DarkPixels = %d\r\n",SaturatedPixels, DarkPixels);
	}
	return false;
}



void
HHPSD_BYD ::
ScanManagerAutoExpCalc(void)
{
	if (is_presentation())
	{
		AutoExpCalcPresentation();
	}
	else if (is_fixed())
	{
		AutoExpCalcFixed();
	}
	else
	{
		//dbg_output("auto exp\r\n");
		get_exposure_control(this, options.nAutoExposureMode)
			->calc_exposure();
		
	}
}

void
HHPSD_BYD ::
ScanManagerFrameEventAutoExposure(void)
{
	if (!is_presentation())
	{
		get_exposure_control(this, options.nAutoExposureMode)
			->frame_event();
	}

	AEStats.last_exp = AEStats.current_exp;
	AEStats.last_gain = AEStats.current_gain;
	if(!(AEStats.next_exp == 0 && AEStats.next_gain == 0))
	{
		AEStats.current_exp = AEStats.next_exp;
		AEStats.current_gain = AEStats.next_gain;
	}
	DBGM(32,)
	{
		dbg_output("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n");
		dbg_output("last exp : 0x%x, current_exp : 0x%x, next_exp : 0x%x\r\n", AEStats.last_exp, AEStats.current_exp, AEStats.next_exp);
		dbg_output("last gain: 0x%x, current_gain: 0x%x, next_gain: 0x%x\r\n", AEStats.last_gain, AEStats.current_gain, AEStats.next_gain);
	}

	resetAEStats();
}



//Kyle???TODO: it is not supported any more, should be removed
//butt it is still be used by other
BOOL Write_Frame_Blank_Time(WORD value)
{	
	return true;
}

static BOOL read_exp_gain(WORD *p_exp, WORD *p_gain)
{
	WORD exp_H,exp_L;
	
	if(!iic_shadow.ReadIIC(BYD_REG_GAIN, p_gain))
		return FALSE;

	if(!iic_shadow.ReadIIC(BYD_REG_TINT_H, &exp_H))
		return FALSE;

	if(!iic_shadow.ReadIIC(BYD_REG_TINT_L, &exp_L))
		return FALSE;
	
	*p_exp = (exp_H << 8) | exp_L;
	
	return TRUE;
}

static BOOL set_exp_gain(WORD exp, WORD gain )
{
	WORD pre_exp_H,pre_exp_L,pre_gain;

	if(!iic_shadow.ReadIIC(BYD_REG_TINT_H, &pre_exp_H))
		return FALSE;
	if(!iic_shadow.ReadIIC(BYD_REG_TINT_L, &pre_exp_L))
		return FALSE;
	
	if(pre_exp_H != (exp >> 8))
	{
		if(!iic_shadow.WriteIIC(BYD_REG_TINT_H, exp >> 8))
			return FALSE;
	}

	if(pre_exp_L != exp & 0xFF)
	{
		if(!iic_shadow.WriteIIC(BYD_REG_TINT_L, exp & 0xFF))
			return FALSE;
	}

	

	if(!iic_shadow.ReadIIC(BYD_REG_GAIN, &pre_gain))
		return FALSE;

	if(pre_gain != gain)
	{
		if(!iic_shadow.WriteIIC(BYD_REG_GAIN, gain))
			return FALSE;
	}

	DBGM(32,)
	{
		dbg_output("from exp: %x, to exp: %x\r\n", (pre_exp_H << 8) | pre_exp_L,  exp);
		dbg_output("from gain: %x, to gain: %x\r\n", pre_gain, gain);
	}
	
	return TRUE;
}


HHPSD_BYD * get_hhpsd_BYD()
{
	static HHPSD_BYD * p_hhpsd_BYD = 0;
	if (p_hhpsd_BYD == 0)
		p_hhpsd_BYD = new HHPSD_BYD();
	return p_hhpsd_BYD;
}


static
Exposure_control *
get_exposure_control(HHPSD_BYD * BYD, HHPScanDriverAutoExposureMode_t mode)
{

	if (mode == HHPSD_CELLPHONE)
	{
		static Exposure_control * p_exposure_control = 0;
		if (p_exposure_control == 0)
			p_exposure_control = new Exposure_control_cellphone(BYD);
		return p_exposure_control;
	}

	if (is_autoexposure(mode) || is_context_sensitive(mode) || is_presentation(mode))
	{
		static Exposure_control * p_auto_exposure = 0;

		if (p_auto_exposure == 0)
			p_auto_exposure = new Auto_exposure_control(BYD);

		return p_auto_exposure;
	}

	// Default case
	static Exposure_control * p_mode_default = 0;
	if (p_mode_default == 0)
		p_mode_default = new Exposure_control();
	return p_mode_default;
}




//static BOOL SetSensorMode(OperatingModesBYD_t nMode);
//POI_Sensor -- Power on initialize
void POI_Sensor(void)
{
	unsigned short nRegVal;
	unsigned char nStartingReg;
	const BYDRegisterEntry_t *pRegisterBlocks = NULL;
	int i;
	DWORD nRegisterBlocks;
	BOOL value = false;
	int ntry = 0;
 
	if (HWLAYER.EngineCurrentlyPowered() == FALSE)
	{
		iic_shadow.reset();
		
		if(HWLAYER.IsPsocSupported())
		{
			HWLAYER.WaitMilliseconds(30);           //wait for psoc reset, remove if our final decision is not to reset
		}	
		
		HWLAYER.ImagerPowerUp();            		//this function waits at least 1ms for psoc after we enable power

		if(HWLAYER.IsPsocSupported())
		{		
			//Reserved, there is no PSOC
		}
		else
		{
   			HWLAYER.WaitMilliseconds(1);
		}

		nRegisterBlocks = HWLAYER_BYD.GetRegisterSettingsForHHPExposureControl(&pRegisterBlocks);

		HWLAYER.WaitMilliseconds(20);				//wait for 20ms according timing diagram

		for (i=0; i<nRegisterBlocks; i++)
		{
			nStartingReg = pRegisterBlocks[i].nRegister;
			nRegVal = pRegisterBlocks[i].nValue;

			value = false;
			ntry = 0;
			while(!value)
			{ 
				ntry++;
				value=iic_shadow.WriteIIC(nStartingReg,nRegVal);
				if(ntry > 5)
				{
					//dbg_output("sensor register Set up failed %d!\r\n", i);	//TODO what to do if sensor init failed!!!
					return;
				}
			}
		}
		HWLAYER.WaitMilliseconds(1);
	}
}

/*-------------------------------------------------------------
 * ScanSensorMode - This function sets various 'control' registers of the sensor
 * relative mostly to init / de-init state.
 *-------------------------------------------------------------*/

BOOL SetSensorMode (OperatingModesBYD_t nMode)
{
	static OperatingModesBYD_t nCurrentMode = POWER_DOWN;
	/* if we are not changing to a different mode, return */
	if (nCurrentMode == nMode)
		return TRUE;

	/* get the register value for the mode */
	unsigned short miscel_ctrl = 0x00;

	switch (nMode)
	{
		case POWER_DOWN:
			HWLAYER.ImagerPowerDown();
			break;
	   	case POWER_UP:
			POI_Sensor();
			break;

	   	case IDLE:
		case STANDBY:
			POI_Sensor();
			//Kyle???TODO: use define like BYD_ENTER_SLEEP();
			if(iic_shadow.ReadIIC(0x09, &miscel_ctrl))
			{
			   	miscel_ctrl |= (1<<4);
				iic_shadow.WriteIIC(0x09, miscel_ctrl);
				iic_shadow.WriteIIC(0x11, 0xA0);
			}
			break;
		case TRIGGER:
			POI_Sensor();

			//Kyle???TODO: use define like BYD_WAKE_UP();
			if(iic_shadow.ReadIIC(0x09, &miscel_ctrl))
			{
			   	miscel_ctrl &= (~(1<<4));
		   		iic_shadow.WriteIIC(0x09, miscel_ctrl);
				iic_shadow.WriteIIC(0x11, 0x80);
			}
			break;

		case CLEAR:
			POI_Sensor();
			break;
	}//end switch()

	nCurrentMode = nMode;
	return TRUE;
}


BOOL ScanEngineIsBYD(void)
{
	BOOL bStatus = FALSE;

	//Kyle???TODO: here we should read BYD version num from registers
	
	bStatus = TRUE;

	if (bStatus)
	{
		pConfigs = new BYDConfig_t();
		
		static const BYTE shadow_registers[] =
		{
			BYD_REG_TINT_H,
			BYD_REG_TINT_L,
			BYD_REG_GAIN
		};
		p_iic_shadow = new IIC_Shadow(shadow_registers);
	}

	return bStatus;

}

BOOL ScanEngineFaultOccurredBYD(void)
{
	UNIMPL FALSE;
}

void AutoAimControlHighPowerBYD(DWORD /*nLineInts*/)
{
	//UNIMPL;                                                           //lmc
}
void AutoAimControlLowPowerBYD(DWORD /*nLineInts*/)
{
	//UNIMPL;                                                          //lmc
}

void cumulativeHistogram(  DWORD (&cumulative)[256], const DWORD (&histogram)[256]  )
{
	unsigned total = 0;
	for (unsigned i = 0; i < 256; ++i)
	{
		total += histogram[i];
		cumulative[i] = total;
	}
}

/** Use a cumulative histogram and calculates grey value for a the given
	percentage.

	Given N pixels, this returns the largest grey value, x such that
	floor(N * percentile / 100) pixels are less than or equal x.
*/

unsigned percentile(DWORD (&cum)[256], unsigned percentile)
{
	unsigned samples = cum[255] * percentile / 100;
	unsigned min = 0;
	unsigned max = 256;
	unsigned mid;

	// This uses a binary search to locate the value in the correct
	// index in the cumulative histogram
	for (;;)
	{
		mid = (min + max) >> 1;
		if (cum[mid] < samples)
		{
			if (min == mid)
				break;
			min = mid;
		}
		else
		{
			if (max == mid)
				break;
			max = mid;
		}
	}
	return max;
}


