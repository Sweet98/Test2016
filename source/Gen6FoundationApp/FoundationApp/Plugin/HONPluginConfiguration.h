//======================================================================================
// HONPluginConfigure.h
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Plugin/HONPluginConfiguration.h $
// $Revision: 1.16 $
// $Date: 2011/11/04 15:39:50EDT $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Module Interface Description:
//    This file contains the plugin configuration object the plugin.
//
//---------------------------------------------------------------------------

#ifndef HON_PLUGIN_CONFIGURATION
#define HON_PLUGIN_CONFIGURATION


#include "tinyxml.h"
#include "thread.h"
#include "HONPlugin.h"
#include "ioman.h"

#include <stdio.h>
#include <stdlib.h>
#include "Uchar.h"
#include "Menu.h"
#include "ReaderConfiguration.h"
#include "ProductConfiguration.h"


#ifndef RFSCAN
extern void SendStringToInterface( const char *pucBuffer, int SizeOfBuffer, bool /* DoNotUseIndicators */ );
#else
extern void SendStringToInterface( const char *pucBuffer, int SizeOfBuffer, bool DoNotUseIndicators );
#endif ///TODO:  this function needs to be in a header file--and include it

#if !defined(RFBASE) && !defined(RFSCAN)
#define PLUGIN_TRACE(a)	\
	if(GET_SETTING(EnableOutputDebugInformations)) 	\
	{	\
		theStdInterface.printf("[PLGUIN_DEBUG] ");	\
		theStdInterface.printf a;	\
	}
#else
#define PLUGIN_TRACE(a)	PLUGIN_TRACE_S##a

#define PLUGIN_TRACE_S(...)	\
	if(GET_SETTING(EnableOutputDebugInformations)) 	\
	{	\
		char PLUGIN_TRACE_BUFFER[1024];	\
		sprintf(PLUGIN_TRACE_BUFFER, __VA_ARGS__);	\
		SendStringToInterface("[PLGUIN_DEBUG] ", 15, true);		\
		SendStringToInterface(PLUGIN_TRACE_BUFFER, strlen(PLUGIN_TRACE_BUFFER), true);		\
	}
#endif

		//SendStringToInterface(buf);	\
#define PLUGIN_DONWLOAD_OSPV



enum HONPluginInfo{
	HON_PLUGIN_INFO_NAME,
	HON_PLUGIN_INFO_COMPANY,
	HON_PLUGIN_INFO_MAJOR,
	HON_PLUGIN_INFO_MINOR,
	HON_PLUGIN_INFO_BUILD,
	HON_PLUGIN_INFO_LICENSED,
	HON_PLUGIN_INFO_ACTIVE,
	HON_PLUGIN_INFO_CERTIFICATE,
	HON_PLUGIN_INFO_CERTIFICATETIME,
	HON_PLUGIN_INFO_GUID,
	HON_PLUGIN_INFO_FILENAME,
	HON_PLUGIN_INFO_LOADSTATUS
};

class HONPluginConfiguration;


/*	HON plug-in configuration class. Inheritance of TiXmlDocument
	The configuration contain the information that plug-ins needed
	to run on HON product.
*/

class HONPluginConfiguration
{
	friend class TiXmlDocument;
	
public:
	//Create a HON plugin configuration instance
	HONPluginConfiguration();

	// Destructor
	~HONPluginConfiguration();

	static void Create(void);
	static void LoadAllPlugins(void);
	static void LoadFormatPlugins(HSTRING *ConfigFileName);
	static void LoadDecodePlugins(HSTRING *ConfigFileName);
	static void RemoveAllPlugins(void);
	static void RemoveFormatPlugins(void);
	static void RemoveDecodePlugins(void);
	
	// Load configuration file
	bool HONPluginLoadConfigFile(void);
	bool HONPluginLoadConfigFile(HSTRING *FileName);
#if 0		
	// Load configuration file with the giving name
	bool HONPluginLoadConfigFile(const char * filename);
#endif

	// Save configuration file
	bool HONPluginSaveConfigFile();
	
	// Save configuration file with the giving name
	bool HONPluginSaveConfigFile(const char * filename);

	// Update configuration file from plug-in
	bool HONPluginUpdateConfigFile(TiXmlElement *PluginID, HONPlugin *Plugin, HONPluginInfo InfoIndex);
	void HONPluginUpdateStatus(TiXmlElement *PluginID, const char *Status);


	bool VerifyPluginIDUniqueness(TiXmlElement *PluginID);

	// Create plugin to executive sequence item
	HONPlugin *HONPluginLoad(const char * filename);

	// Remove plugin from executive sequence item
	bool HONPluginRemove(const char * filename);

	// Configure a plugin
	bool HONPluginConfigure(HONPlugin *Plugin, TiXmlNode *PluginConfigure);

	// Find Plugin in executive list
	HONPlugin *HONPluginSearch(const char *Pluginname);

	// Excution of the sequnce
	bool HONPluginExecuteSequence();

	// Create plugin loading sequance
	bool HONPluginCreateLoadingSequence();

	// Destroy plugin loading sequance
	void HONPluginDestroyLoadingSequence();

	// Create plugin main routine sequance
	bool HONPluginCreateMainRoutineSequence();

	// Create plugin barcode processing sequance
	bool HONPluginCreateBarcodeProcessingSequence();

		// Create Mutex for Plugin
	bool HONPluginCreateMutex();

	// Close Mutex for Plugin
	void HONPluginCloseMutex();

	// Wait Mutex
	bool HONPluginWaitMutex();

	// Release Mutex
	void HONPluginReleaseMutex();

	// Expose plugin functions for plugin developers
	bool HONExposeFunction();

	// Check declaration
	bool HONPluginConfigCheckDeclaration();

	void HONPluginDumpConfigFile();
	void HONPluginDumpProperties();

	HONPlugin *GetFirstPlugin() {return this->m_pFirstPlugin;};

	HONPlugin::HONPluginSymbol *GetSystemExportedSym() {return this->m_pSystemExportedSyms;};

	HONPlugin *GetMainRoutineSequence() {return this->m_pMainRoutineExectiveSequence;};

	void SetMainRoutineSequence(HONPlugin *Plugin) {this->m_pMainRoutineExectiveSequence = Plugin;};

	HONPlugin *GetBarcodeProcessSequence() {return this->m_pBarcodeProcessingSequence;};

	void SetBarcodeProcessSequence(HONPlugin *Plugin) {this->m_pBarcodeProcessingSequence = Plugin;};

	HONPlugin *GetSystemDummy() {return this->m_pSystemDummy;};

	TiXmlDocument *GetConfigFile(){return this->m_pXMLConfigurationFile;};

	void DeleteConfigFile() {if(this->m_pXMLConfigurationFile) delete this->m_pXMLConfigurationFile;this->m_pXMLConfigurationFile = NULL;};

	void SetConfigFile(TiXmlDocument *ConfigFile){this->m_pXMLConfigurationFile = ConfigFile;};

	// Helpler to get XML element first text child
	TiXmlText *ElementGetText(TiXmlElement *Item);
	
	/* The following functions will be exposed to plugin developers
		in order that they could interact with system firmware to
		handle non-std lib operations by using these functions.
	*/

	
	virtual bool GetAmIEnabledSetting(void) = 0;

protected:
	// Configuration file name
	HSTRING m_ConfigurationFileName;

	//Configuration XML file object
	TiXmlDocument *m_pXMLConfigurationFile;
	
	// Configuration Class
	HONPluginClassType m_ClassType;

	// We treat the system routine and barcode process routine as normal plugins
	HONPlugin *m_pSystemDummy;
	
	// Plugin main routine invoke sequence of this class configuration
	HONPlugin *m_pMainRoutineExectiveSequence;

	// Plugin barcode processing sequence of this class configuration
	HONPlugin *m_pBarcodeProcessingSequence;

	// Loading sequence from configuration file
	HONPlugin *m_pFirstPlugin, *m_pLastPlugin;

	// Plugin Configuration Mutex
	MT_Mutex_t m_PluginConfigurationMutex;

	// System main routine chain parameter
	HONPluginChainOnExit m_SystemRoutineChainOnExit;

	HONPlugin::HONPluginSymbol *m_pSystemExportedSyms;


	virtual const char * GetMyConfigFileNameSetting(void) = 0;
	// Customer registered menu settings

private:
	HONPluginConfiguration (const HONPluginConfiguration&);            // declared private, but not defined, to prevent copying         
	HONPluginConfiguration& operator=(const HONPluginConfiguration&);  // declared private, but not defined, to prevent copying 
	
};


class DecodePlugingConfiguration : public HONPluginConfiguration
{
public:
	virtual const char * GetMyConfigFileNameSetting(void) {return GET_SETTING(DecodePluginName);}
	
protected:
	virtual bool GetAmIEnabledSetting(void) {return GET_SETTING(EnableDecodePlugins);}
};


class FormattingPlugingConfiguration : public HONPluginConfiguration
{
public:
	virtual const char * GetMyConfigFileNameSetting(void) {return GET_SETTING(FormatPluginName);}
	
protected:
	virtual bool GetAmIEnabledSetting(void) {return GET_SETTING(EnableFormatPlugins);}
};


class ImagingPlugingConfiguration : public HONPluginConfiguration
{
public:
	virtual const char * GetMyConfigFileNameSetting(void) {return GET_SETTING(ImagePluginName);}
	
protected:
	virtual bool GetAmIEnabledSetting(void) {return GET_SETTING(EnableImagePlugins);}
};


extern HONPluginConfiguration *g_pImagingPluginConfiguration;
extern HONPluginConfiguration *g_pDecodingPluginConfiguration;
extern HONPluginConfiguration *g_pFormattingPluginConfiguration;

extern HONPluginConfiguration *g_PluginConfigurations[3];

extern void PluginsAPIExtend(void);


#endif

//=================================================================================
//                       File Modification History
//===================================================================================

