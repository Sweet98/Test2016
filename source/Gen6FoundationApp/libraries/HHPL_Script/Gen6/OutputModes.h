//======================================================================================
// Outputmodes.h
//======================================================================================
// $RCSfile: Gen6/OutputModes.h $
// $Revision: 1.9 $
// $Date: 2011/11/11 17:01:40EST $
//======================================================================================

//======================================================================================

#ifndef __OUTPUT_MODES_H
#define __OUTPUT_MODES_H

//#define LOW_PRIORITY_CENTERING		//define only if you wish to use low priority centering

#include "stdInclude.h"
#include "Timeout.h"
#include "decodeiface.h"

class HSTRING;
class CBarcodeData;
class CBarcodeFifo;



class COutputModes
{
private:

#define	USE_LARGER_BUFFERS	1                      //lmc
	// This is just a guess.  Change this if you want to have more than
	// 5 possible barcodes as part of your sequence output.  If this
	// changes, please also change the sequence output buffer initializations
	// below.  Currently we initialize 5 locations.
#ifndef	USE_LARGER_BUFFERS
	#define	MAX_MULTI_BARCODES	5
#else
	#define	MAX_MULTI_BARCODES	15
#endif

	typedef CBarcodeData* BarcodeDataPointerArray[MAX_MULTI_BARCODES];
	//BarcodeDataPointerArray* ptr;

	// This is the  output buffer blocks of ram.
	BarcodeDataPointerArray  pSeqBuff;		//CBarcodeData *	pSeqBuff[ MAX_MULTI_BARCODES ];
	BarcodeDataPointerArray  pPrefBuff;		//CBarcodeData *	pPrefBuff[ MAX_MULTI_BARCODES ];

	BarcodeDataPointerArray  pSeqBuffPartial;  //sequence partial output mode buffer

	bool CopyPreferredBarcodeData( CBarcodeData& Msg, int iPreferredBarcodeIndex );

protected:
	COutputModes(void);		// protected constructor to force people to use the create class factory function.

	bool mSequenceStarted;
	bool mPreferredStarted;

	bool bCtsManualMode;


public:
	~COutputModes(void);

	void OnChangedConfiguration(void);	// gets called by the framework after a menu change

	static void CreateOutputModes(void);   //class factory
	static void DestroyOutputModes(void);

	bool IsAnySequenceStarted(void) {return (mSequenceStarted || mPreferredStarted);};      //combined flags for sequence and priority
	bool IsAllSequenceStopped(void) {return ((!mSequenceStarted) && (!mPreferredStarted));};

	//The prefered method section
	bool IsPreferredStarted(void) {return mPreferredStarted;};         //flag checks
	bool IsPreferredStopped(void) {return !mPreferredStarted;};
	bool IsPreferredModeOff(void) {return (!bPreferredMode);};
	bool IsPreferredModeOn(void) {return (bPreferredMode);};

	int  DoPreferred( CBarcodeData& Msg, int* piPreferredBarcodeIndex );    //main methods
	INT32	InsertPreferredMessageIntoSequence( CBarcodeData &Msg, int* pnInsertMatchIndex );
	CBarcodeData& GetPreferredBarcodeData( int iPreferredBarcodeIndex );
	void SetPreferredBarcodeDataNull( int iPreferredBarcodeIndex );

#ifdef LOW_PRIORITY_CENTERING
	float COutputModes::fAngle(Point a, Point b);         //centering stuff
	bool LowPriorityCodeCentered(Point *corners);
#endif

	bool SequencePriorityTimeoutExpired(void);          // Check to see if our priority sequence timeout has expired
	void SequencePriorityTimeoutRefresh(void);          // refresh the priority timeout
	void SetPreferredStartedOff(void) { mPreferredStarted = false;};  // set started flag to false
	void ClearAllowNonPreferredFlag(void) { bAllowNonPreferred = false;};  // set non preferred allowed flag to false
	void ClearPreferredBuffer(void);
	void ClearAndDeletePreferredTimers(void);
	void UpdatePreferredTimers(void);

	//The Sequence method section
	bool IsSequenceStarted(void);							           //flag checks
	bool IsSequenceStopped(void) {return !mSequenceStarted;};
	bool IsSequenceModeOff(void) {return (0==nSequenceMode);};
	bool IsSequenceModeOn(void) {return (1==nSequenceMode);};
	bool IsSequenceModeRequired(void) {return (2==nSequenceMode);};
	bool IsSequenceModeEnabled(void) {return ((1==nSequenceMode) || (2==nSequenceMode));};

	int  DoSequence( CBarcodeData& Msg, int* piSequenceBarcodeCount );    //main methods
	INT32	InsertMessageIntoSequence( CBarcodeData &Msg );
	CBarcodeData& GetSequenceBarcodeData( int iSequenceBarcodeIndex );
	void SetSequenceBarcodeDataNull( int iSequenceBarcodeIndex );

	bool SetSequenceStartedIfRequired(void);  // set started flag if mode is required
	void SetSequenceStartedOff(void) { mSequenceStarted = false;};  // set started flag to false
	void ClearSequenceBuffer(void);

	//The Partial Sequence method section
	bool IsSequenceBarcodeDataPartialEnabled( void ) {return (bool)nPartialSequenceMode;};         //used in partial sequence mode
	int GetSequenceBarcodeDataPartialSize( void );
	void ClearSequenceBarcodeDataPartialSize( void );
	CBarcodeData& GetSequenceBarcodeDataPartial( int iSequenceBarcodeIndex );
	void SetSequenceBarcodeDataPartialNull( int iSequenceBarcodeIndex );
	void ClearSequenceBufferPartial(void);
	INT32	nSequenceMode;  //NG2D-712
	bool	bPreferredMode; //NG2D-712



private:
	//The Sequence variable section
	//INT32	nSequenceMode;
	BYTE  * pSequenceBlk;
	int iMultiBarcodeCountPartial;

	//The prefered variable section
	//bool	bPreferredMode;
	BYTE  * pPreferredBlk;
	BYTE  * pNonPreferredBlk;
	CTimeout	* pPrioritySequenceTimeout;
	int iOutputModesPrioritySequenceTimeValue;
	CTimeout	* pPriorityReadTimeout;
	bool bAllowNonPreferred;
	unsigned char 	PreferredCodeID;

	//Generic variable section
	int iOutputModesGoodReadDly;
	int iOutputModesReReadDly;
	int iOutputModesReReadDly2D;

	//The partial Sequence variable section
	INT32	nPartialSequenceMode;
private:
	COutputModes (const COutputModes&);            // declared private, but not defined, to prevent copying         
	COutputModes& operator=(const COutputModes&);  // declared private, but not defined, to prevent copying 

};

//Global pointer to class factory instance
extern COutputModes *g_pOutputModes;
#define theOutputModes	(*g_pOutputModes)

#endif      //	__OUTPUT_MODES_H

