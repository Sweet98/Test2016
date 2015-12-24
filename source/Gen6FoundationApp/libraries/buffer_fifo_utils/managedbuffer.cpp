/*=================================================================================
  class: CBufferPool

//=================================================================================
   $Source: managedbuffer.cpp $
   $Date: 2008/12/16 15:12:10EST $
   $Revision:


//=================================================================================*/
//! \file

#include "stdInclude.h"
#include "interrupts.h"
#include "ManagedBuffer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MARKER	0xAA551234

inline size_t GetULONGSize(size_t bytesize)
{
	return bytesize/sizeof(ULONG);
}

CManagedBuffer::CManagedBuffer(size_t BufferSize)
{
	m_user_buffer_size = BufferSize;
	m_total_buffer_size	= GetULONGSize(BufferSize+sizeof(ULONG)-1)*sizeof(ULONG);	// allign the end
	const size_t MarkerSize=2*sizeof(ULONG);
	const size_t SafteyMargin=2*sizeof(ULONG);	// +2 = safty margin (safely can add terminating 0)
	// ensure we get a buffer which is aligned for ULONG access
   m_pulRawBuffer = new ULONG[GetULONGSize(m_total_buffer_size)+SafteyMargin+MarkerSize];
	m_in_use=false;

	m_IndexEndMarker = 1+GetULONGSize(m_total_buffer_size)+SafteyMargin;
	// fill in the security guard markers
	m_pulRawBuffer[SafetyLow] = MARKER;
	m_pulRawBuffer[m_IndexEndMarker] = MARKER;

	// now store the pointer to theuser area of the buffer
	m_pulUserBuffer = &m_pulRawBuffer[BorderLow];
}

CManagedBuffer::~CManagedBuffer()
{
	delete [] m_pulRawBuffer;
}

// check security markers
bool CManagedBuffer::IsValid(void)
{
	return ( (m_pulRawBuffer[SafetyLow] == MARKER) && (m_pulRawBuffer[m_IndexEndMarker] == MARKER) );
}

// Get buffer in various overloads
bool CManagedBuffer::GetBuffer(UCHAR *&pData, size_t &size)
{
	pData = GetBuffer_bchar();
	size  = GetMaxElements_8Bit();
	return TRUE;
}

bool CManagedBuffer::GetBuffer(USHORT *&pData, size_t &size)
{
	pData = GetBuffer_16Bit();
	size  = GetMaxElements_16Bit();
	return TRUE;
}

bool CManagedBuffer::GetBuffer(ULONG *&pData, size_t &size)
{
	pData = GetBuffer_32Bit();
	size  = GetMaxElements_32Bit();
	return TRUE;
}

extern volatile int g_freebuff;
extern volatile int g_Original_freebuff;

bool CManagedBuffer::Free(void)
{
	if(this!=NULL)                         // allow a save call with a NULL pointer
	{
		#ifdef _DEBUG_CBARCODE
		if(IsFree())
		{
			DumpDebug(_T("Alarm, already free"), g_freebuff);
		}
		else
		#endif
		{
			DISABLE_INTERRUPTS();
			#ifdef _DEBUG_CBARCODE
			g_freebuff++;
			if(g_freebuff > g_Original_freebuff)
				DumpDebug(_T("Alarm, somethings wrong"), g_freebuff);
			else
				DumpDebug(_T("Free buffer"), g_freebuff);
			#endif
			m_in_use=false;
			RESTORE_INTERRUPTS();
		}
	}
	return true;
}

#ifdef _DEBUG_CBARCODE
void CManagedBuffer::DumpDebug(const tchar_t *pString, int freebuff)
{
//	if(freebuff < 12-3)
	{
	::xprintf(_T("%s 0x%08X, size=%i, g_freebuff = %i\n"), pString, this, m_total_buffer_size, freebuff);
	if(!IsValid())
		::xprintf(_T("Buffer is invalid!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"));
	}
}
#endif


