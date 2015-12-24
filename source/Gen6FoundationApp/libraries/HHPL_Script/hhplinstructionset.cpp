//======================================================================================
// HHPLInstructionSet.cpp
//======================================================================================
// $RCSfile: hhplinstructionset.cpp $
// $Revision: 1.1 $
// $Date: 2009/06/12 16:55:17EDT $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Class Name:     HHPL_INSTRUCTION_SET
//
//  Class Description:
//		HHPL_INSTRUCTION_SET defines the HHPL instructions and their behavior.  This
//		class is intended to be inherited in any instance where access to instruction
//		behavior is needed, such as executing an HHPL program or compiling HHPL
//		program source.
//
//---------------------------------------------------------------------------





#include	"HHPLInstructionSet.h"	// HHPL instruction set definitions

#include	<ctype.h>			// data type conversion functions
#include	<string.h>			// 'C' string functions
#include	<stdio.h>			// Standard I/O







//
//	Constructor for HHPL_INSTRUCTION_SET
//		Setup the instruction return types here.
//
HHPL_INSTRUCTION_SET::HHPL_INSTRUCTION_SET()
{
	int		i;




	HHPLTimingOperationSubInstructionFlags = new unsigned int[HHPL_TimerOperationSubInst_NumberOfInstructions];
	HHPLDisplayControlSubInstructionFlags = new unsigned int[HHPL_DisplayControlSubInst_NumberOfInstructions];
	HHPLDisplayOutputSubInstructionFlags = new unsigned int[HHPL_DisplayOutputSubInst_NumberOfInstructions];
	HHPLAlarmSignalingSubInstructionFlags = new unsigned int[HHPL_AlarmSignalingSubInst_NumberOfInstructions];
	HHPLInputDeviceSubInstructionFlags = new unsigned int[HHPL_InputDeviceSubInst_NumberOfInstructions];
	HHPLCommunicationsSubInstructionFlags = new unsigned int[HHPL_CommunicationsSubInst_NumberOfInstructions];
	HHPLSpecialOperationSubInstructionFlags = new unsigned int[HHPL_SpecialOperationSubInst_NumberOfInstructions];
	HHPLPowerManagementOperationSubInstructionFlags = new unsigned int[HHPL_PowerManagementSubInst_NumberOfInstructions];
	HHPLOSConfigurationSubInstructionFlags = new unsigned int[HHPL_OSConfigurationSubInst_NumberOfInstructions];
	HHPLOSStatusSubInstructionFlags = new unsigned int[HHPL_OSStatusSubInst_NumberOfInstructions];
	HHPLRemoteHostSubInstructionFlags = new unsigned int[HHPL_RemoteHostSubInst_NumberOfInstructions];
	HHPLWideStringConversionSubInstructionFlags = new unsigned int[ HHPL_WideStringOperationsSubInst_NumberOfInstructions ];
	HHPLWideStringVariableSubInstructionFlags = new unsigned int[ HHPL_WideStringVariableOperationsSubInst_NumberOfInstructions ];
	HHPLPrefixSuffixDefaultSubInstructionFlags = new unsigned int[HHPL_PrefixSuffixDefaultSubInst_NumberOfInstructions];
	HHPLPrefixSuffixSubInstructionFlags = new unsigned int[HHPL_PrefixSuffixSubInst_NumberOfInstructions];
	HHPLTwoOperandStringOperationSubInstructionFlags = new unsigned int[HHPL_TwoOperandStringOperationSubInst_NumberOfInstructions];
	HHPLSingleOperandStringOperationSubInstructionFlags = new unsigned int[HHPL_SingleOperandStringOperationSubInst_NumberOfInstructions];
	HHPLStringComparisonSubInstructionFlags = new unsigned int[HHPL_StringComparisonSubInst_NumberOfInstructions];
	HHPLComparisonDefaultSubInstructionFlags = new unsigned int[HHPL_ComparisonDefaultsSubInst_NumberOfInstructions];
	HHPLInstructionFlags = new unsigned int[HHPL_MaximumInstructions];
	HHPLSubInstructionFlagPointers = new unsigned int *[HHPL_MaximumInstructions];




// Timing operation sub-instruction flags
	HHPLTimingOperationSubInstructionFlags[HHPL_TimerOperationSubInst_ResetTimer0] =
												HHPL_Inst_Ret_NoResult;
	HHPLTimingOperationSubInstructionFlags[HHPL_TimerOperationSubInst_GetTimer0] =
												HHPL_Inst_Ret_Number;
	HHPLTimingOperationSubInstructionFlags[HHPL_TimerOperationSubInst_DelayMilliseconds] =
												HHPL_Inst_Ret_NoResult;



// Display control operation sub-instruction flags
	HHPLDisplayControlSubInstructionFlags[HHPL_DisplayControlSubInst_DisplayBacklightOn] =
												HHPL_Inst_Ret_NoResult;
	HHPLDisplayControlSubInstructionFlags[HHPL_DisplayControlSubInst_DisplayBacklightOff] =
												HHPL_Inst_Ret_NoResult;
	HHPLDisplayControlSubInstructionFlags[HHPL_DisplayControlSubInst_DisplayOn] =
												HHPL_Inst_Ret_NoResult;
	HHPLDisplayControlSubInstructionFlags[HHPL_DisplayControlSubInst_DisplayOff] =
												HHPL_Inst_Ret_NoResult;
	HHPLDisplayControlSubInstructionFlags[HHPL_DisplayControlSubInst_EraseDisplay] =
												HHPL_Inst_Ret_NoResult;
	HHPLDisplayControlSubInstructionFlags[HHPL_DisplayControlSubInst_SetFont] =
												HHPL_Inst_Ret_NoResult;
	HHPLDisplayControlSubInstructionFlags[HHPL_DisplayControlSubInst_RestorePreservedDisplay] =
												HHPL_Inst_Ret_Boolean;
	HHPLDisplayControlSubInstructionFlags[HHPL_DisplayControlSubInst_PreserveCurrentDisplay] =
												HHPL_Inst_Ret_NoResult;
	HHPLDisplayControlSubInstructionFlags[HHPL_DisplayControlSubInst_MoveDisplayCursor] =
												HHPL_Inst_Ret_Boolean;
	HHPLDisplayControlSubInstructionFlags[HHPL_DisplayControlSubInst_DisplayHeight] =
												HHPL_Inst_Ret_Number;
	HHPLDisplayControlSubInstructionFlags[HHPL_DisplayControlSubInst_DisplayWidth] =
												HHPL_Inst_Ret_Number;
	HHPLDisplayControlSubInstructionFlags[HHPL_DisplayControlSubInst_CharacterHeight] =
												HHPL_Inst_Ret_Number;
	HHPLDisplayControlSubInstructionFlags[HHPL_DisplayControlSubInst_CharacterWidth] =
												HHPL_Inst_Ret_Number;
	HHPLDisplayControlSubInstructionFlags[HHPL_DisplayControlSubInst_ScrollDisplay] =
												HHPL_Inst_Ret_Boolean;
	HHPLDisplayControlSubInstructionFlags[HHPL_DisplayControlSubInst_TemporarilyEnableBacklight] =
												HHPL_Inst_Ret_NoResult;
	HHPLDisplayControlSubInstructionFlags[HHPL_DisplayControlSubInst_TemporarilyEnableDisplay] =
												HHPL_Inst_Ret_NoResult;
	HHPLDisplayControlSubInstructionFlags[HHPL_DisplayControlSubInst_EraseLine] =
												HHPL_Inst_Ret_NoResult;
	HHPLDisplayControlSubInstructionFlags[HHPL_DisplayControlSubInst_GetFont] =
												HHPL_Inst_Ret_Number;
	HHPLDisplayControlSubInstructionFlags[HHPL_DisplayControlSubInst_DisplayCursorControl] =
												HHPL_Inst_Ret_Number;


// Display output sub-instruction flags
	HHPLDisplayOutputSubInstructionFlags[HHPL_DisplayOutputSubInst_LoadImage] =
												HHPL_Inst_Ret_Boolean;
	HHPLDisplayOutputSubInstructionFlags[HHPL_DisplayOutputSubInst_OutputString] =
												HHPL_Inst_Ret_Boolean;


// Alarm / signaling sub-instruction flags
	HHPLAlarmSignalingSubInstructionFlags[HHPL_AlarmSignalingSubInst_Beep] =
												HHPL_Inst_Ret_NoResult;
	HHPLAlarmSignalingSubInstructionFlags[HHPL_AlarmSignalingSubInst_IndicatorControl] =
												HHPL_Inst_Ret_NoResult;
	HHPLAlarmSignalingSubInstructionFlags[HHPL_AlarmSignalingSubInst_IssueWarning] =
												HHPL_Inst_Ret_NoResult;
	HHPLAlarmSignalingSubInstructionFlags[HHPL_AlarmSignalingSubInst_IssueError] =
												HHPL_Inst_Ret_NoResult;
	HHPLAlarmSignalingSubInstructionFlags[HHPL_AlarmSignalingSubInst_IssueTerminate] =
												HHPL_Inst_Ret_NoResult;


// Input device sub-instruction flags
	HHPLInputDeviceSubInstructionFlags[HHPL_InputDeviceSubInst_InputCharacter] =
												HHPL_Inst_Ret_Number | HHPL_Inst_Ret_WideString;
	HHPLInputDeviceSubInstructionFlags[HHPL_InputDeviceSubInst_TriggerHit] =
												HHPL_Inst_Ret_Boolean;
	HHPLInputDeviceSubInstructionFlags[HHPL_InputDeviceSubInst_InputNumber] =
												HHPL_Inst_Ret_Boolean | HHPL_Inst_Ret_Number;
	HHPLInputDeviceSubInstructionFlags[HHPL_InputDeviceSubInst_InputString] =
												HHPL_Inst_Ret_Number;
	HHPLInputDeviceSubInstructionFlags[HHPL_InputDeviceSubInst_KeyHit] =
												HHPL_Inst_Ret_Number;
	HHPLInputDeviceSubInstructionFlags[HHPL_InputDeviceSubInst_KeyOperations] =
												HHPL_Inst_Ret_NoResult;
	HHPLInputDeviceSubInstructionFlags[HHPL_InputDeviceSubInst_InputStringWithControlFlags] =
												HHPL_Inst_Ret_Number;


// Communications sub-instruction flags
	HHPLCommunicationsSubInstructionFlags[HHPL_CommunicationsSubInst_Send] =
												HHPL_Inst_Ret_Boolean;
	HHPLCommunicationsSubInstructionFlags[HHPL_CommunicationsSubInst_RadioSignalStrength] =
												HHPL_Inst_Ret_Number;
	HHPLCommunicationsSubInstructionFlags[HHPL_CommunicationsSubInst_RadioTransmissionFailures] =
												HHPL_Inst_Ret_Number;
	HHPLCommunicationsSubInstructionFlags[HHPL_CommunicationsSubInst_SendWithControlFlags] =
												HHPL_Inst_Ret_Boolean;


// Special operations sub-instruction flags
	HHPLSpecialOperationSubInstructionFlags[HHPL_SpecialOperationSubInst_DisplayMenu] =
												HHPL_Inst_Ret_WideString | HHPL_Inst_Ret_Number;


// Power management operations sub-instruction flags
	HHPLPowerManagementOperationSubInstructionFlags[HHPL_PowerManagementSubInst_BatteryStrength] =
												HHPL_Inst_Ret_Number;


// O/S configuration operations sub-instruction flags
	HHPLOSConfigurationSubInstructionFlags[HHPL_OSConfigurationSubInst_MenuCommand] =
												HHPL_Inst_Ret_Boolean | HHPL_Inst_Ret_WideString;
	HHPLOSConfigurationSubInstructionFlags[HHPL_OSConfigurationSubInst_DisplayLanguage] =
												HHPL_Inst_Ret_WideString;
	HHPLOSConfigurationSubInstructionFlags[HHPL_OSConfigurationSubInst_InterpreterVersion] =
												HHPL_Inst_Ret_WideString;
	HHPLOSConfigurationSubInstructionFlags[HHPL_OSConfigurationSubInst_ScannerID] =
												HHPL_Inst_Ret_WideString;
	HHPLOSConfigurationSubInstructionFlags[HHPL_OSConfigurationSubInst_Workgroup] =
												HHPL_Inst_Ret_Number;
	HHPLOSConfigurationSubInstructionFlags[HHPL_OSConfigurationSubInst_TerminalID] =
												HHPL_Inst_Ret_Number;
	HHPLOSConfigurationSubInstructionFlags[HHPL_OSConfigurationSubInst_GetEditBufferControlFlags] =
												HHPL_Inst_Ret_Number;
	HHPLOSConfigurationSubInstructionFlags[HHPL_OSConfigurationSubInst_SetEditBufferControlFlags] =
												HHPL_Inst_Ret_Number |
												HHPL_Inst_Ret_Boolean |
												HHPL_Inst_Set_Integer;


// O/S status operations sub-instruction flags
	HHPLOSStatusSubInstructionFlags[HHPL_OSStatusSubInst_LastOperation] =
												HHPL_Inst_Ret_Number;


// Remote host operations sub-instruction flags
	HHPLRemoteHostSubInstructionFlags[HHPL_RemoteHostSubInst_WaitForCommand] =
												HHPL_Inst_Ret_WideString;
	HHPLRemoteHostSubInstructionFlags[HHPL_RemoteHostSubInst_WaitForMultipleCommands] =
												HHPL_Inst_Ret_WideString;
	HHPLRemoteHostSubInstructionFlags[HHPL_RemoteHostSubInst_CheckForCommand] =
												HHPL_Inst_Ret_WideString;


// Wide string conversion operations sub-instruction flags
	HHPLWideStringConversionSubInstructionFlags[ HHPL_WideStringOperationsSubInst_WideStringToBool ] = HHPL_Inst_Ret_Boolean;
	HHPLWideStringConversionSubInstructionFlags[ HHPL_WideStringOperationsSubInst_WideStringToInt ] = HHPL_Inst_Ret_Number;
	HHPLWideStringConversionSubInstructionFlags[ HHPL_WideStringOperationsSubInst_WideStringToString ] = HHPL_Inst_Ret_String;
	HHPLWideStringConversionSubInstructionFlags[ HHPL_WideStringOperationsSubInst_IntToWideString ] = HHPL_Inst_Ret_WideString;
	HHPLWideStringConversionSubInstructionFlags[ HHPL_WideStringOperationsSubInst_BoolToWideString ] = HHPL_Inst_Ret_WideString;
	HHPLWideStringConversionSubInstructionFlags[ HHPL_WideStringOperationsSubInst_StringToWideString ] = HHPL_Inst_Ret_WideString;
	HHPLWideStringConversionSubInstructionFlags[ HHPL_WideStringOperationsSubInst_LengthWideString ] = HHPL_Inst_Ret_Number;
	HHPLWideStringConversionSubInstructionFlags[ HHPL_WideStringOperationsSubInst_SetWideStringLength ] = HHPL_Inst_Ret_NoResult;
	HHPLWideStringConversionSubInstructionFlags[ HHPL_WideStringOperationsSubInst_SetWideStringArrayLength ] = HHPL_Inst_Ret_NoResult;
	HHPLWideStringConversionSubInstructionFlags[ HHPL_WideStringOperationsSubInst_WideCharToInt ] = HHPL_Inst_Ret_Number;
	HHPLWideStringConversionSubInstructionFlags[ HHPL_WideStringOperationsSubInst_IntToWideChar ] = HHPL_Inst_Ret_WideString;
	HHPLWideStringConversionSubInstructionFlags[ HHPL_WideStringOperationsSubInst_WideCharToChar ] = HHPL_Inst_Ret_String;
	HHPLWideStringConversionSubInstructionFlags[ HHPL_WideStringOperationsSubInst_CharToWideChar ] = HHPL_Inst_Ret_WideString;


// Wide string variable operations sub-instruction flags
	HHPLWideStringVariableSubInstructionFlags[ HHPL_WideStringVariableOperationsSubInst_SetWideStringVariable ] =
							HHPL_Inst_Set_WideString;
	HHPLWideStringVariableSubInstructionFlags[ HHPL_WideStringVariableOperationsSubInst_SetWideStringArrayVariable ] =
							HHPL_Inst_Set_WideString;
	HHPLWideStringVariableSubInstructionFlags[ HHPL_WideStringVariableOperationsSubInst_GetWideStringVariable ] =
							HHPL_Inst_Ret_WideString;
	HHPLWideStringVariableSubInstructionFlags[ HHPL_WideStringVariableOperationsSubInst_GetWideStringArrayVariable ] =
							HHPL_Inst_Ret_WideString;
	HHPLWideStringVariableSubInstructionFlags[ HHPL_WideStringVariableOperationsSubInst_WideStringDataInRange ] =
							HHPL_Inst_Ret_WideString |
							HHPL_Inst_Ret_Boolean;
	HHPLWideStringVariableSubInstructionFlags[ HHPL_WideStringVariableOperationsSubInst_WideStringSetArrayDataInRange ] =
							HHPL_Inst_Set_WideString;
	HHPLWideStringVariableSubInstructionFlags[ HHPL_WideStringVariableOperationsSubInst_WideStringArrayDataInRange ] =
							HHPL_Inst_Ret_WideString |
							HHPL_Inst_Ret_Boolean;
	HHPLWideStringVariableSubInstructionFlags[ HHPL_WideStringVariableOperationsSubInst_WideStringSetDataInRange ] =
							HHPL_Inst_Set_WideString;
	HHPLWideStringVariableSubInstructionFlags[ HHPL_WideStringVariableOperationsSubInst_WideStringRegister ] =
							HHPL_Inst_Ret_WideString;


// Prefix / suffix default operations sub-instruction flags
	HHPLPrefixSuffixDefaultSubInstructionFlags
				[HHPL_PrefixSuffixDefaultSubInst_DefineDefaultPrefixToBeAlwaysApplied] =
												HHPL_Inst_Ret_NoResult;
	HHPLPrefixSuffixDefaultSubInstructionFlags
				[HHPL_PrefixSuffixDefaultSubInst_DefineDefaultPrefixToBeAppliedOnNonNullData] =
												HHPL_Inst_Ret_NoResult;
	HHPLPrefixSuffixDefaultSubInstructionFlags
				[HHPL_PrefixSuffixDefaultSubInst_DefineDefaultPrefixToBeAppliedOnNullData] =
												HHPL_Inst_Ret_NoResult;
	HHPLPrefixSuffixDefaultSubInstructionFlags
				[HHPL_PrefixSuffixDefaultSubInst_DefineDefaultSuffixToBeAlwaysApplied] =
												HHPL_Inst_Ret_NoResult;
	HHPLPrefixSuffixDefaultSubInstructionFlags
				[HHPL_PrefixSuffixDefaultSubInst_DefineDefaultSuffixToBeAppliedOnNonNullData] =
												HHPL_Inst_Ret_NoResult;
	HHPLPrefixSuffixDefaultSubInstructionFlags
				[HHPL_PrefixSuffixDefaultSubInst_DefineDefaultSuffixToBeAppliedOnNullData] =
												HHPL_Inst_Ret_NoResult;


// Prefix / suffix operations sub-instruction flags
	HHPLPrefixSuffixSubInstructionFlags[HHPL_PrefixSuffixSubInst_UseDefaultPrefix] =
												HHPL_Inst_Ret_Prefix;
	HHPLPrefixSuffixSubInstructionFlags[HHPL_PrefixSuffixSubInst_UseDefaultSuffix] =
												HHPL_Inst_Ret_Suffix;
	HHPLPrefixSuffixSubInstructionFlags[HHPL_PrefixSuffixSubInst_UseImmediatePrefixAlways] =
												HHPL_Inst_Ret_Prefix;
	HHPLPrefixSuffixSubInstructionFlags[HHPL_PrefixSuffixSubInst_UseImmediatePrefixOnNonNullData] =
												HHPL_Inst_Ret_Prefix;
	HHPLPrefixSuffixSubInstructionFlags[HHPL_PrefixSuffixSubInst_UseImmediatePrefixOnNullData] =
												HHPL_Inst_Ret_Prefix;
	HHPLPrefixSuffixSubInstructionFlags[HHPL_PrefixSuffixSubInst_UseImmediateSuffixAlways] =
												HHPL_Inst_Ret_Suffix;
	HHPLPrefixSuffixSubInstructionFlags[HHPL_PrefixSuffixSubInst_UseImmediateSuffixOnNonNullData] =
												HHPL_Inst_Ret_Suffix;
	HHPLPrefixSuffixSubInstructionFlags[HHPL_PrefixSuffixSubInst_UseImmediateSuffixOnNullData] =
												HHPL_Inst_Ret_Suffix;



// String operations on two results sub-instruction flags
	HHPLTwoOperandStringOperationSubInstructionFlags
				[HHPL_TwoOperandStringOperationSubInst_ConcatenateNextTwoResults] =
												HHPL_Inst_Ret_String;
	HHPLTwoOperandStringOperationSubInstructionFlags
				[HHPL_TwoOperandStringOperationSubInst_ConcatenateNextTwoWideResults] =
												HHPL_Inst_Ret_WideString;


// String operations on one result sub-instruction flags
	HHPLSingleOperandStringOperationSubInstructionFlags
				[HHPL_SingleOperandStringOperationSubInst_ConvertStringToUpperCase] =
												HHPL_Inst_Ret_String;
	HHPLSingleOperandStringOperationSubInstructionFlags
				[HHPL_SingleOperandStringOperationSubInst_ConvertStringToLowerCase] =
												HHPL_Inst_Ret_String;
	HHPLSingleOperandStringOperationSubInstructionFlags
				[HHPL_SingleOperandStringOperationSubInst_ConvertWideStringToUpperCase] =
												HHPL_Inst_Ret_WideString;
	HHPLSingleOperandStringOperationSubInstructionFlags
				[HHPL_SingleOperandStringOperationSubInst_ConvertWideStringToLowerCase] =
												HHPL_Inst_Ret_WideString;


// String comparison operations sub-instruction flags
	HHPLStringComparisonSubInstructionFlags
				[HHPL_StringComparisonSubInst_CaseSensitiveCompareString] =
												HHPL_Inst_Ret_Boolean;
	HHPLStringComparisonSubInstructionFlags
				[HHPL_StringComparisonSubInst_CaseInsensitiveCompareString] =
												HHPL_Inst_Ret_Boolean;
	HHPLStringComparisonSubInstructionFlags
				[HHPL_StringComparisonSubInst_CompareStringAccordingToCaseSensitivitySetting] =
												HHPL_Inst_Ret_Boolean;
	HHPLStringComparisonSubInstructionFlags
				[HHPL_StringComparisonSubInst_WildcardCaseSensitiveCompareString] =
												HHPL_Inst_Ret_Boolean;
	HHPLStringComparisonSubInstructionFlags
				[HHPL_StringComparisonSubInst_WildcardCaseInsensitiveCompareString] =
												HHPL_Inst_Ret_Boolean;
	HHPLStringComparisonSubInstructionFlags
		[HHPL_StringComparisonSubInst_WildcardCompareStringAccordingToCaseSensitivitySetting] =
												HHPL_Inst_Ret_Boolean;
	HHPLStringComparisonSubInstructionFlags
				[HHPL_StringComparisonSubInst_CaseSensitiveCompareWideString] =
												HHPL_Inst_Ret_Boolean;
	HHPLStringComparisonSubInstructionFlags
				[HHPL_StringComparisonSubInst_CaseInsensitiveCompareWideString] =
												HHPL_Inst_Ret_Boolean;
	HHPLStringComparisonSubInstructionFlags
				[HHPL_StringComparisonSubInst_CompareWideStringAccordingToCaseSensitivitySetting] =
												HHPL_Inst_Ret_Boolean;
	HHPLStringComparisonSubInstructionFlags
				[HHPL_StringComparisonSubInst_WildcardCaseSensitiveCompareWideString] =
												HHPL_Inst_Ret_Boolean;
	HHPLStringComparisonSubInstructionFlags
				[HHPL_StringComparisonSubInst_WildcardCaseInsensitiveCompareWideString] =
												HHPL_Inst_Ret_Boolean;
	HHPLStringComparisonSubInstructionFlags
		[HHPL_StringComparisonSubInst_WildcardCompareWideStringAccordingToCaseSensitivitySetting] =
												HHPL_Inst_Ret_Boolean;


// Comparison defaults sub-instruction flags
	HHPLComparisonDefaultSubInstructionFlags
				[HHPL_ComparisonDefaultsSubInst_ComparesAreCaseSensitiveByDefault] =
												HHPL_Inst_Ret_NoResult;
	HHPLComparisonDefaultSubInstructionFlags
				[HHPL_ComparisonDefaultsSubInst_ComparesAreCaseInsensitiveByDefault] =
												HHPL_Inst_Ret_NoResult;





// Return types for no-op.  Note that even though NOP doesn't do anything, we'll tag this
// as being able to process variable assignments so that it can be used in instructions that
// want to write to optional arguments, but no argument was actually specified.  For example,
// HHPL_Inst_StringToInt will write the number of characters used in conversion to a variable,
// but if no variable is specified at compile time, this NOP instruction may be substituted, and
// since this instruction is indicating here that it will set an integer, the HHPL_Inst_StringToInt
// processing is happy.
	HHPLInstructionFlags[HHPL_Inst_NoOp] =
							HHPL_Inst_Set_Boolean |
							HHPL_Inst_Set_Integer |
							HHPL_Inst_Set_String |
							HHPL_Inst_Set_WideString;




// HHPL I/O & signaling commands
	HHPLInstructionFlags[HHPL_Inst_TimerOperations] = HHPL_Inst_TwoByteInstruction;
	HHPLInstructionFlags[HHPL_Inst_DisplayControlOperations] = HHPL_Inst_TwoByteInstruction;
	HHPLInstructionFlags[HHPL_Inst_DisplayOutputOperations] = HHPL_Inst_TwoByteInstruction;
	HHPLInstructionFlags[HHPL_Inst_AlarmSignalingOperations] = HHPL_Inst_TwoByteInstruction;
	HHPLInstructionFlags[HHPL_Inst_InputDeviceOperations] = HHPL_Inst_TwoByteInstruction;
	HHPLInstructionFlags[HHPL_Inst_CommunicationsOperations] = HHPL_Inst_TwoByteInstruction;
	HHPLInstructionFlags[HHPL_Inst_SpecialOperations] = HHPL_Inst_TwoByteInstruction;
	HHPLInstructionFlags[HHPL_Inst_PowerManagementOperations] = HHPL_Inst_TwoByteInstruction;


// HHPL O/S service instructions
	HHPLInstructionFlags[HHPL_Inst_OSConfigurationOperations] = HHPL_Inst_TwoByteInstruction;
	HHPLInstructionFlags[HHPL_Inst_OSStatusOperations] = HHPL_Inst_TwoByteInstruction;



// Constants and NULLs
	HHPLInstructionFlags[HHPL_Inst_NullInteger] = HHPL_Inst_Ret_Number;
	HHPLInstructionFlags[HHPL_Inst_NullString] = HHPL_Inst_Ret_String;
	HHPLInstructionFlags[HHPL_Inst_NullPrefix] = HHPL_Inst_Ret_Prefix;
	HHPLInstructionFlags[HHPL_Inst_NullSuffix] = HHPL_Inst_Ret_Suffix;
	HHPLInstructionFlags[HHPL_Inst_NullWideString] = HHPL_Inst_Ret_WideString;


// HHPL control transfer commands
	HHPLInstructionFlags[HHPL_Inst_IfCommand] = HHPL_Inst_Ret_NoResult;
	HHPLInstructionFlags[HHPL_Inst_WhileCommand] = HHPL_Inst_Ret_NoResult;
	HHPLInstructionFlags[HHPL_Inst_DoWhileCommand] = HHPL_Inst_Ret_NoResult;
	HHPLInstructionFlags[HHPL_Inst_ExecuteSubroutine] = HHPL_Inst_Ret_NoResult;
	HHPLInstructionFlags[HHPL_Inst_Exit] = HHPL_Inst_Ret_NoResult;
	HHPLInstructionFlags[HHPL_Inst_ForeverDoCommand] = HHPL_Inst_Ret_NoResult;
	HHPLInstructionFlags[HHPL_Inst_TestRoutine] = HHPL_Inst_Ret_NoResult;
	HHPLInstructionFlags[HHPL_Inst_RemoteHostOperations] = HHPL_Inst_TwoByteInstruction;
	HHPLInstructionFlags[HHPL_Inst_GoToCommand] = HHPL_Inst_Ret_NoResult;


// Type conversion operations
	HHPLInstructionFlags[HHPL_Inst_StringToBool] = HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_StringToInt] = HHPL_Inst_Ret_Number;
	HHPLInstructionFlags[HHPL_Inst_IntToBool] = HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_IntToString] = HHPL_Inst_Ret_String;
	HHPLInstructionFlags[HHPL_Inst_BoolToInt] = HHPL_Inst_Ret_Number;
	HHPLInstructionFlags[HHPL_Inst_BoolToString] = HHPL_Inst_Ret_String;
	HHPLInstructionFlags[HHPL_Inst_LengthString] = HHPL_Inst_Ret_Number;
	HHPLInstructionFlags[HHPL_Inst_Format] =
							HHPL_Inst_Ret_WideString |
							HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_SetStringLength] = HHPL_Inst_Ret_NoResult;
	HHPLInstructionFlags[HHPL_Inst_SetStringArrayLength] = HHPL_Inst_Ret_NoResult;
	HHPLInstructionFlags[HHPL_Inst_CharToInt] = HHPL_Inst_Ret_Number;
	HHPLInstructionFlags[HHPL_Inst_IntToChar] = HHPL_Inst_Ret_String;


// HHPL wide string operations
	HHPLInstructionFlags[ HHPL_Inst_WideStringOperations ] = HHPL_Inst_TwoByteInstruction;


// HHPL find string command
	HHPLInstructionFlags
			[HHPL_Inst_FindString] = HHPL_Inst_Ret_Number;


// HHPL variable instructions
	HHPLInstructionFlags[HHPL_Inst_SetBooleanVariable] = HHPL_Inst_Set_Boolean;
	HHPLInstructionFlags[HHPL_Inst_SetBooleanArrayVariable] = HHPL_Inst_Set_Boolean;
	HHPLInstructionFlags[HHPL_Inst_SetIntegerVariable] = HHPL_Inst_Set_Integer;
	HHPLInstructionFlags[HHPL_Inst_SetIntegerArrayVariable] = HHPL_Inst_Set_Integer;
	HHPLInstructionFlags[HHPL_Inst_SetStringVariable] = HHPL_Inst_Set_String;
	HHPLInstructionFlags[HHPL_Inst_SetStringArrayVariable] = HHPL_Inst_Set_String;
	HHPLInstructionFlags[HHPL_Inst_GetBooleanVariable] = HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_GetBooleanArrayVariable] = HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_GetIntegerVariable] = HHPL_Inst_Ret_Number;
	HHPLInstructionFlags[HHPL_Inst_GetIntegerArrayVariable] = HHPL_Inst_Ret_Number;
	HHPLInstructionFlags[HHPL_Inst_GetStringVariable] = HHPL_Inst_Ret_String;
	HHPLInstructionFlags[HHPL_Inst_GetStringArrayVariable] = HHPL_Inst_Ret_String;
	HHPLInstructionFlags[HHPL_Inst_DataInRange] =
							HHPL_Inst_Ret_String |
							HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_DataInEditBufferRange] =
							HHPL_Inst_Ret_WideString |
							HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_DataInDataSourceIDRange] =
							HHPL_Inst_Ret_WideString |
							HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_DataInHostRange] =
							HHPL_Inst_Ret_WideString |
							HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_SetArrayDataInRange] = HHPL_Inst_Set_String;
	HHPLInstructionFlags[HHPL_Inst_ArrayDataInRange] =
							HHPL_Inst_Ret_String |
							HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_SetDataInRange] = HHPL_Inst_Set_String;
	HHPLInstructionFlags[HHPL_Inst_SetEditCursor] =
							HHPL_Inst_Ret_Number |
							HHPL_Inst_Ret_Boolean |
							HHPL_Inst_Set_Integer;
	HHPLInstructionFlags[HHPL_Inst_GetEditCursor] =
							HHPL_Inst_Ret_Number;
	HHPLInstructionFlags[HHPL_Inst_SetEditBuffer] = HHPL_Inst_Set_WideString;
	HHPLInstructionFlags[HHPL_Inst_GetEditBuffer] = HHPL_Inst_Ret_WideString;
	HHPLInstructionFlags[HHPL_Inst_SetRange] =
							HHPL_Inst_Ret_Range |
							HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_SendHost] = HHPL_Inst_Set_WideString;
	HHPLInstructionFlags[HHPL_Inst_GetHost] = HHPL_Inst_Ret_WideString;
	HHPLInstructionFlags[HHPL_Inst_BooleanRegister] = HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_IntegerRegister] = HHPL_Inst_Ret_Number;
	HHPLInstructionFlags[HHPL_Inst_StringRegister] = HHPL_Inst_Ret_String;
	HHPLInstructionFlags[HHPL_Inst_PrefixSuffixDefault] = HHPL_Inst_TwoByteInstruction;
	HHPLInstructionFlags[HHPL_Inst_PrefixSuffix] = HHPL_Inst_TwoByteInstruction;
	HHPLInstructionFlags[HHPL_Inst_SetDataSourceID] = HHPL_Inst_Set_WideString;
	HHPLInstructionFlags[HHPL_Inst_GetDataSourceID] = HHPL_Inst_Ret_WideString;
	HHPLInstructionFlags[ HHPL_Inst_WideStringVariableOperations ] = HHPL_Inst_TwoByteInstruction;




// HHPL string value commands
	HHPLInstructionFlags[HHPL_Inst_UCharLengthString] = HHPL_Inst_Ret_String;
	HHPLInstructionFlags[HHPL_Inst_UShortLengthString] = HHPL_Inst_Ret_String;
	HHPLInstructionFlags[HHPL_Inst_StringOperationOnNextTwoResults] = HHPL_Inst_TwoByteInstruction;
	HHPLInstructionFlags[HHPL_Inst_StringOperationOnNextResult] = HHPL_Inst_TwoByteInstruction;
	HHPLInstructionFlags[HHPL_Inst_UCharLengthWideString] = HHPL_Inst_Ret_WideString;
	HHPLInstructionFlags[HHPL_Inst_UShortLengthWideString] = HHPL_Inst_Ret_WideString;


// HHPL number commands
	HHPLInstructionFlags[HHPL_Inst_Int8] = HHPL_Inst_Ret_Number;
	HHPLInstructionFlags[HHPL_Inst_Int16] = HHPL_Inst_Ret_Number;
	HHPLInstructionFlags[HHPL_Inst_Int32] = HHPL_Inst_Ret_Number;
	HHPLInstructionFlags[HHPL_Inst_NumericOperationOnNextTwoResults] = HHPL_Inst_Ret_Number;
	HHPLInstructionFlags[HHPL_Inst_NumericOperationOnNextResult] = HHPL_Inst_Ret_Number;



// HHPL compare commands
	HHPLInstructionFlags[HHPL_Inst_CompareNumber] = HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_LogicalOperationOnNextTwoResults] = HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_LogicalOperationOnNextResult] = HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_LogicalConstant] = HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_CompareStringOperations] = HHPL_Inst_TwoByteInstruction;
	HHPLInstructionFlags[HHPL_Inst_ComparisonDefaults] = HHPL_Inst_TwoByteInstruction;




// Data formatting send (report) character commands
	HHPLInstructionFlags[HHPL_Inst_SendFromCursorToEndWithCharXXAtEnd] =
							HHPL_Inst_Ret_WideString |
							HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_SendNNCharsFromCursorWithCharXXAtEnd] =
							HHPL_Inst_Ret_WideString |
							HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_SendFromCursorToCharSSWithCharXXAtEnd] =
							HHPL_Inst_Ret_WideString |
							HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_SendXXCharNNTimes] = HHPL_Inst_Ret_NoResult;
	HHPLInstructionFlags[HHPL_Inst_SendAllButTheLastNNChars] =
							HHPL_Inst_Ret_WideString |
							HHPL_Inst_Ret_Boolean;


// Data formatting cursor movement commands
	HHPLInstructionFlags[HHPL_Inst_MoveCursorAheadNN] =
							HHPL_Inst_Ret_Number |
							HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_MoveCursorBackwardNN] =
							HHPL_Inst_Ret_Number |
							HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_MoveCursorToBeginning] = HHPL_Inst_Ret_Number;
	HHPLInstructionFlags[HHPL_Inst_MoveCursorToEnd] = HHPL_Inst_Ret_Number;
	HHPLInstructionFlags[HHPL_Inst_MoveCursorAheadToCharXX] =
							HHPL_Inst_Ret_Number |
							HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_MoveCursorBackwardToCharXX] =
							HHPL_Inst_Ret_Number |
							HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_MoveCursorAheadToFirstNonXXChar] =
							HHPL_Inst_Ret_Number |
							HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_MoveCursorBackwardToFirstNonXXChar] =
							HHPL_Inst_Ret_Number |
							HHPL_Inst_Ret_Boolean;


// Data formatting character suppression / replacement commands
	HHPLInstructionFlags[HHPL_Inst_SuppressTheNNCharsXX1ToXXNN] = HHPL_Inst_Ret_NoResult;
	HHPLInstructionFlags[HHPL_Inst_TurnOffCharSuppression] = HHPL_Inst_Ret_NoResult;
	HHPLInstructionFlags[HHPL_Inst_ReplaceTheNNCharsXX1ToXXNN] = HHPL_Inst_Ret_NoResult;
	HHPLInstructionFlags[HHPL_Inst_TurnOffCharReplacement] = HHPL_Inst_Ret_NoResult;


// Data formatting character comparison commands
	HHPLInstructionFlags[HHPL_Inst_CompareCharXXToCharAtCursor] = HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_CheckToSeeThatCharAtCursorIsNumeric] = HHPL_Inst_Ret_Boolean;
	HHPLInstructionFlags[HHPL_Inst_CheckToSeeThatCharAtCursorIsNonNumeric] = HHPL_Inst_Ret_Boolean;


// Data formatting toggle auxiliary port on / off
	HHPLInstructionFlags[HHPL_Inst_ToggleAuxiliaryPortOnAndOff] = HHPL_Inst_Ret_NoResult;

// Data formatting delay command
	HHPLInstructionFlags[HHPL_Inst_DataFormatDelay] = HHPL_Inst_Ret_NoResult;



// Load the sub-instruction flag pointers.  Zero it out first, so that only the instructions that
// have sub-instructions will have non-zero entries.
	for(i = 0;    i < HHPL_MaximumInstructions;    ++i)
	{
		HHPLSubInstructionFlagPointers[i] = NULL;
	}
	HHPLSubInstructionFlagPointers[HHPL_Inst_TimerOperations] = HHPLTimingOperationSubInstructionFlags;
	HHPLSubInstructionFlagPointers[HHPL_Inst_DisplayControlOperations] = HHPLDisplayControlSubInstructionFlags;
	HHPLSubInstructionFlagPointers[HHPL_Inst_DisplayOutputOperations] = HHPLDisplayOutputSubInstructionFlags;
	HHPLSubInstructionFlagPointers[HHPL_Inst_AlarmSignalingOperations] = HHPLAlarmSignalingSubInstructionFlags;
	HHPLSubInstructionFlagPointers[HHPL_Inst_InputDeviceOperations] = HHPLInputDeviceSubInstructionFlags;
	HHPLSubInstructionFlagPointers[HHPL_Inst_CommunicationsOperations] = HHPLCommunicationsSubInstructionFlags;
	HHPLSubInstructionFlagPointers[HHPL_Inst_SpecialOperations] = HHPLSpecialOperationSubInstructionFlags;
	HHPLSubInstructionFlagPointers[HHPL_Inst_PowerManagementOperations] = HHPLPowerManagementOperationSubInstructionFlags;
	HHPLSubInstructionFlagPointers[HHPL_Inst_OSConfigurationOperations] = HHPLOSConfigurationSubInstructionFlags;
	HHPLSubInstructionFlagPointers[HHPL_Inst_OSStatusOperations] = HHPLOSStatusSubInstructionFlags;
	HHPLSubInstructionFlagPointers[HHPL_Inst_RemoteHostOperations] = HHPLRemoteHostSubInstructionFlags;
	HHPLSubInstructionFlagPointers[HHPL_Inst_WideStringOperations] = HHPLWideStringConversionSubInstructionFlags;
	HHPLSubInstructionFlagPointers[HHPL_Inst_WideStringVariableOperations] = HHPLWideStringVariableSubInstructionFlags;
	HHPLSubInstructionFlagPointers[HHPL_Inst_PrefixSuffixDefault] = HHPLPrefixSuffixDefaultSubInstructionFlags;
	HHPLSubInstructionFlagPointers[HHPL_Inst_PrefixSuffix] = HHPLPrefixSuffixSubInstructionFlags;
	HHPLSubInstructionFlagPointers[HHPL_Inst_StringOperationOnNextTwoResults] = HHPLTwoOperandStringOperationSubInstructionFlags;
	HHPLSubInstructionFlagPointers[HHPL_Inst_StringOperationOnNextResult] = HHPLSingleOperandStringOperationSubInstructionFlags;
	HHPLSubInstructionFlagPointers[HHPL_Inst_CompareStringOperations] = HHPLStringComparisonSubInstructionFlags;
	HHPLSubInstructionFlagPointers[HHPL_Inst_ComparisonDefaults] = HHPLComparisonDefaultSubInstructionFlags;



// Minimal functionality to start with.
	FullEditingEnabled = false;

	return;
}  // Constructor





//
//	Destructor for HHPL_INSTRUCTION_SET
//		Free instruction memory.
//
HHPL_INSTRUCTION_SET::~HHPL_INSTRUCTION_SET()
{
	delete HHPLTimingOperationSubInstructionFlags;
	delete HHPLDisplayControlSubInstructionFlags;
	delete HHPLDisplayOutputSubInstructionFlags;
	delete HHPLAlarmSignalingSubInstructionFlags;
	delete HHPLInputDeviceSubInstructionFlags;
	delete HHPLCommunicationsSubInstructionFlags;
	delete HHPLSpecialOperationSubInstructionFlags;
	delete HHPLPowerManagementOperationSubInstructionFlags;
	delete HHPLOSConfigurationSubInstructionFlags;
	delete HHPLOSStatusSubInstructionFlags;
	delete HHPLRemoteHostSubInstructionFlags;
	delete HHPLWideStringConversionSubInstructionFlags;
	delete HHPLWideStringVariableSubInstructionFlags;
	delete HHPLPrefixSuffixDefaultSubInstructionFlags;
	delete HHPLPrefixSuffixSubInstructionFlags;
	delete HHPLTwoOperandStringOperationSubInstructionFlags;
	delete HHPLSingleOperandStringOperationSubInstructionFlags;
	delete HHPLStringComparisonSubInstructionFlags;
	delete HHPLComparisonDefaultSubInstructionFlags;
	delete HHPLInstructionFlags;
	delete HHPLSubInstructionFlagPointers;

    return;
}  // Destructor






// Turn on the full editing capability flag
void HHPL_INSTRUCTION_SET::EnableFullEditing(void)
{
    FullEditingEnabled = true;
}




// Turn off the full editing capability flag
void HHPL_INSTRUCTION_SET::DisableFullEditing(void)
{
    FullEditingEnabled = false;
}




// Tell caller whether or not full editing is enabled.
bool HHPL_INSTRUCTION_SET::FullEditingIsEnabled(void)
{
    return(FullEditingEnabled);
}




// Report a version string to the user.
void HHPL_INSTRUCTION_SET::HHPLVersion(HWSTRING *Version)
{
	Version->Copy(HHPL_VERSION);

	return;
} // HHPLVersion




