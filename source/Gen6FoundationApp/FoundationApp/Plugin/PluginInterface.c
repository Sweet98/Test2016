//======================================================================================
// PluginInterface.c
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Plugin/PluginInterface.c $
// $Revision: 1.13 $
// $Date: 2011/11/22 17:43:05EST $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Module Interface Description:
//    This file contains the interfaces between plugins and Matrix OS.
//
//---------------------------------------------------------------------------

#include "flashdrv.h"
#include "db_chrio.h"
#include <stdio.h>
#include <stdlib.h>
#include "PluginInterface.h"
#include "matrix-syscalls.h"
#include "PluginStandardSysErrno.h"
#include "ioman.h"
#include "beep.h"

#include "PluginProcessDecoding.h"
#include "PluginGpioInterface.h"

#if !defined( RFBASE )
#include "DecoderInterface.h"

/* these structures are copied from legacy products.
	At the earliest opportunity, change this functionality to use the new structure defined in DecoderInterface.h
*/

typedef enum {
	DECODE_TYPE_FAST = 0,
	DECODE_TYPE_FULL = 1
} DecodeType_t;

typedef struct {
   signed int x;
   signed int y;
} Corner_t;

typedef struct {
	Corner_t Corners[4];
	int D_Corner;
	int PdfRows;
	int PdfCols;
} IQImgInfo_t;

typedef struct {
   unsigned char *message; // Output Data String
	int length;	 				// Number of Data Characters
	char menuFlag;				// Boolean: is this for Menuing Purposes
	char mustStop;				// Boolean: signals UnStructured Append result
	char lastRecord;			// Boolean: is this the final "Result" from the current decode?
	char HHPcodeID;			// HHP internal Code (Symbology) ID
	char AIMcodeLetter;		// AIM/FACT/ISO "Symbology Identifier"
	char AIMcodeModifier;	// ... and "Modifier" character
	DecodeType_t DecodeType;
	IQImgInfo_t IQImgInfo;
} DecodeResult;
//end of old structures
#endif

#define PLUGIN_HEAP_SIZE (1024*1024)

// buffer for sbrk
static char heap_test[PLUGIN_HEAP_SIZE];

//#define PLUGIN_KEEP_IN_VOLUME
#define PLUGIN_KEEP_IN_FOLDER
#ifdef PLUGIN_KEEP_IN_VOLUME
#define PLUGIN_USER_DRIVENUM	1
#else
#define PLUGIN_USER_DRIVENUM	0
#endif

#define ENOSYS 88


/* Newlib definition for ARM */
/******************************************************************/
#define	O_RDONLY	0		/* +1 == FREAD */
#define	O_WRONLY	1		/* +1 == FWRITE */
#define	O_RDWR		2		/* +1 == FREAD|FWRITE */
#define	O_APPEND	0x0008
#define	O_CREAT		0x0200
#define	O_TRUNC		0x0400
#define	O_EXCL		0x0800
#define O_SYNC		0x2000

/*****************************************************************/
#if !defined( RFBASE )
extern int PluginDecoderMayContinue(void);
extern int PluginDecoderProcessResult(DecodeResult * msg);
extern unsigned int PluginDecoderMsTimer();
#endif

unsigned int Plugindiv64(unsigned long long *n, unsigned int divisor)
{
	unsigned int remainder = *n % divisor;
	*n = *n / divisor;
	return remainder;
}

void convert_errno(matrix_syscall_param *param, int error)
{
	switch(error){
		case F_NO_ERROR:
		case F_ERR_INVALIDDRIVE:
			param->err = ENOENT;
			break;
		case F_ERR_NOTFORMATTED:
			break;
		case F_ERR_INVALIDDIR:
		case F_ERR_INVALIDNAME:
		case F_ERR_NOTFOUND:
			param->err = ENOENT;
			break;
		case F_ERR_DUPLICATED:
			break;
		case F_ERR_NOMOREENTRY:
			param->err = ENFILE;
			break;
		case F_ERR_NOTOPEN:
			param->err = EBADF;
			break;
		case F_ERR_EOF:
			break;
		case F_ERR_RESERVED:
			break;
		case F_ERR_NOTUSEABLE:
			param->err = EINVAL;
			break;
		case F_ERR_LOCKED:
			break;
		case F_ERR_ACCESSDENIED:
			param->err = EACCES;
			break;
		case F_ERR_NOTEMPTY:
			param->err = ENOTEMPTY;
			break;
		case F_ERR_INITFUNC:
			break;
		case F_ERR_CARDREMOVED:	
			break;
		case F_ERR_ONDRIVE:
		case F_ERR_INVALIDSECTOR:
		case F_ERR_READ:
		case F_ERR_WRITE:
		case F_ERR_INVALIDMEDIA:
		case F_ERR_WRITEPROTECT:
		case F_ERR_INVFATTYPE:
		case F_ERR_MEDIATOOSMALL:
		case F_ERR_MEDIATOOLARGE:
		case F_ERR_NOTSUPPSECTORSIZE:
			param->err = EIO;
			break;
		case F_ERR_BUSY:
			param->err = EBUSY;
			break;
		case F_ERR_UNKNOWN:
			break;
		case F_ERR_DRVALREADYMNT:
			break;
		case F_ERR_TOOLONGNAME:
			param->err = ENAMETOOLONG;
			break;
		case F_ERR_NOTFORREAD:
		case F_ERR_DELFUNC:
		case F_ERR_ALLOCATION:
		case F_ERR_INVALIDPOS:
		case F_ERR_NOMORETASK:
		case F_ERR_NOTAVAILABLE:
		case F_ERR_TASKNOTFOUND:
		case F_ERR_UNUSABLE:
		case F_ERR_CRCERROR:
		default:
			break;
	}
	return;	/* Not implemented yet */
}



int matrix_syscall(struct matrix_syscall_param *param)
{
	int ret;
	char buf[64];
	
	switch(param->syscall_id){

		case MATRIX_READ:
			ret = wrapper_read((int)param->params[0], (void *)param->params[1], (int)param->params[2]);
			convert_errno(param, f_getlasterror());
			return ret;
		case MATRIX_WRITE:
			ret = wrapper_write((int)param->params[0], (void *)param->params[1], (int)param->params[2]);
			convert_errno(param, f_getlasterror());
			return ret;
		case MATRIX_OPEN:
			ret = wrapper_open((const char *)param->params[0], (int)param->params[1], (int)param->params[2]);
			convert_errno(param, f_getlasterror());
			return ret;
		case MATRIX_CLOSE:
			ret = wrapper_close((int)param->params[0]);
			convert_errno(param, f_getlasterror());
			return ret;

		case MATRIX_UNLINK:
			ret = wrapper_delete((char *)param->params[0]);
			convert_errno(param, f_getlasterror());
			return ret;

		case MATRIX_LSEEK:
			ret =wrapper_lseek((int)param->params[0], (int)param->params[1], (int)param->params[2]);
			convert_errno(param, f_getlasterror());
			return ret;
		case MATRIX_ISATTY:
			ret =wrapper_isatty((int)param->params[0]);
			return ret;
		case MATRIX_RENAME:
			ret =wrapper_rename((char*)param->params[0], (char*)param->params[1]);
			convert_errno(param, f_getlasterror());
			return ret;
			
		case MATRIX_REGAPI:
			{
				HONPluginRawInfo *rawinfo = (HONPluginRawInfo *)param->params[0];
				
				if(rawinfo->PluginRawClassType > HON_PLUGIN_DECODE){
					theStdInterface.printf("Unknown type of plugin\r\n");
					break;
				}

				if(param->params[1]){
					rawinfo->PluginApis = (void*)param->params[1];
				}
				
				return 0;
			}
		case MATRIX_IOBEEPLED:
			{
				unsigned int const *seq = (unsigned int const*)param->params[0];
				unsigned int len = (unsigned int)param->params[1];
				// Check the parameter sequence
				if(seq[len-1])		// not end of 0x00
					return -1;

				QueueBeepSequence(seq);
				return 0;
			}
		case MATRIX_IOINIT:
			{
				PluginGpio.GpioInit((int)param->params[0], (int)param->params[1]);
				return 0;
			}
		case MATRIX_IOREAD:
			{
				return PluginGpio.GpioRead();
			}
		case MATRIX_IOWRITE:
			{
				PluginGpio.GpioWrite((int)param->params[0], (int)param->params[1]);
				return 0;
			}
		
#ifdef PLUGIN_DECODER
#if !defined( RFBASE )
		case MATRIX_MAYCONTINUE:
			return PluginDecoderMayContinue();
			
		case MATRIX_PROCESSRST:
			return PluginDecoderProcessResult((DecodeResult *)(param->params[0]));

		case MATRIX_MSTIMER:
			return (int)PluginDecoderMsTimer();
#endif
#endif // PLUGIN_DECODER

		case MATRIX_DIV64:
			return (int)Plugindiv64((unsigned long long*)(param->params[0]), (unsigned int)(param->params[1]));
		case MATRIX_MALLOC:
			return (int)malloc((size_t)param->params[0]);
		case MATRIX_CALLOC:
			return (int)calloc((size_t)param->params[0], (size_t)param->params[1]);
		case MATRIX_FREE:
			free((void*)param->params[0]);
			return 0;
		case MATRIX_REALLOC:
			return (int)realloc((void *)param->params[0], (size_t)param->params[1]);

		case MATRIX_STAT:
			ret =wrapper_stat((char*)param->params[0], (FS_STAT*)param->params[1]);
			convert_errno(param, f_getlasterror());
			return ret;
		case MATRIX_FSTAT:
			ret =wrapper_fstat((int)param->params[0], (FS_STAT*)param->params[1]);
			convert_errno(param, f_getlasterror());
			return ret;
		case MATRIX_SBRK:
			ret =(int)wrapper_sbrk((int)param->params[0]);
			if(ret == -1)
				param->err = ESBRK;
			return ret;
		case MATRIX_VER:
			return wrapper_ver();


		case MATRIX_RESTART:
		case MATRIX_EXIT:
		case MATRIX_FORK:
		case MATRIX_LINK:	
		case MATRIX_EXECVE:
		case MATRIX_GETPID:
		case MATRIX_KILL:
		case MATRIX_TIMES:
		case MATRIX_BRK:
		case MATRIX_WAIT4:
			param->err = ENOSYS;
			break;
		default:
			sprintf(buf,"Unsupported syscall, Syscall ID: %d\r\n", param->syscall_id);
			theStdInterface.printf(buf);
			break;

	}
	return -1;
}


unsigned long MatrixSyscallAdd = (unsigned long)&matrix_syscall;


int flags_convert(int flags);

FS_FILE *filedescriptor_convert(int drivenum, int fd);
char *get_filename(int drivenum, int fd);


char *get_filename(int drivenum, int fd)
{
	char *ret = NULL;
	FS_FILEINT *fileint = 0;
	FS_MULTI *fm;
	FS_VOLUMEINFO *vi;
	int a;
	
	if (_fsm_gettask(&fm)) {
		return 0; /* busy */
	}

	_fg_getvolumeinfo(fm, drivenum, &vi);


	for (a=0; a<vi->maxfile; a++) 
	{
		if (vi->files[a].mode != FS_FILE_CLOSE && (fd-3) == a) 
		{
			fileint=&vi->files[a];
			break;
		}
	}
	
	_fsm_releasesemaphore(fm);

	if(fileint){
		ret = fileint->direntry->lname;
	}
	return ret;
}

FS_FILE *filedescriptor_convert(int drivenum, int fd)
{
	FS_FILE *ret = 0;
	FS_FILEINT *fileint = 0;
	FS_MULTI *fm;
	FS_VOLUMEINFO *vi;
	int a;
	
	if (_fsm_gettask(&fm)) {
		return 0; /* busy */
	}

	_fg_getvolumeinfo(fm, drivenum, &vi);


	for (a=0; a<vi->maxfile; a++) 
	{
		if (vi->files[a].mode != FS_FILE_CLOSE && (fd-3) == a) 
		{
			fileint=&vi->files[a];
			break;
		}
	}
	
	_fsm_releasesemaphore(fm);

	if(fileint)
		ret = &fileint->file;
	return ret;
}



int flags_convert(int flags, char *oflags)
{
  switch (flags)
    {
	case O_RDONLY:							/* open for reading */
		oflags[0] = 'r';
		oflags[1] = 0;
		break;
	
	case O_RDWR:
		oflags[0] = 'r';
		oflags[1] = '+';
		oflags[2] = 0;
		break;
		
	case O_WRONLY | O_CREAT | O_TRUNC:		/* open for writing */
		oflags[0] = 'w';
		oflags[1] = 0;
		break;

	case O_RDWR | O_CREAT | O_TRUNC:
		oflags[0] = 'w';
		oflags[1] = '+';
		oflags[2] = 0;
		break;

	case O_WRONLY | O_CREAT | O_APPEND:		/* open for appending */
		oflags[0] = 'a';
		oflags[1] = 0;
		break;

	case O_RDWR | O_CREAT | O_APPEND:		
		oflags[0] = 'a';
		oflags[1] = '+';
		oflags[2] = 0;
		break;
	default:									/* illegal mode */
		return (0);
    }
	return 1;
}

int filename_convert(const char *file, char *name)
{
	if(*(file+1) == ':' && *(file) != 'A' && *(file) != 'a')
		return -1;

	if(*(file +1) == ':'){	
		while(*file == 'A' || *file == 'a' || *file == ':' 
#if F_SEPARATOR=='/'
			|| *file == '/' ){
#else
			|| *file == '\\'){
#endif
			file++;
		}
	}

#if F_SEPARATOR=='/'
		strcpy(name, "/PluginFolder/");
		strcpy(name+14, file);
#else
		strcpy(name, "\\PluginFolder\\");
		strcpy(name+14, file);
#endif
	return 0;
}


int wrapper_isatty(int fd)
{
  return (fd < 3);
}


int wrapper_rename(char *oldname, char *newname) 
{
	int cur_drive;
	char name[256];
	char *check_name;

#if defined(PLUGIN_KEEP_IN_VOLUME)
	/* File operations are constrained to be in the plug-in user volume */
	
	if(*(oldname+1) == ':' && *oldname != 'B' && *oldname != 'b')
		return -1;					//we cannot access to a file resides in volumes other than B

#elif defined(PLUGIN_KEEP_IN_FOLDER)
	/* FIXME: Currently we can only access files in root folder of plugins, restrict plugins to be in "/PluginFolder" */
	check_name = oldname;
	while(*check_name++){
#if F_SEPARATOR=='/'
		if(*check_name == '/')
#else
		if(*check_name == '\\')
#endif
		return -1;
	}
	if(filename_convert(oldname, name) < 0)
		return -1;
#endif

	cur_drive = f_getdrive();			//Store current drive
	f_chdrive(PLUGIN_USER_DRIVENUM);					//Change to plug-in user drive

	if(f_rename(name, newname)){
		f_chdrive(cur_drive);
		return -1;
	}

	f_chdrive(cur_drive);					//Back to former drive
	
	return 0;
}


int wrapper_stat(char *file, FS_STAT *fstat) 
{
	int cur_drive;
	char name[256];
	char * check_name;

#if defined(PLUGIN_KEEP_IN_VOLUME)
	/* File operations are constrained to be in the plug-in user volume */
	
	if(*(file+1) == ':' && *file != 'B' && *file != 'b')
		return -1;					//we cannot access to a file resides in volumes other than B
#elif defined(PLUGIN_KEEP_IN_FOLDER)
	/* FIXME: Currently we can only access files in root folder of plugins, restrict plugins to be in "/PluginFolder" */
	check_name = file;
	while(*check_name++){
#if F_SEPARATOR=='/'
		if(*check_name == '/')
#else
		if(*check_name == '\\')
#endif
		return -1;
	}
	if(filename_convert(file, name) < 0)
		return -1;
#endif

	cur_drive = f_getdrive();			//Store current drive
	f_chdrive(PLUGIN_USER_DRIVENUM);					//Change to plug-in user drive


	if(f_stat(name, fstat)){
		f_chdrive(cur_drive);
		return -1;
	}

	f_chdrive(cur_drive);					//Back to former drive
	
	return 0;
}




int wrapper_delete(char *file) 
{
	int cur_drive;
	char name[256];
	char *check_name;

#if defined(PLUGIN_KEEP_IN_VOLUME)
	/* File operations are constrained to be in the plug-in user volume */
	
	if(*(file+1) == ':' && *file != 'B' && *file != 'b')
		return -1;					//we cannot access to a file resides in volumes other than B
#elif defined(PLUGIN_KEEP_IN_FOLDER)
	/* FIXME: Currently we can only access files in root folder of plugins, restrict plugins to be in "/PluginFolder" */
	check_name = file;
	while(*check_name++){
#if F_SEPARATOR=='/'
		if(*check_name == '/')
#else
		if(*check_name == '\\')
#endif
		return -1;
	}
	if(filename_convert(file, name) < 0)
		return -1;		
#endif

	cur_drive = f_getdrive();			//Store current drive
	f_chdrive(PLUGIN_USER_DRIVENUM);					//Change to plug-in user drive

	if(f_delete(name)){
		f_chdrive(cur_drive);
		return -1;
	}
	
	f_chdrive(cur_drive);					//Back to former drive
	
	return 0;
}



int wrapper_fstat(int file, FS_STAT * fstat)
{
	return wrapper_stat(get_filename(PLUGIN_USER_DRIVENUM, file), fstat);
}


int wrapper_lseek(int file, int offset, int whence)
{
	FS_FILE *filehandler;
	int ret;
	
	if(file < 3){
		/* TODO: handle stdin, stdout and stderror */
		return -1;
	}

	filehandler = filedescriptor_convert(PLUGIN_USER_DRIVENUM, file);

	if(!filehandler)
		return -1;

	ret = f_seek(filehandler, offset, whence);
	
	if(!ret)
		return f_tell(filehandler);
	else
		return -1;

}

int wrapper_close (int file)
{
	FS_FILE *filehandler;
	int ret;
	
	if(file < 3){
		/* TODO: handle stdin, stdout and stderror */
		;
	}

	filehandler = filedescriptor_convert(PLUGIN_USER_DRIVENUM, file);

	if(!filehandler)
		return -1;

	ret = f_close(filehandler);


	if(!ret)
		return ret;
	else
		return -1;
		
}


int wrapper_read(int file, void *buf, int count)
{
	FS_FILE *filehandler;
	if(file < 3){
		/* TODO: handle stdin, stdout and stderror */
		return -1;
	}

	filehandler = filedescriptor_convert(PLUGIN_USER_DRIVENUM, file);

	if(!filehandler)
		return -1;

	return f_read(buf, 1, count, filehandler);
	
}


int wrapper_write(int file, void *buf, int count)
{
	FS_FILE *filehandler;
	if(file < 3){
		/* TODO: handle the stdin, stdout and stderror */
		if(file != 0){
			*((char*)buf + count) = 0;
			if(GET_SETTING(EnableOutputDebugInformations)){
				PLUGIN_TRACE(((const char*)buf));
			}
			return count;
		}else
			return -1;
	}

	filehandler = filedescriptor_convert(PLUGIN_USER_DRIVENUM, file);

	if(!filehandler)
		return -1;

	return f_write(buf, 1, count, filehandler);
}




int wrapper_open(const char *file, int flags, int mode)
{
	FS_FILE *pfile;
	int cur_drive;
	char oflags[16];
	char name[256];
	F_FIND find;
	char *check_name;


#if defined(PLUGIN_KEEP_IN_VOLUME)
	/* File operations are constrained to be in the plug-in user volume */
	
	if(*(file+1) == ':' && *file != 'B' && *file != 'b')
		return -1;								//we cannot access to a file resides in volumes other than B
#elif defined(PLUGIN_KEEP_IN_FOLDER)
	/* FIXME: Currently we can only access files in root folder of plugins, restrict plugins to be in "/PluginFolder" */
	check_name = (char *)file;
	while(*check_name++){
#if F_SEPARATOR=='/'
		if(*check_name == '/')
#else
		if(*check_name == '\\')
#endif
		return -1;
	}
	if(filename_convert(file, name) < 0)
		return -1;		
#endif


	cur_drive = f_getdrive();			//Store current drive
	f_chdrive(PLUGIN_USER_DRIVENUM);					//Change to plug-in user drive
	
	/* Convert POSIX flags to Safe_FFS flags */
	if(!flags_convert(flags, oflags))
		return -1;
	
	/* Check if the memory for Plugin users is enough 
	    FIXME: 
	    	1. This check is put in open routine and we just make it happen when
	    	    we need to open a writable file. And sometimes perhaps a modification
	    	    of a file might even enhance the available memory
	    	 2. The check assume that all system files are all reside in the root directory
	    	 		space.used - total file size in root = plugin used
	 */

	if(oflags[0] != 'r' && oflags[1] != 0){
		int totalsize = 0;
		int pluginused;
		if (!f_findfirst("A:*.*",&find)){
			do{
				totalsize += find.filesize;
			}while (!f_findnext(&find));
		}
		F_SPACE space;
		if (!f_getfreespace(f_getdrive(),&space)){
			pluginused = space.used - totalsize;
		}else{
			return -1;
		}
		
		if(pluginused > 2*1024*1024){
			return -1;
		}
	}

	
	pfile = f_open(name, oflags);

	f_chdrive(cur_drive);					//Back to former drive

	if(!pfile){
		return -1;
	}
	
	return (pfile->reference+3);		/* +3 avoid conflicting with stdin, stdout and stderror */
}


int wrapper_sbrk(int size) 
{
	static char *heap_end;
	char *prev_heap_end;

	if (heap_end == 0) {
		heap_end = heap_test;
	}

	prev_heap_end = heap_end;
	if (heap_end + size > &heap_test[(PLUGIN_HEAP_SIZE -1)])
		return -1;

	heap_end += size;

	return (int) prev_heap_end;
}

int wrapper_ver()
{
	// Return values other than -1 will cause to call sbrk.
	return 1;
}

//=================================================================================
//                       File Modification History
//===================================================================================



