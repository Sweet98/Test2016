/*=================================================================================
  class: CKeyFixup

//=================================================================================
   $Source: interfaces_wedge/wedgefixup.h $
   $Date: 2009/06/29 05:03:30EDT $
   $Revision: 2.1 $


//=================================================================================*/

#ifndef WEDGE_KEY_FIXUPS_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define WEDGE_KEY_FIXUPS_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#include "WdgType.h"

/******************************************************************************/
/* International support definitions														*/
/******************************************************************************/
/*
	This describes exceptions to the USA key translations.
	The character and the key with it's number and modifier (shift,cntrl...)
	are used to change the "character to keynuber" table.
*/
struct CKeyFixup
{
   tichar_t 		m_character;
   keynum_t	 		m_keynum;
	modifiers_t		m_modifier;
//	attributes_t	attribute;
};

/*
	This structure contains details about the countries supported by a terminal.
*/
struct CKeyFixups
{
// access helpers
// constants
#if 0
	enum
	{
	};
#endif

// data members
	const  struct CKeyFixups *m_pParent;	// to save memory size, the tables can use similar terminals first.
	const  struct CKeyFixup **m_pCountries;
	short	m_MinCountry;
	short	m_MaxCountry;
};

#endif // WEDGE_KEY_FIXUPS_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_




