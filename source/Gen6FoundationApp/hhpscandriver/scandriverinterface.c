/*------------------------------------------------------------------

*-------------------------------------------------------------------
* ScanDriverInterface.c - This is the device driver interface file
* for the Windows CE imaging system
*-------------------------------------------------------------------
* $RCSfile: scandriverinterface.c $
* $Revision: 1.62 $
* $Date: 2010/01/15 13:58:48EST $
*-------------------------------------------------------------------*/
#if defined(_WIN32_WCE)
#include <windows.h>
#include <winerror.h>
#elif defined(PALMOS)
#include <windows.h>
#else // DBALL
#include "db_time.h"
#include <stdlib.h>
#include "string.h"
#include "stdio.h"
#include "language.h"
#include "syncobjects.h"
#include "uchar.h"
#include "tchar.h"
#include "wchar.h"
#define 	TCHAR tchar_t
#endif

#include "ScanDriverInterface.h"
#include "ScanDriverOpenContext.h"
#include "ScanDriverScannerSelect.h"
#include "ScanDriverScanManager.h"
#include "ScanDriverLightControl.h"
#include "ScanDriverGlobalConfig.h"
#include "ScanDriverDebug.h"
#include "ScanDriverEngine.h"

#include "Ioman.h"
// #include "ScanDriverInterfaceDebug.c"

static CRITICAL_SECTION csHHPScanDriverInterface;

typedef struct OpenInstanceHolder{
	ScanDriverOpenContext_t	*pOpenContextEntry;
	struct OpenInstanceHolder *pNext;
}OpenInstanceHolder_t;

static OpenInstanceHolder_t OpenInstanceList={0,NULL};
#if (ESHELL_DEBUG || DBALL_DEBUG)
#define PRINTLIST() {																								\
						OpenInstanceHolder_t *a;																	\
						a = OpenInstanceList.pNext;																	\
						while(a != NULL) {																			\
							DBG_OUTPUT3("pOpenContext:0x%x,pList:0x%x,pNext:0x%x\n",(DWORD)a->pOpenContextEntry,	\
											(DWORD)a,(DWORD)a->pNext);												\
							a = a->pNext;																			\
						}																							\
					}
#else
#define PRINTLIST()
#endif



const   TCHAR   szAPIRev[] = _T("HHPSD $ProjectRevision: 1.129.3.87 $");


/*-------------------------------------------------------------------
 * NewOpenInstance - This creates a new open instance entry, and
 *    returns a pointer to it.
 *-------------------------------------------------------------------*/
static ScanDriverOpenContext_t *NewOpenInstance(void) {
	OpenInstanceHolder_t *pListLocation;
	/* First we need to find the end of the list */
//DBG_OUTPUT("\n\n---NEW OPEN INSTANCE---\n")
	pListLocation = &OpenInstanceList;
	while (pListLocation->pNext != NULL) {
		pListLocation = pListLocation->pNext;
	}
	/* Next we allocate memory for the new instance itself */
	pListLocation->pNext = (OpenInstanceHolder_t *)zalloc(sizeof(OpenInstanceHolder_t));
	/* If the instance creation was successful, go on*/
	if (pListLocation->pNext != NULL) {
		pListLocation = pListLocation->pNext;
		/* This is the new end of the list, and we create a new open context for it*/
		pListLocation->pNext = NULL;
		pListLocation->pOpenContextEntry = ScnDrv_CreateOpenContext();
		if (pListLocation->pOpenContextEntry == NULL) {
DBG_OUTPUT("Creation of Open Context FAILED\n");
			zfree(pListLocation->pOpenContextEntry);
			zfree(pListLocation);
			pListLocation = NULL;
		} else {
//DBG_OUTPUT("Success\n");
//PRINTLIST();
			return pListLocation->pOpenContextEntry;
		}
	}
	return NULL;
}
/*-------------------------------------------------------------------
 * IsOpenContextListEmpty -- This reports whether the context list is emtpy.
 * Information important with the addition of power on / off flexibilities
 *-------------------------------------------------------------------*/
// spd added to know if the IsOpenContextListIsEmpty()
// Necessary with the addition of various power off functionality, ie don't want to power
// off if your context thinks it should if other contexts are open.
//
// Return	FALSE	List not empty
//			TRUE	List empty
BOOL IsOpenContextListEmpty()
{
	BOOL status = FALSE;
	OpenInstanceHolder_t *pListLocation;
	pListLocation = &OpenInstanceList;
	while (pListLocation->pNext != NULL)
	{
		DBG_OUTPUT3 ("Pointer %d, Entry %d, pNext %d\r\n", pListLocation,pListLocation->pOpenContextEntry, pListLocation->pNext);
		pListLocation = pListLocation->pNext;
	}
		DBG_OUTPUT3 ("Pointer %d, Entry %d, pNext %d\r\n", pListLocation,pListLocation->pOpenContextEntry, pListLocation->pNext);
	if (pListLocation->pNext == NULL && pListLocation->pOpenContextEntry == 0) // last entry in list
	{
		status = TRUE;
	}
	else
	{
		status = FALSE;
/*#ifdef ESHELL_DEBUG
		while (pListLocation.pNext != NULL)
		{

		}
#endif*/
	}
DBG_OUTPUT1 ("IOCLE %d\r\n",status );
	return status;
}
/*-------------------------------------------------------------------
 * DeleteOpenInstance - This deletes a given open instance, if it
 *   exists
 *-------------------------------------------------------------------*/
static DWORD DeleteOpenInstance(ScanDriverOpenContext_t *pOpenContext) {
	OpenInstanceHolder_t *pListLocation,*pListLocationLast;
	/* First we attempt to find the entry in the Open Instances*/
	pListLocation = OpenInstanceList.pNext;
	pListLocationLast = &OpenInstanceList;
//DBG_OUTPUT("\n\n---DELETE OPEN INSTANCE---\n")
//DBG_OUTPUT1("Open Context To Find:0x%x\n",(DWORD)pOpenContext);
//PRINTLIST();
	while (pListLocation != NULL) {
		if (pListLocation->pOpenContextEntry == pOpenContext) {
			break;
		}
		pListLocationLast = pListLocation;
		pListLocation = pListLocation->pNext;
	}
	/* If we find it, we delete the Open Context within it.*/
	if (pListLocation != NULL) {

		/* Here we will call the scan manager to get rid of the entries associated with this open context */

		if (!ScnDrv_DeleteOpenContext(pListLocation->pOpenContextEntry)) {
			return FALSE;
		}
		/* If we're deleting the first entry in the list, we have to revamp our
		 * starting point*/
//DBG_OUTPUT1("Remove Entry:0x%x\n",(DWORD)pListLocation);
		/* If not, we simply remove the entry from the list*/
		pListLocationLast->pNext = pListLocation->pNext;
		/* Now we delete the physical memory of the Open Instance */
		zfree(pListLocation);
// spd added for power control
// spd if no instances left after the one above was deleted, power OFF if configured to do so
		if (IsOpenContextListEmpty()) // no other instances exist
      {
	DBG_OUTPUT ("DOI - 2\r\n");
       	if (!IsScanDriverPoweredOff())// && gDriverConfigs.bPowerOffIdle)
			{
				DBG_OUTPUT ("DOI - 3\r\n");
				ScanDriverPowerOff();
			}
    	}

		return TRUE;
	}
	return FALSE;
}

/*-------------------------------------------------------------------
 * ValidateOpenInstance - Here we make sure that the "Open Context"
 *   passed in by a user is a valid one in our list.
 *
 *  return value
 *			TRUE:passed
 *			FALSE:failed
 *-------------------------------------------------------------------*/
static DWORD ValidateOpenInstance(ScanDriverOpenContext_t *pOpenContext) {
	OpenInstanceHolder_t *pListLocation;
	pListLocation = OpenInstanceList.pNext;
	while (pListLocation != NULL) {
		if (pListLocation->pOpenContextEntry == pOpenContext) {
			return TRUE;
		}
		pListLocation = pListLocation->pNext;
	}
	return FALSE;
}
/*-------------------------------------------------------------------
 * GetOpenContextEntry - This function allows you to go through the
 *   different open context pointers in the case where one needs
 *   information from an open context other than the current one.
 *   This is primarily used for lights control because when we are
 *   not scanning, the lights are in a state which is a correlation of
 *   all of the open contexts.
 *
 * THE PROTOTYPE FOR THIS FUNCTION IS IN ScanDriverLightControl.h
 *   because the ScanDriverInterface.h file is used outside the driver
 *   to allow people to link against the driver.  We are not showing
 *   this function to the outside world, therefore we don't want it
 *   in that file.
 *
 * argument -  A pointer to an open context, if this value is NULL,
 *  it gives the starting open context.  If it is not NULL, it goes
 *  to the next open context.
 *
 * return value - NULL - either you've reached the end of the list or
 *   the pointer passed in was invalid
 *   non-zero - a valid open context pointer.
 *-------------------------------------------------------------------*/
ScanDriverOpenContext_t *GetOpenContextEntry(ScanDriverOpenContext_t *pStart) {
	OpenInstanceHolder_t *pListLocation;
	pListLocation = OpenInstanceList.pNext;
	if (pListLocation != NULL) {
		if (pStart != NULL) {
			while	(pListLocation != NULL) {
				/* If we found the open context which matches what the caller sent
				 * in then we go to the next entry to return that. */
				if (pListLocation->pOpenContextEntry == pStart) {
					pListLocation = pListLocation->pNext;
					break;
				}
				pListLocation = pListLocation->pNext;
			}
		}
		/* Have to check for NULL again incase we incremented to the
		 * end of the list in the while loop. */
		if (pListLocation != NULL) {
			return pListLocation->pOpenContextEntry;
		}
	}
	return NULL;
}
/*-------------------------------------------------------------------
 * SCN_Init - This function is called when the driver is first loaded
 *   by the device manager.
 *
 * Parameter - dwContext - This could contain a pointer to a string
 *   identifying the Active key created by the device manager for the
 *   driver.  However, using RegisterDevice can pass any value, so
 *   don't depend on that
 *
 * Return Value - non-zero indicates a successful load  The idea is
 *   that if multiple instances of this driver can exist, we'll return
 *   a handle that can be used to open and deinit.
 *-------------------------------------------------------------------*/
static DWORD dwHHPScanDriverInitialized = 0;
DRIVER_API DWORD SCN_Init(DWORD dwContext) {
 	// DBG_OUTPUT("SCN_Init\n");
//dbg_output("6");
#ifndef _WIN32_WCE
// *Temp* fix for engine diagnostic mode needing to have control of things right from init as pixel clock is updated for efficiency on init
	bPsocDiagnosticMode = dwContext;
	//DBG_OUTPUT1 ("*SCN_Init diagnostic mode = %d\r\n",bPsocDiagnosticMode);

#endif
	dwContext = 0;

	/* if we have already been initialized, return 0*/
	if (dwHHPScanDriverInitialized != 0) {
		SetLastError(ERROR_ALREADY_INITIALIZED);
		ERRMSG ("SCN_DRV: SCN_Init ERROR_ALREADY_INITIALIZED\r\n");
		return 0;
	}
//dbg_output("7");
	/* Verify that any hardware that this driver access will function
	 * correctly */
	/* Initialize the hardware */
	/* Initialize the hardware state */
	if (!AutoDetectScanEngine()) {
		SetLastError(ERROR_BAD_DEVICE);
		ERRMSG ("SCN_DRV:  SCN_Init ERROR_BAD_DEVICE\r\n");
		return 0;
	}
	/* Initialize the scan driver for usage */
	if (!InitializeScanManager()) {
		DeinitializeScanManager();
		ERRMSG("SCN_DRV: SCN_Init InitializeScanManager fail\r\n");
		return 0;
	}
	/* We need a critical section code segment incase 2 threads are trying
	 * to open at once, or one trying to open, while one is trying to close
	 * etc. */
	InitializeCriticalSection(&csHHPScanDriverInterface);
//DBG_OUTPUT ("Calling HHPSD_ImagerPowerOff()\r\n");
	ScanDriverPowerOff();
	dwHHPScanDriverInitialized = 1;
	return dwHHPScanDriverInitialized;
}
/*-------------------------------------------------------------------
 * SCN_Deinit - This is called when the driver is unloaded.
 *
 * Parameters:
 * hDeviceContext - This is really only important when
 *   multiple instances of the device driver are created.  It is the
 *   value that is passed back from us on an "Init" to tell us which
 *   instance to unload.
 *
 * Return Value - Non-zero on success - no real significance
 *-------------------------------------------------------------------*/
DRIVER_API BOOL SCN_Deinit(DWORD /*hDeviceContext*/) {
 	// DBG_OUTPUT("IMG_Deinit\n");
	if (dwHHPScanDriverInitialized == 0) {
		SetLastError(ERROR_SERIAL_NO_DEVICE);
		// DBG_OUTPUT("- Not Initialized : FAIL\n");
		return 0;
	}
	/* Power down any hardware it uses */
	/* free any memory associated with this */

	/* Make sure that we have truly stopped scanning, if we haven't
	 * this will take care of it, if we have, this will have no effect. */
	DeinitializeScanManager();
	/* Delete the critical section used for opening and closing */
	DeleteCriticalSection(&csHHPScanDriverInterface);
	HWLAYER.ImagerPowerOff(0);
	UnloadHardwareLibrary();
	dwHHPScanDriverInitialized = 0;
	return TRUE;
}
/*-------------------------------------------------------------------
 * SCN_Open - This function is called when an application or another
 *   driver calls CreateFile to open the driver
 *
 * Parameters:
 * hDeviceContext - This is the value returned by the init function
 *   it can be used as a handle for multiple instances
 * AccessCode -
 * ShareMode - These 2 parameters are taken directly from CreateFile's
 *   dwDesiredAccess and dwShareMode parameters and indicate how the
 *   application wants to access (read/write) and share
 *   (FILE_SHARE_READ or FILE_SHARE_WRITE) the device.
 *
 * Return Value - non-zero on successful open - In the case where
 *   multiple applications want to use this instance of the driver,
 *   we return an "open context" to every application that calls
 *   this.  This is used to know which application is writing
 *   to, and reading from this driver etc.  If only one application
 *   is allowed to call at a time, return the hDeviceContext value
 *-------------------------------------------------------------------*/
DRIVER_API DWORD SCN_Open(DWORD /*hDeviceContext*/, DWORD /*AccessCode*/, DWORD /*ShareMode*/){
	ScanDriverOpenContext_t *pOptions;
	/* allocate memory, or set up hardware that is used when an application
	 * opens an individual context of a device instance. (i.e. we have
	 * instances of the driver itself, when the driver can be used for
	 * multiple pieces of hardware by calling the Init function, and
	 * multiple open instances where multiple applications can open a
	 * single instance of the driver calling this function with the
	 * appropriate hDeviceContext and getting back an OpenContext)*/

	if (dwHHPScanDriverInitialized == 0) {
		SetLastError(ERROR_SERIAL_NO_DEVICE);
		ERRMSG ("SCN_DRV:SCN_Open ERROR_SERIAL_NO_DEVICE\r\n");
		return 0;
	}

	/* We need to do our initialization of the single open-instance in
	 * a critical section incase 2 threads are trying to do this at the same
	 * time. */
	EnterCriticalSection(&csHHPScanDriverInterface);
	/* Allocate the memory for the local storage of the open-context */
	pOptions = NewOpenInstance();
	/* The allocation failed. */
	if (pOptions == NULL) {
		ERRMSG ("SCN_DRV:SCN_Open ERROR_OPEN_FAILED\r\n");
		SetLastError(ERROR_OPEN_FAILED);
		LeaveCriticalSection(&csHHPScanDriverInterface);
		return 0;
	}
	LeaveCriticalSection(&csHHPScanDriverInterface);
	return (DWORD)pOptions;
}
/*-------------------------------------------------------------------
 * SCN_Close - This function is called when an application or driver
 *   that had previously opened the driver closes it.
 *
 * Parameters:
 * hOpenContext - This is the open context value that the driver
 *   returned from the Open call.  This tells the driver which
 *   "open instance" of a particular driver instance to shut down.
 *
 * Return Value - Non-zero on success - no real significance
 *-------------------------------------------------------------------*/
DRIVER_API BOOL SCN_Close(DWORD hOpenContext) {
	ScanDriverOpenContext_t *pOptions;

	if (dwHHPScanDriverInitialized == 0) {
		SetLastError(ERROR_SERIAL_NO_DEVICE);
		ERRMSG ("SCN_DRV: SCN_Close ERROR_SERIAL_NO_DEVICE\r\n");
		return 0;
	}
	/* We need to do our check of the context in a critical section incase
	 * somebody is trying to close, while another is trying to open, etc.*/
	EnterCriticalSection(&csHHPScanDriverInterface);
	pOptions = (ScanDriverOpenContext_t *)hOpenContext;
	/* Make sure everything goes okay with the delete */
	if (!DeleteOpenInstance(pOptions)) {
		ERRMSG ("SCN_DRV: SCN_Close ERROR_INVALID_NAME\r\n");
		LeaveCriticalSection(&csHHPScanDriverInterface);
		SetLastError(ERROR_INVALID_NAME);
		return FALSE;
	}
// DEBUG	ScanDriverStopScanning((ScanDriverOpenContext_t *)hOpenContext);
// DEBUG	HHPSD.IlluminationOff(); HHPSD.AimerOff();
	LeaveCriticalSection(&csHHPScanDriverInterface);
	// DBG_OUTPUT("SCN_Close\n");
	return TRUE;
}
/*-------------------------------------------------------------------
 * SCN_Read - This function is called when an application or driver
 *   which has an "open instance" of this driver calls the ReadFile
 *   function.
 *
 * Parameters:
 * hOpenContext - This is the "open instance" value returned by the
 *   IMG_Open function.
 * pBuffer - This is the location where the "read" data is to be
 *   copied into.
 * Count - This is the size of the buffer to make sure that we don't
 *   overwrite it.
 *
 * Return Value - The number of bytes that were placed into pBuffer.
 *   >0 - means data was read
 * 	0 - means no data was read
 *   -1 - means an error has occurred
 *-------------------------------------------------------------------*/
DRIVER_API DWORD SCN_Read(DWORD /*hOpenContext*/, LPVOID /*pBuffer*/, DWORD /*Count*/) {
	/* Check to make sure this is a valid buffer for reading by the
	 * application */
//	if (IsBadReadPtr(pBuffer,Count)) {
//		SetLastError(ERROR_INVALID_PARAMETER);
//		return -1;
//	}
	/* Place as many bytes of information into pBuffer as is needed, or
	 * as can be handled (i.e. check Count for size) */

	/* return the number of bytes that were entered into pBuffer. */
	// DBG_OUTPUT("IMG_Read\n");
	return 0;
}
/*-------------------------------------------------------------------
 * SCN_Write - This function is called when an application or driver
 *   which has an "open instance" of this driver calls the WriteFile
 *   function.
 *
 * Parameters:
 * hOpenContext - This is the "open instance" value returned by the
 *   IMG_Open function
 * pBuffer - This is the buffer containing the information that is
 *   to be written.
 * Count - This is how many bytes are in the buffer.  Typically this
 *   is set to the number of bytes that are desired written, but if
 *   some protocol is set up in the buffer, then the information in
 *   the buffer would tell where the end of the data is as a posed
 *   to the end of the buffer.
 *
 * Return Value - Returns the number of bytes written
 *  >0 - means data was written
 *   0 - means no data was written
 *  -1 - means an error occurred.
 *-------------------------------------------------------------------*/
DRIVER_API DWORD SCN_Write(DWORD /*hOpenContext*/, LPVOID /*pBuffer*/, DWORD /*Count*/) {
	/* Check to make sure this is a valid buffer for writing by the
	 * application. i.e. this buffer really has information that was
	 * written to it by the application */
//	if (IsBadWritePtr((void *)pBuffer,Count)) {
//		SetLastError(ERROR_INVALID_PARAMETER);
//		return -1;
//	}
	/* Pull as many bytes of information from pBuffer as is needed, or
	 * as can be handled (i.e. check Count for size) */

	/* return the number of bytes that were extracted from pBuffer. */
	// DBG_OUTPUT("IMG_Write\n");
	return 0;
}
/*-------------------------------------------------------------------
 * SCN_Seek - This function is called when an application or driver
 *   which has an "open instance" of this driver calls the
 *   SetFilePointer function
 *
 * Parameters:
 * hOpenContext - This is the "open instance" value returned by the
 *   IMG_Open function
 * Amount - This is the absolute offset value to move in the file
 * Type - This is the "type" of seek. There are three types
 * 		1) FILE_BEGIN
 * 		2) FILE_CURRENT
 * 		3) FILE_END
 *
 * Return Value - non-zero if successful
 *
 * ## Chances are we won't be doing anything with this function ##
 * ## but it must be supported in a device driver					 ##
 *-------------------------------------------------------------------*/
DRIVER_API DWORD SCN_Seek(DWORD /*hOpenContext*/, long /*Amount*/, WORD /*Type*/) {
	// DBG_OUTPUT("IMG_Seek\n");
	return 0;
}
/*-------------------------------------------------------------------
 * SCN_PowerDown - This function is called by the OS when the system
 *   is about to suspend.  This should power down any hardware that
 *   this driver controls, and save any necessary hardware states.
 * NOTE: NO CALLS CAN BE MADE TO ANY WIN32 API FUNCTIONS IN THIS
 *   DEVICE DRIVER FUNCTION
 *
 *
 * Parameters:
 * hDeviceContext - This is the device context handle returned by the
 *   init call
 *
 * Return Value - <none>
 *-------------------------------------------------------------------*/
DRIVER_API void SCN_PowerDown(DWORD /*hDeviceContext*/) {
	if (dwHHPScanDriverInitialized)
	/* This will power down the imager and the supporting hardware */
		ScanDriverSuspendOperations();
	/* Call the
	 * BOOL SetInterruptEvent(DWORD idInt)
	 * function to signal the event that the driver's interrupt service
	 * thread is waiting for.  We'll have an escape sequence in there
	 * that will terminate the IST */
}
/*-------------------------------------------------------------------
 * SCN_PowerUp - This function is called when a system comes out of
 *   suspend mode.
 * NOTE: NO CALLS CAN BE MADE TO ANY WIN32 API FUNCTIONS IN THIS
 *   DEVICE DRIVER FUNCTION
 *
 * Parameters:
 * hDeviceContext - This is the device context handle returned by the
 *   init call
 *
 * Return value - <none>
 *-------------------------------------------------------------------*/
DRIVER_API void SCN_PowerUp(DWORD /*hDeviceContext*/) {
	if (dwHHPScanDriverInitialized)
	/* This will reinstate the imager and the supporting hardware */
		ScanDriverRestoreOperations();
	/* Only enable minimal hardware support here, people will have
	 * to do an IMG_Open before we actually power things up.*/
}
/*-------------------------------------------------------------------
 * SCN_IOControl - This is the primary interface entry point for the
 *   driver, and is called when the application calls DeviceIOControl
 *
 * Parameters:
 * hOpenContext - This is the "open instance" value returned by the
 *   IMG_Open function
 * dwCode - This is a value defined by us passed by the application
 *   indicating what the application wants done.
 * pBufIn - This parameter points to the input buffer that contains
 *   the data being passed to the driver
 * dwLenIn - This defines the length of the data beeing passed by
 *   pBufIn
 * pBufOut - This is the pointer to the output buffer
 * dwLenOut - This is the size of the output buffer
 * pdwActualOut - This is a pointer that points to a location to store
 *   the number of bytes that were actually written to the output
 *   buffer
 *
 * Return Value - TRUE 	= successfule
 * 				  FALSE = failure
 *-------------------------------------------------------------------*/
DRIVER_API BOOL SCN_IOControl(DWORD hOpenContext, DWORD dwCode, PBYTE pBufIn, DWORD dwLenIn,
		PBYTE pBufOut, DWORD dwLenOut, PDWORD pdwActualOut) {
	BOOL bStatus = FALSE;
	ScanDriverOpenContext_t *pOptions;
	HHPScanDriverIOCTL_t nScanDriverIOCTL;

	/* This really cannot happen unless we are running as a DLL, but it will
	 * blow up as soon as it hits the critical section if we are not initialized
	 * so let's catch that case here. */
	if (dwHHPScanDriverInitialized == 0) {
		SetLastError(ERROR_SERIAL_NO_DEVICE);
ERRMSG ("SCN_DRV:SCN_IOControl Driver Not Initialized\r\n");
		return 0;
	}
	// DBG_OUTPUT("SCN_IOControl\n");
	pOptions = (ScanDriverOpenContext_t *)hOpenContext;
	//theStdInterface.printf("Exposure Mode: 0x%x,dwCode:0x%x\r\n",pOptions->nAutoExposureMode,dwCode);
	EnterCriticalSection(&csHHPScanDriverInterface);
	/* First we'll try to validate the open context passed to us */
	if (ValidateOpenInstance(pOptions) == FALSE) {
		SetLastError(ERROR_INVALID_NAME);
		LeaveCriticalSection(&csHHPScanDriverInterface);
ERRMSG ("SCN_DRV:SCN_IOControl ValidateOpenInstance Fail\r\n");
		return FALSE;
	}

	/* This will be our verification line right here */
	if ((pOptions->nKey) != (DWORD)pOptions) {
		SetLastError(ERROR_INVALID_NAME);
		LeaveCriticalSection(&csHHPScanDriverInterface);
ERRMSG2("SCN_DRV:SCN_IOControl Keys didn't match (0x%x,0x%x)\r\n",(DWORD)pOptions->nKey,(DWORD)pOptions);
		return FALSE;
	}

	nScanDriverIOCTL = (HHPScanDriverIOCTL_t)dwCode;
	UpdatePowerSettings();
	switch(nScanDriverIOCTL) {
		case HHPSD_IOCTL_START_SCANNING:
			//dbg_output("Start Scanning\r\n");
			DBGM(16,) dbg_output("HHPSD_IOCTL_START_SCANNING\r\n");
			if (!ScanDriverStartScanning(pOptions,FALSE)) break;
			bStatus = TRUE;
			break;
		case HHPSD_IOCTL_STOP_SCANNING:
		{
			DBGM(16,) dbg_output("+ HHPSD_IOCTL_STOP_SCANNING\r\n");
			unsigned start = current_time();
			bStatus = TRUE;
			if (!ScanDriverStopScanning(pOptions,FALSE)){
				// DBG_OUTPUT("---SCN_IOControl:Failed to StopScanning\n");
				bStatus = FALSE;
			}
			DBGM(16,) dbg_output("- HHPSD_IOCTL_STOP_SCANNING ellapsed %u\r\n",
								 current_time() - start);
			break;
		}
		case HHPSD_IOCTL_GET_NEW_SCAN:
			// DBG_OUTPUT("---SCN_IOControl:HHPSD_IOCTL_GET_SCAN\n");
			if (ScanManagementRequestHandler(pOptions,pBufIn)) {
				bStatus = TRUE;
			}
			break;
		case HHPSD_IOCTL_RETURN_SCAN:
			if (ScanManagementBufferReturn(pOptions,pBufIn)) {
				bStatus = TRUE;
			}
			break;
		case HHPSD_IOCTL_REGISTER_SCAN_BUFFER:
			// DBG_OUTPUT("---SCN_IOControl:HHPSD_IOCTL_REGISTER_SCAN_BUFFER\n");
// attempting to make all OEM things the same, this is a point of contention currently.
// Casio wants to not power on on connect
// Motorola wants to power on on connect
// other choices for doing this:
			// add Global config for PowerOnConfig
			// have parameters to this Ioctl that controls this
			// both preferred over conditional compile.  At this point being chosen for ease
			// and because the OEM is variant build anyway.  Note, Casio has 3 builds that this
			// conditional compile must apply to (StandardSDK/SH3,StandardSDK/ARM4Vi, DT-X10/ARMv4t)
#ifdef	WIN32_PLATFORM_PSPC
#if (WIN32_PLATFORM_PSPC >= 400)
			if (IsScanDriverPoweredOff())		// doing this here saves the SI and OEM API instantiation from power up/down to getImagerProperties
				ScanDriverPowerOn();
#endif
#endif
			if (RegisterVirtualScanBuffer(pOptions,pBufIn)) {
				bStatus = TRUE;
			}
			break;
		case HHPSD_IOCTL_UNREGISTER_SCAN_BUFFER :
			// DBG_OUTPUT("---SCN_IOControl:HHPSD_IOCTL_UNREGISTER_SCAN_BUFFER\n");
			if (UnregisterVirtualScanBuffer(pOptions,pBufIn)) {
				bStatus = TRUE;
			}
			break;
		case HHPSD_IOCTL_GET_SCAN_HEIGHT:
			// DBG_OUTPUT("---SCN_IOControl:HHPSD_IOCTL_GET_SCAN_HEIGHT\n");
			if (pBufOut != NULL){
				 *(DWORD *)pBufOut = HWLAYER.GetScanHeight();
				if (pdwActualOut != NULL) *pdwActualOut = 4;
				bStatus = TRUE;
			}
			break;
		case HHPSD_IOCTL_GET_SCAN_WIDTH:
			// DBG_OUTPUT("---SCN_IOControl:HHPSD_IOCTL_GET_SCAN_WIDTH\n");
			if (pBufOut != NULL) {
				*(DWORD *)pBufOut = HWLAYER.GetScanWidth();
				if (pdwActualOut != NULL) *pdwActualOut = 4;
				bStatus = TRUE;
			}
			break;
        case HHPSD_IOCTL_SET_ILLUMINATORS_STATE:
            // DBG_OUTPUT("---SCN_IOControl:HHPSD_IOCTL_SET_ILLUMINATORS_STATE\n");
//            bStatus = ( *((PBOOL)pBufIn) == FALSE ) ? HHPSD_IlluminationOff() : HHPSD_IlluminationOn();
			bStatus = ScanDriverAlterLights(nScanDriverIOCTL,pOptions,*((DWORD *)pBufIn));
            break;
        case HHPSD_IOCTL_SET_AIMER_STATE:
            // DBG_OUTPUT("---SCN_IOControl:HHPSD_IOCTL_SET_AIMER_STATE\n");
 //           bStatus = ( *((PBOOL)pBufIn) == FALSE ) ? HHPSD_AimerOff() : HHPSD_AimerOn();
			bStatus = ScanDriverAlterLights(nScanDriverIOCTL,pOptions,*((DWORD *)pBufIn));
            break;
		case HHPSD_IOCTL_SET_ILLUMINATOR_INTENSITY:
	 		bStatus = ScanDriverAlterLights(nScanDriverIOCTL,pOptions,*((DWORD *)pBufIn));
			break;
		case HHPSD_IOCTL_SET_AIMER_INTENSITY:
			bStatus = ScanDriverAlterLights(nScanDriverIOCTL,pOptions,*((DWORD *)pBufIn));
            break;
		case HHPSD_IOCTL_PREVENT_AIMER_IN_IMAGES:
			bStatus = ScanDriverAlterLights(nScanDriverIOCTL,pOptions,*((DWORD *)pBufIn));
            break;
        case HHPSD_IOCTL_SET_EXPOSURE_SETTINGS:
            // DBG_OUTPUT("---SCN_IOControl:HHPSD_IOCTL_SET_EXPOSURE_SETTINGS\n");
			bStatus = ScnDrv_SetExposureSettings((ScanDriverOpenContext_t *)hOpenContext, (DWORD *)pBufIn, dwLenIn);
            break;
        case HHPSD_IOCTL_GET_EXPOSURE_SETTINGS:
            // DBG_OUTPUT("---SCN_IOControl:HHPSD_IOCTL_GET_EXPOSURE_SETTINGS\n");
			bStatus = ScnDrv_GetExposureSettings((ScanDriverOpenContext_t *)hOpenContext, (DWORD *)pBufIn, dwLenIn);
			if (pdwActualOut != NULL) *pdwActualOut = dwLenIn;
            break;
        case HHPSD_IOCTL_SET_SCAN_LIGHTS_MODE:
            // DBG_OUTPUT("---SCN_IOControl:HHPSD_IOCTL_SET_SCAN_LIGHTS_MODE\n");
			bStatus = FALSE;
			SetLastError(ERROR_NOT_SUPPORTED);
			break;
		case HHPSD_IOCTL_GET_ENGINE_TYPE:
            // DBG_OUTPUT("---SCN_IOControl:HHPSD_IOCTL_GET_ENGINE_TYPE\n");
			bStatus = ScnDrv_GetEngineType((ScanDriverOpenContext_t *)hOpenContext,(DWORD *)pBufOut);
         if (pdwActualOut != NULL) *pdwActualOut = 4;
         break;
        case HHPSD_IOCTL_SET_EXPOSURE_MODE:
            // DBG_OUTPUT("---SCN_IOControl:HHPSD_IOCTL_SET_EXPOSURE_MODE\n");
			bStatus = ScnDrv_SetExposureMode((ScanDriverOpenContext_t *)hOpenContext, (DWORD *)pBufIn);
			break;
		case HHPSD_IOCTL_GET_EXPOSURE_MODE:
			bStatus = ScnDrv_GetExposureMode((ScanDriverOpenContext_t *)hOpenContext, (DWORD *)pBufIn);
			break;
		case HHPSD_IOCTL_NEW_IMAGE_AVAILABLE:
			bStatus = ScanManagerNewScanAvailable(pOptions);
			break;
		case HHPSD_IOCTL_GET_IMAGE_ATTRIBUTES:
			/*First make sure we have something to give to the function, if not, return a failure. */
			if ((pBufIn == NULL)||(pBufOut == NULL)||(pdwActualOut==NULL)||(dwLenOut==0)) {bStatus = FALSE; break; }
			*pdwActualOut = ScnDrv_GetImageAttributes((ScanDriverOpenContext_t *)hOpenContext, (void *)pBufIn,(DWORD *)pBufOut,dwLenOut);
			if (*pdwActualOut != 0) bStatus = TRUE; else bStatus = FALSE;
			break;
		case HHPSD_IOCTL_RECONFIG_ENGINE:
			bStatus = ScanManagerReconfigureEngine(pOptions);
			break;
        case HHPSD_IOCTL_SINGLE_FRAME_CAPTURE:
            // DBG_OUTPUT("---SCN_IOControl:HHPSD_IOCTL_SINGLE_FRAME_CAPTURE\n");
			 bStatus = ScanDriverSingleCapture((ScanDriverOpenContext_t *)hOpenContext,(void *)pBufIn);
			 break;
        case HHPSD_IOCTL_GET_REVISION_STRING:
            // DBG_OUTPUT("---SCN_IOControl:HHPSD_IOCTL_GET_REVISION_STRING\n");
		      _tcscpy((TCHAR *)pBufOut, szAPIRev);
            bStatus = TRUE;
            break;
	   case HHPSD_IOCTL_GET_IMAGER_PROPERTIES:
		   bStatus = FALSE;
		   if (  dwLenOut % (2 * sizeof(DWORD)) == 0
				 && dwLenIn >= dwLenOut - sizeof(DWORD)
				 && dwLenIn <= dwLenOut  )
		   {
			   bStatus = ReportImagerProperties((const DWORD *)pBufIn, (DWORD *)pBufOut, dwLenOut);
		   }

		   break;

		case HHPSD_IOCTL_SET_ENGINE_CONFIG:
            DBG_OUTPUT("---SCN_IOControl:HHPSD_IOCTL_SET_PSOC_SETTINGS\n");
			bStatus = ScnDrv_SetEngineConfig((ScanDriverOpenContext_t *)hOpenContext, (DWORD *)pBufIn, dwLenIn);
            break;

		case HHPSD_IOCTL_GET_ENGINE_CONFIG:
            DBG_OUTPUT("---SCN_IOControl:HHPSD_IOCTL_GET_PSOC_SETTINGS\n");
			bStatus = ScnDrv_GetEngineConfig((ScanDriverOpenContext_t *)hOpenContext, (DWORD *)pBufIn, dwLenIn);
			if (pdwActualOut != NULL) *pdwActualOut = dwLenIn;
            break;

		case HHPSD_IOCTL_WRITE_PSOC_REG:
		            DBG_OUTPUT("---SCN_IOControl:HHPSD_IOCTL_WRITE_PSOC_REG\n");
			if (ScanDriverPreEngineCommunication()) {
				if (IsScanDriverPoweredOff())		// make sure sensor is powered on
					ScanDriverPowerOn();
				bStatus = PsocWriteRegisters((DWORD) *pBufIn, (PBYTE) pBufOut, (BYTE) dwLenOut);
				ScanDriverPostEngineCommunication();
			}
			else
				bStatus = FALSE;
			break;

		case HHPSD_IOCTL_READ_PSOC_REG:
		            DBG_OUTPUT("---SCN_IOControl:HHPSD_IOCTL_READ_PSOC_REG\n");
			if (ScanDriverPreEngineCommunication()) {
				if (IsScanDriverPoweredOff())		// make sure sensor is powered on
					ScanDriverPowerOn();
				bStatus = PsocReadRegisters((DWORD) *pBufIn, (PBYTE) pBufOut, (BYTE) dwLenOut);
				ScanDriverPostEngineCommunication();
			}
			else
				bStatus = FALSE;
			break;

		case HHPSD_IOCTL_WRITE_PSOC: // raw
		            DBG_OUTPUT("---SCN_IOControl:HHPSD_IOCTL_WRITE_PSOC\n");
			if (ScanDriverPreEngineCommunication()) {
				if (IsScanDriverPoweredOff())		// make sure sensor is powered on
					ScanDriverPowerOn();
				bStatus = PsocWriteBytes((PBYTE) pBufIn, (BYTE) dwLenIn);
				ScanDriverPostEngineCommunication();
			}
			else
				bStatus = FALSE;
			break;

		case HHPSD_IOCTL_READ_PSOC:
		            DBG_OUTPUT("---SCN_IOControl:HHPSD_IOCTL_READ_PSOC\n");
			if (ScanDriverPreEngineCommunication()) {
				if (IsScanDriverPoweredOff())		// make sure sensor is powered on
					ScanDriverPowerOn();
				bStatus = PsocReadBytes((PBYTE) pBufOut, (BYTE) dwLenOut);
				ScanDriverPostEngineCommunication();
			}
			else
				bStatus = FALSE;
			break;


		case HHPSD_IOCTL_ENGINE_DIAGNOSTIC_MODE:
			            DBG_OUTPUT1("---SCN_IOControl:HHPSD_ENGINE_DIAGNOSTIC_MODE %d\n",*pBufIn);
			if (ScanDriverPreEngineCommunication()){
				if (IsScanDriverPoweredOff())		// make sure sensor is powered on
					ScanDriverPowerOn();
				bStatus = PsocSetupDiagnosticMode((BYTE) *pBufIn);
				ScanDriverPostEngineCommunication();
			}
			else
				bStatus = FALSE;
			break;

		case HHPSD_IOCTL_ENGINE_HAS_PSOC:
			bStatus = bPsocPresent;
			break;

	    case HHPSD_IOCTL_SET_LIGHT_SETTINGS:
			DBGM(16,) dbg_output("HHPSD_IOCTL_SET_LIGHT_SETTINGS\r\n");
			if (dwLenIn % (sizeof(DWORD) * 2) != 0)
				bStatus = FALSE;
			else
			{
				if (ScanDriverPreEngineCommunication()){
					bStatus = LightControlSetTags(  pOptions, (DWORD *)pBufIn, dwLenIn  );
					ScanDriverPostEngineCommunication();
				}
			}
			break;
	    case HHPSD_IOCTL_GET_LIGHT_SETTINGS:
			if (dwLenIn != dwLenOut || dwLenIn % (sizeof(DWORD) * 2) != 0)
				bStatus = FALSE;
			else
			{
				if (ScanDriverPreEngineCommunication()){
					bStatus = LightControlGetTags(  pOptions, (DWORD *)pBufIn, (DWORD *)pBufOut, dwLenIn  );
					ScanDriverPostEngineCommunication();
				}
			}
			break;
		case HHPSD_IOCTL_SCANNING_ENABLED:
			bStatus = ScanDriverIsScanning();
			break;

		//*********************Test Flex/4600R added*******************************************************
 		case HHPSD_IOCTL_UPDATE_PSOC_REG:
			DBG_OUTPUT("---SCN_IOControl:HHPSD_IOCTL_UPDATE_PSOC_REG\n");
			if ( ScanDriverPreEngineCommunication() )
			{
				if ( IsScanDriverPoweredOff() ) ScanDriverPowerOn(); // make sure sensor is powered on

				bStatus = PsocUpdateRegisters((DWORD) *pBufIn, (PBYTE) pBufOut, (BYTE) dwLenOut);
				ScanDriverPostEngineCommunication();
			}
			else
				bStatus = FALSE;

			break;
		case HHPSD_IOCTL_RESET_PSOC:
			bStatus = EngineReset();
			break;
		case HHPSD_IOCTL_RELOAD_PSOC_REGS_COPY:  // have the ScanDriverEngine module reload its internal copy of the PSOC registers
			bStatus = PsocReadConfigArea();
			break;
 		case HHPSD_IOCTL_SET_IMAGER_GRAYSCALE:  // Set the imager to always output a grayscale ramp test image
			if ( pBufOut && (dwLenOut >= sizeof(DWORD)) )
			{
				DWORD status = HWLAYER.SetImagerModeToGrayscale();
				memcpy(pBufOut, &status, sizeof(DWORD));
				bStatus = TRUE;
			}
			else
				bStatus = FALSE;

			break;
		case HHPSD_IOCTL_UNSET_IMAGER_GRAYSCALE:  // Place imager out of it's test mode.
			if ( pBufOut && (dwLenOut >= sizeof(DWORD)) )
			{
				DWORD status = HWLAYER.UnSetImagerModeToGrayscale();
				memcpy(pBufOut, &status, sizeof(DWORD));
				bStatus = TRUE;
			}
			else
				bStatus = FALSE;

			break;
		 //*********************END Test Flex/4600R*******************************************************

		 case HHPSD_IOCTL_ATTACH_TO_SAME_BUFFER:
			bStatus = ScanManagerAttachToSameBuffer(pOptions,pBufOut,pBufIn);
			break;

		 default:
			dwLenIn = 0;
			dwLenOut = 0;
			// DBG_OUTPUT("---SCN_IOControl:default\n");
			break;
	}
	LeaveCriticalSection(&csHHPScanDriverInterface);

	if (!bStatus)
		ERRMSG1 ("SCN_DRV: SCN_IOCTL #%d FAIL\r\n", nScanDriverIOCTL);
	return bStatus;
}
