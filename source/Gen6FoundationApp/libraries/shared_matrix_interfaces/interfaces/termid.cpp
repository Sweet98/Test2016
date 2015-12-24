/*=================================================================================
  class: CTermID

//=================================================================================
   $Source: interfaces/termid.cpp $
   $Date: 2011/10/21 16:02:33EDT $
   $Revision:


//=================================================================================*/

#include "stdInclude.h"
#include "product.h"
#include "TermId.h"
#include "if_table.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
	CTermialID maintains the list of avaialable interface drivers.
	It searches both the std. and the custom interface table for a approproiate driver.
	The model info is used to further define the search pattern.

	Only called by the CIoMan.
*/
#ifndef PRODUCT_CONFIGURATION_APPLICATION

extern CInterfaceTable Terminals[];
extern CInterfaceTable CustomTerminals[];
#endif

//Find if the terminalID is supported by this model
const CInterfaceTable *CTermialID::QueryInterface(const CInterfaceTable *pTable, interface_id_t termid, model_t model)
{
#ifndef PRODUCT_CONFIGURATION_APPLICATION
	while(pTable->m_termid != CInterfaceTable::End)
	{
		if(pTable->m_termid == termid)
		{
			if(pTable->m_Models & model)
			{
				return pTable;
			}
		}
		pTable++;
	}
#endif
	return NULL;
}

const CInterfaceTable *CTermialID::QueryInterface(interface_id_t termid, model_t model)
{
#ifndef PRODUCT_CONFIGURATION_APPLICATION

	const CInterfaceTable *pTable = QueryInterface(CustomTerminals, termid, model);	// first search in customized table
	if(pTable == (CInterfaceTable*)NULL)
		pTable = QueryInterface(Terminals, termid, model);
	return pTable;
#endif

}

MatrixError_t CTermialID::GetDetails(interface_id_t termid, model_t model, USHORT &usType, CDriverBase *(**const ppClassFactory)(void))
{
	const CInterfaceTable *pTable = QueryInterface(termid, model);
	if(pTable != NULL)
	{
		usType = pTable->m_Type;
		*ppClassFactory = pTable->pClassFactory;
		return ERR_PASS;
	}
	return ERR_NO_SUCH_INTERFACE;
}


/*----------------------------------------------------------------------------------------
RemoveInterface

  This removes an interface by setting its m_Models variable to None.

  Parameters:  none
		TermId[in]:  the term ID of the interface to remove

  Returns:  nothing
----------------------------------------------------------------------------------------*/
void CTermialID::RemoveInterface(int TermId)
{
	CInterfaceTable *pTable = Terminals;
	while(pTable->m_termid != CInterfaceTable::End)
	{
		if(pTable->m_termid == TermId)
		{
			pTable->m_Models = CInterfaceTable::None;
			return;
		}
		pTable++;
	}
}





