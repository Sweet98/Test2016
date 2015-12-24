/*=================================================================================
  Codepage translations. This class does not know internas of the CBarcodeData.
	It works on buffers and sizes.
//=================================================================================
   $Source: Gen6FoundationApp/FoundationApp/eci/transcode.cpp $
   $Date: 2010/10/28 18:46:11EDT $
   $Revision: 1.3 $

//=================================================================================*/
//! \file

#include "stdInclude.h"
#include "TransCode.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uchar.h"
#include "ManagedBuffer.h"
#include "BufferPool.h"

#ifdef _WINDOWS
#include "menuSimulation.h"
#else
#include "menu.h"
#include "ReaderConfiguration.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// A helper class to keep the higher level a little simpler
CTransInfoBase::CTransInfoBase(size_t size, codepage_t codepage)
{
	m_codepage		= codepage;
	m_InternalTableSize = size;
	if(size!=0)
	{
		m_TableSize = size;                 // create a table for the derived class
		m_pTable	= new wchar_t[size];
		for(size_t i=0; i<size; i++)       	   // fill with 1:1 translation as a service for derived classes
		{
			m_pTable[i] = (wchar_t)i;
		}
	}
	else
	{
		m_TableSize = 0;       	            // derived class has it's own table
		m_pTable = NULL;
	}
	m_capabilities = 0;
}

CTransInfoBase::~CTransInfoBase(void)
{
	if(m_InternalTableSize!=0)  	         // we must only delete the table if we did create it
	{
		delete [] m_pTable;
	}
}

///////////////////////////////////////////////////////////////////////////////
//! Translate a character into unicode.
/*! 
 \param *pIn  Point to the source character
 \param *pOut Point to the destination character
*/
inline void CTransInfoBase::ToUnicodeChar(const bchar_t *pIn, wchar_t *pOut)
{
	ASSERT(pIn!=NULL);
	ASSERT(pOut!=NULL);
	bchar_t InChar = *pIn;
	if(InChar < m_TableSize)
	{
		*pOut = m_pTable[InChar];
	}
	else
	{
		*pOut = 0xffff;	// illegal UNICODE
	}
}

///////////////////////////////////////////////////////////////////////////////
//! Translate a zero terminated string to unicode.
/*! 
 \param *pIn  Point to the source buffer
 \param *pOut Point to the destination buffer
 \param &DestinationSize In: Size of the destination buffer, Out: used buffersize
 \return MatrixError_t
*/
MatrixError_t CTransInfoBase::ToUnicode(const bchar_t *pIn, wchar_t *pOut, size_t &DestinationSize)
{
	ASSERT(pIn!=NULL);
	ASSERT(pOut!=NULL);
	wchar_t *pStart=pOut;
	while(*pIn != 0)
	{
		if (DestinationSize-- == 0)
			return ERR_INSUFFIZIENT_BUFFER;
		ToUnicodeChar(pIn++, pOut++);
	};
	DestinationSize = pOut-pStart;
	return(ERR_PASS);
}

///////////////////////////////////////////////////////////////////////////////
//! Translate a string with length to unicode.
/*! 
 \param *pIn  Point to the source buffer
 \param length Size of the string in the source buffer
 \param *pOut Point to the destination buffer
 \param &DestinationSize In: Size of the destination buffer, Out: used buffersize
 \return MatrixError_t
*/
MatrixError_t CTransInfoBase::ToUnicode(const bchar_t *pIn, size_t length, wchar_t *pOut, size_t &DestinationSize)
{
	ASSERT(pIn!=NULL);
	ASSERT(pOut!=NULL);
	if(DestinationSize < length)
		return ERR_INSUFFIZIENT_BUFFER;
	DestinationSize = length;	            // return final size
	for (size_t i=0; i<length; i++)
	{
		ToUnicodeChar(pIn++, pOut++);
	};
	return(ERR_PASS);
}

///////////////////////////////////////////////////////////////////////////////
//! Translate a string in place.
/*! This is a dummy function for overloading by derived classes.
 \param pIn Point to the buffer 
 \param length Size of the string in the buffer 
 \return true if ok
*/
bool CTransInfoBase::TranslateInPlace(bchar_t * /* pIn */ , size_t /* length */)
{
	return false;	// never call it
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//! Constructor that creates all the tables for the translation process.
/*! 
*/
CTransCode::CTransCode(void)
{
	// fill pointers with NULL
	for(int i=0; i<CP_MAX; i++)
	{
		m_TransInfo[i] = NULL;
	}
	m_DefaultChar = '?';
	// Now create the dynamic codepage tables (system codepage)
	OnChangedConfiguration();

	Create7BitTables();
	// the order of the below functions is important
	Create8BitTables();

	CreateUTF8Translator();
	CreateUTF16Translator();
	CreateCp932Translator();
	CreateCp936Translator();
	CreateCp950Translator();
}

///////////////////////////////////////////////////////////////////////////////
//! Destructor cleans all the tables.
/*! 
*/
CTransCode::~CTransCode(void)
{
	// free all the tables
	for(int i=0; i<CP_MAX; i++)
	{
		delete m_TransInfo[i];
	}
}

///////////////////////////////////////////////////////////////////////////////
/// A helper to maintain lists of codepages.
/*!
*/
class Country_Codepage_t
{
public:	// MS compiler needs that (unfortunatelly)
	codepage_t	m_WinCP;									//!< Windows codepage
	codepage_t	m_DOSCP;									//!< MS-DOS codepage
public:
	codepage_t GetWinCodepage(void) const { return m_WinCP;	}	//!< Retrieves the windows codepage
	codepage_t GetDOSCodepage(void) const { return m_DOSCP;	}	//!< Retrieves the MS-DOS codepage
};

#ifndef CP932
#define CP932  CP_NOTSUPPORTED
#endif

//! This table contains the 8 bit codepages we use for various countries.
/*!  Now with Unicode this is less important, but we still need it.
*/
static const Country_Codepage_t CountryToCodePage[] =
{
	CP1252,	CP437,	// USA                00
	CP1252,	CP850,	// BELGIUM            01
	CP1252,	CP850,	// FINNISH       		 02 ;Finland (Sweden)
	CP1252,	CP850,	// FRANCE             03
	CP1252,	CP850,	// GERMANY            04 ;Germany/Austria
	CP1252,	CP850,	// ITALY              05
	CP1252,	CP850,	// SWITZERLAND        06
	CP1252,	CP850,	// UK                 07 ;English
	CP1252,	CP850,	// DENMARK            08
	CP1252,	CP850,	// NORWAY             09
	CP1252,	CP850,	// SPAIN              10
	CP1252,	CP850,	// DUTCH              11
	CP1255,	CP862,	// HEBREW    			 12
	CP1252,	CP850,	// PORTUGAL           13
	CP1252,	CP850,	// LATIN_AMERICA      14
	CP1250,	CP852,	// CZEK               15
	CP1252,	CP850,	// BRAZIL				 16
	CP1253,	CP737,	// GREEK					 17
	CP1252,	CP850,	// CANADIAN_FRENCH    18
	CP1250,	CP852,	// HUNGARIAN          19
	CP1250,	CP852,	// POLISH             20
	CP1250,	CP852,	// SCS                21
	CP1250,	CP852,	// SLOVAK_DEC         22
	CP1252,	CP850,	// SWEDISH            23
	CP1254, 	CP857,	// TURKISH_Q          24
	CP1250,	CP852,	// ROMANIA            25
	CP1251,	CP866,	// RUSSIAN            26
	CP1254,	CP857,	// TURKISH_F          27
	CP950,	CP950,	// TAIWAN_BIG5		 	28
	CP1252,	CP850,	// SWITZERLAND_FRENCH	29
	CP1252,	CP437,	// USA_INTERNATIONAL	30
	CP1250,	CP852,	// SLOVENIA				31
	CP1250,	CP852,	// CROATIA				32
	ISO_8859_2,ISO_8859_2,	// BOSNIA		33
	CP1251,	CP866,	// MACEDONIA			34
	CP1250,	CP852,	// ALBANIA				35
	CP1250,	CP852,	// SERBIA_LATIN		36
	CP1251,	CP855,	// SERBIA_CYRILLIC	37
	CP1250,	CP852,	// CZECH_QWERTZ		38
	CP1250,	CP852,	// CZECH_QWERTY		39
	CP1250,	CP852,	// CZECH_PROGRAMMERS	40
	CP1257,	CP775,	// ESTONIA				41
	CP1257,	CP775,	// LATVIA				42
	CP1257,	CP775,	// LATVIA_QWERTY		43
	CP1257,	CP775,	// LITHUANIA			44
	CP1257,	CP775,	// LITHUANIAN_IBM		45
	CP932,	CP932,	// JAPAN_ASCII       	46
	CP936,	CP936,	// CHINA_GB2312       	47
	CP1250,	CP852,	// SLOVAK_QWERTZ		48
	CP1250,	CP852,	// SLOVAK_QWERTY		49
	CP1250,	CP852,	// HUNGARIAN_101_KEY	50
	CP1252,	CP850,	// SPANISH_VARIATION	51
	CP1251,	CP866,	// BULGARIA_CYRILLIC	52
	CP1251,	CP866,	// BULGARIA_LATIN		53
	CP1252,	CP850,	// CANADIAN_FRENCH_LEGACY	54
	CP1252,	CP850,	// CANADIAN_MULTILINGUAL,	55
	CP1252,	CP850,	// ITALIAN_142,				56
	CP1250,	CP852,	// POLISH_214,					57
	CP1250,	CP852,	// POLISH_PROGRAMMERS,		58
	CP1252,	CP850,	// BRAZIL_MS,					59
	CP1253,	CP737,	// GREEK_POLYTONIC,			60
	CP1253,	CP737,	// GREEK_220,					61
	CP1253,	CP737,	// GREEK_319,					62
	CP1253,	CP737,	// GREEK_LATIN,				63
	CP1253,	CP737,	// GREEK_220_LATIN,        64
	CP1253,	CP737,	// GREEK_419_LATIN,        65
	CP1253,	CP737,	// GREEK_MS,		         66
	CP1251,	CP866,	// RUSSIAN_MS,
	CP1251,	CP866,	// RUSSIAN_TYPEWRITER,
	CP1252,	CP437,	// Reserved
	CP1252,	CP437,	// Reserved
	CP1252,	CP437,	// Reserved
	CP1252,	CP437,	// Reserved
	CP1252,	CP850,	// IRISH						73
	ISO_8859_3,ISO_8859_3,	// MALTA			,
	CP1252,	CP850,	// ICELAND				,
	CP1251,	CP866,	// UKRAINIAN,
	CP1251,	CP866,	// UZBEK_CYRILLIC,
	CP1251,	CP866,	// KAZAKH,
	CP1251,	CP866,	// KYRGYZ_CYRILLIC,
	CP1254,	CP857,	// AZERI_LATIN,
	CP1251,	CP866,	// AZERI_CYRILLIC,
	CP1251,	CP866,	// BELARUSIAN,
	CP1252,	CP850,	// FAEROESE,
	CP1252,	CP850,	// GAELIC,
	CP1251,	CP866,	// TATAR,
	CP1251,	CP866,	// MONGOLIAN_CYRILLIC,
	CP1252,	CP437,	// USA_DVORAK,
	CP1252,	CP437,	// USA_DVORAK_LEFT,
	CP1252,	CP437,	// USA_DVORAK_RIGHT,
};

///////////////////////////////////////////////////////////////////////////////
//! Retrieves the windows codepage for a given country.
/*! 
 \param country Country ID
 \return Codepage
*/
codepage_t CTransCode::GetWinCodepage(int country)
{
	codepage_t ret = CP_NOTSUPPORTED;
	if (country < (sizeof(CountryToCodePage)/sizeof(CountryToCodePage[0])) )
	{
		ret = CountryToCodePage[country].GetWinCodepage();
	}
	return ret;
}

///////////////////////////////////////////////////////////////////////////////
//! Retrieves the MS-DOS codepage for a given country.
/*! 
 \param country Country ID
 \return Codepage
*/
codepage_t CTransCode::GetDOSCodepage(int country)
{
	codepage_t ret = CP_NOTSUPPORTED;
	if (country < (sizeof(CountryToCodePage)/sizeof(CountryToCodePage[0])) )
	{
		ret = CountryToCodePage[country].GetDOSCodepage();
	}
	return ret;
}


///////////////////////////////////////////////////////////////////////////////
//! Gets called by the framework after a menu change
/*! This recreates all tables that are affected by a change in the settings.
*/
void 	CTransCode::OnChangedConfiguration(void)
{
	int country;
	country = GET_SETTING(KBD_CtryCode);

	// for support of the codepage ID = 2: same behaviour as older products
	CreateAuto7BitTable(country);

	// if we want to translate old menu stuff (menu response, formatter, Xfixes)
	// to unicode, then we need the correct 8 bit codepage as the system codepage.
	m_SystemCodepage = CP_NOTSUPPORTED;    // fill illegal value, so we can fix it in one single place
	int SystemCodepage =  GET_SETTING(EciSystemCodepage);
	if(SystemCodepage == CP_AUTO)
	{
		m_SystemCodepage = CountryToCodePage[country].GetWinCodepage();
	}
	else
	{
		m_SystemCodepage = SystemCodepage;
	}

	if(m_SystemCodepage == CP_NOTSUPPORTED)
	{
		m_SystemCodepage = CP1252;	// emergeny backup
	}
	CreateSystemCPTable();
}

///////////////////////////////////////////////////////////////////////////////
//! Check whether the codepage is valid.
/*! That means we have an entry in our translation list.
 \param codepage Codepage
 \return true if valid
*/
bool CTransCode::IsValidCP(codepage_t codepage)
{
	ASSERT(codepage < CP_MAX);
	if(codepage < CP_MAX)
	{
		if(m_TransInfo[codepage] != NULL)
		{
			return(PASS);
		}
	}
	return(FAIL);
}

///////////////////////////////////////////////////////////////////////////////
//! Check codepage type.
/*! There are several types of codepages. Some only contain 128 characters (7 bit).
 \param codepage Codepage
 \return true if it is a 7 bit codepage
*/
bool CTransCode::Is7Bit(codepage_t codepage)
{
	if(IsValidCP(codepage))
	{
		return m_TransInfo[codepage]->Is7Bit();
	}
	return(FAIL);
}

///////////////////////////////////////////////////////////////////////////////
//! Check codepage type.
/*! There are several types of codepages. Some contain 256 characters (8 bit).
 \param codepage Codepage
 \return true if it is a 8 bit codepage
*/
inline bool CTransCode::Is8Bit(codepage_t codepage)
{
	if(IsValidCP(codepage))
	{
		return m_TransInfo[codepage]->Is8Bit();
	}
	return(FAIL);
}

///////////////////////////////////////////////////////////////////////////////
//! Check codepage type.
/*! There are several types of codepages. Some contain more than 256 characters and
	uses switch bytes to do some kind of mapping. MBCS == Multi Byte Character Set.
 \param codepage Codepage
 \return true if it is a MBCS bit codepage
*/
inline bool CTransCode::IsMBCS(codepage_t codepage)
{
	if(IsValidCP(codepage))
	{
		return m_TransInfo[codepage]->IsMBCS();
	}
	return(FAIL);
}

///////////////////////////////////////////////////////////////////////////////
//! Menu routine to report the codepage IDs in textual form. 
/*! Called in response to a "xxxDCP*" command.
	Will report all ID numbers supported by this firmware....all numbers in the caller 
	supplied buffer.
 \param *pDst Destination buffer
 \param BufferSize Size of the destination buffer
 \return Number of bytes used in buffer
*/
int CTransCode::CollectAllCodePages(int *pDst, int BufferSize)
{
	int iIndex = 0;
	for(int i=0; i<CP_MAX; i++)
	{
		if(m_TransInfo[i] != NULL)
		{
			pDst[iIndex]=m_TransInfo[i]->GetCurrentCodepage();
			iIndex++;
			if(iIndex >= BufferSize)
				break;
		}
	}
	return(iIndex);
}

#if(0) // not used anymore
void CTransCode::CollectAll7BitCodePages(UCHAR *pDst, size_t BufferSize)
{
	size_t size = strlen(pDst);	                      // we need to append to the string
	for(int i=0; i<CP_MAX; i++)
	{
		if(m_TransInfo[i] != NULL)
		{
			if(Is7Bit(i))
			{
				size += snprintf(pDst+size, BufferSize-size, "%d|", i);
			}
		}
	}
	*(--pDst) = '\0'; /* remove last separator */
}
#endif

///////////////////////////////////////////////////////////////////////////////
//! Translate a character into unicode.
/*! 
 \param codepage Codepage of character
 \param in		Source character
 \param &out   Out: Destination character
 \return MatrixError_t
*/
MatrixError_t CTransCode::CharacterToUnicode(codepage_t codepage, bchar_t in, wchar_t &out)
{
	if(IsValidCP(codepage))
	{
		size_t DummySize=1;
		return m_TransInfo[codepage]->ToUnicode(&in, 1, &out, DummySize);
	}
	out = m_DefaultChar;
	return(ERR_UNKNOWN_CODEPAGE);
}

///////////////////////////////////////////////////////////////////////////////
//! Translate a zero terminated string to unicode.
/*! 
 \param codepage Codepage of string
 \param *pIn  Point to the source buffer
 \param *pOut Point to the destination buffer
 \param &DestinationSize In: Size of the destination buffer, Out: used buffersize
 \return MatrixError_t
*/
MatrixError_t CTransCode::SzStringToUnicode(codepage_t codepage, const bchar_t *pIn, wchar_t *pOut, size_t &DestinationSize)
{
	ASSERT(pIn!=NULL);
	ASSERT(pOut!=NULL);
	if(IsValidCP(codepage))
	{
		return m_TransInfo[codepage]->ToUnicode(pIn, pOut, DestinationSize);
	}
	return(ERR_UNKNOWN_CODEPAGE);
}

///////////////////////////////////////////////////////////////////////////////
//! Translate a string with length to unicode.
/*! 
 \param codepage Codepage of string
 \param *pIn  Point to the source buffer
 \param length Size of the string in the source buffer
 \param *pOut Point to the destination buffer
 \param &DestinationSize In: Size of the destination buffer, Out: used buffersize
 \return MatrixError_t
*/
MatrixError_t CTransCode::StringToUnicode(codepage_t codepage, const bchar_t *pIn, size_t length, wchar_t *pOut, size_t &DestinationSize)
{
	ASSERT(pIn!=NULL);
	ASSERT(pOut!=NULL);
	if(IsValidCP(codepage))
	{
		return m_TransInfo[codepage]->ToUnicode(pIn, length, pOut, DestinationSize);
	}
	return(ERR_UNKNOWN_CODEPAGE);
}

///////////////////////////////////////////////////////////////////////////////
//! Translate a single unicode character into a byte size character.
/*! 
 \param codepage Codepage of destination character
 \param in Source character
 \param &out Out: Destination character
 \return MatrixError_t
*/
MatrixError_t CTransCode::UnicodeToSingleByte(codepage_t codepage, wchar_t in, bchar_t &out)
{
	if(IsValidCP(codepage))
	{
		size_t DummySize=1;
		return m_TransInfo[codepage]->FromUnicode(&in, 1, &out, DummySize) ;
	}
	return(ERR_UNKNOWN_CODEPAGE);
}

///////////////////////////////////////////////////////////////////////////////
//! Translate a zero terminated unicode string to codepage characters.
/*! 
 \param codepage Codepage of destination character
 \param *pIn  Zero terminated unicode string
 \param *pOut Point to the output buffer
 \param &DestinationSize In: Size of the destination buffer, Out: used buffersize
 \return MatrixError_t
*/
MatrixError_t CTransCode::UnicodeToSzString(codepage_t codepage, const wchar_t *pIn, bchar_t *pOut, size_t &DestinationSize)
{
	ASSERT(pIn!=NULL);
	ASSERT(pOut!=NULL);
	if(IsValidCP(codepage))
	{
		return m_TransInfo[codepage]->FromUnicode(pIn, pOut, DestinationSize) ;
	}
	return(ERR_UNKNOWN_CODEPAGE);
}

///////////////////////////////////////////////////////////////////////////////
//! Translate a unicode string with size to codepage characters.
/*! 
 \param codepage Codepage of destination character
 \param *pIn  Zero terminated unicode string
 \param length Size of the string in the source buffer
 \param *pOut Point to the output buffer
 \param &DestinationSize In: Size of the destination buffer, Out: used buffersize
 \return MatrixError_t
*/
MatrixError_t CTransCode::UnicodeToString(codepage_t codepage, const wchar_t *pIn, size_t length, bchar_t *pOut, size_t &DestinationSize)
{
	ASSERT(pIn!=NULL);
	ASSERT(pOut!=NULL);
	if(IsValidCP(codepage))
	{
		return m_TransInfo[codepage]->FromUnicode(pIn, length, pOut, DestinationSize) ;
	}
	return(ERR_UNKNOWN_CODEPAGE);
}

///////////////////////////////////////////////////////////////////////////////
//! Translations from system codepage to Unicode.
/*! Used in the formatter and other legacy places.
 \param in Source character
 \param &out Out: Destination character
 \return MatrixError_t
*/
MatrixError_t CTransCode::SystemCPCharacterToUnicode(bchar_t in, wchar_t &out)
{
	if(IsValidCP(m_SystemCodepage))
	{
		size_t DummySize=1;
		return m_TransInfo[m_SystemCodepage]->ToUnicode(&in, 1, &out, DummySize);
	}
	out = m_DefaultChar;
	return(ERR_UNKNOWN_CODEPAGE);
}

// another flavour of the above
///////////////////////////////////////////////////////////////////////////////
//! Translations from system codepage to Unicode.
/*! Used in the formatter and other legacy places.
 \overload
 \param in Source character
 \return Destination character
*/
wchar_t CTransCode::SystemCPCharacterToUnicode(bchar_t in)
{
	wchar_t out = m_DefaultChar;	// default to emergency characater
	if(IsValidCP(m_SystemCodepage))
	{
		size_t DummySize=1;
		m_TransInfo[m_SystemCodepage]->ToUnicode(&in, 1, &out, DummySize);
	}
	return out;
}

///////////////////////////////////////////////////////////////////////////////
//! Translate a zero terminated string from system codepage to unicode.
/*! 
 \param *pIn  Point to the source buffer
 \param *pOut Point to the destination buffer
 \param &DestinationSize In: Size of the destination buffer, Out: used buffersize
 \return MatrixError_t
*/
MatrixError_t CTransCode::SzSystemCPStringToUnicode(const bchar_t *pIn, wchar_t *pOut, size_t &DestinationSize)
{
	ASSERT(pIn!=NULL);
	ASSERT(pOut!=NULL);
	if(IsValidCP(m_SystemCodepage))
	{
		return m_TransInfo[m_SystemCodepage]->ToUnicode(pIn, pOut, DestinationSize);
	}
	return(ERR_UNKNOWN_CODEPAGE);
}

///////////////////////////////////////////////////////////////////////////////
//! Translate a string with lenght from system codepage to unicode.
/*! 
 \param *pIn  Zero terminated unicode string
 \param length Size of the string in the source buffer
 \param *pOut Point to the output buffer
 \param &DestinationSize In: Size of the destination buffer, Out: used buffersize
 \return MatrixError_t
*/
MatrixError_t CTransCode::SystemCPStringToUnicode(const bchar_t *pIn, size_t length, wchar_t *pOut, size_t &DestinationSize)
{
	ASSERT(pIn!=NULL);
	ASSERT(pOut!=NULL);
	if(IsValidCP(m_SystemCodepage))
	{
		return m_TransInfo[m_SystemCodepage]->ToUnicode(pIn, length, pOut, DestinationSize);
	}
	return(ERR_UNKNOWN_CODEPAGE);
}

///////////////////////////////////////////////////////////////////////////////
//! Inplace translation. Only works for 7 bit codepages and system codepage == CP1252
/*! 
 \param codepage Codepage 
 \param pIn Point to the buffer 
 \param length Size of the string in the buffer 
 \return true if ok
*/
bool CTransCode::CP7ToSystemCPString(codepage_t codepage, bchar_t *pIn, size_t length)
{
	ASSERT(pIn!=NULL);
	if(IsValidCP(m_SystemCodepage))
	{
		return m_TransInfo[codepage]->TranslateInPlace(pIn, length);
	}
	return(FAIL);
}





