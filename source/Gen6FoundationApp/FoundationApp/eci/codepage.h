/* ==============================================================================
   Codepage translation tables Module Header
   ==============================================================================

   $RCSfile: Gen6FoundationApp/FoundationApp/eci/codepage.h $
   $Revision: 1.2 $
   $Date: 2010/10/01 09:32:13EDT $

   ==============================================================================

   ============================================================================== */
//! \file

#ifndef CODEPAGE_DEFS_H_20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define CODEPAGE_DEFS_H_20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#define CODEPAGE_PRG_VERSION 2		//!< This version gets changed when table structure changes

// CAUTION: DO NOT CHANGE ANY NUMBERS ALREADY DEFINED! THEY ARE VISIBLE TO THE OUTSIDE (Menu, VM).

typedef unsigned char codepage_t;	//!< Use it to store a codepage value (different from Windows!)

/* Codepages with our own privat numbers */
#define CP_NOTSUPPORTED 0           //!< Use it for illegal values

/* Good old ASCII */
#define CP_ISO646	 1                //!< ASCII
#define CP_AUTO	 2						//!< Automatic (used by several places)
#define CP_BINARY	 3						//!< Use this to show we shall not translate
#define CP_ISO646_INVARIANT	 4		//!< The common subset of ASCII

#define ISO_10646LE 	5					//!< Unicode ISO/IEC 10646 UCS-2 (Low order byte first)  same as ISO_UTF16LE
#define ISO_10646BE 	6					//!< Unicode ISO/IEC 10646 UCS-2 (High order byte first) same as ISO_UTF16BE
#define ISO_UTF16LE	 ISO_10646LE	//!< Unicode ISO/IEC 10646 UCS-2 (Low order byte first)  same as ISO_10646LE
#define ISO_UTF16BE	 ISO_10646BE	//!< Unicode ISO/IEC 10646 UCS-2 (High order byte first) same as ISO_10646BE
#define ISO_UTF8	 	7					//!< ISO/IEC 10646 UTF-8

/* MS codepages */
/* Windows */
#define CP1250	10		//!< WinLatin2, Eastern Europe
#define CP1251	11		//!< WinCyrillic
#define CP1252	12		//!< USA and central Europe (superset of ISO_8859_1)
#define CP1253	13		//!< WinGreek
#define CP1254	14		//!< WinTurkish
#define CP1255	15		//!< WinHebrew
#define CP1256	16		//!< WinArabic
#define CP1257	17		//!< WinBaltic
#define CP1258	18		//!< WinVietnamese

#define CP950	28		//!< Win CP50 Taiwan (BIG5)

// Place other MBCS codepages here
/* MS-DOS */
#define CP437	30		//!< USA
#define CP737	31		//!< Greek
#define CP775	32		//!< Baltic
#define CP850	33		//!< Central Europe (Multilingual)
#define CP852	34		//!< Eastern Europe, Slavic (Latin II)
#define CP855	35		//!< Cyrillic
#define CP857	36		//!< Turkish
#define CP866	37		//!< Cyrillic - Russian
#define CP862	38		//!< Hebrew
#define CP864	39		//!< Arabic

#define CP858	40		//!< Latin 1 == CP850 with the dotless i, 0xD5, replaced by euro '€'
#define CP860	41		//!< Portuguese
#define CP861	42		//!< Icelandic
#define CP863	43		//!< Canadian French
#define CP865	44		//!< Nordic
#define CP869	45		//!< Modern Greek

#define CP932	46		//!< Win CP932 Japanese (Shift-JIS)
#define CP936	47		//!< Win CP936 Chinese (GB2312)

/* The 8859-x family */
#define ISO_8859_1  51	//!< West European == first 256 characters of Unicode
#define ISO_8859_2  52	//!< East European
#define ISO_8859_3  53	//!< South European
#define ISO_8859_4  54	//!< Latin4, North Europe
#define ISO_8859_5  55	//!< Cyrillic
#define ISO_8859_6  56	//!< Arabic
#define ISO_8859_7  57	//!< Latin/Greek
#define ISO_8859_8  58	//!< Latin/Hebrew
#define ISO_8859_9  59	//!< Latin5, West European/Turkish
#define ISO_8859_10 60	//!< Latin6,
#define ISO_8859_11 61	//!< Thai
#define ISO_8859_13 63	//!< Latin7, Baltic Rim,  Latvian
#define ISO_8859_14 64	//!< Latin8, Celtic
#define ISO_8859_15 65	//!< Latin9, (Latin0) == Latin1 + extensions (euro sign)
#define ISO_8859_16 66

/* The Russian and Ukrainian codepages */
#define CP_KOI8_U	70		//!< Ukrainia
#define CP_KOI8_R 71		//!< Russia

/* Some Apple-Mac codepages */
#define CP10000	75		//!< MAC CP10000 Roman
#define CP10006	76		//!< MAC CP10006 Greek
#define CP10007	77		//!< MAC CP10007 Cyrillic
#define CP10029	78		//!< MAC CP10029 Latin 2
#define CP10079	79		//!< MAC CP10079 Iceland

/* The old 7-Bit codepages based on ISO 2022 */
#define	Iso_2022_AUTO		 CP_AUTO		//!< Automatic national replacement characters
#define	Iso_2022_06_USA	 CP_ISO646	//!< This is the ASCII table !!
#define	Iso_2022_25_FRA	 81			//!< French/Belgium replacement characters
#define	Iso_2022_11_SWE	 82			//!< Swedish replacement characters
#define	Iso_2022_69_FRA	 83			//!< French/Belgium replacement characters
#define	Iso_2022_21_GER	 84			//!< German replacement characters
#define	Iso_2022_15_ITA	 85			//!< Italian replacement characters
#define	Iso_2022_xx_SWI	 86			//!< Swiss replacement characters
#define	Iso_2022_04_UK		 87			//!< British replacement characters
#define	Iso_2022_xx_DAN	 88			//!< Denmark replacement characters
#define	Iso_2022_61_NOR	 89			//!< Norway replacement characters
#define	Iso_2022_17_SPA	 90			//!< Spain replacement characters

#define	Iso_2022_85_SPA	 91			//!< Spain replacement characters
#define	Iso_2022_16_POR	 92			//!< Portugess replacement characters
#define	Iso_2022_84_POR	 93			//!< Portugess replacement characters
#define	Iso_2022_60_NOR	 94			//!< Norway replacement characters

#define	CP_MAX	 			 95		   //!< highest codepage number

#define	CP7_SIZE				128			//!< size of 7 bit codepage
#define	CP8_SIZE				256			//!< size of 8 bit codepage (single byte)
#define	CP16_SIZE			0x10000		//!< size of 16 bit codepage (Unicode)

#endif // CODEPAGE_DEFS_H_20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

// todo:
// Add ISO646 invariants?




