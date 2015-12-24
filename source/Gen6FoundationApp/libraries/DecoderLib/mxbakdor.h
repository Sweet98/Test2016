// "MxBakDor.h" -- "Backdoor" Interface for the "Matrix" Decoder


#ifndef _INC_MXBAKDOR
 #define _INC_MXBAKDOR

#include "mxapi.h"

#ifndef _INC_MXWRKSPC
typedef struct {
	int x;	// x-coordinate
	int y;	// y-coordinate
} Point;
#endif

#define EXCEPTION_QUERIES 1

#ifdef	EXCEPTION_QUERIES

/***********************************************************************/
EXTRA_STUFF void HHP_DisplaySettings(void * decoder, void (*CopyResult)(const char * bfr, int result_len));
// Formats all decoder settings into a string buffer that is returned via the specified the callback.
// Note that the bfr passed to the callback function is only in scope until HHP_DisplaySettings()
// returns, so be sure to copy bfr in the callback, not just save a pointer to it.

/***********************************************************************/

EXTRA_STUFF void HHP_DisplayInternalResults(void * decoder, void (*CopyResult)(const char * bfr, int result_len));
// Formats the current state of the internal result buffer
//	into a string buffer that is returned via the specified the callback.
// Note that the bfr passed to the callback function is only in scope until HHP_DisplayInternalResults()
// returns, so be sure to copy bfr in the callback, not just save a pointer to it.

#endif

/***********************************************************************/

EXTRA_STUFF void MakeTimerCounts (void* decoder, ScanProfile* sp, TimerCounts* tc, int MaxNumTC);
// Performs Scanprofile "Binarization"
// "MaxNumTc" is the length of the TimerCount array which has been allocated
// NB: This function is NOT needed to Decode any "Images" -- it's automatically called
// as needed within Decode() -- but serves other useful auxilliary purposes

/***********************************************************************/

EXTRA_STUFF int CheckScanStand (void* decoder, void* image, unsigned int* GetPixelValue);
// Returns Light(min) - Dark(max) among ScanStand symbol element locations in new *image

/***********************************************************************/

EXTRA_STUFF int GetDecodeSearchTime(void* decoder);

#ifdef HHP_DLL
EXTRA_STUFF void SampleImage (Workspace *ws);

EXTRA_STUFF int VBit2 (Workspace *ws, S32 X, S32 Y);
#endif
/***********************************************************************/
#if defined(PRINT_TIMER_COUNTS_MATRIX) && !defined(BCB)
TimerCounts *GetTimerCounts( void *Decoder, int array );
#endif

#ifdef VERIFIER
TimerCounts *GetTimerCounts(void *Decoder);
int GetFirstTimerCount(void *Decoder);
int GetLastTimerCount(void *Decoder);
int IsReverse(void *Decoder);
int GetSymbolData(void *Decoder, int *dest);
void GetRssElements(void *Decoder, int *w, int ch);
char GetSubCodeID(void *Decoder);
int GetScaleFactor(void *Decoder);
int GetPdfRows(void *Decoder);
int GetPdfCols(void *Decoder);
int GetPdfEcl(void *Decoder);
int GetPdfArray(void *Decoder, int *dest);
int GetMpdSegment(void *Decoder, int *dest);
int GetMpdArray(void *Decoder, int *dest, int *rows, int *cols, int *ecc, int *first, int *rotation);
#endif

#endif //_INC_MXBAKDOR

/*----------------------------------------------------------------------*
 * $RCSfile: mxbakdor.h $
 * $Revision: 4.20 $
 * $Date: 2009/02/25 09:37:06EST $
 *
 *----------------------------------------------------------------------*/

