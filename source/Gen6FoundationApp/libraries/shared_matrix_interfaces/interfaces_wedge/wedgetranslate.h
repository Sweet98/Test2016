/*=================================================================================
  class:

//=================================================================================
   $Source: interfaces_wedge/wedgetranslate.h $
   $Date: 2009/06/29 05:03:56EDT $
   $Revision: 1.4 $



//=================================================================================*/

#ifndef WEDGE_TRANSLATE_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define WEDGE_TRANSLATE_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "WedgeProKey.h"

#define baseCWedgeTranslate CWedgeProKey
class CWedgeTranslate : public baseCWedgeTranslate
{
public:
	CWedgeTranslate(CDriverBase *pDriver, const CWedgeTerminal *pTerminal)
	:baseCWedgeTranslate(pDriver, pTerminal)
	{
	}
//	CWedgeTranslate(const CWedgeTerminal *pTerminal);
//	virtual ~CWedgeTranslate();


protected:
	void CopySequence(keynum_t keynum);
	void CopyBreakCode(keynum_t keynum, drvparamw_t scancode, drvparamw_t exinfo, long predelay);
	void DoMake(keynum_t keynum, drvparamw_t exinfo);
	void DoBreak(keynum_t keynum, drvparamw_t exinfo, long predelay);

	void CheckModifierMake(const CKey *key, int modkeyindex);
	void CheckModifierBreak(const CKey *key, int modkeyindex);
	void HandleModifierMakes(const CKey *key);
	void HandleModifierBreaks(const CKey *key);
	void HandleModifier(const CKey *key);
	bool TranslateKey(const CKey *key, tichar_t ansi);


// helpers
	bool IsCommunicationKey(keynum_t keynum);
	mkbrtype_t GetBreakType(keynum_t keynum)	{ return m_pMakebreak->GetType(keynum);	}
	int GetScancodeType(keynum_t keynum)		{ return m_pScancodes->GetScancodeType(keynum);	}
	scancode_t GetExtensionByte(void)			{ return m_pScancodes->GetExtensionByte();	}
	bool IsNoDelayKey(keynum_t keynum)			{ return m_pScancodes->IsNoDelayKey(keynum);	}

	bool IsModifierMade(modifiers_t ModifierBit)
	{
		return m_ModFlags&ModifierBit ? true : false;
	}

	void StoreModifierMake(modifiers_t ModifierBit)
	{
		m_ModFlags |= ModifierBit;
	}

	void StoreModifierBreak(modifiers_t ModifierBit)
	{
		m_ModFlags &= ~ModifierBit;
	}

	keynum_t GetModifierKeyNumber(int index);
	bool	IsSticky(int modkeyindex);


};

#endif /* WEDGE_TRANSLATE_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */




