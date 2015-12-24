/*=================================================================================
  class:

//=================================================================================
   $Source: Gen6FoundationApp/FoundationApp/eci/codepagetables7bit.cpp $
   $Date: 2009/03/05 14:45:46EST $
   $Revision:


//=================================================================================*/
//! \file

#include "stdInclude.h"
#include "TransCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//! A struct containing an ASCII character and the national replacement character.
struct replacements_t
{
	bchar_t 	character;
	wchar_t	replacement;
};

// USA	00
// Character set:	ISO 6 (ASCII)
const replacements_t Iso_646_ASCII[] =
{
	0,0,			// end of table
};

// BELGIUM     01
// Character set:	ISO 25	(ISO French/Belgium)
// Replaced by ISO 2022-IR69. The only difference is the character 0x60.
// This is what we did in the past.
const replacements_t Iso_BELG_IR25[] =
{
	'#',(UCHAR)'£',		// pound (English)
	'@',(UCHAR)'à',		// a with grave
	'[',(UCHAR)'°',		// degree sign
	'\\',(UCHAR)'ç',		// c with cedilla
	']',(UCHAR)'§',		// section sign
	'{',(UCHAR)'é',		// e with acute
	'|',(UCHAR)'ù',		// u with grave
	'}',(UCHAR)'è',		// e with grave
	'~',(UCHAR)'¨',		// umlaut
	0,0						// end
};

// BELGIUM     01
// Character set:	ISO 69	(ISO French/Belgium)
const replacements_t Iso_BELG[] =
{
	'#',(UCHAR)'£',		// pound (English)
	'@',(UCHAR)'à',		// a with grave
	'[',(UCHAR)'°',		// degree sign
	'\\',(UCHAR)'ç',		// c with cedilla
	']',(UCHAR)'§',		// section sign
	'{',(UCHAR)'é',		// e with acute
	'|',(UCHAR)'ù',		// u with grave
	'}',(UCHAR)'è',		// e with grave
	'~',(UCHAR)'¨',		// umlaut
	'`',(UCHAR)'µ',		// micron
	0,0						// end
};

// SWEDEN_FINNLAND 02	(Sweden/Finnland)
// Character set:	ISO 11	(ISO Swedish)
const replacements_t Iso_SCAN[] =
{
	'$',(UCHAR)'¤',		// general currency symbol
	'@',(UCHAR)'É',		// E with acute
	'[',(UCHAR)'Ä',		// A with umlaut
	'\\',(UCHAR)'Ö',		// O with umlaut
	']',(UCHAR)'Å',		// A with circle above
	'^',(UCHAR)'Ü',		// U with umlaut
	'`',(UCHAR)'é',		// e with acute
	'{',(UCHAR)'ä',		// a with umlaut
	'|',(UCHAR)'ö',		// o with umlaut
	'}',(UCHAR)'å',		// a with circle above
	'~',(UCHAR)'ü',		// u with umlaut
 	0,0						// end
};

// GERMANY     04
// Character set:	ISO 21	(ISO German)
const replacements_t Iso_GERM[] =
{
	'@',(UCHAR)'§',		// section sign
	'[',(UCHAR)'Ä',		// A with umlaut
	'\\',(UCHAR)'Ö',		// O with umlaut
	']',(UCHAR)'Ü',		// U with umlaut
	'{',(UCHAR)'ä',		// a with umlaut
	'|',(UCHAR)'ö',		// o with umlaut
	'}',(UCHAR)'ü',		// u with umlaut
	'~',(UCHAR)'ß',		// eszet (Looks like the beta sign, spoken like ss)
 	0,0						// end
};

// ITALY       05
// Character set:	ISO 15	(ISO Italian)
const replacements_t Iso_ITAL[] =
{
	'#',(UCHAR)'£',		// pound (English)
	'@',(UCHAR)'§',		// section sign
	'[',(UCHAR)'°',		// degree sign
	'\\',(UCHAR)'ç',		// c with cedilla
	']',(UCHAR)'é',		// e with acute
	'`',(UCHAR)'ù',		// u with grave
	'{',(UCHAR)'à',		// a with grave
	'|',(UCHAR)'ò',		// o with grave
	'}',(UCHAR)'è',		// e with grave
	'~',(UCHAR)'ì',		// i with grave
	0,0						// end
};

// SWITZERLAND 06
// Character set:	ISO ??	(ISO Switzerland (German/French))
const replacements_t Iso_SWIT[] =
{
	'#',(UCHAR)'£',		// pound (English)
	'@',(UCHAR)'ç',		// c with cedilla
	'[',(UCHAR)'à',		// a with grave
	'\\',(UCHAR)'é',		// e with acute
	']',(UCHAR)'è',		// e with grave
	'{',(UCHAR)'ä',		// a with umlaut
	'|',(UCHAR)'ö',		// o with umlaut
	'}',(UCHAR)'ü',		// u with umlaut
	'~',(UCHAR)'¨',		// umlaut
	0,0						// end
};

// UK          07
// Character set:	ISO 4	(ISO United Kingdom)
const replacements_t Iso_UKIN[] =
{
	'#',(UCHAR)'£',		// pound (English)
	0,0						// end
};

// DENMARK     08
// Character set:	ISO ??	(ISO Denmark)
const replacements_t Iso_DENM[] =
{
	'[',(UCHAR)'Æ',		// AE ligature
	'\\',(UCHAR)'Ø',		// O with stroke
	']',(UCHAR)'Å',		// A with circle above
	'{',(UCHAR)'æ',		// ae ligature
	'|',(UCHAR)'ø',		// o with stroke
	'}',(UCHAR)'å',		// a with circle above
	0,0						// end
};

// NORWAY      09
// Character set:	ISO 60	(ISO Norway)
const replacements_t Iso_NORW60[] =
{
	'[',(UCHAR)'Æ',		// U+00C6 : LATIN CAPITAL LETTER AE
	'\\',(UCHAR)'Ø',		// U+00D8 : LATIN CAPITAL LETTER O WITH STROKE
	']',(UCHAR)'Å',		// U+00C5 : LATIN CAPITAL LETTER A WITH RING ABOVE
	'{',(UCHAR)'æ',		// U+00E6 : LATIN SMALL LETTER AE
	'|',(UCHAR)'ø',		// U+00F8 : LATIN SMALL LETTER O WITH STROKE
	'}',(UCHAR)'å',		// U+00E5 : LATIN SMALL LETTER A WITH RING ABOVE
	'~',0x203E,				// U+203E : OVERLINE
	0,0						// end
};

// NORWAY      09
// Character set:	ISO 61	(ISO Norway)
const replacements_t Iso_NORW61[] =
{
	'#',(UCHAR)'§',		// U+00A7 : SECTION SIGN
	'~',(UCHAR)'|',		// |	(preserve the order, must be before '|',(UCHAR)'ø')
	'[',(UCHAR)'Æ',		// AE ligature
	'\\',(UCHAR)'Ø',		// O with stroke
	']',(UCHAR)'Å',		// A with circle above
	'{',(UCHAR)'æ',		// ae ligature
	'|',(UCHAR)'ø',		// o with stroke
	'}',(UCHAR)'å',		// a with circle above
	0,0						// end
};

// SPANISH     10
// Character set:	ISO 85	(ISO Spain)
const replacements_t Iso_SPAI85[] =
{
	'@',(UCHAR)'·',		// U+00B7 : MIDDLE DOT
	'[',(UCHAR)'¡',		// U+00A1 : INVERTED EXCLAMATION MARK
	'\\',(UCHAR)'Ñ',		// U+00D1 : LATIN CAPITAL LETTER N WITH TILDE
	']',(UCHAR)'Ç',		// U+00C7 : LATIN CAPITAL LETTER C WITH CEDILLA
	'^',(UCHAR)'¿',		// U+00BF : INVERTED QUESTION MARK
	'{',(UCHAR)'´',		// U+00B4 : ACUTE ACCENT
	'|',(UCHAR)'ñ',		// U+00F1 : LATIN SMALL LETTER N WITH TILDE
	'}',(UCHAR)'ç',		// U+00E7 : LATIN SMALL LETTER C WITH CEDILLA
	'~',(UCHAR)'¨',		// U+00A8 : DIAERESIS
	0,0						// end
};

// SPANISH     10
// Character set:	ISO 17	(ISO Spain)
const replacements_t Iso_SPAI17[] =
{
	'#',(UCHAR)'£',		// pound
	'@',(UCHAR)'§',		// section sign
	'[',(UCHAR)'¡',		// upside down exclamation mark
	'\\',(UCHAR)'Ñ',		// N with tilde
	']',(UCHAR)'¿',		// upside down question mark
	'{',(UCHAR)'°',		// degree sign
	'|',(UCHAR)'ñ',		// n with tilde
	'}',(UCHAR)'ç',		// c with cedilla
	0,0						// end
};

// PORTUGESE   13
// Character set:	ISO 84	(ISO Portugal)
const replacements_t Iso_PORT84[] =
{
	'@',(UCHAR)'´',		// U+00B4 : ACUTE ACCENT
	'[',(UCHAR)'Ã',		// U+00C3 : LATIN CAPITAL LETTER A WITH TILDE
	'\\',(UCHAR)'Ç',		// U+00C7 : LATIN CAPITAL LETTER C WITH CEDILLA
	']',(UCHAR)'Õ',		// U+00D5 : LATIN CAPITAL LETTER O WITH TILDE
	'{',(UCHAR)'ã',		// U+00E3 : LATIN SMALL LETTER A WITH TILDE
	'|',(UCHAR)'ç',		// U+00E7 : LATIN SMALL LETTER C WITH CEDILLA
	'}',(UCHAR)'õ',		// U+00F5 : LATIN SMALL LETTER O WITH TILDE
	0,0						// end
};

// PORTUGESE   13
// Character set:	ISO 16	(ISO Portugal)
const replacements_t Iso_PORT16[] =
{
	'@',(UCHAR)'§',		// U+00A7 : SECTION SIGN
	'[',(UCHAR)'Ã',		// U+00C3 : LATIN CAPITAL LETTER A WITH TILDE
	'\\',(UCHAR)'Ç',		// U+00C7 : LATIN CAPITAL LETTER C WITH CEDILLA
	']',(UCHAR)'Õ',		// U+00D5 : LATIN CAPITAL LETTER O WITH TILDE
	'{',(UCHAR)'ã',		// U+00E3 : LATIN SMALL LETTER A WITH TILDE
	'|',(UCHAR)'ç',		// U+00E7 : LATIN SMALL LETTER C WITH CEDILLA
	'}',(UCHAR)'õ',		// U+00F5 : LATIN SMALL LETTER O WITH TILDE
	'~',(UCHAR)'°',		// U+00B0 : DEGREE SIGN
	0,0						// end
};

//////////////////////////////////////////////////////////////////////////////////////////
//! A struct that maps a codepage to a list of national replacement characters.
struct TranslateItem_t
{
	codepage_t	Codepage;
	const replacements_t	*pTable;
};

//////////////////////////////////////////////////////////////////////////////////////////
//! All supported 7 bit codepages.
const TranslateItem_t All7BitCodePages[] =
{
	Iso_2022_04_UK,	Iso_UKIN,
	Iso_2022_06_USA,	Iso_646_ASCII,
	Iso_2022_11_SWE,	Iso_SCAN,
	Iso_2022_15_ITA,	Iso_ITAL,
	Iso_2022_16_POR,	Iso_PORT16,
	Iso_2022_17_SPA,	Iso_SPAI17,
	Iso_2022_21_GER,	Iso_GERM,
	Iso_2022_25_FRA,	Iso_BELG_IR25,
	Iso_2022_60_NOR,	Iso_NORW60,
	Iso_2022_61_NOR,	Iso_NORW61,
	Iso_2022_69_FRA,	Iso_BELG,
	Iso_2022_85_SPA,	Iso_SPAI85,
	Iso_2022_84_POR,	Iso_PORT84,
	Iso_2022_xx_SWI,	Iso_SWIT,
	Iso_2022_xx_DAN,	Iso_DENM,
	CP_NOTSUPPORTED,	NULL
};

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
/// Handler for the 7 Bit codepages
/*!
*/
class CTransInfo7Bit : public CTransInfoBase
{
public:
	CTransInfo7Bit(codepage_t codepage, const replacements_t *pSrc);
protected:
	virtual MatrixError_t FromUnicode(const wchar_t *pIn, bchar_t *pOut, size_t &DestinationSize);
	virtual MatrixError_t FromUnicode(const wchar_t *pIn, size_t length, bchar_t *pOut, size_t &DestinationSize);
	virtual bool TranslateInPlace(bchar_t *pIn, size_t length);

	void FromUnicode_(wchar_t In, bchar_t &Out);
};

CTransInfo7Bit::CTransInfo7Bit(codepage_t codepage, const replacements_t *pSrc)
: CTransInfoBase(CP8_SIZE, codepage)
{
	m_capabilities = FL_7Bit;
	wchar_t	*pDest = m_pTable;
	size_t i;

	// lower half is filled from base class with 1:1 translation
	// then fill in the rest with illegal Unicode
	for(i=CP7_SIZE; i<CP8_SIZE; i++)
	{
		pDest[i] = 0xFFFF;
	}
	// now fill in the national replacements
	while(pSrc->character != 0)
	{
		wchar_t temp = pSrc->replacement;
		#ifndef UNICODE_FOR_INTERFACES
		if(temp > 256)	// can not be translated in a 8 bit codepage
			temp = '?';
		#endif
		pDest[pSrc->character] = temp;
		pSrc++;
	};
}

const size_t NumReplaces = 12;
const wchar_t ReplaceChars[NumReplaces] =
{
	'[','\\',']','^',
	'{','|','}','~',
	// these are rar, so I keep them at the end
	'`',
	'@',
	'#','$',
};

void CTransInfo7Bit::FromUnicode_(wchar_t In, bchar_t &Out)
{
	// nost values below 128 are 1:1
	// try these first for speed
	if(In <= CP7_SIZE)
	{
		if(m_pTable[In] == In)
		{
			Out = (bchar_t)In;
			return;	// speed
		}
	}

	// only a few (12) characters are replaced
	for (size_t i=0; i<NumReplaces; i++)
	{
		if(m_pTable[ReplaceChars[i]] == In)
		{
			Out = (bchar_t)ReplaceChars[i];
			return;	// speed
		}
	};
	Out = '?';
}

MatrixError_t CTransInfo7Bit::FromUnicode(const wchar_t *pIn, bchar_t *pOut, size_t &DestinationSize)
{
	bchar_t *pStart=pOut;
	while(*pIn != 0)
	{
		if (DestinationSize-- == 0)
			return ERR_INSUFFIZIENT_BUFFER;
		FromUnicode_(*pIn++,*pOut++);
	};
	DestinationSize = pOut-pStart;
	return PASS;
}

MatrixError_t CTransInfo7Bit::FromUnicode(const wchar_t *pIn, size_t length, bchar_t *pOut, size_t &DestinationSize)
{
	if(DestinationSize < length)
		return ERR_INSUFFIZIENT_BUFFER;
	DestinationSize = length;	            // return final size
	for (size_t i=0; i<length; i++)
	{
		FromUnicode_(*pIn++,*pOut++);
	};
	return PASS;
}

// Caution: This only works for system codepage == CP1252 (or ISO8859_1)
bool CTransInfo7Bit::TranslateInPlace(bchar_t *pIn, size_t length)
{
	for (size_t i=0; i<length; i++)
	{
		*pIn = (bchar_t) m_pTable[*pIn];
		pIn++;
	};
	return PASS;
}

///////////////////////////////////////////////////////////////////////////////
//! Create the 7 bit codepage handler.
/*! 
*/
void	CTransCode::Create7BitTables(void)
{
	const TranslateItem_t *pList = All7BitCodePages;

	while ( pList->Codepage != CP_NOTSUPPORTED )
	{
		codepage_t codepage = pList->Codepage;
		ASSERT(codepage < CP_MAX);
		if(codepage < CP_MAX)
		{
			ASSERT(m_TransInfo[codepage] == NULL);
			m_TransInfo[codepage] = new CTransInfo7Bit(codepage, pList->pTable);
		}
		pList++;
	}
}

//////////////////////////////////////////////////////////////////////
//! Look up table for auto selection of the codepage based on country.
/*! This is for compatibility to older products
*/
const TranslateItem_t AutoIsoCP[] =
{
	Iso_2022_06_USA,	Iso_646_ASCII,
	Iso_2022_69_FRA,	Iso_BELG,
	Iso_2022_11_SWE,	Iso_SCAN,
	Iso_2022_69_FRA,	Iso_BELG,
	Iso_2022_21_GER,	Iso_GERM,
	Iso_2022_15_ITA,	Iso_ITAL,
	Iso_2022_xx_SWI,	Iso_SWIT,
	Iso_2022_04_UK,	Iso_UKIN,
	Iso_2022_xx_DAN,	Iso_DENM,
	Iso_2022_61_NOR,	Iso_NORW61,
	Iso_2022_17_SPA,	Iso_SPAI17,
};

///////////////////////////////////////////////////////////////////////////////
//! Create the 7 bit codepage handler with auto select.
/*! 
*/
void	CTransCode::CreateAuto7BitTable(int country)
{
	if (country >= ( sizeof(AutoIsoCP)/sizeof(AutoIsoCP[0]) ) )
		country = 0;

	const replacements_t *pTable = AutoIsoCP[country].pTable;
	codepage_t codepage = Iso_2022_AUTO;

	// remove old table if any
	if (m_TransInfo[codepage] != NULL)
		delete m_TransInfo[codepage];

	m_TransInfo[codepage] = new CTransInfo7Bit(codepage, pTable);
}





