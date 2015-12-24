/*-----------------------------------------------------------7--------

 *-------------------------------------------------------------------
 * ScanDriverScanManager.c - This is the source file for the
 *   functions that deal with registering and requesting a scan.
 *-------------------------------------------------------------------
 * $RCSfile: scandriverscanmanager.c $
 * $Revision: 1.83.1.27 $
 * $Date: 2011/04/07 13:56:34EDT $
 *-------------------------------------------------------------------*/
#if defined(_WIN32_WCE)
#define USE_WINCE_VIRTUAL_MEMORY_SCHEME
#include <windows.h>
#define SD_EVENT_WAIT_TIMEOUT	2000
#define SD_TIME_TO_WAIT_FOR_GOOD_BUFFER ((pOpenCntxt->AutoExpOptions.bImageMustConform)?(1100*(pOpenCntxt->AutoExpOptions.nTriesForConforming+1)):(5500))
#elif defined(PALMOS)
#include <windows.h>
#define SD_EVENT_WAIT_TIMEOUT	2000
#define SD_TIME_TO_WAIT_FOR_GOOD_BUFFER ((pOpenCntxt->AutoExpOptions.bImageMustConform)?(1100*(pOpenCntxt->AutoExpOptions.nTriesForConforming+1)):(5500))
#else // DBALL
#include <stdlib.h>
#include <string.h>
#include "language.h"

#include "product.h"
#include "syncobjects.h"
#define SD_EVENT_WAIT_TIMEOUT	2000 // 1 f/s needs at least 2 seconds ++
#define SD_TIME_TO_WAIT_FOR_GOOD_BUFFER ((pOpenCntxt->AutoExpOptions.bImageMustConform)?(1100*(pOpenCntxt->AutoExpOptions.nTriesForConforming+1)):(5500))
#endif

#include "ScanDriverOpenContext.h"
#include "ScanDriverScanManager.h"
#include "ScanDriverScannerSelect.h"
#include "ScanDriverLightControl.h"
#include "ScanDriverDebug.h"
#include "ScanDriverEngine.h"
#include "Ioman.h"
#include "Interrupts.h"
#define MULTIBIND 0

//int invalidfranes=0;            //jaw  debug  remove

static int volatile nApplicationScanBuffersRegistered=0;
static int volatile nPhysicalScanBuffersAvailable=0;
static DWORD volatile nLineISTnumber=0;

ScanBufferInfo_t *pgPhysicalScanBuffer = NULL;

static HANDLE hBufferLockOutTime=0,hWaitingScanBufferAvailable=0;
static CRITICAL_SECTION csScanBufferLock,csScanningInitiation;
static DWORD volatile nRowsNeededToAllowUpdateCycle;
static DWORD volatile nRowsInAScan;
static BOOL volatile bScanDriverScanningIsEnabled=FALSE;
static volatile BOOL	bScanDriverIsPoweredOff=FALSE;    //spd for full power down
static BOOL volatile bScanDriverForceRestart=FALSE;
static ScanDriverOpenContext_t * volatile pMasterModeContext=0;

static volatile enum {
	CONTINUOUS_SCAN,   // Continuous scan mode
	AUTO_SHUTDOWN,     // In auto shutdown mode waiting for a good iamge.
	SHUTDOWN_PENDING  // In auto shut down mode and a good image has been captured.
} g_autoShutdownMode;


/* The N+2 buffering scheme is used in here, but we can cut it down for various
 * reasons. */
#define TARGET_NUMBER_OF_SCRATCH_BUFFERS 2
void ScanDriverOpenContext_tPtrArgumentVoidReturn_Dummy(ScanDriverOpenContext_t *) {
}

void ScanDriverVsyncISTroutine(void);
void ScanDriverImageLineDataISTroutine(void);
void ScanDriverSofISTroutine(void);

static BOOL ScanManagementInitiate(void);
#ifndef PALMOS
void (*HHPSD_ExposureSettingsInitialize)(ScanDriverOpenContext_t *pOptions) = ScanDriverOpenContext_tPtrArgumentVoidReturn_Dummy;
#else
void (*HHPSD_ExposureSettingsInitialize)(ScanDriverOpenContext_t *pOptions);
#endif

void * zalloc(size_t bytes)
{
	void * p;

#ifdef _WIN32_WCE
	p = LocalAlloc(LMEM_FIXED, bytes);
#else
	p = malloc(bytes);
#endif

	memset(p, 0, bytes);
	return p;
}

void zfree(void * p)
{
#ifdef _WIN32_WCE
	LocalFree((HLOCAL)p);
#else
	free(p);
#endif
}

/*-------------------------------------------------------------------
 * NewBufferInfoEntry - This function creates a new entry into the
 *   info table.  This is a location where a new physical buffer
 *   reference is found
 *-------------------------------------------------------------------*/
ScanBufferInfo_t *NewBufferInfoEntry(void) {
	ScanBufferInfo_t *pInfo=NULL;
	if (pgPhysicalScanBuffer != NULL) {
		pInfo = pgPhysicalScanBuffer;
		while (pInfo->pNext != NULL) {
			pInfo = pInfo->pNext;
		}
		pInfo->pNext = (ScanBufferInfo_t *)zalloc(sizeof(ScanBufferInfo_t));
		pInfo = pInfo->pNext;
	} else {
		pgPhysicalScanBuffer = (ScanBufferInfo_t *)zalloc(sizeof(ScanBufferInfo_t));
		pInfo = pgPhysicalScanBuffer;
	}
	if (pInfo != NULL) {
		pInfo->BufferState = SBS_NOBUFFER;
		pInfo->hPhysicalScanBuffer = 0;
		pInfo->pDriverBuffer = NULL;
		pInfo->pNext = NULL;
	}
	return pInfo;
}
/*-------------------------------------------------------------------
 * DeleteBufferInfoEntry - This function will delete a given entry
 *   in the info table
 *-------------------------------------------------------------------*/
BOOL DeleteBufferInfoEntry(ScanBufferInfo_t *pDelete) {
	ScanBufferInfo_t *pInfo,*pInfoLast;
	if (pgPhysicalScanBuffer != NULL) {
		/* If the one we're deleting is the first entry in the list*/
		if (pDelete == pgPhysicalScanBuffer) {
			pInfo = pgPhysicalScanBuffer;
			pgPhysicalScanBuffer = pgPhysicalScanBuffer->pNext;
		} else {
			pInfo = pgPhysicalScanBuffer->pNext;
			pInfoLast = pgPhysicalScanBuffer;
			while (pInfo != NULL) {
				if (pInfo == pDelete) {
					pInfoLast->pNext = pInfo->pNext;
					break;
				}
				pInfoLast = pInfo;
				pInfo = pInfo->pNext;
			}
		}
		if (pInfo != NULL) {
			zfree(pInfo);
			return TRUE;
		}
	}
	return FALSE;
}
/*-------------------------------------------------------------------
 * FindBufferInfoEntry - This function finds a buffer info entry with
 *   a given state, and then retruns a pointer to that entry
 *-------------------------------------------------------------------*/
ScanBufferInfo_t *FindBufferInfoEntry(ScanBufferState_t nState) {
	ScanBufferInfo_t *pInfo = NULL;
	if (pgPhysicalScanBuffer != NULL) {
		pInfo = pgPhysicalScanBuffer;
		while (pInfo != NULL) {
			if (pInfo->BufferState == nState)
				break;
			pInfo = pInfo->pNext;
		}
	}
	return pInfo;
}
/*-------------------------------------------------------------------
 * InvalidateBufferEntries - This function will invalidate the state of
 *   any buffers which are valid and unlocked.
 *-------------------------------------------------------------------*/
void InvalidateBufferInfoEntries(void) {
	ScanBufferInfo_t *pInfo = NULL;
	if (pgPhysicalScanBuffer != NULL) {
		pInfo = pgPhysicalScanBuffer;
		while (pInfo != NULL) {
			if ((pInfo->BufferState == SBS_FILLING)||(pInfo->BufferState == SBS_WAITING))
				pInfo->BufferState = SBS_INVALID;
			pInfo = pInfo->pNext;
		}
	}
}
/*-------------------------------------------------------------------
 * InitializeScanManager - This is the function that initializes all
 *   of the tools that the scan manager will use
 *-------------------------------------------------------------------*/
BOOL InitializeScanManager(void) {
	DWORD nFrequencyOfLineRuns,nLinesPerLineProcess,i;
	hBufferLockOutTime = CreateEvent(NULL,TRUE,TRUE,0);
	if (hBufferLockOutTime == 0) {
		return 0;
	}
	hWaitingScanBufferAvailable = CreateEvent(NULL,TRUE,FALSE,0);
	if (hWaitingScanBufferAvailable == 0) {
		CloseHandle(hBufferLockOutTime);
		hBufferLockOutTime = 0;
		return 0;
	}
	InitializeCriticalSection(&csScanBufferLock);
	InitializeCriticalSection(&csScanningInitiation);

	// Now we initialize the hardware aspect of this all
	HWLAYER.RegisterSofPsocWriteNotification(ScanDriverSofISTroutine);

	if (!HWLAYER.RegisterVsyncNotification(ScanDriverVsyncISTroutine)) {
		DeleteCriticalSection(&csScanningInitiation);
		DeleteCriticalSection(&csScanBufferLock);
		CloseHandle(hWaitingScanBufferAvailable);
		hWaitingScanBufferAvailable = 0;
		CloseHandle(hBufferLockOutTime);
		hBufferLockOutTime = 0;
		return FALSE;
	}
	if (!HWLAYER.InitializeVsyncProcessing()) {
		DeleteCriticalSection(&csScanningInitiation);
		DeleteCriticalSection(&csScanBufferLock);
		CloseHandle(hWaitingScanBufferAvailable);
		hWaitingScanBufferAvailable = 0;
		CloseHandle(hBufferLockOutTime);
		hBufferLockOutTime = 0;
		return FALSE;
	}
	if (!HWLAYER.RegisterImageLineDataNotification(ScanDriverImageLineDataISTroutine)) {
		HWLAYER.DeinitializeVsyncProcessing();
		DeleteCriticalSection(&csScanningInitiation);
		DeleteCriticalSection(&csScanBufferLock);
		CloseHandle(hWaitingScanBufferAvailable);
		hWaitingScanBufferAvailable = 0;
		CloseHandle(hBufferLockOutTime);
		hBufferLockOutTime = 0;
		return FALSE;
	}
	if (!HWLAYER.InitializeImageLineDataProcessing()) {
		HWLAYER.DeinitializeVsyncProcessing();
		DeleteCriticalSection(&csScanningInitiation);
		DeleteCriticalSection(&csScanBufferLock);
		CloseHandle(hWaitingScanBufferAvailable);
		hWaitingScanBufferAvailable = 0;
		CloseHandle(hBufferLockOutTime);
		hBufferLockOutTime = 0;
		return FALSE;
	}
	nFrequencyOfLineRuns = HWLAYER.GetFrequencyPerFrameOfLineDataCallback();
	nRowsInAScan = HWLAYER.GetScanHeight();
	/* Here we set up when the scan manager will run the update cycle. */
	if (nFrequencyOfLineRuns > 0) {
		nLinesPerLineProcess = nRowsInAScan/nFrequencyOfLineRuns;
#ifdef _WIN32_WCE
		if (nFrequencyOfLineRuns > 5) {
			nRowsNeededToAllowUpdateCycle = nRowsInAScan - (5*nLinesPerLineProcess);
		}
#else
		if (nFrequencyOfLineRuns > 2) {
			nRowsNeededToAllowUpdateCycle = nRowsInAScan - (2*nLinesPerLineProcess);
		}
#endif
		else {
			nRowsNeededToAllowUpdateCycle = 1 * nLinesPerLineProcess;
		}
//DBG_OUTPUT1("Update Cycle to run on line %d\n",nRowsNeededToAllowUpdateCycle);
	} else {
		HWLAYER.DeinitializeVsyncProcessing();
		HWLAYER.DeinitializeImageLineDataProcessing();
		DeleteCriticalSection(&csScanningInitiation);
		DeleteCriticalSection(&csScanBufferLock);
		CloseHandle(hWaitingScanBufferAvailable);
		hWaitingScanBufferAvailable = 0;
		CloseHandle(hBufferLockOutTime);
		hBufferLockOutTime = 0;
		SetLastError(ERROR_BAD_ENVIRONMENT);
		return 0;
	}
	// I don't know if this is where I'll want to do this.  However, when we go to
	// 3 buffers, we're going to want to allocate 2 buffers right off the bat.
	for (i=0;i<TARGET_NUMBER_OF_SCRATCH_BUFFERS;i++) {
		if (AllocatePhysicalBufferAndCreateNewInfoEntry()) {
//DBG_OUTPUT1("Successfully allocated physical buffer %d\n",i);
		}
	}
	LightControlInitialize();
	return 1;
}

/*-------------------------------------------------------------------
 * DeinitializeScanManager - This is the function that deletes
 *   of the tools that the scan manager will use
 *-------------------------------------------------------------------*/
BOOL DeinitializeScanManager(void) {
	if (!HWLAYER.DeinitializeVsyncProcessing())
		return FALSE;
	if (!HWLAYER.DeinitializeImageLineDataProcessing())
		return FALSE;
	DeleteCriticalSection(&csScanningInitiation);
	DeleteCriticalSection(&csScanBufferLock);
	CloseHandle(hWaitingScanBufferAvailable);
	hWaitingScanBufferAvailable = 0;
	CloseHandle(hBufferLockOutTime);
	hBufferLockOutTime = 0;
	nPhysicalScanBuffersAvailable = 0;
	nApplicationScanBuffersRegistered = 0;
	while (pgPhysicalScanBuffer != NULL) {
		if (pgPhysicalScanBuffer->BufferState != SBS_NOBUFFER) {
			if ((pgPhysicalScanBuffer->hPhysicalScanBuffer != 0)&&(pgPhysicalScanBuffer->pDriverBuffer != NULL)) {
#ifdef USE_WINCE_VIRTUAL_MEMORY_SCHEME
				HWLAYER.UnbindPhysicalMemoryFromVirtualMemory(pgPhysicalScanBuffer->hPhysicalScanBuffer,
						pgPhysicalScanBuffer->pDriverBuffer);
#else
				HWLAYER.UnbindPhysicalMemoryFromVirtualMemory((DWORD)pgPhysicalScanBuffer->hPhysicalScanBuffer,
						(void *)&(pgPhysicalScanBuffer->pDriverBuffer));
#endif
			}
			if (pgPhysicalScanBuffer->pDriverBuffer != NULL) {
#ifdef USE_WINCE_VIRTUAL_MEMORY_SCHEME
				VirtualFree(pgPhysicalScanBuffer->pDriverBuffer,0,MEM_RELEASE);
#endif
				pgPhysicalScanBuffer->pDriverBuffer = NULL;
			}
			if (pgPhysicalScanBuffer->hPhysicalScanBuffer != 0) {
				HWLAYER.FreePhysicalScanSpace(pgPhysicalScanBuffer->hPhysicalScanBuffer);
				pgPhysicalScanBuffer->hPhysicalScanBuffer = 0;
			}
			pgPhysicalScanBuffer->BufferState = SBS_NOBUFFER;
		}
		/* This will erase a given entry*/
		DeleteBufferInfoEntry(pgPhysicalScanBuffer);
	}
	return 1;
}

/*-------------------------------------------------------------------
 * AllocatePhysicalBufferAndCreateNewInfoEntry() - This function
 *   petitions to create a new physical buffer and hense a new
 *   entry into the physical table which manages the physical buffers
 *-------------------------------------------------------------------*/
BOOL AllocatePhysicalBufferAndCreateNewInfoEntry(void) {
	DWORD hNewImage,nImageSize;
	void *pNewVirtualImage;
	BOOL bStatus = FALSE;
	ScanBufferInfo_t *pScanBufferInfo;

//	DBG_OUTPUT("\r\n ----AllocatePhysicalBufferAndCreateNewInfoEntry():There's room in the info table \r\n\r\n");

	/* First we try to allocate a physical buffer */
	hNewImage = HWLAYER.AllocatePhysicalScanSpace();

	if (hNewImage != 0)
	{
//		DBG_OUTPUT("----AllocatePhysicalBufferAndCreateNewInfoEntry():Successful allocation of new physical scan buffer\n");
		/* Now we try to get corresponding virtual memory to go along with it so we can access this
		 * memory in our device driver */
		nImageSize = HWLAYER.GetScanWidth() * HWLAYER.GetScanHeight();

#ifdef USE_WINCE_VIRTUAL_MEMORY_SCHEME
        if (nImageSize <= 2048*1024)
		    pNewVirtualImage = VirtualAlloc(NULL,2048*1024,MEM_RESERVE,PAGE_NOACCESS);
        else
		    pNewVirtualImage = VirtualAlloc(NULL,nImageSize,MEM_RESERVE,PAGE_NOACCESS);

		DBG_OUTPUT2 ("AllocatePhysicalBufferAndCreateNewInfoEntry pImg %08x %d\r\n",pNewVirtualImage,hNewImage);

		if (pNewVirtualImage != NULL)
		{
//			DBG_OUTPUT("----AllocatePhysicalBufferAndCreateNewInfoEntry():Successful virtual allocation of driver scan buffer\n");
			/* Finally we try to bind the new virtual memory that we reserved with the physical
			 * scan buffer that was created. */
			if (HWLAYER.BindPhysicalMemoryToVirtualMemory(hNewImage,pNewVirtualImage))
#else
			// in a non-virtual memory environment, the virtual image pointer passed to the bind function, is just a pointer to
			// the real pointer.  We need to be able to alter the pointer in the bind function which is why we need a pointer to
			// that pointer.
			if (HWLAYER.BindPhysicalMemoryToVirtualMemory(hNewImage,(void *)&pNewVirtualImage))
#endif
			{
//				DBG_OUTPUT("----AllocatePhysicalBufferAndCreateNewInfoEntry():Successful binding of physical scan to driver virtual memory\n");
				EnterCriticalSection(&csScanBufferLock);

				if (WaitForSingleObject(hBufferLockOutTime,SD_EVENT_WAIT_TIMEOUT) == WAIT_OBJECT_0)
				{
					pScanBufferInfo = NewBufferInfoEntry();

					if (pScanBufferInfo != NULL)
					{
						/* Everything worked, so we add the new table entry to our info table. */
//						DBG_OUTPUT("----AllocatePhysicalBufferAndCreateNewInfoEntry():Adding Entry to Scan Info Table\n");
						nPhysicalScanBuffersAvailable++;
						pScanBufferInfo->hPhysicalScanBuffer = hNewImage;
						pScanBufferInfo->pDriverBuffer = pNewVirtualImage;
						pScanBufferInfo->pDriverBufferCheckLocation = (void *)((DWORD)pNewVirtualImage + nImageSize - 4);
						pScanBufferInfo->BufferState = SBS_INVALID;
//						DBG_OUTPUT2("\r\n ----AllocatePhysicalBufferAndCreateNewInfoEntry():SUCCESS - New Info Entry - Physical:0x%x,Driver Virtual:0x%x \r\n",pScanBufferInfo->hPhysicalScanBuffer,pScanBufferInfo->pDriverBuffer);
						bStatus = TRUE;
					}
					else
					{
						SetLastError(ERROR_NOT_ENOUGH_MEMORY);

#ifdef USE_WINCE_VIRTUAL_MEMORY_SCHEME
						VirtualFree(pNewVirtualImage,0,MEM_RELEASE);
#endif

						HWLAYER.FreePhysicalScanSpace(hNewImage);
					}
				}
				else
				{
					DBG_OUTPUT("----AllocatePhysicalBufferAndCreateNewInfoEntry():FAILURE doing the WaitForSingleObject(hBufferLockOutTime,SD_EVENT_WAIT_TIMEOUT)\n");
					SetLastError(WAIT_TIMEOUT);

#ifdef USE_WINCE_VIRTUAL_MEMORY_SCHEME
					VirtualFree(pNewVirtualImage,0,MEM_RELEASE);
#endif

					HWLAYER.FreePhysicalScanSpace(hNewImage);
				}
				LeaveCriticalSection(&csScanBufferLock);
			}
			else
			{
				DBG_OUTPUT("----AllocatePhysicalBufferAndCreateNewInfoEntry():Failure binding of physical scan to driver virtual memory\n");
				SetLastError(ERROR_ACCESS_DENIED);

#ifdef USE_WINCE_VIRTUAL_MEMORY_SCHEME
				VirtualFree(pNewVirtualImage,0,MEM_RELEASE);
#endif

				HWLAYER.FreePhysicalScanSpace(hNewImage);
			}

#ifdef USE_WINCE_VIRTUAL_MEMORY_SCHEME
		}
		else
		{
			DBG_OUTPUT("----AllocatePhysicalBufferAndCreateNewInfoEntry():Virtual allocation of driver scan buffer:FAILED\n");
			SetLastError(ERROR_NOT_ENOUGH_MEMORY);
			HWLAYER.FreePhysicalScanSpace(hNewImage);
		}
#endif

	}
	else
	{
//		DBG_OUTPUT("----AllocatePhysicalBufferAndCreateNewInfoEntry():Allocation of physical buffer failed\n");
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
	}
	return bStatus;
}

/*-------------------------------------------------------------------
 * RegisterVirtualScanBuffer() - This function is called to register
 *  a new virtual scan buffer in our driver.  It also calls to
 *  allocate a new physical buffer if needed, and checks to make
 *  sure that there are enough physical buffers to go around to
 *  all of the virtual buffers
 *-------------------------------------------------------------------*/
BOOL RegisterVirtualScanBuffer(ScanDriverOpenContext_t *pOpenCntxt,void *pVirtualScanBuffer)
{
	BOOL bStatus = FALSE;
//	DBG_OUTPUT("\r\n --RegisterVirtualScanBuffer() \r\n\r\n");

	/* Make sure that there is room in the registry table */
	if (CreateNewRegisteredBuffer(pOpenCntxt,pVirtualScanBuffer))
	{
//		DBG_OUTPUT("---RegisterVirtualScanBuffer():New Registered Buffer Successful\n");
		nApplicationScanBuffersRegistered++;

/* Once we have image states, we're going to want to get a "Critical Section" around access to
 * the ScanBufferInfoTable.*/

		/* Now since we have registered a virtual buffer, we'll want to add a physical buffer for it as well.
		 * this will help us keep up with our N+2 buffering scheme when we get to it, or allocate a physical
		 * buffer in the case when we're not. */
		if (AllocatePhysicalBufferAndCreateNewInfoEntry())
		{
//			DBG_OUTPUT("---RegisterVirtualScanBuffer():Allocated Physical Image Buffer\n");
		}
		else
		{
//			DBG_OUTPUT("---RegisterVirtualScanBuffer():Failed to allocate physical image buffer\n");
		}

		/* Regardless of whether or not the physical allocation of a new buffer worked or not,
		 * we have to verify that there are enough physical buffers to meet the demand of
		 * the virtual buffers.  If there are not, we have to get rid of the one we just
		 * created. */
		if (nPhysicalScanBuffersAvailable < nApplicationScanBuffersRegistered)
		{
			DBG_OUTPUT("---RegisterVirtualScanBuffer():FAILURE Not enough physical scan buffers to go around\n");
			DeleteRegisteredBuffer(pOpenCntxt,pVirtualScanBuffer);
			nApplicationScanBuffersRegistered--;
			SetLastError(ERROR_OUTOFMEMORY);
		}
		else
		{
//			DBG_OUTPUT("---RegisterVirtualScanBuffer():Successful\n");
			bStatus = TRUE;
		}
	}
	else
	{
//		DBG_OUTPUT("---RegisterVirtualScanBuffer():New Registered Buffer Failed \n");
		SetLastError(ERROR_OUT_OF_STRUCTURES);
	}
	return bStatus;
}

/*-------------------------------------------------------------------
 * UnregisterVirtualScanBuffer() - This function takes a registered
 *   virtual memory entry out of the register table.  It also deletes
 *   a physical entry if needed as well
 *-------------------------------------------------------------------*/
BOOL UnregisterVirtualScanBuffer(ScanDriverOpenContext_t *pOpenCntxt,void *pVirtualScanBuffer) {
	BOOL bStatus = FALSE;
	ScanBufferRegistry_t *pRegEntry;
	ScanBufferInfo_t *pInfoEntry;

	/* First we search to see if the virtual scan buffer is in our registry table so we know
	 * how to unregister it. */
	pRegEntry = GetBufferRegisterEntry(pOpenCntxt,pVirtualScanBuffer);
	/* If we found a match...*/
	if (pRegEntry != NULL) {
		/* We'll first check to see if the virtual pointer currently has a physical buffer locked up
		 * and if it does, we'll have to unbind it first. */
		pInfoEntry = pRegEntry->pPhysicalScanBuffer;
		if (pInfoEntry != NULL) {
			if (HWLAYER.UnbindPhysicalMemoryFromVirtualMemory(pInfoEntry->hPhysicalScanBuffer,
				pVirtualScanBuffer) == 0) {
DBG_OUTPUT("UnregisterVirtualScanBuffer() - Unbinding of physical memory failed\n");
				SetLastError(ERROR_NOT_READY);
				return FALSE;
			}
			pRegEntry->pPhysicalScanBuffer = NULL;
			EnterCriticalSection(&csScanBufferLock);
			if (WaitForSingleObject(hBufferLockOutTime,SD_EVENT_WAIT_TIMEOUT) == WAIT_OBJECT_0) {
				pInfoEntry->BufferState = SBS_INVALID;
			} else {
				LeaveCriticalSection(&csScanBufferLock);
				SetLastError(WAIT_TIMEOUT);
				return FALSE;
			}
			LeaveCriticalSection(&csScanBufferLock);
		}
		/* Now we Delete out that entry in the table of registered scans */
		if (DeleteRegisteredBuffer(pOpenCntxt,pVirtualScanBuffer)) {
			/* Now that we've taken that registered entry out, we shift everybody else down */
			nApplicationScanBuffersRegistered--;
			bStatus = TRUE;
		}
		/* if we have more buffers than we need (this is already written to handle the
		 * n+2 buffering scheme), we give back one of the physical scan buffers */


		// we might make this to delete a single buffer regardless I'm not sure
		if (nPhysicalScanBuffersAvailable > (nApplicationScanBuffersRegistered+TARGET_NUMBER_OF_SCRATCH_BUFFERS)) {

			EnterCriticalSection(&csScanBufferLock);
			if (WaitForSingleObject(hBufferLockOutTime,SD_EVENT_WAIT_TIMEOUT) == WAIT_OBJECT_0) {
				/* since we allocate at the end, we'll delete from the end as well*/
				pInfoEntry = FindBufferInfoEntry(SBS_INVALID);
				/* if we have found a deletable buffer */
				if (pInfoEntry != NULL) {
#ifdef USE_WINCE_VIRTUAL_MEMORY_SCHEME
					if (HWLAYER.UnbindPhysicalMemoryFromVirtualMemory(pInfoEntry->hPhysicalScanBuffer,
						pInfoEntry->pDriverBuffer))
#else
					if (HWLAYER.UnbindPhysicalMemoryFromVirtualMemory((DWORD)pInfoEntry->hPhysicalScanBuffer,
						(void *)&(pInfoEntry->pDriverBuffer)))
#endif
					{

#ifdef USE_WINCE_VIRTUAL_MEMORY_SCHEME
						if (VirtualFree(pInfoEntry->pDriverBuffer,0,MEM_RELEASE))  {
#endif

							if (HWLAYER.FreePhysicalScanSpace(pInfoEntry->hPhysicalScanBuffer)) {
								if (DeleteBufferInfoEntry(pInfoEntry)) {
//DBG_OUTPUT("UnregisterVirtualScanBuffer() - Successfully deleted physical buffer\n");
									nPhysicalScanBuffersAvailable--;
								} else {
DBG_OUTPUT("UnregisterVirtualScanBuffer() - Couldn't delete input entry\n");
								}
							} else {
DBG_OUTPUT("UnregisterVirtualScanBuffer() - Error in freeing\n");
								SetLastError(ERROR_CANCEL_VIOLATION);
							}

#ifdef USE_WINCE_VIRTUAL_MEMORY_SCHEME
						} else {
DBG_OUTPUT("UnregisterVirtualScanBuffer() - Error in virtual freeing\n");
							SetLastError(ERROR_CANCEL_VIOLATION);
						}
#endif

					} else {
DBG_OUTPUT("UnregisterVirtualScanBuffer() - Error in unbind - deleting from info table\n");
						SetLastError(ERROR_NOT_READY);
					}
				} else {
DBG_OUTPUT("UnregisterVirtualScanBuffer() - Couldn't find a physical buffer to delete\n");
				}
			} else {
				SetLastError(WAIT_TIMEOUT);
			}
			LeaveCriticalSection(&csScanBufferLock);
		} else {
//DBG_OUTPUT("UnregisterVirtualScanBuffer() - No need to delete physical buffer\n");
		}
	} else {
DBG_OUTPUT("UnregisterVirtualScanBuffer() - Virtual Pointer not registered\n");
		SetLastError(ERROR_SECTOR_NOT_FOUND);
	}
	return bStatus;
}


static ScanBufferInfo_t * volatile pWaitingBuffer = NULL;
static ScanBufferInfo_t * volatile pFillingBuffer = NULL;
static BOOL volatile bPendingScanRequest = FALSE;
static BOOL volatile bRanUpdateCycleLate = FALSE;
#define IMG_CHECK_VAL_1 0
#define IMG_CHECK_VAL_2 0xff
#define IMG_CHECK_VAL_3 0
#define IMG_CHECK_VAL_4 0xff
/* Load the image with a value we can check at the end of the transfer. */
static void ScanDriverLoadCheckValue(ScanBufferInfo_t * volatile pScanBuffer) {
	volatile unsigned char *pCheck;
	pCheck = (unsigned char *)pScanBuffer->pDriverBufferCheckLocation;
	pCheck[0] = IMG_CHECK_VAL_1;
	pCheck[1] = IMG_CHECK_VAL_2;
	pCheck[2] = IMG_CHECK_VAL_3;
	pCheck[3] = IMG_CHECK_VAL_4;
}

// If JADE_IMAGE_OKAY is defined ScanDriverImageOkay() looks for a black line
// at the beginning and end of the image.
//ken@2012.02.02 TODO, the Hi704A sensor's image is not black line at the beginning and end of the image. So do not need to
//check the zero row. Use the Macro control is not a good way. It is about to be improved.
#ifndef HI2D_T
#define JADE_IMAGE_OKAY 
#endif
bool check_zero_row(const volatile void * _image, const volatile void * _p, 
					size_t width)
{
	const volatile unsigned char * p 
		= static_cast<const volatile unsigned char *>(_p);
#if ERR_MODE
	const volatile unsigned char * image 
		= static_cast<const volatile unsigned char *>(_image);
#endif

	size_t cnt = width;

	while(cnt--)
	{
		if (*p != 0)
		{
#if ERR_MODE
			size_t offset = p - image;
			dbg_output(  "ScanDriverImageOkay failed: "
						 "Pixel at %u (%u,%u) is 0x%X\r\n", 
						 offset, (offset % width), (offset / width), *p  );
#endif
			return false;
		}
		++p;
	}
	return true;
}

#if 0//def JADE_IMAGE_OKAY
static BOOL ScanDriverImageOkay(ScanBufferInfo_t * volatile pScanBuffer) {
	int width = HWLAYER.GetScanWidth();
	int height = HWLAYER.GetScanHeight();
	volatile UCHAR * top_row = static_cast<UCHAR *>(pScanBuffer->pDriverBuffer);
	volatile UCHAR * bottom_row = top_row + width * (height - 1);

	if (!check_zero_row(top_row, top_row, width))
		return false;
	if (!check_zero_row(top_row, bottom_row, width))
		return false;

	return true;
}

#else
/* Check to see if the check value has disappeared.  If it has, we know we've made it */
static BOOL ScanDriverImageOkay(ScanBufferInfo_t * volatile pScanBuffer) {
	volatile unsigned char *pCheck;
	pCheck = (unsigned char *)pScanBuffer->pDriverBufferCheckLocation;
	if ((pCheck[0] == IMG_CHECK_VAL_1)&&(pCheck[1] == IMG_CHECK_VAL_2)&&(pCheck[2] == IMG_CHECK_VAL_3)&&(pCheck[3] == IMG_CHECK_VAL_4)) {
			ERRMSG ("SCN_DRV: Image Invalid\r\n");
			//invalidfranes++;            //jaw  debug  remove
 			// if the shutdown is pending - then we need to start back up!!!
// 			if (g_autoShutdownMode == SHUTDOWN_PENDING) {
// 				bScanDriverForceRestart = TRUE;
// 				HWLAYER.ImagerPowerUp();
// 				g_autoShutdownMode = AUTO_SHUTDOWN;
// 			}
			return FALSE;
	}
	else
		return TRUE;
}
#endif

// This handles the case where ScanDriverImageOkay() returns false.
// It marks the waiting buffer as invalid.  Also if a shutdown is
// pending, it restarts scanning.
static void HandleImageNotOkay()
{
	if (g_autoShutdownMode == SHUTDOWN_PENDING) 
	{
		bScanDriverForceRestart = TRUE;
		HWLAYER.ImagerPowerUp();
		g_autoShutdownMode = AUTO_SHUTDOWN;
	}

	ResetEvent(hWaitingScanBufferAvailable);
	pWaitingBuffer->BufferState = SBS_INVALID;
	pWaitingBuffer = NULL;
}

//#define STAMP_IMAGE
unsigned stamp_image_cnt = 0;
#ifdef STAMP_IMAGE // test code to stamp images
static const char font[10][5][4] =
{
	{
		"***",
		"* *",
		"* *",
		"* *",
		"***"
	},
	{
		"  *",
		"  *",
		"  *",
		"  *",
		"  *"
	},
	{
		"***",
		"  *",
		"***",
		"*  ",
		"***",
	},
	{
		"***",
		"  *",
		"***",
		"  *",
		"***"
	},
	{
		"* *",
		"* *",
		"***",
		"  *",
		"  *"
	},
	{
		"***",
		"*  ",
		"***",
		"  *",
		"***"
	},
	{
		"*  ",
		"*  ",
		"***",
		"* *",
		"***"
	},
	{
		"***",
		"  *",
		"  *",
		"  *",
		"  *"
	},
	{
		"***",
		"* *",
		"***",
		"* *",
		"***"
	},
	{
		"***",
		"* *",
		"***",
		"  *",
		"  *"
	}
};

void write_digit(void * image, unsigned x_pos, unsigned y_pos, unsigned digit)
{
	if (digit >= 10)
		return;

	const char (*c_font)[4] = font[digit];

	unsigned stride = HWLAYER.GetScanWidth();
	unsigned char * p = (unsigned char *)image;
	p += x_pos + y_pos * stride;

	for (unsigned x = 0; x < 4; ++x)
	{
		unsigned char * p2 = p + x;
		*p2 = 0xff;
		p2[6 * stride] = 0xff;
	}

	for (unsigned y = 1; y < 6; ++y)
		for (unsigned x = 0; x < 4; ++x)
		{
			unsigned char * p2 = p + x + y * stride;
			char c = c_font[y-1][x];
			if (c == '*')
				*p2 = 0;
			else
				*p2 = 0xff;
		}
}

void stamp_image(ScanBufferInfo_t * pBuffer, unsigned number)
{
	for (unsigned i = 5; i--; )
	{
		unsigned m = number % 10;
		write_digit(pBuffer->pDriverBuffer, 300 + i * 4, 0, m);
		number /= 10;
	}

}
#endif // end of stamp image debug

/* Before this function is called, be sure that VSYNC processing is disabled, because
 * it could really screw up the works.  It will set the hBufferLockOutTime event, and
 * it doesn't need the Critical Section to do it.  The VSYNC was designed this way because
 * this allows somebody to be waiting in the critical section on that event. */
void ScanManagementUpdateCycle(DWORD nRowsPresent) {
	ScanBufferInfo_t * volatile pNextToFill = NULL;
	DWORD nInitVal;
	/* If we have run this in the current image before, we don't run it again.  However,
	 * if we haven't run it in the current image before, we'll run it only if we've already
	 * seen atleast 400 rows of the image. */

	if (g_autoShutdownMode == SHUTDOWN_PENDING)
		return;

	//dbg_output("ValidRowsNeeded = %d\r\n", nRowsNeededToAllowUpdateCycle);            //lmc   debug 534 reported for jade

	if ((nRowsPresent >= nRowsNeededToAllowUpdateCycle)&&(WaitForSingleObject(hBufferLockOutTime,0) == WAIT_OBJECT_0)) {
		/* The first thing that must be done is to try and grab the critical section
		 * we don't want somebody else messing with the buffer when we need it. */
#if defined(_WIN32_WCE) || defined(PALMOS)
		EnterCriticalSection(&csScanBufferLock);
#else
		if (TryEnterCriticalSection(&csScanBufferLock))
#endif
		{
//DBG_OUTPUT("L - ");
			/* Here we indicate that nobody is allowed to touch the buffer variables anymore.*/
			ResetEvent(hBufferLockOutTime);
			/* Here we let the auto exposure routines create a new exposure and gain, and send
			 * it to the system. */
			if (pFillingBuffer != NULL) {
				HHPSD.ScanManagerAutoExpCalc();
				/* If we have a filling buffer which is not acceptable, we'll simply fill that one
				 * again. */
#ifdef  STAMP_IMAGE // test code to stamp images
				{
					DBG(== 3)dbg_output("Image cnt = %u\r\n", stamp_image_cnt);
					stamp_image(pFillingBuffer, stamp_image_cnt);
					if (++stamp_image_cnt > 99999)
						stamp_image_cnt = 0;

				}
#endif // end of test code to stamp images
				if (!HHPSD.ScanManagerImageIsAcceptable((ScanDriverLastScanInfo_t *)&(pFillingBuffer->LastScanInfo))) {
					pNextToFill = pFillingBuffer;
					pFillingBuffer = NULL;
				}
				else if (g_autoShutdownMode == AUTO_SHUTDOWN)
				{
					/* If the last captured buffer was good, make it the waiting buffer,
					   clear pFillingBuffer and pNextToFill and bail. */
					pWaitingBuffer = pFillingBuffer;
					SetEvent(hWaitingScanBufferAvailable);
					pFillingBuffer = NULL;
						pNextToFill    = NULL;
						g_autoShutdownMode = SHUTDOWN_PENDING;
						LeaveCriticalSection(&csScanBufferLock);
						return;
				}
			}
			/* If we are going to refill the filling buffer, then we'll leave the waiting buffer
			 * alone. */
			if (pNextToFill == NULL) {
#if MULTIBIND
				/* If the waiting buffer has nobody bound to it, we can take it.  So we do. */
				if (pWaitingBuffer != NULL) {
					/* If the waiting buffer has nobody currently connected, it's free.  So we take it. */
					if (pWaitingBuffer->AppBuffersCurrentlyBound == 0)	{
						pNextToFill = pWaitingBuffer;
						pWaitingBuffer = NULL;
						ResetEvent(hWaitingScanBufferAvailable);
					} else {
						/* The waiting buffer currently has applications bound to it, so we
						 * cannot use it for the next-to-fill*/
						pNextToFill = FindBufferInfoEntry(SBS_INVALID);
					}
				} else {
					/* We didn't find a waiting buffer, therefore we have to search the valid buffers
					* to see if there is one that is currently not being used that we can fill next*/
					pNextToFill = FindBufferInfoEntry(SBS_INVALID);
				}
#else
				/* If the waiting buffer we had before still exists, then we know nobody had
				 * claimed it yet.  So we take it. */
				if (pWaitingBuffer != NULL)
				{
					pNextToFill = pWaitingBuffer;
					pWaitingBuffer = NULL;
					ResetEvent(hWaitingScanBufferAvailable);
				} else {
					/* We didn't find a waiting buffer, therefore we have to search the valid buffers
					 * to see if there is one that is currently not being used that we can fill next*/
					pNextToFill = FindBufferInfoEntry(SBS_INVALID);
				}
#endif
			}
			/* If we still haven't found a buffer to fill next time, our only option is to see whether
			 * there is a buffer currently filling, and if so, is somebody currently waiting for the
			 * buffer. */
			if (pNextToFill == NULL) {
				/* There is a currently filling buffer. */
				if (pFillingBuffer != NULL) {
					/* Somebody is waiting for the buffer, so we make this a waiting buffer, and forfeit
					 * or opportunity to fill next time. */
					if (bPendingScanRequest == TRUE) {
						pFillingBuffer->BufferState = SBS_WAITING;
						pWaitingBuffer = pFillingBuffer;
//DBG_OUTPUT1("W:%d ",pWaitingBuffer->hPhysicalScanBuffer);
						pFillingBuffer = NULL;
 						SetEvent(hWaitingScanBufferAvailable);
					} else {
						/* Nobody is waiting on the filling buffer, wo we'll just fill it again. */
						pNextToFill = pFillingBuffer;
						pFillingBuffer = NULL;
					}
				}
			} else if (pFillingBuffer != NULL) {
				/* If we did already have a buffer which will be filled next time, now we look to see if
				 * there is a currently filling buffer.  Because if there is, we turn that into our
				 * waiting buffer. */
				pFillingBuffer->BufferState = SBS_WAITING;
				pFillingBuffer->AppBuffersCurrentlyBound = 0;
				pWaitingBuffer = pFillingBuffer;
//DBG_OUTPUT1("W:%d ",pWaitingBuffer->hPhysicalScanBuffer);
				pFillingBuffer = NULL;
				SetEvent(hWaitingScanBufferAvailable);
			}

			/* If we make it down here, and we have truly found a buffer to use as the buffer we will fill,
			 * we set the state of that buffer to filling, and we indicate that this is the area we're going
			 * to fill to the HW. */
			if ((pNextToFill != NULL)&&(HHPSD.ScanManagerEngineSaysOkToCaptureNextFrame())) {
				/* This is delicate.  Even though when we do the update cycle, we want the VSYNC handler shut
				 * off, the hardware VSYNC could still hit in the middle of our initiation of the buffer which
				 * would cause the buffer manager to not be synchronized with the hardware.  Therefore, if we
				 * see that to be the case, we'll reset the scan hardware, and try again. We'll do this 5 times
				 * before we give up.  If it takes more than 2 times, we've got serious problems. */
				nInitVal = HWLAYER.InitiateCapture(pNextToFill->hPhysicalScanBuffer, 2 );

				if (nInitVal > 0) {
					if (nInitVal == 1) {
						bRanUpdateCycleLate = TRUE;
					}
					pNextToFill->BufferState = SBS_FILLING;
					pFillingBuffer = pNextToFill;
					ScanDriverLoadCheckValue(pFillingBuffer);
//DBG_OUTPUT1("F:%d ",pFillingBuffer->hPhysicalScanBuffer);
				} else {
					pNextToFill->BufferState = SBS_INVALID;
					pFillingBuffer = NULL;
				}
			} else {
				pNextToFill->BufferState = SBS_INVALID;
				pFillingBuffer = NULL;
				nInitVal = HWLAYER.InitiateCapture( NULL, 1 );            //no buffer to send
         }

//DBG_OUTPUT2("T:%d,R:%d\n",GetTickCount() - nScanTime,HHPSD.GetNumberOfValidRows());
//nScanTime = GetTickCount();
			/* we're done, so we give back the critical section. */
//DBG_OUTPUT("\n");
			LeaveCriticalSection(&csScanBufferLock);
		}
	}
}


void ScanManagementFrameBoundry(BOOL bScanningEnabled) {
	static BOOL bSkipUpdateNextVsync = FALSE;
	ScanBufferInfo_t * volatile pNextToFill;
	DWORD nUpdateTries,nInitVal;
	HHPSD.FrameBoundary();
	/* If we are supposed to be scanning */
	if (bScanningEnabled) {
		/* Here we indicate that nobody is allowed to touch the buffer variables anymore in the Vert .*/
		/* Jade hardware flips buffers in the vert (dma complete), so we do not want to pass a pointer here. */
		ResetEvent(hBufferLockOutTime);		//will set it again at bottom of this method
//DBG_OUTPUT("V-");
		if (HHPSD.ScanEngineFaultOccurred() || bScanDriverForceRestart) {
			/* This will shut down any transfer which is currently taking place */
			HWLAYER.ResetScanTransfer();
			/* If the hBufferLockOutTime is set, then we have some room to play because the line interrupt
			 * has already locked out the buffers for us.  Now, typically when I mess with the buffers, I
			 * need to have the buffer lock critical section.  THEREFORE, I NEED TO BE VERY CAREFUL HERE!!!
			 * Since we know that the hBufferLockOutTime event has been set, we know that the line interrupt
			 * had to gain the critical section to do this.  This means that even if the application has the
			 * critical section at this time, it won't be able to get anywhere without the hBufferLockOutTime,
			 * and therefore, I can feel free to mess with the buffers as we wish.  We also know that the
			 * UpdateCycle can't be running right now, because the VSYNC thread is disabled when run from the
			 * Line Interrupt IST. */
			if (WaitForSingleObject(hBufferLockOutTime,0) != WAIT_OBJECT_0) {
				if (WaitForSingleObject(hWaitingScanBufferAvailable,0) == WAIT_OBJECT_0) {
					pWaitingBuffer->BufferState = SBS_INVALID;
					pWaitingBuffer = NULL;
					ResetEvent(hWaitingScanBufferAvailable);
				}
			}
			nUpdateTries = 0;
			do {
				if (HHPSD.ScanEngineRestart(pMasterModeContext))
					break;
			}while (++nUpdateTries < 5);
			/* This will restart scanning, including writing of exposure and gain if needed. */
			if (nUpdateTries < 5) {
				/* if we made it here, we successfully restart */
				bScanDriverForceRestart = FALSE;
				SetEvent(hBufferLockOutTime);
				/* Now we initiate the transfer*/
				pNextToFill = NULL;
				/* if we already have a dedicated filling buffer from the last run through of the
				 * update cycle, we can simply use that.  It has no implications on the info table
				 * (which would require the critical section) so we can simply set it up to go. */
				if (pFillingBuffer != NULL) {
					pNextToFill = pFillingBuffer;
				} else {
					/* If we have to search for a buffer, then we need the critical section.  Therefore
					 * we try to lock it up. */
					if (TryEnterCriticalSection(&csScanBufferLock)) {
						/* Now we look through to decide what buffer to begin our filling with. */
						pNextToFill = FindBufferInfoEntry(SBS_INVALID);
						LeaveCriticalSection(&csScanBufferLock);
					}
				}
				if (pNextToFill != NULL) {
					/* This is delicate.  The hardware VSYNC could still hit in the middle of our initiation of the
					 * buffer which would cause the buffer manager to not be synchronized with the hardware.
					 * Therefore, if we see that to be the case, we'll reset the scan hardware, and try again. We'll
					 * do this 5 times before we give up.  If it takes more than 2 times, we've got serious problems. */
					nInitVal = HWLAYER.InitiateCapture(pNextToFill->hPhysicalScanBuffer, 2 );

					if (nInitVal > 0) {
						/* We want to indicate that we did initiate a scan from this run through the
						 * frame boundry so we don't try and run an update next time. */
						bSkipUpdateNextVsync = TRUE;
						pNextToFill->BufferState = SBS_FILLING;
						pFillingBuffer = pNextToFill;
						ScanDriverLoadCheckValue(pFillingBuffer);
					} else {
						pNextToFill->BufferState = SBS_INVALID;
						pFillingBuffer = NULL;
					}
				}else{
					nInitVal = HWLAYER.InitiateCapture( NULL, 1 );            //no buffer to send
				}
			}
		}
		/* But the last time around, we didn't run the update cycle, we want
		 * to run the update cycle anyway because if there is a filling buffer
		 * that has to go to a waiting state, or just to keep the scanning working.
		 *
		 * Now, we can't run this twice in a row.  The reason is because when we
		 * run the update cycle from here, we have to wait for a frame for the settings
		 * to take effect.  If we run it twice in a row, then the buffer that we set up
		 * to fill, will be swapped out before it even had a chance to fill.  Also,
		 * when we do run the UpdateCycle from here, since we won't be filling the current
		 * frame, there won't be any line interrupts to run the UpdateCycle outside of
		 * here, so we need to make sure that this doesn't run from here twice in a row
		 * because that would screw up the buffer management when the line interrupt starts
		 * running again. */
		else if ((WaitForSingleObject(hBufferLockOutTime,0) == WAIT_OBJECT_0)&&(!bSkipUpdateNextVsync)) {
			ScanManagementUpdateCycle(nRowsInAScan + 1);
			/* If we set up to fill a new buffer, then we need to indicate that we were
				* here last time.  The transfer will only begin on the NEXT VSYNC, therefore
				* we don't want this doing an update cycle in the next VSYNC because that's
				* the only place where the settings truly take effect. */
			if (pFillingBuffer != NULL) {
				 bSkipUpdateNextVsync = TRUE;
			}
			if (pWaitingBuffer != NULL) {
				if (!ScanDriverImageOkay(pWaitingBuffer))
					HandleImageNotOkay();
			}
		} else if (bRanUpdateCycleLate == TRUE){
			bSkipUpdateNextVsync = TRUE;
			if (pWaitingBuffer != NULL) {
				if (!ScanDriverImageOkay(pWaitingBuffer))
					HandleImageNotOkay();
			}
		} else {
			if (pWaitingBuffer != NULL) {
				if (!ScanDriverImageOkay(pWaitingBuffer))
					HandleImageNotOkay();
			}
			// We only want this to run from here if we know that the scanning update cycle
			// has already been run for this frame.
			HHPSD.ScanManagerFrameEventAutoExposure();
         // If we missed initiating the new buffer even though the filling buffer was initiated,
         // we do not want the update cycle to run on the next Vsync even though it will try to.
         if ((pFillingBuffer != NULL)&&(!HWLAYER.CaptureInitiated())) {
               bSkipUpdateNextVsync = TRUE;
         } else {
               bSkipUpdateNextVsync = FALSE;
         }
      }
		/* This function is called on a frame boundry, and since the changes are locked in on this
		 * boundry, we're good to go. */
	} else {
		bSkipUpdateNextVsync = FALSE;
    }
//DBG_OUTPUT2("V:%d,R:%d\n",GetTickCount() - nScanTime,HHPSD.GetNumberOfValidRows());
//nScanTime = GetTickCount();
	bRanUpdateCycleLate = FALSE;
	SetEvent(hBufferLockOutTime);
//DBG_OUTPUT("X\n");
}
/*-------------------------------------------------------------------
 * ScanManagementBufferReturn - This function is called when an
 *   application is done with a physical buffer, but they don't want
 *   to unregister their buffer.  So basically this function just
 *   takes the buffer that they have, and releases it back to the
 *   system.
 *-------------------------------------------------------------------*/
BOOL ScanManagementBufferReturn(ScanDriverOpenContext_t *pOpenCntxt,void *pAppBuffer) {
	BOOL bStatus = FALSE;
	ScanBufferRegistry_t *pRegEntry;
	ScanBufferInfo_t	*pInfoEntry;
	/* First we search to see if the virtual scan buffer is in our registry table so we know
	 * how to unregister it. */
	pRegEntry = GetBufferRegisterEntry(pOpenCntxt,pAppBuffer);
	/* If we found a match...*/
	if (pRegEntry != NULL) {
		/* We found that this application buffer already has a buffer attached to it.  We must
		 * free that one first. */
		if (pRegEntry->pPhysicalScanBuffer != NULL) {
			/* Now we try to enter the massive group of synchronization variables */

			/* We lock up the critical section first, the reason is because of the hBufferLockOutTime wait event.
			 * We know that event can only be reset in the ScanManagementUpdateCycle IN THE CRITICAL SECTION.
			 * Therefore, we know that if we get the critical section, the wait event will not reset on us.  Now
			 * one might ask, well if you get the critical section, but block on the wait event, aren't you hogging
			 * the critical section.  Well, yes and no, if somebody is doing something other then grabbing buffers
			 * (like registering or unregistering buffers) yes, but otherwise no.  The event goes from reset back
			 * to set in an area not surrounded by the critical section.  So this is fine.  As for the Update Cycle
			 * we're never really holding it off, because the only time we'd come to that situation would be AFTER
			 * the update cycle has already run, and we're waiting for the VSYNC to hit to clear the event.  That
			 * gives us most of the frame to carry out our work before the update cycle will need the critical section
			 * again.*/
			EnterCriticalSection(&csScanBufferLock);
			if (WaitForSingleObject(hBufferLockOutTime,SD_EVENT_WAIT_TIMEOUT)==WAIT_OBJECT_0) {
				/* Alright, we're free to go.  Now we have to give back the buffer, so to speak, that the
				 * application already has.  Therefore, we must unbind the physical memory, and indicate
				 * this in the scan buffer info table. */
				pInfoEntry = pRegEntry->pPhysicalScanBuffer;
				/* If for some reason, the unbinding process is unsuccessful, we'll leave with a failure.*/
				if (!HWLAYER.UnbindPhysicalMemoryFromVirtualMemory(pInfoEntry->hPhysicalScanBuffer,
					pAppBuffer)) {
					/* Before we leave though, we must be sure to give back the critical section. */
					LeaveCriticalSection(&csScanBufferLock);
					SetLastError(ERROR_NOT_READY);
					return bStatus;
				}
				/* Now because we just did the unbind, we reflect the change in the scan buffer table, and
				 * the registry table showing that we are truly unbound. */
				bStatus = TRUE;
				if (pInfoEntry->AppBuffersCurrentlyBound > 1) {
					pInfoEntry->AppBuffersCurrentlyBound--;
//dbg_output("returning - 0x%x, remaining count:%d\r\n",pInfoEntry->hPhysicalScanBuffer,pInfoEntry->AppBuffersCurrentlyBound);
				} else {
#if MULTIBIND
					if (pInfoEntry != pWaitingBuffer) pInfoEntry->BufferState = SBS_INVALID;
					else pWaitingBuffer->BufferState = SBS_WAITING;
#else
					pInfoEntry->BufferState = SBS_INVALID;
#endif
					pInfoEntry->AppBuffersCurrentlyBound = 0;
//dbg_output("returning - 0x%x, remaining count:%d\r\n",pInfoEntry->hPhysicalScanBuffer,pInfoEntry->AppBuffersCurrentlyBound);
				}
				pRegEntry->pPhysicalScanBuffer = NULL;
			} else {
				SetLastError(WAIT_TIMEOUT);
			}
			LeaveCriticalSection(&csScanBufferLock);
		}
	} else {
		SetLastError(ERROR_SECTOR_NOT_FOUND);
	}
	return bStatus;
}

/*-------------------------------------------------------------------
 * ScanManagerNewScanAvailable - This is called when an application
 *   is looking to get a buffer with the latest image in it, but is
 *   interested to know first when one is available and only make
 *   the request when one is available.
 *
 * Return Value - non zero - a new image buffer is available
 *					zero - either we are not scanning, you do not have
 *							master mode, or one isn't available.
 *							Bottom line, it means you can't get a
 *							buffer right now for some reason.
 *-------------------------------------------------------------------*/
BOOL ScanManagerNewScanAvailable(ScanDriverOpenContext_t *pOptions) {
	if (bScanDriverScanningIsEnabled != FALSE) {
		if (pOptions == pMasterModeContext) {
			if ((WaitForSingleObject(hWaitingScanBufferAvailable,0) == WAIT_OBJECT_0)&&(WaitForSingleObject(hBufferLockOutTime,0) == WAIT_OBJECT_0)) {
				return 1;
			}
		}
	}
	return 0;
}

/*-------------------------------------------------------------------
 * ScanManagmentRequestHandler - This is called when an application
 *   is looking to get a buffer with the latest image in it.  First
 *   it releases any image it may currently have, and then is attached
 *   to the most recent buffer.
 *
 * Return Value - non zero on success
 *-------------------------------------------------------------------*/
BOOL ScanManagementRequestHandler(ScanDriverOpenContext_t *pOpenCntxt,void *pAppBuffer) {
	int nTries;
	DWORD nWaitStatus;
	BOOL bReturnStatus = FALSE;
	volatile ScanBufferRegistry_t *pRegEntry;
	ScanBufferInfo_t *pInfoEntry;
	int nTries2;

	/* We only want requests to be made when scanning has been enabled. */
	if (bScanDriverScanningIsEnabled == FALSE) {
		return bReturnStatus;
	}

	if (pOpenCntxt != pMasterModeContext) {
		return bReturnStatus;
	}
	/* First we search to see if the virtual scan buffer is in our registry table so we know
	 * how to unregister it. */
	pRegEntry = GetBufferRegisterEntry(pOpenCntxt,pAppBuffer);
	/* If we found a match...*/
	if (pRegEntry != NULL) {
		/* We found that this application buffer already has a buffer attached to it.  We must
		 * free that one first. */
		if (pRegEntry->pPhysicalScanBuffer != NULL) {
			/* Now we try to enter the massive group of synchronization variables */

			/* We lock up the critical section first, the reason is because of the hBufferLockOutTime wait event.
			 * We know that event can only be reset in the ScanManagementUpdateCycle IN THE CRITICAL SECTION.
			 * Therefore, we know that if we get the critical section, the wait event will not reset on us.  Now
			 * one might ask, well if you get the critical section, but block on the wait event, aren't you hogging
			 * the critical section.  Well, yes and no, if somebody is doing something other then grabbing buffers
			 * (like registering or unregistering buffers) yes, but otherwise no.  The event goes from reset back
			 * to set in an area not surrounded by the critical section.  So this is fine.  As for the Update Cycle
			 * we're never really holding it off, because the only time we'd come to that situation would be AFTER
			 * the update cycle has already run, and we're waiting for the VSYNC to hit to clear the event.  That
			 * gives us most of the frame to carry out our work before the update cycle will need the critical section
			 * again.*/
			EnterCriticalSection(&csScanBufferLock);
			nTries2 = 0;
			while (++nTries2 < 3) {
				if (WaitForSingleObject(hBufferLockOutTime,SD_EVENT_WAIT_TIMEOUT)==WAIT_OBJECT_0) {
					/* Alright, we're free to go.  Now we have to give back the buffer, so to speak, that the
				 	* application already has.  Therefore, we must unbind the physical memory, and indicate
				 	* this in the scan buffer info table. */
					pInfoEntry = pRegEntry->pPhysicalScanBuffer;
					/* If for some reason, the unbinding process is unsuccessful, we'll leave with a failure.*/
					if (!HWLAYER.UnbindPhysicalMemoryFromVirtualMemory(pInfoEntry->hPhysicalScanBuffer,
						pAppBuffer)) {
						/* Before we leave though, we must be sure to give back the critical section. */
						LeaveCriticalSection(&csScanBufferLock);
						SetLastError(ERROR_NOT_READY);
						return bReturnStatus;
					}
					/* Now because we just did the unbind, we reflect the change in the scan buffer table, and
				 	* the registry table showing that we are truly unbound. */
					if (pInfoEntry->AppBuffersCurrentlyBound > 1) {
						pInfoEntry->AppBuffersCurrentlyBound--;
//dbg_output("\r\nreturning - phys:0x%x, remaining count:%d\r\n\r\n",pInfoEntry->hPhysicalScanBuffer,pInfoEntry->AppBuffersCurrentlyBound);
					} else {
#if MULTIBIND
						if (pInfoEntry != pWaitingBuffer) pInfoEntry->BufferState = SBS_INVALID;
						else pWaitingBuffer->BufferState = SBS_WAITING;
#else
						pInfoEntry->BufferState = SBS_INVALID;
#endif
						pInfoEntry->AppBuffersCurrentlyBound = 0;
//dbg_output("returning - 0x%x, remaining count:%d\r\n",pInfoEntry->hPhysicalScanBuffer,pInfoEntry->AppBuffersCurrentlyBound);
					}
					pRegEntry->pPhysicalScanBuffer = NULL;
					/* We know that there are enough scans buffers to go around to every application that there is out there.
				 	* we've designed it that way.  Therefore, the only time that we would have to worry about restarting the
				 	* scanning would be in the case where we've just returned a buffer.  The pFillingBuffer being NULL indicates
				 	* that the last time the UpdateCycle was run, we found no new buffer to fill.  Since we just returned a
				 	* buffer, now there is one.  We COULD just wait for the VSYNC interrupt IST to come around run the special
				 	* case IST routine which would restart scanning.  However, that would take 2+ frames to get an image, but
				 	* starting it here will expedite the process. */
					if (pFillingBuffer == NULL && g_autoShutdownMode == CONTINUOUS_SCAN) {
						/* THE VSYNC ROUTINE MUST BE DISABLED when we are using "other means" for getting the scanning going.
					 	* The reason is because the VSYNC could try to start scanning itself, and cause us to become confused
					 	* over which buffer is filling, and when. */
						HWLAYER.DisableVsyncProcessing();
						BOOL bEngineAutoShutdown = TRUE;     //cause a sync shutdown
						HHPSD.DeinitializeScanEngine(pMasterModeContext, bEngineAutoShutdown);   //untrigger
						HWLAYER.DisableImageLineDataProcessing();

						//Disable completed, now re-enable
						HWLAYER.EnableImageLineDataProcessing();
						bEngineAutoShutdown = FALSE;         //cause a Continuous scan
						nTries = 0;
						do {
            				if (HHPSD.InitializeScanEngine(pMasterModeContext,bEngineAutoShutdown))
								break;
						} while (++nTries < 5);

						if (nTries >= 5) {
							HWLAYER.DisableImageLineDataProcessing();
							bScanDriverScanningIsEnabled = FALSE;
							pMasterModeContext = 0;
							LeaveCriticalSection(&csScanBufferLock);	/*leave critical After Disable above so we don't nest BufferLocks */
							SetLastError(ERROR_GEN_FAILURE);
							return bReturnStatus;
						}
						LeaveCriticalSection(&csScanBufferLock);	/*leave critical After Disable above so we don't nest BufferLocks */

						nTries = 0;
						do {
							if (ScanManagementInitiate())
								break;
						} while (++nTries < 5);

						EnterCriticalSection(&csScanBufferLock); /*re-enter Critical Section */

						if (nTries >= 5) {
							HHPSD.DeinitializeScanEngine(pMasterModeContext,FALSE);
							HWLAYER.DisableImageLineDataProcessing();
							bScanDriverScanningIsEnabled = FALSE;
							pMasterModeContext = 0;
							LeaveCriticalSection(&csScanBufferLock);
							SetLastError(ERROR_GEN_FAILURE);
							return bReturnStatus;
						}
						else {
							HWLAYER.EnableVsyncProcessing();
							HHPSD.ScanEngineStart();            //trigger
						}
					}
					LeaveCriticalSection(&csScanBufferLock);
					break;
				}	//end if (WaitForSingleObject)
			}	//end while
			if (nTries2 >= 3) {
				LeaveCriticalSection(&csScanBufferLock);
				SetLastError(WAIT_TIMEOUT);
				return bReturnStatus;
			}
		}
		/* Alrighty then, here is a pretty confusing area of code, and I'm going to try and explain it as
		 * best I can.  Here goes....
		 * The first thing that is done is to put in a scan request.  The only time that this makes any difference
		 * whatsoever is when there is only a single buffer available and the decision has to be made whether or not
		 * to refill it. */
		bPendingScanRequest = TRUE;
		/* The loop here below has a few things going on.  First off, there's no real gain to having something other
		 * than a while(1) loop, but most people fear those, so I didn't do it.  Or if I did, I overcame my fear.
		 * However, after that we realize that before we can do anything, we need to be sure that there is a waiting
		 * buffer for us.  Therefore, we'll wait for this to occur.  After we see this to be the case, we grab the
		 * critical section, and try for the buffer lock.  Next, we see again if we have a waiting buffer.  It is
		 * very possible to assume that a waiting buffer exists when we check it, but by the time we get the
		 * critical section, and the buffer lock, it's gone.  In this case, we give up on the critical section, and
		 * try to synchronize again.  When we leave this loop, we should have a waiting buffer, have the critical
		 * section, and be sure that we're not in the buffer lock out time period.*/
		nTries2 = 0;
		while (bPendingScanRequest) {
			nTries2++;
			if (WaitForSingleObject(hWaitingScanBufferAvailable,SD_TIME_TO_WAIT_FOR_GOOD_BUFFER)==WAIT_OBJECT_0) {
				EnterCriticalSection(&csScanBufferLock);
				if (WaitForSingleObject(hBufferLockOutTime,SD_EVENT_WAIT_TIMEOUT)==WAIT_OBJECT_0) {
					nWaitStatus = WaitForSingleObject(hWaitingScanBufferAvailable,0);
					if (nWaitStatus == WAIT_OBJECT_0) {
						bPendingScanRequest = FALSE;
						break;
					}
    				}
					if (nTries2 > 3) {
					bPendingScanRequest = FALSE;	// clear bPendingScanRequest before leaving critical section
					}
						LeaveCriticalSection(&csScanBufferLock);
					}
				if (nTries2 > 3) {
					DBG_OUTPUT("Time-out waiting for "
							   "hWaitingScanBufferAvailable in "
							   "ScanManagementRequestHandler\r\n");
							   //invalidfranes++;              //jaw  debug  remove
					bPendingScanRequest = FALSE;
					SetLastError(WAIT_TIMEOUT);
					return bReturnStatus;
				}
			}
		/* At this time, we have the critical section, the buffer lock, and we know that a waiting buffer is there
		 * for us to attach to.*/
		/* First we try to bind to the physical buffer.  If this works, everything else is just grunt work. */
		if (!HWLAYER.BindPhysicalMemoryToVirtualMemory(pWaitingBuffer->hPhysicalScanBuffer,pAppBuffer)) {
			/* For some reason, our bind failed.  Release the critical section, and return FALSE*/
			LeaveCriticalSection(&csScanBufferLock);
			SetLastError(ERROR_ACCESS_DENIED);
			return bReturnStatus;
		}
#if MULTIBIND
		if (pWaitingBuffer->BufferState == SBS_IN_USE) {
			pWaitingBuffer->AppBuffersCurrentlyBound++;
		} else {
			pWaitingBuffer->BufferState = SBS_IN_USE;
			pWaitingBuffer->AppBuffersCurrentlyBound = 1;
		}
#else
		pWaitingBuffer->BufferState = SBS_IN_USE;
		pWaitingBuffer->AppBuffersCurrentlyBound = 1;
#endif
		pRegEntry->pPhysicalScanBuffer = (ScanBufferInfo_t *)(pWaitingBuffer);
//dbg_output("---0x%x - Gets Buffer 0x%x,Info Buffer:0x%x, Count:%d\r\n",pAppBuffer,pWaitingBuffer->hPhysicalScanBuffer,(DWORD)pWaitingBuffer,pWaitingBuffer->AppBuffersCurrentlyBound);
#if (!MULTIBIND)
		pWaitingBuffer = NULL;
		ResetEvent(hWaitingScanBufferAvailable);
#endif
		bReturnStatus = TRUE;
		LeaveCriticalSection(&csScanBufferLock);
	} else {
		SetLastError(ERROR_SECTOR_NOT_FOUND);
	}
	return bReturnStatus;
}

/*--------------------------------------------------------------------------------
 *   ScanManagerAttachToSameBuffer - The idea behind this function is that
 *     the variable pSourceBuffer and pDuplicateBuffer are both registered buffers
 *     with the scan driver.  And the desire of this function is to make
 *     pDuplicateBuffer bind to the same buffer as pSourceBuffer is currently bound
 *     to.  Upon successful return, both buffers should be bound to the same
 *     image buffer.
 *--------------------------------------------------------------------------------*/
BOOL ScanManagerAttachToSameBuffer(ScanDriverOpenContext_t *pOpenCntxt, void *pDuplicateBuffer, void *pSourceBuffer) {
	int nTries;
	BOOL bReturnStatus = FALSE;
	volatile ScanBufferRegistry_t *pRegEntryDup,*pRegEntrySrc;
	ScanBufferInfo_t *pInfoEntry;
	int nTries2;

	/* We only want requests to be made when scanning has been enabled. */
	if (bScanDriverScanningIsEnabled == FALSE) {
		return bReturnStatus;
	}

	if (pOpenCntxt != pMasterModeContext) {
		return bReturnStatus;
	}

	/* First we search to make sure that both the src and duplicate buffers are VALID registered buffers */
	pRegEntryDup = GetBufferRegisterEntry(pOpenCntxt,pDuplicateBuffer);
	pRegEntrySrc = GetBufferRegisterEntry(pOpenCntxt,pSourceBuffer);
	/* If we found BOTH of the entries, we can try to do this ... */
	if ((pRegEntryDup != NULL) && (pRegEntrySrc != NULL)) {
		/* Here we make sure that we actually HAVE a buffer to duplicate.  If there's no buffer to duplicate,
		 * there's no use going on.  */
		if (pRegEntrySrc->pPhysicalScanBuffer != NULL) {
			/* Since we are going to be binding something to the "duplicate" buffer, if it has something, we need to release it */
			if (pRegEntryDup->pPhysicalScanBuffer != NULL) {
				/* Now we try to enter the massive group of synchronization variables  - an explanation of the sequence of logic here
			 	* is explained in the ScanManagerRequestHandler function */
				EnterCriticalSection(&csScanBufferLock);
				nTries2 = 0;
				while (++nTries2 < 3) {
					if (WaitForSingleObject(hBufferLockOutTime,SD_EVENT_WAIT_TIMEOUT)==WAIT_OBJECT_0) {
						/* Alright, we're free to go.  Now we have to give back the buffer, so to speak, that the
				 		* application already has.  Therefore, we must unbind the physical memory, and indicate
				 		* this in the scan buffer info table. */
						pInfoEntry = pRegEntryDup->pPhysicalScanBuffer;
						/* If for some reason, the unbinding process is unsuccessful, we'll leave with a failure.*/
						if (!HWLAYER.UnbindPhysicalMemoryFromVirtualMemory(pInfoEntry->hPhysicalScanBuffer,
							pDuplicateBuffer)) {
							/* Before we leave though, we must be sure to give back the critical section. */
							LeaveCriticalSection(&csScanBufferLock);
							SetLastError(ERROR_NOT_READY);
							return bReturnStatus;
						}
						/* Now because we just did the unbind, we reflect the change in the scan buffer table, and
				 		* the registry table showing that we are truly unbound. */
						if (pInfoEntry->AppBuffersCurrentlyBound > 1) {
							pInfoEntry->AppBuffersCurrentlyBound--;
	//dbg_output("returning - 0x%x, remaining count:%d\r\n",pInfoEntry->hPhysicalScanBuffer,pInfoEntry->AppBuffersCurrentlyBound);
						} else {
#if MULTIBIND
							if (pInfoEntry != pWaitingBuffer) pInfoEntry->BufferState = SBS_INVALID;
							else pWaitingBuffer->BufferState = SBS_WAITING;
#else
							pInfoEntry->BufferState = SBS_INVALID;
#endif
							pInfoEntry->AppBuffersCurrentlyBound = 0;
	//dbg_output("returning - 0x%x, remaining count:%d\r\n",pInfoEntry->hPhysicalScanBuffer,pInfoEntry->AppBuffersCurrentlyBound);
						}
						pRegEntryDup->pPhysicalScanBuffer = NULL;
						/* We know that there are enough scans buffers to go around to every application that there is out there.
				 		* we've designed it that way.  Therefore, the only time that we would have to worry about restarting the
				 		* scanning would be in the case where we've just returned a buffer.  The pFillingBuffer being NULL indicates
				 		* that the last time the UpdateCycle was run, we found no new buffer to fill.  Since we just returned a
				 		* buffer, now there is one.  We COULD just wait for the VSYNC interrupt IST to come around run the special
				 		* case IST routine which would restart scanning.  However, that would take 2+ frames to get an image, but
				 		* starting it here will expedite the process. */
						if (pFillingBuffer == NULL && g_autoShutdownMode == CONTINUOUS_SCAN) {
							/* THE VSYNC ROUTINE MUST BE DISABLED when we are using "other means" for getting the scanning going.
					 		* The reason is because the VSYNC could try to start scanning itself, and cause us to become confused
					 		* over which buffer is filling, and when. */
							HWLAYER.DisableVsyncProcessing();
							BOOL bEngineAutoShutdown = TRUE;     //cause a sync shutdown
							HHPSD.DeinitializeScanEngine(pMasterModeContext, bEngineAutoShutdown);   //untrigger
							HWLAYER.DisableImageLineDataProcessing();

							//Disable completed, now re-enable
							HWLAYER.EnableImageLineDataProcessing();
							bEngineAutoShutdown = FALSE;         //cause a Continuous scan
							nTries = 0;
							do {
            					if (HHPSD.InitializeScanEngine(pMasterModeContext,bEngineAutoShutdown))
									break;
							} while (++nTries < 5);

							if (nTries >= 5) {
								HWLAYER.DisableImageLineDataProcessing();
								bScanDriverScanningIsEnabled = FALSE;
								pMasterModeContext = 0;
								LeaveCriticalSection(&csScanBufferLock);	/*leave critical After Disable above so we don't nest BufferLocks */
								SetLastError(ERROR_GEN_FAILURE);
								return bReturnStatus;
							}
							LeaveCriticalSection(&csScanBufferLock);	/*leave critical After Disable above so we don't nest BufferLocks */

							nTries = 0;
							do {
								if (ScanManagementInitiate())
									break;
							} while (++nTries < 5);

							EnterCriticalSection(&csScanBufferLock); /*re-enter Critical Section */

							if (nTries >= 5) {
								HHPSD.DeinitializeScanEngine(pMasterModeContext,FALSE);
								HWLAYER.DisableImageLineDataProcessing();
								bScanDriverScanningIsEnabled = FALSE;
								pMasterModeContext = 0;
								LeaveCriticalSection(&csScanBufferLock);
								SetLastError(ERROR_GEN_FAILURE);
								return bReturnStatus;
							}
							else {
								HWLAYER.EnableVsyncProcessing();
								HHPSD.ScanEngineStart();            //trigger
							}
						}
						LeaveCriticalSection(&csScanBufferLock);
						break;
					} else if (nTries2 >= 2) {
						LeaveCriticalSection(&csScanBufferLock);
						SetLastError(WAIT_TIMEOUT);
						return bReturnStatus;
					}	//end if (WaitForSingleObject)
				}	//end while
			}
			nTries2 = 0;
			EnterCriticalSection(&csScanBufferLock);
			while (1) {
				nTries2++;
				if (WaitForSingleObject(hBufferLockOutTime,SD_EVENT_WAIT_TIMEOUT)==WAIT_OBJECT_0) {
					break;
				}
				else if( nTries2 > 3 ) {
					LeaveCriticalSection(&csScanBufferLock);
  					SetLastError(WAIT_TIMEOUT);
  					return bReturnStatus;
				}
			}
			pInfoEntry = pRegEntrySrc->pPhysicalScanBuffer;
			/* At this time, we have the critical section, the buffer lock, and we know that a waiting buffer is there
		 	* for us to attach to.*/
			/* First we try to bind to the physical buffer.  If this works, everything else is just grunt work. */
			if (!HWLAYER.BindPhysicalMemoryToVirtualMemory(pInfoEntry->hPhysicalScanBuffer,pDuplicateBuffer)) {
				/* For some reason, our bind failed.  Release the critical section, and return FALSE*/
				LeaveCriticalSection(&csScanBufferLock);
				SetLastError(ERROR_ACCESS_DENIED);
				return bReturnStatus;
			}
			pInfoEntry->AppBuffersCurrentlyBound++;
			pRegEntryDup->pPhysicalScanBuffer = pInfoEntry;
//dbg_output("---0x%x - Gets Buffer 0x%x,Info Buffer:0x%x, Count:%d\r\n",pAppBuffer,pWaitingBuffer->hPhysicalScanBuffer,(DWORD)pWaitingBuffer,pWaitingBuffer->AppBuffersCurrentlyBound);
			bReturnStatus = TRUE;
			LeaveCriticalSection(&csScanBufferLock);
		} else {
			SetLastError(ERROR_SECTOR_NOT_FOUND);
		}
	} else {
		SetLastError(ERROR_SECTOR_NOT_FOUND);
	}
	return bReturnStatus;
}


/* Before this function is called, be sure that VSYNC processing is disabled, because
 * it could really screw up the works.  It will set the hBufferLockOutTime event, and
 * it doesn't need the Critical Section to do it.  The VSYNC was designed this way because
 * this allows somebody to be waiting in the critical section on that event. */
static BOOL ScanManagementInitiate(void) {
	DWORD nInitVal;
	ScanBufferInfo_t *pNextToFill=NULL;
	BOOL bReturnStatus = FALSE;
	/* First thing like always, we want to be sure that we have the synchronization
	 * objects that we need to be sure nobody gets tripped up by this. */
	EnterCriticalSection(&csScanBufferLock);
	if (WaitForSingleObject(hBufferLockOutTime,SD_EVENT_WAIT_TIMEOUT)==WAIT_OBJECT_0) {
		/* The first interrupt to run will be the VSYNC.  This is the initial kick off of the
		 * update cycle, so we want to signal that the update cycle has already run. */
		ResetEvent(hBufferLockOutTime);
		/* As a relic of the last time we ran, this event might still be set.  However, seeing
		 * as how we just cleared the waiting buffer, we must reset this event. */
		ResetEvent(hWaitingScanBufferAvailable);
		/* We nullify any of the old buffers, we're starting a new scanning process, and
		 * we don't want any old pointers pointing somewhere other than NULL at this point. */
		pFillingBuffer = NULL;
		pWaitingBuffer = NULL;
		/* Look for buffer states that were left as either waiting or filling, these again
		 * are relics of the last run.  Delete these.*/
		InvalidateBufferInfoEntries();
		/* Now we look through to decide what buffer to begin our filling with. */
		pNextToFill = FindBufferInfoEntry(SBS_INVALID);
		/* if we found one, we initiate it to fill.  Here we go. */
		if (pNextToFill != NULL) {
			/* This is delicate.  Even though when we do the update cycle, we want the VSYNC handler shut
			 * off, the hardware VSYNC could still hit in the middle of our initiation of the buffer which
			 * would cause the buffer manager to not be synchronized with the hardware.  Therefore, if we
			 * see that to be the case, we'll reset the scan hardware, and try again. We'll do this 5 times
			 * before we give up.  If it takes more than 2 times, we've got serious problems. */
			nInitVal = HWLAYER.InitiateCapture(pNextToFill->hPhysicalScanBuffer, 2 );

			if (nInitVal > 0) {
				if (nInitVal == 1) {
					bRanUpdateCycleLate = TRUE;
				}
				pNextToFill->BufferState = SBS_FILLING;
				pFillingBuffer = pNextToFill;
				ScanDriverLoadCheckValue(pFillingBuffer);
				// This loads to the DMA if the imager is optimized for that (STMicro utilizes this)
				// if the capture loads, then we'll know that it's as though the VSYNC ran and
				// we can set the lockout
				if (HWLAYER.LoadCaptureInfo()) {
					ScanManagementFrameBoundry(TRUE);
				}
				bReturnStatus = TRUE;
			} else {
				ERRMSG1 ("SCN_DRV: Initiate Capture Failed %d\r\n",nInitVal);         //lmc    debug
				pNextToFill->BufferState = SBS_INVALID;
				pFillingBuffer = NULL;
				SetEvent(hBufferLockOutTime);
			}
		}
		/* We cannot help it if no buffers are available, we'll initiate scanning when we have a chance from
		 * the VSYNC service thread.*/
		else {
			// we didn't change anything, so we can indicate the buffer lock is open
			SetEvent(hBufferLockOutTime);
			bReturnStatus = TRUE;
			nInitVal = HWLAYER.InitiateCapture( NULL, 1 );            //no buffer to send
		}
	}
	LeaveCriticalSection(&csScanBufferLock);
	return bReturnStatus;
}

//extern void TriggerTimeNow();             //jaw  debug  remove
//extern void TimeSinceTrigger();           //jaw  debug  remove

/* These are the IST routines which deal with the VSYNC interrput, and
 * Line interrupt respectively.  */
BOOL ScanDriverStartScanning(ScanDriverOpenContext_t *pOptions,BOOL bEngineAutoShutdown) {
	DWORD nTries;
	BOOL bStatus=FALSE;
	//TriggerTimeNow();                        //jaw  debug  remove
	EnterCriticalSection(&csScanningInitiation);
	if (bScanDriverScanningIsEnabled != TRUE) {
		bScanDriverScanningIsEnabled = TRUE;
	   g_autoShutdownMode = (bEngineAutoShutdown ? AUTO_SHUTDOWN : CONTINUOUS_SCAN);
		pMasterModeContext = pOptions;
		if (!HWLAYER.EnableImageLineDataProcessing()) {
			bScanDriverScanningIsEnabled = FALSE;
			pMasterModeContext = 0;
			LeaveCriticalSection(&csScanningInitiation);
			SetLastError(ERROR_GEN_FAILURE);
			return bStatus;
		}
		nTries = 0;
		do {
            if (HHPSD.InitializeScanEngine(pOptions,bEngineAutoShutdown))
				break;
			ERRMSG1 ("SCN_DRV:StartScanning Initialize Failed try %d\r\n",nTries);
		} while (++nTries < 5);
		if (nTries >= 5) {
			HWLAYER.DisableImageLineDataProcessing();
			bScanDriverScanningIsEnabled = FALSE;
			pMasterModeContext = 0;
			LeaveCriticalSection(&csScanningInitiation);
			ERRMSG ("SCN_DRV:Starting Initialize Retries Failed\r\n");
			SetLastError(ERROR_GEN_FAILURE);
			return bStatus;
		}
		LightControlEnableIndividualControl(pOptions);
		/* We need Vsync processing disabled when we are
		 * initiating a scan. */

		/* We give ourselves a few tries to get an initiation to work...*/
		nTries = 0;
		do {
			if (ScanManagementInitiate())
				break;
			ERRMSG1 ("SCN_DRV:StartScanning Initiate Failed try %d\r\n",nTries);
		} while (++nTries < 5);
		/* ... However, if initiation didn't work, we bail.*/
		if (nTries >= 5) {
			HHPSD.DeinitializeScanEngine(pOptions,FALSE);
			HWLAYER.DisableImageLineDataProcessing();
			bScanDriverScanningIsEnabled = FALSE;
			pMasterModeContext = 0;
			ScanManagementFrameBoundry(FALSE);
			LeaveCriticalSection(&csScanningInitiation);
			ERRMSG ("SCN_DRV:StartScanning Initiate Retries Failed\r\n");
			SetLastError(ERROR_GEN_FAILURE);
			return bStatus;
		}
		if (!HWLAYER.EnableVsyncProcessing()) {
			HHPSD.DeinitializeScanEngine(pOptions,FALSE);
			HWLAYER.DisableImageLineDataProcessing();
			bScanDriverScanningIsEnabled = FALSE;
			pMasterModeContext = 0;
			ScanManagementFrameBoundry(FALSE);
			LeaveCriticalSection(&csScanningInitiation);
			ERRMSG ("SCN_DRV:StartScanning EnableVsync Failed\r\n");
			SetLastError(ERROR_GEN_FAILURE);
			return bStatus;
		}
		HHPSD.ScanEngineStart();
		nLineISTnumber = 0;
		bStatus = TRUE;
	} else {
		SetLastError(ERROR_EXCL_SEM_ALREADY_OWNED);
	}
	LeaveCriticalSection(&csScanningInitiation);
	return bStatus;
}

BOOL ScanDriverStopScanning( ScanDriverOpenContext_t *pOptions, BOOL bEngineAutoShutdown ) {
	BOOL bStatus = TRUE;
	EnterCriticalSection(&csScanningInitiation);
	if (bScanDriverScanningIsEnabled != FALSE) {
		if (pOptions != pMasterModeContext) {
			LeaveCriticalSection(&csScanningInitiation);
			SetLastError(ERROR_NOT_OWNER);
			return FALSE;
		}
		if (!HWLAYER.DisableVsyncProcessing()) {
			SetLastError(ERROR_GEN_FAILURE);
			bStatus = FALSE;
		}

		bEngineAutoShutdown = TRUE;
		if (!HHPSD.DeinitializeScanEngine(pOptions, bEngineAutoShutdown)) {            //lmc was bool bEngineAutoShutdown, now always true
			SetLastError(ERROR_GEN_FAILURE);
			bStatus = FALSE;
		}

		if (!HWLAYER.DisableImageLineDataProcessing()) {
			SetLastError(ERROR_GEN_FAILURE);
			bStatus = FALSE;
		}
		LightControlEnableGlobalControl();
		bScanDriverScanningIsEnabled = FALSE;
		pMasterModeContext = 0;
		bScanDriverForceRestart = FALSE;
		ScanManagementFrameBoundry(FALSE);
	} else {
		bStatus = FALSE;
	}
	LeaveCriticalSection(&csScanningInitiation);
	return bStatus;
}

BOOL ScanDriverIsScanning(void) {
	BOOL bStatus;
	EnterCriticalSection(&csScanningInitiation);
	bStatus = bScanDriverScanningIsEnabled;
	LeaveCriticalSection(&csScanningInitiation);
	return bStatus;
}

BOOL ScanDriverPrepareChangeConfig(ScanDriverOpenContext_t *pOptions) {
	EnterCriticalSection(&csScanningInitiation);
	/* If we are currently scanning, and this is the correct open context, return TRUE
	 * indicating we're leaving the initiation section locked.  Cleanup must be called
	 * to free this */
	if ((bScanDriverScanningIsEnabled == TRUE) && (pMasterModeContext == pOptions))
		return TRUE;
	else if (bScanDriverScanningIsEnabled == FALSE)
		return TRUE;
	/* No cleanup necessary.*/
	LeaveCriticalSection(&csScanningInitiation);
	return FALSE;
}

void ScanDriverCleanupChangeConfig(ScanDriverOpenContext_t *pOptions) {
	if ((bScanDriverScanningIsEnabled == TRUE) && (pMasterModeContext == pOptions))
		HHPSD.ExposureSettingsUpdate(pOptions);
	else if (bScanDriverScanningIsEnabled == FALSE)
		HHPSD_ExposureSettingsInitialize(pOptions);
	LeaveCriticalSection(&csScanningInitiation);
}

BOOL ScanDriverSingleCapture(ScanDriverOpenContext_t *pOptions, void *pAppBuffer) {
    BOOL bStatus=TRUE;
    if (ScanDriverStartScanning(pOptions,TRUE)) {
		//dbg_output ("q");               //jaw  debug  remove
		if (ScanManagementRequestHandler(pOptions,pAppBuffer)) {
        	//dbg_output ("a");             //jaw  debug  remove
			ScanDriverStopScanning(pOptions,TRUE);
		} else {
			//dbg_output ("b");             //jaw  debug  remove
			ScanDriverStopScanning(pOptions,TRUE);
			bStatus=FALSE;
		}
    } else {
		//dbg_output ("c");                 //jaw  debug  remove
		bStatus = FALSE;
    }
    return bStatus;
}



BOOL ScanDriverAlterLights(DWORD nCommand,ScanDriverOpenContext_t *pOptions,DWORD nValue) {
	BOOL bStatus = FALSE;
	EnterCriticalSection(&csScanningInitiation);
	switch(nCommand) {
		case HHPSD_IOCTL_SET_ILLUMINATORS_STATE:
			bStatus = LightControlIlluminationState(pOptions,(BOOL)nValue);
			break;
		case HHPSD_IOCTL_SET_AIMER_STATE:
			bStatus = LightControlAimerState(pOptions,(BOOL)nValue);
			break;
		case HHPSD_IOCTL_SET_ILLUMINATOR_INTENSITY:
			bStatus = LightControlIlluminationIntensity(pOptions,nValue);
			break;
		case HHPSD_IOCTL_SET_AIMER_INTENSITY:
			bStatus = LightControlAimerIntensity(pOptions,nValue);
			break;
		case HHPSD_IOCTL_PREVENT_AIMER_IN_IMAGES:
			bStatus = LightControlAutomaticAimerControl(pOptions,nValue);
			break;
		default:
			break;
	}
	LeaveCriticalSection(&csScanningInitiation);
	return bStatus;
}

ScanDriverOpenContext_t * ScanDriverScanningMasterMode(void) {
	return pMasterModeContext;
}

BOOL ScanManagerReconfigureEngine(ScanDriverOpenContext_t *pOptions) {
	BOOL bStatus = FALSE;
	EnterCriticalSection(&csScanningInitiation);
	/* If we are in the midst of scanning, we only want to let the
	 * master mode context holder reconfigure the engine. */
	if (bScanDriverScanningIsEnabled == TRUE) {
		if (pOptions != pMasterModeContext) {
			LeaveCriticalSection(&csScanningInitiation);
			SetLastError(ERROR_NOT_OWNER);
			return bStatus;
		}
		HWLAYER.ResetScanTransfer();
		bScanDriverForceRestart = TRUE;
	}
	bStatus = HHPSD.ResetEngine();
	if ((bStatus == TRUE)&&(bScanDriverScanningIsEnabled == TRUE)) {
		if (TryEnterCriticalSection(&csScanBufferLock)) {
			ResetEvent(hBufferLockOutTime);
			LeaveCriticalSection(&csScanBufferLock);
		}
	}
	LeaveCriticalSection(&csScanningInitiation);
	return bStatus;
}

#define USE_SOF_PSOC_WRITE_THREAD_IST		1		//added here for testing addition of thread for SOF PSOC write


void ScanDriverVsyncISTroutine(void) {
	nLineISTnumber = 0;
	if (TryEnterCriticalSection(&csScanningInitiation)) {
		if (bScanDriverScanningIsEnabled == TRUE) 
		{
			#if	(USE_SOF_PSOC_WRITE_THREAD_IST == 0)
				PsocWriteFlashCurrent( nLineISTnumber );
			#endif

			OemHandleCellPhoneLights( nLineISTnumber );
			LightControlAutomaticAimerControl(nLineISTnumber);
		}
		//dbg_output("V\r\n");                 //jaw  debug  remove
		//TimeSinceTrigger();                  //jaw  debug  remove
		ScanManagementFrameBoundry(bScanDriverScanningIsEnabled);
		LeaveCriticalSection(&csScanningInitiation);
	}
}

void ScanDriverImageLineDataISTroutine(void) {
	DWORD nValidRows;
	/* Count the number of line ISTs for a given frame */
	nLineISTnumber++;
	if (TryEnterCriticalSection(&csScanningInitiation)) {
		if (bScanDriverScanningIsEnabled == TRUE) {
			nValidRows = HWLAYER.GetNumberOfValidRows();
			OemHandleCellPhoneLights( nLineISTnumber );
			
			/* If we have lighting aimer control, use it. */
			LightControlAutomaticAimerControl(nLineISTnumber);

/*		#if defined(OEM_ENGINE)
			HHPSD.ScanManagerAutoPSOCCurrentCalc();
		#endif*/

			//dbg_output("ValidRows = %d\r\n", nValidRows);            //lmc   debug

			/* Be sure that we still have image to process, and also be sure that the update cycle didn't
			 * NULL out the filling buffer because we have no new buffer to place scans. */
			if ((WaitForSingleObject(hBufferLockOutTime,0) == WAIT_OBJECT_0) && (pFillingBuffer != NULL)) {
				HHPSD.ScanManagerAutoExpLineSample(pFillingBuffer->pDriverBuffer,nValidRows);
			}
			ScanManagementUpdateCycle(nValidRows);
		}
		LeaveCriticalSection(&csScanningInitiation);
	}
}


void ScanDriverSofISTroutine(void) 
{
	if (TryEnterCriticalSection(&csScanningInitiation)) 
	{
		if (bScanDriverScanningIsEnabled == TRUE) 
		{
			#if	(USE_SOF_PSOC_WRITE_THREAD_IST == 1)
				PsocWriteFlashCurrent( nLineISTnumber );
			#endif
			//OemHandleCellPhoneLights( nLineISTnumber );			
		}
		
		LeaveCriticalSection(&csScanningInitiation);
	}
}


// ScanDriverPreEngineCommunication -- means by which to keep external PSOC communication under same critical section
BOOL ScanDriverPreEngineCommunication(void) {
	BOOL bStatus = TRUE;
	EnterCriticalSection(&csScanningInitiation);
	return bStatus;
}

// ScanDriverPostEngineCommunication -- means by which to keep external PSOC communication under same critical section
void ScanDriverPostEngineCommunication(void) {
	LeaveCriticalSection(&csScanningInitiation);
}


// spd added power control ... location was to reside next to the suspend and resume functions below
BOOL IsScanDriverPoweredOff(void){
    return (bScanDriverIsPoweredOff);
}

BOOL ScanDriverPowerOff(void) {
#if 0
    if (HWLAYER.ImagerPowerOff)
    {
        ScanDriverSuspendOperations();
        return TRUE;
    }
    else
        return FALSE;
#else
    if (HWLAYER.ImagerPowerOff && HHPSD_ScanEnginePowerDown){ // while we assume device is asleep, put asleep before powering off
        if (HHPSD_ScanEnginePowerDown() && HWLAYER.ImagerPowerOff(0))
        {
            bScanDriverIsPoweredOff=TRUE;
            return TRUE;
        }
        else
            return FALSE;
    }
    else
        return FALSE;
#endif
}

BOOL ScanDriverPowerOn (void) {
#if 0
    if (HHPSD_ImagerPowerOff)
    {
        ScanDriverRestoreOperations();
        return TRUE;
    }
    else
        return FALSE;
#else
    if (HWLAYER.ImagerPowerOn && HHPSD_ScanEngineWaitMs){
        if (IsScanDriverPoweredOff())       // only power up if powered off, either condition returns TRUE
        {
            bScanDriverIsPoweredOff=FALSE;
            HWLAYER.ImagerPowerOn(0);
        }
        return TRUE;
    }
    else
        return FALSE;
#endif

}


//#ifdef _WIN32_WCE
/*-----------------------------------------------------------------------------
 * ScanDriverSuspendOperations - This function is called when the "power down"
 *   function is called in the driver interface of WinCE.  This function should
 *   store the scanning state, and suspend all functionality.
 *-----------------------------------------------------------------------------*/
void ScanDriverSuspendOperations(void) {
    if (HWLAYER.IsPowerOffAndSuspendSupported()) {
        if (bScanDriverScanningIsEnabled == TRUE)
            bScanDriverForceRestart = TRUE;
        HWLAYER.ImagerPowerOffAndSuspend(1);
    }
    else {
        if (bScanDriverScanningIsEnabled == TRUE) {
            HWLAYER.ResetScanTransfer();
            bScanDriverForceRestart = TRUE;
        }
        HHPSD.ScanEngineSaveStateAndPowerDown();
        HWLAYER.ImagerPowerOff(1);
    }
}
/*-----------------------------------------------------------------------------
 * ScanDriverRestoreOperations - This function is called when the "power up"
 *   function is called in the driver interface of WinCE.  This function should
 *   restore the scanning state, and all functionality.
 *-----------------------------------------------------------------------------*/
void ScanDriverRestoreOperations(void) {
    if (HWLAYER.IsPowerOnAndResumeSupported()) {
        HWLAYER.ImagerPowerOnAndResume(1);
        HHPSD.ScanEnginePowerUpAndRestoreRegs();
    }
    else {
        HWLAYER.ImagerPowerOn(1);
        HHPSD.ScanEnginePowerUpAndRestoreState();
    }
}
//#endif
