/*=================================================================================
  class: CBarcodeData

//=================================================================================
   $Source: barcodedata.h $
   $Date: 2011/04/13 15:15:05EDT $
   $Revision:


//=================================================================================*/
//! \file

#ifndef BARCODE_DATA_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define BARCODE_DATA_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#include "ErrorDefs.h"
#include "language.h"


class HSTRING;
class HWSTRING;
class CManagedBuffer;

/// These are data types that can be stored in the CBarcodeData.
/// Most interface drivers are not be able to send all kinds of data. i.e. a keyboard can not send a jpg.
enum IoDataType_t
{
	IO_DATATYPE_UNKNOWN	= 0x00000000,
	IO_DATATYPE_8BIT		= 0x00000001,
	IO_DATATYPE_16BIT		= 0x00000002,
	IO_DATATYPE_32BIT		= 0x00000004,
#ifdef UNICODE
	IO_DATATYPE_8_16		= IO_DATATYPE_16BIT,
#else
	IO_DATATYPE_8_16		= IO_DATATYPE_8BIT,
#endif
	IO_DATATYPE_CHAR		= 0x00000010,		// all kind of characters (8 and 16 bit)
	IO_DATATYPE_CP_SYS	= 0x00000020|IO_DATATYPE_8BIT|IO_DATATYPE_CHAR,
	IO_DATATYPE_UNICODE	= 0x00000040|IO_DATATYPE_16BIT|IO_DATATYPE_CHAR,  // Unicode native (little) endian
	IO_DATATYPE_UTF8		= 0x00000100|IO_DATATYPE_8BIT|IO_DATATYPE_CHAR,   // Unicode in UTF8 encoding
	IO_DATATYPE_UTF16LES	= 0x00000200|IO_DATATYPE_8BIT|IO_DATATYPE_CHAR,  	// Unicode little endian as byte stream
	IO_DATATYPE_UTF16BES	= 0x00000400|IO_DATATYPE_8BIT|IO_DATATYPE_CHAR,  	// Unicode big endian as byte stream
	IO_DATATYPE_TEXT		= IO_DATATYPE_CP_SYS,
	IO_DATATYPE_CP_GEN	= IO_DATATYPE_8BIT|IO_DATATYPE_CHAR,

	IO_DATATYPE_BINARY	= 0x00001000|IO_DATATYPE_8BIT,		// plain binary of unknow structure
	IO_DATATYPE_ULONG		= 0x00002000|IO_DATATYPE_32BIT,
	IO_DATATYPE_INT		= 0x00004000|IO_DATATYPE_32BIT,
	IO_DATATYPE_HHLC		= 0x00008000|IO_DATATYPE_32BIT,

	IO_DATATYPE_JPEG		= 0x00010000|IO_DATATYPE_8BIT,
	IO_DATATYPE_TIFF		= 0x00020000|IO_DATATYPE_8BIT,
	IO_DATATYPE_BMP		= 0x00040000|IO_DATATYPE_8BIT,

	IO_DATATYPE_USER		= 0x80000000|IO_DATATYPE_8BIT		// application specific. Usage might be dangerous.
};

/// The CBarcodeData can contain other data structures.
enum IoContainerType_t
{
	IO_CONTAINER_NONE		= 0x00000001,	   // data is stored in the CBarcodeData object directly
	IO_CONTAINER_HSTRING	= 0x00000010,	   // Barry's excelent string class


	IO_CONTAINER_USER		= 0x80000000		// application specific. Usage might be dangerous.
};

///////////////////////////////////////////////////////////////////////////////
/// Storage space for all kind of decoded barcodes.
class CBarcodeData
{
	friend class CBarcodeManager;
// public methods
public:
	CBarcodeData(void);
	CBarcodeData(size_t size);	            // use this overload with care and if you know what you do
	virtual ~CBarcodeData();

	MatrixError_t IsValid(void);

	bool Free(void);
	bool IsFree(void)							{ return (!m_in_use);			}

	const CBarcodeData  &operator= (const CBarcodeData &rhs);
	const CBarcodeData  &operator= (const CBarcodeData *rhs)
	{
		return operator= (*rhs);
	}

	// replace the buffer with the buffer from the rhs object and free the rhs object
	void MoveBuffer (CBarcodeData &rhs);
	void MoveBuffer (CBarcodeData *rhs)
	{
		MoveBuffer (*rhs);
	}

	bchar_t ReportID();
#if(0)	                                 // would require a circular dependency with the ARM compiler
														// I keep the code for future reference
	// returns NULL if there is no buffer available. Also see GetSaveNewBuffer()
	template <typename TYPE>
	static CBarcodeData *GetNewBuffer(TYPE *&pData, size_t &NumElements)
	{
		CBarcodeData *pLabel = theBarcodeBufferManager.GetNewBuffer(pData, NumElements);
		return pLabel;
	}

	// never returns if there is no buffer (causes endless loop if nobody else frees buffers)
	template <typename TYPE>
	static CBarcodeData *GetSaveNewBuffer(TYPE *&pData, size_t &NumElements)
	{
		CBarcodeData *pLabel = NULL;
		while(pLabel == NULL)
		{
			pLabel = theBarcodeBufferManager.GetNewBuffer(pData, NumElements);
		};
		return pLabel;
	}
#endif
	static CBarcodeData *GetSaveNewBuffer(bchar_t *&pData, size_t &NumElements);
	static CBarcodeData *GetSaveNewBuffer(wchar_t *&pData, size_t &NumElements);
	static CBarcodeData *GetSaveNewBuffer(ULONG   *&pData, size_t &NumElements);
	static CBarcodeData *GetSaveNewBuffer(int *&pData, size_t &NumElements)
	{
		return GetSaveNewBuffer((ULONG *&)pData, NumElements);
	}

	static CBarcodeData *StoreToNewBuffer(const bchar_t *pData, size_t NumElements);
	static CBarcodeData *StoreToNewBuffer(const wchar_t *pData, size_t NumElements);
	static CBarcodeData *StoreToNewBuffer(const  char *pData, size_t NumElements)
	{
		return StoreToNewBuffer((const bchar_t *)pData, NumElements);
	}
//	static CBarcodeData *StoreToNewBuffer(const ULONG *pData, size_t NumElements);	perhaps later?

	// Contents type information
	// Here we keep track of what is inside the object
	void StoreAim(bchar_t id, bchar_t Modifier);
	void StoreHHP(bchar_t id, bchar_t Modifier=0);
	void StoreNonBarcodeIds(void);
	void StoreDataType(IoDataType_t NewType)	{ m_type = NewType;	}
	void SetCharacterData(void) 					{ m_type = IO_DATATYPE_CHAR;		}
	void SetSystemCodepageData(void) 			{ m_type = IO_DATATYPE_CP_SYS;	}
	void SetUnicodeData(void) 						{ m_type = IO_DATATYPE_UNICODE;	}

	bchar_t GetAimId(void)			{ return m_AIM_CodeLetter;		}
	bchar_t GetAimModifier(void)	{ return m_AIM_CodeModifier;	}
	bchar_t GetHHPId(void)			{ return m_HHP_CodeLetter;		}
	bchar_t GetHHPModifier(void)	{ return m_HHP_CodeModifier;	}

	void FillDataSourceInfo(wchar_t *pDataSourceInformation);  					// buffer must be at least 5 char long
	void FillDataSourceInfo(bchar_t *pDataSourceInformation);  					// buffer must be at least 5 char long
	void FillDataSourceInfo(char *pDataSourceInformation)  						// buffer must be at least 5 char long
	{
		FillDataSourceInfo((bchar_t *)pDataSourceInformation);
	}

	void CopyIDsFromDataSourceInfo(const wchar_t *pDataSourceInformation);  // buffer must be at least 5 char long
	void CopyIDsFromDataSourceInfo(const bchar_t *pDataSourceInformation);  // buffer must be at least 5 char long
	void CopyIDsFromDataSourceInfo(const char *pDataSourceInformation)	  	// buffer must be at least 5 char long
	{
		CopyIDsFromDataSourceInfo((const bchar_t *)pDataSourceInformation);
	}

	int GetControlMask(void)			{ return m_ControlMask;	}
	void SetControlMask(int newval)	{ m_ControlMask=newval;	}

	int GetAdditionalInterMessageDelay(void)			{ return m_AdditionalInterMessageDelay;	}
	void SetAdditionalInterMessageDelay(int newval)	{ m_AdditionalInterMessageDelay=newval;	}

	IoDataType_t GetDataType(void) 	{ return (IoDataType_t) m_type; 														}
	bool	IsCharacterData(void) 		{ return (GetDataType() & IO_DATATYPE_CHAR)	== IO_DATATYPE_CHAR;			}
	bool	IsSystemCodepageData(void) { return (GetDataType() & IO_DATATYPE_CP_SYS)  == IO_DATATYPE_CP_SYS;	}
	bool	IsUnicodeData(void)			{ return (GetDataType() & IO_DATATYPE_UNICODE) == IO_DATATYPE_UNICODE;	}
	bool	Is8BitData(void)				{ return (GetDataType() & IO_DATATYPE_8BIT) == IO_DATATYPE_8BIT;			}
	bool	Is16BitData(void)				{ return (GetDataType() & IO_DATATYPE_16BIT) == IO_DATATYPE_16BIT;		}
	bool	Is32BitData(void)				{ return (GetDataType() & IO_DATATYPE_32BIT) == IO_DATATYPE_32BIT;		}

	// copies only the contents info, not any contenst itself
	void CopyTypeInfo(const CBarcodeData &rhs);
	void CopyTypeInfo(const CBarcodeData *pRhs)
	{
		CopyTypeInfo(*pRhs);
	}

	// Accessing the buffer (read only). Various overloads.
	// Get size and buffer in one function
   void GetTotalData(const bchar_t *&pText, size_t &size);
	void GetTotalData(const wchar_t *&pText, size_t &Size);

	// Get size and buffer in two functions
	const tchar_t *GetTotalData(void);
	size_t GetLabelSize(void)			{ return m_NumElements;	}  // Lenght of the stored data in elements
//	size_t GetTotalSize(void)			{ return GetLabelSize();		}  // historically used name

	// Reading data out of the object with outomatic pointer handling.
	size_t BeginnReadingTotalData(void);
	size_t ReadNextTotalData(size_t position, bchar_t &Text);
	size_t ReadNextTotalData(size_t position, wchar_t &Text);

	// Use the functions below to get write access to the buffer.
	// After you are done with your writing, call the FinalizeBuffer member.
	// Get a writable buffer. Does no cleanup.
   bool GetContentsBuffer(bchar_t *&pText, size_t &size);
   bool GetContentsBuffer(wchar_t *&pText, size_t &size);

	void Fill(bchar_t val);

	// Get a writable buffer. Cleans the buffer.
	bool GetCleanBuffer(bchar_t *&pText, size_t &NumElements);
	bool GetCleanBuffer(wchar_t *&pText, size_t &NumElements);
	bool GetCleanBuffer(int *&pData, size_t &NumElements);

	bool GetCleanBuffer(char *&pData, size_t &NumElements)
	{
		return GetCleanBuffer((bchar_t *&) pData, NumElements);
	}

	// Call this after you are done with writing.
	bool FinalizeBuffer(size_t NumElements);
	bool FinalizeBuffer(size_t NumElements, IoDataType_t NewType);

	// Uses the type to find the element size
	// The next only checks (no resize)
	bool CheckFreeElements(size_t NumElements);

	// This one resizes if there is not enought space
	bool CanAddElements(size_t NumElements);  // uses the type to find the element size

	bool FinalizeAddingElements(size_t NumNewElements);	// adds the count to the number of elements

	bool MakeSpace(size_t position, size_t NumElements=1);
	// for now only an overload for char, we'll add later what we need
	bool MakeSpace(bchar_t *&pReturnInsertPos, size_t position, size_t NumElements=1);
	bool MakeSpace(wchar_t *&pReturnInsertPos, size_t position, size_t NumElements=1);
	// for now only an overload for char, we'll add later what we need
	bool InsertElements(const bchar_t *pInsert, size_t position, size_t NumElements=1);
	bool InsertElements(const wchar_t *pInsert, size_t position, size_t NumElements=1);
	// for now only an overload for char, we'll add later what we need
	bool GetAddrOfElement(bchar_t *&pReturnInsertPos, size_t position);
	bool GetAddrOfElement(wchar_t *&pReturnInsertPos, size_t position);


	// Store data functions. Various overloads.
   size_t StoreText(const bchar_t *pText, size_t size);  // store non-barcode text
   size_t StoreText(const wchar_t *pText, size_t size);  // store non-barcode text
   size_t StoreText(const char    *pText, size_t size)	{ return StoreText((const bchar_t *)pText, size);	}

   size_t AddText  (const bchar_t *pText, size_t size);  // append non-barcode text
   size_t AddText  (const wchar_t *pText, size_t size);  // append non-barcode text
   size_t AddText	 (const char    *pText, size_t size)	{ return AddText((const bchar_t *)pText, size);		}


	///////////////////////////////////////////
	// HSTRING interfaces:
	MatrixError_t	GetText(HSTRING &destination);
	MatrixError_t	GetText(HSTRING *pDestination)
	{
		return GetText(*pDestination);
	}
   size_t StoreText(HSTRING &source);
	///////////////////////////////////////////

	///////////////////////////////////////////
	// HWSTRING interfaces:
	MatrixError_t	GetWText(HWSTRING &destination);
	MatrixError_t	GetWText(HWSTRING *pDestination)
	{
		return GetWText(*pDestination);
	}
   size_t StoreWText(HWSTRING &source);
	///////////////////////////////////////////

	///////////////////////////////////////////
	// HHLC interfaces:
	MatrixError_t	GetHHLC(ULONG *&pDestination, size_t &NumberElements);
   size_t StoreHHLC(const ULONG *pSource, size_t NumberElements);
	///////////////////////////////////////////

	///////////////////////////////////////////
	// binary interfaces:
	MatrixError_t	GetBinary(const char *&pDestination, size_t &NumberElements);
   size_t StoreBinary(char*pSource, size_t NumberElements);

	MatrixError_t	GetBinary(const int *&pDestination, size_t &NumberElements);
	MatrixError_t	GetBinary(const ULONG *&pDestination, size_t &NumberElements);
	///////////////////////////////////////////

	#ifdef _DEBUG
	void DumpDebug(const tchar_t *pString);
	#else
	void DumpDebug(const tchar_t *)	{};
	#endif
// internal methods
protected:
	// busy flags
	void Use(void)								{ m_in_use=true; }

	// generic size accessors
	char *GetLabelBuffer_8Bit(void) const;		// returns a pointer to the allocated buffer
	USHORT *GetLabelBuffer_16Bit(void) const;	// returns a pointer to the allocated buffer
	ULONG *GetLabelBuffer_32Bit(void) const;	// returns a pointer to the allocated buffer

	size_t GetMaxElements_8Bit(void) const;	// returns the max number of elements that the buffer can hold
	size_t GetMaxElements_16Bit(void) const;	// returns the max number of elements that the buffer can hold
	size_t GetMaxElements_32Bit(void) const;	// returns the max number of elements that the buffer can hold

	// specific accessors
	tchar_t *GetLabelBuffer_tchar(void) const;// returns a pointer to the allocated buffer (tchar_t)
	wchar_t *GetLabelBuffer_wchar(void) const;// returns a pointer to the allocated buffer (wchar_t)
	bchar_t *GetLabelBuffer_bchar(void) const;// returns a pointer to the allocated buffer (bchar_t)
	char 	  *GetLabelBuffer_char(void) const; // returns a pointer to the allocated buffer (char)
	ULONG	  *GetLabelBuffer_ULONG(void) const;// returns a pointer to the allocated buffer (ULONG)
	int	  *GetLabelBuffer_int(void) const;  // returns a pointer to the allocated buffer (int)

	size_t GetMaxElements_tchar(void) const;	// returns the max number of elements that the buffer can hold
	size_t GetMaxElements_wchar(void) const;	// returns the max number of elements that the buffer can hold
	size_t GetMaxElements_bchar(void) const;	// returns the max number of elements that the buffer can hold
	size_t GetMaxElements_ULONG(void) const;	// returns the max number of elements that the buffer can hold
	size_t GetMaxElements_int(void) const;		// returns the max number of elements that the buffer can hold

	size_t GetLabelBufferSize(void) const;		// returns the size of the allocated buffer in bytes
	size_t GetUsedByteSize(void) const;			// returns the size of the currenty stored elements in bytes
	size_t GetUnUsedByteSize(void) const;		// returns the size of the free elements in bytes
	size_t GetByteSize(size_t NumberElements) const;// returns the size of the NumberElements elements in bytes
	size_t GetElementSize(void);					// get the size of one element

	void Reset(void);
	void AttachBuffer(CManagedBuffer *pBuffer);

	// helpers to prepare the data type
	void AttachBuffer(CManagedBuffer *pBuffer, const bchar_t *)
	{
		AttachBuffer(pBuffer);
		StoreDataType(IO_DATATYPE_8BIT);
	}

	void AttachBuffer(CManagedBuffer *pBuffer, const wchar_t *)
	{
		AttachBuffer(pBuffer);
		StoreDataType(IO_DATATYPE_16BIT);
	}

	void AttachBuffer(CManagedBuffer *pBuffer, const ULONG *)
	{
		AttachBuffer(pBuffer);
		StoreDataType(IO_DATATYPE_32BIT);
	}

	void AttachBuffer(CManagedBuffer *pBuffer, const char *)
	{
		AttachBuffer(pBuffer);
		StoreDataType(IO_DATATYPE_8BIT);
	}

// data members
public:
	bchar_t m_Code_ID;
protected:
	UINT		m_type;							//!< binary, characters and more
	IoContainerType_t	m_container;		//!< future: hstring, jpg and more

	bchar_t m_HHP_CodeLetter;           //!< The traditional Welch Allyn Code ID
	bchar_t m_HHP_CodeModifier;         //!< An extension to the Welch Allyn Code ID (symbology specific)

	bchar_t m_AIM_CodeLetter;           //!< The AIM Standard Code Identifier
	bchar_t m_AIM_CodeModifier;         //!< The AIM Standard Code Modifier character (symbology specific)

	// Allow the script interpreter a clean way to communicate to the driver.
	int m_AdditionalInterMessageDelay;	//!< Scripts can add a delay
	int m_ControlMask;						//!< used to send additional info from script to the rest.
													//!< CBarcodeData does not know any details about the meaning of this variable
private:
	volatile bool	m_in_use;				//!< object has not been sent yet
	size_t m_NumElements;					//!< Holds the length of the decoded data in characters (elements)
	bool	m_bBufferNeedsToBeDeleted;
	union UniPointer_t
	{
		CManagedBuffer *m_pMBuffer;
		HSTRING *m_pHString;
	};
	UniPointer_t m_UniPointer;
};


#include "BarcodeId.h"

#endif // BARCODE_DATA_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_





