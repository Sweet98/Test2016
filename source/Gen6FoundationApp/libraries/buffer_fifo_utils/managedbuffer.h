/*=================================================================================
  class:

//=================================================================================
   $Source: managedbuffer.h $
   $Date: 2011/11/14 13:33:48EST $
   $Revision:


//=================================================================================*/
//! \file


#ifndef BUFFER_H__87C38954_3CBA_4B39_B402_B11D0D8B0151__INCLUDED_
#define BUFFER_H__87C38954_3CBA_4B39_B402_B11D0D8B0151__INCLUDED_

/// A buffer that is managable by CBufferManager.
/// Storage space for all kind of data.
class CManagedBuffer
{
	enum
	{
		SafetyLow=0,
		BorderLow=1
	};
	// public methods
public:
	CManagedBuffer(size_t BufferSize);
	virtual ~CManagedBuffer();

	// busy flags
	bool Use(void)						{ m_in_use=true; return true;	}
	bool Free(void);
	bool IsFree(void)					{ return (!m_in_use);			}

	// checks if the security markers are still ok
	bool IsValid(void);

	// Use the functions below to get write access to the buffer.
	// Get a writable buffer. Does no cleanup.
	// Returns true if ok, pData points to the buffer and
	// size contains the size of the buffer in elements.
   bool GetBuffer(UCHAR *&pData, size_t &size);
   bool GetBuffer(USHORT *&pData, size_t &size);
   bool GetBuffer(ULONG *&pData, size_t &size);

	// simple base address accessors for various data types
	tchar_t *GetBuffer_tchar(void)
	{
		return (tchar_t*)m_pulUserBuffer;
	}

	wchar_t *GetBuffer_wchar(void)
	{
		return (wchar_t*)m_pulUserBuffer;
	}

	char *GetBuffer_char(void)
	{
		return (char*) m_pulUserBuffer;
	}

	bchar_t *GetBuffer_bchar(void)
	{
		return (bchar_t*) m_pulUserBuffer;
	}

	ULONG *GetBuffer_ULONG(void)
	{
		return m_pulUserBuffer;
	}

	int *GetBuffer_int(void)
	{
		return (int*)m_pulUserBuffer;
	}

	char *GetBuffer_8Bit(void)
	{
		return (char*)m_pulUserBuffer;
	}

	USHORT *GetBuffer_16Bit(void)
	{
		return (USHORT*)m_pulUserBuffer;
	}

	ULONG *GetBuffer_32Bit(void)
	{
		return (ULONG*)m_pulUserBuffer;
	}

	// simple size accessors for various data types
	// returns the max number of elements that the buffer can hold
	size_t GetMaxElements_tchar(void)
	{
		return m_user_buffer_size/sizeof(tchar_t);
	}

	size_t GetMaxElements_wchar(void)
	{
		return m_user_buffer_size/sizeof(wchar_t);
	}

	size_t GetMaxElements_bchar(void)
	{
		return m_user_buffer_size/sizeof(bchar_t);
	}

	size_t GetMaxElements_ULONG(void)
	{
		return m_user_buffer_size/sizeof(ULONG);
	}

	size_t GetMaxElements_int(void)
	{
		return m_user_buffer_size/sizeof(int);
	}

	size_t GetMaxElements_8Bit(void)
	{
		return m_user_buffer_size/sizeof(char);
	}

	size_t GetMaxElements_16Bit(void)
	{
		return m_user_buffer_size/sizeof(USHORT);
	}

	size_t GetMaxElements_32Bit(void)
	{
		return m_user_buffer_size/sizeof(ULONG);
	}

	// returns the size of the allocated buffer in bytes
	size_t GetBufferSize(void)
	{
		return m_user_buffer_size;
	}

// internal methods
protected:

// data members
protected:
	volatile bool	m_in_use;				// object has not been sent yet

private:
	// this is the pointer we've get from "new", we must store it to savely call "delete"
	ULONG *m_pulRawBuffer;					// The raw buffer of ULONGS to ensure alignment
	size_t m_total_buffer_size;         // the size of the allocated buffer in bytes

	ULONG *m_pulUserBuffer;					// The user buffer of ULONGS to ensure alignment
	size_t m_user_buffer_size;          // the size of the user buffer in bytes

	size_t m_IndexEndMarker;				// helps to find the security end marker

// debug and test helpers
public:
#ifdef _DEBUG_CBARCODE
	void DumpDebug(const tchar_t *pString, int freebuff);
	void DumpDebug(const char *pString, int freebuff)
	{
		DumpDebug((const tchar_t *)pString, freebuff);
	}
#else
	void DumpDebug(const tchar_t *, int )	{};
	void DumpDebug(const char *, int )		{};
#endif
private:
	CManagedBuffer (const CManagedBuffer&);            // declared private, but not defined, to prevent copying         
	CManagedBuffer& operator=(const CManagedBuffer&);  // declared private, but not defined, to prevent copying 

};


#endif // BUFFER_H__87C38954_3CBA_4B39_B402_B11D0D8B0151__INCLUDED_


