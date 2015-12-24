/*=================================================================================
  A helper for CTransCode to support UTF16
//=================================================================================
   $Source: Gen6FoundationApp/FoundationApp/eci/transutf16.cpp $
   $Date: 2009/03/05 14:45:56EST $
   $Revision: 1.1 $


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

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
/// Handler for the UTF16BE translation.
/*! We translate a byte stream with Big Endian Unicode into real Little Endian Unicode
*/
class CTransInfoUTF16BE : public CTransInfoBase
{
public:
	CTransInfoUTF16BE(codepage_t codepage);
protected:
	virtual MatrixError_t FromUnicode(const wchar_t *pIn, bchar_t *pOut, size_t &DestinationSize);
	virtual MatrixError_t FromUnicode(const wchar_t *pIn, size_t length, bchar_t *pOut, size_t &DestinationSize);
	virtual MatrixError_t ToUnicode(const bchar_t *pIn, size_t length, wchar_t *pOut, size_t &DestinationSize);
	virtual MatrixError_t ToUnicode(const bchar_t *pIn, wchar_t *pOut, size_t &DestinationSize);
	size_t GetUnicodeLenght(const bchar_t *pInput);
};

CTransInfoUTF16BE::CTransInfoUTF16BE(codepage_t codepage)
: CTransInfoBase(0, codepage)
{
	m_capabilities = FL_MULTIBYTE;
}

// s strlen that looks for two 0 bytes at the end
size_t CTransInfoUTF16BE::GetUnicodeLenght(const bchar_t *pInput)
{
	size_t Size=0;
	while((pInput[0] != 0) || (pInput[1] != 0))
	{
		pInput+=2;
		Size+=2;
	}
	return Size;
}

// CATION: This function is not tested yet (not used  in code right now)
MatrixError_t CTransInfoUTF16BE::FromUnicode(const wchar_t *pIn, size_t length, bchar_t *pOut, size_t &DestinationSize)
{
	DestinationSize = length*sizeof(wchar_t);

	for(size_t i=0; i<length; i++)
	{
		bchar_t temp = (*pIn>>8)&0xff;
		*pOut++ = temp;
		temp = *pIn&0xff;
		*pOut++ = temp;
		pIn++;
	}

	return ERR_PASS;
}


MatrixError_t CTransInfoUTF16BE::FromUnicode(const wchar_t *pIn, bchar_t *pOut, size_t &DestinationSize)
{
	return FromUnicode(pIn, wcslen(pIn), pOut, DestinationSize);
}

MatrixError_t CTransInfoUTF16BE::ToUnicode(const bchar_t *pIn, size_t length, wchar_t *pOut, size_t &DestinationSize)
{
	DestinationSize = length/sizeof(wchar_t);

	for(size_t i=0; i<length; i+=2)
	{
		*pOut++ = (pIn[i+0]<<8)|pIn[i+1];
	}

	return ERR_PASS;
}

MatrixError_t CTransInfoUTF16BE::ToUnicode(const bchar_t *pIn, wchar_t *pOut, size_t &DestinationSize)
{
	return ToUnicode(pIn, GetUnicodeLenght(pIn), pOut, DestinationSize);
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
/// Handler for the UTF16LE translation.
/*! We translate a byte stream with Little Endian Unicode into real Little Endian Unicode
*/
class CTransInfoUTF16LE : public CTransInfoUTF16BE
{
public:
	CTransInfoUTF16LE(codepage_t codepage);
protected:
	virtual MatrixError_t FromUnicode(const wchar_t *pIn, bchar_t *pOut, size_t &DestinationSize);
	virtual MatrixError_t FromUnicode(const wchar_t *pIn, size_t length, bchar_t *pOut, size_t &DestinationSize);
	virtual MatrixError_t ToUnicode(const bchar_t *pIn, size_t length, wchar_t *pOut, size_t &DestinationSize);
	virtual MatrixError_t ToUnicode(const bchar_t *pIn, wchar_t *pOut, size_t &DestinationSize);
};

CTransInfoUTF16LE::CTransInfoUTF16LE(codepage_t codepage)
: CTransInfoUTF16BE(codepage)
{
}

// CATION: This function is not tested yet (not used  in code right now)
MatrixError_t CTransInfoUTF16LE::FromUnicode(const wchar_t *pIn, size_t length, bchar_t *pOut, size_t &DestinationSize)
{
	DestinationSize = length*sizeof(wchar_t);

	for(size_t i=0; i<length; i++)
	{
		bchar_t temp = *pIn&0xff;
		*pOut++ = temp;
		temp = (*pIn>>8)&0xff;
		*pOut++ = temp;
		pIn++;
	}

	return ERR_PASS;
}


MatrixError_t CTransInfoUTF16LE::FromUnicode(const wchar_t *pIn, bchar_t *pOut, size_t &DestinationSize)
{
	return FromUnicode(pIn, wcslen(pIn), pOut, DestinationSize);
}

MatrixError_t CTransInfoUTF16LE::ToUnicode(const bchar_t *pIn, size_t length, wchar_t *pOut, size_t &DestinationSize)
{
	DestinationSize = length/sizeof(wchar_t);

	for(size_t i=0; i<length; i+=2)
	{
		*pOut++ = pIn[i+0]|(pIn[i+1]<<8);
	}

	return ERR_PASS;
}

MatrixError_t CTransInfoUTF16LE::ToUnicode(const bchar_t *pIn, wchar_t *pOut, size_t &DestinationSize)
{
	return ToUnicode(pIn, GetUnicodeLenght(pIn), pOut, DestinationSize);
}

///////////////////////////////////////////////////////////////////////////////
//! Create the UTF16 handlers.
/*! 
*/
void	CTransCode::CreateUTF16Translator(void)
{
	ASSERT(m_TransInfo[ISO_UTF16BE] == NULL);
	ASSERT(m_TransInfo[ISO_UTF16LE] == NULL);
	m_TransInfo[ISO_UTF16BE] = new CTransInfoUTF16BE(ISO_UTF16BE);
	m_TransInfo[ISO_UTF16LE] = new CTransInfoUTF16LE(ISO_UTF16LE);
}





