//======================================================================================
// HComm.cpp
//======================================================================================
// $RCSfile: hcomm.cpp $
// $Revision: 1.1 $
// $Date: 2009/03/08 16:03:34EDT $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Class name:     HCOMM
//
//  Class description:
//
//      This class provides abstraction for device communication.  All of it's I/O members are
//		intended to be overloaded with implementation routines.  By using this class, all reader
//		communication support routines can be insulated from the actual implementation and device
//		type.
//
//		Each of the functions is described in the include file for this class.
//
//	Dependencies:
//		StringAndData library (HSTRING support)
//
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

#include "HComm.h"

#include <stdio.h>
#include <string.h>



//---------------------------------------------------------------------------





//
//  Constructor for class HCOMM.
//		Currently, nothing to do.
//
HCOMM::HCOMM(void)
{
	DefaultCharacterReadTimeout = DEFAULT_CHARACTER_READ_TIMEOUT;
	DefaultCharacterReadTimeoutForHTAGNoProtocol = DEFAULT_CHARACTER_READ_TIMEOUT;
	DefaultCharacterReadTimeoutForHTAGNoProtocolFlashImageTransfers = DEFAULT_CHARACTER_READ_TIMEOUT;


	WaitTimeForHTAGCommand = HTAG_DEFAULT_WAIT_TIME_FOR_COMMAND;
	MaximumHTAGCommandSize = HTAG_DEFAULT_MAXIMUM_COMMAND_SIZE;

	WaitTimeForScanEngineFirmwareDownload = DEFAULT_WAIT_TIME_FOR_SCAN_ENGINE_FIRMWARE_DOWNLOAD;

    return;
}
//---------------------------------------------------------------------------;





//
//  Destructor for class HCOMM.
//		Currently, nothing to do.
//
HCOMM::~HCOMM(void)
{
    return;
}
//---------------------------------------------------------------------------;





