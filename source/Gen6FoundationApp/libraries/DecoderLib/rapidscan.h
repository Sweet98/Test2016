//======================================================================================
// RapidScan.h
//======================================================================================
// $Source: rapidscan.h $
// $Revision: 1.12 $
// $Date: 2011/04/13 14:49:02EDT $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Module Interface Description:
//      This file contains the interface definitions to create and interact with 
//		the RapidScan decoder.
//
//---------------------------------------------------------------------------



#ifndef RAPIDSCAN_H
#define RAPIDSCAN_H

/* Preamble */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef unsigned char MxPixel;
typedef enum {
	DECODE_TYPE_FAST = 0,
	DECODE_TYPE_FULL = 1
} DecodeType_t;

typedef struct {
   signed int x;   // x-coordinate
   signed int y;   // y-coordinate
} Point;

typedef struct {
   signed int x;
   signed int y;
} Corner_t;

typedef struct {
   signed int x;
   signed int y;
} RS_Corner_t;

typedef struct {
	Corner_t Corners[4];
	int D_Corner;
	int PdfRows;
	int PdfCols;
} IQImgInfo_t;

typedef struct {
	RS_Corner_t Corners[4];
	int D_Corner;
} RS_IQImgInfo_t;

typedef struct {
   unsigned char *message; // Output Data String
	int length;	 				// Number of Data Characters
	char menuFlag;				// Boolean: is this for Menuing Purposes
	char mustStop;				// Boolean: signals UnStructured Append result
	char lastRecord;			// Boolean: is this the final "Result" from the current decode?
	char HHPcodeID;			// HHP internal Code (Symbology) ID
	char AIMcodeLetter;		// AIM/FACT/ISO "Symbology Identifier"
	char AIMcodeModifier;	// ... and "Modifier" character
	DecodeType_t DecodeType;
	IQImgInfo_t IQImgInfo;
} DecodeResult;

typedef struct {
   unsigned char *message; // Output Data String
	int length;	 				// Number of Data Characters
	char menuFlag;				// Boolean: is this for Menuing Purposes
	char HHPcodeID;			// HHP internal Code (Symbology) ID
	char AIMcodeLetter;		// AIM/FACT/ISO "Symbology Identifier"
	char AIMcodeModifier;	// ... and "Modifier" character
   RS_IQImgInfo_t IQImgInfo;
} RS_DecodeResult;

typedef struct {	//Image Type #4 -- a 2D Grayscale Image
	int imageType;	// MUST be 4!
	int height, width; // Size of grayscale Image
	int arrayWidth;	// Width of the Image array, including any dead pixels
	int verticalCenter;	// line number of Aimer's center (about height/2);
	int horizontalCenter;	// column number of Aimer's center (about width/2);
	char whiteHigh;	// Boolean: 1/TRUE = White high, 0/FALSE = Black high
	char mirrorImage;	// 0 = Normal, 1 = Left-to-Right Mirror Inverse, 2 = Top-to-Bottom Mirror Inverse
	char imageRotation; // 0 = normal, 1 = 90* Right, 2 = 180* (Upside Down), 3 = 270* Right
	char color;			// Boolean: is this a Color Image?
	int WhiteMax;	// Maximum white value
	int WhiteClip;
	int imageNumber; //number since trigger pull
   unsigned char *array;   // Start of the Pixel array
} GrayscaleImage2;

typedef struct {  // Image Type #2 -- a 1D Grayscale Image (i.e. ScanProfile !!)
	int imageType;	// MUST be 2!
	int length;		// Length of linear Image
	char whiteHigh;	// Boolean: 1/TRUE = White high, 0/FALSE = Black high
	MxPixel *array;	// Start of the Pixel array
} ScanProfile;

typedef struct {  // Image Type #3 -- a 1D Binary Image (i.e. ascending TimerCounts !!)
	int imageType;	// MUST be 3!
	int length;		// Length of the TimerCount list
	int *array;		// Start of the TimerCount list (Start of a Space is FIRST entry!!)
} TimerCounts;

typedef struct {
	int imageType; 
	int height, width;
	int arrayWidth;			// Width of the Image array, including any dead pixels
	char whiteHigh;			// Boolean: 1/TRUE = White high, 0/FALSE = Black high
	char color;				// Boolean: is this a Color Image?
   unsigned char *array;	// Start of the Pixel array
} RS_Image;


void *RS_Create(int width, int height, void *resultCB);
int RS_Decode(RS_Image *img);
void RS_Destroy(void);
int RS_Set(int tag, void *data);
int RS_Get(int tag, void *data);
int RS_Reset(void);
void RS_ReportDecoderVersion( char *reportString );

enum {
RS_DEC_C128_FNC1_SUBSTITUTE,
RS_DEC_C128_FNC_TRANSMIT,

RS_DEC_CODABAR_CHECK_DIGIT_MODE,
RS_DEC_CODABAR_CONCAT_ENABLED,
RS_DEC_CODABAR_ENABLED,
RS_DEC_CODABAR_MAX_LENGTH,
RS_DEC_CODABAR_MIN_LENGTH,
RS_DEC_CODABAR_START_STOP_TRANSMIT,

RS_DEC_CODE128_ENABLED,
RS_DEC_CODE128_MAX_LENGTH,
RS_DEC_CODE128_MIN_LENGTH,

RS_DEC_CODABLOCK_A_ENABLED,
RS_DEC_CODABLOCK_F_ENABLED,

RS_DEC_GS1_128_ENABLED,
RS_DEC_GS1_128_MAX_LENGTH,
RS_DEC_GS1_128_MIN_LENGTH,

RS_DEC_C128_ISBT_ENABLED,

RS_DEC_CODE39_BASE32_ENABLED,
RS_DEC_CODE39_CHECK_DIGIT_MODE,
RS_DEC_CODE39_ENABLED,
RS_DEC_CODE39_FULL_ASCII_ENABLED,
RS_DEC_CODE39_APPEND_ENABLED,
RS_DEC_CODE39_MAX_LENGTH,
RS_DEC_CODE39_MIN_LENGTH,
RS_DEC_CODE39_START_STOP_TRANSMIT,

RS_DEC_TLC39_ENABLED,
RS_DEC_COMPOSITE_WITH_UPC_ENABLED,
RS_DEC_COMPOSITE_ENABLED,

RS_DEC_COUPON_CODE_MODE,

RS_DEC_EAN13_2CHAR_ADDENDA_ENABLED,
RS_DEC_EAN13_5CHAR_ADDENDA_ENABLED,
RS_DEC_EAN13_ADDENDA_REQUIRED,
RS_DEC_EAN13_ADDENDA_SEPARATOR,
RS_DEC_EAN13_CHECK_DIGIT_TRANSMIT,
RS_DEC_EAN13_ENABLED,
RS_DEC_EAN13_ISBN_ENABLED,

RS_DEC_EAN8_2CHAR_ADDENDA_ENABLED,
RS_DEC_EAN8_5CHAR_ADDENDA_ENABLED,
RS_DEC_EAN8_ADDENDA_REQUIRED,
RS_DEC_EAN8_ADDENDA_SEPARATOR,
RS_DEC_EAN8_CHECK_DIGIT_TRANSMIT,
RS_DEC_EAN8_ENABLED,

RS_DEC_GET_ENERGY,
RS_DEC_SECURITY_LEVEL,

RS_DEC_I25_CHECK_DIGIT_MODE,
RS_DEC_I25_ENABLED,
RS_DEC_I25_MAX_LENGTH,
RS_DEC_I25_MIN_LENGTH,

RS_DEC_RSS_14_ENABLED,
RS_DEC_RSS_EXPANDED_ENABLED,
RS_DEC_RSS_EXPANDED_MAX_LENGTH,
RS_DEC_RSS_EXPANDED_MIN_LENGTH,
RS_DEC_RSS_LIMITED_ENABLED,
RS_DEC_RSS_MODS_SYMBOLOGY,

RS_DEC_UPCA_2CHAR_ADDENDA_ENABLED,
RS_DEC_UPCA_5CHAR_ADDENDA_ENABLED,
RS_DEC_UPCA_ADDENDA_REQUIRED,
RS_DEC_UPCA_ADDENDA_SEPARATOR,
RS_DEC_UPCA_CHECK_DIGIT_TRANSMIT,
RS_DEC_UPCA_ENABLED,
RS_DEC_UPCA_NUMBER_SYSTEM_TRANSMIT,

RS_DEC_UPCE0_ENABLED,
RS_DEC_UPCE1_ENABLED,
RS_DEC_UPCE_2CHAR_ADDENDA_ENABLED,
RS_DEC_UPCE_5CHAR_ADDENDA_ENABLED,
RS_DEC_UPCE_ADDENDA_REQUIRED,
RS_DEC_UPCE_ADDENDA_SEPARATOR,
RS_DEC_UPCE_CHECK_DIGIT_TRANSMIT,
RS_DEC_UPCE_EXPAND,
RS_DEC_UPCE_NUMBER_SYSTEM_TRANSMIT,

RS_DEC_SEARCH_CENTER_X,
RS_DEC_SEARCH_CENTER_Y,
};

/* Postamble */
#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif

