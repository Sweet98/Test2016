/*===================================================================
  Overview:
  MOCF is implemented with three "types" of classes:  Creator, Parser,
  and Handler.  The Creator classes are used to construct the MOCF
  and the Records it contains.  The Parser classes are used to read
  an MOCF.  The Handler classes are used to write Object Records
  inside the MOCF.

  This module holds all the MOCF Gen6 specific Object Record parsers.
  
  To use these classes, create an instantiation with a pointer to
  the Object Reocrd's data.  Some classes have extra methods that
  are specific to that Object Record.

  This code must compile under these OSs:  Microsoft, Linux, Gen6 embedded
=====================================================================
   $Source: Gen6ObjectParser.cpp $
   $Date: 2011/11/10 03:37:01EST $
   $Revision: 1.19 $


===================================================================*/
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "Gen6ObjectParser.h"
#include "MocfTags.h"


//#define INC_MOCF_DEBUG // uncomment this to include debug prints to std interface

#define FFS_MAX_FILE_NAME_LEN	57 // NOTE:  document says 57, source code comment says 65.  Use the smaller to be safe.



#ifdef INC_MOCF_DEBUG
#include "ioman.h"
// FYI:  the "## in the macro expansion handles the case MOCF_DEBUG("only text, no variables")
#define MOCF_DEBUG(format, ...) theStdInterface.printf(format, ## __VA_ARGS__)
#else
#define MOCF_DEBUG(format, ...)
#endif



/*----------------------------------------------------------------------------------------
IsValid

  This ensures the Gen6 MOCF is valid.
  
  TODO:  When we count the Gen6 Object Records that we know, we make use of the last Tag
  to see if we understand it (and count it).  It would be nicer if we could make use of 
  CreateObjectRecordParser() and avoid having to update that part every time we add a Tag.

  Parameters:  none

  Returns:  true if the MOCF is OK, false otherwise.
----------------------------------------------------------------------------------------*/
bool CGen6MocfParser::IsValid(void)
{
	assert(m_pTlvContainer);
	const CTlv::Header_t * pRecord;
	const CTlv::Header_t * pCompatProdRecord;
	CGen6ObjectRecordParser * pObjectParser;
	bool bIsOk = true;
	bool bCompatCorded;
	bool bCompatRfBase;
	bool bCompatRfScanner;
	bool bCompatVuquest3310;
	bool bCompatGen6Corded1500;


	if ( ! CMocfParser::IsValid() ) return false;
	// check all known Gen6 objects
	if ( ! FindRecord(Mocf::tagMocfCompatibleProducts, &pCompatProdRecord) ) return false;  // Gen6 requires this Record
	// If there are Object Records in this container, check for invalid mixes
	if ( HasObjectRecords() )
	{	// Check Compatible Products Record for mixes not allowed--Boot Firmware is compatible with all products, so mixes are allowed
   		if ( !(FindRecord(Gen6::tagBootFirmware, &pRecord)||FindRecord(Gen6::tagUserFile, &pRecord))) // Boot Firmware and user file runs on all platforms
		{	// No mixes allowed, if 4980 get its compatiable ID, we need to add judgement here to make sure no mixes between 4980 and Xenon
			CMocfCompatProdParser CompatProdParser(pCompatProdRecord);
			
			bCompatCorded = CompatProdParser.FindId(cpGen6CordedScannerAppFw);
			bCompatRfScanner = CompatProdParser.FindId(cpGen6RfScannerAppFw);
			bCompatRfBase = CompatProdParser.FindId(cpGen6RfBaseAppFw);
			if ( bCompatCorded && bCompatRfBase ) return false;
			if ( bCompatCorded && bCompatRfScanner ) return false;
			if ( bCompatRfBase && bCompatRfScanner ) return false;
#ifdef XENON_LITE_1500
			if ( (bCompatCorded || bCompatRfBase || bCompatRfScanner) ) return false;
#endif
#ifdef HI2D_T
#ifdef YJ2D_BASED_ON_HI2D
			bCompatVuquest3310 = CompatProdParser.FindId(cpVuquest3310AppFw);
			bCompatGen6Corded1500 = CompatProdParser.FindId(cpGen6Corded1500AppFw);
			if ( (bCompatCorded || bCompatRfBase || bCompatRfScanner || bCompatVuquest3310 || bCompatGen6Corded1500) ) return false;
#else
			bCompatVuquest3310 = CompatProdParser.FindId(cpVuquest3310AppFw);
			bCompatGen6Corded1500 = CompatProdParser.FindId(cpGen6Corded1500AppFw);
			if ( (bCompatCorded || bCompatRfBase || bCompatRfScanner || bCompatVuquest3310 || bCompatGen6Corded1500) ) return false;
#endif
#endif
		}
	}
	// Check each object to ensure it's valid.  Also, let's make sure we don't have multiple copies of objects that we know about (and don't allow).
	int NumObjects[10];  // This will hold a count of all Tags (we subtract the upper bits so the index starts at 0)
	memset(NumObjects, 0, sizeof NumObjects);
	pRecord = GetFirstRecord();
	while ( bIsOk && pRecord )
	{
		pObjectParser = CGen6ObjectRecordParser::CreateObjectRecordParser(pRecord);
		bIsOk = pObjectParser->IsValid();
		if ( bIsOk && (pRecord->Tag >= Gen6::tagBootFirmware)
			&& (pRecord->Tag < Gen6::tagExecuteMenuSettings) )
		{	// It's a Record we understand--count it
			int index = pRecord->Tag - Gen6::tagBootFirmware;
			NumObjects[index]++;
		}
		delete pObjectParser;
		pRecord = GetNextRecord();
	}
	if ( bIsOk )
	{	// Check for multiple copies of Object Records.  We allow multiple User File Object Records
		const unsigned UserFileIndex = Gen6::tagUserFile - Gen6::tagBootFirmware;
		const unsigned ExecMenuSettingsIndex = Gen6::tagExecuteMenuSettings - Gen6::tagBootFirmware;
		for (unsigned i = 0; i < (sizeof NumObjects)/sizeof(int); i++ )
		{
			if ( (NumObjects[i] > 1) && (i != UserFileIndex) && (i != ExecMenuSettingsIndex) )
			{
				MOCF_DEBUG("Multiple copies of Tag %#x\n", Gen6::tagBootFirmware + i);
				bIsOk = false;
			}
		}
	}
	MOCF_DEBUG("CGen6MocfParser::IsValid returning %d\n", bIsOk);
	return bIsOk;
}



/*----------------------------------------------------------------------------------------
IsSystemUpdateMocf

  Determines if an MOCF is a System Update MOCF (if it has special "system" objects.

  Parameters:  none

  Returns:  true if it is a System Update MOCF, false otherwise.
----------------------------------------------------------------------------------------*/
bool CGen6MocfParser::IsSystemUpdateMocf(void)
{
	assert(m_pTlvContainer);
	const CTlv::Header_t * pObject;
	CGen6ObjectRecordParser * pObjectParser;
	bool bFoundSystemObject = false;


	pObject = GetFirstRecord();
	while ( pObject && ! bFoundSystemObject )
	{
		pObjectParser = CGen6ObjectRecordParser::CreateObjectRecordParser(pObject);
		if ( pObjectParser->IsSystemObject() ) bFoundSystemObject = true;
		delete pObjectParser;
		pObject = GetNextRecord();
	}
	
	return bFoundSystemObject;
}





bool CGen6MocfParser::HasObjectRecords(void)
{
	const CTlv::Header_t * pRecord;
	bool bFoundObjectRecord = false;


	pRecord = GetFirstRecord();
	while ( pRecord && ! bFoundObjectRecord )
	{
		if ( IsObjectRecord(pRecord) ) bFoundObjectRecord = true;
		pRecord = GetNextRecord();
	}
	return bFoundObjectRecord;
}




//			Object Record Parser classes

/*----------------------------------------------------------------------------------------
CreateObjectRecordParser

  Creates a handler for an object inside an MOCF.  If an Object is unknown, it will
  return a handler for that as well.

  TODO:  Rather than have the ugly #if, maybe we could have a smaller class
  (CGen6ObjectRecordParser_Boot) and have CGen6ObjectRecordParser inherit from this smaller one.

  Parameters:  
  			pObject[in]:  pointer to an Object

  Returns:  an Object Handler.
----------------------------------------------------------------------------------------*/
CGen6ObjectRecordParser * CGen6ObjectRecordParser::CreateObjectRecordParser(const CTlv::Header_t * pObject)
{
	CGen6ObjectRecordParser * pParser;
	switch ( pObject->Tag )
	{
		case Gen6::tagBootFirmware:
			pParser = new CFirmwareParser(pObject);
		break;
		
		case Gen6::tagAppFirmware:
			pParser = new CFirmwareParser(pObject);
		break;
		
#if ! defined(MATRIX_BOOT_CODE)  // Boot Firmware doesn't need all these parsers
		case Gen6::tagUserFile:
			pParser = new CUserFileParser(pObject);
		break;
		
		case Gen6::tagSoftwareKeys:
			pParser = new CSwKeysParser(pObject);
		break;
		
		case Gen6::tagCustomDefMenuSettings:
			pParser = new CCustomDefMenuSettingsParser(pObject);
		break;
		
		case Gen6::tagMenuSettings:
			pParser = new CMenuSettingsParser(pObject);
		break;
		
		case Gen6::tagExecuteMenuSettings:
			pParser = new CExecuteMenuSettingsParser(pObject);
		break;
#endif // endof #if ! defined(MATRIX_BOOT_CODE)

		case Mocf::tagMocfId:
		case Gen6::tagMocf:
			pParser = new CNestedMocfParser(pObject);
		break;
		
		default:
			pParser = new CUnkownParser(pObject);
	}
	
	return pParser;
}




/*----------------------------------------------------------------------------------------
IsValid

  This ensures the required Records are present.

  Parameters:  none

  Returns:  true if the object is OK, false otherwise.
----------------------------------------------------------------------------------------*/
bool CFirmwareParser::IsValid(void)
{
	assert(m_pTlvContainer);
	const CTlv::Header_t * pObject;

	pObject = GetFirstRecord();
	if ( ! pObject || CTlv::GetTag(pObject) != Gen6::tagCompatProd )
	{
		MOCF_DEBUG("CompatProd record missing or in wrong place\n");
		return false;
	}
	CMocfCompatProdParser CompatProdParser(pObject);  // We use the same format
	if ( ! CompatProdParser.IsValid() ) return false;
	
	pObject = GetNextRecord();
	if ( ! pObject || CTlv::GetTag(pObject) != Gen6::tagSwVersionStr )
	{
		MOCF_DEBUG("SwVersion record missing or in wrong place\n");
		return false;
	}
	CSwVersionParser SwVersion(pObject);
	if ( ! SwVersion.IsValid() ) return false;

	pObject = GetNextRecord();
	if ( ! pObject || CTlv::GetTag(pObject) != Gen6::tagData )
	{
		MOCF_DEBUG("Data record missing or in wrong place\n");
		return false;
	}

	return true;
}


/*----------------------------------------------------------------------------------------
GetFwData

  This is a helper method to make it easy to get the Firmware binary.

  Parameters:  
  			ppFwData[out]:  pointer to the Firmware binary

  			pLength[out]:  pointer to the Firmware binary length

  Returns:  true if the Record is there, false otherwise.
----------------------------------------------------------------------------------------*/
bool CFirmwareParser::GetFwData(const unsigned char ** ppFwData, DWORD * pLength)
{
	const CTlv::Header_t * pDataRecord;
	if ( FindRecord(Gen6::tagData, &pDataRecord) )
	{
		CDataParser DataParser(pDataRecord);
		*ppFwData = DataParser.GetData();
		*pLength = DataParser.GetLength();
		return true;
	}
	else return false;
}



/*----------------------------------------------------------------------------------------
GetSwVersion

  This is a helper method to make it easy to get the Software Version string.

  Parameters:  
  			ppSwVersion[out]:  pointer to the SW Version string

  Returns:  an Object Handler.
----------------------------------------------------------------------------------------*/
bool CFirmwareParser::GetSwVersion(const char ** ppSwVersion)
{
	const CTlv::Header_t * pSwVersionRecord;
	if ( FindRecord(Gen6::tagSwVersionStr, &pSwVersionRecord) )
	{
		CSwVersionParser SwVersionParser(pSwVersionRecord);
		*ppSwVersion = SwVersionParser.GetData();
		return true;
	}
	else return false;
}


#if ! defined(MATRIX_BOOT_CODE)  // Boot Firmware doesn't need all these parsers


/*----------------------------------------------------------------------------------------
IsValid

  This ensures the required Records are present.

  Parameters:  none

  Returns:  true if the object is OK, false otherwise.
----------------------------------------------------------------------------------------*/
bool CUserFileParser::IsValid(void)
{
	assert(m_pTlvContainer);
	const CTlv::Header_t * pObject;

	pObject = GetFirstRecord();
	if ( ! pObject || CTlv::GetTag(pObject) != Gen6::tagFileName )
	{
		MOCF_DEBUG("FileName record missing or in wrong place\n");
		return false;
	}
	CFileNameParser FileName(pObject);  // We use the same format
	if ( ! FileName.IsValid() ) return false;
	
	pObject = GetNextRecord();
	if ( ! pObject || CTlv::GetTag(pObject) != Gen6::tagData )
	{
		MOCF_DEBUG("Data record missing or in wrong place\n");
		return false;
	}
	CDataParser Data(pObject);
	if ( ! Data.IsValid() ) return false; // TODO:  is this really necessary?

	return true;
}


/*----------------------------------------------------------------------------------------
IsValid

  This ensures the string is a valid Menu Setting string.
  
  Parameters:  none

  Returns:  true if the string is OK, false otherwise.
----------------------------------------------------------------------------------------*/
bool CExecuteMenuSettingsParser::IsValid(void)
{
	assert(m_pTlvContainer);
	const char * pStr = GetData();
	int StrLen = (int) strlen(pStr);


	for (int i = 0; i < StrLen; i++)
	{
		if ( ! isprint(*pStr) ) return false;
		pStr++;
	}

	return true;
}



/*----------------------------------------------------------------------------------------
IsValid

  This ensures the string is a valid Menu Setting string.
  
  Parameters:  none

  Returns:  true if the string is OK, false otherwise.
----------------------------------------------------------------------------------------*/
bool CMenuSettingsParser::IsValid(void)
{
	assert(m_pTlvContainer);
	const char * pStr = GetData();
	int StrLen = (int) strlen(pStr);


	for (int i = 0; i < StrLen - 1; i++)  // -1 because we'll check the terminator after the loop
	{
		if ( ! isprint(*pStr) ) return false;
		pStr++;
	}
	if ( *pStr != GetMyTerminator() ) return false;

	return true;
}



//		These are parsers INSIDE the above Gen6 Object Records


/*----------------------------------------------------------------------------------------
IsValid

  This ensures the File Name has no unusual characters in it and is not too long.

  Parameters:  none

  Returns:  true if the File Name is OK, false otherwise.
----------------------------------------------------------------------------------------*/
bool CFileNameParser::IsValid(void)
{
	assert(m_pTlvContainer);
	const char AllowedNonAlphaNumChars[] = "_.()";
	const char * pStr = GetData();
	int StrLen = (int) strlen(pStr);


	if ( strlen(pStr) > FFS_MAX_FILE_NAME_LEN ) return false;
	for (int i = 0; i < StrLen; i++)
	{
		if ( ! isalnum(*pStr) )
		{
			if ( ! strchr(AllowedNonAlphaNumChars, *pStr) ) return false;
		}
		pStr++;
	}

	return true;
}


#endif // endof #if ! defined(MATRIX_BOOT_CODE)







