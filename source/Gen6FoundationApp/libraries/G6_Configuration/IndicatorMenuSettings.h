//======================================================================================
// IndicatorMenuSettings.h
//======================================================================================
// $RCSfile: IndicatorMenuSettings.h $
// $Revision: 1.13 $
// $Date: 2011/09/02 17:28:39EDT $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Module Interface Description:
//      This file contains the interface definition to a routine that creates
//		indicator menu settings as part of the overall reader configuration for
//		a Matrix device.  It also contains the active value declarations of
//		indicator settings.
//
//---------------------------------------------------------------------------


#ifndef IndicatorMenuSettingsH
#define IndicatorMenuSettingsH
//---------------------------------------------------------------------------


#include	"language.h"					// system global definitions


#define	BEEP_OFF				0
#define	BEEP_LO				1
#define	BEEP_MED				2
#define	BEEP_HI				3

bool		bBelBeepEnable;
bool        bTrigBeepEnable;
bool 	bPowerUpBeepEnable;
int		BeepVolume;
int		BeepOnRead;
bool		GoodReadLEDMode;
bool		GoodReadBeepMode;
int		GoodReadFrequency;
int		BeepOnError;
int		ErrorFrequency;
int		BeepOnDecode;
int		BeepOnNoDecode;
bool		BeepBipMode;
bool		bLEDPowerOnEnable;  
bool     	 BeepOnPaging;
int       	PageTriggerDetection;
bool        EnablePagingMode;

bool illumON;

bool m_bEnableGoodReadTfb;
int m_TfbDuration;

#ifdef RFBASE
int HeapPwmLedDim;
#endif


// Host Ack
bool	bEnableHostAck;
int 	iHostAckTimeout;

#ifdef RFBASE

bool 	BASE_SCANNER_SELECT_bPowerUpBeepEnable;
int		BASE_SCANNER_SELECT_BeepVolume;
int		BASE_SCANNER_SELECT_BeepOnRead;
bool		BASE_SCANNER_SELECT_GoodReadLEDMode;
bool		BASE_SCANNER_SELECT_GoodReadBeepMode;
int		BASE_SCANNER_SELECT_GoodReadFrequency;
int		BASE_SCANNER_SELECT_BeepOnError;
int		BASE_SCANNER_SELECT_ErrorFrequency;
int		BASE_SCANNER_SELECT_BeepOnDecode;
int		BASE_SCANNER_SELECT_BeepOnNoDecode;
bool		BASE_SCANNER_SELECT_BeepBipMode;
bool        BASE_SCANNER_SELECT_bLEDPowerOnEnable; 
bool        BASE_SCANNER_SELECT_PageTriggerDetection;
bool        BASE_SCANNER_SELECT_EnablePagingMode;
#else

#define   BASE_SCANNER_SELECT_bPowerUpBeepEnable   				bPowerUpBeepEnable
#define	BASE_SCANNER_SELECT_BeepVolume							BeepVolume
#define	BASE_SCANNER_SELECT_BeepOnRead							BeepOnRead
#define	BASE_SCANNER_SELECT_GoodReadLEDMode						GoodReadLEDMode
#define	BASE_SCANNER_SELECT_GoodReadBeepMode					GoodReadBeepMode
#define	BASE_SCANNER_SELECT_GoodReadFrequency					GoodReadFrequency
#define	BASE_SCANNER_SELECT_BeepOnError							BeepOnError
#define	BASE_SCANNER_SELECT_ErrorFrequency						ErrorFrequency
#define	BASE_SCANNER_SELECT_BeepOnDecode							BeepOnDecode
#define	BASE_SCANNER_SELECT_BeepOnNoDecode						BeepOnNoDecode
#define	BASE_SCANNER_SELECT_BeepBipMode				  			BeepBipMode
#define   BASE_SCANNER_SELECT_bLEDPowerOnEnable					bLEDPowerOnEnable
#define   BASE_SCANNER_SELECT_PageTriggerDetection					PageTriggerDetection
#define   BASE_SCANNER_SELECT_EnablePagingMode                                 EnablePagingMode
#endif
int           PagingBeepFrequency;
int 		BeepDuration;
bool 		GoodReadLEDPowerOnIndicator;


void CreateIndicatorMenuSettings( int ControlMask );


#endif // IndicatorMenuSettingsH
//----------------------------------------------------------------------------




