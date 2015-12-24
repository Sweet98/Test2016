//======================================================================================
// db_NonMenuCommands.h
//======================================================================================
// $RCSfile: db_shared/db_nonmenucommands.h $
// $Revision: 1.5.2.4 $
// $Date: 2010/02/01 10:41:48EST $
//======================================================================================

//======================================================================================




#ifndef	_NON_MENU_COMMANDS_H
#define	_NON_MENU_COMMANDS_H

#include	<stdlib.h>
#include	<string.h>
#include	"language.h"


#define		MAXIMUM_SIZE_OF_NON_MENU_COMMAND		512


//********** variables ********************************************************************************


// Keep track of whether or not we're in boot code download mode.
extern int BootCodeDownloadMode;


//********** methods ********************************************************************************


// Determine compatibility of new application code and either store it to flash and reboot OR run it from RAM.
bool ProcessNewApplicationCode(const unsigned char *ApplicationCode, int SizeOfApplicationCode, bool SkipApplicationTypeCheck, bool SkipDeviceTypeCheck,
                                bool FlashTheNewCode, bool SendACKIfSuccessful = false, bool ResetAfterFlash = true );


// Determine compatibility of new boot code store it to flash if OK.
bool ProcessNewBootCode( unsigned char *BootCode, int SizeOfBootCode, bool SkipDeviceTypeCheck=false );


// The host has requested that we download new boot code.
bool DownloadBootCode( bool SkipDeviceTypeCheck );


// The host has requested that we download new applcation code.
bool DownloadApplicationCode(bool SkipHeaderCheck=false);


// The host has requested that we download new applcation code.
bool DownloadApplicationCode(bool SkipApplicationTypeCheck, bool SkipDeviceTypeCheck, bool FlashTheNewCode);


// Parse a non-menu command received from the host
void ProcessNonMenuCommands(char *CommandString, UINT CommandStringLength);


// Functions used internally by the Non Menu Commands implementation files.
void ProcessHmodemError(unsigned int HModemStatus);

#endif



