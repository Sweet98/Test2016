/*=================================================================================
  class: CBarcodeManager

//=================================================================================
   $Source: barcodedataman.h $
   $Date: 2008/12/16 15:12:06EST $
   $Revision:


//=================================================================================*/
//! \file

#ifndef BARCODE_BUFFER_MAN_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define BARCODE_BUFFER_MAN_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

class CBarcodeData;
class CManagedBuffer;
#include "BufferPool.h"
#include "BarcodeData.h"
#include "thread.h"
///////////////////////////////////////////////////////////////////////////////
/// Storage space for all kind of decoded barcodes.
/// It manages the CBarcodeData objects.
class CBarcodeManager
{
// public methods
public:
	CBarcodeManager(void);
	virtual ~CBarcodeManager();

	MatrixError_t IsValid(void);

	static void CreateBarcodeManager(void);
	static void DeleteBarcodeManager(void);

#if(0)  // the ARM compiler has a problem with that template
// I keep the code for reference
	template <typename TYPE>
	CBarcodeData *GetNewBuffer(TYPE *&pData, size_t &NumElements)
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
#endif
	CBarcodeData *GetNewBuffer(bchar_t *&pData, size_t &NumElements);
	CBarcodeData *GetNewBuffer(wchar_t *&pData, size_t &NumElements);
	CBarcodeData *GetNewBuffer(ULONG *&pData, size_t &NumElements);

	CBarcodeData *GetNewBuffer(int *&pData, size_t &NumElements)
	{
		return GetNewBuffer((ULONG *&)pData, NumElements);
	}
	CBarcodeData *GetNewBuffer(char *&pData, size_t &NumElements)
	{
		return GetNewBuffer((bchar_t *&)pData, NumElements);
	}

	// find a buffer with a suitable size (compatibility with older code)
	CBarcodeData *GetNewBuffer(size_t bytesize);

// internal methods
protected:
	// busy flags

	CBarcodeData *GetFreshInstance(void);
	void BumpIndex(void);
	MT_Mutex_t barcodepoolmtx;
// data members
private:
	enum
	{
		NumBarcodes = 350
	};
	CBarcodeData *m_BarcodeArray[NumBarcodes];
	size_t	m_NextAvailableIndex;

// debug and test helpers
protected:
#ifdef _DEBUG_CBARCODE
	FILE *m_fp;
	void __cdecl printf(const tchar_t *fmt, ...);
	#define BM_TRACE(fmt)			printf(fmt);
	#define BM_TRACE1(fmt, var)	printf(fmt, var);

	#define PREPARE_DEBUG()		m_fp = fopen(_T("BarMan.log"), _T("w+"))
	#define CLEANUP_DEBUG()		fclose(m_fp)

#else
	#define PREPARE_DEBUG()
	#define BM_TRACE(fmt)
	#define BM_TRACE1(fmt, var)
	#define CLEANUP_DEBUG()
#endif
};

extern   CBarcodeManager  			*g_pBarcodeManager;
#define  theBarcodeBufferManager	(*g_pBarcodeManager)


#endif // BARCODE_BUFFER_MAN_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_





