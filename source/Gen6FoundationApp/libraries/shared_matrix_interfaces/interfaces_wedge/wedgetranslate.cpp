/*=================================================================================
  class: CWedgeTranslate
	This class knows how to create the make/break sequences.
//=================================================================================
   $Source: interfaces_wedge/wedgetranslate.cpp $
   $Date: 2009/07/01 13:39:21EDT $
   $Revision: 1.7 $



//=================================================================================*/

#include "stdInclude.h"
#include "CommonDriver.h"
//#include "wdgtype.h"
#include "WedgeTranslate.h"
#include "WedgeCmd.h"
#include "WedgeCntrlKeys.h"
#include "WedgeTerminal.h"
#include "ParseHelper.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
// Constructor
CWedgeTranslate::CWedgeTranslate(const CWedgeTerminal *pTerminal) : baseCWedgeTranslate(pTerminal)
{

}

// Destructor
CWedgeTranslate::~CWedgeTranslate()
{
}
*/

/*
Some keys are causing the terminal to send data to the keyboard. In wedge mode
we need to wait until the keyboard and the terminal are finished with it communication.
An example is the shift key on PC keyboards. In some countries the shift key clears
the caps lock which causes the PC to turn off the caps lock LED on the keyboard.
*/
bool CWedgeTranslate::IsCommunicationKey(keynum_t keynum)
{
	if (WdgIsKeyEmu())		/* In keyboard emulation mode we do not need the waiting times */
		return FALSE;
	else
		return m_pScancodes->IsCommunicationKey(keynum);
}


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*
	This is a helper function to break a key. Called by DoBreak.
*/
void CWedgeTranslate::CopyBreakCode(keynum_t keynum, drvparamw_t scancode, drvparamw_t exinfo, long predelay)
{
	mkbrtype_t	type	= m_pMakebreak->GetType(keynum);
	scancode_t	pre	= m_pMakebreak->GetPreByte(keynum);

	if (type&BR_HighBit)
	{
		if (predelay)
			AddDelayCommand(predelay);
		AddScancode((drvparamw_t)(scancode|0x80), (drvparamw_t)(exinfo|EX_BREAK));
	}
	else if (type&BR_Pre)
	{
		if (predelay)
			AddDelayCommand(predelay);
		AddScancode(pre, (drvparamw_t)(exinfo|EX_PRE_BREAK));
		AddDelayCommand(GetShortDelay());
		AddScancode(scancode, (drvparamw_t)(exinfo|EX_BREAK));
	}
	else if (type&BR_All)
	{
		if (predelay)
			AddDelayCommand(predelay);
		AddScancode(pre, (drvparamw_t)(exinfo|EX_BREAK));
	}
}

/*
	Some keys need a series of bytes.
	Here we handle such keys.
	For luck it's only a few keys.
*/
void CWedgeTranslate::CopySequence(keynum_t keynum)
{
	int i;
	const scancode_t *pSeqByte;
	const CScancodeSequence *pSec = m_pScancodes->GetSequence(keynum);
	assert (pSec != NULL);
	if (pSec != NULL)
	{
		pSeqByte = pSec->GetBytes();
		for (i=0; i<pSec->GetSize(); i++)
		{
			AddScancode(*pSeqByte++, (drvparamw_t)(EX_SEQUENCE|keynum));
			AddMediumDelayCommand();
		}
	}
}
/*****************************************************************************/

/* Press (make) a key */
void CWedgeTranslate::DoMake(keynum_t keynum, drvparamw_t exinfo)
{
	int type;
	drvparamw_t scancode;
	if (keynum > 0)
	{
		exinfo |= keynum;		/* Store the keynumber into the extra info for the bitbanger */
		if (!(BR_BreakOnly & m_pMakebreak->GetType(keynum)))
		{
			type = m_pScancodes->GetScancodeType(keynum);
			if (type == CScancodes::REGULAR_SCANCODE)
			{
				scancode = m_pScancodes->GetScancode(keynum);
				AddScancode(scancode, (drvparamw_t)(exinfo|EX_MAKE));
			}
			else if (type == CScancodes::USE_EXTENDED_SCANCODE)
			{
				AddScancode(m_pScancodes->GetExtensionByte(), (drvparamw_t)(exinfo|EX_EXTENDED|EX_MAKE));
				AddMediumDelayCommand();
				scancode = m_pScancodes->GetScancode(keynum);
				AddScancode(scancode, (drvparamw_t)(exinfo|EX_MAKE));
			}
			else if (type == CScancodes::USE_SEQUENCE)
			{
				CopySequence(keynum);
			}
//88888888888888888888888888888 need to fix that, only used for Citos K5
//			else if (type == CScancodes::ADD_PARITY)
//			{
//				scancode = m_pScancodes->GetScancode(keynum);
//				scancode += CalcParity(scancode, ODD) * 0x0100;
//				AddScancode(scancode, (drvparamw_t)(exinfo|EX_MAKE));
//			}

			if (IsCommunicationKey(keynum))				/* Do we need a long delay? */
				AddDelayCommand(GetLongDelay());
		}
	}
}

/* Release (break) a key */
void CWedgeTranslate::DoBreak(keynum_t keynum, drvparamw_t exinfo, long predelay)
{
	int type;
	drvparamw_t scancode;
	if (keynum > 0)
	{
		exinfo |= keynum;			/* Store the keynumber into the extra info for the bitbanger */
		if (BR_BreakOnly & GetBreakType(keynum))
		{
			predelay = 0;
		}
		type = GetScancodeType(keynum);
		if (type == CScancodes::REGULAR_SCANCODE)
		{
			scancode = (drvparamw_t) m_pScancodes->GetScancode(keynum);
			CopyBreakCode(keynum, scancode, exinfo, predelay);
		}
		else if (type == CScancodes::USE_EXTENDED_SCANCODE)
		{
			if (predelay)
				AddDelayCommand(predelay);

			AddScancode(GetExtensionByte(), (drvparamw_t)(exinfo|EX_EXTENDED|EX_BREAK));
			scancode = (drvparamw_t) m_pScancodes->GetScancode(keynum);
			CopyBreakCode(keynum, scancode, exinfo, GetShortDelay());
		}
		else if ((type == CScancodes::USE_SEQUENCE) && (BR_BreakOnly & GetBreakType(keynum)))
		{	/* Sequences are supported for break-only keys (for tricky things) */
			CopySequence(keynum);
		}
//88888888888888888888888888888 need to fix that, only used for Citos K5
//		else if (type == CScancodes::ADD_PARITY)
//		{
//			scancode = (drvparamw_t) m_pScancodes->GetScancode(keynum);
//
//			CopyBreakCode(keynum, scancode, exinfo, predelay);
//		}

/*		else if (type == USE_SEQUENCE)
		{
		} */
	}
}

/*****************************************************************************/

/* This table contains bits for storing modifier keys */
static const modifiers_t modflags[CWedgeTerminal::NUM_USED_MOD_KEYS] =
{
	MO_ShiftLeft,
	MO_ShiftRight,
	MO_ControlLeft,
	MO_ControlRight,
	MO_AltLeft,
	MO_AltRight,
//	MO_ComposeLeft,
//	MO_ComposeRight,
};

/* This table contains bits for accessing the sticky bits in the terminals capbilities. */
static const short stickyflags[CWedgeTerminal::NUM_USED_MOD_KEYS] =
{
	CAP_STICKY_SHIFTLEFT,
	CAP_STICKY_SHIFTRIGHT,
	CAP_STICKY_CNTRLLEFT,
	CAP_STICKY_CNTRLRIGHT,
	CAP_STICKY_ALTLEFT,
	CAP_STICKY_ALTRIGHT
};

keynum_t CWedgeTranslate::GetModifierKeyNumber(int index)
{
	return 	m_pTerminal->m_modifierkeys[index];
}

bool	CWedgeTranslate::IsSticky(int modkeyindex)
{
	return ((m_pTerminal->m_capabilities & stickyflags[modkeyindex])!=0);
}


/*****************************************************************************/
/* Press (make) a certain required modifier key for this character  */
void CWedgeTranslate::CheckModifierMake(const CKey *key, int modkeyindex)
{
	keynum_t    keynum;
	modifiers_t ModifierBit = modflags[modkeyindex];	/* Lookup the bit in a table */

// do we need to change anything?
	if ( key->IsModifierRequired(ModifierBit) && !IsModifierMade(ModifierBit) )
	{
		keynum = GetModifierKeyNumber(modkeyindex);
		DoMake(keynum, EX_MODIFIER);			/* Press the modifier key */
		StoreModifierMake(ModifierBit);		/* Store that we pressed this one */

		if (!IsNoDelayKey(keynum))						/* */
			if (!IsCommunicationKey(keynum))			/* for keys causing communication to the keyboard we already .. */
				AddDelayCommand(GetMediumDelay());/* .. added a long delay in DoMake. Otherwise add a medium delay. */

		if (IsSticky(modkeyindex))							/* Some terminal´s have sticky modifiers ... */
		{															/* .. so we need to break them immediately.  */
			DoBreak(keynum, EX_MODIFIER, 0);				/* Release the modifier key */
			AddDelayCommand(GetMediumDelay());
		}
	}
}

/* Release (break) a certain modifier key which is pressed, but need to be released for this character  */
void CWedgeTranslate::CheckModifierBreak(const CKey *key, int modkeyindex)
{
	keynum_t    keynum;
	modifiers_t ModifierBit = modflags[modkeyindex];	/* Lookup the bit in a table */

// do we need to change anything?
	if ( !key->IsModifierRequired(ModifierBit) && IsModifierMade(ModifierBit) )
	{
		keynum = GetModifierKeyNumber(modkeyindex);
		if (IsSticky(modkeyindex))							/* Some terminal´s have sticky modifiers ... */
		{															/* .. so we need to make them first.			*/
			DoMake(keynum, EX_MODIFIER);					/* Remake the modifier key */
			AddDelayCommand(GetMediumDelay());
		}
		DoBreak(keynum, EX_MODIFIER, 0);					/* Release the modifier key */
		StoreModifierBreak(ModifierBit);					/* Store that we released this one */
		AddMediumDelayCommand();
	}
}

/*****************************************************************************/
/* Press (make) all required modifier keys for this character */
void CWedgeTranslate::HandleModifierMakes(const CKey *key)
{
	int i;
	for (i=0; i<CWedgeTerminal::NUM_USED_MOD_KEYS; i++)
	{
		CheckModifierMake(key, i);
	}
}

/* Release (break) all modifier keys which are pressed, but need to be released for this character */
void CWedgeTranslate::HandleModifierBreaks(const CKey *key)
{
	int i;
	for (i=0; i<CWedgeTerminal::NUM_USED_MOD_KEYS; i++)
	{
		CheckModifierBreak(key, i);
	}
}

/* This function is called before we send a character.
	First we check if we need to break any modifier key from the previous character.
	Then we check if we need to press any modifier key for the new character.
	In this way we avoid unnecessary breaks and makes of modifier keys.
	Modifier keys are shift, cntrl and alt. */
void CWedgeTranslate::HandleModifier(const CKey *key)
{
	HandleModifierBreaks(key);
	HandleModifierMakes(key);
}

/*****************************************************************************/
bool CWedgeTranslate::TranslateKey(const CKey *key, tichar_t ansi)
{
	keynum_t keynum = key->GetKeyNumber();
	if (keynum > 0)
	{
		HandleModifier(key);
		if (keynum == KN_PROKEY_SEQUENCE)
		{
			ProkeySequence(ansi);
		}
		else
		{  /* regular key */
			DoMake(keynum, EX_NO);
			DoBreak(keynum, EX_NO, GetMediumDelay());
		}
		AddDelayCommand(GetInterCharDelay(ansi));
		return PASS;
	}
	return FAIL;
}





