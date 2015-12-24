/*=============================================================================
 * Source for a general fifo structur
 *
 *

 *=============================================================================
 * $RCSfile: fifobase.cpp $
 * $Revision: 1.3 $
 * $Date: 2011/10/14 16:07:07EDT $
 *============================================================================*/
//! \file

#include "stdInclude.h"
#include "product.h"
#include "FifoBase.h"
#include "interrupts.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/** \call CFifoBase
	CFifoBase is a generic fifo building block.
	Used to derive:
	- CRxFifo
	- CBarcodeFifo

  It uses a simple design and tries to avoid to disable the interrupts.
  Be extremely carefull if you change anything here. This is boilerplate code!

  It is very easy to break the architecture here! (results in malfunctions once in a while)
*/

CFifoBase::CFifoBase(size_t size)
{
	ASSERT(size < INT_MAX);
	m_size = size;
	m_disable_writes= TRUE;		/* prevent writes */
	m_tail = 0;
	m_head = 0;
	m_overflow = FALSE;
	m_disable_writes= FALSE;		/* allow writes */
}

CFifoBase::~CFifoBase()
{
}

void CFifoBase::Purge(void)
{
	m_disable_writes= TRUE;		/* prevent writes */
	m_tail = 0;
	m_head = 0;
	m_overflow = FALSE;
	m_disable_writes= FALSE;		/* allow writes */
}

size_t CFifoBase::AnyData(void)
{
	DISABLE_INTERRUPTS();
	int size =  m_head - m_tail;
	if (size<0)
	 size+=m_size;
	 RESTORE_INTERRUPTS();
	return size;
}

size_t CFifoBase::SpaceAvailable(void)
{
	size_t size=0;
	if(!m_disable_writes)
		size = m_size - AnyData() - 1;
	return size;
}

void CFifoBase::BumpPointer(volatile int &index, int step)
{
	index += step;
	if (index >= m_size)
	{
		index -= m_size;
	}
	else if( index < 0 )
	{
		index += m_size;
	}
}



