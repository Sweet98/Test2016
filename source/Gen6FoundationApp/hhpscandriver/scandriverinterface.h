/*-------------------------------------------------------------------

*--------------------------------------------------------------------
* ScanDriverInterface.h - This is the header file that an application
* using the scan driver includes
*--------------------------------------------------------------------
* $RCSfile: scandriverinterface.h $
* $Revision: 1.59.4.2 $
* $Date: 2011/09/02 08:14:22EDT $
*--------------------------------------------------------------------*/

#ifndef _SCAN_DRIVER_INTERFACE_H_
#define _SCAN_DRIVER_INTERFACE_H_
// These are the valid IOCTL settings that can be used by callers to the
// driver.

#ifndef HHP_IMAGER_EXPOSURE_TAG_DEF
    #define HHP_IMAGER_EXPOSURE_TAG_DEF
    typedef enum
    {
	    ES_EXPOSURE_METHOD,		/* How should we do this */
    	ES_TARGET_VALUE,		/* This is the target pixel count we shoot for */
	    ES_TARGET_PERCENTILE,	/* This goes along with Value, the target value should be at this percentile */
    	ES_TARGET_ACCEPT_GAP,	/* How close to the target value must we be */
	    ES_MAX_EXP,				/* This is the maximum exposure we're allowed to use */
    	ES_MAX_GAIN,			/* This is the maximum gain we're allowed to use */
	    ES_STARTING_EXP,		/* When scanning starts, this is the first exposure that we try (updated when scanning stops)*/
    	ES_STARTING_GAIN,		/* When scanning starts, this is the first gain that we try (updated when scanning stops)*/
	    ES_FRAME_RATE,			/* This is essentially the frame rate to use, I call it divide because that's what it does */
    	ES_CONFORM_IMAGE,		/* The image must conform to the auto-exposure requirements, if not, it's rejected. */
	    ES_CONFORM_TRIES,		/* The number of times we'll attempt to conform. */
    	ES_SPECULAR_EXCLUSION,	/* Do we exclude specular? to what degree? */
	    ES_SPECULAR_SAT,		/* These next two only come into play if somebody specifies nSpecularExclusion*/
    	ES_SPECULAR_LIMIT,		/* to be HHPSD_SPECULAR_EXCLUSION_SPECIAL. */
	    ES_FIXED_EXP,
    	ES_FIXED_GAIN,
	    ES_FIXED_FRAME_RATE,
		 ES_ADJUST_EXP_WHEN_ILLUM_OFF,
		ES_DECODE_PARAMETER,
		ES_FAST_START,
		ES_EXPOSURE_CELL,		/* This is the % of max exposure for cell */
        ES_NUM_EXPOSURE_ITEMS
    } HHP_IMAGER_EXPOSURE_TAG;
#endif

typedef enum
{
	LI_SCAN_MODE,
	LI_IDLE_MODE,
	LI_ILLLUMINATION_SOURCE,
	LI_ILLUMINATION_INTENSITY,
	LI_AIMER_INTENSITY,
	LI_ILLUMINATION_CONTROL
} HHP_IMAGER_LIGHTING_TAG;

enum HHPScanDriverLightMode_t
{
	HHPSD_LM_OFF             = 0,
	HHPSD_LM_AIM_ON          = 1,
	HHPSD_LM_ILLUM_ON        = 2,
	HHPSD_LM_ILLUM_ON_AIM_ON = (HHPSD_LM_ILLUM_ON | HHPSD_LM_AIM_ON),
	HHPSD_LM_CONCURRENT      = 4,
	HHPSD_LM_INTERLACED      = 5,
	HHPSD_LM_FLASH_deprecated = 6,
	HHPSD_LM_BLINK           = 7,
	HHPSD_LM_AIM_ONLY_NON_EXPOSURE = 8,
	HHPSD_LM_NUM_MODES // Must be last.
};

enum HHPScanDriverIlluminationSource_t
{
	HHPSD_IS_PRIMARY       = 0,
	HHPSD_IS_ALTERNATE      = 1
};


typedef enum {
	HHPSD_IOCTL_START_SCANNING,
	HHPSD_IOCTL_STOP_SCANNING,
	HHPSD_IOCTL_GET_NEW_SCAN,
	HHPSD_IOCTL_UNREGISTER_SCAN_BUFFER,
	HHPSD_IOCTL_REGISTER_SCAN_BUFFER,
	HHPSD_IOCTL_GET_SCAN_HEIGHT,
	HHPSD_IOCTL_GET_SCAN_WIDTH,
	HHPSD_IOCTL_SET_ILLUMINATORS_STATE,
	HHPSD_IOCTL_SET_AIMER_STATE,
	HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,
	HHPSD_IOCTL_GET_EXPOSURE_SETTINGS,
	HHPSD_IOCTL_RETURN_SCAN,
	HHPSD_IOCTL_SET_SCAN_LIGHTS_MODE,
	HHPSD_IOCTL_GET_ENGINE_TYPE,
	HHPSD_IOCTL_SET_EXPOSURE_MODE,
	HHPSD_IOCTL_SET_ILLUMINATOR_INTENSITY,
	HHPSD_IOCTL_SET_AIMER_INTENSITY,
	HHPSD_IOCTL_NEW_IMAGE_AVAILABLE,
	HHPSD_IOCTL_PREVENT_AIMER_IN_IMAGES,
	HHPSD_IOCTL_GET_IMAGE_ATTRIBUTES,
	HHPSD_IOCTL_RECONFIG_ENGINE,
    HHPSD_IOCTL_SINGLE_FRAME_CAPTURE,
	HHPSD_IOCTL_GET_REVISION_STRING,
	HHPSD_IOCTL_GET_IMAGER_PROPERTIES,
	HHPSD_IOCTL_POWER_OFF_IMAGER,
	HHPSD_IOCTL_SET_ENGINE_CONFIG,
	HHPSD_IOCTL_GET_ENGINE_CONFIG,
	HHPSD_IOCTL_WRITE_PSOC_REG,
	HHPSD_IOCTL_READ_PSOC_REG,
	HHPSD_IOCTL_WRITE_PSOC,
	HHPSD_IOCTL_READ_PSOC,
	HHPSD_IOCTL_ENGINE_DIAGNOSTIC_MODE,
	HHPSD_IOCTL_ENGINE_HAS_PSOC,
	HHPSD_IOCTL_SET_LIGHT_SETTINGS,
	HHPSD_IOCTL_GET_LIGHT_SETTINGS,
	HHPSD_IOCTL_SCANNING_ENABLED,
	HHPSD_IOCTL_GET_EXPOSURE_MODE,
	HHPSD_IOCTL_UPDATE_PSOC_REG,                 							 //Test Flex additions
	HHPSD_IOCTL_RESET_PSOC,                                            //Test Flex additions
	HHPSD_IOCTL_RELOAD_PSOC_REGS_COPY,                                 //Test Flex additions
	HHPSD_IOCTL_SET_IMAGER_GRAYSCALE,                                  //Test Flex additions
	HHPSD_IOCTL_UNSET_IMAGER_GRAYSCALE,                                //Test Flex additions
	HHPSD_IOCTL_ATTACH_TO_SAME_BUFFER
}HHPScanDriverIOCTL_t;

typedef enum {
	HHPSD_EXPOSURE_FIXED,
	HHPSD_AUTOEXPOSURE_USE_ONCHIP,
	HHPSD_AUTOEXPOSURE_USE_HHP,
	HHPSD_AUTOEXPOSURE_HHP_PRESENTATION,
	HHPSD_CONTEXT_SENSITIVE_USE_HHP,
	HHPSD_OPENLOOP_GAIN,
	HHPSD_CELLPHONE,
	HHPSD_AUTOEXPOSURE_DUAL_TRACK_USE_HHP,
	HHPSD_CONTEXT_SENSITIVE_DUAL_TRACK_USE_HHP,
}HHPScanDriverAutoExposureMode_t;

typedef enum {
	HHPSD_ILLUM_AIMER_OFF,
	HHPSD_ILLUM_ONLY_ON,
	HHPSD_AIMER_ONLY_ON,
	HHPSD_ILLUM_AIMER_ON
} HHPScanDriverIlluminationStates_t;

typedef enum {
	HHPSD_AE_METHOD_UNIFORM,
	HHPSD_AE_METHOD_CENTER_ONLY,
	HHPSD_AE_METHOD_CENTER_WEIGHTED
} HHPScanDriverHHPExposureMethod;

typedef enum {
	HHPSD_SPECULAR_EXCLUSION_OFF,
	HHPSD_SPECULAR_EXCLUSION_MINIMAL,
	HHPSD_SPECULAR_EXCLUSION_MODERATE,
	HHPSD_SPECULAR_EXCLUSION_AGGRESSIVE,
	HHPSD_SPECULAR_EXCLUSION_SPECIAL
} HHPScanDriverHHPExposureSpecularExclusion;

typedef enum {
	HHPSD_ENGINE_IMAGER_OV7120,
	HHPSD_ENGINE_IMAGER_ICMEDIA,
	HHPSD_ENGINE_LINEAR_IMAGER_TOSHIBA,
	HHPSD_ENGINE_IMAGER_ALD_ICMEDIA,
	HHPSD_ENGINE_IMAGER_MI1300,
	HHPSD_ENGINE_IMAGER_STM_VC700,
	HHPSD_ENGINE_IMAGER_STM_VC602,
	HHPSD_ENGINE_IMAGER_MICRON_MT9V022,
	HHPSD_ENGINE_IMAGER_E2V_JADE,
	HHPSD_ENGINE_IMAGER_E2V_BYD
} HHPScanDriverEngineType_t;

typedef enum {
	HHPSD_IMAGE_STAT_EXPOSURE,
	HHPSD_IMAGE_STAT_GAIN,
	HHPSD_IMAGE_STAT_ILLUM_VALUE,
	HHPSD_IMAGE_STAT_ILLUM_MAXIMUM,
	HHPSD_IMAGE_STAT_ILLUM_CLIP,
	HHPSD_IMAGE_STAT_HEIGHT,
	HHPSD_IMAGE_STAT_WIDTH,
	HHPSD_IMAGE_STAT_IMAGE_NUMBER,
	HHPSD_IMAGE_STAT_IQ_SCORE,
	HHPSD_IMAGE_STAT_DECODE_PARAMETER = ES_DECODE_PARAMETER
} HHPScanDriverImageAttributes_t;


typedef enum {
	IP_SIZE,
	IP_ENGINE_ID,
	IP_NUM_ROWS,
	IP_NUM_COLS,
	IP_BITS_PER_PIXEL,
	IP_ROTATION,
	IP_AIMER_X_OFFSET,
	IP_AIMER_Y_OFFSET,
	IP_YDEPTH,
	IP_PIXEL_CLOCK,
	IP_PSOC_MINOR_REV,
	IP_PSOC_MAJOR_REV
} HHPScanDriverImagerProperties_t;


typedef enum {
	HHPSD_ILLUMINATION_TYPE, /* Depricated.  Use HHPSD_IOCTL_SET_ILLUMINATION_SOURCE instead. */
	HHPSD_LED_CONTROL_MODE_OBSOLETE, /* THESE ARE NOT OBSOLETE BECAUSE OEMs USE THEM */
	HHPSD_PIXEL_CLOCK_FREQUENCY_OBSOLETE,  /* THESE ARE NOT OBSOLETE BECAUSE OEMs USE THEM */
	HHPSD_ENGINE_ID,
	HHPSD_ENGINE_FW_CKSUM,
	HHPSD_ENGINE_FW_VERSION,
	HHPSD_ENGINE_AIMER_CENTER_X,
	HHPSD_ENGINE_AIMER_CENTER_Y,
	HHPSD_ENGINE_SENSOR_ID,
	HHPSD_NUM_ENGINE_CFG_ITEMS
} HHPScanDriverEngineConfiguration_t;


// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the IMAGER_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// IMAGER_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

#ifdef _WIN32_WCE
#ifdef HHPSCANDRIVER_EXPORTS
#define DRIVER_API __declspec(dllexport)
#else
#define DRIVER_API __declspec(dllimport)
#endif
#else
#define DRIVER_API
#endif


//
// Declare the external entry points here. Use declspec so we don't
// need a .def file.  Bracketed with extern C to avoid mangling in C++.
//
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
DRIVER_API DWORD SCN_Init (DWORD dwContext);
DRIVER_API BOOL  SCN_Deinit (DWORD dwContext);
DRIVER_API DWORD SCN_Open (DWORD dwContext, DWORD dwAccess,
									  DWORD dwShare);
DRIVER_API BOOL  SCN_Close (DWORD dwOpen);
DRIVER_API DWORD SCN_Read (DWORD dwOpen, LPVOID pBuffer,
									  DWORD dwCount);
DRIVER_API DWORD SCN_Write (DWORD dwOpen, LPVOID pBuffer,
									   DWORD dwCount);
DRIVER_API DWORD SCN_Seek (DWORD dwOpen, long lDelta,
									  WORD wType);
DRIVER_API BOOL SCN_IOControl(DWORD hOpenContext, DWORD dwCode, PBYTE pBufIn, DWORD dwLenIn,
						PBYTE pBufOut, DWORD dwLenOut, PDWORD pdwActualOut);
DRIVER_API void SCN_PowerDown (DWORD dwContext);
DRIVER_API void SCN_PowerUp (DWORD dwContext);
#ifdef __cplusplus
} // extern "C"
#endif //__cplusplus

#endif // _SCAN_DRIVER_INTERFACE_H_
