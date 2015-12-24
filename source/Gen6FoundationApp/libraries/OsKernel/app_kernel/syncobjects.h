/*------------------------------------------------------------------

 *-------------------------------------------------------------------
 * SyncObjects.h - This file provides the synchronization objects
 *  that have been occasionally used in the windows world, for
 *  the dragonball platform
 *-------------------------------------------------------------------
 * $RCSfile: app_kernel/syncobjects.h $
 * $Revision: 1.2.1.2 $
 * $Date: 2009/01/26 15:11:01EST $
 *-------------------------------------------------------------------*/
#ifndef SYNC_OBJECTS
#define SYNC_OBJECTS
#include "language.h"
#define SetLastError(a)
#include "thread.h"
#define WAIT_OBJECT_0 0
#define WAIT_OBJECT_TIMEOUT 1

typedef  MT_Mutex_t CRITICAL_SECTION;
void InitializeCriticalSection(CRITICAL_SECTION *pCS);
void DeleteCriticalSection(CRITICAL_SECTION *pCS);
void EnterCriticalSection(CRITICAL_SECTION *pCS);
BOOL TryEnterCriticalSection(CRITICAL_SECTION *pCS);
void LeaveCriticalSection(CRITICAL_SECTION *pCS);
HANDLE CreateEvent(LPVOID *pVoid, BOOL bManualReset, BOOL bInitialState, LPVOID *pVoid2);
BOOL SetEvent(volatile HANDLE hEvent);
BOOL ResetEvent(volatile HANDLE hEvent);
BOOL CloseHandle(volatile HANDLE hOpenHandle);
HANDLE WaitForSingleObject(volatile HANDLE hEvent,DWORD nTimeout);
#endif
