/*===================================================================
  Overview:
  MOCF is implemented with three "types" of classes:  Creator, Parser,
  and Handler.  The Creator classes are used to construct the MOCF
  and the Records it contains.  The Parser classes are used to read
  an MOCF.  The Handler classes are used to write Object Records
  inside the MOCF.
  
 This class is responsible for storing the Object Records in an MOCF.
 Unfortunately, the term "handle" is very generic, but frankly, 
 MocfStorer looked too odd.  It creates an Object Handler for each 
 object found in the MOCF and uses that handler to store the Object data.
 
 We avoid a standard constructor to ensure the data always exists.
=====================================================================
   $Source: Gen6FoundationApp/FoundationApp/mocf/MocfHandler.cpp $
   $Date: 2010/03/17 10:24:21EDT $
   $Revision: 1.6 $


===================================================================*/
#include <assert.h>
#include "MocfHandler.h"
#include "Gen6ObjectHandler.h"
#include "MocfTags.h"
#include "StartupFlashDisk.h"

//#define INC_MOCF_DEBUG // uncomment this to include debug prints to std interface

#define FUDGE_FACTOR	(32*1024) // An adjustment for IsThereRoom() in case the math isn't accurate enough


#ifdef INC_MOCF_DEBUG
#include "ioman.h"
// FYI:  the "## in the macro expansion handles the case MOCF_DEBUG("only text, no variables")
#define MOCF_DEBUG(format, ...) theStdInterface.printf(format, ## __VA_ARGS__)
#else
#define MOCF_DEBUG(format, ...)
#endif





/*----------------------------------------------------------------------------------------
StoreObjects

  Stores all Objects in an MOCF.

  Parameters:  none

  Returns:  true if stored OK, false otherwise.
----------------------------------------------------------------------------------------*/
bool CGen6MocfHandler::StoreObjects(void)
{
	const CTlv::Header_t * pObject;
	CGen6ObjectRecordHandler * pObjectHandler;
	bool bIsOk = true;
	assert(m_pTlvContainer);
	

	pObject = GetFirstRecord();
	while ( bIsOk && pObject )
	{
		if ( CMocfParser::IsObjectRecord(pObject) )
		{
			pObjectHandler = CGen6ObjectRecordHandler::CreateObjectRecordHandler(pObject);
			bIsOk = pObjectHandler->StoreObject();
			delete pObjectHandler;
		}
		pObject = GetNextRecord();
	}
	MOCF_DEBUG("StoreObjects returning %d\n", bIsOk);
	return bIsOk;
}





/*----------------------------------------------------------------------------------------
IsCompatible

  Searches all Compatible Products Records to ensure they contain the ID.  

  Parameters:  
  			CompatibleProdId[in]:  ID to search for

  Returns:  true if ID was found, false otherwise.
----------------------------------------------------------------------------------------*/
bool CGen6MocfHandler::IsCompatible(DWORD CompatibleProdId)
{
	const CTlv::Header_t * pRecord;
	bool bIsCompatible = false;
	
	if ( FindRecord(Mocf::tagMocfCompatibleProducts, &pRecord) )
	{
		CMocfCompatProdParser CompatProdParser(pRecord);
		if ( CompatProdParser.FindId(CompatibleProdId) ) bIsCompatible = true;
	}
	else assert(0);  // this should never happen because we should have validated it earlier
	
	if ( bIsCompatible )
	{	// Now look for Firmware Object Records & check the records inside those as well.
		if ( FindRecord(Gen6::tagBootFirmware, &pRecord) )
		{
			bIsCompatible = IsFwCompatible(pRecord, CompatibleProdId);
		}
		if ( bIsCompatible && FindRecord(Gen6::tagAppFirmware, &pRecord) )
		{
			bIsCompatible = IsFwCompatible(pRecord, CompatibleProdId);
		}
	}
	
	return bIsCompatible;
}




/*----------------------------------------------------------------------------------------
IsThereRoom

  This determines if there's enough room to store all the objects.  User files & system
  files share the same volume.  There's no enforced "allocation" of space for either (which
  is nice because it simplifies things a bit).  

  Parameters:  none

  Returns:  true if there's enough space to store the objects, false otherwise.
----------------------------------------------------------------------------------------*/
bool CGen6MocfHandler::IsThereRoom(void)
{
	const CTlv::Header_t * pRecord;
	CGen6ObjectRecordHandler * pObjectHandler;
	bool bIsOk;
	assert(m_pTlvContainer);
	int NumFsObjectRecords;  // number of object records
	int TotalLength;
	

	NumFsObjectRecords = 0;
	TotalLength = 0;
	pRecord = GetFirstRecord();
	while ( pRecord )
	{
		if ( CMocfParser::IsObjectRecord(pRecord) )
		{
			pObjectHandler = CGen6ObjectRecordHandler::CreateObjectRecordHandler(pRecord);
			TotalLength += pObjectHandler->GetSpaceNeededToStore();
			NumFsObjectRecords++;
			delete pObjectHandler;
		}
		pRecord = GetNextRecord();
	}
	// We now have to account for the filesystem.  
	TotalLength += NumFsObjectRecords * GetMaxOverheadPerFile(); // Worse case is we have 1 byte in the last sector.
	///TotalLength += GetFilesystemOverhead(); // This should account for its internal tables
	TotalLength += FUDGE_FACTOR;  // We don't want to cut it too close
	
	int FreeSpace = GetFreeSpace();
	if ( TotalLength <= FreeSpace ) bIsOk = true;
	else bIsOk = false;
	MOCF_DEBUG("IsThereRoom returning %d\n", bIsOk);
	return bIsOk;
}




/*----------------------------------------------------------------------------------------
IsFwCompatible

  Searches the Firmware Object Record's Compatible Products Record to ensure it contains
  the ID.  

  Parameters:  
  			pRecord[in]:  Firmware Object Record

  			CompatibleProdId[in]:  ID to search for

  Returns:  true if ID was found, false otherwise.
----------------------------------------------------------------------------------------*/
bool CGen6MocfHandler::IsFwCompatible(const CTlv::Header_t * pRecord, DWORD CompatibleProdId)
{
	CFirmwareParser FwParser(pRecord);
	if ( ! FwParser.FindRecord(Gen6::tagCompatProd, &pRecord) ) return false;  // should never happen
	CMocfCompatProdParser CompatProdParser(pRecord);
	if ( ! CompatProdParser.FindId(CompatibleProdId) ) return false;
	
	return true;
}









