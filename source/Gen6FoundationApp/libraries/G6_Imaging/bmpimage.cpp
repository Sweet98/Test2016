//======================================================================================
// BmpImage.cpp
//======================================================================================
// $RCSfile: bmpimage.cpp $
// $Revision: 1.5 $
// $Date: 2011/11/23 01:28:16EST $
//======================================================================================

//======================================================================================








//---------------------------------------------------------------------------
//
//  Module Name:     BmpImage
//
//---------------------------------------------------------------------------


#include	<string.h>
#include	<stdlib.h>
#include	"BmpImage.h"		// BmpImage interface definition
#include	"HHPImagingConfig.h"


//---------------------------------------------------------------------------






//
//	CreateBMPImage
//		Create an 8 or 1 bit per pixel BMP image from the input image.
//
bool CreateBMPImage(BITSTRING *BmpImage, BITSTRING *InputImage, int XDimension, int YDimension, int NumberOfBits,
					int Threshold)
{
	int		i;
	bool	Status = true;			// Assume success until we know otherwise.




//	RESET_WATCHDOG_TIMER();

// Only create if X and Y dimensions are > 0 and their product equals the size of the input image.
	if((XDimension > 0) && (YDimension > 0) && ((XDimension * YDimension) == ((int) InputImage->Size)) &&
			((NumberOfBits == 8) || (NumberOfBits == 1)))
	{
	// Ensure that number of bytes per row is a multiple of 4.
		int		NumberOfBytesInRow;		// Always >= XDimension, since this must be a multiple of 4 for BMPs.
		int		NumberOfPadBytes;		// Number of additional bytes required to pad a row to a multiple of 4 bytes for BMPs.

		if(NumberOfBits == 1)
		{
			NumberOfBytesInRow = (XDimension / 8) + ((XDimension % 8) ? 1 : 0);
		}
		else
		{
			NumberOfBytesInRow = XDimension;
		}
		NumberOfPadBytes = 0;
		if(NumberOfBytesInRow % 4)
		{
			NumberOfPadBytes =  (4 - (NumberOfBytesInRow % 4));
			NumberOfBytesInRow += NumberOfPadBytes;
		}


	// First let's build the headers.
		HSTRING			FileHeader(BMP_BITMAPFILEHEADER_SIZE, true);
		HSTRING			InfoHeader(BMP_BITMAPINFOHEADER_SIZE, true);
		BMP_RGBQUAD				RGBPallette[256];
		int						SizeOfRGBPallette = sizeof(BMP_RGBQUAD) * (1 << NumberOfBits);

	// Copy BMP ID, total image, size, 2 reserved words (0), and offset to image data.
		FileHeader.Copy(BMP_ID);
		FileHeader.AppendNumber(BMP_BITMAPFILEHEADER_SIZE + BMP_BITMAPINFOHEADER_SIZE + SizeOfRGBPallette +
						(YDimension * NumberOfBytesInRow), 4);
		FileHeader.AppendNumber(BMP_RESERVED, 2);
		FileHeader.AppendNumber(BMP_RESERVED, 2);
		FileHeader.AppendNumber(BMP_BITMAPFILEHEADER_SIZE + BMP_BITMAPINFOHEADER_SIZE + SizeOfRGBPallette, 4);


		InfoHeader.AppendNumber(BMP_BITMAPINFOHEADER_SIZE, 4);		// Size of header
		InfoHeader.AppendNumber(XDimension, 4);						// Width
	    InfoHeader.AppendNumber(YDimension, 4);						// Height
		InfoHeader.AppendNumber(1, 2);								// # of planes
	    InfoHeader.AppendNumber(NumberOfBits, 2);					// # of bits per plane
		InfoHeader.AppendNumber(0, 4);								// No compression
	    InfoHeader.AppendNumber(0, 4);								// No total size in measurement units
		InfoHeader.AppendNumber(0, 4);								// No width in measurement units
	    InfoHeader.AppendNumber(0, 4);								// No height in measurement units
		InfoHeader.AppendNumber((1 << NumberOfBits), 4);			// # of colors used
	    InfoHeader.AppendNumber((1 << NumberOfBits), 4);			// # of colors of importance


		if(NumberOfBits == 1)
		{
			RGBPallette[0].rgbBlue = 0;
			RGBPallette[0].rgbGreen = 0;
			RGBPallette[0].rgbRed = 0;
			RGBPallette[0].rgbReserved = 0;
			RGBPallette[1].rgbBlue = 255;
			RGBPallette[1].rgbGreen = 255;
			RGBPallette[1].rgbRed = 255;
			RGBPallette[1].rgbReserved = 0;
		}
		else
		{
			for(i = 0;    i < 256;    ++i)
			{
				RGBPallette[i].rgbBlue = i;
				RGBPallette[i].rgbGreen = i;
				RGBPallette[i].rgbRed = i;
				RGBPallette[i].rgbReserved = 0;
			}
		}


	// Store the headers to the output image.
		BmpImage->Zero();
		BmpImage->Append(&FileHeader);
		BmpImage->Append(&InfoHeader);
		BmpImage->Append((char *) &RGBPallette, SizeOfRGBPallette);
	    BmpImage->BytePointer = BmpImage->Size;


	// Is it 8 bits per pixel?
		if(NumberOfBits == 8)
		{
			char *buf = (char*)malloc((XDimension + NumberOfPadBytes) * YDimension +64);
			char *ptr = buf;
			if(ptr==NULL)
				return false;
			// Store in reverse row order.
			for(i = 0;    i < YDimension;    ++i)
			{
				//BmpImage->Append((char *) &(InputImage->Char[(YDimension - i - 1) * XDimension]), XDimension);
				memcpy(ptr, (char *) &(InputImage->Char[(YDimension - i - 1) * XDimension]), XDimension);
				ptr += XDimension;
				if(NumberOfPadBytes)
				{
					//BmpImage->AppendFillChar(0, NumberOfPadBytes);
					memset(ptr, 0x00, NumberOfPadBytes);
					ptr += NumberOfPadBytes;
				}
			}
			BmpImage->Append(buf, (unsigned int)(ptr - buf));
			if(buf)
			free(buf);
		}

		else
		{
		// 1 bit per pixel
			int		X, Y;
			int		NextPixel;

			for(Y = 0;    Y < YDimension;    ++Y)
			{
				NextPixel = (YDimension - Y - 1) * XDimension;
				for(X = 0;    X < XDimension;    ++X)
				{
					if(InputImage->Char[NextPixel] < Threshold)
					{
						BmpImage->AppendBits(0, 1);
					}
					else
					{
						BmpImage->AppendBits(1, 1);
					}
					++NextPixel;
				}
				BmpImage->FillToNextByteBoundary();

				if(NumberOfPadBytes)
				{
					BmpImage->AppendBits(0, ((unsigned long) (8 * NumberOfPadBytes)));
				}
			}
		}
	}

	else
	{
	// Dimensions are bad; WOOF!
		Status = false;
	}

//	RESET_WATCHDOG_TIMER();

	return(Status);
} // CreateBMPImage


bool CreateBMPColorImage(BITSTRING *BmpColorImage, BITSTRING *InputImage, int XDimension, int YDimension, int NumberOfBits, int Threshold)
{
	int i;
	bool Status = true;       //Assume success
	
	// only create if X and Y dimensions are >0 and their product equals the size of the input image.
	if((XDimension > 0) && (YDimension>0) && ((XDimension * YDimension*3) == ((int) InputImage->Size)))
	{
		//Ensure that number of bytes per row is a multiple of 16.
		int NumberOfBytesInRow;		//Always >= XDimension, since this must be a multiple of 16 for BMPs COLOR.
		//int NumberOfPadBytes = 0;		//Number of additional bytes required to pad a row to a multiple of 16 bytes for BMPs.
		unsigned char *dst, *src;
		NumberOfBytesInRow = 3*XDimension;
		//NumberOfPadBytes = 0;
		//if((NumberOfBytesInRow ) % 16)
		//{
		//	NumberOfPadBytes = (16 - ((NumberOfBytesInRow/3) %16));
		//	NumberOfBytesInRow += NumberOfPadBytes * 3;
		//}
		src = InputImage ->Char;
		dst = src +2;
		unsigned char c;
		for(int y=0; y< XDimension * YDimension;y++)
		{
			c=*src; *src = *dst; *dst =c; src+=3; dst+=3;
		}
		// First let's build the headers.
		HSTRING FileHeader(BMP_BITMAPFILEHEADER_SIZE,true);
		HSTRING InfoHeader(BMP_BITMAPINFOHEADER_SIZE,true);

		
		// Copy BMP ID, total image, size, 2 reserved words (0), and offset to image data.
		FileHeader.Copy(BMP_ID);
		FileHeader.AppendNumber(BMP_BITMAPFILEHEADER_SIZE + BMP_BITMAPINFOHEADER_SIZE + YDimension * NumberOfBytesInRow, 4);
		FileHeader.AppendNumber(BMP_RESERVED,2);
		FileHeader.AppendNumber(BMP_RESERVED,2);
		FileHeader.AppendNumber(BMP_BITMAPFILEHEADER_SIZE + BMP_BITMAPINFOHEADER_SIZE, 4);
		InfoHeader.AppendNumber(BMP_BITMAPINFOHEADER_SIZE, 4);
		InfoHeader.AppendNumber(NumberOfBytesInRow/3,4);
		InfoHeader.AppendNumber(YDimension,4);
		InfoHeader.AppendNumber(1,2);
		InfoHeader.AppendNumber(24,2);
		InfoHeader.AppendNumber(0,4);
		InfoHeader.AppendNumber(0,4);
		InfoHeader.AppendNumber(0,4);
		InfoHeader.AppendNumber(0,4);
		InfoHeader.AppendNumber(0,4);
		InfoHeader.AppendNumber(0,4);
		BmpColorImage->Zero();
		BmpColorImage->Append(&FileHeader);
		BmpColorImage->Append(&InfoHeader);
		BmpColorImage->BytePointer = BmpColorImage->Size;
		for(i = 0; i< YDimension; ++i)
		{
			BmpColorImage->Append((char*)&InputImage->Char[(YDimension-i-1)*XDimension*3], XDimension*3);
			//if(NumberOfPadBytes)
			//{
			//	BmpColorImage->AppendFillChar(0,NumberOfPadBytes * 3);
			//}
		}
	}
	else
	{
		Status = false;
	}
	return Status;
}



//
//	DecodeBMPImage
//		Decode an 8 or 1 bit per pixel BMP image into a raw output image.
//
bool DecodeBMPImage(BITSTRING *BmpImage, BITSTRING *OutputImage, int *XDimension, int *YDimension, int *NumberOfBits)
{
	int		i;
	bool	Status = true;		// No errors yet.


//	RESET_WATCHDOG_TIMER();

// Only bitmap image is not empty.
	if(BmpImage->Size > (BMP_BITMAPFILEHEADER_SIZE + BMP_BITMAPINFOHEADER_SIZE))
	{
		BmpImage->ResetBitPointer();

	// First, let's decode the headers.
		BMP_BITMAPFILEHEADER	FileHeader;
		BMP_BITMAPINFOHEADER	InfoHeader;
		BMP_RGBQUAD				RGBPallette[256];

		BmpImage->BytePointer = 0;

		BmpImage->CopyNumberFromString(BmpImage->BytePointer, 2, (unsigned short *) &(FileHeader.bfType));
		BmpImage->BytePointer += 2;
		BmpImage->CopyNumberFromString(BmpImage->BytePointer, 4, (unsigned int *) &(FileHeader.bfSize));
		BmpImage->BytePointer += 4;
		BmpImage->CopyNumberFromString(BmpImage->BytePointer, 2, (unsigned short *) &(FileHeader.bfReserved1));
		BmpImage->BytePointer += 2;
		BmpImage->CopyNumberFromString(BmpImage->BytePointer, 2, (unsigned short *) &(FileHeader.bfReserved2));
		BmpImage->BytePointer += 2;
		BmpImage->CopyNumberFromString(BmpImage->BytePointer, 4, (unsigned int *) &(FileHeader.bfOffBits));
		BmpImage->BytePointer += 4;

		BmpImage->CopyNumberFromString(BmpImage->BytePointer, 4, (unsigned int *) &(InfoHeader.biSize));
		BmpImage->BytePointer += 4;
		BmpImage->CopyNumberFromString(BmpImage->BytePointer, 4, (unsigned int *) &(InfoHeader.biWidth));
		BmpImage->BytePointer += 4;
		BmpImage->CopyNumberFromString(BmpImage->BytePointer, 4, (unsigned int *) &(InfoHeader.biHeight));
		BmpImage->BytePointer += 4;
		BmpImage->CopyNumberFromString(BmpImage->BytePointer, 2, (unsigned short *) &(InfoHeader.biPlanes));
		BmpImage->BytePointer += 2;
		BmpImage->CopyNumberFromString(BmpImage->BytePointer, 2, (unsigned short *) &(InfoHeader.biBitCount));
		BmpImage->BytePointer += 2;
		BmpImage->CopyNumberFromString(BmpImage->BytePointer, 4, (unsigned int *) &(InfoHeader.biCompression));
		BmpImage->BytePointer += 4;
		BmpImage->CopyNumberFromString(BmpImage->BytePointer, 4, (unsigned int *) &(InfoHeader.biSizeImage));
		BmpImage->BytePointer += 4;
		BmpImage->CopyNumberFromString(BmpImage->BytePointer, 4, (unsigned int *) &(InfoHeader.biXPelsPerMeter));
		BmpImage->BytePointer += 4;
		BmpImage->CopyNumberFromString(BmpImage->BytePointer, 4, (unsigned int *) &(InfoHeader.biYPelsPerMeter));
		BmpImage->BytePointer += 4;
		BmpImage->CopyNumberFromString(BmpImage->BytePointer, 4, (unsigned int *) &(InfoHeader.biClrUsed));
		BmpImage->BytePointer += 4;
		BmpImage->CopyNumberFromString(BmpImage->BytePointer, 4, (unsigned int *) &(InfoHeader.biClrImportant));
		BmpImage->BytePointer += 4;

		*XDimension = InfoHeader.biWidth;
		*YDimension = InfoHeader.biHeight;
	    *NumberOfBits = InfoHeader.biBitCount;



	// Ensure that number of bytes per row is a multiple of 4.
		int		NumberOfBytesInRow;		// Always >= XDimension, since this must be a multiple of 4 for BMPs.
		int		NumberOfPadBytes;		// Number of additional bytes required to pad a row to a multiple of 4 bytes for BMPs.
		int		NumberOfColorPlanes;

		if(*NumberOfBits == 1)
		{
			NumberOfBytesInRow = (*XDimension / 8) + ((*XDimension % 8) ? 1 : 0);
			NumberOfColorPlanes = 1;
		}
		else if(*NumberOfBits == 8)
		{
			NumberOfBytesInRow = *XDimension;
			NumberOfColorPlanes = 1;
		}
//
//	For the time being, we won't process 24 bit images.  When we do, we need to setup the output image appropriately.  I think that everything else is
//	OK to go.
//		else if(*NumberOfBits == 24)
//		{
//			NumberOfBytesInRow = *XDimension;
//			NumberOfColorPlanes = 3;
//		}
//
		else
		{
			Status = false;
		}

		if(Status)
		{
			NumberOfPadBytes = 0;
			if(NumberOfBytesInRow % 4)
			{
				NumberOfPadBytes =  (4 - (NumberOfBytesInRow % 4));
				NumberOfBytesInRow += NumberOfPadBytes;
			}


		// Now, let's validate the total size.
			int		SizeOfRGBPallette = (*NumberOfBits <= 8) ? (sizeof(BMP_RGBQUAD) * (1 << *NumberOfBits)) : 0;
			int		TotalSize = FileHeader.bfOffBits + (*YDimension * NumberOfBytesInRow * NumberOfColorPlanes);

			if(TotalSize <= ((int) BmpImage->Size))
			{
			// Next, let's validate the headers.
				BmpImage->CopyFromString(BmpImage->BytePointer, SizeOfRGBPallette, (unsigned char *) &RGBPallette);
				BmpImage->BytePointer = FileHeader.bfOffBits;


				if((FileHeader.bfType[0] == BMP_ID[0]) && (FileHeader.bfType[1] == BMP_ID[1]) &&
					((((int) FileHeader.bfSize) == TotalSize) ||
						(((int) FileHeader.bfSize + BMP_BITMAPINFOHEADER_SIZE) == TotalSize)) && /* For OSPV images */
					(FileHeader.bfReserved1 == BMP_RESERVED) && (FileHeader.bfReserved2 == BMP_RESERVED) &&
					(FileHeader.bfOffBits >=
							((unsigned int) (BMP_BITMAPFILEHEADER_SIZE + BMP_BITMAPINFOHEADER_SIZE + SizeOfRGBPallette))) &&
					(InfoHeader.biSize == BMP_BITMAPINFOHEADER_SIZE) &&
					(InfoHeader.biPlanes == 1) &&
					(InfoHeader.biCompression == 0) &&
					((InfoHeader.biClrUsed == ((unsigned int) (1 << *NumberOfBits))) ||
						(InfoHeader.biClrUsed == 0)) &&
					((InfoHeader.biClrImportant == ((unsigned int) (1 << *NumberOfBits))) ||
						(InfoHeader.biClrImportant == 0)))
				{
				// Validate the pallette table.
					if(*NumberOfBits == 1)
					{
						if((RGBPallette[0].rgbBlue != 0) ||
							(RGBPallette[0].rgbGreen != 0) ||
							(RGBPallette[0].rgbRed != 0) ||
							(RGBPallette[0].rgbReserved != 0) ||
							(RGBPallette[1].rgbBlue != 255) ||
							(RGBPallette[1].rgbGreen != 255) ||
							(RGBPallette[1].rgbRed != 255) ||
							(RGBPallette[1].rgbReserved != 0))
						{
							Status = false;
						}
					}
					else
					{
/*						for(i = 0;    (i < 256) && (Status);    ++i)
						{
							if((RGBPallette[i].rgbBlue != i) ||
								(RGBPallette[i].rgbGreen != i) ||
								(RGBPallette[i].rgbRed != i)) // ||
//								(RGBPallette[i].rgbReserved != 0))
							{
								if((i != 255) || (RGBPallette[i].rgbBlue != 0) || (RGBPallette[i].rgbGreen != 0) ||
												(RGBPallette[i].rgbRed != 0) || (RGBPallette[i].rgbReserved != 0))
								{
									Status = false;
								}
							}
						}
*/					}

				// If no errors, process the actual image data.
					if(Status)
					{
						OutputImage->Zero();
						OutputImage->Reallocate( (*XDimension) * (*YDimension) );

					// Is it 8 bits per pixel?
						if(*NumberOfBits == 8)
						{
						// Copy in reverse row order.
							for(i = 0;    i < *YDimension;    ++i)
							{
								OutputImage->Append((char *)
									&(BmpImage->Char[(int) BmpImage->BytePointer + ((*YDimension - i - 1) * NumberOfBytesInRow)]),
																						*XDimension);
							}
						}

						else
						{
						// 1 bit per pixel
							int		X, Y;

							for(Y = 0;    Y < *YDimension;    ++Y)
							{
								BmpImage->ResetBitPointer();
								BmpImage->BytePointer = ((unsigned long) ((*YDimension - Y - 1) * NumberOfBytesInRow + (int) FileHeader.bfOffBits));
								for(X = 0;    X < *XDimension;    ++X)
								{
									OutputImage->AppendChar(((int) ((BmpImage->CopyNextBits(1)) ? 255 : 0)));
								}
							}
						}
					}
				}

				else
				{
				// Bad parameters; WOOF!
					Status = false;
				}
			}
	
			else
			{
			// Total size is bad; WOOF!
				Status = false;
			}
		} // endif Status still OK
	}
	
	else
	{
	// Dimensions are bad; WOOF!
		Status = false;
	}


// If we made it this far, then the output image size better be equal to the product of the dimensions.
	if((Status) && ((*XDimension * *YDimension) != ((int) OutputImage->Size)))
	{
		Status = false;
	}


//	RESET_WATCHDOG_TIMER();

	return(Status);
} // DecodeBMPImage



