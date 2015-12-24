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

#include "fsf.h"

/****************************************************************************
 *
 * g_adwcrctable32
 *
 * crc table for crc32 calculation
 *
 ***************************************************************************/

static const unsigned long g_adwcrctable32[256] = {  /* crc table for 32bit crc */
  0x00000000,0x77073096,0xee0e612c,0x990951ba,
  0x076dc419,0x706af48f,0xe963a535,0x9e6495a3,
  0x0edb8832,0x79dcb8a4,0xe0d5e91e,0x97d2d988,
  0x09b64c2b,0x7eb17cbd,0xe7b82d07,0x90bf1d91,
  0x1db71064,0x6ab020f2,0xf3b97148,0x84be41de,
  0x1adad47d,0x6ddde4eb,0xf4d4b551,0x83d385c7,
  0x136c9856,0x646ba8c0,0xfd62f97a,0x8a65c9ec,
  0x14015c4f,0x63066cd9,0xfa0f3d63,0x8d080df5,
  0x3b6e20c8,0x4c69105e,0xd56041e4,0xa2677172,
  0x3c03e4d1,0x4b04d447,0xd20d85fd,0xa50ab56b,
  0x35b5a8fa,0x42b2986c,0xdbbbc9d6,0xacbcf940,
  0x32d86ce3,0x45df5c75,0xdcd60dcf,0xabd13d59,
  0x26d930ac,0x51de003a,0xc8d75180,0xbfd06116,
  0x21b4f4b5,0x56b3c423,0xcfba9599,0xb8bda50f,
  0x2802b89e,0x5f058808,0xc60cd9b2,0xb10be924,
  0x2f6f7c87,0x58684c11,0xc1611dab,0xb6662d3d,
  0x76dc4190,0x01db7106,0x98d220bc,0xefd5102a,
  0x71b18589,0x06b6b51f,0x9fbfe4a5,0xe8b8d433,
  0x7807c9a2,0x0f00f934,0x9609a88e,0xe10e9818,
  0x7f6a0dbb,0x086d3d2d,0x91646c97,0xe6635c01,
  0x6b6b51f4,0x1c6c6162,0x856530d8,0xf262004e,
  0x6c0695ed,0x1b01a57b,0x8208f4c1,0xf50fc457,
  0x65b0d9c6,0x12b7e950,0x8bbeb8ea,0xfcb9887c,
  0x62dd1ddf,0x15da2d49,0x8cd37cf3,0xfbd44c65,
  0x4db26158,0x3ab551ce,0xa3bc0074,0xd4bb30e2,
  0x4adfa541,0x3dd895d7,0xa4d1c46d,0xd3d6f4fb,
  0x4369e96a,0x346ed9fc,0xad678846,0xda60b8d0,
  0x44042d73,0x33031de5,0xaa0a4c5f,0xdd0d7cc9,
  0x5005713c,0x270241aa,0xbe0b1010,0xc90c2086,
  0x5768b525,0x206f85b3,0xb966d409,0xce61e49f,
  0x5edef90e,0x29d9c998,0xb0d09822,0xc7d7a8b4,
  0x59b33d17,0x2eb40d81,0xb7bd5c3b,0xc0ba6cad,
  0xedb88320,0x9abfb3b6,0x03b6e20c,0x74b1d29a,
  0xead54739,0x9dd277af,0x04db2615,0x73dc1683,
  0xe3630b12,0x94643b84,0x0d6d6a3e,0x7a6a5aa8,
  0xe40ecf0b,0x9309ff9d,0x0a00ae27,0x7d079eb1,
  0xf00f9344,0x8708a3d2,0x1e01f268,0x6906c2fe,
  0xf762575d,0x806567cb,0x196c3671,0x6e6b06e7,
  0xfed41b76,0x89d32be0,0x10da7a5a,0x67dd4acc,
  0xf9b9df6f,0x8ebeeff9,0x17b7be43,0x60b08ed5,
  0xd6d6a3e8,0xa1d1937e,0x38d8c2c4,0x4fdff252,
  0xd1bb67f1,0xa6bc5767,0x3fb506dd,0x48b2364b,
  0xd80d2bda,0xaf0a1b4c,0x36034af6,0x41047a60,
  0xdf60efc3,0xa867df55,0x316e8eef,0x4669be79,
  0xcb61b38c,0xbc66831a,0x256fd2a0,0x5268e236,
  0xcc0c7795,0xbb0b4703,0x220216b9,0x5505262f,
  0xc5ba3bbe,0xb2bd0b28,0x2bb45a92,0x5cb36a04,
  0xc2d7ffa7,0xb5d0cf31,0x2cd99e8b,0x5bdeae1d,
  0x9b64c2b0,0xec63f226,0x756aa39c,0x026d930a,
  0x9c0906a9,0xeb0e363f,0x72076785,0x05005713,
  0x95bf4a82,0xe2b87a14,0x7bb12bae,0x0cb61b38,
  0x92d28e9b,0xe5d5be0d,0x7cdcefb7,0x0bdbdf21,
  0x86d3d2d4,0xf1d4e242,0x68ddb3f8,0x1fda836e,
  0x81be16cd,0xf6b9265b,0x6fb077e1,0x18b74777,
  0x88085ae6,0xff0f6a70,0x66063bca,0x11010b5c,
  0x8f659eff,0xf862ae69,0x616bffd3,0x166ccf45,
  0xa00ae278,0xd70dd2ee,0x4e048354,0x3903b3c2,
  0xa7672661,0xd06016f7,0x4969474d,0x3e6e77db,
  0xaed16a4a,0xd9d65adc,0x40df0b66,0x37d83bf0,
  0xa9bcae53,0xdebb9ec5,0x47b2cf7f,0x30b5ffe9,
  0xbdbdf21c,0xcabac28a,0x53b39330,0x24b4a3a6,
  0xbad03605,0xcdd70693,0x54de5729,0x23d967bf,
  0xb3667a2e,0xc4614ab8,0x5d681b02,0x2a6f2b94,
  0xb40bbe37,0xc30c8ea1,0x5a05df1b,0x2d02ef8d
};

/****************************************************************************
 *
 * fsm_crc32
 *
 * subfunction for crc32 calculation
 *
 * INPUTS
 *
 * dwcrc - previous crc number
 * bdata - next char to add into crc
 *
 * RETURNS
 *
 * new crc value
 *
 ***************************************************************************/

static hcc_u32 fsm_crc32(hcc_u32 dwcrc,unsigned char bdata)
{
	return g_adwcrctable32[(dwcrc ^ bdata) & 0xff] ^ (dwcrc >> 8);
}

/****************************************************************************
 *
 * fsm_calccrc32
 *
 * calculating an array crc32
 *
 * INPUTS
 *
 * dwcrc -  previous crc value (or FS_CRCINIT value)
 * vbuf  -  where the buffer starts for crc calculation
 * dwlen -  length of data
 *
 * RETURNS
 *
 * crc calculated value
 *
 ***************************************************************************/

hcc_u32 fsm_calccrc32(hcc_u32  dwcrc, const void *vbuf, unsigned long  dwlen)
{
	unsigned char *buf=(unsigned char*)vbuf;	/* byte pointer */

	while (dwlen--)
	{
#if HCC_16BIT_CHAR
		char lo=(*buf)&0x0ff;		   /* DSP has 16bit char */
		char hi=((*buf)>>8)&0x0ff;

		dwcrc=fsm_crc32(dwcrc,lo);
		dwcrc=fsm_crc32(dwcrc,hi);

		buf++;
#else
		dwcrc=fsm_crc32(dwcrc,*buf++);	/* calculate crc */
#endif
	}

	return dwcrc;                       /* return with crc */
}
#pragma thumb
/****************************************************************************
 *
 * fsm_memcpy
 *
 * standard memory copy
 *
 * INPUTS
 *
 * d - destination address
 * s - source address
 * len - length of copied data
 *
 ***************************************************************************/

void _fsm_memcpy(void *d, void *s, long len)
{
	char *dc=(char*)d;
	char *sc=(char*)s;

    while(len--)
	{
		*dc++=*sc++;
    }
}

/****************************************************************************
 *
 * fsm_memset
 *
 * standard memory set into a value
 *
 * INPUTS
 *
 * d - destination address
 * fill - what char used to fill
 * len - length of copied data
 *
 ***************************************************************************/

void _fsm_memset(void *d, unsigned char fill, long len)
{
	unsigned char *dc=(unsigned char*)d;

    while(len--)
	{
		*dc++=fill;
    }
}

/****************************************************************************
 *
 * fsm_allocdatasize
 *
 * convert a size into an aligned size
 *
 * INPUTS
 *
 * size - size of needed memory
 *
 * RETURNS
 *
 * aligned size
 *
 ***************************************************************************/

long fsm_allocdatasize(long size)
{
	if (size & (long)(sizeof(long)-1) )
	{
		size+=(long)sizeof(long)-(size & (long)(sizeof(long)-1));	/* align long */
	}

	return size;
}

/****************************************************************************
 *
 * fsm_allocdata
 *
 * allocation memory in volumeinfo structure
 *
 * INPUTS
 *
 * vi - volumeinfo structure where to allocate memory
 * size - size of needed memory
 *
 * RETURNS
 *
 * buffer pointer of allocated memory or NUL if no space for allocation
 *
 ***************************************************************************/

void *fsm_allocdata(FS_VOLUMEINFO *vi,long size) 
{
	void *ret=vi->buffer;

	if (size<0) return 0;      /*  cannot be allocated  */

	size=fsm_allocdatasize(size);

	vi->usedmem+=size;				/* addused memory */

	if (size>vi->freemem)  return 0; /* no more free mem */

	vi->freemem-=size;				/* set new free memory */
	vi->buffer+=size;				/* set new free pointer */

	return ret;
}

/****************************************************************************
 *
 * _fsm_towchar
 *
 * convert a string into wide char format
 *
 * INPUTS
 *
 * nconv - where to convert
 * s - original string
 *
 * RETURNS
 *
 * wide char string
 *
 ***************************************************************************/

#ifdef HCC_UNICODE
W_CHAR *_fsm_towchar(W_CHAR *nconv, const char *s) 
{
	int a;

	for (a=0; a<FS_MAXPATH-1; a++) 
	{
		unsigned char ch=(unsigned char)*s++;
		nconv[a]=ch;
		if (!ch) break;
	}

	nconv[a]=0; /* terminates it */
	return nconv;
}
#endif

/****************************************************************************
 *
 * _fsm_fromwchar
 *
 * convert a string back from wide char format
 *
 * INPUTS
 *
 * d - destination where to convert
 * s - original wide char format string
 *
 * RETURNS
 *
 * wide char string
 *
 ***************************************************************************/

#ifdef HCC_UNICODE
void _fsm_fromwchar(char *d, W_CHAR *s) 
{
	int a;

	for (a=0; a<FS_MAXPATH; a++) 
	{
		unsigned char ch=(unsigned char)*s++;
		*d++=ch;
		if (!ch) break;
	}
}
#endif

/****************************************************************************
 *
 * fsm_checkname
 *
 * checking filename for special characters
 *
 * INPUTS
 *
 * lname - filename
 *
 * RETURNS
 *
 * 0 - if no contains invalid character
 * other - if contains any invalid character
 *
 ***************************************************************************/

int fsm_checkname(const W_CHAR *lname) 
{
	for (;;) 
	{
		W_CHAR ch=*lname++;

		if (!ch) return 0;

		if (ch=='|') return 1;
		if (ch=='<') return 1;
		if (ch=='>') return 1;
		if (ch=='/') return 1;
		if (ch=='\\') return 1;
		if (ch==':') return 1;
	}
}

/****************************************************************************
 *
 * fsm_checknamewc
 *
 * checking filename and extension for wildcard character
 *
 * INPUTS
 *
 * lname - filename (e.g.: filename.txt)
 *
 * RETURNS
 *
 * 0 - if no contains wildcard character (? or *)
 * other - if contains any wildcard character
 *
 ***************************************************************************/

int fsm_checknamewc(const W_CHAR *lname) 
{
	for (;;) 
	{
		W_CHAR ch=*lname++;

		if (!ch) return 0;

		if (ch>0 && ch<0x20) return 1;
		if (ch=='?') return 1;
		if (ch=='*') return 1;

		if (ch=='|') return 1;
		if (ch=='<') return 1;
		if (ch=='>') return 1;
		if (ch=='/') return 1;
		if (ch=='\\') return 1;
		if (ch==':') return 1;
	}
}

/****************************************************************************
 *
 * fsm_setsectorsize
 *
 * set volume sector size (it also allocates write and read buffers)
 *
 * INPUTS
 *
 * vi - volumeinfo structure which volume needs to be set
 * sectorsize - sector size in bytes
 *
 * RETURNS
 *
 * 0 - if successfully
 * other if no memory space to allocating buffers
 *
 ***************************************************************************/

int fsm_setsectorsize(FS_VOLUMEINFO *vi,long sectorsize) 
{
	vi->sectorsize=sectorsize;
	return 0;
}

/****************************************************************************
 *
 * fsm_setmaxfile
 *
 * set and allocate internal FS_FILEINT structure in volume. This function
 * set the maximum useable file simultaniously open for read and write.
 *
 * INPUTS
 *
 * vi - volumeinfo structure which volume needs to be set
 * maxfile - maximum simultaniously open file number
 *
 * RETURNS
 *
 * 0 - if successfully
 * other if no memory space to allocating buffers
 *
 ***************************************************************************/

int fsm_setmaxfile(FS_VOLUMEINFO *vi,int maxfile) 
{
	int a;

	vi->maxfile=maxfile;

	vi->files=(FS_FILEINT *)fsm_allocdata(vi,(long)vi->maxfile*(long)sizeof(FS_FILEINT));
	if (!vi->files) return 1;

	for (a=0; a<vi->maxfile; a++) 
	{
		vi->files[a].buffer=(char*)fsm_allocdata(vi,vi->sectorsize);
		if (!vi->files[a].buffer) return 1;
	}

	return 0;
}

/****************************************************************************
 *
 * fsm_findfreeblock
 *
 * Search for a block which is completelly not used
 *
 * INPUTS
 *
 * vi - volumeinfo where to find free needsector
 * psector - where to store sector start of that block
 *
 * RETURNS
 *
 * 0 - if not found
 * 1 - if found
 *
 ***************************************************************************/

int fsm_findfreeblock(const FS_VOLUMEINFO *vi, unsigned short *psector) 
{
	unsigned short a;

	for (a=0; a<vi->flash->maxblock; a++) 
	{
		long b;

		if (vi->fatbitsblock==a) continue;  /*2004.07.28*/
		if (vi->prevbitsblock==a) continue; /* new additional avoiding blk */

		for (b=0; b<vi->flash->sectorperblock; b++) 
		{
			if (!fsm_checksectorfree(vi,vi->flash->sectorperblock*a+b)) goto notfree;
		}

		if (psector) *psector=(unsigned short)(a*vi->flash->sectorperblock);
		return 1;  /* this is free! */

notfree:;
	}

	return 0; /* not found */
}

/****************************************************************************
 *
 * _fs_checkfreeblocks
 *
 * it calculates the number of free blocks (blocks contain sectors!)
 * and compares with the safety number if there is minimum free blocks
 *
 * INPUTS
 *
 * vi - volume info pointer
 * sbnum - safety block number for comparing
 *
 * RETURNS
 * 0 - if there is enough free block
 * other - if any error
 *
 ***************************************************************************/

unsigned long _fs_checkfreeblocks(const FS_VOLUMEINFO *vi,unsigned long sbnum) 
{
	unsigned short a;
	unsigned long num=0;
	long sectornum=0;

	for (a=0; a<vi->flash->maxblock; a++,sectornum+=vi->flash->sectorperblock) 
	{
		long b;

		if (vi->fatbitsblock==a) continue;  /*2004.07.28*/
		if (vi->prevbitsblock==a) continue; /* new additional avoiding blk */

		for (b=0; b<vi->flash->sectorperblock; b++) 
		{
			if (!fsm_checksectorfree(vi,sectornum+b)) goto notfree;
		}

		num++;
		if (num>sbnum) return 0;
notfree:;
	}

	return 1; /* not enough free block */
}

/****************************************************************************
 *
 * fsm_swapbadblock
 *
 * set as badsectors a complete block, and swap a new useable block
 *
 * INPUTS
 *
 * vi - volume info where to store anything
 * badsector - start sector of badsectors to be set to bad
 *
 ***************************************************************************/

void fsm_swapbadblock(FS_VOLUMEINFO *vi,unsigned short badsector) 
{
	unsigned short tmpblock;
	long a;

	for (a=0; a<vi->flash->sectorperblock; a++) 
	{
		_fsm_cacheaddsptr(vi,&vi->_fat[badsector+a],FS_FAT_NOTUSED);	   /* set as not used sector for further */
	}

	tmpblock=vi->_blockindex[vi->flash->maxblock];
	_fsm_cacheaddsptr(vi,&vi->_blockindex[vi->flash->maxblock],vi->_blockindex[badsector/vi->flash->sectorperblock]);
	_fsm_cacheaddsptr(vi,&vi->_blockindex[badsector/vi->flash->sectorperblock],tmpblock);
}

/****************************************************************************
 *
 * _fsm_swapbadphy
 *
 * swap current logical block physical to another empty sector,
 * copy all used data into that new physical block
 *
 * INPUTS
 *
 * vi - volume info
 * sdata - signature to write (only NAND)
 * block - original block
 *
 * RETURNS
 *
 * 0 - if success
 * other if any error
 *
 ***************************************************************************/

static int _fsm_swapbadphy(FS_VOLUMEINFO *vi,long sdata,long block) 
{
	/* try to find another physical block to write data */
	for(;;) 
	{	
		unsigned short newsec;
		long tmpsec;
		unsigned short tmpblock;

		/*find a new free block*/
		if (!fsm_findfreeblock(vi,&newsec)) 
		{ 
			return 1; /* no free block to store */
		}

		for (tmpsec=0; tmpsec<vi->flash->sectorperblock; tmpsec++) 
		{
			_fsm_cacheaddsptr(vi,&vi->_fat[newsec+tmpsec],FS_FAT_NOTUSED); /* set all as not used sector in new area */
		}

		tmpblock=vi->_blockindex[newsec/vi->flash->sectorperblock];  /* get block phy! */
		_fsm_cacheaddlptr(vi,&vi->_wearlevel[tmpblock]);

		/* erase it 1st */
		if (!vi->flash->EraseFlash (vi->flash->blockstart+tmpblock)) 
		{ 
			int error=0;

			/* copy original sectors */
			for (tmpsec=0; tmpsec<vi->flash->sectorperblock; tmpsec++) 
			{ 
				/* check if data there */
				if (!fsm_checksectorfree(vi,vi->flash->sectorperblock*vi->fatbitsblock+tmpsec)) 
				{ 
					if (!fsm_readflash(vi,vi->rdbuffer,vi->flash->blockstart+block,tmpsec*vi->flash->sectorsize,vi->flash->sectorsize)) 
					{
						if (fsm_writeverifyflash (vi,vi->rdbuffer,vi->flash->blockstart+tmpblock,tmpsec,vi->flash->sectorsize,sdata)) 
						{
							error=1;
							break;
						}
					} /* if its got ECC error, system cannot repair but later it could be signalled as erased sector */
				}
			}

			if (!error) 
			{
				/* lets swap new and original physical address now */
				_fsm_cacheaddsptr(vi,&vi->_blockindex[newsec/vi->flash->sectorperblock],vi->_blockindex[vi->fatbitsblock]);
				_fsm_cacheaddsptr(vi,&vi->_blockindex[vi->fatbitsblock],tmpblock);
				/* vi->fatbitsblock still points to the same logical block, but its physical block is swapped*/
				return 0; /* new physical is swapped to, so lets go back to write again */
			}
		}
	}
}

/****************************************************************************
 *
 * _fsm_writenextsector
 *
 * store a sector and chain into sector chain. This is called from
 * lower level StoreSector driver function
 *
 * INPUTS
 *
 * vi - volume info where to store anything
 * file - filepointer
 * relsector - relative sector number in the current block
 * data - pointer to buffered data
 * sdata - signature data (important in NAND flash)
 *
 * RETURNS
 *
 * 0 - if success
 * other if any error
 *
 ***************************************************************************/

int _fsm_writenextsector(FS_VOLUMEINFO *vi,FS_FILEINT *file,long relsector,void *data,long sdata)
{
	long block;

	vi->fatbits[(unsigned long)relsector>>5]&=~(1UL<<((unsigned long)relsector&31)); /* clear erase bit on that sector */

	for (;;)
	{
		block=vi->_blockindex[vi->fatbitsblock]; /* get its physical*/

		if (!fsm_writeverifyflash(vi,data,vi->flash->blockstart+block,relsector,vi->flash->sectorsize,sdata))
		{
			fsm_addsectorchain(vi,file,(unsigned short)(relsector+vi->fatbitsblock*vi->flash->sectorperblock));
			return 0;
		}

		if (_fsm_swapbadphy(vi,sdata,block)) return 1; /* if problem cannot be solved then return with error */
	}
}

/****************************************************************************
 *
 * fsm_wearleveling
 *
 * search the less used block for next storage block
 *
 * INPUTS
 *
 * vi - volume info where to store anything
 *
 ***************************************************************************/

void fsm_wearleveling(FS_VOLUMEINFO *vi) 
{
	long freeblock=vi->_blockindex[vi->flash->maxblock];  /* get free block! */
	hcc_32 wear=vi->_wearlevel[freeblock];
	long findblock=-1;
	hcc_32 wear2=wear;
	long findblock2=-1;
	long a;

    if (!vi->flash->chkeraseblk) 
	{
		for (a=0; a<vi->flash->maxblock; a++) 
		{
			long b;
			hcc_32 newwear;

			if (vi->fatbitsblock==a) continue;  /*2004.07.28*/
			if (vi->prevbitsblock==a) continue; /* new additional avoiding blk */

			for (b=0; b<vi->flash->sectorperblock; b++) 
			{
				if (!fsm_checksectorfree(vi,vi->flash->sectorperblock*a+b)) goto notfree;
			}

			newwear=vi->_wearlevel[vi->_blockindex[a]];
			if (newwear < wear) 
			{
				wear=newwear;
				findblock=a;
			}
notfree:;
		}
	}
	else 
	{
		for (a=0; a<vi->flash->maxblock; a++) 
		{
			long pos=vi->flash->blockstart+vi->_blockindex[a];

			/* check if it is preerased */
			if (vi->flash->erasedblk[pos] && vi->flash->chkeraseblk[pos]) 
			{ 
				hcc_32 newwear=vi->_wearlevel[vi->_blockindex[a]];

				/* <= is needed because it may equal*/
				if (newwear <= wear) 
				{	 
					wear2=newwear;
					findblock2=a;
				}
			}
		}

    	for (a=0; a<vi->flash->maxblock; a++) 
		{
			long b;
			hcc_32 newwear;

			if (vi->fatbitsblock==a) continue;  /*2004.07.28*/
			if (vi->prevbitsblock==a) continue; /* new additional avoiding blk */

			for (b=0; b<vi->flash->sectorperblock; b++) 
			{
				if (!fsm_checksectorfree(vi,vi->flash->sectorperblock*a+b)) goto notfree2;
			}

	        vi->flash->chkeraseblk[vi->flash->blockstart+vi->_blockindex[a]]=1; /* request to preerase */

			newwear=vi->_wearlevel[vi->_blockindex[a]];
			if (newwear < wear) 
			{
				wear=newwear;
				findblock=a;
			}
notfree2:;
		}
	}

	if (findblock==-1) 
	{
		if (findblock2==-1)	return;
		findblock=findblock2; /* set this */
	}
	else if (findblock2!=-1) 
	{
		/* check if less */
		if (wear2<=wear) 
		{ 
			findblock=findblock2; /* set this */
		}
		else 
		{
			/* check if it is less than distance */
			if (wear2-wear < FS_MAXPREDISTANCE) 
			{ 
				findblock=findblock2; /* set this */
			}
		}
	}

	_fsm_cacheaddsptr(vi,&vi->_blockindex[vi->flash->maxblock],vi->_blockindex[findblock]);
	_fsm_cacheaddsptr(vi,&vi->_blockindex[findblock],(unsigned short)freeblock);
}

/****************************************************************************
 *
 * fsm_addsectorchain
 *
 * add a sector into sector chain
 *
 * INPUTS
 *
 * vi - volume info where to store anything
 * file - file pointer, which file belongs to
 * sector - which sector to add
 *
 ***************************************************************************/

void fsm_addsectorchain(const FS_VOLUMEINFO *vi,FS_FILEINT *file,unsigned short sector) 
{
	unsigned short currsec=*file->sector;

	if (currsec==FS_FAT_EOF) 
	{
		*file->sector=sector;				 /* set new allocated block */
		vi->fatmirror[sector]=FS_FAT_EOF;
	}
	else 
	{
		*file->sector=sector;				 /* set new allocated block */
		vi->fatmirror[sector]=vi->fatmirror[currsec]; /* set next as not eof */

		/* check if it was originally used */
		if (vi->_fat[currsec]!=FS_FAT_FREE) 
		{	   
			*file->discard=currsec;				   /* set previous sector into discard chain */
			vi->fatmirror[currsec]=FS_FAT_EOF;	   /* previous sector willbe the last */
			file->discard=&vi->fatmirror[currsec]; /* set new last discard sector */
		}
		else 
		{
			vi->fatmirror[currsec]=FS_FAT_FREE;	   /* set back to free if it was not used */
		}
	}
}

/****************************************************************************
 *
 * fsm_checksectorfree
 *
 * check a sector in fat and fatmirror if its really free sector
 *
 * INPUTS
 *
 * vi - volume info where to store anything
 * sector - which sector needed
 *
 * RETURNS
 *
 * 0 - if not free
 * 1 - if free
 *
 ***************************************************************************/

int fsm_checksectorfree(const FS_VOLUMEINFO *vi,long sector) 
{
	if (vi->_fat[sector]!=FS_FAT_FREE) return 0;
	if (vi->fatmirror[sector]!=FS_FAT_FREE) return 0;
	return 1;
}


/****************************************************************************
 *
 * fsm_checksectorbad
 *
 * check a sector in fat or fatmirror if its really a bad sector
 *
 * INPUTS
 *
 * vi - volume info where to store anything
 * sector - which sector needed
 *
 * RETURNS
 *
 * 0 - if not bad
 * 1 - if bad
 *
 ***************************************************************************/

int fsm_checksectorbad(const FS_VOLUMEINFO *vi,long sector) 
{
	if (vi->_fat[sector]==FS_FAT_NOTUSED) return 1;
	if (vi->fatmirror[sector]==FS_FAT_NOTUSED) return 1;
	return 0;
}


/****************************************************************************
 *
 * _f_toupper
 *
 * convert a char into uppercase
 *
 * INPUTS
 *
 * ch - input char to convert
 *
 * RETURN
 *
 * Capital letter or same char if no case sensitive is defined
 *
 ***************************************************************************/

W_CHAR _fsm_toupper(W_CHAR ch) 
{
#if (!FS_EFFS_CASE_SENSITIVE)
	if (ch>='a' && ch<='z') return (W_CHAR)((ch-'a')+'A');
#endif
	return ch;
}

/****************************************************************************
 *
 * _fsm_cacheaddvptr
 *
 * adding a pointer content into cached area, 4 bytes (long)
 *
 * INPUTS
 *
 * vi - volume info
 * ptr - pointer
 *
 ***************************************************************************/

static void _fsm_cacheaddvptr(FS_VOLUMEINFO *vi,void *ptr)
{
	if (vi->cache.desc)
	{
		vi->cache.status |= FS_STCACHE_DESCCHANGE;

		if (!(vi->cache.status&FS_STCACHE_FULL))
		{
			if (vi->cache.free>(hcc_32)(3*sizeof(hcc_32)))
			{
				hcc_u32 p=(((hcc_u32)ptr)-((hcc_u32)vi->fatdesc)) / sizeof(hcc_u32); /* align 4 */

				*vi->cache.ptr++=FS_CHACHETYPE_DESC; /* type */
				*vi->cache.ptr++=p;					/* relativ long addr */
				*vi->cache.ptr++=((hcc_u32*)(vi->fatdesc))[p]; /* value */
				vi->cache.free-=3*(hcc_32)sizeof(hcc_32); /* type,relativ long addr,value */
				vi->cache.status |= FS_STCACHE_ENTRY;
			}
			else
			{
				vi->cache.status |= FS_STCACHE_FULL;
			}
		}
	}
}

/****************************************************************************
 *
 * _fsm_cacheaddsptr
 *
 * compare and adding a short pointer content into cached area
 *
 * INPUTS
 *
 * vi - volume info
 * ptr - pointer
 * value - value what to set and compare
 *
 ***************************************************************************/

void _fsm_cacheaddsptr(FS_VOLUMEINFO *vi,unsigned short *ptr,unsigned short value)
{
	if (*ptr==value) return;
	*ptr=value;
	_fsm_cacheaddvptr(vi,ptr);
}

/****************************************************************************
 *
 * _fsm_cacheaddlptr
 *
 * increase value and adding a long pointer content into cached area
 *
 * INPUTS
 *
 * vi - volume info
 * ptr - pointer
 *
 ***************************************************************************/

void _fsm_cacheaddlptr(FS_VOLUMEINFO *vi,hcc_32 *ptr)
{
	*ptr=(*ptr)+1;
	_fsm_cacheaddvptr(vi,ptr);
}


/****************************************************************************
 *
 * _fsm_cacheaddde
 *
 * adding a direntry content into cached direntry list
 *
 * INPUTS
 *
 * vi - volume info
 * de - directory entry pointer
 *
 ***************************************************************************/

void _fsm_cacheaddde(FS_VOLUMEINFO *vi,void *de)
{
	if (vi->cache.desc)
	{
		int a;

		for (a=0; a<vi->cache.maxde; a++)
		{
			if (vi->cache.des[a]==de)
			{
				return;
			}
		}

		if (vi->cache.maxde==FS_MAXCACHEDE)
		{
		 	vi->cache.status |= FS_STCACHE_FULL;
			return;
		}

		vi->cache.des[vi->cache.maxde++]=de;
	}
}

/****************************************************************************
 *
 * _fsm_cachede
 *
 * internal function for getting direntry pointers out from the list and
 * add its contents into cached area
 *
 * INPUTS
 *
 * vi - volume info
 * de - direntry
 *
 ***************************************************************************/

static void _fsm_cachede(FS_VOLUMEINFO *vi,void *de)
{
	if (vi->cache.desc)
	{
		int desize=(sizeof(FS_DIRENTRY)+sizeof(hcc_32)-1) / sizeof(hcc_32);

		vi->cache.status |= FS_STCACHE_DIRCHANGE;

		if (!(vi->cache.status&FS_STCACHE_FULL))
		{
			if (vi->cache.free>(hcc_32)((hcc_32)desize*(hcc_32)sizeof(hcc_32)+2*(hcc_32)sizeof(hcc_32)))
			{
				hcc_u32 p=(((hcc_u32)de)-((hcc_u32)vi->direntries)) / sizeof(hcc_32); /* align 4 */
				hcc_u32 *lptr=&(((hcc_u32*)(vi->direntries))[p]);
				int a;

				*vi->cache.ptr++=FS_CHACHETYPE_DIR; /* type */
				*vi->cache.ptr++=p;					/* relativ long addr */

				for (a=0; a<desize; a++)
				{
					*vi->cache.ptr++=*lptr++; /* value */
				}

				vi->cache.free-=(hcc_32)desize*(hcc_32)sizeof(hcc_32)+2*(hcc_32)sizeof(hcc_32);
				vi->cache.status |= FS_STCACHE_ENTRY;
			}
			else
			{
				vi->cache.status |= FS_STCACHE_FULL;
			}
		}
	}
}

/****************************************************************************
 *
 * _fsm_cacheupdate
 *
 * This function must be called after all cache entry is added, before
 * writing cache content, it also sets size and crc value of cache
 *
 * INPUTS
 *
 * vi - volume info
 *
 ***************************************************************************/

void _fsm_cacheupdate(FS_VOLUMEINFO *vi)
{
	int a;
	hcc_u32 size;

	if (vi->cache.desc)
	{
		for (a=0; a<vi->cache.maxde; a++)
		{
			_fsm_cachede(vi,vi->cache.des[a]);
		}

		vi->cache.maxde=0;

		if (!(vi->cache.status&FS_STCACHE_FULL))
		{
			size=(((hcc_u32)(vi->cache.ptr))-((hcc_u32)(vi->cache.sptr)))+sizeof(hcc_u32); /* +CRC */
			vi->cache.sptr[0]=vi->cache.reference;
			vi->cache.sptr[1]=size;
			*vi->cache.ptr++=fsm_calccrc32(FS_CRCINIT,(char*)(vi->cache.sptr),size-sizeof(hcc_u32)); /* crc */
		}
	}
}

/****************************************************************************
 *
 * _fsm_cachepagecheck
 *
 * this function for checking page boundary for next cache entry
 *
 * INPUTS
 *
 * vi - volumeinfo
 *
 ***************************************************************************/

void _fsm_cachepagecheck(FS_VOLUMEINFO *vi)
{
	if (vi->flash->cachedpagesize)
	{
		hcc_32 cpos=(hcc_32)(vi->cache.sptr)-(hcc_32)(vi->cache.desc);
		hcc_32 cpage=vi->flash->cachedpagesize;

		cpos+=cpage-1;
		cpos/=cpage;
		cpos*=cpage; /* now it is aligned */

		vi->cache.free=vi->flash->cacheddescsize-(cpos+3*(hcc_32)sizeof(hcc_32));
		vi->cache.sptr=vi->cache.desc+(cpos/(hcc_32)sizeof(hcc_32)); /* desc is hcc_32 ptr */
		vi->cache.ptr=vi->cache.sptr+2;
	}
}

/****************************************************************************
 *
 * _fsm_cachenext
 *
 * setting up next cache entry into cache aread
 *
 * INPUTS
 *
 * vi - volumeinfo
 *
 ***************************************************************************/

void _fsm_cachenext(FS_VOLUMEINFO *vi)
{
	vi->cache.reference++;
	vi->cache.sptr=vi->cache.ptr;
	vi->cache.ptr=vi->cache.sptr+2;
	vi->cache.free-=3*(hcc_32)sizeof(hcc_32); /* ref,size .... crc */

	_fsm_cachepagecheck(vi);

	vi->cache.status=0;

	if (vi->cache.free<=0)
	{
		vi->cache.status=FS_STCACHE_FULL;
	}
}

/****************************************************************************
 *
 * _fsm_cachereset
 *
 * reset cache area, all contents must be written after doing this
 *
 * INPUTS
 *
 * vi - volume info
 *
 ***************************************************************************/

void _fsm_cachereset(FS_VOLUMEINFO *vi)
{
	vi->cache.reference=0;
	vi->cache.free=vi->cache.size-3*(hcc_32)sizeof(hcc_32); /* ref,size .... crc */
	vi->cache.sptr=vi->cache.desc;
	vi->cache.ptr=vi->cache.sptr+2;
	vi->cache.status=0;
}

/****************************************************************************
 *
 * _fsm_chacheupdatechanges
 *
 * this function is for restoring from the cache, this must be called
 * after the descriptor is loaded to set up all the changes
 *
 * INPUTS
 *
 * vi - volume info
 *
 * RETURNS
 *
 * 0 - if ok
 * 1 - if fatal error
 * 2 - last entry find
 * 3 - fatal error
 *
 ***************************************************************************/

int _fsm_chacheupdatechanges(FS_VOLUMEINFO *vi)
{
	hcc_u32 *head=vi->cache.sptr; /* set head position; */
	hcc_u32 *data=head+2; /* set data position */
	hcc_u32 crc32;
	hcc_u32 ref=head[0];
	hcc_32 size=(hcc_32)head[1];
	hcc_u32 *desc=(hcc_u32 *)(vi->fatdesc);
	hcc_u32 *deptr  =(hcc_u32 *)(vi->direntries);
	int desize=(sizeof(FS_DIRENTRY)+(sizeof(hcc_u32)-1))/sizeof(hcc_u32);

	if (ref==0xffffffffUL) return 2; /* last */

	if (ref!=vi->cache.reference) return 1;  /* reference is invalid */
	if (size-3*(hcc_32)sizeof(hcc_32)>vi->cache.free || size<(hcc_32)sizeof(hcc_32)) return 1;  /* size error invalid */

	crc32=fsm_calccrc32(FS_CRCINIT,(char*)head,(unsigned long)size-sizeof(hcc_u32));
	if (head[(size / (hcc_32)sizeof(hcc_32))-1]!=crc32) return 1; /* crc error */

	size-=3*(hcc_32)sizeof(hcc_32); /* ref, size ... crc */
	while (size)
	{
		switch (*data)
		{
			case FS_CHACHETYPE_DESC:
			{
				hcc_u32 addr=data[1];
				hcc_u32 val=data[2];
				desc[addr]=val;
				data+=3;
				size-=3*(hcc_32)sizeof(hcc_u32);
				break;
			}

			case FS_CHACHETYPE_DIR:
			{
				hcc_u32 addr=data[1];
				int a;

				data+=2;
				for (a=0; a<desize; a++)
				{
					deptr[addr++]=*data++;
				}

				size-=2*(hcc_32)sizeof(hcc_u32)+(hcc_32)sizeof(hcc_u32)*desize;
				break;
			}
			default:
				return 1; /* invalid entry */
		}

		if (size<0) return 1; /* invalid size */
	}

	if (crc32!=*data++) return 3; /* invalid crc fatal */

	vi->cache.sptr=data;
	vi->cache.ptr=vi->cache.sptr+2;
	vi->cache.free=vi->cache.size-(hcc_32)( ((hcc_u32)(vi->cache.sptr)-(hcc_u32)(vi->cache.desc))+3*(hcc_32)sizeof(hcc_u32) );
	vi->cache.reference=ref+1;

	_fsm_cachepagecheck(vi);

	if (vi->cache.free<=0)
	{
		vi->cache.status=FS_STCACHE_FULL;
		return 2; /* last */
	}

	return 0;
}

/****************************************************************************
 *
 * fsm_strlen
 *
 * calculate the length of the string
 *
 * INPUTS
 *
 * string - string to calculate length
 *
 * RETURNS
 *
 * length of the string
 *
 ***************************************************************************/

int _fsm_strlen(char *string)
{
	int length=0;

	while (*string++)
	{
		length++;
	}

	return length;
}

/****************************************************************************
 *
 * fg_setlasterror
 *
 * setting errorcode in multi structure, maybe functions is not used
 * depending on thse settings in udefs_f.h
 * fn_setlasterror differs only it has no return value.
 *
 * INPUTS
 *
 * fm - multi structure where to set
 * errorcode - errorcode to be set
 *
 * RETURNS
 *
 * simple return with errorcode
 *
 ***************************************************************************/

int fg_setlasterror (FS_MULTI *fm, int errorcode)
{
	fm->lasterror=errorcode;
	return errorcode;
}

void fg_setlasterror_noret (FS_MULTI *fm, int errorcode)
{
	fm->lasterror=errorcode;
}

/****************************************************************************
 *
 * fsm_readflash
 *
 * read data from flash with retry
 *
 * INPUTS
 *
 * vi - volume info
 * data - where to store data
 * block - block number which block to be read
 * blockrel - relative start address in the block
 * datalen - length of data in bytes
 *
 * RETURNS
 * 0 - if successfully read
 * other if any error
 *
 ***************************************************************************/

int fsm_readflash(const FS_VOLUMEINFO *vi, void *data, long block, long blockrel, long datalen) 
{
	int retry=3;
	int ret=1;

	while (retry--)
	{
		ret=vi->flash->ReadFlash(data,block,blockrel,datalen);

		/* check if no error */
		if (!ret)
		{
			return 0;
		}

		/* check if erased */
		if (ret==-1)
		{
			return ret;
		}
	}

	return ret; /* signal error */
}

/****************************************************************************
 *
 * WriteVerifyFlash
 *
 * Writing (programming) Flash device and verify its content with retry
 *
 * INPUTS
 *
 * vi - volumeinfo
 * data - where data is
 * block - which block is programmed
 * relsector - relative sector in the block (<sectorperblock)
 * len - length of data
 * sdata - signature data to block
 *
 * RETURNS
 *
 * 0 - if ok
 * other if any error
 *
 ***************************************************************************/

int fsm_writeverifyflash(const FS_VOLUMEINFO *vi,void *data, long block,long relsector, long size,long sdata) 
{

	int retry=3;
	int ret;

	ret=vi->flash->WriteFlash(data,block,relsector,size,sdata);
	if (ret)
	{
		return ret;
	}

	while (retry--)
	{
		ret=vi->flash->VerifyFlash(data,block,relsector,size,sdata);

		/* check if no error */
		if (!ret)
		{
			return 0;
		}

		/* check if erased */
		if (ret==-1)
		{
			return ret;
		}
	}

	return ret; /* signal error */
}


/****************************************************************************
 *
 * End of fsm.c
 *
 ***************************************************************************/
