/**************************************************************************************/
//
// HIM 48X0G GENERATION 6 SOFTWARE
//
//  StartupFlashDisk
//  Date:       7/17/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX27
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile
// $Revision: 1.26 $
// $Date: 2011/07/01 09:57:09EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// File:  StartupFlashDisk.cpp
//
// Code Used to Start up the Flash Disk
// This was originally located in Main
//
//------------------------------------------------------------------------------
#if PLATFORM_CURRENT == imx25
#define SAFE_FFS_USED 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include "StartupFlashDisk.h"
#include "ReportStartupTiming.h"
#include "flashapplicationcode.h"
#include "db_sharedtable.h"
#include "db_chrio.h"
#include "MatrixHeader.h"

#ifndef SAFE_FFS_USED
#include "db_FlashDisk.h"
#include "db_flash.h"

#else
#include "num_m25pxx.h"
#include "flashdrv.h"
#include "mx25spi.h"
#include "FileHandler.h"
#endif

extern "C"	void GetBaudRateRegisters(int *, int *);


static void CreateAppUpdateInformation(void);
static 	char * p1buffer;
static bool Flash_debug_state=0;

extern FS_MULTI gl_multi[FS_MAXTASK];
//#define CHUCK_CFG
#ifdef CHUCK_CFG
static unsigned char pChar[FLASH_MAX_CONFIGURATION_LENGTH];
#endif
bool initflashdisk (bool flash_app_code)
{
#ifdef SAFE_FFS_USED
    int error=f_init();		// init filesytem ram variables
    int memsize;
    if (!error)
    {
        memsize=fs_getmem_flashdrive(fs_phy_nor_num_m25pxx); // find required space
        error=(memsize==0);
        if (!error)
        {
            p1buffer=(char*)malloc(memsize);
            if (!p1buffer)
            {
                error=2;
            }
        }
    }
	/*
	xputstring("initflashdisk\r\n");
	{
		int i=0;
		for(i=0;i<FS_MAXTASK;i++)
		{
			if(gl_multi[i].ID == 0)
				xputstring("gl_multi[i].ID = 0\r\n");
			else
				xputstring("gl_multi[i].ID = 1\r\n");
		}
	}
	*/
    if (!error)
    {
        f_enterFS();
		/*
		{
			int i=0;
			xputstring("f_enterFS\r\n");
			for(i=0;i<FS_MAXTASK;i++)
			{
				if(gl_multi[i].ID == 0)
					xputstring("gl_multi[i].ID = 0\r\n");
				else
					xputstring("gl_multi[i].ID = 1\r\n");
			}
		}*/
        error = f_mountdrive(0,p1buffer,memsize,fs_mount_flashdrive,fs_phy_nor_num_m25pxx);

        switch (error)
        {
            //		case FS_VOL_OK: //successfully mounted	use this line to format unconditionally for test
        case FS_VOL_NOTFORMATTED: //drive is mounted but drive is not formatted
            error =f_format(0);
        case FS_VOL_OK: //successfully mounted
#ifdef FORCE_FORMAT
        	error =f_format(0);
#endif
            error =f_chdrive(0);
            break;
        case FS_VOL_NOTMOUNT:// not mounted
        case FS_VOL_NOMEMORY: //not enough memory, drive is not mounted
        case FS_VOL_NOMORE: //no more drive available (FS_MAXVOLUME)
        case FS_VOL_DRVERROR: //mount driver error, not mounted
        default:
            free(p1buffer);
        }
    }
    if (error)
    {
        char buf[80];
        sprintf(buf,"fd mount error %d \r\n",error);
        xputstring(buf);
        return 0;
    }
    MarkTime(__LINE__,"FlashDiskCreationTime:");
    MarkTime(__LINE__,"FlashDiskLoadTime:");
	CreateFolders();
#else // hhp flasdisk

    // Create a working copy of the flash disk.
    int		NumberOfFlashSectors;
    int		*FlashSectorAddresses;
    int		*FlashSectorSizes;
    GetSectorInformation( &NumberOfFlashSectors, &FlashSectorAddresses, &FlashSectorSizes );
    FD_RESERVED_AREA	FlashDiskReservedAreas[2]={
                FlashSectorAddresses[0],(FlashSectorAddresses[1]-FlashSectorAddresses[0]) ,
                FlashSectorAddresses[1], (FlashSectorAddresses[2]-FlashSectorAddresses[1])
            };
    FlashDisk = new EMBEDDED_MATRIX_FLASH_DISK((UINT32) FLASH_READ_BASE, FLASH_SIZE, FLASH_DISK_BASE, FLASH_DISK_SECTOR_SIZE, 2,
                FlashDiskReservedAreas, FlashSectorAddresses, FlashSectorSizes, NumberOfFlashSectors );

    MarkTime(__LINE__,"FlashDiskCreationTime:");

    // If the boot code reported a valid flash disk, load from that location and don't look for alternates or validate the main application code.  Otherwise,
    // start at the default location and search for a good flash disk, where the main application code matches the flash disk header information.
    if((((UINT) FLASH_DISK_LOCATION) != 0xFFFFFFFF) && ((UINT) FLASH_DISK_LOCATION == ~((UINT)INVERSE_FLASH_DISK_LOCATION)))
    {
        FlashDisk->LoadFlashDiskFile((char *) FLASH_DISK_LOCATION, false);
    }
    else //else boot did not report a vlaid flash disk
    {
        FlashDisk->LoadFlashDiskFile(((char *) &FLASH_READ_BYTE_POINTER[FLASH_DISK_BASE]), true);
    } //endof else boot did not report a valid flash disk

    MarkTime(__LINE__,"FlashDiskLoadTime:");
#endif

    RESET_WATCHDOG_TIMER();
    if (flash_app_code && (SIZE_OF_OP_CODE_TO_STORE > 3) && (SIZE_OF_OP_CODE_TO_STORE<Sz_2M))      // assume op code is always <1MB fix this !!!
    {
        if(!RAMOnly((char *)(&RAM_RESET), ( APP_IMAGE_ZI_BASE - APP_IMAGE_RO_BASE)))
        {
            char msg[128];
            int AppSize= (COMPRESSOR_AND_COMPRESSED_APPLICATION_SIZE)?COMPRESSOR_AND_COMPRESSED_APPLICATION_SIZE:COMPRESSED_APPLICATION_SIZE;
            BYTE * AppStart=(APPLICATION_COMPRESSOR_LOCATION)?APPLICATION_COMPRESSOR_LOCATION: (BYTE *) COMPRESSED_APPLICATION_LOCATION;
            sprintf (msg, "Op code of size %d being stored to flash...\r\n",AppSize);
            xputstring (msg);
            StoreApplicationCodeToFlash(AppStart,AppSize,false,NULL);
            // Create MainApplicationUpdateInformation with the serial port parameters.  Query the UBIR to determine what boot code set these to.
            CreateAppUpdateInformation();
            xputstring ("Done!!\r\n");
        } //End RAM Only
    }
    MarkTime(__LINE__,"ApplicationCodeWriteTime:");
    if (flash_debug_on())
    {
        ShowDirectory();
    }
/*
	{
		int i=0;
		xputstring("END\r\n");
		for(i=0;i<FS_MAXTASK;i++)
		{
			if(gl_multi[i].ID == 0)
				xputstring("gl_multi[i].ID = 0\r\n");
			else
				xputstring("gl_multi[i].ID = 1\r\n");
		}
	}
*/
    return 1;
}
void CreateAppUpdateInformation(void)
{
    char	FlashDiskData[ 80 ];
    int		BaudIndex;
    unsigned buadrate=((GetBaudRate()+1200)/2400)*2400;

    switch(buadrate)
    {
    case 921600:
        BaudIndex = 12;
        break;

    case 460800:
        BaudIndex = 11;
        break;

    case 230400:
        BaudIndex = 10;
        break;

    case 115200:
    default:
        BaudIndex = 9;
        break;
    }
    int size = sprintf( FlashDiskData, "060702%d;0607032;0607040;0607050;0607060;0607070;0605000!", BaudIndex );

#ifdef SAFE_FFS_USED

    F_FILE * f =f_open(FILE_FW_UPDATE,"w");
    if (f)
    {
        f_write(FlashDiskData,1,size,f);
        f_close(f);
    }
#else
    FlashDisk->Write(FILE_FW_UPDATE, FlashDiskData, strlen( FlashDiskData ),MATRIX_FLASH_DISK::FD_OTHER, 0, 0, 0);
#endif
}
void ShowDirectory(const char * pDir/*=""*/)
{
#ifndef SAFE_FFS_USED
    HSTRING	Report(300, true);
    xputstring ("CreateFormattedDirectoryReport:\r\n");
    FlashDisk->CreateFormattedDirectoryReport(&Report);
    xputstring((char *) Report.Char, Report.Size);
#else

    F_FIND find;
    char buf[60];
	HSTRING path;
	path.Copy("A:\\");
	path.Append(pDir);
	path.Append("\\*.*");
    if (!f_findfirst((const char *)path.Char,&find))
    {
        do
        {
            sprintf (buf,"%-40.40s",find.filename);
            if (find.attr&F_ATTR_DIR)
            {
                sprintf (&buf[40]," .\r\n");
            }
            else
            {
                sprintf (&buf[40]," %d\r\n",find.filesize);
            }
            xputstring(buf);
        }
        while (!f_findnext(&find));
    }
    F_SPACE space;
    if (!f_getfreespace(f_getdrive(),&space))
    {
        sprintf(buf,"%d total,%d free,%d used\r\n",space.total,space.free,space.used);
        xputstring(buf);
    }
#endif

}


/*----------------------------------------------------------------------------------------
GetFreeSpace

  Gets the free space of the file system.
  
  Parameters:  none

  Returns:  Number of bytes free, 0 if an error.
----------------------------------------------------------------------------------------*/
int GetFreeSpace(void)
{
	int NumBytesFree;
	F_SPACE FreeSpace;
	
	if ( ! f_getfreespace(f_getdrive(), &FreeSpace) ) NumBytesFree = FreeSpace.free;
	else NumBytesFree = 0;
	
	return NumBytesFree;
}


/*----------------------------------------------------------------------------------------
GetMaxOverheadPerFile

  Gets the extra space the file system needs to store each file.
  
  FIXME:  I'm guessing on this one until I hear from the filesystem vendor.
  
  Parameters:  none

  Returns:  Number of bytes extra.
----------------------------------------------------------------------------------------*/
int GetMaxOverheadPerFile()
{
	return SECTORSIZE;
}


int AppendAndCloseFile(const char *name, void *data, int size)
{
    int WriteStatus =-1;
    F_FILE * f =f_open(name,"a+");
    if (!f)
    {
        f =f_open(name,"w");	// if the file does not exist create it
    }
    if (f)
    {
        WriteStatus = f_write(data,1,size,f);
        if (WriteStatus==size)
        {
            f_seteof(f);
            f_close(f);
        }
    }
    return WriteStatus;
}

bool IsConfigurationFile(const char *name)
{
	if(memcmp(name,"ProductConfig",13)==0||memcmp(name,"WorkingConfig",13)==0||memcmp(name,"WorkingConfig0",14)==0||memcmp(name,"WorkingConfig1",14)==0||memcmp(name,"DefaultConfig",13)==0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void EncodeBuffer(const char *name, unsigned char *data, int size)
{
	if(IsConfigurationFile(name))
	{
		int k=0;
		unsigned char cTmp=0;
		for(k=0;k<size;k++)
		{
			cTmp = *(data+k) + 0x80;
			*(data+k) = cTmp;
		}
	}
}


#ifdef CHUCK_CFG
int WriteAndCloseFile(const char *name, const unsigned char *data, int size)
{
    int WriteStatus =-1;
    F_FILE * f =f_open(name,"r+");
	theStdInterface.printf("WriteAndCloseFile--f_open:%s:0x%x\r\n",name,f);

	if(IsConfigurationFile(name))
	{
		int k=0;
		memset(pChar,0,FLASH_MAX_CONFIGURATION_LENGTH * 2);
		for(k=0;k<size;k++)
		{
			pChar[k] = *data++ + 0x80;
			//theStdInterface.printf("%c",pChar[k]);
		}
	}
	
    if (!f)
    {
        f =f_open(name,"w");	// if the file does not exist create it
        theStdInterface.printf("WriteAndCloseFile--Create--f_open:%s:0x%x\r\n",name,f);
    }
    if (f)
    {
    	if(IsConfigurationFile(name))
		{
			WriteStatus = f_write(pChar,1,size,f);
		}
		else
		{
		    WriteStatus = f_write(data,1,size,f);
		}
        if (WriteStatus==size)
        {
            f_seteof(f);
            f_close(f);
        }

		
    }
	if(IsConfigurationFile(name))
	{
		theStdInterface.printf("WriteAndCloseFile, size:%d, f:0x%x, return value:%d\r\n",size,f,WriteStatus);
	}
//	theStdInterface.printf("\r\nWrite Return Value is :%d\r\n",WriteStatus);
    return WriteStatus;
}

int ReadAndCloseFile(const char *name,unsigned int maxsize,unsigned char **Data)
{
    int ReadStatus= f_filelength(name);
	xputstring("ReadAndCloseFile\r\n");
	theStdInterface.printf("ReadAndCloseFile--f_filelength:%s:%d\r\n",name,ReadStatus);
    int size =ReadStatus;

	int i=0;
	for(i=0;i<FS_MAXTASK;i++)
	{
		theStdInterface.printf("gl_multi[%d].ID:%d\r\n",i,gl_multi[i].ID);
	}
    if (ReadStatus>0)
    {
        ReadStatus=-1;		//assume open failure
        F_FILE * f =f_open(name,"r");
		theStdInterface.printf("ReadAndCloseFile--f_open:%s:0x%x\r\n",name,f);
        if (f)				//open ok procede
        {
            ReadStatus=-2;	//assume alloc failure
            unsigned char *p=NULL;
            if (!maxsize)
            {
                p =(unsigned char *)realloc(*Data,size); // no max size specified allocate memory
            }
            else
            {
                if (size<=maxsize) // if enough room
                {
                    p=*Data;	//no allocation make p point to the users buffer
                }
            }
            if (p)
            {
				if(IsConfigurationFile(name))
				{
					memset(pChar,0,FLASH_MAX_CONFIGURATION_LENGTH * 2);
					
					ReadStatus = f_read(pChar,1,size,f);

					{
						int k=0;
						for(k=0;k<size;k++)
						{
							*p++ = pChar[k] + 0x80;
						}
					}
				}
				else
				{
					ReadStatus = f_read(p,1,size,f);
				}
				
                *Data=p;

            }
            f_close(f);
        }
		if(IsConfigurationFile(name))
		{
			theStdInterface.printf("ReadAndCloseFile, size:%d, f:0x%x, return value:%d\r\n",size,f,ReadStatus);
		}
    }
	
    return ReadStatus;
}
#else

	#if 1
		int WriteAndCloseFile(const char *name, const unsigned char *data, int size)
		{
			int WriteStatus =-1;
			F_FILE * f =f_open(name,"r+");
		/*	theStdInterface.printf("WriteAndCloseFile--f_open:%s:0x%x\r\n",name,f);
			if(IsConfigurationFile(name))
			{
				int k=0;
				unsigned char cTmp=0;
				for(k=0;k<size;k++)
				{
					theStdInterface.printf("%c",*data++);
				}
			}
		*/	
			if (!f)
			{
				f =f_open(name,"w");	// if the file does not exist create it
				//theStdInterface.printf("WriteAndCloseFile--Create--f_open:%s:0x%x\r\n",name,f);
			}
			if (f)
			{
				WriteStatus = f_write(data,1,size,f);
				if (WriteStatus==size)
				{
					f_seteof(f);
					f_close(f);
				}
				/*
				if(IsConfigurationFile(name))
				{
					theStdInterface.printf("WriteAndCloseFile, size:%d, f:0x%x, return value:%d\r\n",size,f,WriteStatus);
				}*/
			}
			return WriteStatus;
		}
	
		int ReadAndCloseFile(const char *name,unsigned int maxsize,unsigned char **Data)
		{
			int ReadStatus= f_filelength(name);
			int size =ReadStatus;
			//xputstring("ReadAndCloseFile\r\n");
			//theStdInterface.printf("ReadAndCloseFile--f_filelength:%s:%d\r\n",name,ReadStatus);
			//int i=0;
			//for(i=0;i<FS_MAXTASK;i++)
			//{
			//	theStdInterface.printf("gl_multi[%d].ID:%d\r\n",i,gl_multi[i].ID);
			//}
			if (ReadStatus>0)
			{
				ReadStatus=-1;		//assume open failure
				F_FILE * f =f_open(name,"r");
				//theStdInterface.printf("ReadAndCloseFile--f_open:%s:0x%x\r\n",name,f);
				if (f)				//open ok procede
				{
					ReadStatus=-2;	//assume alloc failure
					unsigned char *p=NULL;
					if (!maxsize)
					{
						p =(unsigned char *)realloc(*Data,size); // no max size specified allocate memory
					}
					else
					{
						if (size<=maxsize) // if enough room
						{
							p=*Data;	//no allocation make p point to the users buffer
						}
					}
					if (p)
					{
						ReadStatus = f_read(p,1,size,f);
						if(IsConfigurationFile(name))
						{
							int k=0;
							unsigned char cTmp=0;
							for(k=0;k<size;k++)
							{
								cTmp = *(p+k) + 0x80;
								*(p+k) = cTmp;
								//theStdInterface.printf("%c",*(p+k));
							}
						}
						*Data=p;
					}
					f_close(f);
					
					//if(IsConfigurationFile(name))
					//{
					//	theStdInterface.printf("ReadAndCloseFile, size:%d, f:0x%x, return value:%d\r\n",size,f,ReadStatus);
					//}
				}
			}
			return ReadStatus;
		}

	#else
	#endif
#endif

int EraseFile(const char * name)
{
    return 	f_delete(name);
}

int RenameFile(const char * oldname,const char * newname)
{
    return 	f_move(oldname,newname);

}
// int CopyFile(const char * destname,const char *sourcename)
// copy file in file system from sourcename to destname
// file must fit in memmory
// returns size of file if sucsess
// <0 if fail -1 file cant be opened -2 if buffer cant be malloced
int CopyFile(const char * destname,const char *sourcename)
{

    unsigned char *p=NULL;
    int Ret=ReadAndCloseFile(sourcename,0,&p);
    if (Ret >=0)
    {
        Ret =WriteAndCloseFile(destname,p,Ret);
    }
    if (p)
    {
        free(p);
    }
    return Ret;
}


/*----------------------------------------------------------------------------------------
GetFileLength

  Gets the length of the file on the file system.
  
  Note:  FFS documentation says f_filelength() will return 0 if the file does not exist.
  I tested this and it returns -1 and f_getlasterror() returns F_ERR_NOTFOUND.  

  Parameters:  
		FileName[in]:  name of file

  Returns:  >=0 if file was found, < 0 if an error.
----------------------------------------------------------------------------------------*/
int GetFileLength(const char * FileName)
{
	int length = f_filelength(FileName);
	return length;
}




int EraseEntireFlash(void)
{
    return SpiEraseAllFlash();
}

// int FormatVolume(int volume)
// format a drive
// int volume =drive number to format
// <0 if fail

int FormatVolume(int volume)
{
    int status = f_format(volume);
    return status;
}

/*----------------------------------------------------------------------------------------
CreateFolders

  Creates all necessary folders on the file system.
  

  Parameters:  none

  Returns:  nothing
----------------------------------------------------------------------------------------*/
void CreateFolders(void)
{
	f_mkdir(USER_FOLDER);
	f_mkdir(SYSTEM_FOLDER);
}



bool flash_debug_on(void)
{
    return Flash_debug_state;
}

void SetFlashDebugState(bool s)
{
    Flash_debug_state =s;
}

