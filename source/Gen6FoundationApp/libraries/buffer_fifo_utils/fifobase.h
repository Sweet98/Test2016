/*=============================================================================
 * Header for a fifo structur optimized for low interrupt latency.
 * We try to avoid disabling the interrupts here.
 * Can be used for receiving bytes from a external communication driver
 * in it's ISR. Other use is dangerous. Use the generic fifo instead.
 *
 *

 *=============================================================================
 * $RCSfile: fifobase.h $
 * $Revision: 1.2 $
 * $Date: 2009/11/25 11:39:35EST $
 *============================================================================*/
//! \file

#ifndef CPP_FIFO_BASE_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define CPP_FIFO_BASE_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include <stdlib.h>

class CFifoBase
{
public:
	CFifoBase(size_t size);
	virtual ~CFifoBase();

	bool IsEmpty(void)	{ return !AnyData(); };
	size_t AnyData(void);
	virtual void Purge(void);
	size_t SpaceAvailable(void);

protected:
	void BumpPointer(volatile int &index, int step=1);

protected:
	int			 m_size;
	volatile int m_head;
	volatile int m_tail;
	bool m_overflow;
	volatile bool m_disable_writes;
};

template <typename ItemType> class TFifo : public CFifoBase
{
public:
	TFifo(size_t size)
	: CFifoBase(size)
	{
		m_pBuffer = new ItemType[size+1];
	}

	virtual ~TFifo()
	{
		delete [] m_pBuffer;
	}

	size_t Write(ItemType item)
	{
	//	TRACE("Fifo Barcode %x\r\n", Ptr);
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
			m_overflow = TRUE;
	//		TRACE("-------- Barcode Fifo Overflow \r\n");
	//		ASSERT(0);
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

protected:
	ItemType *m_pBuffer;
};


#endif /* CPP_FIFO_BASE_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */



