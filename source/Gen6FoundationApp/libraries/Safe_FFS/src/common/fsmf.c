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
#pragma thumb
/****************************************************************************
 *
 * Multi task definitions for EFFS file system. If CAPI is used then
 * this file is not needed to be built in the project
 *
 ***************************************************************************/

#if (!FS_CAPI_USED)
FS_MULTI gl_multi[FS_MAXTASK];
#endif

/****************************************************************************
 *
 * _fsm_checksemaphore
 *
 * Checking if there is a user who already using file system.
 * Only one user could access file system, next user should
 * wait until 1st user finising using.
 *
 * INPUTS
 *
 * fm - multi structure pointer
 * vi - volume info
 *
 * RETURNS
 *
 * 0 - if access granted
 * 1 - if busy
 *
 ***************************************************************************/

int _fsm_checksemaphore(FS_MULTI *fm,FS_VOLUMEINFO *vi) 
{
	/* check if we already locked it */
	if (fm->pmutex==&vi->mutex) 
	{   
		return F_NO_ERROR;
	}

	if (!fs_mutex_get(&vi->mutex)) 
	{
		fm->pmutex=&vi->mutex;
		return F_NO_ERROR;
	}

	return F_ERR_BUSY;
}

/****************************************************************************
 *
 * _fsm_releasesemaphore
 *
 * Release used semaphore
 *
 * INPUTS
 *
 * fm - multi structure pointer
 *
 ***************************************************************************/

#if (!FS_CAPI_USED)

void _fsm_releasesemaphore(FS_MULTI *fm) 
{
	if (fm->pmutex) 
	{
		(void)fs_mutex_put(fm->pmutex);
		fm->pmutex=0;
	}
}

/****************************************************************************
 *
 * _fsm_gettask
 *
 * Searching for an entry where the task infos are
 *
 * INPUTS
 *
 * fm - where to store information
 *
 * RETURNS
 *
 * 0 - if found
 * 1 - if no more entry
 *
 ***************************************************************************/

int _fsm_gettask(FS_MULTI **fm) 
{
	long ID=fn_gettaskID();
	int a;

	/* search if ID exists */
	for (a=0; a<FS_MAXTASK;a++)
	{
		/* check if we have equal ID */
		if (gl_multi[a].ID==ID)
		{
			/* set multi entry */
			if (fm)
			{
				*fm=&gl_multi[a];
				gl_multi[a].pmutex=0;
			}
			return 0; /* free to use */
		}
	}

	return 1;
}

/****************************************************************************
 *
 * fsm_enterFS
 *
 * Put taskID into the task list, this function has to be called when a new
 * task wants to use FS (before call any FS function)
 *
 * RETURNS
 *
 * errorcode or 0 if successful
 *
 ***************************************************************************/

int fsm_enterFS()
{
	long ID=fn_gettaskID();
	int a,b;

	/* check if taskid is exist */
	if (!_fsm_gettask(0)) return F_NO_ERROR;

 	/* wait until global mutex is released */
	for (;;)
	{
		if (!fs_mutex_get(&effs_gmutex)) break;
	}
	/* search for a free entry for ID */
	for (a=0; a<FS_MAXTASK;a++)
	{
		/* check for empty space*/
		if (!gl_multi[a].ID)
		{
			gl_multi[a].ID=ID;		   /* set task ID */
			gl_multi[a].fs_curdrive=FS_CURRDRIVE; /* always set to the 1st drive */
			gl_multi[a].lasterror=0;

			/* reset all current working directories */
			for (b=0; b<FS_MAXVOLUME; b++)
			{
				gl_multi[a].fs_vols[b].cwd[0]=0;
			}

			/* set multi entry */
			gl_multi[a].pmutex=0;

			(void)fs_mutex_put(&effs_gmutex);
			return F_NO_ERROR; /* free to use */
		}
	}

	(void)fs_mutex_put(&effs_gmutex);
	return F_ERR_NOMOREENTRY; /* no space to set up new task */
}

/****************************************************************************
 *
 * fsm_releaseFS
 *
 * Release used task specified F_MULTI structure
 *
 * INPUT
 *
 * ID - which ID need to be released
 *
 ***************************************************************************/

void fsm_releaseFS(long ID) 
{
	int a;

	for (a=0; a<FS_MAXTASK;a++) 
	{
 		if (gl_multi[a].ID==ID) 
		{
			gl_multi[a].ID=0;
			return;
		}
	}
}

/****************************************************************************
 *
 * fsm_format
 *
 * format a volume (deletes every data)
 *
 * INPUTS
 *
 * drivenum - which drive need to be formatted
 *
 * RETURNS
 *
 * 0 - if successfully formatted
 * other - if any error
 *
 ***************************************************************************/

int fsm_format(int drivenum) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_format(fm,drivenum);

	_fsm_releasesemaphore(fm);

	return ret;
}

/****************************************************************************
 *
 * fg_mountdrive
 *
 * Mounting a new drive into filesystem
 *
 * INPUTS
 *
 * drivenum - which drive number is needed to be mount (0-A, 1-B, 2-C)
 * buffer - buffer address where internal memory allocation does
 * buffersize - size in bytes of the useable buffer
 * mountfunc - which drive to be mounted (e.g. fg_mount_ramdrive)
 * phyfunc - physical flash driver if its needed (RAMDRIVE has no physical!)
 *
 * RETURNS
 *
 * 0 - if no error and successfully mounted
 * other if any error FS_VOL_xxx error code
 *
 ***************************************************************************/

int fsm_mountdrive(int drivenum,void *buffer,long buffsize,FS_DRVMOUNT mountfunc,FS_PHYGETID phyfunc) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return FS_VOL_TASKNOTFOUND;

	ret=fg_mountdrive(fm,drivenum,buffer,buffsize,mountfunc,phyfunc);

	_fsm_releasesemaphore(fm);

	return ret;
}

/****************************************************************************
 *
 * fsm_unmountdrive
 *
 * unmount a drive, it seta the status of the volume to FS_VOL_NOTMOUNT
 * memory deallocation task is the host's task
 *
 * INPUTS
 *
 * drivenum - which drive needs to be unmounted
 *
 * RETURN
 *
 * error code or zero if it successful
 *
 ***************************************************************************/

int fsm_unmountdrive(int drivenum) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_unmountdrive(fm,drivenum);

	/* no release necessary because the mutex has been deleted and fm->pmutex is 0 */
	_fsm_releasesemaphore(fm);

	return ret;
}

/****************************************************************************
 *
 * fsm_get_drive_list
 *
 * getting drive list into buffer
 *
 * INPUTS
 *
 * buffer - where to store
 *
 * RETURNS
 *
 * return number of drives
 * -1 - if any error
 *
 ***************************************************************************/

int fsm_get_drive_list(int *buf) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) 
	{
		return -1;
	}

	ret=fg_get_drive_list(buf);

	_fsm_releasesemaphore(fm);

	return ret;
}

/****************************************************************************
 *
 * fsm_get_drive_count
 *
 * getting drive number
 *
 * RETURNS
 *
 * return number of drives
 * -1 - if any error
 *
 ***************************************************************************/

int fsm_get_drive_count(void) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) 
	{
		return -1;
	}

	ret=fg_get_drive_count();

	_fsm_releasesemaphore(fm);

	return ret;
}

/****************************************************************************
 *
 * fsm_getfreespace
 *
 * get free diskspace
 *
 * INPUTS
 *
 * drivenum - which drive free space is requested (0-A, 1-B, 2-C)
 * space - structure to fill space information
 *
 * RETURNS
 * 0 - if successful
 * other - see error codes
 *
 ***************************************************************************/

int fsm_getfreespace(int drivenum, FS_SPACE *space) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_getfreespace(fm,drivenum,space);

	_fsm_releasesemaphore(fm);

	return ret;
}

/****************************************************************************
 *
 * fsm_getdrive
 *
 * Get current drive number
 *
 * RETURNS
 *
 * with the current drive number (0-A, 1-B,...)
 *
 ***************************************************************************/

int fsm_getdrive(void) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) 
	{
		return -1; /* busy! */
	}

	if (fm->fs_curdrive==-1) 
	{
		return -1;
	}

	ret=fg_getdrive(fm);

	_fsm_releasesemaphore(fm);

	return ret;
}

/****************************************************************************
 *
 * fsm_chdrive
 *
 * Change current drive
 *
 * INPUTS
 *
 * drivenum - new current drive number (0-A, 1-B, ...)
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error (e.g invalid drive number)
 *
 ***************************************************************************/

int fsm_chdrive(int drivenum) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_chdrive(fm,drivenum);

	_fsm_releasesemaphore(fm);

	return ret;
}

/****************************************************************************
 *
 * fsm_getdcwd
 *
 * getting a drive current working directory
 *
 * INPUTS
 *
 * drivenum - drive number of which drive current folder needed
 * buffer - where to store current working folder
 * maxlen - buffer length (possible size is FS_MAXPATH)
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error (e.g invalid drive number)
 *
 ***************************************************************************/

int fsm_getdcwd(int drivenum, char *buffer, int maxlen ) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	buffer[0]=FS_SEPARATORCHAR;

	ret=fg_getdcwd(fm,drivenum,buffer+1,maxlen-1);

	_fsm_releasesemaphore(fm);

	return ret;
}

#ifdef HCC_UNICODE
int fsm_wgetdcwd(int drivenum, W_CHAR *buffer, int maxlen ) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	buffer[0]=FS_SEPARATORCHAR;
	buffer[1]=0;

	ret=fg_wgetdcwd(fm,drivenum,buffer+1,maxlen-1);

	_fsm_releasesemaphore(fm);

	return ret;
}
#endif

/****************************************************************************
 *
 * fsm_getcwd
 *
 * getting a current working directory of current drive
 *
 * INPUTS
 *
 * buffer - where to store current working folder
 * maxlen - buffer length (possible size is FS_MAXPATH)
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error
 *
 ***************************************************************************/

int fsm_getcwd(char *buffer, int maxlen ) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	if (!maxlen) return F_NO_ERROR;

	buffer[0]=FS_SEPARATORCHAR;

	ret=fg_getcwd(fm,buffer+1,maxlen-1);

	_fsm_releasesemaphore(fm);

	return ret;
}

#ifdef HCC_UNICODE
int fsm_wgetcwd(W_CHAR *buffer, int maxlen ) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	if (maxlen<2) return F_NO_ERROR;

	buffer[0]=FS_SEPARATORCHAR;
	buffer[1]=0;

	ret=fg_wgetcwd(fm,buffer+1,maxlen-1);

	_fsm_releasesemaphore(fm);

	return ret;
}
#endif

/****************************************************************************
 *
 * fsm_mkdir
 *
 * making a new directory
 *
 * INPUTS
 *
 * dirname - new directory name
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error
 *
 ***************************************************************************/

int fsm_mkdir(const char *dirname) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_mkdir(fm,dirname);

	_fsm_releasesemaphore(fm);

	return ret;
}

#ifdef HCC_UNICODE
int fsm_wmkdir(const W_CHAR *dirname) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_wmkdir(fm,dirname);

	_fsm_releasesemaphore(fm);

	return ret;
}
#endif

/****************************************************************************
 *
 * fsm_chdir
 *
 * change current working directory
 *
 * INPUTS
 *
 * dirname - new working directory name
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error
 *
 ***************************************************************************/

int fsm_chdir(const char *dirname) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_chdir(fm,dirname);

	_fsm_releasesemaphore(fm);

	return ret;
}

#ifdef HCC_UNICODE
int fsm_wchdir(const W_CHAR *dirname) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_wchdir(fm,dirname);

	_fsm_releasesemaphore(fm);

	return ret;
}
#endif

/****************************************************************************
 *
 * fsm_rmdir
 *
 * Remove directory, only could be removed if empty
 *
 * INPUTS
 *
 * dirname - which directory needed to be removed
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error
 *
 ***************************************************************************/

int fsm_rmdir(const char *dirname) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_rmdir(fm,dirname);

	_fsm_releasesemaphore(fm);

	return ret;
}

#ifdef HCC_UNICODE
int fsm_wrmdir(const W_CHAR *dirname) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_wrmdir(fm,dirname);

	_fsm_releasesemaphore(fm);

	return ret;
}
#endif

/****************************************************************************
 *
 * fsm_findfirst
 *
 * find a file(s) or directory(s) in directory
 *
 * INPUTS
 *
 * filename - filename (with or without wildcards)
 * find - where to store found file information
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error or not found
 *
 ***************************************************************************/

int fsm_findfirst(const char *filename,FS_FIND *find) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_findfirst(fm,filename,find);

	_fsm_releasesemaphore(fm);

	return ret;
}

#ifdef HCC_UNICODE
int fsm_wfindfirst(const W_CHAR *filename,FS_WFIND *find) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_wfindfirst(fm,filename,find);

	_fsm_releasesemaphore(fm);

	return ret;
}
#endif

/****************************************************************************
 *
 * fsm_findnext
 *
 * find further file(s) or directory(s) in directory
 *
 * INPUTS
 *
 * find - where to store found file information (findfirst should call 1st)
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error or not found
 *
 ***************************************************************************/

int fsm_findnext(FS_FIND *find) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_findnext(fm,find);

	_fsm_releasesemaphore(fm);

	return ret;
}

#ifdef HCC_UNICODE
int fsm_wfindnext(FS_WFIND *find) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_wfindnext(fm,find);

	_fsm_releasesemaphore(fm);

	return ret;
}
#endif

/****************************************************************************
 *
 * fsm_rename
 *
 * Rename file or directory
 *
 * INPUTS
 *
 * filename - filename or directory name (with or without path)
 * newname - new name of the file or directory (without path)
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error
 *
 ***************************************************************************/

int fsm_rename(const char *filename, const char *newname) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_rename(fm,filename,newname);

	_fsm_releasesemaphore(fm);

	return ret;
}

#ifdef HCC_UNICODE
int fsm_wrename(const W_CHAR *filename, const W_CHAR *newname) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_wrename(fm,filename,newname);

	_fsm_releasesemaphore(fm);

	return ret;
}
#endif

/****************************************************************************
 *
 * fsm_move
 *
 * Rename file or directory
 *
 * INPUTS
 *
 * filename - filename or directory name (with or without path)
 * newname - new name of the file or directory with path
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error
 *
 ***************************************************************************/

int fsm_move(const char *filename, const char *newname) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_move(fm,filename,newname);

	_fsm_releasesemaphore(fm);

	return ret;
}

#ifdef HCC_UNICODE
int fsm_wmove(const W_CHAR *filename, const W_CHAR *newname) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_wmove(fm,filename,newname);

	_fsm_releasesemaphore(fm);

	return ret;
}
#endif

/****************************************************************************
 *
 * fsm_delete
 *
 * delete a file
 *
 * INPUTS
 *
 * filename - file which wanted to be deleted (with or without path)
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error
 *
 ***************************************************************************/

int fsm_delete(const char *filename) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_delete(fm,filename);

	_fsm_releasesemaphore(fm);

	return ret;
}

#ifdef HCC_UNICODE
int fsm_wdelete(const W_CHAR *filename) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_wdelete(fm,filename);

	_fsm_releasesemaphore(fm);

	return ret;
}
#endif

/****************************************************************************
 *
 * fsm_filelength
 *
 * Get a file length
 *
 * INPUTS
 *
 * filename - file whose length is needed
 *
 * RETURNS
 *
 * length of the file
 *
 ***************************************************************************/

long fsm_filelength(const char *filename) 
{
	long ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) 
	{
		return -1; /* busy */
	}

	ret=fg_filelength(fm,filename);

	_fsm_releasesemaphore(fm);

	return ret;
}

#ifdef HCC_UNICODE
long fsm_wfilelength(const W_CHAR *filename) 
{
	long ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) 
	{
		return -1; /* busy */
	}

	ret=fg_wfilelength(fm,filename);

	_fsm_releasesemaphore(fm);

	return ret;
}
#endif

/****************************************************************************
 *
 * fsm_open
 *
 * open a file for reading/writing/appending
 *
 * INPUTS
 *
 * filename - which file need to be opened
 * mode - string how to open ("r"-read, "w"-write, "w+"-overwrite, "a"-append
 *
 * RETURNS
 *
 * FS_FILE pointer if successfully
 * 0 - if any error
 *
 ***************************************************************************/

FS_FILE *fsm_open(const char *filename,const char *mode) 
{
	FS_FILE *ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) 
	{
		return 0; /* busy */
	}

	ret=fg_open(fm,filename,mode);

	_fsm_releasesemaphore(fm);

	return ret;
}

#ifdef HCC_UNICODE
FS_FILE *fsm_wopen(const W_CHAR *filename,const W_CHAR *mode) 
{
	FS_FILE *ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) 
	{
		return 0; /* busy */
	}

	ret=fg_wopen(fm,filename,mode);

	_fsm_releasesemaphore(fm);

	return ret;
}
#endif

/****************************************************************************
 *
 * fsm_seek
 *
 * moves position into given offset in given file
 *
 * INPUTS
 *
 * filehandle - FS_FILE structure which file position needed to be modified
 * offset - relative position
 * whence - where to calculate position (FS_SEEK_SET,FS_SEEK_CUR,FS_SEEK_END)
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error
 *
 ***************************************************************************/

int fsm_seek(FS_FILE *filehandle,long offset,long whence) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_seek(fm,filehandle,offset,whence);

	_fsm_releasesemaphore(fm);

	return ret;
}

/****************************************************************************
 *
 * fsm_tell
 *
 * Tells the current position of opened file
 *
 * INPUTS
 *
 * filehandle - which file needs the position
 *
 * RETURNS
 *
 * position in the file from start
 *
 ***************************************************************************/

long fsm_tell(FS_FILE *filehandle) 
{
	long ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) 
	{
		return 0; /* busy */
	}

	ret=fg_tell(fm,filehandle);

	_fsm_releasesemaphore(fm);

	return ret;
}

 /****************************************************************************
 *
 * fsm_eof
 *
 * Tells if the current position is end of file or not
 *
 * INPUTS
 *
 * filehandle - which file needs the checking
 *
 * RETURNS
 *
 * 0 - if not EOF
 * other - if EOF or invalid file handle or busy
 *
 ***************************************************************************/

int fsm_eof(FS_FILE *filehandle) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND; /* busy */

	ret=fg_eof(fm,filehandle);

	_fsm_releasesemaphore(fm);

	return ret;
}

/****************************************************************************
 *
 * fsm_rewind
 *
 * set the fileposition in the opened file to the begining
 *
 * INPUTS
 *
 * filehandle - which file needs to be rewinded
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error
 *
 ***************************************************************************/

int fsm_rewind(FS_FILE *filehandle) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND; /* busy */

	ret=fg_rewind(fm,filehandle);

	_fsm_releasesemaphore(fm);

	return ret;
}

/****************************************************************************
 *
 * fsm_putc
 *
 * write a character into file
 *
 * INPUTS
 *
 * ch - what to write into file
 * filehandle - file where to write
 *
 * RETURNS
 *
 * with written character, or -1 if any error
 *
 ***************************************************************************/

int fsm_putc(int ch,FS_FILE *filehandle) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) 
	{
		return -1; /* busy */
	}

	ret=fg_putc(fm,ch,filehandle);

	_fsm_releasesemaphore(fm);

	return ret;
}

/****************************************************************************
 *
 * fsm_getc
 *
 * get a character from file
 *
 * INPUTS
 *
 * filehandle - file where to read from
 *
 * RETURNS
 *
 * with the read character or -1 if read was not successfully
 *
 ***************************************************************************/

int fsm_getc(FS_FILE *filehandle) 
{
	int ret;
	FS_MULTI *fm;
	if (_fsm_gettask(&fm)) 
	{
		return -1; /* busy */
	}

	ret=fg_getc(fm,filehandle);

	_fsm_releasesemaphore(fm);

	return ret;
}

/****************************************************************************
 *
 * fsm_read
 *
 * read data from file
 *
 * INPUTS
 *
 * buf - where the store data
 * size - size of items to be read
 * size_st - number of items need to be read
 * filehandle - file where to read from
 *
 * RETURNS
 *
 * with the number of read bytes
 *
 ***************************************************************************/

long fsm_read(void *buf,long size,long size_st,FS_FILE *filehandle) 
{
	long ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) 
	{
		return 0; /* busy */
	}

	ret=fg_read(fm,buf,size,size_st,filehandle);

	_fsm_releasesemaphore(fm);

	return ret;
}

/****************************************************************************
 *
 * fsm_write
 *
 * write data into file
 *
 * INPUTS
 *
 * buf - where the writing data is
 * size - size of items to be written
 * size_st - number of items need to be written
 * filehandle - file where to write
 *
 * RETURNS
 *
 * with the number of written bytes
 *
 ***************************************************************************/

long fsm_write(const void *buf,long size,long size_st,FS_FILE *filehandle) 
{
	long ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) 
	{
		return 0; /* busy */
	}

	ret=fg_write(fm,buf,size,size_st,filehandle);

	_fsm_releasesemaphore(fm);

	return ret;
}

/****************************************************************************
 *
 * fsm_flush
 *
 * flushing current content a file into physical.
 *
 * INPUTS
 *
 * filehandle - which file needs to be flushed
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error
 *
 ***************************************************************************/

int fsm_flush(FS_FILE *filehandle) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) 
	{
		return F_ERR_BUSY; /* busy */
	}

	ret=fg_flush(fm,filehandle);

	_fsm_releasesemaphore(fm);

	return ret;
}

/****************************************************************************
 *
 * fsm_abortclose
 *
 * abort and close a previously opened file
 *
 * INPUTS
 *
 * filehandle - which file needs to be aborted
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error
 *
 ***************************************************************************/

int fsm_abortclose(FS_FILE *filehandle)
{
	FS_MULTI *fm;
	int ret;

	if (_fsm_gettask(&fm)) 
	{
		return F_ERR_BUSY;
	}

	ret=fg_abortclose(fm,filehandle);

	_fsm_releasesemaphore(fm);

	return ret;
}

/****************************************************************************
 *
 * fsm_close
 *
 * close a previously opened file
 *
 * INPUTS
 *
 * filehandle - which file needs to be closed
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error
 *
 ***************************************************************************/

int fsm_close(FS_FILE *filehandle) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_close(fm,filehandle);

	_fsm_releasesemaphore(fm);

	return ret;
}

/****************************************************************************
 *
 * fsm_settimedate
 *
 * set a file time and date
 *
 * INPUTS
 *
 * filename - which file time and date wanted to be set
 * ctime - new ctime of the file
 * cdate - new cdate of the file
 *
 * RETURNS
 *
 * 0 - if successfully set
 * other - if any error
 *
 ***************************************************************************/

int fsm_settimedate(const char *filename,unsigned short ctime,unsigned short cdate) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_settimedate(fm,filename,ctime,cdate);

	_fsm_releasesemaphore(fm);

	return ret;
}

#ifdef HCC_UNICODE
int fsm_wsettimedate(const W_CHAR *filename,unsigned short ctime,unsigned short cdate) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_wsettimedate(fm,filename,ctime,cdate);

	_fsm_releasesemaphore(fm);

	return ret;
}
#endif

/****************************************************************************
 *
 * fsm_gettimedate
 *
 * get a file time and date
 *
 * INPUTS
 *
 * filename - which file time and date wanted to be retrive
 * pctime - ctime of the file where to store
 * pcdate - cdate of the file where to store
 *
 * RETURNS
 *
 * 0 - if successfully get
 * other - if any error
 *
 ***************************************************************************/

int fsm_gettimedate(const char *filename,unsigned short *pctime,unsigned short *pcdate) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_gettimedate(fm,filename,pctime,pcdate);

	_fsm_releasesemaphore(fm);

	return ret;
}

#ifdef HCC_UNICODE
int fsm_wgettimedate(const W_CHAR *filename,unsigned short *pctime,unsigned short *pcdate) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_wgettimedate(fm,filename,pctime,pcdate);

	_fsm_releasesemaphore(fm);

	return ret;
}
#endif

/****************************************************************************
 *
 * fssm_getpermission
 *
 * get a file permission
 *
 * INPUTS
 *
 * filename - which file/directory permission wanted to be retrive
 * psecure - pointer where to store permission
 *
 * RETURNS
 *
 * 0 - if successfully get
 * other - if any error
 *
 ***************************************************************************/

int fsm_getpermission(const char *filename, unsigned long *psecure) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_getpermission(fm,filename,psecure);

	_fsm_releasesemaphore(fm);

	return ret;
}

#ifdef HCC_UNICODE
int fsm_wgetpermission(const W_CHAR *filename, unsigned long *psecure) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_wgetpermission(fm,filename,psecure);

	_fsm_releasesemaphore(fm);

	return ret;
}
#endif

/****************************************************************************
 *
 * fsm_setpermission
 *
 * set a file permission
 *
 * INPUTS
 *
 * filename - which file/directory permission wanted to be retrive
 * secure - permission what to set for file
 *
 * RETURNS
 *
 * 0 - if successfully get
 * other - if any error
 *
 ***************************************************************************/

int fsm_setpermission(const char *filename, unsigned long secure) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_setpermission(fm,filename,secure);

	_fsm_releasesemaphore(fm);

	return ret;
}

#ifdef HCC_UNICODE
int fsm_wsetpermission(const W_CHAR *filename, unsigned long secure) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_wsetpermission(fm,filename,secure);

	_fsm_releasesemaphore(fm);

	return ret;
}
#endif

/****************************************************************************
 *
 * fsm_getlabel
 *
 * get a label of a media
 *
 * INPUTS
 *
 * drivenum - drive number which label's is needed
 * label - char pointer where to store label
 * len - length of label buffer
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fsm_getlabel(int drivenum, char *label, long len) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_getlabel(fm,drivenum,label,len);

	_fsm_releasesemaphore(fm);

	return ret;
}

/****************************************************************************
 *
 * fsm_setlabel
 *
 * set a label of a media
 *
 * INPUTS
 *
 * drivenum - drive number which label's need to be set
 * label - new label for the media
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fsm_setlabel(int drivenum, const char *label) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_setlabel(fm,drivenum,label);

	_fsm_releasesemaphore(fm);

	return ret;
}

/****************************************************************************
 *
 * fsm_truncate
 *
 * truncate a file to a specified length, filepointer will be set to the
 * end
 *
 * INPUTS
 *
 * filename - which file need to be truncated
 * length - length of new file
 *
 * RETURNS
 *
 * filepointer or zero if fails
 *
 ***************************************************************************/

FS_FILE *fsm_truncate(const char *filename,unsigned long length) 
{
	FS_FILE *ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) 
	{
		return 0;
	}

	ret=fg_truncate(fm,filename,length);

	_fsm_releasesemaphore(fm);

	return ret;
}

#ifdef HCC_UNICODE
FS_FILE *fsm_wtruncate(const W_CHAR *filename,unsigned long length) 
{
	FS_FILE *ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) 
	{
		return 0;
	}

	ret=fg_wtruncate(fm,filename,length);

	_fsm_releasesemaphore(fm);

	return ret;
}
#endif

 /****************************************************************************
 *
 * fsm_seteof
 *
 * set end of file at the current position
 *
 * INPUTS
 *
 * filehandle - file where to read from
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fsm_seteof(FS_FILE *filehandle) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND; /* busy */

	ret=fg_seteof(fm,filehandle);

	_fsm_releasesemaphore(fm);

	return ret;
}


/****************************************************************************
 *
 * fsm_stat
 *
 * get status information on a file
 *
 * INPUTS
 *
 * filename - which file time and date wanted to be retrive
 * stat - pointer where to store information
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fsm_stat(const char *filename,FS_STAT *stat)
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_stat(fm,filename,stat);

	_fsm_releasesemaphore(fm);

	return ret;
}

#ifdef HCC_UNICODE
int fsm_wstat(const W_CHAR *filename,FS_STAT *stat)
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_wstat(fm,filename,stat);

	_fsm_releasesemaphore(fm);

	return ret;
}
#endif


/****************************************************************************
 *
 * fsm_ftruncate
 *
 * truncate a file to a specified length, filepointer will be set to the
 * end
 *
 * INPUTS
 *
 * filehandle - which file need to be truncated
 * length - length of new file
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fsm_ftruncate(FS_FILE *filehandle,unsigned long length)
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_ftruncate(fm,filehandle,length);

	_fsm_releasesemaphore(fm);

	return ret;
}

/****************************************************************************
 *
 * fsm_checkvolume
 *
 * Deletes a previously initialized volume.
 *
 * INPUTS
 *
 * drvnumber - number of drive to check
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

int fsm_checkvolume(int drvnumber) 
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_checkvolume(fm,drvnumber);

	_fsm_releasesemaphore(fm);

	return ret;
}

/****************************************************************************
 *
 * fsm_get_oem
 *
 * Get OEM name
 *
 * INPUTS
 *
 * fm - multi structure pointer
 * drivenum - drivenumber
 * str - where to store information
 * maxlen - length of the buffer
 *
 * RETURN
 *
 * errorcode or zero if successful
 *
 ***************************************************************************/

int fsm_get_oem (int drivenum, char *str, long maxlen)
{
	int ret;
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND;

	ret=fg_get_oem (fm,drivenum,str,maxlen);

	_fsm_releasesemaphore(fm);

	return ret;
}

/****************************************************************************
 *
 * fsm_getlasterror
 *
 * returns with the last set error
 *
 * RETURNS
 *
 * error code which was set as lasterror
 *
 ***************************************************************************/

int fsm_getlasterror()
{
	FS_MULTI *fm;

	if (_fsm_gettask(&fm)) return F_ERR_TASKNOTFOUND; /* busy! */

	return fm->lasterror;
}

/****************************************************************************
 *
 * End of fsmf.c
 *
 ***************************************************************************/

#endif /* (!FS_CAPI_USED) */
