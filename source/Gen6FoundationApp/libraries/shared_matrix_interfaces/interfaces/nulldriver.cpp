/*=================================================================================
  class: CNullDriver - byte bucket

  This will silently Write data (with success) and never Read any data.

//=================================================================================
   $Source: interfaces/nulldriver.cpp $
   $Date: 2011/06/06 15:19:17EDT $
   $Revision:


//=================================================================================*/
#include "stdInclude.h"
#include "CommonDriver.h"
#include "NullDriver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
	CNullDriver is what the name says: a black hole for data.
*/


// This is the class factory for this driver
CDriverBase *CreateNullDriver(void)
{
	return (new CNullDriver);
}


/*----------------------------------------------------------------------------------------
DeviceIoControl

  We need to handle IOCTRL_IS_DONE or else the default action is to wait.

  Parameters:  
		cmd[in]:  the I/O control ID

		lparam[in]:  parameter

		wparam[in]:  parameter


  Returns:  IOCRET_YES if IOCTRL_IS_DONE, IOCRET_DEFAULT otherwise
----------------------------------------------------------------------------------------*/
IoControlRet_t CNullDriver::DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam)
{
	if ( cmd == IOCTRL_IS_DONE ) return IOCRET_YES;
	else return IOCRET_DEFAULT;
}





