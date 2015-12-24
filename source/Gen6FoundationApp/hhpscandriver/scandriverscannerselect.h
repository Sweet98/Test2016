/*-------------------------------------------------------------------

 *-------------------------------------------------------------------
 * ScanDriverScannerSelect.h - This is the header file for the
 *   software which will select which scanner/imager is connected and
 *   configure the hardware/software accordingly.  This file also
 *   contains links to the global function variables which will be
 *   linked to the functions associated with the appropriate
 *   scanner/imager
 *-------------------------------------------------------------------
 * $RCSfile: scandriverscannerselect.h $
 * $Revision: 1.45.1.6 $
 * $Date: 2010/10/20 23:30:22EDT $
 *-------------------------------------------------------------------*/
#ifndef _SCAN_DRIVER_SCANNER_SELECT_H_
#define _SCAN_DRIVER_SCANNER_SELECT_H_

#include "scandriveropencontext.h"
#include "scandriverhwlayer.h"
#include "scandriverhwlayerHi704A.h"
#include "scandriverlightcontrol.h"

#ifdef __cplusplus
extern "C" {
#endif

class HHPSD_interface
{
 public:
	HHPSD_interface() : pLightControl(0) { }
	void setLightController(HW_light_control * _pLightControl) { pLightControl = _pLightControl; }
	HW_light_control * getLightController() { return pLightControl; }

	virtual HardwareLayerInterface * GetHardwareLayerInterface() { return 0; } // !! Make pure
	virtual void ScanManagerAutoExpLineSample(void *pImage,DWORD nRowsComplete) = 0;
	virtual BOOL ScanManagerImageIsAcceptable(ScanDriverLastScanInfo_t *pStats) = 0;
	/** This is called by the scan manager during the update cycle.
		The scan manager uses this to determine if it is ok to capture
		another frame.

		If scanning has shutdown or will shutdown because the last
		image was conforming and we're configured to automatically
		shutdown on a conforming image, then this member should return
		FALSE.
	*/
	virtual DWORD ScanManagerEngineSaysOkToCaptureNextFrame(void) = 0;
	virtual DWORD ScanEngineFaultOccurred(void)= 0;
	virtual void ScanManagerAutoExpCalc(void) = 0;

/*#if defined(OEM_ENGINE)
	virtual void ScanManagerAutoPSOCCurrentCalc(void) = 0;
#endif*/

	virtual DWORD ScanEngineRestart(ScanDriverOpenContext_t *)= 0;
	/** This is called by the scan manager to signal that the Vsync or
		end of frame has occurred.
	 */
	virtual void ScanManagerFrameEventAutoExposure(void) = 0;
	virtual void ExposureSettingsUpdate(ScanDriverOpenContext_t *pOptions) = 0;
	virtual void LightSettingsUpdate(const ScanDriverOpenContext_t *pOptions) = 0;
	virtual DWORD ResetEngine(void)= 0;
	virtual DWORD InitializeScanEngine(ScanDriverOpenContext_t *,BOOL)= 0;
	/** This is called by the the scan manager when scanning starts
	   up.  On most sensors, this member does nothting.  However, on
	   the STM_VC700 this member sets the sensors operating mode in RUN mode.
	*/
	virtual void  ScanEngineStart(void) {}
	virtual DWORD DeinitializeScanEngine(ScanDriverOpenContext_t *,BOOL)= 0;
	virtual DWORD ScanEngineSaveStateAndPowerDown(void) = 0;
	virtual DWORD ScanEnginePowerUpAndRestoreState(void) = 0;
	virtual DWORD ScanEnginePowerUpAndRestoreRegs(void) = 0;
	virtual DWORD SetDefaultScannerOptions(ScanDriverOpenContext_t *)= 0;
	virtual BOOL setHwLightMode(  DWORD lm, DWORD nIllVal,
								  DWORD nAimVal, BOOL alt_color = FALSE  )
		{
			if (pLightControl)
				return pLightControl->setHwLightMode(lm, nIllVal, nAimVal, alt_color);
			return FALSE;
		}

	virtual void FrameBoundary() {}

 private:
	HW_light_control * pLightControl;
};

extern HHPSD_interface * p_hhpsd_interface;
extern HardwareLayerInterface * p_hardware_layer_interface;
//extern ICMediaHardwareLayerInterface * p_icmedia_hardware_layer_interface;                  //lmc
//extern OV7120HardwareLayerInterface * p_ov7120_hardware_layer_interface;                    //lmc
//extern MT9V022HardwareLayerInterface *p_mt9v022_hardware_layer_interface;                   //lmc
#ifdef HI2D_T
#ifdef BYD_SENSOR
extern BYDHardwareLayerInterface *p_BYD_hardware_layer_interface;
#else
extern Hi704AHardwareLayerInterface *p_Hi704A_hardware_layer_interface;
#endif
#else
extern JadeHardwareLayerInterface *p_jade_hardware_layer_interface;
#endif

#ifdef DOXYGEN
HHPSD_Jade HHPSD;
HardwareLayerInterface HWLAYER;
//ICMediaHardwareLayerInterface HWLAYER_ICMEDIA;                                            //lmc
//OV7120HardwareLayerInterface HWLAYER_OV7120;                                              //lmc
//MT9V022HardwareLayerInterface HWLAYER_MT9V022;                                             //lmc
JadeHardwareLayerInterface HWLAYER_JADE;
#else
#define HHPSD (*p_hhpsd_interface)
#define HWLAYER (*p_hardware_layer_interface)
//#define HWLAYER_ICMEDIA (* p_icmedia_hardware_layer_interface)                            //lmc
//#define HWLAYER_OV7120 (* p_ov7120_hardware_layer_interface)                              //lmc
//#define HWLAYER_MT9V022 (* p_mt9v022_hardware_layer_interface)                            //lmc
#ifdef HI2D_T
#ifdef BYD_SENSOR
#define HWLAYER_BYD    (*p_BYD_hardware_layer_interface)
#else
#define HWLAYER_JADE    (*p_Hi704A_hardware_layer_interface)//TODO HI2D reuses JADE's temporarily
#endif	//end #ifdef BYD_SENSOR
#else
#define HWLAYER_JADE    (* p_jade_hardware_layer_interface)
#endif
#endif

//extern BOOL	(*fReadIIC_STM_VC700)(unsigned char, unsigned char *, unsigned);                   //lmc
//extern BOOL	(*fWriteIIC_STM_VC700)(unsigned char, const unsigned char *, unsigned);            //lmc
//extern BOOL	(*fImagerPowerUpSTM_VC700)(void);
//extern BOOL	(*fImagerPowerDownSTM_VC700)(void);
//extern void	(*fWaitMillisecondsSTM_VC700)(DWORD);
//extern DWORD	(*fGetRegisterSettingsForHHPExposureControlSTM_VC700)(void *);                  //lmc
//extern DWORD (*fGetRegisterSettingsForHHPExposureControlSTM_VC602)(void *);                  //lmc

//extern DWORD (*fGetNumberOfValidPixelsToshiba)(void);                                       //lmc
//extern DWORD (*fChangeExposureToshiba)(DWORD);                                              //lmc
//extern DWORD (*fImagerPowerUpToshiba)(void);
//extern DWORD (*fImagerPowerDownToshiba)(void);
//extern DWORD (*fGetReferenceBlackValueToshiba)(void);                                       //lmc
//extern DWORD (*fSetDCOffsetToshiba)(DWORD val);                                              //lmc
//extern DWORD (*fRequestAimToshiba)(DWORD);                                                   //lmc
//extern DWORD (*fRequestIllumToshiba)(DWORD);                                                 //lmc
//extern DWORD (*fGetNumberOfTotalLineCallbacks)(void);
//extern DWORD (*fVC700EngineCurrentlyPowered)(void);

extern void VoidArgumentVoidReturn_Dummy(void);



BOOL AutoDetectScanEngine(void);
void UnloadHardwareLibrary(void);

#ifdef __cplusplus
}
#endif

extern DWORD (*HHPSD_ScanEnginePowerDown)(void);
extern DWORD (*HHPSD_ScanEngineWaitMs)(DWORD);

#endif
