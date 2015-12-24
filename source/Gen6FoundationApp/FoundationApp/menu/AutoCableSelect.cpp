//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/menu/AutoCableSelect.cpp $
// $Revision: 1.46 $
// $Date: 2011/11/10 03:25:10EST $
//======================================================================================

//======================================================================================

#include "stdio.h"
#include "stdlib.h"
#include "string.h"		//For strlen

#include"AutoCableSelect.h"
#include "HardwareMacros.h"		//Provides Macros for setting registers
#include "iMX25_IOMUX.h"
#include "iMX25_GPIO.h"
#include "language.h"
#include "ADC_Driver.h"
#include "MenuFlash.h"
#include "ReaderConfiguration.h"
#include "Menu.h"
#include "Ioman.h"
#include "Interface_support_functions.h"
#include "interrupts.h"
#include "db_hdw.h"
#include "product.h"
#include "HardwareToken.h"
#include "StartupFlashDisk.h"

#include "Db_chrio.h"    //edited by Chuck
//#include "StartupFlashDisk.h"                     //lmc debug
//static unsigned int gReadCableVoltage = 0;         //lmc debug

CABLE_STATES PresentInterface = Interface_Unknow;
CABLE_STATES LastInterface;
extern bool bFactoryTestMode;
extern void LoadWorkingConfigurationZero();
extern void LoadWorkingConfigurationOne();

//ADC Formula: 2V*(2^12)/3.3V = 2483 2V = target voltage, 3.3V = ADC input voltage, 2^12 = 12bit ADC
#define INTERFACE_BOUNDRY_VALUE_1	117		//~151 mV
#define INTERFACE_BOUNDRY_VALUE_2 	559 	//~450 mV
#define INTERFACE_BOUNDRY_VALUE_3 	2234	//~1.8 V

// ===Function ======================================================================
//
//Name AutoCable_StrIndex
//
// Description: Helper function to find the substring in a string. HSTRING->FindString doesn't work
//
// - PARAMETER-----------------------------------------------------------------------------
//   Mode    Type           Name            Description
//-----------------------------------------------------------------------------------------
//	in: 	unsigned char*    str		pointer of the string
//   in:	unsigned char*  substr		pointer of substring
//   return:	int					return the start position of the substring in the string
//----------------------------------------------------------------------------------------------
int AutoCable_StrIndex(unsigned char *str,unsigned char *substr)
{
   int end,i,j;

   end = strlen((char*)str) - strlen((char*)substr);  /* End position */
   if ( end > 0 )
   {
      for ( i = 0; i <= end; i++ )
         for ( j = i; str[j] == substr[j-i]; j++ )
            if ( substr[j-i+1] == '\0' )
               return i;
   }
   return -1;
}

UINT ReadCableSelADCVoltage(void);
// ===Function ======================================================================
//
//Name AutoCable_Ctl_Sys_Initialization
//
// Description: Initial Configuration of GPIO Initialization and ADC
//   Initialization take place here. Also sets up Interrupts and Events used by the AutoCable Select system
//
// - PARAMETER-----------------------------------------------------------------------------
//   Mode    Type           Name            Description
//-----------------------------------------------------------------------------------------
//		in:
//      in-out:
//          out:
//      return:
//-------------------------------------------------------------------------------------------------------
void AutoCable_Ctl_Sys_Initialization(void)
{
#ifndef RFSCAN
	if(!(HardwareToken.IAmYoujie4600()) && !(HardwareToken.IAmOEM()) && !(HardwareToken.IAmChargeOnlyBaseType()))
	{
		DISABLE_INTERRUPTS();
		//BANK_SEL  Port1_3
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_GPIO_D,MUX_MODE_0);
		HAZARD_WRITE_FIELD_TO_REG(GPIO1_GDIR0,GPIO_BIT3_LSH,GPIO_BIT3_WID,GPIO_GDIR_INPUT);
		HAZARD_WRITE_FIELD_TO_REG(GPIO1_DR0,GPIO_BIT3_LSH,GPIO_BIT3_WID,0x1);
		
		//CABLE_SEL0 Port1_12
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_I2C1_CLK,MUX_MODE_5);
		HAZARD_WRITE_FIELD_TO_REG(GPIO1_GDIR0,GPIO_BIT12_LSH,GPIO_BIT12_WID,GPIO_GDIR_INPUT);
		HAZARD_WRITE_FIELD_TO_REG(GPIO1_DR0,GPIO_BIT12_LSH,GPIO_BIT12_WID,0x1);

		//CABLE_SEL1  Port1_13
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_I2C1_DAT,MUX_MODE_5);
		HAZARD_WRITE_FIELD_TO_REG(GPIO1_GDIR0,GPIO_BIT13_LSH,GPIO_BIT13_WID,GPIO_GDIR_INPUT);
		HAZARD_WRITE_FIELD_TO_REG(GPIO1_DR0,GPIO_BIT13_LSH,GPIO_BIT13_WID,0x1);


		RESTORE_INTERRUPTS();
	
		PresentInterface=AutoCable_Initialize_Interface();
		LastInterface=PresentInterface;
	}
#endif

}

// ===  Function  ======================================================================
//
//          Name: AutoCable_Initialize_Interface
//
//  Description: Read ADC and Bank_Sel value to judge which interface is connected to.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type              Name          Description
//--------------------------------------------------------------------
//   in:
//   in-out:
//   out:
//   return:
//-------------------------------------------------------------------------------------------------------
CABLE_STATES AutoCable_Initialize_Interface()
{
#if defined(DISABLE_AUTO_CBL)
	return Interface_Unknow; 
#else

	if (0x01 == READ_FIELD(GPIO1_PSR0,GPIO_BIT12_LSH,GPIO_BIT12_WID))
	{
		//DEBUG_puts("USB Cable\r\n"); //edited by Chuck
		return Interface_USB_Group;
	}
	else
	{
		return Interface_232_Group;
	}
#endif
}

#if defined(DISABLE_AUTO_CBL)
// ===Function ======================================================================
//
//Name ManualCable_Ctl_Sys_Initialization
//
// - PARAMETER-----------------------------------------------------------------------------
//   Mode    Type           Name            Description
//-----------------------------------------------------------------------------------------
//		in:
//      in-out:
//          out:
//      return:
//-------------------------------------------------------------------------------------------------------
void ManualCable_Ctl_Sys_Initialization(void)
{
	if (1 == GET_SETTING(InterfaceType))
	{
		PresentInterface = Interface_USB_Group;
	}
	else
	{
		PresentInterface = Interface_232_Group;
	}
	LastInterface=PresentInterface;
}
#endif

// ===  Function  ======================================================================
//
//  Name: AutoCable_Ctl_Sys_Control
//
//  Description:  
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void AutoCable_Ctl_Sys_Control()
{
#if defined(DISABLE_AUTO_CBL)
	static int lastInterfaceType = GET_SETTING(InterfaceType);

	if(GET_SETTING(InterfaceType) != lastInterfaceType)
	{
		reset_to_boot_code_hard();
	}
#endif

	if(!bFactoryTestMode)
	{
		UpdateOpenInterfaceID();
	}
}

// ===  Function  ======================================================================
//
//  Name: ConfigCable_Ctl_Sys_Control
//
//  Description:  if we find menu command TERMID value changed, we should store it in
//                default setting area and reset.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void ConfigCable_Ctl_Sys_Control()
{
	static int lastTermid = GET_SETTING(TerminalID);
	BOOL initDone = FALSE;

	if(GET_SETTING(TerminalID) != lastTermid)
		reset_to_boot_code_hard();

	if(!initDone)
	{
		/*
		  YJ4600 hardware don't support auto-ock KBD_SW_CNTRL when usb cable is pluged
		  so, we must initate it with low status in case of current leakage.
		*/
		if(HardwareToken.IAmYoujie4600() && (lastTermid != TRM_IBM_PC_AT))
		{
			//GPIO2, pin 12, Function: KBD_SW_CNTRL, initiate it with output low status
			HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_EB0,MUX_MODE_5);
			HAZARD_WRITE_FIELD_TO_REG(GPIO2_GDIR0,GPIO_BIT12_LSH,GPIO_BIT12_WID,GPIO_GDIR_OUTPUT);
			HAZARD_WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT12_LSH,GPIO_BIT12_WID,0x0);
		}
		switch(lastTermid)
		{
			default:
			case AUX_RS232:
				PresentInterface = Interface_232_Group;
				break;
			case TRM_IBM_PC_AT:	
				PresentInterface = Interface_KBDWDG_Group;
				break;
			case TRM_USB_KBD_PC:
			case TRM_USB_KBD_MAC:
			case TRM_IBM_SUREPOS_HH:
			case TRM_IBM_SUREPOS_TT:
			case TRM_USB_COMEMU:
			case TRM_USB_HIDPOS:
			case TRM_USB_JAP_KBD:
				PresentInterface = Interface_USB_Group;
				break;
		}
		LastInterface = PresentInterface;
		initDone = TRUE;
	}
}

//======Function===================================================================
//
// Name: Cable_Sel_Sys_stateChangeToUnknow
//
//Description: when cable is unplugged from the interface
//
//---------------------------------------------------------------------------------------------
void Cable_Sel_Sys_stateChangeToUnknow(void)
{
	theStdInterface.Close();
}

//======Function===================================================================
//
//Name: Cable_Sel_Sys_stateChangeTo232Group
//
//Description: when 232 cable is plugged into the device
//
//---------------------------------------------------------------------------------------------

void Cable_Sel_Sys_StateChangeTo232Group(void)
{
	if((LastInterface!=PresentInterface)&&bVisMnuOverride)
		reset_to_boot_code_hard();
	if(GET_SETTING(RS232TabSelNum)!=ReaderConfiguration->GetActiveWorktableNumber())
	{
		if(GET_SETTING(RS232TabSelNum))
		{
			SaveSettingChangeToTableZero();
			LoadWorkingConfigurationOne();
			ReaderConfiguration->SetWorkTable(1);
			ReaderConfiguration->SetActiveWorkTable(1);
			ReaderConfiguration->SetCurrentTableForScannedCommand(1);
		}
		else
		{
			SaveSettingChangeToTableOne();
			LoadWorkingConfigurationZero();
			ReaderConfiguration->SetWorkTable(0);
			ReaderConfiguration->SetActiveWorkTable(0);
			ReaderConfiguration->SetCurrentTableForScannedCommand(0);
		}
	}

	//ReaderConfiguration->UpdateNumericListWorkingValue("TERMID","TRM232");
	UpdateNumericListWorkingValue("060500","060800");
	ReaderConfiguration->UpdateNumericListWorkingDefaltValue("060300",0);
	
	if(!bVisMnuOverride)
		ReconfigureInterface();

}

//=====Function===================================================================
//
//Name: Cable_Sel_Sys_StateChangeToUSBGroup
//
//Description: when USB cable is plugged into the device
//
//---------------------------------------------------------------------------------------------
void Cable_Sel_Sys_StateChangeToUSBGroup(void)
{
	if((LastInterface!=PresentInterface)&&bVisMnuOverride)
		reset_to_boot_code_hard();

	if(GET_SETTING(UsbTabSelNum)!=ReaderConfiguration->GetActiveWorktableNumber())
	{
		if(GET_SETTING(UsbTabSelNum))
		{
			SaveSettingChangeToTableZero();
			LoadWorkingConfigurationOne();
			ReaderConfiguration->SetWorkTable(1);
			ReaderConfiguration->SetActiveWorkTable(1);
			ReaderConfiguration->SetCurrentTableForScannedCommand(1);
		}
		else
		{
			SaveSettingChangeToTableOne();
			LoadWorkingConfigurationZero();
			ReaderConfiguration->SetWorkTable(0);
			ReaderConfiguration->SetActiveWorkTable(0);
			ReaderConfiguration->SetCurrentTableForScannedCommand(0);
		}
	}

	//ReaderConfiguration->UpdateNumericListWorkingValue("TERMID","TRMUSB");
	UpdateNumericListWorkingValue("060500","060802");
	ReaderConfiguration->UpdateNumericListWorkingDefaltValue("060300",2);
	
	if(!bVisMnuOverride)
		ReconfigureInterface();
}
//=====Function====================================================================
//
//Name: Cable_Sel_Sys_StateChangeToKBWGroup
//
//Description: when Keyboard Wedge cable is plugged into the device
//
//---------------------------------------------------------------------------------------------
void Cable_Sel_Sys_StateChangeToKBWGroup(void)
{
	if((LastInterface!=PresentInterface)&&bVisMnuOverride)
		reset_to_boot_code_hard();
	if(GET_SETTING(KBDTabSelNum)!=ReaderConfiguration->GetActiveWorktableNumber())
	{
		if(GET_SETTING(KBDTabSelNum))
		{
			SaveSettingChangeToTableZero();
			LoadWorkingConfigurationOne();
			ReaderConfiguration->SetWorkTable(1);
			ReaderConfiguration->SetActiveWorkTable(1);
			ReaderConfiguration->SetCurrentTableForScannedCommand(1);
		}
		else
		{
			SaveSettingChangeToTableOne();
			LoadWorkingConfigurationZero();
			ReaderConfiguration->SetWorkTable(0);
			ReaderConfiguration->SetActiveWorkTable(0);
			ReaderConfiguration->SetCurrentTableForScannedCommand(0);
		}
	}

	//ReaderConfiguration->UpdateNumericListWorkingValue("TERMID","TRMKBW");
	UpdateNumericListWorkingValue("060500","060801");
	ReaderConfiguration->UpdateNumericListWorkingDefaltValue("060300",1);
	
	if(!bVisMnuOverride)
		ReconfigureInterface();
}
//====Function======================================================================
//
//Name: Cable_Sel_Sys_StateChangeTo485Group
//
//Description: when 485 cable is plugged into the device
void Cable_Sel_Sys_StateChangeTo485Group(void)
{
	if((LastInterface!=PresentInterface)&&bVisMnuOverride)
		reset_to_boot_code_hard();
	if(GET_SETTING(RS485TabSelNum)!=ReaderConfiguration->GetActiveWorktableNumber())
	{
		if(GET_SETTING(RS485TabSelNum))
		{
			SaveSettingChangeToTableZero();
			LoadWorkingConfigurationOne();
			ReaderConfiguration->SetWorkTable(1);
			ReaderConfiguration->SetActiveWorkTable(1);
			ReaderConfiguration->SetCurrentTableForScannedCommand(1);
		}
		else
		{
			SaveSettingChangeToTableOne();
			LoadWorkingConfigurationZero();
			ReaderConfiguration->SetActiveWorkTable(0);
			ReaderConfiguration->SetCurrentTableForScannedCommand(0);
		}
	}

	//ReaderConfiguration->UpdateNumericListWorkingValue("TERMID","TRMRTL");
	UpdateNumericListWorkingValue("060500","060803");
	ReaderConfiguration->UpdateNumericListWorkingDefaltValue("060300",3);
	
	if(!bVisMnuOverride)
		ReconfigureInterface();
}


//======Function====================================================================
//
//	Name: ReadCableSelADCVoltage
//
//Discription: reads the CableSel ADC Voltage
//
// - PARAMETER-------------------------------------------------------------
//   Mode	Type	Name	Discription
//-------------------------------------------------------------------------
//	in:
//	in-out:
//	out:
//	return: UINT						ADC value of CABLE_SEL_ANA
//-------------------------------------------------------------------------
UINT ReadCableSelADCVoltage(void)
{
	UINT  ADC_Value=0;
	ADC_Value=ReadADCChannel(XP,0XA);
	return ADC_Value;
}



//======Function====================================================================
//
//	Name: UpdateNumericListWorkingValue
//
//Discription: Sets Term Id
//
// - PARAMETER-------------------------------------------------------------
//   Mode	Type	Name	Discription
//-------------------------------------------------------------------------
//	in:
//	in-out:
//	out:
//	return: bool						
//------------------------------------------------------------------------- 
bool UpdateNumericListWorkingValue(const char *SettingName1,const char*SettingName2)
{
	bool Match = false;
	bool Succeeded = true;			// Assume this operation works until we know otherwise.
	HSTRING	ConfigurationStorage = HSTRING(32, true);	// Temporary storage for configuration data
	HSTRING	ConfigurationStorage1 = HSTRING(32, true);	// Temporary storage for configuration data

	ConfigurationStorage1.Copy( SettingName1 );
	ConfigurationStorage.Copy( SettingName2 );
	ConfigurationStorage.Append( "$." );
	ConfigurationStorage1.Append( "$." );			   
	ReaderConfiguration->Menu( &ConfigurationStorage );
	ReaderConfiguration->Menu( &ConfigurationStorage1 );

	if( (ConfigurationStorage.Size >= 7) && (ConfigurationStorage1.Size >= 7) )
	{
	 	if( (ConfigurationStorage.Size) == (ConfigurationStorage1.Size) )
		{
		 	int count = ConfigurationStorage.Size - 6;
			int i = 0;

			for(i = 0; i < count; i++)
			{
				if( ConfigurationStorage.Char[6 + i] == ConfigurationStorage1.Char[6 + i] )
				{
				 	Match = true;					
				}
				else
				{
 					Match = false;
					break;
				}
			}
		}	  
	}
	else
	{
		Match = true;	        //menu command passed not big enough, do not send to menu 
	}

	if( !Match )               //if not a match, menu it
	{
		memcpy(ConfigurationStorage.Char,SettingName1,6);
		ReaderConfiguration->Menu( &ConfigurationStorage );
	}	
	Match = false;
	ConfigurationStorage1.Copy( SettingName1 );
	ConfigurationStorage.Copy( SettingName2 );
	ConfigurationStorage.Append( "$!" );
	ConfigurationStorage1.Append( "$!" );			   
	ReaderConfiguration->Menu( &ConfigurationStorage );
	ReaderConfiguration->Menu( &ConfigurationStorage1 );

	if( (ConfigurationStorage.Size >= 7) && (ConfigurationStorage1.Size >= 7) )
	{
	 	if( (ConfigurationStorage.Size) == (ConfigurationStorage1.Size) )
		{
		 	int count = ConfigurationStorage.Size - 6;
			int i = 0;

			for(i = 0; i < count; i++)
			{
				if( ConfigurationStorage.Char[6 + i] == ConfigurationStorage1.Char[6 + i] )
				{
				 	Match = true;					
				}
				else
				{
 					Match = false;
					break;
				}
			}
		}	  
	}
	else
	{
		Match = true;	        //menu command passed not big enough, do not send to menu 
	}

	if( !Match )               //if not a match, menu it
	{
		memcpy(ConfigurationStorage.Char,SettingName1,6);
		ReaderConfiguration->Menu( &ConfigurationStorage );
	}	
	
	return Succeeded;
}



