/*===================================================================

 HIM 48X0G GENERATION 6 SOFTWARE

  Main Loop for boot code --

  TODO:  we have more coupling than we need to with the app decompressor--remove it.
  		 Along with this, revisit the large data structure in db_SharedTable.h and
  		 see how much of this we really need.
  		 
======================================================================================
	Revision information
======================================================================================
 $RCSfile: db_boot/boot_main.c $
 $Revision: 1.35 $
 $Date: 2011/04/18 15:23:54EDT $
======================================================================================

=====================================================================================*/
#if PLATFORM_CURRENT == imx25
#include "HardwareMacros.h"
#include "iMX25_GPIO.h"
#define CHECK_BOOT_PIN()	(READ_FIELD(GPIO4_PSR0,GPIO_BIT3_LSH,GPIO_BIT3_WID))
#define SAFE_FFS_USED 1
#else
#error No Platform Defined!
#endif //endof PLATFORM_CURRENT

#include "language.h"
#include "db_hdw.h"
#include "db_chrio.h"
#include "db_show.h"
#include "db_time.h"
#include "db_SYNCommandParse.h"
#include "boot_main.h"

#ifndef SAFE_FFS_USED
#include "strnicmp.h"
//#include "db_FlashDisk.h"
#include "string.h"
#include "FlashDisk.h"
#include "CheckSum.h"
#else
#include "num_m25pxx.h"
#include "mx25spi.h"
#include "flashdrv.h"
#include "MocfTags.h"
#include "TlvContainerParser.h"
#include "FlashApplicationCode.h"

int ReadApplicationCodeFromSerialFlash(BYTE **DestinationLocation);
#endif


/***** Public variables *****/
#if PLATFORM_CURRENT == imx25
extern "C" const char Boot_msg[] = "\r\nBullet Boot Code -- Version ";
#endif //endof PLATFORM_CURRENT

const char g_TimeStamp[] = __TIME__;
const char g_DateStamp[] = __DATE__;


// had to do this below primitively to trick the compiler to keep the pointer to a char string in ROM, so that
// boot code would always know where to find it... even after operational code was running and resident over its
// data RAM.  A volatile const * still is copied to and used from RAM... why, I have no idea>?!?!?!??!

//00.00 first provided to Joe/Joe/Matt on 3/4/02

//00.01 3/5/02 -->
//   rx queue, tx queue to 2048 each (down from 4k each)
//   removed DummyArray above
//   changed CMDLEN and SCRATCH_BUFFER_SIZE to 512 and 1024 respectively
//   in general effort to get back to the 16KB bb sectore in ROM (we were @ 22K)

//00.02 3/6/02
// Exception scheme implemented so CSI and other interrupt stuff could be started to work on
// The IRQ/FIQ model used is that of standard ARM, not using the AITC at this point... In other words, the C interrupt handler will have to
// handle a max of 64 independent interrupts per the same call (HUGE SWITCH and/or IF string)

//00.03 3/11/02
// General cleanup in db_boot.s -- removed array of ram tests which were making this file inpossible to follow
// 96MHz CSCR... the new plan is let's wait to turn on Async mode until we turn on the MMU/Cache.  At this point, that is all
// being tested in app code land, rather than boot... so for now just up the BCLK source to divide by 1 of USBPLL in boot.
// Cleanup throughout other files... most significantly got rid of extra delays in db_mem.s where it inits SDRAM.

//00.04 fixed fiq to actually work in the simplest sense. optimizations are following, but for now
// the fiq does a independent irq (even with all of the overhead of _irq keyword).

//00.05 EVERYTHINGs changed.  Boot code now supports the MMU dual map of 4kB @ 0x08000000 being mapped to 0 to make application code
//better at responding to interrupts.

//00.06 some general cleanup

// 00.07... moved bootminorversion and bootmajorversion to db_main.h... wrong place, but will clean up later
// ALL MMU translation stuff done in this version... bad news, this version had many versions!!

// 00.08 ... added some gpio init stuff for form factor boards that is thought to be ok for dev boards.
// db_start.s, db_hdw.c changed to get rid of data contention on

//00.09 ... corrected 8MB 32 bit single IC RAM setup for 8col 11rows.  Previously it seemed to be working fine with the default, 16MB setting
// of 8 x 12.


// Delay routine -- generic
// Note, early days of this functionality, there were a number of issues using it with a few optimization selections provided
// by the ARM tools.  Worst case, the ARM tools would simply reduce this function to nothing, in which case we had to put some
// useless instruction internal... ie it didn't like the null for loop.
void Delay(int dely)
{
	RESET_WATCHDOG_TIMER();
	set_time_limit(dely);
	while( !time_limit_expired() )
	{}
}


// Determine if user wants to stay in boot mode
//
// In development time, this was activated by trigger being depressed.  It was
// deemed unreasonable to leave this in for release as this would possibly be
// confusing/damaging to the user.
//
// As a result, 2 key states can generate boot code powering up and remaining
// resident (without copying and running application code, that is).
//
// These 2 states are:
//
// (1)  boot pin GPIO is GND
// (2)  ESC sequence (0x1b) has been received immediately on power up...
//
// both of these will abort normal processing and leave the system essentially
// "crippled" in boot mode.  From this state, the user will be able to do only
// very primitive tasks, like download new application code, request version,
// pull trigger and get a resulting message, etc (actually no need for etc...
// I think that's it.
//
// If the unit is coming out of sleep, these "outs" don't apply.  This is handled
// at the source... not here?!?!?!?!?!??!
//
// Exit: Return TRUE to direct code to stay in boot
// 		FALSE says do normal path stuff
// We need to receive at least 5 ESC characters in order to stay in boot

#define	NUMBER_OF_ESCAPES_NEEDED_TO_STAY_IN_BOOT		5
int stay_in_boot(void)
{
/*
	bool StayInBoot = false;
	int	NumberOfEscapesReceived = NUMBER_OF_ESCAPES_NEEDED_TO_STAY_IN_BOOT;
	int	charstorec= NUMBER_OF_ESCAPES_NEEDED_TO_STAY_IN_BOOT +2;
	poll_getchar_from_rx_fifo();  //;

	while ( (CHECK_BOOT_PIN()==FALSE ) || kbhit())
	{
		if ((get_char() == ESC) || (CHECK_BOOT_PIN()==FALSE ))
		{
			if (--NumberOfEscapesReceived <=0)
			{
				StayInBoot = true;
				break;
			}
		}
		else
		{
			NumberOfEscapesReceived = NUMBER_OF_ESCAPES_NEEDED_TO_STAY_IN_BOOT;
		}
		if (--charstorec<=0)
		{
			break;
		}
		Delay(25);
		poll_getchar_from_rx_fifo();  //;
	}
*/
	bool StayInBoot = false;
	bool BootPinStatus =true;
	int	NumberOfEscapesReceived = NUMBER_OF_ESCAPES_NEEDED_TO_STAY_IN_BOOT;
	set_time_limit(50);		// wait 100ms
	while (!time_limit_expired())
	{
		if (CHECK_BOOT_PIN()==TRUE)
		{
			BootPinStatus = false;
		}
		if (poll_getchar_from_rx_fifo())
		{
		}
		if (NumberOfEscapesReceived<=0)
		{
			StayInBoot=true;
			break;
		}				 
	}
	if (BootPinStatus)
	{
		StayInBoot=true;
	}
#ifdef RAMBASEDBOOT
	StayInBoot = true;	//RAM Based Boot can not load apps.
#endif //endof RAMBASEDBOOT

	return (StayInBoot);
}



//
//	FullPowerOnMessage
//		Display system, version, and RAM information.
//
void FullPowerOnMessage(void)
{
#if PLATFORM_CURRENT == imx25

#ifndef PROTO_HW
	//Turn on Output Enable, only needed for Corded, but will not affect other iMX25 boards
	//Pin has already been configured for GPIO but as an Input, GPIO4_20 = n232/KBD_OE1
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT20_LSH,GPIO_BIT20_WID,GPIO_GDIR_OUTPUT);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT20_LSH,GPIO_BIT20_WID,GPIO_LOW);

	//Force the Keyboard SW open to transmit
	//Pin has already been configured for GPIO, GPIO2_12 - KBD_SW_CNTRL
	HAZARD_WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT12_LSH,GPIO_BIT12_WID,GPIO_LOW);
	
#else
	//Turn on Output Enable, only needed for Corded, but will not affect other iMX25 boards
	//Pin has already been configured for GPIO, GPIO3_16 = n232/KBD_OE1
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT16_LSH,GPIO_BIT16_WID,GPIO_LOW);

	//Force the Keyboard SW open to transmit	
	//Pin has already been configured for GPIO, GPIO1_24 - KBD_SW_CNTRL
	HAZARD_WRITE_FIELD_TO_REG(GPIO1_DR0,GPIO_BIT24_LSH,GPIO_BIT24_WID,GPIO_LOW);
	
#endif //endof PROTO_HW
#endif //endof PLATFORM_CURRENT == mxl

#ifdef RAMBASEDBOOT
	xputstring ("\r\nRAMBASEDBOOT - Boot located in iRAM, Used for Testing Only\r\n");
#endif //endof RAMBASEDBOOT

	show_system();

	// xxxx **** debug messaging for now, being left in only because the 8MB is not working reliably
	// Once this is resolved this will be removed
	//xputstring ("Size of RAM = ");
	switch (SIZE_OF_SDRAM)
	{
	case 0:
		//xputstring ("None");
		break;
	case 1:
		//xputstring ("8MB");
		break;
	case 2:
		//xputstring ("16MB");
		break;
	case 4:
		//xputstring ("32MB");
		break;
	case 5:
		//xputstring ("128MB");
		break;
	case 6:
		//xputstring ("? - mem failure, correct size not found, less than 8MB");
		break;
	case 3:
	default:
		//xputstring ("? - mem error, correct size not found, let's say it's 8MB");
		break;
	}
	//xputstring ("\r\n");
	return;
} // FullPowerOnMessage



#ifndef SAFE_FFS_USED

#define	MAXIMUM_NUMBER_OF_FLASH_DISK_STATISTICS_ENTRIES	20
struct
{
	int							NumberOfFlashDisksFound;
	ENTRY_POINT_INFORMATION		*Location[MAXIMUM_NUMBER_OF_FLASH_DISK_STATISTICS_ENTRIES];
}
FlashDiskSearchData;


//
//	FindFlashDiskHeader
//		Find the flash disk header in the memory range sent by the caller, then back up to the location of the entry point information.
//
void *FindFlashDiskHeader(const void *StartOfAreaToSearchForHeader)
{
	char	*LocationOfEntryPointInformation = (char *)-1;
	char	*StartOfNextSearchLocation = ((char *) StartOfAreaToSearchForHeader);
	char	*EndOfAreaToSearchForHeader = ((char *) &FLASH_READ_BYTE_POINTER[FLASH_BASE + FLASH_SIZE]) - 16;
	// Adjust size of area to search down by 16 bytes
	//	(the size of "MATRIXDISKHEADER").
	// Find the header.
	while((StartOfNextSearchLocation <= EndOfAreaToSearchForHeader) && (((int) LocationOfEntryPointInformation) == -1))
	{
		if( strnicmp( StartOfNextSearchLocation, "MATRIXDISKHEADER", 16 ) == 0 )

		{
			LocationOfEntryPointInformation = StartOfNextSearchLocation - sizeof(ENTRY_POINT_INFORMATION);
		}
		else
		{
			RESET_WATCHDOG_TIMER();
			++StartOfNextSearchLocation;
		}
	}
	return(((void *) LocationOfEntryPointInformation));
} // FindFlashDiskHeader



//******************************************************************************************************
// Check to see if there is valid application code in flash by checking it's checksum.

int ChecksumValidForApplicationInFlash(ENTRY_POINT_INFORMATION *ApplicationCodeEntryPointInformation, BYTE *NewApplicationAddress)
{
	int			Status;							// Return status
	UINT32		Checksum;						// checksum as computed on flash application code
	Status = TRUE;		// No errors yet.

	// Compute checksum of flash code.
	Checksum = CalculateChecksum (NewApplicationAddress,
	                              ApplicationCodeEntryPointInformation->MainApplicationSize);

	// If the computed checksum of flash application code doesn't match the CRC stored in flash.
	if (Checksum != ApplicationCodeEntryPointInformation->MainApplicationChecksum)
	{
		Status = FALSE;
	}
	return(Status);
} // END ChecksumValidForApplicationInFlash



//******************************************************************************************************
// Check to see if there is valid application code in flash by checking it's CRC.

//int CRCValidForApplicationInFlash(ENTRY_POINT_INFORMATION *ApplicationCodeEntryPointInformation, BYTE *NewApplicationAddress)
//{
//	int		Status;						// Return status
//	UINT32	ComputedCRC;				// CRC as computed on flash application code
//
//
//
//	Status = TRUE;		// No errors yet.
//
//
//	// Compute CRC of flash code.
//	ComputedCRC = ((UINT32) (calc_crc (NewApplicationAddress,
//	                                   ((int) ApplicationCodeEntryPointInformation->MainApplicationSize))));
//
//	// If the computed CRC of flash application code doesn't match the CRC stored in flash.
//	if (ComputedCRC != ApplicationCodeEntryPointInformation->MainApplicationCRC)
//	{
//		Status = FALSE;
//	}
//
//
//	return(Status);
//} // END CRCValidForApplicationInFlash






//******************************************************************************************************
//	Validate the application entry point information pointed to by the caller.  Make sure that addresses
//	and sizes make sense.
//******************************************************************************************************
int ValidateEntryPointInformation(ENTRY_POINT_INFORMATION *ApplicationCodeEntryPointInformation)
{
	int		Status;					// Return status
	Status = TRUE;		// No errors yet.

	// If the size of the application code in flash isn't valid, return an error.
	if((ApplicationCodeEntryPointInformation->MainApplicationSize == 0) ||
	        (ApplicationCodeEntryPointInformation->MainApplicationSize > (UINT32) FLASH_MAX_APPLICATION_AREA_LENGTH))
	{
		Status = FALSE;
	}
	else
	{
		// We want to try looking for code where the application code configuration area tells us to, but if that doesn't
		// work, try the fixed location that we've used in the past.  There are two parts to verifying that the address
		// specified in the configuration area is good:  1) The address plus the size of flash code must fit in flash and 2)
		// the checksum must be correct (which is computed down below).
		if(((ApplicationCodeEntryPointInformation->MainApplicationLocation + ((UINT32) &FLASH_READ_BYTE_POINTER[0]) +
		        ApplicationCodeEntryPointInformation->MainApplicationSize) >
		        ((UINT32) &FLASH_READ_BYTE_POINTER[FLASH_BASE + FLASH_SIZE])) ||
		        ((ApplicationCodeEntryPointInformation->MainApplicationLocation + ((UINT32) &FLASH_READ_BYTE_POINTER[0])) <
		         ((UINT32) &FLASH_READ_BYTE_POINTER[FLASH_BASE])) ||
		        ((ApplicationCodeEntryPointInformation->MainApplicationLocation + ((UINT32) &FLASH_READ_BYTE_POINTER[0])) >=
		         ((UINT32) &FLASH_READ_BYTE_POINTER[FLASH_BASE + FLASH_SIZE])))
		{
			Status = FALSE;
		}
	}


	return(Status);
} // ValidateEntryPointInformation


//******************************************************************************************************
//	Copy the application code from flash to the caller specified location and validate it.
//	We want to try looking for code where the application code configuration area tells us
//	to, but if that doesn't work, try the fixed location that we've used in the past.  There
//	are two parts to verifying that the address specified in the configuration area is good:
//	1) The address plus the size of flash code must fit in flash and 2) the checksum must be
//	correct (which is computed down below).
int ReadApplicationCodeFromFlash(BYTE **DestinationLocation, ENTRY_POINT_INFORMATION *ApplicationCodeEntryPointInformation )
{
	int		Status;					// Return status
	ENTRY_POINT_INFORMATION		*PointerToNextApplicationCodeEntryPointInformation;


	// Start looking for a valid header at the default flash disk base.
	PointerToNextApplicationCodeEntryPointInformation = (ENTRY_POINT_INFORMATION *) &FLASH_READ_BYTE_POINTER[FLASH_DISK_BASE];

	// Keep track of the number of flash disks searched.
	FlashDiskSearchData.NumberOfFlashDisksFound = 0;

	do
	{
		// Add the next flash disk location to the search table if there is room.
		if(FlashDiskSearchData.NumberOfFlashDisksFound < MAXIMUM_NUMBER_OF_FLASH_DISK_STATISTICS_ENTRIES)
		{
			FlashDiskSearchData.Location[FlashDiskSearchData.NumberOfFlashDisksFound] = PointerToNextApplicationCodeEntryPointInformation;
			++(FlashDiskSearchData.NumberOfFlashDisksFound);
		}

		// In case the entry point information doesn't fall on a 4 byte boundary, copy it to a structure we know we can reference, since there is a problem
		// accessing this information when it is not on the proper boundary (if on a 2 byte boundary, the first two bytes will be accessed appropriately
		// for a given member, but the second two bytes referenced will be the two falling BEFORE the first two).
		memcpy(ApplicationCodeEntryPointInformation, PointerToNextApplicationCodeEntryPointInformation, sizeof(ENTRY_POINT_INFORMATION));

		// Try to validate at the standard address.  If it looks OK, copy the entry to RAM and try to checksum or CRC it.
		Status = ValidateEntryPointInformation(ApplicationCodeEntryPointInformation);

		// If the entry point information is valid, determine where we're going to copy the application to.  By default, we'll put it in the RAM_RESE
		// location, but if it's too big, and it will fit in the area allotted to heap space, we'll put it there.
		if( Status )
		{
			if( ApplicationCodeEntryPointInformation->MainApplicationSize <= ( ((UINT32) APP_IMAGE_RW_BASE ) - ((UINT32) &RAM_RESET )))
			{
				*DestinationLocation = (BYTE *) &RAM_RESET;
			}
			else if( ApplicationCodeEntryPointInformation->MainApplicationSize <=  ( ((UINT32) HEAP_END ) - ((UINT32) APP_IMAGE_ZI_END )))
			{
				*DestinationLocation = (BYTE *) APP_IMAGE_ZI_END;
			}
			else
			{
				Status = FALSE;
			}
		}


		if(Status)
		{
			BOOT_ENTRY_POINT_INFORMATION_VALIDATION_TIME = current_time();
			// Let's copy from flash to RAM.  We'll have to copy it there anyway if the code is valid.  Also, checksum and CRC
			// calculations are much faster in RAM.

			memcpy(*DestinationLocation, ((BYTE *) (ApplicationCodeEntryPointInformation->MainApplicationLocation)) + ((UINT32) &FLASH_READ_BYTE_POINTER[0]),
			            ApplicationCodeEntryPointInformation->MainApplicationSize);
			BOOT_APPLICATION_COPY_TIME = current_time();

			// Now, let's try checksum first; if that doesn't work, we'll try CRC.
			Status = ChecksumValidForApplicationInFlash(ApplicationCodeEntryPointInformation, *DestinationLocation);
			BOOT_CHECKSUM_VALIDATION_TIME = current_time();

			// what are the chances of checksum wrong and crc correct ?????!!!!!!!!?????
			//			if(!Status)
			//			{
			//				// Checksum didn't work; let's try CRC.
			//				RESET_WATCHDOG_TIMER();
			//				Status = CRCValidForApplicationInFlash(ApplicationCodeEntryPointInformation, *DestinationLocation);
			//				RESET_WATCHDOG_TIMER();
			//			}
			BOOT_CRC_VALIDATION_TIME = current_time();
		}

		// If we didn't find a valid application, let's search for the next flash disk header.
		if(!Status)
		{
			// Start the next search beyond the current entry point information and start of header.
			char	*NextSearchLocation = ((char *) PointerToNextApplicationCodeEntryPointInformation) + sizeof(ENTRY_POINT_INFORMATION) + 1;
			PointerToNextApplicationCodeEntryPointInformation = (ENTRY_POINT_INFORMATION *)FindFlashDiskHeader(NextSearchLocation);
		}
		RESET_WATCHDOG_TIMER();
		// Keep going until we find an application or we don't find another flash disk header.
	}
	while((!Status) && (((int) PointerToNextApplicationCodeEntryPointInformation) != -1));
	if(Status)
	{
		FLASH_DISK_LOCATION = (unsigned char *)PointerToNextApplicationCodeEntryPointInformation;
		INVERSE_FLASH_DISK_LOCATION = (unsigned char *)(~(int)FLASH_DISK_LOCATION);
	}
	return(Status);
} // ReadApplicationCodeFromFlash
#else

int StartFileSysytem(unsigned long long * ptr,unsigned int sz)
{
	int error=f_init();		// init filesytem ram variables
	//	int error;
	unsigned long temp= (unsigned long)ptr; // ensure alignment
	temp= (temp+7)& (~7);
	ptr= (unsigned long long *)temp;
	int memsize;
	char * p1buffer;
	if (!error)
	{
		memsize=fs_getmem_flashdrive(fs_phy_nor_num_m25pxx); // find required space
		error=(memsize==0);
		if (!error)
		{
			p1buffer=(char*)((sz<memsize)?NULL:ptr);
			if (!p1buffer)
			{
				error=2;
			}
		}
	}
	if (!error)
	{
		f_enterFS();
		error = f_mountdrive(0,p1buffer,memsize,fs_mount_flashdrive,fs_phy_nor_num_m25pxx);

		switch (error)
		{
		case FS_VOL_OK: //successfully mounted
			error =f_chdrive(0);
			break;
		case FS_VOL_NOTFORMATTED: //drive is mounted but drive is not formatted
            f_format(0);
			break;
		case FS_VOL_NOTMOUNT:// not mounted
		case FS_VOL_NOMEMORY: //not enough memory, drive is not mounted
		case FS_VOL_NOMORE: //no more drive available (FS_MAXVOLUME)
		case FS_VOL_DRVERROR: //mount driver error, not mounted

		default:
			;
		}
	}
	return error;
}


void SkipToNextRecord(const CTlv::Header_t * pTlvReocrd, F_FILE * pFile)
{
	DWORD DataAndPaddingLength = pTlvReocrd->Length + CTlvContainerParser::CalcNumPaddingBytes(pTlvReocrd);
	f_seek(pFile, DataAndPaddingLength, F_SEEK_CUR);  // skip the Length & padding bytes and point to the next Record
}


int SkipToAppBinary(F_FILE * pFile)
{
	CTlv::Header_t header;
	f_read(&header, 1, sizeof header, pFile); // this should be Gen6::tagCompatProd
	SkipToNextRecord(&header, pFile);
	
	f_read(&header, 1, sizeof header, pFile); // this should be Gen6::tagSwVersionStr
	SkipToNextRecord(&header, pFile);
	
	f_read(&header, 1, sizeof header, pFile); // this should be Gen6::tagData
	return header.Length;
}

int ReadApplicationCodeFromSerialFlash(BYTE **DestinationLocation)
{
	long size = f_filelength(FILENAME_MAIN_APP);
	if (size)
	{
		F_FILE * pFile = f_open(FILENAME_MAIN_APP, "r");
		if ( pFile )
		{
			DWORD dword[2];
			f_read(dword, 1, sizeof dword, pFile);
			if ( dword[0] == Gen6::tagAppFirmware )
			{	// it's an Application Firmware Object (from an MOCF)
				DWORD ObjectLength = dword[1];
				if ( ObjectLength + sizeof(CTlv::Header_t) != size )
				{
					f_close(pFile);
					return -10;
				}
				size = SkipToAppBinary(pFile);
				f_read(dword, 1, sizeof dword, pFile);
				f_seek(pFile, -sizeof dword, F_SEEK_CUR);  // back up to the beginning of the Application Firmware binary
			}
			else
			{	// It's a binary.  This is for migration, but we may leave it in.
				f_seek(pFile, 0, F_SEEK_SET);
			}
			// At this point we have loaded the 1st two DWORDs of the binary app firmware [in the MOCF] into dword
			// and the file pointer is pointing to the start of the binary app firmware.
			if ( dword[1] == 0xeafffffe )  
			{  // It's a bin with the decompressor app in front (we need to jump to it & it'll uncompress & run the app).
				// app decompressor expects app to start at HEAP_END - app size.
				unsigned int temp =(unsigned int)HEAP_END;
				temp-=size;
				temp&= (~7);
				*DestinationLocation = (BYTE *)temp;
			}
			else
			{	// If that instruction is not present, it's probably an uncompressed app that needs to be loaded into the reset vector area.  
				*DestinationLocation =(BYTE *)&RAM_RESET;
			}
			
			size = f_read((void *)(*DestinationLocation), 1, size, pFile);
			//DEBUG_printf("Read size is 0x%x,Destination:0x%x, 0x%x\r\n",size,DestinationLocation, *DestinationLocation);
			if (1)
			{
				int i = 0;
				UINT32 *pBUF = NULL;
				pBUF = (UINT32 *)(*DestinationLocation);
				UINT32 Temp = 0;
				for (i=0; i<size; i=i+4)
				{
					Temp = *((UINT32 *)(pBUF+i/4));
					*((UINT32 *)(pBUF+i/4)) = 0xFFFFFFFF - Temp;
				}
			}
		}
		f_close(pFile);
	}
	
	return size;
}

#endif // endof #ifndef SAFE_FFS_USED







/************************************************************************/
/************************************************************************/
/************************************************************************/
/* Main loop & system initialization.									*/
/************************************************************************/
extern "C" int C_Entry (void)
{
#ifndef SAFE_FFS_USED
	ENTRY_POINT_INFORMATION		ApplicationCodeEntryPointInformation;
#endif
	

	long size=0;

	BYTE						*ApplicationLoadAddress;

	int	ValidFlashApplicationCode =FALSE;		// Set by the routine that validates flash code; used to determine when to print messages.
	int	SkipApplicationCodeLoad =FALSE;				// Assume there isn't any flash code until we know otherwise.
	BOOT_MAIN_ENTRY_TIME = current_time();
	MAGIC_NUMBER_LOCATION = ~MaGiC_NuMbEr; // make sure the magic number is NOT set!!  I am NOT running out of RAM yet!!
	BOOT_TO_APP_REQUEST = REQUEST_NONE; // make sure we start with no request.

	init_hardware();
	init_system();

	BOOT_HARDWARE_SETUP_TIME = current_time();
	BOOT_MEMORY_CHECK_TIME = current_time();
#if PLATFORM_CURRENT == imx25
	//TODO ~DV find out what to do with each of the 4 leds
#endif // endof PLATFORM_CURRENT

	// Only check application code if we're not supposed to stay in boot code.  This allows us a chance to escape in
	// the case where the code tables are so corrupt that simply loading code causes memory exceptions.

	SkipApplicationCodeLoad = stay_in_boot();

	// Copy application code from flash to RAM; execution of the code may be skipped, but it does no harm to know that the
	// flash code is valid and ready to execute.  The read routine will try to determine validity first by checksum and
	// if that doesn't work (because the application code is old enough that it doesn't support checksum), try CRC.

	// just in case some bizarre app code could cause this function to bomb we will not load
	// app if the boot pin is asserted(low).
	//MX25 GPIO Port4 Pin3 "CS1/PORT4_3"
	//MXL port D PIN 7
#ifdef SAFE_FFS_USED
	InitSpi(2,NULL);
	StartFileSysytem((unsigned long long *)(HEAP_END+8),64*1024);  // 64 k from bottom of sys/usr stack
#endif

RetryLoad232Cmd:
	if(!SkipApplicationCodeLoad)
	{
#ifndef SAFE_FFS_USED
		ValidFlashApplicationCode = ReadApplicationCodeFromFlash( &ApplicationLoadAddress, &ApplicationCodeEntryPointInformation );
		size =ApplicationCodeEntryPointInformation.MainApplicationSize;
#else

// uncomment the following line to erase all flash including boot
//		SpiEraseAllFlash();
		BOOT_ENTRY_POINT_INFORMATION_VALIDATION_TIME =current_time();
		size = ReadApplicationCodeFromSerialFlash(&ApplicationLoadAddress);
		BOOT_APPLICATION_COPY_TIME=current_time();
		ValidFlashApplicationCode = (size>0);
		BOOT_CHECKSUM_VALIDATION_TIME=BOOT_CRC_VALIDATION_TIME=current_time();
		ROM_SIZE = GetFlashSize();
#endif

	}
	BOOT_TOTAL_APPLICATION_CODE_LOAD_TIME = current_time();
	// In case we didn't catch the first ESC, let's try again.
	if (!ValidFlashApplicationCode)
	{
		FullPowerOnMessage();
		if (SkipApplicationCodeLoad)
		{
			xputstring ("Skipping load of flash application code...\015\012");
		}
		else
		{
			xputstring ("No application code found\r\n");
#ifndef SAFE_FFS_USED

			xputstring ("Flash disk search data location: ");
			xputhex((int) &FlashDiskSearchData, 8);
			xputstring("\r\n");
#endif

		}
	}
	else
	{
		MAGIC_NUMBER_LOCATION = MaGiC_NuMbEr;

		RESET_WATCHDOG_TIMER();   /*One final kick before we run application */
		Run_application_code(ApplicationLoadAddress,ApplicationLoadAddress, size);
	}
	// Main loop, while forever...
	BOOT_TO_APP_REQUEST = REQUEST_NONE; // make sure we start with no request.
	while (1)
	{
		RESET_WATCHDOG_TIMER();
		poll_getchar_from_rx_fifo();
		if (kbhit())
		{                 /* check for received data, which is simply being polled above  */
			unsigned char Raw = get_char () ;           /* get received unsigned char     */
			ServiceSYNCommand(((UCHAR) Raw));			/* handle receive data        */
			if (BOOT_TO_APP_REQUEST ==REQUEST_232)
			{
				SkipApplicationCodeLoad=false;
				goto RetryLoad232Cmd;

			}
		}
	}
} 	/* main		*/



