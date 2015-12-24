//======================================================================================
// CPUPowerSpeedMenuSettings.cpp
//======================================================================================
// $RCSfile: CPUPowerSpeedMenuSettings.cpp $
// $Revision: 1.4 $
// $Date: 2010/01/16 14:27:38EST $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Module Description:
//      This file contains a routine that creates CPU, power, and speed menu settings as part
//		of the overall reader configuration for a Matrix device.
//
//---------------------------------------------------------------------------


#include "stdInclude.h"
#include	"ReaderConfiguration.h"
#include	"CPUPowerSpeedMenuSettings.h"			// CPUPowerSpeedMenuSettings interface definition






void READER_CONFIGURATION::CreateCPUPowerSpeedMenuSettings( int /* ControlMask */ )
{
	AddNumber	("010100", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING , NULL, NULL, &SystemSpeedIndex, 0, 0,
				0, 2);

	AddBoolean	("010101", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING , NULL, NULL, &KeepPowerOn, false, false);

	return;
}



