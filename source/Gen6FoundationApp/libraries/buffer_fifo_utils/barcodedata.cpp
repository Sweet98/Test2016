/*=================================================================================
  class: CBarcodeData
 
//=================================================================================
   $Source: barcodedata.cpp $
   $Date: 2010/02/01 15:13:19EST $
   $Revision: 1.8 $
 

//=================================================================================*/
//! \file

#include "stdInclude.h"
#include "interrupts.h"
#include "hstring.h"
#include "hwstring.h"
#include "errordefs.h"
#include "BarcodeData.h"
#include "BarcodeDataMan.h"
#include "BufferPool.h"
#include "Thread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////

// helpers to get the size in bytes from number of elements
inline size_t GetByteSize_8Bit(size_t NumElements)
{
	return NumElements*sizeof(UCHAR);
}
inline size_t GetByteSize_16Bit(size_t NumElements)
{
	return NumElements*sizeof(USHORT);
}
inline size_t GetByteSize_32Bit(size_t NumElements)
{
	return NumElements*sizeof(ULONG);
}

///////////////////////////////////////////////////////////////////////////////
// ctor
CBarcodeData::CBarcodeData(void)
{
	m_in_use = false;
	m_UniPointer.m_pMBuffer = NULL;
	m_bBufferNeedsToBeDeleted = false;
	Reset();
}

CBarcodeData::CBarcodeData(size_t size)
{
	m_in_use = false;
	m_UniPointer.m_pMBuffer = new CManagedBuffer(size);
	m_bBufferNeedsToBeDeleted = true;
	Reset();
}

CBarcodeData::~CBarcodeData()
{
	if(( m_UniPointer.m_pMBuffer != NULL ) && ( m_bBufferNeedsToBeDeleted ))
	{
		m_UniPointer.m_pMBuffer->Free();
		delete m_UniPointer.m_pMBuffer;
	}
}

void CBarcodeData::AttachBuffer(CManagedBuffer *pBuffer)
{
	m_UniPointer.m_pMBuffer = pBuffer;
	ASSERT(m_UniPointer.m_pMBuffer!=NULL);
}


// Get the typed address of buffer
inline tchar_t *CBarcodeData::GetLabelBuffer_tchar(void) const
{
	ASSERT(m_UniPointer.m_pMBuffer!=NULL);
	return m_UniPointer.m_pMBuffer->GetBuffer_tchar();
}

inline wchar_t *CBarcodeData::GetLabelBuffer_wchar(void) const
{
	ASSERT(m_UniPointer.m_pMBuffer!=NULL);
	return m_UniPointer.m_pMBuffer->GetBuffer_wchar();
}

inline char *CBarcodeData::GetLabelBuffer_char(void) const
{
	ASSERT(m_UniPointer.m_pMBuffer!=NULL);
	return m_UniPointer.m_pMBuffer->GetBuffer_char();
}

inline bchar_t *CBarcodeData::GetLabelBuffer_bchar(void) const
{
	ASSERT(m_UniPointer.m_pMBuffer!=NULL);
	return m_UniPointer.m_pMBuffer->GetBuffer_bchar();
}

inline ULONG *CBarcodeData::GetLabelBuffer_ULONG(void) const
{
	ASSERT(m_UniPointer.m_pMBuffer!=NULL);
	return m_UniPointer.m_pMBuffer->GetBuffer_ULONG();
}

inline int *CBarcodeData::GetLabelBuffer_int(void) const
{
	ASSERT(m_UniPointer.m_pMBuffer!=NULL);
	return m_UniPointer.m_pMBuffer->GetBuffer_int();
}

inline char *CBarcodeData::GetLabelBuffer_8Bit(void) const
{
	ASSERT(m_UniPointer.m_pMBuffer!=NULL);
	return m_UniPointer.m_pMBuffer->GetBuffer_8Bit();
}

inline USHORT *CBarcodeData::GetLabelBuffer_16Bit(void) const
{
	ASSERT(m_UniPointer.m_pMBuffer!=NULL);
	return m_UniPointer.m_pMBuffer->GetBuffer_16Bit();
}

inline ULONG *CBarcodeData::GetLabelBuffer_32Bit(void) const
{
	ASSERT(m_UniPointer.m_pMBuffer!=NULL);
	return m_UniPointer.m_pMBuffer->GetBuffer_32Bit();
}
// returns the max number of elemets that the buffer can hold
inline size_t CBarcodeData::GetMaxElements_tchar(void) const
{
	ASSERT(m_UniPointer.m_pMBuffer!=NULL);
	return m_UniPointer.m_pMBuffer->GetMaxElements_tchar();
}

inline size_t CBarcodeData::GetMaxElements_wchar(void) const
{
	ASSERT(m_UniPointer.m_pMBuffer!=NULL);
	return m_UniPointer.m_pMBuffer->GetMaxElements_wchar();
}

inline size_t CBarcodeData::GetMaxElements_bchar(void) const
{
	ASSERT(m_UniPointer.m_pMBuffer!=NULL);
	return m_UniPointer.m_pMBuffer->GetMaxElements_bchar();
}

inline size_t CBarcodeData::GetMaxElements_ULONG(void) const
{
	ASSERT(m_UniPointer.m_pMBuffer!=NULL);
	return m_UniPointer.m_pMBuffer->GetMaxElements_ULONG();
}

inline size_t CBarcodeData::GetMaxElements_int(void) const
{
	ASSERT(m_UniPointer.m_pMBuffer!=NULL);
	return m_UniPointer.m_pMBuffer->GetMaxElements_int();
}

inline size_t CBarcodeData::GetMaxElements_8Bit(void) const
{
	ASSERT(m_UniPointer.m_pMBuffer!=NULL);
	return m_UniPointer.m_pMBuffer->GetMaxElements_8Bit();
}

inline size_t CBarcodeData::GetMaxElements_16Bit(void) const
{
	ASSERT(m_UniPointer.m_pMBuffer!=NULL);
	return m_UniPointer.m_pMBuffer->GetMaxElements_16Bit();
}

inline size_t CBarcodeData::GetMaxElements_32Bit(void) const
{
	ASSERT(m_UniPointer.m_pMBuffer!=NULL);
	return m_UniPointer.m_pMBuffer->GetMaxElements_32Bit();
}

inline size_t CBarcodeData::GetLabelBufferSize(void) const
{
	ASSERT(m_UniPointer.m_pMBuffer!=NULL);
	return m_UniPointer.m_pMBuffer->GetBufferSize();
}

size_t CBarcodeData::GetUsedByteSize(void) const
{
	size_t ByteSize=0;
	if (m_type & IO_DATATYPE_8BIT)
	{
		ByteSize=m_NumElements*sizeof(UCHAR);
	}
	else if (m_type & IO_DATATYPE_16BIT)
	{
		ByteSize=m_NumElements*sizeof(USHORT);
	}
	else if (m_type & IO_DATATYPE_32BIT)
	{
		ByteSize=m_NumElements*sizeof(ULONG);
	}
	return ByteSize;
}

size_t CBarcodeData::GetUnUsedByteSize(void) const
{
	size_t ByteSize=0;
	if (m_type & IO_DATATYPE_8BIT)
	{
		ByteSize = GetMaxElements_8Bit() - m_NumElements*sizeof(UCHAR);
	}
	else if (m_type & IO_DATATYPE_16BIT)
	{
		ByteSize = GetMaxElements_16Bit() - m_NumElements*sizeof(USHORT);
	}
	else if (m_type & IO_DATATYPE_32BIT)
	{
		ByteSize = GetMaxElements_32Bit() - m_NumElements*sizeof(ULONG);
	}
	return ByteSize;
}

size_t CBarcodeData::GetByteSize(size_t NumElements) const
{
	size_t ByteSize=0;
	if (m_type & IO_DATATYPE_8BIT)
	{
		ByteSize = NumElements*sizeof(UCHAR);
	}
	else if (m_type & IO_DATATYPE_16BIT)
	{
		ByteSize = NumElements*sizeof(USHORT);
	}
	else if (m_type & IO_DATATYPE_32BIT)
	{
		ByteSize = NumElements*sizeof(ULONG);
	}
	return ByteSize;
}

void CBarcodeData::Reset(void)
{
	m_AdditionalInterMessageDelay = 0;
	m_ControlMask = 0;
	m_NumElements = 0;
	m_type = IO_DATATYPE_BINARY;
	m_container = IO_CONTAINER_NONE;
	// we need to call some kind of cleanup for the containers when we implement containers
	ASSERT(m_container == IO_CONTAINER_NONE);
	StoreNonBarcodeIds();
}

void CBarcodeData::StoreAim(bchar_t id, bchar_t Modifier)
{
	m_AIM_CodeLetter	= id;
	m_AIM_CodeModifier= Modifier;
}

void CBarcodeData::StoreHHP(bchar_t id, bchar_t Modifier)
{
	m_HHP_CodeLetter	 = id;
	m_HHP_CodeModifier= Modifier;
}

void CBarcodeData::StoreNonBarcodeIds(void)
{
	StoreAim(AIM_CODELETTER_OTHER, AIM_CODEMODIFIER_DEFAULT);
	StoreHHP('?', 0);	// 8888888 need to fix the letters !!!!!
}

void CBarcodeData::FillDataSourceInfo(bchar_t *pDataSourceInformation)
{
	pDataSourceInformation[0]=m_HHP_CodeLetter;
	pDataSourceInformation[1]=']';
	pDataSourceInformation[2]=m_AIM_CodeLetter ;
	pDataSourceInformation[3]=m_AIM_CodeModifier;
	pDataSourceInformation[4]=m_HHP_CodeModifier;
}

void CBarcodeData::FillDataSourceInfo(wchar_t *pDataSourceInformation)
{
	pDataSourceInformation[0]=m_HHP_CodeLetter;
	pDataSourceInformation[1]=']';
	pDataSourceInformation[2]=m_AIM_CodeLetter ;
	pDataSourceInformation[3]=m_AIM_CodeModifier;
	pDataSourceInformation[4]=m_HHP_CodeModifier;
}

void CBarcodeData::CopyIDsFromDataSourceInfo(const bchar_t *pDataSourceInformation)
{
	m_HHP_CodeLetter	= pDataSourceInformation[0];
	m_AIM_CodeLetter	= pDataSourceInformation[2];
	m_AIM_CodeModifier= pDataSourceInformation[3];
	m_HHP_CodeModifier= pDataSourceInformation[4];
}

void CBarcodeData::CopyIDsFromDataSourceInfo(const wchar_t *pDataSourceInformation)
{
	m_HHP_CodeLetter	= (bchar_t) pDataSourceInformation[0]&0xff;
	m_AIM_CodeLetter	= (bchar_t) pDataSourceInformation[2]&0xff;
	m_AIM_CodeModifier= (bchar_t) pDataSourceInformation[3]&0xff;
	m_HHP_CodeModifier= (bchar_t) pDataSourceInformation[4]&0xff;
}

void CBarcodeData::CopyTypeInfo(const CBarcodeData &rhs)
{
	ASSERT(m_container == IO_CONTAINER_NONE);
	m_AdditionalInterMessageDelay = rhs.m_AdditionalInterMessageDelay;
	m_ControlMask	= rhs.m_ControlMask;
	m_type 			= rhs.m_type;
	m_container		= rhs.m_container;
	m_AIM_CodeLetter	= rhs.m_AIM_CodeLetter;
	m_AIM_CodeModifier= rhs.m_AIM_CodeModifier;
	m_HHP_CodeLetter	= rhs.m_HHP_CodeLetter;
	m_HHP_CodeModifier= rhs.m_HHP_CodeModifier;
}

void CBarcodeData::Fill(bchar_t val)
{
	memset(GetLabelBuffer_char(), val, GetLabelBufferSize());
}

const CBarcodeData  &CBarcodeData::operator= (const CBarcodeData &rhs)
{
	if(this != &rhs)	                  		// prevent selfcopy
	{
		size_t ByteSize = rhs.GetUsedByteSize();
		if(ByteSize <= GetLabelBufferSize())	// prevent buffer overflow
		{
			if(m_container == rhs.m_container)  // only support non container types
			{
				ASSERT(m_container == IO_CONTAINER_NONE);
				// CAUTION: do not touch the m_in_use !!!
				m_NumElements	= rhs.m_NumElements;
				CopyTypeInfo(rhs);
				memcpy(GetLabelBuffer_char(), rhs.GetLabelBuffer_char(), ByteSize);
			}
		}
	}
	return *this;
}

void CBarcodeData::MoveBuffer (CBarcodeData &rhs)
{
	if(this != &rhs)	                  		// prevent selfcopy
	{
		if(m_container == rhs.m_container)  // only support non container types
		{
			ASSERT(m_container == IO_CONTAINER_NONE);
			// CAUTION: do not touch the m_in_use !!!

			// excange the pointers to the buffer, so the later rhs.Free() does free our current buffer.
			UniPointer_t pTemp = m_UniPointer;	// save it
			m_UniPointer = rhs.m_UniPointer;		// point to new buffer
			rhs.m_UniPointer = pTemp;				// final step of excange

			m_NumElements	= rhs.m_NumElements;	// also use the new size
			m_type 			= rhs.m_type;			// and type
			m_container		= rhs.m_container;	// and container type (future)
#ifdef _DEBUG

			m_UniPointer.m_pMBuffer->DumpDebug(_T("MoveBuffer  "), 0);
#endif

			rhs.Free();
		}
	}
#ifdef _DEBUG
	else
	{
		int test=0;
	}
#endif
}

size_t CBarcodeData::StoreText(const bchar_t *pText, size_t size)
{
	if (m_container == IO_CONTAINER_NONE)
	{
		m_type = IO_DATATYPE_CP_SYS;	// type is used by CanAddElements, store it first
		m_container = IO_CONTAINER_NONE;
		bool CanAdd = CanAddElements(size);
		if(CanAdd)
		{
			size_t ByteSize=size*sizeof(bchar_t);
			char *pSource = (char*)pText;
			bchar_t *pTemp = GetLabelBuffer_bchar();
			char *pDestination = (char*)pTemp;
			memcpy(pDestination, pSource, ByteSize);
			GetLabelBuffer_bchar()[size] = 0;	// add a terminating 0
			m_NumElements = size;
			return size;
		}
	}
	return 0;
}

size_t CBarcodeData::StoreText(const wchar_t *pText, size_t size)
{
	if (m_container == IO_CONTAINER_NONE)
	{
		m_type = IO_DATATYPE_UNICODE;	// type is used by CanAddElements, store it first
		m_container = IO_CONTAINER_NONE;
		bool CanAdd = CanAddElements(size);
		if(CanAdd)
		{
			size_t ByteSize=size*sizeof(wchar_t);
			char *pSource = (char*)pText;
			wchar_t *pTemp = GetLabelBuffer_wchar();
			char *pDestination = (char*)pTemp;
			memcpy(pDestination, pSource, ByteSize);
			GetLabelBuffer_wchar()[size] = 0;	// add a terminating 0
			m_NumElements = size;
			return size;
		}
	}
	return 0;
}

size_t CBarcodeData::AddText(const bchar_t *pText, size_t size)
{
	// check if we contain the correct data type
	if ( (m_container == IO_CONTAINER_NONE) && Is8BitData() )
	{
		if(CanAddElements(size))
		{
			memcpy(GetLabelBuffer_bchar()+m_NumElements, pText, size*sizeof(bchar_t));
			m_NumElements += size;
			GetLabelBuffer_bchar()[m_NumElements] = 0;	// add a terminating 0
			return size;
		}
	}
	return 0;
}

size_t CBarcodeData::AddText(const wchar_t *pText, size_t size)
{
	// check if we contain the correct data type
	if ( (m_container == IO_CONTAINER_NONE) && Is16BitData() )
	{
		if(CanAddElements(size))
		{
			memcpy(GetLabelBuffer_wchar()+m_NumElements, pText, size*sizeof(wchar_t));
			m_NumElements += size;
			GetLabelBuffer_wchar()[m_NumElements] = 0;	// add a terminating 0
			return size;
		}
	}
	return 0;
}

// Get buffer without cleanup
bool CBarcodeData::GetContentsBuffer(wchar_t *&pText, size_t &size)
{
	bool RetVal = ERR_FAIL;
	// check if we contain the correct data type
	if ( (m_container == IO_CONTAINER_NONE) && Is16BitData() )
	{
		pText = GetLabelBuffer_wchar();
		size  = m_NumElements;
		RetVal = ERR_PASS;
	}
	return RetVal;
}

bool CBarcodeData::GetContentsBuffer(bchar_t *&pText, size_t &size)
{
	bool RetVal = ERR_FAIL;
	// check if we contain the correct data type
	if ( (m_container == IO_CONTAINER_NONE) && Is8BitData() )
	{
		pText = GetLabelBuffer_bchar();
		size  = m_NumElements;
		RetVal = ERR_PASS;
	}
	return RetVal;
}

// Get buffer of tchar_t with cleanup
bool CBarcodeData::GetCleanBuffer(wchar_t *&pText, size_t &NumElements)
{
	Reset();
	pText = GetLabelBuffer_wchar();
	NumElements  = GetMaxElements_wchar();
	m_type = IO_DATATYPE_UNICODE;
	return ERR_PASS;
}

bool CBarcodeData::GetCleanBuffer(bchar_t *&pText, size_t &NumElements)
{
	Reset();
	pText = GetLabelBuffer_bchar();
	NumElements  = GetMaxElements_bchar();
	m_type = IO_DATATYPE_CP_SYS;
	return ERR_PASS;
}

// Get buffer of int with cleanup
bool CBarcodeData::GetCleanBuffer(int *&pData, size_t &NumElements)
{
	Reset();
	pData = GetLabelBuffer_int();
	NumElements  = GetMaxElements_int();
	m_type = IO_DATATYPE_BINARY|IO_DATATYPE_INT;
	return ERR_PASS;
}

// return buffer back to the responsibilty of the CBarcodeData
// We also ensure the max size and add a terminating 0
bool CBarcodeData::FinalizeBuffer(size_t NumElements)
{
	size_t MaxElements;
	if (m_type & IO_DATATYPE_8BIT)
	{
		MaxElements = GetMaxElements_8Bit();
		if(NumElements > MaxElements)
			NumElements=MaxElements;
		GetLabelBuffer_bchar()[NumElements] = 0;
	}
	else if (m_type & IO_DATATYPE_16BIT)
	{
		MaxElements = GetMaxElements_16Bit();
		if(NumElements > MaxElements)
			NumElements=MaxElements;
		GetLabelBuffer_wchar()[NumElements] = 0;
	}
	else if (m_type & IO_DATATYPE_32BIT)
	{
		MaxElements = GetMaxElements_32Bit();
		if(NumElements > MaxElements)
			NumElements=MaxElements;
		GetLabelBuffer_int()[NumElements] = 0;
	}

	m_NumElements = NumElements;
	return ERR_PASS;
}

// return buffer back to the responsibilty of the CBarcodeData
bool CBarcodeData::FinalizeBuffer(size_t NumElements,  IoDataType_t NewType)
{
	m_type = NewType;
	return FinalizeBuffer(NumElements);
}

// read-only access (pointer and size)
void CBarcodeData::GetTotalData(const bchar_t *&pText, size_t &Size)
{
	// check if we contain the correct data type
	if ( (m_container == IO_CONTAINER_NONE) && Is8BitData() )
	{
		pText = GetLabelBuffer_bchar();
		Size  = GetLabelSize();
	}
	else
	{
		Size = 0;		                     // bad type
	}
}
// Various overloads to get a fresh CBarcode object in a save way.
// These functions do not return if there is no object available (out of buffer etc.)
CBarcodeData *CBarcodeData::GetSaveNewBuffer(bchar_t *&pData, size_t &NumElements)
{
	CBarcodeData *pLabel = NULL;
	while(true)
	{
		pLabel = theBarcodeBufferManager.GetNewBuffer(pData, NumElements);
		if (pLabel!=NULL)
			break;		
		MT_Delay(1);
	}
	return pLabel;
}

CBarcodeData *CBarcodeData::GetSaveNewBuffer(wchar_t *&pData, size_t &NumElements)
{
	CBarcodeData *pLabel = NULL;
	while(true)
	{
		pLabel = theBarcodeBufferManager.GetNewBuffer(pData, NumElements);
		if (pLabel!=NULL)
		{
			break;
		}	
		MT_Delay(1);
	}
	return pLabel;
}

CBarcodeData *CBarcodeData::GetSaveNewBuffer(ULONG *&pData, size_t &NumElements)
{
	CBarcodeData *pLabel = NULL;
	while(true)
	{
		pLabel = theBarcodeBufferManager.GetNewBuffer(pData, NumElements);
		if (pLabel!=NULL)
		{
			break;
		}	
		MT_Delay(1);
	}
	return pLabel;
}
// Static function gets a new object and copies the data into it.
// Uses the GetSaveNewBuffer.
CBarcodeData *CBarcodeData::StoreToNewBuffer(const bchar_t *pData, size_t NumElements)
{
	bchar_t *pTemp;
	size_t TempElements = NumElements;
	CBarcodeData *pLabel = GetSaveNewBuffer(pTemp, TempElements);
	pLabel->StoreText(pData, NumElements);

	return pLabel;
}

CBarcodeData *CBarcodeData::StoreToNewBuffer(const wchar_t *pData, size_t NumElements)
{
	wchar_t *pTemp;
	size_t TempElements = NumElements;
	CBarcodeData *pLabel = GetSaveNewBuffer(pTemp, TempElements);
	pLabel->StoreText(pData, NumElements);

	return pLabel;

}


// read-only access (pointer and size)
void CBarcodeData::GetTotalData(const wchar_t *&pText, size_t &Size)
{
	// check if we contain the correct data type
	if ( (m_container == IO_CONTAINER_NONE) && Is16BitData() )
	{
		pText = GetLabelBuffer_wchar();
		Size  = GetLabelSize();
	}
	else
	{
		Size = 0;		                     // bad type
	}
}

// read-only access (pointer)
const tchar_t *CBarcodeData::GetTotalData(void)
{
	return GetLabelBuffer_tchar();
}

// read-only access with automated functions
size_t CBarcodeData::BeginnReadingTotalData(void)
{
	return 0;
}

// read byte for byte
size_t CBarcodeData::ReadNextTotalData(size_t position, bchar_t &Text)
{
	// check if we contain the correct data type
	if ( (m_container == IO_CONTAINER_NONE) && IsCharacterData() )
	{
		if(position >= GetLabelSize())
			return 0;
		Text = GetLabelBuffer_bchar()[position];
		return position+1;
	}
	else
	{
		return 0;	                        // bad type !
	}
}

// read byte for byte
size_t CBarcodeData::ReadNextTotalData(size_t position, wchar_t &Text)
{
	// check if we contain the correct data type
	if ( (m_container == IO_CONTAINER_NONE) && IsCharacterData() )
	{
		if(position >= GetLabelSize())
			return 0;
		Text = GetLabelBuffer_wchar()[position];
		return position+1;
	}
	else
	{
		return 0;	                        // bad type !
	}
}

bool CBarcodeData::Free(void)
{
	// allow a save call with a NULL pointer
	if(this> (void *)0x100)			//TODO: jaw  fix this this is a cheat that prevents compiler assuming pointer always non null
	{
		if(m_UniPointer.m_pMBuffer!=NULL)
		{
			switch(m_container)
			{
			case IO_CONTAINER_NONE:
				m_UniPointer.m_pMBuffer->Free();
				break;
			case IO_CONTAINER_HSTRING:
				delete m_UniPointer.m_pHString;
				break;
			default:
				break;
			}
			m_UniPointer.m_pMBuffer = 0;
		}
		DISABLE_INTERRUPTS();
		m_in_use=false;
		RESTORE_INTERRUPTS();
	}
	return ERR_PASS;
}

size_t CBarcodeData::GetElementSize(void)
{
	size_t RetVal = sizeof(UCHAR);	      // default to IO_DATATYPE_8BIT
	if (m_container == IO_CONTAINER_NONE)
	{
		if (m_type & IO_DATATYPE_16BIT)
		{
			RetVal = sizeof(USHORT);
		}
		else if (m_type & IO_DATATYPE_32BIT)
		{
			RetVal = sizeof(ULONG);
		}
	}

	return RetVal;
}

bool CBarcodeData::CheckFreeElements(size_t NumElements)
{
	bool RetVal = ERR_FAIL;
	if (m_container == IO_CONTAINER_NONE)
	{
		if (m_type & IO_DATATYPE_8BIT)
		{
			RetVal = (GetLabelSize() + NumElements) < GetMaxElements_8Bit();
		}
		else if (m_type & IO_DATATYPE_16BIT)
		{
			RetVal = (GetLabelSize() + NumElements) < GetMaxElements_16Bit();
		}
		else if (m_type & IO_DATATYPE_32BIT)
		{
			RetVal = (GetLabelSize() + NumElements) < GetMaxElements_32Bit();
		}
	}

	return RetVal;
}

// This function now automatically increases the buffer space
bool CBarcodeData::CanAddElements(size_t NumElements)
{
	bool RetVal = CheckFreeElements(NumElements);
	if(RetVal==ERR_FAIL)
	{
		if (m_container == IO_CONTAINER_NONE)
		{
			size_t CurrentByteSize = GetLabelSize() * GetElementSize();
			size_t NewByteSize = CurrentByteSize + (NumElements * GetElementSize());
			CManagedBuffer *pBuffer = theBufferManager.GetNewBuffer(NewByteSize);	// byte size
			if(pBuffer != NULL)
			{
				// copy contents to new buffer
				memcpy(pBuffer->GetBuffer_char(), GetLabelBuffer_char(), CurrentByteSize);
				ASSERT(m_UniPointer.m_pMBuffer!=NULL);
				m_UniPointer.m_pMBuffer->Free();	// free current buffer...
				AttachBuffer(pBuffer);				// and attach the new buffer
				RetVal = true;
			}
		}
	}
	return RetVal;
}

// use this if you add elements manually
bool CBarcodeData::FinalizeAddingElements(size_t NumNewElements)
{
	bool RetVal = CheckFreeElements(NumNewElements);
	if(RetVal==true)
		m_NumElements += NumNewElements;

	return RetVal;
}

// move elements towards the end to make space for insertion
bool CBarcodeData::MakeSpace(size_t position, size_t NumElements)
{
	bool RetVal = false;
	char *pDestination;
	char *pSource;
	size_t MoveByteSize=0;
	if ( (m_container == IO_CONTAINER_NONE) && (NumElements != 0) )
	{
		RetVal = CanAddElements(NumElements);
		if (m_type & IO_DATATYPE_8BIT)
		{
			pSource = (char*)&GetLabelBuffer_8Bit()[position];
			pDestination = (char*)&GetLabelBuffer_8Bit()[position+NumElements];
			MoveByteSize = (GetLabelSize() - position) *sizeof(bchar_t);
		}
		else if (m_type & IO_DATATYPE_16BIT)
		{
			pSource = (char*)&GetLabelBuffer_16Bit()[position];
			pDestination = (char*)&GetLabelBuffer_16Bit()[position+NumElements];
			MoveByteSize = (GetLabelSize() - position) *sizeof(USHORT);
		}
		else if (m_type & IO_DATATYPE_32BIT)
		{
			pSource = (char*)&GetLabelBuffer_32Bit()[position];
			pDestination = (char*)&GetLabelBuffer_32Bit()[position+NumElements];
			MoveByteSize = (GetLabelSize() - position) *sizeof(ULONG);
		}
		else
		{
			RetVal = false;	                  // wrong data type
		}

		if(RetVal == true)
		{
			memmove(pDestination, pSource, MoveByteSize);
			m_NumElements += NumElements;
		}
	}

	return RetVal;
}

bool CBarcodeData::MakeSpace(bchar_t *&pReturnInsertPos, size_t position, size_t NumElements)
{
	bool RetVal = ERR_FAIL;
	RetVal = MakeSpace(position, NumElements);
	if(RetVal == ERR_PASS)
	{
		pReturnInsertPos = &GetLabelBuffer_bchar()[position];
	}

	return RetVal;
}

bool CBarcodeData::MakeSpace(wchar_t *&pReturnInsertPos, size_t position, size_t NumElements)
{
	bool RetVal = ERR_FAIL;
	RetVal = MakeSpace(position, NumElements);
	if(RetVal == ERR_PASS)
	{
		pReturnInsertPos = &GetLabelBuffer_wchar()[position];
	}

	return RetVal;
}

bool CBarcodeData::GetAddrOfElement(bchar_t *&pReturnInsertPos, size_t position)
{
	bool RetVal = ERR_FAIL;
	if (m_container == IO_CONTAINER_NONE)
	{
		if (position > GetLabelSize())
		{
			position = GetLabelSize();	      // return the next available element
		}
		if (m_type & IO_DATATYPE_8BIT)
		{
			pReturnInsertPos = &GetLabelBuffer_bchar()[position];
			RetVal = ERR_PASS;
		}
	}

	return RetVal;
}

bool CBarcodeData::GetAddrOfElement(wchar_t *&pReturnInsertPos, size_t position)
{
	bool RetVal = ERR_FAIL;
	if (m_container == IO_CONTAINER_NONE)
	{
		if (position > GetLabelSize())
		{
			position = GetLabelSize();	      // return the next available element
		}
		if (m_type & IO_DATATYPE_16BIT)
		{
			pReturnInsertPos = &GetLabelBuffer_wchar()[position];
			RetVal = ERR_PASS;
		}
	}

	return RetVal;
}

bool CBarcodeData::InsertElements(const bchar_t *pInsert, size_t position, size_t NumElements)
{
	bool RetVal = ERR_FAIL;
	bchar_t *pDestination=NULL;
	RetVal = MakeSpace(pDestination, position, NumElements);
	if(RetVal == ERR_PASS)
	{
		// now insert the data
		size_t InsertByteSize=NumElements*GetElementSize();
		memcpy(pDestination, pInsert, InsertByteSize);
	}
	return RetVal;
}

bool CBarcodeData::InsertElements(const wchar_t *pInsert, size_t position, size_t NumElements)
{
	bool RetVal = ERR_FAIL;
	wchar_t *pDestination=NULL;
	RetVal = MakeSpace(pDestination, position, NumElements);
	if(RetVal == ERR_PASS)
	{
		// now insert the data
		size_t InsertByteSize=NumElements*GetElementSize();
		memcpy(pDestination, pInsert, InsertByteSize);
	}
	return RetVal;
}

///////////////////////////////////////////
// HSTRING interfaces:
MatrixError_t	CBarcodeData::GetText(HSTRING &destination)
{
	MatrixError_t RetVal = ERR_PASS;
	if (m_container == IO_CONTAINER_NONE)
	{
		if (!destination.Copy(GetLabelBuffer_bchar(), GetLabelSize()))  // UNIFIX caution, might need fix after Barry changed HSTRING to wide
		{
			RetVal = ERR_FAIL;
		}
	}
	else
	{
		RetVal = ERR_WRONG_DATA_TYPE;
	}
	return RetVal;
}

size_t CBarcodeData::StoreText(HSTRING &source)
{
	size_t size = source.Size;
	bchar_t *pTemp = source.Char;    // UNIFIX caution, might need fix after Barry changed HSTRING to wide
	return StoreText(pTemp, size);
}

///////////////////////////////////////////
///////////////////////////////////////////
// HWSTRING interfaces:
MatrixError_t	CBarcodeData::GetWText(HWSTRING &destination)
{
	MatrixError_t RetVal = ERR_PASS;
	if (m_container == IO_CONTAINER_NONE)
	{
		if (!destination.Copy(GetLabelBuffer_wchar(), GetLabelSize()))
		{
			RetVal = ERR_FAIL;
		}
	}
	else
	{
		RetVal = ERR_WRONG_DATA_TYPE;
	}
	return RetVal;
}

size_t CBarcodeData::StoreWText(HWSTRING &source)
{
	size_t size = source.Size;
	wchar_t *pTemp = source.WChar;
	return StoreText(pTemp, size);
}

///////////////////////////////////////////

///////////////////////////////////////////
// HHLC interfaces:
MatrixError_t	CBarcodeData::GetHHLC(ULONG *&pDestination, size_t &NumberElements)
{
	MatrixError_t RetVal = ERR_PASS;
	if ( (m_container == IO_CONTAINER_NONE) && (m_type & IO_DATATYPE_HHLC) )
	{
		pDestination	= GetLabelBuffer_ULONG();
		NumberElements = GetLabelSize();
	}
	else
	{
		NumberElements = 0;
		RetVal = ERR_WRONG_DATA_TYPE;
	}
	return RetVal;
}

size_t CBarcodeData::StoreHHLC(const ULONG *pSource, size_t NumberElements)
{
	if (m_container == IO_CONTAINER_NONE)
	{
		m_type = IO_DATATYPE_HHLC;	// type is used by CanAddElements, store it first
		bool CanAdd = CanAddElements(NumberElements);
		if(CanAdd)
		{
			size_t ByteSize = NumberElements*sizeof(ULONG);
			memcpy(GetLabelBuffer_char(), (char*) pSource, ByteSize);
			m_NumElements = NumberElements;
			StoreNonBarcodeIds();
			return NumberElements;
		}
	}
	m_NumElements =0;
	return m_NumElements;
}

///////////////////////////////////////////

///////////////////////////////////////////
// binary interfaces:
MatrixError_t	CBarcodeData::GetBinary(const char *&pDestination, size_t &NumberElements)
{
	MatrixError_t RetVal = ERR_PASS;
	if ( (m_container == IO_CONTAINER_NONE) && (m_type & IO_DATATYPE_BINARY) )
	{
		pDestination	= GetLabelBuffer_char();
		NumberElements = GetLabelSize();
	}
	else
	{
		NumberElements = 0;
		RetVal = ERR_WRONG_DATA_TYPE;
	}
	return RetVal;
}

size_t CBarcodeData::StoreBinary(char *pSource, size_t NumberElements)
{
	if (m_container == IO_CONTAINER_NONE)
	{
		m_type = IO_DATATYPE_BINARY;	// type is used by CanAddElements, store it first
		bool CanAdd = CanAddElements(NumberElements);
		if(CanAdd)
		{
			size_t ByteSize = NumberElements*sizeof(char);
			memcpy(GetLabelBuffer_char(), pSource, ByteSize);
			m_NumElements = NumberElements;
			StoreNonBarcodeIds();
			return NumberElements;
		}
	}

	m_NumElements =0;
	return m_NumElements;
}

MatrixError_t	CBarcodeData::GetBinary(const int *&pDestination, size_t &NumberElements)
{
	MatrixError_t RetVal = ERR_PASS;
	if ( (m_container == IO_CONTAINER_NONE) && (m_type & IO_DATATYPE_INT) )
	{
		pDestination	= GetLabelBuffer_int();
		NumberElements = GetLabelSize();
	}
	else
	{
		NumberElements = 0;
		RetVal = ERR_WRONG_DATA_TYPE;
	}
	return RetVal;
}

MatrixError_t	CBarcodeData::GetBinary(const ULONG *&pDestination, size_t &NumberElements)
{
	MatrixError_t RetVal = ERR_PASS;
	if ( (m_container == IO_CONTAINER_NONE) && (m_type & IO_DATATYPE_ULONG) )
	{
		pDestination	= GetLabelBuffer_ULONG();
		NumberElements = GetLabelSize();
	}
	else
	{
		NumberElements = 0;
		RetVal = ERR_WRONG_DATA_TYPE;
	}
	return RetVal;
}


#ifdef _DEBUG_CBARCODE
extern volatile int g_freebuff;

void CBarcodeData::DumpDebug(const tchar_t *pString)
{
	switch(m_container)
	{
	case IO_CONTAINER_NONE:
		m_UniPointer.m_pMBuffer->DumpDebug(pString, g_freebuff);
		break;
	case IO_CONTAINER_HSTRING:
		break;
	default:
		break;
	}
}
#endif

MatrixError_t CBarcodeData::IsValid(void)
{
	MatrixError_t RetVal = ERR_PASS;
	return RetVal;
}


bchar_t CBarcodeData::ReportID()
{
	m_Code_ID = m_HHP_CodeLetter;
	return m_Code_ID;
}
///////////////////////////////////////////




