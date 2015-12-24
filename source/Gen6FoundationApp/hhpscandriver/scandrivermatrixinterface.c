/*------------------------------------------------------------------

*-------------------------------------------------------------------
* ScanDriverMatrixInterface.c - This is the interface file for Matrix
*       firmware.
*-------------------------------------------------------------------
* $RCSfile: scandrivermatrixinterface.c $
* $Revision: 1.3.1.3 $
* $Date: 2009/02/04 13:25:06EST $
*-------------------------------------------------------------------*/

#include "language.h"

/***** System Wide Globals for application code  *****/

volatile unsigned nOpenCntxt; // "open context" for the scan driver" opened in main called elswhere

//Switchable pointers
UCHAR **ppCurrentScanBuffer;
UCHAR **ppIQHoldScanBuffer;
UCHAR **ppTempScanBuffer;

//Pointers to register with scandriver buffers
UCHAR *pGlobalImage1;
UCHAR *pGlobalImage2;

//same things as above, Gen 6 uses a couple more
#ifdef GEN6_DEVELOPMENT
UCHAR **ppRecentScanBuffer;
UCHAR *pGlobalImage3;
#endif

#if defined(GEN6_DEVELOPMENT) || CONCURRENT
UCHAR **ppFastDecodeBuffer;
UCHAR *pGlobalImage4;
#endif

BOOL	(*fImageRegisterRead)(void);
BOOL	(*fImageRegisterWrite)(void);

//Used by MAIN to configure scan driver
void (*pfSetForCachedImaging)(bool);
unsigned int ScanDriverContextResult;	//For scandriver context


