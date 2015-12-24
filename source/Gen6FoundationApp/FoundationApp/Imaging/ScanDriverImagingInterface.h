/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  Date:       6/11/2009
//  Tools:      RVCT Build 739
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Imaging/ScanDriverImagingInterface.h $
// $Revision: 1.6 $
// $Date: 2010/03/20 11:14:06EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// HEADER:  ScanDriverImagingInterface.h
//
// Protypes used to interface Imaging with the Scan Driver
//------------------------------------------------------------------------------
#ifndef SCANDRIVERIMAGINGINTERFACE_H_
#define SCANDRIVERIMAGINGINTERFACE_H_

#define LED_STATE_NOT_SET	0xFFFFFFFF               //(-1)
#define LED_STATE_ON			1
#define LED_STATE_OFF		0
#define DONT_CHANGE			-1


//****************************************************************************************
//#define _LMC_DEBUGING_SETLIGHTS            //un-comment out for Lights state debugging		lmc

#ifndef	_LMC_DEBUGING_SETLIGHTS
	void SetLights( int LightsState, int LightsInten, int AimerState, int AimerInten );
#else
	void _DEBUG_SetLights( int LightsState, int LightsInten, int AimerState, int AimerInten, unsigned int iCodeLine, char *cCodeFile);    //used to debug lights state
	#define SetLights(LightsState,LightsInten,AimerState,AimerInten)		_DEBUG_SetLights(LightsState,LightsInten,AimerState,AimerInten,__LINE__,__MODULE__)   //lmc_debug    For lights set tracking by line number
#endif                                                                                                                  										          //__FILE__ changed to __MODULE__
//****************************************************************************************


int GetLights( int LightType );


// Heat Mitigation --> generics for image acquire through the SCN_DRV
	BOOL InBatchedMode(void);
	BOOL StartImaging(void);
	BOOL GetImage(void);
	BOOL StopImaging(void);
	BOOL NewImageAvailable(void);

//DR image lock out feature for allignment
	bool IsImagingAllowed( void );

BOOL CurrentlyImaging( void );

#endif /*SCANDRIVERIMAGINGINTERFACE_H_*/

