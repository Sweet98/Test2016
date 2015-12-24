//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Imaging/Image_Send_Binarize_Himage.h $
// $Revision: 1.3 $
// $Date: 2009/04/24 17:44:03EDT $
//======================================================================================
#ifndef _INC_IMAGING_BINARIZE_HIMAGE
#define _INC_IMAGING_BINARIZE_HIMAGE

#ifndef RFBASE


#include	"language.h"
#include	"HImage.h"



//	SendImage (overloaded version #1)
bool SendImage(HIMAGE *ImageParameters, UCHAR *imgptr, int ncols, int nrows, bool UseInversionSetting = true, bool OverrideShipImagesIn640x480VGAMode = false );



//	SendImage (overloaded version #2)
bool SendImage(HIMAGE *ImageParameters = NULL, bool UseInversionSetting = true, bool OverrideShipImagesIn640x480VGAMode = false );



class BINARIZE_HIMAGE : public HIMAGE
{
public:

	//constructor
	BINARIZE_HIMAGE(UCHAR *ImageData, int InputXDimension, int InputYDimension);

	//destructor
	~BINARIZE_HIMAGE(void);

	virtual bool Binarize(BITSTRING *InputImage, BITSTRING *OutputImage, int XDimension, int YDimension, bool UseDecoderThresholds);

private:

	SINT BinaryImage(BYTE *pLocalImg,ULONG *pBinImg,SINT nTopx,SINT nTopy, BOOL /* bUseCurrentThresholds */ );
};


#endif
#endif
//======================================================================================
//	Revision information
//======================================================================================
