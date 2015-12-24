//======================================================================================
// db_HTransferStatus.h
//======================================================================================
// $RCSfile: Matrix/Matrixhtransferstatus.h $
// $Revision: 1.1 $
// $Date: 2009/03/08 15:58:06EDT $
//======================================================================================

//======================================================================================




#ifndef	_MATRIX_HTRANSFER_STATUS_H
#define	_MATRIX_HTRANSFER_STATUS_H

#include	<stdlib.h>
#include	<string.h>
#include	"language.h"
#include	"HTransferStatus.h"
//
// In order to use HTRANSFER_STATUS, several members of the HTRANSFER_STATUS class must be overridden.  The HTransferStatus class accomplishes this.
//

class MatrixHTransferStatus : public HTRANSFER_STATUS
{
private:

public:
	UINT	TotalBytesTransmitted;

protected:
	// Process the HTransferStatus PrepareDownloadStatus event by terminating scans for code storage, flash image storage, and image receive.
	virtual void PrepareDownloadStatus(void);

	virtual void PrepareDownloadStatus( HWSTRING * /* DeviceName */ )
	{
		PrepareDownloadStatus();
	}

	// Process the HTransferStatus CloseDownloadStatus event by restarting scans, if shut off by PrepareDownloadStatus event, for code storage,
	// flash image storage, and image receive.
	virtual void CloseDownloadStatus(void);

	// Process the HTransferStatus MayContinue event on RF bases by sending a progress message to the host.
	virtual bool MayContinue(unsigned int NumberOfBytesSentSinceLastUpdate, unsigned int TotalBytesToBeSent, bool CheckForTerminateConditionOnly );
	virtual bool MayContinue(unsigned int NumberOfBytesSentSinceLastUpdate, unsigned int TotalBytesToBeSent )
	{
		return( MayContinue( NumberOfBytesSentSinceLastUpdate, TotalBytesToBeSent, false ));
	}

public:
	MatrixHTransferStatus(void);
	~MatrixHTransferStatus(void);
};

extern MatrixHTransferStatus * TransferStatusMonitor;

#endif



