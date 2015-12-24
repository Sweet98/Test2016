//======================================================================================
// PluginFileProcessing.cpp
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Plugin/PluginFileProcessing.cpp $
// $Revision: 1.6 $
// $Date: 2011/05/20 13:39:53EDT $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Module Interface Description:
//    This file contains the Method implementation of downloading Plugins via QSPV.
//
//---------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include "PluginFileProcessing.h"
#include "HONPluginConfiguration.h"
#include "db_chrio.h"
#include "StartupFlashDisk.h"
#include "FileHandler.h"


#define MAX_FILENAME_LEN	256



PluginFileProcessor::PluginFileProcessor(void)
	:HTAG_FILE_POST_TRANSFER_PROCESSOR()
{
	return;
}

PluginFileProcessor::~PluginFileProcessor(void)
{
	return;
}


void PluginFileProcessor::StoreFile(int *Status, char *Result, HTAG_FILE *NewFile)
{
	F_FILE *PluginFile;
	F_FIND find;
	char filename[MAX_FILENAME_LEN];
	
	if ( NewFile->FileType == HTAG_FILE::HTAG_FILE_TYPE_MOCF_FILE )
	{
		CFileHandler::ProcessMocf(NewFile->Data->Char, NewFile->Data->Size);
		return;  // we'll never get here unless there's an error because we reboot after we process an MOCF.
	}
	
	char *pName = (char *)NewFile->FileName->Char;

	strncpy(filename, "/PluginFolder/", 15);
	strncat(filename, (const char*)pName, strlen(pName) > (MAX_FILENAME_LEN - 15) ? (MAX_FILENAME_LEN - 15) : strlen(pName));
	filename[MAX_FILENAME_LEN - 1] = 0; // make sure it's null terminated
	PLUGIN_TRACE(("File Saving...\r\n"))
	PLUGIN_TRACE((filename))
	PLUGIN_TRACE(("\r\n"))

	if (!f_findfirst("/PluginFolder",&find)){
		if (!find.attr&F_ATTR_DIR){
			f_mkdir("/PluginFolder");
		}
	} else{
		f_mkdir("/PluginFolder");
	}

	PluginFile = f_open(filename, "w");
	if(!PluginFile){
		PLUGIN_TRACE(("Cannot open file plugin\r\n"))
		return;
	}

	if(f_write(NewFile->Data->Char, 1, NewFile->Data->Size, PluginFile) != NewFile->Data->Size){
		PLUGIN_TRACE(("Write plugin file fail\r\n"))
	}

	PLUGIN_TRACE(("File save complete\r\n"))
	f_close(PluginFile);
	return;
}

void PluginFileProcessor::RetrieveFile(int *Status, char *Result, HTAG_FILE * File)
{
	return;
}

PluginFileProcessor *FileProcessor=NULL;

//=================================================================================
//                       File Modification History
//===================================================================================




