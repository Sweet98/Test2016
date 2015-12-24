//======================================================================================
// HONPlugin.h
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Plugin/HONPlugin.h $
// $Revision: 1.4 $
// $Date: 2011/11/14 13:33:48EST $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Module Interface Description:
//    This file contains the plugin object the plugin.
//
//---------------------------------------------------------------------------

#ifndef HON_PLUGIN
#define HON_PLUGIN

#include "language.h"
#include "elf.h"
#include "stdInclude.h"
#include "db_chrio.h"
#include "uchar.h"
#include "flashdrv.h"
#include "PluginInterface.h"

#define PLUGIN_SYMBOL_LEN	(64)

class HONPlugin;


enum HONPluginMainRoutineOrder
{
	HON_PLUGIN_MAIN_ROUTINE_ORDER_BEFORE = 0,
	HON_PLUGIN_MAIN_ROUTINE_ORDER_AFTER
};

enum HONPluginEntryDataState
{
	HON_PLUGIN_ENTRY_DATA_STATE_ORIGINAL = 0,
	HON_PLUGIN_ENTRY_DATA_STATE_MODIFIED
};

enum HONPluginChainOnExit
{
	HON_PLUGIN_CHAIN_ON_EXIT_CHAINALWAYS = 0,
	HON_PLUGIN_CHAIN_ON_EXIT_CHAINIFSUCCESS ,
	HON_PLUGIN_CHAIN_ON_EXIT_CHAINIFFAILURE ,
	HON_PLUGIN_CHAIN_ON_EXIT_CHAINNEVER
};


enum HONPluginLoadStatus
{
	HON_PLUGIN_STATUS_UNKNOWN = 0,
	HON_PLUGIN_STATUS_SUCCESS,
	HON_PLUGIN_STATUS_INACTIVE,
	HON_PLUGIN_STATUS_UNLICENSED_THISPLUGIN,
       HON_PLUGIN_STATUS_UNLICENSED_ALLPLUGINS,
	HON_PLUGIN_STATUS_MISSING_FILENOTFOUND,
	HON_PLUGIN_STATUS_MISSING_MAINNOTFOUND,
	HON_PLUGIN_STATUS_MISSING_HELPERNOTFOUND,
	HON_PLUGIN_STATUS_NORESOURCE_MALLOC,
	HON_PLUGIN_STATUS_NORESOURCE_NEW,
	HON_PLUGIN_STATUS_CORRUPT_SYMBOLREFERENC,
	HON_PLUGIN_STATUS_CORRUPT_RELOCATIONFAIL,
	HON_PLUGIN_STATUS_CORRUPT_RELOCATIONRANGE,
	HON_PLUGIN_STATUS_CORRUPT_RELOCATIONUNKNOWN,
	HON_PLUGIN_STATUS_CONFIG_ENTRY,
	HON_PLUGIN_STATUS_CONFIG_FILE,
	HON_PLUGIN_STATUS_CONFIG_NOPLUGIN,
	HON_PLUGIN_STATUS_TERMINATE_ABNORMAL,
};


/*	HON plugin class. Describes the features and executive parameters
	of the plugin
*/
class HONPlugin
{

public:
	// Constructor
	HONPlugin();

	HONPlugin(const char *FileName);

	// Destructor
	~HONPlugin();

	// Load Plugin
	bool LoadPlugin();

	bool LoadPlugin(const char *FileName);

	// Remove Plugin
	void RemovePlugin(HONPlugin **FirstPlugin, HONPlugin **LastPlugin);

	// Initialize Plugin
	bool InitializePlugin();

	// Cleanup Plugin stuff when exit
	void ExitPlugin();

	void ClearPlugin();
	bool ReloadPlugin(const char *FileName);
	bool ConfigurePlugin();

	bool AddEndPlugin(HONPlugin **FirstPlugin, HONPlugin **LastPlugin);
	HONPlugin *GetNextPlugin() {return this->Next;};
	HONPlugin *GetPreviousPlugin() {return this->Prev;};
	HONPlugin *GetMainRoutineNext() {return this->MainRoutineNext;};
	HONPlugin *GetBarcodeProcessingNext() {return this->BarcodeProcessingNext;};

	HONPluginRawInfo *GetRawInfo() {return this->RawInfo;};

	void SetNextPlugin(HONPlugin *Plugin) {this->Next = Plugin;};
	void SetPreviousePlugin(HONPlugin *Plugin) {this->Prev = Plugin;};
	void SetMainRoutineNext(HONPlugin *Plugin) {this->MainRoutineNext = Plugin;};
	void SetBarcodeProcessingNext(HONPlugin *Plugin) {this->BarcodeProcessingNext = Plugin;};

	/* Parameter operations */
	
	bool IsInterceptMode() {return this->BarcodeInterceptMode;};
	bool IsPluginLicensed() {return this->PluginLicensed;};
	bool IsPluginActive() {return this->PluginActive;};

	void SetInterceptMode(bool Value) {this->BarcodeInterceptMode = Value;};
	void SetPluginLicensed(bool Value) {this->PluginLicensed = Value;};
	void SetPluginActive(bool Value) {this->PluginActive = Value;};
	
	HONPluginMainRoutineOrder GetMainRoutineOrder() {return MainRoutineOrder;};
	HONPluginEntryDataState GetEntryDataState() {return EntryDataState;};
	HONPluginChainOnExit GetChainOnExit() {return ChainOnExit;};

	void SetMainRoutineOrder(HONPluginMainRoutineOrder Value) {MainRoutineOrder = Value;};
	void SetEntryDataState(HONPluginEntryDataState Value) {EntryDataState = Value;};
	void SetChainOnExit(HONPluginChainOnExit Value) {ChainOnExit = Value;};

	const char* GetPluginIdentifier() {return this->PluginIdentifier;};
	const char* GetPluginName() {return this->PluginName;};
	const char* GetPluginCompanyName() {return this->PluginCompanyName;};
	const char* GetPluginMajorRevision() {return this->PluginMajorRevision;};
	const char* GetPluginMinorRevision() {return this->PluginMinorRevision;};
	const char* GetPluginBuild() {return this->PluginBuild;};
	const char* GetPluginCertificate() {return this->PluginCertificate;};
	const char* GetPluginCertificateTime() {return this->PluginCertificateTime;};
	const char* GetPluginGUID() {return this->PluginGUID;};
	const char* GetPluginFileName() {return this->PluginFileName;};

	HONPluginLoadStatus GetPluginStatus() {return this->PluginStatus;};
	
	void SetPluginIdentifier(const char *Value) {this->PluginIdentifier = Value;};
	void SetPluginName(const char *Value) {this->PluginName = Value;};
	void SetPluginCompanyName(const char *Value) { this->PluginCompanyName = Value;};
	void SetPluginMajorRevision(const char *Value) { this->PluginMajorRevision = Value;};
	void SetPluginMinorRevision(const char *Value) {this->PluginMinorRevision = Value;};
	void SetPluginBuild(const char *Value) {this->PluginBuild = Value;};
	void SetPluginCertificate(const char *Value) {this->PluginCertificate = Value;};
	void SetPluginCertificateTime(const char *Value) {this->PluginCertificateTime = Value;};
	void SetPluginGUID(const char *Value) {this->PluginGUID = Value;};
	void SetPluginFileName(const char *Value) {this->PluginFileName = Value;};

	 void SetPluginStatus(HONPluginLoadStatus Status) {this->PluginStatus = Status;};


	bool GetMenuLock() {return this->IsMenuLocked;};
	void SetMenuLock(bool MenuLock) {this->IsMenuLocked = MenuLock;};

	int GetMenuID() {return RawInfo->MenuIdentifier;};

	void DumpPlugin();


public:


	typedef struct PluginSymbol{
		unsigned long Value;
		//char Name[PLUGIN_SYMBOL_LEN];
		char *Name;
	} HONPluginSymbol;


private:
	bool PluginSanityCheck();
	bool LoadSections();
	void LayoutSections();
	bool SimplifySymbols();
	bool DoRelocation();
	bool VerifyExportSymbols();
	unsigned int FindSection(ElfHeader *Header, ElfSectionHeader *SectionHeaders, const char *NameStrings, const char *SectionName);
	long SectionOffset(unsigned long *Size, ElfSectionHeader *SectionHeader);
	bool AddInternalSymbol(unsigned long SymbolValue, char *SymbolName);
	void DeleteInternalSymbol(const char *Name);
	const HONPluginSymbol* LookupInternalSymbol(const char *Name);	
	const HONPluginSymbol* LookupExternalSymbol(const char *Name, const HONPlugin::HONPluginSymbol *Start, const HONPlugin::HONPluginSymbol *Stop);
	unsigned long FindSymbol(const char *Name, HONPlugin **Owner);
	bool ApplyRelocate(ElfSectionHeader *SectionHeaders, const char *StringTable, unsigned int SymbolIndex, unsigned int RelocateIndex, HONPlugin *plugin);
	bool ApplyRelocateAdd(ElfSectionHeader *SectionHeaders, const char *StringTable, unsigned int SymbolIndex, unsigned int RelocateIndex, HONPlugin *plugin);


	/**************************************************/


	
private:
	// Plugin raw data from plugin binary
	HONPluginRawInfo *RawInfo;

	// Plugin Class Type
	HONPluginClassType ClassType;

	/* Unique handle for this plugin in configuration */
	const char *PluginIdentifier;

	/* Plugin Name */
	const char *PluginName;

	/* Plugin Company Name */
	const char *PluginCompanyName;

	/* Plugin License Feature */
	bool PluginLicensed;

	/* Plugin Active */
	bool PluginActive;

	/* Plugin Major Revision string */
	const char *PluginMajorRevision;

	// Minor Revision string
	const char *PluginMinorRevision;

	// Plugin build number string
	const char *PluginBuild;

	// Certificate number string
	const char *PluginCertificate;

	// Certificate time string
	const char *PluginCertificateTime;

	// GUID
	const char *PluginGUID;

	// Plugin binary file name
	const char *PluginFileName;

	// Plugin main routine order
	HONPluginMainRoutineOrder MainRoutineOrder;

	// Barcode intercept mode
	bool BarcodeInterceptMode;

	// Entry data state
	HONPluginEntryDataState EntryDataState;

	// Chain on exit
	HONPluginChainOnExit ChainOnExit;

	// Exported symbols by this plugin. This member is not used now but may be used in the future
	HONPluginSymbol *ExportedSyms;

	// Number of the exported symbols by this plugin
	unsigned int ExportedSymsNum;

	/* Init section. If this is non-NULL, free memory after init() returns */
	void *PluginInitCode;

	/* Here is the actual code + data, free'd on unload. */
	void *PluginCoreCode;

	/* Here are the sizes of the init and core sections */
	unsigned long InitCodeSize, CoreCodeSize;

	/* The size of the executable code in each section.  */
	unsigned long InitTextSize, CoreTextSize;

	/* Am I unsafe to unload? */
	int Unsafe;

	/* Is the plug-in hold the programming mode lock? */
	int IsMenuLocked;

	// Plugin raw data from plugin file
	unsigned char *PluginRawData;

	unsigned long RawDataLen;

	// Plugin Status
	HONPluginLoadStatus PluginStatus;

	/* Previous and next obj in elf obj list */
	HONPlugin *Next, *Prev;

	/* Main routine next */
	HONPlugin *MainRoutineNext, *BarcodeProcessingNext;

	/* ELF stuff */
	ElfHeader			*PluginElfHeader;
	ElfSectionHeader	*PluginElfSectionHeaders;
	char 			*SectionNameStrings;
	char				*StringTable;
	unsigned int		SymbolTableIndex;
	unsigned int		StringTableIndex;
	unsigned int		PluginInfoIndex;
	unsigned int		ExportFunctionIndex;
	
private:
	HONPlugin (const HONPlugin&);            // declared private, but not defined, to prevent copying         
	HONPlugin& operator=(const HONPlugin&);  // declared private, but not defined, to prevent copying 
	
};
#endif

//=================================================================================
//                       File Modification History
//===================================================================================

