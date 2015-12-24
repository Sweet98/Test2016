/************************************************************************/
//		This module performs Photo taking functions
//
// $RCSfile: Gen6FoundationApp/FoundationApp/Imaging/imaging.c $
// $Revision: 1.64 $
// $Date: 2011/10/19 15:28:02EDT $
//======================================================================================
#include <math.h>
#include <stdlib.h>
#include <stdio.h>                             //lmc  lib includes sprintf()
#include <string.h>

#include "product.h"
//#include "ScanDriverInterface.h"			//changed to "scandrivermatrixinterface.h"
#include "scandrivermatrixinterface.h"
#include "db_chrio.h"
#include "ioman.h"
#include "imaging.h"
#include "HSTRING.h"
#include "readerconfiguration.h"
#include "productconfiguration.h"
#include "menu.h"
#include "db_time.h"
#include "beep.h"
#include "Image_Send_Binarize_Himage.h"
#include "MatrixHComm.h"
#include "DecodeIface.h"
#include "ScanDriverImagingInterface.h"
#include "barcodedata.h"
#include "BufferPool.h"
#include "db_Heap.h"
extern "C" {
#include "ImageCore.h"
}

#include "Hi704A.h"

#define	TRIGGER_5_5		1
#ifdef TRIGGER_5_5
#include "trigger_5_5.h"                                         //lmc   fixme
#else
#include "trigger.h"
#include "TriggerHandler.h"
#endif	//end #ifdef TRIGGER_5_5

#include "HWTrigger.h"


#ifdef FLASH_LEDS
#include "Flash_Illumination_Control.h"
#else
#include "iMX25_Illumination_Control.h"
#endif //endof FLASH_LEDS

#ifdef ENGINE_PSOC
#include "scanenginepsoc.h"
#endif

#ifdef RFSCAN
#include	"BTStackLL.h"
#endif

extern "C" void Fast_memcpy(void *Destination, void *Source, int CopySize);
extern void SendStringToInterface( const char *pucBuffer);

//Local function protos
void SendChasteImage(int X, int Y, unsigned char *img);

//Defines
#define IMAGE_STYLE_DECODER 0
#define IMAGE_STYLE_PHOTO   1
#define IMAGE_STYLE_MANUAL  2

#define IMAGE_LEDS_NONE 0
#define IMAGE_LEDS_DURING_FRAMES 1

//Variables
int IC_Media_Frames [] = {30,20,15,12,10,6,5,4,3,2,1};
BOOL		PhotoJustTaken = FALSE;

//Variables for image snap with default settings
static DWORD FrameRate = 20;
static DWORD ExpoValue = EXPEMX_JADE;
static int TargetWhiteValue = 90;
static DWORD GainValue = 2;
static int ImagingStyle = IMAGE_STYLE_PHOTO;
static int AcceptanceDelta = 10;

static int NumberOfTries = 4;
static int TargetSetPoint = 50;
static int ConformMe = 0;

static int BeepAfterSnap = 0;
static int LedState = IMAGE_LEDS_NONE;
static int LeaveAimerOn = 0;
static int WaitForTrigger = 0;
static int ImageShipWaitForUnTrigger = 0;
#if 1 
	#define Pt(format, ... ) do {theCurrentInterface.printf(format, ## __VA_ARGS__); wait_til_tx_is_empty();} while (0) 
#else
	#define Pt(format, ... ) do {} while(0)
#endif

//#define IMAGECORE_TEST

//char	MsgBuf[80];                                           //lmc   debug

//------------------------------------------------------------------------------------------------
//Function:  void SetImagingLights( bool bEnableLights, int LightsDutyCycle, int bEnableAimer )
//
//Description:
//			Central location  for snap image for setting up the lights.
//			Also takes care of light bar duty cyle for eye safty.
//
//Passed:
//		bEnableLights when true enables the LEDS if 'LedState' is also true , else they are disabled.
//		LightsDutyCycle if -1 does nothing to duty cycle, -2 allows 'LedState' to select duty cycle,
//			else if >= 0, sets initial duty cycle to value passed (0-100).
//		bEnableAimer when true enables the aimer, false disables aimer,
//			and if  -1 does nothing to aimer state.
//Returns:
//			void.
//------------------------------------------------------------------------------------------------

void SetImagingLights( bool bEnableLights, int LightsDutyCycle, int bEnableAimer )
{
	DWORD  Illumination;
	int LedDutyCycle;
	int  Aimer;
	int  AimerIntensity;
	bool bGotALightBar;

	Illumination = FALSE;
	LedDutyCycle = -1;
	Aimer = -1;
	AimerIntensity = -1;
	bGotALightBar = false;

	// Adjust for engine with light bar (we need to limit LEDs to protect your eyes, to keep your eye balls from burning into your skull and melting your face)

	// We have a light bar engine with off axis light bar--adjust LEDs for it.
	bGotALightBar = pTrigger->AreWeLightBarEngine();

	if( LightsDutyCycle >= 0 )                   //set light duty cycle if value passed
	{
		LedDutyCycle = LightsDutyCycle;

#ifndef FLASH_LEDS       //if not a ff3 gen6 board with flash illum
		if( bGotALightBar )
		{
			if( LedDutyCycle > 1 )
			{
				LedDutyCycle = LedDutyCycle / 2;
			}
			else
			{
				LedDutyCycle = 0;
			}
		}
#endif
	}

	if( (LedState != IMAGE_LEDS_NONE) )              //turn lights on if bEnableLights and LedState are true
	{                                                //set light duty cycle if -2 was passed
		if( LightsDutyCycle == -2 )
		{
			if( bGotALightBar )
			{
#ifdef FLASH_LEDS       //if a ff3 gen6 board with flash illum
				LedDutyCycle = 100;
#else
				LedDutyCycle = 50;
#endif
			}
			else
			{
				LedDutyCycle = 100;
			}
		}

		if(bEnableLights)
		{
			Illumination = TRUE;
		}
	}

	if( (bEnableAimer > 0) && (bEnableAimer != 3) )                              //set aimer state by the value passed
	{
		Aimer = 2;				//set to aimer alt, aimer both (1) not supported in gen6
		AimerIntensity = 100;
	}
	else if( bEnableAimer == 3 )
	{
		Aimer = 3;				//set to aimer continuous, allows aimer to be on when not imaging
		AimerIntensity = 100;
	}
	else if( bEnableAimer == 0 )
	{
		Aimer = 0;
		AimerIntensity = -1;
	}

	SetLights( Illumination, LedDutyCycle, Aimer, AimerIntensity );
}




//-------------------------------------------------------------------------
//Function:		void HandleOriginalLightsDutyCycle(int iSaveOrRestore);
//
//Description:
//			Save or restore the original duty cycle state for the LEDS.
//			We now lower Duty cycle just for light bar, want to
//			guarantee that original value is restored when we leave.
//			Order used should be clear, save, initialize, then restore.
//
//Passes:
//       1 to save value, 2 to restore value, 0 to clear, and 3 to initialize duty cycle to 100%.
//
//Returns:
//			void.
//-------------------------------------------------------------------------

void HandleOriginalLightsDutyCycle(int iSaveOrRestore)
{
  static	int LedDutyCycle = -1;

  if( (iSaveOrRestore == 0) && (LedDutyCycle != -1) )          //clear it
  {
		LedDutyCycle = -1;
  }

  if( (iSaveOrRestore == 1) && (LedDutyCycle == -1) )           //save it
  {
		LedDutyCycle = GetLights( 1 );
		//theStdInterface.printf(" Orig duty cycle saved:%d \r\n", LedDutyCycle);           //lmc-debug
  }

  if( (iSaveOrRestore == 2) && (LedDutyCycle != -1) )          //restore it
  {
		//theStdInterface.printf(" Current duty cycle set:%d \r\n", GetLights( 1 ));           //lmc-debug
		SetLights( -1, LedDutyCycle, -1, -1 );
		//theStdInterface.printf(" Orig duty cycle restored:%d \r\n", LedDutyCycle);           //lmc-debug
		LedDutyCycle = -1;
  }

  if( (iSaveOrRestore == 3) )          //initialize it to 100% duty cycle, do not want high intensity 150% here
  {
		SetLights( -1, 100, -1, -1 );
  }
}




void ShowExpoReport(void)
{
	DWORD nSize, Stuff[] = {HHPSD_IMAGE_STAT_EXPOSURE,0,HHPSD_IMAGE_STAT_GAIN,0,HHPSD_IMAGE_STAT_ILLUM_VALUE,0};
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_IMAGE_ATTRIBUTES,(UCHAR *)&P_SCANBUFFER,0,(PBYTE) Stuff,sizeof(Stuff)/sizeof(DWORD),&nSize);
	theStdInterface.printf("Exp:%d,Gain:%d,Illum:%d\r\n",Stuff[1],Stuff[3],Stuff[5]);
}



void GrabFrame(void)
{
	DWORD  DataToSend[12], DataToRestore[12];
	DWORD ModeToSend, ModeToRestore;
	BOOL	 bRestartScanning;
	int iScanDriverState;

	// if STOP_SCANNING returns non-zero it means that imaging was running, and needed to be shutdown, and will
	// therefore need to be restarted
	iScanDriverState = pTrigger->GetScanDriverState();
	bRestartScanning = SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_STOP_SCANNING,NULL,0,NULL,0,NULL);
	pTrigger->SignalScanningFinished();

	//---- things are sent in groups of 2s, the first is the tag, and the second is the value----
	DataToSend[0] = ES_FIXED_FRAME_RATE;
	DataToSend[1] = FrameRate;
	DataToSend[2] = ES_FIXED_EXP;
	DataToSend[3] = ExpoValue;
	DataToSend[4] = ES_FIXED_GAIN;
	DataToSend[5] = GainValue;
	DataToSend[6] = ES_FRAME_RATE;
	DataToSend[7] = FrameRate;
	DataToSend[8] = ES_CONFORM_IMAGE;
	DataToSend[9] = ConformMe;
	DataToSend[10] = ES_CONFORM_TRIES;
	DataToSend[11] = NumberOfTries;

	DataToRestore[0] = ES_FIXED_FRAME_RATE;
	DataToRestore[2] = ES_FIXED_EXP;
	DataToRestore[4] = ES_FIXED_GAIN;
	DataToRestore[6] = ES_FRAME_RATE;
	DataToRestore[8] = ES_CONFORM_IMAGE;
	DataToRestore[10] = ES_CONFORM_TRIES;

	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_EXPOSURE_MODE,(PBYTE)&ModeToRestore,1*sizeof(DWORD),NULL,0,NULL);
	ModeToSend = HHPSD_EXPOSURE_FIXED;
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_MODE,(PBYTE)&ModeToSend,1*sizeof(DWORD),NULL,0,NULL);
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_EXPOSURE_SETTINGS,(PBYTE)DataToRestore,sizeof(DataToRestore),NULL,0,NULL);
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)DataToSend,sizeof(DataToSend),NULL,0,NULL);
//	RESET_WATCHDOG_TIMER();

	SetImagingLights( true, -2, LeaveAimerOn );
	if(HardwareToken.IAmVoyager1400())
	{
		if(LedState)
		{
			((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_flash.turnon(HI2D_AIM_ALT, true/*, ((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_LedCtrlMode*/);
		}
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_START_SCANNING,NULL,0,NULL,0,NULL);
		for(int i = 0; i < 4; i++)
		{
			SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_NEW_SCAN,(UCHAR *) &P_SCANBUFFER,0,NULL,0,NULL);
		}
	 	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_STOP_SCANNING,NULL,0,NULL,0,NULL);
	 	((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_flash.turnoff(HI2D_AIM_ALT, true/*, ((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_LedCtrlMode*/);
	}
	else
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SINGLE_FRAME_CAPTURE,(UCHAR *) &P_SCANBUFFER,0,NULL,0,NULL);
//	RESET_WATCHDOG_TIMER();

	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)DataToRestore,sizeof(DataToRestore),NULL,0,NULL);
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_MODE,(PBYTE)&ModeToRestore,sizeof(DWORD),NULL,0,NULL);

	SetImagingLights( false, -2, -1 );

	if (GET_SETTING(AutoExposureReport)) ShowExpoReport();

	if( bRestartScanning )
	{
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_START_SCANNING,NULL,0,NULL,0,NULL);
		pTrigger->SetScanDriverState( iScanDriverState );
	}
}


void TakePhoto(void)
{
	DWORD	DataToSend[20], DataToRestore[20];
	DWORD ModeToSend, ModeToRestore;
	BOOL	 bRestartScanning;
	int iScanDriverState;

	// if STOP_SCANNING returns non-zero it means that imaging was running, and needed to be shutdown, and will
	// therefore need to be restarted
	iScanDriverState = pTrigger->GetScanDriverState();
	bRestartScanning = SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_STOP_SCANNING,NULL,0,NULL,0,NULL);
	pTrigger->SignalScanningFinished();

	if( WaitForTrigger )
	{
		SetImagingLights( false, -1, false );

		while(pCHWTrigger->IsTriggerPulled())
		{
			RESET_WATCHDOG_TIMER();

			if (theStdInterface.IsAnyRxData())
			{
				char c = get_char();

				if('*' == c)
				{
					return;
				}
			}
		}

		SetImagingLights( false, -1, 3 );
		ImageShipWaitForUnTrigger = TRUE;

		while(!pCHWTrigger->IsTriggerPulled())
		{
			RESET_WATCHDOG_TIMER();

			if (theStdInterface.IsAnyRxData())
			{
				char c = get_char();

				if('*' == c)
				{
					SetImagingLights( false, -1, false );
					return;
				}
			}
		}

#if PLATFORM_CURRENT == imx25
		SetAIM_ONState(0x0);
#else
		theHal.AimerOff();
#endif //endof #if PLATFORM_CURRENT == imx25
	}

	DataToSend[0] = ES_CONFORM_IMAGE;
	DataToSend[1] = ConformMe; //TRUE;
	DataToSend[2] = ES_MAX_EXP;
	DataToSend[3] = ExpoValue;
	DataToSend[4] = ES_TARGET_VALUE;
	DataToSend[5] = TargetWhiteValue;
	DataToSend[6] = ES_TARGET_PERCENTILE;
	DataToSend[7] = TargetSetPoint;
	DataToSend[8] = ES_TARGET_ACCEPT_GAP;
	DataToSend[9] = AcceptanceDelta;
	DataToSend[10] = ES_CONFORM_TRIES;
	DataToSend[11] = NumberOfTries;
	DataToSend[12] = ES_MAX_GAIN;
	DataToSend[13] = GainValue;
	DataToSend[14] = ES_EXPOSURE_METHOD;
	DataToSend[15] = HHPSD_AE_METHOD_UNIFORM;
	DataToSend[16] = ES_SPECULAR_EXCLUSION;
	DataToSend[17] = HHPSD_SPECULAR_EXCLUSION_OFF;
	DataToSend[18] = ES_ADJUST_EXP_WHEN_ILLUM_OFF;
	DataToSend[19] = TRUE;


	DataToRestore[0] = ES_CONFORM_IMAGE;
	DataToRestore[2] = ES_MAX_EXP;
	DataToRestore[4] = ES_TARGET_VALUE;
	DataToRestore[6] = ES_TARGET_PERCENTILE;
	DataToRestore[8] = ES_TARGET_ACCEPT_GAP;
	DataToRestore[10] = ES_CONFORM_TRIES;
	DataToRestore[12] = ES_MAX_GAIN;
	DataToRestore[14] = ES_EXPOSURE_METHOD;
	DataToRestore[16] = ES_SPECULAR_EXCLUSION;
	DataToRestore[18] = ES_ADJUST_EXP_WHEN_ILLUM_OFF;

	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_EXPOSURE_MODE,(PBYTE)&ModeToRestore,1*sizeof(DWORD),NULL,0,NULL);
	if(HardwareToken.IAmVoyager1400())
	{
		if(LedState)
		{
			((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_flash.turnon(HI2D_AIM_ALT, true/*, ((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_LedCtrlMode*/);
		}
		//ModeToSend = HHPSD_EXPOSURE_FIXED;
		ModeToSend = HHPSD_AUTOEXPOSURE_USE_HHP;
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_MODE,(PBYTE)&ModeToSend,1*sizeof(DWORD),NULL,0,NULL);
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_EXPOSURE_SETTINGS,(PBYTE)DataToRestore,sizeof(DataToRestore),NULL,0,NULL);
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)DataToSend,sizeof(DataToSend),NULL,0,NULL);
		
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_START_SCANNING,NULL,0,NULL,0,NULL);
		for(int i = 0; i < 4; i++)
		{
			SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_NEW_SCAN,(UCHAR *) &P_SCANBUFFER,0,NULL,0,NULL);
		}
	 	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_STOP_SCANNING,NULL,0,NULL,0,NULL);

		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)DataToRestore,sizeof(DataToRestore),NULL,0,NULL);
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_MODE,(PBYTE)&ModeToRestore,sizeof(DWORD),NULL,0,NULL);
		((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_flash.turnoff(HI2D_AIM_ALT, true/*, ((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_LedCtrlMode*/);
	}
	else
	{
		ModeToSend = HHPSD_AUTOEXPOSURE_USE_HHP;
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_MODE,(PBYTE)&ModeToSend,1*sizeof(DWORD),NULL,0,NULL);
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_EXPOSURE_SETTINGS,(PBYTE)DataToRestore,sizeof(DataToRestore),NULL,0,NULL);
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)DataToSend,sizeof(DataToSend),NULL,0,NULL);

		SetImagingLights( true, -2, LeaveAimerOn );

		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SINGLE_FRAME_CAPTURE,(UCHAR *) &P_SCANBUFFER,0,NULL,0,NULL);
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)DataToRestore,sizeof(DataToRestore),NULL,0,NULL);
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_MODE,(PBYTE)&ModeToRestore,sizeof(DWORD),NULL,0,NULL);
		SetImagingLights( false, -1, -1 );
	}

	PhotoJustTaken = TRUE;

	if (GET_SETTING(AutoExposureReport)) ShowExpoReport();

	if( bRestartScanning )
	{
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_START_SCANNING,NULL,0,NULL,0,NULL);
		pTrigger->SetScanDriverState( iScanDriverState );
	}
}



void SimulateDecoding(int NumberOfRescans)
{
	DWORD	DataToSend[2], DataToRestore[2];
	DWORD ModeToSend, ModeToRestore;
	BOOL	 bRestartScanning;
	int iScanDriverState;

	// if STOP_SCANNING returns non-zero it means that imaging was running, and needed to be shutdown, and will
	// therefore need to be restarted
	iScanDriverState = pTrigger->GetScanDriverState();
	bRestartScanning = SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_STOP_SCANNING,NULL,0,NULL,0,NULL);
	pTrigger->SignalScanningFinished();

	DataToSend[0] = ES_CONFORM_IMAGE;
	DataToSend[1] = ConformMe; //TRUE;
	DataToRestore[0] = ES_CONFORM_IMAGE;
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_EXPOSURE_MODE,(PBYTE)&ModeToRestore,1*sizeof(DWORD),NULL,0,NULL);
	ModeToSend = HHPSD_AUTOEXPOSURE_USE_HHP;
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_MODE,(PBYTE)&ModeToSend,1*sizeof(DWORD),NULL,0,NULL);
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_EXPOSURE_SETTINGS,(PBYTE)DataToRestore,sizeof(DataToRestore),NULL,0,NULL);
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)DataToSend,sizeof(DataToSend),NULL,0,NULL);

	SetImagingLights( true, 100, LeaveAimerOn );


	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_START_SCANNING,NULL,0,NULL,0,NULL);

	for (int i=0; i<(NumberOfRescans+1); i++)		{SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_NEW_SCAN,(UCHAR *)&P_SCANBUFFER,0,NULL,0,NULL);}

	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_STOP_SCANNING,NULL,0,NULL,0,NULL);

	pTrigger->SignalScanningFinished();

	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)DataToRestore,sizeof(DataToRestore),NULL,0,NULL);
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_MODE,(PBYTE)&ModeToRestore,sizeof(DWORD),NULL,0,NULL);

	SetImagingLights( false, -1, -1 );

	if (GET_SETTING(AutoExposureReport)) ShowExpoReport();

	if( bRestartScanning )
	{
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_START_SCANNING,NULL,0,NULL,0,NULL);
		pTrigger->SetScanDriverState( iScanDriverState );
	}
}

void DoFission(unsigned char *img, int wid, int hgt) {
   if (img) {
      int x, y, yInc; 
      unsigned char *gline, *gimg;
      yInc = wid * 4;
      for (y = 2, gline = img+(wid * y); (y<hgt-1); y+= 4,gline+=yInc) {
         for (x=2 , gimg=gline+x; x<wid-1; x += 4, gimg += 4) {
            int n = *(gimg-wid), w = *(gimg-1), e = *(gimg+1), s = *(gimg+wid);
            int nw = *(gimg-1-wid), ne = *(gimg+1-wid), sw = *(gimg-1+wid), se = *(gimg+1+wid);
            int vertical = (nw-w)*(ne-e) + (w-sw)*(e-se), horizontal = (nw-n)*(sw-s) + (n-ne)*(s-se);
            if (vertical < horizontal) *gimg = (n + s + 1) >> 1; else *gimg = (e + w + 1) >> 1;
         }
      }
   }
}

/*

IMGSNP - Image Snap
 * This is the command processor for the taking of an image. Three styles are supported
 * 	1) Decoding Style - This tries to be faithful to the current format for decoding. There is a slight difference as we
 * 		normally attempt to decode each frame even if exposure parameters are not yet met. This processing will allow a
 * 		few frames to betaken until the exposure parameters are met and then the last frame is available for further use.
 * 	2) Photo Style - This attempts to mimic a simple digital camera and will result in a visually optomized image.
 * 	3) Manual Style - This allows the user the most freedom to set up the imager. There will be no auto-exposure.
 *
 *                                  Decode  Photo  Manual
 *                                   Style  Style  Style    Range and Definition
 *                                  ------  -----  ------   --------------------------------
 *    'D': Delta for Acceptance        N      Y      N      0-255. In mult. expos: quit if '%' of histogram is within +/- 'D' of 'W'
 *    'E': Exposure                    N      N      Y      1-255 larger number => longer exposure time
 *    'F': FrameRate                   N      N      Y      Frames per second 1-200 (depending on imager)
 *    'G': Gain                        N      Y      Y		0-8
 *    'L': LED State                   N      Y      Y		0->No LEDs, 1->during frame, 2-> always on
 *    'P': Imaging Style               -      -      -      0->Decode, 1->photo, 2->manual
 *    'U': Update Tries                N      Y      N		# of frames to take to meet exposure needs
 *    'W': Target White Value          N      Y      N		0-255 grayscale histogram set point
 *    '%': Target Percentage           N      Y      N      1-99 percent of histogram to match white value
 *		'C': Conform														0 - conform disabled, 1 - conform enabled, 2 - give me 'that' image (defined by tries)
 *
 *
 *
*/

int ParseImageSnapHeader(char *pParamList, unsigned int uiParamListLength )
{
//	char 			MsgBuf[100];
	int			ReturnStatus = FALSE;
	int			NextLocationToRead = 0;
	int			iCharCount = 0;
	char			*pNextLocationToSearchString;
	char			*pNextLocationAfterSearchOfString;
	char			*pNextLocationBeforeSearchOfString;
	char			*pEndLocationOfSearchString;
	char			**ppNextLocationAfterSearchOfString;

	//Set up pointers for parsing
	pNextLocationToSearchString =  &pParamList[NextLocationToRead];
	pNextLocationAfterSearchOfString = &pParamList[NextLocationToRead];
	pNextLocationBeforeSearchOfString = &pParamList[NextLocationToRead];
	pEndLocationOfSearchString = &pParamList[uiParamListLength - 1];
	ppNextLocationAfterSearchOfString = &pNextLocationAfterSearchOfString;

	//debug string
	/*                                                                         //lmc   debug
	xputstring(" STRING: ");
	for ( iCharCount = 0; iCharCount < uiParamListLength; iCharCount++ )
	{
		xputchar(pParamList[iCharCount]);
	}
	xputstring(" PARSED ");                                           */       //lmc   debug

	//Next remove [syn] 0xFE yyyy [cr]
	NextLocationToRead = 0;

	if( ((uiParamListLength + NextLocationToRead) >= 7) && (pParamList[NextLocationToRead] == 0x02) &&
			(pParamList[NextLocationToRead + 1] == 0xFE) && (pParamList[NextLocationToRead + 6] == 0x0D) )
	{
		NextLocationToRead += 7;
	}

	//Next remove "IMGSNP"
	if( ((uiParamListLength + NextLocationToRead) >= 6) && (pParamList[NextLocationToRead] == 'I') &&
			(pParamList[NextLocationToRead + 1] == 'M') && (pParamList[NextLocationToRead + 2] == 'G') &&
			(pParamList[NextLocationToRead + 3] == 'S') && (pParamList[NextLocationToRead + 4] == 'N') &&
			(pParamList[NextLocationToRead + 5] == 'P') )
	{
		NextLocationToRead += 6;
	}

	//Parse for parameters
	iCharCount = 0;

	if( uiParamListLength >= 2 )           //must have at least one number and one letter
	{
		int			iNumber = 0;
		int			iFoundNumber = 0;
		char			*pFoundCharInString;
		bool			bValidData = false;

		pFoundCharInString = &pParamList[NextLocationToRead];

		// Parse through the header.
		while( (iCharCount < uiParamListLength) )
		{
			iNumber = (int) strtol( pNextLocationToSearchString, ppNextLocationAfterSearchOfString, 0 );

			//Did we find a letter, then save them,
			if( 	((**ppNextLocationAfterSearchOfString == 0x25) ||
					((**ppNextLocationAfterSearchOfString > 0x40)&&(**ppNextLocationAfterSearchOfString < 0x5B)) ||
					((**ppNextLocationAfterSearchOfString > 0x60)&&(**ppNextLocationAfterSearchOfString < 0x7B))) &&
					( *ppNextLocationAfterSearchOfString != pNextLocationBeforeSearchOfString ) )		//and we also Found a new character
			{
				// Parameter found is OK.
				pNextLocationBeforeSearchOfString = *ppNextLocationAfterSearchOfString;
				bValidData = TRUE;
				iFoundNumber = iNumber;
				pFoundCharInString = *ppNextLocationAfterSearchOfString;
			}

			//sprintf (MsgBuf," Set %c = %d at step %c \r\n", **ppNextLocationAfterSearchOfString, iNumber, pParamList[NextLocationToRead] );      //lmc   debug
			//xputstring (MsgBuf);                                        //lmc   debug

			iCharCount++;
			pNextLocationToSearchString++;
			NextLocationToRead++;

			if( bValidData )
			{
				bValidData = false;

				if( (iCharCount < uiParamListLength) && (pFoundCharInString <= pEndLocationOfSearchString) ) //must have at least one number and one letter (2 char), and not be pointing past end of string
				{
					switch(*pFoundCharInString)
					{
						// Specular, expomethod, expomode may want to be here
						case 'B': case 'b': BeepAfterSnap = iFoundNumber; break;
						case 'D': case 'd': AcceptanceDelta = iFoundNumber; break;
						case 'E': case 'e': ExpoValue = iFoundNumber; break;
						case 'F': case 'f': FrameRate = iFoundNumber; break;
						case 'G': case 'g': GainValue = iFoundNumber; break;
						case 'L': case 'l': LedState = iFoundNumber; break;
						case 'P': case 'p': ImagingStyle = iFoundNumber; break;
						case 'U': case 'u': NumberOfTries = iFoundNumber; break;
						case 'W': case 'w': TargetWhiteValue = iFoundNumber; break;
						case '%':           TargetSetPoint = iFoundNumber; /*theStdInterface.printf ("pct%d",TargetSetPoint);*/break;
						case 'T': case 't': WaitForTrigger = iFoundNumber; break;
               			case 'A': case 'a': LeaveAimerOn = iFoundNumber; break;
						case 'C': case 'c': ConformMe = iFoundNumber; break;
						default:            ReturnStatus = FALSE; break;
					}

					ReturnStatus = TRUE;
//					sprintf (MsgBuf," Set %c = %d \r\n", *pFoundCharInString, iFoundNumber);	 //lmc   debug
//					xputstring (MsgBuf);														 //lmc   debug
				}
				else		//failed because past end of string
				{
					ReturnStatus = FALSE;
					break;
				}
			}
		}//end while
	}
	else if( uiParamListLength == 0 )           //must have at least one number and one letter
	{
		//xputstring(" NoChar ");               //lmc  debug to indicate their was no extranious data      lmc-debug   remove

		ReturnStatus = TRUE;
	}
	else         //less than two characters in sting - fail
	{
		ReturnStatus = FALSE;
	}

	// Allow 1000 milli seconds max exposure
	// (versus 1000ms) max exposure if the illumination leds are enabled.
	// Adjusted for light bar units
	//
	#ifdef HI2D_T
	const int MaxExposure = 13400;		//0.67us, max=13400*0.67=8933us
	const int LightExposure = 13400;	 
	#else
	const int MaxExposure = EXPSNP_JADE;		// 128 uS * 7874, Exposure limit of ~1000ms. No Lights only

	const int LightExposure = EXPSNP_JADE;	  	//Value used : const int MaxExposure = 7874;  // Exposure limit of 1000ms.
															// this 1000ms exposure limit is enforced only if the LEDS will be used
															//for the image capture.
	#endif

	if( (ExpoValue > LightExposure) && (LedState != IMAGE_LEDS_NONE) )           //have lights on
	{
		ExpoValue = LightExposure;
	}
	else if( ( ExpoValue > MaxExposure ) )              //lights off
	{
		ExpoValue = MaxExposure;
	}

	return(ReturnStatus);
} // ParseImageSnapHeader



//
//Function: ImageSnapWithParameters()
//Pass string to configure scandriver
//Then snaps an image
//
bool ImageSnapWithParameters(char **ppParamList, unsigned int *pParamListLength )
{
	bool ReturnStatus = FALSE;
	int ParseStatus = TRUE;
	int time1 = 0;
	int time2 = 0;

	if( GET_SETTING(DebugImageTimeReporting) )
	{
   		time1 = current_time();
	}

	HandleOriginalLightsDutyCycle(0);    //clear

	// Initialize settings
   	LeaveAimerOn         = GET_SETTING(ImageSnapAimerState);							//0;
	AcceptanceDelta      = GET_SETTING(ImageSnapAcceptanceDelta);					//25;
	FrameRate            = GET_SETTING(ImageSnapFrameRate);			            //30;
	LedState             = GET_SETTING(ImageSnapLedState);							//0;
 	ImagingStyle         = GET_SETTING(ImageSnapStyle);		                  //1;
	NumberOfTries        = GET_SETTING(ImageSnapAttempts);                		//6;
	TargetWhiteValue     = GET_SETTING(ImageSnapWhiteTarget);	          		//90;
 	ExpoValue            = GET_SETTING(ImageSnapExposure);	                  //7874;
 	GainValue            = GET_SETTING(ImageSnapGain);			                  //2;
	TargetSetPoint       = GET_SETTING(ImageSnapPercent);			               //50;
	WaitForTrigger			= GET_SETTING(ImageSnapTriggerMode);	              	//0;
	BeepAfterSnap			= GET_SETTING(ImageSnapBeep);                    		//0;
	ConformMe				= GET_SETTING(ImageSnapMustConform);		            //1;

	ParseStatus = ParseImageSnapHeader( *ppParamList, *pParamListLength );

	if( LeaveAimerOn ) 		LeaveAimerOn = 3;		//make aimer on continuous, even during image

	if(ParseStatus)
	{
		HandleOriginalLightsDutyCycle(1);    //save
		HandleOriginalLightsDutyCycle(3);    //initialize

		if (ImagingStyle == IMAGE_STYLE_DECODER)           //0
		{
			SimulateDecoding(0);
			ReturnStatus = TRUE;
		}
		else if (ImagingStyle == IMAGE_STYLE_PHOTO)        //1
		{
			TakePhoto();
			ReturnStatus = TRUE;
		}
		else if (ImagingStyle == IMAGE_STYLE_MANUAL)       //2
		{
			int xFrameRate;

			for( xFrameRate=0; xFrameRate < sizeof(IC_Media_Frames)/sizeof(int); xFrameRate++ )
			{
				if (IC_Media_Frames[xFrameRate] <= FrameRate)	break;
			}
			/*check to see if the search found it by seeing if loop control is what cause for loop to bail. If so we want to use the last frame rate
			 * in the buffer instead of accessing one past the end of the array IC_Media_Frames*/
			if(xFrameRate == sizeof(IC_Media_Frames)/sizeof(int))xFrameRate--;
			
			FrameRate = IC_Media_Frames[xFrameRate];

			GrabFrame();
			ReturnStatus = TRUE;
		}
		else
		{
			ReturnStatus = FALSE;
		}

		if (BeepAfterSnap) SoundGoodReadBeep();

		HandleOriginalLightsDutyCycle(2);    //restore
	}
	else
	{
		ReturnStatus = FALSE;
	}
   
// Get image height and width from scan driver.
   DWORD dwHeight, dwWidth;
   SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&dwHeight,4,NULL);
   SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&dwWidth,4,NULL);


	// If we actually did something (return status is true), restore trigger state.  If in scan stand or auto trigger mode, these modes need to be restarted.
	if(ReturnStatus)
	{
#ifdef TRIGGER_5_5
		pTrigger->SetTriggerMode();
#else
		pTrigger->Reset();
		g_pTriggerHandler->Update();
#endif	//end #ifdef TRIGGER_5_5
	}

	if (GET_SETTING(DebugImageTimeReporting))
   {
		char	MsgBuf[100];
		time2 = current_time();
      sprintf (MsgBuf, "Image Cap time = %dms\r\n", (time2-time1)/1000);
#ifndef RFSCAN
      xputstring (MsgBuf);
#else
		SendStringToInterface(MsgBuf);
#endif
	}

	return ReturnStatus;
}



//
//Function: ImageSnap()
//Use extranious data hstring to configure scandriver from menu
//Then snaps an image.
//
bool ImageSnap(void)
{
	bool	ReturnStatus = FALSE;
	char	ResultString[160];
	unsigned int	MessageCommandLength = 0;
	char *pMessageCommand;
	char *pCopyMessageCommand;

	pCopyMessageCommand = &ResultString[0];

	if( ReaderConfiguration->ExtraneousTextAfterCommand->Size > 0 )
	{
		HSTRING	ImageSnapHeader(ReaderConfiguration->ExtraneousTextAfterCommand);
		MessageCommandLength = ImageSnapHeader.Size;
		pMessageCommand = (char*)ImageSnapHeader.Char;
		strncpy( pCopyMessageCommand, pMessageCommand, MessageCommandLength );

		//debug string
		                                     /*                                    //lmc   debug
		xputstring(" STRING: ");
		for (int iCharCount = 0; iCharCount < MessageCommandLength; iCharCount++ )
		{
			xputchar(pCopyMessageCommand[iCharCount]);
		}
		xputstring(" PARSED ");                               */                   //lmc   debug
	}

	ReturnStatus = ImageSnapWithParameters( &pCopyMessageCommand, &MessageCommandLength );

	return ReturnStatus;
}


void LittleEndien4bytes(unsigned int x, unsigned char *ptr) {*ptr++ = x & 0xff; x >>= 8; *ptr++ = x & 0xff; x >>= 8; *ptr++ = x & 0xff; x >>= 8; *ptr++ = x & 0xff;}
void LittleEndien2bytes(unsigned int x, unsigned char *ptr) {*ptr++ = x & 0xff; x >>= 8; *ptr++ = x & 0xff;}

/// SendChasteImage - This routines send a bmp image out the interface port. It will be unprocessed.
// I didn't bother to check for a width that is not a multiple of 4. So only call this if X%4 == 0.
void SendChasteImage(int X, int Y, unsigned char *img)
{
	unsigned int startTime = current_time();
	unsigned char ColorTable[256 * 4];
	unsigned char Header[7];
	char Cmd[64];
	unsigned int Size;
	unsigned char bmp_header[54];
	unsigned char *ptr = ColorTable;
	int Xpad = X; //use Xpad when working with the image being shipped and X with the original

	//bitmaps must be a multiple of 4
	if( X % 4 ) {
		Xpad += (4 - (X % 4));
	}

	for (int i = 0; i < 256; i++)
	{
		*ptr++ = (unsigned char) i;	*ptr++ = (unsigned char) i;	*ptr++ = (unsigned char) i;	*ptr++ = 0;
	}
	bmp_header[0] = 'B';
	bmp_header[1] = 'M';
	Size = Xpad * Y + 14 + 40 + 1024;
	LittleEndien4bytes(Size, &bmp_header[2]);
	LittleEndien2bytes(0, &bmp_header[6]);
	LittleEndien2bytes(0, &bmp_header[8]);
	LittleEndien4bytes(14 + 40 + 1024, &bmp_header[10]);

	LittleEndien4bytes(40, &bmp_header[14]);
	LittleEndien4bytes(Xpad, &bmp_header[18]);
	LittleEndien4bytes(Y, &bmp_header[22]);
	LittleEndien2bytes(1, &bmp_header[26]);
	LittleEndien2bytes(8, &bmp_header[28]);
	LittleEndien4bytes(0, &bmp_header[30]);
	LittleEndien4bytes(0, &bmp_header[34]);
	LittleEndien4bytes(0, &bmp_header[38]);
	LittleEndien4bytes(0, &bmp_header[42]);
	LittleEndien4bytes(256, &bmp_header[46]);
	LittleEndien4bytes(256, &bmp_header[50]);
	sprintf(Cmd, "0408072P0L%dR%dB0T8D1S8F\x1d", Xpad - 1, Y - 1);
	Header[0] = 0x02; Header[1] = 0xfe;
	Size = Xpad * Y + 14 + 40 + 1024 + strlen(Cmd);
	LittleEndien4bytes(Size, &Header[2]);
	Header[6] = 0x0d;
	theCurrentInterface.Write(Header, 7);
	theCurrentInterface.Write((unsigned char *) Cmd, strlen(Cmd));
	theCurrentInterface.Write(bmp_header, 54);
	theCurrentInterface.Write(ColorTable, sizeof(ColorTable));
	ptr = img + (X * (Y - 1));
	// An image sized buffer was created 540k, try to request that buffer
	// if that buffer is removed from the buffer pool, this function won't work
	unsigned int rowsToSend = (Xpad * Y < (FULL_IMAGE_BUFFER)) ? Y : (FULL_IMAGE_BUFFER) / Xpad;
	unsigned int rowsSent = 0;
	unsigned int sends = 0;

	while (rowsSent < Y)
	{
		unsigned char *pTmp = NULL; // pointer for barcode data buffer
		size_t tmpLen = Xpad * rowsToSend; // size to send
		CBarcodeData *pCurrentMsg = CBarcodeData::GetSaveNewBuffer( pTmp, tmpLen );

		//fills in any potential pad chars with black
		memset( pTmp, 0, tmpLen);

		// tmpLen is now the max size of the buffer so total rows available to send = temLen / X
		rowsToSend = tmpLen/X; // maximize the data to be sent
		// do the copies
		if(Y - rowsSent < rowsToSend)
			rowsToSend = Y - rowsSent;
		for(int j = 0; j < rowsToSend; j++){
			Fast_memcpy((pTmp+j*Xpad),(ptr-j*X),X);
		}
		// increment sizes
		ptr -= (X*rowsToSend); // decrement up the image (sent from bottom up)
		rowsSent += rowsToSend;
		sends ++;
		// finalize
		pCurrentMsg->FinalizeBuffer(Xpad*rowsToSend);
		// write it
		theCurrentInterface.Write(pCurrentMsg);
	}

	if (GET_SETTING(DebugImageTimeReporting))
	{
		unsigned int endTime = current_time();
		char tmp[100];
		sprintf(tmp,"Raw image ship time = %dms, at %d rows per send\r\n",(endTime-startTime)/1000, (Y/sends));
		theCurrentInterface.Write(tmp);
	}
}

void SendSalaciousImage(int X, int Y, unsigned char *img) {
   unsigned char Header[7];
   char Cmd[64];
   unsigned int Size;
   unsigned char bmp_header[54];
   unsigned char *ptr;
	int Xpad = X; //use Xpad when working with the image being shipped and X with the original

	//bitmaps must be a multiple of 4
	if( X % 4 ) {
		Xpad += (4 - (X % 4));
	}

   bmp_header[0] = 'B';
   bmp_header[1] = 'M';
   Size = Xpad*Y*3 + 14 + 40;
   LittleEndien4bytes(Size, &bmp_header[2]);
   LittleEndien2bytes(0, &bmp_header[6]);
   LittleEndien2bytes(0, &bmp_header[8]);
   LittleEndien4bytes(14+40, &bmp_header[10]);

   LittleEndien4bytes(40, &bmp_header[14]);
   LittleEndien4bytes(Xpad, &bmp_header[18]);
   LittleEndien4bytes(Y, &bmp_header[22]);
   LittleEndien2bytes(1, &bmp_header[26]);
   LittleEndien2bytes(24, &bmp_header[28]);
   LittleEndien4bytes(0, &bmp_header[30]);
   LittleEndien4bytes(0, &bmp_header[34]);
   LittleEndien4bytes(0, &bmp_header[38]);
   LittleEndien4bytes(0, &bmp_header[42]);
   LittleEndien4bytes(0, &bmp_header[46]);
   LittleEndien4bytes(0, &bmp_header[50]);
   sprintf(Cmd,"0408072P0L%dR%dB0T1S14F\x1d",Xpad-1,Y-1);

   Header[0] = 0x02; Header[1] = 0xfe;
   Size = Xpad*Y*3 + 14 + 40 + strlen(Cmd);
   LittleEndien4bytes(Size, &Header[2]);
   Header[6] = 0x0d;
   theStdInterface.Write(Header, 7);
   theStdInterface.Write((unsigned char *)Cmd, strlen(Cmd));
   theStdInterface.Write(bmp_header,54);
   ptr = img + X * ( Y - 1) * 3;
    //mimic the grayscale code
   	// An image sized buffer was created 540k, try to request that buffer
	// if that buffer is removed from the buffer pool, this function won't work

	unsigned int rowsToSend = (Xpad * Y * 3< (FULL_IMAGE_BUFFER)) ? Y : (FULL_IMAGE_BUFFER) / Xpad/3;
	unsigned int rowsSent = 0;
	unsigned int sends = 0;

	while (rowsSent < Y)
	{
		unsigned char *pTmp = NULL; // pointer for barcode data buffer
		size_t tmpLen = Xpad * 3* rowsToSend; // size to send
		CBarcodeData *pCurrentMsg = CBarcodeData::GetSaveNewBuffer( pTmp, tmpLen );

		//fills in any potential pad chars with black
		memset( pTmp, 0, tmpLen);

		// tmpLen is now the max size of the buffer so total rows available to send = temLen / X
		rowsToSend = tmpLen/Xpad/3; // maximize the data to be sent
		// do the copies
		if(Y - rowsSent < rowsToSend)
			rowsToSend = Y - rowsSent;
		for(int j = 0; j < rowsToSend; j++){
			Fast_memcpy((pTmp+j*Xpad* 3),(ptr-j*X* 3),X* 3);
		}
		// increment sizes
		ptr -= (X* 3*rowsToSend); // decrement up the image (sent from bottom up)
		rowsSent += rowsToSend;
		sends ++;
		// finalize
		pCurrentMsg->FinalizeBuffer(Xpad* 3*rowsToSend);
		// write it
		theCurrentInterface.Write(pCurrentMsg);
	}
}

#ifndef IMAGECORE_TEST 
//RawImageShipWithParam()
//
//Menu type 1 is menu - M.
//Menu type 2 is non menu - N.
//
bool RawImageShipWithParam( int iMenuType )
{
   unsigned char *ImgPtr = P_SCANBUFFER;
   bool FreeTheImage = false;
   DWORD dwHeight, dwWidth;
   SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&dwHeight,4,NULL);
   SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&dwWidth,4,NULL);

	if( (iMenuType == 1) && (ReaderConfiguration->ExtraneousTextAfterCommand->Size > 0) ) {
      HSTRING           ImageRawHeader(ReaderConfiguration->ExtraneousTextAfterCommand);
      switch(ImageRawHeader.Char[0]) {
			#if defined(GEN6_DEVELOPMENT) || CONCURRENT
         case 'F': case 'f': ImgPtr = P_FASTBUFFER; printf("F");break;
         case 'R': case 'r': ImgPtr = P_RECENTBUFFER; printf("R"); break;
			#endif
         case 'Q': case 'q': ImgPtr = P_SIGCAPBUFFER; printf("Q");break;
         case 'S': case 's': ImgPtr = P_SCANBUFFER; printf("S");break;
         case 'M': case 'm': {
            if (!P_SCANBUFFER) return 0;
            int NewWid = dwWidth;
            int NewHgt = dwHeight;
   			ImageDesc Src, Dst;
   			Src.ImageX = NewWid;
   			Src.ImageY = NewHgt;
   			Src.ImageBlueX = 6; //will make it flexible  			
   			Src.ImageBlueY = 2; //will make it flexible
   			Src.Color = MONOCOLOR;
   			Src.ImageDepth = 8;
   			Src.ImageData = (unsigned char *)ImgPtr;
			Dst.ImageData=0;
   			if (0 == ToGray(&Src, &Dst)) {
				dwWidth = Dst.ImageX; dwHeight = Dst.ImageY;
				ImgPtr = (unsigned char *)Dst.ImageData;
				FreeTheImage = 1;
   			}
            break;         
         }
         case 'C': case 'c': {
         //#define ColorOffset 4 // correction value to put the expected color in the proper position
            if (!P_SCANBUFFER) return 0;
            //RESET_WATCHDOG_TIMER();
   			ImageDesc Src, Dst;
   			Src.ImageX = dwWidth;
   			Src.ImageY = dwHeight;
   			Src.ImageBlueX = 6; //will make it flexible  			
   			Src.ImageBlueY = 2; //will make it flexible
   			Src.Color = MONOCOLOR;
   			Src.ImageDepth = 8;
   			Src.ImageData = (unsigned char *)ImgPtr;
			Dst.ImageData=0;
			//ShowFreeHeapSpace();
   			if (0 == ToRGB(&Src, &Dst)) {
   				dwWidth = Dst.ImageX;dwHeight=Dst.ImageY;
   				ImgPtr = (unsigned char *)MALLOC(dwWidth*dwHeight*3);
   				unsigned char * src, *dst;
   				src = (unsigned char *)Dst.ImageData;//ImgPtr1;
   				dst = ImgPtr+2;for (int y=0; y<dwWidth*dwHeight; y++) {*dst=*src++; dst+=3;}
   				dst = ImgPtr+1;for (int y=0; y<dwWidth*dwHeight; y++) {*dst=*src++; dst+=3;}
   				dst = ImgPtr;for (int y=0; y<dwWidth*dwHeight; y++) {*dst=*src++; dst+=3;}
   				free(Dst.ImageData);
   				SendSalaciousImage(dwWidth, dwHeight, ImgPtr);
   				free(ImgPtr);
/*				dwWidth = Dst.ImageX; dwHeight=Dst.ImageY;
				ImgPtr = (unsigned char *)Dst.ImageData;
				src = ImgPtr;
				dst = src+2;  
				unsigned char c;                
				for (int y=0; y<dwWidth*dwHeight; y++) {c=*src; *src = *dst; *dst=c; src+=3; dst+=3;}
				//timex = BaselineClock;
				SendSalaciousImage(dwWidth, dwHeight, ImgPtr);
				if (ImgPtr){free(ImgPtr); ImgPtr=0;}
*/				return 1;
   			}
   			else
            return 0;
         }
            default: break;
      }
   }
   bool status = (ImgPtr != NULL);
   if (status) SendChasteImage((int)dwWidth, (int)dwHeight, ImgPtr);
   if (FreeTheImage) free(ImgPtr);
   return status;
}
#else    
//this version of RawImageShipWithParam is expanded based on the original one
//to be used for testing the ImageCore library update for color phase II
//
bool RawImageShipWithParam( int iMenuType )
{
   unsigned char *ImgPtr = P_SCANBUFFER;
   ImageDesc Src, Dst;
   bool FreeTheImage = false;
   DWORD dwHeight, dwWidth;
   int NewWid;
   int NewHgt;
   int i, ii, arg1=0, arg2=0, arg3, arg4, sg, tem;
   int time1=0, time2=0;   
   SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&dwHeight,4,NULL);
   SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&dwWidth,4,NULL);

   if( (iMenuType == 1) && (ReaderConfiguration->ExtraneousTextAfterCommand->Size > 0) ) {
   HSTRING ImageRawHeader(ReaderConfiguration->ExtraneousTextAfterCommand);
   time1 = current_time();
   Pt("\r\n>>");
   switch(ImageRawHeader.Char[0]) {
#if defined(GEN6_DEVELOPMENT) || CONCURRENT
         case 'F': case 'f': ImgPtr = P_FASTBUFFER; printf("F");break;
         case 'R': case 'r': 
            if (!P_RECENTBUFFER) return 0;            
         	ImgPtr = P_RECENTBUFFER; 
       		Dst.ImageX = dwWidth;
       		Dst.ImageY = dwHeight;
    		Dst.Color = GRAY;//for display
         	Dst.ImageData=ImgPtr; 
         	printf("R");
         	break;
#endif
         case 'Q': case 'q': ImgPtr = P_SIGCAPBUFFER; printf("Q");break;
         case 'S': case 's': ImgPtr = P_SCANBUFFER; printf("S");break;
         //starting of testing, uing imgraw modifier to test the imagecore functions
         case 'M': case 'm': 
             //if (!P_SCANBUFFER) return 0;
             //ImgPtr = P_RECENTBUFFER;
             if (!P_RECENTBUFFER) return 0;            
             ImgPtr = P_RECENTBUFFER;

             Src.ImageX = dwWidth;
             Src.ImageY = dwHeight;
             Src.ImageBlueX = 6; //put your ImageBlueX here            
             Src.ImageBlueY = 2; //put your ImageBlueY here
             Src.Color = MONOCOLOR;
             Src.ImageData = (unsigned char *)ImgPtr;
             //Dst.ImageData = (unsigned char *)MALLOC(dwWidth*dwHeight*1);
    			Dst.ImageData = NULL;
             //Dst=Src; //do this if need fast, but the buffer would be overwitten
    			ToGray(&Src, &Dst);
             break;                  
          case 'C': case 'c': 
             if (!P_RECENTBUFFER) return 0;
             ImgPtr = P_RECENTBUFFER;
    			Src.ImageX = dwWidth; 
    			
    			Src.ImageY = dwHeight;
    			Src.ImageBlueX = 6; //put your ImageBlueX here  			
    			Src.ImageBlueY = 2; //put your ImageBlueY here 
    			Src.Color = MONOCOLOR;
    			Src.ImageData = (unsigned char *)ImgPtr;
    			//Dst.ImageData = (unsigned char *)MALLOC(dwWidth*dwHeight*3);
    			Dst.ImageData = NULL;
    			if (0 == ToRGB(&Src, &Dst)) {
    				NewWid = Dst.ImageX; NewHgt=Dst.ImageY;
    				FreeTheImage=1;
    				break;
    			}
    			else {
             	return 0;
    			}
             
         case 'A': case 'a': //Infinity Filter
            if (!P_RECENTBUFFER) return 0;            
            ImgPtr = P_RECENTBUFFER;
   			Src.ImageX = dwWidth;
   			Src.ImageY = dwHeight;
   			Src.ImageBlueX = 6;  			
   			Src.ImageBlueY = 2; 
   			Src.Color = MONOCOLOR;
   			Src.ImageData = (unsigned char *)ImgPtr;
            if (ImageRawHeader.Char[1]=='1') {			
            	Dst.ImageData = 0;
            	Dst.Color = RGB; //can be GRAY
            	Pt("Calling InfinityFilter\r\n");	
            	InfinityFilter(&Src, &Dst);           	
            	FreeTheImage=1;
            	break;
            }else {
            	Pt("imgrawA0,= ToRGB\r\n");
            	Dst.ImageData = (unsigned char *)MALLOC(dwWidth*dwHeight*3);
            	ToRGB(&Src, &Dst);
            	FreeTheImage=1;
            	break;
            }
   			
         case 'B': case 'b': //TwoTone (w. Magic)
            if (!P_RECENTBUFFER) return 0;            
            ImgPtr = P_RECENTBUFFER;
   			Src.ImageX = dwWidth;
   			Src.ImageY = dwHeight;
   			Src.ImageBlueX = 6;  			
   			Src.ImageBlueY = 2; 
   			Src.Color = MONOCOLOR;
   			Src.ImageData = (unsigned char *)ImgPtr;
             	if (ImageRawHeader.Char[1]>='0' && ImageRawHeader.Char[1]<='8') {
					arg1 = (int)ImageRawHeader.Char[1]-'0';
					arg2=-1;
					if (ImageRawHeader.Char[2] == ' ' ||ImageRawHeader.Char[2] == ',') arg2=0;
					for (i=3; i<=5; i++){ 
						tem = (int)ImageRawHeader.Char[i]-'0'; 
						if (tem>=0 && tem<10) {
							arg2 = arg2 *10 + tem;
						}				
						else break;
					}
            	}
             	if (arg1>=0 && arg2>880 && arg2<990) {					
		            Dst.ImageData = 0;
		            Dst.Color = GRAY;
	            	Pt("NOT IMPLEMENTED CORLOROUT--Calling TwoTone arg1-2 = %d, %d, %d\r\n", arg1, arg2);	
		            TwoTone(&Src, &Dst, arg1, arg2);
		            FreeTheImage=1;
		            break;
            	}else {
	            	Pt("NOT IMPLEMENTED-- \r\n");	
            		return 1;
            	}
         case 'D': case 'd': //C - Compensation
            if (!P_RECENTBUFFER) return 0;            
            ImgPtr = P_RECENTBUFFER;
   			Src.ImageX = dwWidth;
   			Src.ImageY = dwHeight;
   			Src.ImageBlueX = 6;  			
   			Src.ImageBlueY = 2; 
   			Src.Color = MONOCOLOR;
   			Src.ImageData = (unsigned char *)ImgPtr;
           if (ImageRawHeader.Char[1]=='1') {					
	            Dst.ImageData = 0;
	            Dst.Color = GRAY;
	        	Pt("NOT IMPLEMENTED CORLOROUT--Calling Compensation\r\n");	
	            CompensateImage(&Src, &Dst);
	            FreeTheImage=1;
	            break;
           }else {
           		Pt("NOT IMPLEMENTED--\r\n");	
             	return 1;
            	}
        case 'E': case 'e': //Edge Sharpen
            if (!P_RECENTBUFFER) return 0;            
            ImgPtr = P_RECENTBUFFER;
   			Src.ImageX = dwWidth;
   			Src.ImageY = dwHeight;
   			Src.ImageBlueX = 6;  			
   			Src.ImageBlueY = 2; 
   			Src.Color = MONOCOLOR;
   			Src.ImageData = (unsigned char *)ImgPtr;
            if (ImageRawHeader.Char[1]>='0' && ImageRawHeader.Char[1]<='9') {
				arg1 = (int)ImageRawHeader.Char[1]-'0';
				for (i=2; i<=3; i++){ 
					tem = (int)ImageRawHeader.Char[i]-'0'; 
					if (tem>=0 && tem<10) {
						arg1 = arg1 *10 + tem;
					}				
					else break;
				}			
	            Dst.ImageData = 0;
	            Dst.Color = RGB; //can be GRAY
            	Pt("Calling SharpenEdge arg1 = %d\r\n", arg1);	
	            SharpenEdges(&Src, &Dst, arg1);
	            FreeTheImage=1;
	            break;
            }else {
            	Pt("imgrawE?,check arg\r\n");
            	return 1;
            }
         case 'G': case 'g':
	        if (!P_RECENTBUFFER) return 0;
	        ImgPtr = P_RECENTBUFFER;
            
	        if (ImageRawHeader.Char[1]>='8') {
	            NewWid = dwWidth;
	            NewHgt = dwHeight;
	
	   			Src.ImageX = NewWid;
	   			Src.ImageY = NewHgt;
	   			Src.ImageBlueX = 6; //put your ImageBlueX here   			
	   			Src.ImageBlueY = 2; //put your ImageBlueY here
	   			Src.Color = MONOCOLOR;
	   			Src.ImageDepth = 8;
	   			Src.ImageData = (unsigned char *)ImgPtr;
				Dst = Src;
	        	Pt("Calling ToGray\r\n");	
				ToGray(&Src, &Dst);
	            break; 
	          }
                             
	          if (ImageRawHeader.Char[1]>='1' && ImageRawHeader.Char[2]=='4') {
	            //RESET_WATCHDOG_TIMER();
	   			Src.ImageX = dwWidth;    			
	   			Src.ImageY = dwHeight;
	   			Src.ImageBlueX = 6; //put your ImageBlueX here  			
	   			Src.ImageBlueY = 2; //put your ImageBlueY here 
	   			Src.Color = MONOCOLOR;
	   			Src.ImageDepth = 8;
	   			Src.ImageData = (unsigned char *)ImgPtr;
				Dst.ImageData = (unsigned char *)MALLOC(dwWidth*dwHeight*3);
	        	Pt("Calling ToRGB\r\n");	
	   			if (0 == ToRGB(&Src, &Dst)) {
					break;
	   			}
	  		    else {
	  	        	Pt("Fail, ToRGB\r\n");
	  		    	return 0;
	  		    }
	   		  }
         case 'H': case 'h': //Histogram Stretch
            if (!P_RECENTBUFFER) return 0;            
            ImgPtr = P_RECENTBUFFER;
   			Src.ImageX = dwWidth;
   			Src.ImageY = dwHeight;
   			Src.ImageBlueX = 6;  			
   			Src.ImageBlueY = 2; 
   			Src.Color = MONOCOLOR;
   			Src.ImageData = (unsigned char *)ImgPtr;
			i=1;
			while (ImageRawHeader.Char[i] == ' ' ||ImageRawHeader.Char[i] == ',') i++;
			arg1 = (int)ImageRawHeader.Char[i]-'0';
			for (ii=i+1; ii<=i+3; ii++){ 
				tem = (int)ImageRawHeader.Char[ii]-'0'; 
				if (tem>=0 && tem<10) {
					arg1 = arg1 *10 + tem;
				}else {
					i=ii+1;
					break;
				}
			}	
			arg2 = (int)ImageRawHeader.Char[i]-'0';
			for (ii=i+1; ii<=i+3; ii++){ 
				tem = (int)ImageRawHeader.Char[ii]-'0'; 
				if (tem>=0 && tem<10) {
					arg2 = arg2 *10 + tem;
				}else {
					i=ii+1;
					break;
				}
			}	
				
			Dst.ImageData = 0;
            Dst.Color = RGB; //can be GRAY
        	Pt("Calling HistogramStretch arg1-2 = %d, %d\r\n", arg1, arg2);	
            HistogramStretch(&Src, &Dst, arg1, arg2);
            FreeTheImage=1;
            break;
         case 'I': case 'i': //for testing 4 cases
            if (!P_RECENTBUFFER) return 0;
            //IF- Noise Reduction
            if (ImageRawHeader.Char[1]=='F' || ImageRawHeader.Char[1]=='f' ){
	            ImgPtr = P_RECENTBUFFER;
	   			Src.ImageX = dwWidth;
	   			Src.ImageY = dwHeight;
	   			Src.ImageBlueX = 6; //will make it flexible  			
	   			Src.ImageBlueY = 2; //will make it flexible
	   			Src.Color = MONOCOLOR;
				Src.ImageData = (unsigned char *)ImgPtr;
				if (ImageRawHeader.Char[2] =='1'){
		            Dst.ImageData = 0;
		            Dst.Color = RGB; //can be GRAY
	            	Pt("Calling FlySpeck\r\n");	
					FlySpeck(&Src, &Dst);
		            FreeTheImage=1;
					break;
	            }else{
	            	Pt("imgrawIF0,= ToRGB\r\n");
	            	Dst.ImageData = (unsigned char *)MALLOC(dwWidth*dwHeight*3);
	            	ToRGB(&Src, &Dst);
	            	FreeTheImage=1;
	            	break;
	            }
            }
            //Rotate
            if (ImageRawHeader.Char[1]=='R' || ImageRawHeader.Char[1]=='r' ){
	            ImgPtr = P_RECENTBUFFER;
	   			Src.ImageX = dwWidth;
	   			Src.ImageY = dwHeight;
	   			Src.ImageBlueX = 6; //will make it flexible  			
	   			Src.ImageBlueY = 2; //will make it flexible
	   			Src.Color = MONOCOLOR;
	   			Src.ImageData = (unsigned char *)ImgPtr;
	   			if (ImageRawHeader.Char[2]=='0') arg1 = 0;
	   			if (ImageRawHeader.Char[2]=='1') arg1 = 90;
	   			if (ImageRawHeader.Char[2]=='2') arg1 = 180;
	   			if (ImageRawHeader.Char[2]=='3') arg1 = 270;
				Dst.ImageData=NULL;
	            Dst.Color = RGB; //can be GRAY
	        	Pt("Calling RatateImage arg1 = %d\r\n", arg1);	
				RotateImage(&Src, &Dst, arg1);
	            FreeTheImage=1;
	            break;   
            }
            //Mirror
            if ((ImageRawHeader.Char[1]=='Y' || ImageRawHeader.Char[1]=='y')) {
	            ImgPtr = P_RECENTBUFFER;
	            NewWid = dwWidth;
	            NewHgt = dwHeight;
	   			Src.ImageX = NewWid;
	   			Src.ImageY = NewHgt;
	   			Src.ImageBlueX = 6; //will make it flexible  			
	   			Src.ImageBlueY = 2; //will make it flexible
	   			Src.Color = MONOCOLOR;
	   			Src.ImageDepth = 8;
	   			Src.ImageData = (unsigned char *)ImgPtr;
				if (ImageRawHeader.Char[2] =='1'){
	            Dst.ImageData = 0;
	            Dst.Color = RGB; //can be GRAY
	        	Pt("Calling MirrorImage \r\n");	
				MirrorImage(&Src, &Dst);
	            FreeTheImage=1;
				break;
				}else{
	            	Pt("imgrawIY0,= ToRGB\r\n");
	            	Dst.ImageData = (unsigned char *)MALLOC(dwWidth*dwHeight*3);
	            	ToRGB(&Src, &Dst);
	            	FreeTheImage=1;
	            	break;
	            }
            }
            //Flip
            if (ImageRawHeader.Char[1]=='X' || ImageRawHeader.Char[1]=='x' ) {
	            ImgPtr = P_RECENTBUFFER;
	            NewWid = dwWidth;
	            NewHgt = dwHeight;
	   			Src.ImageX = NewWid;
	   			Src.ImageY = NewHgt;
	   			Src.ImageBlueX = 6; //will make it flexible  			
	   			Src.ImageBlueY = 2; //will make it flexible
	   			Src.Color = MONOCOLOR;
	   			Src.ImageData = (unsigned char *)ImgPtr;
				if (ImageRawHeader.Char[2] =='1'){			
	            Dst.ImageData = 0;
	            Dst.Color = RGB; //can be GRAY
	        	Pt("Calling FlipImage \r\n");	
	            FlipImage(&Src, &Dst);
	            FreeTheImage=1;
	            break;
				}
				else {
	            	Pt("imgrawIX0,= ToRGB\r\n");
	            	Dst.ImageData = (unsigned char *)MALLOC(dwWidth*dwHeight*3);
	            	ToRGB(&Src, &Dst);
	            	FreeTheImage=1;
	            	break;
	            }
             }
          case 'J': case 'j': //Vignette
            if (!P_RECENTBUFFER) return 0;            
            ImgPtr = P_RECENTBUFFER;
   			Src.ImageX = dwWidth;
   			Src.ImageY = dwHeight;
   			Src.ImageBlueX = 6;  			
   			Src.ImageBlueY = 2; 
   			Src.Color = MONOCOLOR;
   			Src.ImageData = (unsigned char *)ImgPtr;
			i=1;
			while (ImageRawHeader.Char[i] == ' ' ||ImageRawHeader.Char[i] == ',') i++;
			arg1 = (int)ImageRawHeader.Char[i]-'0';
			for (ii=i+1; ii<=i+3; ii++){ 
				tem = (int)ImageRawHeader.Char[ii]-'0'; 
				if (tem>=0 && tem<10) {
					arg1 = arg1 *10 + tem;
				}else {
					i=ii+1;
					break;
				}
			}	
			arg2 = (int)ImageRawHeader.Char[i]-'0';
			for (ii=i+1; ii<=i+3; ii++){ 
				tem = (int)ImageRawHeader.Char[ii]-'0'; 
				if (tem>=0 && tem<10) {
					arg2 = arg2 *10 + tem;
				}else {
					i=ii+1;
					break;
				}
			}	
			arg3 = (int)ImageRawHeader.Char[i];
			if (arg3=='-') {
				sg=-1; i++;
			}else sg=1;
			arg3 = (int)ImageRawHeader.Char[i]-'0';
			for (ii=i+1; ii<=i+3; ii++){ 
				tem = (int)ImageRawHeader.Char[ii]-'0'; 
				if (tem>=0 && tem<10) {
					arg3 = arg3 *10 + tem;
				}else {
					i=ii+1;
					break;
				}
			}
			arg3 *=sg;
        	if (arg1>=0 && arg2>=0) {					
	            Dst.ImageData = 0;
	            Dst.Color = RGB; //can be GRAY
	        	Pt("Calling Vignette arg1-3 = %d, %d, %d\r\n", arg1, arg2,arg3);	
	            Vignette(&Src, &Dst, arg1, arg2, arg3);
	            FreeTheImage=1;
	            break;
        	}else {
        		Pt("imgrawJ? ? ?, check args\r\n");
        		return 1;
        	}
                            
         case 'K': case 'k': //Gamma Correction
            if (!P_RECENTBUFFER) return 0;            
            ImgPtr = P_RECENTBUFFER;
   			Src.ImageX = dwWidth;
   			Src.ImageY = dwHeight;
   			Src.ImageBlueX = 6;  			
   			Src.ImageBlueY = 2; 
   			Src.Color = MONOCOLOR;
   			Src.ImageData = (unsigned char *)ImgPtr;
        	if (ImageRawHeader.Char[1]>='0' && ImageRawHeader.Char[1]<='9') {
				arg1 = (int)ImageRawHeader.Char[1]-'0';
				for (i=2; i<=5; i++){ 
					tem = (int)ImageRawHeader.Char[i]-'0'; 
					if (tem>=0 && tem<10) {
						arg1 = arg1 *10 + tem;
					}				
					else break;
				}
				Dst.Color = RGB;
				Dst.ImageData = 0;//(unsigned char *)MALLOC(dwWidth*dwHeight*3);
	        	Pt("Calling GammaCorrection arg1= %d/100.0\r\n", arg1);	
				GammaCorrection(&Src, &Dst, arg1/100.0);
  				FreeTheImage=1;
			    break;	
        	}else {
            	Pt("imgrawK? check arg\r\n");
        		return 1;
        	}
			//}
   			
         case 'L': case 'l': //Crop
            if (!P_RECENTBUFFER) return 0;            
            ImgPtr = P_RECENTBUFFER;
   			Src.ImageX = dwWidth;
   			Src.ImageY = dwHeight;
   			Src.ImageBlueX = 6;  			
   			Src.ImageBlueY = 2; 
   			Src.Color = MONOCOLOR;
   			Src.ImageData = (unsigned char *)ImgPtr;
   				//read 4 args
   				i=1;
				while (ImageRawHeader.Char[i] == ' ' ||ImageRawHeader.Char[i] == ',') i++;
   				arg1 = (int)ImageRawHeader.Char[i]-'0';
				theCurrentInterface.printf("\r\n");
				for (ii=i+1; ii<=i+3; ii++){ 
					tem = (int)ImageRawHeader.Char[ii]-'0'; 
					if (tem>=0 && tem<10) {
						arg1 = arg1 *10 + tem;
					}else {
						i=ii+1;
						break;
					}
				}	
   				arg2 = (int)ImageRawHeader.Char[i]-'0';
				for (ii=i+1; ii<=i+3; ii++){ 
					tem = (int)ImageRawHeader.Char[ii]-'0'; 
					if (tem>=0 && tem<10) {
						arg2 = arg2 *10 + tem;
					}else {
						i=ii+1;
						break;
					}
				}	
   				arg3 = (int)ImageRawHeader.Char[i]-'0';
				for (ii=i+1; ii<=i+3; ii++){ 
					tem = (int)ImageRawHeader.Char[ii]-'0'; 
					if (tem>=0 && tem<10) {
						arg3 = arg3 *10 + tem;
					}else {
						i=ii+1;
						break;
					}
				}	
   				arg4 = (int)ImageRawHeader.Char[i]-'0';
				for (ii=i+1; ii<=i+3; ii++){ 
					tem = (int)ImageRawHeader.Char[ii]-'0'; 
					if (tem>=0 && tem<10) {
						arg4 = arg4 *10 + tem;
					}else {
						break;
					}
				}	
			 	
            	if (arg1<arg2 && arg3<arg4) {
		            Dst.ImageData = 0;
		            Dst.Color = RGB; //can be GRAY
		        	Pt("Calling CropImage arg1-4 = %d, %d, %d, %d\r\n", arg1, arg2,arg3,arg4);	
		            CropImage(&Src, &Dst, arg1, arg2, arg3, arg4);
		            FreeTheImage=1;
		            break;
            	}else {
                	Pt("imgrawL???,check args\r\n");
            		return 1;
            	}
         case 'N': case 'n': //Blur Image
            if (!P_RECENTBUFFER) return 0;            
            ImgPtr = P_RECENTBUFFER;
   			Src.ImageX = dwWidth;
   			Src.ImageY = dwHeight;
   			Src.ImageBlueX = 6;  			
   			Src.ImageBlueY = 2; 
   			Src.Color = MONOCOLOR;
   			Src.ImageData = (unsigned char *)ImgPtr;
        	if (ImageRawHeader.Char[1]=='1') {
				Dst.ImageData = 0;
        		Dst.Color = RGB; //can be GRAY
        		Pt("Calling UnSharp\r\n");
        		UnSharp(&Src, &Dst);
				FreeTheImage=1;
        		break;
        	}else {
            	Pt("imgrawN0,= ToRGB\r\n");
            	Dst.ImageData = (unsigned char *)MALLOC(dwWidth*dwHeight*3);
            	ToRGB(&Src, &Dst);
            	FreeTheImage=1;
            	break;
         	}
          case 'O': case 'o': //Crop from center
            if (!P_RECENTBUFFER) return 0;            
            ImgPtr = P_RECENTBUFFER;
   			Src.ImageX = dwWidth;
   			Src.ImageY = dwHeight;
   			Src.ImageBlueX = 6;  			
   			Src.ImageBlueY = 2; 
   			Src.Color = MONOCOLOR;
   			Src.ImageData = (unsigned char *)ImgPtr;
			i=1;
			while (ImageRawHeader.Char[i] == ' ' ||ImageRawHeader.Char[i] == ',') i++;
			arg1 = (int)ImageRawHeader.Char[i]-'0';
			theCurrentInterface.printf("\r\n");
			for (ii=i+1; ii<=i+3; ii++){ 
				tem = (int)ImageRawHeader.Char[ii]-'0'; 
				if (tem>=0 && tem<10) {
					arg1 = arg1 *10 + tem;
				}else {
					i=ii+1;
					break;
				}
			}	
			theCurrentInterface.printf("arg4 = %d, %d, %d, %d,   \r\n", arg1, dwWidth-arg1, arg1, dwHeight-arg1);	
        	if (arg1<320) {
				Dst.ImageData = 0;
        		Dst.Color = RGB; //can be GRAY
        		Pt("Calling CropImage arg1-4 = %d, %d, %d, %d\r\n",arg1, dwHeight-arg1-1, arg1, dwWidth-arg1-1);
        		CropImage(&Src, &Dst, arg1, dwHeight-arg1-1, arg1, dwWidth-arg1-1);
				FreeTheImage=1;
        		break;
        	}else {
            	Pt("imgrawO? check arg\r\n");
        		return 1; 
        	}

         case 'P': case 'p': //S - Pixel Skip
            if (!P_RECENTBUFFER) return 0;            
            ImgPtr = P_RECENTBUFFER;
   			Src.ImageX = dwWidth;
   			Src.ImageY = dwHeight;
   			Src.ImageBlueX = 6;  			
   			Src.ImageBlueY = 2; 
   			Src.Color = MONOCOLOR;
   			Src.ImageData = (unsigned char *)ImgPtr;
        	if (ImageRawHeader.Char[1]>='0' && ImageRawHeader.Char[1]<='9') {
				arg1 = (int)ImageRawHeader.Char[1]-'0';
				for (i=2; i<=4; i++){ 
					tem = (int)ImageRawHeader.Char[i]-'0'; 
					if (tem>=0 && tem<10) {
						arg1 = arg1 *10 + tem;
					}				
					else break;
				}
				if (arg1 >= 1) {
					Dst.ImageData = 0;
	        		Dst.Color = RGB; //can be GRAY
	        		Pt("Calling PixelSkip arg1 = %d\r\n",arg1);
	        		PixelSkip(&Src, &Dst, arg1);
					FreeTheImage=1;
	        		break;
				}else{
					Pt("imgrawP?, ?=0 is not allowed\r\n");
					return 1;
				}
        	}else {
            	Pt("imgrawP? check arg\r\n");
        		return 1;
        	}

         case 'U': case 'u': //DocumentFilter
            if (!P_RECENTBUFFER) return 0;
            ImgPtr = P_RECENTBUFFER;
            NewWid = dwWidth;
            NewHgt = dwHeight;
   			Src.ImageX = NewWid;
   			Src.ImageY = NewHgt;
   			Src.ImageBlueX = 6; //will make it flexible  			
   			Src.ImageBlueY = 2; //will make it flexible
   			Src.Color = MONOCOLOR;
   			Src.ImageData = (unsigned char *)ImgPtr;

			arg1 = (int)ImageRawHeader.Char[1]-'0';
			for (i=2; i<=3; i++){ 
				tem = (int)ImageRawHeader.Char[i]-'0'; 
				if (tem>=0 && tem<10) {
					arg1 = arg1 *10 + tem;
				}				
				else break;
			}
			Dst.ImageData = 0;
			Dst.Color = RGB; //can be GRAY
			if (arg1>0) {
        		Pt("Calling DocumentFilter arg1 = %d\r\n",arg1);
				DocumentFilter(&Src, &Dst, arg1);
	            FreeTheImage=1;
	            break; 
			} else {
            	Pt("imgrawU? check arg\r\n");
				return 1;
			}
         case 'V': case 'v': //Blur Image
            if (!P_RECENTBUFFER) return 0;            
            ImgPtr = P_RECENTBUFFER;
   			Src.ImageX = dwWidth;
   			Src.ImageY = dwHeight;
   			Src.ImageBlueX = 6;  			
   			Src.ImageBlueY = 2; 
   			Src.Color = MONOCOLOR;
			Src.ImageData = (unsigned char *)ImgPtr;
        	if (ImageRawHeader.Char[1]=='1') {
				Dst.ImageData = 0;
				Dst.Color = RGB; //can be GRAY
        		Pt("Calling BLur\r\n");
				Blur(&Src, &Dst);
        		FreeTheImage=1;
        		break;
        	}else {
            	Pt("imgrawV0,= ToRGB\r\n");
            	Dst.ImageData = (unsigned char *)MALLOC(dwWidth*dwHeight*3);
            	ToRGB(&Src, &Dst);
            	FreeTheImage=1;
            	break;
        	}
         case 'W': case 'w': //Histogram Ship
            if (!P_RECENTBUFFER) return 0;            
            ImgPtr = P_RECENTBUFFER;
   			Src.ImageX = dwWidth;
   			Src.ImageY = dwHeight;
   			Src.ImageBlueX = 6;  			
   			Src.ImageBlueY = 2; 
   			Src.Color = MONOCOLOR;
   			Src.ImageData = (unsigned char *)ImgPtr;
           	if (ImageRawHeader.Char[1]=='1') {
				Dst.ImageData = 0;
				Dst.Color = RGB; //can be GRAY
        		Pt("Calling ShowHistogram \r\n");
				ShowHistogram(&Src, &Dst);
        		FreeTheImage=1;
        		break;
        	}else {
            	Pt("imgrawW0,= ToRGB\r\n");
            	Dst.ImageData = (unsigned char *)MALLOC(dwWidth*dwHeight*3);
            	ToRGB(&Src, &Dst);
            	FreeTheImage=1;
            	break;
        	}
         case 'Z': case 'z': //Autocrop
            if (!P_RECENTBUFFER) return 0;            
            ImgPtr = P_RECENTBUFFER;
   			Src.ImageX = dwWidth;
   			Src.ImageY = dwHeight;
   			Src.ImageBlueX = 6;  			
   			Src.ImageBlueY = 2; 
   			Src.Color = MONOCOLOR;
   			Src.ImageData = (unsigned char *)ImgPtr;
   			
			arg1 = (int)ImageRawHeader.Char[1]-'0';
			for (i=2; i<=5; i++){ 
				tem = (int)ImageRawHeader.Char[i]-'0'; 
				if (tem>=0 && tem<10) {
					arg1 = arg1 *10 + tem;
				}				
				else break;
			}
			if (arg1>0){
				Dst.ImageData = 0;
				Dst.Color = RGB; //can be GRAY
	       		Pt("Calling AutoCrop arg1/100.0 = %d\r\n",arg1);
				AutoCrop(&Src, &Dst, arg1/100.0);
				FreeTheImage=1;
				break;
		  	}else {
		  		Pt("imgrawZ?, ?=0 not allowed\r\n");
		  		return 1;
		  	}
   			
          case '1'://GaussianBlur
            if (!P_RECENTBUFFER) return 0;            
            ImgPtr = P_RECENTBUFFER;
   			Src.ImageX = dwWidth;
   			Src.ImageY = dwHeight;
   			Src.ImageBlueX = 6;  			
   			Src.ImageBlueY = 2; 
   			Src.Color = MONOCOLOR;
   			Src.ImageData = (unsigned char *)ImgPtr;
			//read 3 args
			i=1;
			while (ImageRawHeader.Char[i] == ' ' ||ImageRawHeader.Char[i] == ',') i++;
			arg1 = (int)ImageRawHeader.Char[i]-'0';
			theCurrentInterface.printf("\r\n");
			for (ii=i+1; ii<=i+3; ii++){ 
				tem = (int)ImageRawHeader.Char[ii]-'0'; 
				if (tem>=0 && tem<10) {
					arg1 = arg1 *10 + tem;
				}else {
					i=ii+1;
					break;
				}
			}	
			arg2 = (int)ImageRawHeader.Char[i]-'0';
			for (ii=i+1; ii<=i+3; ii++){ 
				tem = (int)ImageRawHeader.Char[ii]-'0'; 
				if (tem>=0 && tem<10) {
					arg2 = arg2 *10 + tem;
				}else {
					i=ii+1;
					break;
				}
			}	
			arg3 = (int)ImageRawHeader.Char[i]-'0';
			for (ii=i+1; ii<=i+3; ii++){ 
				tem = (int)ImageRawHeader.Char[ii]-'0'; 
				if (tem>=0 && tem<10) {
					arg3 = arg3 *10 + tem;
				}else {
					i=ii+1;
					break;
				}
			}
        	if (arg1>0 & arg2>0 & arg3>0) {
	            Dst.ImageData = 0;
	            Dst.Color = RGB; //can be GRAY
	       		Pt("Calling GaussianBlur arg1-3 =  %d, %d %d/10.0\r\n", arg1, arg2, arg3);
	            GaussianBlur(&Src, &Dst, arg1, arg2, arg3/10.0);
	            FreeTheImage=1;
	            break;
        	}else {
        		Pt("imgraw1? ? ?, check args\r\n");
        		return 1;
        	}
        case '2': //HistogramEqualize
            if (!P_RECENTBUFFER) return 0;            
            ImgPtr = P_RECENTBUFFER;
   			Src.ImageX = dwWidth;
   			Src.ImageY = dwHeight;
   			Src.ImageBlueX = 6;  			
   			Src.ImageBlueY = 2; 
   			Src.Color = MONOCOLOR;
   			Src.ImageData = (unsigned char *)ImgPtr;
        	if (ImageRawHeader.Char[1]=='1') {
				Dst.ImageData = 0;
				Dst.Color = RGB; //can be GRAY
	       		Pt("Calling HistogramEqualize \r\n");
				HistogramEqualize(&Src, &Dst);
        		FreeTheImage=1;
        		break;
        	}else {
            	Pt("imgraw20,= ToRGB\r\n");
            	Dst.ImageData = (unsigned char *)MALLOC(dwWidth*dwHeight*3);
            	ToRGB(&Src, &Dst);
            	FreeTheImage=1;
            	break;
        	}
         case '3': //Averagee
            if (!P_RECENTBUFFER) return 0;            
            ImgPtr = P_RECENTBUFFER;
   			Src.ImageX = dwWidth;
   			Src.ImageY = dwHeight;
   			Src.ImageBlueX = 6;  			
   			Src.ImageBlueY = 2; 
   			Src.Color = MONOCOLOR;
   			Src.ImageData = (unsigned char *)ImgPtr;
        	if (ImageRawHeader.Char[1]=='1') {
				Dst.ImageData = 0;
				Dst.Color = RGB; //can be GRAY
				Pt("Calling Average\r\n");
				Average(&Src, &Dst);
        		FreeTheImage=1;
        		break;
        	}else {
            	Pt("imgraw30,= ToRGB\r\n");
            	Dst.ImageData = (unsigned char *)MALLOC(dwWidth*dwHeight*3);
            	ToRGB(&Src, &Dst);
            	FreeTheImage=1;
            	break;
        	}
         case '4':  //Threshold
            if (!P_RECENTBUFFER) return 0;
            ImgPtr = P_RECENTBUFFER;
            NewWid = dwWidth;
            NewHgt = dwHeight;
   			Src.ImageX = NewWid;
   			Src.ImageY = NewHgt;
   			Src.ImageBlueX = 6; //will make it flexible  			
   			Src.ImageBlueY = 2; //will make it flexible
   			Src.Color = MONOCOLOR;
   			Src.ImageData = (unsigned char *)ImgPtr;
			i=1;
			while (ImageRawHeader.Char[i] == ' ' ||ImageRawHeader.Char[i] == ',') i++;
			arg1 = (int)ImageRawHeader.Char[i]-'0';
			for (ii=i+1; ii<=i+5; ii++){ 
				tem = (int)ImageRawHeader.Char[ii]-'0'; 
				if (tem>=0 && tem<10) {
					arg1 = arg1 *10 + tem;				
				}				
				else break;
			}
			if (arg1>0) {
			Dst.ImageData = 0;
			Dst.Color = RGB; //can be GRAY
       		Pt("Calling Threshold arg1= %d \r\n",arg1);
			Threshold(&Src, &Dst, arg1);
            FreeTheImage=1;
            break; 
			}else {
        		Pt("imgraw4?, check arg\r\n");
        		return 1;
			}
 
          case '5': //UnsharpAndBrighten
            if (!P_RECENTBUFFER) return 0;            
            ImgPtr = P_RECENTBUFFER;
   			Src.ImageX = dwWidth;
   			Src.ImageY = dwHeight;
   			Src.ImageBlueX = 6;  			
   			Src.ImageBlueY = 2; 
   			Src.Color = MONOCOLOR;
   			Src.ImageData = (unsigned char *)ImgPtr;
        	if (ImageRawHeader.Char[1]=='1') {
				Dst.ImageData = 0;
				Dst.Color = RGB; //can be GRAY
	       		Pt("Calling UnsharpAndBrighten\r\n");
				UnsharpAndBrighten(&Src, &Dst);
        		FreeTheImage=1;
        		break;
           	}else {
            	Pt("imgraw50,= ToRGB\r\n");
            	Dst.ImageData = (unsigned char *)MALLOC(dwWidth*dwHeight*3);
            	ToRGB(&Src, &Dst);
            	FreeTheImage=1;
            	break;
            	}
 
          case '6'://UnsharpMask
            //if (!P_SCANBUFFER) return 0;    
            //ImgPtr = P_SCANBUFFER;
            if (!P_RECENTBUFFER) return 0;            
            ImgPtr = P_RECENTBUFFER;
   			Src.ImageX = dwWidth;
   			Src.ImageY = dwHeight;
   			Src.ImageBlueX = 6;  			
   			Src.ImageBlueY = 2; 
   			Src.Color = MONOCOLOR;
   			Src.ImageData = (unsigned char *)ImgPtr;
			//read 3 args
			i=1;
			while (ImageRawHeader.Char[i] == ' ' ||ImageRawHeader.Char[i] == ',') i++;
			arg1 = (int)ImageRawHeader.Char[i]-'0';
			theCurrentInterface.printf("\r\n");
			for (ii=i+1; ii<=i+7; ii++){ 
				tem = (int)ImageRawHeader.Char[ii]-'0'; 
				if (tem>=0 && tem<10) {
					arg1 = arg1 *10 + tem;
				}else {
					i=ii+1;
					break;
				}
			}	
			arg2 = (int)ImageRawHeader.Char[i]-'0';
			for (ii=i+1; ii<=i+3; ii++){ 
				tem = (int)ImageRawHeader.Char[ii]-'0'; 
				if (tem>=0 && tem<10) {
					arg2 = arg2 *10 + tem;
				}else {
					i=ii+1;
					break;
				}
			}	
			arg3 = (int)ImageRawHeader.Char[i]-'0';
			for (ii=i+1; ii<=i+3; ii++){ 
				tem = (int)ImageRawHeader.Char[ii]-'0'; 
				if (tem>=0 && tem<10) {
					arg3 = arg3 *10 + tem;
				}else {
					i=ii+1;
					break;
				}
			}
        	if (arg1>0 & arg2>0 & arg3>0) {
        		Dst.ImageData = 0;
        		Dst.Color = RGB; //can be GRAY
        		RESET_WATCHDOG_TIMER();
	       		Pt("Calling UnsharpMask arg1-3= %d/10.0, %d/1.0, %d\r\n", arg1, arg2, arg3);
        		UnsharpMask(&Src, &Dst, arg1/10.0, arg2/1.0, arg3);
        		FreeTheImage=1;
        		break;
        	}else {
        		Pt("imgraw6? ? ?, check args\r\n");
        		return 1;
        	}
		
          case '7': //Ditortion
            if (!P_RECENTBUFFER) return 0;            
            ImgPtr = P_RECENTBUFFER;
   			Src.ImageX = dwWidth;
   			Src.ImageY = dwHeight;
   			Src.ImageBlueX = 6;  			
   			Src.ImageBlueY = 2; 
   			Src.Color = MONOCOLOR;
   			Src.ImageData = (unsigned char *)ImgPtr;
			//read 4 args
			i=1;
			while (ImageRawHeader.Char[i] == ' ' ||ImageRawHeader.Char[i] == ',') i++;
			arg1 = (int)ImageRawHeader.Char[i]-'0';
			for (ii=i+1; ii<=i+3; ii++){ 
				tem = (int)ImageRawHeader.Char[ii]-'0'; 
				if (tem>=0 && tem<10) {
					arg1 = arg1 *10 + tem;
				}else {
					i=ii+1;
					break;
				}
			}	
			arg2 = (int)ImageRawHeader.Char[i]-'0';
			for (ii=i+1; ii<=i+3; ii++){ 
				tem = (int)ImageRawHeader.Char[ii]-'0'; 
				if (tem>=0 && tem<10) {
					arg2 = arg2 *10 + tem;
				}else {
					i=ii+1;
					break;
				}
			}	
			arg3 = (int)ImageRawHeader.Char[i];
			if (arg3=='-') {
				sg=-1; i++;
			}else sg=1;
			arg3 = (int)ImageRawHeader.Char[i]-'0';
			for (ii=i+1; ii<=i+6; ii++){ 
				tem = (int)ImageRawHeader.Char[ii]-'0'; 
				if (tem>=0 && tem<10) {
					arg3 = arg3 *10 + tem;
				}else {
					i=ii+1;
					break;
				}
			}
			arg3 *=sg;
			arg4 = (int)ImageRawHeader.Char[i]-'0';
			Pt("Calling Distortion i arg4 = %d, %d, %d\r\n", i ,arg4 );
			for (ii=i+1; ii<=i+2; ii++){ 
				tem = (int)ImageRawHeader.Char[ii]-'0'; 
				if (tem>=0 && tem<10) {
					arg4 = arg4 *10 + tem;
				}else {
					break;
				}
				Pt("Calling Distortion ii tem arg4 = %d, %d, %d\r\n", ii,tem,arg4 );
			}	
		 	
        	if (arg1>=0 && arg2>=0 && arg4>=0 && arg4<=3) {
	            Dst.ImageData = 0;
	            Dst.Color = RGB; //can be GRAY
	            Distortion(&Src, &Dst, arg1, arg2, arg3, arg4);
	            FreeTheImage=1;
	            break;
        	}else {
            	Pt("imgraw7????, check args,arg1-4 = %d, %d, %d, %d\r\n", arg1, arg2, arg3, arg4 );
        		return 1;
        	}
         default: break;
      }
    }
	time2 = current_time();
    Pt("Processing time = %dms\r\n", (time2-time1)/1000);
	bool status = (ImgPtr != NULL);
    if (status) {
		if (Dst.Color == RGB){
			NewWid = Dst.ImageX; NewHgt=Dst.ImageY;
			ImgPtr = (unsigned char *)MALLOC(NewWid*NewHgt*3);
			unsigned char * src, *dst;
			src = (unsigned char *)Dst.ImageData;//ImgPtr1;
			dst = ImgPtr+2;for (int y=0; y<NewWid*NewHgt; y++) {*dst=*src++; dst+=3;}
			dst = ImgPtr+1;for (int y=0; y<NewWid*NewHgt; y++) {*dst=*src++; dst+=3;}
			dst = ImgPtr;for (int y=0; y<NewWid*NewHgt; y++) {*dst=*src++; dst+=3;}
			free(Dst.ImageData);
			SendSalaciousImage(NewWid, NewHgt, ImgPtr);
			free(ImgPtr);
	      }
	      else {
			 NewWid = Dst.ImageX; NewHgt=Dst.ImageY;
			 ImgPtr = (unsigned char *)Dst.ImageData;
	         SendChasteImage((int)NewWid, (int)NewHgt, ImgPtr);
	         if (FreeTheImage) free(ImgPtr);
	      }
		}
	time2 = current_time();
    Pt ("Total time = %dms\r\n", (time2-time1)/1000);
    return status;
}
#endif

//RawImageShip()
//
bool RawImageShip(void)
{
	bool status;

	status = RawImageShipWithParam(1);
	return status;
}



//
//	ImageShipSimple
//		Process IMGSHP command.
//		A non menu command
//
bool ImageShipSimple(int iShipType)
{
	bool	ReturnStatus = true;		// Assume success until we know otherwise.

// Get a pointer to the last captured image, then process and send it.
	if(P_SCANBUFFER != NULL)
	{
	// Get image height and width from scan driver.
		DWORD dwHeight, dwWidth;
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&dwHeight,4,NULL);
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&dwWidth,4,NULL);

		if( iShipType == 1 )
		{
			SendChasteImage((int)dwWidth, (int)dwHeight, P_SCANBUFFER);
		}
   		else if( iShipType == 2 )
		{
			RawImageShipWithParam(2);
		}
		else
		{
			ReturnStatus = false;
		}
	}
	else
		ReturnStatus = false;

	return ReturnStatus;
} // ImageShipSimple

//
//	ProcessImageShipStyleCommand
//		This routines actually parses the remaining text and an IMGSHP style command and calls SendImage to process it.  Formerly, this
//		work was done in ImageShip, but ImageTest, which processes an IMGTST command (same as IMGSHP, but sends a wedge pattern instead of
//		an image), needs to supply a buffer, as where IMGSHP wants SendImage to use the last captured image.  In order to eliminate
//		redundancy, both IMGSHP and IMGTST use this routine, and IMGSHP sends a pointer to the last captured image.
//
bool ProcessImageShipStyleCommand(UCHAR *ImageToBeSent, int XDimension, int YDimension, bool subimage)
{
	bool		ReturnStatus = true;	// Assume success until we know otherwise.

   if(ReaderConfiguration->ExtraneousTextAfterCommand->Size > 0)
	{
		HSTRING				ImageShipHeader(ReaderConfiguration->ExtraneousTextAfterCommand);
		HIMAGE				ImageParameters;
		int					MessageCommandLength;

	// Load the image parameters with the menu settings so that any parameters not specified in the command will be set by the reader
	// configuration.  If the image parameters are valid, send the image.
		ImageParameters.SetParameterDefaults();

		ImageParameters.ReportAllParameters( GET_SETTING( ReportAllHTAGParameters ) );
		ImageParameters.JPEGQualityFactor = GET_SETTING(JPEGQualityFactor);
		ImageParameters.JPEGCompressionSmoothingFactor = GET_SETTING(JPEGCompressionSmoothingFactor);
		ImageParameters.JPEGDecompressionBlockSmoothing = GET_SETTING(JPEGDecompressionBlockSmoothing);
		ImageParameters.FastJPEG = GET_SETTING(TransmitImageUseFastAlgorithmForJPEG);
		ImageParameters.TransferProtocol = GET_SETTING(TransmitImageProtocol);
		ImageParameters.PixelIncrement = GET_SETTING(TransmitImageSubsampleIncrement);

		if( subimage )
		{
         	ImageParameters.LeftBoundary = 0;
         	ImageParameters.RightBoundary = XDimension;
         	ImageParameters.TopBoundary = 0;
         	ImageParameters.BottomBoundary = YDimension;
		}
		else
		{
         	ImageParameters.LeftBoundary = GET_SETTING(TransmitImageWindowLeft);
         	ImageParameters.RightBoundary = GET_SETTING(TransmitImageWindowRight);
         	ImageParameters.TopBoundary = GET_SETTING(TransmitImageWindowTop);
         	ImageParameters.BottomBoundary = GET_SETTING(TransmitImageWindowBottom);
		}

		ImageParameters.LeftBoundary = GET_SETTING(TransmitImageWindowLeft);
		ImageParameters.RightBoundary = GET_SETTING(TransmitImageWindowRight);
		ImageParameters.TopBoundary = GET_SETTING(TransmitImageWindowTop);
		ImageParameters.BottomBoundary = GET_SETTING(TransmitImageWindowBottom);
		ImageParameters.Margin = GET_SETTING(TransmitImageMargin);
		ImageParameters.XAxisInversionApplied = GET_SETTING(InvertImageAboutXAxis);
		ImageParameters.YAxisInversionApplied = GET_SETTING(InvertImageAboutYAxis);
		ImageParameters.Format = GET_SETTING(TransmitImageFormat);
		ImageParameters.ImageFlatteningApplied = GET_SETTING(ImageFlattening);
		ImageParameters.EdgeEnhancementApplied = GET_SETTING(ImageEdgeSharpenAmount);
		ImageParameters.RotationApplied = GET_SETTING(ImageRotationValue);
		ImageParameters.FlySpeckApplied = GET_SETTING(FlySpecks);
      	ImageParameters.SnazzyApplied = GET_SETTING(Snazzy);
		ImageParameters.GlitchFixApplied = GET_SETTING(GlitchFix);
		ImageParameters.RowNoiseApplied = GET_SETTING(RowNoise);
		ImageParameters.HistogramStretchApplied = GET_SETTING(ImageStretch);
		ImageParameters.UltraTextApplied = GET_SETTING(UltraText);
      	ImageParameters.AdaptiveUltraTextApplied = GET_SETTING(AdaptiveUltraText);
		ImageParameters.InfinityFilterApplied = GET_SETTING(InfinityFilter);
		ImageParameters.GammaCorrectionApplied = GET_SETTING(GammaCorrection);
		ImageParameters.HistogramApplied = GET_SETTING(Histogram);

      	int ParseStatus = ImageParameters.ParseHeader(&ImageShipHeader, &MessageCommandLength);

		if( ParseStatus == HTAG::HTAG_OK )
		{		
         	ReturnStatus = SendImage(&ImageParameters, ImageToBeSent, XDimension, YDimension);
		}
		else
		{
         	if( GET_SETTING(GeneralDebugOn) )
			{
				char	ResultString[80];
				sprintf(ResultString, "Parse status = %d\r\n", ParseStatus);
				xputstring(ResultString);
			}

			ReturnStatus = false;
		}
	}

// No image parameters were present; send using defaults.
	else
	{
	// For the time being, process the old way if the protocol is < HTAG::HTAG_TRANSMIT_NO_PROTOCOL.     //HTAG_TRANSMIT_NO_PROTOCOL==2
		if( GET_SETTING(TransmitImageProtocol) < HTAG::HTAG_TRANSMIT_NO_PROTOCOL )                        //Default->TRANSMIT_IMAGE_HMODEM_COMPRESSED_PROTOCOL==3
		{                                                                                                 //TRANSMIT_IMAGE_NO_PROTOCOL_INFO_AT_END==0
			                                                                 											//TRANSMIT_IMAGE_DOLPHIN==1
			HSTRING		Command(20, true);

		// Get image height and width from scan driver.
			DWORD dwHeight, dwWidth;
			SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&dwHeight,4,NULL);
			SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&dwWidth,4,NULL);

			HIMAGE		NewImage(P_SCANBUFFER, (int) dwWidth, (int) dwHeight );
			NewImage.ReportAllParameters( GET_SETTING( ReportAllHTAGParameters ) );
			NewImage.JPEGQualityFactor = GET_SETTING(JPEGQualityFactor);	// use the same quality factor for now

			NewImage.PixelIncrement = 1;
		 	bool	InvertedImager = true ^ ((GET_SETTING(iEngineRotation) == IMAGER_UPSIDE_DOWN) ? true : false);		// Soon to be controlled by product configuration.

		// If ShipImagesIn640x480VGAMode is enabled, adjust the image coordinates.
			int		XAdjustment = 0;
			int		YAdjustment =  0;

         	if (subimage)
			{
            	NewImage.LeftBoundary = 0;
            	NewImage.RightBoundary = XDimension;
            	NewImage.TopBoundary = 0;
            	NewImage.BottomBoundary = YDimension;
         	}
			else if( GET_SETTING( ShipImagesIn640x480VGAMode ) )
			{
				NewImage.LeftBoundary = 0;
				NewImage.RightBoundary = 639;
				NewImage.TopBoundary = 0;
				NewImage.BottomBoundary = 479;

				XAdjustment = GET_CONFIG_SETTING( iImageXCenter ) - 320;

				if( XAdjustment < 0 )		XAdjustment = 0;
				if( ( XAdjustment + 640 ) > NewImage.XDimension )		XAdjustment = NewImage.XDimension - 640;

				YAdjustment = GET_CONFIG_SETTING( iImageYCenter ) - 240;

				if( YAdjustment < 0 )		YAdjustment = 0;
				if( ( YAdjustment + 480 ) > NewImage.YDimension )		YAdjustment = NewImage.YDimension - 480;
			}

			if( ModifyImage(NewImage.Data, &(NewImage.XDimension), &(NewImage.YDimension), &(NewImage.StartColorPattern),&(NewImage.OffsetWidth),&(NewImage.OffsetHeight), InvertedImager, (NewImage.XAxisInversionApplied) ? true : false,
				  	(NewImage.YAxisInversionApplied) ? true : false, NewImage.PixelIncrement, NewImage.LeftBoundary + XAdjustment,
				  	NewImage.RightBoundary + XAdjustment, NewImage.TopBoundary + YAdjustment, NewImage.BottomBoundary + YAdjustment, NewImage.Margin,
				  	(NewImage.SmoothingApplied) ? true : false) )
			{
//				NewImage.SubSample(1);
			 	NewImage.FormatImage(HTAG_IMAGE::HTAG_IMAGE_JPEG);
				HSTRING	ImageData(20, true);
				NewImage.CreateTaggedImage(&Command, &ImageData, true);
//				xputstring((char *)NewImage.Data, NewImage.Data->Size);
				ComDevice->Write( NewImage.Data, 0, NewImage.Data->Size, false );
				xputchar(0xff);
				xputchar(0xd9);
			}
			else
			{
			// Couldn't modify image either due to bad image or bad parameters; WOOF!
				ReturnStatus = false;
			}
		}
		else
		{
			ReturnStatus = SendImage(NULL, ImageToBeSent, XDimension, YDimension);
		}
	}

//	for (i=0;i<NewImage.ImageParameters.Size;i++) xputchar(*(NewImage.Data+i));               //debug

	return ReturnStatus;
} // ProcessImageShipStyleCommand



//
//	ImageShip
//		Process IMGSHP command.
//
bool ImageShip(void)
{
	bool	ReturnStatus = true;		// Assume success until we know otherwise.

// Get a pointer to the last captured image, then process and send it.
	if(P_SCANBUFFER != NULL)
	{
	// Get image height and width from scan driver.
		DWORD dwHeight, dwWidth;
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&dwHeight,4,NULL);
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&dwWidth,4,NULL);
		
		ReturnStatus = ProcessImageShipStyleCommand(P_SCANBUFFER, (int) dwWidth, (int) dwHeight, false);
	}
	else
	{
		ReturnStatus = false;
	}

	if( ImageShipWaitForUnTrigger )
	{
		ImageShipWaitForUnTrigger = 0;
		while(pCHWTrigger->IsTriggerPulled())		{RESET_WATCHDOG_TIMER();}
	}

	return ReturnStatus;
} // ImageShip



// ImageSnapandShip uses only the snap menu parameters and not command line modifiers
void ImageSnapAndShip(void)
{
	// Initialize settings
	LeaveAimerOn         = GET_SETTING(ImageSnapAimerState);                    //0;
	AcceptanceDelta      = GET_SETTING(ImageSnapAcceptanceDelta);               //25;
	FrameRate            = GET_SETTING(ImageSnapFrameRate);                     //30;
	LedState             = GET_SETTING(ImageSnapLedState);                      //0;
 	ImagingStyle         = GET_SETTING(ImageSnapStyle);                         //1;
	NumberOfTries        = GET_SETTING(ImageSnapAttempts);                      //6;
	TargetWhiteValue     = GET_SETTING(ImageSnapWhiteTarget);                   //90;
 	ExpoValue            = GET_SETTING(ImageSnapExposure);                      //7874;
 	GainValue            = GET_SETTING(ImageSnapGain);                          //2;
	TargetSetPoint       = GET_SETTING(ImageSnapPercent);                       //50;
	WaitForTrigger			= GET_SETTING(ImageSnapTriggerMode);                   //0;
	BeepAfterSnap			= GET_SETTING(ImageSnapBeep);                          //0;
	ConformMe				= GET_SETTING(ImageSnapMustConform);                   //1;



	// Allow 1000 milli seconds max exposure
	// (versus 1000ms) max exposure if the illumination leds are enabled.
	// Adjusted for light bar units
	//
	const int MaxExposure = EXPSNP_JADE;		// 128 uS * 7874, Exposure limit of ~1000ms. No Lights only

	const int LightExposure = EXPSNP_JADE;	  	//Value used : const int MaxExposure = 7874;  // Exposure limit of 1000ms.
															// this 1000ms exposure limit is enforced only if the LEDS will be used
															//for the image capture.

	if( (ExpoValue > LightExposure) && (LedState != IMAGE_LEDS_NONE) )        //have lights on
	{
		ExpoValue = LightExposure;
	}
	else if( ( ExpoValue > MaxExposure ) )                     //lights off
	{
		ExpoValue = MaxExposure;
	}

	if (ImagingStyle == IMAGE_STYLE_DECODER)
	{
		SimulateDecoding(0);
	}
	else if (ImagingStyle == IMAGE_STYLE_PHOTO)
	{
		TakePhoto();
	}
	else if (ImagingStyle == IMAGE_STYLE_MANUAL)
	{
		int xFrameRate;
		for (xFrameRate=0; xFrameRate< sizeof(IC_Media_Frames)/sizeof(int); xFrameRate++)
		{
			 if (IC_Media_Frames[xFrameRate] <= FrameRate)break;
		}
		/*check to see if the search found it by seeing if loop control is what cause for loop to bail. If so we want to use the last frame rate
		* in the buffer instead of accessing one past the end of the array IC_Media_Frames*/
		if(xFrameRate == sizeof(IC_Media_Frames)/sizeof(int))xFrameRate--;
		FrameRate = IC_Media_Frames[xFrameRate];
		GrabFrame();
	}

	if( BeepAfterSnap )		SoundGoodReadBeep();
   
// Get image height and width from scan driver.
   DWORD dwHeight, dwWidth;
   SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&dwHeight,4,NULL);
   SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&dwWidth,4,NULL);


   if( GET_SETTING(ShipRawImage) )
	{
		 SendChasteImage((int)dwWidth, (int)dwHeight, P_SCANBUFFER);
	}
   else
	{
		SendImage(NULL, P_SCANBUFFER, (int) dwWidth, (int) dwHeight );
	}

	while(pCHWTrigger->IsTriggerPulled())		{RESET_WATCHDOG_TIMER();}
}


//Non menu command test function to take a decoder type image (command ZP)
void Take1dImage(void)
{
	int temp = LedState;
	LedState = 1;
	SimulateDecoding(4);
	LedState = temp;
}



//Menu command test function to take two photo style images
bool TakeTwoImages(void)
{
   DWORD dwHeight, dwWidth;
   unsigned char *FirstPtr;
   SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&dwHeight,4,NULL);
   SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&dwWidth,4,NULL);
   AcceptanceDelta      = GET_SETTING(ImageSnapAcceptanceDelta);
	FrameRate            = GET_SETTING(ImageSnapFrameRate);
   LedState             = 1;
 	ImagingStyle         = GET_SETTING(ImageSnapStyle);
	NumberOfTries        = GET_SETTING(ImageSnapAttempts);
	TargetWhiteValue     = GET_SETTING(ImageSnapWhiteTarget);
 	ExpoValue            = GET_SETTING(ImageSnapExposure);
 	GainValue            = GET_SETTING(ImageSnapGain);
	TargetSetPoint       = GET_SETTING(ImageSnapPercent);
	BeepAfterSnap			= GET_SETTING(ImageSnapBeep);
	WaitForTrigger			= GET_SETTING(ImageSnapTriggerMode);
	ConformMe				= GET_SETTING(ImageSnapMustConform);
   TakePhoto();
   FirstPtr = (unsigned char *)malloc(dwWidth*dwHeight);
   Fast_memcpy(FirstPtr,P_SCANBUFFER,dwWidth*dwHeight);
   LedState             = 0;
   NumberOfTries        = 1;
   AcceptanceDelta      = 255;
   TakePhoto();
   SendChasteImage((int)dwWidth, (int)dwHeight, FirstPtr);
   SendChasteImage((int)dwWidth, (int)dwHeight, P_SCANBUFFER);
   free (FirstPtr);

   return (true);
}




//
//	ImageTest
//		Process IMGTST command.
//		A menu command test routine
int		ImageTestStartValue = 0;

bool ImageTest(void)
{
	bool	ReturnStatus = true;		// Assume success until we know otherwise.

// Create a wedge pattern, then process and send it.
	int	XMax = GET_SETTING( ImageTestXDimension );
	int	YMax = GET_SETTING( ImageTestYDimension );
	UCHAR	*WedgePattern = new UCHAR[XMax * YMax + 2];
	int		i;

	for(i = 0;    i < (XMax * YMax);    ++i)
	{
		WedgePattern[i] = ((UCHAR) (255 - (((i % XMax) + ImageTestStartValue ) & 0xFF))); // + ImageTestStartValue;
	}

	if( GET_SETTING( ImageTestHorizontalRoll ) )
	{
		ImageTestStartValue = ( ImageTestStartValue + 1 ) % 256;
	}

   ReturnStatus = ProcessImageShipStyleCommand(WedgePattern, XMax, YMax, false);

	delete[] WedgePattern;

	return ReturnStatus;
} // ImageTest



//Used to access image hw file read function
//a menu test function
bool ImageRegisterRead(void)
{
	if (fImageRegisterRead != NULL)
	{
		return (fImageRegisterRead())?true:false;
	}
	else return 0;
}


//Used to access image hw file read function
//a menu test function
bool ImageRegisterWrite(void)
{
	if (fImageRegisterWrite != NULL)
	{
		return (fImageRegisterWrite())?true:false;
	}
	else return 0;
}




// struct for LinearRegression()
//local type only
typedef struct
{
	float slope, intercept;
} line_t;

// LinearRegression() computes the line that best fits a set of points using the least-squares method.
// 	Since slopes of vertical lines approach infinity, lines that are vertical need to be translated to horisontal (Swap
//		the X and Y values.)
static line_t linear_regression(int *x_points, int *y_points, int num_points) {
	long x_avg = 0;
	long y_avg = 0;
	long  numerator = 0;
	long denominator = 0;
	int i;
	line_t line;

	for (i=0; i<num_points; i++)
	{
      x_avg += *(x_points+i);
      y_avg += *(y_points+i);
	}
	x_avg = ((long) (((int) x_avg + num_points / 2) / num_points));
	y_avg = ((long) (((int) y_avg + num_points / 2) / num_points));

	for (i=0; i<num_points; i++)
	{
      numerator   += (*(x_points+i) - x_avg) * ((long) *(y_points+i) - y_avg);
      denominator += (*(x_points+i) - x_avg) * (*(x_points+i) - x_avg);
	}

	if (denominator == 0) denominator++;

   float b = (float)numerator/(float)(denominator);
   float a =  (float)y_avg - (b * (float)x_avg);
	line.slope = b;
	line.intercept = a;

	return line;
}

//This routine computes the Row Noise Correction values.
// Assumptions: Image is uniformly illuminated, not saturated, featureless, target white 160 (+/- 30)
//              Row noise changes linearly through the image, and is uniform horizontally.
// Process: Each row of the image is averaged. The ratio of each even/odd row pair is generated. The ratios are then line fit
//          to produce the correction ratio for each line pair. The result is the ratios of the top and bottom line pairs.
//          The ratio R is expressed as 256*R-256
//a menu command test function
bool RowNoiseReduction(void)
{
// Get image height and width from scan driver.
	DWORD dwHeight, dwWidth;
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&dwHeight,4,NULL);
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&dwWidth,4,NULL);
	int		ImageXDimension = (int) dwWidth;
	int		ImageYDimension = (int) dwHeight;

   unsigned char *ptr0, *ptr1;
   int a, b, *sums, *indexes;
   int x, y;
   line_t line;
   sums = (int *) malloc(ImageYDimension*sizeof(int));
   if(NULL != sums)
   {
   		indexes = (int *) malloc(ImageYDimension*sizeof(int));
   		if(NULL != indexes)
   		{

			for (y=0; y<ImageYDimension; y+=2)
			{
      			ptr0 = P_SCANBUFFER + ImageXDimension * y + ImageXDimension/4; // Even rows
      			ptr1 = ptr0 + ImageXDimension; // Odd rows
      			a = b = 0;

      			for (x=0; x<ImageXDimension/2; x++)
				{
        			a += (int)((unsigned int)*ptr0++);
         			b += (int)((unsigned int)*ptr1++);
      			}

      			sums[y>>1] = (int)(.5 + (float)a * 256.0 / (float)b);
      			indexes[y>>1] = y;
			}
   			line = linear_regression(&indexes[ImageYDimension/8],&sums[ImageYDimension/8],3*ImageYDimension/4/2);
   			printf("\r\nTopRatio=%d Put in 040835\r\nBottomRatio=%d Put in 040836\r\n",(int)(line.intercept+.5) - 256,(int)(line.slope *(959.0) + line.intercept + .5) - 256);
   			free(indexes);
   		}
   		free(sums);
   }

   return(1);
}




void Dummy(void){BeepAfterSnap = BeepAfterSnap + LedState + LeaveAimerOn + WaitForTrigger + ImageShipWaitForUnTrigger;}  //get rid of warnings since unused right now   lmc

//#ifdef DEVELOPMENT_SETTINGS
void SendValueToBeColorGraphed(int value, int fullscale, int color)
{
   unsigned int Size;
   unsigned char Header[7];
   char Cmd[40];
   sprintf(Cmd,"COLOR\x1d%d,%d,%d", value, fullscale, color);
   Header[0] = 0x02; Header[1] = 0xfe;
   Size = strlen(Cmd)+1; // add trailing zero
   LittleEndien4bytes(Size, &Header[2]);
   Header[6] = 0x0d;
   theStdInterface.Write(Header, 7);
   theStdInterface.Write((unsigned char *)Cmd, strlen(Cmd)+1);
}
//#endif



