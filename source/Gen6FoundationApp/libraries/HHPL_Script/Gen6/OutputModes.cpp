//======================================================================================
// $RCSfile: Gen6/OutputModes.cpp $
// $Revision: 1.9 $
// $Date: 2010/03/25 18:03:12EDT $
//======================================================================================

//======================================================================================

#include <math.h>
#include "OutputModes.h"
#include "menu.h"
#include "readerconfiguration.h"
#include "HSTRING.H"
#include "barcodedataman.h"// SEQUENCE support
#include "barcodefifo.h"

#define	TRIGGER_5_5		1
#ifdef TRIGGER_5_5
#include "trigger_5_5.h"                                         //lmc   fixme
#else
#include "trigger.h"
#endif	//end #ifdef TRIGGER_5_5

#include "ioman.h"
#include "dataedit.h"
#include "beep.h"


// Constant defines
#define	UNIVERSAL_CHAR			0x99


// Possible return values for the InsertMessageIntoSequence routine
enum	{ NON_CENTERED = -3, NO_MATCH = -2, DUPLICATE = -1, RETURN_FAIL = 0, ADD_ENTRY = 1, MATCH = 2, FOUND_SEQUENCE = 3 };


// These macros are needed to move around in the menued sequence
// block.  Depending on how menu blocks are implemented this might
// need to change.
#define	SEQUENCE_ENTRY_IS_NOT_EMPTY(p)	(p[0]!=0xff)
#define	MOVE_TO_NEXT_ENTRY(p)								\
			if( TRUE )												\
			{															\
				while(SEQUENCE_ENTRY_IS_NOT_EMPTY(p)) 		\
				{														\
					p += 1;											\
				}														\
				p += 1;												\
			}



COutputModes * g_pOutputModes = NULL;

void COutputModes::CreateOutputModes(void)
{
	if (g_pOutputModes == NULL)
	{
		g_pOutputModes = new COutputModes;
	}
	ASSERT(g_pOutputModes != NULL);
}

void COutputModes::DestroyOutputModes(void)
{
	delete g_pOutputModes;
	g_pOutputModes = NULL;
}


COutputModes::COutputModes(void)
{
	nSequenceMode = SEQUENCE_DISABLED;
	pSequenceBlk = NULL;

	bPreferredMode = false;
	bAllowNonPreferred = false;
	pPreferredBlk = NULL;
	pNonPreferredBlk = NULL;
	pPrioritySequenceTimeout = NULL;		// Ensure that this doesn't get used before initialization.
	pPriorityReadTimeout = NULL;		// Ensure that this doesn't get used before initialization.
	PreferredCodeID = 0;

	bCtsManualMode = false;

	iOutputModesPrioritySequenceTimeValue = 1;
	iOutputModesGoodReadDly = 0;
	iOutputModesReReadDly = 0;
	iOutputModesReReadDly2D = 0;

	iMultiBarcodeCountPartial = 0;
	nPartialSequenceMode = SEQUENCE_DISABLED;

	for (int i=0; i<MAX_MULTI_BARCODES;i++)
	{
		pPrefBuff[i]=NULL;
		pSeqBuff[i]=NULL;
		pSeqBuffPartial[i]=NULL;
	}

	OnChangedConfiguration();	// initial call
}


COutputModes::~COutputModes(void)
{
	ClearSequenceBuffer();
	ClearPreferredBuffer();
	ClearSequenceBufferPartial();

	// Dump the read before clear timeout
	// Dump the priority timeout
	ClearAndDeletePreferredTimers();
}


void COutputModes::OnChangedConfiguration(void)
{
	nSequenceMode = GET_SETTING( SequenceEnabled );
	pSequenceBlk = (GET_SETTING( SequenceBlock ))->Char;
	SetSequenceStartedIfRequired(); // if sequence required dont untrigger unless sequence complete (fake sequence already started)

	bPreferredMode = GET_SETTING( PrioritySequence );
	pPreferredBlk = (GET_SETTING( CodePreferredBlock ))->Char;
	pNonPreferredBlk = (GET_SETTING( NonPreferredBlock ))->Char;
	mPreferredStarted = false;

	iOutputModesPrioritySequenceTimeValue = GET_SETTING(PrioritySequenceTimeValue);
	iOutputModesGoodReadDly = GET_SETTING(GoodReadDly);
	iOutputModesReReadDly = GET_SETTING(ReReadDly);
	iOutputModesReReadDly2D = GET_SETTING(ReReadDly2D);

	nPartialSequenceMode = (int)(GET_SETTING( TimeoutTransmitSequenceEnable ));


//ewr-02787
/*
	if( (0 == theStdInterface.GetTerminalID()) && (3 == GET_SETTING(RS232CtsEnable)) )
	{
		bCtsManualMode = true;
	}
	else
	{
		bCtsManualMode = false;
	}
*/
//

}



//The prefered method section
//
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//	DoPreferred
//
//	Checks to see if a barcode messages find a preferred symbology.
// If it does not, it send non preferred barcode after timeout.
//
// Returns:
//  2 if it found the correct sequence
//  0 if main routine needs to handle data
// -1 if a duplicate was found
// -2 if no match was found
// -3 if non centered
//--------------------------------------------------------------------------
int  COutputModes::DoPreferred( CBarcodeData& Msg, int* piPreferredBarcodeIndex )
{
	enum				{ MATCH_NONE = -1 };

	INT32 nStatus = 0;
	BYTE *pSeq;
	BYTE *pPrefSeq;
	int i;
	int nMatchIndex;

	int nInsertMatchIndex = MATCH_NONE;
	int* pnInsertMatchIndex;

	pnInsertMatchIndex = &nInsertMatchIndex;
	nMatchIndex = MATCH_NONE;
	*piPreferredBarcodeIndex = nMatchIndex;

	//Set state of preferred timers
	UpdatePreferredTimers();

	// If not menued for preferred then bail out now and
	// tell the caller to output the message normally. Also check if sequence blocks is loaded
	pSeq = pNonPreferredBlk;
	pPrefSeq = pPreferredBlk;
	if( (bPreferredMode == false) || !(SEQUENCE_ENTRY_IS_NOT_EMPTY(pSeq)) || !(SEQUENCE_ENTRY_IS_NOT_EMPTY(pPrefSeq)) )
	{
		return 0;
	}

	//Check and insert barcode
	nStatus = InsertPreferredMessageIntoSequence( Msg, pnInsertMatchIndex );


	//Make a decision on the returned status

	// this status will only return in prioritized sequencing with PDF as preferred
	if (nStatus == NON_CENTERED)
	{
		// Dump it, but we need to tell the trigger.cpp stuff it's okay to reread this code - therefore, we reset the reread delay
		ClearLastReadTimestamp();      //new rereaddelay clear, used in 4600r
		pTrigger->ClearGoodReadTimeout();
		Msg.Free();
		return -3;
	}
	else if( (nStatus == DUPLICATE) && (bAllowNonPreferred) )      //output duplicate, return=2
	{
		// 'Swallow' the message, since its already been added.
		//Msg.Free();        //no longer needed since we now replace duplicate with current message

		pSeq = pNonPreferredBlk;

		if(nInsertMatchIndex != MATCH_NONE)
		{
			nMatchIndex = nInsertMatchIndex;
		}
		else
		{
			for(i=0; (i<(MAX_MULTI_BARCODES-1)) && (SEQUENCE_ENTRY_IS_NOT_EMPTY(pSeq)); i++)
			{
				if(pPrefBuff[i+1] != NULL)
				{
					nMatchIndex =  i+1;
					break;
				}
				/* move pSequence pointer to start of next entry */
				MOVE_TO_NEXT_ENTRY(pSeq);
			}
		}
	}
	else if ( nStatus == DUPLICATE )
	{
		// 'Swallow' the message, since its already been added.
		Msg.Free();
		pTrigger->ClearGoodReadTimeout();
		return -1;
	}
	// If failed to insert message, yet preferred timeout was started and non preferred is not allowed
	else if( (nStatus == NO_MATCH) && ( mPreferredStarted )  && (!bAllowNonPreferred) )
	{
		// Since preferred required, and we didn't get a match,
		// return -2 which indicates we 'swallowed' the  no match msg.
		Msg.Free();
		return -2;
	}
	else if( (nStatus == NO_MATCH) )   //if not in either preferred or non preferred list and preferred not started,
	{                                       // or preferred started and non preferred allowed and not in either list
		if( mPreferredStarted )
		{
			//Done, so get rid of timers
			ClearAndDeletePreferredTimers();
			// Clear the Preferred buffers
			ClearPreferredBuffer();
			//clear the allow non preferred flag
			bAllowNonPreferred = false;
			// indicate  buffer  is cleared
			//mPreferredStarted = false;  //do this on return now so decode buffer will get cleared (no retriggers)
		}
		// return FALSE so that the message gets output
		// using the normal output mechanism.
		return 0;

	}
	// At this point we do know that there is a sequence found.  So if we
	// have the first entry in the sequence (preferrd), then output.  If however, it isn't the priority entry
	// in the sequence, we have a time delay on when we can output it
	else if( (nStatus == FOUND_SEQUENCE) && (pPrefBuff[0] != NULL) )                    //output preferred, return=2
	{
		nMatchIndex = 0;
	}
	else if( (nStatus == MATCH) && (bAllowNonPreferred) )          //output non preferred match, return=2
	{
		pSeq = pNonPreferredBlk;

		if(nInsertMatchIndex != MATCH_NONE)
		{
			nMatchIndex = nInsertMatchIndex;
		}
		else
		{
			for(i=0; (i<(MAX_MULTI_BARCODES-1)) && (SEQUENCE_ENTRY_IS_NOT_EMPTY(pSeq)); i++)
			{
				if(pPrefBuff[i+1] != NULL)
				{
					nMatchIndex =  i+1;
					break;
				}

				/* move pSequence pointer to start of next entry */
				MOVE_TO_NEXT_ENTRY(pSeq);
			}
		}
	}
	else if( (nStatus == MATCH) )
	{
		pTrigger->ClearGoodReadTimeout();

		//not saving anymore, only read non preferred when the timeout has expired
		pPrefBuff[nInsertMatchIndex]->Free();
		pPrefBuff[nInsertMatchIndex] = NULL;
		return -1;
	}

	//Done, so get rid of timers
	ClearAndDeletePreferredTimers();

	//clear the allow non preferred flag
	bAllowNonPreferred = false;

	//clear the ID of the preferred barcode
	PreferredCodeID = 0;

	*piPreferredBarcodeIndex = nMatchIndex;           //return the match index
	CopyPreferredBarcodeData( Msg, nMatchIndex );     //lets store in original passed barcode data

	return 2;
}


//----------------------------------------------------------------------
// Search through desired barcode sequence menu table to see if the
// current message fits into the sequence.  If so insert the message
// into the decodedMsg_t array, otherwise discard the message.
//
// NOTE:  ALL sequence entries have the following format:
//		Byte 1:			Code ID
//		Byte 2:			A terminating 0xFF character.//
//
// Returns:
// 3 if it found the correct sequence
// 2 if a match is found
// 1 if a new entry was added
// 0 if main routine needs to handle data
// -1 if a duplicate was found
// -2 if no match was found
// -3 if non centered

//----------------------------------------------------------------------
INT32	COutputModes::InsertPreferredMessageIntoSequence( CBarcodeData &Msg, int* pnInsertMatchIndex )
{
	enum				{ MATCH_NONE = -1 };

	unsigned char 	NonPreferredSeqCodeID;
	unsigned char 	PreferredSeqCodeID;
	unsigned char *pNonPreferredSequence;              //non preferred codes
	unsigned char *pPreferredSequence;  						//preferred code ID

	bool				bSeqCharMatch;
	int				nMatchIndex;
	int				nMatchCode;

	unsigned char 	MsgCodeID;

	const tichar_t *pMsg;
	size_t 			nLen;
	int i;

	nMatchIndex = MATCH_NONE;
	nMatchCode = 0x00;
	bSeqCharMatch = false;
	*pnInsertMatchIndex = nMatchIndex;

	pNonPreferredSequence = pNonPreferredBlk;              //non preferred codes
	pPreferredSequence = pPreferredBlk;  						//preferred code ID

	Msg.GetTotalData(pMsg,nLen);

	/* Search the Sequence block for a possible duplicate message */
	for(i=0; (i<(MAX_MULTI_BARCODES-1)) && (SEQUENCE_ENTRY_IS_NOT_EMPTY(pNonPreferredSequence)); i++)
	{
		if(pPrefBuff[i+1] != NULL)
		{
			const tichar_t *Msgptr;
			size_t 			Len;
			pPrefBuff[i+1]->GetTotalData(Msgptr,Len);

			if( (Len == nLen) && (Msg.GetAimId() == pPrefBuff[i+1]->GetAimId()) &&  (0 == memcmp(pMsg,Msgptr,Len)) && (Msg.GetHHPId() == pPrefBuff[i+1]->GetHHPId()) )
			{
				pPrefBuff[ i+1 ]->Free();        //clear if repeat match
				pPrefBuff[ i+1 ] = NULL;
				pPrefBuff[ i+1 ] = &Msg;          //store latest copy
				return DUPLICATE;	// found a dupe entry exit
			}
		}

		MOVE_TO_NEXT_ENTRY( pNonPreferredSequence );
	}

	pNonPreferredSequence = pNonPreferredBlk;              //non preferred codes - reset pointer

	/* Search the Sequence block for a possible matching code ID */
	for(i=0; (i<(MAX_MULTI_BARCODES-1)) && (SEQUENCE_ENTRY_IS_NOT_EMPTY(pNonPreferredSequence)); i++)
	{
		// Test to see if a Msg with this code has already been added to the  array.
		// If it has, then delete it and add this one.

		// Copy this sequence's code ID to local
		NonPreferredSeqCodeID = *pNonPreferredSequence;
		PreferredSeqCodeID = *pPreferredSequence;
		MsgCodeID = (unsigned char)(Msg.GetHHPId());

		//move to next code ID
		MOVE_TO_NEXT_ENTRY(pNonPreferredSequence);
		bSeqCharMatch = false;

		/* Check CodeID match */

		if(MsgCodeID == PreferredSeqCodeID)
		{
			nMatchCode = 0x04;
			bSeqCharMatch = true;
			nMatchIndex = 0;
			break;
		}
		else if(MsgCodeID == NonPreferredSeqCodeID)
		{
			nMatchCode = 0x02;
			bSeqCharMatch = true;
			nMatchIndex = i+1;
			break;
		}
		else
		{
			nMatchCode = 0x01;
		}
	} /* end for loop */


	// DO THE MESSAGE INSERTION but don't insert
	//	message if no match in the table
	if(nMatchIndex != MATCH_NONE)
	{
		// if we are doing prioritized sequencing, we don't have the highest priority code (i.e. nMatchIndex != 0) and
		// we haven't set a timeout yet, then we set the timeout .
		if( (nMatchIndex != 0) )
		{
#ifdef LOW_PRIORITY_CENTERING
			LastDecodeInfo_t *pDecInfo;
			unsigned int nTriggerMode;   //lmc remove: no longer strictly presentation mode

			nTriggerMode = pTrigger->GetTriggerMode();  //lmc remove: no longer strictly presentation mode

			if( (nTriggerMode == STREAMING_PRESENTATION_TRIGGER) && (GET_CONFIG_SETTING(iOpticsPopulation)==TWO_D) )   //lmc remove : (nTriggerMode == STREAMING_PRESENTATION_TRIGGER) &&
			{
				pDecInfo = (LastDecodeInfo_t *)GetLastDecodeInfo();

				if( !LowPriorityCodeCentered(&(pDecInfo->corners[0])) )
				{
					return  NON_CENTERED;
				}
			}
#endif
			if (pPrioritySequenceTimeout == NULL)
			{
				pPrioritySequenceTimeout = new CTimeout( (ULONG)(iOutputModesPrioritySequenceTimeValue * 1000) );
			}
		}

		if(pPrefBuff[nMatchIndex] != NULL)          //clear if repeat match
		{
			pPrefBuff[ nMatchIndex ]->Free();
			pPrefBuff[ nMatchIndex ] = NULL;
		}

		pPrefBuff[ nMatchIndex ] = &Msg;           //save latest match
		*pnInsertMatchIndex = nMatchIndex;         //return the index
		mPreferredStarted = true;                  //indicate preferred started
	}

	if(bSeqCharMatch)                           //determine return type
	{
		if( (nMatchCode & 0x04) == 0x04 )
		{
			return FOUND_SEQUENCE;
		}
		else if( (nMatchCode & 0x02) == 0x02)
		{
			return MATCH;
		}
	}

	return NO_MATCH;
}


CBarcodeData& COutputModes::GetPreferredBarcodeData( int iPreferredBarcodeIndex )
{
	return *pPrefBuff[iPreferredBarcodeIndex];
}


void COutputModes::SetPreferredBarcodeDataNull( int iPreferredBarcodeIndex )
{
	pPrefBuff[iPreferredBarcodeIndex] = NULL;
}


#ifdef LOW_PRIORITY_CENTERING
// fAngle() returns the angular direction from a to b between -179.99 and +180.0, with North = 0.0

float COutputModes::fAngle (Point a, Point b)
{
	float A = -1, dx, dy;
	dx = (float)(b.x - a.x);
	dy = (float)(b.y - a.y);

	if ((dx) || (dy))
	{
		if (abs((int)dx) > abs((int)dy))
		{
			if (dx > 0) /* Eastward */
			{
				A = (float)(atan(dy/dx)*180.0/3.14159 + 90.0);
			}
			else /* Westward */
			{
				A = (float)(atan(dy/dx)*180.0/3.14159 - 90.0);
			}
		}
		else
		{
			if (dy > 0) /* Southward */
			{
				A = (float)(180.0 - atan(dx/dy)*180.0/3.14159);
			}
			else /* Northward */
			{
				A = (float)(-atan(dx/dy)*180.0/3.14159);
			}
		}
	}
	if (A > 180.0)
	{
		A -= 360.0;
	}

	return (A);
}


// we use this to find out if the lines made by the code surround the center.
bool COutputModes::LowPriorityCodeCentered(Point *corners)
{
	int i;
	Point Ctr,tmpCorners[4];
	float fAngles[4],nTemp;
	Ctr.y = GET_CONFIG_SETTING(iImageYCenter);
	Ctr.x = GET_CONFIG_SETTING(iImageXCenter);

	for (i=0;i<4;i++)
	{
		tmpCorners[i].x = corners[i].x>>1;
		tmpCorners[i].y = corners[i].y>>1;
	}

	for (i=0;i<4;i++)
	{
		fAngles[i] = fAngle(Ctr,tmpCorners[i]);
	}

	for (i=0;i<3;i++)
	{
		for (int j=i+1;j<4;j++)
		{
			if (fAngles[j] > fAngles[i])
			{
				nTemp = fAngles[j];
				fAngles[j] = fAngles[i];
				fAngles[i] = nTemp;
			}
		}
	}

	if (((fAngles[0] - fAngles[3]) > 180.0) &&
	        ((fAngles[0] - fAngles[1]) < 180.0) &&
	        ((fAngles[1] - fAngles[2]) < 180.0) &&
	        ((fAngles[2] - fAngles[3]) < 180.0))
	{
		return TRUE;
	}

	return FALSE;
}
#endif         //end  #ifdef LOW_PRIORITY_CENTERING


bool COutputModes::SequencePriorityTimeoutExpired(void)
{
	if (pPrioritySequenceTimeout != NULL)
	{
		return pPrioritySequenceTimeout->HasExpired();
	}
	return FALSE;
}


void COutputModes::SequencePriorityTimeoutRefresh(void)
{
	if( (pPrioritySequenceTimeout != NULL) )
	{                                                                                  //if timer not expired and if
		if( (!pPrioritySequenceTimeout->HasExpired()) && (PreferredCodeID == 0x72) )    //pdf is preferred, then allow timer refresh
		{
			pPrioritySequenceTimeout->SetExpiration( (ULONG)(iOutputModesPrioritySequenceTimeValue * 1000) );
		}
	}
}


void COutputModes::ClearPreferredBuffer(void)
{
	int i;
	//clear the buffer
	for (i=0; i<MAX_MULTI_BARCODES;i++)
	{
		if( pPrefBuff[i] != NULL )
		{
			pPrefBuff[i]->Free();
			pPrefBuff[i] = NULL;
		}
	}

	PreferredCodeID = 0;
}


void COutputModes::ClearAndDeletePreferredTimers(void)
{
	//clear and delete the read before clear timeout
	if (pPriorityReadTimeout != NULL)
	{
		delete pPriorityReadTimeout;
		pPriorityReadTimeout = NULL;
	}
	//clear and delete the priority timeout
	if (pPrioritySequenceTimeout != NULL)
	{
		delete pPrioritySequenceTimeout;
		pPrioritySequenceTimeout = NULL;
	}
}


void COutputModes::UpdatePreferredTimers(void)
{
	int ClearTime = 0;
	int ClearTimerTime = 0;

	if ( (pPrioritySequenceTimeout != NULL) )          //if priority has been started and timer created
	{
		// if it's been a long time since we got the last entry into the priority sequence table, we clear the table.  A long time
		// being twice the normal timeout.  By normal timeout I mean every timeout associated with a read.
		ClearTime = 0;
		if( ClearTime < (iOutputModesGoodReadDly * 1000) )			ClearTime = ( iOutputModesGoodReadDly * 1000 );
		if( ClearTime < (iOutputModesReReadDly * 1000) )			ClearTime = ( iOutputModesReReadDly * 1000 );
		if( ClearTime < (iOutputModesReReadDly2D * 1000) )			ClearTime = ( iOutputModesReReadDly2D * 1000 );

		//300ms + 1/5 of priority time + largest read delay
		ClearTimerTime = ((int)((iOutputModesPrioritySequenceTimeValue) * (1000 / 5))) + ClearTime + 300000;

		if( (pPriorityReadTimeout == NULL) && (pPrioritySequenceTimeout->HasExpired()) )    //start clear timer
		{
			pPriorityReadTimeout = new CTimeout( (ULONG)(ClearTimerTime) );

			//allow non preferred output since priority timeout has expired
			bAllowNonPreferred = true;
		}

		if ( (pPriorityReadTimeout != NULL) )          //if priority clear has been started and timer created
		{
			if( pPriorityReadTimeout->HasExpired() )       //if priority clear timer is expired
			{
				// Clear the Preferred buffers
				ClearPreferredBuffer();

				// indicate  buffer  is cleared
				mPreferredStarted = false;

				// Dump the read before clear timeout
				// Dump the priority timeout
				ClearAndDeletePreferredTimers();

				//do not allow non preferred output
				bAllowNonPreferred = false;
			}
		}
	}
}



//The Sequence method section
//
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//	DoSequence
//
//	Checks to see if a barcode message fits into the desired output sequence.
// If it fits, it inserts it appropriately.
//
// Also, if the sequence is complete, this routine goes ahead and outputs
// the data to the standard interface and does a good read beep.
//
// Returns 2 if routine output a compltete sequence
// 1 if a new entry was added
// 0 if main routine needs to handle datta
// -1 if a duplicate was found
// -2 if no match was found (sequence required or sequence started).
//--------------------------------------------------------------------------
int  COutputModes::DoSequence( CBarcodeData& Msg, int* piSequenceBarcodeCount )
{
	int i = 0;
	int j = 0;

	BYTE *pSeq;
	bool bSeqFull = TRUE;
	int iMultiBarcodeCount = 0;

	pSeq = pSequenceBlk;

	// If not menued for sequence then bail out now and
	// tell the caller to output the message normally. Also check to see if sequence block is empty.
	if( (nSequenceMode == SEQUENCE_DISABLED) || !(SEQUENCE_ENTRY_IS_NOT_EMPTY(pSeq)) )
	{
		return 0;
	}

	// If failed to insert message, yet sequence was REQUIRED,
	// then Houston, we have a problem!
	INT32 nStatus = InsertMessageIntoSequence( Msg );


	if( nStatus == DUPLICATE )
	{
		// 'Swallow' the message, since its already been added.
		Msg.Free();
		return -1;
	}

	if( nStatus == NO_MATCH )
	{
		if( (nSequenceMode == SEQUENCE_REQUIRED) || (mSequenceStarted == true) )
		{
			// Since sequence required, and we didn't get a match,
			// return TRUE which indicates we 'swallowed' the msg.
			Msg.Free();
			return -2;
		}
		else
		{
			// return FALSE so that the message gets output
			// using the normal output mechanism.
			return 0;
		}
	}

	// At this point, we must have inserted a new message into the sequence
	// output buffer.  Therefore, check if Sequence requirements are fully met
	pSeq = pSequenceBlk;
	bSeqFull = TRUE;

	j = 0;
	iMultiBarcodeCountPartial = 0;

	for(i=0; (i<MAX_MULTI_BARCODES) && (SEQUENCE_ENTRY_IS_NOT_EMPTY(pSeq)); i++)
	{
		if(pSeqBuff[i] == NULL)    //this barcode in seq. not read yet
		{
			bSeqFull = FALSE;
		}

		else     //if seq. buf. not equiv. to null (barcode read)
		{
			//If enable partial sequence mode,it will store partial sequence of barcodes into Tempbuffer(pSeqBuffPartial[])
			if( nPartialSequenceMode )
			{
				iMultiBarcodeCountPartial++;

				//partial unrankded sequence
				pSeqBuffPartial[j] = pSeqBuff[i];
				j++;
			}
		}

		/* move pSequence pointer to start of next entry */
		MOVE_TO_NEXT_ENTRY(pSeq);
	}

	if (bSeqFull)
	{
		pSeq = pSequenceBlk;

		iMultiBarcodeCount = 0;

		// Output the Sequence
		for(i=0; (i<MAX_MULTI_BARCODES) && (SEQUENCE_ENTRY_IS_NOT_EMPTY(pSeq)); i++)
		{
			MOVE_TO_NEXT_ENTRY( pSeq );		// move to next entry
			iMultiBarcodeCount++;           //find the number of entries
		}

		*piSequenceBarcodeCount = iMultiBarcodeCount;

		return 2;
	}
	else
	{
		// If this message was inserted, yet sequence is not full,
		// then simply give the user a click sound.
		Click();
		return 1;
	}

	// If we got this far, then sequence processing did indeed
	// 'swallow' the message, so indicate appropriately to caller.
}



//----------------------------------------------------------------------
//
// Search through desired barcode sequence menu table to see if the
// current message fits into the sequence.  If so insert the message
// into the decodedMsg_t array, otherwise discard the message.
//
// NOTE:  ALL sequence entries have the following format:
//		Byte 1:			Code ID
//		Byte 2,3:		Message Length (in Binary Coded decimal)
//		Byte 4 to n-1:	A string of message match chars.
//		Byte n:			A terminating 0xFF character.
//
// PRIORITY OF message insertion:
//		Matching ID, Matching Len, Matching string
// 	Matching ID, Matching Len, Univ string
//		Matching ID, Univ Len, Matching string
//		Matching ID, Univ Len, Univ String
//		Univ ID, Matching Len, Matching string
// 	Univ ID, Matching Len, Univ string
//		Univ ID, Univ Len, Matching string
//		Univ ID, Univ Len, Univ String
//
//	nLen = length of current gmsg to insert into sequence.
//----------------------------------------------------------------------
INT32	COutputModes::InsertMessageIntoSequence( CBarcodeData &Msg )
{
	enum				{ MATCH_NONE = -1 };
	int				nMatchIndex = MATCH_NONE;
	unsigned char 	SeqCodeID;
	unsigned int 	SeqLength;
	unsigned char *pSequence;
	size_t			nDataElements;
	bool				bSeqCharMatch;
	int				nMatchCode;
	int				nBestMatchCode = 0;
	const tichar_t *pMsg;
	size_t 			nLen;

	pSequence = pSequenceBlk;
	Msg.GetTotalData(pMsg,nLen);

	for(int i=0; (i<MAX_MULTI_BARCODES) && (SEQUENCE_ENTRY_IS_NOT_EMPTY(pSequence)); i++)
	{
		if( pSeqBuff[i]!= NULL)
		{
			const tichar_t *Msgptr;
			size_t 			Len;
			pSeqBuff[i]->GetTotalData(Msgptr,Len);

			if( (Len == nLen) && (Msg.GetAimId() == pSeqBuff[i]->GetAimId()) &&  (0 == memcmp(pMsg,Msgptr,Len)) )
			{
				return DUPLICATE;	// found a duplicate entry, exit
			}
		}
		MOVE_TO_NEXT_ENTRY( pSequence );
	}

	pSequence = pSequenceBlk; // point to start of sequence again

	/* Search the Sequence block for a possible matching entry */
	for(int i=0; (i<MAX_MULTI_BARCODES) && (SEQUENCE_ENTRY_IS_NOT_EMPTY(pSequence)); i++)
	{
		/* Test to see if this Msg has already been added to the  array.
		// If it has, then we'll skip it so the same decoded msg doesn't get
		//	added twice.  */
		nDataElements = 0;

		if( pSeqBuff[i]!= NULL)
		{
			MOVE_TO_NEXT_ENTRY( pSequence );
			continue;
		}

		SeqCodeID  =  pSequence[0];
		SeqLength  = (pSequence[1] >> 4)  * 1000;
		SeqLength += (pSequence[1] & 0xf) * 100;
		SeqLength += (pSequence[2] >> 4)  * 10;
		SeqLength += (pSequence[2] & 0xf);

		/* Copy this sequence's character string to SeqChars */
		pSequence += 3;
		nDataElements = 0;
		bSeqCharMatch = true;
		bool MatchedUniChar = false;

		while(SEQUENCE_ENTRY_IS_NOT_EMPTY(pSequence))
		{
			if (*pSequence != pMsg[nDataElements])
			{
				if (*pSequence == UNIVERSAL_CHAR)
					MatchedUniChar = true;
				else
					bSeqCharMatch = false;
			}
			nDataElements++;
			pSequence ++;
		}

		pSequence ++;

		/* Check against message for match, enforcing matching priority
		// Setup a codeword that codes the priority (larger # indicates
		// higher priority. */
		/* Check character match */
		if(bSeqCharMatch)
		{
			if (!MatchedUniChar)
			{
				nMatchCode = 0x20;
			}
			else
			{
				nMatchCode = 0x10;
			}
		}
		else
		{
			nMatchCode = 0;
		}

		/* If got char match, check length match */
		if(nMatchCode > 0)
		{
			if(nLen == SeqLength)
			{
				nMatchCode += 0x08;
			}
			else if(SeqLength == UNIVERSAL_CODE_LENGTH)
			{
				nMatchCode += 0x04;
			}
			else
			{
				nMatchCode = 0;
			}
		}

		/* If got char & length match check CodeID match */
		if(nMatchCode > 0)
		{
			if(Msg.GetHHPId() == SeqCodeID)
			{
				nMatchCode += 0x02;
			}
			else if(SeqCodeID == UNIVERSAL_WA_ID)
			{
				nMatchCode += 0x01;
			}
			else
			{
				nMatchCode = 0;
			}
		}

		/* Now check this sequence entry's codeword against the previous
		// best code word for this pMsg.  If this codeword is higher then
		// we have a new leading candidate */
		if(nMatchCode > nBestMatchCode)
		{
			nBestMatchCode = nMatchCode;
			nMatchIndex = i;
		}
	} /* end for loop */

	// DO THE MESSAGE INSERTION but don't insert
	//	message if no match in the table
	if(nMatchIndex != MATCH_NONE)
	{
		pSeqBuff[ nMatchIndex ] = &Msg;
		mSequenceStarted = true;
		///ewr05430
		if( pTrigger->GetTriggerMode() == TRGMOD_PRESENT || pTrigger->GetTriggerMode() == TRGMOD_STREAM_PRESENT )
		{
			pTrigger->SetSerialTriggerTimeout();
		}
		return MATCH;
	}
	return NO_MATCH;
}


CBarcodeData& COutputModes::GetSequenceBarcodeData( int iSequenceBarcodeIndex )
{
	return *pSeqBuff[iSequenceBarcodeIndex];
}


void COutputModes::SetSequenceBarcodeDataNull( int iSequenceBarcodeIndex )
{
	pSeqBuff[iSequenceBarcodeIndex] = NULL;
}


int COutputModes::GetSequenceBarcodeDataPartialSize( void )
{
	return iMultiBarcodeCountPartial;   //used in partial sequence mode
}


void COutputModes::ClearSequenceBarcodeDataPartialSize( void )
{
	iMultiBarcodeCountPartial = 0;   //used in partial sequence mode
}


CBarcodeData& COutputModes::GetSequenceBarcodeDataPartial( int iSequenceBarcodeIndex )
{
	return *pSeqBuffPartial[iSequenceBarcodeIndex];   //used in partial sequence mode
}


void COutputModes::SetSequenceBarcodeDataPartialNull( int iSequenceBarcodeIndex )
{
	pSeqBuffPartial[iSequenceBarcodeIndex] = NULL; //used in partial sequence mode
}


void COutputModes::ClearSequenceBufferPartial(void)
{
	int i;
	//clear the buffer
	for (i=0; i<MAX_MULTI_BARCODES;i++)
	{
		if( pSeqBuffPartial[i] != NULL )
		{
			pSeqBuffPartial[i]->Free();
			pSeqBuffPartial[i]=NULL;
		}
	}

	iMultiBarcodeCountPartial = 0;
}


void COutputModes::ClearSequenceBuffer(void)
{
	int i;
	//clear the buffer
	for (i=0; i<MAX_MULTI_BARCODES;i++)
	{
		if( pSeqBuff[i] != NULL )
		{
			pSeqBuff[i]->Free();
			pSeqBuff[i]=NULL;
		}
	}
}


bool COutputModes::SetSequenceStartedIfRequired(void)
{
	if( nSequenceMode==2 ) // if sequence required dont untrigger unless sequence complete (fake sequence already started)
	{
		mSequenceStarted = true;
	}
	else
	{
		mSequenceStarted = false;
	}
	return mSequenceStarted;
}


bool COutputModes::IsSequenceStarted(void)
{
	bool CheckForSequenceStarted = false;
	BYTE *pSeq = NULL;

	pSeq = pSequenceBlk;

	if( mSequenceStarted && (SEQUENCE_ENTRY_IS_NOT_EMPTY(pSeq)) )
	{
		CheckForSequenceStarted = true;
	}

	return CheckForSequenceStarted;
}


//----------------------------------------------------------------------------

//Barcode copy and storage back to original message:
//----------------------------------------------------------------------------
bool COutputModes::CopyPreferredBarcodeData( CBarcodeData& Msg, int iPreferredBarcodeIndex )
{
	int j;
	CBarcodeData* pTheMessage;
	const tichar_t *pTheMessageData;
	size_t 			nTheMessageLen;
	const tichar_t *pMsg;
	size_t 			nLen;

	tichar_t	TheMessageHHPid;
	tichar_t	TheMessageAIMid;
	tichar_t	MsgHHPid;
	tichar_t	MsgAIMid;

	pTheMessage = &Msg;

	if( (pPrefBuff[iPreferredBarcodeIndex] == NULL) )
	{
		//theStdInterface.printf( " Barcode copy fails " );      																		//lmc debug
		return false;
	}

	//remove all other symbologies from buffer except one we are sending
	for (j=0; j<MAX_MULTI_BARCODES;j++)
	{
		if( (pPrefBuff[j] == pTheMessage) || (iPreferredBarcodeIndex == j) )  //if THIS pointer matches or barcode data to copy, then leave it
		{
			continue;
		}
		if(pPrefBuff[j] != NULL)
		{
			pPrefBuff[j]->Free();
			pPrefBuff[j] = NULL;
		}
	}

	if( (pPrefBuff[iPreferredBarcodeIndex] == pTheMessage) )     //we already match
	{
		//theStdInterface.printf( " Barcode copy not done " );      																//lmc debug
		//theStdInterface.Write( pTheMessage );         						 														//lmc  debug
		return true;
	}
	else                                                        //else lets copy it to current passed barcode data
	{
		pPrefBuff[iPreferredBarcodeIndex]->GetTotalData(pMsg,nLen);
		MsgAIMid = pPrefBuff[iPreferredBarcodeIndex]->GetAimId();
		MsgHHPid = pPrefBuff[iPreferredBarcodeIndex]->GetHHPId();

		pTheMessage->CopyTypeInfo( pPrefBuff[iPreferredBarcodeIndex] );
		pTheMessage->MoveBuffer( pPrefBuff[iPreferredBarcodeIndex] );

		pTheMessage->GetTotalData(pTheMessageData,nTheMessageLen);
		TheMessageAIMid = pTheMessage->GetAimId();
		TheMessageHHPid = pTheMessage->GetHHPId();

		pPrefBuff[iPreferredBarcodeIndex]->Free();
		pPrefBuff[iPreferredBarcodeIndex] = NULL;

		if( (nTheMessageLen == nLen)  &&  (0 == memcmp(pMsg, pTheMessageData, nTheMessageLen)) && (MsgAIMid == TheMessageAIMid) && (MsgHHPid == TheMessageHHPid) )
		{
			//theStdInterface.printf( " Barcode copy match " );      																	 	//lmc debug
			//theStdInterface.Write( pTheMessage );         						 									 						//lmc  debug
			return true;
		}
	}
	//theStdInterface.printf( " Barcode copy not match " );      																	 	//lmc debug
	return false;
}
//----------------------------------------------------------------------------



