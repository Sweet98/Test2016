/************************************************************************/
//
// HHP 4480 embedded firmware
// Flash programming definitions and interfaces
//
//  Date:       3/12/02
//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Flash/flashapplicationcode.h $
// $Revision: 1.4 $
// $Date: 2010/04/09 15:35:51EDT $
//======================================================================================
#ifndef __FLASH_APPLICATION_CODE_H         /*  prevent multiple definitions    */
#define __FLASH_APPLICATION_CODE_H     1



#include "language.h"
#include "HString.h"


#define FILENAME_MAIN_APP	"mainAPP"
#define FILENAME_BOOT_UPDATE	"BOOTupdate"


// Write new application code to flash.
	bool StoreApplicationCodeToFlash(const UCHAR *ApplicationCodeAddress, int SizeOfApplicationCode, bool RunFromROM,
						HSTRING *UpdateFileContents = NULL);

#endif  /*  __FLASH_APPLICATION_CODE_H */
/*  END */



