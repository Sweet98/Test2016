/* ==============================================================================
   ECI Module Header
   ==============================================================================

   $RCSfile: Gen6FoundationApp/FoundationApp/eci/eci_macro.h $
   $Revision: 1.1 $
   $Date: 2009/03/05 14:45:54EST $

   ==============================================================================

   ============================================================================== */
//! \file

#ifndef ECI_MACRO_H_20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define ECI_MACRO_H_20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#include "codepage.h"
#include "Eci_Charset.h"

///////////////////////////////////////////////////////////////////////////////
/// A helper to store an ECI label that contains macro information.
/*!
*/
class CECI_Macro
{
public:
	CECI_Macro(void);
	void Reset(void);


	// data members
	CBarcodeData *m_pLabel;

	int Contents;
	unsigned long m_MacroIndex;

	/* macro optional fields */
	unsigned long m_MacroFileSize;
	unsigned int  m_MacroSegmentCount;
	unsigned long m_MacroTimeStamp;
	unsigned long m_MacroChecksum;
	int m_MacroFileNameIndex;
	int m_MacroSenderIndex;
	int m_MacroAddresseeIndex;
};

///////////////////////////////////////////////////////////////////////////////
/// Handles Macro ECIs (just a dummy yet).
/*!
*/
class CEciMacro : public CEciCharSet
{
public:
	CEciMacro(void);
	// just a dummy for now
protected:
	size_t ParseMacroECIDetails(const bchar_t *pIn, size_t size);
	size_t ParseMacroECI(const bchar_t *pIn, size_t size);

	MatrixError_t PreParseECI(CBarcodeData *pLabel);
	bool ScanECI(CBarcodeData *pLabel);

	enum
	{
		MaxMacros=20
	};
	// data members
	CECI_Macro m_CurrentReadMacro;
	CECI_Macro m_MacroBuffer[MaxMacros+1];
};


#endif // ECI_MACRO_H_20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_



