/*=================================================================================
  Codepage translations. This class knows internas of the CBarcodeData.

//=================================================================================
   $Source: Gen6FoundationApp/FoundationApp/eci/transcodeex.cpp $
   $Date: 2010/10/01 09:37:54EDT $
   $Revision: 1.5 $

//=================================================================================*/
//! \file

#include "stdInclude.h"
#include "TransCodeEx.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ManagedBuffer.h"
#include "BufferPool.h"
#include "BarcodeData.h"
#include "HSTRING.h"
#include "HWSTRING.h"
#include "uchar.h"

#ifdef _WINDOWS
#include "menuSimulation.h"
#else
#include "menu.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//! The public Transcode object pointer
CTransCodeEx  			*g_pTransCode=NULL;

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//! Use this static member to create the public CTransCodeEx object.
/*!
*/
void CTransCodeEx::CreateTransCode(void)
{
	if (g_pTransCode == NULL)
	{
		g_pTransCode = new CTransCodeEx;
	}
	ASSERT(g_pTransCode != NULL);
}

///////////////////////////////////////////////////////////////////////////////
//! Use this static member to destroy the public CTransCodeEx object.
/*!
*/
void CTransCodeEx::DeleteCTransCode(void)
{
	delete g_pTransCode;
	g_pTransCode = NULL;
}

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//! Translate the CBarcodeData object from the system codepage to unicode.
/*!
 \param *pLabel Point to a CBarcodeData object
*/
void CTransCodeEx::TranslateSystemCPToUnicode(CBarcodeData *pLabel)
{
	TranslateToUnicode(pLabel, GetSystemCodepage());
}

///////////////////////////////////////////////////////////////////////////////
//! Translate the CBarcodeData object from unicode to the system codepage.
/*!
 \param *pLabel Point to a CBarcodeData object
*/
void CTransCodeEx::TranslateFromUnicodeToSystemCP(CBarcodeData *pLabel)
{
	TranslateFromUnicodeToCP(pLabel, GetSystemCodepage(), IO_DATATYPE_CP_SYS);
}

///////////////////////////////////////////////////////////////////////////////
//! Translate the CBarcodeData object from the codepage to unicode.
/*!
 \param *pLabel Point to a CBarcodeData object
 \param CurrentCodepage Codepage
*/
void CTransCodeEx::TranslateToUnicode(CBarcodeData *pLabel, codepage_t CurrentCodepage)
{
	bchar_t *pIn;
	wchar_t *pOut;
	size_t size;
	if (PASS == pLabel->GetContentsBuffer(pIn, size))
	{
		size_t NumUnicode = size;
		CBarcodeData *pDestObj = CBarcodeData::GetSaveNewBuffer(pOut, NumUnicode);

		pDestObj->SetUnicodeData();

		MatrixError_t RetVal = StringToUnicode(CurrentCodepage, pIn, size, pOut, NumUnicode);
		if (!IsOK(RetVal))
		{
			// Translation failed. Probably because of a non supported codepage.
			// This should never happen. We handle it gratefully.
			RetVal = StringToUnicode(CP_BINARY, pIn, size, pOut, NumUnicode);
			if (!IsOK(RetVal))
			{
				// PANIC!!!
				pDestObj->Free();	// at least try to clean up
				pDestObj = NULL;
			}
		}

		if (pDestObj != NULL)
		{
			pDestObj->FinalizeBuffer(NumUnicode);
			pLabel->MoveBuffer(pDestObj);
		}
	}
	else
	{
		// error situation
		// we do nothing and let the situation be handled by a later call.
	}
}

///////////////////////////////////////////////////////////////////////////////
//! Translate the CBarcodeData object from unicode to the codepage.
/*!
 \param *pLabel Point to a CBarcodeData object
 \param CurrentCodepage Codepage
 \param NewType CBarcodeData type as IoDataType_t
*/
void CTransCodeEx::TranslateFromUnicodeToCP(CBarcodeData *pLabel, codepage_t CurrentCodepage, IoDataType_t NewType)
{
	wchar_t *pIn;
	bchar_t *pOut;
	size_t size;
	if (PASS == pLabel->GetContentsBuffer(pIn, size))
	{
		size_t BufferSize = size;
		CBarcodeData *pDestObj = CBarcodeData::GetSaveNewBuffer(pOut, BufferSize);

		MatrixError_t RetVal = UnicodeToString(CurrentCodepage, pIn, size, pOut, BufferSize);
		if (RetVal == ERR_INSUFFIZIENT_BUFFER)
		{
			// Translation failed. We try to handle it gratefully.
			// Try with a larger buffer (UTF8 takes 4 times the space in worst case).
			BufferSize = size*4;
			pDestObj->Free();	// clean up old (too small) object
			pDestObj = CBarcodeData::GetSaveNewBuffer(pOut, BufferSize);
			RetVal = UnicodeToString(CurrentCodepage, pIn, size, pOut, BufferSize);
		}
		if (!IsOK(RetVal))
		{
			// Translation failed. Probably because of a non supported codepage.
			// This should never happen. We handle it gratefully.
			// PANIC!!!
			pDestObj->Free();	// at least try to clean up
			pDestObj = NULL;
		}

		if (pDestObj != NULL)
		{
			pDestObj->StoreDataType(NewType);
			pDestObj->FinalizeBuffer(BufferSize);  // conversion returned the final size
			pLabel->MoveBuffer(pDestObj);
		}
	}
	else
	{
		// error situation
		// we do nothing and let the situation be handled by a later call.
	}
}

///////////////////////////////////////////////////////////////////////////////
//! Translates both byte and wide contents into the oposite endian mode.
/*! User is responsible to pass the correct NewType. You mess everything up here, so choose carefully.
 The size must be an even number for byte streams.

 \param *pLabel Point to a CBarcodeData object
 \param NewType CBarcodeData type as IoDataType_t
*/
void CTransCodeEx::TranslateBigAndLittleEndian(CBarcodeData *pLabel, IoDataType_t NewType)
{
	bchar_t *pBData;
	wchar_t *pWData;
	size_t size;
	if (PASS == pLabel->GetContentsBuffer(pBData, size))
	{
		for(size_t i=0; i<size; i+=2)
		{
			bchar_t temp= pBData[0];
			pBData[0] = pBData[1];
			pBData[1] = temp;
			pBData+=2;
		}
		pLabel->StoreDataType(NewType);
	}
	else if (PASS == pLabel->GetContentsBuffer(pWData, size))
	{
		for(size_t i=0; i<size; i++)
		{
			wchar_t temp = *pWData;
			*pWData = ((temp<<8)&0xff00)|((temp>>8)&0xff);
			pWData++;
		}
		pLabel->StoreDataType(NewType);
	}
	else
	{
		// error situation
		// we do nothing and let the situation be handled by a later call.
	}
}

///////////////////////////////////////////////////////////////////////////////
//! Translates a byte stream to wide characters and fixes the endianess if it is wrong.
/*!
 \param *pLabel Point to a CBarcodeData object
*/
void CTransCodeEx::TranslateByteStreamToWide(CBarcodeData *pLabel)
{
	IoDataType_t CurrentType = pLabel->GetDataType();
	if(CurrentType == IO_DATATYPE_UTF16BES)
	{
		TranslateBigAndLittleEndian(pLabel, IO_DATATYPE_UTF16LES);
		CurrentType = pLabel->GetDataType();
	}
	if(CurrentType == IO_DATATYPE_UTF16LES)
	{
		// now make it a UNICODE interpretation
		size_t NumCharacter = pLabel->GetLabelSize() / 2;
		pLabel->FinalizeBuffer(NumCharacter, IO_DATATYPE_UNICODE);
	}
}

///////////////////////////////////////////////////////////////////////////////
//! Translates wide characters to a byte stream and fixes the endianess if it is wrong.
/*!
 \param *pLabel Point to a CBarcodeData object
 \param NewType CBarcodeData type as IoDataType_t
*/
void CTransCodeEx::TranslateWideToByteStream(CBarcodeData *pLabel, IoDataType_t NewType)
{
	if(NewType == IO_DATATYPE_UTF16BES)
	{
		TranslateBigAndLittleEndian(pLabel, IO_DATATYPE_UNICODE);
	}
 	// now make it a byte stream interpretation
 	size_t NumBytes = pLabel->GetLabelSize() * 2;
 	pLabel->FinalizeBuffer(NumBytes, NewType);
}

///////////////////////////////////////////////////////////////////////////////
//! Translates everything into unicode (wide characters).
/*!
 \param *pLabel Point to a CBarcodeData object
 \param CurrentType CBarcodeData type as IoDataType_t
 \return new CBarcodeData type as IoDataType_t
*/
IoDataType_t CTransCodeEx::EnsureItsUnicode(CBarcodeData *pLabel, IoDataType_t CurrentType)
{
	if(CurrentType!=IO_DATATYPE_UNICODE)
	{
		AutoTranslate(pLabel, IO_DATATYPE_UNICODE);  // translate everything into unicode first
		CurrentType = pLabel->GetDataType();
	}
	return CurrentType;
}

///////////////////////////////////////////////////////////////////////////////
//! Try to fix wrong formats in the label.
/*! We translate the current contents into the OutputFormat.
 \param *pLabel Point to a CBarcodeData object
 \param OutputFormat as in int (IoDataType_t)
*/
void CTransCodeEx::AutoTranslate(CBarcodeData  *pLabel, int OutputFormat)
{
	IoDataType_t CurrentType = pLabel->GetDataType();
	switch(OutputFormat)
	{
		case IO_DATATYPE_UNICODE:
			switch(CurrentType)
			{
				case IO_DATATYPE_UNICODE:
					// already correct, no change required
				break;
				case IO_DATATYPE_CP_SYS:
					TranslateSystemCPToUnicode(pLabel);
				break;
				case CP_BINARY:
					TranslateToUnicode(pLabel, CP_BINARY);
				break;
				case IO_DATATYPE_UTF8:
					TranslateToUnicode(pLabel, ISO_UTF8);
				break;
				case IO_DATATYPE_UTF16LES:
				case IO_DATATYPE_UTF16BES:
					TranslateByteStreamToWide(pLabel);
				break;

				default:
					// do nothing, must be catched later
				break;
			}
		break;

		case IO_DATATYPE_CP_SYS:
			switch(CurrentType)
			{
				case IO_DATATYPE_UNICODE:
					TranslateFromUnicodeToSystemCP(pLabel);
				break;
				case IO_DATATYPE_CP_SYS:
					// already correct, no change required
				break;
				default:
					// do nothing, must be catched later
				break;
			}
			break;

		case IO_DATATYPE_BINARY:
			switch(CurrentType)
			{
				case IO_DATATYPE_UNICODE:
					TranslateFromUnicodeToCP(pLabel, CP_BINARY, IO_DATATYPE_BINARY);
				break;
				case IO_DATATYPE_BINARY:
					// already correct, no change required
				break;
				default:
					// do nothing, must be catched later
				break;
			}
			break;

		case IO_DATATYPE_UTF16LES:
			CurrentType = EnsureItsUnicode(pLabel, CurrentType);	// translate everything into unicode first
			if(CurrentType==IO_DATATYPE_UNICODE)
				TranslateWideToByteStream(pLabel, IO_DATATYPE_UTF16LES);
			break;

		case IO_DATATYPE_UTF16BES:
			CurrentType = EnsureItsUnicode(pLabel, CurrentType);	// translate everything into unicode first
			if(CurrentType==IO_DATATYPE_UNICODE)
				TranslateWideToByteStream(pLabel, IO_DATATYPE_UTF16BES);
			break;

		case IO_DATATYPE_UTF8:
			CurrentType = EnsureItsUnicode(pLabel, CurrentType);	// translate everything into unicode first
			if(CurrentType==IO_DATATYPE_UNICODE)
				TranslateFromUnicodeToCP(pLabel, ISO_UTF8, IO_DATATYPE_UTF8);
			break;

		default:
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
//! Translate the HSTRING object from the system codepage to unicode.
/*!
 \param *pSource HSTRING
 \param *pDestination HWSTRING
 \return true if ok, false if error
*/
bool CTransCodeEx::TranslateSystemCPToUnicode(HSTRING *pSource, HWSTRING *pDestination)
{
	return TranslateToUnicode(pSource, pDestination, GetSystemCodepage());
}

///////////////////////////////////////////////////////////////////////////////
//! Translate the HWSTRING object from unicode to the system codepage.
/*!
 \param *pSource HSTRING
 \param *pDestination HWSTRING
 \return true if ok, false if error
*/
bool CTransCodeEx::TranslateFromUnicodeToSystemCP(HWSTRING *pSource, HSTRING *pDestination)
{
	return TranslateFromUnicodeToCP(pSource, pDestination, GetSystemCodepage());
}

///////////////////////////////////////////////////////////////////////////////
//! Translate the HSTRING object from the codepage to unicode.
/*!
 \param *pSource HSTRING
 \param *pDestination HWSTRING
 \param Codepage Codepage
 \return true if ok, false if error
*/
bool CTransCodeEx::TranslateToUnicode(HSTRING *pSource, HWSTRING *pDestination, codepage_t Codepage)
{
	bool Status = pDestination->Reallocate(pSource->Size);
	if(Status)
	{
		size_t DestinationSize = pSource->Size;
		MatrixError_t RetVal;
		RetVal = StringToUnicode(Codepage, pSource->Char, pSource->Size, pDestination->WChar, DestinationSize);
		Status = IsOK(RetVal);
		pDestination->Size=DestinationSize;
		pDestination->WChar[DestinationSize]=0;	// add a terminating 0
	}
	return Status;
}


///////////////////////////////////////////////////////////////////////////////
//! Translate the HWSTRING object from unicode to the codepage.
/*!
 \param *pSource HSTRING
 \param *pDestination HWSTRING
 \param Codepage Codepage
 \return true if ok, false if error
*/
bool CTransCodeEx::TranslateFromUnicodeToCP(HWSTRING *pSource, HSTRING *pDestination, codepage_t Codepage)
{
	bool Status = pDestination->Reallocate(pSource->Size);
	if(Status)
	{
		size_t DestinationSize = pSource->Size;
		MatrixError_t RetVal;
		RetVal = UnicodeToString(Codepage, pSource->WChar, pSource->Size, pDestination->Char, DestinationSize);
		if (RetVal == ERR_INSUFFIZIENT_BUFFER)
		{
			// Translation failed. We try to handle it gratefully.
			// Try with a larger buffer (UTF8 takes 4 times the space in worst case).
			DestinationSize = pSource->Size*4;          // BufferSize *4
			Status = pDestination->Reallocate(DestinationSize);
			if(Status)
			{
				RetVal = UnicodeToString(Codepage, pSource->WChar, pSource->Size, pDestination->Char, DestinationSize);
			}
		}
		Status = IsOK(RetVal);
		pDestination->Size=DestinationSize;  	// returned by the conversion function
		pDestination->Char[DestinationSize]=0;	// add a terminating 0
	}
	return Status;
}






