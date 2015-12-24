/************************************************************************/
//		This module performs Photo taking functions
//
// $RCSfile: Gen6FoundationApp/FoundationApp/Imaging/imaging.h $
// $Revision: 1.11 $
// $Date: 2011/02/14 14:30:37EST $
//======================================================================================
#ifndef __2D_IMAGING_H
#define __2D_IMAGING_H

extern BOOL PhotoJustTaken;


void ShowExpoReport(void);
void GrabFrame(void);
void TakePhoto(void);
bool ImageSnap(void);
bool ImageSnapWithParameters(char **ppParamList, unsigned int *pParamListLength );
void SendChasteImage(int X, int Y, unsigned char *img);
bool RawImageShip(void);
bool ImageShip(void);
void ImageSnapAndShip(void);
bool ProcessImageShipStyleCommand(UCHAR *ImageToBeSent, int XDimension, int YDimension, bool subimage);

//menu command test functions

bool ImageTest(void);
bool TakeTwoImages(void);
bool ImageRegisterRead(void);
bool ImageRegisterWrite(void);
bool RowNoiseReduction(void);

//Non menu command test functions

bool ImageShipSimple(int iShipType);
void Take1dImage(void);


//#ifdef DEVELOPMENT_SETTINGS
void SendValueToBeColorGraphed(int value, int fullscale, int color);
//#endif

#endif
