/*=================================================================================
  class: CTerminal

//=================================================================================
   $Source: interfaces_wedge/wedgeterminal.h $
   $Date: 2009/06/29 05:03:51EDT $
   $Revision: 1.5 $


//=================================================================================*/

#ifndef WEDGE_TERMINAL_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define WEDGE_TERMINAL_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#include "WdgType.h"
#include "WedgeScancode.h"
#include "WedgeMakeBreak.h"
#include "WedgeKey.h"
#include "WedgeFixup.h"
#include "WedgeKeyNumber.h"
#include "WedgeChar2Key.h"

/* Capabilities for terminals */
#define CAP_CNTRL_MSDOS					0x0001
#define CAP_CNTRL_DECVT					0x0002
#define CAP_ALT_NUMPAD_DOS				0x0004
#define CAP_ALT_NUMPAD_WIN				0x0008
#define CAP_COMPOSE_NUMPAD_HEX		0x0010
#define CAP_COMPOSE_NUMPAD_DECIMAL	0x0020
#define CAP_SUPPORT_KEYEMU				0x0040
#define CAP_NEED_AUTO_KEYEMU			0x0080
#define CAP_STICKY_SHIFTLEFT			0x0100
#define CAP_STICKY_SHIFTRIGHT			0x0200
#define CAP_STICKY_CNTRLLEFT			0x0400
#define CAP_STICKY_CNTRLRIGHT			0x0800
#define CAP_STICKY_ALTLEFT				0x1000
#define CAP_STICKY_ALTRIGHT			0x2000
#define CAP_CNTRL_WIN					0x4000
#define CAP_ALT_NUMPAD_UNI				0x8000

#define CAP_SUPPORT_DIRCON				CAP_SUPPORT_KEYEMU  	/* be compatible with old sources */
#define CAP_NEED_AUTO_DIRCON			CAP_NEED_AUTO_KEYEMU

class CWedgeTerminal
{
public:
// access helpers
	const CChar2Key *GetLayout(void)	const 	{ return m_pLayout;	}
	const CKeyFixups *GetFixups(void) const 	{ return pCountryTable;	}

	bool IsCapCntrlDOS(void) const { return m_capabilities&CAP_CNTRL_MSDOS ? true : false; };
	bool IsCapCntrlWin(void) const { return m_capabilities&CAP_CNTRL_WIN	  ? true : false; };
	bool IsCapCntrlDEC(void) const { return m_capabilities&CAP_CNTRL_DECVT ? true : false; };
	bool IsCapAltNpWin(void) const { return m_capabilities&CAP_ALT_NUMPAD_WIN ? true : false; };
	bool IsCapAltNpDos(void) const { return m_capabilities&CAP_ALT_NUMPAD_DOS ? true : false; };
	bool IsCapAltNpUni(void) const { return m_capabilities&CAP_ALT_NUMPAD_UNI ? true : false; };

	keynum_t GetModifierKeyNumber(int index)	const { return m_modifierkeys[index];	}

// constants
	enum
	{
		NUM_TOTAL_MOD_KEYS = 10,
		NUM_USED_MOD_KEYS  =  6
	};
	enum
	{
	/*
		Indexes use to access the keynumbers of certain keys
		in the terminal properties table (Wedge_Terminal)
	*/
		INDEX_LEFT_SHIFT	= 0,
		INDEX_RIGHT_SHIFT	= 1,
		INDEX_LEFT_CNTRL	= 2,
		INDEX_RIGHT_CNTRL	= 3,
		INDEX_LEFT_ALT  	= 4,
		INDEX_RIGHT_ALT   = 5,
		INDEX_LEFT_COMPOSE= 6,
		INDEX_RIGHT_COMPOSE=7,
		INDEX_CAPS_LOCK 	= 8,
		INDEX_NUM_LOCK   	= 9
	};
// data members
	unsigned short version;
	int		m_capabilities;
	const 	CChar2Key	*m_pLayout;
	const 	CKeyFixups	*pCountryTable;
	const 	CScancodes	*m_pScancodes;
	const 	CMakeBreaks	*m_pMakebreak;
	short		m_delay_short;
	short		m_delay_medium;
	long		m_delay_long;
	short		m_delay_intercharacter;
	keynum_t	m_modifierkeys[NUM_TOTAL_MOD_KEYS];
};



#endif // WEDGE_TERMINAL_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_




