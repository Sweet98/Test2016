/*=================================================================================
  class: CBarcodeData

//=================================================================================
   $Source: barcodedataman.cpp $
   $Date: 2008/12/16 15:12:05EST $
   $Revision:


//=================================================================================*/
//! \file

#include "stdInclude.h"
#ifdef MATRIX
#include "stdlib.h"
#include "string.h"
#include "hstring.h"
#include "interrupts.h"
#endif
#include "errordefs.h"
#include "BarcodeData.h"
#include "BarcodeDataMan.h"
#include "BufferPool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////
CBarcodeManager  			*g_pBarcodeManager=NULL;

void CBarcodeManager::CreateBarcodeManager(void)
{
	if (g_pBarcodeManager == NULL)
	{
		g_pBarcodeManager = new CBarcodeManager;
	}
	ASSERT(g_pBarcodeManager != NULL);
}

void CBarcodeManager::DeleteBarcodeManager(void)
{
	delete g_pBarcodeManager;
	g_pBarcodeManager = NULL;
}

CBarcodeManager::CBarcodeManager(void)
{
	PREPARE_DEBUG();
	barcodepoolmtx =MT_CreateMutex(false);

	m_NextAvailableIndex = 0;
	for(size_t i=0; i<NumBarcodes; i++)
	{
		m_BarcodeArray[i] = new CBarcodeData;
	}
	CBufferManager::CreateBufferManager();
}

CBarcodeManager::~CBarcodeManager()
{
	for(size_t i=0; i<NumBarcodes; i++)
	{
		delete m_BarcodeArray[i];
		m_BarcodeArray[i] = NULL;
	}
	 MT_CloseMutex(barcodepoolmtx);

	CLEANUP_DEBUG();
}

#ifdef _DEBUG_CBARCODE
void __cdecl CBarcodeManager::printf(const tchar_t *fmt, ...)
{
	int ret;
	tchar_t buf[512];
	va_list args;

	ASSERT(m_fp != NULL);
	va_start(args, fmt);
	ret = _vsnprintf(buf, 512-1, fmt, args);
	fputs (buf, m_fp);
	va_end(args);
	ret = 0;
}
#endif

void CBarcodeManager::BumpIndex(void)
{
}

// Speed optimization:
// We first search through the list starting form one place above the last used one.
// This should give us an object almost imediately.
// If it fails, we restart searching from the beginning.
CBarcodeData *CBarcodeManager::GetFreshInstance(void)
{
	CBarcodeData *pLabel = NULL;
	BM_TRACE(_T("Start "));
	pLabel = NULL;
	if (MT_WaitMutex(barcodepoolmtx,INFINITE))
	{
		for(size_t i=m_NextAvailableIndex; i<NumBarcodes; i++)
		{
			BM_TRACE1(_T("%03i "), i);
			if ( m_BarcodeArray[i]->IsFree() )
			{
				pLabel = m_BarcodeArray[i];
				m_NextAvailableIndex = i;
				break;
			}
		}
		if (pLabel == NULL)	// still not found, try a second chance
		{
			BM_TRACE(_T("v "));
			for(size_t i=0; i<m_NextAvailableIndex; i++)
			{
				BM_TRACE1(_T("%03i "), i);
				if ( m_BarcodeArray[i]->IsFree() )
				{
					pLabel = m_BarcodeArray[i];
					m_NextAvailableIndex = i;
					break;
				}
			}
		}

		if (pLabel != NULL)	// if we found one, mark it as used
		{
			pLabel->Use();
		}

		if(++m_NextAvailableIndex >= NumBarcodes)	// pump the index
			m_NextAvailableIndex = 0;

#ifdef _DEBUG_CBARCODE
		if (pLabel == NULL)	// still not found
		{
			BM_TRACE1(_T("No (%03i)\n"), m_NextAvailableIndex);
		}
		else
		{
			BM_TRACE(_T("\n"));
		}
#endif
		MT_ReleaseMutex(barcodepoolmtx);
	}
	return pLabel;
}

MatrixError_t CBarcodeManager::IsValid(void)
{
	MatrixError_t RetVal = ERR_PASS;

	for(size_t i=0; i<NumBarcodes; i++)
	{
		RetVal = m_BarcodeArray[i]->IsValid();
		if (!IsOK(RetVal))
			break;
	}

	return RetVal;
}


// find a buffer with a suitable size and type
CBarcodeData *CBarcodeManager::GetNewBuffer(bchar_t *&pData, size_t &NumElements)
{
	CBarcodeData *pLabel = GetFreshInstance();
	if(pLabel != NULL)
	{
		CManagedBuffer *pBuffer = theBufferManager.GetNewBuffer(pData, NumElements);
		if(pBuffer != NULL)
		{	// so we've got a buffer, now assign it to a CBarcodeData object
			pLabel->AttachBuffer(pBuffer, pData);
			pLabel->Reset();
		}
		else
		{
			pLabel->Free();
			pLabel = NULL;
		}
	}
	return pLabel;
}

CBarcodeData *CBarcodeManager::GetNewBuffer(wchar_t *&pData, size_t &NumElements)
{
	CBarcodeData *pLabel = GetFreshInstance();
	if(pLabel != NULL)
	{
		CManagedBuffer *pBuffer = theBufferManager.GetNewBuffer(pData, NumElements);
		if(pBuffer != NULL)
		{	// so we've got a buffer, now assign it to a CBarcodeData object
			pLabel->AttachBuffer(pBuffer, pData);
			pLabel->Reset();
		}
		else
		{
			pLabel->Free();
			pLabel = NULL;
		}
	}
	return pLabel;
}

CBarcodeData *CBarcodeManager::GetNewBuffer(ULONG *&pData, size_t &NumElements)
{
	CBarcodeData *pLabel = GetFreshInstance();
	if(pLabel != NULL)
	{
		CManagedBuffer *pBuffer = theBufferManager.GetNewBuffer(pData, NumElements);
		if(pBuffer != NULL)
		{	// so we've got a buffer, now assign it to a CBarcodeData object
			pLabel->AttachBuffer(pBuffer, pData);
			pLabel->Reset();
		}
		else
		{
			pLabel->Free();
			pLabel = NULL;
		}
	}
	return pLabel;
}

// find a buffer with a suitable size (compatibility with older code)
CBarcodeData *CBarcodeManager::GetNewBuffer(size_t bytesize)
{
	bchar_t *pDummy;	// use this dummy to select the correct data type in the template version
	return CBarcodeManager::GetNewBuffer(pDummy, bytesize);
}




