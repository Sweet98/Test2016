/*=============================================================================
 *

 *=============================================================================
 * $RCSfile: Gen6/EditScript.h $
 * $Revision: 1.3 $
 * $Date: 2009/12/10 00:20:54EST $
 *============================================================================*/
#ifndef EDIT_SCRIPT_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define EDIT_SCRIPT_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include	"HHPLprocessor.h"		// HHPL program run-time profile information
#ifdef PLUGIN_DEV
#include "HONPluginConfiguration.h"
#endif

class CBarcodeFifo;

class SPECIAL_CLASS_DEFINITION_MODIFIERS CEditScriptProc : public  HHPL_PROCESSOR
{
public:
	CEditScriptProc(CBarcodeFifo *pOutFifo);

	static bool DoDataEditOrScript(CBarcodeData *&pCurrentMsg, CBarcodeFifo *pOutFifo);
#ifdef PLUGIN_DEV
	static bool DoPluginFormat(CBarcodeData *&pCurrentMsg, CBarcodeFifo *pOutFifo);
#endif

protected:
	void ProcessHHPLProgram(HSTRING *pCode, CBarcodeData *&pCurrentMsg);

	virtual	bool	IsScriptingAvailable( void );
	virtual bool	SendToHost( SEND_TO_HOST_PARAMETERS *SendParameters );

	// helpers to keep the code more readable
	static bool CallIQEditScript(CBarcodeData *&pCurrentMsg, CBarcodeFifo *pOutFifo);
	static bool CallClassicDataEdit(CBarcodeData *&pCurrentMsg, CBarcodeFifo *pOutFifo);

// Data members
	CBarcodeFifo *m_pOutFifo;
};

inline bool DoDataEditOrScript(CBarcodeData *&pCurrentMsg, CBarcodeFifo *pOutFifo)
{
	return CEditScriptProc::DoDataEditOrScript(pCurrentMsg, pOutFifo);
}

#endif // EDIT_SCRIPT_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9



