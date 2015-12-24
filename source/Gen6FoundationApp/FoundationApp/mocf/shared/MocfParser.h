/*===================================================================
  See source file for description

  This code must compile under these OSs:  Microsoft, Linux, Gen6 embedded
=====================================================================
   $Source: MocfParser.h $
   $Date: 2011/10/14 14:53:52EDT $
   $Revision: 1.6 $


===================================================================*/
#ifndef _MocfParser_h_
#define _MocfParser_h_

#include "TlvContainerParser.h"
#include "sha1.h"





class CMocfParser : public CTlvContainerParser
{
public:
	CMocfParser(const CTlv::Header_t * pTlvContainer) : CTlvContainerParser(pTlvContainer){}
	virtual bool IsValid(void);
	bool CalcHash(unsigned char * pHash);
	static bool IsObjectRecord(const CTlv::Header_t * pRecord); 

protected:
	CSha1 m_Sha;

	int HashNextRecord(const CTlv::Header_t * pRecord);
};


// Helper classes for parsing.

class CMocfReocrdParser
{
public:
	CMocfReocrdParser(const CTlv::Header_t * pRecord) {m_pObject = pRecord;}
	virtual ~CMocfReocrdParser(void) {};
	virtual bool IsValid(void) = 0;
	
protected:
	const CTlv::Header_t * m_pObject;
};

class CMocfVersionParser : public CMocfReocrdParser
{
public:
	CMocfVersionParser(const CTlv::Header_t * pObjec) : CMocfReocrdParser(pObjec){}
	enum {MajorVersion = 0, MinorVersion = 0};
	virtual bool IsValid(void);

protected:
};

class CMocfHashParser : public CMocfReocrdParser
{
public:
	CMocfHashParser(const CTlv::Header_t * pObjec) : CMocfReocrdParser(pObjec){}
	virtual bool IsValid(void);
	const unsigned char * GetHash(void);
};


class CMocfCompatProdParser : public CMocfReocrdParser
{
public:
	CMocfCompatProdParser(const CTlv::Header_t * pObjec) : CMocfReocrdParser(pObjec){}
	virtual bool IsValid(void);
	bool FindId(DWORD id);
	
protected:
	DWORD GetNumItems(void);
};


#endif // endof #ifndef _MocfParser_h_




