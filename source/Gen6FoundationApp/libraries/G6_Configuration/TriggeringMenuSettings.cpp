//======================================================================================
// TriggeringMenuSettings.cpp
//======================================================================================
// $RCSfile: TriggeringMenuSettings.cpp $
// $Revision: 1.27 $
// $Date: 2011/11/11 16:21:39EST $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Module Description:
//      This file contains a routine that creates triggering menu settings as part
//		of the overall reader configuration for a Matrix device.
//
//---------------------------------------------------------------------------


#include "stdInclude.h"
#include "readerconfiguration.h"
#include "productconfiguration.h"
#include "TriggeringMenuSettings.h"			// TriggeringMenuSettings interface definition
#include "menu.h"
#include "HardwareToken.h"

//ewr04791 Codegate
// UpdateSDRTIM(): entry function for AOSCGD menu command.
// Sets Scan-Driver-Timeout[SDRTIMx] depending on the Aimer-Out-of-Stand-Code-Gate[AOSCGDx] setting.
//   AOSCGD0 (disable) sets SDRTIM1.
//   AOSCGD1 (enable) sets SDRTIM0.
bool UpdateSDRTIM( void )
{
	if(ReaderConfiguration->MenuCommandResponse->Char[0] == '1' )
	{
		HSTRING SDRTIMCommand("024A000",true);
		SDRTIMCommand.AppendChar( ReaderConfiguration->CurrentTable );
		ReaderConfiguration->Menu( &SDRTIMCommand );
	}
	else
	{
		HSTRING SDRTIMCommand("024A001",true);
		SDRTIMCommand.AppendChar( ReaderConfiguration->CurrentTable );
		ReaderConfiguration->Menu( &SDRTIMCommand );
	}
	return true;
}


void READER_CONFIGURATION::CreateTriggeringMenuSettings( int ControlMask, UINT Two_DCommandModifier, UINT HeatMitigationFixedSettingsCommandModifier )
{
//	Trigger Settings

	if(ControlMask & MAKE_2D_COMMANDS_AVAILABLE)
	{
		if(HardwareToken.IAmOEM())
		{
			//OEM supports TRGMOD_MANUAL_LO
			AddNumericList("0E0100", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &TriggerMode, TRGMOD_MANUAL, TRGMOD_MANUAL,
				TRGMOD_MANUAL, TRGMOD_MANUAL_LO, TRGMOD_PRESENT, TRGMOD_NOTIFY_HOST, TRGMOD_SNAP_AND_SHIP, TRGMOD_STREAM_PRESENT);
		}
		else if(HardwareToken.IAmVoyager1400())
		{
			//Voyager 2D only support manual trigger and normal presenation mode
			AddNumericList("0E0100", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &TriggerMode, TRGMOD_MANUAL, TRGMOD_MANUAL,
	             					TRGMOD_MANUAL, TRGMOD_PRESENT);
		}
		else
		{
#ifndef XENON_LITE_1500	/* TRGMOD6 (Image Snap & Ship) is removed for Xenon Lite(1500) */
			//Standard Corded
			AddNumericList("0E0100", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &TriggerMode, TRGMOD_MANUAL, TRGMOD_MANUAL,
	            TRGMOD_MANUAL, TRGMOD_PRESENT, TRGMOD_NOTIFY_HOST, TRGMOD_SNAP_AND_SHIP, TRGMOD_STREAM_PRESENT);
#else
			//Xenon Lite(1500)
			AddNumericList("0E0100", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &TriggerMode, TRGMOD_MANUAL, TRGMOD_MANUAL,
	            TRGMOD_MANUAL, TRGMOD_PRESENT, TRGMOD_NOTIFY_HOST, /*TRGMOD_SNAP_AND_SHIP,*/ TRGMOD_STREAM_PRESENT);
#endif
		}
   }
	else  //if not 2D
	{
//		if( theHal.WhatBoardAmI() == IT3800G_1_TOKEN)
//		{
//			AddNumericList("TRGMOD", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &TriggerMode, TRGMOD_MANUAL, TRGMOD_MANUAL,
//				TRGMOD_MANUAL, TRGMOD_AUTO, TRGMOD_PRESENT, TRGMOD_NOTIFY_HOST);
//		}
//		else
		{
			AddNumericList("0E0100", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &TriggerMode, TRGMOD_MANUAL, TRGMOD_MANUAL,
				TRGMOD_MANUAL, TRGMOD_AUTO, TRGMOD_MANUAL_LO, TRGMOD_PRESENT, TRGMOD_NOTIFY_HOST);
		}
	}

	//Heat Mitigation
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if( (ControlMask & MAKE_HEAT_MITIGATION_FIXED_SETTINGS_FIXED) && (ControlMask & MAKE_HEAT_MITIGATION_AVAILABLE) )  //enabled in standard build
	{
   	AddBoolean("0E0101", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP | HeatMitigationFixedSettingsCommandModifier, NULL, NULL, &OpticalTriggerLowPower, true, true);
  		AddBoolean("0E0102", Two_DCommandModifier | WORKGROUP | HeatMitigationFixedSettingsCommandModifier, NULL, NULL, &PresModeLEDRampDown, false, false);
  		AddNumber("0E0103", Two_DCommandModifier | WORKGROUP | HeatMitigationFixedSettingsCommandModifier, NULL, NULL, &PresModeUpTimeout, 5000, 5000, 0, 300000);
	}
	else      //heat mitigation fixed feature not enabled or it is enabled in engineering build only
	{
		AddBoolean("0E0101", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &OpticalTriggerLowPower, true, true);
  		AddBoolean("0E0102", Two_DCommandModifier | WORKGROUP , NULL, NULL, &PresModeLEDRampDown, false, false);
  		AddNumber("0E0103", Two_DCommandModifier | WORKGROUP , NULL, NULL, &PresModeUpTimeout, 5000, 5000, 0, 300000);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	AddBoolean("0E0104", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | SUB_TAG_TABLE::FIXED_EXCEPT_ACTIVE | WORKGROUP , NULL, NULL, &bIgnoreTrigger, false, false);
		if( (ControlMask & MAKE_HEAT_MITIGATION_FIXED_SETTINGS_FIXED) && (ControlMask & MAKE_HEAT_MITIGATION_AVAILABLE) )  //enabled in standard build
		{
   		AddNumber("0E0105", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP | HeatMitigationFixedSettingsCommandModifier, NULL, NULL, &SerialTrigTimeOut, 30000, 30000, 0,300000);
		}
		else      //heat mitigation fixed feature not enabled or it is enabled in engineering build only
		{
			AddNumber("0E0105", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &SerialTrigTimeOut, 30000, 30000, 0,300000);
			//AddNumber("LEDLVL", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &LedLevel, 1000, 1000, 0,20000);
			//Voyager 2D only support manual trigger and normal presenation mode
			//AddNumericList("CRTLVL", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &CurrentLevel, 1, 1,
	             	//				1, 2,3,4);
		}

	AddNumber("0E0106", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &PresModeTimeOut, 5000, 5000, 0,300000);
	AddBoolean("0E0107", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &bAllowBufferedOutput, true, true);

	if( (ControlMask & MAKE_HEAT_MITIGATION_FIXED_SETTINGS_FIXED) && (ControlMask & MAKE_HEAT_MITIGATION_AVAILABLE) )  //enabled in standard build
	{
		AddNumber("0E0108", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP | HeatMitigationFixedSettingsCommandModifier, NULL, NULL, &LowPowerTimeOut, 120, 120, 0, 300);
		if(HardwareToken.IAmVoyager1400())
			PresModePriceChecker = true;
		else
			AddBoolean("0E0109", Two_DCommandModifier | WORKGROUP | HeatMitigationFixedSettingsCommandModifier, NULL, NULL, &PresModePriceChecker, false, false);     //if heat mitigation
		AddNumber("0E010A", Two_DCommandModifier | WORKGROUP | HeatMitigationFixedSettingsCommandModifier, NULL, NULL, &PresModeSensitivity, 1, 1, 0, 20);
		AddNumber("0E010B", Two_DCommandModifier | WORKGROUP | HeatMitigationFixedSettingsCommandModifier, NULL, NULL, &PresModeNumbTimeout, 500, 500, 0, 9999);     //if heat mitigation
	}
	else if( (ControlMask & MAKE_HEAT_MITIGATION_AVAILABLE) )   //enabled in engineering build only
	{
		AddNumber("0E0108", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP , NULL, NULL, &LowPowerTimeOut, 120, 120, 0, 300);
		if(HardwareToken.IAmVoyager1400())
			PresModePriceChecker = true;
		else
			AddBoolean("0E0109", Two_DCommandModifier | WORKGROUP , NULL, NULL, &PresModePriceChecker, false, false);     //if heat mitigation
		AddNumber("0E010A", Two_DCommandModifier | WORKGROUP , NULL, NULL, &PresModeSensitivity, 1, 1, 0, 20);
		AddNumber("0E010B", Two_DCommandModifier | WORKGROUP , NULL, NULL, &PresModeNumbTimeout, 500, 500, 0, 9999);     //if heat mitigation
	}
	else      //heat mitigation fixed feature not enabled
	{
	AddNumber("0E0108", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP , NULL, NULL, &LowPowerTimeOut, 120, 120, 0, 300);
	if(HardwareToken.IAmVoyager1400())
		PresModePriceChecker = true;
	else
		AddBoolean("0E0109", Two_DCommandModifier | WORKGROUP , NULL, NULL, &PresModePriceChecker, true, true);
#if defined(BYD_SENSOR)
	AddNumber("0E010A", Two_DCommandModifier | WORKGROUP , NULL, NULL, &PresModeSensitivity, 2, 2, 0, 20);
#else
	AddNumber("0E010A", Two_DCommandModifier | WORKGROUP , NULL, NULL, &PresModeSensitivity, 1, 1, 0, 20);
#endif
	AddNumber("0E010B", Two_DCommandModifier | WORKGROUP , NULL, NULL, &PresModeNumbTimeout, 2500, 2500, 0, 9999);
	}

	AddNumber("0E010C", Two_DCommandModifier | WORKGROUP , NULL, NULL, &PresModePreAimerDelay, 0, 0, 0, 9999);

#if defined (FLASH_LEDS) || defined (RFBASE)       //if ff3 gen6 board
	AddNumericList("0E010D", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | Two_DCommandModifier | WORKGROUP , NULL, NULL, &PresModeLowLights, FLASH_LED_OFF, FLASH_LED_OFF, FLASH_LED_OFF, FLASH_LED_LOW);
	AddNumericList("0E010E", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | Two_DCommandModifier | WORKGROUP , NULL, NULL, &PresModeMitigationLowLights, FLASH_LED_LOW, FLASH_LED_LOW, FLASH_LED_OFF, FLASH_LED_LOW, FLASH_LED_HIGH);
#else
	AddNumber("0E010D", Two_DCommandModifier | WORKGROUP , NULL, NULL, &PresModeLowLights, 0, 0, 0, 100);
	AddNumber("0E010E", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | Two_DCommandModifier | WORKGROUP , NULL, NULL, &PresModeMitigationLowLights, 10, 10, 0, 50);
#endif                  //end if ff3 gen6 board

	AddNumber("0E010F", Two_DCommandModifier | WORKGROUP , NULL, NULL, &PresModeLowLightTarget, 50, 50, 30, 100);
   AddNumericList("0E0110", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &TriggerModeModifierFlag, TRGMMF_NONE, TRGMMF_NONE,
    TRGMMF_NONE, TRGMMF_REREAD_FOV);

#ifdef DEVELOPMENT_SETTINGS
   AddBoolean("0E0111", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &AllowSecondaryTriggerMode, false, false);
   AddNumber("0E0112", Two_DCommandModifier | WORKGROUP , NULL, NULL, &TriggerClickTime, 300, 300, 100, 1000);
   AddNumber("0E0113", Two_DCommandModifier | WORKGROUP , NULL, NULL, &TriggerInterClickTime, 800, 800, 100, 5000);
   AddString("0E0114", SUB_TAG_TABLE::STRING_MUST_BE_QUOTED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP,
         NULL, NULL, SecondaryTriggerStartCommand, SIZE_OF_SECONDARY_TRIGGER_STRING, "TRGMOD6\0", "TRGMOD6\0");
   AddString("0E0115", SUB_TAG_TABLE::STRING_MUST_BE_QUOTED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP,
         NULL, NULL, SecondaryTriggerEndCommand, SIZE_OF_SECONDARY_TRIGGER_STRING, "TRGMOD0\0", "TRGMOD0\0");
#endif



	ScanStandSwitchEn=false; // TODO:  default inits like this should probably go in a constructor
	if (GET_CONFIG_SETTING(HasStandSW)) AddBoolean("0E0116", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &ScanStandSwitchEn, true, true);

#ifndef XENON_LITE_1500	/* Codegate is removed for Xenon Lite(1500) */
	//ewr04791 Codegate
	AddBoolean("0E0300", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, &UpdateSDRTIM, &bAimerOutOfStandCodeGate, false, false);
#else
	bAimerOutOfStandCodeGate = false;
#endif

#if defined(SM4100)
	AddNumber("0E0117", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &TriggerReleaseDebounceTimer, 500, 500, 4, 3000);
#endif
#if defined(SM4100) || defined(IR_TRIGGER)
	AddBoolean("0E0119", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &bIRSameCodeDelayEnable, false, false);
#endif

#if defined(SM3100) || defined(SM3110)|| defined(SM5130)
	AddBoolean("0E0118", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &bTriggerAimerSync, false, false);
	AddBoolean("0E0500", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &bAimerIndependOn, false, false);
#endif
	return;
}



