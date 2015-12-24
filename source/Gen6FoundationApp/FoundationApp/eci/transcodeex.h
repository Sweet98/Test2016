/*=================================================================================
	class: CTransCodeEx
	Translate byte characters into Unicode and vice versa
//=================================================================================
   $Source: Gen6FoundationApp/FoundationApp/eci/transcodeex.h $
   $Date: 2010/05/10 02:52:10EDT $
   $Revision: 1.3 $


//=================================================================================*/
//! \file

#ifndef TRANSCODE_EX_H__03164A21_6A2C_443B_BCD9_B1B934C58593__INCLUDED_
#define TRANSCODE_EX_H__03164A21_6A2C_443B_BCD9_B1B934C58593__INCLUDED_

#include "transcode.h"
#include "BarcodeData.h"

class HSTRING;
class HWSTRING;

///////////////////////////////////////////////////////////////////////////////
/// A class that can translate a CBarcodeData object to a different codepage.
/*! It serves for the ECI and output format settings.
*/
class CTransCodeEx : public CTransCode
{
public:
//	CTransCodeEx(void);
//	~CTransCodeEx(void);

	static void CreateTransCode(void);
	static void DeleteCTransCode(void);

	void TranslateSystemCPToUnicode(CBarcodeData *pLabel);
	void TranslateFromUnicodeToSystemCP(CBarcodeData *pLabel);

	void TranslateToUnicode(CBarcodeData *pLabel, codepage_t CurrentCodepage);
	void TranslateFromUnicodeToCP(CBarcodeData *pLabel, codepage_t CurrentCodepage, IoDataType_t NewType);

	// try to fix wrong formats in the label
	void AutoTranslate(CBarcodeData  *pLabel, int OutputFormat);

// overloads for Barrys HSTRING classes
	bool TranslateToUnicode(HSTRING *pSource, HWSTRING *pDestination, codepage_t Codepage);
	bool TranslateToUnicode(HSTRING &Source, HWSTRING &Destination, codepage_t Codepage)
	{
		return TranslateToUnicode(&Source, &Destination, Codepage);
	}

	bool TranslateFromUnicodeToCP(HWSTRING *pSource, HSTRING *pDestination, codepage_t Codepage);
	bool TranslateFromUnicodeToCP(HWSTRING &Source, HSTRING &Destination, codepage_t Codepage)
	{
		return TranslateFromUnicodeToCP(&Source, &Destination, Codepage);
	}

	bool TranslateSystemCPToUnicode(HSTRING *pSource, HWSTRING *pDestination);
	bool TranslateSystemCPToUnicode(HSTRING &Source, HWSTRING &Destination)
	{
		return TranslateSystemCPToUnicode(&Source, &Destination);
	}

	bool TranslateFromUnicodeToSystemCP(HWSTRING *pSource, HSTRING *pDestination);
	bool TranslateFromUnicodeToSystemCP(HWSTRING &Source, HSTRING &Destination)
	{
		return TranslateFromUnicodeToSystemCP(&Source, &Destination);
	}
protected:
	// helpers for AutoTranslate()
	IoDataType_t EnsureItsUnicode(CBarcodeData *pLabel, IoDataType_t CurrentType);
	void TranslateBigAndLittleEndian(CBarcodeData *pLabel, IoDataType_t NewType);
	void TranslateWideToByteStream(CBarcodeData *pLabel, IoDataType_t NewType);
	void TranslateByteStreamToWide(CBarcodeData *pLabel);
};

/////////////////////////////////////////////////////////////////////////////
extern   CTransCodeEx  			*g_pTransCode;

//! The public Transcode object
#define  theTransCode			(*g_pTransCode)


#endif // TRANSCODE_EX_H__03164A21_6A2C_443B_BCD9_B1B934C58593__INCLUDED_


