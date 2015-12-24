/*===================================================================
  See source file for description

  This code must compile under these OSs:  Microsoft, Linux, Gen6 embedded
=====================================================================
   $Source: TlvContainerParser.h $
   $Date: 2011/10/14 14:53:52EDT $
   $Revision: 1.3 $


===================================================================*/
#ifndef _TlvContainerParser_h_
#define _TlvContainerParser_h_

#include "IntSizes.h"


// Helper class.
class CTlv
{
public:
	typedef struct
	{
		DWORD Tag;
		DWORD Length;
	} Header_t;
	
	static DWORD GetTag(const Header_t * pRecord);
	static DWORD GetLength(const Header_t * pRecord);
	static const unsigned char * GetData(const Header_t * pRecord);
};



class CTlvContainerParser
{
public:
	CTlvContainerParser(const CTlv::Header_t * pTlvContainer) {m_pTlvContainer = pTlvContainer;}
	virtual ~CTlvContainerParser(void) {};

	int GetContainerSize(void); 
	int GetLength(void); 
	virtual bool IsValid(void);
	static int CalcNumPaddingBytes(const CTlv::Header_t * pRecordData);
	bool FindRecord(DWORD Tag, const CTlv::Header_t ** ppRecord, int * pRecordLocation = NULL);
	const CTlv::Header_t * GetFirstRecord(void);
	const CTlv::Header_t * GetNextRecord(void);

protected:
	const CTlv::Header_t * m_pTlvContainer;

	typedef struct
	{
		const CTlv::Header_t * pRecord;
		int NumBytesLeftForRecords;
	} ObjectFind_t;
	
	ObjectFind_t m_ObjectFind;
};


#endif // endof #ifndef _TlvContainerParser_h_




