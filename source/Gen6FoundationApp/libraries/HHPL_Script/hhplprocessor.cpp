/*

Check name declarations against keyword list

*/
//======================================================================================
// HHPLProcessor.cpp
//======================================================================================
// $RCSfile: hhplprocessor.cpp $
// $Revision: 1.3 $
// $Date: 2009/11/30 21:31:53EST $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Class Name:     HHPL_PROCESSOR
//
//  Class Description:
//		HHPL_PROCESSOR contains the methods for executing an HHPL program, including
//		all of its' subroutines.  It is derived from HHPL_INSTRUCTION_SET, which defines each
//		instruction and its' associated behavior.
//
//		To prepare the processor, instantiate this class.  When no more processing is to be done,
//		or the program of which this HHPL processor is a part is being destroyed, call the
//		destructor to free memory.
//
//		There are several means of communicating status of HHPL execution to the instantiator.
//		A return status of false indicates that there was a problem with the parsing operation;
//		ErrorMessage contains a description of what the HHPL code processor found to be
//		wrong.
//
//		Please see the HHPL_OUTPUT and HHPL_INSTRUCTION_SET classes for more information.
//
//---------------------------------------------------------------------------







//**************************************************************************************************
//
//	EXTERNAL INTERFACE DESCRIPTION
//
//	The constructor allocates space for the error message string and for the resulting output of
//	a processor run.  The error message string is made available to the outside, so that the result of
//	a processing operation may be queried via this message string.
//
//	The destructor frees space allocated to the output.
//
//	ProcessHHPLProgram is the routine to be used to actually execute the code.
//
//**************************************************************************************************






#include	"HHPLProcessor.h"	// HHPL processor definitions

#include	<ctype.h>			// data type conversion functions
#include	<string.h>			// 'C' string functions
#include	<stdio.h>			// Standard I/O
#include	<stdlib.h>

#include "ioman.h"
#include "readerconfiguration.h"
#include "menu.h"
#include "db_chrio.h"






//
//	Constructor for HHPL_PROCESSOR
//		Allocate storage for error message string.  The error message string can be accessed from the
//		outside to see if an error occurred during an HHPL run.
//
HHPL_PROCESSOR::HHPL_PROCESSOR(void)
	    : HHPL_INSTRUCTION_SET()

{
// Allocate error message
	TempStr = new char[160];
	wchTempStr = new wchar_t[160];

	HHPLInterpreterState = HHPL_INTERPRETER_IDLE;

	NameOfHostRoutineThatWasExecuted = new HWSTRING(100, true);

    return;
}  // Constructor





//
//	Destructor for HHPL_PROCESSOR
//		Free error message memory.
//
HHPL_PROCESSOR::~HHPL_PROCESSOR()
{
	delete TempStr;
	delete wchTempStr;

	delete NameOfHostRoutineThatWasExecuted;

	return;
}  // Destructor






//
//	HHPLInterpreterIdle
//		Return true if the HHPL interpreter is in an idle state.
bool HHPL_PROCESSOR::HHPLInterpreterIdle(void)
{
	return(HHPLInterpreterState == HHPL_INTERPRETER_IDLE);
} // HHPLInterpreterIdle





//
//	HHPLInterpreterBusy
//		Return true if the HHPL interpreter is in a busy state.
bool HHPL_PROCESSOR::HHPLInterpreterBusy(void)
{
	return(HHPLInterpreterState == HHPL_INTERPRETER_BUSY);
} // HHPLInterpreterBusy





//
//	HHPLInterpreterSuspended
//		Return true if the HHPL interpreter is in a suspended state.
bool HHPL_PROCESSOR::HHPLInterpreterSuspended(void)
{
	return(HHPLInterpreterState == HHPL_INTERPRETER_SUSPENDED);
} // HHPLInterpreterSuspended







//
//	HHPLProgramTerminated
//		Check the termination flag for the HHPL program currently being executed.
//
bool HHPL_PROCESSOR::HHPLProgramTerminated(void)
{
	return(TerminateHHPLProgramAtNextInstruction);
} // HHPLProgramTerminated







//
//	ExecuteHostRoutine
//		Find the routine name, send via a host Execute command, in the symbol table and execute it.
//
bool HHPL_PROCESSOR::ExecuteHostRoutine(wchar_t *RoutineName, int RoutineNameLength, bool UseEditBufferContents,
					wchar_t *EditBufferContents, int EditBufferContentsLength)
{
	bool	NameMatchFound = false;				// Found the requested routine in the symbol table.
	int		LocationOfRoutine=0;					// Location of routine in code.
	int		i;



// Look for a match in the table loaded by the host command Check or Wait instruction.
	i = 0;
	while((i < NumberOfHostAccessibleRoutines) &&
				(HostRoutineNames[i]->Compare(RoutineName, RoutineNameLength, false) != 0))
	{
		++i;
	}


// If we found a match in the HostRoutineNames table, we now need to find it in the code image symbol table.
	if(i < NumberOfHostAccessibleRoutines)
	{
		i = 0;
		while((i < ((int) HostRoutineProgramProfile->ParsedHHPLCodeImage->SymbolTable->NumberOfTableEntries)) &&
				((HostRoutineProgramProfile->ParsedHHPLCodeImage->SymbolTable->TableEntries[i].Name->Compare
											(RoutineName, RoutineNameLength, false)) ||
				 (!(HostRoutineProgramProfile->ParsedHHPLCodeImage->SymbolTable->TableEntries[i].Type &
														HHPL_ITEM_TABLE::ROUTINE_ITEM))))
		{
			++i;
		}


	// If we found a match in the symbol table, store the location and set the match flag
		if(i < ((int) HostRoutineProgramProfile->ParsedHHPLCodeImage->SymbolTable->NumberOfTableEntries))
		{
			NameOfHostRoutineThatWasExecuted->Copy
								(HostRoutineProgramProfile->ParsedHHPLCodeImage->SymbolTable->TableEntries[i].Name);

			LocationOfRoutine = HostRoutineProgramProfile->ParsedHHPLCodeImage->SymbolTable->TableEntries[i].Location;
			NameMatchFound = true;
		}
	}



// If we still haven't found a match, check to see if any routine is allowed or if a host routine is allowed via
// the control mask
	if(!NameMatchFound)
	{
	// If any routine may be executed, search the code image symbol table for all routines.
		if(HostRoutineControlMask & HHPL_HOST_COMMAND_EXECUTE_ANY_ROUTINES)
		{
			i = 0;
			while((i < ((int) HostRoutineProgramProfile->ParsedHHPLCodeImage->SymbolTable->NumberOfTableEntries)) &&
					((HostRoutineProgramProfile->ParsedHHPLCodeImage->SymbolTable->TableEntries[i].Name->Compare
											(RoutineName, RoutineNameLength, false)) ||
					 (!(HostRoutineProgramProfile->ParsedHHPLCodeImage->SymbolTable->TableEntries[i].Type &
														HHPL_ITEM_TABLE::ROUTINE_ITEM))))
			{
				++i;
			}


		// If we found a match in the symbol table, store the location and set the match flag
			if(i < ((int) HostRoutineProgramProfile->ParsedHHPLCodeImage->SymbolTable->NumberOfTableEntries))
			{
				NameOfHostRoutineThatWasExecuted->Copy
								(HostRoutineProgramProfile->ParsedHHPLCodeImage->SymbolTable->TableEntries[i].Name);

				LocationOfRoutine = HostRoutineProgramProfile->ParsedHHPLCodeImage->SymbolTable->TableEntries[i].Location;
				NameMatchFound = true;
			}
		}


	// If only host routines may be executed, search the code image symbol table for all host routines.
		else if(HostRoutineControlMask & HHPL_HOST_COMMAND_EXECUTE_ONLY_HOST_ROUTINES)
		{
			i = 0;
			while((i < ((int) HostRoutineProgramProfile->ParsedHHPLCodeImage->SymbolTable->NumberOfTableEntries)) &&
					((HostRoutineProgramProfile->ParsedHHPLCodeImage->SymbolTable->TableEntries[i].Name->Compare
											(RoutineName, RoutineNameLength, false)) ||
					 (!(HostRoutineProgramProfile->ParsedHHPLCodeImage->SymbolTable->TableEntries[i].Type &
														HHPL_ITEM_TABLE::ROUTINE_ITEM)) ||
					 (!(HostRoutineProgramProfile->ParsedHHPLCodeImage->SymbolTable->TableEntries[i].Type &
														HHPL_ITEM_TABLE::HOST_ACCESSIBLE))))
			{
				++i;
			}


		// If we found a match in the symbol table, store the location and set the match flag
			if(i < ((int) HostRoutineProgramProfile->ParsedHHPLCodeImage->SymbolTable->NumberOfTableEntries))
			{
				NameOfHostRoutineThatWasExecuted->Copy
								(HostRoutineProgramProfile->ParsedHHPLCodeImage->SymbolTable->TableEntries[i].Name);

				LocationOfRoutine = HostRoutineProgramProfile->ParsedHHPLCodeImage->SymbolTable->TableEntries[i].Location;
				NameMatchFound = true;
			}
		}
	}


// If we found a match, execute the routine!  Note that we'll also verify that the routine location is in bounds.
	if(NameMatchFound)
	{
		if(LocationOfRoutine < ((int) HostRoutineProgramProfile->Size))
		{
		// Go ahead and execute the routine.
		// Let's save the current instruction pointer, set the instruction pointer to the subroutine, run it,
		// and restore the instruction pointer to the next instruction following this one.
			HWSTRING		OldEditBuffer(HostRoutineProgramProfile->EditBuffer);
			unsigned int	NextInstructionBeyondSubroutineCall;

			NextInstructionBeyondSubroutineCall = HostRoutineProgramProfile->NextInstruction;

			HostRoutineProgramProfile->NextInstruction = ((unsigned int) LocationOfRoutine);

			if(UseEditBufferContents)
			{
				HostRoutineProgramProfile->EditBuffer->Copy(EditBufferContents, EditBufferContentsLength);
			}

			ProcessRoutine(HostRoutineProgramProfile, HostRoutineIPB);

			HostRoutineProgramProfile->EditBuffer->Copy(&OldEditBuffer);

			HostRoutineProgramProfile->NextInstruction = NextInstructionBeyondSubroutineCall;
		}
		else
		{
		// The offset to the host routine is beyond the end of the HHPL code.  WOOF WOOF WOOF...
			HostRoutineProgramProfile->UnrecoverableError = true;
			sprintf(TempStr, "Host routine location is beyond end of format code: IP = %08X",
																			((unsigned int) LocationOfRoutine));
			LogError(TempStr);
		}
	}


	return(NameMatchFound);
} // ExecuteHostRoutine





// Special dispatch routines to process run-time commands

//
//	CheckForCommand
//		Execute a remote host command if one is waiting, then continue on.
//
void HHPL_PROCESSOR::CheckForCommand(HWSTRING *ReturnString)
{
// If a remote host command is waiting, execute it and continue on.
	ExecuteRunTimeScriptCommand();

// If a host routine was executed, copy the name of it to the return string
	ReturnString->Copy(NameOfHostRoutineThatWasExecuted);

	return;
} // CheckForCommand





//
//	WaitForCommand
//		Don't return until a remote host command has been found and an attempt has been made
//		to execute it.
//
void HHPL_PROCESSOR::WaitForCommand(HWSTRING *ReturnString)

{
// Don't quit until we find a remote host command.  It does not have to execute successfully,
// but there must be one in the buffer.
	while(ExecuteRunTimeScriptCommand() == EXEC_RTCMD_NO_COMMAND_FOUND)
	{
		ProcessBackgroundOperationsDuringScriptExecution();
	}

// If a host routine was executed, copy the name of it to the return string
	ReturnString->Copy(NameOfHostRoutineThatWasExecuted);

	return;
} // WaitForCommand




//
//	WaitForMultipleCommands
//		Don't return until a remote host END command has been received.
//
void HHPL_PROCESSOR::WaitForMultipleCommands(HWSTRING *ReturnString)

{
// Don't quit until we find a remote host END command.  We'll continue looking for an END command
// even when one or more remote host commands do not execute successfully.
	while(ExecuteRunTimeScriptCommand() != EXEC_RTCMD_END_COMMAND_ENCOUNTERED)
	{
		ProcessBackgroundOperationsDuringScriptExecution();
	}

// If a host routine was executed, copy the name of it to the return string
	ReturnString->Copy(NameOfHostRoutineThatWasExecuted);

	return;
} // WaitForMultipleCommands







//
//	SendWithTranslation
//		Send the input string to the output, translating and suppressing characters as we go;
//		we'll save the copied material in ReturnString, in case a nested instruction needs this
//		result.  We'll also tack on an optional character.
//
void HHPL_PROCESSOR::SendWithTranslation( wchar_t *StringToSend, unsigned int LengthOfStringToSend,
			unsigned char CharacterToSendAtEnd, HHPL_PROGRAM_PROFILE *ProgramProfile)
{
	unsigned int	i;
	unsigned int	j;
	wchar_t			wchTemp;


	ProgramProfile->ReturnString->Size = 0;

	for(i = 0;    i < LengthOfStringToSend;    ++i)
	{
	// Don't send character if it should be suppressed.
		for(j = 0;    j < ProgramProfile->NumberOfSuppressionCharacters;    ++j)
		{
			if(ProgramProfile->SuppressionCharacters[j] == StringToSend[i])
			{
				break;
			}
		}

	// If we didn't find a suppression character above, replace the character if configured to
	// do so and send it to the output.
		if(j >= ProgramProfile->NumberOfSuppressionCharacters)
		{
		// Replace character if configured to do so.
			for(j = 0;    j < (ProgramProfile->NumberOfReplacementCharacters * 2);    j += 2)
			{
				if(ProgramProfile->ReplacementCharacters[j] == StringToSend[i])
				{
					break;
				}
			}

		// If we found a replacement character, send that, else send the original character
			if(j < (ProgramProfile->NumberOfReplacementCharacters * 2))
			{
				wchTemp = (wchar_t) ProgramProfile->ReplacementCharacters[j+1];
				SendToHostParameters.String = &wchTemp;
				SendToHostParameters.LengthOfString = 1;
				SendToHostParameters.DataSourceInformation = DEFAULT_DATA_SOURCE_ID;
				SendToHostParameters.ControlMask = 0;
				SendToHostParameters.DelayInMicroseconds = 0; 
				SendToHost( &SendToHostParameters );
//				ProgramProfile->ReturnString->Append(&ProgramProfile->ReplacementCharacters[j+1], 1);
				ProgramProfile->ReturnString->AppendChar( ProgramProfile->ReplacementCharacters[j+1] );
			}
			else
			{
				wchTemp = StringToSend[i];
				SendToHostParameters.String = &wchTemp;
				SendToHostParameters.LengthOfString = 1;
				SendToHostParameters.DataSourceInformation = DEFAULT_DATA_SOURCE_ID;
				SendToHostParameters.ControlMask = 0;
				SendToHostParameters.DelayInMicroseconds = 0; 
				SendToHost( &SendToHostParameters );
//				ProgramProfile->ReturnString->Append(&StringToSend[i], 1);
				ProgramProfile->ReturnString->AppendChar( StringToSend[i] );
			}
		}
	}

// If the optional character to send is non-NULL, send it.
	if(CharacterToSendAtEnd != 0)
	{
		wchTemp = (wchar_t) CharacterToSendAtEnd;
		SendToHostParameters.String = &wchTemp;
		SendToHostParameters.LengthOfString = 1;
		SendToHostParameters.DataSourceInformation = DEFAULT_DATA_SOURCE_ID;
		SendToHostParameters.ControlMask = 0;
		SendToHostParameters.DelayInMicroseconds = 0; 
		SendToHost( &SendToHostParameters );
	}


	return;
}






//
//	CursorIsValid
//		Check to see if the number sent to us is in valid cursor range.
//
bool HHPL_PROCESSOR::CursorIsValid(HHPL_PROGRAM_PROFILE *ProgramProfile, int NewCursorSetting)
{
// Check to see if cursor is outside the input string range
	if((NewCursorSetting < 0) || (NewCursorSetting >= ((int) ProgramProfile->EditBuffer->Size)))
	{
		return false;
	}
	else
	{
		return true;
	}
}






//
//	ChangeCursor
//		Move the cursor to the caller specified location.  If the cursor is out of range, then
//		note the failure and indicate an unrecoverable error if this instruction is not nested
//		(nesting level 1).  The reason that we'll continue if this instruction is nested is that
//		the program may be trying the move as a test condition.
//
void HHPL_PROCESSOR::ChangeCursor(HHPL_PROGRAM_PROFILE *ProgramProfile, int NewCursorSetting)
{
	ProgramProfile->ReturnNumber = ProgramProfile->Cursor = ((unsigned int) NewCursorSetting);

// Check to see if cursor is outside the input string range
	if(CursorIsValid(ProgramProfile, NewCursorSetting))
	{
		ProgramProfile->ReturnBool = true;
	}

	else
	{
		ProgramProfile->ReturnBool = false;
	}

	return;
}






//
//	CheckForArguments
//		Check to make sure that there are at least the caller specified minimum number of
//		bytes or instructions remaining in the HHPL program and if not, indicate an
//		unrecoverable error.
//
void HHPL_PROCESSOR::CheckForArguments(HHPL_PROGRAM_PROFILE *ProgramProfile,
										   int MinimumNumberOfArguments)
{
// Check to see if minimum number of arguments exist
	if((ProgramProfile->NextInstruction + MinimumNumberOfArguments) > ProgramProfile->Size)
	{
		ProgramProfile->UnrecoverableError = true;
		sprintf(TempStr, "Missing %d argument(s) on last instruction: IP = %08X",
							MinimumNumberOfArguments, ProgramProfile->NextInstruction);
		LogError(TempStr);
	}

	return;
}






//
//	InstructionTypeMatch
//		Return the true if the next instruction flag value for the next instruction.  If the
//		instruction is a two byte instruction, check the flag value against the sub-instruction
//		rather than the main instruction.
//
bool HHPL_PROCESSOR::InstructionTypeMatch(HHPL_PROGRAM_PROFILE *ProgramProfile, unsigned int TypeFlag)
{
	bool	MatchStatus = false;		// No match yet.


// Is there another  instruction?
	if(ProgramProfile->NextInstruction < ProgramProfile->Size)
	{
	// Is the next instruction a 2 byte instruction?
		if(HHPLInstructionFlags[ProgramProfile->Code[ProgramProfile->NextInstruction]] &
												HHPL_Inst_TwoByteInstruction)
		{
		// Is there another instruction byte?
			if((ProgramProfile->NextInstruction + 1) < ProgramProfile->Size)
			{
			// Does the sub-instruction type match the input type?
				MatchStatus =
					(HHPLSubInstructionFlagPointers
							[ProgramProfile->Code[ProgramProfile->NextInstruction]]
							[ProgramProfile->Code[ProgramProfile->NextInstruction + 1]] & TypeFlag)
						? true
						: false;
			}
		}

		else
		{
		// Does the sub-instruction type match the input type?
			MatchStatus =
					(HHPLInstructionFlags
						[ProgramProfile->Code[ProgramProfile->NextInstruction]] & TypeFlag)
								? true
								: false;
		}
	}

	return(MatchStatus);
} // InstructionTypeMatch






//
//	GetParameters
//		Get the input parameters requested by the caller and store them in operands in the
//		program profile.
//
bool HHPL_PROCESSOR::GetParameters(HHPL_PROGRAM_PROFILE *ProgramProfile,
		HHPL_INSTRUCTION_PARAMETER_BLOCK *IPB,
		const char *InstructionDescription, int NumberOfParameters, int Parameter1Type,
		int Parameter2Type, int Parameter3Type, int Parameter4Type,
		int Parameter5Type, int Parameter6Type, int Parameter7Type)
{
	bool			Status = true;			// No errors yet.
	int				i;
	int				NextParameterType=0;
	int				NumberOfBooleanParametersProcessed=0;
	int				NumberOfNumericParametersProcessed=0;
	int				NumberOfStringParametersProcessed=0;
	int				NumberOfWideStringParametersProcessed=0;
	int				NumberOfDataAddressParametersProcessed=0;
	int				NumberOfCodeAddressParametersProcessed=0;
	int				NumberOfRangeParametersProcessed=0;
	unsigned int	OffsetToData;
	HHPL_INSTRUCTION_PARAMETER_BLOCK	TempIPB;




// Get all the parameters.
	for(i = 0;    ((i < NumberOfParameters) && (Status));    ++i)
	{
	// First, get the next parameter type.
		switch(i)
		{
			case 0:
				NextParameterType = Parameter1Type;
				break;
			case 1:
				NextParameterType = Parameter2Type;
				break;
			case 2:
				NextParameterType = Parameter3Type;
				break;
			case 3:
				NextParameterType = Parameter4Type;
				break;
			case 4:
				NextParameterType = Parameter5Type;
				break;
			case 5:
				NextParameterType = Parameter6Type;
				break;
			case 6:
				NextParameterType = Parameter7Type;
				break;
		}


	// Now, let's process the parameter.
		switch(NextParameterType)
		{
			case HHPL_PROC_GET_BOOLEAN_PARAMETER:
				if(ProgramProfile->NextInstruction >= ProgramProfile->Size)
				{
					ProgramProfile->UnrecoverableError = true;
					sprintf(TempStr, "End of program encountered while looking for get boolean parameter: instruction %s: IP = %08X",
							InstructionDescription, ProgramProfile->NextInstruction);
					LogError(TempStr);
					Status = false;
				}
				else
				{
					if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Ret_Boolean))
					{
						if(ExecuteNextInstruction(ProgramProfile, &TempIPB))
						{
						// We successfully executed the instruction; now, store the operand
						// from returned boolean.
							if(NumberOfBooleanParametersProcessed == 0)
							{
								IPB->BooleanOperand1 = ProgramProfile->ReturnBool;
							}
							else if(NumberOfBooleanParametersProcessed == 1)
							{
								IPB->BooleanOperand2 = ProgramProfile->ReturnBool;
							}
							else
							{
								IPB->BooleanOperand3 = ProgramProfile->ReturnBool;
							}
							++NumberOfBooleanParametersProcessed;
						}
						else
						{
							Status = false;
						}
					}
					else
					{
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "Illegal get boolean parameter:  parameter #%u:  instruction %s: IP = %08X",
							i, InstructionDescription, ProgramProfile->NextInstruction);
						LogError(TempStr);
						Status = false;
					}
				}

				break;


			case HHPL_PROC_SET_BOOLEAN_PARAMETER:
				if(ProgramProfile->NextInstruction >= ProgramProfile->Size)
				{
					ProgramProfile->UnrecoverableError = true;
					sprintf(TempStr, "End of program encountered while looking for set boolean parameter: instruction %s: IP = %08X",
							InstructionDescription, ProgramProfile->NextInstruction);
					LogError(TempStr);
					Status = false;
				}
				else
				{
					if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Set_Boolean))
					{
						Status = ExecuteNextInstruction(ProgramProfile, &TempIPB);
					}
					else
					{
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "Illegal set boolean parameter:  parameter #%u:  instruction %s: IP = %08X",
							i, InstructionDescription, ProgramProfile->NextInstruction);
						LogError(TempStr);
						Status = false;
					}
				}

				break;


			case HHPL_PROC_GET_NUMERIC_PARAMETER:
				if(ProgramProfile->NextInstruction >= ProgramProfile->Size)
				{
					ProgramProfile->UnrecoverableError = true;
					sprintf(TempStr, "End of program encountered while looking for get numeric parameter: instruction %s: IP = %08X",
							InstructionDescription, ProgramProfile->NextInstruction);
					LogError(TempStr);
					Status = false;
				}
				else
				{
					if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Ret_Number))
					{
						if(ExecuteNextInstruction(ProgramProfile, &TempIPB))
						{
						// We successfully executed the instruction; now, store the operand
						// from returned number.
							if(NumberOfNumericParametersProcessed == 0)
							{
								IPB->IntegerOperand1 = ProgramProfile->ReturnNumber;
							}
							else if(NumberOfNumericParametersProcessed == 1)
							{
								IPB->IntegerOperand2 = ProgramProfile->ReturnNumber;
							}
							else if(NumberOfNumericParametersProcessed == 2)
							{
								IPB->IntegerOperand3 = ProgramProfile->ReturnNumber;
							}
							else if(NumberOfNumericParametersProcessed == 3)
							{
								IPB->IntegerOperand4 = ProgramProfile->ReturnNumber;
							}
							else if(NumberOfNumericParametersProcessed == 4)
							{
								IPB->IntegerOperand5 = ProgramProfile->ReturnNumber;
							}
							else if(NumberOfNumericParametersProcessed == 5)
							{
								IPB->IntegerOperand6 = ProgramProfile->ReturnNumber;
							}
							else if(NumberOfNumericParametersProcessed == 6)
							{
								IPB->IntegerOperand7 = ProgramProfile->ReturnNumber;
							}
							else
							{
								IPB->IntegerOperand8 = ProgramProfile->ReturnNumber;
							}
							++NumberOfNumericParametersProcessed;
						}
						else
						{
							Status = false;
						}
					}
					else
					{
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "Illegal get numeric parameter:  parameter #%u:  instruction %s: IP = %08X",
							i, InstructionDescription, ProgramProfile->NextInstruction);
						LogError(TempStr);
						Status = false;
					}
				}

				break;


			case HHPL_PROC_SET_NUMERIC_PARAMETER:
				if(ProgramProfile->NextInstruction >= ProgramProfile->Size)
				{
					ProgramProfile->UnrecoverableError = true;
					sprintf(TempStr, "End of program encountered while looking for set number parameter: instruction %s: IP = %08X",
							InstructionDescription, ProgramProfile->NextInstruction);
					LogError(TempStr);
					Status = false;
				}
				else
				{
					if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Set_Integer))
					{
						Status = ExecuteNextInstruction(ProgramProfile, &TempIPB);
					}
					else
					{
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "Illegal set number parameter:  parameter #%u:  instruction %s: IP = %08X",
							i, InstructionDescription, ProgramProfile->NextInstruction);
						LogError(TempStr);
						Status = false;
					}
				}

				break;


			case HHPL_PROC_GET_STRING_PARAMETER:
			case HHPL_PROC_STRING_RETURN_PARAMETER:
				if(ProgramProfile->NextInstruction >= ProgramProfile->Size)
				{
					ProgramProfile->UnrecoverableError = true;
					sprintf(TempStr, "End of program encountered while looking for get string parameter: instruction %s: IP = %08X",
							InstructionDescription, ProgramProfile->NextInstruction);
					LogError(TempStr);
					Status = false;
				}
				else
				{
					if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Ret_String))
					{
						if(ExecuteNextInstruction(ProgramProfile, &TempIPB))
						{
						// We successfully executed the instruction; now, store the operand
						// from returned string.
							if(NextParameterType == HHPL_PROC_GET_STRING_PARAMETER)
							{
								if(NumberOfStringParametersProcessed)
								{
									if(!IPB->StringOperand2Defined)
									{
										IPB->StringOperand2Defined = true;
										IPB->StringOperand2 = new HSTRING(MAXIMUM_STRING_SIZE, true);
									}

									IPB->StringOperand2->Copy(ProgramProfile->ReturnString);
								}
								else
								{
									if(!IPB->StringOperand1Defined)
									{
										IPB->StringOperand1Defined = true;
										IPB->StringOperand1 = new HSTRING(MAXIMUM_STRING_SIZE, true);
									}

									IPB->StringOperand1->Copy(ProgramProfile->ReturnString);
								}
							}
							++NumberOfStringParametersProcessed;
						}
						else
						{
							Status = false;
						}
					}

				// For the benefit of old programs that are trying to get a byte string from a dispatch function that now
				// returns a wide string, we'll be nice and do the conversion.
					else if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Ret_WideString))
					{
						if(ExecuteNextInstruction(ProgramProfile, &TempIPB))
						{
						// We successfully executed the instruction; now, store the operand
						// from returned string.
							if(NextParameterType == HHPL_PROC_GET_STRING_PARAMETER)
							{
								if(NumberOfStringParametersProcessed)
								{
									if(!IPB->StringOperand2Defined)
									{
										IPB->StringOperand2Defined = true;
										IPB->StringOperand2 = new HSTRING(MAXIMUM_STRING_SIZE, true);
									}

									ProgramProfile->ReturnWideString->ConvertAndCopyToHString( IPB->StringOperand2 );
								}
								else
								{
									if(!IPB->StringOperand1Defined)
									{
										IPB->StringOperand1Defined = true;
										IPB->StringOperand1 = new HSTRING(MAXIMUM_STRING_SIZE, true);
									}

									ProgramProfile->ReturnWideString->ConvertAndCopyToHString( IPB->StringOperand1 );
								}
							}
							else
							{
								ProgramProfile->ReturnWideString->ConvertAndCopyToHString( ProgramProfile->ReturnString );
							}
							++NumberOfStringParametersProcessed;
						}
						else
						{
							Status = false;
						}
					}
					else
					{
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "Illegal get string parameter:  parameter #%u:  instruction %s: IP = %08X",
							i, InstructionDescription, ProgramProfile->NextInstruction);
						LogError(TempStr);
						Status = false;
					}
				}

				break;




			case HHPL_PROC_GET_WIDE_STRING_PARAMETER:
			case HHPL_PROC_WIDE_STRING_RETURN_PARAMETER:
				if(ProgramProfile->NextInstruction >= ProgramProfile->Size)
				{
					ProgramProfile->UnrecoverableError = true;
					sprintf(TempStr, "End of program encountered while looking for get wide string parameter: instruction %s: IP = %08X",
							InstructionDescription, ProgramProfile->NextInstruction);
					LogError(TempStr);
					Status = false;
				}
				else
				{
					if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Ret_WideString))
					{
						if(ExecuteNextInstruction(ProgramProfile, &TempIPB))
						{
						// We successfully executed the instruction; now, store the operand
						// from returned string.
							if(NextParameterType == HHPL_PROC_GET_WIDE_STRING_PARAMETER)
							{
								if(NumberOfWideStringParametersProcessed)
								{
									if(!IPB->WideStringOperand2Defined)
									{
										IPB->WideStringOperand2Defined = true;
										IPB->WideStringOperand2 = new HWSTRING(MAXIMUM_STRING_SIZE, true);
									}

									IPB->WideStringOperand2->Copy(ProgramProfile->ReturnWideString);
								}
								else
								{
									if(!IPB->WideStringOperand1Defined)
									{
										IPB->WideStringOperand1Defined = true;
										IPB->WideStringOperand1 = new HWSTRING(MAXIMUM_STRING_SIZE, true);
									}

									IPB->WideStringOperand1->Copy(ProgramProfile->ReturnWideString);
								}
							}

							++NumberOfWideStringParametersProcessed;
						}
						else
						{
							Status = false;
						}
					}

				// For the benefit of old programs that are returning a byte string to a dispatch function that now
				// requires a wide string, we'll be nice and do the conversion.
					else if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Ret_String))
					{
						if(ExecuteNextInstruction(ProgramProfile, &TempIPB))
						{
						// We successfully executed the instruction; now, store the operand
						// from returned string.
							if(NextParameterType == HHPL_PROC_GET_WIDE_STRING_PARAMETER)
							{
								if(NumberOfWideStringParametersProcessed)
								{
									if(!IPB->WideStringOperand2Defined)
									{
										IPB->WideStringOperand2Defined = true;
										IPB->WideStringOperand2 = new HWSTRING(MAXIMUM_STRING_SIZE, true);
									}

									IPB->WideStringOperand2->ConvertAndCopyFromHString( ProgramProfile->ReturnString );
								}
								else
								{
									if(!IPB->WideStringOperand1Defined)
									{
										IPB->WideStringOperand1Defined = true;
										IPB->WideStringOperand1 = new HWSTRING(MAXIMUM_STRING_SIZE, true);
									}

									IPB->WideStringOperand1->ConvertAndCopyFromHString( ProgramProfile->ReturnString );
								}
							}
							else
							{
								ProgramProfile->ReturnWideString->ConvertAndCopyFromHString( ProgramProfile->ReturnString );
							}
							++NumberOfWideStringParametersProcessed;
						}
						else
						{
							Status = false;
						}
					}
					else
					{
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "Illegal get wide string parameter:  parameter #%u:  instruction %s: IP = %08X",
							i, InstructionDescription, ProgramProfile->NextInstruction);
						LogError(TempStr);
						Status = false;
					}
				}

				break;


			case HHPL_PROC_SET_STRING_PARAMETER:
				if(ProgramProfile->NextInstruction >= ProgramProfile->Size)
				{
					ProgramProfile->UnrecoverableError = true;
					sprintf(TempStr, "End of program encountered while looking for set string parameter: instruction %s: IP = %08X",
							InstructionDescription, ProgramProfile->NextInstruction);
					LogError(TempStr);
					Status = false;
				}
				else
				{
					if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Set_String))
					{
						Status = ExecuteNextInstruction(ProgramProfile, &TempIPB);
					}

				// For the benefit of old programs that are trying to set a byte string from a dispatch function that now
				// returns a wide string, we'll be nice and do the conversion.
					else if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Set_WideString))
					{
						ProgramProfile->ReturnWideString->ConvertAndCopyFromHString( ProgramProfile->ReturnString );
						Status = ExecuteNextInstruction(ProgramProfile, &TempIPB);
					}

					else
					{
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "Illegal set string parameter:  parameter #%u:  instruction %s: IP = %08X",
							i, InstructionDescription, ProgramProfile->NextInstruction);
						LogError(TempStr);
						Status = false;
					}
				}

				break;


			case HHPL_PROC_SET_WIDE_STRING_PARAMETER:
				if(ProgramProfile->NextInstruction >= ProgramProfile->Size)
				{
					ProgramProfile->UnrecoverableError = true;
					sprintf(TempStr, "End of program encountered while looking for set wide string parameter: instruction %s: IP = %08X",
							InstructionDescription, ProgramProfile->NextInstruction);
					LogError(TempStr);
					Status = false;
				}
				else
				{
					if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Set_WideString))
					{
						Status = ExecuteNextInstruction(ProgramProfile, &TempIPB);
					}

				// For the benefit of new programs that are trying to set a wide string from a function that returns byte
				// strings, we'll be nice and do the conversion.
					else if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Set_String))
					{
						ProgramProfile->ReturnWideString->ConvertAndCopyToHString( ProgramProfile->ReturnString );
						Status = ExecuteNextInstruction(ProgramProfile, &TempIPB);
					}

					else
					{
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "Illegal set wide string parameter:  parameter #%u:  instruction %s: IP = %08X",
							i, InstructionDescription, ProgramProfile->NextInstruction);
						LogError(TempStr);
						Status = false;
					}
				}

				break;


			case HHPL_PROC_DATA_ADDRESS_PARAMETER:
				if((ProgramProfile->NextInstruction + 3) >= ProgramProfile->Size)
				{
					ProgramProfile->UnrecoverableError = true;
					sprintf(TempStr, "End of program encountered while looking for data address parameter: instruction %s: IP = %08X",
							InstructionDescription, ProgramProfile->NextInstruction);
					LogError(TempStr);
					Status = false;
				}
				else
				{
					OffsetToData = ((unsigned int) ProgramProfile->Code
									[ProgramProfile->NextInstruction + 3]) * 16777216 +
							((unsigned int) ProgramProfile->Code
									[ProgramProfile->NextInstruction + 2]) * 65536 +
							((unsigned int) ProgramProfile->Code
									[ProgramProfile->NextInstruction + 1]) * 256 +
							((unsigned int) ProgramProfile->Code
									[ProgramProfile->NextInstruction]);
					ProgramProfile->NextInstruction += 4;

				// Check the integrity of the offset (need total of 4 bytes).
					if(OffsetToData < ((int) ProgramProfile->SizeOfDataSpace))
					{
					// Store the address
						if(NumberOfDataAddressParametersProcessed)
						{
							IPB->DataAddress2 = OffsetToData;
						}
						else
						{
							IPB->DataAddress1 = OffsetToData;
						}
						++NumberOfDataAddressParametersProcessed;
					}
					else
					{
					// The data variable address is beyond the end of the HHPL data space.
					// WOOF WOOF WOOF...
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "Data variable location is beyond end of data space: instruction %s:  IP = %08X  Address = %08X",
									InstructionDescription, ProgramProfile->NextInstruction, OffsetToData );
						LogError(TempStr);
					}
				}

				break;


			case HHPL_PROC_CODE_ADDRESS_PARAMETER:
				if((ProgramProfile->NextInstruction + 3) >= ProgramProfile->Size)
				{
					ProgramProfile->UnrecoverableError = true;
					sprintf(TempStr, "End of program encountered while looking for code address parameter: instruction %s: IP = %08X",
							InstructionDescription, ProgramProfile->NextInstruction);
					LogError(TempStr);
					Status = false;
				}
				else
				{
					OffsetToData = ((unsigned int) ProgramProfile->Code
									[ProgramProfile->NextInstruction + 3]) * 16777216 +
							((unsigned int) ProgramProfile->Code
									[ProgramProfile->NextInstruction + 2]) * 65536 +
							((unsigned int) ProgramProfile->Code
									[ProgramProfile->NextInstruction + 1]) * 256 +
							((unsigned int) ProgramProfile->Code
									[ProgramProfile->NextInstruction]);
					ProgramProfile->NextInstruction += 4;

				// Check the integrity of the offset (need total of 4 bytes).
					if(OffsetToData < ProgramProfile->Size)
					{
					// Store the address
						if(NumberOfCodeAddressParametersProcessed)
						{
							IPB->CodeAddress2 = OffsetToData;
						}
						else
						{
							IPB->CodeAddress1 = OffsetToData;
						}
						++NumberOfCodeAddressParametersProcessed;
					}
					else
					{
					// The code address is beyond the end of the HHPL data space.
					// WOOF WOOF WOOF...
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "Code location is beyond end of code space: instruction %s:  IP = %08X",
									InstructionDescription, ProgramProfile->NextInstruction);
						LogError(TempStr);
					}
				}

				break;


			case HHPL_PROC_SUBINSTRUCTION_PARAMETER:
				if(ProgramProfile->NextInstruction >= ProgramProfile->Size)
				{
					ProgramProfile->UnrecoverableError = true;
					sprintf(TempStr, "End of program encountered while looking for sub-instruction parameter: instruction %s: IP = %08X",
							InstructionDescription, ProgramProfile->NextInstruction);
					LogError(TempStr);
					Status = false;
				}
				else
				{
					IPB->SubinstructionOperand1 =
										ProgramProfile->Code[ProgramProfile->NextInstruction];
					++ProgramProfile->NextInstruction;
				}

				break;


			case HHPL_PROC_RANGE_RETURN_PARAMETER:
				if(ProgramProfile->NextInstruction >= ProgramProfile->Size)
				{
					ProgramProfile->UnrecoverableError = true;
					sprintf(TempStr, "End of program encountered while looking for range return parameter: instruction %s: IP = %08X",
							InstructionDescription, ProgramProfile->NextInstruction);
					LogError(TempStr);
					Status = false;
				}
				else
				{
					if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Ret_Range))
					{
						Status = ExecuteNextInstruction(ProgramProfile, &TempIPB);
					}
					else
					{
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "Illegal range return parameter:  parameter #%u:  instruction %s: IP = %08X",
							i, InstructionDescription, ProgramProfile->NextInstruction);
						LogError(TempStr);
						Status = false;
					}
				}

				break;


			case HHPL_PROC_PREFIX_RETURN_PARAMETER:
				if(ProgramProfile->NextInstruction >= ProgramProfile->Size)
				{
					ProgramProfile->UnrecoverableError = true;
					sprintf(TempStr, "End of program encountered while looking for prefix return parameter: instruction %s: IP = %08X",
							InstructionDescription, ProgramProfile->NextInstruction);
					LogError(TempStr);
					Status = false;
				}
				else
				{
					if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Ret_Prefix))
					{
						Status = ExecuteNextInstruction(ProgramProfile, &TempIPB);
					}
					else
					{
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "Illegal prefix return parameter:  parameter #%u:  instruction %s: IP = %08X",
							i, InstructionDescription, ProgramProfile->NextInstruction);
						LogError(TempStr);
						Status = false;
					}
				}

				break;


			case HHPL_PROC_SUFFIX_RETURN_PARAMETER:
				if(ProgramProfile->NextInstruction >= ProgramProfile->Size)
				{
					ProgramProfile->UnrecoverableError = true;
					sprintf(TempStr, "End of program encountered while looking for suffix return parameter: instruction %s: IP = %08X",
							InstructionDescription, ProgramProfile->NextInstruction);
					LogError(TempStr);
					Status = false;
				}
				else
				{
					if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Ret_Suffix))
					{
						Status = ExecuteNextInstruction(ProgramProfile, &TempIPB);
					}
					else
					{
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "Illegal suffix return parameter:  parameter #%u:  instruction %s: IP = %08X",
							i, InstructionDescription, ProgramProfile->NextInstruction);
						LogError(TempStr);
						Status = false;
					}
				}

				break;


			case HHPL_PROC_GET_RANGE_PARAMETER:
				if(ProgramProfile->NextInstruction >= ProgramProfile->Size)
				{
					ProgramProfile->UnrecoverableError = true;
					sprintf(TempStr, "End of program encountered while looking for get range parameter: instruction %s: IP = %08X",
							InstructionDescription, ProgramProfile->NextInstruction);
					LogError(TempStr);
					Status = false;
				}
				else
				{
					if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Ret_Range))
					{
						if(ExecuteNextInstruction(ProgramProfile, &TempIPB))
						{
						// We successfully executed the instruction; now, store the operand
						// from returned range.
							if(NumberOfRangeParametersProcessed == 0)
							{
								IPB->RangeOperand1 = ProgramProfile->ReturnRange;
							}
							else
							{
								IPB->RangeOperand2 = ProgramProfile->ReturnRange;
							}
							++NumberOfRangeParametersProcessed;
						}
						else
						{
							Status = false;
						}
					}
					else
					{
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "Illegal get range parameter:  parameter #%u:  instruction %s: IP = %08X",
							i, InstructionDescription, ProgramProfile->NextInstruction);
						LogError(TempStr);
						Status = false;
					}
				}

				break;
		}
	}


	return(Status);
}






//
//	ProcessTestRoutine
//		Get the test routine parameters requested by the caller and call the test routine.
//
bool HHPL_PROCESSOR::ProcessTestRoutine(int TestRoutineNumber, int NumberOfParameters,
		HHPL_PROGRAM_PROFILE *ProgramProfile, HHPL_INSTRUCTION_PARAMETER_BLOCK *IPB)
{
	bool		Status = true;			// No errors yet.
	int			FirstParameterInstructionPointer;
	bool		BooleanParameters[MAXIMUM_NUMBER_OF_TEST_ROUTINE_PARAMETERS];
	int			IntegerParameters[MAXIMUM_NUMBER_OF_TEST_ROUTINE_PARAMETERS];
	HSTRING		*StringParameters[MAXIMUM_NUMBER_OF_TEST_ROUTINE_PARAMETERS];
	HWSTRING	*WideStringParameters[MAXIMUM_NUMBER_OF_TEST_ROUTINE_PARAMETERS];
	bool		StringParameterWasAllocated[MAXIMUM_NUMBER_OF_TEST_ROUTINE_PARAMETERS];
	bool		WideStringParameterWasAllocated[MAXIMUM_NUMBER_OF_TEST_ROUTINE_PARAMETERS];
//	bool		ParameterMustBeSetAfterTestRoutineExecution[MAXIMUM_NUMBER_OF_TEST_ROUTINE_PARAMETERS];
	void		*ParameterPointers[MAXIMUM_NUMBER_OF_TEST_ROUTINE_PARAMETERS];
	int			NextParameterNumber;




// If the number of parameters requested is too high, WOOF!
	if(NumberOfParameters > MAXIMUM_NUMBER_OF_TEST_ROUTINE_PARAMETERS)
	{
		ProgramProfile->UnrecoverableError = true;
		sprintf(TempStr, "Too many parameters in TestRoutine: IP = %08X",
							ProgramProfile->NextInstruction);
		LogError(TempStr);
		return(false);
	}


// Indicate that no string parameter memory has been allocated.
	for(NextParameterNumber = 0;
		NextParameterNumber < MAXIMUM_NUMBER_OF_TEST_ROUTINE_PARAMETERS;    ++NextParameterNumber)
	{
		StringParameterWasAllocated[NextParameterNumber] = false;
		WideStringParameterWasAllocated[NextParameterNumber] = false;
//		ParameterMustBeSetAfterTestRoutineExecution[NextParameterNumber] = false;
	}


// Indicate where the parameter instructions begin.  We'll have to rerun through these
// instructions to process set parameter commands after calling TestRoutine.
	FirstParameterInstructionPointer = ProgramProfile->NextInstruction;

// Get all the parameters.
	for(NextParameterNumber = 0;    ((NextParameterNumber < NumberOfParameters) && (Status));
													++NextParameterNumber)
	{
	// Let's get the next parameter type.  If we'll be setting a variable, turn off updates.
	// Are we getting a boolean expression?
		if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Ret_Boolean))
		{
			if(ExecuteNextInstruction(ProgramProfile, IPB))
			{
			// We successfully executed the instruction; now, store the operand
			// from returned boolean.
				BooleanParameters[NextParameterNumber] = ProgramProfile->ReturnBool;
				ParameterPointers[NextParameterNumber] = &BooleanParameters[NextParameterNumber];
			}
			else
			{
				ProgramProfile->UnrecoverableError = true;
				sprintf(TempStr, "Illegal TestRoutine get boolean parameter:  parameter #%u:  IP = %08X",
							NextParameterNumber, ProgramProfile->NextInstruction);
				LogError(TempStr);
				Status = false;
			}
		}


	// Are we getting an integer expression?
		else if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Ret_Number))
		{
			if(ExecuteNextInstruction(ProgramProfile, IPB))
			{
			// We successfully executed the instruction; now, store the operand
			// from returned integer.
				IntegerParameters[NextParameterNumber] = ProgramProfile->ReturnNumber;
				ParameterPointers[NextParameterNumber] = &IntegerParameters[NextParameterNumber];
			}
			else
			{
				ProgramProfile->UnrecoverableError = true;
				sprintf(TempStr, "Illegal TestRoutine get numeric parameter:  parameter #%u:  IP = %08X",
							NextParameterNumber, ProgramProfile->NextInstruction);
				LogError(TempStr);
				Status = false;
			}
		}



	// Are we getting a string expression?
		else if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Ret_String))
		{
			if(ExecuteNextInstruction(ProgramProfile, IPB))
			{
			// We successfully executed the instruction; now, store the operand
			// from returned string.
				StringParameters[NextParameterNumber] =
						new HSTRING(ProgramProfile->ReturnString, true);
				StringParameterWasAllocated[NextParameterNumber] = true;
				StringParameters[NextParameterNumber]->Copy(ProgramProfile->ReturnString);
				ParameterPointers[NextParameterNumber] = StringParameters[NextParameterNumber];
			}
			else
			{
				ProgramProfile->UnrecoverableError = true;
				sprintf(TempStr, "Illegal TestRoutine get string parameter:  parameter #%u:  IP = %08X",
							NextParameterNumber, ProgramProfile->NextInstruction);
				LogError(TempStr);
				Status = false;
			}
		}



	// Are we getting a wide string expression?
		else if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Ret_WideString))
		{
			if(ExecuteNextInstruction(ProgramProfile, IPB))
			{
			// We successfully executed the instruction; now, store the operand
			// from returned string.
				WideStringParameters[NextParameterNumber] =
						new HWSTRING(ProgramProfile->ReturnWideString, true);
				WideStringParameterWasAllocated[NextParameterNumber] = true;
				WideStringParameters[NextParameterNumber]->Copy(ProgramProfile->ReturnWideString);
				ParameterPointers[NextParameterNumber] = WideStringParameters[NextParameterNumber];
			}
			else
			{
				ProgramProfile->UnrecoverableError = true;
				sprintf(TempStr, "Illegal TestRoutine get wide string parameter:  parameter #%u:  IP = %08X",
							NextParameterNumber, ProgramProfile->NextInstruction);
				LogError(TempStr);
				Status = false;
			}
		}


	// Are we setting a boolean expression?
		else if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Set_Boolean))
		{
			SkipOverInstructionsWithoutExecutingThem = true;

			if(ExecuteNextInstruction(ProgramProfile, IPB))
			{
			// We successfully executed the instruction; now, store the address of the
			// boolean return variable.
				ParameterPointers[NextParameterNumber] = &BooleanParameters[NextParameterNumber];
			}
			else
			{
				ProgramProfile->UnrecoverableError = true;
				sprintf(TempStr, "Illegal TestRoutine set boolean parameter:  parameter #%u:  IP = %08X",
							NextParameterNumber, ProgramProfile->NextInstruction);
				LogError(TempStr);
				Status = false;
			}

			SkipOverInstructionsWithoutExecutingThem = false;
		}


	// Are we setting an integer expression?
		else if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Set_Integer))
		{
			SkipOverInstructionsWithoutExecutingThem = true;

			if(ExecuteNextInstruction(ProgramProfile, IPB))
			{
			// We successfully executed the instruction; now, point to the integer
			// parameter.
				ParameterPointers[NextParameterNumber] = &IntegerParameters[NextParameterNumber];
			}
			else
			{
				ProgramProfile->UnrecoverableError = true;
				sprintf(TempStr, "Illegal TestRoutine set numeric parameter:  parameter #%u:  IP = %08X",
							NextParameterNumber, ProgramProfile->NextInstruction);
				LogError(TempStr);
				Status = false;
			}

			SkipOverInstructionsWithoutExecutingThem = false;
		}



	// Are we setting a string expression?
		else if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Set_String))
		{
			SkipOverInstructionsWithoutExecutingThem = true;

		// Even though no assignment will be performed, the setstring instruction will
		// still need a register value.
			StringParameters[NextParameterNumber] =
										new HSTRING(MAXIMUM_STRING_SIZE, true);
			StringParameterWasAllocated[NextParameterNumber] = true;
			StringRegisters[0] = StringParameters[NextParameterNumber];

			if(ExecuteNextInstruction(ProgramProfile, IPB))
			{
			// We successfully executed the instruction.
				ParameterPointers[NextParameterNumber] = StringParameters[NextParameterNumber];
			}
			else
			{
				ProgramProfile->UnrecoverableError = true;
				sprintf(TempStr, "Illegal TestRoutine set string parameter:  parameter #%u:  IP = %08X",
							NextParameterNumber, ProgramProfile->NextInstruction);
				LogError(TempStr);
				Status = false;
			}

			SkipOverInstructionsWithoutExecutingThem = false;
		}



	// Are we setting a wide string expression?
		else if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Set_WideString))
		{
			SkipOverInstructionsWithoutExecutingThem = true;

		// Even though no assignment will be performed, the setstring instruction will
		// still need a register value.
			WideStringParameters[NextParameterNumber] =
										new HWSTRING(MAXIMUM_STRING_SIZE, true);
			WideStringParameterWasAllocated[NextParameterNumber] = true;
			WideStringRegisters[0] = WideStringParameters[NextParameterNumber];

			if(ExecuteNextInstruction(ProgramProfile, IPB))
			{
			// We successfully executed the instruction.
				ParameterPointers[NextParameterNumber] = WideStringParameters[NextParameterNumber];
			}
			else
			{
				ProgramProfile->UnrecoverableError = true;
				sprintf(TempStr, "Illegal TestRoutine set widestring parameter:  parameter #%u:  IP = %08X",
							NextParameterNumber, ProgramProfile->NextInstruction);
				LogError(TempStr);
				Status = false;
			}

			SkipOverInstructionsWithoutExecutingThem = false;
		}


	// We don't recognize the parameter type; WOOF!!
		else
		{
			ProgramProfile->UnrecoverableError = true;
			sprintf(TempStr, "Illegal TestRoutine parameter:  parameter #%u:  IP = %08X",
							NextParameterNumber, ProgramProfile->NextInstruction);
			LogError(TempStr);
			Status = false;
		}
	}



	if(Status)
	{
		Status = TestRoutine(TestRoutineNumber, NumberOfParameters, ParameterPointers);

		if(Status)
		{
		// Move back to the beginning of the parameter instructions so that we can loop
		// through and do variable setting.
			ProgramProfile->NextInstruction = ((unsigned int) FirstParameterInstructionPointer);


		// Now, we'll reprocess the parameter instructions.  Before we process a set
		// parameter instruction, we'll load the appropriate register with the return
		// value from TestRoutine.

			for(NextParameterNumber = 0;
						((NextParameterNumber < NumberOfParameters) && (Status));
													++NextParameterNumber)
			{
			// Let's get the next parameter type.  If we're setting a parameter, setup
			// the appropriate register first.
			// Are we setting a boolean expression?
				if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Set_Boolean))
				{
					BooleanRegisters[0] = BooleanParameters[NextParameterNumber];
				}


			// Are we setting an integer expression?
				else if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Set_Integer))
				{
					IntegerRegisters[0] = IntegerParameters[NextParameterNumber];
				}



			// Are we setting a string expression?
				else if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Set_String))
				{
					StringRegisters[0] = StringParameters[NextParameterNumber];
				}


			// Are we setting a wide string expression?
				else if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Set_WideString))
				{
					WideStringRegisters[0] = WideStringParameters[NextParameterNumber];
				}

			// Go ahead and actually execute the instruction.
				Status = ExecuteNextInstruction(ProgramProfile, IPB);
			}
		}
	}



// Free string parameter memory.
	for(NextParameterNumber = 0;
		NextParameterNumber < MAXIMUM_NUMBER_OF_TEST_ROUTINE_PARAMETERS;    ++NextParameterNumber)
	{
		if(StringParameterWasAllocated[NextParameterNumber])
		{
			delete StringParameters[NextParameterNumber];
		}

		if(WideStringParameterWasAllocated[NextParameterNumber])
		{
			delete WideStringParameters[NextParameterNumber];
		}
	}

	return(Status);
} // ProcessTestRoutine







//
//	ExecuteNextInstruction
//		This is the routine that actually executes each instruction.  This routine must be called
//		for every single instruction.  If an instruction contains nested instructions, then this
//		routine will be called recursively once for every nested instruction.  The next
//		instruction will not be executed if an unrecoverable error has been encountered.  In
//		addition, if an exit instruction was encountered while executing nested instructions,
//		processing will stop.  For example, while executing a While instruction, an exit
//		instruction may be encountered and this will cause any other nested instructions to not
//		execute.
//
bool HHPL_PROCESSOR::ExecuteNextInstruction(HHPL_PROGRAM_PROFILE *ProgramProfile,
												HHPL_INSTRUCTION_PARAMETER_BLOCK *IPB)
{
	unsigned int	CharacterCount;		// Used to decode a character count embedded in an instruction
	int				Position;			// Position of cursor; may be set by a search, as the result of
										//	adding an offset to the cursor, or as the result of an
										//	instruction explicitly setting the cursor to some fixed
										//	position.
	wchar_t			Character;			// Character embedded in an instruction.
	int				i;
	unsigned int	MaximumStringSize;
	unsigned int	StringSize;
	unsigned int	NextInstructionBeyondSubroutineCall;
	MENU_PARAMETERS	*MenuParameters;




// Increment the nesting level.  Note that certain traditional data formatting instructions
// will WOOF at nesting level 1 (in other words, they're not nested), but those same instructions
// will not WOOF if they're nested in some other instruction(s).  For example, if a character
// comparison fails, then traditionally we'll WOOF, but we don't want to do a hard WOOF if the
// compare is part of a nested instruction, but instead we'll just treat the condition as a
// failure.
	ProgramProfile->NestingLevel++;


// Don't execute a command if an unrecoverable error has occurred or we've encountered an
// exit instruction.  Note that calls to this routine can be nested, so that an error or an
// exit instruction may be encountered several layers down.
	if((!ProgramProfile->ExitInstructionEncountered) &&
									(!ProgramProfile->UnrecoverableError) &&
									(!TerminateHHPLProgramAtNextInstruction) &&
									(!ProgramProfile->GoToEncountered))
	{
		switch (ProgramProfile->Code[ProgramProfile->NextInstruction])
		{
			case HHPL_Inst_NumericOperationOnNextTwoResults:
		//		Param1:		Numeric operation sub instruction
		//		Param2:		Command that returns the first numeric operand
		//		Param3:		Command that returns the second numeric operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "numeric operation", 3,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER,
							HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_NumberSubInst_Add:
							ProgramProfile->ReturnNumber =
								IPB->IntegerOperand1 + IPB->IntegerOperand2;
							break;

						case HHPL_NumberSubInst_Subtract:
							ProgramProfile->ReturnNumber =
								IPB->IntegerOperand1 - IPB->IntegerOperand2;
							break;

						case HHPL_NumberSubInst_Multiply:
							ProgramProfile->ReturnNumber =
								IPB->IntegerOperand1 * IPB->IntegerOperand2;
							break;

						case HHPL_NumberSubInst_Divide:
							ProgramProfile->ReturnNumber =
								IPB->IntegerOperand1 / IPB->IntegerOperand2;
							break;

						case HHPL_NumberSubInst_BitwiseAnd:
							ProgramProfile->ReturnNumber =
								IPB->IntegerOperand1 & IPB->IntegerOperand2;
							break;

						case HHPL_NumberSubInst_BitwiseOr:
							ProgramProfile->ReturnNumber =
								IPB->IntegerOperand1 | IPB->IntegerOperand2;
							break;

						case HHPL_NumberSubInst_BitwiseXor:
							ProgramProfile->ReturnNumber =
								IPB->IntegerOperand1 ^ IPB->IntegerOperand2;
							break;

						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized number operation sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;



			case HHPL_Inst_NumericOperationOnNextResult:
		//		Param1:		Numeric operation sub instruction
		//		Param2:		Command that returns a numeric operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "numeric operation", 2,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_NumberSubInst_Unary_Plus:
							ProgramProfile->ReturnNumber = +IPB->IntegerOperand1;
							break;

						case HHPL_NumberSubInst_Unary_Minus:
							ProgramProfile->ReturnNumber = -IPB->IntegerOperand1;
							break;

						case HHPL_NumberSubInst_Unary_Complement:
							ProgramProfile->ReturnNumber = ~IPB->IntegerOperand1;
							break;

						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized numeric operation sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;



			case HHPL_Inst_LogicalOperationOnNextTwoResults:
		//		Param1:		Logical operation sub instruction
		//		Param2:		Command that returns a boolean operand
		//		Param3:		Command that returns a boolean operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "logical operation", 3,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, HHPL_PROC_GET_BOOLEAN_PARAMETER,
							HHPL_PROC_GET_BOOLEAN_PARAMETER, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_LogicalSubInst_And:
							ProgramProfile->ReturnBool =
								(IPB->BooleanOperand1 && IPB->BooleanOperand2);
							break;

						case HHPL_LogicalSubInst_Or:
							ProgramProfile->ReturnBool =
								(IPB->BooleanOperand1 || IPB->BooleanOperand2);
							break;

						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized logical operation sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;



			case HHPL_Inst_LogicalOperationOnNextResult:
		//		Param1:		Logical operation sub instruction
		//		Param2:		Command that returns a boolean
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "logical operation", 2,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, HHPL_PROC_GET_BOOLEAN_PARAMETER, 0, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_LogicalSubInst_Not:
							ProgramProfile->ReturnBool = !IPB->BooleanOperand1;
							break;

						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized logical operation sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;




			case HHPL_Inst_LogicalConstant:
		//		Param1:		Logical constant sub instruction
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "logical constant", 1,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_LogicalSubInst_True:
							ProgramProfile->ReturnBool = true;
							break;

						case HHPL_LogicalSubInst_False:
							ProgramProfile->ReturnBool = false;
							break;

						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized logical constant sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;



			case HHPL_Inst_StringToBool:
		//		Param1:		Command that returns a string operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "string to bool conversion", 1,
							HHPL_PROC_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
					ProgramProfile->ReturnBool =
									(ProgramProfile->ReturnString->Size) ? true : false;
				}
				break;



			case HHPL_Inst_StringToInt:
		//		Param1:		Command that returns a string where N1 is the number of bytes in
		//					the command; this command will specify the first (and only) operand
		//		Param2:		Instruction that will set a numeric variable
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "string to int conversion", 1,
							HHPL_PROC_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// We successfully executed the instruction; now, convert the string to
				// an integer.
					int		NumberOfCharactersUsedInConversion;
					int		ConvertedNumber;
					ConvertedNumber = ProgramProfile->ReturnString->ConvertToNumber(0, &NumberOfCharactersUsedInConversion);

				// We have a number.  If the next instruction sets an integer variable,
				// then store the number of characters used in conversion as the return
				// number and execute the instruction.
					IntegerRegisters[0] = NumberOfCharactersUsedInConversion;
					if(GetParameters(ProgramProfile, IPB, "string to int conversion", 1,
									HHPL_PROC_SET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0, 0))
					{
					// We dumped successfully; store the converted number as the return
					// number.
						ProgramProfile->ReturnNumber = ConvertedNumber;
					}
				}
				break;


			case HHPL_Inst_LengthString:
		//		Param1:		Command that returns a string operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "length string", 1,
							HHPL_PROC_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
					ProgramProfile->ReturnNumber =
												ProgramProfile->ReturnString->Size;
				}
				break;



			case HHPL_Inst_IntToBool:
		//		Param1:		Command that returns a numeric operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "int to bool", 1,
							HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
					ProgramProfile->ReturnBool =
										(IPB->IntegerOperand1) ? true : false;
				}
				break;



			case HHPL_Inst_IntToString:
		//		Param1:		Command that returns a numeric operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "int to string", 1,
							HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
					sprintf(TempStr, INT_STRING_TO_INT_SPEC, IPB->IntegerOperand1);
					ProgramProfile->ReturnString->Copy(TempStr);
				}
				break;



			case HHPL_Inst_BoolToInt:
		//		Param1:		Command that returns a boolean operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "bool to int", 1,
							HHPL_PROC_GET_BOOLEAN_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// We successfully executed the instruction; now, convert the boolean to
				// a 0 for false or a 1 for true
					ProgramProfile->ReturnNumber = (IPB->BooleanOperand1) ? 1 : 0;
				}
				break;



			case HHPL_Inst_BoolToString:
		//		Param1:		Command that returns a boolean operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "bool to string", 1,
							HHPL_PROC_GET_BOOLEAN_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// We successfully executed the instruction; now, convert the boolean to
				// a "TRUE" or "FALSE" string
					if(IPB->BooleanOperand1)
					{
						ProgramProfile->ReturnString->Copy("TRUE");
					}
					else
					{
						ProgramProfile->ReturnString->Copy("FALSE");
					}
				}
				break;



			case HHPL_Inst_CharToInt:
		//		Param1:		Command that returns a string operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "char to int", 1,
													HHPL_PROC_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// We must have at least one character.
					if(ProgramProfile->ReturnString->Size > 0)
					{
						ProgramProfile->ReturnNumber =
								((int) ProgramProfile->ReturnString->Char[0]);
					}
					else
					{
					// Didn't find at least one character; WOOF!
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "String expression must contain at least one character for conversion: IP = %08X",
									ProgramProfile->NextInstruction);
						LogError(TempStr);
					}
				}
				break;



			case HHPL_Inst_IntToChar:
		//		Param1:		Command that returns a numeric operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "int to char", 1,
							HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
					TempStr[0] = ((char) IPB->IntegerOperand1);
					ProgramProfile->ReturnString->Copy(TempStr, 1);
				}
				break;





		// The sub-instruction codes used by wide string conversion / operations
			case HHPL_Inst_WideStringOperations:
		//		Param1:		Wide string conversion operations sub instruction
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "wide string operation", 1,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_WideStringOperationsSubInst_WideStringToBool:
					//		Param1:		Command that returns a wide string operand
							if(GetParameters(ProgramProfile, IPB, "wide string to bool conversion", 1,
										HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
								ProgramProfile->ReturnBool =
												(ProgramProfile->ReturnWideString->Size) ? true : false;
							}
							break;

						case HHPL_WideStringOperationsSubInst_WideStringToInt:
					//		Param1:		Command that returns a wide string where N1 is the number of bytes in
					//					the command; this command will specify the first (and only) operand
					//		Param2:		Instruction that will set a numeric variable
							if(GetParameters(ProgramProfile, IPB, "wide string to int conversion", 1,
										HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
							// We successfully executed the instruction; now, convert the wide string to
							// an integer.
								int		NumberOfCharactersUsedInConversion;
								int		ConvertedNumber;
								ConvertedNumber = ProgramProfile->ReturnWideString->ConvertToNumber
															(0, &NumberOfCharactersUsedInConversion);

							// We have a number.  If the next instruction sets an integer variable,
							// then store the number of characters used in conversion as the return
							// number and execute the instruction.
								IntegerRegisters[0] = NumberOfCharactersUsedInConversion;
								if(GetParameters(ProgramProfile, IPB, "wide string to int conversion", 1,
												HHPL_PROC_SET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0, 0))
								{
								// We dumped successfully; store the converted number as the return
								// number.
									ProgramProfile->ReturnNumber = ConvertedNumber;
								}
							}
							break;

						case HHPL_WideStringOperationsSubInst_WideStringToString:
					//		Param1:		Command that returns a wide string operand
							if(GetParameters(ProgramProfile, IPB, "wide string to string conversion", 1,
										HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
								ProgramProfile->ReturnWideString->ConvertAndCopyToHString( ProgramProfile->ReturnString );
							}
							break;

						case HHPL_WideStringOperationsSubInst_IntToWideString:
					//		Param1:		Command that returns a numeric operand
							if(GetParameters(ProgramProfile, IPB, "int to wide string", 1,
										HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
								swprintf( wchTempStr,160, INT_WIDE_STRING_TO_INT_SPEC, IPB->IntegerOperand1 );
								ProgramProfile->ReturnWideString->Copy( wchTempStr );
							}
							break;

						case HHPL_WideStringOperationsSubInst_BoolToWideString:
					//		Param1:		Command that returns a boolean operand
							if(GetParameters(ProgramProfile, IPB, "bool to wide string", 1,
										HHPL_PROC_GET_BOOLEAN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
							// We successfully executed the instruction; now, convert the boolean to
							// a "TRUE" or "FALSE" string
								if(IPB->BooleanOperand1)
								{
									ProgramProfile->ReturnWideString->Copy( L"TRUE" );
								}
								else
								{
									ProgramProfile->ReturnWideString->Copy( L"FALSE" );
								}
							}
							break;

						case HHPL_WideStringOperationsSubInst_StringToWideString:
					//		Param1:		Command that returns a string operand
							if(GetParameters(ProgramProfile, IPB, "string to wide string conversion", 1,
										HHPL_PROC_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
								ProgramProfile->ReturnWideString->ConvertAndCopyFromHString( ProgramProfile->ReturnString );
							}
							break;

						case HHPL_WideStringOperationsSubInst_LengthWideString:
					//		Param1:		Command that returns a wide string operand
							if(GetParameters(ProgramProfile, IPB, "length wide string", 1,
										HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
								ProgramProfile->ReturnNumber =
												ProgramProfile->ReturnWideString->Size;
							}
							break;

						case HHPL_WideStringOperationsSubInst_SetWideStringLength:
					//		Param1:		Address of wide string variable
					//		Param2:		Command that returns a numeric operand
							if(GetParameters(ProgramProfile, IPB, "set wide string length", 2,
										HHPL_PROC_DATA_ADDRESS_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
										0, 0, 0, 0, 0))
							{
								if(!SkipOverInstructionsWithoutExecutingThem)
								{
								// We successfully executed the instruction; now, set the string length
									MaximumStringSize =
										((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1]) +
										((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + 1]) * 256;

									if((IPB->IntegerOperand1 >= 0) &&
											(IPB->IntegerOperand1 <= ((int) MaximumStringSize)) &&
											(( IPB->DataAddress1 + 4 + ( MaximumStringSize * 2 ))
														<= ProgramProfile->SizeOfDataSpace) )
									{
									// The new size is OK.
										ProgramProfile->DataSpace[IPB->DataAddress1 + 2] =
														((unsigned char) (IPB->IntegerOperand1 & 0xFF));
										ProgramProfile->DataSpace[IPB->DataAddress1 + 3] =
														((unsigned char) ((IPB->IntegerOperand1 / 256) & 0xFF));
									}
									else
									{
									// New size is out of range; WOOF!
										ProgramProfile->UnrecoverableError = true;
										sprintf(TempStr, "New string length is out of wide string variable size range: IP = %08X",
												ProgramProfile->NextInstruction);
										LogError(TempStr);
									}
								}
							}
							break;

						case HHPL_WideStringOperationsSubInst_SetWideStringArrayLength:
					//		Param1:		Address of wide string array variable
					//		Param2:		Offset in wide string array variable
					//		Param3:		Command that returns a numeric operand
							if(GetParameters(ProgramProfile, IPB, "set wide string array length", 3,
										HHPL_PROC_DATA_ADDRESS_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
										HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0))
							{
								if(!SkipOverInstructionsWithoutExecutingThem)
								{
								// We successfully executed the instruction; now, set the string length
									MaximumStringSize =
										((unsigned int) ProgramProfile->DataSpace
														[IPB->DataAddress1 + IPB->IntegerOperand1]) +
										((unsigned int) ProgramProfile->DataSpace
														[IPB->DataAddress1 + IPB->IntegerOperand1 + 1]) * 256;

									if((IPB->IntegerOperand2 >= 0) &&
											(IPB->IntegerOperand2 <= ((int) MaximumStringSize)) &&
											(( IPB->DataAddress1 + IPB->IntegerOperand1 + 4 + ( MaximumStringSize * 2 ))
														<= ProgramProfile->SizeOfDataSpace) )
									{
									// The new size is OK.
										ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 2] =
														((unsigned char) (IPB->IntegerOperand2 & 0xFF));
										ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 3] =
														((unsigned char) ((IPB->IntegerOperand2 / 256) & 0xFF));
									}
									else
									{
									// New size is out of range; WOOF!
										ProgramProfile->UnrecoverableError = true;
										sprintf(TempStr, "New string length is out of wide string array variable size range: IP = %08X",
												ProgramProfile->NextInstruction);
										LogError(TempStr);
									}
								}
							}
							break;

						case HHPL_WideStringOperationsSubInst_WideCharToInt:
					//		Param1:		Command that returns a wide string operand
							if(GetParameters(ProgramProfile, IPB, "wide char to int", 1,
													HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
							// We must have at least one character.
								if(ProgramProfile->ReturnWideString->Size > 0)
								{
									ProgramProfile->ReturnNumber =
											((int) ProgramProfile->ReturnWideString->WChar[0]);
								}
								else
								{
								// Didn't find at least one character; WOOF!
									ProgramProfile->UnrecoverableError = true;
									sprintf(TempStr, "Wide string expression must contain at least one character for conversion: IP = %08X",
												ProgramProfile->NextInstruction);
									LogError(TempStr);
								}
							}
							break;

						case HHPL_WideStringOperationsSubInst_IntToWideChar:
					//		Param1:		Command that returns a numeric operand
							if(GetParameters(ProgramProfile, IPB, "int to wide char", 1,
										HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
								wchTempStr[0] = ((wchar_t) IPB->IntegerOperand1);
								ProgramProfile->ReturnWideString->Copy( wchTempStr, 1 );
							}
							break;

						case HHPL_WideStringOperationsSubInst_WideCharToChar:
					//		Param1:		Command that returns a wide string operand
							if(GetParameters(ProgramProfile, IPB, "wide char to char", 1,
													HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
							// We must have at least one character.
								if(ProgramProfile->ReturnWideString->Size > 0)
								{
									ProgramProfile->ReturnWideString->ConvertAndCopyToHString( ProgramProfile->ReturnString,
														0, 0 );
								}
								else
								{
								// Didn't find at least one character; WOOF!
									ProgramProfile->UnrecoverableError = true;
									sprintf(TempStr, "Wide string expression must contain at least one character for conversion: IP = %08X",
												ProgramProfile->NextInstruction);
									LogError(TempStr);
								}
							}
							break;

						case HHPL_WideStringOperationsSubInst_CharToWideChar:
					//		Param1:		Command that returns a string operand
							if(GetParameters(ProgramProfile, IPB, "char to wide char", 1,
										HHPL_PROC_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
							// We must have at least one character.
								if(ProgramProfile->ReturnString->Size > 0)
								{
									ProgramProfile->ReturnWideString->ConvertAndCopyFromHString( ProgramProfile->ReturnString,
														0, 0 );
								}
								else
								{
								// Didn't find at least one character; WOOF!
									ProgramProfile->UnrecoverableError = true;
									sprintf(TempStr, "String expression must contain at least one character for conversion: IP = %08X",
												ProgramProfile->NextInstruction);
									LogError(TempStr);
								}
							}
							break;

						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized wide string operation sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;





		// The sub-instruction codes used by wide string variable operations
			case HHPL_Inst_WideStringVariableOperations:
		//		Param1:		Wide string conversion variable operations sub instruction
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "wide string variable operation", 1,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_WideStringVariableOperationsSubInst_SetWideStringVariable:
					//		Param1:		Address of wide string variable
					//		Param2:		Command that returns a wide string operand
							if(GetParameters(ProgramProfile, IPB, "set wide string", 2,
										HHPL_PROC_DATA_ADDRESS_PARAMETER, HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0))
							{
								if(!SkipOverInstructionsWithoutExecutingThem)
								{
								// We successfully executed the instruction; now, store the string
									MaximumStringSize =
										((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1]) +
										((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + 1]) * 256;

									if(( IPB->DataAddress1 + 4 + ( MaximumStringSize * 2 ))
														> ProgramProfile->SizeOfDataSpace)
									{
										ProgramProfile->UnrecoverableError = true;
										sprintf(TempStr, "Wide string variable (%d max bytes) is outside data space: IP = %08X",
												MaximumStringSize,
												ProgramProfile->NextInstruction);
										LogError(TempStr);
									}
									else if(MaximumStringSize >= ProgramProfile->ReturnWideString->Size)
									{
									// The assignment size is OK.  Let's copy the stuff.
										ProgramProfile->ReturnWideString->CopyFromStringLittleEndian(&ProgramProfile->DataSpace[IPB->DataAddress1 + 4],
													MaximumStringSize);
										ProgramProfile->DataSpace[IPB->DataAddress1 + 2] =
														((unsigned char) (ProgramProfile->ReturnWideString->Size & 0xFF));
										ProgramProfile->DataSpace[IPB->DataAddress1 + 3] =
														((unsigned char) ((ProgramProfile->ReturnWideString->Size / 256) & 0xFF));
									}
									else
									{
									// Not enough room for string; WOOF!
										sprintf(TempStr, "Wide string variable (%d max bytes) too small for assignment (%d bytes): IP = %08X",
												MaximumStringSize,
												ProgramProfile->ReturnWideString->Size,
												ProgramProfile->NextInstruction);
										LogError(TempStr);

										ProgramProfile->ReturnWideString->Size = MaximumStringSize;
										ProgramProfile->ReturnWideString->CopyFromStringLittleEndian(&ProgramProfile->DataSpace[IPB->DataAddress1 + 4],
													MaximumStringSize);
										ProgramProfile->DataSpace[IPB->DataAddress1 + 2] =
														((unsigned char) (ProgramProfile->ReturnWideString->Size & 0xFF));
										ProgramProfile->DataSpace[IPB->DataAddress1 + 3] =
														((unsigned char) ((ProgramProfile->ReturnWideString->Size / 256) & 0xFF));
//										ProgramProfile->UnrecoverableError = true;
									}
								}
							}
							break;

						case HHPL_WideStringVariableOperationsSubInst_SetWideStringArrayVariable:
					//		Param1:		Address of wide string array variable
					//		Param2:		Offset in widestring array variable
					//		Param3:		Command that returns a wide string operand
							if(GetParameters(ProgramProfile, IPB, "set wide string array", 3,
										HHPL_PROC_DATA_ADDRESS_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
										HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0))
							{
								if(!SkipOverInstructionsWithoutExecutingThem)
								{
								// We successfully executed the instruction; now, store the widestring
									MaximumStringSize =
										((unsigned int) ProgramProfile->DataSpace
														[IPB->DataAddress1 + IPB->IntegerOperand1]) +
										((unsigned int) ProgramProfile->DataSpace
														[IPB->DataAddress1 + IPB->IntegerOperand1 + 1]) * 256;

									if(( IPB->DataAddress1 + IPB->IntegerOperand1 + 4 + ( MaximumStringSize * 2 ))
														> ProgramProfile->SizeOfDataSpace)
									{
										ProgramProfile->UnrecoverableError = true;
										sprintf(TempStr, "Wide string variable (%d max bytes) is outside data space: IP = %08X",
												MaximumStringSize,
												ProgramProfile->NextInstruction);
										LogError(TempStr);
									}
									else if(MaximumStringSize >= ProgramProfile->ReturnWideString->Size)
									{
									// The assignment size is OK.  Let's copy the stuff.
										ProgramProfile->ReturnWideString->CopyFromStringLittleEndian(&ProgramProfile->DataSpace
													[IPB->DataAddress1 + IPB->IntegerOperand1 + 4],
													MaximumStringSize);
										ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 2] =
														((unsigned char) (ProgramProfile->ReturnWideString->Size & 0xFF));
										ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 3] =
														((unsigned char) ((ProgramProfile->ReturnWideString->Size / 256) & 0xFF));
									}
									else
									{
									// Not enough room for wide string; WOOF!
										sprintf(TempStr, "Wide string variable (%d max bytes) too small for assignment (%d bytes): IP = %08X",
												MaximumStringSize,
												ProgramProfile->ReturnWideString->Size,
												ProgramProfile->NextInstruction);
										LogError(TempStr);

										ProgramProfile->ReturnWideString->Size = MaximumStringSize;
										ProgramProfile->ReturnWideString->CopyFromStringLittleEndian(
													&ProgramProfile->DataSpace
													[IPB->DataAddress1 + IPB->IntegerOperand1 + 4],
													MaximumStringSize);
										ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 2] =
														((unsigned char) (ProgramProfile->ReturnWideString->Size & 0xFF));
										ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 3] =
														((unsigned char) ((ProgramProfile->ReturnWideString->Size / 256) & 0xFF));
//										ProgramProfile->UnrecoverableError = true;
									}
								}
							}
							break;

						case HHPL_WideStringVariableOperationsSubInst_GetWideStringVariable:
					//		Param1:		Address of wide string variable
							if(GetParameters(ProgramProfile, IPB, "get wide string", 1,
										HHPL_PROC_DATA_ADDRESS_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
							// We successfully executed the instruction; now, get the wide string
							// Check the integrity of the string (must be at least 4 bytes: 2 for
							// length and 2 for max size).
								if((IPB->DataAddress1 + 3) < ((int) ProgramProfile->SizeOfDataSpace))
								{
								// Get the size and make sure that it is in the data space
									StringSize =
										((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + 2]) +
										((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + 3]) * 256;

									if((( StringSize * 2 ) + IPB->DataAddress1 + 4) <= ProgramProfile->SizeOfDataSpace)
									{
									// The assignment size is OK.  Let's copy the stuff.
										ProgramProfile->ReturnWideString->CopyLittleEndian(&ProgramProfile->DataSpace[IPB->DataAddress1 + 4],
												StringSize * 2 );
									}

									else
									{
									// Wide string is corrupt; WOOF!
										ProgramProfile->UnrecoverableError = true;
										sprintf(TempStr, "Wide string variable (%d bytes) extends past end of data space: IP = %08X",
													StringSize,
													ProgramProfile->NextInstruction);
										LogError(TempStr);
									}
								}
								else
								{
								// The offset to the string variable is beyond the end of the HHPL
								// code.  WOOF WOOF WOOF...
									ProgramProfile->UnrecoverableError = true;
									sprintf(TempStr, "Wide string variable location is beyond end of data space: IP = %08X",
													ProgramProfile->NextInstruction);
									LogError(TempStr);
								}
							}
							break;


						case HHPL_WideStringVariableOperationsSubInst_GetWideStringArrayVariable:
					//		Param1:		Address of wide string array variable
					//		Param2:		Offset in wide string array variable
							if(GetParameters(ProgramProfile, IPB, "get wide string array", 2,
									HHPL_PROC_DATA_ADDRESS_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
										0, 0, 0, 0, 0))
							{
							// We successfully executed the instruction; now, get the wide string
							// Check the integrity of the string (must be at least 4 bytes: 2 for
							// length and 2 for max size).
								if((IPB->DataAddress1 + IPB->IntegerOperand1 + 3) <
												((int) ProgramProfile->SizeOfDataSpace))
								{
								// Get the size and make sure that it is in the data space
									StringSize =
										((unsigned int) ProgramProfile->DataSpace
												[IPB->DataAddress1 + IPB->IntegerOperand1 + 2]) +
										((unsigned int) ProgramProfile->DataSpace
												[IPB->DataAddress1 + IPB->IntegerOperand1 + 3]) * 256;

									if((( StringSize * 2 ) + IPB->DataAddress1 + IPB->IntegerOperand1 + 4) <=
												ProgramProfile->SizeOfDataSpace)
									{
									// The assignment size is OK.  Let's copy the stuff.
										ProgramProfile->ReturnWideString->CopyLittleEndian(
												&ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 4],
															StringSize * 2 );
									}

									else
									{
									// Wide string is corrupt; WOOF!
										ProgramProfile->UnrecoverableError = true;
										sprintf(TempStr, "Wide string variable (%d bytes) extends past end of data space: IP = %08X",
													StringSize,
													ProgramProfile->NextInstruction);
										LogError(TempStr);
									}
								}
								else
								{
								// The offset to the wide string variable is beyond the end of the HHPL
								// code.  WOOF WOOF WOOF...
									ProgramProfile->UnrecoverableError = true;
									sprintf(TempStr, "Wide string variable location is beyond end of data space: IP = %08X",
													ProgramProfile->NextInstruction);
									LogError(TempStr);
								}
							}
							break;

						case HHPL_WideStringVariableOperationsSubInst_WideStringDataInRange:
					//		Param1:		Address of wide string variable
					//		Param2:		Command that returns a range operand
							if(GetParameters(ProgramProfile, IPB, "data in range", 2,
										HHPL_PROC_DATA_ADDRESS_PARAMETER, HHPL_PROC_GET_RANGE_PARAMETER, 0, 0, 0, 0, 0))
							{
							// We successfully acquired the parameters.
							// Check the integrity of the wide string (must be at least 4 bytes: 2 for
							// length and 2 for max size).
								if((IPB->DataAddress1 + 3) < ((int) ProgramProfile->SizeOfDataSpace))
								{
								// Get the size and make sure that it is in the data space
									StringSize =
										((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + 2]) +
										((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + 3]) * 256;

									if((( StringSize  * 2 ) + IPB->DataAddress1 + 4) <= ProgramProfile->SizeOfDataSpace)
									{
										if( IPB->RangeOperand1.Begin < 0 )    IPB->RangeOperand1.Begin += (int) StringSize;
										if( IPB->RangeOperand1.End < 0 )    IPB->RangeOperand1.End += (int) StringSize;
										if((IPB->RangeOperand1.Begin < ((int) StringSize)) &&
													 (IPB->RangeOperand1.End < ((int) StringSize)))
										{
											unsigned int	AdjustedRangeBegin, AdjustedRangeEnd;
											AdjustedRangeBegin = (unsigned int) IPB->RangeOperand1.Begin;
											AdjustedRangeEnd = (unsigned int) IPB->RangeOperand1.End;

											if( AdjustedRangeBegin <= AdjustedRangeEnd )
											{
												AdjustedRangeBegin = AdjustedRangeBegin * 2;
												AdjustedRangeEnd = AdjustedRangeEnd * 2 + 1;
											}
											else
											{
												AdjustedRangeBegin = AdjustedRangeBegin * 2 + 1;
												AdjustedRangeEnd = AdjustedRangeEnd * 2;
											}

											ProgramProfile->ReturnWideString->CopyLittleEndian(
													&ProgramProfile->DataSpace[IPB->DataAddress1 + 4],
													StringSize * 2,
													AdjustedRangeBegin,
													AdjustedRangeEnd);
											ProgramProfile->ReturnBool =
														(ProgramProfile->ReturnWideString->Size) ? true : false;
										}
										else
										{
											ProgramProfile->ReturnWideString->Copy( L"" );
											ProgramProfile->ReturnBool = false;
										}
									}

									else
									{
									// Wide string is corrupt; WOOF!
										ProgramProfile->UnrecoverableError = true;
										sprintf(TempStr, "Wide string variable (%d bytes) extends past end of data space: IP = %08X",
													StringSize,
													ProgramProfile->NextInstruction);
										LogError(TempStr);
									}
								}
								else
								{
								// The offset to the wide string variable is beyond the end of the HHPL
								// code.  WOOF WOOF WOOF...
									ProgramProfile->UnrecoverableError = true;
									sprintf(TempStr, "Wide string variable location is beyond end of data space: IP = %08X",
													ProgramProfile->NextInstruction);
									LogError(TempStr);
								}
							}
							break;

						case HHPL_WideStringVariableOperationsSubInst_WideStringSetArrayDataInRange:
					//		Param1:		Address of wide string array variable
					//		Param2:		Offset in wide string array variable
					//		Param3:		Command that returns a range operand
					//		Param4:		Command that returns a wide string operand
							if(GetParameters(ProgramProfile, IPB, "set array data in range", 4,
										HHPL_PROC_DATA_ADDRESS_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
										HHPL_PROC_GET_RANGE_PARAMETER, HHPL_PROC_GET_WIDE_STRING_PARAMETER, 0, 0, 0))
							{
							// We successfully acquired the parameters.  Now, copy the original
							// wide string to ReturnWideString.  If that works, we'll copy the wide string operand
							// to the specified range and then write the result back to the wide string
							// variable.
								if(!SkipOverInstructionsWithoutExecutingThem)
								{
								// Check the integrity of the wide string (must be at least 4 bytes: 2 for
								// length and 2 for max size).
									if((IPB->DataAddress1 + IPB->IntegerOperand1 + 3) <
											((int) ProgramProfile->SizeOfDataSpace))
									{
									// Get the size and make sure that it is in the data space
										StringSize =
											((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 2]) +
											((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 3]) * 256;

										if((( StringSize * 2 ) + IPB->DataAddress1 + IPB->IntegerOperand1 + 4)
														<= ProgramProfile->SizeOfDataSpace)
										{
										// The assignment size is OK.  Let's copy the return wide string.
											ProgramProfile->ReturnWideString->CopyLittleEndian(
													&ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 4],
															StringSize * 2 );


										// Let's make a copy to the specified range.
											if(ProgramProfile->ReturnWideString->CopyToRange(((unsigned int) IPB->RangeOperand1.Begin), ((unsigned int) IPB->RangeOperand1.End),
															IPB->WideStringOperand1))
											{
											// We successfully copied the range; now, store the wide string
												MaximumStringSize =
													((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1]) +
													((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 1]) * 256;

												if(( IPB->DataAddress1 + IPB->IntegerOperand1 + 4 + ( MaximumStringSize * 2 ))
																> ProgramProfile->SizeOfDataSpace)
												{
													ProgramProfile->UnrecoverableError = true;
													sprintf(TempStr, "Wide string variable (%d max bytes) is outside data space: IP = %08X",
														MaximumStringSize,
														ProgramProfile->NextInstruction);
													LogError(TempStr);
												}
												else if(MaximumStringSize >= ProgramProfile->ReturnWideString->Size)
												{
												// The assignment size is OK.  Let's copy the stuff.
													ProgramProfile->ReturnWideString->CopyFromStringLittleEndian(
															&ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 4],
															MaximumStringSize);
													ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 2] =
														((unsigned char) (ProgramProfile->ReturnWideString->Size & 0xFF));
													ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 3] =
														((unsigned char) ((ProgramProfile->ReturnWideString->Size / 256) & 0xFF));
												}
												else
												{
												// Not enough room for string; WOOF!
													ProgramProfile->ReturnWideString->Size = MaximumStringSize;
													ProgramProfile->ReturnWideString->CopyFromStringLittleEndian(
															&ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 4],
															MaximumStringSize);
													ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 2] =
														((unsigned char) (ProgramProfile->ReturnWideString->Size & 0xFF));
													ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 3] =
														((unsigned char) ((ProgramProfile->ReturnWideString->Size / 256) & 0xFF));
//													ProgramProfile->UnrecoverableError = true;
													sprintf(TempStr, "Wide string variable (%d max bytes) too small for assignment (%d bytes): IP = %08X",
														MaximumStringSize,
														IPB->WideStringOperand1->Size,
														ProgramProfile->NextInstruction);
													LogError(TempStr);
												}
											}
											else
											{
											// Range is invalid; WOOF!
												ProgramProfile->UnrecoverableError = true;
												sprintf(TempStr, "Range in SetRange is invalid : IP = %08X",
														ProgramProfile->NextInstruction);
												LogError(TempStr);
											}
										}

										else
										{
										// Wide string is corrupt; WOOF!
											ProgramProfile->UnrecoverableError = true;
											sprintf(TempStr, "Wide string variable (%d bytes) extends past end of data space: IP = %08X",
													StringSize,
													ProgramProfile->NextInstruction);
											LogError(TempStr);
										}
									}
									else
									{
									// The offset to the wide string variable is beyond the end of the HHPL
									// code.  WOOF WOOF WOOF...
										ProgramProfile->UnrecoverableError = true;
										sprintf(TempStr, "Wide string variable location is beyond end of data space: IP = %08X",
															ProgramProfile->NextInstruction);
										LogError(TempStr);
									}
								}
							}
							break;

						case HHPL_WideStringVariableOperationsSubInst_WideStringArrayDataInRange:
					//		Param1:		Address of wide string array variable
					//		Param2:		Offset in wide string array variable
					//		Param3:		Command that returns a range operand
							if(GetParameters(ProgramProfile, IPB, "array data in range", 3,
										HHPL_PROC_DATA_ADDRESS_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
										HHPL_PROC_GET_RANGE_PARAMETER, 0, 0, 0, 0))
							{
							// We successfully acquired the parameters.
							// Check the integrity of the wide string (must be at least 4 bytes: 2 for
							// length and 2 for max size).
								if((IPB->DataAddress1 + IPB->IntegerOperand1 + 3) <
										((int) ProgramProfile->SizeOfDataSpace))
								{
								// Get the size and make sure that it is in the data space
									StringSize =
										((unsigned int) ProgramProfile->DataSpace
												[IPB->DataAddress1 + IPB->IntegerOperand1 + 2]) +
										((unsigned int) ProgramProfile->DataSpace
												[IPB->DataAddress1 + IPB->IntegerOperand1 + 3]) * 256;

									if((( StringSize * 2 ) + IPB->DataAddress1 + IPB->IntegerOperand1 + 4) <=
												ProgramProfile->SizeOfDataSpace)
									{
										if( IPB->RangeOperand1.Begin < 0 )    IPB->RangeOperand1.Begin += (int) StringSize;
										if( IPB->RangeOperand1.End < 0 )    IPB->RangeOperand1.End += (int) StringSize;
										if((IPB->RangeOperand1.Begin < ((int) StringSize)) &&
													 (IPB->RangeOperand1.End < ((int) StringSize)))
										{
											unsigned int	AdjustedRangeBegin, AdjustedRangeEnd;
											AdjustedRangeBegin = (unsigned int) IPB->RangeOperand1.Begin;
											AdjustedRangeEnd = (unsigned int) IPB->RangeOperand1.End;

											if( AdjustedRangeBegin <= AdjustedRangeEnd )
											{
												AdjustedRangeBegin = AdjustedRangeBegin * 2;
												AdjustedRangeEnd = AdjustedRangeEnd * 2 + 1;
											}
											else
											{
												AdjustedRangeBegin = AdjustedRangeBegin * 2 + 1;
												AdjustedRangeEnd = AdjustedRangeEnd * 2;
											}

											ProgramProfile->ReturnWideString->CopyLittleEndian(
													&ProgramProfile->DataSpace
															[IPB->DataAddress1 + IPB->IntegerOperand1 + 4],
													StringSize * 2,
													AdjustedRangeBegin,
													AdjustedRangeEnd );
											ProgramProfile->ReturnBool =
														(ProgramProfile->ReturnWideString->Size) ? true : false;
										}
										else
										{
											ProgramProfile->ReturnWideString->Copy( L"" );
											ProgramProfile->ReturnBool = false;
										}
									}

									else
									{
									// Wide string is corrupt; WOOF!
										ProgramProfile->UnrecoverableError = true;
										sprintf(TempStr, "Wide string array variable (%d bytes) extends past end of data space: IP = %08X",
													StringSize,
													ProgramProfile->NextInstruction);
										LogError(TempStr);
									}
								}
								else
								{
								// The offset to the wide string variable is beyond the end of the HHPL
								// code.  WOOF WOOF WOOF...
									ProgramProfile->UnrecoverableError = true;
									sprintf(TempStr, "Wide string array variable location is beyond end of data space: IP = %08X",
													ProgramProfile->NextInstruction);
									LogError(TempStr);
								}
							}
							break;

						case HHPL_WideStringVariableOperationsSubInst_WideStringSetDataInRange:
					//		Param1:		Address of wide string variable
					//		Param2:		Command that returns a range operand
					//		Param3:		Command that returns a wide string operand
							if(GetParameters(ProgramProfile, IPB, "set data in range", 3,
										HHPL_PROC_DATA_ADDRESS_PARAMETER, HHPL_PROC_GET_RANGE_PARAMETER,
										HHPL_PROC_GET_WIDE_STRING_PARAMETER, 0, 0, 0, 0))
							{
							// We successfully acquired the parameters.  Now, copy the original
							// wide string to ReturnWideString.  If that works, we'll copy the wide string operand
							// to the specified range and then write the result back to the wide string
							// variable.
								if(!SkipOverInstructionsWithoutExecutingThem)
								{
								// Check the integrity of the wide string (must be at least 4 bytes: 2 for
								// length and 2 for max size).
									if((IPB->DataAddress1 + 3) < ((int) ProgramProfile->SizeOfDataSpace))
									{
									// Get the size and make sure that it is in the data space
										StringSize =
											((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + 2]) +
											((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + 3]) * 256;

										if((( StringSize * 2 ) + IPB->DataAddress1 + 4) <= ProgramProfile->SizeOfDataSpace)
										{
										// The assignment size is OK.  Let's copy the return wide string.
											ProgramProfile->ReturnWideString->CopyLittleEndian(
													&ProgramProfile->DataSpace[IPB->DataAddress1 + 4],
															StringSize * 2);


										// Let's make a copy to the specified range.
											if(ProgramProfile->ReturnWideString->CopyToRange(((unsigned int) IPB->RangeOperand1.Begin), ((unsigned int) IPB->RangeOperand1.End),
																		IPB->WideStringOperand1))
											{
											// We successfully copied the range; now, store the string
												MaximumStringSize =
													((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1]) +
													((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + 1]) * 256;

												if(( IPB->DataAddress1 + 4 + ( MaximumStringSize * 2 ))
																> ProgramProfile->SizeOfDataSpace)
												{
													ProgramProfile->UnrecoverableError = true;
													sprintf(TempStr, "Wide string variable (%d max bytes) is outside data space: IP = %08X",
														MaximumStringSize,
														ProgramProfile->NextInstruction);
													LogError(TempStr);
												}
												else if(MaximumStringSize >= ProgramProfile->ReturnWideString->Size)
												{
												// The assignment size is OK.  Let's copy the stuff.
													ProgramProfile->ReturnWideString->CopyFromStringLittleEndian(
															&ProgramProfile->DataSpace[IPB->DataAddress1 + 4],
															MaximumStringSize);
													ProgramProfile->DataSpace[IPB->DataAddress1 + 2] =
														((unsigned char) (ProgramProfile->ReturnWideString->Size & 0xFF));
													ProgramProfile->DataSpace[IPB->DataAddress1 + 3] =
														((unsigned char) ((ProgramProfile->ReturnWideString->Size / 256) & 0xFF));
												}
												else
												{
												// Not enough room for wide string; WOOF!
													ProgramProfile->ReturnWideString->Size = MaximumStringSize;
													ProgramProfile->ReturnWideString->CopyFromStringLittleEndian
														(&ProgramProfile->DataSpace[IPB->DataAddress1 + 4],
															MaximumStringSize);
													ProgramProfile->DataSpace[IPB->DataAddress1 + 2] =
														((unsigned char) (ProgramProfile->ReturnWideString->Size & 0xFF));
													ProgramProfile->DataSpace[IPB->DataAddress1 + 3] =
														((unsigned char) ((ProgramProfile->ReturnWideString->Size / 256) & 0xFF));
//													ProgramProfile->UnrecoverableError = true;
													sprintf(TempStr, "Wide string variable (%d max bytes) too small for assignment (%d bytes): IP = %08X",
														MaximumStringSize,
														IPB->WideStringOperand1->Size,
														ProgramProfile->NextInstruction);
													LogError(TempStr);
												}
											}
											else
											{
											// Range is invalid; WOOF!
												ProgramProfile->UnrecoverableError = true;
												sprintf(TempStr, "Range in SetRange is invalid : IP = %08X",
														ProgramProfile->NextInstruction);
												LogError(TempStr);
											}
										}

										else
										{
										// Wide string is corrupt; WOOF!
											ProgramProfile->UnrecoverableError = true;
											sprintf(TempStr, "Wide string variable (%d bytes) extends past end of data space: IP = %08X",
													StringSize,
													ProgramProfile->NextInstruction);
											LogError(TempStr);
										}
									}
									else
									{
									// The offset to the wide string variable is beyond the end of the HHPL
									// code.  WOOF WOOF WOOF...
										ProgramProfile->UnrecoverableError = true;
										sprintf(TempStr, "Wide string variable location is beyond end of data space: IP = %08X",
													ProgramProfile->NextInstruction);
										LogError(TempStr);
									}
								}
							}
							break;

						case HHPL_WideStringVariableOperationsSubInst_WideStringRegister:
					//		Param1:		Wide string register ID (sub instruction)
							if(GetParameters(ProgramProfile, IPB, "wide string register", 1,
										HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
								ProgramProfile->ReturnWideString->Copy(WideStringRegisters[IPB->SubinstructionOperand1]);
							}
							break;

						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized wide string variable operation sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;





			case HHPL_Inst_NoOp:
		// No-op
				++ProgramProfile->NextInstruction;
				break;


			case HHPL_Inst_BooleanRegister:
		//		Param1:		Boolean register ID (sub instruction)
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "boolean register", 1,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
					ProgramProfile->ReturnBool = BooleanRegisters[IPB->SubinstructionOperand1];
				}
				break;


			case HHPL_Inst_IntegerRegister:
		//		Param1:		Integer register ID (sub instruction)
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "integer register", 1,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
					ProgramProfile->ReturnNumber = IntegerRegisters[IPB->SubinstructionOperand1];
				}
				break;


			case HHPL_Inst_StringRegister:
		//		Param1:		String register ID (sub instruction)
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "string register", 1,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
//					ProgramProfile->ReturnString->Copy(StringRegisters[IPB->SubinstructionOperand1]);
					WideStringRegisters[IPB->SubinstructionOperand1]->ConvertAndCopyToHString( ProgramProfile->ReturnString );
				}
				break;



			case HHPL_Inst_SetBooleanVariable:
		//		Param1:		Address of boolean variable
		//		Param2:		Command that returns a boolean operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "set bool", 2,
							HHPL_PROC_DATA_ADDRESS_PARAMETER, HHPL_PROC_GET_BOOLEAN_PARAMETER, 0, 0, 0, 0, 0))
				{
					if(!SkipOverInstructionsWithoutExecutingThem)
					{
					// We successfully executed the instruction; now, store the boolean
						ProgramProfile->DataSpace[IPB->DataAddress1] =
															IPB->BooleanOperand1;
					}
				}
				break;


			case HHPL_Inst_SetBooleanArrayVariable:
		//		Param1:		Base address of boolean array variable
		//		Param2:		Offset in boolean array variable
		//		Param3:		Command that returns a boolean operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "set bool array ", 3,
							HHPL_PROC_DATA_ADDRESS_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
							HHPL_PROC_GET_BOOLEAN_PARAMETER, 0, 0, 0, 0))
				{
					if(!SkipOverInstructionsWithoutExecutingThem)
					{
					// We successfully executed the instruction; now, store the boolean
						ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1] =
															IPB->BooleanOperand1;
					}
				}
				break;


			case HHPL_Inst_SetIntegerVariable:
		//		Param1:		Address of integer variable
		//		Param2:		Command that returns an integer operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "set number", 2,
							HHPL_PROC_DATA_ADDRESS_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0))
				{
					if(!SkipOverInstructionsWithoutExecutingThem)
					{
					// We successfully executed the instruction; now, store the number
						ProgramProfile->DataSpace[IPB->DataAddress1] =
											((unsigned char) (IPB->IntegerOperand1 & 0xFF));
						ProgramProfile->DataSpace[IPB->DataAddress1 + 1] =
											((unsigned char) ((IPB->IntegerOperand1 >> 8) & 0xFF));
						ProgramProfile->DataSpace[IPB->DataAddress1 + 2] =
											((unsigned char) ((IPB->IntegerOperand1 >> 16) & 0xFF));
						ProgramProfile->DataSpace[IPB->DataAddress1 + 3] =
											((unsigned char) ((IPB->IntegerOperand1 >> 24) & 0xFF));
					}
				}
				break;


			case HHPL_Inst_SetIntegerArrayVariable:
		//		Param1:		Address of integer array variable
		//		Param2:		Offset in integer array variable
		//		Param3:		Command that returns an integer operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "set number array", 3,
							HHPL_PROC_DATA_ADDRESS_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
							HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0))
				{
					if(!SkipOverInstructionsWithoutExecutingThem)
					{
					// We successfully executed the instruction; now, store the number
						ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1] =
											((unsigned char) (IPB->IntegerOperand2 & 0xFF));
						ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 1] =
											((unsigned char) ((IPB->IntegerOperand2 >> 8) & 0xFF));
						ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 2] =
											((unsigned char) ((IPB->IntegerOperand2 >> 16) & 0xFF));
						ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 3] =
											((unsigned char) ((IPB->IntegerOperand2 >> 24) & 0xFF));
					}
				}
				break;



			case HHPL_Inst_SetStringVariable:
		//		Param1:		Address of string variable
		//		Param2:		Command that returns a string operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "set string", 2,
							HHPL_PROC_DATA_ADDRESS_PARAMETER, HHPL_PROC_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0))
				{
					if(!SkipOverInstructionsWithoutExecutingThem)
					{
					// We successfully executed the instruction; now, store the string
						MaximumStringSize =
							((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1]) +
							((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + 1]) * 256;

						if(MaximumStringSize >= ProgramProfile->ReturnString->Size)
						{
						// The assignment size is OK.  Let's copy the stuff.
							ProgramProfile->ReturnString->CopyFromString(&ProgramProfile->DataSpace[IPB->DataAddress1 + 4],
										MaximumStringSize);
							ProgramProfile->DataSpace[IPB->DataAddress1 + 2] =
											((unsigned char) (ProgramProfile->ReturnString->Size & 0xFF));
							ProgramProfile->DataSpace[IPB->DataAddress1 + 3] =
											((unsigned char) ((ProgramProfile->ReturnString->Size / 256) & 0xFF));
						}
						else
						{
						// Not enough room for string; WOOF!
							sprintf(TempStr, "String variable (%d max bytes) too small for assignment (%d bytes): IP = %08X",
									MaximumStringSize,
									ProgramProfile->ReturnString->Size,
									ProgramProfile->NextInstruction);
							LogError(TempStr);

							ProgramProfile->ReturnString->Size = MaximumStringSize;
							ProgramProfile->ReturnString->CopyFromString(&ProgramProfile->DataSpace[IPB->DataAddress1 + 4],
										MaximumStringSize);
							ProgramProfile->DataSpace[IPB->DataAddress1 + 2] =
											((unsigned char) (ProgramProfile->ReturnString->Size & 0xFF));
							ProgramProfile->DataSpace[IPB->DataAddress1 + 3] =
											((unsigned char) ((ProgramProfile->ReturnString->Size / 256) & 0xFF));
//							ProgramProfile->UnrecoverableError = true;
						}
					}
				}
				break;



			case HHPL_Inst_SetStringArrayVariable:
		//		Param1:		Address of string array variable
		//		Param2:		Offset in string array variable
		//		Param3:		Command that returns a string operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "set string array", 3,
							HHPL_PROC_DATA_ADDRESS_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
							HHPL_PROC_STRING_RETURN_PARAMETER, 0, 0, 0, 0))
				{
					if(!SkipOverInstructionsWithoutExecutingThem)
					{
					// We successfully executed the instruction; now, store the string
						MaximumStringSize =
							((unsigned int) ProgramProfile->DataSpace
											[IPB->DataAddress1 + IPB->IntegerOperand1]) +
							((unsigned int) ProgramProfile->DataSpace
											[IPB->DataAddress1 + IPB->IntegerOperand1 + 1]) * 256;

						if(MaximumStringSize >= ProgramProfile->ReturnString->Size)
						{
						// The assignment size is OK.  Let's copy the stuff.
							ProgramProfile->ReturnString->CopyFromString(&ProgramProfile->DataSpace
													[IPB->DataAddress1 + IPB->IntegerOperand1 + 4],
										MaximumStringSize);
							ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 2] =
											((unsigned char) (ProgramProfile->ReturnString->Size & 0xFF));
							ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 3] =
											((unsigned char) ((ProgramProfile->ReturnString->Size / 256) & 0xFF));
						}
						else
						{
						// Not enough room for string; WOOF!
							sprintf(TempStr, "String variable (%d max bytes) too small for assignment (%d bytes): IP = %08X",
									MaximumStringSize,
									ProgramProfile->ReturnString->Size,
									ProgramProfile->NextInstruction);
							LogError(TempStr);

							ProgramProfile->ReturnString->Size = MaximumStringSize;
							ProgramProfile->ReturnString->CopyFromString(
										&ProgramProfile->DataSpace
													[IPB->DataAddress1 + IPB->IntegerOperand1 + 4],
										MaximumStringSize);
							ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 2] =
											((unsigned char) (ProgramProfile->ReturnString->Size & 0xFF));
							ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 3] =
											((unsigned char) ((ProgramProfile->ReturnString->Size / 256) & 0xFF));
//							ProgramProfile->UnrecoverableError = true;
						}
					}
				}
				break;




			case HHPL_Inst_GetBooleanVariable:
		//		Param1:		Address of boolean variable
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "get bool", 1,
									HHPL_PROC_DATA_ADDRESS_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// We successfully executed the instruction; now, get the boolean
					ProgramProfile->ReturnBool =
						(ProgramProfile->DataSpace[IPB->DataAddress1] == 0) ? false : true;
				}
				break;


			case HHPL_Inst_GetBooleanArrayVariable:
		//		Param1:		Address of boolean array variable
		//		Param2:		Offset in boolean array variable
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "get bool array", 2,
						HHPL_PROC_DATA_ADDRESS_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
									0, 0, 0, 0, 0))
				{
				// We successfully executed the instruction; now, get the boolean
					ProgramProfile->ReturnBool =
						(ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1] == 0)
														? false : true;
				}
				break;


			case HHPL_Inst_GetIntegerVariable:
		//		Param1:		Address of integer variable
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "get number", 1,
									HHPL_PROC_DATA_ADDRESS_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// We successfully executed the instruction; now, get the number
					ProgramProfile->ReturnNumber = 0;
					ProgramProfile->ReturnNumber |=
						((unsigned int )ProgramProfile->DataSpace[IPB->DataAddress1]) |
						(((unsigned int )ProgramProfile->DataSpace[IPB->DataAddress1 + 1]) << 8) |
						(((unsigned int )ProgramProfile->DataSpace[IPB->DataAddress1 + 2]) << 16) |
						(((unsigned int )ProgramProfile->DataSpace[IPB->DataAddress1 + 3]) << 24);
				}
				break;


			case HHPL_Inst_GetIntegerArrayVariable:
		//		Param1:		Address of integer array variable
		//		Param2:		Offset in integer array variable
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "get number array", 2,
						HHPL_PROC_DATA_ADDRESS_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
									0, 0, 0, 0, 0))
				{
				// We successfully executed the instruction; now, get the number
					ProgramProfile->ReturnNumber = 0;
					ProgramProfile->ReturnNumber |=
						((unsigned int )ProgramProfile->DataSpace
											[IPB->DataAddress1 + IPB->IntegerOperand1]) |
						(((unsigned int )ProgramProfile->DataSpace
											[IPB->DataAddress1 + IPB->IntegerOperand1 + 1]) << 8) |
						(((unsigned int )ProgramProfile->DataSpace
											[IPB->DataAddress1 + IPB->IntegerOperand1 + 2]) << 16) |
						(((unsigned int )ProgramProfile->DataSpace
											[IPB->DataAddress1 + IPB->IntegerOperand1 + 3]) << 24);
				}
				break;


			case HHPL_Inst_GetStringVariable:
		//		Param1:		Address of string variable
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "get string", 1,
										HHPL_PROC_DATA_ADDRESS_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// We successfully executed the instruction; now, get the string
				// Check the integrity of the string (must be at least 4 bytes: 2 for
				// length and 2 for max size).
					if((IPB->DataAddress1 + 3) < ((int) ProgramProfile->SizeOfDataSpace))
					{
					// Get the size and make sure that it is in the data space
						StringSize =
							((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + 2]) +
							((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + 3]) * 256;

						if((StringSize + IPB->DataAddress1 + 4) <= ProgramProfile->SizeOfDataSpace)
						{
						// The assignment size is OK.  Let's copy the stuff.
							ProgramProfile->ReturnString->Copy(&ProgramProfile->DataSpace[IPB->DataAddress1 + 4],
												StringSize);
						}

						else
						{
						// String is corrupt; WOOF!
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "String variable (%d bytes) extends past end of data space: IP = %08X",
													StringSize,
													ProgramProfile->NextInstruction);
							LogError(TempStr);
						}
					}
					else
					{
					// The offset to the string variable is beyond the end of the HHPL
					// code.  WOOF WOOF WOOF...
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "String variable location is beyond end of data space: IP = %08X",
													ProgramProfile->NextInstruction);
						LogError(TempStr);
					}
				}
				break;


			case HHPL_Inst_GetStringArrayVariable:
		//		Param1:		Address of string array variable
		//		Param2:		Offset in string array variable
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "get string array", 2,
						HHPL_PROC_DATA_ADDRESS_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
									0, 0, 0, 0, 0))
				{
				// We successfully executed the instruction; now, get the string
				// Check the integrity of the string (must be at least 4 bytes: 2 for
				// length and 2 for max size).
					if((IPB->DataAddress1 + IPB->IntegerOperand1 + 3) <
												((int) ProgramProfile->SizeOfDataSpace))
					{
					// Get the size and make sure that it is in the data space
						StringSize =
							((unsigned int) ProgramProfile->DataSpace
												[IPB->DataAddress1 + IPB->IntegerOperand1 + 2]) +
							((unsigned int) ProgramProfile->DataSpace
												[IPB->DataAddress1 + IPB->IntegerOperand1 + 3]) * 256;

						if((StringSize + IPB->DataAddress1 + IPB->IntegerOperand1 + 4) <=
												ProgramProfile->SizeOfDataSpace)
						{
						// The assignment size is OK.  Let's copy the stuff.
							ProgramProfile->ReturnString->Copy(
									&ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 4],
												StringSize);
						}

						else
						{
						// String is corrupt; WOOF!
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "String variable (%d bytes) extends past end of data space: IP = %08X",
													StringSize,
													ProgramProfile->NextInstruction);
							LogError(TempStr);
						}
					}
					else
					{
					// The offset to the string variable is beyond the end of the HHPL
					// code.  WOOF WOOF WOOF...
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "String variable location is beyond end of data space: IP = %08X",
													ProgramProfile->NextInstruction);
						LogError(TempStr);
					}
				}
				break;


			case HHPL_Inst_DataInRange:
		//		Param1:		Address of string variable
		//		Param2:		Command that returns a range operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "data in range", 2,
							HHPL_PROC_DATA_ADDRESS_PARAMETER, HHPL_PROC_GET_RANGE_PARAMETER, 0, 0, 0, 0, 0))
				{
				// We successfully acquired the parameters.
				// Check the integrity of the string (must be at least 4 bytes: 2 for
				// length and 2 for max size).
					if((IPB->DataAddress1 + 3) < ((int) ProgramProfile->SizeOfDataSpace))
					{
					// Get the size and make sure that it is in the data space
						StringSize =
							((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + 2]) +
							((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + 3]) * 256;

						if((StringSize + IPB->DataAddress1 + 4) <= ProgramProfile->SizeOfDataSpace)
						{
							if( IPB->RangeOperand1.Begin < 0 )    IPB->RangeOperand1.Begin += (int) StringSize;
							if( IPB->RangeOperand1.End < 0 )    IPB->RangeOperand1.End += (int) StringSize;
							if((IPB->RangeOperand1.Begin < ((int) StringSize)) &&
										 (IPB->RangeOperand1.End < ((int) StringSize)))
							{
								ProgramProfile->ReturnString->Copy(
										((char *) (&ProgramProfile->DataSpace[IPB->DataAddress1 + 4])),
										StringSize,
										((unsigned int) IPB->RangeOperand1.Begin),
										((unsigned int) IPB->RangeOperand1.End));
								ProgramProfile->ReturnBool =
											(ProgramProfile->ReturnString->Size) ? true : false;
							}
							else
							{
								ProgramProfile->ReturnString->Copy("");
								ProgramProfile->ReturnBool = false;
							}
						}

						else
						{
						// String is corrupt; WOOF!
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "String variable (%d bytes) extends past end of data space: IP = %08X",
													StringSize,
													ProgramProfile->NextInstruction);
							LogError(TempStr);
						}
					}
					else
					{
					// The offset to the string variable is beyond the end of the HHPL
					// code.  WOOF WOOF WOOF...
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "String variable location is beyond end of data space: IP = %08X",
													ProgramProfile->NextInstruction);
						LogError(TempStr);
					}
				}
				break;


			case HHPL_Inst_DataInEditBufferRange:
		//		Param1:		Command that returns a range operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "data in edit buffer range", 1,
											HHPL_PROC_GET_RANGE_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// We successfully executed the instruction; now, get the edit buffer string range.
					if( IPB->RangeOperand1.Begin < 0 )    IPB->RangeOperand1.Begin += (int) ProgramProfile->EditBuffer->Size;
					if( IPB->RangeOperand1.End < 0 )    IPB->RangeOperand1.End += (int) ProgramProfile->EditBuffer->Size;
					if((IPB->RangeOperand1.Begin < ((int) ProgramProfile->EditBuffer->Size)) &&
						 (IPB->RangeOperand1.End < ((int) ProgramProfile->EditBuffer->Size)))
					{
						ProgramProfile->ReturnWideString->Copy(ProgramProfile->EditBuffer,
									((unsigned int) IPB->RangeOperand1.Begin), ((unsigned int) IPB->RangeOperand1.End));
						ProgramProfile->ReturnBool =
									(ProgramProfile->ReturnWideString->Size) ? true : false;
					}
					else
					{
						ProgramProfile->ReturnWideString->Copy( L"" );
						ProgramProfile->ReturnBool = false;
					}
				}
				break;


			case HHPL_Inst_DataInDataSourceIDRange:
		//		Param1:		Command that returns a range operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "data in data source ID range", 1,
											HHPL_PROC_GET_RANGE_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// We successfully executed the instruction; now, get the data source ID string range.
					if( IPB->RangeOperand1.Begin < 0 )    IPB->RangeOperand1.Begin += (int) ProgramProfile->DataSourceID->Size;
					if( IPB->RangeOperand1.End < 0 )    IPB->RangeOperand1.End += (int) ProgramProfile->DataSourceID->Size;
					if((IPB->RangeOperand1.Begin < ((int) ProgramProfile->DataSourceID->Size)) &&
						 (IPB->RangeOperand1.End < ((int) ProgramProfile->DataSourceID->Size)))
					{
						ProgramProfile->ReturnWideString->Copy(ProgramProfile->DataSourceID,
									((unsigned int) IPB->RangeOperand1.Begin), ((unsigned int) IPB->RangeOperand1.End));
						ProgramProfile->ReturnBool =
									(ProgramProfile->ReturnWideString->Size) ? true : false;
					}
					else
					{
						ProgramProfile->ReturnWideString->Copy( L"" );
						ProgramProfile->ReturnBool = false;
					}
				}
				break;


			case HHPL_Inst_DataInHostRange:
		//		Param1:		Command that returns a range operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "data in host range", 1,
											HHPL_PROC_GET_RANGE_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
					ProgramProfile->ReturnWideString->Size =
									((unsigned int) (GetFromHost(ProgramProfile->ReturnWideString->WChar)));

				// We successfully executed the instruction; now, get the host string range.
					if( IPB->RangeOperand1.Begin < 0 )    IPB->RangeOperand1.Begin += (int) ProgramProfile->ReturnWideString->Size;
					if( IPB->RangeOperand1.End < 0 )    IPB->RangeOperand1.End += (int) ProgramProfile->ReturnWideString->Size;
					if((IPB->RangeOperand1.Begin < ((int) ProgramProfile->ReturnWideString->Size)) &&
						 (IPB->RangeOperand1.End < ((int) ProgramProfile->ReturnWideString->Size)))
					{
						ProgramProfile->ReturnWideString->Copy(ProgramProfile->ReturnWideString,
									((unsigned int) IPB->RangeOperand1.Begin), ((unsigned int) IPB->RangeOperand1.End));
						ProgramProfile->ReturnBool =
									(ProgramProfile->ReturnWideString->Size) ? true : false;
					}
					else
					{
						ProgramProfile->ReturnWideString->Copy( L"" );
						ProgramProfile->ReturnBool = false;
					}
				}
				break;





			case HHPL_Inst_ArrayDataInRange:
		//		Param1:		Address of string array variable
		//		Param2:		Offset in string array variable
		//		Param3:		Command that returns a range operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "array data in range", 3,
							HHPL_PROC_DATA_ADDRESS_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
							HHPL_PROC_GET_RANGE_PARAMETER, 0, 0, 0, 0))
				{
				// We successfully acquired the parameters.
				// Check the integrity of the string (must be at least 4 bytes: 2 for
				// length and 2 for max size).
					if((IPB->DataAddress1 + IPB->IntegerOperand1 + 3) <
										((int) ProgramProfile->SizeOfDataSpace))
					{
					// Get the size and make sure that it is in the data space
						StringSize =
							((unsigned int) ProgramProfile->DataSpace
												[IPB->DataAddress1 + IPB->IntegerOperand1 + 2]) +
							((unsigned int) ProgramProfile->DataSpace
												[IPB->DataAddress1 + IPB->IntegerOperand1 + 3]) * 256;

						if((StringSize + IPB->DataAddress1 + IPB->IntegerOperand1 + 4) <=
												ProgramProfile->SizeOfDataSpace)
						{
							if( IPB->RangeOperand1.Begin < 0 )    IPB->RangeOperand1.Begin += (int) StringSize;
							if( IPB->RangeOperand1.End < 0 )    IPB->RangeOperand1.End += (int) StringSize;
							if((IPB->RangeOperand1.Begin < ((int) StringSize)) &&
										 (IPB->RangeOperand1.End < ((int) StringSize)))
							{
								ProgramProfile->ReturnString->Copy(
										((char *) (&ProgramProfile->DataSpace
												[IPB->DataAddress1 + IPB->IntegerOperand1 + 4])),
										StringSize,
										((unsigned int) IPB->RangeOperand1.Begin),
										((unsigned int) IPB->RangeOperand1.End));
								ProgramProfile->ReturnBool =
											(ProgramProfile->ReturnString->Size) ? true : false;
							}
							else
							{
								ProgramProfile->ReturnString->Copy("");
								ProgramProfile->ReturnBool = false;
							}
						}

						else
						{
						// String is corrupt; WOOF!
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "String array variable (%d bytes) extends past end of data space: IP = %08X",
													StringSize,
													ProgramProfile->NextInstruction);
							LogError(TempStr);
						}
					}
					else
					{
					// The offset to the string variable is beyond the end of the HHPL
					// code.  WOOF WOOF WOOF...
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "String array variable location is beyond end of data space: IP = %08X",
													ProgramProfile->NextInstruction);
						LogError(TempStr);
					}
				}
				break;




			case HHPL_Inst_SetDataInRange:
		//		Param1:		Address of string variable
		//		Param2:		Command that returns a range operand
		//		Param3:		Command that returns a string operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "set data in range", 3,
							HHPL_PROC_DATA_ADDRESS_PARAMETER, HHPL_PROC_GET_RANGE_PARAMETER,
							HHPL_PROC_GET_STRING_PARAMETER, 0, 0, 0, 0))
				{
				// We successfully acquired the parameters.  Now, copy the original
				// string to ReturnString.  If that works, we'll copy the string operand
				// to the specified range and then write the result back to the string
				// variable.
					if(!SkipOverInstructionsWithoutExecutingThem)
					{
					// Check the integrity of the string (must be at least 4 bytes: 2 for
					// length and 2 for max size).
						if((IPB->DataAddress1 + 3) < ((int) ProgramProfile->SizeOfDataSpace))
						{
						// Get the size and make sure that it is in the data space
							StringSize =
								((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + 2]) +
								((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + 3]) * 256;

							if((StringSize + IPB->DataAddress1 + 4) <= ProgramProfile->SizeOfDataSpace)
							{
							// The assignment size is OK.  Let's copy the return string.
								ProgramProfile->ReturnString->Copy(
										&ProgramProfile->DataSpace[IPB->DataAddress1 + 4],
												StringSize);


							// Let's make a copy to the specified range.
								if(ProgramProfile->ReturnString->CopyToRange(((unsigned int) IPB->RangeOperand1.Begin), ((unsigned int) IPB->RangeOperand1.End),
																		IPB->StringOperand1))
								{
								// We successfully copied the range; now, store the string
									MaximumStringSize =
										((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1]) +
										((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + 1]) * 256;

									if(MaximumStringSize >= ProgramProfile->ReturnString->Size)
									{
									// The assignment size is OK.  Let's copy the stuff.
										ProgramProfile->ReturnString->CopyFromString(
												&ProgramProfile->DataSpace[IPB->DataAddress1 + 4],
												MaximumStringSize);
										ProgramProfile->DataSpace[IPB->DataAddress1 + 2] =
											((unsigned char) (ProgramProfile->ReturnString->Size & 0xFF));
										ProgramProfile->DataSpace[IPB->DataAddress1 + 3] =
											((unsigned char) ((ProgramProfile->ReturnString->Size / 256) & 0xFF));
									}
									else
									{
									// Not enough room for string; WOOF!
										ProgramProfile->ReturnString->Size = MaximumStringSize;
										ProgramProfile->ReturnString->CopyFromString(&ProgramProfile->DataSpace[IPB->DataAddress1 + 4],
												MaximumStringSize);
										ProgramProfile->DataSpace[IPB->DataAddress1 + 2] =
											((unsigned char) (ProgramProfile->ReturnString->Size & 0xFF));
										ProgramProfile->DataSpace[IPB->DataAddress1 + 3] =
											((unsigned char) ((ProgramProfile->ReturnString->Size / 256) & 0xFF));
//										ProgramProfile->UnrecoverableError = true;
										sprintf(TempStr, "String variable (%d max bytes) too small for assignment (%d bytes): IP = %08X",
											MaximumStringSize,
											IPB->StringOperand1->Size,
											ProgramProfile->NextInstruction);
										LogError(TempStr);
									}
								}
								else
								{
								// Range is invalid; WOOF!
									ProgramProfile->UnrecoverableError = true;
									sprintf(TempStr, "Range in SetRange is invalid : IP = %08X",
											ProgramProfile->NextInstruction);
									LogError(TempStr);
								}
							}

							else
							{
							// String is corrupt; WOOF!
								ProgramProfile->UnrecoverableError = true;
								sprintf(TempStr, "String variable (%d bytes) extends past end of data space: IP = %08X",
													StringSize,
													ProgramProfile->NextInstruction);
								LogError(TempStr);
							}
						}
						else
						{
						// The offset to the string variable is beyond the end of the HHPL
						// code.  WOOF WOOF WOOF...
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "String variable location is beyond end of data space: IP = %08X",
													ProgramProfile->NextInstruction);
							LogError(TempStr);
						}
					}
				}
				break;



			case HHPL_Inst_SetArrayDataInRange:
		//		Param1:		Address of string array variable
		//		Param2:		Offset in string array variable
		//		Param3:		Command that returns a range operand
		//		Param4:		Command that returns a string operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "set array data in range", 4,
							HHPL_PROC_DATA_ADDRESS_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
							HHPL_PROC_GET_RANGE_PARAMETER, HHPL_PROC_GET_STRING_PARAMETER, 0, 0, 0))
				{
				// We successfully acquired the parameters.  Now, copy the original
				// string to ReturnString.  If that works, we'll copy the string operand
				// to the specified range and then write the result back to the string
				// variable.
					if(!SkipOverInstructionsWithoutExecutingThem)
					{
					// Check the integrity of the string (must be at least 4 bytes: 2 for
					// length and 2 for max size).
						if((IPB->DataAddress1 + IPB->IntegerOperand1 + 3) <
											((int) ProgramProfile->SizeOfDataSpace))
						{
						// Get the size and make sure that it is in the data space
							StringSize =
								((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 2]) +
								((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 3]) * 256;

							if((StringSize + IPB->DataAddress1 + IPB->IntegerOperand1 + 4)
											<= ProgramProfile->SizeOfDataSpace)
							{
							// The assignment size is OK.  Let's copy the return string.
								ProgramProfile->ReturnString->Copy(
										&ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 4],
												StringSize);


							// Let's make a copy to the specified range.
								if(ProgramProfile->ReturnString->CopyToRange(((unsigned int) IPB->RangeOperand1.Begin), ((unsigned int) IPB->RangeOperand1.End),
												IPB->StringOperand1))
								{
								// We successfully copied the range; now, store the string
									MaximumStringSize =
										((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1]) +
										((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 1]) * 256;

									if(MaximumStringSize >= ProgramProfile->ReturnString->Size)
									{
									// The assignment size is OK.  Let's copy the stuff.
										ProgramProfile->ReturnString->CopyFromString(
												&ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 4],
												MaximumStringSize);
										ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 2] =
											((unsigned char) (ProgramProfile->ReturnString->Size & 0xFF));
										ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 3] =
											((unsigned char) ((ProgramProfile->ReturnString->Size / 256) & 0xFF));
									}
									else
									{
									// Not enough room for string; WOOF!
										ProgramProfile->ReturnString->Size = MaximumStringSize;
										ProgramProfile->ReturnString->CopyFromString(
												&ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 4],
												MaximumStringSize);
										ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 2] =
											((unsigned char) (ProgramProfile->ReturnString->Size & 0xFF));
										ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 3] =
											((unsigned char) ((ProgramProfile->ReturnString->Size / 256) & 0xFF));
//										ProgramProfile->UnrecoverableError = true;
										sprintf(TempStr, "String variable (%d max bytes) too small for assignment (%d bytes): IP = %08X",
											MaximumStringSize,
											IPB->StringOperand1->Size,
											ProgramProfile->NextInstruction);
										LogError(TempStr);
									}
								}
								else
								{
								// Range is invalid; WOOF!
									ProgramProfile->UnrecoverableError = true;
									sprintf(TempStr, "Range in SetRange is invalid : IP = %08X",
											ProgramProfile->NextInstruction);
									LogError(TempStr);
								}
							}

							else
							{
							// String is corrupt; WOOF!
								ProgramProfile->UnrecoverableError = true;
								sprintf(TempStr, "String variable (%d bytes) extends past end of data space: IP = %08X",
													StringSize,
													ProgramProfile->NextInstruction);
								LogError(TempStr);
							}
						}
						else
						{
						// The offset to the string variable is beyond the end of the HHPL
						// code.  WOOF WOOF WOOF...
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "String variable location is beyond end of data space: IP = %08X",
													ProgramProfile->NextInstruction);
							LogError(TempStr);
						}
					}
				}
				break;



			case HHPL_Inst_SetStringLength:
		//		Param1:		Address of string variable
		//		Param2:		Command that returns a numeric operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "set string length", 2,
							HHPL_PROC_DATA_ADDRESS_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
							0, 0, 0, 0, 0))
				{
					if(!SkipOverInstructionsWithoutExecutingThem)
					{
					// We successfully executed the instruction; now, set the string length
						MaximumStringSize =
							((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1]) +
							((unsigned int) ProgramProfile->DataSpace[IPB->DataAddress1 + 1]) * 256;

						if((IPB->IntegerOperand1 >= 0) &&
								(IPB->IntegerOperand1 <= ((int) MaximumStringSize)))
						{
						// The new size is OK.
							ProgramProfile->DataSpace[IPB->DataAddress1 + 2] =
											((unsigned char) (IPB->IntegerOperand1 & 0xFF));
							ProgramProfile->DataSpace[IPB->DataAddress1 + 3] =
											((unsigned char) ((IPB->IntegerOperand1 / 256) & 0xFF));
						}
						else
						{
						// New size is out of range; WOOF!
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "New string length is out of string variable size range: IP = %08X",
									ProgramProfile->NextInstruction);
							LogError(TempStr);
						}
					}
				}
				break;



			case HHPL_Inst_SetStringArrayLength:
		//		Param1:		Address of string array variable
		//		Param2:		Offset in string array variable
		//		Param3:		Command that returns a numeric operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "set string array length", 3,
							HHPL_PROC_DATA_ADDRESS_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
							HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0))
				{
					if(!SkipOverInstructionsWithoutExecutingThem)
					{
					// We successfully executed the instruction; now, set the string length
						MaximumStringSize =
							((unsigned int) ProgramProfile->DataSpace
											[IPB->DataAddress1 + IPB->IntegerOperand1]) +
							((unsigned int) ProgramProfile->DataSpace
											[IPB->DataAddress1 + IPB->IntegerOperand1 + 1]) * 256;

						if((IPB->IntegerOperand2 >= 0) &&
								(IPB->IntegerOperand2 <= ((int) MaximumStringSize)))
						{
						// The new size is OK.
							ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 2] =
											((unsigned char) (IPB->IntegerOperand2 & 0xFF));
							ProgramProfile->DataSpace[IPB->DataAddress1 + IPB->IntegerOperand1 + 3] =
											((unsigned char) ((IPB->IntegerOperand2 / 256) & 0xFF));
						}
						else
						{
						// New size is out of range; WOOF!
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "New string length is out of string array variable size range: IP = %08X",
									ProgramProfile->NextInstruction);
							LogError(TempStr);
						}
					}
				}
				break;





		// HHPL range commands
			case HHPL_Inst_SetRange:
		//		Param1:		Command that returns the first numeric operand
		//		Param2:		Command that returns the second numeric operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "set range", 2,
							HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					ProgramProfile->ReturnRange.Begin = IPB->IntegerOperand1;
					ProgramProfile->ReturnRange.End = IPB->IntegerOperand2;
				}
				break;






// HHPL edit buffer assignment commands
			case HHPL_Inst_SetEditBuffer:
		//		Param1:		Command that sets the edit buffer string
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "set edit buffer string", 1,
												HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
					if(!SkipOverInstructionsWithoutExecutingThem)
					{
					// We successfully executed the instruction; now, store the edit buffer string
						ProgramProfile->EditBuffer->Copy(ProgramProfile->ReturnWideString);
					}
				}
				break;



			case HHPL_Inst_GetEditBuffer:
		//		Byte 0:		Command that gets the edit buffer string
				++ProgramProfile->NextInstruction;

				ProgramProfile->ReturnWideString->Copy(ProgramProfile->EditBuffer);
				break;







// HHPL data source ID assignment commands
			case HHPL_Inst_SetDataSourceID:
		//		Param1:		Command that sets the data source ID string
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "set data source ID string", 1,
												HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
					if(!SkipOverInstructionsWithoutExecutingThem)
					{
					// We successfully executed the instruction; now, store the data source ID string
						ProgramProfile->DataSourceID->Copy(ProgramProfile->ReturnWideString);
					}
				}
				break;



			case HHPL_Inst_GetDataSourceID:
		//		Byte 0:		Command that gets the data source ID string
				++ProgramProfile->NextInstruction;

				ProgramProfile->ReturnWideString->Copy(ProgramProfile->DataSourceID);
				break;







// HHPL host port input / output "variable" assignment commands
			case HHPL_Inst_SendHost:
		//		Param1:		Command that "sets" (sends) data to the host.
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "set host", 1,
												HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
					if(!SkipOverInstructionsWithoutExecutingThem)
					{
					// We successfully executed the instruction; now, send the string
						SendToHostParameters.String = ( ProgramProfile->ReturnWideString->WChar );
						SendToHostParameters.LengthOfString = ((int) ProgramProfile->ReturnWideString->Size);
						SendToHostParameters.DataSourceInformation = DEFAULT_DATA_SOURCE_ID;
						SendToHostParameters.ControlMask = 0;
						SendToHostParameters.DelayInMicroseconds = 0; 
						SendToHost( &SendToHostParameters );
					}
				}
				break;



			case HHPL_Inst_GetHost:
		//		Byte 0:		Command that "gets" (reads) data from the host.
				++ProgramProfile->NextInstruction;

				ProgramProfile->ReturnWideString->Size =
									((unsigned int) GetFromHost( ProgramProfile->ReturnWideString->WChar ));
				break;





	// HHPL string value commands
			case HHPL_Inst_UCharLengthString:
		//		Byte 0:		HHPL_Inst_UCharLengthString instruction
		//		Byte 1:		Number of characters following this byte that constitute a string
		//		Byte 2...:	String of length specified in byte 1
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 1);	// Must be minimum of 1 argument following

			// Get the character count and characters
				if(!ProgramProfile->UnrecoverableError)
				{
					CharacterCount =
						((unsigned int) (ProgramProfile->Code[ProgramProfile->NextInstruction]));
					++ProgramProfile->NextInstruction;

					CheckForArguments(ProgramProfile, CharacterCount);

					if(!ProgramProfile->UnrecoverableError)
					{
						ProgramProfile->ReturnString->Copy(&ProgramProfile->Code[ProgramProfile->NextInstruction],
									CharacterCount);
						ProgramProfile->NextInstruction += CharacterCount;
					}
				}
				break;


			case HHPL_Inst_UShortLengthString:
		//		Byte 0:		HHPL_Inst_UShortLengthString instruction
		//		Byte 1, 2:	Number of characters following this USHORT that constitute a string;
		//					Note that the USHORT is stored little endian
		//		Byte 2...:	String of length specified in bytes 1 and 2
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 2);// Must be minimum of 2 arguments following

			// Get the character count and characters
				if(!ProgramProfile->UnrecoverableError)
				{
					CharacterCount = ((unsigned int) (
						((unsigned int )ProgramProfile->Code[ProgramProfile->NextInstruction+1]) * 256 +
						((unsigned int )ProgramProfile->Code[ProgramProfile->NextInstruction])));
					ProgramProfile->NextInstruction += 2;

					CheckForArguments(ProgramProfile, CharacterCount);

					if(!ProgramProfile->UnrecoverableError)
					{
						ProgramProfile->ReturnString->Copy(&ProgramProfile->Code[ProgramProfile->NextInstruction],
									CharacterCount);
						ProgramProfile->NextInstruction += CharacterCount;
					}
				}
				break;



			case HHPL_Inst_UCharLengthWideString:
		//		Byte 0:		HHPL_Inst_UCharLengthWideString instruction
		//		Byte 1:		Number of characters following this byte that constitute a string
		//		Byte 2...:	String of length specified in byte 1
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 1);	// Must be minimum of 1 argument following

			// Get the character count and characters
				if(!ProgramProfile->UnrecoverableError)
				{
					CharacterCount =
						((unsigned int) (ProgramProfile->Code[ProgramProfile->NextInstruction]));
					++ProgramProfile->NextInstruction;

					CheckForArguments(ProgramProfile, CharacterCount * 2 );

					if(!ProgramProfile->UnrecoverableError)
					{
						if( CharacterCount > 0 )
						{
							ProgramProfile->ReturnWideString->CopyLittleEndian( ProgramProfile->Code, ProgramProfile->Size,
									ProgramProfile->NextInstruction, 
									ProgramProfile->NextInstruction + ( CharacterCount * 2 ) - 1 );
						}
						else
						{
							ProgramProfile->ReturnWideString->Copy( L"" );
						}
						ProgramProfile->NextInstruction += CharacterCount * 2;
					}
				}
				break;


			case HHPL_Inst_UShortLengthWideString:
		//		Byte 0:		HHPL_Inst_UShortLengthWideString instruction
		//		Byte 1, 2:	Number of characters following this USHORT that constitute a wide string;
		//					Note that the USHORT is stored little endian
		//		Byte 2...:	Wide string of length specified in bytes 1 and 2
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 2);// Must be minimum of 2 arguments following

			// Get the character count and characters
				if(!ProgramProfile->UnrecoverableError)
				{
					CharacterCount = ((unsigned int) (
						((unsigned int )ProgramProfile->Code[ProgramProfile->NextInstruction+1]) * 256 +
						((unsigned int )ProgramProfile->Code[ProgramProfile->NextInstruction])));
					ProgramProfile->NextInstruction += 2;

					CheckForArguments(ProgramProfile, CharacterCount * 2 );

					if(!ProgramProfile->UnrecoverableError)
					{
						if( CharacterCount > 0 )
						{
							ProgramProfile->ReturnWideString->CopyLittleEndian( ProgramProfile->Code, ProgramProfile->Size,
									ProgramProfile->NextInstruction, 
									ProgramProfile->NextInstruction + ( CharacterCount * 2 ) - 1 );
						}
						else
						{
							ProgramProfile->ReturnWideString->Copy( L"" );
						}
						ProgramProfile->NextInstruction += CharacterCount * 2;
					}
				}
				break;



			case HHPL_Inst_StringOperationOnNextTwoResults:
		//		Param1:		String operation sub instruction
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "two operand string operation", 1,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_TwoOperandStringOperationSubInst_ConcatenateNextTwoResults:
						//		Param1:		Command that returns the first string operand
						//		Param2:		Command that returns the second string operand
							if(GetParameters(ProgramProfile, IPB, "concatenate strings", 2,
								HHPL_PROC_GET_STRING_PARAMETER, HHPL_PROC_GET_STRING_PARAMETER, 0, 0, 0, 0, 0))
							{
								ProgramProfile->ReturnString->Copy(IPB->StringOperand1);
								ProgramProfile->ReturnString->Append(IPB->StringOperand2);
							}
							break;

						case HHPL_TwoOperandStringOperationSubInst_ConcatenateNextTwoWideResults:
						//		Param1:		Command that returns the first wide string operand
						//		Param2:		Command that returns the second wide string operand
							if(GetParameters(ProgramProfile, IPB, "concatenate wide strings", 2,
								HHPL_PROC_GET_WIDE_STRING_PARAMETER, HHPL_PROC_GET_WIDE_STRING_PARAMETER, 0, 0, 0, 0, 0))
							{
								ProgramProfile->ReturnWideString->Copy(IPB->WideStringOperand1);
								ProgramProfile->ReturnWideString->Append(IPB->WideStringOperand2);
							}
							break;

						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized two operand string operation sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;



			case HHPL_Inst_StringOperationOnNextResult:
		//		Param1:		String operation sub instruction
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "one operand string operation", 1,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_SingleOperandStringOperationSubInst_ConvertStringToUpperCase:
					//		Param1:		Command that returns the first string operand
							if(GetParameters(ProgramProfile, IPB, "string to upper case", 1,
											HHPL_PROC_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
								ProgramProfile->ReturnString->UpperCase();
							}
							break;

						case HHPL_SingleOperandStringOperationSubInst_ConvertStringToLowerCase:
						//		Param1:		Command that returns the first string operand
							if(GetParameters(ProgramProfile, IPB, "string to lower case", 1,
											HHPL_PROC_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
								ProgramProfile->ReturnString->LowerCase();
							}
							break;

						case HHPL_SingleOperandStringOperationSubInst_ConvertWideStringToUpperCase:
					//		Param1:		Command that returns the first wide string operand
							if(GetParameters(ProgramProfile, IPB, "wide string to upper case", 1,
											HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
								ProgramProfile->ReturnWideString->UpperCase();
							}
							break;

						case HHPL_SingleOperandStringOperationSubInst_ConvertWideStringToLowerCase:
						//		Param1:		Command that returns the first wide string operand
							if(GetParameters(ProgramProfile, IPB, "wide string to lower case", 1,
											HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
								ProgramProfile->ReturnWideString->LowerCase();
							}
							break;

						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized one operand string operation sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;




	// HHPL number commands
			case HHPL_Inst_Int8:
		//		Byte 0:		HHPL_Inst_Int8 instruction
		//		Byte 1, 2:	Number to be returned by this 1 byte int;
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 1);// Must be minimum of 2 arguments following

			// Get the number
				if(!ProgramProfile->UnrecoverableError)
				{
					ProgramProfile->ReturnNumber = 0;
					ProgramProfile->ReturnNumber =
						((int) ((signed char) ProgramProfile->Code[ProgramProfile->NextInstruction]));
					ProgramProfile->NextInstruction += 1;
				}
				break;


			case HHPL_Inst_Int16:
		//		Byte 0:		HHPL_Inst_Int16 instruction
		//		Byte 1, 2:	Number to be returned by this SHORT;
		//					Note that the SHORT is stored little endian
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 2);// Must be minimum of 2 arguments following

			// Get the number
				if(!ProgramProfile->UnrecoverableError)
				{
					ProgramProfile->ReturnNumber = 0;
					ProgramProfile->ReturnNumber |=
						((int) ((signed short)
							((((unsigned short)ProgramProfile->Code[ProgramProfile->NextInstruction+1]) << 8) |
							((unsigned short)ProgramProfile->Code[ProgramProfile->NextInstruction]))));
					ProgramProfile->NextInstruction += 2;
				}
				break;


			case HHPL_Inst_Int32:
		//		Byte 0:		HHPL_Inst_INT32 instruction
		//		Byte 1, 2:	Number to be returned by this 32 bit INT;
		//					Note that the INT32 is stored little endian
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 4);// Must be minimum of 4 arguments following

			// Get the number
				if(!ProgramProfile->UnrecoverableError)
				{
					ProgramProfile->ReturnNumber = 0;
					ProgramProfile->ReturnNumber |=
						(((unsigned int )ProgramProfile->Code[ProgramProfile->NextInstruction+3]) << 24) |
						(((unsigned int )ProgramProfile->Code[ProgramProfile->NextInstruction+2]) << 16) |
						(((unsigned int )ProgramProfile->Code[ProgramProfile->NextInstruction+1]) << 8) |
						((unsigned int )ProgramProfile->Code[ProgramProfile->NextInstruction]);
					ProgramProfile->NextInstruction += 4;
				}
				break;




	// HHPL I/O commands
			case HHPL_Inst_TimerOperations:
		//		Param1:		Timer operation sub instruction
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "timer operation", 1,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_TimerOperationSubInst_ResetTimer0:
						//		Byte 0:		HHPL_Inst_ResetTimer0 instruction
							ZeroSystemTimer0();
							break;

						case HHPL_TimerOperationSubInst_GetTimer0:
						//		Byte 0:		HHPL_Inst_GetTimer0 instruction
							ProgramProfile->ReturnNumber = GetSystemTimer0();
							break;

						case HHPL_TimerOperationSubInst_DelayMilliseconds:
						//		Param1:		Command that returns a numeric operand
							if(GetParameters(ProgramProfile, IPB, "wait", 1,
										HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
							// We successfully executed the instruction; now, set the delay.
							// Delay count must be less than 64000
								if(IPB->IntegerOperand1 > 64000)
								{
									IPB->IntegerOperand1 = 640000;
//									ProgramProfile->UnrecoverableError = true;
									sprintf(TempStr, "DelayMilliseconds: Delay too large: IP = %08X",
												ProgramProfile->NextInstruction);
									LogError(TempStr);
								}

								if(IPB->IntegerOperand1 > 0)
								{
									Delay(IPB->IntegerOperand1);
								}
							}
							break;

						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized timer operation sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;





			case HHPL_Inst_DisplayControlOperations:
		//		Param1:		Display control operation sub instruction
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "display control operation", 1,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_DisplayControlSubInst_DisplayBacklightOn:
						//		Byte 0:		HHPL_Inst_DisplayBacklightOn instruction
							BacklightOn();
							break;

						case HHPL_DisplayControlSubInst_DisplayBacklightOff:
						//		Byte 0:		HHPL_Inst_DisplayBacklightOff instruction
							BacklightOff();
							break;

						case HHPL_DisplayControlSubInst_DisplayOn:
						//		Byte 0:		HHPL_Inst_DisplayOn instruction
							DisplayOn();
							break;

						case HHPL_DisplayControlSubInst_DisplayOff:
						//		Byte 0:		HHPL_Inst_DisplayOff instruction
							DisplayOff();
							break;

						case HHPL_DisplayControlSubInst_EraseDisplay:
							EraseDisplay();
							break;

						case HHPL_DisplayControlSubInst_SetFont:
						//		Param1:		Command that returns a numeric operand
							if(GetParameters(ProgramProfile, IPB, "set font", 1,
								HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
								SelectFont(IPB->IntegerOperand1);
							}
							break;

						case HHPL_DisplayControlSubInst_RestorePreservedDisplay:
						//		Byte 0:		HHPL_Inst_RestorePreservedDisplay instruction
							ProgramProfile->ReturnBool = RestorePreservedDisplay();
							break;

						case HHPL_DisplayControlSubInst_PreserveCurrentDisplay:
						//		Byte 0:		HHPL_Inst_PreserveCurrentDisplay instruction
							PreserveCurrentDisplay();
							break;

						case HHPL_DisplayControlSubInst_MoveDisplayCursor:
						//		Param1:		Command that returns a numeric operand
						//		Param2:		Command that returns a numeric operand
							if(GetParameters(ProgramProfile, IPB, "move display cursor", 2,
								HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0))
							{
								ProgramProfile->ReturnBool = MoveDisplayCursor
									(IPB->IntegerOperand1, IPB->IntegerOperand2);
							}
							break;

						case HHPL_DisplayControlSubInst_DisplayHeight:
						//		Byte 0:		HHPL_Inst_DisplayHeight instruction
							ProgramProfile->ReturnNumber = DisplayHeight();
							break;

						case HHPL_DisplayControlSubInst_DisplayWidth:
						//		Byte 0:		HHPL_Inst_DisplayWidth instruction
							ProgramProfile->ReturnNumber = DisplayWidth();
							break;

						case HHPL_DisplayControlSubInst_CharacterHeight:
						//		Param1:		Command that returns an integer operand
							if(GetParameters(ProgramProfile, IPB, "character height", 1,
									HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
								ProgramProfile->ReturnNumber = CharacterHeight(IPB->IntegerOperand1);
							}
							break;

						case HHPL_DisplayControlSubInst_CharacterWidth:
						//		Param1:		Command that returns an integer operand
						//		Param2:		Command that returns a string operand
							if(GetParameters(ProgramProfile, IPB, "character width", 2,
								HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_WIDE_STRING_RETURN_PARAMETER,
								0, 0, 0, 0, 0))
							{
								ProgramProfile->ReturnNumber = CharacterWidth
									(IPB->IntegerOperand1, ((int) ProgramProfile->ReturnWideString->WChar[0]));
							}
							break;

						case HHPL_DisplayControlSubInst_ScrollDisplay:
						//		Param1:		Command that returns a numeric operand
							if(GetParameters(ProgramProfile, IPB, "scroll display", 1,
								HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0, 0))
								{
									ProgramProfile->ReturnBool = ScrollDisplay(IPB->IntegerOperand1);
								}
							break;

						case HHPL_DisplayControlSubInst_TemporarilyEnableBacklight:
						//		Param1:		Command that returns a numeric operand
								if(GetParameters(ProgramProfile, IPB, "temporarily enable backlight", 1,
									HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0, 0))
								{
									TempEnable(0, IPB->IntegerOperand1);
								}
							break;

						case HHPL_DisplayControlSubInst_TemporarilyEnableDisplay:
						//		Param1:		Command that returns a numeric operand
							if(GetParameters(ProgramProfile, IPB, "temporarily enable display", 1,
								HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
								TempEnable(1, IPB->IntegerOperand1);
							}
							break;

						case HHPL_DisplayControlSubInst_EraseLine:
						//		Param1:		Command that returns a numeric operand
						//		Param2:		Command that returns a numeric operand
							if(GetParameters(ProgramProfile, IPB, "erase line", 2,
								HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0))
							{
								EraseLine(IPB->IntegerOperand1, IPB->IntegerOperand2);
							}
							break;

						case HHPL_DisplayControlSubInst_GetFont:
							ProgramProfile->ReturnNumber = GetFont();
							break;

						case HHPL_DisplayControlSubInst_DisplayCursorControl:
						//		Param1:		Display cursor control sub instruction
							if(GetParameters(ProgramProfile, IPB, "display cursor control", 1,
									HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
							// If successful, perform the operation specified in the sub-instruction
								switch(IPB->SubinstructionOperand1)
								{
									case HHPL_DISPLAY_CURSOR_OFF:
										DisplayCursorOff();
										break;

									case HHPL_DISPLAY_CURSOR_ON_STEADY:
										DisplayCursorOnSteady();
										break;

									case HHPL_DISPLAY_CURSOR_ON_BLINK:
										DisplayCursorOnBlink();
										break;

									default:
										ProgramProfile->UnrecoverableError = true;
										sprintf(TempStr, "Unrecognized display cursor control sub-instruction: IP = %08X",
											ProgramProfile->NextInstruction);
										LogError(TempStr);
										break;
								};
							}
							break;

						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized display control operation sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;





			case HHPL_Inst_DisplayOutputOperations:
		//		Param1:		Display output operation sub instruction
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "display output operation", 1,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_DisplayOutputSubInst_LoadImage:
						//		Param1:		Command that returns a numeric operand
						//		Param2:		Command that returns a numeric operand
						//		Param3:		Command that returns a numeric operand
							if(GetParameters(ProgramProfile, IPB, "load image", 3,
								HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
								HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0))
							{
								ProgramProfile->ReturnBool = LoadSystemBitmap(IPB->IntegerOperand1, IPB->IntegerOperand2,
															IPB->IntegerOperand3);
							}
							break;

						case HHPL_DisplayOutputSubInst_OutputString:
						//		Param1:		Command that returns a string operand
						//		Param2:		Command that returns a numeric operand
						//		Param3:		Command that returns a numeric operand
						//		Param4:		Command that returns a numeric operand
							if(GetParameters(ProgramProfile, IPB, "output string", 4,
								HHPL_PROC_GET_WIDE_STRING_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
								HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
								0, 0, 0))
							{
								ProgramProfile->ReturnBool = OutputString(IPB->IntegerOperand1,
									IPB->IntegerOperand2, IPB->IntegerOperand3, IPB->WideStringOperand1);
							}
							break;

						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized display output sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;





			case HHPL_Inst_AlarmSignalingOperations:
		//		Param1:		Alarm / signaling operation sub instruction
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "alarm / signaling operation", 1,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_AlarmSignalingSubInst_Beep:
						//		Param1:		Command that returns a numeric operand
						//		Param2:		Command that returns a numeric operand
						//		Param3:		Command that returns a numeric operand
							if(GetParameters(ProgramProfile, IPB, "beep", 3,
								HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
								HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0))
							{
								Beep(IPB->IntegerOperand1, IPB->IntegerOperand2, IPB->IntegerOperand3);
							}
							break;

						case HHPL_AlarmSignalingSubInst_IndicatorControl:
						//		Param1:		Indicator control sub instruction
							if(GetParameters(ProgramProfile, IPB, "indicator control", 1,
								HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
							// If successful, perform the operation specified in the sub-instruction
								switch(IPB->SubinstructionOperand1)
								{
									case HHPL_IndicatorControlSubInst_GoodSequence:
										DoGoodIndicator();
										break;

									case HHPL_IndicatorControlSubInst_BadSequence:
										DoBadIndicator();
										break;

									case HHPL_IndicatorControlSubInst_IndicatorLightOn:
										if(GetParameters(ProgramProfile, IPB, "indicator lights on", 3,
											HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
											HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0))
										{
											IndicatorLightOn(IPB->IntegerOperand1, IPB->IntegerOperand2,
																		IPB->IntegerOperand3);
										}
										else
										{
											ProgramProfile->UnrecoverableError = true;
										}
										break;

									case HHPL_IndicatorControlSubInst_IndicatorLightsOff:
										IndicatorLightOn(HHPL_INDICATOR_CANCEL, 0, 0);
										break;

									default:
										ProgramProfile->UnrecoverableError = true;
										sprintf(TempStr, "Unrecognized indicator control sub-instruction: IP = %08X",
											ProgramProfile->NextInstruction);
										LogError(TempStr);
										break;
								};
							}
							break;

						case HHPL_AlarmSignalingSubInst_IssueWarning:
						//		Byte 0:		HHPL_Inst_IssueWarning instruction
							ProcessWarningFlag();
							break;

						case HHPL_AlarmSignalingSubInst_IssueError:
						//		Byte 0:		HHPL_Inst_IssueError instruction
							ProcessErrorFlag();
							break;

						case HHPL_AlarmSignalingSubInst_IssueTerminate:
						//		Byte 0:		HHPL_Inst_IssueTerminate instruction
							ProcessTerminateFlag();
							break;


						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized alarm / signaling operation sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;





			case HHPL_Inst_InputDeviceOperations:
		//		Param1:		Input device operation sub instruction
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "input device operation", 1,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_InputDeviceSubInst_InputCharacter:
						//		Param1:		Command that returns a numeric operand (control mask)
						//		Param2:		Command that returns a numeric operand (timeout)
							if(GetParameters(ProgramProfile, IPB, "input character", 2,
								HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
								0, 0, 0, 0, 0))
							{
							// Get a character and return it as both an integer and a string.
								ProgramProfile->ReturnNumber = InputCharacter(IPB->IntegerOperand1, IPB->IntegerOperand2);
								ProgramProfile->ReturnWideString->Size = 1;
								ProgramProfile->ReturnWideString->WChar[0] = ((wchar_t) ProgramProfile->ReturnNumber);
							}
							break;

						case HHPL_InputDeviceSubInst_TriggerHit:
					//		Byte 0:		HHPL_Inst_TriggerHit instruction
							ProgramProfile->ReturnBool = TriggerHit();
							break;

						case HHPL_InputDeviceSubInst_InputNumber:
						//		Param1:		Command that returns a numeric operand (default value)
						//		Param2:		Command that returns a numeric operand (number of quit keys)
							if(GetParameters(ProgramProfile, IPB, "input number", 2,
								HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
								0, 0, 0, 0, 0))
							{
							// Save the default value, because we're going to be overwriting operand 1.
								int		DefaultValue = IPB->IntegerOperand1;
								int		NumberOfQuitKeyFlags = IPB->IntegerOperand2;
								int		FunctionStatusCode;

							// Initialize quit key flags to zero.
								char	QuitKeyFlags[32];
								for(i = 0;  i < 32;    ++i)    QuitKeyFlags[i] = 0;

							// For each quit key value, set the corresponding bit number in the flag array.
								i = 0;
								while((i < NumberOfQuitKeyFlags) && (!ProgramProfile->UnrecoverableError))
								{
									if(GetParameters(ProgramProfile, IPB, "input number", 1,
										HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0, 0))
									{
//										QuitKeyFlags[(IPB->IntegerOperand1 & 0xFF) / 8] |=
//														((char) (1 << (IPB->IntegerOperand1 % 8)));
										QuitKeyFlags[(IPB->IntegerOperand1 & 0xFF) / 8] =
													((char) (QuitKeyFlags[(IPB->IntegerOperand1 & 0xFF) / 8] |
														((char) (1 << (IPB->IntegerOperand1 % 8)))));
										++i;
									}
								}

							// Get the rest of the parameters, now that we have the quit key mask.  Note that
							// GetParameters will not execute if we hit an unrecoverable error above.
							//		Param1:		Command that returns a numeric operand (control mask)
							//		Param2:		Command that returns a numeric operand (minimum)
							//		Param3:		Command that returns a numeric operand (maximum)
							//		Param4:		Command that returns a numeric operand (timeout)
							//		Param5:		Instruction that will set a numeric variable
							//		Param6:		Instruction that will set a string variable
								if(GetParameters(ProgramProfile, IPB, "input number", 4,
									HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
									HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
									0, 0, 0))
								{
									FunctionStatusCode = InputNumber(IPB->IntegerOperand1,
											&IntegerRegisters[0], IPB->IntegerOperand2,
											IPB->IntegerOperand3, DefaultValue,
											wchTempStr, IPB->IntegerOperand4, QuitKeyFlags);

									WideStringRegisters[0] = new HWSTRING(wchTempStr, true);

								// If an error occurred, don't execute the SetStringVariable instructions
									if(FunctionStatusCode < 0)
									{
										SkipOverInstructionsWithoutExecutingThem = true;
									}

								// Let's store the output number and data source ID.
									GetParameters(ProgramProfile, IPB, "input number", 2,
										HHPL_PROC_SET_NUMERIC_PARAMETER, HHPL_PROC_SET_WIDE_STRING_PARAMETER, 0, 0, 0, 0, 0);

									SkipOverInstructionsWithoutExecutingThem = false;

									delete WideStringRegisters[0];

									ProgramProfile->ReturnNumber = FunctionStatusCode;
									ProgramProfile->ReturnBool =
										(ProgramProfile->ReturnNumber >= 0) ? true : false;
								}
							}
							break;


						case HHPL_InputDeviceSubInst_InputString:
						//		Param1:		Command that returns a string operand (default value)
						//		Param2:		Command that returns a numeric operand (number of quit keys)
							if(GetParameters(ProgramProfile, IPB, "input string", 2,
									HHPL_PROC_GET_WIDE_STRING_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
									0, 0, 0, 0, 0))
							{
								int		NumberOfQuitKeyFlags = IPB->IntegerOperand1;
								int		FunctionStatusCode;
								int		InputStringControlFlags;

							// Initialize quit key flags to zero.
								char	QuitKeyFlags[32];
								for(i = 0;  i < 32;    ++i)    QuitKeyFlags[i] = 0;

							// For each quit key value, set the corresponding bit number in the flag array.
								i = 0;
								while((i < NumberOfQuitKeyFlags) && (!ProgramProfile->UnrecoverableError))
								{
									if(GetParameters(ProgramProfile, IPB, "input string", 1,
										HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0, 0))
									{
//										QuitKeyFlags[(IPB->IntegerOperand1 & 0xFF) / 8] |=
//														((char) (1 << (IPB->IntegerOperand1 % 8)));
										QuitKeyFlags[(IPB->IntegerOperand1 & 0xFF) / 8] =
												((char) (QuitKeyFlags[(IPB->IntegerOperand1 & 0xFF) / 8] |
														((char) (1 << (IPB->IntegerOperand1 % 8)))));
										++i;
									}
								}

							//		Param1:		Command that returns a numeric operand (control mask)
							//		Param2:		Command that returns a numeric operand (minimum length)
							//		Param3:		Command that returns a numeric operand (maximum length)
							//		Param4:		Command that returns a numeric operand (timeout)
							//		Param5:		Instruction that will set a string variable
							//		Param6:		Instruction that will set a string variable
								if(GetParameters(ProgramProfile, IPB, "input string", 4,
										HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
										HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0))
								{
									ProgramProfile->ReturnWideString->Copy(IPB->WideStringOperand1);

									FunctionStatusCode = InputString(IPB->IntegerOperand1,
										ProgramProfile->ReturnWideString,
										IPB->IntegerOperand2,
										IPB->IntegerOperand3, wchTempStr, IPB->IntegerOperand4, QuitKeyFlags,
										&InputStringControlFlags );

									WideStringRegisters[0] = new HWSTRING(ProgramProfile->ReturnWideString, true);

								// If an error occurred, don't execute the SetStringVariable instructions
									if(FunctionStatusCode < 0)
									{
										ProgramProfile->ReturnWideString->Size = ((unsigned int) FunctionStatusCode);
										SkipOverInstructionsWithoutExecutingThem = true;
									}

								// Let's store the output string first, then the data source ID.
									if(GetParameters(ProgramProfile, IPB, "input string", 1,
											HHPL_PROC_SET_WIDE_STRING_PARAMETER, 0, 0, 0, 0, 0, 0))
									{
										if(!SkipOverInstructionsWithoutExecutingThem)
										{
											WideStringRegisters[0]->Copy( wchTempStr );
										}

										GetParameters(ProgramProfile, IPB, "input string", 1,
												HHPL_PROC_SET_WIDE_STRING_PARAMETER, 0, 0, 0, 0, 0, 0);
									}

									SkipOverInstructionsWithoutExecutingThem = false;

									ProgramProfile->ReturnNumber = FunctionStatusCode;

									delete WideStringRegisters[0];
								}
							}
							break;

						case HHPL_InputDeviceSubInst_InputStringWithControlFlags:
						//		Param1:		Command that returns a string operand (default value)
						//		Param2:		Command that returns a numeric operand (number of quit keys)
							if(GetParameters(ProgramProfile, IPB, "input string", 2,
									HHPL_PROC_GET_WIDE_STRING_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
									0, 0, 0, 0, 0))
							{
								int		NumberOfQuitKeyFlags = IPB->IntegerOperand1;
								int		FunctionStatusCode;

							// Initialize quit key flags to zero.
								char	QuitKeyFlags[32];
								for(i = 0;  i < 32;    ++i)    QuitKeyFlags[i] = 0;

							// For each quit key value, set the corresponding bit number in the flag array.
								i = 0;
								while((i < NumberOfQuitKeyFlags) && (!ProgramProfile->UnrecoverableError))
								{
									if(GetParameters(ProgramProfile, IPB, "input string", 1,
										HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0, 0))
									{
//										QuitKeyFlags[(IPB->IntegerOperand1 & 0xFF) / 8] |=
//														((char) (1 << (IPB->IntegerOperand1 % 8)));
										QuitKeyFlags[(IPB->IntegerOperand1 & 0xFF) / 8] =
												((char) (QuitKeyFlags[(IPB->IntegerOperand1 & 0xFF) / 8] |
														((char) (1 << (IPB->IntegerOperand1 % 8)))));
										++i;
									}
								}

							//		Param1:		Command that returns a numeric operand (control mask)
							//		Param2:		Command that returns a numeric operand (minimum length)
							//		Param3:		Command that returns a numeric operand (maximum length)
							//		Param4:		Command that returns a numeric operand (timeout)
							//		Param5:		Instruction that will set a string variable
							//		Param6:		Instruction that will set a string variable
								if(GetParameters(ProgramProfile, IPB, "input string", 4,
										HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
										HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0))
								{
									ProgramProfile->ReturnWideString->Copy(IPB->WideStringOperand1);

									InputStringParameters.ControlMask = IPB->IntegerOperand1;
									InputStringParameters.InputData = ProgramProfile->ReturnWideString;
									InputStringParameters.MinimumLength = IPB->IntegerOperand2;
									InputStringParameters.MaximumLength = IPB->IntegerOperand3;
									InputStringParameters.DataSourceInformation = wchTempStr;
									InputStringParameters.Timeout = IPB->IntegerOperand4;
									InputStringParameters.QuitFlags = QuitKeyFlags;

									FunctionStatusCode = InputString( &InputStringParameters );

									WideStringRegisters[0] = new HWSTRING(ProgramProfile->ReturnWideString, true);

								// If an error occurred, don't execute the SetStringVariable instructions
									if(FunctionStatusCode < 0)
									{
										ProgramProfile->ReturnWideString->Size = ((unsigned int) FunctionStatusCode);
										SkipOverInstructionsWithoutExecutingThem = true;
									}

								// Let's store the output string first, then the data source ID.
									if(GetParameters(ProgramProfile, IPB, "input string", 1,
											HHPL_PROC_SET_WIDE_STRING_PARAMETER, 0, 0, 0, 0, 0, 0))
									{
										if(!SkipOverInstructionsWithoutExecutingThem)
										{
											WideStringRegisters[0]->Copy( wchTempStr );
										}

										GetParameters(ProgramProfile, IPB, "input string", 1,
												HHPL_PROC_SET_WIDE_STRING_PARAMETER, 0, 0, 0, 0, 0, 0);

										if(!SkipOverInstructionsWithoutExecutingThem)
										{
											IntegerRegisters[ 0 ] = InputStringParameters.ControlFlags;
										}

										GetParameters(ProgramProfile, IPB, "input string", 1,
												HHPL_PROC_SET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0, 0);

										if(!SkipOverInstructionsWithoutExecutingThem)
										{
											IntegerRegisters[ 0 ] = InputStringParameters.DelayInMicroseconds;
										}

										GetParameters(ProgramProfile, IPB, "input string", 1,
												HHPL_PROC_SET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0, 0);
									}

									SkipOverInstructionsWithoutExecutingThem = false;

									ProgramProfile->ReturnNumber = FunctionStatusCode;

									delete WideStringRegisters[0];
								}
							}
							break;

						case HHPL_InputDeviceSubInst_KeyHit:
						//		Param1:		Key release control flag
							if(GetParameters(ProgramProfile, IPB, "keyhit", 1,
								HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
								ProgramProfile->ReturnNumber = KeyHit(((int) IPB->IntegerOperand1));
							}
							break;

						case HHPL_InputDeviceSubInst_KeyOperations:
						//		Param1:		Key configuration operation sub instruction
							if(GetParameters(ProgramProfile, IPB, "key operation", 1,
								HHPL_PROC_SUBINSTRUCTION_PARAMETER,
								0, 0, 0, 0, 0, 0))
							{
							// If successful, perform the operation specified in the sub-instruction
								switch(IPB->SubinstructionOperand1)
								{
									case HHPL_KEYPAD_KEYCLICK_OFF:
										KeyClick(0);
										break;

									case HHPL_KEYPAD_KEYCLICK_ON:
										KeyClick(1);
										break;

									case HHPL_KEYPAD_FLUSH:
										FlushKeyPadBuffer();
										break;

									default:
										ProgramProfile->UnrecoverableError = true;
										sprintf(TempStr, "Unrecognized key configuration operation sub-instruction: IP = %08X",
											ProgramProfile->NextInstruction);
										LogError(TempStr);
										break;
								};
							}
							break;

						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized input device operation sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;





			case HHPL_Inst_CommunicationsOperations:
		//		Param1:		Communications operation sub instruction
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "communications operation", 1,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_CommunicationsSubInst_Send:
						//		Param1:		Command that returns a string operand
						//		Param2:		Command that returns a prefix operand
						//		Param3:		Command that returns a suffix operand
						//		Param4:		Command that returns a string operand
						//		Param5:		Command that returns a numeric operand (control mask)
							if(GetParameters(ProgramProfile, IPB, "send", 5,
								HHPL_PROC_GET_WIDE_STRING_PARAMETER, HHPL_PROC_PREFIX_RETURN_PARAMETER,
								HHPL_PROC_SUFFIX_RETURN_PARAMETER, HHPL_PROC_GET_WIDE_STRING_PARAMETER,
								HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0))
							{
								bool						DelayIsOK = true;

								SendToHostParameters.DataSourceInformation = ( IPB->WideStringOperand2->WChar );
								SendToHostParameters.ControlMask = ((int) IPB->IntegerOperand1);
								SendToHostParameters.DelayInMicroseconds = 0; 
								SendToHostParameters.ControlFlags = ProgramProfile->EditBufferControlFlags;

							// If a microsecond delay is included, get that parameter too.
								if( ((unsigned int) IPB->IntegerOperand1) & HHPL_SEND_TO_HOST_USE_MICROSECOND_DELAY )
								{
									DelayIsOK = GetParameters( ProgramProfile, IPB, "send", 1, HHPL_PROC_GET_NUMERIC_PARAMETER,
												0, 0, 0, 0, 0, 0 );
									SendToHostParameters.DelayInMicroseconds = ((int) IPB->IntegerOperand1);
								}

								if( DelayIsOK )
								{
									ProgramProfile->ReturnWideString->Copy( L"" );

								// For the time being, let's concatenate the prefix, decoding result,
								// and suffix into one big string, because SendToHost can't handle it
								// otherwise.
									if(((ProgramProfile->ReturnPrefix.UseIfNoData) && (IPB->WideStringOperand1->Size == 0)) ||
										((ProgramProfile->ReturnPrefix.UseIfData) && (IPB->WideStringOperand1->Size > 0)))
									{
										ProgramProfile->ReturnWideString->Append(ProgramProfile->ReturnPrefix.String);
									}
									ProgramProfile->ReturnWideString->Append(IPB->WideStringOperand1);
									if(((ProgramProfile->ReturnSuffix.UseIfNoData) && (IPB->WideStringOperand1->Size == 0)) ||
										((ProgramProfile->ReturnSuffix.UseIfData) && (IPB->WideStringOperand1->Size > 0)))
									{
										ProgramProfile->ReturnWideString->Append(ProgramProfile->ReturnSuffix.String);
									}

									SendToHostParameters.String = ( ProgramProfile->ReturnWideString->WChar );
									SendToHostParameters.LengthOfString = ( (int) ProgramProfile->ReturnWideString->Size );
									ProgramProfile->ReturnBool =
											SendToHost( &SendToHostParameters );
								}
							}
							break;


						case HHPL_CommunicationsSubInst_SendWithControlFlags:
						//		Param1:		Command that returns a string operand
						//		Param2:		Command that returns a prefix operand
						//		Param3:		Command that returns a suffix operand
						//		Param4:		Command that returns a string operand
						//		Param5:		Command that returns a numeric operand (control mask)
							if(GetParameters(ProgramProfile, IPB, "send", 5,
								HHPL_PROC_GET_WIDE_STRING_PARAMETER, HHPL_PROC_PREFIX_RETURN_PARAMETER,
								HHPL_PROC_SUFFIX_RETURN_PARAMETER, HHPL_PROC_GET_WIDE_STRING_PARAMETER,
								HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0))
							{
								bool						DelayIsOK = true;

								SendToHostParameters.DataSourceInformation = ( IPB->WideStringOperand2->WChar );
								SendToHostParameters.ControlMask = ((int) IPB->IntegerOperand1);
								SendToHostParameters.DelayInMicroseconds = 0; 

							// If a microsecond delay is included, get that parameter too.
								if( ((unsigned int) IPB->IntegerOperand1) & HHPL_SEND_TO_HOST_USE_MICROSECOND_DELAY )
								{
									DelayIsOK = GetParameters( ProgramProfile, IPB, "send", 1, HHPL_PROC_GET_NUMERIC_PARAMETER,
												0, 0, 0, 0, 0, 0 );
									SendToHostParameters.DelayInMicroseconds = ((int) IPB->IntegerOperand1);
								}

							// If we got the delay OK, and we get the control flags parameter, keep going.
								if(( DelayIsOK ) &&
										GetParameters( ProgramProfile, IPB, "send", 1, HHPL_PROC_GET_NUMERIC_PARAMETER,
												0, 0, 0, 0, 0, 0 ))
								{
									SendToHostParameters.ControlFlags = ((int) IPB->IntegerOperand1);

								// Override indicator bit in control flags if specified in control mask.
									if( SendToHostParameters.ControlMask & HHPL_SEND_TO_HOST_NO_INDICATORS )
									{
										SendToHostParameters.ControlFlags |= HHPL_INPUT_AND_SEND_TO_HOST_NO_INDICATORS;
									}
									else if( SendToHostParameters.ControlMask & HHPL_SEND_TO_HOST_USE_INDICATORS )
									{
										SendToHostParameters.ControlFlags &= ~HHPL_INPUT_AND_SEND_TO_HOST_NO_INDICATORS;
									}

									ProgramProfile->ReturnWideString->Copy( L"" );

								// For the time being, let's concatenate the prefix, decoding result,
								// and suffix into one big string, because SendToHost can't handle it
								// otherwise.
									if(((ProgramProfile->ReturnPrefix.UseIfNoData) && (IPB->WideStringOperand1->Size == 0)) ||
										((ProgramProfile->ReturnPrefix.UseIfData) && (IPB->WideStringOperand1->Size > 0)))
									{
										ProgramProfile->ReturnWideString->Append(ProgramProfile->ReturnPrefix.String);
									}
									ProgramProfile->ReturnWideString->Append(IPB->WideStringOperand1);
									if(((ProgramProfile->ReturnSuffix.UseIfNoData) && (IPB->WideStringOperand1->Size == 0)) ||
										((ProgramProfile->ReturnSuffix.UseIfData) && (IPB->WideStringOperand1->Size > 0)))
									{
										ProgramProfile->ReturnWideString->Append(ProgramProfile->ReturnSuffix.String);
									}

									SendToHostParameters.String = ( ProgramProfile->ReturnWideString->WChar );
									SendToHostParameters.LengthOfString = ( (int) ProgramProfile->ReturnWideString->Size );
										ProgramProfile->ReturnBool =
													SendToHost( &SendToHostParameters );
								}
							}
							break;

						case HHPL_CommunicationsSubInst_RadioSignalStrength:
						//		Byte 0:		HHPL_CommunicationsSubInst_RadioSignalStrength instruction
							ProgramProfile->ReturnNumber = RadioStrength();
							break;

						case HHPL_CommunicationsSubInst_RadioTransmissionFailures:
						//		Byte 0:		HHPL_CommunicationsSubInst_RadioTransmissionFailures instruction
							ProgramProfile->ReturnNumber = TxSuccess();
							break;

						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized communications operation sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;





			case HHPL_Inst_SpecialOperations:
		//		Param1:		Special operations sub instruction
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "communications operation", 1,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_SpecialOperationSubInst_DisplayMenu:
						//		Param1:		Command that returns a numeric operand (upper left X-coord)
						//		Param2:		Command that returns a numeric operand (upper left Y-coord)
						//		Param3:		Command that returns a string operand (title)
						//		Param4:		Command that returns a numeric operand (title X-coord)
						//		Param5:		Command that returns a numeric operand (title Y-coord)
						//		Param6:		Command that returns a numeric operand (title control mask)
						//		Param7:		Command that returns a numeric operand (# of menu items)
						//			Menu items:
						//				Param1:	Command that returns a string operand (menu item label)
						//				Param2:	Command that returns a string operand (menu item hotkey)
						//				Param3:	Command that returns a numeric operand (menu item X-coord)
						//				Param4:	Command that returns a numeric operand (menu item Y-coord)
						//				Param5:	Command that returns a numeric operand (menu item control mask)
						//		Param8:		Command that returns a numeric operand (max # of display lines to use)
						//		Param9:		Command that returns a numeric operand (timeout)
						//		Param10:	Command that returns a numeric operand (menu control mask)
							if(GetParameters(ProgramProfile, IPB, "display menu", 7,
								HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
								HHPL_PROC_GET_WIDE_STRING_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
								HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
								HHPL_PROC_GET_NUMERIC_PARAMETER))
							{
							// Allocate space for the parameters
								MenuParameters = ((MENU_PARAMETERS *) malloc(sizeof(MENU_PARAMETERS)));

								MenuParameters->Origin.X = IPB->IntegerOperand1;
								MenuParameters->Origin.Y = IPB->IntegerOperand2;
								MenuParameters->Title.Text = ((wchar_t *) malloc(( IPB->WideStringOperand1->Size + 1 ) * 2 ));
								IPB->WideStringOperand1->CopyFromString(MenuParameters->Title.Text, ((unsigned int) MenuParameters->Title.TextSize));
								MenuParameters->Title.TextSize = IPB->WideStringOperand1->Size;
								MenuParameters->Title.Origin.X = IPB->IntegerOperand3;
								MenuParameters->Title.Origin.Y = IPB->IntegerOperand4;
								MenuParameters->Title.ControlMask = IPB->IntegerOperand5;
								MenuParameters->NumberOfMenuItems = IPB->IntegerOperand6;
								MenuParameters->MenuItems =
										((MENU_ITEM *) malloc
											(((unsigned int) (sizeof(MENU_ITEM) * (MenuParameters->NumberOfMenuItems + 1)))));

							// Get the menu items
								for(i = 0;    ((i < MenuParameters->NumberOfMenuItems) &&
										(!ProgramProfile->UnrecoverableError));    ++i)
								{
									if(GetParameters(ProgramProfile, IPB, "display menu", 5,
										HHPL_PROC_GET_WIDE_STRING_PARAMETER, HHPL_PROC_GET_WIDE_STRING_PARAMETER,
										HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
										HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0))
									{
									// Setup the menu item parameters
										MenuParameters->MenuItems[i].Text = ((wchar_t *) malloc(( IPB->WideStringOperand1->Size + 1 ) * 2));
										MenuParameters->MenuItems[i].TextSize = IPB->WideStringOperand1->Size;
										IPB->WideStringOperand1->CopyFromString(MenuParameters->MenuItems[i].Text,
											((unsigned int) MenuParameters->MenuItems[i].TextSize));
										MenuParameters->MenuItems[i].Hotkey = ((wchar_t *) malloc(( IPB->WideStringOperand2->Size + 1 ) * 2));
										MenuParameters->MenuItems[i].HotkeySize = IPB->WideStringOperand2->Size;
										IPB->WideStringOperand2->CopyFromString(MenuParameters->MenuItems[i].Hotkey,
											((unsigned int) MenuParameters->MenuItems[i].HotkeySize));
										MenuParameters->MenuItems[i].Origin.X = IPB->IntegerOperand1;
										MenuParameters->MenuItems[i].Origin.Y = IPB->IntegerOperand2;
										MenuParameters->MenuItems[i].ControlMask = IPB->IntegerOperand3;
									}
									else
									{
										ProgramProfile->UnrecoverableError = true;
									}
								}


							// Get the remaining parameters, display the menu, and process return information.
								if(!ProgramProfile->UnrecoverableError)
								{
									if(GetParameters(ProgramProfile, IPB, "display menu", 3,
										HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
										HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0))
									{
										MenuParameters->MaximumNumberOfDisplayLinesToUse = IPB->IntegerOperand1;
										MenuParameters->TimeoutOnNoSelection = IPB->IntegerOperand2;
										MenuParameters->ControlMask = IPB->IntegerOperand3;

										ProgramProfile->ReturnNumber =
											DisplayMenu(MenuParameters, ProgramProfile->ReturnWideString->WChar,
													&(ProgramProfile->ReturnWideString->Size));
									}
									else
									{
										ProgramProfile->UnrecoverableError = true;
									}
								}

							// Free memory acquired as part of this process.
								for(i = 0;    (i < MenuParameters->NumberOfMenuItems);    ++i)
								{
									free(MenuParameters->MenuItems[i].Text);
									free(MenuParameters->MenuItems[i].Hotkey);
								}
								free(MenuParameters->MenuItems);
								free(MenuParameters->Title.Text);
								free(MenuParameters);
							}
							break;

						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized special operation sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;





			case HHPL_Inst_PowerManagementOperations:
		//		Param1:		Power management operation sub instruction
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "power management operation", 1,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_PowerManagementSubInst_BatteryStrength:
						//		Byte 0:		HHPL_PowerManagementSubInst_BatteryStrength instruction
							ProgramProfile->ReturnNumber = BatteryStrength();
							break;

						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized power management operation sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;





			case HHPL_Inst_OSConfigurationOperations:
		//		Param1:		O/S configuration operations sub instruction
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "O/S configuration operation", 1,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_OSConfigurationSubInst_MenuCommand:
						//		Param1:		Command that returns the first string operand
							if(GetParameters(ProgramProfile, IPB, "menu command", 1,
											HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
								ProgramProfile->ReturnBool = MenuCommand( ProgramProfile->ReturnWideString->WChar,
									((int *) &(ProgramProfile->ReturnWideString->Size)),
									((int) ProgramProfile->ReturnWideString->MaximumLength()));

							// If the command didn't work, zero the result string so that the
							// application will know that an error occurred.
								if(!(ProgramProfile->ReturnBool))
								{
									ProgramProfile->ReturnWideString->Size = 0;
								}
							}
							break;

						case HHPL_OSConfigurationSubInst_DisplayLanguage:
							DisplayLanguage( ProgramProfile->ReturnWideString->WChar );
							ProgramProfile->ReturnWideString->Size =
												wcslen( ProgramProfile->ReturnWideString->WChar );
							break;

						case HHPL_OSConfigurationSubInst_InterpreterVersion:
							{
								ProgramProfile->ReturnWideString->Copy(HHPL_INTERPRETER_VERSION);
							}
							break;

						case HHPL_OSConfigurationSubInst_ScannerID:
							ScannerID( ProgramProfile->ReturnWideString->WChar );
							ProgramProfile->ReturnWideString->Size =
												wcslen( ProgramProfile->ReturnWideString->WChar );
							break;

						case HHPL_OSConfigurationSubInst_Workgroup:
						//		Byte 0:		HHPL_OSConfigurationSubInst_Workgroup instruction
							ProgramProfile->ReturnNumber = Workgroup();
							break;

						case HHPL_OSConfigurationSubInst_TerminalID:
						//		Byte 0:		HHPL_OSConfigurationSubInst_TerminalID instruction
							ProgramProfile->ReturnNumber = TerminalID();
							break;

						case HHPL_OSConfigurationSubInst_GetEditBufferControlFlags:
						//		Byte 0:		HHPL_OSConfigurationSubInst_GetEditBufferControlFlags instruction
							ProgramProfile->ReturnNumber = ProgramProfile->EditBufferControlFlags;
							break;

						case HHPL_OSConfigurationSubInst_SetEditBufferControlFlags:
						//		Param1:		Command that returns a numeric operand
							if(GetParameters(ProgramProfile, IPB, "set edit buffer control flags", 1,
										HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
								if(!SkipOverInstructionsWithoutExecutingThem)
								{
								// We successfully executed the instruction; now, update the control flags variable.
									ProgramProfile->EditBufferControlFlags = ProgramProfile->ReturnNumber;
								}
							}
							break;

						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized O/S configuration operation sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;





			case HHPL_Inst_OSStatusOperations:
		//		Param1:		O/S status operations sub instruction
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "O/S status operation", 1,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_OSStatusSubInst_LastOperation:
						//		Byte 0:		HHPL_Inst_LastOperation instruction
							ProgramProfile->ReturnNumber = LastSystemOperationStatus();
							break;

						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized O/S status operation sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;





			case HHPL_Inst_RemoteHostOperations:
		//		Param1:		Remote host operations sub instruction
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "remote host operation", 1,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_RemoteHostSubInst_CheckForCommand:
						//		Param1:		Control mask
						//		Param2:		Number of routine names
							if(GetParameters(ProgramProfile, IPB, "check for command", 2,
									HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0))
							{
								HostRoutineControlMask = ((unsigned int) IPB->IntegerOperand1);
								NumberOfHostAccessibleRoutines = ((unsigned int) IPB->IntegerOperand2);

							// Get all routine names
								if(NumberOfHostAccessibleRoutines > 0)
								{
									HostRoutineNames = new HWSTRING *[NumberOfHostAccessibleRoutines];
									for(i = 0;    i < NumberOfHostAccessibleRoutines;    ++i)
									{
										HostRoutineNames[i] = NULL;
									}

									i = 0;
									while((!ProgramProfile->UnrecoverableError) &&
																(i < NumberOfHostAccessibleRoutines))
									{
										if(GetParameters(ProgramProfile, IPB, "check for command", 1,
													HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
										{
											HostRoutineNames[i] = new HWSTRING( ProgramProfile->ReturnWideString, true);
											++i;
										}
									}
								}

							// If we successfully loaded the routine names, go wait for a command.
								if(!ProgramProfile->UnrecoverableError)
								{
									HostRoutineProgramProfile = ProgramProfile;
									HostRoutineIPB = IPB;
									CheckForCommand( ProgramProfile->ReturnWideString );
								}


							// Delete storage space associated with routine names
								if(NumberOfHostAccessibleRoutines > 0)
								{
									i = 0;
									while(i < NumberOfHostAccessibleRoutines)
									{
										if(HostRoutineNames[i] != NULL)
										{
											delete HostRoutineNames[i];
										}
										++i;
									}

									delete HostRoutineNames;
								}
							}
							break;

						case HHPL_RemoteHostSubInst_WaitForCommand:
						//		Param1:		Control mask
						//		Param2:		Number of routine names
							if(GetParameters(ProgramProfile, IPB, "wait for command", 2,
									HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0))
							{
								HostRoutineControlMask = ((unsigned int) IPB->IntegerOperand1);
								NumberOfHostAccessibleRoutines = ((unsigned int) IPB->IntegerOperand2);

							// Get all routine names
								if(NumberOfHostAccessibleRoutines > 0)
								{
									HostRoutineNames = new HWSTRING *[NumberOfHostAccessibleRoutines];
									for(i = 0;    i < NumberOfHostAccessibleRoutines;    ++i)
									{
										HostRoutineNames[i] = NULL;
									}

									i = 0;
									while((!ProgramProfile->UnrecoverableError) &&
																(i < NumberOfHostAccessibleRoutines))
									{
										if(GetParameters(ProgramProfile, IPB, "wait for command", 1,
													HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
										{
											HostRoutineNames[i] = new HWSTRING( ProgramProfile->ReturnWideString, true );
											++i;
										}
									}
								}

							// If we successfully loaded the routine names, go wait for a command.
								if(!ProgramProfile->UnrecoverableError)
								{
									HostRoutineProgramProfile = ProgramProfile;
									HostRoutineIPB = IPB;
									WaitForCommand( ProgramProfile->ReturnWideString );
								}


							// Delete storage space associated with routine names
								if(NumberOfHostAccessibleRoutines > 0)
								{
									i = 0;
									while(i < NumberOfHostAccessibleRoutines)
									{
										if(HostRoutineNames[i] != NULL)
										{
											delete HostRoutineNames[i];
										}
										++i;
									}

									delete HostRoutineNames;
								}
							}
							break;


						case HHPL_RemoteHostSubInst_WaitForMultipleCommands:
						//		Param1:		Control mask
						//		Param2:		Number of routine names
							if(GetParameters(ProgramProfile, IPB, "wait for multiple commands", 2,
									HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0))
							{
								HostRoutineControlMask = ((unsigned int) IPB->IntegerOperand1);
								NumberOfHostAccessibleRoutines = ((unsigned int) IPB->IntegerOperand2);

							// Get all routine names
								if(NumberOfHostAccessibleRoutines > 0)
								{
									HostRoutineNames = new HWSTRING *[NumberOfHostAccessibleRoutines];
									for(i = 0;    i < NumberOfHostAccessibleRoutines;    ++i)
									{
										HostRoutineNames[i] = NULL;
									}

									i = 0;
									while((!ProgramProfile->UnrecoverableError) &&
																(i < NumberOfHostAccessibleRoutines))
									{
										if(GetParameters(ProgramProfile, IPB, "wait for multiple commands", 1,
													HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
										{
											HostRoutineNames[i] = new HWSTRING( ProgramProfile->ReturnWideString, true );
											++i;
										}
									}
								}

							// If we successfully loaded the routine names, go wait for a command.
								if(!ProgramProfile->UnrecoverableError)
								{
									HostRoutineProgramProfile = ProgramProfile;
									HostRoutineIPB = IPB;
									WaitForMultipleCommands( ProgramProfile->ReturnWideString );
								}


							// Delete storage space associated with routine names
								if(NumberOfHostAccessibleRoutines > 0)
								{
									i = 0;
									while(i < NumberOfHostAccessibleRoutines)
									{
										if(HostRoutineNames[i] != NULL)
										{
											delete HostRoutineNames[i];
										}
										++i;
									}

									delete HostRoutineNames;
								}
							}
							break;


						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized remote host operation sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;





			case HHPL_Inst_PrefixSuffixDefault:
		//		Param1:		Prefix / suffix default operations sub instruction
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "prefix / suffix default operation", 1,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_PrefixSuffixDefaultSubInst_DefineDefaultPrefixToBeAlwaysApplied:
						//		Param1:		Command that returns a string operand
							if(GetParameters(ProgramProfile, IPB, "define default prefix always", 1,
										HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
							// We have a string to work with; now, set up the default prefix
								ProgramProfile->DefaultPrefix.UseIfNoData = true;
								ProgramProfile->DefaultPrefix.UseIfData = true;
								ProgramProfile->DefaultPrefix.String->Copy( ProgramProfile->ReturnWideString );
							}
							break;

						case HHPL_PrefixSuffixDefaultSubInst_DefineDefaultPrefixToBeAppliedOnNonNullData:
						//		Param1:		Command that returns a string operand
							if(GetParameters(ProgramProfile, IPB, "define default prefix if data", 1,
										HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
							// We have a string to work with; now, set up the default prefix
								ProgramProfile->DefaultPrefix.UseIfNoData = false;
								ProgramProfile->DefaultPrefix.UseIfData = true;
								ProgramProfile->DefaultPrefix.String->Copy( ProgramProfile->ReturnWideString );
							}
							break;

						case HHPL_PrefixSuffixDefaultSubInst_DefineDefaultPrefixToBeAppliedOnNullData:
						//		Param1:		Command that returns a string operand
							if(GetParameters(ProgramProfile, IPB, "define default prefix if no data", 1,
										HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
							// We have a string to work with; now, set up the default prefix
								ProgramProfile->DefaultPrefix.UseIfNoData = true;
								ProgramProfile->DefaultPrefix.UseIfData = false;
								ProgramProfile->DefaultPrefix.String->Copy( ProgramProfile->ReturnWideString );
							}
							break;

						case HHPL_PrefixSuffixDefaultSubInst_DefineDefaultSuffixToBeAlwaysApplied:
						//		Param1:		Command that returns a string operand
							if(GetParameters(ProgramProfile, IPB, "define default suffix always", 1,
										HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
							// We have a string to work with; now, set up the default suffix
								ProgramProfile->DefaultSuffix.UseIfNoData = true;
								ProgramProfile->DefaultSuffix.UseIfData = true;
								ProgramProfile->DefaultSuffix.String->Copy( ProgramProfile->ReturnWideString );
							}
							break;

						case HHPL_PrefixSuffixDefaultSubInst_DefineDefaultSuffixToBeAppliedOnNonNullData:
						//		Param1:		Command that returns a string operand
							if(GetParameters(ProgramProfile, IPB, "define default suffix if data", 1,
										HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
							// We have a string to work with; now, set up the default suffix
								ProgramProfile->DefaultSuffix.UseIfNoData = false;
								ProgramProfile->DefaultSuffix.UseIfData = true;
								ProgramProfile->DefaultSuffix.String->Copy( ProgramProfile->ReturnWideString );
							}
							break;

						case HHPL_PrefixSuffixDefaultSubInst_DefineDefaultSuffixToBeAppliedOnNullData:
						//		Param1:		Command that returns a string operand
							if(GetParameters(ProgramProfile, IPB, "define default suffix if no data", 1,
										HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
								{
								// We have a string to work with; now, set up the default suffix
									ProgramProfile->DefaultSuffix.UseIfNoData = true;
									ProgramProfile->DefaultSuffix.UseIfData = false;
									ProgramProfile->DefaultSuffix.String->Copy( ProgramProfile->ReturnWideString );
							}
							break;

						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized prefix / suffix default operation sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;




			case HHPL_Inst_PrefixSuffix:
		//		Param1:		Prefix / suffix operations sub instruction
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "prefix / suffix operation", 1,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_PrefixSuffixSubInst_UseDefaultPrefix:
						//		Byte 0:		HHPL_Inst_UseDefaultPrefix instruction
							ProgramProfile->ReturnPrefix.String->Copy(ProgramProfile->DefaultPrefix.String);
							ProgramProfile->ReturnPrefix.UseIfNoData = ProgramProfile->DefaultPrefix.UseIfNoData;
							ProgramProfile->ReturnPrefix.UseIfData = ProgramProfile->DefaultPrefix.UseIfData;
							break;

						case HHPL_PrefixSuffixSubInst_UseDefaultSuffix:
						//		Byte 0:		HHPL_Inst_UseDefaultSuffix instruction
							ProgramProfile->ReturnSuffix.String->Copy(ProgramProfile->DefaultSuffix.String);
							ProgramProfile->ReturnSuffix.UseIfNoData = ProgramProfile->DefaultSuffix.UseIfNoData;
							ProgramProfile->ReturnSuffix.UseIfData = ProgramProfile->DefaultSuffix.UseIfData;
							break;

						case HHPL_PrefixSuffixSubInst_UseImmediatePrefixAlways:
						//		Param1:		Command that returns a string operand
							if(GetParameters(ProgramProfile, IPB, "immediate prefix always", 1,
										HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
							// We have a string to work with; now, set up the immediate prefix
								ProgramProfile->ReturnPrefix.UseIfNoData = true;
								ProgramProfile->ReturnPrefix.UseIfData = true;
								ProgramProfile->ReturnPrefix.String->Copy( ProgramProfile->ReturnWideString );
							}
							break;

						case HHPL_PrefixSuffixSubInst_UseImmediatePrefixOnNonNullData:
						//		Param1:		Command that returns a string operand
							if(GetParameters(ProgramProfile, IPB, "immediate prefix if data", 1,
										HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
							// We have a string to work with; now, set up the immediate prefix
								ProgramProfile->ReturnPrefix.UseIfNoData = false;
								ProgramProfile->ReturnPrefix.UseIfData = true;
								ProgramProfile->ReturnPrefix.String->Copy( ProgramProfile->ReturnWideString );
							}
							break;

						case HHPL_PrefixSuffixSubInst_UseImmediatePrefixOnNullData:
						//		Param1:		Command that returns a string operand
							if(GetParameters(ProgramProfile, IPB, "immediate prefix if data", 1,
										HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
							// We have a string to work with; now, set up the immediate prefix
								ProgramProfile->ReturnPrefix.UseIfNoData = false;
								ProgramProfile->ReturnPrefix.UseIfData = true;
								ProgramProfile->ReturnPrefix.String->Copy( ProgramProfile->ReturnWideString );
							}
							break;

						case HHPL_PrefixSuffixSubInst_UseImmediateSuffixAlways:
						//		Param1:		Command that returns a string operand
							if(GetParameters(ProgramProfile, IPB, "immediate suffix always", 1,
										HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
							// We have a string to work with; now, set up the immediate suffix
								ProgramProfile->ReturnSuffix.UseIfNoData = true;
								ProgramProfile->ReturnSuffix.UseIfData = true;
								ProgramProfile->ReturnSuffix.String->Copy( ProgramProfile->ReturnWideString );
							}
							break;

						case HHPL_PrefixSuffixSubInst_UseImmediateSuffixOnNonNullData:
						//		Param1:		Command that returns a string operand
							if(GetParameters(ProgramProfile, IPB, "immediate suffix if data", 1,
										HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
							// We have a string to work with; now, set up the immediate suffix
								ProgramProfile->ReturnSuffix.UseIfNoData = false;
								ProgramProfile->ReturnSuffix.UseIfData = true;
								ProgramProfile->ReturnSuffix.String->Copy( ProgramProfile->ReturnWideString );
							}
							break;

						case HHPL_PrefixSuffixSubInst_UseImmediateSuffixOnNullData:
						//		Param1:		Command that returns a string operand
							if(GetParameters(ProgramProfile, IPB, "immediate suffix if no data", 1,
										HHPL_PROC_WIDE_STRING_RETURN_PARAMETER, 0, 0, 0, 0, 0, 0))
							{
							// We have a string to work with; now, set up the immediate suffix
								ProgramProfile->ReturnSuffix.UseIfNoData = true;
								ProgramProfile->ReturnSuffix.UseIfData = false;
								ProgramProfile->ReturnSuffix.String->Copy( ProgramProfile->ReturnWideString );
							}
							break;

						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized prefix / suffix operation sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;




			case HHPL_Inst_CompareStringOperations:
		//		Param1:		String comparison operations sub instruction
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "string comparison operation", 1,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_StringComparisonSubInst_CaseSensitiveCompareString:
						//		Param1:		Compare string operation sub instruction
						//		Param2:		Command that returns the first string operand
						//		Param3:		Command that returns the second string operand
							if(GetParameters(ProgramProfile, IPB, "case sensitive string compare operation", 3,
								HHPL_PROC_SUBINSTRUCTION_PARAMETER,
								HHPL_PROC_GET_STRING_PARAMETER, HHPL_PROC_GET_STRING_PARAMETER, 0, 0, 0, 0))
							{
							// If successful, perform the operation specified in the sub-instruction
								switch(IPB->SubinstructionOperand1)
								{
									case HHPL_LogicalSubInst_Op1_EQ_Op2:
										ProgramProfile->ReturnBool =
												(IPB->StringOperand1->Compare(IPB->StringOperand2, true) == 0) ? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_LE_Op2:
										ProgramProfile->ReturnBool =
												(IPB->StringOperand1->Compare(IPB->StringOperand2, true) >= 0) ? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_GE_Op2:
										ProgramProfile->ReturnBool =
												(IPB->StringOperand1->Compare(IPB->StringOperand2, true) <= 0) ? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_LT_Op2:
										ProgramProfile->ReturnBool =
												(IPB->StringOperand1->Compare(IPB->StringOperand2, true) > 0) ? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_GT_Op2:
										ProgramProfile->ReturnBool =
												(IPB->StringOperand1->Compare(IPB->StringOperand2, true) < 0) ? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_NE_Op2:
										ProgramProfile->ReturnBool =
												(IPB->StringOperand1->Compare(IPB->StringOperand2, true) != 0) ? true : false;
										break;

									default:
										{
										ProgramProfile->UnrecoverableError = true;

										sprintf(TempStr, "Unrecognized case sensitive string comparison operation sub-instruction: IP = %08X",
											ProgramProfile->NextInstruction);
										LogError(TempStr);
										}
										break;
								};
							}
							break;

						case HHPL_StringComparisonSubInst_CaseInsensitiveCompareString:
						//		Param1:		Compare string operation sub instruction
						//		Param2:		Command that returns the first string operand
						//		Param3:		Command that returns the second string operand
							if(GetParameters(ProgramProfile, IPB, "case insensitive string compare operation", 3,
								HHPL_PROC_SUBINSTRUCTION_PARAMETER,
								HHPL_PROC_GET_STRING_PARAMETER, HHPL_PROC_GET_STRING_PARAMETER, 0, 0, 0, 0))
							{
							// If successful, perform the operation specified in the sub-instruction
								switch(IPB->SubinstructionOperand1)
								{
									case HHPL_LogicalSubInst_Op1_EQ_Op2:
										ProgramProfile->ReturnBool =
												(IPB->StringOperand1->Compare(IPB->StringOperand2, false) == 0) ? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_LE_Op2:
										ProgramProfile->ReturnBool =
												(IPB->StringOperand1->Compare(IPB->StringOperand2, false) >= 0) ? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_GE_Op2:
										ProgramProfile->ReturnBool =
												(IPB->StringOperand1->Compare(IPB->StringOperand2, false) <= 0) ? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_LT_Op2:
										ProgramProfile->ReturnBool =
												(IPB->StringOperand1->Compare(IPB->StringOperand2, false) > 0) ? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_GT_Op2:
										ProgramProfile->ReturnBool =
												(IPB->StringOperand1->Compare(IPB->StringOperand2, false) < 0) ? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_NE_Op2:
										ProgramProfile->ReturnBool =
												(IPB->StringOperand1->Compare(IPB->StringOperand2, false) != 0) ? true : false;
										break;

									default:
										ProgramProfile->UnrecoverableError = true;
										sprintf(TempStr, "Unrecognized case sensitive string comparison operation sub-instruction: IP = %08X",
											ProgramProfile->NextInstruction);
										LogError(TempStr);
										break;
								};
							}
							break;

						case HHPL_StringComparisonSubInst_CompareStringAccordingToCaseSensitivitySetting:
						//		Param1:		Compare string operation sub instruction
						//		Param2:		Command that returns the first string operand
						//		Param3:		Command that returns the second string operand
							if(GetParameters(ProgramProfile, IPB, "default case sensitivity string compare operation", 3,
								HHPL_PROC_SUBINSTRUCTION_PARAMETER,
								HHPL_PROC_GET_STRING_PARAMETER, HHPL_PROC_GET_STRING_PARAMETER, 0, 0, 0, 0))
							{
							// If successful, perform the operation specified in the sub-instruction
								switch(IPB->SubinstructionOperand1)
								{
									case HHPL_LogicalSubInst_Op1_EQ_Op2:
										ProgramProfile->ReturnBool =
												(IPB->StringOperand1->Compare(IPB->StringOperand2,
													ProgramProfile->StringComparisonsAreCaseSensitiveByDefault) == 0)
													? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_LE_Op2:
										ProgramProfile->ReturnBool =
												(IPB->StringOperand1->Compare(IPB->StringOperand2,
													ProgramProfile->StringComparisonsAreCaseSensitiveByDefault) >= 0)
													? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_GE_Op2:
										ProgramProfile->ReturnBool =
												(IPB->StringOperand1->Compare(IPB->StringOperand2,
													ProgramProfile->StringComparisonsAreCaseSensitiveByDefault) <= 0)
													? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_LT_Op2:
										ProgramProfile->ReturnBool =
												(IPB->StringOperand1->Compare(IPB->StringOperand2,
													ProgramProfile->StringComparisonsAreCaseSensitiveByDefault) > 0)
													? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_GT_Op2:
										ProgramProfile->ReturnBool =
												(IPB->StringOperand1->Compare(IPB->StringOperand2,
													ProgramProfile->StringComparisonsAreCaseSensitiveByDefault) < 0)
													? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_NE_Op2:
										ProgramProfile->ReturnBool =
												(IPB->StringOperand1->Compare(IPB->StringOperand2,
													ProgramProfile->StringComparisonsAreCaseSensitiveByDefault) != 0)
													? true : false;
										break;

									default:
										ProgramProfile->UnrecoverableError = true;
										sprintf(TempStr, "Unrecognized case sensitive string comparison operation sub-instruction: IP = %08X",
											ProgramProfile->NextInstruction);
										LogError(TempStr);
										break;
								};
							}
							break;

						case HHPL_StringComparisonSubInst_WildcardCaseSensitiveCompareString:
					//		Param1:		Command that returns the first string operand
					//		Param2:		Command that returns the second string operand
							if(GetParameters(ProgramProfile, IPB, "case sensitive string wildcard compare operation", 2,
										HHPL_PROC_GET_STRING_PARAMETER, HHPL_PROC_GET_STRING_PARAMETER, 0, 0, 0, 0, 0))
							{
								ProgramProfile->ReturnBool = IPB->StringOperand1->CompareWithWildcards(
											IPB->StringOperand2, true);
							}
							break;

						case HHPL_StringComparisonSubInst_WildcardCaseInsensitiveCompareString:
						//		Param1:		Command that returns the first string operand
						//		Param2:		Command that returns the second string operand
							if(GetParameters(ProgramProfile, IPB, "case insensitive string wildcard compare operation", 2,
									HHPL_PROC_GET_STRING_PARAMETER, HHPL_PROC_GET_STRING_PARAMETER, 0, 0, 0, 0, 0))
							{
								ProgramProfile->ReturnBool = IPB->StringOperand1->CompareWithWildcards(
											IPB->StringOperand2, false);
							}
							break;

						case HHPL_StringComparisonSubInst_WildcardCompareStringAccordingToCaseSensitivitySetting:
						//		Param1:		Command that returns the first string operand
						//		Param2:		Command that returns the second string operand
							if(GetParameters(ProgramProfile, IPB, "default case sensitivity string wildcard compare operation", 2,
									HHPL_PROC_GET_STRING_PARAMETER, HHPL_PROC_GET_STRING_PARAMETER, 0, 0, 0, 0, 0))
							{
								ProgramProfile->ReturnBool = IPB->StringOperand1->CompareWithWildcards(
											IPB->StringOperand2,
												ProgramProfile->StringComparisonsAreCaseSensitiveByDefault);
							}
							break;

						case HHPL_StringComparisonSubInst_CaseSensitiveCompareWideString:
						//		Param1:		Compare wide string operation sub instruction
						//		Param2:		Command that returns the first wide string operand
						//		Param3:		Command that returns the second wide string operand
							if(GetParameters(ProgramProfile, IPB, "case sensitive wide string compare operation", 3,
								HHPL_PROC_SUBINSTRUCTION_PARAMETER,
								HHPL_PROC_GET_WIDE_STRING_PARAMETER, HHPL_PROC_GET_WIDE_STRING_PARAMETER, 0, 0, 0, 0))
							{
							// If successful, perform the operation specified in the sub-instruction
								switch(IPB->SubinstructionOperand1)
								{
									case HHPL_LogicalSubInst_Op1_EQ_Op2:
										ProgramProfile->ReturnBool =
												(IPB->WideStringOperand1->Compare(IPB->WideStringOperand2, true) == 0) ? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_LE_Op2:
										ProgramProfile->ReturnBool =
												(IPB->WideStringOperand1->Compare(IPB->WideStringOperand2, true) >= 0) ? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_GE_Op2:
										ProgramProfile->ReturnBool =
												(IPB->WideStringOperand1->Compare(IPB->WideStringOperand2, true) <= 0) ? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_LT_Op2:
										ProgramProfile->ReturnBool =
												(IPB->WideStringOperand1->Compare(IPB->WideStringOperand2, true) > 0) ? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_GT_Op2:
										ProgramProfile->ReturnBool =
												(IPB->WideStringOperand1->Compare(IPB->WideStringOperand2, true) < 0) ? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_NE_Op2:
										ProgramProfile->ReturnBool =
												(IPB->WideStringOperand1->Compare(IPB->WideStringOperand2, true) != 0) ? true : false;
										break;

									default:
										{
										ProgramProfile->UnrecoverableError = true;

										sprintf(TempStr, "Unrecognized case sensitive wide string comparison operation sub-instruction: IP = %08X",
											ProgramProfile->NextInstruction);
										LogError(TempStr);
										}
										break;
								};
							}
							break;

						case HHPL_StringComparisonSubInst_CaseInsensitiveCompareWideString:
						//		Param1:		Compare wide string operation sub instruction
						//		Param2:		Command that returns the first wide string operand
						//		Param3:		Command that returns the second wide string operand
							if(GetParameters(ProgramProfile, IPB, "case insensitive wide string compare operation", 3,
								HHPL_PROC_SUBINSTRUCTION_PARAMETER,
								HHPL_PROC_GET_WIDE_STRING_PARAMETER, HHPL_PROC_GET_WIDE_STRING_PARAMETER, 0, 0, 0, 0))
							{
							// If successful, perform the operation specified in the sub-instruction
								switch(IPB->SubinstructionOperand1)
								{
									case HHPL_LogicalSubInst_Op1_EQ_Op2:
										ProgramProfile->ReturnBool =
												(IPB->WideStringOperand1->Compare(IPB->WideStringOperand2, false) == 0) ? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_LE_Op2:
										ProgramProfile->ReturnBool =
												(IPB->WideStringOperand1->Compare(IPB->WideStringOperand2, false) >= 0) ? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_GE_Op2:
										ProgramProfile->ReturnBool =
												(IPB->WideStringOperand1->Compare(IPB->WideStringOperand2, false) <= 0) ? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_LT_Op2:
										ProgramProfile->ReturnBool =
												(IPB->WideStringOperand1->Compare(IPB->WideStringOperand2, false) > 0) ? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_GT_Op2:
										ProgramProfile->ReturnBool =
												(IPB->WideStringOperand1->Compare(IPB->WideStringOperand2, false) < 0) ? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_NE_Op2:
										ProgramProfile->ReturnBool =
												(IPB->WideStringOperand1->Compare(IPB->WideStringOperand2, false) != 0) ? true : false;
										break;

									default:
										ProgramProfile->UnrecoverableError = true;
										sprintf(TempStr, "Unrecognized case sensitive wide string comparison operation sub-instruction: IP = %08X",
											ProgramProfile->NextInstruction);
										LogError(TempStr);
										break;
								};
							}
							break;

						case HHPL_StringComparisonSubInst_CompareWideStringAccordingToCaseSensitivitySetting:
						//		Param1:		Compare wide string operation sub instruction
						//		Param2:		Command that returns the first wide string operand
						//		Param3:		Command that returns the second wide string operand
							if(GetParameters(ProgramProfile, IPB, "default case sensitivity wide string compare operation", 3,
								HHPL_PROC_SUBINSTRUCTION_PARAMETER,
								HHPL_PROC_GET_WIDE_STRING_PARAMETER, HHPL_PROC_GET_WIDE_STRING_PARAMETER, 0, 0, 0, 0))
							{
							// If successful, perform the operation specified in the sub-instruction
								switch(IPB->SubinstructionOperand1)
								{
									case HHPL_LogicalSubInst_Op1_EQ_Op2:
										ProgramProfile->ReturnBool =
												(IPB->WideStringOperand1->Compare(IPB->WideStringOperand2,
													ProgramProfile->StringComparisonsAreCaseSensitiveByDefault) == 0)
													? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_LE_Op2:
										ProgramProfile->ReturnBool =
												(IPB->WideStringOperand1->Compare(IPB->WideStringOperand2,
													ProgramProfile->StringComparisonsAreCaseSensitiveByDefault) >= 0)
													? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_GE_Op2:
										ProgramProfile->ReturnBool =
												(IPB->WideStringOperand1->Compare(IPB->WideStringOperand2,
													ProgramProfile->StringComparisonsAreCaseSensitiveByDefault) <= 0)
													? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_LT_Op2:
										ProgramProfile->ReturnBool =
												(IPB->WideStringOperand1->Compare(IPB->WideStringOperand2,
													ProgramProfile->StringComparisonsAreCaseSensitiveByDefault) > 0)
													? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_GT_Op2:
										ProgramProfile->ReturnBool =
												(IPB->WideStringOperand1->Compare(IPB->WideStringOperand2,
													ProgramProfile->StringComparisonsAreCaseSensitiveByDefault) < 0)
													? true : false;
										break;

									case HHPL_LogicalSubInst_Op1_NE_Op2:
										ProgramProfile->ReturnBool =
												(IPB->WideStringOperand1->Compare(IPB->WideStringOperand2,
													ProgramProfile->StringComparisonsAreCaseSensitiveByDefault) != 0)
													? true : false;
										break;

									default:
										ProgramProfile->UnrecoverableError = true;
										sprintf(TempStr, "Unrecognized case sensitive string comparison operation sub-instruction: IP = %08X",
											ProgramProfile->NextInstruction);
										LogError(TempStr);
										break;
								};
							}
							break;

						case HHPL_StringComparisonSubInst_WildcardCaseSensitiveCompareWideString:
					//		Param1:		Command that returns the first wide string operand
					//		Param2:		Command that returns the second wide string operand
							if(GetParameters(ProgramProfile, IPB, "case sensitive wide string wildcard compare operation", 2,
										HHPL_PROC_GET_WIDE_STRING_PARAMETER, HHPL_PROC_GET_WIDE_STRING_PARAMETER, 0, 0, 0, 0, 0))
							{
								ProgramProfile->ReturnBool = IPB->WideStringOperand1->CompareWithWildcards(
											IPB->WideStringOperand2, true);
							}
							break;

						case HHPL_StringComparisonSubInst_WildcardCaseInsensitiveCompareWideString:
						//		Param1:		Command that returns the first wide string operand
						//		Param2:		Command that returns the second wide string operand
							if(GetParameters(ProgramProfile, IPB, "case insensitive string wildcard compare operation", 2,
									HHPL_PROC_GET_WIDE_STRING_PARAMETER, HHPL_PROC_GET_WIDE_STRING_PARAMETER, 0, 0, 0, 0, 0))
							{
								ProgramProfile->ReturnBool = IPB->WideStringOperand1->CompareWithWildcards(
											IPB->WideStringOperand2, false);
							}
							break;

						case HHPL_StringComparisonSubInst_WildcardCompareWideStringAccordingToCaseSensitivitySetting:
						//		Param1:		Command that returns the first wide string operand
						//		Param2:		Command that returns the second wide string operand
							if(GetParameters(ProgramProfile, IPB, "default case sensitivity wide string wildcard compare operation", 2,
									HHPL_PROC_GET_WIDE_STRING_PARAMETER, HHPL_PROC_GET_WIDE_STRING_PARAMETER, 0, 0, 0, 0, 0))
							{
								ProgramProfile->ReturnBool = IPB->WideStringOperand1->CompareWithWildcards(
											IPB->WideStringOperand2,
												ProgramProfile->StringComparisonsAreCaseSensitiveByDefault);
							}
							break;

						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized string comparison operation sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;




			case HHPL_Inst_ComparisonDefaults:
		//		Param1:		Comparison defaults sub instruction
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "comparison defaults", 1,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_ComparisonDefaultsSubInst_ComparesAreCaseSensitiveByDefault:
							ProgramProfile->StringComparisonsAreCaseSensitiveByDefault = true;
							break;

						case HHPL_ComparisonDefaultsSubInst_ComparesAreCaseInsensitiveByDefault:
							ProgramProfile->StringComparisonsAreCaseSensitiveByDefault = false;
							break;

						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized comparison default sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;




			case HHPL_Inst_TestRoutine:
		//		Param1:		Command that returns a numeric operand (test routine #)
		//		Param2:		Command that returns a numeric operand (# of parameters)
		//			Remaining parameters will be processed by ProcessTestRoutine.
				++ProgramProfile->NextInstruction;

				if(GetParameters(ProgramProfile, IPB, "test routine", 2,
						HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0))
				{
					ProcessTestRoutine(IPB->IntegerOperand1, IPB->IntegerOperand2,
																ProgramProfile, IPB);
				}
				break;




	// Format commands
			case HHPL_Inst_Format:
		//		Byte 0:		HHPL_Inst_Format instruction
		//		Byte 1:		Justification (or truncation) direction of formatted output;
		//					"R" for right justification or "L" for left justification
		//		Byte 2:		Fill character specifying character to be used to make up the difference
		//					when the format call for more characters to be reported than are in
		//					the input data; a NULL will result in no fill character being applied
		//		Byte 3, 4:	Total number of characters to be output;
		//					Note that the USHORT is stored little endian
		//		Byte 5..N+4:Command that returns a string where N is the number of bytes in
		//					the command; this command will specify the string to be formatted
				{
				unsigned char		Justification;
				unsigned char		FillCharacter;
				unsigned short		TotalNumberOfCharacters;

				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 5);	// Must be minimum of 5 instructions following

				Justification = ProgramProfile->Code[ProgramProfile->NextInstruction];
				++ProgramProfile->NextInstruction;

				FillCharacter = ProgramProfile->Code[ProgramProfile->NextInstruction];
				++ProgramProfile->NextInstruction;

				TotalNumberOfCharacters =
					((unsigned short) (
						((unsigned short)ProgramProfile->Code[ProgramProfile->NextInstruction+1]) * 256 +
						((unsigned short)ProgramProfile->Code[ProgramProfile->NextInstruction])));
				ProgramProfile->NextInstruction += 2;

			// Instruction following must return a string
				if(!ProgramProfile->UnrecoverableError)
				{
					if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Ret_WideString))
					{
						if(ExecuteNextInstruction(ProgramProfile, IPB))
						{
						// We have a string to work with; format it.

						// Right justification?
							if(Justification == 'R')
							{
							// If we're truncating, drop off characters at the beginning.
								if(ProgramProfile->ReturnWideString->Size >
														TotalNumberOfCharacters)
								{
									ProgramProfile->ReturnWideString->Remove(0,
										(ProgramProfile->ReturnWideString->Size - TotalNumberOfCharacters));
								}

							// If we're adding characters, and the fill character is non-NULL,
							// add the fill character at the beginning to bring us up to
							// TotalNumberOfCharacters.
								else if(FillCharacter != 0)
								{
									if(!ProgramProfile->ReturnWideString->InsertFillChar(((int) FillCharacter),
											(TotalNumberOfCharacters - ProgramProfile->ReturnWideString->Size)))
									{
									// Something went wrong while inserting characters; WOOF!
										ProgramProfile->UnrecoverableError = true;
										sprintf(TempStr, "Unable to add fill characters to format: IP = %08X",
												ProgramProfile->NextInstruction);
										LogError(TempStr);
									}
								}
							}

						// Left justification?
							else if(Justification == 'L')
							{
							// If we're truncating, drop off characters at the end.
								if(ProgramProfile->ReturnWideString->Size > TotalNumberOfCharacters)
								{
									ProgramProfile->ReturnWideString->Remove(TotalNumberOfCharacters,
										(ProgramProfile->ReturnWideString->Size - TotalNumberOfCharacters));
								}

							// If we're adding characters, and the fill character is non-NULL,
							// append the fill character to the end to bring us up to
							// TotalNumberOfCharacters.
								else if(FillCharacter != 0)
								{
									if(!ProgramProfile->ReturnWideString->AppendFillChar(((int) FillCharacter),
											(TotalNumberOfCharacters - ProgramProfile->ReturnWideString->Size)))
									{
									// Something went wrong while inserting characters; WOOF!
										ProgramProfile->UnrecoverableError = true;
										sprintf(TempStr, "Unable to add fill characters to format: IP = %08X",
												ProgramProfile->NextInstruction);
										LogError(TempStr);
									}
								}
							}
						}

						ProgramProfile->ReturnBool =
								(ProgramProfile->ReturnWideString->Size) ? true : false;
					}

				// This instruction handles byte strings as well.
					else if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Ret_String))
					{
						if(ExecuteNextInstruction(ProgramProfile, IPB))
						{
						// We have a string to work with; format it.

						// Right justification?
							if(Justification == 'R')
							{
							// If we're truncating, drop off characters at the beginning.
								if(ProgramProfile->ReturnString->Size >
														TotalNumberOfCharacters)
								{
									ProgramProfile->ReturnString->Remove(0,
										(ProgramProfile->ReturnString->Size - TotalNumberOfCharacters));
								}

							// If we're adding characters, and the fill character is non-NULL,
							// add the fill character at the beginning to bring us up to
							// TotalNumberOfCharacters.
								else if(FillCharacter != 0)
								{
									if(!ProgramProfile->ReturnString->InsertFillChar(((int) FillCharacter),
											(TotalNumberOfCharacters - ProgramProfile->ReturnString->Size)))
									{
									// Something went wrong while inserting characters; WOOF!
										ProgramProfile->UnrecoverableError = true;
										sprintf(TempStr, "Unable to add fill characters to format: IP = %08X",
												ProgramProfile->NextInstruction);
										LogError(TempStr);
									}
								}
							}

						// Left justification?
							else if(Justification == 'L')
							{
							// If we're truncating, drop off characters at the end.
								if(ProgramProfile->ReturnString->Size > TotalNumberOfCharacters)
								{
									ProgramProfile->ReturnString->Remove(TotalNumberOfCharacters,
										(ProgramProfile->ReturnString->Size - TotalNumberOfCharacters));
								}

							// If we're adding characters, and the fill character is non-NULL,
							// append the fill character to the end to bring us up to
							// TotalNumberOfCharacters.
								else if(FillCharacter != 0)
								{
									if(!ProgramProfile->ReturnString->AppendFillChar(((int) FillCharacter),
											(TotalNumberOfCharacters - ProgramProfile->ReturnString->Size)))
									{
									// Something went wrong while inserting characters; WOOF!
										ProgramProfile->UnrecoverableError = true;
										sprintf(TempStr, "Unable to add fill characters to format: IP = %08X",
												ProgramProfile->NextInstruction);
										LogError(TempStr);
									}
								}
							}
						}

						ProgramProfile->ReturnBool =
								(ProgramProfile->ReturnString->Size) ? true : false;
					}
					else
					{
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "Illegal Format command subsequent instruction: IP = %08X",
								ProgramProfile->NextInstruction);
						LogError(TempStr);
						ProgramProfile->ReturnBool = false;
					}
				}

				}

				break;



	// HHPL cursor move commands
			case HHPL_Inst_SetEditCursor:
		//		Param1:		Command that returns a numeric operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "set cursor", 1,
										HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0, 0))
				{
					if(!SkipOverInstructionsWithoutExecutingThem)
					{
					// We successfully executed the instruction; now, update the cursor.
						ChangeCursor(ProgramProfile, IPB->IntegerOperand1);
						ProgramProfile->ReturnNumber = ProgramProfile->Cursor;
					}
				}
				break;




	// HHPL find string commands
			case HHPL_Inst_FindString:
		//		Param1:		Search flags
		//		Param2:		Command that returns a string operand
		//		Param3:		Command that returns a string operand
		//		Param4:		Command that returns a numeric operand
		//		Param5:		Instruction that will set a numeric variable
				++ProgramProfile->NextInstruction;
				// If successful, perform the operation specified in the sub-instruction
				if(GetParameters(ProgramProfile, IPB, "find string", 4,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER,
							HHPL_PROC_GET_WIDE_STRING_PARAMETER, HHPL_PROC_GET_WIDE_STRING_PARAMETER,
							HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0))
				{
				// We have a search string, a string to search, a start position, and search
				// flags; it's time to do the search.
					bool CaseSensitivity;
					if(IPB->SubinstructionOperand1 &
								HHPL_FindStringSubInstFlags_UseDefaultCaseSensitivityForSearch)
					{
						CaseSensitivity = ProgramProfile->
												StringComparisonsAreCaseSensitiveByDefault;
					}
					else
					{
						if(IPB->SubinstructionOperand1 &
												HHPL_FindStringSubInstFlags_CaseSensitiveSearch)
						{
							CaseSensitivity = true;
						}
						else
						{
							CaseSensitivity = false;
						}
					}

					if(IPB->SubinstructionOperand1 &
												HHPL_FindStringSubInstFlags_SearchForward)
					{
						if(IPB->SubinstructionOperand1 &
												HHPL_FindStringSubInstFlags_WildcardSearch)
						{
			    			ProgramProfile->ReturnBool = IPB->WideStringOperand1->FindWildcardString(
			            			&(IPB->WideStringOperand2->WChar[IPB->IntegerOperand1]),
									((unsigned int) (IPB->WideStringOperand2->Size - IPB->IntegerOperand1)),
									CaseSensitivity,
									1,	// first occurrence moving forward
									&Position);
						}
						else
						{
		    				ProgramProfile->ReturnBool = IPB->WideStringOperand1->FindString(
									&(IPB->WideStringOperand2->WChar[IPB->IntegerOperand1]),
									((unsigned int) (IPB->WideStringOperand2->Size - IPB->IntegerOperand1)),
									CaseSensitivity,
									1,	// first occurrence moving forward
									&Position);
						}

					// We have to add the position where the string was
					// found to the starting position for the search.
						Position += ((unsigned int) IPB->IntegerOperand1);
					}
					else
					{
						if(IPB->SubinstructionOperand1 &
												HHPL_FindStringSubInstFlags_WildcardSearch)
						{
			    			ProgramProfile->ReturnBool = IPB->WideStringOperand1->FindWildcardString(
			            			IPB->WideStringOperand2->WChar,
									((unsigned int) IPB->IntegerOperand1 + 1),
									// Since we're searching backward,
									// we'll look at everything from
									// the beginning of the string up
									// to and including the start position
									// (hence the +1)
									CaseSensitivity,
									-1,	// first occurrence moving backward
									&Position);
						}
						else
						{
			    			ProgramProfile->ReturnBool = IPB->WideStringOperand1->FindString(
			            			IPB->WideStringOperand2->WChar,
									((unsigned int) (IPB->IntegerOperand1 + 1)),
									// Since we're searching backward,
									// we'll look at everything from
									// the beginning of the string up
									// to and including the start position
									// (hence the +1)
									CaseSensitivity,
									-1,	// first occurrence moving backward
									&Position);
						}
					}


				// If we found a string, update the return number.
					if(ProgramProfile->ReturnBool)
					{
						ProgramProfile->ReturnNumber = Position;
					}
					else
					{
						ProgramProfile->ReturnNumber = -1;
					}


				// Now we need to store the position.  The calling
				// instruction will get the position via the returned
				// number, but they may also want to store it somewhere
				// else.
					IntegerRegisters[0] = ProgramProfile->ReturnNumber;
					GetParameters(ProgramProfile, IPB, "find string", 1,
											HHPL_PROC_SET_NUMERIC_PARAMETER, 0, 0, 0, 0, 0, 0);
					ProgramProfile->ReturnNumber = IntegerRegisters[0];
				}
				break;




			case HHPL_Inst_GetEditCursor:
		//		Byte 0:		HHPL_Inst_GetEditCursor instruction
				++ProgramProfile->NextInstruction;
				ProgramProfile->ReturnNumber = ProgramProfile->Cursor;
				break;






	// HHPL control transfer commands
			case HHPL_Inst_IfCommand:
				{
		//		Byte 0:		HHPL_Inst_IfCommand instruction
		//		Byte 1..2:	Offset to false part of the IF (stored little endian)
		//		Byte 3..4:	Offset to instruction following the IF (stored little endian)
		//		Byte 5..N+4:	Command that returns a boolean where N is the number of bytes in
		//					the command; this command will specify whether to execute the true
		//					or false parts of the If command.
				unsigned int	OffsetToFalsePartOfIf=0;
				unsigned int	OffsetToInstructionFollowingIf=0;

				++ProgramProfile->NextInstruction;

				++ProgramProfile->IfThenElseLevel;

			// Get the offsets first.
				CheckForArguments(ProgramProfile, 4);	// Minimum of 4 bytes following.

				if(!ProgramProfile->UnrecoverableError)
				{
					OffsetToFalsePartOfIf =
							((unsigned int) ProgramProfile->Code
												[ProgramProfile->NextInstruction + 1]) * 256;
					OffsetToFalsePartOfIf +=
							((unsigned int) ProgramProfile->Code
												[ProgramProfile->NextInstruction]);
					ProgramProfile->NextInstruction += 2;

					OffsetToInstructionFollowingIf =
							((unsigned int) ProgramProfile->Code
												[ProgramProfile->NextInstruction + 1]) * 256;
					OffsetToInstructionFollowingIf +=
							((unsigned int) ProgramProfile->Code
												[ProgramProfile->NextInstruction]);
					ProgramProfile->NextInstruction += 2;

				// Check the integrity of the offsets.
					if(OffsetToFalsePartOfIf > OffsetToInstructionFollowingIf)
					{
					// According to the offsets, the false part falls AFTER the IF; WOOF WOOF WOOF...
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "If false offset beyond end of If: IP = %08X",
								ProgramProfile->NextInstruction);
						LogError(TempStr);
					}
					else if(OffsetToFalsePartOfIf < ProgramProfile->NextInstruction)
					{
					// According to the offsets, the false part falls BEFORE the current
					// intstruction pointer in the If, where the true and false parts are
					// defined.  WOOF WOOF WOOF...
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "If false offset pointer too low: IP = %08X",
								ProgramProfile->NextInstruction);
						LogError(TempStr);
					}
					else if(OffsetToInstructionFollowingIf > ProgramProfile->Size)
					{
					// According to the offsets, the offset to the next instruction following the
					// If is beyond the end of the HHPL code.  WOOF WOOF WOOF...
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "If extends beyond end of format code: IP = %08X",
								ProgramProfile->NextInstruction);
						LogError(TempStr);
					}
				}



				CheckForArguments(ProgramProfile, 1);	// Minimum of 1 instruction following.

			// First instruction after offsets must return a boolean
				if(!ProgramProfile->UnrecoverableError)
				{
					if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Ret_Boolean))
					{
						if(ExecuteNextInstruction(ProgramProfile, IPB))
						{
						// We successfully executed the If conditional, now let's execute either
						// the true or false section based on the conditional result.

							if(ProgramProfile->ReturnBool)
							{
							// If conditional returned true, execute all the instructions
							// in the true part.
								while((ProgramProfile->NextInstruction <
																	OffsetToFalsePartOfIf) &&
									(ExecuteNextInstruction(ProgramProfile, IPB)))
								{
								}
							}
							else
							{
							// Conditional returned false; execute all the instructions
							// in the false part.
								ProgramProfile->NextInstruction = ((unsigned int) OffsetToFalsePartOfIf);
								while((ProgramProfile->NextInstruction <
															OffsetToInstructionFollowingIf) &&
									(ExecuteNextInstruction(ProgramProfile, IPB)))
								{
								}
							}

						// Move the instruction pointer to the first instruction after
						// the If.
							ProgramProfile->NextInstruction = ((unsigned int) OffsetToInstructionFollowingIf);
						}
					}
					else
					{
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "Illegal If conditional instruction: IP = %08X",
								ProgramProfile->NextInstruction);
						LogError(TempStr);
					}
				}
				--ProgramProfile->IfThenElseLevel;
				}
				break;



			case HHPL_Inst_WhileCommand:
				{
		//		Byte 0:		HHPL_Inst_WhileCommand instruction
		//		Byte 1..2:	Offset to instruction following the While (stored little endian)
		//		Byte 3..N1+2:	Command that returns a boolean where N1 is the number of bytes in
		//					the command; this command will specify whether or not to continue
		//					in the while loop.
				unsigned int	OffsetToConditionalPartOfWhile;
				unsigned int	OffsetToInstructionFollowingWhile=0;

				++ProgramProfile->NextInstruction;
				++ProgramProfile->WhileLoopLevel;

			// Get the offsets first.
				CheckForArguments(ProgramProfile, 2);	// Minimum of 2 bytes following.

				if(!ProgramProfile->UnrecoverableError)
				{
					OffsetToInstructionFollowingWhile =
							((unsigned int) ProgramProfile->Code
												[ProgramProfile->NextInstruction + 1]) * 256;
					OffsetToInstructionFollowingWhile +=
							((unsigned int) ProgramProfile->Code
												[ProgramProfile->NextInstruction]);
					ProgramProfile->NextInstruction += 2;

				// Check the integrity of the offsets.
					if(OffsetToInstructionFollowingWhile <= ProgramProfile->NextInstruction)
					{
					// According to the offsets, the instruction following the While does not fall
					// after the next instruction.  There must be at least one more instruction
					// before the end of the While to compute the conditional.  WOOF WOOF WOOF...
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "Offset to next instruction beyond While is inside the While: IP = %08X",
								ProgramProfile->NextInstruction);
						LogError(TempStr);
					}
					else if(OffsetToInstructionFollowingWhile > ProgramProfile->Size)
					{
					// According to the offsets, the offset to the next instruction following the
					// While is beyond the end of the HHPL code.  WOOF WOOF WOOF...
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "While extends beyond end of format code: IP = %08X",
								ProgramProfile->NextInstruction);
						LogError(TempStr);
					}
				}


				OffsetToConditionalPartOfWhile = ProgramProfile->NextInstruction;

				CheckForArguments(ProgramProfile, 1);	// Minimum of 1 instruction following.

			// First instruction after offsets must return a boolean
				if(!ProgramProfile->UnrecoverableError)
				{
					if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Ret_Boolean))
					{
					// Keep computing the conditional and executing the loop contents until
					// the conditional is false or we have an error.
						while((ExecuteNextInstruction(ProgramProfile, IPB)) &&
													(ProgramProfile->ReturnBool))
						{
						// Execute all the instructions through the end of the loop.
							while((ProgramProfile->NextInstruction <
														OffsetToInstructionFollowingWhile) &&
									(ExecuteNextInstruction(ProgramProfile, IPB)))
							{
							}

						// Move instruction pointer back to conditional
							ProgramProfile->NextInstruction = ((unsigned int) OffsetToConditionalPartOfWhile);
						}

					// Move the instruction pointer to the first instruction after the While.
						ProgramProfile->NextInstruction = ((unsigned int) OffsetToInstructionFollowingWhile);
					}
					else
					{
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "Illegal While conditional instruction: IP = %08X",
								ProgramProfile->NextInstruction);
						LogError(TempStr);
					}
				}
				--ProgramProfile->WhileLoopLevel;
				}
				break;



			case HHPL_Inst_DoWhileCommand:
				{
		//		Byte 0:		HHPL_Inst_DoWhileCommand instruction
		//		Byte 1..2:	Offset to conditional instruction in DoWhile (stored little endian)
		//		Byte 3..4:	Offset to instruction following the DoWhile (stored little endian)
		//		Byte 5..N1+4:	Command that returns a boolean where N1 is the number of bytes in
		//					the command; this command will specify whether or not to continue
		//					in the DoWhile loop.
				unsigned int	OffsetToConditionalPartOfDoWhile=0;
				unsigned int	OffsetToInstructionFollowingDoWhile=0;
				unsigned int	OffsetToDoWhileLoopContents=0;

				++ProgramProfile->NextInstruction;
				++ProgramProfile->DoWhileLoopLevel;

			// Get the offsets first.
				CheckForArguments(ProgramProfile, 4);	// Minimum of 2 bytes following.

				if(!ProgramProfile->UnrecoverableError)
				{
					OffsetToConditionalPartOfDoWhile =
							((unsigned int) ProgramProfile->Code
												[ProgramProfile->NextInstruction + 1]) * 256;
					OffsetToConditionalPartOfDoWhile +=
							((unsigned int) ProgramProfile->Code
												[ProgramProfile->NextInstruction]);
					ProgramProfile->NextInstruction += 2;

					OffsetToInstructionFollowingDoWhile =
							((unsigned int) ProgramProfile->Code
												[ProgramProfile->NextInstruction + 1]) * 256;
					OffsetToInstructionFollowingDoWhile +=
							((unsigned int) ProgramProfile->Code
												[ProgramProfile->NextInstruction]);
					ProgramProfile->NextInstruction += 2;

				// Check the integrity of the offsets.
					if(OffsetToConditionalPartOfDoWhile > OffsetToInstructionFollowingDoWhile)
					{
					// According to the offsets, the conditional falls AFTER the DoWhile;
					// WOOF WOOF WOOF...
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "DoWhile conditional offset beyond end of the DoWhile: IP = %08X",
								ProgramProfile->NextInstruction);
						LogError(TempStr);
					}
					else if(OffsetToConditionalPartOfDoWhile < ProgramProfile->NextInstruction)
					{
					// According to the offsets, the conditional falls BEFORE the current
					// intstruction pointer in the DoWhile.  WOOF WOOF WOOF...
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "DoWhile conditional offset pointer too low: IP = %08X",
								ProgramProfile->NextInstruction);
						LogError(TempStr);
					}
					else if(OffsetToInstructionFollowingDoWhile > ProgramProfile->Size)
					{
					// According to the offsets, the offset to the next instruction following the
					// DoWhile is beyond the end of the HHPL code.  WOOF WOOF WOOF...
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "DoWhile extends beyond end of format code: IP = %08X",
								ProgramProfile->NextInstruction);
						LogError(TempStr);
					}
				}


				OffsetToDoWhileLoopContents = ProgramProfile->NextInstruction;

				CheckForArguments(ProgramProfile, 1);	// Minimum of 1 instruction following.

			// Conditional instruction must return a boolean
				if(!ProgramProfile->UnrecoverableError)
				{
					ProgramProfile->NextInstruction = ((unsigned int) OffsetToConditionalPartOfDoWhile);
					if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Ret_Boolean))
					{
					// Keep computing the conditional and executing the loop contents until
					// the conditional is false or we have an error.
						do
						{
							ProgramProfile->NextInstruction = ((unsigned int) OffsetToDoWhileLoopContents);

						// Execute all the instructions up to the conditional.
							while((ProgramProfile->NextInstruction <
														OffsetToConditionalPartOfDoWhile) &&
									(ExecuteNextInstruction(ProgramProfile, IPB)))
							{
							}

						// Move instruction pointer back to conditional
							ProgramProfile->NextInstruction = ((unsigned int) OffsetToConditionalPartOfDoWhile);
						}
						while((ExecuteNextInstruction(ProgramProfile, IPB)) &&
													(ProgramProfile->ReturnBool));

					// Move the instruction pointer to the first instruction after the DoWhile.
						ProgramProfile->NextInstruction = ((unsigned int) OffsetToInstructionFollowingDoWhile);
					}
					else
					{
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "Illegal DoWhile conditional instruction: IP = %08X",
								ProgramProfile->NextInstruction);
						LogError(TempStr);
					}
				}
				--ProgramProfile->DoWhileLoopLevel;
				}
				break;





			case HHPL_Inst_ExecuteSubroutine:
		//		Byte 0:		HHPL_Inst_ExecuteSubroutine instruction
		//		Byte 1..N:	Command that returns a string where N is the number of bytes in
		//					the command; this command will return the string to be used by
		//					the subroutine we're about to call
		//		Byte N+1..N+4:	Address of first instruction of subroutine (stored little endian).
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 1);	// Must be minimum of 1 instruction following

			// Next instruction following must return a string
				if(!ProgramProfile->UnrecoverableError)
				{
					if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Ret_WideString))
					{
						if(ExecuteNextInstruction(ProgramProfile, IPB))
						{
						// We have a string to work with; now we need to get the offset to the
						// subroutine and execute it.
							unsigned int	OffsetToSubroutine=0;

							CheckForArguments(ProgramProfile, 4);	// Minimum of 4 bytes following.

							if(!ProgramProfile->UnrecoverableError)
							{
								OffsetToSubroutine =
										((unsigned int) ProgramProfile->Code
												[ProgramProfile->NextInstruction + 3]);
								OffsetToSubroutine = (OffsetToSubroutine * 256) +
										((unsigned int) ProgramProfile->Code
												[ProgramProfile->NextInstruction + 2]);
								OffsetToSubroutine = (OffsetToSubroutine * 256) +
										((unsigned int) ProgramProfile->Code
												[ProgramProfile->NextInstruction + 1]);
								OffsetToSubroutine = (OffsetToSubroutine * 256) +
										((unsigned int) ProgramProfile->Code
												[ProgramProfile->NextInstruction]);
								ProgramProfile->NextInstruction += 4;

							// Check the integrity of the offset.
								if(OffsetToSubroutine >= ProgramProfile->Size)
								{
								// The offset to the subroutine is beyond the end of the HHPL
								// code.  WOOF WOOF WOOF...
									ProgramProfile->UnrecoverableError = true;
									sprintf(TempStr, "Subroutine location is beyond end of format code: IP = %08X",
											ProgramProfile->NextInstruction);
									LogError(TempStr);
								}
							}


						// If the offset is OK, let's save the current instruction pointer,
						// set the instruction pointer to the subroutine, run it, and restore
						// the instruction pointer to the next instruction following this one.
							NextInstructionBeyondSubroutineCall =
										ProgramProfile->NextInstruction;

							ProgramProfile->NextInstruction = ((unsigned int) OffsetToSubroutine);

							HWSTRING	OldEditBuffer(ProgramProfile->EditBuffer);
							ProgramProfile->EditBuffer->Copy(ProgramProfile->ReturnWideString);

							ProcessRoutine(ProgramProfile, IPB);

							ProgramProfile->EditBuffer->Copy(&OldEditBuffer);

							ProgramProfile->NextInstruction = ((unsigned int) NextInstructionBeyondSubroutineCall);
						}
					}

				// In case we're running an old script that still uses a byte string, we'll accommodate that.
					else if(InstructionTypeMatch(ProgramProfile, HHPL_Inst_Ret_String))
					{
						if(ExecuteNextInstruction(ProgramProfile, IPB))
						{
						// We have a string to work with; now we need to get the offset to the
						// subroutine and execute it.
							unsigned int	OffsetToSubroutine=0;

							CheckForArguments(ProgramProfile, 4);	// Minimum of 4 bytes following.

							if(!ProgramProfile->UnrecoverableError)
							{
								OffsetToSubroutine =
										((unsigned int) ProgramProfile->Code
												[ProgramProfile->NextInstruction + 3]);
								OffsetToSubroutine = (OffsetToSubroutine * 256) +
										((unsigned int) ProgramProfile->Code
												[ProgramProfile->NextInstruction + 2]);
								OffsetToSubroutine = (OffsetToSubroutine * 256) +
										((unsigned int) ProgramProfile->Code
												[ProgramProfile->NextInstruction + 1]);
								OffsetToSubroutine = (OffsetToSubroutine * 256) +
										((unsigned int) ProgramProfile->Code
												[ProgramProfile->NextInstruction]);
								ProgramProfile->NextInstruction += 4;

							// Check the integrity of the offset.
								if(OffsetToSubroutine >= ProgramProfile->Size)
								{
								// The offset to the subroutine is beyond the end of the HHPL
								// code.  WOOF WOOF WOOF...
									ProgramProfile->UnrecoverableError = true;
									sprintf(TempStr, "Subroutine location is beyond end of format code: IP = %08X",
											ProgramProfile->NextInstruction);
									LogError(TempStr);
								}
							}


						// If the offset is OK, let's save the current instruction pointer,
						// set the instruction pointer to the subroutine, run it, and restore
						// the instruction pointer to the next instruction following this one.
							NextInstructionBeyondSubroutineCall =
										ProgramProfile->NextInstruction;

							ProgramProfile->NextInstruction = ((unsigned int) OffsetToSubroutine);

							HWSTRING	OldEditBuffer(ProgramProfile->EditBuffer);
							ProgramProfile->EditBuffer->ConvertAndCopyFromHString( ProgramProfile->ReturnString );

							ProcessRoutine(ProgramProfile, IPB);

							ProgramProfile->EditBuffer->Copy(&OldEditBuffer);

							ProgramProfile->NextInstruction = ((unsigned int) NextInstructionBeyondSubroutineCall);
						}
					}

					else
					{
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "Illegal ExecuteSubroutine command subsequent instruction: IP = %08X",
								ProgramProfile->NextInstruction);
						LogError(TempStr);
					}
				}
				break;



			case HHPL_Inst_Exit:
		//		Byte 0:		HHPL_Inst_Exit instruction
				++ProgramProfile->NextInstruction;
				ProgramProfile->ExitInstructionEncountered = true;
				break;




			case HHPL_Inst_GoToCommand:
		//		Param1:		Numeric DOWHILE level to jump to
		//		Param2:		Numeric FOREVER level to jump to
		//		Param3:		Numeric IF level to jump to
		//		Param4:		Numeric WHILE level to jump to
		//		Param5:		Address to jump to
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "go to", 5, HHPL_PROC_GET_NUMERIC_PARAMETER,
						HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER,
						HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_CODE_ADDRESS_PARAMETER, 0, 0))
				{
				// We successfully found the jump location; now set up the program profile for the
				// jump and do it.
					ProgramProfile->GoToAddress = ((unsigned int) IPB->CodeAddress1);
					ProgramProfile->GoToEncountered = true;
				}
				break;




			case HHPL_Inst_ForeverDoCommand:
				{
		//		Byte 0:		HHPL_Inst_ForeverDoCommand instruction
		//		Byte 1..2:	Offset to instruction following the Forever (stored little endian)
				unsigned int	OffsetToFirstInstructionOfForeverLoop;
				unsigned int	OffsetToInstructionFollowingForever=0;
				bool			ExitInstructionEncountered;

				++ProgramProfile->NextInstruction;
				++ProgramProfile->ForeverLoopLevel;

			// Get the offsets first.
				CheckForArguments(ProgramProfile, 2);	// Minimum of 2 bytes following.

				if(!ProgramProfile->UnrecoverableError)
				{
					OffsetToInstructionFollowingForever =
							((unsigned int) ProgramProfile->Code
												[ProgramProfile->NextInstruction + 1]) * 256;
					OffsetToInstructionFollowingForever +=
							((unsigned int) ProgramProfile->Code
												[ProgramProfile->NextInstruction]);
					ProgramProfile->NextInstruction += 2;

				// Check the integrity of the offsets.
					if(OffsetToInstructionFollowingForever < ProgramProfile->NextInstruction)
					{
					// According to the offsets, the instruction following the Forever falls in
					// the middle of or before the Forever instruction.
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "Offset to next instruction beyond Forever is inside the Forever: IP = %08X",
								ProgramProfile->NextInstruction);
						LogError(TempStr);
					}
					else if(OffsetToInstructionFollowingForever > ProgramProfile->Size)
					{
					// According to the offsets, the offset to the next instruction following the
					// Forever is beyond the end of the HHPL code.  WOOF WOOF WOOF...
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "Forever extends beyond end of format code: IP = %08X",
								ProgramProfile->NextInstruction);
						LogError(TempStr);
					}
				}


			// Keep executing the loop contents until an Exit instruction is encountered or a
			// fatal error is encountered.
				ExitInstructionEncountered = false;
				OffsetToFirstInstructionOfForeverLoop = ProgramProfile->NextInstruction;
				while((!ExitInstructionEncountered) && (!ProgramProfile->UnrecoverableError) &&
						(!ProgramProfile->ExitInstructionEncountered) &&
								(!TerminateHHPLProgramAtNextInstruction) &&
									(!ProgramProfile->GoToEncountered))
				{
				// Execute all the instructions through the end of the loop or an exit instruction.
					while((!ExitInstructionEncountered) &&
							(!ProgramProfile->UnrecoverableError) &&
							(ProgramProfile->NextInstruction <
														OffsetToInstructionFollowingForever) &&
						(!ProgramProfile->ExitInstructionEncountered) &&
						(!TerminateHHPLProgramAtNextInstruction) &&
									(!ProgramProfile->GoToEncountered))
					{
						if(ProgramProfile->Code[ProgramProfile->NextInstruction] ==
																			HHPL_Inst_Exit)
						{
							ExitInstructionEncountered = true;
						}
						else
						{
							ExecuteNextInstruction(ProgramProfile, IPB);
						}
					}

					ProgramProfile->NextInstruction = ((unsigned int) OffsetToFirstInstructionOfForeverLoop);
				}

			// Move the instruction pointer to the first instruction after the Forever.
				if(ProgramProfile->ExitInstructionEncountered)
				{
					ProgramProfile->ExitInstructionEncountered = false;
				}
				ProgramProfile->NextInstruction = ((unsigned int) OffsetToInstructionFollowingForever);
				--ProgramProfile->ForeverLoopLevel;
				}
				break;




	// HHPL compare commands
			case HHPL_Inst_CompareNumber:
		//		Param1:		Compare number operation sub instruction
		//		Param2:		Command that returns the first numeric operand
		//		Param3:		Command that returns the second numeric operand
				++ProgramProfile->NextInstruction;
				if(GetParameters(ProgramProfile, IPB, "numeric compare operation", 3,
							HHPL_PROC_SUBINSTRUCTION_PARAMETER,
							HHPL_PROC_GET_NUMERIC_PARAMETER, HHPL_PROC_GET_NUMERIC_PARAMETER, 0, 0, 0, 0))
				{
				// If successful, perform the operation specified in the sub-instruction
					switch(IPB->SubinstructionOperand1)
					{
						case HHPL_LogicalSubInst_Op1_EQ_Op2:
							ProgramProfile->ReturnBool =
										(IPB->IntegerOperand1 ==
												IPB->IntegerOperand2) ? true : false;
							break;

						case HHPL_LogicalSubInst_Op1_LE_Op2:
							ProgramProfile->ReturnBool =
										(IPB->IntegerOperand1 <=
												IPB->IntegerOperand2) ? true : false;
							break;

						case HHPL_LogicalSubInst_Op1_GE_Op2:
							ProgramProfile->ReturnBool =
										(IPB->IntegerOperand1 >=
												IPB->IntegerOperand2) ? true : false;
							break;

						case HHPL_LogicalSubInst_Op1_LT_Op2:
							ProgramProfile->ReturnBool =
										(IPB->IntegerOperand1 <
												IPB->IntegerOperand2) ? true : false;
							break;

						case HHPL_LogicalSubInst_Op1_GT_Op2:
							ProgramProfile->ReturnBool =
										(IPB->IntegerOperand1 >
												IPB->IntegerOperand2) ? true : false;
							break;

						case HHPL_LogicalSubInst_Op1_NE_Op2:
							ProgramProfile->ReturnBool =
										(IPB->IntegerOperand1 !=
												IPB->IntegerOperand2) ? true : false;
							break;

						default:
							ProgramProfile->UnrecoverableError = true;
							sprintf(TempStr, "Unrecognized numeric comparison operation sub-instruction: IP = %08X",
								ProgramProfile->NextInstruction);
							LogError(TempStr);
							break;
					};
				}
				break;



		// Constants and NULLs
			case HHPL_Inst_NullInteger:
		//		Byte 0:		HHPL_Inst_NullInteger instruction
				++ProgramProfile->NextInstruction;
				ProgramProfile->ReturnNumber = 0;
				break;


			case HHPL_Inst_NullString:
		//		Byte 0:		HHPL_Inst_NullString instruction
				++ProgramProfile->NextInstruction;
				ProgramProfile->ReturnString->Size = 0;
				break;


			case HHPL_Inst_NullWideString:
		//		Byte 0:		HHPL_Inst_NullWideString instruction
				++ProgramProfile->NextInstruction;
				ProgramProfile->ReturnWideString->Size = 0;
				break;


			case HHPL_Inst_NullPrefix:
		//		Byte 0:		HHPL_Inst_NullPrefix instruction
				++ProgramProfile->NextInstruction;
				ProgramProfile->ReturnPrefix.String->Size = 0;
				ProgramProfile->ReturnPrefix.UseIfNoData = false;
				ProgramProfile->ReturnPrefix.UseIfData = false;
				break;


			case HHPL_Inst_NullSuffix:
		//		Byte 0:		HHPL_Inst_NullSuffix instruction
				++ProgramProfile->NextInstruction;
				ProgramProfile->ReturnSuffix.String->Size = 0;
				ProgramProfile->ReturnSuffix.UseIfNoData = false;
				ProgramProfile->ReturnSuffix.UseIfData = false;
				break;


		// Data formatting send (report) character commands
		//		Byte 0:		HHPL_Inst_SendFromCursorToEndWithCharXXAtEnd instruction
		//		Byte 1:		XX - special character to send at end
			case HHPL_Inst_SendFromCursorToEndWithCharXXAtEnd:
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 1);
				if(!ProgramProfile->UnrecoverableError)
				{
					SendWithTranslation
						(&ProgramProfile->EditBuffer->WChar[ProgramProfile->Cursor],
						ProgramProfile->EditBuffer->Size - ProgramProfile->Cursor,
						ProgramProfile->Code[ProgramProfile->NextInstruction],
						ProgramProfile);

					ProgramProfile->Cursor = ProgramProfile->EditBuffer->Size;
					++ProgramProfile->NextInstruction;
				}
				break;


			case HHPL_Inst_SendNNCharsFromCursorWithCharXXAtEnd:
		//		Byte 0:		HHPL_Inst_SendNNCharsFromCursorWithCharXXAtEnd instruction
		//		Byte 1:		NN - number of characters to send from input data starting at cursor
		//		Byte 2:		XX - special character to send at end
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 2);
				if(!ProgramProfile->UnrecoverableError)
				{
					CharacterCount = ProgramProfile->Code[ProgramProfile->NextInstruction];
					CharacterCount = (CharacterCount >> 4) * 10 + (CharacterCount & 0x0F);
					++ProgramProfile->NextInstruction;

				// Number must be less than 100 and less than the number of input characters
				// between the current cursor position and the end of string.
					ProgramProfile->ReturnBool = true;		// So far, no errors.
					if(CharacterCount > 99)
					{
						sprintf(TempStr, "SendNNCharsFromCursorWithCharXXAtEnd: Character count %d out of range: IP = %08X",
							CharacterCount, ProgramProfile->NextInstruction);
						CorruptDataFormatCharacterCount(TempStr);
					}

					else if
						(CharacterCount > (ProgramProfile->EditBuffer->Size - ProgramProfile->Cursor))
					{
					// If we're at nesting level 1, WOOF.  Otherwise, we'll treat this as a soft error
					// and return a boolean value of false.
						if(ProgramProfile->NestingLevel == 1)
						{
							sprintf(TempStr, "SendNNCharsFromCursorWithCharXXAtEnd: Not enough characters to copy: IP = %08X",
								ProgramProfile->NextInstruction);
							DataFormatCursorOutOfRange(TempStr);
						}
						else
						{
							ProgramProfile->ReturnBool = false;
						}
					}

					else
					{
						SendWithTranslation
							(&ProgramProfile->EditBuffer->WChar[ProgramProfile->Cursor],
							CharacterCount,
							ProgramProfile->Code[ProgramProfile->NextInstruction],
							ProgramProfile);

						ProgramProfile->Cursor += CharacterCount;
						++ProgramProfile->NextInstruction;
					}
				}
				break;


			case HHPL_Inst_SendFromCursorToCharSSWithCharXXAtEnd:
		//		Byte 0:		HHPL_Inst_SendFromCursorToCharSSWithCharXXAtEnd instruction
		//		Byte 1:		SS - send from input data starting at cursor to char SS
		//		Byte 2:		XX - special character to send at end
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 2);
				if(!ProgramProfile->UnrecoverableError)
				{
					if(ProgramProfile->EditBuffer->FindCharacterForward(
							ProgramProfile->Code[ProgramProfile->NextInstruction],
							ProgramProfile->Cursor,
							&Position))
					{
						ProgramProfile->ReturnBool = true;

						SendWithTranslation
							(&ProgramProfile->EditBuffer->WChar[ProgramProfile->Cursor],
							Position - ProgramProfile->Cursor,
							ProgramProfile->Code[ProgramProfile->NextInstruction+1],
							ProgramProfile);

						ChangeCursor(ProgramProfile, Position);
					}
					else
					{
						ProgramProfile->ReturnBool = false;

						if(ProgramProfile->NestingLevel == 1)
						{
							sprintf(TempStr, "SendFromCursorToCharSSWithCharXXAtEnd: Failed to find character in input: IP = %08X",
								ProgramProfile->NextInstruction);
							DataFormatFindFailed(TempStr);
						}
					}

					ProgramProfile->NextInstruction += 2;
				}
				break;


			case HHPL_Inst_SendXXCharNNTimes:
		//		Byte 0:		HHPL_Inst_SendXXCharNNTimes instruction
		//		Byte 1:		XX - character to send multiple times, as specified by the next byte
		//		Byte 2:		NN - number of times to send char XX
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 2);
				if(!ProgramProfile->UnrecoverableError)
				{
					Character = (wchar_t) ProgramProfile->Code[ProgramProfile->NextInstruction];
					++ProgramProfile->NextInstruction;
					CharacterCount = ProgramProfile->Code[ProgramProfile->NextInstruction];
					CharacterCount = (CharacterCount >> 4) * 10 + (CharacterCount & 0x0F);
					++ProgramProfile->NextInstruction;

					SendToHostParameters.LengthOfString = 1;
					SendToHostParameters.DataSourceInformation = DEFAULT_DATA_SOURCE_ID;
					SendToHostParameters.ControlMask = 0;
					SendToHostParameters.DelayInMicroseconds = 0; 
					if(CharacterCount < 99)
					{
						for(i = 0;    i < ((int) CharacterCount);    ++i)
						{
							SendToHostParameters.String = &Character;
							SendToHost( &SendToHostParameters );
						}
					}
					else
					{
						sprintf(TempStr, "SendXXCharNNTimes: Character count %d out of range: IP = %08X",
							CharacterCount, ProgramProfile->NextInstruction);
						CorruptDataFormatCharacterCount(TempStr);
					}
				}
				break;


			case HHPL_Inst_SendAllButTheLastNNChars:
		//		Byte 0:		HHPL_Inst_SendAllButTheLastNNChars instruction
		//		Byte 1:		NN - send from input data starting at cursor to NN characters from
		//						the end
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 1);
				if(!ProgramProfile->UnrecoverableError)
				{
					CharacterCount = ProgramProfile->Code[ProgramProfile->NextInstruction];
					CharacterCount = (CharacterCount >> 4) * 10 + (CharacterCount & 0x0F);
					++ProgramProfile->NextInstruction;

				// Number must be less than 100 and less than the number of input characters
				// between the current cursor position and the end of string minus the character
				// count.
					ProgramProfile->ReturnBool = true;		// So far, no errors.
					if(CharacterCount > 99)
					{
						sprintf(TempStr, "SendAllButTheLastNNChars: Character count %d out of range: IP = %08X",
							CharacterCount, ProgramProfile->NextInstruction);
						CorruptDataFormatCharacterCount(TempStr);
					}

					else if
						(CharacterCount > (ProgramProfile->EditBuffer->Size - ProgramProfile->Cursor))
					{
					// If we're at nesting level 1, WOOF.  Otherwise, we'll treat this as a soft error
					// and return a boolean value of false.
						if(ProgramProfile->NestingLevel == 1)
						{
							sprintf(TempStr, "SendAllButTheLastNNChars: Not enough characters to copy: IP = %08X",
								ProgramProfile->NextInstruction);
							DataFormatCursorOutOfRange(TempStr);
						}
						else
						{
							ProgramProfile->ReturnBool = false;
						}
					}

					else
					{
						SendWithTranslation
							(&ProgramProfile->EditBuffer->WChar[ProgramProfile->Cursor],
							(ProgramProfile->EditBuffer->Size - ProgramProfile->Cursor - CharacterCount),
							0,
							ProgramProfile);

						ProgramProfile->Cursor = ProgramProfile->EditBuffer->Size - CharacterCount;
							++ProgramProfile->NextInstruction;
					}
				}
				break;


	// Data formatting cursor movement commands
			case HHPL_Inst_MoveCursorAheadNN:
		//		Byte 0:		HHPL_Inst_MoveCursorAheadNN instruction
		//		Byte 1:		NN - number of character positions to move cursor ahead
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 1);
				if(!ProgramProfile->UnrecoverableError)
				{
					Position = ProgramProfile->Code[ProgramProfile->NextInstruction];
					Position = (Position >> 4) * 10 + (Position & 0x0F);
					++ProgramProfile->NextInstruction;

				// Position increment must be < 100
					if(Position > 99)
					{
						sprintf(TempStr, "MoveCursorAheadNN: Cursor increment out of range: IP = %08X",
								ProgramProfile->NextInstruction);
						DataFormatCursorOutOfRange(TempStr);
					}

					else
					{
						ChangeCursor(ProgramProfile, ((int) (ProgramProfile->Cursor + Position)));
						ProgramProfile->ReturnNumber = ProgramProfile->Cursor;
					}
				}
				break;

			case HHPL_Inst_MoveCursorBackwardNN:
		//		Byte 0:		HHPL_Inst_MoveCursorBackwardNN instruction
		//		Byte 1:		NN - number of character positions to move cursor back
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 1);
				if(!ProgramProfile->UnrecoverableError)
				{
					Position = ProgramProfile->Code[ProgramProfile->NextInstruction];
					Position = (Position >> 4) * 10 + (Position & 0x0F);
					++ProgramProfile->NextInstruction;

				// Position increment must be < 100
					if(Position > 99)
					{
						sprintf(TempStr, "MoveCursorBackwardNN: Cursor decrement out of range: IP = %08X",
								ProgramProfile->NextInstruction);
						DataFormatCursorOutOfRange(TempStr);
					}

					else
					{
						ChangeCursor(ProgramProfile, ((int) (ProgramProfile->Cursor - Position)));
						ProgramProfile->ReturnNumber = ProgramProfile->Cursor;
					}
				}
				break;

			case HHPL_Inst_MoveCursorToBeginning:
		//		Byte 0:		HHPL_Inst_MoveCursorToBeginning instruction
				++ProgramProfile->NextInstruction;
				ProgramProfile->Cursor = 0;
				ProgramProfile->ReturnNumber = ProgramProfile->Cursor;
				break;

			case HHPL_Inst_MoveCursorToEnd:
		//		Byte 0:		HHPL_Inst_MoveCursorToEnd instruction
				++ProgramProfile->NextInstruction;
				ProgramProfile->Cursor = ProgramProfile->EditBuffer->Size - 1;
				ProgramProfile->ReturnNumber = ProgramProfile->Cursor;
				break;

			case HHPL_Inst_MoveCursorAheadToCharXX:
		//		Byte 0:		HHPL_Inst_MoveCursorAheadToCharXX instruction
		//		Byte 1:		XX - character to search for in the forward direction, leaving
		//						cursor at character XX
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 1);
				if(!ProgramProfile->UnrecoverableError)
				{
					if(ProgramProfile->EditBuffer->FindCharacterForward(
							ProgramProfile->Code[ProgramProfile->NextInstruction],
							ProgramProfile->Cursor,
							&Position))
					{
						ProgramProfile->ReturnBool = true;

						ChangeCursor(ProgramProfile, Position);
					}
					else
					{
						ProgramProfile->ReturnBool = false;

						if(ProgramProfile->NestingLevel == 1)
						{
							sprintf(TempStr, "MoveCursorAheadToCharXX: Failed to find character in input: IP = %08X",
									ProgramProfile->NextInstruction);
							DataFormatFindFailed(TempStr);
						}
					}

					ProgramProfile->NextInstruction += 2;
				}
				break;

			case HHPL_Inst_MoveCursorBackwardToCharXX:
		//		Byte 0:		HHPL_Inst_MoveCursorBackwardToCharXX instruction
		//		Byte 1:		XX - character to search for in the backward direction, leaving
		//						cursor at character XX
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 1);
				if(!ProgramProfile->UnrecoverableError)
				{
					if(ProgramProfile->EditBuffer->FindCharacterBackward(
							ProgramProfile->Code[ProgramProfile->NextInstruction],
							ProgramProfile->Cursor,
							&Position))
					{
						ProgramProfile->ReturnBool = true;

						ChangeCursor(ProgramProfile, Position);
					}
					else
					{
						ProgramProfile->ReturnBool = false;

						if(ProgramProfile->NestingLevel == 1)
						{
							sprintf(TempStr, "MoveCursorBackwardToCharXX: Failed to find character in input: IP = %08X",
									ProgramProfile->NextInstruction);
							DataFormatFindFailed(TempStr);
						}
					}

					ProgramProfile->NextInstruction += 2;
				}
				break;

			case HHPL_Inst_MoveCursorAheadToFirstNonXXChar:
		//		Byte 0:		HHPL_Inst_MoveCursorAheadToFirstNonXXChar instruction
		//		Byte 1:		XX - keep moving the cursor ahead until a character not equal to
		//						XX is found
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 1);
				if(!ProgramProfile->UnrecoverableError)
				{
					if(ProgramProfile->EditBuffer->FindNonCharacterForward(
							ProgramProfile->Code[ProgramProfile->NextInstruction],
							ProgramProfile->Cursor,
							&Position))
					{
						ProgramProfile->ReturnBool = true;

						ChangeCursor(ProgramProfile, Position);
					}
					else
					{
						ProgramProfile->ReturnBool = false;

						if(ProgramProfile->NestingLevel == 1)
						{
							sprintf(TempStr, "MoveCursorAheadToFirstNonXXChar: Failed to find character in input: IP = %08X",
									ProgramProfile->NextInstruction);
							DataFormatFindFailed(TempStr);
						}
					}

					ProgramProfile->NextInstruction += 2;
				}
				break;

			case HHPL_Inst_MoveCursorBackwardToFirstNonXXChar:
		//		Byte 0:		HHPL_Inst_MoveCursorAheadToFirstNonXXChar instruction
		//		Byte 1:		XX - keep moving the cursor back until a character not equal to
		//						XX is found
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 1);
				if(!ProgramProfile->UnrecoverableError)
				{
					if(ProgramProfile->EditBuffer->FindNonCharacterBackward(
							ProgramProfile->Code[ProgramProfile->NextInstruction],
							ProgramProfile->Cursor,
							&Position))
					{
						ProgramProfile->ReturnBool = true;

						ChangeCursor(ProgramProfile, Position);
					}
					else
					{
						ProgramProfile->ReturnBool = false;

						if(ProgramProfile->NestingLevel == 1)
						{
							sprintf(TempStr, "MoveCursorBackwardToFirstNonXXChar: Failed to find character in input: IP = %08X",
									ProgramProfile->NextInstruction);
							DataFormatFindFailed(TempStr);
						}
					}

					ProgramProfile->NextInstruction += 2;
				}
				break;



	// Data formatting character suppression / replacement commands
			case HHPL_Inst_SuppressTheNNCharsXX1ToXXNN:
		//		Byte 0:		HHPL_Inst_SuppressTheNNCharsXX1ToXXNN instruction
		//		Byte 1:		NN - number of characters following this byte that will be
		//						suppressed.
		//		Byte 2 to 2+NN-1:	NN characters that will be suppressed.
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 1);
				if(!ProgramProfile->UnrecoverableError)
				{
					ProgramProfile->NumberOfSuppressionCharacters =
									ProgramProfile->Code[ProgramProfile->NextInstruction];
					ProgramProfile->NumberOfSuppressionCharacters =
							(ProgramProfile->NumberOfSuppressionCharacters >> 4) * 10 +
										(ProgramProfile->NumberOfSuppressionCharacters & 0x0F);
					++ProgramProfile->NextInstruction;


				// Number of suppression characters must be in range.
					if(ProgramProfile->NumberOfSuppressionCharacters >
												MAXIMUM_NUMBER_OF_SUPPRESSION_CHARACTERS)
					{
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "SuppressTheNNCharsXX1ToXXNN: Too many suppression characters: IP = %08X",
								ProgramProfile->NextInstruction);
						LogError(TempStr);
					}

					else
					{
						CheckForArguments(ProgramProfile,
											ProgramProfile->NumberOfSuppressionCharacters);

						if(!ProgramProfile->UnrecoverableError)
						{
							memcpy(ProgramProfile->SuppressionCharacters,
									&ProgramProfile->Code[ProgramProfile->NextInstruction],
									ProgramProfile->NumberOfSuppressionCharacters);
							ProgramProfile->NextInstruction +=
											ProgramProfile->NumberOfSuppressionCharacters;
						}
					}
				}
				break;

			case HHPL_Inst_TurnOffCharSuppression:
		//		Byte 0:		HHPL_Inst_TurnOffCharSuppression instruction
				++ProgramProfile->NextInstruction;
				ProgramProfile->NumberOfSuppressionCharacters = 0;
				break;

			case HHPL_Inst_ReplaceTheNNCharsXX1ToXXNN:
		//		Byte 0:		HHPL_Inst_ReplaceTheNNCharsXX1ToXXNN instruction
		//		Byte 1:		NN - number of characters following this byte that will be
		//						used as replacement characters.
		//		Byte 2 to 2+NN-1:	NN characters that will be used as replacement characters.
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 1);
				if(!ProgramProfile->UnrecoverableError)
				{
					ProgramProfile->NumberOfReplacementCharacters =
									ProgramProfile->Code[ProgramProfile->NextInstruction];
					ProgramProfile->NumberOfReplacementCharacters =
							(ProgramProfile->NumberOfReplacementCharacters >> 4) * 10 +
										(ProgramProfile->NumberOfReplacementCharacters & 0x0F);
					++ProgramProfile->NextInstruction;


				// Number of replacement characters must be in range.
					if(ProgramProfile->NumberOfReplacementCharacters >
													MAXIMUM_NUMBER_OF_REPLACEMENT_CHARACTERS)
					{
						ProgramProfile->UnrecoverableError = true;
						sprintf(TempStr, "ReplaceTheNNCharsXX1ToXXNN: Too many replacement characters: IP = %08X",
								ProgramProfile->NextInstruction);
						LogError(TempStr);
					}

					else
					{
						CheckForArguments(ProgramProfile,
											ProgramProfile->NumberOfReplacementCharacters * 2);
						if(!ProgramProfile->UnrecoverableError)
						{
							memcpy(ProgramProfile->ReplacementCharacters,
									&ProgramProfile->Code[ProgramProfile->NextInstruction],
									ProgramProfile->NumberOfReplacementCharacters * 2);
							ProgramProfile->NextInstruction +=
											ProgramProfile->NumberOfReplacementCharacters * 2;
						}
					}
				}
				break;

			case HHPL_Inst_TurnOffCharReplacement:
		//		Byte 0:		HHPL_Inst_TurnOffCharReplacement instruction
				++ProgramProfile->NextInstruction;
				ProgramProfile->NumberOfReplacementCharacters = 0;
				break;


	// Data formatting character comparison commands
			case HHPL_Inst_CompareCharXXToCharAtCursor:
		//		Byte 0:		HHPL_Inst_CompareCharXXToCharAtCursor instruction
		//		Byte 1:		XX - character to compare with character in input data at cursor
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 1);
				if(!ProgramProfile->UnrecoverableError)
				{
					if(ProgramProfile->Code[ProgramProfile->NextInstruction] ==
							(unsigned char) ProgramProfile->EditBuffer->WChar[ProgramProfile->Cursor])
					{
						++ProgramProfile->Cursor;
						ProgramProfile->ReturnBool = true;
					}
					else
					{
						ProgramProfile->ReturnBool = false;

						if(ProgramProfile->NestingLevel == 1)
						{
							sprintf(TempStr, "CompareCharXXToCharAtCursor failed: IP = %08X",
									ProgramProfile->NextInstruction);
							DataFormatCompareFailed(TempStr);
						}
					}
					++ProgramProfile->NextInstruction;
				}
				break;

			case HHPL_Inst_CheckToSeeThatCharAtCursorIsNumeric:
		//		Byte 0:		HHPL_Inst_CheckToSeeThatCharAtCursorIsNumeric instruction
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 1);
				if(!ProgramProfile->UnrecoverableError)
				{
					if(isdigit(ProgramProfile->Code[ProgramProfile->NextInstruction]))
					{
						ProgramProfile->ReturnBool = true;
					}
					else
					{
						ProgramProfile->ReturnBool = false;

						if(ProgramProfile->NestingLevel == 1)
						{
							sprintf(TempStr, "CheckToSeeThatCharAtCursorIsNumeric failed: IP = %08X",
									ProgramProfile->NextInstruction);
							DataFormatCompareFailed(TempStr);
						}
					}
					++ProgramProfile->NextInstruction;
				}
				break;

			case HHPL_Inst_CheckToSeeThatCharAtCursorIsNonNumeric:
		//		Byte 0:		HHPL_Inst_CheckToSeeThatCharAtCursorIsNonNumeric instruction
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 1);
				if(!ProgramProfile->UnrecoverableError)
				{
					if(!isdigit(ProgramProfile->Code[ProgramProfile->NextInstruction]))
					{
						ProgramProfile->ReturnBool = true;
					}
					else
					{
						ProgramProfile->ReturnBool = false;

						if(ProgramProfile->NestingLevel == 1)
						{
							sprintf(TempStr, "CheckToSeeThatCharAtCursorIsNonNumeric failed: IP = %08X",
									ProgramProfile->NextInstruction);
							DataFormatCompareFailed(TempStr);
						}
					}
					++ProgramProfile->NextInstruction;
				}
				break;


		// Data formatting toggle auxiliary port on / off
			case HHPL_Inst_ToggleAuxiliaryPortOnAndOff:
		//		Byte 0:		HHPL_Inst_ToggleAuxiliaryPortOnAndOff instruction
				ToggleAuxiliaryPort();
				++ProgramProfile->NextInstruction;
				break;


		// Data formatting delay command
			case HHPL_Inst_DataFormatDelay:
		//		Byte 0:		HHPL_Inst_DataFormatDelay instruction
		//		Byte 1 to 4:	NNNN - Four ASCII numeric digits specifying delay time in
		//							5 ms intervals;  ex. - "0015" specifies 75 ms delay
				++ProgramProfile->NextInstruction;
				CheckForArguments(ProgramProfile, 4);
				if(!ProgramProfile->UnrecoverableError)
				{
					unsigned int	FirstByte;
					unsigned int	SecondByte;
					unsigned int	DelayCount;

					FirstByte = ProgramProfile->Code[ProgramProfile->NextInstruction];
					FirstByte = (FirstByte >> 4) * 10 + (FirstByte & 0x0F);
					++ProgramProfile->NextInstruction;
					SecondByte = ProgramProfile->Code[ProgramProfile->NextInstruction];
					SecondByte = (SecondByte >> 4) * 10 + (SecondByte & 0x0F);
					++ProgramProfile->NextInstruction;
					DelayCount = FirstByte * 100 + SecondByte;


				// Delay count must be less than 10000
					if(DelayCount > 9999)
					{
						sprintf(TempStr, "DataFormatDelay: Delay too large: IP = %08X",
								ProgramProfile->NextInstruction);
						DataFormatDelayError(TempStr);
					}

					else
					{
						DataFormatDelay(((int) DelayCount) * 5);	// Multiply by 5 millisecods
					}
				}
				break;


		// We don't recognize the instruction; WOOF WOOF WOOF WOOF WOOF WOOF...
			default:
				ProgramProfile->UnrecoverableError = true;
				sprintf(TempStr, "Illegal instruction encountered: IP = %08X",
						ProgramProfile->NextInstruction);
				LogError(TempStr);
				break;
		};


	// Allow processing of background operations in between HHPL image execution
		ProcessBackgroundOperationsDuringScriptExecution();


	// If we were supposed to back out to some level, let's check to see if we've done that.
		if(ProgramProfile->GoToEncountered)
		{
			if((ProgramProfile->DoWhileLoopLevel == 0) &&
					(ProgramProfile->ForeverLoopLevel == 0) &&
					(ProgramProfile->IfThenElseLevel == 0) &&
					(ProgramProfile->WhileLoopLevel == 0))
			{
				ProgramProfile->GoToEncountered = false;
				ProgramProfile->NextInstruction = ProgramProfile->GoToAddress;
			}
		}
	}



// Decrement the nesting level, now that we're done executing.
	ProgramProfile->NestingLevel--;


// If we didn't encounter an Exit instruction and there haven't been  any unrecoverable errors,
// return true to indicate that we successfully executed an instruction and that instruction
// execution may continue.
	return((!ProgramProfile->ExitInstructionEncountered) &&
									(!ProgramProfile->UnrecoverableError) &&
									(!TerminateHHPLProgramAtNextInstruction) &&
									(!ProgramProfile->GoToEncountered));
}







//
//	ProcessRoutine
//		This routine will attempt to process all instructions that are part of an HHPL
//		routine.  We'll start at the current instruction pointer (we assume that the
//		program has been properly set up) and we'll quit when the next instruction moves
//		past the end of the HHPL program, we encounter an exit instruction, or an error
//		occurs.
//
void HHPL_PROCESSOR::ProcessRoutine(HHPL_PROGRAM_PROFILE *ProgramProfile,
									HHPL_INSTRUCTION_PARAMETER_BLOCK *IPB)
{
// Backup the context tracking information.
	unsigned int	BackupOfIfThenElseLevel = ProgramProfile->IfThenElseLevel;
	unsigned int	BackupOfWhileLoopLevel = ProgramProfile->WhileLoopLevel;
	unsigned int	BackupOfDoWhileLoopLevel = ProgramProfile->DoWhileLoopLevel;
	unsigned int	BackupOfForeverLoopLevel = ProgramProfile->ForeverLoopLevel;
	bool			BackupOfGoToEncountered = ProgramProfile->GoToEncountered;



// Haven't encountered an exit instruction yet.
	ProgramProfile->ExitInstructionEncountered = false;

	ProgramProfile->DoWhileLoopLevel = 0;
	ProgramProfile->ForeverLoopLevel = 0;
	ProgramProfile->IfThenElseLevel = 0;
	ProgramProfile->WhileLoopLevel = 0;
	ProgramProfile->GoToEncountered = false;


// Keep going so long as the instruction pointer hasn't moved past the end of the code and
// the next isntruction execution status is OK (if an error occurs, or an exit instruction is
// encountered, the execution status will not be OK).
	while((ProgramProfile->NextInstruction < ProgramProfile->Size) &&
								(ExecuteNextInstruction(ProgramProfile, IPB)))
	{
	}


// Restore the context tracking information.
	ProgramProfile->IfThenElseLevel = ((unsigned int) BackupOfIfThenElseLevel);
	ProgramProfile->WhileLoopLevel = ((unsigned int) BackupOfWhileLoopLevel);
	ProgramProfile->DoWhileLoopLevel = ((unsigned int) BackupOfDoWhileLoopLevel);
	ProgramProfile->ForeverLoopLevel = ((unsigned int) BackupOfForeverLoopLevel);
	ProgramProfile->GoToEncountered = BackupOfGoToEncountered;



// Clear this before returning in case this routine has been called recursively (if it has,
// we don't want the calling routine to quit when it sees this flag.
	ProgramProfile->ExitInstructionEncountered = false;

	return;
}








//
//	ProcessHHPLProgram
//		This routine will attempt to process all routines that are part of an HHPL
//		program.  We'll setup some parameters for the input HHPL program and
//		start executing at the first instruction.
//
void HHPL_PROCESSOR::ProcessHHPLProgram( const unsigned char *InputCode, unsigned int InputCodeSize,
		const wchar_t *InputData, unsigned int InputDataSize, const wchar_t *InputDataSourceID,
		unsigned int InputDataSourceIDSize, int InputDataControlFlags )
{
	HHPLProgramTerminatedBecauseScriptingIsNotAvailable = false;

	NumberOfHostAccessibleRoutines = 0;				// None by default

	HHPLInterpreterState = HHPL_INTERPRETER_BUSY;

	SkipOverInstructionsWithoutExecutingThem = false;	// Execute all instructions (we're not
														//	starting out in an error state).


// Initialize the HHPL program profile
	HHPL_PROGRAM_PROFILE				ProgramProfile(InputCode, InputCodeSize, InputData, InputDataSize,
					InputDataSourceID, InputDataSourceIDSize, MAXIMUM_SIZE_OF_PREFIX_SUFFIX, MAXIMUM_STRING_SIZE,
					InputDataControlFlags );
	HHPL_INSTRUCTION_PARAMETER_BLOCK	IPB;


// Make sure that either scripting is available OR the feature override is on.
	if(( IsScriptingAvailable() ) || ( ProgramProfile.ParsedHHPLCodeImage->FeatureProtectionOverride ))
	{
	// Haven't been told to terminate yet.
		TerminateHHPLProgramAtNextInstruction = false;

	// Run the main routine
		ProcessRoutine(&ProgramProfile, &IPB);

	}
	else
	{
		HHPLProgramTerminatedBecauseScriptingIsNotAvailable = true;
	}

	HHPLInterpreterState = HHPL_INTERPRETER_IDLE;

	return;
}








//
//	ProcessHHPLProgram (using char instead of wchar_t input)
//		This routine will attempt to process all routines that are part of an HHPL
//		program.  We'll setup some parameters for the input HHPL program and
//		start executing at the first instruction.
//
void HHPL_PROCESSOR::ProcessHHPLProgram(const unsigned char *InputCode, unsigned int InputCodeSize,
		const unsigned char *InputData, unsigned int InputDataSize, const unsigned char *InputDataSourceID,
		unsigned int InputDataSourceIDSize, int InputDataControlFlags )
{
	HSTRING		hstrInputData( (unsigned char *) InputData, InputDataSize );
	HSTRING		hstrInputDataSourceID( (unsigned char *) InputDataSourceID, InputDataSourceIDSize );
	HWSTRING	hwstrInputData( InputDataSize, true );
	HWSTRING	hwstrInputDataSourceID( InputDataSourceIDSize, true );
	hwstrInputData.ConvertAndCopyFromHString( &hstrInputData );
	hwstrInputDataSourceID.ConvertAndCopyFromHString( &hstrInputDataSourceID );

	ProcessHHPLProgram( InputCode, InputCodeSize, hwstrInputData.WChar, hwstrInputData.Size, hwstrInputDataSourceID.WChar,
						hwstrInputDataSourceID.Size, InputDataControlFlags );
	return;
}



