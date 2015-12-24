/*=================================================================================
  class: CScancode

//=================================================================================
   $Source: interfaces_wedge/wedgescancode.h $
   $Date: 2009/06/29 05:03:49EDT $
   $Revision:


//=================================================================================*/

#ifndef WEDGE_SCANCODE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define WEDGE_SCANCODE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#include "WdgType.h"

/* type flags for the "keynumber to scancode" tables */
#define KN_NONE				0x00		/* A dummy */
#define KN_EXTENDED			0x01		/* Key uses extended scancodes. example  0xE0 scancode for the IBM AT */
#define KN_SEQUENCE			0x02		/* Key uses a sequence of bytes */
#define KN_COMMUNICATION	0x04		/* Key needs some more delay time to avoid collisions */
//#define KN_notusedyet			0x08		/* can be used later */
#define KN_PARITYVARIATION	0x10		/* Key uses a parity trick to make/break. One parity make. the other break the key */
#define KN_NO_DELAY			0x20		/* Key does not use a translation engine's delay */

/******************************************************************************/
/* scancode definitions																			*/
/******************************************************************************/
/* Some keys just do not fit into any model. For these we can send a sequences of bytes. */
struct CScancodeSequence
{
// access helpers
	int GetSize(void) const
	{
		return m_size;
	}

	const scancode_t *GetBytes(void) const
	{
		return m_scancodes;
	}

	int IsKeyMatch(keynum_t keynum) const
	{
		return (m_key == keynum);
	}

// data members
   UCHAR			 		m_size;
   keynum_t		 		m_key;
   scancode_t	 		m_scancodes[20];
};

/* This holds the scancode and additional information about the key. */
struct CScancode
{
// access helpers
	scancode_t	GetScancode(void)	const	{ return m_scancode;	}
	UCHAR			GetFlags(void)	const 	{ return m_flags;	}

// data members
	scancode_t		m_scancode;
   UCHAR			 	m_flags;
};


/*
	This is the complete "keynumber to scancode" table.
	Used to be struct "kn_table" (wdgtable.h) in the IT3800.
 */

class CScancodes
{
public:
// access helpers
	scancode_t GetIllegalMarker(void) const
	{
		return m_scancodes[0].GetScancode();
	}

	scancode_t GetScancode(keynum_t keynum) const
	{
//		if (keynum > m_numkeys)
//			return GetIllegalMarker();
		return m_scancodes[keynum].GetScancode();
	}

	UCHAR GetFlags(keynum_t keynum) const
	{
//		if (keynum > m_numkeys)
//			return 0;
		return m_scancodes[keynum].GetFlags();
	}

	bool IsCommunicationScancode(scancode_t scancode) const;

	bool IsCommunicationKey(keynum_t keynum) const
	{
		return (GetFlags(keynum) & KN_COMMUNICATION) ? true : false;
	}

	bool IsNoDelayKey(keynum_t keynum) const
	{
		return (GetFlags(keynum) & KN_NO_DELAY) ? true : false;
	}

	scancode_t GetExtensionByte(void)	const
	{
		return m_extension;
	}

	int GetScancodeType(keynum_t keynum) const;
	const CScancodeSequence *GetSequence(keynum_t keynum) const;

	enum
	{
		KEYNUM_BAD				= -2,
		NOTSUPPORTED_SCANCODE= -3,
		USE_SEQUENCE         = -4,
		USE_EXTENDED_SCANCODE= -5,
		REGULAR_SCANCODE		= -6,
		ADD_PARITY				= -7
	};

public:
// data members
	const CScancodeSequence	*m_pSequences;
	unsigned char 	m_numkeys;
   scancode_t	 	m_extension;
	CScancode		m_scancodes[NUMKEYS];
};


/*
	Terminals use different values for an illegal scancode.
	(Most use 0, Apple MAC uses 0xff). So we store this value
	in the "keynumber 0 index". Keynumber 0 is illegal itself,
	so it even fits together.
	GetIllegalMarker() makes it more redable.
*/


#endif // WEDGE_SCANCODE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_




