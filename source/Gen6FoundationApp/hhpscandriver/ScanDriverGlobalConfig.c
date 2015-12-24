/*------------------------------------------------------------------

*-------------------------------------------------------------------
* ScanDriverGlobalConfig.c - This file deals with all of the system
*   level configuration that can be done in the driver.  This is
*   apart from the local configuration that can be done by a given
*   client of the driver, but deals with system level configurations
*   that not only affect all clients, but are not configurable by a
*   client application through the IOCTL interface.
*-------------------------------------------------------------------
* $RCSfile: ScanDriverGlobalConfig.c $
* $Revision: 1.6 $
* $Date: 2009/09/23 17:45:34EDT $
*--------------------------------------------------------------------
*--------------------------------------------------------------------*/
#if defined(_WIN32_WCE) || defined(PALMOS) || defined(_MSC_VER)
#include <windows.h>
#else // DBALL
#include <stdlib.h>
#include "language.h"
#include "platform.h"
#endif
#include "ScanDriverScannerSelect.h"
#include "ScanDriverGlobalConfig.h"


#ifdef SD_NAMESPACE
   namespace SD_NAMESPACE {
#endif

GlobalConfig_t gDriverConfigs = {
	GLOBAL_CONFIG_DEFAULT_PREVENT_SIMULTANEOUS_ILLUMINATION_AND_AIMER,
	GLOBAL_CONFIG_DEFAULT_IMAGER_IDLE_POWER_LEVEL
};

void UpdatePowerSettings(void) {
	gDriverConfigs.bPreventSimultaneousIlluminationAndAimer = HWLAYER.LightsLowPowerMode();
	gDriverConfigs.nImagerIdlePowerLevel					     = HWLAYER.ImagerIdlePowerLevel();
}

#ifdef SD_NAMESPACE
   } // namespace SD_NAMESPACE {
#endif
