/*=============================================================================
 * Source for a general fifo structur
 *
 *

 *=============================================================================
 * $RCSfile: interfaces/rxfifo.cpp $
 * $Revision: 1.11.1.2 $
 * $Date: 2008/12/16 15:42:13EST $
 *============================================================================*/

#include "stdInclude.h"
#include "rxfifo.h"
#include "interrupts.h"

/**
	CRxFifo is a fifo for bytes. Used by the drivers for receiving.
*/

void CRxFifo::RemoveData(size_t size)
{
	if (size)
	{
		DISABLE_INTERRUPTS();
		BumpPointer(m_tail, size);
		m_overflow=false;
		RESTORE_INTERRUPTS();
	}
}

CRxFifo::CRxFifo(size_t size)
	: CFifoBase(size)
{
    m_pBuffer = new bchar_t [size+1];
}

CRxFifo::~CRxFifo()
{
	delete [] m_pBuffer;
}


size_t CRxFifo::Write(const bchar_t *pData, size_t size)
{
	size_t retval = 0;
	DISABLE_INTERRUPTS();
	if (SpaceAvailable() >= size)
	{
		retval = size;
		for(;size>0;size--)
		{
			m_pBuffer[m_head]=*pData;
			BumpPointer(m_head);
			pData ++;
		}
	}
	else
		m_overflow = TRUE;
	RESTORE_INTERRUPTS();
	return retval;
}

int CRxFifo::Read(void)
{
	int retval = -1;
	DISABLE_INTERRUPTS();
	if (AnyData())
	{
		retval = m_pBuffer[m_tail];
		BumpPointer(m_tail);
	}
	RESTORE_INTERRUPTS();
	return retval;
}

// This read function has been optimized to support single byte reads without using the memcpy.
// Only multiple byte requests are read with the memcpy.
size_t CRxFifo::Read(bchar_t *pDestination, size_t size)
{
	size_t RetVal = 0;
	DISABLE_INTERRUPTS();
	size_t contents = AnyData();
	contents=(contents<size)?contents:size;
	if (contents>0)
	{
		for (;RetVal<contents;RetVal++)
		{
			*pDestination = m_pBuffer[m_tail];
			BumpPointer(m_tail);
			pDestination++;
		}
	}
	RESTORE_INTERRUPTS();

	return RetVal;
}

size_t CRxFifo::GetNonWrappedData(bchar_t **pDestination)
{
	DISABLE_INTERRUPTS();
	int RetVal = m_head - m_tail;
	if (RetVal <0)
		RetVal=m_size -m_tail;
	*pDestination=&m_pBuffer[m_tail];
	RESTORE_INTERRUPTS();
	return RetVal;
}


// PEEK at rx data without incrementing data pointer
// offset allow you to look fowards at the next data available in buffer
// returns the data found
int CRxFifo::Peek( unsigned int offset )
{
	int retval = -1;

	DISABLE_INTERRUPTS();

	//get amount of data available
	size_t size = AnyData();

	if( size > 0 )    //if we have something in fifo
	{
		//if enough data available to allow offset
		if( offset < size )
		{
			if( (m_tail + offset) >= m_size )
			{
				retval = m_pBuffer[ ((m_tail + offset) - m_size) ];
			}
			else
			{
				retval = m_pBuffer[ (m_tail + offset) ];
			}
		}
	}
	RESTORE_INTERRUPTS();
	return retval;
}


// PEEK at rx data without incrementing data pointer
// offset allow you to look fowards 'x' number of  data, if it is available in buffer
// returns the number of data found (offset + 1)
// passed is pointer to destination buffer (unsigned char) and the offset into the fifo (unsigned int)
size_t CRxFifo::Peek( bchar_t *pDestination, size_t offset )
{
	unsigned int retval = 0;       //size_t

	DISABLE_INTERRUPTS();

	//get amount of data available
	size_t size = AnyData();

	if( size > 0 )		//if we have something in fifo
	{
		//if enough data available to allow offset
		if( offset < size )
		{
			for(retval = 0; retval <= offset; retval++)
			{
				if( (m_tail + retval) >= m_size )
				{
					*pDestination = m_pBuffer[ ((m_tail + retval) - m_size) ];
				}
				else
				{
					*pDestination = m_pBuffer[ (m_tail + retval) ];
				}
				pDestination++;
			}
		}
	}
	RESTORE_INTERRUPTS();
	return retval;
}






