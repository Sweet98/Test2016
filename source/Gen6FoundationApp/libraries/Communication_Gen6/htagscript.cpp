//======================================================================================
// HTagScript.cpp
//======================================================================================
// $RCSfile: htagscript.cpp $
// $Revision: 1.1 $
// $Date: 2009/03/08 16:04:23EDT $
//======================================================================================

//======================================================================================


#include	"HTagScript.h"		// HTAG_SCRIPT interface definition

#include	<stdlib.h>		/* standard library functions				*/
#include	<stdio.h>
#include	<string.h>

//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//
//  Class Name:     HTAG_SCRIPT
//
//  Class Description:
//		The HTAG_SCRIPT class provides a mechanism to transfer HHPL programs.  Since all fields are tagged, if the
//		receiver does not yet speak the new version of HTAG_SCRIPT, it will ignore tags that it doesn't understand.
//
//---------------------------------------------------------------------------







//
// Constructor for HTAG_SCRIPT; overloaded version #1 for loading scripts later.
//      Indicate that no script has been loaded yet by setting Data size to zero.
//
HTAG_SCRIPT::HTAG_SCRIPT(const char *Command, HCOMM *HCommDevice, HTRANSFER_STATUS *TransferStatus,
								HCOMMAND_HANDLER *CommandHandler)
		: HTAG( HCommDevice, TransferStatus, CommandHandler, false )
{
	Data = new HSTRING(DEFAULT_SCRIPT_DATA_ALLOCATION, true);
	HTAG::Data = Data;				// Give the pointer to the base class so it can operate on the data as well.
	CommandName->Copy(Command);

// Add file name and workgroup parameters.
	ScriptName = new HSTRING(20, true);
	AddHStringParameter(HTAG_SCRIPT_FILE_PARAMETER_NAME, ScriptName, "UNKNOWN", 7, true );
	AddNumericParameter(HTAG_SCRIPT_WORKGROUP_PARAMETER_NAME, &ScriptWorkgroup, HTAG_SCRIPT_DEFAULT_WORKGROUP,
					HTAG_SCRIPT_WORKGROUP_MINIMUM, HTAG_SCRIPT_WORKGROUP_MAXIMUM, true );
	AddNumericParameter(HTAG_SCRIPT_DEFAULT_PARAMETER_NAME, &DefaultScript, 0, 0, 1, true );

    return;
}
//---------------------------------------------------------------------------





//
// Destructor for HTAG_SCRIPT
//      If memory has been allocated to a script, release it.
//
HTAG_SCRIPT::~HTAG_SCRIPT(void)
{
	delete Data;
	delete ScriptName;

    return;
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//
//  Class Name:     HTAG_SCRIPT_HOST_COMMAND
//
//  Class Description:
//		The HTAG_SCRIPT_HOST_COMMAND class provides a mechanism to transfer HHPL host commands.  Since all fields are tagged,
//		if the receiver does not yet speak the new version of HTAG_SCRIPT_HOST_COMMAND, it will ignore tags that it doesn't
//		understand.
//
//---------------------------------------------------------------------------






//
// Constructor for HTAG_SCRIPT_HOST_COMMAND
//      Indicate that no host command has been loaded yet by setting Data size to zero.
//
HTAG_SCRIPT_HOST_COMMAND::HTAG_SCRIPT_HOST_COMMAND(const char *Command, HCOMM *HCommDevice, HTRANSFER_STATUS *TransferStatus,
								HCOMMAND_HANDLER *CommandHandler)
		: HTAG( HCommDevice, TransferStatus, CommandHandler, false )
{
	Data = new HSTRING(DEFAULT_HOST_COMMAND_ALLOCATION, true);
	HTAG::Data = Data;				// Give the pointer to the base class so it can operate on the data as well.
	CommandName->Copy(Command);

    return;
}
//---------------------------------------------------------------------------





//
// Destructor for HTAG_SCRIPT_HOST_COMMAND
//      If memory has been allocated , release it.
//
HTAG_SCRIPT_HOST_COMMAND::~HTAG_SCRIPT_HOST_COMMAND(void)
{
	delete Data;

    return;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//
//  Class Name:     HTAG_SCRIPT_HOST_DATA
//
//  Class Description:
//		The HTAG_SCRIPT_HOST_DATA class provides a mechanism to transfer HHPL host data.  Since all fields are tagged,
//		if the receiver does not yet speak the new version of HTAG_SCRIPT_HOST_DATA, it will ignore tags that it doesn't
//		understand.
//
//---------------------------------------------------------------------------






//
// Constructor for HTAG_SCRIPT_HOST_DATA
//      Indicate that no host data has been loaded yet by setting Data size to zero.
//
HTAG_SCRIPT_HOST_DATA::HTAG_SCRIPT_HOST_DATA(const char *Command, HCOMM *HCommDevice, HTRANSFER_STATUS *TransferStatus,
								HCOMMAND_HANDLER *CommandHandler)
		: HTAG( HCommDevice, TransferStatus, CommandHandler, false )
{
	Data = new HSTRING(DEFAULT_HOST_COMMAND_ALLOCATION, true);
	HTAG::Data = Data;				// Give the pointer to the base class so it can operate on the data as well.
	CommandName->Copy(Command);

    return;
}
//---------------------------------------------------------------------------





//
// Destructor for HTAG_SCRIPT_HOST_DATA
//      If memory has been allocated , release it.
//
HTAG_SCRIPT_HOST_DATA::~HTAG_SCRIPT_HOST_DATA(void)
{
	delete Data;

    return;
}
//---------------------------------------------------------------------------





