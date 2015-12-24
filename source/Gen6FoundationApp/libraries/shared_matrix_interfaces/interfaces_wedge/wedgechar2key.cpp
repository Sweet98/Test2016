/*=================================================================================
  class: CChar2Key

//=================================================================================
   $Source: interfaces_wedge/wedgechar2key.cpp $
   $Date: 2009/06/29 05:03:08EDT $
   $Revision: 1.5 $



//=================================================================================*/

#include "stdInclude.h"
#include "CommonDriver.h"
#include "WedgeBase.h"
#include "WedgeCmd.h"
#include "WedgeChar2Key.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool CChar2Key::GetKeyDetails(tichar_t character, keynum_t &keynum, modifiers_t &modifier) const
{
	if (character <= m_size)
	{
		m_keynumbers[character].GetKeyDetails(keynum, modifier);
		return TRUE;
	}
	return FAIL;
}

bool CChar2Key::GetNumPadKeyDetails(tichar_t character, keynum_t &keynum, modifiers_t &modifier) const
{
	if (m_pNumPadKeyNumbers != NULL)
	{
		if ((character >= '0') && (character <= '9'))
		{
			keynum = m_pNumPadKeyNumbers[character-'0'];
			modifier = 0;
			return TRUE;
		}
	}
	return FALSE;
}



