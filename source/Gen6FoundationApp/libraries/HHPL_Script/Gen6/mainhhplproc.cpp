//======================================================================================
// $RCSfile: Gen6/mainhhplproc.cpp $
// $Revision: 1.51.1.2 $
// $Date: 2011/10/14 10:44:33EDT $
//======================================================================================

//======================================================================================
#include <wchar.h>
#include "stdInclude.h"
#include "mainhhplproc.h"
#include "Thread.h"
#include "db_chrio.h"
#include "dataedit.h"
#include "ioman.h"
#include "barcodedataman.h"
#include "barcodedata.h"
#include "decodeiface.h"
#include "scandrivermatrixinterface.h"
#include "ScanDriverImagingInterface.h"
#include "beep.h"
#include "timeout.h"
#include "menu.h"
#include "Interface_Support_Functions.h"
#include "eci.h"
#include "barcodefifo.h"
#include "EditScript.h"
#include "imaging.h"
#include "db_time.h"
#include "HTAGDataResult.h"
#include "productconfiguration.h"
#include "readerconfiguration.h"
#include "MatrixSynFECommandHandler.h"
#include "OutputModes.h"
#include "mainscript.h"
#include "RemRouter.h"
#include "HardwareToken.h"
#ifdef FLASH_LEDS       //if ff3 gen6 board
#include	"Flash_Illumination_Control.h"
#endif

#define	TRIGGER_5_5		1
#ifdef TRIGGER_5_5
#include "trigger_5_5.h"                                         //lmc   fixme
#else
#include "trigger.h"
#endif	//end #ifdef TRIGGER_5_5


#ifdef PLUGIN_DEV
#include "HONPluginConfiguration.h"
#include "PluginMenuProcess.h"
#include "PluginProcessDecoding.h"
#endif

extern void FlashAndReSynchCustomerDefaults( void );
extern void FlashWorkingChanges( void );
extern void SendStringToInterface( const char *pucBuffer, int SizeOfBuffer, bool /* DoNotUseIndicators */ );
extern volatile UINT	BaselineClock;		// a 1 ms granularity Clock for use by the Decoder (and others?!)
bool SendAsWrappedBarcode(CBarcodeData * pCurrentMsg); // this should get moved somewher else !!!!????
void ScriptThread(void *);

static const wchar_t NoReadMsg[] = L"NR";

volatile bool gOkToPowerDown=FALSE;
extern	bool bscanenable;

#ifdef HI2D_T
unsigned int try_dec_times = 0;
#endif
//==========================================================================================
// SHOTGUN support
// This is the codeid of the shotgun entries which signifies that a
// particular entry in the array is blank.
#define	DEFAULT_CODEID 		0

// This determines how much shotgun history we track, before allowing
// rescans of the same barcode.
#define	MAX_SHOTGUN_HISTORY	20

// These arrays are used to track what barcodes have been scanned
// during a shotgun trigger pull.
BYTE	g_chShotgunCodeID[ MAX_SHOTGUN_HISTORY ] = { DEFAULT_CODEID };
INT32	g_nShotgunLength[ MAX_SHOTGUN_HISTORY ];
INT32	g_nShotgunSum[ MAX_SHOTGUN_HISTORY ];
INT32	g_nShotgunSSum[ MAX_SHOTGUN_HISTORY ];

//==========================================================================================

extern CBarcodeFifo *DecodeResults;


/* --------------------------------------------------
*  ScriptThread:
 * purpose this routine is run as a second thread
 *    and provides main script function
* --------------------------------------------------*/
void ScriptThread(void *)
{
    // Enable full HHPL
    while (true)
    {
        theScriptInterpreter.EnableFullEditing();

        bool	MainScriptFoundInFlashDisk = false;
        HSTRING	MainScript(256,true);

        // Any active script available?
        if (0)//FIXME ReadMainScriptFromFlash(GET_SETTING(ActiveMainScript), &MainScript))
        {
            theScriptInterpreter.SetScriptType( false );
            theScriptInterpreter.ProcessHHPLProgram(MainScript.Char, MainScript.Size, L"" , 0);

            if( !theScriptInterpreter.HHPLProgramTerminatedBecauseScriptingIsNotAvailable )
                MainScriptFoundInFlashDisk = true;
        }

        if( !MainScriptFoundInFlashDisk )
        {
            theScriptInterpreter.SetScriptType( true );
            theScriptInterpreter.ProcessHHPLProgram(mainscript,mainscriptSize, L"", 0);
        }
    }
}

threadhandle_t StartScript(void)
{
    threadhandle_t ScriptHandle=MT_OpenDriverThread(0,SCRIPTSTACKSIZE,ScriptThread,NULL);
    if (ScriptHandle)
    {
        WakeDriverThread(ScriptHandle);
    }
    return ScriptHandle;
}






MAIN_HHPL_PROC *g_pScriptInterpreter=NULL;

void MAIN_HHPL_PROC::CreateScriptInterpreter(void)
{
    COutputModes::CreateOutputModes();

    if (g_pScriptInterpreter == NULL)
    {
        g_pScriptInterpreter = new MAIN_HHPL_PROC;
    }
    ASSERT(g_pScriptInterpreter != NULL);
}

void MAIN_HHPL_PROC::DestroyScriptInterpreter(void)
{
    delete g_pScriptInterpreter;
    g_pScriptInterpreter = NULL;

    COutputModes::DestroyOutputModes();
}



// ctor
MAIN_HHPL_PROC::MAIN_HHPL_PROC(void)
        : m_OutputMode(Standard)
{

    // data members
    mResetScanningState=false;
    m_Injected=false;  			// tool for testing translations
    m_TriggerlessBarcode=0; 	// Typically from DECODE Menu command
    ScriptTimerStart=0;
    m_getScanFailure=0;		// Failure count from GetNextScan calls
    bOposEnable=true;		// Default on, will be disabled in OnChangedConfiguration if needed
    bOposMenuDisable=false;
    scanningstate=STATE_WAIT_FOR_TRIGGER;
    mAllowBuffering=false;
    mFirstTriggerReceived=false;
    BackgroundOperationsState = false;
    bCtsManualMode = false;
    DefaultScriptIsRunning = true;	// Indicate that we're running the default script until instantiator tells us otherwise.
    HostIsRespondingToDataTransmission = true;
    WaitingForHostToRespondToLastDataTransmission = false;
    ReceivedHostAcknowledgement = true;		// Set to true to indicate that there are no host acknowledgements outstanding.
    HostIndicatedThatLastDataTransmissionWasSuccessful = true;
    CodeXMLPktNumber = 1;
	bReceivedDC2 = false; 	//Serial DC2 status.
    m_OutputMode=Standard;
    m_tscenable=0;
    m_tscattempts=0;

	 iSequenceBarcodeCountPartial = -1;

#ifndef RFSCAN

    m_pCurrentMessage = NULL;
#endif /* not RFSCAN */

    ScriptEvent =MT_CreateEvent(false);

    m_FinalResultsQueue= new CBarcodeFifo(BarcodeFifoSize);
    m_FormatterResultsQueue= new CBarcodeFifo(BarcodeFifoSize);
    GoodReadIndicateTimeout = new CTimeoutmS();


    OnChangedConfiguration();	// initial call
}


MAIN_HHPL_PROC::~MAIN_HHPL_PROC(void)
{
    delete m_FormatterResultsQueue;
    delete m_FinalResultsQueue;
    delete GoodReadIndicateTimeout;
}

/*----------------------------------------------------------------------------------------
SetOPOSDisableMode

  Note:  this info obtained by reverse engineering the code.  The code in the #ifdef RFSCAN
  has not been reverse engineered, so these comments pertain to corded.

  The clue to what this does lies in the menu command "OPSENA" which sets bOposMenuEnable.
  In summary, if OPSENA is enabled, the scanner will be disabled and issue a message under
  the following conditions:
	1.  On power up.
	2.  When menued from OFF to ON.

  See the menu command documentation for details.  The global variable bOposMenuDisable
  is used to ensure the two above conditions are met.  It probably could have a
  a different name, like g_bDoOnce or something.  The global variable bOposEnable is used
  by MAIN_HHPL_PROC for enabling/disabling output, etc.

  Parameters:  none

  Returns:  nothing
----------------------------------------------------------------------------------------*/
void MAIN_HHPL_PROC::SetOPOSDisableMode(void)
{


    if( GET_SETTING(bOposMenuEnable) )	// menued on - send power on reset message once
    {
        if (!bOposMenuDisable)
        {
            DisableOPOS();								// just changed to on so send msg	and disable
            theStdInterface.printf("PHHTPADA\005!");	//OPOS powerup string
        }
        bOposMenuDisable = true;						// prevent re doing this on every call
    }
    else	// Menued off - set OPOS men disable to false so message will be sent if/when OPOS is re-enabled
    {
        bOposMenuDisable = false;
    }
}


//
//	ServiceMainScriptTimers
//		Service the good read timeout.
//
//		When the good read timeout has expired:
//		1) For Verifiers, indicate that the host is not responding if we were waiting for an acknowledgement to data transmission.
//		2) For other RF products, if we haven't received acknowledgement yet from the host, do the error indication and indicate that
//			we're not waiting for an acknowledgement anymore.
//
void MAIN_HHPL_PROC::ServiceMainScriptTimers( void )
{
    if(GoodReadIndicateTimeout->HasExpired())
    {
        if( WaitingForHostToRespondToLastDataTransmission )
        {
            HostIsRespondingToDataTransmission = false;
            WaitingForHostToRespondToLastDataTransmission = false;
        }

        if(!ReceivedHostAcknowledgement)
        {
            SoundErrorBeep();
            ReceivedHostAcknowledgement=true;
        }

    }
    return;
} // ServiceMainScriptTimers


//
//	InitializeMainScriptTimers
//		Reset the good read timeout.
//
void MAIN_HHPL_PROC::InitializeMainScriptTimers( void )
{

    return;
} // InitializeMainScriptTimers




//
//	HostAcknowledgedLastDataTransmission
//
void MAIN_HHPL_PROC::HostAcknowledgedLastDataTransmission( bool DataWasReceivedSuccessfullyByHost )
{
    ReceivedHostAcknowledgement = true;
    HostIsRespondingToDataTransmission = true;
    WaitingForHostToRespondToLastDataTransmission = false;
    HostIndicatedThatLastDataTransmissionWasSuccessful = DataWasReceivedSuccessfullyByHost;

    return;
} // HostAcknowledgedLastDataTransmission



//
//	HostIsTalkingToUs
//		When in Verifier buffering mode, we set a flag indicating that the host is not talking to us when we fail to receive an
//		acknowledgement to data transmission.  This routine is called by the command processor to indicate that the host is now
//		talking to us.
//
void MAIN_HHPL_PROC::HostIsTalkingToUs( void )
{
    HostIsRespondingToDataTransmission = true;

    return;
} // HostIsTalkingToUs



//
//	StartCountingForAckTimeout
//		Start the countdown for receipt of host acknowledgement after transferring data.
//
void MAIN_HHPL_PROC::StartCountingForAckTimeout(void)
{
#ifdef EMERALD_VERIFIER

    if(!GET_SETTING(VerifierDataBuffering))
#else

        if(GET_SETTING(bEnableHostAck)) // Is host ack enabled?
#endif

        {
            GoodReadIndicateTimeout->SetExpiration( GET_SETTING( iHostAckTimeout ) * MilliSec); // 10 second ACK timeout for host ack!!
            WaitingForHostToRespondToLastDataTransmission = true;
        }
    ReceivedHostAcknowledgement = false;
} // StartCountingForAckTimeout


//
//	IsAckRequired
//		Determine whether or not host acknowledgement of data transmission is required.
//
#ifdef EMERALD_VERIFIER
bool MAIN_HHPL_PROC::IsAckRequired(void)
{
    return (GET_SETTING(VerifierDataBuffering));
} // IsAckRequired (verifier)
#elif defined(RFSCAN)

#else	// Not defined EMERALD_VERIFIER or RFSCAN

bool MAIN_HHPL_PROC::IsAckRequired(void)
{
    return (GET_SETTING(bEnableHostAck));
} // IsAckRequired (corded)

#endif

//Serial D/E mode.
//	IsDisableLights():
//	  If scanner is disabled by a 'D' serial command, keep the illumination lights OFF on a trigger pull.
//    The scanner recognizes the 'D' command in Stratos aux port interface or if 232_DE1 is enabled for other serial interfaces.
//
//	  returns: false to keep illumination ON
//			   true to keep illumination OFF.
bool MAIN_HHPL_PROC::IsDisableLights(void)
{
	if( GetOPOSMode() || (!(GET_SETTING(RS232CtsEnable) == CTSFST) && !(GET_SETTING(RS232SerialDisableEnable) == true)) )
	{
    	return false;
	}
	else
	{
		return true;;
	}
} //end IsDisableLights()


//
//	IsAckTimeoutPending
//		Start the countdown for receipt of host acknowledgement after transferring data.
//
bool MAIN_HHPL_PROC::IsAckTimeoutPending(void)
{
    return ( ReceivedHostAcknowledgement == false );
} // IsAckTimeoutPending


void MAIN_HHPL_PROC::OnChangedConfiguration(void)
{
    m_tscattempts=m_tscenable=0;			// dof test

#if defined( DEVELOPMENT_SETTINGS ) || defined( MANUFACTURING_IMAGER_TEST )

    m_tscattempts=GET_SETTING(tscattempts);
    m_tscenable=GET_SETTING(tscenable);
#endif


    theOutputModes.OnChangedConfiguration();
    mAllowBuffering=GET_SETTING(bAllowBufferedOutput);
    ULONG DataTypes=0;
    theStdInterface.DeviceIoControl(IOCTRL_GET_PREFERED_DATATYPES, (LPARAM)&DataTypes);// ask the interface driver what it needs
    OutputMode_t SavedMode = m_OutputMode;

    if((DataTypes&IO_DATATYPE_HHLC) == IO_DATATYPE_HHLC)
        m_OutputMode=HHLC;
    else
        m_OutputMode=Standard;

    if (SavedMode != m_OutputMode)
        TerminateHHPLProgram();

    //ewr-02787
    if( (0 == theStdInterface.GetTerminalID()) && ( (3 == GET_SETTING(RS232CtsEnable)) || (4 == GET_SETTING(RS232CtsEnable)) ) )
        bCtsManualMode = true;
    else
        bCtsManualMode = false;
    SetOPOSDisableMode();
}


// Functions in this file over ride those in HHPL_PROCCESSOR and provide
//the desired behavior for the main script
void	MAIN_HHPL_PROC ::Beep(int Frequency, int Duration, int /*Volume*/)
{
    //with currrent beep code menu setting controls volume
    while (!::Beep(false,Frequency,Duration))
        ProcessBackgroundOperationsDuringScriptExecution();
    return;
}


void MAIN_HHPL_PROC ::DoBadIndicator(void)
{
    SoundErrorBeep();
}

void MAIN_HHPL_PROC ::DoGoodIndicator(void)
{
    // RF dont beep here becuse we only beep if base sends ack (non RF or dongle etc beep)
    bool	OKToDoGoodIndicator = true;

#ifdef RFSCAN
#else

    if( IsAckRequired() )	// Corded
        OKToDoGoodIndicator = false;
#endif	// RFSCAN

    if( OKToDoGoodIndicator )
        SoundGoodReadBeep();
}



void	MAIN_HHPL_PROC::SriptRunSome(void)
{
    MT_SetEvent(ScriptEvent);
}



// ProcessBackgroundOperationsDuringScriptExecution:
// Purpose: to switch context to "Main Loop" where
//    serial commands, menu processing, and auto trigger
//    functions are performed

void  MAIN_HHPL_PROC ::ProcessBackgroundOperationsDuringScriptExecution(void)
{
    //Set state of preferred timers
    theOutputModes.UpdatePreferredTimers();

    BackgroundOperationsState = true;
    MT_WaitEvent(ScriptEvent,INFINITE);
    BackgroundOperationsState = false;
    MT_ResetEvent(ScriptEvent);
}


bool	MAIN_HHPL_PROC::ScriptIsInBackgroundOperationsState( void )
{
    return BackgroundOperationsState;
}



//
//	DefaultScriptIsRunning
//		Allow the instantiator to tell us whether or not the current script is the default.  The reason we want to know is that we'll always
//		allow the default script to run, but we won't allow other scripts to run if scripting is not enabled in product configuration.
//
void MAIN_HHPL_PROC::SetScriptType( bool DefaultScript )
{
    DefaultScriptIsRunning = DefaultScript;

    return;
}


void MAIN_HHPL_PROC::ZeroSystemTimer0(void)
{
    ScriptTimerStart=BaselineClock;
}

int MAIN_HHPL_PROC::GetSystemTimer0(void)
{
    int diff = BaselineClock - ScriptTimerStart;
    if ( diff < 0)
        return 0xffffffffUL -diff;
    else
        return diff;
}


void	MAIN_HHPL_PROC::Delay(int NumberOfMillisecondsToDelay)
{
    MT_Delay(NumberOfMillisecondsToDelay);
}


int MAIN_HHPL_PROC::TerminalID(void)
{
#ifdef RFSCAN
#else

    return theStdInterface.GetTerminalID();
#endif
}


int	MAIN_HHPL_PROC::	Workgroup(void)
{
#ifdef RFSCAN
#else

    return 0;
#endif

}


void	MAIN_HHPL_PROC::ScannerID(char * Result)
{
#ifdef RFSCAN
#else

    char * temp = GET_SETTING(ShowProductName);

#endif

    strcpy(Result,temp);
}


//
//	IsScriptingAvailable
//		Overload for HHPL_PROCESSOR function to tell the processor whether or not scripting is enabled by product configuration.
//
bool MAIN_HHPL_PROC::IsScriptingAvailable( void )
{
    return ( GET_CONFIG_SETTING( iScriptingAvailable ) || DefaultScriptIsRunning );
}




// InputString:
// Purpose: Read the barcode from the decoder (or do any other input)
//
//
//int	MAIN_HHPL_PROC ::InputString(int ControlMask, HSTRING *InputData,  int MinimumLength,
//			int MaximumLength, char *DataSourceInformation, int Timeout, char *QuitFlags)
//{
//	return InputString(ControlMask, InputData,  MinimumLength, MaximumLength, (tchar_t *)DataSourceInformation, Timeout, QuitFlags);
//}

int	MAIN_HHPL_PROC ::InputString(int ControlMask, HWSTRING *InputData,  int MinimumLength,
                                 int MaximumLength, wchar_t *DataSourceInformation, int Timeout, char *QuitFlags, int *ControlFlags)
{
    *ControlFlags = HHPL_INPUT_AND_SEND_TO_HOST_DEFAULT;

    switch(m_OutputMode)
    {
        //	case HHLC:
        //	return InputHHLC(ControlMask, InputData,  MinimumLength, MaximumLength, DataSourceInformation, Timeout, QuitFlags, ControlFlags);
    default:
        return InputStandard(ControlMask, InputData,  MinimumLength, MaximumLength, DataSourceInformation, Timeout, QuitFlags, ControlFlags,NULL  );
    }
}

int	MAIN_HHPL_PROC ::InputString( INPUT_STRING_PARAMETERS *Parameters)
{
    Parameters->ControlFlags = HHPL_INPUT_AND_SEND_TO_HOST_DEFAULT;

    switch(m_OutputMode)
    {
        //	case HHLC:
        //		return InputHHLC(Parameters->ControlMask,Parameters->InputData, Parameters-> MinimumLength,
        //		                 Parameters->MaximumLength, Parameters->DataSourceInformation,
        //		                 Parameters->Timeout,Parameters-> QuitFlags, &(Parameters->ControlFlags) );
    default:
        return InputStandard(Parameters->ControlMask,Parameters->InputData, Parameters-> MinimumLength,
                             Parameters->MaximumLength, Parameters->DataSourceInformation, Parameters->Timeout,Parameters-> QuitFlags,
                             &(Parameters->ControlFlags) ,&(Parameters->DelayInMicroseconds));
    }
}



void MAIN_HHPL_PROC ::DoGoodReadFlash(void)
{
    if (GET_CONFIG_SETTING(iOpticsPopulation)==TWO_D && (
                pTrigger->GetTriggerMode()==TRGMOD_AUTO || pTrigger->GetTriggerMode()==TRGMOD_PRESENT || pTrigger->GetTriggerMode()==TRGMOD_STREAM_PRESENT))
    {
		if( !HardwareToken.IAmOEM() )
		{
        	SetLights(0,-1,-1,-1);
		}

        if (!pTrigger->HasSkipDecodeTimeoutExpired())
            pTrigger->SetSkipDecodeTimeout();
    }
}

typedef enum {
    NO_DATA,
    GOOD_DATA,
    BAD_DATA
} DataResultType;



void MAIN_HHPL_PROC::HandleDecode(int starttime)
{
	CBarcodeData *pCurrentMsg;

#ifdef DEVELOPMENT_SETTINGS
                  LastDecodeTime = BaselineClock;
#endif

                  m_Injected=false;	      // tool to simulate barcodes

                  if (theOutputModes.IsSequenceModeOff() && theOutputModes.IsPreferredModeOff() && !CTrigger::GetShotgunMode())      //lmc
                     pTrigger->UnTriggerSerial();		// if we triggered serially untrigger unit

                  if (GET_SETTING(DebugTimeToReadReporting))
                     swprintf(m_MsgTriggerToDecode, DebugBufferSize,L"<ttr=%dms>", (current_time()-starttime)/1000);	 // prepare for later
                  else
                     m_MsgTriggerToDecode[0]=0; // make sure the length of string is 0

                  if(GET_SETTING(ShowSnappyTime))
                  {
                     char	temp[ 80 ];
			#ifdef HI2D_T
			 sprintf ( temp, "Trigger to Decode Done = %d ms try %d times\r\n",(current_time()-starttime)/1000, try_dec_times);
		     try_dec_times = 0;
			#else
                     sprintf ( temp, "Trigger to Decode Done = %d ms\r\n",(current_time()-starttime)/1000);
			#endif
                     SendStringToInterface(temp, strlen( temp ), true );
                  }

                  if (DecodeResults->AnyData())
                  {
                     // If OPOS disabled change state and break
                     if(!GetOPOSMode())
                     {
                        while (DecodeResults->AnyData())
                        {
                           pCurrentMsg = DecodeResults->Read();   // empty q and free cbacoode objects
                           pCurrentMsg->Free();
                        }
						//Stratos aux port - disable beep
						if( (GET_SETTING(RS232CtsEnable) != CTSFST) && (GET_SETTING(RS232SerialDisableEnable) != true) )
						{
							SoundOposBeep();                         // OPOS disable, this needs to be a different beep
						}

                        pTrigger->PrepareForScanning();// do lights and start/stop scanning per trigger mode
                        scanningstate = STATE_WAIT_FOR_UNTRIGGER; // wait for unit to untrigger
                        ///break;
						return;
                     }

                     if( GET_SETTING(bOposAutoDisable) )
                     {
                        DisableOPOS();
                     }

                     scanningstate=STATE_PROCESS_MESSAGE;
               	}
               	else
				{
					scanningstate=STATE_WAIT_FOR_UNTRIGGER; // had a menu or append

					if( CTrigger::GetShotgunMode() || theOutputModes.IsPreferredModeOn() || theOutputModes.IsSequenceModeEnabled() )
    				{
						pTrigger->PreferredSequenceShotgunModeFinishScanning(); //Read menu, so we are finished with preferred or sequence, so stop scanning, since we block stop scanning in other finish scanning() calls, to stop flash LED flicker during repeat decodes
					}
				}

				pTrigger->FinishScanning();// do lights and start/stop scanning per trigger mode
}





//------------------------------------------------------------------------------------------
// InputString:
// Purpose: Read the barcode from the decoder as a normal string (or binary)
//
//note: 	if the decoder returns several messages we queue them and return them one at a
//			time for output
//------------------------------------------------------------------------------------------

int	MAIN_HHPL_PROC::InputStandard(int /*ControlMask*/, HWSTRING *InputData,
                                  int MinimumLength, int MaximumLength,
                                  wchar_t *DataSourceInformation, int Timeout,
                                  char * /*QuitFlags*/, int * ControlFlags, int * DelayInMicroseconds)
							  {
	mFirstTriggerReceived = false;
	//****************************************************************************************
   //****************************************************************************************

   int returnlength = -99;
   bool UseTimeout;
   DataResultType GoodData = NO_DATA;
   CBarcodeData *pCurrentMsg;
   CTimeout InputTimeout(0);
   int starttime=0,decodeStartTime=0;
   static bool bFoundData=FALSE;
   CBarcodeData *m_pCurrentMessage=NULL;
   CBarcodeData *pBCMessage1=NULL;
   CBarcodeData *pBCMessage2=NULL;
   CBarcodeData *pBCMessage3=NULL;
   CBarcodeData *pBCMessage4=NULL;

   // debuging DOF variables
   static int ReadCnt=0;
   static int AttemptCnt=0;
   static int DecodeAverage =0;


   //ewr-02787
   /*
   bCtsManualMode = false;

   if( (0 == theStdInterface.GetTerminalID()) && (3 == GET_SETTING(RS232CtsEnable)) )
   {
    	bCtsManualMode = true;
   }
   */
   //

   if(Timeout >= 0)		// if the user provided a timeout set up to use it
   {
      UseTimeout = true;
      InputTimeout.SetExpiration(Timeout*MilliSec);
   }
   else
   {
      UseTimeout = false;
   }

   while( (returnlength < -98) && !TerminateHHPLProgramAtNextInstruction )
   {

      if (mResetScanningState)
      {
         switch (scanningstate)
         {
         case STATE_WAIT_FOR_UNTRIGGER_AFTER_NOTIFY_HOST:
               mResetScanningState=false;
               break;
         case STATE_WAIT_FOR_UNTRIGGER:
         case STATE_PROCESS_MESSAGE:
         case STATE_OUTPUT_NEXT_MESSAGE:
               break;
         default:
               scanningstate=STATE_WAIT_FOR_TRIGGER;
               mResetScanningState=false;
         }
      }

      ProcessBackgroundOperationsDuringScriptExecution();     // do stuff in the main loop

      if (UseTimeout && InputTimeout.HasExpired())
      {
         returnlength =-4;
         break;		//leave while loop because we are done
      }

      if (IOCRET_YES == theStdInterface.DeviceIoControl(IOCTRL_SHALL_I_BEEP))
         DoGoodIndicator();

      //****************************************************************************************
#ifdef DEVELOPMENT_SETTINGS

      CallDevelopmentSettingSecondaryTrigger();
#endif	//end if def DEVELOPMENT_SETTINGS
      //****************************************************************************************

      switch (scanningstate)
      {
      case STATE_WAIT_FOR_TRIGGER:
		if((GET_SETTING(RS232CtsEnable)==SCANCTRL)&&(IsCtsHigh()==false)&&(AUX_RS232==GET_SETTING(TerminalID)))//add command 'D' and 'E'
		{
			SetLights(0,0,0,0);
			break;
		}
		// Commented the legacy METDES implementation. We replace the implementation with previous implemented 
		// 233_DE and changed the command to be sub tag visible.
		//else if((GET_SETTING(RS232CtsEnable)!=SCANCTRL)&&(bscanenable == FALSE)&&(GET_SETTING(bmetroenable)==TRUE))
		//{
			//SetLights(0,0,0,0);
			//break;
		//}

         //****************************************************************************************
         //****************************************************************************************

         if( TriggerHit() )	      	     //if we are triggered  (per IsUnitTriggered() in trigger.cpp class)
         {
            gOkToPowerDown=FALSE;

            //****************************************************************************************
#ifdef MANUFACTURING_IMAGER_TEST

      		if ( m_tscenable )
      		{
         		if( mFirstTriggerReceived )
         		{
            		//fake a serial trigger so we stay scanning the whole time even if HW trigger
            		// ... is released. Wanted to keep these all in this file so no chagnes to trigger.cpp
            		pTrigger->ForceAimerOff();
            		pTrigger->TriggerSerial();
            		pTrigger->PrepareForScanning();// do lights and start/stop scanning per trigger mode
            		starttime=current_time();
            		scanningstate=STATE_TRIGGERED;
            		mFirstTriggerReceived = false;
         		}
         		else
         		{
            		scanningstate = STATE_WAIT_FOR_UNTRIGGER_AFTER_FIRST_TRIGGER_IN_MANUFACTURING_IMAGER_TEST_MODE;
            		mFirstTriggerReceived = true;
            		pTrigger->ForceAimerOn();
         		}
      		}
            else
#endif 					//end  MANUFACTURING_IMAGER_TEST
               //****************************************************************************************

            {			//begin code block starting after Manu. Test

               // If we're in NOTIFY_HOST mode, notify the host and untrigger the unit.  Otherwise, prepare for scan.
               if( pTrigger->GetTriggerMode() == TRGMOD_NOTIFY_HOST)
               {

                  //****************************************************************************************
#ifdef RFSCAN
#else					//else if not RFSCAN

                  SYNFECommandHandler->SendTriggerEvent(READER_DEVICE_EVENT_PROCESSOR::MANUALLY_TRIGGERED);
#endif

                  scanningstate = STATE_WAIT_FOR_UNTRIGGER_AFTER_NOTIFY_HOST;
               }
               else if( pTrigger->GetTriggerMode() == TRGMOD_SNAP_AND_SHIP)
               {
                  CIoMan   *g_pOriginalInterface = GetCurrentInterface();

                  //****************************************************************************************
                  //****************************************************************************************

                  ImageSnapAndShip();
                  SetCurrentInterface( g_pOriginalInterface );
                  scanningstate = STATE_WAIT_FOR_UNTRIGGER_AFTER_NOTIFY_HOST;
               }
               else if ((mAllowBuffering || theStdInterface.IsAllSent()) )
               {
					ClearDecoderWorkspace();

					//Stratos aux port-  Disable scanner(no lights) when disable 'D' command is received. if METDES is enabled.
					if( !IsDisableLights() )
					{
#ifdef HI2D_T
		 				try_dec_times = 0;						 
#endif
						pTrigger->PrepareForScanning();// do lights and start/stop scanning per trigger mode
						
						if(GET_SETTING(DataRedundant)!=0)
							pTrigger->ForceLEDsOn();
					}

					scanningstate=STATE_TRIGGERED;
					starttime=current_time();
               }
            }			//end code block starting after Manu. Test
      	}
      	else	//if not ( TriggerHit() )
      	{
				if( (DoPartialSequence()) )   //if trigger release, output the partial sequence
				{
					scanningstate = STATE_OUTPUT_NEXT_MESSAGE;
					break;
				}

            for( int i=0; i<MAX_SHOTGUN_HISTORY; i++ )
            {
               g_chShotgunCodeID[i] = DEFAULT_CODEID;
            }

			if(GET_SETTING(DataRedundant)!= 0) //if trigger release, delete pBCMessage1,2,3 and 4
  			{
  				pBCMessage1->Free();
				pBCMessage1 = NULL;
				pBCMessage2->Free();
				pBCMessage2 = NULL;
				pBCMessage3->Free();
				pBCMessage3 = NULL;
				pBCMessage4->Free();
				pBCMessage4 = NULL;
  			}
			
            theOutputModes.ClearSequenceBuffer();              //clear sequence buffers if not triggered     //lmc
				theOutputModes.ClearSequenceBufferPartial();			//clear partial sequence buffers if not triggered
            theOutputModes.SetSequenceStartedIfRequired();     // if sequence required dont untrigger unless sequence complete (fake sequence already started)

            theOutputModes.ClearPreferredBuffer();             //clear preferred buffers if not triggered     //lmc
            theOutputModes.SetPreferredStartedOff();       		//indicate buffers cleared
            theOutputModes.ClearAndDeletePreferredTimers();   	// Dump the priority timeout and Dump the read before clear timeout
            theOutputModes.ClearAllowNonPreferredFlag();  		// set non preferred allowed flag to false

            bFoundData = FALSE;	//No Read initialized

            if (pTrigger->IsLowpowerTriggerMode())
               gOkToPowerDown=TRUE;
      	}
      	break;


      case STATE_WAIT_FOR_UNTRIGGER_AFTER_NOTIFY_HOST:
         // If we're in NOTIFY_HOST trigger mode, only quit if we're untriggered.  If we're no longer in NOTIFY_HOST mode (the setting was changed),
         // quit always.
         if( TriggerReleased() )
               scanningstate = STATE_WAIT_FOR_TRIGGER;

         break;

      case STATE_TRIGGERED:
         //FIXME			pTrigger->HeatMitigationHasDecodeOutput = false;			    //Heat Mitigation

         if (m_tscenable) // test mode code		  //DOF test code
         {
            if (AttemptCnt++ >= m_tscattempts)
            {
				if( CTrigger::GetShotgunMode() || theOutputModes.IsPreferredModeOn() || theOutputModes.IsSequenceModeEnabled() )
    			{
					pTrigger->PreferredSequenceShotgunModeFinishScanning(); //Read menu, so we are finished with preferred or sequence, so stop scanning, since we block stop scanning in other finish scanning() calls, to stop flash LED flicker during repeat decodes
				}
				else
				{
               		pTrigger->FinishScanning();// do lights and start/stop scanning per trigger mode
				}

               theStdInterface.printf(_T("Read the code %d times on %d attempts\r\n"),ReadCnt,--AttemptCnt);
               //				theStdInterface.printf(_T("totalTime is %d \r\n"),(DecodeAverage));
#if !defined( MANUFACTURING_IMAGER_TEST )

               theStdInterface.printf(_T("AverageDecodeTime is %d \r\n"),(DecodeAverage/m_tscattempts));
#endif

               pTrigger->UnTriggerSerial();		// if we triggered serially untrigger unit
               pTrigger->WaitForScanStand();
               scanningstate =STATE_WAIT_FOR_UNTRIGGER;
               ReadCnt=0;
               AttemptCnt=0;
               DecodeAverage =0;
               break;
            }
            else
            {
               if (0!=pTrigger->GetNextScan(nOpenCntxt,&P_SCANBUFFER))
               {
                  m_getScanFailure=0;
                  ProcessedResult = 0;
                  ClearDecoderWorkspace(); /*in this test mode we want to clear the decoder on each attempt */
                  decodeStartTime=current_time();
                  decode ((PIXEL *) P_SCANBUFFER);
                  int DecodeTime = (current_time() - decodeStartTime)/1000;
                  //					theStdInterface.printf(_T("Individual Decode Time = %d ms\r\n"),DecodeTime);
                  DecodeAverage += DecodeTime;
                  //					theStdInterface.printf(_T("Individual Decode Average = %d ms\r\n"),DecodeAverage);

                  if (ProcessedResult)// we either had a menu or a barcode so if there is somthing to send
                  {
                     if(ReadCnt++==0)
                     {
                        if (DecodeResults->AnyData())
                        {
                           scanningstate=STATE_PROCESS_MESSAGE;
                        }
                     }
                     else if(DecodeResults->AnyData())
                     {
                        pCurrentMsg = DecodeResults->Read();
                        pCurrentMsg->Free();
                     }
               	}
            	}
            	break;
         	}
   		}// end DOF test code

         // non test mode code starts here
         if (!TriggerHit())				 //if we are not triggered  (per IsUnitTriggered() in trigger.cpp class)
         {
         	if(HardwareToken.IAmVoyager1400())
			{
				//theStdInterface.printf("off\r\n");
				((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_flash.turnoff((ScanAimerIllumMode_t)GET_SETTING(ScanAimerMode), GET_SETTING(IllOnDuringDecode)/*, ((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_LedCtrlMode*/);
				((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_aimer.turnoff((ScanAimerIllumMode_t)GET_SETTING(ScanAimerMode));
         	}
			if( CTrigger::GetShotgunMode() || theOutputModes.IsPreferredModeOn() || theOutputModes.IsSequenceModeEnabled() )
    		{
				pTrigger->PreferredSequenceShotgunModeFinishScanning(); //Read menu, so we are finished with preferred or sequence, so stop scanning, since we block stop scanning in other finish scanning() calls, to stop flash LED flicker during repeat decodes
			}
			else
			{
            	pTrigger->FinishScanning();  // do lights and start/stop scanning per trigger mode
			}

            if(!GetOPOSMode())  // If OPOS disabled change state and break
            {
               scanningstate = STATE_WAIT_FOR_UNTRIGGER;
               break;
            }

            if(bFoundData==FALSE)
               NoDecodeBeep();				//Go beep on No Decode if it is set in the menu

            if (GET_SETTING(ShowNoRead) && bFoundData==FALSE)
            {
               tichar_t *pDummy;	// use this dummy to select the correct data type
               size_t NumChar=50;
               CBarcodeData *pFakeMsg = CBarcodeData::GetSaveNewBuffer(pDummy, NumChar);
               pFakeMsg->StoreHHP(NR_CODEID, 0);
               pFakeMsg->StoreAim('0', '0');
               pFakeMsg->StoreText(NoReadMsg, wcslen(NoReadMsg));
               pFakeMsg->SetControlMask( pFakeMsg->GetControlMask() | HHPL_INPUT_AND_SEND_TO_HOST_NO_INDICATORS );

               if( DoDataEditOrScript(pFakeMsg, m_FinalResultsQueue) )	// if data edit fails free barcode
               {
                  scanningstate=STATE_OUTPUT_NEXT_MESSAGE;
               }
               else
               {
                  pFakeMsg->Free();
               }
            }
            else
            {
					scanningstate = STATE_WAIT_FOR_TRIGGER;
					//add command 'D' and 'E'
					SendRtsPulse();		//send a 20ms pulse
			}

         }
         else	//else if triggered
         {
			 if ( ProcessedResult )
			 {
				 HandleDecode(starttime);
			 }
#ifdef FIXME

            if( pTrigger->bAllignmentModeEnabled )                    //4800dr allignment mode enabled	lmc
            {
               pTrigger->SearchForAllignmentModeBarcode();            //controls BlockDecoderFromExecuting
            }
#endif
            if( pTrigger->HasSkipDecodeTimeoutExpired() && !(pTrigger->GetBlockDecoderFromExecuting()) )
            {
               if (0!=pTrigger->GetNextScan(nOpenCntxt,&P_SCANBUFFER))
               {
                  m_getScanFailure=0;
					decodeStartTime=current_time();
					#ifdef HI2D_T
					try_dec_times++;
					#endif
#if defined(PLUGIN_DEV) && defined(PLUGIN_DECODER)
					DecodeWithPlugins((PIXEL *) P_SCANBUFFER);
#else
					decode ((PIXEL *) P_SCANBUFFER);
#endif

               }
               else		//else if did not GetNextScan()
               {
                  if(GET_SETTING(GeneralDebugOn))
                        xputstring("SCN_IOControl not open\r\n");
//#ifdef VUQUEST3310
#if 0
			//xputstring("Multiple 'SCN_IOControl not open' failures - reconfigure it\r\n");
			if( pTrigger->GetTriggerMode() == TRGMOD_STREAM_PRESENT || pTrigger->GetTriggerMode() == TRGMOD_PRESENT)
			{
				SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_STOP_SCANNING,NULL,0,NULL,0,NULL);
				SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_RECONFIG_ENGINE,NULL,0,NULL,0,NULL);
				SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_START_SCANNING,NULL,0,NULL,0,NULL);
			}
			else
			{
				SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_RECONFIG_ENGINE,NULL,0,NULL,0,NULL);
			}

		    if ( m_getScanFailure > 3)   // It is possible to get here by going between Presentation Mode and
		    {// Temporary Manual Trigger Mode.  See footnote 1 (below).
		    		reset_to_boot_code_hard();//Have tried to recovery but not successfully
		    }
#else
                  m_getScanFailure++;

                  if ( m_getScanFailure > 3)   // It is possible to get here by going between Presentation Mode and
                  {                            // Temporary Manual Trigger Mode.  See footnote 1 (below).
                     if(GET_SETTING(GeneralDebugOn))
                        xputstring("Multiple 'SCN_IOControl not open' failures - Powering Down\r\n");
                     //FIXME						RemovePowerOnError();
                  }
#endif
                  break;
               }

               if (GET_SETTING(ShowSnappyTime))
               {
                  char	temp[ 80 ];
                  sprintf ( temp, "Individual Decode Time = %d ms\r\n",(current_time() - decodeStartTime)/1000);
                  SendStringToInterface(temp, strlen( temp ), true );
               }

               if (ProcessedResult || m_Injected)// we either had a menu or a barcode so if there is somthing to send
               {
					HandleDecode(starttime);
               }	//end if ProcessedResult()
            }	//end if decoding blocked
      	}	//end else if trigger hit
      	break;

      case STATE_WAIT_FOR_UNTRIGGER:
         if (theOutputModes.IsSequenceModeOff() && theOutputModes.IsPreferredModeOff() && !CTrigger::GetShotgunMode())            //lmc
         {
            if (pTrigger->IsTriggeredSerial())
			{
               pTrigger->UnTriggerSerial(); // if we are already serial triggered untrigger
			}
			//Disable scanner if triggered by DC2 serial character command.
		   if( pTrigger->IsSerialDisable() && GetDC2Status() )
		   {
				theScriptInterpreter.DisableOPOS();
				theScriptInterpreter.ClearDC2Status();
		   }
            pTrigger->WaitForScanStand();
         }

         if (pTrigger->IsLowpowerTriggerMode())
            gOkToPowerDown=TRUE;

         // If OPOS disabled change state and break
         if (!GetOPOSMode())
         {
            if( TriggerReleased() )
            {
				if( CTrigger::GetShotgunMode() || theOutputModes.IsPreferredModeOn() || theOutputModes.IsSequenceModeEnabled() )
    			{
					pTrigger->PreferredSequenceShotgunModeFinishScanning(); //Read menu, so we are finished with preferred or sequence, so stop scanning, since we block stop scanning in other finish scanning() calls, to stop flash LED flicker during repeat decodes
				}
				else
				{
               		pTrigger->FinishScanning();// do lights and start/stop scanning per trigger mode
				}

               	scanningstate = STATE_WAIT_FOR_TRIGGER;
               	break;
            }
         }

         if( TriggerReleased() || (GET_CONFIG_SETTING(iOpticsPopulation)==TWO_D &&
                                    (pTrigger->GetTriggerMode()==TRGMOD_AUTO || pTrigger->GetTriggerMode()==TRGMOD_PRESENT || pTrigger->GetTriggerMode()==TRGMOD_STREAM_PRESENT)))
         {
            if ((mAllowBuffering || theStdInterface.IsAllSent()) && AckRcvdFromRFBase())
               scanningstate=STATE_WAIT_FOR_TRIGGER;
         }
         break;

      case STATE_WAIT_FOR_UNTRIGGER_AFTER_FIRST_TRIGGER_IN_MANUFACTURING_IMAGER_TEST_MODE:
         if( TriggerReleased() )
         {
            scanningstate = STATE_WAIT_FOR_TRIGGER;
         }
         break;

      case STATE_PROCESS_MESSAGE:
         DecodeBeep();		// decode beep means we got a good decode we still have to do data edit and sequence stuff
         GoodData = NO_DATA;
         while (DecodeResults->AnyData())
         {
            bool bTossDecodeResults = false;

            pCurrentMsg = DecodeResults->Read();	// we had a non menu type bar code

			//Add data redundant
			if(GET_SETTING(DataRedundant)==1)
			{
				 //xputstring("DR1\r\n");
				if((pBCMessage1 == NULL)&&(pBCMessage2 == NULL))
				{
					//xputstring("DR2\r\n");
					pBCMessage1 = pCurrentMsg;
					pCurrentMsg = NULL;
					
					//pTrigger->PrepareForScanning();// do lights and start/stop scanning per trigger mode
			        scanningstate = STATE_WAIT_FOR_TRIGGER; // wait for unit to untrigger
			        break;
				}
				else if((pBCMessage1 != NULL)&&(pBCMessage2 == NULL))
				{
					// xputstring("DR3\r\n");
					pBCMessage2 = pCurrentMsg;

					//if pBCMessage1 is same as pBCMessage2, transmit this barcode 
					//else discard pBCMessage1 and copy pBCMessage2 to pBCMessage1, and not XMIT this barcode

					if(memcmp(pBCMessage1->GetTotalData(),pBCMessage2->GetTotalData(),pBCMessage2->GetLabelSize()) != 0)
					{
						pBCMessage1->Free();
						pBCMessage1 = NULL;
						pBCMessage1 = pBCMessage2;
						pBCMessage2 = NULL;
						pCurrentMsg = NULL;
						
						//pTrigger->PrepareForScanning();// do lights and start/stop scanning per trigger mode
			            scanningstate = STATE_WAIT_FOR_TRIGGER; // wait for unit to untrigger
			            break;
					}
				}
				
			}
			else if(GET_SETTING(DataRedundant)==2)
			{
				if((pBCMessage1 == NULL)&&(pBCMessage2 == NULL)&&(pBCMessage3 == NULL))
				{
					// xputstring("DR4\r\n");
					pBCMessage1 = pCurrentMsg;
					pCurrentMsg = NULL;
					
					scanningstate = STATE_WAIT_FOR_TRIGGER; // wait for unit to untrigger
		            break;
				}
				else if((pBCMessage1 != NULL)&&(pBCMessage2 == NULL)&&(pBCMessage3 == NULL))
				{
					pBCMessage2 = pCurrentMsg;
					pCurrentMsg = NULL;

					scanningstate = STATE_WAIT_FOR_TRIGGER; // wait for unit to untrigger
		            break;
				}
				else if((pBCMessage1 != NULL)&&(pBCMessage2 != NULL)&&(pBCMessage3 == NULL))
				{
					// xputstring("DR5\r\n");
					pBCMessage3 = pCurrentMsg;

					//if pBCMessage1 is same as pBCMessage2, transmit this barcode 
					//else discard pBCMessage1 and copy pBCMessage2 to pBCMessage1, and not XMIT this barcode

					if((memcmp(pBCMessage1->GetTotalData(),pBCMessage2->GetTotalData(),pBCMessage2->GetLabelSize()) != 0)\
						||(memcmp(pBCMessage1->GetTotalData(),pBCMessage3->GetTotalData(),pBCMessage3->GetLabelSize()) != 0))
					{
						pBCMessage1->Free();
						pBCMessage1 = NULL;
						pBCMessage1 = pBCMessage2;
						pBCMessage2 = pBCMessage3;
						pBCMessage3 = NULL;
						pCurrentMsg = NULL;

						scanningstate = STATE_WAIT_FOR_TRIGGER; // wait for unit to untrigger
			            break;
					}
				}
			}
			else if(GET_SETTING(DataRedundant)==3)
			{
				if((pBCMessage1 == NULL)&&(pBCMessage2 == NULL)&&(pBCMessage3 == NULL)&&(pBCMessage4 == NULL))
				{
					// xputstring("DR6\r\n");
					pBCMessage1 = pCurrentMsg;
					pCurrentMsg = NULL;

					scanningstate = STATE_WAIT_FOR_TRIGGER; // wait for unit to untrigger
		            break;
				}
				else if((pBCMessage1 != NULL)&&(pBCMessage2 == NULL)&&(pBCMessage3 == NULL)&&(pBCMessage4 == NULL))
				{
					pBCMessage2 = pCurrentMsg;
					pCurrentMsg = NULL;

					scanningstate = STATE_WAIT_FOR_TRIGGER; // wait for unit to untrigger
		            break;
				}
				else if((pBCMessage1 != NULL)&&(pBCMessage2 != NULL)&&(pBCMessage3 == NULL)&&(pBCMessage4 == NULL))
				{
					pBCMessage3 = pCurrentMsg;
					pCurrentMsg = NULL;

					scanningstate = STATE_WAIT_FOR_TRIGGER; // wait for unit to untrigger
		            break;
				}
				else if((pBCMessage1 != NULL)&&(pBCMessage2 != NULL)&&(pBCMessage3 != NULL)&&(pBCMessage4 == NULL))
				{
					// xputstring("DR7\r\n");
					pBCMessage4 = pCurrentMsg;
				
					//if pBCMessage1 is same as pBCMessage2, transmit this barcode 
					//else discard pBCMessage1 and copy pBCMessage2 to pBCMessage1, and not XMIT this barcode
				
					if((memcmp(pBCMessage1->GetTotalData(),pBCMessage2->GetTotalData(),pBCMessage2->GetLabelSize()) != 0)\
						||(memcmp(pBCMessage1->GetTotalData(),pBCMessage3->GetTotalData(),pBCMessage3->GetLabelSize()) != 0)\
						||(memcmp(pBCMessage1->GetTotalData(),pBCMessage4->GetTotalData(),pBCMessage4->GetLabelSize()) != 0))
					{
						pBCMessage1->Free();
						pBCMessage1 = NULL;
						pBCMessage1 = pBCMessage2;
						pBCMessage2 = pBCMessage3;
						pBCMessage3 = pBCMessage4;
						pBCMessage4 = NULL;
						pCurrentMsg = NULL;

						scanningstate = STATE_WAIT_FOR_TRIGGER; // wait for unit to untrigger
			            break;
					}
				}
			}
		
            if (pCurrentMsg)
            {
               theEci.Check(pCurrentMsg);					// codepage tranlations etc.
            }
            // We now call the forrmatter and or the Edit Script. These can create several outputs.
            // Therefore we use a FIFO (m_FormatterResultsQueue) to store these intermediate results.
            if( !DoDataEditOrScript(pCurrentMsg, m_FormatterResultsQueue) )
            {
               pCurrentMsg->Free();
               GoodData = BAD_DATA;
            }
            else // format was ok
            {
               GoodData = GOOD_DATA;
               // PARKERJ - here is where we call shotgun and/or sequence.
               // We feed all the intermediate results from the edit process into the shotgun/sequencer check.
               while(m_FormatterResultsQueue->AnyData())
               {
                  pCurrentMsg = m_FormatterResultsQueue->Read();	// Get next barcode

                  if( !DoShotgun( *pCurrentMsg ) )		// shotgun found a dup msg free it
                  {
                        pCurrentMsg->Free();
                  }
                  else
                  {
                     int seqresult = DoSequences(*pCurrentMsg);	  	// apply all sequences, and then stuff data for output      //lmc
                     switch (seqresult)
                     {
                     case 1:
                        bFoundData = TRUE;
                        break;
                     case 2:
                        bTossDecodeResults = TRUE;
								break;
                     case 3:
                        bTossDecodeResults = TRUE;       //set flag to empty decode resuts Q so mutiple reads dont re trigger
                        bFoundData = TRUE;
                        break;
                     default:        //do not handle returns of 0, -1, -2, -3.
                        break;
                     }  //end switch  sequences results
               	}  //end else shotgun kept data
            	}  //end while formater results any data
         	}   //end else format ok

            if( bTossDecodeResults == true )
            {
               //if(GET_SETTING(GeneralDebugOn))	theStdInterface.printf( " Deleting Barcode %i ", (DecodeResults->AnyData()) );  	 	//lmc debug

               while( DecodeResults->AnyData() )
               {
                  (DecodeResults->Read())->Free();	//empty decode resuts Q so mutiple reads dont re trigger
               }
               bTossDecodeResults = false;
               break;
            }
         }  //end while decode results any data

			if((pCurrentMsg == NULL)&&(GET_SETTING(DataRedundant) != 0))
			{
				pTrigger->FinishScanning();
				break;
			}
		
         if( GoodData == BAD_DATA )
               SoundErrorBeep();
         // Notice there is NO BREAK here.  We intentionally fall
         // right into the next state.

      case STATE_OUTPUT_NEXT_MESSAGE: //process one message from Q of data edited messages
         if (!AckRcvdFromRFBase())
         {
               pTrigger->ResetGoodReadTime();
               break;
         }
         // we must ensure that any current object have ended its life cycle

         // if we ge here with nothing to send wait for user to
         //	retrigger and decode
         m_pCurrentMessage=m_FinalResultsQueue->Read();		//read msg and fill in output data

         if (m_pCurrentMessage==NULL)
         {
            scanningstate = STATE_WAIT_FOR_UNTRIGGER;

            if(((GET_CONFIG_SETTING(iOpticsPopulation)==TWO_D &&
                  (pTrigger->GetTriggerMode()==TRGMOD_AUTO || pTrigger->GetTriggerMode()==TRGMOD_PRESENT || pTrigger->GetTriggerMode()==TRGMOD_STREAM_PRESENT))))
				{
               scanningstate = STATE_WAIT_FOR_TRIGGER;
				}

            if( (CTrigger::GetShotgunMode() || theOutputModes.IsSequenceStarted() || theOutputModes.IsPreferredStarted()) && (ProcessedResult!=2) )      //lmc
				{
					scanningstate = STATE_WAIT_FOR_TRIGGER;
				}

				if  (m_tscenable)
            {
               scanningstate = STATE_TRIGGERED;
#ifdef MANUFACTURING_IMAGER_TEST

               pTrigger->TriggerSerial();		// force triggered until all attempt complete
#endif

            }
         }
         else
         {
            m_pCurrentMessage->GetWText(InputData);
            m_pCurrentMessage->FillDataSourceInfo(DataSourceInformation);

            if (ControlFlags)
               *ControlFlags=m_pCurrentMessage->GetControlMask();

            if (DelayInMicroseconds)
               *DelayInMicroseconds=m_pCurrentMessage->GetAdditionalInterMessageDelay();

            m_pCurrentMessage->Free();	   // can be savely called even when m_pCurrentMessage == NULL
            m_pCurrentMessage = NULL;

            //if we pass user length check return else get next msg
            if (   ((MinimumLength == -1) || (InputData->Size >= MinimumLength))
                  && ((MaximumLength == -1) || (InputData->Size <= MaximumLength)))
            {
               returnlength=InputData->Size;  // fill in length for return
            }

#if defined(SM4100)
			//if IR reset timeout is 0, no IR reset. If the timeout is not 0, do IR reset. Then we can scan barcode when IR cannot trigger again
			if ((GET_SETTING(IRResetTime)!=0)&&GET_SETTING(IRorTrigger)==1)
			{
				HAZARD_WRITE_FIELD_TO_REG(0x53FA4000,18,1,0x0);
				MT_Delay(GET_SETTING(IRResetTime));
				HAZARD_WRITE_FIELD_TO_REG(0x53FA4000,18,1,0x1);
			}
#endif
         }
         break;

      } // switch

   }// while
   scanningstate=STATE_OUTPUT_NEXT_MESSAGE;
   return returnlength;
}


#ifdef RAW_HHLC
// InputHHLC:
// Purpose: Read the barcode from the decoder as HHLC data
//

int	MAIN_HHPL_PROC::InputHHLC(int , HWSTRING *,  int ,
                              int , wchar_t *, int , char *, int *)

{
    CBarcodeData * gpCurrentDecoded=NULL;

    enum SCANSTATES
    {
        STATE_WAIT_FOR_TRIGGER,
        STATE_WAIT_FOR_UNTRIGGER,
        STATE_WAIT_FOR_LASER_ENABLE,
        STATE_SCANNING,
        STATE_WAIT_FOR_UNTRIGGER_AFTER_NOTIFY_HOST
    };
    static SCANSTATES scanningstate;
    static bool IBeeped;
    static bool EnabledDirect;
    static int BeepCount;


    scanningstate=STATE_WAIT_FOR_TRIGGER;
    IBeeped = true;
    EnabledDirect = false;
    BeepCount = 0;

    // we will stay for ever here in this loop unless someone tells us to
    // exit the loop by doing the next instuction -- which for HHLC is meaningless
    while (m_OutputMode==HHLC && !TerminateHHPLProgramAtNextInstruction)
    {
        ProcessBackgroundOperationsDuringScriptExecution();

        // - Good read input gets noise coupled onto it from laser enable and the swithing digital output. Using a BeepCount "debounce"
        //	and a 47 k in series with the good read input transistor.  Later board revs might fix/change this.
        // - 2000 good read signal occurs after enable goes away and Laser enable can go away without a good read done by the
        //	decoder so we really can't assume good read is going to occur.  BUT even though Good read input can occur
        // after enable goes away -- if Good Read is to occur -- it will always occur before another trigger
        if (!IBeeped && IOCRET_YES == theStdInterface.DeviceIoControl(IOCTRL_SHALL_I_BEEP) )
        {
            if (!BeepCount--)
            {
                SoundGoodReadBeep();
                IBeeped = true;
            }
        }

        if( TriggerHit() )                     //if we are triggered  (per IsUnitTriggered() in trigger.cpp class)
        {
            if (scanningstate!=STATE_WAIT_FOR_UNTRIGGER)
                theHal.DoSetPort(CHal::TERM_CLK_OUT);
        }
        else
            theHal.DoClrPort(CHal::TERM_CLK_OUT);
        switch (scanningstate)
        {
        case STATE_WAIT_FOR_TRIGGER:
            {
                if (IOCRET_YES == theStdInterface.DeviceIoControl(IOCTRL_SHALL_I_TRIGGER))
                {
                    gOkToPowerDown=FALSE;
                    EnabledDirect = true;
                    pTrigger->PrepareForScanning();
                    ClearDecoderWorkspace();
                    scanningstate=STATE_SCANNING;
                }
                else
                    gOkToPowerDown=TRUE;

                break;
            }

        case STATE_WAIT_FOR_LASER_ENABLE:
        case STATE_SCANNING:
            {
                if( !TriggerHit() )             //if we are not triggered  (per IsUnitTriggered() in trigger.cpp class)
                {
                    pTrigger->FinishScanning();// do lights and start/stop scanning per trigger mode
                    scanningstate=STATE_WAIT_FOR_TRIGGER;
                    gOkToPowerDown=TRUE;
                }
                else
                {
                    IBeeped = false;
                    BeepCount = 5;
                    if ( IOCRET_YES == theStdInterface.DeviceIoControl(IOCTRL_SHALL_I_UNTRIGGER) )
                    {
                        EnabledDirect = false;
                        pTrigger->FinishScanning();
                        scanningstate=STATE_WAIT_FOR_UNTRIGGER;
                        gOkToPowerDown=TRUE;
                    }
                    else
                    {
                        if (0!=pTrigger->GetNextScan(nOpenCntxt,&P_SCANBUFFER))
                        {
                            m_getScanFailure=0;
                            MatrixError_t	ret=ERR_PASS;
                            decode ((PIXEL *) P_SCANBUFFER);

                            if ((!DecodeResults->AnyData()) && ProcessedResult )			// we had a menu code
                            {
                                pTrigger->FinishScanning();
                                theHal.DoClrPort(CHal::TERM_CLK_OUT);						// remove trigger
                                scanningstate=STATE_WAIT_FOR_UNTRIGGER;
                            }
                            else
                            {
                                while (DecodeResults->AnyData())
                                    (DecodeResults->Read())->Free();
                                size_t WriteCount=0;

                                ret = GetHHLCDataFromDecoder((PIXEL *) P_SCANBUFFER,gpCurrentDecoded);	// refill buffer with timer counts
                                if(IsOK(ret))
                                {
                                    WriteCount = theStdInterface.Write(gpCurrentDecoded);
                                }
                            }
                        }
                    }
                }
                break;
            }
        case STATE_WAIT_FOR_UNTRIGGER:
            {
                if( TriggerReleased() )
                    scanningstate=STATE_WAIT_FOR_TRIGGER;
                break;
            }
        default:
            break;

        }// end switch
    }
    return 0;	// always return a 0, the script does not know about HHLC data
}
#else


#endif


// SendToHost:
// Puurpose:to output data to host interface. A CBarcode object is created with the information provideed
//
// What shall we do with the ControlMask ???? 8888888888888888888888888

bool MAIN_HHPL_PROC::SendToHost( SEND_TO_HOST_PARAMETERS *SendParameters )
{
    bool result = true;


    result =SendToHostInterface(SendParameters,&(theStdInterface),0);

    return result;
}

//
// Code for CodeXML Bluetooth Module communications
//
unsigned short crcTab[256]=
    {0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
     0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
     0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
     0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
     0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
     0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
     0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
     0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
     0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
     0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
     0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
     0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
     0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
     0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
     0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
     0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
     0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
     0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
     0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
     0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
     0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
     0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
     0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
     0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
     0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
     0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
     0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
     0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
     0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
     0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
     0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
     0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0};

typedef unsigned short crc_t;
crc_t crc(crc_t initialCrc, const UCHAR * bufPtr, size_t length)
{
    enum
    {
        crcBits = 16,
        charBits = 8,
        diffBits = crcBits - charBits
    };
    crc_t c = initialCrc;
    while (length--)
        c=(c<<charBits) ^ crcTab[(c >> diffBits) ^ *bufPtr++];
    return c;
}

#ifdef RFSCAN 	// Stub the plugin for corded and RFBASE are below

#else	// from #if RFSCAN stubbed functions for CodeXML functions

bool TranslatenonPrintableCodes_CodeXML(HSTRING * SendParameters)
{
    return false;
}
bool MAIN_HHPL_PROC::SendToCodeXMLBase( SEND_TO_HOST_PARAMETERS *SendParameters,CIoMan * SendInterface )
{
    return false;
}
bool MAIN_HHPL_PROC::Send_A_Packet(HSTRING* data, char packetNumber, CIoMan * SendInterface)
{
    return false;
}
bool MAIN_HHPL_PROC::Send_Z_Packet(HSTRING* data, char packetNumber, CIoMan * SendInterface)
{
    return false;
}
bool MAIN_HHPL_PROC::TransmitCodeXMLPacket( HSTRING *data, CIoMan * SendInterface )
{
    return false;
}
#endif
// End CodeXML Code


/*----------------------------------------------------------------------------------------
SendToHostInterface


  Footnote 1:  We call GetSaveNewBuffer to obtain a CBarcodeData object for ReM instead
  of creating an object off the heap.  We do this because a code inspection shows that
  class, CIoMan and others work quite closely together and they shouldn't be used independently.
  For example, the Free() method of CBarcodeData doesn't free any allocated memory--it just
  marks it as unread.
  
  Parameters:  

  Returns:  
----------------------------------------------------------------------------------------*/
bool MAIN_HHPL_PROC::SendToHostInterface( SEND_TO_HOST_PARAMETERS *SendParameters,CIoMan * SendInterface ,long int Timeout )
{
    const wchar_t *String;
    bool Ret=true;		// assume sucsess
    const IoDataType_t DataType = IO_DATATYPE_UNICODE;
    String = SendParameters->String;

    int LengthOfString = SendParameters->LengthOfString;

    const size_t HtagHeaderSize=7;
    const size_t DecodeHeaderSize=14;
    const size_t TotalHeaderSize=HtagHeaderSize+DecodeHeaderSize;

    size_t TotalLength = LengthOfString;
    size_t DebugTimeLength = 0;

    //ewr-02787
    /*
    	bCtsManualMode = false;

    	if( (0 == theStdInterface.GetTerminalID()) && (3 == GET_SETTING(RS232CtsEnable)) )
    	{
    		bCtsManualMode = true;
    	}
    */
    //

    if (GET_SETTING(DecodeHeader))
        TotalLength += TotalHeaderSize;

    if (GET_SETTING(DebugTimeToReadReporting))
    {
        DebugTimeLength= wcslen(m_MsgTriggerToDecode);
        TotalLength	+= DebugTimeLength;
    }

    size_t BufferLength = TotalLength;
    tichar_t *pText;
    CBarcodeData *pCurrentMsg = CBarcodeData::GetSaveNewBuffer(pText, BufferLength);
    if (pCurrentMsg==NULL)			// could not get a buffer return error
        return false;
    pCurrentMsg->CopyIDsFromDataSourceInfo	(SendParameters->DataSourceInformation);
    pCurrentMsg->SetControlMask(SendParameters->ControlFlags);
    pCurrentMsg->StoreDataType(DataType);
    pCurrentMsg->SetAdditionalInterMessageDelay(SendParameters->DelayInMicroseconds);


        if (GET_SETTING(DecodeHeader)) //decode header not compatible with CodeXML header yet.
        {

            // Async header
            TotalLength -= HtagHeaderSize; // DO NOT INCLUDE THE HEADER SIZE ITSELF !!!!
            // CAUTION: this is endian sensitive
            UCHAR *pTemp = (UCHAR *)(&TotalLength);
            swprintf (pText,BufferLength, L"\x02\xfe%lc%lc%lc%lc\x0d", pTemp[0], pTemp[1], pTemp[2], pTemp[3]);

            // Decode header
            swprintf (pText+HtagHeaderSize,BufferLength,L"091200%04d%lc%lc%lc%lc", LengthOfString+DebugTimeLength, pCurrentMsg->GetHHPId(), pCurrentMsg->GetAimId(), pCurrentMsg->GetAimModifier(), 0x1d);

            pCurrentMsg->FinalizeBuffer(TotalHeaderSize);  							// store the lenght of wriiten characters
        }
    // Barcode data

        pCurrentMsg->AddText(String, LengthOfString);

    if (DebugTimeLength != 0)
    {
        pCurrentMsg->AddText (m_MsgTriggerToDecode, DebugTimeLength);
    }
    if(( SendInterface == g_pStdInterface ))
    {
        if( !WaitingForHostToRespondToLastDataTransmission )
        {
            CTimeout SendTimeout(Timeout);
            //ewr-02787
            bCtsManualMode = ( (IOCRET_YES == theStdInterface.DeviceIoControl(IOCTRL_CHECK_IF_WE_ARE_IN_232_CTS_MAN)) )?true:false;
            //
            while (Ret && 0==SendInterface->Write(pCurrentMsg))
            {
                ProcessBackgroundOperationsDuringScriptExecution();
                if( (Timeout) && (SendTimeout.HasExpired()) && !bCtsManualMode )
                {
                    Ret =false;
                    pCurrentMsg->Free();
                    pCurrentMsg = NULL;
                }
            }	//end while

            //ewr-02787
            /*
            			if( bCtsManualMode && !Ret )
            			{
            				SoundErrorBeep();
            			}
            			else*/ if( bCtsManualMode )
            {
                //ewr-02787
                if( (ERR_PASS == theStdInterface.DeviceIoControl(IOCTRL_CHECK_IF_MESSAGE_NOT_SENT_IN_232_CTS_MAN)) )
                    //
                {
                    if (pCurrentMsg)                     //make sure no memory leak if no longer usb
                    {
                        pCurrentMsg->Free();
                        pCurrentMsg = NULL;
                    }
                    //ewr-02787
                    theStdInterface.DeviceIoControl(IOCTRL_ACK_BACK_TO_IO_THAT_MESSAGE_NOT_SENT_IN_232_CTS_MAN);
                    //
                    Ret = FALSE;
                    SoundErrorBeep();
                }
                else if (!(pCurrentMsg->GetControlMask() & HHPL_SEND_TO_HOST_NO_INDICATORS))
                {
                    if (IOCRET_YES == theStdInterface.DeviceIoControl(IOCTRL_SHALL_I_BEEP_ON_GOOD_READ) )
                        DoGoodIndicator();
                }
            }
            // If send was successful, and host ACK is enabled, start counting.
            if(( Ret ) && ( GET_SETTING( bEnableHostAck )) && ( pCurrentMsg->GetControlMask() & HHPL_PROCESSOR::HHPL_INPUT_AND_SEND_TO_HOST_ACK_REQUIRED ))
            {
                WaitingForHostToRespondToLastDataTransmission = true;
                StartCountingForAckTimeout();
            }
			// DEFAULT_DATA_SOURCE_ID is the  magic DataSourceInformation VersaCode (the script compiler) uses to indicate the script is calling Send() with a user string, not barcode data.
			if ( Ret && (wcscmp(SendParameters->DataSourceInformation, DEFAULT_DATA_SOURCE_ID) != 0) )
			{	// It's a barcode and we sent it OK.  Don't copy additional inter message delay--we'll keep that at 0
				tichar_t * pDummyText;
				size_t RemDataLen = g_pRemRouter->GetBarcodeLobHeaderSize() + SendParameters->LengthOfString;
				CBarcodeData * pRemMsg = CBarcodeData::GetSaveNewBuffer(pDummyText, RemDataLen); // See Footnote 1, above

				pRemMsg->CopyIDsFromDataSourceInfo(SendParameters->DataSourceInformation);
				pRemMsg->SetControlMask(SendParameters->ControlFlags);
				pRemMsg->StoreDataType(DataType);
				g_pRemRouter->RouteBarcodeLob(pRemMsg, SendParameters->String, SendParameters->LengthOfString);
			}
        }
        else
        {
            Ret = false;
            pCurrentMsg->Free();
        }
    }
    else
    {
        Ret = SendAsWrappedBarcode(pCurrentMsg);
        pCurrentMsg->Free();
    }
    return Ret;
}


bool SendAsWrappedBarcode(CBarcodeData * pCurrentMsg) // this should get moved somewher else !!!!????
{
    if( g_pScriptInterpreter)
        return theScriptInterpreter.SendAsWrappedBarcode(pCurrentMsg);
    else
        return false;
}


//RF Protected /private functions
//Helper to deal with scanning/out of range in RF scanner
bool MAIN_HHPL_PROC::AckRcvdFromRFBase(void)
{
    if ( !IsAckRequired())
        return true;
    else
        return !IsAckTimeoutPending();
}

//add command 'D' and 'E'
void MAIN_HHPL_PROC::SendRtsPulse(void)
{
	if(GET_SETTING(RS232CtsEnable) == CTSPLS)
	{
	#if 1
		toggleCtsline();
		MT_Delay(20);//Sleep(20);
		toggleCtsline();
	#else
		theStdInterface.printf("NoRead");
	#endif
	}
}
//


#ifdef RFSCAN
#else		//else not defined RFSCAN

bool  SendAsWrappedBarcode(unsigned char *, int , bool  )
{

    return false;
}
bool MAIN_HHPL_PROC::SendAsWrappedBarcode(CBarcodeData *  )
{
    return false;
}

bool MAIN_HHPL_PROC::SendHTagBarcode(HSTRING * )
{
    return false;
}
#endif

//
// This function is used to convert a BarcodeData object into an HSTRING that will be
// stored in the data accumulation (batch mode) buffer. This function will only fill out
// the bare minimum number of fields to save room in the data accumulation buffer
//
HSTRING * MAIN_HHPL_PROC::BarcodeToTaggedImage(CBarcodeData * pCurrentMsg)
{
    HTAG_DECODE_RESULT  Message(HTAG_DECODE_RESULT_DEFAULT_COMMAND_NAME);
    HWSTRING	hwstrDataToSend( 20, true );
    HSTRING	hstrDecodeResult(64, true );
    HSTRING * TaggedImage;
    char		HHPCodeID[5];
    char		AIMID[5];
    int	IntermessageDelay;
    if (pCurrentMsg==NULL)
        return NULL;
    if ((TaggedImage =new HSTRING(64, true))!=NULL)
    {
        pCurrentMsg->FillDataSourceInfo(HHPCodeID);
        IntermessageDelay=pCurrentMsg->GetAdditionalInterMessageDelay();
        strcpy( AIMID, &HHPCodeID[ 1 ] );
        HHPCodeID[ 1 ] = 0;
        pCurrentMsg->GetWText( &hwstrDataToSend );
        // Create the decode result.
        hwstrDataToSend.CopyToHString(Message.Data);
        Message.UpdateParameter( HTAG_DECODE_RESULT_CODE_ID,(const char *) HHPCodeID );
        Message.UpdateParameter( HTAG_DECODE_RESULT_AIM_ID, (const char *)AIMID );
        Message.UpdateParameter( HTAG_DECODE_RESULT_WIDE_CHARACTER_FORMAT,1);
        Message.UpdateParameter( HTAG_DECODE_RESULT_INTERMESSAGE_DELAY, IntermessageDelay );
        Message.CreateTaggedImage(TaggedImage, false);
        TaggedImage->Reallocate(TaggedImage->Size);
    }
    return TaggedImage;
}


//--------------------------------------------------------------------------
//	DoShotgun
//
//	Again, my due respects to 2dhost.c and MATTY.
//
//	Checks to see if shotgun mode and a barcode message is unique.
//
// If the message is not a duplicate, this routine goes ahead and
// outputs the data to the standard interface and does a good read beep.
//
// Returns TRUE if not a duplicate or shotgun off
// Returns FALSE if duplicate.
//--------------------------------------------------------------------------
bool MAIN_HHPL_PROC::DoShotgun( CBarcodeData& Msg )
{
    bool	bDone = FALSE;
    const tichar_t *pMsg;
    size_t size;

    // Bail out now if we are not menued for shotgun mode
    if( !CTrigger::GetShotgunMode() )
        return TRUE;

    Msg.GetTotalData(pMsg,size);
    /* First Compute this Message's character Sum & Sum-of-Sums */
    INT32 Sum = 0;
    INT32 SSum = 0;
    for (int i=0; i<size; i++)
    {
        Sum += pMsg[i];
        SSum += Sum;
    }

    /* Then Sort Through the list of Previous Message ID's for
    /	 (a) a Match, in which case we Exit doing Nothing, or
    /	 (b) an Empty Location, in which case we Store this one & also Output!	*/
    for (int i=0; (i<MAX_SHOTGUN_HISTORY) && (!bDone); i++)
    {
        if( g_chShotgunCodeID[i] != DEFAULT_CODEID )
        {
            if( (g_chShotgunCodeID[i] == Msg.GetHHPId()) 	&&
                    (g_nShotgunLength[i] == Msg.GetLabelSize())	&&
                    (g_nShotgunSum[i] == Sum) 						&&
                    (g_nShotgunSSum[i] == SSum)						)
            {
                return FALSE;
            }
        }
        else
        {
            g_chShotgunCodeID[i] = Msg.GetHHPId();
            g_nShotgunLength[i] = Msg.GetLabelSize();
            g_nShotgunSum[i]	 = Sum;
            g_nShotgunSSum[i]	 = SSum;
            bDone = TRUE;
        }
    }

    /* If Record of Previous Reads is Full, Discard the Oldest One! */
    if (!bDone)
    {
        int i;

        for (i=0; i<MAX_SHOTGUN_HISTORY-1; i++)
        {
            g_chShotgunCodeID[i] = g_chShotgunCodeID[i+1];
            g_nShotgunLength[i] = g_nShotgunLength[i+1];
            g_nShotgunSum[i]	 =	g_nShotgunSum[i+1];
            g_nShotgunSSum[i]	 =	g_nShotgunSSum[i+1];
        }
        /*now fill the new message into the last spot */
        g_chShotgunCodeID[i] = Msg.GetHHPId();
        g_nShotgunLength[i] = Msg.GetLabelSize();
        g_nShotgunSum[i]	 = Sum;
        g_nShotgunSSum[i]	 = SSum;

    }

	if( CTrigger::GetShotgunMode() )
    {
		pTrigger->PreferredSequenceShotgunModeFinishScanning(); //We are finished with shotgun, so stop scanning, since we block stop scanning in other finish scanning() calls, to stop flash LED flicker during repeat decodes
	}

    return TRUE;		// not a dup msg return true
}

//--------------------------------------------------------------------------
//  DoSequences
//
// Does the output stuff for the two types of sequence modes
// Uses the outputmodes class to process data
//
// Returns:
// 3 if both bTossDecodeResults and bFoundData are set
// 2 if output buffer needs to be flushed to stop re-reads, set bTossDecodeResults to true
// 1 if the sequence call return result is 0 (normal output, iSeqResult == 0), set bFoundData to true
// 0 if output buffer needs nothing to be done and we are not indicating found data
// -1 if a duplicate was found
// -2 if no match was found
// -3 if non centered
//
int  MAIN_HHPL_PROC::DoSequences( CBarcodeData& Msg )
{
    int iPreferredBarcodeIndex = -1;
    int* piPreferredBarcodeIndex = &iPreferredBarcodeIndex;
    int iSequenceBarcodeCount = -1;
    int* piSequenceBarcodeCount = &iSequenceBarcodeCount;

    int iSeqResult;
    int iPriorityState = 0;
    int iSequenceState = 0;

    bool bFoundData = false;
    bool bTossDecodeResults = false;

    //	RESET_WATCHDOG_TIMER();

    //Call Priority
    if(theOutputModes.IsPreferredModeOn())
    {
        iPriorityState = theOutputModes.DoPreferred( Msg, piPreferredBarcodeIndex );
    }

	//theStdInterface.printf(" Prior %d \r\n", iPriorityState); //lmc debug

	if(iPriorityState == 2)     //Got the priority output
    {
        int mask;

        //set flag to empty decode resuts Q so mutiple reads dont re trigger
        bTossDecodeResults = true;

        if( (theOutputModes.IsSequenceModeEnabled()) )          //is sequence mode on
        {
            //set preferred barcode data pointer to Null since we got the data
            theOutputModes.SetPreferredBarcodeDataNull( iPreferredBarcodeIndex );
            // Set flag to Clear the sequence buffers
            theOutputModes.SetPreferredStartedOff();
        }
        else          //if sequence not enabled, output preferred
        {
            if( (IOCRET_YES == theStdInterface.DeviceIoControl(IOCTRL_SHALL_I_BEEP_ON_GOOD_READ)) 	&& !bCtsManualMode) // RF dont beep here becuse we only beep if base sends ack (non RF or dongle etc beep)
            {
                DoGoodIndicator();
            }

            mask = Msg.GetControlMask(); // get the control mask from barcode object

            if (IsAckRequired())
            {
                mask |= HHPL_INPUT_AND_SEND_TO_HOST_ACK_REQUIRED;	// if we are Rf and require and ackowlege set that bit
            }

            Msg.SetControlMask(mask);	//update the mask
            m_FinalResultsQueue->Write( &Msg );

            //set preferred barcode data pointer to Null since we got the data
            theOutputModes.SetPreferredBarcodeDataNull( iPreferredBarcodeIndex );

            // Set flag to Clear the sequence buffers
            theOutputModes.SetPreferredStartedOff();
        } 				//end output preferred
    } 	    //end Got the priority output
    else if( (iPriorityState == 0) && (theOutputModes.IsPreferredStarted()) )           //if we broke out of a priority search
    {
        // Set flag to Clear the sequence buffers
        theOutputModes.SetPreferredStartedOff();

        //set flag to empty decode resuts Q so mutiple reads dont re trigger
        bTossDecodeResults = true;
    }
    else if(iPriorityState < 0)
    {
        return iPriorityState;
    }


    //do sequence mode if enabled and the data was passed on by preferred (or preferred disabled)
    if( ((iPriorityState == 0) || (iPriorityState == 2)) && (theOutputModes.IsSequenceModeEnabled()) )
    {
        iSequenceState = theOutputModes.DoSequence( Msg, piSequenceBarcodeCount );		// apply sequence to anything not handled by preferred
    }

    if(iSequenceState == 2)  //do sequence mode output
    {                                                // RF dont beep here becuse we only beep if base sends ack (non RF or dongle etc beep)
        if (IOCRET_YES == theStdInterface.DeviceIoControl(IOCTRL_SHALL_I_BEEP_ON_GOOD_READ) && !bCtsManualMode)
        {
            DoGoodIndicator();
        }

        // Output the Sequence
        for(int i=0; (i<MAX_MULTI_BARCODES) && (i<iSequenceBarcodeCount); i++)
        {
            BarcodeDataSequencePointerArray[i] = &theOutputModes.GetSequenceBarcodeData( i );

            int mask = BarcodeDataSequencePointerArray[i]->GetControlMask(); // get the control mask from barcode object

            if( IsAckRequired() )
            {
                mask |= HHPL_INPUT_AND_SEND_TO_HOST_ACK_REQUIRED;	// if we are Rf and require and ackowlege set that bit
            }

            if( (i<(iSequenceBarcodeCount-1)) && (i<(MAX_MULTI_BARCODES-1)) ) // not last
            {
                mask |= HHPL_SEND_TO_HOST_NO_INDICATORS; //no indicator required
            }

            BarcodeDataSequencePointerArray[i]->SetControlMask(mask);	//update the mask
            m_FinalResultsQueue->Write( BarcodeDataSequencePointerArray[i] );
            theOutputModes.SetSequenceBarcodeDataNull( i );
        }

        // Set flag to Clear the sequence buffers
        theOutputModes.SetSequenceStartedOff();
    }	//end output sequence
    else if(iSequenceState < 0)
    {
        return iSequenceState;
    }

    if( (theOutputModes.IsSequenceModeEnabled()) )     //set sequence state depending on which sequence modes are enabled
    {
        iSeqResult = iSequenceState;
    }
    else
    {
        iSeqResult = iPriorityState;
    }

    switch (iSeqResult)         //if sequence return state is 0, 1 or 2  (negatives are returned above)
    {
    case 0:// if we are not in sequence mode, put output in Q and beep
        if (IsAckRequired())
        {
            Msg.SetControlMask(Msg.GetControlMask() |HHPL_INPUT_AND_SEND_TO_HOST_ACK_REQUIRED);
        }
        if (!(Msg.GetControlMask() & HHPL_SEND_TO_HOST_NO_INDICATORS))
        {
            if (IOCRET_YES == theStdInterface.DeviceIoControl(IOCTRL_SHALL_I_BEEP_ON_GOOD_READ) )
            {
                if( !bCtsManualMode )
                {
                    DoGoodIndicator();
                }
            }
        }
        if (m_FormatterResultsQueue->AnyData())
        {
            Msg.SetControlMask(Msg.GetControlMask() |HHPL_SEND_TO_HOST_NO_INDICATORS);
        }
        m_FinalResultsQueue->Write( &Msg );
        DoGoodReadFlash();
        bFoundData = TRUE;
        if (!CTrigger::GetShotgunMode())
        {
            pTrigger->UnTriggerSerial();		// if we triggered serially untrigger unit
            pTrigger->WaitForScanStand();
        }
        else
        {
            pTrigger->SetSerialTriggerTimeout();	// in shotgun mode restart serial trigger timer
        }
        break;
    case 1:
        DoGoodReadFlash();
        //if( (theOutputModes.IsSequenceModeEnabled()) )    //this clear method is only needed in 4600R, which uses new reread delay. Old reread delay is currently used in trunk, and 4800dr.	LMC
        //{
        //	ClearAllButLastReadTimestamp();      //new rereaddelay clear, clear all time stamps but last re-read time stamp
        //}
        break;
    case 2:
        bTossDecodeResults = true;       //set flag to empty decode resuts Q so mutiple reads dont re trigger
        DoGoodReadFlash();
        pTrigger->UnTriggerSerial();		// if we triggered serially untrigger unit
        pTrigger->WaitForScanStand();
        break;
    default:
        break;
    }  //end switch  sequences results

	if( theOutputModes.IsPreferredModeOn() || theOutputModes.IsSequenceModeEnabled() )
    {
		pTrigger->PreferredSequenceShotgunModeFinishScanning(); //We are finished with preferred or sequence, so stop scanning, since we block stop scanning in other finish scanning() calls, to stop flash LED flicker during repeat decodes
	}

    iSeqResult = 0;               //Since iSeqResult is greater or equal to 0, then return just the flag states
    if( bTossDecodeResults )
    {
        iSeqResult += 2;
    }
    if( bFoundData )
    {
        iSeqResult += 1;
    }
    return iSeqResult;
}

bool MAIN_HHPL_PROC::IsSequenceStarted(void)
{
    bool bIsAnySequenceStarted = false;
    if(theOutputModes.IsSequenceStarted() || theOutputModes.IsPreferredStarted())
    {
        bIsAnySequenceStarted = true;
    }
    return bIsAnySequenceStarted;
}

bool MAIN_HHPL_PROC:: SequencePriorityTimeoutExpired(void)
{
    bool bSequenceResponse;
    bSequenceResponse = theOutputModes.SequencePriorityTimeoutExpired();
    return bSequenceResponse;
}

void MAIN_HHPL_PROC:: SequencePriorityTimeoutRefresh(void)
{
    theOutputModes.SequencePriorityTimeoutRefresh();
}

bool  MAIN_HHPL_PROC::DoPartialSequence( void )
{
	bool bPartialSeqResult = false;


	if( (GET_CONFIG_SETTING(iOpticsPopulation)==TWO_D) && (pTrigger->GetTriggerMode()!=TRGMOD_AUTO) && (pTrigger->GetTriggerMode()!=TRGMOD_PRESENT) && (pTrigger->GetTriggerMode()!=TRGMOD_STREAM_PRESENT) )
	{
		iSequenceBarcodeCountPartial = (theOutputModes.GetSequenceBarcodeDataPartialSize());

		//do partial sequence mode if enabled
    	if( (theOutputModes.IsSequenceModeEnabled()) && (theOutputModes.IsSequenceBarcodeDataPartialEnabled()) && (theOutputModes.IsSequenceStarted()) )       //removed:  && (theOutputModes.IsSequenceModeRequired())
    	{
    		if( iSequenceBarcodeCountPartial > 0 )  //do sequence mode output if data
    		{
				// RF dont beep here becuse we only beep if base sends ack (non RF or dongle etc beep)
        		if (IOCRET_YES == theStdInterface.DeviceIoControl(IOCTRL_SHALL_I_BEEP_ON_GOOD_READ) && !bCtsManualMode)
        		{
            		//DoGoodIndicator();
					SoundErrorBeep();             //moved error beep from below so it could be menued to error beep on partial sequence
        		}

        		// Output the Partial Sequence
        		for(int i=0; (i<MAX_MULTI_BARCODES) && (i<iSequenceBarcodeCountPartial); i++)
        		{
            		BarcodeDataSequencePointerArray[i] = &theOutputModes.GetSequenceBarcodeDataPartial( i );

            		int mask = BarcodeDataSequencePointerArray[i]->GetControlMask(); // get the control mask from barcode object

            		if( IsAckRequired() )
            		{
                		mask |= HHPL_INPUT_AND_SEND_TO_HOST_ACK_REQUIRED;	// if we are Rf and require and ackowlege set that bit
            		}

            		if( (i<(iSequenceBarcodeCountPartial-1)) && (i<(MAX_MULTI_BARCODES-1)) ) // not last
            		{
                		mask |= HHPL_SEND_TO_HOST_NO_INDICATORS; //no indicator required
            		}

            		BarcodeDataSequencePointerArray[i]->SetControlMask(mask);	//update the mask
            		m_FinalResultsQueue->Write( BarcodeDataSequencePointerArray[i] );
            		theOutputModes.SetSequenceBarcodeDataPartialNull( i );
        		}

        		// Set flag to Clear the sequence buffers
        		theOutputModes.SetSequenceStartedOff();

				//Clear the partial data count flag
				theOutputModes.ClearSequenceBarcodeDataPartialSize();

				bPartialSeqResult = true;
				//SoundErrorBeep();
    		}	//end output sequence

			if( theOutputModes.IsPreferredModeOn() || theOutputModes.IsSequenceModeEnabled() )
    		{
				pTrigger->PreferredSequenceShotgunModeFinishScanning(); //We are finished with sequence, so stop scanning, since we block stop scanning in other finish scanning() calls, to stop flash LED flicker during repeat decodes
			}

		}	//end if sequence enabled
	}	//end if not an auto trigger

	return bPartialSeqResult;
}



// TriggerHit
// Purpose returns TRUE if the trigger button is depressed
bool MAIN_HHPL_PROC:: TriggerHit(void)
{
    return (pTrigger->IsUnitTriggered());
}

// TriggerReleased
// Purpose returns TRUE if the trigger button has been released
bool MAIN_HHPL_PROC:: TriggerReleased(void)
{
    bool bTriggerState;
    //FIXME	bTriggerState = ( (!pTrigger->IsUnitTriggered()) || (pTrigger->bAllignmentModeEnabled) );    //not triggered or in 4800dr allignment mode
    bTriggerState = !pTrigger->IsUnitTriggered();

    return bTriggerState;
}

void MAIN_HHPL_PROC:: LogError( const char * themessage)
{
    theStdInterface.printf(_T("%s \r"),themessage);
    wait_til_tx_is_empty();
}

// Called when decoding is initiated by DECODE menu command since we are not currently in trigger state machine
void  MAIN_HHPL_PROC::ProcessTriggerlessBarcode(int IsThereABarcode)
{
    if(IsThereABarcode)
        m_TriggerlessBarcode = 1;
    else
        m_TriggerlessBarcode = 2;
}

void MAIN_HHPL_PROC::InjectSimulatedBarcode(CBarcodeData *pBarcode)
{
    DecodeResults->Write(pBarcode);
    m_Injected=true;
    pTrigger->TriggerSerial();
}

bool	MAIN_HHPL_PROC::MenuCommand(char *MenuCommandString, int *LengthOfMenuCommandString, int MaximumLengthOfString)
{
    UINT		MenuResult;

    HSTRING	MenuCommandAndResponse(*LengthOfMenuCommandString, true);
    MenuCommandAndResponse.Copy(MenuCommandString, *LengthOfMenuCommandString);

    /*Return messages are passed in pucBuffer so it needs to be big enough for the larges possible menu response */
    /*Menu will update the length parameter with the length of the reponse. */

    MenuResult = ReaderConfiguration->Menu(&MenuCommandAndResponse);
    KCODE_MODE(FALSE);
    FlashAndReSynchCustomerDefaults();
    FlashWorkingChanges();
    ReSynchWorkingAndActive();
    theScriptInterpreter.ResetScanningState();

    *LengthOfMenuCommandString = MenuCommandAndResponse.Size;
    MenuCommandAndResponse.CopyFromString( 0, *LengthOfMenuCommandString, (unsigned char *) MenuCommandString, MaximumLengthOfString );

    return ( MenuResult == MENU_ERROR) ? false : true;
}


/*
  Footnotes:

  1.  After fixing a bug (JIRA NG2D-759), I noticed the failure with GetNextScan().  I traced it to this:  we don't actually start
  scanning right when we go into temp Manual Trigger Mode.  This probably has something to do with the implementation of ResetScanningState()
  and the fact it's not really aware of previous and new Trigger Mode.  However, the next time we cycle through the loop, we will start it.
  There is a decision involved when we start scanning, so rather than make this code harder to follow, I decided to leave it as is and
  document the behavior.

*/

