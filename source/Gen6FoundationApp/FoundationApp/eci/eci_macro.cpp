/* ==============================================================================
   ECI Macro Module Source
   ==============================================================================

   $RCSfile: Gen6FoundationApp/FoundationApp/eci/eci_macro.cpp $
   $Revision: 1.2 $
   $Date: 2009/06/16 10:41:44EDT $

   ==============================================================================

   ============================================================================== */
//! \file

#include "stdInclude.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "uchar.h"
#include "Eci_Macro.h"
#include "TransCodeEx.h"

//This class does all the parsing for macro ECIs
// THIS FILE IS A CONSTRUCTION AREA !
// THIS FILE IS A CONSTRUCTION AREA !
// THIS FILE IS A CONSTRUCTION AREA !
// THIS FILE IS A CONSTRUCTION AREA !
///////////////////////////////////////////////////////////////////////////////
//
CECI_Macro::CECI_Macro(void)
{
	Reset();
}

///////////////////////////////////////////////////////////////////////////////
//
void CECI_Macro::Reset(void)
{
	m_pLabel=NULL;

//	int Contents;

	m_MacroFileSize=0;
	m_MacroSegmentCount=0;
	m_MacroTimeStamp=0;
	m_MacroChecksum=0;
	m_MacroFileNameIndex=0;
	m_MacroSenderIndex=0;
	m_MacroAddresseeIndex=0;

	m_MacroIndex=  -1U;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
CEciMacro::CEciMacro(void)
: CEciCharSet()
{
}

///////////////////////////////////////////////////////////////////////////////
// This is a dummy for now
size_t CEciMacro::ParseMacroECIDetails(const bchar_t *pIn, size_t size)
{
	size_t SkipSize=size;

	if (size > 3)
	{
		if(pIn[1] == 'M')
		{
			switch(pIn[2])
			{
			case 'Y':
				SkipSize = 3;
				break;
			case 'I':	// dummy!!! for now
			case 'O':	// dummy!!! for now
			case 'F':	// dummy!!! for now
				SkipSize = 3;	// let the outer loop search the next
				break;
			case 'Z':	// dummy!!! for now
				SkipSize = 3;
			default:
				break;
			}
		}
		else
		{
			SkipSize = 1;	// emergeny, skip the '\'
		}
	}
	return SkipSize;
}

///////////////////////////////////////////////////////////////////////////////
//
// Called with pIn pointing to the '\'
// An ECi can be:
// "\123456"
// "\C123...C"
// "\\"
// "\M...."
// more?
size_t CEciMacro::ParseMacroECI(const bchar_t *pIn, size_t size)
{
	const size_t ECISize = 6+1;

	size_t SkipSize = 0;
	// sanity first
	if(size < ECISize)
	{
		SkipSize = size;
	}
	else
	{
		bchar_t SecondChar=pIn[1];
		if (SecondChar == '\\')
		{	// the \ as data, copy to destination
			SkipSize = 2;
		}
		else if (SecondChar == 'C')
		{
			SkipSize =  2 + ParseEndOfECI(pIn+2, size-2, pIn[1]);
		}
		else if (isdigit(SecondChar))
		{
			SkipSize =  7;
		}
		else if (SecondChar == 'M')
		{
			SkipSize =  ParseMacroECIDetails(pIn, size);
		}
		else
		{
			SkipSize =  1;	// emergency, just skip backslash
		}
	}
	return SkipSize;
}

///////////////////////////////////////////////////////////////////////////////
// This is the first pass of the parsing. We only look for Macro information.
MatrixError_t CEciMacro::PreParseECI(CBarcodeData *pLabel)
{
	MatrixError_t RetVal = ERR_PASS;
	const bchar_t *pInput;	// input is always bytes
	const bchar_t *pCurrent;
	size_t RestSize;
	pLabel->GetTotalData(pInput, RestSize);

	// now parse and find all ECI codes
	while(RestSize > 0)
	{
		// this loop jumps from one '\' to the next
		pCurrent = SearchForNextBackslash(pInput, RestSize);
		if(pCurrent != NULL)
		{
			size_t PartialSize = pCurrent-pInput;
			pInput += PartialSize;
			RestSize -= PartialSize;
			size_t SkipSize = ParseMacroECI(pInput, RestSize);
			pInput += SkipSize;
			RestSize -= SkipSize;
		}
		else
		{
			break;	// done
		}
	};

	return RetVal;
}


///////////////////////////////////////////////////////////////////////////////
// We store the label into our macro buffer if there is any macro info.
// a dummy for now
bool CEciMacro::ScanECI(CBarcodeData *pLabel)
{
	if(IsMacroEnabled())
	{
		PreParseECI(pLabel);
	}
// ... more code to write....
	size_t OutputSize;
	tichar_t *pOutput;
	CBarcodeData *pDestObj;
#if(0) // just sketching
	if(m_Contents & ECIC_MACRO)
	{
		// check for all labels....
		if( ..)
		{
			OutputSize = GetTotalSizeOfAllLabels();
			// get a buffer for outputting the data
			pDestObj = CBarcodeData::GetSaveNewBuffer(pOutput, OutputSize);
			while(!ALlMarcos())
			{
				pMacroLabel = ??
				ParseAndTranslateCharSetECI(pMacroLabel, pOutput);
				if(pMacroLabel != pLabel)	// don't kill our return buffer
					pMacroLabel->Free();
				pOutput += ??;
			}
		}
		else
		{	// need more labels
			return true;
		}
	}
	else
#endif
	{
		OutputSize = pLabel->GetLabelSize();
		// get a buffer for outputting the data
		pDestObj = CBarcodeData::GetSaveNewBuffer(pOutput, OutputSize);
		size_t FinalSize = ParseAndTranslateCharSetECI(pLabel, pOutput);
		#ifdef UNICODE_FOR_INTERFACES
			pDestObj->StoreDataType(IO_DATATYPE_UNICODE);
		#else
			pDestObj->StoreDataType(IO_DATATYPE_CP_SYS);
		#endif
		pDestObj->FinalizeBuffer(FinalSize);
	}

	pLabel->MoveBuffer(pDestObj);
	return false;
}


