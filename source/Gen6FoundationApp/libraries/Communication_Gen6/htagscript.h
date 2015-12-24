//======================================================================================
// HTagScript.h
//======================================================================================
// $RCSfile: htagscript.h $
// $Revision: 1.2 $
// $Date: 2011/11/11 17:01:41EST $
//======================================================================================

//======================================================================================



//---------------------------------------------------------------------------
#ifndef HTAG_SCRIPT_H
#define HTAG_SCRIPT_H
//---------------------------------------------------------------------------


#include "HTag.h"




//---------------------------------------------------------------------------
//
//  Class definitions for HTAG_SCRIPT
//
//---------------------------------------------------------------------------

//
//	The HTAG_SCRIPT class provides a mechanism to transfer HHPL programs.  Since all fields are tagged, if the
//	receiver does not yet speak the new version of HTAG_SCRIPT, it will ignore tags that it doesn't understand.
//



class HTAG_SCRIPT : public HTAG
{
public:
// Status codes
	typedef enum {
		HTAG_SCRIPT_FILE_NAME = HTAG::HTAG_USER_DEFINED_STATUS_BEGIN,
		HTAG_SCRIPT_WORKGROUP
	} HTAG_SCRIPT_STATUS;


// Workgroup information.
	#define	HTAG_SCRIPT_DEFAULT_WORKGROUP				0
	#define	HTAG_SCRIPT_WORKGROUP_MINIMUM				0
	#define	HTAG_SCRIPT_WORKGROUP_MAXIMUM				9




// Information about the script.
	#define	HTAG_SCRIPT_FILE_PARAMETER_NAME				"FILE"
	#define	HTAG_SCRIPT_WORKGROUP_PARAMETER_NAME		"WG"
	#define	HTAG_SCRIPT_DEFAULT_PARAMETER_NAME			"DEF"

	HSTRING		*ScriptName;
	int			ScriptWorkgroup;
	int			DefaultScript;



public:


// Pointer to the actual script.
	#define	DEFAULT_SCRIPT_DATA_ALLOCATION				200
	HSTRING		*Data;




// Constructor for HTAG_SCRIPT; overloaded version #1 for loading scripts later.
    HTAG_SCRIPT(const char *Command, HCOMM *HCommDevice = 0, HTRANSFER_STATUS *TransferStatus = 0,
								HCOMMAND_HANDLER *CommandHandler = 0);

// Destructor for HTAG_SCRIPT
    ~HTAG_SCRIPT(void);

private:
	HTAG_SCRIPT (const HTAG_SCRIPT&);            // declared private, but not defined, to prevent copying         
	HTAG_SCRIPT& operator=(const HTAG_SCRIPT&);  // declared private, but not defined, to prevent copying 

};
//----------------------------------------------------------------------------










//---------------------------------------------------------------------------
//
//  Class definitions for HTAG_SCRIPT_HOST_COMMAND
//
//---------------------------------------------------------------------------

//
//	The HTAG_SCRIPT_HOST_COMMAND class provides a mechanism to transfer HHPL host commands.  Since all fields are tagged, if the
//	receiver does not yet speak the new version of HTAG_SCRIPT_HOST_COMMAND, it will ignore tags that it doesn't understand.
//



class HTAG_SCRIPT_HOST_COMMAND : public HTAG
{
public:


// Pointer to the actual host command.
	#define	DEFAULT_HOST_COMMAND_ALLOCATION				200
	HSTRING		*Data;




// Constructor for HTAG_SCRIPT_HOST_COMMAND; overloaded version #1 for loading host commands later.
    HTAG_SCRIPT_HOST_COMMAND(const char *Command, HCOMM *HCommDevice = 0, HTRANSFER_STATUS *TransferStatus = 0,
								HCOMMAND_HANDLER *CommandHandler = 0);

// Destructor for HTAG_SCRIPT_HOST_COMMAND
    ~HTAG_SCRIPT_HOST_COMMAND(void);

private:
	HTAG_SCRIPT_HOST_COMMAND (const HTAG_SCRIPT_HOST_COMMAND&);            // declared private, but not defined, to prevent copying         
	HTAG_SCRIPT_HOST_COMMAND& operator=(const HTAG_SCRIPT_HOST_COMMAND&);  // declared private, but not defined, to prevent copying 

};
//----------------------------------------------------------------------------










//---------------------------------------------------------------------------
//
//  Class definitions for HTAG_SCRIPT_HOST_DATA
//
//---------------------------------------------------------------------------

//
//	The HTAG_SCRIPT_HOST_DATA class provides a mechanism to transfer HHPL host data.  Since all fields are tagged, if the
//	receiver does not yet speak the new version of HTAG_SCRIPT_HOST_DATA, it will ignore tags that it doesn't understand.
//



class HTAG_SCRIPT_HOST_DATA : public HTAG
{
public:


// Pointer to the actual host data.
	#define	DEFAULT_HOST_DATA_ALLOCATION				200
	HSTRING		*Data;




// Constructor for HTAG_SCRIPT_HOST_DATA; overloaded version #1 for loading host data later.
    HTAG_SCRIPT_HOST_DATA(const char *Command, HCOMM *HCommDevice = 0, HTRANSFER_STATUS *TransferStatus = 0,
								HCOMMAND_HANDLER *CommandHandler = 0);

// Destructor for HTAG_SCRIPT_HOST_DATA
    ~HTAG_SCRIPT_HOST_DATA(void);

private:
	HTAG_SCRIPT_HOST_DATA (const HTAG_SCRIPT_HOST_DATA&);            // declared private, but not defined, to prevent copying         
	HTAG_SCRIPT_HOST_DATA& operator=(const HTAG_SCRIPT_HOST_DATA&);  // declared private, but not defined, to prevent copying 

};
//----------------------------------------------------------------------------




#endif // HTAG_SCRIPT_H



