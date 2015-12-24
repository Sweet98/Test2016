/*=================================================================================
  class: CChar2key

//=================================================================================
   $Source: interfaces_wedge/wedgechar2key.h $
   $Date: 2009/06/29 05:03:09EDT $
   $Revision: 1.6 $



//=================================================================================*/

#ifndef WEDGE_CHAR_2_KEY_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define WEDGE_CHAR_2_KEY_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#include "WdgType.h"

#define NUM_7BIT_CHAR		128

/*
	This class stores information used to setup the "character to keynumber" table.
	It describes the layout of the keyboard.
	Used to be the struct "Wedge_Layout" (wdgtable.h) in the IT3800.
*/
class CChar2Key
{
public:
// access helpers
	bool GetKeyDetails(tichar_t character, keynum_t &keynum, modifiers_t &modifier) const;
	bool GetNumPadKeyDetails(tichar_t character, keynum_t &keynum, modifiers_t &modifier) const;
	size_t GetSize(void)						{ return m_size;	}
	bool IsNumPadSupported(void) const 	{ return m_pNumPadKeyNumbers!=NULL;	}

// data members
	const keynum_t	*m_pNumPadKeyNumbers;
   int			m_size;
	SKey			m_keynumbers[NUM_7BIT_CHAR];
};



#endif // WEDGE_CHAR_2_KEY_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_




