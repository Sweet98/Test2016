/*=================================================================================
  A helper for CTransCode to support CP936
//=================================================================================
   $Source: Gen6FoundationApp/FoundationApp/eci/transcp936.cpp $
//=================================================================================*/
//! \file

#include "stdInclude.h"
#include "TransCode.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uchar.h"
#include "wchar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern const wchar_t Table_CP936[];

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
/// Handler for the CP936 translation.
/*!
*/
class CTransInfoCP936 : public CTransInfoBase
{
public:
	CTransInfoCP936(codepage_t codepage);
	virtual ~CTransInfoCP936();
protected:
	virtual MatrixError_t FromUnicode(const wchar_t *pIn, bchar_t *pOut, size_t &DestinationSize);
	virtual MatrixError_t FromUnicode(const wchar_t *pIn, size_t length, bchar_t *pOut, size_t &DestinationSize);
	virtual MatrixError_t ToUnicode(const bchar_t *pIn, size_t length, wchar_t *pOut, size_t &DestinationSize);
	virtual MatrixError_t ToUnicode(const bchar_t *pIn, wchar_t *pOut, size_t &DestinationSize);
//	size_t SearchInTable(wchar_t search, wchar_t start, wchar_t end);
	wchar_t *m_pBackTable;
	enum
	{
		SizeTable=0x10000
	};
};

CTransInfoCP936::CTransInfoCP936(codepage_t codepage)
: CTransInfoBase(0, codepage)
, m_pBackTable(NULL)
{
	m_capabilities = FL_MULTIBYTE;
	m_pBackTable = new wchar_t[SizeTable+4];
	memset(m_pBackTable, 0, sizeof(wchar_t)*SizeTable);
	// This CP uses some double mapped characters, so a round trip is not guarantied.
	// Therefore we fill the table from the top to get the same behaviour as WideCharToMultiByte in Win32.
	// That makes testing easier.
	// We still get no 100% compared to WideCharToMultiByte on XP, I worked around by making my test tool smarter.
	for(int i=SizeTable-1; i>=0; i--)
	{
		size_t Unicode = Table_CP936[i];
		m_pBackTable[Unicode] = (wchar_t) i;
	}
}

CTransInfoCP936::~CTransInfoCP936()
{
	delete m_pBackTable;
}

// Below is an atempt to create a memory saving Unicode to CP936 translation (slow).
// While writing it I decided to use the table version. I keep the code here for reference.
//size_t CTransInfoCP936::SearchInTable(wchar_t search, wchar_t start, wchar_t end)
//{
//	size_t Index=0;
//	for(size_t i=start; i<=end; i++)
//	{
//		if(search==Table_CP936[i])
//		{
//			Index=i;
//			break;
//		}
//	}
//	return Index;
//}
// CAUTION: output buffer must be big enough. CP936 can be 2x bigger than UTF16!
//MatrixError_t CTransInfoCP936::FromUnicode(const wchar_t *pIn, size_t length, bchar_t *pOut, size_t &DestinationSize)
//{
//	MatrixError_t RetVal=ERR_PASS;
//	size_t NumBytes = 0;
//	bchar_t byte1=0;
//	bchar_t byte2=0;
//	size_t Index=0;
//
//	for(size_t i=0; i<length; i++)
//	{
//		wchar_t c=*pIn++;
//		if(c<=0x7F)
//		{
//			byte1=(bchar_t)c;
//			byte2=0;
//		}
//		else if ((c>=0xFF61) && (c<=0xFF9F))
//		{
//			Index = SearchInTable(c, 0xA1, 0xDF);
//			byte1=(bchar_t)Index;
//			byte2=0;
//		}
//		else if ((c>=0xFF61) && (c<=0xFF9F))
//		{
//			Index = SearchInTable(c, 0xA1, 0xDF);
//			byte1=(bchar_t)Index;
//			byte2=0;
//		}
//
//		NumBytes++;
//		if(NumBytes=>DestinationSize)
//		{
//			NumBytes--;
//			RetVal = ERR_INSUFFIZIENT_BUFFER;
//			break;
//		}
//		*pOut++ = byte1;
//		if(byte2>0)
//		{
//			NumBytes++;
//			if(NumBytes=>DestinationSize)
//			{
//				NumBytes--;
//				RetVal = ERR_INSUFFIZIENT_BUFFER;
//				break;
//			}
//			*pOut++ = byte2;
//		}
//	}
//
//
//	DestinationSize = NumBytes;
//	return RetVal;
//}


// CAUTION: output buffer must be big enough. CP936 can be 2x bigger than UTF16!
MatrixError_t CTransInfoCP936::FromUnicode(const wchar_t *pIn, size_t length, bchar_t *pOut, size_t &DestinationSize)
{
	ASSERT(m_pBackTable!=NULL);
	MatrixError_t RetVal=ERR_PASS;
	size_t NumBytes = 0;
	bchar_t TrailByte=0;
	bchar_t LeadByte=0;

	for(size_t i=0; i<length; i++)
	{
		wchar_t c=*pIn++;
		if(c!=0xFFFF)
		{
			LeadByte = (bchar_t) (m_pBackTable[c] >> 8);
			TrailByte = (bchar_t) (m_pBackTable[c] & 0xFF);

			if(LeadByte>0)
			{
				NumBytes++;
				if(NumBytes>=DestinationSize)
				{
					RetVal = ERR_INSUFFIZIENT_BUFFER;
					NumBytes = length*2;				//!< return the amount of buffer we would need
					break;
				}
				*pOut++ = LeadByte;
			}

			NumBytes++;
			if(NumBytes>DestinationSize)
			{
				RetVal = ERR_INSUFFIZIENT_BUFFER;
				NumBytes = length*2;				  //!< return the amount of buffer we would need
				break;
			}
			*pOut++ = TrailByte;
		}
	}


	DestinationSize = NumBytes;
	return RetVal;
}

MatrixError_t CTransInfoCP936::FromUnicode(const wchar_t *pIn, bchar_t *pOut, size_t &DestinationSize)
{
	return FromUnicode(pIn, wcslen(pIn), pOut, DestinationSize);
}

MatrixError_t CTransInfoCP936::ToUnicode(const bchar_t *pIn, size_t length, wchar_t *pOut, size_t &DestinationSize)
{
	MatrixError_t RetVal=ERR_PASS;
	size_t Index=0;
	size_t NumCharacters = 0;
	for(size_t i=0; i<length; i++)
	{
		bchar_t c=*pIn++;
		//if ( ((c>=0x81) && (c<=0x9F)) || ((c>=0xE0) && (c<=0xFC)) )
		if ( Table_CP936[c] == 0xFFFF )
		{
			i++;
			if(i==length)
				break;	// malformed char at very last position, do not read past end of buffer
			bchar_t c2=*pIn++;
			Index = c*256 + c2;
		}
		else
		{
			Index = c;
		}
		NumCharacters++;
		if(NumCharacters>=DestinationSize)
		{
			NumCharacters--;
			RetVal = ERR_INSUFFIZIENT_BUFFER;
			break;
		}
		*pOut++ = Table_CP936[Index];
	}

	DestinationSize = NumCharacters;
	return RetVal;
}

MatrixError_t CTransInfoCP936::ToUnicode(const bchar_t *pIn, wchar_t *pOut, size_t &DestinationSize)
{
	return ToUnicode(pIn, strlen(pIn), pOut, DestinationSize);
}

///////////////////////////////////////////////////////////////////////////////
//! Create the CP936 handler.
/*!
*/
void	CTransCode::CreateCp936Translator(void)
{
	const int codepage = CP936;
	if(m_TransInfo[codepage] == NULL)	// only create if not done yet (memory leak!)
	{
		m_TransInfo[codepage] = new CTransInfoCP936(codepage);
	}
}
