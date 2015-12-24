//======================================================================================
// CPUPowerSpeedMenuSettings.h
//======================================================================================
// $RCSfile: CPUPowerSpeedMenuSettings.h $
// $Revision: 1.2 $
// $Date: 2009/02/27 15:56:40EST $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Module Interface Description:
//      This file contains the interface definition to a routine that creates
//		CPU, power, and speed menu settings as part of the overall reader configuration for
//		a Matrix device.  It also contains the active value declarations of
//		CPU, power, and speed settings.
//
//---------------------------------------------------------------------------


#ifndef CPUPowerSpeedMenuSettingsH
#define CPUPowerSpeedMenuSettingsH
//---------------------------------------------------------------------------


#include	"language.h"					// system global definitions


int 		SystemSpeedIndex; 		// prescale = 0, 1 , 2, 3 so system clock is 96MHz/(prescale+1)
bool		KeepPowerOn;

void CreateCPUPowerSpeedMenuSettings( int ControlMask );


#endif // CPUPowerSpeedMenuSettingsH
//----------------------------------------------------------------------------




