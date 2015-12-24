#ifndef SCAN_DRIVER_BYD_HW_LAYER_H
#define SCAN_DRIVER_BYD_HW_LAYER_H


#include "ScanDriverHWLayer.h"

struct BYDRegisterEntry_t;

struct BYDHardwareLayerInterface : public HardwareLayerInterface
{

	BOOL  (*WriteIIC)(unsigned char,const unsigned short *,unsigned int);
	BOOL  (*ReadIIC)(unsigned char,unsigned short *,unsigned int);


	BOOL (*WriteIIC_SpreadSpec_w_ADD)(unsigned char , const unsigned char * ,unsigned int  );
	BOOL  (*WriteIIC_SpreadSpec)( const unsigned char * ucBuffer, unsigned int nCount );
	BOOL  (*ReadIIC_SpreadSpec)(unsigned char *ucBuffer, DWORD offset, unsigned int nCount);


	BOOL  (*InDownTime)(void);
	BOOL  (*WaitForVsync)(void);
	DWORD (*GetRegisterSettingsForHHPExposureControl)(const BYDRegisterEntry_t **);

	void  (*RequestSynchronizedPowerDown)( unsigned char, const unsigned short *, unsigned int );
};

#endif // #ifndef SCAN_DRIVER_BYD_HW_LAYER_H


