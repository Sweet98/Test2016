/*------------------------------------------------------------------

 *-------------------------------------------------------------------
 * SyncObjects.c - This file provides the synchronization objects
 *  that have been occasionally used in the windows world, for
 *  the dragonball platform
 *-------------------------------------------------------------------
 * $RCSfile: app_kernel/syncobjects.c $
 * $Revision: 1.11.1.4 $
 * $Date: 2010/01/16 14:35:07EST $
 *-------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include "syncobjects.h"
#include "db_hdw.h"          /* artwork dependant hardware definitions    */

#if defined(SHOW_RESET_TO_BOOT) && !defined(MATRIX_BOOT_CODE)
#define reset_to_boot_code()		_reset_to_boot_code_hard(__LINE__)              //lmc    temp pointed to reset hard for now
#define reset_to_boot_code_hard() _reset_to_boot_code_hard(__LINE__)
void _reset_to_boot_code_hard(int);
#else
#define reset_to_boot_code		reset_to_boot_code_hard              //lmc    temp pointed to reset hard for now
void reset_to_boot_code(void);
#endif


void InitializeCriticalSection(CRITICAL_SECTION *pCS)
{
	*pCS=MT_CreateMutex(false);
}
void DeleteCriticalSection(CRITICAL_SECTION *pCS)
{
	MT_CloseMutex(*pCS);
	*pCS=NULL;
}
void EnterCriticalSection(CRITICAL_SECTION *pCS)
{
	if (!MT_WaitMutex(*pCS,INFINITE))
		reset_to_boot_code();				// something wrong reset unit
}

BOOL TryEnterCriticalSection(CRITICAL_SECTION *pCS)
{
	return MT_WaitMutex(*pCS,0);
}

void LeaveCriticalSection(CRITICAL_SECTION *pCS)
{
	MT_ReleaseMutex(*pCS);
}

#ifdef NDEBUG
HANDLE CreateEvent(LPVOID *, BOOL , BOOL bInitialState, LPVOID *)
#else
HANDLE CreateEvent(LPVOID *pVoid, BOOL bManualReset, BOOL bInitialState, LPVOID *pVoid2)
#endif

{
	// There are certain assumptions that we are making as per how Events are used in the
	// scan driver.  Make sure that it is correct.
	assert(pVoid == NULL);
	assert(bManualReset == TRUE);
	assert(pVoid2 == NULL);
	return (HANDLE)MT_CreateEvent(bInitialState!=0);
}

BOOL SetEvent(volatile HANDLE hEvent)
{
	MT_SetEvent((MT_Event_t)(hEvent));
	return 0;
}

BOOL ResetEvent(volatile HANDLE hEvent)
{
	MT_ResetEvent((MT_Event_t)(hEvent));
	return 0;
}

BOOL CloseHandle(volatile HANDLE hOpenHandle)
{
	MT_CloseEvent((MT_Event_t)hOpenHandle);
	return TRUE;
}

HANDLE WaitForSingleObject(volatile HANDLE hEvent,DWORD nTimeout)
{
	// note special power managment that was done here
	// should happen automatially now
	// since thread 0 manages power if thre is nothing else to do
	if (MT_WaitEvent((MT_Event_t)(hEvent),nTimeout))
		return WAIT_OBJECT_0;
	else
		return WAIT_OBJECT_TIMEOUT;
}
