/*=================================================================================
  class: CWedgeBase

//=================================================================================
   $Source: interfaces_wedge/wedgebase.cpp $
   $Date: 2011/08/24 11:15:34EDT $
   $Revision: 1.24 $



//=================================================================================*/

#include "stdInclude.h"
#include "CommonDriver.h"
//#include "wdgtype.h"
#include "WedgeBase.h"
#include "WedgeCmd.h"
#include "WedgeCntrlKeys.h"
#include "WedgeTerminal.h"
#include "ParseHelper.h"
#include "WedgeHal.h"
#include "TransCodeEx.h"
#include "stdio.h"
#include "string.h"
#include "db_chrio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Constructor
CWedgeBase::CWedgeBase(CDriverBase *pDriver, const CWedgeTerminal *pTerminal)
{
	ASSERT(pDriver!=NULL);
	ASSERT(pTerminal!=NULL);
	ASSERT(pTerminal->m_pScancodes!=NULL);
	ASSERT(pTerminal->m_pMakebreak!=NULL);
	m_delay_short =500;
	m_delay_medium=500;
	m_delay_long = 500;
	m_Country = 0;
	m_style = 0;
	m_status.word = 0;
	m_ModFlags=0;
	m_options.word=0;
	m_LastSend = 0;
	m_InUse = FALSE;
	m_pTerminal = pTerminal;
	m_pScancodes = m_pTerminal->m_pScancodes;
	m_pMakebreak = m_pTerminal->m_pMakebreak;
	m_pDriver = pDriver;
	m_Running = false;	                  // allow for one time inits
}

// Destructor
CWedgeBase::~CWedgeBase()
{
}

void CWedgeBase::SetupDelays(void)
{
	/* Inter-character delay also increases all inter-byte delays to some destinct */
	/* A separate inter-byte delay in the menu would be better I guess. */
	ULONG InterCharDelay		= m_pDriver->GetInterCharDelay();
	ULONG InterFunctionDelay= m_pDriver->GetInterFunctionDelay();
	ULONG InterMessageDelay	= m_pDriver->GetInterMessageDelay();
	m_delay_short	=	InterCharDelay /4;
	m_delay_medium	=	InterCharDelay /2;
	m_delay_long	=	InterCharDelay *2;

	/* Setup the delays according the specific terminals, also handle turbo mode */
	if (IsTurboMode())
	{
		m_delay_short 			+= ((long)m_pTerminal->m_delay_short  *2)/4;
		m_delay_medium 		+= ((long)m_pTerminal->m_delay_medium *2)/5;
		//ewr-04771, removed for turbo mode.
		//InterCharDelay			+= ((long)m_pTerminal->m_delay_intercharacter *2)/5;
		InterFunctionDelay	+= ((long)m_pTerminal->m_delay_intercharacter *2)/5;
	}
	else
	{
		m_delay_short 			+= m_pTerminal->m_delay_short;
		m_delay_medium 		+= m_pTerminal->m_delay_medium;
		InterCharDelay			+= m_pTerminal->m_delay_intercharacter;
		InterFunctionDelay	+= m_pTerminal->m_delay_intercharacter;
	}

	TweakTiming(m_delay_short);
	TweakTiming(m_delay_medium);

	/* These delays are independent of the turbo mode */
	m_delay_long 				+= m_pTerminal->m_delay_long;

	/* use at least this delay between labels */
	m_pDriver->SetInterMessageDelay(InterMessageDelay + m_delay_long);

	/* Interfunction is minimum as interchar*/
	if (InterFunctionDelay < InterCharDelay)
		InterFunctionDelay = InterCharDelay;
	//CWedgeDriver::SetInterCharDelay(InterCharDelay);
	m_pDriver->SetInterCharDelay(InterCharDelay);
	m_pDriver->SetInterFunctionDelay(InterFunctionDelay);
}

inline void CWedgeBase::TweakTiming(long &delay)
{
	if(delay >= 400)
		delay -= 250; 	// tweak to compensate for other delays
}

void CWedgeBase::StoreKbdStyle(int newvalue)
{
	m_style =newvalue;
}
void CWedgeBase::StoreKbdMode(int newvalue)
{
	m_mode=newvalue;
}
void CWedgeBase::StoreCountry(int newvalue)
{
	m_Country = newvalue;
}
void CWedgeBase::StoreAutoKeyEmu(bool newvalue)
{
	m_options.bits.option_auto_KeyEmu = newvalue;
}
void CWedgeBase::StoreCntrlASCII(int newvalue)
{
   if(newvalue > 2)   // Support for KBDCAS3 menu command
   {
      m_options.bits.option_cntrl_plus_ascii = 2;
      m_options.bits.option_cntrl_plus_ascii3 = 1;
   }
   else
   {
      m_options.bits.option_cntrl_plus_ascii = newvalue;
      m_options.bits.option_cntrl_plus_ascii3 = 0;
   }
}

// Support for KBDCAS3 menu command
void CWedgeBase::SetCntrlASCII(int newvalue)
{
   m_options.bits.option_cntrl_plus_ascii = newvalue;
}

void CWedgeBase::StoreTurboMode(bool newvalue)
{
	m_options.bits.option_turbo = newvalue;
}
void CWedgeBase::StoreDigitsViaNumPad(bool newvalue)
{
	m_options.bits.option_digits_via_numpad = newvalue;
}

/* translations for unsupported keys  */
void CWedgeBase::StoreTransDead(bool newvalue)
{
	m_Translations.bits.trans_Deadkey = newvalue;
}
void CWedgeBase::StoreTransHtml(bool newvalue)
{
	m_Translations.bits.trans_Html = newvalue;
}
void CWedgeBase::StoreTransHex(bool newvalue)
{
	m_Translations.bits.trans_AllHex = newvalue;
}
void CWedgeBase::StoreNonPrint(bool newvalue)
{
	m_Translations.bits.trans_Nonprt=newvalue;
}
void CWedgeBase::StoreTransAltNp(int newvalue)
{
	m_Translations.bits.trans_AltNp = newvalue;
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


bool CWedgeBase::IsKbdStyleCapsLock(void)
{
	return m_style == 1;
}
bool CWedgeBase::IsKbdStyleShiftLock(void)
{
	return m_style == 2;
}
bool CWedgeBase::IsKbdStyleAutoCapsLock(void)
{
	return m_style == 6;		/* This changed from 3 to 6 for Visual Menu (3&4 have no value - dmh 12NOV98). */
}
bool CWedgeBase::IsKbdStyleKeyEmu(void)
{
	return m_style == 5;		/* This changed from 4 to 5 for Visual Menu (3&4 have no value - dmh 12NOV98). */
}
bool CWedgeBase::IsKbdStyleAutoCapsLockViaNumLock(void)
{
	return m_style == 7;
}

bool CWedgeBase::IsAutoKeyEmu(void)
{
	return m_options.bits.option_auto_KeyEmu ? true : false;
}

bool CWedgeBase::IsCntrlASCII(void)
{
	return (m_options.bits.option_cntrl_plus_ascii == 1);
}

bool CWedgeBase::IsCntrlASCII_Win(void)
{
	return (m_options.bits.option_cntrl_plus_ascii == 2);
}

bool CWedgeBase::IsTurboMode(void)
{
	return m_options.bits.option_turbo ? true : false;
}

bool CWedgeBase::IsDigitsViaNumPad(void)
{
	return m_options.bits.option_digits_via_numpad ? true : false;
}

//ewr-04771
bool CWedgeBase::IsTransAltNp(void)
{
	return m_Translations.bits.trans_AltNp ? true : false;
}
//end ewr04771

//////////////////////////////////////////////////////////////////////

/* This is to support a keyboard style. */
void CWedgeBase::HandleNumbersViaNumPad(const CChar2Key *pUsa)
{
   tichar_t i;
	keynum_t			keynum;
	modifiers_t		modifier;
	if (IsDigitsViaNumPad())
	{
		for(i='0'; i<='9'; i++)
		{
			if (pUsa->GetNumPadKeyDetails(i, keynum, modifier))
				m_KeyNumLookup.StoreKey(i, keynum, modifier);
		}
	}
}

MatrixError_t CWedgeBase::ExtractParameters(const tichar_t *pParameters )
{
//	bool ret=CDriverBase::ExtractParameters(pParameters);
	bool ret=true;

	int temp=0;

	// used for selecting the correct keyboard layout
	ret &= CParseHelper::Extract(pParameters, _TI("Country"), m_Country);

	// KeyEmu and several Lock styles
	ret &= CParseHelper::Extract(pParameters, _TI("Style"), temp);
	StoreKbdStyle(temp);

      //Control the output style of the keyboard data.
	ret &= CParseHelper::Extract(pParameters, _TI("Mode"), temp);
	StoreKbdMode(temp);

	// A problem solver (for DEC VT500 series and others)
	ret &= CParseHelper::Extract(pParameters, _TI("AutoDC"), temp);
	StoreAutoKeyEmu(temp ? true : false);

	// Reduce delay between bytes
	ret &= CParseHelper::Extract(pParameters, _TI("Turbo"), temp);
	StoreTurboMode(temp ? true : false);

	// Send control characters as CONTROL-KEY + character
	ret &= CParseHelper::Extract(pParameters, _TI("CtrlAsc"), temp);
	StoreCntrlASCII(temp);

	// Send numbers via the NumPad
	ret &= CParseHelper::Extract(pParameters, _TI("NumPad"), temp);
	StoreDigitsViaNumPad(temp ? true : false);

	// Compose accented characters on international keyboards
	ret &= CParseHelper::Extract(pParameters, _TI("DeadK"), temp);
	StoreTransDead(temp ? true : false);

	// Send unsupported characters with HTML syntax (&#123 is { )
	ret &= CParseHelper::Extract(pParameters, _TI("Html"), temp);
	StoreTransHtml(temp ? true : false);

	// Send all characters as a hex dump (trouble shooter)
	ret &= CParseHelper::Extract(pParameters, _TI("Hex"), temp);
	StoreTransHex(temp ? true : false);

	 //send Non printed characters
       ret &= CParseHelper::Extract(pParameters, _TI("NonPrt"), temp);
	StoreNonPrint(temp ? true : false);

	// Send unsupported characters as ALT + NP
	{	                                    // keep this block together
		ret &= CParseHelper::Extract(pParameters, _TI("AltNp"), temp);
		StoreTransAltNp(temp);

		/* Does the terminal support this mode? */
		if (IsTransAltNpUnicode())
		{
			if ( !(m_pTerminal->IsCapAltNpUni()) )
				temp = 0;	/* not supported, turn feature off */
		}
		if (IsTransAltNpWin())
		{
			if ( !(m_pTerminal->IsCapAltNpWin()) )
				temp = 0;	/* not supported, turn feature off */
		}
		if (IsTransAltNpDos())
		{
			if ( !(m_pTerminal->IsCapAltNpDos()) )
				temp = 0;	/* not supported, turn feature off */
		}
		StoreTransAltNp(temp);
	}

	SetupDelays();

	return ret ? ERR_PASS : ERR_BADPARAM;
}

//MatrixError_t CWedgeBase::Open(interface_id_t nId, const tichar_t *pParameter)
MatrixError_t CWedgeBase::Open(interface_id_t, const tichar_t *pParameter)
{

	MatrixError_t ret = ExtractParameters(pParameter);
	if(IsOK(ret))
	{
		if(!m_Running)								// only do this once
		{
			m_Running = true;
			PrepareIoForWedgeHardware();
		}

		if (IsKbdStyleKeyEmu())
		{
			WdgStoreKeyEmu(true); 				// copy to dynamic flag
			DisableKeyboard();					// just in case someone connected a keyboard by accident
			StoreAutoKeyEmu(false);				// fix nonsense in some manuals
		}
		else
		{
			WdgStoreKeyEmu(false); 				// copy to dynamic flag
			EnableKeyboard();
		}
		WdgStoreLED_Valid(0);					// invalidate the led info
		WdgStoreCapsLockSave(0);				// ensure there is no undesired "Lock close" action after the label

		//ewr-04771
		WdgStoreNumLockSave(0);

	}

	return ret;
}

MatrixError_t CWedgeBase::Close(void)
{
	return ERR_PASS;
}

void CWedgeBase::EnableKeyboard (void)
{
	if (!WdgIsKeyEmu())
		WedgeHal.KeyboardEnable();
}

void CWedgeBase::DisableKeyboard (void)
{
	WedgeHal.KeyboardDisable();
}

// only the USB keyboard needs that so far
void CWedgeBase::NotifyOutTermDelay(ULONG /* uSecDelay */)
{
}

// This is a noop for most interfaces. See the driver for the IBM Thinkpad for a usage example.
void CWedgeBase::OpenLabel(void)
{
}

// This is a noop for most interfaces. See the driver for the IBM Thinkpad for a usage example.
void CWedgeBase::FinalCloseLabel(void)
{
}

int CWedgeBase::GetSystemCodepage(void)
{
	return theTransCode.GetSystemCodepage();
}

// Support for KBDCAS3 menu command
void CWedgeBase::TurnOnControlAscii(void)
{
	SetCntrlASCII(2);
	HandleCntrlPlusAscii();
}

// Support for KBDCAS3 menu command
void CWedgeBase::TurnOffControlAscii(void)
{
	SetCntrlASCII(0);
	HandleCntrlPlusAscii();
//			const CChar2Key * m_pUSA=m_pTerminal->GetLayout();
//			m_KeyNumLookup.InitializeCtrlASCII(m_pUSA);
}


void CWedgeBase::PrepareIoForWedgeHardware(void)
{
	WedgeHal.PrepareIoForWedge();
}






