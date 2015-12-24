//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Imaging/Image_Send_Binarize_Himage.cpp $
// $Revision: 1.19 $
// $Date: 2011/08/26 03:28:40EDT $
//======================================================================================
/************************************************************************/
/************************************************************************/

#include "Image_Send_Binarize_Himage.h"
#include "Matrixnonmenuhmodem.h"
#include "menu.h"
#include "readerconfiguration.h"
#include "productconfiguration.h"
#include "db_chrio.h"
#include "scandrivermatrixinterface.h"
#include "MatrixHComm.h"
#include "db_time.h"
#include "thread.h"
#include "MatrixSynFECommandHandler.h"
#include "RemRouter.h"


#ifdef DEVELOPMENT_SETTINGS
#include "db_heap.h"
#endif
#include "ImageCore.h"
#include "ScanEnginePSOC.h"
#include "HardwareToken.h"

extern void SendStringToInterface( const char *pucBuffer);

//
//	SendImage (overloaded version #1)
//		Send the image sent by the caller.
//
bool SendImage(HIMAGE *ImageParameters, UCHAR *pImage, int Xsize, int Ysize, bool UseInversionSetting, bool OverrideShipImagesIn640x480VGAMode )
{
	HSTRING		Command(20, true);
	int mytime1, mytime2, mytime3;
	int			ImageFormat;
	bool		ReturnStatus = true;	// Assume success until we know otherwise.

#ifdef DEVELOPMENT_SETTINGS

	ULONG EndHeapSpace;
	ULONG StartHeapSpace = ComputeFreeHeapSpace();
#endif

	mytime1 = current_time();

	// Do the HModem transmit.
	NonMenuHModem *hm = new NonMenuHModem();

	BINARIZE_HIMAGE * NewImage = 0;	// Indicate that NewImage is uninitialized.
	NewImage = new BINARIZE_HIMAGE(pImage, Xsize, Ysize);

	//char	MsgBuf[80];                                           //lmc   debug
	//sprintf (MsgBuf," Bin_Image ImageSize:%d XSize:%d YSize:%d \r\n", (int)NewImage->Data->Size, Xsize, Ysize ); //lmc   debug
	//xputstring (MsgBuf);                                        //lmc   debug

	// If the caller sent image parameters, use them.
	if((ImageParameters != NULL) && (ImageParameters->ValidateParameters() == HTAG::HTAG_OK))
	{
		NewImage->CopyParameters( ImageParameters );
		NewImage->JPEGCompressionSmoothingFactor = GET_SETTING(JPEGCompressionSmoothingFactor);
		NewImage->JPEGDecompressionBlockSmoothing = GET_SETTING(JPEGDecompressionBlockSmoothing);
		NewImage->FastJPEG = GET_SETTING(TransmitImageUseFastAlgorithmForJPEG);

		// Change the image format back to HTAG_IMAGE_UNFORMATTED_GRAYSCALE and save the format we're supposed to be converting to; otherwise,
		// FormatImage won't work.
		ImageFormat = NewImage->Format;
		NewImage->Format = HTAG_IMAGE::HTAG_IMAGE_UNFORMATTED_GRAYSCALE;
		/*
				NewImage->Depth = ImageParameters->Depth;
				NewImage->JPEGQualityFactor = ImageParameters->JPEGQualityFactor;
				NewImage->JPEGCompressionSmoothingFactor = GET_SETTING(JPEGCompressionSmoothingFactor);
				NewImage->JPEGDecompressionBlockSmoothing = GET_SETTING(JPEGDecompressionBlockSmoothing);
				NewImage->FastJPEG = GET_SETTING(TransmitImageUseFastAlgorithmForJPEG);
				NewImage->TransferProtocol = ImageParameters->TransferProtocol;
				ImageFormat = ImageParameters->Format;
		 
				NewImage->XAxisInversionApplied = ImageParameters->XAxisInversionApplied;
				NewImage->YAxisInversionApplied = ImageParameters->YAxisInversionApplied;
				NewImage->PixelIncrement = ImageParameters->PixelIncrement;
				NewImage->LeftBoundary = ImageParameters->LeftBoundary;
				NewImage->RightBoundary = ImageParameters->RightBoundary;
				NewImage->TopBoundary = ImageParameters->TopBoundary;
				NewImage->BottomBoundary = ImageParameters->BottomBoundary;
				NewImage->Margin = ImageParameters->Margin;
				NewImage->ImageFlatteningApplied = ImageParameters->ImageFlatteningApplied;
				NewImage->HistogramStretchApplied = ImageParameters->HistogramStretchApplied;
				NewImage->EdgeEnhancementApplied = ImageParameters->EdgeEnhancementApplied;
				NewImage->RotationApplied = ImageParameters->RotationApplied;
				NewImage->FlySpeckApplied = ImageParameters->FlySpeckApplied;
		      NewImage->SnazzyApplied = ImageParameters->SnazzyApplied;
				NewImage->GlitchFixApplied = ImageParameters->GlitchFixApplied;
				NewImage->RowNoiseApplied = ImageParameters->RowNoiseApplied;
				NewImage->UltraTextApplied = ImageParameters->UltraTextApplied;
		      NewImage->UltraTextApplied = ImageParameters->AdaptiveUltraTextApplied;
				NewImage->InfinityFilterApplied = ImageParameters->InfinityFilterApplied;
				NewImage->GammaCorrectionApplied = ImageParameters->GammaCorrectionApplied;
				NewImage->HistogramApplied = ImageParameters->HistogramApplied;
				NewImage->SmoothingApplied = ImageParameters->SmoothingApplied;
            NewImage->FissionApplied = ImageParameters->FissionApplied;
            NewImage->FusionApplied = ImageParameters->FusionApplied;
		*/
	}
	else
	{
		NewImage->ReportAllParameters( GET_SETTING( ReportAllHTAGParameters ) );
		NewImage->JPEGQualityFactor = GET_SETTING(JPEGQualityFactor);
		NewImage->JPEGCompressionSmoothingFactor = GET_SETTING(JPEGCompressionSmoothingFactor);
		NewImage->JPEGDecompressionBlockSmoothing = GET_SETTING(JPEGDecompressionBlockSmoothing);
		NewImage->FastJPEG = GET_SETTING(TransmitImageUseFastAlgorithmForJPEG);
		NewImage->TransferProtocol = GET_SETTING(TransmitImageProtocol);
		ImageFormat = GET_SETTING(TransmitImageFormat);

		NewImage->XAxisInversionApplied = GET_SETTING(InvertImageAboutXAxis) ? 1 : 0;
		NewImage->YAxisInversionApplied = GET_SETTING(InvertImageAboutYAxis) ? 1 : 0;
		NewImage->PixelIncrement = GET_SETTING(TransmitImageSubsampleIncrement);
		NewImage->LeftBoundary = GET_SETTING(TransmitImageWindowLeft);
		NewImage->RightBoundary = GET_SETTING(TransmitImageWindowRight);
		NewImage->TopBoundary = GET_SETTING(TransmitImageWindowTop);
		NewImage->BottomBoundary = GET_SETTING(TransmitImageWindowBottom);
		NewImage->Margin = GET_SETTING(TransmitImageMargin);
		NewImage->ImageFlatteningApplied = GET_SETTING(ImageFlattening) ? 1 : 0;
		NewImage->HistogramStretchApplied = GET_SETTING(ImageStretch) ? 1 : 0;
		NewImage->EdgeEnhancementApplied = GET_SETTING(ImageEdgeSharpenAmount);
		NewImage->RotationApplied = GET_SETTING(ImageRotationValue);
		NewImage->FlySpeckApplied = GET_SETTING(FlySpecks);
		NewImage->SnazzyApplied = GET_SETTING(Snazzy);
		NewImage->GlitchFixApplied = GET_SETTING(GlitchFix);
		NewImage->RowNoiseApplied = GET_SETTING(RowNoise);
		NewImage->UltraTextApplied = GET_SETTING(UltraText);
		NewImage->AdaptiveUltraTextApplied = GET_SETTING(AdaptiveUltraText);
		NewImage->InfinityFilterApplied = GET_SETTING(InfinityFilter) ? 1 : 0;
		NewImage->GammaCorrectionApplied = GET_SETTING(GammaCorrection);
		NewImage->HistogramApplied = GET_SETTING(Histogram) ? 1 : 0;
	}


	// Determine whether or not the image is supposed to be inverted.  This is independent of the IMGSHP inversion parameters, which are applied
	// AFTER this inversion is done in ModifyImage.  If the caller told us to leave the image alone, don't touch it.  This is used primarily by
	// signature capture, which doesn't want image to be modified from what the decoder saw.  Otherwise, we use the ROTATN setting to determine
	// whether or not the image should be inverted.  Note that there is a "true ^" before the ROTATN check; this is because "true" is meant to be
	// replaced eventually with a product configuration setting that should be XORed with ROTATN.  However, we have yet to implement the product
	// configuration setting.
	bool	InvertedImager = (UseInversionSetting) ? ((bool) (GET_SETTING(iEngineRotation) == IMAGER_UPSIDE_DOWN) ? true : false) : false;		// Soon to be controlled by product configuration.
//	RESET_WATCHDOG_TIMER();

	int		XAdjustment = 0;
	int		YAdjustment =  0;

	// If ShipImagesIn640x480VGAMode is enabled and it's not being overridden, adjust the image coordinates.
	if( ( !OverrideShipImagesIn640x480VGAMode ) && GET_SETTING( ShipImagesIn640x480VGAMode ) )
	{
		XAdjustment = GET_CONFIG_SETTING( iImageXCenter ) - 320;

		if( XAdjustment < 0 )
		{
			XAdjustment = 0;
		}

		if( ( XAdjustment + 640 ) > Xsize )
		{
			XAdjustment = Xsize - 640;
		}

		YAdjustment = GET_CONFIG_SETTING( iImageYCenter ) - 240;

		if( YAdjustment < 0 )
		{
			YAdjustment = 0;
		}

		if( ( YAdjustment + 480 ) > Ysize )
		{
			YAdjustment = Ysize - 480;
		}

		// Verify that boundary coordinates are in VGA range.  Don't bother to do any other sanity checks (less than zero, left boundary greater than
		// right boundary, etc.); ModifyImage will do that.
		if( ( NewImage->LeftBoundary >= 640 ) || ( NewImage->RightBoundary >= 640 ) ||
		        ( NewImage->TopBoundary >= 480 ) || ( NewImage->TopBoundary >= 480 ) )
		{
			ReturnStatus = false;
		}
	}
	else if (NewImage->GlitchFixApplied)                     // cosmetically change image to hide last row being black
	{
		int xxx = NewImage->XDimension;
		char *ptr = (char *)(NewImage->Data->Char + xxx-1);

		for (int i=0; i<NewImage->YDimension-1; i++)
		{
			*ptr = (char)(((int)*(ptr-1) + (int)*(ptr-1+xxx) + 1)>>1);
			ptr+=xxx;
		}

		*ptr = *(ptr-1);
	}

	if (ReturnStatus)
	{
		if(NewImage->RowNoiseApplied)
		{
			if (GET_SETTING(ImageEngineType) ==HHPSD_ENGINE_IMAGER_STM_VC602 || GET_SETTING(ImageEngineType) == HHPSD_ENGINE_IMAGER_STM_VC700)
			{
				if (NewImage->XDimension == 1280 && NewImage->YDimension == 960)
				{
					//             int time = BaselineClock;
					NewImage->RowNoise(GET_SETTING(RowNoiseTopRatio), GET_SETTING(RowNoiseBottomRatio));
					//             time = BaselineClock-time;
					//             printf("RowNoiseTime = %d\r\n",time);
				}
			}
		}
	}

	// We will do subsample later after fusion or fission.
	if(( ReturnStatus ) &&
	        (ModifyImage(NewImage->Data, &(NewImage->XDimension), &(NewImage->YDimension), &(NewImage->StartColorPattern),&(NewImage->OffsetWidth),&(NewImage->OffsetHeight), InvertedImager, (NewImage->XAxisInversionApplied) ? true : false,
	                     (NewImage->YAxisInversionApplied) ? true : false, 1, NewImage->LeftBoundary + XAdjustment,
	                     NewImage->RightBoundary + XAdjustment, NewImage->TopBoundary + YAdjustment, NewImage->BottomBoundary + YAdjustment, NewImage->Margin,
	                     (NewImage->SmoothingApplied) ? true : false)))
	{
		// Get Engine ID
		int ImagerType = 0;
		int ColorImage = 0;

		if(HardwareToken.IAmOEMJade())
		{
			// read engine ID from PSOC
			PSoC_5100_REGISTER_TABLE	Registers;
			Psoc5100GetRegisters( &Registers );

			ImagerType = (Registers.EngineID.b.Imager == 1 ? E2V_EV76C454_Jade_Monocolor : E2V_EV76C454_Jade);
		} 
		else
		{
			// read imager type from configuration
			ImagerType = GET_CONFIG_SETTING(ImageSensor);
		}

		if((ImagerType == E2V_EV76C454_Jade_Monocolor) && (GET_CONFIG_SETTING(bColorFusion))){		
			if(ImageFormat == HTAG_IMAGE::HTAG_IMAGE_UNCOMPRESSED_COLOR){
				ReturnStatus = false;
			}
			else if(ImageFormat == HTAG_IMAGE::HTAG_IMAGE_UNCOMPRESSED_RAW)
				ReturnStatus = true;
			else if(ImageFormat >= HTAG_IMAGE::HTAG_IMAGE_TIF_COLOR_COMPRESSED && 
				ImageFormat <= HTAG_IMAGE::HTAG_IMAGE_BMP_COLOR)
			{
				NewImage->FusionImage();
				ColorImage = 1;
			}
			else
			{
				NewImage->FissionImage();
			}
		} else if((ImagerType == E2V_EV76C454_Jade_Monocolor) && (!GET_CONFIG_SETTING(bColorFusion))){
			if((ImageFormat >= HTAG_IMAGE::HTAG_IMAGE_TIF_COLOR_COMPRESSED && 
				ImageFormat <= HTAG_IMAGE::HTAG_IMAGE_BMP_COLOR))
			{
				ReturnStatus = false;
			}else{
				NewImage->FissionImage();		// we need do Fission for color sensor.
			}
		} else if(ImagerType == E2V_EV76C454_Jade){
			if((ImageFormat >= HTAG_IMAGE::HTAG_IMAGE_TIF_COLOR_COMPRESSED && 
				ImageFormat <= HTAG_IMAGE::HTAG_IMAGE_BMP_COLOR))
			{
				ReturnStatus = false;
			}else{
				NewImage->FissionImage();		
			}
		} else {			// for Color sensor (5MPX)
			ColorImage = 1;
		}
		


		if(ColorImage == 1 && ReturnStatus)
		{
			if(NewImage->PixelIncrement > 1)			// Modifier "S", Pixel ship
				NewImage->SubsampleImage(24);

			if(NewImage->RotationApplied)			// Modifier "IR", Rotation
				NewImage->ImageRotateRGB(NewImage->RotationApplied);

		}


		// If the user does not apply a color ship with monochrome sensor, do image processing
		// Note: We now don't support color image processing. The following statement may need
		// change

		if(!ColorImage && ReturnStatus && (ImageFormat != HTAG_IMAGE::HTAG_IMAGE_UNCOMPRESSED_RAW))	
		{
			if(NewImage->InfinityFilterApplied)			// Modifier "A", Inifinity Filter
			{
				NewImage->InfinityFilterImage();
			}
			
			if( NewImage->ImageFlatteningApplied)		// Modifier "C", Compensation
			{
				NewImage->FlattenImage();
			}

			if(NewImage->EdgeEnhancementApplied)	// Modifier "E", Edge sharpen
			{
				NewImage->EdgeSharpenImage(NewImage->EdgeEnhancementApplied);
			}

			if(NewImage->HistogramStretchApplied)	// Modifier "H", Hisrogram stretch
			{
				NewImage->StretchImage();
			}

			if(NewImage->FlySpeckApplied)			// Modifier "IF", Noise reduction
			{
				NewImage->FlySpeckImage();
			}

			if(NewImage->RotationApplied)			// Modifier "IR", Rotation
			{
				NewImage->ImageRotate(NewImage->RotationApplied);
			}

			if(NewImage->GammaCorrectionApplied)		// Modifier "K", Gamma correction
			{
				NewImage->GammaImage(NewImage->GammaCorrectionApplied);
			}
			
			if(NewImage->PixelIncrement > 1)			// Modifier "S", Pixel ship
			{
				NewImage->SubsampleImage(8);
			}
			
			if(NewImage->UltraTextApplied)			// Modifier "U", Document image filter
			{
				NewImage->UltraTextImage();
			}

			if(NewImage->SmoothingApplied)			// Modifier "V", Blur image
			{
				NewImage->Smoothing();
			}

			if(NewImage->HistogramApplied)			// Modifier "W", Histogram ship
			{
				NewImage->HistogramImage();
			}

			if(NewImage->AdaptiveUltraTextApplied)	// Modifier "IU", Adaptive ultra text
			{
				NewImage->AdaptiveUltraTextImage();
			}
		}

		if(ReturnStatus)
			NewImage->FormatImage(ImageFormat);


	}
	else
	{
		// Couldn't modify image either due to bad image or bad parameters; WOOF!
		ReturnStatus = false;
	}

	// If we didn't encounter any errors, send the image.
	if(ReturnStatus)
	{
#ifdef RFSCAN
#else			//else #ifdef RFSCAN not true, so not a RF Scanner

		bool		UseHModemProtocol;
		HSTRING	ImageData(20, true);

		UseHModemProtocol = ((NewImage->TransferProtocol == HTAG::HTAG_TRANSMIT_HMODEM_COMPRESSED_PROTOCOL) ||
		                     (NewImage->TransferProtocol == HTAG::HTAG_TRANSMIT_HMODEM_UNCOMPRESSED_PROTOCOL)) ? true : false;
		NewImage->CreateTaggedImage(&Command, &ImageData, true);

		//sprintf (MsgBuf," CreatedTaggedImage DataSize:%d CommandSize:%d \r\n", ImageData.Size, Command.Size ); //lmc   debug
		//xputstring (MsgBuf);                                        //lmc   debug

		// Delete NewImage to conserve memory, since we don't need it anymore.
		delete NewImage;
		NewImage = 0;

		mytime2 = current_time();

		// Transmit according to protocol setting.
//		RESET_WATCHDOG_TIMER();

		if(UseHModemProtocol)
		{
			UINT HModemStatus = hm->Transmit(&ImageData, &Command, false, true, NO_COMPRESSION /*HUFFMAN_VERSION_1_0*/ );

			if( GET_SETTING(GeneralDebugOn) )
			{
				MT_Delay( SLEEP_AFTER_TERMINATE * 2 );

				switch(HModemStatus)                                   //handle errors 0, 4-8, 10-11, 21, 23, 25
				{
				case HMODEM::TERMINATE_RECEIVED:                                                         //10
					xputstring("HModem Download - Reader terminated download\015\012");
					break;

				case HMODEM::RECEIVER_NOT_RESPONSIVE_TO_FINISH:                                                   //5
					xputstring("HModem Download - Reader not responding after download completion\015\012");
					break;

				case HMODEM::RECEIVER_NAKED:                                                                        //6
					xputstring("HModem Download - Reader NAKed download operation; terminating...\015\012");
					break;

				case HMODEM::RECEIVER_IS_UNRESPONSIVE:
					xputstring("HModem Download - Timeout while waiting for reader response\015\012");                  //7
					break;

				case HMODEM::RECEIVER_NOT_READY:                                                                      //8
					xputstring("HModem Download - Reader not ready to receive data\015\012");
					break;

				case HMODEM::FINISH_SUCCESSFUL:                                                                //4
					break;

				case HMODEM::BAD_INPUT_DATA_POINTER:                                                         //0
					xputstring("Bad input data pointer \015\012");
					break;

				case HMODEM::UNRECOGNIZED_COMPRESSION_MODE:                                                     //21
					xputstring("Not Reconized compression mode \015\012");
					break;

				case HMODEM::TERMINATE_BY_US:                                                                   //11
					xputstring("Terminated by us \015\012");
					break;

				case HMODEM::HEADER_NOT_ACKNOWLEDGED:                                                          //23
					xputstring("Header not acked \015\012");
					break;

				case HMODEM::NAK_RECEIVED:
					xputstring("Nak received from Wait_For_Ack  \015\012");                                     //25
					break;

				default:
					xputstring("Error while transmitting HMODEM image\015\012");
					break;
				}
			}
		}

		// Send the image with no protocol
		else
		{
			unsigned char * ptr = ImageData.Char;
			unsigned int tSize = ImageData.Size/3;
			unsigned int zSize = ImageData.Size%3;
			HSTRING ptr_1(ptr, tSize);
			HSTRING ptr_2(ptr+tSize, tSize);
			HSTRING ptr_3(ptr+tSize*2, tSize);
			HSTRING ptr_4(ptr+tSize*3, zSize);
			ComDevice->Write( &Command, 0, Command.Size, false );
			ComDevice->Write( &ptr_1, 0, ptr_1.Size, false );
		 	ComDevice->Write( &ptr_2, 0, ptr_2.Size, false );
			ComDevice->Write( &ptr_3, 0, ptr_3.Size, false );
			ComDevice->Write( &ptr_4, 0, ptr_4.Size, false );
		}
		g_pRemRouter->RouteImageLob(ImageFormat, &Command, &ImageData);  // RouteImageLob is a null method for RFSCAN & if REMENA is 0

#endif			//end  #ifdef RFSCAN

		mytime3 = current_time();

		if (GET_SETTING(DebugImageTimeReporting))
		{
#ifndef RFSCAN
			theStdInterface.printf("Prepare Image for send time = %dms\r\n", (mytime2-mytime1)/1000);

			theStdInterface.printf("Send time = %dms (%dus)\r\n", (mytime3-mytime2)/1000, (mytime3-mytime2));
#else
#endif
		}
	}

	delete hm;

	// If we haven't already deleted NewImage (pointer is non-zero), do it now.
	if( NewImage != 0 )
	{
		delete NewImage;
	}

#ifdef DEVELOPMENT_SETTINGS
	EndHeapSpace = ComputeFreeHeapSpace();
	if (StartHeapSpace != EndHeapSpace)
		printf("Possible Memory Leak in SendImage() - Start heap=%lx End Heap=%lx\r\n",StartHeapSpace, EndHeapSpace);
#endif

	return ReturnStatus;
} // SendImage (overloaded version #1)




//
//	SendImage (overloaded version #2)
//		Send the last captured image.  Get a pointer the last captured image and call the version of this routine that actually sends the
//		data.
//
bool SendImage(HIMAGE *ImageParameters, bool UseInversionSetting, bool OverrideShipImagesIn640x480VGAMode )
{
	bool	ReturnStatus = true;		// Assume success until we know otherwise.

	// Get a pointer to the last captured image, then process and send it.
	if(P_SCANBUFFER != NULL)
	{
		// Get image height and width from scan driver.
		DWORD dwHeight, dwWidth;
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&dwHeight,4,NULL);
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&dwWidth,4,NULL);
		ReturnStatus = SendImage(ImageParameters, P_SCANBUFFER, (int) dwWidth, (int) dwHeight, UseInversionSetting, OverrideShipImagesIn640x480VGAMode );
	}
	else
	{
		ReturnStatus = false;
	}

	return ReturnStatus;
} // SendImage (overloaded version #2)




//*************************************************************************************************************************
//Class:  BINARIZE_HIMAGE
//*************************************************************************************************************************





//constructor
BINARIZE_HIMAGE::BINARIZE_HIMAGE(UCHAR *ImageData, int InputXDimension, int InputYDimension) : HIMAGE(ImageData, InputXDimension, InputYDimension)
{
	return;
}


//destructor
BINARIZE_HIMAGE::~BINARIZE_HIMAGE(void)
{
	return;
}



bool BINARIZE_HIMAGE::Binarize(BITSTRING *InputImage, BITSTRING * OutputImage, int XDimension, int YDimension,
                               bool UseDecoderThresholds)
{
	ULONG	*pBinImg;

	pBinImg = new ULONG[((XDimension + 32) * YDimension) / 32];
	SINT	Status = BinaryImage(InputImage->Char, pBinImg, XDimension, YDimension, (BOOL) UseDecoderThresholds);

	if(Status)
	{
		int		X, Y, i;
		ULONG	BitMask = 0x80000000;
		int		NumberOfBits = 32;
		int		Byte = 0;

		if(OutputImage->MaximumLength() < (XDimension * YDimension))
		{
			Status = OutputImage->Reallocate(XDimension * YDimension);
		}

		if(Status)
		{
			OutputImage->Size = XDimension * YDimension;
			i = 0;

			for(Y = 0;    Y < YDimension;    ++Y)
			{
				for(X = 0;    X < XDimension;    ++X)
				{
					OutputImage->Char[i] = ((UCHAR) ((pBinImg[Byte] & BitMask) ? 255 : 0));
					++i;
					--NumberOfBits;

					if(NumberOfBits)
					{
						BitMask = BitMask >> 1;
					}
					else
					{
						++Byte;
						NumberOfBits = 32;
						BitMask = 0x80000000;
					}
				}

				if(NumberOfBits != 32)
				{
					++Byte;
					NumberOfBits = 32;
					BitMask = 0x80000000;
				}
			}
		}
	}

	delete [] pBinImg;

	return((Status) ? true : false);
}




/*-----------------------------------------------------------------------
 * BinaryImage - This function takes the already allocated "di" memory
 *   and fills it with the RASTERIZED binary image.  NOTE: When I say
 *   rasterized I mean that it is in order.  It is NOT stored in Andy's
 *   traditional style of storing binary images.  The reason is because
 *   this function is only used with image capture applications.
 *-----------------------------------------------------------------------*/
#define N_DROOP		8
#define N_MIN_MAX_GAP	12
#define DO_MIN_MAX()                                                     																\
								xmax-=N_DROOP;																												\
								if ((xmin+N_DROOP+N_MIN_MAX_GAP)>=xmax) xmin = xmax - N_MIN_MAX_GAP; else xmin += N_DROOP;		\
								if (xmax > *pMax)*pMax = xmax; else xmax = *pMax;																\
								if (xmin < *pMin)*pMin = xmin; else xmin = *pMin
#define DO_MIN_MAX_AVERAGE()	*pMax = ((lowgt*(*pMin) + hiwgt*(*pMax)) >> 3)
#define	THMASK	0x1fff



SINT BINARIZE_HIMAGE::BinaryImage(BYTE *pLocalImg,ULONG *pBinImg,SINT nTopx,SINT nTopy, BOOL /* bUseCurrentThresholds */ )
{
	SINT *pThresholds,*pMax,*pMin,*pThrBegin,*pThrBegin2,nVertBlock=0,ok=FALSE,nThrTopx,nThrTopy;
	SINT c,r,x,y,nStep=8,xmin,xmax,lowgt,hiwgt;
	BYTE	*pTempImg;
	ULONG nBitMask = 0x80000000;
	SINT	WhtMax = 256;

	/* Under cases where we are simply binarizing the grayscale image in memory, we don't need to calculate
	 * new thesholds for it because when we attempted a decode we did it anyway.  */
	//	if (!bUseCurrentThresholds) {

	nThrTopx = (nTopx+7)>>3;
	nThrTopy = (nTopy+7)>>3;
	pThrBegin = (SINT *)MALLOC(sizeof(SINT)*nThrTopx*nThrTopy);

	if (pThrBegin == NULL)
		return FALSE;

	pThrBegin2 = (SINT *)MALLOC(sizeof(SINT)*nThrTopx*nThrTopy);

	if (pThrBegin2 == NULL)
	{
		FREE(pThrBegin);
		return FALSE;
	}
	pMax = pThrBegin;
	pMin = pThrBegin2;

	for (y=0,r=4;y<nThrTopy;y++)
	{
		pTempImg = pLocalImg + (nTopx*r) + 4;

		for (x=0,c=4;x<nThrTopx;x++)
		{
			*pMax++ = *pMin++ = ((((SINT)*pTempImg)>WhtMax)?(WhtMax):((SINT)*pTempImg));
			c+=8;
			if (c<nTopx)
				pTempImg+=8;
		}

		r+=8;

		if( !(r<nTopy) )
			r-=8;
	}

	pMax = pThrBegin;
	pMin = pThrBegin2;
	lowgt = GET_SETTING(PrintWeight);
	hiwgt = 8 - lowgt;

	for (y=0;y<nThrTopy;y++)
	{
		pMax = pThrBegin + (y*nThrTopx);
		pMin = pThrBegin2 + (y*nThrTopx);
		xmax = *pMax;
		xmin = *pMin;

		for( x = 1; x<nThrTopx; x++ )
		{
			pMax++;
			pMin++;
			DO_MIN_MAX();
		}

		for( x = nThrTopx-1; x>0; x-- )
		{
			pMax--;
			pMin--;
			DO_MIN_MAX();
		}
	}

	for (x=0;x<nThrTopx;x++)
	{
		pMax = pThrBegin + x;
		pMin = pThrBegin2 + x;
		xmax = *pMax;
		xmin = *pMin;

		for (y = 1; y<nThrTopy; y++)
		{
			pMax+=nThrTopx;
			pMin+=nThrTopx;
			DO_MIN_MAX();
		}

		DO_MIN_MAX_AVERAGE();

		for (y = nThrTopy-1; y>0; y--)
		{
			pMax-=nThrTopx;
			pMin-=nThrTopx;
			DO_MIN_MAX();
			DO_MIN_MAX_AVERAGE();
		}
	}

	FREE(pThrBegin2);


	//	} else {
	//		pThrBegin = Thr;
	//		nThrTopx = THRTOPX; nThrTopy = THRTOPY;
	//	}


	/********* Generate the BINARY image based on the calculated Thresholds *******/
	/* for every row of the image */
	for (y=0;y<nTopy;)
	{
//		RESET_WATCHDOG_TIMER();

		/* set up pThresholds to point at the right location for the row of
		* pixels we will be thresholding */
		pThresholds = pThrBegin + (nThrTopx * nVertBlock);

		/* set up pImg to point to the correct row of the grayscale image */
		pTempImg = pLocalImg + (y*nTopx);

		/* for every pixel in the row */
		for (x=0;x<nTopx;)
		{
			/* if the pixel is greater than the threshold, it's white so set the bit
			* that will represent this pixel*/
			if ((SINT)(*pTempImg++) > (*pThresholds&THMASK))
			{
				*pBinImg |= nBitMask;
			}
			else /* it's black so clear the bit that will represent this pixel */
			{
				*pBinImg &= ~nBitMask;
			}

			/* increment x because we're done with this pixel */
			x++;

			/* each value stored in pThresholds is good for an nStep squared pixel region.  Make
			* sure that if we have just crossed the boundry into the next threshold reason
			* that we increment pThresholds to point to the next horizontal threshold */
			if (!(x % nStep))
			{
				pThresholds++;
			}

			/* shift the bit mask to the place where the next pixel's bit will be stored */
			nBitMask >>= 1;

			/* if we have gone through all of the bits in the long word, increment the long word pointer
			* that we are using to store the binary image, and reset the bit mask */
			if (!nBitMask)
			{
				nBitMask = 0x80000000;
				pBinImg++;
			}
		}
		/* the same thesholds will be used on "nStep" consecutive rows because a value in pThresholds is
		* good for an "nStep" squared region.  We increment y as we go through the rows.  However, if we have
		* done all "nStep" rows, we need to increment nVertBlock that will tell us to use the next
		* vertical "row" of thresholds */
		y++;

		if (!(y % nStep))
		{
			nVertBlock++;
		}

		/* each new row starts on a long word boundry, however if we just reset the bit mask and incremented
		* the binary image pointer, there is no need to do this twice.*/
		if (nBitMask != 0x80000000)
		{
			nBitMask = 0x80000000;
			pBinImg++;
		}
	}

	/*	if (!bUseCurrentThresholds)*/ FREE(pThrBegin);
	ok=TRUE;
	return (ok);
}




//======================================================================================
