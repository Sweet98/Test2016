//======================================================================================
// HHPLProcessor.h
//======================================================================================
// $RCSfile: Gen6/mainhhplproc.h $
// $Revision: 1.19 $
// $Date: 2011/11/11 17:01:39EST $
//======================================================================================

//======================================================================================



#ifndef __MAINHHPL_PROC_H
#define __MAINHHPL_PROC_H




#include "HHPLprocessor.h"		// HHPL program run-time profile information
#include "Timeout.h"
#include "thread.h"



class CIoMan;
class CBarcodeFifo;
class CBarcodeData;

const int SCRIPTSTACKSIZE = 900*1024;

#ifdef RFSCAN
class BatchManager;
#define BATCH_SENT	0
#define BATCH_EMPTY  -1
#define BATCH_CANT_SEND -2
#define BATCH_NOT_READY -3
#endif

threadhandle_t StartScript(void);
class MAIN_HHPL_PROC;
extern MAIN_HHPL_PROC *g_pScriptInterpreter;
#define theScriptInterpreter (*g_pScriptInterpreter)


bool SendAsWrappedBarcode(CBarcodeData * pCurrentMsg, bool DoNotUseIndicators = false ); // this should get moved somewher else !!!!????
bool SendAsWrappedBarcode(unsigned char * pBuf, int length, bool DoNotUseIndicators = false );

class  MAIN_HHPL_PROC : public  HHPL_PROCESSOR
{
public:
	static void CreateScriptInterpreter(void);
	static void DestroyScriptInterpreter(void);

	void SriptRunSome(void);

	// Service the good read timeout.
	void ServiceMainScriptTimers(void);

	// Reset the good read timeout.
	void InitializeMainScriptTimers( void );

	// When in Verifier buffering mode, allow the command processor to inform us when the host acknowledge data transmission.
	void HostAcknowledgedLastDataTransmission( bool DataWasReceivedSuccessfullyByHost = true );

	// When in Verifier buffering mode, we set a flag indicating that the host is not talking to us when we fail to receive an
	// acknowledgement to data transmission.  This routine is called by the command processor to indicate that the host is now
	// talking to us.
	void HostIsTalkingToUs( void );

	// Start the countdown for receipt of host acknowledgement after transferring data.
	void StartCountingForAckTimeout(void);

	// Determine whether or not host acknowledgement of data transmission is required.
	bool IsAckRequired(void);

	// Disable illumination when scanner is disabled by a serial 'D' character.
	bool IsDisableLights(void);

	// Start the countdown for receipt of host acknowledgement after transferring data.
	bool IsAckTimeoutPending(void);

	bool SendAsWrappedBarcode(CBarcodeData * pCurrentMsg );

	virtual void OnChangedConfiguration(void);	// gets called by the framework after a menu change

	void ResetScanningState(void)
	{
		mResetScanningState=true;
	}

	void InjectSimulatedBarcode(CBarcodeData *pBarcode);  // tool for testing translations
	void ProcessTriggerlessBarcode(int IsThereABarcode);  // Typically from DECODE Menu command

	// functions for calling output mode helper class

	// Check to see if our priority sequence has started
	bool IsSequenceStarted(void);

	// Check to see if our priority sequence timeout has expired
	bool SequencePriorityTimeoutExpired(void);

	// refresh the priority timeout
	void SequencePriorityTimeoutRefresh(void);
	
	//Send a 20ms Pulse when triggered but no read	//add command 'D' and 'E'
	void	SendRtsPulse(void);
	//

	// data members for output mode helper class

	int iSequenceBarcodeCountPartial;

#define	USE_LARGER_BUFFERS	1                      //lmc
	// This is just a guess.  Change this if you want to have more than
	// 5 possible barcodes as part of your sequence output.  If this
	// changes, please also change the sequence output buffer initializations
	// below.  Currently we initialize 5 locations.
#ifndef	USE_LARGER_BUFFERS
	#define	MAX_MULTI_BARCODES	5
#else
	#define	MAX_MULTI_BARCODES	15
#endif

	typedef CBarcodeData* BarcodeDataPointerArray[MAX_MULTI_BARCODES];

	CBarcodeData* pBarcodeDataPreferredReturnValue;

	BarcodeDataPointerArray BarcodeDataSequencePointerArray;

	bool bCtsManualMode;
	enum
	{
	    BarcodeFifoSize = 100
	};

	enum SCANSTATES
	{
	    STATE_WAIT_FOR_TRIGGER,
	    STATE_TRIGGERED,		// we call decoder in this state
	    STATE_WAIT_FOR_UNTRIGGER,
	    STATE_PROCESS_MESSAGE,// do data edit or script and put message(s) on Q for output
	    STATE_OUTPUT_NEXT_MESSAGE,	// output a message from the Q
	    STATE_WAIT_FOR_UNTRIGGER_AFTER_NOTIFY_HOST,
	    STATE_MENU_SCANNED,		// initial state on power up
	    STATE_WAIT_FOR_UNTRIGGER_AFTER_FIRST_TRIGGER_IN_MANUFACTURING_IMAGER_TEST_MODE
	};

#ifdef RFSCAN
// Set the inventory send mode variable (true to start sending, false to stop)
	bool SetInventorySendMode( bool enable );

// Delete everything in the batch mode queue - made public so the user can flush the queue through menu command
	void FlushBatchModeQueue(void);

// Delete the last bached scan
	bool DeleteLastBatchedScan(void);

// Return if there is any data in the batch mode queue (used to prevent power down)
	int AnyBatchModeData( void );

// Return the batch manager's response to updating the inventory count
	bool UpdateInventoryCount( int num );

// Return the total number of barcodes stored in the queue, including inventory counts
	int GetNumberOfStoredBarcodes( void );

// Save batch mode queue to a file before powering down
	bool SaveBatchFile( void );
#ifdef BATCH_DEBUG
	// Debugging commands for non-volatile storage:
	bool DebugLoadBatchFile( void );
	bool DebugDeleteBatchFile( void );
#endif	// BATCH_DEBUG
#endif	// RFSCAN

private:
	CTimeoutmS *GoodReadIndicateTimeout;

	volatile bool DefaultScriptIsRunning;
	volatile bool BackgroundOperationsState;

	MT_Event_t ScriptEvent;

	// When in Verifier buffering mode, keep track of whether or not the host is responding to data transmission.
	bool HostIsRespondingToDataTransmission;

	// When in Verifier buffering mode, keep track of when we're waiting for the host to respond to data transmission.
	bool	WaitingForHostToRespondToLastDataTransmission;

	// When in Verifier buffering mode, keep track of when we're waiting for the host to respond to data transmission.
	bool	HostIndicatedThatLastDataTransmissionWasSuccessful;

	// Indicate when host acknowledge data transmission.
	bool	ReceivedHostAcknowledgement;

protected:
	MAIN_HHPL_PROC(void);		// protected ctor to force people to use the create function.
	virtual ~MAIN_HHPL_PROC(void);		// protected dtor to force people to use the destroy function.

	// The following  functions are used by the interpreter to perform system related functions that are
	// platform / implementation specific.  It is the responsibility of the instantiator to override these, as
	// appropriate, with functions that perform the desired operations.  This requires that this class be used
	// as a base class.
	virtual void	Beep(int /*Frequency*/, int /*Duration*/, int /*Volume*/);
	virtual void	Delay(int /*NumberOfMillisecondsToDelay*/);
	virtual void	DoBadIndicator(void);
	virtual void	DoGoodIndicator(void);
	virtual int		GetSystemTimer0(void);
	virtual void	IndicatorLightOn(int /*Color*/, int /*OnCycleTime*/, int /*NumberOfCycles*/)
	{
		return;
	}
	virtual int	   InputCharacter(int /*ControlMask*/, int /*Timeout*/)
	{
		return(0);
	}
	virtual int	   InputNumber(int /*ControlMask*/, int * /*InputData*/, int /*LowEndOfRange*/, int /*HighEndOfRange*/,
	                           int /*DefaultValue*/, char * /*DataSource*/, int /*Timeout*/, char * /*QuitKeys*/)
	{
		return(0);
	}
	virtual	int		InputNumber(int ControlMask, int *InputData, int LowEndOfRange, int HighEndOfRange,
	                         int DefaultValue, wchar_t *DataSource, int Timeout, char *QuitKeys )
	{
		return 	HHPL_PROCESSOR::InputNumber(ControlMask,InputData,LowEndOfRange,HighEndOfRange,DefaultValue,DataSource,Timeout,QuitKeys );

	}
	virtual	int		InputString(int /*ControlMask*/, HSTRING * /*InputData*/, int /*MinimumLength*/,
	                         int /*MaximumLength*/, char * /*DataSourceInformation*/, int /*Timeout*/, char * /*QuitFlags*/)
	{
		return(0);
	}

	// Note that for InputString, you're on your honor to not exceed MAXIMUM_STRING_SIZE
	virtual	int		InputString(int ControlMask, HWSTRING *InputData, int MinimumLength,
	                         int MaximumLength, wchar_t *DataSourceInformation, int Timeout, char *QuitFlags )
	{
		return InputString(ControlMask,InputData,MinimumLength,
		                   MaximumLength,DataSourceInformation,Timeout,QuitFlags,0 );

	}

	virtual int		InputString(int ControlMask, HWSTRING *InputData, int MinimumLength,
	                         int MaximumLength, wchar_t *DataSourceInformation, int Timeout, char *QuitFlags, int *ControlFlags );
	virtual int		InputString( INPUT_STRING_PARAMETERS *Parameters);

	virtual int 	InputStandard(int /*ControlMask*/, HWSTRING *InputData,
	                           int MinimumLength, int MaximumLength,
	                           wchar_t *DataSourceInformation, int Timeout,
	                           char * /*QuitFlags*/, int * ControlFlags, int * DelayInMicroseconds);


#if defined( DEVELOPMENT_SETTINGS ) || defined( MANUFACTURING_IMAGER_TEST )

	int  Inputtsc(int ControlMask, HWSTRING *InputData,  int MinimumLength,
#ifndef RFSCAN
	              int MaximumLength, wchar_t *DataSourceInformation, int Timeout, char *QuitFlags);
#else /* RFSCAN */

	int MaximumLength, wchar_t *DataSourceInformation, int Timeout, char *QuitFlags, int *ControlFlags);
#endif /* RFSCAN */
#endif
	virtual	bool	IsScriptingAvailable( void );

	virtual int		LastSystemOperationStatus(void)
	{
		return(0);
	}
	virtual void	LogError( const char * /*Error*/);
	virtual	void	LogError( const wchar_t *Error)
	{
		HHPL_PROCESSOR::LogError(Error);
	}
	virtual bool	MenuCommand(char *MenuCommandString, int *LengthOfMenuCommandString, int MaximumLengthOfString);
	virtual	bool	MenuCommand( wchar_t *MenuCommandString, int *LengthOfMenuCommandString, int MaximumLengthOfString )
	{
		return HHPL_PROCESSOR::MenuCommand(MenuCommandString,LengthOfMenuCommandString,MaximumLengthOfString );
	}
	virtual void	ProcessBackgroundOperationsDuringScriptExecution(void);
	virtual void	ProcessErrorFlag(void)
	{
		return;
	}
	virtual void	ProcessTerminateFlag(void)
	{
		TerminateHHPLProgram();
		return;
	}
	virtual void	ProcessWarningFlag(void)
	{
		return;
	}
	virtual void	ScannerID(char * /*Result*/);
	virtual	void	ScannerID( wchar_t *Result)
	{
		HHPL_PROCESSOR::ScannerID(Result);
	}


	virtual bool	SendToHost( SEND_TO_HOST_PARAMETERS * /* SendParameters */ );
	virtual void	TempEnable(int /*Device*/, int /*Time*/)
	{
		return;
	}
	virtual	int	TerminalID(void);
	virtual bool SendToHostInterface( SEND_TO_HOST_PARAMETERS *SendParameters,CIoMan * SendInterface,long int TimeOut);

	//Code XML Functionality
	virtual bool	SendToCodeXMLBase( SEND_TO_HOST_PARAMETERS *SendParameters,CIoMan * SendInterface );
	virtual bool	Send_A_Packet(HSTRING* data, char packetNumber, CIoMan * SendInterface);
	virtual bool	Send_Z_Packet(HSTRING* data, char packetNumber, CIoMan * SendInterface);
	virtual bool	TransmitCodeXMLPacket( HSTRING *data, CIoMan * SendInterface );

	virtual void	TerminateHHPLProgram(void)
	{
		TerminateHHPLProgramAtNextInstruction = true;
		return;
	}
	virtual bool	TestRoutine(int /*TestRoutineNumber*/, int /*NumberOfParameters*/, void ** /*ParameterPointers*/)
	{
		return(false);
	}
	virtual void	ToggleAuxiliaryPort(void)
	{
		return;
	}
	virtual bool TriggerHit(void);
	bool TriggerReleased(void);
	virtual void ZeroSystemTimer0(void);
	bool AckRcvdFromRFBase(void);
	virtual int TxSuccess(void)
	{
		return(0);
	}
	virtual int Workgroup(void);

	//  Shotgun and Sequence
	bool DoShotgun( CBarcodeData& Msg );
	void DoGoodReadFlash(void);
	int  DoSequences( CBarcodeData& Msg );
	bool DoPartialSequence( void );
	void HandleDecode(int starttime);

	//batch mode
	HSTRING *BarcodeToTaggedImage(CBarcodeData * pCurrentMsg);
	bool 	SendHTagBarcode(HSTRING * pBarcode);
	bool	ExpandBatchModeHtagBarcode(HSTRING * pBarcode);
#ifdef RFSCAN
	void	CreateBatchManager(void);
	bool	AddToBatchModeQueue(CBarcodeData * pCurrentMsg, bool DeleteMessageAfterAddingToQueue = true );
	int		mBatchSequenceNumber;
	int		mBatchSequenceLastSent;
	int		mBatchSequenceAck;
	enum BATCHSTATES
	{
		BATCH_WAIT_TO_SEND,
		BATCH_READY_TO_SEND,
		WAITING_FOR_ACK,
		TIMEOUT_WAITING_FOR_ACK,
		RECEIVED_ACK
	};
	BATCHSTATES BatchState;
	BatchManager* mBatchManager;

public:
	int 	ProcessQueueItem(void);
	void	ReceivedSequenceAck(int recentAck);
	bool	GetRecentSequenceAck(void);
	void	SetBatchModeTimeout(int newTimeout);

#endif /* RFSCAN */

protected:
	// data members
	bool mResetScanningState;
	bool m_Injected;  			// tool for testing translations
	int  m_TriggerlessBarcode; 	// Typically from DECODE Menu command
	unsigned ScriptTimerStart;
	unsigned int m_getScanFailure;      // Failure count from GetNextScan calls
	bool bOposEnable;
	bool bOposMenuDisable;
	enum OutputMode_t
	{
	    Standard,
	    HHLC
	};



	SCANSTATES scanningstate;
	bool				mAllowBuffering;
	bool				mFirstTriggerReceived;
	OutputMode_t m_OutputMode;
	CBarcodeFifo *m_FinalResultsQueue;
	CBarcodeFifo *m_FormatterResultsQueue;
	CBarcodeData *m_pCurrentMessage;
	enum
	{
	    DebugBufferSize = 40
	};
	tichar_t m_MsgTriggerToDecode[DebugBufferSize+1];
	int m_tscenable;
	int m_tscattempts;
	char CodeXMLPktNumber;
	bool bReceivedDC2; //Serial DC2 status.

public:
	bool ScriptIsInBackgroundOperationsState( void );
	void SetScriptType( bool DefaultScript );

	/*---------------------------------------------------
	 * EnableOPOS set OPOS state on
	 *---------------------------------------------------*/
	inline void EnableOPOS(void)
	{
		bOposEnable = TRUE;
	}
	/*---------------------------------------------------
	 * DisableOPOS set OPOS state off
	 *---------------------------------------------------*/
	inline void DisableOPOS(void)
	{
		bOposEnable = FALSE;
	}
	/*---------------------------------------------------
	 * GetOPOSMode return the current OPOS disable/enable mode
	 *---------------------------------------------------*/
	inline bool GetOPOSMode(void)
	{
		return bOposEnable;
	}
	/*---------------------------------------------------
	* GetOPOSMode return the current OPOS disable/enable mode
	*---------------------------------------------------*/
	inline bool GetOPOSMenuMode(void)
	{
		return bOposMenuDisable;
	}

	inline void OPOSMenuModeOn(void)
	{
		bOposMenuDisable=false;
	}
	inline void OPOSMenuModeOff(void)
	{
		bOposMenuDisable=true;
	}


	void SetOPOSDisableMode(void);  // get OPOS setting

	/*----------------------------------------------------
	 *  GetDC2Status() return if triggered by DC2 serial char.
	 *---------------------------------------------------*/
	inline bool GetDC2Status(void)
	{
		return bReceivedDC2;
	}
	/*---------------------------------------------------
	 * SetDC2Status() set OPOS state on
	 *---------------------------------------------------*/
	inline void SetDC2Status(void)
	{
		bReceivedDC2 = true;
	}
	/*---------------------------------------------------
	 * SetDC2Status() set OPOS state on
	 *---------------------------------------------------*/
	inline void ClearDC2Status(void)
	{
		bReceivedDC2 = false;
	}

	// Allow the instantiator to tell us whether or not the current script is the default.  The reason we want to know is that we'll always
	// allow the default script to run, but we won't allow other scripts to run if scripting is not enabled in product configuration.
private:
	MAIN_HHPL_PROC (const MAIN_HHPL_PROC&);            // declared private, but not defined, to prevent copying         
	MAIN_HHPL_PROC& operator=(const MAIN_HHPL_PROC&);  // declared private, but not defined, to prevent copying 

	
};


#endif      //	__HHPL_PROC_H



