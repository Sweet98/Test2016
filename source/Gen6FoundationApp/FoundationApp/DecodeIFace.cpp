
/*----------------------------------------------------------------------*
 *	       DecodeIface.cpp is the interface between the decoder and		*
 *	       the function that calls the decoder.									*
 *----------------------------------------------------------------------*/

//=================================================================================
//   $RCSfile: Gen6FoundationApp/FoundationApp/DecodeIFace.cpp $
//   $Date: 2011/11/22 17:42:04EST $
//   $Revision: 1.139 $
//
//=================================================================================*

//=================================================================================

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "product.h"
#include "OsTimer.h"
#include "ioman.h"
#include "beep.h"
#include "menu.h"
#include "DecodeIFace.h"
#include "DecodeIFacePrivate.h"
#include "BarcodeDataMan.h"
#include "thread.h"
//#include "ScanDriverInterface.h"		//changed to "scandrivermatrixinterface.h"
#include "scandrivermatrixinterface.h"
#include "barcodefifo.h"
#include "db_chrio.h"
#include "db_time.h"
#include "mainhhplproc.h"
#include "productconfiguration.h"
#include "readerconfiguration.h"
#include "imaging.h"
#include "HTag.h"			// HTAG interface definition
#include "MatrixSynFECommandHandler.h"			// Interface definition
#include "ScanDriverImagingInterface.h"		//set lights
#include "EditScript.h"
#include "usage_statistics.h"

#include "ScanEnginePSOC.h"
#include "HardwareToken.h"

#include "productconfiguration.h"

#ifdef PLUGIN_DEV
#include "HONPluginConfiguration.h"
#include "PluginMenuProcess.h"
#include "PluginProcessDecoding.h"
#endif

extern unsigned char parsedOCRTemplate[SIZE_OF_OCR_TEMPLATE + 1];

int Decoder_Handle=0;
#include "DecoderInterface.h"
#define Pt theCurrentInterface.printf

void SetImageParameters(int hgt, int wid, int wrap, int cenX, int cenY);
void StoreLastDecodeInfo (DecoderResult *msg);

bool ChangeDecoderSetting( void ) {
	if( ReaderConfiguration->ExtraneousTextAfterCommand->Size == 0 ) {
		return false;
	}

	int numUsed = 0;
	int posOfSemi = 0;
	ReaderConfiguration->ExtraneousTextAfterCommand->FindCharacterForward( ':',0,&posOfSemi );
	int start = 0;

	if( posOfSemi != ReaderConfiguration->ExtraneousTextAfterCommand->Size ) {
		int tag = ReaderConfiguration->ExtraneousTextAfterCommand->ConvertHexToNumber(start,posOfSemi,&numUsed);
		if( numUsed < 8 ) {
			tag += MENU_TYPE_INT;
		}
		int data = ReaderConfiguration->ExtraneousTextAfterCommand->ConvertToNumber(posOfSemi+1,&numUsed);
		if( numUsed ) {
			DecodeSet(tag,(void *)data);
		}
		else {
			ReaderConfiguration->MenuCommandResponse->Append("\r\nno data\r\n");
			return false;
		}
	}
	else {
		int tag = ReaderConfiguration->ExtraneousTextAfterCommand->ConvertHexToNumber(start,&numUsed);
		if( numUsed < 8 ) {
			tag += MENU_TYPE_INT;
		}
		int data = -9999;
		DecodeGet(tag,(void *)&data);

		if( data == -9999 ) {
			char dbg[80];
			sprintf(dbg,"\r\nError retrieving value of tag: %08X \r\n", tag );
			ReaderConfiguration->MenuCommandResponse->Append( dbg );
		}
		else {
			char dbg[80];
			sprintf(dbg,"\r\nValue:  %d\r\n", data);
			ReaderConfiguration->MenuCommandResponse->Append( dbg );
		}
	}
	return true;
}


/************************************************************************/
//Global variables
/************************************************************************/

CBarcodeFifo *DecodeResults = 0;
volatile int  ProcessedResult = 0; // process result was called we either have a message if
// mix >0 or we had a menu and need to wait for anotheer
// trigger mix =0 mix is length of the message
extern BOOL	bKcodeMode;
bool AffectedDefalt=false;
int NumberOfDecodes = 0;
volatile int NumberOfDecodeAttempts = 0;


//ImgBox
LastDecodeInfo_t LastDecodeInfo;
LastDecodeInfo_t *pgLastDecodeInfo = NULL; // = &LastDecodeInfo;

int ComputeImageScore(unsigned char *img, int wid, int hgt, int wrap, int skip);

/************************************************************************/
//File Scope variables
/************************************************************************/

// Some local scope global "First Time" settings!
static int AverageScore = -1;
static int RepCnt = 0;
static int PresModeDecoderTriggered = 0;
static int TriggerDecoderThreshold = 0;
static int UntriggerDecoderThreshold = 0;

static int DecoderWidth = 0;
static int DecoderHeight = 0;
static void* Decoder = 0;
static bool bWasScanStnd = false;

static int IgnoreTrigger = FALSE;
static int ReDecoding = FALSE;
static int OnlyAllowMenuDecodes = FALSE;

UINT DecodingEndTime = 0;
static UINT DecodingStartTime = 0;
static int DecTmn = 0;
static int DecTmx = 0;


//	In order to be able to post process an image, such as processing and IMGBOX command, we need to save the image parameters
//	from the last decode.
static void *pImg = 0;
static DWORD nEngineType = 0;
static DWORD nSize = 0;
static unsigned int nHeight = 0;
static unsigned int nWidth = 0;


//Variables for IsNewMessage( void )
static MessageProfile_t Reads[PREVIOUS_READS_TO_REMEMBER] = {{-1}};
static int LastMessageIndex = -1;


//Variables for Shotgun()
static unsigned char ShotgunCodeID[ MAX_SHOTGUN_HISTORY ] = { DEFAULT_CODEID };
static int ShotgunLength[ MAX_SHOTGUN_HISTORY ];
static int ShotgunSum[ MAX_SHOTGUN_HISTORY ];
static int ShotgunSSum[ MAX_SHOTGUN_HISTORY ];
static int NumberOfShotgunMessages;


//Variables for DecodeIFaceOnChangedConfiguration()
static int		mAuxiliaryDecoder = 1;
static int		mPresentationAuxiliaryDecoder = 1;
static int		mTriggerMode = 0;
static int		mImageQualityMode = 1;
static int		mPresentationImageQualityMode = 1;
static int		selectedImageQualityMode = 1;
static int		imageExposureDecoding = 0;

static int RecentScore = 0;
static int DecodingImageScore = 0;

GrayscaleImage2 StoredImageDescriptor;
unsigned char *StoredImageArray = 0;

static UINT MsTimer (void)
{
	return (BaselineClock);
}


// remove 3800 menu preamble before we send to menu
BOOL IsST3800MenuSymbol( unsigned int *puiLength, unsigned char *pucSymbol )
{
	/* The menu symbol preamble may be 3 OR 4 characters depending on whether
		or not the decoder has suppressed the leading Code 128 Function 3 character
		(depends of the setting of the 'Supress Code 128 Function Characters'
		option) Therefore, we must look for the F3 character and if found,
		adjust our pointers and count so it is removed */

	unsigned int uiIndex = 0 ;
	unsigned int ui ;
	/* If the first character is not equal to '~' or the first two are not equal to Function 3 and '~', then
	 * this can't be a Welch Allyn ST3800 menu symbol - fail */
	if( (pucSymbol[uiIndex] != '~') )
	{
		if( ((pucSymbol[uiIndex++] != 0x83) || (pucSymbol[uiIndex] != '~')))
		{
			return FAIL ;
		}
	}

	uiIndex++;

	/* If the the ID matches either this product or the generic menu ID, continue */
	if( ( pucSymbol[uiIndex] == GENERIC_MENU_ID_1 && pucSymbol[uiIndex+1] == GENERIC_MENU_ID_2 ) ||
	        ( pucSymbol[uiIndex] == PRODUCT_MENU_ID_1 && pucSymbol[uiIndex+1] == PRODUCT_MENU_ID_2 ) )
	{
		uiIndex +=2 ;
	}

	if( *puiLength <= uiIndex )
	{
		return FAIL ;
	}

	ui = 0 ;

	while ( uiIndex < *puiLength )
	{
		if ( pucSymbol[uiIndex] != 0x83 )
		{
			pucSymbol[ui++] = pucSymbol[uiIndex++] ;
		}
		else
		{
			uiIndex++ ;
		}
	}

	pucSymbol[ui] = 0x00;
	*puiLength = (ui) ;

	/* And return successfully */
	return PASS ;
}


/************************************************************************/
//IsNewMessage() and ReRead Delay Support functions
/************************************************************************/


static unsigned int FindReReadDelay(char CodeID)
{
	unsigned int reread = 0;

	switch (CodeID)
	{
	case  WA_CODELETTER_AZTEC:
	case  WA_CODELETTER_DATAMATRIX:
	case  WA_CODELETTER_MAXICODE:
	case  WA_CODELETTER_QRCODE:
	case  WA_CODELETTER_PDF417:
	case  WA_CODELETTER_MICROPDF:
		// Reset the 2D Reread delay for all 2D symbologies - individual delays can be created in the future
		reread = ((GET_SETTING(ReReadDly2D)>0)?(GET_SETTING(ReReadDly2D)):(GET_SETTING(ReReadDly)));
		break;
	default:
		// Reset the original (1D) Reread Delay for all other symbologies
		reread = GET_SETTING(ReReadDly);
		break;
	}

	return reread;
}


void RefreshTimeStamp(char CodeID)
{
	int i,ShortestIndex = -1;
	unsigned int CurrentTime,TimeElapsed,ShortestElapsed = 0xffffffff;
	CurrentTime = read_ms_clock();

	for (i=0;i<PREVIOUS_READS_TO_REMEMBER;i++)
	{
		// End of the list
		if (Reads[i].Length == -1)
			break;

		// We check all of the entries in the list.  If we have multiple matches for the code ID we take the most recent read.
		if (Reads[i].HHPCodeID == CodeID)
		{
			TimeElapsed = (CurrentTime >= Reads[i].TimeStamp)?(CurrentTime - Reads[i].TimeStamp):((0xffffffff - Reads[i].TimeStamp) + CurrentTime);

			if (TimeElapsed < ShortestElapsed)
			{
				ShortestElapsed = TimeElapsed;
				ShortestIndex = i;
			}
		}
	}
	// Time Stamp the most recent read if we're still within the reread delay
	if ((ShortestIndex != -1)&&(ShortestElapsed < FindReReadDelay(CodeID)))
	{
		Reads[ShortestIndex].TimeStamp = CurrentTime;
	}
}


void ClearLastReadTimestamp(void)
{
	unsigned int NewExpiration,Delay;

	if (LastMessageIndex != -1)
	{
		Delay = FindReReadDelay(Reads[LastMessageIndex].HHPCodeID);
		NewExpiration = Reads[LastMessageIndex].TimeStamp;

		if (NewExpiration < Delay)
		{
			NewExpiration = 0xffffffff - (Delay - NewExpiration);
		}
		else
		{
			NewExpiration -= Delay;
		}

		Reads[LastMessageIndex].TimeStamp = NewExpiration;
	}
}


void ClearAllButLastReadTimestamp(void)
{
	int i;
	unsigned int NewExpiration,Delay;

	for (i=0;i<PREVIOUS_READS_TO_REMEMBER;i++)
	{
		// End of the list
		if (Reads[i].Length == -1)
			break;
		// Last Re-Read
		if (LastMessageIndex == i)
			continue;

		Delay = FindReReadDelay(Reads[i].HHPCodeID);
		NewExpiration = Reads[i].TimeStamp;

		if (NewExpiration < Delay)
		{
			NewExpiration = 0xffffffff - (Delay - NewExpiration);
		}
		else
		{
			NewExpiration -= Delay;
		}

		Reads[i].TimeStamp = NewExpiration;
	}
}


/* --------------------------------------------------
	IsNewMessage( void )

	This function uses the data returned in the output buffer (including Code ID, length,
	and various checksums) to determine if the current scan matches the previous scan.
	-------------------------------------------------- */
static unsigned int IsNewMessage(DecoderResult * msg, int ReReadDly )
{
	int Checksum = 0;
	int WeightedChecksum = 0;
	int ui = 0;
	int OldestIndex = 0;
	unsigned int ElapsedTime = 0;
	unsigned int CurrentTime = 0;
	unsigned int OldestTime = 0;

	/* Calculate 8-bit weighted and unweighted checksums for the current message */
	for( Checksum = WeightedChecksum = 0, ui=0 ; ui<msg->Length ; ++ui )
	{
		Checksum += (int)msg->Data[ui] ;
		WeightedChecksum += Checksum ;
	}

	CurrentTime = read_ms_clock();

	for (ui = 0, OldestTime = 0; ui < PREVIOUS_READS_TO_REMEMBER; ui++)
	{
		// if the length is -1 we know we've gotten to the end of the list
		if (Reads[ui].Length == -1)
			break; 

		// If CurrentTime < TimeStamp we know a rollover happened, and we take appropriate measures
		ElapsedTime = (CurrentTime >= Reads[ui].TimeStamp)?(CurrentTime - Reads[ui].TimeStamp):((0xffffffff - Reads[ui].TimeStamp) + CurrentTime);

		if (OldestTime < ElapsedTime)
		{
			OldestTime = ElapsedTime;
			OldestIndex = ui;
		}

		if ((Reads[ui].Checksum == Checksum) && (Reads[ui].WeightedChecksum == WeightedChecksum) && (Reads[ui].Length == msg->Length) && (Reads[ui].HHPCodeID == msg->HHPCodeID))
		{
			LastMessageIndex = ui;
			// Check trigger mode modifier flags (TRGMMF)
			if( !GetTrgModeModFlag() )
			{
				Reads[ui].TimeStamp = CurrentTime;

				if (ElapsedTime > ReReadDly)
					break;

			}
			else if( GetTrgModeModFlag() == 1 )
			{
				if (ElapsedTime > ReReadDly)
				{
					Reads[ui].TimeStamp = CurrentTime;
					ClearAllButLastReadTimestamp();      //new rereaddelay clear, clear all time stamps but last re-read time stamp
					break;
				}
			}

			return FALSE;
		}
	}

	// if we made it here, we're going to return TRUE.  The 2 conditions to get here are either we found it in the list, and the time had expired, or
	// it's going to be a new entry in the list.  In the case of #2, we need to add the entry, in the case of #1 it's already there so we look for cases
	// where the for-loop went all the way to the end without matching an entry in which case we replace the oldest entry.
	if (ui == PREVIOUS_READS_TO_REMEMBER)
	{
		Reads[OldestIndex].Checksum = Checksum;
		Reads[OldestIndex].WeightedChecksum = WeightedChecksum;
		Reads[OldestIndex].Length = msg->Length;
		Reads[OldestIndex].HHPCodeID = msg->HHPCodeID;
		Reads[OldestIndex].TimeStamp = CurrentTime;
		LastMessageIndex = OldestIndex;

	}
	else if (Reads[ui].Length == -1)
	{
		Reads[ui].Checksum = Checksum;
		Reads[ui].WeightedChecksum = WeightedChecksum;
		Reads[ui].Length = msg->Length;
		Reads[ui].HHPCodeID = msg->HHPCodeID;
		Reads[ui].TimeStamp = CurrentTime;

		// We filled an empty entry, and if this was not the last entry in the table, we need to indicate that the next entry is empty
		if (ui != PREVIOUS_READS_TO_REMEMBER-1)
		{
			Reads[ui+1].Length = -1;
		}

		LastMessageIndex = ui;
	}
   return TRUE ;
}

/************************************************************************/
//Scan Stand Support Functions
/************************************************************************/

bool IsScanStandSymbol(DecoderResult * msg)  										//set flag if we have decoded a scan stand
{
	bWasScanStnd=(msg->ProgrammingCode==TRUE) && (msg->Length==0);

	return bWasScanStnd;
}

bool WasLastSymbolScanStand(void)                                        //return scan stand decoded flag
{
	return bWasScanStnd;
}


/************************************************************************/
//ShotGun Support Functions
/************************************************************************/

bool Shotgun(DecoderResult *msg)
{
	bool	bDone = FALSE;
	/* First Compute this Message's character Sum & Sum-of-Sums */
	int Sum = 0;
	int SSum = 0;

	for (int i=0; i<msg->Length; i++)
	{
		Sum += msg->Data[i];
		SSum += Sum;
	}

	/* Then Sort Through the list of Previous Message ID's for
	/	 (a) a Match, in which case we Exit doing Nothing, or
	/	 (b) an Empty Location, in which case we Store this one & also Output!	*/
	for (int i=0; (i<MAX_SHOTGUN_HISTORY) && (!bDone); i++)
	{
		if( ShotgunCodeID[i] != DEFAULT_CODEID )
		{
			if( (ShotgunCodeID[i] == msg->HHPCodeID) &&
			        (ShotgunLength[i] == msg->Length) &&
			        (ShotgunSum[i] == Sum) &&
			        (ShotgunSSum[i] == SSum) )
			{
				return FALSE;
			}
		}
		else
		{
			ShotgunCodeID[i] = msg->HHPCodeID;
			ShotgunLength[i] = msg->Length;
			ShotgunSum[i]	 = Sum;
			ShotgunSSum[i]	 = SSum;
			bDone = TRUE;
		}
	}

	/* If Record of Previous Reads is Full, Discard the Oldest One! */
	if (!bDone)
	{
		int i;

		for (i=0; i<MAX_SHOTGUN_HISTORY-1; i++)
		{
			ShotgunCodeID[i] = ShotgunCodeID[i+1];
			ShotgunLength[i] = ShotgunLength[i+1];
			ShotgunSum[i]	 =	ShotgunSum[i+1];
			ShotgunSSum[i]	 =	ShotgunSSum[i+1];
		}

		/*now fill the new message into the last spot */
		ShotgunCodeID[i] = msg->HHPCodeID;
		ShotgunLength[i] = msg->Length;
		ShotgunSum[i]	 = Sum;
		ShotgunSSum[i]	 = SSum;
	}

	return TRUE;		// not a dup msg return true
}

/************************************************************************/
//Indicators For  Menu Commands  Support Functions
/************************************************************************/

UINT TimeOfLastRead = 666666666;
BOOL bHalted = 1;                     //changed from 0, from 4600r corded  1.235.1.12    LMC
bool	DoIndicatorsForMenuCommands;

/* --------------------------------------------------

	SkipIndicatorsForThisMenuCommand( void )

	If this decode result is a menu command, don't use indicators after processing it (good menu or bad menu beep).

	-------------------------------------------------- */
void SkipIndicatorsForThisMenuCommand( void )
{
	DoIndicatorsForMenuCommands = false;
	return;
}


/* --------------------------------------------------

	UseIndicatorsForThisMenuCommand( void )

	If this decode result is a menu command, use indicators after processing it (good menu or bad menu beep).

	-------------------------------------------------- */
void UseIndicatorsForThisMenuCommand( void )
{
	DoIndicatorsForMenuCommands = true;
	return;
}


#define STOP_DECODER bHalted = 1;
// *handy debug *//#define STOP_DECODER {bHalted = 1;Pt("Line# %d\r\n",__LINE__);}

#define START_DECODER bHalted = 0;
// *handy debug *//#define START_DECODER {bHalted = 0;Pt("Line# %d\r\n",__LINE__);}

#define STOP_DECODER_FLAG	bHalted

int DecoderSendingMessage = -1;
MT_Mutex_t InProcessResult = NULL;
#define RETURN {MT_ReleaseMutex(InProcessResult);return;}


/**************************************************************************************/
/* Process result                                                                     */
/* called by decoder to handle messages                                               */
/* queue data messages into barcode fifo process menu messages                        */
/* handle auttrigger timeouts and concatination beep                                  */
/* changes global variables to indicate decoding status as follows:						  */
/*                                                                                    */
/* int  ProcessedResult: 1 if we should stop scanning and process somthing         	  */
/*						 also 1 if a menu or append is processed with no message in			  */
/*						 DecodeResults Q.  											  					  */
/*                       0 if nothing read or if a duplicate message is read in   	  */
/*                       2 if we had an append result and we need to see an untrigger */
/*                       before decoding again                                        */
/* DecodeResults a pointer to a fifo containing messages to process					  	  */
/**************************************************************************************/

static void ProcessResult (int /*decoder*/, DecoderResult *res/* decoders message structure*/)
{
	int codes = 0;

	for( ; codes < res[0].AppendTotal; codes++ ) {
		CBarcodeData * gpCurrentDecoded = NULL;
		bool DataEditFormatIngore = false;
		DecoderResult *msg = &res[codes];

		if (ImageQualityMode() == 4 && !PresModeDecoderTriggered) {
			return;
		}

		if (msg->ProgrammingCode == TRUE && msg->DecodeType == DEC_TYPE_FLD )
			return;

		if (!MT_WaitMutex(InProcessResult,0))
		return;

		if( DecoderSendingMessage >= 0 && DecoderSendingMessage != msg->DecodeType && ProcessedResult )	{
			RETURN;
		}
//ewr06509		
//#ifdef XENON_LITE_1500
//		Normalize1500(msg->HHPCodeID);
//#endif
		if (msg->HHPCodeID == '!' && GET_SETTING(SudokuEnabled) == 2) {
			showSudoku(P_SCANBUFFER, nWidth, nHeight, (char *)(msg->Data));
		}

		if (GET_SETTING(DecoderExtraInfo) && msg->Data[0] == '<' && msg->Data[1] == '~') {
			int j = 0;

			while (j < msg->Length)
				if (msg->Data[j++] == '>')
					break;

			j+=2; // CR and LF
			theStdInterface.Write((char *) msg->Data, j);

			for (int i=0; i<msg->Length-j; i++) {
				msg->Data[i] = msg->Data[i+j];
			}

			msg->Length = msg->Length - j;
		}

		DoIndicatorsForMenuCommands = true;

		if (OnlyAllowMenuDecodes && msg->ProgrammingCode != DEC_TRUE) {
			STOP_DECODER;
			STOP_PLUGIN_DECODER;
			RETURN;
		}

		if ((IgnoreTrigger || ReDecoding) && !OnlyAllowMenuDecodes) {
			if (!CTrigger::GetShotgunMode()) {
				STOP_DECODER;
				STOP_PLUGIN_DECODER;
			}

			if (msg->Length >= 0) {
				theStdInterface.printf("Decode Msg(%04d,%c%c%c) = ",msg->Length, msg->HHPCodeID, msg->AIMCodeLetter, msg->AIMModifier);
				theStdInterface.Write((char *) msg->Data, msg->Length);
				//theStdInterface.printf("\r\nDecode Time = %d ms \r\n",BaselineClock-DecodingStartTime);
				ProcessedResult=1;

				DecoderSendingMessage = msg->DecodeType;

				StoreLastDecodeInfo(msg);
			}

			RETURN;
		}

		//TRACE(" ProcessResult:");
		if( (!CTrigger::GetShotgunMode()) && (!theScriptInterpreter.IsSequenceStarted()) ) {
				if((!msg->ProgrammingCode)&&(GET_SETTING(NoMatchError))&&((GET_SETTING(DataFormatEnable) == 2)||(GET_SETTING(DataFormatEnable) == 4))) {
					CBarcodeData * pCurrentDecMsg;
					pCurrentDecMsg = CBarcodeData::StoreToNewBuffer(msg->Data, msg->Length);
					pCurrentDecMsg->StoreHHP(msg->HHPCodeID, 0);
					pCurrentDecMsg->StoreAim(msg->AIMCodeLetter, msg->AIMModifier);
					CBarcodeFifo *m_FormatterDecResultsQueue=new CBarcodeFifo(200);

					if(DoDataEditOrScript(pCurrentDecMsg, m_FormatterDecResultsQueue)) {
						STOP_DECODER;              //lmc  sequence started returns status of preferrd and sequence
						STOP_PLUGIN_DECODER;
					}
					else {	
						DataEditFormatIngore=true;
					}
					
					/*not a bug...this was not the real call to dataformatter.  This was a fake one to determine if we should keep this decode
					or not...so no matter what the formatter does with what we passed, we throw it away. The return results from DF 
					is whats important in this case */
					pCurrentDecMsg->Free();
					m_FormatterDecResultsQueue->Purge();
					delete m_FormatterDecResultsQueue;
				}
			else {
				STOP_DECODER;
				STOP_PLUGIN_DECODER;
			}
		}

		if( IsScanStandSymbol(msg) || IsWaitingForScanStand() )	{ //if decoded scan stand or triggerd and waiting for scan stand
			RETURN;
		}

		if (CTrigger::GetShotgunMode()) {
			if (!Shotgun(msg)) {
				RETURN;
			}
				if(!(msg->ProgrammingCode)&&(GET_SETTING(NoMatchError))&&((GET_SETTING(DataFormatEnable) == 2)||(GET_SETTING(DataFormatEnable) == 4))) {
					CBarcodeData * pCurrentDecMsg;
					pCurrentDecMsg = CBarcodeData::StoreToNewBuffer(msg->Data, msg->Length);
					pCurrentDecMsg->StoreHHP(msg->HHPCodeID, 0);
					pCurrentDecMsg->StoreAim(msg->AIMCodeLetter, msg->AIMModifier);
					CBarcodeFifo *m_FormatterDecResultsQueue=new CBarcodeFifo(200);

					if(DoDataEditOrScript(pCurrentDecMsg, m_FormatterDecResultsQueue))
						NumberOfShotgunMessages++;
					else {	
						DataEditFormatIngore=true;
					}

					/*not a bug...this was not the real call to dataformatter.  This was a fake one to determine if we should keep this decode
					or not...so no matter what the formatter does with what we passed, we throw it away. The return results from DF 
					is whats important in this case */
					pCurrentDecMsg->Free();
					m_FormatterDecResultsQueue->Purge();
					delete m_FormatterDecResultsQueue;
				}
			else
				NumberOfShotgunMessages++;

			if (NumberOfShotgunMessages>=GET_SETTING(Shotgun) && GET_SETTING(Shotgun) != 1) {
				STOP_DECODER;
				STOP_PLUGIN_DECODER;
			}
		}

		NumberOfDecodeAttempts = 0; // restart the vigor and timeouts
		unsigned int reread = 0;		//was bool rereadok = FALSE; ,  old reread

		switch (msg->HHPCodeID) {
			case  DEC_HHP_CODE_ID_AZTEC_CODE:
			case  DEC_HHP_CODE_ID_DATAMATRIX:
			case  DEC_HHP_CODE_ID_MAXICODE:
			case  DEC_HHP_CODE_ID_QRCODE:
			case  DEC_HHP_CODE_ID_PDF417:
			case  DEC_HHP_CODE_ID_MICROPDF:
				// Reset the 2D Reread delay for all 2D symbologies - individual delays can be created in the future
				reread = ((GET_SETTING(ReReadDly2D)>0)?(GET_SETTING(ReReadDly2D)):(GET_SETTING(ReReadDly)));
				//rereadok = pTrigger->HasReReadTimeout2DExpired();     //old reread  lmc
				//pTrigger->SetReReadTimeout2D();                       //old reread  lmc
				break;
			default:
				// Reset the original (1D) Reread Delay for all other symbologies
				reread = GET_SETTING(ReReadDly);
				//rereadok = pTrigger->HasReReadTimeoutExpired();       //old reread  lmc
				//pTrigger->SetReReadTimeout();                         //old reread  lmc
				break;
		}

		NumberOfDecodes++;
		TimeOfLastRead = BaselineClock;
		//pTrigger->ResetReReadTime(); // reset reread delays     //old reread  lmc
	
		if(!DataEditFormatIngore) {
			STOP_DECODER;
			STOP_PLUGIN_DECODER;
		}
	
		DecoderSendingMessage = msg->DecodeType;

		if ( msg->Length >= 0 ) { 				// if we have somthing to process get a buffer for it
				gpCurrentDecoded = CBarcodeData::StoreToNewBuffer(msg->Data, msg->Length);
			
			gpCurrentDecoded->StoreHHP(msg->HHPCodeID, 0);
			gpCurrentDecoded->StoreAim(msg->AIMCodeLetter, msg->AIMModifier);

			switch (msg->HHPCodeID) {
				case DEC_HHP_CODE_ID_AZTEC_CODE:
				case DEC_HHP_CODE_ID_DATAMATRIX:
				case DEC_HHP_CODE_ID_MAXICODE:
				case DEC_HHP_CODE_ID_QRCODE:
					UsageStatistics.NumberDecodes2D++;
					UsageStatistics.Accum2DTime+=BaselineClock-DecodingStartTime;
					break;
				
				case DEC_HHP_CODE_ID_PDF417:
				case DEC_HHP_CODE_ID_MICROPDF:
					UsageStatistics.NumberDecodesPDF++;
					UsageStatistics.AccumPDFTime+=BaselineClock-DecodingStartTime;
					break;
				
				default:
					UsageStatistics.NumberDecodes1D++;
					UsageStatistics.Accum1DTime+=BaselineClock-DecodingStartTime;
					break;
			}

			//spd for ImgBox *$^* NOTE this implies 3.5 - 4ms per decode to save the last decoded image "off to the side"... we might consider adding a enable/disable for feature *$^*
			StoreLastDecodeInfo(msg);
		}
		else {
			Click();
			RETURN;
		}

#if defined(SM4100)
		if( (GetTriggerMode() == AUTOMATIC_TRIGGER) || (GetTriggerMode() == PRESENTATION_TRIGGER) || (GetTriggerMode() == STREAMING_PRESENTATION_TRIGGER) || ((GET_SETTING(IRorTrigger)==1)&&(GET_SETTING(bIRSameCodeDelayEnable)==1)))
#elif defined(IR_TRIGGER)
		if( (GetTriggerMode() == AUTOMATIC_TRIGGER) || (GetTriggerMode() == PRESENTATION_TRIGGER) || (GetTriggerMode() == STREAMING_PRESENTATION_TRIGGER) || ((GET_SETTING(IrTriggerEnable) != IR_TRG_OFF)&&(GET_SETTING(bIRSameCodeDelayEnable)==1)))
#else
		if( (GetTriggerMode() == AUTOMATIC_TRIGGER) || (GetTriggerMode() == PRESENTATION_TRIGGER) || (GetTriggerMode() == STREAMING_PRESENTATION_TRIGGER) )
#endif
		{
			if( IsNewMessage(msg,reread) == FALSE )	{		//was if (!IsNewMessage(msg)  && !rereadok)    //old reread  lmc
				if( (GET_SETTING(PrioritySequence)) ) {     //lmc  if message is a repeat, then reset preferred timeout
					if( !theScriptInterpreter.SequencePriorityTimeoutExpired() ) {
						gpCurrentDecoded->Free();	// return the buffer
						RETURN;
					}
				}
				else {
					gpCurrentDecoded->Free();	// return the buffer
					RETURN;
				}
			}//end if IsNewMessage
		} //end if GetTriggerMode ==

		HONPlugin *LockedPlugin = SearchLockedPlugin();

		if (msg->ProgrammingCode == TRUE) { 		// decoder says we have a menu
			UINT Size;
			tchar_t *pContents;
			int menuresult;
			ProcessedResult=1;			// tell outside we had something and to wait for untrigger
		
			DecoderSendingMessage = msg->DecodeType;

			unsigned char pucBuffer[SIZEOF_HIGHLEVEL_PARSE_BUFFER];

			STOP_DECODER;
			STOP_PLUGIN_DECODER;
		
			gpCurrentDecoded->GetContentsBuffer(pContents, Size); // get pointer to data/size  for menu routine
		
			// Don't tamper with the original data by copying it via strcpy, because there may be embedded NULLs in some of our HTAG programming
			// symbols.  However, we will NULL terminate the end of the buffer just in case any of the existing routines rely on that (but I don't
			// think that any of them do).
			memcpy((char *)pucBuffer,(char *)pContents, Size);
			pucBuffer[ Size ] = 0;
			// Apply the next setting to custom defalt table.

			if(ReaderConfiguration->AffectedTableChangedToDefalt) {
				AffectedDefalt=true;

				if(ReaderConfiguration->DirectDefaultCommandPrivilege) {
					ReaderConfiguration->AffectedTableChangedToDefalt=false;
				}
				else {
					ReaderConfiguration->DirectDefaultCommandPrivilege=true;
				}
				
				unsigned char	TableCharacter = pucBuffer[Size-1];

				if(( TableCharacter == '.' )||( TableCharacter== '!' )||( TableCharacter =='&' )||( TableCharacter == '@' )) {
					pucBuffer[Size-1]='@';
				}
			}
			
			if( GET_SETTING( ShowMenuSymbols ) || GET_SETTING( MenuingDebugMode ) ) {
				SendStringToInterface( (char *)pucBuffer, strlen( (char *) pucBuffer ) );
			}

			gpCurrentDecoded->Free();	// return the buffer
			gpCurrentDecoded=NULL;
			
			/*Is Barcode Menuing enabled or is the code scan contain MNUENA1? */
			/* I know I don't handle lower case here but so be it */
			IsST3800MenuSymbol(&Size,pucBuffer);// if its got a leading tilda ids etc remove them

			if( GET_SETTING( MenuingEnabled ) ||
				NULL != strstr((char *)pucBuffer,"0D06021") 	// Corded scanners only allowed to scan in low and medium security levels
			) {
				// If this is a menu command we need to shutdown and HHLC output activity.
				// Send a request to shut down HHLC and wait for it to do so.
				// Only done if request responds with a PASS.  YES we will reset if we wait too long.
				if (IOCRET_PASS == theStdInterface.DeviceIoControl(IOCTRL_REQUEST_HHLC_SHUT_DOWN)) {
					while (IOCRET_NO == theStdInterface.DeviceIoControl(IOCTRL_IS_DONE))
						;
				}

#ifdef PLUGIN_DEV
				HSTRING PluginMenuCommand("",true);
				HSTRING SystemMenuCommand("",true);
				PluginMenuCommand.Copy(pucBuffer, Size);
				SystemMenuCommand.Copy(pucBuffer, Size);
							
				int PluginCmd = AbstractPluginMenuCommands(&PluginMenuCommand);
				int SystemCmd = SystemCmd = AbstractSystemMenuCommands(&SystemMenuCommand);
				bool PluginMenuProcessed = true;

				if(PluginCmd)
					if(!MultiPluginMenuBarcodesProcess(&PluginMenuCommand, PluginCmd))
						PluginMenuProcessed = false;


				if(!SystemCmd) {
					if(PluginMenuProcessed)
						SoundMenuBeep();
					else
						SoundErrorBeep();
				}
				else {
					memcpy((char *)pucBuffer,(char *)SystemMenuCommand.Char, SystemMenuCommand.Size);
					pucBuffer[ SystemMenuCommand.Size ] = 0;
					Size = SystemMenuCommand.Size;
#endif
					// If this is a <SYN>FE command, process it as such, else try to process it as a menu command.
					if( HTAG::ValidateDataStreamAsSYNFECommand( (char *) pucBuffer, Size ) == HTAG::HTAG_OK) {
						char	Result[160];
						int		SYNFEParseStatus = SYNFECommandHandler->ReceiveAndParseServer( Result, (char *) pucBuffer, Size );

						if( DoIndicatorsForMenuCommands ) {
							if( SYNFEParseStatus == HTAG::HTAG_OK ) {
								SoundMenuBeep();
							}
							else {
								SoundErrorBeep();
							}
						}
					}
					else {
						if ((menuresult =EnterScannedMenu(&Size,pucBuffer)) == MENU_QUERY) {	// if its not a query then we send nothing
							gpCurrentDecoded = CBarcodeData::StoreToNewBuffer(pucBuffer,Size);
							gpCurrentDecoded->StoreHHP('6', 0);
							gpCurrentDecoded->StoreAim('Z', '6');
						}

						if(!bKcodeMode) {
							if(ReaderConfiguration->DirectDefaultCommandPrivilege && AffectedDefalt && (!ReaderConfiguration->AffectedTableChangedToDefalt)) {
								ReaderConfiguration->AffectedTableChangedToDefalt=false;
							}
							else if(ReaderConfiguration->DirectDefaultCommandPrivilege && AffectedDefalt && (ReaderConfiguration->AffectedTableChangedToDefalt)) {
								ReaderConfiguration->AffectedTableChangedToDefalt=false;
								ReaderConfiguration->DirectDefaultCommandPrivilege=false;
							}
							
							AffectedDefalt=false;
						}

						if( DoIndicatorsForMenuCommands ) {
							if( menuresult!=MENU_ERROR &&
								(strstr((const char *)pucBuffer,"\005")==NULL) &&
								(strstr((const char *)pucBuffer,"\025")==NULL)) {	  // is this a result failure or NAK or ENQ?
						
									if((ReaderConfiguration->GetActiveWorktableNumber()!=ReaderConfiguration->CurrentWorkTableForScannedMenu())||ReaderConfiguration->TableSettingUpdateSimultaneouslyForScannedCommand) {
#ifdef PLUGIN_DEV
										if(!PluginMenuProcessed)
											SoundErrorBeep();
										else
#endif	
											SoundDifferentMenuBeep();
				
										if(!ReaderConfiguration->ScannedCBLTBLMenu) {
											ReaderConfiguration->SetCurrentTableForScannedCommand(ReaderConfiguration->GetActiveWorktableNumber());
											ReaderConfiguration->TableSettingUpdateSimultaneouslyForScannedCommand = false;
										}
									}
									else
#ifdef PLUGIN_DEV
										if(!PluginMenuProcessed)
											SoundErrorBeep();
										else
#endif
											SoundMenuBeep();
							}
							else {
								SoundErrorBeep(true);			// had an error in menuing error beep
							}
						}
					}

#ifdef PLUGIN_DEV
				}
#endif
			}
		}
#ifdef PLUGIN_DEV
		// if there is a plugin locked, all the scanned data codes will be tossed
		else if(LockedPlugin) {
			ProcessedResult=1;
			gpCurrentDecoded->Free();	// return the buffer
			gpCurrentDecoded = NULL;	// tell we have nothing to send
			SoundErrorBeep();
		}
#endif

		// if we havent returned yet we have a regular barcode msg check good read delay
		// let multiple messages from one decode output without gr delay. coupon codes for example
		else if (!HasGoodReadTimeoutExpired() && (ProcessedResult==0 || ProcessedResult==2) ) {
			gpCurrentDecoded->Free();	// return the buffer
			gpCurrentDecoded = NULL;	// tell we have nothing to send
			//pTrigger->ClearReReadTimeouts();
			ClearLastReadTimestamp();               //lmc  4600r addition

			RETURN;
		}
	
		if (gpCurrentDecoded!=NULL) {
			if(DataEditFormatIngore) {
				gpCurrentDecoded->Free();  /*give back the buffer, in the end the dataformatter qualifier said we did not want this message */
				DataEditFormatIngore=false;
				ProcessedResult=0;
			}
			else {
				DecodeResults->Write(gpCurrentDecoded);	// if we are not concatinating put msg in q
				ProcessedResult=(ProcessedResult==0)?1:ProcessedResult;	// tell outside we had something and to wait for untrigger
			}
		
			DecoderSendingMessage = msg->DecodeType;
		
			ResetGoodReadTime();
		}
	}//for()
	
		
	if(STOP_DECODER_FLAG) {
		StopDecode();
	}
	RETURN;
}

#ifdef PLUGIN_DEV
#ifdef PLUGIN_DECODER
/************************************************************************
 * These functions are exposed to plugin decoders.
 ************************************************************************/
// PluginDecoderMayContinue is a Plugin decoder callback.
//  0 -> Exit decoder immediately
//  1 -> Continue with searching and decoding
int PluginDecoderMayContinue (void)
{

	/* FIXME: What will happen if we are linear? */
	if (GET_CONFIG_SETTING(iOpticsPopulation) == TWO_D)                // Are we 2D??
	{
		if ((STOP_PLUGIN_DECODER_FLAG) /*|| ((BaselineClock-DecodingStartTime) > DecTmx)*/)
			return (0);

		if( !IgnoreTrigger && !pTrigger->IsUnitTriggered() )
		{
			return 0;
		}


	}

	return (1);
}

/* these structures are copied from legacy products.
	At the earliest opportunity, change this functionality to use the new structure defined in DecoderInterface.h
*/

//needed until grayscaleimage descriptor is removed
/*typedef enum {
	DECODE_TYPE_FAST = 0,
	DECODE_TYPE_FULL = 1
} DecodeType_t;

typedef struct {
   signed int x;
   signed int y;
} Corner_t;

typedef struct {
	Corner_t Corners[4];
	int D_Corner;
	int PdfRows;
	int PdfCols;
} IQImgInfo_t;

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
} DecodeResult;*/
//end of old structures

int PluginDecoderProcessResult(DecodeResult * msg)
{
	DecoderResult result;
	for( int jk = 0; jk < msg->length; jk++ ) {
		result.Data[jk] = msg->message[jk];
	}

	result.Length = msg->length;
	result.ProgrammingCode = msg->menuFlag;
	result.HHPCodeID = msg->HHPcodeID;
	result.AIMCodeLetter = msg->AIMcodeLetter;
	result.AIMModifier = msg->AIMcodeModifier;
	result.DecodeType = DEC_TYPE_ID;
	result.AppendIndex = 0;
	result.AppendTotal = 1;
	
	result.MainBounds.corners[0].x = result.IQImageInfo.bounds.corners[0].x = msg->IQImgInfo.Corners[0].x;
	result.MainBounds.corners[0].y = result.IQImageInfo.bounds.corners[0].y = msg->IQImgInfo.Corners[0].y;
	result.MainBounds.corners[1].x = result.IQImageInfo.bounds.corners[1].x = msg->IQImgInfo.Corners[1].x;
	result.MainBounds.corners[1].y = result.IQImageInfo.bounds.corners[1].y = msg->IQImgInfo.Corners[1].y;
	result.MainBounds.corners[2].x = result.IQImageInfo.bounds.corners[2].x = msg->IQImgInfo.Corners[2].x;
	result.MainBounds.corners[2].y = result.IQImageInfo.bounds.corners[2].y = msg->IQImgInfo.Corners[2].y;
	result.MainBounds.corners[3].x = result.IQImageInfo.bounds.corners[3].x = msg->IQImgInfo.Corners[3].x;
	result.MainBounds.corners[3].y = result.IQImageInfo.bounds.corners[3].y = msg->IQImgInfo.Corners[3].y;

	ProcessResult(0,&result);
	return 0;
}

unsigned int PluginDecoderMsTimer()
{
	return MsTimer();
}

#endif // PLUGIN_DECODER
#endif // PLUGIN_DEV

extern "C"     void Fast_memcpy(void *, void *, int);

static int bSD_DecodeInProgress=0;
static int bSD_DataMatrixTimedSearch=0,bSD_TimedSearchTimeout=0,bSD_OCRTimedSearch=0;

static void  SD_CB_Progress(int Handle)
{
	int kill = 0;
	int searchFactor = 1;

	if( /*GET_SETTING(VideoReverse) ||*/ GET_SETTING(ocrEnable) ) {
		searchFactor = 2;
	}

	if (STOP_DECODER_FLAG)
	{
		kill = 1;
	}
	else if (!IgnoreTrigger && !pTrigger->IsUnitTriggered())
	{
		kill = +2;
	}
	//with Video Reverse enabled, the full decode potientially tries 2 times. If the decoder has a timer (which is should), the decoder will
	//split the time based on the DECTMN setting, so on the outside, simply allow double the search time. Video reverse is only applicable to
	//linears, so we should not have to change the DECTMX, that is really for decoding larger 2D symbols.
	else if ((bSD_DecodeInProgress == 0)&&((BaselineClock - DecodingStartTime) > (DecTmn*searchFactor)))
	{
		kill = +4;
	}
	// DecTMX applies to everything, including trumping the data matrix search
	else if ((BaselineClock - DecodingStartTime) > DecTmx)
	{
		kill = +8;
	}
	// The data matrix search is different from the regular search.  It is allocated an amount of time to search
	//   which overrides DecTmn.  It can still get trumped by DecTMX, but can go past DecTMN if it needs to.  If
	//   the search time for data matrix runs out, we call PauseDecode which will tell the decoder to abandon
	//   the data matrix attempt, but go onto the next part of the cycle.
	else if ((bSD_DataMatrixTimedSearch) && (BaselineClock > bSD_TimedSearchTimeout)) {
		PauseDecode();
	} 
	else if( (bSD_OCRTimedSearch) && (BaselineClock > bSD_TimedSearchTimeout) ) {
		kill = +16;
	}
	else {
		ContinueDecode();
	}

	if (kill)
	{
		StopDecode();
	}
}

//#define  SR_STATUS_NO_COMPOSITE           1
//#define  SR_STATUS_COARSE_FIND_COMPLETE   2
//#define  SR_STATUS_DM_BEGIN				3
//#define  SR_STATUS_DM_COMPLETE			4
//#define  SR_STATUS_PDF_BEGIN              5
//#define  SR_STATUS_PDF_COMPLETE           6
//#define  SR_STATUS_NO_PACKAGE             7
//#define  SR_STATUS_QR_BEGIN               8
//#define  SR_STATUS_QR_COMPLETE            9
//#define  SR_STATUS_SKIP_CHUNK            10
//#define  SR_STATUS_MC_BEGIN              11
//#define  SR_STATUS_MC_COMPLETE           12
//#define  SR_STATUS_AZ_BEGIN              13
//#define  SR_STATUS_AZ_COMPLETE           14

#define SD_IN_PDF_DECODE	(1<<0)
#define SD_IN_QR_DECODE		(1<<1)
#define SD_IN_MAXI_DECODE	(1<<2)
#define SD_IN_AZTEC_DECODE	(1<<3)
#define SD_IN_DATAMX_DECODE (1<<4)
#define SD_IN_DATAMX_SEARCH (1<<12)
#define SD_IN_OCR_DECODE	(1<<13)

static void  SD_CB_Status(int Handle, int Status)
{
//	int x = bSD_DecodeInProgress;
//   printf("Stat(%d)@%dms\r\n",Status,BaselineClock-DecodingStartTime);
	switch(Status) {
		// Data Matrix
		case 3: // decode in
			bSD_DecodeInProgress |= SD_IN_DATAMX_DECODE;
			break;
		case 4: // decode out
			bSD_DecodeInProgress &= ~SD_IN_DATAMX_DECODE;
			break;
		case 0x100: // search in
			bSD_DataMatrixTimedSearch = TRUE;
			bSD_DecodeInProgress |= SD_IN_DATAMX_SEARCH;
			bSD_TimedSearchTimeout = BaselineClock + GET_SETTING(idmSearchTimeOut);
			break;
		case 0x101: // search out
			bSD_DecodeInProgress &= ~SD_IN_DATAMX_SEARCH;
			bSD_DataMatrixTimedSearch = FALSE;
			break;


		// PDF
		case 5: // in
			bSD_DecodeInProgress |= SD_IN_PDF_DECODE;
			break;
		case 6: // out
			bSD_DecodeInProgress &= ~SD_IN_PDF_DECODE;
			break;

		// QR
		case 8: // in
			bSD_DecodeInProgress |= SD_IN_QR_DECODE;
			break;
		case 9: // out
			bSD_DecodeInProgress &= ~SD_IN_QR_DECODE;
			break;

		// Maxicode
		case 11: // in
			bSD_DecodeInProgress |= SD_IN_MAXI_DECODE;
			break;
		case 12: // out
			bSD_DecodeInProgress &= ~SD_IN_MAXI_DECODE;
			break;

		// Aztec
		case 13: // in
			bSD_DecodeInProgress |= SD_IN_AZTEC_DECODE;
			break;
		case 14: // out
			bSD_DecodeInProgress &= ~SD_IN_AZTEC_DECODE;
			break;
			
		case 15:
			bSD_OCRTimedSearch = DEC_TRUE;
			bSD_DecodeInProgress |= SD_IN_OCR_DECODE;
			bSD_TimedSearchTimeout = BaselineClock + GET_SETTING(DecodeMaxTimeout);
			break;
		case 16:
			bSD_OCRTimedSearch = DEC_FALSE;
			bSD_DecodeInProgress &= ~SD_IN_OCR_DECODE;
			break;
	}
//theStdInterface.printf("DecStatus - in:0x%x, out:0x%x\r\n",x,bSD_DecodeInProgress);
	SD_CB_Progress(Handle);
}

//notifies the system that there was a result and it is being held pending other results
//For appends
static void SD_CB_ResultNotify( int status ) {
	if( status == 0 ) { //no errors
		BeepBip();
	}
	else { //had some error, only error at the moment is the data exceeded the MAX_DECODED_DATA_LENGTH
		SoundErrorBeep();
	}

	ProcessedResult = 2;
	ResetGoodReadTime();
	STOP_DECODER;
	StopDecode();
}

/*ewr06509
#ifdef XENON_LITE_1500
static void Normalize1500(int codeID)
{
	//got these list of codeIDs from marketing, does not cover all linears, but the important ones 
	int delaylst[] = {0x44,0x45,0x49,0x59,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6d,0x6e,0x79,0x7b,0x7d};

	for(int i=0;i <= sizeof(delaylst)/sizeof(int);i++)
	{
		if(delaylst[i]==codeID)
		{
			MT_Delay(125); 
			break;
		}
	}
}
#endif*/

#if (DECODER_ID)
void FillSwiftDecoderMenu(void) {

	DecodeSet(DEC_GENERAL_IMPROVEMENTS,			(void *)GET_SETTING(GeneralDecodeImprovements));  // Correctly output AIM modifier for GS1-128 (formerly UCC/EAN 128)
   DecodeSet(DEC_ECI_HANDLING,			(void *)GET_SETTING(DecoderECIHandling));
#ifdef DEVELOPMENT_BUILD
	DecodeSet(DEC_FULL_DECODER_ENABLED, (void *)!GET_SETTING(DisableDecoder));
#endif
   	//DecodeSet(DEC_CELL_PHONE_IMPROVEMENTS,	(void *)GET_SETTING(CellPhoneMode) );
	DecodeSet(DEC_LOW_ASPECT_RATIO,		(void *)GET_SETTING(LowApsectRatioLinearFinding) );
   	DecodeSet(DEC_LOW_CONTRAST_IMPROVEMENTS,(void *)GET_SETTING(LowSymbolContrast) );
   	if(DecodeSet(DEC_UPC_IMPROVE_BOUNDS,		(void *)GET_SETTING(ImproveBounds) )){
	   	DecodeSet(DEC_C128_IMPROVE_BOUNDS,		(void *)GET_SETTING(ImproveBounds) );
	   	DecodeSet(DEC_CODE39_IMPROVE_BOUNDS,	(void *)GET_SETTING(ImproveBounds) );
	   	DecodeSet(DEC_I25_IMPROVE_BOUNDS,		(void *)GET_SETTING(ImproveBounds) );
	   	DecodeSet(DEC_S25_IMPROVE_BOUNDS,		(void *)GET_SETTING(ImproveBounds) );
	   	DecodeSet(DEC_IATA25_IMPROVE_BOUNDS,	(void *)GET_SETTING(ImproveBounds) );
	   	DecodeSet(DEC_M25_IMPROVE_BOUNDS,		(void *)GET_SETTING(ImproveBounds) );
	   	DecodeSet(DEC_NEC25_IMPROVE_BOUNDS,		(void *)GET_SETTING(ImproveBounds) );
	   	DecodeSet(DEC_CODE11_IMPROVE_BOUNDS,	(void *)GET_SETTING(ImproveBounds) );
	   	DecodeSet(DEC_CODABAR_IMPROVE_BOUNDS,	(void *)GET_SETTING(ImproveBounds) );
	   	DecodeSet(DEC_TELEPEN_IMPROVE_BOUNDS,	(void *)GET_SETTING(ImproveBounds) );
	   	DecodeSet(DEC_MSI_IMPROVE_BOUNDS,		(void *)GET_SETTING(ImproveBounds) );
	   	DecodeSet(DEC_MICROPDF_IMPROVE_BOUNDS,	(void *)GET_SETTING(ImproveBounds) );
	   	DecodeSet(DEC_CODE93_IMPROVE_BOUNDS,	(void *)GET_SETTING(ImproveBounds) );
	   	DecodeSet(DEC_HK25_IMPROVE_BOUNDS,		(void *)GET_SETTING(ImproveBounds) );
   	}
   	if(DecodeSet(DEC_C128_SHORT_MARGIN,			(void *)GET_SETTING(LinearShortQuietZones) )) {
	   	DecodeSet(DEC_CODE39_SHORT_MARGIN,		(void *)GET_SETTING(LinearShortQuietZones) );
	   	DecodeSet(DEC_TRIOPTIC_SHORT_MARGIN,	(void *)GET_SETTING(LinearShortQuietZones) );
		DecodeSet(DEC_UPC_SHORT_MARGIN,			(void *)(GET_SETTING(LinearShortQuietZones) >=2 ? 1 : GET_SETTING(LinearShortQuietZones) )); //this seting only supports 0 or 1
   	  	DecodeSet(DEC_CODABAR_SHORT_MARGIN,		(void *)(GET_SETTING(LinearShortQuietZones)>=2 ? 1 : GET_SETTING(LinearShortQuietZones) ));//this seting only supports 0 or 1
   		DecodeSet(DEC_I25_SHORT_MARGIN,			(void *)(GET_SETTING(LinearShortQuietZones) >=2 ? 1 : GET_SETTING(LinearShortQuietZones) )); //this seting only supports 0 or 1
   		DecodeSet(DEC_CODE93_SHORT_MARGIN,			(void *)(GET_SETTING(LinearShortQuietZones) >=2 ? 1 : GET_SETTING(LinearShortQuietZones) )); //this seting only supports 0 or 1
   	}
   	if(DecodeSet(DEC_NEC25_ENABLED,			(void *)GET_SETTING(N25Enable) )){
	   	DecodeSet(DEC_NEC25_MIN_LENGTH,			(void *)GET_SETTING(N25MinLength) );
	   	DecodeSet(DEC_NEC25_MAX_LENGTH,			(void *)GET_SETTING(N25MaxLength) );
		switch(GET_SETTING(N25ChkChar)) {
			case 1:		DecodeSet(DEC_NEC25_CHECK_DIGIT_MODE, (void *)DEC_STRIPPED ); break;
			case 2:		DecodeSet(DEC_NEC25_CHECK_DIGIT_MODE, (void *)DEC_ENABLED ); break;
			default:	DecodeSet(DEC_NEC25_CHECK_DIGIT_MODE, (void *)DEC_DISABLED ); break;
		}
   	}
   	DecodeSet(DEC_DATAMATRIX_NON_SQUARE_MODULES,(void *)GET_SETTING(idmNonSquareModules) );
   	DecodeSet(DEC_DATAMATRIX_SHIFTED_TILES,		(void *)GET_SETTING(idmShiftedTiles) );
	DecodeSet(DEC_DATAMATRIX_LOW_CONTRAST,		(void *)GET_SETTING(idmLowContrast) );
	DecodeSet(DEC_DATAMATRIX_SYMBOL_SIZE,	    (void *)GET_SETTING(idmSmallSymbolSize) );
	DecodeSet(DEC_DATAMATRIX_RECTANGLE,		(void *)1);
	DecodeSet(DEC_QR_NON_SQUARE_MODULES,		(void *)GET_SETTING(qrcNonSquareModules) );
	DecodeSet(DEC_QR_SYMBOL_SIZE,	    		(void *)GET_SETTING(qrcSmallSymbolSize) );
	DecodeSet(DEC_MAXICODE_SYMBOL_SIZE,			(void *)GET_SETTING(maxSmallSymbolSize) );
	DecodeSet(DEC_AZTEC_SYMBOL_SIZE,			(void *)GET_SETTING(aztSmallSymbolSize) );

	DecodeSet(DEC_OCR_ACTIVE_TEMPLATES,			(void *)GET_SETTING(ocrActiveTemplates) );
	DecodeSet(DEC_OCR_BUSY_BACKGROUND,			(void *)GET_SETTING(ocrBusyBackground) );

}
#endif //DECODER_ID

#if (DECODER_AD)
void FillAdaptusDecoderMenu(void) {

	DecodeSet(DEC_STACKED_CAPABLE,	(void *)DEC_TRUE);
	DecodeSet(DEC_MATRIX_CAPABLE,	(void *)DEC_TRUE);

   // 2D Image Decoding Adjustments
	DecodeSet(DEC_PRINT_WEIGHT,				(void *)GET_SETTING(PrintWeight));
	DecodeSet(DEC_PRINT_LOCALIZATION,		(void *)GET_SETTING(PrintLocalization));
    //DecodeSet(DEC_FIXED_MOUNT_MODE,			(void *)GET_SETTING(DecodeFixedMountMode));
	DecodeSet(DEC_SCAN_ZONE,				(void *)GET_SETTING(ScanZone));
	DecodeSet(DEC_DECODE_BOX,				(void *)GET_SETTING(DecodeBox));
	DecodeSet(DEC_DECODE_BOX_WIDTH,			(void *)GET_SETTING(DecodeBoxWidth));
	DecodeSet(DEC_DECODE_BOX_HEIGHT,		(void *)GET_SETTING(DecodeBoxHeight));
	DecodeSet(DEC_DECODE_WIDTH,				(void *)GET_SETTING(DecodeWidth));

	DecodeSet(DEC_DECODE_SEARCH_MODE,		(void *)GET_SETTING(DecodeMode));
	DecodeSet(DEC_ALD_SLIT_HEIGHT,			(void *)GET_SETTING(DecodeSlitHeight));
	//DecodeSet(DEC_ALD_START_POINT_X,		(void *)GET_SETTING(AldStartPointX));
	//DecodeSet(DEC_ALD_START_POINT_Y,		(void *)GET_SETTING(AldStartPointY));
	//DecodeSet(DEC_ALD_STOP_POINT_X,			(void *)GET_SETTING(AldStopPointX));
	//DecodeSet(DEC_ALD_STOP_POINT_Y,			(void *)GET_SETTING(AldStopPointY));
	//DecodeSet(DEC_ALD_SPACING,				(void *)GET_SETTING(AldSpacing));
	//DecodeSet(DEC_ALD_ATTEMPTS,				(void *)GET_SETTING(AldAttempts));
	//DecodeSet(DEC_ALD_VOTE,					(void *)GET_SETTING(AldVote));
	DecodeSet(DEC_ADAPTIVE_DECODE_ENABLED,	(void *)GET_SETTING(AutoAdaptive));
	DecodeSet(DEC_PRIORITY_SYMBOLOGIES,		(void *)GET_SETTING(PriorityCodeIds));
	DecodeSet(DEC_REFLECTIONS_ENABLED,		(void *)GET_SETTING(DecodeReflections));
	DecodeSet(DEC_ZOOM,						(void *)GET_SETTING(DecodeZoom));

	DecodeSet(DEC_PRINT_QUAL_ASSESS_MODE,	(void *)GET_SETTING(DDDiag));
	DecodeSet(DEC_SUDOKU_ENABLED,			(void *)GET_SETTING(SudokuEnabled));

	DecodeSet(DEC_AUS_POST_QUAL_ASSESS_MODE,	(void *)GET_SETTING(CBQAMode));

	DecodeSet(DEC_LABEL_CODE_ENABLED,		(void *)GET_SETTING(LabelCodeEnable));	// Labelcode (Follett) enabling

	if (DecodeSet(DEC_POSICODE_ENABLED,			(void *)GET_SETTING(posiEnable))) {	// PosiCode enabling & options
		DecodeSet(DEC_POSICODE_MIN_LENGTH,		(void *)GET_SETTING(posiMinLength));
		DecodeSet(DEC_POSICODE_MAX_LENGTH,		(void *)GET_SETTING(posiMaxLength));
		DecodeSet(DEC_POSICODE_LIMITED_MODE,	(void *)GET_SETTING(posiLimited));	// '1' = A, '2' = B
	}

	if (DecodeSet(DEC_CODE16_ENABLED,		(void *)GET_SETTING(c16KEnable))) {	// Code 16K enabling & options
		DecodeSet(DEC_CODE16_MIN_LENGTH,	(void *)GET_SETTING(c16KMinLength));
		DecodeSet(DEC_CODE16_MAX_LENGTH,	(void *)GET_SETTING(c16KMaxLength));
	}

	if (DecodeSet(DEC_CODE49_ENABLED,		(void *)GET_SETTING(c49Enable))) {	// Code 49 enabling & options
		DecodeSet(DEC_CODE49_MIN_LENGTH,	(void *)GET_SETTING(c49MinLength));
		DecodeSet(DEC_CODE49_MAX_LENGTH,	(void *)GET_SETTING(c49MaxLength));
	}

	DecodeSet(DEC_PDF417_CODEWORD_TICK,		(void *)GET_SETTING(PDFTicks));

	DecodeSet(DEC_AZTEC_RUNES_ENABLED,			(void *)GET_SETTING(aztRunes));  // Aztec Mesas & Runes
	DecodeSet(DEC_AZTEC_MESA_CODE128_ENABLED,	(void *)GET_SETTING(c128MesaEnable));
	DecodeSet(DEC_AZTEC_MESA_CODE39_ENABLED,	(void *)GET_SETTING(c39MesaEnable));
	DecodeSet(DEC_AZTEC_MESA_CODE93_ENABLED,	(void *)GET_SETTING(c93MesaEnable));
	DecodeSet(DEC_AZTEC_MESA_UPCA_ENABLED,		(void *)GET_SETTING(upcaMesaEnable));
	DecodeSet(DEC_AZTEC_MESA_EAN13_ENABLED,		(void *)GET_SETTING(ean13MesaEnable));
	DecodeSet(DEC_AZTEC_MESA_I25_ENABLED,		(void *)GET_SETTING(i25MesaEnable));

	DecodeSet(DEC_MAXICODE_REF_TARGET_ENABLED,	(void *)GET_SETTING(refTargetEnable));	// Area Ref Target

	if (DecodeSet(DEC_GM_ENABLED,		(void *)GET_SETTING(gmxEnable))) {	// GMX (Grid Matrix) Code
		DecodeSet(DEC_GM_MIN_LENGTH,	(void *)GET_SETTING(gmxMinLength));
		DecodeSet(DEC_GM_MAX_LENGTH,	(void *)GET_SETTING(gmxMaxLength));
	}

	if (DecodeSet(DEC_VERICODE_ENABLED,		(void *)GET_SETTING(verEnable))) {	// Vericode
		DecodeSet(DEC_VERICODE_MIN_LENGTH,	(void *)GET_SETTING(verMinLength));
		DecodeSet(DEC_VERICODE_MAX_LENGTH,	(void *)GET_SETTING(verMaxLength));
		DecodeSet(DEC_VERICODE_SIZE_A,		(void *)GET_SETTING(verSizeA));
		DecodeSet(DEC_VERICODE_SIZE_B,		(void *)GET_SETTING(verSizeB));
		DecodeSet(DEC_VERICODE_SIZE_C,		(void *)GET_SETTING(verSizeC));
		DecodeSet(DEC_VERICODE_SIZE_D,		(void *)GET_SETTING(verSizeD));
		DecodeSet(DEC_VERICODE_SIZE_E,		(void *)GET_SETTING(verSizeE));
	}

	if( DecodeSet(DEC_BNB_ENABLED,		(void *)GET_SETTING(BarNoBarEnable))) {
		DecodeSet(DEC_BNB_HALF_HOUR_FORMAT,		(void *)GET_SETTING(BarNoBarHhrFormat));
		DecodeSet(DEC_BNB_MACHINE_ID_FORMAT,		(void *)GET_SETTING(BarNoBarHexFormat));
	}

	DecodeSet(DEC_OCR_GROUP_G,		(void *)GET_SETTING(ocrGroupg));
	DecodeSet(DEC_OCR_GROUP_H,		(void *)GET_SETTING(ocrGrouph));
	DecodeSet(DEC_OCR_CHECK_SET,	(void *)GET_SETTING(ocrCheckSet));
	
	if (DecodeSet(DEC_BC412_ENABLED,		(void *)GET_SETTING(bc412Enable))) {	// BC412 enabling & options
		DecodeSet(DEC_BC412_MIN_LENGTH,		(void *)GET_SETTING(bc412MinLength));
		DecodeSet(DEC_BC412_MAX_LENGTH,		(void *)GET_SETTING(bc412MaxLength));
	}

	if( DecodeSet(DEC_CODEZ_ENABLED,			(void *)GET_SETTING(codezEnable)) ) {	// Code Z enabling & options
		DecodeSet(DEC_CODEZ_MIN_LENGTH,			(void *)GET_SETTING(codezMinLength));
		DecodeSet(DEC_CODEZ_MAX_LENGTH,			(void *)GET_SETTING(codezMaxLength));
	}

	if (DecodeSet(DEC_SECURE_ENABLED,			(void *)GET_SETTING(SecureCodeEnable))) {  // Secure Code
		DecodeSet(DEC_SECURE_MIN_LENGTH,		(void *)GET_SETTING(SecureCodeMinLength));
		DecodeSet(DEC_SECURE_MAX_LENGTH,		(void *)GET_SETTING(SecureCodeMaxLength));
		DecodeSet(DEC_SECURE_APPLICATION_KEY,	(void *)GET_SETTING(SecureCodeKey));
		DecodeSet(DEC_SECURE_VENDOR_KEY,		(void *)GET_CONFIG_SETTING(SecureCodeVendorKey));
	}

	DecodeSet(DEC_DPC_CIRRUS_ENABLED,		(void *)GET_SETTING(DpcCirrusEnable));	// Code DPC Cirrus enabling & options

	if (DecodeSet(DEC_PLESSEY_ENABLED,				(void *)GET_SETTING(PlsyEnable))) {	// Plessey Code enabling & options
		DecodeSet(DEC_PLESSEY_MIN_LENGTH,			(void *)GET_SETTING(PlsyMinLength));
		DecodeSet(DEC_PLESSEY_MAX_LENGTH,			(void *)GET_SETTING(PlsyMaxLength));
		DecodeSet(DEC_PLESSEY_CHECK_DIGIT_ENABLED,	(void *)GET_SETTING(PlsyChkChar));
	}

}
#endif //DECODER_AD

bool FillDecoderMenu(void) {

	DecodeSet(DEC_DECODE_VIGOR,				(void *)GET_SETTING(DecodeVigor));
	DecodeSet(DEC_SHOW_DECODE_WINDOW,		(void *)GET_SETTING(ShowDecodeWindow));

	DecodeSet(DEC_IQ_FILTER_MODE,			(void *)(selectedImageQualityMode));
	DecodeSet(DEC_VIDEO_REVERSE_ENABLED,	(void *)GET_SETTING(VideoReverse));
	DecodeSet(DEC_SHOW_NO_READ_ENABLED,		(void *)GET_SETTING(ShowNoRead));
	DecodeSet(DEC_SHOW_DECODE_DEBUG,		(void *)GET_SETTING(DecoderExtraInfo));
	DecodeSet(DEC_IMAGE_MIRRORED,			(void *)GET_SETTING(UndoMirroredImage));
	DecodeSet(DEC_IMAGE_ROTATION,				(void *)GET_SETTING(iEngineRotation));

	DecodeSet(DEC_USE_MLD,				(void *)GET_SETTING(DecodeMLD));

	DecodeSet(DEC_DECODE_TIMING_CONTROL,	(void *)GET_SETTING(DecodeControl));
	DecodeSet(DEC_DECODE_TIME,				(void *)GET_SETTING(DecodeMinTimeout));
	DecodeSet(DEC_SEARCH_TIME,				(void *)GET_SETTING(DecodeMaxTimeout));
	DecodeSet(DEC_ADD_SEARCH_TIME_ADDENDA,			(void *)GET_SETTING(DecodeAddendaTimeout));
	DecodeSet(DEC_ADD_SEARCH_TIME_CONCAT,			(void *)GET_SETTING(DecodeConCatTimeout));
	DecodeSet(DEC_ADD_SEARCH_TIME_UPC_COMPOSITE,	(void *)GET_SETTING(DecodeUPC_CompTimeout));

	DecodeSet(DEC_PRINT_RESULTS,			(void *)GET_SETTING(PrintResults));
	DecodeSet(DEC_SECURITY_LEVEL,			(void *)GET_SETTING(DecodeSecurityLevel));
	
	if(GET_SETTING(DecodeMonocolorForceInterpolation) == 2){		// Follow Engine ID or Configuration
		if(HardwareToken.IAmOEMJade()){
			// Determine imager type by EngineID
			PSoC_5100_REGISTER_TABLE	Registers;
			int ImagerType = 0;
			Psoc5100GetRegisters( &Registers );
				ImagerType = Registers.EngineID.b.Imager;
			DecodeSet (DEC_MONOCOLOR_ENABLED, (void *)(ImagerType));
		} else{
			if(GET_CONFIG_SETTING(ImageSensor) == (E2V_EV76C454_Jade_Monocolor))
				DecodeSet (DEC_MONOCOLOR_ENABLED, (void *)(1));
			else
				DecodeSet (DEC_MONOCOLOR_ENABLED, (void *)(0));
		}
	}
	else														// Force interpolation on = 1/off = 0
		DecodeSet (DEC_MONOCOLOR_ENABLED, (void*)(GET_SETTING(DecodeMonocolorForceInterpolation)));

	DecodeSet(DEC_COUPON_CODE_MODE,			(void *)GET_SETTING(CouponCodes));	// UPC & EAN Coupon Code enabling
	DecodeSet(DEC_COUPON_SECONDARY_OUTPUT,	(void *)GET_SETTING(CouponSecondaryOutput));
	DecodeSet(DEC_EANUCC_EMULATION_MODE,	(void *)GET_SETTING(EanUccEmulation));	// EAN.UCC Data Format Emulation

	if (DecodeSet(DEC_UPCA_ENABLED,					(void *)GET_SETTING(UPCAEnable))) {	// UPC-A enabling & options
		DecodeSet(DEC_UPCA_CHECK_DIGIT_TRANSMIT,	(void *)GET_SETTING(UPCAChkXmit));
		DecodeSet(DEC_UPCA_NUMBER_SYSTEM_TRANSMIT,	(void *)GET_SETTING(UPCANumXmit));
		DecodeSet(DEC_UPCA_2CHAR_ADDENDA_ENABLED,	(void *)GET_SETTING(UPCAAddenda2));
		DecodeSet(DEC_UPCA_5CHAR_ADDENDA_ENABLED,	(void *)GET_SETTING(UPCAAddenda5));
		DecodeSet(DEC_UPCA_ADDENDA_REQUIRED,		(void *)GET_SETTING(UPCAAddendaReq));
		DecodeSet(DEC_UPCA_ADDENDA_SEPARATOR,		(void *)GET_SETTING(UPCAAddendaSep));
	}

	if (DecodeSet(DEC_UPCE0_ENABLED,				(void *)GET_SETTING(UPCE0Enable))) {	// UPC-E enabling & options
		DecodeSet(DEC_UPCE1_ENABLED,				(void *)GET_SETTING(UPCE1Enable));
		DecodeSet(DEC_UPCE_EXPAND,					(void *)GET_SETTING(UPCEExpand));
		DecodeSet(DEC_UPCE_CHECK_DIGIT_TRANSMIT,	(void *)GET_SETTING(UPCEChkXmit));
		DecodeSet(DEC_UPCE_NUMBER_SYSTEM_TRANSMIT,	(void *)GET_SETTING(UPCENumXmit));
		DecodeSet(DEC_UPCE_2CHAR_ADDENDA_ENABLED,	(void *)GET_SETTING(UPCEAddenda2));
		DecodeSet(DEC_UPCE_5CHAR_ADDENDA_ENABLED,	(void *)GET_SETTING(UPCEAddenda5));
		DecodeSet(DEC_UPCE_ADDENDA_REQUIRED,		(void *)GET_SETTING(UPCEAddendaReq));
		DecodeSet(DEC_UPCE_ADDENDA_SEPARATOR,		(void *)GET_SETTING(UPCEAddendaSep));
	}

	if (DecodeSet(DEC_EAN8_ENABLED,					(void *)GET_SETTING(EAN8Enable))) {	// EAN-8 enabling & options
		DecodeSet(DEC_EAN8_CHECK_DIGIT_TRANSMIT,	(void *)GET_SETTING(EAN8ChkXmit));
		DecodeSet(DEC_EAN8_2CHAR_ADDENDA_ENABLED,	(void *)GET_SETTING(EAN8Addenda2));
		DecodeSet(DEC_EAN8_5CHAR_ADDENDA_ENABLED,	(void *)GET_SETTING(EAN8Addenda5));
		DecodeSet(DEC_EAN8_ADDENDA_REQUIRED,		(void *)GET_SETTING(EAN8AddendaReq));
		DecodeSet(DEC_EAN8_ADDENDA_SEPARATOR,		(void *)GET_SETTING(EAN8AddendaSep));
	}

	if (DecodeSet(DEC_EAN13_ENABLED,				(void *)GET_SETTING(EAN13Enable))) {	// EAN-13 enabling & options
		DecodeSet(DEC_EAN13_CHECK_DIGIT_TRANSMIT,	(void *)GET_SETTING(EAN13ChkXmit));
		DecodeSet(DEC_EAN13_2CHAR_ADDENDA_ENABLED,	(void *)GET_SETTING(EAN13Addenda2));
		DecodeSet(DEC_EAN13_5CHAR_ADDENDA_ENABLED,	(void *)GET_SETTING(EAN13Addenda5));
		DecodeSet(DEC_EAN13_ADDENDA_REQUIRED,		(void *)GET_SETTING(EAN13AddendaReq));
		DecodeSet(DEC_EAN13_ADDENDA_SEPARATOR,		(void *)GET_SETTING(EAN13AddendaSep));
		DecodeSet(DEC_EAN13_ISBN_ENABLED,			(void *)GET_SETTING(EAN13ISBN));
	}

	if (DecodeSet(DEC_CODE128_ENABLED,		(void *)GET_SETTING(C128Enable))) {	// Code 128 enabling & options
		DecodeSet(DEC_CODE128_MIN_LENGTH,	(void *)GET_SETTING(C128MinLength));
		DecodeSet(DEC_CODE128_MAX_LENGTH,	(void *)GET_SETTING(C128MaxLength));
		DecodeSet(DEC_CODE128_APPEND_ENABLED, (void *)GET_SETTING(C128AppendEnable));
	}

	if (DecodeSet(DEC_GS1_128_ENABLED,		(void *)GET_SETTING(G128Enable))) {  // Code GS1_128 enabling & options
		DecodeSet(DEC_GS1_128_MIN_LENGTH,	(void *)GET_SETTING(G128MinLength));
		DecodeSet(DEC_GS1_128_MAX_LENGTH,	(void *)GET_SETTING(G128MaxLength));
	}

	DecodeSet(DEC_C128_ISBT_ENABLED,	(void *)GET_SETTING(C128ISBTEnabled));
	DecodeSet(DEC_C128_FNC1_SUBSTITUTE,	(void *)GET_SETTING(C128CharSubFunc1)); //was not configurable with adaptus
	DecodeSet(DEC_C128_FNC_TRANSMIT,	(void *)GET_SETTING(C128ShowFNCs));

	if (DecodeSet(DEC_CODE39_ENABLED,				(void *)GET_SETTING(C39Enable))) {	// Code 39 enabling & options
		DecodeSet(DEC_CODE39_MIN_LENGTH,			(void *)GET_SETTING(C39MinLength));
		DecodeSet(DEC_CODE39_MAX_LENGTH,			(void *)GET_SETTING(C39MaxLength));
		switch(GET_SETTING(C39ChkChar)) {
			case 1:		DecodeSet(DEC_CODE39_CHECK_DIGIT_MODE, (void *)DEC_STRIPPED ); break;
			case 2:		DecodeSet(DEC_CODE39_CHECK_DIGIT_MODE, (void *)DEC_ENABLED ); break;
			default:	DecodeSet(DEC_CODE39_CHECK_DIGIT_MODE, (void *)DEC_DISABLED ); break;
		}
		DecodeSet(DEC_CODE39_APPEND_ENABLED,		(void *)GET_SETTING(C39Append));
		DecodeSet(DEC_CODE39_FULL_ASCII_ENABLED,	(void *)GET_SETTING(C39FullAscii));
		DecodeSet(DEC_CODE39_START_STOP_TRANSMIT,	(void *)GET_SETTING(C39SSXmit));
		DecodeSet(DEC_CODE39_BASE32_ENABLED,		(void *)GET_SETTING(C39Base32));
	}

	DecodeSet(DEC_TLC39_ENABLED,		(void *)(GET_SETTING(tlc39Enable)));		// TLC39 enabling

	DecodeSet(DEC_TRIOPTIC_ENABLED,			(void *)GET_SETTING(triopticEnable));	// TriOptic Code enabling

	if (DecodeSet(DEC_I25_ENABLED,					(void *)GET_SETTING(I25Enable))) {	// Interleaved 2/5 enabling & options
		DecodeSet(DEC_I25_MIN_LENGTH,				(void *)GET_SETTING(I25MinLength));
		DecodeSet(DEC_I25_MAX_LENGTH,				(void *)GET_SETTING(I25MaxLength));
		switch(GET_SETTING(I25ChkChar)) {
			case 1:		DecodeSet(DEC_I25_CHECK_DIGIT_MODE, (void *)DEC_STRIPPED ); break;
			case 2:		DecodeSet(DEC_I25_CHECK_DIGIT_MODE, (void *)DEC_ENABLED ); break;
			case 3:		DecodeSet(DEC_I25_CHECK_DIGIT_MODE, (void *)4 ); break;
			case 4:		DecodeSet(DEC_I25_CHECK_DIGIT_MODE, (void *)3 ); break;
			default:	DecodeSet(DEC_I25_CHECK_DIGIT_MODE, (void *)DEC_DISABLED ); break;
		}
	}

	if (DecodeSet(DEC_S25_ENABLED,		(void *)GET_SETTING(R25Enable))) {	// Straight 2/5 (3-bar S/S) enabling & options
		DecodeSet(DEC_S25_MIN_LENGTH,	(void *)GET_SETTING(R25MinLength));
		DecodeSet(DEC_S25_MAX_LENGTH,	(void *)GET_SETTING(R25MaxLength));
	}

	if (DecodeSet(DEC_IATA25_ENABLED,		(void *)GET_SETTING(A25Enable))) { // IATA 2/5 (2-bar S/s) enabling & options
		DecodeSet(DEC_IATA25_MIN_LENGTH,	(void *)GET_SETTING(A25MinLength));
		DecodeSet(DEC_IATA25_MAX_LENGTH,	(void *)GET_SETTING(A25MaxLength));
	}

	if (DecodeSet(DEC_M25_ENABLED,		(void *)GET_SETTING(X25Enable))) {	// Matrix 2/5 enabling & options
		DecodeSet(DEC_M25_MIN_LENGTH,	(void *)GET_SETTING(X25MinLength));
		DecodeSet(DEC_M25_MAX_LENGTH,	(void *)GET_SETTING(X25MaxLength));
	}

	if (DecodeSet(DEC_CODE93_ENABLED,		(void *)GET_SETTING(C93Enable))) {	// Code 93 enabling & options
		DecodeSet(DEC_CODE93_MIN_LENGTH,	(void *)GET_SETTING(C93MinLength));
		DecodeSet(DEC_CODE93_MAX_LENGTH,	(void *)GET_SETTING(C93MaxLength));
		DecodeSet(DEC_CODE93_APPEND_ENABLED, (void *)GET_SETTING(C93AppendEnable));
	}

	if (DecodeSet(DEC_CODE11_ENABLED,			(void *)GET_SETTING(C11Enable))) {	// Code 11 enabling & options
		DecodeSet(DEC_CODE11_MIN_LENGTH,		(void *)GET_SETTING(C11MinLength));
		DecodeSet(DEC_CODE11_MAX_LENGTH,		(void *)GET_SETTING(C11MaxLength));
		int tempC11check = !GET_SETTING(C11DoubleCheck);
		tempC11check += 2;
		DecodeSet(DEC_CODE11_CHECK_DIGIT_MODE,	(void *)tempC11check);
	}

	if (DecodeSet(DEC_CODABAR_ENABLED,				(void *)GET_SETTING(CbrEnable))) {	// Codabar enabling & options
		DecodeSet(DEC_CODABAR_MIN_LENGTH,			(void *)GET_SETTING(CbrMinLength));
		DecodeSet(DEC_CODABAR_MAX_LENGTH,			(void *)GET_SETTING(CbrMaxLength));
		DecodeSet(DEC_CODABAR_START_STOP_TRANSMIT,	(void *)GET_SETTING(CbrSSXmit));
		switch(GET_SETTING(CbrChkChar)) {
			case 1:		DecodeSet(DEC_CODABAR_CHECK_DIGIT_MODE, (void *)DEC_STRIPPED ); break;
			case 2:		DecodeSet(DEC_CODABAR_CHECK_DIGIT_MODE, (void *)DEC_ENABLED ); break;
			default:	DecodeSet(DEC_CODABAR_CHECK_DIGIT_MODE, (void *)DEC_DISABLED ); break;
		}
		DecodeSet(DEC_CODABAR_CONCAT_ENABLED,		(void *)GET_SETTING(CbrConCat));
	}

	if (DecodeSet(DEC_TELEPEN_ENABLED,		(void *)GET_SETTING(TELEnable))) {	// Telepen enabling & options
		DecodeSet(DEC_TELEPEN_MIN_LENGTH,	(void *)GET_SETTING(TELMinLength));
		DecodeSet(DEC_TELEPEN_MAX_LENGTH,	(void *)GET_SETTING(TELMaxLength));
		DecodeSet(DEC_TELEPEN_OLD_STYLE,	(void *)GET_SETTING(TELOldStyle));
	}

	if (DecodeSet(DEC_MSI_ENABLED,				(void *)GET_SETTING(MSIEnable))) {	// MSI Code enabling & options
		DecodeSet(DEC_MSI_MIN_LENGTH,			(void *)GET_SETTING(MSIMinLength));
		DecodeSet(DEC_MSI_MAX_LENGTH,			(void *)GET_SETTING(MSIMaxLength));
		switch(GET_SETTING(MSIChkChar)) {
			case 0:	DecodeSet(DEC_MSI_CHECK_DIGIT_MODE, (void *)5); break;
			case 1:	DecodeSet(DEC_MSI_CHECK_DIGIT_MODE, (void *)1); break;
			case 2:	DecodeSet(DEC_MSI_CHECK_DIGIT_MODE, (void *)7); break;
			case 3:	DecodeSet(DEC_MSI_CHECK_DIGIT_MODE, (void *)3); break;
			case 4:	DecodeSet(DEC_MSI_CHECK_DIGIT_MODE, (void *)6); break;
			case 5:	DecodeSet(DEC_MSI_CHECK_DIGIT_MODE, (void *)2); break;
			default: DecodeSet(DEC_MSI_CHECK_DIGIT_MODE, (void *)DEC_DISABLED ); break;
		}
	}

	if (DecodeSet(DEC_RSS_14_ENABLED,			(void *)GET_SETTING(RSS14Enable)) ){	// RSS-14 enabling
		DecodeSet(DEC_RSS_LIMITED_ENABLED,		(void *)GET_SETTING(RSSLimEnable));	// RSS Limited enabling
		DecodeSet(DEC_RSS_EXPANDED_ENABLED,		(void *)GET_SETTING(RSSExEnable));	// RSS Expanded enabling & options
		DecodeSet(DEC_RSS_EXPANDED_MIN_LENGTH,	(void *)GET_SETTING(RSSExMinLength));
		DecodeSet(DEC_RSS_EXPANDED_MAX_LENGTH,	(void *)GET_SETTING(RSSExMaxLength));
	}
	
   	if(DecodeSet(DEC_CODABLOCK_A_ENABLED,	(void *)GET_SETTING(cblkaEnable) )){
	   	DecodeSet(DEC_CODABLOCK_A_MIN_LENGTH,	(void *)GET_SETTING(cblkaMinLength) );
	   	DecodeSet(DEC_CODABLOCK_A_MAX_LENGTH,	(void *)GET_SETTING(cblkaMaxLength) );
   	}

	if (DecodeSet(DEC_CODABLOCK_F_ENABLED,			(void *)GET_SETTING(cblkfEnable))) {	// Codablock-F enabling & options
		DecodeSet(DEC_CODABLOCK_F_MIN_LENGTH,		(void *)GET_SETTING(cblkfMinLength));
		DecodeSet(DEC_CODABLOCK_F_MAX_LENGTH,		(void *)GET_SETTING(cblkfMaxLength));
	}

	if (DecodeSet(DEC_PDF417_ENABLED,		(void *)GET_SETTING(PDFEnable))) {	// PDF417 enabling & options
		DecodeSet(DEC_PDF417_MIN_LENGTH,	(void *)GET_SETTING(PDFMinLength));
		DecodeSet(DEC_PDF417_MAX_LENGTH,	(void *)GET_SETTING(PDFMaxLength));
		DecodeSet(DEC_PDF417_SHOW_GLI,		(void *)GET_SETTING(PDFShowGLI));
		DecodeSet(DEC_PDF417_MACRO_ENABLED,	(void *)GET_SETTING(PDFMacroEnabled));
		//DecodeSet(DEC_PDF417_SHOW_MACRO,	(void *)GET_SETTING(PDFShowMacro));
	}

	if (DecodeSet(DEC_MICROPDF_ENABLED,		(void *)GET_SETTING(MPDEnable))) {	// Micro PDF417 enabling & options
		DecodeSet(DEC_MICROPDF_MIN_LENGTH,	(void *)GET_SETTING(MPDMinLength));
		DecodeSet(DEC_MICROPDF_MAX_LENGTH,	(void *)GET_SETTING(MPDMaxLength));
	}

	if (DecodeSet(DEC_COMPOSITE_ENABLED,			(void *)GET_SETTING(CompositeEnable))) {	// Composites enabling & options
		DecodeSet(DEC_COMPOSITE_MIN_LENGTH,			(void *)GET_SETTING(CompMinLength));
		DecodeSet(DEC_COMPOSITE_MAX_LENGTH,			(void *)GET_SETTING(CompMaxLength));
		DecodeSet(DEC_COMPOSITE_WITH_UPC_ENABLED,	(void *)GET_SETTING(CompsOnUpcEan));
	}


	if (DecodeSet(DEC_AZTEC_ENABLED,		(void *)((GET_SETTING(aztEnable))?3:0))) {  // Aztec Code
		DecodeSet(DEC_AZTEC_MIN_LENGTH,		(void *)GET_SETTING(aztMinLength));
		DecodeSet(DEC_AZTEC_MAX_LENGTH,		(void *)GET_SETTING(aztMaxLength));
		DecodeSet(DEC_AZTEC_APPEND_ENABLED, (void *)GET_SETTING(aztAppendEnable));
		DecodeSet(DEC_AZTEC_APPEND_STRIP_INFO	, (void *)GET_SETTING(aztAppendStripInfo));
	}

	if (DecodeSet(DEC_MAXICODE_ENABLED,		(void *)((GET_SETTING(maxEnable))?0x3F:0))) {	// Maxicode
		DecodeSet(DEC_MAXICODE_MIN_LENGTH,	(void *)GET_SETTING(maxMinLength));
		DecodeSet(DEC_MAXICODE_MAX_LENGTH,	(void *)GET_SETTING(maxMaxLength));
	}

	if (DecodeSet(DEC_DATAMATRIX_ENABLED,		(void *)((GET_SETTING(idmEnable))?3:0))) {	// Data Matrix
		DecodeSet(DEC_DATAMATRIX_MIN_LENGTH,	(void *)GET_SETTING(idmMinLength));
		DecodeSet(DEC_DATAMATRIX_MAX_LENGTH,	(void *)GET_SETTING(idmMaxLength));
		//DecodeSet(DEC_DATAMATRIX_APPEND_ENABLED, (void *)GET_SETTING(idmAppendEnable));
	}

	if (DecodeSet(DEC_QR_ENABLED,		(void *)((GET_SETTING(qrEnable))?15:0))) {	// QR Code
		DecodeSet(DEC_QR_MIN_LENGTH,	(void *)GET_SETTING(qrMinLength));
		DecodeSet(DEC_QR_MAX_LENGTH,	(void *)GET_SETTING(qrMaxLength));
		DecodeSet(DEC_QR_APPEND_ENABLED, (void *)GET_SETTING(qrcAppendEnable));
	}

	if (DecodeSet(DEC_HANXIN_ENABLED,		(void *)GET_SETTING(hxEnable))) {	// Han Xin Code
		DecodeSet(DEC_HANXIN_MIN_LENGTH,	(void *)GET_SETTING(hxMinLength));
		DecodeSet(DEC_HANXIN_MAX_LENGTH,	(void *)GET_SETTING(hxMaxLength));
	}

	if (DecodeSet(DEC_HK25_ENABLED,		(void *)GET_SETTING(ChinaEnable))) {	// HongKong 2of5 - aka China Post
		DecodeSet(DEC_HK25_MIN_LENGTH,	(void *)GET_SETTING(ChinaMinLength));
		DecodeSet(DEC_HK25_MAX_LENGTH,	(void *)GET_SETTING(ChinaMaxLength));
	}

	if (DecodeSet(DEC_OCR_MODE,			(void *)GET_SETTING(ocrEnable))) {	// OCR enabling & options
		//DecodeSet(DEC_OCR_TEMPLATE,		(void *)GET_SETTING(ocrTemplate)); template gets set in menu command entry function
		DecodeSet(DEC_OCR_TEMPLATE,(void *)parsedOCRTemplate);//ewr04923
	}

	if (DecodeSet(DEC_KOREA_POST_ENABLED,		(void *)GET_SETTING(koreanPostEnable))) {	// Korea Post 3/5 enabling & options
		DecodeSet(DEC_KOREA_POST_MIN_LENGTH,	(void *)GET_SETTING(koreanPostMinLength));
		DecodeSet(DEC_KOREA_POST_MAX_LENGTH,	(void *)GET_SETTING(koreanPostMaxLength));
	   	DecodeSet(DEC_KOREA_POST_CHECK_DIGIT_TRANSMIT,(void *)GET_SETTING(koreanPostCheckTransmit));
	   	DecodeSet(DEC_KOREA_POST_REVERSE,		(void *)GET_SETTING(koreanPostReverse));
	}

	DecodeSet(DEC_POSTAL_ENABLED,					(void *)GET_SETTING(postalEnable));
	DecodeSet(DEC_POSTNET_CHECK_DIGIT_TRANSMIT,		(void *)GET_SETTING(netChkXmit));
	DecodeSet(DEC_PLANETCODE_CHECK_DIGIT_TRANSMIT,	(void *)GET_SETTING(plnChkXmit));
	
	DecodeSet(DEC_AUS_POST_BAR_OUTPUT_ENABLED,		(void *)GET_SETTING(ausBarOutput));
	DecodeSet(DEC_AUS_POST_INTERPRET_MODE,	(void *)GET_SETTING(ausCustIntrpt));
	DecodeSet(DEC_AUS_POST_ZERO_FCC,		(void *)GET_SETTING(ausFCCenable));

	DecodeSet(DEC_CAN_POST_BAR_OUTPUT,		(void *)(GET_SETTING(canEnable)==2));

	DecodeSet(DEC_ROYAL_MAIL_FORMAT_CHECK_MIN_LENGTH, (void *)GET_SETTING(royalMailMinLengthNoFormatCheck));

	int disGrp = 0;
	if( GET_SETTING(skipLinear) ) {
		disGrp |= ONE_DIMENSIONAL_CODES;
		disGrp |= STACKED_LINEAR_CODES;
	}
	DecodeSet( DEC_DISABLE_SYMBOLOGY_GROUP, (void *)disGrp );
	
	if( HardwareToken.IAmOEM() && ImageEngineHasPsoc() ) {
		DWORD	GetImagerPropertiesRequest[ 4 ] = { IP_AIMER_X_OFFSET, 0, IP_AIMER_Y_OFFSET, 0 };

		SCN_IOControl( nOpenCntxt, HHPSD_IOCTL_GET_IMAGER_PROPERTIES, (PBYTE) GetImagerPropertiesRequest, sizeof( GetImagerPropertiesRequest ),
			(PBYTE) GetImagerPropertiesRequest, sizeof( GetImagerPropertiesRequest ), NULL ); 
			
		SetImageParameters(nHeight,nWidth,nWidth,GetImagerPropertiesRequest[1],GetImagerPropertiesRequest[3]);
	}
	else {
		SetImageParameters(nHeight,nWidth,nWidth,GET_CONFIG_SETTING(iImageXCenter),GET_CONFIG_SETTING(iImageYCenter));
	}

#if (DECODER_ID)
	FillSwiftDecoderMenu();
#endif
#if (DECODER_AD)
	FillAdaptusDecoderMenu();
#endif


	return true;

}

//ewr04791 Codegate
// FillDecoderWindowCenteringMenu(): Sets Window Centering settings if in/out of stand and TRGSSW1 enabled.
void FillDecoderWindowCenteringMenu(void)
{
	if( pTrigger !=NULL && pTrigger->IsPresentationTypeTrigger() )
	{
		DecodeSet(DEC_WINDOW_MODE,				(void *)GET_SETTING(PresentationDecodeInWindow));
		DecodeSet(DEC_WINDOW_TOP,				(void *)GET_SETTING(PresentationWindowTopY));
		DecodeSet(DEC_WINDOW_BOTTOM,			(void *)GET_SETTING(PresentationWindowBotY));
		DecodeSet(DEC_WINDOW_LEFT,				(void *)GET_SETTING(PresentationWindowTopX));
		DecodeSet(DEC_WINDOW_RIGHT,				(void *)GET_SETTING(PresentationWindowBotX));
	}
	else if( pTrigger !=NULL && pTrigger->GetCodeGateMode() )
	{
		DecodeSet(DEC_WINDOW_MODE,				(void *)1);
		DecodeSet(DEC_WINDOW_TOP,				(void *)47);
		DecodeSet(DEC_WINDOW_BOTTOM,			(void *)53);
		DecodeSet(DEC_WINDOW_LEFT,				(void *)40);
		DecodeSet(DEC_WINDOW_RIGHT,				(void *)60);
	}
	else
	{
		DecodeSet(DEC_WINDOW_MODE,				(void *)GET_SETTING(DecodeInWindow));
		DecodeSet(DEC_WINDOW_TOP,				(void *)GET_SETTING(WindowTopY));
		DecodeSet(DEC_WINDOW_BOTTOM,			(void *)GET_SETTING(WindowBotY));
		DecodeSet(DEC_WINDOW_LEFT,				(void *)GET_SETTING(WindowTopX));
		DecodeSet(DEC_WINDOW_RIGHT,				(void *)GET_SETTING(WindowBotX));
	}

}

void OP_SetUp (int Width, int Height)
{
	Decoder_Handle = InitDecoder(Width,Height,Width);
	InstallCallback( CB_RESULT,		(void *) ProcessResult);
	InstallCallback( CB_PROGRESS,	(void *) SD_CB_Progress);
	InstallCallback( CB_TIMER,		(void *) MsTimer);
	InstallCallback( CB_STATUS,		(void *) SD_CB_Status);
	InstallCallback( CB_RESULT_NOTIFY,		(void *) SD_CB_ResultNotify);
}


void OPdecode (unsigned char *img, int wid, int hgt, int wrap, int SearchX, int SearchY, int alg)
{
	bSD_DecodeInProgress = 0;
	bSD_DataMatrixTimedSearch = 0;
	bSD_OCRTimedSearch = 0;

	START_DECODER; // always clear bHalted!!
	
	if( GET_SETTING(ImageROIEnabled) ) {
		SetImageParameters(hgt, wid, wrap, SearchX, SearchY); 
	}

	if( alg ) {
		Decode((void *)img,alg);
	}
	else {
		int delayCount = 0;
		while(ProcessedResult == 0 && delayCount++ < 50 ) {
			MT_Delay(DecTmn/50); //keep the frequency check high so there isn't too much delay from a fast-only read to beep.
		}
	}
}

void *EstablishDecoder(int Width, int Height)
{
	DecoderWidth = Width;
	DecoderHeight = Height;
	
	OP_SetUp(DecoderWidth, DecoderHeight);

	return 0;
}

// Initialize image structure install decoder callbacks establish decoder workspace and fill menu
// for 1st time
void DecoderPowerupInit	(void)
{
	if (DecodeResults == NULL)
		DecodeResults=new CBarcodeFifo(100);

	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&nHeight,4,NULL);
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&nWidth,4,NULL);
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_ENGINE_TYPE,NULL,0,(PBYTE)&nEngineType,0,(PDWORD)&nSize);

	if (pImg)
		FREE(pImg);
	
	pImg = MALLOC(sizeof(GrayscaleImage2));

	// spd ImgBox command
	PowerOnInitLastDecodeInfo();

	if (InProcessResult != NULL)			// when using mutexs this means we created one so give it back
	{
		MT_CloseMutex(InProcessResult);
	}

	InProcessResult=MT_CreateMutex(false);			// create new mutex not claimed
	
	Decoder = EstablishDecoder(nWidth,nHeight);
	
	FillDecoderMenu();
}

// Magic numbers for first color pixel (FIXME: ScanDriver should supply!) and spacing established here for monocolor handling below 
#define FIRST_COLOR_PIXEL_X 2
#define FIRST_COLOR_PIXEL_Y 2
#define COLOR_PIXEL_SPACING_X 4
#define COLOR_PIXEL_SPACING_Y 4

/************************************************************************/
void SetImageParameters(int hgt, int wid, int wrap, int cenX, int cenY )
{
	int mtop = GET_SETTING(WindowTopY);
	int mbot = GET_SETTING(WindowBotY);
	int mlft = GET_SETTING(WindowTopX);
	int mrgt = GET_SETTING(WindowBotX);
	int mode = GET_SETTING(DecodeInWindow);

	if( pTrigger != NULL ) {
		if( pTrigger->IsPresentationTypeTrigger() ) {
			mtop = GET_SETTING(PresentationWindowTopY);
			mbot = GET_SETTING(PresentationWindowBotY);
			mlft = GET_SETTING(PresentationWindowTopX);
			mrgt = GET_SETTING(PresentationWindowBotX);
		}
	
		if( pTrigger !=NULL && pTrigger->GetCodeGateMode() ) {
			mode = 1;
			mtop = 47;
			mbot = 53;
			mlft = 40;
			mrgt = 60;
		}
	}

	//set up image parameters
	DecodeSet(DEC_IMAGE_CENTER_X,	(void *)cenX);
	DecodeSet(DEC_IMAGE_CENTER_Y,	(void *)cenY);
	DecodeSet(DEC_IMAGE_HEIGHT,		(void *)hgt);
	DecodeSet(DEC_IMAGE_WIDTH,		(void *)wid);
	DecodeSet(DEC_IMAGE_LINE_DELTA,	(void *)wrap);
	
	//set up window parameters
	DecodeSet(DEC_WINDOW_MODE,	(void *)mode);
	DecodeSet(DEC_WINDOW_TOP,	(void *)mtop);
	DecodeSet(DEC_WINDOW_BOTTOM, (void *)mbot);
	DecodeSet(DEC_WINDOW_LEFT,	(void *)mlft);
	DecodeSet(DEC_WINDOW_RIGHT, (void *)mrgt);

/*	Handle Monocolor Fission Configuration!  Need to FIXME for Xenon!!
	Inputs/Assumptions entering here:
		1. Decoder parameters have been set:  DEC_IMAGE_HEIGHT, DEC_IMAGE_WIDTH, DEC_IMAGE_LINE_DELTA
		2. ROI details for 'true windowing' 
		3. Offset (offset) has been calculated for ROI internal to image (DECWIN3)
		4. Assumes magic number values for first color pixel coordinates and color pixel stride for "full native' image 
	Output:
		1. Decoder parameters for X,Y of first color pixel: DEC_MONOCOLOR_OFFSETX,DEC_MONOCOLOR_OFFSETY		*/

	if(HardwareToken.IAmOEMJade())  // Handle Monocolor Fission Configuration! FIXME for Xenon!!
	{
		// Determine imager type by EngineID
		PSoC_5100_REGISTER_TABLE	Registers;
		Psoc5100GetRegisters( &Registers );

		if( Registers.EngineID.b.Imager ) // monocolor handling for fission
		{
			// Establish native full image coordinate system
			int offX = FIRST_COLOR_PIXEL_X;
			int offY = FIRST_COLOR_PIXEL_Y;

			if( GET_SETTING(ImageROIEnabled) ) // If native = ROI, need to adjust base 0,0 coordinate system
			{
				int xo = GET_SETTING(ImageROILeft);
				int yo = GET_SETTING(ImageROITop);

				offY = COLOR_PIXEL_SPACING_Y-((yo-offY)&(COLOR_PIXEL_SPACING_Y-1));		// adjust base 0,0 coordinate reference
				offX = COLOR_PIXEL_SPACING_X-((xo-offX)&(COLOR_PIXEL_SPACING_X-1));
			}

			DecodeSet(DEC_MONOCOLOR_OFFSET_X, (void *)offX);	// re-establish for all other cases... actually needs to get this from SD!!
			DecodeSet(DEC_MONOCOLOR_OFFSET_Y, (void *)offY);
			DecodeSet(DEC_MONOCOLOR_SPACING_X, (void *)COLOR_PIXEL_SPACING_X);
			DecodeSet(DEC_MONOCOLOR_SPACING_Y, (void *)COLOR_PIXEL_SPACING_Y);
		} // end of monocolor handling for color pixel config for fission operation inside decoder
	}	// end of Monocolor Fission Configuration!
}

void SendImageScore(int Score);
int ThisScore = 0;
int RecentAge = 0;

void FastDecode(unsigned char *PointerToImage, int IncludeDecode) {
	
	DecodeSet(DEC_FAST_DECODER_ENABLED,(void *)IncludeDecode); /*Change from Matt M. We pass in if fast is supposed to run for each call now so update the control for every frame.*/

	if( GET_SETTING(ImageROIEnabled) ) {
		SetImageParameters(nHeight,nWidth,nWidth,GET_CONFIG_SETTING(iImageXCenter),GET_CONFIG_SETTING(iImageYCenter));
	}

#if defined(DEVELOPMENT_BUILD)
 	int dh, dw, dr, dx, dy; //variables to save current height, wid, wrap, center x/y
   bool UndoSetup = 0;
   if (StoredImageArray && GET_SETTING(DecodeInjectedImage)) { 
      //save off current parameters
      DecodeGet(DEC_IMAGE_HEIGHT, (void *)&dh);
      DecodeGet(DEC_IMAGE_WIDTH, (void *)&dw);
      DecodeGet(DEC_IMAGE_LINE_DELTA, (void *)&dr);
      DecodeGet(DEC_IMAGE_CENTER_X, (void *)&dx);
      DecodeGet(DEC_IMAGE_CENTER_Y, (void *)&dy);

      //set new ones based on the current image
      SetImageParameters( StoredImageDescriptor.height, StoredImageDescriptor.width, StoredImageDescriptor.arrayWidth, StoredImageDescriptor.width/2, StoredImageDescriptor.height/2 );
      UndoSetup = 1;      
   }
   

#endif

	if( (Decode((void *)PointerToImage,DEC_TYPE_FLD) == DEC_SUCCESS) && IncludeDecode == 2 ) {
 		theCurrentInterface.printf("*");
	}
   
#if defined(DEVELOPMENT_BUILD)
		//restore parameters in case scanning needs to be done
		if (UndoSetup) SetImageParameters(dh, dw, dr, dx, dy);
#endif   
}

int GetBestNewImage(PBYTE *ppScanBuffer) {

	int bStatus = 1;

#if defined(DEVELOPMENT_BUILD)
	int energyColor = 19;
#endif

	if( RecentScore != 0 ) {
	  SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_ATTACH_TO_SAME_BUFFER,(UCHAR *)&P_RECENTBUFFER,1,(UCHAR *)ppScanBuffer,1,NULL);
      DecodingImageScore = RecentScore;
      RecentScore = 0;
      RecentAge = 0;

#if defined(DEVELOPMENT_BUILD)
	  energyColor = 26;
#endif
    } else {
      if (SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_NEW_SCAN,(UCHAR *)ppScanBuffer,0,NULL,0,NULL)) {
		  switch( (ImageQualityMode()) ) {
			  case 1:
				  DecodeGet(DEC_GET_ENERGY,&DecodingImageScore);
				  break;
			  case 2:
           case 4:
				  DecodingImageScore = ComputeImageScore((unsigned char *)ppScanBuffer, nWidth, nHeight, nWidth, GET_SETTING(ImageQualitySkip));
				  break;
			  case 3:
				  DWORD ScanDEnergy[] = {HHPSD_IMAGE_STAT_IQ_SCORE,0};
				  DWORD tempSize;
				  SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_IMAGE_ATTRIBUTES,(UCHAR *)ppScanBuffer,0,(PBYTE) ScanDEnergy,sizeof(ScanDEnergy)/sizeof(DWORD),&tempSize);
				  DecodingImageScore = ScanDEnergy[1];
				  break;
		  }
         RecentScore = 0;
         RecentAge = 0;
	  } else bStatus = 0;
	}
#if defined(DEVELOPMENT_BUILD)
   if( GET_SETTING(ImageQualityRanking) & 3) {
      switch( (ImageQualityMode()) ) {
			case 1:
				SendValueToBeColorGraphed(DecodingImageScore,200000,energyColor);
				break;
			case 2:
				SendValueToBeColorGraphed(DecodingImageScore,20000,energyColor);
				break;
			case 3:
				SendValueToBeColorGraphed(DecodingImageScore,20000,energyColor);
				break;
         case 4:
            if (PresModeDecoderTriggered) SendValueToBeColorGraphed(DecodingImageScore,20000,energyColor);
            break;
		}
	}
#endif
//printf("GetImage:0x%x,Score:%d\r\n",(int)P_SCANBUFFER,DecodingImageScore);
	return bStatus;
}

void ProcessLinearScan(unsigned char *PointerToImage) {

   bool ImageLocked = false;
   
   if (pTrigger != NULL) {
	   if( (ImageQualityMode()) ) {// && (pTrigger->GetTriggerMode() != STREAMING_PRESENTATION_TRIGGER)) {
         if (!STOP_DECODER_FLAG && PointerToImage && !ProcessedResult) {
          if (SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_NEW_IMAGE_AVAILABLE,NULL,0,NULL,0,NULL)) {
                if (TRUE == SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_NEW_SCAN,(UCHAR *)&P_FASTBUFFER,0,NULL,0,NULL)) {
                  ImageLocked = true;
               }
            }
            else {
               if (P_SCANBUFFER) {
                  if (TRUE == SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_ATTACH_TO_SAME_BUFFER,(UCHAR *)&P_SCANBUFFER,1,(UCHAR *)&P_FASTBUFFER,1,NULL)) {
                     ImageLocked = true;
                  }
               }
            }   

            if (ImageLocked) {
#if defined(DEVELOPMENT_BUILD)
               int timex = current_time();
#endif
	            if( (AuxiliaryDecoder() || ImageQualityMode() == 1 && !STOP_DECODER_FLAG && !ProcessedResult) ) {
#if defined(DEVELOPMENT_BUILD)
               if (StoredImageArray && GET_SETTING(DecodeInjectedImage)) FastDecode( StoredImageArray, (AuxiliaryDecoder()) );
               else FastDecode( PointerToImage, (AuxiliaryDecoder()) );
#else              
                  FastDecode( PointerToImage, (AuxiliaryDecoder()) );
#endif                  
	            }
               switch( (ImageQualityMode()) ) {
                  case 1:
                     DecodeGet(DEC_GET_ENERGY,&ThisScore);
                     break;
                  case 2:
                  case 4:
                     ThisScore = ComputeImageScore((unsigned char *)PointerToImage, nWidth, nHeight, nWidth, GET_SETTING(ImageQualitySkip));
                     break;
                  case 3:
                     DWORD ScanDEnergy[] = {HHPSD_IMAGE_STAT_IQ_SCORE,0};
                     DWORD tempSize;
                     SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_IMAGE_ATTRIBUTES,(UCHAR *)&P_FASTBUFFER,0,(PBYTE) ScanDEnergy,sizeof(ScanDEnergy)/sizeof(DWORD),&tempSize);
                     ThisScore = ScanDEnergy[1];
                     break;
               }

			   if( ThisScore && GET_SETTING(ImageQualityCellPhone) ) {
					DWORD exp[] = {HHPSD_IMAGE_STAT_EXPOSURE, 0};
					DWORD tempSize;
					SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_IMAGE_ATTRIBUTES,(UCHAR *)&P_FASTBUFFER,0,(PBYTE) exp,sizeof(exp)/sizeof(DWORD),&tempSize);

					if( imageExposureDecoding == exp[1] ) {
						ThisScore = 0;
					}
			   }
			   
#if defined(DEVELOPMENT_BUILD)
               if (GET_SETTING(ImageQualityRanking) & 3) {
                  timex = current_time() - timex;
                  if (GET_SETTING(ImageQualityRanking) & 2) {
//                     SendValueToBeColorGraphed(timex,2000,12);
                  } else {
                     switch( (ImageQualityMode()) ) {
                        case 1: SendValueToBeColorGraphed(ThisScore,200000,8); break;
                        case 2: SendValueToBeColorGraphed(ThisScore,20000,8); break;
                        case 3: SendValueToBeColorGraphed(ThisScore,20000,8); break;
                     }
                  }
               }
#endif
		//printf("%d ",ThisScore);
	         	RecentAge++;
	         	if (RecentAge > 8) RecentScore -= (RecentScore>>6);
	         	if (ThisScore >= RecentScore) {
	            	if (TRUE == SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_ATTACH_TO_SAME_BUFFER,(UCHAR *)&P_FASTBUFFER,1,(UCHAR *)&P_RECENTBUFFER,1,NULL)) {
		//printf(":0x%x ",(unsigned int)P_RECENTBUFFER);
	               	RecentScore = ThisScore;
	               	RecentAge = 0;
                  }
               }
            }
            else {
               if (ImageQualityMode() == 4) {
                  ThisScore = ComputeImageScore((unsigned char *)PointerToImage, nWidth, nHeight, nWidth, GET_SETTING(ImageQualitySkip));
               }
            }

            if (ImageQualityMode() == 4) { // PresentationMode
            	
//*Constants used to tune the algorithm*
//higher number = slower decay
#define AVE_DECAY 7
//Sets the trigger decoder sensitivity, higher increases sensitivity
#define TRIGGER_SENSITIVITY 2
//Sets the untrigger decoder sensitivity, lower increases sensitivity
#define UNTRIGGER_SENSITIVITY 4
#define maxGraphHeight 35000

               //Set initial value for AverageScore
               if (AverageScore == -1) AverageScore = ThisScore;
               else{
                  //Update AverageScore 
                  AverageScore = ((AverageScore<<AVE_DECAY)-AverageScore+(ThisScore))>>AVE_DECAY;
                  //SendValueToBeColorGraphed(AverageScore, maxGraphHeight, 0);
               }
			
               //**Code to Deactivate the Decoder**
               UntriggerDecoderThreshold = (AverageScore >> UNTRIGGER_SENSITIVITY);
               if ((ThisScore < (AverageScore + UntriggerDecoderThreshold )) && PresModeDecoderTriggered){
                  RepCnt++;
                  //Wait 5 frames before deactivating the decoder
                  if (RepCnt > GET_SETTING(IQSenseSensitivity)) {
                     if (LastMessageIndex != -1)  {
                        Reads[0].Length = -1;
                        LastMessageIndex = -1;
                     }
                     RepCnt = 0;
                     if (PresModeDecoderTriggered) {
                        PresModeDecoderTriggered = 0;//deactivate decoder
                     }
                  }
               }
               
               //**Code to Activate the Decoder**
               TriggerDecoderThreshold =  AverageScore +((int)(AverageScore*1.5)>>TRIGGER_SENSITIVITY);
               if (TriggerDecoderThreshold < ThisScore) {
#if defined(DEVELOPMENT_BUILD)                     
                  if (GET_SETTING(ImageQualityRanking) && !PresModeDecoderTriggered) SendValueToBeColorGraphed(100,100,22);
 #endif                     
                  PresModeDecoderTriggered = 1; //trigger decoder
               }
               //**Code for testing purposes to display the image score graphically**    
#if defined(DEVELOPMENT_BUILD)
               if (GET_SETTING(ImageQualityRanking)) {
                  if (PresModeDecoderTriggered) SendValueToBeColorGraphed(ThisScore,maxGraphHeight,4);
                  else SendValueToBeColorGraphed(ThisScore,maxGraphHeight,11);
               }
#endif               
            }
         }
#if defined(DEVELOPMENT_BUILD)
	   } else if( (AuxiliaryDecoder()) && !STOP_DECODER_FLAG && PointerToImage && !ProcessedResult ) {
         if (StoredImageArray && GET_SETTING(DecodeInjectedImage)) FastDecode( StoredImageArray, (AuxiliaryDecoder()) );
         else FastDecode( PointerToImage, (AuxiliaryDecoder()) );
      }
#else
	   } else if( (AuxiliaryDecoder()) && !STOP_DECODER_FLAG && PointerToImage && !ProcessedResult ) FastDecode( PointerToImage, (AuxiliaryDecoder()) );
#endif      
   }
}

extern int gScanDriverCellPhone; // cell phone flag
void SetVigorEtAl(int NumberOfDecodeAttempts, int *Vigor, int *DecTmn, int *DecTmx)
{
	int Control;
	int Trigger;

	Trigger = GetTriggerMode();

	if ((GET_SETTING(ForcePresentationStyleDecode)) || (Trigger == SCANSTAND_TRIGGER) || ((!PrsModesPDFdetected()) && ((Trigger == STREAMING_PRESENTATION_TRIGGER) || (Trigger == PRESENTATION_TRIGGER))))
	{
		Control = GET_SETTING(PresentationDecodeControl);
		*Vigor = GET_SETTING(PresentationDecodeVigor);
		*DecTmn = GET_SETTING(PresentationDecodeMinTimeout);
		*DecTmx = GET_SETTING(PresentationDecodeMaxTimeout);
#if (DECODER_ID)
		DecodeSet(DEC_PASS_THROUGH, (void *)GET_SETTING(PresenationDecodePassThrough));
		DecodeSet(DEC_CYCLING_FINDER, (void *)GET_SETTING(PresenationDecodeCyclingFinder));
		DecodeSet(DEC_SUBPIXEL_FINDER, (void *)GET_SETTING(PresenationDecodeSubpixelFinder));
		DecodeSet(DEC_EDGE_DETECTOR,(void *)GET_SETTING(PresenationDecodeEdgeDetector));
		DecodeSet(DEC_AZTEC_SYMBOL_SIZE,(void *)GET_SETTING(PresenationDecodeAztecSymbolSize));
		DecodeSet(DEC_MAXICODE_SYMBOL_SIZE,(void *)GET_SETTING(PresenationDecodeMaxicodeSymbolSize));
		DecodeSet(DEC_DATAMATRIX_SYMBOL_SIZE,(void *)GET_SETTING(PresenationDecodeDataMatrixSymbolSize));
		DecodeSet(DEC_DATAMATRIX_LOW_CONTRAST,(void *)GET_SETTING(PresenationDecodeDataMatrixLowContrast));
		DecodeSet(DEC_QR_SYMBOL_SIZE,(void *)GET_SETTING(PresenationDecodeQRCodeSymbolSize));
		DecodeSet(DEC_LOW_CONTRAST_IMPROVEMENTS,(void *)GET_SETTING(PresenationDecodeLowConstrastImprovements));
#endif
	}
	else
	{
		Control = GET_SETTING(DecodeControl);
		*Vigor = GET_SETTING(DecodeVigor);
		*DecTmn = GET_SETTING(DecodeMinTimeout);
		*DecTmx = GET_SETTING(DecodeMaxTimeout);

#if (DECODER_ID)
		DecodeSet(DEC_PASS_THROUGH, (void *)GET_SETTING(DecodePassThrough));
		DecodeSet(DEC_CYCLING_FINDER, (void *)GET_SETTING(UseCyclingFinder));  // CYCLING FINDER
//#ifndef XENON_LITE_1500
//ewr06509
		DecodeSet(DEC_SUBPIXEL_FINDER, (void *)GET_SETTING(DecodeSubpixelFinder));
		DecodeSet(DEC_EDGE_DETECTOR,(void *)GET_SETTING(DecodeEdgeDetector));
//#else
//		DecodeSet(DEC_SUBPIXEL_FINDER, (void *)DEC_FALSE); //TRUE);	// SPD change for ID De feature
//		DecodeSet(DEC_EDGE_DETECTOR,(void *)DEC_FALSE); //TRUE);  // SPD change for ID De feature
//#endif
		DecodeSet(DEC_AZTEC_SYMBOL_SIZE,(void *)GET_SETTING(aztSmallSymbolSize));
		DecodeSet(DEC_MAXICODE_SYMBOL_SIZE,(void *)GET_SETTING(maxSmallSymbolSize));
		DecodeSet(DEC_DATAMATRIX_SYMBOL_SIZE,(void *)GET_SETTING(idmSmallSymbolSize));
		DecodeSet(DEC_DATAMATRIX_LOW_CONTRAST,(void *)GET_SETTING(idmLowContrast));
		DecodeSet(DEC_QR_SYMBOL_SIZE,(void *)GET_SETTING(qrcSmallSymbolSize));
		DecodeSet(DEC_LOW_CONTRAST_IMPROVEMENTS,(void *)GET_SETTING(LowSymbolContrast));
#endif
	}

//#ifndef XENON_LITE_1500//ewr06509
	if (gScanDriverCellPhone && !NumberOfDecodeAttempts)
	{	
		*DecTmn = 50;		// hard code cell phone for the first image to shorter, give illuminated frame abbreviated time to get to cell phone image 
		*DecTmx = 150;
	}
//#endif

	if (!GET_SETTING(DDDiag))
	{
		if (GET_SETTING(DecodeMode) == 1)
		{
			*DecTmn = 50;
			*Vigor = 0;
		}
		else
		{
			if (Control & 0x01)          // Varying MayContinue() timeouts?!
			{
				switch (NumberOfDecodeAttempts)
				{
					case 0:
						if (*DecTmn > 50) {
							*DecTmn = 50;
						}
						break;
					case 1:
						if (*DecTmn > 80) {
							*DecTmn = 80;
						}
						break;
					case 2:
						if (*DecTmn > 110) {
							*DecTmn = 110;
						}
						break;
					case 3:
						if (*DecTmn > 140) {
							*DecTmn = 140;
						}
						break;
					default:
						break;
				}
			}		//end if (Control & 0x01)

			if (*DecTmn == 0) {
				*DecTmn = 99999;
			}

			if (*DecTmx == 0) {
				*DecTmx = 99999;
			}

			if (Control & 0x02)                // Varying Decode "Vigor"?
			{
				switch (NumberOfDecodeAttempts)
				{
					case 0:
					case 1:
						*Vigor = 0;
						break;
					case 2:
					case 3:
						if (*Vigor > 1) {
							*Vigor = 1;
						}
						break;
					default:
						break;
				}
			}
		}                   //end else{},  [if not (GET_SETTING(DecodeMode) == 1)]

		if (Control & 0x04)                // Varying Decode "Vigor" AND time
		{
			switch (NumberOfDecodeAttempts)
			{
				case 0:
					*Vigor = 0;
					*DecTmn = 50;
					break;
				case 1:
					if (*Vigor > 1) {
						*Vigor = 1;
					}
					break;
				default:
					break;
			}
		}
	}               //end if (!GET_SETTING(DDDiag))
	
	DecodeSet(DEC_DECODE_VIGOR,				(void *)*Vigor);
} //end SetVigorEtAl()


//Set the Image Quality according to interface
int ImageQualityMode(void)
{
	int settingSelected = 0;

	if( GetTriggerMode() == STREAMING_PRESENTATION_TRIGGER )         //Get current trigger mode
	{
		settingSelected = mPresentationImageQualityMode;
	}
	else
	{
		settingSelected = mImageQualityMode;
	}

	return settingSelected;
}


//Set the Aux decoder according to interface
int AuxiliaryDecoder(void)
{
	int settingSelected = 0;

	if( GetTriggerMode() == STREAMING_PRESENTATION_TRIGGER )         //Get current trigger mode
	{
		settingSelected = mPresentationAuxiliaryDecoder;
	}
	else
	{
		settingSelected = mAuxiliaryDecoder;
	}

	return settingSelected;
}


/*----------------------------------------------------------------------*/
/*	"DecodeIFaceOnChangedConfiguration(void) is the main call to the decoder, 	*/
/*	from mainhhplproc, to set up menu settings	*/
/*----------------------------------------------------------------------*/

void DecodeIFaceOnChangedConfiguration(void)
{
	mTriggerMode = GET_SETTING(TriggerMode);
	mPresentationAuxiliaryDecoder = GET_SETTING(PresentationAuxiliaryDecoder);
	mPresentationImageQualityMode = GET_SETTING(PresentationImageQualityMode);
	mAuxiliaryDecoder = GET_SETTING(AuxiliaryDecoder);
	mImageQualityMode = GET_SETTING(ImageQualityMode);

	if( mTriggerMode == STREAMING_PRESENTATION_TRIGGER )
	{
		selectedImageQualityMode = mPresentationImageQualityMode;
	}
	else
	{
		selectedImageQualityMode = mImageQualityMode;
	}
}
		

/*----------------------------------------------------------------------*/
/*	"decode(img) is the main call to the decoder, operating on the gray	*/
/*	scale image "img" (TOPXxTOPY), outputting any results via StuffMsg()	*/
/*----------------------------------------------------------------------*/

void decode (PIXEL PC_HUGE *img)
{
	START_DECODER;
	if (ImageQualityMode() == 4 && !PresModeDecoderTriggered) return;

	bWasScanStnd=false;

	int Vigor;
	void *pBackupImg = NULL;

	//TRACE(" Decode ");

	for (int i=0; i<MAX_SHOTGUN_HISTORY; i++)
	{
		ShotgunCodeID[i] = DEFAULT_CODEID;
	}

	NumberOfShotgunMessages = 0;
	
	SetVigorEtAl(NumberOfDecodeAttempts, &Vigor, &DecTmn, &DecTmx);
				
	DecodingStartTime = BaselineClock;  // for a possible Time-out


	if (Decoder_Handle)
	{

#ifdef DEVELOPMENT_BUILD
		if( !GET_SETTING(DisableDecoder) ) {
         if (StoredImageArray && GET_SETTING(DecodeInjectedImage)) { 
            int dh, dw, dr, dx, dy; //variables to save current height, wid, wrap, center x/y
            //save off current parameters
            DecodeGet(DEC_IMAGE_HEIGHT, (void *)&dh);
            DecodeGet(DEC_IMAGE_WIDTH, (void *)&dw);
            DecodeGet(DEC_IMAGE_LINE_DELTA, (void *)&dr);
            DecodeGet(DEC_IMAGE_CENTER_X, (void *)&dx);
            DecodeGet(DEC_IMAGE_CENTER_Y, (void *)&dy);

            //set new ones based on the current image
            SetImageParameters( StoredImageDescriptor.height, StoredImageDescriptor.width, StoredImageDescriptor.arrayWidth, StoredImageDescriptor.width/2, StoredImageDescriptor.height/2 );
         
            OPdecode(StoredImageArray, StoredImageDescriptor.width, StoredImageDescriptor.height, StoredImageDescriptor.width,StoredImageDescriptor.width/2,StoredImageDescriptor.height/2,DEC_TYPE_ID);
            SetImageParameters(dh, dw, dr, dx, dy);
         }
			else OPdecode(img, nWidth, nHeight, nWidth,GET_CONFIG_SETTING(iImageXCenter),GET_CONFIG_SETTING(iImageYCenter),DEC_TYPE_ID);
      }
		else {
			OPdecode(img, nWidth, nHeight, nWidth,GET_CONFIG_SETTING(iImageXCenter),GET_CONFIG_SETTING(iImageYCenter),0);
		}
#else
		if( ImageQualityMode() && GET_SETTING(ImageQualityCellPhone) ) {
			DWORD exp[] = {HHPSD_IMAGE_STAT_EXPOSURE, 0};
			DWORD tempSize;
			SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_IMAGE_ATTRIBUTES,(UCHAR *)&P_SCANBUFFER,0,(PBYTE) exp,sizeof(exp)/sizeof(DWORD),&tempSize);
			imageExposureDecoding = exp[1];
		}

		OPdecode(img, nWidth, nHeight, nWidth,GET_CONFIG_SETTING(iImageXCenter),GET_CONFIG_SETTING(iImageYCenter),DEC_TYPE_ID);
#endif //DEV settings
	}

	DecodingEndTime = BaselineClock; // to check if decoding continues apace...
	
	NumberOfDecodeAttempts++;

	// if we have a 3800G, it will have reassigned pImg to point at the local scan profile
	if (pBackupImg != NULL)
	{
		pImg = pBackupImg;
	}

// contact sensitive, Set the value if we got a good decode
#ifdef _SCAN_DRIVER_INTERFACE_H_
	if( ProcessedResult ) {
		DWORD ExpDcdImg[2]	= { HHPSD_IMAGE_STAT_DECODE_PARAMETER, 0 };
		DWORD IdleExp[2]	= { ES_DECODE_PARAMETER, 0 };
		DWORD nSize;

		if(SCN_IOControl(nOpenCntxt, HHPSD_IOCTL_GET_IMAGE_ATTRIBUTES,
			(UCHAR *)&P_SCANBUFFER,0,(PBYTE)ExpDcdImg,sizeof(ExpDcdImg)/sizeof(DWORD),&nSize))
		{
			IdleExp[1] = ExpDcdImg[1];
			SCN_IOControl(nOpenCntxt, HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)IdleExp,sizeof(IdleExp),NULL,0,NULL);
		}
	}
#endif //SCAN_DRIVER

	if (ProcessedResult)     // P_SIGCAPBUFFER has the buffer that decode - fast or full - put the buffer that decoded in P_SCANBUFFER for IMGSHP
	{
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_ATTACH_TO_SAME_BUFFER,(UCHAR *)&P_SIGCAPBUFFER,1,(UCHAR *)&P_SCANBUFFER,1,NULL);
	}

#if defined(DEVELOPMENT_BUILD)
   if( ProcessedResult && GET_SETTING(ImageQualityRanking)) SendValueToBeColorGraphed(100,100,0);
#endif
}


void DecodeForMenuing(PIXEL PC_HUGE *img)
{
	OnlyAllowMenuDecodes = TRUE;
	IgnoreTrigger = TRUE;
	decode(img);
	IgnoreTrigger = FALSE;
	OnlyAllowMenuDecodes = FALSE;
}


bool DecodeCurrentImage(void)
{
	if (P_SCANBUFFER)
	{
		START_DECODER;
		ProcessedResult = 0;
		ClearDecoderWorkspace();

		for (int i=0; i<MAX_SHOTGUN_HISTORY; i++)
		{
			ShotgunCodeID[i] = DEFAULT_CODEID;
		}

		NumberOfShotgunMessages = 0;
		IgnoreTrigger = TRUE;
		ReDecoding = TRUE;
		DecodingStartTime = BaselineClock;	// for a possible Time-out
		decode(P_SCANBUFFER);
		IgnoreTrigger = FALSE;
		ReDecoding = FALSE;
		xputstring(" Done ReDecoding \r\n");

#if !defined( RFBASE ) && !defined( RFSCAN )
		g_pScriptInterpreter->ProcessTriggerlessBarcode(ProcessedResult);
#endif

		return true;
	}
	else
	{
		return false;
	}
}

#ifdef PLUGIN_DEV
#ifdef PLUGIN_DECODER
extern volatile HONPluginDecoderState  PluginDecoderState;
#endif
#endif



/*----------------------------------------------------------------------------------------
ResetIqScoring

  This resets the scoring using in IQ.  We need to do this when in Presentation Trigger
  Mode and someone hits the trigger going into temporary Manual Trigger Mode because we
  need to not use any old images that have a > 0 score.  Otherwise, we'd get a false decode
  on an old image.

  Parameters:  none

  Returns:  nothing
----------------------------------------------------------------------------------------*/
void ResetIqScoring(void)
{
	RecentScore = 0;
}


/*----------------------------------------------------------------------------------------
ClearDecoderWorkspace

  We need to be careful calling this because it resets ProcessedResult.  We had to fix
  race conditions where the main loop thread was racing against the fast linear thread by
  calling this.

  Parameters:  none

  Returns:  nothing
----------------------------------------------------------------------------------------*/
void ClearDecoderWorkspace(void)
{
	NumberOfDecodeAttempts = 0;
	//DecodingInitiationTime = BaselineClock;                    //not currently being used
	
	MT_ReleaseMutex(InProcessResult);
	DISABLE_INTERRUPTS();
	DecoderSendingMessage = -1;
	ProcessedResult=0;
	START_DECODER;
#ifdef PLUGIN_DEV
#ifdef PLUGIN_DECODER
	START_PLUGIN_DECODER;
	PluginDecoderState = HON_PLUGIN_DECODER_STATE_INIT;
#endif //PLUGIN_DEV
#endif //PLUGIN_DECODER

	RESTORE_INTERRUPTS();
	
	RecentScore = 0;

}

int CheckForScanStand(PIXEL PC_HUGE */*img*/, unsigned int* /*GetPixelValue*/)
{
	int ret = -30000;
	bWasScanStnd=false;

	return ret;
}


/************************************************************************/
void SaveThisImage(GrayscaleImage2 *ImageDescriptor) {
	if (P_SCANBUFFER && (ImageDescriptor->width*ImageDescriptor->height <= nWidth*nHeight)) {
		char *dst,*src;
		dst = (char *)P_SCANBUFFER;
		src = (char *)ImageDescriptor->array;
		for (int x=0; x<ImageDescriptor->width*ImageDescriptor->height; x++) *dst++ = *src++;
	}
}

/*----------------------------------------------------------------------*/
/*	"decode_2d_from_memory(ImageDescriptor, img) is similar to decode,  */
/*	but allows the caller to specify the image descriptor in order to	*/
/*	allow decoding of 2D data uploaded from a host.						*/
/*----------------------------------------------------------------------*/
void decode_2d_from_memory (GrayscaleImage2 *ImageDescriptor, int /*WithVigor*/)
{
	int Vigor;
	int dh, dw, dr, dx, dy; //variables to save current height, wid, wrap, center x/y

	if (Decoder_Handle)
	{
		int searchX,searchY;
		START_DECODER;
		ProcessedResult=0;

		for (int i=0; i<MAX_SHOTGUN_HISTORY; i++)
		{
			ShotgunCodeID[i] = DEFAULT_CODEID;
		}

		if (ImageDescriptor->width != nWidth || ImageDescriptor->height != nHeight || GET_SETTING(UseMiddleOfImage))
		{
			searchX = ImageDescriptor->horizontalCenter;
			searchY = ImageDescriptor->verticalCenter;
		}
		else if( HardwareToken.IAmOEM() && ImageEngineHasPsoc() ) {
			DWORD	GetImagerPropertiesRequest[ 4 ] = { IP_AIMER_X_OFFSET, 0, IP_AIMER_Y_OFFSET, 0 };

			SCN_IOControl( nOpenCntxt, HHPSD_IOCTL_GET_IMAGER_PROPERTIES, (PBYTE) GetImagerPropertiesRequest, sizeof( GetImagerPropertiesRequest ),
		                   (PBYTE) GetImagerPropertiesRequest, sizeof( GetImagerPropertiesRequest ), NULL ); 
			
			searchX = GetImagerPropertiesRequest[1];
			searchY = GetImagerPropertiesRequest[3];
		}
		else
		{
			searchX = GET_CONFIG_SETTING(iImageXCenter);
			searchY = GET_CONFIG_SETTING(iImageYCenter);
		}
		
		//save off current parameters
		DecodeGet(DEC_IMAGE_HEIGHT, (void *)&dh);
		DecodeGet(DEC_IMAGE_WIDTH, (void *)&dw);
		DecodeGet(DEC_IMAGE_LINE_DELTA, (void *)&dr);
		DecodeGet(DEC_IMAGE_CENTER_X, (void *)&dx);
		DecodeGet(DEC_IMAGE_CENTER_Y, (void *)&dy);

		//set new ones based on the current image
		SetImageParameters( ImageDescriptor->height, ImageDescriptor->width, ImageDescriptor->arrayWidth, searchX, searchY );

		NumberOfShotgunMessages = 0;
		IgnoreTrigger = TRUE;
		NumberOfDecodeAttempts = 22;
		SetVigorEtAl(NumberOfDecodeAttempts, &Vigor, &DecTmn, &DecTmx );
		DecodingStartTime = BaselineClock;  // for a possible Time-out

		OPdecode(ImageDescriptor->array,ImageDescriptor->width, ImageDescriptor->height, ImageDescriptor->arrayWidth,searchX,searchY, DEC_TYPE_ID);

		NumberOfDecodeAttempts++;
		theStdInterface.printf("Decode Time = %d ms \r\n",BaselineClock-DecodingStartTime);
		IgnoreTrigger = false;

		//restore parameters in case scanning needs to be done
		SetImageParameters(dh, dw, dr, dx, dy);
	}
}


void SaveDownloadedImage(GrayscaleImage2 *ImageDescriptor) {
   memcpy(&StoredImageDescriptor, ImageDescriptor, sizeof(GrayscaleImage2));

	if (StoredImageArray)
	{
		free(StoredImageArray);
	}

	StoredImageArray = (unsigned char *)malloc(StoredImageDescriptor.width * StoredImageDescriptor.height);
	memcpy(StoredImageArray, StoredImageDescriptor.array, StoredImageDescriptor.width * StoredImageDescriptor.height);
	StoredImageDescriptor.array = StoredImageArray;

}

// DecodeAndSaveImage() -- This is allow multiple decodes of the same image without having to download the image repetatively.
// This might be used in a menu compliance test. Since the image is left on the heap, This call should only be done in a test mode.


void DecodeAndSaveImage(GrayscaleImage2 *ImageDescriptor)
{
   SaveDownloadedImage(ImageDescriptor);
	decode_2d_from_memory(&StoredImageDescriptor, 0);
}


//  \16N,REDECODE\0d
bool RedecodeSavedImage(void)
{
	if (StoredImageArray)
	{
		decode_2d_from_memory(&StoredImageDescriptor, 0);

		return true;
	}

	return false;
}


//
//	GetWorkspacePointer
//		Get pointer to decoder workspace.
//
void *GetWorkspacePointer(void)
{
	return(Decoder);
} // GetWorkspacePointer


void * GetImagePointer(void)
{
	return pImg;
}


int GetImageWidth(void)
{
	return nWidth;
}
int GetImageHeight(void)
{
	return nHeight;
}


// Initializer for the structure --  should be called on power up
// **must be called after scan driver functions have been called to get image size **
void PowerOnInitLastDecodeInfo ()
{
	//one time malloc of image buffer to be used
	//pgLastDecodeInfo->pImg = NULL;  took out writes over reset vector !!!!
	pgLastDecodeInfo = NULL;
}


// None of this information is in one place, so build the Imgbox last decode structure from
// wherever you can get the information.
void StoreLastDecodeInfo (DecoderResult *msg)
{
	static int tTime1, tTime2;

	pgLastDecodeInfo = &LastDecodeInfo;    // setup structure pointer, then go to one of 3 places to fill it in

	// copy image and set up structure pointer
	tTime1 = current_time();
	pgLastDecodeInfo->rows = (int) nHeight;
	pgLastDecodeInfo->cols = (int) nWidth;

	// Lock down the buffer that decode to be later used by SigCap
	if (msg->DecodeType == DECODE_TYPE_FAST)
	{
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_ATTACH_TO_SAME_BUFFER,(UCHAR *)&P_FASTBUFFER,1,(UCHAR *)&P_SIGCAPBUFFER,1,NULL);
	}
	else
	{
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_ATTACH_TO_SAME_BUFFER,(UCHAR *)&P_SCANBUFFER,1,(UCHAR *)&P_SIGCAPBUFFER,1,NULL);
	}

	pgLastDecodeInfo->pImg = P_SIGCAPBUFFER;
	// debug
	tTime2 = current_time();

	if (GET_SETTING(DebugSignatureCapture))		// for now, use the "non used" but present DBGTTR to spit out debug messages
	{
		char MsgBuf[100];
		sprintf (MsgBuf, "Time to copy 300K = %dus\r\n", tTime2-tTime1);
		xputstring (MsgBuf);
	}

	//xputstring (P_SCANBUFFER, 20);
	//xputstring ("\r\n");

	for (int i=0;i<4;i++)
	{
		pgLastDecodeInfo->corners[i].x = msg->IQImageInfo.bounds.corners[i].x;
		pgLastDecodeInfo->corners[i].y = msg->IQImageInfo.bounds.corners[i].y;
	}

	pgLastDecodeInfo->dcorner = msg->IQImageInfo.d_corner;
	pgLastDecodeInfo->pdfrows = msg->IQImageInfo.pdfRows;
	pgLastDecodeInfo->pdfcols = msg->IQImageInfo.pdfCols;
	pgLastDecodeInfo->codeid = msg->HHPCodeID;
	pgLastDecodeInfo->imgx = 0; //IMGX;
	pgLastDecodeInfo->imgy = 0; //IMGY;

	if (GET_SETTING(DebugSignatureCapture))		// for now, use the "non used" but present DBGTTR to spit out debug messages
	{
		//DumpLastDecodeStructure();                                        //lmc   fixme      Code not currently in our project
	}
}

// Returns pointer to structure full of useful things associated with the last image
void *GetLastDecodeInfo (void)
{
	return (pgLastDecodeInfo);
}


static int SendAsPacket;

void CopyResult(const char * bfr, int result_len)
{
	if (SendAsPacket)
	{
		int				PacketHeaderLength = 0;
		unsigned char	PacketHeader[ 20 ];
		const char		*PacketHeaderCommand = "\rBURPED";
		PacketHeader[ PacketHeaderLength++ ] = '\x02';
		PacketHeader[ PacketHeaderLength++ ] = '\xfe';
		PacketHeader[ PacketHeaderLength++ ] = ((unsigned char) ((result_len+6) & 0xff));
		PacketHeader[ PacketHeaderLength++ ] = ((unsigned char) (((result_len+6) >> 8) & 0xff));
		PacketHeader[ PacketHeaderLength++ ] = ((unsigned char) (((result_len+6) >> 16) & 0xff));
		PacketHeader[ PacketHeaderLength++ ] = ((unsigned char) (((result_len+6) >> 24) & 0xff));
		strcpy( (char *) &PacketHeader[ PacketHeaderLength ], PacketHeaderCommand );
		PacketHeaderLength += strlen( PacketHeaderCommand );
		SendStringToInterface((const char *)PacketHeader, PacketHeaderLength );
	}

	SendStringToInterface(bfr,result_len);
}


bool DisplayDecoderSetup(void)
{
	return false;
}


bool DisplayDecoderData(void)
{
	if (Decoder)
	{
		SendAsPacket = false;
		return true;
	}

	return false;
}


void DumpResult(const char * bfr, int)
{
	xputstring(bfr);
}


bool DumpDecoderSetup(void)
{
	if (Decoder)
	{
		DecodeGet(DEC_DUMP_SETTINGS,(void *)DumpResult);
		return true;
	}

	return false;
}


bool DumpDecoderData(void)
{
	if (Decoder)
	{
		DecodeGet(DEC_DISPLAY_DATA,(void *)DumpResult);
		return true;
	}

	return false;
}


bool ResetDecoderWorkspace(void)
{
	ClearDecoderWorkspace();
	NumberOfDecodeAttempts = 0;

	return true;
}


void FreeDecoderWorkspace(void)
{
	if (Decoder)
	{
		Decoder = 0;
	}
}


void SetUpDecoder(int Width, int Height)
{
	if (0 == (Decoder = EstablishDecoder(Width, Height)))
	{
		xputstring("Couldn't establish decoder... ");

		return;
	}
	
	FillDecoderMenu();
	PowerOnInitLastDecodeInfo();
	NumberOfDecodeAttempts = 0;
	ClearDecoderWorkspace();
}

void DecodeDownloadedImage (GrayscaleImage2 */*ImageDescriptor*/) {}


