/*------------------------------------------------------------------

*-------------------------------------------------------------------
* ScanDriverGlobalConfig.h - This file deals with all of the system
*   level configuration that can be done in the driver.  This is
*   apart from the local configuration that can be done by a given
*   client of the driver, but deals with system level configurations
*   that not only affect all clients, but are not configurable by a
*   client application through the IOCTL interface.
*-------------------------------------------------------------------
* $RCSfile: ScanDriverGlobalConfig.h $
* $Revision: 1.3 $
* $Date: 2009/09/23 17:45:31EDT $
*--------------------------------------------------------------------
*--------------------------------------------------------------------*/
#ifndef _GLOBAL_CONFIG_H_
#define _GLOBAL_CONFIG_H_

#define IMAGER_POWER_OFF	0
#define IMAGER_POWER_IDLE	1
#define IMAGER_POWER_RUN	2

#define GLOBAL_CONFIG_DEFAULT_PREVENT_SIMULTANEOUS_ILLUMINATION_AND_AIMER FALSE
#define GLOBAL_CONFIG_DEFAULT_IMAGER_IDLE_POWER_LEVEL							  IMAGER_POWER_OFF

#ifdef SD_NAMESPACE
   namespace SD_NAMESPACE {
#endif


typedef struct GlobalConfig{
	BOOL bPreventSimultaneousIlluminationAndAimer;
	int  nImagerIdlePowerLevel;
} GlobalConfig_t;

extern GlobalConfig_t gDriverConfigs;

void UpdatePowerSettings(void);

#ifdef SD_NAMESPACE
   } // namespace SD_NAMESPACE {
#endif

#endif
