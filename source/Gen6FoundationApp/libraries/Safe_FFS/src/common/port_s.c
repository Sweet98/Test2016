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
 * fs_getcurrenttimedate
 *
 * need to be ported depending on system, it retreives the
 * current time and date in DOS format. User must solve roll-over when reading
 * time and date. Roll-over problem to read a date at 23.59.59 and then reading time at
 * 00:00.00.
 *
 * INPUTS
 *
 * ptime - pointer where to store time or 0 if don't store time
 * pdata - pointer where to store date or 0 if don't store date
 *
 ***************************************************************************/

#if (!FS_CAPI_USED)
void fs_getcurrenttimedate(unsigned short *ptime, unsigned short *pdate)
{
	unsigned short hour=12;
	unsigned short min=0;
	unsigned short sec=0;

	unsigned short time= (unsigned short)(((hour     <<  FS_CTIME_HOUR_SHIFT) & FS_CTIME_HOUR_MASK) |
						   ((min      <<  FS_CTIME_MIN_SHIFT)  & FS_CTIME_MIN_MASK) |
						   (((sec>>1) <<  FS_CTIME_SEC_SHIFT)  & FS_CTIME_SEC_MASK));

	unsigned short year=1980;
	unsigned short month=1;
	unsigned short day=1;

	unsigned short date= (unsigned short)((((year-1980) <<  FS_CDATE_YEAR_SHIFT)  & FS_CDATE_YEAR_MASK) |
						   ((month		 <<  FS_CDATE_MONTH_SHIFT) & FS_CDATE_MONTH_MASK) |
						   ((day		 <<  FS_CDATE_DAY_SHIFT)   & FS_CDATE_DAY_MASK));

	if (ptime)
	{
		*ptime = time;
	}

	if (pdate)
	{
		*pdate = date;
	}
}
#endif

/****************************************************************************
 *
 * fs_mutex_create
 *
 * user function to create a mutex.
 *
 * RETURNS
 *   0 - success
 *   1 - error
 *
 ***************************************************************************/

#if (!FS_CAPI_USED)
int fs_mutex_create (FS_MUTEX_TYPE *mutex)
{
	*mutex=0;
	return 0;
}
#endif

/****************************************************************************
 *
 * fs_mutex_delete
 *
 * user function to delete a mutex.
 *
 * RETURNS
 *   0 - success
 *   1 - error
 *
 ***************************************************************************/

#if (!FS_CAPI_USED)
int fs_mutex_delete (FS_MUTEX_TYPE *mutex)
{
	*mutex=0;
	return 0;
}
#endif

/****************************************************************************
 *
 * fs_mutex_get
 *
 * user function to get a mutex.
 *
 * RETURNS
 *   0 - success
 *   1 - error
 *
 ***************************************************************************/

#if (!FS_CAPI_USED)
int fs_mutex_get (FS_MUTEX_TYPE *mutex)
{
	if (*mutex) return 1;
/*  add disable interrupt function here  */
	*mutex=1;
/*  add enable interrupt function here  */
	return 0;
}
#endif

/****************************************************************************
 *
 * fs_mutex_put
 *
 * user function to release a mutex.
 *
 * RETURNS
 *   0 - success
 *   1 - error
 *
 ***************************************************************************/

#if (!FS_CAPI_USED)
int fs_mutex_put (FS_MUTEX_TYPE *mutex)
{
	*mutex=0;
	return 0;
}
#endif

/****************************************************************************
 *
 * fn_gettaskID
 *
 * user function to get current task ID, zero cannot be a valid task ID
 *
 * RETURNS
 *
 * task ID
 *
 ***************************************************************************/

#if (!FS_CAPI_USED)
#ifndef _FN_GETTASKID_

long fn_gettaskID(void)
{
	return 1;
}

#endif
#endif

/****************************************************************************
 *
 * end of port_s.c
 *
 ***************************************************************************/
