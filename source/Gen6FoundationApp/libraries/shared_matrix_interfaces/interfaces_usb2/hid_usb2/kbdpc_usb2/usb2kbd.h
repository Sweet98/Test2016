/*=================================================================================
  class:

//=================================================================================
   $Source: interfaces_usb2/hid_usb2/kbdpc_usb2/usb2kbd.h $
   $Date: 2010/04/09 18:50:12EDT $
   $Revision: 1.5 $

//=================================================================================

//=================================================================================*/
//! \file

#ifndef USB2KBD_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define USB2KBD_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "string.h"
#include "usb2.h"  

/* for HID keyboards */
#define bHIDNumLock			0x01
#define bHIDCapsLock			0x02
#define bHIDScrollLock		0x04

#define BIT_LEFT_CTRL		0x01
#define BIT_LEFT_SHIFT		0x02
#define BIT_LEFT_ALT			0x04
#define BIT_LEFT_GUI			0x08
#define BIT_RIGHT_CTRL		0x10
#define BIT_RIGHT_SHIFT		0x20
#define BIT_RIGHT_ALT		0x40
#define BIT_RIGHT_GUI		0x80




#define USB_EMPTY_VALUE 0x00	/* empty scancode */
#define USB_NO_MODIFIER 0x00	/* none of the modifier keys is pressed */

#define HID_KEYBOARD_REPORT_SIZE 8

struct KbdInReportBuffer_t
{
	void Clear(void)						{ memset(GetArray(), 0, sizeof(KbdInReportBuffer_t));	}
	UCHAR	*GetArray(void)				{ return u.bytes.ucArray;				}
	size_t GetBufferSize(void)	const { return HID_KEYBOARD_REPORT_SIZE;	}

	UCHAR	GetScanCode(void)	const		{ return u.report.ucScancode;	}
	void	SetScanCode(UCHAR val)		{ u.report.ucScancode = val; }
										  
	void	InitializeUnusedScanCodes(void)	{ u.report.ucNotUsed[0]= 0;
										  	u.report.ucNotUsed[1]= 0;
										  	u.report.ucNotUsed[2]= 0;
										  	u.report.ucNotUsed[3]= 0;
										  	u.report.ucNotUsed[4]= 0;	}

	UCHAR	GetModifiers(void) const	{ return u.report.ucModifiers;}
	void	SetModifiers(UCHAR val)		{ u.report.ucModifiers = val;	}

	union
	{
		struct
		{
			UCHAR ucArray[HID_KEYBOARD_REPORT_SIZE];  	/* HID default keyboard reports are 8 bytes  */
		}bytes;
		struct
		{
			UCHAR ucModifiers;		/* bitmap of modifier keys */
			UCHAR ucDummy;  		/* USB defined dummy */
			UCHAR ucScancode;  	/* scancode  */
			UCHAR ucNotUsed[5];  	/* dummy scancodes, not used by our driver  */
		}report;
	}u;
};

#define HID_LED_REPORT_SIZE 1

struct KbdOutReportBuffer_t
{
	UCHAR	GetLedInfo(void) const			{ return u.report.ucLed;	}
	union
	{
		struct
		{
			UCHAR ucArray[HID_LED_REPORT_SIZE];  	// HID default LED reports are 1 bytes
		}bytes;
		struct
		{
			UCHAR ucLed;										// bitmap of LEDs
		}report;
	}u;
};

#endif /* USB2KBD_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */



