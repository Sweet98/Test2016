//======================================================================================
// HTagFile.h
//======================================================================================
// $RCSfile: HTagFile.h $
// $Revision: 1.3 $
// $Date: 2011/11/08 18:02:47EST $
//======================================================================================

//======================================================================================



//---------------------------------------------------------------------------
#ifndef HTAG_FILE_H
#define HTAG_FILE_H
//---------------------------------------------------------------------------


#include "HTag.h"




//---------------------------------------------------------------------------
//
//  Class definitions for HTAG_FILE
//
//---------------------------------------------------------------------------

//
//	The HTAG_FILE class provides a mechanism to transfer files.  Since all fields are tagged, if the
//	receiver does not yet speak the new version of HTAG_FILE, it will ignore tags that it doesn't understand.
//



class HTAG_FILE : public HTAG
{
public:
// Status codes
//	typedef enum {
//		HTAG_FILE_FILE_NAME = HTAG::HTAG_USER_DEFINED_STATUS_BEGIN,
//		HTAG_FILE_WORKGROUP
//	} HTAG_FILE_STATUS;


// Workgroup information.
	#define	HTAG_FILE_DEFAULT_WORKGROUP				0
	#define	HTAG_FILE_WORKGROUP_MINIMUM				0
	#define	HTAG_FILE_WORKGROUP_MAXIMUM				9




// Information about the script.
	#define	HTAG_FILE_FILE_PARAMETER_NAME				"FILE"
	#define	HTAG_FILE_COMPRESSION_TYPE_PARAMETER_NAME	"CMP"
	#define	HTAG_FILE_TYPE_NAME							"TYPE"
	#define	HTAG_FILE_WORKGROUP_PARAMETER_NAME			"WG"

	HSTRING		*FileName;
	int			CompressionType;
	int			FileWorkgroup;

	typedef enum {
		HTAG_FILE_TYPE_MAIN_APPLICATION = 3,
		HTAG_FILE_TYPE_MAIN_APPLICATION_RUN_FROM_ROM,
		HTAG_FILE_TYPE_WORKING_READER_CONFIGURATION,
		HTAG_FILE_TYPE_DEFAULT_READER_CONFIGURATION,
		HTAG_FILE_TYPE_MAIN_SCRIPT,
		HTAG_FILE_TYPE_EDIT_SCRIPT,
		HTAG_FILE_TYPE_GENERIC,
		HTAG_FILE_TYPE_RESERVED,
		HTAG_FILE_TYPE_PROKEY_FILE,
		HTAG_FILE_TYPE_MOCF_FILE,  // 12
		HTAG_FILE_TYPE_MAXIMUM_FILE_TYPES
	} HTAG_FILE_TYPE;

	int			FileType;



public:


// Pointer to the actual file data.
	#define	DEFAULT_FILE_DATA_ALLOCATION				200
	HSTRING		*Data;




// Constructor for HTAG_FILE; overloaded version #1 for loading reader code later.
    HTAG_FILE(const char *Command, HCOMM *HCommDevice = 0, HTRANSFER_STATUS *TransferStatus = 0,
								HCOMMAND_HANDLER *CommandHandler = 0);

// Destructor for HTAG_FILE
    ~HTAG_FILE(void);

private:
	HTAG_FILE (const HTAG_FILE&);            // declared private, but not defined, to prevent copying         
	HTAG_FILE& operator=(const HTAG_FILE&);  // declared private, but not defined, to prevent copying 

};
//----------------------------------------------------------------------------




#endif // HTAG_FILE_H



