//======================================================================================
// DiagnosticAndDebugMenuSettings.cpp
//======================================================================================
// $RCSfile: DiagnosticAndDebugMenuSettings.cpp $
// $Revision: 1.47 $
// $Date: 2011/11/18 17:14:11EST $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Module Description:
//      This file contains a routine that creates diagnostic and debug menu
//		settings as part of the overall reader configuration for a Matrix device.
//
//---------------------------------------------------------------------------

#include <stdio.h>
#include <rt_fp.h>
#include "ReaderConfiguration.h"
#include "ProductConfiguration.h"
#include "stdInclude.h"
#include "Checksum.h"
#include "CRC.h"
#include "db_chrio.h"
#include "db_hdw.h"
#include "db_heap.h"
#include "db_sharedtable.h"
#include "ioman.h"
#include "menu.h"
#include "Thread.h"
#include "Timeout.h"
#include "DiagnosticAndDebugMenuSettings.h"			// DiagnosticAndDebugMenuSettings interface definition
#include "beep.h"
#include "barcodefifo.h"
#include "barcodedataman.h"
#include "barcodedata.h"
#include "StartupFlashDisk.h"
#include "MenuFlash.h"
//#include "trigger.h"
#include "usage_statistics.h"
#include "MatrixHTransferStatus.h"
#include "MatrixSynFECommandHandler.h"
#include "HardwareToken.h"

#ifdef BATCH_DEBUG
#include "mainhhplproc.h"
#endif
extern int SpiReadDbl(unsigned int address,unsigned int numbytes,char *buffer);

#ifndef RFBASE
#include "scandrivermatrixinterface.h"
#include "ScanDriverHWLayer.h"
#include "HWTrigger.h"

#define	TRIGGER_5_5		1
#ifdef TRIGGER_5_5
#include "trigger_5_5.h"                                         //lmc   fixme
#endif

#include "ScanDriverImagingInterface.h"
#ifdef HI2D_T
#ifdef BYD_SENSOR
	extern BOOL ReadIIC_BYD( unsigned char subAddress, unsigned short * uiBuffer, unsigned int nCount );	
#else	//else #ifdef BYD_SENSOR
	extern BOOL ReadIIC_Hi704A( unsigned char subAddress, unsigned short * uiBuffer, unsigned int nCount );
#endif	//end #ifdef BYD_SENSOR
#else	//else #ifdef HI2D_T
	extern BOOL ReadIIC_Jade( unsigned char subAddress, unsigned short * uiBuffer, unsigned int nCount );
#endif	//else #ifdef HI2D_T
extern void SkipIndicatorsForThisMenuCommand( void );       //found in "decodeiface.cpp"
extern int invalidfranes;                   //jaw  debug  remove
static int invalidfranes = 0;

bool ShowInvalidFrames(void)
{
	theStdInterface.printf("Invalid Frames: %d\r\n",invalidfranes);
	invalidfranes=0;
	return true;
}
#endif // endof #ifndef RFBASE

#ifdef DEVELOPMENT_SETTINGS
int test =0;
bool Divedezederror (void)
{
	int j= 100/(test);
	return j>0;
}
#endif

extern  void print_thread_info(void);

bool Doprint_thread_info(void);

bool Doprint_thread_info(void)
{
	print_thread_info();
	return true;
}



bool DoBarcodePoolReset(void);

bool DoBarcodePoolReset(void)
{


	return true;
}

extern bool ReportStartupTiming( void );


// Used by memory dump routines to keep track of the last displayed address.
static char	*DisplayMemoryBaseAddress = 0;




bool MallocTest(void) {
	int numUsed = 0;
	int malSize = ReaderConfiguration->ExtraneousTextAfterCommand->ConvertToNumber(0,ReaderConfiguration->ExtraneousTextAfterCommand->Size-1,&numUsed);
	if( numUsed ) {
		theStdInterface.printf("Mallocing %d bytes...",malSize);
		int *testMal = (int *)malloc(malSize);
		if( testMal ) {
			theStdInterface.printf("success!\r\n");
		}
		else {
			theStdInterface.printf("failed.\r\n");
		}
		free( testMal );
		return true;
	}
	else {
		theStdInterface.printf("\r\nInvalid malloc size. Please input number of bytes to malloc.\r\n");
	}
	return false;
}

#define TO_READ_MS	500
#define DATA_LENGTH	50000  // MUST be less than or equal to code in PC!  Note:  the COM Em driver limits us to 50K
static BYTE * g_pData = NULL;

/*----------------------------------------------------------------------------------------
EchoRead

  This is a helper function for DbgEcho().  It keeps reading data until on of the following
  conditions is met:

  	- all the requested data is read
  	- the specified timeout ocurrs since the last read() that returned data


  Parameters:
  				pData[out]:  pointer to buffer to hold the requested data bytes

  				NumToRead[in]:  number of data bytes to read

  				TimeoutMs[in]:  number of milliseconds to wait for more data


  Returns:  number of bytes read
----------------------------------------------------------------------------------------*/
int EchoRead(BYTE * pData, int NumToRead, int TimeoutMs)
{
	int NumRead = 0;
	int NumToReadThisTime;
	int NumAvail = 0;
	CTimeoutmS to(TimeoutMs);


	if ( TimeoutMs > 0 )
	{
		do
		{
			while ( (! theCurrentInterface.IsAnyRxData()) && (! to.HasExpired()) );
			NumAvail = theCurrentInterface.IsAnyRxData();
			if ( NumAvail )
			{
				if ( NumAvail >= NumToRead ) NumToReadThisTime = NumToRead;
				else NumToReadThisTime = NumAvail;
				NumRead += theCurrentInterface.Read(pData, NumToReadThisTime);
				pData += NumToReadThisTime;
				NumToRead -= NumToReadThisTime;
				to.Reset();
			}
		}
		while ( NumToRead && ! to.HasExpired() );
	}
	else
	{
		while ( (theCurrentInterface.IsAnyRxData() < NumToRead) ) RESET_WATCHDOG_TIMER();
		NumRead = theCurrentInterface.Read(pData, NumToRead);
	}
	return NumRead;
}

/*----------------------------------------------------------------------------------------
DbgEcho

  Echos back data coming in the main interface port.  The incoming data has the following
  format:

  | 4 byte length (Little Endian)|  data bytes   |

  Length is limited in size to DATA_LENGTH.  If the received length is greater than this,
  it is considered an error.

  Only the data bytes will be echoed (not the 4B length).  This echo command will
  terminate under the following circumstances:

  	- ANY error whatsoever (it will print a diagnostic message before exiting)
  	- if it receives a length of 0 (normal termination)


  Parameters:  none

  Returns:  true if it exited normally, false otherwise
----------------------------------------------------------------------------------------*/
bool DbgEcho(void)
{
	DWORD length;
	int NumRead;
	BYTE byte;
	bool bIsOk = true;


	EchoRead(&byte, 1, TO_READ_MS); // read the leftover '.'
	g_pData = new BYTE[DATA_LENGTH];
	if ( ! g_pData )
	{
		theCurrentInterface.printf("Mem Error\n");
		bIsOk = false;
		goto exit;
	}
	do
	{
		if ( (NumRead = EchoRead((BYTE *) &length, sizeof length, -1)) != sizeof length ) // read 4 byte length
		{
			theCurrentInterface.printf("Error1--read (%d)", NumRead);
			bIsOk = false;
			goto exit;
		}
		if ( length > DATA_LENGTH )
		{
			theCurrentInterface.printf("Error2--bad length (%d) (%#x)", length, length);
			bIsOk = false;
			goto exit;
		}
		else if ( length )
		{	// Read data
			if ( (NumRead = EchoRead(g_pData, length, TO_READ_MS)) != length )
			{
				theCurrentInterface.printf("Error3--read data--got %d bytes", NumRead);
				bIsOk = false;
				goto exit;
			}
			// echo data only (no length)
			if ( ! theCurrentInterface.Write(g_pData, length) )
			{
				theCurrentInterface.printf("Error4--write data");
				bIsOk = false;
				goto exit;
			}
		}
	}
	while ( length );
exit:
	delete [] g_pData;
	return bIsOk;
}



//
//	SetFlashDebugMode
//		Call SetFlashDebugState in the flash processor with the current DBGFLS value.
//
bool SetFlashDebugMode( void )
{
	SetFlashDebugState( GET_SETTING( FlashDebugOn ) );

	return true;
} // SetFlashDebugMode



/*-------------------------------------------------------------------------
 * StuffRxBuffer
 *-------------------------------------------------------------------------*/
bool StuffRxBuffFromTheMenu(void)
{
	if(ReaderConfiguration->ExtraneousTextAfterCommand->Char[0]==0x16 && ReaderConfiguration->ExtraneousTextAfterCommand->Char[1]==0x4E)
		ReaderConfiguration->ExtraneousTextAfterCommand->Char[2]=0x2C;//for [syn] N, replace next char with ","
	theStdInterface.StuffRxBuff((const bchar_t *)ReaderConfiguration->ExtraneousTextAfterCommand->Char, (size_t) ReaderConfiguration->ExtraneousTextAfterCommand->Size);
	xputstring(ReaderConfiguration->ExtraneousTextAfterCommand->Char,ReaderConfiguration->ExtraneousTextAfterCommand->Size);
	return true;
}


// Flash disk routines.

//
//	FlashDiskDirectory
//		Output a flash disk directory listing to the host.
//
bool FlashDiskDirectory(void)
{
	ShowDirectory((const char *)ReaderConfiguration->ExtraneousTextAfterCommand->Char);
	return true;
} // FlashDiskDirectory



//
//	FlashDiskDeleteFile
//		Delete a flash disk file.
//
bool FlashDiskDeleteFile(void)
{

	HSTRING	FileName( ReaderConfiguration->ExtraneousTextAfterCommand, true );
	FileName.UnprintableFormatsToCharacters();
	return  EraseFile((const char *)FileName.Char)==0;
} // FlashDiskDeleteFile



//
//	FlashDiskReadFile
//		Read a flash disk file.
//
bool FlashDiskReadFile(void)
{
	int size=-1;
	unsigned char * pContents =NULL;
	HSTRING	FileName( ReaderConfiguration->ExtraneousTextAfterCommand, true );
	FileName.UnprintableFormatsToCharacters();

	size = ReadAndCloseFile((char *) FileName.Char,0,&pContents); // this call allocates from heap need to free below
	for (int i=0;i<size;i+=80)
	{
		// Write out 80 characters per line.
		theStdInterface.Write(((char *) &pContents[i]),((size-i)>80)?80:(size-i));
	}
	if (pContents)
	{
		free(pContents);
	}
	return (size < 0) ? false : true;
} // FlashDiskReadFile





//
//	FlashDiskRenameFile
//		Rename a flash disk file.  The old name and new name should be separated by ":".
//
bool FlashDiskRenameFile(void)
{
	int		FileIndex = -1;				// Haven't succeeded yet.

	HSTRING	OldFileName( 20, true );
	HSTRING	NewFileName( 20, true );
	int		PositionWhereColonWasFound;


	if( ReaderConfiguration->ExtraneousTextAfterCommand->Find( (unsigned char *) ":", 1, false, 1, &PositionWhereColonWasFound ) )
	{
		OldFileName.Copy( ReaderConfiguration->ExtraneousTextAfterCommand, 0, PositionWhereColonWasFound - 1 );
		NewFileName.Copy( ReaderConfiguration->ExtraneousTextAfterCommand, PositionWhereColonWasFound + 1,
		                  ReaderConfiguration->ExtraneousTextAfterCommand->Size - 1 );
#ifdef FIXME

		FileIndex = FlashDisk->RenameFile( (char *) OldFileName.Char, (char *) NewFileName.Char );
#else

		FileIndex= RenameFile((const char *)OldFileName.Char,(const char *)NewFileName.Char);
#endif

	}
	return (FileIndex < 0) ? false : true;
} // FlashDiskRenameFile



//
//	FlashDiskCopyFile
//		Copy a flash disk file.  The old name and new name should be separated by ":".
//
bool FlashDiskCopyFile(void)
{
	int		FileIndex = -1;				// Haven't succeeded yet.


	HSTRING	OldFileName( 20, true );
	HSTRING	NewFileName( 20, true );
	int		PositionWhereColonWasFound;

	if( ReaderConfiguration->ExtraneousTextAfterCommand->Find( (unsigned char *) ":", 1, false, 1, &PositionWhereColonWasFound ) )
	{
		OldFileName.Copy( ReaderConfiguration->ExtraneousTextAfterCommand, 0, PositionWhereColonWasFound - 1 );
		NewFileName.Copy( ReaderConfiguration->ExtraneousTextAfterCommand, PositionWhereColonWasFound + 1,
		                  ReaderConfiguration->ExtraneousTextAfterCommand->Size - 1 );
#ifdef FIXME
		// Get the file contents and information, then duplicate everything but the filename when creating the new file.
		HSTRING	FileContents(20, true);

		FileIndex = FlashDisk->Read( (char *) OldFileName.Char, &FileContents );

		// If we had no errors, get the file information and copy it.
		if(FileIndex >= 0)
		{
			FileIndex = FlashDisk->Write( (char *) NewFileName.Char, (char *) FileContents.Char, FileContents.Size,
			                              FlashDisk->MatrixFlashDiskEntries[ FileIndex ].Type, FlashDisk->MatrixFlashDiskEntries[ FileIndex ].Placement,
			                              0, FlashDisk->MatrixFlashDiskEntries[ FileIndex ].ReservedSize, false, false,
			                              FlashDisk->MatrixFlashDiskEntries[ FileIndex ].Modifier );
		}
#else
		FileIndex=CopyFile((const char *)NewFileName.Char,(const char *)OldFileName.Char);
#endif

	}

	return ( (FileIndex < 0) ? false : true );
} // FlashDiskCopyFile


bool FlashDiskFormatVolume(void)
{
	bool status =false;
	int vol=ReaderConfiguration->ExtraneousTextAfterCommand->Char[0]-'0';
	switch (vol)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		status=	(FormatVolume(vol)==0);
	}
	return status;
}
//
//	FlashDiskInformationDump
//		Output a flash disk directory listing to the host.
//
bool FlashDiskInformationDump(void)
{

	return true;
} // FlashDiskDirectory




//
//	ShowStackUsage
//		Show stack space usage.
//
bool ShowStackUsage( void )
{
	char	ResultString[ 160 ];
	int		i;



	UINT	SvcStackSize = (UINT) SVC_STACK - (UINT) ABT_STACK;
	UINT	SvcStackBase = (UINT) SVC_STACK - SvcStackSize;
	UINT	AbtStackSize = (UINT) ABT_STACK - (UINT) UNDEF_STACK;
	UINT	AbtStackBase = (UINT) ABT_STACK - AbtStackSize;
	UINT	UndStackSize = (UINT) UNDEF_STACK - (UINT) IRQ_STACK;
	UINT	UndStackBase = (UINT) UNDEF_STACK - UndStackSize;
	UINT	IrqStackSize = (UINT) IRQ_STACK - (UINT) FIQ_STACK;
	UINT	IrqStackBase = (UINT) IRQ_STACK - IrqStackSize;
	UINT	FiqStackSize = (UINT) FIQ_STACK - (UINT) SYS_STACK;
	UINT	FiqStackBase = (UINT) FIQ_STACK - FiqStackSize;
	UINT	UsrStackSize = (UINT) SYS_STACK - (UINT) HEAP_END;
	UINT	UsrStackBase = (UINT) SYS_STACK - UsrStackSize;
	UINT	SysStackSize = UsrStackSize;
	UINT	SysStackBase = UsrStackBase;

	char	*StackPtr;

	// Test the system stack.
	StackPtr = ((char *) SysStackBase);
	for(i = 0;    i < SysStackSize;    ++i)
	{
		if(StackPtr[i] != 0xFF)
			break;
	}
	sprintf(ResultString, "Unused Sys Stack (%08X) = %d bytes\r\n", SysStackBase, i);
	xputstring(ResultString);

	// Test the user stack.
	StackPtr = ((char *) UsrStackBase);
	for(i = 0;    i < UsrStackSize;    ++i)
	{
		if(StackPtr[i] != 0xFF)
			break;
	}
	sprintf(ResultString, "Unused Usr Stack (%08X) = %d bytes\r\n", UsrStackBase, i);
	xputstring(ResultString);

	// Test the FIQ stack.
	StackPtr = ((char *) FiqStackBase);
	for(i = 0;    i < FiqStackSize;    ++i)
	{
		if(StackPtr[i] != 0xFF)
			break;
	}
	sprintf(ResultString, "Unused Fiq Stack (%08X) = %d bytes\r\n", FiqStackBase, i);
	xputstring(ResultString);

	// Test the IRQ stack.
	StackPtr = ((char *) IrqStackBase);
	for(i = 0;    i < IrqStackSize;    ++i)
	{
		if(StackPtr[i] != 0xFF)
			break;
	}
	sprintf(ResultString, "Unused Irq Stack (%08X) = %d bytes\r\n", IrqStackBase, i);
	xputstring(ResultString);

	// Test the Und stack.
	StackPtr = ((char *) UndStackBase);
	for(i = 0;    i < UndStackSize;    ++i)
	{
		if(StackPtr[i] != 0xFF)
			break;
	}
	sprintf(ResultString, "Unused Und Stack (%08X) = %d bytes\r\n", UndStackBase, i);
	xputstring(ResultString);

	// Test the Abt stack.
	StackPtr = ((char *) AbtStackBase);
	for(i = 0;    i < AbtStackSize;    ++i)
	{
		if(StackPtr[i] != 0xFF)
			break;
	}
	sprintf(ResultString, "Unused Abt Stack (%08X) = %d bytes\r\n", AbtStackBase, i);
	xputstring(ResultString);

	// Test the Svc stack.
	StackPtr = ((char *) SvcStackBase);
	for(i = 0;    i < SvcStackSize;    ++i)
	{
		if(StackPtr[i] != 0xFF)
			break;
	}
	sprintf(ResultString, "Unused Svc Stack (%08X) = %d bytes\r\n", SvcStackBase, i);
	xputstring(ResultString);


	for (threadhandle_t hndl=1;hndl<MAXTHREADS;hndl++)
		if (IsThreadOpen(hndl))
		{
			sprintf(ResultString, "Thread %ld:Unused Driver Thread Stack = %d bytes\r\n",hndl,ReportDriverThreadUnusedStack(hndl));
			xputstring(ResultString);
		}

	return true;
} // ShowStackUsage





//
//	DumpMemoryConfiguration
//		Show memory configuration.
//
bool DumpMemoryConfiguration(void)
{
	char	ResultString[ 80 ];

	// Display application image segment information.
	sprintf(ResultString, "Read only base:                   %08X\r\n", APP_IMAGE_RO_BASE);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Read only end:                    %08X\r\n", APP_IMAGE_RO_END);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Read write base:                  %08X\r\n", APP_IMAGE_RW_BASE);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Read write end:                   %08X\r\n", APP_IMAGE_RW_END);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Zero init base:                   %08X\r\n", APP_IMAGE_ZI_BASE);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Zero init end:                    %08X\r\n", APP_IMAGE_ZI_END);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Size of shared table:             %08X\r\n", SIZE_OF_SHARED_TABLE);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Size of code to store:            %08X\r\n", SIZE_OF_OP_CODE_TO_STORE);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Compressed application location:  %08X\r\n", (int) COMPRESSED_APPLICATION_LOCATION);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Compressed application size:      %08X\r\n", COMPRESSED_APPLICATION_SIZE);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Application compressor location:  %08X\r\n", (int) APPLICATION_COMPRESSOR_LOCATION);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Compressor and application size:  %08X\r\n", COMPRESSOR_AND_COMPRESSED_APPLICATION_SIZE);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Flash disk location:              %08X\r\n", (int) FLASH_DISK_LOCATION);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Inverse flash disk location:      %08X\r\n", (int) INVERSE_FLASH_DISK_LOCATION);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Compressor size:                  %08X\r\n", COMPRESSOR_SIZE);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Compressed app header size:       %08X\r\n", COMPRESSED_APPLICATION_HEADER_SIZE);
	theStdInterface.Write(ResultString);

	theStdInterface.Write( "\r\n" );

	// Display memory map information.
	sprintf(ResultString, "Dynamic memory map flag:          %08X\r\n", DYNAMIC_MEMORY_MAP);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Inverse dynamic memory map flag:  %08X\r\n", INVERSE_DYNAMIC_MEMORY_MAP);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Cached memory base:               %08X\r\n", (int) CACHED_MEMORY_BASE);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Non-cached memory base:           %08X\r\n", (int) NON_CACHED_MEMORY_BASE);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Memory size:                      %08X\r\n", MEMORY_SIZE);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Cached flash base:                %08X\r\n", (int) CACHED_ROM_BASE);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Non-cached flash base:            %08X\r\n", (int) NON_CACHED_ROM_BASE);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Flash size:                       %08X\r\n", ROM_SIZE);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Heap start:                       %08X\r\n", (int) HEAP_START);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Heap end:                         %08X\r\n", (int) HEAP_END);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "MMU level 1 table location:       %08X\r\n", (int) MMU_LEVEL_1_TABLE);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "MMU level 1 table size:           %08X\r\n", MMU_LEVEL_1_TABLE_SIZE);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "MMU level 2 table location:       %08X\r\n", (int) MMU_LEVEL_2_TABLE);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "MMU level 2 table size:           %08X\r\n", MMU_LEVEL_2_TABLE_SIZE);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "MMU disable buffer location:      %08X\r\n", (int) MMU_DISABLE_BUFFER);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "MMU disable buffer size:          %08X\r\n", MMU_DISABLE_BUFFER_SIZE);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "SVC stack location:               %08X\r\n", (int) SVC_STACK);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "ABT stack location:               %08X\r\n", (int) ABT_STACK);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "UNDEF stack location:             %08X\r\n", (int) UNDEF_STACK);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "IRQ stack location:               %08X\r\n", (int) IRQ_STACK);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "FIQ stack location:               %08X\r\n", (int) FIQ_STACK);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "SYS stack location:               %08X\r\n", (int) SYS_STACK);
	theStdInterface.Write(ResultString);

	sprintf(ResultString, "Decompressor heap start:          %08X\r\n", (int) DCMP_HEAP_START);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Decompressor heap end:            %08X\r\n", (int) DCMP_HEAP_END);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Decompressor read only base:      %08X\r\n", DCMP_APP_IMAGE_RO_BASE);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Decompressor read only end:       %08X\r\n", DCMP_APP_IMAGE_RO_END);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Decompressor read write base:     %08X\r\n", DCMP_APP_IMAGE_RW_BASE);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Decompressor read write end:      %08X\r\n", DCMP_APP_IMAGE_RW_END);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Decompressor zero init base:      %08X\r\n", DCMP_APP_IMAGE_ZI_BASE);
	theStdInterface.Write(ResultString);
	sprintf(ResultString, "Decompressor zero init end:       %08X\r\n", DCMP_APP_IMAGE_ZI_END);
	theStdInterface.Write(ResultString);

	return true;
} // DumpMemoryConfiguration



//
//	DumpMemoryBlock
//		Dump a memory block.
//
bool DumpMemoryBlock( void )
{
	bool	Status = true;		// No problems encountered yet.


	// If there is an address included, go get it, else we'll use the last address specified.
	if( ReaderConfiguration->ExtraneousTextAfterCommand->Size > 0 )
	{
		int		NumberOfCharactersUsedInConversion;
		char	*NewAddress = (char *) ReaderConfiguration->ExtraneousTextAfterCommand->ConvertHexToNumber
		                   ( 0, ReaderConfiguration->ExtraneousTextAfterCommand->Size - 1, &NumberOfCharactersUsedInConversion );
		if( NumberOfCharactersUsedInConversion == (int) ReaderConfiguration->ExtraneousTextAfterCommand->Size )
		{
			DisplayMemoryBaseAddress = NewAddress;
		}
		else
		{
			// There was extraneous text included with the number; WOOF!
			Status = false;
		}
	}


	// If no errors occurred, dump the block.
	if( Status )
	{
		char	ResultString[ 160 ];
		int		i;

		if ((((int) DisplayMemoryBaseAddress) % 16) != 0)
		{
			sprintf (ResultString,"\r\n%08X -", ((int) DisplayMemoryBaseAddress)) ;
			theStdInterface.Write(ResultString);
		}

		for (i = 0;    i < 256 ; i++)
		{
			if ((((int) DisplayMemoryBaseAddress) % 16) == 0)
			{
				sprintf (ResultString,"\r\n%08X -", ((int) DisplayMemoryBaseAddress)) ;
				theStdInterface.Write(ResultString);
			}

			sprintf (ResultString," %02X", ((int) *DisplayMemoryBaseAddress)) ;
			theStdInterface.Write (ResultString);
			++DisplayMemoryBaseAddress;
		}

		theStdInterface.Write("\r\n") ;
	}

	return Status;
} // DumpMemoryBlock



//
//	DumpOneByte
//		Dump a byte.
//
bool DumpOneByte( void )
{
	bool	Status = true;		// No problems encountered yet.


	// If there is an address included, go get it, else we'll use the last address specified.
	if( ReaderConfiguration->ExtraneousTextAfterCommand->Size > 0 )
	{
		int		NumberOfCharactersUsedInConversion;
		char	*NewAddress = (char *) ReaderConfiguration->ExtraneousTextAfterCommand->ConvertHexToNumber
		                   ( 0, ReaderConfiguration->ExtraneousTextAfterCommand->Size - 1, &NumberOfCharactersUsedInConversion );
		if( NumberOfCharactersUsedInConversion == (int) ReaderConfiguration->ExtraneousTextAfterCommand->Size )
		{
			DisplayMemoryBaseAddress = NewAddress;
		}
		else
		{
			// There was extraneous text included with the number; WOOF!
			Status = false;
		}
	}


	// If no errors occurred, dump.
	if( Status )
	{
		char	ResultString[ 160 ];

		sprintf (ResultString,"\r\n%08X - %02X\r\n", ((int) DisplayMemoryBaseAddress), ((int) *DisplayMemoryBaseAddress) ) ;
		theStdInterface.Write(ResultString);
	}

	++DisplayMemoryBaseAddress;

	return Status;
} // DumpOneByte





//
//	DumpTwoBytes
//		Dump two bytes.
//
bool DumpTwoBytes( void )
{
	bool	Status = true;		// No problems encountered yet.


	// If there is an address included, go get it, else we'll use the last address specified.
	if( ReaderConfiguration->ExtraneousTextAfterCommand->Size > 0 )
	{
		int		NumberOfCharactersUsedInConversion;
		char	*NewAddress = (char *) ReaderConfiguration->ExtraneousTextAfterCommand->ConvertHexToNumber
		                   ( 0, ReaderConfiguration->ExtraneousTextAfterCommand->Size - 1, &NumberOfCharactersUsedInConversion );
		if( NumberOfCharactersUsedInConversion == (int) ReaderConfiguration->ExtraneousTextAfterCommand->Size )
		{
			DisplayMemoryBaseAddress = NewAddress;
		}
		else
		{
			// There was extraneous text included with the number; WOOF!
			Status = false;
		}
	}


	// If no errors occurred, dump.
	if( Status )
	{
		// Make sure that we're on a two byte boundary.
		DisplayMemoryBaseAddress = ( char * ) ( (( unsigned int ) DisplayMemoryBaseAddress ) & 0xFFFFFFFE );
		short	*shDisplayMemoryBaseAddress = (short *) DisplayMemoryBaseAddress;

		char	ResultString[ 160 ];

		sprintf (ResultString,"\r\n%08X - %04X\r\n", ((int) DisplayMemoryBaseAddress), ((int) *shDisplayMemoryBaseAddress) ) ;
		theStdInterface.Write(ResultString);

		DisplayMemoryBaseAddress += 2;
	}

	return Status;
} // DumpTwoBytes




//
//	DumpFourBytes
//		Dump four bytes.
//
bool DumpFourBytes( void )
{
	bool	Status = true;		// No problems encountered yet.


	// If there is an address included, go get it, else we'll use the last address specified.
	if( ReaderConfiguration->ExtraneousTextAfterCommand->Size > 0 )
	{
		int		NumberOfCharactersUsedInConversion;
		char	*NewAddress = (char *) ReaderConfiguration->ExtraneousTextAfterCommand->ConvertHexToNumber
		                   ( 0, ReaderConfiguration->ExtraneousTextAfterCommand->Size - 1, &NumberOfCharactersUsedInConversion );
		if( NumberOfCharactersUsedInConversion == (int) ReaderConfiguration->ExtraneousTextAfterCommand->Size )
		{
			DisplayMemoryBaseAddress = NewAddress;
		}
		else
		{
			// There was extraneous text included with the number; WOOF!
			Status = false;
		}
	}


	// If no errors occurred, dump.
	if( Status )
	{
		// Make sure that we're on a four byte boundary.
		DisplayMemoryBaseAddress = ( char * ) ( (( unsigned int ) DisplayMemoryBaseAddress ) & 0xFFFFFFFC );
		int		*iDisplayMemoryBaseAddress = (int *) DisplayMemoryBaseAddress;

		char	ResultString[ 160 ];

		sprintf (ResultString,"\r\n%08X - %08X\r\n", ((int) DisplayMemoryBaseAddress), *iDisplayMemoryBaseAddress ) ;
		theStdInterface.Write(ResultString);

		DisplayMemoryBaseAddress += 4;
	}

	return Status;
} // DumpFourBytes





//
//	GetAddressAndDataToSet
//		Get and address and data to set from the input string.  The form of the string is A:D, where A is the address expressed in hexadecimal
//		and D is the data expressed in hexadecimal.
//
bool GetAddressAndDataToSet( HSTRING *InputString, unsigned char **Address, int *Data )
{
	bool	Status = true;		// No problems encountered yet.


	// Get the address and data, which are both in hexadecimal and are separated by a colon.
	if( InputString->Size > 0 )
	{
		int		NumberOfCharactersUsedInConversion;
		int		PositionOfColon;

		bool	ColonFound = InputString->Find( (unsigned char *) ":", 1, false, 1, &PositionOfColon );

		if(( ColonFound ) && ( PositionOfColon > 0 ) && ( PositionOfColon < ( (int) InputString->Size - 1 ) ))
		{
			*Address = (unsigned char *) InputString->ConvertHexToNumber( 0, PositionOfColon - 1, &NumberOfCharactersUsedInConversion );
			if( NumberOfCharactersUsedInConversion == PositionOfColon )
			{
				*Data = InputString->ConvertHexToNumber( PositionOfColon + 1, InputString->Size - 1, &NumberOfCharactersUsedInConversion );
				if( NumberOfCharactersUsedInConversion != ( (int) InputString->Size - PositionOfColon - 1 ) )
				{
					// Corrupt data; WOOF!
					Status = false;
				}
			}
			else
			{
				// Corrupt address; WOOF!
				Status = false;
			}
		}
		else
		{
			// No colon was found; WOOF!
			Status = false;
		}
	}
	else
	{
		Status = false;
	}

	return Status;
} // GetAddressAndDataToSet




//
//	SetOneByte
//		Set a byte.
//
bool SetOneByte( void )
{
	unsigned char	*Address;
	int				Data;
	bool			Status = GetAddressAndDataToSet( ReaderConfiguration->ExtraneousTextAfterCommand, &Address, &Data );

	if( Status )
	{
		*Address = ( char ) Data;
	}

	return Status;
} // SetOneByte




//
//	SetTwoBytes
//		Set two bytes.
//
bool SetTwoBytes( void )
{
	unsigned char	*Address;
	int				Data;
	bool			Status = GetAddressAndDataToSet( ReaderConfiguration->ExtraneousTextAfterCommand, &Address, &Data );

	if( Status )
	{
		// Make sure that we're on a two byte boundary.
		Address = ( unsigned char * ) ( (( unsigned int ) Address ) & 0xFFFFFFFE );

		unsigned short		*shAddress = (unsigned short *) Address;
		*shAddress = ( unsigned short ) Data;
	}

	return Status;
} // SetTwoBytes




//
//	SetFourBytes
//		Set four bytes.
//
bool SetFourBytes( void )
{
	unsigned char	*Address;
	int				Data;
	bool			Status = GetAddressAndDataToSet( ReaderConfiguration->ExtraneousTextAfterCommand, &Address, &Data );

	if( Status )
	{
		// Make sure that we're on a four byte boundary.
		Address = ( unsigned char * ) ( (( unsigned int ) Address ) & 0xFFFFFFFC );

		unsigned int		*iAddress = (unsigned int *) Address;
		*iAddress = ( unsigned int ) Data;
	}

	return Status;
} // SetFourBytes
void DumpBytes(unsigned char * buff,int length)
{
	for (int i= 0;i<length;i+=32)
	{
		theStdInterface.printf("%8.8X ",i);
		for (int j=0;j<32;j++)
			theStdInterface.printf("%2.2X ",buff[i+j]);
		theStdInterface.printf("\r\n");
	}

}

bool Readspiflash( void )
{
	unsigned char	*Address;
	int				Data;
	unsigned int buffer[512/sizeof(int)];

	bool			Status = GetAddressAndDataToSet( ReaderConfiguration->ExtraneousTextAfterCommand, &Address, &Data );

	if( Status )
	{
		SpiReadDbl((unsigned int)Address,Data,(char *)buffer);
		DumpBytes( (unsigned char *)buffer,Data);
	}

	return Status;
} // SetFourBytes


//
//	DiagnosticReset
//		Reset device.
//
bool DiagnosticReset( void )
{
	SoundMenuBeep();
	while(IsBeepInProgress())
//		RESET_WATCHDOG_TIMER();
	reset_to_boot_code_hard();

	return false;		// Should never return
} // DiagnosticReset

#ifdef BATCH_DEBUG
//
// Create Sequencial Barcodes
//
bool CreateSequenceCodes( void )
{
#ifdef RFSCAN
	bool ret = true;
	int i = 0;
	//HSTRING hData(8, true);
	while( i < 1000 && ret == true )
	{
		wchar_t temp[13];
		swprintf(temp,12,L"%iTest%d\r\n",GET_SETTING(WorkgroupSelection),i);
		tichar_t *pText;
		size_t tmpLen = 20;

		CBarcodeData *pCurrentMsg = CBarcodeData::GetSaveNewBuffer( pText, tmpLen );
		if( pCurrentMsg == NULL )
		{
			ret = false;
			break;
		}
		HWSTRING	WideDecodeResult( wcslen(temp), true );
		WideDecodeResult.Copy( temp );
		pCurrentMsg->SetUnicodeData();
		pCurrentMsg->StoreHHP('6', 0);
		pCurrentMsg->StoreAim('Z', '6');
		pCurrentMsg->StoreWText( WideDecodeResult );
		pCurrentMsg->FinalizeBuffer( WideDecodeResult.Size );

		ret = theScriptInterpreter.SendAsWrappedBarcode(pCurrentMsg);
		//ret = theScriptInterpreter.AddToBatchModeQueue( pCurrentMsg );
		pCurrentMsg->Free();
		i++;
	}
	return true;
#else

	return false;
#endif
}
#endif

//
//	DelayedReset
//		Do a delayed reset of the device.  Put out a string to the host to let them know we received the command.  Turn off flash updates.
//
bool DelayedReset( void )
{
	FlashAndReSynchCustomerDefaults();
	FlashWorkingChanges();

	RAM_SET_ACTUAL( FlashingEnabled, false );		// Turn off flashing first.
	xputstring( "030B00\006" );
	xputchar( ReaderConfiguration->CurrentTable );
	wait_til_tx_is_empty();

	while(IsBeepInProgress())
	{
//		RESET_WATCHDOG_TIMER();
	}


	DISABLE_INTERRUPTS(); 	// to be safe, make sure I stay here and go nowhere else... after all, I want to reset!!

	CTimeout	DelayedResetTimer( 10000000 );
	while( !DelayedResetTimer.HasExpired() )
	{
//		RESET_WATCHDOG_TIMER();
	}

	reset_to_boot_code_hard();

	return false;		// Should never return
} // DiagnosticReset




//
//	GetFlashCheckPointers
//		Get pointer to a particular flash file or to the entire flash if no file was specified.
//
bool GetFlashCheckPointers( HSTRING *FileName, unsigned char **FlashFilePointer, unsigned int *FlashFileSize )
{
	bool			Status = true;		// Assume success until we know otherwise.

	return Status;
} // GetFlashCheckPointers




//
//	DiagnosticChecksum
//		Calculate simple checksum (not alternating) on a particular flash file or on the entire flash if no file was specified.
//
bool DiagnosticChecksum( void )
{
#ifdef FIXME
	unsigned char	*FlashFilePointer;
	unsigned int	FlashFileSize;
	bool Status = GetFlashCheckPointers( ReaderConfiguration->ExtraneousTextAfterCommand, &FlashFilePointer, &FlashFileSize );

	// If we found data, compute the simple checksum.
	if( Status )
	{
		char	temp[ 80 ];
		unsigned int	Checksum;
		int				i;
		Checksum = (unsigned int) FlashFilePointer[ 0 ];
		for( i = 1;    i < FlashFileSize;    ++i )
			Checksum += (unsigned int) FlashFilePointer[ i ];

		sprintf( temp, "%08X", Checksum );
		ReaderConfiguration->MenuCommandResponse->Append( temp );
	}

	return Status;
#else

	return false;
#endif
} // DiagnosticChecksum




//
//	DiagnosticCRC
//		Calculate CRC on a particular flash file or on the entire flash if no file was specified.
//
bool DiagnosticCRC( void )
{
#ifdef FIXME
	unsigned char	*FlashFilePointer;
	unsigned int	FlashFileSize;
	bool Status = GetFlashCheckPointers( ReaderConfiguration->ExtraneousTextAfterCommand, &FlashFilePointer, &FlashFileSize );

	// If we found data, compute the CRC.
	if( Status )
	{
		char	temp[ 80 ];
		sprintf( temp, "%08X", calc_crc( FlashFilePointer, FlashFileSize ) );
		ReaderConfiguration->MenuCommandResponse->Append( temp );
	}
	return Status;
#else

	return false;
#endif
} // DiagnosticCRC




//
//	DiagnosticOutputString
//		Translate formats in the parameter sent, then send it back.
//
bool DiagnosticOutputString( void )
{
	HSTRING		Text( ReaderConfiguration->ExtraneousTextAfterCommand, true );
	Text.UnprintableFormatsToCharacters();
	xputstring( Text.Char, Text.Size );

	return true;
} // DiagnosticOutputString


//DOG DEBUG added to MAKEFILE. UNCOMMENT IT THERE.
#ifdef WDOGDEBUG
//
//	CauseWatchDogReset
//		Cause a watchdog reset.
//
bool CauseWatchDogReset( void )
{
	while( RAM_RESET != 0 )
		;

	return false;		// Should never return
} // CauseWatchDogReset
#endif





#ifdef	HEAP_DEBUG
//
//	ShowUsedHeapSpaceWithOptionalSorting
//		Show used heap space, using the optional parameter included with the command.
//
bool ShowUsedHeapSpaceWithOptionalSorting(void)
{
	ShowUsedHeapSpace( (char *) ReaderConfiguration->ExtraneousTextAfterCommand->Char );

	return true;
} // ShowUsedHeapSpaceWithOptionalSorting
#endif




#ifdef READER_CONFIGURATION_STORAGE_DEBUG
//
//	DisplayReaderConfigurationDuplicateActiveValues
//		Get a list of duplicate active values and display it.
//
bool DisplayReaderConfigurationDuplicateActiveValues( void )
{
	HSTRING	List( 20, true );
	ReaderConfiguration->BuildListOfSettingsWithCommonActiveValue( &List );
	theStdInterface.Write( (char *) List.Char, List.Size );
	theStdInterface.Write( "\r\n" );

	return true;
} // DisplayReaderConfigurationDuplicateActiveValues
#endif











//
//	DumpI2CRegisters
//		Dump the I2C registers and break them down into fields for reporting
//
bool DumpI2CRegisters( void )
{
	return true;
} // DumpI2CRegisters



bool DbgTrigger(void)
{
#ifdef FIXME
#ifndef RFBASE
	pTrigger->DebugIt();
#endif
#endif

	return TRUE;
}

#if !defined( RFBASE )
bool DBGIsUnitInStand(void)
{

	if (ReaderConfiguration->ExtraneousTextAfterCommand->Compare("?",true))
		return false;
	ReaderConfiguration->MenuCommandResponse->Append((pCHWTrigger->ReadReedSwitch())?"1":"0");

	return true;
}
#endif

extern "C" void ForceIllegalInstructiion(void);
extern "C" void ForcePrefetchAbort(void);
int idiv(int num,int denom)
{
	return num/denom;
}
extern int test_zero;
bool TestAborts(void)
{
	volatile int temp;

	switch (ReaderConfiguration->ExtraneousTextAfterCommand->Char[0])
	{
	case '0':
		{
			temp = *((int *)0xfffffff0); // data
			return (temp!=0);
		}
	case '1':		// prefetch abort
		ForcePrefetchAbort();
		return true;
	case '2':	// illegal instruction
		ForceIllegalInstructiion();

	case '3':	// software int
		// This is not portable code, for now I exclude it in WINSIM. Should be cleaned up.
		// The correct way is to create a function in a HW file.
#ifndef WINSIM	                           // a simulated debug environement with MSVC

		asm ("SWI 0x00234444");  //0x02
#endif

		return true;
	case '4':
		DISABLE_NORMAL_INTERRUPTS();
		while (true)
		{}
		;
	case '5':
		{
			temp=100;
			for (int i=-10;i<10;i++)
				temp += temp/i;
			return temp>0;
		}
	case '6':
		{
		    return __aeabi_fdiv(100.0,0.0)<0.0;
		}
	case '7':
		{
			reset_to_boot_code_hard();
		}

		default:
				return false;
	}

}
bool DBG2IF( void )
{
	//	return theHal.IsSecondary();
	return true;
}


#ifndef RFBASE
#ifdef	ENABLE_FLEX_TESTS

//****************************************************************************************************************************
//Test Flex and Test Current support functions
//****************************************************************************************************************************

extern CBarcodeFifo *DecodeResults;
//extern int BlockDecoderFromExecuting;        //fixme

// Initialize imager settings																						//values based on setting below
static int AcceptanceDelta = 25;                //acceptance delta from target white         GET_SETTING(ImageSnapAcceptanceDelta)
static int NumberOfTries = 6;               		//image snap attempts to conform			      GET_SETTING(ImageSnapAttempts)
static int TargetWhiteValue = 125;              //target white                               GET_SETTING(ImageSnapWhiteTarget)
static DWORD ExpoValue = 200;                   //exposure                                   GET_SETTING(ImageSnapExposure)
static DWORD GainValue = 1;                     //gain                                       GET_SETTING(ImageSnapGain)
static int TargetSetPoint = 50;                 //image snap percentage point                GET_SETTING(ImageSnapPercent)
static BOOL MustConform = false;                //Image must conform to target white         GET_SETTING(bImageMustConform)
static BOOL AdjustExposureLightsOff = false;		//Adjust exposure when lights are off			GET_SETTING(AutoExposureLightsOffAdjust)
static BOOL	ValuesSaved = false;


//1:	Stores old values and sets new test values
//2:	Restores the old values
bool SetOrRestoreImagerRegisterValues( int SetOrRestore )
{
	bool bReturnValue = false;
	DWORD ModeToSend;
	static DWORD ModeToRestore;
	DWORD	DataToSend[20];
	static DWORD DataToRestore[20];
	DWORD	DataToSendSize = 0;
	static DWORD DataToRestoreSize;

	if( !ValuesSaved )
	{
		if (1)// FIXME(GET_CONFIG_SETTING(iOpticsPopulation)==TWO_D) //Are we 2D??
		{
			DataToSendSize = 20;

			//set up new image settings
			DataToSend[0] = ES_CONFORM_IMAGE;
			DataToSend[1] = MustConform;
			DataToSend[2] = ES_MAX_EXP;
			DataToSend[3] = ExpoValue;
			DataToSend[4] = ES_TARGET_VALUE;
			DataToSend[5] = TargetWhiteValue;
			DataToSend[6] = ES_TARGET_PERCENTILE;
			DataToSend[7] = TargetSetPoint;
			DataToSend[8] = ES_TARGET_ACCEPT_GAP;
			DataToSend[9] = AcceptanceDelta;
			DataToSend[10] = ES_CONFORM_TRIES;
			DataToSend[11] = NumberOfTries;
			DataToSend[12] = ES_MAX_GAIN;
			DataToSend[13] = GainValue;
			DataToSend[14] = ES_EXPOSURE_METHOD;
			DataToSend[15] = HHPSD_AE_METHOD_UNIFORM;
			DataToSend[16] = ES_SPECULAR_EXCLUSION;
			DataToSend[17] = HHPSD_SPECULAR_EXCLUSION_OFF;
			DataToSend[18] = ES_ADJUST_EXP_WHEN_ILLUM_OFF;
			DataToSend[19] = AdjustExposureLightsOff;

			DataToRestoreSize = 20;

			//store original image settings
			DataToRestore[0] = ES_CONFORM_IMAGE;
			DataToRestore[2] = ES_MAX_EXP;
			DataToRestore[4] = ES_TARGET_VALUE;
			DataToRestore[6] = ES_TARGET_PERCENTILE;
			DataToRestore[8] = ES_TARGET_ACCEPT_GAP;
			DataToRestore[10] = ES_CONFORM_TRIES;
			DataToRestore[12] = ES_MAX_GAIN;
			DataToRestore[14] = ES_EXPOSURE_METHOD;
			DataToRestore[16] = ES_SPECULAR_EXCLUSION;
			DataToRestore[18] = ES_ADJUST_EXP_WHEN_ILLUM_OFF;
		}
		else	//1D linear
		{
			DataToSendSize = 6;

			//set up new image settings
			DataToSend[0] = ES_MAX_GAIN;
			DataToSend[1] = GainValue;
			DataToSend[2] = ES_MAX_EXP;
			DataToSend[3] = ExpoValue;
			DataToSend[4] = ES_CONFORM_IMAGE;
			DataToSend[5] = MustConform;

			DataToRestoreSize = 6;

			//store original image settings
			DataToRestore[0] = ES_MAX_GAIN;
			DataToRestore[2] = ES_MAX_EXP;
			DataToRestore[4] = ES_CONFORM_IMAGE;
		}
	}

	if( (1 == SetOrRestore) && (!ValuesSaved) )
	{
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_EXPOSURE_MODE,(PBYTE)&ModeToRestore,1*sizeof(DWORD),NULL,0,NULL);
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_EXPOSURE_SETTINGS,(PBYTE)DataToRestore,DataToRestoreSize*sizeof(DWORD),NULL,0,NULL);
		ModeToSend = HHPSD_EXPOSURE_FIXED;                        //alternate mode to test: HHPSD_AUTOEXPOSURE_USE_HHP
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_MODE,(PBYTE)&ModeToSend,1*sizeof(DWORD),NULL,0,NULL);
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)DataToSend,DataToSendSize*sizeof(DWORD),NULL,0,NULL);
		ValuesSaved = true;
		bReturnValue = true;
	}
	if( (2 == SetOrRestore) && (ValuesSaved) )
	{
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_MODE,(PBYTE)&ModeToRestore,1*sizeof(DWORD),NULL,0,NULL);
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)DataToRestore,DataToRestoreSize*sizeof(DWORD),NULL,0,NULL);
		ValuesSaved = false;
		bReturnValue = true;
	}

	return bReturnValue;
}



#define	SET_TAG_SIZE		8

//set light mode to fixed test state, 0 is off, see case statement below for other states, states 1 through 5 only
//set the state of the lights for scan and idle modes.
void SetTestLights( int iOnOff )
{
	DWORD setTags[SET_TAG_SIZE];

	setTags[0] = LI_ILLUMINATION_INTENSITY;				//location 0 is tag
	setTags[1] = GET_SETTING( iTestCurrentLedDutyCycle );
	setTags[2] = LI_AIMER_INTENSITY;                  	//location 2 is tag
	setTags[3] = 100;
	setTags[4] = LI_SCAN_MODE;            					//location 4 is tag
	setTags[5] = HHPSD_LM_CONCURRENT;	 																// alternate value HHPSD_LM_ILLUM_ON_AIM_ON;
	setTags[6] = LI_IDLE_MODE;                  			//location 6 is tag
	setTags[7] = HHPSD_LM_ILLUM_ON_AIM_ON;

	// Initialize settings
	switch ( iOnOff )
	{
	case 7:
		{
			setTags[5] = HHPSD_LM_CONCURRENT;             	//scanning
			setTags[7] = HHPSD_LM_ILLUM_ON;             		//idle
			break;
		}
	case 6:
		{
			setTags[5] = HHPSD_LM_CONCURRENT;             	//scanning
			setTags[7] = HHPSD_LM_AIM_ON;             		//idle
			break;
		}
	case 5:
		{
			setTags[5] = HHPSD_LM_CONCURRENT;             	//scanning
			setTags[7] = HHPSD_LM_ILLUM_ON_AIM_ON;          //idle
			break;
		}
	case 4:
		{
			setTags[5] = HHPSD_LM_INTERLACED;            		//scanning
			setTags[7] = HHPSD_LM_ILLUM_ON_AIM_ON;            	//idle
			break;
		}
	case 3:
		{
			setTags[5] = HHPSD_LM_ILLUM_ON_AIM_ON;            //scanning
			setTags[7] = HHPSD_LM_ILLUM_ON_AIM_ON;            //idle
			break;
		}
	case 2:
		{
			setTags[5] = HHPSD_LM_ILLUM_ON;            //scanning
			setTags[7] = HHPSD_LM_ILLUM_ON;            //idle
			break;
		}
	case 1:
		{
			setTags[5] = HHPSD_LM_AIM_ON;           	//scanning
			setTags[7] = HHPSD_LM_AIM_ON;             //idle
			break;
		}
	case 0:
		{
			setTags[1] = GET_SETTING(LedIntensityPercentage);
			setTags[3] = GET_SETTING(LedIntensityPercentage);
			setTags[5] = HHPSD_LM_OFF;                                //scanning
			setTags[7] = HHPSD_LM_OFF;			                          //idle
			break;
		}
	default:
		{
			setTags[1] = setTags[3]=setTags[5]=setTags[7]=(DWORD)-1;
		}
	}

	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_LIGHT_SETTINGS,PBYTE(setTags),(DWORD)(SET_TAG_SIZE*sizeof(DWORD)),0, 0, 0  );
}



//Do a single frame image capture with fixed settings and exposure
bool GrabOneImageFrame(void)
{
	bool bReturnValue = false;
	bool bImageResult = TRUE;

	ValuesSaved = false;         //global file scope used for image parameter saving

	//lights off
	SetTestLights(0);

	//set up new imager values
	bReturnValue = SetOrRestoreImagerRegisterValues(1);

	if( bReturnValue )
	{
		//capture a single frame
		bImageResult = SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SINGLE_FRAME_CAPTURE,(UCHAR *) &P_SCANBUFFER,0,NULL,0,NULL);
	}

	//restore original imager values
	bReturnValue = SetOrRestoreImagerRegisterValues(2);

	return bImageResult;
}



//****************************************************************************************************************************
//Test flex data
//****************************************************************************************************************************
#define OFFSET_COL	256L  // the first & last columns can be a wraparound of the grayscale ramp--we want to ignore them
#define DELTA_Y		256L  // how many rows we skip for each scan line
#define DELTA_X		256L  // how many columns we skip for 8 bit pixel resolution 

#define	LMC_DEBUG	0		//Set to one for full debugging output using General Debug ON, default to 0          //lmc_debug
#define	SCAN_DRIVER_RETRIES     5
#define	WAIT_DELAY_TIME			20			//in ms
#define	RE_SEND_DEBUG				0			//Set to one for re-send debugging, default to 0           //lmc_debug
/*----------------------------------------------------------------------------------------
TestFlexCommand()

  Tests the flex strip connection between the main CPU and the imager's PSOC.  We stop
  any scanning by placing the device in manual trigger mode (we restore the trigger mode
  when we're done).  Then we test the I2C connection.  If that passes, we put the imager
  into a grayscale test mode and retrieve/test the image.  Manufacturing wanted to know
  where the first bad pixel was, so we don't just do a histogram (and ensure all pixels
  are represented).  Instead, we go horizontally across and ensure the pixels ramp in
  magnitude.  Note, the Micron imager does a single ramp that spans the width of the image.
  Therefore, some adjacent pixels will be the same.  Also, the Micron imager's test image
  has a ramp wraparound at each edge, so we have to compensate for this (by ignoring the
  left and right most columns).  If future imagers don't send a similar test image, the
  IOCTL will have to return some information concerning the image (ramps up going to the
  left, etc.), or we'll just do the histogram part & leave out the reporting of the first
  bad pixel.

  Parameters Passed:  unsigned int iTestMode

			0:	uses menued test mode ("TSTGRY") instead of passed parameter, and sets trigger mode to manual. Runs full test if "TSTGRY" is set to zero (default).
			1:	sets test mode to one (to disable gray scale mode) and sets trigger mode to manual.
			2:	sets test mode to two (to enable gray scale mode) and sets trigger mode to manual.
			3:	sets test mode to one (disable gray scale) and does not set trigger mode.
			4:	sets test mode to two (enable gray scale) and does not set trigger mode.

			NOTE: menued test mode "iTestFlexTestMode" only supports modes 0, 1, and 2, when
					iTestMode passes a zero in (zero default). To use this test mode,
					set "TSTGRY" first to mode 0, 1, or 2, as described above,  then run "TSTFLX"
					command to actually set the mode you want.

  Returns:  true if test was successful, false otherwise
----------------------------------------------------------------------------------------*/
bool TestFlexCommand( unsigned int iTestMode )
{
	DWORD status = 1;
	unsigned char address = 0xff;		//Jade Id register
	unsigned short JadeData1;
	unsigned short JadeData2;
	bool bIsOk = true;
	int iTestFlexTestModeCurrent;
	int i;
	int count = SCAN_DRIVER_RETRIES;            //number of I2C retries
	ULONG	waitDelayTime = 0;
	BOOL bRestartScanning = false;

	//Make a delay timer
	CTimeoutmS	waitDelay(WAIT_DELAY_TIME);          //20 ms

	// Go into TRGMOD0 to stop any scanning that might be going on, and then set scan driver time out to lock scanning back on
	//unsigned int OrigTriggerMode = 0;//FIXME remove pTrigger->GetTriggerMode();
	unsigned int OrigTriggerMode = pTrigger->GetTriggerMode();	 
	unsigned int OrigScanDriverTimeOut = GET_SETTING(ScanDriverTimeout);         //range is 0 to 999999
	char OrigScanDriverTimeOutString[10 + 1];
	unsigned int OrigExposureMax = GET_SETTING(MaximumExposure);         //range is 0 to 7874
	char OrigExposureMaxString[10 + 1];
	unsigned int OrigPowerLevel = GET_SETTING(ImagerIdlePowerLevel);     //0 == power off, 1 == standby, 2 == running but not triggered
	char OrigPowerLevelString[10 + 1];

	sprintf(OrigScanDriverTimeOutString,"%u",OrigScanDriverTimeOut);
	sprintf(OrigExposureMaxString,"%u",OrigExposureMax);
	sprintf(OrigPowerLevelString,"%u",OrigPowerLevel);

	if ( OrigTriggerMode == TEMP_MANUAL_TRIG ) OrigTriggerMode = GET_SETTING(TriggerMode);

	HSTRING TrgMod("0E01000!", true);                    //manual trigger
	HSTRING ScanDrivTimOut("024A001!", true);            //stop scanning
	HSTRING ExpMax("040203100!", true);                    //set max exposure for 13ms for fast shutdown
	HSTRING PowerLevel("0408161!", true);                  //set Power Level so we stay running

	if( (iTestMode >= 5) )              //if test mode not valid
	{
		return false;
	}

	if( (iTestMode <= 2) )              //if test mode is 0, 1, or 2, set trigger mode
	{
		if ( ! ReaderConfiguration->Menu(&TrgMod) )
		{
			theStdInterface.printf("Unable to put device in manual trigger mode.");
			return false;
		}

		if ( ! ReaderConfiguration->Menu(&ScanDrivTimOut) )
		{
			theStdInterface.printf("Unable to set scan driver time out.");
			return false;
		}

		if ( ! ReaderConfiguration->Menu(&ExpMax) )
		{
			theStdInterface.printf("Unable to set scan driver max exp.");
			return false;
		}

		if ( ! ReaderConfiguration->Menu(&PowerLevel) )
		{
			theStdInterface.printf("Unable to set scan driver power level.");
			return false;
		}

		pTrigger->UpdateTriggerStatus();
		pTrigger->SetBlockDecoderFromExecuting(TRUE);     //set decode blocking right after update trigger status

		// if STOP_SCANNING returns non-zero it means that imaging was running, and needed to be shutdown, and will
		// therefore need to be restarted
		bRestartScanning = SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_STOP_SCANNING,NULL,0,NULL,0,NULL);
	}


	// Talk to the Jade to ensure comm works--read the same data twice and compare.	
	SetTestLights(0);     //lights off
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_START_SCANNING,NULL,0,NULL,0,NULL);    //turn on and off scanning to set imager power on so we can talk I2C.
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_STOP_SCANNING,NULL,0,NULL,0,NULL);
	MT_Delay(25);	//wait for frame with short exposure to finish, before we try to talk I2C
#ifdef HI2D_T
#ifdef BYD_SENSOR
	if(ReadIIC_BYD(address,&JadeData1,1))
	{
		if(ReadIIC_BYD(address,&JadeData2,1))
		{
			if( JadeData1 != JadeData2 )		bIsOk = false;
		}
		else	
		{
			bIsOk = false;
		}
	}
	else
	{
		bIsOk = false;
	}
#else	//else #ifdef BYD_SENSOR
	if(ReadIIC_Hi704A(address,&JadeData1,1))
	{
		if(ReadIIC_Hi704A(address,&JadeData2,1))
		{
			if( JadeData1 != JadeData2 )		bIsOk = false;
		}
		else	
		{
			bIsOk = false;
		}
	}
	else
	{
		bIsOk = false;
	}
#endif	//end #ifdef BYD_SENSOR
#else	//else #ifdef HI2D_T
	if(ReadIIC_Jade(address,&JadeData1,1))
	{
		if(ReadIIC_Jade(address,&JadeData2,1))
		{
			if( JadeData1 != JadeData2 )		bIsOk = false;
		}
		else	
		{
			bIsOk = false;
		}
	}
	else
	{
		bIsOk = false;
	}
#endif	//end #ifdef HI2D_T

	if ( ! bIsOk )        //Jade read failed
	{
		theStdInterface.printf("Unable to communicate with the Jade.");
		return false;
	} 

	//Choose the test to run
	if( (iTestMode > 0) && (iTestMode <= 2) )      //use the parameter passed if it is 1 or 2
	{
		iTestFlexTestModeCurrent = iTestMode;
	}
	else if( (iTestMode > 2) && (iTestMode < 5) )      //else modify parameter passed if it is 3 or 4
	{
		iTestFlexTestModeCurrent = (iTestMode - 2);
	}
	else                                             //else if zero passed, use the menu setting
	{
		iTestFlexTestModeCurrent =  GET_SETTING( iTestFlexTestMode );
	}

	//turn imager gray scale test mode on and leave, does not run histogram test, allows you to ship gray scale pattern images over and over
	if( iTestFlexTestModeCurrent == 2 )
	{
		if( GET_SETTING(GeneralDebugOn) )
		{
			theStdInterface.printf("Enter Test Flex Mode %d \r\n", iTestFlexTestModeCurrent);      //lmc_debug
		}

		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_IMAGER_GRAYSCALE, NULL, 0, (BYTE *)&status, sizeof status, NULL);	// Place image sensor into gray scale test mode

		if( status == SDHW_ERR_NONE )
		{
			if( (iTestMode <= 2) )              //if test mode is 0, 1, or 2, get image
			{
                //HSTRING ImgSnp("IMGSNP1P1U0L.", true);  // Manual mode (2P) would not retrieve a frame if one was already present (a bug), so we can't use it.  We use photo so we can tell it to just grab one frame (1U).
				//1P - Photo style image, 1U - one update try (grab one frame). 0L - LEDS off
				//ReaderConfiguration->Menu(&ImgSnp);
				
				if( GrabOneImageFrame() )
				{
					bIsOk = true;
				}
				else
				{
					theStdInterface.printf("\r\n Unable to grab image from ScanDriver ");
					bIsOk = false;
				}
			}
			else
			{
				bIsOk = true;
			}
		}
		else
		{
			if( status == SDHW_ERR_NOT_IMPLEMENTED ) theStdInterface.printf("\r\n Grayscale test not implemented ");
			else theStdInterface.printf("\r\n Unable to enable imager grayscale mode ");
			bIsOk = false;
		}
	}

	//turn imager gray scale test mode off and leave, shuts above mode ( # 2 ) off when you are finished
	if( iTestFlexTestModeCurrent == 1 )
	{
		if( GET_SETTING(GeneralDebugOn) )
		{
			theStdInterface.printf("Enter Test Flex Mode %d \r\n", iTestFlexTestModeCurrent);      //lmc_debug
		}

		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_UNSET_IMAGER_GRAYSCALE, NULL, 0, (BYTE *) &status, sizeof status, NULL);	// Place image sensor back to non test mode

		if( status == SDHW_ERR_NONE )
		{
			if( (iTestMode <= 2) )              //if test mode is 0, 1, or 2, get image
			{
			   //HSTRING ImgSnp("IMGSNP1P1U0L.", true);  // Manual mode (2P) would not retrieve a frame if one was already present (a bug), so we can't use it.  We use photo so we can tell it to just grab one frame (1U).
				//1P - Photo style image, 1U - one update try (grab one frame). 0L - LEDS off
				//ReaderConfiguration->Menu(&ImgSnp);
				
				if( GrabOneImageFrame() )
				{
					bIsOk = true;
				}
				else
				{
					theStdInterface.printf("\r\n Unable to grab image from ScanDriver ");
					bIsOk = false;
				}
			}
			else
			{
				bIsOk = true;
			}
		}
		else
		{
			if ( status == SDHW_ERR_NOT_IMPLEMENTED ) theStdInterface.printf("\r\n Grayscale test not implemented ");
			else theStdInterface.printf("\r\n Unable to disable imager grayscale mode ");
			bIsOk = false;
		}
	}

	//Normal test flex test mode, the MFG test
	if( (iTestFlexTestModeCurrent == 0) )
	{
		waitDelay.Reset();

		// Place image sensor into grayscale test mode
		for(i = 0; i < count; i++)
		{
			SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_IMAGER_GRAYSCALE, NULL, 0, (BYTE *)&status, sizeof status, NULL);
			if( status == SDHW_ERR_NONE )
			{
				break;
			}
			else
			{
				RESET_WATCHDOG_TIMER();

				if( i < (count - 1) )
				{
					while( ! waitDelay.HasExpired() )   MT_Delay(5);

					waitDelayTime = (ULONG) (WAIT_DELAY_TIME * (i + 1));			//Adjust backoff as # of retries increases
					waitDelay.SetExpiration( waitDelayTime );
				}

				if( (GET_SETTING(GeneralDebugOn)) || RE_SEND_DEBUG  )           //outputs number of retries to send command (I2C error)
				{
					theStdInterface.printf("\r\n Number of Retries to enter Grayscale Mode: %d Time: %d \r\n\r\n", (i+1),  (WAIT_DELAY_TIME*(i + 1)) );      //lmc_debug
				}
			}
		}

		//Retrieve image, and test
		if( (status == SDHW_ERR_NONE) )         //was able to enter gray scale mode
		{
			if( (iTestMode <= 2) )              //if test mode is 0, 1, or 2, get image
			{
				//HSTRING ImgSnp("IMGSNP1P1U0L.", true);  // Manual mode (2P) would not retrieve a frame if one was already present (a bug), so we can't use it.  We use photo so we can tell it to just grab one frame (1U).
				//1P - Photo style image, 1U - one update try (grab one frame). 0L - LEDS off
				//ReaderConfiguration->Menu(&ImgSnp);
				
				if( GrabOneImageFrame() )
				{
					bIsOk = true;
				}
				else
				{
					theStdInterface.printf("\r\n Unable to grab image from ScanDriver ");
					bIsOk = false;
				}
			}
			else		//no image
			{
				bIsOk = false;
			}

			if( bIsOk )    //got an image, so test it
			{
				if ( P_SCANBUFFER )
				{
					BYTE * pPixel;
					DWORD ImgWidth;
					DWORD histogram[256];  // holds a count of pixels at a particular value
					int x, y;
					unsigned int uiPixMax, uiPixMin, uiColumn;

					uiColumn = 0;
					memset(histogram, 0, sizeof histogram);
					SCN_IOControl(nOpenCntxt, HHPSD_IOCTL_GET_SCAN_WIDTH,NULL, 0, (BYTE *)&ImgWidth, sizeof ImgWidth, NULL);

					for (y = 1; (y <= 2) && bIsOk; y++)        //test rows 256 and 512
					{	// Check several horizontal lines (one should be enough, but let's do more)
						pPixel = P_SCANBUFFER + ((y * DELTA_Y) * ImgWidth) + DELTA_X;
						uiPixMax = 0;
						uiPixMin = 256;

						for (x = DELTA_X; (x < 3 * OFFSET_COL) && bIsOk; x++)      //test pixels 256 to 767 
						{       
							if( (*(pPixel + 1) < *pPixel) || (*(pPixel + 1) - *pPixel > 1) )
							{
								if( *pPixel != 255 )               //if we are not starting a new ramp
								{
									theStdInterface.printf("Horizontal pixels not in nondecreasing order (location %d = %d, next pixel val = %d)\r\n", x, *pPixel, *(pPixel + 1));
									bIsOk = false;
								}
							}

							if( *pPixel > uiPixMax )
							{
								uiPixMax = *pPixel;         //store largest pixel value found
							}

							if( *pPixel < uiPixMin )
							{
								uiPixMin = *pPixel;        //store smallest pixel value found
							}

							if( (GET_SETTING(GeneralDebugOn) && ((x%10)==0)) && LMC_DEBUG )
							{
								theStdInterface.printf("PixNum %d PixVal %d \r\n", x, *pPixel );      //outputs current pixel info	lmc_debug
							}

							histogram[*pPixel]++;
							pPixel++;
						}//column loop

						uiColumn++;

						if( (GET_SETTING(GeneralDebugOn)) && LMC_DEBUG )           //outputs min and max pixel, along with  number of columns tested
						{
							theStdInterface.printf("\r\n PixMax %d PixMin %d Ending Column %d \r\n\r\n", uiPixMax, uiPixMin, uiColumn);      //lmc_debug
						}
					}//row loop

					if ( bIsOk )
					{	// Check for a true grayscale (in case we got all black pixels).  We should have pixels at every level within a span of values.						
						for (x = (uiPixMin); x < (uiPixMax); x++)       
						{
							//Has every pixel value been found in the range tested
							if ( ! histogram[x] )
							{
								theStdInterface.printf("Missing pixel values in grayscale image (no value of %d\r\n", x);
								bIsOk = false;
							}

							//Is the histogram an even distribution of pixel values in the range tested
							if( ((histogram[x] % uiColumn) != 0) && (GET_SETTING(GeneralDebugOn)) )    
							{
								theStdInterface.printf("Histogram value not equal to the rest (wrong value at %d, Value is %ld )\r\n", x, histogram[x]);   //lmc_debug
								bIsOk = false;
							}
						}
					}
				}//end if (P_SCANBUFFER)
			}//end if ( bIsOk ), was able to get an image
			else		//else (bIsOk == false), not able to get an image
			{
				theStdInterface.printf("\r\n No image to test ");
			}

			// Place image sensor back to non test mode
			waitDelay.Reset();

			for(i = 0; i < count; i++)
			{
				SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_UNSET_IMAGER_GRAYSCALE, NULL, 0, (BYTE *)&status, sizeof status, NULL);

				if( status == SDHW_ERR_NONE )
				{
					break;
				}
				else
				{
//					RESET_WATCHDOG_TIMER();

					if( i < (count - 1) )
					{
						while( ! waitDelay.HasExpired() )   MT_Delay(5);

						waitDelayTime = (ULONG) (WAIT_DELAY_TIME * (i + 1));			//Adjust backoff as # of retries increases
						waitDelay.SetExpiration( waitDelayTime );
					}

					if( (GET_SETTING(GeneralDebugOn)) || RE_SEND_DEBUG  )           //outputs number of retries to send command (I2C error)
					{
						theStdInterface.printf("\r\n Number of Retries to shut down Grayscale Mode: %d Time: %d \r\n\r\n", (i+1),  (WAIT_DELAY_TIME*(i + 1)) );      //lmc_debug
					}
				}
			}

			if( status != SDHW_ERR_NONE  )   //Could not turn off imager gray scale mode
			{         								// IOCTL failed--check status byte.
				if ( status == SDHW_ERR_NOT_IMPLEMENTED ) theStdInterface.printf("Grayscale test not implemented");
				else theStdInterface.printf("Problem restoring device to normal operation.  You may need to reset (turn power off/on) this device.");
				bIsOk = false;
			}
		}
		else	//Could not set imager gray scale mode
		{		// IOCTL failed--check status byte.
			if ( status == SDHW_ERR_NOT_IMPLEMENTED ) theStdInterface.printf("Grayscale test not implemented");
			else theStdInterface.printf("Unable to set imager to grayscale mode (possible comm failure)");
			bIsOk = false;
		}
	}	//END Normal test flex test mode

	// Restore the trigger mode and scan driver time out.
	TrgMod = "0E0100!";
	char TrgModChar = OrigTriggerMode + '0';
	TrgMod.Insert(&TrgModChar, 1, 6);

	ScanDrivTimOut = "024A00";
	ScanDrivTimOut.Append(OrigScanDriverTimeOutString);
	ScanDrivTimOut.Append("!");

	ExpMax = "040203";
	ExpMax.Append(OrigExposureMaxString);
	ExpMax.Append("!");

	PowerLevel = "040816";
	PowerLevel.Append(OrigPowerLevelString);
	PowerLevel.Append("!");

	if( (iTestMode <= 2) )              //if test mode is 0, 1, or 2, set trigger mode
	{
		//start scanning if required
		if (bRestartScanning) SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_START_SCANNING,NULL,0,NULL,0,NULL);

		if ( ! ReaderConfiguration->Menu(&TrgMod) )
		{
			theStdInterface.printf("Unable to put device in original trigger mode.");
			return false;
		}

		if ( ! ReaderConfiguration->Menu(&ScanDrivTimOut) )
		{
			theStdInterface.printf("Unable to set original scan driver time out.");
			return false;
		}

		if ( ! ReaderConfiguration->Menu(&ExpMax) )
		{
			theStdInterface.printf("Unable to set original scan driver exposure max.");
			return false;
		}

		if ( ! ReaderConfiguration->Menu(&PowerLevel) )
		{
			theStdInterface.printf("Unable to set original scan driver power level.");
			return false;
		}

		pTrigger->SetBlockDecoderFromExecuting(FALSE);
		pTrigger->UpdateTriggerStatus();
	}

	return bIsOk;
}


//This is the menu call for test flex command
bool TestFlex( void )
{
	return TestFlexCommand(0);
}
//End test Flex routines *************************************************************************************************

#endif	//#ifdef	ENABLE_FLEX_TESTS



#ifdef	ENABLE_CURRENT_TESTS

//sends out interface one linear 1D image of the size passed
void SendImageData_Linear( unsigned long sizeImage )
{
	int i;
	char MsgBuf[8];

	if( P_SCANBUFFER != NULL )
	{
		printf("\r\n Linear image data: \r\n");

		for(i = 0; i < sizeImage; i++)
		{
			sprintf((char *)MsgBuf,"%04d\r\n",(((int)P_SCANBUFFER[i])));
			xputstring(MsgBuf);
			wait_til_tx_is_empty();
		}
	}
	return;
}


#define UPC_SIZE	330
#define WHITE_ZONE_SIZE	100
#define START_LOCATION	1000

//Place an image of a UPC barcode in the current P_SCANBUFFER, for test
bool PlaceLinearImageInBuffer( DWORD dwHeight, DWORD dwWidth )
{
	bool bCanDo = false;

	UCHAR WhiteZone = 102;    //white value to place before and after barcode

	//Note: size is stated in UPC_SIZE defined above, size is currently 330 characters
	UCHAR UpcBarcode[] =	{105,107,108,104,104,105,105,105,104,104,104,103,104,103,102,101,101,101,101,105,103,104,
	                      102,103,104,105,103,104,104,104,104,104,104,105,103,103,103,104,105,103,104,104,103,105,
	                      104,103,103,103,106,106,106,106,109,115,124,138,148,152,150,148,149,151,153,147,136,126,
	                      120,117,114,118,128,140,154,167,169,168,163,155,152,155,158,153,143,133,126,128,135,150,
	                      161,169,168,163,149,136,128,128,133,144,151,150,144,135,127,127,134,142,149,146,138,130,
	                      126,131,138,151,163,170,169,165,157,156,159,167,176,183,186,187,188,188,183,175,164,156,
	                      153,156,159,158,155,154,155,158,154,143,129,120,113,112,115,123,136,154,166,172,172,166,
	                      156,152,154,163,170,172,168,158,144,131,120,115,111,116,124,136,148,155,152,153,154,160,
	                      159,155,149,152,155,158,157,152,149,153,154,153,147,147,151,155,153,142,130,120,111,107,
	                      104,104,104,113,125,140,146,143,135,124,116,111,111,119,130,143,150,149,139,128,122,127,
	                      136,143,146,143,132,123,122,128,136,144,143,135,124,113,110,107,114,126,142,158,170,179,
	                      181,182,178,168,158,152,154,158,154,146,132,122,126,137,150,164,175,182,184,183,176,163,
	                      146,131,123,126,136,148,153,152,146,149,151,151,148,136,126,119,124,140,154,170,178,180,
	                      182,179,168,158,150,150,153,156,152,148,148,152,152,144,132,120,109,104,100,101,100,97,
	                      97,97,95,99,98,97,97,98,98,97,96,95,96,97,96,96,96,97,98,95,98,99};

	//check to see if image is linear and big enough
	if( (1 == dwHeight) && ((START_LOCATION+WHITE_ZONE_SIZE+UPC_SIZE+WHITE_ZONE_SIZE) < dwWidth) )
	{
		int x;
		//place test pattern into P_SCANBUFFER
		for( x=(START_LOCATION); (x < (START_LOCATION+WHITE_ZONE_SIZE+UPC_SIZE+WHITE_ZONE_SIZE)); x++)
		{
			if( (x < (START_LOCATION + WHITE_ZONE_SIZE)) || (x >= (START_LOCATION+WHITE_ZONE_SIZE+UPC_SIZE)) )        //place white zone
			{
				P_SCANBUFFER[x] = WhiteZone;
			}
			else if( (x >= (START_LOCATION+WHITE_ZONE_SIZE)) && (x < (START_LOCATION+WHITE_ZONE_SIZE+UPC_SIZE)) )      //place upc barcode
			{
				P_SCANBUFFER[x] = UpcBarcode[ (x - (START_LOCATION+WHITE_ZONE_SIZE)) ];
			}
		}

		bCanDo = true;
	}

	return bCanDo;
}


//****************************************************************************************************************************
//TestCurrent()
//
//Allows for a timed test that will supply a steady current draw by the product.
//Lights and laser will be enabled in a periodic manner to help stabilize current draw.
//Image capture will be repeatedly done excercise DMA.
//Decoder will be called to exercise memory.
//
//Pass Parameter:	None
//
//Returns:	Zero for failure
//
//Options:
//				"TSTDLY" sets test duration in mSec.
//				"TSTILL" sets the LEDS and aimer modes.
//				"TSTEXP" sets the fixed exposure for 2D imagers.
//				"TSTCAP" if true, allows continuous image capture, else if false it only allows single frame capture (2D only).
//				"TSTDTY" sets the Illumination LED duty cycle for test.
//
//				Note: All settings above were carefully thought out and had temperature and current measurements performed.
//						Do not change them randomly without performing validation testing afterwards.
//
//****************************************************************************************************************************

bool TestCurrent( void )
{
	bool bReturnValue = true;
#ifdef FIXME

	CBarcodeData *pCurrentMsg;
	BOOL bRestartScanning = false;
	ULONG	testDelayTime = 0;
	char RxStatus = 0;

	ValuesSaved = false;       //global file scope used for image parameter saving
	testDelayTime = GET_SETTING( iTestDelayTime );

	//Make a delay timer
	CTimeoutmS	testDelay(testDelayTime);          //20 ms

	// Go into TRGMOD0 to stop any scanning that might be going on, and then set scan driver time out to lock scanning back on
	unsigned int OrigTriggerMode = pTrigger->GetTriggerMode();
	unsigned int OrigScanDriverTimeOut = GET_SETTING(ScanDriverTimeout);         //range is 0 to 999999
	char OrigScanDriverTimeOutString[] = {'\0','\0','\0','\0','\0','\0','\0','\0'};

	sprintf(OrigScanDriverTimeOutString,"%u",OrigScanDriverTimeOut);

	if ( OrigTriggerMode == TEMP_MANUAL_TRIG ) OrigTriggerMode = GET_SETTING(TriggerMode);

	HSTRING TrgMod("0E01000!", true);                    //manual trigger mode
	HSTRING ScanDrivTimOut("024A001!", true);            //stop scanning

	if ( ! ReaderConfiguration->Menu(&TrgMod) )
	{
		theStdInterface.printf("Unable to put device in manual trigger mode.");
		bReturnValue = false;
	}

	if ( ! ReaderConfiguration->Menu(&ScanDrivTimOut) )
	{
		theStdInterface.printf("Unable to set scan driver time out.");
		bReturnValue = false;
	}

	if( bReturnValue )
	{
		pTrigger->UpdateTriggerStatus();
		BlockDecoderFromExecuting = TRUE;     //set decode blocking right after update trigger status

		// if STOP_SCANNING returns non-zero it means that imaging was running, and needed to be shutdown, and will
		// therefore need to be restarted
		bRestartScanning = SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_STOP_SCANNING,NULL,0,NULL,0,NULL);

		if(GET_CONFIG_SETTING(iOpticsPopulation)==TWO_D) //Are we 2D??
		{
			bool bImageGrabStyleContinuous = GET_SETTING( iTestCurrentAllowContinuousImageGrab );
			BOOL bImageAvailable = false;

			ExpoValue = GET_SETTING( iTestCurrentMaximumExposure );   //set value before calling SetOrRestoreImagerRegisterValues()
			GainValue = 1;

			//set gray scale mode
			TestFlexCommand(4);

			//set the state of the lights for test
			SetTestLights( GET_SETTING(iTestCurrentIlluminationState) );

			//set up new imager values
			SetOrRestoreImagerRegisterValues(1);

			if( bImageGrabStyleContinuous )		//if continuous image capture is allowed, then start scanning
			{
				SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_START_SCANNING,NULL,0,NULL,0,NULL);
			}

			testDelay.Reset();

			do
			{
				if( bImageGrabStyleContinuous )   //continuous image capture
				{
					bImageAvailable = false;

					bImageAvailable = SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_NEW_IMAGE_AVAILABLE,NULL,0,NULL,0,NULL);

					if( bImageAvailable )
					{
						bImageAvailable = SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_NEW_SCAN,(UCHAR *) &P_SCANBUFFER,0,NULL,0,NULL);
					}
				}
				else		//single frame image capture
				{
					bImageAvailable = true;

					//capture a single frame
					SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SINGLE_FRAME_CAPTURE,(UCHAR *) &P_SCANBUFFER,0,NULL,0,NULL);
				}

				if( (P_SCANBUFFER != NULL) && bImageAvailable )
				{
					//try decoding gray scale
					decode ((PIXEL *) P_SCANBUFFER);
				}

				if( DecodeResults->AnyData() )
				{
					pCurrentMsg = DecodeResults->Read();   // empty q and free cbarcode objects
					pCurrentMsg->Free();
				}

				if( 0 == RxStatus )           //check for an ESC in RX data to end test early
				{
					RxStatus = get_peek();
					//theStdInterface.printf("%c",RxStatus);
					if( ESC == RxStatus )
					{
						get_char();        //remove ESC from rx fifo
					}
				}
			}
			while( (!testDelay.HasExpired()) && (ESC != RxStatus) );

			if( bImageGrabStyleContinuous )		//if continuous image capture is allowed, then stop scanning
			{
				SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_STOP_SCANNING,NULL,0,NULL,0,NULL);
			}

			//restore original imager values
			SetOrRestoreImagerRegisterValues(2);

			//lights off
			SetTestLights(0);

			//clear gray scale mode
			TestFlexCommand(3);

			bReturnValue = true;
		}
		else		//1D imager
		{
			DWORD dwHeight, dwWidth;

			ExpoValue = GET_SETTING( StartingExposure );
			GainValue = 1;             //fixed for linear

			SetTestLights( GET_SETTING(iTestCurrentIlluminationState) );            //set the state of the lights for test
			SetOrRestoreImagerRegisterValues(1);				//set up new imager values

			pTrigger->PrepareForScanning();	// do lights and start/stop scanning per trigger mode
			testDelay.Reset();

			do
			{
				if( 0 != pTrigger->GetNextScan(nOpenCntxt,&P_SCANBUFFER) )
				{
					// Get image height and width from scan driver.
					SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&dwHeight,4,NULL);
					SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&dwWidth,4,NULL);
					PlaceLinearImageInBuffer( dwHeight, dwWidth );

					decode ((PIXEL *) P_SCANBUFFER);

					if( DecodeResults->AnyData() )
					{
						pCurrentMsg = DecodeResults->Read();   // empty q and free cbarcode objects
						pCurrentMsg->Free();
					}
				}

				if( 0 == RxStatus )           //check for an ESC in RX data to end test early
				{
					RxStatus = get_peek();
					//theStdInterface.printf("%c",RxStatus);
					if( ESC == RxStatus )
					{
						get_char();        //remove ESC from rx fifo
					}
				}
			}
			while( (!testDelay.HasExpired()) && (ESC != RxStatus) );

			pTrigger->FinishScanning();	// do lights and start/stop scanning per trigger mode
			SetOrRestoreImagerRegisterValues(2);				//restore original imager values
			SetTestLights(0);            //lights off

			if( (GET_SETTING(GeneralDebugOn)) || RE_SEND_DEBUG  )           //outputs size of image
			{
				printf("\r\n Linear image width: %lu height: %lu \r\n\r\n", dwWidth, dwHeight);      //lmc_debug
				SendImageData_Linear( dwWidth );
			}

			bReturnValue = true;
		}

		//start scanning if required
		if (bRestartScanning) SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_START_SCANNING,NULL,0,NULL,0,NULL);
	}

	// Restore the trigger mode and scan driver time out.
	TrgMod = "0E0100!";
	char TrgModChar = OrigTriggerMode + '0';
	TrgMod.Insert(&TrgModChar, 1, 6);

	ScanDrivTimOut = "024A00";
	ScanDrivTimOut.Append(OrigScanDriverTimeOutString);
	ScanDrivTimOut.Append("!");

	if ( ! ReaderConfiguration->Menu(&TrgMod) )
	{
		theStdInterface.printf("Unable to put device in original trigger mode.");
		bReturnValue = false;
	}

	if ( ! ReaderConfiguration->Menu(&ScanDrivTimOut) )
	{
		theStdInterface.printf("Unable to set original scan driver time out.");
		bReturnValue = false;
	}

	BlockDecoderFromExecuting = FALSE;
	pTrigger->UpdateTriggerStatus();
#endif

	return bReturnValue;
}
//End Test Current routines *************************************************************************************************

#endif //ifdef	ENABLE_CURRENT_TESTS






//AllignmentFlagSet()
//Sets the allignment mode flag by doing a DR stand allignment
//Passes nothing, returns bool for status (true).
//
bool READER_CONFIGURATION::AllignmentFlagSet( void )
{

	return false;
}


#ifdef	ENABLE_DR_ALLIGNMENT

//**********************************************************************************************
#define SIZE_OF_SER_NUM	64
#define	NUMBER_OF_TRIGGER_TIMEOUTS_BEFORE_SHUTDOWN			120
//
//	DiagnosticDocReaderAllignment
//		Enter an alignment mode for the 4800dr to allign pod to document reader stand.
//		Checks for correct S.N. and product type before enabling allignment mode.
//		Currently only supported on 4800dr.
//
bool DiagnosticDocReaderAllignment( void )
{
	char 	cSerNumDevice[SIZE_OF_SER_NUM+1];         //extraneous data storage - Digital board serial number
	char 	cSerNumEngine[SIZE_OF_SER_NUM+1];         //extraneous data storage - image engine serial number
	char	*ExtraneousDataPointer;
	int	ExtraneousDataSize;

	char 	cUSBSerNum[SIZE_OF_SER_NUM+1];            //USB serial number - the digital board serial number
	char	*USBSerNumDataPointer;
	int	USBSerNumDataSize;

	char 	cEngSerNum[SIZE_OF_SER_NUM+1];            //image engine serial number
	char	*EngSerNumDataPointer;
	int	EngSerNumDataSize;

	int i;
	bool bStatus = false;
	bool bResetTriggerModeAndUpdate = false;

	static int	iMaxTimeOutCount = 0;

	//Get extraneous data attached to command, data format is: 4800dr serial number, then a space for a delimiter, then the engine serial number
	ExtraneousDataPointer = (char *) ReaderConfiguration->ExtraneousTextAfterCommand->Char;
	ExtraneousDataSize = (int) ReaderConfiguration->ExtraneousTextAfterCommand->Size;

	//Get the USB serial number from digital baord
	HSTRING		USBSerialNumber( GET_SETTING( USBSerNum ), true );           //The USB serial number - old way, please leave as example if needed
	USBSerialNumber.UniCodeToSimpleAsciiFormat();                           //convert from unicode
	USBSerNumDataPointer = (char *) USBSerialNumber.Char;                   //set local pointer to string
	USBSerNumDataSize = (int) USBSerialNumber.Size;                         //set local variable to string size

	// If scan engine has PSOC , then get the serial number in the engine PSOC.
	PSoC_5100_REGISTER_TABLE	Registers;
	HSTRING		hsImagerSerialNumber( 20, true );                                 //The imager serial number holder
	if( ImageEngineHasPsoc() )
	{
		if( Psoc5100GetRegisters( &Registers ) )
		{
			hsImagerSerialNumber.Copy( Registers.HandHeldProductSerialNumber, sizeof( Registers.HandHeldProductSerialNumber ) );
			EngSerNumDataPointer = (char *) hsImagerSerialNumber.Char;                   //set local pointer to string
			EngSerNumDataSize = (int) hsImagerSerialNumber.Size;                         //set local variable to string size
			bStatus = true;
		}
	}

	//make local copy of extraneous data and parse out the two serial numbers
	if( (ExtraneousDataSize <= SIZE_OF_SER_NUM) && (ExtraneousDataSize > 0) && bStatus )
	{
		int iDeviceCount = 0;
		int iEngineCount = 0;
		bool bFoundSpace = false;
		bool bValidData = true;

		bStatus = false;

		for( i = 0; i < ExtraneousDataSize; i++ )
		{
			if( (*ExtraneousDataPointer == 0x20) && !bFoundSpace )            //found the "space" delimiter
			{
				cSerNumDevice[i] = '\0';
				iDeviceCount = i;
				bFoundSpace = true;
				//theStdInterface.printf("\r\n");       																//lmc-debug
			}
			else if( (*ExtraneousDataPointer != 0x20) && !bFoundSpace )         //device serial number parsed first
			{
				cSerNumDevice[i] = *ExtraneousDataPointer;
				iDeviceCount = iDeviceCount + 1;
				//theStdInterface.printf("char = %c ", cSerNumDevice[i]);       									//lmc-debug

				if( (*ExtraneousDataPointer < 0x20) || (*ExtraneousDataPointer > 0x7A) )
				{
					cSerNumDevice[i] = '\0';
					bValidData = false;
				}
			}
			else if( (*ExtraneousDataPointer != 0x20) && bFoundSpace && (i > iDeviceCount) )          //engine serial number parsed second
			{
				cSerNumEngine[(i - (iDeviceCount + 1))] = *ExtraneousDataPointer;
				iEngineCount = iEngineCount + 1;
				//theStdInterface.printf( "char = %c ", cSerNumEngine[(i - (iDeviceCount + 1))] );       	//lmc-debug

				if( (*ExtraneousDataPointer < 0x20) || (*ExtraneousDataPointer > 0x7A) )
				{
					cSerNumEngine[(i - (iDeviceCount + 1))] = '\0';
					bValidData = false;
				}
			}
			else             //found another "space" delimiter - error
			{
				bFoundSpace = false;
				bValidData = false;
				iDeviceCount = 0;
				iEngineCount = 0;
				break;
			}

			ExtraneousDataPointer++;
		}

		if( bValidData && bFoundSpace && (iDeviceCount > 0) && (iEngineCount > 0) )      //do we have valid data?
		{
			if( ((i - (iDeviceCount + 1)) >= 0) && ((i - (iDeviceCount + 1)) <= SIZE_OF_SER_NUM) )   //valid storage location?
			{
				cSerNumEngine[(i - (iDeviceCount + 1))] = '\0';
				bStatus = true;
			}
		}
		//theStdInterface.printf("\r\n valid %d \r\n",bStatus);         											//lmc-debug
	}
	else
	{
		bStatus = false;
	}

	//make copys of the two serial numbers in product
	if( bStatus )
	{
		bStatus = false;

		//make local copy of usb serial number taken from digital board
		if( USBSerNumDataSize <= SIZE_OF_SER_NUM )
		{
			for( i = 0; i < USBSerNumDataSize; i++ )
			{
				cUSBSerNum[i] = *USBSerNumDataPointer;
				USBSerNumDataPointer++;
				//theStdInterface.printf("char = %c ", cUSBSerNum[i]);       									//lmc-debug
			}

			cUSBSerNum[i] = '\0';
			bStatus = true;
		}

		bStatus = false;	//not currently concerned with the USB serial number , future thing
		//theStdInterface.printf("\r\n");       																			//lmc-debug

		//make local copy of image engine serial number
		if( EngSerNumDataSize <= SIZE_OF_SER_NUM )
		{
			for( i = 0; i < EngSerNumDataSize; i++ )
			{
				cEngSerNum[i] = *EngSerNumDataPointer;
				EngSerNumDataPointer++;
				//theStdInterface.printf("char = %c ", cEngSerNum[i]);       										//lmc-debug
			}

			cEngSerNum[i] = '\0';
			bStatus = true;
		}

		//theStdInterface.printf("\r\n valid %d \r\n",bStatus);         												//lmc-debug
	}

	if( pTrigger->CheckIfAllignmentModeAllowed() )
	{
		SkipIndicatorsForThisMenuCommand();

		// If we found extraneous data, compare contents to serial number from product, and run allignment if we have a match.
		if( bStatus )
		{
			if( (strcmp(cSerNumEngine, cEngSerNum)) == 0 )	//got a S.N. match
			{
				if( pTrigger->bAllignmentModeEnabled == false )    //if we read allignment menu barcode for the first time
				{
					//ReaderConfiguration->AllignmentFlagSet();            //first, set the image block allignment done flag
					GoodMenuBeep();
					iMaxTimeOutCount = 0;
					pTrigger->SetMaxAllignmentModeTriggerTimeout();
					//theStdInterface.printf(" Match: %s to %s \r\n", cSerNumEngine, cEngSerNum );           //lmc-debug
				}
				else
				{
					iMaxTimeOutCount++;
				}

				if( (iMaxTimeOutCount < NUMBER_OF_TRIGGER_TIMEOUTS_BEFORE_SHUTDOWN) && (!pTrigger->HasMaxAllignmentModeTriggerTimeoutExpired()) )
				{
					pTrigger->bAllignmentModeEnabled = true;
					pTrigger->TriggerAllignmentMode();
				}
				else         //we are done, so lets turn this mode off
				{
					iMaxTimeOutCount = 0;
					pTrigger->bAllignmentModeEnabled = false;
					pTrigger->UnTriggerAllignmentMode();
					bResetTriggerModeAndUpdate = true;
					ReaderConfiguration->AllignmentFlagSet();            //last, set the image block allignment done flag
					//theStdInterface.printf( " TimeOut: \r\n" );           //lmc-debug
				}
			}
			else       //error, S.N. does not match
			{
				bStatus = false;
				//theStdInterface.printf(" No Match: %s to %s \r\n", cSerNumEngine, cUSBSerNum );         //lmc-debug
			}
		}
		/*else         //error, bStatus is false, No S.N. found in menu command
	{
			theStdInterface.printf(" No SN: \r\n" );         //lmc-debug
	}*/

		if( !bStatus )       //if error
		{
			ErrorMenuBeep();
			iMaxTimeOutCount = 0;
			pTrigger->bAllignmentModeEnabled = false;
			pTrigger->UnTriggerAllignmentMode();
			bResetTriggerModeAndUpdate = true;
		}

		if( bResetTriggerModeAndUpdate )
		{
			pTrigger->SetTriggerMode();
			pTrigger->UpdateTriggerStatus();
		}
	}
	else      //if mode not allowed
	{
		bStatus = false;
	}

	return bStatus;
}
//DiagnosticDocReaderAllignment - the end********************************************************************************
#endif	//#ifdef	ENABLE_DR_ALLIGNMENT
#endif	//#ifndef RFBASE


void READER_CONFIGURATION::CreateDiagnosticAndDebugMenuSettings( int /* ControlMask */ )
{
#if defined( DEVELOPMENT_SETTINGS )
	AddCommand("030100", SUB_TAG_TABLE::VISIBLE_BY_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, TestAborts, NULL);

#endif

	AddNumber("030101", NULL | LOCAL_SETTING, NULL, NULL, &AutoTriggergreyband, 16,16,0,255); //1D auto trigger remove gen6 ?? FIXME
	AddCommand("030102", SUB_TAG_TABLE::VISIBLE_BY_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &StuffRxBuffFromTheMenu, NULL);



#ifndef RFBASE
#ifdef	ENABLE_FLEX_TESTS

	AddCommand("030103", SUB_TAG_TABLE::VISIBLE_BY_TAG | LOCAL_SETTING , &TestFlex, NULL);
#endif
#endif

	AddNumber("030104", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, NULL, &iTestFlexTestMode, 0,0,0,2);


#ifndef RFBASE

	AddNumber("030200", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, NULL, &CsiTestpatternType,0,0,0,3);
	AddNumber("030201", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, NULL, &CsiRxLevel,1,1,1,7);
	AddNumber("030300", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, NULL, &csidmaincomplete,0,0,0,0x7fffffff);
	AddCommand("030202", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &ShowInvalidFrames,NULL);
	//AddNumber("CSIERF", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, ShowInvalidFrames,NULL, &csiBadframes,0,0,0,0x7fffffff);
#ifdef	ENABLE_CURRENT_TESTS

	AddCommand("030105", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING , &TestCurrent, NULL);
#endif
#endif

	AddNumber("030106", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, NULL, &iTestCurrentIlluminationState,5,5,0,7);
	AddNumber("030107",SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING ,NULL,NULL,&iTestCurrentMaximumExposure,100,100,1,7874);
	AddNumber("030108", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, NULL, &iTestDelayTime,5000,5000,0,6000);
	AddBoolean("030109", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, NULL, &iTestCurrentAllowContinuousImageGrab, true, true);
	AddNumber("03010A", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, NULL, &iTestCurrentLedDutyCycle,50,50,0,100);

#if defined( DEVELOPMENT_SETTINGS ) || defined( MANUFACTURING_IMAGER_TEST )

	AddNumber("030400",SUB_TAG_TABLE::VISIBLE_BY_TAG | LOCAL_SETTING, NULL, NULL, &tscattempts, 10,10,1,100);
	AddBoolean("030401", SUB_TAG_TABLE::VISIBLE_BY_TAG | LOCAL_SETTING, NULL, NULL, &tscenable, false, false);
#endif


	AddBoolean("030500", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, NULL, &EMITestReRead, false, false);

	AddNumber("030600", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | SUB_TAG_TABLE::FIXED_EXCEPT_ACTIVE | LOCAL_SETTING, NULL, NULL, &DebugButton,0,0,0,100);

	// Matrix Debug Settings
#ifndef WINSIM	// Barry you can remove this later when DefaultFlashImageStartAddress is enabled (Dieter)

	AddBoolean("030700", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::FIXED_EXCEPT_ACTIVE, NULL, NULL, &FlashingEnabled, true, true);
	AddNumber("030701", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::FIXED_EXCEPT_ACTIVE, NULL, NULL,
	          &DefaultFlashImageStartAddress, FLASH_MANUFACTURING_CONFIGURATION_BASE + FLASH_MAX_MANUFACTURING_CONFIGURATION_LENGTH,
	          FLASH_MANUFACTURING_CONFIGURATION_BASE + FLASH_MAX_MANUFACTURING_CONFIGURATION_LENGTH, 0, FLASH_SIZE );
#endif
#if 0 //FIXME

	FlashType = determine_flash_type();
	AddNumber("FLSTYP", SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG, NULL, NULL, &FlashType, 0, FlashType, FlashType, 0x7FFFFFFF );
#endif

	AddCommand("030601", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING , &DbgEcho, NULL);
	AddBoolean("030602", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING , NULL, NULL, &GeneralDebugOn, false, false);


	AddBoolean("030603", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, SetFlashDebugMode, &FlashDebugOn, false, false);
	AddBoolean("030604", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, NULL, &TurnOffInterruptsDuringApplicationWrite,
	           false, false);

	AddBoolean("030605", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING , NULL, NULL, &ImagerDebugOn, false, false);

	AddBoolean("030606", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, NULL, &MenuingDebugMode, false, false);

	AddCommand( "030800", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &ReportStartupTiming, NULL);
	AddBoolean( "030801", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, NULL, &ReportAllHTAGParameters, false, false);

	AddBoolean 	("030607", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, NULL, &DebugStartupTimeReporting, false, false);

	AddBoolean 	("030608", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, NULL, &DebugTimeToReadReporting, false, false);
	AddBoolean 	("030609", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, NULL, &DebugSignatureCapture, false, false);

	AddBoolean 	("03060A", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, NULL, &DebugImageTimeReporting, false, false);
	AddBoolean 	("03060B", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, NULL, &DebugSleepReporting, false, false);

	AddNumber("03060C", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, NULL, &DebugScanDriver,0,0,0,255);



	AddNumericList("03060D", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::FIXED_EXCEPT_ACTIVE, NULL, NULL,
	               &DebugCommandProccessingTraceMode, TRACE_MODE_OFF, TRACE_MODE_OFF, TRACE_MODE_OFF, TRACE_MESSAGE_MODE_ON );
	AddNumber("03060E", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, NULL,
	          &MainLoopDebugReportingCycleFrequency, 0, 0, 0, 0x7FFFFFFF );
	AddCommand("03060F", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &DBG2IF, NULL);

	AddCommand("030900", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &FlashDiskDirectory, NULL);
	AddCommand("030901", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &FlashDiskDeleteFile, NULL);
	AddCommand("030902", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &FlashDiskReadFile, NULL);
	AddCommand("030903", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &FlashDiskRenameFile, NULL);
	AddCommand("030904", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &FlashDiskCopyFile, NULL);
	AddCommand("030905", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &FlashDiskInformationDump, NULL);
	//AddCommand("DSKCWD", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &FlashDiskChangeDir, NULL);
	AddCommand("030906", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &FlashDiskFormatVolume, NULL);


	// Diagnostics
	AddCommand("030A00", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &MallocTest, NULL);
	AddCommand("030A01", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &ShowStackUsage, NULL);
	AddCommand("030A02", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &DumpMemoryConfiguration, NULL);
	AddCommand("030A03", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &ShowFreeHeapSpace, NULL);
	AddCommand("030A04", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &ShowFreeHeapSpaceShortVersion, NULL);
	AddCommand("030A05", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &ShowUntouchedHeapSpace, NULL);
	AddCommand("030A06", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &DumpMemoryBlock, NULL);
	AddCommand("030A07", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &DumpOneByte, NULL);
	AddCommand("030A08", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &DumpTwoBytes, NULL);
	AddCommand("030A09", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &DumpFourBytes, NULL);
	AddCommand("030A0A", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &SetOneByte, NULL);
	AddCommand("030A0B", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &SetTwoBytes, NULL);
	AddCommand("030A0C", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &SetFourBytes, NULL);
	AddCommand("030A0D", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &DiagnosticReset, NULL);
	AddCommand("030B00", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &DelayedReset, NULL);
	AddCommand("030A0E", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &DiagnosticChecksum, NULL);
	AddCommand("030A0F", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &DiagnosticCRC, NULL);
	AddCommand("030A10", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &DiagnosticOutputString, NULL);
	AddCommand("030A11", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &DbgTrigger, NULL);
	AddCommand("030A12", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &Readspiflash, NULL);

#ifndef RFBASE
#ifdef	ENABLE_DR_ALLIGNMENT

	AddCommand("030A13", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &DiagnosticDocReaderAllignment, NULL);
#endif
#endif

//	AddBoolean("ALNBIE", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING, NULL, NULL, &BlockImagingEnabledWhenNotAlligned, false, false);
//	AddBoolean("ALNFLG", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | SUB_TAG_TABLE::DEFAULTING_DISABLED | LOCAL_SETTING, NULL, NULL, &AllignmentCompletedFlag, false, false);
#ifndef RFBASE
#ifdef	ENABLE_DR_ALLIGNMENT

	AddCommand("030C00", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING, &AllignmentFlagReset, NULL);
#endif
#endif

#ifdef DEVELOPMENT_SETTINGS

	AddCommand("030A14", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &Divedezederror, NULL);
#endif

#ifdef WDOGDEBUG

	AddCommand("030A15", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &CauseWatchDogReset, NULL);
#endif

	AddCommand("030D00", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &DiagnosticReset, NULL); // another alias for RF

	AddCommand("030E00", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &DoBarcodePoolReset, NULL); // free all cbarcode objects debug

	// thread information
	AddCommand("030F00", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &Doprint_thread_info, NULL); // another alias for RF

	// Heap diagnostics (HEPFRE is equivalent to DIAHEP).
	AddCommand("031000", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &ShowFreeHeapSpace, NULL);

#ifdef	HEAP_DEBUG

	AddCommand("031001", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND,
	           &ShowUsedHeapSpaceWithOptionalSorting, NULL);
	AddCommand("031002", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &ClearUsedHeapSpaceTrackingBuffer, NULL);
#endif

	AddASCIIString(	"030A16", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, NULL, NULL, &DiagnosticString, "", "" );

#ifdef READER_CONFIGURATION_STORAGE_DEBUG

	AddCommand("030A17", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &DisplayReaderConfigurationDuplicateActiveValues, NULL);
#endif

#if defined(RFSCAN) || defined(RFBASE)

	AddCommand( "030A18", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND,
	            &SendTestFile, NULL);
#endif

	AddCommand( "030A19", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &DumpI2CRegisters, NULL);

#define USE_TAGS SUB_TAG_TABLE::VISIBLE_BY_TAG | SUB_TAG_TABLE::DEFAULTING_DISABLED | LOCAL_SETTING

	AddBoolean("031100", USE_TAGS, NULL, NULL, &UsageStatistics.UsageStatsStorageEnabled, false, false);
	AddCommand("031101", USE_TAGS, &DumpUsageInfo, NULL);
	AddCommand("031102", USE_TAGS, &ResetUsageInfo, NULL);
	// Some accumulations are done in micro seconds so we can't go more than an hour, see "usage_statistics.h"
	AddNumber ("031103", USE_TAGS, NULL, NULL, &UsageStatistics.SaveInterval,	15, 15, 1, 60);
	AddString ("031104", USE_TAGS, NULL, NULL, UsageStatistics.Text, SIZE_OF_USE_TEXT, "None", "None");
	//	AddCommand("CT_DFT", USE_TAGS, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);
	//	AddCommand("TM_DFT", USE_TAGS, NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);

	AddNumber ("031200", USE_TAGS, NULL, NULL, &UsageStatistics.PowerOnCycles,	 		0, 0, 0, 0x7fffffff);
	AddNumber ("031300", USE_TAGS, NULL, NULL, &UsageStatistics.PowerOnTime,			0, 0, 0, 0x7fffffff);
	AddNumber ("031201", USE_TAGS, NULL, NULL, &UsageStatistics.PowerDownCycles,	 	0, 0, 0, 0x7fffffff);
	AddNumber ("031301", USE_TAGS, NULL, NULL, &UsageStatistics.DozeTime,				0, 0, 0, 0x7fffffff);
	AddNumber ("031302", USE_TAGS, NULL, NULL, &UsageStatistics.DozeLowTime,			0, 0, 0, 0x7fffffff);
	AddNumber ("031202", USE_TAGS, NULL, NULL, &UsageStatistics.StopCycles,		 		0, 0, 0, 0x7fffffff);
	AddNumber ("031303", USE_TAGS, NULL, NULL, &UsageStatistics.StopTime,				0, 0, 0, 0x7fffffff);

	AddNumber ("031203", USE_TAGS, NULL, NULL, &UsageStatistics.HardwareTriggers, 	0, 0, 0, 0x7fffffff);
	AddNumber ("031204", USE_TAGS, NULL, NULL, &UsageStatistics.SerialTriggers,		0, 0, 0, 0x7fffffff);
	AddNumber ("031304", USE_TAGS, NULL, NULL, &UsageStatistics.TriggerTime,			0, 0, 0, 0x7fffffff);
	AddNumber ("031205", USE_TAGS, NULL, NULL, &UsageStatistics.TriggerTimeouts,	 	0, 0, 0, 0x7fffffff);

	AddNumber ("031206", USE_TAGS, NULL, NULL, &UsageStatistics.NumberDecodes1D,	 	0, 0, 0, 0x7fffffff);
	AddNumber ("031207", USE_TAGS, NULL, NULL, &UsageStatistics.NumberDecodes2D,	 	0, 0, 0, 0x7fffffff);
	AddNumber ("031208", USE_TAGS, NULL, NULL, &UsageStatistics.NumberDecodesPDF,		0, 0, 0, 0x7fffffff);
	AddNumber ("031305", USE_TAGS, NULL, NULL, &UsageStatistics.DecodeTime1D,		 	0, 0, 0, 0x7fffffff);
	AddNumber ("031306", USE_TAGS, NULL, NULL, &UsageStatistics.DecodeTime2D,		 	0, 0, 0, 0x7fffffff);
	AddNumber ("031307", USE_TAGS, NULL, NULL, &UsageStatistics.DecodeTimePDF,			0, 0, 0, 0x7fffffff);

	AddNumber ("031209", USE_TAGS, NULL, NULL, &UsageStatistics.TimesAppFlashed, 		0, 0, 0, 0x7fffffff);
	AddNumber ("03120A", USE_TAGS, NULL, NULL, &UsageStatistics.TimesBootFlashed, 	0, 0, 0, 0x7fffffff);
	AddNumber ("03120B", USE_TAGS, NULL, NULL, &UsageStatistics.TimesWorkingFlashed, 0, 0, 0, 0x7fffffff);
	AddNumber ("03120C", USE_TAGS, NULL, NULL, &UsageStatistics.TimesDefaultFlashed, 0, 0, 0, 0x7fffffff);

	AddNumber ("031308", USE_TAGS, NULL, NULL, &UsageStatistics.IlluminationOnTime, 	0, 0, 0, 0x7fffffff);
	AddNumber ("031309", USE_TAGS, NULL, NULL, &UsageStatistics.AimOnTime,			 	0, 0, 0, 0x7fffffff);
	if ( GET_CONFIG_SETTING(m_TfbPop) ) AddNumber ("03130A", USE_TAGS, NULL, NULL, &UsageStatistics.m_TfbOnTime.m_TotalTime, 0, 0, 0, 0x7fffffff);

	AddNumber ("03120D", USE_TAGS, NULL, NULL, &UsageStatistics.CradleInsertions, 	0, 0, 0, 0x7fffffff);
	AddNumber ("03120E", USE_TAGS, NULL, NULL, &UsageStatistics.EsdHibernations,	 	0, 0, 0, 0x7fffffff);
	AddNumber ("03120F", USE_TAGS, NULL, NULL, &UsageStatistics.WatchDogResets,	 	0, 0, 0, 0x7fffffff);

	AddNumber ("031210", USE_TAGS, NULL, NULL, &UsageStatistics.NumberTimesStored,	0, 0, 0, 0x7fffffff);

#if !defined( RFBASE )

	AddCommand("03010B",SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND|SUB_TAG_TABLE::PASS_QUERY_CHARACTER_TO_ENTRY_EXIT_FUNCTIONS,NULL,&DBGIsUnitInStand);

#endif
#ifdef BATCH_DEBUG

	AddCommand("031400", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &CreateSequenceCodes, NULL); // another alias for RF
#endif
	AddNumber("031500", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING , NULL, NULL, &UserQueryAuthority, 0, 0, 0, 0xFFFF);

	return;
}



