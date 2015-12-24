;/************************************************************************/
;//
;// HHP 4480 embedded firmware
;//
;//  Shared table structure for info shared between boot and assembly code
;**** Note this is the assembly version and there is a C equivalent
;**** For now, the assumption is he/she who enters here, must also enter there and visa versa
;//  Date:       2/24/02
;//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
;//  Target:     D-ball 4480
;//
;//======================================================================================
;//   Revision information
;//======================================================================================
;// $RCSfile: db_shared/db_sharedtable_h.s $
;// $Revision: 1.12.2.5 $
;// $Date: 2009/03/10 11:49:29EDT $
;//======================================================================================
;/************************************************************************/
;/*|=======================================================================
;  |           MODIFICATION HISTORY
;  |=======================================================================
;*************************************************************************/

;; This is the assembly equivalent for the C header for the same purpose
;; Some time was spent imagining a slick way to only have to change one of these files, but with the changing landscape toolswise
;; I didn't want to do something that was tool specific.  For this reason, any time things are added to the table, it should be added
;; in 2 places... here and in db_sharedtable.h

;;// So to define the memory area definitely

	IF :DEF: RAMBASEDBOOT
ACTUAL_TABLE_LOCATION_S    EQU     SharedTableLocation ;Located in iRAM just below the stacks

	ELSE
ACTUAL_TABLE_LOCATION_S    EQU     RAMStartOpCode - 0x400

	ENDIF
	
;; Actual Shared Structure/Table -- Again NOTE, if you add here, please add equivalent entry into the db_sharedtable.h C header
SIZE_OF_SHARED_TABLE_S		EQU		ACTUAL_TABLE_LOCATION_S + 0x0		;; how big the table is
BOOT_CODE_MINOR_VERSION_S	EQU		ACTUAL_TABLE_LOCATION_S + 0x4		;; Boot code version (minor)
BOOT_CODE_MAJOR_VERSION_S	EQU		ACTUAL_TABLE_LOCATION_S + 0x8		;; Boot code version (minor)
SIZE_OF_OP_CODE_TO_STORE_S	EQU		ACTUAL_TABLE_LOCATION_S + 0xc		;; means for boot code to inform app to *consider* storing itself to flash
SIZE_OF_SDRAM_S				EQU		ACTUAL_TABLE_LOCATION_S + 0x10		;; means to inform app of size of RAM

;; Boot code sets DYNAMIC_MEMORY_MAP to 0x12345678 to indicate that the boot code has configured the dynamic memory map variables that follow after.
;; Since application code is looking for this value, but may be running on a unit with older boot code that doesn't load this value,
;; INVERSE_DYNAMIC_MEMORY_MAP is loaded with the 1s complement of DYNAMIC_MEMORY_MAP.  The application can compare DYNAMIC_MEMORY_MAP to the
;; inverse of INVERSE_DYNAMIC_MEMORY_MAP and if they don't match, it knows that DYNAMIC_MEMORY_MAP is not valid and that it will have to default to
;; an 8MB RAM space memory map.
DYNAMIC_MEMORY_MAP			EQU		ACTUAL_TABLE_LOCATION_S + 0x14
INVERSE_DYNAMIC_MEMORY_MAP	EQU		ACTUAL_TABLE_LOCATION_S + 0x18

CACHED_MEMORY_BASE			EQU		ACTUAL_TABLE_LOCATION_S + 0x1C		;; Address of cached memory space, which is a
																		;;	mirror of non-cached space, but allows cached /
																		;;	buffered access to memory.
MEMORY_SIZE					EQU		ACTUAL_TABLE_LOCATION_S + 0x20		;; Size of detected memory in bytes.

HEAP_START					EQU		ACTUAL_TABLE_LOCATION_S + 0x24		;; Start location of the application heap
HEAP_END					EQU		ACTUAL_TABLE_LOCATION_S + 0x28		;; End location of the application heap
APP_IMAGE_RO_BASE			EQU		ACTUAL_TABLE_LOCATION_S + 0x2C		;; Start of read only space in application image
APP_IMAGE_RO_END			EQU		ACTUAL_TABLE_LOCATION_S + 0x30		;; End of read only space in application image
APP_IMAGE_RW_BASE			EQU		ACTUAL_TABLE_LOCATION_S + 0x34		;; Start of read / write space in application image
APP_IMAGE_RW_END			EQU		ACTUAL_TABLE_LOCATION_S + 0x38		;; End of read / write space in application image
APP_IMAGE_ZI_BASE			EQU		ACTUAL_TABLE_LOCATION_S + 0x3C		;; Start of zero init space in application image
APP_IMAGE_ZI_END			EQU		ACTUAL_TABLE_LOCATION_S + 0x40		;; End of zero init space in application image
BOOT_CODE_VERSION_STRING_S	EQU		ACTUAL_TABLE_LOCATION_S + 0x44		;; Boot code version string location.
COMPRESSED_APPLICATION_LOCATION	EQU		ACTUAL_TABLE_LOCATION_S + 0x48	;;
COMPRESSED_APPLICATION_SIZE	EQU		ACTUAL_TABLE_LOCATION_S + 0x4C		;;
APPLICATION_COMPRESSOR_LOCATION	EQU		ACTUAL_TABLE_LOCATION_S + 0x50	;;
COMPRESSOR_AND_COMPRESSED_APPLICATION_SIZE	EQU		ACTUAL_TABLE_LOCATION_S + 0x54	;;

;; Boot code sets FLASH_DISK_LOCATION to the address where it found a flash disk header with a valid application to load.  If none is found, this value
;; should be 0xFFFFFFFF.  Since application code is looking for this value, but may be running on a unit with older boot code that doesn't load this
;; value, INVERSE_FLASH_DISK_LOCATION is loaded with the 1s complement of FLASH_DISK_LOCATION.  The application can compare FLASH_DISK_LOCATION to the
;; inverse of INVERSE_FLASH_DISK_LOCATION and if they don't match, it knows that FLASH_DISK_LOCATION is not valid and that it will have to find the
;; flash disk itself.
FLASH_DISK_LOCATION			EQU		ACTUAL_TABLE_LOCATION_S + 0x58		;;
INVERSE_FLASH_DISK_LOCATION	EQU		ACTUAL_TABLE_LOCATION_S + 0x5C		;;

;; Boot code startup times that application may want access to for reporting purposes.
BOOT_MAIN_ENTRY_TIME		EQU		ACTUAL_TABLE_LOCATION_S + 0x60		;;
BOOT_HARDWARE_SETUP_TIME	EQU		ACTUAL_TABLE_LOCATION_S + 0x64		;;
BOOT_MEMORY_CHECK_TIME		EQU		ACTUAL_TABLE_LOCATION_S + 0x68		;;
BOOT_ENTRY_POINT_INFORMATION_VALIDATION_TIME	EQU		ACTUAL_TABLE_LOCATION_S + 0x6C		;;
BOOT_APPLICATION_COPY_TIME	EQU		ACTUAL_TABLE_LOCATION_S + 0x70		;;
BOOT_CHECKSUM_VALIDATION_TIME	EQU		ACTUAL_TABLE_LOCATION_S + 0x74		;;
BOOT_CRC_VALIDATION_TIME	EQU		ACTUAL_TABLE_LOCATION_S + 0x78		;;
BOOT_TOTAL_APPLICATION_CODE_LOAD_TIME	EQU		ACTUAL_TABLE_LOCATION_S + 0x7c		;;

COMPRESSOR_SIZE						EQU		ACTUAL_TABLE_LOCATION_S + 0x80		;;
COMPRESSED_APPLICATION_HEADER_SIZE	EQU		ACTUAL_TABLE_LOCATION_S + 0x84		;;

NON_CACHED_MEMORY_BASE		EQU		ACTUAL_TABLE_LOCATION_S + 0x88		;; Address of non-cached memory space, which is a
																		;;	mirror of cached space, but allows non-cached /
																		;;	non-buffered access to memory.
																									
CACHED_ROM_BASE				EQU		ACTUAL_TABLE_LOCATION_S + 0x8C		;; Address of cached ROM space, which is a
																		;;	mirror of non-cached space, but allows cached
																		;;	access to memory for speedier read operations.
NON_CACHED_ROM_BASE			EQU		ACTUAL_TABLE_LOCATION_S + 0x90		;; Address of non-cached ROM space, which is a
																		;;	mirror of cached space, but allows non-cached /
																		;;	non-buffered access to ROM.
ROM_SIZE					EQU		ACTUAL_TABLE_LOCATION_S + 0x94		;; Size of detected ROM in bytes.
MMU_LEVEL_1_TABLE			EQU		ACTUAL_TABLE_LOCATION_S + 0x98		;; Address of MMU level 1 translation table.
MMU_LEVEL_1_TABLE_SIZE		EQU		ACTUAL_TABLE_LOCATION_S + 0x9C		;; Size of MMU level 1 translation table.
MMU_LEVEL_2_TABLE			EQU		ACTUAL_TABLE_LOCATION_S + 0xA0		;; Address of MMU level 2 vector table.
MMU_LEVEL_2_TABLE_SIZE		EQU		ACTUAL_TABLE_LOCATION_S + 0xA4		;; Size of MMU level 2 vector table.
NOT_USED1					EQU		ACTUAL_TABLE_LOCATION_S + 0xA8		;; Address of MMU cache disable buffer (no longer
																		;;	needed, but removing it now will cause boot /
																		;;	application code compatibility problems).
NOT_USED2					EQU		ACTUAL_TABLE_LOCATION_S + 0xAC		;; Size of MMU cache disable buffer (no longer
																		;;	needed, but removing it now will cause boot /
																		;;	application code compatibility problems).
SVC_STACK					EQU		ACTUAL_TABLE_LOCATION_S + 0xB0		;; Address of SVC stack top.
ABT_STACK					EQU		ACTUAL_TABLE_LOCATION_S + 0xB4		;; Address of ABT stack base.
UNDEF_STACK					EQU		ACTUAL_TABLE_LOCATION_S + 0xB8		;; Address of UNDEF stack base.
IRQ_STACK					EQU		ACTUAL_TABLE_LOCATION_S + 0xBC		;; Address of IQR stack base.
FIQ_STACK					EQU		ACTUAL_TABLE_LOCATION_S + 0xC0		;; Address of FIQ stack base.
SYS_STACK					EQU		ACTUAL_TABLE_LOCATION_S + 0xC4		;; Address of SYS stack base.

BOOT_TO_APP_REQUEST		EQU		ACTUAL_TABLE_LOCATION_S + 0xC8		;; Reserved space to pass a request from boot to app.

DCMP_HEAP_START			EQU		ACTUAL_TABLE_LOCATION_S + 0xCC
DCMP_HEAP_END			EQU		ACTUAL_TABLE_LOCATION_S + 0xD0
DCMP_APP_IMAGE_RO_BASE	EQU		ACTUAL_TABLE_LOCATION_S + 0xD4
DCMP_APP_IMAGE_RO_END	EQU		ACTUAL_TABLE_LOCATION_S + 0xD8
DCMP_APP_IMAGE_RW_BASE	EQU		ACTUAL_TABLE_LOCATION_S + 0xDC
DCMP_APP_IMAGE_RW_END	EQU		ACTUAL_TABLE_LOCATION_S + 0xE0
DCMP_APP_IMAGE_ZI_BASE	EQU		ACTUAL_TABLE_LOCATION_S + 0xE4
DCMP_APP_IMAGE_ZI_END	EQU		ACTUAL_TABLE_LOCATION_S + 0xE8

SHARED_TABLE_FLASHTYPE	EQU		ACTUAL_TABLE_LOCATION_S + 0xEC		;; flash type
DEVICE_TYPE_ST			EQU		ACTUAL_TABLE_LOCATION_S + 0xF0		;; device type

SHARED_TABLE_LAST_ENTRY_S	EQU		ACTUAL_TABLE_LOCATION_S + 0xF0		;; primitive copy of end of struct/table

     END

