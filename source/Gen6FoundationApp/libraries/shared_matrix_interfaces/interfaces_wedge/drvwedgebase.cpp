/*=================================================================================
  class: CWedgeDriver

//=================================================================================
   $Source: interfaces_wedge/drvwedgebase.cpp $
   $Date: 2011/08/24 11:03:53EDT $
   $Revision: 1.16 $



//=================================================================================*/

#include "stdInclude.h"
#include "CommonDriver.h"
#include "DrvWedgeBase.h"
#include "WedgeEngine.h"
#include "BitBanger.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// default Constructor, use it if you derive from this call
CWedgeDriver::CWedgeDriver(void)
{
	m_pWedge = NULL;
	m_OutputFormat = WEDGE_DATATYPE;
}

// Constructor 2
CWedgeDriver::CWedgeDriver(const CWedgeTerminal *pTerminal, CBitBanger *pBitBanger)
{
	ASSERT(pBitBanger!=NULL);
	ASSERT(pTerminal!=NULL);
	m_pWedge = new CWedgeEngine(this, pTerminal, pBitBanger);
	m_OutputFormat = WEDGE_DATATYPE;
}

// Destructor
CWedgeDriver::~CWedgeDriver()
{
	delete m_pWedge;
	m_pWedge = NULL;	                     // allow for a derived class to do delete m_pWedge in a save way.
}

IoControlRet_t CWedgeDriver::DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam)
{
	IoControlRet_t RetVal=baseCWedgeDriver::DeviceIoControl(cmd, lparam, wparam);
	switch(cmd)
	{
		case IOCTRL_IS_INTERPRET_DATA:
			RetVal = IOCRET_YES;
			break;
		case IOCTRL_IS_ANA_SWITCH_ON_REQUIRED:
			if (!m_pWedge->IsKbdStyleKeyEmu())
				RetVal = IOCRET_YES;
			break;
		case IOCTRL_IS_COUNTRY_SUPPORTED:
			if (m_pWedge->IsCountrySupported(lparam))
				RetVal = IOCRET_YES;
			break;
		case IOCTRL_SHALL_I_DELAY:
			RetVal = IOCRET_YES;
			break;
		case IOCTRL_NEED_PRESUFFIX_ESC:
			if(m_pWedge->NeedsPreSuffixEsc())
				RetVal = IOCRET_YES;
			break;
		default:
			break;
	}

	return RetVal;
}

MatrixError_t CWedgeDriver::Open(interface_id_t nId, const tichar_t *pParameter)
{
	ASSERT(m_pWedge!=NULL);
	MatrixError_t ret = baseCWedgeDriver::Open(nId, pParameter);
	if(IsOK(ret))
	{
		ret = m_pWedge->Open(nId, pParameter);
		if(IsOK(ret))
		{
			RunThread();
//			m_pThread->Enable();
		}
	}
	return ret;
}

MatrixError_t CWedgeDriver::Close(void)
{
	ASSERT(m_pWedge!=NULL);
	m_pWedge->Close();
	return baseCWedgeDriver::Close();
}

/*
 Overloaded ThreadLoop.
 This function runs as a second thread of execution.
 Here we just call the loop inside the wedge engine.
*/
void CWedgeDriver::ThreadLoop(void)
{
	ASSERT(m_pWedge!=NULL);
	m_pWedge->SendLabelThread();
}





