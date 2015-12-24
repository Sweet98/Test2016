//======================================================================================
// DecoderInterface.c
//======================================================================================
// $Source: Source/DecoderInterface.c $
// $Revision: 1.93 $
// $Date: 2011/10/31 16:06:30EDT $
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



/* Decoder Functions
 *	These are the functions used to communicate to the decoder.
 */

#include "../DecoderInterface.h"

#if (DECODER_ID)
#include "SwiftInterface.h"
#endif

#if (DECODER_AD)
#include "AdaptusInterface.h"
#endif

#if (DECODER_FLD)
#include "RapidScanInterface.h"
#endif

#if (DECODER_ULD)
#include "UnifiedLaserInterface.h"
#endif


//temporary until callbacks are straight
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DECODE_DEBUG 0

#if DECODE_DEBUG
	#include "db_chrio.h"
	#include "ioman.h"
	#define Pt(format, ... ) do {theCurrentInterface.printf(format, ## __VA_ARGS__); wait_til_tx_is_empty();} while (0) 
#else
	#define Pt(format, ... ) do{}while(0)
#endif

/* Preamble */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if (DECODER_ID)
static int fullDecoder;
#endif
#if (DECODER_AD)
static int fullDecoder2;
#endif
#if (DECODER_FLD)
static int fastDecoder;
#endif
#if (DECODER_ULD)
static int laserDecoder;
#endif

static int FastDecoderEnabled;
static int FullDecoderEnabled;

static volatile char needToClearResults;

/* Result Handling
	This section defines the things needed to store any and all results, whether is it a single result
	or a list of them. 
*/

#define MAX_STORED_RESULTS 10
DecoderResult tempResults[MAX_STORED_RESULTS]; //results storage
static volatile int ResultMutex;

static volatile int noread = 0;
#define MAX_ID_LENGTH	50
static int finishingAppendType;			//flag to tell what is needed to finish a sequence
static char appendID[MAX_ID_LENGTH];	//ID holder so that if there is an ID for an append, we can test against it so data is not mixed. 

#if (OCR_SEPARATE)
#define OCR_MAX_TEMPLATES		MAX_STORED_RESULTS	//only allow the number of breakdowns per number of allowed results, since those are the pieces to be stitched
#define OCR_MAX_TEMPLATE_CHARS	50			//OCR has at most 50chars per line. This is only used for spaces. 25 cuts the line in half. if more is needed, adjustments can be made per build.
#define OCR_NO_MATCH			-1
#define OCR_MULTI_MATCH			SORT_BY_LOCATION
#define OCR_USER_TEMPLATE		1

typedef struct {
	int length;
	int templ[OCR_MAX_TEMPLATE_CHARS];
	int groupTotal;
	int group;
} OCRTemplate;

static OCRTemplate OCRTemplates[OCR_MAX_TEMPLATES];
static int OCRNumberOfTemplates;
static int OCRSeparateTemplate;
static int OCRActiveTemplates;	//only care when we're separating the user template, and so we do it when only user template is active
static int OCRResultsMatchMultiple;
#endif

#if (DECODER_ID)
static int SI_OptimizeEnables;	// flag to optimize enables for append reading. Save the append type to this and it will optimize for that type of append, saving all changed settings to restore them.
static int SI_DisabledGroups;	//used to store DEC_DISABLE_SYMBOLOGY_GROUP setting before changing it to improve append reading.
static int SI_DisableQR;
static int SI_DisableHanXin;
static int SI_DisableMaxi;
static int SI_DisableDataMatrix;
static int SI_DisableAztec;
#endif
static int StopTheDecoder;		//used to store DEC_DISABLE_SYMBOLOGY_GROUP setting before changing it to improve append reading.
//static int FullDecoderStop;
static int decodeStartTime;			//start timestamp for a decode, if we need to track it
static int decodeTime;				//max amount of time in an image to find a result
static int additionalSearchTime;	//timer to tell how much longer to search for more pieces
static int timeStamp;				//time stamp for when a piece of an append is found
static int storedResultsIndex;		//Index of the next result to store. Also tells us how many are currently stored.
static Bounds EmptyBounds;
static IQImageInfo_t EmptyIQInfo;

//Types of appends
#define NO_APPEND				0x0000
#define CODE39_APPEND			(1 << 0)
#define COUPON_CODE_APPEND		(1 << 1)
#define CODE93_APPEND			(1 << 2)
#define MACROPDF_APPEND			(1 << 3)
#define CODE128_APPEND			(1 << 4)
#define AZTEC_APPEND			(1 << 5)
#define QR_APPEND				(1 << 6)
#define DATAMATRIX_APPEND		(1 << 7)
#define GRIDMATRIX_APPEND		(1 << 8)
#define UCC_COMPOSITE_APPEND	(1 << 9)
#define CODABAR_APPEND			(1 << 10)
#define ISBT_APPEND				(1 << 11)
#define UPC_COMPOSITE_APPEND	(1 << 12)
#define OCR_APPEND				(1 << 13) //Passport Reading
#define OCR_USER_APPEND			(1 << 14) //Space separated template
#define KIX_REVERSE_CODE		(1 << 15) //KIX reverse code reading.
#define APPEND_FINISHED			0xFFFF//;Pt("fin:%d\r\n",__LINE__)

#define SORT_BY_LOCATION	(MAX_STORED_RESULTS + 1)	//flag to tell sorting of order by the position of the code in the image

//We don't support reading these code, but they should be mared as having append capabilites
//in case we do support them in the future
//#define REFERENCE_TARGET_APPEND	0x1000	
//#define DOTCODE_APPEND			0x2000	
//#define POSICODE_APPEND			0x4000	
//#define CODE49_APPEND				0x8000

typedef struct {
	int topy, boty, *lftx, *rgtx, maxy;
	Point_tag lastxy;
} outline;
#define TY ol->topy
#define BY ol->boty
#define LX ol->lftx
#define RX ol->rgtx
#define LASTXY ol->lastxy
#define MAXY ol->maxy

static void From (outline *ol, Point_tag a);
static void To (outline *ol, Point_tag b);

static DecoderResult *StoreResult( int decoder, DecoderResult *res );
static void ClearStoredResults( int excludeFlags );
static void SortResults( void );
static void CopyResult( DecoderResult *source, DecoderResult *dest );
static void PrintResult( DecoderResult *res, int resNumber );
static int CheckAppendType( DecoderResult *res );
static void CheckEANEmulation( void );
static int CheckDecodeWindow( DecoderResult *res );
static void CombineResults( int endIndex );
static void DeleteCharacter( DecoderResult *res, int charIndex );
static void InsertCharacter (unsigned char *res,int *len, unsigned char c, int x);
static void ConvertTLCMessage( DecoderResult *res );
static void InvertImage(void *p_image);

#if( OCR_SEPARATE )
static int CheckOCRTemplate( unsigned char *templ );
static int CheckOCRResult( DecoderResult *res, int *temps );
#endif

static int MacroPDFEnabled;
static int PDFCodewordIndex;
static int MacroPDFZero;	//firstID in the sequence
//this is in case not all of the pieces are scanned, in which case it would reset the scanner
static int MacroPDFMax;		//last ID in the sequence
#define PDF_MAX_CODEWORDS 20 //max number of code words we can store from the end of the code. 
static int PDFCodewords[PDF_MAX_CODEWORDS];
static void ClearPDFCodewords(void);

static int ECIHandling;
static int Code39AppendEnabled;
static int Code128AppendEnabled;
static int Code93AppendEnabled;
static int AztecAppendEnabled;
static int AztecAppendStripInfo;
static int QRAppendEnabled;
//static int DatamatrixAppendEnabled;
//static int DatamatrixAppendStripInfo;
//static int GridmatrixAppendEnabled; in adaptus
static int TLC39Enabled;
static int CouponCodesEnabled;
static int CouponSecondaryOutput; //output secondary GS1-databar only if enabled and present.
static int ISBTEnabled;
static int Code128EnableTemp;
static int EANEmulationMode;
static int UPCEExpansionTemp;
static int CodabarAppendEnabled;
static int CodabarAppendRequired;
static int CodabarAppendStartStop;
static int CodabarStartStopTransmit;
static int ConcatAdditionalSearchTime;
static int UPCCompositeAdditionalSearchTime;
static int UPCCompositeEnabled;
static int CompositeEnabled;
static int CompositeMin;
static int CompositeMax;

static int PrintDecodeResults;
static int CombineCompositeResults;

#define VID_REVERSE_OFF		0
#define VID_REVERSE_ONLY	1
#define VID_REVERSE_BOTH	2

static int TryReverseVideo;

#define VID_REVERSE_NONE			0	
#define VID_REVERSE_ATTEMPT			1
#define VID_REVERSE_STORED_RESULT	2
#define VID_REVERSE_DECODED			4

static int VideoReverseResult;

/* END Result Info */

/* Windowing */
static int DecodeWindowMode;
static int WindowOffset;				//new offset if DECWIN3. 
static int ShowDecodeWindow; 
static int ImageRotation;

static int DecodeWindowTop;				//coordinate value
static int DecodeWindowTopPercent;		//percentage used to calculate coordinate value
static int DecodeWindowBottom;
static int DecodeWindowBottomPercent;
static int DecodeWindowLeft;
static int DecodeWindowLeftPercent;
static int DecodeWindowRight;
static int DecodeWindowRightPercent;

static int MonocolorImage;
static int MonocolorOffsetX;
static int MonocolorOffsetY;
static int MonocolorSpacingX;
static int MonocolorSpacingY;
static int ImageHeight;
static int ImageWidth;
static int ImageWrap;
static int ImageCenterX;
static int ImageCenterY;

void SetupWindow( void );

//Callback functions
typedef  void (*fPrintf_t)(void);
typedef  int (*fTimer_t)(void);
typedef  void (*fResult_t)(int,DecoderResult *);
typedef  void (*fResultNotify_t)(int);
typedef  void (*fWatchDog_t)(void);
typedef  void (*fStatus_t)(int,int);
typedef  void (*fProgress_t)(int);


// should be static eventualy

fPrintf_t fPrintf;
fTimer_t fTimer;
fResult_t fResult;
fResultNotify_t fResultNotify;
fWatchDog_t fWatchDog;
fStatus_t fStatus;
fProgress_t fProgress;

#define DECPRINTF	fPrintf
#define DECTIMER	fTimer
#define DECRESULT	fResult
#define DECNOTIFY	fResultNotify
#define DECWATCHDOG	fWatchDog
#define DECSTATUS	fStatus
#define DECPROGRESS	fProgress

//Local fucntions
static void ResultCallback( int decoder, DecoderResult *res ) {

	DecoderResult *tempRes;

	if( ResultMutex ) {
		return;
	}

	ResultMutex = 1;

	Pt("Main result %s\r\n", res->Data);
	
	//If someone told the outside that we have results, we don't need/want them here locally.
	//This prevents fast/full conflict when each are working on an image and results need to be cleared in between.
	if( needToClearResults ) {
		ClearStoredResults(0);
	}

	if( CheckDecodeWindow( res ) ) { //if the result is within the defined window
		tempRes = StoreResult(decoder, res);
	}
	else {
		ResultMutex = 0;
		return;
	}
	
	//Everything is good, call next level
	if( DECRESULT && tempRes ) {
		if( (finishingAppendType == APPEND_FINISHED) || //if we're done with the append (or single code)
			( (additionalSearchTime > 0) && (DECTIMER) &&			//if we have a timer and we're allowed to use it
			( ((DECTIMER())-timeStamp) > additionalSearchTime) )) { //and the timeout has expired
			SortResults();
			//if we only got a partial, reduce the total expected by 1
			if( finishingAppendType == COUPON_CODE_APPEND ||
				finishingAppendType == CODABAR_APPEND ||
				finishingAppendType == UPC_COMPOSITE_APPEND || 
				tempResults[0].AppendType == KIX_REVERSE_CODE ) { 
					
				tempResults[0].AppendTotal--;
				
				if( finishingAppendType == CODABAR_APPEND ) {
					if( !CodabarStartStopTransmit ) {
						tempResults[0].Data[tempResults[0].Length] = 0;
						tempResults[0].Length--;
						DeleteCharacter(&(tempResults[0]),0);
					}
				}
						
			}

			needToClearResults = 1;
			timeStamp = additionalSearchTime = 0;
			if( VideoReverseResult & VID_REVERSE_ATTEMPT ) {
				VideoReverseResult = VID_REVERSE_DECODED;
			}

			DECRESULT(decoder, &tempResults[0]);
		}
		else if( (finishingAppendType == CODE128_APPEND ||
				  finishingAppendType == CODE39_APPEND ||
				  finishingAppendType == CODE93_APPEND ||
				  finishingAppendType == MACROPDF_APPEND ) &&
				  DECNOTIFY ) {

					ClearPDFCodewords();
					  DECNOTIFY(0);
		}
	}
	ResultMutex = 0;
}

static void StatusCallback( int decoder, int status ) {

	//Everything is good, call next level
	if( DECSTATUS ) {
		DECSTATUS(decoder,status);
	}
}

static void ProgressCallback( int decoder ) {
	if( StopTheDecoder ) {
#if (DECODER_ID)
		SI_StopDecode(fullDecoder);
#endif
		StopTheDecoder = 0;
	}

	else if( DECTIMER && decodeStartTime && (DECTIMER()-decodeStartTime) > decodeTime/2 ) {
#if (DECODER_ID)
		SI_StopDecode(fullDecoder);
#endif
		decodeStartTime = 0;
	}
	//Everything is good, call next level
	else if( DECPROGRESS ) {
		DECPROGRESS(decoder);
	}
}


static DecoderResult *StoreResult( int decoder, DecoderResult *res ) {
	/*TBD: add new space to the structures if needed*/

	Pt("Store Result\r\n");
	CopyResult(res, &tempResults[storedResultsIndex]);
	//check to see if there is an append and fill in the append info
	int appendReturn = CheckAppendType(&tempResults[storedResultsIndex]);
	if( appendReturn != DEC_SUCCESS ) {
		if( appendReturn != DEC_SKIP ) {
			Pt("append fail %d\r\n", appendReturn);
			ClearStoredResults(0);			//the append type failed, which means we had some append info, but that is incorrect now
		}
		if( appendReturn == DEC_HALT || appendReturn == DEC_SKIP ) {
			return (DecoderResult *)0;
		}
		else {
			return StoreResult(decoder,res);	//so clear everything out and re-store this latest result
		}
	}

	if( VideoReverseResult & VID_REVERSE_ATTEMPT ) {
		VideoReverseResult |= VID_REVERSE_STORED_RESULT;
	}
	return &tempResults[storedResultsIndex++];
}

static void ClearStoredResults( int excludeFlags ) {
	//results will only have 1 type of append, so if the type that's stored doesn't need to be excluded
	//from being cleared, clear the results
	Pt("Clear Results\r\n");
	if( !(excludeFlags & tempResults[0].AppendType) ) {
		for( int jk = 0; jk<MAX_STORED_RESULTS; jk++ ) {
			if( tempResults[jk].Length >= 0 ) {
				for( int jk2 = 0; jk2<MAX_DECODED_DATA_LENGTH; jk2++ ) {
					tempResults[jk].Data[jk2] = 0;
				}
				tempResults[jk].Length = 0;
				tempResults[jk].MainBounds = EmptyBounds;
				tempResults[jk].Symbology = 0;
				tempResults[jk].SymbologyEx = 0;
				tempResults[jk].Modifier = 0;
				tempResults[jk].ModifierEx = 0;
				tempResults[jk].HHPCodeID = 0;
				tempResults[jk].AIMCodeLetter = 0;
				tempResults[jk].AIMModifier = 0;
				tempResults[jk].IQImageInfo = EmptyIQInfo;
				tempResults[jk].Parity = 0;
				tempResults[jk].AppendType = NO_APPEND;
				tempResults[jk].AppendIndex = 0;
				tempResults[jk].AppendTotal = 0;
				tempResults[jk].LinkFlag = 0;
				tempResults[jk].ProgrammingCode = 0;
				tempResults[jk].DecodeType = DEC_TYPE_NONE;
			}
		}

		ClearPDFCodewords();

		MacroPDFZero = MacroPDFMax = 0;
		CodabarAppendStartStop = 0;
		finishingAppendType = NO_APPEND;
		additionalSearchTime = storedResultsIndex = 0;
		
		memset(appendID,0,MAX_ID_LENGTH);
	}
	needToClearResults = 0;
}

static void ClearPDFCodewords( void ) {
	for( PDFCodewordIndex = 0; PDFCodewordIndex<PDF_MAX_CODEWORDS; PDFCodewordIndex++ ) {
		PDFCodewords[PDFCodewordIndex] = 0;
	}		
	PDFCodewordIndex = 0;
}

static void SortResults( void ) {

	int jj, jk;

	//only do this is we have more than 1 result
	if( storedResultsIndex > 1 ) {
		DecoderResult *tempRes = (DecoderResult *)malloc(sizeof(DecoderResult)*(storedResultsIndex+1));	//array to sort the results into
		char pieces[MAX_STORED_RESULTS];	//array to mark which ones got sorted
		int totalPieces = tempResults[storedResultsIndex-1].AppendTotal;
		int lastPiece = totalPieces-1;
		int needsSorting;

resort:
		//clear the memory so if something goes wrong it doesn't reset anything
		memset(tempRes,0,sizeof(DecoderResult)*(storedResultsIndex+1));
		memset(pieces,0,MAX_STORED_RESULTS);
		
		needsSorting = totalPieces;

		// go through all the pieces we're supposed to have to see if any were marked with their place
		for( jj = 0; jj < totalPieces; jj++ ) {
			//mark each piece to be sorted by location in case it isn't actually found in the results
			pieces[jj] = SORT_BY_LOCATION; 
			
			Pt("Result[%d]: ",jj); for( int jl=0; jl<tempResults[jj].Length; jl++ ) {Pt( "%c", tempResults[jj].Data[jl]);}Pt("    %d\r\n",tempResults[jj].Length);

			//go through all the results to see if there are any marked for the current position (jj)
			for( jk = 0; jk < storedResultsIndex; jk++ ) {
				//if is a match, copy it to the proper location
				if( tempResults[jk].AppendIndex == jj ) {
					CopyResult( &tempResults[jk], &tempRes[jj] );
					pieces[jj] = 1;	//mark the position as found
					needsSorting--;	//decrement the number to be sorted, since this one doesn't
				}
			}
		}
		
		//if there are still some left to be sorted
		if( needsSorting > 0) {
			int walk = 1;			//which direction to walk along the results
			int locked = 0;			//piece we're looking at to determine the next one
			int start = locked;		//position we began to we can go to it and walk the other direction
			int dist = 0;			//distance between corners we're testing
			int minDist;			//shortest distance when we're testing
			int minLock = locked;	//result of the shortest distance
			Point_tag cornerL,cornerD;	//corners we're testing distance between
			int thisCorner = 1;		//corner we want to find the closest to
			int testCorner = 0;		//corner closest to
			int sortedAll = DEC_FALSE;	//marker for if we sorted all pieces by position
			int direction = tempResults[0].MainBounds.corners[0].x-tempResults[0].MainBounds.corners[1].x;
			
			Pt("%d need sorting!\r\n",needsSorting);
			
			//initial corners are for searching left to right with codes left to right,
			//we still want to search left to right, but codes will be right to left, so switch the corners
			if( (ImageRotation <= 1 && direction >= 0) || (ImageRotation > 1 && direction < 0 ) ) {
				thisCorner = 0; 
				testCorner = 1;
			}

			//if there weren't any pieces locked in place by the first scan, find the left-most one
			if( needsSorting == totalPieces ) {
				int center = tempResults[0].GraphicalCenter.x;

				for( jk = 1; jk < storedResultsIndex; jk++ ) {
					if( center > tempResults[jk].GraphicalCenter.x ) {
						center = tempResults[jk].GraphicalCenter.x;
						locked = jk;
					}
				}

				CopyResult(&tempResults[locked],&tempRes[0]);
				tempRes[0].AppendIndex = 0;
				locked = 0;
				pieces[locked] = 1;
				needsSorting--;
				sortedAll = DEC_TRUE;
			}
			else {
				//find the first piece to sort against
				for( jk = 0; jk < totalPieces; jk++ ) {
					if( pieces[jk] != SORT_BY_LOCATION ) {
						locked = start = jk;
						break;
					}
				}
			}
			
			while( needsSorting ) {

				cornerL = tempRes[locked].MainBounds.corners[thisCorner];
				minDist = 0;

				//find the closest corner
				for( jk = 0; jk < storedResultsIndex; jk++ ) {
					//if both corners are the same, then we're dealing with the exact same point, so we just skip it
					if( cornerL.x != tempResults[jk].MainBounds.corners[thisCorner].x ||
						cornerL.y != tempResults[jk].MainBounds.corners[thisCorner].y ) { 

						cornerD = tempResults[jk].MainBounds.corners[testCorner];

						dist = ((cornerD.x-cornerL.x)*(cornerD.x-cornerL.x)) + ((cornerD.y-cornerL.y)*(cornerD.y-cornerL.y));
					
						if( minDist > dist || minDist == 0 ) {
							minDist = dist;
							minLock = jk;
						}
					}
				}
				
				//advance to the next position, which we just found
				locked += walk;

				//only set this piece if there wasn't anything set to it previously
				if( pieces[locked] == SORT_BY_LOCATION ) {
					pieces[locked] = 1;
					//copy the result there so it's now locked
					CopyResult(&tempResults[minLock],&tempRes[locked]);
					tempRes[locked].AppendIndex = locked;
					
					needsSorting--;
					Pt("---left to be sorted %d\r\n",needsSorting);
				}
					
				//if we reached the end, go back to the start and go the other direction
				if( locked >= lastPiece ) {
					locked = start;
					walk = -walk;
				}
			}

			if( sortedAll ) {
				//we only care about reversing the order of the sort if all of the pieces were sorted graphically
				if( (ImageRotation <= 1 && direction >= 0) || (ImageRotation > 1 && direction < 0 ) ) {
					for( jj = 0, jk = lastPiece; jj < totalPieces; jj++, jk-- ) {
						tempRes[jj].AppendIndex = jk;
						tempRes[jj].AppendTotal = totalPieces;
						CopyResult(&tempRes[jj],&tempResults[jj]);
					}

					goto resort;
				}
			}
		}
		
		for( jk = 0; jk < storedResultsIndex; jk++ ) {
			CopyResult( &tempRes[jk], &tempResults[jk] );
			Pt("Result[%d]: ",jk); for( int jl=0; jl<tempResults[jk].Length; jl++ ) {Pt( "%c", tempResults[jk].Data[jl]);}Pt("    %d\r\n",tempResults[jk].MainBounds.corners[0].x);
			
			if( PrintDecodeResults && (tempResults[0].DecodeType == DEC_TYPE_ID || tempResults[0].DecodeType == DEC_TYPE_AD) ) {
				PrintResult(&tempResults[jk],jk);
			}
		}
		free(tempRes);

		CheckEANEmulation();
		
		if( tempResults[0].Symbology == DEC_SYMBOLOGY_AZTEC_CODE || 
			tempResults[0].AppendType == OCR_APPEND ||
			tempResults[0].AppendType == MACROPDF_APPEND ||
			tempResults[0].AppendType == QR_APPEND ) {
			
				CombineResults(storedResultsIndex-1);
		}
	}//storedResultsIndex > 1
	else { //only 1 result
		CheckEANEmulation();

		if( PrintDecodeResults && (tempResults[0].DecodeType == DEC_TYPE_ID || tempResults[0].DecodeType == DEC_TYPE_AD|| tempResults[0].DecodeType == DEC_TYPE_ULD) && storedResultsIndex == 1 ) {
			PrintResult(&tempResults[0],0);
		}
	}
}


static void CopyResult( DecoderResult *source, DecoderResult *dest ) {
	for( int jk=0; jk<source->Length; jk++ ) {
		dest->Data[jk] = source->Data[jk];
	}
	dest->Length				= source->Length;
	dest->MainBounds			= source->MainBounds;
	dest->GraphicalCenter		= source->GraphicalCenter;
	dest->Symbology				= source->Symbology;
	dest->SymbologyEx			= source->SymbologyEx;
	dest->Modifier				= source->Modifier;
	dest->ModifierEx			= source->ModifierEx;
	dest->HHPCodeID				= source->HHPCodeID;
	dest->AIMCodeLetter			= source->AIMCodeLetter;
	dest->AIMModifier			= source->AIMModifier;
	dest->IQImageInfo			= source->IQImageInfo;
	dest->Parity				= source->Parity;
	dest->AppendType			= source->AppendType;
	dest->AppendIndex			= source->AppendIndex;
	dest->AppendTotal			= source->AppendTotal;
	dest->LinkFlag				= source->LinkFlag;
	dest->ProgrammingCode		= source->ProgrammingCode;
	dest->DecodeType			= source->DecodeType;
}

static void PrintResult( DecoderResult *res, int resNumber ) {
	Pt( "___________________\r\n" );
	Pt( "Result #%d:\r\n", resNumber );
	Pt( "Data: " );
	for( int jk=0; jk<res->Length; jk++ ) {
		Pt( "%c", res->Data[jk]);
	}
	Pt( "\r\nLength: %d\r\n",res->Length );
	Pt( "Symbology: %d\r\n", res->Symbology );
	Pt( "SymbologyEx: %d\r\n", res->SymbologyEx );
	Pt( "Modifier: %d/%c\r\n", res->Modifier, res->Modifier );
	Pt( "ModifierEx: %d/%c\r\n", res->ModifierEx,res->ModifierEx );
	Pt( "HHPCodeID: %d\r\n", res->HHPCodeID );
	Pt( "AIMCodeLetter: %c\r\n", res->AIMCodeLetter );
	Pt( "AIMModifier: %c\r\n", res->AIMModifier );
	Pt( "Parity: %d\r\n", res->Parity );
	Pt( "AppendType: %04X\r\n", res->AppendType );
	Pt( "AppendIndex: %d\r\n", res->AppendIndex );
	Pt( "AppendTotal: %d\r\n", res->AppendTotal );
	Pt( "LinkFlag: %d\r\n", res->LinkFlag );
	Pt( "Programming Label: %d\r\n",res->ProgrammingCode );
	switch(res->DecodeType) {
		case DEC_TYPE_ID:
			Pt( "Decode Type: ID\r\n"); break;
		case DEC_TYPE_AD:
			Pt( "Decode Type: ADAPTUS\r\n"); break;
		case DEC_TYPE_FLD:
			Pt( "Decode Type: RAPIDSCAN\r\n"); break;
		case DEC_TYPE_ULD:
			Pt( "Decode Type: LASER\r\n"); break;
		default:
			Pt( "Decode Type: NONE\r\n"); break;
	}
	Pt( "Graphical Center: ( %d, %d )\r\n",res->GraphicalCenter.x,res->GraphicalCenter.y);
	Pt( "Bounds (main):\r\n" );
	Pt( "  (0) %d, %d\r\n",res->MainBounds.corners[0].x,res->MainBounds.corners[0].y );
	Pt( "  (1) %d, %d\r\n",res->MainBounds.corners[1].x,res->MainBounds.corners[1].y );
	Pt( "  (2) %d, %d\r\n",res->MainBounds.corners[2].x,res->MainBounds.corners[2].y );
	Pt( "  (3) %d, %d\r\n",res->MainBounds.corners[3].x,res->MainBounds.corners[3].y );
	Pt( "IQ Image Info:\r\n" );
	Pt( "  (0) %d, %d\r\n",res->IQImageInfo.bounds.corners[0].x,res->IQImageInfo.bounds.corners[0].y );
	Pt( "  (1) %d, %d\r\n",res->IQImageInfo.bounds.corners[1].x,res->IQImageInfo.bounds.corners[1].y );
	Pt( "  (2) %d, %d\r\n",res->IQImageInfo.bounds.corners[2].x,res->IQImageInfo.bounds.corners[2].y );
	Pt( "  (3) %d, %d\r\n",res->IQImageInfo.bounds.corners[3].x,res->IQImageInfo.bounds.corners[3].y );
	Pt( "  d_corner: %d\r\n",res->IQImageInfo.d_corner );
	Pt( "  pdfRows: %d\r\n",res->IQImageInfo.pdfRows );
	Pt( "  pdfCols: %d\r\n",res->IQImageInfo.pdfCols );
	Pt( "___________________\r\n\r\n" );
}


static int CheckAppendType( DecoderResult *res ) {
	int testLen = 0;
	//this info is the very basic info needed for letting the system know how many barcodes there are
	//could put this in the FillResultInfo(), but it doesn't really matter since this is always called
	res->AppendType = NO_APPEND;
	
	//So far this only applies to QR Append (based on swift manual). 
	//If it is an append, then these values are already stored.
	//Otherwise, set up values.

	if( res->Parity < 256 ) { 
		res->AppendIndex = storedResultsIndex;
		res->AppendTotal = storedResultsIndex+1;
	}

	switch( res->Symbology ) {
		case DEC_SYMBOLOGY_CODE39:
			switch(res->LinkFlag) {
				case DEC_LINKED_C39:

					if( TLC39Enabled ) {
						Pt("Linked c39\r\n");
						finishingAppendType = UCC_COMPOSITE_APPEND;
						res->AppendType = UCC_COMPOSITE_APPEND;
						res->AppendIndex = 0;
						res->AppendTotal = 2;
						res->AIMCodeLetter	= DEC_AIM_CODELETTER_TLC39;
						res->AIMModifier	= '2';
						res->HHPCodeID		= DEC_HHP_CODE_ID_TLC39;
						if( DECTIMER ) {
							if( timeStamp == 0 ) { //if this is the first occurance of this
								timeStamp = DECTIMER();
							}
							additionalSearchTime = 99999;
						}
					}
					else if( finishingAppendType != NO_APPEND ) {
						return DEC_FAILURE;
					}
					else {
						finishingAppendType = APPEND_FINISHED;
					}
					break;
				
				case DEC_NOT_LINKED:
					if( Code39AppendEnabled ) {			//if appending is enabled
						if( res->Data[0] == ' ' ) {
							DeleteCharacter(res,0); //remove space from the first position
							res->AppendTotal = 2; //we're combining them as we go, so this will always be 2 until the very end
							res->AppendType = CODE39_APPEND;

							if( finishingAppendType == NO_APPEND ) {  //if this is our first code
								res->AppendIndex = 0; //the first one needs to be 0 so when the others are combined, they are combined in the correct order
							}
							else if( finishingAppendType == CODE39_APPEND ) {
								if( (tempResults[0].Length + res->Length) >= MAX_DECODED_DATA_LENGTH && DECNOTIFY ) { //if this code pushes over the edge
									DECNOTIFY(1); //notify the append handler of an error
									return DEC_HALT; //stops the storing process and will clear stored results
								}
								
								//if this code will fit into the buffer, go and add it
								res->AppendIndex = 1;
								CombineResults(1);
								storedResultsIndex--; //subtract 1 from the index to keep it at 1. We need to allow it to go to 1 to say we're looking for more pieces
							}

							finishingAppendType = CODE39_APPEND;
						}
						else if( finishingAppendType == CODE39_APPEND ) { //we had a previous result and this is a finishing code 39
							
							if( (tempResults[0].Length + res->Length) >= MAX_DECODED_DATA_LENGTH && DECNOTIFY ) { //if this code pushes over the edge
								DECNOTIFY(1); //notify the append handler of an error
								return DEC_HALT; //stops the storing process and will clear stored results
							}
							res->AppendIndex = 1;
							res->AppendTotal = 2;
							finishingAppendType = APPEND_FINISHED;
							CombineResults(1);
						}
						else if( finishingAppendType != NO_APPEND ) {
							return DEC_FAILURE;
						}
						else {
							finishingAppendType = APPEND_FINISHED;
						}
					}
					else if( finishingAppendType != NO_APPEND ) {	//check if we already had an append going
						return DEC_FAILURE;							//if so, fail
					}
					else {
						finishingAppendType = APPEND_FINISHED;
					}
					break; //DEC_NOT_LINKED
			}//switch(LinkFlag)
			break; //Code 39

		case DEC_SYMBOLOGY_CODE93:
			if( Code93AppendEnabled && res->Data[0] == ' ' && (finishingAppendType == CODE93_APPEND || finishingAppendType == NO_APPEND) ) {		//this is a code to append
				
				DeleteCharacter(res,0); //remove space from the first position
				res->AppendTotal = 2; //we're combining them as we go, so this will always be 2 until the very end
				res->AppendType = CODE93_APPEND;

				if( finishingAppendType == NO_APPEND ) {  //if this is our first code
					res->AppendIndex = 0; //the first one needs to be 0 so when the others are combined, they are combined in the correct order
				}
				else if( finishingAppendType == CODE93_APPEND ) {
					if( (tempResults[0].Length + res->Length) >= MAX_DECODED_DATA_LENGTH && DECNOTIFY ) { //if this code pushes over the edge
						DECNOTIFY(1); //notify the append handler of an error
						return DEC_HALT; //stops the storing process and will clear stored results
					}
								
					//if this code will fit into the buffer, go and add it
					res->AppendIndex = 1;
					CombineResults(1);
					storedResultsIndex--; //subtract 1 from the index to keep it at 1. We need to allow it to go to 1 to say we're looking for more pieces
				}

				finishingAppendType = CODE93_APPEND;
			}
			else if( finishingAppendType == CODE93_APPEND ) { //we had a previous result and this is a finishing code 93
				if( (tempResults[0].Length + res->Length) >= MAX_DECODED_DATA_LENGTH && DECNOTIFY ) { //if this code pushes over the edge
					DECNOTIFY(1); //notify the append handler of an error
					return DEC_HALT; //stops the storing process and will clear stored results
				}
				res->AppendIndex = 1;
				res->AppendTotal = 2;
				finishingAppendType = APPEND_FINISHED;
				CombineResults(1);
			}
			else if( finishingAppendType != NO_APPEND ) {
				return DEC_FAILURE;
			}
			else {
				finishingAppendType = APPEND_FINISHED;
			}
			break;
		case DEC_SYMBOLOGY_PDF:
			switch(res->LinkFlag) {
				case DEC_LINKED_UPDF:
				case DEC_LINKED_CC_A:
				case DEC_LINKED_CC_B:
				case DEC_LINKED_CC_C:
					Pt("linked 2D\r\n");
						finishingAppendType = APPEND_FINISHED;
						res->AppendType = UCC_COMPOSITE_APPEND;
						res->AppendIndex = 1;
						res->AppendTotal = 2;
						res->AIMCodeLetter	= DEC_AIM_CODELETTER_COMPOSITE;
						res->AIMModifier	= '0';

						if( tempResults[storedResultsIndex-1].Symbology == DEC_SYMBOLOGY_CODE39 ) {
							CombineResults(storedResultsIndex);
							ConvertTLCMessage(&tempResults[0]);
						}
						testLen = (EANEmulationMode)?0:tempResults[1].Length;
						testLen += tempResults[0].Length;

						if( testLen < CompositeMin || testLen > CompositeMax ) {
							return DEC_HALT;
						}
						break;
				case DEC_NOT_LINKED:
					if( res->Data[res->Length-4] == '\\' ) {
						int jk = res->Length-4; //index to create values for parsing
	
						//parse values from the end of the code.
						//Codewords are shown in the format "\XXX". 
						while( jk >= 4 && res->Data[jk] == '\\' ) {
							PDFCodewords[PDFCodewordIndex] += (res->Data[jk+1]-'0') * 100;
							PDFCodewords[PDFCodewordIndex] += (res->Data[jk+2]-'0') * 10;
							PDFCodewords[PDFCodewordIndex] += (res->Data[jk+3]-'0');
							jk-=4;
							Pt("MacroVal[%d]=%d\r\n",PDFCodewordIndex,PDFCodewords[PDFCodewordIndex]);
							PDFCodewordIndex++;
						}

						if( MacroPDFEnabled ) {
							if( PDFCodewords[PDFCodewordIndex-1] == 928 ) {
								int mpdfID = 0; //ID to determine the ID.
								
								Pt("Macro PDF!!!\r\n");
								res->Length -= PDFCodewordIndex*4; //remove the codes
	
								//build the ID, which is a 2 codeword value
								mpdfID = PDFCodewords[PDFCodewordIndex-2]*1000;
								mpdfID += PDFCodewords[PDFCodewordIndex-3];

								//test to make sure we don't already have this piece
								for( jk = 0; jk < storedResultsIndex; jk++ ) {
									if( tempResults[jk].AppendIndex == mpdfID ) {
										Pt("already have this piece!\r\n");
										ClearPDFCodewords();
										return DEC_SKIP; 
									}
								}

								//index and total will be adjusted when the terminator is scanned
								res->AppendIndex = mpdfID;
								res->AppendTotal = 1; 

								//find the lowest index, Since they can be scanned in any order, this is needed for 
								//creating the correct indexes once the terminator is scanned
								if(	MacroPDFZero == 0 || mpdfID < MacroPDFZero ) {
									MacroPDFZero = mpdfID;
									Pt("NEW zero:%d\r\n",MacroPDFZero);
								}
								if( MacroPDFMax == 0 || mpdfID > MacroPDFMax ) {
									MacroPDFMax = mpdfID;
									Pt("NEW max:%d\r\n",MacroPDFMax);
								}
								if( PDFCodewords[0] == 922) { //if we have a terminator
									//NOTE: Right now we will only be able to handle 10 codes and 8K of data total
									finishingAppendType = APPEND_FINISHED;

									//This assumed that the terminator is scanned last. 
									//There is no mention that the terminator is scanned last in the spec, but this is 
									//how it will work. 
									Pt("zero:%d\r\n",MacroPDFZero);
									for(jk=0; jk<=storedResultsIndex; jk++ ) {
										tempResults[jk].AppendIndex = tempResults[jk].AppendIndex - MacroPDFZero;
										tempResults[jk].AppendTotal = (MacroPDFMax-MacroPDFZero)+1; 
	
										Pt("res[%d].Index = %d\r\n",jk,tempResults[jk].AppendIndex);
										Pt("res[%d].Total = %d\r\n",jk,tempResults[jk].AppendTotal);
									}
								}
								else { //just another piece of the macro block
									finishingAppendType = MACROPDF_APPEND;
								}

								res->AppendType = MACROPDF_APPEND;

							}
							else if( finishingAppendType != NO_APPEND ) {	//check if we already had an append going
								return DEC_FAILURE;							//if so, fail
							}
							else {
								if( ECIHandling == 2 ) {
									res->Length -= PDFCodewordIndex*4; //remove the codes 
								}
								finishingAppendType = APPEND_FINISHED;
							}
						}
						else if( finishingAppendType != NO_APPEND ) {	//check if we already had an append going
							return DEC_FAILURE;							//if so, fail
						}		
						else {
							if( ECIHandling == 2 ) {
								res->Length -= PDFCodewordIndex*4; //remove the codes
							}
							finishingAppendType = APPEND_FINISHED;
						}
					}//no codewords trailing the data
					else if( finishingAppendType != NO_APPEND ) {	//check if we already had an append going
						return DEC_FAILURE;							//if so, fail
					}	
					else {
						finishingAppendType = APPEND_FINISHED;
					}
					break;//link flag == NO
				}
				break; //PDF

		case DEC_SYMBOLOGY_CODE128:
		case DEC_SYMBOLOGY_RSS:
		case DEC_SYMBOLOGY_RSS_14:
		case DEC_SYMBOLOGY_RSS_14_ST:
		case DEC_SYMBOLOGY_RSS_14_LIM:
		case DEC_SYMBOLOGY_RSS_EXP:
		case DEC_SYMBOLOGY_RSS_EXP_ST:
			switch(res->LinkFlag) {
				case DEC_LINKED_C128:
				case DEC_LINKED_RSS:
					if( finishingAppendType != NO_APPEND ) {
						return DEC_FAILURE;
					}
					else if( CompositeEnabled ) {
						finishingAppendType = UCC_COMPOSITE_APPEND;
						res->AppendType = UCC_COMPOSITE_APPEND;
						res->AppendIndex = 0;
						res->AppendTotal = 2;
						break; 
					}

				case DEC_NOT_LINKED:

					if( CouponCodesEnabled &&
						(res->Symbology == DEC_SYMBOLOGY_CODE128 && !strncmp((char*)res->Data,"810",3) || 
						!strncmp((char*)res->Data,"8110",4))) { //this is part of a coupon
							res->AppendType = COUPON_CODE_APPEND;
							res->AppendIndex = 1;
							res->AppendTotal = 2;
	
							//check if we have the first piece already
							if( storedResultsIndex > 0 ) {
								if( finishingAppendType == COUPON_CODE_APPEND && 
									tempResults[storedResultsIndex-1].Symbology == DEC_SYMBOLOGY_UPC ) {

									if( CouponSecondaryOutput && res->Symbology != DEC_SYMBOLOGY_CODE128 ) {
										return DEC_FAILURE; //by returning fail, the Store will try again, pushing us to the next section, as if it was the first code found.
									}
	
									finishingAppendType = APPEND_FINISHED; //found both pieces, we're done
								}
								else { // this was some other kind of append, fail
									return DEC_FAILURE;
								}
							}
							else { //this is the first code we've found
								if( CouponSecondaryOutput && res->Symbology != DEC_SYMBOLOGY_CODE128 ) {
									res->AppendType = COUPON_CODE_APPEND;
									res->AppendIndex = 0;
									res->AppendTotal = 1;
									finishingAppendType = APPEND_FINISHED;
								}
								else {
									finishingAppendType = COUPON_CODE_APPEND;

									if( DECTIMER ) {
										if( timeStamp == 0 ) { //if this is the first occurance of this
											timeStamp = DECTIMER();
										}
										if( CouponCodesEnabled == 1 ) {
											additionalSearchTime = ConcatAdditionalSearchTime;
										}
									}
								}
							}				
					} //CouponCodes
					else if( Code128AppendEnabled && res->AIMModifier == '4' ) {
						res->AppendTotal = 2; //we're combining them as we go, so this will always be 2 until the very end
						res->AppendType = CODE128_APPEND;

						if( finishingAppendType == NO_APPEND ) {  //if this is our first code
							res->AppendIndex = 0; //the first one needs to be 0 so when the others are combined, they are combined in the correct order
						}
						else if( finishingAppendType == CODE128_APPEND ) {
							if( (tempResults[0].Length + res->Length) >= MAX_DECODED_DATA_LENGTH && DECNOTIFY ) { //if this code pushes over the edge
								DECNOTIFY(1); //notify the append handler of an error
								return DEC_HALT; //stops the storing process and will clear stored results
							}
								
							//if this code will fit into the buffer, go and add it
							res->AppendIndex = 1;
							CombineResults(1);
							storedResultsIndex--; //subtract 1 from the index to keep it at 1. We need to allow it to go to 1 to say we're looking for more pieces
						}
						else {
							return DEC_FAILURE;
						}

						finishingAppendType = CODE128_APPEND;
					}
					//ISBT concatentation. 
					//This is slightly more restricted than Adaptus.
					//There are 4 types of codes and 2 "pairs" that are meant to be pairs.
					//This code will allow either of the left hand side codes and either of the
					//right hand side codes. Adaptus would allow any combination of the 4
					else if( res->Data[0] == '=' || res->Data[0] == '&' )  {
						if( res->Data[1] == '<' || res->Data[1] == 'W' ) {
							res->AppendIndex = 0;
							res->HHPCodeID = DEC_HHP_CODE_ID_CODE128;
							res->AIMCodeLetter = DEC_AIM_CODELETTER_CODE128;

							if( !ISBTEnabled ) {
								if( Code128EnableTemp ) {
									finishingAppendType = APPEND_FINISHED;
									res->AppendType = APPEND_FINISHED;
									res->AppendTotal = 1;
									res->AIMModifier = '0';
									return DEC_SUCCESS; //only return 1 side
								}
								else {
									return DEC_HALT;
								}
							}
							
							res->AppendType = ISBT_APPEND;
							res->AppendTotal = 2;
							res->AIMModifier = '4';

							if( storedResultsIndex == 1 && (tempResults[0].Data[1] == '>' || tempResults[0].Data[1] == '%') ) {
								if( tempResults[0].AppendType == ISBT_APPEND ) {
									finishingAppendType = APPEND_FINISHED;
									storedResultsIndex++;
									SortResults(); //SortResults() is meant to be used after a result is stored, so pretend to store another result to sort the current ones.
									storedResultsIndex--;
									CombineResults(storedResultsIndex);
								}
								else {
									return DEC_FAILURE;
								}
							}
							else if( storedResultsIndex == 0 ) {
								finishingAppendType = ISBT_APPEND;
							}
							else {
								return DEC_FAILURE;
							}
						}
						else if( res->Data[1] == '>' || res->Data[1] == '%' ) {
							res->HHPCodeID = DEC_HHP_CODE_ID_CODE128;
							res->AIMCodeLetter = DEC_AIM_CODELETTER_CODE128;

							if( !ISBTEnabled ) {
								if( Code128EnableTemp ) {
									finishingAppendType = APPEND_FINISHED;
									res->AppendType = APPEND_FINISHED;
									res->AppendIndex = 0;
									res->AppendTotal = 1;
									res->AIMModifier = '0';
									return DEC_SUCCESS; //only return 1 side
								}
								else {
									return DEC_HALT;
								}
							}

							res->AppendType = ISBT_APPEND;
							res->AppendIndex = 1;
							res->AppendTotal = 2;
							res->AIMModifier = '4';

							if( storedResultsIndex == 1 && (tempResults[0].Data[1] == '<' || tempResults[0].Data[1] == 'W') ) {
								if( tempResults[0].AppendType == ISBT_APPEND ) {
									finishingAppendType = APPEND_FINISHED;
									storedResultsIndex++;
									SortResults(); //SortResults() is meant to be used after a result is stored, so pretend to store another result to sort the current ones.
									storedResultsIndex--;
									CombineResults(storedResultsIndex);
								}
								else {
									return DEC_FAILURE;
								}
							}
							else if( storedResultsIndex == 0 ) {
								finishingAppendType = ISBT_APPEND;
							}
							else {
								return DEC_FAILURE;
							}
						}
						else {
							finishingAppendType = APPEND_FINISHED;
						}
					}
					else if( res->Symbology == DEC_SYMBOLOGY_CODE128 && finishingAppendType == CODE128_APPEND ) { 
						if( (tempResults[0].Length + res->Length) >= MAX_DECODED_DATA_LENGTH && DECNOTIFY ) { //if this code pushes over the edge
							DECNOTIFY(1); //notify the append handler of an error
							return DEC_HALT; //stops the storing process and will clear stored results
						}
						res->AppendIndex = 1;
						res->AppendTotal = 2;
						finishingAppendType = APPEND_FINISHED;
						CombineResults(1);
					}
					//regular code 128 or RSSExpanded - no appending
					else if( finishingAppendType != NO_APPEND ) {	//check if we already had an append going
						return DEC_FAILURE;							//if so, fail
					}
					else if( ISBTEnabled && res->Symbology == DEC_SYMBOLOGY_CODE128 && 
							!res->ProgrammingCode && //not a programming code
							res->AIMModifier != '1' ) { //not a GS1

						if( !Code128EnableTemp ) { //only care if ISBT is enabled
							return DEC_HALT;
						}
					else {
						finishingAppendType = APPEND_FINISHED;
					}
					}
					else {
						finishingAppendType = APPEND_FINISHED;
					}
					break; //DEC_NOT_LINKED
			}//switch(LinkFlag)
			break;

		case DEC_SYMBOLOGY_UPC:
			if( CouponCodesEnabled && (
				(res->Modifier == 'A' && res->Data[0] == '5') ||						//UPC-A part of the coupon
				(res->Modifier == 'E' && res->Data[0] == '9' && res->Data[1] == '9'))) { //EAN13 part of the coupon
					res->AppendType = COUPON_CODE_APPEND;
					res->AppendIndex = 0;
					res->AppendTotal = 2;

					if( storedResultsIndex > 0 ) {
						if( finishingAppendType == COUPON_CODE_APPEND && 
							(tempResults[storedResultsIndex-1].Symbology == DEC_SYMBOLOGY_CODE128 ||
							 tempResults[storedResultsIndex-1].Symbology == DEC_SYMBOLOGY_RSS_EXP ||
							 tempResults[storedResultsIndex-1].Symbology == DEC_SYMBOLOGY_RSS_EXP_ST) ) {
								
								finishingAppendType = APPEND_FINISHED;
						}
						else { //was some other kind of append, fail
							return DEC_FAILURE;
						}
					}
					else { // this was the first code we've found
						finishingAppendType = COUPON_CODE_APPEND;
						if( DECTIMER ) {
							if( timeStamp == 0 ) { //if this is the first occurance of this
								timeStamp = DECTIMER();
							}
							if( CouponCodesEnabled == 1 ) {
								additionalSearchTime = ConcatAdditionalSearchTime;
							}
						}
					}
			}
			else if( UPCCompositeEnabled ) {
				if( finishingAppendType != NO_APPEND ) {
					return DEC_FAILURE;
				}

				finishingAppendType = UPC_COMPOSITE_APPEND;
				res->AppendType = UPC_COMPOSITE_APPEND;
				res->AppendIndex = 0;
				res->AppendTotal = 2;
				if( DECTIMER ) {
					if( timeStamp == 0 ) { //if this is the first occurance of this
						timeStamp = DECTIMER();
					}
					additionalSearchTime = UPCCompositeAdditionalSearchTime;
				}
			}
			//this is a regular UPC of some type
			else if( finishingAppendType != NO_APPEND ) {
				return DEC_FAILURE;
			}
			else { 
				finishingAppendType = APPEND_FINISHED;
			}
			break;
		case DEC_SYMBOLOGY_CODABAR:
			if( CodabarAppendEnabled && (res->Data[0] == 'D' || res->Data[res->Length-1] == 'D') ) {

				res->AppendType = CODABAR_APPEND;
				res->AppendTotal = 2;
				res->AIMModifier = '1';

				if( res->Data[0] == 'D' && res->Data[res->Length-1] != 'D' ) {
					res->AppendIndex = 1;
					finishingAppendType = CODABAR_APPEND;
					CodabarAppendStartStop |= 1;
				}
				else if( res->Data[0] != 'D' && res->Data[res->Length-1] == 'D' ) {
					res->AppendIndex = 0;
					finishingAppendType = CODABAR_APPEND;
					CodabarAppendStartStop |= 2;
				}
				else if( res->Data[0] == 'D' && res->Data[res->Length-1] == 'D' ) {
					if( storedResultsIndex ) {
						if( tempResults[0].AppendIndex == 0 ) {
							res->AppendIndex = 1;
						}
						else {
							res->AppendIndex = 0;
						}
					}
					else {
						 return DEC_HALT;
					}
				}

				//test if we have all the pieces
				if( finishingAppendType == CODABAR_APPEND && CodabarAppendStartStop == 3) {
					finishingAppendType = APPEND_FINISHED;
					CodabarAppendStartStop = 0;

					storedResultsIndex++;
					SortResults(); //SortResults() is meant to be used after a result is stored, so pretend to store another result to sort the current ones.
					storedResultsIndex--;

					tempResults[0].Length--;			//delete 'D'
					DeleteCharacter(&(tempResults[1]),0);	//delete 'D'
					CombineResults(storedResultsIndex);

					if( !CodabarStartStopTransmit ) {
						tempResults[0].Length--;
						DeleteCharacter(&(tempResults[0]),0);
					}

					return DEC_SUCCESS;
				}

				//Should only get here if there is less than the number of pieces. 
				//Set additional search time, if any
				if( CodabarAppendRequired ) {
					additionalSearchTime = 0;
				}
				else if( DECTIMER ) {
					if( timeStamp == 0 ) { //if we didn't have a timestamp before
						timeStamp = DECTIMER();
					}
					additionalSearchTime = ConcatAdditionalSearchTime;
				}
				return DEC_SUCCESS; //need a return here for appending
			}

			//This condition should only hit when append is disabled or this results is not an append result
			if( finishingAppendType != NO_APPEND ) {	//if we were expecting an append,
				return DEC_FAILURE;						//fail so we clear the stored stuff and re-store this code
			}
			else { //this is a single barcode with no appending, have to put some info here to let the outside know how many codes are here
				if( !CodabarStartStopTransmit ) {
					DeleteCharacter(res,0);
					res->Length--;
				}
				finishingAppendType = APPEND_FINISHED;
			}
			break;
		case DEC_SYMBOLOGY_AZTEC_CODE:
			if( res->Modifier >= '6' && res->Modifier <= 'B' ) { //AIM spec for a structured append
				int jk = 0;
#if (DECODER_ID)
				if( storedResultsIndex == 0 && SI_OptimizeEnables == 0 ) {
					StopTheDecoder = 1;
					SI_OptimizeEnables = AZTEC_APPEND;
				
					return DEC_HALT;
				}
#endif

				if( res->Data[0] == ' ' ) { //the ID comes first, if there is one
					int jkID = 0;
					jk++;
					if( appendID[0] == 0 ) { //if we don't have a stored ID yet
						while( res->Data[jk] != ' ' && jk < res->Length ) { //there can be no spaces inside the ID
							appendID[jkID++] = res->Data[jk++];
						}
						jk++;
					}
					else if( !strncmp((const char *)appendID,(const char *)&res->Data[1],strlen(appendID)) ) { //if the IDs don't match
						jk = strlen(appendID) + 2; //compendate for ID and 2 spaces
					}
					else {
						return DEC_FAILURE;
					}
				}
				else if( appendID[0] != 0 ) { //append with no ID, but we have an ID already, 
					return DEC_FAILURE;
				}

				res->AppendIndex = res->Data[jk++] - 'A';		//M of
				res->AppendTotal = res->Data[jk++] - 'A' + 1;	//N, +1 gives us the total pieces, because N = index
				
				//if there is no ID, check to see if there are other pieces. If there are, check to see if there
				//are any that have the current index or the total is mis-matched, fail it and start over
				if( storedResultsIndex > 0  ) {
					for( int jks = 0; jks<storedResultsIndex; jks++ ) {
						if( tempResults[jks].AppendTotal != res->AppendTotal ||
							tempResults[jks].AppendIndex == res->AppendIndex ) {
								return DEC_FAILURE;
						}
					}
				}

				if( AztecAppendStripInfo ) {
					res->Length -= jk;	//Adjust the length based on what we did for the ID and M/N
				
					//copy the rest of the data to a temp string
					char *tmpStr = (char *)malloc(res->Length);
					strncpy(tmpStr,(const char *)&res->Data[jk],res->Length);
					tmpStr[res->Length] = 0;
				
					//copy it back to the other string
					strncpy((char *)res->Data,tmpStr,res->Length);
					res->Data[res->Length] = 0;
					free(tmpStr);
				}

				if( AztecAppendEnabled && storedResultsIndex == res->AppendTotal-1 ) { //if we've reached the number of expected results
					finishingAppendType = APPEND_FINISHED;
				}
				else { //need more pieces
					finishingAppendType = AZTEC_APPEND;
				}
			}
			else if( finishingAppendType != NO_APPEND ) {
				return DEC_FAILURE;
			}
			else {
				finishingAppendType = APPEND_FINISHED;
			}
			break;

		case DEC_SYMBOLOGY_KIX:
			if( res->ModifierEx ) {
				res->AppendIndex = 0;
				res->AppendTotal = 2;
				res->AppendType = KIX_REVERSE_CODE;
				finishingAppendType = KIX_REVERSE_CODE;	
			}
			else if( storedResultsIndex == 1 && finishingAppendType == KIX_REVERSE_CODE ) {
				int corner1 = tempResults[0].MainBounds.corners[0].x+tempResults[0].MainBounds.corners[0].y;
				int corner2 = res->MainBounds.corners[0].x+res->MainBounds.corners[0].y;

				res->AppendType = KIX_REVERSE_CODE;
				finishingAppendType = APPEND_FINISHED;

				switch( ImageRotation ) {
					case 0: //take closest to 0,0
					case 1:
					default:
						if( corner1 <= corner2 ) {
							res->AppendIndex = 1;
							tempResults[0].AppendIndex = 0;
						}
						else {
							res->AppendIndex = 0;
							tempResults[0].AppendIndex = 1;
						}
						res->ModifierEx = 0; //makes sure we issue a result
						break;
					case 2: 
					case 3:
						if( corner1 > corner2 ) {
							res->AppendIndex = 1;
							tempResults[0].AppendIndex = 0;
						}
						else {
							res->AppendIndex = 0;
							tempResults[0].AppendIndex = 1;
						}
						res->ModifierEx = 0; //makes sure we issue a result
						break;
				}
			}
			else if( finishingAppendType != NO_APPEND ) {
				return DEC_FAILURE;						
			}
			else { 
				finishingAppendType = APPEND_FINISHED;
			}
			break;
		case DEC_SYMBOLOGY_QRCODE:
			if( QRAppendEnabled && res->Parity > 255 ) {
#if (DECODER_ID)
				if( storedResultsIndex == 0 && SI_OptimizeEnables == 0 ) {
					StopTheDecoder = 1;
					SI_OptimizeEnables = QR_APPEND;
				
					return DEC_HALT;
				}
#endif
				res->Parity -= 256; //parity+256 = append
				finishingAppendType = QR_APPEND;
				res->AppendType = QR_APPEND;
				res->AppendIndex--; //our index goes from 0-n while what comes out of ID is 1-n

				if( storedResultsIndex+1 == res->AppendTotal ) { //have all the pieces
					finishingAppendType = APPEND_FINISHED;
				}
			}
			else if( finishingAppendType != NO_APPEND ) {
				return DEC_FAILURE;
			}
			else {
				if( res->Parity > 256 ) {
					res->Parity -= 256; //if this was an append, but append wasn't enabled, we will want to read as a single code and preserve the parity
				}
				finishingAppendType = APPEND_FINISHED;
			}
			break;
		case 0:
			switch( res->SymbologyEx ) {
				case DEC_SYMBOLOGY_EX_OCR:
					if( finishingAppendType == NO_APPEND || finishingAppendType == OCR_APPEND || finishingAppendType == OCR_USER_APPEND ) {

						switch( res->ModifierEx ) {
							case 2: 
								Pt("Passport - line 1\r\n");
								finishingAppendType = OCR_APPEND;
								res->AppendType = OCR_APPEND;
								res->AppendIndex = 0;
								res->AppendTotal = 2;
								if( storedResultsIndex > 0 ) {
									if( tempResults[storedResultsIndex-1].ModifierEx == 1 ||
										tempResults[storedResultsIndex-1].ModifierEx == 7 ) { 
										finishingAppendType = APPEND_FINISHED;
									}
									else { //otherwise, it's not part of a passport and start over
										return DEC_FAILURE;
									}
								}
								break;
							case 1: 
								Pt("Passport - line 2\r\n");
								finishingAppendType = OCR_APPEND;
								res->AppendType = OCR_APPEND;
								res->AppendIndex = 1;
								res->AppendTotal = 2;

								if( storedResultsIndex > 0 ) {
									if( tempResults[storedResultsIndex-1].ModifierEx == 2 || 
										tempResults[storedResultsIndex-1].ModifierEx == 6 ) {
										finishingAppendType = APPEND_FINISHED;
									}
									else { //otherwise, it's not part of a passport and start over
										return DEC_FAILURE;
									}
								}
								break;
							case 6:
								Pt("Format A Visa - line 1\r\n");
								finishingAppendType = OCR_APPEND;
								res->AppendType = OCR_APPEND;
								res->AppendIndex = 0;
								res->AppendTotal = 2;

								if( storedResultsIndex > 0 ) {
									if( tempResults[storedResultsIndex-1].ModifierEx == 7 ||
										tempResults[storedResultsIndex-1].ModifierEx == 1 ) { 
										finishingAppendType = APPEND_FINISHED;
									}
									else {
										return DEC_FAILURE;
									}
								}
								break;
							case 7:
								Pt("Format A Visa - line 2\r\n");
								finishingAppendType = OCR_APPEND;
								res->AppendType = OCR_APPEND;
								res->AppendIndex = 1;
								res->AppendTotal = 2;

								if( storedResultsIndex > 0 ) {
									if( tempResults[storedResultsIndex-1].ModifierEx == 6 ||
										tempResults[storedResultsIndex-1].ModifierEx == 2 ) {
										finishingAppendType = APPEND_FINISHED;
									}
									else {
										return DEC_FAILURE;
									}
								}
								break;
							case 8:
								Pt("Format B Visa - line 1\r\n");
								finishingAppendType = OCR_APPEND;
								res->AppendType = OCR_APPEND;
								res->AppendIndex = 0;
								res->AppendTotal = 2;

								if( storedResultsIndex > 0 ) {
									if( tempResults[storedResultsIndex-1].ModifierEx == 9 ||
										tempResults[storedResultsIndex-1].ModifierEx == 11 ) {
										finishingAppendType = APPEND_FINISHED;
									}
									else {
										return DEC_FAILURE;
									}
								}
								break;
							case 9:
								Pt("Format B Visa - line 2\r\n");
								finishingAppendType = OCR_APPEND;
								res->AppendType = OCR_APPEND;
								res->AppendIndex = 1;
								res->AppendTotal = 2;

								if( storedResultsIndex > 0 ) {
									if( tempResults[storedResultsIndex-1].ModifierEx == 8 ||
										tempResults[storedResultsIndex-1].ModifierEx == 10 ) {
										finishingAppendType = APPEND_FINISHED;
									}
									else {
										return DEC_FAILURE;
									}
								}
								break;
							case 12:
								Pt("TD-1 travel doc - line 1\r\n");
								finishingAppendType = OCR_APPEND;
								res->AppendType = OCR_APPEND;
								res->AppendIndex = 0;
								res->AppendTotal = 3;

								if( storedResultsIndex == 1 ) {
									if( tempResults[0].ModifierEx != 13 && tempResults[0].ModifierEx != 14 ) {
										return DEC_FAILURE;
									}
								}
								else if( storedResultsIndex == 2 ) {
									if( tempResults[0].ModifierEx == 13 ) {
										if( tempResults[1].ModifierEx == 14 ) {
											finishingAppendType = APPEND_FINISHED;
										}
										else {
											return DEC_FAILURE;
										}
									}
									else if( tempResults[0].ModifierEx == 14 ) {
										if( tempResults[1].ModifierEx == 13 ) {
											finishingAppendType = APPEND_FINISHED;
										}
										else {
											return DEC_FAILURE;
										}
									}
									else {
										return DEC_FAILURE;
									}
								}
								break;
							case 13:
								Pt("TD-1 travel doc - line 2\r\n");
								finishingAppendType = OCR_APPEND;
								res->AppendType = OCR_APPEND;
								res->AppendIndex = 1;
								res->AppendTotal = 3;

								if( storedResultsIndex == 1 ) {
									if( tempResults[0].ModifierEx != 12 && tempResults[0].ModifierEx != 14 ) {
										return DEC_FAILURE;
									}
								}
								else if( storedResultsIndex == 2 ) {
									if( tempResults[0].ModifierEx == 12 ) {
										if( tempResults[1].ModifierEx == 14 ) {
											finishingAppendType = APPEND_FINISHED;
										}
										else {
											return DEC_FAILURE;
										}
									}
									else if( tempResults[0].ModifierEx == 14 ) {
										if( tempResults[1].ModifierEx == 12 ) {
											finishingAppendType = APPEND_FINISHED;
										}
										else {
											return DEC_FAILURE;
										}
									}
									else {
										return DEC_FAILURE;
									}
								}
								break;
							case 14:
								Pt("TD-1 travel doc - line 3\r\n");
								finishingAppendType = OCR_APPEND;
								res->AppendType = OCR_APPEND;
								res->AppendIndex = 2;
								res->AppendTotal = 3;

								if( storedResultsIndex == 1 ) {
									if( tempResults[0].ModifierEx != 13 && tempResults[0].ModifierEx != 12 ) {
										return DEC_FAILURE;
									}
								}
								else if( storedResultsIndex == 2 ) {
									if( tempResults[0].ModifierEx == 13 ) {
										if( tempResults[1].ModifierEx == 12 ) {
											finishingAppendType = APPEND_FINISHED;
										}
										else {
											return DEC_FAILURE;
										}
									}
									else if( tempResults[0].ModifierEx == 12 ) {
										if( tempResults[1].ModifierEx == 13 ) {
											finishingAppendType = APPEND_FINISHED;
										}
										else {
											return DEC_FAILURE;
										}
									}
									else {
										return DEC_FAILURE;
									}
								}
								break;
							case 10:
								Pt("TD-2 travel doc - line 1\r\n");
								finishingAppendType = OCR_APPEND;
								res->AppendType = OCR_APPEND;
								res->AppendIndex = 0;
								res->AppendTotal = 2;

								if( storedResultsIndex > 0 ) {
									if( tempResults[storedResultsIndex-1].ModifierEx == 11 ||
										tempResults[storedResultsIndex-1].ModifierEx == 9 ) {
										finishingAppendType = APPEND_FINISHED;
									}
									else {
										return DEC_FAILURE;
									}
								}
								break;
							case 11:
								Pt("TD-2 travel doc - line 2\r\n");
								finishingAppendType = OCR_APPEND;
								res->AppendType = OCR_APPEND;
								res->AppendIndex = 1;
								res->AppendTotal = 2;

								if( storedResultsIndex > 0 ) {
									if( tempResults[storedResultsIndex-1].ModifierEx == 10 ||
										tempResults[storedResultsIndex-1].ModifierEx == 8 ) {
										finishingAppendType = APPEND_FINISHED;
									}
									else {
										return DEC_FAILURE;
									}
								}
								break;
							default: //standard OCR read
#if (OCR_SEPARATE)
								Pt("Standard OCR\r\n");
								if( OCRSeparateTemplate && OCRActiveTemplates == OCR_USER_TEMPLATE ) {
									//check to see if this result matches one of the templates
									int ocrTemps[OCR_MAX_TEMPLATES];
									int ocrRes = CheckOCRResult( res, &ocrTemps[0] );
									
									//if there was a match
									if( ocrRes ) {
										OCRTemplate *ocrt = &OCRTemplates[ocrTemps[0]]; 
										
										//if we already have results
										if( storedResultsIndex ) {
											int jj, grpFound;
											for( jj = 0, grpFound = 0; jj < ocrRes; jj++ ) {
												//if the group of one of the matches is the same as one we already have, that's probably
												//the template we want, so go with it
												if( OCRTemplates[ocrTemps[jj]].group == tempResults[0].Parity ) {
													ocrt = &OCRTemplates[ocrTemps[jj]];
													grpFound = 1;
													break;
												}
											}
											//if we didn't find the matching group to this, clear the stored results and store this one.
											if( !grpFound ) {
												return DEC_FAILURE;
											}
										}
										//if there were more than 1 matched templates
										else if( ocrRes > 1 ) {
											int jj; 
											for( jj = 0; jj < ocrRes; jj++ ) {
												//if we're expecting more than 1 piece in this group, we want ot favor it, so we'll just
												//keep this template
												if( OCRTemplates[ocrTemps[jj]].groupTotal > 1 ) {
													ocrt = &OCRTemplates[ocrTemps[jj]];
													OCRResultsMatchMultiple = 1;	//say we chose a template, but it was out of a few choices
													DECSTATUS(fullDecoder,15);		//call out to give us more time to find the other piece(s)
													break;
												}
											}
										}

										res->AppendType = OCR_USER_APPEND;
										res->AppendIndex = SORT_BY_LOCATION;	//sort them by the position in the image
										res->AppendTotal = ocrt->groupTotal;
										res->Parity = ocrt->group;
										
										//if we have all the pieces, we might be done
										if( storedResultsIndex+1 >= res->AppendTotal ) {
											int jk, grp;
											
											//call this to put the results in order so we can check them against the OCR template order
											storedResultsIndex++;
											SortResults();		
											storedResultsIndex--;
											
											//check to see if the graphical order matches the template order
											for( jk = 0, grp = 0; jk <= storedResultsIndex && grp < OCR_MAX_TEMPLATES; grp++) {
												//first we have to find which group we want to check against
												if( OCRTemplates[grp].group == tempResults[jk].Parity ) {
													//since the tempate values were checked already, should just need the lengths checked
													if( OCRTemplates[grp].length != tempResults[jk].Length ) {
														return DEC_FAILURE;
													}
													
													//if this is not the first piece, add a space
													if( jk ) {
														InsertCharacter(&tempResults[jk].Data[0],&tempResults[jk].Length,' ',0);
													}

													jk++; //only increment after we've found the group
												}
											}

											CombineResults(storedResultsIndex);

											//this will be automatically incremeneted after leaving this function, so since we combined the results here, 
											//it will be accurate after this point
											storedResultsIndex = 0; 
											finishingAppendType = APPEND_FINISHED;
										}
										//don't have enough pieces
										else {
											finishingAppendType = OCR_USER_APPEND;
										}

										//returning here is important so we can compile this section out nicely if we want
										return DEC_SUCCESS;
									}
								}
#endif
								
								if( finishingAppendType != NO_APPEND ) {	//if we were expecting an append,
									return DEC_FAILURE;						//fail so we clear the stored stuff and re-store this code
								}
								else { //this is a single barcode with no appending, have to put some info here to let the outside know how many codes are here
									finishingAppendType = APPEND_FINISHED;
								}
						} //switch(modifier)
					}//if( finishingType )
					else {
						return DEC_FAILURE;
					}
					break; //case OCR
				default: //not a value append symbology
					if( finishingAppendType != NO_APPEND ) {	//if we were expecting an append,
						return DEC_FAILURE;						//fail so we clear the stored stuff and re-store this code
					}
					else { //this is a single barcode with no appending, have to put some info here to let the outside know how many codes are here
						finishingAppendType = APPEND_FINISHED;
					}
			}//switch(symbologyEx)
			break; //case 0
				
		default: //not a valid append symbology
			if( finishingAppendType != NO_APPEND ) {	//if we were expecting an append,
				return DEC_FAILURE;						//fail so we clear the stored stuff and re-store this code
			}
			else { //this is a single barcode with no appending, have to put some info here to let the outside know how many codes are here
				finishingAppendType = APPEND_FINISHED;
			}
			break;
	}//switch(Symbology)
	return DEC_SUCCESS; //append info is filled out correctly
}


static void CheckEANEmulation( void ) {
	DecoderResult eanResult;
	CopyResult(&tempResults[0],&eanResult); //fill in parameters to start

	//change what we're going to change
	eanResult.AppendType = NO_APPEND;
	eanResult.AppendIndex = 0;
	eanResult.AppendTotal = 1;
	eanResult.IQImageInfo = tempResults[0].IQImageInfo;

	int start = 0; //position to start copying other data to this result
	int copyResults = DEC_FALSE;
	int changeAIM = DEC_TRUE;

	if( EANEmulationMode && tempResults[0].AppendType == COUPON_CODE_APPEND ) { 
		if( EANEmulationMode < 3 ) {//in 1 and 2, we stuff this in front
			strncpy( (char *)eanResult.Data,"010",3);
			start += 3;
			if( tempResults[0].HHPCodeID == DEC_HHP_CODE_ID_UPCA ) {
				eanResult.Data[start++] = '0';
			}
		}
		for( int jk=0; jk<tempResults[0].Length; jk++ ) {
			eanResult.Data[start+jk] = tempResults[0].Data[jk];
		}
		start += tempResults[0].Length;
		eanResult.Length = start;

		if( storedResultsIndex > 1 ) {
			eanResult.HHPCodeID = DEC_HHP_CODE_ID_MERGED_COUPON; //if there is more than 1 result, change the code ID to show merged data

			for( int jk=0; jk<tempResults[1].Length; jk++ ) {
				eanResult.Data[start+jk] = tempResults[1].Data[jk];
			}
			eanResult.Length += tempResults[1].Length;
		}
		else { //needed because when the result goes out with just 1 code, it's going to decrement this value
			eanResult.AppendTotal++;
		}
		copyResults = DEC_TRUE;
	}
	else if( tempResults[0].LinkFlag == DEC_LINKED_RSS ||  //if it's an RSS or a C128 with emulation turned on, combine the codes.
		( (EANEmulationMode && EANEmulationMode < 4 && tempResults[0].LinkFlag == DEC_LINKED_C128 ) ) ) {

		CopyResult(&tempResults[0],&eanResult); //make sure all of the info is filled in first
		eanResult.AppendType = NO_APPEND;
		eanResult.AppendIndex = 0;
		eanResult.AppendTotal = 1;

		start += tempResults[0].Length;
		for( int jk=0; jk<tempResults[1].Length; jk++ ) {
			eanResult.Data[start+jk] = tempResults[1].Data[jk];
		}
		eanResult.Length = start+tempResults[1].Length;

		copyResults = DEC_TRUE;
	}
	else if( tempResults[0].Symbology == DEC_SYMBOLOGY_UPC ) {
		
		CopyResult(&tempResults[0],&eanResult); //make sure most info is filled in first.
		eanResult.AppendType = NO_APPEND;
		eanResult.AppendIndex = 0;
		eanResult.AppendTotal = 1;

		if( EANEmulationMode == 1 || EANEmulationMode == 2 ) {//in 1 and 2, we stuff this in front
			strncpy( (char *)eanResult.Data,"010",3);
			start += 3;
			if( tempResults[0].HHPCodeID == DEC_HHP_CODE_ID_UPCA ||
				tempResults[0].HHPCodeID == DEC_HHP_CODE_ID_UPCE ) {
				eanResult.Data[start++] = '0';
			}
			else if( tempResults[0].HHPCodeID == DEC_HHP_CODE_ID_EAN8 ) {
				strncpy( (char *)&eanResult.Data[start],"00000",5);
				start += 5;
			}
			copyResults = DEC_TRUE;
		}	
		else if( EANEmulationMode == 4 && tempResults[0].HHPCodeID == DEC_HHP_CODE_ID_EAN8 ) {
			for( start=0; start<5; start++ ) {
				eanResult.Data[start] = '0';
			}
			copyResults = DEC_TRUE;
			eanResult.AIMModifier = '0';
			eanResult.HHPCodeID = DEC_HHP_CODE_ID_EAN13;
		}
		else {
			start = eanResult.Length;
		}

		if( copyResults == DEC_TRUE ) {
			for( int jk=0; jk<tempResults[0].Length; jk++ ) {
				eanResult.Data[start+jk] = tempResults[0].Data[jk];
			}
			start += tempResults[0].Length;
			eanResult.Length = start;
		}

		if( storedResultsIndex > 1 ) {
			if( EANEmulationMode && tempResults[0].AppendType == UPC_COMPOSITE_APPEND ) {
				for( int jk=0; jk<tempResults[1].Length; jk++ ) {
					eanResult.Data[start+jk] = tempResults[1].Data[jk];
				}
				eanResult.Length = start+tempResults[1].Length;
				copyResults = DEC_TRUE;
			}
		}
	}
	else if( (EANEmulationMode == 1 || EANEmulationMode == 2) && 
			  tempResults[0].Length == 14 && tempResults[0].Symbology == DEC_SYMBOLOGY_I25 ) {
		
		InsertCharacter(&(tempResults[0].Data[0]),&(tempResults[0].Length),'1',0); 
		InsertCharacter(&(tempResults[0].Data[0]),&(tempResults[0].Length),'0',0); 
		changeAIM = DEC_FALSE;
	}

	if( changeAIM == DEC_TRUE ) {
		switch( EANEmulationMode ) {
			case 1: 
			case 3:
				eanResult.AIMCodeLetter = DEC_AIM_CODELETTER_GS1_128;
				eanResult.AIMModifier = '1';
				break;
			case 2:
				eanResult.AIMCodeLetter = DEC_AIM_CODELETTER_GS1_DATABAR;
				eanResult.AIMModifier = '0';
				break;
		}
	}

	if( copyResults == DEC_TRUE ) {
		CopyResult(&eanResult,&tempResults[0]);
	}
}

#define TWIX(a,b,c) ((a <= c)&&(c <= b))
static void From (outline *ol, Point_tag a) {
	LASTXY = a; TY = BY = LASTXY.y;
	if (TWIX(0,MAXY-1,a.y)) LX[a.y] = RX[a.y] = a.x;
}

static void Update (outline *ol, int x, int y) {
	if (y < TY) {
		TY = y; if (TWIX(0,MAXY-1,y)) LX[y] = RX[y] = x;
	} else {
		if (y > BY) {
			BY = y; if (TWIX(0,MAXY-1,y)) LX[y] = RX[y] = x;
		} else {
			if (TWIX(0,MAXY-1,y)) {
				if (x < LX[y]) LX[y] = x; else if (x > RX[y]) RX[y] = x;
			}
		}
	}
}

static void To (outline *ol, Point_tag b) {
	int x, y;
	if (b.y > LASTXY.y) {
		for (y=LASTXY.y; y<=b.y; y++) {
			x = LASTXY.x + (int)(y-LASTXY.y)*(b.x-LASTXY.x)/(b.y-LASTXY.y);
			Update(ol,x,y);
		}
	} else {
		if (b.y < LASTXY.y) {
			for (y=LASTXY.y; y>=b.y; y--) {
				x = LASTXY.x + (int)(y-LASTXY.y)*(b.x-LASTXY.x)/(b.y-LASTXY.y);
				Update(ol,x,y);
			}
		} else {
			Update(ol,b.x,b.y);
		}
	}
	LASTXY = b;
}

void DrawBorder( void *img, int w, int t, int b, int l, int r, int c ) {
	int x,y;
	
	//Draw top line
	for( x = t, y = l; y < r; y++ ) {
		*((char *)img+(x*w)+y) = c;
	}

	//Draw bottom line
	for( x = b, y = l; y <= r; y++ ) {
		*((char *)img+(x*w)+y) = c;
	}

	//Draw left line
	for( x = t, y = l; x < b; x++ ) {
		*((char *)img+(x*w)+y) = c;
	}

	//Draw right line
	for( x = t, y = r; x < b; x++ ) {
		*((char *)img+(x*w)+y) = c;
	}
}

void DrawDecodeWindow( void *imgPtr ) {

	int c;
	
	switch( ShowDecodeWindow ) {
		case 2: //black
			c = 0; break;
		case 3: //color
			//c = ?; break;
		default: //white
			c = 255; break; 
	}
	
	DrawBorder( imgPtr, ImageWrap, DecodeWindowTop, DecodeWindowBottom, DecodeWindowLeft, DecodeWindowRight, c );
}

void DrawSearchCrosshair( void *imgPtr ) {
	int jk, c;
	int centerx = ImageCenterX;
	int centery = ImageCenterY;
	
	switch( ShowDecodeWindow ) {
		case 2: //black
			c = 0; break;
		case 3: //color
			//c = ?; break;
		default: //white
			c = 255; break; 
	}

	if( DecodeWindowMode ) {
		centerx = ((DecodeWindowRight-DecodeWindowLeft)/2);
		centery = ((DecodeWindowBottom-DecodeWindowTop)/2);
	
		if( DecodeWindowMode != 3 ) {
			centerx += DecodeWindowLeft;
			centery += DecodeWindowTop;
		}
	}
	
	//Draw the search center given to the decoder as a crosshair
	for( jk=-5; jk<=5; jk++ ) {
		*((char *)imgPtr+WindowOffset+(centery*ImageWrap)+(centerx+jk)) = c;
	}

	for( jk=-5; jk<=5; jk++ ) {
		*((char *)imgPtr+WindowOffset+((centery+jk)*ImageWrap)+centerx) = c;
	}

	//Draw the programmed search center, since it could be different than the decoder search start as an 'X'
	*((char *)imgPtr+((ImageCenterY+2)*ImageWrap)+(ImageCenterX+2)) = c;
	*((char *)imgPtr+((ImageCenterY-2)*ImageWrap)+(ImageCenterX+2)) = c;
	*((char *)imgPtr+((ImageCenterY+1)*ImageWrap)+(ImageCenterX+1)) = c;
	*((char *)imgPtr+((ImageCenterY-1)*ImageWrap)+(ImageCenterX+1)) = c;
	*((char *)imgPtr+((ImageCenterY)*ImageWrap)+(ImageCenterX)) = c;
	*((char *)imgPtr+((ImageCenterY+1)*ImageWrap)+(ImageCenterX-1)) = c;
	*((char *)imgPtr+((ImageCenterY-1)*ImageWrap)+(ImageCenterX-1)) = c;
	*((char *)imgPtr+((ImageCenterY+2)*ImageWrap)+(ImageCenterX-2)) = c;
	*((char *)imgPtr+((ImageCenterY-2)*ImageWrap)+(ImageCenterX-2)) = c;

}


int CheckDecodeWindow( DecoderResult *res ) {
	int ret = DEC_SUCCESS;

	if( DecodeWindowMode == 1 || DecodeWindowMode == 2 ) {
		int top = DecodeWindowTop;
		int bot = DecodeWindowBottom;
		int left = DecodeWindowLeft;
		int right = DecodeWindowRight;
		outline ol;
		
		ol.maxy = ImageHeight;
		ol.rgtx = (int *)malloc(ImageHeight*sizeof(int));
		ol.lftx = (int *)malloc(ImageHeight*sizeof(int));
		
		if( ol.rgtx && ol.lftx ) {			
			//Draw the outline of the symbol
			From(&ol,res->MainBounds.corners[0]); 
			To(&ol,res->MainBounds.corners[1]); 
			To(&ol,res->MainBounds.corners[2]);
			To(&ol,res->MainBounds.corners[3]); 
			To(&ol,res->MainBounds.corners[0]);
			
			for( ret=DEC_FAILURE; (top <= bot)&&(ret==DEC_FAILURE); top++ ) {
				Pt("t:%d-%d  oly:%d-%d \r\n", top,bot,ol.topy,ol.boty);
				if( ol.topy <= top &&  top <= ol.boty ) {
					Pt("lr:%d-%d  olx:%d-%d \r\n",left,right,ol.rgtx[top],ol.lftx[top]);
					if( left <= ol.rgtx[top] && right >= ol.lftx[top] ) {
						ret = DEC_SUCCESS;
					}
				}
			}

			if( ShowDecodeWindow ) {
				int olt,olb,oll,olr;
				void *imgPtr = 0;
#if (DECODER_ID)
				unsigned int getImagePointer = 0x40004004;
				SI_DecodeGet(fullDecoder,getImagePointer,&imgPtr);
#endif

				if( res->MainBounds.corners[0].y < res->MainBounds.corners[2].y ) {
					olt = res->MainBounds.corners[0].y;
					olb = res->MainBounds.corners[2].y;
				}
				else {
					olt = res->MainBounds.corners[2].y;
					olb = res->MainBounds.corners[0].y;
				}	
	
				if( res->MainBounds.corners[0].x < res->MainBounds.corners[2].x ) {
					oll = res->MainBounds.corners[0].x;
					olr = res->MainBounds.corners[2].x;
				}
				else {
					oll = res->MainBounds.corners[2].x;
					olr = res->MainBounds.corners[0].x;
				}
				
				switch( ShowDecodeWindow ) {
					case 2: //black
						DrawBorder( imgPtr, ImageWrap, olt, olb, oll, olr, 0 ); break;
					case 3: //color
						//DrawBorder( imgPtr, ImageWidth, olt, olb, oll, olr, 255 ); break;
					default: //white
						DrawBorder( imgPtr, ImageWrap, olt, olb, oll, olr, 255 ); break; 
				}
			}
		}
		free( ol.rgtx );
		free( ol.lftx );
	}

	return ret;
}

void SetupWindow( void ) {

	WindowOffset = 0; //reset window offest to 0 for non-DECWIN3 mode. 
	
	//set all of the stored params first. Each mode will change what is wants below
#if (DECODER_ID)
	SI_DecodeSet( fullDecoder, DEC_IMAGE_HEIGHT, (void *)ImageHeight );
	SI_DecodeSet( fullDecoder, DEC_IMAGE_WIDTH, (void *)ImageWidth );
	SI_DecodeSet( fullDecoder, DEC_IMAGE_LINE_DELTA, (void *)ImageWrap );
	SI_DecodeSet( fullDecoder, DEC_IMAGE_CENTER_X, (void *)ImageCenterX );
	SI_DecodeSet( fullDecoder, DEC_IMAGE_CENTER_Y, (void *)ImageCenterY );
	SI_DecodeSet( fullDecoder, DEC_MONOCOLOR_ENABLED, (void *)MonocolorImage );
	SI_DecodeSet( fullDecoder, DEC_MONOCOLOR_OFFSET_X, (void *)MonocolorOffsetX );
	SI_DecodeSet( fullDecoder, DEC_MONOCOLOR_OFFSET_Y, (void *)MonocolorOffsetY );
	SI_DecodeSet( fullDecoder, DEC_MONOCOLOR_SPACING_X, (void *)MonocolorSpacingX );
	SI_DecodeSet( fullDecoder, DEC_MONOCOLOR_SPACING_Y, (void *)MonocolorSpacingY );
#endif
#if (DECODER_FLD)
	RSI_DecodeSet( DEC_IMAGE_HEIGHT, (void *)ImageHeight );
	RSI_DecodeSet( DEC_IMAGE_WIDTH, (void *)ImageWidth );
	RSI_DecodeSet( DEC_IMAGE_LINE_DELTA, (void *)ImageWrap );
	RSI_DecodeSet( DEC_IMAGE_CENTER_X, (void *)ImageCenterX );
	RSI_DecodeSet( DEC_IMAGE_CENTER_Y, (void *)ImageCenterY );
	RSI_DecodeSet( DEC_MONOCOLOR_ENABLED, (void *)MonocolorImage );
	RSI_DecodeSet( DEC_MONOCOLOR_OFFSET_X, (void *)MonocolorOffsetX );
	RSI_DecodeSet( DEC_MONOCOLOR_OFFSET_Y, (void *)MonocolorOffsetY );
	RSI_DecodeSet( DEC_MONOCOLOR_SPACING_X, (void *)MonocolorSpacingX );
	RSI_DecodeSet( DEC_MONOCOLOR_SPACING_Y, (void *)MonocolorSpacingY );
#endif
#if (DECODER_AD)
	ADI_DecodeSet( fullDecoder2, DEC_IMAGE_HEIGHT, (void *)ImageHeight );
	ADI_DecodeSet( fullDecoder2, DEC_IMAGE_WIDTH, (void *)ImageWidth );
	ADI_DecodeSet( fullDecoder2, DEC_IMAGE_LINE_DELTA, (void *)ImageWrap );
	ADI_DecodeSet( fullDecoder2, DEC_IMAGE_CENTER_X, (void *)ImageCenterX );
	ADI_DecodeSet( fullDecoder2, DEC_IMAGE_CENTER_Y, (void *)ImageCenterY );
	ADI_DecodeSet( fullDecoder2, DEC_MONOCOLOR_ENABLED, (void *)MonocolorImage );
	ADI_DecodeSet( fullDecoder2, DEC_MONOCOLOR_OFFSET_X, (void *)MonocolorOffsetX );
	ADI_DecodeSet( fullDecoder2, DEC_MONOCOLOR_OFFSET_Y, (void *)MonocolorOffsetY );
	ADI_DecodeSet( fullDecoder2, DEC_MONOCOLOR_SPACING_X, (void *)MonocolorSpacingX );
	ADI_DecodeSet( fullDecoder2, DEC_MONOCOLOR_SPACING_Y, (void *)MonocolorSpacingY );
#endif

	switch( ImageRotation ) {
		case 1: //90deg CCW
			DecodeWindowTop = ImageHeight-(ImageHeight*DecodeWindowRightPercent)/100;
			DecodeWindowBottom = ImageHeight-(ImageHeight*DecodeWindowLeftPercent)/100;
			DecodeWindowLeft = (ImageWidth*DecodeWindowTopPercent)/100;
			DecodeWindowRight = (ImageWidth*DecodeWindowBottomPercent)/100;
			break;
		case 2: //180deg
			DecodeWindowTop = ImageHeight-(ImageHeight*DecodeWindowBottomPercent)/100;
			DecodeWindowBottom = ImageHeight-(ImageHeight*DecodeWindowTopPercent)/100;
			DecodeWindowLeft = ImageWidth-(ImageWidth*DecodeWindowRightPercent)/100;
			DecodeWindowRight = ImageWidth-(ImageWidth*DecodeWindowLeftPercent)/100;
			break;
		case 3: //270deg CCW
			DecodeWindowTop = (ImageHeight*DecodeWindowLeftPercent)/100;
			DecodeWindowBottom = (ImageHeight*DecodeWindowRightPercent)/100;
			DecodeWindowLeft = ImageWidth-(ImageWidth*DecodeWindowBottomPercent)/100;
			DecodeWindowRight = ImageWidth-(ImageWidth*DecodeWindowTopPercent)/100;
			break;
		default: //0deg or any other value
			DecodeWindowTop = (ImageHeight*DecodeWindowTopPercent)/100;
			DecodeWindowBottom = (ImageHeight*DecodeWindowBottomPercent)/100;
			DecodeWindowLeft = (ImageWidth*DecodeWindowLeftPercent)/100;
			DecodeWindowRight = (ImageWidth*DecodeWindowRightPercent)/100;
			break;
	}

	Pt("Setup: %d-%d, %d-%d\r\n",DecodeWindowTop, DecodeWindowBottom, DecodeWindowLeft, DecodeWindowRight);
	if( DecodeWindowMode == 1 && 
		(DecodeWindowTopPercent <= 50 && DecodeWindowBottomPercent >= 50) &&	//if the window is around the 50% mark, adjust the window around the actual image "center"
		(DecodeWindowLeftPercent <= 50 && DecodeWindowRightPercent >= 50) ) {
			
		DecodeWindowTop += (ImageCenterY - (ImageHeight>>1));
		DecodeWindowBottom += (ImageCenterY - (ImageHeight>>1));
		DecodeWindowLeft += (ImageCenterX- (ImageWidth>>1));
		DecodeWindowRight += (ImageCenterX - (ImageWidth>>1));
		Pt("Mode1: %d-%d, %d-%d\r\n",DecodeWindowTop, DecodeWindowBottom, DecodeWindowLeft, DecodeWindowRight);
	}
	else if( DecodeWindowMode ) { //if we're windowed, adjust the search center to the window center
		int centerx = ((DecodeWindowRight-DecodeWindowLeft)/2);
		int centery = ((DecodeWindowBottom-DecodeWindowTop)/2);

		if( DecodeWindowMode == 3 ) { //if mode 3, adjust the height, width, and starting pointer
			WindowOffset = (ImageWrap*DecodeWindowTop)+DecodeWindowLeft;
			Pt( "Mode 3: %d-(%d,%d),%d-%d\r\n",WindowOffset, WindowOffset%ImageWrap, WindowOffset/ImageWrap,(DecodeWindowBottom-DecodeWindowTop),(DecodeWindowRight-DecodeWindowLeft));
#if (DECODER_ID)
			SI_DecodeSet( fullDecoder, DEC_IMAGE_HEIGHT, (void *)(DecodeWindowBottom-DecodeWindowTop) );
			SI_DecodeSet( fullDecoder, DEC_IMAGE_WIDTH, (void *)(DecodeWindowRight-DecodeWindowLeft) );
#endif
#if (DECODER_FLD)
			RSI_DecodeSet( DEC_IMAGE_HEIGHT, (void *)(DecodeWindowBottom-DecodeWindowTop) );
			RSI_DecodeSet( DEC_IMAGE_WIDTH, (void *)(DecodeWindowRight-DecodeWindowLeft) );
#endif

			if( MonocolorImage ) { //also if monocolor, adjust the monocolor pixel
				int windowOffX = WindowOffset % ImageWrap;
				int windowOffY = WindowOffset / ImageWrap;
				
				int monoOffX = MonocolorSpacingX - ((windowOffX-MonocolorOffsetX) & (MonocolorSpacingX-1));
				int monoOffY = MonocolorSpacingY - ((windowOffY-MonocolorOffsetY) & (MonocolorSpacingY-1));
#if (DECODER_ID)
				SI_DecodeSet( fullDecoder, DEC_MONOCOLOR_OFFSET_X, (void *)monoOffX );
				SI_DecodeSet( fullDecoder, DEC_MONOCOLOR_OFFSET_Y, (void *)monoOffY );
#endif
#if (DECODER_FLD)
				RSI_DecodeSet( DEC_MONOCOLOR_OFFSET_X, (void *)monoOffX );
				RSI_DecodeSet( DEC_MONOCOLOR_OFFSET_Y, (void *)monoOffY );
#endif
			}
		}
		else {
			centerx += DecodeWindowLeft;
			centery += DecodeWindowTop;
		}

#if (DECODER_ID)
		SI_DecodeSet( fullDecoder, DEC_IMAGE_CENTER_X, (void *)centerx );
		SI_DecodeSet( fullDecoder, DEC_IMAGE_CENTER_Y, (void *)centery );
#endif
#if (DECODER_FLD)
		RSI_DecodeSet( DEC_IMAGE_CENTER_X, (void *)centerx );
		RSI_DecodeSet( DEC_IMAGE_CENTER_Y, (void *)centery );
#endif
	}
}


//assumes results are ordered
//assumes to use all IDs and such from the first result
static void CombineResults( int endIndex ) {
	for( int jk=1; jk<tempResults[endIndex].AppendTotal; jk++ ) {
		memcpy(&tempResults[0].Data[tempResults[0].Length],&tempResults[jk].Data[0],tempResults[jk].Length);
		tempResults[0].Length += tempResults[jk].Length;
	}
	tempResults[0].AppendTotal = 1;
	
	//since all results were combined into 1, mark that we now only have 1 result
	storedResultsIndex = 1;

}

static void DeleteCharacter( DecoderResult *res, int charIndex ) {
	for( ; charIndex<res->Length-1; charIndex++ ) {
		res->Data[charIndex] = res->Data[charIndex+1];
	}
	res->Length--;
}

static void InsertCharacter (unsigned char *res,int *len, unsigned char c, int x) {
	int i;
	if (x < (*len+1)) {
		for (i= *len; i>x; i--) res[i] = res[i-1];
		 res[x] = c; (*len)++;
	}
}

//assumes combined message
static void ConvertTLCMessage(DecoderResult *res) {
	   int i;
       for (i=6; i<res->Length; i++) {
			switch (res->Data[i]) {
				case '*': res->Data[i] = 29; break;
				case ';': res->Data[i] = '0'; break;
				case '<': res->Data[i] = '1'; break;
				case '>': res->Data[i] = '2'; break;
				case '@': res->Data[i] = '3'; break;
				case '[': res->Data[i] = '4'; break;
				case '\\': res->Data[i] = '5'; break;
				case ']': res->Data[i] = '6'; break;
				case '_': res->Data[i] = '7'; break;
				case '`': res->Data[i] = '8'; break;
				case '~': res->Data[i] = '9'; break;
			}
		}
		InsertCharacter(&res->Data[0],&res->Length,29,6); 
		InsertCharacter(&res->Data[0],&res->Length,'P',0); 
		InsertCharacter(&res->Data[0],&res->Length,'6',0); 
		InsertCharacter(&res->Data[0],&res->Length,29,0);
		InsertCharacter(&res->Data[0],&res->Length,'0',0); 
		InsertCharacter(&res->Data[0],&res->Length,30,0); 
		InsertCharacter(&res->Data[0],&res->Length,'>',0); 
		InsertCharacter(&res->Data[0],&res->Length,')',0); 
		InsertCharacter(&res->Data[0],&res->Length,'[',0);

		if ((res->Data[15] >= 'A') && (res->Data[15] <= 'Z')) {
			InsertCharacter(&res->Data[0],&res->Length,'S',15); 
			InsertCharacter(&res->Data[0],&res->Length,'6',5);
		} 
		else {
			InsertCharacter(&res->Data[0],&res->Length,'4',15); 
			InsertCharacter(&res->Data[0],&res->Length,'0',15); 
			InsertCharacter(&res->Data[0],&res->Length,'0',15); 
			InsertCharacter(&res->Data[0],&res->Length,'8',15);
			InsertCharacter(&res->Data[0],&res->Length,'0',6); 
			InsertCharacter(&res->Data[0],&res->Length,'9',6); 
			InsertCharacter(&res->Data[0],&res->Length,'5',5);
		}
		res->Data[res->Length++] = 30;
		res->Data[res->Length++] = 4;
		res->HHPCodeID = DEC_HHP_CODE_ID_TLC39;	
		res->AIMCodeLetter = DEC_AIM_CODELETTER_TLC39;
		res->AIMModifier = '2';
}

static void InvertImage(void *p_image)
{
    unsigned char   *src_addr;
    int             currentRow;
    int             currentCol;
    int             maxRow;
    int             maxCol;
    int             wrap;

	maxRow = ImageHeight; //SI_DecodeGet(fullDecoder, DEC_IMAGE_HEIGHT,		(void *)&maxRow);
	maxCol = ImageWidth; //SI_DecodeGet(fullDecoder, DEC_IMAGE_WIDTH,		(void *)&maxCol);
	wrap = ImageWrap; //SI_DecodeGet(fullDecoder, DEC_IMAGE_LINE_DELTA,	(void *)&wrap);

    for (currentRow=0; currentRow<maxRow; currentRow++)
    {
        src_addr= (unsigned char *)p_image + (currentRow*wrap);

        for (currentCol=0; currentCol < maxCol; currentCol++)
        {
            *(src_addr + currentCol) = ~(*(src_addr + currentCol));
        }
    }
}

#if (OCR_SEPARATE)
static int CheckOCRTemplate( unsigned char *templ ) {

	int tSize = strlen( (const char *)templ );	//template ends with 0
	int badTemplate = 0;
	int groupTotals[OCR_MAX_TEMPLATES];
	int returnCode = DEC_FALSE;
	unsigned char *newTemp; //space for changed template
	int font = 3;	//to track the font for each tempalte section, setting it to something removes the compile warning
	int t;	//template character
	int g;	//group number
	int jk;	//random counter
	
	//clear all current templates
	OCRNumberOfTemplates = 0;

	for( jk = 0; jk < OCR_MAX_TEMPLATES; jk++ ) {
		OCRTemplates[jk].length = 0;
		groupTotals[jk] = 1;		//each group will have at least 1 template in it
	}

	//first pass scan for invalid tempate codes
	for( t = 0, g = 0; t < tSize && g < OCR_MAX_TEMPLATES; ) {
		switch( templ[t] ) {
			case 2:		//new line, can only handle 1 line
			case 3:		//signifies "group" usage, so shouldn't need to test for 4 or 10
			case 11:	//in-line group, don't need to test for 12
			case 15:	//variable range, don't want to take up too much room
				badTemplate = 1; break;
			case 1:		//new template
				g++;	//move to the next group
			case 13:	//checksum
			case 14:	//fixed repeat
				t += 2; break;
			case ' ':	
				groupTotals[g]++;	//no break! each space adds a section to the group
			default:	//anything else is a 5,6,7,8, or ASCII
				t++;
		}

		if( badTemplate ) {
			Pt("bad Template  %d\r\n",templ[t]);
			return DEC_TRUE; //we can't handle this template here, so just flag it to pass along
		}
	}
	
	//if we got out of the first scan, we can handle the template here
	newTemp = (unsigned char *)malloc(tSize + OCR_MAX_TEMPLATES); //add some space for added font chars

	//if we didn't actually get memory, don't continue
	if( newTemp == 0 ) {
		Pt("newTemp malloc fail\r\n");
		return DEC_TRUE;
	}

	//copy current template to new space so we can modify it
	memcpy( newTemp, templ, tSize );
		
	//test each codeword to build the templates
	//g = 1 this time because it's used before we get into the template. We do this so each "template" stored has the 
	//correct group total and we don't have to go back through a 3rd time. The group number itself doesn't matter as 
	//long as the templates all match their group numbers
	//have the return code in the test because most things are inside the switch, not sure how that's handled with 
	//breaking the loop using "break"
	for( t = 0, g = 1; t < tSize && returnCode == DEC_FALSE; ) {
		
		OCRTemplate *ocrt = &OCRTemplates[OCRNumberOfTemplates];	//for ease of reading
		
		ocrt->group = g;
		ocrt->groupTotal = groupTotals[g];

		switch( newTemp[t] ) {
			case 1:		//new template means we're done with the section of the template
				//if t > 0, then we are past the first '1'. these values are set appropiately at the start
				if( t ) {
					g++;
					OCRNumberOfTemplates++;
				}

				font = newTemp[t+1];	//save the font for this template in case we break it up into new ones
				t += 2;	//step over the font char as it's not important for testing
				break;

			//for a space, we need to go to the next template storage and mark the user template as a "new template"
			case ' ':
				if( OCRNumberOfTemplates < OCR_MAX_TEMPLATES ) { 
					newTemp[t] = 1;				//replace the space with a new template
					InsertCharacter((unsigned char *)newTemp,&tSize,font,t+1); //insert the font
					OCRNumberOfTemplates++;		//move to the next template storage
				}
				else {
					returnCode = DEC_TRUE;	//ran out of template room
				}
					
				//advance to the next code word we care about
				t += 2;		//this is +2 because we added a char to the new template
				break;
				
			case 13:	//checksum
				if( ocrt->length < OCR_MAX_TEMPLATE_CHARS ) {
					ocrt->templ[ocrt->length++] = 7;	//char is alphanumeric
				}
				else {
					returnCode = DEC_TRUE;	//ran out of char space
				}

				t += 2;	//skip the checksum definition char, don't care what it is since we're not checking it as a checksum
				break;

			case 14:	//fixed repeat
				//fill template rest of the way with the repeated char
				for( jk = 0; jk < newTemp[t+1]-1; jk++ ) { //minus 1 since there is already the defining char in the template
					if( ocrt->length < OCR_MAX_TEMPLATE_CHARS ) {
						ocrt->templ[ocrt->length++] = newTemp[t-1];
					}
					else {
						returnCode = DEC_TRUE;	//ran out of char space
					}
				}

				t += 2;	//skip over the amount to repeat digit since we handled it
				break;
					
			default:	//5, 6, 7, 8, or ASCII, just put it in the template
				if( ocrt->length < OCR_MAX_TEMPLATE_CHARS ) {
					ocrt->templ[ocrt->length++] = newTemp[t];
				}
				else {
					returnCode = DEC_TRUE;	//ran out of char space
				}

				t++;
				break;
		}
	}
	
	newTemp[tSize] = 0; //be sure it's null terminated

	OCRNumberOfTemplates++;	//increment to get the number of templates instead of the index

#if (DECODE_DEBUG)
		Pt("**stored templates: %d**\r\n", OCRNumberOfTemplates);

		for( g = 0; g < OCRNumberOfTemplates; g++ ) {
			OCRTemplate *ocrt = &OCRTemplates[g];
			Pt("[%d] (%d  %d): ", g, ocrt->group,ocrt->groupTotal);
			for( jk = 0; jk < ocrt->length; jk++ ) {
				Pt("%d ",ocrt->templ[jk]);
			}
			Pt(" -- %d\r\n",ocrt->length);
		}
		Pt("********\r\n");
		Pt("---new template---\r\n");
		for( g = 0; g < strlen((char *)newTemp); g++ ) {
			Pt("%d, ",newTemp[g]);
		}
		Pt("\r\n-------------------\r\n");
#endif

#if (DECODER_ID)
	SI_DecodeSet( fullDecoder, DEC_OCR_TEMPLATE, (void *)newTemp ); //only applies to ID
#endif

	free( newTemp );

	return returnCode; 
}

static int CheckOCRResult( DecoderResult *res, int *temps ) {

	int match[OCR_MAX_TEMPLATES];	//templates this result matches 
	int matches;					//number of templates the result matched
	int i, jk;						//counters
	unsigned char t,d;				//testing chars, for readability
	
	//go through all templates saved
	for( i = 0, matches = 0; i < OCRNumberOfTemplates; i++ ) {
		
		//set the current testing template
		OCRTemplate *ocrt = &OCRTemplates[i];

		//if the lengths don't match, then it can't match this template
		if( res->Length == ocrt->length ) {
			//test the characters
			for( jk = 0; jk < ocrt->length; jk++ ) {
				//for readability
				t = ocrt->templ[jk];
				d = res->Data[jk];

				if(  t == d ||	//if it's the specific char 
					 t == 8 ||	//or any char
					(t == 5 && d >= '0' && d <= '9') ||	//or numeric
					(t == 6 && d >= 'A' && d <= 'Z') ||	//or alpha
					(t == 7 && ((d >= '0' && d <= '9') || (d >= 'A' && d <= 'Z'))) ) {	//or alphanumeric
					
						//if this is the last char in the template, then we got through it all passing
						if( jk == ocrt->length-1 ) {
							match[matches++] = i;
						}
				}
				else { //failed to match the template, stop testing this template
					break;
				}
			}
		} //if( length )
	}

	//if we got any matches, copy the matches into the buffer
	if( matches ) {
		memcpy(temps, &match, sizeof(int)*matches);
	}

	return matches;
}
#endif //OCR_SEPARATE

//Visible Functions
int InitDecoder( int width, int height, int wrap ) {
	int returnCode = 0;

	DecodeReset();
#if (DECODER_ID)
	fullDecoder = SI_InitDecoder(width,height,wrap);
	if( fullDecoder ) {
		SI_InstallCallback( fullDecoder, CB_RESULT, (void *)ResultCallback );
		SI_InstallCallback( fullDecoder, CB_STATUS, (void *)StatusCallback );
		SI_InstallCallback( fullDecoder, CB_PROGRESS, (void *)ProgressCallback );
	}
	returnCode |= fullDecoder;
#endif

#if (DECODER_AD)
	fullDecoder2 = ADI_InitDecoder(width,height,wrap);
	if( fullDecoder2 ) {
		ADI_InstallCallback( fullDecoder2, CB_RESULT, (void *)ResultCallback );
		ADI_InstallCallback( fullDecoder2, CB_STATUS, (void *)StatusCallback );
		ADI_InstallCallback( fullDecoder2, CB_PROGRESS, (void *)ProgressCallback );
	}
	returnCode |= fullDecoder2;
#endif

#if (DECODER_FLD)
	fastDecoder = RSI_InitDecoder(width,height,wrap);
	if( fastDecoder ) {
		RSI_InstallCallback( CB_RESULT, (void *)ResultCallback );
		RSI_InstallCallback( CB_STATUS, (void *)StatusCallback );
		RSI_InstallCallback( CB_PROGRESS, (void *)ProgressCallback );
	}
	returnCode |= fastDecoder;
#endif

#if (DECODER_ULD)
	laserDecoder = ULI_InitDecoder(width);
	if( laserDecoder ) {
		ULI_InstallCallback( CB_RESULT, (void *)ResultCallback );
		ULI_InstallCallback( CB_STATUS, (void *)StatusCallback );
		ULI_InstallCallback( CB_PROGRESS, (void *)ProgressCallback );
	}
	returnCode |= laserDecoder;
#endif

	ImageHeight = height;
	ImageWidth = width;
	ImageWrap = wrap;

	SetupWindow();

	return returnCode;
}

void DestroyDecoder( void ) {

#if (DECDOER_ID)
	SI_DestroyDecoder(fullDecoder);
	fullDecoder = 0;
#endif

#if (DECODER_AD)
	ADI_DestroyDecoder();
	fullDecoder2 = 0;
#endif

#if (DECODER_FLD)
	RSI_DestroyDecoder();
	fastDecoder = 0;
#endif

#if (DECODER_ULD)
	ULI_DestroyDecoder();
	laserDecoder = 0;
#endif
}

void ReportDecoderVersion( int type, char *reportString ) {
	int stringOffset = 0;
	char logicRev[80] = "$ProjectRevision: 1.97 $";
	
	if( type & DEC_REVISION_CONTROL_LOGIC ) {
		sprintf(reportString,"Control Logic Version: %s",&logicRev[18]);
		stringOffset = strlen(reportString);
		reportString[stringOffset-2] = ' ';
		reportString[stringOffset-1] = '\r';
		reportString[stringOffset] = '\n';
		reportString[stringOffset+1] = '\0';
		stringOffset = strlen(reportString);
	}
	
#if (DECODER_ID)
	if( type & DEC_REVISION_ID ) {
		SI_ReportDecoderVersion(fullDecoder, &reportString[stringOffset]);
		strcat(reportString," \r\n");
		stringOffset = strlen(reportString);
	}
#endif

#if (DECODER_AD)
	if( type & DEC_REVISION_AD ) {
		ADI_ReportDecoderVersion( &reportString[stringOffset]);
		strcat(reportString," \r\n");
		stringOffset = strlen(reportString);
	}
#endif

#if (DECODER_FLD)
	if( type & DEC_REVISION_FLD ) {
		RSI_ReportDecoderVersion(&reportString[stringOffset]);
		strcat(reportString,"\r\n");
		stringOffset = strlen(reportString);
	}
#endif

#if (DECODER_ULD)
	if( type & DEC_REVISION_ULD ) {
		ULI_ReportDecoderVersion( &reportString[stringOffset]);
		strcat(reportString," \r\n");
		stringOffset = strlen(reportString);
	}
#endif
}

int InstallCallback( unsigned int tag, void *function ) {

	int idSet = 0;
	int adSet = 0;
	int fldSet = 0;
	int uldSet = 0;

	switch( tag ) {
		case CB_PRINTF:
			DECPRINTF = (fPrintf_t)function;
			idSet = 1;
			adSet = 1;
			fldSet = 1;
			uldSet = 1;
			break;
		case CB_TIMER:
			DECTIMER = (fTimer_t)function;
			idSet = 1;
			adSet = 1;
			fldSet = 1;
			uldSet = 1;
			break;
		case CB_RESULT:
			DECRESULT = (fResult_t)function;
			break;
		case CB_RESULT_NOTIFY:
			DECNOTIFY = (fResultNotify_t)function;
			break;
		case CB_WATCHDOG:
			DECWATCHDOG = (fWatchDog_t)function;
			idSet = 1;
			adSet = 1;
			fldSet = 1;
			uldSet = 1;
			break;
		case CB_STATUS:
			DECSTATUS = (fStatus_t)function;
			break;
		case CB_PROGRESS:
			DECPROGRESS = (fProgress_t)function;
			break;
		default:
			return DEC_FAILURE; //not a valid tag, return error
	}
	
	//now break down the functions
	//still need the if() because otherwise it gives usage warnings
	if( idSet ) {
#if (DECODER_ID)
		SI_InstallCallback( fullDecoder, tag, function );
#endif
	}

	if( adSet ) {
#if (DECODER_AD)
		ADI_InstallCallback( fullDecoder2, tag, function );
#endif
	}

	if( fldSet ) {
#if (DECODER_FLD)
		RSI_InstallCallback( tag, function );
#endif
	}

	if( uldSet ) {
#if (DECODER_ULD)
		ULI_InstallCallback( tag, function );
#endif
	}

	return DEC_SUCCESS; //found a good tag
}

int Decode( void *image, int decodeType ) {

	int returnCode = DEC_FAILURE;
	void *img = (void *)((char *)image+WindowOffset);

	//clear any appends that are required to read in the same image, these
	//are ones that are to be excluded

	if( needToClearResults == 1 && ResultMutex == 0 ) {
		ClearStoredResults(0);
	}

	if( storedResultsIndex > 0 ) {
#if (DECODER_FLD)
		if( decodeType == DEC_TYPE_FLD ) {
			RSI_Decode(img,0);
			return DEC_FAILURE;
		}
#endif
		if( ResultMutex == 0 ) {
			ClearStoredResults( CODE128_APPEND |
							CODE39_APPEND |
							CODE93_APPEND |
							MACROPDF_APPEND|
							OCR_APPEND );
		}
	}

#if (DECODER_FLD)
	if( decodeType == DEC_TYPE_FLD ) {
		returnCode = RSI_Decode(img,FastDecoderEnabled);
	}
#endif


	if( FullDecoderEnabled ) {
#if (DECODER_ID)
		if( decodeType == DEC_TYPE_ID ) {
			//if there are no stored results and TryRev is on or if we got a stored result using video reverse
			if( TryReverseVideo ) {
				VideoReverseResult |= VID_REVERSE_ATTEMPT;
				decodeStartTime = DECTIMER();
				InvertImage(img);
				returnCode = SI_Decode(fullDecoder,img);
				decodeStartTime = 0; //only needed if we need to kill it during a reverse image decode. 
			}
		
			if( (TryReverseVideo == VID_REVERSE_OFF || TryReverseVideo == VID_REVERSE_BOTH) &&
				(VideoReverseResult & VID_REVERSE_STORED_RESULT) == 0 &&
				(VideoReverseResult & VID_REVERSE_DECODED) == 0 ) { 
				//if it is on, then the image at this point is the reverse video
				if( VideoReverseResult & VID_REVERSE_ATTEMPT ) { 
					VideoReverseResult = VID_REVERSE_NONE; //set this to 1 so we know this image is not reversed
					InvertImage(img);	//reverse it back to reg video
				}

				//FullDecoderStop = decodeType;
				returnCode = SI_Decode(fullDecoder,img);
				if( storedResultsIndex == 0 ) {
					noread++;
					if( noread >= 3 ) {
						timeStamp = 0;
					}
				}
				else {
					noread = 0;
				}

				if( SI_OptimizeEnables ) { //if we disabled some group for faster processing
					//Save off current config
					SI_DecodeGet( fullDecoder, DEC_DISABLE_SYMBOLOGY_GROUP, &SI_DisabledGroups);
					SI_DecodeGet( fullDecoder, DEC_QR_ENABLED, &SI_DisableQR );
					SI_DecodeGet( fullDecoder, DEC_HANXIN_ENABLED, &SI_DisableHanXin );
					SI_DecodeGet( fullDecoder, DEC_MAXICODE_ENABLED, &SI_DisableMaxi );
					SI_DecodeGet( fullDecoder, DEC_DATAMATRIX_ENABLED, &SI_DisableDataMatrix );
					SI_DecodeGet( fullDecoder, DEC_AZTEC_ENABLED, &SI_DisableAztec );
				
					//Disable Everything
					SI_DecodeSet( fullDecoder, DEC_DISABLE_SYMBOLOGY_GROUP, (void *)(ONE_DIMENSIONAL_CODES|STACKED_LINEAR_CODES));
					SI_DecodeSet( fullDecoder, DEC_QR_ENABLED, (void *)DEC_DISABLED );
					SI_DecodeSet( fullDecoder, DEC_HANXIN_ENABLED, (void *)DEC_DISABLED );
					SI_DecodeSet( fullDecoder, DEC_MAXICODE_ENABLED, (void *)DEC_DISABLED );
					SI_DecodeSet( fullDecoder, DEC_DATAMATRIX_ENABLED, (void *)DEC_DISABLED );
					SI_DecodeSet( fullDecoder, DEC_AZTEC_ENABLED, (void *)DEC_DISABLED );

					//Turn on what we want
					//The enables use previous values because to trigger this, it had to read the symbology to begin with.
					switch( SI_OptimizeEnables ) {
						case AZTEC_APPEND:
							SI_DecodeSet( fullDecoder, DEC_AZTEC_ENABLED, (void *)SI_DisableAztec ); break;
						case QR_APPEND:
							SI_DecodeSet( fullDecoder, DEC_QR_ENABLED, (void *)SI_DisableQR );
					}

					//now try decoding again
					returnCode = SI_Decode(fullDecoder,img);

					//Restore settings
					SI_DecodeSet( fullDecoder, DEC_DISABLE_SYMBOLOGY_GROUP, (void *)SI_DisabledGroups);
					SI_DecodeSet( fullDecoder, DEC_QR_ENABLED, (void *)SI_DisableQR );
					SI_DecodeSet( fullDecoder, DEC_HANXIN_ENABLED, (void *)SI_DisableHanXin );
					SI_DecodeSet( fullDecoder, DEC_MAXICODE_ENABLED, (void *)SI_DisableMaxi );
					SI_DecodeSet( fullDecoder, DEC_DATAMATRIX_ENABLED, (void *)SI_DisableDataMatrix );
					SI_DecodeSet( fullDecoder, DEC_AZTEC_ENABLED, (void *)SI_DisableAztec );
				
					//Change local settings to default/invalid values. 
					SI_OptimizeEnables = 0; 
					SI_DisabledGroups = -1;
					SI_DisableQR = -1;
					SI_DisableHanXin = -1;
					SI_DisableMaxi = -1;
					SI_DisableDataMatrix = -1;
					SI_DisableAztec = -1;
				}
			}
			else if( VideoReverseResult == VID_REVERSE_DECODED ) { //completed decode via reverse video
				VideoReverseResult = VID_REVERSE_NONE;
			}
			
#if (OCR_SEPARATE)
			//if the decoder was stopped while stillt trying to get the other pieces of a multiple template match
			if( storedResultsIndex && 
				OCRResultsMatchMultiple && 
				tempResults[0].AppendType == OCR_USER_APPEND &&
				tempResults[0].AppendTotal > 1 &&
				!needToClearResults ) {

				tempResults[0].AppendTotal = 1;
				tempResults[0].AppendIndex = 0;
				needToClearResults = 1;
				OCRResultsMatchMultiple = 0;
				DECRESULT(fullDecoder, &tempResults[0]);
				returnCode = DEC_TRUE;
			}
#endif
		}
#endif //DECODER_ID
#if (DECODER_AD)
		if( decodeType == DEC_TYPE_AD ) {
			returnCode = ADI_Decode( img, 1 );
		}
#endif
	}
	
#if (DECODER_ULD)
	if( decodeType == DEC_TYPE_ULD ) {
		returnCode = ULI_Decode(img);
	}
#endif
	
	if( ShowDecodeWindow ) {
		if( DecodeWindowMode ) {
			DrawDecodeWindow( image );
		}
		DrawSearchCrosshair( image );
	}
	
	return returnCode;
}

int StopDecode( void ) {
	int returnCode = DEC_FAILURE;

#if (DECODER_ID)
	returnCode = SI_StopDecode(fullDecoder);
#endif

#if (DECODER_AD)
	returnCode = ADI_StopDecode();
#endif

#if (DECODER_ULD)
	returnCode = ULI_StopDecode();
#endif

	return returnCode;
}

int ContinueDecode(void) {
	int returnCode = DEC_FAILURE;

#if (DECODER_ID)
	returnCode = SI_ContinueDecode(fullDecoder);
#endif

#if (DECODER_ULD)
	returnCode = ULI_ContinueDecode();
#endif

	return returnCode;
}

int PauseDecode(void) {
	int returnCode = DEC_FAILURE;

#if (DECODER_ID)
	returnCode = SI_PauseDecode(fullDecoder);
#endif

#if (DECODER_ULD)
	returnCode = ULI_PauseDecode();
#endif

	return returnCode;
}


int DecodeGet( unsigned int tag, void *data ) {
	int tempInt = 0;
	int returnCode = DEC_SUCCESS;

	switch(tag) {
		case DEC_WINDOW_MODE:
			memcpy(data,&DecodeWindowMode,sizeof(int)); break;
		case DEC_SHOW_DECODE_WINDOW:
			memcpy(data,&ShowDecodeWindow,sizeof(int)); break;
		case DEC_WINDOW_TOP:
			memcpy(data,&DecodeWindowTopPercent,sizeof(int)); break;
		case DEC_WINDOW_BOTTOM:
			memcpy(data,&DecodeWindowBottomPercent,sizeof(int)); break;
		case DEC_WINDOW_LEFT:
			memcpy(data,&DecodeWindowLeftPercent,sizeof(int)); break;
		case DEC_WINDOW_RIGHT:
			memcpy(data,&DecodeWindowRightPercent,sizeof(int)); break;
		case DEC_IMAGE_ROTATION:
			memcpy(data,&ImageRotation,sizeof(int)); break;
		case DEC_VIDEO_REVERSE_ENABLED:
			memcpy(data,&TryReverseVideo,sizeof(int)); break;
		case DEC_DECODE_TIME:
			memcpy(data,&decodeTime,sizeof(int)); break;
		case DEC_FAST_DECODER_ENABLED:
			memcpy(data,&FastDecoderEnabled,sizeof(int)); break;
		case DEC_FULL_DECODER_ENABLED:
			memcpy(data,&FullDecoderEnabled,sizeof(int)); break;
		case DEC_PRINT_RESULTS:
			memcpy(data,&PrintDecodeResults,sizeof(int)); break; 
		case DEC_COUPON_CODE_MODE:
			memcpy(data,&CouponCodesEnabled,sizeof(int)); break; 
		case DEC_COUPON_SECONDARY_OUTPUT:
			memcpy(data,&CouponSecondaryOutput,sizeof(int)); break;
		case DEC_EANUCC_EMULATION_MODE:
			memcpy(data,&EANEmulationMode,sizeof(int)); break;
		case DEC_CODE39_APPEND_ENABLED:
			memcpy(data,&Code39AppendEnabled,sizeof(int)); break;
		case DEC_CODABAR_START_STOP_TRANSMIT:
			memcpy(data,&CodabarStartStopTransmit,sizeof(int)); break;
		case DEC_CODABAR_CONCAT_ENABLED:
			tempInt = CodabarAppendEnabled + CodabarAppendRequired;
			memcpy(data,&tempInt,sizeof(int)); break;
		case DEC_C128_ISBT_ENABLED:
			memcpy(data,&ISBTEnabled,sizeof(int));
			break;
		case DEC_ADD_SEARCH_TIME_CONCAT:
			memcpy(data,&ConcatAdditionalSearchTime,sizeof(int));
			break;
		case DEC_ADD_SEARCH_TIME_UPC_COMPOSITE:
			memcpy(data,&UPCCompositeAdditionalSearchTime,sizeof(int));
			break;
		case DEC_COMBINE_COMPOSITES:
			memcpy(data,&CombineCompositeResults,sizeof(int));
			break;
		case DEC_COMPOSITE_WITH_UPC_ENABLED:
			memcpy(data,&UPCCompositeEnabled,sizeof(int)); break;
		case DEC_COMPOSITE_ENABLED:
			memcpy(data,&CompositeEnabled,sizeof(int)); break;
		case DEC_COMPOSITE_MIN_LENGTH:
			memcpy(data,&CompositeMin,sizeof(int)); break;
		case DEC_COMPOSITE_MAX_LENGTH:
			memcpy(data,&CompositeMax,sizeof(int)); break;
		case DEC_PDF417_MACRO_ENABLED:
			memcpy(data,&MacroPDFEnabled,sizeof(int)); break;
		case DEC_ECI_HANDLING:
			memcpy(data,&ECIHandling,sizeof(int)); break;
		case DEC_CODE93_APPEND_ENABLED:
			memcpy(data,&Code93AppendEnabled,sizeof(int)); break;
		case DEC_CODE128_APPEND_ENABLED:
			memcpy(data,&Code128AppendEnabled,sizeof(int)); break;
		case DEC_AZTEC_APPEND_ENABLED:
			memcpy(data,&AztecAppendEnabled,sizeof(int)); break;
		case DEC_AZTEC_APPEND_STRIP_INFO:
			memcpy(data,&AztecAppendStripInfo,sizeof(int)); break;
		//case DEC_DATAMATRIX_APPEND_ENABLED:
		//	memcpy(data,&DatamatrixAppendEnabled,sizeof(int)); break;
		//case DEC_DATAMATRIX_APPEND_STRIP_INFO:
		//	memcpy(data,&DatamatrixAppendStripInfo,sizeof(int)); break;
		case DEC_QR_APPEND_ENABLED:
			memcpy(data,&QRAppendEnabled,sizeof(int)); break;
		case DEC_IMAGE_HEIGHT:
			memcpy(data,&ImageHeight,sizeof(int)); break;
		case DEC_IMAGE_WIDTH:
			memcpy(data,&ImageWidth,sizeof(int)); break;
		case DEC_IMAGE_LINE_DELTA:
			memcpy(data,&ImageWrap,sizeof(int)); break;
		case DEC_MONOCOLOR_ENABLED:
			memcpy(data,&MonocolorImage,sizeof(int)); break;
		case DEC_MONOCOLOR_OFFSET_X:
			memcpy(data,&MonocolorOffsetX,sizeof(int)); break;
		case DEC_MONOCOLOR_OFFSET_Y:
			memcpy(data,&MonocolorOffsetY,sizeof(int)); break;
		case DEC_MONOCOLOR_SPACING_X:
			memcpy(data,&MonocolorSpacingX,sizeof(int)); break;
		case DEC_MONOCOLOR_SPACING_Y:
			memcpy(data,&MonocolorSpacingY,sizeof(int)); break;
		case DEC_IMAGE_CENTER_X:
			memcpy(data,&ImageCenterX,sizeof(int)); break;
		case DEC_IMAGE_CENTER_Y:
			memcpy(data,&ImageCenterY,sizeof(int)); break;
#if (OCR_SEPARATE)
		case DEC_OCR_SEPARATE_TEMPLATE:
			memcpy(data,&OCRSeparateTemplate,sizeof(int)); break;
#endif

		default:
#if (DECODER_ID) 
				returnCode = SI_DecodeGet( fullDecoder, tag, data); 
#endif
#if (DECODER_AD)
				returnCode = ADI_DecodeGet( fullDecoder2, tag, data );
#endif
#if (DECODER_FLD)
				returnCode = RSI_DecodeGet( tag,data );
#endif
#if (DECODER_ULD)
				returnCode = ULI_DecodeGet( tag, data );
#endif
				break;
	}
	return returnCode;
}

int DecodeSet( unsigned int tag, void *data ) {
	int returnCode = DEC_SUCCESS;

	//Flags for passing to decoders. Seemed the best way to do the specific passthroughs as they are not always the function tag param.
	//and this way the functions can be compiled out cleanly, making the code more readable.
	int idSet = 0;			//flag to say something needs to be passed on
	int idTag = tag;		//if it's just the tag/data that got passed in, then only need to get the flag
	int idData = (int)data;	//otherwise, these tag/data variables can be set and these will be passed on.
	int adSet = 0;
	int adTag = tag;
	int adData = (int)data;
	int fldSet = 0;
	int fldTag = tag;
	int fldData = (int)data;
	int uldSet = 0;
	int uldTag = tag;
	int uldData = (int)data;

	switch( tag ) {
		case DEC_WINDOW_MODE:
			if( DecodeWindowMode != (int)data ) {
				DecodeWindowMode = (int)data; 
				SetupWindow();
			}
			break;
		case DEC_SHOW_DECODE_WINDOW:
			ShowDecodeWindow = (int)data; break;
		case DEC_WINDOW_TOP:
			if( DecodeWindowTopPercent != (int)data ) {
				DecodeWindowTopPercent = (int)data; 
				SetupWindow();
			}
			break;
		case DEC_WINDOW_BOTTOM:
			if( DecodeWindowBottomPercent != (int)data ) {
				DecodeWindowBottomPercent = (int)data; 
				SetupWindow();
			}
			break;
		case DEC_WINDOW_LEFT:
			if( DecodeWindowLeftPercent != (int)data ) {
				DecodeWindowLeftPercent = (int)data; 
				SetupWindow();
			}
			break;
		case DEC_WINDOW_RIGHT:
			if( DecodeWindowRightPercent != (int)data ) {
				DecodeWindowRightPercent = (int)data;
				SetupWindow();
			}
			break;
		case DEC_IMAGE_ROTATION:
			ImageRotation = (int)data; 
			idSet = 1;
			break;
		case DEC_VIDEO_REVERSE_ENABLED:
			TryReverseVideo = (int)data;
			fldSet = 1;
			break;
		case DEC_DECODE_TIME:
			decodeTime = (int)data;
			break;
		case DEC_FAST_DECODER_ENABLED:
			FastDecoderEnabled = (int)data;
			break;
		case DEC_FULL_DECODER_ENABLED:
			FullDecoderEnabled = (int)data;
			break;
		case DEC_PRINT_RESULTS:
			PrintDecodeResults = (int)data; 
			break;
		case DEC_COUPON_CODE_MODE:
			CouponCodesEnabled = (int)data;
			fldSet = 1;
			break;
		case DEC_COUPON_SECONDARY_OUTPUT:
			CouponSecondaryOutput = (int)data;
			break;
		case DEC_EANUCC_EMULATION_MODE:
			EANEmulationMode = (int)data; 
			if( (int)data == 1 || (int)data == 2 ) { 
				idSet = 1; idTag = DEC_UPCE_EXPAND; idData = DEC_TRUE;
				adSet = 1; adTag = DEC_UPCE_EXPAND; adData = DEC_TRUE;
				fldSet = 1; fldTag = DEC_UPCE_EXPAND; fldData = DEC_TRUE;
				uldSet = 1; uldTag = DEC_UPCE_EXPAND; uldData = DEC_TRUE;
			}
			else {
				idSet = 1; idTag = DEC_UPCE_EXPAND; idData = UPCEExpansionTemp;
				adSet = 1; adTag = DEC_UPCE_EXPAND; adData = UPCEExpansionTemp;
				fldSet = 1; fldTag = DEC_UPCE_EXPAND; fldData = UPCEExpansionTemp;
				uldSet = 1; uldTag = DEC_UPCE_EXPAND; uldData = UPCEExpansionTemp;
			}
			break;
		case DEC_UPCE_EXPAND:
			UPCEExpansionTemp = (int)data;
			idSet = 1; 
			adSet = 1; 
			fldSet = 1; 
			uldSet = 1; 
			if( EANEmulationMode == 1 || EANEmulationMode == 2 ) {
				idData = DEC_TRUE;
				adData = DEC_TRUE;
				fldData = DEC_TRUE;
				uldData = DEC_TRUE;
			}
			break;
		case DEC_CODE39_APPEND_ENABLED:
			Code39AppendEnabled = (int)data;
			fldSet = 1;
			break;
		case DEC_TLC39_ENABLED:
			TLC39Enabled = (int)data;
			idSet = 1;
			adSet = 1;
			fldSet = 1;
			break;
		case DEC_CODABAR_CONCAT_ENABLED:
			CodabarAppendEnabled = (int)data;
			if( CodabarAppendEnabled > 1 ) {
				CodabarAppendRequired = 1;
			}
			else {
				CodabarAppendRequired = 0;
			}
			fldSet = 1;
			break;
		case DEC_CODABAR_START_STOP_TRANSMIT:
			CodabarStartStopTransmit = (int)data;
			break;
		case DEC_C128_ISBT_ENABLED:
			ISBTEnabled = (int)data;
			idSet = 1;
			idTag = DEC_CODE128_ENABLED;
			fldSet = 1;
			adSet = 1;
			adTag = DEC_CODE128_ENABLED;
			if( (int)data ) {
				idData = DEC_TRUE;
				adData = DEC_TRUE;
			}
			else {
				idData = Code128EnableTemp;
				adData = Code128EnableTemp;
			}
			break;
		case DEC_CODE128_ENABLED:
			Code128EnableTemp = (int)data;
			fldSet = 1;
			uldSet = 1;
			idSet = 1;
			adSet = 1;
			if( ISBTEnabled ) { 
				idData = DEC_TRUE;
				adData = DEC_TRUE;
			}
			break;
		case DEC_ADD_SEARCH_TIME_CONCAT:
			ConcatAdditionalSearchTime = (int)data;
			break;
		case DEC_ADD_SEARCH_TIME_UPC_COMPOSITE:
			UPCCompositeAdditionalSearchTime = (int)data;
			break;
		case DEC_COMBINE_COMPOSITES:
			CombineCompositeResults = (int)data;
			break;
		case DEC_COMPOSITE_WITH_UPC_ENABLED: 
			UPCCompositeEnabled = (int)data;
			fldSet = 1;
			break;
		case DEC_COMPOSITE_ENABLED: 
			CompositeEnabled = (int)data;
			idSet = 1;
			fldSet = 1;
			break;
		case DEC_COMPOSITE_MIN_LENGTH: 
			CompositeMin = (int)data; break;
		case DEC_COMPOSITE_MAX_LENGTH: 
			CompositeMax = (int)data; break;
		case DEC_PDF417_MACRO_ENABLED:
			MacroPDFEnabled = (int)data; break;
		case DEC_ECI_HANDLING: //NOTE: this needs to have other symbologies besides PDF handled for complete functionality transfer from SD to logic
			ECIHandling = (int)data;
			idSet = 1;
			break;
		case DEC_CODE93_APPEND_ENABLED:
			Code93AppendEnabled = (int)data; break;
		case DEC_CODE128_APPEND_ENABLED:
			Code128AppendEnabled = (int)data; break;
		case DEC_AZTEC_APPEND_ENABLED:
			AztecAppendEnabled = (int)data; break;
		case DEC_AZTEC_APPEND_STRIP_INFO:
			AztecAppendStripInfo = (int)data; break;
		//case DEC_DATAMATRIX_APPEND_ENABLED:
			//DatamatrixAppendEnabled = (int)data; break;
		//case DEC_DATAMATRIX_APPEND_STRIP_INFO:
			//DatamatrixAppendStripInfo = (int)data; break;
		case DEC_QR_APPEND_ENABLED:
			QRAppendEnabled = (int)data; break;
		case DEC_IMAGE_HEIGHT:
			if( ImageHeight != (int)data ) {
				ImageHeight = (int)data;
				SetupWindow();
			}
			break;
		case DEC_IMAGE_WIDTH:
			if( ImageWidth != (int)data ) {
				ImageWidth = (int)data;
				SetupWindow();
			}
			break;
		case DEC_IMAGE_LINE_DELTA:
			if( ImageWrap != (int)data ) {
				ImageWrap = (int)data;
				SetupWindow();
			}
			break;
		case DEC_MONOCOLOR_ENABLED:
			if( MonocolorImage != (int)data ) {
				MonocolorImage = (int)data;
				SetupWindow();
			}
			break;
		case DEC_MONOCOLOR_OFFSET_X:
			MonocolorOffsetX = (int)data;
			SetupWindow();
			break;
		case DEC_MONOCOLOR_OFFSET_Y:
			if( MonocolorOffsetY != (int)data ) {
				MonocolorOffsetY = (int)data;
				SetupWindow();
			}
			break;
		case DEC_MONOCOLOR_SPACING_X:
			if( MonocolorSpacingX != (int)data ) {
				MonocolorSpacingX = (int)data;
				SetupWindow();
			}
			break;
		case DEC_MONOCOLOR_SPACING_Y:
			if( MonocolorSpacingY != (int)data ) {
				MonocolorSpacingY = (int)data;
				SetupWindow();
			}
			break;
		case DEC_IMAGE_CENTER_X:
			if( 1 ) {//ImageCenterX != (int)data ) {//ken@2012.01.15
			//if( ImageCenterX != (int)data ) {
				ImageCenterX = (int)data;
				SetupWindow();
			}
			break;
		case DEC_IMAGE_CENTER_Y:
			if( 1 ) {//ImageCenterY != (int)data ) {
			//if( ImageCenterY != (int)data ) {
				ImageCenterY = (int)data;
				SetupWindow();
			}
			break;
#if (OCR_SEPARATE)
		case DEC_OCR_SEPARATE_TEMPLATE:
			OCRSeparateTemplate = (int)data; break;
		case DEC_OCR_ACTIVE_TEMPLATES:
			OCRActiveTemplates = (int)data;
			idSet = 1;
			break;
		case DEC_OCR_TEMPLATE:
			if( CheckOCRTemplate((unsigned char *)data) == DEC_TRUE ) {
				idSet = 1;
				adSet = 1;
			}
			break;
#endif
		default: //none of these, pass it on
			idSet = 1;
			adSet = 1;
			fldSet = 1;
			uldSet = 1;
			break;
	}

	//FIXIT: do return codes so they are more meaningful

	//if passing something along, set the return code to fail so that if none of the decoders pass, it returns
	//the failure.
	if( idSet || adSet || fldSet || uldSet ) {
		returnCode = DEC_FAILURE;
	}

#if (DECODER_ID)
	if( idSet ) {
		returnCode |= SI_DecodeSet( fullDecoder, idTag, (void *)idData );
	}
#else //needed to prevent warnings
	if( idSet || idTag || idData ) {}
#endif

#if (DECODER_AD)
	if( adSet ) {
		returnCode |= ADI_DecodeSet( fullDecoder2, adTag, (void *)adData );
	}
#else //needed to prevent warnings
	if( adSet || adTag || adData ) {}
#endif

#if (DECODER_FLD)
	if( fldSet ) {
		returnCode |= RSI_DecodeSet( fldTag, (void *)fldData );
	}
#else //needed to prevent warnings
	if( fldSet || fldTag || fldData ) {}
#endif

#if (DECODER_ULD)
	if( uldSet ) {
		returnCode |= ULI_DecodeSet( uldTag, (void *)uldData );
	}
#else //needed to prevent warnings
	if( uldSet || uldTag || uldData ) {}
#endif

	return returnCode;
}

void DecodeReset( void ) {

#if (DECODER_ID)
	fullDecoder = 0;
#endif
#if (DECODER_AD)
	fullDecoder2 = 0;
#endif
#if (DECODER_FLD)
	fastDecoder = 0;
#endif
#if (DECODER_ULD)
	laserDecoder = 0;
#endif

	FastDecoderEnabled = 0;
	FullDecoderEnabled = 1;

	ClearStoredResults(0); //just to make sure the array is initialized

	ResultMutex = 0;
	
	finishingAppendType = NO_APPEND;

#if (OCR_SEPARATE)
	memset(OCRTemplates,0,sizeof(OCRTemplate)*OCR_MAX_TEMPLATES);
	OCRSeparateTemplate = 1;
	OCRNumberOfTemplates = 0;
	OCRActiveTemplates = OCR_USER_TEMPLATE;
	OCRResultsMatchMultiple = 0;
#endif

#if (DECODER_ID)
	SI_OptimizeEnables = 0;	
	SI_DisabledGroups = -1;	
	SI_DisableQR = -1;
	SI_DisableHanXin = -1;
	SI_DisableMaxi = -1;
	SI_DisableDataMatrix = -1;
	SI_DisableAztec = -1;
#endif
	StopTheDecoder = 0;	
	decodeStartTime = 0;
	decodeTime = 0;
	additionalSearchTime = 0;
	timeStamp = 0;
	storedResultsIndex = 0;
	memset(&EmptyBounds,0,sizeof(EmptyBounds));
	memset(&EmptyIQInfo,0,sizeof(EmptyIQInfo));
	
	MacroPDFEnabled = 1;

	ECIHandling = 2;

	Code39AppendEnabled = 0;
	Code128AppendEnabled = 0;
	Code93AppendEnabled = 0;
	AztecAppendEnabled = 1;
	AztecAppendStripInfo = 1;
	QRAppendEnabled = 1;
	//DatamatrixAppendEnabled = 0;
	//DatamatrixAppendStripInfo = 1;
	//GridmatrixAppendEnabled = 0; in adaptus
	TLC39Enabled = 0;
	CouponCodesEnabled = 0;
	CouponSecondaryOutput = 0;
	ISBTEnabled = 0;
	Code128EnableTemp = 0;
	EANEmulationMode = 0;
	UPCEExpansionTemp = 0;
	CodabarAppendEnabled = 0;
	CodabarAppendRequired = 0;
	CodabarAppendStartStop = 0;
	CodabarStartStopTransmit = 1;
	ConcatAdditionalSearchTime = 300;
	UPCCompositeAdditionalSearchTime = 300;
	UPCCompositeEnabled = 0;
	CompositeEnabled = 0;
	CompositeMin = 1;
	CompositeMax = 180;

	PrintDecodeResults = 0;
	CombineCompositeResults = 0;

	TryReverseVideo = VID_REVERSE_OFF;
	VideoReverseResult = VID_REVERSE_NONE;

	DecodeWindowMode = 0;
	WindowOffset = 0;
	ShowDecodeWindow = 0;
	ImageRotation = 0;

	//coordinate vals get set in SetupWindow()
	DecodeWindowTopPercent = 40;
	DecodeWindowBottomPercent = 60;
	DecodeWindowLeftPercent = 40;
	DecodeWindowRightPercent = 60;
	
	//FIXIT: send these values to proper decoders at reset
	MonocolorImage = 0;
	MonocolorOffsetX = 2;
	MonocolorOffsetY = 2;
	MonocolorSpacingX = 4;
	MonocolorSpacingY = 4;
	ImageHeight = 480;
	ImageWidth = 640;
	ImageWrap = ImageWidth;
	ImageCenterX = ImageWidth/2;
	ImageCenterY = ImageHeight/2;

	SetupWindow();

	fPrintf = 0;
	fTimer = 0;
	fResult = 0;
	fResultNotify = 0;
	fWatchDog = 0;
	fStatus = 0;
	fProgress = 0;
}

int GetLastDecoderError( void ) {
	int returnCode = DEC_ERR_NONE;
#if (DECODER_ID)
	returnCode = SI_GetLastDecoderError();
#endif
	return returnCode;
}

/* Postamble */
#ifdef __cplusplus
}
#endif  /* __cplusplus */
