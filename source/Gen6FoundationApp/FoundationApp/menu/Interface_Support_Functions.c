//***********************************************************
//  Interface Support Functions
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/menu/Interface_Support_Functions.c $
//
// $Date: 2011/11/18 16:00:08EST $
// $Revision: 1.71 $
//======================================================================================
#include "db_hdw.h"
#include "menu.h"
#include "ioman.h"
#include "if_table.h"
#include "timeout.h"
#include "Interface_support_functions.h"
#include "readerconfiguration.h"
#include "productconfiguration.h"
#include "ErrorDefs.h"
#include "beep.h"
#include "mainhhplproc.h"
#include "AutoCableSelect.h"
#include "RemRouter.h"
#include "TermId.h"
#define	TRIGGER_5_5		1
#ifdef TRIGGER_5_5
#include "trigger_5_5.h"                                         //lmc   fixme
#else
//#include "trigger.h"                                      //lmc   not needed
#include "TriggerHandler.h"
#endif	//end #ifdef TRIGGER_5_5
#if PLATFORM_CURRENT == imx25
#include "HardwareMacros.h"
#include "iMX25_GPIO.h"	//GPIO Defines
#include "iMX25_IOMUX.h"	//IO MUX Defines
#endif
#include "HardwareToken.h"

#include "HWTrigger.h"

#include "decodeiface.h"
#include "eci.h"
#include "PowerManagement.h"
#include "HWPageButton.h"


#ifndef RFBASE
CTimeout Imagertimeout;
#endif




#ifdef DIETER_TEST
#include "UsbDataPump.h"
#include "DebugTrace.h"
#endif


#ifndef LARRY_TEST
//#define LARRY_TEST              //lmc_debug
#endif

#ifdef LARRY_TEST
#include "DebugTrace.h"     //lmc-debug
#endif



//extern bool TermIDTestPASWRD();                      //lmc   fixme



#ifdef WINSIM
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


bool bVisMnuOverride = FALSE;

extern CABLE_STATES PresentInterface;


int GetCurrentHostInterface(void)
{
#ifdef RFSCAN
#else

	return theStdInterface.GetTerminalID();
#endif
}



int GetInterfaceClass(void)
{
	interface_id_t Termid=theStdInterface.GetTerminalID();
	switch (Termid)
	{
	case 124:	// pc kbd
	case 125:	// mac kbd
	case 128:	// surepos hh
	case 129:	// surepos tt
	case 130:	//com em
	case 131:	// hid scanner

		return USB_INTERFACE_CLASS;
	case 0:
	case 50:
		return SERIAL_INTERFACE_CLASS;
	case 61:
	case 64:
	case 87:
	case 88:
	case 89:
		return BARCODE_INTERFACE_CLASS;
	default:
		return TERMINAL_INTERFACE_CLASS;
	}
}

void UpdateCodePages(void)	             // go add CodePages to the menu
{
	const int NumCodepages=150;
	int piTempTerms[NumCodepages];
	int iNumberOfTempTerms;
	iNumberOfTempTerms=theEci.CollectAllCodePages(piTempTerms, NumCodepages);
	ReaderConfiguration->ChangeNumericList("020309", piTempTerms, iNumberOfTempTerms);	//update the menu
	ReaderConfiguration->ChangeNumericList("020A06", piTempTerms, iNumberOfTempTerms);	//update the menu
	ReaderConfiguration->ChangeNumericList("020D04", piTempTerms, iNumberOfTempTerms);	//update the menu
	ReaderConfiguration->ChangeNumericList("021F07", piTempTerms, iNumberOfTempTerms);	//update the menu
	ReaderConfiguration->ChangeNumericList("022004", piTempTerms, iNumberOfTempTerms);	//update the menu
	ReaderConfiguration->ChangeNumericList("022105", piTempTerms, iNumberOfTempTerms);	//update the menu
	ReaderConfiguration->ChangeNumericList("023307", piTempTerms, iNumberOfTempTerms);	//update the menu
	ReaderConfiguration->ChangeNumericList("023604", piTempTerms, iNumberOfTempTerms);	//update the menu
	ReaderConfiguration->ChangeNumericList("023704", piTempTerms, iNumberOfTempTerms);	//update the menu
}

void ConfigureSerialPolarity (void)
{
	if(HardwareToken.IAmOEM())
	{
		switch (GET_SETTING(RS232InvEnable)){
		// Normal inverted
		default:
		case 0:
			HAZARD_WRITE_FIELD_TO_REG(IOMUXC_SW_MUX_CTL_PAD_UART2_CTS,IOMUXC_SW_MUX_CTL_MUX_MODE_LSH,IOMUXC_SW_MUX_CTL_MUX_MODE_WID,MUX_MODE_5);
			HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART2_CTS,0x000000E0);  //100k pullup
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,GPIO_HIGH);
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,GPIO_GDIR_OUTPUT);
			break;
		// non inverted
		case 1:
			HAZARD_WRITE_FIELD_TO_REG(IOMUXC_SW_MUX_CTL_PAD_UART2_CTS,IOMUXC_SW_MUX_CTL_MUX_MODE_LSH,IOMUXC_SW_MUX_CTL_MUX_MODE_WID,MUX_MODE_5);
			HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART2_CTS,0x000000E0); //100k pullup
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,GPIO_LOW);
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,GPIO_GDIR_OUTPUT);
			break;
		// Normal with cable select
		case 2:
			HAZARD_WRITE_FIELD_TO_REG(IOMUXC_SW_MUX_CTL_PAD_UART2_CTS,IOMUXC_SW_MUX_CTL_MUX_MODE_LSH,IOMUXC_SW_MUX_CTL_MUX_MODE_WID,MUX_MODE_5);
			HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART2_CTS,0x000000E0); //100k pullup
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,GPIO_HIGH);
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,GPIO_GDIR_OUTPUT);
			break;
		}
	}
}

#if defined( RFSCAN ) && !defined( EMERALD_VERIFIER )         //RF scanner
#elif defined (EMERALD_VERIFIER)

MatrixError_t ReconfigureInterface(void)                      //Emerald verifier
{
	MatrixError_t status;
	xputstring("ReconfigureInterface 2\r\n");
	status =theStdInterface.Open(GET_SETTING(TerminalID));	//Go open the interface - RS / 232 only!!
	if (IsError(status))
	{
		SoundErrorBeep();		//signal an error
		theStdInterface.Open(0);	//open 232
	}
	return status;  // note:  no code checks the return of this function
}

#else

MatrixError_t ReconfigureInterface(void) 		//RF base  or  corded
{

	int InterfaceType=GET_SETTING(TerminalID);
	MatrixError_t status = ERR_PASS;

	theRemInterface.Open(TRM_USB_REM, (unsigned char *)"hi"); // We must Open() this interface before theStdInterface.  We need SOMETHING in Open()'s parameter string or else we fail to open.
	status =theStdInterface.Open((interface_id_t)InterfaceType);	//Go open the interface
	ConfigureSerialPolarity();                                              //lmc   fixme
	if (IsError(status))
	{
		SoundErrorBeep();		//signal an error
		theStdInterface.Open(0);	//open 232
	}

	char miscmnuparams[128] ="";
	// USB HID should enable scanner by default which is different from other USB interfaces
	// The scanner gets enabled here because the interface is created before the menu.
	// An IOCTRL is used instead of a static variable because REM can flash over HID now which
	// causes MainApplicationUpdateInformation to open the interface multiple times
	if( IOCRET_YES == theStdInterface.DeviceIoControl(IOCTRL_EXPLICITLY_ENABLE_SCANNER) )
	{
		strcat(miscmnuparams,"060F0B1!");
	}
	unsigned int length=strlen(miscmnuparams);
	Menu(&length,(unsigned char *)miscmnuparams);


// TODO: does anyone use these tests anymore?  Maybe we can remove this code.
#ifdef LARRY_TEST       //define to configure the debug interface

	int iUartNum = 1;             //set the UART to set debug interface to

	if(iUartNum == 2)
	{
		::OpenDebugPortTwo(InterfaceType);                         //lmc-debug

		if(GET_SETTING(GeneralDebugOn))
		{
			if(IsInterfaceRunning_2())
			{
				//xPutString2("Debug output Uart 2 ok\r");                        //test one output statement or the other
				theDebugInterface.printf("Debug output Uart 2 ok\r");          //lmc-debug
			}
		}
	}
	else
	{
		::OpenDebugPort(InterfaceType);

		if(GET_SETTING(GeneralDebugOn))
		{
			if(IsInterfaceRunning())
			{
				//xputstring("Debug output Uart 1 ok\r");                        //test one output statement or the other
				theDebugInterface.printf("Debug output Uart 1 ok\r");          //lmc-debug
			}
		}
	}
#endif	//end if defined LARRY_TEST

#ifdef DIETER_TEST
	if (IsOK(status))
	{
		::OpenDebugPort(InterfaceType);
		theDebugInterface.printf("Debug output ok\r");
		bool sleep = false;
		bool quit = false;
		if (InterfaceType != 0)
		{
#if defined( RFBASE ) || defined( RFSCAN )
			while(!quit)
#else
				while(!pCHWTrigger->IsTriggerPulled())
#endif
				{
//					RESET_WATCHDOG_TIMER();
					static tchar_t hugebuf[10000];
					tchar_t buf[10];
					buf[0] = 0;
					//				HWTRACE4_PH();
					theDebugInterface.Read(buf, 1);

					//				if(buf[0] != 0)	theDebugInterface.Write(buf, 1);

					if(buf[0] == 'q')
					{
						DEBUG_USB_SHOW();
					}

					if(buf[0] == 'Q')
					{
						DEBUG_USB_RESET();
					}

					if(buf[0] == 'w')
					{
						HWTRACE4_PH();
						HWTRACE3_PH();
						theStdInterface.Write("Test-Sheet");
						HWTRACE3_PH();
						HWTRACE3_PH();
					}

					if(buf[0] == 'd')
					{
						theStdInterface.Write("012345678901234567890123456789012345678901234567890123456789012d");  // 64
					}

					if(buf[0] == 'D')
					{
						theStdInterface.Write("0123456789012345678901234567890123456789012345678901234567890123D");  // 64+1
					}

					if(buf[0] == 'a')
					{
						theStdInterface.Write("01234567890123456789012345678901234567890123456789012345678901a");  // 64-1
					}

					if(buf[0] == 'h')
					{
						theStdInterface.Write("h");
					}

					if(buf[0] == 'r')
					{
						size_t size = 0;
						tchar_t *pDest= hugebuf;
						for(int i=0; i<1000; i+=128-32)
						{
//							RESET_WATCHDOG_TIMER();	//88888888888888888888888
							for (int j=' '; j<0x7f; j++)
							{
								size++;
								*pDest++ = j;
							}
							size++;
							*pDest++ = '\r';
						}
						size++;
						*pDest++ = '\n';
						theStdInterface.Write(hugebuf, size);
					}

					if(buf[0] == 'R')
					{
						size_t size = 1200;
						size = theStdInterface.Read(hugebuf, size);
						// remove a trailing \n
						if(hugebuf[size] == '\n')
							size--;
						theStdInterface.Write(hugebuf, size);
					}

					if(buf[0] == 'x')
					{
						quit = true;
					}

					if(buf[0] == '6')
					{
						theStdInterface.Write("\x06");
						sleep = true;
					}

					if(buf[0] == '7')
					{
						theStdInterface.Write("\x07");
						sleep = false;
					}

					if(sleep)
					{
						ManageSystemPower();
						HWTRACE3_PH();
						HWTRACE3_PH();
						HWTRACE3_PH();
						HWTRACE3_PH();
					}
				}
		}

		//		theStdInterface.Write("Leave 1");
		DEBUG_CAPTURE("db_main");
		theDebugInterface.printf("leaving debug loop\r");
		//		theStdInterface.Write("Leave 2");
	}
#endif	//end if defined DIETER_TEST

	return status;  // note:  no code checks the return of this function
}				// end reconfigure interface
#endif       //end rf base or corded



#define MAX_TERMID_DIGITS 3

void UpdateTermIDs(void)	             // go add termIDs from the IF_Table to a minimized list
{
	int piTempTerms[400];
	int iNumberOfTempTerms;
	if ( HardwareToken.IAmIndustrial() )
	{
		CTermialID::RemoveInterface(TRM_IBM_4683);
	}
	/*Youjie2D 4600 USB serial and IBM 485 are not supported*/
	if (HardwareToken.IAmYoujie4600())
	{
		CTermialID::RemoveInterface(TRM_IBM_4683);
//		CTermialID::RemoveInterface(TRM_USB_COMEMU);	
		CTermialID::RemoveInterface(TRM_USB_HIDPOS);
		CTermialID::RemoveInterface(TRM_IBM_SUREPOS_HH);
		CTermialID::RemoveInterface(TRM_IBM_SUREPOS_TT);
	}
	iNumberOfTempTerms=CollectAllInterfaceTerminalIds(sizeof piTempTerms/sizeof(int), piTempTerms);	//query the interface table
	ReaderConfiguration->ChangeNumericList("060500", piTempTerms, iNumberOfTempTerms, MAX_TERMID_DIGITS);	//update the menu
}

void UpdateTermUSBIDs(void)
{
	int piTempTerms[100];
	int iNumberOfTempTerms;
	iNumberOfTempTerms=CollectInterfaceUSBTerminalIds(piTempTerms);	//query the interface table
	ReaderConfiguration->ChangeNumericList("060802", piTempTerms, iNumberOfTempTerms, MAX_TERMID_DIGITS);	//update the menu

}

void UpdateTermKBWIDs(void)	  // go add termIDs from the IF_Table tominimizedlist
{
	int piTempTerms[100];
	int iNumberOfTempTerms;
	iNumberOfTempTerms=CollectInterfaceKBWTerminalIds(sizeof piTempTerms/sizeof(int), piTempTerms);	//query the interface table
	ReaderConfiguration->ChangeNumericList("060801", piTempTerms, iNumberOfTempTerms, MAX_TERMID_DIGITS);	//update the menu
}

void FillInProductName(void)
{
	HSTRING		ProductNameString( SIZE_OF_PROD_NAME, true );

	 if( GET_CONFIG_SETTING( ProductNameNoSuffix ) )
	 {
	 	ProductNameString.Append(GET_CONFIG_SETTING(ProductName));
		ProductNameString.UniCodeToSimpleAsciiFormat();
	 }
	 else
	 {
		ProductNameString.Copy(GET_CONFIG_SETTING(ProductName));
		ProductNameString.UniCodeToSimpleAsciiFormat();

		if( HardwareToken.IAmOEM() )
		{
			ProductNameString.Insert("Sweetcorn ",0);
			ProductNameString.Append(" Series Area Image Engine");
		}
		else if(HardwareToken.IAmVoyager1400())
		{
			if(HardwareToken.IAmYoujie4600())	//Youjie 4600 is based on 1400
			{
				ProductNameString.Insert("YJ",0);
				ProductNameString.Append(" Area-Imaging Scanner");
			}
			else
			{
				ProductNameString.Insert("SuperMax ",0);
			}
		}
		else
		{
			ProductNameString.Insert("SuperMax ",0);
			ProductNameString.Append(" Area-Imaging Scanner");
		}

	}
	ReaderConfiguration->AddString("0F0100", SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_TAG, NULL, NULL,
	                               ReaderConfiguration->ShowProductName, SIZE_OF_PROD_NAME, (char *) ProductNameString.Char, (char *) ProductNameString.Char);

}

void UpdateOpenInterfaceID(void)
{
	switch(PresentInterface)
	{
		case Interface_232_Group:
			//ReaderConfiguration->UpdateNumericListWorkingValue("TERMID","TRM232");
			UpdateNumericListWorkingValue("060500","060800");
			break;
		case Interface_USB_Group:
			//ReaderConfiguration->UpdateNumericListWorkingValue("TERMID","TRMUSB");
			UpdateNumericListWorkingValue("060500","060802");
			break;
		case Interface_485_Group:
			//ReaderConfiguration->UpdateNumericListWorkingValue("TERMID","TRMRTL");
			UpdateNumericListWorkingValue("060500","060803");
			break;
		case Interface_KBDWDG_Group:
			//ReaderConfiguration->UpdateNumericListWorkingValue("TERMID","TRMKBW");
			UpdateNumericListWorkingValue("060500","060801");
			break;
		case Interface_Fac232_Group:
		case Interface_Unknow:
			break;
	}
}

void UpdateDynamicMenuSettings(void)
{
	UpdateTermIDs();
	if(!(HardwareToken.IAmOEM()) && !(HardwareToken.IAmYoujie4600()))   //NGOEM-109
	{
		UpdateTermUSBIDs();
		UpdateTermKBWIDs();
		UpdateOpenInterfaceID();
	}
	UpdateCodePages();
	ExpandBaudRates();
	//SetupKioskDecodeMode();                                               //lmc   fixme
}



//
//	UpdateImageTransferSettings
//		Update image transfer settings based on interface type.
//
void UpdateImageTransferSettings(void)
{
	// If reader configuration image transfer protocol is HMODEM, and this device is communicating via a fail safe interface (such as USB), switch to
	// non-HMODEM SYN-FE protocol (type 2).
	MatrixError_t RetVal;
	RetVal = theStdInterface.DeviceIoControl(IOCTRL_IS_FAILSAVE_INTERFACE);
	if( (RetVal == IOCRET_YES) &&
	        ( (ReaderConfiguration->TransmitImageProtocol == TRANSMIT_IMAGE_HMODEM_COMPRESSED_PROTOCOL) ||
	          (ReaderConfiguration->TransmitImageProtocol == TRANSMIT_IMAGE_HMODEM_UNCOMPRESSED_PROTOCOL) ) )
	{
		ReaderConfiguration->TransmitImageProtocol = TRANSMIT_IMAGE_NO_PROTOCOL;
	}

	return;
} // UpdateImageTransferSettings












extern bool NeedToRemoveScanners;

#ifdef EMERALD_VERIFIER
bool bSecondarySaved;
#endif


void ReSynchWorkingAndActive(bool ForceUpdate)	//void ReSynchWorkingAndActive()
{
	if( !GET_SETTING( UpdatePerMenuChanges ) )
		return;

#ifdef EMERALD_VERIFIER

	if(bSecondarySaved!=theHal.IsSecondary())
	{
		bSecondarySaved=!bSecondarySaved;	//toggle Secondary Mode
		ReconfigureInterface();
	}
#endif



	// If one or more active values were defaulted, reconcile any settings that need it.
	if( ReaderConfiguration->AtLeastOneActiveValueWasDefaulted() )
	{
		ReaderConfiguration->ReconcileSettingsAfterHardwareInitializationOrDefaultCommand();
	}



	if((ReaderConfiguration->WorkingValuesUpdated()) || (ReaderConfiguration->ActiveValuesUpdated()) || ForceUpdate)
	{
		if( ReaderConfiguration->WorkingValuesUpdated() )
		{
			AlertDriverOfSettingsChange();
		}
		CRemRouterHandler::OnChangedConfiguration();

		crank_up_clocks(GET_CONFIG_SETTING(iMaxProccesorFreq));

		if(bVisMnuOverride == FALSE)
		{
			InitializeGlobalTimers();		// this resets CTimeouts that are declared top level. some have menuable timeouts
			ReconfigureInterface(); 		//Go open the I/F again. without the secondary check

#ifndef RFBASE
			
			theScriptInterpreter.OnChangedConfiguration();	// the interpreter need to update it's internal state
#endif
			theEci.OnChangedConfiguration();						// the ECI need to update it's internal state
			//theHal.InitializeGoodReadLEDPowerOnIndicator();     //If good read indicator is configured to be power indicator, turn it on    //lmc   fixme
		}
		UpdateImageTransferSettings();	// Update image transfer settings based on interface type.

#ifndef RFBASE
#ifdef VERIFIER

		if(!bInPDFDecodingmode) //If PDF in process, don't reset trigger state
		{
			pTrigger->OnChangedConfiguration();
		}
		DecodeIFaceOnChangedConfiguration(); //this must be called after trigger update to onchange 
		FillDecoderMenu(); //go do decoder menu changes
		FillVerifierMenu();
		theOidManager.ConfigureOID_Auto();

#else

 #ifdef TRIGGER_5_5
		pTrigger->OnChangedConfiguration();
#else
		g_pTriggerHandler->OnChangedConfiguration();
#endif	//end #ifdef TRIGGER_5_5
		DecodeIFaceOnChangedConfiguration(); //this must be called after trigger update to onchange
		FillDecoderMenu(); //go do decoder menu changes

#endif    //if verifier
#endif	//if not rf base



		ReaderConfiguration->WorkingValuesUpToDate();
		ReaderConfiguration->ActiveValuesUpToDate();
	}
}



/*********************************************************************
 * Some drivers need to know the default/ROM setting of a variable
 * use this function to get and send the new setting to the driver.
 *
 * Initially this was done for 4683 which needs to know the default
 * value of the beeper volume. It uses a '!' setting to disable the
 * beeper and needs to know the default value to re-enable the beeper
 * to the correct value.
 *********************************************************************/
void AlertDriverOfSettingsChange( void )
{
	// Check to see if we're in an interface that needs to know
	if( GET_SETTING(TerminalID) == TRM_IBM_4683 )
	{
		// Check menu settings that the driver needs
		SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY	*BeepLevelEntry;
		bool FoundBeepLevelEntry = ReaderConfiguration->GetSubTagTableEntry( "05021D", &BeepLevelEntry );

		if( FoundBeepLevelEntry )
		{
			SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY *BeepLevelWorkingValue = ReaderConfiguration->WorkingValueForCurrentWorkgroup( BeepLevelEntry );
			if( BeepLevelWorkingValue->Updated )	// Change the value if the setting has been updated
			{
				theStdInterface.DeviceIoControl(IOCTRL_SET_DEFAULT_BEEP_LEVEL, BeepLevelWorkingValue->i, NULL );      // should work need to check to make sure retail drivers support this see jim l
			}
		}
	}
	return;
}



#if defined( DEVELOPMENT_SETTINGS )
#define MAXIMUM_TERMINAL_ID 1000 //reserve last 99 termids for development purposes
#else
#define MAXIMUM_TERMINAL_ID 900
#endif

#define MAXIMUM_TERMINAL_KBW_ID 121
#define MINIMUM_TERMINAL_KBW_ID 1
#define DUMMYMODEL		0xff
#define NONE				0x00
#define STANDARD			CInterfaceTable::Standard		 //0x01
#define TRUE232			CInterfaceTable::True232		//0x02
#define HHLC				CInterfaceTable::Hhlc			//0x04
#define IT4080			 	CInterfaceTable::IT4080			//0x08
#define STANDARD_4683	CInterfaceTable::Standard_4683 //0x01



BOOL ValidateTermId( int iTermId )
{
	// if password is high security don't allow PnP to change terminal interface
	if(0)//( iTermId != GET_SETTING(TerminalID) && !TermIDTestPASWRD() )                    //lmc   fixme
	{
		return false;
	}

	if( iTermId < MAXIMUM_TERMINAL_ID )
	{
		int iModel;

#if !defined( RFBASE ) && !defined( RFSCAN )
		//UINT bdToken = 0; //CHal::WhatBoardAmI();                    //lmc   fixme
#endif

		iModel = GET_CONFIG_SETTING( iInterfacePopulation );

#ifdef RFSCAN

		iModel = PSEUDO_KBW_4683_USB;
#endif

		switch ( iModel )
		{
		case PSEUDO_KBW_4683_USB :
			if(0)//( CHal::WhatBoardAmI() == IT4200_1_TOKEN )                        //lmc   fixme
			{
				return(theStdInterface.QueryInterface(iTermId, STANDARD));
			}
			else
			{
				if(0)//( CHal::WhatBoardAmI() == IT3800G_1_TOKEN )	// 3800G does not support serial wedge or wandem.       //lmc   fixme
				{
					if( iTermId == 50 || iTermId == 60 || iTermId == 61 || iTermId == 64)
					{
						return(theStdInterface.QueryInterface(iTermId, NONE));
					}

					if(0)//( theHal.Is3800R() )                                                           //lmc   fixme
					{
						return(theStdInterface.QueryInterface(iTermId, STANDARD_4683));
					}
					else
					{
						return(theStdInterface.QueryInterface(iTermId, STANDARD));
					}
				}
				else
				{
					return(theStdInterface.QueryInterface(iTermId, STANDARD_4683));
				}
			}

		case RS232_ONLY :
			return(theStdInterface.QueryInterface(iTermId, TRUE232));

#if defined( RFBASE ) || defined( RFSCAN )

		case HHLC_LP :
			return(theStdInterface.QueryInterface(iTermId, HHLC));
#else
			//case IT4080_RS232_HHLC :	same as below
		case HHLC_LP :
			if(0)// ( bdToken >= IT4080_1_TOKEN  && bdToken < IT4080_LAST_TOKEN)                   //lmc   fixme
			{
				return(theStdInterface.QueryInterface(iTermId, CInterfaceTable::IT4080woUSB));
			}
			else
			{
				return(theStdInterface.QueryInterface(iTermId, HHLC));
			}
#endif

		case RS232_USB_HHLC :
			return(theStdInterface.QueryInterface(iTermId, IT4080));

		case PSEUDO232_USB :
			if(0)//( (CHal::WhatBoardAmI() == IT4800DR_1_TOKEN) || (CHal::WhatBoardAmI() == IT5085_1_TOKEN) )  //4800DR mega document reader         //lmc   fixme
			{
				if( iTermId == 87 || iTermId == 89 || iTermId == 137 || iTermId == 139)    // 4800DR does not support HHLC
				{
					return(theStdInterface.QueryInterface(iTermId, NONE));
				}
				else
				{
					return(theStdInterface.QueryInterface(iTermId, IT4080));
				}
			}
			else
			{
				return(theStdInterface.QueryInterface(iTermId, IT4080));
			}

		default :
			return(theStdInterface.QueryInterface(iTermId, NONE));
		}
	}
	return FALSE;
}



int CollectAllInterfaceTerminalIds(int MaxNumItems, int * pTagBufferData)
{
	int i,j=0;

	for ( i=0; (i<MAXIMUM_TERMINAL_ID) && (j<MaxNumItems); i++ )
	{
		if ( ValidateTermId(i) )
		{
			pTagBufferData[j]=i;
			j++;
		}
	}

	return(j);
}
int CollectInterfaceKBWTerminalIds(int MaxNumItems, int * pTagBufferData)
{
	int i,j=0;
	for(i=MINIMUM_TERMINAL_KBW_ID;(i!=51) && (i<=MAXIMUM_TERMINAL_KBW_ID) && (j<MaxNumItems);i++)
	{
		if(ValidateTermId(i))
		{
			pTagBufferData[j]=i;
			j++;
		 }
	}
	return(j);
}
int CollectInterfaceUSBTerminalIds(int * pTagBufferData)
{
	int i,j=0;
	for(i=MINIMUN_TERMINAL_USB_ID;i<=MAXIMUM_TERMINAL_USB_ID;i++)
	{
		if(ValidateTermId(i))
		{
			pTagBufferData[j]=i;
			j++;
		 }
	}
	return (j);
}



void InitializeGlobalTimers(void)
{
	ResetPowerTimeOuts();
#ifndef RFBASE
	Imagertimeout.SetExpiration((GET_SETTING(iImagerResetTime)*MilliSec*1000));
	if (g_pScriptInterpreter) theScriptInterpreter.InitializeMainScriptTimers();
#endif

}



// Safer function to call to detect hardware trigger pulls
// for base or corded scanner.
bool IsHwTriggerPulled( void )
{
	bool ret = false;
#ifdef RFBASE
#else
	if( pCHWTrigger != NULL )
		ret = pCHWTrigger->IsTriggerPulled();
#endif
	return ret;
}

#define USB_RECOVER_TIME 20000
extern bool IsPagingActive(void);
void DoPageButtonDetecting(void)
{
}




