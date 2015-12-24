/*===================================================================
  Overview:
  MOCF is implemented with three "types" of classes:  Creator, Parser,
  and Handler.  The Creator classes are used to construct the MOCF
  and the Records it contains.  The Parser classes are used to read
  an MOCF.  The Handler classes are used to write the MOCF.
  
  This module has two classes.  CMocfParser parses an MOCF binary for
  MOCF Records.  It can also check for the validity of an MOCF.  
  The constructor initializes it with data.  Usually IsValid() is the
  first method called.
  
  CMocfReocrdParser and the derived classes are parsers for the MOCF
  Records.


  This code must compile under these OSs:  Microsoft, Linux, Gen6 embedded
=====================================================================
   $Source: MocfParser.cpp $
   $Date: 2010/02/12 16:21:40EST $
   $Revision: 1.4 $


===================================================================*/
#include <assert.h>
#include <string.h>
#include "MocfParser.h"
#include "sha1.h"
#include "MocfTags.h"




/*----------------------------------------------------------------------------------------
IsValid

  Determines if the MOCF data is a valid MOCF.

  Parameters: none


  Returns:  true if the MOCF data is a valid MOCF, false otherwise.
----------------------------------------------------------------------------------------*/
bool CMocfParser::IsValid(void)
{
	unsigned char TheHash[CSha1::HashLength];
	const CTlv::Header_t * pRecord;
	int ObjectLocation;

	if ( ! CTlvContainerParser::IsValid() ) return false;
	if ( CTlv::GetTag((const CTlv::Header_t *) m_pTlvContainer) != Mocf::tagMocfId ) return false;

	// Verfify all known MOCF Objects are valid and in right order.
	// The following Objects are required.
	if ( ! FindRecord(Mocf::tagMocfVersion, &pRecord, &ObjectLocation) ) return false;
	if ( ObjectLocation != 1 ) return false;
	CMocfVersionParser MocfVersion(pRecord);
	if ( ! MocfVersion.IsValid() ) return false;
	
	// The following Objects are optional.
	if ( FindRecord(Mocf::tagMocfHash, &pRecord, &ObjectLocation) )
	{
		if ( ObjectLocation != 2 ) return false;
		CMocfHashParser MocfHash(pRecord);
		if ( ! MocfHash.IsValid() ) return false;
		if ( ! CalcHash(TheHash) ) return false;
		if ( memcmp(MocfHash.GetHash(), TheHash, sizeof TheHash) ) return false;
	}
	if ( FindRecord(Mocf::tagMocfCompatibleProducts, &pRecord, &ObjectLocation) )
	{
		CMocfCompatProdParser CompatProd(pRecord);
		if ( ! CompatProd.IsValid() ) return false;
	}

	return true;
}



/*----------------------------------------------------------------------------------------
CalcHash

  Calculates the hash of the MOCF.  

  Parameters: 
		pHash[out]:  pointer to the MOCF hash.  The caller is responsible for ensuring
		             there is enough memory to hold the hash.



  Returns:  true if the hash was calculated OK, false otherwise.
----------------------------------------------------------------------------------------*/
bool CMocfParser::CalcHash(unsigned char * pHash)
{
	assert(m_pTlvContainer);
	int NumBytesLeft;
	const unsigned char * pBytes = (const unsigned char *) m_pTlvContainer;

	NumBytesLeft = GetContainerSize();
	m_Sha.Init();
	m_Sha.Update(pBytes, sizeof(CTlv::Header_t));  // MOCF 2 DWORD header (ID & Length)
	pBytes += sizeof(CTlv::Header_t);
	NumBytesLeft -= sizeof(CTlv::Header_t);
	while ( NumBytesLeft > 0 )
	{
		int NumBytesHashed = HashNextRecord((const CTlv::Header_t *) pBytes);
		pBytes += NumBytesHashed;
		NumBytesLeft -= NumBytesHashed;
	}
	m_Sha.GetResult(pHash);
	return (NumBytesLeft == 0);  // something bad happened if it's < 0
}




/*----------------------------------------------------------------------------------------
IsObjectRecord

  Detrmines if the Record is an Object Record (versus an MOCF Record).  

  Parameters: 
		pRecord[in]:  pointer to an MOCF Record.



  Returns:  true if the hash was calculated OK, false otherwise.
----------------------------------------------------------------------------------------*/
bool CMocfParser::IsObjectRecord(const CTlv::Header_t * pRecord)
{
	if ( pRecord->Tag >= Mocf::tagMocfObject ) return true;
	return false;
}
 


/*----------------------------------------------------------------------------------------
HashNextRecord

  This is a helper method for calculating the hash.  It hashes any padding bytes after
  the Object.


  Parameters: 
		pRecord[in]:  pointer to an Object to hash


  Returns:  the number of bytes hashed.
----------------------------------------------------------------------------------------*/
int CMocfParser::HashNextRecord(const CTlv::Header_t * pRecord)
{
	int NumBytesToHash = sizeof(CTlv::Header_t) + CTlv::GetLength(pRecord) + CalcNumPaddingBytes(pRecord);
	if ( CTlv::GetTag(pRecord) != Mocf::tagMocfHash ) m_Sha.Update((const unsigned char *) pRecord, NumBytesToHash);
	return NumBytesToHash;
}






//			MOCF Object Parsers



/*===========================================================================
 CMocfVersionParser

 Record Format:
	typedef struct
	{
		CMocfParser::CTlv::Header_t header;
		unsigned char MajorVersion;
		unsigned char MinorVersion;
	} Header_t;
============================================================================*/

/*----------------------------------------------------------------------------------------
IsValid

  Determines if the MOCF Version Object is valid for this version of the MOCF.

  Parameters: 
		pRecord[in]:  pointer to an Object to hash


  Returns:  true if the Object a valid MOCF Object, false otherwise.
----------------------------------------------------------------------------------------*/
bool CMocfVersionParser::IsValid(void)
{
	const CTlv::Header_t * pRecord = m_pObject;
	pRecord++;  // point to the data
	const unsigned char * pMajorVersionByte = (const unsigned char *) pRecord; // Major Version is the 1st byte
	if ( *pMajorVersionByte > MajorVersion ) return false; 
	return true;
}



/*===========================================================================
 CMocfHashParser

 Record Format:
	typedef struct
	{
		CMocfParser::CTlv::Header_t header;
		unsigned char hash[SHA_HASH_LEN];
	} Header_t;
============================================================================*/

/*----------------------------------------------------------------------------------------
IsValid

  Determines if MOCF Hash Object is valid.

  Parameters: 
		pRecord[in]:  pointer to an Object to hash


  Returns:  true if the Object a valid MOCF Object, false otherwise.
----------------------------------------------------------------------------------------*/
bool CMocfHashParser::IsValid(void)
{
	if ( CTlv::GetLength(m_pObject) != (DWORD) CSha1::HashLength ) return false; 
	return true;
}

/*----------------------------------------------------------------------------------------
GetHash

  Gets the hash data from an MOCF Hash Object.

  Parameters: 
		pRecord[in]:  pointer to an Object to hash


  Returns:  pointer to the hash data.
----------------------------------------------------------------------------------------*/
const unsigned char * CMocfHashParser::GetHash(void)
{
	const unsigned char * pObjectData = ((const unsigned char *)m_pObject) + sizeof(CTlv::Header_t);
	return pObjectData;
}




/*===========================================================================
 CMocfCompatProdParser

 Record Format:
	typedef struct
	{
		CMocfParser::CTlv::Header_t header;
		DWORD NumItems;
		DWORD Items[];
	} Header_t;
============================================================================*/

bool CMocfCompatProdParser::IsValid(void)
{
	DWORD NumItems = GetNumItems();
	if ( m_pObject->Length != (NumItems + 1) * sizeof(DWORD) ) return false;
	
	return true;
}


/*----------------------------------------------------------------------------------------
FindId

  Searches the list for an ID.

  Parameters: 
		item[in]:  ID to look for


  Returns:  true if the ID was found, false otherwise.
----------------------------------------------------------------------------------------*/
bool CMocfCompatProdParser::FindId(DWORD id)
{
	const DWORD * pDwords = (DWORD *) CTlv::GetData(m_pObject);
	DWORD NumItems;
	memcpy(&NumItems, pDwords, sizeof NumItems);
	pDwords++;  // skip over Num of Items DWORD--we now point to the list of DWORDs
	for ( DWORD i = 0; (i < NumItems ); i++)
	{
		if ( *pDwords == id ) return true;
		pDwords++;
	}
	return false;
}


/*----------------------------------------------------------------------------------------
GetNumItems

  Gets the number of items (IDs) in the list.

  Parameters: none


  Returns:  the number of items in the list.
----------------------------------------------------------------------------------------*/
DWORD CMocfCompatProdParser::GetNumItems(void)
{
	const DWORD * pDwords = (DWORD *) CTlv::GetData(m_pObject);
	DWORD NumItems;
	memcpy(&NumItems, pDwords, sizeof NumItems);
	
	return NumItems;
}



