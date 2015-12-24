/* ==============================================================================
   ECI Module Header
   ==============================================================================

   $RCSfile: Gen6FoundationApp/FoundationApp/eci/eci_charset.h $
   $Revision: 1.1 $
   $Date: 2009/03/05 14:45:53EST $

   ==============================================================================
   
   ============================================================================== */
//! \file

#ifndef ECI_CHARSET_H_20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define ECI_CHARSET_H_20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#include "codepage.h"
#include "Eci_Base.h"

class CBarcodeData;

///////////////////////////////////////////////////////////////////////////////
/// Handles Character set ECIs.
/*!
	
*/
class CEciCharSet : public CEciBase
{
public:
	CEciCharSet(void);
	~CEciCharSet(void);

	void	TurnTranslationOn(void)				{ m_translate = true;	}
	bool	IsTranslationOn(void)				{ return m_translate;	}

	static int CollectAllCodePages(int *pDst, int BufferSize);
	static bool ValidateCodePage(int codepage);
protected:

	void TranslateToUnicode(CBarcodeData *pLabel, codepage_t CurrentCodepage);
	void TranslateCodepage(CBarcodeData *pLabel);
	void TranslateFromUnicodeToSystemCP(CBarcodeData *pLabel);

#ifndef UNICODE_FOR_INTERFACES
	void TranslateFrom7BitToSystem(CBarcodeData *pLabel);
#endif
	codepage_t GetCodePageFromEci(UINT eci);
	bool HandleECINumber(UINT nECI);
	MatrixError_t TranslateToUnicode(const bchar_t *pIn, wchar_t *pOut, size_t size, size_t &ReturnedSize);
	MatrixError_t TranslateFromUnicodeToSystemCP(const wchar_t *pIn, bchar_t *pOut, size_t size);
	MatrixError_t TranslateFromCPToSystemCP(const bchar_t *pIn, tichar_t *pOut, size_t size, size_t &ReturnedSize);
	MatrixError_t TranslateISO646IToSystemCP(const bchar_t *pIn, tichar_t *pOut, size_t size);
	bool IsDataBackslash(const bchar_t *pIn, size_t size);
	size_t ParseForECI(const bchar_t *pIn, size_t size);
	size_t ParseECICode(const bchar_t *pIn, size_t size);
	size_t ParseMacro(const bchar_t *pIn, size_t size);
	size_t ParseUninterpretedCodeWord(const bchar_t *pIn, size_t size);

	size_t ParseAndTranslateCharSetECI(CBarcodeData *pSource, tichar_t *pOutput);

	bool IsUTF16(void)		{ return (m_CurrentCodepage==ISO_UTF16BE) || (m_CurrentCodepage==ISO_UTF16LE);	}
// data members
	/* character set stuff */
	codepage_t	m_CurrentCodepage;
	codepage_t	m_DefaultCodepage;
	int	m_CountryId;
	bool m_translate;
};


#endif // ECI_CHARSET_H_20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_



