//======================================================================================
// PluginMenuProcess.h
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Plugin/PluginMenuProcess.h $
// $Revision: 1.6 $
// $Date: 2011/06/17 04:18:36EDT $
//======================================================================================

//======================================================================================

#ifndef PLUGIN_MENU_PROCESS
#define PLUGIN_MENU_PROCESS


#ifdef PLUGIN_DEV

#include "HONPluginConfiguration.h"

extern void PluginSerialMenuCommandProcess(const char *pucBuffer, int  Size,  HSTRING *MnuCommand);
extern void PluginSerialMenuCommandProcess_Ext(HSTRING *MnuCommand);
extern bool MultiPluginMenuBarcodesProcess(HSTRING *PluginMenuCommand, int MenuCount);
extern bool PluginBarcodeProcess(const char *Data, int DataLen, int MenuID, HONPluginClassType Type);
extern bool PluginCommandProcess(const char *Data, int DataLen, int MenuID, HONPluginClassType Type);
extern int AbstractPluginMenuCommands(HSTRING *PluginMnuCommand);
extern int AbstractSystemMenuCommands(HSTRING *SystemPluginMenuCommand);


/**************************************************************************************
 * IsPluginMenuEnterCode
 * called to validate the decode result is a Plugin menuing enter code or not
 * Format of enter code is:
 *			990XYYEnt
 * 
 * bool  IsPluginMenuEnterCode: 
 *			return true if this is a enter code.
 *			return false if not a enter code
 *			The identifier is defined as 2 digits Hex for the time being
 **************************************************************************************/
extern bool IsPluginMenuEnterCode(const char *Data, int DataLen, int &MenuID, HONPluginClassType &Type);



/**************************************************************************************
 * IsPluginMenuExitCode
 * called to validate the decode result is a Plugin menuing exit code or not
 * Format of exit code is:
 *			99Exit
 * 
 * static int  IsPluginMenuEnterCode: 
 *			return 0 if not a exit code and 1 if a exit code.
 **************************************************************************************/
extern bool IsPluginMenuExitCode(const char *Data, int DataLen);

extern bool IsNormalPluginMenuCode(const char *Data, int DataLen, int &MenuID, HONPluginClassType &Type);
extern bool IsLegacyPluginMenuCode(const char *Data, int DataLen, HONPluginClassType &Type);

extern bool LockPlugin(int MenuID, HONPluginClassType Type);



extern void UnlockPlugin(HONPlugin *Plugin);



extern HONPlugin* SearchLockedPlugin();

#endif


#endif




