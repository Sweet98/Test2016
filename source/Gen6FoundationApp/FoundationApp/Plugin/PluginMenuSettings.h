//======================================================================================
// PluginMenuSettings.h
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Plugin/PluginMenuSettings.h $
// $Revision: 1.3 $
// $Date: 2010/03/30 10:02:21EDT $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Module Interface Description:
//      This file contains the interface definition to a routine that creates
//		Plugin menu settings as part of the overall reader configuration for
//		an Emerald device.  It also contains the active value declarations of
//		Plugin settings.
//
//---------------------------------------------------------------------------


#ifndef PLUGIN_MENUSETTINGS_H
#define PLUGIN_MENUSETTINGS_H
//---------------------------------------------------------------------------

#ifdef PLUGIN_DEV

#define PLUGIN_CLASS_IMAGE  0
#define PLUGIN_CLASS_DECODE 1
#define PLUGIN_CLASS_FORMAT 2
#define PLUGIN_CLASS_ALL 99

#define SIZEOF_PLUGIN_NAME 256

	bool EnableImagePlugins;
	bool	EnableDecodePlugins;
	bool EnableFormatPlugins;
	bool EnableOutputDebugInformations;
	char ImagePluginName[SIZEOF_PLUGIN_NAME + 1];
	char DecodePluginName[SIZEOF_PLUGIN_NAME + 1];
	char FormatPluginName[SIZEOF_PLUGIN_NAME + 1];
	int   PluginReportFile;
	

void CreatePluginMenuSettings(int ControlMask);

#endif // PLUGIN_DEV

#endif // PLUGIN_MENUSETTINGS_H
//----------------------------------------------------------------------------

//=================================================================================
//                       File Modification History
//===================================================================================
