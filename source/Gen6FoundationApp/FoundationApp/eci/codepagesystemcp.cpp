/*=================================================================================
	Translation tables for 8 bit Codepages into Unicode
	Based on the file cpbase.cpp used by codemap.exe
//=================================================================================
   $Source: Gen6FoundationApp/FoundationApp/eci/codepagesystemcp.cpp $
   $Date: 2009/03/05 14:45:46EST $
   $Revision:


//=================================================================================*/
//! \file
/*
	I removed a few less important codepages by conditional compiling to save a few bytes.
*/
#include "stdInclude.h"
#include "TransCode.h"
#include "CodePageSystemCP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct FuzzyTranslation
{
	wchar_t	Unicode;
	bchar_t	Ascii;
};

// Some of the Unicode characters can not be tranlated back to the system codepage.
// So we do our best.
static const FuzzyTranslation FuzzyTable[] =
{
	// mostly block graphics from CP437
	0x2500,	'-',
	0x2502,	'¦',
	0x250C,	'+',
	0x2510,	'+',
	0x2514,	'+',
	0x2518,	'+',
	0x251C,	'+',
	0x2524,	'¦',
	0x252C,	'-',
	0x2534,	'-',
	0x253C,	'+',
	0x2550,	'-',
	0x2551,	'¦',
	0x2552,	'+',
	0x2553,	'+',
	0x2554,	'+',
	0x2555,	'+',
	0x2556,	'+',
	0x2557,	'+',
	0x2558,	'+',
	0x2559,	'+',
	0x255A,	'+',
	0x255B,	'+',
	0x255C,	'+',
	0x255D,	'+',
	0x255E,	'¦',
	0x255F,	'¦',
	0x2560,	'¦',
	0x2561,	'¦',
	0x2562,	'¦',
	0x2563,	'¦',
	0x2564,	'-',
	0x2565,	'-',
	0x2566,	'-',
	0x2567,	'-',
	0x2568,	'-',
	0x2569,	'-',
	0x256A,	'+',
	0x256B,	'+',
	0x256C,	'+',
	0x2580,	'¯',
	0x2584,	'_',
	0x2588,	'¦',
	0x258C,	'¦',
	0x2590,	'¦',
	0x2591,	'¦',
	0x2592,	'¦',
	0x2593,	'¦',
	0,0
};

CTransInfoSystemCP::CTransInfoSystemCP(codepage_t codepage, const wchar_t *pSrc, size_t SrcTableSize)
: CTransInfoBase(CP8_SIZE, codepage)
{
	m_capabilities = FL_8Bit;
	wchar_t	*pDest = m_pTable;
	int i;

	if(SrcTableSize == CP8_SIZE)           // exotic codepage with differences even in the ASCII range
	{
		for(i=0; i<CP8_SIZE; i++)
		{
			pDest[i] = pSrc[i];
		}
	}
	else
	{
		// fill lower half with 1:1 translation
		for(i=0; i<CP7_SIZE; i++)
		{
			pDest[i] = (wchar_t)i;
		}
		// then fill in the rest from the table in ROM
		for(int j=0; i<CP8_SIZE; i++,j++)
		{
			pDest[i] = pSrc[j];
		}
	}

	// back translation table
	int k;
	for(k=0; k<CP8_SIZE; k++)				// This improves behaviour if we have binaries by accident
	{
		m_ToSystemCP_Table[k] = k;
	}

	for(k=CP8_SIZE; k<CP16_SIZE; k++)	// we don't need to touch the first 256 bytes
	{
		m_ToSystemCP_Table[k] = '?';
	}

	for(k=0; k<(sizeof(FuzzyTable)/sizeof(FuzzyTable[0])); k++)
	{
		m_ToSystemCP_Table[FuzzyTable[k].Unicode] = FuzzyTable[k].Ascii;
	}

	for(k=0; k<CP8_SIZE; k++)
	{
		m_ToSystemCP_Table[pDest[k]] = k;
	}
}

void CTransInfoSystemCP::FromUnicode_(wchar_t In, bchar_t &Out)
{
	Out = m_ToSystemCP_Table[In];
}

MatrixError_t CTransInfoSystemCP::FromUnicode(const wchar_t *pIn, bchar_t *pOut, size_t &DestinationSize)
{
	bchar_t *pStart=pOut;
	while(*pIn != 0)
	{
		if (DestinationSize-- == 0)
			return ERR_INSUFFIZIENT_BUFFER;
		FromUnicode_(*pIn++,*pOut++);
	};
	DestinationSize = pOut-pStart;
	return ERR_PASS;
}

MatrixError_t CTransInfoSystemCP::FromUnicode(const wchar_t *pIn, size_t length, bchar_t *pOut, size_t &DestinationSize)
{
	if(DestinationSize < length)
		return ERR_INSUFFIZIENT_BUFFER;
	DestinationSize = length;	            // return final size
	for (size_t i=0; i<length; i++)
	{
		FromUnicode_(*pIn++,*pOut++);
	};
	return ERR_PASS;
}



