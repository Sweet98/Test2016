/************************************************************************/
//
// HHP 4480 embedded firmware
//
//  Shared table structure for info shared between boot and assembly code
//;**** Note this is the C version and there is a Assembly equivalent
//;**** For now, the assumption is he/she who enters here, must also enter there and visa versa
//  Date:       2/24/02
//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: db_shared/db_sharedtable.h $
// $Revision: 1.13.2.9 $
// $Date: 2011/04/18 15:23:54EDT $
//======================================================================================
/************************************************************************/
/*|=======================================================================
  |           MODIFICATION HISTORY
  |=======================================================================
*************************************************************************/

#ifndef __DB_SHAREDTABLE_H         /*  prevent multiple definitions    */
#define __DB_SHAREDTABLE_H     1

#include "db_hdw.h"      // used for memory map understanding


// The concept of this file is to define the look 'n feel of a shared table between boot and app code that is
// useful to the operation of both.  The current layout of this is in sort of fixed memory mapped space as follows:
//   SDRAM start 0x08000000
//   Code built for SDRAM start + 4k
// The 4k block is set aside for the copy of the vectors, any associative fall through of the FIQ and this table.
// Most basic format is as follows:
//   0x08000000 - 0x0800001f (32 bytes) --> actual ldr pc,= vector loads for the exception handlers
//   0x08000020 - 0x0800003f (32 bytes) --> Vector addresses for the above vector ldrs
//   0x08000040 - 0x08001000 (4k - 64 bytes) --> Room for this table
//        **** The concept is this... 4k was chosen as it implies certain things to the MMU redirect of that table, in that
//        **** 4kB is the smallest page table you can direct with a level 2 translation table...
//        **** But the bottom line is 4KB is way more than I need for this table... at this point I have use for about 10 bytes and
//        **** while the need wants to be able to grow to support future things, it will never fill the 4KB space.
//        **** So, add that to the possible need to improve FIQ interrupt response time by writing the interrupt in assembly and having
//        **** the FIQ vector "fall through" to FIQ handler immediately below, I would hope to reserve space in this block for that
//        **** type of enhancement.
//
//  Having said this, my 2 immediate choices are to build this table from the top down or just reserve enough space to imagine that need

// Note, this 4k block at the start of boot code time is NOT MMU mapped to 0x0 as the boot flash area is still responsible for running
// Therefore access in boot land should be done by accessing SDRAM base to SDRAM base + 4k
// App code should be able to access either location, so this file *implies* it will do so in the same SDRAM physical 1:1 mapped location

// So to define the memory area definitely
#ifdef RAMBASEDBOOT		//Boot Compiler SW that defines a fully iRAM based Boot for HW testing
	#define SHARED_TABLE_BASE          SharedTableLocation 
	#define SHARED_TABLE_END           RAM_BASE_TABLE         // this should be defined as the beginning of runnable code space in RAM at 0x08000000 + 4kB (0x08001000)

	#define ACTUAL_TABLE_LOCATION      0x78017000 //SharedTableLocation
#else
	#define SHARED_TABLE_BASE          (SDRAMBase + 64)         // this is where the vector end in the simplest approach (where the FIQ does not fall through)
	#define SHARED_TABLE_END           RAM_BASE_TABLE         // this should be defined as the beginning of runnable code space in RAM at 0x08000000 + 4kB (0x08001000)

	#define ACTUAL_TABLE_LOCATION      (RAM_BASE_TABLE - 0x400)   // take the top 1k for this table, allowing far more than you'll ever need for both table and growth of fiqs
#endif //endof RAMBASEDBOOT

// enumerator to break down the memory into useful blocks... not sure if this will be used
enum MEM_LOCATION {
    FLASH_BOOT_BLOCK = 0, FLASH_1ST_8K_BLOCK, FLASH_2ND_8K_BLOCK, FLASH_32K_BLOCK, FLASH_1ST_64K_BLOCK, FLASH_LAST_64K_BLOCK,
    SRAM_BLOCK, SDRAM_BLOCK
};

// Actual Shared Structure -- Again concept is boot sets this up before handing controls over to app code
// This table has the ability to grow to be 1KB... so there should always be throw away space... which seems ok at the time... caus'in we got 8MB
// and the MMU table size already implies the 4KB above... although I just had a thought... what if the realtime wedge table could be stored here?!
// Well, we can think about this later...
// Note, the original thougth was to use a C structure with efficient use of bitfields, etc... but the concern moving formward with changing landscape (compilers, etc)
// it would be difficult to track how C and Assembly code work together.  Noting that some of this information wants to be set and/or interpreted from
// assembly source, it seemed more secure moving forward to use the more generic fixed address approach of setting up variable names at fixed addresses off
// some offset... hopefully the implied IMPORT/EXPORT scheme will work with assembly today AND tommorow
#define SIZE_OF_SHARED_TABLE       (*( int *)(ACTUAL_TABLE_LOCATION + 0x0))  // how big this table is
#define BOOT_CODE_MINOR_VERSION    (*( int *)(ACTUAL_TABLE_LOCATION + 0x4))  // Boot code version is stored in 2 parts, major.minor for now
#define BOOT_CODE_MAJOR_VERSION    (*( int *)(ACTUAL_TABLE_LOCATION + 0x8))  // Boot code version is stored in 2 parts, major.minor for now
#define SIZE_OF_OP_CODE_TO_STORE   (*( int *)(ACTUAL_TABLE_LOCATION + 0xc))  // means for boot code to inform app to *consider* storing itself to flash
#define SIZE_OF_SDRAM              (*( int *)(ACTUAL_TABLE_LOCATION + 0x10)) // means to inform app of size of RAM

// Boot code sets DYNAMIC_MEMORY_MAP to 0x12345678 to indicate that the boot code has configured the dynamic memory map variables that follow after.
// Since application code is looking for this value, but may be running on a unit with older boot code that doesn't load this value,
// INVERSE_DYNAMIC_MEMORY_MAP is loaded with the 1s complement of DYNAMIC_MEMORY_MAP.  The application can compare DYNAMIC_MEMORY_MAP to the
// inverse of INVERSE_DYNAMIC_MEMORY_MAP and if they don't match, it knows that DYNAMIC_MEMORY_MAP is not valid and that it will have to default to
// an 8MB RAM space memory map.
#define DYNAMIC_MEMORY_MAP			(*( int*)(ACTUAL_TABLE_LOCATION + 0x14))
#define INVERSE_DYNAMIC_MEMORY_MAP	(*( int*)(ACTUAL_TABLE_LOCATION + 0x18))

#define	CACHED_MEMORY_BASE			(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0x1C))	// Address of cached memory space, which is a
//	mirror of non-cached space, but allows cached /
//	buffered access to memory.
#define	MEMORY_SIZE					(*( int *)(ACTUAL_TABLE_LOCATION + 0x20))	// Size of detected memory in bytes.

#define	HEAP_START					(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0x24))	// Start location of the application heap
#define	HEAP_END					(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0x28))	// End location of the application heap
#define	APP_IMAGE_RO_BASE			(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0x2C))	// Start of read only space in application image
#define	APP_IMAGE_RO_END			(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0x30))	// End of read only space in application image
#define	APP_IMAGE_RW_BASE			(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0x34))	// Start of read / write space in application image
#define	APP_IMAGE_RW_END			(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0x38))	// End of read / write space in application image
#define	APP_IMAGE_ZI_BASE			(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0x3C))	// Start of zero init space in application image
#define	APP_IMAGE_ZI_END			(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0x40))	// End of zero init space in application image
#define BOOT_CODE_VERSION_STRING   (*( char **)(ACTUAL_TABLE_LOCATION + 0x44))  // Boot code version string location.
#define COMPRESSED_APPLICATION_LOCATION	(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0x48))
#define COMPRESSED_APPLICATION_SIZE	(*( int *)(ACTUAL_TABLE_LOCATION + 0x4C))
#define APPLICATION_COMPRESSOR_LOCATION	(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0x50))
#define COMPRESSOR_AND_COMPRESSED_APPLICATION_SIZE	(*( int *)(ACTUAL_TABLE_LOCATION + 0x54))

// Boot code sets FLASH_DISK_LOCATION to the address where it found a flash disk header with a valid application to load.  If none is found, this value
// should be 0xFFFFFFFF.  Since application code is looking for this value, but may be running on a unit with older boot code that doesn't load this
// value, INVERSE_FLASH_DISK_LOCATION is loaded with the 1s complement of FLASH_DISK_LOCATION.  The application can compare FLASH_DISK_LOCATION to the
// inverse of INVERSE_FLASH_DISK_LOCATION and if they don't match, it knows that FLASH_DISK_LOCATION is not valid and that it will have to find the
// flash disk itself.
#define FLASH_DISK_LOCATION			(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0x58))
#define INVERSE_FLASH_DISK_LOCATION	(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0x5C))

// Boot code startup times that application may want access to for reporting purposes.
#define BOOT_MAIN_ENTRY_TIME		(*( int *)(ACTUAL_TABLE_LOCATION + 0x60))
#define BOOT_HARDWARE_SETUP_TIME	(*( int *)(ACTUAL_TABLE_LOCATION + 0x64))
#define BOOT_MEMORY_CHECK_TIME		(*( int *)(ACTUAL_TABLE_LOCATION + 0x68))
#define BOOT_ENTRY_POINT_INFORMATION_VALIDATION_TIME	(*( int *)(ACTUAL_TABLE_LOCATION + 0x6C))
#define BOOT_APPLICATION_COPY_TIME	(*( int *)(ACTUAL_TABLE_LOCATION + 0x70))
#define BOOT_CHECKSUM_VALIDATION_TIME	(*( int *)(ACTUAL_TABLE_LOCATION + 0x74))
#define BOOT_CRC_VALIDATION_TIME	(*( int *)(ACTUAL_TABLE_LOCATION + 0x78))
#define BOOT_TOTAL_APPLICATION_CODE_LOAD_TIME	(*( int *)(ACTUAL_TABLE_LOCATION + 0x7C))

#define COMPRESSOR_SIZE						(*( int *)(ACTUAL_TABLE_LOCATION + 0x80))
#define COMPRESSED_APPLICATION_HEADER_SIZE	(*( int *)(ACTUAL_TABLE_LOCATION + 0x84))

#define	NON_CACHED_MEMORY_BASE		(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0x88))	// Address of non-cached memory space, which is a
//	mirror of cached space, but allows non-cached /
//	non-buffered access to memory.

#define	CACHED_ROM_BASE				(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0x8C))	// Address of cached ROM space, which is a
//	mirror of non-cached space, but allows cached
//	access to memory for speedier read operations.
#define	NON_CACHED_ROM_BASE			(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0x90))	// Address of non-cached ROM space, which is a
//	mirror of cached space, but allows non-cached /
//	non-buffered access to ROM.
#define	ROM_SIZE					(*( int *)(ACTUAL_TABLE_LOCATION + 0x94))	// Size of detected ROM in bytes.
#define	MMU_LEVEL_1_TABLE			(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0x98))	// Address of MMU level 1 translation table.
#define	MMU_LEVEL_1_TABLE_SIZE		(*( int *)(ACTUAL_TABLE_LOCATION + 0x9C))	// Size of MMU level 1 translation table.
#define	MMU_LEVEL_2_TABLE			(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0xA0))	// Address of MMU level 2 vector table.
#define	MMU_LEVEL_2_TABLE_SIZE		(*( int *)(ACTUAL_TABLE_LOCATION + 0xA4))	// Size of MMU level 2 vector table.
#define	MMU_DISABLE_BUFFER			(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0xA8))	// Address of MMU cache disable buffer (no longer
//	needed, but removing it now will cause boot /
//	application code compatibility problems).
#define	MMU_DISABLE_BUFFER_SIZE		(*( int *)(ACTUAL_TABLE_LOCATION + 0xAC))	// Size of MMU cache disable buffer (no longer
//	needed, but removing it now will cause boot /
//	application code compatibility problems).
#define	SVC_STACK					(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0xB0))	// Address of SVC stack top.
#define	ABT_STACK					(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0xB4))	// Address of ABT stack base.
#define	UNDEF_STACK					(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0xB8))	// Address of UNDEF stack base.
#define	IRQ_STACK					(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0xBC))	// Address of IQR stack base.
#define	FIQ_STACK					(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0xC0))	// Address of FIQ stack base.
#define	SYS_STACK					(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0xC4))	// Address of SYS stack base.

#define	BOOT_TO_APP_REQUEST			(*( int *)(ACTUAL_TABLE_LOCATION + 0xC8))	// Reserved space to pass a request from boot to app.

#define	DCMP_HEAP_START				(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0xCC))	// Start location of the application heap
#define	DCMP_HEAP_END				(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0xD0))	// End location of the application heap
#define	DCMP_APP_IMAGE_RO_BASE		(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0xD4))	// Start of read only space in application image
#define	DCMP_APP_IMAGE_RO_END		(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0xD8))	// End of read only space in application image
#define	DCMP_APP_IMAGE_RW_BASE		(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0xDC))	// Start of read / write space in application image
#define	DCMP_APP_IMAGE_RW_END		(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0xE0))	// End of read / write space in application image
#define	DCMP_APP_IMAGE_ZI_BASE		(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0xE4))	// Start of zero init space in application image
#define	DCMP_APP_IMAGE_ZI_END		(*( unsigned char **)(ACTUAL_TABLE_LOCATION + 0xE8))	// End of zero init space in application image
#define SHARED_TABLE_FLASHTYPE  	(*( int *)(ACTUAL_TABLE_LOCATION +0xEC))    // flash type for nor flash for now
#define DEVICE_TYPE_ST  			(*( int *)(ACTUAL_TABLE_LOCATION +0xF0))    // device type
// Added in BootMinorVersion 2
#define BOOT_CODE_REVISION_STR		(*( char **)(ACTUAL_TABLE_LOCATION + 0xF4))  // Boot code sw revision (part number)
#define	BOOT_COMPAT_PROD_ID			(*( int *)(ACTUAL_TABLE_LOCATION + 0xF8))	//  Boot code MOCF compatible products ID
// Added in BootMinorVersion 3
#define	BOOT_DATE_STR				(*(const char **)(ACTUAL_TABLE_LOCATION + 0x0FC))
#define	BOOT_TIME_STR				(*(const char **)(ACTUAL_TABLE_LOCATION + 0x100))

#define SHARED_TABLE_LAST			(ACTUAL_TABLE_LOCATION +0x104)  // Update this if you add to the table
//**** up to the user to ensure this is one past the last entry in the table  ****//
//**** the limit is this cannot cross the RAM_BASE_TABLE above, but it should never ever get close... 1KB was set aside ****//

#endif  /*  __DB_SHAREDTABLE_H */

