/* ==============================================================================
   ECI Module Source
   ==============================================================================
 
   $RCSfile: Gen6FoundationApp/FoundationApp/eci/eci.cpp $
   $Revision: 1.4 $
   $Date: 2011/01/21 07:44:30EST $
 
   ==============================================================================
 
   ============================================================================== */
//! \file

#include "stdInclude.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "eci.h"
#include "BarcodeData.h"
#include "TransCodeEx.h"
#ifdef _WINDOWS
#include "menuSimulation.h"
#else
#include "menu.h"
#include "IoMan.h"
#include "readerconfiguration.h"
#include "uchar.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//! The public ECI object pointer.
CEci *g_pEci = NULL;

///////////////////////////////////////////////////////////////////////////////
//! Use this static member to create the public CEci object.
/*!
*/
void CEci::CreateEci(void)
{
	CTransCodeEx::CreateTransCode();
	if (g_pEci == NULL)
	{
		g_pEci = new CEci;
	}
	ASSERT(g_pEci != NULL);
}

///////////////////////////////////////////////////////////////////////////////
//! Use this static member to destroy the public CEci object.
/*!
*/
void CEci::DeleteEci(void)
{
	delete g_pEci;
	g_pEci = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CEci::CEci(void)
		: CEciMacro()
{
	m_CountryId = 0;
	m_translate = false;
}

CEci::~CEci(void)
{
}

///////////////////////////////////////////////////////////////////////////////
//! Gets called by the framework after a menu change
/*! This recreates all tables that are affected by a change in the settings.
*/
void CEci::OnChangedConfiguration(void)
{
	theTransCode.OnChangedConfiguration();

	m_ModeFlags = ECIMO_None;
	int mode = GET_SETTING(EciEnable);
	if(mode & READER_CONFIGURATION::ECI_ON)
		m_ModeFlags |= ECIMO_ENABLE_ECI;
	//	if(mode & READER_CONFIGURATION::ECI_BUFFER_MACROS)
	//		m_ModeFlags |= ECIMO_ENABLE_MACRO;
	if(mode & READER_CONFIGURATION::ECI_DIAGNOSTIC)
		m_ModeFlags |= ECIMO_SHOW_ECI|ECIMO_SHOW_GLI|ECIMO_SHOW_MACRO;

	// This a TODO:
	//	m_ModeFlags = ECIMO_None;
	//			if (GET_SETTING(PDFMacroEnabled))
	//				m_ModeFlags |= ECIMO_ENABLE_MACRO;
	//			if (GET_SETTING(PDFShowGLI))
	//				m_ModeFlags |= ECIMO_SHOW_GLI;
	//			if (GET_SETTING(PDFShowMacro))
	//				m_ModeFlags |= ECIMO_SHOW_MACRO;
	//			m_ModeFlags |= ECIMO_ENABLE_ECI;

	m_CountryId = GET_SETTING(KBD_CtryCode);
	mode = GET_SETTING(EciTranslateMode);

	switch(mode)
	{
	case ECI_TRANSLATE_OFF:	// always off
		m_translate=false;
		break;
	case ECI_TRANSLATE_ON:	// always on
		m_translate=true;
		break;
	default:	// auto (ask driver)
		{
			// ask the interface driver what it needs
			m_translate = ( IOCRET_YES == theStdInterface.DeviceIoControl(IOCTRL_IS_INTERPRET_DATA) );
		}
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////
//! This is the main entrance of the ECI decoder.
/*! We also call the codepage translation as we always did from main.

 \param *pLabel Point to a CBarcodeData object
 \return true means that we do not send a label yet (macro ECI)
*/
bool CEci::Check(CBarcodeData *pLabel)
{
	bool RetVal = false;
	SymbolType_t Type = GetSymbologyTypeAndOptions(pLabel);

	if ((Type == DECODER_FLAGS_ECI) && IsECIEnabled())
	{
		RetVal = ScanECI(pLabel);
		FixAimModifier(pLabel);
	}
	else
	{
		TranslateSimple(pLabel);
	}
	return RetVal;
}



///////////////////////////////////////////////////////////////////////////////
// Helper functions

///////////////////////////////////////////////////////////////////////////////
//! This function is used to translate the complete barcode contents.
/*! (vs. the ECI that can translate parts)

 \param *pLabel Point to a CBarcodeData object
*/
void CEci::TranslateSimple(CBarcodeData *pLabel)
{
	if(IsTranslationOn() || IsECIEnabled())	// checking for both settings results in a more consisten behavior bwetween decoders and products
	{
		pLabel->StoreDataType(IO_DATATYPE_CP_GEN);  // this is a kludge and should be done more early
	}
	else
	{
		pLabel->StoreDataType(IO_DATATYPE_BINARY);  // this is a kludge and should be done more early
		m_CurrentCodepage = CP_BINARY;
	}
	TranslateCodepage(pLabel);
}

///////////////////////////////////////////////////////////////////////////////
// Code 93 helpers
const char C93None=0;
const char C93Seq=1;
const char C93Eci=2;
const char C93Word=4;
const char C93FNC1_23=8;
const char C93FNC1_1=8;
const char C93Assoc=0x10;
const char Code93Modifiers[] =
    {
        //	Assoc.
        //	Data
        // Carrier 		FNC1 1st 	FNC1 2d,3d 	Word Mode 	ECI 			Lnk'd Seq'n
        C93None		|C93None		|C93None		|C93None		|C93None		|C93None,	// 0
        C93None		|C93None		|C93None		|C93None		|C93None		|C93None,	// 1
        C93None		|C93None		|C93None		|C93None		|C93None		|C93Seq,		// 2
        C93None		|C93None		|C93None		|C93None		|C93Eci		|C93None,	// 3
        C93None		|C93None		|C93None		|C93None		|C93Eci		|C93Seq,		// 4
        C93None		|C93None		|C93None		|C93Word		|C93None		|C93None,	// 5
        C93None		|C93None		|C93None		|C93Word		|C93None		|C93Seq,		// 6
        C93None		|C93None		|C93None		|C93Word		|C93Eci		|C93None,	// 7
        C93None		|C93None		|C93None		|C93Word		|C93Eci		|C93Seq,		// 8
        C93None		|C93None		|C93FNC1_23	|C93None		|C93None		|C93None,	// 9
        C93None		|C93None		|C93None		|C93None		|C93None		|C93None,	// :
        C93None		|C93None		|C93None		|C93None		|C93None		|C93None,	// ;
        C93None		|C93None		|C93None		|C93None		|C93None		|C93None,	// <
        C93None		|C93None		|C93None		|C93None		|C93None		|C93None,	// =
        C93None		|C93None		|C93None		|C93None		|C93None		|C93None,	// >
        C93None		|C93None		|C93None		|C93None		|C93None		|C93None,	// ?
        C93None		|C93None		|C93None		|C93None		|C93None		|C93None,	// @
        C93None		|C93None		|C93FNC1_23	|C93None		|C93None		|C93Seq,		// A
        C93None		|C93None		|C93FNC1_23	|C93None		|C93Eci		|C93None,	// B
        C93None		|C93None		|C93FNC1_23	|C93None		|C93Eci		|C93Seq,		// C
        C93None		|C93None		|C93FNC1_23	|C93Word		|C93None		|C93None,	// D
        C93None		|C93None		|C93FNC1_23	|C93Word		|C93None		|C93Seq,		// E
        C93None		|C93None		|C93FNC1_23	|C93Word		|C93Eci		|C93None,	// F
        C93None		|C93None		|C93FNC1_23	|C93Word		|C93Eci		|C93Seq,		// G
        C93None		|C93FNC1_1	|C93None		|C93None		|C93None		|C93None,	// H
        C93None		|C93FNC1_1	|C93None		|C93None		|C93None		|C93Seq,		// I
        C93None		|C93FNC1_1	|C93None		|C93None		|C93Eci		|C93None,	// J
        C93None		|C93FNC1_1	|C93None		|C93None		|C93Eci		|C93Seq,		// K
        C93None		|C93FNC1_1	|C93None		|C93Word		|C93None		|C93None,	// L
        C93None		|C93FNC1_1	|C93None		|C93Word		|C93None		|C93Seq,		// M
        C93None		|C93FNC1_1	|C93None		|C93Word		|C93Eci		|C93None,	// N
        C93None		|C93FNC1_1	|C93None		|C93Word		|C93Eci		|C93Seq,		// O
        C93Assoc		|C93None		|C93None		|C93None		|C93None		|C93None,	// P
        C93Assoc		|C93None		|C93None		|C93None		|C93None		|C93Seq,		// Q
        C93Assoc		|C93None		|C93None		|C93None		|C93Eci		|C93None,	// R
        C93Assoc		|C93None		|C93None		|C93None		|C93Eci		|C93Seq,		// S
        C93Assoc		|C93None		|C93None		|C93Word		|C93None		|C93None,	// T
        C93Assoc		|C93None		|C93None		|C93Word		|C93None		|C93Seq,		// U
        C93Assoc		|C93None		|C93None		|C93Word		|C93Eci		|C93None,	// V
        C93Assoc		|C93None		|C93None		|C93Word		|C93Eci		|C93Seq,		// W
        C93Assoc		|C93None		|C93FNC1_23	|C93None		|C93None		|C93None,	// X
        C93Assoc		|C93None		|C93FNC1_23	|C93None		|C93None		|C93Seq,		// Y
        C93Assoc		|C93None		|C93FNC1_23	|C93None		|C93Eci		|C93None,	// Z
        C93None		|C93None		|C93None		|C93None		|C93None		|C93None,	// [
        C93None		|C93None		|C93None		|C93None		|C93None		|C93None,	// \ do not remove this text
        C93None		|C93None		|C93None		|C93None		|C93None		|C93None,	// ]
        C93None		|C93None		|C93None		|C93None		|C93None		|C93None,	// ^
        C93None		|C93None		|C93None		|C93None		|C93None		|C93None,	// _
        C93None		|C93None		|C93None		|C93None		|C93None		|C93None,	// `
        C93Assoc		|C93None		|C93FNC1_23	|C93None		|C93Eci		|C93Seq,		// a
        C93Assoc		|C93None		|C93FNC1_23	|C93Word		|C93None		|C93None,	// b
        C93Assoc		|C93None		|C93FNC1_23	|C93Word		|C93None		|C93Seq,		// c
        C93Assoc		|C93None		|C93FNC1_23	|C93Word		|C93Eci		|C93None,	// d
        C93Assoc		|C93None		|C93FNC1_23	|C93Word		|C93Eci		|C93Seq,		// e
        C93Assoc		|C93FNC1_1	|C93None		|C93None		|C93None		|C93None,	// f
        C93Assoc		|C93FNC1_1	|C93None		|C93None		|C93None		|C93Seq,		// g
        C93Assoc		|C93FNC1_1	|C93None		|C93None		|C93Eci		|C93None,	// h
        C93Assoc		|C93FNC1_1	|C93None		|C93None		|C93Eci		|C93Seq,		// i
        C93Assoc		|C93FNC1_1	|C93None		|C93Word		|C93None		|C93None,	// j
        C93Assoc		|C93FNC1_1	|C93None		|C93Word		|C93None		|C93Seq,		// k
        C93Assoc		|C93FNC1_1	|C93None		|C93Word		|C93Eci		|C93None,	// l
        C93Assoc		|C93FNC1_1	|C93None		|C93Word		|C93Eci		|C93Seq,		// m
    };

char CEci::GetC93Type(char AimModifier)
{
	char rtn=C93None;

	AimModifier -= '0';	                  // remove offset to get a table index

	/*add a catch to make absolutely sure we don't access out of bounds of the array */
	if(AimModifier < sizeof(Code93Modifiers)/sizeof(Code93Modifiers[0]))
	{
		rtn = Code93Modifiers[AimModifier];
	}
	else
	{
		assert(0);
	}

	return rtn;
}

///////////////////////////////////////////////////////////////////////////////
// This is an interface between the barcode decoder and the ECI decoder options
CEci::SymbolType_t CEci::GetSymbologyTypeAndOptions(CBarcodeData *pLabel)
{
	SymbolType_t RetVal = LABEL_CONTAINS_NO_ECI;
	UCHAR AimModifier = pLabel->GetAimModifier();
	switch (pLabel->GetHHPId())
	{
	case  WA_CODELETTER_PDF417:
		m_DefaultCodepage = (codepage_t) GET_SETTING(PDFDfltCodePage);
		if (AimModifier == '1')
		{
				RetVal = DECODER_FLAGS_ECI;
				if(IsECIEnabled() && IsECICharsetContent(pLabel))
			m_DefaultCodepage = CP437;
		}
		break;

	case  WA_CODELETTER_MICROPDF:
		m_DefaultCodepage = (codepage_t) GET_SETTING(MPDDfltCodePage);
		if (AimModifier == '1')
		{
				RetVal = DECODER_FLAGS_ECI;
				if(IsECIEnabled() && IsECICharsetContent(pLabel))
			m_DefaultCodepage = CP437;
		}
		break;

	case  WA_CODELETTER_COMPOSITE:
		m_DefaultCodepage = (codepage_t) GET_SETTING(COMDfltCodePage);
		if (AimModifier == '3')
		{
				RetVal = DECODER_FLAGS_ECI;
				if(IsECIEnabled() && IsECICharsetContent(pLabel))
			m_DefaultCodepage = ISO_8859_1;
		}
		break;

	case WA_CODELETTER_DATAMATRIX:
		m_DefaultCodepage = (codepage_t) GET_SETTING(idmDfltCodePage);
		if ( (AimModifier >= '4') && (AimModifier <= '6') )
				RetVal = DECODER_FLAGS_ECI;
				if(IsECIEnabled() && IsECICharsetContent(pLabel))
					m_DefaultCodepage = ISO_8859_1;
		break;

	case WA_CODELETTER_AZTEC:
		m_DefaultCodepage = (codepage_t) GET_SETTING(aztDfltCodePage);
		if ( ((AimModifier >= '3') && (AimModifier <= '5'))
		        || (AimModifier == '9') || (AimModifier == 'A') || (AimModifier == 'B') )
				RetVal = DECODER_FLAGS_ECI;
				if(IsECIEnabled() && IsECICharsetContent(pLabel))
					m_DefaultCodepage = ISO_8859_1;
		break;

	case WA_CODELETTER_MAXICODE:
		m_DefaultCodepage = ISO_8859_1;
		if ( (AimModifier == '2') || (AimModifier == '3') )
				RetVal = DECODER_FLAGS_ECI;
		break;

	case WA_CODELETTER_QRCODE:
			m_DefaultCodepage = (codepage_t) GET_SETTING(qrDfltCodePage);
		if ( (AimModifier == '2') || (AimModifier == '4') || (AimModifier == '6') )
				RetVal = DECODER_FLAGS_ECI;
				if(IsECIEnabled() && IsECICharsetContent(pLabel))
					m_DefaultCodepage = CP_BINARY;	// most code does not support the ShiftJis, so for now I use binary
		break;

	case  WA_CODELETTER_UCC128:
	case  WA_CODELETTER_CODE128:
		m_DefaultCodepage = (codepage_t) GET_SETTING(C128DfltCodePage);
		break;
	case  WA_CODELETTER_CODE39:
		m_DefaultCodepage = (codepage_t) GET_SETTING(C39DfltCodePage);
		break;

	case  WA_CODELETTER_CODE93:
		{
			char c93Type = GetC93Type(AimModifier);
			if (c93Type & C93Eci)
			{
				RetVal = DECODER_FLAGS_ECI;
				if (c93Type & C93Word)
					m_DefaultCodepage=ISO_UTF16BE;
				else if(IsECIEnabled() && IsECICharsetContent(pLabel))
					m_DefaultCodepage = ISO_8859_1;
				else
					m_DefaultCodepage = (codepage_t) GET_SETTING(C93DfltCodePage);
			}
			else
			{
				m_DefaultCodepage = (codepage_t) GET_SETTING(C93DfltCodePage);
			}
		}
		break;

	default:	// EAN, UPC, ... (digits and a few alpha characters)
		m_DefaultCodepage = CP_ISO646;	// ASCII
		break;
	}
	m_CurrentCodepage = m_DefaultCodepage;	// ECI can change the codepage dynamically

	return RetVal;
}

///////////////////////////////////////////////////////////////////////////////
// After we decoded the ECI, we must switch the AimModifier to "does not contain ECI"
void CEci::FixAimModifier(CBarcodeData *pLabel)
{
	if (!IsShowECI())
	{
		UCHAR AimModifier = pLabel->GetAimModifier();
		switch (pLabel->GetHHPId())
		{
		case  WA_CODELETTER_PDF417:
		case  WA_CODELETTER_MICROPDF:
			if (AimModifier == '1')
			{
				AimModifier = '2';
			}
			break;

		case  WA_CODELETTER_COMPOSITE:
			if (AimModifier == '3')
			{
				AimModifier = '0';
			}
			break;

		case WA_CODELETTER_DATAMATRIX:
			if ( (AimModifier >= '4') && (AimModifier <= '6') )
				AimModifier = AimModifier-3;
			break;

		case WA_CODELETTER_AZTEC:
			m_DefaultCodepage = ISO_8859_1;
			if ( (AimModifier >= '3') && (AimModifier <= '5') )
				AimModifier = AimModifier-3;
			else if ( (AimModifier == 'A') || (AimModifier == 'B') )
				AimModifier = AimModifier-('A'-'7');
			else if ( AimModifier == '9' )
				AimModifier = AimModifier-3;
			break;

		case WA_CODELETTER_MAXICODE:
			if ( (AimModifier == '2') || (AimModifier == '3') )
				AimModifier = AimModifier-2;
			break;

		case WA_CODELETTER_QRCODE:
			if ( (AimModifier == '2') || (AimModifier == '4') || (AimModifier == '6') )
				AimModifier = AimModifier-1;
			break;

		case  WA_CODELETTER_UCC128:
			AimModifier = '0';
			break;

		case  WA_CODELETTER_CODE93:
			{
				char c93Type = GetC93Type(AimModifier);
				if (c93Type & C93Eci)
				{
					// the guys that made this std. should learn to program
					if (AimModifier == 'B')
						AimModifier = '9';
					else if (AimModifier == 'a')
						AimModifier = 'Y';
					else
						AimModifier = AimModifier-2;
				}
			}
			break;

		default:
			break;
		}
		pLabel->StoreAim(pLabel->GetAimId(), AimModifier);
	}
}

bool  CEci::IsECICharsetContent(CBarcodeData *pLabel)
{
	const bchar_t *pIn;
	size_t size;

	pLabel->GetTotalData(pIn, size);
	pIn = strstr(pIn, "\\0");
	return (pIn!=NULL);
}



