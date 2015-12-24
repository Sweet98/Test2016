/* ==============================================================================
   CPP Wedge Engine: International translation tables
   ==============================================================================

   $RCSfile: interfaces_wedge/keyintf.cpp $
   $Revision: 1.6 $
   $Date: 2010/10/29 07:02:36EDT $

   ==============================================================================

   ============================================================================== */


/* ==============================================================================
   Use the Windows ANSI code page (CP1252) to view this file!
	This file is automatically produced by Forkey32 - 3.12.51
	Source Name     : J:\Forkey32\files\MATRIX.KLF
	Destination Name: J:\Forkey32\Debug\keyintf.CPP

	Date: Tue Oct 01 10:39:48 2002

   ============================================================================== */

#define INTERNATIONAL_TABLE_VERSION 100	/* This version gets changed when table structure changes */

/* Format version history: */
/* 100 Initial release */


#include "stdInclude.h"
/* This single include file allows for adding you special requirements */
#include "internat.h"

#ifndef UNICODE_FOR_INTERFACES	                        // manually added

/* Here we test whether the format versions match */
#if (INTERNATIONAL_PRG_VERSION != INTERNATIONAL_TABLE_VERSION)
#error "International tables do have the wrong version"
#endif

// COrderExceptions::WriteDump
//   	   0    1    2    3    4    5    6    7    8    9   10   11
//ID:	   1    3    4    5    6    7    8   25   35   49  104  112
//   	 385, 167, 232, 167, 171, 236, 279, 312, 166, 591, 212,2337,	//000:ID=001
//   	3975,1298, 912,  97,3676,3838,3880,4129,3873,3976, 873,6145,	//001:ID=003
//   	4040, 912,1042, 819,3832,3866,3893,4134,4048,3989,  56,6145,	//002:ID=004
//   	3975,  97, 819,1374,3676,3838,3880,4129,3873,3976, 778,6145,	//003:ID=005
//   	 395,  92, 248,  92, 465, 220, 264, 517, 347, 599, 226,2561,	//004:ID=006
//   	 460, 254, 282, 254, 220, 447,  77, 364, 447, 676, 304,2557,	//005:ID=007
//   	 503, 296, 309, 296, 264,  77, 422, 301, 493, 706, 331,2557,	//006:ID=008
//   	 312, 321, 326, 321, 293, 140,  77, 364, 311, 710, 346,2334,	//007:ID=025
//   	 166,  65, 240,  65, 123, 223, 269, 311, 417, 595, 220,2337,	//008:ID=035
//   	1039, 616, 629, 616, 823, 900, 930,1158,1043, 846, 607,2799,	//009:ID=049
//   	4020, 873,  56, 778,3810,3888,3915,4154,4028,3967,1082,6145,	//010:ID=104
//   	  97,  97,  97,  97,  97,  93,  93,  94,  97, 111,  97,   2,	//011:ID=112

//Chain        ,    ,    ,    ,    ,    ,    ,    ,    ,    ,    ,    ,
//ParIDs       ,    ,    ,    ,    ,    ,    ,    ,    ,    ,    ,    ,
//IDs         1,   3,   4,   5,   6,   7,   8,  25,  35,  49, 104, 112,

/*
 Terminal IDs and it's parents:
1 <- 35 <- 3 <- 5 <- 104
3 <- 5 <- 104
4 <- 104
5 <- 104
6 <- 3 <- 5 <- 104
7 <- 8 <- 6 <- 3 <- 5 <- 104
8 <- 6 <- 3 <- 5 <- 104
25 <- 8 <- 6 <- 3 <- 5 <- 104
35 <- 3 <- 5 <- 104
49 <- 104
104
112
*/
// COrderExceptions::WriteDump
//   	   0    1    2    3    4    5    6    7    8    9   10   11
//ID:	   1    3    4    5    6    7    8   25   35   49  104  112
//   	    ,    ,    ,    ,    ,    ,    ,    , 166,    ,    ,    ,	//000:ID=001
//   	    ,    ,    ,  97,    ,    ,    ,    ,    ,    ,    ,    ,	//001:ID=003
//   	    ,    ,    ,    ,    ,    ,    ,    ,    ,    ,  56,    ,	//002:ID=004
//   	    ,    ,    ,    ,    ,    ,    ,    ,    ,    , 778,    ,	//003:ID=005
//   	    ,  92,    ,    ,    ,    ,    ,    ,    ,    ,    ,    ,	//004:ID=006
//   	    ,    ,    ,    ,    ,    ,  77,    ,    ,    ,    ,    ,	//005:ID=007
//   	    ,    ,    ,    , 264,    ,    ,    ,    ,    ,    ,    ,	//006:ID=008
//   	    ,    ,    ,    ,    ,    ,  77,    ,    ,    ,    ,    ,	//007:ID=025
//   	    ,  65,    ,    ,    ,    ,    ,    ,    ,    ,    ,    ,	//008:ID=035
//   	    ,    ,    ,    ,    ,    ,    ,    ,    ,    , 607,    ,	//009:ID=049
//   	    ,    ,    ,    ,    ,    ,    ,    ,    ,    ,1082,    ,	//010:ID=104
//   	    ,    ,    ,    ,    ,    ,    ,    ,    ,    ,    ,   2,	//011:ID=112

//Chain       8,   3,  10,  10,   1,   6,   4,   6,   1,  10,  10,  11,
//ParIDs     35,   5, 104, 104,   3,   8,   6,   8,   3, 104, 104, 112,
//IDs         1,   3,   4,   5,   6,   7,   8,  25,  35,  49, 104, 112,

/*
*/

#define NOS 0	/* Character is not supported by this leyout */

/* This is used if we need no exceptions for a particular country           */
/* It allows us to use the NULL pointer for marking not supported countries */
const CKeyFixup NoExceptions[1] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* end */ 0,KN_ENDMARKER,0

};

/******************************************************/
/* Exception tables                                   */
/******************************************************/
#define IBM_XT_USA_		NoExceptions	/* not required */

/* TermId=  1 */
const CKeyFixup IBM_XT_BELG[12] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,  45, MO_AltRight,
	/* '`' */ 0x60,  42, MO_AltRight,
	/* '{' */ 0x7B, NOS, 0,
	/* '|' */ 0x7C, NOS, 0,
	/* '}' */ 0x7D, NOS, 0,
	/* '~' */ 0x7E, NOS, 0,
	/* '¨' */ 0xA8,  27, MO_ShiftLeft,
	/* '²' */ 0xB2, NOS, 0,
	/* '³' */ 0xB3, NOS, 0,
	/* '´' */ 0xB4,  41, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	12 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		48 (0x30) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		36 (0x24) bytes
*/

/* TermId=  1 */
const CKeyFixup IBM_XT_FINN[18] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '#' */ 0x23,   4, MO_AltRight,
	/* '$' */ 0x24,   5, MO_ShiftLeft,
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,  45, MO_AltRight,
	/* ']' */ 0x5D,  28, MO_AltRight,
	/* '^' */ 0x5E,   7, MO_AltRight,
	/* '`' */ 0x60,  13, MO_ShiftLeft,
	/* '{' */ 0x7B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x7C,  45, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x7D,  28, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x7E,  42, MO_ShiftLeft|MO_AltRight,
	/* '£' */ 0xA3,   4, MO_ShiftLeft,
	/* '¤' */ 0xA4, NOS, 0,
	/* '§' */ 0xA7, NOS, 0,
	/* '¨' */ 0xA8,  28, 0,
	/* '´' */ 0xB4,  13, 0,
	/* '½' */ 0xBD, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	18 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		72 (0x48) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		54 (0x36) bytes
*/

/* TermId=  1 */
const CKeyFixup IBM_XT_FRAN[24] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,   9, 0,
	/* '*' */ 0x2A,  28, MO_ShiftLeft,
	/* '+' */ 0x2B,  55, MO_ShiftLeft,
	/* '-' */ 0x2D,  13, 0,
	/* '=' */ 0x3D,  55, 0,
	/* '@' */ 0x40,   3, MO_AltRight,
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,  45, MO_AltRight,
	/* ']' */ 0x5D,  28, MO_AltRight,
	/* '^' */ 0x5E,   7, MO_AltRight,
	/* '_' */ 0x5F,  13, MO_ShiftLeft,
	/* '`' */ 0x60,  42, MO_AltRight,
	/* '{' */ 0x7B, NOS, 0,
	/* '|' */ 0x7C, NOS, 0,
	/* '}' */ 0x7D, NOS, 0,
	/* '~' */ 0x7E, NOS, 0,
	/* '£' */ 0xA3,  42, MO_ShiftLeft,
	/* '¤' */ 0xA4, NOS, 0,
	/* '§' */ 0xA7,   7, 0,
	/* '¨' */ 0xA8,  27, MO_ShiftLeft,
	/* '²' */ 0xB2, NOS, 0,
	/* '´' */ 0xB4,  41, MO_AltRight,
	/* 'µ' */ 0xB5,  42, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	24 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		96 (0x60) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		72 (0x48) bytes
*/

/* TermId=  1 */
const CKeyFixup IBM_XT_GERM[16] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* ''' */ 0x27,  13, 0,
	/* '@' */ 0x40,   3, MO_AltRight,
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,  45, MO_AltRight,
	/* ']' */ 0x5D,  28, MO_AltRight,
	/* '^' */ 0x5E,  42, MO_ShiftLeft,
	/* '`' */ 0x60,  13, MO_ShiftLeft,
	/* '{' */ 0x7B, NOS, 0,
	/* '|' */ 0x7C, NOS, 0,
	/* '}' */ 0x7D, NOS, 0,
	/* '~' */ 0x7E, NOS, 0,
	/* '°' */ 0xB0, NOS, 0,
	/* '²' */ 0xB2, NOS, 0,
	/* '³' */ 0xB3, NOS, 0,
	/* '´' */ 0xB4, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	16 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		64 (0x40) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		48 (0x30) bytes
*/

/* TermId=  1 */
const CKeyFixup IBM_XT_ITAL[11] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '#' */ 0x23,  41, MO_ShiftLeft,
	/* '@' */ 0x40, NOS, 0,
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,  45, MO_AltRight,
	/* '`' */ 0x60, NOS, 0,
	/* '{' */ 0x7B, NOS, 0,
	/* '|' */ 0x7C, NOS, 0,
	/* '}' */ 0x7D, NOS, 0,
	/* '~' */ 0x7E, NOS, 0,
	/* '°' */ 0xB0, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	11 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		44 (0x2C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		33 (0x21) bytes
*/

/* TermId=  1 */
const CKeyFixup IBM_XT_SWIT[14] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '[' */ 0x5B, NOS, 0,
	/* '\' */ 0x5C,  45, MO_AltRight,
	/* ']' */ 0x5D, NOS, 0,
	/* '^' */ 0x5E,  13, 0,
	/* '`' */ 0x60,  13, MO_ShiftLeft,
	/* '{' */ 0x7B, NOS, 0,
	/* '|' */ 0x7C,   8, MO_AltRight,
	/* '}' */ 0x7D, NOS, 0,
	/* '~' */ 0x7E,  13, MO_AltRight,
	/* '§' */ 0xA7,   6, MO_AltRight,
	/* '¨' */ 0xA8,  28, 0,
	/* '°' */ 0xB0,   5, MO_AltRight,
	/* '´' */ 0xB4,  12, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	14 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		56 (0x38) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		42 (0x2A) bytes
*/

/* TermId=  1 */
const CKeyFixup IBM_XT_UKIN[8] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '[' */ 0x5B,  27, 0,
	/* '\' */ 0x5C,  45, 0,
	/* '`' */ 0x60, NOS, 0,
	/* '{' */ 0x7B,  27, MO_ShiftLeft,
	/* '|' */ 0x7C,  45, MO_ShiftLeft,
	/* '}' */ 0x7D,  28, MO_ShiftLeft,
	/* '~' */ 0x7E,  42, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	8 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		32 (0x20) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		24 (0x18) bytes
*/

/* TermId=  1 */
const CKeyFixup IBM_XT_DENM[15] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,  45, MO_AltRight,
	/* ']' */ 0x5D,  28, MO_AltRight,
	/* '^' */ 0x5E,   7, MO_AltRight,
	/* '`' */ 0x60,  13, MO_ShiftLeft,
	/* '{' */ 0x7B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x7C,  45, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x7D,  28, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x7E,  42, MO_ShiftLeft|MO_AltRight,
	/* '£' */ 0xA3, NOS, 0,
	/* '§' */ 0xA7, NOS, 0,
	/* '¨' */ 0xA8,  28, 0,
	/* '´' */ 0xB4,  13, 0,
	/* '½' */ 0xBD, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	15 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		60 (0x3C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		45 (0x2D) bytes
*/

/* TermId=  1 */
const CKeyFixup IBM_XT_NORW[17] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '#' */ 0x23,   4, MO_AltRight,
	/* '$' */ 0x24,   5, MO_ShiftLeft,
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,  45, MO_AltRight,
	/* ']' */ 0x5D,  28, MO_AltRight,
	/* '^' */ 0x5E,   7, MO_AltRight,
	/* '`' */ 0x60,  13, 0,
	/* '{' */ 0x7B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x7C,  45, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x7D,  28, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x7E,  42, MO_ShiftLeft|MO_AltRight,
	/* '£' */ 0xA3,   4, MO_ShiftLeft,
	/* '¤' */ 0xA4, NOS, 0,
	/* '§' */ 0xA7, NOS, 0,
	/* '¨' */ 0xA8,  28, 0,
	/* '´' */ 0xB4,  13, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	17 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		68 (0x44) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		51 (0x33) bytes
*/

/* TermId=  1 */
const CKeyFixup IBM_XT_SPAI[34] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,  54, MO_ShiftLeft,
	/* '"' */ 0x22,  55, MO_ShiftLeft,
	/* '#' */ 0x23,   4, MO_ShiftLeft,
	/* '&' */ 0x26,   8, MO_ShiftLeft,
	/* ''' */ 0x27,  55, 0,
	/* '(' */ 0x28,  10, MO_ShiftLeft,
	/* ')' */ 0x29,  11, MO_ShiftLeft,
	/* '*' */ 0x2A,   9, MO_ShiftLeft,
	/* '+' */ 0x2B,  13, MO_ShiftLeft,
	/* '-' */ 0x2D,  12, 0,
	/* '/' */ 0x2F,   7, MO_ShiftLeft,
	/* ':' */ 0x3A,  41, MO_ShiftLeft,
	/* ';' */ 0x3B,  41, 0,
	/* '=' */ 0x3D,  13, 0,
	/* '?' */ 0x3F,  53, MO_ShiftLeft,
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,  45, MO_AltRight,
	/* '^' */ 0x5E,  28, MO_ShiftLeft,
	/* '_' */ 0x5F,  12, MO_ShiftLeft,
	/* '`' */ 0x60,  28, 0,
	/* '{' */ 0x7B, NOS, 0,
	/* '|' */ 0x7C, NOS, 0,
	/* '}' */ 0x7D, NOS, 0,
	/* '~' */ 0x7E, NOS, 0,
	/* '¡' */ 0xA1,   2, MO_ShiftLeft,
	/* '¨' */ 0xA8,  27, 0,
	/* 'ª' */ 0xAA, NOS, 0,
	/* '¬' */ 0xAC, NOS, 0,
	/* '°' */ 0xB0, NOS, 0,
	/* '´' */ 0xB4,  27, MO_ShiftLeft,
	/* '·' */ 0xB7, NOS, 0,
	/* '¿' */ 0xBF,   3, MO_ShiftLeft,
	/* 'Ç' */ 0xC7, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	34 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		136 (0x88) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		102 (0x66) bytes
*/
/* IBM_XT_DUTC is not supported */
/* IBM_XT_HEBR is not supported */
/* IBM_XT_PORT is not supported */
/* IBM_XT_LATI is not supported */
/* IBM_XT_CZEC is not supported */
/* IBM_XT_BRAZ is not supported */
/* IBM_XT_GREK is not supported */
/* IBM_XT_FCAN is not supported */
/* IBM_XT_HUNG is not supported */
/* IBM_XT_POLI is not supported */
/* IBM_XT_SCS_ is not supported */
/* IBM_XT_SLOV is not supported */
/* IBM_XT_SWED is not supported */
/* IBM_XT_TURQ is not supported */
/* IBM_XT_ROMA is not supported */
/* IBM_XT_RUSS is not supported */
/* IBM_XT_TURF is not supported */
/* IBM_XT_JAPA is not supported */
#define IBM_AT_USA_		NoExceptions	/* not required */
#define IBM_AT_BELG		NoExceptions	/* not required */
#define IBM_AT_FINN		NoExceptions	/* not required */
#define IBM_AT_FRAN		NoExceptions	/* not required */
#define IBM_AT_GERM		NoExceptions	/* not required */
#define IBM_AT_ITAL		NoExceptions	/* not required */
#define IBM_AT_SWIT		NoExceptions	/* not required */
#define IBM_AT_UKIN		NoExceptions	/* not required */
#define IBM_AT_DENM		NoExceptions	/* not required */
#define IBM_AT_NORW		NoExceptions	/* not required */
#define IBM_AT_SPAI		NoExceptions	/* not required */
#define IBM_AT_DUTC		NoExceptions	/* not required */
#define IBM_AT_HEBR		NoExceptions	/* not required */
#define IBM_AT_PORT		NoExceptions	/* not required */
#define IBM_AT_LATI		NoExceptions	/* not required */
#define IBM_AT_CZEC		NoExceptions	/* not required */
#define IBM_AT_BRAZ		NoExceptions	/* not required */
/* IBM_AT_GREK is not supported */
#define IBM_AT_FCAN		NoExceptions	/* not required */
#define IBM_AT_HUNG		NoExceptions	/* not required */
#define IBM_AT_POLI		NoExceptions	/* not required */
#define IBM_AT_SCS_		NoExceptions	/* not required */
#define IBM_AT_SLOV		NoExceptions	/* not required */
#define IBM_AT_SWED		NoExceptions	/* not required */
#define IBM_AT_TURQ		NoExceptions	/* not required */
#define IBM_AT_ROMA		NoExceptions	/* not required */
#define IBM_AT_RUSS		NoExceptions	/* not required */
#define IBM_AT_TURF		NoExceptions	/* not required */

/* TermId=  3 	,  2 , 97 ,106 ,124 */
const CKeyFixup IBM_AT_JAPA[98] =
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
	/* '\' */ 0x5C,  56, 0,        //ewr04810,29
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
	/* '¥' */ 0xA5,  14, 0,
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
#define DEC_VT420_USA_		NoExceptions	/* not required */

/* TermId=  4 */
const CKeyFixup DEC_VT420_BELG[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_FINN[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_FRAN[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_GERM[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_ITAL[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_SWIT[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_UKIN[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_DENM[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_NORW[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_SPAI[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_DUTC[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_HEBR[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_PORT[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_LATI[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_CZEC[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  53, MO_AltRight,
	/* '>' */ 0x3E,  54, MO_AltRight,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_BRAZ[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  53, MO_ShiftLeft,
	/* '>' */ 0x3E,  54, MO_ShiftLeft,
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
/* DEC_VT420_GREK is not supported */

/* TermId=  4 */
const CKeyFixup DEC_VT420_FCAN[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_HUNG[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, MO_AltRight,
	/* '>' */ 0x3E,  45, MO_ShiftLeft|MO_AltRight,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_POLI[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_SCS_[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_SLOV[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  53, MO_AltRight,
	/* '>' */ 0x3E,  54, MO_AltRight,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_SWED[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_TURQ[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_ROMA[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  54, MO_ShiftLeft,
	/* '>' */ 0x3E,  53, MO_ShiftLeft,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_RUSS[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_TURF[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
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

/* TermId=  4 */
const CKeyFixup DEC_VT420_JAPA[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C, NOS, 0,
	/* '>' */ 0x3E, NOS, 0,
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
#define DEC_VT510_PC_USA_		NoExceptions	/* not required */

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_BELG[17] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '#' */ 0x23,   4, MO_AltRight,
	/* '@' */ 0x40,   3, MO_AltRight,
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,  45, MO_AltRight,
	/* ']' */ 0x5D,  28, MO_AltRight,
	/* '^' */ 0x5E,   7, MO_AltRight,
	/* '`' */ 0x60,  42, MO_AltRight|MO_DeadKey,
	/* '|' */ 0x7C,   2, MO_AltRight,
	/* '~' */ 0x7E,  55, MO_AltRight|MO_DeadKey,
	/* '€' */ 0x80,  19, MO_AltRight,
	/* '£' */ 0xA3,  42, MO_ShiftLeft,
	/* '¨' */ 0xA8,  27, MO_ShiftLeft|MO_DeadKey,
	/* '²' */ 0xB2,   1, 0,
	/* '³' */ 0xB3,   1, MO_ShiftLeft,
	/* '´' */ 0xB4,  41, MO_AltRight|MO_DeadKey,
	/* 'µ' */ 0xB5,  42, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	17 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		68 (0x44) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		51 (0x33) bytes
*/

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_FINN[22] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '#' */ 0x23,   4, MO_ShiftLeft,
	/* '$' */ 0x24,   5, MO_AltRight,
	/* '@' */ 0x40,   3, MO_AltRight,
	/* '[' */ 0x5B,   9, MO_AltRight,
	/* '\' */ 0x5C,  12, MO_AltRight,
	/* ']' */ 0x5D,  10, MO_AltRight,
	/* '^' */ 0x5E,  28, MO_ShiftLeft|MO_DeadKey,
	/* '`' */ 0x60,  13, MO_ShiftLeft|MO_DeadKey,
	/* '{' */ 0x7B,   8, MO_AltRight,
	/* '|' */ 0x7C,  45, MO_AltRight,
	/* '}' */ 0x7D,  11, MO_AltRight,
	/* '~' */ 0x7E,  28, MO_AltRight|MO_DeadKey,
	/* '€' */ 0x80,  19, MO_AltRight,
	/* '£' */ 0xA3,   4, MO_AltRight,
	/* '¤' */ 0xA4,   5, MO_ShiftLeft,
	/* '§' */ 0xA7,   1, 0,
	/* '¨' */ 0xA8,  28, MO_DeadKey,
	/* '´' */ 0xB4,  13, MO_DeadKey,
	/* '½' */ 0xBD,   1, MO_ShiftLeft,
	/* 'Ü' */ 0xDC, NOS, 0,
	/* 'ü' */ 0xFC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	22 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		88 (0x58) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		66 (0x42) bytes
*/

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_FRAN[25] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,  55, 0,
	/* '#' */ 0x23,   4, MO_AltRight,
	/* '*' */ 0x2A,  42, 0,
	/* '+' */ 0x2B,  13, MO_ShiftLeft,
	/* '-' */ 0x2D,   7, 0,
	/* '=' */ 0x3D,  13, 0,
	/* '@' */ 0x40,  11, MO_AltRight,
	/* '[' */ 0x5B,   6, MO_AltRight,
	/* '\' */ 0x5C,   9, MO_AltRight,
	/* ']' */ 0x5D,  12, MO_AltRight,
	/* '^' */ 0x5E,  10, MO_AltRight,
	/* '_' */ 0x5F,   9, 0,
	/* '`' */ 0x60,   8, MO_AltRight|MO_DeadKey,
	/* '{' */ 0x7B,   5, MO_AltRight,
	/* '|' */ 0x7C,   7, MO_AltRight,
	/* '}' */ 0x7D,  13, MO_AltRight,
	/* '~' */ 0x7E,   3, MO_AltRight|MO_DeadKey,
	/* '€' */ 0x80,  19, MO_AltRight,
	/* '£' */ 0xA3,  28, MO_ShiftLeft,
	/* '¤' */ 0xA4,  28, MO_AltRight,
	/* '§' */ 0xA7,  55, MO_ShiftLeft,
	/* '¨' */ 0xA8,  27, MO_ShiftLeft|MO_DeadKey,
	/* '²' */ 0xB2,   1, 0,
	/* 'µ' */ 0xB5,  42, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	25 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		100 (0x64) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		75 (0x4B) bytes
*/

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_GERM[18] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '\' */ 0x5C,  12, MO_AltRight,
	/* '^' */ 0x5E,   1, MO_DeadKey,
	/* '`' */ 0x60,  13, MO_ShiftLeft|MO_DeadKey,
	/* '|' */ 0x7C,  45, MO_AltRight,
	/* '€' */ 0x80,  19, MO_AltRight,
	/* '¡' */ 0xA1, NOS, 0,
	/* '£' */ 0xA3, NOS, 0,
	/* '¤' */ 0xA4, NOS, 0,
	/* '¬' */ 0xAC, NOS, 0,
	/* '±' */ 0xB1, NOS, 0,
	/* '´' */ 0xB4,  13, MO_DeadKey,
	/* '¸' */ 0xB8, NOS, 0,
	/* '¹' */ 0xB9, NOS, 0,
	/* '¼' */ 0xBC, NOS, 0,
	/* '½' */ 0xBD, NOS, 0,
	/* '¾' */ 0xBE, NOS, 0,
	/* '¿' */ 0xBF, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	18 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		72 (0x48) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		54 (0x36) bytes
*/

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_ITAL[57] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,   2, MO_ShiftLeft,
	/* '"' */ 0x22,   3, MO_ShiftLeft,
	/* '#' */ 0x23,  41, MO_AltRight,
	/* '$' */ 0x24,   5, MO_ShiftLeft,
	/* '%' */ 0x25,   6, MO_ShiftLeft,
	/* '&' */ 0x26,   7, MO_ShiftLeft,
	/* ''' */ 0x27,  12, 0,
	/* '(' */ 0x28,   9, MO_ShiftLeft,
	/* ')' */ 0x29,  10, MO_ShiftLeft,
	/* '*' */ 0x2A,  28, MO_ShiftLeft,
	/* '+' */ 0x2B,  28, 0,
	/* ',' */ 0x2C,  53, 0,
	/* '-' */ 0x2D,  55, 0,
	/* '.' */ 0x2E,  54, 0,
	/* '/' */ 0x2F,   8, MO_ShiftLeft,
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
	/* ':' */ 0x3A,  54, MO_ShiftLeft,
	/* ';' */ 0x3B,  53, MO_ShiftLeft,
	/* '=' */ 0x3D,  11, MO_ShiftLeft,
	/* '?' */ 0x3F,  12, MO_ShiftLeft,
	/* '@' */ 0x40,  40, MO_AltRight,
	/* 'M' */ 0x4D,  52, MO_ShiftLeft,
	/* 'W' */ 0x57,  18, MO_ShiftLeft,
	/* 'Z' */ 0x5A,  46, MO_ShiftLeft,
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,   1, 0,
	/* ']' */ 0x5D,  28, MO_AltRight,
	/* '^' */ 0x5E,  13, MO_ShiftLeft,
	/* '_' */ 0x5F,  55, MO_ShiftLeft,
	/* '`' */ 0x60, NOS, 0,
	/* 'm' */ 0x6D,  52, 0,
	/* 'w' */ 0x77,  18, 0,
	/* 'z' */ 0x7A,  46, 0,
	/* '{' */ 0x7B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x7C,   1, MO_ShiftLeft,
	/* '}' */ 0x7D,  28, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x7E, NOS, 0,
	/* '€' */ 0x80,  19, MO_AltRight,
	/* '£' */ 0xA3,   4, MO_ShiftLeft,
	/* '°' */ 0xB0,  41, MO_ShiftLeft,
	/* 'à' */ 0xE0,  41, 0,
	/* 'ç' */ 0xE7,  40, MO_ShiftLeft,
	/* 'è' */ 0xE8,  27, 0,
	/* 'é' */ 0xE9,  27, MO_ShiftLeft,
	/* 'ì' */ 0xEC,  13, 0,
	/* 'ò' */ 0xF2,  40, 0,
	/* 'ù' */ 0xF9,  42, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	57 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		228 (0xE4) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		171 (0xAB) bytes
*/

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_SWIT[17] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,  28, MO_ShiftLeft,
	/* '#' */ 0x23,   4, MO_AltRight,
	/* '@' */ 0x40,   3, MO_AltRight,
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,  45, MO_AltRight,
	/* ']' */ 0x5D,  28, MO_AltRight,
	/* '^' */ 0x5E,  13, MO_DeadKey,
	/* '`' */ 0x60,  13, MO_ShiftLeft|MO_DeadKey,
	/* '{' */ 0x7B,  41, MO_AltRight,
	/* '|' */ 0x7C,   2, MO_AltRight,
	/* '}' */ 0x7D,  42, MO_AltRight,
	/* '~' */ 0x7E,  13, MO_AltRight|MO_DeadKey,
	/* '€' */ 0x80,  19, MO_AltRight,
	/* '§' */ 0xA7,   1, 0,
	/* '¨' */ 0xA8,  28, MO_DeadKey,
	/* '´' */ 0xB4,  12, MO_AltRight|MO_DeadKey,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	17 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		68 (0x44) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		51 (0x33) bytes
*/

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_UKIN[11] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '"' */ 0x22,   3, MO_ShiftLeft,
	/* '#' */ 0x23,  42, 0,
	/* '<' */ 0x3C,  53, MO_ShiftLeft,
	/* '>' */ 0x3E,  54, MO_ShiftLeft,
	/* '@' */ 0x40,  41, MO_ShiftLeft,
	/* '\' */ 0x5C,  45, 0,
	/* '|' */ 0x7C,  45, MO_ShiftLeft,
	/* '~' */ 0x7E,  42, MO_ShiftLeft,
	/* '€' */ 0x80,  19, MO_AltRight,
	/* '£' */ 0xA3,   4, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	11 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		44 (0x2C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		33 (0x21) bytes
*/

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_DENM[20] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '#' */ 0x23,   4, MO_ShiftLeft,
	/* '$' */ 0x24,   5, MO_AltRight,
	/* '@' */ 0x40,   3, MO_AltRight,
	/* '[' */ 0x5B,   9, MO_AltRight,
	/* '\' */ 0x5C,  45, MO_AltRight,
	/* ']' */ 0x5D,  10, MO_AltRight,
	/* '^' */ 0x5E,  28, MO_ShiftLeft|MO_DeadKey,
	/* '`' */ 0x60,  13, MO_ShiftLeft|MO_DeadKey,
	/* '{' */ 0x7B,   8, MO_AltRight,
	/* '|' */ 0x7C,  13, MO_AltRight,
	/* '}' */ 0x7D,  11, MO_AltRight,
	/* '~' */ 0x7E,  28, MO_AltRight|MO_DeadKey,
	/* '€' */ 0x80,  19, MO_AltRight,
	/* '£' */ 0xA3,   4, MO_AltRight,
	/* '¤' */ 0xA4,   5, MO_ShiftLeft,
	/* '§' */ 0xA7,   1, 0,
	/* '¨' */ 0xA8,  28, MO_DeadKey,
	/* '´' */ 0xB4,  13, MO_DeadKey,
	/* '½' */ 0xBD,   1, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	20 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		80 (0x50) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		60 (0x3C) bytes
*/

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_NORW[18] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '$' */ 0x24,   5, MO_AltRight,
	/* '@' */ 0x40,   3, MO_AltRight,
	/* '[' */ 0x5B,   9, MO_AltRight,
	/* '\' */ 0x5C,  13, 0,
	/* ']' */ 0x5D,  10, MO_AltRight,
	/* '^' */ 0x5E,  28, MO_ShiftLeft|MO_DeadKey,
	/* '`' */ 0x60,  13, MO_ShiftLeft|MO_DeadKey,
	/* '{' */ 0x7B,   8, MO_AltRight,
	/* '|' */ 0x7C,   1, 0,
	/* '}' */ 0x7D,  11, MO_AltRight,
	/* '~' */ 0x7E,  28, MO_AltRight|MO_DeadKey,
	/* '€' */ 0x80,  19, MO_AltRight,
	/* '£' */ 0xA3,   4, MO_AltRight,
	/* '¤' */ 0xA4,   5, MO_ShiftLeft,
	/* '§' */ 0xA7,   1, MO_ShiftLeft,
	/* '¨' */ 0xA8,  28, MO_DeadKey,
	/* '´' */ 0xB4,  13, MO_AltRight|MO_DeadKey,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	18 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		72 (0x48) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		54 (0x36) bytes
*/

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_SPAI[22] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '#' */ 0x23,   4, MO_AltRight,
	/* '@' */ 0x40,   3, MO_AltRight,
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,   1, MO_AltRight,
	/* ']' */ 0x5D,  28, MO_AltRight,
	/* '^' */ 0x5E,  27, MO_ShiftLeft|MO_DeadKey,
	/* '`' */ 0x60,  27, MO_DeadKey,
	/* '{' */ 0x7B,  41, MO_AltRight,
	/* '|' */ 0x7C,   2, MO_AltRight,
	/* '}' */ 0x7D,  42, MO_AltRight,
	/* '~' */ 0x7E,   5, MO_AltRight|MO_DeadKey,
	/* '€' */ 0x80,  19, MO_AltRight,
	/* '¡' */ 0xA1,  13, 0,
	/* '¨' */ 0xA8,  41, MO_ShiftLeft|MO_DeadKey,
	/* 'ª' */ 0xAA,   1, MO_ShiftLeft,
	/* '¬' */ 0xAC,   7, MO_AltRight,
	/* '°' */ 0xB0,   1, 0,
	/* '´' */ 0xB4,  41, MO_DeadKey,
	/* '·' */ 0xB7,   4, MO_ShiftLeft,
	/* '¿' */ 0xBF,  13, MO_ShiftLeft,
	/* 'Ç' */ 0xC7,  42, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	22 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		88 (0x58) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		66 (0x42) bytes
*/

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_DUTC[46] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '"' */ 0x22,   3, MO_ShiftLeft,
	/* '&' */ 0x26,   7, MO_ShiftLeft,
	/* ''' */ 0x27,  11, MO_ShiftLeft,
	/* '(' */ 0x28,   9, MO_ShiftLeft,
	/* ')' */ 0x29,  10, MO_ShiftLeft,
	/* '*' */ 0x2A,  28, 0,
	/* '+' */ 0x2B,  40, 0,
	/* '-' */ 0x2D, NOS, 0,
	/* '/' */ 0x2F,  12, 0,
	/* ':' */ 0x3A,  54, MO_ShiftLeft,
	/* ';' */ 0x3B,  53, MO_ShiftLeft,
	/* '<' */ 0x3C,  42, 0,
	/* '=' */ 0x3D,  55, MO_ShiftLeft,
	/* '>' */ 0x3E,  42, MO_ShiftLeft,
	/* '?' */ 0x3F,  12, MO_ShiftLeft,
	/* '@' */ 0x40,   1, 0,
	/* '[' */ 0x5B,  45, MO_ShiftLeft,
	/* '\' */ 0x5C,  12, MO_AltRight,
	/* ']' */ 0x5D,  45, 0,
	/* '^' */ 0x5E,  27, MO_ShiftLeft,
	/* '_' */ 0x5F,   8, MO_ShiftLeft,
	/* '`' */ 0x60,  41, MO_ShiftLeft,
	/* '{' */ 0x7B,   9, MO_AltRight,
	/* '|' */ 0x7C,  28, MO_ShiftLeft,
	/* '}' */ 0x7D,  10, MO_AltRight,
	/* '~' */ 0x7E,  13, MO_ShiftLeft,
	/* '•' */ 0x95,  13, 0,
	/* '¢' */ 0xA2,  48, MO_AltRight,
	/* '£' */ 0xA3,   8, MO_AltRight,
	/* '§' */ 0xA7,   1, MO_ShiftLeft,
	/* '¨' */ 0xA8,  27, 0,
	/* '«' */ 0xAB,  46, MO_AltRight,
	/* '¬' */ 0xAC,   1, MO_AltRight,
	/* '±' */ 0xB1,  40, MO_ShiftLeft,
	/* '²' */ 0xB2,   3, MO_AltRight,
	/* '³' */ 0xB3,   4, MO_AltRight,
	/* '´' */ 0xB4,  41, 0,
	/* 'µ' */ 0xB5,  52, MO_AltRight,
	/* '¶' */ 0xB6,  20, MO_AltRight,
	/* '¹' */ 0xB9,   2, MO_AltRight,
	/* '»' */ 0xBB,  47, MO_AltRight,
	/* '¼' */ 0xBC,   5, MO_AltRight,
	/* '½' */ 0xBD,   6, MO_AltRight,
	/* '¾' */ 0xBE,   7, MO_AltRight,
	/* 'ß' */ 0xDF,  32, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	46 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		184 (0xB8) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		138 (0x8A) bytes
*/

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_HEBR[5] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  53, MO_ShiftLeft,
	/* '>' */ 0x3E,  54, MO_ShiftLeft,
	/* '\' */ 0x5C,  29, 0,
	/* '|' */ 0x7C,  29, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	5 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		20 (0x14) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		15 (0xF) bytes
*/

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_PORT[31] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '"' */ 0x22,   3, MO_ShiftLeft,
	/* '&' */ 0x26,   7, MO_ShiftLeft,
	/* ''' */ 0x27,  12, 0,
	/* '(' */ 0x28,   9, MO_ShiftLeft,
	/* ')' */ 0x29,  10, MO_ShiftLeft,
	/* '*' */ 0x2A,  27, MO_ShiftLeft,
	/* '+' */ 0x2B,  27, 0,
	/* '-' */ 0x2D,  55, 0,
	/* '/' */ 0x2F,   8, MO_ShiftLeft,
	/* '=' */ 0x3D,  11, MO_ShiftLeft,
	/* '?' */ 0x3F,  12, MO_ShiftLeft,
	/* '@' */ 0x40,   3, MO_AltRight,
	/* '[' */ 0x5B,   9, MO_AltRight,
	/* '\' */ 0x5C,   1, 0,
	/* ']' */ 0x5D,  10, MO_AltRight,
	/* '^' */ 0x5E,  42, MO_ShiftLeft,
	/* '_' */ 0x5F,  55, MO_ShiftLeft,
	/* '`' */ 0x60,  28, MO_ShiftLeft,
	/* '{' */ 0x7B,   8, MO_AltRight,
	/* '|' */ 0x7C,   1, MO_ShiftLeft,
	/* '}' */ 0x7D,  11, MO_AltRight,
	/* '~' */ 0x7E,  42, 0,
	/* '£' */ 0xA3,   4, MO_AltRight,
	/* '§' */ 0xA7,   5, MO_AltRight,
	/* '¨' */ 0xA8,  27, MO_AltRight,
	/* 'ª' */ 0xAA,  41, MO_ShiftLeft,
	/* '«' */ 0xAB,  13, 0,
	/* '°' */ 0xB0,  41, 0,
	/* '´' */ 0xB4,  28, 0,
	/* '»' */ 0xBB,  13, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	31 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		124 (0x7C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		93 (0x5D) bytes
*/

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_LATI[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '\' */ 0x5C,  12, MO_AltRight,
	/* '|' */ 0x7C,   1, 0,
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

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_CZEC[20] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,   2, MO_AltRight,
	/* ''' */ 0x27,   1, MO_AltRight,
	/* '(' */ 0x28,  10, MO_AltRight,
	/* ')' */ 0x29,  11, MO_AltRight,
	/* ',' */ 0x2C,  53, MO_AltRight,
	/* '-' */ 0x2D,  12, MO_AltRight,
	/* '.' */ 0x2E,  54, MO_AltRight,
	/* ':' */ 0x3A,  40, MO_ShiftLeft|MO_AltRight,
	/* '<' */ 0x3C,  53, MO_ShiftLeft|MO_AltRight,
	/* '>' */ 0x3E,  54, MO_ShiftLeft|MO_AltRight,
	/* '\' */ 0x5C,  42, MO_AltRight,
	/* '_' */ 0x5F,  12, MO_ShiftLeft|MO_AltRight,
	/* '{' */ 0x7B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x7C,  42, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x7D,  28, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x7E,   1, MO_ShiftLeft|MO_AltRight,
	/* '°' */ 0xB0, NOS, 0,
	/* ??? */ 0xDF,   1, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xE4,   1, 0,	/* Not CP1252 */
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	20 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		80 (0x50) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		60 (0x3C) bytes
*/

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_BRAZ[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '\' */ 0x5C,  45, 0,
	/* '|' */ 0x7C,  45, MO_ShiftLeft,
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
/* DEC_VT510_PC_GREK is not supported */

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_FCAN[36] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '#' */ 0x23,   1, 0,
	/* ''' */ 0x27,  28, 0,
	/* '<' */ 0x3C,  42, 0,
	/* '>' */ 0x3E,  42, MO_ShiftLeft,
	/* '@' */ 0x40,   3, MO_AltRight,
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,   1, MO_AltRight,
	/* ']' */ 0x5D,  28, MO_AltRight,
	/* '^' */ 0x5E,  27, 0,
	/* '`' */ 0x60,  41, MO_ShiftLeft,
	/* '{' */ 0x7B,  41, MO_AltRight,
	/* '|' */ 0x7C,   1, MO_ShiftLeft,
	/* '}' */ 0x7D,  42, MO_AltRight,
	/* '~' */ 0x7E,  40, MO_AltRight,
	/* '•' */ 0x95,  45, MO_AltRight,
	/* '¢' */ 0xA2,   5, MO_AltRight,
	/* '£' */ 0xA3,   4, MO_AltRight,
	/* '¤' */ 0xA4,   6, MO_AltRight,
	/* '§' */ 0xA7,  25, MO_AltRight,
	/* '¨' */ 0xA8,  28, MO_ShiftLeft,
	/* 'ª' */ 0xAA,   9, MO_AltRight,
	/* '«' */ 0xAB,  45, 0,
	/* '¬' */ 0xAC,   7, MO_AltRight,
	/* '°' */ 0xB0, NOS, 0,
	/* '±' */ 0xB1,   2, MO_AltRight,
	/* '³' */ 0xB3,  10, MO_AltRight,
	/* '´' */ 0xB4,  55, MO_AltRight,
	/* 'µ' */ 0xB5,  52, MO_AltRight,
	/* '¶' */ 0xB6,  26, MO_AltRight,
	/* '»' */ 0xBB,  45, MO_ShiftLeft,
	/* '¼' */ 0xBC,  11, MO_AltRight,
	/* '½' */ 0xBD,  12, MO_AltRight,
	/* '¾' */ 0xBE,  13, MO_AltRight,
	/* 'Ç' */ 0xC7, NOS, 0,
	/* 'ç' */ 0xE7, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	36 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		144 (0x90) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		108 (0x6C) bytes
*/

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_HUNG[51] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,   5, MO_ShiftLeft,
	/* '#' */ 0x23,  47, MO_AltRight,
	/* '$' */ 0x24,  40, MO_AltRight,
	/* '&' */ 0x26,  48, MO_AltRight,
	/* '(' */ 0x28,   9, MO_ShiftLeft,
	/* ')' */ 0x29,  10, MO_ShiftLeft,
	/* '*' */ 0x2A,  55, MO_AltRight,
	/* '-' */ 0x2D,  55, 0,
	/* '0' */ 0x30,   1, 0,
	/* ':' */ 0x3A,  54, MO_ShiftLeft,
	/* ';' */ 0x3B,  53, MO_AltRight,
	/* '>' */ 0x3E,  46, MO_AltRight,
	/* '@' */ 0x40,  49, MO_AltRight,
	/* '[' */ 0x5B,  34, MO_AltRight,
	/* '\' */ 0x5C,  17, MO_AltRight,
	/* ']' */ 0x5D,  35, MO_AltRight,
	/* '^' */ 0x5E,   4, MO_AltRight,
	/* '_' */ 0x5F,  55, MO_ShiftLeft,
	/* '`' */ 0x60,   8, MO_AltRight,
	/* '{' */ 0x7B,  50, MO_AltRight,
	/* '|' */ 0x7C,  18, MO_AltRight,
	/* '}' */ 0x7D,  51, MO_AltRight,
	/* '~' */ 0x7E,   2, MO_AltRight,
	/* ??? */ 0x82,  13, MO_AltRight,	/* Not CP1252 */
	/* '‘' */ 0x91, NOS, 0,
	/* ??? */ 0xA1,   3, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xA2,   5, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xA3,  39, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xA4,  42, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xA7,   1, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xA8,  12, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xB0,   6, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xB3,  38, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xB4,  10, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xB7,   9, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xB8,   7, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xBD,  11, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xD0,  33, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xD3,  13, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xD6,  11, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xD7,  28, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xDB,  42, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xDC,  12, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xDF,  41, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xF0,  32, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xF3,  13, 0,	/* Not CP1252 */
	/* ??? */ 0xF6,  11, 0,	/* Not CP1252 */
	/* ??? */ 0xF7,  27, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xFB,  42, 0,	/* Not CP1252 */
	/* ??? */ 0xFC,  12, 0,	/* Not CP1252 */
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	51 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		204 (0xCC) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		153 (0x99) bytes
*/

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_POLI[41] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '"' */ 0x22,   3, MO_ShiftLeft,
	/* '#' */ 0x23,   4, MO_AltRight,
	/* '$' */ 0x24,   5, MO_AltRight,
	/* '&' */ 0x26,   8, MO_AltRight,
	/* ''' */ 0x27,  41, MO_AltRight,
	/* '(' */ 0x28,   9, MO_ShiftLeft,
	/* ')' */ 0x29,  10, MO_ShiftLeft,
	/* '*' */ 0x2A,   9, MO_AltRight,
	/* '+' */ 0x2B,   7, MO_ShiftLeft,
	/* '-' */ 0x2D,  55, 0,
	/* '/' */ 0x2F,  28, 0,
	/* ':' */ 0x3A,   4, MO_ShiftLeft,
	/* ';' */ 0x3B,  42, MO_ShiftLeft,
	/* '=' */ 0x3D,  11, MO_ShiftLeft,
	/* '?' */ 0x3F,   5, MO_ShiftLeft,
	/* '@' */ 0x40,   3, MO_AltRight,
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,  42, 0,
	/* ']' */ 0x5D,  28, MO_AltRight,
	/* '^' */ 0x5E,   7, MO_AltRight,
	/* '_' */ 0x5F,   8, MO_ShiftLeft,
	/* '{' */ 0x7B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x7C,  42, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x7D,  28, MO_ShiftLeft|MO_AltRight,
	/* ??? */ 0x91,  13, 0,	/* Not CP1252 */
	/* ??? */ 0x9C,  53, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0x9F,  27, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xA3,  40, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xA4,  42, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xA7,  28, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xAF,  12, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xB3,  40, 0,	/* Not CP1252 */
	/* ??? */ 0xB4,  13, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xB7,   1, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xB9,  41, 0,	/* Not CP1252 */
	/* ??? */ 0xBF,  12, 0,	/* Not CP1252 */
	/* ??? */ 0xE6,  55, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xEA,  41, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xF1,  54, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xF3,  27, 0,	/* Not CP1252 */
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	41 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		164 (0xA4) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		123 (0x7B) bytes
*/

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_SCS_[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '\' */ 0x5C,  42, MO_AltRight,
	/* '|' */ 0x7C,  42, MO_ShiftLeft|MO_AltRight,
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

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_SLOV[22] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,   2, MO_AltRight,
	/* '(' */ 0x28,  10, MO_AltRight,
	/* ')' */ 0x29,  11, MO_AltRight,
	/* ',' */ 0x2C,  53, MO_AltRight,
	/* '-' */ 0x2D,  12, MO_AltRight,
	/* '.' */ 0x2E,  54, MO_AltRight,
	/* ':' */ 0x3A,  40, MO_ShiftLeft|MO_AltRight,
	/* '<' */ 0x3C,  53, MO_ShiftLeft|MO_AltRight,
	/* '>' */ 0x3E,  54, MO_ShiftLeft|MO_AltRight,
	/* 'Y' */ 0x59,  22, MO_ShiftLeft|MO_AltRight,
	/* '\' */ 0x5C,  42, MO_AltRight,
	/* '_' */ 0x5F,  12, MO_ShiftLeft|MO_AltRight,
	/* '`' */ 0x60,   1, MO_AltRight,
	/* 'y' */ 0x79,  22, MO_AltRight,
	/* '{' */ 0x7B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x7C,  42, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x7D,  28, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x7E,   1, MO_ShiftLeft|MO_AltRight,
	/* ??? */ 0xA8,  45, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xDF,   1, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xF9,   1, 0,	/* Not CP1252 */
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	22 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		88 (0x58) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		66 (0x42) bytes
*/

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_SWED[23] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '#' */ 0x23,   4, MO_ShiftLeft,
	/* '$' */ 0x24,   5, MO_AltRight,
	/* '@' */ 0x40,   3, MO_AltRight,
	/* '[' */ 0x5B,   9, MO_AltRight,
	/* '\' */ 0x5C,  12, MO_AltRight,
	/* ']' */ 0x5D,  10, MO_AltRight,
	/* '^' */ 0x5E,  28, MO_ShiftLeft|MO_DeadKey,
	/* '`' */ 0x60,  13, MO_ShiftLeft|MO_DeadKey,
	/* '{' */ 0x7B,   8, MO_AltRight,
	/* '|' */ 0x7C,  45, MO_AltRight,
	/* '}' */ 0x7D,  11, MO_AltRight,
	/* '~' */ 0x7E,  28, MO_AltRight|MO_DeadKey,
	/* '€' */ 0x80,  19, MO_AltRight,
	/* '£' */ 0xA3,   4, MO_AltRight,
	/* '¤' */ 0xA4,   5, MO_ShiftLeft,
	/* '§' */ 0xA7,   1, 0,
	/* '¨' */ 0xA8,  28, MO_DeadKey,
	/* '°' */ 0xB0, NOS, 0,
	/* '´' */ 0xB4,  13, MO_DeadKey,
	/* '½' */ 0xBD,   1, MO_ShiftLeft,
	/* 'Ü' */ 0xDC, NOS, 0,
	/* 'ü' */ 0xFC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	23 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		92 (0x5C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		69 (0x45) bytes
*/

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_TURQ[44] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '"' */ 0x22,   1, 0,
	/* '#' */ 0x23,   4, MO_AltRight,
	/* '$' */ 0x24,   5, MO_AltRight,
	/* '&' */ 0x26,   7, MO_ShiftLeft,
	/* '(' */ 0x28,   9, MO_ShiftLeft,
	/* ')' */ 0x29,  10, MO_ShiftLeft,
	/* '*' */ 0x2A,  12, 0,
	/* '+' */ 0x2B,   5, MO_ShiftLeft,
	/* ',' */ 0x2C,  42, 0,
	/* '-' */ 0x2D,  13, 0,
	/* '.' */ 0x2E,  55, 0,
	/* '/' */ 0x2F,   8, MO_ShiftLeft,
	/* ':' */ 0x3A,  55, MO_ShiftLeft,
	/* ';' */ 0x3B,  42, MO_ShiftLeft,
	/* '=' */ 0x3D,  11, MO_ShiftLeft,
	/* '?' */ 0x3F,  12, MO_ShiftLeft,
	/* '@' */ 0x40,  17, MO_AltRight,
	/* '[' */ 0x5B,   9, MO_AltRight,
	/* '\' */ 0x5C,  12, MO_AltRight,
	/* ']' */ 0x5D,  10, MO_AltRight,
	/* '^' */ 0x5E,   4, MO_ShiftLeft,
	/* '_' */ 0x5F,  13, MO_ShiftLeft,
	/* '`' */ 0x60,  40, MO_AltRight,
	/* 'i' */ 0x69,  41, 0,
	/* '{' */ 0x7B,   8, MO_AltRight,
	/* '|' */ 0x7C,  45, MO_AltRight,
	/* '}' */ 0x7D,  11, MO_AltRight,
	/* '~' */ 0x7E,  28, MO_AltRight,
	/* ??? */ 0xA8,  27, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xC6,  31, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xC7,  54, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xD0,  27, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xD6,  53, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xDC,  28, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xDD,  41, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xDF,  32, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xE6,  31, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xE7,  54, 0,	/* Not CP1252 */
	/* ??? */ 0xE9,   1, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xF0,  27, 0,	/* Not CP1252 */
	/* ??? */ 0xF6,  53, 0,	/* Not CP1252 */
	/* ??? */ 0xFC,  28, 0,	/* Not CP1252 */
	/* ??? */ 0xFD,  24, 0,	/* Not CP1252 */
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	44 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		176 (0xB0) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		132 (0x84) bytes
*/

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_ROMA[56] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '"' */ 0x22,   3, MO_ShiftLeft,
	/* '#' */ 0x23,  47, MO_AltRight,
	/* '$' */ 0x24,  40, MO_AltRight,
	/* '&' */ 0x26,   7, MO_ShiftLeft,
	/* ''' */ 0x27,  13, 0,
	/* '(' */ 0x28,   9, MO_ShiftLeft,
	/* ')' */ 0x29,  10, MO_ShiftLeft,
	/* '*' */ 0x2A,  13, MO_ShiftLeft,
	/* '+' */ 0x2B,   4, MO_ShiftLeft,
	/* '-' */ 0x2D,  55, 0,
	/* '/' */ 0x2F,   8, MO_ShiftLeft,
	/* ':' */ 0x3A,  54, MO_ShiftLeft,
	/* ';' */ 0x3B,  53, MO_ShiftLeft,
	/* '<' */ 0x3C,  45, 0,
	/* '=' */ 0x3D,  11, MO_ShiftLeft,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '?' */ 0x3F,  12, MO_ShiftLeft,
	/* '@' */ 0x40,  49, MO_AltRight,
	/* 'Y' */ 0x59,  46, MO_ShiftLeft,
	/* 'Z' */ 0x5A,  22, MO_ShiftLeft,
	/* '[' */ 0x5B,   1, MO_ShiftLeft,
	/* '\' */ 0x5C,  17, MO_AltRight,
	/* ']' */ 0x5D,   1, 0,
	/* '^' */ 0x5E,   4, MO_AltRight,
	/* '_' */ 0x5F,  55, MO_ShiftLeft,
	/* '`' */ 0x60,   8, MO_AltRight,
	/* 'y' */ 0x79,  46, 0,
	/* 'z' */ 0x7A,  22, 0,
	/* '{' */ 0x7B,  50, MO_AltRight,
	/* '|' */ 0x7C,  18, MO_AltRight,
	/* '}' */ 0x7D,  51, MO_AltRight,
	/* '~' */ 0x7E,   2, MO_AltRight,
	/* ??? */ 0x82,  13, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xA1,   3, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xA2,   5, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xA3,  39, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xA4,   5, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xA7,  52, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xA8,  12, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xAA,  40, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xB0,   6, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xB3,  38, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xB4,  10, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xB7,   9, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xB8,   7, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xBA,  40, 0,	/* Not CP1252 */
	/* ??? */ 0xC2,  42, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xD0,  33, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xD7,  28, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xDE,  41, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xDF,  41, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xE2,  42, 0,	/* Not CP1252 */
	/* ??? */ 0xF0,  32, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xF7,  27, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xFE,  41, 0,	/* Not CP1252 */
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	56 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		224 (0xE0) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		168 (0xA8) bytes
*/

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_RUSS[142] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '"' */ 0x22,   3, MO_ShiftLeft,
	/* '#' */ 0x23, NOS, 0,
	/* '$' */ 0x24, NOS, 0,
	/* '&' */ 0x26, NOS, 0,
	/* ''' */ 0x27, NOS, 0,
	/* ',' */ 0x2C,  55, MO_ShiftLeft,
	/* '.' */ 0x2E,  55, 0,
	/* '/' */ 0x2F,  42, MO_ShiftLeft,
	/* ':' */ 0x3A,   7, MO_ShiftLeft,
	/* ';' */ 0x3B,   5, MO_ShiftLeft,
	/* '?' */ 0x3F,   8, MO_ShiftLeft,
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
	/* '\' */ 0x5C,  42, 0,
	/* ']' */ 0x5D, NOS, 0,
	/* '^' */ 0x5E, NOS, 0,
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
	/* '§' */ 0xA7, NOS, 0,
	/* ??? */ 0xA8,   1, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xB8,   1, 0,	/* Not CP1252 */
	/* ??? */ 0xB9,   4, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xC0,  34, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xC1,  53, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xC2,  33, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xC3,  23, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xC4,  39, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xC5,  21, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xC6,  40, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xC7,  26, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xC8,  50, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xC9,  17, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xCA,  20, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xCB,  38, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xCC,  49, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xCD,  22, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xCE,  37, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xCF,  35, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xD0,  36, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xD1,  48, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xD2,  51, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xD3,  19, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xD4,  31, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xD5,  27, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xD6,  18, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xD7,  47, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xD8,  24, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xD9,  25, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xDA,  28, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xDB,  32, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xDC,  52, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xDD,  41, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xDE,  54, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xDF,  46, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xE0,  34, 0,	/* Not CP1252 */
	/* ??? */ 0xE1,  53, 0,	/* Not CP1252 */
	/* ??? */ 0xE2,  33, 0,	/* Not CP1252 */
	/* ??? */ 0xE3,  23, 0,	/* Not CP1252 */
	/* ??? */ 0xE4,  39, 0,	/* Not CP1252 */
	/* ??? */ 0xE5,  21, 0,	/* Not CP1252 */
	/* ??? */ 0xE6,  40, 0,	/* Not CP1252 */
	/* ??? */ 0xE7,  26, 0,	/* Not CP1252 */
	/* ??? */ 0xE8,  50, 0,	/* Not CP1252 */
	/* ??? */ 0xE9,  17, 0,	/* Not CP1252 */
	/* ??? */ 0xEA,  20, 0,	/* Not CP1252 */
	/* ??? */ 0xEB,  38, 0,	/* Not CP1252 */
	/* ??? */ 0xEC,  49, 0,	/* Not CP1252 */
	/* ??? */ 0xED,  22, 0,	/* Not CP1252 */
	/* ??? */ 0xEE,  37, 0,	/* Not CP1252 */
	/* ??? */ 0xEF,  35, 0,	/* Not CP1252 */
	/* ??? */ 0xF0,  36, 0,	/* Not CP1252 */
	/* ??? */ 0xF1,  48, 0,	/* Not CP1252 */
	/* ??? */ 0xF2,  51, 0,	/* Not CP1252 */
	/* ??? */ 0xF3,  19, 0,	/* Not CP1252 */
	/* ??? */ 0xF4,  31, 0,	/* Not CP1252 */
	/* ??? */ 0xF5,  27, 0,	/* Not CP1252 */
	/* ??? */ 0xF6,  18, 0,	/* Not CP1252 */
	/* ??? */ 0xF7,  47, 0,	/* Not CP1252 */
	/* ??? */ 0xF8,  24, 0,	/* Not CP1252 */
	/* ??? */ 0xF9,  25, 0,	/* Not CP1252 */
	/* ??? */ 0xFA,  28, 0,	/* Not CP1252 */
	/* ??? */ 0xFB,  32, 0,	/* Not CP1252 */
	/* ??? */ 0xFC,  52, 0,	/* Not CP1252 */
	/* ??? */ 0xFD,  41, 0,	/* Not CP1252 */
	/* ??? */ 0xFE,  54, 0,	/* Not CP1252 */
	/* ??? */ 0xFF,  46, 0,	/* Not CP1252 */
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	142 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		568 (0x238) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		426 (0x1AA) bytes
*/

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_TURF[47] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '"' */ 0x22,   3, MO_ShiftLeft,
	/* '#' */ 0x23,   4, MO_AltRight,
	/* '$' */ 0x24,   5, MO_ShiftLeft,
	/* '%' */ 0x25,   6, MO_ShiftLeft,
	/* '&' */ 0x26,   7, MO_ShiftLeft,
	/* ''' */ 0x27,   8, MO_ShiftLeft,
	/* '(' */ 0x28,   9, MO_ShiftLeft,
	/* ')' */ 0x29,  10, MO_ShiftLeft,
	/* '-' */ 0x2D,  13, 0,
	/* '/' */ 0x2F,  12, 0,
	/* ':' */ 0x3A,  54, MO_ShiftLeft,
	/* '=' */ 0x3D,  11, MO_ShiftLeft,
	/* '?' */ 0x3F,  12, MO_ShiftLeft,
	/* '@' */ 0x40,  17, MO_AltRight,
	/* 'I' */ 0x49, NOS, 0,
	/* '[' */ 0x5B,   9, MO_AltRight,
	/* '\' */ 0x5C,  12, MO_AltRight,
	/* ']' */ 0x5D,  10, MO_AltRight,
	/* '^' */ 0x5E,   4, MO_ShiftLeft,
	/* '`' */ 0x60,  42, MO_AltRight,
	/* 'i' */ 0x69,  20, MO_ShiftLeft,
	/* '{' */ 0x7B,   8, MO_AltRight,
	/* '|' */ 0x7C,  45, MO_AltRight,
	/* '}' */ 0x7D,  11, MO_AltRight,
	/* '~' */ 0x7E,  28, MO_AltRight,
	/* ??? */ 0xA2,  48, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xA3,  26, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xA8,  27, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xAB,  46, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xAC,   1, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xB2,   3, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xB4,  40, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xB5,  52, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xB6,  20, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xB9,   2, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xBA,  25, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xBB,  47, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xBC,   5, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xBD,   6, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xBE,   7, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xC6,  31, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xD7,  53, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xDF,  32, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xE6,  31, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xF7,  54, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xFD,  20, 0,	/* Not CP1252 */
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	47 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		188 (0xBC) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		141 (0x8D) bytes
*/

/* TermId=  5 */
const CKeyFixup DEC_VT510_PC_JAPA[3] =
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
#define IBM_3486_102_USA_		NoExceptions	/* not required */

/* TermId=  6 	, 45 , 84 */
const CKeyFixup IBM_3486_102_BELG[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/*     */ 0x7F,  76, 0,
	/* '€' */ 0x80, NOS, 0,
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

/* TermId=  6 	, 45 , 84 */
const CKeyFixup IBM_3486_102_FINN[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/*     */ 0x7F,  76, 0,
	/* '€' */ 0x80, NOS, 0,
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

/* TermId=  6 	, 45 , 84 */
const CKeyFixup IBM_3486_102_FRAN[27] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,   9, 0,
	/* '*' */ 0x2A,  28, MO_ShiftLeft,
	/* '+' */ 0x2B,  55, MO_ShiftLeft,
	/* '-' */ 0x2D,  13, 0,
	/* '=' */ 0x3D,  55, 0,
	/* '@' */ 0x40,   3, MO_AltRight,
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,  45, MO_AltRight,
	/* ']' */ 0x5D,  28, MO_AltRight,
	/* '^' */ 0x5E,  27, 0,
	/* '_' */ 0x5F,  13, MO_ShiftLeft,
	/* '`' */ 0x60,   8, MO_AltRight,
	/* '{' */ 0x7B,  10, MO_AltRight,
	/* '|' */ 0x7C,   2, MO_AltRight,
	/* '}' */ 0x7D,  11, MO_AltRight,
	/* '~' */ 0x7E,  55, MO_AltRight,
	/*     */ 0x7F,  76, 0,
	/* '€' */ 0x80, NOS, 0,
	/* '£' */ 0xA3,  42, MO_ShiftLeft,
	/* '¤' */ 0xA4, NOS, 0,
	/* '§' */ 0xA7,   7, 0,
	/* '¨' */ 0xA8,  27, MO_ShiftLeft,
	/* '°' */ 0xB0, NOS, 0,
	/* '²' */ 0xB2, NOS, 0,
	/* '´' */ 0xB4,  41, MO_AltRight,
	/* 'µ' */ 0xB5,  42, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	27 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		108 (0x6C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		81 (0x51) bytes
*/

/* TermId=  6 	, 45 , 84 */
const CKeyFixup IBM_3486_102_GERM[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/*     */ 0x7F,  76, 0,
	/* '€' */ 0x80, NOS, 0,
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

/* TermId=  6 	, 45 , 84 */
const CKeyFixup IBM_3486_102_ITAL[11] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '#' */ 0x23,   4, MO_AltRight,
	/* '@' */ 0x40,  17, MO_AltRight,
	/* '[' */ 0x5B,   9, MO_AltRight,
	/* ']' */ 0x5D,  10, MO_AltRight,
	/* '`' */ 0x60,  42, MO_AltRight,
	/* '{' */ 0x7B,   8, MO_AltRight,
	/* '}' */ 0x7D,  11, MO_AltRight,
	/* '~' */ 0x7E,  28, MO_AltRight,
	/*     */ 0x7F,  76, 0,
	/* '€' */ 0x80, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	11 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		44 (0x2C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		33 (0x21) bytes
*/

/* TermId=  6 	, 45 , 84 */
const CKeyFixup IBM_3486_102_SWIT[10] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '^' */ 0x5E,  13, 0,
	/* '`' */ 0x60,  13, MO_ShiftLeft,
	/* '~' */ 0x7E,  13, MO_AltRight,
	/*     */ 0x7F,  76, 0,
	/* '€' */ 0x80, NOS, 0,
	/* '¢' */ 0xA2,   9, MO_AltRight,
	/* '¨' */ 0xA8,  28, 0,
	/* '¬' */ 0xAC,   7, MO_AltRight,
	/* '´' */ 0xB4,  12, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	10 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		40 (0x28) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		30 (0x1E) bytes
*/

/* TermId=  6 	, 45 , 84 */
const CKeyFixup IBM_3486_102_UKIN[27] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '#' */ 0x23,  11, MO_ShiftLeft,
	/* '&' */ 0x26,   7, MO_ShiftLeft,
	/* ''' */ 0x27,  27, MO_ShiftLeft,
	/* '(' */ 0x28,   9, MO_ShiftLeft,
	/* ')' */ 0x29,  10, MO_ShiftLeft,
	/* '*' */ 0x2A,  41, MO_ShiftLeft,
	/* '+' */ 0x2B,  40, MO_ShiftLeft,
	/* ':' */ 0x3A,  41, 0,
	/* '=' */ 0x3D,  12, MO_ShiftLeft,
	/* '@' */ 0x40,  27, 0,
	/* '[' */ 0x5B,  28, 0,
	/* '\' */ 0x5C,   1, 0,
	/* ']' */ 0x5D,  42, 0,
	/* '^' */ 0x5E,  41, MO_AltRight,
	/* '_' */ 0x5F,  13, MO_ShiftLeft,
	/* '`' */ 0x60, NOS, 0,
	/* '{' */ 0x7B,  28, MO_ShiftLeft,
	/* '|' */ 0x7C,   1, MO_ShiftLeft,
	/* '}' */ 0x7D,  42, MO_ShiftLeft,
	/* '~' */ 0x7E,  28, MO_AltRight,
	/*     */ 0x7F,  76, 0,
	/* '€' */ 0x80, NOS, 0,
	/* '¬' */ 0xAC,  13, 0,
	/* '²' */ 0xB2,   3, MO_AltRight,
	/* '³' */ 0xB3,   4, MO_AltRight,
	/* '´' */ 0xB4,   8, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	27 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		108 (0x6C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		81 (0x51) bytes
*/

/* TermId=  6 	, 45 , 84 */
const CKeyFixup IBM_3486_102_DENM[10] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '^' */ 0x5E,  28, MO_ShiftLeft,
	/* '`' */ 0x60,  13, MO_ShiftLeft,
	/* '~' */ 0x7E,  28, MO_AltRight,
	/*     */ 0x7F,  76, 0,
	/* '€' */ 0x80, NOS, 0,
	/* '§' */ 0xA7, NOS, 0,
	/* '¨' */ 0xA8, NOS, 0,
	/* '´' */ 0xB4,  13, 0,
	/* '½' */ 0xBD, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	10 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		40 (0x28) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		30 (0x1E) bytes
*/

/* TermId=  6 	, 45 , 84 */
const CKeyFixup IBM_3486_102_NORW[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/*     */ 0x7F,  76, 0,
	/* '€' */ 0x80, NOS, 0,
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

/* TermId=  6 	, 45 , 84 */
const CKeyFixup IBM_3486_102_SPAI[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/*     */ 0x7F,  76, 0,
	/* '€' */ 0x80, NOS, 0,
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
/* IBM_3486_102_DUTC is not supported */
/* IBM_3486_102_HEBR is not supported */
/* IBM_3486_102_PORT is not supported */

/* TermId=  6 	, 45 , 84 */
const CKeyFixup IBM_3486_102_LATI[2] =
{
	/*        ANSI,  KN, MODIFIERS */
	/*     */ 0x7F,  76, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	2 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		8 (0x8) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		6 (0x6) bytes
*/
/* IBM_3486_102_CZEC is not supported */
/* IBM_3486_102_BRAZ is not supported */
/* IBM_3486_102_GREK is not supported */
/* IBM_3486_102_FCAN is not supported */
/* IBM_3486_102_HUNG is not supported */
/* IBM_3486_102_POLI is not supported */
/* IBM_3486_102_SCS_ is not supported */
/* IBM_3486_102_SLOV is not supported */
/* IBM_3486_102_SWED is not supported */
/* IBM_3486_102_TURQ is not supported */
/* IBM_3486_102_ROMA is not supported */
/* IBM_3486_102_RUSS is not supported */
/* IBM_3486_102_TURF is not supported */
/* IBM_3486_102_JAPA is not supported */
#define IBM_3191_122_USA_		NoExceptions	/* not required */

/* TermId=  7 	, 24 , 46 , 71 */
const CKeyFixup IBM_3191_122_BELG[55] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,   9, 0,
	/* '"' */ 0x22,   4, 0,
	/* '#' */ 0x23,   4, MO_AltRight,
	/* '$' */ 0x24,  28, 0,
	/* '%' */ 0x25,  41, MO_ShiftLeft,
	/* '&' */ 0x26,   2, 0,
	/* ''' */ 0x27,   5, 0,
	/* '(' */ 0x28,   6, 0,
	/* ')' */ 0x29,  12, 0,
	/* '+' */ 0x2B,  55, MO_ShiftLeft,
	/* ',' */ 0x2C,  52, 0,
	/* '-' */ 0x2D,  13, 0,
	/* '.' */ 0x2E,  53, MO_ShiftLeft,
	/* '/' */ 0x2F,  54, MO_ShiftLeft,
	/* '0' */ 0x30,  11, MO_ShiftLeft,
	/* '1' */ 0x31,   2, MO_ShiftLeft,
	/* '2' */ 0x32,   3, MO_ShiftLeft,
	/* '3' */ 0x33,   4, MO_ShiftLeft,
	/* '4' */ 0x34,   5, MO_ShiftLeft,
	/* '5' */ 0x35,   6, MO_ShiftLeft,
	/* '6' */ 0x36,   7, MO_ShiftLeft,
	/* '7' */ 0x37,   8, MO_ShiftLeft,
	/* '8' */ 0x38,   9, MO_ShiftLeft,
	/* '9' */ 0x39,  10, MO_ShiftLeft,
	/* ':' */ 0x3A,  54, 0,
	/* ';' */ 0x3B,  53, 0,
	/* '=' */ 0x3D,  55, 0,
	/* '?' */ 0x3F,  52, MO_ShiftLeft,
	/* '@' */ 0x40,   3, MO_AltRight,
	/* 'M' */ 0x4D,  40, MO_ShiftLeft,
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,  45, MO_AltRight,
	/* ']' */ 0x5D,  28, MO_AltRight,
	/* '^' */ 0x5E,   7, MO_AltRight,
	/* '_' */ 0x5F,  13, MO_ShiftLeft,
	/* '`' */ 0x60,  42, MO_AltRight,
	/* 'm' */ 0x6D,  40, 0,
	/* '{' */ 0x7B,  10, MO_AltRight,
	/* '|' */ 0x7C,   2, MO_AltRight,
	/* '}' */ 0x7D,  11, MO_AltRight,
	/* '~' */ 0x7E,  55, MO_AltRight,
	/* '£' */ 0xA3,  42, MO_ShiftLeft,
	/* '§' */ 0xA7,   7, 0,
	/* '¨' */ 0xA8,  27, MO_ShiftLeft,
	/* '°' */ 0xB0,  12, MO_ShiftLeft,
	/* '²' */ 0xB2,   1, 0,
	/* '³' */ 0xB3,   1, MO_ShiftLeft,
	/* '´' */ 0xB4,  41, MO_AltRight,
	/* 'µ' */ 0xB5,  42, 0,
	/* 'à' */ 0xE0,  11, 0,
	/* 'ç' */ 0xE7,  10, 0,
	/* 'è' */ 0xE8,   8, 0,
	/* 'é' */ 0xE9,   3, 0,
	/* 'ù' */ 0xF9,  41, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	55 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		220 (0xDC) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		165 (0xA5) bytes
*/
#define IBM_3191_122_FINN		NoExceptions	/* not required */
#define IBM_3191_122_FRAN		NoExceptions	/* not required */
#define IBM_3191_122_GERM		NoExceptions	/* not required */
#define IBM_3191_122_ITAL		NoExceptions	/* not required */

/* TermId=  7 	, 24 , 46 , 71 */
const CKeyFixup IBM_3191_122_SWIT[15] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '#' */ 0x23,   4, MO_AltRight,
	/* '@' */ 0x40,   3, MO_AltRight,
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,  45, MO_AltRight,
	/* ']' */ 0x5D,  28, MO_AltRight,
	/* '{' */ 0x7B,  41, MO_AltRight,
	/* '|' */ 0x7C,   2, MO_AltRight,
	/* '}' */ 0x7D,  42, MO_AltRight,
	/* '~' */ 0x7E,  13, MO_AltRight,
	/* '¢' */ 0xA2,   9, MO_AltRight,
	/* '§' */ 0xA7,   1, 0,
	/* '¬' */ 0xAC,   7, MO_AltRight,
	/* '°' */ 0xB0,   1, MO_ShiftLeft,
	/* '´' */ 0xB4,  12, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	15 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		60 (0x3C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		45 (0x2D) bytes
*/
#define IBM_3191_122_UKIN		NoExceptions	/* not required */
#define IBM_3191_122_DENM		NoExceptions	/* not required */
#define IBM_3191_122_NORW		NoExceptions	/* not required */
#define IBM_3191_122_SPAI		NoExceptions	/* not required */
/* IBM_3191_122_DUTC is not supported */
/* IBM_3191_122_HEBR is not supported */
/* IBM_3191_122_PORT is not supported */

/* TermId=  7 	, 24 , 46 , 71 */
const CKeyFixup IBM_3191_122_LATI[10] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '\' */ 0x5C,  27, 0,
	/* '`' */ 0x60,  27, MO_ShiftLeft,
	/* '|' */ 0x7C, NOS, 0,
	/* '¡' */ 0xA1, NOS, 0,
	/* '¨' */ 0xA8,  13, MO_ShiftLeft,
	/* '¬' */ 0xAC,  13, 0,
	/* '°' */ 0xB0, NOS, 0,
	/* '´' */ 0xB4, NOS, 0,
	/* '¿' */ 0xBF, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	10 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		40 (0x28) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		30 (0x1E) bytes
*/
/* IBM_3191_122_CZEC is not supported */
/* IBM_3191_122_BRAZ is not supported */
/* IBM_3191_122_GREK is not supported */
/* IBM_3191_122_FCAN is not supported */
/* IBM_3191_122_HUNG is not supported */
/* IBM_3191_122_POLI is not supported */
/* IBM_3191_122_SCS_ is not supported */
/* IBM_3191_122_SLOV is not supported */
/* IBM_3191_122_SWED is not supported */
/* IBM_3191_122_TURQ is not supported */
/* IBM_3191_122_ROMA is not supported */
/* IBM_3191_122_RUSS is not supported */
/* IBM_3191_122_TURF is not supported */
/* IBM_3191_122_JAPA is not supported */

/* TermId=  8 	, 72 */
const CKeyFixup IBM_3486_122_USA_[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '¢' */ 0xA2,  27, 0,
	/* '¬' */ 0xAC,   7, MO_ShiftLeft,
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

/* TermId=  8 	, 72 */
const CKeyFixup IBM_3486_122_BELG[60] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,   2, MO_ShiftLeft,
	/* '"' */ 0x22,   3, MO_ShiftLeft,
	/* '#' */ 0x23,   4, MO_ShiftLeft,
	/* '$' */ 0x24,   5, MO_ShiftLeft,
	/* '%' */ 0x25,   6, MO_ShiftLeft,
	/* '&' */ 0x26,  28, 0,
	/* ''' */ 0x27,  12, 0,
	/* '(' */ 0x28,   9, MO_ShiftLeft,
	/* ')' */ 0x29,  10, MO_ShiftLeft,
	/* '+' */ 0x2B,   7, MO_ShiftLeft,
	/* ',' */ 0x2C,  53, 0,
	/* '-' */ 0x2D,  55, 0,
	/* '.' */ 0x2E,  54, 0,
	/* '/' */ 0x2F,   8, MO_ShiftLeft,
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
	/* ':' */ 0x3A,  54, MO_ShiftLeft,
	/* ';' */ 0x3B,  53, MO_ShiftLeft,
	/* '<' */ 0x3C,  45, 0,
	/* '=' */ 0x3D,  11, MO_ShiftLeft,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '?' */ 0x3F,  12, MO_ShiftLeft,
	/* '@' */ 0x40, NOS, 0,
	/* 'M' */ 0x4D,  52, MO_ShiftLeft,
	/* '[' */ 0x5B,  27, MO_ShiftLeft,
	/* '\' */ 0x5C, NOS, 0,
	/* ']' */ 0x5D,  42, MO_ShiftLeft,
	/* '^' */ 0x5E,  13, 0,
	/* '_' */ 0x5F,  55, MO_ShiftLeft,
	/* '`' */ 0x60,  42, 0,
	/* 'm' */ 0x6D,  52, 0,
	/* '{' */ 0x7B, NOS, 0,
	/* '|' */ 0x7C, NOS, 0,
	/* '}' */ 0x7D, NOS, 0,
	/* '~' */ 0x7E, NOS, 0,
	/*     */ 0x7F,  86, 0,
	/* '¢' */ 0xA2, NOS, 0,
	/* '£' */ 0xA3, NOS, 0,
	/* '§' */ 0xA7, NOS, 0,
	/* '¨' */ 0xA8,  13, MO_ShiftLeft,
	/* '¬' */ 0xAC, NOS, 0,
	/* '°' */ 0xB0, NOS, 0,
	/* '²' */ 0xB2, NOS, 0,
	/* '³' */ 0xB3, NOS, 0,
	/* '´' */ 0xB4, NOS, 0,
	/* 'µ' */ 0xB5, NOS, 0,
	/* 'à' */ 0xE0,  41, 0,
	/* 'ç' */ 0xE7,  40, MO_ShiftLeft,
	/* 'è' */ 0xE8,  27, 0,
	/* 'é' */ 0xE9,  40, 0,
	/* 'ù' */ 0xF9,  41, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	60 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		240 (0xF0) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		180 (0xB4) bytes
*/

/* TermId=  8 	, 72 */
const CKeyFixup IBM_3486_122_FINN[27] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,   2, MO_ShiftLeft,
	/* '#' */ 0x23, NOS, 0,
	/* '$' */ 0x24, NOS, 0,
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '@' */ 0x40, NOS, 0,
	/* '[' */ 0x5B, NOS, 0,
	/* '\' */ 0x5C, NOS, 0,
	/* ']' */ 0x5D, NOS, 0,
	/* '^' */ 0x5E,  28, MO_ShiftLeft,
	/* '`' */ 0x60, NOS, 0,
	/* '{' */ 0x7B, NOS, 0,
	/* '|' */ 0x7C, NOS, 0,
	/* '}' */ 0x7D, NOS, 0,
	/* '~' */ 0x7E, NOS, 0,
	/*     */ 0x7F,  86, 0,
	/* '¢' */ 0xA2, NOS, 0,
	/* '£' */ 0xA3, NOS, 0,
	/* '§' */ 0xA7,   4, MO_ShiftLeft,
	/* '¨' */ 0xA8, NOS, 0,
	/* '¬' */ 0xAC, NOS, 0,
	/* '´' */ 0xB4, NOS, 0,
	/* '½' */ 0xBD, NOS, 0,
	/* 'É' */ 0xC9,  13, MO_ShiftLeft,
	/* 'é' */ 0xE9,  13, 0,
	/* 'ü' */ 0xFC,  28, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	27 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		108 (0x6C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		81 (0x51) bytes
*/

/* TermId=  8 	, 72 */
const CKeyFixup IBM_3486_122_FRAN[22] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,   9, 0,
	/* '#' */ 0x23, NOS, 0,
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '@' */ 0x40, NOS, 0,
	/* '[' */ 0x5B, NOS, 0,
	/* '\' */ 0x5C, NOS, 0,
	/* ']' */ 0x5D, NOS, 0,
	/* '^' */ 0x5E,  27, 0,
	/* '`' */ 0x60, NOS, 0,
	/* '{' */ 0x7B, NOS, 0,
	/* '|' */ 0x7C, NOS, 0,
	/* '}' */ 0x7D, NOS, 0,
	/* '~' */ 0x7E, NOS, 0,
	/*     */ 0x7F,  86, 0,
	/* '¢' */ 0xA2, NOS, 0,
	/* '¬' */ 0xAC, NOS, 0,
	/* '°' */ 0xB0,  12, MO_ShiftLeft,
	/* '²' */ 0xB2,   1, 0,
	/* '³' */ 0xB3,   1, MO_ShiftLeft,
	/* '´' */ 0xB4, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	22 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		88 (0x58) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		66 (0x42) bytes
*/

/* TermId=  8 	, 72 */
const CKeyFixup IBM_3486_122_GERM[22] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,   2, MO_ShiftLeft,
	/* ''' */ 0x27,  13, 0,
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '@' */ 0x40, NOS, 0,
	/* '[' */ 0x5B, NOS, 0,
	/* '\' */ 0x5C, NOS, 0,
	/* ']' */ 0x5D, NOS, 0,
	/* '^' */ 0x5E,  42, MO_ShiftLeft,
	/* '`' */ 0x60,  13, MO_ShiftLeft,
	/* '{' */ 0x7B, NOS, 0,
	/* '|' */ 0x7C, NOS, 0,
	/* '}' */ 0x7D, NOS, 0,
	/* '~' */ 0x7E, NOS, 0,
	/*     */ 0x7F,  86, 0,
	/* '¢' */ 0xA2, NOS, 0,
	/* '¬' */ 0xAC, NOS, 0,
	/* '°' */ 0xB0, NOS, 0,
	/* '²' */ 0xB2, NOS, 0,
	/* '³' */ 0xB3, NOS, 0,
	/* '´' */ 0xB4, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	22 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		88 (0x58) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		66 (0x42) bytes
*/

/* TermId=  8 	, 72 */
const CKeyFixup IBM_3486_122_ITAL[18] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,   2, MO_ShiftLeft,
	/* '#' */ 0x23, NOS, 0,
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '@' */ 0x40, NOS, 0,
	/* '[' */ 0x5B, NOS, 0,
	/* '\' */ 0x5C, NOS, 0,
	/* ']' */ 0x5D, NOS, 0,
	/* '^' */ 0x5E,  13, MO_ShiftLeft,
	/* '`' */ 0x60, NOS, 0,
	/* '{' */ 0x7B, NOS, 0,
	/* '|' */ 0x7C, NOS, 0,
	/* '}' */ 0x7D, NOS, 0,
	/* '~' */ 0x7E, NOS, 0,
	/*     */ 0x7F,  86, 0,
	/* '¢' */ 0xA2, NOS, 0,
	/* '¬' */ 0xAC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	18 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		72 (0x48) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		54 (0x36) bytes
*/

/* TermId=  8 	, 72 */
const CKeyFixup IBM_3486_122_SWIT[20] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,  28, MO_ShiftLeft,
	/* '#' */ 0x23,   1, 0,
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '@' */ 0x40, NOS, 0,
	/* '[' */ 0x5B, NOS, 0,
	/* '\' */ 0x5C, NOS, 0,
	/* ']' */ 0x5D, NOS, 0,
	/* '^' */ 0x5E,  13, 0,
	/* '{' */ 0x7B, NOS, 0,
	/* '|' */ 0x7C,   1, MO_ShiftLeft,
	/* '}' */ 0x7D, NOS, 0,
	/* '~' */ 0x7E, NOS, 0,
	/*     */ 0x7F,  86, 0,
	/* '¢' */ 0xA2, NOS, 0,
	/* '§' */ 0xA7, NOS, 0,
	/* '¬' */ 0xAC, NOS, 0,
	/* '°' */ 0xB0, NOS, 0,
	/* '´' */ 0xB4, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	20 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		80 (0x50) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		60 (0x3C) bytes
*/

/* TermId=  8 	, 72 */
const CKeyFixup IBM_3486_122_UKIN[17] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,   2, MO_ShiftLeft,
	/* '<' */ 0x3C,  53, MO_ShiftLeft,
	/* '>' */ 0x3E,  54, MO_ShiftLeft,
	/* '[' */ 0x5B, NOS, 0,
	/* '\' */ 0x5C,   1, 0,
	/* ']' */ 0x5D, NOS, 0,
	/* '^' */ 0x5E, NOS, 0,
	/* '{' */ 0x7B,  28, MO_ShiftLeft,
	/* '|' */ 0x7C,   1, MO_ShiftLeft,
	/* '}' */ 0x7D,  42, MO_ShiftLeft,
	/* '~' */ 0x7E, NOS, 0,
	/*     */ 0x7F,  86, 0,
	/* '¢' */ 0xA2, NOS, 0,
	/* '¬' */ 0xAC,  13, 0,
	/* '²' */ 0xB2, NOS, 0,
	/* '³' */ 0xB3, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	17 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		68 (0x44) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		51 (0x33) bytes
*/

/* TermId=  8 	, 72 */
const CKeyFixup IBM_3486_122_DENM[20] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,   2, MO_ShiftLeft,
	/* '$' */ 0x24, NOS, 0,
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '@' */ 0x40, NOS, 0,
	/* '[' */ 0x5B, NOS, 0,
	/* '\' */ 0x5C,  13, 0,
	/* ']' */ 0x5D, NOS, 0,
	/* '^' */ 0x5E,  28, MO_ShiftLeft,
	/* '{' */ 0x7B, NOS, 0,
	/* '|' */ 0x7C, NOS, 0,
	/* '}' */ 0x7D, NOS, 0,
	/* '~' */ 0x7E, NOS, 0,
	/*     */ 0x7F,  86, 0,
	/* '¢' */ 0xA2, NOS, 0,
	/* '£' */ 0xA3, NOS, 0,
	/* '¬' */ 0xAC, NOS, 0,
	/* '´' */ 0xB4, NOS, 0,
	/* 'ü' */ 0xFC,  28, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	20 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		80 (0x50) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		60 (0x3C) bytes
*/

/* TermId=  8 	, 72 */
const CKeyFixup IBM_3486_122_NORW[23] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,   2, MO_ShiftLeft,
	/* '$' */ 0x24, NOS, 0,
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '@' */ 0x40, NOS, 0,
	/* '[' */ 0x5B, NOS, 0,
	/* '\' */ 0x5C,  13, 0,
	/* ']' */ 0x5D, NOS, 0,
	/* '^' */ 0x5E,  28, MO_ShiftLeft,
	/* '`' */ 0x60,  13, MO_ShiftLeft,
	/* '{' */ 0x7B, NOS, 0,
	/* '|' */ 0x7C, NOS, 0,
	/* '}' */ 0x7D, NOS, 0,
	/* '~' */ 0x7E, NOS, 0,
	/*     */ 0x7F,  86, 0,
	/* '¢' */ 0xA2, NOS, 0,
	/* '£' */ 0xA3, NOS, 0,
	/* '§' */ 0xA7, NOS, 0,
	/* '¨' */ 0xA8, NOS, 0,
	/* '¬' */ 0xAC, NOS, 0,
	/* '´' */ 0xB4, NOS, 0,
	/* 'ü' */ 0xFC,  28, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	23 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		92 (0x5C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		69 (0x45) bytes
*/

/* TermId=  8 	, 72 */
const CKeyFixup IBM_3486_122_SPAI[15] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,   2, MO_ShiftLeft,
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,   1, MO_AltRight,
	/* ']' */ 0x5D,  28, MO_AltRight,
	/* '^' */ 0x5E,  27, MO_ShiftLeft|MO_DeadKey,
	/* '{' */ 0x7B,  41, MO_AltRight,
	/* '|' */ 0x7C,   2, MO_AltRight,
	/* '}' */ 0x7D,  42, MO_AltRight,
	/* '~' */ 0x7E, NOS, 0,
	/*     */ 0x7F,  86, 0,
	/* '¢' */ 0xA2, NOS, 0,
	/* '¬' */ 0xAC,   7, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	15 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		60 (0x3C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		45 (0x2D) bytes
*/
/* IBM_3486_122_DUTC is not supported */
/* IBM_3486_122_HEBR is not supported */
/* IBM_3486_122_PORT is not supported */

/* TermId=  8 	, 72 */
const CKeyFixup IBM_3486_122_LATI[18] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,   2, MO_ShiftLeft,
	/* '#' */ 0x23, NOS, 0,
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '@' */ 0x40,   4, MO_ShiftLeft,
	/* '[' */ 0x5B,  41, MO_ShiftLeft,
	/* '\' */ 0x5C,   1, MO_AltRight,
	/* ']' */ 0x5D,  42, MO_ShiftLeft,
	/* '^' */ 0x5E, NOS, 0,
	/* '{' */ 0x7B,  41, 0,
	/* '|' */ 0x7C,   1, 0,
	/* '}' */ 0x7D,  42, 0,
	/* '~' */ 0x7E, NOS, 0,
	/*     */ 0x7F,  86, 0,
	/* '¢' */ 0xA2, NOS, 0,
	/* '¬' */ 0xAC,   7, MO_AltRight,
	/* '´' */ 0xB4,  27, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	18 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		72 (0x48) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		54 (0x36) bytes
*/
/* IBM_3486_122_CZEC is not supported */
/* IBM_3486_122_BRAZ is not supported */
/* IBM_3486_122_GREK is not supported */
/* IBM_3486_122_FCAN is not supported */
/* IBM_3486_122_HUNG is not supported */
/* IBM_3486_122_POLI is not supported */
/* IBM_3486_122_SCS_ is not supported */
/* IBM_3486_122_SLOV is not supported */
/* IBM_3486_122_SWED is not supported */
/* IBM_3486_122_TURQ is not supported */
/* IBM_3486_122_ROMA is not supported */
/* IBM_3486_122_RUSS is not supported */
/* IBM_3486_122_TURF is not supported */
/* IBM_3486_122_JAPA is not supported */
#define Telex_88_USA_		NoExceptions	/* not required */

/* TermId= 25 */
const CKeyFixup Telex_88_BELG[4] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* ''' */ 0x27, NOS, 0,
	/* '^' */ 0x5E,  13, 0,
	/* '´' */ 0xB4,  12, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	4 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		16 (0x10) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		12 (0xC) bytes
*/

/* TermId= 25 */
const CKeyFixup Telex_88_FINN[2] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '^' */ 0x5E,  28, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	2 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		8 (0x8) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		6 (0x6) bytes
*/

/* TermId= 25 */
const CKeyFixup Telex_88_FRAN[41] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,   2, MO_ShiftLeft,
	/* '"' */ 0x22,   3, MO_ShiftLeft,
	/* '$' */ 0x24,   5, MO_ShiftLeft,
	/* '%' */ 0x25,   6, MO_ShiftLeft,
	/* '&' */ 0x26,  28, 0,
	/* ''' */ 0x27,  12, 0,
	/* '(' */ 0x28,   9, MO_ShiftLeft,
	/* ')' */ 0x29,  10, MO_ShiftLeft,
	/* '+' */ 0x2B,   7, MO_ShiftLeft,
	/* ',' */ 0x2C,  53, 0,
	/* '-' */ 0x2D,  55, 0,
	/* '.' */ 0x2E,  54, 0,
	/* '/' */ 0x2F,   8, MO_ShiftLeft,
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
	/* ':' */ 0x3A,  54, MO_ShiftLeft,
	/* ';' */ 0x3B,  53, MO_ShiftLeft,
	/* '=' */ 0x3D,  11, MO_ShiftLeft,
	/* '?' */ 0x3F,  12, MO_ShiftLeft,
	/* '^' */ 0x5E,  13, 0,
	/* '_' */ 0x5F,  55, MO_ShiftLeft,
	/* '`' */ 0x60,  42, 0,
	/* '§' */ 0xA7,   4, MO_ShiftLeft,
	/* '¨' */ 0xA8,  13, MO_ShiftLeft,
	/* '°' */ 0xB0,  41, MO_ShiftLeft,
	/* '²' */ 0xB2, NOS, 0,
	/* '³' */ 0xB3, NOS, 0,
	/* 'µ' */ 0xB5, NOS, 0,
	/* 'à' */ 0xE0,  27, 0,
	/* 'ç' */ 0xE7,  27, MO_ShiftLeft,
	/* 'è' */ 0xE8, NOS, 0,
	/* 'é' */ 0xE9, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	41 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		164 (0xA4) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		123 (0x7B) bytes
*/

/* TermId= 25 */
const CKeyFixup Telex_88_GERM[2] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '^' */ 0x5E,  42, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	2 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		8 (0x8) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		6 (0x6) bytes
*/

/* TermId= 25 */
const CKeyFixup Telex_88_ITAL[2] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '^' */ 0x5E,  13, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	2 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		8 (0x8) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		6 (0x6) bytes
*/

/* TermId= 25 */
const CKeyFixup Telex_88_SWIT[5] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '#' */ 0x23, NOS, 0,
	/* '+' */ 0x2B, NOS, 0,
	/* '^' */ 0x5E,  13, 0,
	/* '|' */ 0x7C,   2, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	5 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		20 (0x14) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		15 (0xF) bytes
*/

/* TermId= 25 */
const CKeyFixup Telex_88_UKIN[2] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '^' */ 0x5E, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	2 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		8 (0x8) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		6 (0x6) bytes
*/

/* TermId= 25 */
const CKeyFixup Telex_88_DENM[2] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '^' */ 0x5E,  28, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	2 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		8 (0x8) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		6 (0x6) bytes
*/

/* TermId= 25 */
const CKeyFixup Telex_88_NORW[2] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '^' */ 0x5E,  28, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	2 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		8 (0x8) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		6 (0x6) bytes
*/

/* TermId= 25 */
const CKeyFixup Telex_88_SPAI[24] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,  42, MO_AltRight,
	/* '#' */ 0x23, NOS, 0,
	/* '$' */ 0x24, NOS, 0,
	/* '@' */ 0x40,   4, MO_ShiftLeft,
	/* '[' */ 0x5B,  41, MO_ShiftLeft,
	/* '\' */ 0x5C,  27, 0,
	/* ']' */ 0x5D,  42, MO_ShiftLeft,
	/* '^' */ 0x5E, NOS, 0,
	/* '`' */ 0x60,  27, MO_ShiftLeft,
	/* '{' */ 0x7B,  41, 0,
	/* '|' */ 0x7C,   2, MO_ShiftLeft,
	/* '}' */ 0x7D,  42, 0,
	/* '¡' */ 0xA1, NOS, 0,
	/* '¢' */ 0xA2,  41, MO_AltRight,
	/* '¨' */ 0xA8,  13, MO_ShiftLeft,
	/* 'ª' */ 0xAA, NOS, 0,
	/* '¬' */ 0xAC,  13, 0,
	/* '°' */ 0xB0, NOS, 0,
	/* '´' */ 0xB4, NOS, 0,
	/* '·' */ 0xB7, NOS, 0,
	/* '¿' */ 0xBF, NOS, 0,
	/* 'Ç' */ 0xC7, NOS, 0,
	/* 'ç' */ 0xE7, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	24 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		96 (0x60) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		72 (0x48) bytes
*/
/* Telex_88_DUTC is not supported */
/* Telex_88_HEBR is not supported */
/* Telex_88_PORT is not supported */
/* Telex_88_LATI is not supported */
/* Telex_88_CZEC is not supported */
/* Telex_88_BRAZ is not supported */
/* Telex_88_GREK is not supported */
/* Telex_88_FCAN is not supported */
/* Telex_88_HUNG is not supported */
/* Telex_88_POLI is not supported */
/* Telex_88_SCS_ is not supported */
/* Telex_88_SLOV is not supported */
/* Telex_88_SWED is not supported */
/* Telex_88_TURQ is not supported */
/* Telex_88_ROMA is not supported */
/* Telex_88_RUSS is not supported */
/* Telex_88_TURF is not supported */
/* Telex_88_JAPA is not supported */
#define BDS_7_USA_		NoExceptions	/* not required */

/* TermId= 35 */
const CKeyFixup BDS_7_BELG[7] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,  45, MO_AltRight,
	/* '{' */ 0x7B,  10, MO_AltRight,
	/* '|' */ 0x7C,   2, MO_AltRight,
	/* '}' */ 0x7D,  11, MO_AltRight,
	/* '€' */ 0x80, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	7 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		28 (0x1C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		21 (0x15) bytes
*/

/* TermId= 35 */
const CKeyFixup BDS_7_FINN[7] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '[' */ 0x5B,   9, MO_AltRight,
	/* '\' */ 0x5C,  12, MO_AltRight,
	/* '{' */ 0x7B,   8, MO_AltRight,
	/* '|' */ 0x7C,  45, MO_AltRight,
	/* '}' */ 0x7D,  11, MO_AltRight,
	/* '€' */ 0x80, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	7 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		28 (0x1C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		21 (0x15) bytes
*/

/* TermId= 35 */
const CKeyFixup BDS_7_FRAN[7] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '[' */ 0x5B,   6, MO_AltRight,
	/* '\' */ 0x5C,   9, MO_AltRight,
	/* '{' */ 0x7B,   5, MO_AltRight,
	/* '|' */ 0x7C,   7, MO_AltRight,
	/* '}' */ 0x7D,  13, MO_AltRight,
	/* '€' */ 0x80, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	7 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		28 (0x1C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		21 (0x15) bytes
*/

/* TermId= 35 */
const CKeyFixup BDS_7_GERM[7] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '[' */ 0x5B,   9, MO_AltRight,
	/* '\' */ 0x5C,  12, MO_AltRight,
	/* '{' */ 0x7B,   8, MO_AltRight,
	/* '|' */ 0x7C,  45, MO_AltRight,
	/* '}' */ 0x7D,  11, MO_AltRight,
	/* '€' */ 0x80, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	7 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		28 (0x1C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		21 (0x15) bytes
*/

/* TermId= 35 */
const CKeyFixup BDS_7_ITAL[7] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,   1, 0,
	/* '{' */ 0x7B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x7C,   1, MO_ShiftLeft,
	/* '}' */ 0x7D,  28, MO_ShiftLeft|MO_AltRight,
	/* '€' */ 0x80, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	7 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		28 (0x1C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		21 (0x15) bytes
*/

/* TermId= 35 */
const CKeyFixup BDS_7_SWIT[7] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,  45, MO_AltRight,
	/* '{' */ 0x7B,  41, MO_AltRight,
	/* '|' */ 0x7C,   2, MO_AltRight,
	/* '}' */ 0x7D,  42, MO_AltRight,
	/* '€' */ 0x80, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	7 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		28 (0x1C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		21 (0x15) bytes
*/

/* TermId= 35 */
const CKeyFixup BDS_7_UKIN[7] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '[' */ 0x5B,  27, 0,
	/* '\' */ 0x5C,  45, 0,
	/* '{' */ 0x7B,  27, MO_ShiftLeft,
	/* '|' */ 0x7C,  45, MO_ShiftLeft,
	/* '}' */ 0x7D,  28, MO_ShiftLeft,
	/* '€' */ 0x80, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	7 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		28 (0x1C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		21 (0x15) bytes
*/

/* TermId= 35 */
const CKeyFixup BDS_7_DENM[7] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '[' */ 0x5B,   9, MO_AltRight,
	/* '\' */ 0x5C,  45, MO_AltRight,
	/* '{' */ 0x7B,   8, MO_AltRight,
	/* '|' */ 0x7C,  13, MO_AltRight,
	/* '}' */ 0x7D,  11, MO_AltRight,
	/* '€' */ 0x80, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	7 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		28 (0x1C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		21 (0x15) bytes
*/

/* TermId= 35 */
const CKeyFixup BDS_7_NORW[7] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '[' */ 0x5B,   9, MO_AltRight,
	/* '\' */ 0x5C,  13, 0,
	/* '{' */ 0x7B,   8, MO_AltRight,
	/* '|' */ 0x7C,   1, 0,
	/* '}' */ 0x7D,  11, MO_AltRight,
	/* '€' */ 0x80, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	7 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		28 (0x1C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		21 (0x15) bytes
*/

/* TermId= 35 */
const CKeyFixup BDS_7_SPAI[7] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,   1, MO_AltRight,
	/* '{' */ 0x7B,  41, MO_AltRight,
	/* '|' */ 0x7C,   2, MO_AltRight,
	/* '}' */ 0x7D,  42, MO_AltRight,
	/* '€' */ 0x80, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	7 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		28 (0x1C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		21 (0x15) bytes
*/
/* BDS_7_DUTC is not supported */
/* BDS_7_HEBR is not supported */
/* BDS_7_PORT is not supported */
/* BDS_7_LATI is not supported */
/* BDS_7_CZEC is not supported */
/* BDS_7_BRAZ is not supported */
/* BDS_7_GREK is not supported */
/* BDS_7_FCAN is not supported */
/* BDS_7_HUNG is not supported */
/* BDS_7_POLI is not supported */
/* BDS_7_SCS_ is not supported */
/* BDS_7_SLOV is not supported */
/* BDS_7_SWED is not supported */
/* BDS_7_TURQ is not supported */
/* BDS_7_ROMA is not supported */
/* BDS_7_RUSS is not supported */
/* BDS_7_TURF is not supported */
/* BDS_7_JAPA is not supported */

/* TermId= 49 	,125 */
const CKeyFixup APPLE_ADB_USA_[15] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '€' */ 0x80,   3, MO_ShiftLeft|MO_AltRight,
	/* 'ƒ' */ 0x83,  34, MO_AltRight,
	/* '™' */ 0x99,   3, MO_AltRight,
	/* '£' */ 0xA3,   4, MO_AltRight,
	/* '¥' */ 0xA5,  22, MO_AltRight,
	/* '§' */ 0xA7,   1, MO_AltRight,
	/* '±' */ 0xB1,   1, MO_ShiftLeft|MO_AltRight,
	/* 'µ' */ 0xB5,  52, MO_AltRight,
	/* '¶' */ 0xB6,   8, MO_AltRight,
	/* 'Å' */ 0xC5,  31, MO_ShiftLeft|MO_AltRight,
	/* 'Ç' */ 0xC7,  48, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0xDF,  32, MO_AltRight,
	/* 'å' */ 0xE5,  31, MO_AltRight,
	/* 'ç' */ 0xE7,  48, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	15 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		60 (0x3C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		45 (0x2D) bytes
*/

/* TermId= 49 	,125 */
const CKeyFixup APPLE_ADB_BELG[76] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '#' */ 0x23,  56, MO_ShiftLeft,
	/* '@' */ 0x40,  56, 0,
	/* '[' */ 0x5B,   6, MO_ShiftLeft|MO_AltRight,
	/* '\' */ 0x5C,  54, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x5D,  12, MO_ShiftLeft|MO_AltRight,
	/* '`' */ 0x60,  42, 0,
	/* '{' */ 0x7B,   6, MO_AltRight,
	/* '|' */ 0x7C,  39, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x7D,  12, MO_AltRight,
	/* '~' */ 0x7E,  51, MO_AltRight,
	/*     */ 0x7F,  15, 0,
	/* '€' */ 0x80,  20, MO_ShiftLeft|MO_AltRight,
	/* '‚' */ 0x82,  28, MO_AltRight,
	/* 'ƒ' */ 0x83,  34, MO_AltRight,
	/* '„' */ 0x84,   3, MO_ShiftLeft|MO_AltRight,
	/* '…' */ 0x85,  53, MO_AltRight,
	/* '†' */ 0x86,  21, MO_AltRight,
	/* '‡' */ 0x87,  31, MO_AltRight,
	/* '‰' */ 0x89,  41, MO_ShiftLeft|MO_AltRight,
	/* 'Œ' */ 0x8C,  25, MO_ShiftLeft|MO_AltRight,
	/* '‘' */ 0x91,   5, MO_AltRight,
	/* '’' */ 0x92,   5, MO_ShiftLeft|MO_AltRight,
	/* '“' */ 0x93,   4, MO_AltRight,
	/* '”' */ 0x94,   4, MO_ShiftLeft|MO_AltRight,
	/* '•' */ 0x95,  34, MO_ShiftLeft|MO_AltRight,
	/* '™' */ 0x99,  21, MO_ShiftLeft|MO_AltRight,
	/* 'œ' */ 0x9C,  25, MO_AltRight,
	/* 'Ÿ' */ 0x9F,  22, MO_ShiftLeft|MO_AltRight,
	/* '¡' */ 0xA1,   9, MO_AltRight,
	/* '¢' */ 0xA2,  48, MO_ShiftLeft|MO_AltRight,
	/* '£' */ 0xA3,  42, MO_ShiftLeft,
	/* '¥' */ 0xA5,  28, MO_ShiftLeft|MO_AltRight,
	/* '§' */ 0xA7,   7, 0,
	/* '©' */ 0xA9,  48, MO_AltRight,
	/* 'ª' */ 0xAA,  23, MO_ShiftLeft|MO_AltRight,
	/* '«' */ 0xAB,   8, MO_AltRight,
	/* '¬' */ 0xAC,  39, MO_AltRight,
	/* '®' */ 0xAE,  20, MO_AltRight,
	/* '±' */ 0xB1,  55, MO_ShiftLeft|MO_AltRight,
	/* '´' */ 0xB4,   2, MO_ShiftLeft|MO_AltRight,
	/* 'µ' */ 0xB5,  40, MO_AltRight,
	/* '¶' */ 0xB6,   7, MO_AltRight,
	/* 'º' */ 0xBA,  23, MO_AltRight,
	/* '»' */ 0xBB,   8, MO_ShiftLeft|MO_AltRight,
	/* '¿' */ 0xBF,  52, MO_ShiftLeft|MO_AltRight,
	/* 'Á' */ 0xC1,  10, MO_ShiftLeft|MO_AltRight,
	/* 'Â' */ 0xC2,  18, MO_AltRight,
	/* 'Å' */ 0xC5,  18, MO_ShiftLeft|MO_AltRight,
	/* 'Æ' */ 0xC6,  17, MO_ShiftLeft|MO_AltRight,
	/* 'Ç' */ 0xC7,  10, MO_AltRight,
	/* 'È' */ 0xC8,  38, MO_AltRight,
	/* 'Ê' */ 0xCA,  19, MO_ShiftLeft|MO_AltRight,
	/* 'Ë' */ 0xCB,  38, MO_ShiftLeft|MO_AltRight,
	/* 'Ì' */ 0xCC,  36, MO_AltRight,
	/* 'Í' */ 0xCD,  37, MO_ShiftLeft|MO_AltRight,
	/* 'Î' */ 0xCE,  36, MO_ShiftLeft|MO_AltRight,
	/* 'Ï' */ 0xCF,  37, MO_AltRight,
	/* 'Ò' */ 0xD2,  32, MO_AltRight,
	/* 'Ó' */ 0xD3,  40, MO_ShiftLeft|MO_AltRight,
	/* 'Ô' */ 0xD4,  27, MO_ShiftLeft|MO_AltRight,
	/* 'Ø' */ 0xD8,  11, MO_ShiftLeft|MO_AltRight,
	/* 'Ù' */ 0xD9,  41, MO_AltRight,
	/* 'Ú' */ 0xDA,  22, MO_AltRight,
	/* 'Û' */ 0xDB,   9, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0xDF,  50, MO_AltRight,
	/* 'å' */ 0xE5,   7, MO_ShiftLeft|MO_AltRight,
	/* 'æ' */ 0xE6,  17, MO_AltRight,
	/* 'ç' */ 0xE7,  10, 0,
	/* 'ê' */ 0xEA,  19, MO_AltRight,
	/* 'ë' */ 0xEB,   3, MO_AltRight,
	/* 'î' */ 0xEE,  24, MO_AltRight,
	/* 'ï' */ 0xEF,  24, MO_ShiftLeft|MO_AltRight,
	/* 'ô' */ 0xF4,  27, MO_AltRight,
	/* '÷' */ 0xF7,  54, MO_AltRight,
	/* 'ø' */ 0xF8,  11, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	76 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		304 (0x130) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		228 (0xE4) bytes
*/

/* TermId= 49 	,125 */
const CKeyFixup APPLE_ADB_FINN[51] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '#' */ 0x23,   4, MO_ShiftLeft,
	/* '$' */ 0x24,   5, MO_AltRight,
	/* '@' */ 0x40,  42, MO_AltRight,
	/* '[' */ 0x5B,   9, MO_AltRight,
	/* '\' */ 0x5C,   8, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x5D,  10, MO_AltRight,
	/* '^' */ 0x5E,  28, MO_ShiftLeft,
	/* '{' */ 0x7B,   9, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x7C,   8, MO_AltRight,
	/* '}' */ 0x7D,  10, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x7E,  28, MO_AltRight,
	/*     */ 0x7F,  15, 0,
	/* '€' */ 0x80,   5, MO_ShiftLeft,
	/* 'ƒ' */ 0x83,  34, MO_AltRight,
	/* '†' */ 0x86,  21, MO_AltRight,
	/* '‡' */ 0x87,  21, MO_ShiftLeft|MO_AltRight,
	/* '‰' */ 0x89,   6, MO_ShiftLeft|MO_AltRight,
	/* 'Œ' */ 0x8C,  25, MO_ShiftLeft|MO_AltRight,
	/* '”' */ 0x94,   3, MO_ShiftLeft|MO_AltRight,
	/* '•' */ 0x95,  56, MO_ShiftLeft|MO_AltRight,
	/* '™' */ 0x99,   3, MO_AltRight,
	/* 'œ' */ 0x9C,  25, MO_AltRight,
	/* '¡' */ 0xA1,   2, MO_ShiftLeft|MO_AltRight,
	/* '£' */ 0xA3,   4, MO_AltRight,
	/* '¥' */ 0xA5, NOS, 0,
	/* '§' */ 0xA7,   7, MO_AltRight,
	/* '¨' */ 0xA8,  28, 0,
	/* '©' */ 0xA9,   2, MO_AltRight,
	/* 'ª' */ 0xAA,  38, MO_AltRight,
	/* '¬' */ 0xAC,  37, MO_AltRight,
	/* '®' */ 0xAE,  20, MO_AltRight,
	/* '°' */ 0xB0,  56, MO_ShiftLeft,
	/* '±' */ 0xB1, NOS, 0,
	/* '´' */ 0xB4,  13, 0,
	/* 'µ' */ 0xB5,  22, MO_AltRight,
	/* '¶' */ 0xB6,  56, MO_AltRight,
	/* 'º' */ 0xBA,  38, MO_ShiftLeft|MO_AltRight,
	/* 'Å' */ 0xC5,  27, MO_ShiftLeft,
	/* 'Æ' */ 0xC6,  41, MO_ShiftLeft|MO_AltRight,
	/* 'Ç' */ 0xC7,  48, MO_ShiftLeft|MO_AltRight,
	/* 'É' */ 0xC9,  19, MO_ShiftLeft|MO_AltRight,
	/* 'Ø' */ 0xD8,  40, MO_ShiftLeft|MO_AltRight,
	/* 'Ü' */ 0xDC,  23, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0xDF,  32, MO_AltRight,
	/* 'å' */ 0xE5,  27, 0,
	/* 'æ' */ 0xE6,  41, MO_AltRight,
	/* 'ç' */ 0xE7,  48, MO_AltRight,
	/* 'é' */ 0xE9,  19, MO_AltRight,
	/* 'ø' */ 0xF8,  40, MO_AltRight,
	/* 'ü' */ 0xFC,  23, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	51 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		204 (0xCC) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		153 (0x99) bytes
*/

/* TermId= 49 	,125 */
const CKeyFixup APPLE_ADB_FRAN[76] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '#' */ 0x23,  56, MO_ShiftLeft,
	/* '@' */ 0x40,  56, 0,
	/* '[' */ 0x5B,   6, MO_ShiftLeft|MO_AltRight,
	/* '\' */ 0x5C,  54, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x5D,  12, MO_ShiftLeft|MO_AltRight,
	/* '`' */ 0x60,  42, 0,
	/* '{' */ 0x7B,   6, MO_AltRight,
	/* '|' */ 0x7C,  39, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x7D,  12, MO_AltRight,
	/* '~' */ 0x7E,  51, MO_AltRight,
	/*     */ 0x7F,  15, 0,
	/* '€' */ 0x80,  20, MO_ShiftLeft|MO_AltRight,
	/* '‚' */ 0x82,  28, MO_AltRight,
	/* 'ƒ' */ 0x83,  34, MO_AltRight,
	/* '„' */ 0x84,   3, MO_ShiftLeft|MO_AltRight,
	/* '…' */ 0x85,  53, MO_AltRight,
	/* '†' */ 0x86,  21, MO_AltRight,
	/* '‡' */ 0x87,  31, MO_AltRight,
	/* '‰' */ 0x89,  41, MO_ShiftLeft|MO_AltRight,
	/* 'Œ' */ 0x8C,  25, MO_ShiftLeft|MO_AltRight,
	/* '‘' */ 0x91,   5, MO_AltRight,
	/* '’' */ 0x92,   5, MO_ShiftLeft|MO_AltRight,
	/* '“' */ 0x93,   4, MO_AltRight,
	/* '”' */ 0x94,   4, MO_ShiftLeft|MO_AltRight,
	/* '•' */ 0x95,  34, MO_ShiftLeft|MO_AltRight,
	/* '™' */ 0x99,  21, MO_ShiftLeft|MO_AltRight,
	/* 'œ' */ 0x9C,  25, MO_AltRight,
	/* 'Ÿ' */ 0x9F,  22, MO_ShiftLeft|MO_AltRight,
	/* '¡' */ 0xA1,   9, MO_AltRight,
	/* '¢' */ 0xA2,  48, MO_ShiftLeft|MO_AltRight,
	/* '£' */ 0xA3,  42, MO_ShiftLeft,
	/* '¥' */ 0xA5,  28, MO_ShiftLeft|MO_AltRight,
	/* '§' */ 0xA7,   7, 0,
	/* '©' */ 0xA9,  48, MO_AltRight,
	/* 'ª' */ 0xAA,  23, MO_ShiftLeft|MO_AltRight,
	/* '«' */ 0xAB,   8, MO_AltRight,
	/* '¬' */ 0xAC,  39, MO_AltRight,
	/* '®' */ 0xAE,  20, MO_AltRight,
	/* '±' */ 0xB1,  55, MO_ShiftLeft|MO_AltRight,
	/* '´' */ 0xB4,   2, MO_ShiftLeft|MO_AltRight,
	/* 'µ' */ 0xB5,  40, MO_AltRight,
	/* '¶' */ 0xB6,   7, MO_AltRight,
	/* 'º' */ 0xBA,  23, MO_AltRight,
	/* '»' */ 0xBB,   8, MO_ShiftLeft|MO_AltRight,
	/* '¿' */ 0xBF,  52, MO_ShiftLeft|MO_AltRight,
	/* 'Á' */ 0xC1,  10, MO_ShiftLeft|MO_AltRight,
	/* 'Â' */ 0xC2,  18, MO_AltRight,
	/* 'Å' */ 0xC5,  18, MO_ShiftLeft|MO_AltRight,
	/* 'Æ' */ 0xC6,  17, MO_ShiftLeft|MO_AltRight,
	/* 'Ç' */ 0xC7,  10, MO_AltRight,
	/* 'È' */ 0xC8,  38, MO_AltRight,
	/* 'Ê' */ 0xCA,  19, MO_ShiftLeft|MO_AltRight,
	/* 'Ë' */ 0xCB,  38, MO_ShiftLeft|MO_AltRight,
	/* 'Ì' */ 0xCC,  36, MO_AltRight,
	/* 'Í' */ 0xCD,  37, MO_ShiftLeft|MO_AltRight,
	/* 'Î' */ 0xCE,  36, MO_ShiftLeft|MO_AltRight,
	/* 'Ï' */ 0xCF,  37, MO_AltRight,
	/* 'Ò' */ 0xD2,  32, MO_AltRight,
	/* 'Ó' */ 0xD3,  40, MO_ShiftLeft|MO_AltRight,
	/* 'Ô' */ 0xD4,  27, MO_ShiftLeft|MO_AltRight,
	/* 'Ø' */ 0xD8,  11, MO_ShiftLeft|MO_AltRight,
	/* 'Ù' */ 0xD9,  41, MO_AltRight,
	/* 'Ú' */ 0xDA,  22, MO_AltRight,
	/* 'Û' */ 0xDB,   9, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0xDF,  50, MO_AltRight,
	/* 'å' */ 0xE5,   7, MO_ShiftLeft|MO_AltRight,
	/* 'æ' */ 0xE6,  17, MO_AltRight,
	/* 'ç' */ 0xE7,  10, 0,
	/* 'ê' */ 0xEA,  19, MO_AltRight,
	/* 'ë' */ 0xEB,   3, MO_AltRight,
	/* 'î' */ 0xEE,  24, MO_AltRight,
	/* 'ï' */ 0xEF,  24, MO_ShiftLeft|MO_AltRight,
	/* 'ô' */ 0xF4,  27, MO_AltRight,
	/* '÷' */ 0xF7,  54, MO_AltRight,
	/* 'ø' */ 0xF8,  11, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	76 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		304 (0x130) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		228 (0xE4) bytes
*/

/* TermId= 49 	,125 */
const CKeyFixup APPLE_ADB_GERM[70] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '@' */ 0x40,   2, MO_ShiftLeft|MO_AltRight,
	/* '[' */ 0x5B,   6, MO_AltRight,
	/* '\' */ 0x5C,   8, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x5D,   7, MO_AltRight,
	/* '^' */ 0x5E,  56, 0,
	/* '{' */ 0x7B,   9, MO_AltRight,
	/* '|' */ 0x7C,   8, MO_AltRight,
	/* '}' */ 0x7D,  10, MO_AltRight,
	/* '~' */ 0x7E,  51, MO_AltRight,
	/*     */ 0x7F,  15, 0,
	/* '€' */ 0x80,  33, MO_ShiftLeft|MO_AltRight,
	/* '‚' */ 0x82,  32, MO_AltRight,
	/* 'ƒ' */ 0x83,  34, MO_AltRight,
	/* '„' */ 0x84,  56, MO_AltRight,
	/* '…' */ 0x85,  54, MO_AltRight,
	/* '†' */ 0x86,  21, MO_AltRight,
	/* '‡' */ 0x87,  46, MO_ShiftLeft|MO_AltRight,
	/* '‰' */ 0x89,  19, MO_ShiftLeft|MO_AltRight,
	/* 'Œ' */ 0x8C,  40, MO_ShiftLeft|MO_AltRight,
	/* '‘' */ 0x91,  42, MO_AltRight,
	/* '’' */ 0x92,  42, MO_ShiftLeft|MO_AltRight,
	/* '“' */ 0x93,   3, MO_AltRight,
	/* '”' */ 0x94,   3, MO_ShiftLeft|MO_AltRight,
	/* '™' */ 0x99,  19, MO_AltRight,
	/* 'œ' */ 0x9C,  40, MO_AltRight,
	/* '¡' */ 0xA1,   2, MO_AltRight,
	/* '¢' */ 0xA2,   5, MO_AltRight,
	/* '£' */ 0xA3,   5, MO_ShiftLeft|MO_AltRight,
	/* '¤' */ 0xA4, NOS, 0,
	/* '¥' */ 0xA5,  46, MO_AltRight,
	/* '§' */ 0xA7,   4, MO_ShiftLeft,
	/* '¨' */ 0xA8,  23, MO_AltRight,
	/* '©' */ 0xA9,  35, MO_AltRight,
	/* 'ª' */ 0xAA,  36, MO_AltRight,
	/* '«' */ 0xAB,  17, MO_AltRight,
	/* '¬' */ 0xAC,  39, MO_AltRight,
	/* '®' */ 0xAE,  20, MO_AltRight,
	/* '°' */ 0xB0,  56, MO_ShiftLeft,
	/* '±' */ 0xB1,  28, MO_AltRight,
	/* '²' */ 0xB2, NOS, 0,
	/* '³' */ 0xB3, NOS, 0,
	/* 'µ' */ 0xB5,  52, MO_AltRight,
	/* '¶' */ 0xB6,   4, MO_AltRight,
	/* '·' */ 0xB7,  10, MO_ShiftLeft|MO_AltRight,
	/* '¸' */ 0xB8, NOS, 0,
	/* '¹' */ 0xB9, NOS, 0,
	/* 'º' */ 0xBA,  37, MO_AltRight,
	/* '»' */ 0xBB,  17, MO_ShiftLeft|MO_AltRight,
	/* '¼' */ 0xBC, NOS, 0,
	/* '½' */ 0xBD, NOS, 0,
	/* '¾' */ 0xBE, NOS, 0,
	/* '¿' */ 0xBF,  12, MO_AltRight,
	/* 'Á' */ 0xC1,  23, MO_ShiftLeft|MO_AltRight,
	/* 'Å' */ 0xC5,  31, MO_ShiftLeft|MO_AltRight,
	/* 'Æ' */ 0xC6,  41, MO_ShiftLeft|MO_AltRight,
	/* 'Ç' */ 0xC7,  48, MO_ShiftLeft|MO_AltRight,
	/* 'Ì' */ 0xCC,  35, MO_ShiftLeft|MO_AltRight,
	/* 'Í' */ 0xCD,  32, MO_ShiftLeft|MO_AltRight,
	/* 'Ï' */ 0xCF,  34, MO_ShiftLeft|MO_AltRight,
	/* 'Ó' */ 0xD3,  36, MO_ShiftLeft|MO_AltRight,
	/* 'Ø' */ 0xD8,  25, MO_ShiftLeft|MO_AltRight,
	/* 'Ù' */ 0xD9,  47, MO_ShiftLeft|MO_AltRight,
	/* 'Û' */ 0xDB,  24, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0xDF,  12, 0,
	/* 'å' */ 0xE5,  31, MO_AltRight,
	/* 'æ' */ 0xE6,  41, MO_AltRight,
	/* 'ç' */ 0xE7,  48, MO_AltRight,
	/* '÷' */ 0xF7,  54, MO_ShiftLeft|MO_AltRight,
	/* 'ø' */ 0xF8,  25, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	70 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		280 (0x118) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		210 (0xD2) bytes
*/

/* TermId= 49 	,125 */
const CKeyFixup APPLE_ADB_ITAL[80] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '"' */ 0x22,   3, 0,
	/* '#' */ 0x23,  56, MO_ShiftLeft,
	/* '&' */ 0x26,   2, 0,
	/* ''' */ 0x27,   4, 0,
	/* '(' */ 0x28,   5, 0,
	/* ')' */ 0x29,   8, 0,
	/* '*' */ 0x2A,  28, MO_ShiftLeft,
	/* '-' */ 0x2D,  12, 0,
	/* '=' */ 0x3D,  13, 0,
	/* '@' */ 0x40,  56, 0,
	/* '[' */ 0x5B,   5, MO_ShiftLeft|MO_AltRight,
	/* '\' */ 0x5C,  54, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x5D,   8, MO_ShiftLeft|MO_AltRight,
	/* '^' */ 0x5E,  27, MO_ShiftLeft,
	/* '_' */ 0x5F,  12, MO_ShiftLeft,
	/* '`' */ 0x60,  10, MO_AltRight,
	/* '{' */ 0x7B,   5, MO_AltRight,
	/* '|' */ 0x7C,  54, MO_AltRight,
	/* '}' */ 0x7D,   8, MO_AltRight,
	/* '~' */ 0x7E,  51, MO_AltRight,
	/*     */ 0x7F,  15, 0,
	/* '€' */ 0x80,  24, MO_AltRight,
	/* 'ƒ' */ 0x83,  34, MO_AltRight,
	/* '„' */ 0x84,  46, MO_ShiftLeft|MO_AltRight,
	/* '…' */ 0x85,  53, MO_AltRight,
	/* '†' */ 0x86,  21, MO_AltRight,
	/* '‡' */ 0x87,  34, MO_ShiftLeft|MO_AltRight,
	/* '‰' */ 0x89,  47, MO_ShiftLeft|MO_AltRight,
	/* 'Œ' */ 0x8C,  27, MO_ShiftLeft|MO_AltRight,
	/* '‘' */ 0x91,   4, MO_AltRight,
	/* '’' */ 0x92,   4, MO_ShiftLeft|MO_AltRight,
	/* '“' */ 0x93,   3, MO_AltRight,
	/* '”' */ 0x94,   3, MO_ShiftLeft|MO_AltRight,
	/* '™' */ 0x99,  19, MO_AltRight,
	/* 'œ' */ 0x9C,  27, MO_AltRight,
	/* 'Ÿ' */ 0x9F,  35, MO_AltRight,
	/* '¡' */ 0xA1,  55, MO_AltRight,
	/* '¢' */ 0xA2,  28, MO_AltRight,
	/* '£' */ 0xA3,   9, 0,
	/* '¥' */ 0xA5,  22, MO_AltRight,
	/* '§' */ 0xA7,  42, 0,
	/* '¨' */ 0xA8,  23, MO_AltRight,
	/* '©' */ 0xA9,  48, MO_AltRight,
	/* '¬' */ 0xAC,  39, MO_AltRight,
	/* '®' */ 0xAE,  20, MO_AltRight,
	/* '°' */ 0xB0,  42, MO_ShiftLeft,
	/* '±' */ 0xB1,  13, MO_ShiftLeft|MO_AltRight,
	/* '´' */ 0xB4,   2, MO_ShiftLeft|MO_AltRight,
	/* 'µ' */ 0xB5,  40, MO_AltRight,
	/* '¶' */ 0xB6,   7, MO_AltRight,
	/* '·' */ 0xB7,   7, MO_ShiftLeft|MO_AltRight,
	/* '¿' */ 0xBF,  52, MO_AltRight,
	/* 'Á' */ 0xC1,  17, MO_ShiftLeft|MO_AltRight,
	/* 'Â' */ 0xC2,  48, MO_ShiftLeft|MO_AltRight,
	/* 'Å' */ 0xC5,  31, MO_ShiftLeft|MO_AltRight,
	/* 'Æ' */ 0xC6,  41, MO_ShiftLeft|MO_AltRight,
	/* 'Ç' */ 0xC7,   6, MO_AltRight,
	/* 'È' */ 0xC8,  19, MO_ShiftLeft|MO_AltRight,
	/* 'Ê' */ 0xCA,  50, MO_ShiftLeft|MO_AltRight,
	/* 'Ë' */ 0xCB,  18, MO_ShiftLeft|MO_AltRight,
	/* 'Ì' */ 0xCC,   6, MO_ShiftLeft|MO_AltRight,
	/* 'Í' */ 0xCD,  20, MO_ShiftLeft|MO_AltRight,
	/* 'Î' */ 0xCE,  22, MO_ShiftLeft|MO_AltRight,
	/* 'Ï' */ 0xCF,  21, MO_ShiftLeft|MO_AltRight,
	/* 'Ò' */ 0xD2,  11, MO_ShiftLeft|MO_AltRight,
	/* 'Ô' */ 0xD4,  10, MO_ShiftLeft|MO_AltRight,
	/* 'Ø' */ 0xD8,  25, MO_ShiftLeft|MO_AltRight,
	/* 'Ù' */ 0xD9,  24, MO_ShiftLeft|MO_AltRight,
	/* 'Ú' */ 0xDA,  28, MO_ShiftLeft|MO_AltRight,
	/* 'Û' */ 0xDB,  23, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0xDF,  32, MO_AltRight,
	/* 'à' */ 0xE0,  10, 0,
	/* 'å' */ 0xE5,  31, MO_AltRight,
	/* 'æ' */ 0xE6,  41, MO_AltRight,
	/* 'ç' */ 0xE7,   6, 0,
	/* 'è' */ 0xE8,   7, 0,
	/* 'é' */ 0xE9,  11, 0,
	/* '÷' */ 0xF7,  53, MO_ShiftLeft|MO_AltRight,
	/* 'ø' */ 0xF8,  25, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	80 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		320 (0x140) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		240 (0xF0) bytes
*/

/* TermId= 49 	,125 */
const CKeyFixup APPLE_ADB_SWIT[63] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,  28, MO_ShiftLeft,
	/* '#' */ 0x23,   4, MO_AltRight,
	/* '@' */ 0x40,  35, MO_AltRight,
	/* '[' */ 0x5B,   6, MO_AltRight,
	/* '\' */ 0x5C,   8, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x5D,   7, MO_AltRight,
	/* '{' */ 0x7B,   9, MO_AltRight,
	/* '|' */ 0x7C,   8, MO_AltRight,
	/* '}' */ 0x7D,  10, MO_AltRight,
	/* '~' */ 0x7E,  51, MO_AltRight,
	/*     */ 0x7F,  15, 0,
	/* '€' */ 0x80,  47, MO_ShiftLeft|MO_AltRight,
	/* 'ƒ' */ 0x83,  34, MO_AltRight,
	/* '…' */ 0x85,  54, MO_AltRight,
	/* '†' */ 0x86,  21, MO_AltRight,
	/* '‡' */ 0x87,  34, MO_ShiftLeft|MO_AltRight,
	/* '‰' */ 0x89,  56, MO_ShiftLeft|MO_AltRight,
	/* 'Œ' */ 0x8C,  17, MO_ShiftLeft|MO_AltRight,
	/* '‘' */ 0x91,  28, MO_AltRight,
	/* '’' */ 0x92,  28, MO_ShiftLeft|MO_AltRight,
	/* '“' */ 0x93,   3, MO_AltRight,
	/* '”' */ 0x94,   3, MO_ShiftLeft|MO_AltRight,
	/* '•' */ 0x95,  42, MO_ShiftLeft|MO_AltRight,
	/* '™' */ 0x99,  19, MO_AltRight,
	/* 'œ' */ 0x9C,  17, MO_AltRight,
	/* 'Ÿ' */ 0x9F,  46, MO_ShiftLeft|MO_AltRight,
	/* '¡' */ 0xA1,  24, MO_AltRight,
	/* '¢' */ 0xA2,  40, MO_AltRight,
	/* '£' */ 0xA3,  42, MO_ShiftLeft,
	/* '¥' */ 0xA5,  46, MO_AltRight,
	/* '§' */ 0xA7,  56, 0,
	/* 'ª' */ 0xAA,  36, MO_AltRight,
	/* '«' */ 0xAB,  53, MO_AltRight,
	/* '¬' */ 0xAC,  39, MO_AltRight,
	/* '®' */ 0xAE,  20, MO_AltRight,
	/* '°' */ 0xB0,  56, MO_ShiftLeft,
	/* '±' */ 0xB1,   2, MO_AltRight,
	/* '´' */ 0xB4,  13, MO_AltRight,
	/* 'µ' */ 0xB5,  52, MO_AltRight,
	/* '¶' */ 0xB6,  42, MO_AltRight,
	/* 'º' */ 0xBA,  37, MO_AltRight,
	/* '»' */ 0xBB,  53, MO_ShiftLeft|MO_AltRight,
	/* '¿' */ 0xBF,  12, MO_AltRight,
	/* 'Á' */ 0xC1,  18, MO_ShiftLeft|MO_AltRight,
	/* 'Å' */ 0xC5,  31, MO_ShiftLeft|MO_AltRight,
	/* 'Æ' */ 0xC6,  41, MO_ShiftLeft|MO_AltRight,
	/* 'Ç' */ 0xC7,   5, MO_AltRight,
	/* 'È' */ 0xC8,  20, MO_ShiftLeft|MO_AltRight,
	/* 'Ë' */ 0xCB,  19, MO_ShiftLeft|MO_AltRight,
	/* 'Í' */ 0xCD,  22, MO_ShiftLeft|MO_AltRight,
	/* 'Î' */ 0xCE,  21, MO_ShiftLeft|MO_AltRight,
	/* 'Ò' */ 0xD2,   9, MO_ShiftLeft|MO_AltRight,
	/* 'Ô' */ 0xD4,  10, MO_ShiftLeft|MO_AltRight,
	/* 'Ø' */ 0xD8,  25, MO_ShiftLeft|MO_AltRight,
	/* 'Ú' */ 0xDA,  11, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0xDF,  32, MO_AltRight,
	/* 'å' */ 0xE5,  31, MO_AltRight,
	/* 'æ' */ 0xE6,  41, MO_AltRight,
	/* 'ç' */ 0xE7,   5, MO_ShiftLeft,
	/* '÷' */ 0xF7,  54, MO_ShiftLeft|MO_AltRight,
	/* 'ø' */ 0xF8,  25, MO_AltRight,
	/* 'ÿ' */ 0xFF,  27, MO_ShiftLeft|MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	63 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		252 (0xFC) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		189 (0xBD) bytes
*/

/* TermId= 49 	,125 */
const CKeyFixup APPLE_ADB_UKIN[23] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '#' */ 0x23,   4, MO_AltRight,
	/* '<' */ 0x3C,  53, MO_ShiftLeft,
	/* '>' */ 0x3E,  54, MO_ShiftLeft,
	/* '\' */ 0x5C,  42, 0,
	/* '`' */ 0x60,  45, 0,
	/* '|' */ 0x7C,  42, MO_ShiftLeft,
	/* '~' */ 0x7E,  45, MO_ShiftLeft,
	/*     */ 0x7F,  15, 0,
	/* '€' */ 0x80,   3, MO_ShiftLeft|MO_AltRight,
	/* 'ƒ' */ 0x83,  34, MO_AltRight,
	/* '™' */ 0x99,   3, MO_AltRight,
	/* '£' */ 0xA3,   4, MO_ShiftLeft,
	/* '¥' */ 0xA5,  22, MO_AltRight,
	/* '§' */ 0xA7,  56, 0,
	/* '±' */ 0xB1,  56, MO_ShiftLeft,
	/* 'µ' */ 0xB5,  52, MO_AltRight,
	/* '¶' */ 0xB6,   8, MO_AltRight,
	/* 'Å' */ 0xC5,  31, MO_ShiftLeft|MO_AltRight,
	/* 'Ç' */ 0xC7,  48, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0xDF,  32, MO_AltRight,
	/* 'å' */ 0xE5,  31, MO_AltRight,
	/* 'ç' */ 0xE7,  48, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	23 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		92 (0x5C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		69 (0x45) bytes
*/

/* TermId= 49 	,125 */
const CKeyFixup APPLE_ADB_DENM[34] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '#' */ 0x23,   4, MO_ShiftLeft,
	/* '$' */ 0x24,  56, 0,
	/* '@' */ 0x40,  42, MO_AltRight,
	/* '[' */ 0x5B,   9, MO_AltRight,
	/* '\' */ 0x5C,   8, MO_AltRight,
	/* ']' */ 0x5D,  10, MO_AltRight,
	/* '{' */ 0x7B,   9, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x7C,  24, MO_AltRight,
	/* '}' */ 0x7D,  10, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x7E,  28, MO_AltRight,
	/*     */ 0x7F,  15, 0,
	/* '€' */ 0x80,   5, MO_ShiftLeft,
	/* 'ƒ' */ 0x83, NOS, 0,
	/* '™' */ 0x99, NOS, 0,
	/* '£' */ 0xA3,   5, MO_AltRight,
	/* '¥' */ 0xA5, NOS, 0,
	/* '§' */ 0xA7,  56, MO_ShiftLeft,
	/* '±' */ 0xB1, NOS, 0,
	/* 'µ' */ 0xB5, NOS, 0,
	/* '¶' */ 0xB6, NOS, 0,
	/* 'Ä' */ 0xC4,  40, MO_ShiftLeft|MO_AltRight,
	/* 'Å' */ 0xC5,  27, MO_ShiftLeft,
	/* 'Ç' */ 0xC7, NOS, 0,
	/* 'É' */ 0xC9,  19, MO_ShiftLeft|MO_AltRight,
	/* 'Ö' */ 0xD6,  41, MO_ShiftLeft|MO_AltRight,
	/* 'Ü' */ 0xDC,  23, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0xDF, NOS, 0,
	/* 'ä' */ 0xE4,  40, MO_AltRight,
	/* 'å' */ 0xE5,  27, 0,
	/* 'ç' */ 0xE7, NOS, 0,
	/* 'é' */ 0xE9,  19, MO_AltRight,
	/* 'ö' */ 0xF6,  41, MO_AltRight,
	/* 'ü' */ 0xFC,  23, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	34 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		136 (0x88) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		102 (0x66) bytes
*/

/* TermId= 49 	,125 */
const CKeyFixup APPLE_ADB_NORW[31] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* ''' */ 0x27,  56, 0,
	/* '@' */ 0x40,  42, 0,
	/* '[' */ 0x5B,   9, MO_AltRight,
	/* '\' */ 0x5C,   8, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x5D,  10, MO_AltRight,
	/* '{' */ 0x7B,   9, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x7C,   8, MO_AltRight,
	/* '}' */ 0x7D,  10, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x7E,  28, MO_AltRight,
	/*     */ 0x7F,  15, 0,
	/* '€' */ 0x80,  56, MO_AltRight,
	/* 'ƒ' */ 0x83, NOS, 0,
	/* '™' */ 0x99, NOS, 0,
	/* '£' */ 0xA3,   4, MO_AltRight,
	/* '¥' */ 0xA5, NOS, 0,
	/* '§' */ 0xA7,  56, MO_ShiftLeft,
	/* '±' */ 0xB1, NOS, 0,
	/* 'µ' */ 0xB5, NOS, 0,
	/* '¶' */ 0xB6, NOS, 0,
	/* 'Ä' */ 0xC4,  41, MO_ShiftLeft|MO_AltRight,
	/* 'Å' */ 0xC5,  27, MO_ShiftLeft,
	/* 'Ç' */ 0xC7, NOS, 0,
	/* 'Ö' */ 0xD6,  40, MO_ShiftLeft|MO_AltRight,
	/* 'Ü' */ 0xDC,  23, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0xDF, NOS, 0,
	/* 'ä' */ 0xE4,  41, MO_AltRight,
	/* 'å' */ 0xE5,  27, 0,
	/* 'ç' */ 0xE7, NOS, 0,
	/* 'ö' */ 0xF6,  40, MO_AltRight,
	/* 'ü' */ 0xFC,  23, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	31 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		124 (0x7C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		93 (0x5D) bytes
*/

/* TermId= 49 	,125 */
const CKeyFixup APPLE_ADB_SPAI[34] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '#' */ 0x23,   4, MO_AltRight,
	/* '@' */ 0x40,   3, MO_AltRight,
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,  56, MO_AltRight,
	/* ']' */ 0x5D,  28, MO_AltRight,
	/* '{' */ 0x7B,  41, MO_AltRight,
	/* '|' */ 0x7C,   2, MO_AltRight,
	/* '}' */ 0x7D,  42, MO_AltRight,
	/* '~' */ 0x7E,  40, MO_AltRight,
	/*     */ 0x7F,  15, 0,
	/* '€' */ 0x80,  48, MO_ShiftLeft|MO_AltRight,
	/* 'ƒ' */ 0x83, NOS, 0,
	/* '™' */ 0x99, NOS, 0,
	/* '¡' */ 0xA1,  13, 0,
	/* '¢' */ 0xA2,   5, MO_AltRight,
	/* '£' */ 0xA3,   5, MO_ShiftLeft|MO_AltRight,
	/* '¥' */ 0xA5, NOS, 0,
	/* '§' */ 0xA7,  38, MO_AltRight,
	/* '¨' */ 0xA8,  41, MO_ShiftLeft,
	/* '©' */ 0xA9,  48, MO_AltRight,
	/* 'ª' */ 0xAA,  56, MO_ShiftLeft,
	/* '¬' */ 0xAC,   7, MO_AltRight,
	/* '°' */ 0xB0,  56, MO_ShiftLeft|MO_AltRight,
	/* '±' */ 0xB1, NOS, 0,
	/* 'µ' */ 0xB5, NOS, 0,
	/* '¶' */ 0xB6, NOS, 0,
	/* '·' */ 0xB7,   4, MO_ShiftLeft,
	/* '¿' */ 0xBF,  13, MO_ShiftLeft,
	/* 'Å' */ 0xC5, NOS, 0,
	/* 'Ç' */ 0xC7,  42, MO_ShiftLeft,
	/* 'ß' */ 0xDF, NOS, 0,
	/* 'å' */ 0xE5, NOS, 0,
	/* 'ç' */ 0xE7,  42, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	34 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		136 (0x88) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		102 (0x66) bytes
*/
/* APPLE_ADB_DUTC is not supported */
/* APPLE_ADB_HEBR is not supported */
/* APPLE_ADB_PORT is not supported */

/* TermId= 49 	,125 */
const CKeyFixup APPLE_ADB_LATI[20] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '\' */ 0x5C,  12, MO_AltRight,
	/* '|' */ 0x7C,  56, 0,
	/*     */ 0x7F,  15, 0,
	/* '€' */ 0x80, NOS, 0,
	/* 'ƒ' */ 0x83, NOS, 0,
	/* '™' */ 0x99, NOS, 0,
	/* '£' */ 0xA3, NOS, 0,
	/* '¥' */ 0xA5, NOS, 0,
	/* '§' */ 0xA7, NOS, 0,
	/* '¬' */ 0xAC,  56, MO_AltRight,
	/* '°' */ 0xB0,  56, MO_ShiftLeft,
	/* '±' */ 0xB1, NOS, 0,
	/* 'µ' */ 0xB5, NOS, 0,
	/* '¶' */ 0xB6, NOS, 0,
	/* 'Å' */ 0xC5, NOS, 0,
	/* 'Ç' */ 0xC7, NOS, 0,
	/* 'ß' */ 0xDF, NOS, 0,
	/* 'å' */ 0xE5, NOS, 0,
	/* 'ç' */ 0xE7, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	20 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		80 (0x50) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		60 (0x3C) bytes
*/
/* APPLE_ADB_CZEC is not supported */
/* APPLE_ADB_BRAZ is not supported */
/* APPLE_ADB_GREK is not supported */

/* TermId= 49 	,125 */
const CKeyFixup APPLE_ADB_FCAN[44] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '#' */ 0x23,  56, 0,
	/* ''' */ 0x27,  28, 0,
	/* '<' */ 0x3C,  42, 0,
	/* '>' */ 0x3E,  42, MO_ShiftLeft,
	/* '@' */ 0x40,   3, MO_AltRight,
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,  56, MO_AltRight,
	/* ']' */ 0x5D,  28, MO_AltRight,
	/* '^' */ 0x5E,  27, 0,
	/* '`' */ 0x60,  41, MO_ShiftLeft,
	/* '{' */ 0x7B,  41, MO_AltRight,
	/* '|' */ 0x7C,   8, MO_AltRight,
	/* '}' */ 0x7D,  42, MO_AltRight,
	/* '~' */ 0x7E,  40, MO_AltRight,
	/*     */ 0x7F,  15, 0,
	/* '€' */ 0x80, NOS, 0,
	/* 'ƒ' */ 0x83, NOS, 0,
	/* '•' */ 0x95,  45, MO_AltRight,
	/* '™' */ 0x99, NOS, 0,
	/* '¢' */ 0xA2,   5, MO_AltRight,
	/* '£' */ 0xA3,   4, MO_AltRight,
	/* '¤' */ 0xA4,   6, MO_AltRight,
	/* '¥' */ 0xA5, NOS, 0,
	/* '§' */ 0xA7,  25, MO_AltRight,
	/* '¨' */ 0xA8,  28, MO_ShiftLeft,
	/* 'ª' */ 0xAA,   9, MO_AltRight,
	/* '«' */ 0xAB,  45, 0,
	/* '¬' */ 0xAC,   7, MO_AltRight,
	/* '°' */ 0xB0, NOS, 0,
	/* '±' */ 0xB1,   2, MO_AltRight,
	/* '³' */ 0xB3,  10, MO_AltRight,
	/* '´' */ 0xB4,  55, MO_AltRight,
	/* 'µ' */ 0xB5,  52, MO_AltRight,
	/* '¶' */ 0xB6,  26, MO_AltRight,
	/* '»' */ 0xBB,  45, MO_ShiftLeft,
	/* '¼' */ 0xBC,  11, MO_AltRight,
	/* '½' */ 0xBD,  12, MO_AltRight,
	/* '¾' */ 0xBE,  13, MO_AltRight,
	/* 'Å' */ 0xC5, NOS, 0,
	/* 'Ç' */ 0xC7, NOS, 0,
	/* 'ß' */ 0xDF, NOS, 0,
	/* 'å' */ 0xE5, NOS, 0,
	/* 'ç' */ 0xE7, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	44 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		176 (0xB0) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		132 (0x84) bytes
*/
/* APPLE_ADB_HUNG is not supported */
/* APPLE_ADB_POLI is not supported */
/* APPLE_ADB_SCS_ is not supported */
/* APPLE_ADB_SLOV is not supported */
/* APPLE_ADB_SWED is not supported */
/* APPLE_ADB_TURQ is not supported */
/* APPLE_ADB_ROMA is not supported */
/* APPLE_ADB_RUSS is not supported */
/* APPLE_ADB_TURF is not supported */
/* APPLE_ADB_JAPA is not supported */
#define DEC_VT510_LK411_USA_		NoExceptions	/* not required */

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_BELG[59] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,   9, 0,
	/* '"' */ 0x22,   4, 0,
	/* '#' */ 0x23,  42, 0,
	/* '$' */ 0x24,  28, 0,
	/* '%' */ 0x25,  41, MO_ShiftLeft,
	/* '&' */ 0x26,   2, 0,
	/* ''' */ 0x27,   5, 0,
	/* '(' */ 0x28,   6, 0,
	/* ')' */ 0x29,  12, 0,
	/* '*' */ 0x2A,  28, MO_ShiftLeft,
	/* '+' */ 0x2B,  55, MO_ShiftLeft,
	/* ',' */ 0x2C,  52, 0,
	/* '-' */ 0x2D,  13, 0,
	/* '.' */ 0x2E,  53, MO_ShiftLeft,
	/* '/' */ 0x2F,  54, MO_ShiftLeft,
	/* '0' */ 0x30,  11, MO_ShiftLeft,
	/* '1' */ 0x31,   2, MO_ShiftLeft,
	/* '2' */ 0x32,   3, MO_ShiftLeft,
	/* '3' */ 0x33,   4, MO_ShiftLeft,
	/* '4' */ 0x34,   5, MO_ShiftLeft,
	/* '5' */ 0x35,   6, MO_ShiftLeft,
	/* '6' */ 0x36,   7, MO_ShiftLeft,
	/* '7' */ 0x37,   8, MO_ShiftLeft,
	/* '8' */ 0x38,   9, MO_ShiftLeft,
	/* '9' */ 0x39,  10, MO_ShiftLeft,
	/* ':' */ 0x3A,  54, 0,
	/* ';' */ 0x3B,  53, 0,
	/* '<' */ 0x3C,  45, 0,
	/* '=' */ 0x3D,  55, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '?' */ 0x3F,  52, MO_ShiftLeft,
	/* '@' */ 0x40,  42, MO_ShiftLeft,
	/* 'A' */ 0x41,  17, MO_ShiftLeft,
	/* 'M' */ 0x4D,  40, MO_ShiftLeft,
	/* 'Q' */ 0x51,  31, MO_ShiftLeft,
	/* 'W' */ 0x57,  46, MO_ShiftLeft,
	/* 'Z' */ 0x5A,  18, MO_ShiftLeft,
	/* '[' */ 0x5B,   7, MO_AltRight,
	/* '\' */ 0x5C,  41, MO_AltRight,
	/* ']' */ 0x5D,   8, MO_AltRight,
	/* '^' */ 0x5E,  27, 0,
	/* '_' */ 0x5F,  13, MO_ShiftLeft,
	/* 'a' */ 0x61,  17, 0,
	/* 'm' */ 0x6D,  40, 0,
	/* 'q' */ 0x71,  31, 0,
	/* 'w' */ 0x77,  46, 0,
	/* 'z' */ 0x7A,  18, 0,
	/* '{' */ 0x7B,  10, MO_AltRight,
	/* '|' */ 0x7C,   3, MO_AltRight,
	/* '}' */ 0x7D,  11, MO_AltRight,
	/* '§' */ 0xA7,   7, 0,
	/* '¨' */ 0xA8,  27, MO_ShiftLeft,
	/* '°' */ 0xB0,  12, MO_ShiftLeft,
	/* 'à' */ 0xE0,  11, 0,
	/* 'ç' */ 0xE7,  10, 0,
	/* 'è' */ 0xE8,   8, 0,
	/* 'é' */ 0xE9,   3, 0,
	/* 'ù' */ 0xF9,  41, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	59 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		236 (0xEC) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		177 (0xB1) bytes
*/

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_FINN[37] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '"' */ 0x22,   3, MO_ShiftLeft,
	/* '#' */ 0x23,   4, MO_ShiftLeft|MO_AltRight,
	/* '&' */ 0x26,   7, MO_ShiftLeft,
	/* ''' */ 0x27,  42, 0,
	/* '(' */ 0x28,   9, MO_ShiftLeft,
	/* ')' */ 0x29,  10, MO_ShiftLeft,
	/* '*' */ 0x2A,  42, MO_ShiftLeft,
	/* '+' */ 0x2B,  12, 0,
	/* '-' */ 0x2D,  55, 0,
	/* '/' */ 0x2F,   8, MO_ShiftLeft,
	/* ':' */ 0x3A,  54, MO_ShiftLeft,
	/* ';' */ 0x3B,  53, MO_ShiftLeft,
	/* '<' */ 0x3C,  45, 0,
	/* '=' */ 0x3D,  11, MO_ShiftLeft,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '?' */ 0x3F,  12, MO_ShiftLeft,
	/* '@' */ 0x40,   1, MO_ShiftLeft,
	/* '[' */ 0x5B,  41, MO_ShiftLeft|MO_AltRight,
	/* '\' */ 0x5C,  40, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x5D,  27, MO_ShiftLeft|MO_AltRight,
	/* '^' */ 0x5E,  13, 0,
	/* '_' */ 0x5F,  55, MO_ShiftLeft,
	/* '`' */ 0x60,  13, MO_ShiftLeft,
	/* '{' */ 0x7B,  41, MO_AltRight,
	/* '|' */ 0x7C,  40, MO_AltRight,
	/* '}' */ 0x7D,  27, MO_AltRight,
	/* '~' */ 0x7E,   1, 0,
	/* '§' */ 0xA7,   4, MO_ShiftLeft,
	/* 'Ä' */ 0xC4,  41, MO_ShiftLeft,
	/* 'Å' */ 0xC5,  27, MO_ShiftLeft,
	/* 'Ö' */ 0xD6,  40, MO_ShiftLeft,
	/* 'Ü' */ 0xDC,  28, MO_ShiftLeft,
	/* 'ä' */ 0xE4,  41, 0,
	/* 'å' */ 0xE5,  27, 0,
	/* 'ö' */ 0xF6,  40, 0,
	/* 'ü' */ 0xFC,  28, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	37 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		148 (0x94) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		111 (0x6F) bytes
*/

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_FRAN[59] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,   9, 0,
	/* '"' */ 0x22,   4, 0,
	/* '#' */ 0x23,  42, 0,
	/* '$' */ 0x24,  28, 0,
	/* '%' */ 0x25,  41, MO_ShiftLeft,
	/* '&' */ 0x26,   2, 0,
	/* ''' */ 0x27,   5, 0,
	/* '(' */ 0x28,   6, 0,
	/* ')' */ 0x29,  12, 0,
	/* '*' */ 0x2A,  28, MO_ShiftLeft,
	/* '+' */ 0x2B,  55, MO_ShiftLeft,
	/* ',' */ 0x2C,  52, 0,
	/* '-' */ 0x2D,  13, 0,
	/* '.' */ 0x2E,  53, MO_ShiftLeft,
	/* '/' */ 0x2F,  54, MO_ShiftLeft,
	/* '0' */ 0x30,  11, MO_ShiftLeft,
	/* '1' */ 0x31,   2, MO_ShiftLeft,
	/* '2' */ 0x32,   3, MO_ShiftLeft,
	/* '3' */ 0x33,   4, MO_ShiftLeft,
	/* '4' */ 0x34,   5, MO_ShiftLeft,
	/* '5' */ 0x35,   6, MO_ShiftLeft,
	/* '6' */ 0x36,   7, MO_ShiftLeft,
	/* '7' */ 0x37,   8, MO_ShiftLeft,
	/* '8' */ 0x38,   9, MO_ShiftLeft,
	/* '9' */ 0x39,  10, MO_ShiftLeft,
	/* ':' */ 0x3A,  54, 0,
	/* ';' */ 0x3B,  53, 0,
	/* '<' */ 0x3C,  45, 0,
	/* '=' */ 0x3D,  55, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '?' */ 0x3F,  52, MO_ShiftLeft,
	/* '@' */ 0x40,  42, MO_ShiftLeft,
	/* 'A' */ 0x41,  17, MO_ShiftLeft,
	/* 'M' */ 0x4D,  40, MO_ShiftLeft,
	/* 'Q' */ 0x51,  31, MO_ShiftLeft,
	/* 'W' */ 0x57,  46, MO_ShiftLeft,
	/* 'Z' */ 0x5A,  18, MO_ShiftLeft,
	/* '[' */ 0x5B,   7, MO_AltRight,
	/* '\' */ 0x5C,  41, MO_AltRight,
	/* ']' */ 0x5D,   8, MO_AltRight,
	/* '^' */ 0x5E,  27, 0,
	/* '_' */ 0x5F,  13, MO_ShiftLeft,
	/* 'a' */ 0x61,  17, 0,
	/* 'm' */ 0x6D,  40, 0,
	/* 'q' */ 0x71,  31, 0,
	/* 'w' */ 0x77,  46, 0,
	/* 'z' */ 0x7A,  18, 0,
	/* '{' */ 0x7B,  10, MO_AltRight,
	/* '|' */ 0x7C,   3, MO_AltRight,
	/* '}' */ 0x7D,  11, MO_AltRight,
	/* '§' */ 0xA7,   7, 0,
	/* '¨' */ 0xA8,  27, MO_ShiftLeft,
	/* '°' */ 0xB0,  12, MO_ShiftLeft,
	/* 'à' */ 0xE0,  11, 0,
	/* 'ç' */ 0xE7,  10, 0,
	/* 'è' */ 0xE8,   8, 0,
	/* 'é' */ 0xE9,   3, 0,
	/* 'ù' */ 0xF9,  41, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	59 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		236 (0xEC) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		177 (0xB1) bytes
*/

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_GERM[55] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '"' */ 0x22,   3, MO_ShiftLeft,
	/* '#' */ 0x23,  42, 0,
	/* '&' */ 0x26,   7, MO_ShiftLeft,
	/* ''' */ 0x27,  42, MO_ShiftLeft,
	/* '(' */ 0x28,   9, MO_ShiftLeft,
	/* ')' */ 0x29,  10, MO_ShiftLeft,
	/* '*' */ 0x2A,  28, MO_ShiftLeft,
	/* '+' */ 0x2B,  28, 0,
	/* '-' */ 0x2D,  55, 0,
	/* '/' */ 0x2F,   8, MO_ShiftLeft,
	/* ':' */ 0x3A,  54, MO_ShiftLeft,
	/* ';' */ 0x3B,  53, MO_ShiftLeft,
	/* '<' */ 0x3C,  45, 0,
	/* '=' */ 0x3D,  11, MO_ShiftLeft,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '?' */ 0x3F,  12, MO_ShiftLeft,
	/* '@' */ 0x40,  17, MO_AltRight,
	/* 'Y' */ 0x59,  46, MO_ShiftLeft,
	/* 'Z' */ 0x5A,  22, MO_ShiftLeft,
	/* '[' */ 0x5B,   9, MO_AltRight,
	/* '\' */ 0x5C,  12, MO_AltRight,
	/* ']' */ 0x5D,  10, MO_AltRight,
	/* '^' */ 0x5E,   1, 0,
	/* '_' */ 0x5F,  55, MO_ShiftLeft,
	/* '`' */ 0x60,  13, MO_ShiftLeft,
	/* 'y' */ 0x79,  46, 0,
	/* 'z' */ 0x7A,  22, 0,
	/* '{' */ 0x7B,   8, MO_AltRight,
	/* '|' */ 0x7C,  45, MO_AltRight,
	/* '}' */ 0x7D,  11, MO_AltRight,
	/* '~' */ 0x7E,  28, MO_AltRight,
	/* '¡' */ 0xA1,   2, MO_ShiftLeft|MO_AltRight,
	/* '£' */ 0xA3,   4, MO_ShiftLeft|MO_AltRight,
	/* '¤' */ 0xA4,   5, MO_ShiftLeft|MO_AltRight,
	/* '§' */ 0xA7,   4, MO_ShiftLeft,
	/* '¬' */ 0xAC,   1, MO_AltRight,
	/* '°' */ 0xB0,   1, MO_ShiftLeft,
	/* '±' */ 0xB1,  10, MO_ShiftLeft|MO_AltRight,
	/* '²' */ 0xB2,   3, MO_AltRight,
	/* '³' */ 0xB3,   4, MO_AltRight,
	/* '´' */ 0xB4,  13, 0,
	/* '¸' */ 0xB8,  13, MO_AltRight,
	/* '¹' */ 0xB9,   2, MO_AltRight,
	/* '¼' */ 0xBC,   5, MO_AltRight,
	/* '½' */ 0xBD,   6, MO_AltRight,
	/* '¾' */ 0xBE,   7, MO_AltRight,
	/* '¿' */ 0xBF,  12, MO_ShiftLeft|MO_AltRight,
	/* 'Ä' */ 0xC4,  41, MO_ShiftLeft,
	/* 'Ö' */ 0xD6,  40, MO_ShiftLeft,
	/* 'Ü' */ 0xDC,  27, MO_ShiftLeft,
	/* 'ß' */ 0xDF,  12, 0,
	/* 'ä' */ 0xE4,  41, 0,
	/* 'ö' */ 0xF6,  40, 0,
	/* 'ü' */ 0xFC,  27, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	55 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		220 (0xDC) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		165 (0xA5) bytes
*/

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_ITAL[56] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,  55, MO_ShiftLeft,
	/* '"' */ 0x22,   4, 0,
	/* '#' */ 0x23,   3, MO_AltRight,
	/* '$' */ 0x24,  28, 0,
	/* '%' */ 0x25,  41, MO_ShiftLeft,
	/* '&' */ 0x26,  28, MO_ShiftLeft,
	/* ''' */ 0x27,   5, 0,
	/* '(' */ 0x28,   6, 0,
	/* ')' */ 0x29,  12, 0,
	/* '*' */ 0x2A,  42, 0,
	/* ',' */ 0x2C,  52, 0,
	/* '-' */ 0x2D,  13, 0,
	/* '.' */ 0x2E,  53, MO_ShiftLeft,
	/* '/' */ 0x2F,  54, MO_ShiftLeft,
	/* '0' */ 0x30,  11, MO_ShiftLeft,
	/* '1' */ 0x31,   2, MO_ShiftLeft,
	/* '2' */ 0x32,   3, MO_ShiftLeft,
	/* '3' */ 0x33,   4, MO_ShiftLeft,
	/* '4' */ 0x34,   5, MO_ShiftLeft,
	/* '5' */ 0x35,   6, MO_ShiftLeft,
	/* '6' */ 0x36,   7, MO_ShiftLeft,
	/* '7' */ 0x37,   8, MO_ShiftLeft,
	/* '8' */ 0x38,   9, MO_ShiftLeft,
	/* '9' */ 0x39,  10, MO_ShiftLeft,
	/* ':' */ 0x3A,  54, 0,
	/* ';' */ 0x3B,  53, 0,
	/* '<' */ 0x3C,  45, 0,
	/* '=' */ 0x3D,  27, MO_ShiftLeft,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '?' */ 0x3F,  52, MO_ShiftLeft,
	/* '@' */ 0x40,   2, MO_AltRight,
	/* 'M' */ 0x4D,  40, MO_ShiftLeft,
	/* 'W' */ 0x57,  46, MO_ShiftLeft,
	/* 'Z' */ 0x5A,  18, MO_ShiftLeft,
	/* '[' */ 0x5B,  10, MO_AltRight,
	/* '\' */ 0x5C,  42, MO_AltRight,
	/* ']' */ 0x5D,  11, MO_AltRight,
	/* '^' */ 0x5E,   9, 0,
	/* '_' */ 0x5F,   7, 0,
	/* 'm' */ 0x6D,  40, 0,
	/* 'w' */ 0x77,  46, 0,
	/* 'z' */ 0x7A,  18, 0,
	/* '{' */ 0x7B,  41, MO_AltRight,
	/* '|' */ 0x7C,  55, MO_AltRight,
	/* '}' */ 0x7D,  27, MO_AltRight,
	/* '£' */ 0xA3,   2, 0,
	/* '§' */ 0xA7,  42, MO_ShiftLeft,
	/* '°' */ 0xB0,  12, MO_ShiftLeft,
	/* 'à' */ 0xE0,  11, 0,
	/* 'ç' */ 0xE7,  10, 0,
	/* 'è' */ 0xE8,   8, 0,
	/* 'é' */ 0xE9,   3, 0,
	/* 'ì' */ 0xEC,  27, 0,
	/* 'ò' */ 0xF2,  55, 0,
	/* 'ù' */ 0xF9,  41, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	56 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		224 (0xE0) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		168 (0xA8) bytes
*/

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_SWIT[44] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,   1, 0,
	/* '"' */ 0x22,   3, MO_ShiftLeft,
	/* '#' */ 0x23,  42, MO_ShiftLeft|MO_AltRight,
	/* '$' */ 0x24,  42, 0,
	/* '&' */ 0x26,   7, MO_ShiftLeft,
	/* ''' */ 0x27,  12, 0,
	/* '(' */ 0x28,   9, MO_ShiftLeft,
	/* ')' */ 0x29,  10, MO_ShiftLeft,
	/* '*' */ 0x2A,   4, MO_ShiftLeft,
	/* '+' */ 0x2B,   2, MO_ShiftLeft,
	/* '-' */ 0x2D,  55, 0,
	/* '/' */ 0x2F,   8, MO_ShiftLeft,
	/* ':' */ 0x3A,  54, MO_ShiftLeft,
	/* ';' */ 0x3B,  53, MO_ShiftLeft,
	/* '<' */ 0x3C,  45, 0,
	/* '=' */ 0x3D,  11, MO_ShiftLeft,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '?' */ 0x3F,  12, MO_ShiftLeft,
	/* '@' */ 0x40,   5, MO_ShiftLeft|MO_AltRight,
	/* 'Y' */ 0x59,  46, MO_ShiftLeft,
	/* 'Z' */ 0x5A,  22, MO_ShiftLeft,
	/* '[' */ 0x5B,  40, MO_ShiftLeft|MO_AltRight,
	/* '\' */ 0x5C,  27, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x5D,  41, MO_ShiftLeft|MO_AltRight,
	/* '^' */ 0x5E,  13, 0,
	/* '_' */ 0x5F,  55, MO_ShiftLeft,
	/* '`' */ 0x60,  13, MO_ShiftLeft,
	/* 'y' */ 0x79,  46, 0,
	/* 'z' */ 0x7A,  22, 0,
	/* '{' */ 0x7B,  40, MO_AltRight,
	/* '|' */ 0x7C,  27, MO_AltRight,
	/* '}' */ 0x7D,  41, MO_AltRight,
	/* '~' */ 0x7E,  28, MO_ShiftLeft,
	/* '£' */ 0xA3,  42, MO_ShiftLeft,
	/* '¨' */ 0xA8,  28, 0,
	/* '°' */ 0xB0,   1, MO_ShiftLeft,
	/* 'à' */ 0xE0,  41, MO_ShiftLeft,
	/* 'ä' */ 0xE4,  41, 0,
	/* 'ç' */ 0xE7,   5, MO_ShiftLeft,
	/* 'è' */ 0xE8,  27, MO_ShiftLeft,
	/* 'é' */ 0xE9,  40, MO_ShiftLeft,
	/* 'ö' */ 0xF6,  40, 0,
	/* 'ü' */ 0xFC,  27, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	44 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		176 (0xB0) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		132 (0x84) bytes
*/

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_UKIN[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
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
const CKeyFixup DEC_VT510_LK411_DENM[38] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '"' */ 0x22,   3, MO_ShiftLeft,
	/* '#' */ 0x23,   4, MO_ShiftLeft|MO_AltRight,
	/* '&' */ 0x26,   7, MO_ShiftLeft,
	/* ''' */ 0x27,  42, 0,
	/* '(' */ 0x28,   9, MO_ShiftLeft,
	/* ')' */ 0x29,  10, MO_ShiftLeft,
	/* '*' */ 0x2A,  42, MO_ShiftLeft,
	/* '+' */ 0x2B,  12, 0,
	/* '-' */ 0x2D,  55, 0,
	/* '/' */ 0x2F,   8, MO_ShiftLeft,
	/* ':' */ 0x3A,  54, MO_ShiftLeft,
	/* ';' */ 0x3B,  53, MO_ShiftLeft,
	/* '<' */ 0x3C,  45, 0,
	/* '=' */ 0x3D,  11, MO_ShiftLeft,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '?' */ 0x3F,  12, MO_ShiftLeft,
	/* '@' */ 0x40,   1, MO_AltRight,
	/* '[' */ 0x5B,  40, MO_ShiftLeft|MO_AltRight,
	/* '\' */ 0x5C,  41, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x5D,  27, MO_ShiftLeft|MO_AltRight,
	/* '^' */ 0x5E,  28, MO_ShiftLeft,
	/* '_' */ 0x5F,  55, MO_ShiftLeft,
	/* '`' */ 0x60,  13, MO_ShiftLeft,
	/* '{' */ 0x7B,  40, MO_AltRight,
	/* '|' */ 0x7C,  41, MO_AltRight,
	/* '}' */ 0x7D,  27, MO_AltRight,
	/* '~' */ 0x7E,   1, 0,
	/* '£' */ 0xA3,   4, MO_ShiftLeft,
	/* '§' */ 0xA7,   1, MO_ShiftLeft,
	/* '¨' */ 0xA8,  28, 0,
	/* '´' */ 0xB4,  13, 0,
	/* 'Å' */ 0xC5,  27, MO_ShiftLeft,
	/* 'Æ' */ 0xC6,  40, MO_ShiftLeft,
	/* 'Ø' */ 0xD8,  41, MO_ShiftLeft,
	/* 'å' */ 0xE5,  27, 0,
	/* 'æ' */ 0xE6,  40, 0,
	/* 'ø' */ 0xF8,  41, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	38 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		152 (0x98) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		114 (0x72) bytes
*/

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_NORW[35] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '"' */ 0x22,   3, MO_ShiftLeft,
	/* '&' */ 0x26,   7, MO_ShiftLeft,
	/* ''' */ 0x27,  42, 0,
	/* '(' */ 0x28,   9, MO_ShiftLeft,
	/* ')' */ 0x29,  10, MO_ShiftLeft,
	/* '*' */ 0x2A,  42, MO_ShiftLeft,
	/* '+' */ 0x2B,  12, 0,
	/* '-' */ 0x2D,  55, 0,
	/* '/' */ 0x2F,   8, MO_ShiftLeft,
	/* ':' */ 0x3A,  54, MO_ShiftLeft,
	/* ';' */ 0x3B,  53, MO_ShiftLeft,
	/* '<' */ 0x3C,  45, 0,
	/* '=' */ 0x3D,  11, MO_ShiftLeft,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '?' */ 0x3F,  12, MO_ShiftLeft,
	/* '@' */ 0x40,   1, MO_ShiftLeft,
	/* '[' */ 0x5B,  41, MO_ShiftLeft|MO_AltRight,
	/* '\' */ 0x5C,  40, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x5D,  27, MO_ShiftLeft|MO_AltRight,
	/* '^' */ 0x5E,  28, MO_ShiftLeft,
	/* '_' */ 0x5F,  55, MO_ShiftLeft,
	/* '`' */ 0x60,  13, MO_ShiftLeft,
	/* '{' */ 0x7B,  41, MO_AltRight,
	/* '|' */ 0x7C,  40, MO_AltRight,
	/* '}' */ 0x7D,  27, MO_AltRight,
	/* '~' */ 0x7E,   1, 0,
	/* '¨' */ 0xA8,  28, 0,
	/* '´' */ 0xB4,  13, 0,
	/* 'Å' */ 0xC5,  27, MO_ShiftLeft,
	/* 'Æ' */ 0xC6,  41, MO_ShiftLeft,
	/* 'Ø' */ 0xD8,  40, MO_ShiftLeft,
	/* 'å' */ 0xE5,  27, 0,
	/* 'æ' */ 0xE6,  41, 0,
	/* 'ø' */ 0xF8,  40, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	35 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		140 (0x8C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		105 (0x69) bytes
*/

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_SPAI[34] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '"' */ 0x22,   3, MO_ShiftLeft,
	/* '&' */ 0x26,   7, MO_ShiftLeft,
	/* ''' */ 0x27,  12, 0,
	/* '(' */ 0x28,   9, MO_ShiftLeft,
	/* ')' */ 0x29,  10, MO_ShiftLeft,
	/* '*' */ 0x2A,  28, MO_ShiftLeft,
	/* '+' */ 0x2B,  28, 0,
	/* '-' */ 0x2D,  55, 0,
	/* '/' */ 0x2F,   8, MO_ShiftLeft,
	/* ':' */ 0x3A,  54, MO_ShiftLeft,
	/* ';' */ 0x3B,  53, MO_ShiftLeft,
	/* '<' */ 0x3C,  45, 0,
	/* '=' */ 0x3D,  11, MO_ShiftLeft,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '?' */ 0x3F,  12, MO_ShiftLeft,
	/* '@' */ 0x40,   1, MO_AltRight,
	/* '[' */ 0x5B,  13, MO_ShiftLeft|MO_AltRight,
	/* '\' */ 0x5C,   1, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x5D,  13, MO_AltRight,
	/* '^' */ 0x5E,  27, MO_ShiftLeft,
	/* '_' */ 0x5F,  55, MO_ShiftLeft,
	/* '`' */ 0x60,  27, 0,
	/* '{' */ 0x7B,  40, MO_AltRight,
	/* '|' */ 0x7C,  42, MO_AltRight,
	/* '}' */ 0x7D,  40, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x7E,  42, MO_ShiftLeft,
	/* '¡' */ 0xA1,   1, MO_ShiftLeft,
	/* '°' */ 0xB0,  13, 0,
	/* '´' */ 0xB4,  41, 0,
	/* '¿' */ 0xBF,   1, 0,
	/* 'Ñ' */ 0xD1,  40, MO_ShiftLeft,
	/* 'ç' */ 0xE7,  42, 0,
	/* 'ñ' */ 0xF1,  40, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	34 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		136 (0x88) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		102 (0x66) bytes
*/

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_DUTC[4] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '£' */ 0xA3,   4, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	4 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		16 (0x10) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		12 (0xC) bytes
*/

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_HEBR[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
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
const CKeyFixup DEC_VT510_LK411_PORT[16] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '"' */ 0x22, NOS, 0,
	/* ''' */ 0x27, NOS, 0,
	/* ':' */ 0x3A,  54, MO_ShiftLeft,
	/* ';' */ 0x3B,  53, MO_ShiftLeft,
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '[' */ 0x5B,  42, 0,
	/* '\' */ 0x5C,   1, 0,
	/* '`' */ 0x60,  27, MO_ShiftLeft,
	/* '{' */ 0x7B,  42, MO_ShiftLeft,
	/* '|' */ 0x7C,   1, MO_ShiftLeft,
	/* '~' */ 0x7E,  41, 0,
	/* '´' */ 0xB4,  27, 0,
	/* 'Ç' */ 0xC7,  40, MO_ShiftLeft,
	/* 'ç' */ 0xE7,  40, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	16 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		64 (0x40) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		48 (0x30) bytes
*/

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_LATI[34] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '"' */ 0x22,   3, MO_ShiftLeft,
	/* '&' */ 0x26,   7, MO_ShiftLeft,
	/* ''' */ 0x27,  12, 0,
	/* '(' */ 0x28,   9, MO_ShiftLeft,
	/* ')' */ 0x29,  10, MO_ShiftLeft,
	/* '*' */ 0x2A,  28, MO_ShiftLeft,
	/* '+' */ 0x2B,  28, 0,
	/* '-' */ 0x2D,  55, 0,
	/* '/' */ 0x2F,   8, MO_ShiftLeft,
	/* ':' */ 0x3A,  54, MO_ShiftLeft,
	/* ';' */ 0x3B,  53, MO_ShiftLeft,
	/* '<' */ 0x3C,  45, 0,
	/* '=' */ 0x3D,  11, MO_ShiftLeft,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '?' */ 0x3F,  12, MO_ShiftLeft,
	/* '@' */ 0x40,  17, MO_AltRight,
	/* '[' */ 0x5B,  41, MO_ShiftLeft,
	/* '\' */ 0x5C,  12, MO_AltRight,
	/* ']' */ 0x5D,  42, MO_ShiftLeft,
	/* '^' */ 0x5E,  41, MO_AltRight,
	/* '_' */ 0x5F,  55, MO_ShiftLeft,
	/* '`' */ 0x60,  27, 0,
	/* '{' */ 0x7B,  41, 0,
	/* '|' */ 0x7C,   1, 0,
	/* '}' */ 0x7D,  42, 0,
	/* '~' */ 0x7E,  28, MO_AltRight,
	/* '¡' */ 0xA1,  13, MO_ShiftLeft,
	/* '¨' */ 0xA8,  27, MO_ShiftLeft,
	/* '¬' */ 0xAC,   1, MO_AltRight,
	/* '°' */ 0xB0,   1, MO_ShiftLeft,
	/* '¿' */ 0xBF,  13, 0,
	/* 'Ñ' */ 0xD1,  40, MO_ShiftLeft,
	/* 'ñ' */ 0xF1,  40, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	34 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		136 (0x88) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		102 (0x66) bytes
*/

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_CZEC[60] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,  41, MO_ShiftLeft,
	/* '"' */ 0x22,  40, MO_ShiftLeft,
	/* '#' */ 0x23,   4, MO_AltRight,
	/* '$' */ 0x24,   5, MO_AltRight,
	/* '%' */ 0x25,   6, MO_AltRight,
	/* '&' */ 0x26,   8, MO_AltRight,
	/* ''' */ 0x27,   1, 0,
	/* '(' */ 0x28,  28, MO_ShiftLeft,
	/* ')' */ 0x29,  28, 0,
	/* '*' */ 0x2A,   9, MO_AltRight,
	/* '+' */ 0x2B,   2, 0,
	/* '-' */ 0x2D,  55, 0,
	/* '/' */ 0x2F,  27, MO_ShiftLeft,
	/* '0' */ 0x30,  11, MO_ShiftLeft,
	/* '1' */ 0x31,   2, MO_ShiftLeft,
	/* '2' */ 0x32,   3, MO_ShiftLeft,
	/* '3' */ 0x33,   4, MO_ShiftLeft,
	/* '4' */ 0x34,   5, MO_ShiftLeft,
	/* '5' */ 0x35,   6, MO_ShiftLeft,
	/* '6' */ 0x36,   7, MO_ShiftLeft,
	/* '7' */ 0x37,   8, MO_ShiftLeft,
	/* '8' */ 0x38,   9, MO_ShiftLeft,
	/* '9' */ 0x39,  10, MO_ShiftLeft,
	/* ':' */ 0x3A,  54, MO_ShiftLeft,
	/* ';' */ 0x3B,  40, MO_AltRight,
	/* '<' */ 0x3C,  53, MO_AltRight,
	/* '=' */ 0x3D,  12, 0,
	/* '>' */ 0x3E,  54, MO_AltRight,
	/* '?' */ 0x3F,  53, MO_ShiftLeft,
	/* '@' */ 0x40,   3, MO_AltRight,
	/* 'Y' */ 0x59,  22, MO_ShiftLeft|MO_AltRight,
	/* 'Z' */ 0x5A,  22, MO_ShiftLeft,
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,  45, 0,
	/* ']' */ 0x5D,  28, MO_AltRight,
	/* '^' */ 0x5E,   7, MO_AltRight,
	/* '_' */ 0x5F,  55, MO_ShiftLeft,
	/* '`' */ 0x60,  42, MO_ShiftLeft,
	/* 'y' */ 0x79,  22, MO_AltRight,
	/* 'z' */ 0x7A,  22, 0,
	/* '{' */ 0x7B,  10, MO_AltRight,
	/* '|' */ 0x7C,  45, MO_ShiftLeft,
	/* '}' */ 0x7D,  11, MO_AltRight,
	/* ??? */ 0x9A,   4, 0,	/* Not CP1252 */
	/* ??? */ 0x9E,   7, 0,	/* Not CP1252 */
	/* ??? */ 0xA1,  13, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xA7,  41, 0,	/* Not CP1252 */
	/* ??? */ 0xA8,  42, 0,	/* Not CP1252 */
	/* ??? */ 0xB0,  13, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xB4,  13, 0,	/* Not CP1252 */
	/* ??? */ 0xE1,   9, 0,	/* Not CP1252 */
	/* ??? */ 0xE8,   5, 0,	/* Not CP1252 */
	/* ??? */ 0xE9,  11, 0,	/* Not CP1252 */
	/* ??? */ 0xEC,   3, 0,	/* Not CP1252 */
	/* ??? */ 0xED,  10, 0,	/* Not CP1252 */
	/* ??? */ 0xF8,   6, 0,	/* Not CP1252 */
	/* ??? */ 0xF9,  40, 0,	/* Not CP1252 */
	/* ??? */ 0xFA,  27, 0,	/* Not CP1252 */
	/* ??? */ 0xFD,   8, 0,	/* Not CP1252 */
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	60 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		240 (0xF0) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		180 (0xB4) bytes
*/

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_BRAZ[31] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '"' */ 0x22,   1, MO_ShiftLeft,
	/* ''' */ 0x27,   1, 0,
	/* '/' */ 0x2F,  14, 0,
	/* ':' */ 0x3A,  55, MO_ShiftLeft,
	/* ';' */ 0x3B,  55, 0,
	/* '?' */ 0x3F,  14, MO_ShiftLeft,
	/* '[' */ 0x5B,  28, 0,
	/* '\' */ 0x5C,  45, 0,
	/* ']' */ 0x5D,  42, 0,
	/* '^' */ 0x5E,  41, MO_ShiftLeft|MO_DeadKey,
	/* '`' */ 0x60,  27, MO_ShiftLeft|MO_DeadKey,
	/* '{' */ 0x7B,  28, MO_ShiftLeft,
	/* '|' */ 0x7C,  45, MO_ShiftLeft,
	/* '}' */ 0x7D,  42, MO_ShiftLeft,
	/* '~' */ 0x7E,  41, MO_DeadKey,
	/* '€' */ 0x80,  19, MO_AltRight,
	/* '¢' */ 0xA2,   6, MO_AltRight,
	/* '£' */ 0xA3,   5, MO_AltRight,
	/* '§' */ 0xA7,  13, MO_AltRight,
	/* '¨' */ 0xA8,   7, MO_ShiftLeft|MO_DeadKey,
	/* 'ª' */ 0xAA,  28, MO_AltRight,
	/* '¬' */ 0xAC,   7, MO_AltRight,
	/* '°' */ 0xB0,  14, MO_AltRight,
	/* '²' */ 0xB2,   3, MO_AltRight,
	/* '³' */ 0xB3,   4, MO_AltRight,
	/* '´' */ 0xB4,  27, MO_DeadKey,
	/* '¹' */ 0xB9,   2, MO_AltRight,
	/* 'º' */ 0xBA,  42, MO_AltRight,
	/* 'Ç' */ 0xC7,  40, MO_ShiftLeft,
	/* 'ç' */ 0xE7,  40, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	31 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		124 (0x7C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		93 (0x5D) bytes
*/
/* DEC_VT510_LK411_GREK is not supported */

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_FCAN[22] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '"' */ 0x22,   3, MO_ShiftLeft,
	/* '#' */ 0x23,  28, 0,
	/* ''' */ 0x27, NOS, 0,
	/* '/' */ 0x2F,   4, MO_ShiftLeft,
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '?' */ 0x3F,   7, MO_ShiftLeft,
	/* '@' */ 0x40,  28, MO_ShiftLeft,
	/* '[' */ 0x5B,  27, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x5D,  27, MO_AltRight,
	/* '^' */ 0x5E,  41, MO_ShiftLeft,
	/* '`' */ 0x60,  41, 0,
	/* '{' */ 0x7B, NOS, 0,
	/* '}' */ 0x7D, NOS, 0,
	/* '~' */ 0x7E,   1, 0,
	/* '°' */ 0xB0,   1, MO_ShiftLeft,
	/* '´' */ 0xB4,  53, MO_ShiftLeft,
	/* 'Ç' */ 0xC7,  27, MO_ShiftLeft,
	/* 'É' */ 0xC9,  55, MO_ShiftLeft,
	/* 'ç' */ 0xE7,  27, 0,
	/* 'é' */ 0xE9,  55, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	22 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		88 (0x58) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		66 (0x42) bytes
*/

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_HUNG[48] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,   2, MO_ShiftLeft|MO_AltRight,
	/* '"' */ 0x22,   3, MO_ShiftLeft,
	/* '#' */ 0x23,   4, MO_ShiftLeft|MO_AltRight,
	/* '$' */ 0x24,   5, MO_ShiftLeft|MO_AltRight,
	/* '&' */ 0x26,   8, MO_ShiftLeft|MO_AltRight,
	/* ''' */ 0x27,   2, MO_ShiftLeft,
	/* '+' */ 0x2B,   4, MO_ShiftLeft,
	/* '-' */ 0x2D,  12, MO_AltRight,
	/* '/' */ 0x2F,   7, MO_ShiftLeft,
	/* ':' */ 0x3A,  40, MO_ShiftLeft|MO_AltRight,
	/* ';' */ 0x3B,  40, MO_AltRight,
	/* '<' */ 0x3C,  45, MO_AltRight,
	/* '=' */ 0x3D,   8, MO_ShiftLeft,
	/* '>' */ 0x3E,  45, MO_ShiftLeft|MO_AltRight,
	/* '?' */ 0x3F,  53, MO_ShiftLeft,
	/* '@' */ 0x40,   3, MO_ShiftLeft|MO_AltRight,
	/* 'Y' */ 0x59,  46, MO_ShiftLeft,
	/* 'Z' */ 0x5A,  22, MO_ShiftLeft,
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,  42, MO_AltRight,
	/* ']' */ 0x5D,  28, MO_AltRight,
	/* '^' */ 0x5E,   7, MO_ShiftLeft|MO_AltRight,
	/* '_' */ 0x5F,  12, MO_ShiftLeft|MO_AltRight,
	/* '`' */ 0x60, NOS, 0,
	/* 'y' */ 0x79,  46, 0,
	/* 'z' */ 0x7A,  22, 0,
	/* '{' */ 0x7B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x7C,  42, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x7D,  28, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x7E,   1, MO_ShiftLeft|MO_AltRight,
	/* ??? */ 0x91,   1, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xC1,  41, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xC9,  40, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xCD,  45, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xD3,   1, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xD5,  27, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xD6,  12, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xDA,  28, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xDC,  13, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xE1,  41, 0,	/* Not CP1252 */
	/* ??? */ 0xE9,  40, 0,	/* Not CP1252 */
	/* ??? */ 0xED,  45, 0,	/* Not CP1252 */
	/* ??? */ 0xF3,   1, 0,	/* Not CP1252 */
	/* ??? */ 0xF5,  27, 0,	/* Not CP1252 */
	/* ??? */ 0xF6,  12, 0,	/* Not CP1252 */
	/* ??? */ 0xFA,  28, 0,	/* Not CP1252 */
	/* ??? */ 0xFC,  13, 0,	/* Not CP1252 */
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	48 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		192 (0xC0) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		144 (0x90) bytes
*/

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_POLI[14] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* ??? */ 0x9C,  32, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0x9F,  47, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xA3,  39, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xAF,  46, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xB3,  39, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xB9,  31, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xBF,  46, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xE6,  48, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xEA,  19, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xF1,  51, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xF3,  47, MO_AltRight,	/* Not CP1252 */
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	14 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		56 (0x38) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		42 (0x2A) bytes
*/

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_SCS_[36] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '"' */ 0x22,   3, MO_ShiftLeft,
	/* '&' */ 0x26,   7, MO_ShiftLeft,
	/* ''' */ 0x27,   1, 0,
	/* '(' */ 0x28,   9, MO_ShiftLeft,
	/* ')' */ 0x29,  10, MO_ShiftLeft,
	/* '*' */ 0x2A,   9, MO_AltRight,
	/* '+' */ 0x2B,  13, MO_ShiftLeft|MO_AltRight,
	/* '-' */ 0x2D,  12, MO_AltRight,
	/* '/' */ 0x2F,  12, 0,
	/* ':' */ 0x3A,  40, MO_ShiftLeft|MO_AltRight,
	/* ';' */ 0x3B,  40, MO_AltRight,
	/* '<' */ 0x3C,  45, 0,
	/* '=' */ 0x3D,  11, MO_ShiftLeft,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '?' */ 0x3F,  12, MO_ShiftLeft,
	/* '@' */ 0x40,   3, MO_AltRight,
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,  42, MO_AltRight,
	/* ']' */ 0x5D,  28, MO_AltRight,
	/* '^' */ 0x5E,   7, MO_AltRight,
	/* '_' */ 0x5F,  12, MO_ShiftLeft|MO_AltRight,
	/* '`' */ 0x60, NOS, 0,
	/* '{' */ 0x7B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x7C,  42, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x7D,  28, MO_ShiftLeft|MO_AltRight,
	/* ??? */ 0x8A,  27, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0x8E,  42, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0x9A,  27, 0,	/* Not CP1252 */
	/* ??? */ 0x9E,  42, 0,	/* Not CP1252 */
	/* ??? */ 0xC6,  41, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xC8,  40, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xD0,  28, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xE6,  41, 0,	/* Not CP1252 */
	/* ??? */ 0xE8,  40, 0,	/* Not CP1252 */
	/* ??? */ 0xF0,  28, 0,	/* Not CP1252 */
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	36 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		144 (0x90) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		108 (0x6C) bytes
*/

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_SLOV[60] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '!' */ 0x21,  41, MO_ShiftLeft,
	/* '"' */ 0x22,  40, MO_ShiftLeft,
	/* '#' */ 0x23,   4, MO_AltRight,
	/* '$' */ 0x24,   5, MO_AltRight,
	/* '%' */ 0x25,   6, MO_AltRight,
	/* '&' */ 0x26,   8, MO_AltRight,
	/* ''' */ 0x27,  41, MO_AltRight,
	/* '(' */ 0x28,  28, MO_ShiftLeft,
	/* ')' */ 0x29,  42, MO_ShiftLeft,
	/* '*' */ 0x2A,   9, MO_AltRight,
	/* '+' */ 0x2B,   2, 0,
	/* '-' */ 0x2D,  55, 0,
	/* '/' */ 0x2F,  27, MO_ShiftLeft,
	/* '0' */ 0x30,  11, MO_ShiftLeft,
	/* '1' */ 0x31,   2, MO_ShiftLeft,
	/* '2' */ 0x32,   3, MO_ShiftLeft,
	/* '3' */ 0x33,   4, MO_ShiftLeft,
	/* '4' */ 0x34,   5, MO_ShiftLeft,
	/* '5' */ 0x35,   6, MO_ShiftLeft,
	/* '6' */ 0x36,   7, MO_ShiftLeft,
	/* '7' */ 0x37,   8, MO_ShiftLeft,
	/* '8' */ 0x38,   9, MO_ShiftLeft,
	/* '9' */ 0x39,  10, MO_ShiftLeft,
	/* ':' */ 0x3A,  54, MO_ShiftLeft,
	/* ';' */ 0x3B,  40, MO_AltRight,
	/* '<' */ 0x3C,  53, MO_AltRight,
	/* '=' */ 0x3D,  12, 0,
	/* '>' */ 0x3E,  54, MO_AltRight,
	/* '?' */ 0x3F,  53, MO_ShiftLeft,
	/* '@' */ 0x40,   3, MO_AltRight,
	/* 'Y' */ 0x59,  46, MO_ShiftLeft,
	/* 'Z' */ 0x5A,  22, MO_ShiftLeft,
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,  45, 0,
	/* ']' */ 0x5D,  28, MO_AltRight,
	/* '^' */ 0x5E,   7, MO_AltRight,
	/* '_' */ 0x5F,  55, MO_ShiftLeft,
	/* 'y' */ 0x79,  46, 0,
	/* 'z' */ 0x7A,  22, 0,
	/* '{' */ 0x7B,  10, MO_AltRight,
	/* '|' */ 0x7C,  45, MO_ShiftLeft,
	/* '}' */ 0x7D,  11, MO_AltRight,
	/* ??? */ 0x9A,   4, 0,	/* Not CP1252 */
	/* ??? */ 0x9D,   6, 0,	/* Not CP1252 */
	/* ??? */ 0x9E,   7, 0,	/* Not CP1252 */
	/* ??? */ 0xA1,  13, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xA7,  41, 0,	/* Not CP1252 */
	/* ??? */ 0xA8,  13, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xB4,  13, 0,	/* Not CP1252 */
	/* ??? */ 0xBE,   3, 0,	/* Not CP1252 */
	/* ??? */ 0xE1,   9, 0,	/* Not CP1252 */
	/* ??? */ 0xE4,  28, 0,	/* Not CP1252 */
	/* ??? */ 0xE8,   5, 0,	/* Not CP1252 */
	/* ??? */ 0xE9,  11, 0,	/* Not CP1252 */
	/* ??? */ 0xED,  10, 0,	/* Not CP1252 */
	/* ??? */ 0xF2,  42, 0,	/* Not CP1252 */
	/* ??? */ 0xF4,  40, 0,	/* Not CP1252 */
	/* ??? */ 0xFA,  27, 0,	/* Not CP1252 */
	/* ??? */ 0xFD,   8, 0,	/* Not CP1252 */
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	60 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		240 (0xF0) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		180 (0xB4) bytes
*/

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_SWED[38] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '"' */ 0x22,   3, MO_ShiftLeft,
	/* '#' */ 0x23,  28, MO_AltRight,
	/* '&' */ 0x26,   7, MO_ShiftLeft,
	/* ''' */ 0x27,  42, 0,
	/* '(' */ 0x28,   9, MO_ShiftLeft,
	/* ')' */ 0x29,  10, MO_ShiftLeft,
	/* '*' */ 0x2A,  42, MO_ShiftLeft,
	/* '+' */ 0x2B,  12, 0,
	/* '-' */ 0x2D,  55, 0,
	/* '/' */ 0x2F,   8, MO_ShiftLeft,
	/* ':' */ 0x3A,  54, MO_ShiftLeft,
	/* ';' */ 0x3B,  53, MO_ShiftLeft,
	/* '<' */ 0x3C,  45, 0,
	/* '=' */ 0x3D,  11, MO_ShiftLeft,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '?' */ 0x3F,  12, MO_ShiftLeft,
	/* '@' */ 0x40,   4, MO_ShiftLeft|MO_AltRight,
	/* '[' */ 0x5B, NOS, 0,
	/* '\' */ 0x5C,  28, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x5D,  27, MO_ShiftLeft|MO_AltRight,
	/* '^' */ 0x5E,  13, 0,
	/* '_' */ 0x5F,  55, MO_ShiftLeft,
	/* '`' */ 0x60,  13, MO_ShiftLeft,
	/* '{' */ 0x7B,  40, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x7C,  41, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x7D,  27, MO_AltRight,
	/* '~' */ 0x7E,   1, 0,
	/* '§' */ 0xA7,   4, MO_ShiftLeft,
	/* '°' */ 0xB0,   1, MO_ShiftLeft,
	/* 'Ä' */ 0xC4,  41, MO_ShiftLeft,
	/* 'Å' */ 0xC5,  27, MO_ShiftLeft,
	/* 'Ö' */ 0xD6,  40, MO_ShiftLeft,
	/* 'Ü' */ 0xDC,  28, MO_ShiftLeft,
	/* 'ä' */ 0xE4,  41, 0,
	/* 'å' */ 0xE5,  27, 0,
	/* 'ö' */ 0xF6,  40, 0,
	/* 'ü' */ 0xFC,  28, 0,
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	38 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		152 (0x98) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		114 (0x72) bytes
*/

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_TURQ[37] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '"' */ 0x22,  28, MO_ShiftLeft,
	/* '#' */ 0x23,  12, MO_ShiftLeft|MO_AltRight,
	/* '&' */ 0x26,  13, MO_ShiftLeft|MO_AltRight,
	/* ''' */ 0x27,   3, MO_ShiftLeft,
	/* '+' */ 0x2B,  42, MO_ShiftLeft,
	/* ',' */ 0x2C,  55, 0,
	/* '-' */ 0x2D,  55, MO_ShiftLeft,
	/* '/' */ 0x2F,   4, MO_ShiftLeft,
	/* ':' */ 0x3A,  27, MO_ShiftLeft,
	/* ';' */ 0x3B,  27, 0,
	/* '<' */ 0x3C,  45, 0,
	/* '=' */ 0x3D,  42, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '?' */ 0x3F,  54, MO_ShiftLeft,
	/* '@' */ 0x40,   1, MO_AltRight,
	/* '[' */ 0x5B,  40, MO_AltRight,
	/* '\' */ 0x5C,  53, MO_AltRight,
	/* ']' */ 0x5D,  41, MO_AltRight,
	/* '_' */ 0x5F,   8, MO_ShiftLeft,
	/* '`' */ 0x60, NOS, 0,
	/* '{' */ 0x7B,  40, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x7C,  53, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x7D,  41, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x7E,   1, MO_ShiftLeft|MO_AltRight,
	/* ??? */ 0xC7,  41, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xD0,   1, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xD6,  13, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xDC,  12, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xDD,  53, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xDE,  40, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xE7,  41, 0,	/* Not CP1252 */
	/* ??? */ 0xF0,   1, 0,	/* Not CP1252 */
	/* ??? */ 0xF6,  13, 0,	/* Not CP1252 */
	/* ??? */ 0xFC,  12, 0,	/* Not CP1252 */
	/* ??? */ 0xFD,  53, 0,	/* Not CP1252 */
	/* ??? */ 0xFE,  40, 0,	/* Not CP1252 */
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	37 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		148 (0x94) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		111 (0x6F) bytes
*/

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_ROMA[21] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '<' */ 0x3C,  54, MO_ShiftLeft,
	/* '>' */ 0x3E,  53, MO_ShiftLeft,
	/* '[' */ 0x5B,  27, MO_AltRight,
	/* '\' */ 0x5C,  42, MO_AltRight,
	/* ']' */ 0x5D,  28, MO_AltRight,
	/* '`' */ 0x60,   1, MO_AltRight,
	/* '{' */ 0x7B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x7C,  42, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x7D,  28, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x7E,   1, MO_ShiftLeft|MO_AltRight,
	/* ??? */ 0xAA,  45, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xBA,  45, 0,	/* Not CP1252 */
	/* ??? */ 0xC2,   1, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xC3,  27, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xCE,  28, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xDE,  41, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xE2,   1, 0,	/* Not CP1252 */
	/* ??? */ 0xE3,  27, 0,	/* Not CP1252 */
	/* ??? */ 0xEE,  28, 0,	/* Not CP1252 */
	/* ??? */ 0xFE,  41, MO_AltRight,	/* Not CP1252 */
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	21 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		84 (0x54) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		63 (0x3F) bytes
*/

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_RUSS[77] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '"' */ 0x22,   3, MO_AltRight,
	/* ''' */ 0x27,   7, MO_AltRight,
	/* '/' */ 0x2F,   4, MO_AltRight,
	/* ':' */ 0x3A,   6, MO_AltRight,
	/* ';' */ 0x3B,   9, MO_AltRight,
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '?' */ 0x3F,  10, MO_AltRight,
	/* ??? */ 0xA7,  12, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xA8,  55, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xB8,  55, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xB9,   5, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xC0,  34, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xC1,  53, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xC2,  33, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xC3,  23, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xC4,  39, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xC5,  21, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xC6,  40, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xC7,  26, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xC8,  50, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xC9,  17, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xCA,  20, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xCB,  38, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xCC,  49, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xCD,  22, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xCE,  37, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xCF,  35, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xD0,  36, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xD1,  48, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xD2,  51, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xD3,  19, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xD4,  31, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xD5,  27, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xD6,  18, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xD7,  47, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xD8,  24, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xD9,  25, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xDA,  28, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xDB,  32, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xDC,  52, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xDD,  41, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xDE,  54, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xDF,  46, MO_ShiftLeft|MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xE0,  34, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xE1,  53, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xE2,  33, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xE3,  23, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xE4,  39, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xE5,  21, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xE6,  40, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xE7,  26, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xE8,  50, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xE9,  17, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xEA,  20, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xEB,  38, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xEC,  49, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xED,  22, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xEE,  37, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xEF,  35, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xF0,  36, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xF1,  48, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xF2,  51, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xF3,  19, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xF4,  31, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xF5,  27, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xF6,  18, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xF7,  47, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xF8,  24, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xF9,  25, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xFA,  28, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xFB,  32, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xFC,  52, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xFD,  41, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xFE,  54, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xFF,  46, MO_AltRight,	/* Not CP1252 */
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	77 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		308 (0x134) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		231 (0xE7) bytes
*/

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_TURF[91] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '"' */ 0x22,   4, MO_ShiftLeft,
	/* '#' */ 0x23,   1, MO_AltRight,
	/* '$' */ 0x24,  12, 0,
	/* '%' */ 0x25,  10, MO_ShiftLeft,
	/* '&' */ 0x26,  41, MO_AltRight,
	/* ''' */ 0x27,   5, MO_ShiftLeft,
	/* '(' */ 0x28,   6, MO_ShiftLeft,
	/* ')' */ 0x29,   8, MO_ShiftLeft,
	/* '*' */ 0x2A,   1, MO_ShiftLeft,
	/* '+' */ 0x2B,   1, 0,
	/* ',' */ 0x2C,  55, 0,
	/* '-' */ 0x2D,   9, MO_ShiftLeft,
	/* '/' */ 0x2F,   7, MO_ShiftLeft,
	/* ':' */ 0x3A,  11, MO_ShiftLeft,
	/* ';' */ 0x3B,  55, MO_ShiftLeft,
	/* '<' */ 0x3C,  45, 0,
	/* '>' */ 0x3E,  45, MO_ShiftLeft,
	/* '?' */ 0x3F,  54, MO_ShiftLeft,
	/* '@' */ 0x40,  12, MO_ShiftLeft,
	/* 'A' */ 0x41,  34, MO_ShiftLeft,
	/* 'B' */ 0x42,  53, MO_ShiftLeft,
	/* 'C' */ 0x43,  49, MO_ShiftLeft,
	/* 'D' */ 0x44,  22, MO_ShiftLeft,
	/* 'E' */ 0x45,  33, MO_ShiftLeft,
	/* 'F' */ 0x46,  17, MO_ShiftLeft,
	/* 'G' */ 0x47,  18, MO_ShiftLeft,
	/* 'H' */ 0x48,  25, MO_ShiftLeft,
	/* 'I' */ 0x49,  20, MO_ShiftLeft,
	/* 'J' */ 0x4A,  46, MO_ShiftLeft,
	/* 'K' */ 0x4B,  37, MO_ShiftLeft,
	/* 'M' */ 0x4D,  38, MO_ShiftLeft,
	/* 'N' */ 0x4E,  24, MO_ShiftLeft,
	/* 'O' */ 0x4F,  21, MO_ShiftLeft,
	/* 'Q' */ 0x51,  27, MO_ShiftLeft,
	/* 'R' */ 0x52,  23, MO_ShiftLeft,
	/* 'S' */ 0x53,  52, MO_ShiftLeft,
	/* 'T' */ 0x54,  36, MO_ShiftLeft,
	/* 'U' */ 0x55,  31, MO_ShiftLeft,
	/* 'V' */ 0x56,  48, MO_ShiftLeft,
	/* 'W' */ 0x57,  28, MO_ShiftLeft,
	/* 'X' */ 0x58,  42, MO_ShiftLeft,
	/* 'Y' */ 0x59,  40, MO_ShiftLeft,
	/* 'Z' */ 0x5A,  51, MO_ShiftLeft,
	/* '[' */ 0x5B,  47, MO_AltRight,
	/* '\' */ 0x5C,  32, MO_AltRight,
	/* ']' */ 0x5D,  50, MO_AltRight,
	/* '^' */ 0x5E,   3, MO_ShiftLeft,
	/* '_' */ 0x5F,  13, MO_ShiftLeft,
	/* '`' */ 0x60, NOS, 0,
	/* 'a' */ 0x61,  34, 0,
	/* 'b' */ 0x62,  53, 0,
	/* 'c' */ 0x63,  49, 0,
	/* 'd' */ 0x64,  22, 0,
	/* 'e' */ 0x65,  33, 0,
	/* 'f' */ 0x66,  17, 0,
	/* 'g' */ 0x67,  18, 0,
	/* 'h' */ 0x68,  25, 0,
	/* 'i' */ 0x69,  20, 0,
	/* 'j' */ 0x6A,  46, 0,
	/* 'k' */ 0x6B,  37, 0,
	/* 'm' */ 0x6D,  38, 0,
	/* 'n' */ 0x6E,  24, 0,
	/* 'o' */ 0x6F,  21, 0,
	/* 'q' */ 0x71,  27, 0,
	/* 'r' */ 0x72,  23, 0,
	/* 's' */ 0x73,  52, 0,
	/* 't' */ 0x74,  36, 0,
	/* 'u' */ 0x75,  31, 0,
	/* 'v' */ 0x76,  48, 0,
	/* 'w' */ 0x77,  28, 0,
	/* 'x' */ 0x78,  42, 0,
	/* 'y' */ 0x79,  40, 0,
	/* 'z' */ 0x7A,  51, 0,
	/* '{' */ 0x7B,  47, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x7C,  32, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x7D,  50, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x7E,   1, MO_ShiftLeft|MO_AltRight,
	/* ??? */ 0xA5,  22, MO_AltRight,	/* Not CP1252 */
	/* ??? */ 0xC7,  50, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xD0,  19, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xD6,  47, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xDC,  35, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xDD,  32, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xDE,  41, MO_ShiftLeft,	/* Not CP1252 */
	/* ??? */ 0xE7,  50, 0,	/* Not CP1252 */
	/* ??? */ 0xF0,  19, 0,	/* Not CP1252 */
	/* ??? */ 0xF6,  47, 0,	/* Not CP1252 */
	/* ??? */ 0xFC,  35, 0,	/* Not CP1252 */
	/* ??? */ 0xFD,  32, 0,	/* Not CP1252 */
	/* ??? */ 0xFE,  41, 0,	/* Not CP1252 */
	/* end */ 0,KN_ENDMARKER,0
};
/*
Memory
------
	91 exceptions + 0 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		364 (0x16C) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		273 (0x111) bytes
*/

/* TermId=104 */
const CKeyFixup DEC_VT510_LK411_JAPA[97] =
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

/* TermId=112 	,114 */
const CKeyFixup Telex_88_DATA_ENTRY_USA_[3] =
{
	/*        ANSI,  KN, MODIFIERS */
	/* '¢' */ 0xA2,  20, MO_ShiftLeft,
	/* '¬' */ 0xAC,  35, MO_ShiftLeft,
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
/* Telex_88_DATA_ENTRY_BELG is not supported */
/* Telex_88_DATA_ENTRY_FINN is not supported */
/* Telex_88_DATA_ENTRY_FRAN is not supported */
/* Telex_88_DATA_ENTRY_GERM is not supported */
/* Telex_88_DATA_ENTRY_ITAL is not supported */
/* Telex_88_DATA_ENTRY_SWIT is not supported */
/* Telex_88_DATA_ENTRY_UKIN is not supported */
/* Telex_88_DATA_ENTRY_DENM is not supported */
/* Telex_88_DATA_ENTRY_NORW is not supported */
/* Telex_88_DATA_ENTRY_SPAI is not supported */
/* Telex_88_DATA_ENTRY_DUTC is not supported */
/* Telex_88_DATA_ENTRY_HEBR is not supported */
/* Telex_88_DATA_ENTRY_PORT is not supported */
/* Telex_88_DATA_ENTRY_LATI is not supported */
/* Telex_88_DATA_ENTRY_CZEC is not supported */
/* Telex_88_DATA_ENTRY_BRAZ is not supported */
/* Telex_88_DATA_ENTRY_GREK is not supported */
/* Telex_88_DATA_ENTRY_FCAN is not supported */
/* Telex_88_DATA_ENTRY_HUNG is not supported */
/* Telex_88_DATA_ENTRY_POLI is not supported */
/* Telex_88_DATA_ENTRY_SCS_ is not supported */
/* Telex_88_DATA_ENTRY_SLOV is not supported */
/* Telex_88_DATA_ENTRY_SWED is not supported */
/* Telex_88_DATA_ENTRY_TURQ is not supported */
/* Telex_88_DATA_ENTRY_ROMA is not supported */
/* Telex_88_DATA_ENTRY_RUSS is not supported */
/* Telex_88_DATA_ENTRY_TURF is not supported */
/* Telex_88_DATA_ENTRY_JAPA is not supported */

/* Forward declarations for pointer tables */
extern const CKeyFixups IBM_XT_FixupPtr;
extern const CKeyFixups IBM_AT_FixupPtr;
extern const CKeyFixups DEC_VT420_FixupPtr;
extern const CKeyFixups DEC_VT510_PC_FixupPtr;
extern const CKeyFixups IBM_3486_102_FixupPtr;
extern const CKeyFixups IBM_3191_122_FixupPtr;
extern const CKeyFixups IBM_3486_122_FixupPtr;
extern const CKeyFixups Telex_88_FixupPtr;
extern const CKeyFixups BDS_7_FixupPtr;
extern const CKeyFixups APPLE_ADB_FixupPtr;
extern const CKeyFixups DEC_VT510_LK411_FixupPtr;
extern const CKeyFixups Telex_88_DATA_ENTRY_FixupPtr;

/******************************************************/
/* Pointer tables                                     */
/******************************************************/

/* TermId=  1 */
const CKeyFixups IBM_XT_FixupPtr =
{
	&BDS_7_FixupPtr,              		/* pointer to parent table */
	NoExceptions,                 		/* US not required */
	IBM_XT_BELG,                  		/* BELGIUM */
	IBM_XT_FINN,                  		/* FINNLAND (SWEDEN) */
	IBM_XT_FRAN,                  		/* FRANCE */
	IBM_XT_GERM,                  		/* GERMANY */
	IBM_XT_ITAL,                  		/* ITALY */
	IBM_XT_SWIT,                  		/* SWITZERLAND (German) */
	IBM_XT_UKIN,                  		/* UK */
	IBM_XT_DENM,                  		/* DENMARK */
	IBM_XT_NORW,                  		/* NORWAY */
	IBM_XT_SPAI,                  		/* SPAIN */
	NULL,                         		/* DUTCH not supported */
	NULL,                         		/* HEBREW not supported */
	NULL,                         		/* PORTUGAL not supported */
	NULL,                         		/* LATIN AMERICA not supported */
	NULL,                         		/* CZECH not supported */
	NULL,                         		/* BRAZILIAN not supported */
	NULL,                         		/* GREEK not supported */
	NULL,                         		/* FRENCH CANADIAN not supported */
	NULL,                         		/* HUNGARY not supported */
	NULL,                         		/* POLAND not supported */
	NULL,                         		/* SCS not supported */
	NULL,                         		/* SLOVAK not supported */
	NULL,                         		/* SWEDEN not supported */
	NULL,                         		/* TURKEY Q not supported */
	NULL,                         		/* ROMANIA not supported */
	NULL,                         		/* RUSSIAN not supported */
	NULL,                         		/* TURKEY F not supported */
	NULL,                         		/* JAPAN ASCII not supported */
};


/* TermId=  3 	,  2 , 97 ,106 ,124 */
const CKeyFixups IBM_AT_FixupPtr =
{
	&DEC_VT510_PC_FixupPtr,       		/* pointer to parent table */
	NoExceptions,                 		/* US not required */
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
	IBM_AT_JAPA,                  		/* JAPAN ASCII */
};


/* TermId=  4 */
const CKeyFixups DEC_VT420_FixupPtr =
{
	&DEC_VT510_LK411_FixupPtr,    		/* pointer to parent table */
	NoExceptions,                 		/* US not required */
	DEC_VT420_BELG,               		/* BELGIUM */
	DEC_VT420_FINN,               		/* FINNLAND (SWEDEN) */
	DEC_VT420_FRAN,               		/* FRANCE */
	DEC_VT420_GERM,               		/* GERMANY */
	DEC_VT420_ITAL,               		/* ITALY */
	DEC_VT420_SWIT,               		/* SWITZERLAND (German) */
	DEC_VT420_UKIN,               		/* UK */
	DEC_VT420_DENM,               		/* DENMARK */
	DEC_VT420_NORW,               		/* NORWAY */
	DEC_VT420_SPAI,               		/* SPAIN */
	DEC_VT420_DUTC,               		/* DUTCH */
	DEC_VT420_HEBR,               		/* HEBREW */
	DEC_VT420_PORT,               		/* PORTUGAL */
	DEC_VT420_LATI,               		/* LATIN AMERICA */
	DEC_VT420_CZEC,               		/* CZECH */
	DEC_VT420_BRAZ,               		/* BRAZILIAN */
	NULL,                         		/* GREEK not supported */
	DEC_VT420_FCAN,               		/* FRENCH CANADIAN */
	DEC_VT420_HUNG,               		/* HUNGARY */
	DEC_VT420_POLI,               		/* POLAND */
	DEC_VT420_SCS_,               		/* SCS */
	DEC_VT420_SLOV,               		/* SLOVAK */
	DEC_VT420_SWED,               		/* SWEDEN */
	DEC_VT420_TURQ,               		/* TURKEY Q */
	DEC_VT420_ROMA,               		/* ROMANIA */
	DEC_VT420_RUSS,               		/* RUSSIAN */
	DEC_VT420_TURF,               		/* TURKEY F */
	DEC_VT420_JAPA,               		/* JAPAN ASCII */
};


/* TermId=  5 */
const CKeyFixups DEC_VT510_PC_FixupPtr =
{
	&DEC_VT510_LK411_FixupPtr,    		/* pointer to parent table */
	NoExceptions,                 		/* US not required */
	DEC_VT510_PC_BELG,            		/* BELGIUM */
	DEC_VT510_PC_FINN,            		/* FINNLAND (SWEDEN) */
	DEC_VT510_PC_FRAN,            		/* FRANCE */
	DEC_VT510_PC_GERM,            		/* GERMANY */
	DEC_VT510_PC_ITAL,            		/* ITALY */
	DEC_VT510_PC_SWIT,            		/* SWITZERLAND (German) */
	DEC_VT510_PC_UKIN,            		/* UK */
	DEC_VT510_PC_DENM,            		/* DENMARK */
	DEC_VT510_PC_NORW,            		/* NORWAY */
	DEC_VT510_PC_SPAI,            		/* SPAIN */
	DEC_VT510_PC_DUTC,            		/* DUTCH */
	DEC_VT510_PC_HEBR,            		/* HEBREW */
	DEC_VT510_PC_PORT,            		/* PORTUGAL */
	DEC_VT510_PC_LATI,            		/* LATIN AMERICA */
	DEC_VT510_PC_CZEC,            		/* CZECH */
	DEC_VT510_PC_BRAZ,            		/* BRAZILIAN */
	NULL,                         		/* GREEK not supported */
	DEC_VT510_PC_FCAN,            		/* FRENCH CANADIAN */
	DEC_VT510_PC_HUNG,            		/* HUNGARY */
	DEC_VT510_PC_POLI,            		/* POLAND */
	DEC_VT510_PC_SCS_,            		/* SCS */
	DEC_VT510_PC_SLOV,            		/* SLOVAK */
	DEC_VT510_PC_SWED,            		/* SWEDEN */
	DEC_VT510_PC_TURQ,            		/* TURKEY Q */
	DEC_VT510_PC_ROMA,            		/* ROMANIA */
	DEC_VT510_PC_RUSS,            		/* RUSSIAN */
	DEC_VT510_PC_TURF,            		/* TURKEY F */
	DEC_VT510_PC_JAPA,            		/* JAPAN ASCII */
};


/* TermId=  6 	, 45 , 84 */
const CKeyFixups IBM_3486_102_FixupPtr =
{
	&IBM_AT_FixupPtr,             		/* pointer to parent table */
	NoExceptions,                 		/* US not required */
	IBM_3486_102_BELG,            		/* BELGIUM */
	IBM_3486_102_FINN,            		/* FINNLAND (SWEDEN) */
	IBM_3486_102_FRAN,            		/* FRANCE */
	IBM_3486_102_GERM,            		/* GERMANY */
	IBM_3486_102_ITAL,            		/* ITALY */
	IBM_3486_102_SWIT,            		/* SWITZERLAND (German) */
	IBM_3486_102_UKIN,            		/* UK */
	IBM_3486_102_DENM,            		/* DENMARK */
	IBM_3486_102_NORW,            		/* NORWAY */
	IBM_3486_102_SPAI,            		/* SPAIN */
	NULL,                         		/* DUTCH not supported */
	NULL,                         		/* HEBREW not supported */
	NULL,                         		/* PORTUGAL not supported */
	IBM_3486_102_LATI,            		/* LATIN AMERICA */
	NULL,                         		/* CZECH not supported */
	NULL,                         		/* BRAZILIAN not supported */
	NULL,                         		/* GREEK not supported */
	NULL,                         		/* FRENCH CANADIAN not supported */
	NULL,                         		/* HUNGARY not supported */
	NULL,                         		/* POLAND not supported */
	NULL,                         		/* SCS not supported */
	NULL,                         		/* SLOVAK not supported */
	NULL,                         		/* SWEDEN not supported */
	NULL,                         		/* TURKEY Q not supported */
	NULL,                         		/* ROMANIA not supported */
	NULL,                         		/* RUSSIAN not supported */
	NULL,                         		/* TURKEY F not supported */
	NULL,                         		/* JAPAN ASCII not supported */
};


/* TermId=  7 	, 24 , 46 , 71 */
const CKeyFixups IBM_3191_122_FixupPtr =
{
	&IBM_3486_122_FixupPtr,       		/* pointer to parent table */
	NoExceptions,                 		/* US not required */
	IBM_3191_122_BELG,            		/* BELGIUM */
	NoExceptions,                 		/* FINNLAND (SWEDEN) not required */
	NoExceptions,                 		/* FRANCE not required */
	NoExceptions,                 		/* GERMANY not required */
	NoExceptions,                 		/* ITALY not required */
	IBM_3191_122_SWIT,            		/* SWITZERLAND (German) */
	NoExceptions,                 		/* UK not required */
	NoExceptions,                 		/* DENMARK not required */
	NoExceptions,                 		/* NORWAY not required */
	NoExceptions,                 		/* SPAIN not required */
	NULL,                         		/* DUTCH not supported */
	NULL,                         		/* HEBREW not supported */
	NULL,                         		/* PORTUGAL not supported */
	IBM_3191_122_LATI,            		/* LATIN AMERICA */
	NULL,                         		/* CZECH not supported */
	NULL,                         		/* BRAZILIAN not supported */
	NULL,                         		/* GREEK not supported */
	NULL,                         		/* FRENCH CANADIAN not supported */
	NULL,                         		/* HUNGARY not supported */
	NULL,                         		/* POLAND not supported */
	NULL,                         		/* SCS not supported */
	NULL,                         		/* SLOVAK not supported */
	NULL,                         		/* SWEDEN not supported */
	NULL,                         		/* TURKEY Q not supported */
	NULL,                         		/* ROMANIA not supported */
	NULL,                         		/* RUSSIAN not supported */
	NULL,                         		/* TURKEY F not supported */
	NULL,                         		/* JAPAN ASCII not supported */
};


/* TermId=  8 	, 72 */
const CKeyFixups IBM_3486_122_FixupPtr =
{
	&IBM_3486_102_FixupPtr,       		/* pointer to parent table */
	IBM_3486_122_USA_,            		/* US */
	IBM_3486_122_BELG,            		/* BELGIUM */
	IBM_3486_122_FINN,            		/* FINNLAND (SWEDEN) */
	IBM_3486_122_FRAN,            		/* FRANCE */
	IBM_3486_122_GERM,            		/* GERMANY */
	IBM_3486_122_ITAL,            		/* ITALY */
	IBM_3486_122_SWIT,            		/* SWITZERLAND (German) */
	IBM_3486_122_UKIN,            		/* UK */
	IBM_3486_122_DENM,            		/* DENMARK */
	IBM_3486_122_NORW,            		/* NORWAY */
	IBM_3486_122_SPAI,            		/* SPAIN */
	NULL,                         		/* DUTCH not supported */
	NULL,                         		/* HEBREW not supported */
	NULL,                         		/* PORTUGAL not supported */
	IBM_3486_122_LATI,            		/* LATIN AMERICA */
	NULL,                         		/* CZECH not supported */
	NULL,                         		/* BRAZILIAN not supported */
	NULL,                         		/* GREEK not supported */
	NULL,                         		/* FRENCH CANADIAN not supported */
	NULL,                         		/* HUNGARY not supported */
	NULL,                         		/* POLAND not supported */
	NULL,                         		/* SCS not supported */
	NULL,                         		/* SLOVAK not supported */
	NULL,                         		/* SWEDEN not supported */
	NULL,                         		/* TURKEY Q not supported */
	NULL,                         		/* ROMANIA not supported */
	NULL,                         		/* RUSSIAN not supported */
	NULL,                         		/* TURKEY F not supported */
	NULL,                         		/* JAPAN ASCII not supported */
};


/* TermId= 25 */
const CKeyFixups Telex_88_FixupPtr =
{
	&IBM_3486_122_FixupPtr,       		/* pointer to parent table */
	NoExceptions,                 		/* US not required */
	Telex_88_BELG,                		/* BELGIUM */
	Telex_88_FINN,                		/* FINNLAND (SWEDEN) */
	Telex_88_FRAN,                		/* FRANCE */
	Telex_88_GERM,                		/* GERMANY */
	Telex_88_ITAL,                		/* ITALY */
	Telex_88_SWIT,                		/* SWITZERLAND (German) */
	Telex_88_UKIN,                		/* UK */
	Telex_88_DENM,                		/* DENMARK */
	Telex_88_NORW,                		/* NORWAY */
	Telex_88_SPAI,                		/* SPAIN */
	NULL,                         		/* DUTCH not supported */
	NULL,                         		/* HEBREW not supported */
	NULL,                         		/* PORTUGAL not supported */
	NULL,                         		/* LATIN AMERICA not supported */
	NULL,                         		/* CZECH not supported */
	NULL,                         		/* BRAZILIAN not supported */
	NULL,                         		/* GREEK not supported */
	NULL,                         		/* FRENCH CANADIAN not supported */
	NULL,                         		/* HUNGARY not supported */
	NULL,                         		/* POLAND not supported */
	NULL,                         		/* SCS not supported */
	NULL,                         		/* SLOVAK not supported */
	NULL,                         		/* SWEDEN not supported */
	NULL,                         		/* TURKEY Q not supported */
	NULL,                         		/* ROMANIA not supported */
	NULL,                         		/* RUSSIAN not supported */
	NULL,                         		/* TURKEY F not supported */
	NULL,                         		/* JAPAN ASCII not supported */
};


/* TermId= 35 */
const CKeyFixups BDS_7_FixupPtr =
{
	&IBM_AT_FixupPtr,             		/* pointer to parent table */
	NoExceptions,                 		/* US not required */
	BDS_7_BELG,                   		/* BELGIUM */
	BDS_7_FINN,                   		/* FINNLAND (SWEDEN) */
	BDS_7_FRAN,                   		/* FRANCE */
	BDS_7_GERM,                   		/* GERMANY */
	BDS_7_ITAL,                   		/* ITALY */
	BDS_7_SWIT,                   		/* SWITZERLAND (German) */
	BDS_7_UKIN,                   		/* UK */
	BDS_7_DENM,                   		/* DENMARK */
	BDS_7_NORW,                   		/* NORWAY */
	BDS_7_SPAI,                   		/* SPAIN */
	NULL,                         		/* DUTCH not supported */
	NULL,                         		/* HEBREW not supported */
	NULL,                         		/* PORTUGAL not supported */
	NULL,                         		/* LATIN AMERICA not supported */
	NULL,                         		/* CZECH not supported */
	NULL,                         		/* BRAZILIAN not supported */
	NULL,                         		/* GREEK not supported */
	NULL,                         		/* FRENCH CANADIAN not supported */
	NULL,                         		/* HUNGARY not supported */
	NULL,                         		/* POLAND not supported */
	NULL,                         		/* SCS not supported */
	NULL,                         		/* SLOVAK not supported */
	NULL,                         		/* SWEDEN not supported */
	NULL,                         		/* TURKEY Q not supported */
	NULL,                         		/* ROMANIA not supported */
	NULL,                         		/* RUSSIAN not supported */
	NULL,                         		/* TURKEY F not supported */
	NULL,                         		/* JAPAN ASCII not supported */
};


/* TermId= 49 	,125 */
const CKeyFixups APPLE_ADB_FixupPtr =
{
	&DEC_VT510_LK411_FixupPtr,    		/* pointer to parent table */
	APPLE_ADB_USA_,               		/* US */
	APPLE_ADB_BELG,               		/* BELGIUM */
	APPLE_ADB_FINN,               		/* FINNLAND (SWEDEN) */
	APPLE_ADB_FRAN,               		/* FRANCE */
	APPLE_ADB_GERM,               		/* GERMANY */
	APPLE_ADB_ITAL,               		/* ITALY */
	APPLE_ADB_SWIT,               		/* SWITZERLAND (German) */
	APPLE_ADB_UKIN,               		/* UK */
	APPLE_ADB_DENM,               		/* DENMARK */
	APPLE_ADB_NORW,               		/* NORWAY */
	APPLE_ADB_SPAI,               		/* SPAIN */
	NULL,                         		/* DUTCH not supported */
	NULL,                         		/* HEBREW not supported */
	NULL,                         		/* PORTUGAL not supported */
	APPLE_ADB_LATI,               		/* LATIN AMERICA */
	NULL,                         		/* CZECH not supported */
	NULL,                         		/* BRAZILIAN not supported */
	NULL,                         		/* GREEK not supported */
	APPLE_ADB_FCAN,               		/* FRENCH CANADIAN */
	NULL,                         		/* HUNGARY not supported */
	NULL,                         		/* POLAND not supported */
	NULL,                         		/* SCS not supported */
	NULL,                         		/* SLOVAK not supported */
	NULL,                         		/* SWEDEN not supported */
	NULL,                         		/* TURKEY Q not supported */
	NULL,                         		/* ROMANIA not supported */
	NULL,                         		/* RUSSIAN not supported */
	NULL,                         		/* TURKEY F not supported */
	NULL,                         		/* JAPAN ASCII not supported */
};


/* TermId=104 */
const CKeyFixups DEC_VT510_LK411_FixupPtr =
{
	NULL,                         		/* pointer to parent table (none here) */
	NoExceptions,                 		/* US not required */
	DEC_VT510_LK411_BELG,         		/* BELGIUM */
	DEC_VT510_LK411_FINN,         		/* FINNLAND (SWEDEN) */
	DEC_VT510_LK411_FRAN,         		/* FRANCE */
	DEC_VT510_LK411_GERM,         		/* GERMANY */
	DEC_VT510_LK411_ITAL,         		/* ITALY */
	DEC_VT510_LK411_SWIT,         		/* SWITZERLAND (German) */
	DEC_VT510_LK411_UKIN,         		/* UK */
	DEC_VT510_LK411_DENM,         		/* DENMARK */
	DEC_VT510_LK411_NORW,         		/* NORWAY */
	DEC_VT510_LK411_SPAI,         		/* SPAIN */
	DEC_VT510_LK411_DUTC,         		/* DUTCH */
	DEC_VT510_LK411_HEBR,         		/* HEBREW */
	DEC_VT510_LK411_PORT,         		/* PORTUGAL */
	DEC_VT510_LK411_LATI,         		/* LATIN AMERICA */
	DEC_VT510_LK411_CZEC,         		/* CZECH */
	DEC_VT510_LK411_BRAZ,         		/* BRAZILIAN */
	NULL,                         		/* GREEK not supported */
	DEC_VT510_LK411_FCAN,         		/* FRENCH CANADIAN */
	DEC_VT510_LK411_HUNG,         		/* HUNGARY */
	DEC_VT510_LK411_POLI,         		/* POLAND */
	DEC_VT510_LK411_SCS_,         		/* SCS */
	DEC_VT510_LK411_SLOV,         		/* SLOVAK */
	DEC_VT510_LK411_SWED,         		/* SWEDEN */
	DEC_VT510_LK411_TURQ,         		/* TURKEY Q */
	DEC_VT510_LK411_ROMA,         		/* ROMANIA */
	DEC_VT510_LK411_RUSS,         		/* RUSSIAN */
	DEC_VT510_LK411_TURF,         		/* TURKEY F */
	DEC_VT510_LK411_JAPA,         		/* JAPAN ASCII */
};


/* TermId=112 	,114 */
const CKeyFixups Telex_88_DATA_ENTRY_FixupPtr =
{
	NULL,                         		/* pointer to parent table (none here) */
	Telex_88_DATA_ENTRY_USA_,     		/* US */
	NULL,                         		/* BELGIUM not supported */
	NULL,                         		/* FINNLAND (SWEDEN) not supported */
	NULL,                         		/* FRANCE not supported */
	NULL,                         		/* GERMANY not supported */
	NULL,                         		/* ITALY not supported */
	NULL,                         		/* SWITZERLAND (German) not supported */
	NULL,                         		/* UK not supported */
	NULL,                         		/* DENMARK not supported */
	NULL,                         		/* NORWAY not supported */
	NULL,                         		/* SPAIN not supported */
	NULL,                         		/* DUTCH not supported */
	NULL,                         		/* HEBREW not supported */
	NULL,                         		/* PORTUGAL not supported */
	NULL,                         		/* LATIN AMERICA not supported */
	NULL,                         		/* CZECH not supported */
	NULL,                         		/* BRAZILIAN not supported */
	NULL,                         		/* GREEK not supported */
	NULL,                         		/* FRENCH CANADIAN not supported */
	NULL,                         		/* HUNGARY not supported */
	NULL,                         		/* POLAND not supported */
	NULL,                         		/* SCS not supported */
	NULL,                         		/* SLOVAK not supported */
	NULL,                         		/* SWEDEN not supported */
	NULL,                         		/* TURKEY Q not supported */
	NULL,                         		/* ROMANIA not supported */
	NULL,                         		/* RUSSIAN not supported */
	NULL,                         		/* TURKEY F not supported */
	NULL,                         		/* JAPAN ASCII not supported */
};

/*
Supported terminals:
--------------------
 	 ID	Name
 	  1	XT
 	  2	PS2
 	  3	AT
 	  4	DEC 420
 	  5	DEC 510 PC keyboard
 	  6	IBM102
 	  7	IBM 3191 (122)
 	  8	IBM 3486 (122)
 	 24	IBM 3180 (122)
 	 25	Telex (88)
 	 35	BDS 7
 	 45	Telex (102)
 	 46	Telex (122)
 	 49	Apple ADB
 	 71	DDC 3496 / Nokia/ICL 472 (122)
 	 72	ADI 1496
 	 84	DDC 3496 (102)
 	 97	IBM Thinkpad PS/2
 	104	DEC 510 LK411 keyboard
 	106	IBM Thinkpad AT
 	112	Telex (88) DataEntry
 	114	IBM 3180 (122) DataEntry
 	124	USB EPC keyboard
 	125	Apple USB keyboard
 A total of 24 terminals


Some terminals use a table of an other terminal:
------------------------------------------------
    Unique  Equal		Name
		  3				AT
				  2		PS2
				 97		IBM Thinkpad PS/2
				106		IBM Thinkpad AT
				124		USB EPC keyboard

		  7				IBM 3191 (122)
				 24		IBM 3180 (122)
				 46		Telex (122)
				 71		DDC 3496 / Nokia/ICL 472 (122)

		  6				IBM102
				 45		Telex (102)
				 84		DDC 3496 (102)

		112				Telex (88) DataEntry
				114		IBM 3180 (122) DataEntry

		  8				IBM 3486 (122)
				 72		ADI 1496

		 49				Apple ADB
				125		Apple USB keyboard




These exception tables are same as others:
--------------------------------------------


Full list of supported terminals and countries (sorted by TermId):
------------------------------------------------------------------

IBM_XT (TermId = 1)
	US
	BELGIUM
	FINNLAND (SWEDEN)
	FRANCE
	GERMANY
	ITALY
	SWITZERLAND (German)
	UK
	DENMARK
	NORWAY
	SPAIN


IBM_PS2 (TermId = 2)
	US
	BELGIUM
	FINNLAND (SWEDEN)
	FRANCE
	GERMANY
	ITALY
	SWITZERLAND (German)
	UK
	DENMARK
	NORWAY
	SPAIN
	DUTCH
	HEBREW
	PORTUGAL
	LATIN AMERICA
	CZECH
	BRAZILIAN
	FRENCH CANADIAN
	HUNGARY
	POLAND
	SCS
	SLOVAK
	SWEDEN
	TURKEY Q
	ROMANIA
	RUSSIAN
	TURKEY F
	JAPAN ASCII


IBM_AT (TermId = 3)
	US
	BELGIUM
	FINNLAND (SWEDEN)
	FRANCE
	GERMANY
	ITALY
	SWITZERLAND (German)
	UK
	DENMARK
	NORWAY
	SPAIN
	DUTCH
	HEBREW
	PORTUGAL
	LATIN AMERICA
	CZECH
	BRAZILIAN
	FRENCH CANADIAN
	HUNGARY
	POLAND
	SCS
	SLOVAK
	SWEDEN
	TURKEY Q
	ROMANIA
	RUSSIAN
	TURKEY F
	JAPAN ASCII


DEC_VT420 (TermId = 4)
	US
	BELGIUM
	FINNLAND (SWEDEN)
	FRANCE
	GERMANY
	ITALY
	SWITZERLAND (German)
	UK
	DENMARK
	NORWAY
	SPAIN
	DUTCH
	HEBREW
	PORTUGAL
	LATIN AMERICA
	CZECH
	BRAZILIAN
	FRENCH CANADIAN
	HUNGARY
	POLAND
	SCS
	SLOVAK
	SWEDEN
	TURKEY Q
	ROMANIA
	RUSSIAN
	TURKEY F
	JAPAN ASCII


DEC_VT510_PC (TermId = 5)
	US
	BELGIUM
	FINNLAND (SWEDEN)
	FRANCE
	GERMANY
	ITALY
	SWITZERLAND (German)
	UK
	DENMARK
	NORWAY
	SPAIN
	DUTCH
	HEBREW
	PORTUGAL
	LATIN AMERICA
	CZECH
	BRAZILIAN
	FRENCH CANADIAN
	HUNGARY
	POLAND
	SCS
	SLOVAK
	SWEDEN
	TURKEY Q
	ROMANIA
	RUSSIAN
	TURKEY F
	JAPAN ASCII


IBM_3486_102 (TermId = 6)
	US
	BELGIUM
	FINNLAND (SWEDEN)
	FRANCE
	GERMANY
	ITALY
	SWITZERLAND (German)
	UK
	DENMARK
	NORWAY
	SPAIN
	LATIN AMERICA


IBM_3191_122 (TermId = 7)
	US
	BELGIUM
	FINNLAND (SWEDEN)
	FRANCE
	GERMANY
	ITALY
	SWITZERLAND (German)
	UK
	DENMARK
	NORWAY
	SPAIN
	LATIN AMERICA


IBM_3486_122 (TermId = 8)
	US
	BELGIUM
	FINNLAND (SWEDEN)
	FRANCE
	GERMANY
	ITALY
	SWITZERLAND (German)
	UK
	DENMARK
	NORWAY
	SPAIN
	LATIN AMERICA


IBM_3180 (TermId = 24)
	US
	BELGIUM
	FINNLAND (SWEDEN)
	FRANCE
	GERMANY
	ITALY
	SWITZERLAND (German)
	UK
	DENMARK
	NORWAY
	SPAIN
	LATIN AMERICA


Telex_88 (TermId = 25)
	US
	BELGIUM
	FINNLAND (SWEDEN)
	FRANCE
	GERMANY
	ITALY
	SWITZERLAND (German)
	UK
	DENMARK
	NORWAY
	SPAIN


BDS_7 (TermId = 35)
	US
	BELGIUM
	FINNLAND (SWEDEN)
	FRANCE
	GERMANY
	ITALY
	SWITZERLAND (German)
	UK
	DENMARK
	NORWAY
	SPAIN


Telex_102 (TermId = 45)
	US
	BELGIUM
	FINNLAND (SWEDEN)
	FRANCE
	GERMANY
	ITALY
	SWITZERLAND (German)
	UK
	DENMARK
	NORWAY
	SPAIN
	LATIN AMERICA


Telex_122 (TermId = 46)
	US
	BELGIUM
	FINNLAND (SWEDEN)
	FRANCE
	GERMANY
	ITALY
	SWITZERLAND (German)
	UK
	DENMARK
	NORWAY
	SPAIN
	LATIN AMERICA


APPLE_ADB (TermId = 49)
	US
	BELGIUM
	FINNLAND (SWEDEN)
	FRANCE
	GERMANY
	ITALY
	SWITZERLAND (German)
	UK
	DENMARK
	NORWAY
	SPAIN
	LATIN AMERICA
	FRENCH CANADIAN


DDC_3496_122 (TermId = 71)
	US
	BELGIUM
	FINNLAND (SWEDEN)
	FRANCE
	GERMANY
	ITALY
	SWITZERLAND (German)
	UK
	DENMARK
	NORWAY
	SPAIN
	LATIN AMERICA


ADI_1496 (TermId = 72)
	US
	BELGIUM
	FINNLAND (SWEDEN)
	FRANCE
	GERMANY
	ITALY
	SWITZERLAND (German)
	UK
	DENMARK
	NORWAY
	SPAIN
	LATIN AMERICA


DDC_3496_102 (TermId = 84)
	US
	BELGIUM
	FINNLAND (SWEDEN)
	FRANCE
	GERMANY
	ITALY
	SWITZERLAND (German)
	UK
	DENMARK
	NORWAY
	SPAIN
	LATIN AMERICA


IBM_THINKPAD_PS2 (TermId = 97)
	US
	BELGIUM
	FINNLAND (SWEDEN)
	FRANCE
	GERMANY
	ITALY
	SWITZERLAND (German)
	UK
	DENMARK
	NORWAY
	SPAIN
	DUTCH
	HEBREW
	PORTUGAL
	LATIN AMERICA
	CZECH
	BRAZILIAN
	FRENCH CANADIAN
	HUNGARY
	POLAND
	SCS
	SLOVAK
	SWEDEN
	TURKEY Q
	ROMANIA
	RUSSIAN
	TURKEY F
	JAPAN ASCII


DEC_VT510_LK411 (TermId = 104)
	US
	BELGIUM
	FINNLAND (SWEDEN)
	FRANCE
	GERMANY
	ITALY
	SWITZERLAND (German)
	UK
	DENMARK
	NORWAY
	SPAIN
	DUTCH
	HEBREW
	PORTUGAL
	LATIN AMERICA
	CZECH
	BRAZILIAN
	FRENCH CANADIAN
	HUNGARY
	POLAND
	SCS
	SLOVAK
	SWEDEN
	TURKEY Q
	ROMANIA
	RUSSIAN
	TURKEY F
	JAPAN ASCII


IBM_THINKPAD_AT (TermId = 106)
	US
	BELGIUM
	FINNLAND (SWEDEN)
	FRANCE
	GERMANY
	ITALY
	SWITZERLAND (German)
	UK
	DENMARK
	NORWAY
	SPAIN
	DUTCH
	HEBREW
	PORTUGAL
	LATIN AMERICA
	CZECH
	BRAZILIAN
	FRENCH CANADIAN
	HUNGARY
	POLAND
	SCS
	SLOVAK
	SWEDEN
	TURKEY Q
	ROMANIA
	RUSSIAN
	TURKEY F
	JAPAN ASCII


Telex_88_DATA_ENTRY (TermId = 112)
	US


IBM_3180_DATA_ENTRY (TermId = 114)
	US


USB_EPC_Keyboard (TermId = 124)
	US
	BELGIUM
	FINNLAND (SWEDEN)
	FRANCE
	GERMANY
	ITALY
	SWITZERLAND (German)
	UK
	DENMARK
	NORWAY
	SPAIN
	DUTCH
	HEBREW
	PORTUGAL
	LATIN AMERICA
	CZECH
	BRAZILIAN
	FRENCH CANADIAN
	HUNGARY
	POLAND
	SCS
	SLOVAK
	SWEDEN
	TURKEY Q
	ROMANIA
	RUSSIAN
	TURKEY F
	JAPAN ASCII


APPLE_USB (TermId = 125)
	US
	BELGIUM
	FINNLAND (SWEDEN)
	FRANCE
	GERMANY
	ITALY
	SWITZERLAND (German)
	UK
	DENMARK
	NORWAY
	SPAIN
	LATIN AMERICA
	FRENCH CANADIAN

*/
/*
Total Memory
------------
	3484 exceptions + 360 pointers
	With padding (4 bytes per exception, 4 bytes per pointer)
		15376 (0x3C10) bytes
	Other (3 bytes per exception, 4 bytes per pointer)
		11892 (0x2E74) bytes
*/


#endif
