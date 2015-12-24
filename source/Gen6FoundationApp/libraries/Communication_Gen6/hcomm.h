//======================================================================================
// HComm.h
//======================================================================================
// $RCSfile: hcomm.h $
// $Revision: 1.1 $
// $Date: 2009/03/08 16:03:36EDT $
//======================================================================================

//======================================================================================






//---------------------------------------------------------------------------
//
//  Class definitions for HCOMM
//
//---------------------------------------------------------------------------
#ifndef HCOMMH
#define HCOMMH
//---------------------------------------------------------------------------

#include "HString.h"

#ifndef HCOMM_API	                        // Only if it has not been defined yet
#ifdef WIN32
//#if (WIN32 || _WIN32_WCE)
#ifdef WINAPP
	    #define HCOMM_API
	#else
		#if (_USRDLL || DLL)
		  #define HCOMM_API __declspec(dllexport)
		#else
		  #define HCOMM_API __declspec(dllimport)
		#endif
   #endif
#else
	#ifdef _WIN32_WCE
	    #if (_USRDLL || DLL)
	        #define HCOMM_API __declspec(dllexport)
		#else
			#define HCOMM_API __declspec(dllimport)
		#endif
	#else
	    #define HCOMM_API
	#endif
#endif
#endif	                                 // ifndef HCOMM_API

//---------------------------------------------------------------------------
class HCOMM_API HCOMM
{
public:
#define	DEFAULT_CHARACTER_READ_TIMEOUT	1000	// 1000 ms maximum time between received characters.

	int		DefaultCharacterReadTimeout;
	int		DefaultCharacterReadTimeoutForHTAGNoProtocol;
	int		DefaultCharacterReadTimeoutForHTAGNoProtocolFlashImageTransfers;

public:
#define	HTAG_DEFAULT_WAIT_TIME_FOR_COMMAND			2000

	int		WaitTimeForHTAGCommand;		// Time, in milliseconds, to wait for an HTAG command.  Zero indicates that we're to
	// wait indefinitely.

public:
#define	DEFAULT_WAIT_TIME_FOR_SCAN_ENGINE_FIRMWARE_DOWNLOAD			60000

	int		WaitTimeForScanEngineFirmwareDownload;	// Time, in milliseconds, to wait for scan engine firmware download to
	// complete.

public:
#define	HTAG_DEFAULT_MAXIMUM_COMMAND_SIZE		4096*1024

	int		MaximumHTAGCommandSize;						// Maximum length of a command.




	// The following functions are required in order to support HCOMM.  It is the responsibility of the
	// instantiator to override these, which requires that they use this class as a base class.

	// Instantiator may support an elapsed timer, whose granularity is in seconds.  ZeroElapsedTime resets the counter
	// and ElapsedTime returns the amount of time elapsed, in seconds, since the last call to ZeroElapsedTime.  These
	// allow a user of HCOMM to time I/O operations.  These are required by some of the I/O classes in the communication
	// library.
	virtual void ZeroElapsedTime(void)
	{
		return;
	}
	virtual unsigned int ElapsedTime(void)
	{
		return(0x7FFFFFFF);
	}


	// These functions are required by users of the HCOMM class in order to do minimal I/O to a device.  FlushReadBuffer
	// should clear a device read queue and, if applicable (and possible), hardware and driver queues as well.  Protocol
	// transfers use this function to get rid of pending input when an exception occurs.  Read is used to read data from
	// the communication port.  Up to MaximumReadSize bytes are read; Read must return when either MaximumReadSize is
	// reached or Timeout milliseconds has elapsed.  Write simply sends data to the output port.  All functions return true
	// if a device is connected, and false otherwise.  IsPortAssigned returns true if a communication port is assigned, and
	// false otherwise.
	virtual bool FlushReadBuffer( int /* NumberOfMillisecondsToWait */ )
	{
		return(false);
	}
	virtual bool FlushReadBuffer(void)
	{
		return FlushReadBuffer( 0 );
	}
	virtual bool Read(HSTRING * /* Input */, unsigned int /* MaximumReadSize */, unsigned int /* Timeout */)
	{
		return(false);
	}
	virtual bool Read( char *Input, unsigned int *SizeOfRead, unsigned int MaximumReadSize, unsigned int Timeout )
	{
		HSTRING		strInput( (unsigned char *) Input, MaximumReadSize, false, false, true );
		strInput.Size = 0;
		bool		Status;
		Status = Read( &strInput, MaximumReadSize, Timeout );
		*SizeOfRead = strInput.Size;
		return Status;
	}

	virtual bool Write(HSTRING * /* Output */, int /* StartPosition */, int /* LengthToWrite */,
	                   bool /* WaitForWriteCompletion */ )
	{
		return(false);
	}
	virtual bool Write(HSTRING *Output)
	{
		return Write( Output, 0, Output->Size, false );
	}
	virtual bool Write( char *Output, int SizeOfOutput )
	{
		HSTRING	hstrOutput( (unsigned char *) Output, SizeOfOutput, false, true, true );
		return Write( &hstrOutput );
	}
	virtual bool Write( int Character )
	{
		char	cOutput[ 2 ];
		cOutput[ 0 ] = (char) Character;
		cOutput[ 1 ] = 0;
		return Write( cOutput, 1 );
	}
	virtual bool IsPortAssigned(void)
	{
		return(false);
	}

	// These functions are optional.  FlushWriteBuffer should clear a device write queue and, if applicable and possible,
	// the hardware and driver queues as well.  Protocol transfers use this function to flush output queues if an error
	// occurs, so that normal message traffic can resume as quickly as possible.  If this function is not implemented, error
	// recovery may be significantly delayed.  FlushWriteBuffer returns true if a device is connected, and false otherwise.
	// BytesInWriteBuffer is not currently used, but may be used in the future during protocol transfers.  HCommSleep is used
	// after error recovery and transfer completion to allow a transmitter or receiver to recover.  SleepTime is expressed in
	// milliseconds.  This routine is not required, but is strongly recommended.
	virtual bool FlushWriteBuffer(void)
	{
		return(false);
	}
	virtual unsigned int BytesInWriteBuffer(void)
	{
		return(0);
	}
	virtual void HCommSleep(unsigned int /* SleepTime */)
	{
		return;
	}

	// These functions are optional.  SwitchTo8BitTransferMode should cause communication characteristics to be changed, if
	// possible, to allow 8 bit data transfers, which is required for image and code transmission.  RevertFrom8BitTransferMode
	// will cause the port configuration to return to what it was.
	virtual bool SwitchTo8BitTransferMode(void)
	{
		return(true);
	}
	virtual bool RevertFrom8BitTransferMode(void)
	{
		return(true);
	}

	// These functions are optional.  The baud rate is expressed as the actual value (i.e. 115200, etc.).  At this point, these
	// functions are only used in Matrix boot mode when the baud rate must be 115200.
	virtual bool		ChangeBaudRate( int /* BaudRate */ )
	{
		return false;
	}
	virtual int			BaudRate( void )
	{
		return 0;
	}

	// Timeout setting routines.

public:		// User declarations

	// Contstructor for HCOMM.
	//  Prepare for an HCOMM I/O.
	HCOMM(void);

	// Destructor for HCOMM.
	~HCOMM(void);
};
//---------------------------------------------------------------------------
#endif





