/*=================================================================================
  class:

//=================================================================================
   $Source: interfaces_usb2/hid_usb2/kbdpc_usb2/usb2kbdbase.cpp $
   $Date: 2010/04/09 18:50:13EDT $
   $Revision: 1.6 $

//=================================================================================

//=================================================================================*/
//! \file

#include "stdInclude.h"
#include "product.h"
#include "Usb2Kbdbase.h"
#include "Usb2Kbd.h"
#include "usb2hidkbd.h"
#include "WedgeTerminal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBitBangUsb2Kbd::CBitBangUsb2Kbd(const CWedgeTerminal *pTermInfo, CUsb2HidKbd *pUsb)
: CBitBanger(0)
{	
	m_ucScancode = USB_EMPTY_VALUE;
	m_ucModifiers= USB_NO_MODIFIER;
	m_ucDontLooseModifierBreaks = USB_NO_MODIFIER;
	m_KbdStatus = 0;

	m_pUsb = pUsb;
	m_pTermInfo = pTermInfo;
	ASSERT(m_pUsb != NULL);
	ASSERT(m_pTermInfo != NULL);
}

CBitBangUsb2Kbd::~CBitBangUsb2Kbd( void )
{
}

MatrixError_t CBitBangUsb2Kbd::BitBangOutTerm(UINT Data, UINT extrainfo)
{
	ASSERT(m_pUsb != NULL);

	if (extrainfo & EX_CLOSE)				   // clean up phase (after we send everything)
	{
		m_ucScancode = USB_EMPTY_VALUE;
		m_ucModifiers= USB_NO_MODIFIER;
		m_pUsb->SendAllKeysUp();
		return PASS;
	}

	/* This is to support sequences in prokey */
	if (m_KbdStatus & NEXT_IS_BREAK)
	{
		/* only modifiers can have a break */
		BreakModifier (Data, extrainfo);
		m_KbdStatus &= ~NEXT_IS_BREAK;
		return PASS;
	}
	else
	{
		if (Data == 0xf0)		/* support sequences in prokey */
		{
			m_KbdStatus |= NEXT_IS_BREAK;
			return PASS;
		}
		else
		{
			if ((extrainfo & EX_MODIFIER) || (extrainfo & EX_SEQUENCE))
			{
				if (MakeModifier (Data, extrainfo))
				{
					return PASS;
				}
			}
			m_ucScancode = (UCHAR) Data;
			DeliverToUSB();
		}
	}

	return PASS;
}

#define NUM_MOD_INDEX 6

/* This table contains offsets into the terminal properties table */
static const char modindexes[NUM_MOD_INDEX] =
{
	CWedgeTerminal::INDEX_LEFT_SHIFT,
	CWedgeTerminal::INDEX_RIGHT_SHIFT,
	CWedgeTerminal::INDEX_LEFT_CNTRL,
	CWedgeTerminal::INDEX_RIGHT_CNTRL,
	CWedgeTerminal::INDEX_LEFT_ALT,
	CWedgeTerminal::INDEX_RIGHT_ALT,
};

#define NUM_USB_MOD_KEYS 8

/* This table contains bits for storing modifier keys */
static const UCHAR modflags[NUM_USB_MOD_KEYS] =
{
	BIT_LEFT_SHIFT,
	BIT_RIGHT_SHIFT,
	BIT_LEFT_CTRL,
	BIT_RIGHT_CTRL,
	BIT_LEFT_ALT,
	BIT_RIGHT_ALT,
	BIT_LEFT_GUI,
	BIT_RIGHT_GUI,
};

/* This table contains scancodes (usages) for the USB modifier keys.
	This is not too clean according the forkey database idea, but is very USB specific and only used
	for Sequences (prokey and the special case where we only send make or break for some modifier keys.
	(left Cntrl make, left Cntrl break, left Alt make, left Alt break ==  characters 03..06)
*/
static const UCHAR usbmodkeys[NUM_USB_MOD_KEYS] =
{
	0xE1,		// BIT_LEFT_SHIFT,
	0xE5,		// BIT_RIGHT_SHIFT,
	0xE0,		// BIT_LEFT_CTRL,
	0xE4,		// BIT_RIGHT_CTRL,
	0xE2,		// BIT_LEFT_ALT,
	0xE6,		// BIT_RIGHT_ALT,
	0xE3,		// BIT_LEFT_GUI,
	0xE7,		// BIT_RIGHT_GUI,
};

void CBitBangUsb2Kbd::BreakModifier (UINT scancode, UINT extrainfo)
{
	ASSERT(m_pTermInfo != NULL);

	short modkey = extrainfo & EX_KEYNUMBER;
	short i;
	for (i=0; i<NUM_MOD_INDEX; i++)
	{
		if (modkey == m_pTermInfo->GetModifierKeyNumber(modindexes[i]))
		{
			m_ucModifiers &= ~modflags[i];
			// ensure we catch the case where we make-break-make a modifier key. We must send the break in these cases.
			m_ucDontLooseModifierBreaks = m_ucModifiers;
			return;
		}	
	}
	// now ensure we do not miss a modifier key inside a sequence
	for (i=0; i<NUM_USB_MOD_KEYS; i++)
	{
		if (scancode == usbmodkeys[i])
		{
			m_ucModifiers &= ~modflags[i];
			m_ucDontLooseModifierBreaks = m_ucModifiers;
			return;
		}
	}
}


int CBitBangUsb2Kbd::MakeModifier (UINT scancode, UINT extrainfo)
{
	ASSERT(m_pTermInfo != NULL);

	short modkey = extrainfo & EX_KEYNUMBER;
	short i;
	for (i=0; i<NUM_MOD_INDEX; i++)
	{		
		if (modkey == m_pTermInfo->GetModifierKeyNumber(modindexes[i]))
		{
			m_ucModifiers |= modflags[i];
			return PASS;
		}	
	}
	// now ensure we do not miss a modifier key inside a sequence
	for (i=0; i<NUM_USB_MOD_KEYS; i++)
	{
		if (scancode == usbmodkeys[i])
		{
			m_ucModifiers |= modflags[i];
			return PASS;
		}
	}
	return FAIL;
}

inline KbdInReportBuffer_t *CBitBangUsb2Kbd::GetReportBuffer(void)
{
	ASSERT(m_pUsb != NULL);
	return m_pUsb->GetWritableKbdReportBuffer();
}

// send the current keyboard report to the host and wait for done if bWait is set.
inline  void CBitBangUsb2Kbd::SendKbdReport(bool bWait)
{
	ASSERT(m_pUsb != NULL);
	m_pUsb->SendKbdReport(bWait);
}

void CBitBangUsb2Kbd::DeliverToUSB(void)
{
	KbdInReportBuffer_t *pReport = GetReportBuffer();
	pReport->InitializeUnusedScanCodes();
	
	// any modifier break on purpose? Then we need to send that break.
	if (m_ucDontLooseModifierBreaks != pReport->GetModifiers())
	{
		pReport->SetScanCode(USB_EMPTY_VALUE);
		pReport->SetModifiers(m_ucDontLooseModifierBreaks);
		SendKbdReport();
	}

	// same character as last time? then we need a gap
	if ( (m_ucScancode == pReport->GetScanCode()) && (m_ucScancode != USB_EMPTY_VALUE) )
	{
		m_KbdStatus |= NEED_GAP;
	}
	else	/* any modifier key changed? */
	{
		if (m_ucModifiers != pReport->GetModifiers())
			m_KbdStatus |= NEED_GAP;
	}

	if (m_KbdStatus & NEED_GAP)
	{
		m_KbdStatus &= ~NEED_GAP;
		pReport->SetScanCode(USB_EMPTY_VALUE);
		pReport->SetModifiers(m_ucModifiers);
		SendKbdReport();
	}

	pReport->SetScanCode(m_ucScancode);
	pReport->SetModifiers(m_ucModifiers);
	m_ucDontLooseModifierBreaks = m_ucModifiers;
	SendKbdReport();
}

/*
	In USB keyboards, the host does do the autorepeat.
	So if we do not send anything after we pressed a key, the auto repeat will add characters.
	To avoid this, we now send an "no normal keys pressed" if there are any longer Inter-X delays.

	We only add that additional report for longer delays to keep the time for short delays more accurate.
	With 30mSec and more, the additional report does not hurt much.
	And we are still save because a human being would have trouble with typing if the autorepeat is that fast.

 	This function is called from CWedgeLabel::SendCommands() which knows about the delays.
*/
void CBitBangUsb2Kbd::NotifyOutTermDelay(ULONG uSecDelay)
{
	const ULONG MaxDelayWithoutProblems=30000;
	const bool bWait=false;

	if(uSecDelay > MaxDelayWithoutProblems)
	{
		KbdInReportBuffer_t *pReport = GetReportBuffer();
		pReport->SetScanCode(USB_EMPTY_VALUE);
		pReport->SetModifiers(m_ucModifiers);  // autorepeat for modifiers does not hurt, so we keep them (faster)
		SendKbdReport(bWait);	// no wait until report is sent to keep influence of this action as small as possible
	}
}




