/*=================================================================================
  class: CBufferPool

//=================================================================================
   $Source: BufferPool.cpp $
   $Date: 2011/11/14 13:33:47EST $
   $Revision:


//=================================================================================*/
//! \file

#include "stdInclude.h"
#include "ManagedBuffer.h"
#include "BufferPool.h"
#include "errordefs.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


volatile int g_freebuff;
volatile int g_Original_freebuff;

// A helper class to keep the manager simpler.
// This class holds all buffers of the same size.
// So for different sizes, we need an instance of this class for every size.
class CPoolHelper
{
friend class CBufferManager;
	CPoolHelper(size_t NumBuffers, size_t BufferSize);
	virtual ~CPoolHelper();

	size_t GetSize(void)				{ return m_buffer_bytesize;				};
	BOOL IsSuitable(size_t size)	{ return (size < m_buffer_bytesize);	};

	MatrixError_t IsValid(void);

	CManagedBuffer *GetNewBuffer(void);

	void FreeAllBuffers(void);	// Free means to mark it unused. It does not delete the buffer.

	size_t m_NextAvailableIndex;
	size_t m_num_buffers;
	size_t m_buffer_bytesize;
	CManagedBuffer **m_buffers;

	// debug and test helpers
protected:
#ifdef _DEBUG_CBARCODE
	static FILE *m_fp;
	void __cdecl printf(const tchar_t *fmt, ...);
	#define PH_TRACE(fmt)			printf(fmt);
	#define PH_TRACE1(fmt, var)	printf(fmt, var);
	#define PH_TRACE2(fmt, var1, var2)	printf(fmt, var1, var2);

	#define PH_PREPARE_DEBUG()			if(m_fp==NULL) { m_fp = fopen(_T("CBufferManager.log"), _T("w+"));	}
	#define PH_CLEANUP_DEBUG()			if(m_fp!=NULL) { fclose(m_fp); m_fp=NULL; }

#else
	#define PH_PREPARE_DEBUG()
	#define PH_TRACE(fmt)
	#define PH_TRACE1(fmt, var)
	#define PH_TRACE2(fmt, var1, var2)
	#define PH_CLEANUP_DEBUG()
#endif
private:
	CPoolHelper (const CPoolHelper&);            // declared private, but not defined, to prevent copying         
	CPoolHelper& operator=(const CPoolHelper&);  // declared private, but not defined, to prevent copying 

};

#ifdef _DEBUG_CBARCODE
FILE *CPoolHelper::m_fp=NULL;
#endif

CPoolHelper::CPoolHelper(size_t NumBuffers, size_t BufferSize)
{
	PH_PREPARE_DEBUG();
	size_t i;
	m_NextAvailableIndex = 0;
	// Get an array to hold the pointers
	m_buffers = new CManagedBuffer* [NumBuffers];

	// now create all the buffers
	for(i=0; i<NumBuffers; i++)
	{
		m_buffers[i] = new CManagedBuffer(BufferSize);
//		TRACE2("Buffer = 0x%08X, size = %i\r\n", m_buffers[i], BufferSize);
	}
	m_num_buffers = NumBuffers;
	// the size can be adjusted for allignment, so we read it back
	m_buffer_bytesize = m_buffers[0]->GetBufferSize();	// all objects in this pool have the same size
}

CPoolHelper::~CPoolHelper(void)
{
	size_t i;
	for(i=0; i<m_num_buffers; i++)
	{
//		TRACE2("Delete Buffer = 0x%08X, size = %i\r\n", m_buffers[i], m_buffer_bytesize);
		delete m_buffers[i];
	}
//	TRACE1("Delete Array of pointers = 0x%08X\r\n", m_buffers);
	delete [] m_buffers;
	PH_CLEANUP_DEBUG();
}

#ifdef _DEBUG_CBARCODE
void __cdecl CPoolHelper::printf(const tchar_t *fmt, ...)
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

// Speed optimization:
// We first search through the list starting form one place above the last used one.
// This should give us an object almost imediately.
// If it fails, we restart searching from the beginning.
CManagedBuffer *CPoolHelper::GetNewBuffer(void)
{
	size_t i;
	CManagedBuffer *pBuffer= NULL;
	PH_TRACE1(_T("Start(%i) "), GetSize());
	for(i=m_NextAvailableIndex; i<m_num_buffers; i++)
	{
		PH_TRACE1(_T("%03i "), i);
		if (m_buffers[i]->IsFree())
		{
			pBuffer = m_buffers[i];
			break;
		}
	}
	if (pBuffer == NULL)	// still not found, try a second chance
	{
		PH_TRACE(_T("v "));
		for(i=0; i<m_NextAvailableIndex; i++)
		{
			PH_TRACE1(_T("%03i "), i);
			if (m_buffers[i]->IsFree())
			{
				pBuffer = m_buffers[i];
				break;
			}
		}
	}

	if (pBuffer != NULL)	// if we found one, mark it as used
	{
		pBuffer->Use();
		g_freebuff--;
		pBuffer->DumpDebug(_T("Use buffer "), g_freebuff);
	}

	if(++m_NextAvailableIndex >= m_num_buffers)	// pump the index
		m_NextAvailableIndex = 0;

#ifdef _DEBUG_CBARCODE
	if (pBuffer == NULL)	// still not found
	{
		PH_TRACE1(_T("No (%03i)\n"), m_NextAvailableIndex);
	}
	else
	{
		PH_TRACE(_T("\n"));
	}
#endif

	return pBuffer;
}

void CPoolHelper::FreeAllBuffers(void)
{
	size_t i;
	for(i=0; i<m_num_buffers; i++)
	{
		m_buffers[i]->Free();
	}
}

MatrixError_t CPoolHelper::IsValid(void)
{
	MatrixError_t RetVal = ERR_PASS;
	size_t i;
	for(i=0; i<m_num_buffers; i++)
	{
		if (!m_buffers[i]->IsValid())
		{
			RetVal = ERR_BAD_GUARD;
			break;
		}
	}
	return RetVal;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct BufferSizes_t
{
	size_t Size;
	size_t NumBuffers;
};



static BufferSizes_t BufferList[] =
{
//	Size,		NumBuffers
	30,		100,
	60,		100,
	200,		100,
	600,		40,
	1050,		20,	// can hold a 1024 block with some header/trailer space
	3000,		20,
	16000,	3,		// can hold an HHLC scan
	30000,	3,
	66000,	2,
	FULL_IMAGE_BUFFER,	1,	// can hold a image (541238 bytes)
};


/////////////////////////////////////////////////////////////////////
CBufferManager  			*g_pBufferManager=NULL;
void DebugFreeAllBuffers(void)
{		if (g_pBufferManager != NULL)
			g_pBufferManager->FreeAllBuffers();

}

void CBufferManager::CreateBufferManager(void)
{
	if (g_pBufferManager == NULL)
	{
		g_pBufferManager = new CBufferManager;
	}
	ASSERT(g_pBufferManager != NULL);
}

void CBufferManager::DeleteBufferManager(void)
{
	delete g_pBufferManager;
	g_pBufferManager = NULL;
}

CBufferManager::CBufferManager()
{
	poolmtx= MT_CreateMutex(false);
	g_freebuff = 0;
	m_NumPools = sizeof(BufferList)/sizeof(BufferSizes_t);
	// Get space for the pools
	m_pools = new CPoolHelper*[m_NumPools];

	// Now create the pools
	for(size_t i=0; i<m_NumPools; i++)
	{
		m_pools[i] = new CPoolHelper(BufferList[i].NumBuffers, BufferList[i].Size);
//		TRACE3("Pool = 0x%08X with %i of size = %i\r\n", m_pools[i],BufferList[i].NumBuffers, BufferList[i].Size);
		g_freebuff += BufferList[i].NumBuffers;
	}

	g_Original_freebuff = g_freebuff;
	m_pools[0]->m_buffers[0]->DumpDebug(_T("CBufferManager ctor:"), g_freebuff);
}

CBufferManager::~CBufferManager(void)
{
	size_t i;
	for(i=0; i<m_NumPools; i++)
	{
//		TRACE1("Delete Pool = 0x%08X\r\n", m_pools[i]);
		delete m_pools[i];
	}
//	TRACE1("Delete Array of pools = 0x%08X\r\n", m_pools);
	delete[] m_pools;
	MT_CloseMutex(poolmtx);
}

// find a buffer with a suitable size and look it
CManagedBuffer *CBufferManager::GetNewBuffer(size_t bytesize)
{
	CManagedBuffer *RetVal= NULL;
	if (MT_WaitMutex(poolmtx,INFINITE))
	{
		for(size_t i=0; i<m_NumPools; i++)
		{
			if (m_pools[i]->IsSuitable(bytesize))
			{
				RetVal = m_pools[i]->GetNewBuffer();
				if(RetVal!=NULL)
				{
					break;
				}
			}
		}
	#ifdef _DEBUG_CBARCODE
		if (RetVal==NULL)
		{
			int test=0;
		}
	#endif
	MT_ReleaseMutex(poolmtx);
	}
	return RetVal;
}

// a buffer for NumElemets of type tchar_t
CManagedBuffer *CBufferManager::GetNewBuffer(tchar_t *&pText, size_t &NumElements)
{
	CManagedBuffer *RetVal= GetNewBuffer(NumElements*sizeof(tchar_t));
	if(RetVal != NULL)
	{
		RetVal->GetBuffer(pText, NumElements);
	}
	return RetVal;
}

#ifdef UNICODE
// a buffer for NumElemets of type char
CManagedBuffer *CBufferManager::GetNewBuffer(UCHAR *&pData, size_t &NumElements)
{
	CManagedBuffer *RetVal= GetNewBuffer(NumElements*sizeof(UCHAR));
	if(RetVal != NULL)
	{
		RetVal->GetBuffer(pData, NumElements);
	}
	return RetVal;
}
#else
// a buffer for NumElemets of type tchar_t
CManagedBuffer *CBufferManager::GetNewBuffer(wchar_t *&pText, size_t &NumElements)
{
	CManagedBuffer *RetVal= GetNewBuffer(NumElements*sizeof(wchar_t));
	if(RetVal != NULL)
	{
		RetVal->GetBuffer((USHORT *&)pText, NumElements);
	}
	return RetVal;
}
#endif

// a buffer for NumElemets of type int
CManagedBuffer *CBufferManager::GetNewBuffer(ULONG *&pData, size_t &NumElements)
{
	CManagedBuffer *RetVal= GetNewBuffer(NumElements*sizeof(ULONG));
	if(RetVal != NULL)
	{
		RetVal->GetBuffer(pData, NumElements);
	}
	return RetVal;
}

// some clean-up
void CBufferManager::FreeAllBuffers(void)
{
	size_t i;
	for(i=0; i<m_NumPools; i++)
	{
//		TRACE1("Freeing Pool = 0x%08X\r\n", m_pools[i]);
		m_pools[i]->FreeAllBuffers();
	}
}

MatrixError_t CBufferManager::IsValid(void)
{
	MatrixError_t RetVal = ERR_PASS;
	size_t i;
	for(i=0; i<m_NumPools; i++)
	{
		RetVal = m_pools[i]->IsValid();
		if (!IsOK(RetVal))
			break;
	}
	return RetVal;
}

// we could implement some time counting and free very old buffers.
// (kind of garbage collection)
// It is not required if all parts of the software do it right,
// but if not, then it might help a lot.





