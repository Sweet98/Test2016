/*=================================================================================
  class: CWedgeMultiKey
		This class knows how to translate a single character into a series of keystrokes.
//=================================================================================
   $Source: interfaces_wedge/wedgemultikey.cpp $
   $Date: 2011/08/24 11:23:04EDT $
   $Revision: 2.8 $



//=================================================================================*/

#include "stdInclude.h"
#include "stdio.h"
#include "CommonDriver.h"
#include "WedgeMultiKey.h"
#include "TransCodeEx.h"
#include "db_chrio.h"

#ifdef WINSIM
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#ifndef UNICODE_FOR_INTERFACES
#error "This module works only for unicode. Use an older version (1.x) for non-unicode builds"
#endif


// Constructor
CWedgeMultiKey::CWedgeMultiKey(CDriverBase *pDriver, const CWedgeTerminal *pTerminal)
:baseCWedgeMultiKey(pDriver, pTerminal)
{
}

/*
// Destructor
CWedgeMultiKey::~CWedgeMultiKey()
{
}
*/

/************************************************************************
	Dead key handling:
	Dead keys are those that do not type anything until you hit another key.
	Then a new character will be composed from the two key strokes.

	We use this method to send characters that are not direcly avaialable
	on a keyboard.
	A list of predefined composed characters (unicode) is used to look up
	the two key combination.

	Note: Some of the characters have an alias assignmet in the CP1252 range.
	For backward compatibility we always check if these are available if
	the original dead key is not there.
************************************************************************/
struct NonPrintChar_t
{
   tchar_t  first;
   tchar_t second;
   tchar_t third;
};
const static NonPrintChar_t TableNonPrintChars[]=
{
	0x4E,0x55,0x4C,	//NUL
	0x53,0x4F,0x48,	//SOH
	0x53,0x54,0x58,	//STX
	0x45,0x54,0x58,	//ETX
	0x45,0x4F,0x54,	//EOT
	0x45,0x4E,0x51,	//ENQ
	0x41,0x43,0x4B,	//ACK
	0x42,0x45,0x4C,	//BEL
	0x42,0x53,0x00,	//BS
	0x48,0x54,0x00,	//HT
	0x4C,0x46,0x00,	//LF
	0x56,0x54,0x00,	//VT
	0x46,0x46,0x00,	//FF
	0x43,0x52,0x00,	//CR
	0x53,0x4F,0x00,	//SO
	0x53,0x49,0x00,	//SI
	0x44,0x4C,0x45,	//DLE
	0x44,0x43,0x31,	//DC1
	0x44,0x43,0x32,	//DC2
	0x44,0x43,0x33,	//DC3
	0x44,0x43,0x34,	//DC4
	0x4E,0x41,0x4B,	//NAK
	0x53,0x59,0x4E,	//SYN
	0x45,0x54,0x42,	//ETB
	0x43,0x41,0x4E,	//CAN
	0x45,0x4D,0x00,	//EM
	0x53,0x55,0x42,	//SUB
	0x45,0x53,0x43,	//ESC
	0x46,0x53,0x00,	//FS
	0x47,0x53,0x00,	//GS
	0x52,0x53,0x00,	//RS
	0x55,0x53,0x00	//US
};
struct DeadKeyAlias_t
{
	wchar_t	unicode;
	wchar_t	ansi;
};

struct DeadKeyTripple_t
{
	wchar_t	composed;
	wchar_t	second;
	wchar_t	first;
};

#define  MinTwoKeys	0xC0	// below there are not dead keys
#define	empty 0				// used as the table end

const static DeadKeyAlias_t CompatibleDeadKeys[] =
{
	0x0300,0x60,	// GRAVE
	0x0301,0xB4,	// ACUTE
	0x0302,0x5E,	// CIRCUMFLEX
	0x0303,0x7E,	// TILDE
	0x0308,0xA8,	// DIAERESIS
	0x0327,0xB8,	// CEDILLA
	empty,empty		// end of table
};

const static DeadKeyTripple_t TableDeadKeys[] =
{
	0x00C0,0x0041,0x0300,	// LATIN CAPITAL LETTER A WITH GRAVE
	0x00C1,0x0041,0x0301,	// LATIN CAPITAL LETTER A WITH ACUTE
	0x00C2,0x0041,0x0302,	// LATIN CAPITAL LETTER A WITH CIRCUMFLEX
	0x00C3,0x0041,0x0303,	// LATIN CAPITAL LETTER A WITH TILDE
	0x00C4,0x0041,0x0308,	// LATIN CAPITAL LETTER A WITH DIAERESIS
	0x00C5,0x0041,0x030A,	// LATIN CAPITAL LETTER A WITH RING ABOVE
	0x00C7,0x0043,0x0327,	// LATIN CAPITAL LETTER C WITH CEDILLA
	0x00C8,0x0045,0x0300,	// LATIN CAPITAL LETTER E WITH GRAVE
	0x00C9,0x0045,0x0301,	// LATIN CAPITAL LETTER E WITH ACUTE
	0x00CA,0x0045,0x0302,	// LATIN CAPITAL LETTER E WITH CIRCUMFLEX
//	0x00CB,0x0045,0x0308,	// LATIN CAPITAL LETTER E WITH DIAERESIS     // Windows drivers do not support this
	0x00CC,0x0049,0x0300,	// LATIN CAPITAL LETTER I WITH GRAVE
	0x00CD,0x0049,0x0301,	// LATIN CAPITAL LETTER I WITH ACUTE
	0x00CE,0x0049,0x0302,	// LATIN CAPITAL LETTER I WITH CIRCUMFLEX
	0x00CF,0x0049,0x0308,	// LATIN CAPITAL LETTER I WITH DIAERESIS
	0x00D1,0x004E,0x0303,	// LATIN CAPITAL LETTER N WITH TILDE
	0x00D2,0x004F,0x0300,	// LATIN CAPITAL LETTER O WITH GRAVE
	0x00D3,0x004F,0x0301,	// LATIN CAPITAL LETTER O WITH ACUTE
	0x00D4,0x004F,0x0302,	// LATIN CAPITAL LETTER O WITH CIRCUMFLEX
	0x00D5,0x004F,0x0303,	// LATIN CAPITAL LETTER O WITH TILDE
	0x00D6,0x004F,0x0308,	// LATIN CAPITAL LETTER O WITH DIAERESIS
	0x00D9,0x0055,0x0300,	// LATIN CAPITAL LETTER U WITH GRAVE
	0x00DA,0x0055,0x0301,	// LATIN CAPITAL LETTER U WITH ACUTE
	0x00DB,0x0055,0x0302,	// LATIN CAPITAL LETTER U WITH CIRCUMFLEX
	0x00DC,0x0055,0x0308,	// LATIN CAPITAL LETTER U WITH DIAERESIS
//	0x00DD,0x0059,0x0301,	// LATIN CAPITAL LETTER Y WITH ACUTE     // Windows drivers do not support this
	0x00E0,0x0061,0x0300,	// LATIN SMALL LETTER A WITH GRAVE
	0x00E1,0x0061,0x0301,	// LATIN SMALL LETTER A WITH ACUTE
	0x00E2,0x0061,0x0302,	// LATIN SMALL LETTER A WITH CIRCUMFLEX
	0x00E3,0x0061,0x0303,	// LATIN SMALL LETTER A WITH TILDE
	0x00E4,0x0061,0x0308,	// LATIN SMALL LETTER A WITH DIAERESIS
	0x00E5,0x0061,0x030A,	// LATIN SMALL LETTER A WITH RING ABOVE
	0x00E7,0x0063,0x0327,	// LATIN SMALL LETTER C WITH CEDILLA
	0x00E8,0x0065,0x0300,	// LATIN SMALL LETTER E WITH GRAVE
	0x00E9,0x0065,0x0301,	// LATIN SMALL LETTER E WITH ACUTE
	0x00EA,0x0065,0x0302,	// LATIN SMALL LETTER E WITH CIRCUMFLEX
	0x00EB,0x0065,0x0308,	// LATIN SMALL LETTER E WITH DIAERESIS
	0x00EC,0x0069,0x0300,	// LATIN SMALL LETTER I WITH GRAVE
	0x00ED,0x0069,0x0301,	// LATIN SMALL LETTER I WITH ACUTE
	0x00EE,0x0069,0x0302,	// LATIN SMALL LETTER I WITH CIRCUMFLEX
	0x00EF,0x0069,0x0308,	// LATIN SMALL LETTER I WITH DIAERESIS
	0x00F1,0x006E,0x0303,	// LATIN SMALL LETTER N WITH TILDE
	0x00F2,0x006F,0x0300,	// LATIN SMALL LETTER O WITH GRAVE
	0x00F3,0x006F,0x0301,	// LATIN SMALL LETTER O WITH ACUTE
	0x00F4,0x006F,0x0302,	// LATIN SMALL LETTER O WITH CIRCUMFLEX
	0x00F5,0x006F,0x0303,	// LATIN SMALL LETTER O WITH TILDE
	0x00F6,0x006F,0x0308,	// LATIN SMALL LETTER O WITH DIAERESIS
	0x00F9,0x0075,0x0300,	// LATIN SMALL LETTER U WITH GRAVE
	0x00FA,0x0075,0x0301,	// LATIN SMALL LETTER U WITH ACUTE
	0x00FB,0x0075,0x0302,	// LATIN SMALL LETTER U WITH CIRCUMFLEX
	0x00FC,0x0075,0x0308,	// LATIN SMALL LETTER U WITH DIAERESIS
	0x00FD,0x0079,0x0301,	// LATIN SMALL LETTER Y WITH ACUTE
//	0x00FF,0x0079,0x0308,	// LATIN SMALL LETTER Y WITH DIAERESIS  // Windows drivers do not support this
	0x0100,0x0041,0x0304,	// LATIN CAPITAL LETTER A WITH MACRON
	0x0101,0x0061,0x0304,	// LATIN SMALL LETTER A WITH MACRON
	0x0102,0x0041,0x0306,	// LATIN CAPITAL LETTER A WITH BREVE
	0x0103,0x0061,0x0306,	// LATIN SMALL LETTER A WITH BREVE
	0x0104,0x0041,0x0328,	// LATIN CAPITAL LETTER A WITH OGONEK
	0x0105,0x0061,0x0328,	// LATIN SMALL LETTER A WITH OGONEK
	0x0106,0x0043,0x0301,	// LATIN CAPITAL LETTER C WITH ACUTE
	0x0107,0x0063,0x0301,	// LATIN SMALL LETTER C WITH ACUTE
	0x0108,0x0043,0x0302,	// LATIN CAPITAL LETTER C WITH CIRCUMFLEX
	0x0109,0x0063,0x0302,	// LATIN SMALL LETTER C WITH CIRCUMFLEX
	0x010A,0x0043,0x0307,	// LATIN CAPITAL LETTER C WITH DOT ABOVE
	0x010B,0x0063,0x0307,	// LATIN SMALL LETTER C WITH DOT ABOVE
	0x010C,0x0043,0x030C,	// LATIN CAPITAL LETTER C WITH CARON
	0x010D,0x0063,0x030C,	// LATIN SMALL LETTER C WITH CARON
	0x010E,0x0044,0x030C,	// LATIN CAPITAL LETTER D WITH CARON
	0x010F,0x0064,0x030C,	// LATIN SMALL LETTER D WITH CARON
	0x0112,0x0045,0x0304,	// LATIN CAPITAL LETTER E WITH MACRON
	0x0113,0x0065,0x0304,	// LATIN SMALL LETTER E WITH MACRON
	0x0114,0x0045,0x0306,	// LATIN CAPITAL LETTER E WITH BREVE
	0x0115,0x0065,0x0306,	// LATIN SMALL LETTER E WITH BREVE
	0x0116,0x0045,0x0307,	// LATIN CAPITAL LETTER E WITH DOT ABOVE
	0x0117,0x0065,0x0307,	// LATIN SMALL LETTER E WITH DOT ABOVE
	0x0118,0x0045,0x0328,	// LATIN CAPITAL LETTER E WITH OGONEK
	0x0119,0x0065,0x0328,	// LATIN SMALL LETTER E WITH OGONEK
	0x011A,0x0045,0x030C,	// LATIN CAPITAL LETTER E WITH CARON
	0x011B,0x0065,0x030C,	// LATIN SMALL LETTER E WITH CARON
	0x011C,0x0047,0x0302,	// LATIN CAPITAL LETTER G WITH CIRCUMFLEX
	0x011D,0x0067,0x0302,	// LATIN SMALL LETTER G WITH CIRCUMFLEX
	0x011E,0x0047,0x0306,	// LATIN CAPITAL LETTER G WITH BREVE
	0x011F,0x0067,0x0306,	// LATIN SMALL LETTER G WITH BREVE
	0x0120,0x0047,0x0307,	// LATIN CAPITAL LETTER G WITH DOT ABOVE
	0x0121,0x0067,0x0307,	// LATIN SMALL LETTER G WITH DOT ABOVE
	0x0122,0x0047,0x0327,	// LATIN CAPITAL LETTER G WITH CEDILLA
	0x0123,0x0067,0x0327,	// LATIN SMALL LETTER G WITH CEDILLA
	0x0124,0x0048,0x0302,	// LATIN CAPITAL LETTER H WITH CIRCUMFLEX
	0x0125,0x0068,0x0302,	// LATIN SMALL LETTER H WITH CIRCUMFLEX
	0x0128,0x0049,0x0303,	// LATIN CAPITAL LETTER I WITH TILDE
	0x0129,0x0069,0x0303,	// LATIN SMALL LETTER I WITH TILDE
	0x012A,0x0049,0x0304,	// LATIN CAPITAL LETTER I WITH MACRON
	0x012B,0x0069,0x0304,	// LATIN SMALL LETTER I WITH MACRON
	0x012C,0x0049,0x0306,	// LATIN CAPITAL LETTER I WITH BREVE
	0x012D,0x0069,0x0306,	// LATIN SMALL LETTER I WITH BREVE
	0x012E,0x0049,0x0328,	// LATIN CAPITAL LETTER I WITH OGONEK
	0x012F,0x0069,0x0328,	// LATIN SMALL LETTER I WITH OGONEK
	0x0130,0x0049,0x0307,	// LATIN CAPITAL LETTER I WITH DOT ABOVE
	0x0134,0x004A,0x0302,	// LATIN CAPITAL LETTER J WITH CIRCUMFLEX
	0x0135,0x006A,0x0302,	// LATIN SMALL LETTER J WITH CIRCUMFLEX
	0x0136,0x004B,0x0327,	// LATIN CAPITAL LETTER K WITH CEDILLA
	0x0137,0x006B,0x0327,	// LATIN SMALL LETTER K WITH CEDILLA
	0x0139,0x004C,0x0301,	// LATIN CAPITAL LETTER L WITH ACUTE
	0x013A,0x006C,0x0301,	// LATIN SMALL LETTER L WITH ACUTE
	0x013B,0x004C,0x0327,	// LATIN CAPITAL LETTER L WITH CEDILLA
	0x013C,0x006C,0x0327,	// LATIN SMALL LETTER L WITH CEDILLA
	0x013D,0x004C,0x030C,	// LATIN CAPITAL LETTER L WITH CARON
	0x013E,0x006C,0x030C,	// LATIN SMALL LETTER L WITH CARON
	0x0143,0x004E,0x0301,	// LATIN CAPITAL LETTER N WITH ACUTE
	0x0144,0x006E,0x0301,	// LATIN SMALL LETTER N WITH ACUTE
	0x0145,0x004E,0x0327,	// LATIN CAPITAL LETTER N WITH CEDILLA
	0x0146,0x006E,0x0327,	// LATIN SMALL LETTER N WITH CEDILLA
	0x0147,0x004E,0x030C,	// LATIN CAPITAL LETTER N WITH CARON
	0x0148,0x006E,0x030C,	// LATIN SMALL LETTER N WITH CARON
	0x014C,0x004F,0x0304,	// LATIN CAPITAL LETTER O WITH MACRON
	0x014D,0x006F,0x0304,	// LATIN SMALL LETTER O WITH MACRON
	0x014E,0x004F,0x0306,	// LATIN CAPITAL LETTER O WITH BREVE
	0x014F,0x006F,0x0306,	// LATIN SMALL LETTER O WITH BREVE
	0x0150,0x004F,0x030B,	// LATIN CAPITAL LETTER O WITH DOUBLE ACUTE
	0x0151,0x006F,0x030B,	// LATIN SMALL LETTER O WITH DOUBLE ACUTE
	0x0154,0x0052,0x0301,	// LATIN CAPITAL LETTER R WITH ACUTE
	0x0155,0x0072,0x0301,	// LATIN SMALL LETTER R WITH ACUTE
	0x0156,0x0052,0x0327,	// LATIN CAPITAL LETTER R WITH CEDILLA
	0x0157,0x0072,0x0327,	// LATIN SMALL LETTER R WITH CEDILLA
	0x0158,0x0052,0x030C,	// LATIN CAPITAL LETTER R WITH CARON
	0x0159,0x0072,0x030C,	// LATIN SMALL LETTER R WITH CARON
	0x015A,0x0053,0x0301,	// LATIN CAPITAL LETTER S WITH ACUTE
	0x015B,0x0073,0x0301,	// LATIN SMALL LETTER S WITH ACUTE
	0x015C,0x0053,0x0302,	// LATIN CAPITAL LETTER S WITH CIRCUMFLEX
	0x015D,0x0073,0x0302,	// LATIN SMALL LETTER S WITH CIRCUMFLEX
	0x015E,0x0053,0x0327,	// LATIN CAPITAL LETTER S WITH CEDILLA
	0x015F,0x0073,0x0327,	// LATIN SMALL LETTER S WITH CEDILLA
	0x0160,0x0053,0x030C,	// LATIN CAPITAL LETTER S WITH CARON
	0x0161,0x0073,0x030C,	// LATIN SMALL LETTER S WITH CARON
//	0x0162,0x0054,0x0327,	// LATIN CAPITAL LETTER T WITH CEDILLA    // Windows drivers do not support this
//	0x0163,0x0074,0x0327,	// LATIN SMALL LETTER T WITH CEDILLA      // Windows drivers do not support this
	0x0164,0x0054,0x030C,	// LATIN CAPITAL LETTER T WITH CARON
	0x0165,0x0074,0x030C,	// LATIN SMALL LETTER T WITH CARON
	0x0168,0x0055,0x0303,	// LATIN CAPITAL LETTER U WITH TILDE
	0x0169,0x0075,0x0303,	// LATIN SMALL LETTER U WITH TILDE
	0x016A,0x0055,0x0304,	// LATIN CAPITAL LETTER U WITH MACRON
	0x016B,0x0075,0x0304,	// LATIN SMALL LETTER U WITH MACRON
	0x016C,0x0055,0x0306,	// LATIN CAPITAL LETTER U WITH BREVE
	0x016D,0x0075,0x0306,	// LATIN SMALL LETTER U WITH BREVE
	0x016E,0x0055,0x030A,	// LATIN CAPITAL LETTER U WITH RING ABOVE
	0x016F,0x0075,0x030A,	// LATIN SMALL LETTER U WITH RING ABOVE
	0x0170,0x0055,0x030B,	// LATIN CAPITAL LETTER U WITH DOUBLE ACUTE
	0x0171,0x0075,0x030B,	// LATIN SMALL LETTER U WITH DOUBLE ACUTE
	0x0172,0x0055,0x0328,	// LATIN CAPITAL LETTER U WITH OGONEK
	0x0173,0x0075,0x0328,	// LATIN SMALL LETTER U WITH OGONEK
	0x0174,0x0057,0x0302,	// LATIN CAPITAL LETTER W WITH CIRCUMFLEX
	0x0175,0x0077,0x0302,	// LATIN SMALL LETTER W WITH CIRCUMFLEX
	0x0176,0x0059,0x0302,	// LATIN CAPITAL LETTER Y WITH CIRCUMFLEX
	0x0177,0x0079,0x0302,	// LATIN SMALL LETTER Y WITH CIRCUMFLEX
//	0x0178,0x0059,0x0308,	// LATIN CAPITAL LETTER Y WITH DIAERESIS  // Windows drivers do not support this
	0x0179,0x005A,0x0301,	// LATIN CAPITAL LETTER Z WITH ACUTE
	0x017A,0x007A,0x0301,	// LATIN SMALL LETTER Z WITH ACUTE
	0x017B,0x005A,0x0307,	// LATIN CAPITAL LETTER Z WITH DOT ABOVE
	0x017C,0x007A,0x0307,	// LATIN SMALL LETTER Z WITH DOT ABOVE
	0x017D,0x005A,0x030C,	// LATIN CAPITAL LETTER Z WITH CARON
	0x017E,0x007A,0x030C,	// LATIN SMALL LETTER Z WITH CARON
	0x01A0,0x004F,0x031B,	// LATIN CAPITAL LETTER O WITH HORN
	0x01A1,0x006F,0x031B,	// LATIN SMALL LETTER O WITH HORN
	0x01AF,0x0055,0x031B,	// LATIN CAPITAL LETTER U WITH HORN
	0x01B0,0x0075,0x031B,	// LATIN SMALL LETTER U WITH HORN
	0x01CD,0x0041,0x030C,	// LATIN CAPITAL LETTER A WITH CARON
	0x01CE,0x0061,0x030C,	// LATIN SMALL LETTER A WITH CARON
	0x01CF,0x0049,0x030C,	// LATIN CAPITAL LETTER I WITH CARON
	0x01D0,0x0069,0x030C,	// LATIN SMALL LETTER I WITH CARON
	0x01D1,0x004F,0x030C,	// LATIN CAPITAL LETTER O WITH CARON
	0x01D2,0x006F,0x030C,	// LATIN SMALL LETTER O WITH CARON
	0x01D3,0x0055,0x030C,	// LATIN CAPITAL LETTER U WITH CARON
	0x01D4,0x0075,0x030C,	// LATIN SMALL LETTER U WITH CARON
	0x01D5,0x00DC,0x0304,	// LATIN CAPITAL LETTER U WITH DIAERESIS AND MACRON
	0x01D6,0x00FC,0x0304,	// LATIN SMALL LETTER U WITH DIAERESIS AND MACRON
	0x01D7,0x00DC,0x0301,	// LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE
	0x01D8,0x00FC,0x0301,	// LATIN SMALL LETTER U WITH DIAERESIS AND ACUTE
	0x01D9,0x00DC,0x030C,	// LATIN CAPITAL LETTER U WITH DIAERESIS AND CARON
	0x01DA,0x00FC,0x030C,	// LATIN SMALL LETTER U WITH DIAERESIS AND CARON
	0x01DB,0x00DC,0x0300,	// LATIN CAPITAL LETTER U WITH DIAERESIS AND GRAVE
	0x01DC,0x00FC,0x0300,	// LATIN SMALL LETTER U WITH DIAERESIS AND GRAVE
	0x01DE,0x00C4,0x0304,	// LATIN CAPITAL LETTER A WITH DIAERESIS AND MACRON
	0x01DF,0x00E4,0x0304,	// LATIN SMALL LETTER A WITH DIAERESIS AND MACRON
	0x01E0,0x0226,0x0304,	// LATIN CAPITAL LETTER A WITH DOT ABOVE AND MACRON
	0x01E1,0x0227,0x0304,	// LATIN SMALL LETTER A WITH DOT ABOVE AND MACRON
	0x01E2,0x00C6,0x0304,	// LATIN CAPITAL LETTER AE WITH MACRON
	0x01E3,0x00E6,0x0304,	// LATIN SMALL LETTER AE WITH MACRON
	0x01E6,0x0047,0x030C,	// LATIN CAPITAL LETTER G WITH CARON
	0x01E7,0x0067,0x030C,	// LATIN SMALL LETTER G WITH CARON
	0x01E8,0x004B,0x030C,	// LATIN CAPITAL LETTER K WITH CARON
	0x01E9,0x006B,0x030C,	// LATIN SMALL LETTER K WITH CARON
	0x01EA,0x004F,0x0328,	// LATIN CAPITAL LETTER O WITH OGONEK
	0x01EB,0x006F,0x0328,	// LATIN SMALL LETTER O WITH OGONEK
	0x01EC,0x01EA,0x0304,	// LATIN CAPITAL LETTER O WITH OGONEK AND MACRON
	0x01ED,0x01EB,0x0304,	// LATIN SMALL LETTER O WITH OGONEK AND MACRON
	0x01EE,0x01B7,0x030C,	// LATIN CAPITAL LETTER EZH WITH CARON
	0x01EF,0x0292,0x030C,	// LATIN SMALL LETTER EZH WITH CARON
	0x01F0,0x006A,0x030C,	// LATIN SMALL LETTER J WITH CARON
	0x01F4,0x0047,0x0301,	// LATIN CAPITAL LETTER G WITH ACUTE
	0x01F5,0x0067,0x0301,	// LATIN SMALL LETTER G WITH ACUTE
	0x01F8,0x004E,0x0300,	// LATIN CAPITAL LETTER N WITH GRAVE
	0x01F9,0x006E,0x0300,	// LATIN SMALL LETTER N WITH GRAVE
	0x01FA,0x00C5,0x0301,	// LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE
	0x01FB,0x00E5,0x0301,	// LATIN SMALL LETTER A WITH RING ABOVE AND ACUTE
	0x01FC,0x00C6,0x0301,	// LATIN CAPITAL LETTER AE WITH ACUTE
	0x01FD,0x00E6,0x0301,	// LATIN SMALL LETTER AE WITH ACUTE
	0x01FE,0x00D8,0x0301,	// LATIN CAPITAL LETTER O WITH STROKE AND ACUTE
	0x01FF,0x00F8,0x0301,	// LATIN SMALL LETTER O WITH STROKE AND ACUTE
	0x0200,0x0041,0x030F,	// LATIN CAPITAL LETTER A WITH DOUBLE GRAVE
	0x0201,0x0061,0x030F,	// LATIN SMALL LETTER A WITH DOUBLE GRAVE
	0x0202,0x0041,0x0311,	// LATIN CAPITAL LETTER A WITH INVERTED BREVE
	0x0203,0x0061,0x0311,	// LATIN SMALL LETTER A WITH INVERTED BREVE
	0x0204,0x0045,0x030F,	// LATIN CAPITAL LETTER E WITH DOUBLE GRAVE
	0x0205,0x0065,0x030F,	// LATIN SMALL LETTER E WITH DOUBLE GRAVE
	0x0206,0x0045,0x0311,	// LATIN CAPITAL LETTER E WITH INVERTED BREVE
	0x0207,0x0065,0x0311,	// LATIN SMALL LETTER E WITH INVERTED BREVE
	0x0208,0x0049,0x030F,	// LATIN CAPITAL LETTER I WITH DOUBLE GRAVE
	0x0209,0x0069,0x030F,	// LATIN SMALL LETTER I WITH DOUBLE GRAVE
	0x020A,0x0049,0x0311,	// LATIN CAPITAL LETTER I WITH INVERTED BREVE
	0x020B,0x0069,0x0311,	// LATIN SMALL LETTER I WITH INVERTED BREVE
	0x020C,0x004F,0x030F,	// LATIN CAPITAL LETTER O WITH DOUBLE GRAVE
	0x020D,0x006F,0x030F,	// LATIN SMALL LETTER O WITH DOUBLE GRAVE
	0x020E,0x004F,0x0311,	// LATIN CAPITAL LETTER O WITH INVERTED BREVE
	0x020F,0x006F,0x0311,	// LATIN SMALL LETTER O WITH INVERTED BREVE
	0x0210,0x0052,0x030F,	// LATIN CAPITAL LETTER R WITH DOUBLE GRAVE
	0x0211,0x0072,0x030F,	// LATIN SMALL LETTER R WITH DOUBLE GRAVE
	0x0212,0x0052,0x0311,	// LATIN CAPITAL LETTER R WITH INVERTED BREVE
	0x0213,0x0072,0x0311,	// LATIN SMALL LETTER R WITH INVERTED BREVE
	0x0214,0x0055,0x030F,	// LATIN CAPITAL LETTER U WITH DOUBLE GRAVE
	0x0215,0x0075,0x030F,	// LATIN SMALL LETTER U WITH DOUBLE GRAVE
	0x0216,0x0055,0x0311,	// LATIN CAPITAL LETTER U WITH INVERTED BREVE
	0x0217,0x0075,0x0311,	// LATIN SMALL LETTER U WITH INVERTED BREVE
	0x0218,0x0053,0x0326,	// LATIN CAPITAL LETTER S WITH COMMA BELOW
	0x0219,0x0073,0x0326,	// LATIN SMALL LETTER S WITH COMMA BELOW
	0x021A,0x0054,0x0326,	// LATIN CAPITAL LETTER T WITH COMMA BELOW
	0x021B,0x0074,0x0326,	// LATIN SMALL LETTER T WITH COMMA BELOW
	0x021E,0x0048,0x030C,	// LATIN CAPITAL LETTER H WITH CARON
	0x021F,0x0068,0x030C,	// LATIN SMALL LETTER H WITH CARON
	0x0226,0x0041,0x0307,	// LATIN CAPITAL LETTER A WITH DOT ABOVE
	0x0227,0x0061,0x0307,	// LATIN SMALL LETTER A WITH DOT ABOVE
	0x0228,0x0045,0x0327,	// LATIN CAPITAL LETTER E WITH CEDILLA
	0x0229,0x0065,0x0327,	// LATIN SMALL LETTER E WITH CEDILLA
	0x022A,0x00D6,0x0304,	// LATIN CAPITAL LETTER O WITH DIAERESIS AND MACRON
	0x022B,0x00F6,0x0304,	// LATIN SMALL LETTER O WITH DIAERESIS AND MACRON
	0x022C,0x00D5,0x0304,	// LATIN CAPITAL LETTER O WITH TILDE AND MACRON
	0x022D,0x00F5,0x0304,	// LATIN SMALL LETTER O WITH TILDE AND MACRON
	0x022E,0x004F,0x0307,	// LATIN CAPITAL LETTER O WITH DOT ABOVE
	0x022F,0x006F,0x0307,	// LATIN SMALL LETTER O WITH DOT ABOVE
	0x0230,0x022E,0x0304,	// LATIN CAPITAL LETTER O WITH DOT ABOVE AND MACRON
	0x0231,0x022F,0x0304,	// LATIN SMALL LETTER O WITH DOT ABOVE AND MACRON
	0x0232,0x0059,0x0304,	// LATIN CAPITAL LETTER Y WITH MACRON
	0x0233,0x0079,0x0304,	// LATIN SMALL LETTER Y WITH MACRON
	0x0344,0x0308,0x0301,	// COMBINING GREEK DIALYTIKA TONOS
	0x0386,0x0391,0x0301,	// GREEK CAPITAL LETTER ALPHA WITH TONOS
	0x0388,0x0395,0x0301,	// GREEK CAPITAL LETTER EPSILON WITH TONOS
	0x0389,0x0397,0x0301,	// GREEK CAPITAL LETTER ETA WITH TONOS
	0x038A,0x0399,0x0301,	// GREEK CAPITAL LETTER IOTA WITH TONOS
	0x038C,0x039F,0x0301,	// GREEK CAPITAL LETTER OMICRON WITH TONOS
	0x038E,0x03A5,0x0301,	// GREEK CAPITAL LETTER UPSILON WITH TONOS
	0x038F,0x03A9,0x0301,	// GREEK CAPITAL LETTER OMEGA WITH TONOS
	0x0390,0x03CA,0x0301,	// GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS
	0x03AA,0x0399,0x0308,	// GREEK CAPITAL LETTER IOTA WITH DIALYTIKA
	0x03AB,0x03A5,0x0308,	// GREEK CAPITAL LETTER UPSILON WITH DIALYTIKA
	0x03AC,0x03B1,0x0301,	// GREEK SMALL LETTER ALPHA WITH TONOS
	0x03AD,0x03B5,0x0301,	// GREEK SMALL LETTER EPSILON WITH TONOS
	0x03AE,0x03B7,0x0301,	// GREEK SMALL LETTER ETA WITH TONOS
	0x03AF,0x03B9,0x0301,	// GREEK SMALL LETTER IOTA WITH TONOS
	0x03B0,0x03CB,0x0301,	// GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS
	0x03CA,0x03B9,0x0308,	// GREEK SMALL LETTER IOTA WITH DIALYTIKA
	0x03CB,0x03C5,0x0308,	// GREEK SMALL LETTER UPSILON WITH DIALYTIKA
	0x03CC,0x03BF,0x0301,	// GREEK SMALL LETTER OMICRON WITH TONOS
	0x03CD,0x03C5,0x0301,	// GREEK SMALL LETTER UPSILON WITH TONOS
	0x03CE,0x03C9,0x0301,	// GREEK SMALL LETTER OMEGA WITH TONOS
	0x03D3,0x03D2,0x0301,	// GREEK UPSILON WITH ACUTE AND HOOK SYMBOL
	0x03D4,0x03D2,0x0308,	// GREEK UPSILON WITH DIAERESIS AND HOOK SYMBOL
	0x0400,0x0415,0x0300,	// CYRILLIC CAPITAL LETTER IE WITH GRAVE
	0x0401,0x0415,0x0308,	// CYRILLIC CAPITAL LETTER IO
	0x0403,0x0413,0x0301,	// CYRILLIC CAPITAL LETTER GJE
	0x0407,0x0406,0x0308,	// CYRILLIC CAPITAL LETTER YI
	0x040C,0x041A,0x0301,	// CYRILLIC CAPITAL LETTER KJE
	0x040D,0x0418,0x0300,	// CYRILLIC CAPITAL LETTER I WITH GRAVE
	0x040E,0x0423,0x0306,	// CYRILLIC CAPITAL LETTER SHORT U
	0x0419,0x0418,0x0306,	// CYRILLIC CAPITAL LETTER SHORT I
	0x0439,0x0438,0x0306,	// CYRILLIC SMALL LETTER SHORT I
	0x0450,0x0435,0x0300,	// CYRILLIC SMALL LETTER IE WITH GRAVE
	0x0451,0x0435,0x0308,	// CYRILLIC SMALL LETTER IO
	0x0453,0x0433,0x0301,	// CYRILLIC SMALL LETTER GJE
	0x0457,0x0456,0x0308,	// CYRILLIC SMALL LETTER YI
	0x045C,0x043A,0x0301,	// CYRILLIC SMALL LETTER KJE
	0x045D,0x0438,0x0300,	// CYRILLIC SMALL LETTER I WITH GRAVE
	0x045E,0x0443,0x0306,	// CYRILLIC SMALL LETTER SHORT U
	0x0476,0x0474,0x030F,	// CYRILLIC CAPITAL LETTER IZHITSA WITH DOUBLE GRAVE ACCENT
	0x0477,0x0475,0x030F,	// CYRILLIC SMALL LETTER IZHITSA WITH DOUBLE GRAVE ACCENT
	0x04C1,0x0416,0x0306,	// CYRILLIC CAPITAL LETTER ZHE WITH BREVE
	0x04C2,0x0436,0x0306,	// CYRILLIC SMALL LETTER ZHE WITH BREVE
	0x04D0,0x0410,0x0306,	// CYRILLIC CAPITAL LETTER A WITH BREVE
	0x04D1,0x0430,0x0306,	// CYRILLIC SMALL LETTER A WITH BREVE
	0x04D2,0x0410,0x0308,	// CYRILLIC CAPITAL LETTER A WITH DIAERESIS
	0x04D3,0x0430,0x0308,	// CYRILLIC SMALL LETTER A WITH DIAERESIS
	0x04D6,0x0415,0x0306,	// CYRILLIC CAPITAL LETTER IE WITH BREVE
	0x04D7,0x0435,0x0306,	// CYRILLIC SMALL LETTER IE WITH BREVE
	0x04DA,0x04D8,0x0308,	// CYRILLIC CAPITAL LETTER SCHWA WITH DIAERESIS
	0x04DB,0x04D9,0x0308,	// CYRILLIC SMALL LETTER SCHWA WITH DIAERESIS
	0x04DC,0x0416,0x0308,	// CYRILLIC CAPITAL LETTER ZHE WITH DIAERESIS
	0x04DD,0x0436,0x0308,	// CYRILLIC SMALL LETTER ZHE WITH DIAERESIS
	0x04DE,0x0417,0x0308,	// CYRILLIC CAPITAL LETTER ZE WITH DIAERESIS
	0x04DF,0x0437,0x0308,	// CYRILLIC SMALL LETTER ZE WITH DIAERESIS
	0x04E2,0x0418,0x0304,	// CYRILLIC CAPITAL LETTER I WITH MACRON
	0x04E3,0x0438,0x0304,	// CYRILLIC SMALL LETTER I WITH MACRON
	0x04E4,0x0418,0x0308,	// CYRILLIC CAPITAL LETTER I WITH DIAERESIS
	0x04E5,0x0438,0x0308,	// CYRILLIC SMALL LETTER I WITH DIAERESIS
	0x04E6,0x041E,0x0308,	// CYRILLIC CAPITAL LETTER O WITH DIAERESIS
	0x04E7,0x043E,0x0308,	// CYRILLIC SMALL LETTER O WITH DIAERESIS
	0x04EA,0x04E8,0x0308,	// CYRILLIC CAPITAL LETTER BARRED O WITH DIAERESIS
	0x04EB,0x04E9,0x0308,	// CYRILLIC SMALL LETTER BARRED O WITH DIAERESIS
	0x04EC,0x042D,0x0308,	// CYRILLIC CAPITAL LETTER E WITH DIAERESIS
	0x04ED,0x044D,0x0308,	// CYRILLIC SMALL LETTER E WITH DIAERESIS
	0x04EE,0x0423,0x0304,	// CYRILLIC CAPITAL LETTER U WITH MACRON
	0x04EF,0x0443,0x0304,	// CYRILLIC SMALL LETTER U WITH MACRON
	0x04F0,0x0423,0x0308,	// CYRILLIC CAPITAL LETTER U WITH DIAERESIS
	0x04F1,0x0443,0x0308,	// CYRILLIC SMALL LETTER U WITH DIAERESIS
	0x04F2,0x0423,0x030B,	// CYRILLIC CAPITAL LETTER U WITH DOUBLE ACUTE
	0x04F3,0x0443,0x030B,	// CYRILLIC SMALL LETTER U WITH DOUBLE ACUTE
	0x04F4,0x0427,0x0308,	// CYRILLIC CAPITAL LETTER CHE WITH DIAERESIS
	0x04F5,0x0447,0x0308,	// CYRILLIC SMALL LETTER CHE WITH DIAERESIS
	0x04F8,0x042B,0x0308,	// CYRILLIC CAPITAL LETTER YERU WITH DIAERESIS
	0x04F9,0x044B,0x0308,	// CYRILLIC SMALL LETTER YERU WITH DIAERESIS
	empty,empty,empty			// end of table
};


/************************************************************************/
/************************************************************************/
/* Dead keys do not show the character on the terminals screen, because they wait
for a second keystroke to compose the final character. If we want to send a dead key alone,
we must add a neutral character (SPACE) */
bool CWedgeMultiKey::AddSpaceToDeadKey(tichar_t ansi)
{
	if (IsTransDead())
	{
		TranslateCharacter(ansi);		// Send the dead key
		TranslateCharacter(' ');		// Add the space
		return TRUE;
	}
	return FALSE;
}

// A helper to find the deadkey in the table
static const DeadKeyTripple_t *LookupDeadKey(wchar_t Character)
{
	const DeadKeyTripple_t *pTripple;

	pTripple = TableDeadKeys;

	// for now a simple linear search
	while (pTripple->composed != empty)
	{
		if(pTripple->composed == Character)
		{
			break;	// found
		}

		pTripple++;
	}
	return pTripple;
}

// Checks whether that key is available
bool CWedgeMultiKey::IsValidKey(const CKey *pKey)
{
	return (pKey->GetKeyNumber() > 0);
}

// Checks whether that key is available and if it is a dead key at all
bool CWedgeMultiKey::IsValidDeadKey(const CKey *pKey)
{
	return ( IsValidKey(pKey) && (pKey->IsDeadKey()) );
}

// Some dead keys have two values assigned in unicode, here we translate to the other one.
wchar_t CWedgeMultiKey::LookUpCompatibleDeadKey(wchar_t DeadCharacter)
{
	const DeadKeyAlias_t *pCompatTable = CompatibleDeadKeys;
	// a simple linear search will do it. It's just a few entries
	while (pCompatTable->unicode != empty)
	{
		if(pCompatTable->unicode == DeadCharacter)
		{
			DeadCharacter = pCompatTable->ansi;
			break;
		}

		pCompatTable++;
	}
	return DeadCharacter;
}

/* We look for a valid dead key combination and compose
the character with two keystrokes */
bool CWedgeMultiKey::TryDeadKey(wchar_t Character)
{
	bool bRet=false;
	wchar_t DeadChar;

	if (Character >= MinTwoKeys)	// no dead keys below
	{
		const DeadKeyTripple_t *pTripple = LookupDeadKey(Character);

		if (pTripple->first != empty)
		{
			if (IsValidDeadKey(GetKey(pTripple->first)) )	// some dead keys have two values in unicode
			{
				DeadChar = pTripple->first;
			}
			else
			{
				DeadChar = LookUpCompatibleDeadKey(pTripple->first);
			}

			if (IsValidDeadKey(GetKey(DeadChar)) )
			{
				if (IsValidKey(GetKey(pTripple->second)) )	// second key of the two key sequence
				{
					TranslateCharacter(DeadChar);
					TranslateCharacter(pTripple->second);
					bRet = true;
				}
			}
		}
	}
	return bRet;
}

bool CWedgeMultiKey::TryNonPrintChar(tchar_t Character)
{
       bool bRet=false;
const NonPrintChar_t  pPrintChar=TableNonPrintChars[(int)Character];
	       TranslateCharacter('[');
	       TranslateCharacter(pPrintChar.first);
	       TranslateCharacter(pPrintChar.second);
             if(pPrintChar.third!=0x00)
		TranslateCharacter(pPrintChar.third);
	        TranslateCharacter(']');
		bRet=true;

	return bRet;
}

// A helper that translates a characters into the command buffer
bool CWedgeMultiKey::TranslateCharacter(tichar_t character)
{
	const CKey *pKey;
	pKey	= GetKey(character);
	return TranslateKey(pKey, character);
}

/************************************************************************/
bool CWedgeMultiKey::SendAltNumPad(const tichar_t * pFormat, UINT character)
{
	int size;
	tichar_t cBuffer[9+1];
	size_t i;
	keynum_t alt_keynum;
	CKey key;

	// make the number string that we will send via the numpad
	size = swprintf(cBuffer, 9, pFormat, character);

	/* Search through the various pointers to get the NumPad */
	const CChar2Key *pUSA = m_pTerminal->GetLayout();

	/* Ensure we have a valid NumPad field */
	if (pUSA->IsNumPadSupported())
	{
		alt_keynum = GetModifierKeyNumber(CWedgeTerminal::INDEX_LEFT_ALT);

		/* Ensure the keyboard has got a left ALT key */
		if (alt_keynum > 0)
		{
			/* Press the ALT key */
			DoMake(alt_keynum, EX_MODIFIER);
			AddDelayCommand(GetMediumDelay());

			/* Send digits as NumPad digits */
			for(i=0; i<size; i++)
			{
				if (pUSA->GetNumPadKeyDetails(cBuffer[i], key.m_keynum, key.m_modifier))
					TranslateKey(&key, cBuffer[i]);
			}

			/* Release the ALT key */
			DoBreak(alt_keynum, EX_MODIFIER, 0);
			StoreModifierBreak(MO_AltLeft);					/* Store that we released this one */
			AddDelayCommand(GetMediumDelay());
			return TRUE;
		}
	}
	return FALSE;
}

bool CWedgeMultiKey::TryAltNumPadWin(tichar_t character, codepage_t codepage)
{
	UCHAR ansi;

	// translate to WIN CP
	if( IsOK(theTransCode.UnicodeToSingleByte(codepage, character, ansi)) )
	{
		/* Windows format uses a leading zero and windows codepage ordinal number (decimal) */
		return SendAltNumPad(_TI("0%i"), ansi);
	}
	return FALSE;
}

bool CWedgeMultiKey::TryAltNumPadDos(tichar_t character, codepage_t codepage)
{
	UCHAR oem;

	// translate to OEM CP
	if( IsOK(theTransCode.UnicodeToSingleByte(codepage, character, oem)) )
	{
		/* Windows format uses a leading zero and windows codepage ordinal number (decimal) */
		return SendAltNumPad(_TI("%i"), oem);
	}
	return FALSE;
}

// This mode is not supported by the operating system itself (Win2000, XP).
// However, the Richedit3 control does support it nicely. At least a chance to use it.
bool CWedgeMultiKey::TryAltNumPadUnicode(tichar_t character)
{
	/* Richedit3 controls in windows can receive this. unicode ordinal number (decimal) */
	return SendAltNumPad(_TI("%04i"), character);
}

/************************************************************************
HTLM format means to sen characters as thier decimal number with a header.
This is the '{': &#123
  */
bool CWedgeMultiKey::TryHTMLFormat(tichar_t character)
{
	CKey *pKey;
	tichar_t *pString;
	tichar_t cBuffer[9+1];
	swprintf(cBuffer, 9, _TI("&#%i;"), (UINT)character);
	pString = cBuffer;

	// check if all characters in the new string are supported
	while (*pString !='\0')
	{
		pKey = GetKey(*pString++);
		if (0 == pKey->GetKeyNumber())
			return FALSE;		/* found a non supported character */
	}
	/* All characters are supported, now send it */
	pString = cBuffer;
	while (*pString !='\0')
	{
		TranslateCharacter(*pString++);
	}
	return TRUE;
}
/************************************************************************/
// Now sends 4 digits. I hope nobody used the old format, but it is a debug tool anyway.
void CWedgeMultiKey::SendInHexFormat(tichar_t character)
{
	const tichar_t *pFormat;
	tichar_t cBuffer[9+1];
	CKey *pKey = GetKey('1');				/* See if the numbers need a shift (France ..) */
	if (pKey->IsShifted())
		pFormat = _TI("%04X");			/* in France use uppercase letters to avoid the shift (speed) */
	else
		pFormat = _TI("%04x");			/* rest uses lowercase letters to avoid the shift (speed) */

	swprintf(cBuffer, 9, pFormat, character);
	TranslateCharacter(' ');
	TranslateCharacter(cBuffer[0]);
	TranslateCharacter(cBuffer[1]);
	TranslateCharacter(cBuffer[2]);
	TranslateCharacter(cBuffer[3]);
}
/************************************************************************/
/************************************************************************/
/* Try to send an unsuported character via multiple key methods */
bool CWedgeMultiKey::MultiKey(tichar_t character)
{
	if (IsTransDead())
	{
		if (TryDeadKey(character))
			return TRUE;
	}

	if (IsTransAltNpUnicode())
	{
		if (TryAltNumPadUnicode(character))
			return TRUE;
	}
	else if (IsTransAltNpWin())
	{
		codepage_t codepage = theTransCode.GetWinCodepage(GetCountry());
		if (TryAltNumPadWin(character, codepage))
			return TRUE;
	}
	else if (IsTransAltNpDos())
	{
		codepage_t codepage = theTransCode.GetDOSCodepage(GetCountry());
		if (TryAltNumPadDos(character, codepage))
			return TRUE;
	}
	else if (IsTransAltNpWinSysCp())
	{
		codepage_t codepage = theTransCode.GetSystemCodepage();
		if (TryAltNumPadWin(character, codepage))
			return TRUE;
	}
	else if (IsTransAltNpDosSysCp())
	{
		codepage_t codepage = theTransCode.GetSystemCodepage();
		if (TryAltNumPadDos(character, codepage))
			return TRUE;
	}

	if (IsTransHtml())
	{
		if (TryHTMLFormat(character))
			return TRUE;
	}
	return FALSE;
}


void CWedgeMultiKey::Translate(tichar_t ansi)
{

	bool sent=FALSE;
	m_CmdBuffer.PrepareWrite();
	const CKey *key;
	key	= m_KeyNumLookup.GetKey(ansi);
	if (IsTransHex())
	{
		SendInHexFormat(ansi);	/* kind a hex dump format */
	}
	else
	{
		if (key->IsDeadKey())
		{ 	/* Dead keys need special attention */
			sent = AddSpaceToDeadKey(ansi);
		}
		if (!sent)
		{
			   if((ansi<0x20)&& IsTransNonPrint())
			  	sent=TryNonPrintChar(ansi);
			 else
				sent = TranslateCharacter(ansi);
		}
		if (!sent)
		{	/* Key is not available on this keyboard, try multiple keys */
			MultiKey(ansi);
		}
	}
	m_CmdBuffer.AddEndCommand();
}

void CWedgeMultiKey::InitializeKeyLookup(void)
{
	/* prepare the character to keynumber translation table  */
	const CChar2Key *pUsa = m_pTerminal->GetLayout();

	m_KeyNumLookup.Initialize(pUsa);

	/* international support */
	DoInternationalSupport(m_pTerminal->GetFixups(), GetCountry());

	/* some keyboard styles */
	HandleNumbersViaNumPad(pUsa);
	HandleCntrlPlusAscii();

	/* prokey support */
	ProkeyReplacements();

	/* Caps lock keyboard style after prokey ! */
	if(!GetMode())
		HandleCapsLock();	/* The other keyboard styles are handled later at the time we send the label */      // Dangerous Jess
	else
		HandleKbdMode();

}

MatrixError_t CWedgeMultiKey::Open(interface_id_t nId, const tichar_t *pParameter)
{
	MatrixError_t ret = baseCWedgeMultiKey::Open(nId, pParameter);
	if(IsOK(ret))
	{
		InitializeKeyLookup();
		WdgStoreConfigurationValid(TRUE);
	}
	return ret;
}

// Support for KBDCAS3 menu command
void CWedgeMultiKey::InitializeCtrlASCIIKeyLookup(void)
{
   /* prepare the character to keynumber translation table  */
   const CChar2Key *pUsa = m_pTerminal->GetLayout();

   m_KeyNumLookup.InitializeCtrlASCII(pUsa);

   /* some keyboard styles */
   HandleCntrlPlusAscii();

   /* prokey support */
   ProkeyReplacements();
}





