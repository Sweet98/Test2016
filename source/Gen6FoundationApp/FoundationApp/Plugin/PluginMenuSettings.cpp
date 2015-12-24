//======================================================================================
// PluginMenuSettings.cpp
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Plugin/PluginMenuSettings.cpp $
// $Revision: 1.16 $
// $Date: 2011/07/28 22:13:31EDT $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Module Description:
//      This file contains a routine that creates Plugin menu settings as part
//		of the overall reader configuration for an Emerald device.
//
//---------------------------------------------------------------------------
#ifdef PLUGIN_DEV

#include	"stdInclude.h"
#include	"Menu.h"
#include	"ReaderConfiguration.h"
#include	"readerconfigurationstorage.h"
#include "ioman.h"

#include	"PluginMenuSettings.h"			// PluginMenuSettings interface definition
#include "HONPluginConfiguration.h"
#include "StartupFlashDisk.h"
#include "db_hdw.h"


#ifndef SAFE_FFS_USED
#include "db_FlashDisk.h"
#include "db_flash.h"

#else
#include "num_m25pxx.h"
#include "flashdrv.h"
#include "mx25spi.h"
#endif

//extern void SendStringToInterface(const char *pucBuffer);
//extern void SendStringToInterface( const char *pucBuffer, int SizeOfBuffer);

#ifndef RFSCAN
extern void SendStringToInterface( const char *pucBuffer, int SizeOfBuffer, bool /* DoNotUseIndicators */ );
#endif

#define UNRECONG_PLUGIN_INFO	"Unrecognized plugin class type\r\n"
#define CONFIG_NOTFOUND_INFO	"Configuration not found, Plugin diabled or configuration file corrupt\r\n"


// Used by plugin report routine to save the last report class type
static int ReportedClass = 99;

bool ReportConfigFile(void)
{
	bool	Status = true;		// No problems encountered yet.

	// If there is an clasee assigned, get it, else we'll use the last class specified.
	if( ReaderConfiguration->ExtraneousTextAfterCommand->Size > 0 )
	{
		int NumberOfCharactersUsedInConversion;
		int ClassType = ReaderConfiguration->ExtraneousTextAfterCommand->ConvertToNumber
		                   ( 0, ReaderConfiguration->ExtraneousTextAfterCommand->Size - 1, &NumberOfCharactersUsedInConversion);
		if( NumberOfCharactersUsedInConversion == (int) ReaderConfiguration->ExtraneousTextAfterCommand->Size )
		{
			ReportedClass = ClassType;
		}
		else
		{
			Status = false;
		}
	}

	// If no errors occurred, report configuration file.
	if( Status )
	{		
		if(ReportedClass != 0 && ReportedClass != 1 && ReportedClass != 2 && ReportedClass != 99)
		{
			SendStringToInterface(UNRECONG_PLUGIN_INFO, strlen(UNRECONG_PLUGIN_INFO), true);
			return false;
		}
		
		if(ReportedClass == 0 && g_pImagingPluginConfiguration && g_pImagingPluginConfiguration->GetConfigFile())
		{
			g_pImagingPluginConfiguration->HONPluginDumpConfigFile();
		}
		else if(ReportedClass == 1 && g_pDecodingPluginConfiguration && g_pDecodingPluginConfiguration->GetConfigFile())
		{
			g_pDecodingPluginConfiguration->HONPluginDumpConfigFile();
		}
		else if(ReportedClass == 2 && g_pFormattingPluginConfiguration && g_pFormattingPluginConfiguration->GetConfigFile())
		{
			g_pFormattingPluginConfiguration->HONPluginDumpConfigFile();
		}
		else if(ReportedClass == 99)
		{
			if(g_pImagingPluginConfiguration && g_pImagingPluginConfiguration->GetConfigFile())
				g_pImagingPluginConfiguration->HONPluginDumpConfigFile();
			if(g_pDecodingPluginConfiguration && g_pDecodingPluginConfiguration->GetConfigFile())
				g_pDecodingPluginConfiguration->HONPluginDumpConfigFile();
			if(g_pFormattingPluginConfiguration && g_pFormattingPluginConfiguration->GetConfigFile())
				g_pFormattingPluginConfiguration->HONPluginDumpConfigFile();
		}
		else
		{
			SendStringToInterface(CONFIG_NOTFOUND_INFO, strlen(CONFIG_NOTFOUND_INFO), true);
			return false;
		}
	}

	return Status;

}


bool PluginDeleteFile(void)
{
	char RealFileName[1024];

	HSTRING	FileName( ReaderConfiguration->ExtraneousTextAfterCommand, true );
	FileName.UnprintableFormatsToCharacters();

	sprintf(RealFileName, "/PluginFolder/%s", FileName.Char);
	
#if 0

	int	FileIndex = FlashDisk->RemoveFile( (char *) FileName.Char );
	return ( (FileIndex < 0) ? false : true );
#else

	return  EraseFile(RealFileName)==0;
#endif

}

#define PLUGIN_TOTAL_SIZE 2*1024*1024
bool PluginShowDirectory(void)
{
#ifndef SAFE_FFS_USED

#else
	unsigned long used = 0;
	F_FIND find;
	char buf[FS_MAXPATH + FS_MAXLNAME + 1];
	if (!f_findfirst("/PluginFolder/*.*",&find))
	{
		do
		{
			sprintf (buf,"%-40.40s",find.filename);
			if (find.attr&F_ATTR_DIR)
			{
				sprintf (&buf[40]," .\r\n");
			}
			else
			{
				sprintf (&buf[40]," %d\r\n",find.filesize);
				used += find.filesize;
			}
			SendStringToInterface(buf, strlen(buf), true);
		}
		while (!f_findnext(&find));
	}

	sprintf(buf,"%d total,%d free,%d used\r\n",PLUGIN_TOTAL_SIZE,PLUGIN_TOTAL_SIZE - used, used);
	SendStringToInterface(buf, strlen(buf), true);
#endif
	return true;
}



//
//	PluginReadFile
//		Read a file in plugin folder.
//
bool PluginReadFile(void)
{
	int size=-1;
	char RealFileName[1024];
	unsigned char * pContents =NULL;
	HSTRING	FileName( ReaderConfiguration->ExtraneousTextAfterCommand, true );
	FileName.UnprintableFormatsToCharacters();

	sprintf(RealFileName, "/PluginFolder/%s", FileName.Char);
#if 0 //FIXME

	HSTRING	FileContents(20, true);
	int FileIndex = FlashDisk->Read( (char *) FileName.Char, &FileContents );
	// If we had no errors or the file is corrupt (mismatched CRC), show it.
	if((FileIndex >= 0) || (FileIndex == MATRIX_FLASH_DISK::FD_CORRUPT_FILE))
	{
		size =FileContents.Size;
		pContents=FileContents.Char;
	}
#else
	size = ReadAndCloseFile(RealFileName,0,&pContents); // this call allocates from heap need to free below
#endif

	for (int i=0;i<size;i+=80)
	{
		// Write out 80 characters per line.
		SendStringToInterface(((char *) &pContents[i]),((size-i)>80)?80:(size-i), true);
	}
#if 0 //FIXME
#else
	if (pContents)
	{
		free(pContents);
	}
#endif
	return (size < 0) ? false : true;
} // FlashDiskReadFile





//
//	PluginRenameFile
//		Rename a file in plugin folder.  The old name and new name should be separated by ":".
//
bool PluginRenameFile(void)
{
	int		FileIndex = -1;				// Haven't succeeded yet.
	char RealOld[128];
	char RealNew[128];
	HSTRING	OldFileName( 20, true );
	HSTRING	NewFileName( 20, true );
	int		PositionWhereColonWasFound;

	if( ReaderConfiguration->ExtraneousTextAfterCommand->Find( (unsigned char *) ":", 1, false, 1, &PositionWhereColonWasFound ) )
	{
		OldFileName.Copy( ReaderConfiguration->ExtraneousTextAfterCommand, 0, PositionWhereColonWasFound - 1 );
		NewFileName.Copy( ReaderConfiguration->ExtraneousTextAfterCommand, PositionWhereColonWasFound + 1,
		                  ReaderConfiguration->ExtraneousTextAfterCommand->Size - 1 );

	OldFileName.UnprintableFormatsToCharacters();
	NewFileName.UnprintableFormatsToCharacters();
	
	sprintf(RealOld, "/PluginFolder/%s", OldFileName.Char);
	sprintf(RealNew, "/PluginFolder/%s", NewFileName.Char);
#ifdef FIXME

		FileIndex = FlashDisk->RenameFile( (char *) OldFileName.Char, (char *) NewFileName.Char );
#else

		FileIndex= RenameFile(RealOld,RealNew);
#endif

	}
	return (FileIndex < 0) ? false : true;
}



//
//	PluginCopyFile
//		Copy a file in plugin folder.  The old name and new name should be separated by ":".
//
bool PluginCopyFile(void)
{
	int		FileIndex = -1;				// Haven't succeeded yet.
	char RealOld[128];
	char RealNew[128];

	HSTRING	OldFileName( 20, true );
	HSTRING	NewFileName( 20, true );
	int		PositionWhereColonWasFound;

	if( ReaderConfiguration->ExtraneousTextAfterCommand->Find( (unsigned char *) ":", 1, false, 1, &PositionWhereColonWasFound ) )
	{
		OldFileName.Copy( ReaderConfiguration->ExtraneousTextAfterCommand, 0, PositionWhereColonWasFound - 1 );
		NewFileName.Copy( ReaderConfiguration->ExtraneousTextAfterCommand, PositionWhereColonWasFound + 1,
		                  ReaderConfiguration->ExtraneousTextAfterCommand->Size - 1 );

	OldFileName.UnprintableFormatsToCharacters();
	NewFileName.UnprintableFormatsToCharacters();

	sprintf(RealOld, "/PluginFolder/%s", OldFileName.Char);
	sprintf(RealNew, "/PluginFolder/%s", NewFileName.Char);
#ifdef FIXME
		// Get the file contents and information, then duplicate everything but the filename when creating the new file.
		HSTRING	FileContents(20, true);

		FileIndex = FlashDisk->Read( (char *) OldFileName.Char, &FileContents );

		// If we had no errors, get the file information and copy it.
		if(FileIndex >= 0)
		{
			FileIndex = FlashDisk->Write( (char *) NewFileName.Char, (char *) FileContents.Char, FileContents.Size,
			                              FlashDisk->MatrixFlashDiskEntries[ FileIndex ].Type, FlashDisk->MatrixFlashDiskEntries[ FileIndex ].Placement,
			                              0, FlashDisk->MatrixFlashDiskEntries[ FileIndex ].ReservedSize, false, false,
			                              FlashDisk->MatrixFlashDiskEntries[ FileIndex ].Modifier );
		}
#else
		FileIndex=CopyFile(RealNew,RealOld);
#endif

	}

	return ( (FileIndex < 0) ? false : true );
}


bool PluginDeleteAll(void)
{
#ifndef SAFE_FFS_USED

#else
	F_FIND find;
	char buf[FS_MAXPATH + FS_MAXLNAME + 1];
	if (!f_findfirst("/PluginFolder/*.*",&find))
	{
		do
		{
			if (find.attr&F_ATTR_DIR)
			{
				;
			}
			else
			{
				sprintf (buf,"/PluginFolder/%s", find.filename);
				f_delete(buf);
			}
		}
		while (!f_findnext(&find));
	}

#endif
	return true;
}



bool PluginDecodeEnableChanged(void)
{
	int PluginEnable = ReaderConfiguration->MenuCommandResponse->Char[0]-'0';

	// No action if the setting value is the same as the old one.
	if(PluginEnable == (int)GET_SETTING(EnableDecodePlugins))
		return true;

	if(PluginEnable == 1){
		HSTRING ConfigFileName(GET_SETTING(DecodePluginName),true);
		ConfigFileName.UnprintableFormatsToCharacters();
		HONPluginConfiguration::RemoveDecodePlugins();
		HONPluginConfiguration::LoadDecodePlugins(&ConfigFileName);
	}else if(PluginEnable == 0){
		HONPluginConfiguration::RemoveDecodePlugins();
	}

	return true;
}


bool PluginFormatEnableChanged(void)
{

	int PluginEnable = ReaderConfiguration->MenuCommandResponse->Char[0]-'0';

	// No action if the setting value is the same as the old one.
	if(PluginEnable == (int)GET_SETTING(EnableFormatPlugins))
		return true;

	if(PluginEnable == 1){
		HSTRING ConfigFileName(GET_SETTING(FormatPluginName),true);
		ConfigFileName.UnprintableFormatsToCharacters();
		HONPluginConfiguration::RemoveFormatPlugins();
		HONPluginConfiguration::LoadFormatPlugins(&ConfigFileName);
	}else if(PluginEnable == 0){
		HONPluginConfiguration::RemoveFormatPlugins();
	}

	return true;
}

bool PluginDecodeConfigChanged(void)
{
	bool PluginEnable = GET_SETTING(EnableDecodePlugins);

	if(!PluginEnable)
		return true;
	
	HSTRING	FileName( ReaderConfiguration->MenuCommandResponse, true );
	FileName.UnprintableFormatsToCharacters();

	HSTRING ConfigFileName(GET_SETTING(DecodePluginName),true);
	ConfigFileName.UnprintableFormatsToCharacters();

	// Plugin config file name not changed, reture.
	if(!FileName.Compare(&ConfigFileName, false))
		return true;
	
	HONPluginConfiguration::RemoveDecodePlugins();
	HONPluginConfiguration::LoadDecodePlugins(&FileName);
	return true;
}


bool PluginFormatConfigChanged(void)
{
	bool PluginEnable = GET_SETTING(EnableFormatPlugins);

	if(!PluginEnable)
		return true;
	
	HSTRING	FileName( ReaderConfiguration->MenuCommandResponse, true );
	FileName.UnprintableFormatsToCharacters();

	HSTRING ConfigFileName(GET_SETTING(FormatPluginName),true);
	ConfigFileName.UnprintableFormatsToCharacters();

#ifdef XENON_LITE_1500		// 1500 only support EasyDL, if other config name, ignore.
	HSTRING EZDLConfigName("EasyDLConf", true);
	if(FileName.Compare(&EZDLConfigName, false) && FileName.Char[0] != NULL)
		return false;
#endif

	// Plugin config file name not changed, reture.
	if(!FileName.Compare(&ConfigFileName, false))
		return true;
	
	HONPluginConfiguration::RemoveFormatPlugins();
	HONPluginConfiguration::LoadFormatPlugins(&FileName);
	return true;
}


void READER_CONFIGURATION::CreatePluginMenuSettings(int ControlMask)
{
#ifndef RFBASE
#ifndef XENON_LITE_1500		// non 1500 settings
	AddBoolean("0A0100", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING, NULL, NULL, &EnableImagePlugins, false, false);
	AddBoolean("0A0101", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING, &PluginDecodeEnableChanged, NULL, &EnableDecodePlugins, false, false);
	AddString("0A0102", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING, NULL, NULL, ImagePluginName, SIZEOF_PLUGIN_NAME, "", "");
	AddString("0A0103", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING, &PluginDecodeConfigChanged, NULL, DecodePluginName, SIZEOF_PLUGIN_NAME, "", "");
	AddString("0A0104", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING, &PluginFormatConfigChanged, NULL, FormatPluginName, SIZEOF_PLUGIN_NAME, "", "");
#else
	AddStringList("0A0104", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING, &PluginFormatConfigChanged, NULL, FormatPluginName, SIZEOF_PLUGIN_NAME, "EasyDLConf", "EasyDLConf", "EasyDLConf");
#endif
	AddBoolean("0A0105", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING, &PluginFormatEnableChanged, NULL, &EnableFormatPlugins, false, false);
	//AddString("PLGFON", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING, &PluginFormatConfigChanged, NULL, FormatPluginName, SIZEOF_PLUGIN_NAME, "", "");
	AddBoolean("0A0106", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING, NULL, NULL, &EnableOutputDebugInformations, false, false);
	AddCommand("0A0107", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &ReportConfigFile, NULL);

#ifndef XENON_LITE_1500
	AddCommand("0A0108", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &PluginDeleteFile, NULL);
	AddCommand("0A0109", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &PluginShowDirectory, NULL);
	AddCommand("0A010A", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &PluginReadFile, NULL);
	AddCommand("0A010B", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &PluginRenameFile, NULL);
	AddCommand("0A010C", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &PluginCopyFile, NULL);
	AddCommand("0A010D", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &PluginDeleteAll, NULL);
#endif
	
#endif

}

#endif

//=================================================================================
//                       File Modification History
//===================================================================================

