/*=================================================================================
  class: CTermID

//=================================================================================
   $Source: interfaces/termid.h $
   $Date: 2011/10/21 16:02:33EDT $
   $Revision:


//=================================================================================*/

#ifndef TERMID_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define TERMID_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#include "InterfaceTypedefs.h"

class CTermialID
{
public:
	static const CInterfaceTable *QueryInterface(interface_id_t termid, model_t model);
	static MatrixError_t GetDetails(interface_id_t termid, model_t model, USHORT &usType, CDriverBase *(**const ppClassFactory)(void));
	static void RemoveInterface(int TermId);
private:
	static const CInterfaceTable *QueryInterface(const CInterfaceTable *pTable, interface_id_t termid, model_t model);
};

#endif // TERMID_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_




