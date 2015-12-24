/* ==============================================================================
   CPP Wedge Engine: International translation tables
   ==============================================================================

   $RCSfile: interfaces_wedge/keyintf_uni.cpp $
   $Revision: 2.5 $
   $Date: 2011/02/15 06:28:16EST $

   ==============================================================================

   ============================================================================== */
/* ==============================================================================
   Use the Windows ANSI code page (CP1252) to view this file!
	This file is automatically produced by Forkey32 - 4.6.0.14
	Source Name     : J:\forkey4\files\MATRIX.KLF
	Destination Name: J:\forkey4\NewDatabaseCPP\keyintf_uni.CPP

	Date: Tue Feb 15 08:51:10 2011

   ============================================================================== */

#define INTERNATIONAL_TABLE_VERSION 201	/* This version gets changed when table structure changes */

/* Format version history: */
/* 200 Initial release for unicode characters */
/* 100 Initial release for byte characters */
/* x01 Changed structure of pointers, so the tables are the only place that defines countries */

#include "stdInclude.h"
/* This single include file allows for adding you special requirements */
#include "internat.h"

#ifdef UNICODE_FOR_INTERFACES


/* Here we test whether the format versions match */
#if (INTERNATIONAL_PRG_VERSION != INTERNATIONAL_TABLE_VERSION)
#error "International tables do have the wrong version"
#endif

// COrderExceptions::WriteDump
//   	   0    1    2    3    4    5    6    7    8    9   10
//ID:	   1    3    5    6    7    8   25   35  104  112  125
//   	 386, 157, 157, 170, 235, 278, 313, 166, 213,2337, 487,	//000:ID=001
//   	    ,3903,  22,    ,    ,    ,    ,    ,    ,    ,    ,	//001:ID=003
//   	    ,  22,3889,    ,    ,    ,    ,    ,    ,    ,    ,	//002:ID=005
//   	 394,  88,  88, 465, 220, 264, 517, 348, 227,2561, 493,	//003:ID=006
//   	 459, 246, 246, 220, 447,  77, 364, 448, 305,2557, 570,	//004:ID=007
//   	 502, 289, 289, 264,  77, 422, 301, 494, 332,2557, 600,	//005:ID=008
//   	 313, 312, 312, 293, 140,  77, 364, 312, 346,2334, 606,	//006:ID=025
//   	 166,  55,  55, 124, 224, 270, 312, 418, 221,2337, 493,	//007:ID=035
//   	4469, 714, 699,4259,4337,4364,4602,4477, 949,6593,1949,	//008:ID=104
//   	  97,  97,  97,  97,  93,  93,  94,  97,  97,   2, 108,	//009:ID=112
//   	3847, 782, 762,3629,3706,3736,3966,3853,1053,5708,1473,	//010:ID=125

//Chain        ,    ,    ,    ,    ,    ,    ,    ,    ,    ,    ,
//ParIDs       ,    ,    ,    ,    ,    ,    ,    ,    ,    ,    ,
//IDs         1,   3,   5,   6,   7,   8,  25,  35, 104, 112, 125,

/*
 Terminal IDs and it's parents:
1 <- 3 <- 5
3 <- 5
5
6 <- 3 <- 5
7 <- 8 <- 6 <- 3 <- 5
8 <- 6 <- 3 <- 5
25 <- 8 <- 6 <- 3 <- 5
35 <- 3 <- 5
104 <- 5
112
125 <- 5
*/
// COrderExceptions::WriteDump
//   	   0    1    2    3    4    5    6    7    8    9   10
//ID:	   1    3    5    6    7    8   25   35  104  112  125
//   	    , 157,    ,    ,    ,    ,    ,    ,    ,    ,    ,	//000:ID=001
//   	    ,    ,  22,    ,    ,    ,    ,    ,    ,    ,    ,	//001:ID=003
//   	    ,    ,3889,    ,    ,    ,    ,    ,    ,    ,    ,	//002:ID=005
//   	    ,  88,    ,    ,    ,    ,    ,    ,    ,    ,    ,	//003:ID=006
//   	    ,    ,    ,    ,    ,  77,    ,    ,    ,    ,    ,	//004:ID=007
//   	    ,    ,    , 264,    ,    ,    ,    ,    ,    ,    ,	//005:ID=008
//   	    ,    ,    ,    ,    ,  77,    ,    ,    ,    ,    ,	//006:ID=025
//   	    ,  55,    ,    ,    ,    ,    ,    ,    ,    ,    ,	//007:ID=035
//   	    ,    , 699,    ,    ,    ,    ,    ,    ,    ,    ,	//008:ID=104
//   	    ,    ,    ,    ,    ,    ,    ,    ,    ,   2,    ,	//009:ID=112
//   	    ,    , 762,    ,    ,    ,    ,    ,    ,    ,    ,	//010:ID=125

//Chain       1,   2,   2,   1,   5,   3,   5,   1,   2,   9,   2,
//ParIDs      3,   5,   5,   3,   8,   6,   8,   3,   5, 112,   5,
//IDs         1,   3,   5,   6,   7,   8,  25,  35, 104, 112, 125,

/*
*/

#define NOS 0	/* Character is not supported by this leyout */


/******************************************************/
/* Exception tables                                   */
/******************************************************/

#define IBM_XT_USA                            		NoExceptions	// not required

// The next is used if we need no exceptions for a particular country
// It allows us to use the NULL pointer for marking not supported countries
// We just use the endmarker of the very first table for this
#define NoExceptions	&IBM_XT_Belgian[11]

// TermId=  1
const CKeyFixup IBM_XT_Belgian[12] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '\' */ 0x005C,  45, MO_AltRight,
	/* '`' */ 0x0060,  42, MO_AltRight,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* '¨' */ 0x00A8,  27, MO_ShiftLeft,
	/* '²' */ 0x00B2, NOS, 0,
	/* '³' */ 0x00B3, NOS, 0,
	/* '´' */ 0x00B4,  41, MO_AltRight,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  1
const CKeyFixup IBM_XT_Finnish_Swedish[19] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '#' */ 0x0023,   4, MO_AltRight,
	/* '$' */ 0x0024,   5, MO_ShiftLeft,
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,  45, MO_AltRight,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '^' */ 0x005E,   7, MO_AltRight,
	/* '`' */ 0x0060,  13, MO_ShiftLeft,
	/* '{' */ 0x007B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x007C,  45, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x007D,  28, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x007E,  42, MO_ShiftLeft|MO_AltRight,
	/* '£' */ 0x00A3,   4, MO_ShiftLeft,
	/* '¤' */ 0x00A4, NOS, 0,
	/* '§' */ 0x00A7, NOS, 0,
	/* '¨' */ 0x00A8,  28, 0,
	/* '´' */ 0x00B4,  13, 0,
	/* '½' */ 0x00BD, NOS, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  1
const CKeyFixup IBM_XT_French[25] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   9, 0,
	/* '*' */ 0x002A,  28, MO_ShiftLeft,
	/* '+' */ 0x002B,  55, MO_ShiftLeft,
	/* '-' */ 0x002D,  13, 0,
	/* '=' */ 0x003D,  55, 0,
	/* '@' */ 0x0040,   3, MO_AltRight,
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,  45, MO_AltRight,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '^' */ 0x005E,   7, MO_AltRight,
	/* '_' */ 0x005F,  13, MO_ShiftLeft,
	/* '`' */ 0x0060,  42, MO_AltRight,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* '£' */ 0x00A3,  42, MO_ShiftLeft,
	/* '¤' */ 0x00A4, NOS, 0,
	/* '§' */ 0x00A7,   7, 0,
	/* '¨' */ 0x00A8,  27, MO_ShiftLeft,
	/* '²' */ 0x00B2, NOS, 0,
	/* '´' */ 0x00B4,  41, MO_AltRight,
	/* 'µ' */ 0x00B5,  42, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  1
const CKeyFixup IBM_XT_German[17] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* ''' */ 0x0027,  13, 0,
	/* '@' */ 0x0040,   3, MO_AltRight,
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,  45, MO_AltRight,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '^' */ 0x005E,  42, MO_ShiftLeft,
	/* '`' */ 0x0060,  13, MO_ShiftLeft,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* '°' */ 0x00B0, NOS, 0,
	/* '²' */ 0x00B2, NOS, 0,
	/* '³' */ 0x00B3, NOS, 0,
	/* '´' */ 0x00B4, NOS, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  1
const CKeyFixup IBM_XT_Italian[11] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '#' */ 0x0023,  41, MO_ShiftLeft,
	/* '@' */ 0x0040, NOS, 0,
	/* '\' */ 0x005C,  45, MO_AltRight,
	/* '`' */ 0x0060, NOS, 0,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* '°' */ 0x00B0, NOS, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  1
const CKeyFixup IBM_XT_Swiss_German[16] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '[' */ 0x005B, NOS, 0,
	/* '\' */ 0x005C,  45, MO_AltRight,
	/* ']' */ 0x005D, NOS, 0,
	/* '^' */ 0x005E,  13, 0,
	/* '`' */ 0x0060,  13, MO_ShiftLeft,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C,   8, MO_AltRight,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E,  13, MO_AltRight,
	/* '¦' */ 0x00A6, NOS, 0,
	/* '§' */ 0x00A7,   6, MO_AltRight,
	/* '¨' */ 0x00A8,  28, 0,
	/* '°' */ 0x00B0,   5, MO_AltRight,
	/* '´' */ 0x00B4,  12, MO_AltRight,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  1
const CKeyFixup IBM_XT_British[5] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '\' */ 0x005C,  45, 0,
	/* '`' */ 0x0060, NOS, 0,
	/* '|' */ 0x007C,  45, MO_ShiftLeft,
	/* '~' */ 0x007E,  42, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  1
const CKeyFixup IBM_XT_Danish[16] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,  45, MO_AltRight,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '^' */ 0x005E,   7, MO_AltRight,
	/* '`' */ 0x0060,  13, MO_ShiftLeft,
	/* '{' */ 0x007B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x007C,  45, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x007D,  28, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x007E,  42, MO_ShiftLeft|MO_AltRight,
	/* '£' */ 0x00A3, NOS, 0,
	/* '§' */ 0x00A7, NOS, 0,
	/* '¨' */ 0x00A8,  28, 0,
	/* '´' */ 0x00B4,  13, 0,
	/* '½' */ 0x00BD, NOS, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  1
const CKeyFixup IBM_XT_Norwegian[18] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '#' */ 0x0023,   4, MO_AltRight,
	/* '$' */ 0x0024,   5, MO_ShiftLeft,
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,  45, MO_AltRight,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '^' */ 0x005E,   7, MO_AltRight,
	/* '`' */ 0x0060,  13, 0,
	/* '{' */ 0x007B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x007C,  45, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x007D,  28, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x007E,  42, MO_ShiftLeft|MO_AltRight,
	/* '£' */ 0x00A3,   4, MO_ShiftLeft,
	/* '¤' */ 0x00A4, NOS, 0,
	/* '§' */ 0x00A7, NOS, 0,
	/* '¨' */ 0x00A8,  28, 0,
	/* '´' */ 0x00B4,  13, MO_ShiftLeft,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  1
const CKeyFixup IBM_XT_Spanish[33] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,  54, MO_ShiftLeft,
	/* '"' */ 0x0022,  55, MO_ShiftLeft,
	/* '#' */ 0x0023,   4, MO_ShiftLeft,
	/* '&' */ 0x0026,   8, MO_ShiftLeft,
	/* ''' */ 0x0027,  55, 0,
	/* '(' */ 0x0028,  10, MO_ShiftLeft,
	/* ')' */ 0x0029,  11, MO_ShiftLeft,
	/* '*' */ 0x002A,   9, MO_ShiftLeft,
	/* '+' */ 0x002B,  13, MO_ShiftLeft,
	/* '-' */ 0x002D,  12, 0,
	/* '/' */ 0x002F,   7, MO_ShiftLeft,
	/* ':' */ 0x003A,  41, MO_ShiftLeft,
	/* ';' */ 0x003B,  41, 0,
	/* '=' */ 0x003D,  13, 0,
	/* '?' */ 0x003F,  53, MO_ShiftLeft,
	/* '\' */ 0x005C,  45, MO_AltRight,
	/* '^' */ 0x005E,  28, MO_ShiftLeft,
	/* '_' */ 0x005F,  12, MO_ShiftLeft,
	/* '`' */ 0x0060,  28, 0,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* '¡' */ 0x00A1,   2, MO_ShiftLeft,
	/* '¨' */ 0x00A8,  27, 0,
	/* 'ª' */ 0x00AA, NOS, 0,
	/* '¬' */ 0x00AC, NOS, 0,
	/* '´' */ 0x00B4,  27, MO_ShiftLeft,
	/* '·' */ 0x00B7, NOS, 0,
	/* 'º' */ 0x00BA, NOS, 0,
	/* '¿' */ 0x00BF,   3, MO_ShiftLeft,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

#define IBM_AT_USA                            		NoExceptions	// not required

#define IBM_AT_Belgian                        		NoExceptions	// not required

#define IBM_AT_Finnish_Swedish                		NoExceptions	// not required

#define IBM_AT_French                         		NoExceptions	// not required

#define IBM_AT_German                         		NoExceptions	// not required

#define IBM_AT_Italian                        		NoExceptions	// not required

#define IBM_AT_Swiss_German                   		NoExceptions	// not required

#define IBM_AT_British                        		NoExceptions	// not required

#define IBM_AT_Danish                         		NoExceptions	// not required

#define IBM_AT_Norwegian                      		NoExceptions	// not required

#define IBM_AT_Spanish                        		NoExceptions	// not required

#define IBM_AT_Dutch                          		NoExceptions	// not required

#define IBM_AT_Hebrew                         		NoExceptions	// not required

#define IBM_AT_Portuguese                     		NoExceptions	// not required

#define IBM_AT_Latin_America                  		NoExceptions	// not required

#define IBM_AT_Czech_DEC                      		NoExceptions	// not required

#define IBM_AT_Brazilian                      		NoExceptions	// not required

#define IBM_AT_Greek_DEC                      		NoExceptions	// not required

#define IBM_AT_Canadian_French                		NoExceptions	// not required

#define IBM_AT_Hungarian                      		NoExceptions	// not required

#define IBM_AT_Polish                         		NoExceptions	// not required

#define IBM_AT_SCS                            		NoExceptions	// not required

#define IBM_AT_Slovakian_DEC                  		NoExceptions	// not required

#define IBM_AT_Swedish                        		NoExceptions	// not required

// TermId=  3 	,  2 , 97 ,106 ,124
const CKeyFixup IBM_AT_Turkish_Q[9] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '<' */ 0x003C,   1, MO_AltRight,
	/* '>' */ 0x003E,   2, MO_AltRight,
	/* 'i' */ 0x0069,  24, MO_AltRight,
	/* '|' */ 0x007C,  13, MO_AltRight,
	/* '£' */ 0x00A3,   3, MO_AltRight,
	/* '½' */ 0x00BD,   6, MO_AltRight,
	/* ??? */ 0x0130,  24, MO_ShiftLeft|MO_AltRight,	// LATIN CAPITAL LETTER I WITH DOT ABOVE
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

#define IBM_AT_Romanian                       		NoExceptions	// not required

#define IBM_AT_Russian                        		NoExceptions	// not required

// TermId=  3 	,  2 , 97 ,106 ,124
const CKeyFixup IBM_AT_Turkish_F[18] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* 'I' */ 0x0049,  20, MO_ShiftLeft,
	/* 'i' */ 0x0069,  32, 0,
	/* '|' */ 0x007C,  13, MO_AltRight,
	/* '¡' */ 0x00A1,   2, MO_ShiftLeft|MO_AltRight,
	/* '¤' */ 0x00A4,   5, MO_ShiftLeft|MO_AltRight,
	/* '¦' */ 0x00A6,  45, MO_ShiftLeft|MO_AltRight,
	/* '§' */ 0x00A7,  32, MO_ShiftLeft|MO_AltRight,
	/* '©' */ 0x00A9,  48, MO_ShiftLeft|MO_AltRight,
	/* 'ª' */ 0x00AA,  34, MO_ShiftLeft|MO_AltRight,
	/* '­' */ 0x00AD,  55, MO_AltRight,
	/* '®' */ 0x00AE,  20, MO_ShiftLeft|MO_AltRight,
	/* '³' */ 0x00B3,   4, MO_ShiftLeft|MO_AltRight,
	/* 'º' */ 0x00BA,  52, MO_ShiftLeft|MO_AltRight,
	/* '¿' */ 0x00BF,  12, MO_ShiftLeft|MO_AltRight,
	/* 'Ø' */ 0x00D8,  25, MO_ShiftLeft|MO_AltRight,
	/* 'ø' */ 0x00F8,  25, MO_AltRight,
	/* '€' */ 0x20AC,  33, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

#define IBM_AT_Japanese_ASCII                 		NoExceptions	// not required

#define IBM_AT_Swiss_French                   		NoExceptions	// not required

#define IBM_AT_USA_International              		NoExceptions	// not required

#define IBM_AT_Slovenian                      		NoExceptions	// not required

#define IBM_AT_Croatian                       		NoExceptions	// not required

#define IBM_AT_Bosnian                        		NoExceptions	// not required

#define IBM_AT_Macedonian                     		NoExceptions	// not required

#define IBM_AT_Albanian                       		NoExceptions	// not required

#define IBM_AT_Serbian_Latin                  		NoExceptions	// not required

#define IBM_AT_Serbian_Cyrillic               		NoExceptions	// not required

#define IBM_AT_Czech_QWERTZ                   		NoExceptions	// not required

#define IBM_AT_Czech_QWERTY                   		NoExceptions	// not required

#define IBM_AT_Czech_Programmers              		NoExceptions	// not required

#define IBM_AT_Estonian                       		NoExceptions	// not required

#define IBM_AT_Latvian                        		NoExceptions	// not required

#define IBM_AT_Latvian_QWERTY                 		NoExceptions	// not required

#define IBM_AT_Lithuania                      		NoExceptions	// not required

#define IBM_AT_Lithuanian_IBM                 		NoExceptions	// not required

#define IBM_AT_Slovakian_QWERTZ               		NoExceptions	// not required

#define IBM_AT_Slovakian_QWERTY               		NoExceptions	// not required

#define IBM_AT_Hungarian_101_Key              		NoExceptions	// not required

#define IBM_AT_Spanish_Variation              		NoExceptions	// not required

#define IBM_AT_Bulgarian_Cyrillic             		NoExceptions	// not required

#define IBM_AT_Bulgarian_Latin                		NoExceptions	// not required

#define IBM_AT_Canadian_French_Legacy         		NoExceptions	// not required

#define IBM_AT_Canadian_Multilingual          		NoExceptions	// not required

#define IBM_AT_Italian_142                    		NoExceptions	// not required

#define IBM_AT_Polish_214                     		NoExceptions	// not required

#define IBM_AT_Polish_Programmers             		NoExceptions	// not required

#define IBM_AT_Brazilian_MS                   		NoExceptions	// not required

#define IBM_AT_Greek_Polytonic                		NoExceptions	// not required

#define IBM_AT_Greek_220                      		NoExceptions	// not required

#define IBM_AT_Greek_319                      		NoExceptions	// not required

#define IBM_AT_Greek_Latin                    		NoExceptions	// not required

#define IBM_AT_Greek_220_Latin                		NoExceptions	// not required

#define IBM_AT_Greek_319_Latin                		NoExceptions	// not required

#define IBM_AT_Greek_MS                       		NoExceptions	// not required

#define IBM_AT_Russian_MS                     		NoExceptions	// not required

#define IBM_AT_Russian_Typewriter             		NoExceptions	// not required

#define IBM_AT_Irish                          		NoExceptions	// not required

#define IBM_AT_Maltese                        		NoExceptions	// not required

#define IBM_AT_Icelandic                      		NoExceptions	// not required

#define IBM_AT_Ukrainian                      		NoExceptions	// not required

#define IBM_AT_Uzbek_Cyrillic                 		NoExceptions	// not required

#define IBM_AT_Kazakh                         		NoExceptions	// not required

#define IBM_AT_Kyrgyz_Cyrillic                		NoExceptions	// not required

#define IBM_AT_Azeri_Latin                    		NoExceptions	// not required

#define IBM_AT_Azeri_Cyrillic                 		NoExceptions	// not required

#define IBM_AT_Belarusian                     		NoExceptions	// not required

#define IBM_AT_Faeroese                       		NoExceptions	// not required

#define IBM_AT_Gaelic                         		NoExceptions	// not required

#define IBM_AT_Tatar                          		NoExceptions	// not required

#define IBM_AT_Mongolian_Cyrillic             		NoExceptions	// not required

#define DEC_VT510_PC_USA                      		NoExceptions	// not required

// TermId=  5
const CKeyFixup DEC_VT510_PC_Belgian[67] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   9, 0,
	/* '"' */ 0x0022,   4, 0,
	/* '#' */ 0x0023,   4, MO_AltRight,
	/* '$' */ 0x0024,  28, 0,
	/* '%' */ 0x0025,  41, MO_ShiftLeft,
	/* '&' */ 0x0026,   2, 0,
	/* ''' */ 0x0027,   5, 0,
	/* '(' */ 0x0028,   6, 0,
	/* ')' */ 0x0029,  12, 0,
	/* '*' */ 0x002A,  28, MO_ShiftLeft,
	/* '+' */ 0x002B,  55, MO_ShiftLeft,
	/* ',' */ 0x002C,  52, 0,
	/* '-' */ 0x002D,  13, 0,
	/* '.' */ 0x002E,  53, MO_ShiftLeft,
	/* '/' */ 0x002F,  54, MO_ShiftLeft,
	/* '0' */ 0x0030,  11, MO_ShiftLeft,
	/* '1' */ 0x0031,   2, MO_ShiftLeft,
	/* '2' */ 0x0032,   3, MO_ShiftLeft,
	/* '3' */ 0x0033,   4, MO_ShiftLeft,
	/* '4' */ 0x0034,   5, MO_ShiftLeft,
	/* '5' */ 0x0035,   6, MO_ShiftLeft,
	/* '6' */ 0x0036,   7, MO_ShiftLeft,
	/* '7' */ 0x0037,   8, MO_ShiftLeft,
	/* '8' */ 0x0038,   9, MO_ShiftLeft,
	/* '9' */ 0x0039,  10, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, 0,
	/* ';' */ 0x003B,  53, 0,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  55, 0,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  52, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_AltRight,
	/* 'A' */ 0x0041,  17, MO_ShiftLeft,
	/* 'M' */ 0x004D,  40, MO_ShiftLeft,
	/* 'Q' */ 0x0051,  31, MO_ShiftLeft,
	/* 'W' */ 0x0057,  46, MO_ShiftLeft,
	/* 'Z' */ 0x005A,  18, MO_ShiftLeft,
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,  45, MO_AltRight,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '^' */ 0x005E,   7, MO_AltRight,
	/* '_' */ 0x005F,  13, MO_ShiftLeft,
	/* '`' */ 0x0060,  42, MO_AltRight|MO_DeadKey,
	/* 'a' */ 0x0061,  17, 0,
	/* 'm' */ 0x006D,  40, 0,
	/* 'q' */ 0x0071,  31, 0,
	/* 'w' */ 0x0077,  46, 0,
	/* 'z' */ 0x007A,  18, 0,
	/* '{' */ 0x007B,  10, MO_AltRight,
	/* '|' */ 0x007C,   2, MO_AltRight,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* '~' */ 0x007E,  55, MO_AltRight|MO_DeadKey,
	/* '£' */ 0x00A3,  42, MO_ShiftLeft,
	/* '§' */ 0x00A7,   7, 0,
	/* '¨' */ 0x00A8,  27, MO_ShiftLeft|MO_DeadKey,
	/* '°' */ 0x00B0,  12, MO_ShiftLeft,
	/* '²' */ 0x00B2,   1, 0,
	/* '³' */ 0x00B3,   1, MO_ShiftLeft,
	/* '´' */ 0x00B4,  41, MO_AltRight|MO_DeadKey,
	/* 'µ' */ 0x00B5,  42, 0,
	/* 'à' */ 0x00E0,  11, 0,
	/* 'ç' */ 0x00E7,  10, 0,
	/* 'è' */ 0x00E8,   8, 0,
	/* 'é' */ 0x00E9,   3, 0,
	/* 'ù' */ 0x00F9,  41, 0,
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Finnish_Swedish[41] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '$' */ 0x0024,   5, MO_AltRight,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,  42, 0,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  42, MO_ShiftLeft,
	/* '+' */ 0x002B,  12, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_AltRight,
	/* '[' */ 0x005B,   9, MO_AltRight,
	/* '\' */ 0x005C,  12, MO_AltRight,
	/* ']' */ 0x005D,  10, MO_AltRight,
	/* '^' */ 0x005E,  28, MO_ShiftLeft|MO_DeadKey,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,  13, MO_ShiftLeft|MO_DeadKey,
	/* '{' */ 0x007B,   8, MO_AltRight,
	/* '|' */ 0x007C,  45, MO_AltRight,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* '~' */ 0x007E,  28, MO_AltRight|MO_DeadKey,
	/* '£' */ 0x00A3,   4, MO_AltRight,
	/* '¤' */ 0x00A4,   5, MO_ShiftLeft,
	/* '§' */ 0x00A7,   1, 0,
	/* '¨' */ 0x00A8,  28, MO_DeadKey,
	/* '´' */ 0x00B4,  13, MO_DeadKey,
	/* '½' */ 0x00BD,   1, MO_ShiftLeft,
	/* 'Ä' */ 0x00C4,  41, MO_ShiftLeft,
	/* 'Å' */ 0x00C5,  27, MO_ShiftLeft,
	/* 'Ö' */ 0x00D6,  40, MO_ShiftLeft,
	/* 'ä' */ 0x00E4,  41, 0,
	/* 'å' */ 0x00E5,  27, 0,
	/* 'ö' */ 0x00F6,  40, 0,
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_French[64] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,  55, 0,
	/* '"' */ 0x0022,   4, 0,
	/* '#' */ 0x0023,   4, MO_AltRight,
	/* '$' */ 0x0024,  28, 0,
	/* '%' */ 0x0025,  41, MO_ShiftLeft,
	/* '&' */ 0x0026,   2, 0,
	/* ''' */ 0x0027,   5, 0,
	/* '(' */ 0x0028,   6, 0,
	/* ')' */ 0x0029,  12, 0,
	/* '*' */ 0x002A,  42, 0,
	/* ',' */ 0x002C,  52, 0,
	/* '-' */ 0x002D,   7, 0,
	/* '.' */ 0x002E,  53, MO_ShiftLeft,
	/* '/' */ 0x002F,  54, MO_ShiftLeft,
	/* '0' */ 0x0030,  11, MO_ShiftLeft,
	/* '1' */ 0x0031,   2, MO_ShiftLeft,
	/* '2' */ 0x0032,   3, MO_ShiftLeft,
	/* '3' */ 0x0033,   4, MO_ShiftLeft,
	/* '4' */ 0x0034,   5, MO_ShiftLeft,
	/* '5' */ 0x0035,   6, MO_ShiftLeft,
	/* '6' */ 0x0036,   7, MO_ShiftLeft,
	/* '7' */ 0x0037,   8, MO_ShiftLeft,
	/* '8' */ 0x0038,   9, MO_ShiftLeft,
	/* '9' */ 0x0039,  10, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, 0,
	/* ';' */ 0x003B,  53, 0,
	/* '<' */ 0x003C,  45, 0,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  52, MO_ShiftLeft,
	/* '@' */ 0x0040,  11, MO_AltRight,
	/* 'A' */ 0x0041,  17, MO_ShiftLeft,
	/* 'M' */ 0x004D,  40, MO_ShiftLeft,
	/* 'Q' */ 0x0051,  31, MO_ShiftLeft,
	/* 'W' */ 0x0057,  46, MO_ShiftLeft,
	/* 'Z' */ 0x005A,  18, MO_ShiftLeft,
	/* '[' */ 0x005B,   6, MO_AltRight,
	/* '\' */ 0x005C,   9, MO_AltRight,
	/* ']' */ 0x005D,  12, MO_AltRight,
	/* '^' */ 0x005E,  10, MO_AltRight,
	/* '_' */ 0x005F,   9, 0,
	/* '`' */ 0x0060,   8, MO_AltRight|MO_DeadKey,
	/* 'a' */ 0x0061,  17, 0,
	/* 'm' */ 0x006D,  40, 0,
	/* 'q' */ 0x0071,  31, 0,
	/* 'w' */ 0x0077,  46, 0,
	/* 'z' */ 0x007A,  18, 0,
	/* '{' */ 0x007B,   5, MO_AltRight,
	/* '|' */ 0x007C,   7, MO_AltRight,
	/* '}' */ 0x007D,  13, MO_AltRight,
	/* '~' */ 0x007E,   3, MO_AltRight|MO_DeadKey,
	/* '£' */ 0x00A3,  28, MO_ShiftLeft,
	/* '¤' */ 0x00A4,  28, MO_AltRight,
	/* '§' */ 0x00A7,  55, MO_ShiftLeft,
	/* '¨' */ 0x00A8,  27, MO_ShiftLeft|MO_DeadKey,
	/* '°' */ 0x00B0,  12, MO_ShiftLeft,
	/* '²' */ 0x00B2,   1, 0,
	/* 'µ' */ 0x00B5,  42, MO_ShiftLeft,
	/* 'à' */ 0x00E0,  11, 0,
	/* 'ç' */ 0x00E7,  10, 0,
	/* 'è' */ 0x00E8,   8, 0,
	/* 'é' */ 0x00E9,   3, 0,
	/* 'ù' */ 0x00F9,  41, 0,
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_German[45] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023,  42, 0,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,  42, MO_ShiftLeft,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  28, MO_ShiftLeft,
	/* '+' */ 0x002B,  28, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,  17, MO_AltRight,
	/* 'Y' */ 0x0059,  46, MO_ShiftLeft,
	/* 'Z' */ 0x005A,  22, MO_ShiftLeft,
	/* '[' */ 0x005B,   9, MO_AltRight,
	/* '\' */ 0x005C,  12, MO_AltRight,
	/* ']' */ 0x005D,  10, MO_AltRight,
	/* '^' */ 0x005E,   1, MO_DeadKey,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,  13, MO_ShiftLeft|MO_DeadKey,
	/* 'y' */ 0x0079,  46, 0,
	/* 'z' */ 0x007A,  22, 0,
	/* '{' */ 0x007B,   8, MO_AltRight,
	/* '|' */ 0x007C,  45, MO_AltRight,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* '~' */ 0x007E,  28, MO_AltRight,
	/* '§' */ 0x00A7,   4, MO_ShiftLeft,
	/* '°' */ 0x00B0,   1, MO_ShiftLeft,
	/* '²' */ 0x00B2,   3, MO_AltRight,
	/* '³' */ 0x00B3,   4, MO_AltRight,
	/* '´' */ 0x00B4,  13, MO_DeadKey,
	/* 'Ä' */ 0x00C4,  41, MO_ShiftLeft,
	/* 'Ö' */ 0x00D6,  40, MO_ShiftLeft,
	/* 'Ü' */ 0x00DC,  27, MO_ShiftLeft,
	/* 'ß' */ 0x00DF,  12, 0,
	/* 'ä' */ 0x00E4,  41, 0,
	/* 'ö' */ 0x00F6,  40, 0,
	/* 'ü' */ 0x00FC,  27, 0,
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Italian[39] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023,  41, MO_AltRight,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,  12, 0,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  28, MO_ShiftLeft,
	/* '+' */ 0x002B,  28, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,  40, MO_AltRight,
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,   1, 0,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '^' */ 0x005E,  13, MO_ShiftLeft,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060, NOS, 0,
	/* '{' */ 0x007B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x007C,   1, MO_ShiftLeft,
	/* '}' */ 0x007D,  28, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x007E, NOS, 0,
	/* '£' */ 0x00A3,   4, MO_ShiftLeft,
	/* '§' */ 0x00A7,  42, MO_ShiftLeft,
	/* '°' */ 0x00B0,  41, MO_ShiftLeft,
	/* 'à' */ 0x00E0,  41, 0,
	/* 'ç' */ 0x00E7,  40, MO_ShiftLeft,
	/* 'è' */ 0x00E8,  27, 0,
	/* 'é' */ 0x00E9,  27, MO_ShiftLeft,
	/* 'ì' */ 0x00EC,  13, 0,
	/* 'ò' */ 0x00F2,  40, 0,
	/* 'ù' */ 0x00F9,  42, 0,
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Swiss_German[48] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,  28, MO_ShiftLeft,
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023,   4, MO_AltRight,
	/* '$' */ 0x0024,  42, 0,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,  12, 0,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,   4, MO_ShiftLeft,
	/* '+' */ 0x002B,   2, MO_ShiftLeft,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_AltRight,
	/* 'Y' */ 0x0059,  46, MO_ShiftLeft,
	/* 'Z' */ 0x005A,  22, MO_ShiftLeft,
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,  45, MO_AltRight,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '^' */ 0x005E,  13, MO_DeadKey,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,  13, MO_ShiftLeft|MO_DeadKey,
	/* 'y' */ 0x0079,  46, 0,
	/* 'z' */ 0x007A,  22, 0,
	/* '{' */ 0x007B,  41, MO_AltRight,
	/* '|' */ 0x007C,   8, MO_AltRight,
	/* '}' */ 0x007D,  42, MO_AltRight,
	/* '~' */ 0x007E,  13, MO_AltRight|MO_DeadKey,
	/* '£' */ 0x00A3,  42, MO_ShiftLeft,
	/* '¦' */ 0x00A6,   2, MO_AltRight,
	/* '§' */ 0x00A7,   1, 0,
	/* '¨' */ 0x00A8,  28, MO_DeadKey,
	/* '°' */ 0x00B0,   1, MO_ShiftLeft,
	/* '´' */ 0x00B4,  12, MO_AltRight|MO_DeadKey,
	/* 'à' */ 0x00E0,  41, MO_ShiftLeft,
	/* 'ä' */ 0x00E4,  41, 0,
	/* 'ç' */ 0x00E7,   5, MO_ShiftLeft,
	/* 'è' */ 0x00E8,  27, MO_ShiftLeft,
	/* 'é' */ 0x00E9,  40, MO_ShiftLeft,
	/* 'ö' */ 0x00F6,  40, 0,
	/* 'ü' */ 0x00FC,  27, 0,
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_British[8] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023,  42, 0,
	/* '@' */ 0x0040,  41, MO_ShiftLeft,
	/* '\' */ 0x005C,  45, 0,
	/* '|' */ 0x007C,  45, MO_ShiftLeft,
	/* '~' */ 0x007E,  42, MO_ShiftLeft,
	/* '£' */ 0x00A3,   4, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Danish[41] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '$' */ 0x0024,   5, MO_AltRight,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,  42, 0,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  42, MO_ShiftLeft,
	/* '+' */ 0x002B,  12, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_AltRight,
	/* '[' */ 0x005B,   9, MO_AltRight,
	/* '\' */ 0x005C,  45, MO_AltRight,
	/* ']' */ 0x005D,  10, MO_AltRight,
	/* '^' */ 0x005E,  28, MO_ShiftLeft|MO_DeadKey,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,  13, MO_ShiftLeft|MO_DeadKey,
	/* '{' */ 0x007B,   8, MO_AltRight,
	/* '|' */ 0x007C,  13, MO_AltRight,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* '~' */ 0x007E,  28, MO_AltRight|MO_DeadKey,
	/* '£' */ 0x00A3,   4, MO_AltRight,
	/* '¤' */ 0x00A4,   5, MO_ShiftLeft,
	/* '§' */ 0x00A7,   1, MO_ShiftLeft,
	/* '¨' */ 0x00A8,  28, MO_DeadKey,
	/* '´' */ 0x00B4,  13, MO_DeadKey,
	/* '½' */ 0x00BD,   1, 0,
	/* 'Å' */ 0x00C5,  27, MO_ShiftLeft,
	/* 'Æ' */ 0x00C6,  40, MO_ShiftLeft,
	/* 'Ø' */ 0x00D8,  41, MO_ShiftLeft,
	/* 'å' */ 0x00E5,  27, 0,
	/* 'æ' */ 0x00E6,  40, 0,
	/* 'ø' */ 0x00F8,  41, 0,
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Norwegian[40] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '$' */ 0x0024,   5, MO_AltRight,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,  42, 0,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  42, MO_ShiftLeft,
	/* '+' */ 0x002B,  12, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_AltRight,
	/* '[' */ 0x005B,   9, MO_AltRight,
	/* '\' */ 0x005C,  13, 0,
	/* ']' */ 0x005D,  10, MO_AltRight,
	/* '^' */ 0x005E,  28, MO_ShiftLeft|MO_DeadKey,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,  13, MO_ShiftLeft|MO_DeadKey,
	/* '{' */ 0x007B,   8, MO_AltRight,
	/* '|' */ 0x007C,   1, 0,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* '~' */ 0x007E,  28, MO_AltRight|MO_DeadKey,
	/* '£' */ 0x00A3,   4, MO_AltRight,
	/* '¤' */ 0x00A4,   5, MO_ShiftLeft,
	/* '§' */ 0x00A7,   1, MO_ShiftLeft,
	/* '¨' */ 0x00A8,  28, MO_DeadKey,
	/* '´' */ 0x00B4,  13, MO_AltRight|MO_DeadKey,
	/* 'Å' */ 0x00C5,  27, MO_ShiftLeft,
	/* 'Æ' */ 0x00C6,  41, MO_ShiftLeft,
	/* 'Ø' */ 0x00D8,  40, MO_ShiftLeft,
	/* 'å' */ 0x00E5,  27, 0,
	/* 'æ' */ 0x00E6,  41, 0,
	/* 'ø' */ 0x00F8,  40, 0,
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Spanish[41] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023,   4, MO_AltRight,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,  12, 0,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  28, MO_ShiftLeft,
	/* '+' */ 0x002B,  28, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_AltRight,
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,   1, MO_AltRight,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '^' */ 0x005E,  27, MO_ShiftLeft|MO_DeadKey,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,  27, MO_DeadKey,
	/* '{' */ 0x007B,  41, MO_AltRight,
	/* '|' */ 0x007C,   2, MO_AltRight,
	/* '}' */ 0x007D,  42, MO_AltRight,
	/* '~' */ 0x007E,   5, MO_AltRight|MO_DeadKey,
	/* '¡' */ 0x00A1,  13, 0,
	/* '¨' */ 0x00A8,  41, MO_ShiftLeft|MO_DeadKey,
	/* 'ª' */ 0x00AA,   1, MO_ShiftLeft,
	/* '¬' */ 0x00AC,   7, MO_AltRight,
	/* '´' */ 0x00B4,  41, MO_DeadKey,
	/* '·' */ 0x00B7,   4, MO_ShiftLeft,
	/* 'º' */ 0x00BA,   1, 0,
	/* '¿' */ 0x00BF,  13, MO_ShiftLeft,
	/* 'Ç' */ 0x00C7,  42, MO_ShiftLeft,
	/* 'Ñ' */ 0x00D1,  40, MO_ShiftLeft,
	/* 'ç' */ 0x00E7,  42, 0,
	/* 'ñ' */ 0x00F1,  40, 0,
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Dutch[50] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,  11, MO_ShiftLeft,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  28, 0,
	/* '+' */ 0x002B,  40, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,  12, 0,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  42, 0,
	/* '=' */ 0x003D,  55, MO_ShiftLeft,
	/* '>' */ 0x003E,  42, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,   1, 0,
	/* '[' */ 0x005B,  45, MO_ShiftLeft,
	/* '\' */ 0x005C,  12, MO_AltRight,
	/* ']' */ 0x005D,  45, 0,
	/* '^' */ 0x005E,  27, MO_ShiftLeft|MO_DeadKey,
	/* '_' */ 0x005F,   8, MO_ShiftLeft,
	/* '`' */ 0x0060,  41, MO_ShiftLeft|MO_DeadKey,
	/* '{' */ 0x007B,   9, MO_AltRight,
	/* '|' */ 0x007C,  28, MO_ShiftLeft,
	/* '}' */ 0x007D,  10, MO_AltRight,
	/* '~' */ 0x007E,  13, MO_ShiftLeft|MO_DeadKey,
	/* '¢' */ 0x00A2,  48, MO_AltRight,
	/* '£' */ 0x00A3,   8, MO_AltRight,
	/* '¦' */ 0x00A6,  45, MO_AltRight,
	/* '§' */ 0x00A7,   1, MO_ShiftLeft,
	/* '¨' */ 0x00A8,  27, MO_DeadKey,
	/* '«' */ 0x00AB,  46, MO_AltRight,
	/* '¬' */ 0x00AC,   1, MO_AltRight,
	/* '°' */ 0x00B0,  13, 0,
	/* '±' */ 0x00B1,  40, MO_ShiftLeft,
	/* '²' */ 0x00B2,   3, MO_AltRight,
	/* '³' */ 0x00B3,   4, MO_AltRight,
	/* '´' */ 0x00B4,  41, MO_DeadKey,
	/* 'µ' */ 0x00B5,  52, MO_AltRight,
	/* '¶' */ 0x00B6,  20, MO_AltRight,
	/* '·' */ 0x00B7,  54, MO_AltRight,
	/* '¸' */ 0x00B8,  13, MO_AltRight|MO_DeadKey,
	/* '¹' */ 0x00B9,   2, MO_AltRight,
	/* '»' */ 0x00BB,  47, MO_AltRight,
	/* '¼' */ 0x00BC,   5, MO_AltRight,
	/* '½' */ 0x00BD,   6, MO_AltRight,
	/* '¾' */ 0x00BE,   7, MO_AltRight,
	/* 'ß' */ 0x00DF,  32, MO_AltRight,
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

#define DEC_VT510_PC_Hebrew                   		NoExceptions	// not required

// TermId=  5
const CKeyFixup DEC_VT510_PC_Portuguese[38] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,  12, 0,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  27, MO_ShiftLeft,
	/* '+' */ 0x002B,  27, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_AltRight,
	/* '[' */ 0x005B,   9, MO_AltRight,
	/* '\' */ 0x005C,   1, 0,
	/* ']' */ 0x005D,  10, MO_AltRight,
	/* '^' */ 0x005E,  42, MO_ShiftLeft|MO_DeadKey,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,  28, MO_ShiftLeft|MO_DeadKey,
	/* '{' */ 0x007B,   8, MO_AltRight,
	/* '|' */ 0x007C,   1, MO_ShiftLeft,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* '~' */ 0x007E,  42, MO_DeadKey,
	/* '£' */ 0x00A3,   4, MO_AltRight,
	/* '§' */ 0x00A7,   5, MO_AltRight,
	/* '¨' */ 0x00A8,  27, MO_AltRight|MO_DeadKey,
	/* 'ª' */ 0x00AA,  41, MO_ShiftLeft,
	/* '«' */ 0x00AB,  13, 0,
	/* '´' */ 0x00B4,  28, MO_DeadKey,
	/* 'º' */ 0x00BA,  41, 0,
	/* '»' */ 0x00BB,  13, MO_ShiftLeft,
	/* 'Ç' */ 0x00C7,  40, MO_ShiftLeft,
	/* 'ç' */ 0x00E7,  40, 0,
	/* '€' */ 0x20AC,   6, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Latin_America[35] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,  12, 0,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  28, MO_ShiftLeft,
	/* '+' */ 0x002B,  28, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,  17, MO_AltRight,
	/* '[' */ 0x005B,  41, MO_ShiftLeft,
	/* '\' */ 0x005C,  12, MO_AltRight,
	/* ']' */ 0x005D,  42, MO_ShiftLeft,
	/* '^' */ 0x005E,  41, MO_AltRight|MO_DeadKey,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,  42, MO_AltRight|MO_DeadKey,
	/* '{' */ 0x007B,  41, 0,
	/* '|' */ 0x007C,   1, 0,
	/* '}' */ 0x007D,  42, 0,
	/* '~' */ 0x007E,  28, MO_AltRight,
	/* '¡' */ 0x00A1,  13, MO_ShiftLeft,
	/* '¨' */ 0x00A8,  27, MO_ShiftLeft|MO_DeadKey,
	/* '¬' */ 0x00AC,   1, MO_AltRight,
	/* '°' */ 0x00B0,   1, MO_ShiftLeft,
	/* '´' */ 0x00B4,  27, MO_DeadKey,
	/* '¿' */ 0x00BF,  13, 0,
	/* 'Ñ' */ 0x00D1,  40, MO_ShiftLeft,
	/* 'ñ' */ 0x00F1,  40, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Czech_DEC[62] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   2, MO_AltRight,
	/* '"' */ 0x0022,  40, MO_ShiftLeft,
	/* '#' */ 0x0023,   4, MO_AltRight,
	/* '$' */ 0x0024,   5, MO_AltRight,
	/* '%' */ 0x0025,   6, MO_AltRight,
	/* '&' */ 0x0026,   8, MO_AltRight,
	/* ''' */ 0x0027,   1, MO_AltRight,
	/* '(' */ 0x0028,  10, MO_AltRight,
	/* ')' */ 0x0029,  11, MO_AltRight,
	/* '*' */ 0x002A,   9, MO_AltRight,
	/* '+' */ 0x002B,   2, 0,
	/* '-' */ 0x002D,  12, MO_AltRight,
	/* '/' */ 0x002F,  27, MO_ShiftLeft,
	/* '0' */ 0x0030,  11, MO_ShiftLeft,
	/* '1' */ 0x0031,   2, MO_ShiftLeft,
	/* '2' */ 0x0032,   3, MO_ShiftLeft,
	/* '3' */ 0x0033,   4, MO_ShiftLeft,
	/* '4' */ 0x0034,   5, MO_ShiftLeft,
	/* '5' */ 0x0035,   6, MO_ShiftLeft,
	/* '6' */ 0x0036,   7, MO_ShiftLeft,
	/* '7' */ 0x0037,   8, MO_ShiftLeft,
	/* '8' */ 0x0038,   9, MO_ShiftLeft,
	/* '9' */ 0x0039,  10, MO_ShiftLeft,
	/* ':' */ 0x003A,  40, MO_ShiftLeft|MO_AltRight,
	/* ';' */ 0x003B,  40, MO_AltRight,
	/* '<' */ 0x003C,  53, MO_ShiftLeft|MO_AltRight,
	/* '=' */ 0x003D,  12, 0,
	/* '>' */ 0x003E,  54, MO_ShiftLeft|MO_AltRight,
	/* '?' */ 0x003F,  53, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_AltRight,
	/* 'Y' */ 0x0059,  22, MO_ShiftLeft|MO_AltRight,
	/* 'Z' */ 0x005A,  22, MO_ShiftLeft,
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,  42, MO_AltRight,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '^' */ 0x005E,   7, MO_AltRight,
	/* '_' */ 0x005F,  12, MO_ShiftLeft|MO_AltRight,
	/* '`' */ 0x0060,  42, MO_ShiftLeft,
	/* 'y' */ 0x0079,  22, MO_AltRight,
	/* 'z' */ 0x007A,  22, 0,
	/* '{' */ 0x007B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x007C,  42, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x007D,  28, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x007E,   1, MO_ShiftLeft|MO_AltRight,
	/* '§' */ 0x00A7,  41, 0,
	/* '¨' */ 0x00A8,  42, 0,
	/* '´' */ 0x00B4,  13, 0,
	/* 'ß' */ 0x00DF,   1, MO_ShiftLeft,
	/* 'á' */ 0x00E1,   9, 0,
	/* 'ä' */ 0x00E4,   1, 0,
	/* 'é' */ 0x00E9,  11, 0,
	/* 'í' */ 0x00ED,  10, 0,
	/* 'ú' */ 0x00FA,  27, 0,
	/* 'ý' */ 0x00FD,   8, 0,
	/* ??? */ 0x010D,   5, 0,									// LATIN SMALL LETTER C WITH CARON
	/* ??? */ 0x011B,   3, 0,									// LATIN SMALL LETTER E WITH CARON
	/* ??? */ 0x0159,   6, 0,									// LATIN SMALL LETTER R WITH CARON
	/* 'š' */ 0x0161,   4, 0,
	/* ??? */ 0x016F,  40, 0,									// LATIN SMALL LETTER U WITH RING ABOVE
	/* 'ž' */ 0x017E,   7, 0,
	/* ??? */ 0x02C7,  13, MO_ShiftLeft,					// CARON
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Brazilian[30] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   1, MO_ShiftLeft,
	/* ''' */ 0x0027,   1, 0,
	/* '/' */ 0x002F,  56, 0,
	/* ':' */ 0x003A,  55, MO_ShiftLeft,
	/* ';' */ 0x003B,  55, 0,
	/* '?' */ 0x003F,  56, MO_ShiftLeft,
	/* '[' */ 0x005B,  28, 0,
	/* '\' */ 0x005C,  45, 0,
	/* ']' */ 0x005D,  42, 0,
	/* '^' */ 0x005E,  41, MO_ShiftLeft|MO_DeadKey,
	/* '`' */ 0x0060,  27, MO_ShiftLeft|MO_DeadKey,
	/* '{' */ 0x007B,  28, MO_ShiftLeft,
	/* '|' */ 0x007C,  45, MO_ShiftLeft,
	/* '}' */ 0x007D,  42, MO_ShiftLeft,
	/* '~' */ 0x007E,  41, MO_DeadKey,
	/* '¢' */ 0x00A2,   6, MO_AltRight,
	/* '£' */ 0x00A3,   5, MO_AltRight,
	/* '§' */ 0x00A7,  13, MO_AltRight,
	/* '¨' */ 0x00A8,   7, MO_ShiftLeft|MO_DeadKey,
	/* 'ª' */ 0x00AA,  28, MO_AltRight,
	/* '¬' */ 0x00AC,   7, MO_AltRight,
	/* '°' */ 0x00B0,  56, MO_AltRight,
	/* '²' */ 0x00B2,   3, MO_AltRight,
	/* '³' */ 0x00B3,   4, MO_AltRight,
	/* '´' */ 0x00B4,  27, MO_DeadKey,
	/* '¹' */ 0x00B9,   2, MO_AltRight,
	/* 'º' */ 0x00BA,  42, MO_AltRight,
	/* 'Ç' */ 0x00C7,  40, MO_ShiftLeft,
	/* 'ç' */ 0x00E7,  40, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Greek_DEC[50] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '¨' */ 0x00A8,  40, MO_ShiftLeft|MO_AltRight,
	/* ??? */ 0x0391,  31, MO_AltRight,						// GREEK CAPITAL LETTER ALPHA
	/* ??? */ 0x0392,  50, MO_AltRight,						// GREEK CAPITAL LETTER BETA
	/* ??? */ 0x0393,  35, MO_AltRight,						// GREEK CAPITAL LETTER GAMMA
	/* ??? */ 0x0394,  33, MO_AltRight,						// GREEK CAPITAL LETTER DELTA
	/* ??? */ 0x0395,  19, MO_AltRight,						// GREEK CAPITAL LETTER EPSILON
	/* ??? */ 0x0396,  46, MO_AltRight,						// GREEK CAPITAL LETTER ZETA
	/* ??? */ 0x0397,  36, MO_AltRight,						// GREEK CAPITAL LETTER ETA
	/* ??? */ 0x0398,  23, MO_AltRight,						// GREEK CAPITAL LETTER THETA
	/* ??? */ 0x0399,  24, MO_AltRight,						// GREEK CAPITAL LETTER IOTA
	/* ??? */ 0x039A,  38, MO_AltRight,						// GREEK CAPITAL LETTER KAPPA
	/* ??? */ 0x039B,  39, MO_AltRight,						// GREEK CAPITAL LETTER LAMDA
	/* ??? */ 0x039C,  52, MO_AltRight,						// GREEK CAPITAL LETTER MU
	/* ??? */ 0x039D,  51, MO_AltRight,						// GREEK CAPITAL LETTER NU
	/* ??? */ 0x039E,  37, MO_AltRight,						// GREEK CAPITAL LETTER XI
	/* ??? */ 0x039F,  25, MO_AltRight,						// GREEK CAPITAL LETTER OMICRON
	/* ??? */ 0x03A0,  26, MO_AltRight,						// GREEK CAPITAL LETTER PI
	/* ??? */ 0x03A1,  20, MO_AltRight,						// GREEK CAPITAL LETTER RHO
	/* ??? */ 0x03A3,  32, MO_AltRight,						// GREEK CAPITAL LETTER SIGMA
	/* ??? */ 0x03A4,  21, MO_AltRight,						// GREEK CAPITAL LETTER TAU
	/* ??? */ 0x03A5,  22, MO_AltRight,						// GREEK CAPITAL LETTER UPSILON
	/* ??? */ 0x03A6,  34, MO_AltRight,						// GREEK CAPITAL LETTER PHI
	/* ??? */ 0x03A7,  47, MO_AltRight,						// GREEK CAPITAL LETTER CHI
	/* ??? */ 0x03A8,  48, MO_AltRight,						// GREEK CAPITAL LETTER PSI
	/* ??? */ 0x03A9,  49, MO_AltRight,						// GREEK CAPITAL LETTER OMEGA
	/* ??? */ 0x03B1,  31, MO_ShiftLeft|MO_AltRight,	// GREEK SMALL LETTER ALPHA
	/* ??? */ 0x03B2,  50, MO_ShiftLeft|MO_AltRight,	// GREEK SMALL LETTER BETA
	/* ??? */ 0x03B3,  35, MO_ShiftLeft|MO_AltRight,	// GREEK SMALL LETTER GAMMA
	/* ??? */ 0x03B4,  33, MO_ShiftLeft|MO_AltRight,	// GREEK SMALL LETTER DELTA
	/* ??? */ 0x03B5,  19, MO_ShiftLeft|MO_AltRight,	// GREEK SMALL LETTER EPSILON
	/* ??? */ 0x03B6,  18, MO_AltRight,						// GREEK SMALL LETTER ZETA
	/* ??? */ 0x03B7,  36, MO_ShiftLeft|MO_AltRight,	// GREEK SMALL LETTER ETA
	/* ??? */ 0x03B8,  23, MO_ShiftLeft|MO_AltRight,	// GREEK SMALL LETTER THETA
	/* ??? */ 0x03B9,  24, MO_ShiftLeft|MO_AltRight,	// GREEK SMALL LETTER IOTA
	/* ??? */ 0x03BA,  38, MO_ShiftLeft|MO_AltRight,	// GREEK SMALL LETTER KAPPA
	/* ??? */ 0x03BB,  39, MO_ShiftLeft|MO_AltRight,	// GREEK SMALL LETTER LAMDA
	/* ??? */ 0x03BC,  52, MO_ShiftLeft|MO_AltRight,	// GREEK SMALL LETTER MU
	/* ??? */ 0x03BD,  51, MO_ShiftLeft|MO_AltRight,	// GREEK SMALL LETTER NU
	/* ??? */ 0x03BE,  37, MO_ShiftLeft|MO_AltRight,	// GREEK SMALL LETTER XI
	/* ??? */ 0x03BF,  25, MO_ShiftLeft|MO_AltRight,	// GREEK SMALL LETTER OMICRON
	/* ??? */ 0x03C0,  26, MO_ShiftLeft|MO_AltRight,	// GREEK SMALL LETTER PI
	/* ??? */ 0x03C1,  20, MO_ShiftLeft|MO_AltRight,	// GREEK SMALL LETTER RHO
	/* ??? */ 0x03C3,  32, MO_ShiftLeft|MO_AltRight,	// GREEK SMALL LETTER SIGMA
	/* ??? */ 0x03C4,  21, MO_ShiftLeft|MO_AltRight,	// GREEK SMALL LETTER TAU
	/* ??? */ 0x03C5,  22, MO_ShiftLeft|MO_AltRight,	// GREEK SMALL LETTER UPSILON
	/* ??? */ 0x03C6,  34, MO_ShiftLeft|MO_AltRight,	// GREEK SMALL LETTER PHI
	/* ??? */ 0x03C7,  47, MO_ShiftLeft|MO_AltRight,	// GREEK SMALL LETTER CHI
	/* ??? */ 0x03C8,  48, MO_ShiftLeft|MO_AltRight,	// GREEK SMALL LETTER PSI
	/* ??? */ 0x03C9,  49, MO_ShiftLeft|MO_AltRight,	// GREEK SMALL LETTER OMEGA
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Canadian_French[42] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023,   1, 0,
	/* ''' */ 0x0027,  53, MO_ShiftLeft,
	/* '/' */ 0x002F,   4, MO_ShiftLeft,
	/* '<' */ 0x003C,  42, 0,
	/* '>' */ 0x003E,  42, MO_ShiftLeft,
	/* '?' */ 0x003F,   7, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_AltRight,
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,   1, MO_AltRight,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '^' */ 0x005E,  27, MO_DeadKey,
	/* '`' */ 0x0060,  41, MO_DeadKey,
	/* '{' */ 0x007B,  41, MO_AltRight,
	/* '|' */ 0x007C,   1, MO_ShiftLeft,
	/* '}' */ 0x007D,  42, MO_AltRight,
	/* '~' */ 0x007E,  40, MO_AltRight,
	/* '¢' */ 0x00A2,   5, MO_AltRight,
	/* '£' */ 0x00A3,   4, MO_AltRight,
	/* '¤' */ 0x00A4,   6, MO_AltRight,
	/* '¦' */ 0x00A6,   8, MO_AltRight,
	/* '§' */ 0x00A7,  25, MO_AltRight,
	/* '¨' */ 0x00A8,  28, MO_ShiftLeft|MO_DeadKey,
	/* '«' */ 0x00AB,  45, 0,
	/* '¬' */ 0x00AC,   7, MO_AltRight,
	/* '­' */ 0x00AD,  54, MO_AltRight,
	/* '¯' */ 0x00AF,  53, MO_AltRight,
	/* '°' */ 0x00B0,  45, MO_AltRight,
	/* '±' */ 0x00B1,   2, MO_AltRight,
	/* '²' */ 0x00B2,   9, MO_AltRight,
	/* '³' */ 0x00B3,  10, MO_AltRight,
	/* '´' */ 0x00B4,  55, MO_AltRight|MO_DeadKey,
	/* 'µ' */ 0x00B5,  52, MO_AltRight,
	/* '¶' */ 0x00B6,  26, MO_AltRight,
	/* '¸' */ 0x00B8,  28, MO_DeadKey,
	/* '»' */ 0x00BB,  45, MO_ShiftLeft,
	/* '¼' */ 0x00BC,  11, MO_AltRight,
	/* '½' */ 0x00BD,  12, MO_AltRight,
	/* '¾' */ 0x00BE,  13, MO_AltRight,
	/* 'É' */ 0x00C9,  55, MO_ShiftLeft,
	/* 'é' */ 0x00E9,  55, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Hungarian[74] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   5, MO_ShiftLeft,
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023,  47, MO_AltRight,
	/* '$' */ 0x0024,  40, MO_AltRight,
	/* '&' */ 0x0026,  48, MO_AltRight,
	/* ''' */ 0x0027,   2, MO_ShiftLeft,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  55, MO_AltRight,
	/* '+' */ 0x002B,   4, MO_ShiftLeft,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,   7, MO_ShiftLeft,
	/* '0' */ 0x0030,   1, 0,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_AltRight,
	/* '<' */ 0x003C,  45, MO_AltRight,
	/* '=' */ 0x003D,   8, MO_ShiftLeft,
	/* '>' */ 0x003E,  46, MO_AltRight,
	/* '?' */ 0x003F,  53, MO_ShiftLeft,
	/* '@' */ 0x0040,  49, MO_AltRight,
	/* 'Y' */ 0x0059,  46, MO_ShiftLeft,
	/* 'Z' */ 0x005A,  22, MO_ShiftLeft,
	/* '[' */ 0x005B,  34, MO_AltRight,
	/* '\' */ 0x005C,  17, MO_AltRight,
	/* ']' */ 0x005D,  35, MO_AltRight,
	/* '^' */ 0x005E,   4, MO_AltRight|MO_DeadKey,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,   8, MO_AltRight,
	/* 'y' */ 0x0079,  46, 0,
	/* 'z' */ 0x007A,  22, 0,
	/* '{' */ 0x007B,  50, MO_AltRight,
	/* '|' */ 0x007C,  18, MO_AltRight,
	/* '}' */ 0x007D,  51, MO_AltRight,
	/* '~' */ 0x007E,   2, MO_AltRight,
	/* '¤' */ 0x00A4,  42, MO_AltRight,
	/* '§' */ 0x00A7,   1, MO_ShiftLeft,
	/* '¨' */ 0x00A8,  12, MO_AltRight|MO_DeadKey,
	/* '°' */ 0x00B0,   6, MO_AltRight|MO_DeadKey,
	/* '´' */ 0x00B4,  10, MO_AltRight|MO_DeadKey,
	/* '¸' */ 0x00B8,  13, MO_AltRight|MO_DeadKey,
	/* 'Á' */ 0x00C1,  41, MO_ShiftLeft,
	/* 'Ä' */ 0x00C4,  19, MO_AltRight,
	/* 'É' */ 0x00C9,  40, MO_ShiftLeft,
	/* 'Í' */ 0x00CD,  24, MO_AltRight,
	/* 'Ó' */ 0x00D3,  13, MO_ShiftLeft,
	/* 'Ö' */ 0x00D6,  11, MO_ShiftLeft,
	/* '×' */ 0x00D7,  28, MO_AltRight,
	/* 'Ú' */ 0x00DA,  28, MO_ShiftLeft,
	/* 'Ü' */ 0x00DC,  12, MO_ShiftLeft,
	/* 'ß' */ 0x00DF,  41, MO_AltRight,
	/* 'á' */ 0x00E1,  41, 0,
	/* 'ä' */ 0x00E4,  31, MO_AltRight,
	/* 'é' */ 0x00E9,  40, 0,
	/* 'í' */ 0x00ED,  37, MO_AltRight,
	/* 'ó' */ 0x00F3,  13, 0,
	/* 'ö' */ 0x00F6,  11, 0,
	/* '÷' */ 0x00F7,  27, MO_AltRight,
	/* 'ú' */ 0x00FA,  28, 0,
	/* 'ü' */ 0x00FC,  12, 0,
	/* ??? */ 0x0110,  33, MO_AltRight,						// LATIN CAPITAL LETTER D WITH STROKE
	/* ??? */ 0x0111,  32, MO_AltRight,						// LATIN SMALL LETTER D WITH STROKE
	/* ??? */ 0x0141,  39, MO_AltRight,						// LATIN CAPITAL LETTER L WITH STROKE
	/* ??? */ 0x0142,  38, MO_AltRight,						// LATIN SMALL LETTER L WITH STROKE
	/* ??? */ 0x0150,  27, MO_ShiftLeft,					// LATIN CAPITAL LETTER O WITH DOUBLE ACUTE
	/* ??? */ 0x0151,  27, 0,									// LATIN SMALL LETTER O WITH DOUBLE ACUTE
	/* ??? */ 0x0170,  42, MO_ShiftLeft,					// LATIN CAPITAL LETTER U WITH DOUBLE ACUTE
	/* ??? */ 0x0171,  42, 0,									// LATIN SMALL LETTER U WITH DOUBLE ACUTE
	/* ??? */ 0x02C7,   3, MO_AltRight|MO_DeadKey,		// CARON
	/* ??? */ 0x02D8,   5, MO_AltRight|MO_DeadKey,		// BREVE
	/* ??? */ 0x02D9,   9, MO_AltRight|MO_DeadKey,		// DOT ABOVE
	/* ??? */ 0x02DB,   7, MO_AltRight|MO_DeadKey,		// OGONEK
	/* ??? */ 0x02DD,  11, MO_AltRight|MO_DeadKey,		// DOUBLE ACUTE ACCENT
	/* '€' */ 0x20AC,  23, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Polish[43] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023,   4, MO_AltRight,
	/* '$' */ 0x0024,   5, MO_AltRight,
	/* '&' */ 0x0026,   8, MO_AltRight,
	/* ''' */ 0x0027,  41, MO_AltRight,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,   9, MO_AltRight,
	/* '+' */ 0x002B,   7, MO_ShiftLeft,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,  28, 0,
	/* ':' */ 0x003A,   4, MO_ShiftLeft,
	/* ';' */ 0x003B,  42, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,   5, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_AltRight,
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,  42, 0,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '^' */ 0x005E,   7, MO_AltRight,
	/* '_' */ 0x005F,   8, MO_ShiftLeft,
	/* '{' */ 0x007B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x007C,  42, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x007D,  28, MO_ShiftLeft|MO_AltRight,
	/* '¤' */ 0x00A4,  42, MO_AltRight,
	/* '§' */ 0x00A7,  28, MO_ShiftLeft,
	/* '´' */ 0x00B4,  13, MO_ShiftLeft,
	/* '·' */ 0x00B7,   1, MO_AltRight,
	/* 'ó' */ 0x00F3,  27, 0,
	/* ??? */ 0x0105,  41, 0,									// LATIN SMALL LETTER A WITH OGONEK
	/* ??? */ 0x0107,  55, MO_ShiftLeft,					// LATIN SMALL LETTER C WITH ACUTE
	/* ??? */ 0x0119,  41, MO_ShiftLeft,					// LATIN SMALL LETTER E WITH OGONEK
	/* ??? */ 0x0141,  40, MO_ShiftLeft,					// LATIN CAPITAL LETTER L WITH STROKE
	/* ??? */ 0x0142,  40, 0,									// LATIN SMALL LETTER L WITH STROKE
	/* ??? */ 0x0144,  54, MO_ShiftLeft,					// LATIN SMALL LETTER N WITH ACUTE
	/* ??? */ 0x015B,  53, MO_ShiftLeft,					// LATIN SMALL LETTER S WITH ACUTE
	/* ??? */ 0x017A,  27, MO_ShiftLeft,					// LATIN SMALL LETTER Z WITH ACUTE
	/* ??? */ 0x017B,  12, MO_ShiftLeft,					// LATIN CAPITAL LETTER Z WITH DOT ABOVE
	/* ??? */ 0x017C,  12, 0,									// LATIN SMALL LETTER Z WITH DOT ABOVE
	/* '‘' */ 0x2018,  13, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_SCS[36] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,   1, 0,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,   9, MO_AltRight,
	/* '+' */ 0x002B,  13, 0,
	/* '-' */ 0x002D,  12, MO_AltRight,
	/* '/' */ 0x002F,  12, 0,
	/* ':' */ 0x003A,  40, MO_ShiftLeft|MO_AltRight,
	/* ';' */ 0x003B,  40, MO_AltRight,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_AltRight,
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,  42, MO_AltRight,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '^' */ 0x005E,   7, MO_AltRight,
	/* '_' */ 0x005F,  12, MO_ShiftLeft|MO_AltRight,
	/* '`' */ 0x0060, NOS, 0,
	/* '{' */ 0x007B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x007C,  42, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x007D,  28, MO_ShiftLeft|MO_AltRight,
	/* ??? */ 0x0106,  41, MO_ShiftLeft,					// LATIN CAPITAL LETTER C WITH ACUTE
	/* ??? */ 0x0107,  41, 0,									// LATIN SMALL LETTER C WITH ACUTE
	/* ??? */ 0x010C,  40, MO_ShiftLeft,					// LATIN CAPITAL LETTER C WITH CARON
	/* ??? */ 0x010D,  40, 0,									// LATIN SMALL LETTER C WITH CARON
	/* ??? */ 0x0110,  28, MO_ShiftLeft,					// LATIN CAPITAL LETTER D WITH STROKE
	/* ??? */ 0x0111,  28, 0,									// LATIN SMALL LETTER D WITH STROKE
	/* 'Š' */ 0x0160,  27, MO_ShiftLeft,
	/* 'š' */ 0x0161,  27, 0,
	/* 'Ž' */ 0x017D,  42, MO_ShiftLeft,
	/* 'ž' */ 0x017E,  42, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Slovakian_DEC[64] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   2, MO_AltRight,
	/* '"' */ 0x0022,  40, MO_ShiftLeft,
	/* '#' */ 0x0023,   4, MO_AltRight,
	/* '$' */ 0x0024,   5, MO_AltRight,
	/* '%' */ 0x0025,   6, MO_AltRight,
	/* '&' */ 0x0026,   8, MO_AltRight,
	/* ''' */ 0x0027,  41, MO_AltRight,
	/* '(' */ 0x0028,  10, MO_AltRight,
	/* ')' */ 0x0029,  11, MO_AltRight,
	/* '*' */ 0x002A,   9, MO_AltRight,
	/* '+' */ 0x002B,   2, 0,
	/* '-' */ 0x002D,  12, MO_AltRight,
	/* '/' */ 0x002F,  27, MO_ShiftLeft,
	/* '0' */ 0x0030,  11, MO_ShiftLeft,
	/* '1' */ 0x0031,   2, MO_ShiftLeft,
	/* '2' */ 0x0032,   3, MO_ShiftLeft,
	/* '3' */ 0x0033,   4, MO_ShiftLeft,
	/* '4' */ 0x0034,   5, MO_ShiftLeft,
	/* '5' */ 0x0035,   6, MO_ShiftLeft,
	/* '6' */ 0x0036,   7, MO_ShiftLeft,
	/* '7' */ 0x0037,   8, MO_ShiftLeft,
	/* '8' */ 0x0038,   9, MO_ShiftLeft,
	/* '9' */ 0x0039,  10, MO_ShiftLeft,
	/* ':' */ 0x003A,  40, MO_ShiftLeft|MO_AltRight,
	/* ';' */ 0x003B,  40, MO_AltRight,
	/* '<' */ 0x003C,  53, MO_ShiftLeft|MO_AltRight,
	/* '=' */ 0x003D,  12, 0,
	/* '>' */ 0x003E,  54, MO_ShiftLeft|MO_AltRight,
	/* '?' */ 0x003F,  53, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_AltRight,
	/* 'Y' */ 0x0059,  22, MO_ShiftLeft|MO_AltRight,
	/* 'Z' */ 0x005A,  22, MO_ShiftLeft,
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,  42, MO_AltRight,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '^' */ 0x005E,   7, MO_AltRight,
	/* '_' */ 0x005F,  12, MO_ShiftLeft|MO_AltRight,
	/* '`' */ 0x0060,   1, MO_AltRight,
	/* 'y' */ 0x0079,  22, MO_AltRight,
	/* 'z' */ 0x007A,  22, 0,
	/* '{' */ 0x007B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x007C,  42, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x007D,  28, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x007E,   1, MO_ShiftLeft|MO_AltRight,
	/* '§' */ 0x00A7,  41, 0,
	/* '¨' */ 0x00A8,  45, MO_AltRight,
	/* '´' */ 0x00B4,  13, 0,
	/* 'ß' */ 0x00DF,   1, MO_ShiftLeft,
	/* 'á' */ 0x00E1,   9, 0,
	/* 'ä' */ 0x00E4,  28, 0,
	/* 'é' */ 0x00E9,  11, 0,
	/* 'í' */ 0x00ED,  10, 0,
	/* 'ô' */ 0x00F4,  40, 0,
	/* 'ú' */ 0x00FA,  27, 0,
	/* 'ý' */ 0x00FD,   8, 0,
	/* ??? */ 0x010D,   5, 0,									// LATIN SMALL LETTER C WITH CARON
	/* ??? */ 0x013E,   3, 0,									// LATIN SMALL LETTER L WITH CARON
	/* ??? */ 0x0148,  42, 0,									// LATIN SMALL LETTER N WITH CARON
	/* 'š' */ 0x0161,   4, 0,
	/* ??? */ 0x0165,   6, 0,									// LATIN SMALL LETTER T WITH CARON
	/* ??? */ 0x016F,   1, 0,									// LATIN SMALL LETTER U WITH RING ABOVE
	/* 'ž' */ 0x017E,   7, 0,
	/* ??? */ 0x02C7,  13, MO_ShiftLeft,					// CARON
	/* end */ 0,KN_ENDMARKER,0
};

// The next table is equal to another, so we save the memory
#define DEC_VT510_PC_Swedish          		DEC_VT510_PC_Finnish_Swedish

// TermId=  5
const CKeyFixup DEC_VT510_PC_Turkish_Q[50] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   1, 0,
	/* '#' */ 0x0023,   4, MO_AltRight,
	/* '$' */ 0x0024,   5, MO_AltRight,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,   3, MO_ShiftLeft,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  12, 0,
	/* '+' */ 0x002B,   5, MO_ShiftLeft,
	/* ',' */ 0x002C,  42, 0,
	/* '-' */ 0x002D,  13, 0,
	/* '.' */ 0x002E,  55, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
	/* ':' */ 0x003A,  55, MO_ShiftLeft,
	/* ';' */ 0x003B,  42, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,  17, MO_AltRight,
	/* '[' */ 0x005B,   9, MO_AltRight,
	/* '\' */ 0x005C,  12, MO_AltRight,
	/* ']' */ 0x005D,  10, MO_AltRight,
	/* '^' */ 0x005E,   4, MO_ShiftLeft|MO_DeadKey,
	/* '_' */ 0x005F,  13, MO_ShiftLeft,
	/* '`' */ 0x0060,  42, MO_AltRight|MO_DeadKey,
	/* 'i' */ 0x0069,  41, 0,
	/* '{' */ 0x007B,   8, MO_AltRight,
	/* '|' */ 0x007C,  45, MO_AltRight,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* '~' */ 0x007E,  28, MO_AltRight|MO_DeadKey,
	/* '¨' */ 0x00A8,  27, MO_AltRight|MO_DeadKey,
	/* '´' */ 0x00B4,  40, MO_AltRight|MO_DeadKey,
	/* 'Æ' */ 0x00C6,  31, MO_ShiftLeft|MO_AltRight,
	/* 'Ç' */ 0x00C7,  54, MO_ShiftLeft,
	/* 'Ö' */ 0x00D6,  53, MO_ShiftLeft,
	/* 'Ü' */ 0x00DC,  28, MO_ShiftLeft,
	/* 'ß' */ 0x00DF,  32, MO_AltRight,
	/* 'æ' */ 0x00E6,  31, MO_AltRight,
	/* 'ç' */ 0x00E7,  54, 0,
	/* 'é' */ 0x00E9,   1, MO_ShiftLeft,
	/* 'ö' */ 0x00F6,  53, 0,
	/* 'ü' */ 0x00FC,  28, 0,
	/* ??? */ 0x011E,  27, MO_ShiftLeft,					// LATIN CAPITAL LETTER G WITH BREVE
	/* ??? */ 0x011F,  27, 0,									// LATIN SMALL LETTER G WITH BREVE
	/* ??? */ 0x0130,  41, MO_ShiftLeft,					// LATIN CAPITAL LETTER I WITH DOT ABOVE
	/* ??? */ 0x0131,  24, 0,									// LATIN SMALL LETTER DOTLESS I
	/* ??? */ 0x015E,  40, MO_ShiftLeft,					// LATIN CAPITAL LETTER S WITH CEDILLA
	/* ??? */ 0x015F,  40, 0,									// LATIN SMALL LETTER S WITH CEDILLA
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Romanian[60] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '$' */ 0x0024,  40, MO_AltRight,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,  13, 0,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  13, MO_ShiftLeft,
	/* '+' */ 0x002B,  12, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,  49, MO_AltRight,
	/* 'Y' */ 0x0059,  46, MO_ShiftLeft,
	/* 'Z' */ 0x005A,  22, MO_ShiftLeft,
	/* '[' */ 0x005B,   1, MO_ShiftLeft,
	/* '\' */ 0x005C,  17, MO_AltRight,
	/* ']' */ 0x005D,   1, 0,
	/* '^' */ 0x005E,   4, MO_AltRight,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,   8, MO_AltRight,
	/* 'y' */ 0x0079,  46, 0,
	/* 'z' */ 0x007A,  22, 0,
	/* '{' */ 0x007B,  50, MO_AltRight,
	/* '|' */ 0x007C,  18, MO_AltRight,
	/* '}' */ 0x007D,  51, MO_AltRight,
	/* '~' */ 0x007E,   2, MO_AltRight,
	/* '¤' */ 0x00A4,   5, MO_ShiftLeft,
	/* '§' */ 0x00A7,  52, MO_AltRight,
	/* '¨' */ 0x00A8,  12, MO_AltRight,
	/* '°' */ 0x00B0,   6, MO_AltRight,
	/* '´' */ 0x00B4,  10, MO_AltRight,
	/* '·' */ 0x00B7,   9, MO_AltRight,
	/* '¸' */ 0x00B8,  13, MO_AltRight,
	/* 'Â' */ 0x00C2,  42, MO_ShiftLeft,
	/* 'Î' */ 0x00CE,  28, MO_ShiftLeft,
	/* '×' */ 0x00D7,  28, MO_AltRight,
	/* 'ß' */ 0x00DF,  41, MO_AltRight,
	/* 'â' */ 0x00E2,  42, 0,
	/* 'î' */ 0x00EE,  28, 0,
	/* '÷' */ 0x00F7,  27, MO_AltRight,
	/* ??? */ 0x0102,  27, MO_ShiftLeft,					// LATIN CAPITAL LETTER A WITH BREVE
	/* ??? */ 0x0103,  27, 0,									// LATIN SMALL LETTER A WITH BREVE
	/* ??? */ 0x0110,  33, MO_AltRight,						// LATIN CAPITAL LETTER D WITH STROKE
	/* ??? */ 0x0111,  32, MO_AltRight,						// LATIN SMALL LETTER D WITH STROKE
	/* ??? */ 0x0141,  39, MO_AltRight,						// LATIN CAPITAL LETTER L WITH STROKE
	/* ??? */ 0x0142,  38, MO_AltRight,						// LATIN SMALL LETTER L WITH STROKE
	/* ??? */ 0x015E,  40, MO_ShiftLeft,					// LATIN CAPITAL LETTER S WITH CEDILLA
	/* ??? */ 0x015F,  40, 0,									// LATIN SMALL LETTER S WITH CEDILLA
	/* ??? */ 0x0162,  41, MO_ShiftLeft,					// LATIN CAPITAL LETTER T WITH CEDILLA
	/* ??? */ 0x0163,  41, 0,									// LATIN SMALL LETTER T WITH CEDILLA
	/* ??? */ 0x02C7,   3, MO_AltRight,						// CARON
	/* ??? */ 0x02D8,   5, MO_AltRight,						// BREVE
	/* ??? */ 0x02DB,   7, MO_AltRight,						// OGONEK
	/* ??? */ 0x02DD,  11, MO_AltRight,						// DOUBLE ACUTE ACCENT
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Russian[143] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023, NOS, 0,
	/* '$' */ 0x0024, NOS, 0,
	/* '&' */ 0x0026, NOS, 0,
	/* ''' */ 0x0027, NOS, 0,
	/* ',' */ 0x002C,  55, MO_ShiftLeft,
	/* '.' */ 0x002E,  55, 0,
	/* '/' */ 0x002F,  42, MO_ShiftLeft,
	/* ':' */ 0x003A,   7, MO_ShiftLeft,
	/* ';' */ 0x003B,   5, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,   8, MO_ShiftLeft,
	/* '@' */ 0x0040, NOS, 0,
	/* 'A' */ 0x0041, NOS, 0,
	/* 'B' */ 0x0042, NOS, 0,
	/* 'C' */ 0x0043, NOS, 0,
	/* 'D' */ 0x0044, NOS, 0,
	/* 'E' */ 0x0045, NOS, 0,
	/* 'F' */ 0x0046, NOS, 0,
	/* 'G' */ 0x0047, NOS, 0,
	/* 'H' */ 0x0048, NOS, 0,
	/* 'I' */ 0x0049, NOS, 0,
	/* 'J' */ 0x004A, NOS, 0,
	/* 'K' */ 0x004B, NOS, 0,
	/* 'L' */ 0x004C, NOS, 0,
	/* 'M' */ 0x004D, NOS, 0,
	/* 'N' */ 0x004E, NOS, 0,
	/* 'O' */ 0x004F, NOS, 0,
	/* 'P' */ 0x0050, NOS, 0,
	/* 'Q' */ 0x0051, NOS, 0,
	/* 'R' */ 0x0052, NOS, 0,
	/* 'S' */ 0x0053, NOS, 0,
	/* 'T' */ 0x0054, NOS, 0,
	/* 'U' */ 0x0055, NOS, 0,
	/* 'V' */ 0x0056, NOS, 0,
	/* 'W' */ 0x0057, NOS, 0,
	/* 'X' */ 0x0058, NOS, 0,
	/* 'Y' */ 0x0059, NOS, 0,
	/* 'Z' */ 0x005A, NOS, 0,
	/* '[' */ 0x005B, NOS, 0,
	/* '\' */ 0x005C,  42, 0,
	/* ']' */ 0x005D, NOS, 0,
	/* '^' */ 0x005E, NOS, 0,
	/* '`' */ 0x0060, NOS, 0,
	/* 'a' */ 0x0061, NOS, 0,
	/* 'b' */ 0x0062, NOS, 0,
	/* 'c' */ 0x0063, NOS, 0,
	/* 'd' */ 0x0064, NOS, 0,
	/* 'e' */ 0x0065, NOS, 0,
	/* 'f' */ 0x0066, NOS, 0,
	/* 'g' */ 0x0067, NOS, 0,
	/* 'h' */ 0x0068, NOS, 0,
	/* 'i' */ 0x0069, NOS, 0,
	/* 'j' */ 0x006A, NOS, 0,
	/* 'k' */ 0x006B, NOS, 0,
	/* 'l' */ 0x006C, NOS, 0,
	/* 'm' */ 0x006D, NOS, 0,
	/* 'n' */ 0x006E, NOS, 0,
	/* 'o' */ 0x006F, NOS, 0,
	/* 'p' */ 0x0070, NOS, 0,
	/* 'q' */ 0x0071, NOS, 0,
	/* 'r' */ 0x0072, NOS, 0,
	/* 's' */ 0x0073, NOS, 0,
	/* 't' */ 0x0074, NOS, 0,
	/* 'u' */ 0x0075, NOS, 0,
	/* 'v' */ 0x0076, NOS, 0,
	/* 'w' */ 0x0077, NOS, 0,
	/* 'x' */ 0x0078, NOS, 0,
	/* 'y' */ 0x0079, NOS, 0,
	/* 'z' */ 0x007A, NOS, 0,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* ??? */ 0x0401,   1, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER IO
	/* ??? */ 0x0410,  34, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER A
	/* ??? */ 0x0411,  53, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER BE
	/* ??? */ 0x0412,  33, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER VE
	/* ??? */ 0x0413,  23, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER GHE
	/* ??? */ 0x0414,  39, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER DE
	/* ??? */ 0x0415,  21, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER IE
	/* ??? */ 0x0416,  40, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZHE
	/* ??? */ 0x0417,  26, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZE
	/* ??? */ 0x0418,  50, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER I
	/* ??? */ 0x0419,  17, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHORT I
	/* ??? */ 0x041A,  20, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER KA
	/* ??? */ 0x041B,  38, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EL
	/* ??? */ 0x041C,  49, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EM
	/* ??? */ 0x041D,  22, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EN
	/* ??? */ 0x041E,  37, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER O
	/* ??? */ 0x041F,  35, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER PE
	/* ??? */ 0x0420,  36, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ER
	/* ??? */ 0x0421,  48, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ES
	/* ??? */ 0x0422,  51, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TE
	/* ??? */ 0x0423,  19, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER U
	/* ??? */ 0x0424,  31, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EF
	/* ??? */ 0x0425,  27, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER HA
	/* ??? */ 0x0426,  18, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TSE
	/* ??? */ 0x0427,  47, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER CHE
	/* ??? */ 0x0428,  24, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHA
	/* ??? */ 0x0429,  25, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHCHA
	/* ??? */ 0x042A,  28, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER HARD SIGN
	/* ??? */ 0x042B,  32, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YERU
	/* ??? */ 0x042C,  52, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SOFT SIGN
	/* ??? */ 0x042D,  41, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER E
	/* ??? */ 0x042E,  54, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YU
	/* ??? */ 0x042F,  46, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YA
	/* ??? */ 0x0430,  34, 0,									// CYRILLIC SMALL LETTER A
	/* ??? */ 0x0431,  53, 0,									// CYRILLIC SMALL LETTER BE
	/* ??? */ 0x0432,  33, 0,									// CYRILLIC SMALL LETTER VE
	/* ??? */ 0x0433,  23, 0,									// CYRILLIC SMALL LETTER GHE
	/* ??? */ 0x0434,  39, 0,									// CYRILLIC SMALL LETTER DE
	/* ??? */ 0x0435,  21, 0,									// CYRILLIC SMALL LETTER IE
	/* ??? */ 0x0436,  40, 0,									// CYRILLIC SMALL LETTER ZHE
	/* ??? */ 0x0437,  26, 0,									// CYRILLIC SMALL LETTER ZE
	/* ??? */ 0x0438,  50, 0,									// CYRILLIC SMALL LETTER I
	/* ??? */ 0x0439,  17, 0,									// CYRILLIC SMALL LETTER SHORT I
	/* ??? */ 0x043A,  20, 0,									// CYRILLIC SMALL LETTER KA
	/* ??? */ 0x043B,  38, 0,									// CYRILLIC SMALL LETTER EL
	/* ??? */ 0x043C,  49, 0,									// CYRILLIC SMALL LETTER EM
	/* ??? */ 0x043D,  22, 0,									// CYRILLIC SMALL LETTER EN
	/* ??? */ 0x043E,  37, 0,									// CYRILLIC SMALL LETTER O
	/* ??? */ 0x043F,  35, 0,									// CYRILLIC SMALL LETTER PE
	/* ??? */ 0x0440,  36, 0,									// CYRILLIC SMALL LETTER ER
	/* ??? */ 0x0441,  48, 0,									// CYRILLIC SMALL LETTER ES
	/* ??? */ 0x0442,  51, 0,									// CYRILLIC SMALL LETTER TE
	/* ??? */ 0x0443,  19, 0,									// CYRILLIC SMALL LETTER U
	/* ??? */ 0x0444,  31, 0,									// CYRILLIC SMALL LETTER EF
	/* ??? */ 0x0445,  27, 0,									// CYRILLIC SMALL LETTER HA
	/* ??? */ 0x0446,  18, 0,									// CYRILLIC SMALL LETTER TSE
	/* ??? */ 0x0447,  47, 0,									// CYRILLIC SMALL LETTER CHE
	/* ??? */ 0x0448,  24, 0,									// CYRILLIC SMALL LETTER SHA
	/* ??? */ 0x0449,  25, 0,									// CYRILLIC SMALL LETTER SHCHA
	/* ??? */ 0x044A,  28, 0,									// CYRILLIC SMALL LETTER HARD SIGN
	/* ??? */ 0x044B,  32, 0,									// CYRILLIC SMALL LETTER YERU
	/* ??? */ 0x044C,  52, 0,									// CYRILLIC SMALL LETTER SOFT SIGN
	/* ??? */ 0x044D,  41, 0,									// CYRILLIC SMALL LETTER E
	/* ??? */ 0x044E,  54, 0,									// CYRILLIC SMALL LETTER YU
	/* ??? */ 0x044F,  46, 0,									// CYRILLIC SMALL LETTER YA
	/* ??? */ 0x0451,   1, 0,									// CYRILLIC SMALL LETTER IO
	/* ??? */ 0x2116,   4, MO_ShiftLeft,					// NUMERO SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Turkish_F[110] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023,   4, MO_AltRight,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,   8, MO_ShiftLeft,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,   1, MO_ShiftLeft,
	/* '+' */ 0x002B,   1, 0,
	/* ',' */ 0x002C,  55, 0,
	/* '-' */ 0x002D,  13, 0,
	/* '/' */ 0x002F,  12, 0,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  55, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,  17, MO_AltRight,
	/* 'A' */ 0x0041,  34, MO_ShiftLeft,
	/* 'B' */ 0x0042,  53, MO_ShiftLeft,
	/* 'C' */ 0x0043,  49, MO_ShiftLeft,
	/* 'D' */ 0x0044,  22, MO_ShiftLeft,
	/* 'E' */ 0x0045,  33, MO_ShiftLeft,
	/* 'F' */ 0x0046,  17, MO_ShiftLeft,
	/* 'G' */ 0x0047,  18, MO_ShiftLeft,
	/* 'H' */ 0x0048,  25, MO_ShiftLeft,
	/* 'I' */ 0x0049, NOS, 0,
	/* 'J' */ 0x004A,  46, MO_ShiftLeft,
	/* 'K' */ 0x004B,  37, MO_ShiftLeft,
	/* 'M' */ 0x004D,  38, MO_ShiftLeft,
	/* 'N' */ 0x004E,  24, MO_ShiftLeft,
	/* 'O' */ 0x004F,  21, MO_ShiftLeft,
	/* 'Q' */ 0x0051,  27, MO_ShiftLeft,
	/* 'R' */ 0x0052,  23, MO_ShiftLeft,
	/* 'S' */ 0x0053,  52, MO_ShiftLeft,
	/* 'T' */ 0x0054,  36, MO_ShiftLeft,
	/* 'U' */ 0x0055,  31, MO_ShiftLeft,
	/* 'V' */ 0x0056,  48, MO_ShiftLeft,
	/* 'W' */ 0x0057,  28, MO_ShiftLeft,
	/* 'X' */ 0x0058,  42, MO_ShiftLeft,
	/* 'Y' */ 0x0059,  40, MO_ShiftLeft,
	/* 'Z' */ 0x005A,  51, MO_ShiftLeft,
	/* '[' */ 0x005B,   9, MO_AltRight,
	/* '\' */ 0x005C,  12, MO_AltRight,
	/* ']' */ 0x005D,  10, MO_AltRight,
	/* '^' */ 0x005E,   4, MO_ShiftLeft|MO_DeadKey,
	/* '_' */ 0x005F,  13, MO_ShiftLeft,
	/* '`' */ 0x0060,  42, MO_AltRight|MO_DeadKey,
	/* 'a' */ 0x0061,  34, 0,
	/* 'b' */ 0x0062,  53, 0,
	/* 'c' */ 0x0063,  49, 0,
	/* 'd' */ 0x0064,  22, 0,
	/* 'e' */ 0x0065,  33, 0,
	/* 'f' */ 0x0066,  17, 0,
	/* 'g' */ 0x0067,  18, 0,
	/* 'h' */ 0x0068,  25, 0,
	/* 'i' */ 0x0069,  20, MO_ShiftLeft,
	/* 'j' */ 0x006A,  46, 0,
	/* 'k' */ 0x006B,  37, 0,
	/* 'm' */ 0x006D,  38, 0,
	/* 'n' */ 0x006E,  24, 0,
	/* 'o' */ 0x006F,  21, 0,
	/* 'q' */ 0x0071,  27, 0,
	/* 'r' */ 0x0072,  23, 0,
	/* 's' */ 0x0073,  52, 0,
	/* 't' */ 0x0074,  36, 0,
	/* 'u' */ 0x0075,  31, 0,
	/* 'v' */ 0x0076,  48, 0,
	/* 'w' */ 0x0077,  28, 0,
	/* 'x' */ 0x0078,  42, 0,
	/* 'y' */ 0x0079,  40, 0,
	/* 'z' */ 0x007A,  51, 0,
	/* '{' */ 0x007B,   8, MO_AltRight,
	/* '|' */ 0x007C,  45, MO_AltRight,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* '~' */ 0x007E,  28, MO_AltRight|MO_DeadKey,
	/* '¢' */ 0x00A2,  48, MO_AltRight,
	/* '£' */ 0x00A3,  26, MO_AltRight,
	/* '¥' */ 0x00A5,  22, MO_AltRight,
	/* '¨' */ 0x00A8,  27, MO_AltRight|MO_DeadKey,
	/* '«' */ 0x00AB,  46, MO_AltRight,
	/* '¬' */ 0x00AC,   1, MO_AltRight,
	/* '²' */ 0x00B2,   3, MO_AltRight,
	/* '´' */ 0x00B4,  40, MO_AltRight|MO_DeadKey,
	/* 'µ' */ 0x00B5,  52, MO_AltRight,
	/* '¶' */ 0x00B6,  20, MO_AltRight,
	/* '¹' */ 0x00B9,   2, MO_AltRight,
	/* 'º' */ 0x00BA,  25, MO_AltRight,
	/* '»' */ 0x00BB,  47, MO_AltRight,
	/* '¼' */ 0x00BC,   5, MO_AltRight,
	/* '½' */ 0x00BD,   6, MO_AltRight,
	/* '¾' */ 0x00BE,   7, MO_AltRight,
	/* 'Æ' */ 0x00C6,  31, MO_ShiftLeft|MO_AltRight,
	/* 'Ç' */ 0x00C7,  50, MO_ShiftLeft,
	/* 'Ö' */ 0x00D6,  47, MO_ShiftLeft,
	/* '×' */ 0x00D7,  53, MO_AltRight,
	/* 'Ü' */ 0x00DC,  35, MO_ShiftLeft,
	/* 'ß' */ 0x00DF,  32, MO_AltRight,
	/* 'æ' */ 0x00E6,  31, MO_AltRight,
	/* 'ç' */ 0x00E7,  50, 0,
	/* 'ö' */ 0x00F6,  47, 0,
	/* '÷' */ 0x00F7,  54, MO_AltRight,
	/* 'ü' */ 0x00FC,  35, 0,
	/* ??? */ 0x011E,  19, MO_ShiftLeft,					// LATIN CAPITAL LETTER G WITH BREVE
	/* ??? */ 0x011F,  19, 0,									// LATIN SMALL LETTER G WITH BREVE
	/* ??? */ 0x0130,  32, MO_ShiftLeft,					// LATIN CAPITAL LETTER I WITH DOT ABOVE
	/* ??? */ 0x0131,  20, 0,									// LATIN SMALL LETTER DOTLESS I
	/* ??? */ 0x015E,  41, MO_ShiftLeft,					// LATIN CAPITAL LETTER S WITH CEDILLA
	/* ??? */ 0x015F,  41, 0,									// LATIN SMALL LETTER S WITH CEDILLA
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Japanese_ASCII[22] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,   8, MO_ShiftLeft,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  41, MO_ShiftLeft,
	/* '+' */ 0x002B,  40, MO_ShiftLeft,
	/* ':' */ 0x003A,  41, 0,
	/* '=' */ 0x003D,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,  27, 0,
	/* '[' */ 0x005B,  28, 0,
	/* '\' */ 0x005C,  56, 0,
	/* ']' */ 0x005D,  42, 0,
	/* '^' */ 0x005E,  13, 0,
	/* '_' */ 0x005F,  56, MO_ShiftLeft,
	/* '`' */ 0x0060,  27, MO_ShiftLeft,
	/* '{' */ 0x007B,  28, MO_ShiftLeft,
	/* '|' */ 0x007C,  14, MO_ShiftLeft,
	/* '}' */ 0x007D,  42, MO_ShiftLeft,
	/* '~' */ 0x007E,  13, MO_ShiftLeft,
	/* '¥' */ 0x00A5,  14, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Swiss_French[48] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,  28, MO_ShiftLeft,
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023,   4, MO_AltRight,
	/* '$' */ 0x0024,  42, 0,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027, NOS, 0,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,   4, MO_ShiftLeft,
	/* '+' */ 0x002B,   2, MO_ShiftLeft,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F, NOS, 0,
	/* '@' */ 0x0040,   3, MO_AltRight,
	/* 'Y' */ 0x0059,  46, MO_ShiftLeft,
	/* 'Z' */ 0x005A,  22, MO_ShiftLeft,
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,  45, MO_AltRight,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '^' */ 0x005E,  13, MO_DeadKey,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,  13, MO_ShiftLeft|MO_DeadKey,
	/* 'y' */ 0x0079,  46, 0,
	/* 'z' */ 0x007A,  22, 0,
	/* '{' */ 0x007B,  41, MO_AltRight,
	/* '|' */ 0x007C,   8, MO_AltRight,
	/* '}' */ 0x007D,  42, MO_AltRight,
	/* '~' */ 0x007E,  13, MO_AltRight|MO_DeadKey,
	/* '£' */ 0x00A3,  42, MO_ShiftLeft,
	/* '¦' */ 0x00A6,   2, MO_AltRight,
	/* '§' */ 0x00A7,   1, 0,
	/* '¨' */ 0x00A8,  28, MO_DeadKey,
	/* '°' */ 0x00B0,   1, MO_ShiftLeft,
	/* '´' */ 0x00B4,  12, MO_AltRight|MO_DeadKey,
	/* 'à' */ 0x00E0,  41, 0,
	/* 'ä' */ 0x00E4,  41, MO_ShiftLeft,
	/* 'ç' */ 0x00E7,   5, MO_ShiftLeft,
	/* 'è' */ 0x00E8,  27, 0,
	/* 'é' */ 0x00E9,  40, 0,
	/* 'ö' */ 0x00F6,  40, MO_ShiftLeft,
	/* 'ü' */ 0x00FC,  27, MO_ShiftLeft,
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_USA_International[66] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,  41, MO_ShiftLeft|MO_DeadKey,
	/* ''' */ 0x0027,  41, MO_DeadKey,
	/* '^' */ 0x005E,   7, MO_ShiftLeft|MO_DeadKey,
	/* '`' */ 0x0060,   1, MO_DeadKey,
	/* '~' */ 0x007E,   1, MO_ShiftLeft|MO_DeadKey,
	/* '¡' */ 0x00A1,   2, MO_AltRight,
	/* '¢' */ 0x00A2,  48, MO_ShiftLeft|MO_AltRight,
	/* '£' */ 0x00A3,   5, MO_ShiftLeft|MO_AltRight,
	/* '¤' */ 0x00A4,   5, MO_AltRight,
	/* '¥' */ 0x00A5,  12, MO_AltRight,
	/* '¦' */ 0x00A6,  29, MO_ShiftLeft|MO_AltRight,
	/* '§' */ 0x00A7,  32, MO_ShiftLeft|MO_AltRight,
	/* '¨' */ 0x00A8,  41, MO_ShiftLeft|MO_AltRight,
	/* '©' */ 0x00A9,  48, MO_AltRight,
	/* '«' */ 0x00AB,  27, MO_AltRight,
	/* '¬' */ 0x00AC,  29, MO_AltRight,
	/* '®' */ 0x00AE,  20, MO_AltRight,
	/* '°' */ 0x00B0,  40, MO_ShiftLeft|MO_AltRight,
	/* '²' */ 0x00B2,   3, MO_AltRight,
	/* '³' */ 0x00B3,   4, MO_AltRight,
	/* '´' */ 0x00B4,  41, MO_AltRight,
	/* 'µ' */ 0x00B5,  52, MO_AltRight,
	/* '¶' */ 0x00B6,  40, MO_AltRight,
	/* '¹' */ 0x00B9,   2, MO_ShiftLeft|MO_AltRight,
	/* '»' */ 0x00BB,  28, MO_AltRight,
	/* '¼' */ 0x00BC,   7, MO_AltRight,
	/* '½' */ 0x00BD,   8, MO_AltRight,
	/* '¾' */ 0x00BE,   9, MO_AltRight,
	/* '¿' */ 0x00BF,  55, MO_AltRight,
	/* 'Á' */ 0x00C1,  31, MO_ShiftLeft|MO_AltRight,
	/* 'Ä' */ 0x00C4,  17, MO_ShiftLeft|MO_AltRight,
	/* 'Å' */ 0x00C5,  18, MO_ShiftLeft|MO_AltRight,
	/* 'Æ' */ 0x00C6,  46, MO_ShiftLeft|MO_AltRight,
	/* 'Ç' */ 0x00C7,  53, MO_ShiftLeft|MO_AltRight,
	/* 'É' */ 0x00C9,  19, MO_ShiftLeft|MO_AltRight,
	/* 'Í' */ 0x00CD,  24, MO_ShiftLeft|MO_AltRight,
	/* 'Ð' */ 0x00D0,  33, MO_ShiftLeft|MO_AltRight,
	/* 'Ñ' */ 0x00D1,  51, MO_ShiftLeft|MO_AltRight,
	/* 'Ó' */ 0x00D3,  25, MO_ShiftLeft|MO_AltRight,
	/* 'Ö' */ 0x00D6,  26, MO_ShiftLeft|MO_AltRight,
	/* '×' */ 0x00D7,  13, MO_AltRight,
	/* 'Ø' */ 0x00D8,  39, MO_ShiftLeft|MO_AltRight,
	/* 'Ú' */ 0x00DA,  23, MO_ShiftLeft|MO_AltRight,
	/* 'Ü' */ 0x00DC,  22, MO_ShiftLeft|MO_AltRight,
	/* 'Þ' */ 0x00DE,  21, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0x00DF,  32, MO_AltRight,
	/* 'á' */ 0x00E1,  31, MO_AltRight,
	/* 'ä' */ 0x00E4,  17, MO_AltRight,
	/* 'å' */ 0x00E5,  18, MO_AltRight,
	/* 'æ' */ 0x00E6,  46, MO_AltRight,
	/* 'ç' */ 0x00E7,  53, MO_AltRight,
	/* 'é' */ 0x00E9,  19, MO_AltRight,
	/* 'í' */ 0x00ED,  24, MO_AltRight,
	/* 'ð' */ 0x00F0,  33, MO_AltRight,
	/* 'ñ' */ 0x00F1,  51, MO_AltRight,
	/* 'ó' */ 0x00F3,  25, MO_AltRight,
	/* 'ö' */ 0x00F6,  26, MO_AltRight,
	/* '÷' */ 0x00F7,  13, MO_ShiftLeft|MO_AltRight,
	/* 'ø' */ 0x00F8,  39, MO_AltRight,
	/* 'ú' */ 0x00FA,  23, MO_AltRight,
	/* 'ü' */ 0x00FC,  22, MO_AltRight,
	/* 'þ' */ 0x00FE,  21, MO_AltRight,
	/* '‘' */ 0x2018,  10, MO_AltRight,
	/* '’' */ 0x2019,  11, MO_AltRight,
	/* '€' */ 0x20AC,   6, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Slovenian[58] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,  12, 0,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  13, MO_ShiftLeft,
	/* '+' */ 0x002B,  13, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,  49, MO_AltRight,
	/* 'Y' */ 0x0059,  46, MO_ShiftLeft,
	/* 'Z' */ 0x005A,  22, MO_ShiftLeft,
	/* '[' */ 0x005B,  34, MO_AltRight,
	/* '\' */ 0x005C,  17, MO_AltRight,
	/* ']' */ 0x005D,  35, MO_AltRight,
	/* '^' */ 0x005E,   4, MO_AltRight|MO_DeadKey,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,   8, MO_AltRight,
	/* 'y' */ 0x0079,  46, 0,
	/* 'z' */ 0x007A,  22, 0,
	/* '{' */ 0x007B,  50, MO_AltRight,
	/* '|' */ 0x007C,  18, MO_AltRight,
	/* '}' */ 0x007D,  51, MO_AltRight,
	/* '~' */ 0x007E,   2, MO_AltRight,
	/* '¤' */ 0x00A4,  42, MO_AltRight,
	/* '§' */ 0x00A7,  52, MO_AltRight,
	/* '¨' */ 0x00A8,   1, MO_ShiftLeft|MO_DeadKey,
	/* '°' */ 0x00B0,   6, MO_AltRight|MO_DeadKey,
	/* '´' */ 0x00B4,  10, MO_AltRight|MO_DeadKey,
	/* '¸' */ 0x00B8,   1, MO_DeadKey,
	/* '×' */ 0x00D7,  28, MO_AltRight,
	/* 'ß' */ 0x00DF,  41, MO_AltRight,
	/* '÷' */ 0x00F7,  27, MO_AltRight,
	/* ??? */ 0x0106,  41, MO_ShiftLeft,					// LATIN CAPITAL LETTER C WITH ACUTE
	/* ??? */ 0x0107,  41, 0,									// LATIN SMALL LETTER C WITH ACUTE
	/* ??? */ 0x010C,  40, MO_ShiftLeft,					// LATIN CAPITAL LETTER C WITH CARON
	/* ??? */ 0x010D,  40, 0,									// LATIN SMALL LETTER C WITH CARON
	/* ??? */ 0x0110,  28, MO_ShiftLeft,					// LATIN CAPITAL LETTER D WITH STROKE
	/* ??? */ 0x0111,  28, 0,									// LATIN SMALL LETTER D WITH STROKE
	/* ??? */ 0x0141,  39, MO_AltRight,						// LATIN CAPITAL LETTER L WITH STROKE
	/* ??? */ 0x0142,  38, MO_AltRight,						// LATIN SMALL LETTER L WITH STROKE
	/* 'Š' */ 0x0160,  27, MO_ShiftLeft,
	/* 'š' */ 0x0161,  27, 0,
	/* 'Ž' */ 0x017D,  42, MO_ShiftLeft,
	/* 'ž' */ 0x017E,  42, 0,
	/* ??? */ 0x02C7,   3, MO_AltRight|MO_DeadKey,		// CARON
	/* ??? */ 0x02D8,   5, MO_AltRight|MO_DeadKey,		// BREVE
	/* ??? */ 0x02D9,   9, MO_AltRight|MO_DeadKey,		// DOT ABOVE
	/* ??? */ 0x02DB,   7, MO_AltRight|MO_DeadKey,		// OGONEK
	/* ??? */ 0x02DD,  11, MO_AltRight|MO_DeadKey,		// DOUBLE ACUTE ACCENT
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// The next table is equal to another, so we save the memory
#define DEC_VT510_PC_Croatian         		DEC_VT510_PC_Slovenian

// The next table is equal to another, so we save the memory
#define DEC_VT510_PC_Bosnian          		DEC_VT510_PC_Slovenian

// TermId=  5
const CKeyFixup DEC_VT510_PC_Macedonian[143] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022, NOS, 0,
	/* '#' */ 0x0023, NOS, 0,
	/* '$' */ 0x0024, NOS, 0,
	/* ''' */ 0x0027, NOS, 0,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C, NOS, 0,
	/* '>' */ 0x003E, NOS, 0,
	/* '@' */ 0x0040,  49, MO_AltRight,
	/* 'A' */ 0x0041, NOS, 0,
	/* 'B' */ 0x0042, NOS, 0,
	/* 'C' */ 0x0043, NOS, 0,
	/* 'D' */ 0x0044, NOS, 0,
	/* 'E' */ 0x0045, NOS, 0,
	/* 'F' */ 0x0046, NOS, 0,
	/* 'G' */ 0x0047, NOS, 0,
	/* 'H' */ 0x0048, NOS, 0,
	/* 'I' */ 0x0049, NOS, 0,
	/* 'J' */ 0x004A, NOS, 0,
	/* 'K' */ 0x004B, NOS, 0,
	/* 'L' */ 0x004C, NOS, 0,
	/* 'M' */ 0x004D, NOS, 0,
	/* 'N' */ 0x004E, NOS, 0,
	/* 'O' */ 0x004F, NOS, 0,
	/* 'P' */ 0x0050, NOS, 0,
	/* 'Q' */ 0x0051, NOS, 0,
	/* 'R' */ 0x0052, NOS, 0,
	/* 'S' */ 0x0053, NOS, 0,
	/* 'T' */ 0x0054, NOS, 0,
	/* 'U' */ 0x0055, NOS, 0,
	/* 'V' */ 0x0056, NOS, 0,
	/* 'W' */ 0x0057, NOS, 0,
	/* 'X' */ 0x0058, NOS, 0,
	/* 'Y' */ 0x0059, NOS, 0,
	/* 'Z' */ 0x005A, NOS, 0,
	/* '[' */ 0x005B,  34, MO_AltRight,
	/* '\' */ 0x005C, NOS, 0,
	/* ']' */ 0x005D,  35, MO_AltRight,
	/* '^' */ 0x005E, NOS, 0,
	/* 'a' */ 0x0061, NOS, 0,
	/* 'b' */ 0x0062, NOS, 0,
	/* 'c' */ 0x0063, NOS, 0,
	/* 'd' */ 0x0064, NOS, 0,
	/* 'e' */ 0x0065, NOS, 0,
	/* 'f' */ 0x0066, NOS, 0,
	/* 'g' */ 0x0067, NOS, 0,
	/* 'h' */ 0x0068, NOS, 0,
	/* 'i' */ 0x0069, NOS, 0,
	/* 'j' */ 0x006A, NOS, 0,
	/* 'k' */ 0x006B, NOS, 0,
	/* 'l' */ 0x006C, NOS, 0,
	/* 'm' */ 0x006D, NOS, 0,
	/* 'n' */ 0x006E, NOS, 0,
	/* 'o' */ 0x006F, NOS, 0,
	/* 'p' */ 0x0070, NOS, 0,
	/* 'q' */ 0x0071, NOS, 0,
	/* 'r' */ 0x0072, NOS, 0,
	/* 's' */ 0x0073, NOS, 0,
	/* 't' */ 0x0074, NOS, 0,
	/* 'u' */ 0x0075, NOS, 0,
	/* 'v' */ 0x0076, NOS, 0,
	/* 'w' */ 0x0077, NOS, 0,
	/* 'x' */ 0x0078, NOS, 0,
	/* 'y' */ 0x0079, NOS, 0,
	/* 'z' */ 0x007A, NOS, 0,
	/* '{' */ 0x007B,  50, MO_AltRight,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D,  51, MO_AltRight,
	/* '§' */ 0x00A7,  52, MO_AltRight,
	/* ??? */ 0x0401,  45, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER IO
	/* ??? */ 0x0402,  27, MO_AltRight,						// CYRILLIC CAPITAL LETTER DJE
	/* ??? */ 0x0403,  28, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER GJE
	/* ??? */ 0x0405,  22, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER DZE
	/* ??? */ 0x0408,  37, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER JE
	/* ??? */ 0x0409,  17, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER LJE
	/* ??? */ 0x040A,  18, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER NJE
	/* ??? */ 0x040B,  40, MO_AltRight,						// CYRILLIC CAPITAL LETTER TSHE
	/* ??? */ 0x040C,  41, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER KJE
	/* ??? */ 0x040F,  47, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER DZHE
	/* ??? */ 0x0410,  31, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER A
	/* ??? */ 0x0411,  50, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER BE
	/* ??? */ 0x0412,  49, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER VE
	/* ??? */ 0x0413,  35, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER GHE
	/* ??? */ 0x0414,  33, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER DE
	/* ??? */ 0x0415,  19, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER IE
	/* ??? */ 0x0416,  42, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZHE
	/* ??? */ 0x0417,  46, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZE
	/* ??? */ 0x0418,  24, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER I
	/* ??? */ 0x041A,  38, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER KA
	/* ??? */ 0x041B,  39, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EL
	/* ??? */ 0x041C,  52, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EM
	/* ??? */ 0x041D,  51, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EN
	/* ??? */ 0x041E,  25, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER O
	/* ??? */ 0x041F,  26, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER PE
	/* ??? */ 0x0420,  20, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ER
	/* ??? */ 0x0421,  32, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ES
	/* ??? */ 0x0422,  21, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TE
	/* ??? */ 0x0423,  23, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER U
	/* ??? */ 0x0424,  34, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EF
	/* ??? */ 0x0425,  36, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER HA
	/* ??? */ 0x0426,  48, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TSE
	/* ??? */ 0x0427,  40, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER CHE
	/* ??? */ 0x0428,  27, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHA
	/* ??? */ 0x0430,  31, 0,									// CYRILLIC SMALL LETTER A
	/* ??? */ 0x0431,  50, 0,									// CYRILLIC SMALL LETTER BE
	/* ??? */ 0x0432,  49, 0,									// CYRILLIC SMALL LETTER VE
	/* ??? */ 0x0433,  35, 0,									// CYRILLIC SMALL LETTER GHE
	/* ??? */ 0x0434,  33, 0,									// CYRILLIC SMALL LETTER DE
	/* ??? */ 0x0435,  19, 0,									// CYRILLIC SMALL LETTER IE
	/* ??? */ 0x0436,  42, 0,									// CYRILLIC SMALL LETTER ZHE
	/* ??? */ 0x0437,  46, 0,									// CYRILLIC SMALL LETTER ZE
	/* ??? */ 0x0438,  24, 0,									// CYRILLIC SMALL LETTER I
	/* ??? */ 0x043A,  38, 0,									// CYRILLIC SMALL LETTER KA
	/* ??? */ 0x043B,  39, 0,									// CYRILLIC SMALL LETTER EL
	/* ??? */ 0x043C,  52, 0,									// CYRILLIC SMALL LETTER EM
	/* ??? */ 0x043D,  51, 0,									// CYRILLIC SMALL LETTER EN
	/* ??? */ 0x043E,  25, 0,									// CYRILLIC SMALL LETTER O
	/* ??? */ 0x043F,  26, 0,									// CYRILLIC SMALL LETTER PE
	/* ??? */ 0x0440,  20, 0,									// CYRILLIC SMALL LETTER ER
	/* ??? */ 0x0441,  32, 0,									// CYRILLIC SMALL LETTER ES
	/* ??? */ 0x0442,  21, 0,									// CYRILLIC SMALL LETTER TE
	/* ??? */ 0x0443,  23, 0,									// CYRILLIC SMALL LETTER U
	/* ??? */ 0x0444,  34, 0,									// CYRILLIC SMALL LETTER EF
	/* ??? */ 0x0445,  36, 0,									// CYRILLIC SMALL LETTER HA
	/* ??? */ 0x0446,  48, 0,									// CYRILLIC SMALL LETTER TSE
	/* ??? */ 0x0447,  40, 0,									// CYRILLIC SMALL LETTER CHE
	/* ??? */ 0x0448,  27, 0,									// CYRILLIC SMALL LETTER SHA
	/* ??? */ 0x0451,  45, 0,									// CYRILLIC SMALL LETTER IO
	/* ??? */ 0x0452,  28, MO_AltRight,						// CYRILLIC SMALL LETTER DJE
	/* ??? */ 0x0453,  28, 0,									// CYRILLIC SMALL LETTER GJE
	/* ??? */ 0x0455,  22, 0,									// CYRILLIC SMALL LETTER DZE
	/* ??? */ 0x0458,  37, 0,									// CYRILLIC SMALL LETTER JE
	/* ??? */ 0x0459,  17, 0,									// CYRILLIC SMALL LETTER LJE
	/* ??? */ 0x045A,  18, 0,									// CYRILLIC SMALL LETTER NJE
	/* ??? */ 0x045B,  41, MO_AltRight,						// CYRILLIC SMALL LETTER TSHE
	/* ??? */ 0x045C,  41, 0,									// CYRILLIC SMALL LETTER KJE
	/* ??? */ 0x045F,  47, 0,									// CYRILLIC SMALL LETTER DZHE
	/* '‘' */ 0x2018,   7, MO_ShiftLeft,
	/* '’' */ 0x2019,   5, MO_ShiftLeft,
	/* '“' */ 0x201C,   4, MO_ShiftLeft,
	/* '„' */ 0x201E,   3, MO_ShiftLeft,
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Albanian[43] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* ''' */ 0x0027,  28, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '@' */ 0x0040,  28, 0,
	/* 'Y' */ 0x0059,  46, MO_ShiftLeft,
	/* 'Z' */ 0x005A,  22, MO_ShiftLeft,
	/* '[' */ 0x005B,  34, MO_AltRight,
	/* '\' */ 0x005C,   1, 0,
	/* ']' */ 0x005D,  35, MO_AltRight,
	/* '^' */ 0x005E,   4, MO_AltRight|MO_DeadKey,
	/* '`' */ 0x0060,   8, MO_AltRight,
	/* 'y' */ 0x0079,  46, 0,
	/* 'z' */ 0x007A,  22, 0,
	/* '{' */ 0x007B,  41, MO_ShiftLeft,
	/* '|' */ 0x007C,   1, MO_ShiftLeft,
	/* '}' */ 0x007D,  42, MO_ShiftLeft,
	/* '~' */ 0x007E,   2, MO_AltRight,
	/* '¤' */ 0x00A4,  42, MO_AltRight,
	/* '§' */ 0x00A7,  52, MO_AltRight,
	/* '¨' */ 0x00A8,  12, MO_AltRight|MO_DeadKey,
	/* '°' */ 0x00B0,   6, MO_AltRight|MO_DeadKey,
	/* '´' */ 0x00B4,  10, MO_AltRight|MO_DeadKey,
	/* '¸' */ 0x00B8,  13, MO_AltRight|MO_DeadKey,
	/* 'Ç' */ 0x00C7,  27, MO_ShiftLeft,
	/* 'Ë' */ 0x00CB,  40, MO_ShiftLeft,
	/* '×' */ 0x00D7,  28, MO_AltRight,
	/* 'ß' */ 0x00DF,  41, MO_AltRight,
	/* 'ç' */ 0x00E7,  27, 0,
	/* 'ë' */ 0x00EB,  40, 0,
	/* '÷' */ 0x00F7,  27, MO_AltRight,
	/* ??? */ 0x0110,  33, MO_AltRight,						// LATIN CAPITAL LETTER D WITH STROKE
	/* ??? */ 0x0111,  32, MO_AltRight,						// LATIN SMALL LETTER D WITH STROKE
	/* ??? */ 0x0141,  39, MO_AltRight,						// LATIN CAPITAL LETTER L WITH STROKE
	/* ??? */ 0x0142,  38, MO_AltRight,						// LATIN SMALL LETTER L WITH STROKE
	/* ??? */ 0x02C7,   3, MO_AltRight|MO_DeadKey,		// CARON
	/* ??? */ 0x02D8,   5, MO_AltRight|MO_DeadKey,		// BREVE
	/* ??? */ 0x02D9,   9, MO_AltRight|MO_DeadKey,		// DOT ABOVE
	/* ??? */ 0x02DB,   7, MO_AltRight|MO_DeadKey,		// OGONEK
	/* ??? */ 0x02DD,  11, MO_AltRight|MO_DeadKey,		// DOUBLE ACUTE ACCENT
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Serbian_Latin[58] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,  12, 0,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  13, MO_ShiftLeft,
	/* '+' */ 0x002B,  13, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,  49, MO_AltRight,
	/* 'Y' */ 0x0059,  46, MO_ShiftLeft,
	/* 'Z' */ 0x005A,  22, MO_ShiftLeft,
	/* '[' */ 0x005B,  34, MO_AltRight,
	/* '\' */ 0x005C,  17, MO_AltRight,
	/* ']' */ 0x005D,  35, MO_AltRight,
	/* '^' */ 0x005E,   4, MO_AltRight|MO_DeadKey,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,   8, MO_AltRight,
	/* 'y' */ 0x0079,  46, 0,
	/* 'z' */ 0x007A,  22, 0,
	/* '{' */ 0x007B,  50, MO_AltRight,
	/* '|' */ 0x007C,  18, MO_AltRight,
	/* '}' */ 0x007D,  51, MO_AltRight,
	/* '¤' */ 0x00A4,  42, MO_AltRight,
	/* '§' */ 0x00A7,  52, MO_AltRight,
	/* '¨' */ 0x00A8,  12, MO_AltRight|MO_DeadKey,
	/* '°' */ 0x00B0,   6, MO_AltRight|MO_DeadKey,
	/* '´' */ 0x00B4,  10, MO_AltRight|MO_DeadKey,
	/* '¸' */ 0x00B8,  13, MO_AltRight|MO_DeadKey,
	/* '×' */ 0x00D7,  28, MO_AltRight,
	/* 'ß' */ 0x00DF,  41, MO_AltRight,
	/* '÷' */ 0x00F7,  27, MO_AltRight,
	/* ??? */ 0x0106,  41, MO_ShiftLeft,					// LATIN CAPITAL LETTER C WITH ACUTE
	/* ??? */ 0x0107,  41, 0,									// LATIN SMALL LETTER C WITH ACUTE
	/* ??? */ 0x010C,  40, MO_ShiftLeft,					// LATIN CAPITAL LETTER C WITH CARON
	/* ??? */ 0x010D,  40, 0,									// LATIN SMALL LETTER C WITH CARON
	/* ??? */ 0x0110,  28, MO_ShiftLeft,					// LATIN CAPITAL LETTER D WITH STROKE
	/* ??? */ 0x0111,  28, 0,									// LATIN SMALL LETTER D WITH STROKE
	/* ??? */ 0x0141,  39, MO_AltRight,						// LATIN CAPITAL LETTER L WITH STROKE
	/* ??? */ 0x0142,  38, MO_AltRight,						// LATIN SMALL LETTER L WITH STROKE
	/* 'Š' */ 0x0160,  27, MO_ShiftLeft,
	/* 'š' */ 0x0161,  27, 0,
	/* 'Ž' */ 0x017D,  42, MO_ShiftLeft,
	/* 'ž' */ 0x017E,  42, 0,
	/* ??? */ 0x02C7,   3, MO_AltRight|MO_DeadKey,		// CARON
	/* ??? */ 0x02D8,   5, MO_AltRight|MO_DeadKey,		// BREVE
	/* ??? */ 0x02D9,   9, MO_AltRight|MO_DeadKey,		// DOT ABOVE
	/* ??? */ 0x02DB,   7, MO_AltRight|MO_DeadKey,		// OGONEK
	/* ??? */ 0x02DD,  11, MO_AltRight|MO_DeadKey,		// DOUBLE ACUTE ACCENT
	/* '‚' */ 0x201A,   1, MO_DeadKey,
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Serbian_Cyrillic[140] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,  12, MO_DeadKey,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  13, MO_ShiftLeft,
	/* '+' */ 0x002B,  13, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040, NOS, 0,
	/* 'A' */ 0x0041, NOS, 0,
	/* 'B' */ 0x0042, NOS, 0,
	/* 'C' */ 0x0043, NOS, 0,
	/* 'D' */ 0x0044, NOS, 0,
	/* 'E' */ 0x0045, NOS, 0,
	/* 'F' */ 0x0046, NOS, 0,
	/* 'G' */ 0x0047, NOS, 0,
	/* 'H' */ 0x0048, NOS, 0,
	/* 'I' */ 0x0049, NOS, 0,
	/* 'J' */ 0x004A, NOS, 0,
	/* 'K' */ 0x004B, NOS, 0,
	/* 'L' */ 0x004C, NOS, 0,
	/* 'M' */ 0x004D, NOS, 0,
	/* 'N' */ 0x004E, NOS, 0,
	/* 'O' */ 0x004F, NOS, 0,
	/* 'P' */ 0x0050, NOS, 0,
	/* 'Q' */ 0x0051, NOS, 0,
	/* 'R' */ 0x0052, NOS, 0,
	/* 'S' */ 0x0053, NOS, 0,
	/* 'T' */ 0x0054, NOS, 0,
	/* 'U' */ 0x0055, NOS, 0,
	/* 'V' */ 0x0056, NOS, 0,
	/* 'W' */ 0x0057, NOS, 0,
	/* 'X' */ 0x0058, NOS, 0,
	/* 'Y' */ 0x0059, NOS, 0,
	/* 'Z' */ 0x005A, NOS, 0,
	/* '[' */ 0x005B, NOS, 0,
	/* '\' */ 0x005C, NOS, 0,
	/* ']' */ 0x005D, NOS, 0,
	/* '^' */ 0x005E, NOS, 0,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* 'a' */ 0x0061, NOS, 0,
	/* 'b' */ 0x0062, NOS, 0,
	/* 'c' */ 0x0063, NOS, 0,
	/* 'd' */ 0x0064, NOS, 0,
	/* 'e' */ 0x0065, NOS, 0,
	/* 'f' */ 0x0066, NOS, 0,
	/* 'g' */ 0x0067, NOS, 0,
	/* 'h' */ 0x0068, NOS, 0,
	/* 'i' */ 0x0069, NOS, 0,
	/* 'j' */ 0x006A, NOS, 0,
	/* 'k' */ 0x006B, NOS, 0,
	/* 'l' */ 0x006C, NOS, 0,
	/* 'm' */ 0x006D, NOS, 0,
	/* 'n' */ 0x006E, NOS, 0,
	/* 'o' */ 0x006F, NOS, 0,
	/* 'p' */ 0x0070, NOS, 0,
	/* 'q' */ 0x0071, NOS, 0,
	/* 'r' */ 0x0072, NOS, 0,
	/* 's' */ 0x0073, NOS, 0,
	/* 't' */ 0x0074, NOS, 0,
	/* 'u' */ 0x0075, NOS, 0,
	/* 'v' */ 0x0076, NOS, 0,
	/* 'w' */ 0x0077, NOS, 0,
	/* 'x' */ 0x0078, NOS, 0,
	/* 'y' */ 0x0079, NOS, 0,
	/* 'z' */ 0x007A, NOS, 0,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D, NOS, 0,
	/* ??? */ 0x0402,  28, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER DJE
	/* ??? */ 0x0405,  46, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER DZE
	/* ??? */ 0x0408,  37, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER JE
	/* ??? */ 0x0409,  17, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER LJE
	/* ??? */ 0x040A,  18, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER NJE
	/* ??? */ 0x040B,  41, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TSHE
	/* ??? */ 0x040F,  47, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER DZHE
	/* ??? */ 0x0410,  31, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER A
	/* ??? */ 0x0411,  50, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER BE
	/* ??? */ 0x0412,  49, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER VE
	/* ??? */ 0x0413,  35, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER GHE
	/* ??? */ 0x0414,  33, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER DE
	/* ??? */ 0x0415,  19, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER IE
	/* ??? */ 0x0416,  42, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZHE
	/* ??? */ 0x0417,  22, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZE
	/* ??? */ 0x0418,  24, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER I
	/* ??? */ 0x041A,  38, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER KA
	/* ??? */ 0x041B,  39, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EL
	/* ??? */ 0x041C,  52, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EM
	/* ??? */ 0x041D,  51, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EN
	/* ??? */ 0x041E,  25, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER O
	/* ??? */ 0x041F,  26, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER PE
	/* ??? */ 0x0420,  20, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ER
	/* ??? */ 0x0421,  32, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ES
	/* ??? */ 0x0422,  21, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TE
	/* ??? */ 0x0423,  23, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER U
	/* ??? */ 0x0424,  34, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EF
	/* ??? */ 0x0425,  36, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER HA
	/* ??? */ 0x0426,  48, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TSE
	/* ??? */ 0x0427,  40, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER CHE
	/* ??? */ 0x0428,  27, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHA
	/* ??? */ 0x0430,  31, 0,									// CYRILLIC SMALL LETTER A
	/* ??? */ 0x0431,  50, 0,									// CYRILLIC SMALL LETTER BE
	/* ??? */ 0x0432,  49, 0,									// CYRILLIC SMALL LETTER VE
	/* ??? */ 0x0433,  35, 0,									// CYRILLIC SMALL LETTER GHE
	/* ??? */ 0x0434,  33, 0,									// CYRILLIC SMALL LETTER DE
	/* ??? */ 0x0435,  19, 0,									// CYRILLIC SMALL LETTER IE
	/* ??? */ 0x0436,  42, 0,									// CYRILLIC SMALL LETTER ZHE
	/* ??? */ 0x0437,  22, 0,									// CYRILLIC SMALL LETTER ZE
	/* ??? */ 0x0438,  24, 0,									// CYRILLIC SMALL LETTER I
	/* ??? */ 0x043A,  38, 0,									// CYRILLIC SMALL LETTER KA
	/* ??? */ 0x043B,  39, 0,									// CYRILLIC SMALL LETTER EL
	/* ??? */ 0x043C,  52, 0,									// CYRILLIC SMALL LETTER EM
	/* ??? */ 0x043D,  51, 0,									// CYRILLIC SMALL LETTER EN
	/* ??? */ 0x043E,  25, 0,									// CYRILLIC SMALL LETTER O
	/* ??? */ 0x043F,  26, 0,									// CYRILLIC SMALL LETTER PE
	/* ??? */ 0x0440,  20, 0,									// CYRILLIC SMALL LETTER ER
	/* ??? */ 0x0441,  32, 0,									// CYRILLIC SMALL LETTER ES
	/* ??? */ 0x0442,  21, 0,									// CYRILLIC SMALL LETTER TE
	/* ??? */ 0x0443,  23, 0,									// CYRILLIC SMALL LETTER U
	/* ??? */ 0x0444,  34, 0,									// CYRILLIC SMALL LETTER EF
	/* ??? */ 0x0445,  36, 0,									// CYRILLIC SMALL LETTER HA
	/* ??? */ 0x0446,  48, 0,									// CYRILLIC SMALL LETTER TSE
	/* ??? */ 0x0447,  40, 0,									// CYRILLIC SMALL LETTER CHE
	/* ??? */ 0x0448,  27, 0,									// CYRILLIC SMALL LETTER SHA
	/* ??? */ 0x0452,  28, 0,									// CYRILLIC SMALL LETTER DJE
	/* ??? */ 0x0455,  46, 0,									// CYRILLIC SMALL LETTER DZE
	/* ??? */ 0x0458,  37, 0,									// CYRILLIC SMALL LETTER JE
	/* ??? */ 0x0459,  17, 0,									// CYRILLIC SMALL LETTER LJE
	/* ??? */ 0x045A,  18, 0,									// CYRILLIC SMALL LETTER NJE
	/* ??? */ 0x045B,  41, 0,									// CYRILLIC SMALL LETTER TSHE
	/* ??? */ 0x045F,  47, 0,									// CYRILLIC SMALL LETTER DZHE
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Czech_QWERTZ[74] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,  41, MO_ShiftLeft,
	/* '"' */ 0x0022,  40, MO_ShiftLeft,
	/* '#' */ 0x0023,  47, MO_AltRight,
	/* '$' */ 0x0024,  40, MO_AltRight,
	/* '%' */ 0x0025,  12, MO_ShiftLeft,
	/* '&' */ 0x0026,  48, MO_AltRight,
	/* ''' */ 0x0027,  42, MO_ShiftLeft,
	/* '(' */ 0x0028,  28, MO_ShiftLeft,
	/* ')' */ 0x0029,  28, 0,
	/* '*' */ 0x002A,  55, MO_AltRight,
	/* '+' */ 0x002B,   2, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,  27, MO_ShiftLeft,
	/* '0' */ 0x0030,  11, MO_ShiftLeft,
	/* '1' */ 0x0031,   2, MO_ShiftLeft,
	/* '2' */ 0x0032,   3, MO_ShiftLeft,
	/* '3' */ 0x0033,   4, MO_ShiftLeft,
	/* '4' */ 0x0034,   5, MO_ShiftLeft,
	/* '5' */ 0x0035,   6, MO_ShiftLeft,
	/* '6' */ 0x0036,   7, MO_ShiftLeft,
	/* '7' */ 0x0037,   8, MO_ShiftLeft,
	/* '8' */ 0x0038,   9, MO_ShiftLeft,
	/* '9' */ 0x0039,  10, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,   1, 0,
	/* '<' */ 0x003C,  53, MO_AltRight,
	/* '=' */ 0x003D,  12, 0,
	/* '>' */ 0x003E,  54, MO_AltRight,
	/* '?' */ 0x003F,  53, MO_ShiftLeft,
	/* '@' */ 0x0040,  49, MO_AltRight,
	/* 'Y' */ 0x0059,  46, MO_ShiftLeft,
	/* 'Z' */ 0x005A,  22, MO_ShiftLeft,
	/* '[' */ 0x005B,  34, MO_AltRight,
	/* '\' */ 0x005C,  17, MO_AltRight,
	/* ']' */ 0x005D,  35, MO_AltRight,
	/* '^' */ 0x005E,   4, MO_AltRight|MO_DeadKey,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,   8, MO_AltRight|MO_DeadKey,
	/* 'y' */ 0x0079,  46, 0,
	/* 'z' */ 0x007A,  22, 0,
	/* '{' */ 0x007B,  50, MO_AltRight,
	/* '|' */ 0x007C,  18, MO_AltRight,
	/* '}' */ 0x007D,  51, MO_AltRight,
	/* '~' */ 0x007E,   2, MO_AltRight,
	/* '¤' */ 0x00A4,  42, MO_AltRight,
	/* '§' */ 0x00A7,  41, 0,
	/* '¨' */ 0x00A8,  12, MO_AltRight|MO_DeadKey,
	/* '°' */ 0x00B0,   1, MO_ShiftLeft|MO_DeadKey,
	/* '´' */ 0x00B4,  10, MO_AltRight|MO_DeadKey,
	/* '¸' */ 0x00B8,  13, MO_AltRight|MO_DeadKey,
	/* '×' */ 0x00D7,  28, MO_AltRight,
	/* 'ß' */ 0x00DF,  41, MO_AltRight,
	/* 'á' */ 0x00E1,   9, 0,
	/* 'é' */ 0x00E9,  11, 0,
	/* 'í' */ 0x00ED,  10, 0,
	/* '÷' */ 0x00F7,  27, MO_AltRight,
	/* 'ú' */ 0x00FA,  27, 0,
	/* 'ý' */ 0x00FD,   8, 0,
	/* ??? */ 0x010D,   5, 0,									// LATIN SMALL LETTER C WITH CARON
	/* ??? */ 0x0110,  33, MO_AltRight,						// LATIN CAPITAL LETTER D WITH STROKE
	/* ??? */ 0x0111,  32, MO_AltRight,						// LATIN SMALL LETTER D WITH STROKE
	/* ??? */ 0x011B,   3, 0,									// LATIN SMALL LETTER E WITH CARON
	/* ??? */ 0x0141,  39, MO_AltRight,						// LATIN CAPITAL LETTER L WITH STROKE
	/* ??? */ 0x0142,  38, MO_AltRight,						// LATIN SMALL LETTER L WITH STROKE
	/* ??? */ 0x0159,   6, 0,									// LATIN SMALL LETTER R WITH CARON
	/* 'š' */ 0x0161,   4, 0,
	/* ??? */ 0x016F,  40, 0,									// LATIN SMALL LETTER U WITH RING ABOVE
	/* 'ž' */ 0x017E,   7, 0,
	/* ??? */ 0x02C7,   3, MO_AltRight|MO_DeadKey,		// CARON
	/* ??? */ 0x02D8,   5, MO_AltRight|MO_DeadKey,		// BREVE
	/* ??? */ 0x02DB,   7, MO_AltRight|MO_DeadKey,		// OGONEK
	/* ??? */ 0x02DD,  11, MO_AltRight|MO_DeadKey,		// DOUBLE ACUTE ACCENT
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Czech_QWERTY[63] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   2, MO_AltRight,
	/* '"' */ 0x0022,  40, MO_ShiftLeft,
	/* '#' */ 0x0023,   4, MO_AltRight,
	/* '$' */ 0x0024,   5, MO_AltRight,
	/* '%' */ 0x0025,   6, MO_AltRight,
	/* '&' */ 0x0026,   8, MO_AltRight,
	/* ''' */ 0x0027,  42, MO_ShiftLeft,
	/* '(' */ 0x0028,  10, MO_AltRight,
	/* ')' */ 0x0029,  11, MO_AltRight,
	/* '*' */ 0x002A,   9, MO_AltRight,
	/* '+' */ 0x002B,   2, 0,
	/* '-' */ 0x002D,  12, MO_AltRight,
	/* '/' */ 0x002F,  27, MO_ShiftLeft,
	/* '0' */ 0x0030,  11, MO_ShiftLeft,
	/* '1' */ 0x0031,   2, MO_ShiftLeft,
	/* '2' */ 0x0032,   3, MO_ShiftLeft,
	/* '3' */ 0x0033,   4, MO_ShiftLeft,
	/* '4' */ 0x0034,   5, MO_ShiftLeft,
	/* '5' */ 0x0035,   6, MO_ShiftLeft,
	/* '6' */ 0x0036,   7, MO_ShiftLeft,
	/* '7' */ 0x0037,   8, MO_ShiftLeft,
	/* '8' */ 0x0038,   9, MO_ShiftLeft,
	/* '9' */ 0x0039,  10, MO_ShiftLeft,
	/* ':' */ 0x003A,  40, MO_ShiftLeft|MO_AltRight,
	/* ';' */ 0x003B,   1, 0,
	/* '<' */ 0x003C,  53, MO_AltRight,
	/* '=' */ 0x003D,  12, 0,
	/* '>' */ 0x003E,  54, MO_AltRight,
	/* '?' */ 0x003F,  53, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_AltRight,
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,  42, MO_AltRight,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '^' */ 0x005E,   7, MO_AltRight,
	/* '_' */ 0x005F,  12, MO_ShiftLeft|MO_AltRight,
	/* '`' */ 0x0060,   1, MO_AltRight,
	/* '{' */ 0x007B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x007C,  42, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x007D,  28, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x007E,   1, MO_ShiftLeft|MO_AltRight,
	/* '¤' */ 0x00A4,  41, MO_AltRight,
	/* '§' */ 0x00A7,  41, 0,
	/* '¨' */ 0x00A8,  42, MO_DeadKey,
	/* '°' */ 0x00B0,   1, MO_ShiftLeft|MO_DeadKey,
	/* '´' */ 0x00B4,  13, MO_DeadKey,
	/* '×' */ 0x00D7,  53, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0x00DF,  45, MO_AltRight,
	/* 'á' */ 0x00E1,   9, 0,
	/* 'é' */ 0x00E9,  11, 0,
	/* 'í' */ 0x00ED,  10, 0,
	/* '÷' */ 0x00F7,  54, MO_ShiftLeft|MO_AltRight,
	/* 'ú' */ 0x00FA,  27, 0,
	/* 'ý' */ 0x00FD,   8, 0,
	/* ??? */ 0x010D,   5, 0,									// LATIN SMALL LETTER C WITH CARON
	/* ??? */ 0x011B,   3, 0,									// LATIN SMALL LETTER E WITH CARON
	/* ??? */ 0x0159,   6, 0,									// LATIN SMALL LETTER R WITH CARON
	/* 'š' */ 0x0161,   4, 0,
	/* ??? */ 0x016F,  40, 0,									// LATIN SMALL LETTER U WITH RING ABOVE
	/* 'ž' */ 0x017E,   7, 0,
	/* ??? */ 0x02C7,  13, MO_ShiftLeft|MO_DeadKey,		// CARON
	/* ??? */ 0x02DD,  45, MO_ShiftLeft|MO_AltRight|MO_DeadKey,// DOUBLE ACUTE ACCENT
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Czech_Programmers[30] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,  40, MO_ShiftLeft|MO_AltRight,
	/* '+' */ 0x002B,   2, MO_AltRight,
	/* '/' */ 0x002F,  27, MO_ShiftLeft|MO_AltRight,
	/* ';' */ 0x003B,   1, MO_AltRight,
	/* '=' */ 0x003D,  12, MO_AltRight,
	/* '?' */ 0x003F,  53, MO_AltRight,
	/* '\' */ 0x005C,  42, 0,
	/* '|' */ 0x007C,  42, MO_ShiftLeft,
	/* '§' */ 0x00A7,  41, MO_AltRight,
	/* '¨' */ 0x00A8,  42, MO_AltRight|MO_DeadKey,
	/* '°' */ 0x00B0,   1, MO_ShiftLeft|MO_AltRight|MO_DeadKey,
	/* '´' */ 0x00B4,  13, MO_AltRight|MO_DeadKey,
	/* '×' */ 0x00D7,  53, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0x00DF,  45, MO_AltRight,
	/* 'á' */ 0x00E1,   9, MO_AltRight,
	/* 'é' */ 0x00E9,  11, MO_AltRight,
	/* 'í' */ 0x00ED,  10, MO_AltRight,
	/* '÷' */ 0x00F7,  54, MO_ShiftLeft|MO_AltRight,
	/* 'ú' */ 0x00FA,  27, MO_AltRight,
	/* 'ý' */ 0x00FD,   8, MO_AltRight,
	/* ??? */ 0x010D,   5, MO_AltRight,						// LATIN SMALL LETTER C WITH CARON
	/* ??? */ 0x011B,   3, MO_AltRight,						// LATIN SMALL LETTER E WITH CARON
	/* ??? */ 0x0159,   6, MO_AltRight,						// LATIN SMALL LETTER R WITH CARON
	/* 'š' */ 0x0161,   4, MO_AltRight,
	/* ??? */ 0x016F,  40, MO_AltRight,						// LATIN SMALL LETTER U WITH RING ABOVE
	/* 'ž' */ 0x017E,   7, MO_AltRight,
	/* ??? */ 0x02C7,  13, MO_ShiftLeft|MO_AltRight|MO_DeadKey,// CARON
	/* ??? */ 0x02DD,  45, MO_ShiftLeft|MO_AltRight|MO_DeadKey,// DOUBLE ACUTE ACCENT
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Estonian[47] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '$' */ 0x0024,   5, MO_AltRight,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,  42, 0,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  42, MO_ShiftLeft,
	/* '+' */ 0x002B,  12, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_AltRight,
	/* '[' */ 0x005B,   9, MO_AltRight,
	/* '\' */ 0x005C,  12, MO_AltRight,
	/* ']' */ 0x005D,  10, MO_AltRight,
	/* '^' */ 0x005E,  41, MO_AltRight|MO_DeadKey,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,  13, MO_ShiftLeft|MO_DeadKey,
	/* '{' */ 0x007B,   8, MO_AltRight,
	/* '|' */ 0x007C,  45, MO_AltRight,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* '~' */ 0x007E,   1, MO_ShiftLeft|MO_DeadKey,
	/* '£' */ 0x00A3,   4, MO_AltRight,
	/* '¤' */ 0x00A4,   5, MO_ShiftLeft,
	/* '§' */ 0x00A7,  28, MO_AltRight,
	/* '´' */ 0x00B4,  13, MO_DeadKey,
	/* '½' */ 0x00BD,  42, MO_AltRight,
	/* 'Ä' */ 0x00C4,  41, MO_ShiftLeft,
	/* 'Õ' */ 0x00D5,  28, MO_ShiftLeft,
	/* 'Ö' */ 0x00D6,  40, MO_ShiftLeft,
	/* 'Ü' */ 0x00DC,  27, MO_ShiftLeft,
	/* 'ä' */ 0x00E4,  41, 0,
	/* 'õ' */ 0x00F5,  28, 0,
	/* 'ö' */ 0x00F6,  40, 0,
	/* 'ü' */ 0x00FC,  27, 0,
	/* 'Š' */ 0x0160,  32, MO_ShiftLeft|MO_AltRight,
	/* 'š' */ 0x0161,  32, MO_AltRight,
	/* 'Ž' */ 0x017D,  46, MO_ShiftLeft|MO_AltRight,
	/* 'ž' */ 0x017E,  46, MO_AltRight,
	/* ??? */ 0x02C7,   1, MO_DeadKey,						// CARON
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Latvian[111] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   6, MO_AltRight,
	/* '#' */ 0x0023,   4, MO_ShiftLeft|MO_AltRight,
	/* ''' */ 0x0027, NOS, 0,
	/* '*' */ 0x002A, NOS, 0,
	/* '+' */ 0x002B, NOS, 0,
	/* '/' */ 0x002F,   7, MO_ShiftLeft,
	/* ':' */ 0x003A,   9, MO_AltRight,
	/* ';' */ 0x003B,  13, MO_ShiftLeft|MO_AltRight,
	/* '<' */ 0x003C,  53, MO_AltRight,
	/* '=' */ 0x003D,  13, MO_AltRight,
	/* '>' */ 0x003E,  54, MO_AltRight,
	/* '?' */ 0x003F,   1, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_ShiftLeft|MO_AltRight,
	/* 'A' */ 0x0041,  37, MO_ShiftLeft,
	/* 'B' */ 0x0042,  47, MO_ShiftLeft,
	/* 'C' */ 0x0043,  40, MO_ShiftLeft,
	/* 'D' */ 0x0044,  36, MO_ShiftLeft,
	/* 'E' */ 0x0045,  39, MO_ShiftLeft,
	/* 'F' */ 0x0046,  13, MO_ShiftLeft,
	/* 'G' */ 0x0047,  18, MO_ShiftLeft,
	/* 'H' */ 0x0048,  28, MO_ShiftLeft,
	/* 'I' */ 0x0049,  34, MO_ShiftLeft,
	/* 'J' */ 0x004A,  19, MO_ShiftLeft,
	/* 'K' */ 0x004B,  49, MO_ShiftLeft,
	/* 'L' */ 0x004C,  35, MO_ShiftLeft,
	/* 'M' */ 0x004D,  21, MO_ShiftLeft,
	/* 'N' */ 0x004E,  23, MO_ShiftLeft,
	/* 'O' */ 0x004F,  51, MO_ShiftLeft,
	/* 'P' */ 0x0050,  50, MO_ShiftLeft,
	/* 'Q' */ 0x0051,  17, MO_ShiftLeft|MO_AltRight,
	/* 'S' */ 0x0053,  33, MO_ShiftLeft,
	/* 'T' */ 0x0054,  38, MO_ShiftLeft,
	/* 'U' */ 0x0055,  32, MO_ShiftLeft,
	/* 'V' */ 0x0056,  22, MO_ShiftLeft,
	/* 'W' */ 0x0057,  21, MO_ShiftLeft|MO_AltRight,
	/* 'X' */ 0x0058,  47, MO_ShiftLeft|MO_AltRight,
	/* 'Y' */ 0x0059,  22, MO_ShiftLeft|MO_AltRight,
	/* 'Z' */ 0x005A,  24, MO_ShiftLeft,
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,  45, MO_AltRight,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '^' */ 0x005E,   7, MO_ShiftLeft|MO_AltRight,
	/* '`' */ 0x0060, NOS, 0,
	/* 'a' */ 0x0061,  37, 0,
	/* 'b' */ 0x0062,  47, 0,
	/* 'c' */ 0x0063,  40, 0,
	/* 'd' */ 0x0064,  36, 0,
	/* 'e' */ 0x0065,  39, 0,
	/* 'f' */ 0x0066,  13, 0,
	/* 'g' */ 0x0067,  18, 0,
	/* 'h' */ 0x0068,  28, 0,
	/* 'i' */ 0x0069,  34, 0,
	/* 'j' */ 0x006A,  19, 0,
	/* 'k' */ 0x006B,  49, 0,
	/* 'l' */ 0x006C,  35, 0,
	/* 'm' */ 0x006D,  21, 0,
	/* 'n' */ 0x006E,  23, 0,
	/* 'o' */ 0x006F,  51, 0,
	/* 'p' */ 0x0070,  50, 0,
	/* 'q' */ 0x0071,  17, MO_AltRight,
	/* 's' */ 0x0073,  33, 0,
	/* 't' */ 0x0074,  38, 0,
	/* 'u' */ 0x0075,  32, 0,
	/* 'v' */ 0x0076,  22, 0,
	/* 'w' */ 0x0077,  21, MO_AltRight,
	/* 'x' */ 0x0078,  47, MO_AltRight,
	/* 'y' */ 0x0079,  22, MO_AltRight,
	/* 'z' */ 0x007A,  24, 0,
	/* '{' */ 0x007B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x007C,  45, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x007D,  28, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x007E,   6, MO_ShiftLeft|MO_AltRight|MO_DeadKey,
	/* '¨' */ 0x00A8,  41, MO_ShiftLeft|MO_AltRight|MO_DeadKey,
	/* '«' */ 0x00AB,   2, MO_AltRight,
	/* '­' */ 0x00AD,   1, 0,
	/* '°' */ 0x00B0,  41, MO_ShiftLeft|MO_DeadKey,
	/* '±' */ 0x00B1,   8, MO_ShiftLeft|MO_AltRight,
	/* '´' */ 0x00B4,  41, MO_DeadKey,
	/* '»' */ 0x00BB,   4, MO_ShiftLeft,
	/* 'Õ' */ 0x00D5,  51, MO_ShiftLeft|MO_AltRight,
	/* '×' */ 0x00D7,   9, MO_ShiftLeft,
	/* 'õ' */ 0x00F5,  51, MO_AltRight,
	/* ??? */ 0x0100,  52, MO_ShiftLeft,					// LATIN CAPITAL LETTER A WITH MACRON
	/* ??? */ 0x0101,  52, 0,									// LATIN SMALL LETTER A WITH MACRON
	/* ??? */ 0x010C,  26, MO_ShiftLeft,					// LATIN CAPITAL LETTER C WITH CARON
	/* ??? */ 0x010D,  26, 0,									// LATIN SMALL LETTER C WITH CARON
	/* ??? */ 0x0112,  25, MO_ShiftLeft,					// LATIN CAPITAL LETTER E WITH MACRON
	/* ??? */ 0x0113,  25, 0,									// LATIN SMALL LETTER E WITH MACRON
	/* ??? */ 0x0122,  18, MO_ShiftLeft|MO_AltRight,	// LATIN CAPITAL LETTER G WITH CEDILLA
	/* ??? */ 0x0123,  18, MO_AltRight,						// LATIN SMALL LETTER G WITH CEDILLA
	/* ??? */ 0x012A,  48, MO_ShiftLeft,					// LATIN CAPITAL LETTER I WITH MACRON
	/* ??? */ 0x012B,  48, 0,									// LATIN SMALL LETTER I WITH MACRON
	/* ??? */ 0x0136,  42, MO_ShiftLeft,					// LATIN CAPITAL LETTER K WITH CEDILLA
	/* ??? */ 0x0137,  42, 0,									// LATIN SMALL LETTER K WITH CEDILLA
	/* ??? */ 0x013B,  55, MO_ShiftLeft,					// LATIN CAPITAL LETTER L WITH CEDILLA
	/* ??? */ 0x013C,  55, 0,									// LATIN SMALL LETTER L WITH CEDILLA
	/* ??? */ 0x0145,  46, MO_ShiftLeft,					// LATIN CAPITAL LETTER N WITH CEDILLA
	/* ??? */ 0x0146,  46, 0,									// LATIN SMALL LETTER N WITH CEDILLA
	/* ??? */ 0x0156,  20, MO_ShiftLeft|MO_AltRight,	// LATIN CAPITAL LETTER R WITH CEDILLA
	/* ??? */ 0x0157,  20, MO_AltRight,						// LATIN SMALL LETTER R WITH CEDILLA
	/* 'Š' */ 0x0160,  31, MO_ShiftLeft,
	/* 'š' */ 0x0161,  31, 0,
	/* ??? */ 0x016A,  17, MO_ShiftLeft,					// LATIN CAPITAL LETTER U WITH MACRON
	/* ??? */ 0x016B,  17, 0,									// LATIN SMALL LETTER U WITH MACRON
	/* 'Ž' */ 0x017D,  27, MO_ShiftLeft,
	/* 'ž' */ 0x017E,  27, 0,
	/* '–' */ 0x2013,  12, MO_AltRight,
	/* '—' */ 0x2014,  12, MO_ShiftLeft|MO_AltRight,
	/* '’' */ 0x2019,   7, MO_AltRight,
	/* '€' */ 0x20AC,   5, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Latvian_QWERTY[44] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '\' */ 0x005C,  45, 0,
	/* '|' */ 0x007C,  42, MO_ShiftLeft,
	/* '~' */ 0x007E,   1, MO_ShiftLeft|MO_DeadKey,
	/* ' ' */ 0x00A0,   2, MO_AltRight,
	/* '§' */ 0x00A7,   5, MO_ShiftLeft|MO_AltRight,
	/* '¨' */ 0x00A8,  41, MO_ShiftLeft|MO_AltRight|MO_DeadKey,
	/* '«' */ 0x00AB,   3, MO_AltRight,
	/* '­' */ 0x00AD,   1, MO_AltRight,
	/* '°' */ 0x00B0,   6, MO_ShiftLeft|MO_AltRight,
	/* '±' */ 0x00B1,   8, MO_ShiftLeft|MO_AltRight,
	/* '´' */ 0x00B4,  41, MO_AltRight|MO_DeadKey,
	/* '»' */ 0x00BB,   4, MO_AltRight,
	/* 'Õ' */ 0x00D5,  25, MO_ShiftLeft|MO_AltRight,
	/* '×' */ 0x00D7,   9, MO_ShiftLeft|MO_AltRight,
	/* 'õ' */ 0x00F5,  25, MO_AltRight,
	/* ??? */ 0x0100,  31, MO_ShiftLeft|MO_AltRight,	// LATIN CAPITAL LETTER A WITH MACRON
	/* ??? */ 0x0101,  31, MO_AltRight,						// LATIN SMALL LETTER A WITH MACRON
	/* ??? */ 0x010C,  48, MO_ShiftLeft|MO_AltRight,	// LATIN CAPITAL LETTER C WITH CARON
	/* ??? */ 0x010D,  48, MO_AltRight,						// LATIN SMALL LETTER C WITH CARON
	/* ??? */ 0x0112,  19, MO_ShiftLeft|MO_AltRight,	// LATIN CAPITAL LETTER E WITH MACRON
	/* ??? */ 0x0113,  19, MO_AltRight,						// LATIN SMALL LETTER E WITH MACRON
	/* ??? */ 0x0122,  35, MO_ShiftLeft|MO_AltRight,	// LATIN CAPITAL LETTER G WITH CEDILLA
	/* ??? */ 0x0123,  35, MO_AltRight,						// LATIN SMALL LETTER G WITH CEDILLA
	/* ??? */ 0x012A,  24, MO_ShiftLeft|MO_AltRight,	// LATIN CAPITAL LETTER I WITH MACRON
	/* ??? */ 0x012B,  24, MO_AltRight,						// LATIN SMALL LETTER I WITH MACRON
	/* ??? */ 0x0136,  38, MO_ShiftLeft|MO_AltRight,	// LATIN CAPITAL LETTER K WITH CEDILLA
	/* ??? */ 0x0137,  38, MO_AltRight,						// LATIN SMALL LETTER K WITH CEDILLA
	/* ??? */ 0x013B,  39, MO_ShiftLeft|MO_AltRight,	// LATIN CAPITAL LETTER L WITH CEDILLA
	/* ??? */ 0x013C,  39, MO_AltRight,						// LATIN SMALL LETTER L WITH CEDILLA
	/* ??? */ 0x0145,  51, MO_ShiftLeft|MO_AltRight,	// LATIN CAPITAL LETTER N WITH CEDILLA
	/* ??? */ 0x0146,  51, MO_AltRight,						// LATIN SMALL LETTER N WITH CEDILLA
	/* ??? */ 0x0156,  20, MO_ShiftLeft|MO_AltRight,	// LATIN CAPITAL LETTER R WITH CEDILLA
	/* ??? */ 0x0157,  20, MO_AltRight,						// LATIN SMALL LETTER R WITH CEDILLA
	/* 'Š' */ 0x0160,  32, MO_ShiftLeft|MO_AltRight,
	/* 'š' */ 0x0161,  32, MO_AltRight,
	/* ??? */ 0x016A,  23, MO_ShiftLeft|MO_AltRight,	// LATIN CAPITAL LETTER U WITH MACRON
	/* ??? */ 0x016B,  23, MO_AltRight,						// LATIN SMALL LETTER U WITH MACRON
	/* 'Ž' */ 0x017D,  46, MO_ShiftLeft|MO_AltRight,
	/* 'ž' */ 0x017E,  46, MO_AltRight,
	/* '–' */ 0x2013,  12, MO_AltRight,
	/* '—' */ 0x2014,  12, MO_ShiftLeft|MO_AltRight,
	/* '’' */ 0x2019,   7, MO_AltRight,
	/* '€' */ 0x20AC,   5, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Lithuania[40] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   2, MO_ShiftLeft|MO_AltRight,
	/* '#' */ 0x0023,   4, MO_ShiftLeft|MO_AltRight,
	/* '$' */ 0x0024,   5, MO_ShiftLeft|MO_AltRight,
	/* '%' */ 0x0025,   6, MO_ShiftLeft|MO_AltRight,
	/* '&' */ 0x0026,   8, MO_ShiftLeft|MO_AltRight,
	/* '*' */ 0x002A,   9, MO_ShiftLeft|MO_AltRight,
	/* '+' */ 0x002B,  13, MO_ShiftLeft|MO_AltRight,
	/* '1' */ 0x0031,   2, MO_AltRight,
	/* '2' */ 0x0032,   3, MO_AltRight,
	/* '3' */ 0x0033,   4, MO_AltRight,
	/* '4' */ 0x0034,   5, MO_AltRight,
	/* '5' */ 0x0035,   6, MO_AltRight,
	/* '6' */ 0x0036,   7, MO_AltRight,
	/* '7' */ 0x0037,   8, MO_AltRight,
	/* '8' */ 0x0038,   9, MO_AltRight,
	/* '=' */ 0x003D,  13, MO_AltRight,
	/* '@' */ 0x0040,   3, MO_ShiftLeft|MO_AltRight,
	/* '\' */ 0x005C,  42, 0,
	/* '^' */ 0x005E,   7, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x007C,  42, MO_ShiftLeft,
	/* ??? */ 0x0104,   2, MO_ShiftLeft,					// LATIN CAPITAL LETTER A WITH OGONEK
	/* ??? */ 0x0105,   2, 0,									// LATIN SMALL LETTER A WITH OGONEK
	/* ??? */ 0x010C,   3, MO_ShiftLeft,					// LATIN CAPITAL LETTER C WITH CARON
	/* ??? */ 0x010D,   3, 0,									// LATIN SMALL LETTER C WITH CARON
	/* ??? */ 0x0116,   5, MO_ShiftLeft,					// LATIN CAPITAL LETTER E WITH DOT ABOVE
	/* ??? */ 0x0117,   5, 0,									// LATIN SMALL LETTER E WITH DOT ABOVE
	/* ??? */ 0x0118,   4, MO_ShiftLeft,					// LATIN CAPITAL LETTER E WITH OGONEK
	/* ??? */ 0x0119,   4, 0,									// LATIN SMALL LETTER E WITH OGONEK
	/* ??? */ 0x012E,   6, MO_ShiftLeft,					// LATIN CAPITAL LETTER I WITH OGONEK
	/* ??? */ 0x012F,   6, 0,									// LATIN SMALL LETTER I WITH OGONEK
	/* 'Š' */ 0x0160,   7, MO_ShiftLeft,
	/* 'š' */ 0x0161,   7, 0,
	/* ??? */ 0x016A,   9, MO_ShiftLeft,					// LATIN CAPITAL LETTER U WITH MACRON
	/* ??? */ 0x016B,   9, 0,									// LATIN SMALL LETTER U WITH MACRON
	/* ??? */ 0x0172,   8, MO_ShiftLeft,					// LATIN CAPITAL LETTER U WITH OGONEK
	/* ??? */ 0x0173,   8, 0,									// LATIN SMALL LETTER U WITH OGONEK
	/* 'Ž' */ 0x017D,  13, MO_ShiftLeft,
	/* 'ž' */ 0x017E,  13, 0,
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Lithuanian_IBM[68] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   2, 0,
	/* '"' */ 0x0022,   3, 0,
	/* '#' */ 0x0023, NOS, 0,
	/* '$' */ 0x0024, NOS, 0,
	/* '%' */ 0x0025, NOS, 0,
	/* '&' */ 0x0026, NOS, 0,
	/* ''' */ 0x0027, NOS, 0,
	/* '(' */ 0x0028,  10, 0,
	/* ')' */ 0x0029,  11, 0,
	/* '*' */ 0x002A, NOS, 0,
	/* '+' */ 0x002B,  13, 0,
	/* ',' */ 0x002C,   7, 0,
	/* '-' */ 0x002D,  12, MO_ShiftLeft,
	/* '.' */ 0x002E,   8, 0,
	/* '/' */ 0x002F,   4, 0,
	/* '0' */ 0x0030,  11, MO_ShiftLeft,
	/* '1' */ 0x0031,   2, MO_ShiftLeft,
	/* '2' */ 0x0032,   3, MO_ShiftLeft,
	/* '3' */ 0x0033,   4, MO_ShiftLeft,
	/* '4' */ 0x0034,   5, MO_ShiftLeft,
	/* '5' */ 0x0035,   6, MO_ShiftLeft,
	/* '6' */ 0x0036,   7, MO_ShiftLeft,
	/* '7' */ 0x0037,   8, MO_ShiftLeft,
	/* '8' */ 0x0038,   9, MO_ShiftLeft,
	/* '9' */ 0x0039,  10, MO_ShiftLeft,
	/* ':' */ 0x003A,   6, 0,
	/* ';' */ 0x003B,   5, 0,
	/* '<' */ 0x003C, NOS, 0,
	/* '=' */ 0x003D,  13, MO_ShiftLeft,
	/* '>' */ 0x003E, NOS, 0,
	/* '?' */ 0x003F,   9, 0,
	/* '@' */ 0x0040, NOS, 0,
	/* 'Q' */ 0x0051, NOS, 0,
	/* 'W' */ 0x0057, NOS, 0,
	/* 'X' */ 0x0058, NOS, 0,
	/* '[' */ 0x005B,   9, MO_AltRight,
	/* '\' */ 0x005C,  42, MO_ShiftLeft,
	/* ']' */ 0x005D,  10, MO_AltRight,
	/* '^' */ 0x005E, NOS, 0,
	/* '_' */ 0x005F,  12, 0,
	/* 'q' */ 0x0071, NOS, 0,
	/* 'w' */ 0x0077, NOS, 0,
	/* 'x' */ 0x0078, NOS, 0,
	/* '{' */ 0x007B,   8, MO_AltRight,
	/* '|' */ 0x007C,  42, 0,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* ??? */ 0x0104,  17, MO_ShiftLeft,					// LATIN CAPITAL LETTER A WITH OGONEK
	/* ??? */ 0x0105,  17, 0,									// LATIN SMALL LETTER A WITH OGONEK
	/* ??? */ 0x010C,  53, MO_ShiftLeft,					// LATIN CAPITAL LETTER C WITH CARON
	/* ??? */ 0x010D,  53, 0,									// LATIN SMALL LETTER C WITH CARON
	/* ??? */ 0x0116,  41, MO_ShiftLeft,					// LATIN CAPITAL LETTER E WITH DOT ABOVE
	/* ??? */ 0x0117,  41, 0,									// LATIN SMALL LETTER E WITH DOT ABOVE
	/* ??? */ 0x0118,  55, MO_ShiftLeft,					// LATIN CAPITAL LETTER E WITH OGONEK
	/* ??? */ 0x0119,  55, 0,									// LATIN SMALL LETTER E WITH OGONEK
	/* ??? */ 0x012E,  27, MO_ShiftLeft,					// LATIN CAPITAL LETTER I WITH OGONEK
	/* ??? */ 0x012F,  27, 0,									// LATIN SMALL LETTER I WITH OGONEK
	/* 'Š' */ 0x0160,  54, MO_ShiftLeft,
	/* 'š' */ 0x0161,  54, 0,
	/* ??? */ 0x016A,  47, MO_ShiftLeft,					// LATIN CAPITAL LETTER U WITH MACRON
	/* ??? */ 0x016B,  47, 0,									// LATIN SMALL LETTER U WITH MACRON
	/* ??? */ 0x0172,  40, MO_ShiftLeft,					// LATIN CAPITAL LETTER U WITH OGONEK
	/* ??? */ 0x0173,  40, 0,									// LATIN SMALL LETTER U WITH OGONEK
	/* 'Ž' */ 0x017D,  18, MO_ShiftLeft,
	/* 'ž' */ 0x017E,  18, 0,
	/* '“' */ 0x201C,  28, 0,
	/* '”' */ 0x201D,  28, MO_ShiftLeft,
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Slovakian_QWERTZ[76] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,  41, MO_ShiftLeft,
	/* '"' */ 0x0022,  40, MO_ShiftLeft,
	/* '#' */ 0x0023,  47, MO_AltRight,
	/* '$' */ 0x0024,  40, MO_AltRight,
	/* '%' */ 0x0025,  12, MO_ShiftLeft,
	/* '&' */ 0x0026,  45, 0,
	/* ''' */ 0x0027,  26, MO_AltRight,
	/* '(' */ 0x0028,  28, MO_ShiftLeft,
	/* ')' */ 0x0029,  42, MO_ShiftLeft,
	/* '*' */ 0x002A,  45, MO_ShiftLeft,
	/* '+' */ 0x002B,   2, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,  27, MO_ShiftLeft,
	/* '0' */ 0x0030,  11, MO_ShiftLeft,
	/* '1' */ 0x0031,   2, MO_ShiftLeft,
	/* '2' */ 0x0032,   3, MO_ShiftLeft,
	/* '3' */ 0x0033,   4, MO_ShiftLeft,
	/* '4' */ 0x0034,   5, MO_ShiftLeft,
	/* '5' */ 0x0035,   6, MO_ShiftLeft,
	/* '6' */ 0x0036,   7, MO_ShiftLeft,
	/* '7' */ 0x0037,   8, MO_ShiftLeft,
	/* '8' */ 0x0038,   9, MO_ShiftLeft,
	/* '9' */ 0x0039,  10, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,   1, 0,
	/* '<' */ 0x003C,  45, MO_AltRight,
	/* '=' */ 0x003D,  12, 0,
	/* '>' */ 0x003E,  46, MO_AltRight,
	/* '?' */ 0x003F,  53, MO_ShiftLeft,
	/* '@' */ 0x0040,  49, MO_AltRight,
	/* 'Y' */ 0x0059,  46, MO_ShiftLeft,
	/* 'Z' */ 0x005A,  22, MO_ShiftLeft,
	/* '[' */ 0x005B,  34, MO_AltRight,
	/* '\' */ 0x005C,  17, MO_AltRight,
	/* ']' */ 0x005D,  35, MO_AltRight,
	/* '^' */ 0x005E,   4, MO_AltRight|MO_DeadKey,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,   8, MO_AltRight,
	/* 'y' */ 0x0079,  46, 0,
	/* 'z' */ 0x007A,  22, 0,
	/* '{' */ 0x007B,  50, MO_AltRight,
	/* '|' */ 0x007C,  18, MO_AltRight,
	/* '}' */ 0x007D,  51, MO_AltRight,
	/* '~' */ 0x007E,   2, MO_AltRight,
	/* '¤' */ 0x00A4,  42, MO_AltRight,
	/* '§' */ 0x00A7,  41, 0,
	/* '¨' */ 0x00A8,  12, MO_AltRight|MO_DeadKey,
	/* '°' */ 0x00B0,   1, MO_ShiftLeft|MO_DeadKey,
	/* '´' */ 0x00B4,  10, MO_AltRight|MO_DeadKey,
	/* '¸' */ 0x00B8,  13, MO_AltRight|MO_DeadKey,
	/* '×' */ 0x00D7,  28, MO_AltRight,
	/* 'ß' */ 0x00DF,  41, MO_AltRight,
	/* 'á' */ 0x00E1,   9, 0,
	/* 'ä' */ 0x00E4,  28, 0,
	/* 'é' */ 0x00E9,  11, 0,
	/* 'í' */ 0x00ED,  10, 0,
	/* 'ô' */ 0x00F4,  40, 0,
	/* '÷' */ 0x00F7,  27, MO_AltRight,
	/* 'ú' */ 0x00FA,  27, 0,
	/* 'ý' */ 0x00FD,   8, 0,
	/* ??? */ 0x010D,   5, 0,									// LATIN SMALL LETTER C WITH CARON
	/* ??? */ 0x0110,  33, MO_AltRight,						// LATIN CAPITAL LETTER D WITH STROKE
	/* ??? */ 0x0111,  32, MO_AltRight,						// LATIN SMALL LETTER D WITH STROKE
	/* ??? */ 0x013E,   3, 0,									// LATIN SMALL LETTER L WITH CARON
	/* ??? */ 0x0141,  39, MO_AltRight,						// LATIN CAPITAL LETTER L WITH STROKE
	/* ??? */ 0x0142,  38, MO_AltRight,						// LATIN SMALL LETTER L WITH STROKE
	/* ??? */ 0x0148,  42, 0,									// LATIN SMALL LETTER N WITH CARON
	/* 'š' */ 0x0161,   4, 0,
	/* ??? */ 0x0165,   6, 0,									// LATIN SMALL LETTER T WITH CARON
	/* 'ž' */ 0x017E,   7, 0,
	/* ??? */ 0x02C7,   3, MO_AltRight|MO_DeadKey,		// CARON
	/* ??? */ 0x02D8,   5, MO_AltRight|MO_DeadKey,		// BREVE
	/* ??? */ 0x02DB,   7, MO_AltRight|MO_DeadKey,		// OGONEK
	/* ??? */ 0x02DD,  11, MO_AltRight|MO_DeadKey,		// DOUBLE ACUTE ACCENT
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Slovakian_QWERTY[73] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,  41, MO_ShiftLeft,
	/* '"' */ 0x0022,  40, MO_ShiftLeft,
	/* '#' */ 0x0023,  47, MO_AltRight,
	/* '$' */ 0x0024,  40, MO_AltRight,
	/* '%' */ 0x0025,  12, MO_ShiftLeft,
	/* '&' */ 0x0026,  45, 0,
	/* ''' */ 0x0027,  26, MO_AltRight,
	/* '(' */ 0x0028,  28, MO_ShiftLeft,
	/* ')' */ 0x0029,  42, MO_ShiftLeft,
	/* '*' */ 0x002A,  45, MO_ShiftLeft,
	/* '+' */ 0x002B,   2, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,  27, MO_ShiftLeft,
	/* '0' */ 0x0030,  11, MO_ShiftLeft,
	/* '1' */ 0x0031,   2, MO_ShiftLeft,
	/* '2' */ 0x0032,   3, MO_ShiftLeft,
	/* '3' */ 0x0033,   4, MO_ShiftLeft,
	/* '4' */ 0x0034,   5, MO_ShiftLeft,
	/* '5' */ 0x0035,   6, MO_ShiftLeft,
	/* '6' */ 0x0036,   7, MO_ShiftLeft,
	/* '7' */ 0x0037,   8, MO_ShiftLeft,
	/* '8' */ 0x0038,   9, MO_ShiftLeft,
	/* '9' */ 0x0039,  10, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,   1, 0,
	/* '<' */ 0x003C,  45, MO_AltRight,
	/* '=' */ 0x003D,  12, 0,
	/* '>' */ 0x003E,  46, MO_AltRight,
	/* '?' */ 0x003F,  53, MO_ShiftLeft,
	/* '@' */ 0x0040,  49, MO_AltRight,
	/* '[' */ 0x005B,  34, MO_AltRight,
	/* '\' */ 0x005C,  17, MO_AltRight,
	/* ']' */ 0x005D,  35, MO_AltRight,
	/* '^' */ 0x005E,   4, MO_AltRight|MO_DeadKey,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,   8, MO_AltRight,
	/* '{' */ 0x007B,  50, MO_AltRight,
	/* '|' */ 0x007C,  18, MO_AltRight,
	/* '}' */ 0x007D,  51, MO_AltRight,
	/* '~' */ 0x007E,   2, MO_AltRight,
	/* '¤' */ 0x00A4,  42, MO_AltRight,
	/* '§' */ 0x00A7,  41, 0,
	/* '¨' */ 0x00A8,  12, MO_AltRight|MO_DeadKey,
	/* '°' */ 0x00B0,   1, MO_ShiftLeft|MO_DeadKey,
	/* '´' */ 0x00B4,  10, MO_AltRight|MO_DeadKey,
	/* '¸' */ 0x00B8,  13, MO_AltRight|MO_DeadKey,
	/* '×' */ 0x00D7,  28, MO_AltRight,
	/* 'ß' */ 0x00DF,  41, MO_AltRight,
	/* 'á' */ 0x00E1,   9, 0,
	/* 'ä' */ 0x00E4,  28, 0,
	/* 'é' */ 0x00E9,  11, 0,
	/* 'í' */ 0x00ED,  10, 0,
	/* 'ô' */ 0x00F4,  40, 0,
	/* '÷' */ 0x00F7,  27, MO_AltRight,
	/* 'ú' */ 0x00FA,  27, 0,
	/* 'ý' */ 0x00FD,   8, 0,
	/* ??? */ 0x010D,   5, 0,									// LATIN SMALL LETTER C WITH CARON
	/* ??? */ 0x0110,  33, MO_AltRight,						// LATIN CAPITAL LETTER D WITH STROKE
	/* ??? */ 0x0111,  32, MO_AltRight,						// LATIN SMALL LETTER D WITH STROKE
	/* ??? */ 0x013E,   3, 0,									// LATIN SMALL LETTER L WITH CARON
	/* ??? */ 0x0141,  39, MO_AltRight,						// LATIN CAPITAL LETTER L WITH STROKE
	/* ??? */ 0x0142,  38, MO_AltRight,						// LATIN SMALL LETTER L WITH STROKE
	/* ??? */ 0x0148,  42, 0,									// LATIN SMALL LETTER N WITH CARON
	/* 'š' */ 0x0161,   4, 0,
	/* ??? */ 0x0165,   6, 0,									// LATIN SMALL LETTER T WITH CARON
	/* 'ž' */ 0x017E,   7, 0,
	/* ??? */ 0x02C7,   3, MO_AltRight|MO_DeadKey,		// CARON
	/* ??? */ 0x02D8,   5, MO_AltRight|MO_DeadKey,		// BREVE
	/* ??? */ 0x02D9,   9, MO_AltRight|MO_DeadKey,		// DOT ABOVE
	/* ??? */ 0x02DB,   7, MO_AltRight|MO_DeadKey,		// OGONEK
	/* ??? */ 0x02DD,  11, MO_AltRight|MO_DeadKey,		// DOUBLE ACUTE ACCENT
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Hungarian_101_Key[68] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   5, MO_ShiftLeft,
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023,  47, MO_AltRight,
	/* '$' */ 0x0024,  40, MO_AltRight,
	/* '&' */ 0x0026,  48, MO_AltRight,
	/* ''' */ 0x0027,   2, MO_ShiftLeft,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  55, MO_AltRight,
	/* '+' */ 0x002B,   4, MO_ShiftLeft,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,   7, MO_ShiftLeft,
	/* '0' */ 0x0030,   1, MO_AltRight,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_AltRight,
	/* '<' */ 0x003C,  45, MO_AltRight,
	/* '=' */ 0x003D,   8, MO_ShiftLeft,
	/* '>' */ 0x003E,  46, MO_AltRight,
	/* '?' */ 0x003F,  53, MO_ShiftLeft,
	/* '@' */ 0x0040,  49, MO_AltRight,
	/* '[' */ 0x005B,  34, MO_AltRight,
	/* '\' */ 0x005C,  17, MO_AltRight,
	/* ']' */ 0x005D,  35, MO_AltRight,
	/* '^' */ 0x005E,   4, MO_AltRight,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,   8, MO_AltRight,
	/* '{' */ 0x007B,  50, MO_AltRight,
	/* '|' */ 0x007C,  18, MO_AltRight,
	/* '}' */ 0x007D,  51, MO_AltRight,
	/* '~' */ 0x007E,   2, MO_AltRight,
	/* '¤' */ 0x00A4,  21, MO_AltRight,
	/* '§' */ 0x00A7,  20, MO_AltRight,
	/* '°' */ 0x00B0,   6, MO_AltRight,
	/* '´' */ 0x00B4,  10, MO_AltRight,
	/* 'Á' */ 0x00C1,  41, MO_ShiftLeft,
	/* 'Ä' */ 0x00C4,  19, MO_AltRight,
	/* 'É' */ 0x00C9,  40, MO_ShiftLeft,
	/* 'Í' */ 0x00CD,   1, MO_ShiftLeft,
	/* 'Ó' */ 0x00D3,  13, MO_ShiftLeft,
	/* 'Ö' */ 0x00D6,  11, MO_ShiftLeft,
	/* '×' */ 0x00D7,  28, MO_AltRight,
	/* 'Ú' */ 0x00DA,  28, MO_ShiftLeft,
	/* 'Ü' */ 0x00DC,  12, MO_ShiftLeft,
	/* 'ß' */ 0x00DF,  41, MO_AltRight,
	/* 'á' */ 0x00E1,  41, 0,
	/* 'ä' */ 0x00E4,  31, MO_AltRight,
	/* 'é' */ 0x00E9,  40, 0,
	/* 'í' */ 0x00ED,   1, 0,
	/* 'ó' */ 0x00F3,  13, 0,
	/* 'ö' */ 0x00F6,  11, 0,
	/* '÷' */ 0x00F7,  27, MO_AltRight,
	/* 'ú' */ 0x00FA,  28, 0,
	/* 'ü' */ 0x00FC,  12, 0,
	/* ??? */ 0x0110,  33, MO_AltRight,						// LATIN CAPITAL LETTER D WITH STROKE
	/* ??? */ 0x0111,  32, MO_AltRight,						// LATIN SMALL LETTER D WITH STROKE
	/* ??? */ 0x0141,  39, MO_AltRight,						// LATIN CAPITAL LETTER L WITH STROKE
	/* ??? */ 0x0142,  38, MO_AltRight,						// LATIN SMALL LETTER L WITH STROKE
	/* ??? */ 0x0150,  27, MO_ShiftLeft,					// LATIN CAPITAL LETTER O WITH DOUBLE ACUTE
	/* ??? */ 0x0151,  27, 0,									// LATIN SMALL LETTER O WITH DOUBLE ACUTE
	/* ??? */ 0x0170,  42, MO_ShiftLeft,					// LATIN CAPITAL LETTER U WITH DOUBLE ACUTE
	/* ??? */ 0x0171,  42, 0,									// LATIN SMALL LETTER U WITH DOUBLE ACUTE
	/* ??? */ 0x02C7,   3, MO_AltRight,						// CARON
	/* ??? */ 0x02D8,   5, MO_AltRight,						// BREVE
	/* ??? */ 0x02D9,   9, MO_AltRight,						// DOT ABOVE
	/* ??? */ 0x02DB,   7, MO_AltRight,						// OGONEK
	/* ??? */ 0x02DD,  11, MO_AltRight,						// DOUBLE ACUTE ACCENT
	/* '€' */ 0x20AC,  23, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Spanish_Variation[50] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   8, MO_ShiftLeft,
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023,   4, MO_AltRight,
	/* '$' */ 0x0024,  31, MO_AltRight,
	/* '%' */ 0x0025,  55, MO_ShiftLeft,
	/* '&' */ 0x0026,  32, MO_AltRight,
	/* ''' */ 0x0027,   1, 0,
	/* '(' */ 0x0028,   5, MO_ShiftLeft,
	/* ')' */ 0x0029,   6, MO_ShiftLeft,
	/* '*' */ 0x002A,  12, MO_AltRight,
	/* '+' */ 0x002B,  12, MO_ShiftLeft,
	/* '/' */ 0x002F,   4, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  55, 0,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  10, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_AltRight,
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,   1, MO_AltRight,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '^' */ 0x005E,  54, MO_AltRight|MO_DeadKey,
	/* '_' */ 0x005F,   8, MO_AltRight,
	/* '`' */ 0x0060,  28, MO_DeadKey,
	/* '{' */ 0x007B,  41, MO_AltRight,
	/* '|' */ 0x007C,   2, MO_AltRight,
	/* '}' */ 0x007D,  42, MO_AltRight,
	/* '~' */ 0x007E,  13, MO_AltRight|MO_DeadKey,
	/* '¡' */ 0x00A1,   7, MO_ShiftLeft,
	/* '£' */ 0x00A3,  37, MO_AltRight,
	/* '§' */ 0x00A7,  10, MO_AltRight,
	/* '¨' */ 0x00A8,  13, MO_DeadKey,
	/* 'ª' */ 0x00AA,   2, MO_ShiftLeft,
	/* '¬' */ 0x00AC,   7, MO_AltRight,
	/* '±' */ 0x00B1,  38, MO_AltRight,
	/* '´' */ 0x00B4,  42, MO_DeadKey,
	/* '·' */ 0x00B7,   1, MO_ShiftLeft,
	/* '¼' */ 0x00BC,   5, MO_AltRight,
	/* '½' */ 0x00BD,   6, MO_AltRight,
	/* '¿' */ 0x00BF,   9, MO_ShiftLeft,
	/* 'Ç' */ 0x00C7,  41, MO_ShiftLeft,
	/* 'Ñ' */ 0x00D1,  40, MO_ShiftLeft,
	/* '×' */ 0x00D7,  27, MO_ShiftLeft,
	/* 'ç' */ 0x00E7,  41, 0,
	/* 'ñ' */ 0x00F1,  40, 0,
	/* '÷' */ 0x00F7,  27, 0,
	/* ??? */ 0x20A7,  11, MO_ShiftLeft,					// PESETA SIGN
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Bulgarian_Cyrillic[146] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   5, MO_ShiftLeft,
	/* '#' */ 0x0023, NOS, 0,
	/* '$' */ 0x0024, NOS, 0,
	/* '&' */ 0x0026, NOS, 0,
	/* ''' */ 0x0027, NOS, 0,
	/* '(' */ 0x0028,  42, 0,
	/* ')' */ 0x0029,  42, MO_ShiftLeft,
	/* '*' */ 0x002A, NOS, 0,
	/* '+' */ 0x002B,   4, MO_ShiftLeft,
	/* ',' */ 0x002C,  17, 0,
	/* '.' */ 0x002E,  13, 0,
	/* '/' */ 0x002F,   9, MO_ShiftLeft,
	/* ':' */ 0x003A,   8, MO_ShiftLeft,
	/* ';' */ 0x003B,  28, 0,
	/* '<' */ 0x003C, NOS, 0,
	/* '=' */ 0x003D,   7, MO_ShiftLeft,
	/* '>' */ 0x003E, NOS, 0,
	/* '?' */ 0x003F,   3, MO_ShiftLeft,
	/* '@' */ 0x0040, NOS, 0,
	/* 'A' */ 0x0041, NOS, 0,
	/* 'B' */ 0x0042, NOS, 0,
	/* 'C' */ 0x0043, NOS, 0,
	/* 'D' */ 0x0044, NOS, 0,
	/* 'E' */ 0x0045, NOS, 0,
	/* 'F' */ 0x0046, NOS, 0,
	/* 'G' */ 0x0047, NOS, 0,
	/* 'H' */ 0x0048, NOS, 0,
	/* 'I' */ 0x0049, NOS, 0,
	/* 'J' */ 0x004A, NOS, 0,
	/* 'K' */ 0x004B, NOS, 0,
	/* 'L' */ 0x004C, NOS, 0,
	/* 'M' */ 0x004D, NOS, 0,
	/* 'N' */ 0x004E, NOS, 0,
	/* 'O' */ 0x004F, NOS, 0,
	/* 'P' */ 0x0050, NOS, 0,
	/* 'Q' */ 0x0051, NOS, 0,
	/* 'R' */ 0x0052, NOS, 0,
	/* 'S' */ 0x0053, NOS, 0,
	/* 'T' */ 0x0054, NOS, 0,
	/* 'U' */ 0x0055, NOS, 0,
	/* 'V' */ 0x0056,  13, MO_ShiftLeft,
	/* 'W' */ 0x0057, NOS, 0,
	/* 'X' */ 0x0058, NOS, 0,
	/* 'Y' */ 0x0059, NOS, 0,
	/* 'Z' */ 0x005A, NOS, 0,
	/* '[' */ 0x005B, NOS, 0,
	/* '\' */ 0x005C,  45, 0,
	/* ']' */ 0x005D, NOS, 0,
	/* '^' */ 0x005E, NOS, 0,
	/* '_' */ 0x005F,  10, MO_ShiftLeft,
	/* 'a' */ 0x0061, NOS, 0,
	/* 'b' */ 0x0062, NOS, 0,
	/* 'c' */ 0x0063, NOS, 0,
	/* 'd' */ 0x0064, NOS, 0,
	/* 'e' */ 0x0065, NOS, 0,
	/* 'f' */ 0x0066, NOS, 0,
	/* 'g' */ 0x0067, NOS, 0,
	/* 'h' */ 0x0068, NOS, 0,
	/* 'i' */ 0x0069, NOS, 0,
	/* 'j' */ 0x006A, NOS, 0,
	/* 'k' */ 0x006B, NOS, 0,
	/* 'l' */ 0x006C, NOS, 0,
	/* 'm' */ 0x006D, NOS, 0,
	/* 'n' */ 0x006E, NOS, 0,
	/* 'o' */ 0x006F, NOS, 0,
	/* 'p' */ 0x0070, NOS, 0,
	/* 'q' */ 0x0071, NOS, 0,
	/* 'r' */ 0x0072, NOS, 0,
	/* 's' */ 0x0073, NOS, 0,
	/* 't' */ 0x0074, NOS, 0,
	/* 'u' */ 0x0075, NOS, 0,
	/* 'v' */ 0x0076, NOS, 0,
	/* 'w' */ 0x0077, NOS, 0,
	/* 'x' */ 0x0078, NOS, 0,
	/* 'y' */ 0x0079, NOS, 0,
	/* 'z' */ 0x007A, NOS, 0,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C,  45, MO_ShiftLeft,
	/* '}' */ 0x007D, NOS, 0,
	/* '§' */ 0x00A7,  28, MO_ShiftLeft,
	/* ??? */ 0x0406,  12, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I
	/* ??? */ 0x0410,  33, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER A
	/* ??? */ 0x0411,  55, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER BE
	/* ??? */ 0x0412,  39, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER VE
	/* ??? */ 0x0413,  36, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER GHE
	/* ??? */ 0x0414,  25, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER DE
	/* ??? */ 0x0415,  19, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER IE
	/* ??? */ 0x0416,  35, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZHE
	/* ??? */ 0x0417,  26, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZE
	/* ??? */ 0x0418,  20, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER I
	/* ??? */ 0x0419,  47, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHORT I
	/* ??? */ 0x041A,  23, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER KA
	/* ??? */ 0x041B,  54, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EL
	/* ??? */ 0x041C,  40, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EM
	/* ??? */ 0x041D,  38, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EN
	/* ??? */ 0x041E,  34, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER O
	/* ??? */ 0x041F,  52, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER PE
	/* ??? */ 0x0420,  53, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ER
	/* ??? */ 0x0421,  24, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ES
	/* ??? */ 0x0422,  37, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TE
	/* ??? */ 0x0423,  18, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER U
	/* ??? */ 0x0424,  50, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EF
	/* ??? */ 0x0425,  51, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER HA
	/* ??? */ 0x0426,  27, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TSE
	/* ??? */ 0x0427,  41, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER CHE
	/* ??? */ 0x0428,  21, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHA
	/* ??? */ 0x0429,  22, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHCHA
	/* ??? */ 0x042A,  48, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER HARD SIGN
	/* ??? */ 0x042C,  31, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SOFT SIGN
	/* ??? */ 0x042D,  49, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER E
	/* ??? */ 0x042E,  46, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YU
	/* ??? */ 0x042F,  32, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YA
	/* ??? */ 0x0430,  33, 0,									// CYRILLIC SMALL LETTER A
	/* ??? */ 0x0431,  55, 0,									// CYRILLIC SMALL LETTER BE
	/* ??? */ 0x0432,  39, 0,									// CYRILLIC SMALL LETTER VE
	/* ??? */ 0x0433,  36, 0,									// CYRILLIC SMALL LETTER GHE
	/* ??? */ 0x0434,  25, 0,									// CYRILLIC SMALL LETTER DE
	/* ??? */ 0x0435,  19, 0,									// CYRILLIC SMALL LETTER IE
	/* ??? */ 0x0436,  35, 0,									// CYRILLIC SMALL LETTER ZHE
	/* ??? */ 0x0437,  26, 0,									// CYRILLIC SMALL LETTER ZE
	/* ??? */ 0x0438,  20, 0,									// CYRILLIC SMALL LETTER I
	/* ??? */ 0x0439,  47, 0,									// CYRILLIC SMALL LETTER SHORT I
	/* ??? */ 0x043A,  23, 0,									// CYRILLIC SMALL LETTER KA
	/* ??? */ 0x043B,  54, 0,									// CYRILLIC SMALL LETTER EL
	/* ??? */ 0x043C,  40, 0,									// CYRILLIC SMALL LETTER EM
	/* ??? */ 0x043D,  38, 0,									// CYRILLIC SMALL LETTER EN
	/* ??? */ 0x043E,  34, 0,									// CYRILLIC SMALL LETTER O
	/* ??? */ 0x043F,  52, 0,									// CYRILLIC SMALL LETTER PE
	/* ??? */ 0x0440,  53, 0,									// CYRILLIC SMALL LETTER ER
	/* ??? */ 0x0441,  24, 0,									// CYRILLIC SMALL LETTER ES
	/* ??? */ 0x0442,  37, 0,									// CYRILLIC SMALL LETTER TE
	/* ??? */ 0x0443,  18, 0,									// CYRILLIC SMALL LETTER U
	/* ??? */ 0x0444,  50, 0,									// CYRILLIC SMALL LETTER EF
	/* ??? */ 0x0445,  51, 0,									// CYRILLIC SMALL LETTER HA
	/* ??? */ 0x0446,  27, 0,									// CYRILLIC SMALL LETTER TSE
	/* ??? */ 0x0447,  41, 0,									// CYRILLIC SMALL LETTER CHE
	/* ??? */ 0x0448,  21, 0,									// CYRILLIC SMALL LETTER SHA
	/* ??? */ 0x0449,  22, 0,									// CYRILLIC SMALL LETTER SHCHA
	/* ??? */ 0x044A,  48, 0,									// CYRILLIC SMALL LETTER HARD SIGN
	/* ??? */ 0x044B,  17, MO_ShiftLeft,					// CYRILLIC SMALL LETTER YERU
	/* ??? */ 0x044C,  31, 0,									// CYRILLIC SMALL LETTER SOFT SIGN
	/* ??? */ 0x044D,  49, 0,									// CYRILLIC SMALL LETTER E
	/* ??? */ 0x044E,  46, 0,									// CYRILLIC SMALL LETTER YU
	/* ??? */ 0x044F,  32, 0,									// CYRILLIC SMALL LETTER YA
	/* ??? */ 0x2116,  11, MO_ShiftLeft,					// NUMERO SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Bulgarian_Latin[3] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '\' */ 0x005C,  42, 0,
	/* '|' */ 0x007C,  42, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Canadian_French_Legacy[63] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* ''' */ 0x0027,  53, MO_ShiftLeft,
	/* '/' */ 0x002F,  55, MO_AltRight,
	/* '<' */ 0x003C,  53, MO_AltRight,
	/* '>' */ 0x003E,  54, MO_AltRight,
	/* '?' */ 0x003F,   7, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_AltRight,
	/* '[' */ 0x005B,   9, MO_AltRight,
	/* '\' */ 0x005C,  45, MO_AltRight,
	/* ']' */ 0x005D,  10, MO_AltRight,
	/* '^' */ 0x005E,  27, MO_DeadKey,
	/* '`' */ 0x0060,  42, MO_AltRight|MO_DeadKey,
	/* '{' */ 0x007B,   8, MO_AltRight,
	/* '|' */ 0x007C,  12, MO_AltRight,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* '~' */ 0x007E,  28, MO_AltRight|MO_DeadKey,
	/* '¡' */ 0x00A1,   2, MO_ShiftLeft|MO_AltRight,
	/* '¢' */ 0x00A2,  48, MO_AltRight,
	/* '£' */ 0x00A3,   4, MO_ShiftLeft|MO_AltRight,
	/* '¤' */ 0x00A4,   5, MO_ShiftLeft|MO_AltRight,
	/* '¥' */ 0x00A5,  22, MO_AltRight,
	/* '§' */ 0x00A7,  32, MO_ShiftLeft|MO_AltRight,
	/* '¨' */ 0x00A8,  28, MO_ShiftLeft|MO_AltRight|MO_DeadKey,
	/* '©' */ 0x00A9,  48, MO_ShiftLeft|MO_AltRight,
	/* 'ª' */ 0x00AA,  34, MO_AltRight,
	/* '«' */ 0x00AB,  46, MO_AltRight,
	/* '¬' */ 0x00AC,   1, MO_AltRight,
	/* '®' */ 0x00AE,  20, MO_ShiftLeft|MO_AltRight,
	/* '°' */ 0x00B0,   1, 0,
	/* '±' */ 0x00B1,  10, MO_ShiftLeft|MO_AltRight,
	/* '²' */ 0x00B2,   3, MO_ShiftLeft|MO_AltRight,
	/* '³' */ 0x00B3,   4, MO_AltRight,
	/* '´' */ 0x00B4,  40, MO_AltRight|MO_DeadKey,
	/* 'µ' */ 0x00B5,  52, MO_AltRight,
	/* '¶' */ 0x00B6,  20, MO_AltRight,
	/* '¸' */ 0x00B8,  13, MO_AltRight,
	/* '¹' */ 0x00B9,   2, MO_AltRight,
	/* 'º' */ 0x00BA,  52, MO_ShiftLeft|MO_AltRight,
	/* '»' */ 0x00BB,  47, MO_AltRight,
	/* '¼' */ 0x00BC,   5, MO_AltRight,
	/* '½' */ 0x00BD,   6, MO_AltRight,
	/* '¾' */ 0x00BE,   7, MO_AltRight,
	/* '¿' */ 0x00BF,  12, MO_ShiftLeft|MO_AltRight,
	/* 'À' */ 0x00C0,  42, MO_ShiftLeft,
	/* 'Æ' */ 0x00C6,  31, MO_ShiftLeft|MO_AltRight,
	/* 'Ç' */ 0x00C7,  28, MO_ShiftLeft,
	/* 'È' */ 0x00C8,  41, MO_ShiftLeft,
	/* 'É' */ 0x00C9,  55, MO_ShiftLeft,
	/* 'Ð' */ 0x00D0,  33, MO_ShiftLeft|MO_AltRight,
	/* 'Ø' */ 0x00D8,  25, MO_ShiftLeft|MO_AltRight,
	/* 'Ù' */ 0x00D9,  45, MO_ShiftLeft,
	/* 'Þ' */ 0x00DE,  26, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0x00DF,  32, MO_AltRight,
	/* 'à' */ 0x00E0,  42, 0,
	/* 'æ' */ 0x00E6,  31, MO_AltRight,
	/* 'ç' */ 0x00E7,  28, 0,
	/* 'è' */ 0x00E8,  41, 0,
	/* 'é' */ 0x00E9,  55, 0,
	/* 'ð' */ 0x00F0,  33, MO_AltRight,
	/* 'ø' */ 0x00F8,  25, MO_AltRight,
	/* 'ù' */ 0x00F9,  45, 0,
	/* 'þ' */ 0x00FE,  26, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Canadian_Multilingual[33] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,  54, MO_ShiftLeft,
	/* ''' */ 0x0027,  53, MO_ShiftLeft,
	/* '/' */ 0x002F,   1, 0,
	/* '<' */ 0x003C,  53, MO_AltRight,
	/* '>' */ 0x003E,  54, MO_AltRight,
	/* '?' */ 0x003F,   7, MO_ShiftLeft,
	/* '[' */ 0x005B,  10, MO_AltRight,
	/* '\' */ 0x005C,   1, MO_ShiftLeft,
	/* ']' */ 0x005D,  11, MO_AltRight,
	/* '^' */ 0x005E,  27, MO_DeadKey,
	/* '`' */ 0x0060,  27, MO_AltRight|MO_DeadKey,
	/* '{' */ 0x007B,   8, MO_AltRight,
	/* '|' */ 0x007C,   1, MO_AltRight,
	/* '}' */ 0x007D,   9, MO_AltRight,
	/* '~' */ 0x007E,  28, MO_AltRight|MO_DeadKey,
	/* '¤' */ 0x00A4,   5, MO_AltRight,
	/* '¨' */ 0x00A8,  27, MO_ShiftLeft|MO_DeadKey,
	/* '«' */ 0x00AB,  46, MO_AltRight,
	/* '¬' */ 0x00AC,  13, MO_AltRight,
	/* '°' */ 0x00B0,  40, MO_AltRight,
	/* '»' */ 0x00BB,  47, MO_AltRight,
	/* 'À' */ 0x00C0,  42, MO_ShiftLeft,
	/* 'Ç' */ 0x00C7,  28, MO_ShiftLeft,
	/* 'È' */ 0x00C8,  41, MO_ShiftLeft,
	/* 'É' */ 0x00C9,  55, MO_ShiftLeft,
	/* 'Ù' */ 0x00D9,  45, MO_ShiftLeft,
	/* 'à' */ 0x00E0,  42, 0,
	/* 'ç' */ 0x00E7,  28, 0,
	/* 'è' */ 0x00E8,  41, 0,
	/* 'é' */ 0x00E9,  55, 0,
	/* 'ù' */ 0x00F9,  45, 0,
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Italian_142[39] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023,   4, MO_AltRight,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,  12, 0,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  28, MO_ShiftLeft,
	/* '+' */ 0x002B,  28, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,  17, MO_AltRight,
	/* '[' */ 0x005B,   9, MO_AltRight,
	/* '\' */ 0x005C,   1, 0,
	/* ']' */ 0x005D,  10, MO_AltRight,
	/* '^' */ 0x005E,  13, MO_ShiftLeft,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,  42, MO_AltRight,
	/* '{' */ 0x007B,   8, MO_AltRight,
	/* '|' */ 0x007C,   1, MO_ShiftLeft,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* '~' */ 0x007E,  28, MO_AltRight,
	/* '£' */ 0x00A3,   4, MO_ShiftLeft,
	/* '§' */ 0x00A7,  42, MO_ShiftLeft,
	/* '°' */ 0x00B0,  41, MO_ShiftLeft,
	/* 'à' */ 0x00E0,  41, 0,
	/* 'ç' */ 0x00E7,  40, MO_ShiftLeft,
	/* 'è' */ 0x00E8,  27, 0,
	/* 'é' */ 0x00E9,  27, MO_ShiftLeft,
	/* 'ì' */ 0x00EC,  13, 0,
	/* 'ò' */ 0x00F2,  40, 0,
	/* 'ù' */ 0x00F9,  42, 0,
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Polish_214[60] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '$' */ 0x0024,  40, MO_AltRight,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,  13, 0,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  13, MO_ShiftLeft,
	/* '+' */ 0x002B,  12, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,  49, MO_AltRight,
	/* 'Y' */ 0x0059,  46, MO_ShiftLeft,
	/* 'Z' */ 0x005A,  22, MO_ShiftLeft,
	/* '[' */ 0x005B, NOS, 0,
	/* '\' */ 0x005C,  17, MO_AltRight,
	/* ']' */ 0x005D, NOS, 0,
	/* '^' */ 0x005E,   4, MO_AltRight|MO_DeadKey,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,   8, MO_AltRight,
	/* 'y' */ 0x0079,  46, 0,
	/* 'z' */ 0x007A,  22, 0,
	/* '{' */ 0x007B,  50, MO_AltRight,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D,  51, MO_AltRight,
	/* '~' */ 0x007E,   2, MO_AltRight,
	/* '¤' */ 0x00A4,   5, MO_ShiftLeft,
	/* '¦' */ 0x00A6,  18, MO_AltRight,
	/* '§' */ 0x00A7,  52, MO_AltRight,
	/* '¨' */ 0x00A8,  12, MO_AltRight|MO_DeadKey,
	/* '°' */ 0x00B0,   6, MO_AltRight|MO_DeadKey,
	/* '´' */ 0x00B4,  10, MO_AltRight|MO_DeadKey,
	/* '·' */ 0x00B7,   1, MO_ShiftLeft|MO_DeadKey,
	/* '¸' */ 0x00B8,  13, MO_AltRight|MO_DeadKey,
	/* '×' */ 0x00D7,  28, MO_AltRight,
	/* 'ß' */ 0x00DF,  41, MO_AltRight,
	/* 'ó' */ 0x00F3,  42, 0,
	/* '÷' */ 0x00F7,  27, MO_AltRight,
	/* ??? */ 0x0105,  41, 0,									// LATIN SMALL LETTER A WITH OGONEK
	/* ??? */ 0x0107,  28, MO_ShiftLeft,					// LATIN SMALL LETTER C WITH ACUTE
	/* ??? */ 0x0110,  33, MO_AltRight,						// LATIN CAPITAL LETTER D WITH STROKE
	/* ??? */ 0x0111,  32, MO_AltRight,						// LATIN SMALL LETTER D WITH STROKE
	/* ??? */ 0x0119,  41, MO_ShiftLeft,					// LATIN SMALL LETTER E WITH OGONEK
	/* ??? */ 0x0141,  40, MO_ShiftLeft,					// LATIN CAPITAL LETTER L WITH STROKE
	/* ??? */ 0x0142,  40, 0,									// LATIN SMALL LETTER L WITH STROKE
	/* ??? */ 0x0144,  27, MO_ShiftLeft,					// LATIN SMALL LETTER N WITH ACUTE
	/* ??? */ 0x015B,  28, 0,									// LATIN SMALL LETTER S WITH ACUTE
	/* ??? */ 0x017A,  42, MO_ShiftLeft,					// LATIN SMALL LETTER Z WITH ACUTE
	/* ??? */ 0x017C,  27, 0,									// LATIN SMALL LETTER Z WITH DOT ABOVE
	/* ??? */ 0x02C7,   3, MO_AltRight|MO_DeadKey,		// CARON
	/* ??? */ 0x02D8,   5, MO_AltRight|MO_DeadKey,		// BREVE
	/* ??? */ 0x02DB,   1, MO_DeadKey,						// OGONEK
	/* ??? */ 0x02DD,  11, MO_AltRight|MO_DeadKey,		// DOUBLE ACUTE ACCENT
	/* '€' */ 0x20AC,  23, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Polish_Programmers[23] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '\' */ 0x005C,  42, 0,
	/* '|' */ 0x007C,  42, MO_ShiftLeft,
	/* '~' */ 0x007E,   1, MO_ShiftLeft|MO_DeadKey,
	/* 'Ó' */ 0x00D3,  25, MO_ShiftLeft|MO_AltRight,
	/* 'ó' */ 0x00F3,  25, MO_AltRight,
	/* ??? */ 0x0104,  31, MO_ShiftLeft|MO_AltRight,	// LATIN CAPITAL LETTER A WITH OGONEK
	/* ??? */ 0x0105,  31, MO_AltRight,						// LATIN SMALL LETTER A WITH OGONEK
	/* ??? */ 0x0106,  48, MO_ShiftLeft|MO_AltRight,	// LATIN CAPITAL LETTER C WITH ACUTE
	/* ??? */ 0x0107,  48, MO_AltRight,						// LATIN SMALL LETTER C WITH ACUTE
	/* ??? */ 0x0118,  19, MO_ShiftLeft|MO_AltRight,	// LATIN CAPITAL LETTER E WITH OGONEK
	/* ??? */ 0x0119,  19, MO_AltRight,						// LATIN SMALL LETTER E WITH OGONEK
	/* ??? */ 0x0141,  39, MO_ShiftLeft|MO_AltRight,	// LATIN CAPITAL LETTER L WITH STROKE
	/* ??? */ 0x0142,  39, MO_AltRight,						// LATIN SMALL LETTER L WITH STROKE
	/* ??? */ 0x0143,  51, MO_ShiftLeft|MO_AltRight,	// LATIN CAPITAL LETTER N WITH ACUTE
	/* ??? */ 0x0144,  51, MO_AltRight,						// LATIN SMALL LETTER N WITH ACUTE
	/* ??? */ 0x015A,  32, MO_ShiftLeft|MO_AltRight,	// LATIN CAPITAL LETTER S WITH ACUTE
	/* ??? */ 0x015B,  32, MO_AltRight,						// LATIN SMALL LETTER S WITH ACUTE
	/* ??? */ 0x0179,  47, MO_ShiftLeft|MO_AltRight,	// LATIN CAPITAL LETTER Z WITH ACUTE
	/* ??? */ 0x017A,  47, MO_AltRight,						// LATIN SMALL LETTER Z WITH ACUTE
	/* ??? */ 0x017B,  46, MO_ShiftLeft|MO_AltRight,	// LATIN CAPITAL LETTER Z WITH DOT ABOVE
	/* ??? */ 0x017C,  46, MO_AltRight,						// LATIN SMALL LETTER Z WITH DOT ABOVE
	/* '€' */ 0x20AC,  23, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Brazilian_MS[31] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   1, MO_ShiftLeft,
	/* ''' */ 0x0027,   1, 0,
	/* '/' */ 0x002F,  17, MO_AltRight,
	/* ':' */ 0x003A,  55, MO_ShiftLeft,
	/* ';' */ 0x003B,  55, 0,
	/* '?' */ 0x003F,  18, MO_AltRight,
	/* '[' */ 0x005B,  28, 0,
	/* '\' */ 0x005C,  45, 0,
	/* ']' */ 0x005D,  42, 0,
	/* '^' */ 0x005E,  41, MO_ShiftLeft|MO_DeadKey,
	/* '`' */ 0x0060,  27, MO_ShiftLeft|MO_DeadKey,
	/* '{' */ 0x007B,  28, MO_ShiftLeft,
	/* '|' */ 0x007C,  45, MO_ShiftLeft,
	/* '}' */ 0x007D,  42, MO_ShiftLeft,
	/* '~' */ 0x007E,  41, MO_DeadKey,
	/* '¢' */ 0x00A2,   6, MO_AltRight,
	/* '£' */ 0x00A3,   5, MO_AltRight,
	/* '§' */ 0x00A7,  13, MO_AltRight,
	/* '¨' */ 0x00A8,   7, MO_ShiftLeft|MO_DeadKey,
	/* 'ª' */ 0x00AA,  28, MO_AltRight,
	/* '¬' */ 0x00AC,   7, MO_AltRight,
	/* '°' */ 0x00B0,  19, MO_AltRight,
	/* '²' */ 0x00B2,   3, MO_AltRight,
	/* '³' */ 0x00B3,   4, MO_AltRight,
	/* '´' */ 0x00B4,  27, MO_DeadKey,
	/* '¹' */ 0x00B9,   2, MO_AltRight,
	/* 'º' */ 0x00BA,  42, MO_AltRight,
	/* 'Ç' */ 0x00C7,  40, MO_ShiftLeft,
	/* 'ç' */ 0x00E7,  40, 0,
	/* ??? */ 0x20A2,  48, MO_AltRight,						// CRUZEIRO SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Greek_Polytonic[154] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,  41, MO_ShiftLeft|MO_DeadKey,
	/* ''' */ 0x0027,  41, MO_DeadKey,
	/* '+' */ 0x002B,  13, MO_ShiftLeft|MO_DeadKey,
	/* '-' */ 0x002D,  12, MO_DeadKey,
	/* '/' */ 0x002F,  55, MO_DeadKey,
	/* ':' */ 0x003A,  17, MO_ShiftLeft|MO_DeadKey,
	/* ';' */ 0x003B,  17, MO_DeadKey,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  13, MO_DeadKey,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  55, MO_ShiftLeft|MO_DeadKey,
	/* 'A' */ 0x0041, NOS, 0,
	/* 'B' */ 0x0042, NOS, 0,
	/* 'C' */ 0x0043, NOS, 0,
	/* 'D' */ 0x0044, NOS, 0,
	/* 'E' */ 0x0045, NOS, 0,
	/* 'F' */ 0x0046, NOS, 0,
	/* 'G' */ 0x0047, NOS, 0,
	/* 'H' */ 0x0048, NOS, 0,
	/* 'I' */ 0x0049, NOS, 0,
	/* 'J' */ 0x004A, NOS, 0,
	/* 'K' */ 0x004B, NOS, 0,
	/* 'L' */ 0x004C, NOS, 0,
	/* 'M' */ 0x004D, NOS, 0,
	/* 'N' */ 0x004E, NOS, 0,
	/* 'O' */ 0x004F, NOS, 0,
	/* 'P' */ 0x0050, NOS, 0,
	/* 'Q' */ 0x0051, NOS, 0,
	/* 'R' */ 0x0052, NOS, 0,
	/* 'S' */ 0x0053, NOS, 0,
	/* 'T' */ 0x0054, NOS, 0,
	/* 'U' */ 0x0055, NOS, 0,
	/* 'V' */ 0x0056, NOS, 0,
	/* 'W' */ 0x0057, NOS, 0,
	/* 'X' */ 0x0058, NOS, 0,
	/* 'Y' */ 0x0059, NOS, 0,
	/* 'Z' */ 0x005A, NOS, 0,
	/* '[' */ 0x005B,  27, MO_DeadKey,
	/* '\' */ 0x005C,  42, MO_DeadKey,
	/* ']' */ 0x005D,  28, MO_DeadKey,
	/* '_' */ 0x005F,  12, MO_ShiftLeft|MO_DeadKey,
	/* '`' */ 0x0060,   1, MO_ShiftLeft|MO_DeadKey,
	/* 'a' */ 0x0061, NOS, 0,
	/* 'b' */ 0x0062, NOS, 0,
	/* 'c' */ 0x0063, NOS, 0,
	/* 'd' */ 0x0064, NOS, 0,
	/* 'e' */ 0x0065, NOS, 0,
	/* 'f' */ 0x0066, NOS, 0,
	/* 'g' */ 0x0067, NOS, 0,
	/* 'h' */ 0x0068, NOS, 0,
	/* 'i' */ 0x0069, NOS, 0,
	/* 'j' */ 0x006A, NOS, 0,
	/* 'k' */ 0x006B, NOS, 0,
	/* 'l' */ 0x006C, NOS, 0,
	/* 'm' */ 0x006D, NOS, 0,
	/* 'n' */ 0x006E, NOS, 0,
	/* 'o' */ 0x006F, NOS, 0,
	/* 'p' */ 0x0070, NOS, 0,
	/* 'q' */ 0x0071, NOS, 0,
	/* 'r' */ 0x0072, NOS, 0,
	/* 's' */ 0x0073, NOS, 0,
	/* 't' */ 0x0074, NOS, 0,
	/* 'u' */ 0x0075, NOS, 0,
	/* 'v' */ 0x0076, NOS, 0,
	/* 'w' */ 0x0077, NOS, 0,
	/* 'x' */ 0x0078, NOS, 0,
	/* 'y' */ 0x0079, NOS, 0,
	/* 'z' */ 0x007A, NOS, 0,
	/* '{' */ 0x007B,  27, MO_ShiftLeft|MO_DeadKey,
	/* '|' */ 0x007C,  42, MO_ShiftLeft|MO_DeadKey,
	/* '}' */ 0x007D,  28, MO_ShiftLeft|MO_DeadKey,
	/* '~' */ 0x007E,   1, MO_DeadKey,
	/* '£' */ 0x00A3,   5, MO_AltRight,
	/* '¤' */ 0x00A4,   9, MO_AltRight,
	/* '¥' */ 0x00A5,  22, MO_AltRight,
	/* '¦' */ 0x00A6,  10, MO_AltRight,
	/* '§' */ 0x00A7,   6, MO_AltRight,
	/* '¨' */ 0x00A8,  40, MO_ShiftLeft|MO_DeadKey,
	/* '©' */ 0x00A9,  48, MO_AltRight,
	/* '«' */ 0x00AB,  27, MO_AltRight|MO_DeadKey,
	/* '¬' */ 0x00AC,  42, MO_AltRight|MO_DeadKey,
	/* '®' */ 0x00AE,  20, MO_AltRight,
	/* '°' */ 0x00B0,  11, MO_AltRight,
	/* '±' */ 0x00B1,  12, MO_AltRight,
	/* '²' */ 0x00B2,   3, MO_ShiftLeft|MO_AltRight,
	/* '³' */ 0x00B3,   4, MO_ShiftLeft|MO_AltRight,
	/* '¶' */ 0x00B6,   7, MO_AltRight,
	/* '»' */ 0x00BB,  28, MO_AltRight|MO_DeadKey,
	/* '½' */ 0x00BD,  13, MO_AltRight|MO_DeadKey,
	/* ??? */ 0x0384,  40, MO_DeadKey,						// GREEK TONOS
	/* ??? */ 0x0385,  18, MO_ShiftLeft|MO_DeadKey,		// GREEK DIALYTIKA TONOS
	/* ??? */ 0x0387,  28, MO_ShiftLeft|MO_AltRight,	// GREEK ANO TELEIA
	/* ??? */ 0x0391,  31, MO_ShiftLeft,					// GREEK CAPITAL LETTER ALPHA
	/* ??? */ 0x0392,  50, MO_ShiftLeft,					// GREEK CAPITAL LETTER BETA
	/* ??? */ 0x0393,  35, MO_ShiftLeft,					// GREEK CAPITAL LETTER GAMMA
	/* ??? */ 0x0394,  33, MO_ShiftLeft,					// GREEK CAPITAL LETTER DELTA
	/* ??? */ 0x0395,  19, MO_ShiftLeft,					// GREEK CAPITAL LETTER EPSILON
	/* ??? */ 0x0396,  46, MO_ShiftLeft,					// GREEK CAPITAL LETTER ZETA
	/* ??? */ 0x0397,  36, MO_ShiftLeft,					// GREEK CAPITAL LETTER ETA
	/* ??? */ 0x0398,  23, MO_ShiftLeft,					// GREEK CAPITAL LETTER THETA
	/* ??? */ 0x0399,  24, MO_ShiftLeft,					// GREEK CAPITAL LETTER IOTA
	/* ??? */ 0x039A,  38, MO_ShiftLeft,					// GREEK CAPITAL LETTER KAPPA
	/* ??? */ 0x039B,  39, MO_ShiftLeft,					// GREEK CAPITAL LETTER LAMDA
	/* ??? */ 0x039C,  52, MO_ShiftLeft,					// GREEK CAPITAL LETTER MU
	/* ??? */ 0x039D,  51, MO_ShiftLeft,					// GREEK CAPITAL LETTER NU
	/* ??? */ 0x039E,  37, MO_ShiftLeft,					// GREEK CAPITAL LETTER XI
	/* ??? */ 0x039F,  25, MO_ShiftLeft,					// GREEK CAPITAL LETTER OMICRON
	/* ??? */ 0x03A0,  26, MO_ShiftLeft,					// GREEK CAPITAL LETTER PI
	/* ??? */ 0x03A1,  20, MO_ShiftLeft,					// GREEK CAPITAL LETTER RHO
	/* ??? */ 0x03A3,  32, MO_ShiftLeft,					// GREEK CAPITAL LETTER SIGMA
	/* ??? */ 0x03A4,  21, MO_ShiftLeft,					// GREEK CAPITAL LETTER TAU
	/* ??? */ 0x03A5,  22, MO_ShiftLeft,					// GREEK CAPITAL LETTER UPSILON
	/* ??? */ 0x03A6,  34, MO_ShiftLeft,					// GREEK CAPITAL LETTER PHI
	/* ??? */ 0x03A7,  47, MO_ShiftLeft,					// GREEK CAPITAL LETTER CHI
	/* ??? */ 0x03A8,  48, MO_ShiftLeft,					// GREEK CAPITAL LETTER PSI
	/* ??? */ 0x03A9,  49, MO_ShiftLeft,					// GREEK CAPITAL LETTER OMEGA
	/* ??? */ 0x03B1,  31, 0,									// GREEK SMALL LETTER ALPHA
	/* ??? */ 0x03B2,  50, 0,									// GREEK SMALL LETTER BETA
	/* ??? */ 0x03B3,  35, 0,									// GREEK SMALL LETTER GAMMA
	/* ??? */ 0x03B4,  33, 0,									// GREEK SMALL LETTER DELTA
	/* ??? */ 0x03B5,  19, 0,									// GREEK SMALL LETTER EPSILON
	/* ??? */ 0x03B6,  46, 0,									// GREEK SMALL LETTER ZETA
	/* ??? */ 0x03B7,  36, 0,									// GREEK SMALL LETTER ETA
	/* ??? */ 0x03B8,  23, 0,									// GREEK SMALL LETTER THETA
	/* ??? */ 0x03B9,  24, 0,									// GREEK SMALL LETTER IOTA
	/* ??? */ 0x03BA,  38, 0,									// GREEK SMALL LETTER KAPPA
	/* ??? */ 0x03BB,  39, 0,									// GREEK SMALL LETTER LAMDA
	/* ??? */ 0x03BC,  52, 0,									// GREEK SMALL LETTER MU
	/* ??? */ 0x03BD,  51, 0,									// GREEK SMALL LETTER NU
	/* ??? */ 0x03BE,  37, 0,									// GREEK SMALL LETTER XI
	/* ??? */ 0x03BF,  25, 0,									// GREEK SMALL LETTER OMICRON
	/* ??? */ 0x03C0,  26, 0,									// GREEK SMALL LETTER PI
	/* ??? */ 0x03C1,  20, 0,									// GREEK SMALL LETTER RHO
	/* ??? */ 0x03C2,  18, 0,									// GREEK SMALL LETTER FINAL SIGMA
	/* ??? */ 0x03C3,  32, 0,									// GREEK SMALL LETTER SIGMA
	/* ??? */ 0x03C4,  21, 0,									// GREEK SMALL LETTER TAU
	/* ??? */ 0x03C5,  22, 0,									// GREEK SMALL LETTER UPSILON
	/* ??? */ 0x03C6,  34, 0,									// GREEK SMALL LETTER PHI
	/* ??? */ 0x03C7,  47, 0,									// GREEK SMALL LETTER CHI
	/* ??? */ 0x03C8,  48, 0,									// GREEK SMALL LETTER PSI
	/* ??? */ 0x03C9,  49, 0,									// GREEK SMALL LETTER OMEGA
	/* ??? */ 0x03DA,   2, MO_AltRight,						// GREEK LETTER STIGMA
	/* ??? */ 0x03DE,   3, MO_AltRight,						// GREEK LETTER KOPPA
	/* ??? */ 0x03E0,   4, MO_AltRight,						// GREEK LETTER SAMPI
	/* ??? */ 0x1FBE,  55, MO_AltRight|MO_DeadKey,		// GREEK PROSGEGRAMMENI
	/* ??? */ 0x1FBF,  41, MO_AltRight|MO_DeadKey,		// GREEK PSILI
	/* ??? */ 0x1FC1,   1, MO_AltRight|MO_DeadKey,		// GREEK DIALYTIKA AND PERISPOMENI
	/* ??? */ 0x1FDD,  42, MO_ShiftLeft|MO_AltRight|MO_DeadKey,// GREEK DASIA AND VARIA
	/* ??? */ 0x1FDE,  55, MO_ShiftLeft|MO_AltRight|MO_DeadKey,// GREEK DASIA AND OXIA
	/* ??? */ 0x1FDF,  13, MO_ShiftLeft|MO_AltRight|MO_DeadKey,// GREEK DASIA AND PERISPOMENI
	/* ??? */ 0x1FFD,  17, MO_AltRight|MO_DeadKey,		// GREEK OXIA
	/* ??? */ 0x1FFE,  41, MO_ShiftLeft|MO_AltRight|MO_DeadKey,// GREEK DASIA
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Greek_220[150] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023,  42, 0,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,  12, 0,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  27, MO_ShiftLeft,
	/* '+' */ 0x002B,  27, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
	/* ':' */ 0x003A,  17, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,  42, MO_ShiftLeft,
	/* 'A' */ 0x0041, NOS, 0,
	/* 'B' */ 0x0042, NOS, 0,
	/* 'C' */ 0x0043, NOS, 0,
	/* 'D' */ 0x0044, NOS, 0,
	/* 'E' */ 0x0045, NOS, 0,
	/* 'F' */ 0x0046, NOS, 0,
	/* 'G' */ 0x0047, NOS, 0,
	/* 'H' */ 0x0048, NOS, 0,
	/* 'I' */ 0x0049, NOS, 0,
	/* 'J' */ 0x004A, NOS, 0,
	/* 'K' */ 0x004B, NOS, 0,
	/* 'L' */ 0x004C, NOS, 0,
	/* 'M' */ 0x004D, NOS, 0,
	/* 'N' */ 0x004E, NOS, 0,
	/* 'O' */ 0x004F, NOS, 0,
	/* 'P' */ 0x0050, NOS, 0,
	/* 'Q' */ 0x0051, NOS, 0,
	/* 'R' */ 0x0052, NOS, 0,
	/* 'S' */ 0x0053, NOS, 0,
	/* 'T' */ 0x0054, NOS, 0,
	/* 'U' */ 0x0055, NOS, 0,
	/* 'V' */ 0x0056, NOS, 0,
	/* 'W' */ 0x0057, NOS, 0,
	/* 'X' */ 0x0058, NOS, 0,
	/* 'Y' */ 0x0059, NOS, 0,
	/* 'Z' */ 0x005A, NOS, 0,
	/* '[' */ 0x005B,  13, MO_ShiftLeft,
	/* '\' */ 0x005C, NOS, 0,
	/* ']' */ 0x005D,  13, 0,
	/* '^' */ 0x005E, NOS, 0,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060, NOS, 0,
	/* 'a' */ 0x0061, NOS, 0,
	/* 'b' */ 0x0062, NOS, 0,
	/* 'c' */ 0x0063, NOS, 0,
	/* 'd' */ 0x0064, NOS, 0,
	/* 'e' */ 0x0065, NOS, 0,
	/* 'f' */ 0x0066, NOS, 0,
	/* 'g' */ 0x0067, NOS, 0,
	/* 'h' */ 0x0068, NOS, 0,
	/* 'i' */ 0x0069, NOS, 0,
	/* 'j' */ 0x006A, NOS, 0,
	/* 'k' */ 0x006B, NOS, 0,
	/* 'l' */ 0x006C, NOS, 0,
	/* 'm' */ 0x006D, NOS, 0,
	/* 'n' */ 0x006E, NOS, 0,
	/* 'o' */ 0x006F, NOS, 0,
	/* 'p' */ 0x0070, NOS, 0,
	/* 'q' */ 0x0071, NOS, 0,
	/* 'r' */ 0x0072, NOS, 0,
	/* 's' */ 0x0073, NOS, 0,
	/* 't' */ 0x0074, NOS, 0,
	/* 'u' */ 0x0075, NOS, 0,
	/* 'v' */ 0x0076, NOS, 0,
	/* 'w' */ 0x0077, NOS, 0,
	/* 'x' */ 0x0078, NOS, 0,
	/* 'y' */ 0x0079, NOS, 0,
	/* 'z' */ 0x007A, NOS, 0,
	/* '{' */ 0x007B,  28, MO_ShiftLeft,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D,  28, 0,
	/* '~' */ 0x007E,  18, MO_ShiftLeft,
	/* '£' */ 0x00A3,   4, MO_ShiftLeft,
	/* '¤' */ 0x00A4,   9, MO_AltRight,
	/* '¥' */ 0x00A5,  22, MO_AltRight,
	/* '¦' */ 0x00A6,  10, MO_AltRight,
	/* '§' */ 0x00A7,   6, MO_AltRight,
	/* '¨' */ 0x00A8,  40, MO_ShiftLeft|MO_DeadKey,
	/* '©' */ 0x00A9,  48, MO_AltRight,
	/* '«' */ 0x00AB,  27, MO_AltRight,
	/* '¬' */ 0x00AC,  42, MO_AltRight,
	/* '®' */ 0x00AE,  20, MO_AltRight,
	/* '°' */ 0x00B0,  11, MO_AltRight,
	/* '±' */ 0x00B1,   1, MO_ShiftLeft,
	/* '²' */ 0x00B2,   3, MO_AltRight,
	/* '³' */ 0x00B3,   4, MO_AltRight,
	/* '¶' */ 0x00B6,   7, MO_AltRight,
	/* '»' */ 0x00BB,  28, MO_AltRight,
	/* '½' */ 0x00BD,   1, 0,
	/* ??? */ 0x037E,  17, 0,									// GREEK QUESTION MARK
	/* ??? */ 0x0384,  40, MO_DeadKey,						// GREEK TONOS
	/* ??? */ 0x0385,  40, MO_AltRight|MO_DeadKey,		// GREEK DIALYTIKA TONOS
	/* ??? */ 0x0391,  31, MO_ShiftLeft,					// GREEK CAPITAL LETTER ALPHA
	/* ??? */ 0x0392,  50, MO_ShiftLeft,					// GREEK CAPITAL LETTER BETA
	/* ??? */ 0x0393,  35, MO_ShiftLeft,					// GREEK CAPITAL LETTER GAMMA
	/* ??? */ 0x0394,  33, MO_ShiftLeft,					// GREEK CAPITAL LETTER DELTA
	/* ??? */ 0x0395,  19, MO_ShiftLeft,					// GREEK CAPITAL LETTER EPSILON
	/* ??? */ 0x0396,  46, MO_ShiftLeft,					// GREEK CAPITAL LETTER ZETA
	/* ??? */ 0x0397,  36, MO_ShiftLeft,					// GREEK CAPITAL LETTER ETA
	/* ??? */ 0x0398,  23, MO_ShiftLeft,					// GREEK CAPITAL LETTER THETA
	/* ??? */ 0x0399,  24, MO_ShiftLeft,					// GREEK CAPITAL LETTER IOTA
	/* ??? */ 0x039A,  38, MO_ShiftLeft,					// GREEK CAPITAL LETTER KAPPA
	/* ??? */ 0x039B,  39, MO_ShiftLeft,					// GREEK CAPITAL LETTER LAMDA
	/* ??? */ 0x039C,  52, MO_ShiftLeft,					// GREEK CAPITAL LETTER MU
	/* ??? */ 0x039D,  51, MO_ShiftLeft,					// GREEK CAPITAL LETTER NU
	/* ??? */ 0x039E,  37, MO_ShiftLeft,					// GREEK CAPITAL LETTER XI
	/* ??? */ 0x039F,  25, MO_ShiftLeft,					// GREEK CAPITAL LETTER OMICRON
	/* ??? */ 0x03A0,  26, MO_ShiftLeft,					// GREEK CAPITAL LETTER PI
	/* ??? */ 0x03A1,  20, MO_ShiftLeft,					// GREEK CAPITAL LETTER RHO
	/* ??? */ 0x03A3,  32, MO_ShiftLeft,					// GREEK CAPITAL LETTER SIGMA
	/* ??? */ 0x03A4,  21, MO_ShiftLeft,					// GREEK CAPITAL LETTER TAU
	/* ??? */ 0x03A5,  22, MO_ShiftLeft,					// GREEK CAPITAL LETTER UPSILON
	/* ??? */ 0x03A6,  34, MO_ShiftLeft,					// GREEK CAPITAL LETTER PHI
	/* ??? */ 0x03A7,  47, MO_ShiftLeft,					// GREEK CAPITAL LETTER CHI
	/* ??? */ 0x03A8,  48, MO_ShiftLeft,					// GREEK CAPITAL LETTER PSI
	/* ??? */ 0x03A9,  49, MO_ShiftLeft,					// GREEK CAPITAL LETTER OMEGA
	/* ??? */ 0x03B1,  31, 0,									// GREEK SMALL LETTER ALPHA
	/* ??? */ 0x03B2,  50, 0,									// GREEK SMALL LETTER BETA
	/* ??? */ 0x03B3,  35, 0,									// GREEK SMALL LETTER GAMMA
	/* ??? */ 0x03B4,  33, 0,									// GREEK SMALL LETTER DELTA
	/* ??? */ 0x03B5,  19, 0,									// GREEK SMALL LETTER EPSILON
	/* ??? */ 0x03B6,  46, 0,									// GREEK SMALL LETTER ZETA
	/* ??? */ 0x03B7,  36, 0,									// GREEK SMALL LETTER ETA
	/* ??? */ 0x03B8,  23, 0,									// GREEK SMALL LETTER THETA
	/* ??? */ 0x03B9,  24, 0,									// GREEK SMALL LETTER IOTA
	/* ??? */ 0x03BA,  38, 0,									// GREEK SMALL LETTER KAPPA
	/* ??? */ 0x03BB,  39, 0,									// GREEK SMALL LETTER LAMDA
	/* ??? */ 0x03BC,  52, 0,									// GREEK SMALL LETTER MU
	/* ??? */ 0x03BD,  51, 0,									// GREEK SMALL LETTER NU
	/* ??? */ 0x03BE,  37, 0,									// GREEK SMALL LETTER XI
	/* ??? */ 0x03BF,  25, 0,									// GREEK SMALL LETTER OMICRON
	/* ??? */ 0x03C0,  26, 0,									// GREEK SMALL LETTER PI
	/* ??? */ 0x03C1,  20, 0,									// GREEK SMALL LETTER RHO
	/* ??? */ 0x03C2,  18, 0,									// GREEK SMALL LETTER FINAL SIGMA
	/* ??? */ 0x03C3,  32, 0,									// GREEK SMALL LETTER SIGMA
	/* ??? */ 0x03C4,  21, 0,									// GREEK SMALL LETTER TAU
	/* ??? */ 0x03C5,  22, 0,									// GREEK SMALL LETTER UPSILON
	/* ??? */ 0x03C6,  34, 0,									// GREEK SMALL LETTER PHI
	/* ??? */ 0x03C7,  47, 0,									// GREEK SMALL LETTER CHI
	/* ??? */ 0x03C8,  48, 0,									// GREEK SMALL LETTER PSI
	/* ??? */ 0x03C9,  49, 0,									// GREEK SMALL LETTER OMEGA
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Greek_319[147] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023, NOS, 0,
	/* '&' */ 0x0026, NOS, 0,
	/* ''' */ 0x0027,  12, 0,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  13, MO_ShiftLeft,
	/* '+' */ 0x002B,  13, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C, NOS, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E, NOS, 0,
	/* '?' */ 0x003F, NOS, 0,
	/* '@' */ 0x0040, NOS, 0,
	/* 'A' */ 0x0041, NOS, 0,
	/* 'B' */ 0x0042, NOS, 0,
	/* 'C' */ 0x0043, NOS, 0,
	/* 'D' */ 0x0044, NOS, 0,
	/* 'E' */ 0x0045, NOS, 0,
	/* 'F' */ 0x0046, NOS, 0,
	/* 'G' */ 0x0047, NOS, 0,
	/* 'H' */ 0x0048, NOS, 0,
	/* 'I' */ 0x0049, NOS, 0,
	/* 'J' */ 0x004A, NOS, 0,
	/* 'K' */ 0x004B, NOS, 0,
	/* 'L' */ 0x004C, NOS, 0,
	/* 'M' */ 0x004D, NOS, 0,
	/* 'N' */ 0x004E, NOS, 0,
	/* 'O' */ 0x004F, NOS, 0,
	/* 'P' */ 0x0050, NOS, 0,
	/* 'Q' */ 0x0051, NOS, 0,
	/* 'R' */ 0x0052, NOS, 0,
	/* 'S' */ 0x0053, NOS, 0,
	/* 'T' */ 0x0054, NOS, 0,
	/* 'U' */ 0x0055, NOS, 0,
	/* 'V' */ 0x0056, NOS, 0,
	/* 'W' */ 0x0057, NOS, 0,
	/* 'X' */ 0x0058, NOS, 0,
	/* 'Y' */ 0x0059, NOS, 0,
	/* 'Z' */ 0x005A, NOS, 0,
	/* '\' */ 0x005C, NOS, 0,
	/* '^' */ 0x005E, NOS, 0,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060, NOS, 0,
	/* 'a' */ 0x0061, NOS, 0,
	/* 'b' */ 0x0062, NOS, 0,
	/* 'c' */ 0x0063, NOS, 0,
	/* 'd' */ 0x0064, NOS, 0,
	/* 'e' */ 0x0065, NOS, 0,
	/* 'f' */ 0x0066, NOS, 0,
	/* 'g' */ 0x0067, NOS, 0,
	/* 'h' */ 0x0068, NOS, 0,
	/* 'i' */ 0x0069, NOS, 0,
	/* 'j' */ 0x006A, NOS, 0,
	/* 'k' */ 0x006B, NOS, 0,
	/* 'l' */ 0x006C, NOS, 0,
	/* 'm' */ 0x006D, NOS, 0,
	/* 'n' */ 0x006E, NOS, 0,
	/* 'o' */ 0x006F, NOS, 0,
	/* 'p' */ 0x0070, NOS, 0,
	/* 'q' */ 0x0071, NOS, 0,
	/* 'r' */ 0x0072, NOS, 0,
	/* 's' */ 0x0073, NOS, 0,
	/* 't' */ 0x0074, NOS, 0,
	/* 'u' */ 0x0075, NOS, 0,
	/* 'v' */ 0x0076, NOS, 0,
	/* 'w' */ 0x0077, NOS, 0,
	/* 'x' */ 0x0078, NOS, 0,
	/* 'y' */ 0x0079, NOS, 0,
	/* 'z' */ 0x007A, NOS, 0,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* '£' */ 0x00A3,   4, MO_ShiftLeft,
	/* '¦' */ 0x00A6,  18, MO_ShiftLeft,
	/* '§' */ 0x00A7,  45, 0,
	/* '¨' */ 0x00A8,  40, MO_ShiftLeft|MO_DeadKey,
	/* '©' */ 0x00A9,  45, MO_ShiftLeft,
	/* '«' */ 0x00AB,  27, MO_ShiftLeft,
	/* '¬' */ 0x00AC,   7, MO_ShiftLeft,
	/* '°' */ 0x00B0,  12, MO_ShiftLeft,
	/* '±' */ 0x00B1,   1, MO_ShiftLeft,
	/* '²' */ 0x00B2,  42, 0,
	/* '³' */ 0x00B3,  42, MO_ShiftLeft,
	/* '·' */ 0x00B7,  17, 0,
	/* '»' */ 0x00BB,  28, MO_ShiftLeft,
	/* '½' */ 0x00BD,   1, 0,
	/* ??? */ 0x0384,  40, MO_DeadKey,						// GREEK TONOS
	/* ??? */ 0x0385,  40, MO_AltRight|MO_DeadKey,		// GREEK DIALYTIKA TONOS
	/* ??? */ 0x0391,  31, MO_ShiftLeft,					// GREEK CAPITAL LETTER ALPHA
	/* ??? */ 0x0392,  50, MO_ShiftLeft,					// GREEK CAPITAL LETTER BETA
	/* ??? */ 0x0393,  35, MO_ShiftLeft,					// GREEK CAPITAL LETTER GAMMA
	/* ??? */ 0x0394,  33, MO_ShiftLeft,					// GREEK CAPITAL LETTER DELTA
	/* ??? */ 0x0395,  19, MO_ShiftLeft,					// GREEK CAPITAL LETTER EPSILON
	/* ??? */ 0x0396,  46, MO_ShiftLeft,					// GREEK CAPITAL LETTER ZETA
	/* ??? */ 0x0397,  36, MO_ShiftLeft,					// GREEK CAPITAL LETTER ETA
	/* ??? */ 0x0398,  23, MO_ShiftLeft,					// GREEK CAPITAL LETTER THETA
	/* ??? */ 0x0399,  24, MO_ShiftLeft,					// GREEK CAPITAL LETTER IOTA
	/* ??? */ 0x039A,  38, MO_ShiftLeft,					// GREEK CAPITAL LETTER KAPPA
	/* ??? */ 0x039B,  39, MO_ShiftLeft,					// GREEK CAPITAL LETTER LAMDA
	/* ??? */ 0x039C,  52, MO_ShiftLeft,					// GREEK CAPITAL LETTER MU
	/* ??? */ 0x039D,  51, MO_ShiftLeft,					// GREEK CAPITAL LETTER NU
	/* ??? */ 0x039E,  37, MO_ShiftLeft,					// GREEK CAPITAL LETTER XI
	/* ??? */ 0x039F,  25, MO_ShiftLeft,					// GREEK CAPITAL LETTER OMICRON
	/* ??? */ 0x03A0,  26, MO_ShiftLeft,					// GREEK CAPITAL LETTER PI
	/* ??? */ 0x03A1,  20, MO_ShiftLeft,					// GREEK CAPITAL LETTER RHO
	/* ??? */ 0x03A3,  32, MO_ShiftLeft,					// GREEK CAPITAL LETTER SIGMA
	/* ??? */ 0x03A4,  21, MO_ShiftLeft,					// GREEK CAPITAL LETTER TAU
	/* ??? */ 0x03A5,  22, MO_ShiftLeft,					// GREEK CAPITAL LETTER UPSILON
	/* ??? */ 0x03A6,  34, MO_ShiftLeft,					// GREEK CAPITAL LETTER PHI
	/* ??? */ 0x03A7,  47, MO_ShiftLeft,					// GREEK CAPITAL LETTER CHI
	/* ??? */ 0x03A8,  48, MO_ShiftLeft,					// GREEK CAPITAL LETTER PSI
	/* ??? */ 0x03A9,  49, MO_ShiftLeft,					// GREEK CAPITAL LETTER OMEGA
	/* ??? */ 0x03B1,  31, 0,									// GREEK SMALL LETTER ALPHA
	/* ??? */ 0x03B2,  50, 0,									// GREEK SMALL LETTER BETA
	/* ??? */ 0x03B3,  35, 0,									// GREEK SMALL LETTER GAMMA
	/* ??? */ 0x03B4,  33, 0,									// GREEK SMALL LETTER DELTA
	/* ??? */ 0x03B5,  19, 0,									// GREEK SMALL LETTER EPSILON
	/* ??? */ 0x03B6,  46, 0,									// GREEK SMALL LETTER ZETA
	/* ??? */ 0x03B7,  36, 0,									// GREEK SMALL LETTER ETA
	/* ??? */ 0x03B8,  23, 0,									// GREEK SMALL LETTER THETA
	/* ??? */ 0x03B9,  24, 0,									// GREEK SMALL LETTER IOTA
	/* ??? */ 0x03BA,  38, 0,									// GREEK SMALL LETTER KAPPA
	/* ??? */ 0x03BB,  39, 0,									// GREEK SMALL LETTER LAMDA
	/* ??? */ 0x03BC,  52, 0,									// GREEK SMALL LETTER MU
	/* ??? */ 0x03BD,  51, 0,									// GREEK SMALL LETTER NU
	/* ??? */ 0x03BE,  37, 0,									// GREEK SMALL LETTER XI
	/* ??? */ 0x03BF,  25, 0,									// GREEK SMALL LETTER OMICRON
	/* ??? */ 0x03C0,  26, 0,									// GREEK SMALL LETTER PI
	/* ??? */ 0x03C1,  20, 0,									// GREEK SMALL LETTER RHO
	/* ??? */ 0x03C2,  18, 0,									// GREEK SMALL LETTER FINAL SIGMA
	/* ??? */ 0x03C3,  32, 0,									// GREEK SMALL LETTER SIGMA
	/* ??? */ 0x03C4,  21, 0,									// GREEK SMALL LETTER TAU
	/* ??? */ 0x03C5,  22, 0,									// GREEK SMALL LETTER UPSILON
	/* ??? */ 0x03C6,  34, 0,									// GREEK SMALL LETTER PHI
	/* ??? */ 0x03C7,  47, 0,									// GREEK SMALL LETTER CHI
	/* ??? */ 0x03C8,  48, 0,									// GREEK SMALL LETTER PSI
	/* ??? */ 0x03C9,  49, 0,									// GREEK SMALL LETTER OMEGA
	/* ??? */ 0x2015,  17, MO_ShiftLeft,					// HORIZONTAL BAR
	/* '‘' */ 0x2018,  41, MO_ShiftLeft,
	/* '’' */ 0x2019,  41, 0,
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Greek_Latin[66] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '\' */ 0x005C,  42, 0,
	/* '^' */ 0x005E,   7, MO_ShiftLeft|MO_DeadKey,
	/* '`' */ 0x0060,   1, MO_DeadKey,
	/* '|' */ 0x007C,  42, MO_ShiftLeft,
	/* '~' */ 0x007E,   1, MO_ShiftLeft|MO_DeadKey,
	/* '¡' */ 0x00A1,   2, MO_AltRight,
	/* '¢' */ 0x00A2,  48, MO_ShiftLeft|MO_AltRight,
	/* '£' */ 0x00A3,   5, MO_ShiftLeft|MO_AltRight,
	/* '¤' */ 0x00A4,   5, MO_AltRight,
	/* '¥' */ 0x00A5,  12, MO_AltRight,
	/* '¦' */ 0x00A6,  42, MO_ShiftLeft|MO_AltRight,
	/* '§' */ 0x00A7,  32, MO_ShiftLeft|MO_AltRight,
	/* '¨' */ 0x00A8,  41, MO_ShiftLeft|MO_AltRight|MO_DeadKey,
	/* '©' */ 0x00A9,  48, MO_AltRight,
	/* '«' */ 0x00AB,  27, MO_AltRight,
	/* '¬' */ 0x00AC,  42, MO_AltRight,
	/* '®' */ 0x00AE,  20, MO_AltRight,
	/* '°' */ 0x00B0,  40, MO_ShiftLeft|MO_AltRight,
	/* '²' */ 0x00B2,   3, MO_AltRight,
	/* '³' */ 0x00B3,   4, MO_AltRight,
	/* '´' */ 0x00B4,  41, MO_AltRight|MO_DeadKey,
	/* 'µ' */ 0x00B5,  52, MO_AltRight,
	/* '¶' */ 0x00B6,  40, MO_AltRight,
	/* '¹' */ 0x00B9,   2, MO_ShiftLeft|MO_AltRight,
	/* '»' */ 0x00BB,  28, MO_AltRight,
	/* '¼' */ 0x00BC,   7, MO_AltRight,
	/* '½' */ 0x00BD,   8, MO_AltRight,
	/* '¾' */ 0x00BE,   9, MO_AltRight,
	/* '¿' */ 0x00BF,  55, MO_AltRight,
	/* 'Á' */ 0x00C1,  31, MO_ShiftLeft|MO_AltRight,
	/* 'Ä' */ 0x00C4,  17, MO_ShiftLeft|MO_AltRight,
	/* 'Å' */ 0x00C5,  18, MO_ShiftLeft|MO_AltRight,
	/* 'Æ' */ 0x00C6,  46, MO_ShiftLeft|MO_AltRight,
	/* 'Ç' */ 0x00C7,  53, MO_ShiftLeft|MO_AltRight,
	/* 'É' */ 0x00C9,  19, MO_ShiftLeft|MO_AltRight,
	/* 'Í' */ 0x00CD,  24, MO_ShiftLeft|MO_AltRight,
	/* 'Ð' */ 0x00D0,  33, MO_ShiftLeft|MO_AltRight,
	/* 'Ñ' */ 0x00D1,  51, MO_ShiftLeft|MO_AltRight,
	/* 'Ó' */ 0x00D3,  25, MO_ShiftLeft|MO_AltRight,
	/* 'Ö' */ 0x00D6,  26, MO_ShiftLeft|MO_AltRight,
	/* '×' */ 0x00D7,  13, MO_AltRight,
	/* 'Ø' */ 0x00D8,  39, MO_ShiftLeft|MO_AltRight,
	/* 'Ú' */ 0x00DA,  23, MO_ShiftLeft|MO_AltRight,
	/* 'Ü' */ 0x00DC,  22, MO_ShiftLeft|MO_AltRight,
	/* 'Þ' */ 0x00DE,  21, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0x00DF,  32, MO_AltRight,
	/* 'á' */ 0x00E1,  31, MO_AltRight,
	/* 'ä' */ 0x00E4,  17, MO_AltRight,
	/* 'å' */ 0x00E5,  18, MO_AltRight,
	/* 'æ' */ 0x00E6,  46, MO_AltRight,
	/* 'ç' */ 0x00E7,  53, MO_AltRight,
	/* 'é' */ 0x00E9,  19, MO_AltRight,
	/* 'í' */ 0x00ED,  24, MO_AltRight,
	/* 'ð' */ 0x00F0,  33, MO_AltRight,
	/* 'ñ' */ 0x00F1,  51, MO_AltRight,
	/* 'ó' */ 0x00F3,  25, MO_AltRight,
	/* 'ö' */ 0x00F6,  26, MO_AltRight,
	/* '÷' */ 0x00F7,  13, MO_ShiftLeft|MO_AltRight,
	/* 'ø' */ 0x00F8,  39, MO_AltRight,
	/* 'ú' */ 0x00FA,  23, MO_AltRight,
	/* 'ü' */ 0x00FC,  22, MO_AltRight,
	/* 'þ' */ 0x00FE,  21, MO_AltRight,
	/* '‘' */ 0x2018,  10, MO_AltRight,
	/* '’' */ 0x2019,  11, MO_AltRight,
	/* '€' */ 0x20AC,   6, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Greek_220_Latin[44] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,  12, 0,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  27, MO_ShiftLeft,
	/* '+' */ 0x002B,  27, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,  42, MO_ShiftLeft,
	/* '[' */ 0x005B,  13, MO_ShiftLeft,
	/* '\' */ 0x005C,   1, 0,
	/* ']' */ 0x005D,  13, 0,
	/* '^' */ 0x005E, NOS, 0,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060, NOS, 0,
	/* '{' */ 0x007B,  28, MO_ShiftLeft,
	/* '|' */ 0x007C,   1, MO_ShiftLeft,
	/* '}' */ 0x007D,  28, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* '£' */ 0x00A3,   5, MO_AltRight,
	/* '¤' */ 0x00A4,   9, MO_AltRight,
	/* '¦' */ 0x00A6,  10, MO_AltRight,
	/* '§' */ 0x00A7,   6, MO_AltRight,
	/* '¨' */ 0x00A8,  40, MO_ShiftLeft|MO_DeadKey,
	/* '«' */ 0x00AB,  27, MO_AltRight,
	/* '¬' */ 0x00AC,  42, MO_AltRight,
	/* '°' */ 0x00B0,  11, MO_AltRight,
	/* '±' */ 0x00B1,  12, MO_AltRight,
	/* '²' */ 0x00B2,   3, MO_AltRight,
	/* '³' */ 0x00B3,   4, MO_AltRight,
	/* '¶' */ 0x00B6,   7, MO_AltRight,
	/* '»' */ 0x00BB,  28, MO_AltRight,
	/* '½' */ 0x00BD,  13, MO_AltRight,
	/* ??? */ 0x0384,  40, MO_DeadKey,						// GREEK TONOS
	/* ??? */ 0x0385,  40, MO_AltRight|MO_DeadKey,		// GREEK DIALYTIKA TONOS
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Greek_319_Latin[25] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,  12, 0,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  13, MO_ShiftLeft,
	/* '+' */ 0x002B,  13, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,  42, MO_ShiftLeft,
	/* '\' */ 0x005C,   1, 0,
	/* '^' */ 0x005E,  41, MO_DeadKey,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,  42, MO_DeadKey,
	/* '|' */ 0x007C,   1, MO_ShiftLeft,
	/* '~' */ 0x007E,  41, MO_ShiftLeft|MO_DeadKey,
	/* '¨' */ 0x00A8,  40, MO_ShiftLeft|MO_DeadKey,
	/* '´' */ 0x00B4,  40, MO_DeadKey,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Greek_MS[128] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* ':' */ 0x003A,  17, MO_ShiftLeft,
	/* ';' */ 0x003B,  17, 0,
	/* '<' */ 0x003C,  45, 0,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* 'A' */ 0x0041, NOS, 0,
	/* 'B' */ 0x0042, NOS, 0,
	/* 'C' */ 0x0043, NOS, 0,
	/* 'D' */ 0x0044, NOS, 0,
	/* 'E' */ 0x0045, NOS, 0,
	/* 'F' */ 0x0046, NOS, 0,
	/* 'G' */ 0x0047, NOS, 0,
	/* 'H' */ 0x0048, NOS, 0,
	/* 'I' */ 0x0049, NOS, 0,
	/* 'J' */ 0x004A, NOS, 0,
	/* 'K' */ 0x004B, NOS, 0,
	/* 'L' */ 0x004C, NOS, 0,
	/* 'M' */ 0x004D, NOS, 0,
	/* 'N' */ 0x004E, NOS, 0,
	/* 'O' */ 0x004F, NOS, 0,
	/* 'P' */ 0x0050, NOS, 0,
	/* 'Q' */ 0x0051, NOS, 0,
	/* 'R' */ 0x0052, NOS, 0,
	/* 'S' */ 0x0053, NOS, 0,
	/* 'T' */ 0x0054, NOS, 0,
	/* 'U' */ 0x0055, NOS, 0,
	/* 'V' */ 0x0056, NOS, 0,
	/* 'W' */ 0x0057, NOS, 0,
	/* 'X' */ 0x0058, NOS, 0,
	/* 'Y' */ 0x0059, NOS, 0,
	/* 'Z' */ 0x005A, NOS, 0,
	/* '\' */ 0x005C,  42, 0,
	/* 'a' */ 0x0061, NOS, 0,
	/* 'b' */ 0x0062, NOS, 0,
	/* 'c' */ 0x0063, NOS, 0,
	/* 'd' */ 0x0064, NOS, 0,
	/* 'e' */ 0x0065, NOS, 0,
	/* 'f' */ 0x0066, NOS, 0,
	/* 'g' */ 0x0067, NOS, 0,
	/* 'h' */ 0x0068, NOS, 0,
	/* 'i' */ 0x0069, NOS, 0,
	/* 'j' */ 0x006A, NOS, 0,
	/* 'k' */ 0x006B, NOS, 0,
	/* 'l' */ 0x006C, NOS, 0,
	/* 'm' */ 0x006D, NOS, 0,
	/* 'n' */ 0x006E, NOS, 0,
	/* 'o' */ 0x006F, NOS, 0,
	/* 'p' */ 0x0070, NOS, 0,
	/* 'q' */ 0x0071, NOS, 0,
	/* 'r' */ 0x0072, NOS, 0,
	/* 's' */ 0x0073, NOS, 0,
	/* 't' */ 0x0074, NOS, 0,
	/* 'u' */ 0x0075, NOS, 0,
	/* 'v' */ 0x0076, NOS, 0,
	/* 'w' */ 0x0077, NOS, 0,
	/* 'x' */ 0x0078, NOS, 0,
	/* 'y' */ 0x0079, NOS, 0,
	/* 'z' */ 0x007A, NOS, 0,
	/* '|' */ 0x007C,  42, MO_ShiftLeft,
	/* '£' */ 0x00A3,   5, MO_AltRight,
	/* '¤' */ 0x00A4,   9, MO_AltRight,
	/* '¥' */ 0x00A5,  22, MO_AltRight,
	/* '¦' */ 0x00A6,  10, MO_AltRight,
	/* '§' */ 0x00A7,   6, MO_AltRight,
	/* '¨' */ 0x00A8,  40, MO_ShiftLeft|MO_DeadKey,
	/* '©' */ 0x00A9,  48, MO_AltRight,
	/* '«' */ 0x00AB,  27, MO_AltRight,
	/* '¬' */ 0x00AC,  42, MO_AltRight,
	/* '®' */ 0x00AE,  20, MO_AltRight,
	/* '°' */ 0x00B0,  11, MO_AltRight,
	/* '±' */ 0x00B1,  12, MO_AltRight,
	/* '²' */ 0x00B2,   3, MO_AltRight,
	/* '³' */ 0x00B3,   4, MO_AltRight,
	/* '¶' */ 0x00B6,   7, MO_AltRight,
	/* '»' */ 0x00BB,  28, MO_AltRight,
	/* '½' */ 0x00BD,  13, MO_AltRight,
	/* ??? */ 0x0384,  40, MO_DeadKey,						// GREEK TONOS
	/* ??? */ 0x0385,  18, MO_ShiftLeft|MO_DeadKey,		// GREEK DIALYTIKA TONOS
	/* ??? */ 0x0391,  31, MO_ShiftLeft,					// GREEK CAPITAL LETTER ALPHA
	/* ??? */ 0x0392,  50, MO_ShiftLeft,					// GREEK CAPITAL LETTER BETA
	/* ??? */ 0x0393,  35, MO_ShiftLeft,					// GREEK CAPITAL LETTER GAMMA
	/* ??? */ 0x0394,  33, MO_ShiftLeft,					// GREEK CAPITAL LETTER DELTA
	/* ??? */ 0x0395,  19, MO_ShiftLeft,					// GREEK CAPITAL LETTER EPSILON
	/* ??? */ 0x0396,  46, MO_ShiftLeft,					// GREEK CAPITAL LETTER ZETA
	/* ??? */ 0x0397,  36, MO_ShiftLeft,					// GREEK CAPITAL LETTER ETA
	/* ??? */ 0x0398,  23, MO_ShiftLeft,					// GREEK CAPITAL LETTER THETA
	/* ??? */ 0x0399,  24, MO_ShiftLeft,					// GREEK CAPITAL LETTER IOTA
	/* ??? */ 0x039A,  38, MO_ShiftLeft,					// GREEK CAPITAL LETTER KAPPA
	/* ??? */ 0x039B,  39, MO_ShiftLeft,					// GREEK CAPITAL LETTER LAMDA
	/* ??? */ 0x039C,  52, MO_ShiftLeft,					// GREEK CAPITAL LETTER MU
	/* ??? */ 0x039D,  51, MO_ShiftLeft,					// GREEK CAPITAL LETTER NU
	/* ??? */ 0x039E,  37, MO_ShiftLeft,					// GREEK CAPITAL LETTER XI
	/* ??? */ 0x039F,  25, MO_ShiftLeft,					// GREEK CAPITAL LETTER OMICRON
	/* ??? */ 0x03A0,  26, MO_ShiftLeft,					// GREEK CAPITAL LETTER PI
	/* ??? */ 0x03A1,  20, MO_ShiftLeft,					// GREEK CAPITAL LETTER RHO
	/* ??? */ 0x03A3,  32, MO_ShiftLeft,					// GREEK CAPITAL LETTER SIGMA
	/* ??? */ 0x03A4,  21, MO_ShiftLeft,					// GREEK CAPITAL LETTER TAU
	/* ??? */ 0x03A5,  22, MO_ShiftLeft,					// GREEK CAPITAL LETTER UPSILON
	/* ??? */ 0x03A6,  34, MO_ShiftLeft,					// GREEK CAPITAL LETTER PHI
	/* ??? */ 0x03A7,  47, MO_ShiftLeft,					// GREEK CAPITAL LETTER CHI
	/* ??? */ 0x03A8,  48, MO_ShiftLeft,					// GREEK CAPITAL LETTER PSI
	/* ??? */ 0x03A9,  49, MO_ShiftLeft,					// GREEK CAPITAL LETTER OMEGA
	/* ??? */ 0x03B1,  31, 0,									// GREEK SMALL LETTER ALPHA
	/* ??? */ 0x03B2,  50, 0,									// GREEK SMALL LETTER BETA
	/* ??? */ 0x03B3,  35, 0,									// GREEK SMALL LETTER GAMMA
	/* ??? */ 0x03B4,  33, 0,									// GREEK SMALL LETTER DELTA
	/* ??? */ 0x03B5,  19, 0,									// GREEK SMALL LETTER EPSILON
	/* ??? */ 0x03B6,  46, 0,									// GREEK SMALL LETTER ZETA
	/* ??? */ 0x03B7,  36, 0,									// GREEK SMALL LETTER ETA
	/* ??? */ 0x03B8,  23, 0,									// GREEK SMALL LETTER THETA
	/* ??? */ 0x03B9,  24, 0,									// GREEK SMALL LETTER IOTA
	/* ??? */ 0x03BA,  38, 0,									// GREEK SMALL LETTER KAPPA
	/* ??? */ 0x03BB,  39, 0,									// GREEK SMALL LETTER LAMDA
	/* ??? */ 0x03BC,  52, 0,									// GREEK SMALL LETTER MU
	/* ??? */ 0x03BD,  51, 0,									// GREEK SMALL LETTER NU
	/* ??? */ 0x03BE,  37, 0,									// GREEK SMALL LETTER XI
	/* ??? */ 0x03BF,  25, 0,									// GREEK SMALL LETTER OMICRON
	/* ??? */ 0x03C0,  26, 0,									// GREEK SMALL LETTER PI
	/* ??? */ 0x03C1,  20, 0,									// GREEK SMALL LETTER RHO
	/* ??? */ 0x03C2,  18, 0,									// GREEK SMALL LETTER FINAL SIGMA
	/* ??? */ 0x03C3,  32, 0,									// GREEK SMALL LETTER SIGMA
	/* ??? */ 0x03C4,  21, 0,									// GREEK SMALL LETTER TAU
	/* ??? */ 0x03C5,  22, 0,									// GREEK SMALL LETTER UPSILON
	/* ??? */ 0x03C6,  34, 0,									// GREEK SMALL LETTER PHI
	/* ??? */ 0x03C7,  47, 0,									// GREEK SMALL LETTER CHI
	/* ??? */ 0x03C8,  48, 0,									// GREEK SMALL LETTER PSI
	/* ??? */ 0x03C9,  49, 0,									// GREEK SMALL LETTER OMEGA
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Russian_MS[142] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023, NOS, 0,
	/* '$' */ 0x0024, NOS, 0,
	/* '&' */ 0x0026, NOS, 0,
	/* ''' */ 0x0027, NOS, 0,
	/* ',' */ 0x002C,  55, MO_ShiftLeft,
	/* '.' */ 0x002E,  55, 0,
	/* '/' */ 0x002F,  29, MO_ShiftLeft,
	/* ':' */ 0x003A,   7, MO_ShiftLeft,
	/* ';' */ 0x003B,   5, MO_ShiftLeft,
	/* '<' */ 0x003C, NOS, 0,
	/* '>' */ 0x003E, NOS, 0,
	/* '?' */ 0x003F,   8, MO_ShiftLeft,
	/* '@' */ 0x0040, NOS, 0,
	/* 'A' */ 0x0041, NOS, 0,
	/* 'B' */ 0x0042, NOS, 0,
	/* 'C' */ 0x0043, NOS, 0,
	/* 'D' */ 0x0044, NOS, 0,
	/* 'E' */ 0x0045, NOS, 0,
	/* 'F' */ 0x0046, NOS, 0,
	/* 'G' */ 0x0047, NOS, 0,
	/* 'H' */ 0x0048, NOS, 0,
	/* 'I' */ 0x0049, NOS, 0,
	/* 'J' */ 0x004A, NOS, 0,
	/* 'K' */ 0x004B, NOS, 0,
	/* 'L' */ 0x004C, NOS, 0,
	/* 'M' */ 0x004D, NOS, 0,
	/* 'N' */ 0x004E, NOS, 0,
	/* 'O' */ 0x004F, NOS, 0,
	/* 'P' */ 0x0050, NOS, 0,
	/* 'Q' */ 0x0051, NOS, 0,
	/* 'R' */ 0x0052, NOS, 0,
	/* 'S' */ 0x0053, NOS, 0,
	/* 'T' */ 0x0054, NOS, 0,
	/* 'U' */ 0x0055, NOS, 0,
	/* 'V' */ 0x0056, NOS, 0,
	/* 'W' */ 0x0057, NOS, 0,
	/* 'X' */ 0x0058, NOS, 0,
	/* 'Y' */ 0x0059, NOS, 0,
	/* 'Z' */ 0x005A, NOS, 0,
	/* '[' */ 0x005B, NOS, 0,
	/* ']' */ 0x005D, NOS, 0,
	/* '^' */ 0x005E, NOS, 0,
	/* '`' */ 0x0060, NOS, 0,
	/* 'a' */ 0x0061, NOS, 0,
	/* 'b' */ 0x0062, NOS, 0,
	/* 'c' */ 0x0063, NOS, 0,
	/* 'd' */ 0x0064, NOS, 0,
	/* 'e' */ 0x0065, NOS, 0,
	/* 'f' */ 0x0066, NOS, 0,
	/* 'g' */ 0x0067, NOS, 0,
	/* 'h' */ 0x0068, NOS, 0,
	/* 'i' */ 0x0069, NOS, 0,
	/* 'j' */ 0x006A, NOS, 0,
	/* 'k' */ 0x006B, NOS, 0,
	/* 'l' */ 0x006C, NOS, 0,
	/* 'm' */ 0x006D, NOS, 0,
	/* 'n' */ 0x006E, NOS, 0,
	/* 'o' */ 0x006F, NOS, 0,
	/* 'p' */ 0x0070, NOS, 0,
	/* 'q' */ 0x0071, NOS, 0,
	/* 'r' */ 0x0072, NOS, 0,
	/* 's' */ 0x0073, NOS, 0,
	/* 't' */ 0x0074, NOS, 0,
	/* 'u' */ 0x0075, NOS, 0,
	/* 'v' */ 0x0076, NOS, 0,
	/* 'w' */ 0x0077, NOS, 0,
	/* 'x' */ 0x0078, NOS, 0,
	/* 'y' */ 0x0079, NOS, 0,
	/* 'z' */ 0x007A, NOS, 0,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* ??? */ 0x0401,   1, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER IO
	/* ??? */ 0x0410,  34, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER A
	/* ??? */ 0x0411,  53, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER BE
	/* ??? */ 0x0412,  33, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER VE
	/* ??? */ 0x0413,  23, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER GHE
	/* ??? */ 0x0414,  39, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER DE
	/* ??? */ 0x0415,  21, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER IE
	/* ??? */ 0x0416,  40, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZHE
	/* ??? */ 0x0417,  26, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZE
	/* ??? */ 0x0418,  50, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER I
	/* ??? */ 0x0419,  17, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHORT I
	/* ??? */ 0x041A,  20, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER KA
	/* ??? */ 0x041B,  38, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EL
	/* ??? */ 0x041C,  49, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EM
	/* ??? */ 0x041D,  22, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EN
	/* ??? */ 0x041E,  37, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER O
	/* ??? */ 0x041F,  35, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER PE
	/* ??? */ 0x0420,  36, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ER
	/* ??? */ 0x0421,  48, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ES
	/* ??? */ 0x0422,  51, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TE
	/* ??? */ 0x0423,  19, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER U
	/* ??? */ 0x0424,  31, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EF
	/* ??? */ 0x0425,  27, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER HA
	/* ??? */ 0x0426,  18, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TSE
	/* ??? */ 0x0427,  47, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER CHE
	/* ??? */ 0x0428,  24, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHA
	/* ??? */ 0x0429,  25, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHCHA
	/* ??? */ 0x042A,  28, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER HARD SIGN
	/* ??? */ 0x042B,  32, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YERU
	/* ??? */ 0x042C,  52, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SOFT SIGN
	/* ??? */ 0x042D,  41, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER E
	/* ??? */ 0x042E,  54, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YU
	/* ??? */ 0x042F,  46, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YA
	/* ??? */ 0x0430,  34, 0,									// CYRILLIC SMALL LETTER A
	/* ??? */ 0x0431,  53, 0,									// CYRILLIC SMALL LETTER BE
	/* ??? */ 0x0432,  33, 0,									// CYRILLIC SMALL LETTER VE
	/* ??? */ 0x0433,  23, 0,									// CYRILLIC SMALL LETTER GHE
	/* ??? */ 0x0434,  39, 0,									// CYRILLIC SMALL LETTER DE
	/* ??? */ 0x0435,  21, 0,									// CYRILLIC SMALL LETTER IE
	/* ??? */ 0x0436,  40, 0,									// CYRILLIC SMALL LETTER ZHE
	/* ??? */ 0x0437,  26, 0,									// CYRILLIC SMALL LETTER ZE
	/* ??? */ 0x0438,  50, 0,									// CYRILLIC SMALL LETTER I
	/* ??? */ 0x0439,  17, 0,									// CYRILLIC SMALL LETTER SHORT I
	/* ??? */ 0x043A,  20, 0,									// CYRILLIC SMALL LETTER KA
	/* ??? */ 0x043B,  38, 0,									// CYRILLIC SMALL LETTER EL
	/* ??? */ 0x043C,  49, 0,									// CYRILLIC SMALL LETTER EM
	/* ??? */ 0x043D,  22, 0,									// CYRILLIC SMALL LETTER EN
	/* ??? */ 0x043E,  37, 0,									// CYRILLIC SMALL LETTER O
	/* ??? */ 0x043F,  35, 0,									// CYRILLIC SMALL LETTER PE
	/* ??? */ 0x0440,  36, 0,									// CYRILLIC SMALL LETTER ER
	/* ??? */ 0x0441,  48, 0,									// CYRILLIC SMALL LETTER ES
	/* ??? */ 0x0442,  51, 0,									// CYRILLIC SMALL LETTER TE
	/* ??? */ 0x0443,  19, 0,									// CYRILLIC SMALL LETTER U
	/* ??? */ 0x0444,  31, 0,									// CYRILLIC SMALL LETTER EF
	/* ??? */ 0x0445,  27, 0,									// CYRILLIC SMALL LETTER HA
	/* ??? */ 0x0446,  18, 0,									// CYRILLIC SMALL LETTER TSE
	/* ??? */ 0x0447,  47, 0,									// CYRILLIC SMALL LETTER CHE
	/* ??? */ 0x0448,  24, 0,									// CYRILLIC SMALL LETTER SHA
	/* ??? */ 0x0449,  25, 0,									// CYRILLIC SMALL LETTER SHCHA
	/* ??? */ 0x044A,  28, 0,									// CYRILLIC SMALL LETTER HARD SIGN
	/* ??? */ 0x044B,  32, 0,									// CYRILLIC SMALL LETTER YERU
	/* ??? */ 0x044C,  52, 0,									// CYRILLIC SMALL LETTER SOFT SIGN
	/* ??? */ 0x044D,  41, 0,									// CYRILLIC SMALL LETTER E
	/* ??? */ 0x044E,  54, 0,									// CYRILLIC SMALL LETTER YU
	/* ??? */ 0x044F,  46, 0,									// CYRILLIC SMALL LETTER YA
	/* ??? */ 0x0451,   1, 0,									// CYRILLIC SMALL LETTER IO
	/* ??? */ 0x2116,   4, MO_ShiftLeft,					// NUMERO SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Russian_Typewriter[162] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,  12, 0,
	/* '"' */ 0x0022,   5, 0,
	/* '#' */ 0x0023, NOS, 0,
	/* '$' */ 0x0024, NOS, 0,
	/* '%' */ 0x0025,  11, 0,
	/* '&' */ 0x0026, NOS, 0,
	/* ''' */ 0x0027, NOS, 0,
	/* '(' */ 0x0028,  42, MO_ShiftLeft,
	/* ')' */ 0x0029,  42, 0,
	/* '*' */ 0x002A, NOS, 0,
	/* '+' */ 0x002B,   1, MO_ShiftLeft,
	/* ',' */ 0x002C,   7, 0,
	/* '-' */ 0x002D,   3, 0,
	/* '.' */ 0x002E,   8, 0,
	/* '/' */ 0x002F,   4, 0,
	/* '0' */ 0x0030,  11, MO_ShiftLeft,
	/* '1' */ 0x0031,   2, MO_ShiftLeft,
	/* '2' */ 0x0032,   3, MO_ShiftLeft,
	/* '3' */ 0x0033,   4, MO_ShiftLeft,
	/* '4' */ 0x0034,   5, MO_ShiftLeft,
	/* '5' */ 0x0035,   6, MO_ShiftLeft,
	/* '6' */ 0x0036,   7, MO_ShiftLeft,
	/* '7' */ 0x0037,   8, MO_ShiftLeft,
	/* '8' */ 0x0038,   9, MO_ShiftLeft,
	/* '9' */ 0x0039,  10, MO_ShiftLeft,
	/* ':' */ 0x003A,   6, 0,
	/* ';' */ 0x003B,  13, 0,
	/* '<' */ 0x003C, NOS, 0,
	/* '=' */ 0x003D,  12, MO_ShiftLeft,
	/* '>' */ 0x003E, NOS, 0,
	/* '?' */ 0x003F,  10, 0,
	/* '@' */ 0x0040, NOS, 0,
	/* 'A' */ 0x0041, NOS, 0,
	/* 'B' */ 0x0042, NOS, 0,
	/* 'C' */ 0x0043, NOS, 0,
	/* 'D' */ 0x0044, NOS, 0,
	/* 'E' */ 0x0045, NOS, 0,
	/* 'F' */ 0x0046, NOS, 0,
	/* 'G' */ 0x0047, NOS, 0,
	/* 'H' */ 0x0048, NOS, 0,
	/* 'I' */ 0x0049, NOS, 0,
	/* 'J' */ 0x004A, NOS, 0,
	/* 'K' */ 0x004B, NOS, 0,
	/* 'L' */ 0x004C, NOS, 0,
	/* 'M' */ 0x004D, NOS, 0,
	/* 'N' */ 0x004E, NOS, 0,
	/* 'O' */ 0x004F, NOS, 0,
	/* 'P' */ 0x0050, NOS, 0,
	/* 'Q' */ 0x0051, NOS, 0,
	/* 'R' */ 0x0052, NOS, 0,
	/* 'S' */ 0x0053, NOS, 0,
	/* 'T' */ 0x0054, NOS, 0,
	/* 'U' */ 0x0055, NOS, 0,
	/* 'V' */ 0x0056, NOS, 0,
	/* 'W' */ 0x0057, NOS, 0,
	/* 'X' */ 0x0058, NOS, 0,
	/* 'Y' */ 0x0059, NOS, 0,
	/* 'Z' */ 0x005A, NOS, 0,
	/* '[' */ 0x005B, NOS, 0,
	/* '\' */ 0x005C,  13, MO_ShiftLeft,
	/* ']' */ 0x005D, NOS, 0,
	/* '^' */ 0x005E, NOS, 0,
	/* '_' */ 0x005F,   9, 0,
	/* '`' */ 0x0060, NOS, 0,
	/* 'a' */ 0x0061, NOS, 0,
	/* 'b' */ 0x0062, NOS, 0,
	/* 'c' */ 0x0063, NOS, 0,
	/* 'd' */ 0x0064, NOS, 0,
	/* 'e' */ 0x0065, NOS, 0,
	/* 'f' */ 0x0066, NOS, 0,
	/* 'g' */ 0x0067, NOS, 0,
	/* 'h' */ 0x0068, NOS, 0,
	/* 'i' */ 0x0069, NOS, 0,
	/* 'j' */ 0x006A, NOS, 0,
	/* 'k' */ 0x006B, NOS, 0,
	/* 'l' */ 0x006C, NOS, 0,
	/* 'm' */ 0x006D, NOS, 0,
	/* 'n' */ 0x006E, NOS, 0,
	/* 'o' */ 0x006F, NOS, 0,
	/* 'p' */ 0x0070, NOS, 0,
	/* 'q' */ 0x0071, NOS, 0,
	/* 'r' */ 0x0072, NOS, 0,
	/* 's' */ 0x0073, NOS, 0,
	/* 't' */ 0x0074, NOS, 0,
	/* 'u' */ 0x0075, NOS, 0,
	/* 'v' */ 0x0076, NOS, 0,
	/* 'w' */ 0x0077, NOS, 0,
	/* 'x' */ 0x0078, NOS, 0,
	/* 'y' */ 0x0079, NOS, 0,
	/* 'z' */ 0x007A, NOS, 0,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C,   1, 0,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* ??? */ 0x0401,  55, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER IO
	/* ??? */ 0x0410,  34, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER A
	/* ??? */ 0x0411,  53, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER BE
	/* ??? */ 0x0412,  33, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER VE
	/* ??? */ 0x0413,  23, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER GHE
	/* ??? */ 0x0414,  39, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER DE
	/* ??? */ 0x0415,  21, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER IE
	/* ??? */ 0x0416,  40, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZHE
	/* ??? */ 0x0417,  26, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZE
	/* ??? */ 0x0418,  50, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER I
	/* ??? */ 0x0419,  17, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHORT I
	/* ??? */ 0x041A,  20, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER KA
	/* ??? */ 0x041B,  38, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EL
	/* ??? */ 0x041C,  49, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EM
	/* ??? */ 0x041D,  22, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EN
	/* ??? */ 0x041E,  37, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER O
	/* ??? */ 0x041F,  35, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER PE
	/* ??? */ 0x0420,  36, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ER
	/* ??? */ 0x0421,  48, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ES
	/* ??? */ 0x0422,  51, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TE
	/* ??? */ 0x0423,  19, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER U
	/* ??? */ 0x0424,  31, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EF
	/* ??? */ 0x0425,  27, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER HA
	/* ??? */ 0x0426,  18, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TSE
	/* ??? */ 0x0427,  47, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER CHE
	/* ??? */ 0x0428,  24, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHA
	/* ??? */ 0x0429,  25, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHCHA
	/* ??? */ 0x042A,  28, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER HARD SIGN
	/* ??? */ 0x042B,  32, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YERU
	/* ??? */ 0x042C,  52, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SOFT SIGN
	/* ??? */ 0x042D,  41, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER E
	/* ??? */ 0x042E,  54, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YU
	/* ??? */ 0x042F,  46, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YA
	/* ??? */ 0x0430,  34, 0,									// CYRILLIC SMALL LETTER A
	/* ??? */ 0x0431,  53, 0,									// CYRILLIC SMALL LETTER BE
	/* ??? */ 0x0432,  33, 0,									// CYRILLIC SMALL LETTER VE
	/* ??? */ 0x0433,  23, 0,									// CYRILLIC SMALL LETTER GHE
	/* ??? */ 0x0434,  39, 0,									// CYRILLIC SMALL LETTER DE
	/* ??? */ 0x0435,  21, 0,									// CYRILLIC SMALL LETTER IE
	/* ??? */ 0x0436,  40, 0,									// CYRILLIC SMALL LETTER ZHE
	/* ??? */ 0x0437,  26, 0,									// CYRILLIC SMALL LETTER ZE
	/* ??? */ 0x0438,  50, 0,									// CYRILLIC SMALL LETTER I
	/* ??? */ 0x0439,  17, 0,									// CYRILLIC SMALL LETTER SHORT I
	/* ??? */ 0x043A,  20, 0,									// CYRILLIC SMALL LETTER KA
	/* ??? */ 0x043B,  38, 0,									// CYRILLIC SMALL LETTER EL
	/* ??? */ 0x043C,  49, 0,									// CYRILLIC SMALL LETTER EM
	/* ??? */ 0x043D,  22, 0,									// CYRILLIC SMALL LETTER EN
	/* ??? */ 0x043E,  37, 0,									// CYRILLIC SMALL LETTER O
	/* ??? */ 0x043F,  35, 0,									// CYRILLIC SMALL LETTER PE
	/* ??? */ 0x0440,  36, 0,									// CYRILLIC SMALL LETTER ER
	/* ??? */ 0x0441,  48, 0,									// CYRILLIC SMALL LETTER ES
	/* ??? */ 0x0442,  51, 0,									// CYRILLIC SMALL LETTER TE
	/* ??? */ 0x0443,  19, 0,									// CYRILLIC SMALL LETTER U
	/* ??? */ 0x0444,  31, 0,									// CYRILLIC SMALL LETTER EF
	/* ??? */ 0x0445,  27, 0,									// CYRILLIC SMALL LETTER HA
	/* ??? */ 0x0446,  18, 0,									// CYRILLIC SMALL LETTER TSE
	/* ??? */ 0x0447,  47, 0,									// CYRILLIC SMALL LETTER CHE
	/* ??? */ 0x0448,  24, 0,									// CYRILLIC SMALL LETTER SHA
	/* ??? */ 0x0449,  25, 0,									// CYRILLIC SMALL LETTER SHCHA
	/* ??? */ 0x044A,  28, 0,									// CYRILLIC SMALL LETTER HARD SIGN
	/* ??? */ 0x044B,  32, 0,									// CYRILLIC SMALL LETTER YERU
	/* ??? */ 0x044C,  52, 0,									// CYRILLIC SMALL LETTER SOFT SIGN
	/* ??? */ 0x044D,  41, 0,									// CYRILLIC SMALL LETTER E
	/* ??? */ 0x044E,  54, 0,									// CYRILLIC SMALL LETTER YU
	/* ??? */ 0x044F,  46, 0,									// CYRILLIC SMALL LETTER YA
	/* ??? */ 0x0451,  55, 0,									// CYRILLIC SMALL LETTER IO
	/* ??? */ 0x2116,   2, 0,									// NUMERO SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Irish[23] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023,  42, 0,
	/* '@' */ 0x0040,  41, MO_ShiftLeft,
	/* '\' */ 0x005C,  45, 0,
	/* '`' */ 0x0060,   1, MO_DeadKey,
	/* '|' */ 0x007C,  45, MO_ShiftLeft,
	/* '~' */ 0x007E,  42, MO_ShiftLeft,
	/* '£' */ 0x00A3,   4, MO_ShiftLeft,
	/* '¦' */ 0x00A6,   1, MO_AltRight,
	/* '¬' */ 0x00AC,   1, MO_ShiftLeft,
	/* '´' */ 0x00B4,  41, MO_AltRight|MO_DeadKey,
	/* 'Á' */ 0x00C1,  31, MO_ShiftLeft|MO_AltRight,
	/* 'É' */ 0x00C9,  19, MO_ShiftLeft|MO_AltRight,
	/* 'Í' */ 0x00CD,  24, MO_ShiftLeft|MO_AltRight,
	/* 'Ó' */ 0x00D3,  25, MO_ShiftLeft|MO_AltRight,
	/* 'Ú' */ 0x00DA,  23, MO_ShiftLeft|MO_AltRight,
	/* 'á' */ 0x00E1,  31, MO_AltRight,
	/* 'é' */ 0x00E9,  19, MO_AltRight,
	/* 'í' */ 0x00ED,  24, MO_AltRight,
	/* 'ó' */ 0x00F3,  25, MO_AltRight,
	/* 'ú' */ 0x00FA,  23, MO_AltRight,
	/* '€' */ 0x20AC,   5, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Maltese[33] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023,  42, 0,
	/* '@' */ 0x0040,  41, MO_ShiftLeft,
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,  45, MO_AltRight,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '`' */ 0x0060,   1, MO_AltRight,
	/* '{' */ 0x007B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x007C,  45, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x007D,  28, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x007E,  42, MO_ShiftLeft,
	/* '£' */ 0x00A3,   4, MO_AltRight,
	/* '¬' */ 0x00AC,   1, MO_ShiftLeft|MO_AltRight,
	/* 'À' */ 0x00C0,  31, MO_ShiftLeft|MO_AltRight,
	/* 'È' */ 0x00C8,  19, MO_ShiftLeft|MO_AltRight,
	/* 'Ì' */ 0x00CC,  24, MO_ShiftLeft|MO_AltRight,
	/* 'Ò' */ 0x00D2,  25, MO_ShiftLeft|MO_AltRight,
	/* 'Ù' */ 0x00D9,  23, MO_ShiftLeft|MO_AltRight,
	/* 'à' */ 0x00E0,  31, MO_AltRight,
	/* 'è' */ 0x00E8,  19, MO_AltRight,
	/* 'ì' */ 0x00EC,  24, MO_AltRight,
	/* 'ò' */ 0x00F2,  25, MO_AltRight,
	/* 'ù' */ 0x00F9,  23, MO_AltRight,
	/* ??? */ 0x010A,   1, MO_ShiftLeft,					// LATIN CAPITAL LETTER C WITH DOT ABOVE
	/* ??? */ 0x010B,   1, 0,									// LATIN SMALL LETTER C WITH DOT ABOVE
	/* ??? */ 0x0120,  27, MO_ShiftLeft,					// LATIN CAPITAL LETTER G WITH DOT ABOVE
	/* ??? */ 0x0121,  27, 0,									// LATIN SMALL LETTER G WITH DOT ABOVE
	/* ??? */ 0x0126,  28, MO_ShiftLeft,					// LATIN CAPITAL LETTER H WITH STROKE
	/* ??? */ 0x0127,  28, 0,									// LATIN SMALL LETTER H WITH STROKE
	/* ??? */ 0x017B,  45, 0,									// LATIN CAPITAL LETTER Z WITH DOT ABOVE
	/* ??? */ 0x017C,  45, MO_ShiftLeft,					// LATIN SMALL LETTER Z WITH DOT ABOVE
	/* '€' */ 0x20AC,   4, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Icelandic[40] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,  28, 0,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  42, MO_ShiftLeft,
	/* '+' */ 0x002B,  42, 0,
	/* '-' */ 0x002D,  13, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  28, MO_ShiftLeft,
	/* '@' */ 0x0040,  17, MO_AltRight,
	/* '[' */ 0x005B,   9, MO_AltRight,
	/* '\' */ 0x005C,  12, MO_AltRight,
	/* ']' */ 0x005D,  10, MO_AltRight,
	/* '^' */ 0x005E,  41, MO_AltRight|MO_DeadKey,
	/* '_' */ 0x005F,  13, MO_ShiftLeft,
	/* '`' */ 0x0060,  42, MO_AltRight|MO_DeadKey,
	/* '{' */ 0x007B,   8, MO_AltRight,
	/* '|' */ 0x007C,  45, MO_AltRight,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* '~' */ 0x007E,  28, MO_AltRight,
	/* '¨' */ 0x00A8,   1, MO_ShiftLeft|MO_DeadKey,
	/* '°' */ 0x00B0,   1, MO_DeadKey,
	/* '´' */ 0x00B4,  41, MO_DeadKey,
	/* 'µ' */ 0x00B5,  52, MO_AltRight,
	/* 'Æ' */ 0x00C6,  40, MO_ShiftLeft,
	/* 'Ð' */ 0x00D0,  27, MO_ShiftLeft,
	/* 'Ö' */ 0x00D6,  12, MO_ShiftLeft,
	/* 'Þ' */ 0x00DE,  55, MO_ShiftLeft,
	/* 'æ' */ 0x00E6,  40, 0,
	/* 'ð' */ 0x00F0,  27, 0,
	/* 'ö' */ 0x00F6,  12, 0,
	/* 'þ' */ 0x00FE,  55, 0,
	/* '€' */ 0x20AC,  19, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Ukrainian[145] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023, NOS, 0,
	/* '$' */ 0x0024, NOS, 0,
	/* '&' */ 0x0026, NOS, 0,
	/* ''' */ 0x0027, NOS, 0,
	/* ',' */ 0x002C,  55, MO_ShiftLeft,
	/* '.' */ 0x002E,  55, 0,
	/* '/' */ 0x002F,  42, MO_ShiftLeft,
	/* ':' */ 0x003A,   7, MO_ShiftLeft,
	/* ';' */ 0x003B,   5, MO_ShiftLeft,
	/* '<' */ 0x003C, NOS, 0,
	/* '>' */ 0x003E, NOS, 0,
	/* '?' */ 0x003F,   8, MO_ShiftLeft,
	/* '@' */ 0x0040, NOS, 0,
	/* 'A' */ 0x0041, NOS, 0,
	/* 'B' */ 0x0042, NOS, 0,
	/* 'C' */ 0x0043, NOS, 0,
	/* 'D' */ 0x0044, NOS, 0,
	/* 'E' */ 0x0045, NOS, 0,
	/* 'F' */ 0x0046, NOS, 0,
	/* 'G' */ 0x0047, NOS, 0,
	/* 'H' */ 0x0048, NOS, 0,
	/* 'I' */ 0x0049, NOS, 0,
	/* 'J' */ 0x004A, NOS, 0,
	/* 'K' */ 0x004B, NOS, 0,
	/* 'L' */ 0x004C, NOS, 0,
	/* 'M' */ 0x004D, NOS, 0,
	/* 'N' */ 0x004E, NOS, 0,
	/* 'O' */ 0x004F, NOS, 0,
	/* 'P' */ 0x0050, NOS, 0,
	/* 'Q' */ 0x0051, NOS, 0,
	/* 'R' */ 0x0052, NOS, 0,
	/* 'S' */ 0x0053, NOS, 0,
	/* 'T' */ 0x0054, NOS, 0,
	/* 'U' */ 0x0055, NOS, 0,
	/* 'V' */ 0x0056, NOS, 0,
	/* 'W' */ 0x0057, NOS, 0,
	/* 'X' */ 0x0058, NOS, 0,
	/* 'Y' */ 0x0059, NOS, 0,
	/* 'Z' */ 0x005A, NOS, 0,
	/* '[' */ 0x005B, NOS, 0,
	/* '\' */ 0x005C,  42, 0,
	/* ']' */ 0x005D, NOS, 0,
	/* '^' */ 0x005E, NOS, 0,
	/* '`' */ 0x0060, NOS, 0,
	/* 'a' */ 0x0061, NOS, 0,
	/* 'b' */ 0x0062, NOS, 0,
	/* 'c' */ 0x0063, NOS, 0,
	/* 'd' */ 0x0064, NOS, 0,
	/* 'e' */ 0x0065, NOS, 0,
	/* 'f' */ 0x0066, NOS, 0,
	/* 'g' */ 0x0067, NOS, 0,
	/* 'h' */ 0x0068, NOS, 0,
	/* 'i' */ 0x0069, NOS, 0,
	/* 'j' */ 0x006A, NOS, 0,
	/* 'k' */ 0x006B, NOS, 0,
	/* 'l' */ 0x006C, NOS, 0,
	/* 'm' */ 0x006D, NOS, 0,
	/* 'n' */ 0x006E, NOS, 0,
	/* 'o' */ 0x006F, NOS, 0,
	/* 'p' */ 0x0070, NOS, 0,
	/* 'q' */ 0x0071, NOS, 0,
	/* 'r' */ 0x0072, NOS, 0,
	/* 's' */ 0x0073, NOS, 0,
	/* 't' */ 0x0074, NOS, 0,
	/* 'u' */ 0x0075, NOS, 0,
	/* 'v' */ 0x0076, NOS, 0,
	/* 'w' */ 0x0077, NOS, 0,
	/* 'x' */ 0x0078, NOS, 0,
	/* 'y' */ 0x0079, NOS, 0,
	/* 'z' */ 0x007A, NOS, 0,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* ??? */ 0x0401,   1, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER IO
	/* ??? */ 0x0404,  41, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER UKRAINIAN IE
	/* ??? */ 0x0406,  32, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I
	/* ??? */ 0x0407,  28, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YI
	/* ??? */ 0x0410,  34, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER A
	/* ??? */ 0x0411,  53, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER BE
	/* ??? */ 0x0412,  33, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER VE
	/* ??? */ 0x0413,  23, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER GHE
	/* ??? */ 0x0414,  39, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER DE
	/* ??? */ 0x0415,  21, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER IE
	/* ??? */ 0x0416,  40, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZHE
	/* ??? */ 0x0417,  26, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZE
	/* ??? */ 0x0418,  50, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER I
	/* ??? */ 0x0419,  17, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHORT I
	/* ??? */ 0x041A,  20, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER KA
	/* ??? */ 0x041B,  38, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EL
	/* ??? */ 0x041C,  49, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EM
	/* ??? */ 0x041D,  22, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EN
	/* ??? */ 0x041E,  37, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER O
	/* ??? */ 0x041F,  35, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER PE
	/* ??? */ 0x0420,  36, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ER
	/* ??? */ 0x0421,  48, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ES
	/* ??? */ 0x0422,  51, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TE
	/* ??? */ 0x0423,  19, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER U
	/* ??? */ 0x0424,  31, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EF
	/* ??? */ 0x0425,  27, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER HA
	/* ??? */ 0x0426,  18, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TSE
	/* ??? */ 0x0427,  47, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER CHE
	/* ??? */ 0x0428,  24, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHA
	/* ??? */ 0x0429,  25, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHCHA
	/* ??? */ 0x042C,  52, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SOFT SIGN
	/* ??? */ 0x042E,  54, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YU
	/* ??? */ 0x042F,  46, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YA
	/* ??? */ 0x0430,  34, 0,									// CYRILLIC SMALL LETTER A
	/* ??? */ 0x0431,  53, 0,									// CYRILLIC SMALL LETTER BE
	/* ??? */ 0x0432,  33, 0,									// CYRILLIC SMALL LETTER VE
	/* ??? */ 0x0433,  23, 0,									// CYRILLIC SMALL LETTER GHE
	/* ??? */ 0x0434,  39, 0,									// CYRILLIC SMALL LETTER DE
	/* ??? */ 0x0435,  21, 0,									// CYRILLIC SMALL LETTER IE
	/* ??? */ 0x0436,  40, 0,									// CYRILLIC SMALL LETTER ZHE
	/* ??? */ 0x0437,  26, 0,									// CYRILLIC SMALL LETTER ZE
	/* ??? */ 0x0438,  50, 0,									// CYRILLIC SMALL LETTER I
	/* ??? */ 0x0439,  17, 0,									// CYRILLIC SMALL LETTER SHORT I
	/* ??? */ 0x043A,  20, 0,									// CYRILLIC SMALL LETTER KA
	/* ??? */ 0x043B,  38, 0,									// CYRILLIC SMALL LETTER EL
	/* ??? */ 0x043C,  49, 0,									// CYRILLIC SMALL LETTER EM
	/* ??? */ 0x043D,  22, 0,									// CYRILLIC SMALL LETTER EN
	/* ??? */ 0x043E,  37, 0,									// CYRILLIC SMALL LETTER O
	/* ??? */ 0x043F,  35, 0,									// CYRILLIC SMALL LETTER PE
	/* ??? */ 0x0440,  36, 0,									// CYRILLIC SMALL LETTER ER
	/* ??? */ 0x0441,  48, 0,									// CYRILLIC SMALL LETTER ES
	/* ??? */ 0x0442,  51, 0,									// CYRILLIC SMALL LETTER TE
	/* ??? */ 0x0443,  19, 0,									// CYRILLIC SMALL LETTER U
	/* ??? */ 0x0444,  31, 0,									// CYRILLIC SMALL LETTER EF
	/* ??? */ 0x0445,  27, 0,									// CYRILLIC SMALL LETTER HA
	/* ??? */ 0x0446,  18, 0,									// CYRILLIC SMALL LETTER TSE
	/* ??? */ 0x0447,  47, 0,									// CYRILLIC SMALL LETTER CHE
	/* ??? */ 0x0448,  24, 0,									// CYRILLIC SMALL LETTER SHA
	/* ??? */ 0x0449,  25, 0,									// CYRILLIC SMALL LETTER SHCHA
	/* ??? */ 0x044C,  52, 0,									// CYRILLIC SMALL LETTER SOFT SIGN
	/* ??? */ 0x044E,  54, 0,									// CYRILLIC SMALL LETTER YU
	/* ??? */ 0x044F,  46, 0,									// CYRILLIC SMALL LETTER YA
	/* ??? */ 0x0451,   1, 0,									// CYRILLIC SMALL LETTER IO
	/* ??? */ 0x0454,  41, 0,									// CYRILLIC SMALL LETTER UKRAINIAN IE
	/* ??? */ 0x0456,  32, 0,									// CYRILLIC SMALL LETTER BYELORUSSIAN-UKRAINIAN I
	/* ??? */ 0x0457,  28, 0,									// CYRILLIC SMALL LETTER YI
	/* ??? */ 0x0490,  23, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER GHE WITH UPTURN
	/* ??? */ 0x0491,  23, MO_AltRight,						// CYRILLIC SMALL LETTER GHE WITH UPTURN
	/* ??? */ 0x2116,   4, MO_ShiftLeft,					// NUMERO SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Uzbek_Cyrillic[151] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023, NOS, 0,
	/* '$' */ 0x0024, NOS, 0,
	/* '&' */ 0x0026, NOS, 0,
	/* ''' */ 0x0027, NOS, 0,
	/* '+' */ 0x002B, NOS, 0,
	/* ',' */ 0x002C,  55, MO_ShiftLeft,
	/* '-' */ 0x002D, NOS, 0,
	/* '.' */ 0x002E,  55, 0,
	/* '/' */ 0x002F,  42, MO_ShiftLeft,
	/* ':' */ 0x003A,   7, MO_ShiftLeft,
	/* ';' */ 0x003B,   5, MO_ShiftLeft,
	/* '<' */ 0x003C, NOS, 0,
	/* '=' */ 0x003D, NOS, 0,
	/* '>' */ 0x003E, NOS, 0,
	/* '?' */ 0x003F,   8, MO_ShiftLeft,
	/* '@' */ 0x0040, NOS, 0,
	/* 'A' */ 0x0041, NOS, 0,
	/* 'B' */ 0x0042, NOS, 0,
	/* 'C' */ 0x0043, NOS, 0,
	/* 'D' */ 0x0044, NOS, 0,
	/* 'E' */ 0x0045, NOS, 0,
	/* 'F' */ 0x0046, NOS, 0,
	/* 'G' */ 0x0047, NOS, 0,
	/* 'H' */ 0x0048, NOS, 0,
	/* 'I' */ 0x0049, NOS, 0,
	/* 'J' */ 0x004A, NOS, 0,
	/* 'K' */ 0x004B, NOS, 0,
	/* 'L' */ 0x004C, NOS, 0,
	/* 'M' */ 0x004D, NOS, 0,
	/* 'N' */ 0x004E, NOS, 0,
	/* 'O' */ 0x004F, NOS, 0,
	/* 'P' */ 0x0050, NOS, 0,
	/* 'Q' */ 0x0051, NOS, 0,
	/* 'R' */ 0x0052, NOS, 0,
	/* 'S' */ 0x0053, NOS, 0,
	/* 'T' */ 0x0054, NOS, 0,
	/* 'U' */ 0x0055, NOS, 0,
	/* 'V' */ 0x0056, NOS, 0,
	/* 'W' */ 0x0057, NOS, 0,
	/* 'X' */ 0x0058, NOS, 0,
	/* 'Y' */ 0x0059, NOS, 0,
	/* 'Z' */ 0x005A, NOS, 0,
	/* '[' */ 0x005B, NOS, 0,
	/* '\' */ 0x005C,  42, 0,
	/* ']' */ 0x005D, NOS, 0,
	/* '^' */ 0x005E, NOS, 0,
	/* '_' */ 0x005F, NOS, 0,
	/* '`' */ 0x0060, NOS, 0,
	/* 'a' */ 0x0061, NOS, 0,
	/* 'b' */ 0x0062, NOS, 0,
	/* 'c' */ 0x0063, NOS, 0,
	/* 'd' */ 0x0064, NOS, 0,
	/* 'e' */ 0x0065, NOS, 0,
	/* 'f' */ 0x0066, NOS, 0,
	/* 'g' */ 0x0067, NOS, 0,
	/* 'h' */ 0x0068, NOS, 0,
	/* 'i' */ 0x0069, NOS, 0,
	/* 'j' */ 0x006A, NOS, 0,
	/* 'k' */ 0x006B, NOS, 0,
	/* 'l' */ 0x006C, NOS, 0,
	/* 'm' */ 0x006D, NOS, 0,
	/* 'n' */ 0x006E, NOS, 0,
	/* 'o' */ 0x006F, NOS, 0,
	/* 'p' */ 0x0070, NOS, 0,
	/* 'q' */ 0x0071, NOS, 0,
	/* 'r' */ 0x0072, NOS, 0,
	/* 's' */ 0x0073, NOS, 0,
	/* 't' */ 0x0074, NOS, 0,
	/* 'u' */ 0x0075, NOS, 0,
	/* 'v' */ 0x0076, NOS, 0,
	/* 'w' */ 0x0077, NOS, 0,
	/* 'x' */ 0x0078, NOS, 0,
	/* 'y' */ 0x0079, NOS, 0,
	/* 'z' */ 0x007A, NOS, 0,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* ??? */ 0x0401,   1, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER IO
	/* ??? */ 0x040E,  25, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHORT U
	/* ??? */ 0x0410,  34, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER A
	/* ??? */ 0x0411,  53, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER BE
	/* ??? */ 0x0412,  33, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER VE
	/* ??? */ 0x0413,  23, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER GHE
	/* ??? */ 0x0414,  39, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER DE
	/* ??? */ 0x0415,  21, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER IE
	/* ??? */ 0x0416,  40, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZHE
	/* ??? */ 0x0417,  26, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZE
	/* ??? */ 0x0418,  50, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER I
	/* ??? */ 0x0419,  17, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHORT I
	/* ??? */ 0x041A,  20, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER KA
	/* ??? */ 0x041B,  38, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EL
	/* ??? */ 0x041C,  49, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EM
	/* ??? */ 0x041D,  22, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EN
	/* ??? */ 0x041E,  37, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER O
	/* ??? */ 0x041F,  35, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER PE
	/* ??? */ 0x0420,  36, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ER
	/* ??? */ 0x0421,  48, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ES
	/* ??? */ 0x0422,  51, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TE
	/* ??? */ 0x0423,  19, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER U
	/* ??? */ 0x0424,  31, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EF
	/* ??? */ 0x0425,  27, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER HA
	/* ??? */ 0x0426,  18, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TSE
	/* ??? */ 0x0427,  47, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER CHE
	/* ??? */ 0x0428,  24, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHA
	/* ??? */ 0x042A,  28, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER HARD SIGN
	/* ??? */ 0x042C,  52, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SOFT SIGN
	/* ??? */ 0x042D,  41, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER E
	/* ??? */ 0x042E,  54, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YU
	/* ??? */ 0x042F,  46, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YA
	/* ??? */ 0x0430,  34, 0,									// CYRILLIC SMALL LETTER A
	/* ??? */ 0x0431,  53, 0,									// CYRILLIC SMALL LETTER BE
	/* ??? */ 0x0432,  33, 0,									// CYRILLIC SMALL LETTER VE
	/* ??? */ 0x0433,  23, 0,									// CYRILLIC SMALL LETTER GHE
	/* ??? */ 0x0434,  39, 0,									// CYRILLIC SMALL LETTER DE
	/* ??? */ 0x0435,  21, 0,									// CYRILLIC SMALL LETTER IE
	/* ??? */ 0x0436,  40, 0,									// CYRILLIC SMALL LETTER ZHE
	/* ??? */ 0x0437,  26, 0,									// CYRILLIC SMALL LETTER ZE
	/* ??? */ 0x0438,  50, 0,									// CYRILLIC SMALL LETTER I
	/* ??? */ 0x0439,  17, 0,									// CYRILLIC SMALL LETTER SHORT I
	/* ??? */ 0x043A,  20, 0,									// CYRILLIC SMALL LETTER KA
	/* ??? */ 0x043B,  38, 0,									// CYRILLIC SMALL LETTER EL
	/* ??? */ 0x043C,  49, 0,									// CYRILLIC SMALL LETTER EM
	/* ??? */ 0x043D,  22, 0,									// CYRILLIC SMALL LETTER EN
	/* ??? */ 0x043E,  37, 0,									// CYRILLIC SMALL LETTER O
	/* ??? */ 0x043F,  35, 0,									// CYRILLIC SMALL LETTER PE
	/* ??? */ 0x0440,  36, 0,									// CYRILLIC SMALL LETTER ER
	/* ??? */ 0x0441,  48, 0,									// CYRILLIC SMALL LETTER ES
	/* ??? */ 0x0442,  51, 0,									// CYRILLIC SMALL LETTER TE
	/* ??? */ 0x0443,  19, 0,									// CYRILLIC SMALL LETTER U
	/* ??? */ 0x0444,  31, 0,									// CYRILLIC SMALL LETTER EF
	/* ??? */ 0x0445,  27, 0,									// CYRILLIC SMALL LETTER HA
	/* ??? */ 0x0446,  18, 0,									// CYRILLIC SMALL LETTER TSE
	/* ??? */ 0x0447,  47, 0,									// CYRILLIC SMALL LETTER CHE
	/* ??? */ 0x0448,  24, 0,									// CYRILLIC SMALL LETTER SHA
	/* ??? */ 0x044A,  28, 0,									// CYRILLIC SMALL LETTER HARD SIGN
	/* ??? */ 0x044C,  52, 0,									// CYRILLIC SMALL LETTER SOFT SIGN
	/* ??? */ 0x044D,  41, 0,									// CYRILLIC SMALL LETTER E
	/* ??? */ 0x044E,  54, 0,									// CYRILLIC SMALL LETTER YU
	/* ??? */ 0x044F,  46, 0,									// CYRILLIC SMALL LETTER YA
	/* ??? */ 0x0451,   1, 0,									// CYRILLIC SMALL LETTER IO
	/* ??? */ 0x045E,  25, 0,									// CYRILLIC SMALL LETTER SHORT U
	/* ??? */ 0x0492,  12, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER GHE WITH STROKE
	/* ??? */ 0x0493,  12, 0,									// CYRILLIC SMALL LETTER GHE WITH STROKE
	/* ??? */ 0x049A,  32, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER KA WITH DESCENDER
	/* ??? */ 0x049B,  32, 0,									// CYRILLIC SMALL LETTER KA WITH DESCENDER
	/* ??? */ 0x04B2,  13, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER HA WITH DESCENDER
	/* ??? */ 0x04B3,  13, 0,									// CYRILLIC SMALL LETTER HA WITH DESCENDER
	/* ??? */ 0x2116,   4, MO_ShiftLeft,					// NUMERO SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Kazakh[176] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   2, 0,
	/* '#' */ 0x0023, NOS, 0,
	/* '$' */ 0x0024, NOS, 0,
	/* '%' */ 0x0025, NOS, 0,
	/* '&' */ 0x0026, NOS, 0,
	/* ''' */ 0x0027, NOS, 0,
	/* '(' */ 0x0028,   1, 0,
	/* ')' */ 0x0029,   1, MO_ShiftLeft,
	/* '*' */ 0x002A, NOS, 0,
	/* '+' */ 0x002B, NOS, 0,
	/* ',' */ 0x002C,   7, 0,
	/* '-' */ 0x002D, NOS, 0,
	/* '.' */ 0x002E,   8, 0,
	/* '/' */ 0x002F,  42, MO_ShiftLeft,
	/* '0' */ 0x0030, NOS, 0,
	/* '1' */ 0x0031, NOS, 0,
	/* '2' */ 0x0032, NOS, 0,
	/* '3' */ 0x0033, NOS, 0,
	/* '4' */ 0x0034, NOS, 0,
	/* '5' */ 0x0035, NOS, 0,
	/* '6' */ 0x0036, NOS, 0,
	/* '7' */ 0x0037, NOS, 0,
	/* '8' */ 0x0038, NOS, 0,
	/* '9' */ 0x0039, NOS, 0,
	/* ':' */ 0x003A,   8, MO_ShiftLeft,
	/* ';' */ 0x003B,   7, MO_ShiftLeft,
	/* '<' */ 0x003C, NOS, 0,
	/* '=' */ 0x003D, NOS, 0,
	/* '>' */ 0x003E, NOS, 0,
	/* '@' */ 0x0040, NOS, 0,
	/* 'A' */ 0x0041, NOS, 0,
	/* 'B' */ 0x0042, NOS, 0,
	/* 'C' */ 0x0043, NOS, 0,
	/* 'D' */ 0x0044, NOS, 0,
	/* 'E' */ 0x0045, NOS, 0,
	/* 'F' */ 0x0046, NOS, 0,
	/* 'G' */ 0x0047, NOS, 0,
	/* 'H' */ 0x0048, NOS, 0,
	/* 'I' */ 0x0049, NOS, 0,
	/* 'J' */ 0x004A, NOS, 0,
	/* 'K' */ 0x004B, NOS, 0,
	/* 'L' */ 0x004C, NOS, 0,
	/* 'M' */ 0x004D, NOS, 0,
	/* 'N' */ 0x004E, NOS, 0,
	/* 'O' */ 0x004F, NOS, 0,
	/* 'P' */ 0x0050, NOS, 0,
	/* 'Q' */ 0x0051, NOS, 0,
	/* 'R' */ 0x0052, NOS, 0,
	/* 'S' */ 0x0053, NOS, 0,
	/* 'T' */ 0x0054, NOS, 0,
	/* 'U' */ 0x0055, NOS, 0,
	/* 'V' */ 0x0056, NOS, 0,
	/* 'W' */ 0x0057, NOS, 0,
	/* 'X' */ 0x0058, NOS, 0,
	/* 'Y' */ 0x0059, NOS, 0,
	/* 'Z' */ 0x005A, NOS, 0,
	/* '[' */ 0x005B, NOS, 0,
	/* '\' */ 0x005C,  42, 0,
	/* ']' */ 0x005D, NOS, 0,
	/* '^' */ 0x005E, NOS, 0,
	/* '_' */ 0x005F, NOS, 0,
	/* '`' */ 0x0060, NOS, 0,
	/* 'a' */ 0x0061, NOS, 0,
	/* 'b' */ 0x0062, NOS, 0,
	/* 'c' */ 0x0063, NOS, 0,
	/* 'd' */ 0x0064, NOS, 0,
	/* 'e' */ 0x0065, NOS, 0,
	/* 'f' */ 0x0066, NOS, 0,
	/* 'g' */ 0x0067, NOS, 0,
	/* 'h' */ 0x0068, NOS, 0,
	/* 'i' */ 0x0069, NOS, 0,
	/* 'j' */ 0x006A, NOS, 0,
	/* 'k' */ 0x006B, NOS, 0,
	/* 'l' */ 0x006C, NOS, 0,
	/* 'm' */ 0x006D, NOS, 0,
	/* 'n' */ 0x006E, NOS, 0,
	/* 'o' */ 0x006F, NOS, 0,
	/* 'p' */ 0x0070, NOS, 0,
	/* 'q' */ 0x0071, NOS, 0,
	/* 'r' */ 0x0072, NOS, 0,
	/* 's' */ 0x0073, NOS, 0,
	/* 't' */ 0x0074, NOS, 0,
	/* 'u' */ 0x0075, NOS, 0,
	/* 'v' */ 0x0076, NOS, 0,
	/* 'w' */ 0x0077, NOS, 0,
	/* 'x' */ 0x0078, NOS, 0,
	/* 'y' */ 0x0079, NOS, 0,
	/* 'z' */ 0x007A, NOS, 0,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C,  45, MO_ShiftLeft,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* ??? */ 0x0406,   4, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I
	/* ??? */ 0x0410,  34, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER A
	/* ??? */ 0x0411,  53, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER BE
	/* ??? */ 0x0412,  33, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER VE
	/* ??? */ 0x0413,  23, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER GHE
	/* ??? */ 0x0414,  39, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER DE
	/* ??? */ 0x0415,  21, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER IE
	/* ??? */ 0x0416,  40, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZHE
	/* ??? */ 0x0417,  26, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZE
	/* ??? */ 0x0418,  50, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER I
	/* ??? */ 0x0419,  17, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHORT I
	/* ??? */ 0x041A,  20, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER KA
	/* ??? */ 0x041B,  38, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EL
	/* ??? */ 0x041C,  49, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EM
	/* ??? */ 0x041D,  22, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EN
	/* ??? */ 0x041E,  37, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER O
	/* ??? */ 0x041F,  35, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER PE
	/* ??? */ 0x0420,  36, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ER
	/* ??? */ 0x0421,  48, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ES
	/* ??? */ 0x0422,  51, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TE
	/* ??? */ 0x0423,  19, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER U
	/* ??? */ 0x0424,  31, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EF
	/* ??? */ 0x0425,  27, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER HA
	/* ??? */ 0x0426,  18, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TSE
	/* ??? */ 0x0427,  47, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER CHE
	/* ??? */ 0x0428,  24, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHA
	/* ??? */ 0x0429,  25, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHCHA
	/* ??? */ 0x042A,  28, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER HARD SIGN
	/* ??? */ 0x042B,  32, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YERU
	/* ??? */ 0x042C,  52, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SOFT SIGN
	/* ??? */ 0x042D,  41, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER E
	/* ??? */ 0x042E,  54, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YU
	/* ??? */ 0x042F,  46, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YA
	/* ??? */ 0x0430,  34, 0,									// CYRILLIC SMALL LETTER A
	/* ??? */ 0x0431,  53, 0,									// CYRILLIC SMALL LETTER BE
	/* ??? */ 0x0432,  33, 0,									// CYRILLIC SMALL LETTER VE
	/* ??? */ 0x0433,  23, 0,									// CYRILLIC SMALL LETTER GHE
	/* ??? */ 0x0434,  39, 0,									// CYRILLIC SMALL LETTER DE
	/* ??? */ 0x0435,  21, 0,									// CYRILLIC SMALL LETTER IE
	/* ??? */ 0x0436,  40, 0,									// CYRILLIC SMALL LETTER ZHE
	/* ??? */ 0x0437,  26, 0,									// CYRILLIC SMALL LETTER ZE
	/* ??? */ 0x0438,  50, 0,									// CYRILLIC SMALL LETTER I
	/* ??? */ 0x0439,  17, 0,									// CYRILLIC SMALL LETTER SHORT I
	/* ??? */ 0x043A,  20, 0,									// CYRILLIC SMALL LETTER KA
	/* ??? */ 0x043B,  38, 0,									// CYRILLIC SMALL LETTER EL
	/* ??? */ 0x043C,  49, 0,									// CYRILLIC SMALL LETTER EM
	/* ??? */ 0x043D,  22, 0,									// CYRILLIC SMALL LETTER EN
	/* ??? */ 0x043E,  37, 0,									// CYRILLIC SMALL LETTER O
	/* ??? */ 0x043F,  35, 0,									// CYRILLIC SMALL LETTER PE
	/* ??? */ 0x0440,  36, 0,									// CYRILLIC SMALL LETTER ER
	/* ??? */ 0x0441,  48, 0,									// CYRILLIC SMALL LETTER ES
	/* ??? */ 0x0442,  51, 0,									// CYRILLIC SMALL LETTER TE
	/* ??? */ 0x0443,  19, 0,									// CYRILLIC SMALL LETTER U
	/* ??? */ 0x0444,  31, 0,									// CYRILLIC SMALL LETTER EF
	/* ??? */ 0x0445,  27, 0,									// CYRILLIC SMALL LETTER HA
	/* ??? */ 0x0446,  18, 0,									// CYRILLIC SMALL LETTER TSE
	/* ??? */ 0x0447,  47, 0,									// CYRILLIC SMALL LETTER CHE
	/* ??? */ 0x0448,  24, 0,									// CYRILLIC SMALL LETTER SHA
	/* ??? */ 0x0449,  25, 0,									// CYRILLIC SMALL LETTER SHCHA
	/* ??? */ 0x044A,  28, 0,									// CYRILLIC SMALL LETTER HARD SIGN
	/* ??? */ 0x044B,  32, 0,									// CYRILLIC SMALL LETTER YERU
	/* ??? */ 0x044C,  52, 0,									// CYRILLIC SMALL LETTER SOFT SIGN
	/* ??? */ 0x044D,  41, 0,									// CYRILLIC SMALL LETTER E
	/* ??? */ 0x044E,  54, 0,									// CYRILLIC SMALL LETTER YU
	/* ??? */ 0x044F,  46, 0,									// CYRILLIC SMALL LETTER YA
	/* ??? */ 0x0456,   4, 0,									// CYRILLIC SMALL LETTER BYELORUSSIAN-UKRAINIAN I
	/* ??? */ 0x0492,   6, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER GHE WITH STROKE
	/* ??? */ 0x0493,   6, 0,									// CYRILLIC SMALL LETTER GHE WITH STROKE
	/* ??? */ 0x049A,  11, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER KA WITH DESCENDER
	/* ??? */ 0x049B,  11, 0,									// CYRILLIC SMALL LETTER KA WITH DESCENDER
	/* ??? */ 0x04A2,   5, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EN WITH DESCENDER
	/* ??? */ 0x04A3,   5, 0,									// CYRILLIC SMALL LETTER EN WITH DESCENDER
	/* ??? */ 0x04AE,   9, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER STRAIGHT U
	/* ??? */ 0x04AF,   9, 0,									// CYRILLIC SMALL LETTER STRAIGHT U
	/* ??? */ 0x04B0,  10, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER STRAIGHT U WITH STROKE
	/* ??? */ 0x04B1,  10, 0,									// CYRILLIC SMALL LETTER STRAIGHT U WITH STROKE
	/* ??? */ 0x04BA,  13, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHHA
	/* ??? */ 0x04BB,  13, 0,									// CYRILLIC SMALL LETTER SHHA
	/* ??? */ 0x04D8,   3, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SCHWA
	/* ??? */ 0x04D9,   3, 0,									// CYRILLIC SMALL LETTER SCHWA
	/* ??? */ 0x04E8,  12, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER BARRED O
	/* ??? */ 0x04E9,  12, 0,									// CYRILLIC SMALL LETTER BARRED O
	/* ??? */ 0x2116,  55, 0,									// NUMERO SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Kyrgyz_Cyrillic[149] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023, NOS, 0,
	/* '$' */ 0x0024, NOS, 0,
	/* '&' */ 0x0026, NOS, 0,
	/* ''' */ 0x0027, NOS, 0,
	/* ',' */ 0x002C,  55, MO_ShiftLeft,
	/* '.' */ 0x002E,  55, 0,
	/* '/' */ 0x002F,  42, MO_ShiftLeft,
	/* ':' */ 0x003A,   7, MO_ShiftLeft,
	/* ';' */ 0x003B,   5, MO_ShiftLeft,
	/* '<' */ 0x003C, NOS, 0,
	/* '>' */ 0x003E, NOS, 0,
	/* '?' */ 0x003F,   8, MO_ShiftLeft,
	/* '@' */ 0x0040, NOS, 0,
	/* 'A' */ 0x0041, NOS, 0,
	/* 'B' */ 0x0042, NOS, 0,
	/* 'C' */ 0x0043, NOS, 0,
	/* 'D' */ 0x0044, NOS, 0,
	/* 'E' */ 0x0045, NOS, 0,
	/* 'F' */ 0x0046, NOS, 0,
	/* 'G' */ 0x0047, NOS, 0,
	/* 'H' */ 0x0048, NOS, 0,
	/* 'I' */ 0x0049, NOS, 0,
	/* 'J' */ 0x004A, NOS, 0,
	/* 'K' */ 0x004B, NOS, 0,
	/* 'L' */ 0x004C, NOS, 0,
	/* 'M' */ 0x004D, NOS, 0,
	/* 'N' */ 0x004E, NOS, 0,
	/* 'O' */ 0x004F, NOS, 0,
	/* 'P' */ 0x0050, NOS, 0,
	/* 'Q' */ 0x0051, NOS, 0,
	/* 'R' */ 0x0052, NOS, 0,
	/* 'S' */ 0x0053, NOS, 0,
	/* 'T' */ 0x0054, NOS, 0,
	/* 'U' */ 0x0055, NOS, 0,
	/* 'V' */ 0x0056, NOS, 0,
	/* 'W' */ 0x0057, NOS, 0,
	/* 'X' */ 0x0058, NOS, 0,
	/* 'Y' */ 0x0059, NOS, 0,
	/* 'Z' */ 0x005A, NOS, 0,
	/* '[' */ 0x005B, NOS, 0,
	/* '\' */ 0x005C,  42, 0,
	/* ']' */ 0x005D, NOS, 0,
	/* '^' */ 0x005E, NOS, 0,
	/* '`' */ 0x0060, NOS, 0,
	/* 'a' */ 0x0061, NOS, 0,
	/* 'b' */ 0x0062, NOS, 0,
	/* 'c' */ 0x0063, NOS, 0,
	/* 'd' */ 0x0064, NOS, 0,
	/* 'e' */ 0x0065, NOS, 0,
	/* 'f' */ 0x0066, NOS, 0,
	/* 'g' */ 0x0067, NOS, 0,
	/* 'h' */ 0x0068, NOS, 0,
	/* 'i' */ 0x0069, NOS, 0,
	/* 'j' */ 0x006A, NOS, 0,
	/* 'k' */ 0x006B, NOS, 0,
	/* 'l' */ 0x006C, NOS, 0,
	/* 'm' */ 0x006D, NOS, 0,
	/* 'n' */ 0x006E, NOS, 0,
	/* 'o' */ 0x006F, NOS, 0,
	/* 'p' */ 0x0070, NOS, 0,
	/* 'q' */ 0x0071, NOS, 0,
	/* 'r' */ 0x0072, NOS, 0,
	/* 's' */ 0x0073, NOS, 0,
	/* 't' */ 0x0074, NOS, 0,
	/* 'u' */ 0x0075, NOS, 0,
	/* 'v' */ 0x0076, NOS, 0,
	/* 'w' */ 0x0077, NOS, 0,
	/* 'x' */ 0x0078, NOS, 0,
	/* 'y' */ 0x0079, NOS, 0,
	/* 'z' */ 0x007A, NOS, 0,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* ??? */ 0x0401,   1, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER IO
	/* ??? */ 0x0410,  34, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER A
	/* ??? */ 0x0411,  53, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER BE
	/* ??? */ 0x0412,  33, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER VE
	/* ??? */ 0x0413,  23, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER GHE
	/* ??? */ 0x0414,  39, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER DE
	/* ??? */ 0x0415,  21, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER IE
	/* ??? */ 0x0416,  40, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZHE
	/* ??? */ 0x0417,  26, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZE
	/* ??? */ 0x0418,  50, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER I
	/* ??? */ 0x0419,  17, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHORT I
	/* ??? */ 0x041A,  20, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER KA
	/* ??? */ 0x041B,  38, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EL
	/* ??? */ 0x041C,  49, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EM
	/* ??? */ 0x041D,  22, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EN
	/* ??? */ 0x041E,  37, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER O
	/* ??? */ 0x041F,  35, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER PE
	/* ??? */ 0x0420,  36, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ER
	/* ??? */ 0x0421,  48, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ES
	/* ??? */ 0x0422,  51, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TE
	/* ??? */ 0x0423,  19, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER U
	/* ??? */ 0x0424,  31, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EF
	/* ??? */ 0x0425,  27, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER HA
	/* ??? */ 0x0426,  18, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TSE
	/* ??? */ 0x0427,  47, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER CHE
	/* ??? */ 0x0428,  24, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHA
	/* ??? */ 0x0429,  25, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHCHA
	/* ??? */ 0x042A,  28, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER HARD SIGN
	/* ??? */ 0x042B,  32, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YERU
	/* ??? */ 0x042C,  52, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SOFT SIGN
	/* ??? */ 0x042D,  41, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER E
	/* ??? */ 0x042E,  54, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YU
	/* ??? */ 0x042F,  46, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YA
	/* ??? */ 0x0430,  34, 0,									// CYRILLIC SMALL LETTER A
	/* ??? */ 0x0431,  53, 0,									// CYRILLIC SMALL LETTER BE
	/* ??? */ 0x0432,  33, 0,									// CYRILLIC SMALL LETTER VE
	/* ??? */ 0x0433,  23, 0,									// CYRILLIC SMALL LETTER GHE
	/* ??? */ 0x0434,  39, 0,									// CYRILLIC SMALL LETTER DE
	/* ??? */ 0x0435,  21, 0,									// CYRILLIC SMALL LETTER IE
	/* ??? */ 0x0436,  40, 0,									// CYRILLIC SMALL LETTER ZHE
	/* ??? */ 0x0437,  26, 0,									// CYRILLIC SMALL LETTER ZE
	/* ??? */ 0x0438,  50, 0,									// CYRILLIC SMALL LETTER I
	/* ??? */ 0x0439,  17, 0,									// CYRILLIC SMALL LETTER SHORT I
	/* ??? */ 0x043A,  20, 0,									// CYRILLIC SMALL LETTER KA
	/* ??? */ 0x043B,  38, 0,									// CYRILLIC SMALL LETTER EL
	/* ??? */ 0x043C,  49, 0,									// CYRILLIC SMALL LETTER EM
	/* ??? */ 0x043D,  22, 0,									// CYRILLIC SMALL LETTER EN
	/* ??? */ 0x043E,  37, 0,									// CYRILLIC SMALL LETTER O
	/* ??? */ 0x043F,  35, 0,									// CYRILLIC SMALL LETTER PE
	/* ??? */ 0x0440,  36, 0,									// CYRILLIC SMALL LETTER ER
	/* ??? */ 0x0441,  48, 0,									// CYRILLIC SMALL LETTER ES
	/* ??? */ 0x0442,  51, 0,									// CYRILLIC SMALL LETTER TE
	/* ??? */ 0x0443,  19, 0,									// CYRILLIC SMALL LETTER U
	/* ??? */ 0x0444,  31, 0,									// CYRILLIC SMALL LETTER EF
	/* ??? */ 0x0445,  27, 0,									// CYRILLIC SMALL LETTER HA
	/* ??? */ 0x0446,  18, 0,									// CYRILLIC SMALL LETTER TSE
	/* ??? */ 0x0447,  47, 0,									// CYRILLIC SMALL LETTER CHE
	/* ??? */ 0x0448,  24, 0,									// CYRILLIC SMALL LETTER SHA
	/* ??? */ 0x0449,  25, 0,									// CYRILLIC SMALL LETTER SHCHA
	/* ??? */ 0x044A,  28, 0,									// CYRILLIC SMALL LETTER HARD SIGN
	/* ??? */ 0x044B,  32, 0,									// CYRILLIC SMALL LETTER YERU
	/* ??? */ 0x044C,  52, 0,									// CYRILLIC SMALL LETTER SOFT SIGN
	/* ??? */ 0x044D,  41, 0,									// CYRILLIC SMALL LETTER E
	/* ??? */ 0x044E,  54, 0,									// CYRILLIC SMALL LETTER YU
	/* ??? */ 0x044F,  46, 0,									// CYRILLIC SMALL LETTER YA
	/* ??? */ 0x0451,   1, 0,									// CYRILLIC SMALL LETTER IO
	/* ??? */ 0x04A2,  22, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER EN WITH DESCENDER
	/* ??? */ 0x04A3,  22, MO_AltRight,						// CYRILLIC SMALL LETTER EN WITH DESCENDER
	/* ??? */ 0x04AE,  19, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER STRAIGHT U
	/* ??? */ 0x04AF,  19, MO_AltRight,						// CYRILLIC SMALL LETTER STRAIGHT U
	/* ??? */ 0x04E8,  37, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER BARRED O
	/* ??? */ 0x04E9,  37, MO_AltRight,						// CYRILLIC SMALL LETTER BARRED O
	/* ??? */ 0x2116,   4, MO_ShiftLeft,					// NUMERO SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Azeri_Latin[39] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023, NOS, 0,
	/* '$' */ 0x0024, NOS, 0,
	/* '&' */ 0x0026, NOS, 0,
	/* ''' */ 0x0027, NOS, 0,
	/* ',' */ 0x002C,  55, MO_ShiftLeft,
	/* '.' */ 0x002E,  55, 0,
	/* '/' */ 0x002F,  29, MO_ShiftLeft,
	/* ':' */ 0x003A,   7, MO_ShiftLeft,
	/* ';' */ 0x003B,   5, MO_ShiftLeft,
	/* '<' */ 0x003C, NOS, 0,
	/* '>' */ 0x003E, NOS, 0,
	/* '?' */ 0x003F,   8, MO_ShiftLeft,
	/* '@' */ 0x0040, NOS, 0,
	/* 'I' */ 0x0049,  40, MO_ShiftLeft,
	/* 'W' */ 0x0057, NOS, 0,
	/* '[' */ 0x005B, NOS, 0,
	/* ']' */ 0x005D, NOS, 0,
	/* '^' */ 0x005E, NOS, 0,
	/* 'w' */ 0x0077, NOS, 0,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D, NOS, 0,
	/* 'Ç' */ 0x00C7,  53, MO_ShiftLeft,
	/* 'Ö' */ 0x00D6,  27, MO_ShiftLeft,
	/* 'Ü' */ 0x00DC,  18, MO_ShiftLeft,
	/* 'ç' */ 0x00E7,  53, 0,
	/* 'ö' */ 0x00F6,  27, 0,
	/* 'ü' */ 0x00FC,  18, 0,
	/* ??? */ 0x011E,  28, MO_ShiftLeft,					// LATIN CAPITAL LETTER G WITH BREVE
	/* ??? */ 0x011F,  28, 0,									// LATIN SMALL LETTER G WITH BREVE
	/* ??? */ 0x0130,  24, MO_ShiftLeft,					// LATIN CAPITAL LETTER I WITH DOT ABOVE
	/* ??? */ 0x0131,  40, 0,									// LATIN SMALL LETTER DOTLESS I
	/* ??? */ 0x015E,  54, MO_ShiftLeft,					// LATIN CAPITAL LETTER S WITH CEDILLA
	/* ??? */ 0x015F,  54, 0,									// LATIN SMALL LETTER S WITH CEDILLA
	/* ??? */ 0x018F,  41, MO_ShiftLeft,					// LATIN CAPITAL LETTER SCHWA
	/* ??? */ 0x0259,  41, 0,									// LATIN SMALL LETTER SCHWA
	/* ??? */ 0x2166,   4, MO_ShiftLeft,					// ROMAN NUMERAL SEVEN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Azeri_Cyrillic[138] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023, NOS, 0,
	/* '$' */ 0x0024, NOS, 0,
	/* '&' */ 0x0026, NOS, 0,
	/* ''' */ 0x0027, NOS, 0,
	/* ',' */ 0x002C,  55, MO_ShiftLeft,
	/* '.' */ 0x002E,  55, 0,
	/* '/' */ 0x002F,  29, MO_ShiftLeft,
	/* ':' */ 0x003A,   7, MO_ShiftLeft,
	/* ';' */ 0x003B,   5, MO_ShiftLeft,
	/* '<' */ 0x003C, NOS, 0,
	/* '>' */ 0x003E, NOS, 0,
	/* '?' */ 0x003F,   8, MO_ShiftLeft,
	/* '@' */ 0x0040, NOS, 0,
	/* 'A' */ 0x0041, NOS, 0,
	/* 'B' */ 0x0042, NOS, 0,
	/* 'C' */ 0x0043, NOS, 0,
	/* 'D' */ 0x0044, NOS, 0,
	/* 'E' */ 0x0045, NOS, 0,
	/* 'F' */ 0x0046, NOS, 0,
	/* 'G' */ 0x0047, NOS, 0,
	/* 'H' */ 0x0048, NOS, 0,
	/* 'I' */ 0x0049, NOS, 0,
	/* 'J' */ 0x004A, NOS, 0,
	/* 'K' */ 0x004B, NOS, 0,
	/* 'L' */ 0x004C, NOS, 0,
	/* 'M' */ 0x004D, NOS, 0,
	/* 'N' */ 0x004E, NOS, 0,
	/* 'O' */ 0x004F, NOS, 0,
	/* 'P' */ 0x0050, NOS, 0,
	/* 'Q' */ 0x0051, NOS, 0,
	/* 'R' */ 0x0052, NOS, 0,
	/* 'S' */ 0x0053, NOS, 0,
	/* 'T' */ 0x0054, NOS, 0,
	/* 'U' */ 0x0055, NOS, 0,
	/* 'V' */ 0x0056, NOS, 0,
	/* 'W' */ 0x0057, NOS, 0,
	/* 'X' */ 0x0058, NOS, 0,
	/* 'Y' */ 0x0059, NOS, 0,
	/* 'Z' */ 0x005A, NOS, 0,
	/* '[' */ 0x005B, NOS, 0,
	/* ']' */ 0x005D, NOS, 0,
	/* '^' */ 0x005E, NOS, 0,
	/* 'a' */ 0x0061, NOS, 0,
	/* 'b' */ 0x0062, NOS, 0,
	/* 'c' */ 0x0063, NOS, 0,
	/* 'd' */ 0x0064, NOS, 0,
	/* 'e' */ 0x0065, NOS, 0,
	/* 'f' */ 0x0066, NOS, 0,
	/* 'g' */ 0x0067, NOS, 0,
	/* 'h' */ 0x0068, NOS, 0,
	/* 'i' */ 0x0069, NOS, 0,
	/* 'j' */ 0x006A, NOS, 0,
	/* 'k' */ 0x006B, NOS, 0,
	/* 'l' */ 0x006C, NOS, 0,
	/* 'm' */ 0x006D, NOS, 0,
	/* 'n' */ 0x006E, NOS, 0,
	/* 'o' */ 0x006F, NOS, 0,
	/* 'p' */ 0x0070, NOS, 0,
	/* 'q' */ 0x0071, NOS, 0,
	/* 'r' */ 0x0072, NOS, 0,
	/* 's' */ 0x0073, NOS, 0,
	/* 't' */ 0x0074, NOS, 0,
	/* 'u' */ 0x0075, NOS, 0,
	/* 'v' */ 0x0076, NOS, 0,
	/* 'w' */ 0x0077, NOS, 0,
	/* 'x' */ 0x0078, NOS, 0,
	/* 'y' */ 0x0079, NOS, 0,
	/* 'z' */ 0x007A, NOS, 0,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C,  45, MO_ShiftLeft,
	/* '}' */ 0x007D, NOS, 0,
	/* ??? */ 0x0408,  17, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER JE
	/* ??? */ 0x0410,  34, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER A
	/* ??? */ 0x0411,  53, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER BE
	/* ??? */ 0x0412,  33, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER VE
	/* ??? */ 0x0413,  23, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER GHE
	/* ??? */ 0x0414,  39, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER DE
	/* ??? */ 0x0415,  21, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER IE
	/* ??? */ 0x0416,  40, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZHE
	/* ??? */ 0x0417,  26, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZE
	/* ??? */ 0x0418,  50, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER I
	/* ??? */ 0x041A,  20, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER KA
	/* ??? */ 0x041B,  38, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EL
	/* ??? */ 0x041C,  49, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EM
	/* ??? */ 0x041D,  22, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EN
	/* ??? */ 0x041E,  37, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER O
	/* ??? */ 0x041F,  35, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER PE
	/* ??? */ 0x0420,  36, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ER
	/* ??? */ 0x0421,  48, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ES
	/* ??? */ 0x0422,  51, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TE
	/* ??? */ 0x0423,  19, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER U
	/* ??? */ 0x0424,  31, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EF
	/* ??? */ 0x0425,  27, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER HA
	/* ??? */ 0x0427,  47, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER CHE
	/* ??? */ 0x0428,  24, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHA
	/* ??? */ 0x042B,  32, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YERU
	/* ??? */ 0x0430,  34, 0,									// CYRILLIC SMALL LETTER A
	/* ??? */ 0x0431,  53, 0,									// CYRILLIC SMALL LETTER BE
	/* ??? */ 0x0432,  33, 0,									// CYRILLIC SMALL LETTER VE
	/* ??? */ 0x0433,  23, 0,									// CYRILLIC SMALL LETTER GHE
	/* ??? */ 0x0434,  39, 0,									// CYRILLIC SMALL LETTER DE
	/* ??? */ 0x0435,  21, 0,									// CYRILLIC SMALL LETTER IE
	/* ??? */ 0x0436,  40, 0,									// CYRILLIC SMALL LETTER ZHE
	/* ??? */ 0x0437,  26, 0,									// CYRILLIC SMALL LETTER ZE
	/* ??? */ 0x0438,  50, 0,									// CYRILLIC SMALL LETTER I
	/* ??? */ 0x043A,  20, 0,									// CYRILLIC SMALL LETTER KA
	/* ??? */ 0x043B,  38, 0,									// CYRILLIC SMALL LETTER EL
	/* ??? */ 0x043C,  49, 0,									// CYRILLIC SMALL LETTER EM
	/* ??? */ 0x043D,  22, 0,									// CYRILLIC SMALL LETTER EN
	/* ??? */ 0x043E,  37, 0,									// CYRILLIC SMALL LETTER O
	/* ??? */ 0x043F,  35, 0,									// CYRILLIC SMALL LETTER PE
	/* ??? */ 0x0440,  36, 0,									// CYRILLIC SMALL LETTER ER
	/* ??? */ 0x0441,  48, 0,									// CYRILLIC SMALL LETTER ES
	/* ??? */ 0x0442,  51, 0,									// CYRILLIC SMALL LETTER TE
	/* ??? */ 0x0443,  19, 0,									// CYRILLIC SMALL LETTER U
	/* ??? */ 0x0444,  31, 0,									// CYRILLIC SMALL LETTER EF
	/* ??? */ 0x0445,  27, 0,									// CYRILLIC SMALL LETTER HA
	/* ??? */ 0x0447,  47, 0,									// CYRILLIC SMALL LETTER CHE
	/* ??? */ 0x0448,  24, 0,									// CYRILLIC SMALL LETTER SHA
	/* ??? */ 0x044B,  32, 0,									// CYRILLIC SMALL LETTER YERU
	/* ??? */ 0x0458,  17, 0,									// CYRILLIC SMALL LETTER JE
	/* ??? */ 0x0492,  52, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER GHE WITH STROKE
	/* ??? */ 0x0493,  52, 0,									// CYRILLIC SMALL LETTER GHE WITH STROKE
	/* ??? */ 0x049C,  41, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER KA WITH VERTICAL STROKE
	/* ??? */ 0x049D,  41, 0,									// CYRILLIC SMALL LETTER KA WITH VERTICAL STROKE
	/* ??? */ 0x04AE,  18, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER STRAIGHT U
	/* ??? */ 0x04AF,  18, 0,									// CYRILLIC SMALL LETTER STRAIGHT U
	/* ??? */ 0x04B8,  28, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER CHE WITH VERTICAL STROKE
	/* ??? */ 0x04B9,  28, 0,									// CYRILLIC SMALL LETTER CHE WITH VERTICAL STROKE
	/* ??? */ 0x04BA,  25, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHHA
	/* ??? */ 0x04BB,  25, 0,									// CYRILLIC SMALL LETTER SHHA
	/* ??? */ 0x04D8,  46, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SCHWA
	/* ??? */ 0x04D9,  46, 0,									// CYRILLIC SMALL LETTER SCHWA
	/* ??? */ 0x04E8,  54, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER BARRED O
	/* ??? */ 0x04E9,  54, 0,									// CYRILLIC SMALL LETTER BARRED O
	/* ??? */ 0x2116,   4, MO_ShiftLeft,					// NUMERO SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Belarusian[140] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023, NOS, 0,
	/* '$' */ 0x0024, NOS, 0,
	/* '&' */ 0x0026, NOS, 0,
	/* ''' */ 0x0027,  28, 0,
	/* ',' */ 0x002C,  55, MO_ShiftLeft,
	/* '.' */ 0x002E,  55, 0,
	/* '/' */ 0x002F,  29, MO_ShiftLeft,
	/* ':' */ 0x003A,   7, MO_ShiftLeft,
	/* ';' */ 0x003B,   5, MO_ShiftLeft,
	/* '<' */ 0x003C, NOS, 0,
	/* '>' */ 0x003E, NOS, 0,
	/* '?' */ 0x003F,   8, MO_ShiftLeft,
	/* '@' */ 0x0040, NOS, 0,
	/* 'A' */ 0x0041, NOS, 0,
	/* 'B' */ 0x0042, NOS, 0,
	/* 'C' */ 0x0043, NOS, 0,
	/* 'D' */ 0x0044, NOS, 0,
	/* 'E' */ 0x0045, NOS, 0,
	/* 'F' */ 0x0046, NOS, 0,
	/* 'G' */ 0x0047, NOS, 0,
	/* 'H' */ 0x0048, NOS, 0,
	/* 'I' */ 0x0049, NOS, 0,
	/* 'J' */ 0x004A, NOS, 0,
	/* 'K' */ 0x004B, NOS, 0,
	/* 'L' */ 0x004C, NOS, 0,
	/* 'M' */ 0x004D, NOS, 0,
	/* 'N' */ 0x004E, NOS, 0,
	/* 'O' */ 0x004F, NOS, 0,
	/* 'P' */ 0x0050, NOS, 0,
	/* 'Q' */ 0x0051, NOS, 0,
	/* 'R' */ 0x0052, NOS, 0,
	/* 'S' */ 0x0053, NOS, 0,
	/* 'T' */ 0x0054, NOS, 0,
	/* 'U' */ 0x0055, NOS, 0,
	/* 'V' */ 0x0056, NOS, 0,
	/* 'W' */ 0x0057, NOS, 0,
	/* 'X' */ 0x0058, NOS, 0,
	/* 'Y' */ 0x0059, NOS, 0,
	/* 'Z' */ 0x005A, NOS, 0,
	/* '[' */ 0x005B, NOS, 0,
	/* ']' */ 0x005D, NOS, 0,
	/* '^' */ 0x005E, NOS, 0,
	/* '`' */ 0x0060, NOS, 0,
	/* 'a' */ 0x0061, NOS, 0,
	/* 'b' */ 0x0062, NOS, 0,
	/* 'c' */ 0x0063, NOS, 0,
	/* 'd' */ 0x0064, NOS, 0,
	/* 'e' */ 0x0065, NOS, 0,
	/* 'f' */ 0x0066, NOS, 0,
	/* 'g' */ 0x0067, NOS, 0,
	/* 'h' */ 0x0068, NOS, 0,
	/* 'i' */ 0x0069, NOS, 0,
	/* 'j' */ 0x006A, NOS, 0,
	/* 'k' */ 0x006B, NOS, 0,
	/* 'l' */ 0x006C, NOS, 0,
	/* 'm' */ 0x006D, NOS, 0,
	/* 'n' */ 0x006E, NOS, 0,
	/* 'o' */ 0x006F, NOS, 0,
	/* 'p' */ 0x0070, NOS, 0,
	/* 'q' */ 0x0071, NOS, 0,
	/* 'r' */ 0x0072, NOS, 0,
	/* 's' */ 0x0073, NOS, 0,
	/* 't' */ 0x0074, NOS, 0,
	/* 'u' */ 0x0075, NOS, 0,
	/* 'v' */ 0x0076, NOS, 0,
	/* 'w' */ 0x0077, NOS, 0,
	/* 'x' */ 0x0078, NOS, 0,
	/* 'y' */ 0x0079, NOS, 0,
	/* 'z' */ 0x007A, NOS, 0,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* ??? */ 0x0401,   1, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER IO
	/* ??? */ 0x0406,  50, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I
	/* ??? */ 0x040E,  25, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHORT U
	/* ??? */ 0x0410,  34, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER A
	/* ??? */ 0x0411,  53, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER BE
	/* ??? */ 0x0412,  33, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER VE
	/* ??? */ 0x0413,  23, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER GHE
	/* ??? */ 0x0414,  39, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER DE
	/* ??? */ 0x0415,  21, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER IE
	/* ??? */ 0x0416,  40, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZHE
	/* ??? */ 0x0417,  26, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZE
	/* ??? */ 0x0419,  17, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHORT I
	/* ??? */ 0x041A,  20, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER KA
	/* ??? */ 0x041B,  38, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EL
	/* ??? */ 0x041C,  49, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EM
	/* ??? */ 0x041D,  22, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EN
	/* ??? */ 0x041E,  37, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER O
	/* ??? */ 0x041F,  35, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER PE
	/* ??? */ 0x0420,  36, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ER
	/* ??? */ 0x0421,  48, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ES
	/* ??? */ 0x0422,  51, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TE
	/* ??? */ 0x0423,  19, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER U
	/* ??? */ 0x0424,  31, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EF
	/* ??? */ 0x0425,  27, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER HA
	/* ??? */ 0x0426,  18, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TSE
	/* ??? */ 0x0427,  47, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER CHE
	/* ??? */ 0x0428,  24, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHA
	/* ??? */ 0x042B,  32, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YERU
	/* ??? */ 0x042C,  52, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SOFT SIGN
	/* ??? */ 0x042D,  41, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER E
	/* ??? */ 0x042E,  54, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YU
	/* ??? */ 0x042F,  46, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YA
	/* ??? */ 0x0430,  34, 0,									// CYRILLIC SMALL LETTER A
	/* ??? */ 0x0431,  53, 0,									// CYRILLIC SMALL LETTER BE
	/* ??? */ 0x0432,  33, 0,									// CYRILLIC SMALL LETTER VE
	/* ??? */ 0x0433,  23, 0,									// CYRILLIC SMALL LETTER GHE
	/* ??? */ 0x0434,  39, 0,									// CYRILLIC SMALL LETTER DE
	/* ??? */ 0x0435,  21, 0,									// CYRILLIC SMALL LETTER IE
	/* ??? */ 0x0436,  40, 0,									// CYRILLIC SMALL LETTER ZHE
	/* ??? */ 0x0437,  26, 0,									// CYRILLIC SMALL LETTER ZE
	/* ??? */ 0x0439,  17, 0,									// CYRILLIC SMALL LETTER SHORT I
	/* ??? */ 0x043A,  20, 0,									// CYRILLIC SMALL LETTER KA
	/* ??? */ 0x043B,  38, 0,									// CYRILLIC SMALL LETTER EL
	/* ??? */ 0x043C,  49, 0,									// CYRILLIC SMALL LETTER EM
	/* ??? */ 0x043D,  22, 0,									// CYRILLIC SMALL LETTER EN
	/* ??? */ 0x043E,  37, 0,									// CYRILLIC SMALL LETTER O
	/* ??? */ 0x043F,  35, 0,									// CYRILLIC SMALL LETTER PE
	/* ??? */ 0x0440,  36, 0,									// CYRILLIC SMALL LETTER ER
	/* ??? */ 0x0441,  48, 0,									// CYRILLIC SMALL LETTER ES
	/* ??? */ 0x0442,  51, 0,									// CYRILLIC SMALL LETTER TE
	/* ??? */ 0x0443,  19, 0,									// CYRILLIC SMALL LETTER U
	/* ??? */ 0x0444,  31, 0,									// CYRILLIC SMALL LETTER EF
	/* ??? */ 0x0445,  27, 0,									// CYRILLIC SMALL LETTER HA
	/* ??? */ 0x0446,  18, 0,									// CYRILLIC SMALL LETTER TSE
	/* ??? */ 0x0447,  47, 0,									// CYRILLIC SMALL LETTER CHE
	/* ??? */ 0x0448,  24, 0,									// CYRILLIC SMALL LETTER SHA
	/* ??? */ 0x044B,  32, 0,									// CYRILLIC SMALL LETTER YERU
	/* ??? */ 0x044C,  52, 0,									// CYRILLIC SMALL LETTER SOFT SIGN
	/* ??? */ 0x044D,  41, 0,									// CYRILLIC SMALL LETTER E
	/* ??? */ 0x044E,  54, 0,									// CYRILLIC SMALL LETTER YU
	/* ??? */ 0x044F,  46, 0,									// CYRILLIC SMALL LETTER YA
	/* ??? */ 0x0451,   1, 0,									// CYRILLIC SMALL LETTER IO
	/* ??? */ 0x0456,  50, 0,									// CYRILLIC SMALL LETTER BYELORUSSIAN-UKRAINIAN I
	/* ??? */ 0x045E,  25, 0,									// CYRILLIC SMALL LETTER SHORT U
	/* ??? */ 0x2116,   4, MO_ShiftLeft,					// NUMERO SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Faeroese[44] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '$' */ 0x0024,   5, MO_AltRight,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,  42, 0,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  42, MO_ShiftLeft,
	/* '+' */ 0x002B,  12, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_AltRight,
	/* '[' */ 0x005B,   9, MO_AltRight,
	/* '\' */ 0x005C,  45, MO_AltRight,
	/* ']' */ 0x005D,  10, MO_AltRight,
	/* '^' */ 0x005E,  41, MO_AltRight|MO_DeadKey,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,  13, MO_ShiftLeft|MO_DeadKey,
	/* '{' */ 0x007B,   8, MO_AltRight,
	/* '|' */ 0x007C,  13, MO_AltRight,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* '~' */ 0x007E,  28, MO_AltRight|MO_DeadKey,
	/* '£' */ 0x00A3,   4, MO_AltRight,
	/* '¤' */ 0x00A4,   5, MO_ShiftLeft,
	/* '§' */ 0x00A7,   1, MO_ShiftLeft,
	/* '¨' */ 0x00A8,  27, MO_AltRight|MO_DeadKey,
	/* '´' */ 0x00B4,  13, MO_DeadKey,
	/* 'µ' */ 0x00B5,  52, MO_AltRight,
	/* '½' */ 0x00BD,   1, 0,
	/* 'Å' */ 0x00C5,  27, MO_ShiftLeft,
	/* 'Æ' */ 0x00C6,  40, MO_ShiftLeft,
	/* 'Ð' */ 0x00D0,  28, MO_ShiftLeft,
	/* 'Ø' */ 0x00D8,  41, MO_ShiftLeft,
	/* 'å' */ 0x00E5,  27, 0,
	/* 'æ' */ 0x00E6,  40, 0,
	/* 'ð' */ 0x00F0,  28, 0,
	/* 'ø' */ 0x00F8,  41, 0,
	/* '€' */ 0x20AC,   6, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Gaelic[25] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023,  42, 0,
	/* ''' */ 0x0027,  41, MO_DeadKey,
	/* '@' */ 0x0040,  41, MO_ShiftLeft,
	/* '\' */ 0x005C,  45, 0,
	/* '`' */ 0x0060,   1, MO_DeadKey,
	/* '|' */ 0x007C,  45, MO_ShiftLeft,
	/* '~' */ 0x007E,  42, MO_ShiftLeft,
	/* '£' */ 0x00A3,   4, MO_ShiftLeft,
	/* '¦' */ 0x00A6,   1, MO_AltRight,
	/* '¬' */ 0x00AC,   1, MO_ShiftLeft|MO_AltRight,
	/* 'Á' */ 0x00C1,  31, MO_ShiftLeft|MO_AltRight,
	/* 'É' */ 0x00C9,  19, MO_ShiftLeft|MO_AltRight,
	/* 'Í' */ 0x00CD,  24, MO_ShiftLeft|MO_AltRight,
	/* 'Ó' */ 0x00D3,  25, MO_ShiftLeft|MO_AltRight,
	/* 'Ú' */ 0x00DA,  23, MO_ShiftLeft|MO_AltRight,
	/* 'Ý' */ 0x00DD,  22, MO_ShiftLeft|MO_AltRight,
	/* 'á' */ 0x00E1,  31, MO_AltRight,
	/* 'é' */ 0x00E9,  19, MO_AltRight,
	/* 'í' */ 0x00ED,  24, MO_AltRight,
	/* 'ó' */ 0x00F3,  25, MO_AltRight,
	/* 'ú' */ 0x00FA,  23, MO_AltRight,
	/* 'ý' */ 0x00FD,  22, MO_AltRight,
	/* '€' */ 0x20AC,   5, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Tatar[157] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023,   4, MO_AltRight,
	/* '$' */ 0x0024,   5, MO_AltRight,
	/* '&' */ 0x0026, NOS, 0,
	/* ''' */ 0x0027,  41, MO_AltRight,
	/* ',' */ 0x002C,  55, MO_ShiftLeft,
	/* '.' */ 0x002E,  55, 0,
	/* '/' */ 0x002F,  42, MO_ShiftLeft,
	/* ':' */ 0x003A,   7, MO_ShiftLeft,
	/* ';' */ 0x003B,   5, MO_ShiftLeft,
	/* '<' */ 0x003C,  53, MO_AltRight,
	/* '>' */ 0x003E,  54, MO_AltRight,
	/* '?' */ 0x003F,   8, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_AltRight,
	/* 'A' */ 0x0041, NOS, 0,
	/* 'B' */ 0x0042, NOS, 0,
	/* 'C' */ 0x0043, NOS, 0,
	/* 'D' */ 0x0044, NOS, 0,
	/* 'E' */ 0x0045, NOS, 0,
	/* 'F' */ 0x0046, NOS, 0,
	/* 'G' */ 0x0047, NOS, 0,
	/* 'H' */ 0x0048, NOS, 0,
	/* 'I' */ 0x0049, NOS, 0,
	/* 'J' */ 0x004A, NOS, 0,
	/* 'K' */ 0x004B, NOS, 0,
	/* 'L' */ 0x004C, NOS, 0,
	/* 'M' */ 0x004D, NOS, 0,
	/* 'N' */ 0x004E, NOS, 0,
	/* 'O' */ 0x004F, NOS, 0,
	/* 'P' */ 0x0050, NOS, 0,
	/* 'Q' */ 0x0051, NOS, 0,
	/* 'R' */ 0x0052, NOS, 0,
	/* 'S' */ 0x0053, NOS, 0,
	/* 'T' */ 0x0054, NOS, 0,
	/* 'U' */ 0x0055, NOS, 0,
	/* 'V' */ 0x0056, NOS, 0,
	/* 'W' */ 0x0057, NOS, 0,
	/* 'X' */ 0x0058, NOS, 0,
	/* 'Y' */ 0x0059, NOS, 0,
	/* 'Z' */ 0x005A, NOS, 0,
	/* '[' */ 0x005B,   8, MO_AltRight,
	/* '\' */ 0x005C,  42, 0,
	/* ']' */ 0x005D,   9, MO_AltRight,
	/* '^' */ 0x005E, NOS, 0,
	/* '`' */ 0x0060, NOS, 0,
	/* 'a' */ 0x0061, NOS, 0,
	/* 'b' */ 0x0062, NOS, 0,
	/* 'c' */ 0x0063, NOS, 0,
	/* 'd' */ 0x0064, NOS, 0,
	/* 'e' */ 0x0065, NOS, 0,
	/* 'f' */ 0x0066, NOS, 0,
	/* 'g' */ 0x0067, NOS, 0,
	/* 'h' */ 0x0068, NOS, 0,
	/* 'i' */ 0x0069, NOS, 0,
	/* 'j' */ 0x006A, NOS, 0,
	/* 'k' */ 0x006B, NOS, 0,
	/* 'l' */ 0x006C, NOS, 0,
	/* 'm' */ 0x006D, NOS, 0,
	/* 'n' */ 0x006E, NOS, 0,
	/* 'o' */ 0x006F, NOS, 0,
	/* 'p' */ 0x0070, NOS, 0,
	/* 'q' */ 0x0071, NOS, 0,
	/* 'r' */ 0x0072, NOS, 0,
	/* 's' */ 0x0073, NOS, 0,
	/* 't' */ 0x0074, NOS, 0,
	/* 'u' */ 0x0075, NOS, 0,
	/* 'v' */ 0x0076, NOS, 0,
	/* 'w' */ 0x0077, NOS, 0,
	/* 'x' */ 0x0078, NOS, 0,
	/* 'y' */ 0x0079, NOS, 0,
	/* 'z' */ 0x007A, NOS, 0,
	/* '{' */ 0x007B,  10, MO_AltRight,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* '~' */ 0x007E, NOS, 0,
	/* ??? */ 0x0401,   1, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER IO
	/* ??? */ 0x0410,  34, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER A
	/* ??? */ 0x0411,  53, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER BE
	/* ??? */ 0x0412,  33, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER VE
	/* ??? */ 0x0413,  23, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER GHE
	/* ??? */ 0x0414,  39, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER DE
	/* ??? */ 0x0415,  21, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER IE
	/* ??? */ 0x0416,  40, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER ZHE
	/* ??? */ 0x0417,  26, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZE
	/* ??? */ 0x0418,  50, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER I
	/* ??? */ 0x0419,  17, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHORT I
	/* ??? */ 0x041A,  20, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER KA
	/* ??? */ 0x041B,  38, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EL
	/* ??? */ 0x041C,  49, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EM
	/* ??? */ 0x041D,  22, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EN
	/* ??? */ 0x041E,  37, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER O
	/* ??? */ 0x041F,  35, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER PE
	/* ??? */ 0x0420,  36, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ER
	/* ??? */ 0x0421,  48, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ES
	/* ??? */ 0x0422,  51, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TE
	/* ??? */ 0x0423,  19, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER U
	/* ??? */ 0x0424,  31, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EF
	/* ??? */ 0x0425,  27, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER HA
	/* ??? */ 0x0426,  18, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER TSE
	/* ??? */ 0x0427,  47, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER CHE
	/* ??? */ 0x0428,  24, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHA
	/* ??? */ 0x0429,  25, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER SHCHA
	/* ??? */ 0x042A,  28, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER HARD SIGN
	/* ??? */ 0x042B,  32, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YERU
	/* ??? */ 0x042C,  52, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER SOFT SIGN
	/* ??? */ 0x042D,  41, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER E
	/* ??? */ 0x042E,  54, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YU
	/* ??? */ 0x042F,  46, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YA
	/* ??? */ 0x0430,  34, 0,									// CYRILLIC SMALL LETTER A
	/* ??? */ 0x0431,  53, 0,									// CYRILLIC SMALL LETTER BE
	/* ??? */ 0x0432,  33, 0,									// CYRILLIC SMALL LETTER VE
	/* ??? */ 0x0433,  23, 0,									// CYRILLIC SMALL LETTER GHE
	/* ??? */ 0x0434,  39, 0,									// CYRILLIC SMALL LETTER DE
	/* ??? */ 0x0435,  21, 0,									// CYRILLIC SMALL LETTER IE
	/* ??? */ 0x0436,  40, MO_AltRight,						// CYRILLIC SMALL LETTER ZHE
	/* ??? */ 0x0437,  26, 0,									// CYRILLIC SMALL LETTER ZE
	/* ??? */ 0x0438,  50, 0,									// CYRILLIC SMALL LETTER I
	/* ??? */ 0x0439,  17, 0,									// CYRILLIC SMALL LETTER SHORT I
	/* ??? */ 0x043A,  20, 0,									// CYRILLIC SMALL LETTER KA
	/* ??? */ 0x043B,  38, 0,									// CYRILLIC SMALL LETTER EL
	/* ??? */ 0x043C,  49, 0,									// CYRILLIC SMALL LETTER EM
	/* ??? */ 0x043D,  22, 0,									// CYRILLIC SMALL LETTER EN
	/* ??? */ 0x043E,  37, 0,									// CYRILLIC SMALL LETTER O
	/* ??? */ 0x043F,  35, 0,									// CYRILLIC SMALL LETTER PE
	/* ??? */ 0x0440,  36, 0,									// CYRILLIC SMALL LETTER ER
	/* ??? */ 0x0441,  48, 0,									// CYRILLIC SMALL LETTER ES
	/* ??? */ 0x0442,  51, 0,									// CYRILLIC SMALL LETTER TE
	/* ??? */ 0x0443,  19, 0,									// CYRILLIC SMALL LETTER U
	/* ??? */ 0x0444,  31, 0,									// CYRILLIC SMALL LETTER EF
	/* ??? */ 0x0445,  27, 0,									// CYRILLIC SMALL LETTER HA
	/* ??? */ 0x0446,  18, MO_AltRight,						// CYRILLIC SMALL LETTER TSE
	/* ??? */ 0x0447,  47, 0,									// CYRILLIC SMALL LETTER CHE
	/* ??? */ 0x0448,  24, 0,									// CYRILLIC SMALL LETTER SHA
	/* ??? */ 0x0449,  25, MO_AltRight,						// CYRILLIC SMALL LETTER SHCHA
	/* ??? */ 0x044A,  28, MO_AltRight,						// CYRILLIC SMALL LETTER HARD SIGN
	/* ??? */ 0x044B,  32, 0,									// CYRILLIC SMALL LETTER YERU
	/* ??? */ 0x044C,  52, MO_AltRight,						// CYRILLIC SMALL LETTER SOFT SIGN
	/* ??? */ 0x044D,  41, 0,									// CYRILLIC SMALL LETTER E
	/* ??? */ 0x044E,  54, 0,									// CYRILLIC SMALL LETTER YU
	/* ??? */ 0x044F,  46, 0,									// CYRILLIC SMALL LETTER YA
	/* ??? */ 0x0451,   1, MO_AltRight,						// CYRILLIC SMALL LETTER IO
	/* ??? */ 0x0490,  45, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER GHE WITH UPTURN
	/* ??? */ 0x0491,  45, 0,									// CYRILLIC SMALL LETTER GHE WITH UPTURN
	/* ??? */ 0x0496,  52, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZHE WITH DESCENDER
	/* ??? */ 0x0497,  52, 0,									// CYRILLIC SMALL LETTER ZHE WITH DESCENDER
	/* ??? */ 0x04A2,  40, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EN WITH DESCENDER
	/* ??? */ 0x04A3,  40, 0,									// CYRILLIC SMALL LETTER EN WITH DESCENDER
	/* ??? */ 0x04AE,  28, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER STRAIGHT U
	/* ??? */ 0x04AF,  28, 0,									// CYRILLIC SMALL LETTER STRAIGHT U
	/* ??? */ 0x04BA,   1, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHHA
	/* ??? */ 0x04BB,   1, 0,									// CYRILLIC SMALL LETTER SHHA
	/* ??? */ 0x04D8,  25, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SCHWA
	/* ??? */ 0x04D9,  25, 0,									// CYRILLIC SMALL LETTER SCHWA
	/* ??? */ 0x04E8,  18, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER BARRED O
	/* ??? */ 0x04E9,  18, 0,									// CYRILLIC SMALL LETTER BARRED O
	/* ??? */ 0x2116,   4, MO_ShiftLeft,					// NUMERO SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  5
const CKeyFixup DEC_VT510_PC_Mongolian_Cyrillic[165] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021, NOS, 0,
	/* '"' */ 0x0022,   4, 0,
	/* '#' */ 0x0023, NOS, 0,
	/* '$' */ 0x0024, NOS, 0,
	/* '%' */ 0x0025,  10, 0,
	/* '&' */ 0x0026, NOS, 0,
	/* ''' */ 0x0027, NOS, 0,
	/* '(' */ 0x0028, NOS, 0,
	/* ')' */ 0x0029, NOS, 0,
	/* '*' */ 0x002A, NOS, 0,
	/* '+' */ 0x002B,   1, MO_ShiftLeft,
	/* ',' */ 0x002C,   9, 0,
	/* '-' */ 0x002D,   3, 0,
	/* '.' */ 0x002E,   7, 0,
	/* '/' */ 0x002F, NOS, 0,
	/* '0' */ 0x0030,  11, MO_ShiftLeft,
	/* '1' */ 0x0031,   2, MO_ShiftLeft,
	/* '2' */ 0x0032,   3, MO_ShiftLeft,
	/* '3' */ 0x0033,   4, MO_ShiftLeft,
	/* '4' */ 0x0034,   5, MO_ShiftLeft,
	/* '5' */ 0x0035,   6, MO_ShiftLeft,
	/* '6' */ 0x0036,   7, MO_ShiftLeft,
	/* '7' */ 0x0037,   8, MO_ShiftLeft,
	/* '8' */ 0x0038,   9, MO_ShiftLeft,
	/* '9' */ 0x0039,  10, MO_ShiftLeft,
	/* ':' */ 0x003A,   6, 0,
	/* ';' */ 0x003B, NOS, 0,
	/* '<' */ 0x003C, NOS, 0,
	/* '=' */ 0x003D,   1, 0,
	/* '>' */ 0x003E, NOS, 0,
	/* '?' */ 0x003F,  11, 0,
	/* '@' */ 0x0040, NOS, 0,
	/* 'A' */ 0x0041, NOS, 0,
	/* 'B' */ 0x0042, NOS, 0,
	/* 'C' */ 0x0043, NOS, 0,
	/* 'D' */ 0x0044, NOS, 0,
	/* 'E' */ 0x0045, NOS, 0,
	/* 'F' */ 0x0046, NOS, 0,
	/* 'G' */ 0x0047, NOS, 0,
	/* 'H' */ 0x0048, NOS, 0,
	/* 'I' */ 0x0049, NOS, 0,
	/* 'J' */ 0x004A, NOS, 0,
	/* 'K' */ 0x004B, NOS, 0,
	/* 'L' */ 0x004C, NOS, 0,
	/* 'M' */ 0x004D, NOS, 0,
	/* 'N' */ 0x004E, NOS, 0,
	/* 'O' */ 0x004F, NOS, 0,
	/* 'P' */ 0x0050, NOS, 0,
	/* 'Q' */ 0x0051, NOS, 0,
	/* 'R' */ 0x0052, NOS, 0,
	/* 'S' */ 0x0053, NOS, 0,
	/* 'T' */ 0x0054, NOS, 0,
	/* 'U' */ 0x0055, NOS, 0,
	/* 'V' */ 0x0056, NOS, 0,
	/* 'W' */ 0x0057, NOS, 0,
	/* 'X' */ 0x0058, NOS, 0,
	/* 'Y' */ 0x0059, NOS, 0,
	/* 'Z' */ 0x005A, NOS, 0,
	/* '[' */ 0x005B, NOS, 0,
	/* ']' */ 0x005D, NOS, 0,
	/* '^' */ 0x005E, NOS, 0,
	/* '_' */ 0x005F,   8, 0,
	/* '`' */ 0x0060, NOS, 0,
	/* 'a' */ 0x0061, NOS, 0,
	/* 'b' */ 0x0062, NOS, 0,
	/* 'c' */ 0x0063, NOS, 0,
	/* 'd' */ 0x0064, NOS, 0,
	/* 'e' */ 0x0065, NOS, 0,
	/* 'f' */ 0x0066, NOS, 0,
	/* 'g' */ 0x0067, NOS, 0,
	/* 'h' */ 0x0068, NOS, 0,
	/* 'i' */ 0x0069, NOS, 0,
	/* 'j' */ 0x006A, NOS, 0,
	/* 'k' */ 0x006B, NOS, 0,
	/* 'l' */ 0x006C, NOS, 0,
	/* 'm' */ 0x006D, NOS, 0,
	/* 'n' */ 0x006E, NOS, 0,
	/* 'o' */ 0x006F, NOS, 0,
	/* 'p' */ 0x0070, NOS, 0,
	/* 'q' */ 0x0071, NOS, 0,
	/* 'r' */ 0x0072, NOS, 0,
	/* 's' */ 0x0073, NOS, 0,
	/* 't' */ 0x0074, NOS, 0,
	/* 'u' */ 0x0075, NOS, 0,
	/* 'v' */ 0x0076, NOS, 0,
	/* 'w' */ 0x0077, NOS, 0,
	/* 'x' */ 0x0078, NOS, 0,
	/* 'y' */ 0x0079, NOS, 0,
	/* 'z' */ 0x007A, NOS, 0,
	/* '{' */ 0x007B, NOS, 0,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* ??? */ 0x0401,  48, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER IO
	/* ??? */ 0x0410,  35, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER A
	/* ??? */ 0x0411,  33, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER BE
	/* ??? */ 0x0412,  54, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER VE
	/* ??? */ 0x0413,  23, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER GHE
	/* ??? */ 0x0414,  40, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER DE
	/* ??? */ 0x0415,  12, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER IE
	/* ??? */ 0x0416,  20, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZHE
	/* ??? */ 0x0417,  26, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ZE
	/* ??? */ 0x0418,  51, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER I
	/* ??? */ 0x0419,  31, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHORT I
	/* ??? */ 0x041A,  27, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER KA
	/* ??? */ 0x041B,  39, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EL
	/* ??? */ 0x041C,  50, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EM
	/* ??? */ 0x041D,  22, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EN
	/* ??? */ 0x041E,  38, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER O
	/* ??? */ 0x041F,  41, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER PE
	/* ??? */ 0x0420,  37, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ER
	/* ??? */ 0x0421,  49, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER ES
	/* ??? */ 0x0422,  52, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TE
	/* ??? */ 0x0423,  19, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER U
	/* ??? */ 0x0424,  17, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER EF
	/* ??? */ 0x0425,  36, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER HA
	/* ??? */ 0x0426,  18, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER TSE
	/* ??? */ 0x0427,  47, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER CHE
	/* ??? */ 0x0428,  24, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHA
	/* ??? */ 0x0429,  13, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SHCHA
	/* ??? */ 0x042A,  28, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER HARD SIGN
	/* ??? */ 0x042B,  32, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YERU
	/* ??? */ 0x042C,  53, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER SOFT SIGN
	/* ??? */ 0x042D,  21, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER E
	/* ??? */ 0x042E,  55, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YU
	/* ??? */ 0x042F,  46, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER YA
	/* ??? */ 0x0430,  35, 0,									// CYRILLIC SMALL LETTER A
	/* ??? */ 0x0431,  33, 0,									// CYRILLIC SMALL LETTER BE
	/* ??? */ 0x0432,  54, 0,									// CYRILLIC SMALL LETTER VE
	/* ??? */ 0x0433,  23, 0,									// CYRILLIC SMALL LETTER GHE
	/* ??? */ 0x0434,  40, 0,									// CYRILLIC SMALL LETTER DE
	/* ??? */ 0x0435,  12, 0,									// CYRILLIC SMALL LETTER IE
	/* ??? */ 0x0436,  20, 0,									// CYRILLIC SMALL LETTER ZHE
	/* ??? */ 0x0437,  26, 0,									// CYRILLIC SMALL LETTER ZE
	/* ??? */ 0x0438,  51, 0,									// CYRILLIC SMALL LETTER I
	/* ??? */ 0x0439,  31, 0,									// CYRILLIC SMALL LETTER SHORT I
	/* ??? */ 0x043A,  27, 0,									// CYRILLIC SMALL LETTER KA
	/* ??? */ 0x043B,  39, 0,									// CYRILLIC SMALL LETTER EL
	/* ??? */ 0x043C,  50, 0,									// CYRILLIC SMALL LETTER EM
	/* ??? */ 0x043D,  22, 0,									// CYRILLIC SMALL LETTER EN
	/* ??? */ 0x043E,  38, 0,									// CYRILLIC SMALL LETTER O
	/* ??? */ 0x043F,  41, 0,									// CYRILLIC SMALL LETTER PE
	/* ??? */ 0x0440,  37, 0,									// CYRILLIC SMALL LETTER ER
	/* ??? */ 0x0441,  49, 0,									// CYRILLIC SMALL LETTER ES
	/* ??? */ 0x0442,  52, 0,									// CYRILLIC SMALL LETTER TE
	/* ??? */ 0x0443,  19, 0,									// CYRILLIC SMALL LETTER U
	/* ??? */ 0x0444,  17, 0,									// CYRILLIC SMALL LETTER EF
	/* ??? */ 0x0445,  36, 0,									// CYRILLIC SMALL LETTER HA
	/* ??? */ 0x0446,  18, 0,									// CYRILLIC SMALL LETTER TSE
	/* ??? */ 0x0447,  47, 0,									// CYRILLIC SMALL LETTER CHE
	/* ??? */ 0x0448,  24, 0,									// CYRILLIC SMALL LETTER SHA
	/* ??? */ 0x0449,  13, 0,									// CYRILLIC SMALL LETTER SHCHA
	/* ??? */ 0x044A,  28, 0,									// CYRILLIC SMALL LETTER HARD SIGN
	/* ??? */ 0x044B,  32, 0,									// CYRILLIC SMALL LETTER YERU
	/* ??? */ 0x044C,  53, 0,									// CYRILLIC SMALL LETTER SOFT SIGN
	/* ??? */ 0x044D,  21, 0,									// CYRILLIC SMALL LETTER E
	/* ??? */ 0x044E,  55, 0,									// CYRILLIC SMALL LETTER YU
	/* ??? */ 0x044F,  46, 0,									// CYRILLIC SMALL LETTER YA
	/* ??? */ 0x0451,  48, 0,									// CYRILLIC SMALL LETTER IO
	/* ??? */ 0x04AE,  25, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER STRAIGHT U
	/* ??? */ 0x04AF,  25, 0,									// CYRILLIC SMALL LETTER STRAIGHT U
	/* ??? */ 0x04E8,  34, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER BARRED O
	/* ??? */ 0x04E9,  34, 0,									// CYRILLIC SMALL LETTER BARRED O
	/* ??? */ 0x20AE,   5, 0,									// TUGRIK SIGN
	/* ??? */ 0x2116,   2, 0,									// NUMERO SIGN
	/* end */ 0,KN_ENDMARKER,0
};

#define IBM_3486_102_USA                      		NoExceptions	// not required

// TermId=  6 	, 45 , 84
const CKeyFixup IBM_3486_102_Belgian[3] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* ... */ 0x007F,  76, 0,									// <control> (DELETE)
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// The next table is equal to another, so we save the memory
#define IBM_3486_102_Finnish_Swedish  		IBM_3486_102_Belgian

// TermId=  6 	, 45 , 84
const CKeyFixup IBM_3486_102_French[27] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   9, 0,
	/* '*' */ 0x002A,  28, MO_ShiftLeft,
	/* '+' */ 0x002B,  55, MO_ShiftLeft,
	/* '-' */ 0x002D,  13, 0,
	/* '=' */ 0x003D,  55, 0,
	/* '@' */ 0x0040,   3, MO_AltRight,
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,  45, MO_AltRight,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '^' */ 0x005E,  27, 0,
	/* '_' */ 0x005F,  13, MO_ShiftLeft,
	/* '`' */ 0x0060,   8, MO_AltRight,
	/* '{' */ 0x007B,  10, MO_AltRight,
	/* '|' */ 0x007C,   2, MO_AltRight,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* '~' */ 0x007E,  55, MO_AltRight,
	/* ... */ 0x007F,  76, 0,									// <control> (DELETE)
	/* '£' */ 0x00A3,  42, MO_ShiftLeft,
	/* '¤' */ 0x00A4, NOS, 0,
	/* '§' */ 0x00A7,   7, 0,
	/* '¨' */ 0x00A8,  27, MO_ShiftLeft,
	/* '°' */ 0x00B0, NOS, 0,
	/* '²' */ 0x00B2, NOS, 0,
	/* '´' */ 0x00B4,  41, MO_AltRight,
	/* 'µ' */ 0x00B5,  42, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// The next table is equal to another, so we save the memory
#define IBM_3486_102_German           		IBM_3486_102_Belgian

// TermId=  6 	, 45 , 84
const CKeyFixup IBM_3486_102_Italian[11] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '#' */ 0x0023,   4, MO_AltRight,
	/* '@' */ 0x0040,  17, MO_AltRight,
	/* '[' */ 0x005B,   9, MO_AltRight,
	/* ']' */ 0x005D,  10, MO_AltRight,
	/* '`' */ 0x0060,  42, MO_AltRight,
	/* '{' */ 0x007B,   8, MO_AltRight,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* '~' */ 0x007E,  28, MO_AltRight,
	/* ... */ 0x007F,  76, 0,									// <control> (DELETE)
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  6 	, 45 , 84
const CKeyFixup IBM_3486_102_Swiss_German[12] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '^' */ 0x005E,  13, 0,
	/* '`' */ 0x0060,  13, MO_ShiftLeft,
	/* '|' */ 0x007C,   2, MO_AltRight,
	/* '~' */ 0x007E,  13, MO_AltRight,
	/* ... */ 0x007F,  76, 0,									// <control> (DELETE)
	/* '¢' */ 0x00A2,   9, MO_AltRight,
	/* '¦' */ 0x00A6, NOS, 0,
	/* '¨' */ 0x00A8,  28, 0,
	/* '¬' */ 0x00AC,   7, MO_AltRight,
	/* '´' */ 0x00B4,  12, MO_AltRight,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  6 	, 45 , 84
const CKeyFixup IBM_3486_102_British[26] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '#' */ 0x0023,  11, MO_ShiftLeft,
	/* '&' */ 0x0026,   7, MO_ShiftLeft,
	/* ''' */ 0x0027,  27, MO_ShiftLeft,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '*' */ 0x002A,  41, MO_ShiftLeft,
	/* '+' */ 0x002B,  40, MO_ShiftLeft,
	/* ':' */ 0x003A,  41, 0,
	/* '=' */ 0x003D,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,  27, 0,
	/* '[' */ 0x005B,  28, 0,
	/* '\' */ 0x005C,   1, 0,
	/* ']' */ 0x005D,  42, 0,
	/* '^' */ 0x005E,  41, MO_AltRight,
	/* '_' */ 0x005F,  13, MO_ShiftLeft,
	/* '`' */ 0x0060, NOS, 0,
	/* '{' */ 0x007B,  28, MO_ShiftLeft,
	/* '|' */ 0x007C,   1, MO_ShiftLeft,
	/* '}' */ 0x007D,  42, MO_ShiftLeft,
	/* '~' */ 0x007E,  28, MO_AltRight,
	/* ... */ 0x007F,  76, 0,									// <control> (DELETE)
	/* '¬' */ 0x00AC,  13, 0,
	/* '²' */ 0x00B2,   3, MO_AltRight,
	/* '³' */ 0x00B3,   4, MO_AltRight,
	/* '´' */ 0x00B4,   8, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  6 	, 45 , 84
const CKeyFixup IBM_3486_102_Danish[10] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '^' */ 0x005E,  28, MO_ShiftLeft,
	/* '`' */ 0x0060,  13, MO_ShiftLeft,
	/* '~' */ 0x007E,  28, MO_AltRight,
	/* ... */ 0x007F,  76, 0,									// <control> (DELETE)
	/* '§' */ 0x00A7, NOS, 0,
	/* '¨' */ 0x00A8, NOS, 0,
	/* '´' */ 0x00B4,  13, 0,
	/* '½' */ 0x00BD, NOS, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// The next table is equal to another, so we save the memory
#define IBM_3486_102_Norwegian        		IBM_3486_102_Belgian

// The next table is equal to another, so we save the memory
#define IBM_3486_102_Spanish          		IBM_3486_102_Belgian

// TermId=  6 	, 45 , 84
const CKeyFixup IBM_3486_102_Latin_America[6] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '^' */ 0x005E,  41, MO_AltRight,
	/* '`' */ 0x0060,  27, 0,
	/* ... */ 0x007F,  76, 0,									// <control> (DELETE)
	/* '¨' */ 0x00A8,  27, MO_ShiftLeft,
	/* '´' */ 0x00B4, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

#define IBM_3191_122_USA                      		NoExceptions	// not required

// TermId=  7 	, 24 , 46 , 71
const CKeyFixup IBM_3191_122_Belgian[55] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   9, 0,
	/* '"' */ 0x0022,   4, 0,
	/* '#' */ 0x0023,   4, MO_AltRight,
	/* '$' */ 0x0024,  28, 0,
	/* '%' */ 0x0025,  41, MO_ShiftLeft,
	/* '&' */ 0x0026,   2, 0,
	/* ''' */ 0x0027,   5, 0,
	/* '(' */ 0x0028,   6, 0,
	/* ')' */ 0x0029,  12, 0,
	/* '+' */ 0x002B,  55, MO_ShiftLeft,
	/* ',' */ 0x002C,  52, 0,
	/* '-' */ 0x002D,  13, 0,
	/* '.' */ 0x002E,  53, MO_ShiftLeft,
	/* '/' */ 0x002F,  54, MO_ShiftLeft,
	/* '0' */ 0x0030,  11, MO_ShiftLeft,
	/* '1' */ 0x0031,   2, MO_ShiftLeft,
	/* '2' */ 0x0032,   3, MO_ShiftLeft,
	/* '3' */ 0x0033,   4, MO_ShiftLeft,
	/* '4' */ 0x0034,   5, MO_ShiftLeft,
	/* '5' */ 0x0035,   6, MO_ShiftLeft,
	/* '6' */ 0x0036,   7, MO_ShiftLeft,
	/* '7' */ 0x0037,   8, MO_ShiftLeft,
	/* '8' */ 0x0038,   9, MO_ShiftLeft,
	/* '9' */ 0x0039,  10, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, 0,
	/* ';' */ 0x003B,  53, 0,
	/* '=' */ 0x003D,  55, 0,
	/* '?' */ 0x003F,  52, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_AltRight,
	/* 'M' */ 0x004D,  40, MO_ShiftLeft,
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,  45, MO_AltRight,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '^' */ 0x005E,   7, MO_AltRight,
	/* '_' */ 0x005F,  13, MO_ShiftLeft,
	/* '`' */ 0x0060,  42, MO_AltRight,
	/* 'm' */ 0x006D,  40, 0,
	/* '{' */ 0x007B,  10, MO_AltRight,
	/* '|' */ 0x007C,   2, MO_AltRight,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* '~' */ 0x007E,  55, MO_AltRight,
	/* '£' */ 0x00A3,  42, MO_ShiftLeft,
	/* '§' */ 0x00A7,   7, 0,
	/* '¨' */ 0x00A8,  27, MO_ShiftLeft,
	/* '°' */ 0x00B0,  12, MO_ShiftLeft,
	/* '²' */ 0x00B2,   1, 0,
	/* '³' */ 0x00B3,   1, MO_ShiftLeft,
	/* '´' */ 0x00B4,  41, MO_AltRight,
	/* 'µ' */ 0x00B5,  42, 0,
	/* 'à' */ 0x00E0,  11, 0,
	/* 'ç' */ 0x00E7,  10, 0,
	/* 'è' */ 0x00E8,   8, 0,
	/* 'é' */ 0x00E9,   3, 0,
	/* 'ù' */ 0x00F9,  41, 0,
	/* end */ 0,KN_ENDMARKER,0
};

#define IBM_3191_122_Finnish_Swedish          		NoExceptions	// not required

#define IBM_3191_122_French                   		NoExceptions	// not required

#define IBM_3191_122_German                   		NoExceptions	// not required

#define IBM_3191_122_Italian                  		NoExceptions	// not required

// TermId=  7 	, 24 , 46 , 71
const CKeyFixup IBM_3191_122_Swiss_German[15] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '#' */ 0x0023,   4, MO_AltRight,
	/* '@' */ 0x0040,   3, MO_AltRight,
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,  45, MO_AltRight,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '{' */ 0x007B,  41, MO_AltRight,
	/* '|' */ 0x007C,   2, MO_AltRight,
	/* '}' */ 0x007D,  42, MO_AltRight,
	/* '~' */ 0x007E,  13, MO_AltRight,
	/* '¢' */ 0x00A2,   9, MO_AltRight,
	/* '§' */ 0x00A7,   1, 0,
	/* '¬' */ 0x00AC,   7, MO_AltRight,
	/* '°' */ 0x00B0,   1, MO_ShiftLeft,
	/* '´' */ 0x00B4,  12, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

#define IBM_3191_122_British                  		NoExceptions	// not required

#define IBM_3191_122_Danish                   		NoExceptions	// not required

#define IBM_3191_122_Norwegian                		NoExceptions	// not required

#define IBM_3191_122_Spanish                  		NoExceptions	// not required

// TermId=  8 	, 72
const CKeyFixup IBM_3191_122_Latin_America[10] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '\' */ 0x005C,  27, 0,
	/* '`' */ 0x0060,  27, MO_ShiftLeft,
	/* '|' */ 0x007C, NOS, 0,
	/* '¡' */ 0x00A1, NOS, 0,
	/* '¨' */ 0x00A8,  13, MO_ShiftLeft,
	/* '¬' */ 0x00AC,  13, 0,
	/* '°' */ 0x00B0, NOS, 0,
	/* '´' */ 0x00B4, NOS, 0,
	/* '¿' */ 0x00BF, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

const CKeyFixup IBM_3486_122_USA[3] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '¢' */ 0x00A2,  27, 0,
	/* '¬' */ 0x00AC,   7, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  8 	, 72
const CKeyFixup IBM_3486_122_Belgian[60] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   2, MO_ShiftLeft,
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '#' */ 0x0023,   4, MO_ShiftLeft,
	/* '$' */ 0x0024,   5, MO_ShiftLeft,
	/* '%' */ 0x0025,   6, MO_ShiftLeft,
	/* '&' */ 0x0026,  28, 0,
	/* ''' */ 0x0027,  12, 0,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '+' */ 0x002B,   7, MO_ShiftLeft,
	/* ',' */ 0x002C,  53, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '.' */ 0x002E,  54, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
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
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040, NOS, 0,
	/* 'M' */ 0x004D,  52, MO_ShiftLeft,
	/* '[' */ 0x005B,  27, MO_ShiftLeft,
	/* '\' */ 0x005C, NOS, 0,
	/* ']' */ 0x005D,  42, MO_ShiftLeft,
	/* '^' */ 0x005E,  13, 0,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,  42, 0,
	/* 'm' */ 0x006D,  52, 0,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* ... */ 0x007F,  86, 0,									// <control> (DELETE)
	/* '¢' */ 0x00A2, NOS, 0,
	/* '£' */ 0x00A3, NOS, 0,
	/* '§' */ 0x00A7, NOS, 0,
	/* '¨' */ 0x00A8,  13, MO_ShiftLeft,
	/* '¬' */ 0x00AC, NOS, 0,
	/* '°' */ 0x00B0, NOS, 0,
	/* '²' */ 0x00B2, NOS, 0,
	/* '³' */ 0x00B3, NOS, 0,
	/* '´' */ 0x00B4, NOS, 0,
	/* 'µ' */ 0x00B5, NOS, 0,
	/* 'à' */ 0x00E0,  41, 0,
	/* 'ç' */ 0x00E7,  40, MO_ShiftLeft,
	/* 'è' */ 0x00E8,  27, 0,
	/* 'é' */ 0x00E9,  40, 0,
	/* 'ù' */ 0x00F9,  41, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  8 	, 72
const CKeyFixup IBM_3486_122_Finnish_Swedish[27] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   2, MO_ShiftLeft,
	/* '#' */ 0x0023, NOS, 0,
	/* '$' */ 0x0024, NOS, 0,
	/* '<' */ 0x003C,  45, 0,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '@' */ 0x0040, NOS, 0,
	/* '[' */ 0x005B, NOS, 0,
	/* '\' */ 0x005C, NOS, 0,
	/* ']' */ 0x005D, NOS, 0,
	/* '^' */ 0x005E,  28, MO_ShiftLeft,
	/* '`' */ 0x0060, NOS, 0,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* ... */ 0x007F,  86, 0,									// <control> (DELETE)
	/* '¢' */ 0x00A2, NOS, 0,
	/* '£' */ 0x00A3, NOS, 0,
	/* '§' */ 0x00A7,   4, MO_ShiftLeft,
	/* '¨' */ 0x00A8, NOS, 0,
	/* '¬' */ 0x00AC, NOS, 0,
	/* '´' */ 0x00B4, NOS, 0,
	/* '½' */ 0x00BD, NOS, 0,
	/* 'É' */ 0x00C9,  13, MO_ShiftLeft,
	/* 'é' */ 0x00E9,  13, 0,
	/* 'ü' */ 0x00FC,  28, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  8 	, 72
const CKeyFixup IBM_3486_122_French[22] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   9, 0,
	/* '#' */ 0x0023, NOS, 0,
	/* '<' */ 0x003C,  45, 0,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '@' */ 0x0040, NOS, 0,
	/* '[' */ 0x005B, NOS, 0,
	/* '\' */ 0x005C, NOS, 0,
	/* ']' */ 0x005D, NOS, 0,
	/* '^' */ 0x005E,  27, 0,
	/* '`' */ 0x0060, NOS, 0,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* ... */ 0x007F,  86, 0,									// <control> (DELETE)
	/* '¢' */ 0x00A2, NOS, 0,
	/* '¬' */ 0x00AC, NOS, 0,
	/* '°' */ 0x00B0,  12, MO_ShiftLeft,
	/* '²' */ 0x00B2,   1, 0,
	/* '³' */ 0x00B3,   1, MO_ShiftLeft,
	/* '´' */ 0x00B4, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  8 	, 72
const CKeyFixup IBM_3486_122_German[22] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   2, MO_ShiftLeft,
	/* ''' */ 0x0027,  13, 0,
	/* '<' */ 0x003C,  45, 0,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '@' */ 0x0040, NOS, 0,
	/* '[' */ 0x005B, NOS, 0,
	/* '\' */ 0x005C, NOS, 0,
	/* ']' */ 0x005D, NOS, 0,
	/* '^' */ 0x005E,  42, MO_ShiftLeft,
	/* '`' */ 0x0060,  13, MO_ShiftLeft,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* ... */ 0x007F,  86, 0,									// <control> (DELETE)
	/* '¢' */ 0x00A2, NOS, 0,
	/* '¬' */ 0x00AC, NOS, 0,
	/* '°' */ 0x00B0, NOS, 0,
	/* '²' */ 0x00B2, NOS, 0,
	/* '³' */ 0x00B3, NOS, 0,
	/* '´' */ 0x00B4, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  8 	, 72
const CKeyFixup IBM_3486_122_Italian[18] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   2, MO_ShiftLeft,
	/* '#' */ 0x0023, NOS, 0,
	/* '<' */ 0x003C,  45, 0,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '@' */ 0x0040, NOS, 0,
	/* '[' */ 0x005B, NOS, 0,
	/* '\' */ 0x005C, NOS, 0,
	/* ']' */ 0x005D, NOS, 0,
	/* '^' */ 0x005E,  13, MO_ShiftLeft,
	/* '`' */ 0x0060, NOS, 0,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* ... */ 0x007F,  86, 0,									// <control> (DELETE)
	/* '¢' */ 0x00A2, NOS, 0,
	/* '¬' */ 0x00AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  8 	, 72
const CKeyFixup IBM_3486_122_Swiss_German[20] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,  28, MO_ShiftLeft,
	/* '#' */ 0x0023,   1, 0,
	/* '<' */ 0x003C,  45, 0,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '@' */ 0x0040, NOS, 0,
	/* '[' */ 0x005B, NOS, 0,
	/* '\' */ 0x005C, NOS, 0,
	/* ']' */ 0x005D, NOS, 0,
	/* '^' */ 0x005E,  13, 0,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C,   1, MO_ShiftLeft,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* ... */ 0x007F,  86, 0,									// <control> (DELETE)
	/* '¢' */ 0x00A2, NOS, 0,
	/* '§' */ 0x00A7, NOS, 0,
	/* '¬' */ 0x00AC, NOS, 0,
	/* '°' */ 0x00B0, NOS, 0,
	/* '´' */ 0x00B4, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  8 	, 72
const CKeyFixup IBM_3486_122_British[17] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   2, MO_ShiftLeft,
	/* '<' */ 0x003C,  53, MO_ShiftLeft,
	/* '>' */ 0x003E,  54, MO_ShiftLeft,
	/* '[' */ 0x005B, NOS, 0,
	/* '\' */ 0x005C,   1, 0,
	/* ']' */ 0x005D, NOS, 0,
	/* '^' */ 0x005E, NOS, 0,
	/* '{' */ 0x007B,  28, MO_ShiftLeft,
	/* '|' */ 0x007C,   1, MO_ShiftLeft,
	/* '}' */ 0x007D,  42, MO_ShiftLeft,
	/* '~' */ 0x007E, NOS, 0,
	/* ... */ 0x007F,  86, 0,									// <control> (DELETE)
	/* '¢' */ 0x00A2, NOS, 0,
	/* '¬' */ 0x00AC,  13, 0,
	/* '²' */ 0x00B2, NOS, 0,
	/* '³' */ 0x00B3, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  8 	, 72
const CKeyFixup IBM_3486_122_Danish[20] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   2, MO_ShiftLeft,
	/* '$' */ 0x0024, NOS, 0,
	/* '<' */ 0x003C,  45, 0,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '@' */ 0x0040, NOS, 0,
	/* '[' */ 0x005B, NOS, 0,
	/* '\' */ 0x005C,  13, 0,
	/* ']' */ 0x005D, NOS, 0,
	/* '^' */ 0x005E,  28, MO_ShiftLeft,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* ... */ 0x007F,  86, 0,									// <control> (DELETE)
	/* '¢' */ 0x00A2, NOS, 0,
	/* '£' */ 0x00A3, NOS, 0,
	/* '¬' */ 0x00AC, NOS, 0,
	/* '´' */ 0x00B4, NOS, 0,
	/* 'ü' */ 0x00FC,  28, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  8 	, 72
const CKeyFixup IBM_3486_122_Norwegian[23] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   2, MO_ShiftLeft,
	/* '$' */ 0x0024, NOS, 0,
	/* '<' */ 0x003C,  45, 0,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '@' */ 0x0040, NOS, 0,
	/* '[' */ 0x005B, NOS, 0,
	/* '\' */ 0x005C,  13, 0,
	/* ']' */ 0x005D, NOS, 0,
	/* '^' */ 0x005E,  28, MO_ShiftLeft,
	/* '`' */ 0x0060,  13, MO_ShiftLeft,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C, NOS, 0,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* ... */ 0x007F,  86, 0,									// <control> (DELETE)
	/* '¢' */ 0x00A2, NOS, 0,
	/* '£' */ 0x00A3, NOS, 0,
	/* '§' */ 0x00A7, NOS, 0,
	/* '¨' */ 0x00A8, NOS, 0,
	/* '¬' */ 0x00AC, NOS, 0,
	/* '´' */ 0x00B4, NOS, 0,
	/* 'ü' */ 0x00FC,  28, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  8 	, 72
const CKeyFixup IBM_3486_122_Spanish[15] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   2, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,   1, MO_AltRight,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '^' */ 0x005E,  27, MO_ShiftLeft|MO_DeadKey,
	/* '{' */ 0x007B,  41, MO_AltRight,
	/* '|' */ 0x007C,   2, MO_AltRight,
	/* '}' */ 0x007D,  42, MO_AltRight,
	/* '~' */ 0x007E, NOS, 0,
	/* ... */ 0x007F,  86, 0,									// <control> (DELETE)
	/* '¢' */ 0x00A2, NOS, 0,
	/* '¬' */ 0x00AC,   7, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=  8 	, 72
const CKeyFixup IBM_3486_122_Latin_America[18] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   2, MO_ShiftLeft,
	/* '#' */ 0x0023, NOS, 0,
	/* '<' */ 0x003C,  45, 0,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '@' */ 0x0040,   4, MO_ShiftLeft,
	/* '[' */ 0x005B,  41, MO_ShiftLeft,
	/* '\' */ 0x005C,   1, MO_AltRight,
	/* ']' */ 0x005D,  42, MO_ShiftLeft,
	/* '^' */ 0x005E, NOS, 0,
	/* '{' */ 0x007B,  41, 0,
	/* '|' */ 0x007C,   1, 0,
	/* '}' */ 0x007D,  42, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* ... */ 0x007F,  86, 0,									// <control> (DELETE)
	/* '¢' */ 0x00A2, NOS, 0,
	/* '¬' */ 0x00AC,   7, MO_AltRight,
	/* '´' */ 0x00B4,  27, MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

#define Telex_88_USA                          		NoExceptions	// not required

// TermId= 25
const CKeyFixup Telex_88_Belgian[4] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* ''' */ 0x0027, NOS, 0,
	/* '^' */ 0x005E,  13, 0,
	/* '´' */ 0x00B4,  12, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId= 25
const CKeyFixup Telex_88_Finnish_Swedish[2] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '^' */ 0x005E,  28, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId= 25
const CKeyFixup Telex_88_French[41] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   2, MO_ShiftLeft,
	/* '"' */ 0x0022,   3, MO_ShiftLeft,
	/* '$' */ 0x0024,   5, MO_ShiftLeft,
	/* '%' */ 0x0025,   6, MO_ShiftLeft,
	/* '&' */ 0x0026,  28, 0,
	/* ''' */ 0x0027,  12, 0,
	/* '(' */ 0x0028,   9, MO_ShiftLeft,
	/* ')' */ 0x0029,  10, MO_ShiftLeft,
	/* '+' */ 0x002B,   7, MO_ShiftLeft,
	/* ',' */ 0x002C,  53, 0,
	/* '-' */ 0x002D,  55, 0,
	/* '.' */ 0x002E,  54, 0,
	/* '/' */ 0x002F,   8, MO_ShiftLeft,
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
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* ';' */ 0x003B,  53, MO_ShiftLeft,
	/* '=' */ 0x003D,  11, MO_ShiftLeft,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '^' */ 0x005E,  13, 0,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,  42, 0,
	/* '§' */ 0x00A7,   4, MO_ShiftLeft,
	/* '¨' */ 0x00A8,  13, MO_ShiftLeft,
	/* '°' */ 0x00B0,  41, MO_ShiftLeft,
	/* '²' */ 0x00B2, NOS, 0,
	/* '³' */ 0x00B3, NOS, 0,
	/* 'µ' */ 0x00B5, NOS, 0,
	/* 'à' */ 0x00E0,  27, 0,
	/* 'ç' */ 0x00E7,  27, MO_ShiftLeft,
	/* 'è' */ 0x00E8, NOS, 0,
	/* 'é' */ 0x00E9, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId= 25
const CKeyFixup Telex_88_German[2] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '^' */ 0x005E,  42, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId= 25
const CKeyFixup Telex_88_Italian[2] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '^' */ 0x005E,  13, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId= 25
const CKeyFixup Telex_88_Swiss_German[5] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '#' */ 0x0023, NOS, 0,
	/* '+' */ 0x002B, NOS, 0,
	/* '^' */ 0x005E,  13, 0,
	/* '|' */ 0x007C,   2, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId= 25
const CKeyFixup Telex_88_British[2] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '^' */ 0x005E, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// The next table is equal to another, so we save the memory
#define Telex_88_Danish               		Telex_88_Finnish_Swedish

// The next table is equal to another, so we save the memory
#define Telex_88_Norwegian            		Telex_88_Finnish_Swedish

// TermId= 25
const CKeyFixup Telex_88_Spanish[24] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,  42, MO_AltRight,
	/* '#' */ 0x0023, NOS, 0,
	/* '$' */ 0x0024, NOS, 0,
	/* '@' */ 0x0040,   4, MO_ShiftLeft,
	/* '[' */ 0x005B,  41, MO_ShiftLeft,
	/* '\' */ 0x005C,  27, 0,
	/* ']' */ 0x005D,  42, MO_ShiftLeft,
	/* '^' */ 0x005E, NOS, 0,
	/* '`' */ 0x0060,  27, MO_ShiftLeft,
	/* '{' */ 0x007B,  41, 0,
	/* '|' */ 0x007C,   2, MO_ShiftLeft,
	/* '}' */ 0x007D,  42, 0,
	/* '¡' */ 0x00A1, NOS, 0,
	/* '¢' */ 0x00A2,  41, MO_AltRight,
	/* '¨' */ 0x00A8,  13, MO_ShiftLeft,
	/* 'ª' */ 0x00AA, NOS, 0,
	/* '¬' */ 0x00AC,  13, 0,
	/* '´' */ 0x00B4, NOS, 0,
	/* '·' */ 0x00B7, NOS, 0,
	/* 'º' */ 0x00BA, NOS, 0,
	/* '¿' */ 0x00BF, NOS, 0,
	/* 'Ç' */ 0x00C7, NOS, 0,
	/* 'ç' */ 0x00E7, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

#define BDS_7_USA                             		NoExceptions	// not required

// TermId= 35
const CKeyFixup BDS_7_Belgian[7] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,  45, MO_AltRight,
	/* '{' */ 0x007B,  10, MO_AltRight,
	/* '|' */ 0x007C,   2, MO_AltRight,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId= 35
const CKeyFixup BDS_7_Finnish_Swedish[7] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '[' */ 0x005B,   9, MO_AltRight,
	/* '\' */ 0x005C,  12, MO_AltRight,
	/* '{' */ 0x007B,   8, MO_AltRight,
	/* '|' */ 0x007C,  45, MO_AltRight,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId= 35
const CKeyFixup BDS_7_French[7] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '[' */ 0x005B,   6, MO_AltRight,
	/* '\' */ 0x005C,   9, MO_AltRight,
	/* '{' */ 0x007B,   5, MO_AltRight,
	/* '|' */ 0x007C,   7, MO_AltRight,
	/* '}' */ 0x007D,  13, MO_AltRight,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// The next table is equal to another, so we save the memory
#define BDS_7_German                  		BDS_7_Finnish_Swedish

// TermId= 35
const CKeyFixup BDS_7_Italian[7] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,   1, 0,
	/* '{' */ 0x007B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x007C,   1, MO_ShiftLeft,
	/* '}' */ 0x007D,  28, MO_ShiftLeft|MO_AltRight,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId= 35
const CKeyFixup BDS_7_Swiss_German[7] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,  45, MO_AltRight,
	/* '{' */ 0x007B,  41, MO_AltRight,
	/* '|' */ 0x007C,   8, MO_AltRight,
	/* '}' */ 0x007D,  42, MO_AltRight,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId= 35
const CKeyFixup BDS_7_British[6] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '[' */ 0x005B,  27, 0,
	/* '\' */ 0x005C,  45, 0,
	/* '{' */ 0x007B,  27, MO_ShiftLeft,
	/* '|' */ 0x007C,  45, MO_ShiftLeft,
	/* '}' */ 0x007D,  28, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId= 35
const CKeyFixup BDS_7_Danish[7] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '[' */ 0x005B,   9, MO_AltRight,
	/* '\' */ 0x005C,  45, MO_AltRight,
	/* '{' */ 0x007B,   8, MO_AltRight,
	/* '|' */ 0x007C,  13, MO_AltRight,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId= 35
const CKeyFixup BDS_7_Norwegian[7] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '[' */ 0x005B,   9, MO_AltRight,
	/* '\' */ 0x005C,  13, 0,
	/* '{' */ 0x007B,   8, MO_AltRight,
	/* '|' */ 0x007C,   1, 0,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId= 35
const CKeyFixup BDS_7_Spanish[7] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,   1, MO_AltRight,
	/* '{' */ 0x007B,  41, MO_AltRight,
	/* '|' */ 0x007C,   2, MO_AltRight,
	/* '}' */ 0x007D,  42, MO_AltRight,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

#define DEC_VT510_LK411_USA                   		NoExceptions	// not required

// TermId=104
const CKeyFixup DEC_VT510_LK411_Belgian[17] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '#' */ 0x0023,  42, 0,
	/* '@' */ 0x0040,  42, MO_ShiftLeft,
	/* '[' */ 0x005B,   7, MO_AltRight,
	/* '\' */ 0x005C,  41, MO_AltRight,
	/* ']' */ 0x005D,   8, MO_AltRight,
	/* '^' */ 0x005E,  27, 0,
	/* '`' */ 0x0060,   1, 0,
	/* '|' */ 0x007C,   3, MO_AltRight,
	/* '~' */ 0x007E,   1, MO_ShiftLeft,
	/* '£' */ 0x00A3, NOS, 0,
	/* '¨' */ 0x00A8,  27, MO_ShiftLeft,
	/* '²' */ 0x00B2, NOS, 0,
	/* '³' */ 0x00B3, NOS, 0,
	/* '´' */ 0x00B4, NOS, 0,
	/* 'µ' */ 0x00B5, NOS, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_Finnish_Swedish[22] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '#' */ 0x0023,   4, MO_ShiftLeft|MO_AltRight,
	/* '$' */ 0x0024,   5, MO_ShiftLeft,
	/* '@' */ 0x0040,   1, MO_ShiftLeft,
	/* '[' */ 0x005B,  41, MO_ShiftLeft|MO_AltRight,
	/* '\' */ 0x005C,  40, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x005D,  27, MO_ShiftLeft|MO_AltRight,
	/* '^' */ 0x005E,  13, 0,
	/* '`' */ 0x0060,  13, MO_ShiftLeft,
	/* '{' */ 0x007B,  41, MO_AltRight,
	/* '|' */ 0x007C,  40, MO_AltRight,
	/* '}' */ 0x007D,  27, MO_AltRight,
	/* '~' */ 0x007E,   1, 0,
	/* '£' */ 0x00A3, NOS, 0,
	/* '¤' */ 0x00A4, NOS, 0,
	/* '§' */ 0x00A7,   4, MO_ShiftLeft,
	/* '¨' */ 0x00A8, NOS, 0,
	/* '´' */ 0x00B4, NOS, 0,
	/* '½' */ 0x00BD, NOS, 0,
	/* 'Ü' */ 0x00DC,  28, MO_ShiftLeft,
	/* 'ü' */ 0x00FC,  28, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_French[25] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   9, 0,
	/* '#' */ 0x0023,  42, 0,
	/* '*' */ 0x002A,  28, MO_ShiftLeft,
	/* '+' */ 0x002B,  55, MO_ShiftLeft,
	/* '-' */ 0x002D,  13, 0,
	/* '=' */ 0x003D,  55, 0,
	/* '@' */ 0x0040,  42, MO_ShiftLeft,
	/* '[' */ 0x005B,   7, MO_AltRight,
	/* '\' */ 0x005C,  41, MO_AltRight,
	/* ']' */ 0x005D,   8, MO_AltRight,
	/* '^' */ 0x005E,  27, 0,
	/* '_' */ 0x005F,  13, MO_ShiftLeft,
	/* '`' */ 0x0060,   1, 0,
	/* '{' */ 0x007B,  10, MO_AltRight,
	/* '|' */ 0x007C,   3, MO_AltRight,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* '~' */ 0x007E,   1, MO_ShiftLeft,
	/* '£' */ 0x00A3, NOS, 0,
	/* '¤' */ 0x00A4, NOS, 0,
	/* '§' */ 0x00A7,   7, 0,
	/* '¨' */ 0x00A8,  27, MO_ShiftLeft,
	/* '²' */ 0x00B2, NOS, 0,
	/* 'µ' */ 0x00B5, NOS, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_German[18] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '\' */ 0x005C,  12, MO_AltRight,
	/* '^' */ 0x005E,   1, 0,
	/* '`' */ 0x0060,  13, MO_ShiftLeft,
	/* '|' */ 0x007C,  45, MO_AltRight,
	/* '¡' */ 0x00A1,   2, MO_ShiftLeft|MO_AltRight,
	/* '£' */ 0x00A3,   4, MO_ShiftLeft|MO_AltRight,
	/* '¤' */ 0x00A4,   5, MO_ShiftLeft|MO_AltRight,
	/* '¬' */ 0x00AC,   1, MO_AltRight,
	/* '±' */ 0x00B1,  10, MO_ShiftLeft|MO_AltRight,
	/* '´' */ 0x00B4,  13, 0,
	/* '¸' */ 0x00B8,  13, MO_AltRight,
	/* '¹' */ 0x00B9,   2, MO_AltRight,
	/* '¼' */ 0x00BC,   5, MO_AltRight,
	/* '½' */ 0x00BD,   6, MO_AltRight,
	/* '¾' */ 0x00BE,   7, MO_AltRight,
	/* '¿' */ 0x00BF,  12, MO_ShiftLeft|MO_AltRight,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_Italian[57] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,  55, MO_ShiftLeft,
	/* '"' */ 0x0022,   4, 0,
	/* '#' */ 0x0023,   3, MO_AltRight,
	/* '$' */ 0x0024,  28, 0,
	/* '%' */ 0x0025,  41, MO_ShiftLeft,
	/* '&' */ 0x0026,  28, MO_ShiftLeft,
	/* ''' */ 0x0027,   5, 0,
	/* '(' */ 0x0028,   6, 0,
	/* ')' */ 0x0029,  12, 0,
	/* '*' */ 0x002A,  42, 0,
	/* '+' */ 0x002B,  13, MO_ShiftLeft,
	/* ',' */ 0x002C,  52, 0,
	/* '-' */ 0x002D,  13, 0,
	/* '.' */ 0x002E,  53, MO_ShiftLeft,
	/* '/' */ 0x002F,  54, MO_ShiftLeft,
	/* '0' */ 0x0030,  11, MO_ShiftLeft,
	/* '1' */ 0x0031,   2, MO_ShiftLeft,
	/* '2' */ 0x0032,   3, MO_ShiftLeft,
	/* '3' */ 0x0033,   4, MO_ShiftLeft,
	/* '4' */ 0x0034,   5, MO_ShiftLeft,
	/* '5' */ 0x0035,   6, MO_ShiftLeft,
	/* '6' */ 0x0036,   7, MO_ShiftLeft,
	/* '7' */ 0x0037,   8, MO_ShiftLeft,
	/* '8' */ 0x0038,   9, MO_ShiftLeft,
	/* '9' */ 0x0039,  10, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, 0,
	/* ';' */ 0x003B,  53, 0,
	/* '=' */ 0x003D,  27, MO_ShiftLeft,
	/* '?' */ 0x003F,  52, MO_ShiftLeft,
	/* '@' */ 0x0040,   2, MO_AltRight,
	/* 'M' */ 0x004D,  40, MO_ShiftLeft,
	/* 'W' */ 0x0057,  46, MO_ShiftLeft,
	/* 'Z' */ 0x005A,  18, MO_ShiftLeft,
	/* '[' */ 0x005B,  10, MO_AltRight,
	/* '\' */ 0x005C,  42, MO_AltRight,
	/* ']' */ 0x005D,  11, MO_AltRight,
	/* '^' */ 0x005E,   9, 0,
	/* '_' */ 0x005F,   7, 0,
	/* '`' */ 0x0060,   1, 0,
	/* 'm' */ 0x006D,  40, 0,
	/* 'w' */ 0x0077,  46, 0,
	/* 'z' */ 0x007A,  18, 0,
	/* '{' */ 0x007B,  41, MO_AltRight,
	/* '|' */ 0x007C,  55, MO_AltRight,
	/* '}' */ 0x007D,  27, MO_AltRight,
	/* '~' */ 0x007E,   1, MO_ShiftLeft,
	/* '£' */ 0x00A3,   2, 0,
	/* '°' */ 0x00B0,  12, MO_ShiftLeft,
	/* 'à' */ 0x00E0,  11, 0,
	/* 'ç' */ 0x00E7,  10, 0,
	/* 'è' */ 0x00E8,   8, 0,
	/* 'é' */ 0x00E9,   3, 0,
	/* 'ì' */ 0x00EC,  27, 0,
	/* 'ò' */ 0x00F2,  55, 0,
	/* 'ù' */ 0x00F9,  41, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_Swiss_German[18] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   1, 0,
	/* '#' */ 0x0023,  42, MO_ShiftLeft|MO_AltRight,
	/* '@' */ 0x0040,   5, MO_ShiftLeft|MO_AltRight,
	/* '[' */ 0x005B,  40, MO_ShiftLeft|MO_AltRight,
	/* '\' */ 0x005C,  27, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x005D,  41, MO_ShiftLeft|MO_AltRight,
	/* '^' */ 0x005E,  13, 0,
	/* '`' */ 0x0060,  13, MO_ShiftLeft,
	/* '{' */ 0x007B,  40, MO_AltRight,
	/* '|' */ 0x007C,  27, MO_AltRight,
	/* '}' */ 0x007D,  41, MO_AltRight,
	/* '~' */ 0x007E,  28, MO_ShiftLeft,
	/* '¦' */ 0x00A6, NOS, 0,
	/* '§' */ 0x00A7, NOS, 0,
	/* '¨' */ 0x00A8,  28, 0,
	/* '´' */ 0x00B4, NOS, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_British[10] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,  41, MO_ShiftLeft,
	/* '#' */ 0x0023,   4, MO_ShiftLeft,
	/* '<' */ 0x003C,  45, 0,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_ShiftLeft,
	/* '\' */ 0x005C,  42, 0,
	/* '|' */ 0x007C,  42, MO_ShiftLeft,
	/* '~' */ 0x007E,   1, MO_ShiftLeft,
	/* '£' */ 0x00A3, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_Danish[19] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '#' */ 0x0023,   4, MO_ShiftLeft|MO_AltRight,
	/* '$' */ 0x0024,   5, MO_ShiftLeft,
	/* '@' */ 0x0040,   1, MO_AltRight,
	/* '[' */ 0x005B,  40, MO_ShiftLeft|MO_AltRight,
	/* '\' */ 0x005C,  41, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x005D,  27, MO_ShiftLeft|MO_AltRight,
	/* '^' */ 0x005E,  28, MO_ShiftLeft,
	/* '`' */ 0x0060,  13, MO_ShiftLeft,
	/* '{' */ 0x007B,  40, MO_AltRight,
	/* '|' */ 0x007C,  41, MO_AltRight,
	/* '}' */ 0x007D,  27, MO_AltRight,
	/* '~' */ 0x007E,   1, 0,
	/* '£' */ 0x00A3,   4, MO_ShiftLeft,
	/* '¤' */ 0x00A4, NOS, 0,
	/* '¨' */ 0x00A8,  28, 0,
	/* '´' */ 0x00B4,  13, 0,
	/* '½' */ 0x00BD, NOS, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_Norwegian[18] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '$' */ 0x0024,   5, MO_ShiftLeft,
	/* '@' */ 0x0040,   1, MO_ShiftLeft,
	/* '[' */ 0x005B,  41, MO_ShiftLeft|MO_AltRight,
	/* '\' */ 0x005C,  40, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x005D,  27, MO_ShiftLeft|MO_AltRight,
	/* '^' */ 0x005E,  28, MO_ShiftLeft,
	/* '`' */ 0x0060,  13, MO_ShiftLeft,
	/* '{' */ 0x007B,  41, MO_AltRight,
	/* '|' */ 0x007C,  40, MO_AltRight,
	/* '}' */ 0x007D,  27, MO_AltRight,
	/* '~' */ 0x007E,   1, 0,
	/* '£' */ 0x00A3, NOS, 0,
	/* '¤' */ 0x00A4, NOS, 0,
	/* '§' */ 0x00A7, NOS, 0,
	/* '¨' */ 0x00A8,  28, 0,
	/* '´' */ 0x00B4,  13, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_Spanish[23] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '#' */ 0x0023,   4, MO_ShiftLeft,
	/* '@' */ 0x0040,   1, MO_AltRight,
	/* '[' */ 0x005B,  13, MO_ShiftLeft|MO_AltRight,
	/* '\' */ 0x005C,   1, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x005D,  13, MO_AltRight,
	/* '^' */ 0x005E,  27, MO_ShiftLeft,
	/* '`' */ 0x0060,  27, 0,
	/* '{' */ 0x007B,  40, MO_AltRight,
	/* '|' */ 0x007C,  42, MO_AltRight,
	/* '}' */ 0x007D,  40, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x007E,  42, MO_ShiftLeft,
	/* '¡' */ 0x00A1,   1, MO_ShiftLeft,
	/* '¨' */ 0x00A8, NOS, 0,
	/* 'ª' */ 0x00AA, NOS, 0,
	/* '¬' */ 0x00AC, NOS, 0,
	/* '°' */ 0x00B0,  13, 0,
	/* '´' */ 0x00B4,  41, 0,
	/* '·' */ 0x00B7, NOS, 0,
	/* 'º' */ 0x00BA, NOS, 0,
	/* '¿' */ 0x00BF,   1, 0,
	/* 'Ç' */ 0x00C7, NOS, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_Dutch[50] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,  41, MO_ShiftLeft,
	/* '&' */ 0x0026,   8, MO_ShiftLeft,
	/* ''' */ 0x0027,  41, 0,
	/* '(' */ 0x0028,  10, MO_ShiftLeft,
	/* ')' */ 0x0029,  11, MO_ShiftLeft,
	/* '*' */ 0x002A,   9, MO_ShiftLeft,
	/* '+' */ 0x002B,  13, MO_ShiftLeft,
	/* '-' */ 0x002D,  12, 0,
	/* '/' */ 0x002F,  55, 0,
	/* ':' */ 0x003A,  40, MO_ShiftLeft,
	/* ';' */ 0x003B,  40, 0,
	/* '<' */ 0x003C,  45, 0,
	/* '=' */ 0x003D,  13, 0,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '?' */ 0x003F,  55, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_ShiftLeft,
	/* '[' */ 0x005B,  27, 0,
	/* '\' */ 0x005C,  42, 0,
	/* ']' */ 0x005D,  28, 0,
	/* '^' */ 0x005E,   7, MO_ShiftLeft,
	/* '_' */ 0x005F,  12, MO_ShiftLeft,
	/* '`' */ 0x0060,   1, 0,
	/* '{' */ 0x007B,  27, MO_ShiftLeft,
	/* '|' */ 0x007C,  42, MO_ShiftLeft,
	/* '}' */ 0x007D,  28, MO_ShiftLeft,
	/* '~' */ 0x007E,   1, MO_ShiftLeft,
	/* '¢' */ 0x00A2, NOS, 0,
	/* '£' */ 0x00A3,   4, MO_AltRight,
	/* '¦' */ 0x00A6, NOS, 0,
	/* '§' */ 0x00A7, NOS, 0,
	/* '¨' */ 0x00A8, NOS, 0,
	/* '«' */ 0x00AB, NOS, 0,
	/* '¬' */ 0x00AC, NOS, 0,
	/* '°' */ 0x00B0, NOS, 0,
	/* '±' */ 0x00B1, NOS, 0,
	/* '²' */ 0x00B2, NOS, 0,
	/* '³' */ 0x00B3, NOS, 0,
	/* '´' */ 0x00B4, NOS, 0,
	/* 'µ' */ 0x00B5, NOS, 0,
	/* '¶' */ 0x00B6, NOS, 0,
	/* '·' */ 0x00B7, NOS, 0,
	/* '¸' */ 0x00B8, NOS, 0,
	/* '¹' */ 0x00B9, NOS, 0,
	/* '»' */ 0x00BB, NOS, 0,
	/* '¼' */ 0x00BC, NOS, 0,
	/* '½' */ 0x00BD, NOS, 0,
	/* '¾' */ 0x00BE, NOS, 0,
	/* 'ß' */ 0x00DF, NOS, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_Hebrew[5] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '<' */ 0x003C,  45, 0,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '\' */ 0x005C,  42, 0,
	/* '|' */ 0x007C,  42, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_Portuguese[32] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022, NOS, 0,
	/* '&' */ 0x0026,   8, MO_ShiftLeft,
	/* ''' */ 0x0027, NOS, 0,
	/* '(' */ 0x0028,  10, MO_ShiftLeft,
	/* ')' */ 0x0029,  11, MO_ShiftLeft,
	/* '*' */ 0x002A,   9, MO_ShiftLeft,
	/* '+' */ 0x002B,  13, MO_ShiftLeft,
	/* '-' */ 0x002D,  12, 0,
	/* '/' */ 0x002F,  55, 0,
	/* '=' */ 0x003D,  13, 0,
	/* '?' */ 0x003F,  55, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_ShiftLeft,
	/* '[' */ 0x005B,  42, 0,
	/* '\' */ 0x005C,   1, 0,
	/* ']' */ 0x005D,  28, 0,
	/* '^' */ 0x005E,   7, MO_ShiftLeft,
	/* '_' */ 0x005F,  12, MO_ShiftLeft,
	/* '`' */ 0x0060,  27, MO_ShiftLeft,
	/* '{' */ 0x007B,  42, MO_ShiftLeft,
	/* '|' */ 0x007C,   1, MO_ShiftLeft,
	/* '}' */ 0x007D,  28, MO_ShiftLeft,
	/* '~' */ 0x007E,  41, 0,
	/* '£' */ 0x00A3, NOS, 0,
	/* '§' */ 0x00A7, NOS, 0,
	/* '¨' */ 0x00A8, NOS, 0,
	/* 'ª' */ 0x00AA, NOS, 0,
	/* '«' */ 0x00AB, NOS, 0,
	/* '´' */ 0x00B4,  27, 0,
	/* 'º' */ 0x00BA, NOS, 0,
	/* '»' */ 0x00BB, NOS, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_Latin_America[7] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '\' */ 0x005C,  12, MO_AltRight,
	/* '^' */ 0x005E,  41, MO_AltRight,
	/* '`' */ 0x0060,  27, 0,
	/* '|' */ 0x007C,   1, 0,
	/* '¨' */ 0x00A8,  27, MO_ShiftLeft,
	/* '´' */ 0x00B4, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_Czech_DEC[18] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,  41, MO_ShiftLeft,
	/* ''' */ 0x0027,   1, 0,
	/* '(' */ 0x0028,  28, MO_ShiftLeft,
	/* ')' */ 0x0029,  28, 0,
	/* '-' */ 0x002D,  55, 0,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* '<' */ 0x003C,  53, MO_AltRight,
	/* '>' */ 0x003E,  54, MO_AltRight,
	/* '\' */ 0x005C,  45, 0,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '{' */ 0x007B,  10, MO_AltRight,
	/* '|' */ 0x007C,  45, MO_ShiftLeft,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* '~' */ 0x007E,   1, MO_ShiftLeft,
	/* '°' */ 0x00B0,  13, MO_AltRight,
	/* 'ß' */ 0x00DF, NOS, 0,
	/* 'ä' */ 0x00E4, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_Brazilian[3] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '\' */ 0x005C,  45, 0,
	/* '|' */ 0x007C,  45, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_Greek_DEC[7] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '<' */ 0x003C,  45, 0,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '\' */ 0x005C,  42, 0,
	/* '|' */ 0x007C,  42, MO_ShiftLeft,
	/* '¥' */ 0x00A5, NOS, 0,
	/* 'Å' */ 0x00C5, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_Canadian_French[39] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '#' */ 0x0023,  28, 0,
	/* ''' */ 0x0027, NOS, 0,
	/* '<' */ 0x003C,  45, 0,
	/* '>' */ 0x003E,  45, MO_ShiftLeft,
	/* '@' */ 0x0040,  28, MO_ShiftLeft,
	/* '[' */ 0x005B,  27, MO_ShiftLeft|MO_AltRight,
	/* '\' */ 0x005C,  42, 0,
	/* ']' */ 0x005D,  27, MO_AltRight,
	/* '^' */ 0x005E,  41, MO_ShiftLeft,
	/* '`' */ 0x0060,  41, 0,
	/* '{' */ 0x007B, NOS, 0,
	/* '|' */ 0x007C,  42, MO_ShiftLeft,
	/* '}' */ 0x007D, NOS, 0,
	/* '~' */ 0x007E,   1, 0,
	/* '¢' */ 0x00A2, NOS, 0,
	/* '£' */ 0x00A3, NOS, 0,
	/* '¤' */ 0x00A4, NOS, 0,
	/* '¦' */ 0x00A6, NOS, 0,
	/* '§' */ 0x00A7, NOS, 0,
	/* '¨' */ 0x00A8, NOS, 0,
	/* '«' */ 0x00AB, NOS, 0,
	/* '¬' */ 0x00AC, NOS, 0,
	/* '­' */ 0x00AD, NOS, 0,
	/* '¯' */ 0x00AF, NOS, 0,
	/* '°' */ 0x00B0,   1, MO_ShiftLeft,
	/* '±' */ 0x00B1, NOS, 0,
	/* '²' */ 0x00B2, NOS, 0,
	/* '³' */ 0x00B3, NOS, 0,
	/* '´' */ 0x00B4,  53, MO_ShiftLeft,
	/* 'µ' */ 0x00B5, NOS, 0,
	/* '¶' */ 0x00B6, NOS, 0,
	/* '¸' */ 0x00B8, NOS, 0,
	/* '»' */ 0x00BB, NOS, 0,
	/* '¼' */ 0x00BC, NOS, 0,
	/* '½' */ 0x00BD, NOS, 0,
	/* '¾' */ 0x00BE, NOS, 0,
	/* 'Ç' */ 0x00C7,  27, MO_ShiftLeft,
	/* 'ç' */ 0x00E7,  27, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_Hungarian[56] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   2, MO_ShiftLeft|MO_AltRight,
	/* '#' */ 0x0023,   4, MO_ShiftLeft|MO_AltRight,
	/* '$' */ 0x0024,   5, MO_ShiftLeft|MO_AltRight,
	/* '&' */ 0x0026,   8, MO_ShiftLeft|MO_AltRight,
	/* '(' */ 0x0028,  10, MO_ShiftLeft,
	/* ')' */ 0x0029,  11, MO_ShiftLeft,
	/* '*' */ 0x002A,   9, MO_ShiftLeft,
	/* '-' */ 0x002D,  12, MO_AltRight,
	/* '0' */ 0x0030,  11, 0,
	/* ':' */ 0x003A,  40, MO_ShiftLeft|MO_AltRight,
	/* ';' */ 0x003B,  40, MO_AltRight,
	/* '>' */ 0x003E,  45, MO_ShiftLeft|MO_AltRight,
	/* '@' */ 0x0040,   3, MO_ShiftLeft|MO_AltRight,
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,  42, MO_AltRight,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '^' */ 0x005E,   7, MO_ShiftLeft|MO_AltRight,
	/* '_' */ 0x005F,  12, MO_ShiftLeft|MO_AltRight,
	/* '`' */ 0x0060, NOS, 0,
	/* '{' */ 0x007B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x007C,  42, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x007D,  28, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x007E,   1, MO_ShiftLeft|MO_AltRight,
	/* '¤' */ 0x00A4, NOS, 0,
	/* '§' */ 0x00A7, NOS, 0,
	/* '¨' */ 0x00A8, NOS, 0,
	/* '°' */ 0x00B0, NOS, 0,
	/* '´' */ 0x00B4, NOS, 0,
	/* '¸' */ 0x00B8, NOS, 0,
	/* 'Ä' */ 0x00C4, NOS, 0,
	/* 'Í' */ 0x00CD,  45, MO_ShiftLeft,
	/* 'Ó' */ 0x00D3,   1, MO_ShiftLeft,
	/* 'Ö' */ 0x00D6,  12, MO_ShiftLeft,
	/* '×' */ 0x00D7, NOS, 0,
	/* 'Ü' */ 0x00DC,  13, MO_ShiftLeft,
	/* 'ß' */ 0x00DF, NOS, 0,
	/* 'ä' */ 0x00E4, NOS, 0,
	/* 'í' */ 0x00ED,  45, 0,
	/* 'ó' */ 0x00F3,   1, 0,
	/* 'ö' */ 0x00F6,  12, 0,
	/* '÷' */ 0x00F7, NOS, 0,
	/* 'ü' */ 0x00FC,  13, 0,
	/* ... */ 0x0010, NOS, 0,									// <control> (DATA LINK ESCAPE)
	/* ... */ 0x0011, NOS, 0,									// <control> (DEVICE CONTROL ONE)
	/* 'A' */ 0x0041, NOS, 0,
	/* 'B' */ 0x0042, NOS, 0,
	/* 'p' */ 0x0070, NOS, 0,
	/* 'q' */ 0x0071, NOS, 0,
	/* 'Ç' */ 0x00C7, NOS, 0,
	/* 'Ø' */ 0x00D8, NOS, 0,
	/* 'Ù' */ 0x00D9, NOS, 0,
	/* 'Û' */ 0x00DB, NOS, 0,
	/* 'Ý' */ 0x00DD, NOS, 0,
	/* '‘' */ 0x2018,   1, MO_AltRight,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_Polish[41] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,  41, MO_ShiftLeft,
	/* '#' */ 0x0023,   4, MO_ShiftLeft,
	/* '$' */ 0x0024,   5, MO_ShiftLeft,
	/* '&' */ 0x0026,   8, MO_ShiftLeft,
	/* ''' */ 0x0027,  41, 0,
	/* '(' */ 0x0028,  10, MO_ShiftLeft,
	/* ')' */ 0x0029,  11, MO_ShiftLeft,
	/* '*' */ 0x002A,   9, MO_ShiftLeft,
	/* '+' */ 0x002B,  13, MO_ShiftLeft,
	/* '-' */ 0x002D,  12, 0,
	/* '/' */ 0x002F,  55, 0,
	/* ':' */ 0x003A,  40, MO_ShiftLeft,
	/* ';' */ 0x003B,  40, 0,
	/* '=' */ 0x003D,  13, 0,
	/* '?' */ 0x003F,  55, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_ShiftLeft,
	/* '[' */ 0x005B,  27, 0,
	/* '\' */ 0x005C,  42, 0,
	/* ']' */ 0x005D,  28, 0,
	/* '^' */ 0x005E,   7, MO_ShiftLeft,
	/* '_' */ 0x005F,  12, MO_ShiftLeft,
	/* '{' */ 0x007B,  27, MO_ShiftLeft,
	/* '|' */ 0x007C,  42, MO_ShiftLeft,
	/* '}' */ 0x007D,  28, MO_ShiftLeft,
	/* '¤' */ 0x00A4, NOS, 0,
	/* '§' */ 0x00A7, NOS, 0,
	/* '´' */ 0x00B4, NOS, 0,
	/* '·' */ 0x00B7, NOS, 0,
	/* 'ó' */ 0x00F3,  47, MO_AltRight,
	/* ??? */ 0x0105,  31, MO_AltRight,						// LATIN SMALL LETTER A WITH OGONEK
	/* ??? */ 0x0107,  48, MO_AltRight,						// LATIN SMALL LETTER C WITH ACUTE
	/* ??? */ 0x0119,  19, MO_AltRight,						// LATIN SMALL LETTER E WITH OGONEK
	/* ??? */ 0x0141,  39, MO_ShiftLeft|MO_AltRight,	// LATIN CAPITAL LETTER L WITH STROKE
	/* ??? */ 0x0142,  39, MO_AltRight,						// LATIN SMALL LETTER L WITH STROKE
	/* ??? */ 0x0144,  51, MO_AltRight,						// LATIN SMALL LETTER N WITH ACUTE
	/* ??? */ 0x015B,  32, MO_AltRight,						// LATIN SMALL LETTER S WITH ACUTE
	/* ??? */ 0x017A,  47, MO_ShiftLeft|MO_AltRight,	// LATIN SMALL LETTER Z WITH ACUTE
	/* ??? */ 0x017B,  46, MO_ShiftLeft|MO_AltRight,	// LATIN CAPITAL LETTER Z WITH DOT ABOVE
	/* ??? */ 0x017C,  46, MO_AltRight,						// LATIN SMALL LETTER Z WITH DOT ABOVE
	/* '‘' */ 0x2018, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_SCS[3] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '\' */ 0x005C,  42, MO_AltRight,
	/* '|' */ 0x007C,  42, MO_ShiftLeft|MO_AltRight,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_Slovakian_DEC[20] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,  41, MO_ShiftLeft,
	/* '(' */ 0x0028,  28, MO_ShiftLeft,
	/* ')' */ 0x0029,  42, MO_ShiftLeft,
	/* '-' */ 0x002D,  55, 0,
	/* ':' */ 0x003A,  54, MO_ShiftLeft,
	/* '<' */ 0x003C,  53, MO_AltRight,
	/* '>' */ 0x003E,  54, MO_AltRight,
	/* 'Y' */ 0x0059,  46, MO_ShiftLeft,
	/* '\' */ 0x005C,  45, 0,
	/* '_' */ 0x005F,  55, MO_ShiftLeft,
	/* '`' */ 0x0060,   1, 0,
	/* 'y' */ 0x0079,  46, 0,
	/* '{' */ 0x007B,  10, MO_AltRight,
	/* '|' */ 0x007C,  45, MO_ShiftLeft,
	/* '}' */ 0x007D,  11, MO_AltRight,
	/* '~' */ 0x007E,   1, MO_ShiftLeft,
	/* '¨' */ 0x00A8,  13, MO_AltRight,
	/* 'ß' */ 0x00DF, NOS, 0,
	/* 'o' */ 0x006F, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_Swedish[23] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '#' */ 0x0023,  28, MO_AltRight,
	/* '$' */ 0x0024,   5, MO_ShiftLeft,
	/* '@' */ 0x0040,   4, MO_ShiftLeft|MO_AltRight,
	/* '[' */ 0x005B, NOS, 0,
	/* '\' */ 0x005C,  28, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x005D,  27, MO_ShiftLeft|MO_AltRight,
	/* '^' */ 0x005E,  13, 0,
	/* '`' */ 0x0060,  13, MO_ShiftLeft,
	/* '{' */ 0x007B,  40, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x007C,  41, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x007D,  27, MO_AltRight,
	/* '~' */ 0x007E,   1, 0,
	/* '£' */ 0x00A3, NOS, 0,
	/* '¤' */ 0x00A4, NOS, 0,
	/* '§' */ 0x00A7,   4, MO_ShiftLeft,
	/* '¨' */ 0x00A8, NOS, 0,
	/* '°' */ 0x00B0,   1, MO_ShiftLeft,
	/* '´' */ 0x00B4, NOS, 0,
	/* '½' */ 0x00BD, NOS, 0,
	/* 'Ü' */ 0x00DC,  28, MO_ShiftLeft,
	/* 'ü' */ 0x00FC,  28, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_Turkish_Q[45] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,  28, MO_ShiftLeft,
	/* '#' */ 0x0023,  12, MO_ShiftLeft|MO_AltRight,
	/* '$' */ 0x0024,   5, MO_ShiftLeft,
	/* '&' */ 0x0026,  13, MO_ShiftLeft|MO_AltRight,
	/* '(' */ 0x0028,  10, MO_ShiftLeft,
	/* ')' */ 0x0029,  11, MO_ShiftLeft,
	/* '*' */ 0x002A,   9, MO_ShiftLeft,
	/* '+' */ 0x002B,  42, MO_ShiftLeft,
	/* ',' */ 0x002C,  55, 0,
	/* '-' */ 0x002D,  55, MO_ShiftLeft,
	/* '.' */ 0x002E,  54, 0,
	/* '/' */ 0x002F,   4, MO_ShiftLeft,
	/* ':' */ 0x003A,  27, MO_ShiftLeft,
	/* ';' */ 0x003B,  27, 0,
	/* '=' */ 0x003D,  42, 0,
	/* '?' */ 0x003F,  54, MO_ShiftLeft,
	/* '@' */ 0x0040,   1, MO_AltRight,
	/* '[' */ 0x005B,  40, MO_AltRight,
	/* '\' */ 0x005C,  53, MO_AltRight,
	/* ']' */ 0x005D,  41, MO_AltRight,
	/* '^' */ 0x005E,   7, MO_ShiftLeft,
	/* '_' */ 0x005F,   8, MO_ShiftLeft,
	/* '`' */ 0x0060, NOS, 0,
	/* 'i' */ 0x0069,  24, 0,
	/* '{' */ 0x007B,  40, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x007C,  53, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x007D,  41, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x007E,   1, MO_ShiftLeft|MO_AltRight,
	/* '¨' */ 0x00A8, NOS, 0,
	/* '´' */ 0x00B4, NOS, 0,
	/* 'Æ' */ 0x00C6, NOS, 0,
	/* 'Ç' */ 0x00C7,  41, MO_ShiftLeft,
	/* 'Ö' */ 0x00D6,  13, MO_ShiftLeft,
	/* 'Ü' */ 0x00DC,  12, MO_ShiftLeft,
	/* 'ß' */ 0x00DF, NOS, 0,
	/* 'æ' */ 0x00E6, NOS, 0,
	/* 'ç' */ 0x00E7,  41, 0,
	/* 'é' */ 0x00E9, NOS, 0,
	/* 'ö' */ 0x00F6,  13, 0,
	/* 'ü' */ 0x00FC,  12, 0,
	/* ??? */ 0x011E,   1, MO_ShiftLeft,					// LATIN CAPITAL LETTER G WITH BREVE
	/* ??? */ 0x011F,   1, 0,									// LATIN SMALL LETTER G WITH BREVE
	/* ??? */ 0x0130,  53, MO_ShiftLeft,					// LATIN CAPITAL LETTER I WITH DOT ABOVE
	/* ??? */ 0x0131,  53, 0,									// LATIN SMALL LETTER DOTLESS I
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_Romanian[56] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,  41, MO_ShiftLeft,
	/* '$' */ 0x0024,   5, MO_ShiftLeft,
	/* '&' */ 0x0026,   8, MO_ShiftLeft,
	/* ''' */ 0x0027,  41, 0,
	/* '(' */ 0x0028,  10, MO_ShiftLeft,
	/* ')' */ 0x0029,  11, MO_ShiftLeft,
	/* '*' */ 0x002A,   9, MO_ShiftLeft,
	/* '+' */ 0x002B,  13, MO_ShiftLeft,
	/* '-' */ 0x002D,  12, 0,
	/* '/' */ 0x002F,  55, 0,
	/* ':' */ 0x003A,  40, MO_ShiftLeft,
	/* ';' */ 0x003B,  40, 0,
	/* '<' */ 0x003C,  54, MO_ShiftLeft,
	/* '=' */ 0x003D,  13, 0,
	/* '>' */ 0x003E,  53, MO_ShiftLeft,
	/* '?' */ 0x003F,  55, MO_ShiftLeft,
	/* '@' */ 0x0040,   3, MO_ShiftLeft,
	/* 'Y' */ 0x0059,  22, MO_ShiftLeft,
	/* 'Z' */ 0x005A,  46, MO_ShiftLeft,
	/* '[' */ 0x005B,  27, MO_AltRight,
	/* '\' */ 0x005C,  42, MO_AltRight,
	/* ']' */ 0x005D,  28, MO_AltRight,
	/* '^' */ 0x005E,   7, MO_ShiftLeft,
	/* '_' */ 0x005F,  12, MO_ShiftLeft,
	/* '`' */ 0x0060,   1, MO_AltRight,
	/* 'y' */ 0x0079,  22, 0,
	/* 'z' */ 0x007A,  46, 0,
	/* '{' */ 0x007B,  27, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x007C,  42, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x007D,  28, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x007E,   1, MO_ShiftLeft|MO_AltRight,
	/* '¤' */ 0x00A4, NOS, 0,
	/* '§' */ 0x00A7, NOS, 0,
	/* '¨' */ 0x00A8, NOS, 0,
	/* '°' */ 0x00B0, NOS, 0,
	/* '´' */ 0x00B4, NOS, 0,
	/* '·' */ 0x00B7, NOS, 0,
	/* '¸' */ 0x00B8, NOS, 0,
	/* 'Â' */ 0x00C2,   1, MO_ShiftLeft,
	/* '×' */ 0x00D7, NOS, 0,
	/* 'ß' */ 0x00DF, NOS, 0,
	/* 'â' */ 0x00E2,   1, 0,
	/* '÷' */ 0x00F7, NOS, 0,
	/* ... */ 0x0010, NOS, 0,									// <control> (DATA LINK ESCAPE)
	/* ... */ 0x0011, NOS, 0,									// <control> (DEVICE CONTROL ONE)
	/* 'A' */ 0x0041, NOS, 0,
	/* 'B' */ 0x0042, NOS, 0,
	/* ??? */ 0x015E,  45, MO_ShiftLeft,					// LATIN CAPITAL LETTER S WITH CEDILLA
	/* ??? */ 0x015F,  45, 0,									// LATIN SMALL LETTER S WITH CEDILLA
	/* ??? */ 0x0162,  41, MO_ShiftLeft|MO_AltRight,	// LATIN CAPITAL LETTER T WITH CEDILLA
	/* ??? */ 0x0163,  41, MO_AltRight,						// LATIN SMALL LETTER T WITH CEDILLA
	/* ??? */ 0x02C7, NOS, 0,									// CARON
	/* ??? */ 0x02D8, NOS, 0,									// BREVE
	/* ??? */ 0x02DB, NOS, 0,									// OGONEK
	/* ??? */ 0x02DD, NOS, 0,									// DOUBLE ACUTE ACCENT
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_Russian[142] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   3, MO_AltRight,
	/* '#' */ 0x0023,   4, MO_ShiftLeft,
	/* '$' */ 0x0024,   5, MO_ShiftLeft,
	/* '&' */ 0x0026,   8, MO_ShiftLeft,
	/* ''' */ 0x0027,   7, MO_AltRight,
	/* ',' */ 0x002C,  53, 0,
	/* '.' */ 0x002E,  54, 0,
	/* '/' */ 0x002F,   4, MO_AltRight,
	/* ':' */ 0x003A,   6, MO_AltRight,
	/* ';' */ 0x003B,   9, MO_AltRight,
	/* '?' */ 0x003F,  10, MO_AltRight,
	/* '@' */ 0x0040,   3, MO_ShiftLeft,
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
	/* '[' */ 0x005B,  27, 0,
	/* '\' */ 0x005C,  42, 0,
	/* ']' */ 0x005D,  28, 0,
	/* '^' */ 0x005E,   7, MO_ShiftLeft,
	/* '`' */ 0x0060,   1, 0,
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
	/* '{' */ 0x007B,  27, MO_ShiftLeft,
	/* '|' */ 0x007C,  42, MO_ShiftLeft,
	/* '}' */ 0x007D,  28, MO_ShiftLeft,
	/* '~' */ 0x007E,   1, MO_ShiftLeft,
	/* '§' */ 0x00A7,  12, MO_AltRight,
	/* ??? */ 0x0401,  55, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER IO
	/* ??? */ 0x0410,  34, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER A
	/* ??? */ 0x0411,  53, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER BE
	/* ??? */ 0x0412,  33, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER VE
	/* ??? */ 0x0413,  23, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER GHE
	/* ??? */ 0x0414,  39, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER DE
	/* ??? */ 0x0415,  21, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER IE
	/* ??? */ 0x0416,  40, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER ZHE
	/* ??? */ 0x0417,  26, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER ZE
	/* ??? */ 0x0418,  50, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER I
	/* ??? */ 0x0419,  17, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER SHORT I
	/* ??? */ 0x041A,  20, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER KA
	/* ??? */ 0x041B,  38, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER EL
	/* ??? */ 0x041C,  49, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER EM
	/* ??? */ 0x041D,  22, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER EN
	/* ??? */ 0x041E,  37, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER O
	/* ??? */ 0x041F,  35, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER PE
	/* ??? */ 0x0420,  36, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER ER
	/* ??? */ 0x0421,  48, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER ES
	/* ??? */ 0x0422,  51, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER TE
	/* ??? */ 0x0423,  19, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER U
	/* ??? */ 0x0424,  31, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER EF
	/* ??? */ 0x0425,  27, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER HA
	/* ??? */ 0x0426,  18, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER TSE
	/* ??? */ 0x0427,  47, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER CHE
	/* ??? */ 0x0428,  24, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER SHA
	/* ??? */ 0x0429,  25, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER SHCHA
	/* ??? */ 0x042A,  28, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER HARD SIGN
	/* ??? */ 0x042B,  32, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER YERU
	/* ??? */ 0x042C,  52, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER SOFT SIGN
	/* ??? */ 0x042D,  41, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER E
	/* ??? */ 0x042E,  54, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER YU
	/* ??? */ 0x042F,  46, MO_ShiftLeft|MO_AltRight,	// CYRILLIC CAPITAL LETTER YA
	/* ??? */ 0x0430,  34, MO_AltRight,						// CYRILLIC SMALL LETTER A
	/* ??? */ 0x0431,  53, MO_AltRight,						// CYRILLIC SMALL LETTER BE
	/* ??? */ 0x0432,  33, MO_AltRight,						// CYRILLIC SMALL LETTER VE
	/* ??? */ 0x0433,  23, MO_AltRight,						// CYRILLIC SMALL LETTER GHE
	/* ??? */ 0x0434,  39, MO_AltRight,						// CYRILLIC SMALL LETTER DE
	/* ??? */ 0x0435,  21, MO_AltRight,						// CYRILLIC SMALL LETTER IE
	/* ??? */ 0x0436,  40, MO_AltRight,						// CYRILLIC SMALL LETTER ZHE
	/* ??? */ 0x0437,  26, MO_AltRight,						// CYRILLIC SMALL LETTER ZE
	/* ??? */ 0x0438,  50, MO_AltRight,						// CYRILLIC SMALL LETTER I
	/* ??? */ 0x0439,  17, MO_AltRight,						// CYRILLIC SMALL LETTER SHORT I
	/* ??? */ 0x043A,  20, MO_AltRight,						// CYRILLIC SMALL LETTER KA
	/* ??? */ 0x043B,  38, MO_AltRight,						// CYRILLIC SMALL LETTER EL
	/* ??? */ 0x043C,  49, MO_AltRight,						// CYRILLIC SMALL LETTER EM
	/* ??? */ 0x043D,  22, MO_AltRight,						// CYRILLIC SMALL LETTER EN
	/* ??? */ 0x043E,  37, MO_AltRight,						// CYRILLIC SMALL LETTER O
	/* ??? */ 0x043F,  35, MO_AltRight,						// CYRILLIC SMALL LETTER PE
	/* ??? */ 0x0440,  36, MO_AltRight,						// CYRILLIC SMALL LETTER ER
	/* ??? */ 0x0441,  48, MO_AltRight,						// CYRILLIC SMALL LETTER ES
	/* ??? */ 0x0442,  51, MO_AltRight,						// CYRILLIC SMALL LETTER TE
	/* ??? */ 0x0443,  19, MO_AltRight,						// CYRILLIC SMALL LETTER U
	/* ??? */ 0x0444,  31, MO_AltRight,						// CYRILLIC SMALL LETTER EF
	/* ??? */ 0x0445,  27, MO_AltRight,						// CYRILLIC SMALL LETTER HA
	/* ??? */ 0x0446,  18, MO_AltRight,						// CYRILLIC SMALL LETTER TSE
	/* ??? */ 0x0447,  47, MO_AltRight,						// CYRILLIC SMALL LETTER CHE
	/* ??? */ 0x0448,  24, MO_AltRight,						// CYRILLIC SMALL LETTER SHA
	/* ??? */ 0x0449,  25, MO_AltRight,						// CYRILLIC SMALL LETTER SHCHA
	/* ??? */ 0x044A,  28, MO_AltRight,						// CYRILLIC SMALL LETTER HARD SIGN
	/* ??? */ 0x044B,  32, MO_AltRight,						// CYRILLIC SMALL LETTER YERU
	/* ??? */ 0x044C,  52, MO_AltRight,						// CYRILLIC SMALL LETTER SOFT SIGN
	/* ??? */ 0x044D,  41, MO_AltRight,						// CYRILLIC SMALL LETTER E
	/* ??? */ 0x044E,  54, MO_AltRight,						// CYRILLIC SMALL LETTER YU
	/* ??? */ 0x044F,  46, MO_AltRight,						// CYRILLIC SMALL LETTER YA
	/* ??? */ 0x0451,  55, MO_AltRight,						// CYRILLIC SMALL LETTER IO
	/* ??? */ 0x2116,   5, MO_AltRight,						// NUMERO SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_Turkish_F[47] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,   4, MO_ShiftLeft,
	/* '#' */ 0x0023,   1, MO_AltRight,
	/* '$' */ 0x0024,  12, 0,
	/* '%' */ 0x0025,  10, MO_ShiftLeft,
	/* '&' */ 0x0026,  41, MO_AltRight,
	/* ''' */ 0x0027,   5, MO_ShiftLeft,
	/* '(' */ 0x0028,   6, MO_ShiftLeft,
	/* ')' */ 0x0029,   8, MO_ShiftLeft,
	/* '-' */ 0x002D,   9, MO_ShiftLeft,
	/* '/' */ 0x002F,   7, MO_ShiftLeft,
	/* ':' */ 0x003A,  11, MO_ShiftLeft,
	/* '=' */ 0x003D,  13, 0,
	/* '?' */ 0x003F,  54, MO_ShiftLeft,
	/* '@' */ 0x0040,  12, MO_ShiftLeft,
	/* 'I' */ 0x0049,  20, MO_ShiftLeft,
	/* '[' */ 0x005B,  47, MO_AltRight,
	/* '\' */ 0x005C,  32, MO_AltRight,
	/* ']' */ 0x005D,  50, MO_AltRight,
	/* '^' */ 0x005E,   3, MO_ShiftLeft,
	/* '`' */ 0x0060, NOS, 0,
	/* 'i' */ 0x0069,  20, 0,
	/* '{' */ 0x007B,  47, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x007C,  32, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x007D,  50, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x007E,   1, MO_ShiftLeft|MO_AltRight,
	/* '¢' */ 0x00A2, NOS, 0,
	/* '£' */ 0x00A3, NOS, 0,
	/* '¨' */ 0x00A8, NOS, 0,
	/* '«' */ 0x00AB, NOS, 0,
	/* '¬' */ 0x00AC, NOS, 0,
	/* '²' */ 0x00B2, NOS, 0,
	/* '´' */ 0x00B4, NOS, 0,
	/* 'µ' */ 0x00B5, NOS, 0,
	/* '¶' */ 0x00B6, NOS, 0,
	/* '¹' */ 0x00B9, NOS, 0,
	/* 'º' */ 0x00BA, NOS, 0,
	/* '»' */ 0x00BB, NOS, 0,
	/* '¼' */ 0x00BC, NOS, 0,
	/* '½' */ 0x00BD, NOS, 0,
	/* '¾' */ 0x00BE, NOS, 0,
	/* 'Æ' */ 0x00C6, NOS, 0,
	/* '×' */ 0x00D7, NOS, 0,
	/* 'ß' */ 0x00DF, NOS, 0,
	/* 'æ' */ 0x00E6, NOS, 0,
	/* '÷' */ 0x00F7, NOS, 0,
	/* ??? */ 0x0131,  32, 0,									// LATIN SMALL LETTER DOTLESS I
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=104
const CKeyFixup DEC_VT510_LK411_Swiss_French[20] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   1, 0,
	/* '#' */ 0x0023,  42, MO_ShiftLeft|MO_AltRight,
	/* ''' */ 0x0027,  12, 0,
	/* '?' */ 0x003F,  12, MO_ShiftLeft,
	/* '@' */ 0x0040,   5, MO_ShiftLeft|MO_AltRight,
	/* '[' */ 0x005B,  40, MO_ShiftLeft|MO_AltRight,
	/* '\' */ 0x005C,  27, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x005D,  41, MO_ShiftLeft|MO_AltRight,
	/* '^' */ 0x005E,  13, 0,
	/* '`' */ 0x0060,  13, MO_ShiftLeft,
	/* '{' */ 0x007B,  40, MO_AltRight,
	/* '|' */ 0x007C,  27, MO_AltRight,
	/* '}' */ 0x007D,  41, MO_AltRight,
	/* '~' */ 0x007E,  28, MO_ShiftLeft,
	/* '¦' */ 0x00A6, NOS, 0,
	/* '§' */ 0x00A7, NOS, 0,
	/* '¨' */ 0x00A8,  28, 0,
	/* '´' */ 0x00B4, NOS, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=112 	,114
const CKeyFixup DEC_VT510_LK411_Slovenian[14] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '\' */ 0x005C,  17, MO_AltRight,
	/* '^' */ 0x005E,   4, MO_AltRight,
	/* '|' */ 0x007C,  18, MO_AltRight,
	/* '¨' */ 0x00A8,   1, MO_ShiftLeft,
	/* '°' */ 0x00B0,   6, MO_AltRight,
	/* '´' */ 0x00B4,  10, MO_AltRight,
	/* '·' */ 0x00B7,   9, MO_AltRight,
	/* '¸' */ 0x00B8,   1, 0,
	/* ??? */ 0x02C7,   3, MO_AltRight,						// CARON
	/* ??? */ 0x02D8,   5, MO_AltRight,						// BREVE
	/* 'Ù' */ 0x00D9, NOS, 0,
	/* 'Û' */ 0x00DB, NOS, 0,
	/* 'Ý' */ 0x00DD, NOS, 0,
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=125
const CKeyFixup Telex_88_DATA_ENTRY_USA[3] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '¢' */ 0x00A2,  20, MO_ShiftLeft,
	/* '¬' */ 0x00AC,  35, MO_ShiftLeft,
	/* end */ 0,KN_ENDMARKER,0
};

const CKeyFixup APPLE_USB_USA[15] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '£' */ 0x00A3,   4, MO_AltRight,
	/* '¥' */ 0x00A5,  22, MO_AltRight,
	/* '§' */ 0x00A7,   1, MO_AltRight,
	/* '±' */ 0x00B1,   1, MO_ShiftLeft|MO_AltRight,
	/* 'µ' */ 0x00B5,  52, MO_AltRight,
	/* '¶' */ 0x00B6,   8, MO_AltRight,
	/* 'Å' */ 0x00C5,  31, MO_ShiftLeft|MO_AltRight,
	/* 'Ç' */ 0x00C7,  48, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0x00DF,  32, MO_AltRight,
	/* 'å' */ 0x00E5,  31, MO_AltRight,
	/* 'ç' */ 0x00E7,  48, MO_AltRight,
	/* 'ƒ' */ 0x0192,  34, MO_AltRight,
	/* '€' */ 0x20AC,   3, MO_ShiftLeft|MO_AltRight,
	/* ... */ 0x2122,   3, MO_AltRight,						// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=125
const CKeyFixup APPLE_USB_Belgian[80] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '#' */ 0x0023,  56, MO_ShiftLeft,
	/* '@' */ 0x0040,  56, 0,
	/* '[' */ 0x005B,   6, MO_ShiftLeft|MO_AltRight,
	/* '\' */ 0x005C,  54, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x005D,  12, MO_ShiftLeft|MO_AltRight,
	/* '^' */ 0x005E,  27, 0,
	/* '`' */ 0x0060,  42, 0,
	/* '{' */ 0x007B,   6, MO_AltRight,
	/* '|' */ 0x007C,  39, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x007D,  12, MO_AltRight,
	/* '~' */ 0x007E,  51, MO_AltRight,
	/* ... */ 0x007F,  15, 0,									// <control> (DELETE)
	/* '¡' */ 0x00A1,   9, MO_AltRight,
	/* '¢' */ 0x00A2,  48, MO_ShiftLeft|MO_AltRight,
	/* '£' */ 0x00A3,  42, MO_ShiftLeft,
	/* '¥' */ 0x00A5,  28, MO_ShiftLeft|MO_AltRight,
	/* '§' */ 0x00A7,   7, 0,
	/* '¨' */ 0x00A8,  27, MO_ShiftLeft,
	/* '©' */ 0x00A9,  48, MO_AltRight,
	/* 'ª' */ 0x00AA,  23, MO_ShiftLeft|MO_AltRight,
	/* '«' */ 0x00AB,   8, MO_AltRight,
	/* '¬' */ 0x00AC,  39, MO_AltRight,
	/* '®' */ 0x00AE,  20, MO_AltRight,
	/* '±' */ 0x00B1,  55, MO_ShiftLeft|MO_AltRight,
	/* '²' */ 0x00B2, NOS, 0,
	/* '³' */ 0x00B3, NOS, 0,
	/* '´' */ 0x00B4,   2, MO_ShiftLeft|MO_AltRight,
	/* 'µ' */ 0x00B5,  40, MO_AltRight,
	/* '¶' */ 0x00B6,   7, MO_AltRight,
	/* 'º' */ 0x00BA,  23, MO_AltRight,
	/* '»' */ 0x00BB,   8, MO_ShiftLeft|MO_AltRight,
	/* '¿' */ 0x00BF,  52, MO_ShiftLeft|MO_AltRight,
	/* 'Á' */ 0x00C1,  10, MO_ShiftLeft|MO_AltRight,
	/* 'Â' */ 0x00C2,  18, MO_AltRight,
	/* 'Å' */ 0x00C5,  18, MO_ShiftLeft|MO_AltRight,
	/* 'Æ' */ 0x00C6,  17, MO_ShiftLeft|MO_AltRight,
	/* 'Ç' */ 0x00C7,  10, MO_AltRight,
	/* 'È' */ 0x00C8,  38, MO_AltRight,
	/* 'Ê' */ 0x00CA,  19, MO_ShiftLeft|MO_AltRight,
	/* 'Ë' */ 0x00CB,  38, MO_ShiftLeft|MO_AltRight,
	/* 'Ì' */ 0x00CC,  36, MO_AltRight,
	/* 'Í' */ 0x00CD,  37, MO_ShiftLeft|MO_AltRight,
	/* 'Î' */ 0x00CE,  36, MO_ShiftLeft|MO_AltRight,
	/* 'Ï' */ 0x00CF,  37, MO_AltRight,
	/* 'Ò' */ 0x00D2,  32, MO_AltRight,
	/* 'Ó' */ 0x00D3,  40, MO_ShiftLeft|MO_AltRight,
	/* 'Ô' */ 0x00D4,  27, MO_ShiftLeft|MO_AltRight,
	/* 'Ø' */ 0x00D8,  11, MO_ShiftLeft|MO_AltRight,
	/* 'Ù' */ 0x00D9,  41, MO_AltRight,
	/* 'Ú' */ 0x00DA,  22, MO_AltRight,
	/* 'Û' */ 0x00DB,   9, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0x00DF,  50, MO_AltRight,
	/* 'å' */ 0x00E5,   7, MO_ShiftLeft|MO_AltRight,
	/* 'æ' */ 0x00E6,  17, MO_AltRight,
	/* 'ç' */ 0x00E7,  10, 0,
	/* 'ê' */ 0x00EA,  19, MO_AltRight,
	/* 'ë' */ 0x00EB,   3, MO_AltRight,
	/* 'î' */ 0x00EE,  24, MO_AltRight,
	/* 'ï' */ 0x00EF,  24, MO_ShiftLeft|MO_AltRight,
	/* 'ô' */ 0x00F4,  27, MO_AltRight,
	/* '÷' */ 0x00F7,  54, MO_AltRight,
	/* 'ø' */ 0x00F8,  11, MO_AltRight,
	/* 'Œ' */ 0x0152,  25, MO_ShiftLeft|MO_AltRight,
	/* 'œ' */ 0x0153,  25, MO_AltRight,
	/* 'Ÿ' */ 0x0178,  22, MO_ShiftLeft|MO_AltRight,
	/* 'ƒ' */ 0x0192,  34, MO_AltRight,
	/* '‘' */ 0x2018,   5, MO_AltRight,
	/* '’' */ 0x2019,   5, MO_ShiftLeft|MO_AltRight,
	/* '‚' */ 0x201A,  28, MO_AltRight,
	/* '“' */ 0x201C,   4, MO_AltRight,
	/* '”' */ 0x201D,   4, MO_ShiftLeft|MO_AltRight,
	/* '„' */ 0x201E,   3, MO_ShiftLeft|MO_AltRight,
	/* '†' */ 0x2020,  21, MO_AltRight,
	/* '‡' */ 0x2021,  31, MO_AltRight,
	/* '•' */ 0x2022,  34, MO_ShiftLeft|MO_AltRight,
	/* '…' */ 0x2026,  53, MO_AltRight,
	/* '‰' */ 0x2030,  41, MO_ShiftLeft|MO_AltRight,
	/* '€' */ 0x20AC,  20, MO_ShiftLeft|MO_AltRight,
	/* ... */ 0x2122,  21, MO_ShiftLeft|MO_AltRight,	// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=125
const CKeyFixup APPLE_USB_Finnish_Swedish[50] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '@' */ 0x0040,  42, MO_AltRight,
	/* '\' */ 0x005C,   8, MO_ShiftLeft|MO_AltRight,
	/* '^' */ 0x005E,  28, MO_ShiftLeft,
	/* '`' */ 0x0060,  13, MO_ShiftLeft,
	/* '{' */ 0x007B,   9, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x007C,   8, MO_AltRight,
	/* '}' */ 0x007D,  10, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x007E,  28, MO_AltRight,
	/* ... */ 0x007F,  15, 0,									// <control> (DELETE)
	/* '¡' */ 0x00A1,   2, MO_ShiftLeft|MO_AltRight,
	/* '£' */ 0x00A3,   4, MO_AltRight,
	/* '¤' */ 0x00A4, NOS, 0,
	/* '¥' */ 0x00A5, NOS, 0,
	/* '§' */ 0x00A7,   7, MO_AltRight,
	/* '¨' */ 0x00A8,  28, 0,
	/* '©' */ 0x00A9,   2, MO_AltRight,
	/* 'ª' */ 0x00AA,  38, MO_AltRight,
	/* '¬' */ 0x00AC,  37, MO_AltRight,
	/* '®' */ 0x00AE,  20, MO_AltRight,
	/* '°' */ 0x00B0,  56, MO_ShiftLeft,
	/* '±' */ 0x00B1, NOS, 0,
	/* '´' */ 0x00B4,  13, 0,
	/* 'µ' */ 0x00B5,  22, MO_AltRight,
	/* '¶' */ 0x00B6,  56, MO_AltRight,
	/* 'º' */ 0x00BA,  38, MO_ShiftLeft|MO_AltRight,
	/* '½' */ 0x00BD, NOS, 0,
	/* 'Å' */ 0x00C5,  27, MO_ShiftLeft,
	/* 'Æ' */ 0x00C6,  41, MO_ShiftLeft|MO_AltRight,
	/* 'Ç' */ 0x00C7,  48, MO_ShiftLeft|MO_AltRight,
	/* 'É' */ 0x00C9,  19, MO_ShiftLeft|MO_AltRight,
	/* 'Ø' */ 0x00D8,  40, MO_ShiftLeft|MO_AltRight,
	/* 'Ü' */ 0x00DC,  23, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0x00DF,  32, MO_AltRight,
	/* 'å' */ 0x00E5,  27, 0,
	/* 'æ' */ 0x00E6,  41, MO_AltRight,
	/* 'ç' */ 0x00E7,  48, MO_AltRight,
	/* 'é' */ 0x00E9,  19, MO_AltRight,
	/* 'ø' */ 0x00F8,  40, MO_AltRight,
	/* 'ü' */ 0x00FC,  23, MO_AltRight,
	/* 'Œ' */ 0x0152,  25, MO_ShiftLeft|MO_AltRight,
	/* 'œ' */ 0x0153,  25, MO_AltRight,
	/* 'ƒ' */ 0x0192,  34, MO_AltRight,
	/* '”' */ 0x201D,   3, MO_ShiftLeft|MO_AltRight,
	/* '†' */ 0x2020,  21, MO_AltRight,
	/* '‡' */ 0x2021,  21, MO_ShiftLeft|MO_AltRight,
	/* '•' */ 0x2022,  56, MO_ShiftLeft|MO_AltRight,
	/* '‰' */ 0x2030,   6, MO_ShiftLeft|MO_AltRight,
	/* '€' */ 0x20AC,   5, MO_ShiftLeft,
	/* ... */ 0x2122,   3, MO_AltRight,						// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=125
const CKeyFixup APPLE_USB_French[86] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,   9, 0,
	/* '#' */ 0x0023,  56, MO_ShiftLeft,
	/* '*' */ 0x002A,  28, MO_ShiftLeft,
	/* '+' */ 0x002B,  55, MO_ShiftLeft,
	/* '-' */ 0x002D,  13, 0,
	/* '=' */ 0x003D,  55, 0,
	/* '@' */ 0x0040,  56, 0,
	/* '[' */ 0x005B,   6, MO_ShiftLeft|MO_AltRight,
	/* '\' */ 0x005C,  54, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x005D,  12, MO_ShiftLeft|MO_AltRight,
	/* '^' */ 0x005E,  27, 0,
	/* '_' */ 0x005F,  13, MO_ShiftLeft,
	/* '`' */ 0x0060,  42, 0,
	/* '{' */ 0x007B,   6, MO_AltRight,
	/* '|' */ 0x007C,  39, MO_ShiftLeft|MO_AltRight,
	/* '}' */ 0x007D,  12, MO_AltRight,
	/* '~' */ 0x007E,  51, MO_AltRight,
	/* ... */ 0x007F,  15, 0,									// <control> (DELETE)
	/* '¡' */ 0x00A1,   9, MO_AltRight,
	/* '¢' */ 0x00A2,  48, MO_ShiftLeft|MO_AltRight,
	/* '£' */ 0x00A3,  42, MO_ShiftLeft,
	/* '¤' */ 0x00A4, NOS, 0,
	/* '¥' */ 0x00A5,  28, MO_ShiftLeft|MO_AltRight,
	/* '§' */ 0x00A7,   7, 0,
	/* '¨' */ 0x00A8,  27, MO_ShiftLeft,
	/* '©' */ 0x00A9,  48, MO_AltRight,
	/* 'ª' */ 0x00AA,  23, MO_ShiftLeft|MO_AltRight,
	/* '«' */ 0x00AB,   8, MO_AltRight,
	/* '¬' */ 0x00AC,  39, MO_AltRight,
	/* '®' */ 0x00AE,  20, MO_AltRight,
	/* '±' */ 0x00B1,  55, MO_ShiftLeft|MO_AltRight,
	/* '²' */ 0x00B2, NOS, 0,
	/* '´' */ 0x00B4,   2, MO_ShiftLeft|MO_AltRight,
	/* 'µ' */ 0x00B5,  40, MO_AltRight,
	/* '¶' */ 0x00B6,   7, MO_AltRight,
	/* 'º' */ 0x00BA,  23, MO_AltRight,
	/* '»' */ 0x00BB,   8, MO_ShiftLeft|MO_AltRight,
	/* '¿' */ 0x00BF,  52, MO_ShiftLeft|MO_AltRight,
	/* 'Á' */ 0x00C1,  10, MO_ShiftLeft|MO_AltRight,
	/* 'Â' */ 0x00C2,  18, MO_AltRight,
	/* 'Å' */ 0x00C5,  18, MO_ShiftLeft|MO_AltRight,
	/* 'Æ' */ 0x00C6,  17, MO_ShiftLeft|MO_AltRight,
	/* 'Ç' */ 0x00C7,  10, MO_AltRight,
	/* 'È' */ 0x00C8,  38, MO_AltRight,
	/* 'Ê' */ 0x00CA,  19, MO_ShiftLeft|MO_AltRight,
	/* 'Ë' */ 0x00CB,  38, MO_ShiftLeft|MO_AltRight,
	/* 'Ì' */ 0x00CC,  36, MO_AltRight,
	/* 'Í' */ 0x00CD,  37, MO_ShiftLeft|MO_AltRight,
	/* 'Î' */ 0x00CE,  36, MO_ShiftLeft|MO_AltRight,
	/* 'Ï' */ 0x00CF,  37, MO_AltRight,
	/* 'Ò' */ 0x00D2,  32, MO_AltRight,
	/* 'Ó' */ 0x00D3,  40, MO_ShiftLeft|MO_AltRight,
	/* 'Ô' */ 0x00D4,  27, MO_ShiftLeft|MO_AltRight,
	/* 'Ø' */ 0x00D8,  11, MO_ShiftLeft|MO_AltRight,
	/* 'Ù' */ 0x00D9,  41, MO_AltRight,
	/* 'Ú' */ 0x00DA,  22, MO_AltRight,
	/* 'Û' */ 0x00DB,   9, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0x00DF,  50, MO_AltRight,
	/* 'å' */ 0x00E5,   7, MO_ShiftLeft|MO_AltRight,
	/* 'æ' */ 0x00E6,  17, MO_AltRight,
	/* 'ç' */ 0x00E7,  10, 0,
	/* 'ê' */ 0x00EA,  19, MO_AltRight,
	/* 'ë' */ 0x00EB,   3, MO_AltRight,
	/* 'î' */ 0x00EE,  24, MO_AltRight,
	/* 'ï' */ 0x00EF,  24, MO_ShiftLeft|MO_AltRight,
	/* 'ô' */ 0x00F4,  27, MO_AltRight,
	/* '÷' */ 0x00F7,  54, MO_AltRight,
	/* 'ø' */ 0x00F8,  11, MO_AltRight,
	/* 'Œ' */ 0x0152,  25, MO_ShiftLeft|MO_AltRight,
	/* 'œ' */ 0x0153,  25, MO_AltRight,
	/* 'Ÿ' */ 0x0178,  22, MO_ShiftLeft|MO_AltRight,
	/* 'ƒ' */ 0x0192,  34, MO_AltRight,
	/* '‘' */ 0x2018,   5, MO_AltRight,
	/* '’' */ 0x2019,   5, MO_ShiftLeft|MO_AltRight,
	/* '‚' */ 0x201A,  28, MO_AltRight,
	/* '“' */ 0x201C,   4, MO_AltRight,
	/* '”' */ 0x201D,   4, MO_ShiftLeft|MO_AltRight,
	/* '„' */ 0x201E,   3, MO_ShiftLeft|MO_AltRight,
	/* '†' */ 0x2020,  21, MO_AltRight,
	/* '‡' */ 0x2021,  31, MO_AltRight,
	/* '•' */ 0x2022,  34, MO_ShiftLeft|MO_AltRight,
	/* '…' */ 0x2026,  53, MO_AltRight,
	/* '‰' */ 0x2030,  41, MO_ShiftLeft|MO_AltRight,
	/* '€' */ 0x20AC,  20, MO_ShiftLeft|MO_AltRight,
	/* ... */ 0x2122,  21, MO_ShiftLeft|MO_AltRight,	// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=125
const CKeyFixup APPLE_USB_German[66] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '@' */ 0x0040,   2, MO_ShiftLeft|MO_AltRight,
	/* '[' */ 0x005B,   6, MO_AltRight,
	/* '\' */ 0x005C,   8, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x005D,   7, MO_AltRight,
	/* '^' */ 0x005E,  56, 0,
	/* '`' */ 0x0060,  13, MO_ShiftLeft,
	/* '{' */ 0x007B,   9, MO_AltRight,
	/* '|' */ 0x007C,   8, MO_AltRight,
	/* '}' */ 0x007D,  10, MO_AltRight,
	/* '~' */ 0x007E,  51, MO_AltRight,
	/* ... */ 0x007F,  15, 0,									// <control> (DELETE)
	/* '¡' */ 0x00A1,   2, MO_AltRight,
	/* '¢' */ 0x00A2,   5, MO_AltRight,
	/* '£' */ 0x00A3,   5, MO_ShiftLeft|MO_AltRight,
	/* '¥' */ 0x00A5,  46, MO_AltRight,
	/* '§' */ 0x00A7,   4, MO_ShiftLeft,
	/* '¨' */ 0x00A8,  23, MO_AltRight,
	/* '©' */ 0x00A9,  35, MO_AltRight,
	/* 'ª' */ 0x00AA,  36, MO_AltRight,
	/* '«' */ 0x00AB,  17, MO_AltRight,
	/* '¬' */ 0x00AC,  39, MO_AltRight,
	/* '®' */ 0x00AE,  20, MO_AltRight,
	/* '°' */ 0x00B0,  56, MO_ShiftLeft,
	/* '±' */ 0x00B1,  28, MO_AltRight,
	/* '²' */ 0x00B2, NOS, 0,
	/* '³' */ 0x00B3, NOS, 0,
	/* '´' */ 0x00B4,  13, 0,
	/* 'µ' */ 0x00B5,  52, MO_AltRight,
	/* '¶' */ 0x00B6,   4, MO_AltRight,
	/* '·' */ 0x00B7,  10, MO_ShiftLeft|MO_AltRight,
	/* 'º' */ 0x00BA,  37, MO_AltRight,
	/* '»' */ 0x00BB,  17, MO_ShiftLeft|MO_AltRight,
	/* '¿' */ 0x00BF,  12, MO_AltRight,
	/* 'Á' */ 0x00C1,  23, MO_ShiftLeft|MO_AltRight,
	/* 'Å' */ 0x00C5,  31, MO_ShiftLeft|MO_AltRight,
	/* 'Æ' */ 0x00C6,  41, MO_ShiftLeft|MO_AltRight,
	/* 'Ç' */ 0x00C7,  48, MO_ShiftLeft|MO_AltRight,
	/* 'Ì' */ 0x00CC,  35, MO_ShiftLeft|MO_AltRight,
	/* 'Í' */ 0x00CD,  32, MO_ShiftLeft|MO_AltRight,
	/* 'Ï' */ 0x00CF,  34, MO_ShiftLeft|MO_AltRight,
	/* 'Ó' */ 0x00D3,  36, MO_ShiftLeft|MO_AltRight,
	/* 'Ø' */ 0x00D8,  25, MO_ShiftLeft|MO_AltRight,
	/* 'Ù' */ 0x00D9,  47, MO_ShiftLeft|MO_AltRight,
	/* 'Û' */ 0x00DB,  24, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0x00DF,  12, 0,
	/* 'å' */ 0x00E5,  31, MO_AltRight,
	/* 'æ' */ 0x00E6,  41, MO_AltRight,
	/* 'ç' */ 0x00E7,  48, MO_AltRight,
	/* '÷' */ 0x00F7,  54, MO_ShiftLeft|MO_AltRight,
	/* 'ø' */ 0x00F8,  25, MO_AltRight,
	/* 'Œ' */ 0x0152,  40, MO_ShiftLeft|MO_AltRight,
	/* 'œ' */ 0x0153,  40, MO_AltRight,
	/* 'ƒ' */ 0x0192,  34, MO_AltRight,
	/* '‘' */ 0x2018,  42, MO_AltRight,
	/* '’' */ 0x2019,  42, MO_ShiftLeft|MO_AltRight,
	/* '‚' */ 0x201A,  32, MO_AltRight,
	/* '“' */ 0x201C,   3, MO_AltRight,
	/* '”' */ 0x201D,   3, MO_ShiftLeft|MO_AltRight,
	/* '„' */ 0x201E,  56, MO_AltRight,
	/* '†' */ 0x2020,  21, MO_AltRight,
	/* '‡' */ 0x2021,  46, MO_ShiftLeft|MO_AltRight,
	/* '…' */ 0x2026,  54, MO_AltRight,
	/* '‰' */ 0x2030,  19, MO_ShiftLeft|MO_AltRight,
	/* '€' */ 0x20AC,  33, MO_ShiftLeft|MO_AltRight,
	/* ... */ 0x2122,  19, MO_AltRight,						// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=125
const CKeyFixup APPLE_USB_Italian[108] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '!' */ 0x0021,  55, MO_ShiftLeft,
	/* '"' */ 0x0022,   3, 0,
	/* '#' */ 0x0023,  56, MO_ShiftLeft,
	/* '$' */ 0x0024,  28, 0,
	/* '%' */ 0x0025,  41, MO_ShiftLeft,
	/* '&' */ 0x0026,   2, 0,
	/* ''' */ 0x0027,   4, 0,
	/* '(' */ 0x0028,   5, 0,
	/* ')' */ 0x0029,   8, 0,
	/* '+' */ 0x002B,  13, MO_ShiftLeft,
	/* ',' */ 0x002C,  52, 0,
	/* '-' */ 0x002D,  12, 0,
	/* '.' */ 0x002E,  53, MO_ShiftLeft,
	/* '/' */ 0x002F,  54, MO_ShiftLeft,
	/* '0' */ 0x0030,  11, MO_ShiftLeft,
	/* '1' */ 0x0031,   2, MO_ShiftLeft,
	/* '2' */ 0x0032,   3, MO_ShiftLeft,
	/* '3' */ 0x0033,   4, MO_ShiftLeft,
	/* '4' */ 0x0034,   5, MO_ShiftLeft,
	/* '5' */ 0x0035,   6, MO_ShiftLeft,
	/* '6' */ 0x0036,   7, MO_ShiftLeft,
	/* '7' */ 0x0037,   8, MO_ShiftLeft,
	/* '8' */ 0x0038,   9, MO_ShiftLeft,
	/* '9' */ 0x0039,  10, MO_ShiftLeft,
	/* ':' */ 0x003A,  54, 0,
	/* ';' */ 0x003B,  53, 0,
	/* '=' */ 0x003D,  13, 0,
	/* '?' */ 0x003F,  52, MO_ShiftLeft,
	/* '@' */ 0x0040,  56, 0,
	/* 'M' */ 0x004D,  40, MO_ShiftLeft,
	/* 'W' */ 0x0057,  46, MO_ShiftLeft,
	/* 'Z' */ 0x005A,  18, MO_ShiftLeft,
	/* '[' */ 0x005B,   5, MO_ShiftLeft|MO_AltRight,
	/* '\' */ 0x005C,  54, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x005D,   8, MO_ShiftLeft|MO_AltRight,
	/* '^' */ 0x005E,  27, MO_ShiftLeft,
	/* '_' */ 0x005F,  12, MO_ShiftLeft,
	/* '`' */ 0x0060,  10, MO_AltRight,
	/* 'm' */ 0x006D,  40, 0,
	/* 'w' */ 0x0077,  46, 0,
	/* 'z' */ 0x007A,  18, 0,
	/* '{' */ 0x007B,   5, MO_AltRight,
	/* '|' */ 0x007C,  54, MO_AltRight,
	/* '}' */ 0x007D,   8, MO_AltRight,
	/* '~' */ 0x007E,  51, MO_AltRight,
	/* ... */ 0x007F,  15, 0,									// <control> (DELETE)
	/* '¡' */ 0x00A1,  55, MO_AltRight,
	/* '¢' */ 0x00A2,  28, MO_AltRight,
	/* '£' */ 0x00A3,   9, 0,
	/* '¥' */ 0x00A5,  22, MO_AltRight,
	/* '§' */ 0x00A7,  42, 0,
	/* '¨' */ 0x00A8,  23, MO_AltRight,
	/* '©' */ 0x00A9,  48, MO_AltRight,
	/* '¬' */ 0x00AC,  39, MO_AltRight,
	/* '®' */ 0x00AE,  20, MO_AltRight,
	/* '°' */ 0x00B0,  42, MO_ShiftLeft,
	/* '±' */ 0x00B1,  13, MO_ShiftLeft|MO_AltRight,
	/* '´' */ 0x00B4,   2, MO_ShiftLeft|MO_AltRight,
	/* 'µ' */ 0x00B5,  40, MO_AltRight,
	/* '¶' */ 0x00B6,   7, MO_AltRight,
	/* '·' */ 0x00B7,   7, MO_ShiftLeft|MO_AltRight,
	/* '¿' */ 0x00BF,  52, MO_AltRight,
	/* 'Á' */ 0x00C1,  17, MO_ShiftLeft|MO_AltRight,
	/* 'Â' */ 0x00C2,  48, MO_ShiftLeft|MO_AltRight,
	/* 'Å' */ 0x00C5,  31, MO_ShiftLeft|MO_AltRight,
	/* 'Æ' */ 0x00C6,  41, MO_ShiftLeft|MO_AltRight,
	/* 'Ç' */ 0x00C7,   6, MO_AltRight,
	/* 'È' */ 0x00C8,  19, MO_ShiftLeft|MO_AltRight,
	/* 'Ê' */ 0x00CA,  50, MO_ShiftLeft|MO_AltRight,
	/* 'Ë' */ 0x00CB,  18, MO_ShiftLeft|MO_AltRight,
	/* 'Ì' */ 0x00CC,   6, MO_ShiftLeft|MO_AltRight,
	/* 'Í' */ 0x00CD,  20, MO_ShiftLeft|MO_AltRight,
	/* 'Î' */ 0x00CE,  22, MO_ShiftLeft|MO_AltRight,
	/* 'Ï' */ 0x00CF,  21, MO_ShiftLeft|MO_AltRight,
	/* 'Ò' */ 0x00D2,  11, MO_ShiftLeft|MO_AltRight,
	/* 'Ô' */ 0x00D4,  10, MO_ShiftLeft|MO_AltRight,
	/* 'Ø' */ 0x00D8,  25, MO_ShiftLeft|MO_AltRight,
	/* 'Ù' */ 0x00D9,  24, MO_ShiftLeft|MO_AltRight,
	/* 'Ú' */ 0x00DA,  28, MO_ShiftLeft|MO_AltRight,
	/* 'Û' */ 0x00DB,  23, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0x00DF,  32, MO_AltRight,
	/* 'à' */ 0x00E0,  10, 0,
	/* 'å' */ 0x00E5,  31, MO_AltRight,
	/* 'æ' */ 0x00E6,  41, MO_AltRight,
	/* 'ç' */ 0x00E7,   6, 0,
	/* 'è' */ 0x00E8,   7, 0,
	/* 'é' */ 0x00E9,  11, 0,
	/* 'ì' */ 0x00EC,  27, 0,
	/* 'ò' */ 0x00F2,  55, 0,
	/* '÷' */ 0x00F7,  53, MO_ShiftLeft|MO_AltRight,
	/* 'ø' */ 0x00F8,  25, MO_AltRight,
	/* 'ù' */ 0x00F9,  41, 0,
	/* 'Œ' */ 0x0152,  27, MO_ShiftLeft|MO_AltRight,
	/* 'œ' */ 0x0153,  27, MO_AltRight,
	/* 'Ÿ' */ 0x0178,  35, MO_AltRight,
	/* 'ƒ' */ 0x0192,  34, MO_AltRight,
	/* '‘' */ 0x2018,   4, MO_AltRight,
	/* '’' */ 0x2019,   4, MO_ShiftLeft|MO_AltRight,
	/* '“' */ 0x201C,   3, MO_AltRight,
	/* '”' */ 0x201D,   3, MO_ShiftLeft|MO_AltRight,
	/* '„' */ 0x201E,  46, MO_ShiftLeft|MO_AltRight,
	/* '†' */ 0x2020,  21, MO_AltRight,
	/* '‡' */ 0x2021,  34, MO_ShiftLeft|MO_AltRight,
	/* '…' */ 0x2026,  53, MO_AltRight,
	/* '‰' */ 0x2030,  47, MO_ShiftLeft|MO_AltRight,
	/* '€' */ 0x20AC,  24, MO_AltRight,
	/* ... */ 0x2122,  19, MO_AltRight,						// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=125
const CKeyFixup APPLE_USB_Swiss_German[64] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '@' */ 0x0040,  35, MO_AltRight,
	/* '[' */ 0x005B,   6, MO_AltRight,
	/* '\' */ 0x005C,   8, MO_ShiftLeft|MO_AltRight,
	/* ']' */ 0x005D,   7, MO_AltRight,
	/* '^' */ 0x005E,  13, 0,
	/* '`' */ 0x0060,  13, MO_ShiftLeft,
	/* '{' */ 0x007B,   9, MO_AltRight,
	/* '}' */ 0x007D,  10, MO_AltRight,
	/* '~' */ 0x007E,  51, MO_AltRight,
	/* ... */ 0x007F,  15, 0,									// <control> (DELETE)
	/* '¡' */ 0x00A1,  24, MO_AltRight,
	/* '¢' */ 0x00A2,  40, MO_AltRight,
	/* '£' */ 0x00A3,  42, MO_ShiftLeft,
	/* '¥' */ 0x00A5,  46, MO_AltRight,
	/* '¦' */ 0x00A6, NOS, 0,
	/* '§' */ 0x00A7,  56, 0,
	/* '¨' */ 0x00A8,  28, 0,
	/* 'ª' */ 0x00AA,  36, MO_AltRight,
	/* '«' */ 0x00AB,  53, MO_AltRight,
	/* '¬' */ 0x00AC,  39, MO_AltRight,
	/* '®' */ 0x00AE,  20, MO_AltRight,
	/* '°' */ 0x00B0,  56, MO_ShiftLeft,
	/* '±' */ 0x00B1,   2, MO_AltRight,
	/* '´' */ 0x00B4,  13, MO_AltRight,
	/* 'µ' */ 0x00B5,  52, MO_AltRight,
	/* '¶' */ 0x00B6,  42, MO_AltRight,
	/* 'º' */ 0x00BA,  37, MO_AltRight,
	/* '»' */ 0x00BB,  53, MO_ShiftLeft|MO_AltRight,
	/* '¿' */ 0x00BF,  12, MO_AltRight,
	/* 'Á' */ 0x00C1,  18, MO_ShiftLeft|MO_AltRight,
	/* 'Å' */ 0x00C5,  31, MO_ShiftLeft|MO_AltRight,
	/* 'Æ' */ 0x00C6,  41, MO_ShiftLeft|MO_AltRight,
	/* 'Ç' */ 0x00C7,   5, MO_AltRight,
	/* 'È' */ 0x00C8,  20, MO_ShiftLeft|MO_AltRight,
	/* 'Ë' */ 0x00CB,  19, MO_ShiftLeft|MO_AltRight,
	/* 'Í' */ 0x00CD,  22, MO_ShiftLeft|MO_AltRight,
	/* 'Î' */ 0x00CE,  21, MO_ShiftLeft|MO_AltRight,
	/* 'Ò' */ 0x00D2,   9, MO_ShiftLeft|MO_AltRight,
	/* 'Ô' */ 0x00D4,  10, MO_ShiftLeft|MO_AltRight,
	/* 'Ø' */ 0x00D8,  25, MO_ShiftLeft|MO_AltRight,
	/* 'Ú' */ 0x00DA,  11, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0x00DF,  32, MO_AltRight,
	/* 'å' */ 0x00E5,  31, MO_AltRight,
	/* 'æ' */ 0x00E6,  41, MO_AltRight,
	/* 'ç' */ 0x00E7,   5, MO_ShiftLeft,
	/* '÷' */ 0x00F7,  54, MO_ShiftLeft|MO_AltRight,
	/* 'ø' */ 0x00F8,  25, MO_AltRight,
	/* 'ÿ' */ 0x00FF,  27, MO_ShiftLeft|MO_AltRight,
	/* 'Œ' */ 0x0152,  17, MO_ShiftLeft|MO_AltRight,
	/* 'œ' */ 0x0153,  17, MO_AltRight,
	/* 'Ÿ' */ 0x0178,  46, MO_ShiftLeft|MO_AltRight,
	/* 'ƒ' */ 0x0192,  34, MO_AltRight,
	/* '‘' */ 0x2018,  28, MO_AltRight,
	/* '’' */ 0x2019,  28, MO_ShiftLeft|MO_AltRight,
	/* '“' */ 0x201C,   3, MO_AltRight,
	/* '”' */ 0x201D,   3, MO_ShiftLeft|MO_AltRight,
	/* '†' */ 0x2020,  21, MO_AltRight,
	/* '‡' */ 0x2021,  34, MO_ShiftLeft|MO_AltRight,
	/* '•' */ 0x2022,  42, MO_ShiftLeft|MO_AltRight,
	/* '…' */ 0x2026,  54, MO_AltRight,
	/* '‰' */ 0x2030,  56, MO_ShiftLeft|MO_AltRight,
	/* '€' */ 0x20AC,  47, MO_ShiftLeft|MO_AltRight,
	/* ... */ 0x2122,  19, MO_AltRight,						// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=125
const CKeyFixup APPLE_USB_British[23] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,  41, MO_ShiftLeft,
	/* '#' */ 0x0023,   4, MO_AltRight,
	/* '@' */ 0x0040,   3, MO_ShiftLeft,
	/* '\' */ 0x005C,  42, 0,
	/* '`' */ 0x0060,  45, 0,
	/* '|' */ 0x007C,  42, MO_ShiftLeft,
	/* '~' */ 0x007E,  45, MO_ShiftLeft,
	/* ... */ 0x007F,  15, 0,									// <control> (DELETE)
	/* '£' */ 0x00A3,   4, MO_ShiftLeft,
	/* '¥' */ 0x00A5,  22, MO_AltRight,
	/* '§' */ 0x00A7,  56, 0,
	/* '±' */ 0x00B1,  56, MO_ShiftLeft,
	/* 'µ' */ 0x00B5,  52, MO_AltRight,
	/* '¶' */ 0x00B6,   8, MO_AltRight,
	/* 'Å' */ 0x00C5,  31, MO_ShiftLeft|MO_AltRight,
	/* 'Ç' */ 0x00C7,  48, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0x00DF,  32, MO_AltRight,
	/* 'å' */ 0x00E5,  31, MO_AltRight,
	/* 'ç' */ 0x00E7,  48, MO_AltRight,
	/* 'ƒ' */ 0x0192,  34, MO_AltRight,
	/* '€' */ 0x20AC,   3, MO_ShiftLeft|MO_AltRight,
	/* ... */ 0x2122,   3, MO_AltRight,						// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=125
const CKeyFixup APPLE_USB_Danish[37] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '$' */ 0x0024,  56, 0,
	/* '@' */ 0x0040,  42, MO_AltRight,
	/* '\' */ 0x005C,   8, MO_AltRight,
	/* '^' */ 0x005E,  28, MO_ShiftLeft,
	/* '`' */ 0x0060,  13, MO_ShiftLeft,
	/* '{' */ 0x007B,   9, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x007C,  24, MO_AltRight,
	/* '}' */ 0x007D,  10, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x007E,  28, MO_AltRight,
	/* ... */ 0x007F,  15, 0,									// <control> (DELETE)
	/* '£' */ 0x00A3,   5, MO_AltRight,
	/* '¤' */ 0x00A4, NOS, 0,
	/* '¥' */ 0x00A5, NOS, 0,
	/* '§' */ 0x00A7,  56, MO_ShiftLeft,
	/* '¨' */ 0x00A8,  28, 0,
	/* '±' */ 0x00B1, NOS, 0,
	/* '´' */ 0x00B4,  13, 0,
	/* 'µ' */ 0x00B5, NOS, 0,
	/* '¶' */ 0x00B6, NOS, 0,
	/* '½' */ 0x00BD, NOS, 0,
	/* 'Ä' */ 0x00C4,  40, MO_ShiftLeft|MO_AltRight,
	/* 'Å' */ 0x00C5,  27, MO_ShiftLeft,
	/* 'Ç' */ 0x00C7, NOS, 0,
	/* 'É' */ 0x00C9,  19, MO_ShiftLeft|MO_AltRight,
	/* 'Ö' */ 0x00D6,  41, MO_ShiftLeft|MO_AltRight,
	/* 'Ü' */ 0x00DC,  23, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0x00DF, NOS, 0,
	/* 'ä' */ 0x00E4,  40, MO_AltRight,
	/* 'å' */ 0x00E5,  27, 0,
	/* 'ç' */ 0x00E7, NOS, 0,
	/* 'é' */ 0x00E9,  19, MO_AltRight,
	/* 'ö' */ 0x00F6,  41, MO_AltRight,
	/* 'ü' */ 0x00FC,  23, MO_AltRight,
	/* '’' */ 0x2019, NOS, 0,
	/* '€' */ 0x20AC,   5, MO_ShiftLeft,
	/* ... */ 0x2122, NOS, 0,									// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=125
const CKeyFixup APPLE_USB_Norwegian[35] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '$' */ 0x0024,   5, MO_ShiftLeft,
	/* ''' */ 0x0027,  56, 0,
	/* '@' */ 0x0040,  42, 0,
	/* '\' */ 0x005C,   8, MO_ShiftLeft|MO_AltRight,
	/* '^' */ 0x005E,  28, MO_ShiftLeft,
	/* '`' */ 0x0060,  13, MO_ShiftLeft,
	/* '{' */ 0x007B,   9, MO_ShiftLeft|MO_AltRight,
	/* '|' */ 0x007C,   8, MO_AltRight,
	/* '}' */ 0x007D,  10, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x007E,  28, MO_AltRight,
	/* ... */ 0x007F,  15, 0,									// <control> (DELETE)
	/* '£' */ 0x00A3,   4, MO_AltRight,
	/* '¤' */ 0x00A4, NOS, 0,
	/* '¥' */ 0x00A5, NOS, 0,
	/* '§' */ 0x00A7,  56, MO_ShiftLeft,
	/* '¨' */ 0x00A8,  28, 0,
	/* '±' */ 0x00B1, NOS, 0,
	/* '´' */ 0x00B4,  13, 0,
	/* 'µ' */ 0x00B5, NOS, 0,
	/* '¶' */ 0x00B6, NOS, 0,
	/* 'Ä' */ 0x00C4,  41, MO_ShiftLeft|MO_AltRight,
	/* 'Å' */ 0x00C5,  27, MO_ShiftLeft,
	/* 'Ç' */ 0x00C7, NOS, 0,
	/* 'Ö' */ 0x00D6,  40, MO_ShiftLeft|MO_AltRight,
	/* 'Ü' */ 0x00DC,  23, MO_ShiftLeft|MO_AltRight,
	/* 'ß' */ 0x00DF, NOS, 0,
	/* 'ä' */ 0x00E4,  41, MO_AltRight,
	/* 'å' */ 0x00E5,  27, 0,
	/* 'ç' */ 0x00E7, NOS, 0,
	/* 'ö' */ 0x00F6,  40, MO_AltRight,
	/* 'ü' */ 0x00FC,  23, MO_AltRight,
	/* '’' */ 0x2019, NOS, 0,
	/* '€' */ 0x20AC,  56, MO_AltRight,
	/* ... */ 0x2122, NOS, 0,									// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=125
const CKeyFixup APPLE_USB_Spanish[27] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '\' */ 0x005C,  56, MO_AltRight,
	/* '^' */ 0x005E,  27, MO_ShiftLeft,
	/* '`' */ 0x0060,  27, 0,
	/* '~' */ 0x007E,  40, MO_AltRight,
	/* ... */ 0x007F,  15, 0,									// <control> (DELETE)
	/* '¢' */ 0x00A2,   5, MO_AltRight,
	/* '£' */ 0x00A3,   5, MO_ShiftLeft|MO_AltRight,
	/* '¥' */ 0x00A5, NOS, 0,
	/* '§' */ 0x00A7,  38, MO_AltRight,
	/* '¨' */ 0x00A8,  41, MO_ShiftLeft,
	/* '©' */ 0x00A9,  48, MO_AltRight,
	/* 'ª' */ 0x00AA,  56, MO_ShiftLeft,
	/* '°' */ 0x00B0,  56, 0,
	/* '±' */ 0x00B1, NOS, 0,
	/* '´' */ 0x00B4,  41, 0,
	/* 'µ' */ 0x00B5, NOS, 0,
	/* '¶' */ 0x00B6, NOS, 0,
	/* 'º' */ 0x00BA, NOS, 0,
	/* 'Å' */ 0x00C5, NOS, 0,
	/* 'Ç' */ 0x00C7,  42, MO_ShiftLeft,
	/* 'ß' */ 0x00DF, NOS, 0,
	/* 'å' */ 0x00E5, NOS, 0,
	/* 'ç' */ 0x00E7,  42, 0,
	/* '’' */ 0x2019, NOS, 0,
	/* '€' */ 0x20AC,  48, MO_ShiftLeft|MO_AltRight,
	/* ... */ 0x2122, NOS, 0,									// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=125
const CKeyFixup APPLE_USB_Dutch[29] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '-' */ 0x002D, NOS, 0,
	/* '@' */ 0x0040,  56, 0,
	/* '^' */ 0x005E,  27, MO_ShiftLeft,
	/* '`' */ 0x0060,  41, MO_ShiftLeft,
	/* '~' */ 0x007E,  13, MO_ShiftLeft,
	/* ... */ 0x007F,  15, 0,									// <control> (DELETE)
	/* '£' */ 0x00A3,   8, MO_AltRight,
	/* '¥' */ 0x00A5, NOS, 0,
	/* '¦' */ 0x00A6, NOS, 0,
	/* '§' */ 0x00A7,  56, MO_ShiftLeft,
	/* '¨' */ 0x00A8,  27, 0,
	/* '¬' */ 0x00AC,  56, MO_AltRight,
	/* '°' */ 0x00B0, NOS, 0,
	/* '±' */ 0x00B1,  40, MO_ShiftLeft,
	/* '´' */ 0x00B4,  41, 0,
	/* 'µ' */ 0x00B5,  52, MO_AltRight,
	/* '¶' */ 0x00B6,  20, MO_AltRight,
	/* '·' */ 0x00B7, NOS, 0,
	/* '¸' */ 0x00B8, NOS, 0,
	/* 'Å' */ 0x00C5, NOS, 0,
	/* 'Ç' */ 0x00C7, NOS, 0,
	/* 'ß' */ 0x00DF,  32, MO_AltRight,
	/* 'å' */ 0x00E5, NOS, 0,
	/* 'ç' */ 0x00E7, NOS, 0,
	/* '’' */ 0x2019, NOS, 0,
	/* '•' */ 0x2022,  13, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* ... */ 0x2122, NOS, 0,									// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=125
const CKeyFixup APPLE_USB_Hebrew[19] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* ' ' */ 0x0020,   1, 0,
	/* '`' */ 0x0060, NOS, 0,
	/* '~' */ 0x007E, NOS, 0,
	/* ... */ 0x007F,  15, 0,									// <control> (DELETE)
	/* '£' */ 0x00A3, NOS, 0,
	/* '¥' */ 0x00A5, NOS, 0,
	/* '§' */ 0x00A7, NOS, 0,
	/* '±' */ 0x00B1, NOS, 0,
	/* 'µ' */ 0x00B5, NOS, 0,
	/* '¶' */ 0x00B6, NOS, 0,
	/* 'Å' */ 0x00C5, NOS, 0,
	/* 'Ç' */ 0x00C7, NOS, 0,
	/* 'ß' */ 0x00DF, NOS, 0,
	/* 'å' */ 0x00E5, NOS, 0,
	/* 'ç' */ 0x00E7, NOS, 0,
	/* 'ƒ' */ 0x0192, NOS, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* ... */ 0x2122, NOS, 0,									// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=125
const CKeyFixup APPLE_USB_Portuguese[25] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '\' */ 0x005C,  56, 0,
	/* '^' */ 0x005E,  42, MO_ShiftLeft,
	/* '`' */ 0x0060,  28, MO_ShiftLeft,
	/* '|' */ 0x007C,  56, MO_ShiftLeft,
	/* '~' */ 0x007E,  42, 0,
	/* ... */ 0x007F,  15, 0,									// <control> (DELETE)
	/* '£' */ 0x00A3,   4, MO_AltRight,
	/* '¥' */ 0x00A5, NOS, 0,
	/* '§' */ 0x00A7,   5, MO_AltRight,
	/* '¨' */ 0x00A8,  27, MO_AltRight,
	/* '°' */ 0x00B0,  41, 0,
	/* '±' */ 0x00B1, NOS, 0,
	/* '´' */ 0x00B4,  28, 0,
	/* 'µ' */ 0x00B5, NOS, 0,
	/* '¶' */ 0x00B6, NOS, 0,
	/* 'º' */ 0x00BA, NOS, 0,
	/* 'Å' */ 0x00C5, NOS, 0,
	/* 'Ç' */ 0x00C7,  40, MO_ShiftLeft,
	/* 'ß' */ 0x00DF, NOS, 0,
	/* 'å' */ 0x00E5, NOS, 0,
	/* 'ç' */ 0x00E7,  40, 0,
	/* 'ƒ' */ 0x0192, NOS, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* ... */ 0x2122, NOS, 0,									// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=125
const CKeyFixup APPLE_USB_Latin_America[23] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '^' */ 0x005E,  41, MO_AltRight,
	/* '`' */ 0x0060,  27, 0,
	/* '|' */ 0x007C,  56, 0,
	/* ... */ 0x007F,  15, 0,									// <control> (DELETE)
	/* '£' */ 0x00A3, NOS, 0,
	/* '¥' */ 0x00A5, NOS, 0,
	/* '§' */ 0x00A7, NOS, 0,
	/* '¨' */ 0x00A8,  27, MO_ShiftLeft,
	/* '¬' */ 0x00AC,  56, MO_AltRight,
	/* '°' */ 0x00B0,  56, MO_ShiftLeft,
	/* '±' */ 0x00B1, NOS, 0,
	/* '´' */ 0x00B4, NOS, 0,
	/* 'µ' */ 0x00B5, NOS, 0,
	/* '¶' */ 0x00B6, NOS, 0,
	/* 'Å' */ 0x00C5, NOS, 0,
	/* 'Ç' */ 0x00C7, NOS, 0,
	/* 'ß' */ 0x00DF, NOS, 0,
	/* 'å' */ 0x00E5, NOS, 0,
	/* 'ç' */ 0x00E7, NOS, 0,
	/* 'ƒ' */ 0x0192, NOS, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* ... */ 0x2122, NOS, 0,									// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=125
const CKeyFixup APPLE_USB_Czech_DEC[19] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* ''' */ 0x0027,  41, MO_AltRight,
	/* '~' */ 0x007E,  56, MO_ShiftLeft|MO_AltRight,
	/* ... */ 0x007F,  15, 0,									// <control> (DELETE)
	/* '£' */ 0x00A3, NOS, 0,
	/* '¥' */ 0x00A5, NOS, 0,
	/* '§' */ 0x00A7,  41, 0,
	/* '±' */ 0x00B1, NOS, 0,
	/* 'µ' */ 0x00B5, NOS, 0,
	/* '¶' */ 0x00B6, NOS, 0,
	/* 'Å' */ 0x00C5, NOS, 0,
	/* 'Ç' */ 0x00C7, NOS, 0,
	/* 'ß' */ 0x00DF,  56, MO_ShiftLeft,
	/* 'ä' */ 0x00E4,  56, 0,
	/* 'å' */ 0x00E5, NOS, 0,
	/* 'ç' */ 0x00E7, NOS, 0,
	/* '’' */ 0x2019, NOS, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* ... */ 0x2122, NOS, 0,									// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=125
const CKeyFixup APPLE_USB_Canadian_French[32] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '#' */ 0x0023,  56, 0,
	/* ''' */ 0x0027,  28, 0,
	/* '\' */ 0x005C,  56, MO_AltRight,
	/* '^' */ 0x005E,  27, 0,
	/* '`' */ 0x0060,  41, MO_ShiftLeft,
	/* '|' */ 0x007C,   8, MO_AltRight,
	/* ... */ 0x007F,  15, 0,									// <control> (DELETE)
	/* '£' */ 0x00A3,   4, MO_AltRight,
	/* '¥' */ 0x00A5, NOS, 0,
	/* '¦' */ 0x00A6, NOS, 0,
	/* '§' */ 0x00A7,  25, MO_AltRight,
	/* '¨' */ 0x00A8,  28, MO_ShiftLeft,
	/* 'ª' */ 0x00AA,   9, MO_AltRight,
	/* '­' */ 0x00AD, NOS, 0,
	/* '¯' */ 0x00AF, NOS, 0,
	/* '°' */ 0x00B0, NOS, 0,
	/* '±' */ 0x00B1,   2, MO_AltRight,
	/* '²' */ 0x00B2, NOS, 0,
	/* '´' */ 0x00B4,  55, MO_AltRight,
	/* 'µ' */ 0x00B5,  52, MO_AltRight,
	/* '¶' */ 0x00B6,  26, MO_AltRight,
	/* '¸' */ 0x00B8, NOS, 0,
	/* 'Å' */ 0x00C5, NOS, 0,
	/* 'Ç' */ 0x00C7, NOS, 0,
	/* 'ß' */ 0x00DF, NOS, 0,
	/* 'å' */ 0x00E5, NOS, 0,
	/* 'ç' */ 0x00E7, NOS, 0,
	/* '’' */ 0x2019, NOS, 0,
	/* '•' */ 0x2022,  45, MO_AltRight,
	/* '€' */ 0x20AC, NOS, 0,
	/* ... */ 0x2122, NOS, 0,									// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=125
const CKeyFixup APPLE_USB_Hungarian[33] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '0' */ 0x0030,  56, 0,
	/* '^' */ 0x005E,   4, MO_AltRight,
	/* ... */ 0x007F,  15, 0,									// <control> (DELETE)
	/* '£' */ 0x00A3, NOS, 0,
	/* '¥' */ 0x00A5, NOS, 0,
	/* '§' */ 0x00A7,  56, MO_ShiftLeft,
	/* '¨' */ 0x00A8,  12, MO_AltRight,
	/* '°' */ 0x00B0,   6, MO_AltRight,
	/* '±' */ 0x00B1, NOS, 0,
	/* '´' */ 0x00B4,  10, MO_AltRight,
	/* 'µ' */ 0x00B5, NOS, 0,
	/* '¶' */ 0x00B6, NOS, 0,
	/* '·' */ 0x00B7,   9, MO_AltRight,
	/* '¸' */ 0x00B8,   7, MO_AltRight,
	/* 'Ä' */ 0x00C4, NOS, 0,
	/* 'Å' */ 0x00C5, NOS, 0,
	/* 'Ç' */ 0x00C7, NOS, 0,
	/* 'Í' */ 0x00CD,  45, MO_ShiftLeft,
	/* 'ß' */ 0x00DF,  41, MO_AltRight,
	/* 'ä' */ 0x00E4, NOS, 0,
	/* 'å' */ 0x00E5, NOS, 0,
	/* 'ç' */ 0x00E7, NOS, 0,
	/* 'í' */ 0x00ED,  45, 0,
	/* '’' */ 0x2019, NOS, 0,
	/* ??? */ 0x02C7,   3, MO_AltRight,						// CARON
	/* ??? */ 0x02D8,   5, MO_AltRight,						// BREVE
	/* 'Ù' */ 0x00D9, NOS, 0,
	/* 'Û' */ 0x00DB, NOS, 0,
	/* ??? */ 0x02DD,  11, MO_AltRight,						// DOUBLE ACUTE ACCENT
	/* '‚' */ 0x201A,  13, MO_AltRight,
	/* '€' */ 0x20AC, NOS, 0,
	/* ... */ 0x2122, NOS, 0,									// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=125
const CKeyFixup APPLE_USB_Polish[19] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '`' */ 0x0060,  56, 0,
	/* '~' */ 0x007E,  56, MO_ShiftLeft,
	/* ... */ 0x007F,  15, 0,									// <control> (DELETE)
	/* '£' */ 0x00A3, NOS, 0,
	/* '¥' */ 0x00A5, NOS, 0,
	/* '§' */ 0x00A7,  28, MO_ShiftLeft,
	/* '±' */ 0x00B1, NOS, 0,
	/* 'µ' */ 0x00B5, NOS, 0,
	/* '¶' */ 0x00B6, NOS, 0,
	/* '·' */ 0x00B7,  56, MO_AltRight,
	/* 'Å' */ 0x00C5, NOS, 0,
	/* 'Ç' */ 0x00C7, NOS, 0,
	/* 'ß' */ 0x00DF, NOS, 0,
	/* 'å' */ 0x00E5, NOS, 0,
	/* 'ç' */ 0x00E7, NOS, 0,
	/* '’' */ 0x2019, NOS, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* ... */ 0x2122, NOS, 0,									// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=125
const CKeyFixup APPLE_USB_SCS[18] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* ''' */ 0x0027,   8, MO_ShiftLeft,
	/* '~' */ 0x007E,  56, MO_ShiftLeft,
	/* ... */ 0x007F,  15, 0,									// <control> (DELETE)
	/* '£' */ 0x00A3, NOS, 0,
	/* '¥' */ 0x00A5, NOS, 0,
	/* '§' */ 0x00A7, NOS, 0,
	/* '±' */ 0x00B1, NOS, 0,
	/* 'µ' */ 0x00B5, NOS, 0,
	/* '¶' */ 0x00B6, NOS, 0,
	/* 'Å' */ 0x00C5, NOS, 0,
	/* 'Ç' */ 0x00C7, NOS, 0,
	/* 'ß' */ 0x00DF, NOS, 0,
	/* 'å' */ 0x00E5, NOS, 0,
	/* 'ç' */ 0x00E7, NOS, 0,
	/* 'ƒ' */ 0x0192, NOS, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* ... */ 0x2122, NOS, 0,									// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=125
const CKeyFixup APPLE_USB_Slovakian_DEC[19] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '`' */ 0x0060,  45, MO_ShiftLeft|MO_AltRight,
	/* '~' */ 0x007E,  56, MO_ShiftLeft|MO_AltRight,
	/* ... */ 0x007F,  15, 0,									// <control> (DELETE)
	/* '£' */ 0x00A3, NOS, 0,
	/* '¥' */ 0x00A5, NOS, 0,
	/* '§' */ 0x00A7,  41, 0,
	/* '±' */ 0x00B1, NOS, 0,
	/* 'µ' */ 0x00B5, NOS, 0,
	/* '¶' */ 0x00B6, NOS, 0,
	/* 'Å' */ 0x00C5, NOS, 0,
	/* 'Ç' */ 0x00C7, NOS, 0,
	/* 'ß' */ 0x00DF,  56, MO_ShiftLeft,
	/* 'å' */ 0x00E5, NOS, 0,
	/* 'ç' */ 0x00E7, NOS, 0,
	/* 'o' */ 0x006F, NOS, 0,
	/* '’' */ 0x2019, NOS, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* ... */ 0x2122, NOS, 0,									// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// The next table is equal to another, so we save the memory
#define APPLE_USB_Swedish             		APPLE_USB_Finnish_Swedish

// TermId=125
const CKeyFixup APPLE_USB_Turkish_Q[18] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '"' */ 0x0022,  56, 0,
	/* ... */ 0x007F,  15, 0,									// <control> (DELETE)
	/* '£' */ 0x00A3, NOS, 0,
	/* '¥' */ 0x00A5, NOS, 0,
	/* '§' */ 0x00A7, NOS, 0,
	/* '±' */ 0x00B1, NOS, 0,
	/* 'µ' */ 0x00B5, NOS, 0,
	/* '¶' */ 0x00B6, NOS, 0,
	/* 'Å' */ 0x00C5, NOS, 0,
	/* 'Ç' */ 0x00C7,  54, MO_ShiftLeft,
	/* 'ß' */ 0x00DF,  32, MO_AltRight,
	/* 'å' */ 0x00E5, NOS, 0,
	/* 'ç' */ 0x00E7,  54, 0,
	/* 'é' */ 0x00E9,  56, MO_ShiftLeft,
	/* 'ƒ' */ 0x0192, NOS, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* ... */ 0x2122, NOS, 0,									// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=125
const CKeyFixup APPLE_USB_Romanian[24] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '#' */ 0x0023,  47, MO_AltRight,
	/* '+' */ 0x002B,   4, MO_ShiftLeft,
	/* '[' */ 0x005B,  34, MO_AltRight,
	/* ']' */ 0x005D,  35, MO_AltRight,
	/* ... */ 0x007F,  15, 0,									// <control> (DELETE)
	/* '£' */ 0x00A3, NOS, 0,
	/* '¥' */ 0x00A5, NOS, 0,
	/* '§' */ 0x00A7,  52, MO_AltRight,
	/* '±' */ 0x00B1, NOS, 0,
	/* 'µ' */ 0x00B5, NOS, 0,
	/* '¶' */ 0x00B6, NOS, 0,
	/* '¸' */ 0x00B8,   7, MO_AltRight,
	/* 'Å' */ 0x00C5, NOS, 0,
	/* 'Ç' */ 0x00C7, NOS, 0,
	/* 'ß' */ 0x00DF,  41, MO_AltRight,
	/* 'å' */ 0x00E5, NOS, 0,
	/* 'ç' */ 0x00E7, NOS, 0,
	/* '’' */ 0x2019, NOS, 0,
	/* 'Û' */ 0x00DB, NOS, 0,
	/* 'Ý' */ 0x00DD, NOS, 0,
	/* '‚' */ 0x201A,  13, MO_AltRight,
	/* '€' */ 0x20AC, NOS, 0,
	/* ... */ 0x2122, NOS, 0,									// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=125
const CKeyFixup APPLE_USB_Russian[19] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* ' ' */ 0x0020,   1, 0,
	/* ... */ 0x007F,  15, 0,									// <control> (DELETE)
	/* '£' */ 0x00A3, NOS, 0,
	/* '¥' */ 0x00A5, NOS, 0,
	/* '§' */ 0x00A7, NOS, 0,
	/* '±' */ 0x00B1, NOS, 0,
	/* 'µ' */ 0x00B5, NOS, 0,
	/* '¶' */ 0x00B6, NOS, 0,
	/* 'Å' */ 0x00C5, NOS, 0,
	/* 'Ç' */ 0x00C7, NOS, 0,
	/* 'ß' */ 0x00DF, NOS, 0,
	/* 'å' */ 0x00E5, NOS, 0,
	/* 'ç' */ 0x00E7, NOS, 0,
	/* '’' */ 0x2019, NOS, 0,
	/* ??? */ 0x0401,  56, MO_ShiftLeft,					// CYRILLIC CAPITAL LETTER IO
	/* ??? */ 0x0451,  56, 0,									// CYRILLIC SMALL LETTER IO
	/* '¬' */ 0x00AC, NOS, 0,
	/* ... */ 0x2122, NOS, 0,									// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};

// TermId=125
const CKeyFixup APPLE_USB_Turkish_F[19] =
{
	/*       Unicode,  KN, MODIFIERS */
	/* '*' */ 0x002A,  56, MO_ShiftLeft,
	/* '+' */ 0x002B,  56, 0,
	/* ... */ 0x007F,  15, 0,									// <control> (DELETE)
	/* '£' */ 0x00A3,  26, MO_AltRight,
	/* '¥' */ 0x00A5,  22, MO_AltRight,
	/* '§' */ 0x00A7, NOS, 0,
	/* '¬' */ 0x00AC,  56, MO_AltRight,
	/* '±' */ 0x00B1, NOS, 0,
	/* 'µ' */ 0x00B5,  52, MO_AltRight,
	/* '¶' */ 0x00B6,  20, MO_AltRight,
	/* 'Å' */ 0x00C5, NOS, 0,
	/* 'Ç' */ 0x00C7,  50, MO_ShiftLeft,
	/* 'ß' */ 0x00DF,  32, MO_AltRight,
	/* 'å' */ 0x00E5, NOS, 0,
	/* 'ç' */ 0x00E7,  50, 0,
	/* 'ƒ' */ 0x0192, NOS, 0,
	/* '€' */ 0x20AC, NOS, 0,
	/* ... */ 0x2122, NOS, 0,									// TRADE MARK SIGN
	/* end */ 0,KN_ENDMARKER,0
};


/******************************************************/
/* Pointer tables                                     */
/******************************************************/

// Forward declarations for pointer tables
extern const CKeyFixups IBM_XT_FixupPtr;
extern const CKeyFixups IBM_AT_FixupPtr;
extern const CKeyFixups DEC_VT510_PC_FixupPtr;
extern const CKeyFixups IBM_3486_102_FixupPtr;
extern const CKeyFixups IBM_3191_122_FixupPtr;
extern const CKeyFixups IBM_3486_122_FixupPtr;
extern const CKeyFixups Telex_88_FixupPtr;
extern const CKeyFixups BDS_7_FixupPtr;
extern const CKeyFixups DEC_VT510_LK411_FixupPtr;
extern const CKeyFixups Telex_88_DATA_ENTRY_FixupPtr;
extern const CKeyFixups APPLE_USB_FixupPtr;

const CKeyFixup *IBM_XT_Fixups[11] =
{
	NoExceptions,                         		// 00 USA not required
	IBM_XT_Belgian,                       		// 01
	IBM_XT_Finnish_Swedish,               		// 02
	IBM_XT_French,                        		// 03
	IBM_XT_German,                        		// 04
	IBM_XT_Italian,                       		// 05
	IBM_XT_Swiss_German,                  		// 06
	IBM_XT_British,                       		// 07
	IBM_XT_Danish,                        		// 08
	IBM_XT_Norwegian,                     		// 09
	IBM_XT_Spanish,                       		// 10
};


// TermId=  1
const CKeyFixups IBM_XT_FixupPtr =
{
	&IBM_AT_FixupPtr,                     		// pointer to parent table
	IBM_XT_Fixups,                        		// pointer fixup table
	00, 10,                               		// min = USA, max = Spanish
};

const CKeyFixup *IBM_AT_Fixups[87] =
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
	NoExceptions,                         		// 09 Norwegian not required
	NoExceptions,                         		// 10 Spanish not required
	NoExceptions,                         		// 11 Dutch not required
	NoExceptions,                         		// 12 Hebrew not required
	NoExceptions,                         		// 13 Portuguese not required
	NoExceptions,                         		// 14 Latin America not required
	NoExceptions,                         		// 15 Czech DEC not required
	NoExceptions,                         		// 16 Brazilian not required
	NoExceptions,                         		// 17 Greek DEC not required
	NoExceptions,                         		// 18 Canadian French not required
	NoExceptions,                         		// 19 Hungarian not required
	NoExceptions,                         		// 20 Polish not required
	NoExceptions,                         		// 21 SCS not required
	NoExceptions,                         		// 22 Slovakian DEC not required
	NoExceptions,                         		// 23 Swedish not required
	IBM_AT_Turkish_Q,                     		// 24
	NoExceptions,                         		// 25 Romanian not required
	NoExceptions,                         		// 26 Russian not required
	IBM_AT_Turkish_F,                     		// 27
	NoExceptions,                         		// 28 Japanese ASCII not required
	NoExceptions,                         		// 29 Swiss (French) not required
	NoExceptions,                         		// 30 USA International not required
	NoExceptions,                         		// 31 Slovenian not required
	NoExceptions,                         		// 32 Croatian not required
	NoExceptions,                         		// 33 Bosnian not required
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
	NoExceptions,                         		// 59 Brazilian MS not required
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
	NoExceptions,                         		// 74 Maltese not required
	NoExceptions,                         		// 75 Icelandic not required
	NoExceptions,                         		// 76 Ukrainian not required
	NoExceptions,                         		// 77 Uzbek Cyrillic not required
	NoExceptions,                         		// 78 Kazakh not required
	NoExceptions,                         		// 79 Kyrgyz Cyrillic not required
	NoExceptions,                         		// 80 Azeri Latin not required
	NoExceptions,                         		// 81 Azeri Cyrillic not required
	NoExceptions,                         		// 82 Belarusian not required
	NoExceptions,                         		// 83 Faeroese not required
	NoExceptions,                         		// 84 Gaelic not required
	NoExceptions,                         		// 85 Tatar not required
	NoExceptions,                         		// 86 Mongolian Cyrillic not required
};


// TermId=  3 	,  2 , 97 ,106 ,124
const CKeyFixups IBM_AT_FixupPtr =
{
	&DEC_VT510_PC_FixupPtr,               		// pointer to parent table
	IBM_AT_Fixups,                        		// pointer fixup table
	00, 86,                               		// min = USA, max = Mongolian Cyrillic
};

const CKeyFixup *DEC_VT510_PC_Fixups[87] =
{
	NoExceptions,                         		// 00 USA not required
	DEC_VT510_PC_Belgian,                 		// 01
	DEC_VT510_PC_Finnish_Swedish,         		// 02
	DEC_VT510_PC_French,                  		// 03
	DEC_VT510_PC_German,                  		// 04
	DEC_VT510_PC_Italian,                 		// 05
	DEC_VT510_PC_Swiss_German,            		// 06
	DEC_VT510_PC_British,                 		// 07
	DEC_VT510_PC_Danish,                  		// 08
	DEC_VT510_PC_Norwegian,               		// 09
	DEC_VT510_PC_Spanish,                 		// 10
	DEC_VT510_PC_Dutch,                   		// 11
	NoExceptions,                         		// 12 Hebrew not required
	DEC_VT510_PC_Portuguese,              		// 13
	DEC_VT510_PC_Latin_America,           		// 14
	DEC_VT510_PC_Czech_DEC,               		// 15
	DEC_VT510_PC_Brazilian,               		// 16
	DEC_VT510_PC_Greek_DEC,               		// 17
	DEC_VT510_PC_Canadian_French,         		// 18
	DEC_VT510_PC_Hungarian,               		// 19
	DEC_VT510_PC_Polish,                  		// 20
	DEC_VT510_PC_SCS,                     		// 21
	DEC_VT510_PC_Slovakian_DEC,           		// 22
	DEC_VT510_PC_Swedish,                 		// 23
	DEC_VT510_PC_Turkish_Q,               		// 24
	DEC_VT510_PC_Romanian,                		// 25
	DEC_VT510_PC_Russian,                 		// 26
	DEC_VT510_PC_Turkish_F,               		// 27
	DEC_VT510_PC_Japanese_ASCII,          		// 28
	DEC_VT510_PC_Swiss_French,            		// 29
	DEC_VT510_PC_USA_International,       		// 30
	DEC_VT510_PC_Slovenian,               		// 31
	DEC_VT510_PC_Croatian,                		// 32
	DEC_VT510_PC_Bosnian,                 		// 33
	DEC_VT510_PC_Macedonian,              		// 34
	DEC_VT510_PC_Albanian,                		// 35
	DEC_VT510_PC_Serbian_Latin,           		// 36
	DEC_VT510_PC_Serbian_Cyrillic,        		// 37
	DEC_VT510_PC_Czech_QWERTZ,            		// 38
	DEC_VT510_PC_Czech_QWERTY,            		// 39
	DEC_VT510_PC_Czech_Programmers,       		// 40
	DEC_VT510_PC_Estonian,                		// 41
	DEC_VT510_PC_Latvian,                 		// 42
	DEC_VT510_PC_Latvian_QWERTY,          		// 43
	DEC_VT510_PC_Lithuania,               		// 44
	DEC_VT510_PC_Lithuanian_IBM,          		// 45
	NULL,                                 		// 46 Reserved5 not supported
	NULL,                                 		// 47 Reserved6 not supported
	DEC_VT510_PC_Slovakian_QWERTZ,        		// 48
	DEC_VT510_PC_Slovakian_QWERTY,        		// 49
	DEC_VT510_PC_Hungarian_101_Key,       		// 50
	DEC_VT510_PC_Spanish_Variation,       		// 51
	DEC_VT510_PC_Bulgarian_Cyrillic,      		// 52
	DEC_VT510_PC_Bulgarian_Latin,         		// 53
	DEC_VT510_PC_Canadian_French_Legacy,  		// 54
	DEC_VT510_PC_Canadian_Multilingual,   		// 55
	DEC_VT510_PC_Italian_142,             		// 56
	DEC_VT510_PC_Polish_214,              		// 57
	DEC_VT510_PC_Polish_Programmers,      		// 58
	DEC_VT510_PC_Brazilian_MS,            		// 59
	DEC_VT510_PC_Greek_Polytonic,         		// 60
	DEC_VT510_PC_Greek_220,               		// 61
	DEC_VT510_PC_Greek_319,               		// 62
	DEC_VT510_PC_Greek_Latin,             		// 63
	DEC_VT510_PC_Greek_220_Latin,         		// 64
	DEC_VT510_PC_Greek_319_Latin,         		// 65
	DEC_VT510_PC_Greek_MS,                		// 66
	DEC_VT510_PC_Russian_MS,              		// 67
	DEC_VT510_PC_Russian_Typewriter,      		// 68
	NULL,                                 		// 69 Reserved1 not supported
	NULL,                                 		// 70 Reserved2 not supported
	NULL,                                 		// 71 Reserved3 not supported
	NULL,                                 		// 72 Reserved4 not supported
	DEC_VT510_PC_Irish,                   		// 73
	DEC_VT510_PC_Maltese,                 		// 74
	DEC_VT510_PC_Icelandic,               		// 75
	DEC_VT510_PC_Ukrainian,               		// 76
	DEC_VT510_PC_Uzbek_Cyrillic,          		// 77
	DEC_VT510_PC_Kazakh,                  		// 78
	DEC_VT510_PC_Kyrgyz_Cyrillic,         		// 79
	DEC_VT510_PC_Azeri_Latin,             		// 80
	DEC_VT510_PC_Azeri_Cyrillic,          		// 81
	DEC_VT510_PC_Belarusian,              		// 82
	DEC_VT510_PC_Faeroese,                		// 83
	DEC_VT510_PC_Gaelic,                  		// 84
	DEC_VT510_PC_Tatar,                   		// 85
	DEC_VT510_PC_Mongolian_Cyrillic,      		// 86
};


// TermId=  5
const CKeyFixups DEC_VT510_PC_FixupPtr =
{
	NULL,                                 		// pointer to parent table (none here)
	DEC_VT510_PC_Fixups,                  		// pointer fixup table
	00, 86,                               		// min = USA, max = Mongolian Cyrillic
};

const CKeyFixup *IBM_3486_102_Fixups[15] =
{
	NoExceptions,                         		// 00 USA not required
	IBM_3486_102_Belgian,                 		// 01
	IBM_3486_102_Finnish_Swedish,         		// 02
	IBM_3486_102_French,                  		// 03
	IBM_3486_102_German,                  		// 04
	IBM_3486_102_Italian,                 		// 05
	IBM_3486_102_Swiss_German,            		// 06
	IBM_3486_102_British,                 		// 07
	IBM_3486_102_Danish,                  		// 08
	IBM_3486_102_Norwegian,               		// 09
	IBM_3486_102_Spanish,                 		// 10
	NULL,                                 		// 11 Dutch not supported
	NULL,                                 		// 12 Hebrew not supported
	NULL,                                 		// 13 Portuguese not supported
	IBM_3486_102_Latin_America,           		// 14
};


// TermId=  6 	, 45 , 84
const CKeyFixups IBM_3486_102_FixupPtr =
{
	&IBM_AT_FixupPtr,                     		// pointer to parent table
	IBM_3486_102_Fixups,                  		// pointer fixup table
	00, 14,                               		// min = USA, max = Latin America
};

const CKeyFixup *IBM_3191_122_Fixups[15] =
{
	NoExceptions,                         		// 00 USA not required
	IBM_3191_122_Belgian,                 		// 01
	NoExceptions,                         		// 02 Finnish (Swedish) not required
	NoExceptions,                         		// 03 French not required
	NoExceptions,                         		// 04 German not required
	NoExceptions,                         		// 05 Italian not required
	IBM_3191_122_Swiss_German,            		// 06
	NoExceptions,                         		// 07 British not required
	NoExceptions,                         		// 08 Danish not required
	NoExceptions,                         		// 09 Norwegian not required
	NoExceptions,                         		// 10 Spanish not required
	NULL,                                 		// 11 Dutch not supported
	NULL,                                 		// 12 Hebrew not supported
	NULL,                                 		// 13 Portuguese not supported
	IBM_3191_122_Latin_America,           		// 14
};


// TermId=  7 	, 24 , 46 , 71
const CKeyFixups IBM_3191_122_FixupPtr =
{
	&IBM_3486_122_FixupPtr,               		// pointer to parent table
	IBM_3191_122_Fixups,                  		// pointer fixup table
	00, 14,                               		// min = USA, max = Latin America
};

const CKeyFixup *IBM_3486_122_Fixups[15] =
{
	IBM_3486_122_USA,                     		// 00
	IBM_3486_122_Belgian,                 		// 01
	IBM_3486_122_Finnish_Swedish,         		// 02
	IBM_3486_122_French,                  		// 03
	IBM_3486_122_German,                  		// 04
	IBM_3486_122_Italian,                 		// 05
	IBM_3486_122_Swiss_German,            		// 06
	IBM_3486_122_British,                 		// 07
	IBM_3486_122_Danish,                  		// 08
	IBM_3486_122_Norwegian,               		// 09
	IBM_3486_122_Spanish,                 		// 10
	NULL,                                 		// 11 Dutch not supported
	NULL,                                 		// 12 Hebrew not supported
	NULL,                                 		// 13 Portuguese not supported
	IBM_3486_122_Latin_America,           		// 14
};


// TermId=  8 	, 72
const CKeyFixups IBM_3486_122_FixupPtr =
{
	&IBM_3486_102_FixupPtr,               		// pointer to parent table
	IBM_3486_122_Fixups,                  		// pointer fixup table
	00, 14,                               		// min = USA, max = Latin America
};

const CKeyFixup *Telex_88_Fixups[11] =
{
	NoExceptions,                         		// 00 USA not required
	Telex_88_Belgian,                     		// 01
	Telex_88_Finnish_Swedish,             		// 02
	Telex_88_French,                      		// 03
	Telex_88_German,                      		// 04
	Telex_88_Italian,                     		// 05
	Telex_88_Swiss_German,                		// 06
	Telex_88_British,                     		// 07
	Telex_88_Danish,                      		// 08
	Telex_88_Norwegian,                   		// 09
	Telex_88_Spanish,                     		// 10
};


// TermId= 25
const CKeyFixups Telex_88_FixupPtr =
{
	&IBM_3486_122_FixupPtr,               		// pointer to parent table
	Telex_88_Fixups,                      		// pointer fixup table
	00, 10,                               		// min = USA, max = Spanish
};

const CKeyFixup *BDS_7_Fixups[11] =
{
	NoExceptions,                         		// 00 USA not required
	BDS_7_Belgian,                        		// 01
	BDS_7_Finnish_Swedish,                		// 02
	BDS_7_French,                         		// 03
	BDS_7_German,                         		// 04
	BDS_7_Italian,                        		// 05
	BDS_7_Swiss_German,                   		// 06
	BDS_7_British,                        		// 07
	BDS_7_Danish,                         		// 08
	BDS_7_Norwegian,                      		// 09
	BDS_7_Spanish,                        		// 10
};


// TermId= 35
const CKeyFixups BDS_7_FixupPtr =
{
	&IBM_AT_FixupPtr,                     		// pointer to parent table
	BDS_7_Fixups,                         		// pointer fixup table
	00, 10,                               		// min = USA, max = Spanish
};

const CKeyFixup *DEC_VT510_LK411_Fixups[32] =
{
	NoExceptions,                         		// 00 USA not required
	DEC_VT510_LK411_Belgian,              		// 01
	DEC_VT510_LK411_Finnish_Swedish,      		// 02
	DEC_VT510_LK411_French,               		// 03
	DEC_VT510_LK411_German,               		// 04
	DEC_VT510_LK411_Italian,              		// 05
	DEC_VT510_LK411_Swiss_German,         		// 06
	DEC_VT510_LK411_British,              		// 07
	DEC_VT510_LK411_Danish,               		// 08
	DEC_VT510_LK411_Norwegian,            		// 09
	DEC_VT510_LK411_Spanish,              		// 10
	DEC_VT510_LK411_Dutch,                		// 11
	DEC_VT510_LK411_Hebrew,               		// 12
	DEC_VT510_LK411_Portuguese,           		// 13
	DEC_VT510_LK411_Latin_America,        		// 14
	DEC_VT510_LK411_Czech_DEC,            		// 15
	DEC_VT510_LK411_Brazilian,            		// 16
	DEC_VT510_LK411_Greek_DEC,            		// 17
	DEC_VT510_LK411_Canadian_French,      		// 18
	DEC_VT510_LK411_Hungarian,            		// 19
	DEC_VT510_LK411_Polish,               		// 20
	DEC_VT510_LK411_SCS,                  		// 21
	DEC_VT510_LK411_Slovakian_DEC,        		// 22
	DEC_VT510_LK411_Swedish,              		// 23
	DEC_VT510_LK411_Turkish_Q,            		// 24
	DEC_VT510_LK411_Romanian,             		// 25
	DEC_VT510_LK411_Russian,              		// 26
	DEC_VT510_LK411_Turkish_F,            		// 27
	NULL,                                 		// 28 Japanese ASCII not supported
	DEC_VT510_LK411_Swiss_French,         		// 29
	NULL,                                 		// 30 USA International not supported
	DEC_VT510_LK411_Slovenian,            		// 31
};


// TermId=104
const CKeyFixups DEC_VT510_LK411_FixupPtr =
{
	&DEC_VT510_PC_FixupPtr,               		// pointer to parent table
	DEC_VT510_LK411_Fixups,               		// pointer fixup table
	00, 31,                               		// min = USA, max = Slovenian
};

const CKeyFixup *Telex_88_DATA_ENTRY_Fixups[1] =
{
	Telex_88_DATA_ENTRY_USA,              		// 00
};


// TermId=112 	,114
const CKeyFixups Telex_88_DATA_ENTRY_FixupPtr =
{
	NULL,                                 		// pointer to parent table (none here)
	Telex_88_DATA_ENTRY_Fixups,           		// pointer fixup table
	00, 00,                               		// min = USA, max = USA
};

const CKeyFixup *APPLE_USB_Fixups[28] =
{
	APPLE_USB_USA,                        		// 00
	APPLE_USB_Belgian,                    		// 01
	APPLE_USB_Finnish_Swedish,            		// 02
	APPLE_USB_French,                     		// 03
	APPLE_USB_German,                     		// 04
	APPLE_USB_Italian,                    		// 05
	APPLE_USB_Swiss_German,               		// 06
	APPLE_USB_British,                    		// 07
	APPLE_USB_Danish,                     		// 08
	APPLE_USB_Norwegian,                  		// 09
	APPLE_USB_Spanish,                    		// 10
	APPLE_USB_Dutch,                      		// 11
	APPLE_USB_Hebrew,                     		// 12
	APPLE_USB_Portuguese,                 		// 13
	APPLE_USB_Latin_America,              		// 14
	APPLE_USB_Czech_DEC,                  		// 15
	NULL,                                 		// 16 Brazilian not supported
	NULL,                                 		// 17 Greek DEC not supported
	APPLE_USB_Canadian_French,            		// 18
	APPLE_USB_Hungarian,                  		// 19
	APPLE_USB_Polish,                     		// 20
	APPLE_USB_SCS,                        		// 21
	APPLE_USB_Slovakian_DEC,              		// 22
	APPLE_USB_Swedish,                    		// 23
	APPLE_USB_Turkish_Q,                  		// 24
	APPLE_USB_Romanian,                   		// 25
	APPLE_USB_Russian,                    		// 26
	APPLE_USB_Turkish_F,                  		// 27
};


// TermId=125
const CKeyFixups APPLE_USB_FixupPtr =
{
	&DEC_VT510_PC_FixupPtr,               		// pointer to parent table
	APPLE_USB_Fixups,                     		// pointer fixup table
	00, 27,                               		// min = USA, max = Turkish F
};

/*
Supported terminals:
--------------------
 	 ID	Name
 	  1	IBM XT
 	  2	IBM PS2
 	  3	IBM AT
 	  5	DEC VT 500 PC-Keyboard
 	  6	IBM 102
 	  7	IBM 3191 (122)
 	  8	IBM 3486 (122)
 	 24	IBM 3180 (122)
 	 25	Telex (88)
 	 35	BDS 7
 	 45	Telex (102)
 	 46	Telex (122)
 	 71	DDC 3496 / Nokia/ICL 472 (122)
 	 72	ADI 1496
 	 84	DDC 3496 (102)
 	 97	IBM Thinkpad PS/2
 	104	DEC VT 500 LK411 keyboard
 	106	IBM Thinkpad AT
 	112	Telex (88) DataEntry
 	114	IBM 3180 (122) DataEntry
 	124	USB EPC keyboard
 	125	Apple USB keyboard
 A total of 22 terminals


Some terminals use a table of an other terminal:
------------------------------------------------
    Unique  Equal		Name
		  3				IBM AT
				  2		IBM PS2
				 97		IBM Thinkpad PS/2
				106		IBM Thinkpad AT
				124		USB EPC keyboard

		  7				IBM 3191 (122)
				 24		IBM 3180 (122)
				 46		Telex (122)
				 71		DDC 3496 / Nokia/ICL 472 (122)

		  6				IBM 102
				 45		Telex (102)
				 84		DDC 3496 (102)

		112				Telex (88) DataEntry
				114		IBM 3180 (122) DataEntry

		  8				IBM 3486 (122)
				 72		ADI 1496




Full list of supported terminals and countries (sorted by TermId):
------------------------------------------------------------------

IBM_XT (TermId = 1)
	USA
	Belgian
	Finnish (Swedish)
	French
	German
	Italian
	Swiss (German)
	British
	Danish
	Norwegian
	Spanish


IBM_PS2 (TermId = 2)
	USA
	Belgian
	Finnish (Swedish)
	French
	German
	Italian
	Swiss (German)
	British
	Danish
	Norwegian
	Spanish
	Dutch
	Hebrew
	Portuguese
	Latin America
	Czech DEC
	Brazilian
	Greek DEC
	Canadian French
	Hungarian
	Polish
	SCS
	Slovakian DEC
	Swedish
	Turkish Q
	Romanian
	Russian
	Turkish F
	Japanese ASCII
	Swiss (French)
	USA International
	Slovenian
	Croatian
	Bosnian
	Macedonian
	Albanian
	Serbian Latin
	Serbian Cyrillic
	Czech QWERTZ
	Czech QWERTY
	Czech Programmers
	Estonian
	Latvian
	Latvian QWERTY
	Lithuania
	Lithuanian IBM
	Slovakian QWERTZ
	Slovakian QWERTY
	Hungarian 101 Key
	Spanish Variation
	Bulgarian Cyrillic
	Bulgarian Latin
	Canadian French Legacy
	Canadian Multilingual
	Italian 142
	Polish 214
	Polish Programmers
	Brazilian MS
	Greek Polytonic
	Greek 220
	Greek 319
	Greek Latin
	Greek 220 Latin
	Greek 319 Latin
	Greek MS
	Russian MS
	Russian Typewriter
	Irish
	Maltese
	Icelandic
	Ukrainian
	Uzbek Cyrillic
	Kazakh
	Kyrgyz Cyrillic
	Azeri Latin
	Azeri Cyrillic
	Belarusian
	Faeroese
	Gaelic
	Tatar
	Mongolian Cyrillic


IBM_AT (TermId = 3)
	USA
	Belgian
	Finnish (Swedish)
	French
	German
	Italian
	Swiss (German)
	British
	Danish
	Norwegian
	Spanish
	Dutch
	Hebrew
	Portuguese
	Latin America
	Czech DEC
	Brazilian
	Greek DEC
	Canadian French
	Hungarian
	Polish
	SCS
	Slovakian DEC
	Swedish
	Turkish Q
	Romanian
	Russian
	Turkish F
	Japanese ASCII
	Swiss (French)
	USA International
	Slovenian
	Croatian
	Bosnian
	Macedonian
	Albanian
	Serbian Latin
	Serbian Cyrillic
	Czech QWERTZ
	Czech QWERTY
	Czech Programmers
	Estonian
	Latvian
	Latvian QWERTY
	Lithuania
	Lithuanian IBM
	Slovakian QWERTZ
	Slovakian QWERTY
	Hungarian 101 Key
	Spanish Variation
	Bulgarian Cyrillic
	Bulgarian Latin
	Canadian French Legacy
	Canadian Multilingual
	Italian 142
	Polish 214
	Polish Programmers
	Brazilian MS
	Greek Polytonic
	Greek 220
	Greek 319
	Greek Latin
	Greek 220 Latin
	Greek 319 Latin
	Greek MS
	Russian MS
	Russian Typewriter
	Irish
	Maltese
	Icelandic
	Ukrainian
	Uzbek Cyrillic
	Kazakh
	Kyrgyz Cyrillic
	Azeri Latin
	Azeri Cyrillic
	Belarusian
	Faeroese
	Gaelic
	Tatar
	Mongolian Cyrillic


DEC_VT510_PC (TermId = 5)
	USA
	Belgian
	Finnish (Swedish)
	French
	German
	Italian
	Swiss (German)
	British
	Danish
	Norwegian
	Spanish
	Dutch
	Hebrew
	Portuguese
	Latin America
	Czech DEC
	Brazilian
	Greek DEC
	Canadian French
	Hungarian
	Polish
	SCS
	Slovakian DEC
	Swedish
	Turkish Q
	Romanian
	Russian
	Turkish F
	Japanese ASCII
	Swiss (French)
	USA International
	Slovenian
	Croatian
	Bosnian
	Macedonian
	Albanian
	Serbian Latin
	Serbian Cyrillic
	Czech QWERTZ
	Czech QWERTY
	Czech Programmers
	Estonian
	Latvian
	Latvian QWERTY
	Lithuania
	Lithuanian IBM
	Slovakian QWERTZ
	Slovakian QWERTY
	Hungarian 101 Key
	Spanish Variation
	Bulgarian Cyrillic
	Bulgarian Latin
	Canadian French Legacy
	Canadian Multilingual
	Italian 142
	Polish 214
	Polish Programmers
	Brazilian MS
	Greek Polytonic
	Greek 220
	Greek 319
	Greek Latin
	Greek 220 Latin
	Greek 319 Latin
	Greek MS
	Russian MS
	Russian Typewriter
	Irish
	Maltese
	Icelandic
	Ukrainian
	Uzbek Cyrillic
	Kazakh
	Kyrgyz Cyrillic
	Azeri Latin
	Azeri Cyrillic
	Belarusian
	Faeroese
	Gaelic
	Tatar
	Mongolian Cyrillic


IBM_3486_102 (TermId = 6)
	USA
	Belgian
	Finnish (Swedish)
	French
	German
	Italian
	Swiss (German)
	British
	Danish
	Norwegian
	Spanish
	Latin America


IBM_3191_122 (TermId = 7)
	USA
	Belgian
	Finnish (Swedish)
	French
	German
	Italian
	Swiss (German)
	British
	Danish
	Norwegian
	Spanish
	Latin America


IBM_3486_122 (TermId = 8)
	USA
	Belgian
	Finnish (Swedish)
	French
	German
	Italian
	Swiss (German)
	British
	Danish
	Norwegian
	Spanish
	Latin America


IBM_3180 (TermId = 24)
	USA
	Belgian
	Finnish (Swedish)
	French
	German
	Italian
	Swiss (German)
	British
	Danish
	Norwegian
	Spanish
	Latin America


Telex_88 (TermId = 25)
	USA
	Belgian
	Finnish (Swedish)
	French
	German
	Italian
	Swiss (German)
	British
	Danish
	Norwegian
	Spanish


BDS_7 (TermId = 35)
	USA
	Belgian
	Finnish (Swedish)
	French
	German
	Italian
	Swiss (German)
	British
	Danish
	Norwegian
	Spanish


Telex_102 (TermId = 45)
	USA
	Belgian
	Finnish (Swedish)
	French
	German
	Italian
	Swiss (German)
	British
	Danish
	Norwegian
	Spanish
	Latin America


Telex_122 (TermId = 46)
	USA
	Belgian
	Finnish (Swedish)
	French
	German
	Italian
	Swiss (German)
	British
	Danish
	Norwegian
	Spanish
	Latin America


DDC_3496_122 (TermId = 71)
	USA
	Belgian
	Finnish (Swedish)
	French
	German
	Italian
	Swiss (German)
	British
	Danish
	Norwegian
	Spanish
	Latin America


ADI_1496 (TermId = 72)
	USA
	Belgian
	Finnish (Swedish)
	French
	German
	Italian
	Swiss (German)
	British
	Danish
	Norwegian
	Spanish
	Latin America


DDC_3496_102 (TermId = 84)
	USA
	Belgian
	Finnish (Swedish)
	French
	German
	Italian
	Swiss (German)
	British
	Danish
	Norwegian
	Spanish
	Latin America


IBM_THINKPAD_PS2 (TermId = 97)
	USA
	Belgian
	Finnish (Swedish)
	French
	German
	Italian
	Swiss (German)
	British
	Danish
	Norwegian
	Spanish
	Dutch
	Hebrew
	Portuguese
	Latin America
	Czech DEC
	Brazilian
	Greek DEC
	Canadian French
	Hungarian
	Polish
	SCS
	Slovakian DEC
	Swedish
	Turkish Q
	Romanian
	Russian
	Turkish F
	Japanese ASCII
	Swiss (French)
	USA International
	Slovenian
	Croatian
	Bosnian
	Macedonian
	Albanian
	Serbian Latin
	Serbian Cyrillic
	Czech QWERTZ
	Czech QWERTY
	Czech Programmers
	Estonian
	Latvian
	Latvian QWERTY
	Lithuania
	Lithuanian IBM
	Slovakian QWERTZ
	Slovakian QWERTY
	Hungarian 101 Key
	Spanish Variation
	Bulgarian Cyrillic
	Bulgarian Latin
	Canadian French Legacy
	Canadian Multilingual
	Italian 142
	Polish 214
	Polish Programmers
	Brazilian MS
	Greek Polytonic
	Greek 220
	Greek 319
	Greek Latin
	Greek 220 Latin
	Greek 319 Latin
	Greek MS
	Russian MS
	Russian Typewriter
	Irish
	Maltese
	Icelandic
	Ukrainian
	Uzbek Cyrillic
	Kazakh
	Kyrgyz Cyrillic
	Azeri Latin
	Azeri Cyrillic
	Belarusian
	Faeroese
	Gaelic
	Tatar
	Mongolian Cyrillic


DEC_VT510_LK411 (TermId = 104)
	USA
	Belgian
	Finnish (Swedish)
	French
	German
	Italian
	Swiss (German)
	British
	Danish
	Norwegian
	Spanish
	Dutch
	Hebrew
	Portuguese
	Latin America
	Czech DEC
	Brazilian
	Greek DEC
	Canadian French
	Hungarian
	Polish
	SCS
	Slovakian DEC
	Swedish
	Turkish Q
	Romanian
	Russian
	Turkish F
	Swiss (French)
	Slovenian


IBM_THINKPAD_AT (TermId = 106)
	USA
	Belgian
	Finnish (Swedish)
	French
	German
	Italian
	Swiss (German)
	British
	Danish
	Norwegian
	Spanish
	Dutch
	Hebrew
	Portuguese
	Latin America
	Czech DEC
	Brazilian
	Greek DEC
	Canadian French
	Hungarian
	Polish
	SCS
	Slovakian DEC
	Swedish
	Turkish Q
	Romanian
	Russian
	Turkish F
	Japanese ASCII
	Swiss (French)
	USA International
	Slovenian
	Croatian
	Bosnian
	Macedonian
	Albanian
	Serbian Latin
	Serbian Cyrillic
	Czech QWERTZ
	Czech QWERTY
	Czech Programmers
	Estonian
	Latvian
	Latvian QWERTY
	Lithuania
	Lithuanian IBM
	Slovakian QWERTZ
	Slovakian QWERTY
	Hungarian 101 Key
	Spanish Variation
	Bulgarian Cyrillic
	Bulgarian Latin
	Canadian French Legacy
	Canadian Multilingual
	Italian 142
	Polish 214
	Polish Programmers
	Brazilian MS
	Greek Polytonic
	Greek 220
	Greek 319
	Greek Latin
	Greek 220 Latin
	Greek 319 Latin
	Greek MS
	Russian MS
	Russian Typewriter
	Irish
	Maltese
	Icelandic
	Ukrainian
	Uzbek Cyrillic
	Kazakh
	Kyrgyz Cyrillic
	Azeri Latin
	Azeri Cyrillic
	Belarusian
	Faeroese
	Gaelic
	Tatar
	Mongolian Cyrillic


Telex_88_DATA_ENTRY (TermId = 112)
	USA


IBM_3180_DATA_ENTRY (TermId = 114)
	USA


USB_EPC_Keyboard (TermId = 124)
	USA
	Belgian
	Finnish (Swedish)
	French
	German
	Italian
	Swiss (German)
	British
	Danish
	Norwegian
	Spanish
	Dutch
	Hebrew
	Portuguese
	Latin America
	Czech DEC
	Brazilian
	Greek DEC
	Canadian French
	Hungarian
	Polish
	SCS
	Slovakian DEC
	Swedish
	Turkish Q
	Romanian
	Russian
	Turkish F
	Japanese ASCII
	Swiss (French)
	USA International
	Slovenian
	Croatian
	Bosnian
	Macedonian
	Albanian
	Serbian Latin
	Serbian Cyrillic
	Czech QWERTZ
	Czech QWERTY
	Czech Programmers
	Estonian
	Latvian
	Latvian QWERTY
	Lithuania
	Lithuanian IBM
	Slovakian QWERTZ
	Slovakian QWERTY
	Hungarian 101 Key
	Spanish Variation
	Bulgarian Cyrillic
	Bulgarian Latin
	Canadian French Legacy
	Canadian Multilingual
	Italian 142
	Polish 214
	Polish Programmers
	Brazilian MS
	Greek Polytonic
	Greek 220
	Greek 319
	Greek Latin
	Greek 220 Latin
	Greek 319 Latin
	Greek MS
	Russian MS
	Russian Typewriter
	Irish
	Maltese
	Icelandic
	Ukrainian
	Uzbek Cyrillic
	Kazakh
	Kyrgyz Cyrillic
	Azeri Latin
	Azeri Cyrillic
	Belarusian
	Faeroese
	Gaelic
	Tatar
	Mongolian Cyrillic


APPLE_USB (TermId = 125)
	USA
	Belgian
	Finnish (Swedish)
	French
	German
	Italian
	Swiss (German)
	British
	Danish
	Norwegian
	Spanish
	Dutch
	Hebrew
	Portuguese
	Latin America
	Czech DEC
	Canadian French
	Hungarian
	Polish
	SCS
	Slovakian DEC
	Swedish
	Turkish Q
	Romanian
	Russian
	Turkish F

*/
/*
Total Memory
------------
	8263 exceptions + 335 pointers
	(4 bytes per exception, 4 bytes per pointer)
		34392 (0x8658) bytes
*/
#endif	// UNICODE_FOR_INTERFACES


