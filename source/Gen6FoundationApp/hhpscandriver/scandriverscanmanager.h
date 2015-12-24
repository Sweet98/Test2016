/*-------------------------------------------------------------------

 *-------------------------------------------------------------------
 * ScanDriverScanManager.h - This is the header file with prototypes
 *   for the functions that deal with registering and requesting a
 *   scan.
 *-------------------------------------------------------------------
 * $RCSfile: scandriverscanmanager.h $
 * $Revision: 1.23.1.3 $
 * $Date: 2010/10/20 23:30:10EDT $
 *-------------------------------------------------------------------*/
#ifndef _SCAN_DRIVER_SCAN_MANAGER_H_
#define _SCAN_DRIVER_SCAN_MANAGER_H_

#ifdef SD_NAMESPACE
   namespace SD_NAMESPACE {
#endif


void * zalloc(unsigned bytes);
void zfree(void * p);

BOOL AllocatePhysicalBufferAndCreateNewInfoEntry(void);
BOOL RegisterVirtualScanBuffer(ScanDriverOpenContext_t *pOpenCntxt,void *pVirtualScanBuffer);
BOOL UnregisterVirtualScanBuffer(ScanDriverOpenContext_t *pOpenCntxt,void *pVirtualScanBuffer);
BOOL ScanManagementRequestHandler(ScanDriverOpenContext_t *pOpenCntxt,void *pAppBuffer);
BOOL ScanManagementBufferReturn(ScanDriverOpenContext_t *pOpenCntxt,void *pAppBuffer);
BOOL ScanManagerAttachToSameBuffer(ScanDriverOpenContext_t *pOpenCntxt, void *pSourceBuffer,void *pDuplicateBuffer);
BOOL InitializeScanManager(void);
BOOL DeinitializeScanManager(void);
void ScanManagementUpdateCycle(DWORD nRowsPresent);
void ScanManagementFrameBoundry(BOOL bScanningEnabled);
BOOL ScanDriverStartScanning(ScanDriverOpenContext_t *pOptions, BOOL bEngineAutoShutdown);
BOOL ScanDriverStopScanning(ScanDriverOpenContext_t *pOptions,BOOL bEngineAutoShutdown);
ScanDriverOpenContext_t * ScanDriverScanningMasterMode(void);
BOOL ScanManagerNewScanAvailable(ScanDriverOpenContext_t *pOptions);
BOOL ScanDriverAlterLights(DWORD nCommand,ScanDriverOpenContext_t *pOptions,DWORD nValue);
BOOL ScanManagerReconfigureEngine(ScanDriverOpenContext_t *pOptions);
BOOL ScanDriverSingleCapture(ScanDriverOpenContext_t *pOptions, void *pAppBuffer);
BOOL ScanDriverPrepareChangeConfig(ScanDriverOpenContext_t *pOptions);
void ScanDriverCleanupChangeConfig(ScanDriverOpenContext_t *pOptions);
BOOL ScanDriverIsScanning(void);

BOOL ScanDriverPreEngineCommunication(void);
void ScanDriverPostEngineCommunication(void);

BOOL IsScanDriverPoweredOff(void);
BOOL ScanDriverPowerOff(void);
BOOL ScanDriverPowerOn (void);

//#ifdef _WIN32_WCE
void ScanDriverSuspendOperations(void);
void ScanDriverRestoreOperations(void);
//#endif

extern void (*HHPSD_ScanManagerAutoExpLineSample)(void *pImage,DWORD nRowsComplete);
extern void (*HHPSD_ScanManagerAutoExpCalc)(void);

/*#if defined(OEM_ENGINE)
extern void (* HHPSD_ScanManagerAutoPSOCCurrentCalc)(void);
#endif*/

extern BOOL (*HHPSD_ScanManagerImageIsAcceptable)(ScanDriverLastScanInfo_t *pStats);
extern void (*HHPSD_ScanManagerFrameEventAutoExposure)(void);
extern DWORD (*HHPSD_ScanManagerEngineSaysOkToCaptureNextFrame)(void);
extern void (*HHPSD_ExposureSettingsUpdate)(ScanDriverOpenContext_t *pOptions);
extern void (*HHPSD_ExposureSettingsInitialize)(ScanDriverOpenContext_t *pOptions);

#ifdef SD_NAMESPACE
   } // namespace SD_NAMESPACE {
#endif


#endif
