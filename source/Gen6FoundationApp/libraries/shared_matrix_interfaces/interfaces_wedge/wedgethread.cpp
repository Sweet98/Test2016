/*=================================================================================
  class: CWedgeThread

//=================================================================================
   $Source: interfaces_wedge/wedgethread.cpp $
   $Date: 2009/06/29 05:03:52EDT $
   $Revision: 1.8 $
 


//=================================================================================*/

#include "stdInclude.h"
#include "CommonDriver.h"
#include "ThreadedDriver.h"
#include "WedgeThread.h"
#include "db_chrio.h"

 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor
CWedgeThread::CWedgeThread(CThreadedDriver *pDriver, const CWedgeTerminal *pTerminal)    
	:baseCWedgeThread(pDriver, pTerminal)
{

}

// Destructor
CWedgeThread::~CWedgeThread()
{
}

MatrixError_t CWedgeThread::Open(interface_id_t nId, const tichar_t *pParameter)
{

	MatrixError_t ret = baseCWedgeThread::Open(nId, pParameter);

	return ret;
}

MatrixError_t CWedgeThread::Close(void)
{
	return baseCWedgeThread::Close();	   // call base class for cleanup there
}






