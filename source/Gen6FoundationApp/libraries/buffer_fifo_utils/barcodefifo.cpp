/*=============================================================================
 * Source for a general fifo structur
 *
 *

 *=============================================================================
 * $RCSfile: barcodefifo.cpp $
 * $Revision: 1.3 $
 * $Date: 2009/11/20 18:08:44EST $
 *============================================================================*/
//! \file

#include "stdInclude.h"
#include "barcodefifo.h"
#include "interrupts.h"
#include "barcodedata.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
	CBarcodeFifo is a fifo for CBarcodeData pointers. Used by the drivers for sending.
*/

CBarcodeFifo::CBarcodeFifo(size_t size)
: CFifoBase(size)
{
	m_pBuffer = new CBarcodeData*[size+1];
}

CBarcodeFifo::~CBarcodeFifo()
{
	delete [] m_pBuffer;
}

size_t CBarcodeFifo::Write(CBarcodeData *Ptr)
{
	size_t retval = 0;
	DISABLE_INTERRUPTS();
	int size =  m_head - m_tail;
	if (size<0)
	 size+=m_size;
	size= m_size-size;
	size-=1;
	if (!m_disable_writes && size > 0)
	{
		m_pBuffer[m_head] = Ptr;
		BumpPointer(m_head);
		retval = 1;
	}
	else
	{
	//	BumpPointer(m_tail);   //fix lock up issue. 
		m_overflow = TRUE;
	}
	RESTORE_INTERRUPTS();
	return retval;
}

CBarcodeData *CBarcodeFifo::Read(void)
{
	CBarcodeData *retval = NULL;
	DISABLE_INTERRUPTS();
	int size =  m_head - m_tail;
	if (size<0)
	 size+=m_size;
	if (size)
	{
		retval = m_pBuffer[m_tail];
		BumpPointer(m_tail);
	}
	RESTORE_INTERRUPTS();
	return retval;
}

void CBarcodeFifo::Purge(void)
{
	CBarcodeData *pDummy = NULL;
	while(true)
	{
		pDummy = Read();
		if (pDummy==NULL)
			break;
		pDummy->Free();
	}
}


