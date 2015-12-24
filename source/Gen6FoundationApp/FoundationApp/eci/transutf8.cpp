/*=================================================================================
  A helper for CTransCode to support UTF8
//=================================================================================
   $Source: Gen6FoundationApp/FoundationApp/eci/transutf8.cpp $
   $Date: 2011/01/25 08:28:03EST $
   $Revision: 1.1.1.1 $


//=================================================================================*/
//! \file

#include "stdInclude.h"
#include "TransCode.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uchar.h"
#include "wchar.h"
#include "ConvertUTF.h"		// interface to the official converter from unicode.org

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
/// Handler for the UTF8 translation.
/*!
*/
class CTransInfoUTF8 : public CTransInfoBase
{
public:
	CTransInfoUTF8(codepage_t codepage);
protected:
	virtual MatrixError_t FromUnicode(const wchar_t *pIn, bchar_t *pOut, size_t &DestinationSize);
	virtual MatrixError_t FromUnicode(const wchar_t *pIn, size_t length, bchar_t *pOut, size_t &DestinationSize);
	virtual MatrixError_t ToUnicode(const bchar_t *pIn, size_t length, wchar_t *pOut, size_t &DestinationSize);
	virtual MatrixError_t ToUnicode(const bchar_t *pIn, wchar_t *pOut, size_t &DestinationSize);
	MatrixError_t MakeMatrixError(ConversionResult Result);
};

CTransInfoUTF8::CTransInfoUTF8(codepage_t codepage)
: CTransInfoBase(0, codepage)
{
	m_capabilities = FL_MULTIBYTE;
}

MatrixError_t CTransInfoUTF8::MakeMatrixError(ConversionResult Result)
{
	switch(Result)
	{
		case conversionOK:
			return ERR_PASS;
		//break;
		case targetExhausted:
			return ERR_INSUFFIZIENT_BUFFER;
		//break;
		case sourceExhausted:
		case sourceIllegal:
		default:
			//return ERR_BAD_TRANSLATION;
		break;
	}
	return ERR_BAD_TRANSLATION;
}

// CAUTION: output buffer must be big enough. UTF8 can be 4x bigger than UTF16!
MatrixError_t CTransInfoUTF8::FromUnicode(const wchar_t *pIn, size_t length, bchar_t *pOut, size_t &DestinationSize)
{
	const UTF16* sourceStart = (const UTF16*) pIn;
	const UTF16* sourceEnd = (const UTF16*) (pIn+length);
	UTF8* targetStart = pOut;
	UTF8* targetEnd = pOut+DestinationSize;
	ConversionResult Result = ConvertUTF16toUTF8 ( &sourceStart, sourceEnd, &targetStart, targetEnd, strictConversion);
	DestinationSize = targetStart-pOut;    // targetStart gets updated by the conversion function
	return MakeMatrixError(Result);
}


MatrixError_t CTransInfoUTF8::FromUnicode(const wchar_t *pIn, bchar_t *pOut, size_t &DestinationSize)
{
	return FromUnicode(pIn, wcslen(pIn), pOut, DestinationSize);
}

MatrixError_t CTransInfoUTF8::ToUnicode(const bchar_t *pIn, size_t length, wchar_t *pOut, size_t &DestinationSize)
{
	const bchar_t* sourceStart = pIn;
	const bchar_t* sourceEnd = pIn+length;
	UTF16* targetStart = (UTF16*) pOut;
	UTF16* targetEnd = (UTF16*) (pOut+DestinationSize);
	ConversionResult Result = ConvertUTF8toUTF16 ( &sourceStart, sourceEnd, &targetStart, targetEnd, strictConversion);
	DestinationSize = ((wchar_t*)targetStart)-pOut;    // targetStart gets updated by the conversion function

	return MakeMatrixError(Result);
}

MatrixError_t CTransInfoUTF8::ToUnicode(const bchar_t *pIn, wchar_t *pOut, size_t &DestinationSize)
{
	return ToUnicode(pIn, strlen(pIn), pOut, DestinationSize);
}

///////////////////////////////////////////////////////////////////////////////
//! Create the UTF8 handler.
/*!
*/
void	CTransCode::CreateUTF8Translator(void)
{
	ASSERT(m_TransInfo[ISO_UTF8] == NULL);
	m_TransInfo[ISO_UTF8] = new CTransInfoUTF8(ISO_UTF8);
}





