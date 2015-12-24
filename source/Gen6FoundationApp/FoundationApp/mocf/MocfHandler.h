/*===================================================================
 See .cpp file for class description.
=====================================================================
   $Source: Gen6FoundationApp/FoundationApp/mocf/MocfHandler.h $
   $Date: 2010/03/17 10:24:43EDT $
   $Revision: 1.5 $


===================================================================*/
#ifndef _MocfHandler_h_
#define _MocfHandler_h_

#include "Gen6ObjectParser.h"



class CGen6MocfHandler : public CGen6MocfParser
{
public:
	CGen6MocfHandler(const CTlv::Header_t * pTlvContainer) : CGen6MocfParser(pTlvContainer){}
	bool IsThereRoom(void);
	bool StoreObjects(void);
	bool IsCompatible(DWORD CompatibleProdId);
	
protected:
	
	bool IsFwCompatible(const CTlv::Header_t * pRecord, DWORD CompatibleProdId);
};


#endif // endof #ifndef _MocfHandler_h_



