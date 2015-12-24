/*===================================================================
  Overview:
  MOCF is implemented with three "types" of classes:  Creator, Parser,
  and Handler.  The Creator classes are used to construct the MOCF
  and the Records it contains.  The Parser classes are used to read
  an MOCF.  The Handler classes are used to write Object Records
  inside the MOCF.
  
 This module contains all the Object Handler classes.  For now we're
 putting all classes here (they're expected to be small).  If the file
 gets too large, we should break this up into separate modules per
 class.
 
 To add a new Object Record Handler, you create the class and then
 add the creation of it to CreateObjectRecordHandler().
=====================================================================
   $Source: Gen6FoundationApp/FoundationApp/mocf/Gen6ObjectHandler.cpp $
   $Date: 2011/01/18 15:44:39EST $
   $Revision: 1.14 $


===================================================================*/
#include <assert.h>
#include <string.h>
#include "Gen6ObjectHandler.h"
#include "Gen6ObjectParser.h"
#include "MocfTags.h"

// includes for system functions & classes
#include "flashapplicationcode.h"
#include "hab_header.h"
#include "MenuFlash.h"
#include "StartupFlashDisk.h"
#include "FileHandler.h"
#include "ReaderConfiguration.h"
#include "db_hdw.h"
#include "ProjectRevision.h" ///** temporary--remove when we flip to "full MOCF"

//#define INC_MOCF_DEBUG // uncomment this to include debug prints to std interface



#ifdef INC_MOCF_DEBUG
#include "ioman.h"
// FYI:  the "## in the macro expansion handles the case MOCF_DEBUG("only text, no variables")
#define MOCF_DEBUG(format, ...) theStdInterface.printf(format, ## __VA_ARGS__)
#else
#define MOCF_DEBUG(format, ...)
#endif



/*----------------------------------------------------------------------------------------
CreateObjectRecordHandler

  Creates a handler for an object inside an MOCF.  If an Object is unknown, it will
  return a handler for that as well.

  Parameters:  
  			pObject[in]:  pointer to an Object

  Returns:  an Object Handler.
----------------------------------------------------------------------------------------*/
CGen6ObjectRecordHandler * CGen6ObjectRecordHandler::CreateObjectRecordHandler(const CTlv::Header_t * pObject)
{
	CGen6ObjectRecordHandler * pHandler;
	switch ( pObject->Tag )
	{
		case Gen6::tagAppFirmware:
			pHandler = new CAppFirmwareHandler(pObject);
		break;
		
		case Gen6::tagBootFirmware:
			pHandler = new CBootFirmwareHandler(pObject);
		break;
		
		case Gen6::tagUserFile:
			pHandler = new CUserFileHandler(pObject);
		break;
		
		case Gen6::tagSoftwareKeys:
			pHandler = new CSwKeysHandler(pObject);
		break;
		
		case Gen6::tagCustomDefMenuSettings:
			pHandler = new CCustomDefMenuSettingsHandler(pObject);
		break;
		
		case Gen6::tagExecuteMenuSettings:
			pHandler = new CExecuteMenuSettingsHandler(pObject);
		break;
		
		case Gen6::tagMenuSettings:
			pHandler = new CMenuSettingsHandler(pObject);
		break;
		
		default:
			pHandler = new CUnknownHandler(pObject);
	}
	
	return pHandler;
}




/*----------------------------------------------------------------------------------------
StoreObject

  This stores the Object.  We store the entire fw object.  True, we don't really need
  the Compatible Products Record, but storing the fw object gives the bootloader the length 
  and a Tag which assists the bootloader in verifying the validity of the file.  Plus,
  giving boot the ability to read the Compatible Products Record could come in handy
  some day.
  
  
  Parameters:  none

  Returns:  true if the object is stored OK, false otherwise.
----------------------------------------------------------------------------------------*/
bool CAppFirmwareHandler::StoreObject(void)
{
	assert(m_pObject);
	bool bIsOk;
	DWORD FwLength;
	
	FwLength = sizeof(CTlv::Header_t) + CTlv::GetLength(m_pObject);  // (Tag + Length) + ObjectData
	bIsOk = StoreApplicationCodeToFlash((const unsigned char *) m_pObject, FwLength, true);
	
	MOCF_DEBUG("Storing App Fw returned %d\n", bIsOk);
	return bIsOk;
}


/*----------------------------------------------------------------------------------------
GetSpaceNeededToStore

 
  Parameters:  none

  Returns:  Number of extra bytes to store the object--could be < 0 if the new object
  			is smaller than the current one
----------------------------------------------------------------------------------------*/
int CAppFirmwareHandler::GetSpaceNeededToStore(void)
{
	int NewLength;
	int SpaceNeeded;
	
	
	int CurrentLength = GetFileLength(FILENAME_MAIN_APP);
	if ( CurrentLength < 0 ) CurrentLength = 0;  // <0 means error or no file--assume no file
	NewLength = sizeof(CTlv::Header_t) + CTlv::GetLength(m_pObject);  // We store the Object Record: (Tag + Length) + ObjectData
	SpaceNeeded = NewLength - CurrentLength;
	MOCF_DEBUG("GetSpaceNeededToStore App Fw returned %d\n", SpaceNeeded);
	return SpaceNeeded;
}



/*----------------------------------------------------------------------------------------
StoreObject

  This stores the Object.

  Parameters:  none

  Returns:  true if the object is stored OK, false otherwise.
----------------------------------------------------------------------------------------*/
bool CBootFirmwareHandler::StoreObject(void)
{
	assert(m_pObject);
	
	
	CFirmwareParser FwParser(m_pObject);
	const unsigned char * pFirmwareBin;
	DWORD FwLength;
	bool bIsOk;
	
	if ( ! FwParser.GetFwData(&pFirmwareBin, &FwLength) ) return false;
	// Some explanation:  SpiFlashBootCode() eventually calls SpiXfr().  SpiXfr() can do reads & writes, so the pointer to the data cannot be a const *.  Rather
	// than re-factor that module I decided to just cast a non const pointer. 
	if ( SpiFlashBootCode((unsigned char *) pFirmwareBin, FwLength) == 0 ) bIsOk = true;
	else bIsOk = false;
	
	MOCF_DEBUG("Storing Boot Fw returned %d\n", bIsOk);
	return bIsOk;
}


/*----------------------------------------------------------------------------------------
GetSpaceNeededToStore

  This one is unique.  It doesn't use the filesystem, so what we do is return 0 if there's 
  room.  If there's no room, we return a very large number.

  Parameters:  none

  Returns:  0 or a very large number
----------------------------------------------------------------------------------------*/
int CBootFirmwareHandler::GetSpaceNeededToStore(void)
{
	assert(m_pObject);
	const unsigned char * pFirmwareBin;
	DWORD FwLength;
	int SpaceNeeded;
	CFirmwareParser FwParser(m_pObject);
	
	FwParser.GetFwData(&pFirmwareBin, &FwLength);
	if ( FwLength <= FLASH_MAX_BOOT_CODE_LENGTH ) SpaceNeeded = 0;
	else SpaceNeeded = 1000000000;  // we don't want to use the max int macro because the function calling this will be adding to it & we don't want to roll over
	MOCF_DEBUG("GetSpaceNeededToStore Boot Fw returned %d\n", SpaceNeeded);
	return SpaceNeeded;
}



/*----------------------------------------------------------------------------------------
StoreObject

  This stores the Object by executing the Menu String.

  Parameters:  none

  Returns:  true if the object is stored OK, false otherwise.
----------------------------------------------------------------------------------------*/
bool CExecuteMenuSettingsHandler::StoreObject(void)
{
	assert(m_pObject);
	bool bIsOk;
	
	CMenuSettingsParser parser(m_pObject);
	HSTRING MenuStr(parser.GetLength() + 1);  // create empty HSTRING with max size of string + 1 (+1 for potentially adding terminator below)
	MenuStr.Copy(parser.GetData());
	if ( ! ReaderConfiguration->IsTerminatorValid(MenuStr.Char[MenuStr.Size - 1]) ) MenuStr.AppendChar('.');
	bIsOk = (bool) ReaderConfiguration->Menu(&MenuStr);
	if ( bIsOk )
	{	// We're gonna reset after we're done processing the MOCF, so we better save things.  TODO:  maybe we should put this in another spot--perhaps even a system wide (ResetDevice() that does this & other cleanup)
		FlashAndReSynchCustomerDefaults();
		FlashWorkingChanges();
	}
	
	MOCF_DEBUG("Executing Menu Settings returned %d\n", bIsOk);
	return bIsOk;
}


/*----------------------------------------------------------------------------------------
StoreObject

  This stores the Object.

  Parameters:  none

  Returns:  true if the object is stored OK, false otherwise.
----------------------------------------------------------------------------------------*/
bool CMenuSettingsHandler::StoreObject(void)
{
	assert(m_pObject);
	bool bIsOk;
	
	CMenuSettingsParser parser(m_pObject);
	bIsOk =  OverWriteCurrentWorkingConfiguration(parser.GetData(), parser.GetLength() - 1);
	
	MOCF_DEBUG("Storing Menu Settings returned %d\n", bIsOk);
	return bIsOk;
}


/*----------------------------------------------------------------------------------------
GetSpaceNeededToStore

  This calculates the extra space needed to store an object in our file system.
 
  Parameters:  none

  Returns:  Number of extra bytes to store the object--could be < 0 if the new object
  			is smaller than the current one
----------------------------------------------------------------------------------------*/
int CMenuSettingsHandler::GetSpaceNeededToStore(void)
{
	assert(m_pObject);
	int SpaceNeeded;
	CMenuSettingsParser parser(m_pObject);
	const char * pWhichConfig;
	
	if ( ReaderConfiguration->GetActiveWorktableNumber() ) pWhichConfig = FILENAME_WORKING_CONFIG_1;
	else pWhichConfig = FILENAME_WORKING_CONFIG_0;

	int CurrentLength = GetFileLength(pWhichConfig);
	if ( CurrentLength < 0 ) CurrentLength = 0;  // <0 means error or no file--assume no file
	int NewLength = parser.GetLength() - 1;
	SpaceNeeded = NewLength - CurrentLength;
	MOCF_DEBUG("GetSpaceNeededToStore Menu Settings returned %d\n", SpaceNeeded);
	return SpaceNeeded;
}




/*----------------------------------------------------------------------------------------
StoreObject

  This stores the Object.

  Parameters:  none

  Returns:  true if the object is stored OK, false otherwise.
----------------------------------------------------------------------------------------*/
bool CCustomDefMenuSettingsHandler::StoreObject(void)
{
	assert(m_pObject);
	bool bIsOk;
	
	CCustomDefMenuSettingsParser parser(m_pObject);
	bIsOk =  OverWriteCustomDefaultConfiguration(parser.GetData(), parser.GetLength() - 1);
	
	MOCF_DEBUG("Storing Custom Def Menu Settings returned %d\n", bIsOk);
	return bIsOk;
}


/*----------------------------------------------------------------------------------------
GetSpaceNeededToStore

  This calculates the extra space needed to store an object in our file system.
 
  Parameters:  none

  Returns:  Number of extra bytes to store the object--could be < 0 if the new object
  			is smaller than the current one
----------------------------------------------------------------------------------------*/
int CCustomDefMenuSettingsHandler::GetSpaceNeededToStore(void)
{
	assert(m_pObject);
	int SpaceNeeded;
	CMenuSettingsParser parser(m_pObject);
	
	int CurrentLength = GetFileLength(FILENAME_DEFAULT_CONFIG);
	if ( CurrentLength < 0 ) CurrentLength = 0;  // <0 means error or no file--assume no file
	int NewLength = parser.GetLength() - 1;
	SpaceNeeded = NewLength - CurrentLength;
	MOCF_DEBUG("GetSpaceNeededToStore Custom Default Menu Settings returned %d\n", SpaceNeeded);
	return SpaceNeeded;
}





/*----------------------------------------------------------------------------------------
StoreObject

  This stores the Object.

  Parameters:  none

  Returns:  true if the object is stored OK, false otherwise.
----------------------------------------------------------------------------------------*/
bool CSwKeysHandler::StoreObject(void)
{
	assert(m_pObject);
	bool bIsOk;
	
	CSwKeysParser parser(m_pObject);
	bIsOk =  OverWriteProductConfiguration(parser.GetData(), parser.GetLength() - 1);
	
	MOCF_DEBUG("Storing SW Keys returned %d\n", bIsOk);
	return bIsOk;
}


/*----------------------------------------------------------------------------------------
GetSpaceNeededToStore

  This calculates the extra space needed to store an object in our file system.
 
  Parameters:  none

  Returns:  Number of extra bytes to store the object--could be < 0 if the new object
  			is smaller than the current one
----------------------------------------------------------------------------------------*/
int CSwKeysHandler::GetSpaceNeededToStore(void)
{
	assert(m_pObject);
	int SpaceNeeded;
	CMenuSettingsParser parser(m_pObject);
	
	int CurrentLength = GetFileLength(FILENAME_PRODUCT_CONFIG);
	if ( CurrentLength < 0 ) CurrentLength = 0;  // <0 means error or no file--assume no file
	int NewLength = parser.GetLength() - 1;
	SpaceNeeded = NewLength - CurrentLength;
	MOCF_DEBUG("GetSpaceNeededToStore SW Keys returned %d\n", SpaceNeeded);
	return SpaceNeeded;
}





/*----------------------------------------------------------------------------------------
StoreObject

  This stores the Object.

  Parameters:  none

  Returns:  true if the object is stored OK, false otherwise.
----------------------------------------------------------------------------------------*/
bool CUserFileHandler::StoreObject(void)
{
	assert(m_pObject);
	const CTlv::Header_t * pFileNameObject;
	const CTlv::Header_t * pDataObject;
	bool bIsOk;
	
	
	CUserFileParser parser(m_pObject);
	if ( ! parser.FindRecord(Gen6::tagFileName, &pFileNameObject) ) return false;
	if ( ! parser.FindRecord(Gen6::tagData, &pDataObject) ) return false;
	CFileNameParser FileName(pFileNameObject);
	CDataParser data(pDataObject);
	// we now have the necessary Object Records--store the data
	bIsOk =  CFileHandler::StoreUserFile(FileName.GetData(), data.GetData(), data.GetLength());
	
	MOCF_DEBUG("Storing User File returned %d\n", bIsOk);
	return bIsOk;
}


/*----------------------------------------------------------------------------------------
GetSpaceNeededToStore

  This calculates the extra space needed to store an object in our file system.
 
  Parameters:  none

  Returns:  Number of extra bytes to store the object--could be < 0 if the new object
  			is smaller than the current one
----------------------------------------------------------------------------------------*/
int CUserFileHandler::GetSpaceNeededToStore(void)
{
	assert(m_pObject);
	const CTlv::Header_t * pFileNameObject;
	int SpaceNeeded;
	CUserFileParser parser(m_pObject);
	
	parser.FindRecord(Gen6::tagFileName, &pFileNameObject);
	CFileNameParser FileName(pFileNameObject);
	HSTRING UserFile(100, true);
	CDataParser data(pFileNameObject);
	
	UserFile.Copy(USER_FOLDER);
	UserFile.Append("/");
	UserFile.Append(FileName.GetData());
	
	int CurrentLength = GetFileLength((const char *)UserFile.Char);
	if ( CurrentLength < 0 ) CurrentLength = 0;  // <0 means error or no file--assume no file
	int NewLength = data.GetLength();
	SpaceNeeded = NewLength - CurrentLength;
	MOCF_DEBUG("GetSpaceNeededToStore User File returned %d\n", SpaceNeeded);
	return SpaceNeeded;
}







