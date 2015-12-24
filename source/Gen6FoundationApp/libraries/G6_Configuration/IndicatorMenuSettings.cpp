//======================================================================================
// IndicatorMenuSettings.cpp
//======================================================================================
// $RCSfile: IndicatorMenuSettings.cpp $
// $Revision: 1.54 $
// $Date: 2011/11/18 17:14:30EST $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Module Description:
//      This file contains a routine that creates indicator menu settings as part
//		of the overall reader configuration for a Matrix device.
//
//---------------------------------------------------------------------------


#include "stdInclude.h"
#include "beep.h"
//#include "MainHHPLProc.h"
#include "Menu.h"
#include "ioman.h"
//#include "Trigger.h"
#include "ProductConfiguration.h"
#include "ReaderConfiguration.h"
#include "IndicatorMenuSettings.h"			// IndicatorMenuSettings interface definition
#if PLATFORM_CURRENT == imx25
	#ifdef FLASH_LEDS
		#include "Flash_Illumination_Control.h"
	#else
		#include "iMX25_Illumination_Control.h"
	#endif //endof FLASH_LEDS
	
	#include "Trigger_5_5.h"
	#include "HardwareToken.h"
	
#endif



/*-------------------------------------------------------------------------
 * ExecuteSound
 *-------------------------------------------------------------------------*/
bool ExecuteSound(void)
{
	int nStyle;


/* Convert the extra data at end of menu command into beeper style.	*/
	if((ReaderConfiguration->ExtraneousTextAfterCommand->Size != 1))
	{
		return false;
	}
	else
	{
		nStyle = ((int) (ReaderConfiguration->ExtraneousTextAfterCommand->Char[0] - '0'));
	}


	/*	The 3800 only uses style 0. Matrix compatible with 3800.	*/
 	switch ( nStyle)
 	{
 		case 0 :
 		case 1 :
 		case 2 :
 		case 3 : SoundStandardBeep(1); break;
 		case 4 : SoundErrorBeep(); break;
 		default:	return false;
 	}
 	return true;
}

bool MenuExecuteHostAck(void)
{
	return ExecuteHostAckBeep(ReaderConfiguration->ExtraneousTextAfterCommand->Char[0]);
}

#ifdef RFBASE

#else //elsof #ifdef RFBASE

bool ForceAimOn(void)
{
	if(!(HardwareToken.IAmVoyager1400()))
	{
		pTrigger->ForceAimerOn();

		if(!(HardwareToken.IAmOEMJade()||(GET_CONFIG_SETTING(AimerType) == AIMER_PSOC_LASER)))   
		{
			IllTestMode(ILL_ON,0x0);
		}
	}
	else
	{
		((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_aimer.turnon(HI2D_AIM_ALT);
	}

 	return true;
}

bool ForceAimOff(void)
{
	if(!(HardwareToken.IAmVoyager1400()))
	{	
		pTrigger->ForceAimerOff();
	
		if(!(HardwareToken.IAmOEMJade()||(GET_CONFIG_SETTING(AimerType) == AIMER_PSOC_LASER)))   
		{
			IllTestMode(ILL_OFF,0x0);
		}
	}
	else
	{
		((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_aimer.turnoff(HI2D_AIM_ALT);
	}
 	return true;
}

bool ForceLedOn(void)
{
	//theStdInterface.printf("hello token is %d\r\n", HardwareToken.InterfaceToken());
	if(!(HardwareToken.IAmVoyager1400()))
	{
		pTrigger->ForceLEDsOn();
	
		if(!HardwareToken.IAmOEMJade())
		{
			IllTestMode(ILL_ON,0x1);
		}
	}
	else
	{
		((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_flash.turnon(HI2D_AIM_ALT, true/*, ((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_LedCtrlMode*/);
	}
 	return true;
}

bool ForceLedOff(void)
{
	if(!(HardwareToken.IAmVoyager1400()))
	{
		pTrigger->ForceLEDsOff();
		
		if(!HardwareToken.IAmOEMJade())
		{
			IllTestMode(ILL_OFF,0x1);
		}
	}
	else
	{
		((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_flash.turnoff(HI2D_AIM_ALT, true/*, ((HI2D_illumination_control *)get_HI2D_illum_crl())->HI2D_LedCtrlMode*/);
	}
	return true;
}

bool ForceAllLightsOff(void)
{
	if(!(HardwareToken.IAmVoyager1400()))
	{	
		pTrigger->ForceAimerOff();
		pTrigger->ForceLEDsOff();

		if(!HardwareToken.IAmOEMJade())
		{
			IllTestMode(ILL_OFF,0x0);
		
			SetLedControlState(0x0);
		}
	}
	else
	{
		//TODO
	}

 	return true;
}

bool ForceImagerOn(void)
{
//FIXME   theHal.Power2dOn();
 	return true;
}

bool ForceImagerOff(void)
{
//FIXME    theHal.Power2dOff();
 	return true;
}

#endif //endof #ifdef RFBASE


static bool bPagingActive=false; /* static varible used to for scanner or base to track if they are currently paging. */
bool IsPagingActive(void)
{
	return bPagingActive;
}

bool CheckPageMode()
{
	
	if(ReaderConfiguration->ExtraneousTextAfterCommand->Size == 1 && (	ReaderConfiguration->ExtraneousTextAfterCommand->Char[0] == '1' 
	|| ReaderConfiguration->ExtraneousTextAfterCommand->Char[0] == '0') )
	{
		bPagingActive = ReaderConfiguration->ExtraneousTextAfterCommand->Char[0] == '1';
	} 
	else
	{
		return false; /* will give us a NAK for the menu */
	}
	
	return true;
}



void READER_CONFIGURATION::CreateIndicatorMenuSettings( int /* ControlMask */ )
{
	AddBoolean("050100", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &illumON, true, true);


		AddCommand("050200", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, &ExecuteSound,NULL);
		AddCommand("050201", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &ExecuteGoodReadBeepSequence, NULL );
		AddCommand("050202", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &ExecuteNoReadBeepSequence, NULL );
		AddCommand("050203", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &GoodMenuBeep, NULL );
		AddCommand("050204", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &ErrorMenuBeep, NULL );
	

#ifdef RFBASE
#else
#if !defined(RFSCAN)

//enable BEL Beep mode
	AddBoolean("050500", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &bBelBeepEnable, false, false);

#endif  //end !defined(RFSCAN)
#endif  //end ifdef RFBASE

	
//	Beeper Settings

#ifndef XENON_LITE_1500
		AddNumericList("05021D", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &BASE_SCANNER_SELECT_BeepVolume, BEEP_HI, BEEP_HI,
				BEEP_OFF, BEEP_LO, BEEP_MED, BEEP_HI);
#else
		AddNumericList("05021D", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &BASE_SCANNER_SELECT_BeepVolume, BEEP_MED, BEEP_MED,
				BEEP_OFF, BEEP_LO, BEEP_MED, BEEP_HI);
#endif

//	else
//		AddNumericList("BEPLVL", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &BASE_SCANNER_SELECT_BeepVolume, BEEP_MED, BEEP_MED,
//				BEEP_OFF, BEEP_LO, BEEP_MED, BEEP_HI);	

//FIXME	if( (CHal::WhatBoardAmI() == IT3800G_1_TOKEN) )
//FIXME		AddNumber("BEPFQ1", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &BASE_SCANNER_SELECT_GoodReadFrequency, 2550, 2550, 400, 9000);
//FIXME	else if( (CHal::WhatBoardAmI() == IT4800DR_1_TOKEN) )
//FIXME		AddNumber("BEPFQ1", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &BASE_SCANNER_SELECT_GoodReadFrequency, 2850, 2850, 400, 9000);
//FIXME	else
	int DefaultGoodReadFreq = 2730;
	AddNumber("05020D", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &BASE_SCANNER_SELECT_GoodReadFrequency, DefaultGoodReadFreq, DefaultGoodReadFreq, 400, 9000);
	AddNumber("05020E", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &BASE_SCANNER_SELECT_BeepOnRead, 1, 1, 1, 9);
	AddBoolean("05020F", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &BASE_SCANNER_SELECT_GoodReadLEDMode, TRUE, TRUE);
	AddBoolean("050210", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &BASE_SCANNER_SELECT_GoodReadBeepMode, TRUE, TRUE);
	AddBoolean("050211", SUB_TAG_TABLE::FULLY_VISIBLE| WORKGROUP, NULL, NULL, &bTrigBeepEnable, false, false);
	AddNumber("050212", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &BASE_SCANNER_SELECT_BeepOnError, 1, 1, 0, 9);
	AddNumber("050213", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &BASE_SCANNER_SELECT_ErrorFrequency, 150, 150, 100, 9000);

	AddNumber("050214", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &BASE_SCANNER_SELECT_BeepOnDecode, 0, 0, 0, 9);
	AddNumber("050215", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &BASE_SCANNER_SELECT_BeepOnNoDecode, 0, 0, 0, 9);
	AddBoolean("050216", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &BASE_SCANNER_SELECT_BeepBipMode, false, false);
	AddBoolean("050217", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &BASE_SCANNER_SELECT_bPowerUpBeepEnable, true, true);
	
							

	
	AddCommand("050600", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING, &ForceAimOn, NULL);
	AddCommand("050601", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING, &ForceAimOff, NULL);


#if defined( DEVELOPMENT_SETTINGS )
	AddCommand("050602", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING, &ForceLedOn, NULL);
	AddCommand("050603", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING, &ForceLedOff, NULL);
	AddCommand("050604", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING, &ForceAllLightsOff, NULL);
	AddCommand("050605", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING, &ForceImagerOn, NULL);
	AddCommand("050606", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING, &ForceImagerOff, NULL);
#else
	AddCommand("050602", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &ForceLedOn, NULL);
	AddCommand("050603", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &ForceLedOff, NULL);
	AddCommand("050604", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &ForceAllLightsOff, NULL);
	AddCommand("050605", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &ForceImagerOn, NULL);
	AddCommand("050606", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING, &ForceImagerOff, NULL);
#endif


#ifndef XENON_LITE_1500
	AddNumber("05021B", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING , NULL, NULL, &BeepDuration, 100, 100, 1, 1000);
#else
	AddNumber("05021B", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING , NULL, NULL, &BeepDuration, 150, 150, 1, 1000);
#endif
	
#ifndef XENON_LITE_1500	/* Host Command Acknowledgement is removed for Xenon Lite(1500) */
	AddBoolean("050700", SUB_TAG_TABLE::FULLY_VISIBLE, NULL, NULL, &bEnableHostAck, FALSE, FALSE);
	AddNumber("050701", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &iHostAckTimeout, 10,10,1,60 );
#else
	bEnableHostAck = FALSE;
	iHostAckTimeout = 10;
#endif


#ifdef FIXME
	if( (CHal::WhatBoardAmI() == IT4800DR_1_TOKEN) )
		AddBoolean("05021C", SUB_TAG_TABLE::FULLY_VISIBLE , NULL, NULL, &GoodReadLEDPowerOnIndicator, FALSE, FALSE);
	else
#endif
	GoodReadLEDPowerOnIndicator = FALSE;


	if ( GET_CONFIG_SETTING(m_TfbPop) )
	{
		AddBoolean("050800", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &m_bEnableGoodReadTfb, false, false);
		AddNumber("050801", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &m_TfbDuration, 300, 300, 100, 2000);
	}
	else
	{
		m_bEnableGoodReadTfb = false;
		m_TfbDuration = 0;
	}

	return;
}

