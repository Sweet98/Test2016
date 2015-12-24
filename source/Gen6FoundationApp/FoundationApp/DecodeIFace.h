
/*----------------------------------------------------------------------*
 *	       DecodeIface.h is the interface between the decoder and		*
 *	       the function that calls the decoder.									*
 *----------------------------------------------------------------------*
 *======================================================================*
 *   $RCSfile: Gen6FoundationApp/FoundationApp/DecodeIFace.h $
 *   $Date: 2011/08/22 14:56:58EDT $
 *   $Revision: 1.18 $
 *
 *======================================================================*

 *======================================================================*/
#ifndef DECODEIFACE_H
#define DECODEIFACE_H

typedef unsigned char /*UCHAR*/ PIXEL;
#include "platform.h"
#ifndef RFBASE
	#include "rapidscan.h"	//only used for GrayscaleImage2 anymore
#include "product.h"
#ifdef PLUGIN_DEV
#include "HONPluginConfiguration.h"
#endif

extern volatile int  ProcessedResult;
extern int NumberOfDecodes;
extern GrayscaleImage2 StoredImageDescriptor;
extern unsigned char *StoredImageArray;


class CBarcodeData;

// spd ImgBox support
typedef struct LastDecodeInfo
{
	unsigned char *pImg;
	int rows;
	int cols;
	int imgx;
	int imgy;
	Point corners[4];
	int dcorner;
	int pdfrows;
	int pdfcols;
	int codeid;
} LastDecodeInfo_t;



extern LastDecodeInfo_t * pgLastDecodeInfo;
//extern CBarcodeData *gpCurrentDecoded;


bool FillDecoderMenu (void);
void DecodeIFaceOnChangedConfiguration(void);
void decode (PIXEL PC_HUGE *img);
void DecodeForMenuing (PIXEL PC_HUGE *img);
void ClearDecoderWorkspace(void);
bool WasLastSymbolScanStand(void);		// last decode was a scan stand symbol
int CheckForScanStand(PIXEL PC_HUGE *img, unsigned int* GetPixelValue); // quick check for scan stand symbol
void ClearLastReadTimestamp(void);
void RefreshTimeStamp(char CodeID);
void ClearAllButLastReadTimestamp(void);                //clears all the re-read timestamps but the last one read
void DecoderPowerupInit();
void ResetIqScoring(void);

//ewr04791 Codegate
void FillDecoderWindowCenteringMenu (void);
MatrixError_t GetHHLCDataFromDecoder(PIXEL PC_HUGE *img,CBarcodeData * &gpCurrentDecoded);

void decode_2d_from_memory (GrayscaleImage2 *ImageDescriptor, int WithVigor);
void DecodeAndSaveImage(GrayscaleImage2 *ImageDescriptor);
void DecodeDownloadedImage (GrayscaleImage2 *ImageDescriptor);
void SaveThisImage(GrayscaleImage2 *ImageDescriptor);
void SaveDownloadedImage(GrayscaleImage2 *ImageDescriptor);

int GetImageWidth(void);
int GetImageHeight(void);
bool DecodeCurrentImage(void);
bool RedecodeSavedImage(void);
bool ResetDecoderWorkspace(void);

#ifdef VERIFIER
bool FillVerifierMenu(void);
void DecodeAnalog(PIXEL PC_HUGE *img, int length);
void DecodeTimerCounts(int *pTicks, int length);
#endif


#if defined( RFSCAN )
void SkipIndicatorsForThisMenuCommand( void );
void UseIndicatorsForThisMenuCommand( void );
#endif

#endif		//end #ifndef RFBASE

#endif		//end #ifndef DECODEIFACE_H



