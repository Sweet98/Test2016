/*------------------------------------------------------------------

*-------------------------------------------------------------------
* ScanDriverLightControl.h - This file contains the functions
* associated with controlling the illumination/aimer of a given
* imaging engine
*-------------------------------------------------------------------
* $RCSfile: ScanDriverLightControl.h $
* $Revision: 1.26 $
* $Date: 2010/12/17 17:50:52EST $
*--------------------------------------------------------------------
*--------------------------------------------------------------------*/
#ifndef _SCAN_DRIVER_LIGHT_CONTROL_H_
#define _SCAN_DRIVER_LIGHT_CONTROL_H_
#include "ScanDriverOpenContext.h"

#ifdef SD_NAMESPACE
namespace SD_NAMESPACE {
#endif


#define LIGHTS_CONTROL_MAXIMUM_ILLUM_SETTING	100
#define LIGHTS_CONTROL_MINIMUM_ILLUM_SETTING	1

void LightControlInitialize(void);
BOOL LightControlIlluminationState(ScanDriverOpenContext_t *pOpenContext,BOOL bState);
BOOL LightControlAimerState(ScanDriverOpenContext_t *pOpenContext,BOOL bState);
BOOL LightControlIlluminationIntensity(ScanDriverOpenContext_t *pOpenContext,DWORD nValue);
BOOL LightControlAimerIntensity(ScanDriverOpenContext_t *pOpenContext,DWORD nValue);
BOOL LightControlEnableGlobalControl(void);
BOOL LightControlEnableIndividualControl(ScanDriverOpenContext_t *pOpenContext);

BOOL LightControlAutomaticAimerAdjustment(DWORD nNewAimerValue);
BOOL LightControlAutomaticIlluminationAdjustment(DWORD nNewAimerValue);
void LightControlAutomaticAimerControl(DWORD nLines);
BOOL LightControlAutomaticAimerControl(ScanDriverOpenContext_t *pOpenContext,BOOL bState);
BOOL LightControlSetScanMode(ScanDriverOpenContext_t *pOpenContext, DWORD mode);
BOOL LightControlSetIdleMode(ScanDriverOpenContext_t *pOpenContext, DWORD mode);
BOOL LightControlSetIlluminationSource(ScanDriverOpenContext_t *pOpenContext, DWORD source);

BOOL LightControlFlashCurrent( bool bEnableFlashCurrent, int tint = 1, bool bAllowDeferredBoost = true );

ScanDriverOpenContext_t *GetOpenContextEntry(ScanDriverOpenContext_t *pStart);
void SetupAutoAimControlFunctions(void (*pHigh)(DWORD),void (*pLow)(DWORD));
void GetCurrentLightsStates(BOOL *pIllState,BOOL *pAimState);
BOOL LightControlSetTags(  ScanDriverOpenContext_t *pOpenContext,
						   const DWORD * pBuffIn, DWORD buffSize  );
BOOL LightControlGetTags(  ScanDriverOpenContext_t *pOpenContext,
						   const DWORD * pBuffIn, DWORD *pBuffOut, 
						   DWORD buffSize  );
void LightControlOpenContextChanged(ScanDriverOpenContext_t *pOpenContext);

void ForceCellphone(bool cellphone_flag);
bool GetForceCellphone();

void PsocWriteFlashCurrent( DWORD nLineISTnum );
void OemHandleCellPhoneLights( DWORD nLineISTnum );
void SaveLastExposureSetForFlashControl( DWORD tint );


struct PSOC_current_reg
{
public:
	PSOC_current_reg(unsigned _op_current_reg, unsigned _max_current_reg)
		: is_initialized(false),
		  op_current_reg(_op_current_reg), 
		  max_current_reg(_max_current_reg), 
		  op_current(0),
	max_current(0)
		{
		}
	
	void setIntensity(unsigned intensity);
	
protected:
	bool  is_initialized;
	const BYTE op_current_reg;
	const BYTE max_current_reg;
	BYTE op_current;
	BYTE max_current;
};

struct PSOC_current_reg_with_flash : public PSOC_current_reg
{
public:
	PSOC_current_reg_with_flash(  unsigned _op_current_reg, 
								  unsigned _max_current_reg, 
								  unsigned _flash_current  )
		: PSOC_current_reg(_op_current_reg, _max_current_reg),
		  flash_current(_flash_current)
		{
		}
	void setFlashIntensity();

protected:
	const BYTE flash_current;
};

class HW_light_control
{
 public:
	virtual BOOL setHwLightMode(  DWORD lm, DWORD nIllVal, 
								  DWORD nAimVal, DWORD illumSource = HHPSD_IS_PRIMARY  ) = 0;

	virtual bool flashCurrent( bool /* bEnableFlashCurrent */, int /* tint */ ){return true;}

	virtual bool flashCurrent( bool /* bEnableFlashCurrent */ ){return true;}	
};

class HW_light_control_primitive : public HW_light_control
{
 public:
	virtual BOOL setHwLightMode(  DWORD lm, DWORD nIllVal, 
								  DWORD nAimVal, DWORD illumSource  );
};

class HW_light_control_STMVC700 : public HW_light_control
{
 public:
	HW_light_control_STMVC700();
	virtual BOOL setHwLightMode(  DWORD lm, DWORD nIllVal, 
								  DWORD nAimVal, DWORD illumSource  );

	void setRedIllumIntensity(unsigned intensity);

private:
	PSOC_current_reg illum_current;
};

class HW_light_control_MT9V022 : public HW_light_control
{
 public:

	HW_light_control_MT9V022();
	virtual BOOL setHwLightMode(  DWORD lm, DWORD nIllVal, DWORD nAimVal, DWORD illumSource  );
	void resetLights();

	virtual bool flashCurrent(bool bEnableFlashCurrent);
	virtual bool flashCurrent( bool /* bEnableFlashCurrent */, int /* tint */ ){return true;}

	void restoreLedMode();

	BOOL boostOn() const;

	void setLedMode(unsigned ledMode, unsigned ledModeMask);

 private:

	void setIllum(bool bOn);
	void setAim(bool bOn);

	void setRedIllumIntensity(unsigned intensity);
	void setGreenIllumIntensity(unsigned intensity);
	void setAimIntensity(unsigned intensity);

	bool illumState;
	bool aimState;
	BYTE psocLedModeReg;

	DWORD current_mode;
	PSOC_current_reg_with_flash red_current;
	PSOC_current_reg green_current;
	PSOC_current_reg aim_current;
};

class HW_light_control_OEM_Jade : public HW_light_control
{
 public:

	HW_light_control_OEM_Jade();
	virtual BOOL setHwLightMode(  DWORD lm, DWORD nIllVal, DWORD nAimVal, DWORD illumSource  );
	void resetLights();

	virtual bool flashCurrent(bool bEnableFlashCurrent, int tint);
	virtual bool flashCurrent( bool /* bEnableFlashCurrent */ ){return true;}

	void restoreLedMode();

	int boostOn() const;

	int CurrentBoostIntensityLevel() const;

	int CurrentBoostIntensityPercentLevel() const;

	int SelectBoostIntensityLevel( int tint );

	int FindBoostIntensityLevel( int intens );
	
	int SelectHighBoostIntensityLevel( int tint );

	void setLedMode(unsigned ledMode, unsigned ledModeMask);

	inline void SetBoostDeferred(){boostUp = true; psocLedModeRegCurrent = psocLedModeReg; boostUpPending = false;};

	inline void ClearBoostDeferred(){boostUp = false; psocLedModeRegCurrent = 0; psocLedModeRegNext = 0; boostUpPending = false;};

	void WriteFlashCurrent( DWORD nLineISTnum );

	void HandleCellPhoneLights( DWORD nLineISTnum );

	bool SetUpCellPhoneLights( DWORD cellphone_flag, DWORD lightMode, DWORD nIllVal, DWORD nAimVal );
 
 private:

	void setIllum(bool bOn);
	void setAim(bool bOn);

	void setRedIllumIntensity(unsigned intensity);
	void setGreenIllumIntensity(unsigned intensity);
	void setAimIntensity(unsigned intensity);

	bool illumState;
	bool aimState;
	BYTE psocLedModeReg;

	DWORD current_mode;
	PSOC_current_reg_with_flash red_current;
	PSOC_current_reg green_current;
	PSOC_current_reg aim_current;

	bool boostUp;
	bool boostUpPending;
	BYTE psocLedModeRegNext;
	BYTE psocLedModeRegCurrent;

	DWORD m_cellPhoneFlagMode;
	DWORD m_lightMode; 
	DWORD m_IllVal;
	DWORD m_AimVal;
	DWORD m_nLineISTnum;                
};

#ifdef SD_NAMESPACE
} // namespace SD_NAMESPACE {
#endif


#endif //_SCAN_DRIVER_LIGHT_CONTROL_H_

