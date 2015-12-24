//======================================================================================
// ImagingMenuSettings.h
//======================================================================================
// $RCSfile: ImagingMenuSettings.h $
// $Revision: 1.32 $
// $Date: 2011/11/10 04:44:07EST $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Module Interface Description:
//      This file contains the interface definition to a routine that creates
//		imaging menu settings as part of the overall reader configuration for
//		a Matrix device.  It also contains the active value declarations of
//		imaging settings.
//
//---------------------------------------------------------------------------


#ifndef ImagingMenuSettingsH
#define ImagingMenuSettingsH
//---------------------------------------------------------------------------


#include	"language.h"					// system global definitions

#ifdef BYD_SENSOR

#define EXPEST_JADE			BYD_INT_MAX
#define EXPEMX_JADE			BYD_INT_MAX
#define EXPSNP_JADE			BYD_INT_MAX 
#define EXPEST_JADE_MANUAL	BYD_INT_MAX          
#define EXPEMX_JADE_MANUAL	BYD_INT_MAX 


#else

#define EXPEST_JADE	60          //60 * 128 uS  =  8 ms, to allow for half of frame to be for aimer
#define EXPEMX_JADE	63          //63 * 128 uS  =  8 ms, to allow for half of frame to be for aimer
#define EXPSNP_JADE	7800 			//7874 			// 128 uS * 7800, Exposure limit of ~1000ms.
#define EXPEST_JADE_MANUAL	4          //4 * 128 uS  =  500 Us, DOES NOT allow for half of frame to be for aimer
#define EXPEMX_JADE_MANUAL	100          //100 * 128 uS  =  12.8 ms, DOES NOT allow for half of frame to be for aimer

#endif
bool			AutoExposureReport;
int			MaximumGain;
int			MaximumExposure;
int         MinimumExposure;
int			PresentationExposure;
int			PresentationGain;
int			StartingGain;
int			StartingExposure;
int 			DigitalGain;
int			TargetWhite;
int			AldTargetWhite;
int			AldMaximumGain;
int			AldStartingGain;
int			ExposureMethod;
int      	SpecularDetection;
int			TargetPercentile;
//bool 		bImageMustConform;      //was bool, changed to an int below, need to change the 'b'
int	 		bImageMustConform;
int			iImagerResetTime;
int  			ImagerIdlePowerLevel;
int 			ExposureDCOffset;
int         ExpDecodeAttempts;
int			FixedExposure;
int			FLODelayUserSetting;
int			FixedGain;
int   		MaximumGainAuto;
int			MaximumExposureAuto;
int			PresentationFixedExposure;
int			PresentationFixedGain;
bool			EnConnecttoExt;
#ifdef VUQUEST3310
int			PresentationIdleExposure;
int			PresentationIdleGain;
#endif
//	int ExposureZone;
// likely temp setting for development version only
#define MICRON_INI_SENSOR		0
#define MICRON_INI_1311 		1
#define MICRON_INI_1300 		2
#define MICRON_INI_RESET_751 	3

	bool 	CompandingMode;
	bool 	RisingEdge;
	int 	MicronInitTable;
	int 	MicronI2CAddress;
	int 	ExposureMode;                    //Exposure mode for manual triggers
	int	ExposureModeStreaming;           //Exposure mode for streaming Pres trigger
	int	ExposureModePresLowLights;       //Exposure mode for Pres low lights trigger
	int	ExposureModePresNoLights;        //Exposure mode for Pres No lights trigger
	bool  AutoExposureLightsOffAdjust;


	int		ImageEngineType;	// Filled in from scan driver
	int		EngineIlluminationColor;

	int		EnginePixelClockFrequency;

	int		I2CReadTimeout;

	bool	EngineCommandPrivilege;
	bool	EngineFirmwareDiagnosticMode;

	bool	EngineFirmwareUpdateVerify;
	bool	PSOCVSSetting;				// Not used anymore, but is here because PSOC download tool wants to see it.

#if defined (DEVELOPMENT_SETTINGS)
	int EngineLoopType;
#endif

//	CCD Scan Control Settings
	int 	ScanZone;
//	bool	bLightsLowPowerMode;
//	bool	bAimerOffInImage;
	int	ScanAimerDelay;
	int   ScanAimerIdleTimeout;
	int	ScanAimerDuration;
	int   IllOnDuringDecode;
	int	ScanAutoAimer;
#define AIM_OFF 	0
#define AIM_BOTH	1
#define AIM_ALT		2
#define AIM_CONTINUOUS	3

	int	ScanAimerMode;
	int	CellScanAimerMode;
    int AimerOnTimeforCellPhoneMode;

#if 0
	#define	SCN_PERF_67HI		0
	#define	SCN_PERF_135HI		1
	#define	SCN_PERF_270HI		2

	int		ScanPerformance;

	#define	SCN_RATE_67			0
	#define	SCN_RATE_135		1
	#define	SCN_RATE_270		2

	int		ScanFlashRate;
	bool	ScanHighPowerLED;
#endif

#if !defined( RFBASE ) && !defined( RFSCAN )
	bool	IlluminationAlwaysOn;
#endif

// Force the use of non-ALD engine where it would otherwise be ignored.
bool	bUseNonALDEngine;



int 	LedIntensityPercentage; 			//For Pres mode only, really a percentage of duty cycle supplied ~ translated to almost linear current control through current source to either set of LEDs
int	NonOpticalTrigLedIntensityPercentage;  // Non optical trigger (not Presentation) version of above,  really a percentage of duty cycle supplied ~ translated to almost linear current control through current source to either set of LEDs
int 	Hi2dPresIdleCurrent;//for HI2D trgmod3 idle current
// Replace conditional compile fixed image parameters with a define with some absolute limit --for now 2M*2MB -- all to be automatically sorted on init by scan driver information
	#define ABSOLUTE_IMAGE_MAX_COL 		(2*1024)
	#define ABSOLUTE_IMAGE_MAX_ROW		(2*1024)

/*These are hear for compaitiblity with readerconfig for product config */
#ifdef BYD_SENSOR
 	#define	IMAGE_MAX_COL				640 // ENABLE 752 when working with a MICRON MT9V022 wide VGA (i.e. IT4000new)
 	#define	IMAGE_MAX_ROW				480
#endif

	int		ImgRowNumberOfPixels;
	int		ImgColNumberOfPixels;
	int		ImgRowNumberOfPixelsMinus1;
	int		ImgColNumberOfPixelsMinus1;
	int		ImgBitsPerPixel;


	int		PDF_SigCapRowHeightToWidth;
	int		Lin_HeightToNarrowElementWidth;

   int RowNoiseTopRatio;
   int RowNoiseBottomRatio;

#if !defined( RFBASE ) || defined( DEVELOPMENT_SETTINGS )
	bool	ImageTestHorizontalRoll;
	int		ImageTestXDimension;
	int		ImageTestYDimension;
	bool		UseMiddleOfImage;
#endif


// Imager settings.
	#define	JPEG_QUALITY_MINIMUM		0
	#define	JPEG_QUALITY_MAXIMUM		100
	#define	JPEG_QUALITY_DEFAULT		50

	int		JPEGQualityFactor;

	bool	InvertImageAboutXAxis;
	bool	InvertImageAboutYAxis;

	int		TransmitImageWindowLeft;
	int		TransmitImageWindowRight;
	int		TransmitImageWindowTop;
	int		TransmitImageWindowBottom;

	#define	TRANSMIT_IMAGE_SUBSAMPLE_MIN	1
	#define	TRANSMIT_IMAGE_SUBSAMPLE_MAX	10

	int		TransmitImageSubsampleIncrement;

	#define	TRANSMIT_IMAGE_NO_PROTOCOL_INFO_AT_END			0
	#define	TRANSMIT_IMAGE_DOLPHIN							1
	#define	TRANSMIT_IMAGE_NO_PROTOCOL						2
	#define	TRANSMIT_IMAGE_HMODEM_COMPRESSED_PROTOCOL		3
	#define	TRANSMIT_IMAGE_HMODEM_UNCOMPRESSED_PROTOCOL		4

	int		TransmitImageProtocol;

	#define	JPEG_COMPRESSION_SMOOTHING_FACTOR_MINIMUM		0
	#define	JPEG_COMPRESSION_SMOOTHING_FACTOR_MAXIMUM		100
	#define	JPEG_COMPRESSION_SMOOTHING_FACTOR_DEFAULT		0

	int		JPEGCompressionSmoothingFactor;
	bool		JPEGDecompressionBlockSmoothing;

	#define	TRANSMIT_IMAGE_FORMAT_KIM						0
	#define	TRANSMIT_IMAGE_FORMAT_TIF_BINARY				1
	#define	TRANSMIT_IMAGE_FORMAT_TIF_GRP					2
	#define	TRANSMIT_IMAGE_FORMAT_TIF_GRAYSCALE			3
	#define	TRANSMIT_IMAGE_FORMAT_BINARY					4
	#define	TRANSMIT_IMAGE_FORMAT_GRAYSCALE					5
	#define	TRANSMIT_IMAGE_FORMAT_JPEG						6
	#define	TRANSMIT_IMAGE_FORMAT_OUTLINED_IMAGE			7
	#define	TRANSMIT_IMAGE_FORMAT_BMP						8
 	#define 	TRANSMIT_IMAGE_FORMAT_LOSSLESS_HUFFMAN		9
 	#define	TRANSMIT_IMAGE_FORMAT_TIF_COLOR_COMPRESSED	10
	#define	TRANSMIT_IMAGE_FORMAT_TIF_COLOR_UNCOMPRESSED	11
	#define	TRANSMIT_IMAGE_FORMAT_JPEG_COLOR				12
	#define	TRANSMIT_IMAGE_FORMAT_UNCOMPRESSED_COLOR		13
	#define	TRANSMIT_IMAGE_FORMAT_BMP_COLOR				14
	#define	TRANSMIT_IMAGE_FORMAT_UNCOMPRESSED_RAW		15

	int	TransmitImageFormat;
	int	TransmitImageMargin;
	bool	TransmitImageUseFastAlgorithmForJPEG;

	bool	ImageFlattening;
	bool	ImageStretch;
	int	ImageEdgeSharpenAmount;
	int	ImageRotationValue;
	bool	FlySpecks;
   bool  Snazzy;
	bool	GlitchFix;
	int	RowNoise;
	int	GammaCorrection;
	bool	Histogram;
	int	UltraText;
   bool	AdaptiveUltraText;
	bool	InfinityFilter;
	bool	ShipImagesIn640x480VGAMode;
   //bool  Fission;
   //bool  Fusion;
   //bool   ApplyFissionOnImageSnap;

// old way see ImageSnapStyle	bool		ImagePhotoMode;

	int	ImageSnapAimerState;
	int	ImageSnapAcceptanceDelta;
	int	ImageSnapFrameRate;
	int	ImageSnapLedState;
	int	ImageSnapStyle;
	int	ImageSnapTriggerMode;
	int	ImageSnapAttempts;
	int	ImageSnapWhiteTarget;
	int	ImageSnapExposure;
	int	ImageSnapGain;
	int	ImageSnapPercent;
	int	ImageSnapBeep;
	int	ImageSnapMustConform;
   bool  ShipRawImage;

	bool	BatchedImagingMode;      //Heat Mitigation
  	int 	BatchedImagingLeds;

	int	ImagePresentationExposure;

#ifdef CF2D
	bool	AimersOnWhenDecoding;
#endif

#ifdef STREAM_CAPTURE
   #define SIZE_OF_IMAGE_STREAM_TEMPLATE 128
   char ImgStreamTemplate[SIZE_OF_IMAGE_STREAM_TEMPLATE];
   int SaveStream;
   bool StreamAutoClear;
   int StreamAutoDump;
   int StreamExtraImages;
   int StreamDecodeStart;
#endif

	bool ImageROIEnabled;
#ifdef DEVELOPMENT_SETTINGS
	int	ImageROIXZero;
	int	ImageROIYZero;
	int	ImageROIWidth;
	int	ImageROIHeight;
	bool	ImageE2Vi2cTableSelect;
#else
	int ImageROILeft;
	int ImageROIRight;
	int ImageROITop;
	int ImageROIBottom;
#endif

    int ImageVertFlip;
    int ImageHorizFlip;
	int	ExposureCellphone;				//Exposure value for cell phone



void CreateImagingMenuSettings( int ControlMask, UINT AimerCommandModifier, UINT ImageShippingCommandModifier, UINT Two_DCommandModifier,
			UINT Two_DTagCommandModifier, UINT Two_DSubtagCommandModifier, UINT HeatMitigationFixedSettingsCommandModifier );

   int ImageQualityRanking;
   int ImageQualitySkip;
   int ImageQualityMode;
   int PresentationImageQualityMode;
   int IQSenseSensitivity;
   bool ImageQualityCellPhone;

	bool Filter3x3Sign;
	int Filter3x3ShiftingCoefficient;
// 3 x 3 filter coefficients left to right, top row to bottom row
// 0 1 2
// 3 4 5
// 6 7 8
	int Filter3x3Coefficient[9];
	int Filter3x3Config;

	bool ScanDriverCellPhone;
	bool PresentationScanDriverCellPhone;
#ifdef VUQUEST3310
 	bool ExternalIllEnb;/*external illumination enable*/
#endif
#endif // ImagingMenuSettingsH
//----------------------------------------------------------------------------




