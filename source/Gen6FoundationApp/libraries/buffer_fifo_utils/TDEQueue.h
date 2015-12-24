/*=============================================================================
 * Template implementation of a double ended queue. This is broken out into 
 * it's own file so that it is not included everywhere fifobase.h is included.
 *

 *=============================================================================
 * $RCSfile: TDEQueue.h $
 * $Revision: 1.1.1.2 $
 * $Date: 2011/10/14 10:44:18EDT $
 *============================================================================*/

#ifndef TDEQUEUE_H_
#define TDEQUEUE_H_

#include "Fifobase.h"
#include "InterruptMacros.h"

template <class ItemType> class TDEQueue : public CFifoBase
{
public:
	TDEQueue(size_t size)
	: CFifoBase(size)
	{
		m_pBuffer = new ItemType[size+1];
	}

	virtual ~TDEQueue
	()
	{
		delete [] m_pBuffer;
	}

	size_t Write(ItemType item)
	{
		size_t retval = 0;
		DISABLE_INTERRUPTS();
		if (SpaceAvailable() > 0)
		{
			m_pBuffer[m_head] = item;
			BumpPointer(m_head);
			retval = 1;
		}
		else
		{
			m_overflow = true;
		}
		RESTORE_INTERRUPTS();
		return retval;
	}

	ItemType Read(void)
	{
		ItemType retval = 0;	               // CAUTION, this is good for pointers, but be aware of it for int etc.
		DISABLE_INTERRUPTS();
		if (AnyData())
		{
			retval = m_pBuffer[m_tail];
			BumpPointer(m_tail);
		}
		RESTORE_INTERRUPTS();
		return retval;
	}

	ItemType Look(void)
	{
		ItemType retval = NULL;
		if (AnyData())
		{
			retval = m_pBuffer[m_tail];
		}
		return retval;
	}
	
	size_t WriteTail(ItemType item)
	{
		size_t retval = 0;
		DISABLE_INTERRUPTS();
		if (SpaceAvailable() > 0)
		{
			BumpPointer(m_tail,-1);
			m_pBuffer[m_tail] = item;
			retval = 1;
		}
		else
		{
			m_overflow = TRUE;
		}
		RESTORE_INTERRUPTS();
		return retval;
	}

	ItemType ReadHead(void)
	{
		ItemType retval = 0;	// CAUTION, this is good for pointers, but be aware of it for int etc.
		DISABLE_INTERRUPTS();
		if (AnyData())
		{
			BumpPointer(m_head,-1);
			retval = m_pBuffer[m_head];
		}
		RESTORE_INTERRUPTS();
		return retval;
	}

	ItemType LookHead(void)
	{
		ItemType retval = NULL;
		if (AnyData())
		{
			int temp = m_head;
			BumpPointer(temp,-1);
			retval = m_pBuffer[temp];
		}
		return retval;
	}

	int GetHead(void)
	{
		return m_head;
	}

	int GetTail(void)
	{
		return m_tail;
	}

	int GetSize(void)
	{
		return m_size;
	}

	ItemType GetItemAt(int location)
	{
		if( location > m_size )
		{
			return NULL;
		}
		else
		{
			return m_pBuffer[location];
		}
	}
	
protected:
	ItemType *m_pBuffer;
};

#endif /*TDEQUEUE_H_*/


