//======================================================================================
// HHPLProcessor.h
//======================================================================================
// $RCSfile: hhplprocessor.h $
// $Revision: 1.2 $
// $Date: 2011/11/11 17:01:40EST $
//======================================================================================

//======================================================================================



#ifndef __HHPL_PROC_H
#define __HHPL_PROC_H



#include	"HWSTRING.h"				// HWSTRING interface definition

#include	"HHPLProgramProfile.h"		// HHPL program run-time profile information
#include	"HHPLIPB.h"					// HHPL instruction parameter block information
#include	"HHPLInstructionSet.h"		// HHPL instruction set definitions

#include	<string.h>



class HHPL_PROCESSOR : public HHPL_INSTRUCTION_SET

{
public:
// In non-C++ environments, we can't expand the strings dynamically, so we need to set
// maximums for such operations as InputString.  In the C++ environment, these parameters reflect
// the minimum size of strings, prefixes, and suffixes whereas in the non-C++ environment, they
// reflect the maximum sizes.
	#define	MAXIMUM_SIZE_OF_PREFIX_SUFFIX						100
	#define	MAXIMUM_STRING_SIZE									3000


	#define	HHPL_INTERPRETER_VERSION			HHPL_VERSION


public:

// ExecuteRunTimeScriptCommand return codes.
	#define		EXEC_RTCMD_COMMAND_SUCCESSFULLY_EXECUTED			0
	#define		EXEC_RTCMD_NO_COMMAND_FOUND							1
	#define		EXEC_RTCMD_END_COMMAND_ENCOUNTERED					2
	#define		EXEC_RTCMD_UNRECOGNIZED_COMMAND_ENCOUNTERED			3


//protected:

// Flag used to indicate when HHPL program is to be terminated
	bool	TerminateHHPLProgramAtNextInstruction;


// Flag used to indicate when HHPL program was terminated because scripting was not available and the script was
// not built with the FeatureProtectionOverride flag on.
	bool	HHPLProgramTerminatedBecauseScriptingIsNotAvailable;




// Define data structures used by the display menu command handler.
	typedef struct {
		int		X;
		int 	Y;
	} ORIGIN;

	typedef struct {
		ORIGIN			Origin;
		wchar_t			*Text;
		int				TextSize;
		int				ControlMask;
		wchar_t			*Hotkey;
		int				HotkeySize;
	} MENU_ITEM;

	typedef struct {
		ORIGIN		Origin;
		MENU_ITEM	Title;
		int			NumberOfMenuItems;
		MENU_ITEM	*MenuItems;
		int			MaximumNumberOfDisplayLinesToUse;
		int			TimeoutOnNoSelection;
		int			ControlMask;
	} MENU_PARAMETERS;


protected:  // previously the structures above were protected as well, but Codewarrior took offense with ORIGIN and MENU_ITEM being protected?

// The following virtual functions are used by the interpreter to perform system related functions that are
// platform / implementation specific.  It is the responsibility of the instantiator to override these, as
// appropriate, with functions that perform the desired operations.  This requires that this class be used
// as a base class.
	virtual	void	BacklightOn(void)   { return; }
	virtual	void	BacklightOff(void)   { return; }
	virtual	int		BatteryStrength(void)   { return(0); }
	virtual	void	Beep(int /*Frequency*/, int /*Duration*/, int /*Volume*/)   { return; }
	virtual	int		CharacterHeight(int /*Font*/)   { return(0); }
	virtual	int		CharacterWidth(int /*Font*/, int /*Character*/)   { return(0); }
	virtual	void	CorruptDataFormatCharacterCount(char * /*ErrorDescription*/)   { return; }
	virtual	void	CorruptDataFormatCharacterCount( wchar_t *ErrorDescription )
				{
					HWSTRING	hwstrErrorDescription( ErrorDescription );
					HSTRING		hstrErrorDescription( 20, true );
					hwstrErrorDescription.ConvertAndCopyToHString( &hstrErrorDescription );
					CorruptDataFormatCharacterCount( (char *) hstrErrorDescription.Char );
					return;
				}
	virtual	void	DataFormatCompareFailed(char * /*ErrorDescription*/)   { return; }
	virtual	void	DataFormatCompareFailed( wchar_t *ErrorDescription )
				{
					HWSTRING	hwstrErrorDescription( ErrorDescription );
					HSTRING		hstrErrorDescription( 20, true );
					hwstrErrorDescription.ConvertAndCopyToHString( &hstrErrorDescription );
					DataFormatCompareFailed( (char *) hstrErrorDescription.Char );
					return;
				}
	virtual	void	DataFormatCursorOutOfRange(char * /* ErrorDescription*/)   { return; }
	virtual	void	DataFormatCursorOutOfRange( wchar_t *ErrorDescription )
				{
					HWSTRING	hwstrErrorDescription( ErrorDescription );
					HSTRING		hstrErrorDescription( 20, true );
					hwstrErrorDescription.ConvertAndCopyToHString( &hstrErrorDescription );
					DataFormatCursorOutOfRange( (char *) hstrErrorDescription.Char );
					return;
				}
	virtual	void	DataFormatDelay(int /*NumberOfMillisecondsToDelay*/)   { return; }
	virtual	void	DataFormatDelayError(char * /*ErrorDescription*/)   { return; }
	virtual	void	DataFormatDelayError( wchar_t *ErrorDescription )
				{
					HWSTRING	hwstrErrorDescription( ErrorDescription );
					HSTRING		hstrErrorDescription( 20, true );
					hwstrErrorDescription.ConvertAndCopyToHString( &hstrErrorDescription );
					DataFormatDelayError( (char *) hstrErrorDescription.Char );
					return;
				}
	virtual	void	DataFormatFindFailed(char * /*ErrorDescription*/)   { return; }
	virtual	void	DataFormatFindFailed( wchar_t *ErrorDescription )
				{
					HWSTRING	hwstrErrorDescription( ErrorDescription );
					HSTRING		hstrErrorDescription( 20, true );
					hwstrErrorDescription.ConvertAndCopyToHString( &hstrErrorDescription );
					DataFormatFindFailed( (char *) hstrErrorDescription.Char );
					return;
				}
	virtual	void	Delay(int /*NumberOfMillisecondsToDelay*/)   { return; }
	virtual	void	DisplayCursorOff(void)   { return; }
	virtual	void	DisplayCursorOnBlink(void)   { return; }
	virtual	void	DisplayCursorOnSteady(void)   { return; }
	virtual	int		DisplayHeight(void)   { return(0); }
	virtual	void	DisplayLanguage(char * /*Result*/)   { return; }
	virtual	void	DisplayLanguage( wchar_t *Result )
				{
					HSTRING		hstrResult( MAXIMUM_STRING_SIZE, true );
					DisplayLanguage( (char *) hstrResult.Char );
					hstrResult.Size = strlen( (char *) hstrResult.Char );
					HWSTRING	hwstrResult( hstrResult.Size, true );
					hwstrResult.ConvertAndCopyFromHString( &hstrResult );
					hwstrResult.CopyFromString( Result, MAXIMUM_STRING_SIZE );
					return;
				}
	virtual	int		DisplayMenu(MENU_PARAMETERS * /*MenuParameters*/, unsigned char * /*SelectedMenuItemText*/, unsigned int * /*LengthOfSelectedMenuItemText*/)
					{ return(-1); }
	virtual	int		DisplayMenu(MENU_PARAMETERS *MenuParameters, wchar_t *SelectedMenuItemText, unsigned int *LengthOfSelectedMenuItemText)
				{
					HSTRING		hstrSelectedMenuItemText( MAXIMUM_STRING_SIZE, true );
					int Status = DisplayMenu( MenuParameters, hstrSelectedMenuItemText.Char, LengthOfSelectedMenuItemText );
					hstrSelectedMenuItemText.Size = *LengthOfSelectedMenuItemText;
					HWSTRING	hwstrSelectedMenuItemText( hstrSelectedMenuItemText.Size, true );
					hwstrSelectedMenuItemText.ConvertAndCopyFromHString( &hstrSelectedMenuItemText );
					hwstrSelectedMenuItemText.CopyFromString( SelectedMenuItemText, MAXIMUM_STRING_SIZE );
					return Status;
				}
	virtual	void	DisplayOn(void)   { return; }
	virtual	void	DisplayOff(void)   { return; }
	virtual	int		DisplayWidth(void)   { return(0); }
	virtual	void	DoBadIndicator(void)   { return; }
	virtual	void	DoGoodIndicator(void)   { return; }
	virtual	void	EraseDisplay(void)   { return; }
	virtual	void	EraseLine(int /*X*/, int /*Y*/)   { return; }
	virtual int		ExecuteRunTimeScriptCommand(void)   { return(EXEC_RTCMD_END_COMMAND_ENCOUNTERED); }
	virtual	void	FlushKeyPadBuffer(void)   { return; }
	virtual	int		GetFont(void)   { return(0); }
	virtual	int		GetFromHost(char * /*String*/)   { return(0); }
	virtual	int		GetFromHost( wchar_t *String )
				{
					HSTRING		hstrString( MAXIMUM_STRING_SIZE, true );
					hstrString.Size = GetFromHost( (char *) hstrString.Char );
					HWSTRING	hwstrString( hstrString.Size, true );
					hwstrString.ConvertAndCopyFromHString( &hstrString );
					hwstrString.CopyFromString( String, MAXIMUM_STRING_SIZE );
					return hwstrString.Size;
				}
	virtual	int		GetSystemTimer0(void)   { return(0); }
	virtual	void	IndicatorLightOn(int /*Color*/, int /*OnCycleTime*/, int /*NumberOfCycles*/)   { return; }
	virtual	int		InputCharacter(int /*ControlMask*/, int /*Timeout*/)   { return(0); }
	virtual	int		InputNumber(int /*ControlMask*/, int * /*InputData*/, int /*LowEndOfRange*/, int /*HighEndOfRange*/,
					int /*DefaultValue*/, char * /*DataSource*/, int /*Timeout*/, char * /*QuitKeys*/)
						{ return(0); }
	virtual	int		InputNumber(int ControlMask, int *InputData, int LowEndOfRange, int HighEndOfRange,
					int DefaultValue, wchar_t *DataSource, int Timeout, char *QuitKeys )
				{
					HSTRING		hstrDataSource( MAXIMUM_STRING_SIZE, true );
					int Status = InputNumber( ControlMask, InputData, LowEndOfRange, HighEndOfRange, DefaultValue,
								(char *) hstrDataSource.Char, Timeout, QuitKeys );
					hstrDataSource.Size = strlen( (char *) hstrDataSource.Char );
					HWSTRING	hwstrDataSource( hstrDataSource.Size, true );
					hwstrDataSource.ConvertAndCopyFromHString( &hstrDataSource );
					hwstrDataSource.CopyFromString( DataSource, MAXIMUM_STRING_SIZE );
					return Status;
				}
// Note that for InputString, you're on your honor to not exceed MAXIMUM_STRING_SIZE
	virtual	int		InputString(int /*ControlMask*/, HSTRING * /*InputData*/, int /*MinimumLength*/,
					int /*MaximumLength*/, char * /*DataSourceInformation*/, int /*Timeout*/, char * /*QuitFlags*/)
						{ return(0); }
	virtual	int		InputString(int ControlMask, HWSTRING *InputData, int MinimumLength,
					int MaximumLength, wchar_t *DataSourceInformation, int Timeout, char *QuitFlags )
				{
					HSTRING		hstrInputData( MAXIMUM_STRING_SIZE, true );
					InputData->ConvertAndCopyToHString( &hstrInputData );
					HSTRING		hstrDataSource( MAXIMUM_STRING_SIZE, true );
					int Status = InputString( ControlMask, &hstrInputData, MinimumLength, MaximumLength,
								(char *) hstrDataSource.Char, Timeout, QuitFlags );
					hstrDataSource.Size = strlen( (char *) hstrDataSource.Char );
					HWSTRING	hwstrDataSource( hstrDataSource.Size, true );
					hwstrDataSource.ConvertAndCopyFromHString( &hstrDataSource );
					hwstrDataSource.CopyFromString( DataSourceInformation, MAXIMUM_STRING_SIZE );
					InputData->ConvertAndCopyFromHString( &hstrInputData );
					return Status;
				}
	virtual	int		InputString(int ControlMask, HWSTRING *InputData, int MinimumLength,
					int MaximumLength, wchar_t *DataSourceInformation, int Timeout, char *QuitFlags,
					int * /* ControlFlags */ )
				{
					return 	InputString( ControlMask, InputData, MinimumLength, MaximumLength, DataSourceInformation, Timeout,
								QuitFlags );
				}

public:
	typedef struct {
		int				ControlMask;
		HWSTRING		*InputData;
		int				MinimumLength;
		int				MaximumLength;
		wchar_t			*DataSourceInformation;
		int				Timeout;
		char			*QuitFlags;
		int				ControlFlags;
		int				DelayInMicroseconds;
	} INPUT_STRING_PARAMETERS;

protected:
	virtual	int		InputString( INPUT_STRING_PARAMETERS *Parameters )
				{
					Parameters->DelayInMicroseconds = 0;
					return 	InputString( Parameters->ControlMask, Parameters->InputData, Parameters->MinimumLength,
								Parameters->MaximumLength, Parameters->DataSourceInformation, Parameters->Timeout,
								Parameters->QuitFlags, &(Parameters->ControlFlags) );
				}


	virtual	bool	IsScriptingAvailable( void )   { return true; }
	virtual	void	KeyClick(int /*KeyClickSetting*/)   { return; }
	virtual	int		KeyHit(int /*ControlMask*/)   { return(0); }
	virtual	int		LastSystemOperationStatus(void)   { return(0); }
	virtual	bool	LoadSystemBitmap(int /*BitmapNumber*/, int /*XCoordOfUpperLeft*/, int /*YcoordOfUpperLeft*/)   { return(false); }
	virtual	void	LogError( const char * /*Error*/)   { return; }
	virtual	void	LogError( const wchar_t *Error)
				{
					HWSTRING	hwstrError( Error );
					HSTRING		hstrError( 20, true );
					hwstrError.ConvertAndCopyToHString( &hstrError );
					LogError( (char *) hstrError.Char );
					return;
				}
	virtual	bool	MenuCommand(char * /*MenuCommandString*/, int * /*LengthOfMenuCommandString*/, int /*MaximumLengthOfString*/)
					{ return(false); }
	virtual	bool	MenuCommand( wchar_t *MenuCommandString, int *LengthOfMenuCommandString, int MaximumLengthOfString )
				{
					HWSTRING	hwstrMenuCommandString( MaximumLengthOfString, true );
					hwstrMenuCommandString.Copy( MenuCommandString, (unsigned int) *LengthOfMenuCommandString );
					HSTRING		hstrMenuCommandString( MaximumLengthOfString, true );
					hwstrMenuCommandString.ConvertAndCopyToHString( &hstrMenuCommandString );
					bool Status = MenuCommand( (char * ) hstrMenuCommandString.Char, LengthOfMenuCommandString,
									MaximumLengthOfString );
					hstrMenuCommandString.Size = *LengthOfMenuCommandString;
					hwstrMenuCommandString.ConvertAndCopyFromHString( &hstrMenuCommandString );
					hwstrMenuCommandString.CopyFromString( MenuCommandString, MaximumLengthOfString );
					return Status;
				}
	virtual	bool	MoveDisplayCursor(int /*XCoord*/, int /*YCoord*/)   { return(false); }
	virtual	bool	OutputString(int /*VideoMode*/, int /*Font*/, int /*ControlMask*/, HSTRING * /*StringToOutput*/)
						{ return(false); }
	virtual	bool	OutputString( int VideoMode, int Font, int ControlMask, HWSTRING *StringToOutput )
				{
					HSTRING		hstrStringToOutput( StringToOutput->Size, true );
					StringToOutput->ConvertAndCopyFromHString( &hstrStringToOutput );
					return 	OutputString( VideoMode, Font, ControlMask, &hstrStringToOutput );
				}
	virtual	void	PreserveCurrentDisplay(void)   { return; }
	virtual	void	ProcessBackgroundOperationsDuringScriptExecution(void)   { return; }
	virtual	void	ProcessErrorFlag(void)   { return; }
	virtual	void	ProcessTerminateFlag(void)   { TerminateHHPLProgram(); return; }
	virtual	void	ProcessWarningFlag(void)   { return; }
	virtual	int		RadioStrength(void)   { return(0); }
	virtual	bool	RestorePreservedDisplay(void)   { return(false); }
	virtual	void	ScannerID(char * /*Result*/)   { return; }
	virtual	void	ScannerID( wchar_t *Result)
				{
					HSTRING		hstrResult( MAXIMUM_STRING_SIZE, true );
					ScannerID( (char *) hstrResult.Char );
					hstrResult.Size = strlen( (char *) hstrResult.Char );
					HWSTRING	hwstrResult( hstrResult.Size, true );
					hwstrResult.ConvertAndCopyFromHString( &hstrResult );
					hwstrResult.CopyFromString( Result, MAXIMUM_STRING_SIZE );
					return;
				}
	virtual	bool	ScrollDisplay(int /*NumberOfLinesToScroll*/)   { return(false); }
	virtual	void	SelectFont(int /*FontNumber*/)   { return; }

public:
	typedef struct {
		const wchar_t	*String;
		int				LengthOfString;
		const wchar_t	*DataSourceInformation;
		int				ControlMask;
		int				DelayInMicroseconds;
		int				ControlFlags;
	} SEND_TO_HOST_PARAMETERS;

protected:
	virtual	bool	SendToHost( SEND_TO_HOST_PARAMETERS * /* Parameters */ )     { return(false); }






	virtual	void	TempEnable(int /*Device*/, int /*Time*/)   { return; }
	virtual	int		TerminalID(void)   { return(0); }
	virtual	void	TerminateHHPLProgram(void)   { TerminateHHPLProgramAtNextInstruction = true;  return; }
	virtual	bool	TestRoutine(int /*TestRoutineNumber*/, int /*NumberOfParameters*/, void ** /*ParameterPointers*/)   { return(false); }
	virtual	void	ToggleAuxiliaryPort(void)   { return; }
	virtual	bool	TriggerHit(void)   { return(false); }
	virtual	int		TxSuccess(void)   { return(0); }
	virtual	int		Workgroup(void)   { return(0); }
	virtual	void	ZeroSystemTimer0(void)   { return; }









public:
//	Constructor for HHPL_PROCESSOR
	HHPL_PROCESSOR(void);


//	Destructor for HHPL_PROCESSOR
	~HHPL_PROCESSOR(void);


// Attempt to process all routines that are part of an HHPL format program.
	void ProcessHHPLProgram(const unsigned char *InputCode, unsigned int InputCodeSize, const wchar_t *InputData,
				unsigned int InputDataSize, const wchar_t *InputDataSourceID = DEFAULT_DATA_SOURCE_ID,
				unsigned int InputDataSourceIDSize = 4, int InputDataControlFlags = HHPL_INPUT_AND_SEND_TO_HOST_DEFAULT );

// Attempt to process all routines that are part of an HHPL format program, using char input rather than wchar_t input.
	void ProcessHHPLProgram(const unsigned char *InputCode, unsigned int InputCodeSize, const unsigned char *InputData,
				unsigned int InputDataSize, const unsigned char *InputDataSourceID, unsigned int InputDataSourceIDSize,
				int InputDataControlFlags = HHPL_INPUT_AND_SEND_TO_HOST_DEFAULT );



// Return the status of the HHPL interpreter.
	bool HHPLInterpreterIdle(void);
	bool HHPLInterpreterBusy(void);
	bool HHPLInterpreterSuspended(void);
	bool HHPLProgramTerminated(void);


// Host routine execution.
	bool ExecuteHostRoutine(wchar_t *RoutineName, int RoutineNameLength, bool UseEditBufferContents,
					wchar_t *EditBufferContents, int EditBufferContentsLength);




private:



// Define the number of processor boolean, integer, and string registers
	#define	NUMBER_OF_REGISTERS									16

// Allocate processor boolean, integer, and string registers
	bool		BooleanRegisters[NUMBER_OF_REGISTERS];
	int			IntegerRegisters[NUMBER_OF_REGISTERS];
	HSTRING		*StringRegisters[NUMBER_OF_REGISTERS];
	HWSTRING	*WideStringRegisters[NUMBER_OF_REGISTERS];


	enum HHPL_INTERPRETER_STATE_FLAGS {
		HHPL_INTERPRETER_IDLE = 0,
		HHPL_INTERPRETER_BUSY,
		HHPL_INTERPRETER_SUSPENDED
	};

	int		HHPLInterpreterState;



// When set, this flag indicates that we're to skip over instructions rather than execute them.
// It is used when we're in an instruction error state and want to get past any remaining sub-
// instructions without actually performing operations associated with them.  An example would be
// InputString returning an error; we don't want the string variable to be set in this case, so
// we set this flag, execute the SetStringVariable instruction (which won't set the variable,
// because this flag is set), and clear this flag.
	bool	SkipOverInstructionsWithoutExecutingThem;

	enum {
		HHPL_PROC_GET_BOOLEAN_PARAMETER=0,
		HHPL_PROC_SET_BOOLEAN_PARAMETER,
		HHPL_PROC_GET_NUMERIC_PARAMETER,
		HHPL_PROC_SET_NUMERIC_PARAMETER,
		HHPL_PROC_GET_STRING_PARAMETER,
		HHPL_PROC_STRING_RETURN_PARAMETER,
		HHPL_PROC_SET_STRING_PARAMETER,
		HHPL_PROC_GET_WIDE_STRING_PARAMETER,
		HHPL_PROC_WIDE_STRING_RETURN_PARAMETER,
		HHPL_PROC_SET_WIDE_STRING_PARAMETER,
		HHPL_PROC_DATA_ADDRESS_PARAMETER,
		HHPL_PROC_CODE_ADDRESS_PARAMETER,
		HHPL_PROC_SUBINSTRUCTION_PARAMETER,
		HHPL_PROC_RANGE_RETURN_PARAMETER,
		HHPL_PROC_PREFIX_RETURN_PARAMETER,
		HHPL_PROC_SUFFIX_RETURN_PARAMETER,
		HHPL_PROC_GET_RANGE_PARAMETER
	};


	#define	INT_STRING_TO_INT_SPEC			"%d"
	#define	INT_WIDE_STRING_TO_INT_SPEC		L"%d"


// Scratch string used for holding output data.  In order to fit in a small memory model, we need
// to conserve stack space and so instead of allocating scratch string space in every routine
// that needs it, we allocate it so that all HHPL processor routines can share the same string.
	char		*TempStr;
	wchar_t		*wchTempStr;



// Define parameters associated with host routine usage by host commands
	int									NumberOfHostAccessibleRoutines;
	int									HostRoutineControlMask;
	HWSTRING							**HostRoutineNames;
	HWSTRING							*NameOfHostRoutineThatWasExecuted;
	HHPL_PROGRAM_PROFILE				*HostRoutineProgramProfile;
	HHPL_INSTRUCTION_PARAMETER_BLOCK	*HostRoutineIPB;




// Define a set of these globally so that we don't have to allocate them every time we do a SendToHost or InputString
// operation.
	SEND_TO_HOST_PARAMETERS				SendToHostParameters;
	INPUT_STRING_PARAMETERS				InputStringParameters;





// Execute a remote host command if one is waiting, then continue on.
	void CheckForCommand(HWSTRING *ReturnString);


// Don't return until a remote host command has been found and an attempt has been made to execute it.
	void WaitForCommand(HWSTRING *ReturnString);


// Don't return until a remote host END command has been received.
	void WaitForMultipleCommands(HWSTRING *ReturnString);



// Send the input string to the output, translating and suppressing characters as we go.
	void SendWithTranslation( wchar_t *StringToSend, unsigned int LengthOfStringToSend,
			unsigned char CharacterToSendAtEnd, HHPL_PROGRAM_PROFILE *ProgramProfile);


// Check to see if the number sent to us is in valid cursor range.
	bool CursorIsValid(HHPL_PROGRAM_PROFILE *ProgramProfile, int NewCursorSetting);


// Move the cursor to the caller specified location.
	void ChangeCursor(HHPL_PROGRAM_PROFILE *ProgramProfile, int NewCursorSetting);


// Check to make sure that there are at least the caller specified minimum number of
// bytes or instructions remaining in the program.
	void CheckForArguments(HHPL_PROGRAM_PROFILE *ProgramProfile,
										   int MinimumNumberOfArguments);


// Return the true if the next instruction flag value for the next instruction.  If the
// instruction is a two byte instruction, check the flag value against the sub-instruction
// rather than the main instruction.
	bool InstructionTypeMatch(HHPL_PROGRAM_PROFILE *ProgramProfile, unsigned int TypeFlag);


// Get the input parameters requested by the caller and store them in operands in the program
// profile.
	bool GetParameters(HHPL_PROGRAM_PROFILE *ProgramProfile,
		HHPL_INSTRUCTION_PARAMETER_BLOCK *IPB,
		const char *InstructionDescription, int NumberOfParameters, int Parameter1Type,
		int Parameter2Type, int Parameter3Type, int Parameter4Type,
		int Parameter5Type, int Parameter6Type, int Parameter7Type);


// Get the test routine parameters requested by the caller and call the test routine.
	bool ProcessTestRoutine(int TestRoutineNumber, int NumberOfParameters,
			HHPL_PROGRAM_PROFILE *ProgramProfile, HHPL_INSTRUCTION_PARAMETER_BLOCK *IPB);


// Execute the next HHPL instruction.
	bool ExecuteNextInstruction(HHPL_PROGRAM_PROFILE *ProgramProfile,
											HHPL_INSTRUCTION_PARAMETER_BLOCK *IPB);

// Attempt to process all instructions that are part of an HHPL routine.
	void ProcessRoutine(HHPL_PROGRAM_PROFILE *ProgramProfile,
											HHPL_INSTRUCTION_PARAMETER_BLOCK *IPB);
private:
	HHPL_PROCESSOR (const HHPL_PROCESSOR&);            // declared private, but not defined, to prevent copying         
	HHPL_PROCESSOR& operator=(const HHPL_PROCESSOR&);  // declared private, but not defined, to prevent copying 

};
#endif      //	__HHPL_PROC_H


