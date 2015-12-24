/* ==============================================================================
   ST3800 Wedge International translation tables Module Source (Hand made)
   ==============================================================================

   $RCSfile: interfaces_wedge/keyint.cpp $
   $Revision: 2.2 $
   $Date: 2009/06/29 05:02:28EDT $

   ==============================================================================

   ============================================================================== */


/* ==============================================================================
   Use the Windows ANSI code page (CP1252) to view this file!
 	Place all hand made international support into this file.
   ============================================================================== */

#include "stdInclude.h"
/* This single include file allows for adding you special requirments */
#include "internat.h"

			  #ifdef UNICODE_FOR_INTERFACES

#define INTERNATIONAL_TABLE_VERSION 201	/* This version gets changed when table structure changes */

/* Here we test whether the format versions match */
#if (INTERNATIONAL_PRG_VERSION != INTERNATIONAL_TABLE_VERSION)
#error "International tables do have the wrong version"
#endif

#define NOS 0	/* Character is not supported by this leyout */

extern const CKeyFixups JAP_106_USB_DEC_VT510_PC_FixupPtr;

extern const CKeyFixups JAP_106_USB_IBM_AT_FixupPtr;

// below are dummy entries for terminals with only USA layouts
const CKeyFixup NoExceptions[1] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* end */ 0,KN_ENDMARKER,0
};

const CKeyFixup *USA_Only_Fixups[1] =
{
	NoExceptions,
};


extern const CKeyFixups USA_Only_FixupPtr =
{
	NULL,                         		// pointer to parent table (none here)
	USA_Only_Fixups,
	00, 00,                       		// min = USA, max = USA
};

//*********************************************************************************
//*********************************************************************************

/******************************************************/
/* Exception tables                                   */
/******************************************************/

/* TermId=  3 	,  2 , 97 ,106 ,124 */
const CKeyFixup JAP_106_USB_DEC_VT510_PC_Japanese_ASCII[99] =
{
	/*        Unicode,  KN, MODIFIERS */
	/* ' ' */ 0x0020,  61, 0,
	/* '!' */ 0x0021,   2, MO_ShiftLeft,
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023,   4, MO_ShiftLeft,
	/* '$' */ 0x0024,   5, MO_ShiftLeft,
	/* '%' */ 0x0025,   6, MO_ShiftLeft,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,   8, MO_ShiftLeft,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  41, MO_ShiftLeft,
	/* '+' */ 0x002B,  40, MO_ShiftLeft,
	/* ',' */ 0x002C,  53, 0,
	/* '-' */ 0x002D,  12, 0,
	/* '.' */ 0x002E,  54, 0,
	/* '/' */ 0x002F,  55, 0,
	/* '0' */ 0x0030,  11, 0,
	/* '1' */ 0x0031,   2, 0,
	/* '2' */ 0x0032,   3, 0,
	/* '3' */ 0x0033,   4, 0,
	/* '4' */ 0x0034,   5, 0,
	/* '5' */ 0x0035,   6, 0,
	/* '6' */ 0x0036,   7, 0,
	/* '7' */ 0x0037,   8, 0,
	/* '8' */ 0x0038,   9, 0,
	/* '9' */ 0x0039,  10, 0,
	/* ':' */ 0x003A,  41, 0,
	/* ';' */ 0x003B,  40, 0,
	/* '<' */ 0x003C,  53, MO_ShiftLeft,
	/* '=' */ 0x003D,  12, MO_ShiftLeft,
	/* '>' */ 0x003E,  54, MO_ShiftLeft,
	/* '?' */ 0x003F,  55, MO_ShiftLeft,
	/* '@' */ 0x0040,  27, 0,
	/* 'A' */ 0x0041,  31, MO_ShiftLeft,
	/* 'B' */ 0x0042,  50, MO_ShiftLeft,
	/* 'C' */ 0x0043,  48, MO_ShiftLeft,
	/* 'D' */ 0x0044,  33, MO_ShiftLeft,
	/* 'E' */ 0x0045,  19, MO_ShiftLeft,
	/* 'F' */ 0x0046,  34, MO_ShiftLeft,
	/* 'G' */ 0x0047,  35, MO_ShiftLeft,
	/* 'H' */ 0x0048,  36, MO_ShiftLeft,
	/* 'I' */ 0x0049,  24, MO_ShiftLeft,
	/* 'J' */ 0x004A,  37, MO_ShiftLeft,
	/* 'K' */ 0x004B,  38, MO_ShiftLeft,
	/* 'L' */ 0x004C,  39, MO_ShiftLeft,
	/* 'M' */ 0x004D,  52, MO_ShiftLeft,
	/* 'N' */ 0x004E,  51, MO_ShiftLeft,
	/* 'O' */ 0x004F,  25, MO_ShiftLeft,
	/* 'P' */ 0x0050,  26, MO_ShiftLeft,
	/* 'Q' */ 0x0051,  17, MO_ShiftLeft,
	/* 'R' */ 0x0052,  20, MO_ShiftLeft,
	/* 'S' */ 0x0053,  32, MO_ShiftLeft,
	/* 'T' */ 0x0054,  21, MO_ShiftLeft,
	/* 'U' */ 0x0055,  23, MO_ShiftLeft,
	/* 'V' */ 0x0056,  49, MO_ShiftLeft,
	/* 'W' */ 0x0057,  18, MO_ShiftLeft,
	/* 'X' */ 0x0058,  47, MO_ShiftLeft,
	/* 'Y' */ 0x0059,  22, MO_ShiftLeft,
	/* 'Z' */ 0x005A,  46, MO_ShiftLeft,
	/* '[' */ 0x005B,  28, 0,
	/* '\' */ 0x005C,  56, 0,
	/* ']' */ 0x005D,  42, 0,
	/* '^' */ 0x005E,  13, 0,
	/* '_' */ 0x005F,  56, MO_ShiftLeft,
	/* '`' */ 0x0060,  27, MO_ShiftLeft,
	/* 'a' */ 0x0061,  31, 0,
	/* 'b' */ 0x0062,  50, 0,
	/* 'c' */ 0x0063,  48, 0,
	/* 'd' */ 0x0064,  33, 0,
	/* 'e' */ 0x0065,  19, 0,
	/* 'f' */ 0x0066,  34, 0,
	/* 'g' */ 0x0067,  35, 0,
	/* 'h' */ 0x0068,  36, 0,
	/* 'i' */ 0x0069,  24, 0,
	/* 'j' */ 0x006A,  37, 0,
	/* 'k' */ 0x006B,  38, 0,
	/* 'l' */ 0x006C,  39, 0,
	/* 'm' */ 0x006D,  52, 0,
	/* 'n' */ 0x006E,  51, 0,
	/* 'o' */ 0x006F,  25, 0,
	/* 'p' */ 0x0070,  26, 0,
	/* 'q' */ 0x0071,  17, 0,
	/* 'r' */ 0x0072,  20, 0,
	/* 's' */ 0x0073,  32, 0,
	/* 't' */ 0x0074,  21, 0,
	/* 'u' */ 0x0075,  23, 0,
	/* 'v' */ 0x0076,  49, 0,
	/* 'w' */ 0x0077,  18, 0,
	/* 'x' */ 0x0078,  47, 0,
	/* 'y' */ 0x0079,  22, 0,
	/* 'z' */ 0x007A,  46, 0,
	/* '{' */ 0x007B,  28, MO_ShiftLeft,
	/* '|' */ 0x007C,  14, MO_ShiftLeft,
	/* '}' */ 0x007D,  42, MO_ShiftLeft,
	/* '~' */ 0x007E,  13, MO_ShiftLeft,
	/*     */ 0x007F, 104, 0,
	/* '¥' */ 0x00A5,  14, 0,              /* 0xA5 is a '¥' ,appears as a 0xD1 to this interface with PDF for yet to be discovered reasons*/
	/* '¥' */ 0x00D1,  14, 0,               /*temporary work around for PDF*/
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	98 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		392 (0x188) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		294 (0x126) bytes
*/

const CKeyFixup *JAP_106_USB_DEC_VT510_PC_Fixups[76] =
{
	JAP_106_USB_DEC_VT510_PC_Japanese_ASCII,	// 00 USA  required, no country selected in this interface
	NoExceptions,			                 		// 01
	NoExceptions,					         		// 02
	NoExceptions,		                  		// 03
	NoExceptions,		                  		// 04
	NoExceptions,			                 		// 05
	NoExceptions,				            		// 06
	NoExceptions,			                 		// 07
	NoExceptions,		                  		// 08
	NoExceptions,		                  		// 09
	NoExceptions,			                 		// 10
	NoExceptions,		                   		// 11
	NoExceptions,                         		// 12 Hebrew not required
	NoExceptions,				              		// 13
	NoExceptions,					           		// 14
	NoExceptions,			               		// 15
	NoExceptions,			                		// 16
	NoExceptions,			               		// 17
	NoExceptions,					         		// 18
	NoExceptions,			               		// 19
	NoExceptions,		                  		// 20
	NoExceptions,	                     		// 21
	NoExceptions,					           		// 22
	NoExceptions,			                 		// 23
	NoExceptions,			               		// 24
	NoExceptions,			                		// 25
	NoExceptions,			                 		// 26
	NoExceptions,  			             		// 27
	JAP_106_USB_DEC_VT510_PC_Japanese_ASCII,  // 28 Japanese_ASCII required, just in case it is selected
	NoExceptions,				            		// 29
	NoExceptions,						       		// 30
	NoExceptions,			               		// 31
	NoExceptions,			                		// 32
	NULL,                                 		// 33 Bosnian not supported
	NoExceptions,				              		// 34
	NoExceptions,			                		// 35
	NoExceptions,					           		// 36
	NoExceptions,						        		// 37
	NoExceptions,				            		// 38
	NoExceptions,				            		// 39
	NoExceptions,						       		// 40
	NoExceptions,			                		// 41
	NoExceptions, 			                 		// 42
	NoExceptions,					          		// 43
	NoExceptions,			               		// 44
	NoExceptions,					          		// 45
	NULL,                                 		// 46 Reserved5 not supported
	NULL,                                 		// 47 Reserved6 not supported
	NoExceptions,						        		// 48
	NoExceptions,						        		// 49
	NoExceptions,						       		// 50
	NoExceptions,						       		// 51
	NoExceptions,						      		// 52
	NoExceptions,					         		// 53
	NoExceptions,								  		// 54
	NoExceptions,							   		// 55
	NoExceptions,				             		// 56
	NoExceptions,				              		// 57
	NoExceptions,						      		// 58
	NoExceptions,				             		// 59
	NoExceptions,					         		// 60
	NoExceptions,			               		// 61
	NoExceptions,			               		// 62
	NoExceptions,				             		// 63
	NoExceptions,					         		// 64
	NoExceptions,					         		// 65
	NoExceptions,			                		// 66
	NoExceptions,			               		// 67
	NoExceptions,       				        		// 68
	NULL,                                 		// 69 Reserved1 not supported
	NULL,                                 		// 70 Reserved2 not supported
	NULL,                                 		// 71 Reserved3 not supported
	NULL,                                 		// 72 Reserved4 not supported
	NoExceptions,                   				// 73
	NULL,                                 		// 74 Malta not supported
	NoExceptions,               					// 75
};

// TermId=  5
const CKeyFixups JAP_106_USB_DEC_VT510_PC_FixupPtr =
{
	NULL,                                 		// pointer to parent table (none here)
	JAP_106_USB_DEC_VT510_PC_Fixups,          // pointer fixup table
	00, 75,                               		// min = USA, max = Icelandic
};

const CKeyFixup *JAP_106_USB_IBM_AT_Fixups[76] =
{
	NoExceptions,                         		// 00 USA not required
	NoExceptions,                         		// 01 Belgian not required
	NoExceptions,                         		// 02 Finnish (Swedish) not required
	NoExceptions,                         		// 03 French not required
	NoExceptions,                         		// 04 German not required
	NoExceptions,                         		// 05 Italian not required
	NoExceptions,                         		// 06 Swiss (German) not required
	NoExceptions,                         		// 07 British not required
	NoExceptions,                         		// 08 Danish not required
	NoExceptions,                         		// 09 Norway not required
	NoExceptions,                         		// 10 Spanish not required
	NoExceptions,                         		// 11 Dutch not required
	NoExceptions,                         		// 12 Hebrew not required
	NoExceptions,                         		// 13 Portuguese not required
	NoExceptions,                         		// 14 Latin America not required
	NoExceptions,                         		// 15 Czech DEC not required
	NoExceptions,                         		// 16 Brazilia not required
	NoExceptions,                         		// 17 Greek DEC not required
	NoExceptions,                         		// 18 Canadian French not required
	NoExceptions,                         		// 19 Hungarian not required
	NoExceptions,                         		// 20 Polish not required
	NoExceptions,                         		// 21 SCS not required
	NoExceptions,                         		// 22 Slovakian DEC not required
	NoExceptions,                         		// 23 Swedish not required
	NoExceptions,                     			// 24
	NoExceptions,                         		// 25 Romanian not required
	NoExceptions,                         		// 26 Russian not required
	NoExceptions,                     			// 27
	NoExceptions,                         		// 28 Japanese ASCII not required
	NoExceptions,                         		// 29 Swiss (French) not required
	NoExceptions,                         		// 30 USA International not required
	NoExceptions,                         		// 31 Slovenian not required
	NoExceptions,                         		// 32 Croatian not required
	NULL,                                 		// 33 Bosnian not supported
	NoExceptions,                         		// 34 Macedonian not required
	NoExceptions,                         		// 35 Albanian not required
	NoExceptions,                         		// 36 Serbian Latin not required
	NoExceptions,                         		// 37 Serbian Cyrillic not required
	NoExceptions,                         		// 38 Czech QWERTZ not required
	NoExceptions,                         		// 39 Czech QWERTY not required
	NoExceptions,                         		// 40 Czech Programmers not required
	NoExceptions,                         		// 41 Estonian not required
	NoExceptions,                         		// 42 Latvian not required
	NoExceptions,                         		// 43 Latvian QWERTY not required
	NoExceptions,                         		// 44 Lithuania not required
	NoExceptions,                         		// 45 Lithuanian IBM not required
	NULL,                                 		// 46 Reserved5 not supported
	NULL,                                 		// 47 Reserved6 not supported
	NoExceptions,                         		// 48 Slovakian QWERTZ not required
	NoExceptions,                         		// 49 Slovakian QWERTY not required
	NoExceptions,                         		// 50 Hungarian 101 Key not required
	NoExceptions,                         		// 51 Spanish Variation not required
	NoExceptions,                         		// 52 Bulgarian Cyrillic not required
	NoExceptions,                         		// 53 Bulgarian Latin not required
	NoExceptions,                         		// 54 Canadian French Legacy not required
	NoExceptions,                         		// 55 Canadian Multilingual not required
	NoExceptions,                         		// 56 Italian 142 not required
	NoExceptions,                         		// 57 Polish 214 not required
	NoExceptions,                         		// 58 Polish Programmers not required
	NoExceptions,                         		// 59 Brazilia MS not required
	NoExceptions,                         		// 60 Greek Polytonic not required
	NoExceptions,                         		// 61 Greek 220 not required
	NoExceptions,                         		// 62 Greek 319 not required
	NoExceptions,                         		// 63 Greek Latin not required
	NoExceptions,                         		// 64 Greek 220 Latin not required
	NoExceptions,                         		// 65 Greek 319 Latin not required
	NoExceptions,                         		// 66 Greek MS not required
	NoExceptions,                         		// 67 Russian MS not required
	NoExceptions,                         		// 68 Russian Typewriter not required
	NULL,                                 		// 69 Reserved1 not supported
	NULL,                                 		// 70 Reserved2 not supported
	NULL,                                 		// 71 Reserved3 not supported
	NULL,                                 		// 72 Reserved4 not supported
	NoExceptions,                         		// 73 Irish not required
	NULL,                                 		// 74 Malta not supported
	NoExceptions,                         		// 75 Icelandic not required
};

 // TermId=  3 	,  2 , 97 ,106 ,124
const CKeyFixups JAP_106_USB_IBM_AT_FixupPtr =
{
	&JAP_106_USB_DEC_VT510_PC_FixupPtr,          // pointer to parent table
	JAP_106_USB_IBM_AT_Fixups,                	// pointer fixup table
	00, 75,                               			// min = USA, max = Icelandic
};


//*********************************************************************************
//*********************************************************************************

#endif

//*********************************************************************************
//*********************************************************************************

#ifndef UNICODE_FOR_INTERFACES	                        // manually added

#define INTERNATIONAL_TABLE_VERSION 100	/* This version gets changed when table structure changes */

/* Here we test whether the format versions match */
#if (INTERNATIONAL_PRG_VERSION != INTERNATIONAL_TABLE_VERSION)
#error "International tables do have the wrong version"
#endif
/* This is used if we need no exceptions for a particular country           */
/* It allows us to use the NULL pointer for marking not supported countries */
const CKeyFixup NoExceptions[1] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* end */ 0,KN_ENDMARKER,0
};

#define NOS 0	/* Character is not supported by this leyout */

extern const CKeyFixups JAP_106_USB_IBM_AT_FixupPtr;

extern const CKeyFixups JAP_106_USB_DEC_VT510_PC_FixupPtr;

extern const CKeyFixups JAP_106_USB_DEC_VT510_LK411_FixupPtr;


/******************************************************/
/* Exception tables                                   */
/******************************************************/

/* TermId=  3 	,  2 , 97 ,106 ,124 */
const CKeyFixup JAP_106_USB_IBM_AT_JAPA[99] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* ' ' */ 0x20,  61, 0,
	/* '!' */ 0x21,   2, MO_ShiftLeft,
	/* '"' */ 0x22,   3, MO_ShiftLeft,
	/* '#' */ 0x23,   4, MO_ShiftLeft,
	/* '$' */ 0x24,   5, MO_ShiftLeft,
	/* '%' */ 0x25,   6, MO_ShiftLeft,
	/* '&' */ 0x26,   7, MO_ShiftLeft,
	/* ''' */ 0x27,   8, MO_ShiftLeft,
	/* '(' */ 0x28,   9, MO_ShiftLeft,
	/* ')' */ 0x29,  10, MO_ShiftLeft,
	/* '*' */ 0x2A,  41, MO_ShiftLeft,
	/* '+' */ 0x2B,  40, MO_ShiftLeft,
	/* ',' */ 0x2C,  53, 0,
	/* '-' */ 0x2D,  12, 0,
	/* '.' */ 0x2E,  54, 0,
	/* '/' */ 0x2F,  55, 0,
	/* '0' */ 0x30,  11, 0,
	/* '1' */ 0x31,   2, 0,
	/* '2' */ 0x32,   3, 0,
	/* '3' */ 0x33,   4, 0,
	/* '4' */ 0x34,   5, 0,
	/* '5' */ 0x35,   6, 0,
	/* '6' */ 0x36,   7, 0,
	/* '7' */ 0x37,   8, 0,
	/* '8' */ 0x38,   9, 0,
	/* '9' */ 0x39,  10, 0,
	/* ':' */ 0x3A,  41, 0,
	/* ';' */ 0x3B,  40, 0,
	/* '<' */ 0x3C,  53, MO_ShiftLeft,
	/* '=' */ 0x3D,  12, MO_ShiftLeft,
	/* '>' */ 0x3E,  54, MO_ShiftLeft,
	/* '?' */ 0x3F,  55, MO_ShiftLeft,
	/* '@' */ 0x40,  27, 0,
	/* 'A' */ 0x41,  31, MO_ShiftLeft,
	/* 'B' */ 0x42,  50, MO_ShiftLeft,
	/* 'C' */ 0x43,  48, MO_ShiftLeft,
	/* 'D' */ 0x44,  33, MO_ShiftLeft,
	/* 'E' */ 0x45,  19, MO_ShiftLeft,
	/* 'F' */ 0x46,  34, MO_ShiftLeft,
	/* 'G' */ 0x47,  35, MO_ShiftLeft,
	/* 'H' */ 0x48,  36, MO_ShiftLeft,
	/* 'I' */ 0x49,  24, MO_ShiftLeft,
	/* 'J' */ 0x4A,  37, MO_ShiftLeft,
	/* 'K' */ 0x4B,  38, MO_ShiftLeft,
	/* 'L' */ 0x4C,  39, MO_ShiftLeft,
	/* 'M' */ 0x4D,  52, MO_ShiftLeft,
	/* 'N' */ 0x4E,  51, MO_ShiftLeft,
	/* 'O' */ 0x4F,  25, MO_ShiftLeft,
	/* 'P' */ 0x50,  26, MO_ShiftLeft,
	/* 'Q' */ 0x51,  17, MO_ShiftLeft,
	/* 'R' */ 0x52,  20, MO_ShiftLeft,
	/* 'S' */ 0x53,  32, MO_ShiftLeft,
	/* 'T' */ 0x54,  21, MO_ShiftLeft,
	/* 'U' */ 0x55,  23, MO_ShiftLeft,
	/* 'V' */ 0x56,  49, MO_ShiftLeft,
	/* 'W' */ 0x57,  18, MO_ShiftLeft,
	/* 'X' */ 0x58,  47, MO_ShiftLeft,
	/* 'Y' */ 0x59,  22, MO_ShiftLeft,
	/* 'Z' */ 0x5A,  46, MO_ShiftLeft,
	/* '[' */ 0x5B,  28, 0,
	/* '\' */ 0x5C,  56, 0,
	/* ']' */ 0x5D,  42, 0,
	/* '^' */ 0x5E,  13, 0,
	/* '_' */ 0x5F,  56, MO_ShiftLeft,
	/* '`' */ 0x60,  27, MO_ShiftLeft,
	/* 'a' */ 0x61,  31, 0,
	/* 'b' */ 0x62,  50, 0,
	/* 'c' */ 0x63,  48, 0,
	/* 'd' */ 0x64,  33, 0,
	/* 'e' */ 0x65,  19, 0,
	/* 'f' */ 0x66,  34, 0,
	/* 'g' */ 0x67,  35, 0,
	/* 'h' */ 0x68,  36, 0,
	/* 'i' */ 0x69,  24, 0,
	/* 'j' */ 0x6A,  37, 0,
	/* 'k' */ 0x6B,  38, 0,
	/* 'l' */ 0x6C,  39, 0,
	/* 'm' */ 0x6D,  52, 0,
	/* 'n' */ 0x6E,  51, 0,
	/* 'o' */ 0x6F,  25, 0,
	/* 'p' */ 0x70,  26, 0,
	/* 'q' */ 0x71,  17, 0,
	/* 'r' */ 0x72,  20, 0,
	/* 's' */ 0x73,  32, 0,
	/* 't' */ 0x74,  21, 0,
	/* 'u' */ 0x75,  23, 0,
	/* 'v' */ 0x76,  49, 0,
	/* 'w' */ 0x77,  18, 0,
	/* 'x' */ 0x78,  47, 0,
	/* 'y' */ 0x79,  22, 0,
	/* 'z' */ 0x7A,  46, 0,
	/* '{' */ 0x7B,  28, MO_ShiftLeft,
	/* '|' */ 0x7C,  14, MO_ShiftLeft,
	/* '}' */ 0x7D,  42, MO_ShiftLeft,
	/* '~' */ 0x7E,  13, MO_ShiftLeft,
	/*     */ 0x7F, 104, 0,
	/* '¥' */ 0xA5,  14, 0,              /* 0xA5 is a '¥' ,appears as a 0xD1 to this interface with PDF for yet to be discovered reasons*/
	/* '¥' */ 0xD1,  14, 0,               /*temporary work around for PDF*/
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	98 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		392 (0x188) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		294 (0x126) bytes
*/


/* TermId=  5 */
const CKeyFixup JAP_106_USB_DEC_VT510_PC_JAPA[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '\' */ 0x5C, NOS, 0,
	/* '|' */ 0x7C, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	3 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		12 (0xC) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		9 (0x9) bytes
*/


/* TermId=104 */
const CKeyFixup JAP_106_USB_DEC_VT510_LK411_JAPA[97] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* ' ' */ 0x20, NOS, 0,
	/* '!' */ 0x21, NOS, 0,
	/* '"' */ 0x22, NOS, 0,
	/* '#' */ 0x23, NOS, 0,
	/* '$' */ 0x24, NOS, 0,
	/* '%' */ 0x25, NOS, 0,
	/* '&' */ 0x26, NOS, 0,
	/* ''' */ 0x27, NOS, 0,
	/* '(' */ 0x28, NOS, 0,
	/* ')' */ 0x29, NOS, 0,
	/* '*' */ 0x2A, NOS, 0,
	/* '+' */ 0x2B, NOS, 0,
	/* ',' */ 0x2C, NOS, 0,
	/* '-' */ 0x2D, NOS, 0,
	/* '.' */ 0x2E, NOS, 0,
	/* '/' */ 0x2F, NOS, 0,
	/* '0' */ 0x30, NOS, 0,
	/* '1' */ 0x31, NOS, 0,
	/* '2' */ 0x32, NOS, 0,
	/* '3' */ 0x33, NOS, 0,
	/* '4' */ 0x34, NOS, 0,
	/* '5' */ 0x35, NOS, 0,
	/* '6' */ 0x36, NOS, 0,
	/* '7' */ 0x37, NOS, 0,
	/* '8' */ 0x38, NOS, 0,
	/* '9' */ 0x39, NOS, 0,
	/* ':' */ 0x3A, NOS, 0,
	/* ';' */ 0x3B, NOS, 0,
	/* '<' */ 0x3C, NOS, 0,
	/* '=' */ 0x3D, NOS, 0,
	/* '>' */ 0x3E, NOS, 0,
	/* '?' */ 0x3F, NOS, 0,
	/* '@' */ 0x40, NOS, 0,
	/* 'A' */ 0x41, NOS, 0,
	/* 'B' */ 0x42, NOS, 0,
	/* 'C' */ 0x43, NOS, 0,
	/* 'D' */ 0x44, NOS, 0,
	/* 'E' */ 0x45, NOS, 0,
	/* 'F' */ 0x46, NOS, 0,
	/* 'G' */ 0x47, NOS, 0,
	/* 'H' */ 0x48, NOS, 0,
	/* 'I' */ 0x49, NOS, 0,
	/* 'J' */ 0x4A, NOS, 0,
	/* 'K' */ 0x4B, NOS, 0,
	/* 'L' */ 0x4C, NOS, 0,
	/* 'M' */ 0x4D, NOS, 0,
	/* 'N' */ 0x4E, NOS, 0,
	/* 'O' */ 0x4F, NOS, 0,
	/* 'P' */ 0x50, NOS, 0,
	/* 'Q' */ 0x51, NOS, 0,
	/* 'R' */ 0x52, NOS, 0,
	/* 'S' */ 0x53, NOS, 0,
	/* 'T' */ 0x54, NOS, 0,
	/* 'U' */ 0x55, NOS, 0,
	/* 'V' */ 0x56, NOS, 0,
	/* 'W' */ 0x57, NOS, 0,
	/* 'X' */ 0x58, NOS, 0,
	/* 'Y' */ 0x59, NOS, 0,
	/* 'Z' */ 0x5A, NOS, 0,
	/* '[' */ 0x5B, NOS, 0,
	/* '\' */ 0x5C, NOS, 0,
	/* ']' */ 0x5D, NOS, 0,
	/* '^' */ 0x5E, NOS, 0,
	/* '_' */ 0x5F, NOS, 0,
	/* '`' */ 0x60, NOS, 0,
	/* 'a' */ 0x61, NOS, 0,
	/* 'b' */ 0x62, NOS, 0,
	/* 'c' */ 0x63, NOS, 0,
	/* 'd' */ 0x64, NOS, 0,
	/* 'e' */ 0x65, NOS, 0,
	/* 'f' */ 0x66, NOS, 0,
	/* 'g' */ 0x67, NOS, 0,
	/* 'h' */ 0x68, NOS, 0,
	/* 'i' */ 0x69, NOS, 0,
	/* 'j' */ 0x6A, NOS, 0,
	/* 'k' */ 0x6B, NOS, 0,
	/* 'l' */ 0x6C, NOS, 0,
	/* 'm' */ 0x6D, NOS, 0,
	/* 'n' */ 0x6E, NOS, 0,
	/* 'o' */ 0x6F, NOS, 0,
	/* 'p' */ 0x70, NOS, 0,
	/* 'q' */ 0x71, NOS, 0,
	/* 'r' */ 0x72, NOS, 0,
	/* 's' */ 0x73, NOS, 0,
	/* 't' */ 0x74, NOS, 0,
	/* 'u' */ 0x75, NOS, 0,
	/* 'v' */ 0x76, NOS, 0,
	/* 'w' */ 0x77, NOS, 0,
	/* 'x' */ 0x78, NOS, 0,
	/* 'y' */ 0x79, NOS, 0,
	/* 'z' */ 0x7A, NOS, 0,
	/* '{' */ 0x7B, NOS, 0,
	/* '|' */ 0x7C, NOS, 0,
	/* '}' */ 0x7D, NOS, 0,
	/* '~' */ 0x7E, NOS, 0,
	/*     */ 0x7F, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	97 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		388 (0x184) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		291 (0x123) bytes
*/



/* TermId=  3 	,  2 , 97 ,106 ,124 */
const CKeyFixups JAP_106_USB_IBM_AT_FixupPtr =
{
	&JAP_106_USB_DEC_VT510_PC_FixupPtr, /* pointer to parent table */
	JAP_106_USB_IBM_AT_JAPA,            /* US must correct for YEN too */
	NoExceptions,                 		/* BELGIUM not required */
	NoExceptions,                 		/* FINNLAND (SWEDEN) not required */
	NoExceptions,                 		/* FRANCE not required */
	NoExceptions,                 		/* GERMANY not required */
	NoExceptions,                 		/* ITALY not required */
	NoExceptions,                 		/* SWITZERLAND (German) not required */
	NoExceptions,                 		/* UK not required */
	NoExceptions,                 		/* DENMARK not required */
	NoExceptions,                 		/* NORWAY not required */
	NoExceptions,                 		/* SPAIN not required */
	NoExceptions,                 		/* DUTCH not required */
	NoExceptions,                 		/* HEBREW not required */
	NoExceptions,                 		/* PORTUGAL not required */
	NoExceptions,                 		/* LATIN AMERICA not required */
	NoExceptions,                 		/* CZECH not required */
	NoExceptions,                 		/* BRAZILIAN not required */
	NULL,                         		/* GREEK not supported */
	NoExceptions,                 		/* FRENCH CANADIAN not required */
	NoExceptions,                 		/* HUNGARY not required */
	NoExceptions,                 		/* POLAND not required */
	NoExceptions,                 		/* SCS not required */
	NoExceptions,                 		/* SLOVAK not required */
	NoExceptions,                 		/* SWEDEN not required */
	NoExceptions,                 		/* TURKEY Q not required */
	NoExceptions,                 		/* ROMANIA not required */
	NoExceptions,                 		/* RUSSIAN not required */
	NoExceptions,                 		/* TURKEY F not required */
	JAP_106_USB_IBM_AT_JAPA,            /* JAPAN ASCII */
};


/* TermId=  5 */
const CKeyFixups JAP_106_USB_DEC_VT510_PC_FixupPtr =
{
	&JAP_106_USB_DEC_VT510_LK411_FixupPtr, /* pointer to parent table */
	JAP_106_USB_DEC_VT510_PC_JAPA,         /* US not required */
	NoExceptions,            					/* BELGIUM */
	NoExceptions,            					/* FINNLAND (SWEDEN) */
	NoExceptions,            					/* FRANCE */
	NoExceptions,            					/* GERMANY */
	NoExceptions,            					/* ITALY */
	NoExceptions,            					/* SWITZERLAND (German) */
	NoExceptions,            					/* UK */
	NoExceptions,            					/* DENMARK */
	NoExceptions,            					/* NORWAY */
	NoExceptions,            					/* SPAIN */
	NoExceptions,            					/* DUTCH */
	NoExceptions,            					/* HEBREW */
	NoExceptions,            					/* PORTUGAL */
	NoExceptions,            					/* LATIN AMERICA */
	NoExceptions,            					/* CZECH */
	NoExceptions,            					/* BRAZILIAN */
	NULL,                         			/* GREEK not supported */
	NoExceptions,            					/* FRENCH CANADIAN */
	NoExceptions,            					/* HUNGARY */
	NoExceptions,            					/* POLAND */
	NoExceptions,            					/* SCS */
	NoExceptions,            					/* SLOVAK */
	NoExceptions,            					/* SWEDEN */
	NoExceptions,            					/* TURKEY Q */
	NoExceptions,            					/* ROMANIA */
	NoExceptions,            					/* RUSSIAN */
	NoExceptions,            					/* TURKEY F */
	JAP_106_USB_DEC_VT510_PC_JAPA,         /* JAPAN ASCII */
};


/* TermId=104 */
const CKeyFixups JAP_106_USB_DEC_VT510_LK411_FixupPtr =
{
	NULL,                         		/* pointer to parent table (none here) */
	JAP_106_USB_DEC_VT510_LK411_JAPA,      /* US not required */
	NoExceptions,            					/* BELGIUM */
	NoExceptions,            					/* FINNLAND (SWEDEN) */
	NoExceptions,            					/* FRANCE */
	NoExceptions,            					/* GERMANY */
	NoExceptions,            					/* ITALY */
	NoExceptions,            					/* SWITZERLAND (German) */
	NoExceptions,            					/* UK */
	NoExceptions,            					/* DENMARK */
	NoExceptions,            					/* NORWAY */
	NoExceptions,            					/* SPAIN */
	NoExceptions,            					/* DUTCH */
	NoExceptions,            					/* HEBREW */
	NoExceptions,            					/* PORTUGAL */
	NoExceptions,            					/* LATIN AMERICA */
	NoExceptions,            					/* CZECH */
	NoExceptions,            					/* BRAZILIAN */
	NULL,                         			/* GREEK not supported */
	NoExceptions,            					/* FRENCH CANADIAN */
	NoExceptions,            					/* HUNGARY */
	NoExceptions,            					/* POLAND */
	NoExceptions,            					/* SCS */
	NoExceptions,            					/* SLOVAK */
	NoExceptions,            					/* SWEDEN */
	NoExceptions,            					/* TURKEY Q */
	NoExceptions,            					/* ROMANIA */
	NoExceptions,            					/* RUSSIAN */
	NoExceptions,            					/* TURKEY F */
	JAP_106_USB_DEC_VT510_LK411_JAPA,      /* JAPAN ASCII */
};

#endif




