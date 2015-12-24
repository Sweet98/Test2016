//======================================================================================
// RapidScanInterface.c
//======================================================================================
// $Source: Source/RapidScanInterface.c $
// $Revision: 1.23 $
// $Date: 2011/06/14 17:20:43EDT $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Module Interface Description:
//      This file contains the interface definitions to create and interact with 
//		the RapidScan decoder. 
//
//---------------------------------------------------------------------------



/* Decoder Functions
 *	These are the functions used to communicate to the decoder.
 */

#include "DecoderInterface.h"
#include "RapidScanInterface.h"
#include "rapidscan.h"

#define RSI_DEBUG 0

#if RSI_DEBUG
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

//temporary until callbacks are straight
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Local control settings
 *  These are settings that the decoder core does not handle, but Adaptus decoder core does,
 *  so they need to be handled in this portion either until the decoder core does handle these
 *  things or forever. 
 */

static DecoderResult localTempResult;	//local result that is passed to the next level after some basic
								//checks and info filled in
#define RSI_LTR localTempResult

static RS_Image localImage;	//local image passed into the decoder. Most of these values will stay
											// the same for every decode, so having one will save setup time
#define RS_Img localImage

/* Disable Symbology Groups
 *	This functionality saves off previous settings of symbologies and overrides any setting. It also disables hidden
 *	enables to read menu commands while that symbology is disabled (if any). 
 */

//max number of symbologies to save. Needs to change per interface depending on the amount of groups to save off.
#define MAX_DISABLED	20			
static int DisableSymbologyGroup;	//specifies the group of symbologies to disable

//stores the previous setting. First location is symbology identifier tag, second is setting to restore.
//this way there does not need to be a particular order to the storage and have settings get crossed if things aren't
//in the correct order. The symbology identification tag can be anything as long as for each symbology it's unique for the group
static int DisabledGroupSettings[MAX_DISABLED][2];	

/* END Disable Symbology Groups */

/* Local function declarations. 
	This is just so we don't have to worry about the order they are defined and they can be grouped logically
*/
static int FillResultInfo(RS_DecodeResult *res);
/* END Local function declarations */

//temporary blocking of results while addendas are enabled.
static int UPCAAddendaEnabled;
static int UPCEAddendaEnabled;
static int EAN13AddendaEnabled;
static int EAN8AddendaEnabled;

/* Callbacks
 *	This section has the defines for the callbacks. Callback functions passed 
 *	into this interface are saved (where applicable) so that a this level, we can 
 *	do things that need to be done before calling the layer above.
*/
typedef  void (*fRsiPrintf_t)(void);
typedef  int (*fRsiTimer_t)(void);
typedef  void (*fRsiResult_t)(int,DecoderResult *);
typedef  void (*fRsiResultNotify_t)(int);
typedef  void (*fRsiWatchDog_t)(void);
typedef  void (*fRsiStatus_t)(int,int);
typedef  void (*fRsiProgress_t)(int);


// should be static eventualy

fRsiPrintf_t fRsiPrintf;
fRsiTimer_t fRsiTimer;
fRsiResult_t fRsiResult;
fRsiResultNotify_t fRsiResultNotify;
fRsiWatchDog_t fRsiWatchDog;
fRsiStatus_t fRsiStatus;
fRsiProgress_t fRsiProgress;

#define RSIPRINTF	fRsiPrintf
#define RSITIMER	fRsiTimer
#define RSIRESULT	fRsiResult
#define RSINOTIFY	fRsiResultNotify
#define RSIWATCHDOG	fRsiWatchDog
#define RSISTATUS	fRsiStatus
#define RSIPROGRESS	fRsiProgress
//#define RSIFREE	free
//#define RSIMALLOC	malloc

//Local callback fucntions
void RSI_ResultCallback( RS_DecodeResult *res ) {

	Pt("Rapid result\r\n");
	if( FillResultInfo(res) ) {

		//Everything is good, call next level
		if( RSIRESULT ) {
			RSIRESULT(0, &RSI_LTR);
		}
	}
}

void RSI_StatusCallback( int status ) {

	//Everything is good, call next level
	if( RSISTATUS ) {
		RSISTATUS(0, status);
	}
}

void RSI_ProgressCallback( void ) {

	//Everything is good, call next level
	if( RSIPROGRESS ) {
		RSIPROGRESS(0);
	}
}

/* END Callbacks */


static int FillResultInfo( RS_DecodeResult *res ) {

	int tempInt1=0;
	int tempInt2=0;
	int tempInt3=0; //temp ints to be used for various things

	//fill in the basics
	memcpy(RSI_LTR.Data,res->message,res->length);
	RSI_LTR.Length = res->length;
	RSI_LTR.AIMCodeLetter = res->AIMcodeLetter;
	RSI_LTR.AIMModifier = res->AIMcodeModifier;
	RSI_LTR.ProgrammingCode = res->menuFlag;
	RSI_LTR.HHPCodeID = res->HHPcodeID;
	RSI_LTR.DecodeType = DEC_TYPE_FLD;

	for (int i=0;i<4;i++)
	{
		RSI_LTR.IQImageInfo.bounds.corners[i].x = RSI_LTR.MainBounds.corners[i].x = res->IQImgInfo.Corners[i].x;
		RSI_LTR.IQImageInfo.bounds.corners[i].y = RSI_LTR.MainBounds.corners[i].y = res->IQImgInfo.Corners[i].y;
	}
	//Calculate center of barcode
	RSI_LTR.GraphicalCenter.x = RSI_LTR.IQImageInfo.bounds.corners[0].x;
	RSI_LTR.GraphicalCenter.y = RSI_LTR.IQImageInfo.bounds.corners[0].y;
	
	for(tempInt1 = 1; tempInt1 < 4; tempInt1++)
	{
		RSI_LTR.GraphicalCenter.x += RSI_LTR.IQImageInfo.bounds.corners[tempInt1].x;
		RSI_LTR.GraphicalCenter.y += RSI_LTR.IQImageInfo.bounds.corners[tempInt1].y;
	}
	RSI_LTR.GraphicalCenter.x >>= 2;
	RSI_LTR.GraphicalCenter.y >>= 2;

	RSI_LTR.IQImageInfo.d_corner = res->IQImgInfo.D_Corner;

	switch( RSI_LTR.HHPCodeID ) {
		case DEC_HHP_CODE_ID_CODE39:				//Code39
			RSI_LTR.Symbology = DEC_SYMBOLOGY_CODE39;
			RSI_LTR.Modifier = 0;
			RSI_LTR.SymbologyEx = 0;
			RSI_LTR.ModifierEx = 0;
			break;
		case DEC_HHP_CODE_ID_TELEPEN:			//Telepen
			RSI_LTR.Symbology = 0;
			RSI_LTR.Modifier = 0;
			RSI_LTR.SymbologyEx = DEC_SYMBOLOGY_EX_TELEPEN;
			RSI_LTR.Modifier = 0;
			break;
		case DEC_HHP_CODE_ID_GS1_128:
		case DEC_HHP_CODE_ID_CODE128:
			RSI_LTR.Symbology = DEC_SYMBOLOGY_CODE128;
			RSI_LTR.Modifier = RSI_LTR.AIMModifier;
			RSI_LTR.SymbologyEx = 0;
			RSI_LTR.ModifierEx = 0;
			break;
		/*'D' = Code1, N/A*/
		case DEC_HHP_CODE_ID_UPCA: //UPCA
			//don't return any results while this addenda is enabled until FLD can read addendas
			if( UPCAAddendaEnabled ) return DEC_FAILURE;
			RSI_LTR.Symbology = DEC_SYMBOLOGY_UPC;
			RSI_LTR.SymbologyEx = 0;
			RSI_LTR.ModifierEx = 0;

			if( RSI_LTR.AIMModifier == '3' ) { // Addenda
				RS_Get(DEC_UPCA_CHECK_DIGIT_TRANSMIT,(void *)&tempInt1);
				RS_Get(DEC_UPCA_NUMBER_SYSTEM_TRANSMIT,(void *)&tempInt2);

				if( (RSI_LTR.Length-tempInt1-tempInt2) == 12 ) { //2 char
					RSI_LTR.Modifier = 'a';
				}
				else { //5 char
					RSI_LTR.Modifier = '0'; 
				}
			}
			else { //No addenda
				RSI_LTR.Modifier = 'A';
			}
			break;
		case DEC_HHP_CODE_ID_UPCE: //UPCE
			//don't return any results while this addenda is enabled until FLD can read addendas
			if( UPCEAddendaEnabled ) return DEC_FAILURE;
			RSI_LTR.Symbology = DEC_SYMBOLOGY_UPC;
			RSI_LTR.SymbologyEx = 0;
			RSI_LTR.ModifierEx = 0;
			
			if( RSI_LTR.Data[0] == '0' ) { //UPCE-0
				tempInt3 = 0;
			}
			else {	//UPCE-1
				tempInt3 = 1;
			}

			if( RSI_LTR.AIMModifier == '3' ) { // Addenda
				RS_Get(DEC_UPCE_CHECK_DIGIT_TRANSMIT,(void *)&tempInt1);
				RS_Get(DEC_UPCE_NUMBER_SYSTEM_TRANSMIT,(void *)&tempInt2);

				if( (RSI_LTR.Length-tempInt1-tempInt2) == 12 ) { //2 char
					RSI_LTR.Modifier = 'b'+tempInt3;
				}
				else { //5 char
					RSI_LTR.Modifier = '1'+tempInt3; 
				}
			}
			else { //No addenda
				RSI_LTR.Modifier = 'B'+tempInt3;
			}
			break;
		case DEC_HHP_CODE_ID_EAN8: //EAN8
			//don't return any results while this addenda is enabled until FLD can read addendas
			if( EAN8AddendaEnabled ) return DEC_FAILURE;
			RSI_LTR.Symbology = DEC_SYMBOLOGY_UPC;
			RSI_LTR.SymbologyEx = 0;
			RSI_LTR.ModifierEx = 0;

			if( RSI_LTR.AIMModifier == '3' ) { // Addenda

				if( RSI_LTR.Length == 10 ) { //2 char
					RSI_LTR.Modifier = 'd';
				}
				else { //5 char
					RSI_LTR.Modifier = '3'; 
				}
			}
			else { //No addenda
				RSI_LTR.Modifier = 'D';
			}
			break;
		case DEC_HHP_CODE_ID_EAN13: //EAN13
			//don't return any results while this addenda is enabled until FLD can read addendas
			if( EAN13AddendaEnabled ) return DEC_FAILURE;
			RSI_LTR.Symbology = DEC_SYMBOLOGY_UPC;
			RSI_LTR.SymbologyEx = 0;
			RSI_LTR.ModifierEx = 0;

			if( RSI_LTR.AIMModifier == '3' ) { // Addenda

				if( RSI_LTR.Length == 14 ) { //2 char
					RSI_LTR.Modifier = 'e';
				}
				else { //5 char
					RSI_LTR.Modifier = '4'; 
				}
			}
			else { //No addenda
				RSI_LTR.Modifier = 'E';
			}
			break;

		case DEC_HHP_CODE_ID_CODABAR:				//Codabar
			RSI_LTR.Symbology = DEC_SYMBOLOGY_CODABAR;
			RSI_LTR.Modifier = 0;
			RSI_LTR.SymbologyEx = 0;
			RSI_LTR.ModifierEx = 0;
			break;
		case DEC_HHP_CODE_ID_CODE93:				//Code93
			RSI_LTR.Symbology = DEC_SYMBOLOGY_CODE93;
			RSI_LTR.Modifier = 0;
			RSI_LTR.SymbologyEx = 0;
			RSI_LTR.ModifierEx = 0;
			break;
		/*'H' = Code11, N/A*/
		case DEC_HHP_CODE_ID_I25:				//Interleaved25
			RSI_LTR.Symbology = DEC_SYMBOLOGY_I25;
			RSI_LTR.Modifier = 0;
			RSI_LTR.SymbologyEx = 0;
			RSI_LTR.ModifierEx = 0;
			break;
		/*'J' = Reserved*/
		/*'K' = Code16K, N/A*/
		case DEC_HHP_CODE_ID_PDF417:
			RSI_LTR.Symbology = DEC_SYMBOLOGY_PDF;
			RSI_LTR.Modifier = 0;
			RSI_LTR.SymbologyEx = 0;
			RSI_LTR.ModifierEx = 0;
			break;
		case DEC_HHP_CODE_ID_MICROPDF:
			RSI_LTR.Symbology = DEC_SYMBOLOGY_PDF;
			RSI_LTR.Modifier = 0;
			RSI_LTR.SymbologyEx = 0;
			RSI_LTR.ModifierEx = 1;
			break;
		/*'M' = MSI, RapidScan does not support */
			/* Swift does not support Plessey */
		/*'N' = Anker, N/A*/
		/*'O' = Codablock, RapidScan does not support */
		/*'P' = Plessey, See SD_CONST_MSIP*/
		/*'Q' = QR, RapidScan does not support */
		/*'R' = Straight25 2bar (IATA), RapidScan does not support */
		/*'S' = Straing25 3bar (Industrial), RapidScan does not support */
		/*'T' = Code49, N/A*/
		/*'U' = Maxicode, RapidScan does not support */
		/*'V' = Reserved*/
		/*'W' = Reserved*/
		/*'X' = Other barcode, used with symbols who don't have a given letter*/
		/*'Y' = System Expansion*/
		/*'Z' = Non-bar Code*/
		/*'a' = Reserved*/
		/*'b' = Reserved*/
		/*'c' = Channel Code*/
		/*'d' = DataMatrix, RapidScan does not support */
		case DEC_HHP_CODE_ID_GS1_DATABAR:
		case DEC_HHP_CODE_ID_GS1_DATABAR_LIM:
		case DEC_HHP_CODE_ID_GS1_DATABAR_EXP:			
			RSI_LTR.Symbology = DEC_SYMBOLOGY_RSS+RSI_LTR.AIMModifier;
			RSI_LTR.AIMModifier = '0';
			RSI_LTR.Modifier = 0;
			RSI_LTR.SymbologyEx = 0;
			RSI_LTR.ModifierEx = 1;
			break;		
		/*'f' = Reserved*/
		/*'g' = Reserved*/
		/*'h' = Reserved*/
		/*'i' = Reserved*/
		/*'j' = Reserved*/
		/*'k' = Reserved*/
		/*'l' = Reserved*/
		/*'m' = Reserved*/
		/*'n' = Reserved*/
		/*'o' = OCR, RapidScan does not support*/
		/*'p' = Posicode, N/A*/
		/*'s' = SuperCode, N/A*/
		/*'t' = Reserved*/
		/*'u' = Reserved*/
		/*'v' = UltraCode, N/A*/
		/*'w' = Reserved*/
		/*'x' = Reserved*/
		/*'y' = Reserved*/
		/*'z' = Aztec, RapidScan does not support */
		
	}//switch(RSI_LTR.Symbology)
	return DEC_SUCCESS;

}//FillResultInfo()


//______________________________________________________________
/* Visible Functions */
int RSI_InitDecoder( int width, int height, int wrap ) {
	int create = (int)RS_Create(width,height,(void *)RSI_ResultCallback);
	//Set up default values for the local image
	RS_Img.height = height;
	RS_Img.width = width;
	RS_Img.arrayWidth = wrap;
	RS_Img.whiteHigh = DEC_TRUE;
	RS_Img.color = 0;

	RS_Set( RS_DEC_RSS_MODS_SYMBOLOGY, (void *)1);

	return create;
}
void RSI_DestroyDecoder( void ) {
	return RS_Destroy();
}

void RSI_ReportDecoderVersion( char *reportString ) {
	RS_ReportDecoderVersion(reportString);
}

int RSI_InstallCallback( unsigned int tag, void *function ) {
	
	switch( tag ) {
		case CB_PRINTF:
			RSIPRINTF = (fRsiPrintf_t)function;
			break;
		case CB_TIMER:
			RSITIMER = (fRsiTimer_t)function;
			RS_Set(tag,function);
			break;
		case CB_RESULT:
			RSIRESULT = (fRsiResult_t)function;
			break;
		case CB_RESULT_NOTIFY:
			RSINOTIFY = (fRsiResultNotify_t)function;
			break;
		case CB_WATCHDOG:
			RSIWATCHDOG = (fRsiWatchDog_t)function;
			break;
		case CB_STATUS:
			RSISTATUS = (fRsiStatus_t)function;
			break;
		case CB_PROGRESS:
			RSIPROGRESS = (fRsiProgress_t)function;
			break;
		default:
			return DEC_FAILURE; //not a valid tag, return error
	}
	return DEC_SUCCESS; //found a good tag
}

int RSI_Decode( void *image, int decodeImage ) {

	if (decodeImage) {
		RS_Img.imageType   = 14;  // ... for FAST Decoding ONLY!
	}
	else {
		RS_Img.imageType   = 15;  // ... for Energy Sampling ONLY!
	}

	RS_Img.array       = (MxPixel *)image;

	return RS_Decode((RS_Image *)&RS_Img);

}

int RSI_StopDecode( void ) {
	return DEC_SUCCESS;
}

int RSI_DecodeGet( unsigned int tag, void *data ) {
	
	int returnCode = DEC_SUCCESS;
	
	switch(tag) {
		case DEC_IMAGE_HEIGHT:
			memcpy( data, &RS_Img.height, sizeof(int) ); break;
		case DEC_IMAGE_WIDTH:
			memcpy( data, &RS_Img.width, sizeof(int) ); break;
		case DEC_IMAGE_LINE_DELTA:
			memcpy( data, &RS_Img.arrayWidth, sizeof(int) ); break;
		case DEC_VIDEO_REVERSE_ENABLED:
			memcpy( data, &RS_Img.whiteHigh, sizeof(int)); break;
		case DEC_MONOCOLOR_ENABLED:
			memcpy( data, &RS_Img.color, sizeof(int) ); break;
		case DEC_C128_FNC1_SUBSTITUTE:
			returnCode = RS_Get( RS_DEC_C128_FNC1_SUBSTITUTE, data ); break;
		case DEC_C128_FNC_TRANSMIT:
			returnCode = RS_Get( RS_DEC_C128_FNC_TRANSMIT, data ); break;
		case DEC_CODABAR_CHECK_DIGIT_MODE:
			returnCode = RS_Get( RS_DEC_CODABAR_CHECK_DIGIT_MODE, data ); break;
		case DEC_CODABAR_CONCAT_ENABLED:
			returnCode = RS_Get( RS_DEC_CODABAR_CONCAT_ENABLED, data ); break;
		case DEC_CODABAR_ENABLED:
			returnCode = RS_Get( RS_DEC_CODABAR_ENABLED, data ); break;
		case DEC_CODABAR_MAX_LENGTH:
			returnCode = RS_Get( RS_DEC_CODABAR_MAX_LENGTH, data ); break;
		case DEC_CODABAR_MIN_LENGTH:
			returnCode = RS_Get( RS_DEC_CODABAR_MIN_LENGTH, data ); break;
		case DEC_CODE128_ENABLED:
			returnCode = RS_Get( RS_DEC_CODE128_ENABLED, data ); break;
		case DEC_CODE128_MAX_LENGTH:
			returnCode = RS_Get( RS_DEC_CODE128_MAX_LENGTH, data ); break;
		case DEC_CODE128_MIN_LENGTH:
			returnCode = RS_Get( RS_DEC_CODE128_MIN_LENGTH, data ); break;
		case DEC_CODABLOCK_A_ENABLED:
			returnCode = RS_Get( RS_DEC_CODABLOCK_A_ENABLED, data ); break;
		case DEC_CODABLOCK_F_ENABLED:
			returnCode = RS_Get( RS_DEC_CODABLOCK_F_ENABLED, data ); break;
		case DEC_GS1_128_ENABLED:
			returnCode = RS_Get( RS_DEC_GS1_128_ENABLED, data ); break;
		case DEC_GS1_128_MAX_LENGTH:
			returnCode = RS_Get( RS_DEC_GS1_128_MAX_LENGTH, data ); break;
		case DEC_GS1_128_MIN_LENGTH:
			returnCode = RS_Get( RS_DEC_GS1_128_MIN_LENGTH, data ); break;
		case DEC_C128_ISBT_ENABLED:
			returnCode = RS_Get( RS_DEC_C128_ISBT_ENABLED, data ); break;
		case DEC_CODE39_BASE32_ENABLED:
			returnCode = RS_Get( RS_DEC_CODE39_BASE32_ENABLED, data ); break;
		case DEC_CODE39_CHECK_DIGIT_MODE:
			returnCode = RS_Get( RS_DEC_CODE39_CHECK_DIGIT_MODE, data ); break;
		case DEC_CODE39_ENABLED:
			returnCode = RS_Get( RS_DEC_CODE39_ENABLED, data ); break;
		case DEC_CODE39_FULL_ASCII_ENABLED:
			returnCode = RS_Get( RS_DEC_CODE39_FULL_ASCII_ENABLED, data ); break;
		case DEC_CODE39_APPEND_ENABLED:
			returnCode = RS_Get( RS_DEC_CODE39_APPEND_ENABLED, data ); break;
		case DEC_CODE39_MAX_LENGTH:
			returnCode = RS_Get( RS_DEC_CODE39_MAX_LENGTH, data ); break;
		case DEC_CODE39_MIN_LENGTH:
			returnCode = RS_Get( RS_DEC_CODE39_MIN_LENGTH, data ); break;
		case DEC_CODE39_START_STOP_TRANSMIT:
			returnCode = RS_Get( RS_DEC_CODE39_START_STOP_TRANSMIT, data ); break;
		case DEC_TLC39_ENABLED:
			returnCode = RS_Get( RS_DEC_TLC39_ENABLED, data ); break;
		case DEC_COMPOSITE_WITH_UPC_ENABLED:
			returnCode = RS_Get( RS_DEC_COMPOSITE_WITH_UPC_ENABLED, data ); break;
		case DEC_COMPOSITE_ENABLED:
			returnCode = RS_Get( RS_DEC_COMPOSITE_ENABLED, data ); break;
		case DEC_COUPON_CODE_MODE:
			returnCode = RS_Get( RS_DEC_COUPON_CODE_MODE, data ); break;
		case DEC_EAN13_2CHAR_ADDENDA_ENABLED:
			returnCode = RS_Get( RS_DEC_EAN13_2CHAR_ADDENDA_ENABLED, data ); break;
		case DEC_EAN13_5CHAR_ADDENDA_ENABLED:
			returnCode = RS_Get( RS_DEC_EAN13_5CHAR_ADDENDA_ENABLED, data ); break;
		case DEC_EAN13_ADDENDA_REQUIRED:
			returnCode = RS_Get( RS_DEC_EAN13_ADDENDA_REQUIRED, data ); break;
		case DEC_EAN13_ADDENDA_SEPARATOR:
			returnCode = RS_Get( RS_DEC_EAN13_ADDENDA_SEPARATOR, data ); break;
		case DEC_EAN13_CHECK_DIGIT_TRANSMIT:
			returnCode = RS_Get( RS_DEC_EAN13_CHECK_DIGIT_TRANSMIT, data ); break;
		case DEC_EAN13_ENABLED:
			returnCode = RS_Get( RS_DEC_EAN13_ENABLED, data ); break;
		case DEC_EAN13_ISBN_ENABLED:
			returnCode = RS_Get( RS_DEC_EAN13_ISBN_ENABLED, data ); break;
		case DEC_EAN8_2CHAR_ADDENDA_ENABLED:
			returnCode = RS_Get( RS_DEC_EAN8_2CHAR_ADDENDA_ENABLED, data ); break;
		case DEC_EAN8_5CHAR_ADDENDA_ENABLED:
			returnCode = RS_Get( RS_DEC_EAN8_5CHAR_ADDENDA_ENABLED, data ); break;
		case DEC_EAN8_ADDENDA_REQUIRED:
			returnCode = RS_Get( RS_DEC_EAN8_ADDENDA_REQUIRED, data ); break;
		case DEC_EAN8_ADDENDA_SEPARATOR:
			returnCode = RS_Get( RS_DEC_EAN8_ADDENDA_SEPARATOR, data ); break;
		case DEC_EAN8_CHECK_DIGIT_TRANSMIT:
			returnCode = RS_Get( RS_DEC_EAN8_CHECK_DIGIT_TRANSMIT, data ); break;
		case DEC_EAN8_ENABLED:
			returnCode = RS_Get( RS_DEC_EAN8_ENABLED, data ); break;
		case DEC_GET_ENERGY: 
			returnCode = RS_Get( RS_DEC_GET_ENERGY, data ); break;
		case DEC_SECURITY_LEVEL:
			returnCode = RS_Get( RS_DEC_SECURITY_LEVEL, data ); break;
		case DEC_I25_CHECK_DIGIT_MODE:
			returnCode = RS_Get( RS_DEC_I25_CHECK_DIGIT_MODE, data ); break;
		case DEC_I25_ENABLED:
			returnCode = RS_Get( RS_DEC_I25_ENABLED, data ); break;
		case DEC_I25_MAX_LENGTH:
			returnCode = RS_Get( RS_DEC_I25_MAX_LENGTH, data ); break;
		case DEC_I25_MIN_LENGTH:
			returnCode = RS_Get( RS_DEC_I25_MIN_LENGTH, data ); break;
		case DEC_RSS_14_ENABLED:
			returnCode = RS_Get( RS_DEC_RSS_14_ENABLED, data ); break;
		case DEC_RSS_EXPANDED_ENABLED:
			returnCode = RS_Get( RS_DEC_RSS_EXPANDED_ENABLED, data ); break;
		case DEC_RSS_EXPANDED_MAX_LENGTH:
			returnCode = RS_Get( RS_DEC_RSS_EXPANDED_MAX_LENGTH, data ); break;
		case DEC_RSS_EXPANDED_MIN_LENGTH:
			returnCode = RS_Get( RS_DEC_RSS_EXPANDED_MIN_LENGTH, data ); break;
		case DEC_RSS_LIMITED_ENABLED:
			returnCode = RS_Get( RS_DEC_RSS_LIMITED_ENABLED, data ); break;
		case DEC_UPCA_2CHAR_ADDENDA_ENABLED:
			returnCode = RS_Get( RS_DEC_UPCA_2CHAR_ADDENDA_ENABLED, data ); break;
		case DEC_UPCA_5CHAR_ADDENDA_ENABLED:
			returnCode = RS_Get( RS_DEC_UPCA_5CHAR_ADDENDA_ENABLED, data ); break;
		case DEC_UPCA_ADDENDA_REQUIRED:
			returnCode = RS_Get( RS_DEC_UPCA_ADDENDA_REQUIRED, data ); break;
		case DEC_UPCA_ADDENDA_SEPARATOR:
			returnCode = RS_Get( RS_DEC_UPCA_ADDENDA_SEPARATOR, data ); break;
		case DEC_UPCA_CHECK_DIGIT_TRANSMIT:
			returnCode = RS_Get( RS_DEC_UPCA_CHECK_DIGIT_TRANSMIT, data ); break;
		case DEC_UPCA_ENABLED:
			returnCode = RS_Get( RS_DEC_UPCA_ENABLED, data ); break;
		case DEC_UPCA_NUMBER_SYSTEM_TRANSMIT:
			returnCode = RS_Get( RS_DEC_UPCA_NUMBER_SYSTEM_TRANSMIT, data ); break;
		case DEC_UPCE0_ENABLED:
			returnCode = RS_Get( RS_DEC_UPCE0_ENABLED, data ); break;
		case DEC_UPCE1_ENABLED:
			returnCode = RS_Get( RS_DEC_UPCE1_ENABLED, data ); break;
		case DEC_UPCE_2CHAR_ADDENDA_ENABLED:
			returnCode = RS_Get( RS_DEC_UPCE_2CHAR_ADDENDA_ENABLED, data ); break; 
		case DEC_UPCE_5CHAR_ADDENDA_ENABLED:
			returnCode = RS_Get( RS_DEC_UPCE_5CHAR_ADDENDA_ENABLED, data ); break;
		case DEC_UPCE_ADDENDA_REQUIRED:
			returnCode = RS_Get( RS_DEC_UPCE_ADDENDA_REQUIRED, data ); break;
		case DEC_UPCE_ADDENDA_SEPARATOR:
			returnCode = RS_Get( RS_DEC_UPCE_ADDENDA_SEPARATOR, data ); break;
		case DEC_UPCE_CHECK_DIGIT_TRANSMIT:
			returnCode = RS_Get( RS_DEC_UPCE_CHECK_DIGIT_TRANSMIT, data ); break;
		case DEC_UPCE_EXPAND:
			returnCode = RS_Get( RS_DEC_UPCE_EXPAND, data ); break; 
		case DEC_UPCE_NUMBER_SYSTEM_TRANSMIT:
			returnCode = RS_Get( RS_DEC_UPCE_NUMBER_SYSTEM_TRANSMIT, data ); break;
		case DEC_IMAGE_CENTER_X:
			returnCode = RS_Get( RS_DEC_SEARCH_CENTER_X, data ); break;
		case DEC_IMAGE_CENTER_Y:
			returnCode = RS_Get( RS_DEC_SEARCH_CENTER_Y, data ); break;
		case DEC_DISABLE_SYMBOLOGY_GROUP:
			memcpy( data, &DisableSymbologyGroup, sizeof(int) ); break;

		default:
			returnCode = RS_Get( tag, data );break;
	}
	return returnCode;
}

int RSI_DecodeSet( unsigned int tag, void *data ) {
	int returnCode = DEC_SUCCESS;
	int symb;
	
	switch(tag) {
		case DEC_IMAGE_HEIGHT:
			RS_Img.height = (int)data; break;
		case DEC_IMAGE_WIDTH:
			RS_Img.width = (int)data; break;
		case DEC_IMAGE_LINE_DELTA:
			RS_Img.arrayWidth = (int)data; break;
		case DEC_VIDEO_REVERSE_ENABLED:
			RS_Img.whiteHigh = !((int)data); break;
		case DEC_MONOCOLOR_ENABLED:
			RS_Img.color = (int)data; break;
		case DEC_C128_FNC1_SUBSTITUTE:
			returnCode = RS_Set( RS_DEC_C128_FNC1_SUBSTITUTE, data ); break;
		case DEC_C128_FNC_TRANSMIT:
			returnCode = RS_Set( RS_DEC_C128_FNC_TRANSMIT, data ); break;
		case DEC_CODABAR_CHECK_DIGIT_MODE:
			returnCode = RS_Set( RS_DEC_CODABAR_CHECK_DIGIT_MODE, data ); break;
		case DEC_CODABAR_CONCAT_ENABLED:
			returnCode = RS_Set( RS_DEC_CODABAR_CONCAT_ENABLED, data ); break;
		case DEC_CODABAR_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == RS_DEC_CODABAR_ENABLED ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = RS_Set( RS_DEC_CODABAR_ENABLED, data ); 
			}
			break;
		case DEC_CODABAR_MAX_LENGTH:
			returnCode = RS_Set( RS_DEC_CODABAR_MAX_LENGTH, data ); break;
		case DEC_CODABAR_MIN_LENGTH:
			returnCode = RS_Set( RS_DEC_CODABAR_MIN_LENGTH, data ); break;
		case DEC_CODE128_ENABLED:
			if( DisableSymbologyGroup&ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == RS_DEC_CODE128_ENABLED ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = RS_Set( RS_DEC_CODE128_ENABLED, data ); 
			}
			break;
		case DEC_CODE128_MAX_LENGTH:
			returnCode = RS_Set( RS_DEC_CODE128_MAX_LENGTH, data ); break;
		case DEC_CODE128_MIN_LENGTH:
			returnCode = RS_Set( RS_DEC_CODE128_MIN_LENGTH, data ); break;
		case DEC_CODABLOCK_A_ENABLED:
			returnCode = RS_Set( RS_DEC_CODABLOCK_A_ENABLED, data ); break;
		case DEC_CODABLOCK_F_ENABLED:
			returnCode = RS_Set( RS_DEC_CODABLOCK_F_ENABLED, data ); break;
		case DEC_GS1_128_ENABLED:
			if( DisableSymbologyGroup&ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == RS_DEC_GS1_128_ENABLED ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = RS_Set( RS_DEC_GS1_128_ENABLED, data );
			}
			break;
		case DEC_GS1_128_MAX_LENGTH:
			returnCode = RS_Set( RS_DEC_GS1_128_MAX_LENGTH, data ); break;
		case DEC_GS1_128_MIN_LENGTH:
			returnCode = RS_Set( RS_DEC_GS1_128_MIN_LENGTH, data ); break;
		case DEC_C128_ISBT_ENABLED:
			returnCode = RS_Set( RS_DEC_C128_ISBT_ENABLED, data ); break;
		case DEC_CODE39_BASE32_ENABLED:
			returnCode = RS_Set( RS_DEC_CODE39_BASE32_ENABLED, data ); break;
		case DEC_CODE39_CHECK_DIGIT_MODE:
			returnCode = RS_Set( RS_DEC_CODE39_CHECK_DIGIT_MODE, data ); break;
		case DEC_CODE39_ENABLED:
			if( DisableSymbologyGroup&ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == RS_DEC_CODE39_ENABLED ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = RS_Set( RS_DEC_CODE39_ENABLED, data ); 
			}
			break;
		case DEC_CODE39_FULL_ASCII_ENABLED:
			returnCode = RS_Set( RS_DEC_CODE39_FULL_ASCII_ENABLED, data ); break;
		case DEC_CODE39_APPEND_ENABLED:
			returnCode = RS_Set( RS_DEC_CODE39_APPEND_ENABLED, data ); break;
		case DEC_CODE39_MAX_LENGTH:
			returnCode = RS_Set( RS_DEC_CODE39_MAX_LENGTH, data ); break;
		case DEC_CODE39_MIN_LENGTH:
			returnCode = RS_Set( RS_DEC_CODE39_MIN_LENGTH, data ); break;
		case DEC_CODE39_START_STOP_TRANSMIT:
			returnCode = RS_Set( RS_DEC_CODE39_START_STOP_TRANSMIT, data ); break;
		case DEC_TLC39_ENABLED:
			returnCode = RS_Set( RS_DEC_TLC39_ENABLED, data ); break;
		case DEC_COMPOSITE_WITH_UPC_ENABLED:
			returnCode = RS_Set( RS_DEC_COMPOSITE_WITH_UPC_ENABLED, data ); break;
		case DEC_COMPOSITE_ENABLED:
			returnCode = RS_Set( RS_DEC_COMPOSITE_ENABLED, data ); break;
		case DEC_COUPON_CODE_MODE:
			returnCode = RS_Set( RS_DEC_COUPON_CODE_MODE, data ); break;
		case DEC_EAN13_2CHAR_ADDENDA_ENABLED:
			EAN13AddendaEnabled |= ((int)data);
			returnCode = RS_Set( RS_DEC_EAN13_2CHAR_ADDENDA_ENABLED, data ); break;
		case DEC_EAN13_5CHAR_ADDENDA_ENABLED:
			EAN13AddendaEnabled |= ((int)data)<<1;
			returnCode = RS_Set( RS_DEC_EAN13_5CHAR_ADDENDA_ENABLED, data ); break;
		case DEC_EAN13_ADDENDA_REQUIRED:
			returnCode = RS_Set( RS_DEC_EAN13_ADDENDA_REQUIRED, data ); break;
		case DEC_EAN13_ADDENDA_SEPARATOR:
			returnCode = RS_Set( RS_DEC_EAN13_ADDENDA_SEPARATOR, data ); break;
		case DEC_EAN13_CHECK_DIGIT_TRANSMIT:
			returnCode = RS_Set( RS_DEC_EAN13_CHECK_DIGIT_TRANSMIT, data ); break;
		case DEC_EAN13_ENABLED:
			if( DisableSymbologyGroup&ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == RS_DEC_EAN13_ENABLED ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
			
				returnCode = RS_Set( RS_DEC_EAN13_ENABLED, data ); 
			}
			break;
		case DEC_EAN13_ISBN_ENABLED:
			returnCode = RS_Set( RS_DEC_EAN13_ISBN_ENABLED, data ); break;
		case DEC_EAN8_2CHAR_ADDENDA_ENABLED:
			EAN8AddendaEnabled |= ((int)data);
			returnCode = RS_Set( RS_DEC_EAN8_2CHAR_ADDENDA_ENABLED, data ); break;
		case DEC_EAN8_5CHAR_ADDENDA_ENABLED:
			EAN8AddendaEnabled |= ((int)data)<<1;
			returnCode = RS_Set( RS_DEC_EAN8_5CHAR_ADDENDA_ENABLED, data ); break;
		case DEC_EAN8_ADDENDA_REQUIRED:
			returnCode = RS_Set( RS_DEC_EAN8_ADDENDA_REQUIRED, data ); break;
		case DEC_EAN8_ADDENDA_SEPARATOR:
			returnCode = RS_Set( RS_DEC_EAN8_ADDENDA_SEPARATOR, data ); break;
		case DEC_EAN8_CHECK_DIGIT_TRANSMIT:
			returnCode = RS_Set( RS_DEC_EAN8_CHECK_DIGIT_TRANSMIT, data ); break;
		case DEC_EAN8_ENABLED:
			if( DisableSymbologyGroup&ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == RS_DEC_EAN8_ENABLED ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = RS_Set( RS_DEC_EAN8_ENABLED, data ); 
			}
			break;
		//case DEC_GET_ENERGY: NO SET
		case DEC_SECURITY_LEVEL:
			returnCode = RS_Set( RS_DEC_SECURITY_LEVEL, data ); break;
		case DEC_I25_CHECK_DIGIT_MODE:
			returnCode = RS_Set( RS_DEC_I25_CHECK_DIGIT_MODE, data ); break;
		case DEC_I25_ENABLED:
			if( DisableSymbologyGroup&ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == RS_DEC_I25_ENABLED ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = RS_Set( RS_DEC_I25_ENABLED, data ); 
			}
			break;
		case DEC_I25_MAX_LENGTH:
			returnCode = RS_Set( RS_DEC_I25_MAX_LENGTH, data ); break;
		case DEC_I25_MIN_LENGTH:
			returnCode = RS_Set( RS_DEC_I25_MIN_LENGTH, data ); break;
		case DEC_RSS_14_ENABLED:
			if( DisableSymbologyGroup&ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == RS_DEC_RSS_14_ENABLED ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = RS_Set( RS_DEC_RSS_14_ENABLED, data ); 
			}
			break;
		case DEC_RSS_EXPANDED_ENABLED:
			if( DisableSymbologyGroup&ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == RS_DEC_RSS_EXPANDED_ENABLED ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = RS_Set( RS_DEC_RSS_EXPANDED_ENABLED, data ); 
			}
			break;
		case DEC_RSS_EXPANDED_MAX_LENGTH:
			returnCode = RS_Set( RS_DEC_RSS_EXPANDED_MAX_LENGTH, data ); break;
		case DEC_RSS_EXPANDED_MIN_LENGTH:
			returnCode = RS_Set( RS_DEC_RSS_EXPANDED_MIN_LENGTH, data ); break;
		case DEC_RSS_LIMITED_ENABLED:
			if( DisableSymbologyGroup&ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == RS_DEC_RSS_LIMITED_ENABLED ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = RS_Set( RS_DEC_RSS_LIMITED_ENABLED, data ); 
			}
			break;
		case DEC_UPCA_2CHAR_ADDENDA_ENABLED:
			UPCAAddendaEnabled |= ((int)data);
			returnCode = RS_Set( RS_DEC_UPCA_2CHAR_ADDENDA_ENABLED, data ); break;
		case DEC_UPCA_5CHAR_ADDENDA_ENABLED:
			UPCAAddendaEnabled |= ((int)data)<<1;
			returnCode = RS_Set( RS_DEC_UPCA_5CHAR_ADDENDA_ENABLED, data ); break;
		case DEC_UPCA_ADDENDA_REQUIRED:
			returnCode = RS_Set( RS_DEC_UPCA_ADDENDA_REQUIRED, data ); break;
		case DEC_UPCA_ADDENDA_SEPARATOR:
			returnCode = RS_Set( RS_DEC_UPCA_ADDENDA_SEPARATOR, data ); break;
		case DEC_UPCA_CHECK_DIGIT_TRANSMIT:
			returnCode = RS_Set( RS_DEC_UPCA_CHECK_DIGIT_TRANSMIT, data ); break;
		case DEC_UPCA_ENABLED:
			if( DisableSymbologyGroup&ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == RS_DEC_UPCA_ENABLED ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = RS_Set( RS_DEC_UPCA_ENABLED, data );
			}
			break;
		case DEC_UPCA_NUMBER_SYSTEM_TRANSMIT:
			returnCode = RS_Set( RS_DEC_UPCA_NUMBER_SYSTEM_TRANSMIT, data ); break;
		case DEC_UPCE0_ENABLED:
			if( DisableSymbologyGroup&ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == RS_DEC_UPCE0_ENABLED ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = RS_Set( RS_DEC_UPCE0_ENABLED, data ); 
			}
			break;
		case DEC_UPCE1_ENABLED:
			if( DisableSymbologyGroup&ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == RS_DEC_UPCE1_ENABLED ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = RS_Set( RS_DEC_UPCE1_ENABLED, data ); 
			}
			break;
		case DEC_UPCE_2CHAR_ADDENDA_ENABLED:
			UPCEAddendaEnabled |= ((int)data);
			returnCode = RS_Set( RS_DEC_UPCE_2CHAR_ADDENDA_ENABLED, data ); break; 
		case DEC_UPCE_5CHAR_ADDENDA_ENABLED:
			UPCEAddendaEnabled |= ((int)data)<<1;
			returnCode = RS_Set( RS_DEC_UPCE_5CHAR_ADDENDA_ENABLED, data ); break;
		case DEC_UPCE_ADDENDA_REQUIRED:
			returnCode = RS_Set( RS_DEC_UPCE_ADDENDA_REQUIRED, data ); break;
		case DEC_UPCE_ADDENDA_SEPARATOR:
			returnCode = RS_Set( RS_DEC_UPCE_ADDENDA_SEPARATOR, data ); break;
		case DEC_UPCE_CHECK_DIGIT_TRANSMIT:
			returnCode = RS_Set( RS_DEC_UPCE_CHECK_DIGIT_TRANSMIT, data ); break;
		case DEC_UPCE_EXPAND:
			returnCode = RS_Set( RS_DEC_UPCE_EXPAND, data ); break; 
		case DEC_UPCE_NUMBER_SYSTEM_TRANSMIT:
			returnCode = RS_Set( RS_DEC_UPCE_NUMBER_SYSTEM_TRANSMIT, data ); break;
		case DEC_IMAGE_CENTER_X:
			returnCode = RS_Set( RS_DEC_SEARCH_CENTER_X, data ); break;
		case DEC_IMAGE_CENTER_Y:
			returnCode = RS_Set( RS_DEC_SEARCH_CENTER_Y, data ); break;
		case DEC_DISABLE_SYMBOLOGY_GROUP:
			//if turning on, get all of the settings and save them off
			if( (int)data & ONE_DIMENSIONAL_CODES && (DisableSymbologyGroup & ONE_DIMENSIONAL_CODES) == 0 ) {
				symb = 0;

				DisabledGroupSettings[symb++][0] = RS_DEC_CODABAR_ENABLED;
				DisabledGroupSettings[symb++][0] = RS_DEC_CODE39_ENABLED;
				DisabledGroupSettings[symb++][0] = RS_DEC_I25_ENABLED;
				DisabledGroupSettings[symb++][0] = RS_DEC_CODE128_ENABLED;
				DisabledGroupSettings[symb++][0] = RS_DEC_GS1_128_ENABLED;
				DisabledGroupSettings[symb++][0] = RS_DEC_UPCA_ENABLED;
				DisabledGroupSettings[symb++][0] = RS_DEC_UPCE0_ENABLED;
				DisabledGroupSettings[symb++][0] = RS_DEC_UPCE1_ENABLED;
				DisabledGroupSettings[symb++][0] = RS_DEC_EAN8_ENABLED;
				DisabledGroupSettings[symb++][0] = RS_DEC_EAN13_ENABLED;
				DisabledGroupSettings[symb++][0] = RS_DEC_RSS_14_ENABLED;
				DisabledGroupSettings[symb++][0] = RS_DEC_RSS_LIMITED_ENABLED;
				DisabledGroupSettings[symb++][0] = RS_DEC_RSS_EXPANDED_ENABLED;

				for( symb--; symb>=0; symb-- ) {
					RS_Get( DisabledGroupSettings[symb][0], (void *)&DisabledGroupSettings[symb][1] );
					RS_Set( DisabledGroupSettings[symb][0], (void *)DEC_DISABLED );
				}
			}
			//if turning off, restore the saved settings
			else if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES && ((int)data & ONE_DIMENSIONAL_CODES) == 0 ) {
				for( symb=0; (symb<MAX_DISABLED) && (DisabledGroupSettings[symb][0]); symb++ ) {
					RS_Set( DisabledGroupSettings[symb][0], (void *)DisabledGroupSettings[symb][1] );
					DisabledGroupSettings[symb][0] = DisabledGroupSettings[symb][1] = 0;
				}
			}
			
			DisableSymbologyGroup = (int)data;

			break;
		default:
			returnCode = RS_Set( tag, data );
			break;
	}

	return returnCode;
}

int RSI_DecodeReset( void ) {
	int jk;

	for( jk=0; jk<MAX_DISABLED; jk++ ) {
		DisabledGroupSettings[jk][0] = DisabledGroupSettings[jk][0] = 0;
	}

	DisableSymbologyGroup = 0;
	UPCAAddendaEnabled = 0;
	UPCEAddendaEnabled = 0;
	EAN13AddendaEnabled = 0;
	EAN8AddendaEnabled = 0;

	return RS_Reset();
}

int RSI_GetLastDecoderError( void ) {
	return 0;
}

/* Postamble */
#ifdef __cplusplus
}
#endif  /* __cplusplus */

