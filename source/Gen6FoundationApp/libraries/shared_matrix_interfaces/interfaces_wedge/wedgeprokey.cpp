/*=================================================================================
  class: CWedgeProKey

//=================================================================================
   $Source: interfaces_wedge/wedgeprokey.cpp $
   $Date: 2009/06/29 05:03:46EDT $
   $Revision: 2.2 $



//=================================================================================*/

#include "stdInclude.h"
#include "CommonDriver.h"
#include "WedgeproKey.h"
#include "prokeytb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TableFormat 216				// We now use a flash file


#define SIZE_OF_CHARACTER			2
#define SIZE_OF_REPLACEMENT 		(SIZE_OF_CHARACTER+2)
#define SIZE_OF_SEQUENCEHEADER	3

// Constructor
CWedgeProKey::CWedgeProKey(CDriverBase *pDriver, const CWedgeTerminal *pTerminal)
:baseCWedgeProKey(pDriver, pTerminal)
{
	m_pProkeyTable = LoadProkeyFile();
}

/*
// Destructor
CWedgeProKey::~CWedgeProKey()
{
}
*/

/**********************************************************************/
/* Some helpers */

inline bool CWedgeProKey::TableVersionCheck(void)
{
	if (m_pProkeyTable!=NULL)
	{
		const UCHAR *pTable = m_pProkeyTable+PK_FormatOffs;
		return (*pTable == TableFormat);
	}
	return false;
}

// this ensures that the code even works on "little endian" machines
inline unsigned short CWedgeProKey::GetWord(const UCHAR *pMem)
{
	UCHAR highbyte = *pMem++;
	UCHAR lowbyte  = *pMem;
	return (highbyte<<8) + lowbyte;
}

inline const UCHAR *CWedgeProKey::GetPointerFromOffset(int offset)
{
	const UCHAR *pTable = m_pProkeyTable;
	return (const UCHAR *) pTable + GetWord(pTable + offset) + PK_FormatOffs;
}

inline const UCHAR *CWedgeProKey::GetRepStart(void)
{
	return GetPointerFromOffset(PK_RepStartOffs);
}

inline const UCHAR *CWedgeProKey::GetSeqStart(void)
{
	return GetPointerFromOffset(PK_SeqStartOffs);
}

inline wchar_t CWedgeProKey::GetCharacter(const UCHAR *&pRep)
{
	wchar_t temp = GetWord(pRep);
	pRep += SIZE_OF_CHARACTER;
	return temp;
}

/**********************************************************************/
/* Replacements, they work similar to the international exception tables.
 * The corrosponding entry in the ANSI to keynumber translation table
 * will be replaced by the new entries from prokey. This allows to
 * map all known keys to every character.
 * Modifiers can be added to the mapped key.
 * This module is much more powerfull than the old HC11 based prokey.
 * Again, the detour via the keynumbers has added a benefit.
 * */

const UCHAR *CWedgeProKey::SearchReplacementTable(UCHAR TermId, UCHAR Country, int &NumKeys)
{
	UCHAR TabTermId;                   			/* Values from the prokey table */
	UCHAR TabCountry;
	UCHAR TabNumKeys=0;
	const UCHAR *pRep = GetRepStart();			/* pointer into the replacement table */

	while ((TabTermId  = *pRep++) != 0)			/* loop until we found the correct table */
	{
		TabCountry = *pRep++;
		TabNumKeys = *pRep++;
		if ( (TermId == TabTermId) && (TabCountry == Country) )
		{  /* found */
			NumKeys = TabNumKeys;
			return pRep;
		}
		pRep += TabNumKeys*SIZE_OF_REPLACEMENT;
	};
	/* not found */
	NumKeys = 0;
	return NULL;
}

/* Install the replacements into the ASNI to keynumber table. */
void CWedgeProKey::CopyReplacements(const UCHAR *pRep, int NumKeys)
{
	keynum_t keynum;
	wchar_t	character;
	modifiers_t	modifier;
	int key;

	for (key =0; key < NumKeys; key++)
	{
		character= GetCharacter(pRep);
		keynum	= *pRep++;
		modifier	= *pRep++;
		m_KeyNumLookup.StoreKey(character, keynum, modifier);
	}
}

/**********************************************************************/
/* This is called right after the international exception tables have
 * been merged into the ASNI to keynumber table.
 * Note:
 * 	A universal terminal really makes sense because we use keynumbers here.
 * 	Keynumbers are independent of terminals to some distingt.
 * 	You must be carefull using universal terminal, but it's your choice. */

void CWedgeProKey::ProkeyReplacements(void)
{
	int NumKeys;
	const UCHAR *pRep;
	UCHAR Country = (UCHAR)GetCountry();
	UCHAR TermId = (UCHAR)GetTerminalID();

	if (!(TableVersionCheck()))	/* some security for myself */
	{
		return;
	}

	/* First look for fully "universal" replacements */
	pRep = SearchReplacementTable(UNIVERSAL_TERMINAL, UNIVERSAL_COUNTRY, NumKeys);
	CopyReplacements(pRep, NumKeys);

	/* Now look for "universal" terminal replacements */
	pRep = SearchReplacementTable(UNIVERSAL_TERMINAL, Country, NumKeys);
	CopyReplacements(pRep, NumKeys);

	/* Now look for "universal" country replacements */
	pRep = SearchReplacementTable(TermId, UNIVERSAL_COUNTRY, NumKeys);
	CopyReplacements(pRep, NumKeys);

	/* Now look for specific replacements, so the specific ones can override. */
	pRep = SearchReplacementTable(TermId, Country, NumKeys);
	CopyReplacements(pRep, NumKeys);
}

/**********************************************************************/
/* Sequences, are similar to the sequences for the scancode tables.
 * Here we do not make any detour via keynumbers, because the
 * prokey sequences are a tool to add new and unknown stuff to an interface.
 * A sequence of bytes is sent unchanged with a medium delay between the
 * bytes.
 * For each sequence there is also an entry in the replacement table
 * which writes a special sequence marker (an illegal keynumber) into
 * the ANSI to Keynumber translation table. This marker is used to
 * call the ProkeySequence function if found.
 * The modifier entry of "mapped to sequence" characters is 0, therefore
 * all modifier keys are broken (if necessary) before we send the sequence.
 * */

/* Finally find the sequence for this character.
	Returns a pointer to the sequence scancodes */
const UCHAR *CWedgeProKey::SearchCharacterSequence(const UCHAR *pSeq, wchar_t character, int &NumScancodes)
{
	/* We are here if we found the Sequence table for this terminal/country */
	wchar_t TabCharacter;
	UCHAR TabNumBytes=0;
	if (pSeq != NULL)
	{
		while ((TabNumBytes  = *pSeq++) != 0)
		{
			TabNumBytes -= SIZE_OF_SEQUENCEHEADER;	/* take into account that we autoincrement pSeq */
			TabCharacter= GetCharacter(pSeq);
			if (TabCharacter == character)
			{  /* found sequence for this character */
				NumScancodes = TabNumBytes;
				return pSeq;
			}
			pSeq += TabNumBytes;
		}
	}
	/* Not found any usefull table */
	NumScancodes = 0;
	return NULL;
}

/* Search for a sequence table for this particular terminal/country. */
const UCHAR *CWedgeProKey::SearchTermCountrySequenceTable(UCHAR TermId, UCHAR Country)
{
	char TabTermId;
	char TabCountry;
	unsigned short TabSize;
	const UCHAR *pSeq = GetSeqStart();

	while ((TabTermId  = *pSeq++) != 0)
	{
		TabCountry = *pSeq++;
		TabSize = GetWord(pSeq);
		if ( (TermId == TabTermId) && (TabCountry == Country) )
		{  /* found */
			return pSeq+2;
		}
		pSeq += TabSize;
	};

	return NULL;
}

/* Search through all sequence tables.
 * Because we do this on run time of a particular character, we must search
 * for specific Terminal/Countries first. If not found, then try the
 * universal country.
 * It makes no sense to introduce a universal terminal here, because we
 * send scancodes which are usual terminal specific. */
const UCHAR *CWedgeProKey::SearchSequence(wchar_t ansi, int &NumScancodes)
{
	UCHAR Country = (UCHAR)GetCountry();
	UCHAR TermId = (UCHAR)GetTerminalID();

	/* Specific entries have priority over universal ones. */
	const UCHAR *pSeq = SearchTermCountrySequenceTable(TermId, Country);
	const UCHAR *pCharSeq = SearchCharacterSequence(pSeq, ansi, NumScancodes);

	if (pCharSeq == NULL)
	{
		/* No specific entry found, now look for a universal one. */
		pSeq = SearchTermCountrySequenceTable(TermId, UNIVERSAL_COUNTRY);
		pCharSeq = SearchCharacterSequence(pSeq, ansi, NumScancodes);
	}
	return pCharSeq;
}

// We search through the list if scancodes to find our scancode,
// then we check if we have a communication key and add a long delay if so.
// For luck we do need to do this only for prokey sequences.

void CWedgeProKey::AutoFixDelay(scancode_t scancode)
{
	if (m_pScancodes->IsCommunicationScancode(scancode))
	{
		AddDelayCommand(GetLongDelay());
	}
	else
	{
		AddMediumDelayCommand();
	}
}


/* This is called while translating characters into keynumbers and
 * finally into commands. */
void CWedgeProKey::ProkeySequence(wchar_t ansi)
{
	if (!(TableVersionCheck()))	/* some security for myself */
	{
		return;
	}

	int i;
	int NumScancodes;
	scancode_t scancode;
	const UCHAR *pSeqByte = SearchSequence(ansi, NumScancodes);

	/* Store the sequence byte into the command buffer. */
	for (i=0; i<NumScancodes; i++)
	{
		scancode = *pSeqByte++;
		AddScancode(scancode, EX_SEQUENCE);
		AutoFixDelay(scancode);
	}
}




