#ifndef MENUFLASH_H_
#define MENUFLASH_H_
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/menu/MenuFlash.h $
// $Revision: 1.9 $
// $Date: 2010/04/23 10:05:01EDT $
//======================================================================================

//======================================================================================
#include "hstring.h"

#define FILENAME_PRODUCT_CONFIG	"ProductConfig"

#define FILENAME_WORKING_CONFIG		"WorkingConfig"  // RFSCAN has a single file
#define FILENAME_WORKING_CONFIG_0	"WorkingConfig0"
#define FILENAME_WORKING_CONFIG_1	"WorkingConfig1"

#define FILENAME_DEFAULT_CONFIG		"DefaultConfig"

bool StoreWorkingConfigurationToFlash(unsigned char *WorkingConfigurationAddress, int SizeOfWorkingConfiguration);
bool StoreDefaultConfigurationToFlash(unsigned char *DefaultConfigurationAddress, int SizeOfDefaultConfiguration, bool WriteBackupDefaultConfiguration);
bool StoreProductConfigurationToFlash(bool);
bool ReadProductConfigurationFromFlash(HSTRING * pProdConfigData);
void FlashAndReSynchCustomerDefaults(void);
bool DeleteDefaultConfigurationFromFlash(void);
void FlashWorkingChanges(void);
void FlashProductConfigurationChanges( bool bOverRidePrivilege = false );
void SaveSettingChangeToTableZero(void);
void SaveSettingChangeToTableOne(void);

bool OverWriteProductConfiguration(const char * pStr, int StrLength);
bool OverWriteCustomDefaultConfiguration(const char * pStr, int StrLength);
bool OverWriteCurrentWorkingConfiguration(const char * pStr, int StrLength);



#endif /*MENUFLASH_H_*/
