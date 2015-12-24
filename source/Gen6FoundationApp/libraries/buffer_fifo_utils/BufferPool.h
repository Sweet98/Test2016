/*=================================================================================
  class: CDataBufferMan

//=================================================================================
   $Source: BufferPool.h $
   $Date: 2011/11/14 13:33:49EST $
   $Revision:


//=================================================================================*/
//! \file

#ifndef BUFFER_POOL_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define BUFFER_POOL_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#include "errordefs.h"
#include "ManagedBuffer.h"
#include "thread.h"
class CPoolHelper;

#define FULL_IMAGE_BUFFER (540*1024)

/// Manage buffers in a pool.
class CBufferManager
{
public:
	CBufferManager(void);
	virtual ~CBufferManager(void);

	MatrixError_t IsValid(void);

	static void CreateBufferManager(void);
	static void DeleteBufferManager(void);

	CManagedBuffer *GetNewBuffer(size_t bytesize);  // find a buffer with a suitable size
   CManagedBuffer *GetNewBuffer(bchar_t *&pText, size_t &NumElements);
   CManagedBuffer *GetNewBuffer(wchar_t *&pText, size_t &NumElements);

	CManagedBuffer *GetNewBuffer(ULONG *&pData, size_t &NumElements);
	CManagedBuffer *GetNewBuffer(char *&pData, size_t &NumElements)
	{
		return GetNewBuffer((bchar_t*&) pData, NumElements);
	}
	CManagedBuffer *GetNewBuffer(int *&pData, size_t &NumElements)
	{
		return GetNewBuffer((ULONG*&) pData, NumElements);
	}

//	BOOL FreeBuffer(CManagedBuffer *Buffer);  // back to the pool
	void FreeAllBuffers(void);

// data members
private:
	CPoolHelper **m_pools;								//!< Point to the pool list
	size_t m_NumPools;									//!< Number of pools
	MT_Mutex_t poolmtx;				
	
	CBufferManager (const CBufferManager&);            // declared private, but not defined, to prevent copying         
	CBufferManager& operator=(const CBufferManager&);  // declared private, but not defined, to prevent copying 

};

extern   CBufferManager  	*g_pBufferManager;
/// The public CBufferManager object
#define  theBufferManager	(*g_pBufferManager)




#endif // BUFFER_POOL_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

