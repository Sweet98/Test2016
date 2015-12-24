//======================================================================================
// SwiftInterface.c
//======================================================================================
// $Source: Source/SwiftInterface.c $
// $Revision: 1.88 $
// $Date: 2011/09/22 11:16:34EDT $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Module Interface Description:
//      This file contains the interface definitions to create and interact with 
//		the Swift decoder. 
//
//---------------------------------------------------------------------------



/* Decoder Functions
 *	These are the functions used to communicate to the decoder.
 */

#include "DecoderInterface.h"
#include "SwiftInterface.h"

#define SI_DEBUG 0

#if SI_DEBUG
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
#define SI_LTR localTempResult

static int thisDecoder;

/* Individual Enables
 *  Fine-tuned control of enables. 
 *  Example: SD_PROP_UPC_ENABLED enables UPCA,E0,E1,EAN8,13, and JAN symbologies
 */
static int UPCAEnabled;
static int UPCE0Enabled;
static int UPCE1Enabled;
static int EAN8Enabled;
static int EAN13Enabled;
//This mix is used to enable the property in Swift if any 1 of these is on. 
#define UPC_ENABLE_MIX (UPCAEnabled|UPCE0Enabled|UPCE1Enabled|EAN8Enabled|EAN13Enabled)

/* Other Enableds */
static int UPCA2CharAddendaEnabled;
static int UPCA5CharAddendaEnabled;
static int UPCAAddendaRequired;
static int UPCE2CharAddendaEnabled;
static int UPCE5CharAddendaEnabled;
static int UPCEAddendaRequired;
static int EAN82CharAddendaEnabled;
static int EAN85CharAddendaEnabled;
static int EAN8AddendaRequired;
static int EAN132CharAddendaEnabled;
static int EAN135CharAddendaEnabled;
static int EAN13AddendaRequired;
//This mix is the same idea as the enable mix
#define UPC_ADDENDA_MIX (UPCA2CharAddendaEnabled|UPCA5CharAddendaEnabled|	\
						 UPCE2CharAddendaEnabled|UPCE5CharAddendaEnabled|	\
						 EAN82CharAddendaEnabled|EAN85CharAddendaEnabled|	\
						 EAN132CharAddendaEnabled|EAN135CharAddendaEnabled)	

static int AddendaTimeStamp;		//timestamp for not finding an addenda if we're supposed to look for one. Will stay across images. 
static int AddendaSearchTime;		//time to continue searching for an addenda

static int AztecEnabled;

static int RSSExpandedEnabled;
static int RSS14Enabled;
static int RSSLimitedEnabled;
#define RSS_ENABLE_MIX ( (((SD_CONST_RSS_EXP) + (SD_CONST_RSS_EXP_ST) - (2*(SD_CONST_RSS))) * RSSExpandedEnabled ) + \
						 (((SD_CONST_RSS_14) + (SD_CONST_RSS_14_ST) - (2*(SD_CONST_RSS))) * RSS14Enabled ) + \
						 (((SD_CONST_RSS_14_LIM) - (SD_CONST_RSS)) * RSSLimitedEnabled ) )

static int CompositeEnabled;
static int TLC39CompositeEnabled;
#define COMPOSITE_ENABLE_MIX ((TLC39CompositeEnabled<<1)|(CompositeEnabled))

static int Code128Enabled; //need a local because Swift does not override the enable for programming codes
static int GS1128Enabled;

static int Code128FncTransmit; //converts FNC chars to 0x8#

static int UseMLD;

static int PostalEnabled; //index of postal set in PostalGroups[] (DecoderInterface.h)
//These are the only combinations of postal codes allowed
//The value passed to DEC_POSTAL_ENABLED is the value at the beginning of each line.
unsigned int PostalOptions[POSTAL_OPTIONS] = {
/* 0 */		DEC_DISABLED,
/* 1 */		DEC_SYMBOLOGY_AUS_POST,
/* 2 */		DEC_SYMBOLOGY_EX_INFOMAIL,
/* 3 */		DEC_SYMBOLOGY_JAPAN_POST,
/* 4 */		DEC_SYMBOLOGY_KIX,
/* 5 */		DEC_SYMBOLOGY_PLANETCODE,
/* 6 */		DEC_SYMBOLOGY_POSTNET,
/* 7 */		DEC_SYMBOLOGY_ROYAL_MAIL,
/* 8 */		DEC_SYMBOLOGY_EX_INFOMAIL	+ DEC_SYMBOLOGY_ROYAL_MAIL,
/* 9 */		DEC_SYMBOLOGY_UPU_4_STATE,
/* 10 */	DEC_SYMBOLOGY_USPS_4_STATE,
/* 11 */	DEC_SYMBOLOGY_POSTNET		+ DEC_B_AND_B_FIELDS,
/* 12 */	DEC_SYMBOLOGY_PLANETCODE	+ DEC_SYMBOLOGY_POSTNET,
/* 13 */	DEC_SYMBOLOGY_PLANETCODE	+ DEC_SYMBOLOGY_UPU_4_STATE,
/* 14 */	DEC_SYMBOLOGY_POSTNET		+ DEC_SYMBOLOGY_UPU_4_STATE,
/* 15 */	DEC_SYMBOLOGY_PLANETCODE	+ DEC_SYMBOLOGY_USPS_4_STATE,
/* 16 */	DEC_SYMBOLOGY_POSTNET		+ DEC_SYMBOLOGY_USPS_4_STATE,
/* 17 */	DEC_SYMBOLOGY_UPU_4_STATE	+ DEC_SYMBOLOGY_USPS_4_STATE,
/* 18 */	DEC_SYMBOLOGY_PLANETCODE	+ DEC_SYMBOLOGY_POSTNET			+ DEC_B_AND_B_FIELDS,
/* 19 */	DEC_SYMBOLOGY_POSTNET		+ DEC_SYMBOLOGY_UPU_4_STATE		+ DEC_B_AND_B_FIELDS,
/* 20 */	DEC_SYMBOLOGY_POSTNET		+ DEC_SYMBOLOGY_USPS_4_STATE	+ DEC_B_AND_B_FIELDS,
/* 21 */	DEC_SYMBOLOGY_PLANETCODE	+ DEC_SYMBOLOGY_POSTNET			+ DEC_SYMBOLOGY_UPU_4_STATE,
/* 22 */	DEC_SYMBOLOGY_PLANETCODE	+ DEC_SYMBOLOGY_POSTNET			+ DEC_SYMBOLOGY_USPS_4_STATE,
/* 23 */	DEC_SYMBOLOGY_PLANETCODE	+ DEC_SYMBOLOGY_UPU_4_STATE		+ DEC_SYMBOLOGY_USPS_4_STATE,
/* 24 */	DEC_SYMBOLOGY_POSTNET		+ DEC_SYMBOLOGY_UPU_4_STATE		+ DEC_SYMBOLOGY_USPS_4_STATE,
/* 25 */	DEC_SYMBOLOGY_PLANETCODE	+ DEC_SYMBOLOGY_POSTNET			+ DEC_SYMBOLOGY_UPU_4_STATE		+ DEC_B_AND_B_FIELDS,
/* 26 */	DEC_SYMBOLOGY_PLANETCODE	+ DEC_SYMBOLOGY_POSTNET			+ DEC_SYMBOLOGY_USPS_4_STATE	+ DEC_B_AND_B_FIELDS,
/* 27 */	DEC_SYMBOLOGY_POSTNET		+ DEC_SYMBOLOGY_UPU_4_STATE		+ DEC_SYMBOLOGY_USPS_4_STATE	+ DEC_B_AND_B_FIELDS,
/* 28 */	DEC_SYMBOLOGY_PLANETCODE	+ DEC_SYMBOLOGY_POSTNET			+ DEC_SYMBOLOGY_UPU_4_STATE		+ DEC_SYMBOLOGY_USPS_4_STATE,
/* 29 */	DEC_SYMBOLOGY_PLANETCODE	+ DEC_SYMBOLOGY_POSTNET			+ DEC_SYMBOLOGY_UPU_4_STATE		+ DEC_SYMBOLOGY_USPS_4_STATE + DEC_B_AND_B_FIELDS,
/* 30 */	DEC_SYMBOLOGY_EX_CANADIAN_POST
};

/* Disable Symbology Groups
 *	This functionality saves off previous settings of symbologies and overrides any setting. It also disables hidden
 *	enables to read menu commands while that symbology is disabled (if any). 
 */

//max number of symbologies to save. Needs to change per interface depending on the amount of groups to save off.
#ifdef ONE_DIMENSIONAL_CODES
#define MAX_LINEARS		26
#else
#define MAX_LINEARS		0
#endif 

#ifdef STACKED_LINEAR_CODES
#define MAX_STACKED		4
#else
#define MAX_STACKED		0
#endif

#ifdef TWO_DIMENSIONAL_CODES
#define MAX_2D			0 //2D not yet implemented
#else
#define MAX_2D			0
#endif

#define MAX_DISABLED	(MAX_LINEARS+MAX_STACKED+MAX_2D+1) //max number for all possible disable groups

//stores the previous setting. First location is symbology identifier tag, second is setting to restore.
//this way there does not need to be a particular order to the storage and have settings get crossed if things aren't
//in the correct order. The symbology identification tag can be anything as long as for each symbology it's unique for the group
static int DisabledGroupSettings[MAX_DISABLED][2];	
static int DisableSymbologyGroup;	//specifies the group of symbologies to disable

/* END Disable Symbology Groups */

/* Min/max */
static int CodabarMin;
static int CodabarMax;
static int Code39Min;
static int Code39Max;
static int Interleaved25Min;
static int Interleaved25Max;
static int Interleaved25CheckDigitMode;
static int Standard250Min;
static int Standard250Max;
static int Standard251Min;
static int Standard251Max;
static int Matrix25Min;
static int Matrix25Max;
static int HongKong25Min;
static int HongKong25Max;
static int NEC25Min;
static int NEC25Max;
static int Code11Min;
static int Code11Max;
static int Code128Min;
static int Code128Max;
static int GS1128Min;
static int GS1128Max;
static int Code93Min;
static int Code93Max;
static int MSIMin;
static int MSIMax;
static int TelepenMin;
static int TelepenMax;
static int RSSExpandedMin;
static int RSSExpandedMax;
static int PDF417Min;
static int PDF417Max;
static int MicroPDFMin;
static int MicroPDFMax;
static int CodablockAMin;
static int CodablockAMax;
static int CodablockFMin;
static int CodablockFMax;
static int AztecMin;
static int AztecMax;
static int MaxicodeMin;
static int MaxicodeMax;
static int DataMatrixMin;
static int DataMatrixMax;
static int QRMin;
static int QRMax;
static int HanXinMin;
static int HanXinMax;
static int KoreaPostMin;
static int KoreaPostMax;

/* Transmit Enables */
static int UPCACheckDigitTransmit;
static int UPCANumberSystemTransmit;
static int UPCECheckDigitTransmit;
static int UPCENumberSystemTransmit;
static int EAN8CheckDigitTransmit;
static int EAN13CheckDigitTransmit;
static int Code39StartStopTransmit;
static int PostnetCheckDigitTransmit;
static int PlanetCodeCheckDigitTransmit;
static int KoreaPostCheckDigitTransmit;

/* Addenda Separator Enables */
static int UPCAAddendaSeparator;
static int UPCEAddendaSeparator;
static int EAN8AddendaSeparator;
static int EAN13AddendaSeparator;

/* Interpretation */
const char ISBNCheckChars[] = "0123456789X";
static int EAN13ISBNEnabled;
static int Code39Base32Enabled;

#define PROGRAMMINGCODE 0x20000000		//if Code128 has this bit set, there was a FNC3 seen in the code, which means a programming code

/* END Local Settings */


/* Local function declarations. 
	This is just so we don't have to worry about the order they are defined and they can be grouped logically
*/
static int CheckLength( void );
static int CheckAddenda( void );
static int FillResultInfo( int decoder );
static void FormatResult( void );
static void DeleteChar( int charIndex );
static void InsertChar( int charIndex, unsigned char newChar );
static int FindPostalMatch( unsigned int combo );
static int Base32Value (unsigned char c);
static int TryBase32( void );
/* END Local function declarations */


/* Callbacks
	This section has the defines for the callbacks. Callback functions passed 
	into this interface are saved (where applicable) so that a this level, we can 
	do things that need to be done before calling the layer above.
*/
typedef void (*fSdPrintf_t)(void);
typedef int (*fSdTimer_t)(void);
typedef void (*fSdResult_t)(int,DecoderResult*);
typedef void (*fSdResultNotify_t)(int);
typedef void (*fSdWatchDog_t)(void);
typedef void (*fSdStatus_t)(int,int);
typedef void (*fSdProgress_t)(int);


fSdPrintf_t fSdPrintf;
fSdTimer_t fSdTimer;
fSdResult_t fSdResult;
fSdResultNotify_t fSdResultNotify;
fSdWatchDog_t fSdWatchDog;
fSdStatus_t fSdStatus;
fSdProgress_t fSdProgress;

#define SIPRINTF	fSdPrintf
#define SITIMER		fSdTimer
#define SIRESULT	fSdResult
#define SINOTIFY	fSdResultNotify
#define SIWATCHDOG	fSdWatchDog
#define SISTATUS	fSdStatus
#define SIPROGRESS	fSdProgress
//#define SIFREE	free
//#define SIMALLOC	malloc

//Local callback fucntions
void SI_ResultCallback( int decoder ) {
	Pt("Swift result\r\n");

	if( FillResultInfo(decoder) == DEC_FAILURE ) {
		Pt("result info failed\r\n");
		return;
	}

	if( SI_LTR.ProgrammingCode == DEC_FALSE ) {
		if( CheckLength() == DEC_FAILURE ) {
			Pt("bad len: %d\r\n",SI_LTR.Length);
			return;
		}
	}
	if( CheckAddenda() == DEC_FAILURE ) {
		Pt("addenda failed\r\n");
		return;
	}

	FormatResult();

	//Everything is good, call next level
	if( SIRESULT ) {
		SIRESULT(decoder, &SI_LTR);
		memset((void *)&SI_LTR,0,sizeof(DecoderResult));
		AddendaTimeStamp = 0;
	}
}

void SI_StatusCallback( int decoder, int status ) {

	//Everything is good, call next level
	if( SISTATUS ) {
		SISTATUS(decoder, status);
	}
}

void SI_ProgressCallback( int decoder ) {

	//Everything is good, call next level
	if( SIPROGRESS ) {
		SIPROGRESS(decoder);
	}
}

/* END Callbacks */

static int CheckLength( void ) {
	int ret = DEC_SUCCESS;

	switch( SI_LTR.Symbology ) {
		case SD_CONST_CB:
			if( (SI_LTR.Length-2) < CodabarMin || 
				(SI_LTR.Length-2) > CodabarMax ) {
				ret = DEC_FAILURE;
			}
			break;
		case SD_CONST_C39:
			if( SI_LTR.Length < Code39Min || 
				SI_LTR.Length > Code39Max ) {
				ret = DEC_FAILURE;
			}
			break;
		case SD_CONST_I25:
			if( SI_LTR.Length < Interleaved25Min || 
				SI_LTR.Length > Interleaved25Max ) {
				ret = DEC_FAILURE;
			}
			break;
		/*Code 2 of 5 ?????????
		case SD_CONST_25:
			if( SI_LTR.Length < Standard250Min || SI_LTR.Length > Standard250Max ) {
				ret = DEC_FAILURE;
			}
			break;
		*/
		case SD_CONST_S25_2SS: //IATA 2of5
			if( SI_LTR.Length < Standard251Min || 
				SI_LTR.Length > Standard251Max ) {
				ret = DEC_FAILURE;
			}
			break;
		case SD_CONST_S25_3SS: //Straight (Industrial) 2of5
			if( SI_LTR.Length < Standard250Min || 
				SI_LTR.Length > Standard250Max ) {
				ret = DEC_FAILURE;
			}
			break;
		case SD_CONST_C11:
			if( (SI_LTR.Length) < Code11Min || 
				(SI_LTR.Length) > Code11Max ) {
				ret = DEC_FAILURE;
			}
			break;
		case SD_CONST_C128:
			if( SI_LTR.AIMModifier == '1' ) {
				if( SI_LTR.Length < GS1128Min || 
					SI_LTR.Length > GS1128Max ) {
					ret = DEC_FAILURE;
				}
			}
			else {
				if( SI_LTR.Length < Code128Min || 
					SI_LTR.Length > Code128Max ) {
					ret = DEC_FAILURE;
				}
			}
			break;
		case SD_CONST_C93:
			if( SI_LTR.Length < Code93Min || 
				SI_LTR.Length > Code93Max ) {
				ret = DEC_FAILURE;
			}
			break;
		case SD_CONST_MSIP:
			if( SI_LTR.Length < MSIMin || 
				SI_LTR.Length > MSIMax ) {
				ret = DEC_FAILURE;
			}
			break;
		case SD_CONST_RSS_EXP:
			if( SI_LTR.Length < RSSExpandedMin || 
				SI_LTR.Length > RSSExpandedMax ) {
				ret = DEC_FAILURE;
			}
			break;
		case SD_CONST_PDF:
			if( SI_LTR.HHPCodeID == DEC_HHP_CODE_ID_PDF417 ) {
				if( SI_LTR.Length < PDF417Min || 
					SI_LTR.Length > PDF417Max ) {
					ret = DEC_FAILURE;
				}
			}
			else if( SI_LTR.HHPCodeID == DEC_HHP_CODE_ID_MICROPDF ) {
				if( SI_LTR.Length < MicroPDFMin || 
					SI_LTR.Length > MicroPDFMax ) {
					ret = DEC_FAILURE;
				}
			}
			break;
		case SD_CONST_CODABLOCK_A:
			if( SI_LTR.Length < CodablockAMin || 
				SI_LTR.Length > CodablockAMax ) {
				ret = DEC_FAILURE;
			}
			break;
		case SD_CONST_CODABLOCK_F:
			if( SI_LTR.Length < CodablockFMin || 
				SI_LTR.Length > CodablockFMax ) {
				ret = DEC_FAILURE;
			}
			break;
		case SD_CONST_AZ:
			if( SI_LTR.Length < AztecMin || 
				SI_LTR.Length > AztecMax ) {
				ret = DEC_FAILURE;
			}
			break;
		case SD_CONST_MC:
			if( SI_LTR.Length < MaxicodeMin || 
				SI_LTR.Length > MaxicodeMax ) {
				ret = DEC_FAILURE;
			}
			break;
		case SD_CONST_DM:
			if( SI_LTR.Length < DataMatrixMin || 
				SI_LTR.Length > DataMatrixMax ) {
				ret = DEC_FAILURE;
			}
			break;
		case SD_CONST_QR:
			if( SI_LTR.Length < QRMin || 
				SI_LTR.Length > QRMax ) {
				ret = DEC_FAILURE;
			}
			break;
		case 0: //Extended Table
			switch( SI_LTR.SymbologyEx ) {
				case SD_CONST_KP: 
					if( (SI_LTR.Length-KoreaPostCheckDigitTransmit) < KoreaPostMin || 
						(SI_LTR.Length-KoreaPostCheckDigitTransmit) > KoreaPostMax ) {
						ret = DEC_FAILURE;
					}
					break;
				case SD_CONST_HK25:
					if( SI_LTR.Length < HongKong25Min || 
						SI_LTR.Length > HongKong25Max ) {
						ret = DEC_FAILURE;
					}
					break;
				case SD_CONST_NEC25:
					if( SI_LTR.Length < NEC25Min || 
						SI_LTR.Length > NEC25Max ) {
						ret = DEC_FAILURE;
					}
					break;
				case SD_CONST_M25:
					if( SI_LTR.Length < Matrix25Min || 
						SI_LTR.Length > Matrix25Max ) {
						ret = DEC_FAILURE;
					}
					break;
				case SD_CONST_TP:
					if( SI_LTR.Length < TelepenMin || 
						SI_LTR.Length > TelepenMax ) {
						ret = DEC_FAILURE;
					}
					break;
				case SD_CONST_HX:
					if( SI_LTR.Length < HanXinMin || 
						SI_LTR.Length > HanXinMax ) {
						ret = DEC_FAILURE;
					}
					break;
			}
			break;
		default:
			//didn't find one that matched, so don't care about length
			break;
	}

	return ret;
}//CheckLength()


static int CheckAddenda(void) {
	int returnCode = DEC_SUCCESS;
	//check for individual enables and addenda requirements
	//so far for UPC code types only
	if( SI_LTR.Symbology == SD_CONST_UPC ) {
		switch( SI_LTR.Modifier ) {
			//no supplements
			case 'A':
				if( UPCAEnabled ) {

					if( UPCAAddendaRequired ) {
						returnCode = DEC_FAILURE;
					}
					else if( SITIMER && (UPCA2CharAddendaEnabled || UPCA5CharAddendaEnabled) && SI_LTR.Data[0] != '5' ) {
						returnCode = DEC_FAILURE;
						SD_Set(thisDecoder, SD_PROP_PROGRESS_CANCEL, (void *) 1); //get us to the next image

						if( AddendaTimeStamp == 0 ) {
							AddendaTimeStamp = SITIMER();
						}
						
						if( (SITIMER()-AddendaTimeStamp) >= AddendaSearchTime ) { //>= allows the search time to be 0 for no delay
							returnCode = DEC_SUCCESS;
						}
					}
				}
				else {
					returnCode = DEC_FAILURE;
				}
				break;
			case 'B':
				if( UPCE0Enabled ) {
					
					if( UPCEAddendaRequired ) {
						returnCode = DEC_FAILURE;
					}
					else if( SITIMER && (UPCE2CharAddendaEnabled || UPCE5CharAddendaEnabled) ) {
						returnCode = DEC_FAILURE;
						SD_Set(thisDecoder, SD_PROP_PROGRESS_CANCEL, (void *) 1); //get us to the next image

						if( AddendaTimeStamp == 0 ) {
							AddendaTimeStamp = SITIMER();
						}
						
						if( (SITIMER()-AddendaTimeStamp) >= AddendaSearchTime ) { //>= allows the search time to be 0 for no delay
							returnCode = DEC_SUCCESS;
						}
					}
				}
				else {
					returnCode = DEC_FAILURE;
				}
				break;
			case 'C':
				if( UPCE1Enabled ) {

					if( UPCEAddendaRequired ) {
						returnCode = DEC_FAILURE;
					}
					else if( SITIMER && (UPCE2CharAddendaEnabled || UPCE5CharAddendaEnabled) ) {
						returnCode = DEC_FAILURE;
						SD_Set(thisDecoder, SD_PROP_PROGRESS_CANCEL, (void *) 1); //get us to the next image

						if( AddendaTimeStamp == 0 ) {
							AddendaTimeStamp = SITIMER();
						}

						if( (SITIMER()-AddendaTimeStamp) >= AddendaSearchTime ) { //>= allows the search time to be 0 for no delay
							returnCode = DEC_SUCCESS;
						}
					}
				}
				else {
					returnCode = DEC_FAILURE;
				}
				break;
			case 'D':
				if( EAN8Enabled ) {
					if( EAN8AddendaRequired ) {
						returnCode = DEC_FAILURE;
					}
					else if( SITIMER && (EAN82CharAddendaEnabled || EAN85CharAddendaEnabled) ) {
						returnCode = DEC_FAILURE;
						SD_Set(thisDecoder, SD_PROP_PROGRESS_CANCEL, (void *) 1); //get us to the next image

						if( AddendaTimeStamp == 0 ) {
							AddendaTimeStamp = SITIMER();
						}

						if( (SITIMER()-AddendaTimeStamp) >= AddendaSearchTime ) { //>= allows the search time to be 0 for no delay
							returnCode = DEC_SUCCESS;
						}
					}
				}
				else {
					returnCode = DEC_FAILURE;
				}
				break;
			case 'E':
				if( EAN13Enabled ) {
					if( EAN13AddendaRequired ) {
						returnCode = DEC_FAILURE;
					}
					else if( SITIMER && (EAN132CharAddendaEnabled || EAN135CharAddendaEnabled) && SI_LTR.Data[0] != '9' && SI_LTR.Data[1] != '9' ) {
						returnCode = DEC_FAILURE;
						SD_Set(thisDecoder, SD_PROP_PROGRESS_CANCEL, (void *) 1); //get us to the next image

						if( AddendaTimeStamp == 0 ) {
							AddendaTimeStamp = SITIMER();
						}

						if( (SITIMER()-AddendaTimeStamp) >= AddendaSearchTime ) { //>= allows the search time to be 0 for no delay
							returnCode = DEC_SUCCESS;
						}
					}
				}
				else {
					returnCode = DEC_FAILURE;
				}
				break;
			//2 char addendas
			case 'a':
				if( UPCAEnabled ) {
					if( !UPCA2CharAddendaEnabled ) {
						SI_LTR.Length-=2;
						SI_LTR.Modifier = 'A';
						SI_LTR.AIMModifier = '0';
					}					
				}
				else {
					returnCode = DEC_FAILURE;
				}
				break;
			case 'b':
				if( UPCE0Enabled ) {
					if( !UPCE2CharAddendaEnabled ) {
						SI_LTR.Length-=2;
						SI_LTR.Modifier = 'B';
						SI_LTR.AIMModifier = '0';
					}
				}
				else {
					returnCode = DEC_FAILURE;
				}
				break;
			case 'c':
				if( UPCE1Enabled ) {
					if( !UPCE2CharAddendaEnabled ) {
						SI_LTR.Length-=2;
						SI_LTR.Modifier = 'C';
						SI_LTR.AIMModifier = '0';
					}
				}
				else {
					returnCode = DEC_FAILURE;
				}
				break;
			case 'd':
				if( EAN8Enabled ) {
					if( !EAN82CharAddendaEnabled ) {
						SI_LTR.Length-=2;
						SI_LTR.Modifier = 'D';
						SI_LTR.AIMModifier = '0';
					}
				}
				else {
					returnCode = DEC_FAILURE;
				}
				break;
			case 'e':
				if( EAN13Enabled ) {
					if( !EAN132CharAddendaEnabled ) {
						SI_LTR.Length-=2;
						SI_LTR.Modifier = 'E';
						SI_LTR.AIMModifier = '0';
					}
				}
				else {
					returnCode = DEC_FAILURE;
				}
				break;
			//5 char addendas
			case '0':
				if( UPCAEnabled ) {
					if( !UPCA5CharAddendaEnabled ) {
						SI_LTR.Length-=5;
						SI_LTR.Modifier = 'A';
						SI_LTR.AIMModifier = '0';
					}
				}
				else {
					returnCode = DEC_FAILURE;
				}
				break;
			case '1':
				if( UPCE0Enabled ) {
					if( !UPCE5CharAddendaEnabled ) {
						SI_LTR.Length-=5;
						SI_LTR.Modifier = 'B';
						SI_LTR.AIMModifier = '0';
					}
				}
				else {
					returnCode = DEC_FAILURE;
				}
				break;
			case '2':
				if( UPCE1Enabled ) {
					if( !UPCE5CharAddendaEnabled ) {
						SI_LTR.Length-=5;
						SI_LTR.Modifier = 'C';
						SI_LTR.AIMModifier = '0';
					}
				}
				else {
					returnCode = DEC_FAILURE;
				}
				break;
			case '3':
				if( EAN8Enabled ) {
					if( !EAN85CharAddendaEnabled ) {
						SI_LTR.Length-=5;
						SI_LTR.Modifier = 'D';
						SI_LTR.AIMModifier = '0';
					}
				}
				else {
					returnCode = DEC_FAILURE;
				}
				break;
			case '4':
				if( EAN13Enabled ) {
					if( !EAN135CharAddendaEnabled ) {
						SI_LTR.Length-=5;
						SI_LTR.Modifier = 'E';
						SI_LTR.AIMModifier = '0';
					}
				}
				else {
					returnCode = DEC_FAILURE;
				}
				break;
		} //switch( Modifier )
	}//if(symbology == UPC)
	return returnCode;
}//AllowedCode()


static int FillResultInfo( int decoder ) {

	//fill in the basics
	if( SD_Get(decoder, SD_PROP_RESULT_STRING, &SI_LTR.Data) == DEC_FAILURE ) {
		Pt("fail:%d\r\n",__LINE__);
		return DEC_FAILURE;
	}
	if( SD_Get(decoder, SD_PROP_RESULT_LENGTH, &SI_LTR.Length) == DEC_FAILURE ) {
		Pt("fail:%d\r\n",__LINE__);
		return DEC_FAILURE;
	}
	if( SD_Get(decoder, SD_PROP_RESULT_BOUNDS, &SI_LTR.MainBounds) == DEC_FAILURE ) {
		Pt("fail:%d\r\n",__LINE__);
		return DEC_FAILURE;
	}
	if( SD_Get(decoder, SD_PROP_RESULT_CENTER, &SI_LTR.GraphicalCenter) == DEC_FAILURE ) {
		Pt("fail:%d\r\n",__LINE__);
		return DEC_FAILURE;
	}
	if( SD_Get(decoder, SD_PROP_RESULT_SYMBOLOGY, &SI_LTR.Symbology) == DEC_FAILURE ) {
		Pt("fail:%d\r\n",__LINE__);
		return DEC_FAILURE;
	}
	if( SD_Get(decoder, SD_PROP_RESULT_SYMBOLOGY_EX, &SI_LTR.SymbologyEx) == DEC_FAILURE ) { 
		Pt("fail:%d\r\n",__LINE__);
		return DEC_FAILURE;
	}
	if( SD_Get(decoder, SD_PROP_RESULT_MODIFIER, &SI_LTR.Modifier) == DEC_FAILURE ) {
		Pt("fail:%d\r\n",__LINE__);
		return DEC_FAILURE;
	}
	if( SD_Get(decoder, SD_PROP_RESULT_MODIFIER_EX, &SI_LTR.ModifierEx) == DEC_FAILURE ) {
		if( SD_GetLastError() != SD_ERR_PROPERTY_LIFETIME ) {
			Pt("fail:%d\r\n",__LINE__);
			return DEC_FAILURE;
		}
	}
	if( SD_Get(decoder, SD_PROP_RESULT_LINKAGE, &SI_LTR.LinkFlag) == DEC_FAILURE ) {
		if( SD_GetLastError() != SD_ERR_PROPERTY_LIFETIME ) {
			Pt("fail:%d\r\n",__LINE__);
			return DEC_FAILURE;
		}
	}
	
	//fill in the AIM and HHP codeletter and modifier info
	//These are ordered by their AIM codelettes, uppercase first
	SI_LTR.AIMCodeLetter = DEC_AIM_CODELETTER_OTHER;	//put in some initial values
	SI_LTR.AIMModifier = SI_LTR.Modifier;
	SI_LTR.ProgrammingCode = DEC_FALSE;
	SI_LTR.DecodeType = DEC_TYPE_ID;

	for(int jk=0; jk<4; jk++ ) {
		SI_LTR.IQImageInfo.bounds.corners[jk].x = SI_LTR.MainBounds.corners[jk].x;
		SI_LTR.IQImageInfo.bounds.corners[jk].y = SI_LTR.MainBounds.corners[jk].y;
	}
	SI_LTR.IQImageInfo.d_corner = 1;
	SI_LTR.IQImageInfo.pdfRows = 1;
	SI_LTR.IQImageInfo.pdfCols = 1;

	switch( SI_LTR.Symbology ) {
		case SD_CONST_C39:				//Code39
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_CODE39;
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_CODE39;
			if( SD_Get(decoder, SD_PROP_RESULT_2D_MODULESX, &SI_LTR.IQImageInfo.d_corner) == DEC_FAILURE) {
				Pt("fail:%d\r\n",__LINE__);
				return DEC_FAILURE;
			}

			Pt("dcorner:%d\r\n",SI_LTR.IQImageInfo.d_corner);
			break;
		/*'B' = Telepen, See case 0 (Ex Properties)*/
		case SD_CONST_C128:				//Code128
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_CODE128;
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_CODE128;
			for( int jk=0; jk<SI_LTR.Length; jk++ ) {
				switch( SI_LTR.Data[jk] ) {
					case 0xF1:
						if( jk > 0 ) {
							SI_LTR.Data[jk] = 0x1D;
							if( jk == 1 && SI_LTR.AIMModifier == '0' ) {
								SI_LTR.AIMModifier = '2'; //Adaptus precedented the first position FNC1 AIM mod over any other.
							}
						}
						else {
							if( !GS1128Enabled ) {
								Pt("fail:%d\r\n",__LINE__);
								return DEC_FALSE;
							}

							SI_LTR.AIMModifier	= '1';
							SI_LTR.HHPCodeID	= DEC_HHP_CODE_ID_GS1_128;

							if( Code128FncTransmit ) {
								SI_LTR.Data[jk] = 0x81;
							}
							else {
								DeleteChar(jk);
								jk--;
							}
						}
						break;
					case 0xF2:
						SI_LTR.AIMModifier = '4'; //append
						if( Code128FncTransmit ) {
							SI_LTR.Data[jk] = 0x82;
						}
						else {
							DeleteChar(jk);
							jk--;
						}
						break;
					case 0xF3:
						SI_LTR.ProgrammingCode = DEC_TRUE;
						if( Code128FncTransmit ) {
							SI_LTR.Data[jk] = 0x83;
						}
						else {
							DeleteChar(jk);
							jk--;
						}
						break;
					case 0xF4:
						SI_LTR.Data[jk+1] += 0x80;
						if( Code128FncTransmit ) {
							SI_LTR.Data[jk] = 0x84;
						}
						else {
							DeleteChar(jk);
							jk--;
						}
						break;
				}
			}			
			if( SI_LTR.AIMModifier != '1' ) { //GS1 handled above
				if( !SI_LTR.ProgrammingCode ) {
					if( !Code128Enabled ) {
						Pt("fail:%d\r\n",__LINE__);
						return DEC_FAILURE;
					}
				}
			}

			if( SD_Get(decoder, SD_PROP_RESULT_2D_MODULESX, &SI_LTR.IQImageInfo.d_corner) == DEC_FAILURE) {
				Pt("fail:%d\r\n",__LINE__);
				return DEC_FAILURE;
			}
			Pt("dcorner:%d\r\n",SI_LTR.IQImageInfo.d_corner);
			break;
		/*'D' = Code1, N/A*/
		case SD_CONST_UPC:
			SI_LTR.AIMCodeLetter = DEC_AIM_CODELETTER_UPC;  
			SI_LTR.AIMModifier	= '0'; 
			switch( SI_LTR.Modifier ) {
				case 'A':				//UPC-A
					SI_LTR.AIMModifier	= '0'; 
					SI_LTR.HHPCodeID	= DEC_HHP_CODE_ID_UPCA; 
					break;
				case 'a':
				case '0':
					SI_LTR.AIMModifier	= '3'; 
					SI_LTR.HHPCodeID	= DEC_HHP_CODE_ID_UPCA; 
					break;
				case 'B':				//UPC-E0
					SI_LTR.AIMModifier	= '0'; 
					SI_LTR.HHPCodeID	= DEC_HHP_CODE_ID_UPCE; 
					break;
				case 'b':
				case '1':
					SI_LTR.AIMModifier	= '3'; 
					SI_LTR.HHPCodeID	= DEC_HHP_CODE_ID_UPCE; 
					break;
				case 'C':				//UPC-E1
					SI_LTR.AIMModifier	= '0'; 
					SI_LTR.AIMCodeLetter = DEC_AIM_CODELETTER_OTHER;  
					SI_LTR.HHPCodeID	= DEC_HHP_CODE_ID_UPCE; 
					break;
				case 'c':
				case '2':
					SI_LTR.AIMModifier	= '3'; 
					SI_LTR.AIMCodeLetter = DEC_AIM_CODELETTER_OTHER;  
					SI_LTR.HHPCodeID	= DEC_HHP_CODE_ID_UPCE; 
					break;
				case 'D':				//EAN-8
				case 'd':
				case '3':
					SI_LTR.AIMModifier	= '4'; 
					SI_LTR.HHPCodeID	= DEC_HHP_CODE_ID_EAN8;
					break;
				case 'E':				//EAN-13
					SI_LTR.AIMModifier	= '0'; 
					SI_LTR.HHPCodeID	= DEC_HHP_CODE_ID_EAN13; 
					break;
				case 'e':
				case '4':
					SI_LTR.AIMModifier	= '3'; 
					SI_LTR.HHPCodeID	= DEC_HHP_CODE_ID_EAN13;
					break;
			}
			break;
		case SD_CONST_CB:				//Codabar
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_CODABAR;
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_CODABAR;
			if( SD_Get(decoder, SD_PROP_RESULT_2D_MODULESX, &SI_LTR.IQImageInfo.d_corner) == DEC_FAILURE) {
				Pt("fail:%d\r\n",__LINE__);
				return DEC_FAILURE;
			}
			Pt("dcorner:%d\r\n",SI_LTR.IQImageInfo.d_corner);
			break;
		case SD_CONST_C93:				//Code93
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_CODE93;
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_CODE93;
			break;
		case SD_CONST_C11:				//Code 11
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_CODE11;
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_CODE11;
			break;
		case SD_CONST_I25:				//Interleaved25
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_I25;
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_I25;
			if( SD_Get(decoder, SD_PROP_RESULT_2D_MODULESX, &SI_LTR.IQImageInfo.d_corner) == DEC_FAILURE) {
				Pt("fail:%d\r\n",__LINE__);
				return DEC_FAILURE;
			}
			Pt("dcorner:%d\r\n",SI_LTR.IQImageInfo.d_corner);
			break;
		/*'J' = Reserved*/
		/*'K' = Code16K, N/A*/
		case SD_CONST_PDF:
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_PDF417;
			if( SI_LTR.ModifierEx&0x1 ) {	//MicroPDF
				SI_LTR.HHPCodeID = DEC_HHP_CODE_ID_MICROPDF;
			}
			else {						//PDF417
				SI_LTR.HHPCodeID = DEC_HHP_CODE_ID_PDF417;
				if( SD_Get(decoder, SD_PROP_RESULT_2D_MODULESX, &SI_LTR.IQImageInfo.d_corner) == DEC_FAILURE) {
					Pt("fail:%d\r\n",__LINE__);
					return DEC_FAILURE;
				}
				SI_LTR.IQImageInfo.pdfCols = SI_LTR.IQImageInfo.d_corner/17; // we don't need this, but I figured I'd put it in
				if( SD_Get(decoder, SD_PROP_RESULT_2D_MODULESY, &SI_LTR.IQImageInfo.pdfRows) == DEC_FAILURE) {
					Pt("fail:%d\r\n",__LINE__);
					return DEC_FAILURE;
				}
				Pt("dcorner:%d, Rows:%d, Cols:%d\r\n",SI_LTR.IQImageInfo.d_corner,SI_LTR.IQImageInfo.pdfRows,SI_LTR.IQImageInfo.pdfCols);
			}
// fix for menu commands, need to review
//			SI_LTR.ProgrammingCode = (SI_LTR.ModifierEx&(PROGRAMMINGCODE>>26))?DEC_TRUE:DEC_FALSE;
			SI_LTR.ProgrammingCode = (SI_LTR.ModifierEx&(0x2))?DEC_TRUE:DEC_FALSE;
	
			break;
		case SD_CONST_MSIP:
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_MSI;
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_MSI;
			break;
			/* Swift does not support Plessey */
		/*'N' = Anker, N/A*/
		case SD_CONST_CODABLOCK_A:		//Codablock
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_CODABLOCK;
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_CODABLOCK_A;
			break;
		case SD_CONST_CODABLOCK_F:
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_CODABLOCK;
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_CODABLOCK_F;
			SI_LTR.ProgrammingCode = (SI_LTR.Modifier&PROGRAMMINGCODE);
			break;
		/*'P' = Plessey, See SD_CONST_MSIP*/
		case SD_CONST_QR:
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_QRCODE;
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_QRCODE;
			SD_Get(decoder, SD_PROP_RESULT_QRTOTAL, &SI_LTR.AppendTotal);
			SD_Get(decoder, SD_PROP_RESULT_QRPOSITION, &SI_LTR.AppendIndex);
			SD_Get(decoder, SD_PROP_RESULT_QRPARITY, &SI_LTR.Parity);
			//QRTOTAl > 0 if part of an append, and since parity is 0-255, we can add 256 to tell next level that is is an append.
			if( SI_LTR.AppendTotal ) { 
				SI_LTR.Parity += 256;
			}
			break;
		case SD_CONST_S25_2SS:			//IATA25 in adaptus
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_S25_2BAR;
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_S25;
			break;
		case SD_CONST_S25_3SS:			//Straight25 in adaptus
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_S25_3BAR;
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_S25;
			break;
		/*'T' = Code49, N/A*/
		case SD_CONST_MC:				//MaxiCode
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_MAXICODE;
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_MAXICODE;
			break;
		/*'V' = Reserved*/
		/*'W' = Reserved*/
		/*'X' = Other barcode, used with symbols who don't have a given letter*/
		/*'Y' = System Expansion*/
		/*'Z' = Non-bar Code*/
		/*'a' = Reserved*/
		/*'b' = Reserved*/
		/*'c' = Channel Code*/
		case SD_CONST_DM:				//DataMatrix
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_DATAMATRIX;
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_DATAMATRIX;
			if( SD_Get(decoder, SD_PROP_RESULT_2D_MODULESX, &SI_LTR.IQImageInfo.d_corner) == DEC_FAILURE) {
				Pt("fail:%d\r\n",__LINE__);
				return DEC_FAILURE;
			}
			Pt("dcorner:%d\r\n",SI_LTR.IQImageInfo.d_corner);
			SI_LTR.ProgrammingCode = (SI_LTR.Modifier == 'R')?DEC_TRUE:DEC_FALSE;
			break;
		case SD_CONST_RSS_EXP:			//RSS/EAN-UCC Composites
		case SD_CONST_RSS_EXP_ST:
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_GS1_DATABAR;
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_GS1_DATABAR_EXP;
			SI_LTR.AIMModifier		= '0';
			break;
		case SD_CONST_RSS_14_LIM:
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_GS1_DATABAR;
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_GS1_DATABAR_LIM;
			SI_LTR.AIMModifier		= '0';
			break;
		case SD_CONST_RSS_14:
		case SD_CONST_RSS_14_ST:
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_GS1_DATABAR;
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_GS1_DATABAR;
			SI_LTR.AIMModifier		= '0';
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
		/*'o' = OCR, See case 0 (Ex Properties)*/
		/*'p' = Posicode, N/A*/
		/*'s' = SuperCode, N/A*/
		/*'t' = Reserved*/
		/*'u' = Reserved*/
		/*'v' = UltraCode, N/A*/
		/*'w' = Reserved*/
		/*'x' = Reserved*/
		/*'y' = Reserved*/
		case SD_CONST_AZ:				//Aztec
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_AZTEC_CODE;
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_AZTEC_CODE;
			SI_LTR.ProgrammingCode = SI_LTR.ModifierEx;
			if( SD_Get(decoder, SD_PROP_RESULT_2D_MODULESX, &SI_LTR.IQImageInfo.d_corner) == DEC_FAILURE) {
				Pt("fail:%d\r\n",__LINE__);
				return DEC_FAILURE;
			}
			Pt("dcorner:%d\r\n",SI_LTR.IQImageInfo.d_corner);
			if( !SI_LTR.ProgrammingCode && !AztecEnabled ) {
				return DEC_FAILURE;
			}
			break;
		/*Postal Symbologies not handdled by AIM letters*/
		case SD_CONST_PN:				//PostNet
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_OTHER;
			SI_LTR.AIMModifier		= '0';
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_POSTNET;
			break;
		case SD_CONST_PL:				//PlanetCode
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_OTHER;
			SI_LTR.AIMModifier		= '0';
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_PLANET_CODE;
			break;
		case SD_CONST_AP:				//Australia Post
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_OTHER;
			SI_LTR.AIMModifier		= '0';
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_AUS_POST;
			break;
		case SD_CONST_RM:				//Royal Main (British Post)
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_OTHER;
			SI_LTR.AIMModifier		= '0';
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_BRITISH_POST;
			break;
		case SD_CONST_JP:				//Japan Post
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_OTHER;
			SI_LTR.AIMModifier		= '0';
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_JAPAN_POST;
			break;
		case SD_CONST_KIX:				//Netherlands Post
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_OTHER;
			SI_LTR.AIMModifier		= '0';
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_KIX_CODE;
			break;
		case SD_CONST_USPS4CB:				//USPS 4CB
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_OTHER;
			SI_LTR.AIMModifier		= '0';
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_USPS_4_STATE;
			break;
		case SD_CONST_UPU:				//UPU
			SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_OTHER;
			SI_LTR.AIMModifier		= '0';
			SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_UPU_4_STATE;
			break;
		/*Extended Symbologies*/
		case 0: 
			switch( SI_LTR.SymbologyEx ) {
				case SD_CONST_TP:			//Telepen
					SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_TELEPEN;
					SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_TELEPEN;
					break;
				case SD_CONST_OCR:			//OCR
					SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_OCR;
					SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_OCR;
					break;
				case SD_CONST_HK25:			//Hong Kong 25 aka China Post
					SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_OTHER;
					SI_LTR.AIMModifier		= '0';
					SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_HK25;
					break;
				case SD_CONST_M25:			//Matrix25
					SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_OTHER;
					SI_LTR.AIMModifier		= '0';
					SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_M25;
					break;
				case SD_CONST_NEC25:		//NEC25
					SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_OTHER;
					SI_LTR.AIMModifier		= '0';
					SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_NEC25;
					break;
				case SD_CONST_TRIOPTIC:		//Trioptic
					SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_OTHER;
					SI_LTR.AIMModifier		= '0';
					SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_TRIOPTIC;
					//switch 123 with 456
					SI_LTR.Data[10] = SI_LTR.Data[0];
					SI_LTR.Data[11] = SI_LTR.Data[1];
					SI_LTR.Data[12] = SI_LTR.Data[2];
					
					SI_LTR.Data[0] = SI_LTR.Data[3];
					SI_LTR.Data[1] = SI_LTR.Data[4];
					SI_LTR.Data[2] = SI_LTR.Data[5];
					
					SI_LTR.Data[3] = SI_LTR.Data[10];
					SI_LTR.Data[4] = SI_LTR.Data[11];
					SI_LTR.Data[5] = SI_LTR.Data[12];
					break;
				case SD_CONST_INFOMAIL:		//Royal Mail Infomail
					SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_OTHER;
					SI_LTR.AIMModifier		= '0';
					SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_INFOMAIL;
					break;
				case SD_CONST_KP:			//Korea Post
					SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_OTHER;
					SI_LTR.AIMModifier		= '0';
					SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_KOREA_POST;
					break;
				case SD_CONST_CP:			//Canadian Post
					SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_OTHER;
					SI_LTR.AIMModifier		= '0';
					SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_CANADIAN_POST;
					break;
				case SD_CONST_HX:
					SI_LTR.AIMCodeLetter	= DEC_AIM_CODELETTER_OTHER;
					SI_LTR.AIMModifier		= '0';
					SI_LTR.HHPCodeID		= DEC_HHP_CODE_ID_HAN_XIN_CODE;
					break;
				default:
				Pt("fail:%d\r\n",__LINE__);
					return DEC_FAILURE;
			}//switch(SI_LTR.SymbologyEx)
	}//switch(SI_LTR.Symbology)
	return DEC_SUCCESS;

}//FillResultInfo()

static void FormatResult( void ) {

	switch(SI_LTR.Symbology) {
		case SD_CONST_UPC:
			switch( SI_LTR.HHPCodeID ) {
				case DEC_HHP_CODE_ID_UPCA:
					if( !UPCANumberSystemTransmit ) {
						DeleteChar(0);
					}
					if( !UPCACheckDigitTransmit ) {
						if( SI_LTR.Modifier == 'a' ) {
							DeleteChar(SI_LTR.Length-3);
						}
						else if( SI_LTR.Modifier == '0' ) {
							DeleteChar(SI_LTR.Length-6);
						}
						else {
							SI_LTR.Length--;
						}
					}
					if( UPCAAddendaSeparator ) {
						if( SI_LTR.Modifier == 'a' ) {
							InsertChar( SI_LTR.Length-2, ' ' );
						}
						else if( SI_LTR.Modifier == '0' ) {
							InsertChar( SI_LTR.Length-5, ' ' );
						}
					}
					break;
				case DEC_HHP_CODE_ID_UPCE:
					if( SI_LTR.Modifier == 'C' ) {
						SI_LTR.Length--;
					}
					else if( SI_LTR.Modifier == 'c' ) {
						DeleteChar(SI_LTR.Length-3);
					}
					else if( SI_LTR.Modifier == '2' ) {
						DeleteChar(SI_LTR.Length-6);
					}
					else { //UPC-E0
						if( !UPCENumberSystemTransmit ) {
							DeleteChar(0);
						}
						if( !UPCECheckDigitTransmit  ) { 
							if( SI_LTR.Modifier == 'b' ) {
								DeleteChar(SI_LTR.Length-3);
							}
							else if( SI_LTR.Modifier == '1' ) {
								DeleteChar(SI_LTR.Length-6);
							}
							else {
								SI_LTR.Length--;
							}
						}
					}

					if( UPCEAddendaSeparator ) {
						if( SI_LTR.Modifier == 'b' || SI_LTR.Modifier == 'c' ) {
							InsertChar( SI_LTR.Length-2, ' ' );
						}
						else if( SI_LTR.Modifier == '1' || SI_LTR.Modifier == '2' ) {
							InsertChar( SI_LTR.Length-5, ' ' );
						}
					}
					break;
				case DEC_HHP_CODE_ID_EAN8:
					if( !EAN8CheckDigitTransmit ) {
						if( SI_LTR.Modifier == 'd' ) {
							DeleteChar(SI_LTR.Length-3);
						}
						else if( SI_LTR.Modifier == '3' ) {
							DeleteChar(SI_LTR.Length-6);
						}
						else {
							SI_LTR.Length--;
						}
					}
					if( EAN8AddendaSeparator ) {
						if( SI_LTR.Modifier == 'd' ) {
							InsertChar( SI_LTR.Length-2, ' ' );
						}
						else if( SI_LTR.Modifier == '3' ) {
							InsertChar( SI_LTR.Length-5, ' ' );
						}
					}
					break;
				case DEC_HHP_CODE_ID_EAN13:
					if( EAN13ISBNEnabled && (!strncmp((char *)SI_LTR.Data,"978",3) || !strncmp((char *)SI_LTR.Data,"979",3)) ) {
							int weight, check, index;

							DeleteChar(0); DeleteChar(0); DeleteChar(0); //delete Bookland header

							for (weight=10,index=check=0; weight>1; weight--) {
								check += weight * (SI_LTR.Data[index++] - '0');
							}
							check = (3300 - check) % 11;
							InsertChar(9,ISBNCheckChars[check]);
							DeleteChar(10);

							if( SI_LTR.Modifier == '4' ) {
								if( SI_LTR.Data[SI_LTR.Length-5] == '5' && 
									(SI_LTR.Data[SI_LTR.Length-4] != '9' || 
									 SI_LTR.Data[SI_LTR.Length-3] != '9' || 
									 SI_LTR.Data[SI_LTR.Length-2] != '9' || 
									 SI_LTR.Data[SI_LTR.Length-1] != '9') ) { //5xxxx where xxxx != 9999
										SI_LTR.Data[SI_LTR.Length-5] = '0';
								}
								else if( SI_LTR.Data[SI_LTR.Length-5] < '1' && SI_LTR.Data[SI_LTR.Length-5] > '4' ) { //leave any 1xxxx to 4xxxx
									SI_LTR.Length = 10;
									SI_LTR.Modifier = 'E';
								}
								else { //remove any other addenda combo
									SI_LTR.Length = 10;
									SI_LTR.Modifier = 'E';
								}
							}//if( mod = '4' )
					}//if( ISBN )
					else if( !EAN13CheckDigitTransmit ) {
						if( SI_LTR.Modifier == 'e' ) {
							DeleteChar(SI_LTR.Length-3);
						}
						else if( SI_LTR.Modifier == '4' ) {
							DeleteChar(SI_LTR.Length-6);
						}
						else {
							SI_LTR.Length--;
						}
					}

					if( EAN13AddendaSeparator ) {
						if( SI_LTR.Modifier == 'e' ) {
							InsertChar( SI_LTR.Length-2, ' ' );
						}
						else if( SI_LTR.Modifier == '4' ) {
							InsertChar( SI_LTR.Length-5, ' ' );
						}
					}

					break;
			}//switch(HHPCodeID)
			break;//break CONST_UPC
		case SD_CONST_C39:
			if( Code39Base32Enabled && 
				SI_LTR.Length == 6 && 
				SI_LTR.LinkFlag != SD_CONST_LINKED_C39) {

				TryBase32();
			}
			if( Code39StartStopTransmit ) {
				InsertChar(0,'*');
				SI_LTR.Data[SI_LTR.Length] = '*';
				SI_LTR.Length++;
			}
			break;
		case SD_CONST_PN:
			if( !PostnetCheckDigitTransmit ) {
				SI_LTR.Length--;
			}
			break;
		case SD_CONST_PL:
			if( !PlanetCodeCheckDigitTransmit ) {
				SI_LTR.Length--;
			}
			break;

	}//switch(Symbology)
}//FormatResult()
		
static void InsertChar( int charIndex, unsigned char newChar ) {
	for( int i = SI_LTR.Length; i!=charIndex; i-- ) {
		SI_LTR.Data[i] = SI_LTR.Data[i-1];
	}
	SI_LTR.Data[charIndex] = newChar;
	SI_LTR.Length++;
}

static void DeleteChar( int charIndex ) {
	for( ; charIndex<SI_LTR.Length-1; charIndex++ ) {
		SI_LTR.Data[charIndex] = SI_LTR.Data[charIndex+1];
	}
	SI_LTR.Length--;
}

static int FindPostalMatch( unsigned int combo ) {
	for( int jk=0; jk<POSTAL_OPTIONS; jk++ ) {
		if( combo == PostalOptions[jk] ) {
			return jk;
		}
	}
	return -1;
}

/*----------------------------------------------------------------------*/
/* "TryBase32(ws)" tests for, then converts a Base 32 (PARAF) message	*/
/*----------------------------------------------------------------------*/
static const char Twice[10] = { 0, 2, 4, 6, 8, 1, 3, 5, 7, 9 };
static int Base32Value (unsigned char c) {
	if (c >= '0' && c <= '9') return (c - '0');
	if (c >= 'B' && c <= 'D') return (c - 'B' + 10);
	if (c >= 'F' && c <= 'H') return (c - 'F' + 13);
	if (c >= 'J' && c <= 'N') return (c - 'J' + 16);
	if (c >= 'P' && c <= 'Z') return (c - 'P' + 21);
	return (-1);
}

static int TryBase32( void ) {
	int i, sum, c; char s[12]; int value = 0; int ok = DEC_TRUE;
	for (i=value=0; (i<SI_LTR.Length)&&(ok); i++) {
		c = Base32Value(SI_LTR.Data[i]);
		if (c >= 0) value = (value<<5) + c; 
		else ok = DEC_FALSE;
	}

	if (ok) {
		sprintf(s,"%09ld",(long)value);
		for (i=sum=0; i<8; i+=2) sum += (s[i]-'0') + Twice[s[i+1]-'0'];
		if ((sum % 10) == (int)(s[i] - '0')) {
			for (i=0; i<9; i++) SI_LTR.Data[i] = s[i]; 
			SI_LTR.Length = 9;	// Without a leading 'A' !!
			SI_LTR.HHPCodeID = DEC_HHP_CODE_ID_CODE39_BASE32;
			SI_LTR.AIMCodeLetter = DEC_AIM_CODELETTER_OTHER;
			SI_LTR.AIMModifier = '0';
		} else ok = DEC_FALSE;
	} return ok;
}


//______________________________________________________________
/* Visible Functions */
int SI_InitDecoder( int width, int height, int wrap ) {
	thisDecoder = 0;
	thisDecoder = SD_Create();
	if( thisDecoder ) {
		SD_Set(thisDecoder, SD_PROP_IMAGE_WIDTH, (void *)width);
		SD_Set(thisDecoder, SD_PROP_IMAGE_HEIGHT, (void *)height);
		SD_Set(thisDecoder, SD_PROP_IMAGE_LINE_DELTA, (void *)wrap);
		SI_DecodeReset(thisDecoder);
	}

	return thisDecoder;
}
void SI_DestroyDecoder( int decoder ) {
	SD_Destroy(decoder);
}

void SI_ReportDecoderVersion( int decoder, char *reportString ) {
    int     major, minor, build;

    SD_Get(decoder, SD_PROP_VERSION_MAJOR, &major);
    SD_Get(decoder, SD_PROP_VERSION_MINOR, &minor);
    SD_Get(decoder, SD_PROP_VERSION_BUILD, &build);

	sprintf( reportString,"Integrated Decoder Version: %d.%d.%d", (major),(minor), build );
}

int SI_InstallCallback( int decoder, /*CallbackTag*/ unsigned int tag, void *function ) {
	
	switch( tag ) {
		case CB_PRINTF:
			SIPRINTF = (fSdPrintf_t)function;
			break;
		case CB_TIMER:
			SITIMER = (fSdTimer_t)function;
			break;
		case CB_RESULT:
			SIRESULT = (fSdResult_t)function;
			SD_Set(decoder,SD_PROP_CALLBACK_RESULT, (void *)SI_ResultCallback);
			break;
		case CB_RESULT_NOTIFY:
			SINOTIFY = (fSdResultNotify_t)function;
			break;
		case CB_WATCHDOG:
			SIWATCHDOG = (fSdWatchDog_t)function;
			break;
		case CB_STATUS:
			SISTATUS = (fSdStatus_t)function;
			SD_Set(decoder, SD_PROP_CALLBACK_STATUS, (void *)SI_StatusCallback);
			break;
		case CB_PROGRESS:
			SIPROGRESS = (fSdProgress_t)function;
			SD_Set(decoder, SD_PROP_CALLBACK_PROGRESS, (void *)SI_ProgressCallback);
			break;
		default:
			return DEC_FAILURE; //not a valid tag, return error
	}
	return DEC_SUCCESS; //found a good tag
}

int SI_Decode( int decoder, void *image ) {
	int ret = -99;
	if( SD_Set(decoder, SD_PROP_IMAGE_POINTER, image) != 0 ) {
		ret = SD_Decode(decoder);
	}
	return ret;
}

int SI_StopDecode( int decoder ) {
	return SD_Set(decoder, SD_PROP_PROGRESS_CANCEL, (void *) 1);
}

int SI_ContinueDecode(int decoder) {
	return SD_Set(decoder, SD_PROP_PROGRESS_CANCEL, (void *) 0);
}

int SI_PauseDecode(int decoder) {
	return SD_Set(decoder, SD_PROP_PROGRESS_CANCEL, (void *) 2);
}

int SI_DecodeGet( int decoder, unsigned int tag, void *data ) {
int returnCode = DEC_SUCCESS; 

	switch( tag ) {
		case DEC_MONOCOLOR_ENABLED:
			returnCode = SD_Get(decoder, ID_PROP_MONOCOLOR_CORRECTION_ENABLE, data); break;
		case DEC_IMAGE_HEIGHT:
			returnCode = SD_Get(decoder, SD_PROP_IMAGE_HEIGHT, data ); break;
		case DEC_IMAGE_WIDTH:
			returnCode = SD_Get(decoder, SD_PROP_IMAGE_WIDTH, data ); break;
		case DEC_IMAGE_CENTER_X:
			returnCode = SD_Get(decoder, SD_PROP_IMAGE_SEARCH_CENTER_X, data ); break;
		case DEC_IMAGE_CENTER_Y:
			returnCode = SD_Get(decoder, SD_PROP_IMAGE_SEARCH_CENTER_Y, data ); break;
		case DEC_IMAGE_LINE_DELTA:
			returnCode = SD_Get(decoder, SD_PROP_IMAGE_LINE_DELTA, data ); break;
		case DEC_IMAGE_MIRRORED:
			returnCode = SD_Get(decoder, SD_PROP_IMAGE_MIRRORED, data ); break;
		case DEC_USE_MLD:
			memcpy(data,&UseMLD,sizeof(int)); break;
		case DEC_USE_DISTANCE_MAP:
			returnCode = SD_Get(decoder, ID_PROP_USE_DISTANCE_MAP, data ); break;
		case DEC_EDGE_DETECTOR:
			returnCode = SD_Get(decoder, ID_PROP_EDGE_DETECTOR, data ); break;
		case DEC_CYCLING_FINDER: 
			returnCode = SD_Get(decoder, ID_PROP_CYCLING_FINDER, data ); break;
		case DEC_SUBPIXEL_FINDER:
			returnCode = SD_Get(decoder, ID_PROP_SUBPIXEL_FINDER, data ); break;
		case DEC_USE_SUBPIXEL_MLD:
			returnCode = SD_Get(decoder, ID_PROP_USE_SUBPIXEL_MLD, data ); break;

		case DEC_BLACK_LEVEL:
			returnCode = SD_Get(decoder, SD_PROP_MISC_BLACK_LEVEL, data ); break;
		case DEC_OPERATING_MODE:
			returnCode = SD_Get(decoder, SD_PROP_MISC_OP_MODE, data ); break;
		case DEC_TYPICAL_IMAGE_DENSITY:
			returnCode = SD_Get(decoder, SD_PROP_IMAGE_TYP_DENSITY, data ); break;
// Advanced
		case DEC_CELL_PHONE_IMPROVEMENTS:
			returnCode = SD_Get(decoder, SD_PROP_MISC_CELLPHONE_IMPROVEMENTS, data ); break;
		case DEC_ECI_HANDLING:
			returnCode = SD_Get(decoder, SD_PROP_MISC_ECI_HANDLING, data ); break;
		case DEC_GENERAL_IMPROVEMENTS:
			returnCode = SD_Get(decoder, SD_PROP_MISC_IMPROVEMENTS, data ); break;
		case DEC_ISSUE_IDENTICAL_SYMBOLS:
			returnCode = SD_Get(decoder, SD_PROP_MISC_ISSUE_IDENTICAL_SYMBOLS, data ); break;
		case DEC_ISSUE_IDENTICAL_SPACING:
			returnCode = SD_Get(decoder, SD_PROP_MISC_ISSUE_IDENTICAL_SPACING, data ); break;
		case DEC_LOW_ASPECT_RATIO:
			returnCode = SD_Get(decoder, SD_PROP_MISC_LOW_ASPECT_RATIO, data ); break;
		case DEC_LOW_CONTRAST_IMPROVEMENTS:
			returnCode = SD_Get(decoder, SD_PROP_MISC_LOW_CONTRAST_IMPROVEMENTS, data ); break;
		case DEC_MISENCODED_SYMBOLS:
			returnCode = SD_Get(decoder, SD_PROP_MISC_MISENCODED_SYMBOLS, data ); break;
		case DEC_PASS_THROUGH:
			returnCode = SD_Get(decoder, SD_PROP_MISC_PASS_THROUGH, data ); break;
		case DEC_UNDECODABLE_SYMBOLS:
			returnCode = SD_Get(decoder, SD_PROP_MISC_UNDECODABLE_SYMBOLS, data ); break;
		case DEC_UNDECODABLE_SYMBOLS_EX:
			returnCode = SD_Get(decoder, SD_PROP_MISC_UNDECODABLE_SYMBOLS_EX, data ); break;
		case DEC_SUBREGION_PROCESSING:
			returnCode = SD_Get(decoder, SD_PROP_MISC_SUBREGION_PROCESSING, data ); break;
			
// Linear Specific
		case DEC_SUBREGION_HEIGHT:
			returnCode = SD_Get(decoder, SD_PROP_LINEAR_SUBREGION_HEIGHT, data ); break;
		case DEC_SUBREGION_LEFT:
			returnCode = SD_Get(decoder, SD_PROP_LINEAR_SUBREGION_LEFT, data ); break;
		case DEC_SUBREGION_TOP:
			returnCode = SD_Get(decoder, SD_PROP_LINEAR_SUBREGION_TOP, data ); break;
		case DEC_SUBREGION_WIDTH:
			returnCode = SD_Get(decoder, SD_PROP_LINEAR_SUBREGION_WIDTH, data ); break;

//Misc Commands
		case DEC_DISABLE_SYMBOLOGY_GROUP:
			memcpy( data, &DisableSymbologyGroup, sizeof(int) ); break;
		//case DEC_DUMP_SETTINGS: BURP!!!
		//case DEC_DISPLAY_DATA: ?? workspace maybe?

//AGC
		case DEC_AGC_EXPOSURE_QUALITY:
			returnCode = SD_Get(decoder, SD_PROP_AGC_EXPOSURE_QUALITY, data ); break;
		case DEC_AGC_NEXT_GAIN:
			returnCode = SD_Get(decoder, SD_PROP_AGC_NEXT_GAIN, data ); break;
		case DEC_AGC_SAMPLING_OPTIONS:
			returnCode = SD_Get(decoder, SD_PROP_AGC_SAMPLING_OPTIONS, data ); break;
			
/* Symbology Specific Settings
 *	These settings are specific for each symbology.
 */

/* Linear Symbologies */
		case DEC_ADD_SEARCH_TIME_ADDENDA:
			memcpy(data,&AddendaSearchTime,sizeof(int)); break;

//UPC-A
		case DEC_UPCA_ENABLED:
			memcpy(data,&UPCAEnabled,sizeof(int)); break;
		case DEC_UPCA_CHECK_DIGIT_TRANSMIT:
			memcpy(data,&UPCACheckDigitTransmit,sizeof(int)); break;
		case DEC_UPCA_NUMBER_SYSTEM_TRANSMIT:
			memcpy(data,&UPCANumberSystemTransmit,sizeof(int)); break;
		case DEC_UPCA_2CHAR_ADDENDA_ENABLED:
			memcpy(data,&UPCA2CharAddendaEnabled,sizeof(int)); break;
			//returnCode = SD_Get(decoder, SD_PROP_UPC_SUPPLEMENTALS, data ); break;
		case DEC_UPCA_5CHAR_ADDENDA_ENABLED:
			memcpy(data,&UPCA5CharAddendaEnabled,sizeof(int)); break;
			//returnCode = SD_Get(decoder, SD_PROP_UPC_SUPPLEMENTALS, data ); break;
		case DEC_UPCA_ADDENDA_REQUIRED:
			memcpy(data,&UPCAAddendaRequired,sizeof(int)); break;
		case DEC_UPCA_ADDENDA_SEPARATOR:
			memcpy(data,&UPCAAddendaSeparator,sizeof(int)); break;

//UPC-E
		case DEC_UPCE0_ENABLED:
			memcpy(data,&UPCE0Enabled,sizeof(int)); break;
		case DEC_UPCE1_ENABLED:
			memcpy(data,&UPCE1Enabled,sizeof(int)); break;
		case DEC_UPCE_EXPAND:
			returnCode = SD_Get(decoder, SD_PROP_UPC_EXPANSION, data ); break;
		case DEC_UPCE_CHECK_DIGIT_TRANSMIT:
			memcpy(data,&UPCECheckDigitTransmit,sizeof(int)); break;
		case DEC_UPCE_NUMBER_SYSTEM_TRANSMIT:
			memcpy(data,&UPCENumberSystemTransmit,sizeof(int)); break;
		case DEC_UPCE_2CHAR_ADDENDA_ENABLED:
			memcpy(data,&UPCE2CharAddendaEnabled,sizeof(int)); break;
		case DEC_UPCE_5CHAR_ADDENDA_ENABLED:
			memcpy(data,&UPCE5CharAddendaEnabled,sizeof(int)); break;
		case DEC_UPCE_ADDENDA_REQUIRED:
			memcpy(data,&UPCEAddendaRequired,sizeof(int)); break;
		case DEC_UPCE_ADDENDA_SEPARATOR:
			memcpy(data,&UPCEAddendaSeparator,sizeof(int)); break;

//EAN-8
		case DEC_EAN8_ENABLED:
			memcpy(data,&EAN8Enabled,sizeof(int)); break;
		case DEC_EAN8_CHECK_DIGIT_TRANSMIT:
			memcpy(data,&EAN8CheckDigitTransmit,sizeof(int)); break;
		case DEC_EAN8_2CHAR_ADDENDA_ENABLED:
			memcpy(data,&EAN82CharAddendaEnabled,sizeof(int)); break;
		case DEC_EAN8_5CHAR_ADDENDA_ENABLED:
			memcpy(data,&EAN85CharAddendaEnabled,sizeof(int)); break;
		case DEC_EAN8_ADDENDA_REQUIRED:
			memcpy(data,&EAN8AddendaRequired,sizeof(int)); break;
		case DEC_EAN8_ADDENDA_SEPARATOR:
			memcpy(data,&EAN8AddendaSeparator,sizeof(int)); break;

//EAN-13
		case DEC_EAN13_ENABLED:
			memcpy(data,&EAN13Enabled,sizeof(int)); break;
		case DEC_EAN13_CHECK_DIGIT_TRANSMIT:
			memcpy(data,&EAN13CheckDigitTransmit,sizeof(int)); break;
		case DEC_EAN13_2CHAR_ADDENDA_ENABLED:
			memcpy(data,&EAN132CharAddendaEnabled,4); break;
		case DEC_EAN13_5CHAR_ADDENDA_ENABLED:
			memcpy(data,&EAN135CharAddendaEnabled,sizeof(int)); break;
		case DEC_EAN13_ADDENDA_REQUIRED:
			memcpy(data,&EAN13AddendaRequired,sizeof(int)); break;
		case DEC_EAN13_ADDENDA_SEPARATOR:
			memcpy(data,&EAN13AddendaSeparator,sizeof(int)); break;
		case DEC_EAN13_ISBN_ENABLED: 
			memcpy(data,&EAN13ISBNEnabled,sizeof(int)); break;

//UPC-Misc
		case DEC_UPC_IMPROVE_BOUNDS:
			returnCode = SD_Get(decoder, SD_PROP_UPC_IMPROVE_BOUNDS, data ); break;
		case DEC_UPC_SHORT_MARGIN:
			returnCode = SD_Get(decoder, SD_PROP_UPC_SHORT_MARGIN, data ); break;

//Code 128
		case DEC_CODE128_ENABLED:
			memcpy(data,&Code128Enabled,sizeof(int)); break;
		case DEC_CODE128_MIN_LENGTH:
			memcpy(data,&Code128Min,sizeof(int)); break;
		case DEC_CODE128_MAX_LENGTH:
			memcpy(data,&Code128Max,sizeof(int)); break;

//GS1-128
		case DEC_GS1_128_ENABLED:
			memcpy(data,&GS1128Enabled,sizeof(int)); break;
		case DEC_GS1_128_MIN_LENGTH:
			memcpy(data,&GS1128Min,sizeof(int)); break;
		case DEC_GS1_128_MAX_LENGTH:
			memcpy(data,&GS1128Max,sizeof(int)); break;

//Code 128/GS1-128 Misc
		case DEC_C128_FNC_TRANSMIT:
			memcpy(data,&Code128FncTransmit,sizeof(int)); break;
		case DEC_C128_IMPROVE_BOUNDS:
			returnCode = SD_Get(decoder, SD_PROP_C128_IMPROVE_BOUNDS, data ); break;
		case DEC_C128_PARTIAL:
			returnCode = SD_Get(decoder, SD_PROP_C128_PARTIAL, data ); break;
		case DEC_C128_SHORT_MARGIN:
			returnCode = SD_Get(decoder, SD_PROP_C128_SHORT_MARGIN, data ); break;
		case DEC_C128_SUPPRESS_CODABLOCK_CONFLICT:
			returnCode = SD_Get(decoder, SD_PROP_C128_SUPPRESS_CB_CONFLICT, data ); break;

//Code 39
		case DEC_CODE39_ENABLED:
			returnCode = SD_Get(decoder, SD_PROP_C39_ENABLED, data ); break;
		case DEC_CODE39_MIN_LENGTH:
			memcpy(data,&Code39Min,sizeof(int)); break;
		case DEC_CODE39_MAX_LENGTH:
			memcpy(data,&Code39Max,sizeof(int)); break;
		case DEC_CODE39_START_STOP_TRANSMIT:
			memcpy(data,&Code39StartStopTransmit,sizeof(int)); break;
		case DEC_CODE39_BASE32_ENABLED:
			memcpy(data,&Code39Base32Enabled,sizeof(int)); break;
		case DEC_CODE39_CHECK_DIGIT_MODE:
			returnCode = SD_Get(decoder, SD_PROP_C39_CHECKSUM, data ); break;
		case DEC_CODE39_FULL_ASCII_ENABLED:
			returnCode = SD_Get(decoder, SD_PROP_C39_FULL_ASCII, data ); break;
		case DEC_CODE39_IMPROVE_BOUNDS:
			returnCode = SD_Get(decoder, SD_PROP_C39_IMPROVE_BOUNDS, data ); break;
		case DEC_CODE39_PARTIAL:
			returnCode = SD_Get(decoder, SD_PROP_C39_PARTIAL, data ); break;
		case DEC_CODE39_SHORT_MARGIN:
			returnCode = SD_Get(decoder, SD_PROP_C39_SHORT_MARGIN, data ); break;
		case DEC_CODE39_SUPPRESS_CODABLOCK_CONFLICT:
			returnCode = SD_Get(decoder, SD_PROP_C39_SUPPRESS_CB_CONFLICT, data ); break;

//Pharmacode
		case DEC_PHARMACODE_ENABLED:
			returnCode = SD_Get(decoder, SD_PROP_PHARMA_ENABLED, data ); break;
		case DEC_PHARMACODE_MIN_LENGTH:
			returnCode = SD_Get(decoder, SD_PROP_PHARMA_MIN_VALUE, data ); break;
		case DEC_PHARMACODE_MAX_LENGTH:
			returnCode = SD_Get(decoder, SD_PROP_PHARMA_MAX_VALUE, data ); break;
		case DEC_PHARMACODE_MIN_BAR_COUNT:
			returnCode = SD_Get(decoder, SD_PROP_PHARMA_MIN_BAR_COUNT, data ); break;
		case DEC_PHARMACODE_MAX_BAR_COUNT:
			returnCode = SD_Get(decoder, SD_PROP_PHARMA_MAX_BAR_COUNT, data ); break;
		case DEC_PHARMACODE_COLOR_BARS:
			returnCode = SD_Get(decoder, SD_PROP_PHARMA_COLOR_BARS, data ); break;
		case DEC_PHARMACODE_ORIENTATION:
			returnCode = SD_Get(decoder, SD_PROP_PHARMA_ORIENTATION, data ); break;
		case DEC_PHARMACODE_REVERSE:
			returnCode = SD_Get(decoder, SD_PROP_PHARMA_REVERSE, data ); break;

//TLC 39
		case DEC_TLC39_ENABLED:
			memcpy(data,&TLC39CompositeEnabled,sizeof(int));
			break;

//Trioptic
		case DEC_TRIOPTIC_ENABLED:	
			returnCode = SD_Get(decoder, SD_PROP_TRIOPTIC_ENABLED, data ); break;
		case DEC_TRIOPTIC_SHORT_MARGIN:
			returnCode = SD_Get(decoder, SD_PROP_TRIOPTIC_SHORT_MARGIN, data ); break;

//Interleaved 2 of 5
		case DEC_I25_ENABLED:
			returnCode = SD_Get(decoder, SD_PROP_I25_ENABLED, data ); break;
		case DEC_I25_MIN_LENGTH:
			memcpy(data,&Interleaved25Min,sizeof(int)); break;
		case DEC_I25_MAX_LENGTH:
			memcpy(data,&Interleaved25Max,sizeof(int)); break;
		case DEC_I25_CHECK_DIGIT_MODE:
			memcpy(data,&Interleaved25CheckDigitMode,sizeof(int)); break;
		case DEC_I25_IMPROVE_BOUNDS:
			returnCode = SD_Get(decoder, SD_PROP_I25_IMPROVE_BOUNDS, data ); break;
		case DEC_I25_PARTIAL:
			returnCode = SD_Get(decoder, SD_PROP_I25_PARTIAL, data ); break;
		case DEC_I25_SHORT_MARGIN:
			returnCode = SD_Get(decoder, SD_PROP_I25_SHORT_MARGIN, data ); break;

//Standard 2 of 5
		case DEC_S25_ENABLED:
			returnCode = SD_Get(decoder, SD_PROP_S25_3SS_ENABLED, data ); break;
		case DEC_S25_MIN_LENGTH:
			memcpy(data,&Standard250Min,sizeof(int)); break;
		case DEC_S25_MAX_LENGTH:
			memcpy(data,&Standard250Max,sizeof(int)); break;
		case DEC_S25_IMPROVE_BOUNDS:
			returnCode = SD_Get(decoder, SD_PROP_S25_3SS_IMPROVE_BOUNDS, data ); break;

//IATA 2 of 5
		case DEC_IATA25_ENABLED:
			returnCode = SD_Get(decoder, SD_PROP_S25_2SS_ENABLED, data ); break;
		case DEC_IATA25_MIN_LENGTH:
			memcpy(data,&Standard251Min,sizeof(int)); break;
		case DEC_IATA25_MAX_LENGTH:
			memcpy(data,&Standard251Max,sizeof(int)); break;
		case DEC_IATA25_IMPROVE_BOUNDS:
			returnCode = SD_Get(decoder, SD_PROP_S25_2SS_IMPROVE_BOUNDS, data ); break;

//Matrix 2 of 5
		case DEC_M25_ENABLED:
			returnCode = SD_Get(decoder, SD_PROP_M25_ENABLED, data ); break;
		case DEC_M25_MIN_LENGTH:
			memcpy(data,&Matrix25Min,sizeof(int)); break;
		case DEC_M25_MAX_LENGTH:
			memcpy(data,&Matrix25Max,sizeof(int)); break;
		case DEC_M25_CHECK_DIGIT_MODE:
			returnCode = SD_Get(decoder, SD_PROP_M25_CHECKSUM, data ); break;
		case DEC_M25_IMPROVE_BOUNDS:
			returnCode = SD_Get(decoder, SD_PROP_M25_IMPROVE_BOUNDS, data ); break;

//NEC 2 of 5
		case DEC_NEC25_ENABLED:
			returnCode = SD_Get(decoder, SD_PROP_NEC25_ENABLED, data ); break;
		case DEC_NEC25_MIN_LENGTH:
			memcpy(data,&NEC25Min,sizeof(int)); break;
		case DEC_NEC25_MAX_LENGTH:
			memcpy(data,&NEC25Max,sizeof(int)); break;
		case DEC_NEC25_CHECK_DIGIT_MODE:
			returnCode = SD_Get(decoder, SD_PROP_NEC25_CHECKSUM, data ); break;
		case DEC_NEC25_IMPROVE_BOUNDS:
			returnCode = SD_Get(decoder, SD_PROP_NEC25_IMPROVE_BOUNDS, data ); break;

//Code 93
		case DEC_CODE93_ENABLED:
			returnCode = SD_Get(decoder, SD_PROP_C93_ENABLED, data ); break;
		case DEC_CODE93_MIN_LENGTH:
			memcpy(data,&Code93Min,sizeof(int)); break;
		case DEC_CODE93_MAX_LENGTH:
			memcpy(data,&Code93Max,sizeof(int)); break;
		case DEC_CODE93_IMPROVE_BOUNDS:
			returnCode = SD_Get(decoder, SD_PROP_C93_IMPROVE_BOUNDS, data ); break;
		case DEC_CODE93_SHORT_MARGIN:
			returnCode = SD_Get(decoder, SD_PROP_C93_SHORT_MARGIN, data ); break;

//Code 11
		case DEC_CODE11_ENABLED:
			returnCode = SD_Get(decoder, SD_PROP_C11_ENABLED, data ); break;
		case DEC_CODE11_MIN_LENGTH:
			memcpy(data,&Code11Min,sizeof(int)); break;
		case DEC_CODE11_MAX_LENGTH:
			memcpy(data,&Code11Max,sizeof(int)); break;
		case DEC_CODE11_CHECK_DIGIT_MODE:
			returnCode = SD_Get(decoder, SD_PROP_C11_CHECKSUM, data ); break;
		case DEC_CODE11_IMPROVE_BOUNDS:
			returnCode = SD_Get(decoder, SD_PROP_C11_IMPROVE_BOUNDS, data ); break;

//Codabar
		case DEC_CODABAR_ENABLED:
			returnCode = SD_Get(decoder, SD_PROP_CB_ENABLED, data ); break;
		case DEC_CODABAR_MIN_LENGTH:
			memcpy(data,&CodabarMin,sizeof(int)); break;
		case DEC_CODABAR_MAX_LENGTH:
			memcpy(data,&CodabarMax,sizeof(int)); break;
		case DEC_CODABAR_CHECK_DIGIT_MODE:
			returnCode = SD_Get(decoder, SD_PROP_CB_CHECKSUM, data ); break;
		case DEC_CODABAR_IMPROVE_BOUNDS:
			returnCode = SD_Get(decoder, SD_PROP_CB_IMPROVE_BOUNDS, data ); break;
		case DEC_CODABAR_SHORT_MARGIN:
			returnCode = SD_Get(decoder, SD_PROP_CB_SHORT_MARGIN, data ); break;

//Telepen
		case DEC_TELEPEN_ENABLED:
			returnCode = SD_Get(decoder, SD_PROP_TP_ENABLED, data ); break;
		case DEC_TELEPEN_MIN_LENGTH:
			memcpy(data,&TelepenMin,sizeof(int)); break;
		case DEC_TELEPEN_MAX_LENGTH:
			memcpy(data,&TelepenMax,sizeof(int)); break;
		case DEC_TELEPEN_OLD_STYLE:
			returnCode = SD_Get(decoder, SD_PROP_TP_NUM_START, data ); break;
		case DEC_TELEPEN_IMPROVE_BOUNDS:
			returnCode = SD_Get(decoder, SD_PROP_TP_IMPROVE_BOUNDS, data ); break;

//MSI
		case DEC_MSI_ENABLED:
			returnCode = SD_Get(decoder, SD_PROP_MSIP_ENABLED, data ); break;
		case DEC_MSI_MIN_LENGTH:
			memcpy(data,&MSIMin,sizeof(int)); break;
		case DEC_MSI_MAX_LENGTH:
			memcpy(data,&MSIMax,sizeof(int)); break;
		case DEC_MSI_CHECK_DIGIT_MODE:
			returnCode = SD_Get(decoder, SD_PROP_MSIP_CHECKSUM, data ); break;
		case DEC_MSI_IMPROVE_BOUNDS:
			returnCode = SD_Get(decoder, SD_PROP_MSIP_IMPROVE_BOUNDS, data ); break;

//RSS (GS1)
		case DEC_RSS_14_ENABLED:
			memcpy(data,&RSS14Enabled,sizeof(int)); break;
		case DEC_RSS_LIMITED_ENABLED: 
			memcpy(data,&RSSLimitedEnabled,sizeof(int)); break;
		case DEC_RSS_EXPANDED_ENABLED:
			memcpy(data,&RSSExpandedEnabled,sizeof(int)); break;
		case DEC_RSS_EXPANDED_MIN_LENGTH:
			memcpy(data,&RSSExpandedMin,sizeof(int)); break;
		case DEC_RSS_EXPANDED_MAX_LENGTH:
			memcpy(data,&RSSExpandedMax,sizeof(int)); break;

//Korea Post
		case DEC_KOREA_POST_ENABLED:
			returnCode = SD_Get(decoder, SD_PROP_KP_ENABLED, data ); break;
		case DEC_KOREA_POST_MIN_LENGTH:
			memcpy(data,&KoreaPostMin,sizeof(int)); break;
		case DEC_KOREA_POST_MAX_LENGTH:
			memcpy(data,&KoreaPostMax,sizeof(int)); break;
		case DEC_KOREA_POST_CHECK_DIGIT_TRANSMIT:
			memcpy( data,&KoreaPostCheckDigitTransmit,sizeof(int));
			//returnCode = SD_Get(decoder, SD_PROP_KP_CHECKSUM, data ); 
			break;
		case DEC_KOREA_POST_REVERSE:
			returnCode = SD_Get(decoder, SD_PROP_KP_REVERSE, data ); break;

/* Stacked Linear Symbologies */

//Codablock A
		case DEC_CODABLOCK_A_ENABLED: 
			returnCode = SD_Get(decoder, SD_PROP_CODABLOCK_A_ENABLED, data ); break;
		case DEC_CODABLOCK_A_MIN_LENGTH:
			memcpy(data,&CodablockAMin,sizeof(int)); break;
		case DEC_CODABLOCK_A_MAX_LENGTH:
			memcpy(data,&CodablockAMax,sizeof(int)); break;

//Codablock F
		case DEC_CODABLOCK_F_ENABLED: 
			returnCode = SD_Get(decoder, SD_PROP_CODABLOCK_F_ENABLED, data ); break;
		case DEC_CODABLOCK_F_MIN_LENGTH:
			memcpy(data,&CodablockFMin,sizeof(int)); break;
		case DEC_CODABLOCK_F_MAX_LENGTH:
			memcpy(data,&CodablockFMax,sizeof(int)); break;

//PDF417
		case DEC_PDF417_ENABLED:
			returnCode = SD_Get(decoder, SD_PROP_PDF_ENABLED, data ); break;
		case DEC_PDF417_MIN_LENGTH:
			memcpy(data,&PDF417Min,sizeof(int)); break;
		case DEC_PDF417_MAX_LENGTH:
			memcpy(data,&PDF417Max,sizeof(int)); break;
		//case DEC_PDF417_SHOW_GLI:

//Micro PDF417
		case DEC_MICROPDF_ENABLED:
			returnCode = SD_Get(decoder, SD_PROP_MICROPDF_ENABLED, data ); break;
		case DEC_MICROPDF_MIN_LENGTH:
			memcpy(data,&MicroPDFMin,sizeof(int)); break;
		case DEC_MICROPDF_MAX_LENGTH:
			memcpy(data,&MicroPDFMax,sizeof(int)); break;
		case DEC_MICROPDF_IMPROVE_BOUNDS:
			returnCode = SD_Get(decoder, SD_PROP_MICROPDF_IMPROVE_BOUNDS, data ); break;

//Composite
		case DEC_COMPOSITE_ENABLED:
			memcpy(data,&CompositeEnabled,sizeof(int)); break;
			//returnCode = SD_Get(decoder, SD_PROP_CC_ENABLED, data ); break;


/* Matrix 2D Symbologies */

//Aztec Code
		case DEC_AZTEC_ENABLED:
			memcpy(data,&AztecEnabled,sizeof(int)); break;
		case DEC_AZTEC_MIN_LENGTH:
			memcpy(data,&AztecMin,sizeof(int)); break;
		case DEC_AZTEC_MAX_LENGTH:
			memcpy(data,&AztecMax,sizeof(int)); break;
		case DEC_AZTEC_SYMBOL_SIZE:
			returnCode = SD_Get(decoder, SD_PROP_AZ_SYMBOL_SIZE, data ); break;

//Maxicode
		case DEC_MAXICODE_ENABLED:
			returnCode = SD_Get(decoder, SD_PROP_MC_ENABLED, data ); break;
		case DEC_MAXICODE_SYMBOL_SIZE:
			returnCode = SD_Get(decoder, SD_PROP_MC_SYMBOL_SIZE, data ); break;
		case DEC_MAXICODE_MIN_LENGTH:
			memcpy(data,&MaxicodeMin,sizeof(int)); break;
		case DEC_MAXICODE_MAX_LENGTH:
			memcpy(data,&MaxicodeMax,sizeof(int)); break;

//DataMatrix
		case DEC_DATAMATRIX_ENABLED:
			returnCode = SD_Get(decoder, SD_PROP_DM_ENABLED, data ); break;
		case DEC_DATAMATRIX_MIN_LENGTH:
			memcpy(data,&DataMatrixMin,sizeof(int)); break;
		case DEC_DATAMATRIX_MAX_LENGTH:
			memcpy(data,&DataMatrixMax,sizeof(int)); break;
		case DEC_DATAMATRIX_MIN_MODULE_COUNT:
			returnCode = SD_Get(decoder, SD_PROP_DM_MIN_MODULE_COUNT, data ); break;
		case DEC_DATAMATRIX_MAX_MODULE_COUNT:
			returnCode = SD_Get(decoder, SD_PROP_DM_MAX_MODULE_COUNT, data ); break;
		case DEC_DATAMATRIX_MIN_MODULE_SIZE:
			returnCode = SD_Get(decoder, SD_PROP_DM_MIN_MODULE_SIZE, data ); break;
		case DEC_DATAMATRIX_MAX_MODULE_SIZE:
			returnCode = SD_Get(decoder, SD_PROP_DM_MAX_MODULE_SIZE, data ); break;
		case DEC_DATAMATRIX_ORIENTATIONS:
			returnCode = SD_Get(decoder, SD_PROP_DM_ORIENTATIONS, data ); break;
		case DEC_DATAMATRIX_BINARY_IMPROVEMENTS:
			returnCode = SD_Get(decoder, SD_PROP_DM_BINARY_IMPROVEMENTS, data ); break;
		case DEC_DATAMATRIX_NON_SQUARE_MODULES:
			returnCode = SD_Get(decoder, SD_PROP_DM_NON_SQUARE_MODULES, data ); break;
		case DEC_DATAMATRIX_SHIFTED_TILES:
			returnCode = SD_Get(decoder, SD_PROP_DM_SHIFTED_TILES, data ); break;
		case DEC_DATAMATRIX_LOW_CONTRAST:
			returnCode = SD_Get(decoder, SD_PROP_DM_LOW_CONTRAST, data ); break;
		case DEC_DATAMATRIX_SYMBOL_SIZE:
			returnCode = SD_Get(decoder, SD_PROP_DM_SYMBOL_SIZE, data ); break;
		case DEC_DATAMATRIX_RECTANGLE:
			returnCode = SD_Get(decoder, SD_PROP_DM_RECT, data ); break;
		case DEC_DATAMATRIX_SUBREGION_HEIGHT:
			returnCode = SD_Get(decoder, SD_PROP_DM_SUBREGION_HEIGHT, data ); break;
		case DEC_DATAMATRIX_SUBREGION_LEFT:
			returnCode = SD_Get(decoder, SD_PROP_DM_SUBREGION_LEFT, data ); break;
		case DEC_DATAMATRIX_SUBREGION_TOP:
			returnCode = SD_Get(decoder, SD_PROP_DM_SUBREGION_TOP, data ); break;
		case DEC_DATAMATRIX_SUBREGION_WIDTH:
			returnCode = SD_Get(decoder, SD_PROP_DM_SUBREGION_WIDTH, data ); break;

//QR Code
		case DEC_QR_ENABLED:
			returnCode = SD_Get(decoder, SD_PROP_QR_ENABLED, data ); break;
		case DEC_QR_MIN_LENGTH:
			memcpy(data,&QRMin,sizeof(int)); break;
		case DEC_QR_MAX_LENGTH:
			memcpy(data,&QRMax,sizeof(int)); break;
		case DEC_QR_NON_SQUARE_MODULES:
			returnCode = SD_Get(decoder, SD_PROP_QR_NON_SQUARE_MODULES, data ); break;
		case DEC_QR_SYMBOL_SIZE:
			returnCode = SD_Get(decoder, SD_PROP_QR_SYMBOL_SIZE, data ); break;

//HanXin
		case DEC_HANXIN_ENABLED:
			returnCode = SD_Get(decoder, SD_PROP_HX_ENABLED, data ); break;
		case DEC_HANXIN_MIN_LENGTH:
			memcpy(data,&HanXinMin,sizeof(int)); break;
		case DEC_HANXIN_MAX_LENGTH:
			memcpy(data,&HanXinMax,sizeof(int)); break;

//Go Code
		case DEC_GOCODE_ENABLED:
			returnCode = SD_Get(decoder, SD_PROP_GC_ENABLED, data ); break;

//Hong Kong 2 of 5
		case DEC_HK25_ENABLED:
			returnCode = SD_Get(decoder, SD_PROP_HK25_ENABLED, data ); break;
		case DEC_HK25_MIN_LENGTH:
			memcpy(data,&HongKong25Min,sizeof(int)); break;
		case DEC_HK25_MAX_LENGTH:
			memcpy(data,&HongKong25Max,sizeof(int)); break;
		case DEC_HK25_IMPROVE_BOUNDS:
			returnCode = SD_Get(decoder, SD_PROP_HK25_IMPROVE_BOUNDS, data ); break;

/* OCR */
		case DEC_OCR_MODE:
			returnCode = SD_Get(decoder, SD_PROP_OCR_ENABLED, data ); break;
		case DEC_OCR_TEMPLATE:
			returnCode = SD_Get(decoder, SD_PROP_OCR_USER_TEMPLATES, data ); break;
		case DEC_OCR_ACTIVE_TEMPLATES:
			returnCode = SD_Get(decoder, SD_PROP_OCR_ACTIVE_TEMPLATES, data ); break;
		case DEC_OCR_BUSY_BACKGROUND:
			returnCode = SD_Get(decoder, SD_PROP_OCR_BUSY_BACKGROUND, data ); break;
		case DEC_IMAGE_ROTATION:
			SD_Get( decoder, SD_PROP_OCR_PREFERRED_ORIENTATION, &returnCode );
			switch( returnCode ) {
				case 0: returnCode = 1; break;
				case 1: returnCode = 3; break;
				case 2: returnCode = 2; break;
				case 3: returnCode = 0; break;
				default:  break;
			}
			memcpy( data, &returnCode, sizeof(int) );
			returnCode = DEC_SUCCESS;
			break;

/* Postal Symbologies */

//Enabled for all postal symbologies
		case DEC_POSTAL_ENABLED:
			memcpy(data,&PostalEnabled,sizeof(int)); break;
		case DEC_POSTAL_ENABLED_DIRECT:
			returnCode = SD_Get(decoder, SD_PROP_POSTAL_ENABLED, data ); break;
//PostNet
		case DEC_POSTNET_CHECK_DIGIT_TRANSMIT:
			memcpy(data,&PostnetCheckDigitTransmit,sizeof(int)); break;

//Planet Code
		case DEC_PLANETCODE_CHECK_DIGIT_TRANSMIT:
			memcpy(data,&PlanetCodeCheckDigitTransmit,sizeof(int)); break;
//Royal Mail 
		case DEC_ROYAL_MAIL_FORMAT_CHECK_MIN_LENGTH:
			returnCode = SD_Get(decoder, SD_PROP_POSTAL_RM_MISC, data); break;                                   
//Australia Post
		case DEC_AUS_POST_BAR_OUTPUT_ENABLED:
			returnCode = SD_Get(decoder, SD_PROP_POSTAL_AP_BAR_OUTPUT, data ); break;
		case DEC_AUS_POST_INTERPRET_MODE:
			returnCode = SD_Get(decoder, SD_PROP_POSTAL_AP_CUST_INTERPRET, data ); break;
		case DEC_AUS_POST_ZERO_FCC:
			returnCode = SD_Get(decoder, SD_PROP_POSTAL_AP_ZERO_FCC, data ); break;

		case DEC_CAN_POST_BAR_OUTPUT:
			returnCode = SD_Get(decoder, SD_PROP_POSTAL_CP_BAR_OUTPUT, data ); break;

//Misc Postal - applies to all postal symbologies
		case DEC_POSTAL_MIN_BAR_COUNT:
			returnCode = SD_Get(decoder, SD_PROP_POSTAL_MIN_BAR_COUNT, data ); break;
		case DEC_POSTAL_MAX_BAR_COUNT:
			returnCode = SD_Get(decoder, SD_PROP_POSTAL_MAX_BAR_COUNT, data ); break;
		case DEC_POSTAL_ORIENTATIONS:
			returnCode = SD_Get(decoder, SD_PROP_POSTAL_ORIENTATIONS, data ); break;
		case DEC_POSTAL_SUBREGION_HEIGHT:
			returnCode = SD_Get(decoder, SD_PROP_POSTAL_SUBREGION_HEIGHT, data ); break;
		case DEC_POSTAL_SUBREGION_LEFT:
			returnCode = SD_Get(decoder, SD_PROP_POSTAL_SUBREGION_LEFT, data ); break;
		case DEC_POSTAL_SUBREGION_TOP:
			returnCode = SD_Get(decoder, SD_PROP_POSTAL_SUBREGION_TOP, data ); break;
		case DEC_POSTAL_SUBREGION_WIDTH:
			returnCode = SD_Get(decoder, SD_PROP_POSTAL_SUBREGION_WIDTH, data ); break;
		case DEC_POSTAL_UNDECODABLE_HEIGHT:
			returnCode = SD_Get(decoder, SD_PROP_POSTAL_UNDECODABLE_HEIGHT, data ); break;
		case DEC_POSTAL_UNDECODABLE_LEFT:
			returnCode = SD_Get(decoder, SD_PROP_POSTAL_UNDECODABLE_LEFT, data ); break;
		case DEC_POSTAL_UNDECODABLE_TOP:
			returnCode = SD_Get(decoder, SD_PROP_POSTAL_UNDECODABLE_TOP, data ); break;
		case DEC_POSTAL_UNDECODABLE_WIDTH:
			returnCode = SD_Get(decoder, SD_PROP_POSTAL_UNDECODABLE_WIDTH, data ); break;

		default: returnCode = SD_Get(decoder, tag, data); break; //default, just pass through
	}
	return returnCode;
}

int SI_DecodeSet( int decoder, unsigned int tag, void *data ) {
	int returnCode = DEC_SUCCESS; 
	int symb;
	int lengths = 0xFFFFFFFC;

	switch( tag ) {
		case DEC_MONOCOLOR_ENABLED:
			returnCode = SD_Set(decoder, ID_PROP_MONOCOLOR_CORRECTION_ENABLE, data); break;
		case DEC_IMAGE_HEIGHT:
			returnCode = SD_Set(decoder, SD_PROP_IMAGE_HEIGHT, data ); break;
		case DEC_IMAGE_WIDTH:
			returnCode = SD_Set(decoder, SD_PROP_IMAGE_WIDTH, data ); break;
		case DEC_IMAGE_CENTER_X:
			returnCode = SD_Set(decoder, SD_PROP_IMAGE_SEARCH_CENTER_X, data ); break;
		case DEC_IMAGE_CENTER_Y:
			returnCode = SD_Set(decoder, SD_PROP_IMAGE_SEARCH_CENTER_Y, data ); break;
		case DEC_IMAGE_LINE_DELTA:
			returnCode = SD_Set(decoder, SD_PROP_IMAGE_LINE_DELTA, data ); break;
		case DEC_IMAGE_MIRRORED:
			returnCode = SD_Set(decoder, SD_PROP_IMAGE_MIRRORED, data ); break;
		case DEC_USE_MLD: 
			UseMLD = (int)data;
			if( UseMLD ) {
				returnCode = SD_Set(decoder, ID_PROP_USE_MLD, (void *)(SD_CONST_C128+SD_CONST_UPC+SD_CONST_C39) ); 
			}
			else {
				returnCode = SD_Set(decoder, ID_PROP_USE_MLD, (void *)SD_CONST_DISABLED ); 
			}
			break;
		case DEC_USE_DISTANCE_MAP: 
			returnCode = SD_Set(decoder, ID_PROP_USE_DISTANCE_MAP, data ); break;
		case DEC_EDGE_DETECTOR: 
			returnCode = SD_Set(decoder, ID_PROP_EDGE_DETECTOR, data ); break;
		case DEC_CYCLING_FINDER: 
			returnCode = SD_Set(decoder, ID_PROP_CYCLING_FINDER, data ); break;
		case DEC_SUBPIXEL_FINDER: 
			returnCode = SD_Set(decoder, ID_PROP_SUBPIXEL_FINDER, data ); break;
		case DEC_USE_SUBPIXEL_MLD: 
			returnCode = SD_Set(decoder, ID_PROP_USE_SUBPIXEL_MLD, data ); break;

		case DEC_BLACK_LEVEL: 
			returnCode = SD_Set(decoder, SD_PROP_MISC_BLACK_LEVEL, data ); break;
		case DEC_OPERATING_MODE: 
			returnCode = SD_Set(decoder, SD_PROP_MISC_OP_MODE, data ); break;
		case DEC_TYPICAL_IMAGE_DENSITY: 
			returnCode = SD_Set(decoder, SD_PROP_IMAGE_TYP_DENSITY, data ); break;
// Advanced
		case DEC_CELL_PHONE_IMPROVEMENTS: 
			returnCode = SD_Set(decoder, SD_PROP_MISC_CELLPHONE_IMPROVEMENTS, data ); break;
		case DEC_ECI_HANDLING: 
			returnCode = SD_Set(decoder, SD_PROP_MISC_ECI_HANDLING, data ); break;
		case DEC_GENERAL_IMPROVEMENTS: 
			returnCode = SD_Set(decoder, SD_PROP_MISC_IMPROVEMENTS, data ); break;
		case DEC_ISSUE_IDENTICAL_SYMBOLS: 
			returnCode = SD_Set(decoder, SD_PROP_MISC_ISSUE_IDENTICAL_SYMBOLS, data ); break;
		case DEC_ISSUE_IDENTICAL_SPACING: 
			returnCode = SD_Set(decoder, SD_PROP_MISC_ISSUE_IDENTICAL_SPACING, data ); break;
		case DEC_LOW_ASPECT_RATIO: 
			returnCode = SD_Set(decoder, SD_PROP_MISC_LOW_ASPECT_RATIO, data ); break;
		case DEC_LOW_CONTRAST_IMPROVEMENTS: 
			returnCode = SD_Set(decoder, SD_PROP_MISC_LOW_CONTRAST_IMPROVEMENTS, data ); break;
		case DEC_MISENCODED_SYMBOLS: 
			returnCode = SD_Set(decoder, SD_PROP_MISC_MISENCODED_SYMBOLS, data ); break;
		case DEC_PASS_THROUGH: 
			returnCode = SD_Set(decoder, SD_PROP_MISC_PASS_THROUGH, data ); break;
		case DEC_UNDECODABLE_SYMBOLS: 
			returnCode = SD_Set(decoder, SD_PROP_MISC_UNDECODABLE_SYMBOLS, data ); break;
		case DEC_UNDECODABLE_SYMBOLS_EX: 
			returnCode = SD_Set(decoder, SD_PROP_MISC_UNDECODABLE_SYMBOLS_EX, data ); break;
		case DEC_SUBREGION_PROCESSING: 
			returnCode = SD_Set(decoder, SD_PROP_MISC_SUBREGION_PROCESSING, data ); break;

// Linear Specific
		case DEC_SUBREGION_HEIGHT: 
			returnCode = SD_Set(decoder, SD_PROP_LINEAR_SUBREGION_HEIGHT, data ); break;
		case DEC_SUBREGION_LEFT: 
			returnCode = SD_Set(decoder, SD_PROP_LINEAR_SUBREGION_LEFT, data ); break;
		case DEC_SUBREGION_TOP: 
			returnCode = SD_Set(decoder, SD_PROP_LINEAR_SUBREGION_TOP, data ); break;
		case DEC_SUBREGION_WIDTH: 
			returnCode = SD_Set(decoder, SD_PROP_LINEAR_SUBREGION_WIDTH, data ); break;

//Misc Commands
		case DEC_RESET_DECODER: 
			SI_DecodeReset(decoder); break;
		case DEC_DISABLE_SYMBOLOGY_GROUP:
			//if turning on, get all of the settings and save them off
			if( ((int)data & ONE_DIMENSIONAL_CODES) && (DisableSymbologyGroup & ONE_DIMENSIONAL_CODES) == 0 ) {
				symb = 0;
				while( DisabledGroupSettings[symb][0] != 0 && symb < MAX_DISABLED ) {
					symb++;
				}

				if( symb >= (MAX_DISABLED-MAX_LINEARS) ) {
					Pt("failure: not enough disabled storage:%d\r\n",__LINE__);
					return DEC_FAILURE;
				}

				DisabledGroupSettings[symb][0] = DEC_UPCA_ENABLED;
				DisabledGroupSettings[symb][1] = UPCAEnabled;
				UPCAEnabled = 0; symb++;
				DisabledGroupSettings[symb][0] = DEC_UPCE0_ENABLED;
				DisabledGroupSettings[symb][1] = UPCE0Enabled;
				UPCE0Enabled = 0; symb++;
				DisabledGroupSettings[symb][0] = DEC_UPCE1_ENABLED;
				DisabledGroupSettings[symb][1] = UPCE1Enabled;
				UPCE1Enabled = 0; symb++;
				DisabledGroupSettings[symb][0] = DEC_EAN8_ENABLED;
				DisabledGroupSettings[symb][1] = EAN8Enabled;
				EAN8Enabled = 0; symb++;
				DisabledGroupSettings[symb][0] = DEC_EAN13_ENABLED;
				DisabledGroupSettings[symb][1] = EAN13Enabled;
				EAN13Enabled = 0; symb++; 
				SD_Set(decoder, SD_PROP_UPC_ENABLED, (void *)DEC_DISABLED );

				DisabledGroupSettings[symb][0] = DEC_CODE128_ENABLED;
				DisabledGroupSettings[symb][1] = Code128Enabled;
				Code128Enabled = 0; symb++;
				DisabledGroupSettings[symb][0] = DEC_GS1_128_ENABLED;
				DisabledGroupSettings[symb][1] = GS1128Enabled;
				GS1128Enabled = 0; symb++;
				SD_Set(decoder, SD_PROP_C128_ENABLED, (void *)DEC_DISABLED );

				DisabledGroupSettings[symb][0] = DEC_CODE39_ENABLED;
				SD_Get( decoder, SD_PROP_C39_ENABLED, &DisabledGroupSettings[symb++][1] );
				SD_Set( decoder, SD_PROP_C39_ENABLED, (void *)DEC_DISABLED );

				DisabledGroupSettings[symb][0] = DEC_TRIOPTIC_ENABLED;
				SD_Get( decoder, SD_PROP_TRIOPTIC_ENABLED, &DisabledGroupSettings[symb++][1] );
				SD_Set( decoder, SD_PROP_TRIOPTIC_ENABLED, (void *)DEC_DISABLED );

				DisabledGroupSettings[symb][0] = DEC_I25_ENABLED;
				SD_Get( decoder, SD_PROP_I25_ENABLED, &DisabledGroupSettings[symb++][1] );
				SD_Set( decoder, SD_PROP_I25_ENABLED, (void *)DEC_DISABLED );

				DisabledGroupSettings[symb][0] = DEC_S25_ENABLED;
				SD_Get( decoder, SD_PROP_S25_3SS_ENABLED, &DisabledGroupSettings[symb++][1] );
				SD_Set( decoder, SD_PROP_S25_3SS_ENABLED, (void *)DEC_DISABLED );

				DisabledGroupSettings[symb][0] = DEC_IATA25_ENABLED;
				SD_Get( decoder, SD_PROP_S25_2SS_ENABLED, &DisabledGroupSettings[symb++][1] );
				SD_Set( decoder, SD_PROP_S25_2SS_ENABLED, (void *)DEC_DISABLED );

				DisabledGroupSettings[symb][0] = DEC_M25_ENABLED;
				SD_Get( decoder, SD_PROP_M25_ENABLED, &DisabledGroupSettings[symb++][1] );
				SD_Set( decoder, SD_PROP_M25_ENABLED, (void *)DEC_DISABLED );

				DisabledGroupSettings[symb][0] = DEC_CODE93_ENABLED;
				SD_Get( decoder, SD_PROP_C93_ENABLED, &DisabledGroupSettings[symb++][1] );
				SD_Set( decoder, SD_PROP_C93_ENABLED, (void *)DEC_DISABLED );

				DisabledGroupSettings[symb][0] = DEC_CODE11_ENABLED;
				SD_Get( decoder, SD_PROP_C11_ENABLED, &DisabledGroupSettings[symb++][1] );
				SD_Set( decoder, SD_PROP_C11_ENABLED, (void *)DEC_DISABLED );

				DisabledGroupSettings[symb][0] = DEC_CODABAR_ENABLED;
				SD_Get( decoder, SD_PROP_CB_ENABLED, &DisabledGroupSettings[symb++][1] );
				SD_Set( decoder, SD_PROP_CB_ENABLED, (void *)DEC_DISABLED );

				DisabledGroupSettings[symb][0] = DEC_TELEPEN_ENABLED;
				SD_Get( decoder, SD_PROP_TP_ENABLED, &DisabledGroupSettings[symb++][1] );
				SD_Set( decoder, SD_PROP_TP_ENABLED, (void *)DEC_DISABLED );

				DisabledGroupSettings[symb][0] = DEC_MSI_ENABLED;
				SD_Get( decoder, SD_PROP_MSIP_ENABLED, &DisabledGroupSettings[symb++][1] );
				SD_Set( decoder, SD_PROP_MSIP_ENABLED, (void *)DEC_DISABLED );

				DisabledGroupSettings[symb][0] = DEC_RSS_14_ENABLED;
				DisabledGroupSettings[symb][1] = RSS14Enabled;
				RSS14Enabled = 0; symb++;
				DisabledGroupSettings[symb][0] = DEC_RSS_LIMITED_ENABLED;
				DisabledGroupSettings[symb][1] = RSSLimitedEnabled;
				RSSLimitedEnabled = 0; symb++;
				DisabledGroupSettings[symb][0] = DEC_RSS_EXPANDED_ENABLED;
				DisabledGroupSettings[symb][1] = RSSExpandedEnabled;
				RSSExpandedEnabled = 0; symb++;
				SD_Set( decoder, SD_PROP_RSS_ENABLED, (void *)DEC_DISABLED );

				DisabledGroupSettings[symb][0] = DEC_KOREA_POST_ENABLED;
				SD_Get( decoder, SD_PROP_KP_ENABLED, &DisabledGroupSettings[symb++][1] );
				SD_Set( decoder, SD_PROP_KP_ENABLED, (void *)DEC_DISABLED );

				DisabledGroupSettings[symb][0] = DEC_HK25_ENABLED;
				SD_Get( decoder, SD_PROP_HK25_ENABLED, &DisabledGroupSettings[symb++][1] );
				SD_Set( decoder, SD_PROP_HK25_ENABLED, (void *)DEC_DISABLED );
				
				DisabledGroupSettings[symb][0] = DEC_NEC25_ENABLED;
				SD_Get( decoder, SD_PROP_NEC25_ENABLED, &DisabledGroupSettings[symb++][1] );
				SD_Set( decoder, SD_PROP_NEC25_ENABLED, (void *)DEC_DISABLED );

				DisabledGroupSettings[symb][0] = DEC_PHARMACODE_ENABLED;
				SD_Get( decoder, SD_PROP_PHARMA_ENABLED, &DisabledGroupSettings[symb++][1] );
				SD_Set( decoder, SD_PROP_PHARMA_ENABLED, (void *)DEC_DISABLED );

				DisabledGroupSettings[symb][0] = DEC_GOCODE_ENABLED;
				SD_Get( decoder, SD_PROP_GC_ENABLED, &DisabledGroupSettings[symb++][1] );
				SD_Set( decoder, SD_PROP_GC_ENABLED, (void *)DEC_DISABLED );
			}
			//if turning off, restore the saved settings
			else if( (DisableSymbologyGroup & ONE_DIMENSIONAL_CODES) && ((int)data & ONE_DIMENSIONAL_CODES) == 0 ) {
				for( symb = 0; symb < MAX_DISABLED; symb++ ) {
					int foundSymb = 1;

					switch( DisabledGroupSettings[symb][0] ) {
						case DEC_UPCA_ENABLED:
							UPCAEnabled = DisabledGroupSettings[symb][1];
							SD_Set( decoder, SD_PROP_UPC_ENABLED, (void *)UPC_ENABLE_MIX );
							break;
						case DEC_UPCE0_ENABLED:
							UPCE0Enabled = DisabledGroupSettings[symb][1];
							SD_Set( decoder, SD_PROP_UPC_ENABLED, (void *)UPC_ENABLE_MIX ); 
							break;
						case DEC_UPCE1_ENABLED:
							UPCE1Enabled = DisabledGroupSettings[symb][1];
							SD_Set( decoder, SD_PROP_UPC_ENABLED, (void *)UPC_ENABLE_MIX );
							break;
						case DEC_EAN8_ENABLED:
							EAN8Enabled = DisabledGroupSettings[symb][1];
							SD_Set( decoder, SD_PROP_UPC_ENABLED, (void *)UPC_ENABLE_MIX );
							break;
						case DEC_EAN13_ENABLED:
							EAN13Enabled = DisabledGroupSettings[symb][1];
							SD_Set( decoder, SD_PROP_UPC_ENABLED, (void *)UPC_ENABLE_MIX );
							break;
						case DEC_CODE128_ENABLED:
							Code128Enabled = DisabledGroupSettings[symb][1];
							SD_Set( decoder, SD_PROP_C128_ENABLED, (void *)1 );
							break;
						case DEC_GS1_128_ENABLED:
							GS1128Enabled = DisabledGroupSettings[symb][1];
							SD_Set( decoder, SD_PROP_C128_ENABLED, (void *)1 );
							break;
						case DEC_CODE39_ENABLED:
							SD_Set( decoder, SD_PROP_C39_ENABLED, (void *)DisabledGroupSettings[symb][1] );
							break;
						case DEC_TRIOPTIC_ENABLED:
							SD_Set( decoder, SD_PROP_TRIOPTIC_ENABLED, (void *)DisabledGroupSettings[symb][1] );
							break;
						case DEC_I25_ENABLED:
							SD_Set( decoder, SD_PROP_I25_ENABLED, (void *)DisabledGroupSettings[symb][1] );
							break;
						case DEC_S25_ENABLED:
							SD_Set( decoder, SD_PROP_S25_3SS_ENABLED, (void *)DisabledGroupSettings[symb][1] );
							break;
						case DEC_IATA25_ENABLED:
							SD_Set( decoder, SD_PROP_S25_2SS_ENABLED, (void *)DisabledGroupSettings[symb][1] );
							break;
						case DEC_M25_ENABLED:
							SD_Set( decoder, SD_PROP_M25_ENABLED, (void *)DisabledGroupSettings[symb][1] );
							break;
						case DEC_CODE93_ENABLED:
							SD_Set( decoder, SD_PROP_C93_ENABLED, (void *)DisabledGroupSettings[symb][1] );
							break;
						case DEC_CODE11_ENABLED:
							SD_Set( decoder, SD_PROP_C11_ENABLED, (void *)DisabledGroupSettings[symb][1] );
							break;
						case DEC_CODABAR_ENABLED:
							SD_Set( decoder, SD_PROP_CB_ENABLED, (void *)DisabledGroupSettings[symb][1] );
							break;
						case DEC_TELEPEN_ENABLED:
							SD_Set( decoder, SD_PROP_TP_ENABLED, (void *)DisabledGroupSettings[symb][1] );
							break;
						case DEC_MSI_ENABLED:
							SD_Set( decoder, SD_PROP_MSIP_ENABLED, (void *)DisabledGroupSettings[symb][1] );
							break;
						case DEC_RSS_14_ENABLED:
							RSS14Enabled = DisabledGroupSettings[symb][1];
							SD_Set( decoder, SD_PROP_RSS_ENABLED, (void *)RSS_ENABLE_MIX );
							break;
						case DEC_RSS_LIMITED_ENABLED:
							RSSLimitedEnabled = DisabledGroupSettings[symb][1];
							SD_Set( decoder, SD_PROP_RSS_ENABLED, (void *)RSS_ENABLE_MIX );
							break;
						case DEC_RSS_EXPANDED_ENABLED:
							RSSExpandedEnabled = DisabledGroupSettings[symb][1];
							SD_Set( decoder, SD_PROP_RSS_ENABLED, (void *)RSS_ENABLE_MIX );
							break;
						case DEC_HK25_ENABLED:
							SD_Set( decoder, SD_PROP_HK25_ENABLED, (void *)DisabledGroupSettings[symb][1] );
							break;
						case DEC_KOREA_POST_ENABLED:
							SD_Set( decoder, SD_PROP_KP_ENABLED, (void *)DisabledGroupSettings[symb][1] );
							break;
						case DEC_NEC25_ENABLED:
							SD_Set( decoder, SD_PROP_NEC25_ENABLED, (void *)DisabledGroupSettings[symb][1] );
							break;
						case DEC_PHARMACODE_ENABLED:
							SD_Set( decoder, SD_PROP_PHARMA_ENABLED, (void *)DisabledGroupSettings[symb][1] );
							break;
						case DEC_GOCODE_ENABLED:
							SD_Set( decoder, SD_PROP_GC_ENABLED, (void *)DisabledGroupSettings[symb][1] );
							break;
						default: foundSymb = 0; break;
					}//switch()

					if( foundSymb ) {
						DisabledGroupSettings[symb][0] = DisabledGroupSettings[symb][1] = 0;
					}
				}//for()
			}
			//if turning on, get all of the settings and save them off
			if( ((int)data & STACKED_LINEAR_CODES) && (DisableSymbologyGroup & STACKED_LINEAR_CODES) == 0 ) {
				symb = 0;
				while( DisabledGroupSettings[symb][0] != 0 && symb < MAX_DISABLED ) {
					symb++;
				}

				if( symb >= (MAX_DISABLED-MAX_STACKED) ) {
					Pt("failure: not enough disabled storage:%d\r\n",__LINE__);
					return DEC_FAILURE;
				}

				DisabledGroupSettings[symb][0] = DEC_CODABLOCK_A_ENABLED;
				SD_Get( decoder, SD_PROP_CODABLOCK_A_ENABLED, &DisabledGroupSettings[symb++][1] );
				SD_Set( decoder, SD_PROP_CODABLOCK_A_ENABLED, (void *)DEC_DISABLED );
				
				DisabledGroupSettings[symb][0] = DEC_CODABLOCK_F_ENABLED;
				SD_Get( decoder, SD_PROP_CODABLOCK_F_ENABLED, &DisabledGroupSettings[symb++][1] );
				SD_Set( decoder, SD_PROP_CODABLOCK_F_ENABLED, (void *)DEC_DISABLED );
				
				DisabledGroupSettings[symb][0] = DEC_PDF417_ENABLED;
				SD_Get( decoder, SD_PROP_PDF_ENABLED, &DisabledGroupSettings[symb++][1] );
				SD_Set( decoder, SD_PROP_PDF_ENABLED, (void *)DEC_DISABLED );
				
				DisabledGroupSettings[symb][0] = DEC_MICROPDF_ENABLED;
				SD_Get( decoder, SD_PROP_MICROPDF_ENABLED, &DisabledGroupSettings[symb++][1] );
				SD_Set( decoder, SD_PROP_MICROPDF_ENABLED, (void *)DEC_DISABLED );
			}
			//if turning off, restore the saved settings
			else if( (DisableSymbologyGroup & STACKED_LINEAR_CODES) && ((int)data & STACKED_LINEAR_CODES) == 0 ) {
				for( symb = 0; symb < MAX_DISABLED; symb++ ) {
					int foundSymb = 1;

					switch( DisabledGroupSettings[symb][0] ) {
						case DEC_CODABLOCK_A_ENABLED:
							SD_Set( decoder, SD_PROP_CODABLOCK_A_ENABLED, (void *)DisabledGroupSettings[symb][1] );
							break;
						case DEC_CODABLOCK_F_ENABLED:
							SD_Set( decoder, SD_PROP_CODABLOCK_F_ENABLED, (void *)DisabledGroupSettings[symb][1] );
							break;
						case DEC_PDF417_ENABLED:
							SD_Set( decoder, SD_PROP_PDF_ENABLED, (void *)DisabledGroupSettings[symb][1] );
							break;
						case DEC_MICROPDF_ENABLED:
							SD_Set( decoder, SD_PROP_MICROPDF_ENABLED, (void *)DisabledGroupSettings[symb][1] );
							break;
						default: foundSymb = 0; break;
					}//switch()

					if( foundSymb ) {
						DisabledGroupSettings[symb][0] = DisabledGroupSettings[symb][1] = 0;
					}
				}
			}

			//always save current setting. If it failed it will have returned before now.
			DisableSymbologyGroup = (int)data;
			break;
//AGC
		case DEC_AGC_EXPOSURE_QUALITY: 
			returnCode = SD_Set(decoder, SD_PROP_AGC_EXPOSURE_QUALITY, data ); break;
		case DEC_AGC_NEXT_GAIN: 
			returnCode = SD_Set(decoder, SD_PROP_AGC_NEXT_GAIN, data ); break;
		case DEC_AGC_SAMPLING_OPTIONS: 
			returnCode = SD_Set(decoder, SD_PROP_AGC_SAMPLING_OPTIONS, data ); break;
			
/* Symbology Specific Settings
 *	These settings are specific for each symbology.
 */

/* Linear Symbologies */

		case DEC_ADD_SEARCH_TIME_ADDENDA:
			AddendaSearchTime = (int)data; break;

//UPC-A
		case DEC_UPCA_ENABLED: 
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				UPCAEnabled = (int)data;
				returnCode = SD_Set(decoder, SD_PROP_UPC_ENABLED, (void *)UPC_ENABLE_MIX ); 
			}
			break;
		case DEC_UPCA_CHECK_DIGIT_TRANSMIT: 
			UPCACheckDigitTransmit = (int)data; break;
		case DEC_UPCA_NUMBER_SYSTEM_TRANSMIT: 
			UPCANumberSystemTransmit = (int)data; break;
		case DEC_UPCA_2CHAR_ADDENDA_ENABLED: 
			UPCA2CharAddendaEnabled = (int)data;
			returnCode = SD_Set(decoder, SD_PROP_UPC_SUPPLEMENTALS, (void *)UPC_ADDENDA_MIX); break;
		case DEC_UPCA_5CHAR_ADDENDA_ENABLED: 
			UPCA5CharAddendaEnabled = (int)data;
			returnCode = SD_Set(decoder, SD_PROP_UPC_SUPPLEMENTALS, (void *)UPC_ADDENDA_MIX ); break;
		case DEC_UPCA_ADDENDA_REQUIRED: 
			UPCAAddendaRequired = (int)data; break;
		case DEC_UPCA_ADDENDA_SEPARATOR: 
			UPCAAddendaSeparator = (int)data; break;
		case DEC_SECURITY_LEVEL:
			{
				int setval;
				setval = ((int)data > 2) ? (1) : (0); // anything less than 3 is "normal" UPC security, 3 = high security.
				returnCode = SD_Set(decoder, SD_PROP_UPC_SECURITY, (void *)setval ); 
				break;
			}

//UPC-E
		case DEC_UPCE0_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else { 
				UPCE0Enabled = (int)data;
				returnCode = SD_Set(decoder, SD_PROP_UPC_ENABLED, (void *)UPC_ENABLE_MIX ); 
			}
			break;
		case DEC_UPCE1_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				UPCE1Enabled = (int)data;
				returnCode = SD_Set(decoder, SD_PROP_UPC_ENABLED, (void *)UPC_ENABLE_MIX ); 
			}
			break;
		case DEC_UPCE_EXPAND: 
			returnCode = SD_Set(decoder, SD_PROP_UPC_EXPANSION, data ); break;
		case DEC_UPCE_CHECK_DIGIT_TRANSMIT: 
			UPCECheckDigitTransmit = (int)data; break;
		case DEC_UPCE_NUMBER_SYSTEM_TRANSMIT: 
			UPCENumberSystemTransmit = (int)data; break;
		case DEC_UPCE_2CHAR_ADDENDA_ENABLED: 
			UPCE2CharAddendaEnabled = (int)data;
			returnCode = SD_Set(decoder, SD_PROP_UPC_SUPPLEMENTALS, (void *)UPC_ADDENDA_MIX ); break;
		case DEC_UPCE_5CHAR_ADDENDA_ENABLED: 
			UPCE5CharAddendaEnabled = (int)data;
			returnCode = SD_Set(decoder, SD_PROP_UPC_SUPPLEMENTALS, (void *)UPC_ADDENDA_MIX ); break;
		case DEC_UPCE_ADDENDA_REQUIRED: 
			UPCEAddendaRequired = (int)data; break;
		case DEC_UPCE_ADDENDA_SEPARATOR: 
			UPCEAddendaSeparator = (int)data; break;

//EAN-8
		case DEC_EAN8_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				EAN8Enabled = (int)data; 
				returnCode = SD_Set(decoder, SD_PROP_UPC_ENABLED, (void *)UPC_ENABLE_MIX ); 
			}
			break;
		case DEC_EAN8_CHECK_DIGIT_TRANSMIT: 
			EAN8CheckDigitTransmit = (int)data; break;
		case DEC_EAN8_2CHAR_ADDENDA_ENABLED: 
			EAN82CharAddendaEnabled = (int)data;
			returnCode = SD_Set(decoder, SD_PROP_UPC_SUPPLEMENTALS, (void *)UPC_ADDENDA_MIX ); break;
		case DEC_EAN8_5CHAR_ADDENDA_ENABLED: 
			EAN85CharAddendaEnabled = (int)data;
			returnCode = SD_Set(decoder, SD_PROP_UPC_SUPPLEMENTALS, (void *)UPC_ADDENDA_MIX ); break;
		case DEC_EAN8_ADDENDA_REQUIRED: 
			EAN8AddendaRequired = (int)data; break;
		case DEC_EAN8_ADDENDA_SEPARATOR: 
			EAN8AddendaSeparator = (int)data; break;

//EAN-13
		case DEC_EAN13_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				EAN13Enabled = (int)data;
				returnCode = SD_Set(decoder, SD_PROP_UPC_ENABLED, (void *)UPC_ENABLE_MIX ); 
			}
			break;
		case DEC_EAN13_CHECK_DIGIT_TRANSMIT: 
			EAN13CheckDigitTransmit = (int)data; break;
		case DEC_EAN13_2CHAR_ADDENDA_ENABLED: 
			EAN132CharAddendaEnabled = (int)data;
			returnCode = SD_Set(decoder, SD_PROP_UPC_SUPPLEMENTALS, (void *)UPC_ADDENDA_MIX ); break;
		case DEC_EAN13_5CHAR_ADDENDA_ENABLED: 
			EAN135CharAddendaEnabled = (int)data;
			returnCode = SD_Set(decoder, SD_PROP_UPC_SUPPLEMENTALS, (void *)UPC_ADDENDA_MIX ); break;
		case DEC_EAN13_ADDENDA_REQUIRED: 
			EAN13AddendaRequired = (int)data; break;
		case DEC_EAN13_ADDENDA_SEPARATOR: 
			EAN13AddendaSeparator = (int)data; break;
		case DEC_EAN13_ISBN_ENABLED: 
			EAN13ISBNEnabled = (int)data; break;

//UPC-Misc
		case DEC_UPC_IMPROVE_BOUNDS: 
			returnCode = SD_Set(decoder, SD_PROP_UPC_IMPROVE_BOUNDS, data ); break;
		case DEC_UPC_SHORT_MARGIN: 
			returnCode = SD_Set(decoder, SD_PROP_UPC_SHORT_MARGIN, data ); break;

//Code 128
		case DEC_CODE128_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				Code128Enabled = (int)data;
				returnCode = SD_Set(decoder, SD_PROP_C128_ENABLED, (void *)1 ); 
			}
			break;
		case DEC_CODE128_MIN_LENGTH: 
			Code128Min = (int)data; break;
		case DEC_CODE128_MAX_LENGTH: 
			Code128Max = (int)data; break;

//GS1-128
		case DEC_GS1_128_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				GS1128Enabled = (int)data;
				returnCode = SD_Set(decoder, SD_PROP_C128_ENABLED, (void *)1 ); 
			}
			break;
		case DEC_GS1_128_MIN_LENGTH: 
			GS1128Min = (int)data; break;
		case DEC_GS1_128_MAX_LENGTH: 
			GS1128Max = (int)data; break;

//Code 128/GS1-128 Misc
		case DEC_C128_FNC_TRANSMIT: 
			Code128FncTransmit = (int)data; break;
		case DEC_C128_IMPROVE_BOUNDS: 
			returnCode = SD_Set(decoder, SD_PROP_C128_IMPROVE_BOUNDS, data ); break;
		case DEC_C128_PARTIAL: 
			returnCode = SD_Set(decoder, SD_PROP_C128_PARTIAL, data ); break;
		case DEC_C128_SHORT_MARGIN: 
			returnCode = SD_Set(decoder, SD_PROP_C128_SHORT_MARGIN, data ); break;
		case DEC_C128_SUPPRESS_CODABLOCK_CONFLICT: 
			returnCode = SD_Set(decoder, SD_PROP_C128_SUPPRESS_CB_CONFLICT, data ); break;

//Code 39
		case DEC_CODE39_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = SD_Set(decoder, SD_PROP_C39_ENABLED, data ); 
			}
			break;
		case DEC_CODE39_MIN_LENGTH: 
			Code39Min = (int)data; break;
		case DEC_CODE39_MAX_LENGTH: 
			Code39Max = (int)data; break;
		case DEC_CODE39_START_STOP_TRANSMIT: 
			Code39StartStopTransmit = (int)data; break;
		case DEC_CODE39_BASE32_ENABLED: 
			Code39Base32Enabled = (int)data; break;
		case DEC_CODE39_CHECK_DIGIT_MODE: 
			returnCode = SD_Set(decoder, SD_PROP_C39_CHECKSUM, data); break;
		case DEC_CODE39_FULL_ASCII_ENABLED: 
			returnCode = SD_Set(decoder, SD_PROP_C39_FULL_ASCII, data ); break;
		case DEC_CODE39_IMPROVE_BOUNDS: 
			returnCode = SD_Set(decoder, SD_PROP_C39_IMPROVE_BOUNDS, data ); break;
		case DEC_CODE39_PARTIAL: 
			returnCode = SD_Set(decoder, SD_PROP_C39_PARTIAL, data ); break;
		case DEC_CODE39_SHORT_MARGIN: 
			returnCode = SD_Set(decoder, SD_PROP_C39_SHORT_MARGIN, data ); break;
		case DEC_CODE39_SUPPRESS_CODABLOCK_CONFLICT: 
			returnCode = SD_Set(decoder, SD_PROP_C39_SUPPRESS_CB_CONFLICT, data ); break;

//Pharmacode
		case DEC_PHARMACODE_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = SD_Set(decoder, SD_PROP_PHARMA_ENABLED, data ); 
			}
			break;
		case DEC_PHARMACODE_MIN_LENGTH: 
			returnCode = SD_Set(decoder, SD_PROP_PHARMA_MIN_VALUE, data ); break;
		case DEC_PHARMACODE_MAX_LENGTH: 
			returnCode = SD_Set(decoder, SD_PROP_PHARMA_MAX_VALUE, data ); break;
		case DEC_PHARMACODE_MIN_BAR_COUNT: 
			returnCode = SD_Set(decoder, SD_PROP_PHARMA_MIN_BAR_COUNT, data ); break;
		case DEC_PHARMACODE_MAX_BAR_COUNT: 
			returnCode = SD_Set(decoder, SD_PROP_PHARMA_MAX_BAR_COUNT, data ); break;
		case DEC_PHARMACODE_COLOR_BARS: 
			returnCode = SD_Set(decoder, SD_PROP_PHARMA_COLOR_BARS, data ); break;
		case DEC_PHARMACODE_ORIENTATION: 
			returnCode = SD_Set(decoder, SD_PROP_PHARMA_ORIENTATION, data ); break;
		case DEC_PHARMACODE_REVERSE: 
			returnCode = SD_Set(decoder, SD_PROP_PHARMA_REVERSE, data ); break;

//TLC 39
		case DEC_TLC39_ENABLED: 
			TLC39CompositeEnabled = (int)data;
			returnCode = SD_Set(decoder, SD_PROP_CC_ENABLED, (void *)COMPOSITE_ENABLE_MIX ); break;

//Trioptic
		case DEC_TRIOPTIC_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = SD_Set(decoder, SD_PROP_TRIOPTIC_ENABLED, data ); 
			}
			break;
		case DEC_TRIOPTIC_SHORT_MARGIN: 
			returnCode = SD_Set(decoder, SD_PROP_TRIOPTIC_SHORT_MARGIN, data ); break;

//Interleaved 2 of 5
		case DEC_I25_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = SD_Set(decoder, SD_PROP_I25_ENABLED, data ); 
			}
			break;
		case DEC_I25_MIN_LENGTH:
			if( (int)data <= 4 ) {
				lengths |= 0x2;
			}
			if( (int)data <= 2 ) {
				lengths |= 0x1;
			}
			
			SD_Set( decoder, SD_PROP_I25_LENGTHS, (void *)lengths );
			Interleaved25Min = (int)data; break;
		case DEC_I25_MAX_LENGTH: 
			Interleaved25Max = (int)data; break;
		case DEC_I25_CHECK_DIGIT_MODE: 
			Interleaved25CheckDigitMode = (int)data;
			returnCode = SD_Set(decoder, SD_PROP_I25_CHECKSUM, (void *)(Interleaved25CheckDigitMode&0x3) ); break;
		case DEC_I25_IMPROVE_BOUNDS: 
			returnCode = SD_Set(decoder, SD_PROP_I25_IMPROVE_BOUNDS, data ); break;
		case DEC_I25_PARTIAL: 
			returnCode = SD_Set(decoder, SD_PROP_I25_PARTIAL, data ); break;
		case DEC_I25_SHORT_MARGIN: 
			returnCode = SD_Set(decoder, SD_PROP_I25_SHORT_MARGIN, data ); break;

//Standard 2 of 5
		case DEC_S25_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = SD_Set(decoder, SD_PROP_S25_3SS_ENABLED, data ); 
			}
			break;
		case DEC_S25_MIN_LENGTH:
			if( (int)data <= 2 ) {
				lengths |= 0x2;
			}
			if( (int)data <= 1 ) {
				lengths |= 0x1;
			}
			
			SD_Set( decoder, SD_PROP_S25_3SS_LENGTHS, (void *)lengths );
			Standard250Min = (int)data; break;
		case DEC_S25_MAX_LENGTH: 
			Standard250Max = (int)data; break;
		case DEC_S25_IMPROVE_BOUNDS: 
			returnCode = SD_Set(decoder, SD_PROP_S25_3SS_IMPROVE_BOUNDS, data ); break;

//IATA 2 of 5
		case DEC_IATA25_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = SD_Set(decoder, SD_PROP_S25_2SS_ENABLED, data ); 
			}
			break;
		case DEC_IATA25_MIN_LENGTH:
			if( (int)data <= 2 ) {
				lengths |= 0x2;
			}
			if( (int)data <= 1 ) {
				lengths |= 0x1;
			}
			
			SD_Set( decoder, SD_PROP_S25_2SS_LENGTHS, (void *)lengths );
			Standard251Min = (int)data; break;
		case DEC_IATA25_MAX_LENGTH: 
			Standard251Max = (int)data; break;
		case DEC_IATA25_IMPROVE_BOUNDS: 
			returnCode = SD_Set(decoder, SD_PROP_S25_2SS_IMPROVE_BOUNDS, data ); break;

//Matrix 2 of 5
		case DEC_M25_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = SD_Set(decoder, SD_PROP_M25_ENABLED, data ); 
			}
			break;
		case DEC_M25_MIN_LENGTH: 
			if( (int)data <= 2 ) {
				lengths |= 0x2;
			}
			if( (int)data <= 1 ) {
				lengths |= 0x1;
			}
			
			SD_Set( decoder, SD_PROP_M25_LENGTHS, (void *)lengths );
			Matrix25Min = (int)data; break;
		case DEC_M25_MAX_LENGTH: 
			Matrix25Max = (int)data; break;
		case DEC_M25_CHECK_DIGIT_MODE: 
			returnCode = SD_Set(decoder, SD_PROP_M25_CHECKSUM, data ); break;
		case DEC_M25_IMPROVE_BOUNDS: 
			returnCode = SD_Set(decoder, SD_PROP_M25_IMPROVE_BOUNDS, data ); break;

//NEC 2 of 5
		case DEC_NEC25_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = SD_Set(decoder, SD_PROP_NEC25_ENABLED, data ); 
			}
			break;
		case DEC_NEC25_MIN_LENGTH: 
			if( (int)data <= 2 ) {
				lengths |= 0x2;
			}
			if( (int)data <= 1 ) {
				lengths |= 0x1;
			}
			
			SD_Set( decoder, SD_PROP_NEC25_LENGTHS, (void *)lengths );
			NEC25Min = (int)data; break;
		case DEC_NEC25_MAX_LENGTH: 
			NEC25Max = (int)data; break;
		case DEC_NEC25_CHECK_DIGIT_MODE: 
			returnCode = SD_Set(decoder, SD_PROP_NEC25_CHECKSUM, data ); break;
		case DEC_NEC25_IMPROVE_BOUNDS: 
			returnCode = SD_Set(decoder, SD_PROP_NEC25_IMPROVE_BOUNDS, data ); break;

//Code 93
		case DEC_CODE93_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = SD_Set(decoder, SD_PROP_C93_ENABLED, data ); 
			}
			break;
		case DEC_CODE93_MIN_LENGTH: 
			Code93Min = (int)data; break;
		case DEC_CODE93_MAX_LENGTH: 
			Code93Max = (int)data; break;
		case DEC_CODE93_IMPROVE_BOUNDS: 
			returnCode = SD_Set(decoder, SD_PROP_C93_IMPROVE_BOUNDS, data ); break;
		case DEC_CODE93_SHORT_MARGIN: 
			returnCode = SD_Set(decoder, SD_PROP_C93_SHORT_MARGIN, data ); break;

//Code 11
		case DEC_CODE11_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = SD_Set(decoder, SD_PROP_C11_ENABLED, data ); 
			}
			break;
		case DEC_CODE11_MIN_LENGTH: 
			Code11Min = (int)data; break;
		case DEC_CODE11_MAX_LENGTH: 
			Code11Max = (int)data; break;
		case DEC_CODE11_CHECK_DIGIT_MODE: 
			returnCode = SD_Set(decoder, SD_PROP_C11_CHECKSUM, data ); break;
		case DEC_CODE11_IMPROVE_BOUNDS: 
			returnCode = SD_Set(decoder, SD_PROP_C11_IMPROVE_BOUNDS, data ); break;

//Codabar
		case DEC_CODABAR_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = SD_Set(decoder, SD_PROP_CB_ENABLED, data ); 
			}
			break;
		case DEC_CODABAR_MIN_LENGTH: 
			CodabarMin = (int)data; break;
		case DEC_CODABAR_MAX_LENGTH: 
			CodabarMax = (int)data; break;
		case DEC_CODABAR_CHECK_DIGIT_MODE: 
			returnCode = SD_Set(decoder, SD_PROP_CB_CHECKSUM, data ); break;
		case DEC_CODABAR_IMPROVE_BOUNDS: 
			returnCode = SD_Set(decoder, SD_PROP_CB_IMPROVE_BOUNDS, data ); break;
		case DEC_CODABAR_SHORT_MARGIN: 
			returnCode = SD_Set(decoder, SD_PROP_CB_SHORT_MARGIN, data ); break;

//Telepen
		case DEC_TELEPEN_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = SD_Set(decoder, SD_PROP_TP_ENABLED, data ); 
			}
			break;
		case DEC_TELEPEN_MIN_LENGTH: 
			TelepenMin = (int)data; break;
		case DEC_TELEPEN_MAX_LENGTH: 
			TelepenMax = (int)data; break;
		case DEC_TELEPEN_OLD_STYLE: 
			returnCode = SD_Set(decoder, SD_PROP_TP_NUM_START, data ); break;			
		case DEC_TELEPEN_IMPROVE_BOUNDS: 
			returnCode = SD_Set(decoder, SD_PROP_TP_IMPROVE_BOUNDS, data ); break;

//MSI
		case DEC_MSI_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = SD_Set(decoder, SD_PROP_MSIP_ENABLED, data ); 
			}
			break;
		case DEC_MSI_MIN_LENGTH: 
			MSIMin = (int)data; break;
		case DEC_MSI_MAX_LENGTH: 
			MSIMax = (int)data; break;
		case DEC_MSI_CHECK_DIGIT_MODE: 
			returnCode = SD_Set(decoder, SD_PROP_MSIP_CHECKSUM, data); break;
		case DEC_MSI_IMPROVE_BOUNDS: 
			returnCode = SD_Set(decoder, SD_PROP_MSIP_IMPROVE_BOUNDS, data ); break;

//RSS (GS1)
		case DEC_RSS_14_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				RSS14Enabled = (int)data;
				returnCode = SD_Set(decoder, SD_PROP_RSS_ENABLED, (void *)RSS_ENABLE_MIX ); 
			}
			break;
		case DEC_RSS_LIMITED_ENABLED: 
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				RSSLimitedEnabled = (int)data;
				returnCode = SD_Set(decoder, SD_PROP_RSS_ENABLED, (void *)RSS_ENABLE_MIX ); 
			}
			break;
		case DEC_RSS_EXPANDED_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				RSSExpandedEnabled = (int)data;
				returnCode = SD_Set(decoder, SD_PROP_RSS_ENABLED, (void *)RSS_ENABLE_MIX ); 
			}
			break;
		case DEC_RSS_EXPANDED_MIN_LENGTH: 
			RSSExpandedMin = (int)data; break;
		case DEC_RSS_EXPANDED_MAX_LENGTH: 
			RSSExpandedMax = (int)data; break;

//Korea Post
		case DEC_KOREA_POST_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = SD_Set(decoder, SD_PROP_KP_ENABLED, data ); 
			}
			break;
		case DEC_KOREA_POST_MIN_LENGTH: 
			KoreaPostMin = (int)data; break;
		case DEC_KOREA_POST_MAX_LENGTH: 
			KoreaPostMax = (int)data; break;
		case DEC_KOREA_POST_CHECK_DIGIT_TRANSMIT:
			KoreaPostCheckDigitTransmit = (int)data;
			if( (int)data ) {
				returnCode = SD_Set(decoder, SD_PROP_KP_CHECKSUM, (void *)0 );
			}
			else {
				returnCode = SD_Set(decoder, SD_PROP_KP_CHECKSUM, (void *)1 );
			}
			break;
		case DEC_KOREA_POST_REVERSE: 
			returnCode = SD_Set(decoder, SD_PROP_KP_REVERSE, data ); break;

/* Stacked Linear Symbologies */

//Codablock A
		case DEC_CODABLOCK_A_ENABLED: 
			if( DisableSymbologyGroup & STACKED_LINEAR_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else { 
				returnCode = SD_Set(decoder, SD_PROP_CODABLOCK_A_ENABLED, data ); 
			}
			break;
		case DEC_CODABLOCK_A_MIN_LENGTH: 
			CodablockAMin = (int)data; break;
		case DEC_CODABLOCK_A_MAX_LENGTH: 
			CodablockAMax = (int)data; break;

//Codablock F
		case DEC_CODABLOCK_F_ENABLED:
			if( DisableSymbologyGroup & STACKED_LINEAR_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {  
				returnCode = SD_Set(decoder, SD_PROP_CODABLOCK_F_ENABLED, data ); 
			}
			break;
		case DEC_CODABLOCK_F_MIN_LENGTH: 
			CodablockFMin = (int)data; break;
		case DEC_CODABLOCK_F_MAX_LENGTH: 
			CodablockFMax = (int)data; break;

//PDF417
		case DEC_PDF417_ENABLED:
			if( DisableSymbologyGroup & STACKED_LINEAR_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else { 
				returnCode = SD_Set(decoder, SD_PROP_PDF_ENABLED, data ); 
			}
			break;
		case DEC_PDF417_MIN_LENGTH: 
			PDF417Min = (int)data; break;
		case DEC_PDF417_MAX_LENGTH: 
			PDF417Max = (int)data; break;
		//case DEC_PDF417_SHOW_GLI: 

//Micro PDF417
		case DEC_MICROPDF_ENABLED: 
			if( DisableSymbologyGroup & STACKED_LINEAR_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = SD_Set(decoder, SD_PROP_MICROPDF_ENABLED, data ); 
			}
			break;
		case DEC_MICROPDF_MIN_LENGTH: 
			MicroPDFMin = (int)data; break;
		case DEC_MICROPDF_MAX_LENGTH: 
			MicroPDFMax = (int)data; break;
		case DEC_MICROPDF_IMPROVE_BOUNDS: 
			returnCode = SD_Set(decoder, SD_PROP_MICROPDF_IMPROVE_BOUNDS, data ); break;

//Composite
		case DEC_COMPOSITE_ENABLED:
			CompositeEnabled = (int)data;
			returnCode = SD_Set(decoder, SD_PROP_CC_ENABLED, (void *)COMPOSITE_ENABLE_MIX ); break;


/* Matrix 2D Symbologies */

//Aztec Code
		case DEC_AZTEC_ENABLED: 
			AztecEnabled = (int)data;
			if( AztecEnabled == 0 ) { //if aztec is turned off, only allow normal video to read menu codes.
				returnCode = SD_Set(decoder, SD_PROP_AZ_ENABLED, (void *)1 );
			}
			else {
				returnCode = SD_Set(decoder, SD_PROP_AZ_ENABLED, (void *)data ); 
			}
			break;
		case DEC_AZTEC_MIN_LENGTH: 
			AztecMin = (int)data; break;
		case DEC_AZTEC_MAX_LENGTH: 
			AztecMax = (int)data; break;
		case DEC_AZTEC_SYMBOL_SIZE:
			returnCode = SD_Set(decoder, SD_PROP_AZ_SYMBOL_SIZE, data ); break;

//Maxicode
		case DEC_MAXICODE_ENABLED: 
			returnCode = SD_Set(decoder, SD_PROP_MC_ENABLED, data ); break;
		case DEC_MAXICODE_SYMBOL_SIZE: 
			returnCode = SD_Set(decoder, SD_PROP_MC_SYMBOL_SIZE, data ); break;
		case DEC_MAXICODE_MIN_LENGTH: 
			MaxicodeMin = (int)data; break;
		case DEC_MAXICODE_MAX_LENGTH: 
			MaxicodeMax = (int)data; break;

//DataMatrix
		case DEC_DATAMATRIX_ENABLED: 
			returnCode = SD_Set(decoder, SD_PROP_DM_ENABLED, data ); break;
		case DEC_DATAMATRIX_MIN_LENGTH: 
			DataMatrixMin = (int)data; break;
		case DEC_DATAMATRIX_MAX_LENGTH: 
			DataMatrixMax = (int)data; break;
		case DEC_DATAMATRIX_MIN_MODULE_COUNT: 
			returnCode = SD_Set(decoder, SD_PROP_DM_MIN_MODULE_COUNT, data ); break;
		case DEC_DATAMATRIX_MAX_MODULE_COUNT: 
			returnCode = SD_Set(decoder, SD_PROP_DM_MAX_MODULE_COUNT, data ); break;
		case DEC_DATAMATRIX_MIN_MODULE_SIZE: 
			returnCode = SD_Set(decoder, SD_PROP_DM_MIN_MODULE_SIZE, data ); break;
		case DEC_DATAMATRIX_MAX_MODULE_SIZE: 
			returnCode = SD_Set(decoder, SD_PROP_DM_MAX_MODULE_SIZE, data ); break;
		case DEC_DATAMATRIX_ORIENTATIONS: 
			returnCode = SD_Set(decoder, SD_PROP_DM_ORIENTATIONS, data ); break;
		case DEC_DATAMATRIX_BINARY_IMPROVEMENTS: 
			returnCode = SD_Set(decoder, SD_PROP_DM_BINARY_IMPROVEMENTS, data ); break;
		case DEC_DATAMATRIX_NON_SQUARE_MODULES: 
			returnCode = SD_Set(decoder, SD_PROP_DM_NON_SQUARE_MODULES, data ); break;
		case DEC_DATAMATRIX_SHIFTED_TILES: 
			returnCode = SD_Set(decoder, SD_PROP_DM_SHIFTED_TILES, data ); break;
		case DEC_DATAMATRIX_LOW_CONTRAST: 
			returnCode = SD_Set(decoder, SD_PROP_DM_LOW_CONTRAST, data ); break;
		case DEC_DATAMATRIX_SYMBOL_SIZE: 
			returnCode = SD_Set(decoder, SD_PROP_DM_SYMBOL_SIZE, data ); break;
		case DEC_DATAMATRIX_RECTANGLE: 
			returnCode = SD_Set(decoder, SD_PROP_DM_RECT, data ); break;
		case DEC_DATAMATRIX_SUBREGION_HEIGHT: 
			returnCode = SD_Set(decoder, SD_PROP_DM_SUBREGION_HEIGHT, data ); break;
		case DEC_DATAMATRIX_SUBREGION_LEFT: 
			returnCode = SD_Set(decoder, SD_PROP_DM_SUBREGION_LEFT, data ); break;
		case DEC_DATAMATRIX_SUBREGION_TOP: 
			returnCode = SD_Set(decoder, SD_PROP_DM_SUBREGION_TOP, data ); break;
		case DEC_DATAMATRIX_SUBREGION_WIDTH: 
			returnCode = SD_Set(decoder, SD_PROP_DM_SUBREGION_WIDTH, data ); break;

//QR Code
		case DEC_QR_ENABLED: 
			returnCode = SD_Set(decoder, SD_PROP_QR_ENABLED, data ); break;
		case DEC_QR_MIN_LENGTH: 
			QRMin = (int)data; break;
		case DEC_QR_MAX_LENGTH: 
			QRMax = (int)data; break;
		case DEC_QR_NON_SQUARE_MODULES: 
			returnCode = SD_Set(decoder, SD_PROP_QR_NON_SQUARE_MODULES, data ); break;
		case DEC_QR_SYMBOL_SIZE: 
			returnCode = SD_Set(decoder, SD_PROP_QR_SYMBOL_SIZE, data ); break;

//HanXin
		case DEC_HANXIN_ENABLED: 
			returnCode = SD_Set(decoder, SD_PROP_HX_ENABLED, data ); break;
		case DEC_HANXIN_MIN_LENGTH: 
			HanXinMin = (int)data; break;
		case DEC_HANXIN_MAX_LENGTH: 
			HanXinMax = (int)data; break;

//Go Code
		case DEC_GOCODE_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = SD_Set(decoder, SD_PROP_GC_ENABLED, data ); 
			}
			break;

//Hong Kong 2 of 5
		case DEC_HK25_ENABLED:
			if( DisableSymbologyGroup & ONE_DIMENSIONAL_CODES ) {
				for( symb = 0; symb<MAX_DISABLED; symb++ ) {
					if( DisabledGroupSettings[symb][0] == tag ) {
						DisabledGroupSettings[symb][1] = (int)data;
						break;
					}
				}
			}
			else {
				returnCode = SD_Set(decoder, SD_PROP_HK25_ENABLED, data ); 
			}
			break;
		case DEC_HK25_MIN_LENGTH:
			if( (int)data <= 2 ) {
				lengths |= 0x2;
			}
			if( (int)data <= 1 ) {
				lengths |= 0x1;
			}
			
			SD_Set( decoder, SD_PROP_HK25_LENGTHS, (void *)lengths );
			HongKong25Min = (int)data; break;
		case DEC_HK25_MAX_LENGTH: 
			HongKong25Max = (int)data; break;
		case DEC_HK25_IMPROVE_BOUNDS: 
			returnCode = SD_Set(decoder, SD_PROP_HK25_IMPROVE_BOUNDS, data ); break;


/* OCR */
		case DEC_OCR_MODE:
			returnCode = SD_Set(decoder, SD_PROP_OCR_ENABLED, data ); break;
		case DEC_OCR_TEMPLATE:
			returnCode = SD_Set(decoder, SD_PROP_OCR_USER_TEMPLATES, data ); break;
		case DEC_OCR_ACTIVE_TEMPLATES:
			returnCode = SD_Set(decoder, SD_PROP_OCR_ACTIVE_TEMPLATES, data ); break;
		case DEC_OCR_BUSY_BACKGROUND:
			returnCode = SD_Set(decoder, SD_PROP_OCR_BUSY_BACKGROUND, data ); break;
		case DEC_IMAGE_ROTATION:
			switch((int)data) { //each reflects a "left to right" reading depending on the "rotation" of the image.
				case 0: returnCode = SD_Set(decoder, SD_PROP_OCR_PREFERRED_ORIENTATION, (void *)3 ); break;
				case 1: returnCode = SD_Set(decoder, SD_PROP_OCR_PREFERRED_ORIENTATION, (void *)0 ); break;
				case 2: returnCode = SD_Set(decoder, SD_PROP_OCR_PREFERRED_ORIENTATION, (void *)2 ); break;
				case 3: returnCode = SD_Set(decoder, SD_PROP_OCR_PREFERRED_ORIENTATION, (void *)1 ); break;
				default: returnCode = SD_Set(decoder, SD_PROP_OCR_PREFERRED_ORIENTATION, (void *)4 ); break;
			}
			break;
/* Postal Symbologies */

		case DEC_POSTAL_ENABLED:
			PostalEnabled = (int)data;
			returnCode = SD_Set(decoder, SD_PROP_POSTAL_ENABLED, (void *)PostalOptions[(int)data]); break;
		case DEC_POSTAL_ENABLED_DIRECT: 
			if( (returnCode = FindPostalMatch( (unsigned int)data )) > 0 ) {
				SD_Set(decoder, SD_PROP_POSTAL_ENABLED, data);
			}
			break;

//PostNet
		case DEC_POSTNET_CHECK_DIGIT_TRANSMIT: 
			PostnetCheckDigitTransmit = (int)data; break;

//Planet Code
		case DEC_PLANETCODE_CHECK_DIGIT_TRANSMIT: 
			PlanetCodeCheckDigitTransmit = (int)data; break;
//Royal Mail 
		case DEC_ROYAL_MAIL_FORMAT_CHECK_MIN_LENGTH:
			returnCode = SD_Set(decoder, SD_PROP_POSTAL_RM_MISC, data); break;                                   
//Australian Post
		case DEC_AUS_POST_BAR_OUTPUT_ENABLED: 
			returnCode = SD_Set(decoder, SD_PROP_POSTAL_AP_BAR_OUTPUT, data ); break;
		case DEC_AUS_POST_INTERPRET_MODE: 
			returnCode = SD_Set(decoder, SD_PROP_POSTAL_AP_CUST_INTERPRET, data ); break;
		case DEC_AUS_POST_ZERO_FCC: 
			returnCode = SD_Set(decoder, SD_PROP_POSTAL_AP_ZERO_FCC, data ); break;

// Canadian Post output of bar information instead of data
		case DEC_CAN_POST_BAR_OUTPUT:
			returnCode = SD_Set(decoder, SD_PROP_POSTAL_CP_BAR_OUTPUT, data ); break;

			
//Misc Postal - applies to all postal symbologies
		case DEC_POSTAL_MIN_BAR_COUNT: 
			returnCode = SD_Set(decoder, SD_PROP_POSTAL_MIN_BAR_COUNT, data ); break;
		case DEC_POSTAL_MAX_BAR_COUNT: 
			returnCode = SD_Set(decoder, SD_PROP_POSTAL_MAX_BAR_COUNT, data ); break;
		case DEC_POSTAL_ORIENTATIONS: 
			returnCode = SD_Set(decoder, SD_PROP_POSTAL_ORIENTATIONS, data ); break;
		case DEC_POSTAL_SUBREGION_HEIGHT: 
			returnCode = SD_Set(decoder, SD_PROP_POSTAL_SUBREGION_HEIGHT, data ); break;
		case DEC_POSTAL_SUBREGION_LEFT: 
			returnCode = SD_Set(decoder, SD_PROP_POSTAL_SUBREGION_LEFT, data ); break;
		case DEC_POSTAL_SUBREGION_TOP: 
			returnCode = SD_Set(decoder, SD_PROP_POSTAL_SUBREGION_TOP, data ); break;
		case DEC_POSTAL_SUBREGION_WIDTH: 
			returnCode = SD_Set(decoder, SD_PROP_POSTAL_SUBREGION_WIDTH, data ); break;
		case DEC_POSTAL_UNDECODABLE_HEIGHT: 
			returnCode = SD_Set(decoder, SD_PROP_POSTAL_UNDECODABLE_HEIGHT, data ); break;
		case DEC_POSTAL_UNDECODABLE_LEFT: 
			returnCode = SD_Set(decoder, SD_PROP_POSTAL_UNDECODABLE_LEFT, data ); break;
		case DEC_POSTAL_UNDECODABLE_TOP: 
			returnCode = SD_Set(decoder, SD_PROP_POSTAL_UNDECODABLE_TOP, data ); break;
		case DEC_POSTAL_UNDECODABLE_WIDTH: 
			returnCode = SD_Set(decoder, SD_PROP_POSTAL_UNDECODABLE_WIDTH, data ); break;

		default:  returnCode = SD_Set(decoder, tag, data); break; //default, just pass through
	}
	return returnCode;
}

void SI_DecodeReset( int /*decoder*/ ) {
	int jk;

	DisableSymbologyGroup = 0;
	for( jk=0; jk<MAX_DISABLED; jk++ ) {
		DisabledGroupSettings[jk][0] = DisabledGroupSettings[jk][1] = 0;
	}

	//default values for the decoder.
	AztecEnabled	=3;
	UPCAEnabled		=0;
	UPCE0Enabled	=0;
	UPCE1Enabled	=0;
	EAN8Enabled		=0;
	EAN13Enabled	=0;
	Code128Enabled	=0;
	GS1128Enabled	=0;
	Code128FncTransmit = 0;
	PostalEnabled	=0;
	KoreaPostCheckDigitTransmit = 0;
	
	UPCA2CharAddendaEnabled		=0;
	UPCA5CharAddendaEnabled		=0;
	UPCAAddendaRequired			=0;
	UPCE2CharAddendaEnabled		=0;
	UPCE5CharAddendaEnabled		=0;
	UPCEAddendaRequired			=0;
	EAN82CharAddendaEnabled		=0;
	EAN85CharAddendaEnabled		=0;
	EAN8AddendaRequired			=0;
	EAN132CharAddendaEnabled	=0;
	EAN135CharAddendaEnabled	=0;
	EAN13AddendaRequired		=0;

	AddendaTimeStamp	=0;
	AddendaSearchTime	=0;

/* Min/max */
	CodabarMin			=4;
	CodabarMax			=60;
	Code39Min			=1;
	Code39Max			=48;
	Interleaved25Min	=4;
	Interleaved25Max	=80;
	Standard250Min		=4;
	Standard250Max		=48;
	Standard251Min		=4;
	Standard251Max		=48;
	Matrix25Min			=4;
	Matrix25Max			=80;
	Code11Min			=4;
	Code11Max			=80;
	Code128Min			=1;
	Code128Max			=80;
	GS1128Min			=1;
	GS1128Max			=80;
	Code93Min			=0;
	Code93Max			=80;
	MSIMin				=4;
	MSIMax				=48;
	TelepenMin			=1;
	TelepenMax			=60;
	HongKong25Min		=4;
	HongKong25Max		=250;
	RSSExpandedMin		=4;
	RSSExpandedMax		=74;
	PDF417Min			=1;
	PDF417Max			=2028;
	MicroPDFMin			=1;
	MicroPDFMax			=250;
	CodablockFMin		=1;
	CodablockFMax		=2048;
	AztecMin			=1;
	AztecMax			=3832;
	MaxicodeMin			=1;
	MaxicodeMax			=150;
	DataMatrixMin		=1;
	DataMatrixMax		=3116;
	QRMin				=1;
	QRMax				=7089;
	HanXinMin			=1;
	HanXinMax			=6000;
	KoreaPostMin		=4;
	KoreaPostMax		=48;

}

int SI_GetLastDecoderError( void ) {
	return SD_GetLastError();
}

/* Postamble */
#ifdef __cplusplus
}
#endif  /* __cplusplus */
