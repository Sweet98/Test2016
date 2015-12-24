/*===================================================================
 See .cpp file for class description.
 =====================================================================
   $Source: Gen6FoundationApp/FoundationApp/mocf/Gen6ObjectHandler.h $
   $Date: 2011/09/02 17:14:40EDT $
   $Revision: 1.8 $


===================================================================*/
#ifndef _Gen6ObjectHandler_h_
#define _Gen6ObjectHandler_h_

#include "MocfParser.h"


class CGen6ObjectRecordHandler
{
public:
	CGen6ObjectRecordHandler(const CTlv::Header_t * pObject) {m_pObject = pObject;}

	static CGen6ObjectRecordHandler * CreateObjectRecordHandler(const CTlv::Header_t * pObject);
	virtual int GetSpaceNeededToStore(void) = 0;
	virtual bool StoreObject(void) = 0;

protected:
	const CTlv::Header_t * m_pObject;
};


class CUnknownHandler : public CGen6ObjectRecordHandler
{
public:
	CUnknownHandler(const CTlv::Header_t * pObjec) : CGen6ObjectRecordHandler(pObjec){}
	virtual bool StoreObject(void) {return true;}
	virtual int GetSpaceNeededToStore(void) {return 0;}

protected:
};



class CAppFirmwareHandler : public CGen6ObjectRecordHandler
{
public:
	CAppFirmwareHandler(const CTlv::Header_t * pObjec) : CGen6ObjectRecordHandler(pObjec){}
	virtual bool StoreObject(void);
	virtual int GetSpaceNeededToStore(void);

protected:
};


class CBootFirmwareHandler : public CGen6ObjectRecordHandler
{
public:
	CBootFirmwareHandler(const CTlv::Header_t * pObjec) : CGen6ObjectRecordHandler(pObjec){}
	virtual bool StoreObject(void);
	virtual int GetSpaceNeededToStore(void);

protected:
};


class CExecuteMenuSettingsHandler : public CGen6ObjectRecordHandler
{
public:
	CExecuteMenuSettingsHandler(const CTlv::Header_t * pObjec) : CGen6ObjectRecordHandler(pObjec){}
	virtual bool StoreObject(void);
	virtual int GetSpaceNeededToStore(void) {return 0;} // Technically, we could increase the space, but it should be small and not worth the trouble to calculate

protected:
};


class CMenuSettingsHandler : public CGen6ObjectRecordHandler
{
public:
	CMenuSettingsHandler(const CTlv::Header_t * pObjec) : CGen6ObjectRecordHandler(pObjec){}
	virtual bool StoreObject(void);
	virtual int GetSpaceNeededToStore(void);

protected:
};


class CCustomDefMenuSettingsHandler : public CGen6ObjectRecordHandler
{
public:
	CCustomDefMenuSettingsHandler(const CTlv::Header_t * pObjec) : CGen6ObjectRecordHandler(pObjec){}
	virtual bool StoreObject(void);
	virtual int GetSpaceNeededToStore(void);

protected:
};


class CSwKeysHandler : public CGen6ObjectRecordHandler
{
public:
	CSwKeysHandler(const CTlv::Header_t * pObjec) : CGen6ObjectRecordHandler(pObjec){}
	virtual bool StoreObject(void);
	virtual int GetSpaceNeededToStore(void);

protected:
};


class CUserFileHandler : public CGen6ObjectRecordHandler
{
public:
	CUserFileHandler(const CTlv::Header_t * pObjec) : CGen6ObjectRecordHandler(pObjec){}
	virtual bool StoreObject(void);
	virtual int GetSpaceNeededToStore(void);

protected:
};



#endif // endof #ifndef _Gen6ObjectHandler_h_


