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
// $Revision: 1.14 $
// $Date: 2011/07/01 09:57:10EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// HEADER:  StartupFlashDisk.h
//
// Code Used to Start up the Flash Disk
// This was originally located in Main
//
// ***TODO:  This code is hardly startup code.  Most of it is a wrapper(or abstraction
// layer) for the flash fileystem.  We should move these functions into another module name
// and place them in a C++ class.
//------------------------------------------------------------------------------

#ifndef STARTUPFLASHDISK_H_
#define STARTUPFLASHDISK_H_

#define FILE_FW_UPDATE	"FirmwareUpdateInformation"

bool initflashdisk (bool flash_app_code);
void ShowDirectory(const char * pDir = "");
int GetFreeSpace(void);
int GetMaxOverheadPerFile();


void EncodeBuffer(const char *name, unsigned char *data, int size);


//WriteAndCloseFile
//	Inputs
//		const char *name the name of the file to write
//		unsigned int size size of data to write
//	Returns
//		>=0 length of data written
//		others <0 file system error i.e no space etc
//If this function fails to rewrite data it should leave old file unmodified

int WriteAndCloseFile(const char *name, const unsigned char *data, int size);

//ApendAndCloseFile
//	Inputs
//		const char *name the name of the file to write
//		unsigned int size size of data to write
//	Returns
//		>=0 length of data written
//		others <0 file system error i.e no space etc
//This function creates and opens file, appends data to it, should leave old file unmodified

int AppendAndCloseFile(const char *name, void *data, int size);


//ReadAndCloseFile
//	Inputs
//		const char *name the name of the file to read
//		unsigned int maxsize max size to read if 0 memory will be (re)allocated to fit
//	Outputs
//		unsigned char **Data address of pointer to destination for data
//	Returns
//		>=0 length of data read
//		-2 memory allocation or size too small
//		others <0 file system error i.e file not found
//
// Note:  It is up to the user to make sure **Data is freed.
// TODO:  replace this with something safer--use HSTRING.
int ReadAndCloseFile(const char *name,unsigned int maxsize,unsigned char **Data);


//EraseFile
//Inputs
//	const char * name name of the file to delete
//Returns
//	0 no error
// others file system error
int EraseFile(const char * name);

//RenameFile
//Inputs
//	const char * oldname name of the file to rename
//	const char * newname new name of file

//Returns
//	0 no error
// others file system error

int RenameFile(const char * oldname,const char * newname);

int CopyFile(const char * destname,const char *sourcename);

int GetFileLength(const char * FileName);


// int FormatVolume(int volume)
// format a drive 
// int volume =drive number to format
// <0 if fail 

int FormatVolume(int volume);
void CreateFolders(void);

int EraseEntireFlash(void);

bool flash_debug_on(void);

void SetFlashDebugState(bool);
#endif /*STARTUPFLASHDISK_H_*/

