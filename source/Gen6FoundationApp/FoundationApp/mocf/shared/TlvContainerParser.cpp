/*===================================================================
  There are two classes in this module.  CTLV is a helper class.
  CTlvContainerParser is a base class is a "read only" TLV container 
  object.  This container is a single TLV object that contains a 
  series/sequence/array of other TLV Objects (Records):
    TLV
      TLV 1
      TLV 2
      ...
      TLV n

  Each TLV Record's Tag is aligned on a 4 byte boundary (driven by
  an MOCF requirement).  The constructor guarantees the class only 
  exists with data.  Usually IsValid() is the first method called.
  
  Note:  this class does NOT make a copy of the data.  The
  pointer to the data must persist through the entire use of this
  class as the other methods will refer to this pointer.  The classes
  are constructed with a pointer to a CTlv::Header_t *, but in reality,
  it's an entire Record of data that begins with that header.  We do
  this for object oriented convenience.  This constructor also ensure
  us that the internal m_pTlvContainer pointer is always valid.
  
   Also, all CTlv::Header_t pointers are into this data as well.  Even
  though MOCFs are 32 bit aligned, you may not always guarantee the
  start of the MOCF is on a 32 bit boundary.  Therefore, you will not
  see CTlv::Header_t pointer variables dereferenced.  Instead, helper methods
  (GetTag() & GetLength()) are used.  These methods can also adjust 
  for Endianness should the need arise.


  This code must compile under these OSs:  Microsoft, Linux, Gen6 embedded
=====================================================================
   $Source: TlvContainerParser.cpp $
   $Date: 2010/02/05 18:23:32EST $
   $Revision: 1.3 $


===================================================================*/
#include <assert.h>
#include <string.h>
#include "TlvContainerParser.h"




/*----------------------------------------------------------------------------------------
GetTag

  This takes a CTlv::Header_t pointer and returns the Tag it contains.  This ensures the
  CTlv::Header_t pointers do not have to point to 32 bit aligned code (for ARM).  It
  can also adjust for Big Endian when needed.

  Note:  this code only works for Little Endian


  Parameters: 
		pRecord[in]:  pointer to an Object.


  Returns:  DWORD Tag value.
----------------------------------------------------------------------------------------*/
DWORD CTlv::GetTag(const CTlv::Header_t * pRecord)
{
	DWORD Tag;

	const unsigned char * pData = (const unsigned char *) pRecord;
	memcpy(&Tag, pData, sizeof(Tag));

	return Tag;
}

/*----------------------------------------------------------------------------------------
GetLength

  This takes a CTlv::Header_t pointer and returns the Length it contains.  This ensures the
  CTlv::Header_t pointers do not have to point to 32 bit aligned code (for ARM).  It
  can also adjust for Big Endian when needed.

  Note:  this code only works for Little Endian


  Parameters: 
		pRecord[in]:  pointer to an Object.


  Returns:  DWORD Length value.
----------------------------------------------------------------------------------------*/
DWORD CTlv::GetLength(const CTlv::Header_t * pRecord)
{
	DWORD Length;

	const unsigned char * pData = (const unsigned char *) pRecord;
	pData += sizeof(DWORD); // skip over Tag field
	memcpy(&Length, pData, sizeof(Length));

	return Length;
}


/*----------------------------------------------------------------------------------------
GetData

  This takes a CTlv::Header_t pointer and returns a pointer to the data it contains.  



  Parameters: 
		pRecord[in]:  pointer to an Object.


  Returns:  pointer to the TLV Value (data).
----------------------------------------------------------------------------------------*/
const unsigned char * CTlv::GetData(const CTlv::Header_t * pRecord)
{
	pRecord++;  // skip over header and point to data
	return (const unsigned char *)pRecord;
}








/*----------------------------------------------------------------------------------------
GetContainerSize

  This helper method returns the total size of the container (header & data).  



  Parameters: none


  Returns:  the total size of the container.
----------------------------------------------------------------------------------------*/
int CTlvContainerParser::GetContainerSize(void)
{
	assert(m_pTlvContainer);
	return CTlv::GetLength(m_pTlvContainer) + sizeof(CTlv::Header_t);
}


/*----------------------------------------------------------------------------------------
GetLength

  This method returns the length of data in the container.  


  Parameters: none


  Returns:  the length of data in the container.
----------------------------------------------------------------------------------------*/
 int CTlvContainerParser::GetLength(void)
{
	assert(m_pTlvContainer);
	return CTlv::GetLength(m_pTlvContainer);
}


/*----------------------------------------------------------------------------------------
IsValid

  Determines if this contains a valid TLV Container Object.  There's not much that
  can be done since this is such an abstract container.

  Parameters: none


  Returns:  true if the TLV container pointer is set, false otherwiase.
----------------------------------------------------------------------------------------*/
bool CTlvContainerParser::IsValid(void)
{
	return (m_pTlvContainer != NULL);
}





/*----------------------------------------------------------------------------------------
CalcNumPaddingBytes

  All Records start on a 4B boundary.  This helper method calculates the number of padding 
  byes to add to the supplied object to keep boundary requirement.

  Parameters: 
		pRecord[in]:  pointer to a Record.


  Returns:  the number of padding bytes (0 to 3).
----------------------------------------------------------------------------------------*/
int CTlvContainerParser::CalcNumPaddingBytes(const CTlv::Header_t * pRecord)
{
	int NumPaddingBytes;
	int NumPast4BBoundary = (CTlv::GetLength(pRecord) & 0x03);
	if ( NumPast4BBoundary )
	{	// we need to pad
		NumPaddingBytes = 4 - NumPast4BBoundary;
	}
	else NumPaddingBytes = 0;

	return NumPaddingBytes;
}


/*----------------------------------------------------------------------------------------
FindRecord

  This looks for a specific MOCF Object (Record) in an MOCF.  This makes use of 
  GetFirstRecord() and GetFirstRecord().  In case the caller is also using these and then
  calls this method, we save & restore the state for them.  


  Parameters: 
		Tag[in]:  the tag of the object to look for.

		ppRecord[out]:  pointer pointer to a Record.  Will get updated to point to the
		               Record, if found.

		pRecordLocation[out]:  If not NULL, it will be updated with the location of the
		                       Record (relative to other Records).


  Returns:  true if the Record was found, false otherwise.
----------------------------------------------------------------------------------------*/
bool CTlvContainerParser::FindRecord(DWORD Tag, const CTlv::Header_t ** ppRecord, int * pRecordLocation /*= NULL*/)
{
	ObjectFind_t OriginalFind;
	const CTlv::Header_t * pRecord;
	bool bTagFound = false;
	int NumObjects = 1;

	OriginalFind = m_ObjectFind;
	pRecord = GetFirstRecord();
	while ( pRecord && ! bTagFound )
	{
		if ( CTlv::GetTag(pRecord) == Tag )
		{
			*ppRecord = pRecord;
			if ( pRecordLocation ) *pRecordLocation = NumObjects;
			bTagFound = true;
		}
		else
		{
			pRecord = GetNextRecord();
			NumObjects++;
		}
	}
	m_ObjectFind = OriginalFind;

	return bTagFound;
}


/*----------------------------------------------------------------------------------------
GetFirstRecord

  This gets the first Record in the TLV container.  You then call GetNextRecord() to get the
  remaining Records.  This MUST be called before you call GetNextRecord().


  Parameters: none


  Returns:  pointer to a Record (there's always at least one.
----------------------------------------------------------------------------------------*/
const CTlv::Header_t * CTlvContainerParser::GetFirstRecord(void)
{
	assert(m_pTlvContainer);

	m_ObjectFind.pRecord = (m_pTlvContainer + 1);
	m_ObjectFind.NumBytesLeftForRecords = CTlv::GetLength(m_pTlvContainer);

	return m_ObjectFind.pRecord;
}




/*----------------------------------------------------------------------------------------
GetNextRecord

  This gets the next Record in the TLV Container.  You MUST have called GetFirstRecord() first.


  Parameters: none


  Returns:  pointer to a Record or NULL if we reached the end of the TLV Container.  
----------------------------------------------------------------------------------------*/
const CTlv::Header_t * CTlvContainerParser::GetNextRecord(void)
{
	assert(m_ObjectFind.pRecord);

	// remember, m_ObjectFind.pRecord is the LAST object returned--we need to skip over it
	int NumBytesInRecord = sizeof(CTlv::Header_t) + CTlv::GetLength(m_ObjectFind.pRecord) + CalcNumPaddingBytes(m_ObjectFind.pRecord);
	if ( m_ObjectFind.NumBytesLeftForRecords > NumBytesInRecord )
	{
		m_ObjectFind.pRecord = (const CTlv::Header_t *) ((const unsigned char *) m_ObjectFind.pRecord + NumBytesInRecord);
		m_ObjectFind.NumBytesLeftForRecords -= NumBytesInRecord;
		return m_ObjectFind.pRecord;
	}
	else return NULL;
}



