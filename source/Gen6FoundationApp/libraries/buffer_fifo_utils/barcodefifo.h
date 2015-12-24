/*=============================================================================
 * Header for a fifo structure
 *
 *

 *=============================================================================
 * $RCSfile: barcodefifo.h $
 * $Revision: 1.2 $
 * $Date: 2011/11/14 13:33:51EST $
 *============================================================================*/
//! \file

#ifndef CPP_POINTER_FIFO_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define CPP_POINTER_FIFO_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "FifoBase.h"

class CBarcodeData;

/// CBarcodeFifo is a fifo for CBarcodeData pointers. Used by the drivers for sending.
class CBarcodeFifo : public CFifoBase
{
public:
	CBarcodeFifo(size_t size);
	virtual ~CBarcodeFifo();

	size_t Write(CBarcodeData *Ptr);
	CBarcodeData *Read(void);
	virtual void Purge(void);

protected:
	CBarcodeData **m_pBuffer;

private:
	CBarcodeFifo (const CBarcodeFifo&);            // declared private, but not defined, to prevent copying         
	CBarcodeFifo& operator=(const CBarcodeFifo&);  // declared private, but not defined, to prevent copying 

};

#endif /* CPP_POINTER_FIFO_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */


