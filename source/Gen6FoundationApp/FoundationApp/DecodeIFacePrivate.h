
/*----------------------------------------------------------------------*
 *	       DecodeIfacePrivate.h is the interface between the decoder and	*
 *	       the function that calls the decoder.									*
 *        This file holds private declarations.									*
 *			 Do not use as an include file, except for DecodeIFace.			*
 *----------------------------------------------------------------------*
 *======================================================================*
 *   $RCSfile: Gen6FoundationApp/FoundationApp/DecodeIFacePrivate.h $
 *   $Date: 2011/01/28 14:47:16EST $
 *   $Revision: 1.14 $
 *
 *======================================================================*

 *======================================================================*/
#ifndef DECODEIFACEPRIVATE_H
#define DECODEIFACEPRIVATE_H


//#define RUN_API_TEST_STUBS	1				//LMC   	Run the test stubs when you do not have a trigger, to test, comment out to run normal




#define	TRIGGER_5_5		1
#ifdef TRIGGER_5_5
#include "trigger_5_5.h"                                         //lmc   fixme
#else
#include "trigger.h"
#endif	//end #ifdef TRIGGER_5_5

//***********************************************************************
//debugging and build options
//***********************************************************************

#define MOTION FALSE // 0 or 1 or 2 (with diagnostics) Andy's Motion Sensivity Testing


//***********************************************************************
//defines
//***********************************************************************


// this code lets us read 3800 menus for testing
// still need to decide on menu bar code format
#define PRODUCT_MENU_ID_1	'0'
#define PRODUCT_MENU_ID_2	'1'

/* The Generic Menu ID */
#define GENERIC_MENU_ID_1	'0'
#define GENERIC_MENU_ID_2	'0'


//Defines for IsNewMessage( void )
#define PREVIOUS_READS_TO_REMEMBER 10


//Defines for Shotgun()
#define	DEFAULT_CODEID 		0
#define	MAX_SHOTGUN_HISTORY	20


//***********************************************************************
//externs
//***********************************************************************

// Routines which route diagnostic output to the correct interface (from PlugAndPlayMenuSettings.cpp).
extern void SendStringToInterface( const char *pucBuffer, int SizeOfBuffer);

extern UINT volatile BaselineClock; // a 1 ms granularity Timer for use by the Decoder (and others?!)

extern int GetCurrentHostInterface(void);

extern void showSudoku(unsigned char *ImageBuffer, int wid, int hgt, char *BcMessage);

// spd
// ImgBox Assessor functions -- may grow -- for now, this is the only functionality that needs retaining information about a given decode and image combination to
// be handled at a later unpredicable time.
//extern void DumpLastDecodeStructure(void);                             //lmc   fixme      Code not currently in our project

//***********************************************************************
//Private Structure Declarations
//***********************************************************************


//Structs for IsNewMessage( void )
typedef struct
{
	int Length;
	char HHPCodeID;
	int Checksum;
	int WeightedChecksum;
	unsigned int TimeStamp;
} MessageProfile_t;



//***********************************************************************
//Private Function Declarations
//***********************************************************************


// Get pointer to decoder workspace.
void * GetWorkspacePointer(void);
void * GetImagePointer(void);

void PowerOnInitLastDecodeInfo(void);
void StoreLastDecodeInfo (DecodeResult *msg);
void *GetLastDecodeInfo (void);
void FreeDecoderWorkspace(void);
void SetUpDecoder(int Width, int Height);

//Set the Image Quality according to interface
int ImageQualityMode(void);

//Set the Aux decoder according to interface
int AuxiliaryDecoder(void);



//***********************************************************************
//Private Inline Functions
//***********************************************************************


//API Interface to the Trigger class
#ifdef RUN_API_TEST_STUBS

	inline int GetTriggerMode() { return 0;/* (int)pTrigger->GetTriggerMode();*/ };                          //lmc   fixme

	inline bool IsWaitingForScanStand() { return 0;/* pTrigger->IsWaitingForScanStand();*/ };                     //lmc   fixme

	inline void ResetGoodReadTime() {/* pTrigger->ResetGoodReadTime();*/ };                                //lmc   fixme

	inline bool HasGoodReadTimeoutExpired() { return 1;/* pTrigger->HasGoodReadTimeoutExpired();*/ };                //lmc   fixme

	inline bool IsUnitTriggered() { return 0;/* pTrigger->IsUnitTriggered();*/ };                                //lmc   fixme

	inline void PrsModesFoundPDF( int iFoundType ) {/* pTrigger->PrsModesFoundPDF( iFoundType );*/ };             //lmc   fixme

	inline bool PrsModesPDFdetected() { return 0;/* pTrigger->PrsModesPDFdetected();*/ };                      //lmc   fixme

	inline int GetTrgModeModFlag() { return 0;/* pTrigger->GetTrgModeModFlag();*/ };        //lmc   fixme, this stuff not in our trigger model

#else

	inline int GetTriggerMode() { return (int)pTrigger->GetTriggerMode(); };

	inline bool IsWaitingForScanStand() { return pTrigger->IsWaitingForScanStand(); };

	inline void ResetGoodReadTime() { pTrigger->ResetGoodReadTime(); };

	inline bool HasGoodReadTimeoutExpired() { return pTrigger->HasGoodReadTimeoutExpired(); };

	inline bool IsUnitTriggered() { return pTrigger->IsUnitTriggered(); };

	inline void PrsModesFoundPDF( int iFoundType ) { pTrigger->PrsModesFoundPDF( iFoundType ); };

	inline bool PrsModesPDFdetected() { return pTrigger->PrsModesPDFdetected(); };

	inline int GetTrgModeModFlag() { return pTrigger->GetTrgModeModFlag(); };

#endif


/************************************************************************/
/************************************************************************/



//***********************************************************************
#endif		//end #ifndef DECODEIFACEPRIVATE_H


