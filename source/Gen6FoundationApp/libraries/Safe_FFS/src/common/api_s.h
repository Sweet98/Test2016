#ifndef HCC_API_S_H_
#define HCC_API_S_H_

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

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 *
 * File API structures
 *
 ***************************************************************************/

typedef unsigned short wchar;

/* Set UNICODE_DEF to 1 if unicode is used, other case set to 0 */
#define UNICODE_DEF	0

#if UNICODE_DEFS
#define W_CHAR wchar
#else
#define W_CHAR char
#endif

typedef struct {
	long reference;		/* reference which fileint used */
} FS_FILE;


typedef struct {
	int drivenum;				/*  0-A 1-B 2-C  */
	W_CHAR path [FS_MAXPATH];		/*   /directory1/dir2/   */
	W_CHAR lname[FS_MAXPATH];		/*  filename  */
	unsigned short dirnum;		/*  0xffff-root other case in subdir n  */
} FS_NAME;


typedef struct {
	char attr;					/* attribute of the file/entry */
	char filename[FS_MAXPATH];	/* file name+ext */

	unsigned short ctime;		/* creation time */
	unsigned short cdate;		/* creation date */

	long len;					/* length of file */

	unsigned long secure;		/* secure */

	FS_NAME findfsname;		   	/* find properties */
	unsigned short findpos;		/* find position */
} FS_FIND;


typedef struct {
	char attr;					/* attribute of the file/entry */
	W_CHAR filename[FS_MAXPATH];	/* file name+ext */

	unsigned short ctime;		/* creation time */
	unsigned short cdate;		/* creation date */

	long len;					/* length of file */

	unsigned long secure;		/* secure */

	FS_NAME findfsname;		   	/* find properties */
	unsigned short findpos;		/* find position */
} FS_WFIND;


typedef struct {
   unsigned long total;
   unsigned long free;
   unsigned long used;
   unsigned long bad;
} FS_SPACE;


/* Beginning of file */
#ifdef SEEK_SET
#define FS_SEEK_SET SEEK_SET
#else
#define FS_SEEK_SET 0
#endif

/* Current position of file pointer */
#ifdef SEEK_CUR
#define FS_SEEK_CUR SEEK_CUR
#else
#define FS_SEEK_CUR 1
#endif

/* End of file */
#ifdef SEEK_END
#define FS_SEEK_END SEEK_END
#else
#define FS_SEEK_END 2
#endif

#define FSSEC_ATTR_ARC      (0x20UL<<(31-6))
#define FSSEC_ATTR_DIR      (0x10UL<<(31-6))
#define FSSEC_ATTR_VOLUME   (0x08UL<<(31-6))
#define FSSEC_ATTR_SYSTEM   (0x04UL<<(31-6))
#define FSSEC_ATTR_HIDDEN   (0x02UL<<(31-6))
#define FSSEC_ATTR_READONLY (0x01UL<<(31-6))

typedef int (*FS_DRVMOUNT)(FS_VOLUMEDESC *vd,FS_PHYGETID phyfunc);
typedef int (*FS_PHYGETID)(FS_FLASH *flash);

/****************************************************************************
 *
 * File API defines
 *
 ***************************************************************************/

#define F_FILE FS_FILE
#define F_FIND FS_FIND
#define F_SPACE FS_SPACE
#define F_MAXPATH FS_MAXPATH
#define F_SEEK_SET FS_SEEK_SET
#define F_SEEK_END FS_SEEK_END
#define F_SEEK_CUR FS_SEEK_CUR

/****************************************************************************
 *
 * File API function externs
 *
 ***************************************************************************/

extern char *fg_getversion(void);
extern void fg_init(void);
extern int fg_mountdrive(int drivenum,void *buffer,long buffsize,FS_DRVMOUNT mountfunc,FS_PHYGETID phyfunc);
extern int fg_unmountdrive (int drvnum);

#define fsm_getversion fg_getversion
#define fsm_init fg_init

extern int fsm_mountdrive(int drivenum,void *buffer,long buffsize,FS_DRVMOUNT mountfunc,FS_PHYGETID phyfunc);
extern int fsm_unmountdrive(int drivenum);

extern int fsm_format(int drivenum);
extern int fsm_getfreespace(int drivenum, FS_SPACE *space);
extern void fsm_enterFS(void);
extern void fsm_releaseFS(long ID);
extern int fsm_get_oem(int drivenum, char *str, long maxlen);

extern int fsm_get_drive_list(int *buf);
extern int fsm_get_drive_count(void);

extern int fsm_getdrive(void);
extern int fsm_chdrive(int drivenum);

extern int fsm_getcwd(char *buffer, int maxlen );
extern int fsm_getdcwd(int drivenum, char *buffer, int maxlen );

extern int fsm_mkdir(const char *dirname);
extern int fsm_chdir(const char *dirname);
extern int fsm_rmdir(const char *dirname);

extern int fsm_getlabel(int drivenum, char *label, long len);
extern int fsm_setlabel(int drivenum, const char *label);

extern int fsm_rename(const char *filename,const char *newname);
extern int fsm_move(const char *filename, const char *newname);
extern int fsm_delete(const char *filename);

extern long fsm_filelength(const char *filename);

extern int fsm_findfirst(const char *filename,FS_FIND *find);
extern int fsm_findnext(FS_FIND *find);

extern FS_FILE *fsm_open(const char *filename,const char *mode);
extern int fsm_close(FS_FILE *filehandle);
extern int fsm_abortclose(FS_FILE *filehandle);
extern long fsm_write(const void *buf,long size,long size_st,FS_FILE *filehandle);
extern long fsm_read (void *buf,long size,long size_st,FS_FILE *filehandle);
extern int fsm_seek (FS_FILE *filehandle,long offset,long whence);
extern long fsm_tell (FS_FILE *filehandle);
extern int fsm_eof(FS_FILE *filehandle);
extern int fsm_seteof(FS_FILE *filehandle);
extern int fsm_rewind(FS_FILE *filehandle);
extern int fsm_putc(int ch,FS_FILE *filehandle);
extern int fsm_getc(FS_FILE *filehandle);
extern int fsm_flush(FS_FILE *filehandle);


extern int fsm_settimedate(const char *filename,unsigned short ctime,unsigned short cdate);
extern int fsm_gettimedate(const char *filename,unsigned short *ctime,unsigned short *cdate);
extern int fsm_getpermission(const char *filename, unsigned long *psecure);
extern int fsm_setpermission(const char *filename, unsigned long secure);
extern FS_FILE *fsm_truncate(const char *filename,unsigned long length);

/****************************************************************************
 *
 * Unicode externs
 *
 ***************************************************************************/

extern int fsm_wgetcwd(W_CHAR *buffer, int maxlen );
extern int fsm_wgetdcwd(int drivenum, W_CHAR *buffer, int maxlen );
extern int fsm_wmkdir(const W_CHAR *dirname);
extern int fsm_wchdir(const W_CHAR *dirname);
extern int fsm_wrmdir(const W_CHAR *dirname);
extern int fsm_wrename(const W_CHAR *filename,const W_CHAR *newname);
extern int fsm_wmove(const W_CHAR *filename,const W_CHAR *newname);
extern int fsm_wdelete(const W_CHAR *filename);
extern long fsm_wfilelength(const W_CHAR *filename);
extern int fsm_wfindfirst(const W_CHAR *filename,FS_WFIND *find);
extern int fsm_wfindnext(FS_WFIND *find);
extern int fsm_wgetpermission(const W_CHAR *filename, unsigned long *psecure);
extern int fsm_wsetpermission(const W_CHAR *filename, unsigned long secure);
extern FS_FILE *fsm_wopen(const W_CHAR *filename,const W_CHAR *mode);
extern int fsm_wsettimedate(const W_CHAR *filename,unsigned short ctime,unsigned short cdate);
extern int fsm_wgettimedate(const W_CHAR *filename,unsigned short *ctime,unsigned short *cdate);
extern FS_FILE *fsm_wtruncate(const W_CHAR *filename,unsigned long length);

/****************************************************************************
 *
 * File API functions
 *
 ***************************************************************************/

#define f_getversion fsm_getversion
#define f_init fsm_init
#define f_mountdrive(drivenum,buffer,buffsize,mountfunc,phyfunc) fsm_mountdrive(drivenum,buffer,buffsize,mountfunc,phyfunc)
#define f_unmountdrive(drvnum) fsm_unmountdrive (drvnum)
#define f_format(drivenum) fsm_format(drivenum)
#define f_getfreespace(drivenum,space) fsm_getfreespace(drivenum,space)
#define f_enterFS fsm_enterFS
#define f_releaseFS(ID) fsm_releaseFS(ID)
#define f_get_oem(drivenum,str,maxlen) fsm_get_oem(drivenum,str,maxlen)
#define f_get_drive_list(buf) fsm_get_drive_list (buf)
#define f_get_drive_count fsm_get_drive_count
#define f_getdrive fsm_getdrive
#define f_chdrive(drivenum) fsm_chdrive(drivenum)
#define f_getcwd(buffer,maxlen) fsm_getcwd(buffer,maxlen)
#define f_getdcwd(drivenum,buffer,maxlen) fsm_getdcwd(drivenum,buffer,maxlen)
#define f_mkdir(dirname) fsm_mkdir(dirname)
#define f_chdir(dirname) fsm_chdir(dirname)
#define f_rmdir(dirname) fsm_rmdir(dirname)
#define f_setlabel(drivenum,label) fsm_setlabel(drivenum,label)
#define f_getlabel(drivenum,label,len) fsm_getlabel(drivenum,label,len)
#define f_rename(filename,newname) fsm_rename(filename,newname)
#define f_move(filename,newname) fsm_move(filename,newname)
#define f_delete(filename) fsm_delete(filename)
#define f_filelength(filename) fsm_filelength(filename)
#define f_findfirst(filename,find) fsm_findfirst(filename,find)
#define f_findnext(find) fsm_findnext(find)
#define f_open(filename,mode) fsm_open(filename,mode)
#define f_close(filehandle) fsm_close(filehandle)
#define f_abortclose(filehandle) fsm_abortclose(filehandle)
#define f_write(buf,size,size_st,filehandle) fsm_write(buf,size,size_st,filehandle)
#define f_read(buf,size,size_st,filehandle) fsm_read(buf,size,size_st,filehandle)
#define f_seek(filehandle,offset,whence) fsm_seek(filehandle,offset,whence)
#define f_tell(filehandle) fsm_tell(filehandle)
#define f_eof(filehandle) fsm_eof(filehandle)
#define f_seteof(filehandle) fsm_seteof(filehandle)
#define f_rewind(filehandle) fsm_rewind(filehandle)
#define f_putc(ch,filehandle) fsm_putc(ch,filehandle)
#define f_getc(filehandle) fsm_getc(filehandle)
#define f_flush(filehandle) fsm_flush(filehandle)
#define f_settimedate(filename,ctime,cdate) fsm_settimedate(filename,ctime,cdate)
#define f_gettimedate(filename,pctime,pcdate) fsm_gettimedate(filename,pctime,pcdate)
#define f_getpermission(filename,psecure) fsm_getpermission(filename,psecure)
#define f_setpermission(filename,secure) fsm_setpermission(filename,secure)
#define f_truncate(filename,length) fsm_truncate(filename,length)

/****************************************************************************
 *
 * Unicode functions
 *
 ***************************************************************************/

#define f_wgetcwd(buffer,maxlen) fsm_wgetcwd(buffer,maxlen)
#define f_wgetdcwd(drivenum,buffer,maxlen) fsm_wgetdcwd(drivenum,buffer,maxlen)
#define f_wmkdir(dirname) fsm_wmkdir(dirname)
#define f_wchdir(dirname) fsm_wchdir(dirname)
#define f_wrmdir(dirname) fsm_wrmdir(dirname)
#define f_wrename(filename,newname) fsm_wrename(filename,newname)
#define f_wmove(filename,newname) fsm_wmove(filename,newname)
#define f_wdelete(filename) fsm_wdelete(filename)
#define f_wfilelength(filename) fsm_wfilelength(filename)
#define f_wfindfirst(filename,find) fsm_wfindfirst(filename,find)
#define f_wfindnext(find) fsm_wfindnext(find)
#define f_wopen(filename,mode) fsm_wopen(filename,mode)
#define f_wsettimedate(filename,ctime,cdate) fsm_wsettimedate(filename,ctime,cdate)
#define f_wgettimedate(filename,ctime,cdate) fsm_wgettimedate(filename,ctime,cdate)
#define f_wgetpermission(filename,psecure) fsm_wgetpermission(filename,psecure)
#define f_wsetpermission(filename,secure) fsm_wsetpermission(filename,secure)
#define f_wtruncate(filename,length) fsm_wtruncate(filename,length)


/****************************************************************************
 *
 * errorcodes
 *
 ***************************************************************************/
enum{
/*  0 */ F_NO_ERROR,
/*  1 */ F_ERR_INVALIDDRIVE,
/*  2 */ F_ERR_NOTFORMATTED,
/*  3 */ F_ERR_INVALIDDIR,
/*  4 */ F_ERR_INVALIDNAME,
/*  5 */ F_ERR_NOTFOUND,
/*  6 */ F_ERR_DUPLICATED,
/*  7 */ F_ERR_NOMOREENTRY,
/*  8 */ F_ERR_NOTOPEN,
/*  9 */ F_ERR_EOF,
/* 10 */ F_ERR_RESERVED,
/* 11 */ F_ERR_NOTUSEABLE,
/* 12 */ F_ERR_LOCKED,
/* 13 */ F_ERR_ACCESSDENIED,
/* 14 */ F_ERR_NOTEMPTY,
/* 15 */ F_ERR_INITFUNC,
/* 16 */ F_ERR_CARDREMOVED,
/* 17 */ F_ERR_ONDRIVE,
/* 18 */ F_ERR_INVALIDSECTOR,
/* 19 */ F_ERR_READ,
/* 20 */ F_ERR_WRITE,
/* 21 */ F_ERR_INVALIDMEDIA,
/* 22 */ F_ERR_BUSY,
/* 23 */ F_ERR_WRITEPROTECT,
/* 24 */ F_ERR_INVFATTYPE,
/* 25 */ F_ERR_MEDIATOOSMALL,
/* 26 */ F_ERR_MEDIATOOLARGE,
/* 27 */ F_ERR_NOTSUPPSECTORSIZE,
/* 28 */ F_ERR_UNKNOWN,
/* 29 */ F_ERR_DRVALREADYMNT,
/* 30 */ F_ERR_TOOLONGNAME,
/* 31 */ F_ERR_NOTFORREAD,
/* 32 */ F_ERR_DELFUNC,
/* 33 */ F_ERR_ALLOCATION,
/* 34 */ F_ERR_INVALIDPOS,
/* 35 */ F_ERR_NOMORETASK,
/* 36 */ F_ERR_NOTAVAILABLE
};

#ifdef __cplusplus
}
#endif

/****************************************************************************
 *
 * End of api_s.h
 *
 ***************************************************************************/

#endif /* HCC_API_S_H_ */
