/*=================================================================================
  class: CWedgeCmd

//=================================================================================
   $Source: interfaces_wedge/wedgekeynumber.h $
   $Date: 2011/11/14 16:48:54EST $
   $Revision: 1.9 $



//=================================================================================*/

#ifndef WEDGE_KEY_NUMBER_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define WEDGE_KEY_NUMBER_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#include "WdgType.h"
#include "WedgeKey.h"

class CChar2Key;

class CWedgeKeyNumber
{
public:
	CWedgeKeyNumber();
	virtual ~CWedgeKeyNumber();

	void Initialize(const CChar2Key *pUsa);
	void InitializeCtrlASCII(const CChar2Key *pUsa);
   void StoreKey(tichar_t character, keynum_t keynumber, modifiers_t flags);

	enum
	{
		#ifdef UNICODE_FOR_INTERFACES
		NumChars = 0x10000
		#else
		NumChars = 256
		#endif
	};

	CKey *GetKey(tichar_t character);
   keynum_t GetKeyNumber(tichar_t character);

protected:
	CKey	*m_pKeys;
	
private:
	CWedgeKeyNumber (const CWedgeKeyNumber&);            // declared private, but not defined, to prevent copying         
	CWedgeKeyNumber& operator=(const CWedgeKeyNumber&);  // declared private, but not defined, to prevent copying 

};

#endif // WEDGE_KEY_NUMBER_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_




