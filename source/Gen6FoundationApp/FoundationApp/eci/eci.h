/* ==============================================================================
   ECI Module Header
   ==============================================================================

   $RCSfile: Gen6FoundationApp/FoundationApp/eci/eci.h $
   $Revision: 1.1 $
   $Date: 2009/06/15 12:47:11EDT $

   ==============================================================================

   ============================================================================== */
//! \file

#ifndef ECI_H_20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define ECI_H_20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#include "codepage.h"
#include "Eci_Macro.h"


///////////////////////////////////////////////////////////////////////////////
/// The ECI decoder as it is exposed to the rest of the system.
/*!
*/
class CEci : public CEciMacro
{
public:
	CEci(void);
	~CEci(void);

	static void CreateEci(void);
	static void DeleteEci(void);

	bool Check(CBarcodeData *pLabel);
	void 	OnChangedConfiguration(void);	// gets called by the framework after a menu change

private:
	enum SymbolType_t
	{
		LABEL_CONTAINS_NO_ECI,
		DECODER_FLAGS_ECI
	};

	void TranslateSimple(CBarcodeData *pLabel);
	void FixAimModifier(CBarcodeData *pLabel);
	SymbolType_t GetSymbologyTypeAndOptions(CBarcodeData *pLabel);
	char GetC93Type(char AimModifier);
	bool IsECICharsetContent(CBarcodeData *pLabel);

// data members
};

extern   CEci          		*g_pEci;

//! The public ECI decoder object
#define  theEci      		(*g_pEci)

#endif                                    // ECI_H_20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_



