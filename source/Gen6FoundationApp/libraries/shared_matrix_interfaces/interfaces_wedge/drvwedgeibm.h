/*=================================================================================
  class: CWedgeIBMFamily

//=================================================================================
   $Source: interfaces_wedge/drvwedgeibm.h $
   $Date: 2010/02/11 00:19:43EST $
   $Revision: 1.4 $



//=================================================================================*/

#ifndef WEDGE_IBM_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define WEDGE_IBM_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#include "DrvWedgeBase.h"

class CBarcodeData;
class CBitBanger;

#define baseCWedgeIBMFamily CWedgeDriver
class CWedgeIBMFamily : public baseCWedgeIBMFamily
{
public:
	CWedgeIBMFamily(const CWedgeTerminal *pTerminal, UINT ScancodeSet);
	virtual size_t Write(CBarcodeData  *pLabel);
	virtual size_t Write(const bchar_t *pBin, size_t nSize)
	{
		return CDriverBase::Write(pBin,nSize);
	}
	virtual size_t Write(const wchar_t *pText, size_t nSize)
	{
		return CDriverBase::Write(pText,nSize);
	}
	virtual ~CWedgeIBMFamily();
	

};

#endif // WEDGE_IBM_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_




