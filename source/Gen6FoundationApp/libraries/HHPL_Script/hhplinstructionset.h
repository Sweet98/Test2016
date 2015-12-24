//======================================================================================
// HHPLInstructionSet.h
//======================================================================================
// $RCSfile: hhplinstructionset.h $
// $Revision: 1.2 $
// $Date: 2011/11/11 17:01:42EST $
//======================================================================================

//======================================================================================



#ifndef __HHPL_INSTR_SET_H
#define __HHPL_INSTR_SET_H



#include	"HWSTRING.h"				// HWSTRING interface definition




class HHPL_INSTRUCTION_SET
{
public:

	#define		HHPL_INSTRUCTION_SET_VERSION				1

    #define     HHPL_DELAY_COMMAND_INTERVAL_IN_MS			10		// Delay interval = 10 ms
    #define     MAXIMUM_HHPL_DELAY_COMMAND_INTERVAL_IN_MS	60000	// 60 second maximum delay


	#define		VERSACODE_BYTES_IN_NUMBER					4


// Define maximum number of test routine parameters and the format of each string
// parameter.
	#define	MAXIMUM_NUMBER_OF_TEST_ROUTINE_PARAMETERS			7



// Minimum and maximum number of DisplayMenu items
	#define	MINIMUM_NUMBER_OF_DISPLAY_MENU_ITEMS				1
	#define	MAXIMUM_NUMBER_OF_DISPLAY_MENU_ITEMS				15


	typedef struct {		// Range variable type, consisting of a beginning, an end, and
		int		Begin;		//	a flag to indicate whether or not it is valid.
		int		End;
		bool	Valid;
	} RANGE;


	typedef struct {		// Prefix or suffix variable type, consisting of the prefix or suffix
		HWSTRING	*String;	//	string itself and flags that indicate whether the prefix or suffix
		bool		UseIfNoData;//	should be applied in the instances where a non-null and a null data
		bool		UseIfData;	//	stream is being reported.
	} PREFIX_SUFFIX;



//	Constructor for HHPL_INSTRUCTION_SET
	HHPL_INSTRUCTION_SET(void);


//	Destructor for HHPL_INSTRUCTION_SET
	~HHPL_INSTRUCTION_SET(void);



// Turn on the full editing capability flag
	void EnableFullEditing(void);

// Turn off the full editing capability flag
	void DisableFullEditing(void);

// Tell caller whether or not full editing is enabled.
	bool FullEditingIsEnabled(void);


// Report a version string to the user.
	void HHPLVersion(HWSTRING *Version);



protected:


	#define	HHPL_VERSION			L"1.14"


    bool		FullEditingEnabled;			// Are we allowing full HHPL capability or
											// restricting to the original data formatting.


// Default data source is HHP ID '9' for script; used for send operations when no data source ID
// has been explicitly defined, ensuring that the data is NOT edited.
	#define		DEFAULT_DATA_SOURCE_ID		L"9]Z9"


// The sub-instruction codes used by numeric operations with two operands are defined as follows:
	enum TWO_OPERAND_NUMERIC_OPERATION_SUB_INSTRUCTIONS {
	// Add numbers
		HHPL_NumberSubInst_Add=0,

	// Subtract numbers
		HHPL_NumberSubInst_Subtract,

	// Multiply numbers
		HHPL_NumberSubInst_Multiply,

	// Divide numbers
		HHPL_NumberSubInst_Divide,

	// Bitwise AND numbers
		HHPL_NumberSubInst_BitwiseAnd,

	// Bitwise OR numbers
		HHPL_NumberSubInst_BitwiseOr,

	// Bitwise XOR numbers
		HHPL_NumberSubInst_BitwiseXor
	};


// The sub-instruction codes used by numeric operations with one operand are defined as follows:
	enum ONE_OPERAND_NUMERIC_OPERATION_SUB_INSTRUCTIONS{
	// Unary +
		HHPL_NumberSubInst_Unary_Plus=0,

	// Unary -
		HHPL_NumberSubInst_Unary_Minus,

	// Unary ~
		HHPL_NumberSubInst_Unary_Complement
	};


// The sub-instruction codes used by logical operations with two operands are defined as follows:
	enum TWO_OPERAND_LOGICAL_OPERATION_SUB_INSTRUCTIONS {
	// And booleans
		HHPL_LogicalSubInst_And=0,

	// Or booleans
		HHPL_LogicalSubInst_Or
	};


// The sub-instruction codes used by logical operations with a single operand are defined as follows:
	enum ONE_OPERAND_LOGICAL_OPERATION_SUB_INSTRUCTIONS {
	// Negate booleans
		HHPL_LogicalSubInst_Not=0
	};


// The sub-instruction codes used by logical constants are defined as follows:
	enum LOGICAL_CONSTANT_SUB_INSTRUCTIONS {
	// True result
		HHPL_LogicalSubInst_True=0,

	// False result
		HHPL_LogicalSubInst_False
	};



// The sub-instruction codes used by comparison operations with two operands are defined as follows:
	enum TWO_OPERAND_COMPARISON_OPERATION_SUB_INSTRUCTIONS {
	// Compare equal
		HHPL_LogicalSubInst_Op1_EQ_Op2=0,

	// Compare less than or equal
		HHPL_LogicalSubInst_Op1_LE_Op2,

	// Compare greater than or equal
		HHPL_LogicalSubInst_Op1_GE_Op2,

	// Compare less than
		HHPL_LogicalSubInst_Op1_LT_Op2,

	// Compare greater than
		HHPL_LogicalSubInst_Op1_GT_Op2,

	// Compare not equal
		HHPL_LogicalSubInst_Op1_NE_Op2
	};



// HHPL find string sub-instruction flags
	enum FIND_STRING_SUB_INSTRUCTIONS {
		HHPL_FindStringSubInstFlags_WildcardSearch								= 0x01,
		HHPL_FindStringSubInstFlags_CaseSensitiveSearch							= 0x02,
		HHPL_FindStringSubInstFlags_UseDefaultCaseSensitivityForSearch			= 0x04,
		HHPL_FindStringSubInstFlags_SearchForward								= 0x08
	};



// HHPL font settings
	enum HHPL_FONT_SETTINGS {
		HHPL_FONT_VERYCLEAR8 = 0,
		HHPL_FONT_VERYCLEAR11,
		HHPL_FONT_VERYCLEAR16,
		HHPL_FONT_BOLD8,
		HHPL_FONT_BOLD11,
		HHPL_FONT_BOLD16
	};



// HHPL cursor control
	enum HHPL_DISPLAY_CURSOR_CONTROL_SETTINGS {
		HHPL_DISPLAY_CURSOR_OFF = 0,
		HHPL_DISPLAY_CURSOR_ON_STEADY,
		HHPL_DISPLAY_CURSOR_ON_BLINK
	};



// HHPL keypad control
	enum HHPL_KEYPAD_CONTROL_SETTINGS {
		HHPL_DO_NOT_WAIT_FOR_KEY_RELEASE = 0,
		HHPL_WAIT_FOR_KEY_RELEASE,
		HHPL_FORCE_FOR_KEY_RELEASE,
		HHPL_LOOK_AT_KEYPAD
	};



// HHPL keypad operations
	enum HHPL_KEYPAD_OPERATIONS {
		HHPL_KEYPAD_KEYCLICK_OFF = 0,
		HHPL_KEYPAD_KEYCLICK_ON,
		HHPL_KEYPAD_FLUSH
	};



// HHPL indicator control
	enum HHPL_INDICATOR_CONTROL_SETTINGS {
		HHPL_IndicatorControlSubInst_GoodSequence = 0,
		HHPL_IndicatorControlSubInst_BadSequence,
		HHPL_IndicatorControlSubInst_IndicatorLightOn,
		HHPL_IndicatorControlSubInst_IndicatorLightsOff
	};



// HHPL indicator colors
	enum HHPL_INDICATOR_COLORS {
		HHPL_INDICATOR_CANCEL = 0,
		HHPL_INDICATOR_RED,
		HHPL_INDICATOR_GREEN,
		HHPL_INDICATOR_YELLOW
	};



// HHPL send to host flags
	enum HHPL_SEND_TO_HOST_FLAGS {
		HHPL_SEND_TO_HOST_DEFAULT									= 0x0000,
		HHPL_SEND_TO_HOST_NO_INDICATORS								= 0x0001,
		HHPL_SEND_TO_HOST_RUN_THROUGH_FORMATTER_BEFORE_OUTPUTTING	= 0x0002,
		HHPL_SEND_TO_HOST_USE_MICROSECOND_DELAY						= 0x0004,
		HHPL_SEND_TO_HOST_USE_INDICATORS							= 0x0008
	};



// HHPL input operation modifiers
	enum HHPL_INPUT_OPERATION_MODIFIERS {
		HHPL_INPUT_MOD_ACCEPT_HOST_INPUT			= 0x0001,
		HHPL_INPUT_MOD_ACCEPT_KEYED_INPUT			= 0x0002,
		HHPL_INPUT_MOD_ACCEPT_BARCODE_INPUT			= 0x0004,
		HHPL_INPUT_MOD_TURN_OFF_INPUT_SOURCES_MASK =
					~(HHPL_INPUT_MOD_ACCEPT_HOST_INPUT		|
						HHPL_INPUT_MOD_ACCEPT_KEYED_INPUT	|
						HHPL_INPUT_MOD_ACCEPT_BARCODE_INPUT),
		HHPL_INPUT_MOD_TERMINATE_ON_NO_DECODE		= 0x0008,
		HHPL_INPUT_MOD_TURN_CURSOR_OFF				= 0x0010,
		HHPL_INPUT_MOD_USE_DEFAULT_VALUE			= 0x0020,
		HHPL_INPUT_MOD_SILENT_INPUT					= 0x0040,
		HHPL_INPUT_MOD_REFRESH_DISPLAY				= 0x0080,
		HHPL_INPUT_MOD_USE_ARROW_KEYS_ON_NUMBER		= 0x0100,
		HHPL_INPUT_MOD_PRESERVE_BUFFER				= 0x0200,
		HHPL_INPUT_MOD_PRESERVE_FLUSH_NOT_SPECIFIED	= 0x0400,
		HHPL_INPUT_MOD_WAIT_FOR_CHARACTER			= 0x0800,
		HHPL_INPUT_MOD_NO_BEEP						= 0x1000
	};

	#define	HHPL_INPUT_MAXIMUM_QUIT_KEYS			256



public:
// HHPL input / send to host flags
	enum HHPL_INPUT_AND_SEND_TO_HOST_FLAGS {
		HHPL_INPUT_AND_SEND_TO_HOST_DEFAULT = 0,
		HHPL_INPUT_AND_SEND_TO_HOST_NO_INDICATORS	= 0x01,
		HHPL_INPUT_AND_SEND_TO_HOST_ACK_REQUIRED	= 0x02
	};



protected:
// HHPL check / wait for host command modifiers
	enum HHPL_HOST_COMMAND_EXECUTE_MODIFIERS {
		HHPL_HOST_COMMAND_EXECUTE_NO_ROUTINES			= 0x0000,
		HHPL_HOST_COMMAND_EXECUTE_ONLY_HOST_ROUTINES	= 0x0001,
		HHPL_HOST_COMMAND_EXECUTE_ANY_ROUTINES			= 0x0002
	};


// The sub-instruction codes used by timing operations
	enum HHPL_TIMER_OPERATIONS {
		HHPL_TimerOperationSubInst_ResetTimer0 = 0,
		HHPL_TimerOperationSubInst_GetTimer0,
		HHPL_TimerOperationSubInst_DelayMilliseconds,
		HHPL_TimerOperationSubInst_NumberOfInstructions
	};


// The sub-instruction codes used by display control operations
	enum HHPL_DISPLAY_CONTROL_OPERATIONS {
		HHPL_DisplayControlSubInst_DisplayBacklightOn = 0,
		HHPL_DisplayControlSubInst_DisplayBacklightOff,
		HHPL_DisplayControlSubInst_DisplayOn,
		HHPL_DisplayControlSubInst_DisplayOff,
		HHPL_DisplayControlSubInst_EraseDisplay,
		HHPL_DisplayControlSubInst_SetFont,
		HHPL_DisplayControlSubInst_RestorePreservedDisplay,
		HHPL_DisplayControlSubInst_PreserveCurrentDisplay,
		HHPL_DisplayControlSubInst_MoveDisplayCursor,
		HHPL_DisplayControlSubInst_DisplayHeight,
		HHPL_DisplayControlSubInst_DisplayWidth,
		HHPL_DisplayControlSubInst_CharacterHeight,
		HHPL_DisplayControlSubInst_CharacterWidth,
		HHPL_DisplayControlSubInst_ScrollDisplay,
		HHPL_DisplayControlSubInst_TemporarilyEnableBacklight,
		HHPL_DisplayControlSubInst_TemporarilyEnableDisplay,
		HHPL_DisplayControlSubInst_EraseLine,
		HHPL_DisplayControlSubInst_GetFont,
		HHPL_DisplayControlSubInst_DisplayCursorControl,
		HHPL_DisplayControlSubInst_NumberOfInstructions
	};


// The sub-instruction codes used by display output operations
	enum HHPL_DISPLAY_OUTPUT_OPERATIONS {
		HHPL_DisplayOutputSubInst_LoadImage = 0,
		HHPL_DisplayOutputSubInst_OutputString,
		HHPL_DisplayOutputSubInst_NumberOfInstructions
	};


// The sub-instruction codes used by alarm and signaling operations
	enum HHPL_ALARM_SIGNALING_OPERATIONS {
		HHPL_AlarmSignalingSubInst_Beep = 0,
		HHPL_AlarmSignalingSubInst_IndicatorControl,
		HHPL_AlarmSignalingSubInst_IssueWarning,
		HHPL_AlarmSignalingSubInst_IssueError,
		HHPL_AlarmSignalingSubInst_IssueTerminate,
		HHPL_AlarmSignalingSubInst_NumberOfInstructions
	};


// The sub-instruction codes used by input device operations
	enum HHPL_INPUT_DEVICE_OPERATIONS {
		HHPL_InputDeviceSubInst_InputCharacter = 0,
		HHPL_InputDeviceSubInst_TriggerHit,
		HHPL_InputDeviceSubInst_InputNumber,
		HHPL_InputDeviceSubInst_InputString,
		HHPL_InputDeviceSubInst_KeyHit,
		HHPL_InputDeviceSubInst_KeyOperations,
		HHPL_InputDeviceSubInst_InputStringWithControlFlags,
		HHPL_InputDeviceSubInst_NumberOfInstructions
	};


// The sub-instruction codes used by communications operations
	enum HHPL_COMMUNICATIONS_OPERATIONS {
		HHPL_CommunicationsSubInst_Send = 0,
		HHPL_CommunicationsSubInst_RadioSignalStrength,
		HHPL_CommunicationsSubInst_RadioTransmissionFailures,
		HHPL_CommunicationsSubInst_SendWithControlFlags,
		HHPL_CommunicationsSubInst_NumberOfInstructions
	};


// The sub-instruction codes used by special operations
	enum HHPL_SPECIAL_OPERATIONS {
		HHPL_SpecialOperationSubInst_DisplayMenu = 0,
		HHPL_SpecialOperationSubInst_NumberOfInstructions
	};


// The sub-instruction codes used by power management operations
	enum HHPL_POWER_MANAGEMENT_OPERATIONS {
		HHPL_PowerManagementSubInst_BatteryStrength = 0,
		HHPL_PowerManagementSubInst_NumberOfInstructions
	};


// The sub-instruction codes used by O/S configuration operations
	enum HHPL_OS_CONFIGURATION_OPERATIONS {
		HHPL_OSConfigurationSubInst_MenuCommand = 0,
		HHPL_OSConfigurationSubInst_DisplayLanguage,
		HHPL_OSConfigurationSubInst_InterpreterVersion,
		HHPL_OSConfigurationSubInst_ScannerID,
		HHPL_OSConfigurationSubInst_Workgroup,
		HHPL_OSConfigurationSubInst_TerminalID,
		HHPL_OSConfigurationSubInst_GetEditBufferControlFlags,
		HHPL_OSConfigurationSubInst_SetEditBufferControlFlags,
		HHPL_OSConfigurationSubInst_NumberOfInstructions
	};


// The sub-instruction codes used by O/S status operations
	enum HHPL_OS_STATUS_OPERATIONS {
		HHPL_OSStatusSubInst_LastOperation = 0,
		HHPL_OSStatusSubInst_NumberOfInstructions
	};


// The sub-instruction codes used by remote host operations
	enum HHPL_REMOTE_HOST_OPERATIONS {
		HHPL_RemoteHostSubInst_WaitForCommand = 0,
		HHPL_RemoteHostSubInst_WaitForMultipleCommands,
		HHPL_RemoteHostSubInst_CheckForCommand,
		HHPL_RemoteHostSubInst_NumberOfInstructions
	};


// The sub-instruction codes used by wide string conversion operations
	enum HHPL_WIDE_STRING_OPERATIONS {
		HHPL_WideStringOperationsSubInst_WideStringToBool = 0,
		HHPL_WideStringOperationsSubInst_WideStringToInt,
		HHPL_WideStringOperationsSubInst_WideStringToString,
		HHPL_WideStringOperationsSubInst_IntToWideString,
		HHPL_WideStringOperationsSubInst_BoolToWideString,
		HHPL_WideStringOperationsSubInst_StringToWideString,
		HHPL_WideStringOperationsSubInst_LengthWideString,
		HHPL_WideStringOperationsSubInst_SetWideStringLength,
		HHPL_WideStringOperationsSubInst_SetWideStringArrayLength,
		HHPL_WideStringOperationsSubInst_WideCharToInt,
		HHPL_WideStringOperationsSubInst_IntToWideChar,
		HHPL_WideStringOperationsSubInst_WideCharToChar,
		HHPL_WideStringOperationsSubInst_CharToWideChar,
		HHPL_WideStringOperationsSubInst_NumberOfInstructions
	};


// The sub-instruction codes used by wide string variable operations
	enum HHPL_WIDE_STRING_VARIABLE_OPERATIONS {
		HHPL_WideStringVariableOperationsSubInst_SetWideStringVariable = 0,
		HHPL_WideStringVariableOperationsSubInst_SetWideStringArrayVariable,
		HHPL_WideStringVariableOperationsSubInst_GetWideStringVariable,
		HHPL_WideStringVariableOperationsSubInst_GetWideStringArrayVariable,
		HHPL_WideStringVariableOperationsSubInst_WideStringDataInRange,
		HHPL_WideStringVariableOperationsSubInst_WideStringSetArrayDataInRange,
		HHPL_WideStringVariableOperationsSubInst_WideStringArrayDataInRange,
		HHPL_WideStringVariableOperationsSubInst_WideStringSetDataInRange,
		HHPL_WideStringVariableOperationsSubInst_WideStringRegister,
		HHPL_WideStringVariableOperationsSubInst_NumberOfInstructions
	};


// The sub-instruction codes used by prefix / suffix default operations
	enum HHPL_PREFIX_SUFFIX_DEFAULT_OPERATIONS {
		HHPL_PrefixSuffixDefaultSubInst_DefineDefaultPrefixToBeAlwaysApplied = 0,
		HHPL_PrefixSuffixDefaultSubInst_DefineDefaultPrefixToBeAppliedOnNonNullData,
		HHPL_PrefixSuffixDefaultSubInst_DefineDefaultPrefixToBeAppliedOnNullData,
		HHPL_PrefixSuffixDefaultSubInst_DefineDefaultSuffixToBeAlwaysApplied,
		HHPL_PrefixSuffixDefaultSubInst_DefineDefaultSuffixToBeAppliedOnNonNullData,
		HHPL_PrefixSuffixDefaultSubInst_DefineDefaultSuffixToBeAppliedOnNullData,
		HHPL_PrefixSuffixDefaultSubInst_NumberOfInstructions
	};


// The sub-instruction codes used by prefix / suffix operations
	enum HHPL_PREFIX_SUFFIX_OPERATIONS {
		HHPL_PrefixSuffixSubInst_UseDefaultPrefix = 0,
		HHPL_PrefixSuffixSubInst_UseDefaultSuffix,
		HHPL_PrefixSuffixSubInst_UseImmediatePrefixAlways,
		HHPL_PrefixSuffixSubInst_UseImmediatePrefixOnNonNullData,
		HHPL_PrefixSuffixSubInst_UseImmediatePrefixOnNullData,
		HHPL_PrefixSuffixSubInst_UseImmediateSuffixAlways,
		HHPL_PrefixSuffixSubInst_UseImmediateSuffixOnNonNullData,
		HHPL_PrefixSuffixSubInst_UseImmediateSuffixOnNullData,
		HHPL_PrefixSuffixSubInst_NumberOfInstructions
	};



// The sub-instruction codes used by string operations on two results
	enum HHPL_STRING_OPERATION_ON_NEXT_TWO_RESULTS {
		HHPL_TwoOperandStringOperationSubInst_ConcatenateNextTwoResults = 0,
		HHPL_TwoOperandStringOperationSubInst_ConcatenateNextTwoWideResults,
		HHPL_TwoOperandStringOperationSubInst_NumberOfInstructions
	};


// The sub-instruction codes used by string operations on two results
	enum HHPL_STRING_OPERATION_ON_NEXT_RESULT {
		HHPL_SingleOperandStringOperationSubInst_ConvertStringToUpperCase = 0,
		HHPL_SingleOperandStringOperationSubInst_ConvertStringToLowerCase,
		HHPL_SingleOperandStringOperationSubInst_ConvertWideStringToUpperCase,
		HHPL_SingleOperandStringOperationSubInst_ConvertWideStringToLowerCase,
		HHPL_SingleOperandStringOperationSubInst_NumberOfInstructions
	};


// The sub-instruction codes used by string comparison operations
	enum HHPL_STRING_COMPARISON_OPERATIONS {
		HHPL_StringComparisonSubInst_CaseSensitiveCompareString = 0,
		HHPL_StringComparisonSubInst_CaseInsensitiveCompareString,
		HHPL_StringComparisonSubInst_CompareStringAccordingToCaseSensitivitySetting,
		HHPL_StringComparisonSubInst_WildcardCaseSensitiveCompareString,
		HHPL_StringComparisonSubInst_WildcardCaseInsensitiveCompareString,
		HHPL_StringComparisonSubInst_WildcardCompareStringAccordingToCaseSensitivitySetting,
		HHPL_StringComparisonSubInst_CaseSensitiveCompareWideString,
		HHPL_StringComparisonSubInst_CaseInsensitiveCompareWideString,
		HHPL_StringComparisonSubInst_CompareWideStringAccordingToCaseSensitivitySetting,
		HHPL_StringComparisonSubInst_WildcardCaseSensitiveCompareWideString,
		HHPL_StringComparisonSubInst_WildcardCaseInsensitiveCompareWideString,
		HHPL_StringComparisonSubInst_WildcardCompareWideStringAccordingToCaseSensitivitySetting,
		HHPL_StringComparisonSubInst_NumberOfInstructions
	};


// The sub-instruction codes used by comparison default operations
	enum HHPL_COMPARISON_DEFAULTS_OPERATIONS {
		HHPL_ComparisonDefaultsSubInst_ComparesAreCaseSensitiveByDefault = 0,
		HHPL_ComparisonDefaultsSubInst_ComparesAreCaseInsensitiveByDefault,
		HHPL_ComparisonDefaultsSubInst_NumberOfInstructions
	};




// The instruction codes used by HHPL are defined as follows:
	enum HHPL_INSTRUCTIONS {
	// NoOp instruction used by HHPL
		HHPL_Inst_NoOp															= 0x00,


	// HHPL I/O & signaling commands
		HHPL_Inst_TimerOperations												= 0x10,
		HHPL_Inst_DisplayControlOperations										= 0x11,
		HHPL_Inst_DisplayOutputOperations										= 0x12,
		HHPL_Inst_AlarmSignalingOperations										= 0x13,
		HHPL_Inst_InputDeviceOperations											= 0x14,
		HHPL_Inst_CommunicationsOperations										= 0x15,
		HHPL_Inst_SpecialOperations												= 0x16,
		HHPL_Inst_PowerManagementOperations										= 0x17,


	// HHPL O/S and interpreter service instructions
		HHPL_Inst_OSConfigurationOperations										= 0x30,
		HHPL_Inst_OSStatusOperations											= 0x31,


	// Constants and NULLs
		HHPL_Inst_NullInteger													= 0x40,
		HHPL_Inst_NullString													= 0x41,
		HHPL_Inst_NullPrefix													= 0x42,
		HHPL_Inst_NullSuffix													= 0x43,
		HHPL_Inst_NullWideString												= 0x44,


	// HHPL control transfer commands
		HHPL_Inst_IfCommand														= 0x50,
		HHPL_Inst_WhileCommand													= 0x51,
		HHPL_Inst_DoWhileCommand												= 0x52,
		HHPL_Inst_ExecuteSubroutine												= 0x53,
		HHPL_Inst_Exit															= 0x54,
		HHPL_Inst_ForeverDoCommand												= 0x55,
		HHPL_Inst_TestRoutine													= 0x56,
		HHPL_Inst_RemoteHostOperations											= 0x57,
		HHPL_Inst_GoToCommand													= 0x58,


	// Type conversion / string operations
		HHPL_Inst_StringToBool													= 0x60,
		HHPL_Inst_StringToInt													= 0x61,
		HHPL_Inst_IntToBool														= 0x62,
		HHPL_Inst_IntToString													= 0x63,
		HHPL_Inst_BoolToInt														= 0x64,
		HHPL_Inst_BoolToString													= 0x65,
		HHPL_Inst_LengthString													= 0x66,
		HHPL_Inst_Format														= 0x67,
		HHPL_Inst_SetStringLength												= 0x68,
		HHPL_Inst_SetStringArrayLength											= 0x69,
		HHPL_Inst_CharToInt														= 0x6A,
		HHPL_Inst_IntToChar														= 0x6B,

	// HHPL wide string operations
		HHPL_Inst_WideStringOperations											= 0x6E,

	// HHPL find string commands
		HHPL_Inst_FindString													= 0x6F,


	// HHPL variable instructions
		HHPL_Inst_SetBooleanVariable											= 0x70,
		HHPL_Inst_SetBooleanArrayVariable										= 0x71,
		HHPL_Inst_SetIntegerVariable											= 0x72,
		HHPL_Inst_SetIntegerArrayVariable										= 0x73,
		HHPL_Inst_SetStringVariable												= 0x74,
		HHPL_Inst_SetStringArrayVariable										= 0x75,
		HHPL_Inst_GetBooleanVariable											= 0x76,
		HHPL_Inst_GetBooleanArrayVariable										= 0x77,
		HHPL_Inst_GetIntegerVariable											= 0x78,
		HHPL_Inst_GetIntegerArrayVariable										= 0x79,
		HHPL_Inst_GetStringVariable												= 0x7A,
		HHPL_Inst_GetStringArrayVariable										= 0x7B,
		HHPL_Inst_DataInRange													= 0x7C,
		HHPL_Inst_DataInEditBufferRange											= 0x7D,
		HHPL_Inst_DataInHostRange												= 0x7E,
		HHPL_Inst_SetArrayDataInRange											= 0x7F,
		HHPL_Inst_ArrayDataInRange												= 0x80,
		HHPL_Inst_SetDataInRange												= 0x81,
		HHPL_Inst_SetEditCursor													= 0x82,
		HHPL_Inst_GetEditCursor													= 0x83,
		HHPL_Inst_SetEditBuffer													= 0x84,
		HHPL_Inst_GetEditBuffer													= 0x85,
		HHPL_Inst_SetRange														= 0x86,
		HHPL_Inst_SendHost														= 0x87,
		HHPL_Inst_GetHost														= 0x88,
		HHPL_Inst_BooleanRegister												= 0x89,
		HHPL_Inst_IntegerRegister												= 0x8A,
		HHPL_Inst_StringRegister												= 0x8B,
		HHPL_Inst_PrefixSuffixDefault											= 0x8C,
		HHPL_Inst_PrefixSuffix													= 0x8D,
		HHPL_Inst_SetDataSourceID												= 0x8E,
		HHPL_Inst_GetDataSourceID												= 0x8F,
		HHPL_Inst_DataInDataSourceIDRange										= 0x90,
		HHPL_Inst_WideStringVariableOperations									= 0x91,


	// HHPL string commands
		HHPL_Inst_UCharLengthString												= 0xA0,
		HHPL_Inst_UShortLengthString											= 0xA1,
		HHPL_Inst_StringOperationOnNextTwoResults								= 0xA2,
		HHPL_Inst_StringOperationOnNextResult									= 0xA3,
		HHPL_Inst_UCharLengthWideString											= 0xA4,
		HHPL_Inst_UShortLengthWideString										= 0xA5,


	// HHPL number commands
		HHPL_Inst_Int8															= 0xAA,
		HHPL_Inst_Int16															= 0xAB,
		HHPL_Inst_Int32															= 0xAC,
		HHPL_Inst_NumericOperationOnNextTwoResults								= 0xAD,
		HHPL_Inst_NumericOperationOnNextResult									= 0xAE,


	// HHPL logical / comparison commands
		HHPL_Inst_CompareNumber													= 0xD0,
		HHPL_Inst_LogicalOperationOnNextTwoResults								= 0xD1,
		HHPL_Inst_LogicalOperationOnNextResult									= 0xD2,
		HHPL_Inst_LogicalConstant												= 0xD3,
		HHPL_Inst_CompareStringOperations										= 0xD4,
		HHPL_Inst_ComparisonDefaults											= 0xD5,


	// Data formatting send (report) character commands
		HHPL_Inst_SendFromCursorToEndWithCharXXAtEnd							= 0xF1,
		HHPL_Inst_SendNNCharsFromCursorWithCharXXAtEnd							= 0xF2,
		HHPL_Inst_SendFromCursorToCharSSWithCharXXAtEnd							= 0xF3,
		HHPL_Inst_SendXXCharNNTimes												= 0xF4,
		HHPL_Inst_SendAllButTheLastNNChars										= 0xE9,

	// Data formatting cursor movement commands
		HHPL_Inst_MoveCursorAheadNN												= 0xF5,
		HHPL_Inst_MoveCursorBackwardNN											= 0xF6,
		HHPL_Inst_MoveCursorToBeginning											= 0xF7,
		HHPL_Inst_MoveCursorToEnd												= 0xEA,
		HHPL_Inst_MoveCursorAheadToCharXX										= 0xF8,
		HHPL_Inst_MoveCursorBackwardToCharXX									= 0xF9,
		HHPL_Inst_MoveCursorAheadToFirstNonXXChar								= 0xE6,
		HHPL_Inst_MoveCursorBackwardToFirstNonXXChar							= 0xE7,

	// Data formatting character suppression / replacement commands
		HHPL_Inst_SuppressTheNNCharsXX1ToXXNN									= 0xFB,
		HHPL_Inst_TurnOffCharSuppression										= 0xFC,
		HHPL_Inst_ReplaceTheNNCharsXX1ToXXNN									= 0xE4,
		HHPL_Inst_TurnOffCharReplacement										= 0xE5,

	// Data formatting character comparison commands
		HHPL_Inst_CompareCharXXToCharAtCursor									= 0xFE,
		HHPL_Inst_CheckToSeeThatCharAtCursorIsNumeric							= 0xEC,
		HHPL_Inst_CheckToSeeThatCharAtCursorIsNonNumeric						= 0xED,

	// Data formatting toggle auxiliary port on / off
		HHPL_Inst_ToggleAuxiliaryPortOnAndOff									= 0xEB,

	// Data formatting delay command
		HHPL_Inst_DataFormatDelay												= 0xEF,


	// Maximum number of instruction codes is 256, since the codes are one byte in length
		HHPL_MaximumInstructions												= 256

	};



// The following is a list of possible instruction results.
	enum HHPL_INSTRUCTION_FLAGS {
		HHPL_Inst_Ret_NoResult = 0,				// Instruction returns no value
		HHPL_Inst_Ret_Boolean = 0x01,			// Instruction returns a boolean
		HHPL_Inst_Ret_String = 0x02,			// Instruction returns a string
		HHPL_Inst_Ret_Range = 0x04,				// Instruction returns a beginning and ending
												//	cursor position for a range of characters
		HHPL_Inst_Ret_Prefix = 0x08,			// Instruction returns a prefix
		HHPL_Inst_Ret_Suffix = 0x10,			// Instruction returns a suffix
		HHPL_Inst_Ret_Number = 0x20,			// Instruction returns a numeric value
		HHPL_Inst_Set_Boolean= 0x40,			// Instruction sets a boolean variable or intrinsic
		HHPL_Inst_Set_Integer= 0x80,			// Instruction sets an integer variable or intrinsic
		HHPL_Inst_Set_String = 0x100,			// Instruction sets a string variable or intrinsic
		HHPL_Inst_TwoByteInstruction = 0x200,	// Instruction code is 2 bytes long
		HHPL_Inst_Ret_WideString = 0x0400,		// Instruction returns a wide string
		HHPL_Inst_Set_WideString = 0x0800		// Instruction sets a wide string variable or intrinsic
	};


// Run time copy of definitions for each timing operation sub-instruction.
	unsigned int	*HHPLTimingOperationSubInstructionFlags;


// Run time copy of definitions for each display control sub-instruction.
	unsigned int	*HHPLDisplayControlSubInstructionFlags;


// Run time copy of definitions for each display output sub-instruction.
	unsigned int	*HHPLDisplayOutputSubInstructionFlags;


// Run time copy of definitions for each alarm and signaling sub-instruction.
	unsigned int	*HHPLAlarmSignalingSubInstructionFlags;


// Run time copy of definitions for each input device sub-instruction.
	unsigned int	*HHPLInputDeviceSubInstructionFlags;


// Run time copy of definitions for each communications sub-instruction.
	unsigned int	*HHPLCommunicationsSubInstructionFlags;


// Run time copy of definitions for each special operation sub-instruction.
	unsigned int	*HHPLSpecialOperationSubInstructionFlags;


// Run time copy of definitions for each power management sub-instruction.
	unsigned int	*HHPLPowerManagementOperationSubInstructionFlags;


// Run time copy of definitions for each O/S configuration sub-instruction.
	unsigned int	*HHPLOSConfigurationSubInstructionFlags;


// Run time copy of definitions for each O/S status sub-instruction.
	unsigned int	*HHPLOSStatusSubInstructionFlags;


// Run time copy of definitions for each remote host sub-instruction.
	unsigned int	*HHPLRemoteHostSubInstructionFlags;


// Run time copy of definitions for each wide string conversion sub-instruction.
	unsigned int	*HHPLWideStringConversionSubInstructionFlags;


// Run time copy of definitions for each wide string variable sub-instruction.
	unsigned int	*HHPLWideStringVariableSubInstructionFlags;


// Run time copy of definitions for each prefix suffix default sub-instruction.
	unsigned int	*HHPLPrefixSuffixDefaultSubInstructionFlags;


// Run time copy of definitions for each prefix suffix sub-instruction.
	unsigned int	*HHPLPrefixSuffixSubInstructionFlags;


// Run time copy of definitions for each two operand string operation sub-instruction.
	unsigned int	*HHPLTwoOperandStringOperationSubInstructionFlags;


// Run time copy of definitions for each single operand string operation sub-instruction.
	unsigned int	*HHPLSingleOperandStringOperationSubInstructionFlags;


// Run time copy of definitions for each string comparison sub-instruction.
	unsigned int	*HHPLStringComparisonSubInstructionFlags;


// Run time copy of definitions for each comparison default sub-instruction.
	unsigned int	*HHPLComparisonDefaultSubInstructionFlags;


// Run time copy of definitions for each instruction.
	unsigned int	*HHPLInstructionFlags;


// Run time copy of pointers to sub-instruction definitions for each instruction.
	unsigned int	**HHPLSubInstructionFlagPointers;

private:
	HHPL_INSTRUCTION_SET (const HHPL_INSTRUCTION_SET&);            // declared private, but not defined, to prevent copying         
	HHPL_INSTRUCTION_SET& operator=(const HHPL_INSTRUCTION_SET&);  // declared private, but not defined, to prevent copying 



};
#endif      //	__HHPL_INSTR_SET_H


