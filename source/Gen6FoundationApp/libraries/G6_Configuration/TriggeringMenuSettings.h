//======================================================================================
// TriggeringMenuSettings.h
//======================================================================================
// $RCSfile: TriggeringMenuSettings.h $
// $Revision: 1.14 $
// $Date: 2011/11/10 20:35:30EST $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Module Interface Description:
//      This file contains the interface definition to a routine that creates
//		triggering menu settings as part of the overall reader configuration for
//		a Matrix device.  It also contains the active value declarations of
//		triggering settings.
//
//---------------------------------------------------------------------------


#ifndef TriggeringMenuSettingsH
#define TriggeringMenuSettingsH
//---------------------------------------------------------------------------


#include	"language.h"					// system global definitions





#define	TRGMOD_MANUAL				0
#define	TRGMOD_AUTO					1
#define	TRGMOD_MANUAL_LO			2
#define	TRGMOD_PRESENT				3
#define	TRGMOD_SSTAND				4
#define	TRGMOD_NOTIFY_HOST		5
#define	TRGMOD_SNAP_AND_SHIP		6
#define 	TRGMOD_TEMP_MANUAL		7
#define  TRGMOD_STREAM_PRESENT	8
int			TriggerMode;

#define  TRGMMF_NONE          0
#define  TRGMMF_REREAD_FOV    1         // barcode will reread after tomeout while still in field of view
int TriggerModeModifierFlag;

#define FLASH_LED_HIGH			150
#define FLASH_LED_MEDIUM           120
#define FLASH_LED_LOW			100
#define FLASH_LED_OFF			0

#if defined(SM4100)
int		TriggerReleaseDebounceTimer;
#endif
#if defined(SM4100) || defined(IR_TRIGGER)
bool	bIRSameCodeDelayEnable;
#endif

int		LowPowerTimeOut;
int		SerialTrigTimeOut;
int		PresModeTimeOut;
bool 		bAllowBufferedOutput;
bool		PresModePriceChecker;  // if true: the LEDs stay on after a decode, else, off after a decode
int		PresModeSensitivity;   // 0-20, 0 is most sensitive, 1 is nominal
int		PresModeNumbTimeout; // how long unit stays off after a read with trgpck0
int		PresModePreAimerDelay; // how long aimer is on before LEDs.
int		PresModeLowLights;
int		PresModeMitigationLowLights; //Heat mitigation for trig mod 3 presentation mode
int 		PresModeLowLightTarget;
bool		ScanStandSwitchEn;
bool		bIgnoreTrigger;
#ifdef VUQUEST3310/*External trigger for 3310*/
bool		ExternalTrgEnb;
bool		ExternalTrgPolar;
#endif
#if defined(RFSCAN) || defined(RFBASE)
int		AllowTriggerInBase;
int		AllowTriggerInChargeOnlyBase;
#define PREVENT_TRIGGERING_IN_CRADLE	0
#define ALLOW_TRIGGERING_IN_CRADLE		1
#define TURN_OFF_SCANNER_IN_CRADLE		2

#endif

#if defined(SM3100) || defined(SM3110)|| defined(SM5130)
bool  bTriggerAimerSync; //true--trigger&aimer sync; false-- trigger&Aimer are independent
bool  bAimerIndependOn;  //when trigger&Aimer are independent, true--aimer always on  false --aimer always off
#endif
bool  OpticalTriggerLowPower;            //Heat Mitigation
int 	PresModeUpTimeout;                 //Heat Mitigation
bool	PresModeLEDRampDown;               //Heat Mitigation

//ewr04791 Codegate
bool 	bAimerOutOfStandCodeGate;

#ifdef DEVELOPMENT_SETTINGS
#define SIZE_OF_SECONDARY_TRIGGER_STRING 128
bool AllowSecondaryTriggerMode;
int TriggerClickTime;
int TriggerInterClickTime;
char SecondaryTriggerStartCommand[SIZE_OF_SECONDARY_TRIGGER_STRING+1];
char SecondaryTriggerEndCommand[SIZE_OF_SECONDARY_TRIGGER_STRING+1];
#endif

void CreateTriggeringMenuSettings( int ControlMask, UINT Two_DCommandModifier, UINT HeatMitigationFixedSettingsCommandModifier );


#endif // TriggeringMenuSettingsH
//----------------------------------------------------------------------------




