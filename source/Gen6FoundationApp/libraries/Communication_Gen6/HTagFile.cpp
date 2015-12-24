//======================================================================================
// HTagFile.cpp
//======================================================================================
// $RCSfile: HTagFile.cpp $
// $Revision: 1.1 $
// $Date: 2009/03/08 16:04:14EDT $
//======================================================================================

//======================================================================================


#include	"Compression.h"			// Compression interface definitions
#include	"HTagFile.h"			// HTAG_FILE interface definition

#include	<stdlib.h>				// Standard library functions
#include	<stdio.h>
#include	<string.h>

//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//
//  Class Name:     HTAG_FILE
//
//  Class Description:
//		The HTAG_FILE class provides a mechanism to transfer files.  Since all fields are tagged, if the
//		receiver does not yet speak the new version of HTAG_FILE, it will ignore tags that it doesn't understand.
//
//---------------------------------------------------------------------------







//
// Constructor for HTAG_FILE; overloaded version #1 for loading files later.
//      Indicate that no file has been loaded yet by setting Data size to zero.
//
HTAG_FILE::HTAG_FILE(const char *Command, HCOMM *HCommDevice, HTRANSFER_STATUS *TransferStatus,
								HCOMMAND_HANDLER *CommandHandler)
		: HTAG( HCommDevice, TransferStatus, CommandHandler, false )
{
	Data = new HSTRING(DEFAULT_FILE_DATA_ALLOCATION, true);
	HTAG::Data = Data;				// Give the pointer to the base class so it can operate on the data as well.
	CommandName->Copy(Command);

// Add file name, compression type, and file type parameters.
	FileName = new HSTRING(20, true);
	AddHStringParameter(HTAG_FILE_FILE_PARAMETER_NAME, FileName, "UNKNOWN", 7, true );
	AddNumericParameter(HTAG_FILE_COMPRESSION_TYPE_PARAMETER_NAME, &CompressionType, NO_COMPRESSION,
					NO_COMPRESSION, INVALID_COMPRESSION_MODE - 1, true );
	AddNumericParameter(HTAG_FILE_TYPE_NAME, &FileType, HTAG_FILE_TYPE_GENERIC,
					HTAG_FILE_TYPE_MAIN_APPLICATION, HTAG_FILE_TYPE_MAXIMUM_FILE_TYPES - 1, true );
	AddNumericParameter(HTAG_FILE_WORKGROUP_PARAMETER_NAME, &FileWorkgroup, HTAG_FILE_DEFAULT_WORKGROUP,
					HTAG_FILE_WORKGROUP_MINIMUM, HTAG_FILE_WORKGROUP_MAXIMUM, true );

    return;
}
//---------------------------------------------------------------------------





//
// Destructor for HTAG_FILE
//      If memory has been allocated to reader code, release it.
//
HTAG_FILE::~HTAG_FILE(void)
{
	delete Data;
	delete FileName;

    return;
}
//---------------------------------------------------------------------------







