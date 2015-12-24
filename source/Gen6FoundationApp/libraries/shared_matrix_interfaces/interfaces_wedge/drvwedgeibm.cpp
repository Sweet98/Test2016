/*=================================================================================
  IBM family of wedge interfaces

//=================================================================================
   $Source: interfaces_wedge/drvwedgeibm.cpp $
   $Date: 2010/02/11 00:19:41EST $
   $Revision: 1.4 $



//=================================================================================*/

#include "stdInclude.h"
#include "CommonDriver.h"
#include "WedgeEngineIBM.h"
#include "DrvWedgeIBM.h"
#include "BbIbm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern const CWedgeTerminal IBM_PS2_TERM;
extern const CWedgeTerminal IBM_AT_TERM;
extern const CWedgeTerminal IBM_3486_102_TERM;
extern const CWedgeTerminal IBM_3191_122_TERM;
extern const CWedgeTerminal IBM_3486_122_TERM;
extern const CWedgeTerminal IBM_3180_TERM;
extern const CWedgeTerminal IBM_3180_DATA_ENTRY_TERM;

//extern const CWedgeTerminal IBM_THINKPAD_AT_TERM;
//extern const CWedgeTerminal IBM_THINKPAD_PS2_TERM;

// This is the class factory for this driver
CDriverBase *CreateIBM_PS2(void)
{
	return (new CWedgeIBMFamily(&IBM_PS2_TERM, 1));
}

// This is the class factory for this driver
CDriverBase *CreateIBM_AT(void)
{
	return (new CWedgeIBMFamily(&IBM_AT_TERM, 2));
}

// This is the class factory for this driver
CDriverBase *CreateIBM_3486_102(void)
{
	return (new CWedgeIBMFamily(&IBM_3486_102_TERM, 3));
}

// This is the class factory for this driver
CDriverBase *CreateIBM_3191_122(void)
{
	return (new CWedgeIBMFamily(&IBM_3191_122_TERM, 3));
}

// This is the class factory for this driver
CDriverBase *CreateIBM_3486_122(void)
{
	return (new CWedgeIBMFamily(&IBM_3486_122_TERM, 3));
}

// This is the class factory for this driver
CDriverBase *CreateIBM_3180(void)
{
	return (new CWedgeIBMFamily(&IBM_3180_TERM, 3));
}

// This is the class factory for this driver
CDriverBase *CreateIBM_3180_DATA_ENTRY(void)
{
	return (new CWedgeIBMFamily(&IBM_3180_DATA_ENTRY_TERM, 3));
}

// Constructor
CWedgeIBMFamily::CWedgeIBMFamily(const CWedgeTerminal *pTerminal, UINT ScancodeSet)
{
	m_pWedge = new CWedgeEngineIBM(this, pTerminal, ScancodeSet, new CBitBangIBM);
}

// Destructor
CWedgeIBMFamily::~CWedgeIBMFamily()
{
	delete m_pWedge;
	m_pWedge = NULL;
}

size_t CWedgeIBMFamily::Write(CBarcodeData  *pLabel)
{
	size_t ret = CDriverBase::Write(pLabel);
	return ret;
}




