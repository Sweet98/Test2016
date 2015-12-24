#ifndef _NUM_M25PXX_C_
#define _NUM_M25PXX_C_
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile
// $Revision: 1.10 $
// $Date: 2010/05/11 22:48:53EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// File:  num_m25pxx.c
//
// physical layer driver for safe ffs
// for numonyx m25p32 or winbond
//
//------------------------------------------------------------------------------


/****************************************************************************
 *
 *            Copyright (c) 2005-2008 by HCC Embedded
 *
 * This software is copyrighted by and is the sole property of
 * HCC.  All rights, title, ownership, or other interests
 * in the software remain the property of HCC.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of HCC.
 *
 * HCC reserves the right to modify this software without notice.
 *
 * HCC Embedded
 * Budapest 1133
 * Vaci ut 110
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/
#include <stdlib.h>
#include "num_m25pxx.h"
#include "mx25spi.h"
#ifndef SEPARATED_VOLUME_FOR_PLUGIN
#define SEPARATED_VOLUME_FOR_PLUGIN 0
#endif
//#include "elf_plugin.h"  

#pragma thumb
/****************************************************************************
 *
 *   num_m25pxx.c
 *
 *  some defintions
 *
 ***************************************************************************/




#if !SEPARATED_VOLUME_FOR_PLUGIN
/*  system config  */
/*  block 0	 - 64k reserved for booting  */
/*  block 1-126 	 - 126x64k blocks used for data storage  */
/*  block 127-134 - 8x8k blocks for descriptors  */


#define BLOCKSIZE	0x10000		/* 64k - size of file storage blocks */
#define BLOCKSTART	1		/*  logical number of first file storage block  */
#define MAXBLOCKS	(numblks-BLOCKSTART-DESCBLOCKS)		/*  number of logical blocks for file storage  */

#define DESCSIZE	0x4000		/*  16K descriptor blocks  */
#define DESCBLOCKSTART	(MAXBLOCKS+1)		/*  logical number of first descriptor block  */
#define DESCBLOCKS	2		/*  8x8k number of descriptor blocks  */
#define DESCCACHE	0x1000		/*  descriptor cache size  */


#define SECTORPERBLOCK	(BLOCKSIZE/SECTORSIZE)	/*  number of sectors per block  */
#else

#define BLOCKNUMPART2	32				/* 2M for plug-in user */

#define BLOCKSIZE	0x10000				/* 64k - size of file storage blocks */
#define BLOCKSTART1	1					/*  logical number of first file storage block  */
#define MAXBLOCKS1	(numblks-BLOCKSTART1-DESCBLOCKS-BLOCKNUMPART2)		/*  number of logical blocks for file storage  */

#define DESCSIZE	0x4000					/*  16K descriptor blocks  */
#define DESCBLOCKSTART1	(MAXBLOCKS1+1)		/*  logical number of first descriptor block  */
#define DESCBLOCKS	2					/*  8x8k number of descriptor blocks  */
#define DESCCACHE	0x1000					/*  descriptor cache size  */


#define SECTORPERBLOCK	(BLOCKSIZE/SECTORSIZE)	/*  number of sectors per block  */

#define BLOCKSTART2	(numblks - 32)		/*  logical number of first file storage block  */
#define MAXBLOCKS2	(numblks-BLOCKSTART2-DESCBLOCKS)		/*  number of logical blocks for file storage  */

#define DESCBLOCKSTART2	(MAXBLOCKS2+BLOCKSTART2)		/*  logical number of first descriptor block  */

#endif


/* Please check maximum storeable file and used sector number in FSmem.exe */


/****************************************************************************
 *
 * Commands and datas for flash
 *
 ***************************************************************************/


/****************************************************************************
 *
 * RemoveWriteProtect
 *
 * Remove write protection on chip select 0,
 * e.g. when flash is accessed by write
 *
 ***************************************************************************/

static void RemoveWriteProtect()
{}

/****************************************************************************
 *
 * SetWriteProtect
 *
 * Set write protection on chip select 0 for prevention flash from
 * more write accessing
 *
 ***************************************************************************/

static void SetWriteProtect()
{}

/****************************************************************************
 *
 * GetBlockAddr
 *
 * Get a logical block physical relative address in flash
 *
 * INPUTS
 *
 * block - block number
 * relsector - relative sector in the block (<sectorperblock)
 *
 * RETURNS
 *
 * relative physical address of block
 *
 ***************************************************************************/

static long GetBlockAddr(long block,long relsector)
{
	return block*BLOCKSIZE+relsector*SECTORSIZE;  /* 0-126 64K */
}


/****************************************************************************
 *
 * EraseFlash
 *
 * Erase a block in flash
 *
 * INPUTS
 *
 * block - which block needs to be erased
 *
 * RETURNS
 *
 * 0 - if ok
 * 1 - if there was any error with prompt
 *
 ***************************************************************************/

static int EraseFlash(long block)
{
	long addr=GetBlockAddr(block,0);
	RemoveWriteProtect();
	int ret =SpiEraseSector(addr);
	SetWriteProtect();
	return ret;		// return 0 if succsess
}

/****************************************************************************
 *
 * WriteFlash
 *
 * Writing (programming) Flash device
 *
 * INPUTS
 *
 * data - where data is
 * block - which block is programmed
 * relsector - relative sector in the block (<sectorperblock)
 * len - length of data
 * relpos - offset to data in block
 *
 * RETURNS
 *
 * 0 - if ok
 * 1 - if there was any error
 *
 ***************************************************************************/

static int WriteFlash(void *data, long block, long relsector, long len,long relpos)
{
	long dest=GetBlockAddr(block,relsector)+relpos;
	char *s=( char*)(data);
	int ret=1;
	RemoveWriteProtect();
	ret =SpiWriteMemory(dest,len,s);
	SetWriteProtect();
	return ret;
}

/****************************************************************************
 *
 * VerifyFlash
 *
 * Compares data with flash containes
 *
 * INPUTS
 *
 * data - where data is
 * block - which block is programmed
 * relsector - relative sector in the block (<sectorperblock)
 * len - length of data
 * relpos - offset to data in block
 *
 * RETURNS
 *
 * 0 - if ok
 * 1 - if there was any error
 *
 ***************************************************************************/

static int VerifyFlash(void *data, long block, long relsector,long size,long relpos)
{
	unsigned long dest=GetBlockAddr(block,relsector)+relpos;
	char *s=( char*)(data);
	return SpiMemVerify(dest,size,s);
}


/****************************************************************************
 *
 * ReadFlash
 *
 * read data from flash
 *
 * INPUTS
 *
 * data - where to store data
 * block - block number which block to be read
 * blockrel - relative start address in the block
 * datalen - length of data in bytes
 *
 * RETURNS
 * 0 - if sucessfully read
 * other - if any error
 *
 ***************************************************************************/

static int ReadFlash(void *data, long block, long blockrel, long datalen)
{
	unsigned long dest=GetBlockAddr(block,0)+blockrel;
	char *s= (char*)(data);
	return SpiMemRead(dest,datalen,s);
}

#if !SEPARATED_VOLUME_FOR_PLUGIN
/****************************************************************************
 *
 * fs_phy_nor_at49bn6416t
 *
 * Identify a flash and fills FS_FLASH structure with data
 *
 * INPUTS
 *
 * flash - structure which is filled with data of flash properties
 *
 * RETURNS
 *
 * 0 - if successfully
 * other if flash cannot be identified
 *
 ***************************************************************************/

int fs_phy_nor_num_m25pxx(FS_FLASH *flash)
{

	flash->ReadFlash=ReadFlash;       /* read content */
	flash->EraseFlash=EraseFlash;     /* erase a block */
	flash->WriteFlash=WriteFlash;     /* write content */
	flash->VerifyFlash=VerifyFlash;   /* verify content */


	RemoveWriteProtect();
	unsigned int cmd =0x9f000000;
	SpiXfr(2,1,&cmd,4,SPICTYP_WRITE,NULL);
	SetWriteProtect();
	cmd &=0x00ffffff;
	int numblks;
	switch (cmd)
	{
	case 0x202018:
		numblks=256;		//numonyx 128M
		break;
	case 0x207117:			// numonyx 64M	M25PX64
	case 0x010216:			// spansion 64M
	case 0xef3017:			// winbond 64M
	case 0xef4017:			// winbond 64M  W25Q64BV
		numblks =128;
		break;
	case 0x207116:			// numonyx 32M M25PX32
	case 0x010215:			// spansion32M
	case 0xef3016:			// winbond 32M
	case 0xef4016:                 //winbond 25Q32BV for low-end 2d ken@2011.10.11
		numblks=64;
		break;
	default:
		return 1;   /* unidentified or wrong device   */
	}
	flash->maxblock=MAXBLOCKS;	/* 126x64k filesystem, (1block reserved for boot!) */
	flash->blocksize=BLOCKSIZE;	/* 64k	 */
	flash->sectorsize=SECTORSIZE; 	/* 4k */
	flash->sectorperblock=SECTORPERBLOCK;
	flash->blockstart=BLOCKSTART;		   /* where 1st block starts, (1block reserved for boot) */
	flash->descsize=DESCSIZE;	   /* 8K 	 */
	flash->descblockstart=DESCBLOCKSTART; 		/* fat block start */
	flash->descblockend=DESCBLOCKSTART+DESCBLOCKS-1;   /* fat block end (2 FATS!!!) */
	flash->cacheddescsize=DESCCACHE;			/*  set the descriptor cache size	  */

	return 0; /* num_m25pxx */
}

#else

int fs_phy_nor_num_m25pxx_part1(FS_FLASH *flash)
{

	flash->ReadFlash=ReadFlash;       /* read content */
	flash->EraseFlash=EraseFlash;     /* erase a block */
	flash->WriteFlash=WriteFlash;     /* write content */
	flash->VerifyFlash=VerifyFlash;   /* verify content */


	RemoveWriteProtect();
	unsigned int cmd =0x9f000000;
	SpiXfr(2,1,&cmd,4,SPICTYP_WRITE,NULL);
	SetWriteProtect();
	cmd &=0x00ffffff;
	int numblks;
	switch (cmd)
	{
	case 0x202018:
		numblks=256;		//numonyx 128M
		break;
	case 0x202017:			// numonyx 64M	
	case 0x010216:			// spansion 64M
	case 0xef3017:			// winbond 64M
		numblks =128;
		break;
	case 0x202016:			// numonyx 32M
	case 0x010215:			// spansion32M
	case 0xef3016:			// winbond 32M
		numblks=64;
		break;
	default:
		return 1;   /* unidentified or wrong device   */
	}

	
	flash->maxblock=MAXBLOCKS1;	/* 126x64k filesystem, (1block reserved for boot!) */
	flash->blocksize=BLOCKSIZE;	/* 64k	 */
	flash->sectorsize=SECTORSIZE; 	/* 4k */
	flash->sectorperblock=SECTORPERBLOCK;
	flash->blockstart=BLOCKSTART1;		   /* where 1st block starts, (1block reserved for boot) */
	flash->descsize=DESCSIZE;	   /* 8K 	 */
	flash->descblockstart=DESCBLOCKSTART1; 		/* fat block start */
	flash->descblockend=DESCBLOCKSTART1+DESCBLOCKS-1;   /* fat block end (2 FATS!!!) */
	flash->cacheddescsize=DESCCACHE;			/*  set the descriptor cache size	  */

	return 0; /* num_m25pxx */
}

int fs_phy_nor_num_m25pxx_part2(FS_FLASH *flash)
{

	flash->ReadFlash=ReadFlash;       /* read content */
	flash->EraseFlash=EraseFlash;     /* erase a block */
	flash->WriteFlash=WriteFlash;     /* write content */
	flash->VerifyFlash=VerifyFlash;   /* verify content */


	RemoveWriteProtect();
	unsigned int cmd =0x9f000000;
	SpiXfr(2,1,&cmd,4,SPICTYP_WRITE,NULL);
	SetWriteProtect();
	cmd &=0x00ffffff;
	int numblks;
	switch (cmd)
	{
	case 0x202018:
		numblks=256;		//numonyx 128M
		break;
	case 0x202017:			// numonyx 64M	
	case 0x010216:			// spansion 64M
	case 0xef3017:			// winbond 64M
		numblks =128;
		break;
	case 0x202016:			// numonyx 32M
	case 0x010215:			// spansion32M
	case 0xef3016:			// winbond 32M
	case 0xef4016:                 //winbond 25Q32BV for low-end 2d ken@2011.10.11
		numblks=64;
		break;
	default:
		return 1;   /* unidentified or wrong device   */
	}
	flash->maxblock=MAXBLOCKS2;	/* 126x64k filesystem, (1block reserved for boot!) */
	flash->blocksize=BLOCKSIZE;	/* 64k	 */
	flash->sectorsize=SECTORSIZE; 	/* 4k */
	flash->sectorperblock=SECTORPERBLOCK;
	flash->blockstart=BLOCKSTART2;		   /* where 1st block starts, (1block reserved for boot) */
	flash->descsize=DESCSIZE;	   /* 8K 	 */
	flash->descblockstart=DESCBLOCKSTART2; 		/* fat block start */
	flash->descblockend=DESCBLOCKSTART2+DESCBLOCKS-1;   /* fat block end (2 FATS!!!) */
	flash->cacheddescsize=DESCCACHE;			/*  set the descriptor cache size	  */

	return 0; /* num_m25pxx */
}

#endif			/* #if !SEPARATED_VOLUME_FOR_PLUGIN */

/****************************************************************************
 *
 * GetFlashSize
 *
 * Get size of the serial flash 
 *
 * RETURNS
 *
 * bytes of the flash size
 * 0x100000 if flash cannot be identified
 *
 ***************************************************************************/
int GetFlashSize(void)
{

	RemoveWriteProtect();
	unsigned int cmd =0x9f000000;
	SpiXfr(2,1,&cmd,4,SPICTYP_WRITE,NULL);
	SetWriteProtect();
	cmd &=0x00ffffff;

	int size = 1;
	switch (cmd)
	{
	case 0x202018:
		size = 16;		//numonyx 128Mbit
		break;
	case 0x202017:			// numonyx 64Mbit	
	case 0x010216:			// spansion 64Mbit
	case 0xef3017:			// winbond 64Mbit
	case 0xef4017:			// winbond 64Mbit  W25Q64BV
		size = 8;
		break;
	case 0x202016:			// numonyx 32Mbit
	case 0x010215:			// spansion32Mbit
	case 0xef3016:			// winbond 32Mbit
	case 0xef4016:                 //winbond 25Q32BV for low-end 2d ken@2011.10.11
		size = 4;
		break;
	default:
		size = 1;   /* unidentified or wrong device   */
		break;
	}

	return size * 0x100000;
}

/****************************************************************************
 *
 *  end of  num_m25pxx.c
 *
 ***************************************************************************/

#endif
