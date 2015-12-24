2.43
	Drivers (NOR,NAND,DFLASH) _wear and cache.desc allocation are casted to hcc_32, hcc_u32
2.42
	if power loss at write descriptor then next time another block was allocated for new descriptor, fixed
	erase, copy previous data (file content) wasn't protected from other allocations, fixed
2.41
	retry 3 is added on read and verify on nand flash driver
	F_MAXPATH is checked in f_mkdir, f_rename and f_move
	f_mountdrive need task (uses gettask) and it also lock mutex after creation
	hcc_32 and hcc_u32 is added into udefs.h, cache handling is changed into this type because of 32bit compatibility

SAFE 2.40
=========
June 7, 2008

Fixes:
------

1. fsm_get_drive_list and fsm_get_drive_count return -1 if they have any error.
2. dataflash low level driver small bug with TimeOut repaired (no timeout before)
3. memory overwrite in dataflash driver is removed
4. NAND flash phy drivers same variable used in internal loop bug fixed
5. f->direntry was not initiated when a+ file started search if r file opens the same direntry - this could lead to corruption
6. a+ removed chain from mirror when r file was opened the same file, but when it was opened next time it doesn't copy back because r file was found, in this case 0xffff was in the chain and f_seek seeked out of range. In this situation file corruption and lost blocks can result.
7. time rollover issue fixed - see changes below.
8. Missing F_WFIND definition added to defs.h
9. f_poweroff is called before f_poweron is added in test_s.c to avoid potential malloc problems
10. ARCHIV bit was toggling on a directory name - corrected

Changes:
--------

1. fs_gettime, fs_getdate changed to fs_getcurrenttimedate, avoiding time/date roll-over possibility.
2. port_s.c is conditioned out completely when CAPI is used - meaning you only need to port the CAPI port files - all done in one place.
3. f_result parameters and type is set to int,int in header files as it was implemented in port_s.c
4. default MAXFILE is set to back to 4 in NAND flash driver
5. New atmel dataflash driver option using the NOR style instead of DFML - s_atmel.c.
6. Warnings in certain compilers removed
7. manual extensively revised.
8. first poweron test check is removed from test code, because it created errors when flash wasn't formatted at startup.
9. f_mountdrive checks passed buffer's address. If it is not aligned to sizeof(long) then signals error.
10. f_result parameters and type is set to int,int in header files as it was implemented in port_s.c
11. Manual extensively revised and many minor corrections.
12. f_getoem added to the api_s.h file
13. Unicode defintion option added to the api_s.h file.


SAFE 2.31
=========
November 5, 2007

Changes:

1. initialize bug for systems that do not zero memory correctly fixed
2. Bug in test code, testdrv_s for NOR flash, fixed - (descriptor's address are absolute)
3. All NAND flash drivers 'unsigned char page' is converted to long aligned unsigned long. 
Note: init function must be called before calling any sub function because init function initiate page=(unsigned char*)l_page buffer
4. Documentation Upgraded
5. Option for allowing compressed storage of data on a TI DSP added where the 16bit char would normally make storage inefficient. (This option must be purchased separately).

SAFE 2.26
=========
June 12, 2007


Important NAND Flash Issue Resolved:

An error has been discovered in the use of NAND flash in certain configurations which may cause corruption of the file system. This can happen if either the system is badly configured or the system has run out of usable NAND flash blocks (i.e. it is very heavily worn). The FS_NAND_RESERVEDBLOCK definition in nflshdrv.h should always be set to be at least 3 plus the number of separate directory blocks. It is recommended to allocate a couple more blocks to ensure that if bad blocks develop then the system can still function. In the event that the NAND becomes very worn and there are no reserved blocks left the system now returns F_ERR_UNUSABLE and becomes a read-only system.
If the value of FS_NAND_RESERVEDBLOCK is changed then it is recommended to re-format the drive.

New Features:

1. Allow a file to be opened for a or a+ simultaneously with open for r. 
The system accurately maintains the file pointer so that any changes to the file are readable.
2. f_getlasterror added - manual updated
3. f_enterFS function call added - this must be called before a task uses the file API.
4. Manual updated to include enter/release file system functions and also other improvements.
5. Default drive definition added, FS_CURRDRIVE in udefs_f.h
6. Test program added for driver level testing of the flash drivers - testdrv_s.c

Bug Fixes:

1. A bug in the multitask locking mechanism has been fixed.


SAFE 2.18
=========
April 26, 2007


New Features:

1.f_abortclose function is created
2.f_stat,f_wstat function is added
3.f_get_oem is added
4.Dataflash: block copy function is added for static wear
5.f_stat,f_wstat function is added
6.f_ftruncate and f_checkvolume are added
7. Nandflash pages are only written incrementally to support NAND flash that requires this.

Enhancements:

1. Major review of warning levels - implicit type conversions removed
2. All trailing white space removed.
3. Test suite updated for new features and error numbering cleaned up.
4. HCC_16_BIT_CHAR #define is added for TI DSP-s also in udefs.h
5. f_eof has error return values added
6. findfreeblock leaves prevbitsblock out from search
7. TI DSP ramdriver is added
8. Memory allocation for flash drivers redesigned to be more accurate.
9. NAND flash memset 0xff changed to 0xffff on TI DSP
10. Flash Driver version is stored to the descriptor

Bug fixes:

1. cannot create another file if a file already exists with same first 12 characters.
2. f_unmountdrive release mutex bug fix
3. Bug in settask fixed by use of global mutex.
4. f_unmountdrive mutex release problem fixed
5. global mutex is added to protect f_open and other functions from recursion
6. ramdrive problem with re-use - mirror fat erasing now done at format
7. NANDflash: 1bit error checking on ecc values is added to spare area
8. Problem when no more free sectors available fixed to return error.
9. Slow NAND flash boot after multiple power failures fixed by removal of signature on invalid descriptor blocks.
10. prevbitsblock is initiated to 0xffff (findfreeblock) which allows one extra block to be available to the fiel system.

Changes:

1. FILEAPI.H is renamed to API_S.H
2. DEFSOLD.H is removed
3. UDEFS.H is renamed to UDEFS_S.H
4. drivenumber is not initialized to A: (f_chdrive is needed)
5. fn_gettaskID is added into fsm.h
6. f_filelength return value changed to -1 if any error
7. sizeof(long) is added for TI DSP-s to the code
8. f_init has interger return value
9. SDATA_NR is added into nandflash driver for clarity
10. spi.c added empty chip select #defines for convenience.


EFFS-STD 2.04
=============
May 2006

Changes from 2.00 to 2.04

1. The file separator character can be chosen. Default is backslash.
2. Case-sensitive file name option
3. The next block allocation algorithm can be chosen from two methods by setting 
FSF_MOST_FREE_ALLOC most free alloc fsf.h. The default setting is recommended and 
should give better performance in most cases.
4. Bug fix in block allocation introduced in version 2.00 whereby in certain situations the file
system would appear prematurely full.
5. Manual corrections and additions to explain all definitions.

EFFS-STD 2.00
=============
October 2005

The file system format of this release is not compatible with previous releases - this has been done to enable
many performance enhancements. 
The API functions have been changed from fs_xxx to f_xxx to be compatible with ou other file APIs - 
a defsold.h header file has been provided for easy porting.

The main new features of this version are:

1. Much improved read, write and seek performance.
2. reduced RAM usage (use fsmem.exe to check new numbers)
3. Improved compatibility with EFFS-CAPI (common API for using EFFS-FAT and EFFS-STD in a single system).
4. More sample drivers have been added - particularly for NAND devices.

Functionally the file system is otherwise identical to previous versions.

============================================================================================================
