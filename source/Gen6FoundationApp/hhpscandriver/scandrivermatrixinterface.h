/*------------------------------------------------------------------

*-------------------------------------------------------------------
* ScanDriverMatrixInterface.c - This is the interface file for Matrix
*       firmware. This is the only file needed to be included when
*       access to the scandrive is needed for Matrix firmware.
*-------------------------------------------------------------------
* $RCSfile: scandrivermatrixinterface.h $
* $Revision: 1.4.1.5 $
* $Date: 2009/10/23 17:45:32EDT $
*-------------------------------------------------------------------*/

#ifndef _SCANDRIVER_MATRIX_INTERFACE_H
#define _SCANDRIVER_MATRIX_INTERFACE_H

#include "scandriverinterface.h"


#define P_SCANBUFFER (*ppCurrentScanBuffer)
/***** System Wide Globals for application code  *****/

extern volatile unsigned nOpenCntxt; // "open context" for the scan driver" opened in main called elswhere

//Switchable pointers
extern UCHAR **ppCurrentScanBuffer;
extern UCHAR **ppIQHoldScanBuffer;
extern UCHAR **ppTempScanBuffer;

//Pointers to register with scandriver buffers
extern UCHAR *pGlobalImage1;
extern UCHAR *pGlobalImage2;

//global reference for the current image buffer
#define P_SCANBUFFER (*ppCurrentScanBuffer)
//global reference for the current image buffer
#define P_SIGCAPBUFFER (*ppIQHoldScanBuffer)

//same things as above, Gen 6 uses a couple more
#ifdef GEN6_DEVELOPMENT
extern UCHAR **ppRecentScanBuffer;
extern UCHAR *pGlobalImage3;
#define P_RECENTBUFFER (*ppRecentScanBuffer)
#endif

#if defined(GEN6_DEVELOPMENT) || CONCURRENT
extern UCHAR **ppFastDecodeBuffer;
extern UCHAR *pGlobalImage4;
#define P_FASTBUFFER (*ppFastDecodeBuffer)
int GetBestNewImage(PBYTE *ppScanBuffer); //sets the current scan buffer to the one it should use, may not be the most recent
int GetBestNewImage(void); //sets the current scan buffer to the one it should use, may not be the most recent
#endif

extern BOOL	(*fImageRegisterRead)(void);
extern BOOL	(*fImageRegisterWrite)(void);

//Used by MAIN to configure scan driver
extern void (*pfSetForCachedImaging)(bool);
extern unsigned int ScanDriverContextResult;	//For scandriver context

#endif //_SCANDRIVER_MATRIX_INTERFACE_H


