/*=================================================================================
  class: CWedgeBase

//=================================================================================
   $Source: interfaces_wedge/wedgebase.h $
   $Date: 2011/08/24 11:16:05EDT $
   $Revision: 1.19 $



//=================================================================================*/

#ifndef WEDGE_BASE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define WEDGE_BASE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#include "DriverBase.h"
#include "WdgType.h"
#include "WedgeCmd.h"
#include "WedgeTerminal.h"
#include "IoMan.h"

/* Keyboard Style */
typedef union
{
	struct tag_bits
	{
		unsigned int	option_cntrl_plus_ascii : 2;
		unsigned int 	option_cntrl_plus_ascii3 : 1;
		unsigned int	option_turbo  : 1;
		unsigned int	option_digits_via_numpad  : 1;
		unsigned int	option_auto_KeyEmu	: 1;
	}bits;
	unsigned int word;
}wdg_options_t;

/* Wedge Status */
typedef union
{
	struct tag_wdg_status_bits
	{
		unsigned int	wst_configured : 1;	/* used to sycronize the thread at powerup */
		unsigned int	wst_enabled : 1;		/* used by the Thinkpad driver to control the output. See below */
		unsigned int	wst_KeyEmu  : 1;		/* Keyboard emulation mode. */
		unsigned int	wst_LED_caps : 1;
		unsigned int	wst_LED_caps_save : 1;
		unsigned int	wst_LED_num  : 1;
		unsigned int	wst_LED_num_save  : 1;
		unsigned int	wst_LED_scroll : 1;
		unsigned int	wst_LED_scroll_save : 1;
		unsigned int	wst_LED_valid : 1;
		unsigned int	wst_disable_highlevel_output : 1;	/* used to turn on/off all output to the wedge drivers. See below */
   }bits;
	unsigned int word;
}wdg_status_t;
/* Some notes on the two enable/disable bits in wdg_status_t:
 * wst_enabled is used for controlling the output by the terminal specific wedge driver.
 * 	This driver is responsible for enabling/disabling the enable bit.
 * 	A good example is the Thinkpad driver which uses it wait until the PC gives permittion to send scancodes.
 * 	Actually all IBM sytyle drivers use this function (there is a DoEnable command defined by the IBM interface),
 * 	but the Thinkpad uses it intensively.
 * 	In other words: This bit controls the output on the byte level.
 * 	If a driver does not need to use it, that's perfectly fine.
 * wst_disable_highlevel_output on the other hand controls the output even before the specific wedge driver gets called.
 * 	It works on the label level. Currently used by the USB keyboard driver to redirect the output to the
 * 	HIDPOS interface. (USB devices can have several interfaces at the same time,
 * 	our USB keyboard is a dual interface device).
 * 	This bit is initialized to 0, so for compatibility I choose the disable function (rather enable).
 */

/* Translation bits for usTranslation (wedge part).
	They are here to decouple the relasionship between the interfaces
	and the ECI translator.
	Translate not supported characters into: */
typedef union
{
	struct tag_wdg_trans_bits
	{
		unsigned int	trans_Deadkey	: 1;	/* Try to compose characters with dead keys */
		unsigned int	trans_Html		: 1;	/* HTML syntax */
		unsigned int	trans_AllHex	: 1;	/* For sending binary data via keyboard (kind a hex dump) */
		unsigned int    trans_Nonprt   	: 1; 	/*Non print*/
		unsigned int	trans_AltNp		: 3;	/* Try to compose characters via ALT+ 4 digit NumPAD */
	}bits;
	unsigned int word;
}wdg_translations_t;

#define NUMBER_OF_SPECIAL_CONTROL_KEYS	5

typedef struct tag_CntrlKeynumbers
{
	keynum_t	key[NUMBER_OF_SPECIAL_CONTROL_KEYS];
}CntrlKeynumbers;

//class CWedgeKeyNumber;
//class CWedgeTerminal;
class CScancodes;
class CMakeBreaks;

class CWedgeBase
{
	friend class CBitBangerISR;
public:
	CWedgeBase(CDriverBase *pDriver, const CWedgeTerminal *pTerminal);
	virtual ~CWedgeBase();

   	virtual MatrixError_t Open(interface_id_t nId, const tichar_t *pParameter);
	virtual MatrixError_t Close(void);
//	virtual IoControlRet_t DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam)=0;

	void StoreKbdStyle(int newvalue);
	void StoreKbdMode(int newvalue);
	void StoreCountry(int newvalue);

	void StoreAutoKeyEmu(bool newvalue);
	void StoreCntrlASCII(int newvalue);
	void StoreTurboMode(bool newvalue);
	void StoreDigitsViaNumPad(bool newvalue);
	void SetCntrlASCII(int newvalue);

	/* translations for unsupported keys  */
	void StoreTransDead(bool newvalue);
	void StoreTransHtml(bool newvalue);
	void StoreTransHex(bool newvalue);
	void StoreNonPrint(bool newvalue);
	void StoreTransAltNp(int newvalue);
//	void TurnOnControlAscii(void);
//	void TurnOffControlAscii(void);

	bool IsTransDead(void)
	{
		return m_Translations.bits.trans_Deadkey ? true : false;
	}
	bool IsTransHtml(void)
	{
		return m_Translations.bits.trans_Html ? true : false;
	}
	bool IsTransHex(void)
	{
		return m_Translations.bits.trans_AllHex ? true : false;
	}
	bool IsTransNonPrint(void)
	{
		return m_Translations.bits.trans_Nonprt ? true : false;
	}
	bool IsTransAltNpWin(void)
	{
		return (m_Translations.bits.trans_AltNp == 1);
	}
	bool IsTransAltNpDos(void)
	{
		return (m_Translations.bits.trans_AltNp == 2);
	}
	bool IsTransAltNpWinSysCp(void)
	{
		return (m_Translations.bits.trans_AltNp == 4);
	}
	bool IsTransAltNpDosSysCp(void)
	{
		return (m_Translations.bits.trans_AltNp == 5);
	}
	bool IsTransAltNpUnicode(void)
	{
		return (m_Translations.bits.trans_AltNp == 3);
	}
	bool NeedsPreSuffixEsc(void)
	{
		return (m_options.bits.option_cntrl_plus_ascii3 == 1);
	}


	int GetKbdStyle(void);
	int GetMode(void)			{return m_mode; }

	bool IsAutoKeyEmu(void);
	bool IsCntrlASCII(void);
	bool IsCntrlASCII_Win(void);
	bool IsTurboMode(void);
	bool IsDigitsViaNumPad(void);

	//ewr-04771
	bool IsTransAltNp(void);

	bool IsKbdStyleCapsLock(void);
	bool IsKbdStyleShiftLock(void);
	bool IsKbdStyleAutoCapsLock(void);
	bool IsKbdStyleAutoCapsLockViaNumLock(void);
	bool IsKbdStyleKeyEmu(void);

///////////////////////////////////////////////////////////////////////
/*****************************************************************************
Here we keep helper functions to store and retrieve status information for the
wedge translation engine.
*****************************************************************************/

	bool WdgIsKeyEmu(void)
	{
		return m_status.bits.wst_KeyEmu ? true : false;
	}
	bool WdgIsEnabled(void)
	{
		return m_status.bits.wst_enabled ? true : false;
	}


	bool WdgIsCapsLock(void)
	{
		return m_status.bits.wst_LED_caps ? true : false;
	}
	bool WdgIsNumLock(void)
	{
		return m_status.bits.wst_LED_num ? true : false;
	}
	bool WdgIsScrollLock(void)
	{
		return m_status.bits.wst_LED_scroll ? true : false;
	}
	bool WdgIsLED_Valid(void)
	{
		return m_status.bits.wst_LED_valid ? true : false;
	}
	bool WdgIsCapsLockSave(void)
	{
		return m_status.bits.wst_LED_caps_save ? true : false;
	}
	bool WdgIsHigLevelOutputDisabled(void)
	{
		return m_status.bits.wst_disable_highlevel_output ? true : false;
	}
	bool WdgIsConfigurationValid(void)
	{
		return m_status.bits.wst_configured ? true : false;
	}

	void WdgStoreHigLevelOutputDisabled(bool newvalue)
	{
		m_status.bits.wst_disable_highlevel_output = newvalue ;
	}
	void WdgStoreEnabled(bool newvalue)
	{
		m_status.bits.wst_enabled = newvalue ;
	}
	void WdgStoreCapsLock(bool newvalue)
	{
		m_status.bits.wst_LED_caps = newvalue;
	}
	void WdgStoreNumLock(bool newvalue)
	{
		m_status.bits.wst_LED_num = newvalue;
	}
	void WdgStoreScrollLock(bool newvalue)
	{
		m_status.bits.wst_LED_scroll = newvalue;
	}
	void WdgStoreKeyEmu(bool newvalue)
	{
		m_status.bits.wst_KeyEmu = newvalue;
	}
	void WdgStoreLED_Valid(bool newvalue)
	{
		m_status.bits.wst_LED_valid = newvalue ;
	}
	void WdgStoreCapsLockSave(bool newvalue)
	{
		m_status.bits.wst_LED_caps_save = newvalue ;
	}
	void WdgStoreConfigurationValid(bool newvalue)
	{
		m_status.bits.wst_configured = newvalue ;
	}

	//ewr-04771
	void WdgStoreNumLockSave(bool newvalue)
	{
		m_status.bits.wst_LED_num_save = newvalue ;
	}

	bool WdgIsNumLockSave(void)
	{
		return m_status.bits.wst_LED_num_save ? true : false;
	}
	//end ewr-04771

protected:
	virtual MatrixError_t ExtractParameters(const tichar_t *pParameters );
	virtual MatrixError_t BitBangOut(unsigned int ucData, unsigned int exinfo)=0;
	virtual MatrixError_t BitBangIn(unsigned int &uiReturnData)=0;
	virtual MatrixError_t OpenKeyboardEmulation(void)=0;
//	virtual MatrixError_t CheckKeyboardEmulation(void)=0;
	virtual void NotifyOutTermDelay(ULONG uSecDelay);
	virtual void OpenLabel(void);
	virtual void FinalCloseLabel(void);

	virtual void PrepareIoForWedgeHardware(void);

protected:
	void SetupDelays(void);
	void TweakTiming(long &delay);

	void HandleNumbersViaNumPad(const CChar2Key *pUsa);
	void HandleCntrlPlusAscii(void);
	void HandleCapsLock(void);
	void TurnOnControlAscii(void);
	void TurnOffControlAscii(void);
	void HandleKbdMode(void);
// helper functions

	virtual void EnableKeyboard(void);
	virtual void DisableKeyboard(void);

	keynum_t CntrlToKeynumber(const CntrlKeynumbers *cntrlkey, tichar_t ascii);
	keynum_t GetCntrlPlusASCIIKeynumber(tichar_t ascii);
	void GetCntrlPlusASCII_Win_Keynumber(tichar_t ascii, keynum_t &keynumber, modifiers_t &flags);

	long GetShortDelay(void)		{ return m_delay_short;	}
	long GetMediumDelay(void)		{ return m_delay_medium;	}
	long GetLongDelay(void)			{ return m_delay_long;	}

	static int GetSystemCodepage(void);

	unsigned long GetInterCharDelay(tichar_t ansi)	{ return m_pDriver->GetInterCharDelay(ansi);	}
	unsigned long GetInterMessageDelay(void)			{ return m_pDriver->GetInterMessageDelay();	}
	int GetCountry(void)						{ return m_Country;	}
	int GetTerminalID(void)					{ return m_pDriver->GetTerminalID();	}

	CKey *GetKey(tichar_t character)	{ return m_KeyNumLookup.GetKey(character);	}

	void AddDelayCommand(long delay)	{ m_CmdBuffer.AddDelayCommand(delay);					}
	void AddMediumDelayCommand(void)	{ m_CmdBuffer.AddDelayCommand(GetMediumDelay());	}
	void AddEndCommand(void)			{ m_CmdBuffer.AddEndCommand();	}
	void AddScancode(drvparamw_t scancode, drvparamw_t exinfo)	{ m_CmdBuffer.AddScancode(scancode, exinfo);	}

	//	const wdg_bitbang		*bitbangs;
	CDriverBase						*m_pDriver;		// a pointer to our driver (we are a component of this driver)
	const CWedgeTerminal			*m_pTerminal;

	/* Translations */
	const CScancodes				*m_pScancodes;
	const CMakeBreaks				*m_pMakebreak;
	CWedgeKeyNumber				m_KeyNumLookup;
	CWedgeCmd						m_CmdBuffer;

	modifiers_t						m_ModFlags;
	volatile wdg_status_t		m_status;
	wdg_options_t					m_options;
	wdg_translations_t			m_Translations;    /* various enable/disable bits for translations */
	int								m_style;
	int 								m_mode;
	long								m_delay_short;
	long								m_delay_medium;
	long								m_delay_long;
	unsigned int					m_LastSend;
	volatile bool					m_InUse;				/* Anti overrun */
	bool 								m_Running;			// run parts of the Open() only once

	int								m_Country;
};



#endif // WEDGE_BASE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

 //   ewr020560 Ctrl+AscII: Added support for new KBDCAS3 menu command to turn off Ctrl+AscII
 //   translation in wedge interfaces.




