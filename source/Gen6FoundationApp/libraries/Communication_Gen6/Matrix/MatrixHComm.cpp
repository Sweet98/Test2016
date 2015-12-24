//======================================================================================
// db_HComm.c
//======================================================================================
// $RCSfile: Matrix/MatrixHComm.cpp $
// $Revision: 1.5 $
// $Date: 2010/11/29 03:46:58EST $
//======================================================================================

//======================================================================================





#include <stdlib.h>
#include <string.h>
#include "MatrixHComm.h"
#include "db_chrio.h"
#include "timeout.h"
#include "ioman.h"
#include "thread.h"
#include "Menu.h"
#include "ReaderConfiguration.h"






//
// In order to use HComm, several members of the HComm class must be overridden.  The MatrixHComm class accomplishes this.
//



//
//	Constructor
//		Nothing to do.
//
MatrixHComm::MatrixHComm(void)
		: HCOMM()
{

	DefaultCharacterReadTimeoutForHTAGNoProtocol = 3000;
	DefaultCharacterReadTimeoutForHTAGNoProtocolFlashImageTransfers = 10000;

	return;
} // MatrixHComm constructor



//
//	Destructor
//		Nothing to do.
//
MatrixHComm::~MatrixHComm(void)
{
	return;
} // MatrixHComm destructor



//
//  HCOMM override to zero the elapsed timer.
//
void MatrixHComm::ZeroElapsedTime(void)
{
	ElapsedTimer.SetExpiration(0); // set up new srart time so elapsed time can use
	return;
}





//
//  HCOMM override to get the value of the elapsed timer.
//
UINT MatrixHComm::ElapsedTime(void)
{
	return ElapsedTimer.GetElapsed()/1000;
}





//
//  HCOMM override to flush all characters from the serial port read buffer.
//
bool MatrixHComm::FlushReadBuffer(void)
{
	while(kbhit())
	{
		get_char();
	}
	return(true);
}

bool MatrixHComm::FlushReadBuffer( int /* NumberOfMillisecondsToWait */ )
{
	return FlushReadBuffer();
}





//
//  HCOMM override to flush all characters from the serial port write buffer.
//
bool MatrixHComm::FlushWriteBuffer(void)
{
	theCurrentInterface.TxFlush();
	return(true);
}




//
//  HCOMM override to read data from the serial port up to MaximumReadSize characters, waiting no longer
//  than Timeout milliseconds for the read.
//
bool MatrixHComm::Read(HSTRING *Input, UINT MaximumReadSize, UINT Timeout)
{

	Input->Size = 0L;
	if( Input->MaximumLength() < (int) MaximumReadSize )
	{
		Input->Reallocate( MaximumReadSize );
	}
	CTimeoutmS SerialPortTimer(Timeout);
	CTimeoutmS SerialPortIntercharacterReceiveTimer(IntercharacterReceiveTimeoutForThisInterface());

	if (IsInterfaceRunning())		// use iomanager i/o
	{
		CIoMan * Iface = g_pCurrentInterface;
		int	AmountOfDataRemainingToBeRead = MaximumReadSize;
		int	AmountOfDataActuallyRead;
		while ((Input->Size < MaximumReadSize) && ((Timeout==0) || !SerialPortTimer.HasExpired()) && ((Input->Size==0) || !SerialPortIntercharacterReceiveTimer.HasExpired()))
		{
			RESET_WATCHDOG_TIMER();

			if (Iface->IsAnyRxData()>0)
			{
				AmountOfDataActuallyRead = Iface->Read( &(Input->Char[ Input->Size ]), AmountOfDataRemainingToBeRead);
				if (AmountOfDataActuallyRead>0)
				{
					SerialPortIntercharacterReceiveTimer.Reset();
					Input->Size += AmountOfDataActuallyRead;
					AmountOfDataRemainingToBeRead -= AmountOfDataActuallyRead;
				}
			}
		}
		return Input->Size>0;

	}
	else
	{
		if (!DoesInterfaceDoSimpleIO()) // else use low level i/o
		{
			return false;
		}
		else
		{
			while ((Input->Size < MaximumReadSize) && ((Timeout==0) || !SerialPortTimer.HasExpired()) && ((Input->Size==0) || !SerialPortIntercharacterReceiveTimer.HasExpired()))
			{
				RESET_WATCHDOG_TIMER();
				poll_getchar_from_rx_fifo();
				if(kbhit())
				{
					SerialPortIntercharacterReceiveTimer.Reset();
					Input->AppendNumber(((UCHAR) get_char()), 1);
				}
			}
			return Input->Size>0;
		}
	}
}

bool MatrixHComm::Read( char *Input, unsigned int *SizeOfRead, unsigned int MaximumReadSize, unsigned int Timeout )
{
	return 	Read(Input,SizeOfRead,MaximumReadSize, Timeout );
}

//
//  HCOMM override to write data to the serial port.
//
bool MatrixHComm::Write( HSTRING *Output, int StartPosition, int LengthToWrite, bool /* WaitForWriteCompletion */ )
{
	if (StartPosition+LengthToWrite>Output->Size)
	{
		return false;
	}
	else
	{
		return Write( (char *)&(Output->Char[StartPosition]),LengthToWrite);
	}
}

bool MatrixHComm::Write( char *Output, int SizeOfOutput )
{
	int		OffsetOfNextWrite = 0;
	int		NumberOfBytesRemainingToBeWritten = SizeOfOutput;
	int		NumberOfBytesToWriteThisCycle;
	int		NumberOfBytesToWriteInOneCycle=30 * 1024;
	CIoMan	*IFace;

	if ( !IsInterfaceRunning()&& DoesInterfaceDoSimpleIO())
	{
		while( NumberOfBytesRemainingToBeWritten )
		{
			NumberOfBytesToWriteThisCycle=( NumberOfBytesRemainingToBeWritten > NumberOfBytesToWriteInOneCycle )?NumberOfBytesToWriteInOneCycle:NumberOfBytesRemainingToBeWritten;
			NumberOfBytesRemainingToBeWritten -= NumberOfBytesToWriteThisCycle;
			xputstring(&(Output[ OffsetOfNextWrite ]), NumberOfBytesToWriteThisCycle );
			OffsetOfNextWrite += NumberOfBytesToWriteThisCycle;
		}
	}
	else
	{
		// To improve transfer speed of USB High Speed
		if (GET_SETTING(UsbHighspEnable) && (GET_SETTING(TerminalID) == 130))
		{
			NumberOfBytesToWriteInOneCycle=540 * 1024;
		}

		IFace= g_pCurrentInterface;		// this prvents changing ports if some other thread changes g_pCurrentInterface
		while( NumberOfBytesRemainingToBeWritten )
		{
			NumberOfBytesToWriteThisCycle=( NumberOfBytesRemainingToBeWritten > NumberOfBytesToWriteInOneCycle )?NumberOfBytesToWriteInOneCycle:NumberOfBytesRemainingToBeWritten;
			if (IFace->IsPortAvailable())
			{
				// Idealy write should return the number of bytes written cordless interface does but others do not
				// a change to driverbase can be made to fix this however until then code checks only for >0
				//NumberOfBytesToWriteThisCycle=IFace->Write(((char *) &(Output->Char[ OffsetOfNextWrite ])), NumberOfBytesToWriteThisCycle );
				size_t ret =IFace->Write(&(Output[ OffsetOfNextWrite ]), NumberOfBytesToWriteThisCycle );
				if (ret>0)
					//if (NumberOfBytesToWriteThisCycle>0) // see above
				{
					NumberOfBytesRemainingToBeWritten -= NumberOfBytesToWriteThisCycle;
					OffsetOfNextWrite += NumberOfBytesToWriteThisCycle;
				}
				else
					return false;
			}
			else
				return false;
		}
	}
	return(true);
}




//
//  HCOMM override to find the number of bytes remaining in the serial write buffer.
//
UINT MatrixHComm::BytesInWriteBuffer(void)
{
	return (0);
}



//
//  HCOMM override to sleep for SleepTime milliseconds.
//
void MatrixHComm::HCommSleep(UINT SleepTime)
{
	MT_Delay(SleepTime);
	return;
}



//
//  HCOMM override to indicate that a communication port is assigned (always true for a reader device).
//
bool MatrixHComm::IsPortAssigned(void)
{
	return(true);
}

MatrixHComm *ComDevice=NULL;

