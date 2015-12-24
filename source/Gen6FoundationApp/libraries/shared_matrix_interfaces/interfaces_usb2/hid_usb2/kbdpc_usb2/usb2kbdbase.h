/*=================================================================================
  class:

//=================================================================================
   $Source: interfaces_usb2/hid_usb2/kbdpc_usb2/usb2kbdbase.h $
   $Date: 2009/11/16 13:55:10EST $
   $Revision: 1.4 $

//=================================================================================

//=================================================================================*/
//! \file

#ifndef BITBANG_USB2KBD_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define BITBANG_USB2KBD_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "BitBanger.h"

class CWedgeTerminal;
class CUsb2HidKbd;
struct KbdInReportBuffer_t;

class CBitBangUsb2Kbd : public CBitBanger
{
public:
	CBitBangUsb2Kbd(const CWedgeTerminal *pTermInfo, CUsb2HidKbd *pUsb);
	virtual ~CBitBangUsb2Kbd(void);

	virtual MatrixError_t BitBangOutTerm(UINT Data, UINT exinfo);
	virtual void NotifyOutTermDelay(ULONG uSecDelay);
private:
	KbdInReportBuffer_t *GetReportBuffer(void);
	int  MakeModifier (UINT scancode, UINT extrainfo);
	void BreakModifier (UINT scancode, UINT extrainfo);
	void DeliverToUSB(void);
	void SendKbdReport(bool bWait=true);
private:
	/* for keyboard status */
	enum
	{
		NEXT_IS_BREAK	= 0x01,
		NEW_DATA			= 0x02,
		NEED_GAP			= 0x04,
		CAN_SEND			= 0x08,
		BUSY				= 0x10
	};

	CUsb2HidKbd *m_pUsb;
	const CWedgeTerminal *m_pTermInfo;

	volatile UCHAR	m_KbdStatus;
	UCHAR m_ucDontLooseModifierBreaks;		/* bitmap of modifier keys, ensures we send the break in certain cases */
	UCHAR m_ucModifiers;		/* bitmap of modifier keys */
	UCHAR m_ucScancode;  		/* scancode  */
};


#endif /* BITBANG_USBKBD_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */



