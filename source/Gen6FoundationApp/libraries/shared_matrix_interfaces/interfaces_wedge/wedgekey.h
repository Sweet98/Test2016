/*=================================================================================
  class: CKey

//=================================================================================
   $Source: interfaces_wedge/wedgekey.h $
   $Date: 2009/06/29 05:03:35EDT $
   $Revision:


//=================================================================================*/

#ifndef WEDGE_KEY_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define WEDGE_KEY_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#include "WdgType.h"

#define KN_ENDMARKER			255	/* Used to mark the end of exception tables in the international support */
#define KN_PROKEY_SEQUENCE 254	/* Used to mark a (prokey) sequence in the ANSI to keynumber table */

/* Bit flags for storing infomation about the modifier keys */
#define MO_ShiftLeft		0x01
#define MO_ShiftRight	0x02
#define MO_AltLeft		0x04
#define MO_AltRight		0x08
#define MO_ControlLeft	0x10
#define MO_ControlRight	0x20
//#define MO_ComposeLeft	0x40
//#define MO_ComposeRight	0x80
#define MO_DeadKey		0x40		/* This character has the dead key characteristic */
//#define MO_freeyet	0x80


/*
	This describes a key with it's number and modifier (shift,cntrl...).
	Historically the name means "ASCII to Keynumber". But we use 8 bit characters
	now, so a better name would be something with ANSI. Soon we might change this
	to support Unicode (16-bit characters). So we keep this name.
*/
// used to be ascii_kn in IT3800

// This one is used for creating tables of keys in ROM
struct SKey
{
   keynum_t	 		m_keynum;
	modifiers_t		m_modifier;
//	attributes_t	attribute;

	void GetKeyDetails(keynum_t &keynum, modifiers_t &modifier) const
	{
		keynum  = m_keynum;
		modifier= m_modifier;
	}
};

// This one is used at run time
class CKey : public SKey
{
public:
	CKey();
	CKey(keynum_t keynum, modifiers_t modifier);
//
	keynum_t GetKeyNumber(void) const
	{
		return m_keynum;
	}
	bool IsDeadKey(void) const
	{
		return m_modifier&MO_DeadKey ? true : false;
	}
	bool IsShifted(void) const
	{
		return m_modifier&(MO_ShiftLeft|MO_ShiftRight) ? true : false;
	}

	bool IsModifierRequired(modifiers_t modifier) const
	{
		return m_modifier&modifier ? true : false;
	}

protected:

};




#endif // WEDGE_KEY_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_




