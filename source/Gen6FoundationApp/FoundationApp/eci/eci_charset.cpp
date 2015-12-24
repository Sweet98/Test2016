/* ==============================================================================
   ECI character set Module Source
   ==============================================================================

   $RCSfile: Gen6FoundationApp/FoundationApp/eci/eci_charset.cpp $
   $Revision: 1.1 $
   $Date: 2009/03/05 14:45:53EST $

   ==============================================================================

   ============================================================================== */
//! \file

#include "stdInclude.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "uchar.h"
#include "Eci_Charset.h"
#include "TransCodeEx.h"

//This class does all the parsing and translations for the various character sets.

///////////////////////////////////////////////////////////////////////////////////////////////////
// Define the ECI numbers
// CAUTION: These ECI numbers are not released by the working group yet.
// They are subject to change (Dieter Fauth, April 1999).

// ISO/IEC 15438 Bar code symbology specification-PDF417:
//		Default character set to 1994 specification with GLI rules
#define CHARSET_437_PDF			0
// ISO/IEC 15438 Bar code symbology specification-PDF417:
//		Latin 1 character set to 1994 specification with GLI rules
#define CHARSET_8859_1_PDF		1
// ISO/IEC 15438 Bar code symbology specification-PDF417:
//		Default character set with ECI rules
#define CHARSET_437				2

#define CHARSET_8859_1			3	// Latin alphabet No. 1
#define CHARSET_8859_2			4	// Latin alphabet No. 2
#define CHARSET_8859_3			5	// Latin alphabet No. 3
#define CHARSET_8859_4			6	// Latin alphabet No. 4
#define CHARSET_8859_5			7	// Latin/Cyrillic alphabet
#define CHARSET_8859_6			8	// Latin/Arabic alphabet
#define CHARSET_8859_7			9	// Latin/Greek alphabet
#define CHARSET_8859_8			10	// Latin/Hebrew alphabet
#define CHARSET_8859_9			11	// Latin alphabet No. 5
#define CHARSET_8859_10			12	// Latin alphabet No. 6
#define CHARSET_8859_11			13	// Latin/Thai alphabet
// 8859_12 skipped (14)
#define CHARSET_8859_13			15	// ISO/IEC 8859-13 Latin alphabet No. 7 (Baltic Rim)
#define CHARSET_8859_14			16	// ISO/IEC 8859-14 Latin alphabet No. 8 (Celtic)
#define CHARSET_8859_15			17	// ISO/IEC 8859-15 Latin alphabet No. 9
#define CHARSET_8859_16			18	//	ISO/IEC 8859-16 Latin alphabet No. 10
// 000019	Reserved

#define CHARSET_SHIFT_JIS		20	//	Shift JIS (JIS X 0208 Annex 1 + JIS X 0201)

#define CHARSET_1250				21	//	Windows 1250 Latin 2 (Central Europe)
#define CHARSET_1251				22	//	Windows 1251 Cyrillic
#define CHARSET_1252				23	//	Windows 1252 Latin 1
#define CHARSET_1256				24	//	Windows 1256 Arabic

#define CHARSET_UCS2				25	//	ISO/IEC 10646 UCS-2 (High order octet first)
#define CHARSET_UTF8				26	//	ISO/IEC 10646 UTF- 8

#define CHARSET_ISO646			27	//	ISO/IEC 646:1991 International Reference Version of ISO 7-bit coded character set
#define CHARSET_ISO646I		  170	// ISO/IEC 646 : ISO 7-bit coded character set  - Invariant Characters
#define CHARSET_BINARY		  899	// Null Interpretative ECI

// missing and not likely to be added...
#if(0)
#define CHARSET_1253
#define CHARSET_1254
#define CHARSET_1255
#define CHARSET_1257

#define CHARSET_737
#define CHARSET_775
#define CHARSET_850
#define CHARSET_852
#define CHARSET_857
#define CHARSET_866
#define CHARSET_862
#define CHARSET_864

#define CHARSET_KOI8_R
#define CHARSET_KOI8_U
#endif

#define CHARSET_TABLE_END		900					/* 900 is the first non charset eci */


/////////////////////
#ifdef _DEBUGx

typedef struct tag_ECI_CP_NAMES
{
	unsigned short charset_eci;
	 char *name;
}ECI_CP_NAMES;

ECI_CP_NAMES Nametable[] =
{
	CHARSET_437_PDF	, "CHARSET_437_PDF"		,
	CHARSET_8859_1_PDF, "CHARSET_8859_1_PDF"	,
	CHARSET_437			, "CHARSET_437"			,
	CHARSET_8859_1		, "CHARSET_8859_1"		,
	CHARSET_8859_2		, "CHARSET_8859_2"		,
	CHARSET_8859_3		, "CHARSET_8859_3"		,
	CHARSET_8859_4		, "CHARSET_8859_4"		,
	CHARSET_8859_5		, "CHARSET_8859_5"		,
	CHARSET_8859_6		, "CHARSET_8859_6"		,
	CHARSET_8859_7		, "CHARSET_8859_7"		,
	CHARSET_8859_8		, "CHARSET_8859_8"		,
	CHARSET_8859_9		, "CHARSET_8859_9"		,
	CHARSET_8859_10	, "CHARSET_8859_10"		,
	CHARSET_8859_11	, "CHARSET_8859_11"		,
	CHARSET_8859_13	, "CHARSET_8859_13"		,
	CHARSET_8859_14	, "CHARSET_8859_14"		,
	CHARSET_8859_15	, "CHARSET_8859_15"		,
	CHARSET_1250		, "CHARSET_1250"			,
	CHARSET_1251		, "CHARSET_1251"			,
	CHARSET_1252		, "CHARSET_1252"			,
	CHARSET_1253		, "CHARSET_1253"			,
	CHARSET_1254		, "CHARSET_1254"			,
	CHARSET_1255		, "CHARSET_1255"			,
	CHARSET_1256		, "CHARSET_1256"			,
	CHARSET_1257		, "CHARSET_1257"			,
	CHARSET_737			, "CHARSET_737"			,
	CHARSET_775			, "CHARSET_775"		,
	CHARSET_850			, "CHARSET_850"			,
	CHARSET_852			, "CHARSET_852"			,
	CHARSET_857			, "CHARSET_857"			,
	CHARSET_866			, "CHARSET_866"			,
	CHARSET_862			, "CHARSET_862"			,
	CHARSET_864			, "CHARSET_864"			,
	CHARSET_ISO646		, "CHARSET_ISO646"		,
	CHARSET_KOI8_R		, "CHARSET_KOI8_R"		,
	CHARSET_KOI8_U		, "CHARSET_KOI8_U"		,
	CHARSET_BINARY		, "CHARSET_BINARY",
	CHARSET_UTF8		, "CHARSET_UTF8",
	CHARSET_UCS2		, "CHARSET_UTF16BE",
	CHARSET_TABLE_END	,""
};

void MakeHumanReadableTable(void)
{
	const ECI_CP_NAMES *pTable = Nametable;
	while (pTable->charset_eci != CHARSET_TABLE_END)
	{
		printf("\t%s\t%3i\n",pTable->name, pTable->charset_eci);
		pTable++;
	}
}

#endif
/////////////////////



struct ECI_CP
{
	UINT charset_eci;
	codepage_t codepage;
};

const ECI_CP ECItoCodepage[] =
{
	CHARSET_BINARY		,	CP_BINARY,
	CHARSET_437_PDF	,	CP437,
	CHARSET_8859_1_PDF,	ISO_8859_1,
	CHARSET_437			,	CP437,
	CHARSET_8859_1		,	ISO_8859_1,
	CHARSET_8859_2		,	ISO_8859_2,
	CHARSET_8859_3		,	ISO_8859_3,
	CHARSET_8859_4		,	ISO_8859_4,
	CHARSET_8859_5		,	ISO_8859_5,
	CHARSET_8859_6		,	ISO_8859_6,
	CHARSET_8859_7		,	ISO_8859_7,
	CHARSET_8859_8		,	ISO_8859_8,
	CHARSET_8859_9		,	ISO_8859_9,
	CHARSET_8859_10	,	ISO_8859_10,
	CHARSET_8859_11	,	ISO_8859_11,
	CHARSET_8859_13	,	ISO_8859_13,
	CHARSET_8859_14	,	ISO_8859_14,
	CHARSET_8859_15	,	ISO_8859_15,
	CHARSET_8859_16	,	ISO_8859_16,
	CHARSET_1250		,	CP1250,
	CHARSET_1251		,	CP1251,
	CHARSET_1252		,	CP1252,
//	CHARSET_1253		,	CP1253,
//	CHARSET_1254		,	CP1254,
//	CHARSET_1255		,	CP1255,
	CHARSET_1256		,	CP1256,
//	CHARSET_1257		,	CP1257,
	CHARSET_UTF8		,	ISO_UTF8,
	CHARSET_UCS2		,	ISO_UTF16BE,
#if(0)
	CHARSET_737			,	CP737,
	CHARSET_775			,	CP775,
	CHARSET_850			,	CP850,
	CHARSET_852			,	CP852,
	CHARSET_857			,	CP857,
	CHARSET_866			,	CP866,
	CHARSET_862			,	CP862,
	CHARSET_864			,	CP864,
	CHARSET_ISO646		,	CP_ISO646,
	CHARSET_KOI8_R		,	CP_KOI8_R,
	CHARSET_KOI8_U		,	CP_KOI8_U,
#endif
	// The end marker
	CHARSET_TABLE_END,	CP_NOTSUPPORTED
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CEciCharSet::CEciCharSet(void)
: CEciBase()
{
	m_CurrentCodepage = CP_BINARY;
	m_DefaultCodepage = CP_BINARY;
}

CEciCharSet::~CEciCharSet(void)
{
}

///////////////////////////////////////////////////////////////////////////////
// Translate a ECI number into a codepage
codepage_t CEciCharSet::GetCodePageFromEci(UINT eci)
{
	const ECI_CP *pTable = ECItoCodepage;
	while (pTable->charset_eci != CHARSET_TABLE_END)
	{
		if (pTable->charset_eci == eci)
		{
			break;
		}
		else
		{
			pTable++;
		}
	}
	return pTable->codepage;
}

///////////////////////////////////////////////////////////////////////////////
//
bool CEciCharSet::HandleECINumber(UINT nECI)
{
	bool ret=false;
	if (nECI < 900)
	{
		ret = true;
		m_CurrentCodepage = GetCodePageFromEci(nECI);
		if(m_CurrentCodepage == CP_NOTSUPPORTED)	// fall back if not supported
		{
			m_CurrentCodepage = m_DefaultCodepage;
			ret = false;
		}
	}
	return ret;
}

///////////////////////////////////////////////////////////////////////////////
// helper for the menu system
bool CEciCharSet::ValidateCodePage(int codepage)
{
	return theTransCode.ValidateCodePage(codepage);
}

///////////////////////////////////////////////////////////////////////////////
// helper for the menu system
int CEciCharSet::CollectAllCodePages(int *pDst, int BufferSize)
{
	return theTransCode.CollectAllCodePages(pDst, BufferSize);
}

///////////////////////////////////////////////////////////////////////////////
//
// Called with pIn pointing to "\C123...C"
size_t CEciCharSet::ParseUninterpretedCodeWord(const bchar_t *pIn, size_t size)
{
	size_t SkipSize;
	if(size > 3)
		SkipSize = 2 + ParseEndOfECI(pIn+2, size-2, pIn[1]);
	else
		SkipSize = size;
	return SkipSize;
}

///////////////////////////////////////////////////////////////////////////////
//
// Called with pIn pointing to "\M...\"
size_t CEciCharSet::ParseMacro(const bchar_t *pIn, size_t size)
{
	size_t SkipSize = ParseEndOfECI(pIn+1, size-1, '\\');
	if(SkipSize == (size-1))
	{ // special case at the end of the barcode
		SkipSize++;
	}
	return SkipSize;
}

///////////////////////////////////////////////////////////////////////////////
//
// Called with pIn pointing to "\123456"
size_t CEciCharSet::ParseECICode(const bchar_t *pIn, size_t size)
{
	const size_t ECISize = 6+1;
	size_t SkipSize=ECISize;
	if(size < ECISize)
	{
		SkipSize = size;
	}
	else
	{	// valid ECI? not 100% sure yet, neet to parse it as number
		int i;
		for(i=1; i<ECISize; i++)
		{
			if (!isdigit(pIn[i]))
			{
				break;
			}
		}
		if(i==ECISize)
		{
			char Buffer[ECISize+1];	// only copy the digits
			memcpy(Buffer, &pIn[1], ECISize-1);
			Buffer[ECISize-1] = 0;
			int eci = atoi(Buffer);
			HandleECINumber(eci);
		}
	}

	return SkipSize;
}

///////////////////////////////////////////////////////////////////////////////
//
// Called with pIn pointing to the '\'
// An ECi can be:
// "\123456"
// "\C123C"
// "\\"
// more?
bool CEciCharSet::IsDataBackslash(const bchar_t *pIn, size_t size)
{
	bool ret=false;
	// sanity first
	if(size >= 2)
	{
		if (pIn[1] == '\\')
		{	// the \ as data, copy to destination
			ret=true;
		}
	}
	return ret;
}

///////////////////////////////////////////////////////////////////////////////
//
// Called with pIn pointing to the '\'
// An ECi can be:
// "\123456"
// "\C123C"
// "\\"
// more?
size_t CEciCharSet::ParseForECI(const bchar_t *pIn, size_t size)
{
	size_t SkipSize = 0;
	// sanity first
	if(size < 2)
	{
		SkipSize = size;
	}
	else
	{
		if (pIn[1] == '\\')
		{	// the \ as data, copy to destination
			SkipSize = 2;
		}
		else if (pIn[1] == 'C')
		{
			SkipSize =  ParseUninterpretedCodeWord(pIn, size);
		}
		else if (pIn[1] == 'M')
		{
			SkipSize =  ParseMacro(pIn, size);
		}
		else
		{
			SkipSize =  ParseECICode(pIn, size);
		}
	}
	return SkipSize;
}

///////////////////////////////////////////////////////////////////////////////
//

size_t CEciCharSet::ParseAndTranslateCharSetECI(CBarcodeData *pSource, tichar_t *pOutput)
{
	bool bDataBackslash=false;
	bool bCP16Bit=false;
	size_t SkipSize=0;
	tichar_t *pStartOfOutputBuffer = pOutput;
	const bchar_t *pInput;
	const bchar_t *pNextSearch;		// points after the ECI or '\'
	const bchar_t *pCurrent;
	size_t RestSize;
	pSource->GetTotalData(pInput, RestSize);
	pNextSearch = pInput;
	size_t ReturnedSize;

	// now parse and find all ECI codes
	while(RestSize > 0)
	{
		// this loop jumps from one '\' to the next
		pCurrent = SearchForNextBackslash(pNextSearch, RestSize);
		if(pCurrent != NULL)
		{
			pNextSearch = pCurrent;
			bDataBackslash = IsDataBackslash(pCurrent, RestSize);
			if (bDataBackslash)
			{
				bCP16Bit = IsUTF16();	// 16bit needs special care with the double backslash
				if(bCP16Bit)
				{
					int SegmentLenght=pCurrent-pInput;
					if(SegmentLenght&1)	// second byte in 16 bit?
					{
						pCurrent++;	// add the first '\' to the proccessed part
					}
				}
			}
			// translate what we have got so far
			size_t PartialSize = pCurrent-pInput;
			TranslateFromCPToSystemCP(pInput, pOutput, PartialSize, ReturnedSize);
			pInput += PartialSize;
			pOutput += ReturnedSize;
			RestSize -= PartialSize;

			if (bDataBackslash)
			{
				SkipSize=1;	// skip a '\', either the first one (8bit,16bit-first '\') or the second one (16bit-second '\')
				if(IsShowECI() & !bCP16Bit)
				{
					*pOutput++ = '\\';
				}
				pNextSearch+=2;			// ensure we do not catch our current '\'
			}
			else
			{
				// try to read the ECI number
				SkipSize = ParseForECI(pInput, RestSize);
				if(IsShowECI())
				{
					// keep ECI in data stream
					TranslateISO646IToSystemCP(pInput, pOutput, SkipSize);
					pOutput += SkipSize;
				}
				pNextSearch += SkipSize;
			}

			pInput += SkipSize;
			RestSize -= SkipSize;
		}
		else
		{
			break;	// done
		}
	};

	// don't forget the last chunk!!
	TranslateFromCPToSystemCP(pInput, pOutput, RestSize, ReturnedSize);
	// calculate the final size
	size_t FinalSize = pOutput-pStartOfOutputBuffer+ReturnedSize;
	return FinalSize;
}

///////////////////////////////////////////////////////////////////////////////
//
MatrixError_t CEciCharSet::TranslateToUnicode(const bchar_t *pIn, wchar_t *pOut, size_t size, size_t &ReturnedSize)
{
	MatrixError_t RetVal = ERR_PASS;
	ReturnedSize=size;
	RetVal = theTransCode.StringToUnicode(m_CurrentCodepage, pIn, size, pOut, ReturnedSize);
	if (!IsOK(RetVal))
	{
		// Translation failed. Probably because of a non supported codepage.
		// This should never happen. We handle it gratefully.
		ReturnedSize=size;
		RetVal = theTransCode.StringToUnicode(CP_BINARY, pIn, size, pOut, ReturnedSize);
		if (!IsOK(RetVal))
		{
			// PANIC!!!
//			m_Status |= ECIC_BAD_CHARSET;
		}
	}
	return RetVal;
}

///////////////////////////////////////////////////////////////////////////////
//
MatrixError_t CEciCharSet::TranslateFromUnicodeToSystemCP(const wchar_t *pIn, bchar_t *pOut, size_t size)
{
	MatrixError_t RetVal = ERR_PASS;
	RetVal = theTransCode.UnicodeToSystemCPString(pIn, size, pOut, size);
//	if (!IsOK(RetVal))
//	{
		// Translation failed. Probably because of a non supported codepage.
		// This should never happen. We handle it gratefully.
		// PANIC!!!
//		m_Status |= ECIC_BAD_CHARSET;
//	}
	return RetVal;
}

///////////////////////////////////////////////////////////////////////////////
//
MatrixError_t CEciCharSet::TranslateFromCPToSystemCP(const bchar_t *pIn, tichar_t *pOut, size_t size, size_t &ReturnedSize)
{
	MatrixError_t RetVal = ERR_PASS;
#ifndef UNICODE_FOR_INTERFACES
	wchar_t *pTemp;
	size_t NumUnicode = size;
	CBarcodeData *pDestObj = CBarcodeData::GetSaveNewBuffer(pTemp, NumUnicode);
	pDestObj->SetUnicodeData();
	pDestObj->FinalizeBuffer(size);
	RetVal = TranslateToUnicode(pIn, pTemp, size, ReturnedSize);
	if (IsOK(RetVal))
	{
		RetVal = TranslateFromUnicodeToSystemCP(pTemp, pOut, size);
	}
#else
	RetVal = TranslateToUnicode(pIn, pOut, size, ReturnedSize);
#endif
	return RetVal;
}

///////////////////////////////////////////////////////////////////////////////
// Copy the invariant ASCII  characters to the destination
MatrixError_t CEciCharSet::TranslateISO646IToSystemCP(const bchar_t *pIn, tichar_t *pOut, size_t size)
{
	MatrixError_t RetVal = ERR_PASS;
	for(size_t i=0; i<size; i++)
	{
		*pOut++ = *pIn++;
	}

	return RetVal;
}

///////////////////////////////////////////////////////////////////////////////
//
void CEciCharSet::TranslateCodepage(CBarcodeData *pLabel)
{
#ifndef UNICODE_FOR_INTERFACES
	bool bTranslate=true;
	if (m_CurrentCodepage == theTransCode.GetSystemCodepage())
		bTranslate=false;

	if(m_CurrentCodepage == CP_BINARY)
		bTranslate=false;

	if ( CP1252 == theTransCode.GetSystemCodepage() && (m_CurrentCodepage == ISO_8859_1) )
		bTranslate=false;

	if(bTranslate)
	{
		if( theTransCode.Is7Bit(m_CurrentCodepage) && (CP1252 == theTransCode.GetSystemCodepage()) )
		{	// this is faster, so we try to use it (works for USA and most European countries)
			TranslateFrom7BitToSystem(pLabel);
		}
		else
		{	// the slow, but save way
			TranslateToUnicode(pLabel, m_CurrentCodepage);
			TranslateFromUnicodeToSystemCP(pLabel);
		}
	}
	else
	{
		pLabel->SetSystemCodepageData();
	}
#else
	TranslateToUnicode(pLabel, m_CurrentCodepage);
#endif
}

///////////////////////////////////////////////////////////////////////////////
//
void CEciCharSet::TranslateToUnicode(CBarcodeData *pLabel, codepage_t CurrentCodepage)
{
	theTransCode.TranslateToUnicode(pLabel, CurrentCodepage);
}

///////////////////////////////////////////////////////////////////////////////
//
void CEciCharSet::TranslateFromUnicodeToSystemCP(CBarcodeData *pLabel)
{
	theTransCode.TranslateFromUnicodeToSystemCP(pLabel);
}

#ifndef UNICODE_FOR_INTERFACES

///////////////////////////////////////////////////////////////////////////////
//
void CEciCharSet::TranslateFrom7BitToSystem(CBarcodeData *pLabel)
{
	bchar_t *pIn;
	size_t size;
	if (PASS == pLabel->GetContentsBuffer(pIn, size))
	{
		if (!theTransCode.CP7ToSystemCPString(m_CurrentCodepage, pIn, size))
		{
			// Translation failed. Probably because of a non supported codepage.
			// This should never happen.
			// PANIC!!!
		}

		pLabel->SetSystemCodepageData();
	}
	else
	{
		// error situation
		// we do nothing and let the situation be handled by a later call.
	}
}

#endif


