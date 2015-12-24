/*=================================================================================
  class: CMakeBreak

//=================================================================================
   $Source: interfaces_wedge/wedgemakebreak.h $
   $Date: 2009/06/29 05:03:41EDT $
   $Revision:


//=================================================================================*/

#ifndef WEDGE_MAKE_BREAK_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define WEDGE_MAKE_BREAK_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#include "WdgType.h"

/******************************************************************************/
/* Make break definitions																		*/
/******************************************************************************/

#define BR_Pre			0x01		/* Key uses a pre byte to break. Example '0xF0 scancode' for IBM AT */
#define BR_HighBit	0x02		/* Key uses the high bit to flag the break. Example Make = 0x12, Break = 0x92 */
#define BR_MakeOnly	0x04		/* Key does not need to be broken */
#define BR_BreakOnly	0x08		/* Trick to support the release of keys previously pressed */
#define BR_All			0x10		/* Key uses the "All keys up" break code */
#define BR_SpecialParity 0x20		/* Key uses a parity trick to make/break. One parity make. the other break the key */
#define BR_Default	0x80     /* For debugging of forkey, will be removed soon */


/*
	Pressing a key is called Make, releasing a key is called Break.
	There are several different methods to break a key.
	This class handles them all.
	Used to be Wedge_MakeBreak in the IT3800.
*/

class CMakeBreak
{
public:
// access helpers
	scancode_t	GetPreByte(void)	const { return m_prebyte;	};
	mkbrtype_t	GetType(void)		const { return m_type;		};

// data members
   scancode_t 		m_prebyte;
   mkbrtype_t 		m_type;
};

class CMakeBreaks
{
public:
// access helpers
	scancode_t	GetPreByte(keynum_t keynum) const	{ return m_keys[keynum].GetPreByte();	};
	mkbrtype_t	GetType(keynum_t keynum)	 const	{ return m_keys[keynum].GetType();		};
// constants
	enum
	{
		NUM_MAKE_BREAKS =	150
	};
// data members
	CMakeBreak		m_keys[NUM_MAKE_BREAKS];
};

#endif // WEDGE_MAKE_BREAK_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_




