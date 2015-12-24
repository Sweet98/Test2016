//======================================================================================
// HCommandHandler.cpp
//======================================================================================
// $RCSfile: hcommandhandler.cpp $
// $Revision: 1.1 $
// $Date: 2009/03/08 16:03:37EDT $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Class name:     HCOMMAND_HANDLER
//
//  Class description:
//
//      This class provides abstraction for sending common commands, such as menu commands and non-menu commands, to
//		a device.  All of it's members are intended to be overload with implementation routines.  By using this class,
//		all routines that need to use these commands can be insulated from the actual implementation.
//
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

#include "HCommandHandler.h"

#include <stdio.h>
#include <string.h>



//---------------------------------------------------------------------------





//
//  Constructor for class HCOMMAND_HANDLER.
//		Currently, nothing to do.
//
HCOMMAND_HANDLER::HCOMMAND_HANDLER(void)
{
    return;
}
//---------------------------------------------------------------------------;





//
//  Destructor for class HCOMMAND_HANDLER.
//		Currently, nothing to do.
//
HCOMMAND_HANDLER::~HCOMMAND_HANDLER(void)
{
    return;
}
//---------------------------------------------------------------------------;





