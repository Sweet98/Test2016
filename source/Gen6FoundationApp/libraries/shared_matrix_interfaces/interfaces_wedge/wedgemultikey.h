/*=================================================================================
  class:

//=================================================================================
   $Source: interfaces_wedge/wedgemultikey.h $
   $Date: 2011/08/24 11:23:50EDT $
   $Revision: 2.5 $



//=================================================================================*/

#ifndef MULTIKEY_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define MULTIKEY_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "WedgeInternational.h"
#include "Codepage.h"

#define baseCWedgeMultiKey CWedgeInternational
class CWedgeMultiKey : public baseCWedgeMultiKey
{
public:
	CWedgeMultiKey(CDriverBase *pDriver, const CWedgeTerminal *pTerminal);

//	CWedgeMultiKey(const CWedgeTerminal *pTerminal);
//	virtual ~CWedgeMultiKey();
   virtual MatrixError_t Open(interface_id_t nId, const tichar_t *pParameter);
	void Translate(tichar_t ansi);

protected:
	void InitializeKeyLookup(void);

	void SendInHexFormat(tichar_t ansi);	/* kind a hex dump format */
	bool AddSpaceToDeadKey(tichar_t ansi);
//	bool TranslateKey(const CKey *key, tichar_t ansi);
	bool MultiKey(tichar_t ansi);
	void InitializeCtrlASCIIKeyLookup(void);   // Support for KBDCAS3 menu command

// helpers
protected:
	bool IsValidKey(const CKey *pKey);
	bool IsValidDeadKey(const CKey *pKey);
	wchar_t LookUpCompatibleDeadKey(wchar_t DeadCharacter);

	bool TryDeadKey(tichar_t ansi);
	bool TryAltNumPadUnicode(tichar_t ansi);
	bool TryAltNumPadWin(tichar_t ansi, codepage_t codepage);
	bool TryAltNumPadDos(tichar_t ansi, codepage_t codepage);
	bool TryHTMLFormat(tichar_t ansi);
	bool TryNonPrintChar(tchar_t ansi);

	bool TranslateCharacter(tichar_t ansi);
	bool SendAltNumPad(const tichar_t* pFormat, UINT character);

};

#endif /* MULTIKEY_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */





