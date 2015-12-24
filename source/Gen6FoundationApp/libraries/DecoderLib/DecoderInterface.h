//======================================================================================
// DecoderInterface.h
//======================================================================================
// $Source: DecoderInterface.h $
// $Revision: 1.37 $
// $Date: 2011/07/07 13:22:41EDT $
//======================================================================================

//======================================================================================



 
//---------------------------------------------------------------------------
//
//  Module Interface Description:
//      This file contains the interface definitions to create and interact with 
//		the decoder(s). This module will create separate decoder threads for each 
//		decoder it will interact with. 
//
//---------------------------------------------------------------------------



#ifndef DECODER_INTERFACE_H
#define DECODER_INTERFACE_H

/* Preamble */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//Constants file
#include "DecoderConstants.h"

//Overall config file for the control logic
#include "DecoderConfig.h"
	
//Settings that apply to multiple decoders. Still may not apply to all decoders, so be wary for now...
#include "DecoderSettings.h"

//Include the settings that apply to decoders that are built in.
#if (DECODER_ID)
#include "SwiftSettings.h"
#endif

#if (DECODER_AD)
#include "AdaptusSettings.h"
#endif

#if (DECODER_FLD)
//#include "FastLinearSettings.h" //not done yet, will have at least something in it
#endif

#if (DECODER_ULD)
#include "UnifiedLaserSettings.h"
#endif


/* Decoder Structures
 *	These are the structures by the decoder.
 */
typedef enum {
	CB_PRINTF,
	CB_TIMER,
	CB_RESULT,
	CB_RESULT_NOTIFY,
	CB_WATCHDOG,
	CB_STATUS,
	CB_PROGRESS
} CallbackTag;
	
typedef struct {
	int x;
	int y;
} Point_tag;

typedef struct {
	Point_tag corners[4];
} Bounds;

typedef struct {
	Bounds bounds;
	int d_corner;
	int pdfRows;
	int pdfCols;
} IQImageInfo_t;

typedef struct {
	int frameID;
	int line;
	int state;
} CircluarBufferState;

typedef enum {
	DEC_TYPE_NONE = 0,
	DEC_TYPE_ID = 1,
	DEC_TYPE_AD = 2,
	DEC_TYPE_FLD = 3,
	DEC_TYPE_ULD = 4
} DecodeType;

typedef struct {
	unsigned char Data[MAX_DECODED_DATA_LENGTH];
	int Length;
	Bounds MainBounds;
	Point_tag GraphicalCenter;
	unsigned int Symbology;
	unsigned int SymbologyEx;
	int Modifier;
	int ModifierEx;
	char HHPCodeID;
	char AIMCodeLetter;
	char AIMModifier;
	IQImageInfo_t IQImageInfo;
	int Parity;
	int AppendType;
	int AppendIndex;
	int AppendTotal;
	int LinkFlag;
	int ProgrammingCode;
	DecodeType DecodeType;
} DecoderResult;

/* Decoder Functions
 *	These are the functions used to communicate to the decoder.
 */

int InitDecoder( int width, int height, int wrap );
void DestroyDecoder( void );

void ReportDecoderVersion( int type, char *reportString );

int InstallCallback( /*Callback*/ unsigned int tag, void *function );

int Decode( void *image, int decodeType );
int StopDecode(void);
int ContinueDecode(void);
int PauseDecode(void);

int DecodeGet( unsigned int tag, void *data );
int DecodeSet( unsigned int tag, void *data );
void DecodeReset( void );

int GetLastDecoderError(void); //doesn't do anything, need to define how this will work.

/* 
	Temperature -- measure of how far along decoder is, not sure how to make portable 
*/
 #define COLD 2    // Just searching for symbols
 #define LUKEWARM 5   // Evidence of a particular symbol has been found
 #define WARM 8    // The type of symbol has been confirmed; Data is being collected
 #define HOT 16    // Data capture has been successful at some level, with more to go
 #define COOKED 48 //  A fully valid read has occurred; putting on finishing touches


/* Debug 
 *
 * These are the constants to dig into a specific level of the interface. Used only for debug set/get purposes for now.
 * They are what is passed into the "level" parameter of the debug functions below.
 */

#define DECODER_INTERFACE		0xDEC0
#define SWIFT_INTERFACE			0xDEC1
#define SWIFT_DECODER			0xDEC2
#define RAPIDSCAN_INTERFACE		0xDEC3
#define RAPIDSCAN_DECODER		0xDEC4

int DecodeGetDebug( unsigned int level, unsigned int tag, void *data );
int DecodeSetDebug( unsigned int level, unsigned int tag, void *data );

/* Postamble */
#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif //DECODER_INTERFACE_H

