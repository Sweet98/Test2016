/*=================================================================================
  class: CScancodes

//=================================================================================
   $Source: interfaces_wedge/wedgescancode.cpp $
   $Date: 2009/06/29 05:03:48EDT $
   $Revision: 1.2 $



//=================================================================================*/

#include "stdInclude.h"
#include "CommonDriver.h"
#include "WedgeScancode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const CScancodeSequence *CScancodes::GetSequence(keynum_t keynum) const
{
	const CScancodeSequence *pSeq;

	if (keynum > m_numkeys)
		return NULL;

	if (m_pSequences == NULL)
		return NULL;

	if (GetFlags(keynum) & KN_SEQUENCE)
	{
		pSeq = m_pSequences;
		while (pSeq->GetSize() > 0)
		{
			if (pSeq->IsKeyMatch(keynum))
				return pSeq;
			pSeq++;
		}
	}
	return NULL;
}

int CScancodes::GetScancodeType(keynum_t keynum) const
{
	if (keynum > m_numkeys)
		return KEYNUM_BAD;

	if (GetFlags(keynum) & KN_SEQUENCE)	// must be before check for illegal scancode
		return USE_SEQUENCE;

	if (GetScancode(keynum) == GetIllegalMarker())
		return NOTSUPPORTED_SCANCODE;

	if (GetFlags(keynum) & KN_EXTENDED)
		return USE_EXTENDED_SCANCODE;

	if (GetFlags(keynum) & KN_PARITYVARIATION)
		return ADD_PARITY;

	return REGULAR_SCANCODE;
}

bool CScancodes::IsCommunicationScancode(scancode_t scancode) const
{
	for(UINT keynum=1; keynum<m_numkeys; keynum++)
	{
		if (GetScancode(keynum) == scancode)
		{
			if (IsCommunicationKey(keynum))
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}



