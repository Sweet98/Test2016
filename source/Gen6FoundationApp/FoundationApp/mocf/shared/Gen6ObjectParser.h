/*===================================================================
 See .cpp file for class description.

  This code must compile under these OSs:  Microsoft, Linux, Gen6 embedded
=====================================================================
   $Source: Gen6ObjectParser.h $
   $Date: 2011/09/02 17:11:24EDT $
   $Revision: 1.8 $


===================================================================*/
#ifndef _Gen6ObjectParser_h_
#define _Gen6ObjectParser_h_

#include "MocfParser.h"


class CGen6MocfParser : public CMocfParser
{
public:

	CGen6MocfParser(const CTlv::Header_t * pTlvContainer) : CMocfParser(pTlvContainer){}
	virtual bool IsValid(void);
	bool IsSystemUpdateMocf(void);

protected:
	bool HasObjectRecords(void);
};


class CGen6ObjectRecordParser : public CTlvContainerParser
{
public:
	CGen6ObjectRecordParser(const CTlv::Header_t * pObjectRecord) : CTlvContainerParser(pObjectRecord){}
	static CGen6ObjectRecordParser * CreateObjectRecordParser(const CTlv::Header_t * pObject);
	virtual bool IsValid(void) = 0;
	virtual bool IsSystemObject(void) = 0;

protected:
};



//			Object Record Parser classes

class CUnkownParser : public CGen6ObjectRecordParser
{
public:
	CUnkownParser(const CTlv::Header_t * pObjectRecord) : CGen6ObjectRecordParser(pObjectRecord){}
	virtual bool IsValid(void) {return true;}
	virtual bool IsSystemObject(void) {return false;}
};


class CNestedMocfParser : public CGen6ObjectRecordParser
{
public:
	CNestedMocfParser(const CTlv::Header_t * pObjectRecord) : CGen6ObjectRecordParser(pObjectRecord){}
	virtual bool IsValid(void) {return false;}
	virtual bool IsSystemObject(void) {return false;}
};


class CFirmwareParser : public CGen6ObjectRecordParser
{
public:
	CFirmwareParser(const CTlv::Header_t * pObjectRecord) : CGen6ObjectRecordParser(pObjectRecord){}
	virtual bool IsValid(void);
	virtual bool IsSystemObject(void) {return false;}
	virtual bool GetFwData(const unsigned char ** ppFwData, DWORD * pLength);
	virtual bool GetSwVersion(const char ** ppSwVersion);
};

// Boot Firmware doesn't need all these parsers & we want to keep it small.  The original intent was to create
// a small parser class for boot firmware and derive a parser class from that one for app (to re-use code).
// Unfortunately, I ran into problems with the static method CreateObjectRecordParser(), so I had to resort to #ifdefs.
#if ! defined(MATRIX_BOOT_CODE)  

class CUserFileParser : public CGen6ObjectRecordParser
{
public:
	CUserFileParser(const CTlv::Header_t * pObjectRecord) : CGen6ObjectRecordParser(pObjectRecord){}
	virtual bool IsValid(void);
	virtual bool IsSystemObject(void) {return false;}
};



class CExecuteMenuSettingsParser : public CGen6ObjectRecordParser
{
public:
	CExecuteMenuSettingsParser(const CTlv::Header_t * pObjectRecord) : CGen6ObjectRecordParser(pObjectRecord){}
	virtual bool IsValid(void);
	virtual bool IsSystemObject(void) {return false;}
	virtual const char * GetData(void) {return (const char *) CTlv::GetData(m_pTlvContainer);}

protected:
};



class CMenuSettingsParser : public CGen6ObjectRecordParser
{
public:
	CMenuSettingsParser(const CTlv::Header_t * pObjectRecord) : CGen6ObjectRecordParser(pObjectRecord){}
	virtual bool IsValid(void);
	virtual bool IsSystemObject(void) {return true;}
	virtual const char * GetData(void) {return (const char *) CTlv::GetData(m_pTlvContainer);}

protected:
	virtual char GetMyTerminator(void) {return '.';}
};



class CCustomDefMenuSettingsParser : public CMenuSettingsParser
{
public:
	CCustomDefMenuSettingsParser(const CTlv::Header_t * pObjectRecord) : CMenuSettingsParser(pObjectRecord){}
	virtual bool IsSystemObject(void) {return false;}

protected:
	virtual char GetMyTerminator(void) {return '@';}
};



class CSwKeysParser : public CMenuSettingsParser
{
public:
	CSwKeysParser(const CTlv::Header_t * pObjectRecord) : CMenuSettingsParser(pObjectRecord){}
	virtual bool IsSystemObject(void) {return true;}
};



class CFileNameParser : public CGen6ObjectRecordParser
{
public:
	CFileNameParser(const CTlv::Header_t * pObjectRecord) : CGen6ObjectRecordParser(pObjectRecord){}
	virtual bool IsValid(void);
	virtual bool IsSystemObject(void) {return false;}
	const char * GetData(void) {return (const char *) CTlv::GetData(m_pTlvContainer);}
};


#endif  // endof #if ! defined(MATRIX_BOOT_CODE)


//		These are parsers INSIDE the above Gen6 Object Records

class CSwVersionParser : public CGen6ObjectRecordParser
{
public:
	CSwVersionParser(const CTlv::Header_t * pObjectRecord) : CGen6ObjectRecordParser(pObjectRecord){}
	virtual bool IsValid(void) {return true;}
	virtual bool IsSystemObject(void) {return false;}
	const char * GetData(void) {return (const char *) CTlv::GetData(m_pTlvContainer);}
};



class CDataParser : public CGen6ObjectRecordParser
{
public:
	CDataParser(const CTlv::Header_t * pObjectRecord) : CGen6ObjectRecordParser(pObjectRecord){}
	virtual bool IsValid(void) {return true;}
	virtual bool IsSystemObject(void) {return false;}
	const unsigned char * GetData(void) {return (const unsigned char *) CTlv::GetData(m_pTlvContainer);}
};







#endif // endof #ifndef _Gen6ObjectParser_h_




