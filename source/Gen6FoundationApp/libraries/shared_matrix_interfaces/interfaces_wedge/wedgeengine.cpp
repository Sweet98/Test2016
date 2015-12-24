/*=================================================================================
  class: CWedgeEngine

//=================================================================================
   $Source: interfaces_wedge/wedgeengine.cpp $
   $Date: 2009/11/16 13:54:26EST $
   $Revision: 1.7 $



//=================================================================================*/

#include "stdInclude.h"
#include "CommonDriver.h"
#include "WedgeEngine.h"
#include "ThreadRealtime.h"
#include "Bitbanger.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor
CWedgeEngine::CWedgeEngine(CThreadedDriver *pDriver, const CWedgeTerminal *pTerminal, CBitBanger *pBitBanger)
	:baseCWedgeEngine(pDriver, pTerminal)
{
	ASSERT(pBitBanger!=NULL);
	m_pBitBang = pBitBanger;
	m_pBitBang->CreateMailBox();	     // the bitbanger needs access to the thread object
}

// Destructor
CWedgeEngine::~CWedgeEngine()
{
	delete m_pBitBang;	                  // it is our responsibilty to cleanup the bitbanger
	m_pBitBang = NULL;
}

MatrixError_t CWedgeEngine::Close(void)
{
	m_pBitBang->Close();
	return baseCWedgeEngine::Close();
}

MatrixError_t CWedgeEngine::BitBangOut(unsigned int data, unsigned int extrainfo)
{
	ASSERT(m_pBitBang!=NULL);
	return m_pBitBang->BitBangOutTerm(data, extrainfo);
}

MatrixError_t CWedgeEngine::BitBangIn(unsigned int &uiReturnData)
{
	ASSERT(m_pBitBang!=NULL);

	return m_pBitBang->BitBangInTerm(uiReturnData);
}

MatrixError_t CWedgeEngine::OpenKeyboardEmulation(void)
{
	return ERR_PASS;
}

MatrixError_t CWedgeEngine::CheckKeyboardEmulation(void)
{
	return ERR_BB_READ_NoData;	            // at the end this means not to abort the delays
}




