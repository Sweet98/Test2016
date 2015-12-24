/*=================================================================================
	class: CTransInfoSystemCP
	Handler for system codepage to/from Unicode
//=================================================================================
   $Source: Gen6FoundationApp/FoundationApp/eci/codepagesystemcp.h $
   $Date: 2009/03/05 14:45:46EST $
   $Revision:


//=================================================================================*/
//! \file

#ifndef TRANSCODE_SYSTEM_CP_H__03164A21_6A2C_443B_BCD9_B1B934C58593__INCLUDED_
#define TRANSCODE_SYSTEM_CP_H__03164A21_6A2C_443B_BCD9_B1B934C58593__INCLUDED_

#include "TransCode.h"

//////////////////////////////////////////////////////////////////////////////////////////
/// Handler for the 8 Bit codepages.
/*!
*/
class CTransInfoSystemCP : public CTransInfoBase
{
public:
	CTransInfoSystemCP(codepage_t codepage, const wchar_t *pSrc, size_t SrcTableSize);

protected:
	virtual MatrixError_t FromUnicode(const wchar_t *pIn, bchar_t *pOut, size_t &DestinationSize);
	virtual MatrixError_t FromUnicode(const wchar_t *pIn, size_t length, bchar_t *pOut, size_t &DestinationSize);

	void FromUnicode_(wchar_t In, bchar_t &Out);

	bchar_t m_ToSystemCP_Table[CP16_SIZE];	// translate back from Unicode
};

/////////////////////////////////////////////////////////////////////////////


#endif // TRANSCODE_SYSTEM_CP_H__03164A21_6A2C_443B_BCD9_B1B934C58593__INCLUDED_


