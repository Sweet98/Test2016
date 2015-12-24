//======================================================================================
// HTransferProcessing.cpp
//======================================================================================
// $RCSfile: htransferprocessing.cpp $
// $Revision: 1.1 $
// $Date: 2009/03/08 16:04:30EDT $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------

#include "HTransferProcessing.h"

#include <stdio.h>
#include <string.h>




//---------------------------------------------------------------------------
//
//  Class name:     HTAG_POST_TRANSFER_PROCESSOR
//
//  Class description:
//
//      This class provides abstraction for protocol wrapped HTAG functions that need to forward data
//		on for processing.  All of it's members are intended to be overloaded with implementation routines.
//		By using this class, all processing can be insulated from the actual implementation, such as GUI,
//		that it drives.
//
//		Each of the functions is described in the include file for this class.
//
//	Dependencies:
//		StringAndData library (HSTRING support)
//		HTAG (this Communication library)
//
//	IMPORTANT NOTE:
//		In general, we will use ProcessHTAGData for all new data types, and we may migrate existing types to use
//		this function, thereby eliminating many of the functions below.  The reason for this is two-fold: there
//		are too many classes needing to be overloaded AND sometimes the data type class wants to reference the
//		transfer processor, but it can't do that if the transfer processor classes in turn reference it.  The
//		original intent of using several classes was that the application using this code may want to provide an
//		overload for images in one place, scripts in another, etc.  This can still be achieved by having the
//		application provide it's own overload layer on top of this.
//
//---------------------------------------------------------------------------




//
//  Constructor for class HTAG_POST_TRANSFER_PROCESSOR.
//		Currently, nothing to do.
//
HTAG_POST_TRANSFER_PROCESSOR::HTAG_POST_TRANSFER_PROCESSOR(void)
{
    return;
}
//---------------------------------------------------------------------------;





//
//  Destructor for class HTAG_POST_TRANSFER_PROCESSOR.
//		Currently, nothing to do.
//
HTAG_POST_TRANSFER_PROCESSOR::~HTAG_POST_TRANSFER_PROCESSOR(void)
{
    return;
}
//---------------------------------------------------------------------------;










//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//
//  Class name:     HTAG_IMAGE_POST_TRANSFER_PROCESSOR
//
//  Class description:
//
//      This class provides abstraction for protocol wrapped image transfer functions that need to forward
//		image data on for processing.  All of it's members are intended to be overloaded with implementation
//		routines.  By using this class, all image processing can be insulated from the actual implementation,
//		such as GUI, that it drives.
//
//		Each of the functions is described in the include file for this class.
//
//	Dependencies:
//		StringAndData library (HSTRING support)
//		HTAG_IMAGE (this Communication library)
//
//---------------------------------------------------------------------------




//
//  Constructor for class HTAG_IMAGE_POST_TRANSFER_PROCESSOR.
//		Currently, nothing to do.
//
HTAG_IMAGE_POST_TRANSFER_PROCESSOR::HTAG_IMAGE_POST_TRANSFER_PROCESSOR(void)
{
    return;
}
//---------------------------------------------------------------------------;





//
//  Destructor for class HTAG_IMAGE_POST_TRANSFER_PROCESSOR.
//		Currently, nothing to do.
//
HTAG_IMAGE_POST_TRANSFER_PROCESSOR::~HTAG_IMAGE_POST_TRANSFER_PROCESSOR(void)
{
    return;
}
//---------------------------------------------------------------------------;







//---------------------------------------------------------------------------
//
//  Class name:     HTAG_FILE_POST_TRANSFER_PROCESSOR
//
//  Class description:
//
//      This class provides abstraction for protocol wrapped file transfer functions that need to forward
//		file contents on for processing.  All of it's members are intended to be overloaded with implementation
//		routines.  By using this class, all file processing can be insulated from the actual implementation,
//		such as file I/O, that it drives.
//
//		Each of the functions is described in the include file for this class.
//
//	Dependencies:
//		StringAndData library (HSTRING support)
//		HTAG_FILE (this Communication library)
//
//---------------------------------------------------------------------------




//
//  Constructor for class HTAG_FILE_POST_TRANSFER_PROCESSOR.
//		Currently, nothing to do.
//
HTAG_FILE_POST_TRANSFER_PROCESSOR::HTAG_FILE_POST_TRANSFER_PROCESSOR(void)
{
    return;
}
//---------------------------------------------------------------------------;





//
//  Destructor for class HTAG_FILE_POST_TRANSFER_PROCESSOR.
//		Currently, nothing to do.
//
HTAG_FILE_POST_TRANSFER_PROCESSOR::~HTAG_FILE_POST_TRANSFER_PROCESSOR(void)
{
    return;
}
//---------------------------------------------------------------------------;







//---------------------------------------------------------------------------
//
//  Class name:     HTAG_SCRIPT_POST_TRANSFER_PROCESSOR
//
//  Class description:
//
//      This class provides abstraction for protocol wrapped script transfer functions that need to forward
//		script data on for processing.  All of it's members are intended to be overloaded with implementation
//		routines.  By using this class, all script data processing can be insulated from the actual implementation,
//		such as GUI, that it drives.
//
//		Each of the functions is described in the include file for this class.
//
//	Dependencies:
//		StringAndData library (HSTRING support)
//		HTAG_SCRIPT (this Communication library)
//
//---------------------------------------------------------------------------




//
//  Constructor for class HTAG_SCRIPT_POST_TRANSFER_PROCESSOR.
//		Currently, nothing to do.
//
HTAG_SCRIPT_POST_TRANSFER_PROCESSOR::HTAG_SCRIPT_POST_TRANSFER_PROCESSOR(void)
{
    return;
}
//---------------------------------------------------------------------------;





//
//  Destructor for class HTAG_SCRIPT_POST_TRANSFER_PROCESSOR.
//		Currently, nothing to do.
//
HTAG_SCRIPT_POST_TRANSFER_PROCESSOR::~HTAG_SCRIPT_POST_TRANSFER_PROCESSOR(void)
{
    return;
}
//---------------------------------------------------------------------------;







//---------------------------------------------------------------------------
//
//  Class name:     HTAG_SCRIPT_HOST_COMMAND_POST_TRANSFER_PROCESSOR
//
//  Class description:
//
//      This class provides abstraction for protocol wrapped script host command functions that need to forward
//		a host command on for processing.  All of it's members are intended to be overloaded with implementation
//		routines.  By using this class, all host command processing can be insulated from the actual implementation,
//		such as GUI, that it drives.
//
//		Each of the functions is described in the include file for this class.
//
//	Dependencies:
//		StringAndData library (HSTRING support)
//		HTAG_SCRIPT_HOST_COMMAND (this Communication library)
//
//---------------------------------------------------------------------------




//
//  Constructor for class HTAG_SCRIPT_HOST_COMMAND_POST_TRANSFER_PROCESSOR.
//		Currently, nothing to do.
//
HTAG_SCRIPT_HOST_COMMAND_POST_TRANSFER_PROCESSOR::HTAG_SCRIPT_HOST_COMMAND_POST_TRANSFER_PROCESSOR(void)
{
    return;
}
//---------------------------------------------------------------------------;





//
//  Destructor for class HTAG_SCRIPT_HOST_COMMAND_POST_TRANSFER_PROCESSOR.
//		Currently, nothing to do.
//
HTAG_SCRIPT_HOST_COMMAND_POST_TRANSFER_PROCESSOR::~HTAG_SCRIPT_HOST_COMMAND_POST_TRANSFER_PROCESSOR(void)
{
    return;
}
//---------------------------------------------------------------------------;







//---------------------------------------------------------------------------
//
//  Class name:     HTAG_SCRIPT_HOST_DATA_POST_TRANSFER_PROCESSOR
//
//  Class description:
//
//      This class provides abstraction for protocol wrapped script host data functions that need to forward
//		host data on for processing.  All of it's members are intended to be overloaded with implementation
//		routines.  By using this class, all host data processing can be insulated from the actual implementation,
//		such as GUI, that it drives.
//
//		Each of the functions is described in the include file for this class.
//
//	Dependencies:
//		StringAndData library (HSTRING support)
//		HTAG_SCRIPT_HOST_DATA (this Communication library)
//
//---------------------------------------------------------------------------




//
//  Constructor for class HTAG_SCRIPT_HOST_DATA_POST_TRANSFER_PROCESSOR.
//		Currently, nothing to do.
//
HTAG_SCRIPT_HOST_DATA_POST_TRANSFER_PROCESSOR::HTAG_SCRIPT_HOST_DATA_POST_TRANSFER_PROCESSOR(void)
{
    return;
}
//---------------------------------------------------------------------------;





//
//  Destructor for class HTAG_SCRIPT_HOST_DATA_POST_TRANSFER_PROCESSOR.
//		Currently, nothing to do.
//
HTAG_SCRIPT_HOST_DATA_POST_TRANSFER_PROCESSOR::~HTAG_SCRIPT_HOST_DATA_POST_TRANSFER_PROCESSOR(void)
{
    return;
}
//---------------------------------------------------------------------------;







//---------------------------------------------------------------------------
//
//  Class name:     READER_DEVICE_EVENT_PROCESSOR
//
//  Class description:
//
//      This class provides abstraction for protocol wrapped reader device events that need to be passed on to
//		the application.  All of it's members are intended to be overloaded with implementation routines.  By
//		using this class, all event processing can be insulated from the actual implementation, such as GUI,
//		that it drives.
//
//		Each of the functions is described in the include file for this class.
//
//	Dependencies:
//		None
//
//---------------------------------------------------------------------------




//
//  Constructor for class READER_DEVICE_EVENT_PROCESSOR.
//		Currently, nothing to do.
//
READER_DEVICE_EVENT_PROCESSOR::READER_DEVICE_EVENT_PROCESSOR(void)
{
    return;
}
//---------------------------------------------------------------------------;





//
//  Destructor for class READER_DEVICE_EVENT_PROCESSOR.
//		Currently, nothing to do.
//
READER_DEVICE_EVENT_PROCESSOR::~READER_DEVICE_EVENT_PROCESSOR(void)
{
    return;
}
//---------------------------------------------------------------------------;







//---------------------------------------------------------------------------
//
//  Class name:     HTAG_CODE_POST_TRANSFER_PROCESSOR
//
//  Class description:
//
//      This class provides abstraction for protocol wrapped code transfer functions that need to forward
//		code images on for processing.  All of it's members are intended to be overloaded with implementation
//		routines.  By using this class, all code processing can be insulated from the actual implementation,
//		such as GUI, that it drives.
//
//		Each of the functions is described in the include file for this class.
//
//	Dependencies:
//		StringAndData library (HSTRING support)
//		HTAG_READER_CODE (this Communication library)
//
//---------------------------------------------------------------------------




//
//  Constructor for class HTAG_CODE_POST_TRANSFER_PROCESSOR.
//		Currently, nothing to do.
//
HTAG_CODE_POST_TRANSFER_PROCESSOR::HTAG_CODE_POST_TRANSFER_PROCESSOR(void)
{
    return;
}
//---------------------------------------------------------------------------;





//
//  Destructor for class HTAG_CODE_POST_TRANSFER_PROCESSOR.
//		Currently, nothing to do.
//
HTAG_CODE_POST_TRANSFER_PROCESSOR::~HTAG_CODE_POST_TRANSFER_PROCESSOR(void)
{
    return;
}
//---------------------------------------------------------------------------;






