//======================================================================================
// InterfaceMenuSettings.cpp
//======================================================================================
// $RCSfile: InterfaceMenuSettings.cpp $
// $Revision: 1.53 $
// $Date: 2011/11/01 01:28:26EDT $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Module Description:
//      This file contains a routine that creates interface menu settings as part
//			of the overall reader configuration for a Matrix device.
//
//---------------------------------------------------------------------------


#include "stdInclude.h"
#include	"Menu.h"
#include	"ReaderConfiguration.h"
#include	"InterfaceMenuSettings.h"			// InterfaceMenuSettings interface definition
#include "Interface_support_functions.h"

#include	"ProductConfiguration.h"
#include "AutoCableSelect.h"
#include "db_hdw.h"
#include "db_chrio.h"
#include "IoMan.h"
#include "startupFlashdisk.h"
#include "MenuFlash.h"
//#include	"hal.h"                                                                                //lmc  fixme
#include "PowerManagement.h"
#include "HardwareToken.h"
#include "mainhhplproc.h"


extern bool ProcessingScannedMenu;
bool UpdateCorrespondingInterface=false;
bool CableUSBModeChngmnu=false;
bool CableRS232ModeChngmnu=false;
bool CableKBWModeChngmnu=false;
bool CableRS485ModeChngmun=false;
bool CableWorkingTableChngmun=false;
bool WorkingTableSelChngmnu=false;



bool ExpandBaudRates(void)	                // go allow higher baud rates for engineers.
{
	if(GET_SETTING(bEngSettings))
		ReaderConfiguration->ChangeNumericList("060702", RCFG_BAUD_300, RCFG_BAUD_600, RCFG_BAUD_1200, RCFG_BAUD_2400,
																RCFG_BAUD_4800,RCFG_BAUD_9600, RCFG_BAUD_19200, RCFG_BAUD_38400, RCFG_BAUD_57600,
																RCFG_BAUD_115200, RCFG_BAUD_230400, RCFG_BAUD_460800, RCFG_BAUD_921600);
 	return TRUE; // It's called in an exit function, return TRUE always.
}

bool Update232Lock()
{

	int iRS232Lock = ReaderConfiguration->MenuCommandResponse->Char[0]-'0';

	// No action if the setting value is the same as the old one.
	if(iRS232Lock == (int)GET_SETTING(RS232Unlock))
		return true;

	if(HardwareToken.IAmOEM() && (RS232_ONLY != GET_CONFIG_SETTING(iInterfacePopulation))){
		int piTempTerms[400];
		int iNumberOfTempTerms;


		if(!iRS232Lock){
			iNumberOfTempTerms=CollectInterfaceUSBTerminalIds(piTempTerms);
		}
		else{
			iNumberOfTempTerms=CollectAllInterfaceTerminalIds(sizeof piTempTerms/sizeof(int), piTempTerms);	//query the interface table	
		}

		ReaderConfiguration->ChangeNumericList("060500", piTempTerms, iNumberOfTempTerms, 3);	//update the menu
	}

	return true;
}

bool CheckTermid(void)
{
	#ifndef RFSCAN
	bool Status=true;
	if(Status)
	{
		if(!(CableUSBModeChngmnu||CableWorkingTableChngmun||CableKBWModeChngmnu||CableRS232ModeChngmnu||CableRS485ModeChngmun||ReaderConfiguration->FirstTimeLoadWorkingConfigurationInFlash ))
		{
			UpdateCorrespondingInterface=true;
		}

	}
	#endif
	return true;
}

bool UpdateCableInterface(void)
{
	#ifndef RFSCAN
	if(UpdateCorrespondingInterface)
	{
		UpdateCorrespondingInterface=false;
		int i=GET_SETTING(TerminalID);
		HSTRING		MenuCommandText( 20, true );
		if(i>121)
			MenuCommandText.Append("060802");
		else if(i==0)
			MenuCommandText.Append("060800");
		else if(i==51)
			MenuCommandText.Append("060803");
		else if((i>0)&&(i<=121))
			MenuCommandText.Append("060801");

		MenuCommandText.Append(ReaderConfiguration->MenuCommandResponse->Char);
		if(ReaderConfiguration->CurrentTable=='.')
			MenuCommandText.Append(".");
		else if(ReaderConfiguration->CurrentTable=='!')
			MenuCommandText.Append("!");
		ReaderConfiguration->Menu( &MenuCommandText );
	}
	#endif
	return true;

}

//update termid base on TRMUSB TRM232 TRMRTL for NGOEM board
bool UpdateTermIDsUponTRM()
{
	bool Status = true;
	#ifndef RFSCAN
	if(UpdateCorrespondingInterface)
	{
		UpdateCorrespondingInterface=false;
		HSTRING		MenuCommandText( 20, true );
		MenuCommandText.Append("060500");
		MenuCommandText.Append(ReaderConfiguration->MenuCommandResponse->Char);
		if(ReaderConfiguration->CurrentTable == '.')
			MenuCommandText.Append(".");
		else if(ReaderConfiguration->CurrentTable == '!')
			MenuCommandText.Append("!");
		Status =	ReaderConfiguration ->Menu(&MenuCommandText);
	}
	#endif
	return Status;
}


void LoadUSBWorkingConfigurationZero(){
	HSTRING CommandAndResponse(2*FLASH_MAX_CONFIGURATION_LENGTH,true);
	HSTRING CabSel(20,false);
	int sz = ReadAndCloseFile("WorkingConfig0",FLASH_MAX_CONFIGURATION_LENGTH * 2,&CommandAndResponse.Char);
		if(sz>0)
		{
			CommandAndResponse.Size=sz;
			CommandAndResponse.Char[CommandAndResponse.Size] = 0;		// Probably unnecessary, since ReadWorkingConfigurationFromFlash should do this.
			CommandAndResponse.ReplaceAll("060301", "060305");
			CommandAndResponse.ReplaceAll("060300", "060305");
			CommandAndResponse.ReplaceAll("BEPPAG", "060305");
//				RESET_WATCHDOG_TIMER();
			ReaderConfiguration->Menu(&CommandAndResponse.Size, CommandAndResponse.Char);
//				RESET_WATCHDOG_TIMER();
			}
}
void LoadUSBWorkingConfigurationOne(){
	HSTRING CommandAndResponse(FLASH_MAX_CONFIGURATION_LENGTH*2,true);
	int sz = ReadAndCloseFile("WorkingConfig1",FLASH_MAX_CONFIGURATION_LENGTH * 2,&CommandAndResponse.Char);
		if(sz>0)
		{
			CommandAndResponse.Size=sz;
			CommandAndResponse.Char[CommandAndResponse.Size] = 0;		// Probably unnecessary, since ReadWorkingConfigurationFromFlash should do this.
			CommandAndResponse.ReplaceAll("060301", "060305");
			CommandAndResponse.ReplaceAll("060300", "060305");
			CommandAndResponse.ReplaceAll("BEPPAG", "060305");
//				RESET_WATCHDOG_TIMER();
			ReaderConfiguration->Menu(&CommandAndResponse.Size, CommandAndResponse.Char);
//					RESET_WATCHDOG_TIMER();
			}
}

void LoadKBWWorkingConfigurationZero(){
	HSTRING CommandAndResponse(2*FLASH_MAX_CONFIGURATION_LENGTH,true);
	HSTRING CabSel(20,false);
	int sz = ReadAndCloseFile("WorkingConfig0",FLASH_MAX_CONFIGURATION_LENGTH * 2,&CommandAndResponse.Char);
		if(sz>0)
		{
			CommandAndResponse.Size=sz;
			CommandAndResponse.Char[CommandAndResponse.Size] = 0;		// Probably unnecessary, since ReadWorkingConfigurationFromFlash should do this.
			CommandAndResponse.ReplaceAll("060303", "060305");
			CommandAndResponse.ReplaceAll("060300", "060305");
			CommandAndResponse.ReplaceAll("BEPPAG", "060305");
//				RESET_WATCHDOG_TIMER();
			ReaderConfiguration->Menu(&CommandAndResponse.Size, CommandAndResponse.Char);
//				RESET_WATCHDOG_TIMER();
			}
}
void LoadKBWWorkingConfigurationOne(){
	HSTRING CommandAndResponse(FLASH_MAX_CONFIGURATION_LENGTH*2,true);
	//int PositionTableSettingWasFound;
	int sz = ReadAndCloseFile("WorkingConfig1",FLASH_MAX_CONFIGURATION_LENGTH * 2,&CommandAndResponse.Char);
		if(sz>0)
		{
			CommandAndResponse.Size=sz;
			CommandAndResponse.Char[CommandAndResponse.Size] = 0;		// Probably unnecessary, since ReadWorkingConfigurationFromFlash should do this.
			CommandAndResponse.ReplaceAll("060303", "060305");
			CommandAndResponse.ReplaceAll("060300", "060305");
			CommandAndResponse.ReplaceAll("BEPPAG", "060305");
//				RESET_WATCHDOG_TIMER();
			ReaderConfiguration->Menu(&CommandAndResponse.Size, CommandAndResponse.Char);
//					RESET_WATCHDOG_TIMER();
			}
}

void Load232WorkingConfigurationZero(){
	HSTRING CommandAndResponse(2*FLASH_MAX_CONFIGURATION_LENGTH,true);
	HSTRING CabSel(20,false);
	int sz = ReadAndCloseFile("WorkingConfig0",FLASH_MAX_CONFIGURATION_LENGTH * 2,&CommandAndResponse.Char);
		if(sz>0)
		{
			CommandAndResponse.Size=sz;
			CommandAndResponse.Char[CommandAndResponse.Size] = 0;		// Probably unnecessary, since ReadWorkingConfigurationFromFlash should do this.
			CommandAndResponse.ReplaceAll("060302", "060305");
			CommandAndResponse.ReplaceAll("060300", "060305");
			CommandAndResponse.ReplaceAll("BEPPAG", "060305");
//				RESET_WATCHDOG_TIMER();
			ReaderConfiguration->Menu(&CommandAndResponse.Size, CommandAndResponse.Char);
//				RESET_WATCHDOG_TIMER();
			}
}
void Load232WorkingConfigurationOne(){
	HSTRING CommandAndResponse(FLASH_MAX_CONFIGURATION_LENGTH*2,true);
	//int PositionTableSettingWasFound;
	int sz = ReadAndCloseFile("WorkingConfig1",FLASH_MAX_CONFIGURATION_LENGTH * 2,&CommandAndResponse.Char);
		if(sz>0)
		{
			CommandAndResponse.Size=sz;
			CommandAndResponse.Char[CommandAndResponse.Size] = 0;		// Probably unnecessary, since ReadWorkingConfigurationFromFlash should do this.
			CommandAndResponse.ReplaceAll("060302", "060305");
			CommandAndResponse.ReplaceAll("060300", "060305");
			CommandAndResponse.ReplaceAll("BEPPAG", "060305");
//				RESET_WATCHDOG_TIMER();
			ReaderConfiguration->Menu(&CommandAndResponse.Size, CommandAndResponse.Char);
//					RESET_WATCHDOG_TIMER();
			}
}

void Load485WorkingConfigurationZero(){
	HSTRING CommandAndResponse(2*FLASH_MAX_CONFIGURATION_LENGTH,true);
	HSTRING CabSel(20,false);
	int sz = ReadAndCloseFile("WorkingConfig0",FLASH_MAX_CONFIGURATION_LENGTH * 2,&CommandAndResponse.Char);
		if(sz>0)
		{
			CommandAndResponse.Size=sz;
			CommandAndResponse.Char[CommandAndResponse.Size] = 0;		// Probably unnecessary, since ReadWorkingConfigurationFromFlash should do this.
			CommandAndResponse.ReplaceAll("060304", "060305");
			CommandAndResponse.ReplaceAll("060300", "060305");
			CommandAndResponse.ReplaceAll("BEPPAG", "060305");
//				RESET_WATCHDOG_TIMER();
			ReaderConfiguration->Menu(&CommandAndResponse.Size, CommandAndResponse.Char);
//				RESET_WATCHDOG_TIMER();
			}
}
void Load485WorkingConfigurationOne(){
	HSTRING CommandAndResponse(FLASH_MAX_CONFIGURATION_LENGTH*2,true);
	int sz = ReadAndCloseFile("WorkingConfig1",FLASH_MAX_CONFIGURATION_LENGTH * 2,&CommandAndResponse.Char);
		if(sz>0)
		{
			CommandAndResponse.Size=sz;
			CommandAndResponse.Char[CommandAndResponse.Size] = 0;		// Probably unnecessary, since ReadWorkingConfigurationFromFlash should do this.
			CommandAndResponse.ReplaceAll("060304", "060305");
			CommandAndResponse.ReplaceAll("060300", "060305");
			CommandAndResponse.ReplaceAll("BEPPAG", "060305");
//				RESET_WATCHDOG_TIMER();
			ReaderConfiguration->Menu(&CommandAndResponse.Size, CommandAndResponse.Char);
//					RESET_WATCHDOG_TIMER();
			}
}

void LoadWorkingConfigurationOne()
{
	HSTRING CommandAndResponse(2*FLASH_MAX_CONFIGURATION_LENGTH,true);
	HSTRING CabSel(20,false);
	int sz = ReadAndCloseFile("WorkingConfig1",FLASH_MAX_CONFIGURATION_LENGTH * 2,&CommandAndResponse.Char);
		if(sz>0)
		{
			CommandAndResponse.Size=sz;
			CommandAndResponse.Char[CommandAndResponse.Size] = 0;		// Probably unnecessary, since ReadWorkingConfigurationFromFlash should do this.
			CommandAndResponse.ReplaceAll("060304", "060305");
			CommandAndResponse.ReplaceAll("060303","060305");
			CommandAndResponse.ReplaceAll("060301", "060305");
			CommandAndResponse.ReplaceAll("060302", "060305");
			CommandAndResponse.ReplaceAll("060300", "060305");
			CommandAndResponse.ReplaceAll("BEPPAG", "060305");
//				RESET_WATCHDOG_TIMER();
			ReaderConfiguration->Menu(&CommandAndResponse.Size, CommandAndResponse.Char);
//				RESET_WATCHDOG_TIMER();
			}
}
void LoadWorkingConfigurationZero()
{
	HSTRING CommandAndResponse(2*FLASH_MAX_CONFIGURATION_LENGTH,true);
	HSTRING CabSel(20,false);
	int sz = ReadAndCloseFile("WorkingConfig0",FLASH_MAX_CONFIGURATION_LENGTH * 2,&CommandAndResponse.Char);
		if(sz>0)
		{
			CommandAndResponse.Size=sz;
			CommandAndResponse.Char[CommandAndResponse.Size] = 0;		// Probably unnecessary, since ReadWorkingConfigurationFromFlash should do this.
			CommandAndResponse.ReplaceAll("060304", "060305");
			CommandAndResponse.ReplaceAll("060303","060305");
			CommandAndResponse.ReplaceAll("060301", "060305");
			CommandAndResponse.ReplaceAll("060302", "060305");
			CommandAndResponse.ReplaceAll("060300", "060305");
			CommandAndResponse.ReplaceAll("BEPPAG", "060305");
//				RESET_WATCHDOG_TIMER();
			ReaderConfiguration->Menu(&CommandAndResponse.Size, CommandAndResponse.Char);
//				RESET_WATCHDOG_TIMER();
			}
}

bool CheckUSBCableMode(void)
{
	bool Status=true;
	#ifndef RFSCAN
	if(ReaderConfiguration->TableSettingCommandisProcessing)
		return false;
	if(Status)
	{
		char temp[16];
		char demp[16];
		sprintf(demp,"%d",ReaderConfiguration->GetActiveWorktableNumber());
		sprintf(temp,"%d",GET_SETTING(UsbTabSelNum));
		if(((strlen(temp)==ReaderConfiguration->MenuCommandResponse->Size)&&
		( memcmp(temp,ReaderConfiguration->MenuCommandResponse->Char,ReaderConfiguration->MenuCommandResponse->Size)==0))||CableWorkingTableChngmun||CableKBWModeChngmnu||CableRS232ModeChngmnu||CableRS485ModeChngmun||ReaderConfiguration->FirstTimeLoadWorkingConfigurationInFlash )
		{	CableUSBModeChngmnu=false;
			if(GET_SETTING(GeneralDebugOn))
				theStdInterface.printf("we need not load new table for usb\r\n");
		}
		else if(!((strlen(temp)==ReaderConfiguration->MenuCommandResponse->Size)&&
		( memcmp(temp,ReaderConfiguration->MenuCommandResponse->Char,ReaderConfiguration->MenuCommandResponse->Size)==0))&&PresentInterface==Interface_USB_Group)
		{
			CableUSBModeChngmnu=true;
			CableWorkingTableChngmun=true;
			ReaderConfiguration->TableNeedChange=true;
			if(GET_SETTING(GeneralDebugOn))
				theStdInterface.printf( "Active Work table is changing from %d to %s\r\n",GET_SETTING(UsbTabSelNum),
				(char*) ReaderConfiguration->MenuCommandResponse->Char);
		}
		else
		{

			CableUSBModeChngmnu=true;
			CableWorkingTableChngmun=false;
			if(GET_SETTING(GeneralDebugOn))
				theStdInterface.printf("Cable is not usb and we need to save this setting to two table \r\n");
		}
	}
	#endif
	return Status;
}
//
//ChangeCableTable
//    When calbe table has been changed,reconfigure two table.
//
bool ChangeUSBCableTable(void)
{
	#ifndef RFSCAN
	if(CableUSBModeChngmnu && CableWorkingTableChngmun)
	{
			if(GET_SETTING(UsbTabSelNum))
			{
				SaveSettingChangeToTableZero();
				LoadUSBWorkingConfigurationOne();
				SaveSettingChangeToTableOne();
			}
			else
			{
				SaveSettingChangeToTableOne();
				LoadUSBWorkingConfigurationZero();
				SaveSettingChangeToTableZero();
			}
			CableWorkingTableChngmun=false;
			CableUSBModeChngmnu=false;
		}
	else if(CableUSBModeChngmnu && (!CableWorkingTableChngmun))
	{

		if(ReaderConfiguration->GetActiveWorktableNumber())
		{
			SaveSettingChangeToTableOne();
			LoadUSBWorkingConfigurationZero();
			SaveSettingChangeToTableZero();
			LoadUSBWorkingConfigurationOne();
		}
		else
		{
			SaveSettingChangeToTableOne();
			LoadUSBWorkingConfigurationZero();
			SaveSettingChangeToTableZero();
			LoadUSBWorkingConfigurationOne();
		}
		CableUSBModeChngmnu=false;
	}
	#endif
	return true;

}


bool CheckRS232CableMode(void)
{
	bool Status=true;
	#ifndef RFSCAN
	if(ReaderConfiguration->TableSettingCommandisProcessing)
		return false;
	if(Status)
	{
		char temp[16];
		char demp[16];
		sprintf(demp,"%d",ReaderConfiguration->GetActiveWorktableNumber());
		sprintf(temp,"%d",GET_SETTING(RS232TabSelNum));
		if(((strlen(temp)==ReaderConfiguration->MenuCommandResponse->Size)&&
		( memcmp(temp,ReaderConfiguration->MenuCommandResponse->Char,ReaderConfiguration->MenuCommandResponse->Size)==0))||CableWorkingTableChngmun||CableKBWModeChngmnu||CableUSBModeChngmnu||CableRS485ModeChngmun||ReaderConfiguration->FirstTimeLoadWorkingConfigurationInFlash)
		{	CableRS232ModeChngmnu=false;
			if(GET_SETTING(GeneralDebugOn))
				theStdInterface.printf("we need not load new table for 232\r\n");
		}
		else if(!((strlen(temp)==ReaderConfiguration->MenuCommandResponse->Size)&&
		( memcmp(temp,ReaderConfiguration->MenuCommandResponse->Char,ReaderConfiguration->MenuCommandResponse->Size)==0))&&PresentInterface==Interface_232_Group)
		{
			CableRS232ModeChngmnu=true;
			CableWorkingTableChngmun=true;
			ReaderConfiguration->TableNeedChange=true;
			if(GET_SETTING(GeneralDebugOn))
				theStdInterface.printf( "Active Work table is changing from %d to %s\r\n",GET_SETTING(RS232TabSelNum),
				(char*) ReaderConfiguration->MenuCommandResponse->Char);

		}
		else
		{
			CableRS232ModeChngmnu=true;
			CableWorkingTableChngmun=false;
			if(GET_SETTING(GeneralDebugOn))
				theStdInterface.printf("Cable is not 232, and we need to save this setting to two table \r\n");
		}
	}
	#endif
	return Status;
}
//
//ChangeCableTable
//    When calbe table has been changed,reconfigure two table.
//
bool ChangeRS232CableTable(void)
{
	#ifndef RFSCAN
	if(CableRS232ModeChngmnu&& CableWorkingTableChngmun)
	{
			if(GET_SETTING(RS232TabSelNum))
			{
				SaveSettingChangeToTableZero();
				Load232WorkingConfigurationOne();
				SaveSettingChangeToTableOne();
			}
			else
			{
				SaveSettingChangeToTableOne();
				Load232WorkingConfigurationZero();
				SaveSettingChangeToTableZero();
			}
			CableWorkingTableChngmun=false;
			CableRS232ModeChngmnu=false;
		}
	else if(CableRS232ModeChngmnu && (!CableWorkingTableChngmun))
	{

		if(ReaderConfiguration->GetActiveWorktableNumber())
		{
			SaveSettingChangeToTableOne();
			Load232WorkingConfigurationZero();
			SaveSettingChangeToTableZero();
			Load232WorkingConfigurationOne();
		}
		else
		{
			SaveSettingChangeToTableOne();
			Load232WorkingConfigurationZero();
			SaveSettingChangeToTableZero();
			Load232WorkingConfigurationOne();
		}
		CableRS232ModeChngmnu=false;
	}
	#endif
	return true;


}

bool CheckRS485CableMode(void)
{
	bool Status=true;
	#ifndef RFSCAN
	if(ReaderConfiguration->TableSettingCommandisProcessing)
		return false;
	if(Status)
	{
		char temp[16];
		char demp[16];
		sprintf(demp,"%d",ReaderConfiguration->GetActiveWorktableNumber());
		sprintf(temp,"%d",GET_SETTING(RS485TabSelNum));
		if(((strlen(temp)==ReaderConfiguration->MenuCommandResponse->Size)&&
		( memcmp(temp,ReaderConfiguration->MenuCommandResponse->Char,ReaderConfiguration->MenuCommandResponse->Size)==0))||CableWorkingTableChngmun||CableKBWModeChngmnu||CableUSBModeChngmnu||CableRS232ModeChngmnu||ReaderConfiguration->FirstTimeLoadWorkingConfigurationInFlash)
		{	CableRS485ModeChngmun=false;
			if(GET_SETTING(GeneralDebugOn))
				theStdInterface.printf("we need not load new table for RS485\r\n");
		}
		else if(!((strlen(temp)==ReaderConfiguration->MenuCommandResponse->Size)&&
		( memcmp(temp,ReaderConfiguration->MenuCommandResponse->Char,ReaderConfiguration->MenuCommandResponse->Size)==0))&&PresentInterface==Interface_485_Group)
		{
			CableRS485ModeChngmun=true;
			CableWorkingTableChngmun=true;
			ReaderConfiguration->TableNeedChange=true;
			if(GET_SETTING(GeneralDebugOn))
				theStdInterface.printf( "Active Work table is changing from %d to %s\r\n",GET_SETTING(RS485TabSelNum),
				(char*) ReaderConfiguration->MenuCommandResponse->Char);
		}
		else
		{
			CableRS485ModeChngmun=true;
			CableWorkingTableChngmun=false;
			if(GET_SETTING(GeneralDebugOn))
				theStdInterface.printf("Cable is  not 485 and we need to save this setting to two table \r\n");

		}
	}
	#endif
	return Status;
}
//
//ChangeCableTable
//    When calbe table has been changed,reconfigure two table.
//
bool ChangeRS485CableTable(void)
{
	#ifndef RFSCAN
	if(CableRS485ModeChngmun&&CableWorkingTableChngmun)
	{
			if(GET_SETTING(RS485TabSelNum))
			{
				SaveSettingChangeToTableZero();
				Load485WorkingConfigurationOne();
				SaveSettingChangeToTableOne();
			}
			else
			{
				SaveSettingChangeToTableOne();
				Load232WorkingConfigurationZero();
				SaveSettingChangeToTableZero();
			}
			CableWorkingTableChngmun=false;
			CableRS485ModeChngmun=false;
		}
	else if(CableRS485ModeChngmun && (!CableWorkingTableChngmun))
	{

		if(ReaderConfiguration->GetActiveWorktableNumber())
		{
			SaveSettingChangeToTableOne();
			Load485WorkingConfigurationZero();
			SaveSettingChangeToTableZero();
			Load485WorkingConfigurationOne();
		}
		else
		{
			SaveSettingChangeToTableOne();
			Load485WorkingConfigurationZero();
			SaveSettingChangeToTableZero();
			Load485WorkingConfigurationOne();
		}
		CableRS485ModeChngmun=false;
	}
	#endif
	return true;


}

bool CheckKBWCableMode(void)
{
	bool Status=true;
	#ifndef RFSCAN
	if(ReaderConfiguration->TableSettingCommandisProcessing)
		return false;
	if(Status)
	{
		char temp[16];
		char demp[16];
		sprintf(demp,"%d",ReaderConfiguration->GetActiveWorktableNumber());
		sprintf(temp,"%d",GET_SETTING(KBDTabSelNum));
		if(((strlen(temp)==ReaderConfiguration->MenuCommandResponse->Size)&&
		( memcmp(temp,ReaderConfiguration->MenuCommandResponse->Char,ReaderConfiguration->MenuCommandResponse->Size)==0))||CableWorkingTableChngmun||CableUSBModeChngmnu||CableRS232ModeChngmnu||CableRS485ModeChngmun||ReaderConfiguration->FirstTimeLoadWorkingConfigurationInFlash)
		{	CableKBWModeChngmnu=false;
			if(GET_SETTING(GeneralDebugOn))
				theStdInterface.printf("we need not load new table for kbw\r\n");
		}
	else if(!((strlen(temp)==ReaderConfiguration->MenuCommandResponse->Size) &&
		( memcmp(temp,ReaderConfiguration->MenuCommandResponse->Char,ReaderConfiguration->MenuCommandResponse->Size)==0)) && PresentInterface==Interface_KBDWDG_Group)
		{
			CableKBWModeChngmnu=true;
			CableWorkingTableChngmun=true;
			ReaderConfiguration->TableNeedChange=true;
			if(GET_SETTING(GeneralDebugOn))
				theStdInterface.printf( "Active Work table is changing from %d to %s\r\n",GET_SETTING(KBDTabSelNum),
				(char*) ReaderConfiguration->MenuCommandResponse->Char);
		}
	else
		{
			CableKBWModeChngmnu=true;
			CableWorkingTableChngmun=false;
			if(GET_SETTING(GeneralDebugOn))
				theStdInterface.printf("Cable is not keyboard wedge and we need to save this setting to two table \r\n");
		}
	}
	#endif
	return Status;
}
//
//ChangeCableTable
//    When calbe table has been changed,reconfigure two table.
//
bool ChangeKBWCableTable(void)
{
	#ifndef RFSCAN
	if(CableKBWModeChngmnu && CableWorkingTableChngmun)
	{
			if(GET_SETTING(KBDTabSelNum))
			{
				SaveSettingChangeToTableZero();
				LoadKBWWorkingConfigurationOne();
				SaveSettingChangeToTableOne();
			}
			else
			{
				SaveSettingChangeToTableOne();
				LoadKBWWorkingConfigurationZero();
				SaveSettingChangeToTableZero();
			}
			CableWorkingTableChngmun=false;
			CableKBWModeChngmnu=false;
		}
	else if(CableKBWModeChngmnu && (!CableWorkingTableChngmun))
	{

		if(ReaderConfiguration->GetActiveWorktableNumber())
		{
			SaveSettingChangeToTableOne();
			LoadKBWWorkingConfigurationZero();
			SaveSettingChangeToTableZero();
			LoadKBWWorkingConfigurationOne();
		}
		else
		{
			SaveSettingChangeToTableOne();
			LoadKBWWorkingConfigurationZero();
			SaveSettingChangeToTableZero();
			LoadKBWWorkingConfigurationOne();
		}
		CableKBWModeChngmnu=false;
	}
	#endif

	return true;
}

bool CheckTableSelSetting(void)
{

	bool Status=true;
	#ifndef RFSCAN
	char temp[16];
		sprintf(temp,"%d",GET_SETTING(CblSelNumber));
	if(!ProcessingScannedMenu){
		if((strlen(temp)==ReaderConfiguration->MenuCommandResponse->Size)&&
		( memcmp(temp,ReaderConfiguration->MenuCommandResponse->Char,ReaderConfiguration->MenuCommandResponse->Size)==0))
		{	WorkingTableSelChngmnu=false;
			if(GET_SETTING(GeneralDebugOn))
				theStdInterface.printf("we need not change to new table\r\n");
		}
		else
		{
			WorkingTableSelChngmnu=true;
		}
	}

	#endif

		return Status;
}
bool ChangeTableSelSetting(void)
{
	#ifndef RFSCAN
	if(!ProcessingScannedMenu && !ReaderConfiguration->TwoTableUpdateSimultaneously && WorkingTableSelChngmnu && (ReaderConfiguration->CurrentWorkTable()!=ReaderConfiguration->GetActiveWorktableNumber()))
	{
		if(GET_SETTING(GeneralDebugOn))
				theStdInterface.printf("we change to new table\r\n");
		if(ReaderConfiguration->CurrentWorkTable())
		{
			SaveSettingChangeToTableZero();
			LoadWorkingConfigurationOne();
		}
		else
		{
			SaveSettingChangeToTableOne();
			LoadWorkingConfigurationZero();
		}
	}
	#endif
	return true;

}

bool Enter232Standby(void)
{
	if(HardwareToken.IAmOEM())
	{
		if(GET_SETTING( GeneralDebugOn ))
		{
			xputstring("Now Entering Stop Mode\r\n");
			wait_til_tx_is_empty();
		}
		EnterPowerState(STOP);
		return true;
	}
	else
		return false;
}

//Disable or Enable the scanner depending on METDES setting.
bool CheckDEstatus(void)
{
	#ifndef RFBASE
	if( ReaderConfiguration->MenuCommandResponse->Char[0] == '0' )
	{
		theScriptInterpreter.EnableOPOS();
	}
	else
	{
		theScriptInterpreter.DisableOPOS();
	}
	#endif
	return true;
}

#ifndef RFBASE
void TriggerUpdateOposRetail(bool enable)
{
	BOOL updateOpos = FALSE;
	
	//this terminal id maybe not the true one, if we change cable type and recycle power
	//terminal ID is not updated before this function be called
	//so we much judge cable type here.
	switch (GET_SETTING(TerminalID))
	{
		case 51:
			if(PresentInterface == Interface_485_Group)
				updateOpos = TRUE;
			break;
		case 128:
		case 129:
		case 131:
			if(PresentInterface == Interface_USB_Group)
				updateOpos = TRUE;
   	  		break;
		default:
			break;
	}

	if(updateOpos)
	{
		if (enable)
			theScriptInterpreter.EnableOPOS();
		else
			theScriptInterpreter.DisableOPOS();

		theScriptInterpreter.OPOSMenuModeOff();	// dont send opos power up msg	
	}
}

bool SetRetailScannerEnable(void)
{
#ifdef RFSCAN
#else
	switch (ReaderConfiguration->MenuCommandResponse->Char[0])
#endif
	{
		case '0':
			TriggerUpdateOposRetail(false);
			break;
		case '1':
			TriggerUpdateOposRetail(true);
			break;

	}
	return true;
}
#endif


void READER_CONFIGURATION::CreateInterfaceMenuSettings( int ControlMask )
{
	AddBoolean("060100", SUB_TAG_TABLE::FULLY_VISIBLE, NULL, NULL, &OpenInterfaceEnable, false, false );
#if defined(SM4100)
	AddBoolean("061600", SUB_TAG_TABLE::FULLY_VISIBLE, NULL, NULL, &IRorTrigger, true, true );
	AddNumber("061601", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &IRResetTime, 0, 0, 0, 5000);
#elif defined(IR_TRIGGER)
	AddNumericList("061600", SUB_TAG_TABLE::FULLY_VISIBLE, NULL, NULL, &IrTriggerEnable, IR_TRG_OFF, IR_TRG_OFF, IR_TRG_OFF, IR_TRG_ON_STD, IR_TRG_ON_NEAR, IR_TRG_ON_FAR );
#endif

//LinNing for ReM1.0: POSTMO
#if defined( RFBASE ) || defined( RFSCAN )
#else
	AddNumber("060200", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &HModemDownAppReceiveTimeOut, 500, 500, 10, 20000);
#endif
	AddNumericList("060300",SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG|SUB_TAG_TABLE::WORKTABLE_SELECTION ,CheckTableSelSetting,ChangeTableSelSetting,&CblSelNumber,CBL_USB,CBL_USB,CBL_RS232,CBL_KBW,CBL_USB,CBL_RS485,CBL_ALL);
	// HIBAUD:  this must be added before 232BAD.  I believe there are other ordering dependancies with other 232 settings.  TODO:  document these.
	if(HardwareToken.IAmOEM())
	{
		AddBoolean("060400", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | SUB_TAG_TABLE::FIXED,
						NULL, &ExpandBaudRates, &bEngSettings, false, false);  // HIDDEN allow engineering settings such as higher baud rates
	}
	else
	{
		AddBoolean("060400", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG 	,
						NULL, &ExpandBaudRates, &bEngSettings, false, false);  // HIDDEN allow engineering settings such as higher baud rates
	}

//  Terminal Interface Selection Setting.   Use RS232 for development.
//  We switch defaults based on the product configuration
#ifndef ETM
#ifdef EMERALD_VERIFIER
	AddNumericList("060500", SUB_TAG_TABLE::FULLY_VISIBLE , NULL, NULL, &TerminalID, TRM_USB_COMEMU,
			TRM_USB_COMEMU, AUX_RS232, TRM_USB_COMEMU); /*, Allow a minimal list to be added to upon power-up*/
#else
	if(ControlMask & MAKE_USB_KB_WEDGE_DEFAULT)
	{
 		AddNumericList("060500", SUB_TAG_TABLE::FULLY_VISIBLE , NULL, NULL, &TerminalID,TRM_USB_KBD_PC,
 				TRM_USB_KBD_PC,TRM_USB_KBD_PC);/*, Allow a minimal list to be added to upon power-up*/
	}
	else if(ControlMask & MAKE_KB_WEDGE_DEFAULT)
	{
//ewr-02787
 		AddNumericList("060500", SUB_TAG_TABLE::FULLY_VISIBLE , NULL, NULL, &TerminalID,AUX_RS232,
 				AUX_RS232,TRM_IBM_PC_AT,AUX_RS232);/*, Allow a minimal list to be added to upon power-up*/
//
	}
	else if((ControlMask & MAKE_USB_COMM_EMULATION_DEFAULT) || (HardwareToken.IAmOEM() && (RS232_ONLY != GET_CONFIG_SETTING(iInterfacePopulation))))
	{
		AddNumericList("060500", SUB_TAG_TABLE::FULLY_VISIBLE , NULL, NULL, &TerminalID, TRM_USB_COMEMU,
 				TRM_USB_COMEMU, AUX_RS232, TRM_USB_COMEMU);/*, Allow a minimal list of 232 and comm emm, to be added to upon power-up*/
	}
	else
	{
		if(HardwareToken.IAmOEM() || HardwareToken.IAmYoujie4600())
			AddNumericList("060500", SUB_TAG_TABLE::FULLY_VISIBLE , NULL, NULL, &TerminalID,AUX_RS232,
			AUX_RS232,AUX_RS232);/*, Allow a minimal list to be added to upon power-up*/
		else
			AddNumericList("060500", SUB_TAG_TABLE::FULLY_VISIBLE , CheckTermid, UpdateCableInterface, &TerminalID,AUX_RS232,
			AUX_RS232,AUX_RS232);/*, Allow a minimal list to be added to upon power-up*/

	}
#endif

#else
		AddNumericList("060500", SUB_TAG_TABLE::FULLY_VISIBLE , NULL, NULL, &TerminalID,AUX_RS232,
			AUX_RS232,AUX_RS232);/*, Allow a minimal list to be added to upon power-up*/
#endif

#if defined(DISABLE_AUTO_CBL)
//Interface type 0--232;1--USB Group
	AddNumber("060600", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::DEFAULTING_DISABLED , NULL, NULL, &InterfaceType, 0, 0, 0, 2);
#endif

	/* NGOEM-199 Add a new sugtage command to unlock RS232 interface */
	if(HardwareToken.IAmOEM() && (RS232_ONLY != GET_CONFIG_SETTING(iInterfacePopulation)))
		AddBoolean("060700", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG ,Update232Lock, NULL, &RS232Unlock, false, false);

	if( (HardwareToken.IAmOEM()) || (HardwareToken.IAmYoujie4600()))  //NGOEM-109
	{
		//NGOEM-150 changed modifier of TRM* from sub tag to NULL to not show these commands in OEM
		AddNumericList("060800",NULL,NULL,NULL,&Term232ID,AUX_RS232,AUX_RS232,AUX_RS232);
		AddNumericList("060801",NULL,NULL,NULL,&TermKBWID,TRM_IBM_PC_AT,TRM_IBM_PC_AT,TRM_IBM_PC_AT);
		AddNumericList("060802",NULL,NULL,NULL,&TermUSBID,TRM_USB_KBD_PC,TRM_USB_KBD_PC,TRM_USB_KBD_PC);
		AddNumericList("060803",NULL,NULL,NULL,&Term485ID,TRM_IBM_4683,TRM_IBM_4683,TRM_IBM_4683);
	}
	else  //NGOEM-109
	{
		AddNumericList("060800",SUB_TAG_TABLE::FULLY_VISIBLE,NULL,NULL,&Term232ID,AUX_RS232,AUX_RS232,AUX_RS232);
		AddNumericList("060801",SUB_TAG_TABLE::FULLY_VISIBLE,NULL,NULL,&TermKBWID,TRM_IBM_PC_AT,TRM_IBM_PC_AT,TRM_IBM_PC_AT);
		AddNumericList("060802",SUB_TAG_TABLE::FULLY_VISIBLE,NULL,NULL,&TermUSBID,TRM_USB_KBD_PC,TRM_USB_KBD_PC,TRM_USB_KBD_PC);
		if ( HardwareToken.IAmIndustrial() || HardwareToken.IAmYoujie4600()) Term485ID = 0; // this appears to be unused.
		else AddNumericList("060803",SUB_TAG_TABLE::FULLY_VISIBLE,NULL,NULL,&Term485ID,TRM_IBM_4683,TRM_IBM_4683,TRM_IBM_4683);
	}

//	Decoded Output Asynchronous ASCII Settings
	AddCommand("060701", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);


#if defined( DEVELOPMENT_SETTINGS )		//	Use 230400, N,8,1 for development
	AddNumericList("060702", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &RS232BaudRate,
			RCFG_BAUD_115200, RCFG_BAUD_115200,
			RCFG_BAUD_300, RCFG_BAUD_600, RCFG_BAUD_1200, RCFG_BAUD_2400, RCFG_BAUD_4800, RCFG_BAUD_9600,
			RCFG_BAUD_19200, RCFG_BAUD_38400, RCFG_BAUD_57600, RCFG_BAUD_115200, RCFG_BAUD_230400,
			RCFG_BAUD_460800, RCFG_BAUD_921600);
	AddNumericList("060703", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &RS232WordFormat, SER_WORD_N81, SER_WORD_N81,
			SER_WORD_N71, SER_WORD_N72, SER_WORD_N81, SER_WORD_E71, SER_WORD_E72, SER_WORD_E81, SER_WORD_O71, SER_WORD_O72,
			SER_WORD_O81, SER_WORD_M81);
#else		//	Use these parameters for release
	#if defined( RFBASE )
	#else
		AddNumericList("060702", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &RS232BaudRate,
		RCFG_BAUD_115200, RCFG_BAUD_115200,
		RCFG_BAUD_300, RCFG_BAUD_600, RCFG_BAUD_1200, RCFG_BAUD_2400, RCFG_BAUD_4800, RCFG_BAUD_9600,
		RCFG_BAUD_19200, RCFG_BAUD_38400, RCFG_BAUD_57600, RCFG_BAUD_115200);
	#endif

		AddNumericList("060703", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &RS232WordFormat, SER_WORD_N81, SER_WORD_N81,
			SER_WORD_N71, SER_WORD_N72, SER_WORD_N81, SER_WORD_E71, SER_WORD_E72, SER_WORD_E81, SER_WORD_O71, SER_WORD_O72,
			SER_WORD_O81, SER_WORD_M81);
#endif

//ewr-02787
	AddNumericList("060704", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &RS232CtsEnable, CTSOFF, CTSOFF,CTSOFF,HNDSHK,FLOW,CTSMAN,CTSFST,SCANCTRL,CTSPLS);
//
	AddBoolean("060705", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &RS232XonEnable, false, false);
	AddBoolean("060706", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &RS232AckEnable, false, false);
	AddNumber("060707", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &RS232LowPowerTimeOut, 0, 0, 0, 300);// Zero is off
//ewr-02787
	AddNumber("060708", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &RS232CtsDelay, 3100, 3100, 0, 5100);// Zero is off, works only when CTSMAN enabled
	AddNumericList("060709", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &RS232PacketMode, PKTOFF, PKTOFF,PKTOFF,PKTTWO);   // Zero is off, works only when CTSFST enabled,
	AddBoolean("06070A", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &RS232PacketModeNakEnable, false, false);      // false is off, works only when CTSFST enabled, and RS232PacketMode selected
   AddNumber("06070B", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &RS232AckNakDelay, 5100, 5100, 0, 30000);// Zero is off, timeout for packet mode Ack/Nak
//
//ewr-05579
	AddBoolean("060900", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG, &CheckDEstatus, NULL, &RS232SerialDisableEnable, false, false);      // false is off, works only when CTSFST enabled, and RS232PacketMode selected
//
	if(HardwareToken.IAmOEM())
		AddCommand("06070C", SUB_TAG_TABLE::FULLY_VISIBLE, Enter232Standby,NULL);
	// settings for USB ComPort emulation
	AddBoolean("060A00", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &UsbCtsEnable, false, false);
	AddBoolean("060A01", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &UsbAckEnable, false, false);
	AddNumber("060A02",  SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG ,NULL, NULL, &UsbReEnumerationTimeout, 2000, 2000, 0, 300000);
	AddBoolean("060A03", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  , NULL, NULL, &UsbFifoErrEnable, true, true);
	if(HardwareToken.IAmOEM())
	{
		int ioemModel = GET_CONFIG_SETTING(iInterfacePopulation);
		switch ( ioemModel)
		{
			case PSEUDO_KBW_4683_USB :
			case PSEUDO232_USB :
				if(OEM_TOKEN_HISPEED == HardwareToken.InterfaceToken())
				{
					AddBoolean("060A04", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  , NULL, NULL, &UsbHighspEnable, true, true);
				}
				else
				{
					AddBoolean("060A04", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG| SUB_TAG_TABLE::FIXED   , NULL, NULL, &UsbHighspEnable, false, false);
				}
			break;
			//under this condition we need not support menu command USPSPD.
			case  RS232_ONLY:
			break;
			default :
				AddBoolean("060A04", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG| SUB_TAG_TABLE::FIXED   , NULL, NULL, &UsbHighspEnable, false, false);
			break;
		}
	}
	else
	{
		AddBoolean("060A04", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  , NULL, NULL, &UsbHighspEnable, false, false);
	}
#ifdef RFSCAN
	// ReM doesn't make much sense for RFSCAN.
	RemIfaceEnable = false;
	RemRouting = 0;
#else
	bool bRemIsEnabled = false;
#if (defined(XENON_LITE_1500) || defined(VUQUEST3310))
	bRemIsEnabled = true;
#endif
	if ( HardwareToken.IAmVoyager1400() ) 
	{
		if(HardwareToken.IAmYoujie4600())
			bRemIsEnabled = false;	
		else	
			bRemIsEnabled = true;
	}
	if ( HardwareToken.IAmIndustrial() ) bRemIsEnabled = true;
	AddBoolean("060B00", SUB_TAG_TABLE::FULLY_VISIBLE, NULL, NULL, &RemIfaceEnable, bRemIsEnabled, bRemIsEnabled);
	// Note:  the ReM spec says to treat a '.' as a '!', but we were also told to behave like Voyager.  Voyager NAKs a '.' and doesn't update.  We will ENQ and not update.
	AddNumber("060B01", SUB_TAG_TABLE::FIXED_EXCEPT_ACTIVE | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG, NULL, NULL, &RemRouting, 0, 0, 0, 1);
#endif // endof #ifdef RFSCAN
	AddNumber("060301", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::DEFAULTING_DISABLED |SUB_TAG_TABLE::CABLE_SETTING ,CheckUSBCableMode,ChangeUSBCableTable,&UsbTabSelNum,0,0,0,1);
	AddNumber("060302",	SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::DEFAULTING_DISABLED |SUB_TAG_TABLE::CABLE_SETTING ,CheckRS232CableMode,ChangeRS232CableTable,&RS232TabSelNum,0,0,0,1);
	AddNumber("060303", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::DEFAULTING_DISABLED |SUB_TAG_TABLE::CABLE_SETTING ,CheckKBWCableMode,ChangeKBWCableTable,&KBDTabSelNum,0,0,0,1);
	if ( HardwareToken.IAmIndustrial() ) RS485TabSelNum = 0;
	else AddNumber("060304", SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::DEFAULTING_DISABLED |SUB_TAG_TABLE::CABLE_SETTING ,CheckRS485CableMode,ChangeRS485CableTable,&RS485TabSelNum,0,0,0,1);
	AddNumber("060305", SUB_TAG_TABLE::FULLY_VISIBLE, NULL, NULL, &CBLTestMenu,0,0,0,1);

#ifdef EMERALD_VERIFIER
	// some USB interfaces can turn off the serial number
	AddNumericList("060A05", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &UsbSerialNumber, USB_SERNUM_OFF, USB_SERNUM_OFF,
			USB_SERNUM_OFF, USB_SERNUM_ON);
#else
	// some USB interfaces can turn off the serial number
	AddNumericList("060A05", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &UsbSerialNumber, USB_SERNUM_ON, USB_SERNUM_ON,
			USB_SERNUM_OFF, USB_SERNUM_ON);
#endif

	AddNumericList("060C00", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  , NULL, NULL, &SerWdgConfig, TO_P1, TO_P1,
			TO_P1_AND_P2, TO_P1, TO_P2);


//	Decoded Output Keyboard Wedge Settings
	AddCommand("060D00", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);
	AddNumber("060E00", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &KBD_CtryCode, CTY_USA, CTY_USA, CTY_USA, CTY_MAX_COUNTRY);

	AddNumericList("060D01", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &KBD_Style, STY_STANDARD, STY_STANDARD,
			STY_STANDARD, STY_CAPS_LOCK, STY_SHIFT_LOCK, STY_EMULATE_EXT, STY_AUTOCAPS_LOCK,
			STY_AUTOCAPS_LOCK_VIA_NUMLOCK);
	  AddNumericList("060D02", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &KBD_Mode,      MOD_STANDARD, MOD_STANDARD, MOD_STANDARD, MOD_UPPER, MOD_LOWER);
	AddBoolean("060D03", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &KBD_AutoDirCon, false, false);
	AddBoolean("060D04", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &KBD_TurboMode, false, false);
	AddBoolean("060D05", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &KBD_NumKeypad, false, false);
	AddBoolean("060D06", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &KBD_TransDeadKey, true, true);
	AddNumericList("060D07", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &KBD_CtrlASCII, CNTRL_ASCII_OFF, CNTRL_ASCII_OFF,
			CNTRL_ASCII_OFF, CNTRL_ASCII_DOS, CNTRL_ASCII_WIN,CNTRL_ASCII_ESC);
	AddNumericList("060D08", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &KBD_TransAltNum, ALT_OFF, ALT_OFF,
			ALT_OFF, ALT_WINDOWS, ALT_DOS, ALT_UNICODE, ALT_WINDOWS_SYS_CP, ALT_DOS_SYS_CP);
	AddBoolean("060D09", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &KBD_TransHtml, false, false);
	AddBoolean("060D0A", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &KBD_TransHex, false, false);
	AddBoolean("060D0B", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &KBD_NonPrint, false, false);


//	Wand Emulation Output Settings
// Retail Interface Control Settings
	AddNumericList("060F00", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  , NULL, NULL, &IBM_4683_Protocol,IBM_4683_PORT_5B, IBM_4683_PORT_5B,
	   		IBM_4683_PORT_5B, IBM_4683_PORT_9B_HHBCR1, IBM_4683_PORT_9B_HHBCR2, IBM_4683_PORT_17);
    AddNumericList("060F01", SUB_TAG_TABLE::FULLY_VISIBLE, NULL, NULL, &IBM_4683_Serial, IBM_4683_PORT_NULL, IBM_4683_PORT_NULL,
        IBM_4683_PORT_NULL, IBM_4683_PORT_23_ADDR68, IBM_4683_PORT_23_ADDR69, IBM_4683_PORT_25_ADDR64,IBM_4683_PORT_25_ADDR65);

	AddBoolean("060F02", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  , NULL, NULL, &IBM_4683_IgnoreFirstDisable, false, false);
	AddBoolean("060F03", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  , NULL, NULL, &UsbSurePosIgnCfg, false, false);
	AddBoolean("060F04", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  , NULL, NULL, &UsbSurePosIgnDis, false, false);
	AddBoolean("060F05", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  , NULL, NULL, &UsbSurePosIgnBep, false, false);     //cvs_cust_default==true, standard==false
	AddBoolean("060F06", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  , NULL, NULL, &UsbSurePosSelectOCR, false, false);
	AddBoolean("060F07", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &UsbSurePosUseBCD, false, false);
	AddNumericList("060F08", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  , NULL, NULL, &OCIA_Protocol, NCR_SHORT, NCR_SHORT,
	        NCR_SHORT, NCR_LONG, NIXDORF, SPECTRA_PHYSICS);
	AddNumber("060F09", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  , NULL, NULL, &UsbSurePosGenNum, 0, 0, 0, 255);
	AddNumber("060F0A", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  , NULL, NULL, &UsbSurePosLevNum, 0, 0, 0, 255);



// HHLC Output Settings
	AddCommand("061000", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  , NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);
	AddNumericList("061001", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  , NULL, NULL, &HHLCRate, HHLC_36, HHLC_36,
		    HHLC_36, HHLC_100);
	AddNumericList("061002", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  , NULL, NULL, &HHLCPolarity, POL_WHITE_HI, POL_WHITE_HI,
			POL_BLACK_HI, POL_WHITE_HI);
	AddNumericList("061003", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  , NULL, NULL, &HHLCIdleState, IDLE_HI, IDLE_HI,
			IDLE_LO, IDLE_HI);
	// smallest module in uSec, if smaller we stretch SOS
	AddNumber("061004", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  , NULL, NULL, &HHLCMinModuleSize, 30, 30, 5, 500);
	// Stretch SOS in %, 100% = no stretching
	AddNumber("061005", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  , NULL, NULL, &HHLCStretchFactor, 500, 500, 100, 1000);
	// We zoom and center the space/bar pattern. 100% = full SOS time, 80% gives similar quietzones as WandEm
	AddNumber("061006", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  , NULL, NULL, &HHLCZoomFactor, 20, 20, 10, 100);
	// Timeout, after more than X tries (SOS windows) we give up.
	// This needs to be set higher than the voting in the external decoder
	AddNumber("061007", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  , NULL, NULL, &HHLCVotingTimeout, 100, 100, 1, 1000);
	AddNumericList("061008", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG  , NULL, NULL, &HHLCLaserEnablePolarity, HHLC_LAS_ENABLE_POLARITY_AUTO, HHLC_LAS_ENABLE_POLARITY_AUTO,
		    HHLC_LAS_ENABLE_POLARITY_AUTO, HHLC_LAS_ENABLE_POLARITY_LOW, HHLC_LAS_ENABLE_POLARITY_HIGH);


// OEM Settings
	if(HardwareToken.IAmOEM())
	{
		AddBoolean("061100", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &GoodReadLedInvert, false, false);
		//0=pwm active low(ie -idle high)    1=pwm active high (ie - idle low)      2=dc active low    3=dc active high
		AddNumber("061200", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &BeeperInvert, 0,0,0,3);
		AddNumber("06070D", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &RS232InvEnable, 2,2,0,2);//0=Normal Only, 1=NonInverted Only, 2=Normal(w/cable select)
	}

#if WALMART_DEBUG	// Debugging commands for flashing over trickle feed/flashscan for NCR/Walmart
#ifndef RFSCAN
	AddBoolean("061300", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG, NULL, NULL, &AllowWalMartFlash, true, true);
	AddBoolean("061400", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG, NULL, NULL, &EnableWalmartDebug, true, true);
#endif
#endif 

// Post decode settings
	AddNumericList("061500", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &EciTranslateMode, ECI_TRANSLATE_ASK_DRIVER, ECI_TRANSLATE_ASK_DRIVER,
			ECI_TRANSLATE_OFF, ECI_TRANSLATE_ON, ECI_TRANSLATE_ASK_DRIVER);
	AddNumericList("061501", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &EciSystemCodepage, 2, 2,
			// I did not want to include codepage.h here
			2,10,11,12,13,14,15,16,17,18, 30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,51,52,53,54,55,56,57,58,59,60,61,63,64,65,66,
			70,71,75,76,77,78,79);
	AddNumericList("061502", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &EciEnable, ECI_OFF, ECI_OFF,
			ECI_OFF, ECI_ON, ECI_ON|ECI_BUFFER_MACROS, ECI_ON|ECI_DIAGNOSTIC, ECI_ON|ECI_BUFFER_MACROS|ECI_DIAGNOSTIC );
	AddNumericList("061503", SUB_TAG_TABLE::FULLY_VISIBLE  , NULL, NULL, &EciOutputFormat, ECI_OUTPUT_BINARY, ECI_OUTPUT_BINARY,
			ECI_OUTPUT_BINARY, ECI_OUTPUT_SYSCP, ECI_OUTPUT_UTF8, ECI_OUTPUT_UTF16BE, ECI_OUTPUT_UTF16LE);

	AddNumber("060A06", SUB_TAG_TABLE::FIXED_EXCEPT_ACTIVE   | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | SUB_TAG_TABLE::IGNORE_ON_STORAGE_QUERY,
			NULL, NULL, &NumberOfUSBRestarts, 0, 0, 0, 0x7FFFFFFF);


	AddUnicodeString("060A07", SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | SUB_TAG_TABLE::DEFAULTING_DISABLED,
			NULL, NULL, &USBSerNum, SIZE_OF_SER_NUM, SER_NUM, SER_NUM);
	HSTRING		ASCIISerialNumber( GET_CONFIG_SETTING( SerNum ), true );
	ASCIISerialNumber.UniCodeToSimpleAsciiFormat();

	if( ASCIISerialNumber.Compare( SER_NUM, true ) == 0 )        //If product config serial number is not configured
	{
		ClearFixedAttribute( "060A07" );                         //allow this setting to be read from flash
	}

	HSTRING		SerialNumberCommandString( 80, true );
	SerialNumberCommandString.Copy( "060A07" );
	SerialNumberCommandString.Append( &ASCIISerialNumber );
	SerialNumberCommandString.Append( "." );
	Menu( &SerialNumberCommandString, true );

	AddBoolean("060F0B", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG,	&SetRetailScannerEnable, NULL, &RetailScannerEnable, false, false);

	return;
}



