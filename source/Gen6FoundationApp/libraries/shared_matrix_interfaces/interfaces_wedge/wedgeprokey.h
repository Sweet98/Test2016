/*=================================================================================
  class:

//=================================================================================
   $Source: interfaces_wedge/wedgeprokey.h $
   $Date: 2009/06/29 05:03:47EDT $
   $Revision: 2.1 $



//=================================================================================*/

#ifndef PROKEY_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define PROKEY_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "WedgeBase.h"

#define baseCWedgeProKey CWedgeBase
class CWedgeProKey : public baseCWedgeProKey
{
public:
	CWedgeProKey(CDriverBase *pDriver, const CWedgeTerminal *pTerminal);

protected:
	void ProkeySequence(wchar_t ansi);
	void ProkeyReplacements(void);

private:
	void CopyReplacements(const UCHAR *pRep, int NumKeys);
	const UCHAR *SearchReplacementTable(UCHAR TermId, UCHAR Country, int &NumKeys);

	const UCHAR *SearchCharacterSequence(const UCHAR *pSeq, wchar_t ansi, int &NumScancodes);
	const UCHAR *SearchTermCountrySequenceTable(UCHAR TermId, UCHAR Country);
	const UCHAR *SearchSequence(wchar_t ansi, int &NumScancodes);
	void AutoFixDelay(scancode_t scancode);

	// low level helpers to access the prokey table
	static unsigned short GetWord(const UCHAR *pMem);
	static wchar_t GetCharacter(const UCHAR *&pRep);
	const UCHAR *GetPointerFromOffset(int offset);
	const UCHAR *GetRepStart(void);
	const UCHAR *GetSeqStart(void);
	bool TableVersionCheck(void);

	const UCHAR *m_pProkeyTable;	         // points to the table if there is one, else NULL
};

#endif /* PROKEY_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */




