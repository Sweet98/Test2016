/*=================================================================================
	class: CTransCode
	Translate byte characters into Unicode and vice versa
//=================================================================================
   $Source: Gen6FoundationApp/FoundationApp/eci/transcode.h $
   $Date: 2011/11/08 18:02:46EST $
   $Revision: 1.2 $

//=================================================================================*/
//! \file

#ifndef TRANSCODE_H__03164A21_6A2C_443B_BCD9_B1B934C58593__INCLUDED_
#define TRANSCODE_H__03164A21_6A2C_443B_BCD9_B1B934C58593__INCLUDED_

#include "codepage.h"
struct TranslateItemUnicode_t;

///////////////////////////////////////////////////////////////////////////////
/// This is a helper, so we can expand to "multi byte" codepages later.
/*! It also makes the higher level much simpler.
 To add a new codepage type, derive a specific class and fill the virtual functions with live.
	
*/
class CTransInfoBase
{
protected:
	friend class CTransCode;

	CTransInfoBase(size_t size, codepage_t codepage);
	virtual ~CTransInfoBase(void);

	enum
	{
		FL_7Bit			= 0x0001,
		FL_8Bit			= 0x0002,
		FL_MULTIBYTE	= 0x0004,				// future expansion (Chinese, Japanese,...)
		FL_DUMMY			= 0
	};													//!< constants used by this class

	bool Is7Bit(void)	{ return m_capabilities&FL_7Bit ? true : false;	}			//!< Check codepage type
	bool Is8Bit(void)	{ return m_capabilities&FL_8Bit ? true : false;	}			//!< Check codepage type
	bool IsMBCS(void)	{ return m_capabilities&FL_MULTIBYTE ? true : false;	}	//!< Check codepage type
	codepage_t GetCurrentCodepage(void)	{ return m_codepage;	};

	void ToUnicodeChar(const bchar_t *pIn, wchar_t *pOut);

	wchar_t	*m_pTable;							//!< translation table
	UINT		m_capabilities;					//!< flags and hints
	size_t	m_TableSize;                  //!< size of the translation table
private:
	size_t	m_InternalTableSize;	         //!< size of the table we create ourselves
	CTransInfoBase (const CTransInfoBase&);            // declared private, but not defined, to prevent copying         
	CTransInfoBase& operator=(const CTransInfoBase&);  // declared private, but not defined, to prevent copying 

protected:
	codepage_t m_codepage;						//!< The codepage used here

	virtual MatrixError_t ToUnicode(const bchar_t *pIn, size_t length, wchar_t *pOut, size_t &DestinationSize);
	virtual MatrixError_t ToUnicode(const bchar_t *pIn, wchar_t *pOut, size_t &DestinationSize);
	virtual MatrixError_t FromUnicode(const wchar_t *pIn, bchar_t *pOut, size_t &DestinationSize) = 0;
	virtual MatrixError_t FromUnicode(const wchar_t *pIn, size_t length, bchar_t *pOut, size_t &DestinationSize) = 0;

	virtual bool TranslateInPlace(bchar_t *pIn, size_t length);
};


///////////////////////////////////////////////////////////////////////////////
/// A class that can translate buffers to a different codepage.
/*! It serves as a universal base class for codepage translations.
	Most functions copy the translated result into a new buffer, but some can do it inplace.
*/
class CTransCode
{
public:
	CTransCode(void);
	~CTransCode(void);

	void 	OnChangedConfiguration(void);	// gets called by the framework after a menu change

	MatrixError_t CharacterToUnicode(codepage_t codepage, bchar_t in, wchar_t &out);
	MatrixError_t SzStringToUnicode(codepage_t codepage, const bchar_t *pIn, wchar_t *pOut, size_t &DestinationSize);
	MatrixError_t StringToUnicode(codepage_t codepage, const bchar_t *pIn, size_t length, wchar_t *pOut, size_t &DestinationSize);

	MatrixError_t UnicodeToSingleByte(codepage_t codepage, wchar_t in, bchar_t &out);
	MatrixError_t UnicodeToSzString(codepage_t codepage, const wchar_t *pIn, bchar_t *pOut, size_t &DestinationSize);
	MatrixError_t UnicodeToString(codepage_t codepage, const wchar_t *pIn, size_t length, bchar_t *pOut, size_t &DestinationSize);

	wchar_t SystemCPCharacterToUnicode(bchar_t in);
	MatrixError_t SystemCPCharacterToUnicode(bchar_t in, wchar_t &out);
	MatrixError_t SzSystemCPStringToUnicode(const bchar_t *pIn, wchar_t *pOut, size_t &DestinationSize);
	MatrixError_t SystemCPStringToUnicode(const bchar_t *pIn, size_t length, wchar_t *pOut, size_t &DestinationSize);

	bool CP7ToSystemCPString(codepage_t codepage, bchar_t *pIn, size_t length);	// inplace translation

	MatrixError_t UnicodeToSystemCPCharacter(wchar_t in, bchar_t &out)
	{
		return UnicodeToSingleByte(GetSystemCodepage(), in, out);
	}

	MatrixError_t UnicodeToSystemCPString(const wchar_t *pIn, size_t length, bchar_t *pOut, size_t &DestinationSize)
	{
		return UnicodeToString(GetSystemCodepage(), pIn, length, pOut, DestinationSize);
	}

	int CollectAllCodePages(int *pDst, int BufferSize);
	bool ValidateCodePage(UINT codepage)
	{
		return IsValidCP(codepage);
	}

	codepage_t GetSystemCodepage(void)	{ return m_SystemCodepage;	};
	codepage_t GetWinCodepage(int country);
	codepage_t GetDOSCodepage(int country);

	bool Is7Bit(codepage_t codepage);
	bool Is8Bit(codepage_t codepage);
	bool IsMBCS(codepage_t codepage);
protected:
	bool IsValidCP(codepage_t codepage);

	// initializing helpers:
	void	Create7BitTables(void);
	void	CreateAuto7BitTable(int country);
	void	Create8BitTables(void);
	void	CreateSystemCPTable(void);
	void	CreateUTF8Translator(void);
	void	CreateUTF16Translator(void);
	void	CreateCp932Translator(void);
	void	CreateCp936Translator(void);
	void	CreateCp950Translator(void);
	void	Create8BitTable(const TranslateItemUnicode_t * const pList);
protected:
	wchar_t	m_DefaultChar;								//!< Used if there is no translation available. 
	codepage_t m_SystemCodepage;						//!< We use the windows codepage for the keyboard country
	CTransInfoBase	*m_TransInfo[CP_MAX+1];			//!< Translation tables, use codepage as an index to select table
};

/////////////////////////////////////////////////////////////////////////////


#endif // TRANSCODE_H__03164A21_6A2C_443B_BCD9_B1B934C58593__INCLUDED_


