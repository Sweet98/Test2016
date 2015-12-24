/************************************************************************/
//
// HHP 4480 embedded firmware
// Starting point for code was PSC Img8k code -- comments remain below
//
//  Hardware module -- intent is this module would contain prototypes and
// header information pertaining to relative schematic details referencing the processor (db_mx1.h)
// header file.  So net names would be declared in this module referencing the processor bits from that
// module, if that makes sense.
//  Date:       5/18/01
//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: db_shared/db_hdw.h $
// $Revision: 1.40.2.15 $
// $Date: 2010/04/14 07:57:40EDT $
//======================================================================================
/*|=======================================================================
  |           MODIFICATION HISTORY
  |=======================================================================
*/
#ifndef __DB_HDW_H         /*  prevent multiple definitions    */
#define __DB_HDW_H     1

//#define SHOW_RESET_TO_BOOT 1  //uncommenting this prints a diag msg to help find cause of reset_to_boot calls, defines debugging "_reset_to_boot_code(__LINE__)"

#include "language.h"
#include "db_sharedtable.h"

#if PLATFORM_CURRENT == imx25
#define SAFE_FFS_USED 1
#include "iMX25_EXTMEM.h"
#elif PLATFORM_CURRENT == imx27
#include "iMX27_extmem.h"
#endif

#ifndef MATRIX_BOOT_CODE
#include "Hardware_Clocks.h"
#include "Abort.h"
#include "db_time.h"
#endif //MATRIX_BOOT_CODE

/* Externs from assembler stuf */
extern "C" void EnableIRQ(void);
extern "C" void MMU_init(void);
extern "C" void MMU_reset(void);
extern "C" int MMU_IsEnabled( void );
extern "C" void MMU_CleanAndInvalidateBothCaches(void);
extern "C" void Move_Vector_Table_To_Zero(void);
extern unsigned long getFlashDiskBase(void);
extern "C" void RelocateAndRun(const void *src, void *dest, unsigned int bytesize);


/* handy sizes */
#define Sz_1K                           0x00000400
#define Sz_4K                           0x00001000     /// used below for the special block of RAM that get's daul mapped
#define Sz_16K                          0x00004000
#define Sz_64K                          0x00010000
#define Sz_256K                         0x00040000
#define Sz_512K                         0x00080000

#define Sz_1M                           0x00100000
#define Sz_2M                           0x00200000
#define Sz_4M                           0x00400000
#define Sz_8M                           0x00800000
#define Sz_16M                          0x01000000
#define Sz_32M                          0x02000000
#define Sz_64M                          0x04000000
#define Sz_128M                         0x08000000
#define Sz_256M                         0x10000000
#define Sz_512M							0x20000000
#define Sz_1G                           0x40000000

#define HB_DELAY    10000          			// 10ms worth of ticks... I have no idea why its 1MHz freq related, SYSCLK, as far as I can tell is HCLK/6... just don't know where the /6 is done... or set up... or?!

#if PLATFORM_CURRENT == imx25
#define SDRAMBase   CSD0_BASE_ADDR          /* DRAM Address 0x80000000 located in file iMX25_EXTMEM.h*/
#elif PLATFORM_CURRENT == imx27
#define SDRAMBase    CSD0_BASE_ADDR         /* DRAM Address 0xA0000000 located in file iMX27_extmem.h*/
#elif PLATFORM_CURRENT == mxl
#define SDRAMBase    0x08000000          	/* DRAM */
#endif


#define MaGiC_NuMbEr 0x01234567
#define ROM_MaGiC_NuMbEr 0x01234568
#define REQUEST_NONE 0
#define REQUEST_232 232

#define RAM_BASE_TABLE SDRAMBase + Sz_4K                 // this is being used to define the RAM base table for the exception vector
#define RAM_RESET      (* (volatile unsigned int *) (RAM_BASE_TABLE + 0))     // when RAM gets binary file downloaded this is starting point
#define	RAM_ADR_EXC	(* (volatile unsigned int *) (RAM_BASE_TABLE + 0x14))	// RAM vector for address exceptions

#define	MAGIC_NUMBER_LOCATION RAM_ADR_EXC		// Magic number is located at the address exception handler in RAM



#define	FLASH_BASE			0x0000			// Flash starts at address 0x0
#define	CACHED_FLASH_BASE	CACHED_ROM_BASE		//0x10000000		// Remapped cached flash starts at address 0x10000000
#define	UNCACHED_FLASH_BASE	NON_CACHED_ROM_BASE	//0x00400000		// Remapped uncached flash starts at address 0x00400000
#define	FLASH_READ_BASE		CACHED_ROM_BASE			//CACHED_FLASH_BASE
#define	FLASH_WRITE_BASE	NON_CACHED_ROM_BASE		//UNCACHED_FLASH_BASE

#define	FLASH_SIZE			ROM_SIZE			// 0x100000	// Flash is 1Mb in size


// Define byte, word, and long pointers to flash
#define	FLASH_READ_BYTE_POINTER ((volatile unsigned char *) (FLASH_READ_BASE))
#define	FLASH_READ_WORD_POINTER ((volatile unsigned short *) (FLASH_READ_BASE))
#define	FLASH_READ_LONG_POINTER ((volatile unsigned int *) (FLASH_READ_BASE))
#define	FLASH_WRITE_BYTE_POINTER ((volatile unsigned char *) (FLASH_WRITE_BASE))
#define	FLASH_WRITE_WORD_POINTER ((volatile unsigned short *) (FLASH_WRITE_BASE))
#define	FLASH_WRITE_LONG_POINTER ((volatile unsigned int *) (FLASH_WRITE_BASE))



#define	FLASH_BOOT_CODE_BASE			0x0000		// Start of boot sector
#ifdef SAFE_FFS_USED
#define BOOTCODE_ROM_ADDR 0x800		// where our boot code goes in rom
#define	FLASH_MAX_BOOT_CODE_LENGTH		(64 *1024 - BOOTCODE_ROM_ADDR)  // Boot code is restricted to the 1st 64K block.  HAB info preceeds it.
#else
#define	FLASH_MAX_BOOT_CODE_LENGTH		0x4000		// Boot code is restricted to 16K.
#endif
#define	FLASH_WORKING_CONFIGURATION_BASE		FLASH_MANUFACTURING_CONFIGURATION_BASE+	FLASH_MAX_MANUFACTURING_CONFIGURATION_LENGTH
// Start configuration area at 16K offset into flash.
#define	FLASH_BACKUP_WORKING_CONFIGURATION_BASE	0xC000		// Start backup configuration area just below default configuration
#define	FLASH_DEFAULT_CONFIGURATION_BASE		0xE000		// Start default configuration area
#define	FLASH_MAX_CONFIGURATION_LENGTH			0x2000		// Maximum size of configuration area is 8K



#define	FLASH_MANUFACTURING_CONFIGURATION_BASE			0x4000	// Start manufacturing configuration area at 32K
//	offset into flash.
#define	FLASH_MAX_MANUFACTURING_CONFIGURATION_LENGTH	0x2000	// Maximum size of manufacturing configuration
//	area is 8K.
#define	FLASH_PRODUCT_CONFIGURATION_BASE				FLASH_MANUFACTURING_CONFIGURATION_BASE
// Start product configuration area at 32K offset into flash.
#define	FLASH_MAX_PRODUCT_CONFIGURATION_LENGTH			0x1000	// Maximum size of product configuration area is 4K
#define	FLASH_MFG_VERIFIER_CALIBRATION_DATA_BASE		FLASH_PRODUCT_CONFIGURATION_BASE+FLASH_MAX_PRODUCT_CONFIGURATION_LENGTH
// Start product configuration area at 36K offset into flash.
#define	FLASH_MAX_MFG_VERIFIER_CALIBRATION_DATA_LENGTH	0x1000	// Maximum size of manufacturing Verifier calibration data area is 4K

#define	FLASH_START_OF_WRITABLE_AREA		0x6000	// Start of area writable by FlashDisk
#define	FLASH_SIZE_OF_WRITABLE_AREA			0xFA000	// Size of area writable by FlashDisk


#define	FLASH_APPLICATION_BASE				0x30000					// Start application area at 64K offset into flash.
// Maximum size of FLASH application is 896K (1024-128), or 1920k (2048-128), depending on ROM size
// Thus the macro below gives a maximum size of application area at 896K for 1MB and 2MB ROMs, and 1920K for ROMs >= 4MB
#if PLATFORM_CURRENT == imx25
#define	FLASH_MAX_APPLICATION_AREA_LENGTH	(0x200000-0x20000)
#else
#define	FLASH_MAX_APPLICATION_AREA_LENGTH	((ROM_SIZE>=(4*1024*1024))?(0x200000-0x20000):(0x100000-0x20000))
#endif
#define	FLASH_DISK_BASE						getFlashDiskBase()			// Start of flash disk.
#define	FLASH_DISK_SECTOR_SIZE				0x4000			// Size of a flash disk sector






// Include definitions and address information for device type information based on whether we're running boot or application code.
#ifdef MATRIX_BOOT_CODE
#define	CODETYPE		"BOOT"
#define	DEVICE_TYPE_FLAGS_LENGTH ((volatile unsigned int *) (CACHED_FLASH_BASE + 76))
#define	DEVICE_TYPE_FLAGS ((volatile unsigned char *) (CACHED_FLASH_BASE + 80))
#else
#define	CODETYPE		"APP"
#define	DEVICE_TYPE_FLAGS_LENGTH ((volatile unsigned int *) (RAM_BASE_TABLE + 76))
#define	DEVICE_TYPE_FLAGS ((volatile unsigned char *) (RAM_BASE_TABLE + 80))
#endif



// Power management definitions

// SWI defines for passed argument control


extern int 	EscReceived;
extern int	CharsWrittenToUart;







//void init_SoftwareDefines(void);
void init_hardware (void) ;  /* pre-configuration hardware setup */
void init_system (void) ;    /* post configuration system init */
void init_wdog (void);

void configure_watchdog (void);

#if defined(SHOW_RESET_TO_BOOT) && !defined(MATRIX_BOOT_CODE)
#define reset_to_boot_code_hard() _reset_to_boot_code_hard(__LINE__)
void _reset_to_boot_code_hard(int);
#else
void reset_to_boot_code_hard(void);
#endif


/* notification versions of enable and disable interrupts */
/* necessary at this time, to indicate RX ready */
extern "C" void EnableIRQ(void);		// enable interrupts and notify the host that they have been enabled
extern "C" void DisableIRQ(void);		// disable and notify

//extern "C" void Report_Boot_Version();		// routine made available to call the SWI responsible for reporting boot code version level




//bool BootVersion(ULONG *MajorNumber, ULONG *MinorNumber);	// Return the boot version number to the caller.



// Determine if we're running in ROM by checking to see if this routine is located in ROM or RAM.
//bool running_in_ROM(void);

// report boot version
void Report_Boot_Version(void);




#if !defined( MATRIX_BOOT_CODE ) && !defined( SIMPLE_IO_ONLY )
	#include "ioman.h"

CIoMan *GetCurrentInterface( void );
void SetCurrentInterface( CIoMan *NewCurrentInterfacePointer );
#endif


#endif  /*  __DB_HDW_H */



