//======================================================================================
// DiagnosticAndDebugMenuSettings.h
//======================================================================================
// $RCSfile: DiagnosticAndDebugMenuSettings.h $
// $Revision: 1.7 $
// $Date: 2011/05/04 17:35:10EDT $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Module Interface Description:
//      This file contains the interface definition to a routine that creates
//		diagnostic and debug menu settings as part of the overall reader
//		configuration for a Matrix device.  It also contains the active value
//		declarations of diagnostic and debug menu settings.
//
//---------------------------------------------------------------------------


#ifndef DiagnosticAndDebugMenuSettingsH
#define DiagnosticAndDebugMenuSettingsH
//---------------------------------------------------------------------------


#include	"language.h"					// system global definitions
#define RX_FIFO_MENU

//****use these defines to enable different product tests*****
#define ENABLE_FLEX_TESTS 1
#define ENABLE_CURRENT_TESTS 1
//#define	ENABLE_DR_ALLIGNMENT 1


//Temp settings
bool			bTrigger232OnPowerup;
bool			bKillTheDog;
int			AutoTriggergreyband;
int			AutoTriggerUnTriggerEdges;
int			AutoTriggerTriggerEdges;
int			iCanIPowerDown;
int			iTestFlexTestMode;
int			iTestCurrentIlluminationState;
int			iTestCurrentMaximumExposure;
int			iTestDelayTime;
bool			iTestCurrentAllowContinuousImageGrab;
int			iTestCurrentLedDutyCycle;
int 		CsiRxLevel;
int			csidmaincomplete;
#if defined( DEVELOPMENT_SETTINGS ) || defined( MANUFACTURING_IMAGER_TEST )
	int		tscattempts;
	bool		tscenable;
#endif


bool		EMITestReRead;

int			DebugButton;

bool		FlashingEnabled;
int			FlashType;

#define		ALLOW_SUB_TAG_QUERY 0x7AB7
int			UserQueryAuthority;
bool		GeneralDebugOn;
bool		FlashDebugOn;
bool		TurnOffInterruptsDuringApplicationWrite;
bool		ImagerDebugOn;
bool		MenuingDebugMode;


bool		ReportAllHTAGParameters;

bool		DebugStartupTimeReporting;
bool		DebugTimeToReadReporting;
bool		DebugSignatureCapture;
bool		DebugImageTimeReporting;
bool	 	DebugSleepReporting;
int			DebugScanDriver;
int			CsiTestpatternType;
#define		TRACE_MODE_OFF			0
#define		TRACE_MESSAGE_MODE_ON	1
int			DebugCommandProccessingTraceMode;
int			MainLoopDebugReportingCycleFrequency;

int			DefaultFlashImageStartAddress;

HSTRING		*DiagnosticString;

//bool BlockImagingEnabledWhenNotAlligned;
//bool AllignmentCompletedFlag;

bool AllignmentFlagSet( void );

void CreateDiagnosticAndDebugMenuSettings( int ControlMask );


#endif // DiagnosticAndDebugMenuSettingsH
//----------------------------------------------------------------------------




