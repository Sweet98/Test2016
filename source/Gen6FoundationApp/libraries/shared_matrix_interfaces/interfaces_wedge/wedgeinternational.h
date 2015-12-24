/*=================================================================================
  class: CWedgeInternational

//=================================================================================
   $Source: interfaces_wedge/wedgeinternational.h $
   $Date: 2009/06/29 05:03:32EDT $
   $Revision: 2.1 $



//=================================================================================*/

#ifndef WEDGE_INTERNATIONAL__H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define WEDGE_INTERNATIONAL__H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "WedgeTranslate.h"

//struct CKeyFixup;
//struct CKeyFixups;

#define baseCWedgeInternational CWedgeTranslate
class CWedgeInternational : public baseCWedgeInternational
{
public:
	// two overloads for country testing
	bool IsCountrySupported(const CKeyFixups *pFix, int country);
	bool IsCountrySupported(int country);
	
	CWedgeInternational(CDriverBase *pDriver, const CWedgeTerminal *pTerminal);

	void DoInternationalSupport(const CKeyFixups *pp, int country);

private:
	void DoInternationalExceptions(const CKeyFixup *pExc);
};

#endif /* WEDGE_INTERNATIONAL__H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */




