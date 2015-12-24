/* ==============================================================================
   Matrix Menu Module Source
   ==============================================================================
 
   $RCSfile: Gen6FoundationApp/FoundationApp/menu/Menu.c $
   $Revision: 1.18 $
   $Date: 2011/07/12 13:06:48EDT $
 
   ==============================================================================
 
   ============================================================================== */



// Issues:
//	- In AllEnable, all RAM entries used to overwrite the ROM entries.  Now, only the
//		updated entries overwrite.
//	- For all macro types, make sure that the actual value is also copied to the ROM
//		value.  Currently, we're only doing this for block data.
//	- Need to be able to load all table entries.  We probably want to use a special
//		query character, which will use brackets to separate the setting value for
//		each table ("$" will have to recognize this, but we probably want another
//		query command that will give us the information in the new format, unlike "?".
//		Note that we'll also update settings based on this same bracketed syntax; if
//		there is only one parameter, then it is applied to the current table.
//	- Make sure that settings are stored and retrieved properly in KCODE mode.
//

#define	MAIN_MENU

//#define _DEBUG 1
#include <stdio.h>
#include "stdInclude.h"
#include "Beep.h"
#include "dataedit.h"
#include "db_chrio.h"        /* Character I/O header prototypes */
#include "IOMan.h"
#include "readerconfiguration.h"
#include "MatrixSynFECommandHandler.h"
#include "timeout.h"
#include "AutoCableSelect.h"




/*-----------------------------------------------------------------------------------------
// Menu memory functions
//-----------------------------------------------------------------------------------------*/

#include "codepage.h"
#include "menu.h"
#include "MenuFlash.h"

//-------------------------------------------------------------------
//  Menu Utility Function Prototypes
//-------------------------------------------------------------------

BOOL			bKcodeMode;
Kcode_t		Kcode;
MenuMsg_t *	pTagBuffer;
bool ProcessingScannedMenu=false;
#ifndef RFBASE
extern bool AffectedDefalt;
#endif
extern void LoadWorkingConfigurationOne();
extern void LoadWorkingConfigurationZero();
#ifndef SIZEOF_MAX_MESSAGE
#define  SIZEOF_MAX_MESSAGE 3000
#endif

typedef struct _tagScannedMenuTable_t
{
   const unsigned char	*pMStr1;
   const unsigned char	*pMStr2;
} MenuScannedTable_t;

static MenuScannedTable_t ScannedTable[] =
{
	{"CMDDFT.",	"090100."},
	{"ADD_CR.",	"090200."},
	{"ADD_LF.", "090202."},
	{"232USB.",	"090400."},
	{"KBWUSB.",	"090500."},
	{"A_CRLF.", "090300."},
	{"ADDTAB.",	"090201."},
	{"IMGRVT.",	"090600."},
	{"CRRPSC.", "090700."},
	{"BRNTST.",	"090800."},
	{"WRDN71.",	"090E00."},
	{"WRDN72.", "090E01."},
	{"WRDN81.",	"090E02."},
	{"WRDE71.",	"090E03."},
	{"WRDE72.", "090E04."},
	{"WRDE81.",	"090E05."},
	{"WRDO71.",	"090E06."},
	{"WRDO72.", "090E07."},
	{"WRDO81.",	"090E08."},
	{"WRDM81.",	"090E09."},
	{"BR0300.", "090D00."},
	{"BR0600.",	"090D01."},
	{"BR1200.",	"090D02."},
	{"BR2400.", "090D03."},
	{"BR4800.",	"090D04."},
	{"BR9600.",	"090D05."},
	{"BR19K2.", "090D06."},
	{"BR38K4.",	"090D07."},
	{"BR57K6.",	"090D08."},
	{"BR115K.", "090D09."},
	{"PRESNT.",	"090901."},
	{"PRESNC.",	"090902."},
	{"CELLPH.", "091B00."},
	{"FULDOF.",	"091A00."},
	{"SNAPPY.",	"091900."},
	{"PREAST.", "090900."},
	{"SUFAST.",	"090A00."},
	{"CLEARP.",	"090C00."},
	{"CLEARS.", "090C01."},
	{"S_ASCR.",	"090B00."},
	{"PREFIX",	"080400"},
	{"SUFFIX", 	"080500"},
	{"CUNTRY",	"060E00"},
	{"MNLTRG.",	"090F00."},
	{"B64ENA1.","0806001."},
	{"39CHKN.",	"091100."},
	{"39CHKR.",	"091101."},
	{"39CHKX.", "091102."},
	{"CRRPCM.",	"090701."},
	{"232GRP.",	"090401."},
	{"USBGRP.", "091000."},
	{"ILL_ON1.","0501001."},
	{"ILL_ON0.","0501000."},
};

#define SCANNEDTABLESIZE		53
/*-----------------9/15/98 10:54AM------------------
 * The Menu() function processes all menu messages, regardless of whether they originated from the
 * barcode menu or the serial port.
 *
 * Inputs:
 * puiLength points to the length of the message.
 *
 * pucMessage points to the message to be processed.
 *
 * Outputs:
 * A byte of flag bits that indicate:
 * 	MENU_PASSED - command(s) in pucMessage were valid
 * 	MENU_QUERY - pucMessage contains a query response
 * 	MENU_RECONFIGURE - scanner should be reconfigured
 *
 * If the message was a query then the response is placed in pucMessage and puiLength is adjusted.
 * --------------------------------------------------*/
unsigned BYTE_T Menu(unsigned int *puiLength, unsigned char *pucMessage)
{
	return((unsigned BYTE_T) ReaderConfiguration->Menu(puiLength, pucMessage));
}

void IndicateCustomDefaultProgrammingMode( void )
{
	static CTimeoutmS indicatorTimeout(100);
	
	if( ReaderConfiguration->CustomDefaultProgrammingMode )
	{
		if( indicatorTimeout.HasExpired() )
		{
			BlinkCustomDefaultIndicator();
			indicatorTimeout.SetExpiration(1000);
		}
	}
}

int Compare232BaudRateCommand(unsigned char *pucMessage)
{
	if((memcmp(pucMessage,"BR0300.",7)!=0)&&(memcmp(pucMessage,"BR0600.",7)!=0)&&(memcmp(pucMessage,"BR1200.",7)!=0)&&(memcmp(pucMessage,"BR2400.",7)!=0)\
		&&(memcmp(pucMessage,"BR4800.",7)!=0)&&(memcmp(pucMessage,"BR9600.",7)!=0)&&(memcmp(pucMessage,"BR19K2.",7)!=0)&&(memcmp(pucMessage,"BR38K4.",7)!=0)\
		&&(memcmp(pucMessage,"BR57K6.",7)!=0)&&(memcmp(pucMessage,"BR115K.",7)!=0))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int Compare232WordFormatCommand(unsigned char *pucMessage)
{
	if((memcmp(pucMessage,"WRDN71.",7)!=0)&&(memcmp(pucMessage,"WRDN72.",7)!=0)&&(memcmp(pucMessage,"WRDN81.",7)!=0)&&(memcmp(pucMessage,"WRDE71.",7)!=0)\
		&&(memcmp(pucMessage,"WRDE72.",7)!=0)&&(memcmp(pucMessage,"WRDE81.",7)!=0)&&(memcmp(pucMessage,"WRDO71.",7)!=0)&&(memcmp(pucMessage,"WRDO72.",7)!=0)\
		&&(memcmp(pucMessage,"WRDO81.",7)!=0)&&(memcmp(pucMessage,"WRDM81.",7)!=0))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//Scanned menu cannot query
int SearchQueryChar(unsigned char *pucMessage)
{
	int len = strlen(pucMessage);
	int i = 0;

	for(i=0;i<len;i++)
	{
		if((pucMessage[i]=='$')||(pucMessage[i]=='?')||(pucMessage[i]=='*')||(pucMessage[i]=='^')\
			||(pucMessage[i]==';')||(pucMessage[i]==',')||(pucMessage[i]=='!')||(pucMessage[i]=='&'))
		{
			return 1;
		}
	}

	return 0;
}

/**
 * @brief	This function find if there is sub tag query in command message, 
 * 			like "?", "050?", if find these sub tag query will return True.
 * @param	pucMessage the input message buffer.
 * @param	RemoveSubQueryCommand Set it [true] to remove the sub tag query command, 
 * 			[flase] do nothing.
 * @return	[true] if found sub tag query command; [false] if not found.
 */
bool CheckSubTagQueryCommand(char *pucMessage, bool RemoveSubQueryCommand)
{
	int positionStart = 0;
	int positionWhereChar = 0;
	bool foundChar = false;
	bool foundSubTagQuery = false;
	HSTRING InputCommandMsg("", true);

	InputCommandMsg.Copy(pucMessage);

	// find string length less than 6 between ';'
	do {
		foundChar = InputCommandMsg.FindCharacterForward(';', positionStart, &positionWhereChar);
		if((positionWhereChar - positionStart) < SIZE_OF_TAG + SIZE_OF_SUB_TAG) {
			foundSubTagQuery = true;
			break;
		}
		positionStart = positionWhereChar + 1;
	} while(foundChar);

	// find ','
	positionStart = 0;
	positionWhereChar = 0;
	do {
		foundChar = InputCommandMsg.FindCharacterForward(',', positionStart, &positionWhereChar);
		if (foundChar) {
			foundSubTagQuery = true;
			break;
		}
		positionStart = positionWhereChar + 1;
	} while(foundChar);

	if ((foundSubTagQuery) && (RemoveSubQueryCommand)) {
		InputCommandMsg.Remove(0, InputCommandMsg.Size);
		strcpy(pucMessage, (char *)InputCommandMsg.Char);
	}

	return foundSubTagQuery;
}

int ProcessScannedMenuCommand( unsigned int *puiLength, unsigned char *pucMessage )
{
	ProcessingScannedMenu=true;
	bool bWasForwarded;


	bool MenuForwarded = ForwardPrefixedCommand(true, pucMessage, *puiLength, &bWasForwarded);
	//theStdInterface.printf("\r\nProcessScannedMenuCommand:%s\r\n", pucMessage);
#if defined(SM5130)
	if(*puiLength == 4)//BT command 
	{
		if(GET_SETTING(TerminalID)==0)
		{
			xputchar(0x02);
			xputchar(0x0);
			xputchar(0x05);
			xputchar(0x83);
			xputchar(pucMessage[0]);
			xputchar(pucMessage[1]);
			xputchar(pucMessage[2]);
			xputchar(pucMessage[3]);
			xputchar(0x03);
			return MENU_PASSED;
		}
	}
	else if(memcmp(pucMessage,"BC:CS=",6)==0)
	{
		if(GET_SETTING(TerminalID)==0)
		{
			xputchar(0x02);
			xputchar((*puiLength+1)>>8);
			xputchar(((*puiLength+1)&0xFF));
			xputchar(0x83);
			{
				int i=0;
				for(i=0;i<*puiLength;i++)
				{
					xputchar(pucMessage[i]);
				}
			}
			xputchar(0x03);
			return MENU_PASSED;
		}
	}
	else if((memcmp(pucMessage,"SX",2)==0)&&(*puiLength>=9)) //forward configuration to cradle
	{
		if(GET_SETTING(TerminalID)==0)
		{
			xputchar(0x02);
			xputchar((*puiLength+1)>>8);
			xputchar(((*puiLength+1)&0xFF));
			xputchar(0x83);
			{
				int i=0;
				for(i=0;i<*puiLength;i++)
				{
					xputchar(pucMessage[i]);
				}
			}
			xputchar(0x03);
			return MENU_PASSED;
		}
	}
#endif	

#if 0
			if(SearchQueryChar(pucMessage)!=0)
			{
				return MENU_ERROR;
			}
#endif
	
#if 0
	if ((memcmp(pucMessage,"CMDDFT.",7)!=0)&&(memcmp(pucMessage,"ADD_CR.",7)!=0)&&(memcmp(pucMessage,"ADD_LF.",7)!=0)&&(memcmp(pucMessage,"232USB.",7)!=0)\
		&&(memcmp(pucMessage,"KBWUSB.",7)!=0)&&(memcmp(pucMessage,"A_CRLF.",7)!=0)&&(memcmp(pucMessage,"CMDENA1.",8)!=0)&&(memcmp(pucMessage,"ADDTAB.",7)!=0)\
		&&(memcmp(pucMessage,"IMGRVT.",7)!=0)&&(memcmp(pucMessage,"CRRPSC.",7)!=0)&&(memcmp(pucMessage,"BRNTST.",7)!=0)&&(Compare232WordFormatCommand(pucMessage)!=0)\
		&&(Compare232BaudRateCommand(pucMessage)!=0)&&(memcmp(pucMessage,"PRESNT.",7)!=0)&&(memcmp(pucMessage,"PRESNC.",7)!=0)&&(memcmp(pucMessage,"CELLPH.",7)!=0)\
		&&(memcmp(pucMessage,"FULDOF.",7)!=0)&&(memcmp(pucMessage,"SNAPPY.",7)!=0)&&(memcmp(pucMessage,"PREAST.",7)!=0)&&(memcmp(pucMessage,"SUFAST.",7)!=0)\
		&&(memcmp(pucMessage,"CLEARP.",7)!=0)&&(memcmp(pucMessage,"CLEARS.",7)!=0)&&(memcmp(pucMessage,"S_ASCR.",7)!=0)&&(memcmp(pucMessage,"PREFIX",6)!=0)\
		&&(memcmp(pucMessage,"SUFFIX",6)!=0)&&(memcmp(pucMessage,"CUNTRY",6)!=0)&&(memcmp(pucMessage,"MNLTRG.",7)!=0)&&(memcmp(pucMessage,"B64ENA1.",8)!=0)\
		&&(memcmp(pucMessage,"39CHKN.",7)!=0)&&(memcmp(pucMessage,"39CHKR.",7)!=0)&&(memcmp(pucMessage,"39CHKX.",7)!=0)&&(memcmp(pucMessage,"CRRPCM.",7)!=0)\
		&&(memcmp(pucMessage,"232GRP.",7)!=0)&&(memcmp(pucMessage,"USBGRP.",7)!=0)&&(memcmp(pucMessage,"ILL_ON1.",8)!=0)&&(memcmp(pucMessage,"ILL_ON0.",8)!=0))
	{
		//theStdInterface.printf("\r\nReturn Error!\r\n");
		//return MENU_ERROR;
	}
#endif	

#if 1
	{
		unsigned int i=0;
		for(i=0;i<SCANNEDTABLESIZE;i++)
		{
			if(strcmp(pucMessage,ScannedTable[i].pMStr1)==0)
			{
				strcpy(pucMessage,ScannedTable[i].pMStr2);
			}
		}

		if(memcmp(pucMessage,"PREFIX",6)==0)
			memcpy(pucMessage,"080400",6);
		
		if(memcmp(pucMessage,"SUFFIX",6)==0)
			memcpy(pucMessage,"080500",6);

		if(memcmp(pucMessage,"CUNTRY",6)==0)
			memcpy(pucMessage,"060E00",6);
	}
#endif

	int		Status =1;
	if (!MenuForwarded)
	{
#ifndef RFSCAN
		int PositionTableSettingWasFound;
		int PositionSettingWasFound;

		// Use AutoCable_StrIndex function instead of HSTRING->FindString since
		// the FindString function doen't work
		PositionTableSettingWasFound = AutoCable_StrIndex(pucMessage,(unsigned char*)"CBL");
		PositionSettingWasFound = AutoCable_StrIndex(pucMessage,(unsigned char*)"cbl");
		if((PositionTableSettingWasFound == -1)&&(PositionSettingWasFound == -1)&&ReaderConfiguration->ScannedCBLTBLMenu&&(ReaderConfiguration->GetActiveWorktableNumber()!=ReaderConfiguration->CurrentWorkTableForScannedMenu()))
		{
			if (ReaderConfiguration->CurrentWorkTableForScannedMenu())
			{
				LoadWorkingConfigurationOne();
				Status =ReaderConfiguration->Menu(puiLength, pucMessage);
				SaveSettingChangeToTableOne();
				LoadWorkingConfigurationZero();
			}
			else
			{
				LoadWorkingConfigurationZero();
				Status =ReaderConfiguration->Menu(puiLength, pucMessage);
				SaveSettingChangeToTableZero();
				LoadWorkingConfigurationOne();
			}
		}
		else if((PositionTableSettingWasFound == -1) && (PositionSettingWasFound == -1)&&ReaderConfiguration->ScannedCBLTBLMenu&&(ReaderConfiguration->TableSettingUpdateSimultaneouslyForScannedCommand))
		{
			HSTRING MnuCopy(pucMessage,*puiLength,true);
			Status =ReaderConfiguration->Menu(puiLength, pucMessage);
			if(ReaderConfiguration->CurrentWorkTableForScannedMenu())
			{
				SaveSettingChangeToTableOne();
				LoadWorkingConfigurationZero();
				ReaderConfiguration->Menu(&MnuCopy);
				SaveSettingChangeToTableZero();
				LoadWorkingConfigurationOne();
			}
			else
			{
				SaveSettingChangeToTableZero();
				LoadWorkingConfigurationOne();
				ReaderConfiguration->Menu(&MnuCopy);
				SaveSettingChangeToTableOne();
				LoadWorkingConfigurationZero();
			}
		}
		else if(((AutoCable_StrIndex(pucMessage,(unsigned char*)"060300") != -1))&&(AutoCable_StrIndex(pucMessage,(unsigned char*)";") != -1))
		{
			HSTRING MnuCommandCopy("",true);
			HSTRING MnuCommand("",true);
			MnuCommand.Copy(pucMessage);
			MnuCommandCopy.Copy(pucMessage);
			Status =ReaderConfiguration->Menu(puiLength, pucMessage);
			if (Status&&(ReaderConfiguration->GetActiveWorktableNumber()!=ReaderConfiguration->CurrentWorkTableForScannedMenu()))
			{
				MnuCommandCopy.Remove(0,AutoCable_StrIndex(pucMessage,(unsigned char*)"060300")+8);
				MnuCommand.Remove(AutoCable_StrIndex(pucMessage,(unsigned char*)"060300"),MnuCommandCopy.Size-AutoCable_StrIndex(pucMessage,(unsigned char*)"cbltbl"));
				if (ReaderConfiguration->CurrentWorkTableForScannedMenu())
				{
					LoadWorkingConfigurationOne();
					ReaderConfiguration->Menu(&MnuCommandCopy);
					SaveSettingChangeToTableOne();
					LoadWorkingConfigurationZero();
				}
				else
				{
					LoadWorkingConfigurationZero();
					ReaderConfiguration->Menu(&MnuCommandCopy);
					SaveSettingChangeToTableZero();
					LoadWorkingConfigurationOne();
				}
				ReaderConfiguration->Menu(&MnuCommand);
			}
			else if(ReaderConfiguration->TableSettingUpdateSimultaneouslyForScannedCommand)
			{
				MnuCommandCopy.Remove(0,AutoCable_StrIndex(pucMessage,(unsigned char*)"060300")+11);
				MnuCommand.Remove(AutoCable_StrIndex(pucMessage,(unsigned char*)"060300"),MnuCommandCopy.Size-AutoCable_StrIndex(pucMessage,(unsigned char*)"cbltbl"));
				if(ReaderConfiguration->CurrentWorkTableForScannedMenu())
				{
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
				ReaderConfiguration->Menu(&MnuCommand);
			}
			if(Status&&ReaderConfiguration->ScannedCBLTBLMenu)
			{	
				ReaderConfiguration->ScannedCBLTBLMenu = false;
			}
		}
		else
#endif

			Status = ReaderConfiguration->Menu(puiLength, pucMessage);
	}
	ProcessingScannedMenu=false;

	return Status;
}



/*------------------------------------------------------------------------------
// EnterScannedMenu() - Go decide whether we will be buffering Kcodes here, or
 * if we will be processing a complete menu string
//------------------------------------------------------------------------------*/

int EnterScannedMenu(unsigned int *puiLength, unsigned char *pucMessage)
{
	bool bMaxLength=FALSE;
	bool * bpMaxLengthReached=&bMaxLength;
	int iTempPtr;
	//theStdInterface.printf("\r\nEnterScannedMenu:%s\r\n", pucMessage);

	if(GET_SETTING(UserQueryAuthority) != ALLOW_SUB_TAG_QUERY) {
		CheckSubTagQueryCommand((char *)pucMessage, true);
	}

	if(strlen((char *)pucMessage)>7)
	{
		if(IS_KCODE_MODE()==TRUE)
			KCODE_MODE(FALSE);
		return(ProcessScannedMenuCommand(puiLength, pucMessage)); //too long for Kcode, process the data
	}
	if(CONTAINS_OCR_DATA(pucMessage)&&IS_KCODE_MODE()==TRUE) //is this new OCR Template data??
	{
		pucMessage[2]=0;
		pucMessage[5]=0;
		strcat((char *)Kcode.nBuffer,(char *)(pucMessage+1));  //get the Kcode data
		strcat((char *)Kcode.nBuffer,(char *)(pucMessage+4));  //get the Kcode data

	}
	else if(CONTAINS_KCODE_DATA(pucMessage)&&IS_KCODE_MODE()==TRUE) //is this new KxK data??
	{
		pucMessage[2]=0;
		strcat((char *)Kcode.nBuffer,(char *)(pucMessage+1));  //get the Kcode data
	}
	else
	{
		if(IS_KCODE_MODE()==FALSE && strlen((char *)pucMessage)==7)
		{
			Kcode.nBuffer[0]=0;		//clear out the Kcode buffer
			strcat((char *)Kcode.nBuffer,(char *)pucMessage);  //get the data into the Kcode buffer
			iTempPtr=strlen((char *)Kcode.nBuffer)-1;	//find the terminator

			// Get rid of the terminator if it is not the customer defaults table character.
			if( Kcode.nBuffer[iTempPtr] != '@' || AffectedDefalt )
				Kcode.nBuffer[iTempPtr]=0;
		}
		else if(stricmp((char *)pucMessage,"0D0500."))//if not an abort, its a save or other command
		{                                    //go save the previously buffered Kcode stuff
			KCODE_MODE(FALSE);
			if(Kcode.nBuffer[0]!=NULL)
			{
				if(AffectedDefalt)
					strcat((char *)Kcode.nBuffer,"@");							//MAX hit, terminate string.
				else
					strcat((char *)Kcode.nBuffer,".");	
				strcpy((char *)pucMessage,(char *)Kcode.nBuffer);
				Kcode.nBuffer[0]=0;		//clear out the Kcode buffer
			}
			*puiLength=strlen((char *)pucMessage);
			return(ProcessScannedMenuCommand(puiLength,pucMessage));
		}
		else	//this was a MNUABT. clear Kcode mode and get outta here.
		{
			KCODE_MODE(FALSE);
			Kcode.nBuffer[0]=0;		//clear out the Kcode buffer
			return(MENU_PASSED);
		}
	}

	// Validate the command.  If ValidatePartialCommand fails, and this is an RFSCAN build, and we're not in dongle mode, and the command failed,
	// because the scanner doesn't recognize it, treat the command as valid anyway, because the base may recognize it.
	bool	CommandIsValid = ReaderConfiguration->ValidatePartialCommand((char *)Kcode.nBuffer, bpMaxLengthReached);


	if( CommandIsValid )
	{
		if(*bpMaxLengthReached)	//have we hit our max?
		{
			KCODE_MODE(FALSE);
			if(Kcode.nBuffer[0]!=NULL)
			{
				if(AffectedDefalt)
					strcat((char *)Kcode.nBuffer,"@");							//MAX hit, terminate string.
				else
					strcat((char *)Kcode.nBuffer,".");	
				strcpy((char *)pucMessage,(char *)Kcode.nBuffer);
			}
			*puiLength=strlen((char *)pucMessage);
			return(ProcessScannedMenuCommand(puiLength,pucMessage));
		}
		else if(IS_KCODE_MODE()==FALSE)
		{
			KCODE_MODE(TRUE);       //set into Kcode mode
		}
	}
	else
	{
		KCODE_MODE(FALSE);	//try sending the menu the buffer to process
		return(ProcessScannedMenuCommand(puiLength,pucMessage));
	}
	return(TRUE);
}

/*-----------------------------------------------------
 * This function will put two /FFs after any AddHexString BLOCK, as an exit function.
 * --------------------------------------------------*/
bool TerminateAddHexStringBlock( const char * cMenuString )
{
	SUB_TAG_TABLE::SUB_TAG_TABLE_ENTRY *SequenceBlockEntry;

	if(ReaderConfiguration->GetSubTagTableEntry(cMenuString, &SequenceBlockEntry))
	{
		if( ReaderConfiguration->CurrentTable == '.' )
		{
			int iLocationFound;
			SUB_TAG_TABLE::SUB_TAG_TABLE_VALUE_ENTRY	*WorkingValue = ReaderConfiguration->WorkingValueForCurrentWorkgroup( SequenceBlockEntry );
			if(WorkingValue->BinaryString->Find((UCHAR *)"\xFF\xFF",2,TRUE,-1,&iLocationFound)<=0)
			{
				WorkingValue->BinaryString->Append("\xFF\xFF");
				ReaderConfiguration->OneOrMoreWorkingValuesUpdated=TRUE;
			}
		}

		if( ( ReaderConfiguration->CurrentTable == '.' ) || ( ReaderConfiguration->CurrentTable == '!' ) )
		{
			int iLocationFound;
			if(SequenceBlockEntry->InstantiatorCopyOfActiveValue.BinaryString->Find((UCHAR *)"\xFF\xFF",2,TRUE,-1,&iLocationFound)<=0)
			{
				SequenceBlockEntry->InstantiatorCopyOfActiveValue.BinaryString->Append("\xFF\xFF");
				ReaderConfiguration->OneOrMoreActiveValuesUpdated=TRUE;
			}
		}
	}
	return(TRUE);
}


/*-----------------4/8/2003 4:49PM------------------
 * This function will put two /FFs after the SEQUENCE BLOCK.
 * --------------------------------------------------*/
bool TerminateSeqBlock(void)
{
	const char cMenuString[] = "025301";

	TerminateAddHexStringBlock( cMenuString );

	return(TRUE);
}

/*-----------------4/8/2003 4:49PM------------------
 * This function will put two /FFs after the NON PREFERRED BLOCK.
 * --------------------------------------------------*/
bool TerminateNonPreferredBlock(void)
{
	const char cMenuString[] = "025402";

	TerminateAddHexStringBlock( cMenuString );

	return(TRUE);
}

/*-----------------4/8/2003 4:49PM------------------
 * This function will put two /FFs after the CODE PREFERRED BLOCK.
 * --------------------------------------------------*/
bool TerminateCodePreferredBlock(void)
{
	const char cMenuString[] = "025403";

	TerminateAddHexStringBlock( cMenuString );

	return(TRUE);
}
//****************End sequence and preferred******************



#if defined ( RFBASE ) || defined ( RFSCAN )
#else // just return false for corded devices
bool ForwardPrefixedCommand(bool, const unsigned char *, unsigned int, bool * pbCommandWasForwarded)
{
	*pbCommandWasForwarded = false;
	return false;
}

#endif // endof #if defined ( RFBASE ) || defined ( RFSCAN )










/*  End of File */
