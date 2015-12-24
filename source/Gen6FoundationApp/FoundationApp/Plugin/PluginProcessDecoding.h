//======================================================================================
// PluginProcessDecoding.h
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Plugin/PluginProcessDecoding.h $
// $Revision: 1.4 $
// $Date: 2011/08/18 16:20:19EDT $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//	       This file conatins the plugin decoding processing		*
//	       routines.	
//
//---------------------------------------------------------------------------


#ifndef _PLUGIN_PROCESS_DECODING_H
#define _PLUGIN_PROCESS_DECODING_H

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "db_chrio.h"

#include "HONPluginConfiguration.h"
#include "PluginMenuProcess.h"
#include "PluginInterface.h"

#define PLUGIN_DECODER // enable plugin decoder


extern BOOL bPluginHalted;

#ifdef PLUGIN_DECODER
#define STOP_PLUGIN_DECODER bPluginHalted = 1;
#define START_PLUGIN_DECODER bPluginHalted = 0;
#define STOP_PLUGIN_DECODER_FLAG	bPluginHalted
#else
#define STOP_PLUGIN_DECODER
#define START_PLUGIN_DECODER
#define STOP_PLUGIN_DECODER_FLAG
#endif

enum HONPluginDecoderState
{
	HON_PLUGIN_DECODER_STATE_INIT = 0,
	HON_PLUGIN_DECODER_STATE_NOTMENU,
	HON_PLUGIN_DECODER_STATE_DECODEFAIL
};


#endif

extern void DecodeWithPlugins (unsigned char *img);


//=================================================================================
//                       File Modification History
//===================================================================================


