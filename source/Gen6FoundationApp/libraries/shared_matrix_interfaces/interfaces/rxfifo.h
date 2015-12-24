/*=============================================================================
 * Header for a fifo structur optimized for low interrupt latency.
 * We try to avoid disabling the interrupts here.
 * Can be used for receiving bytes from a external communication driver
 * in it's ISR. Other use is dangerous. Use the generic fifo instead.
 *
 *

 *=============================================================================
 * $RCSfile: interfaces/rxfifo.h $
 * $Revision: 1.13.3.2 $
 * $Date: 2011/11/14 13:33:47EST $
 *============================================================================*/

#ifndef CPP_RX_FIFO_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define CPP_RX_FIFO_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "FifoBase.h"

class CRxFifo : public CFifoBase
{
public:
	CRxFifo(size_t size);
	virtual ~CRxFifo();
	//This function returns the size of non wrapped portion of unread data
// and modifies the pointer passed in to point to its start
size_t GetNonWrappedData(bchar_t **pDestination);

void RemoveData(size_t size);


inline size_t Write(bchar_t data)
{
	int retval = 0;
	int usedsize =m_head - m_tail;
	size_t tempsize=m_size-1;
	tempsize -=(usedsize>=0)?(usedsize):(m_size + usedsize);
	if (tempsize> 0)
	{
		m_pBuffer[m_head] = data;
		m_head++;
		if (m_head==m_size)
			m_head=0;
		retval = 1;
	}
	else
	{
		m_overflow = TRUE;
	}

	return retval;
}
//	size_t Write(schar_t data)
//	{
//		return Write((bchar_t) data);
//	}

	size_t Write(const bchar_t *pData, size_t size);
	size_t Write(const schar_t *pData, size_t size)
	{
		return Write((const bchar_t*) pData, size);
	}

	int Read(void);
	size_t Read(bchar_t *pDestination, size_t size);
	int Peek( unsigned int offset = 0 );
	size_t Peek( bchar_t *pDestination, size_t offset = 0 );

//	short ReadWithWait(void);
//	int ReadWithWait (char *pDestination, int size);
private:
	size_t Read_(bchar_t *pDestination, size_t size, size_t contents);

protected:
	bchar_t *m_pBuffer;

private:
	CRxFifo (const CRxFifo&);            // declared private, but not defined, to prevent copying         
	CRxFifo& operator=(const CRxFifo&);  // declared private, but not defined, to prevent copying 

};


#endif /* CPP_RX_FIFO_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */


