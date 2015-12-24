//======================================================================================
// BmpImage.h
//======================================================================================
// $RCSfile: bmpimage.h $
// $Revision: 1.2 $
// $Date: 2011/06/17 04:37:04EDT $
//======================================================================================

//======================================================================================






//---------------------------------------------------------------------------
//
//  Definitions for BmpImage
//
//---------------------------------------------------------------------------
#ifndef BMP_IMAGE_H
#define BMP_IMAGE_H
//---------------------------------------------------------------------------




#include "BitString.h"			// BITSTRING interface definition



#define	BMP_ID				"BM"
#define	BMP_RESERVED		0



#define	BMP_BITMAPFILEHEADER_SIZE	14

typedef struct {
	char			bfType[2]; 
	unsigned int	bfSize; 
	unsigned short	bfReserved1; 
	unsigned short	bfReserved2; 
	unsigned int	bfOffBits; 
} BMP_BITMAPFILEHEADER;



#define	BMP_BITMAPINFOHEADER_SIZE	40

typedef struct {
    unsigned int	biSize;
    int				biWidth;
    int				biHeight;
    unsigned short	biPlanes;
    unsigned short	biBitCount;
    unsigned int	biCompression;
    unsigned int	biSizeImage;
    int				biXPelsPerMeter;
    int				biYPelsPerMeter;
    unsigned int	biClrUsed;
    unsigned int	biClrImportant;
} BMP_BITMAPINFOHEADER;


typedef struct { 
	unsigned char	rgbBlue;
	unsigned char	rgbGreen;
	unsigned char	rgbRed;
	unsigned char	rgbReserved;
} BMP_RGBQUAD;
 

/*
grayscale
		puts("BM");
#if COLORCHIP
		x = (((x1-x0+skip-1)/skip*3+3)/4*4)*((y1-y0+skip-1)/skip)+54; PUTCHAR((char)((x)&0xff));PUTCHAR((char)((x>>8)&0xff));PUTCHAR((char)((x>>16)&0xff));PUTCHAR((char)((x>>24)&0xff));
#else
		x = (((x1-x0+skip-1)/skip+3)/4*4)*((y1-y0+skip-1)/skip)+1024+54; PUTCHAR((char)((x)&0xff));PUTCHAR((char)((x>>8)&0xff));PUTCHAR((char)((x>>16)&0xff));PUTCHAR((char)((x>>24)&0xff));
#endif
		PUTCHAR(0); PUTCHAR(0); PUTCHAR(0); PUTCHAR(0);
#if COLORCHIP
		PUTCHAR(54); PUTCHAR(0); PUTCHAR(0); PUTCHAR(0);
#else
		PUTCHAR(54); PUTCHAR(4); PUTCHAR(0); PUTCHAR(0);
#endif
		PUTCHAR(40); PUTCHAR(0); putchar(0); putchar(0);
		x = (x1-x0+skip-1)/skip; PUTCHAR((char)((x)&0xff));PUTCHAR((char)((x>>8)&0xff));PUTCHAR((char)((x>>16)&0xff));PUTCHAR((char)((x>>24)&0xff));
		x = (y1-y0+skip-1)/skip; PUTCHAR((char)((x)&0xff));PUTCHAR((char)((x>>8)&0xff));PUTCHAR((char)((x>>16)&0xff));PUTCHAR((char)((x>>24)&0xff));
		PUTCHAR(1); PUTCHAR(0);
#if COLORCHIP
		PUTCHAR(24); PUTCHAR(0);
#else
		PUTCHAR(8); PUTCHAR(0);
#endif
		PUTCHAR(0); PUTCHAR(0); PUTCHAR(0); PUTCHAR(0);
		PUTCHAR(0); PUTCHAR(0); PUTCHAR(0); PUTCHAR(0);
		PUTCHAR(0); PUTCHAR(0); PUTCHAR(0); PUTCHAR(0);
		PUTCHAR(0); PUTCHAR(0); PUTCHAR(0); PUTCHAR(0);
		PUTCHAR(0); PUTCHAR(1); PUTCHAR(0); PUTCHAR(0);
		PUTCHAR(0); PUTCHAR(1); PUTCHAR(0); PUTCHAR(0);



binary
		puts("BM");
		x = (((x1-x0+skip-1)/skip+31)/32*4)*((y1-y0+skip-1)/skip)+62; PUTCHAR((char)((x)&0xff));PUTCHAR((char)((x>>8)&0xff));PUTCHAR((char)((x>>16)&0xff));PUTCHAR((char)((x>>24)&0xff));
		PUTCHAR(0); PUTCHAR(0); PUTCHAR(0); PUTCHAR(0);
		PUTCHAR(62); PUTCHAR(0); PUTCHAR(0); PUTCHAR(0);
		PUTCHAR(40); PUTCHAR(0); putchar(0); putchar(0);
		x = (x1-x0+skip-1)/skip; PUTCHAR((char)((x)&0xff));PUTCHAR((char)((x>>8)&0xff));PUTCHAR((char)((x>>16)&0xff));PUTCHAR((char)((x>>24)&0xff));
		x = (y1-y0+skip-1)/skip; PUTCHAR((char)((x)&0xff));PUTCHAR((char)((x>>8)&0xff));PUTCHAR((char)((x>>16)&0xff));PUTCHAR((char)((x>>24)&0xff));
		PUTCHAR(1); PUTCHAR(0);
		PUTCHAR(1); PUTCHAR(0);
		PUTCHAR(0); PUTCHAR(0); PUTCHAR(0); PUTCHAR(0);
		PUTCHAR(0); PUTCHAR(0); PUTCHAR(0); PUTCHAR(0);
		PUTCHAR(0); PUTCHAR(0); PUTCHAR(0); PUTCHAR(0);
		PUTCHAR(0); PUTCHAR(0); PUTCHAR(0); PUTCHAR(0);
		PUTCHAR(2); PUTCHAR(0); PUTCHAR(0); PUTCHAR(0);
		PUTCHAR(2); PUTCHAR(0); PUTCHAR(0); PUTCHAR(0);

		PUTCHAR(0); PUTCHAR(0); PUTCHAR(0); PUTCHAR(0);
		PUTCHAR(255); PUTCHAR(255); PUTCHAR(255); PUTCHAR(0);
		bYInvert = !bYInvert;
*/
/*
    DWORD  biSize;				// Specifies the number of bytes required by the structure.
    LONG   biWidth;				// Specifies the width of the bitmap, in pixels.
    LONG   biHeight;			// Specifies the height of the bitmap, in pixels. If biHeight is positive, the bitmap is a
								//	bottom-up DIB and its origin is the lower-left corner. If biHeight is negative, the bitmap
								//	is a top-down DIB and its origin is the upper-left corner.  If biHeight is negative,
								//	indicating a top-down DIB, biCompression must be either BI_RGB or BI_BITFIELDS.
								//	Top-down DIBs cannot be compressed. 
biPlanes 
Specifies the number of planes for the target device. This value must be set to 1. 
biBitCount 
Specifies the number of bits per pixel. The biBitCount member of the BITMAPINFOHEADER structure determines the number of bits that define each pixel and the maximum number of colors in the bitmap. This member must be one of the following values. 
Value Description 
1 The bitmap is monochrome, and the bmiColors member contains two entries. Each bit in the bitmap array represents a pixel. If the bit is clear, the pixel is displayed with the color of the first entry in the bmiColors table; if the bit is set, the pixel has the color of the second entry in the table. 
2 The bitmap has four possible color values.  
4 The bitmap has a maximum of 16 colors, and the bmiColors member contains up to 16 entries. Each pixel in the bitmap is represented by a 4-bit index into the color table. For example, if the first byte in the bitmap is 0x1F, the byte represents two pixels. The first pixel contains the color in the second table entry, and the second pixel contains the color in the sixteenth table entry. 
8 The bitmap has a maximum of 256 colors, and the bmiColors member contains up to 256 entries. In this case, each byte in the array represents a single pixel. 
16 The bitmap has a maximum of 2^16 colors. If the biCompression member of the BITMAPINFOHEADER is BI_RGB, the bmiColors member is NULL. Each WORD in the bitmap array represents a single pixel. The relative intensities of red, green, and blue are represented with 5 bits for each color component. The value for blue is in the least significant 5 bits, followed by 5 bits each for green and red. The most significant bit is not used. The bmiColors color table is used for optimizing colors used on palette-based devices, and must contain the number of entries specified by the biClrUsed member of the BITMAPINFOHEADER.  
24 The bitmap has a maximum of 2^24 colors, and the bmiColors member is NULL. Each 3-byte triplet in the bitmap array represents the relative intensities of blue, green, and red, respectively, for a pixel. The bmiColors color table is used for optimizing colors used on palette-based devices, and must contain the number of entries specified by the biClrUsed member of the BITMAPINFOHEADER.  
32 The bitmap has a maximum of 2^32 colors. If the biCompression member of the BITMAPINFOHEADER is BI_RGB, the bmiColors member is NULL. Each DWORD in the bitmap array represents the relative intensities of blue, green, and red, respectively, for a pixel. The high byte in each DWORD is not used. The bmiColors color table is used for optimizing colors used on palette-based devices, and must contain the number of entries specified by the biClrUsed member of the BITMAPINFOHEADER. 
If the biCompression member of the BITMAPINFOHEADER is BI_BITFIELDS, the bmiColors member contains three DWORD color masks that specify the red, green, and blue components, respectively, of each pixel. Each DWORD in the bitmap array represents a single pixel.
 




biCompression 
Specifies the type of compression for a compressed bottom-up bitmap (top-down DIBs cannot be compressed). This member can be the one of the following values. 
Value Description 
BI_RGB An uncompressed format 
BI_BITFIELDS Specifies that the bitmap is not compressed and that the color table consists of three DWORD color masks that specify the red, green, and blue components, respectively, of each pixel. This is valid when used with 16- and 32-bpp bitmaps. This value is valid in Windows CE versions 2.0 and later. 




biSizeImage 
Specifies the size, in bytes, of the image. This may be set to zero for BI_RGB bitmaps. 
biXPelsPerMeter 
Specifies the horizontal resolution, in pixels per meter, of the target device for the bitmap. An application can use this value to select a bitmap from a resource group that best matches the characteristics of the current device. 
biYPelsPerMeter 
Specifies the vertical resolution, in pixels per meter, of the target device for the bitmap 
biClrUsed 
Specifies the number of color indexes in the color table that are actually used by the bitmap. If this value is zero, the bitmap uses the maximum number of colors corresponding to the value of the biBitCount member for the compression mode specified by biCompression. 
If biClrUsed is nonzero and the biBitCount member is less than 16, the biClrUsed member specifies the actual number of colors the graphics engine or device driver accesses. If biBitCount is 16 or greater, the biClrUsed member specifies the size of the color table used to optimize performance of the system color palettes. If biBitCount equals 16 or 32, the optimal color palette starts immediately following the three DWORD masks. 

If the bitmap is a packed bitmap (a bitmap in which the bitmap array immediately follows the BITMAPINFO header and is referenced by a single pointer), the biClrUsed member must be either zero or the actual size of the color table. 

biClrImportant 
Specifies the number of color indexes required for displaying the bitmap. If this value is zero, all colors are required. 
Remarks
*/



// Create an 8 or 1 bit per pixel BMP image from the input image.
bool CreateBMPImage(BITSTRING *BmpImage, BITSTRING *InputImage, int XDimension, int YDimension, int NumberOfBits = 8,
					int Threshold = 128);
bool CreateBMPColorImage(BITSTRING *BmpColorImage, BITSTRING *InputImage, int XDimension, int YDimension, int NumberOfBits, int Threshold);

// Decode an 8 or 1bit per pixel BMP image into a raw output image.
bool DecodeBMPImage(BITSTRING *BmpImage, BITSTRING *OutputImage, int *XDimension, int *YDimension, int *NumberOfBits);

//----------------------------------------------------------------------------
#endif // BMP_IMAGE_H



