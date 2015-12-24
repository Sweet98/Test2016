//======================================================================================
// ScriptingMenuSettings.h
//======================================================================================
// $RCSfile: ScriptingMenuSettings.h $
// $Revision: 1.2 $
// $Date: 2009/02/27 15:56:19EST $
//======================================================================================

//=====================================================================================




//---------------------------------------------------------------------------
//
//  Module Interface Description:
//      This file contains the interface definition to a routine that creates
//		scripting menu settings as part of the overall reader configuration for
//		a Matrix device.  It also contains the active value declarations of
//		scripting settings.
//
//---------------------------------------------------------------------------


#ifndef ScriptingMenuSettingsH
#define ScriptingMenuSettingsH
//---------------------------------------------------------------------------


#include	"language.h"					// system global definitions




// Script configuration.
#define	SIZE_OF_SCRIPT_NAME								40
char	ActiveIQEditProScript[SIZE_OF_SCRIPT_NAME + 1];
char	ActiveMainScript[SIZE_OF_SCRIPT_NAME + 1];

#define	DEFAULT_SCRIPT_STORAGE_SPACE					32768
#define	MINIMUM_SCRIPT_STORAGE_SPACE					16384
#define	MAXIMUM_SCRIPT_STORAGE_SPACE					65536 * 2
int		TotalScriptStorageSpace;

bool	MainScriptErrorsOn;
bool	EditScriptErrorsOn;



void CreateScriptingMenuSettings( int ControlMask, UINT ScriptingCommandModifier );


#endif // ScriptingMenuSettingsH
//----------------------------------------------------------------------------




