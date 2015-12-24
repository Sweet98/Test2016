//======================================================================================
// db_HTransferStatus.c
//======================================================================================
// $RCSfile: Matrix/Matrixhtransferstatus.cpp $
// $Revision: 1.4 $
// $Date: 2009/10/01 14:29:59EDT $
//======================================================================================

//======================================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "beep.h"
#include "MatrixHTransferStatus.h"
#include "IOMan.h"
#include "HTagDataResult.h"

MatrixHTransferStatus * TransferStatusMonitor=NULL;
#ifndef RFBASE
#else
#include "BaseUpdateScanners.h"
#endif


//
// In order to use HTRANSFER_STATUS, several members of the HTRANSFER_STATUS class must be overridden.  The HTransferStatus class accomplishes this.
//
//
//	Constructor
//		Nothing to do.
//
MatrixHTransferStatus:: MatrixHTransferStatus(void)
		: HTRANSFER_STATUS()
{
	return;
} // NonMenuHModem constructor



//
//	Destructor
//		Nothing to do.
//
MatrixHTransferStatus::~MatrixHTransferStatus(void)
{
	return;
} //  MatrixHTransferStatus destructor



//
//	PrepareDownloadStatus
//		Process the HTransferStatus PrepareDownloadStatus event by terminating scans for code storage, flash image storage, and image receive.
//
void MatrixHTransferStatus::PrepareDownloadStatus(void)
{
	switch(TransferType)
	{
	case RETRIEVE_FLASH_IMAGE:
		FlashImageDownloadBeep();

	case STORE_APPLICATION_CODE:
	case STORE_BOOT_CODE:
	case PROCESS_IMAGE:
	case STORE_FLASH_IMAGE:
	case PROCESS_JPEG_IMAGE:
		break;
	default:
		break;
	}
	return;
} // PrepareDownloadStatus




//
//	CloseDownloadStatus
//		Process the HTransferStatus CloseDownloadStatus event by restarting scans, if shut off by PrepareDownloadStatus event, for code storage,
//		flash image storage, and image receive.
//
void MatrixHTransferStatus::CloseDownloadStatus(void)
{
	switch(TransferType)
	{
	case RETRIEVE_FLASH_IMAGE:
		//ResetBeeper();

	case STORE_APPLICATION_CODE:
	case STORE_BOOT_CODE:
	case PROCESS_IMAGE:
	case STORE_FLASH_IMAGE:
	case PROCESS_JPEG_IMAGE:
		break;
	default:
		break;
	}
	return;
} // CloseDownloadStatus





//
//
//		Process the HTransferStatus MayContinue event on RF bases by sending a progress message to the host.
//
#if defined(RFBASE) || defined(RFSCAN)
#else
bool MatrixHTransferStatus::MayContinue( unsigned int /* NumberOfBytesSentSinceLastUpdate */ , unsigned int /* TotalBytesToBeSent */,
        bool /* CheckForTerminateConditionOnly */ )
{
	return true;
} // MayContinue
#endif







