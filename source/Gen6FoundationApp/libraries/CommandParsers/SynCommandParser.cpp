/************************************************************************/
//
// HHP Matrix embedded firmware
//
//  SYN command handler
//  Date:       3/27/02
//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: SynCommandParser.cpp $
// $Revision: 1.57 $
// $Date: 2011/10/26 09:59:53EDT $
//======================================================================================
/************************************************************************/
/* This module is artwork independent and processor dependent.          */
/************************************************************************/
#include	<ctype.h>		/* character typing functions				*/
#include	<string.h>
#include	<stdio.h>

#include "db_hdw.h"   /* artwork dependant hardware definitions   */
#include "SynCommandParser.h"		// SYN command handler definitions
#include "barcodedataman.h"
#include "ioman.h"
#include "interrupts.h"
#include "hstring.h"
#include "db_NonMenuCommands.h"
#include "ESCCommandparse.h"
#include "MatrixSynFECommandHandler.h"
#include "imaging.h"
#include "Readerconfiguration.h"
#include "productconfiguration.h"
#include "menu.h"
#include "MenuFlash.h"
#include "beep.h"
#include "Crc.h"
#include "Timeout.h"

#if PLATFORM_CURRENT == imx25
#include "HardwareMacros.h"
#include "iMX25_GPIO.h"	//GPIO Defines
#include "iMX25_IOMUX.h"	//IO MUX Defines
#else
#error No Platform Defined!
#endif //endof PLATFORM_CURRENT

#ifdef PLUGIN_DEV
#include "PluginMenuProcess.h"
#include "HONPlugin.h"
#endif

#ifdef LEAN_AUTOMATION
#include "LeanAutomation.h"
#endif


#ifdef RFBASE
#else
#include "mainhhplproc.h"

#define	TRIGGER_5_5		1
#ifdef TRIGGER_5_5
#include "trigger_5_5.h"                                         //lmc   fixme
#else
#include "trigger.h"
#endif	//end #ifdef TRIGGER_5_5

#endif // endof #ifdef RFBASE #else

#ifdef EMERALD_VERIFIER
extern volatile char cTriggerAimerVerifier;
#endif



#ifdef SAVE_ADDRESS_HISTORY
HSTRING *	DebugMenuCommandAndResponse =NULL;
#endif
extern int UntriggerWasDetected;
bool DoDataEditOrScript(CBarcodeData * gpCurrentDecoded);
#ifndef RFSCAN
extern void LoadWorkingConfigurationOne();
extern void LoadWorkingConfigurationZero();
#endif


//static unsigned int	debug_count = 0;
bool bscanenable = TRUE;

void ProcessSynCommand(CIoMan * pIface, SYN_COMMAND_PARSE * pSynParser)
{
	unsigned char byte;

	if( pIface->IsPortAvailable() )
	{
		SetCurrentInterface(pIface);
		while ( pIface->Read(&byte, 1) )
		{
			bool bProcessingByte = pSynParser->ServiceSYNCommand(byte); // Note:  Any ReM data should've been taken care here
			// We don't handle HOSTACK mode for Verifiers.
#ifndef VERIFIER

			if ( (! bProcessingByte) && (GET_SETTING(bEnableHostAck)) )
			{
				bProcessingByte = ESCCommandParser->ServiceESCCommand(byte);
			}
#endif
		}
	}
}



// Processes SYN Commands
void ProcessInterfaceData(void)
{

	int	NextInterfaceToCheck;


	ProcessSynCommand(g_pRemInterface, g_pRemSynCommandParser);

	{
		for ( NextInterfaceToCheck =0; NextInterfaceToCheck < ( MAX_SPP_PORTS + 1 ); ++NextInterfaceToCheck )
		{
			ProcessSynCommand(&theSPP_Ports(NextInterfaceToCheck), SYNCommandParser[NextInterfaceToCheck]);
		}
	}
	// For Verifier builds, if the TERMID is RF, set the current interface pointer to the RF device.
#ifndef EMERALD_VERIFIER
	SetCurrentInterface( g_pStdInterface );
#endif
#ifdef FIXME
	/*********************************************************************
	   * This code block is used for the dual 4683 port configuration.  A
	   * second port is created to allow menu data to be transmitted in the
	   * background while the primary port acts as a normal 5B/9B/17 device.
	   *********************************************************************/
	unsigned char tempChar;

	if( theStdInterface.DeviceIoControl(IOCTRL_SET_AUX_PORT, NULL, NULL) )
	{
		while (theStdInterface.IsAnyRxData())       // check for received data - in 4683 its already been received
		{
			theStdInterface.Read(&tempChar,1);
			AuxSYNCommandParser->ServiceSYNCommand(tempChar);
		}
		theStdInterface.DeviceIoControl(IOCTRL_SET_PRIMARY_PORT, NULL, NULL);
	}
#endif
}


void ProcessStxCommand(CIoMan * pIface, STX_COMMAND_PARSE * pStxParser)
{
	unsigned char byte;

	if( pIface->IsPortAvailable() )
	{
		SetCurrentInterface(pIface);
		while ( pIface->Read(&byte, 1) )
		{
			bool bProcessingByte = pStxParser->ServiceSTXCommand(byte); // Note:  Any ReM data should've been taken care here
			// We don't handle HOSTACK mode for Verifiers.
#ifndef VERIFIER

			if ( (! bProcessingByte) && (GET_SETTING(bEnableHostAck)) )
			{
			//	bProcessingByte = ESCCommandParser->ServiceESCCommand(byte);
			}
#endif
		}
	}
}

// Processes STX Commands
void ProcessEngineData(void)
{
	int NextInterfaceToCheck;
	
	
		ProcessStxCommand(g_pRemInterface, g_pRemStxCommandParser);
		{
			for ( NextInterfaceToCheck =0; NextInterfaceToCheck < ( MAX_SPP_PORTS + 1 ); ++NextInterfaceToCheck )
			{
				ProcessStxCommand(&theSPP_Ports(NextInterfaceToCheck), STXCommandParser[NextInterfaceToCheck]);
			}
		}
		SetCurrentInterface( g_pStdInterface );

}


static void ProcessNonPluginMenuCommands(HSTRING *MenuCommand)
{
	HSTRING MenuCommandCopy("", true);
#ifndef RFSCAN
	MenuCommandCopy.Copy(MenuCommand);
	int PositionCBLTBLWasFound;
	HSTRING ConfigCabSel(20,false);
	ConfigCabSel.Copy("060300");
#endif

	ReaderConfiguration->Menu(MenuCommand);

#ifndef RFSCAN
	if(ReaderConfiguration->TableSettingCommandisProcessing)
		ReaderConfiguration->TableSettingCommandisProcessing=false;
	if(ReaderConfiguration->TwoTableUpdateSimultaneously)
	{
		ReaderConfiguration->TwoTableUpdateSimultaneously=false;
		MenuCommandCopy.Find(&ConfigCabSel,false,1,&PositionCBLTBLWasFound);
		MenuCommandCopy.Remove(0,PositionCBLTBLWasFound+11);
		if(ReaderConfiguration->CurrentWorkTable()){
			SaveSettingChangeToTableOne();
			LoadWorkingConfigurationZero();
			ReaderConfiguration->Menu(&MenuCommandCopy);
			SaveSettingChangeToTableZero();
			LoadWorkingConfigurationOne();
		}
		else
		{
			SaveSettingChangeToTableZero();
			LoadWorkingConfigurationOne();
			ReaderConfiguration->Menu(&MenuCommandCopy);
			SaveSettingChangeToTableOne();
			LoadWorkingConfigurationZero();
		}
		if(ReaderConfiguration->TableSettingCommandisProcessing)
			ReaderConfiguration->TableSettingCommandisProcessing=false;
	}
	else if(ReaderConfiguration->CurrentWorkTable()!=ReaderConfiguration->GetActiveWorktableNumber())
	{
		if(ReaderConfiguration->CurrentWorkTable())
		{
			SaveSettingChangeToTableOne();
			LoadWorkingConfigurationZero();
		}
		else
		{
			SaveSettingChangeToTableZero();
			LoadWorkingConfigurationOne();
		}
		ReaderConfiguration->SetWorkTable(ReaderConfiguration->GetActiveWorktableNumber());
	}
#endif
	KCODE_MODE(FALSE);
	FlashAndReSynchCustomerDefaults();
	FlashWorkingChanges();

}


/* ========================================================================
	ServiceHighLevelInput()

	This function implements a state machine to parse incoming high level
	data that may contain commands for the scanner. Usually, this data
	will only come from the serial port. However, future designs may also
	accept input from other sources (i.e. keyboard)
	======================================================================== */

#define INPUT_RESET__WAITING_FOR_SYN 0
#define INPUT_SYN_RECEIVED__WAITING_FOR_COMMAND_BYTE 1
#define INPUT_COMMAND_BYTE_RECEIVED__WAITING_FOR_COMMAND_DATA 2
#define INPUT_SYN_MENU_COMMAND_RECEIVED__WAITING_FOR_MENU_DATA 3
#define	INPUT_SYN_PRODUCT_MENU_COMMAND_RECEIVED__WAITING_FOR_PRODUCT_MENU_DATA 4

// Constructor
SYN_COMMAND_PARSE::SYN_COMMAND_PARSE(void)
{
	pucBuffer = new char[SIZEOF_HIGHLEVEL_PARSE_BUFFER] ;
	ui = 0;
	uiState = INPUT_RESET__WAITING_FOR_SYN ;
	Busy = false;
}


SYN_COMMAND_PARSE::SYN_COMMAND_PARSE( int BufferSize )
{
	pucBuffer = new char[BufferSize] ;
	ui = 0;
	uiState = INPUT_RESET__WAITING_FOR_SYN ;
	Busy = false;
	return;
}


// Destructor
SYN_COMMAND_PARSE::~SYN_COMMAND_PARSE( void )
{
	delete [] pucBuffer;
	return;
}


bool SYN_COMMAND_PARSE::ServiceSYNCommand( UCHAR ucData )
{
	bool ReceivedByteWasPartOfASynCommand = false;
	Busy = true;

	/* ========================================================================
		State INPUT_RESET__WAITING_FOR_SYN :

		Nothing valid has been received - We are waiting for [SYN]
		======================================================================== */

	if( INPUT_RESET__WAITING_FOR_SYN==uiState )
	{
		if ( SYN==ucData )
		{
			uiState=INPUT_SYN_RECEIVED__WAITING_FOR_COMMAND_BYTE ;
			ReceivedByteWasPartOfASynCommand = true;
		}

		else if( (0x07 == ucData) && (GET_SETTING(bBelBeepEnable)) )              //enable BEL Beep Mode, Beep on BEL with no SYN
		{
			SoundStandardBeep(1);
			uiState = INPUT_RESET__WAITING_FOR_SYN;
			ReceivedByteWasPartOfASynCommand = true;    //not a syn command, but do not want to process this again in Imager 8000
		}
		//Check serial character commands if Stratos aux port or if 232D_E is enabled.
		else if( CTSFST == GET_SETTING(RS232CtsEnable) || true == GET_SETTING(RS232SerialDisableEnable) )
		{
			// 'D' received to disable scanner.
			if(0x44 == ucData)
			{
				#ifndef RFBASE
      				theScriptInterpreter.DisableOPOS();
					pTrigger->UnTriggerSerial();
				#endif
			}
			// 'E' received to enable scanner.
			else if(0x45 == ucData)
			{
				#ifndef RFBASE
					pTrigger->UnTriggerSerial();
					theScriptInterpreter.ClearDC2Status();
					theScriptInterpreter.EnableOPOS();
					pTrigger->FinishScanning();// do lights and start/stop scanning per trigger mode
				#endif
			}
			// 'DC4' received untrigger and disable scanner.
			else if(0x14 == ucData)
			{
				#ifndef RFBASE
      				theScriptInterpreter.DisableOPOS();
					pTrigger->UnTriggerSerial();

				#endif
			}
			// 'DC2' received enable and trigger scanner.
			else if(0x12 == ucData)
			{
				#ifndef RFBASE
					theScriptInterpreter.EnableOPOS();
					pTrigger->FinishScanning();// do lights and start/stop scanning per trigger mode
					if( pTrigger->GetTriggerMode() == TRGMOD_SNAP_AND_SHIP)
					{
						ImageSnapAndShip();
					}
					else
					{
						theScriptInterpreter.SetDC2Status();
						pTrigger->TriggerSerial();
					}

				#endif
			}
		}
	}

	/* ========================================================================
		State INPUT_SYN_RECEIVED__WAITING_FOR_COMMAND_BYTE :

		A SYN character has been received - Waiting for a valid command byte
		======================================================================== */

	else if( INPUT_SYN_RECEIVED__WAITING_FOR_COMMAND_BYTE==uiState )
	{
		ui=0 ;

		switch( ((ucData >= 'a') && (ucData <= 'z')) ? (ucData - 32) : ucData )//toupper(((int) ucData)) )
		{

		case 'A' :                	/* Initiate flashing sequence */
		case 'C' :                	/* Auto-baud detection request */
		case 'D' :					/* Special Debug Mode */
		case 'E' :                 /* Flash at specified baud rate */
		case 'F' :                 /* Flash at specified baud rate */
		case 'G' :                 /* Check for a supported baud rate */
		case 'H' :				   /* Checksum command */
		case 'I' :                 /* Reset scanner via watchdog timeout */
		case 'K' :           	  	/* Display Backlight OFF */
		case 'L' :           	  	/* Toggle Backlight on display */
		case 'M' :           	  	/* Menuing symbol to follow */
		case 'N' :                 /* Process non-menu command */
		case 'O' :                 /* Set second derivative digitizer to fixed Aggressive mode */
		case 'P' :                 /* Set second derivative digitizer to auto-switch between Aggressive/ Non-aggressive mode */
		case 'Q' :                 /* Turn the DMA edge capture off */
		case 'R' :                 /* Turn the DMA edge capture on */
		case 'S' :                 /* Turn the DMA edge capture on */
		case 'T' :                 /* Trigger the unit */
		case 'U' :                 /* Untrigger the unit */
		case 'V' :				   /* Verify operational status of unit */
		case 'W' :				   /* Protocol wrapped command */
		case 'X' :				   /* Extended Flash functions */
		case 'Y' :					/* Product menu commands */
		case 'Z' :				   /* Extended Flash functions */
		case '1' :					/* OPOS enable scanner */
		case '0' :					/* OPOS disable scanner */
		case BEL :                 /* Beep */
		case DC1 :                 /* LEAN_AUTOMATION Test Engineering commands */

			pucBuffer[ui++]=ucData ;
			uiState=INPUT_COMMAND_BYTE_RECEIVED__WAITING_FOR_COMMAND_DATA ;
		case SYN :
			break ;

		case 0xFE:					// Dispatch the <SYN>FE handler for protocol wrapped commands
			SYNFECommandHandler->ReceiveAndParseServer(pucBuffer, "\x02\xFE");
			uiState=INPUT_RESET__WAITING_FOR_SYN ;

			//			if( GET_SETTING( DebugCommandProccessingTraceMode ) == TRACE_MESSAGE_MODE_ON )
			//			{
			//				theStdInterface.Write( "Finished processing <SYN>FE\r\n" );
			//			}
			break;

		default :
			uiState=INPUT_RESET__WAITING_FOR_SYN ;
		}

		ReceivedByteWasPartOfASynCommand = true;
	}

	/* ========================================================================
		State INPUT_COMMAND_BYTE_RECEIVED__WAITING_FOR_COMMAND_DATA :

		A SYN character and valid command byte have been received - Now waiting
		valid command data folled by a [CR]
		======================================================================== */

	else if( INPUT_COMMAND_BYTE_RECEIVED__WAITING_FOR_COMMAND_DATA==uiState )
	{
		if( CR !=ucData )
		{
			if( ui==SIZEOF_HIGHLEVEL_PARSE_BUFFER )
			{
				SoundErrorBeep();
				uiState=INPUT_RESET__WAITING_FOR_SYN ;
			}
			else if (SYN==ucData)
			{
				uiState=INPUT_SYN_RECEIVED__WAITING_FOR_COMMAND_BYTE;
			}
			else
			{
				pucBuffer[ui++]=ucData;
			}
		}
		else
		{
			uiState=INPUT_RESET__WAITING_FOR_SYN ;
			switch( ((pucBuffer[0] >= 'a') && (pucBuffer[0] <= 'z')) ? (pucBuffer[0] - 32) : pucBuffer[0] )//toupper(pucBuffer[0]) )
			{

#ifdef LEAN_AUTOMATION
			case DC1 :
				{
					pucBuffer[ui] = 0;
					if (ProcessLeanAutomationTestCommand(pucBuffer))
						theCurrentInterface.Write("\006");
					else
						theCurrentInterface.Write("\025");
					break;
				}
#endif

			case 'C' :
				{
					// Respond with ACK NAK DEh DFh
					theCurrentInterface.Write("\x06\x15\xde\xdf");

					break;
				}

			case 'D' :
				{
					//**					bSpecialDebugMode = TRUE;
					break;
				}

			case 'E' :
			case 'F' :
			case 'A' :              // this case needs to follow e and f because flashing is done here

				{
					// Return a NAK, because this product doesn't process these commands.
					theCurrentInterface.Write("\025");
					break;
				}

			case 'G' :
				{
					/* Insert a NUL terminating character so we can perform string compares */
					pucBuffer[ui]='\0' ;

					if(strcmp(pucBuffer+1, "300") &&
							strcmp(pucBuffer+1, "600") &&
					        strcmp(pucBuffer+1, "1200") &&
					        strcmp(pucBuffer+1, "2400") &&
					        strcmp(pucBuffer+1, "4800") &&
					        strcmp(pucBuffer+1, "9600") &&
					        strcmp(pucBuffer+1, "19200") &&
					        strcmp(pucBuffer+1, "38400") &&
					        strcmp(pucBuffer+1, "57600") &&
					        strcmp(pucBuffer+1, "115200"))
						theCurrentInterface.Write("\025");
					else
						theCurrentInterface.Write("\006");
					break;
				}

			case 'H' :                 /* Stored Checksum only */	//DF
				{
					//**					sprintf(pucBuffer,"%04X%04X%04X%04X%c",0, 0, ((BootHeader_t*)AppVectors)->Checksum, ((BootHeader_t*)AppVectors)->Wchecksum, 0x06);
					//**					xputstring(pucBuffer, 17);
					break;
				}

			case 'I' :
				{
					reset_to_boot_code_hard();
				}

			case 'M' :
				{
					ui=0 ;
					uiState=INPUT_SYN_MENU_COMMAND_RECEIVED__WAITING_FOR_MENU_DATA ;

					break ;
				}

			case 'N' :
				{
					pucBuffer[ui] = 0;
					ProcessNonMenuCommands(pucBuffer, ui);
					break;
				}

			case BEL :
				{
					if(ui > 1)	               // extended bel command ?
					{
						switch (pucBuffer[1])
						{
						case 'e':  // error beep?
							SoundErrorBeep();
							break;
						case 'p':  // powerup beep?
							SoundPowerupBeep();
							break;
						case 'u':
							break;

						default:
							SoundStandardBeep(1);
							break;
						}
					}
					else	                     // old simple bel command
					{
						SoundStandardBeep(1);
					}
					break ;
				}
			case 'T' :
				{
#ifdef RFBASE
#else

					if( pTrigger->GetTriggerMode() == TRGMOD_SNAP_AND_SHIP)
					{
						ImageSnapAndShip();
						//FIXME						if(!ImageSnapAndShip())
						//FIXME							NoDecodeBeep();		//Added in Gen5.5
					}
					else
					{
						pTrigger->TriggerSerial();

					}
#endif
					break ;
				}
			case 'U' :
				{
#ifdef RFBASE
#else

					pTrigger->UnTriggerSerial();
#endif

					break ;
				}


			case 'Y' :
				{
					ui=0 ;
					uiState=INPUT_SYN_PRODUCT_MENU_COMMAND_RECEIVED__WAITING_FOR_PRODUCT_MENU_DATA ;
					break ;
				}

			case 'Z' :	/* stop a script */
				{
					//**					TerminateHHPLProgram();
					//**					Script =NULL;
					break;
				}
			case '1' :  //enable OPOS
				{
#ifndef RFBASE
					theScriptInterpreter.EnableOPOS();
					pTrigger->FinishScanning();// do lights and start/stop scanning per trigger mode
					pucBuffer[ui++] = ACK;
					theCurrentInterface.Write( pucBuffer, 2 ) ;
#endif

					break;
				}
			case '0' :  //enable OPOS
				{
#ifndef RFBASE
					theScriptInterpreter.DisableOPOS();
					pucBuffer[ui++] = ACK;
					theCurrentInterface.Write( pucBuffer, 2 ) ;
#endif

					break;
				}

			}     // switch end

		}   		// end if-else

		ReceivedByteWasPartOfASynCommand = true;
	}
	/* ========================================================================
		State INPUT_SYN_MENU_COMMAND_RECEIVED__WAITING_FOR_MENU_DATA :

		A [SYN]M[CR] command sequence has been received - Wait for a complete
		menu string terminated with a '.' or '!' to be recevied
		======================================================================== */

	else if( INPUT_SYN_MENU_COMMAND_RECEIVED__WAITING_FOR_MENU_DATA==uiState )
	{
		if( ui==SIZEOF_HIGHLEVEL_PARSE_BUFFER )
		{
			SoundErrorBeep();
			uiState=INPUT_RESET__WAITING_FOR_SYN ;
		}
		else if ( SYN==ucData )
		{
			uiState=INPUT_SYN_RECEIVED__WAITING_FOR_COMMAND_BYTE;
		}
		else
		{
			pucBuffer[ui++]=ucData ;
			if(('!'==ucData) || ('.'==ucData) || ('&' == ucData) || ('@' == ucData))
			{
				pucBuffer[ui] = 0;

				bool bCommandWasForwarded;
				bool bPrefixFound = ForwardPrefixedCommand(true, (const unsigned char *)pucBuffer, ui, &bCommandWasForwarded);
				// For RFBASE or RFSCAN, don't process this command if it turns out that there was
				//	a device name on the front of the command and we sent the command to that device.
				uiState=INPUT_RESET__WAITING_FOR_SYN ;

				if ( bPrefixFound && ! bCommandWasForwarded )
				{	// User tried to send a Menu Command to a scanner from a base & there wasn't anything connected to the base.  Create a NAK reply
					HSTRING MenuCommand(pucBuffer, true);
					MenuCommand.ReplaceAll("?", "");
					MenuCommand.Insert("\x15", MenuCommand.Size - 1);
					SendResponse(&MenuCommand);
				}
				else if( ! bPrefixFound )
				{
#ifdef SAVE_ADDRESS_HISTORY

					if (DebugMenuCommandAndResponse ==NULL) DebugMenuCommandAndResponse =new HSTRING("",true);
					DebugMenuCommandAndResponse->Copy(pucBuffer);
#endif

					HSTRING MnuCommand("",true);
					#ifndef RFSCAN
					HSTRING MnuCommandCopy("",true);
					#endif
					/*	Call the Menu	*/
					UINT length;
					length = strlen(pucBuffer); /*get length of menu command*/
//					theCurrentInterface.printf("length is %d\r\n", length);

					/*Return messages are passed in pucBuffer so it needs to be big enough for the larges possible menu response */
					/*Menu will update the length parameter with the length of the reponse. */
					// If the last character in the response string is a ";" (due to a hidden entry being skipped), remove it.
					if((length > 1) &&	((pucBuffer[length - 2] == ';')||(pucBuffer[length - 2] == ',')))
					{
						pucBuffer[length-2] = pucBuffer[length-1];
						length -= 1;
						pucBuffer[length] = 0;
					}



#ifdef PLUGIN_DEV
						// Abstract plugin/system menu commands.
						HSTRING PluginMenuCommand("",true);
						HSTRING SystemMenuCommand("",true);
						PluginMenuCommand.Copy(pucBuffer);
						SystemMenuCommand.Copy(pucBuffer);

						int PluginCmd = 0, SystemCmd = 0;

						PluginCmd = AbstractPluginMenuCommands(&PluginMenuCommand);
						SystemCmd = AbstractSystemMenuCommands(&SystemMenuCommand);


						if(PluginCmd)
						{

							//process 990 menu barcode for plugin
							PluginSerialMenuCommandProcess_Ext(&PluginMenuCommand);
							MnuCommand.Copy(&PluginMenuCommand);

							if(SystemCmd){
								// Process system commands
								ProcessNonPluginMenuCommands(&SystemMenuCommand);
								// Combine the results together
								MnuCommand.Remove(MnuCommand.Size - 1, 1);
								MnuCommand.AppendChar(';');
								MnuCommand.Append(&SystemMenuCommand);
							}

							SendResponse(&MnuCommand);
							//ReSynchWorkingAndActive(); // fixme ???
							uiState=INPUT_RESET__WAITING_FOR_SYN ;
#ifndef RFBASE

							theScriptInterpreter.ResetScanningState();
#endif

						}else{
#endif //PLUGIN_DEV
							MnuCommand.Copy(pucBuffer);
#ifndef RFSCAN
							MnuCommandCopy.Copy(pucBuffer);
							int PositionCBLTBLWasFound;
							HSTRING ConfigCabSel(20,false);
							ConfigCabSel.Copy("060300");
#endif
							//	MnuCommandCopy.FindString(&ConfigCabSel,false,1,&PositionCBLTBLWasFound);
							ReaderConfiguration->Menu(&MnuCommand);
#ifndef RFSCAN
							if(ReaderConfiguration->TableSettingCommandisProcessing)
								ReaderConfiguration->TableSettingCommandisProcessing=false;
							if(ReaderConfiguration->TwoTableUpdateSimultaneously)
							{
								ReaderConfiguration->TwoTableUpdateSimultaneously=false;
								MnuCommandCopy.Find(&ConfigCabSel,false,1,&PositionCBLTBLWasFound);
								MnuCommandCopy.Remove(0,PositionCBLTBLWasFound+11);
								if(ReaderConfiguration->CurrentWorkTable()){
									SaveSettingChangeToTableOne();
									LoadWorkingConfigurationZero();
									ReaderConfiguration->Menu(&MnuCommandCopy);
									SaveSettingChangeToTableZero();
									LoadWorkingConfigurationOne();
								}
								else
								{
									SaveSettingChangeToTableZero();
									LoadWorkingConfigurationOne();
									ReaderConfiguration->Menu(&MnuCommandCopy);
									SaveSettingChangeToTableOne();
									LoadWorkingConfigurationZero();
								}
								if(ReaderConfiguration->TableSettingCommandisProcessing)
								ReaderConfiguration->TableSettingCommandisProcessing=false;

							}
							else if(ReaderConfiguration->CurrentWorkTable()!=ReaderConfiguration->GetActiveWorktableNumber())
							{
								if(ReaderConfiguration->CurrentWorkTable())
								{
									SaveSettingChangeToTableOne();
									LoadWorkingConfigurationZero();
								}
								else
								{
									SaveSettingChangeToTableZero();
									LoadWorkingConfigurationOne();
								}
								ReaderConfiguration->SetWorkTable(ReaderConfiguration->GetActiveWorktableNumber());

							}
#endif
							KCODE_MODE(FALSE);
							FlashAndReSynchCustomerDefaults();
							FlashWorkingChanges();
#ifndef RFBASE
							PhotoJustTaken = FALSE;
#endif
							SendResponse(&MnuCommand);
							//ReSynchWorkingAndActive(); // fixme ???
							uiState=INPUT_RESET__WAITING_FOR_SYN ;
#ifndef RFBASE

							theScriptInterpreter.ResetScanningState();
#endif

#ifdef PLUGIN_DEV
						}
#endif
				}
			}
		}

			ReceivedByteWasPartOfASynCommand = true;
	}
		/* ========================================================================
			State INPUT_SYN_PRODUCT_MENU_COMMAND_RECEIVED__WAITING_FOR_PRODUCT_MENU_DATA :

			A [SYN]Y[CR] command sequence has been received - Wait for a complete
			menu string terminated with a '.' or '!' to be recevied
			======================================================================== */

		else if( INPUT_SYN_PRODUCT_MENU_COMMAND_RECEIVED__WAITING_FOR_PRODUCT_MENU_DATA==uiState )
		{
			if( ui==SIZEOF_HIGHLEVEL_PARSE_BUFFER )
			{
				SoundErrorBeep();
				uiState=INPUT_RESET__WAITING_FOR_SYN ;
			}

			else if ( SYN==ucData )
			{
				uiState=INPUT_SYN_RECEIVED__WAITING_FOR_COMMAND_BYTE;
			}
			else
			{
				pucBuffer[ui++]=ucData;
				if(('!'==ucData) || ('.'==ucData))
				{
					pucBuffer[ui] = 0;
					HSTRING	MenuCommandAndResponse(pucBuffer, true);
					uiState=INPUT_RESET__WAITING_FOR_SYN ;
					bool bCommandWasForwarded;
					bool	bPrefixFound = ForwardPrefixedCommand(false, (const unsigned char *)pucBuffer, ui, &bCommandWasForwarded);

					

					if ( bPrefixFound && ! bCommandWasForwarded )
					{	// User tried to send a Product Config Command to a scanner from a base & there wasn't anything connected to the base.  Create a NAK reply
						MenuCommandAndResponse.ReplaceAll("?", "");
						MenuCommandAndResponse.Insert("\x15", MenuCommandAndResponse.Size - 1);
						SendResponse(&MenuCommandAndResponse);
					}
					else if( ! bPrefixFound )
					{	/*	Call the Menu	*/
						ProductConfiguration->Menu(&MenuCommandAndResponse);
						FlashProductConfigurationChanges();
						theCurrentInterface.Write((char *) MenuCommandAndResponse.Char, MenuCommandAndResponse.Size);
					}
				}
			}
			ReceivedByteWasPartOfASynCommand = true;
		}
		Busy = false;
		return(ReceivedByteWasPartOfASynCommand) ;
	}



// Only return a response to the sender if the message was sent via the standard interface.  If sent over the radio, don't return a response.
void SYN_COMMAND_PARSE::SendResponse(const HSTRING * pMenuCommand)
{
	if ( (GetCurrentInterface() == g_pStdInterface) || (GetCurrentInterface() == g_pRemInterface) )
	{
		CBarcodeData * menumsg;
		menumsg = CBarcodeData::StoreToNewBuffer(pMenuCommand->Char,pMenuCommand->Size);
		menumsg->StoreHHP('6', 0);
		menumsg->StoreAim('Z', '6');
		theCurrentInterface.Write(menumsg);
	}
}



	SYN_COMMAND_PARSE ** SYNCommandParser;
	SYN_COMMAND_PARSE * OWSYNCommandParser;
	SYN_COMMAND_PARSE * g_pRemSynCommandParser;



/*edited by Charlse
Change SYN Command to STX Command
*/
/* ========================================================================
	ServiceHighLevelInput()

	This function implements a state machine to parse incoming high level
	data that may contain commands for the scanner. Usually, this data
	will only come from the serial port. However, future designs may also
	accept input from other sources (i.e. keyboard)
======================================================================== */
	
#define INPUT_RESET__WAITING_FOR_STX 0
#define INPUT_STX_RECEIVED__WAITING_FOR_COMMAND_BYTE 1
#define INPUT_COMMAND_BYTE_RECEIVED_STX_WAITING_FOR_COMMAND_DATA 2
#define INPUT_STX_MENU_COMMAND_RECEIVED__WAITING_FOR_MENU_DATA 3
#define	INPUT_STX_PRODUCT_MENU_COMMAND_RECEIVED__WAITING_FOR_PRODUCT_MENU_DATA 4
	
	// Constructor
	STX_COMMAND_PARSE::STX_COMMAND_PARSE(void)
	{
		pucBuffer = new char[SIZEOF_HIGHLEVEL_PARSE_BUFFER] ;
		ui = 0;
		uiState = INPUT_RESET__WAITING_FOR_STX ;
		Busy = false;
	}
	
	
	STX_COMMAND_PARSE::STX_COMMAND_PARSE( int BufferSize )
	{
		pucBuffer = new char[BufferSize] ;
		ui = 0;
		uiState = INPUT_RESET__WAITING_FOR_STX ;
		Busy = false;
		return;
	}
	
	
	// Destructor
	STX_COMMAND_PARSE::~STX_COMMAND_PARSE( void )
	{
		delete [] pucBuffer;
		return;
	}


	bool STX_COMMAND_PARSE::ServiceSTXCommand( UCHAR ucData )
	{
		bool ReceivedByteWasPartOfASynCommand = false;
		Busy = true;
		/* ========================================================================
			State INPUT_RESET__WAITING_FOR_SYN :
	
			Nothing valid has been received - We are waiting for [SYN]
			======================================================================== */
	
		if( INPUT_RESET__WAITING_FOR_STX==uiState )
		{
			if ( STX==ucData )
			{
				uiState=INPUT_STX_RECEIVED__WAITING_FOR_COMMAND_BYTE ;
				ReceivedByteWasPartOfASynCommand = true;
			}

			else if( (0x07 == ucData) && (GET_SETTING(bBelBeepEnable)) )              //enable BEL Beep Mode, Beep on BEL with no SYN
			{
				SoundStandardBeep(1);
				uiState = INPUT_RESET__WAITING_FOR_STX;
				ReceivedByteWasPartOfASynCommand = true;    //not a syn command, but do not want to process this again in Imager 8000
			}
			//Check serial character commands if Stratos aux port or if 232D_E is enabled.
			else if( CTSFST == GET_SETTING(RS232CtsEnable) || true == GET_SETTING(RS232SerialDisableEnable) )
			{
				// 'D' received to disable scanner.
				if(0x44 == ucData)
				{
					#ifndef RFBASE
		  				theScriptInterpreter.DisableOPOS();
						pTrigger->UnTriggerSerial();
					#endif
				}
				// 'E' received to enable scanner.
				else if(0x45 == ucData)
				{
					#ifndef RFBASE
						pTrigger->UnTriggerSerial();
						theScriptInterpreter.ClearDC2Status();
						theScriptInterpreter.EnableOPOS();
						pTrigger->FinishScanning();// do lights and start/stop scanning per trigger mode
					#endif
				}
				// 'DC4' received untrigger and disable scanner.
				else if(0x14 == ucData)
				{
					#ifndef RFBASE
		  				theScriptInterpreter.DisableOPOS();
						pTrigger->UnTriggerSerial();

					#endif
				}
				// 'DC2' received enable and trigger scanner.
				else if(0x12 == ucData)
				{
					#ifndef RFBASE
						theScriptInterpreter.EnableOPOS();
						pTrigger->FinishScanning();// do lights and start/stop scanning per trigger mode
						if( pTrigger->GetTriggerMode() == TRGMOD_SNAP_AND_SHIP)
						{
							ImageSnapAndShip();
						}
						else
						{
							theScriptInterpreter.SetDC2Status();
							pTrigger->TriggerSerial();
						}

					#endif
				}
			}
		}
	
		/* ========================================================================
			State INPUT_STX_RECEIVED__WAITING_FOR_COMMAND_BYTE :
	
			A SYN character has been received - Waiting for a valid command byte
			======================================================================== */
	
		else if( INPUT_STX_RECEIVED__WAITING_FOR_COMMAND_BYTE==uiState )
		{
			ui=0 ;

			switch( ((ucData >= 'a') && (ucData <= 'z')) ? (ucData - 32) : ucData )//toupper(((int) ucData)) )
			{

			case 'A' :                	/* Initiate flashing sequence */
			case 'C' :                	/* Auto-baud detection request */
			case 'D' :					/* Special Debug Mode */
			case 'E' :                 /* Flash at specified baud rate */
			case 'F' :                 /* Flash at specified baud rate */
			case 'G' :                 /* Check for a supported baud rate */
			case 'H' :				   /* Checksum command */
			case 'I' :                 /* Reset scanner via watchdog timeout */
			case 'K' :           	  	/* Display Backlight OFF */
			case 'L' :           	  	/* Toggle Backlight on display */
			case 'M' :           	  	/* Menuing symbol to follow */
			case 'N' :                 /* Process non-menu command */
			case 'O' :                 /* Set second derivative digitizer to fixed Aggressive mode */
			case 'P' :                 /* Set second derivative digitizer to auto-switch between Aggressive/ Non-aggressive mode */
			case 'Q' :                 /* Turn the DMA edge capture off */
			case 'R' :                 /* Turn the DMA edge capture on */
			case 'S' :                 /* Turn the DMA edge capture on */
			case 'T' :                 /* Trigger the unit */
			case 'U' :                 /* Untrigger the unit */
			case 'V' :				   /* Verify operational status of unit */
			case 'W' :				   /* Protocol wrapped command */
			case 'X' :				   /* Extended Flash functions */
			case 'Y' :					/* Product menu commands */
			case 'Z' :				   /* Extended Flash functions */
			case '1' :					/* OPOS enable scanner */
			case '0' :					/* OPOS disable scanner */
			case BEL :                 /* Beep */
			case DC1 :                 /* LEAN_AUTOMATION Test Engineering commands */

				pucBuffer[ui++]=ucData ;
				uiState=INPUT_COMMAND_BYTE_RECEIVED_STX_WAITING_FOR_COMMAND_DATA ;
			case SYN :
				break ;

			case 0xFE:					// Dispatch the <SYN>FE handler for protocol wrapped commands
				SYNFECommandHandler->ReceiveAndParseServer(pucBuffer, "\x02\xFE");
				uiState=INPUT_RESET__WAITING_FOR_STX ;

				//			if( GET_SETTING( DebugCommandProccessingTraceMode ) == TRACE_MESSAGE_MODE_ON )
				//			{
				//				theStdInterface.Write( "Finished processing <SYN>FE\r\n" );
				//			}
				break;

			default :
				uiState=INPUT_RESET__WAITING_FOR_STX ;
			}

			ReceivedByteWasPartOfASynCommand = true;
		}
	
		/* ========================================================================
			State INPUT_COMMAND_BYTE_RECEIVED_STX_WAITING_FOR_COMMAND_DATA :
	
			A SYN character and valid command byte have been received - Now waiting
			valid command data folled by a [CR]
			======================================================================== */
	
		else if( INPUT_COMMAND_BYTE_RECEIVED_STX_WAITING_FOR_COMMAND_DATA==uiState )
		{
			if( CR !=ucData )
			{
				if( ui==SIZEOF_HIGHLEVEL_PARSE_BUFFER )
				{
					SoundErrorBeep();
					uiState=INPUT_RESET__WAITING_FOR_STX ;
				}
				else if (STX==ucData)
				{
					uiState=INPUT_STX_RECEIVED__WAITING_FOR_COMMAND_BYTE;
				}
				else
				{
					pucBuffer[ui++]=ucData;
				}
			}
			else
			{
				uiState=INPUT_RESET__WAITING_FOR_STX ;
				switch( ((pucBuffer[0] >= 'a') && (pucBuffer[0] <= 'z')) ? (pucBuffer[0] - 32) : pucBuffer[0] )//toupper(pucBuffer[0]) )
				{

#ifdef LEAN_AUTOMATION
				case DC1 :
					{
						pucBuffer[ui] = 0;
						if (ProcessLeanAutomationTestCommand(pucBuffer))
							theCurrentInterface.Write("\006");
						else
							theCurrentInterface.Write("\025");
						break;
					}
#endif

				case 'C' :
					{
						// Respond with ACK NAK DEh DFh
						theCurrentInterface.Write("\x06\x15\xde\xdf");

						break;
					}

				case 'D' :
					{
						//**					bSpecialDebugMode = TRUE;
						break;
					}

				case 'E' :
				case 'F' :
				case 'A' :              // this case needs to follow e and f because flashing is done here

					{
						// Return a NAK, because this product doesn't process these commands.
						theCurrentInterface.Write("\025");
						break;
					}

				case 'G' :
					{
						/* Insert a NUL terminating character so we can perform string compares */
						pucBuffer[ui]='\0' ;

						if(strcmp(pucBuffer+1, "300") &&
								strcmp(pucBuffer+1, "600") &&
						        strcmp(pucBuffer+1, "1200") &&
						        strcmp(pucBuffer+1, "2400") &&
						        strcmp(pucBuffer+1, "4800") &&
						        strcmp(pucBuffer+1, "9600") &&
						        strcmp(pucBuffer+1, "19200") &&
						        strcmp(pucBuffer+1, "38400") &&
						        strcmp(pucBuffer+1, "57600") &&
						        strcmp(pucBuffer+1, "115200"))
							theCurrentInterface.Write("\025");
						else
							theCurrentInterface.Write("\006");
						break;
					}

				case 'H' :                 /* Stored Checksum only */	//DF
					{
						//**					sprintf(pucBuffer,"%04X%04X%04X%04X%c",0, 0, ((BootHeader_t*)AppVectors)->Checksum, ((BootHeader_t*)AppVectors)->Wchecksum, 0x06);
						//**					xputstring(pucBuffer, 17);
						break;
					}

				case 'I' :
					{
						reset_to_boot_code_hard();
					}

				case 'M' :
					{
						ui=0 ;
						uiState=INPUT_STX_MENU_COMMAND_RECEIVED__WAITING_FOR_MENU_DATA ;

						break ;
					}

				case 'N' :
					{
						pucBuffer[ui] = 0;
						ProcessNonMenuCommands(pucBuffer, ui);
						break;
					}

				case BEL :
					{
						if(ui > 1)	               // extended bel command ?
						{
							switch (pucBuffer[1])
							{
							case 'e':  // error beep?
								SoundErrorBeep();
								break;
							case 'p':  // powerup beep?
								SoundPowerupBeep();
								break;
							case 'u':
								break;

							default:
								SoundStandardBeep(1);
								break;
							}
						}
						else	                     // old simple bel command
						{
							SoundStandardBeep(1);
						}
						break ;
					}
				case 'T' :
					{
#ifdef RFBASE
#else

						if( pTrigger->GetTriggerMode() == TRGMOD_SNAP_AND_SHIP)
						{
							ImageSnapAndShip();
							//FIXME						if(!ImageSnapAndShip())
							//FIXME							NoDecodeBeep();		//Added in Gen5.5
						}
						else
						{
							pTrigger->TriggerSerial();

						}
#endif
						break ;
					}
				case 'U' :
					{
#ifdef RFBASE
#else

						pTrigger->UnTriggerSerial();
#endif

						break ;
					}


				case 'Y' :
					{
						ui=0 ;
						uiState=INPUT_STX_PRODUCT_MENU_COMMAND_RECEIVED__WAITING_FOR_PRODUCT_MENU_DATA ;
						break ;
					}

				case 'Z' :	/* stop a script */
					{
						//**					TerminateHHPLProgram();
						//**					Script =NULL;
						break;
					}
				case '1' :  //enable OPOS
					{
#ifndef RFBASE
						theScriptInterpreter.EnableOPOS();
						pTrigger->FinishScanning();// do lights and start/stop scanning per trigger mode
						pucBuffer[ui++] = ACK;
						theCurrentInterface.Write( pucBuffer, 2 ) ;
#endif

						break;
					}
				case '0' :  //enable OPOS
					{
#ifndef RFBASE
						theScriptInterpreter.DisableOPOS();
						pucBuffer[ui++] = ACK;
						theCurrentInterface.Write( pucBuffer, 2 ) ;
#endif

						break;
					}

				}     // switch end

			}   		// end if-else

			ReceivedByteWasPartOfASynCommand = true;
		}
		/* ========================================================================
			State INPUT_STX_MENU_COMMAND_RECEIVED__WAITING_FOR_MENU_DATA :
	
			A [SYN]M[CR] command sequence has been received - Wait for a complete
			menu string terminated with a '.' or '!' to be recevied
			======================================================================== */
	
		else if( INPUT_STX_MENU_COMMAND_RECEIVED__WAITING_FOR_MENU_DATA==uiState )
		{
			if( ui==SIZEOF_HIGHLEVEL_PARSE_BUFFER )
			{
				SoundErrorBeep();
				uiState=INPUT_RESET__WAITING_FOR_STX ;
			}
			else if ( STX==ucData )
			{
				uiState=INPUT_STX_RECEIVED__WAITING_FOR_COMMAND_BYTE;
			}
			else
			{
				pucBuffer[ui++]=ucData ;
				if(('!'==ucData) || ('.'==ucData) || ('&' == ucData) || ('@' == ucData))
				{
					pucBuffer[ui] = 0;

					bool bCommandWasForwarded;
					bool bPrefixFound = ForwardPrefixedCommand(true, (const unsigned char *)pucBuffer, ui, &bCommandWasForwarded);
					// For RFBASE or RFSCAN, don't process this command if it turns out that there was
					//	a device name on the front of the command and we sent the command to that device.
					uiState=INPUT_RESET__WAITING_FOR_STX ;

					if ( bPrefixFound && ! bCommandWasForwarded )
					{	// User tried to send a Menu Command to a scanner from a base & there wasn't anything connected to the base.  Create a NAK reply
						HSTRING MenuCommand(pucBuffer, true);
						MenuCommand.ReplaceAll("?", "");
						MenuCommand.Insert("\x15", MenuCommand.Size - 1);
						SendResponse(&MenuCommand);
					}
					else if( ! bPrefixFound )
					{
#ifdef SAVE_ADDRESS_HISTORY

						if (DebugMenuCommandAndResponse ==NULL) DebugMenuCommandAndResponse =new HSTRING("",true);
						DebugMenuCommandAndResponse->Copy(pucBuffer);
#endif

						HSTRING MnuCommand("",true);
						#ifndef RFSCAN
						HSTRING MnuCommandCopy("",true);
						#endif
						/*	Call the Menu	*/
						UINT length;
						length = strlen(pucBuffer); /*get length of menu command*/
	//					theCurrentInterface.printf("length is %d\r\n", length);

						/*Return messages are passed in pucBuffer so it needs to be big enough for the larges possible menu response */
						/*Menu will update the length parameter with the length of the reponse. */
						// If the last character in the response string is a ";" (due to a hidden entry being skipped), remove it.
						if((length > 1) &&	((pucBuffer[length - 2] == ';')||(pucBuffer[length - 2] == ',')))
						{
							pucBuffer[length-2] = pucBuffer[length-1];
							length -= 1;
							pucBuffer[length] = 0;
						}

						if(GET_SETTING(UserQueryAuthority) != ALLOW_SUB_TAG_QUERY) {
							CheckSubTagQueryCommand(pucBuffer, true);
						}


#ifdef PLUGIN_DEV
							// Abstract plugin/system menu commands.
							HSTRING PluginMenuCommand("",true);
							HSTRING SystemMenuCommand("",true);
							PluginMenuCommand.Copy(pucBuffer);
							SystemMenuCommand.Copy(pucBuffer);

							int PluginCmd = 0, SystemCmd = 0;

							PluginCmd = AbstractPluginMenuCommands(&PluginMenuCommand);
							SystemCmd = AbstractSystemMenuCommands(&SystemMenuCommand);


							if(PluginCmd)
							{

								//process 990 menu barcode for plugin
								PluginSerialMenuCommandProcess_Ext(&PluginMenuCommand);
								MnuCommand.Copy(&PluginMenuCommand);

								if(SystemCmd){
									// Process system commands
									ProcessNonPluginMenuCommands(&SystemMenuCommand);
									// Combine the results together
									MnuCommand.Remove(MnuCommand.Size - 1, 1);
									MnuCommand.AppendChar(';');
									MnuCommand.Append(&SystemMenuCommand);
								}

								SendResponse(&MnuCommand);
								//ReSynchWorkingAndActive(); // fixme ???
								uiState=INPUT_RESET__WAITING_FOR_STX ;
#ifndef RFBASE

								theScriptInterpreter.ResetScanningState();
#endif

							}else{
#endif //PLUGIN_DEV
								MnuCommand.Copy(pucBuffer);
#ifndef RFSCAN
								MnuCommandCopy.Copy(pucBuffer);
								int PositionCBLTBLWasFound;
								HSTRING ConfigCabSel(20,false);
								ConfigCabSel.Copy("060300");
#endif
								//	MnuCommandCopy.FindString(&ConfigCabSel,false,1,&PositionCBLTBLWasFound);
								ReaderConfiguration->Menu(&MnuCommand);
#ifndef RFSCAN
								if(ReaderConfiguration->TableSettingCommandisProcessing)
									ReaderConfiguration->TableSettingCommandisProcessing=false;
								if(ReaderConfiguration->TwoTableUpdateSimultaneously)
								{
									ReaderConfiguration->TwoTableUpdateSimultaneously=false;
									MnuCommandCopy.Find(&ConfigCabSel,false,1,&PositionCBLTBLWasFound);
									MnuCommandCopy.Remove(0,PositionCBLTBLWasFound+11);
									if(ReaderConfiguration->CurrentWorkTable()){
										SaveSettingChangeToTableOne();
										LoadWorkingConfigurationZero();
										ReaderConfiguration->Menu(&MnuCommandCopy);
										SaveSettingChangeToTableZero();
										LoadWorkingConfigurationOne();
									}
									else
									{
										SaveSettingChangeToTableZero();
										LoadWorkingConfigurationOne();
										ReaderConfiguration->Menu(&MnuCommandCopy);
										SaveSettingChangeToTableOne();
										LoadWorkingConfigurationZero();
									}
									if(ReaderConfiguration->TableSettingCommandisProcessing)
									ReaderConfiguration->TableSettingCommandisProcessing=false;

								}
								else if(ReaderConfiguration->CurrentWorkTable()!=ReaderConfiguration->GetActiveWorktableNumber())
								{
									if(ReaderConfiguration->CurrentWorkTable())
									{
										SaveSettingChangeToTableOne();
										LoadWorkingConfigurationZero();
									}
									else
									{
										SaveSettingChangeToTableZero();
										LoadWorkingConfigurationOne();
									}
									ReaderConfiguration->SetWorkTable(ReaderConfiguration->GetActiveWorktableNumber());

								}
#endif
								KCODE_MODE(FALSE);
								FlashAndReSynchCustomerDefaults();
								FlashWorkingChanges();
#ifndef RFBASE
								PhotoJustTaken = FALSE;
#endif
								SendResponse(&MnuCommand);
								//ReSynchWorkingAndActive(); // fixme ???
								uiState=INPUT_RESET__WAITING_FOR_STX ;
#ifndef RFBASE

								theScriptInterpreter.ResetScanningState();
#endif

#ifdef PLUGIN_DEV
							}
#endif
					}
				}
			}

				ReceivedByteWasPartOfASynCommand = true;
		}
			/* ========================================================================
				State INPUT_STX_PRODUCT_MENU_COMMAND_RECEIVED__WAITING_FOR_PRODUCT_MENU_DATA :
	
				A [SYN]Y[CR] command sequence has been received - Wait for a complete
				menu string terminated with a '.' or '!' to be recevied
				======================================================================== */
	
		else if( INPUT_STX_PRODUCT_MENU_COMMAND_RECEIVED__WAITING_FOR_PRODUCT_MENU_DATA==uiState )
		{
			if( ui==SIZEOF_HIGHLEVEL_PARSE_BUFFER )
			{
				SoundErrorBeep();
				uiState=INPUT_RESET__WAITING_FOR_STX ;
			}

			else if ( STX==ucData )
			{
				uiState=INPUT_STX_RECEIVED__WAITING_FOR_COMMAND_BYTE;
			}
			else
			{
				pucBuffer[ui++]=ucData;
				if(('!'==ucData) || ('.'==ucData))
				{
					pucBuffer[ui] = 0;
					HSTRING	MenuCommandAndResponse(pucBuffer, true);
					uiState=INPUT_RESET__WAITING_FOR_STX ;
					bool bCommandWasForwarded;
					bool	bPrefixFound = ForwardPrefixedCommand(false, (const unsigned char *)pucBuffer, ui, &bCommandWasForwarded);

					

					if ( bPrefixFound && ! bCommandWasForwarded )
					{	// User tried to send a Product Config Command to a scanner from a base & there wasn't anything connected to the base.  Create a NAK reply
						MenuCommandAndResponse.ReplaceAll("?", "");
						MenuCommandAndResponse.Insert("\x15", MenuCommandAndResponse.Size - 1);
						SendResponse(&MenuCommandAndResponse);
					}
					else if( ! bPrefixFound )
					{	/*	Call the Menu	*/
						ProductConfiguration->Menu(&MenuCommandAndResponse);
						FlashProductConfigurationChanges();
						theCurrentInterface.Write((char *) MenuCommandAndResponse.Char, MenuCommandAndResponse.Size);
					}
				}
			}
			ReceivedByteWasPartOfASynCommand = true;
		}
			Busy = false;
			return(ReceivedByteWasPartOfASynCommand) ;
		}


	// Only return a response to the sender if the message was sent via the standard interface.  If sent over the radio, don't return a response.
	void STX_COMMAND_PARSE::SendResponse(const HSTRING * pMenuCommand)
	{
		if ( (GetCurrentInterface() == g_pStdInterface) || (GetCurrentInterface() == g_pRemInterface) )
		{
			CBarcodeData * menumsg;
			menumsg = CBarcodeData::StoreToNewBuffer(pMenuCommand->Char,pMenuCommand->Size);
			menumsg->StoreHHP('6', 0);
			menumsg->StoreAim('Z', '6');
			theCurrentInterface.Write(menumsg);
		}
	}
	
	
	
		STX_COMMAND_PARSE ** STXCommandParser;
		STX_COMMAND_PARSE * OWSTXCommandParser;
		STX_COMMAND_PARSE * g_pRemStxCommandParser;

