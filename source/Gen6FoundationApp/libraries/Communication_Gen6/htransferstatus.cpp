//======================================================================================
// HTransferStatus.cpp
//======================================================================================
// $RCSfile: htransferstatus.cpp $
// $Revision: 1.1 $
// $Date: 2009/03/08 16:04:33EDT $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Class name:     HTRANSFER_STATUS
//
//  Class description:
//
//      This class provides abstraction for protocol wrapped data transfer functions that provide
//		feedback to the instantiator about the progress of the transfer.  All of it's members are
//		intended to be overloaded with implementation routines.  By using this class, all reader
//		protocol wrapped data transfer status routines can be insulated from the actual implementation,
//		such as GUI, that it drives.
//
//		Each of the functions is described in the include file for this class.
//
//	Dependencies:
//		StringAndData library (HSTRING support)
//
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

#include "HTransferStatus.h"

#include <stdio.h>
#include <string.h>



//---------------------------------------------------------------------------





//
//  Constructor for class HTRANSFER_STATUS.
//		Initialize transfer type that is used to report what type of transfer is going on.
//
HTRANSFER_STATUS::HTRANSFER_STATUS(void)
{
	TransferType = IDLE;

// Nothing going on yet that requires processing of asynchronously received data to be locked out.
	ActivityCounter = 0;

	DeviceName = new HSTRING( 20, true );

    return;
}
//---------------------------------------------------------------------------;





//
//  Destructor for class HTRANSFER_STATUS.
//		Currently, nothing to do.
//
HTRANSFER_STATUS::~HTRANSFER_STATUS(void)
{
	delete DeviceName;

    return;
}
//---------------------------------------------------------------------------;





//
//  IncrementProcessReceivedDataLockSemaphore
//		Indicate that there is something else wanting to grab received data in place of ProcessReceivedData, then
//		tell the instantiator that the semaphore changed.
//
void HTRANSFER_STATUS::IncrementProcessReceivedDataLockSemaphore(void)
{
	++ActivityCounter;
	ReceivedDataLockSemaphoreWasChanged();

    return;
} // IncrementProcessReceivedDataLockSemaphore





//
//  DecrementProcessReceivedDataLockSemaphore
//		Indicate that there is one less process wanting to grab received data in place of ProcessReceivedData, then
//		tell the instantiator that the semaphore changed.
//
void HTRANSFER_STATUS::DecrementProcessReceivedDataLockSemaphore(void)
{
	--ActivityCounter;
	if( ActivityCounter < 0 )
	{
		ActivityCounter = 0;
	}
	ReceivedDataLockSemaphoreWasChanged();

    return;
} // DecrementProcessReceivedDataLockSemaphore





//
//  GetProcessReceivedDataLockSemaphore
//		Return the current value of ActivityCounter to the caller.
//
int HTRANSFER_STATUS::GetProcessReceivedDataLockSemaphore(void)
{
	return ActivityCounter;
} // GetProcessReceivedDataLockSemaphore





//
//  GetActivityCounter
//		Return the current value of ActivityCounter to the caller.
//
int HTRANSFER_STATUS::GetActivityCounter(void)
{
	return ActivityCounter;
} // GetActivityCounter




