/*======================================================================
 *	"SigCapture.c" -- Signature Capture (IQ Imaaging)
 *======================================================================
 *
 * $RCSfile: Gen6FoundationApp/FoundationApp/Imaging/sigcapture.c $
 * $Revision: 1.7 $
 * $Date: 2010/03/08 14:52:00EST $
 *
 *======================================================================*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "platform.h"
#include "language.h"
#include "db_chrio.h"
//#include "trigger.h"
#include "ReaderConfiguration.h"
#include "menu.h"
#include "DecodeIFace.h"
#include "himage.h"
#include "Image_Send_Binarize_Himage.h"
#include "sigcapture.h"

static signed int dcornerPDF_lin_x;
static signed int dcornerPDF_lin_y;
static int dcorner;
static point ctr;
static int hht, hhb, hwl, hwr;
static int hMultiplier = 1;
static int npix = 1;
static int dstwidth = 200,dstheight = 100;

int ParseImageBoxHeader(HSTRING *ImageBoxHeader, HIMAGE *ImageParameters, int *MessageCommandLength) {
	int		ReturnStatus = TRUE;
	int		NextLocationToRead = 0;
	int		Number;
	int		NumberOfCharactersUsedInConversion;
	BOOL		bNegative = FALSE;

	*MessageCommandLength = -1;
	if(((ImageBoxHeader->Size + NextLocationToRead) >= 7) && (ImageBoxHeader->Char[NextLocationToRead] == 0x02) &&
			(ImageBoxHeader->Char[NextLocationToRead + 1] == 0xFE) && (ImageBoxHeader->Char[NextLocationToRead + 6] == 0x0D)) {
		ImageBoxHeader->CopyFromString(NextLocationToRead + 2, 4, ((UINT *) MessageCommandLength));
		NextLocationToRead += 7;
	}
	// Is there IMGSNP text to remove?
	*MessageCommandLength = -1;
	if(((ImageBoxHeader->Size + NextLocationToRead) >= 6) &&
						(ImageBoxHeader->CompareOnShorter(NextLocationToRead, ((UCHAR *) "04080A"), 6, false) == 0)) {
		NextLocationToRead += 6;
	}
	// Parse through the header.
	while((NextLocationToRead < ((int) ImageBoxHeader->Size)) && (ReturnStatus == TRUE)) {
		Number = ((int) ImageBoxHeader->ConvertToNumber(NextLocationToRead, &NumberOfCharactersUsedInConversion));
		if(NumberOfCharactersUsedInConversion < 1) {
			ReturnStatus = FALSE; // Parameter must have at least one digit; WOOF.
		} else {
			// Parameter is OK.  Get the image format character.
			NextLocationToRead += NumberOfCharactersUsedInConversion;
			if(NextLocationToRead < ((int) ImageBoxHeader->Size)) {
				switch(ImageBoxHeader->Char[NextLocationToRead])
				{
					case 'x': case 'X':	if (bNegative) Number = -Number; ctr.x = Number;	Number = 0; bNegative = FALSE;	break;
					case 'y': case 'Y':	if (bNegative) Number = -Number; ctr.y = Number;	Number = 0; bNegative = FALSE;	break;
					case 'a': case 'A':	dstwidth = Number;	Number = 0; bNegative = FALSE;	break;
					case 'b': case 'B':	dstheight = Number;	Number = 0; bNegative = FALSE;	break;
					case 'h': case 'H':	hht = Number>>1; hhb = Number - (Number>>1); Number = 0;	bNegative = FALSE;break;
					case 'w': case 'W':	hwl = Number>>1; hwr = Number - (Number>>1); Number = 0;	bNegative = FALSE;break;
					case 'f': case 'F':	ImageParameters->Format = Number;	Number = 0; bNegative = FALSE; break;
					case 'r': case 'R':	npix = Number;	Number = 0;	bNegative = FALSE; break;
					case 'd': case 'D':  ImageParameters->Depth = Number; Number = 0;	bNegative = FALSE; break;
					case 'p': case 'P':  ImageParameters->TransferProtocol = Number; Number = 0; bNegative = FALSE; break;
					case 'j': case 'J':  ImageParameters->JPEGQualityFactor = Number; Number = 0; bNegative=FALSE; break;
					case 'k': case 'K':  ImageParameters->GammaCorrectionApplied = Number; Number = 0; bNegative=FALSE; break;
					case '~':			   ImageParameters->RowNoiseApplied = Number; Number = 0; bNegative=FALSE; break;
					case 's': case 'S':	hMultiplier = Number; if (hMultiplier <= 0) hMultiplier = 1; bNegative=FALSE; Number=0; break;
					case '-':				bNegative = TRUE; break;
					default :   if((ImageBoxHeader->Char[NextLocationToRead] >= '0') && (ImageBoxHeader->Char[NextLocationToRead] <= '9'))
										Number = Number*10 + (ImageBoxHeader->Char[NextLocationToRead] - 0x30);
									else Number = 0;
									break;
				}
				++NextLocationToRead;
			} else {
				ReturnStatus = FALSE;
			}
		}
	}
	return(ReturnStatus);
} // ParseImageBoxHeader

point pnt(point a, point b, int i)
{
	point p;
	p.x = (a.x+b.x)/2 + ((b.x-a.x) * i) / dcorner;
	p.y = (a.y+b.y)/2 + ((b.y-a.y) * i) / dcorner;
	return (p);
}
point pntPDFlin_x(point a, point b, int i) {
	point p;
	p.x = (a.x+b.x)/2 + ((b.x-a.x) * i) / dcornerPDF_lin_x;
	p.y = (a.y+b.y)/2 + ((b.y-a.y) * i) / dcornerPDF_lin_x;
	return (p);
}
point pntPDFlin_y(point a, point b, int i) {
	point p;
	p.x = (a.x+b.x)/2 + ((b.x-a.x) * i) / dcornerPDF_lin_y;
	p.y = (a.y+b.y)/2 + ((b.y-a.y) * i) / dcornerPDF_lin_y;
	return (p);
}


BOOL DumpImgBox(void) {

	BOOL ReturnStatus = NO_ERROR;
	tRect SymbolCorners;
	tRect SubRect;
	int x, y, ix, iy;
	LONG x0, x1, y0, y1, xp, yp;
	UCHAR PC_HUGE *subimg, *Subptr;
//	int hMultiplier = 1;
	point 	a, b;
//	char LastCodeid;
//	SINT nPDFrows;
	point 	ulc, lrc;
	int ParseStatus = TRUE;
	int 		nrows, ncols;
	HIMAGE	ImageParameters;
//	int		Width, Height;
	if (pgLastDecodeInfo == NULL)	// bail if I have no last decoded image
	{
//			xputstring ("No Last Img, b'bye\r\n");
			return false;
	}

  	//pTrigger->FinishScanning();
  	//pTrigger->UpdateTriggerStatus();

// Don't like how this is done , but NEED to be consistent here, need to do for all things that may or may not be parsed out of the command
		ctr.x = ctr.y = hht = hhb = hwl = hwr = 0;
		npix=1;
		hMultiplier = 1;
	ImageParameters.SetParameterDefaults();
	ImageParameters.ReportAllParameters( GET_SETTING( ReportAllHTAGParameters ) );
	ImageParameters.JPEGQualityFactor = GET_SETTING(JPEGQualityFactor);
	ImageParameters.JPEGCompressionSmoothingFactor = GET_SETTING(JPEGCompressionSmoothingFactor);
	ImageParameters.JPEGDecompressionBlockSmoothing = GET_SETTING(JPEGDecompressionBlockSmoothing);
	ImageParameters.FastJPEG = GET_SETTING(TransmitImageUseFastAlgorithmForJPEG);
	ImageParameters.TransferProtocol = GET_SETTING(TransmitImageProtocol);
	ImageParameters.PixelIncrement = GET_SETTING(TransmitImageSubsampleIncrement);
	ImageParameters.LeftBoundary = GET_SETTING(TransmitImageWindowLeft);
	ImageParameters.RightBoundary = GET_SETTING(TransmitImageWindowRight);
	ImageParameters.TopBoundary = GET_SETTING(TransmitImageWindowTop);
	ImageParameters.BottomBoundary = GET_SETTING(TransmitImageWindowBottom);
	ImageParameters.Margin = GET_SETTING(TransmitImageMargin);
	ImageParameters.XAxisInversionApplied = GET_SETTING(InvertImageAboutXAxis);
	ImageParameters.YAxisInversionApplied = GET_SETTING(InvertImageAboutYAxis);
	ImageParameters.Format = GET_SETTING(TransmitImageFormat);
	ImageParameters.GammaCorrectionApplied = 60;
	ImageParameters.RowNoiseApplied = GET_SETTING(RowNoise);

// There are config commands controlling aspect ratio, which I believe can / should want to be over-written by the 's' parameter in the parser
// so let's load it here "before" the parser --> original version always used the config because this was done after the parse
	if ((pgLastDecodeInfo->codeid == 'a')||(pgLastDecodeInfo->codeid == 'b')||(pgLastDecodeInfo->codeid == 'j')||(pgLastDecodeInfo->codeid == 'I')||(pgLastDecodeInfo->codeid == 'e')) hMultiplier = GET_SETTING(Lin_HeightToNarrowElementWidth);
	else if (pgLastDecodeInfo->codeid == 'r') hMultiplier = GET_SETTING(PDF_SigCapRowHeightToWidth);

	if(ReaderConfiguration->ExtraneousTextAfterCommand->Size > 0) {
		HSTRING				ImageBoxHeader(ReaderConfiguration->ExtraneousTextAfterCommand);
		int					MessageCommandLength;
		ParseStatus = ParseImageBoxHeader(&ImageBoxHeader, &ImageParameters, &MessageCommandLength);
	}
	if(ParseStatus) {
      ReturnStatus = GetBestSymbolCorners(&SymbolCorners);   // get best symbol corners for last image decoded


		if (ReturnStatus == NO_ERROR) {
//			if ((LastCodeid == 'a')||(LastCodeid == 'b')||(LastCodeid == 'j')||(LastCodeid == 'e')) hMultiplier = GET_SETTING(Lin_HeightToNarrowElementWidth);
//			else if (LastCodeid == 'r') hMultiplier = GET_SETTING(PDF_SigCapRowHeightToWidth);

			ulc.x = ctr.x-hwl; ulc.y = ctr.y - hht;	lrc.x = ctr.x + hwr; lrc.y = ctr.y + hhb;

			dcorner = pgLastDecodeInfo->dcorner; // setup local track variable for dcorner
         if ((pgLastDecodeInfo->codeid == 'r') || (pgLastDecodeInfo->codeid == 'a') || (pgLastDecodeInfo->codeid == 'b') || (pgLastDecodeInfo->codeid == 'j') || (pgLastDecodeInfo->codeid == 'I') || (pgLastDecodeInfo->codeid == 'e')) {
				if ((pgLastDecodeInfo->codeid == 'a') || (pgLastDecodeInfo->codeid == 'b')||(pgLastDecodeInfo->codeid == 'j')||(pgLastDecodeInfo->codeid == 'I')||(pgLastDecodeInfo->codeid == 'e')) {
					dcornerPDF_lin_y = hMultiplier;
					dcornerPDF_lin_x = dcorner;
				} else {
					dcornerPDF_lin_x = dcorner;
					dcornerPDF_lin_y = (hMultiplier*pgLastDecodeInfo->pdfrows);
				}

				a = pntPDFlin_y(SymbolCorners.ul,SymbolCorners.ll,ulc.y);
				b = pntPDFlin_y(SymbolCorners.ur,SymbolCorners.lr,ulc.y);
				SubRect.ul = pntPDFlin_x(a,b,ulc.x);
				SubRect.ur = pntPDFlin_x(a,b,lrc.x);
				a = pntPDFlin_y(SymbolCorners.ul,SymbolCorners.ll,lrc.y);
				b = pntPDFlin_y(SymbolCorners.ur,SymbolCorners.lr,lrc.y);
				SubRect.lr = pntPDFlin_x(a,b,lrc.x);
				SubRect.ll = pntPDFlin_x(a,b,ulc.x);
			} else {
				a = pnt(SymbolCorners.ul,SymbolCorners.ll,ulc.y);
				b = pnt(SymbolCorners.ur,SymbolCorners.lr,ulc.y);
				SubRect.ul = pnt(a,b,ulc.x);
				SubRect.ur = pnt(a,b,lrc.x);
				a = pnt(SymbolCorners.ul,SymbolCorners.ll,lrc.y);
				b = pnt(SymbolCorners.ur,SymbolCorners.lr,lrc.y);
				SubRect.lr = pnt(a,b,lrc.x);
				SubRect.ll = pnt(a,b,ulc.x);
			}
			// Test to make sure the entire SubRect is within the image boundaries.
			if((SubRect.lr.x > pgLastDecodeInfo->cols) || (SubRect.lr.x < 0) ||
				(SubRect.ll.x > pgLastDecodeInfo->cols) || (SubRect.ll.x < 0) ||
				(SubRect.ul.x > pgLastDecodeInfo->cols) || (SubRect.ul.x < 0) ||
				(SubRect.ur.x > pgLastDecodeInfo->cols) || (SubRect.ur.x < 0) ||
				(SubRect.lr.y > pgLastDecodeInfo->rows) || (SubRect.lr.y < 0) ||
				(SubRect.ll.y > pgLastDecodeInfo->rows) || (SubRect.ll.y < 0) ||
				(SubRect.ul.y > pgLastDecodeInfo->rows) || (SubRect.ul.y < 0) ||
				(SubRect.ur.y > pgLastDecodeInfo->rows) || (SubRect.ur.y < 0) ) {
				ReturnStatus = IMAGE_OVERFLOW;
			}

			if(ReturnStatus == NO_ERROR)
			{
				if (npix) {
 					nrows = (lrc.y - ulc.y) * npix;
					ncols = (lrc.x - ulc.x) * npix;
					if (npix >= 1000) {nrows/=1000; ncols/=1000;}
				} else {
					nrows = dstheight;
					ncols = dstwidth;
				}
				Subptr = subimg = (UCHAR PC_HUGE *) MALLOC((LONG)sizeof(UCHAR) * nrows * ncols);
				if (subimg) {
					for (y=0; y<nrows; y++) {
						RESET_WATCHDOG_TIMER();
//						WOOF;
						x0 = ((LONG)y*((LONG) SubRect.ll.x - (LONG) SubRect.ul.x)<<8)/nrows + ((LONG) SubRect.ul.x<<8);
						y0 = ((LONG)y*((LONG) SubRect.ll.y - (LONG) SubRect.ul.y)<<8)/nrows + ((LONG) SubRect.ul.y<<8);
						x1 = ((LONG)y*((LONG) SubRect.lr.x - (LONG) SubRect.ur.x)<<8)/nrows + ((LONG) SubRect.ur.x<<8);
						y1 = ((LONG)y*((LONG) SubRect.lr.y - (LONG) SubRect.ur.y)<<8)/nrows + ((LONG) SubRect.ur.y<<8);
						for (x=0; x<ncols; x++) {
							xp = ((LONG)x*(x1 - x0)/ncols) + x0; ix = xp >>8;
							yp = ((LONG)x*(y1 - y0)/ncols) + y0; iy = yp >>8;
							if ((ix>=0)&&(ix<pgLastDecodeInfo->cols-1)&&(iy>=0)&&(iy<pgLastDecodeInfo->rows-1))
							{
								*(Subptr++) = subpixel(pgLastDecodeInfo,xp,yp);
							}
							else *(Subptr++) = 64;
						}
					}

					ImageParameters.XDimension = ncols;
					ImageParameters.YDimension = nrows;
					ImageParameters.PixelIncrement = 1;
					ImageParameters.LeftBoundary = 0;
					ImageParameters.RightBoundary = ncols-1;
					ImageParameters.TopBoundary = 0;
					ImageParameters.BottomBoundary = nrows-1;
					ImageParameters.Margin = 0;
					ImageParameters.XAxisInversionApplied = false;
					ImageParameters.YAxisInversionApplied = false;
					
					/*error check for large resultions. Line below is...since we have enough memory in our image processing
					 * to handle the native image size, then we are restricting the sig cap image size after resultion zoom
					 * to be the native image size. Could have done a malloc and then a free for downstream stuff but I did not like
					 * that option. */
					if (ncols*nrows > (GET_SETTING(ImgRowNumberOfPixels)*GET_SETTING(ImgColNumberOfPixels)))
					{	
						ReturnStatus = UNSPECIFIED_ERROR;
					}
					else if(!SendImage(&ImageParameters, subimg, ncols, nrows, false, true ))
					{
						ReturnStatus = UNSPECIFIED_ERROR;
					}
				}
				else
					ReturnStatus = UNSPECIFIED_ERROR; //  needs more cleanup!! but this solves the bug where imgbox[Ack] ... no image
				FREE (subimg);
			}
		}

		return ((ReturnStatus == NO_ERROR) ? true:false);
	}
	else
	{
		return FALSE;
	}
}



