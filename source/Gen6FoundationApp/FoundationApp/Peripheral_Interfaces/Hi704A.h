#ifndef _BYD_H_
#define _BYD_H_

#ifndef _BYD_HRDWR_H_
#include "Hi704A_hrdwr.h"
#endif



DWORD GetRegisterSettingsForHHPExposureControlBYD(const BYDRegisterEntry_t **);
BOOL WriteIIC_BYD(unsigned char subaddress, const unsigned short * uiBuffer,unsigned int nCount  );
BOOL ReadIIC_BYD(unsigned char subaddress, unsigned short * uiBuffer,unsigned int nCount  );


BOOL IsPsocSupportedByTokenBYD(void);
BOOL IsPsocInterfaceSupportedBYD(void);


BOOL ImagerPowerUpBYD(void);
BOOL ImagerPowerDownBYD(void);
DWORD	ImagerIsPoweredBYD(void);
void WaitMillisecondsBYD(DWORD nTime);
BOOL InDownTimeBYD(void);
BOOL  LedControlStateBYD(UINT State);
BOOL  AimIsOnBYD(void);
DWORD AimOnBYD(void);
DWORD AimOffBYD(void);
DWORD AimConfigBYD(DWORD nLevel);
DWORD IllumOnBYD(void);
DWORD IllumOffBYD(void);
BOOL  IllumIsOnBYD(void);
DWORD IllumConfigBYD(DWORD nLevel);
DWORD IllumOffTimerBYD(void);
DWORD GetScanWidthBYD(void);
DWORD GetScanHeightBYD(void);
DWORD AllocatePhysicalScanSpaceBYD(void);
DWORD FreePhysicalScanSpaceBYD(DWORD hImage);
DWORD BindPhysicalMemoryToVirtualMemoryBYD(DWORD hPhysical,void *pVirtual);
DWORD UnbindPhysicalMemoryFromVirtualMemoryBYD(DWORD hPhysical,void *pVirtual);

DWORD RegisterVsyncNotificationBYD(void(*fVsyncNotification)(void));
DWORD InitializeVsyncProcessingBYD(void);
DWORD DeinitializeVsyncProcessingBYD(void);
DWORD RegisterImageLineDataNotificationBYD(void(*fImageNotification)(void));
DWORD InitializeImageLineDataProcessingBYD(void);
DWORD DeinitializeImageLineDataProcessingBYD(void);
DWORD DisableVsyncProcessingBYD(void);
DWORD EnableVsyncProcessingBYD(void);
DWORD DisableImageLineDataProcessingBYD(void);
DWORD EnableImageLineDataProcessingBYD(void);
DWORD InitCaptureBYD(DWORD hPhysical, DWORD dPointerPassed=2 );
void ResetTransferHardwareBYD(void);
DWORD GetFrequencyPerFrameOfLineDataCallbackBYD(void);
DWORD GetNumberOfValidRowsBYD(void);
DWORD RegisterSofPsocWriteNotificationBYD(void(*fSofNotification)(void));

DWORD LightsLowPowerModeBYD(void);
BOOL  GetImagerPropertiesBYD(const DWORD *pBuffIn, DWORD *pBuffOut, DWORD dwLen);
DWORD ImagerIdlePowerLevelBYD(void);

void RequestSynchronizedPowerDownBYD( unsigned char, const unsigned short *, unsigned int );
BOOL SynchronizedPowerDownBYD(void);
BOOL WaitForVsyncBYD(void);

BOOL LoadCaptureInfoBYD(void);

DWORD SetImagerModeToGrayscaleBYD(void);       //test flex
DWORD UnSetImagerModeToGrayscaleBYD(void);    	//test flex
void UseCachedMemoryBYD(bool bValue);

extern void (*pfSetForCachedImaging)(bool);
extern void SendStringToInterface( const char *pucBuffer, int SizeOfBuffer);
extern "C"	void MMU_CleanAndInvalidate(void);
//for HI2D it for configure spread chip
bool configure_spread_chip();


static  void ROILeftRight(unsigned * left, unsigned * right);
static  void ROITopBottom(unsigned * top, unsigned * bottom);
static void ROILeftWidth(unsigned * left, unsigned * width);
static void ROITopHeight(unsigned * top, unsigned * height);

#endif

