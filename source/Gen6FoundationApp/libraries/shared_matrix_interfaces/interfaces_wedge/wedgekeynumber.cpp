/*=================================================================================
  class: CWedgeCmd

//=================================================================================
   $Source: interfaces_wedge/wedgekeynumber.cpp $
   $Date: 2009/08/26 21:47:25EDT $
   $Revision: 1.7 $



//=================================================================================*/

#include "stdInclude.h"
#include "CommonDriver.h"
#include "WedgeKeyNumber.h"
#include "WedgeKey.h"
#include "WedgeChar2Key.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CWedgeKeyNumber::CWedgeKeyNumber()
{
	m_pKeys = new CKey[NumChars];
}

CWedgeKeyNumber::~CWedgeKeyNumber()
{
	delete [] m_pKeys;
}

void CWedgeKeyNumber::Initialize(const CChar2Key *pUsa)
{
	/* prepare the character to keynumber translation table  */
	ASSERT(m_pKeys != NULL);
   keynum_t	 		keynum;
	modifiers_t		modifier;
	int i;

	// ASCII range
	for (i=0; i < NUM_7BIT_CHAR; i++)
	{
		if (pUsa->GetKeyDetails(i, keynum, modifier))
      	StoreKey((tichar_t) i, keynum, modifier);
		else
      	StoreKey((tichar_t) i, 0, 0);
	}
	// the rest
	for (; i < NumChars; i++)
	{
      StoreKey((tichar_t) i, 0, 0);
	}
}
void CWedgeKeyNumber::InitializeCtrlASCII(const CChar2Key *pUsa)
{
	/* prepare the character to keynumber translation table  */
	ASSERT(m_pKeys != NULL);
             keynum_t	 	keynum;
	modifiers_t	modifier;
	int i;

	// CtrlASCII range
	for (i=0; i <32; i++)
	{
	      if (pUsa->GetKeyDetails(i, keynum, modifier))
      		StoreKey((tichar_t) i, keynum, modifier);
	     else
      	                 StoreKey((tichar_t) i, 0, 0);
	}
}


/* Store the character to keynumber+modifier information into our ram translation table. */
void CWedgeKeyNumber::StoreKey(tichar_t character, keynum_t keynumber, modifiers_t flags)
{
	m_pKeys[character].m_keynum	= keynumber;
	m_pKeys[character].m_modifier= flags;
}

CKey *CWedgeKeyNumber::GetKey(tichar_t character)
{
	return &m_pKeys[character];
}

keynum_t CWedgeKeyNumber::GetKeyNumber(tichar_t character)
{
	return m_pKeys[character].GetKeyNumber();
}



