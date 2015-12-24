/*=================================================================================
  class: CKey
		This class contains details about a certain key of a keyboard
//=================================================================================
   $Source: interfaces_wedge/wedgekey.cpp $
   $Date: 2009/06/29 05:03:34EDT $
   $Revision: 1.4 $



//=================================================================================*/

#include "stdInclude.h"
#include "WedgeKey.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor
CKey::CKey()
{
   m_keynum = 0;
	m_modifier=0;
}

CKey::CKey(keynum_t keynum, modifiers_t modifier)
{
   m_keynum = keynum;
	m_modifier=modifier;
}

// Destructor
//CKey::~CKey()
//{
//}



