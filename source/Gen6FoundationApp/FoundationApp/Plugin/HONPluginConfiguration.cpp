/*===============================================================================
 // HONPluginConfigure.cpp

   This file contains the plugin conbfiguration object the plugin.
   
   TODO:  This class does two things.  It handles the configuration file and
   it handles the plugins.  We should break it up into two classes (this and
   a PluginHandler one).
//===============================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Plugin/HONPluginConfiguration.cpp $
// $Revision: 1.24 $
// $Date: 2011/05/25 02:49:25EDT $
===================================================================*/
#include <ctype.h>

#ifdef TIXML_USE_STL
#include <sstream>
#include <iostream>
#endif

#include "HONPluginConfiguration.h"
#include "thread.h"
#include "db_chrio.h"

#include "PluginGpioInterface.h"
#include "FileHandler.h"
#include "beep.h"








//Create a HON plugin configuration instance
HONPluginConfiguration :: HONPluginConfiguration()
{
	m_ClassType = HON_PLUGIN_TYPE_UNKNOW;
	m_pMainRoutineExectiveSequence = NULL;
	m_pBarcodeProcessingSequence = NULL;
	m_PluginConfigurationMutex = NULL;
	m_SystemRoutineChainOnExit = HON_PLUGIN_CHAIN_ON_EXIT_CHAINIFFAILURE;
	m_pSystemExportedSyms = NULL;
	m_pSystemDummy = NULL;

	m_pFirstPlugin = NULL;
	m_pLastPlugin = NULL;

	m_pXMLConfigurationFile = NULL;

	HONPluginCreateMutex();
}


// Destructor
HONPluginConfiguration :: ~HONPluginConfiguration()
{
	if(m_pXMLConfigurationFile){
		delete m_pXMLConfigurationFile;
		m_pXMLConfigurationFile = NULL;
	}
}




void HONPluginConfiguration::Create(void)
{
	
	g_pImagingPluginConfiguration = new ImagingPlugingConfiguration();
	g_pDecodingPluginConfiguration = new DecodePlugingConfiguration();
	g_pFormattingPluginConfiguration = new FormattingPlugingConfiguration();
	
	g_PluginConfigurations[0] = g_pImagingPluginConfiguration;
	g_PluginConfigurations[1] = g_pDecodingPluginConfiguration;
	g_PluginConfigurations[2] = g_pFormattingPluginConfiguration;
}


#define PLUGIN_CLASS_NUM 3
void HONPluginConfiguration::LoadAllPlugins(void)
{
    HONPluginConfiguration  *Config = NULL;
    int i;

	CPluginGpio::CreatePluginGpio();

    for(i = 0; i < PLUGIN_CLASS_NUM; i++)
    {
        Config = g_PluginConfigurations[i];
        // No Configuration, continue;
        if(!Config) continue;

        // Not enabled, continue
        if(!Config->GetAmIEnabledSetting()) continue;
 
        // If load config file failed, it will not be treated as error, continue silently
        // FIXME: Or if we can provide another error beep to notice this
        if(!Config->HONPluginLoadConfigFile())
        {
            Config->DeleteConfigFile();
            Config->SetConfigFile(NULL);
            //SoundBadPsocCommunication();
            continue;
        }


        if(Config->HONPluginCreateLoadingSequence())
        {
            Config->HONPluginCreateMainRoutineSequence();
            Config->HONPluginCreateBarcodeProcessingSequence();
        }
        else
        {
            SoundBadPsocCommunication();
        }
    }
}


void HONPluginConfiguration::LoadFormatPlugins(HSTRING *ConfigFileName)
{
	// No Configuration, continue;
	if(!g_pFormattingPluginConfiguration) return;

 
	// If load config file failed, it will not be treated as error, continue silently
	// FIXME: Or if we can provide another error beep to notice this
	if(!g_pFormattingPluginConfiguration->HONPluginLoadConfigFile(ConfigFileName))
	{
		g_pFormattingPluginConfiguration->DeleteConfigFile();
		g_pFormattingPluginConfiguration->SetConfigFile(NULL);
		return;
	}

	if(!g_pFormattingPluginConfiguration->HONPluginCreateLoadingSequence())
	{
		SoundBadPsocCommunication();
		return;
	}

	return;
}


void HONPluginConfiguration::LoadDecodePlugins(HSTRING *ConfigFileName)
{
	// No Configuration, continue;
	if(!g_pDecodingPluginConfiguration) return;

	// Reload the plugin
	if(!g_pDecodingPluginConfiguration->HONPluginLoadConfigFile(ConfigFileName))
	{
		g_pDecodingPluginConfiguration->DeleteConfigFile();
		g_pDecodingPluginConfiguration->SetConfigFile(NULL);
		return;
	}

	if(!g_pDecodingPluginConfiguration->HONPluginCreateLoadingSequence())
	{
		SoundBadPsocCommunication();
		return;
	}

	return;
}

void HONPluginConfiguration::RemoveAllPlugins(void)
{
    HONPluginConfiguration  *Config = NULL;
    int i;

	CPluginGpio::CreatePluginGpio();

    for(i = 0; i < PLUGIN_CLASS_NUM; i++)
    {
        Config = g_PluginConfigurations[i];
        // No Configuration, continue;
        if(!Config) continue;

	// Destroy the loading sequence
	Config->HONPluginDestroyLoadingSequence();

	// Release the configuration file
	Config->DeleteConfigFile();

    }
}


void HONPluginConfiguration::RemoveFormatPlugins(void)
{
	if(g_pFormattingPluginConfiguration){
		g_pFormattingPluginConfiguration->HONPluginDestroyLoadingSequence();
		g_pFormattingPluginConfiguration->DeleteConfigFile();
	}

}


void HONPluginConfiguration::RemoveDecodePlugins(void)
{
	if(g_pDecodingPluginConfiguration){
		g_pDecodingPluginConfiguration->HONPluginDestroyLoadingSequence();
		g_pDecodingPluginConfiguration->DeleteConfigFile();
	}

}


// Create Mutex for Plugin
bool HONPluginConfiguration :: HONPluginCreateMutex()
{
	m_PluginConfigurationMutex = MT_CreateMutex(false);
	if(!m_PluginConfigurationMutex)
		return (false);
	else
		return (true);
}

// Close Mutex for Plugin
void HONPluginConfiguration :: HONPluginCloseMutex()
{
	MT_CloseMutex(m_PluginConfigurationMutex);
}

// Wait Mutex
bool HONPluginConfiguration :: HONPluginWaitMutex()
{
	return MT_WaitMutex(m_PluginConfigurationMutex, INFINITE);
}

// Release Mutex
void HONPluginConfiguration :: HONPluginReleaseMutex()
{
	MT_ReleaseMutex(m_PluginConfigurationMutex);
}


// Load configuration file
bool HONPluginConfiguration :: HONPluginLoadConfigFile(void)
{
	HSTRING ConfigFileName(GetMyConfigFileNameSetting(),true);
	ConfigFileName.UnprintableFormatsToCharacters();

	if( ConfigFileName.Size == 0 ) return false;

	m_ConfigurationFileName.Copy(USER_FOLDER);
	m_ConfigurationFileName.Append("/");
	m_ConfigurationFileName.Append(&ConfigFileName);

	if(!m_pXMLConfigurationFile)
		m_pXMLConfigurationFile = new TiXmlDocument();

	// Since we will clear the existing data every time we load the config file,
	// we don't need to delete the TiXmlDocument.
	return m_pXMLConfigurationFile->LoadFile((const char *)m_ConfigurationFileName.Char);
}


// Load configuration file
bool HONPluginConfiguration :: HONPluginLoadConfigFile(HSTRING *FileName)
{
	FileName->UnprintableFormatsToCharacters();

	if( FileName->Size == 0 ) return false;

	m_ConfigurationFileName.Copy(USER_FOLDER);
	m_ConfigurationFileName.Append("/");
	m_ConfigurationFileName.Append(FileName);

	if(!m_pXMLConfigurationFile)
		m_pXMLConfigurationFile = new TiXmlDocument();

	// Since we will clear the existing data every time we load the config file,
	// we don't need to delete the TiXmlDocument.
	return m_pXMLConfigurationFile->LoadFile((const char *)m_ConfigurationFileName.Char);
}

// Save configuration file
bool HONPluginConfiguration :: HONPluginSaveConfigFile()
{
	return m_pXMLConfigurationFile->SaveFile();
}
	
// Save configuration file with the giving name
bool HONPluginConfiguration :: HONPluginSaveConfigFile(const char * FileName)
{
	return m_pXMLConfigurationFile->SaveFile(FileName);
}

void HONPluginConfiguration :: HONPluginUpdateStatus(TiXmlElement *PluginID, const char *Status)
{
	TiXmlElement *Element;
	TiXmlText *LoadStatus;
	TiXmlText NewNode(Status);
	const char *NodeValue;

	Element = PluginID->FirstChildElement("loadstatus");

	// If there is no loadstatus element, add it
	if(!Element){
		TiXmlElement NewElement("loadstatus");
		PluginID->InsertEndChild(NewElement);
	}

	if(Element){
		/* Get value */
		LoadStatus = ElementGetText(Element);
		if(!LoadStatus){		// No Child
			Element->InsertEndChild(NewNode);
		}else {
			NodeValue = LoadStatus->Value();
			if(!strcmp(NodeValue, Status)){
				// Do nothing
			}
			else
			{
				// New Text Value
				Element->ReplaceChild(LoadStatus, NewNode);
			}
		}
	}

	HONPluginSaveConfigFile((const char *)m_ConfigurationFileName.Char);
}


bool HONPluginConfiguration :: HONPluginUpdateConfigFile(TiXmlElement *PluginID, HONPlugin *Plugin, HONPluginInfo InfoIdex)
{
	TiXmlElement *Element = NULL;
	TiXmlNode *OldNode;
	HONPluginRawInfo *RawInfo = Plugin->GetRawInfo();
	char *Info = NULL;
	HONPluginLoadStatus status = HON_PLUGIN_STATUS_UNKNOWN;

	if(InfoIdex == HON_PLUGIN_INFO_NAME){// name
		Element = PluginID->FirstChildElement("name");
		Info = RawInfo->PluginRawName;
	}else if(InfoIdex == HON_PLUGIN_INFO_COMPANY){// company
		Element = PluginID->FirstChildElement("company");
		Info = RawInfo->CompanyName;		
	}else if(InfoIdex == HON_PLUGIN_INFO_MAJOR){// major
		Element = PluginID->FirstChildElement("majorrevision");
		Info = RawInfo->MajorVersionNumber;
	}else if(InfoIdex == HON_PLUGIN_INFO_MINOR){// minor
		Element = PluginID->FirstChildElement("minorrevision");
		Info = RawInfo->MinorVersionNumber;
	}else if(InfoIdex == HON_PLUGIN_INFO_BUILD){// build
		Element = PluginID->FirstChildElement("build");
		Info = RawInfo->BuildNumber;		
	}else if(InfoIdex == HON_PLUGIN_INFO_LICENSED){// licesed
		Element = PluginID->FirstChildElement("licensed");
		if(Plugin->IsPluginLicensed())
			Info = "YES";
		else
			Info = "NO";
	}else if(InfoIdex == HON_PLUGIN_INFO_ACTIVE){// active
		Element = PluginID->FirstChildElement("active");
		if(Plugin->IsPluginActive())
			Info = "YES";
		else
			Info = "NO";	
	}else if(InfoIdex == HON_PLUGIN_INFO_CERTIFICATE){// certificate
		Element = PluginID->FirstChildElement("certificate");
		Info = RawInfo->CertificateNumber;
	}else if(InfoIdex == HON_PLUGIN_INFO_CERTIFICATETIME){// certificatetime
		Element = PluginID->FirstChildElement("certificatetime");
		Info = RawInfo->CertificateTime;
	}else if(InfoIdex == HON_PLUGIN_INFO_GUID){// guid
			Element = PluginID->FirstChildElement("guid");
			Info = RawInfo->GUID;
	}else if(InfoIdex == HON_PLUGIN_INFO_FILENAME){//filename
		Element = PluginID->FirstChildElement("filename");
		Info = RawInfo->PluginFileName;
	}else if(InfoIdex == HON_PLUGIN_INFO_LOADSTATUS){//filename
		Element = PluginID->FirstChildElement("loadstatus");
		// If there is no loadstatus element, add it
		if(!Element){
			TiXmlElement NewElement("loadstatus");
			PluginID->InsertEndChild(NewElement);
			Element = PluginID->FirstChildElement("loadstatus");
		}
		status = Plugin->GetPluginStatus();
		switch(status){
			case HON_PLUGIN_STATUS_UNKNOWN:
				Info = "UNKNOWN";
				break;
			case HON_PLUGIN_STATUS_SUCCESS:
				Info = "SUCCESS";
				break;
			case HON_PLUGIN_STATUS_INACTIVE:
				Info = "INACTIVE";
				break;
			case HON_PLUGIN_STATUS_UNLICENSED_THISPLUGIN:
				Info = "UNLICENSED:THISPLUGIN";
				break;
			case HON_PLUGIN_STATUS_UNLICENSED_ALLPLUGINS:
				Info = "UNLICENSED:ALLPLUGINS";
				break;
			case HON_PLUGIN_STATUS_MISSING_FILENOTFOUND:
				Info = "MISSING:FILENOTFOUND";
				break;
			case HON_PLUGIN_STATUS_MISSING_MAINNOTFOUND:
				Info = "MISSING:MAINNOTFOUND";
				break;
			case HON_PLUGIN_STATUS_MISSING_HELPERNOTFOUND:
				Info = "MISSING:HELPERNOTFOUND";			
				break;
			case HON_PLUGIN_STATUS_NORESOURCE_MALLOC:
				Info = "NORESOURCE:MALLOC";
				break;
			case HON_PLUGIN_STATUS_NORESOURCE_NEW:
				Info = "NORESOURCE:NEW";
				break;
			case HON_PLUGIN_STATUS_CORRUPT_SYMBOLREFERENC:
				Info = "CORRUPT:SYMBOLREFERENC";
				break;
			case HON_PLUGIN_STATUS_CORRUPT_RELOCATIONFAIL:
				Info = "CORRUPT:RELOCATIONFAIL";
				break;
			case HON_PLUGIN_STATUS_CORRUPT_RELOCATIONRANGE:
				Info = "CORRUPT:RELOCATIONRANGE";
				break;
			case HON_PLUGIN_STATUS_CORRUPT_RELOCATIONUNKNOWN:
				Info = "CORRUPT:RELOCATIONUNKNOWN";
				break;
			case HON_PLUGIN_STATUS_CONFIG_ENTRY:
				Info = "CONFIG:ENTRY";
				break;
			case HON_PLUGIN_STATUS_CONFIG_FILE:
				Info = "CONFIG:FILE";
				break;
			case HON_PLUGIN_STATUS_CONFIG_NOPLUGIN:
				Info = "CONFIG:NOPLUGIN";
				break;
			case HON_PLUGIN_STATUS_TERMINATE_ABNORMAL:
				Info = "TERMINATE:ABNORMAL";
				break;
			default:
				Info = "UNKNOWN";
				break;
		}
	}else
		return false;


	
	if(Element && Info && *Info != '\0'){
		TiXmlText NewNode(Info);
		/* Get value */
		for(OldNode = Element->FirstChild(); OldNode; OldNode = OldNode->NextSibling())
		{
			if(OldNode->ToText())		// Value
				break;
		}

		if(!OldNode || !(OldNode->ToText())){		// No Child
			Element->InsertEndChild(NewNode);
			return true;
		}else if(OldNode->ToText()){
			const char *NodeValue = OldNode->Value();
			if(!strcmp(NodeValue, Info)){
				// Do nothing
				return true;
			}
			else
			{
				// New Text Value
				Element->ReplaceChild(OldNode, NewNode);
				return true;
			}
		}

	}
	return false;
}



bool HONPluginConfiguration :: VerifyPluginIDUniqueness(TiXmlElement *PluginID)
{
	TiXmlElement *ID, *FileName, *PluginName;
	TiXmlNode *RootElement = PluginID->Parent();
	TiXmlText *FileNameV, *PluginNameV;
	const char *ThisValue, *CmpValue;

	if(!RootElement)
		return false;
	
	for( ID = RootElement->FirstChildElement(); ID != PluginID; ID = ID->NextSiblingElement()){
		// If SystemRoutine, continue
		if(ID->Value() && !strcmp(ID->Value(), "SystemRoutine"))
			continue;
		
		if(!strcmp(ID->Value(), PluginID->Value()))	// Two plugin have the same ID
			return false;
		
		PluginName = PluginID->FirstChildElement("name");
		FileName = PluginID->FirstChildElement("filename");
		if(!PluginName || !FileName)
			return false;

		// Check file name uniqueness
		FileNameV = ElementGetText(FileName);
		if(!FileNameV)
			return false;

		ThisValue = FileNameV->Value();

		FileName = ID->FirstChildElement("filename");
		FileNameV = ElementGetText(FileName);
		CmpValue = FileNameV->Value();

		if(!FileName || !FileNameV || !CmpValue)
			continue;


		if(!strcmp(ThisValue, CmpValue))
			return false;

		// Check name uniqueness
		PluginNameV = ElementGetText(PluginName);
		if(!PluginNameV)
			return false;

		ThisValue = PluginNameV->Value();

		PluginName = ID->FirstChildElement("name");
		PluginNameV = ElementGetText(PluginName);
		CmpValue = PluginNameV->Value();

		if(!PluginName || !PluginNameV || !CmpValue)
			continue;

		if(!strcmp(ThisValue, CmpValue))
			return false;

	}

	return true;

}




// Create plugin to executive sequence item
HONPlugin* HONPluginConfiguration :: HONPluginLoad(const char * PluginName)
{
	bool Ret;
	HONPlugin *Plugin = new HONPlugin(PluginName);

	if(!Plugin){
		return NULL;
	}

	Ret = Plugin->LoadPlugin();

	if(Ret == false)
	{
		Plugin->ClearPlugin();
	}

	return Plugin;
}


// Remove plugin from executive sequence item
bool HONPluginConfiguration :: HONPluginRemove(const char * PluginName)
{
	HONPlugin *Plugin = NULL;
	
	Plugin = HONPluginSearch(PluginName);
	if(!Plugin)
		return false;

	Plugin->RemovePlugin(&m_pFirstPlugin, &m_pLastPlugin);

	return true;

}


// Find Plugin in executive list
HONPlugin* HONPluginConfiguration :: HONPluginSearch(const char *PluginName)
{
	HONPlugin *Plugin;

	for(Plugin = m_pFirstPlugin; Plugin; Plugin = Plugin->GetNextPlugin())
	{
		if(!strncmp(PluginName, Plugin->GetPluginIdentifier(), strlen(PluginName)))
			return Plugin;
	}

	return NULL;
}


// Check declaration
bool HONPluginConfiguration :: HONPluginConfigCheckDeclaration()
{
	TiXmlNode *node;
	TiXmlDeclaration* declaration;


	node = m_pXMLConfigurationFile->FirstChild();
	declaration = node->ToDeclaration();

	if(!declaration){
		return false;
	}

	/* Check Version */	
	if((!declaration->Version()) || strncmp(declaration->Version(), "1.0", 3)){
		return false;
	}

	/* Check encoding */

	/* Check Standalong */

	return true;
	
}


// Configure a plugin
bool HONPluginConfiguration :: HONPluginConfigure(HONPlugin *Plugin, TiXmlNode *PluginConfigure)
{
	TiXmlElement *PluginConfigItem; 
	TiXmlText *PluginConfigValueItem;
	const char *ItemName;
	const char *ItemValue;
	
	PluginConfigItem = PluginConfigure->FirstChildElement();

	if(!PluginConfigItem)
		return false;

	/* Identifier */
	ItemValue = PluginConfigure->Value();

	if(!ItemValue)
		return false;

	//Plugin->SetPluginIdentifier(ItemValue);

	for(PluginConfigItem = PluginConfigure->FirstChildElement(); PluginConfigItem; PluginConfigItem = PluginConfigItem->NextSiblingElement())
	{
		ItemName = PluginConfigItem->Value();
		if(!ItemName){
			continue;
		}

		PluginConfigValueItem = ElementGetText(PluginConfigItem);

		if(!PluginConfigValueItem){
			ItemValue = NULL;

			// Default active
			//Plugin->SetPluginActive(true);
			continue;
		}else
			ItemValue = PluginConfigValueItem->Value();


		if(!memcmp(ItemName, "name", 4))
		{
			Plugin->SetPluginName(ItemValue);
		}
		else if(!memcmp(ItemName, "company", 7))
		{
			Plugin->SetPluginCompanyName(ItemValue);
		}
		else if(!memcmp(ItemName, "licensed", 8))
		{
			if(!memcmp(ItemValue, "YES", 3))
				Plugin->SetPluginLicensed(true);
			else
				Plugin->SetPluginLicensed(false);
		}
		else if(!memcmp(ItemName, "active", 6))
		{
			if(!memcmp(ItemValue, "NO", 3))
				Plugin->SetPluginActive(false);
			else
				Plugin->SetPluginActive(true);
		}
		else if(!memcmp(ItemName, "majorrevision", 13))
		{
			Plugin->SetPluginMajorRevision(ItemValue);
		}
		else if(!memcmp(ItemName, "minorrevision", 13))
		{
			Plugin->SetPluginMinorRevision(ItemValue);
		}
		else if(!memcmp(ItemName, "build", 5))
		{
			Plugin->SetPluginBuild(ItemValue);
		}
		else if(!memcmp(ItemName, "certificate", 11) && (*(ItemName+11) == '\0'))
		{
			Plugin->SetPluginCertificate(ItemValue);
		}
		else if(!memcmp(ItemName, "certificatetime", 15))
		{
			Plugin->SetPluginCertificateTime(ItemValue);
		}
		else if(!memcmp(ItemName, "guid", 4))
		{
			Plugin->SetPluginGUID(ItemValue);
		}
		else if(!memcmp(ItemName, "filename", 8))
		{
			Plugin->SetPluginFileName(ItemValue);
		}
		else if(!memcmp(ItemName, "mainroutineorder", 16))
		{
			if(!memcmp(ItemValue, "BEFORE", 6))
				Plugin->SetMainRoutineOrder(HON_PLUGIN_MAIN_ROUTINE_ORDER_BEFORE);
			else
				Plugin->SetMainRoutineOrder(HON_PLUGIN_MAIN_ROUTINE_ORDER_AFTER);
		}
		else if(!memcmp(ItemName, "barcodeinterceptmode", 20))
		{
			if(!memcmp(ItemValue, "YES", 3))
				Plugin->SetInterceptMode(true);
			else
				Plugin->SetInterceptMode(false);
		}
		else if(!memcmp(ItemName, "entrydatastate", 14))
		{	
			if(!memcmp(ItemValue, "MODIFIED", 8))
				Plugin->SetEntryDataState(HON_PLUGIN_ENTRY_DATA_STATE_MODIFIED);
			else
				Plugin->SetEntryDataState(HON_PLUGIN_ENTRY_DATA_STATE_ORIGINAL);
		}
		else if(!memcmp(ItemName, "chainonexit", 11))
		{
			if(!memcmp(ItemValue, "CHAINALWAYS", 11))
				Plugin->SetChainOnExit(HON_PLUGIN_CHAIN_ON_EXIT_CHAINALWAYS);
			else if(!memcmp(ItemValue, "CHAINIFFAILURE", 14))
				Plugin->SetChainOnExit(HON_PLUGIN_CHAIN_ON_EXIT_CHAINIFFAILURE);
			else if(!memcmp(ItemValue, "CHAINIFSUCCESS", 14))
				Plugin->SetChainOnExit(HON_PLUGIN_CHAIN_ON_EXIT_CHAINIFSUCCESS);
			else
				Plugin->SetChainOnExit(HON_PLUGIN_CHAIN_ON_EXIT_CHAINNEVER);
		}
	}
	
	return true;
	
}



// Excution of the sequnce
bool HONPluginConfiguration :: HONPluginExecuteSequence()
{
	return true;
}


// Create plugin main routine sequance
bool HONPluginConfiguration :: HONPluginCreateMainRoutineSequence()
{
	HONPlugin *Plugin, *PluginPrev;
	HONPluginRawInfo *RawInfo;

	m_pMainRoutineExectiveSequence = NULL;

	// Add all 'BEFORE' routines to sequence first
	for(Plugin = m_pFirstPlugin, PluginPrev = NULL; Plugin; Plugin = Plugin->GetNextPlugin())
	{
		RawInfo = Plugin->GetRawInfo();
		if(RawInfo && RawInfo->PluginApis){
			if(Plugin->GetMainRoutineOrder() == HON_PLUGIN_MAIN_ROUTINE_ORDER_BEFORE)
			{
				if(!PluginPrev)
					m_pMainRoutineExectiveSequence = Plugin;
				else
					PluginPrev->SetMainRoutineNext(Plugin);

				PluginPrev = Plugin;
			}
		}
	}

	// Then add system routine to sequence
	if(!PluginPrev)
		m_pMainRoutineExectiveSequence = m_pSystemDummy;
	else
		PluginPrev->SetMainRoutineNext(m_pSystemDummy);

	// Add remaining plugins
	for(Plugin = m_pFirstPlugin, PluginPrev = m_pSystemDummy; Plugin; Plugin = Plugin->GetNextPlugin())
	{
		RawInfo = Plugin->GetRawInfo();
		if(RawInfo && RawInfo->PluginApis){
			if(Plugin->GetMainRoutineOrder() == HON_PLUGIN_MAIN_ROUTINE_ORDER_AFTER)
			{
				PluginPrev->SetMainRoutineNext(Plugin);
				PluginPrev = Plugin;
			}
		}
	}

	if(PluginPrev)
		PluginPrev->SetMainRoutineNext(NULL);

	return true;
	
}

// Create plugin barcode processing sequance
bool HONPluginConfiguration :: HONPluginCreateBarcodeProcessingSequence()
{
	HONPlugin *Plugin, *PluginPrev;
	HONPluginRawInfo *RawInfo;

	m_pBarcodeProcessingSequence = NULL;
	PluginPrev = m_pBarcodeProcessingSequence;

	// Add all 'BEFORE' routines to sequence first
	for(Plugin = m_pFirstPlugin; Plugin; Plugin = Plugin->GetNextPlugin())
	{
		RawInfo = Plugin->GetRawInfo();
		if(RawInfo && RawInfo->PluginApis){
			if(Plugin->IsInterceptMode() == true)
			{
				if(!PluginPrev)
					m_pBarcodeProcessingSequence = Plugin;
				else
					PluginPrev->SetBarcodeProcessingNext(Plugin);

				PluginPrev = Plugin;
			}
		}
	}


	/* We have no system code processing handler in this chain */
	// Then add system barcode processing function to sequence
//	if(!PluginPrev)
//		m_pMainRoutineExectiveSequence = m_pSystemDummy;
//	else
//		PluginPrev->SetMainRoutineNext(m_pSystemDummy);

	// Add remaining plugins
	for(Plugin = m_pFirstPlugin; Plugin; Plugin = Plugin->GetNextPlugin())
	{
		RawInfo = Plugin->GetRawInfo();
		if(RawInfo && RawInfo->PluginApis){
			if(Plugin->IsInterceptMode() == false)
			{
				if(!PluginPrev)
					m_pBarcodeProcessingSequence = Plugin;

				else
					PluginPrev->SetBarcodeProcessingNext(Plugin);

				PluginPrev = Plugin;
			}
		}
	}

	if(PluginPrev)
		PluginPrev->SetBarcodeProcessingNext(NULL);

	return true;
}


// Destroy plugin loading sequance
void HONPluginConfiguration :: HONPluginDestroyLoadingSequence()
{
	HONPlugin *Plugin, *Next;
	
	for(Plugin = m_pFirstPlugin;Plugin;)
	{
		Next = Plugin->GetNextPlugin();
		delete Plugin;
		Plugin = Next;
	}

	m_pFirstPlugin = m_pLastPlugin = NULL;
	m_pSystemDummy = NULL;
	m_pMainRoutineExectiveSequence = NULL;
	m_pBarcodeProcessingSequence = NULL;
	
	return;
}

// Create plugin loading sequance
bool HONPluginConfiguration :: HONPluginCreateLoadingSequence()
{
	TiXmlText *FileName, *ActiveValue; //TiXmlText *IsPluginLicensedValue;
	TiXmlElement *RootElement, *Active, *PluginID, *File; //TiXmlElement *IsPluginLicensed;
	HONPlugin *Plugin;
	int SysRoutineAdded = 0;
	bool Err = true;

	/* Skip comments and get the plugin class type */
	RootElement = m_pXMLConfigurationFile->RootElement();
	
	PluginID = RootElement->FirstChildElement();
	
	/* Check Declaration */
	if(!HONPluginConfigCheckDeclaration()){
		HONPluginUpdateStatus(PluginID, "CONFIG:ENTRY");
		PLUGIN_TRACE(("Config file is corrupted\r\n"))
		return false;
	}

	if(!RootElement){
		HONPluginUpdateStatus(PluginID, "CONFIG:ENTRY");
		PLUGIN_TRACE(("Cannot parse plugin config file\r\n"))
		return false;
	}

	if(!strncmp(RootElement->Value(), "Format_PlugIn", 13))
		m_ClassType = HON_PLUGIN_FORMAT;
	else if(!strncmp(RootElement->Value(), "ImageProcessing_PlugIn", 22))
		m_ClassType = HON_PLUGIN_IMAGE;
	else if(!strncmp(RootElement->Value(), "Decode_PlugIn", 13))
		m_ClassType = HON_PLUGIN_DECODE;
	else{
		HONPluginUpdateStatus(PluginID, "CONFIG:FILE");
		PLUGIN_TRACE(("Unkown plugin class type\r\n"))
		return false;
	}

	/* Create system dummy plug-in instance */
	m_pSystemDummy = new HONPlugin("m_pSystemDummy");

	/* Default m_pSystemDummy's setting so that if we don't have SystemRoutine entry still can set default */
	m_pSystemDummy->SetChainOnExit(HON_PLUGIN_CHAIN_ON_EXIT_CHAINALWAYS);
	m_pSystemDummy->SetEntryDataState(HON_PLUGIN_ENTRY_DATA_STATE_MODIFIED);

	/* Load and configure plugins */
	for( PluginID = RootElement->FirstChildElement(); PluginID != NULL; PluginID = PluginID->NextSiblingElement())
	{
		bool Ret;

		// Check if it is a system rotine setting
		if(PluginID->Value() && !strcmp(PluginID->Value(), "SystemRoutine"))
		{
			TiXmlElement *Element;
			TiXmlText *Value;

			// Chain on exit setting
			Element = PluginID->FirstChildElement("chainonexit");
			if(Element){
				Value = ElementGetText(Element);
				if(Value && Value->Value() && !strcmp(Value->Value(), "CHAINNEVER")){
					// CHAINNERVER
					m_pSystemDummy->SetChainOnExit(HON_PLUGIN_CHAIN_ON_EXIT_CHAINNEVER);
				}else if(Value && Value->Value() && !strcmp(Value->Value(), "CHAINIFFAILURE")){
					// CHAINIFFAILURE
					m_pSystemDummy->SetChainOnExit(HON_PLUGIN_CHAIN_ON_EXIT_CHAINIFFAILURE);
				}else if(Value && Value->Value() && !strcmp(Value->Value(), "CHAINIFSUCCESS")){
					// CHAINIFSUCCESS
					m_pSystemDummy->SetChainOnExit(HON_PLUGIN_CHAIN_ON_EXIT_CHAINIFSUCCESS);
				}else if(Value && Value->Value() && !strcmp(Value->Value(), "CHAINALWAYS")){
					// CHAINALWAYS
					m_pSystemDummy->SetChainOnExit(HON_PLUGIN_CHAIN_ON_EXIT_CHAINALWAYS);
				}else{
					// Default set to call system routine always
					m_pSystemDummy->SetChainOnExit(HON_PLUGIN_CHAIN_ON_EXIT_CHAINALWAYS);
				}
			}else{
				// If no chainonexit defined, just set default
				m_pSystemDummy->SetChainOnExit(HON_PLUGIN_CHAIN_ON_EXIT_CHAINALWAYS);
			}

			// Entry data state setting
			Element = PluginID->FirstChildElement("entrydatastate");
			if(Element){
				Value = ElementGetText(Element);
				if(Value && Value->Value() && !strcmp(Value->Value(), "MODIFIED")){
					// MODIFIED
					m_pSystemDummy->SetEntryDataState(HON_PLUGIN_ENTRY_DATA_STATE_MODIFIED);
				}else if(Value && Value->Value() && !strcmp(Value->Value(), "ORIGINAL")){
					// ORIGINAL
					m_pSystemDummy->SetEntryDataState(HON_PLUGIN_ENTRY_DATA_STATE_ORIGINAL);
				}else{
					// Default set to pass modified data as input
					m_pSystemDummy->SetEntryDataState(HON_PLUGIN_ENTRY_DATA_STATE_MODIFIED);
				}
			}else{
				// If no entrydatastate defined, just set default
				m_pSystemDummy->SetEntryDataState(HON_PLUGIN_ENTRY_DATA_STATE_MODIFIED);
			}

			// Add system routine to the plugin call sequence as a normal plugin
			m_pSystemDummy->AddEndPlugin(&m_pFirstPlugin, &m_pLastPlugin);
			SysRoutineAdded = 1;

			continue;
		}


		// If there are more than two plug-ins get same names, return error
		if(!VerifyPluginIDUniqueness(PluginID)){
			HONPluginUpdateStatus(PluginID, "DUPLICATEDEFINE");
			PLUGIN_TRACE(("Duplicated plugin configurations\r\n"));
			continue;
		}

		/* If Inactive, save status and skip */
		Active = PluginID->FirstChildElement("active");
		if(!Active){
			TiXmlNode *PreNode;
			TiXmlElement NewElement("active");

			PreNode=PluginID->FirstChildElement("licensed");
			PluginID->InsertAfterChild(PreNode,NewElement);
			Active = PluginID->FirstChildElement("active");
		}
		
		ActiveValue = ElementGetText(Active);

		if(ActiveValue && !strcmp("NO", ActiveValue->Value())){
			HONPluginUpdateStatus(PluginID, "INACTIVE");
			PLUGIN_TRACE((PluginID->Value()));
			PLUGIN_TRACE((" plugin is in-activated\r\n"));
			continue;
		}

		// Default as active
		if(!ActiveValue){
			HONPluginUpdateStatus(PluginID, "ACTIVE");
		}

		// Get filename
		File = PluginID->FirstChildElement("filename");
		if(!File){
			HONPluginUpdateStatus(PluginID, "CONFIG:NOPLUGIN");
			PLUGIN_TRACE(("Cannot get plugin file name definition in configuration file\r\n"));
			continue;
		}

		FileName = ElementGetText(File);
		if(!FileName){
			HONPluginUpdateStatus(PluginID, "CONFIG:NOPLUGIN");
			PLUGIN_TRACE(("Cannot get plugin file name definition in configuration file\r\n"));
			continue;
		}

		// Load Plugin
		Plugin = HONPluginLoad(FileName->Value());

		if(!Plugin){		//no plugin instance continue
			HONPluginUpdateStatus(PluginID, "NORESOURCE:NEW");
			continue;
		}

	       char *Info = NULL;
	       HONPluginLoadStatus status = HON_PLUGIN_STATUS_UNKNOWN;
              status = Plugin->GetPluginStatus();
		switch(status){
			case HON_PLUGIN_STATUS_UNKNOWN:
				Info = "UNKNOWN";
				break;
			case HON_PLUGIN_STATUS_SUCCESS:
				Info = "SUCCESS";
				break;
			case HON_PLUGIN_STATUS_INACTIVE:
				Info = "INACTIVE";
				break;
			case HON_PLUGIN_STATUS_UNLICENSED_THISPLUGIN:
				Info = "UNLICENSED:THISPLUGIN";
				break;
			case HON_PLUGIN_STATUS_UNLICENSED_ALLPLUGINS:
				Info = "UNLICENSED:ALLPLUGINS";
				break;
			case HON_PLUGIN_STATUS_MISSING_FILENOTFOUND:
				Info = "MISSING:FILENOTFOUND";
				break;
			case HON_PLUGIN_STATUS_MISSING_MAINNOTFOUND:
				Info = "MISSING:MAINNOTFOUND";
				break;
			case HON_PLUGIN_STATUS_MISSING_HELPERNOTFOUND:
				Info = "MISSING:HELPERNOTFOUND";			
				break;
			case HON_PLUGIN_STATUS_NORESOURCE_MALLOC:
				Info = "NORESOURCE:MALLOC";
				break;
			case HON_PLUGIN_STATUS_NORESOURCE_NEW:
				Info = "NORESOURCE:NEW";
				break;
			case HON_PLUGIN_STATUS_CORRUPT_SYMBOLREFERENC:
				Info = "CORRUPT:SYMBOLREFERENC";
				break;
			case HON_PLUGIN_STATUS_CORRUPT_RELOCATIONFAIL:
				Info = "CORRUPT:RELOCATIONFAIL";
				break;
			case HON_PLUGIN_STATUS_CORRUPT_RELOCATIONRANGE:
				Info = "CORRUPT:RELOCATIONRANGE";
				break;
			case HON_PLUGIN_STATUS_CORRUPT_RELOCATIONUNKNOWN:
				Info = "CORRUPT:RELOCATIONUNKNOWN";
				break;
			case HON_PLUGIN_STATUS_CONFIG_ENTRY:
				Info = "CONFIG:ENTRY";
				break;
			case HON_PLUGIN_STATUS_CONFIG_FILE:
				Info = "CONFIG:FILE";
				break;
			case HON_PLUGIN_STATUS_CONFIG_NOPLUGIN:
				Info = "CONFIG:NOPLUGIN";
				break;
			case HON_PLUGIN_STATUS_TERMINATE_ABNORMAL:
				Info = "TERMINATE:ABNORMAL";
				break;
			default:
				Info = "UNKNOWN";
				break;
		}
		
		HONPluginUpdateStatus(PluginID, Info);
		if(status!=HON_PLUGIN_STATUS_SUCCESS)
		{
			PLUGIN_TRACE(("\r\nload plugin failed, status:\r\n"));
			PLUGIN_TRACE(("%s\r\n", Info));
			if(Plugin)
				delete Plugin;
			
			continue;
		}

		// If we connot load a plugin by a giving name, do not return directly
		//if(Plugin && !Plugin->GerRowInfo())
			//HONPluginUpdateConfigFile(PluginID, Plugin, HON_PLUGIN_INFO_LOADSTATUS);

		// Update configuration file
		if(Plugin->GetRawInfo()){
			HONPluginUpdateConfigFile(PluginID, Plugin, HON_PLUGIN_INFO_NAME);
			HONPluginUpdateConfigFile(PluginID, Plugin, HON_PLUGIN_INFO_COMPANY);
			HONPluginUpdateConfigFile(PluginID, Plugin, HON_PLUGIN_INFO_MAJOR);
			HONPluginUpdateConfigFile(PluginID, Plugin, HON_PLUGIN_INFO_MINOR);
			HONPluginUpdateConfigFile(PluginID, Plugin, HON_PLUGIN_INFO_BUILD);
			HONPluginUpdateConfigFile(PluginID, Plugin, HON_PLUGIN_INFO_CERTIFICATE);
			HONPluginUpdateConfigFile(PluginID, Plugin, HON_PLUGIN_INFO_CERTIFICATETIME);
			HONPluginUpdateConfigFile(PluginID, Plugin, HON_PLUGIN_INFO_GUID);
			// Do not update plugin file name to avoid get wrong filename from plugin.
			//HONPluginUpdateConfigFile(PluginID, Plugin, HON_PLUGIN_INFO_FILENAME);

			HONPluginSaveConfigFile((const char *)m_ConfigurationFileName.Char);
		}

		// Configure plugin
		if(Plugin->GetRawInfo()){
			Ret = HONPluginConfigure(Plugin, PluginID);
			if(!Ret){
				Plugin->ClearPlugin();
				Plugin->SetPluginStatus(HON_PLUGIN_STATUS_CONFIG_ENTRY);
				HONPluginUpdateStatus(PluginID, "CONFIG:ENTRY");
			}

			Plugin->SetPluginStatus(HON_PLUGIN_STATUS_SUCCESS);
			HONPluginUpdateStatus(PluginID, "SUCCESS");

			Ret = Plugin->InitializePlugin();
			if(!Ret){
				Plugin->ClearPlugin();
				Plugin->SetPluginStatus(HON_PLUGIN_STATUS_TERMINATE_ABNORMAL);
			}
		}
		Plugin->AddEndPlugin(&m_pFirstPlugin, &m_pLastPlugin);

		HONPluginUpdateConfigFile(PluginID, Plugin, HON_PLUGIN_INFO_LICENSED);
		HONPluginUpdateConfigFile(PluginID, Plugin, HON_PLUGIN_INFO_ACTIVE);
		HONPluginUpdateConfigFile(PluginID, Plugin, HON_PLUGIN_INFO_LOADSTATUS);
		HONPluginSaveConfigFile((const char *)m_ConfigurationFileName.Char);

		if(Plugin->GetPluginStatus() != HON_PLUGIN_STATUS_SUCCESS)
			Err = false;
			
	}


	/* If there is no system routine added, add system routine at the end of the sequence */
	if(SysRoutineAdded == 0 && Err)
		m_pSystemDummy->AddEndPlugin(&m_pFirstPlugin, &m_pLastPlugin);

//	if(!Err)
//		delete m_pSystemDummy;

	return Err;
}



void HONPluginConfiguration :: HONPluginDumpConfigFile()
{
	TiXmlElement *RootElement, *PluginID;
	char* OutInfo;

	if(!m_pXMLConfigurationFile){
		OutInfo = "Corrupt Configuration\r\n";
		SendStringToInterface(OutInfo, strlen(OutInfo), true);
		return;
	}

		/* Check Declaration */
	if(!HONPluginConfigCheckDeclaration()){
		OutInfo = "Corrupt Configuration\r\n";
		SendStringToInterface(OutInfo, strlen(OutInfo), true);
		return;
	}

	/* Skip comments and get the plugin class type */
	RootElement = m_pXMLConfigurationFile->RootElement();

	if(!RootElement){
		OutInfo = "Corrupt Configuration\r\n";
		SendStringToInterface(OutInfo, strlen(OutInfo), true);
		return;
	}

	if(!strncmp(RootElement->Value(), "Format_PlugIn", 13)){
		OutInfo = "[Format Plugin Configuration]\r\n";
		SendStringToInterface(OutInfo, strlen(OutInfo), true);
	}else if(!strncmp(RootElement->Value(), "ImageProcessing_PlugIn", 22)){
		OutInfo = "[Image Plugin Configuration]\r\n";
		SendStringToInterface(OutInfo, strlen(OutInfo), true);
	}else if(!strncmp(RootElement->Value(), "Decode_PlugIn", 13)){
		OutInfo = "[Decode Plugin Configuration]\r\n";
		SendStringToInterface(OutInfo, strlen(OutInfo), true);
	}
	else{
		OutInfo = "Corrupt Configuration\r\n";
		SendStringToInterface(OutInfo, strlen(OutInfo), true);
		return;
	}



/* Load and configure plugins */
	for( PluginID = RootElement->FirstChildElement(); PluginID != NULL; PluginID = PluginID->NextSiblingElement())
	{
		TiXmlElement *PluginConfigItem; 
		TiXmlText *PluginConfigValueItem;
		const char *ItemName;
		const char *ItemValue;
		char buf[256];
		char buf2[512];
		
		PluginConfigItem = PluginID->FirstChildElement();

		if(!PluginConfigItem)
			continue;

		/* Identifier */
		ItemValue = PluginID->Value();

		if(!ItemValue)
			continue;

		sprintf(buf, "\t<%s>\r\n", PluginID->Value());
		SendStringToInterface(buf, strlen(buf), true);


		for(PluginConfigItem = PluginID->FirstChildElement(); PluginConfigItem; PluginConfigItem = PluginConfigItem->NextSiblingElement())
		{
			ItemName = PluginConfigItem->Value();
			if(!ItemName){
				continue;
			}

			/* Get Text value */
			PluginConfigValueItem = ElementGetText(PluginConfigItem);

			if(!PluginConfigValueItem){
				ItemValue = NULL;
			}else
				ItemValue = PluginConfigValueItem->Value();
			if(ItemValue){
				strcpy(buf, "\t\t[");
				strcat(buf, ItemName);
				strcat(buf, "]:");
				sprintf(buf2, "%-32s%-32s\r\n", buf, ItemValue);
			}else
				sprintf(buf2, "\t\t[%s]:\r\n", ItemName);
			SendStringToInterface(buf2, strlen(buf2), true);
			
		}

		SendStringToInterface("\r\n", 2, true);
	}


}


/* Debug */
void HONPluginConfiguration :: HONPluginDumpProperties()
{
	HONPlugin *Plugin;

	PLUGIN_TRACE(("Plugin Configurations:\r\n"))

	for(Plugin = m_pFirstPlugin; Plugin; Plugin = Plugin->GetNextPlugin())
	{	
		Plugin->DumpPlugin();
	}

}


TiXmlText* HONPluginConfiguration :: ElementGetText(TiXmlElement *Item)
{
	TiXmlNode *Child = NULL;
	TiXmlText *ChildText = NULL;
	for(Child = Item->FirstChild(); Child; Child = Child->NextSibling()){
		ChildText = Child->ToText();
		if(ChildText)
			return ChildText;
			
	}

	return NULL;
}

HONPluginConfiguration *g_pImagingPluginConfiguration = NULL;
HONPluginConfiguration*g_pDecodingPluginConfiguration = NULL;
HONPluginConfiguration*g_pFormattingPluginConfiguration = NULL;

HONPluginConfiguration *g_PluginConfigurations[3] ={	NULL,	NULL,	NULL};

void PluginsAPIExtend()
{

#ifdef PLUGIN_DEV
#ifndef RFBASE
	HONPluginRawInfo *PluginInfo;
	HONPlugin *Plugin;
	DataEditApi *FormatApis;
	DecodeApi *DecodeApis;
	volatile int ret=-1;

	if(GET_SETTING(EnableFormatPlugins))  //format plugin
	{
		if(g_pFormattingPluginConfiguration && g_pFormattingPluginConfiguration->GetFirstPlugin())
		{
			for(Plugin = g_pFormattingPluginConfiguration->GetFirstPlugin(); 
			Plugin; Plugin = Plugin->GetNextPlugin())
			{
				PluginInfo = Plugin->GetRawInfo();
				FormatApis = (DataEditApi*)PluginInfo->PluginApis;

				if(!PluginInfo || !FormatApis || !FormatApis->GPIO_Plugins)
					continue;

#if 1
				ret=FormatApis->RevisionNumber;
				if(ret>1)
				{
					FormatApis->GPIO_Plugins();
				}

#endif
			}

		}
	}
	else if(GET_SETTING(EnableDecodePlugins))  //decode plugin
	{
		if(g_pDecodingPluginConfiguration && g_pDecodingPluginConfiguration->GetFirstPlugin())
		{
			for(Plugin = g_pDecodingPluginConfiguration->GetFirstPlugin(); 
			Plugin; Plugin = Plugin->GetNextPlugin())
			{
				PluginInfo = Plugin->GetRawInfo();
				DecodeApis = (DecodeApi*)PluginInfo->PluginApis;

				if(!PluginInfo || !DecodeApis || !DecodeApis->GPIO_Plugins)
					continue;

#if 1
				ret=DecodeApis->RevisionNumber;
				if(ret>1)
				{
					DecodeApis->GPIO_Plugins();
				}

#endif
			}
		}
	

	}
	else if(GET_SETTING(EnableImagePlugins))   //image plugin
	{}
	
#endif
#endif

}

//=================================================================================
//                       File Modification History
//===================================================================================


