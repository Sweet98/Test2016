//======================================================================================
// db_HComm.h
//======================================================================================
// $RCSfile: Matrix/MatrixHComm.h $
// $Revision: 1.2 $
// $Date: 2009/04/05 12:17:01EDT $
//======================================================================================

//======================================================================================




#ifndef	_MATRIX_HCOMM_H
#define	_MATRIX_HCOMM_H

#include "language.h"
#include "HComm.h"
#include "timeout.h"

//
// In order to use HCOMM, several members of the HCOMM class must be overridden.  The MatrixHComm class accomplishes this.
//

class MatrixHComm : public HCOMM
{
public:
	MatrixHComm(void);
	~MatrixHComm(void);
	virtual void ZeroElapsedTime(void);
	virtual UINT ElapsedTime(void);
	virtual bool FlushReadBuffer(void);
	virtual bool FlushWriteBuffer(void);
	virtual bool Read(HSTRING *Input, UINT MaximumReadSize, UINT Timeout);
	virtual bool Write( HSTRING *Output, int StartPosition, int LengthToWrite, bool WaitForWriteCompletion );
	virtual UINT BytesInWriteBuffer(void);
	virtual void HCommSleep(UINT SleepTime);
	virtual bool IsPortAssigned(void);
	virtual bool Read( char *Input, unsigned int *SizeOfRead, unsigned int MaximumReadSize, unsigned int Timeout );
	virtual bool FlushReadBuffer( int /* NumberOfMillisecondsToWait */ );
	virtual bool Write(HSTRING *Output)
	{
		return Write( Output, 0, Output->Size, false );
	}
	virtual bool Write( char *Output, int SizeOfOutput );
	virtual bool Write( int Character )
	{
		char c=Character;
		return Write( &c, 1 );
	}
private: CTimeoutmS ElapsedTimer;
};
extern MatrixHComm *ComDevice;

#endif



