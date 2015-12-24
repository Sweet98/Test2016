//======================================================================================
// ScriptingMenuSettings.cpp
//======================================================================================
// $RCSfile: ScriptingMenuSettings.cpp $
// $Revision: 1.4 $
// $Date: 2009/08/12 10:44:56EDT $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Module Description:
//      This file contains a routine that creates scripting menu settings as part
//		of the overall reader configuration for a Matrix device.
//
//---------------------------------------------------------------------------

#include	<stdio.h>

#include "stdInclude.h"
#include	"ReaderConfiguration.h"
#include	"ScriptingMenuSettings.h"			// ScriptingMenuSettings interface definition


#if !defined(RFBASE)
#include	"MainHHPLProc.h"
#endif




//
//	ValidateMainScriptName
//		Check to see whether the script name that the configuration system is trying to set as main script default actually exists.
//
bool ValidateMainScriptName(void)
{
#ifdef FIXME
// We will allow a NULL string, which turns off main scripting.
	if( ReaderConfiguration->MenuCommandResponse->Size == 0 )
	{
		return true;
	}
	
// Get the index of the file in the flash disk.
	else
	{
		int		ScriptFileIndex = FlashDisk->Find((char *) ReaderConfiguration->MenuCommandResponse->Char);

	// If the file is in the flash disk and it is a main script, then we're OK.
		if((ScriptFileIndex >= 0) && (FlashDisk->MatrixFlashDiskEntries[ScriptFileIndex].Type == MATRIX_FLASH_DISK::FD_MAIN_SCRIPT))
		{
			return(true);
		}
		else
		{
			return(false);
		}
	}
#else
	return true;
#endif
} // ValidateMainScriptName





//
//	ValidateIQEditProScriptName
//		Check to see whether the script name that the configuration system is trying to set as edit script default actually exists.
//
bool ValidateIQEditProScriptName(void)
{
// We will allow a NULL string, which turns off edit scripting.
	if( ReaderConfiguration->MenuCommandResponse->Size == 0 )
	{
		return true;
	}
#ifdef FIXME	
// Get the index of the file in the flash disk.
	else
	{
		int		ScriptFileIndex = FlashDisk->Find((char *) ReaderConfiguration->MenuCommandResponse->Char);

	// If the file is in the flash disk and it is an edit script, then we're OK.
		if((ScriptFileIndex >= 0) && (FlashDisk->MatrixFlashDiskEntries[ScriptFileIndex].Type == MATRIX_FLASH_DISK::FD_EDIT_SCRIPT))
		{
			return(true);
		}
		else
		{
			return(false);
		}
	}
#else
	return true;
#endif
} // ValidateIQEditProScriptName





//
//	DeleteHHPLMainScript
//		Check to see whether the script name that the configuration system is trying to delete is a main script.  If it is, delete it.
//
bool DeleteHHPLMainScript(void)
{
#ifdef FIXME
// Get the index of the file in the flash disk.
	int		ScriptFileIndex = FlashDisk->Find((char *) ReaderConfiguration->ExtraneousTextAfterCommand->Char);

// If the file is in the flash disk and it is a main script, then delete it.
	if((ScriptFileIndex >= 0) && (FlashDisk->MatrixFlashDiskEntries[ScriptFileIndex].Type == MATRIX_FLASH_DISK::FD_MAIN_SCRIPT))
	{
		FlashDisk->RemoveFile((char *) ReaderConfiguration->ExtraneousTextAfterCommand->Char);
		return(true);
	}
	else
	{
		return(false);
	}
#else
	return false;
#endif
} // DeleteHHPLMainScript





//
//	DeleteHHPLEditScript
//		Check to see whether the script name that the configuration system is trying to delete is an edit script.  If it is, delete it.
//
bool DeleteHHPLEditScript(void)
{
#ifdef FIXME
// Get the index of the file in the flash disk.
	int		ScriptFileIndex = FlashDisk->Find((char *) ReaderConfiguration->ExtraneousTextAfterCommand->Char);

// If the file is in the flash disk and it is a main script, then delete it.
	if((ScriptFileIndex >= 0) && (FlashDisk->MatrixFlashDiskEntries[ScriptFileIndex].Type == MATRIX_FLASH_DISK::FD_EDIT_SCRIPT))
	{
		FlashDisk->RemoveFile((char *) ReaderConfiguration->ExtraneousTextAfterCommand->Char);
		return(true);
	}
	else
	{
		return(false);
	}
#else
		return(true);
#endif
} // DeleteHHPLEditScript





//
//	TerminateHHPLMainScript
//		Terminate the main HHPL script.
//
bool TerminateHHPLMainScript(void)
{
	bool bIsOk;
#if defined(RFBASE) || defined(CF2D)
#else
	theScriptInterpreter.TerminateHHPLProgramAtNextInstruction = true;
	bIsOk = true;
#endif

	return bIsOk;
} // TerminateHHPLMainScript





//
//	TerminateHHPLEditScript
//		Terminate the HHPL edit script.
//
bool TerminateHHPLEditScript(void)
{
//	theScriptInterpreter.TerminateHHPLProgramAtNextInstruction = true;

	return(false);
} // TerminateHHPLEditScript





//
//	HHPLMainScriptDirectory
//		Display a formatted directory listing of main script files.
//
bool HHPLMainScriptDirectory(void)
{
#ifdef FIXME
	char	Temp[160];
	int		i;


	ReaderConfiguration->MenuCommandResponse->Append("\r\n              Reserved\r\n    Location    Size      Size      CRC     Name\r\n");
	for(i = 0;    i < FlashDisk->NumberOfMatrixFlashDiskEntries;    ++i)
	{
		if(FlashDisk->MatrixFlashDiskEntries[i].Type == MATRIX_FLASH_DISK::FD_MAIN_SCRIPT)
		{
			sprintf(Temp, "    %08X  %08X  %08X  %08X  %s\r\n",
						((UINT) FlashDisk->MatrixFlashDiskEntries[i].Location),
						((UINT) FlashDisk->MatrixFlashDiskEntries[i].ReservedSize),
						((UINT) FlashDisk->MatrixFlashDiskEntries[i].Size),
						((UINT) FlashDisk->MatrixFlashDiskEntries[i].CRC),
						FlashDisk->MatrixFlashDiskEntries[i].Name);
			ReaderConfiguration->MenuCommandResponse->Append(Temp);
		}
	}

	ReaderConfiguration->MenuCommandResponse->Append("\r\n");
#endif
	return(true);
} // HHPLMainScriptDirectory





//
//	HHPLEditScriptDirectory
//		Display a formatted directory listing of edit script files.
//
bool HHPLEditScriptDirectory(void)
{
#ifdef FIXME
	char	Temp[160];
	int		i;


	ReaderConfiguration->MenuCommandResponse->Append("\r\n              Reserved\r\n    Location    Size      Size      CRC     Name\r\n");
	for(i = 0;    i < FlashDisk->NumberOfMatrixFlashDiskEntries;    ++i)
	{
		if(FlashDisk->MatrixFlashDiskEntries[i].Type == MATRIX_FLASH_DISK::FD_EDIT_SCRIPT)
		{
			sprintf(Temp, "    %08X  %08X  %08X  %08X  %s\r\n",
						((UINT) FlashDisk->MatrixFlashDiskEntries[i].Location),
						((UINT) FlashDisk->MatrixFlashDiskEntries[i].ReservedSize),
						((UINT) FlashDisk->MatrixFlashDiskEntries[i].Size),
						((UINT) FlashDisk->MatrixFlashDiskEntries[i].CRC),
						FlashDisk->MatrixFlashDiskEntries[i].Name);
			ReaderConfiguration->MenuCommandResponse->Append(Temp);
		}
	}

	ReaderConfiguration->MenuCommandResponse->Append("\r\n");
#endif
	return(true);
} // HHPLEditScriptDirectory





//
//	HHPLMainScriptFileList
//		Output a list of main script files separated by commas.
//
bool HHPLMainScriptFileList(void)
{
#ifdef FIXME
	int		i;
	bool	AlreadyAtLeastOneFileFound = false;			// Used to indicate when a RS character is needed.


	for(i = 0;    i < FlashDisk->NumberOfMatrixFlashDiskEntries;    ++i)
	{
		if(FlashDisk->MatrixFlashDiskEntries[i].Type == MATRIX_FLASH_DISK::FD_MAIN_SCRIPT)
		{
			if(AlreadyAtLeastOneFileFound)
			{
				ReaderConfiguration->MenuCommandResponse->AppendChar(RS);
			}
			ReaderConfiguration->MenuCommandResponse->Append(FlashDisk->MatrixFlashDiskEntries[i].Name);
			AlreadyAtLeastOneFileFound = true;
		}
	}
#endif
	return(true);
} // HHPLMainScriptFileList





//
//	HHPLEditScriptFileList
//		Output a list of edit script files separated by commas.
//
bool HHPLEditScriptFileList(void)
{
#ifdef FIXME
	int		i;
	bool	AlreadyAtLeastOneFileFound = false;			// Used to indicate when a RS character is needed.


	for(i = 0;    i < FlashDisk->NumberOfMatrixFlashDiskEntries;    ++i)
	{
		if(FlashDisk->MatrixFlashDiskEntries[i].Type == MATRIX_FLASH_DISK::FD_EDIT_SCRIPT)
		{
			if(AlreadyAtLeastOneFileFound)
			{
				ReaderConfiguration->MenuCommandResponse->AppendChar(RS);
			}
			ReaderConfiguration->MenuCommandResponse->Append(FlashDisk->MatrixFlashDiskEntries[i].Name);
			AlreadyAtLeastOneFileFound = true;
		}
	}
#endif
	return(true);
} // HHPLEditScriptFileList





//
//	HHPLInterpreterVersion
//		Return the interpreter version string.
//
bool HHPLInterpreterVersion(void)
{
	bool bIsOk;
#if defined(RFBASE) || defined(CF2D)
#else
	HWSTRING	hwstrVersionString( HHPL_INTERPRETER_VERSION );
	hwstrVersionString.ConvertAndAppendToHString( ReaderConfiguration->MenuCommandResponse );
	bIsOk = true;
#endif

	return bIsOk;
} // HHPLInterpreterVersion





#ifndef RFBASE
void READER_CONFIGURATION::CreateScriptingMenuSettings( int /* ControlMask */, UINT ScriptingCommandModifier )
#else
#endif
{
// Script settings.
	AddNumber("0C0100", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING , NULL, NULL, &TotalScriptStorageSpace,
				DEFAULT_SCRIPT_STORAGE_SPACE, DEFAULT_SCRIPT_STORAGE_SPACE,
				MINIMUM_SCRIPT_STORAGE_SPACE, MAXIMUM_SCRIPT_STORAGE_SPACE);

#if !defined(RFBASE) && !defined(CF2D)
	AddString("0C0101", ScriptingCommandModifier | WORKGROUP , &ValidateMainScriptName, NULL, ActiveMainScript,
				SIZE_OF_SCRIPT_NAME, "", "");
	AddString("0C0102", ScriptingCommandModifier | WORKGROUP, &ValidateIQEditProScriptName, NULL, ActiveIQEditProScript,
				SIZE_OF_SCRIPT_NAME, "", "");

	AddCommand("0C0103", ScriptingCommandModifier | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &DeleteHHPLMainScript, NULL);
	AddCommand("0C0104", ScriptingCommandModifier | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &DeleteHHPLEditScript, NULL);

	AddCommand("0C0105", ScriptingCommandModifier | LOCAL_SETTING , &TerminateHHPLMainScript, NULL);
	AddCommand("0C0106", ScriptingCommandModifier | LOCAL_SETTING , &TerminateHHPLEditScript, NULL);

	AddCommand("0C0107", ScriptingCommandModifier | LOCAL_SETTING , &HHPLMainScriptDirectory, NULL);
	AddCommand("0C0108", ScriptingCommandModifier | LOCAL_SETTING , &HHPLEditScriptDirectory, NULL);
	AddCommand("0C0109", ScriptingCommandModifier | LOCAL_SETTING , &HHPLMainScriptFileList, NULL);
	AddCommand("0C010A", ScriptingCommandModifier | LOCAL_SETTING , &HHPLEditScriptFileList, NULL);

	AddBoolean	("0C010B", ScriptingCommandModifier | WORKGROUP, NULL, NULL, &MainScriptErrorsOn, false, false);
	AddBoolean	("0C010C", ScriptingCommandModifier | WORKGROUP, NULL, NULL, &EditScriptErrorsOn, false, false);

	AddCommand("0C010D", ScriptingCommandModifier | LOCAL_SETTING , &HHPLInterpreterVersion, NULL);
#endif


	return;
}



