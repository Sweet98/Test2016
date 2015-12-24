//======================================================================================
// PluginMenuProcess.c
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Plugin/PluginMenuProcess.c $
// $Revision: 1.15 $
// $Date: 2011/10/17 21:05:20EDT $
//======================================================================================

//======================================================================================

#ifdef PLUGIN_DEV

#include "PluginMenuProcess.h"
#include "Hstring.h"
#include "HONPlugin.h"
#include "HONPluginConfiguration.h"

#include "ioman.h"


static int HexAtoI(char *Data, int DataLen)
{
	char Digit;
	int Sum = 0, Value = 0;
	int i = 0, j;
	
	if(!DataLen)
		return -1;
	
	while(i < DataLen)
	{
		Digit = *(Data + i);
		if(Digit < '0' || (Digit > '9' && Digit < 'A') || (Digit > 'F' && Digit < 'a') || Digit > 'f')
			return -1;

		if(Digit >= '0' && Digit <= '9')
			Value = (int)(Digit - '0');
		else if(Digit >= 'A' && Digit <= 'F')
			Value = (int)(Digit - 0x37);
		else if(Digit >= 'a' && Digit <= 'f')
			Value = (int(Digit - 0x57));

		j = DataLen - i;
		while(--j)
			Value*=16;

		Sum+=Value;

		i++;
	}

	return Sum;
	
}


void PluginSerialMenuCommandProcess_Ext(HSTRING *PluginMenuCommand)
{

	HSTRING SingleCmd("",true);

	int LastOccurrencePosition = 0;
	int NextOccurrencePosition = 0;
	int NextOccurrenceNumber = 1;
	const unsigned char *PluginCmd = "990";
	const unsigned char *ExitCmd = "99Exit";
	int ID = -1;
	HONPluginClassType Type = HON_PLUGIN_TYPE_UNKNOW;

	while(NextOccurrencePosition < PluginMenuCommand->Size)
	{
		if(PluginMenuCommand->Find((unsigned char*)";", 1, false, NextOccurrenceNumber, &NextOccurrencePosition))
		{
			//theStdInterface.printf("NextP is %d, LastP is %d\r\n", NextOccurrencePosition, LastOccurrencePosition);
			if(!PluginMenuCommand->CompareOnShorter((unsigned int)LastOccurrencePosition, PluginCmd, 3, true) || 
				!PluginMenuCommand->CompareOnShorter((unsigned int)LastOccurrencePosition, ExitCmd, 6, true))
			{
				SingleCmd.Copy(PluginMenuCommand->Char + LastOccurrencePosition, NextOccurrencePosition - LastOccurrencePosition);
				// If this is a plug-in enter code, toss it and reply ENQ
				if(IsPluginMenuEnterCode((const char *) SingleCmd.Char, (int)SingleCmd.Size, ID, Type))
				{
					PluginMenuCommand->InsertNumber((NextOccurrencePosition - 1), ENQ, 1);
				}
				// If Exit code, reply ENQ
				else if(IsPluginMenuExitCode((const char *) SingleCmd.Char, (int)SingleCmd.Size))
				{
					PluginMenuCommand->InsertNumber((NextOccurrencePosition - 1), ENQ, 1);
				}
				// If normal plug-in menu codes, pass the code data to plug-in
				else if(IsNormalPluginMenuCode((const char *) SingleCmd.Char, (int)SingleCmd.Size, ID, Type))
				{
				// Just pass the data to plug-in and let plug-in do indication.
					if(!PluginCommandProcess((const char*)SingleCmd.Char+9, (int)(SingleCmd.Size-9), ID, Type))
						PluginMenuCommand->InsertNumber((NextOccurrencePosition - 1), ENQ, 1);
					else
						PluginMenuCommand->InsertNumber((NextOccurrencePosition - 1), ACK, 1);
				}
				else{
					PluginMenuCommand->InsertNumber((NextOccurrencePosition - 1), ENQ, 1);
				}
			}
			
			LastOccurrencePosition = NextOccurrencePosition + 1;
			NextOccurrenceNumber++;

		}
		else
		{
			break;
		}

	}

	// The last one
	if(!PluginMenuCommand->CompareOnShorter((unsigned int)LastOccurrencePosition, PluginCmd, 3, true) || 
			!PluginMenuCommand->CompareOnShorter((unsigned int)LastOccurrencePosition, ExitCmd, 6, true))
	{
		SingleCmd.Copy(PluginMenuCommand->Char + LastOccurrencePosition, NextOccurrencePosition - LastOccurrencePosition-1);
		// If this is a plug-in enter code, toss it and reply ENQ
		if(IsPluginMenuEnterCode((const char *) SingleCmd.Char, (int)SingleCmd.Size, ID, Type))
		{
			PluginMenuCommand->InsertNumber((NextOccurrencePosition - 1), ENQ, 1);
		}
		// If Exit code, reply ENQ
		else if(IsPluginMenuExitCode((const char *) SingleCmd.Char, (int)SingleCmd.Size))
		{
			PluginMenuCommand->InsertNumber((NextOccurrencePosition - 1), ENQ, 1);
		}
		// If normal plug-in menu codes, pass the code data to plug-in
		else if(IsNormalPluginMenuCode((const char *) SingleCmd.Char, (int)SingleCmd.Size, ID, Type))
		{
		// Just pass the data to plug-in and let plug-in do indication.
			if(!PluginCommandProcess((const char*)SingleCmd.Char+9, (int)(SingleCmd.Size-9), ID, Type))
				PluginMenuCommand->InsertNumber((NextOccurrencePosition - 1), ENQ, 1);
			else
				PluginMenuCommand->InsertNumber((NextOccurrencePosition - 1), ACK, 1);
		}
		else{
			PluginMenuCommand->InsertNumber((NextOccurrencePosition - 1), ENQ, 1);
		}
	}

	// Append terminator
	PluginMenuCommand->AppendChar((int)'.');

}


bool MultiPluginMenuBarcodesProcess(HSTRING *PluginMenuCommand, int MenuCount)
{

	HSTRING SingleCmd("",true);

	int LastOccurrencePosition = 0;
	int NextOccurrencePosition = 0;
	int NextOccurrenceNumber = 1;
	const unsigned char *PluginCmd = "990";
	const unsigned char *ExitCmd = "99Exit";
	int ID = -1;

	HONPluginClassType Type = HON_PLUGIN_TYPE_UNKNOW;
	bool processed = true;

	HONPlugin *LockedPlugin = SearchLockedPlugin();


	while(NextOccurrencePosition < PluginMenuCommand->Size)
	{
		if(PluginMenuCommand->Find((unsigned char*)";", 1, false, NextOccurrenceNumber, &NextOccurrencePosition))
		{
			//theStdInterface.printf("NextP is %d, LastP is %d\r\n", NextOccurrencePosition, LastOccurrencePosition);
			if(!PluginMenuCommand->CompareOnShorter((unsigned int)LastOccurrencePosition, PluginCmd, 3, true) || 
				!PluginMenuCommand->CompareOnShorter((unsigned int)LastOccurrencePosition, ExitCmd, 6, true))
			{
				SingleCmd.Copy(PluginMenuCommand->Char + LastOccurrencePosition, NextOccurrencePosition - LastOccurrencePosition);
				// If this is a plug-in enter code, we don't process enter and exit code for multiple plugin
				if(IsPluginMenuEnterCode((const char *) SingleCmd.Char, (int)SingleCmd.Size, ID, Type))
				{
					if(LockedPlugin){
						processed = false;
					}
					else if(MenuCount > 1){
						processed = false;
					}
					else{
						if(!LockPlugin(ID, Type)){
							processed = false;
						}
					}
				}
				// If Exit code
				else if(IsPluginMenuExitCode((const char *) SingleCmd.Char, (int)SingleCmd.Size))
				{
					if(!LockedPlugin){
						processed = false;
					}
					else if(MenuCount > 1)
						processed = false;
					else
						UnlockPlugin(LockedPlugin);
				}
				// If normal plug-in menu codes, pass the code data to plug-in
				else if(IsNormalPluginMenuCode((const char *) SingleCmd.Char, (int)SingleCmd.Size, ID, Type)&&(LockedPlugin==NULL))
				{
					if(!PluginCommandProcess((const char*)SingleCmd.Char+9, (int)(SingleCmd.Size-9), ID, Type)){
						processed = false;
					}
				}
				else if(IsLegacyPluginMenuCode((const char *) SingleCmd.Char, (int)SingleCmd.Size, Type))
				{
					if(LockedPlugin){
						ID = LockedPlugin->GetMenuID();
						if(!PluginCommandProcess((const char*)SingleCmd.Char+4, (int)(SingleCmd.Size-4), ID, Type)){
							processed = false;
						}
					}else{
						processed = false;
					}
				}
				else{
					processed = false;
					
				}
			}
			
			LastOccurrencePosition = NextOccurrencePosition + 1;
			NextOccurrenceNumber++;

		}
		else
		{
			break;
		}

	}

	// The last one
	if(!PluginMenuCommand->CompareOnShorter((unsigned int)LastOccurrencePosition, PluginCmd, 3, true) || 
			!PluginMenuCommand->CompareOnShorter((unsigned int)LastOccurrencePosition, ExitCmd, 6, true))
	{
		SingleCmd.Copy(PluginMenuCommand->Char + LastOccurrencePosition, NextOccurrencePosition - LastOccurrencePosition);
		// If this is a plug-in enter code, we don't process enter and exit code for multiple plugin
		if(IsPluginMenuEnterCode((const char *) SingleCmd.Char, (int)SingleCmd.Size, ID, Type))
		{
			if(LockedPlugin)
				processed = false;
			else if(MenuCount > 1)
				processed = false;
			else{
				if(!LockPlugin(ID, Type))
					processed = false;
			}
		}
		// If Exit code
		else if(IsPluginMenuExitCode((const char *) SingleCmd.Char, (int)SingleCmd.Size))
		{
			if(!LockedPlugin)
				processed = false;
			else if(MenuCount > 1)
				processed = false;
			else
				UnlockPlugin(LockedPlugin);
		}
		// If normal plug-in menu codes, pass the code data to plug-in
		else if(IsNormalPluginMenuCode((const char *) SingleCmd.Char, (int)SingleCmd.Size, ID, Type)&&(LockedPlugin==NULL))
		{
			if(!PluginCommandProcess((const char*)SingleCmd.Char+9, (int)(SingleCmd.Size-9), ID, Type)){
				processed = false;
			}
		}
		else if(IsLegacyPluginMenuCode((const char *) SingleCmd.Char, (int)SingleCmd.Size, Type))
		{
			if(LockedPlugin){
				ID = LockedPlugin->GetMenuID();
				if(!PluginCommandProcess((const char*)SingleCmd.Char+4, (int)(SingleCmd.Size-4), ID, Type))
					processed = false;
			}else{
				processed = false;
			}
		}
		else{
			processed = false;
		}
	}

	return processed;

}



#if 1
void PluginSerialMenuCommandProcess(const char *pucBuffer, int  Size, HSTRING *PluginMenuCommand)
{
	int ID = -1;
	HONPluginClassType Type = HON_PLUGIN_TYPE_UNKNOW;
	// If this is a plug-in enter code, toss it and reply ENQ
	if(IsPluginMenuEnterCode((char *) pucBuffer, Size, ID, Type))
	{				
		PluginMenuCommand->AppendNumber(((unsigned int) ENQ), 1);
	}
	// If Exit code, reply ENQ
	else if(IsPluginMenuExitCode((char *) pucBuffer, Size))
	{
		PluginMenuCommand->AppendNumber(((unsigned int) ENQ), 1);
	}
	// If normal plug-in menu codes, pass the code data to plug-in
	else if(IsNormalPluginMenuCode((char *) pucBuffer, Size, ID, Type))
	{
	// Just pass the data to plug-in and let plug-in do indication.
		if(!PluginCommandProcess((char*)pucBuffer+9, Size-9, ID, Type))
			PluginMenuCommand->AppendNumber(((unsigned int) ENQ), 1);
		else
			PluginMenuCommand->AppendNumber(((unsigned int) ACK), 1);
	}
	else{
		PluginMenuCommand->AppendNumber(((unsigned int) ENQ), 1);
	}

}
#endif


bool PluginCommandProcess(const char *Data, int DataLen, int MenuID, HONPluginClassType Type)
{

	HONPluginRawInfo *PluginInfo;
	HONPlugin *Plugin;
	DataEditApi *FormatApis;
	DecodeApi *DecodeApis;
	void *Apis;
	int ret = -1;

	int i;

	for(i = 0; i<3; i++)
	{
		if(!g_PluginConfigurations[i]) continue;
		
		for(Plugin = g_PluginConfigurations[i]->GetFirstPlugin();Plugin;Plugin = Plugin->GetNextPlugin())
		{
			if(MenuID == Plugin->GetMenuID())
			{				
				PluginInfo = Plugin->GetRawInfo();
				if ( ! PluginInfo ) return false;
				Apis = PluginInfo->PluginApis;

				if(!Apis) return false;

				if(Type != PluginInfo->PluginRawClassType) return false;

				switch(Type){
					case HON_PLUGIN_IMAGE:
						return false;
					case HON_PLUGIN_FORMAT:
						FormatApis = (DataEditApi*)Apis;
						if(!FormatApis->ProcessingBarcode) return false;

						ret = FormatApis->ProcessingBarcode((char*)Data, DataLen);
						return ret?false:true;
					case HON_PLUGIN_DECODE:
						DecodeApis = (DecodeApi*)Apis;
						if(!DecodeApis->ProcessingBarcode) return false;

						ret = DecodeApis->ProcessingBarcode((char*)Data, DataLen);
						return ret?false:true;
				}

			}
		}
	}

	return false;		
}

bool PluginBarcodeProcess(const char *Data, int DataLen, int MenuID, HONPluginClassType Type)
{

	HONPluginRawInfo *PluginInfo;
	HONPlugin *Plugin;
	DataEditApi *FormatApis;
	DecodeApi *DecodeApis;
	void *Apis;

	int i;

	for(i = 0; i<3; i++)
	{
		if(!g_PluginConfigurations[i])
			continue;
		
		for(Plugin = g_PluginConfigurations[i]->GetFirstPlugin();Plugin;Plugin = Plugin->GetNextPlugin())
		{
			if(MenuID == Plugin->GetMenuID())
			{				
				PluginInfo = Plugin->GetRawInfo();
				Apis = PluginInfo->PluginApis;
				
				if(!PluginInfo || !Apis)
					return false;

				if(Type != PluginInfo->PluginRawClassType)
					return false;

				switch(Type){
					case HON_PLUGIN_IMAGE:
						return false;
					case HON_PLUGIN_FORMAT:
						FormatApis = (DataEditApi*)Apis;
						if(!FormatApis->ProcessingBarcode)
							return false;
						PLUGIN_TRACE(("Firmware:menu is %s\r\n", (char*)Data))
						PLUGIN_TRACE(("Firmware:menu length is %d\r\n", DataLen))	
						FormatApis->ProcessingBarcode((char*)Data, DataLen);
						return true;
					case HON_PLUGIN_DECODE:
						DecodeApis = (DecodeApi*)Apis;
						if(!DecodeApis->ProcessingBarcode)
							return false;
						PLUGIN_TRACE(("Firmware:menu is %s\r\n", (char*)Data))
						PLUGIN_TRACE(("Firmware:menu length is %d\r\n", DataLen))	
						DecodeApis->ProcessingBarcode((char*)Data, DataLen);
						return true;
				}

			}
		}
	}

	return false;


		
}

/**************************************************************************************
 * IsPluginMenuEnterCode
 * called to validate the decode result is a Plugin menuing enter code or not
 * Format of enter code is:
 *			990XEntYYYYY
 * 
 * static int  IsPluginMenuEnterCode: 
 *			return the identifier of the enter code if this is a enter code.
 *			return -1 if not a enter code
 *			The identifier is defined as 2 digits Hex for the time being
 **************************************************************************************/
bool IsPluginMenuEnterCode(const char *Data, int DataLen, int &MenuID, HONPluginClassType &Type)
{
	if(DataLen != 12)
		return false;

	
	if(*Data != '9' || *(Data+1) !='9' || *(Data+2) != '0' || 
		*(Data+4) != 'E' || *(Data+5) != 'n' || *(Data+6) != 't')
		return false;

	switch(*(Data+3))
	{
		case '0':
			Type = HON_PLUGIN_DECODE;
			break;
		case '1':
			Type = HON_PLUGIN_IMAGE;
			break;
		case '2':
			Type = HON_PLUGIN_FORMAT;
			break;

		default:
			return false;
	}

	MenuID = HexAtoI((char*)(Data+7), 5);

	if(MenuID != -1)
		return true;
	else
		return false;
	
}


/**************************************************************************************
 * IsPluginMenuExitCode
 * called to validate the decode result is a Plugin menuing exit code or not
 * Format of exit code is:
 *			99Exit
 * 
 * static int  IsPluginMenuEnterCode: 
 *			return 0 if not a exit code and 1 if a exit code.
 **************************************************************************************/
bool IsPluginMenuExitCode(const char *Data, int DataLen)
{
	if(DataLen != 6)
		return false;
	
	if(!strncmp(Data, "99Exit", DataLen))
		return true;
	else
		return false;
}


bool IsNormalPluginMenuCode(const char *Data, int DataLen, int &MenuID, HONPluginClassType &Type)
{
	if(DataLen < 10)
		return false;

	if(*Data != '9' || *(Data+1) !='9' || *(Data+2) != '0')
		return false;

	switch(*(Data+3))
	{
		case '0':
			Type = HON_PLUGIN_DECODE;
			break;
		case '1':
			Type = HON_PLUGIN_IMAGE;
			break;
		case '2':
			Type = HON_PLUGIN_FORMAT;
			break;

		default:
			return false;
	}

	MenuID = HexAtoI((char*)(Data+4), 5);

	if(MenuID != -1)
		return true;
	else
		return false;
}



bool IsLegacyPluginMenuCode(const char *Data, int DataLen, HONPluginClassType &Type)
{
	if(DataLen < 5)
		return false;

	if(*Data != '9' || *(Data+1) !='9' || *(Data+2) != '0')
		return false;

	switch(*(Data+3))
	{
		case '0':
			Type = HON_PLUGIN_DECODE;
			break;
		case '1':
			Type = HON_PLUGIN_IMAGE;
			break;
		case '2':
			Type = HON_PLUGIN_FORMAT;
			break;

		default:
			return false;
	}

	return true;

}

bool LockPlugin(int MenuID, HONPluginClassType Type)
{
	int i = 0;
	HONPlugin *Plugin = NULL;

	
	for(i = 0; i<3; i++)
	{
		if(!g_PluginConfigurations[i])
			continue;
		
		for(Plugin = g_PluginConfigurations[i]->GetFirstPlugin();Plugin;Plugin = Plugin->GetNextPlugin())
		{
			if(MenuID == Plugin->GetMenuID() && Type == Plugin->GetRawInfo()->PluginRawClassType)
			{
				if(Plugin->GetMenuLock() == false)
				{
					Plugin->SetMenuLock(true);
					return true;
				}
				else
					return false;
			}
		}
	}

	return false;
}


void UnlockPlugin(HONPlugin *Plugin)
{
	Plugin->SetMenuLock(false);
}


HONPlugin* SearchLockedPlugin()
{
	int i = 0;
	HONPlugin *Plugin = NULL;

	
	for(i = 0; i<3; i++)
	{
		if(!g_PluginConfigurations[i])
			continue;
		
		for(Plugin = g_PluginConfigurations[i]->GetFirstPlugin();Plugin;Plugin = Plugin->GetNextPlugin())
		{
			if(Plugin->GetMenuLock() == true)
			{
				return Plugin;
			}
		}
	}

	return NULL;
}

int AbstractPluginMenuCommands(HSTRING *PluginMenuCommand)
{
	int LastOccurrencePosition = 0;
	int NextOccurrencePosition = 0;
	int NextOccurrenceNumber = 1;
	const unsigned char *PluginCmd = "990";
	const unsigned char *ExitCmd = "99Exit";
	int nCount = 0;

	while(NextOccurrencePosition < PluginMenuCommand->Size)
	{
		if(PluginMenuCommand->Find((unsigned char*)";", 1, false, NextOccurrenceNumber, &NextOccurrencePosition))
		{
			//theStdInterface.printf("NextP is %d, LastP is %d\r\n", NextOccurrencePosition, LastOccurrencePosition);
			if(PluginMenuCommand->CompareOnShorter((unsigned int)LastOccurrencePosition, PluginCmd, 3, true) && 
				PluginMenuCommand->CompareOnShorter((unsigned int)LastOccurrencePosition, ExitCmd, 6, true))
			{
				PluginMenuCommand->Remove(LastOccurrencePosition, NextOccurrencePosition - LastOccurrencePosition + 1);
				NextOccurrenceNumber--;
			}else{
				LastOccurrencePosition = NextOccurrencePosition + 1;
				nCount ++;
			}
			
			NextOccurrenceNumber++;

		}
		else
		{
			break;
		}

	}


	// The last one
	if(PluginMenuCommand->CompareOnShorter((unsigned int)LastOccurrencePosition, PluginCmd, 3, true) && 
			PluginMenuCommand->CompareOnShorter((unsigned int)LastOccurrencePosition, ExitCmd, 6, true))
	{
		PluginMenuCommand->Remove(LastOccurrencePosition - 1, PluginMenuCommand->Size - LastOccurrencePosition);
	} else{
		nCount ++;
	}

	//theStdInterface.printf("Size is %d\r\n", PluginMenuCommand->Size);
	//if(PluginMenuCommand->Size == 0)
		//return false;

	return nCount;

	
}


int AbstractSystemMenuCommands(HSTRING *SystemPluginMenuCommand)
{
	int LastOccurrencePosition = 0;
	int NextOccurrencePosition = 0;
	int NextOccurrenceNumber = 1;
	const unsigned char *PluginCmd = "990";
	const unsigned char *ExitCmd = "99Exit";
	int nCount = 0;

	while(NextOccurrencePosition < SystemPluginMenuCommand->Size)
	{
		if(SystemPluginMenuCommand->Find((unsigned char*)";", 1, false, NextOccurrenceNumber, &NextOccurrencePosition))
		{
			//theStdInterface.printf("NextP is %d, LastP is %d\r\n", NextOccurrencePosition, LastOccurrencePosition);
			if(!SystemPluginMenuCommand->CompareOnShorter((unsigned int)LastOccurrencePosition, PluginCmd, 3, true) || 
				!SystemPluginMenuCommand->CompareOnShorter((unsigned int)LastOccurrencePosition, ExitCmd, 6, true))
			{
				SystemPluginMenuCommand->Remove(LastOccurrencePosition, NextOccurrencePosition - LastOccurrencePosition + 1);
				NextOccurrenceNumber--;
			}else{
				LastOccurrencePosition = NextOccurrencePosition + 1;
				nCount ++;
			}
			
			NextOccurrenceNumber++;

		}
		else
		{
			break;
		}

	}


	// The last one
	if(!SystemPluginMenuCommand->CompareOnShorter((unsigned int)LastOccurrencePosition, PluginCmd, 3, true) || 
		!SystemPluginMenuCommand->CompareOnShorter((unsigned int)LastOccurrencePosition, ExitCmd, 6, true))
	{
		SystemPluginMenuCommand->Remove(LastOccurrencePosition - 1, SystemPluginMenuCommand->Size - LastOccurrencePosition);
	} else{
		nCount ++;
	}

	//if(SystemPluginMenuCommand->Size == 0)
		//return false;

	return nCount;

	
}


#endif
//---------------------------------------------------------------------------
//
//  Module Interface Description:
//    This file contains the interfaces of the plug-in programming barcodes processing.
//
//---------------------------------------------------------------------------




