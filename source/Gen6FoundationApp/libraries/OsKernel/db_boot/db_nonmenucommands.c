//======================================================================================
// db_NonMenuCommands.c
//======================================================================================
// $RCSfile: db_boot/db_nonmenucommands.c $
// $Revision: 1.6.2.24 $
// $Date: 2010/04/09 15:35:26EDT $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Description:
//		NonMenuCommands.c contains the dispatch functions for processing non-menu system commands.
//		Currently, we process the following non-menu commands:
//			DEVICETYPE
//				Return the device type flag bytes in the beginning of the code image.
//			NEWAPP
//				Start downloading new application code to the reader.
//
//---------------------------------------------------------------------------

#include <stdlib.h>
#include <string.h>

#include "db_chrio.h"
#include "db_hdw.h"
#include "db_NonMenuCommands.h"
#include "db_sharedtable.h"
#include "db_show.h"     // configuration display message prototypes
#include "db_xmod.h"		// XMODEM transfer definitions
#include "MatrixHeader.h"
#include "db_SYNCommandParse.h"	
#pragma thumb
#ifdef RAMBASEDBOOT
#include "HardwareTest.h"
#include "Full_RAM_Test.h"
#endif

#define MAX_APP_SZ (4*1024*1024)

#ifdef SAFE_FFS_USED
#include "mx25spi.h"
#include "flashdrv.h"
#endif
#include "Gen6ObjectParser.h"
#include "MocfTags.h"
#include "flashapplicationcode.h"
#include "boot_main.h"


extern "C"	void Run_op_code(void) ;
//extern "C"	void MMU_CleanAndInvalidateBothCaches(void);
extern "C"	char MATRIX_BOOT_HEADER[];

// Keep track of whether or not we're in skip header check mode.

int	SkipHeaderCheckOnNextDownloadMode = 0;
static DWORD m_BaudRate = 115200;


// Keep track of last address query.
int DisplayMemoryBlockBaseAddress = 0;

#ifdef SAFE_FFS_USED
//You need to type the non-menu command twice before the flash will be erased
static bool EraseAllFlashQuestion = 0;
#endif


/*----------------------------------------------------------------------------------------
LaunchAppInFlash

  Loads app into ram and runs it.

  Parameters:  none

  Returns:  nothing.
----------------------------------------------------------------------------------------*/
void LaunchAppInFlash(void)
{
	long size;
	BYTE *ApplicationLoadAddress;
	size = ReadApplicationCodeFromSerialFlash(&ApplicationLoadAddress);
	Run_application_code(ApplicationLoadAddress, ApplicationLoadAddress, size);
}

/*----------------------------------------------------------------------------------------
WriteAndCloseFile

  This is a copy of source code found in StartupFlashDisk.cpp.  It looked like we might
  need to include too many files to bring that one in & this function is pretty simple.  

  Parameters:  
  			name[in]:  file name
  			
  			data[in]:  data to write
  			
  			size[in]:  length of data to write

  Returns:  < 0 on error, or num of bytes written (may be less than the requested size)
----------------------------------------------------------------------------------------*/
int WriteAndCloseFile(const char *name, const unsigned char *data, int size)
{
    int WriteStatus =-1;
    F_FILE * f =f_open(name,"r+");
    if (!f)
    {
        f =f_open(name,"w");	// if the file does not exist create it
    }
    if (f)
    {
        WriteStatus = f_write(data,1,size,f);
        if (WriteStatus==size)
        {
            f_seteof(f);
            f_close(f);
        }
    }
    return WriteStatus;
}


/*----------------------------------------------------------------------------------------
MocfIsCompatible

  Searches all Compatible Products Records to ensure they contain the ID.  

  Parameters:  
  			CompatibleProdId[in]:  ID to search for

  Returns:  true if ID was found, false otherwise.
----------------------------------------------------------------------------------------*/
bool MocfIsCompatible(CGen6MocfParser * pParser, DWORD CompatibleProdId)
{
	const CTlv::Header_t * pRecord;
	bool bIsCompatible = false;
	
	if ( pParser->FindRecord(Mocf::tagMocfCompatibleProducts, &pRecord) )
	{
		CMocfCompatProdParser CompatProdParser(pRecord);
		if ( CompatProdParser.FindId(CompatibleProdId) ) bIsCompatible = true;
	}
	
	return bIsCompatible;
}


/*----------------------------------------------------------------------------------------
ProcessMocf

  This will process an MOCF.
  
  Some reverse engineering notes:
	  Legacy code had an option to send an ACK if OK, but there was no code that ever enabled it.  So, it looks
	  like we send a NAK on error (and never an ACK).  The ACK for OK comes out later after we reboot and apply the Menu Settings
	  in the UpdateInformation file (see SaveInterfaceMenuSettings()).  However, since we are not
	  going to have app store itself, we send the ACK here.


  TODO:  Right now we only store the app binary--we need to store Sfotware Version string as well.
   
  Parameters:  
  				pMocf[in]:  pointer to the MOCF.

  				MocfLength[in]:  length of the MOCF.

  				bDoCompatibleCheck[in]:  This tells us to do the compatibility check.  No external customer application can set this to false.

  Returns:  true if false if the MOCF does not contain an Application Firmware.
----------------------------------------------------------------------------------------*/
bool ProcessMocf(const unsigned char * pMocf, int MocfLength, bool bDoCompatibleCheck)
{
	CGen6MocfParser parser((CTlv::Header_t *) pMocf);
	CMocfParser MocfParser((CTlv::Header_t *) pMocf);
	const CTlv::Header_t * pRecord;
	
	
	if ( ! MocfParser.IsValid() ) return false; // We use CMocfParser and not CGen6MocfParser because CGen6MocfParser needs a heap (which we don't have) 
	if ( ! bDoCompatibleCheck )
	{
		if ( ! MocfIsCompatible(&parser, COMPAT_PROD_ID) ) return false;
	}
	xputchar(ACK);
	if ( parser.FindRecord(Gen6::tagAppFirmware, &pRecord) )
	{
		const unsigned char * pFirmwareBin;
		DWORD FwLength;
		pFirmwareBin = (const unsigned char *) pRecord;
		FwLength = sizeof(CTlv::Header_t) + CTlv::GetLength(pRecord);  // (Tag + Length) + ObjectData
		int		WriteStatus=-1;

		WriteStatus = WriteAndCloseFile(FILENAME_MAIN_APP, pFirmwareBin, FwLength);
		if ( WriteStatus == FwLength ) // safe ffs returns number of bytes written
		{	// App stored OK.  Create a file for app firmware:  app will read the data, set teh baud rate and output a string to indicate to the test fixture it can turn power off.
			WriteAndCloseFile(FILENAME_BOOT_UPDATE, (const unsigned char *)&m_BaudRate, sizeof m_BaudRate);
			LaunchAppInFlash();  // for some reason, Test Engineering was seeing multiple 0 bytes come out when we called reset_to_boot_code_hard(), so insetead, we launch the app to avoid this
		}
		else
		{	// worse case is we wrote some, but not all data and the file exists.  I'm not sure this could really happen (doc implies it could), but we'll account for it to be safe.
			f_delete(FILENAME_MAIN_APP);
			return false;
		}
	}
	// we should never get here if things went OK.
	return false;
}








//
//	DownloadApplicationCode
//		The host has requested that we download new applcation code.
//
bool DownloadApplicationCode(bool SkipHeaderCheck)
{
	int	NumberOfBytesTransferred;          /* Number of bytes downloaded from host.     */
	if(xmodem(((BYTE *) (&RAM_RESET)), &NumberOfBytesTransferred,MAX_APP_SZ) == 0) // go try and xmodem data to the right spot
	{
		const BYTE * pMocf = (const BYTE *)&RAM_RESET;
		if ( (pMocf[0] == 'M') && (pMocf[1] == 'O') && (pMocf[2] == 'C') && (pMocf[3] == 'F') )
		{	// It looks like an MOCF.
			if ( ! ProcessMocf(pMocf, NumberOfBytesTransferred, SkipHeaderCheck) ) xputchar(NAK);
		}
		else
		{
			if((SkipHeaderCheck) ||(MatchApplicationType(((char *) (&RAM_RESET)), NumberOfBytesTransferred, ((char *) CACHED_ROM_BASE), FLASH_MAX_BOOT_CODE_LENGTH)))
			{
				xputchar(ACK);
				wait_til_tx_is_empty();
				SIZE_OF_OP_CODE_TO_STORE = NumberOfBytesTransferred;      // non false tells app code it has to save itself, as well as how big the image is!!
				Run_op_code() ;                    // go try and run code @ c0000000
			}
			else
			{
				//xputstring("\x15""Device Type Error\r\n");
			}
		}
	}
	// xmodem failed. Display a message and return.
	else
	{
		//xputstring("Download App Stopped\r\n");
	}
	// should never ever ever get here... stack is off a little but who
	//... really cares...
	return(false);
}

//
//	CheckForNonMenuCommand
//		Check the command string for the presence of the non-menu command sent by the caller.
//		If present, adjust the command string pointer and length past the non-menu command
//		string.  The comparison is done in a case sensitive manner.
//
bool CheckForNonMenuCommand(char **CommandString, UINT *CommandStringLength,
                            char *CommandToCheckFor)
{
	int	LengthOfCommandToCheckFor = strlen(CommandToCheckFor);
	// If we find the non-menu command in the command string, adjust the command string pointer
	// and length.
	if( memcmp( *CommandString, CommandToCheckFor, LengthOfCommandToCheckFor ) == 0 )
	{
		*CommandString += LengthOfCommandToCheckFor;
		*CommandStringLength -= LengthOfCommandToCheckFor;
		return true;
	}
	else
	{
		return false;
	}
} // CheckForNonMenuCommand


//
//	ProcessNonMenuCommands
//		We've received a <SYN>Ncmd<CR> to indicate that we're supposed to process a
//		non-menu system command.  The following is a list of non-menu commands:
//
//			DEVICETYPE
//				Return the device type flag bytes in the beginning of the code image.
//			NEWAPP
//				Start downloading new application code to the reader.
//			NEWBOOT
//				Start downloading new boot code to the reader.  Note that SETBOOTCODEDOWNLOADMODE
//				must have been sent first.
//			SETBOOTCODEDOWNLOADMODE
//				Enables boot code download mode so that the next code download may be boot code.
//
void ProcessNonMenuCommands(char *CommandString, UINT CommandStringLength)
{
	int			i,k;
	char        CmdStr1[11]={'D'+3,'E'+3,'V'+3,'I'+3,'C'+3,'E'+3,'T'+3,'Y'+3,'P'+3,'E'+3,0+3};
	char        CmdStr2[10]={'B'+4,'O'+4,'O'+4,'T'+4,','+4,'\x30'+4,'\x30'+4,'\x30'+4,'\r'+4,0+4};
	char        CmdStr3[24]={'S'+5,'K'+5,'I'+5,'P'+5,'H'+5,'E'+5,'A'+5,'D'+5,'E'+5,'R'+5,'C'+5,'H'+5,'E'+5,'C'+5,'K'+5,'!'+5,'@'+5,'#'+5,'$'+5,'%'+5,'^'+5,'&'+5,'*'+5,0+5};
	char        CmdStr4[7]={'N'+6,'E'+6,'W'+6,'A'+6,'P'+6,'P'+6,0+6};
	char        CmdStr5[6]={'R'+7,'E'+7,'S'+7,'E'+7,'T'+7,0+7};
	char        CmdStr6[8]={'V'+8,'E'+8,'R'+8,'S'+8,'I'+8,'O'+8,'N'+8,0+8};
	char        CmdStr7[5]={'B'+9,'A'+9,'U'+9,'D'+9,0+9};
	char        CmdStr8[5]={'I'+10,'D'+10,'S'+10,'K'+10,0+10};
	char        CmdStr9[16]={'r'+11,'e'+11,'s'+11,'p'+11,'o'+11,'n'+11,'s'+11,'e'+11,' '+11,'I'+11,'D'+11,'S'+11,'K'+11,'\r'+11,'\n'+11,0+11};
	char        CmdStr10[4]={'E'+12,'F'+12,'L'+12,0+12};
	char        CmdStr11[8]={'G'+2,'o'+2,'o'+2,'d'+2,'b'+2,'y'+2,'e'+2,0+2};
	char        CmdStr12[36]={'S'+1,'e'+1,'n'+1,'d'+1,' '+1,'c'+1,'o'+1,'m'+1,'m'+1,'a'+1,'n'+1,'d'+1,' '+1,'a'+1,'g'+1,'a'+1,'i'+1,'n'+1,' '+1,'t'+1,'o'+1,' '+1,'E'+1,'r'+1,'a'+1,'s'+1,'e'+1,' '+1,'F'+1,'l'+1,'a'+1,'s'+1,'h'+1,'\r'+1,'\n'+1,0+1};
	char        CmdStr13[3]={'D'+13,'M'+13,0+13};

	for(k=0;k<11;k++)
	{
		CmdStr1[k] = CmdStr1[k]-3;
	}

	for(k=0;k<10;k++)
	{
		CmdStr2[k] = CmdStr2[k]-4;
	}

	for(k=0;k<24;k++)
	{
		CmdStr3[k] = CmdStr3[k]-5;
	}

	for(k=0;k<7;k++)
	{
		CmdStr4[k] = CmdStr4[k]-6;
	}

	for(k=0;k<6;k++)
	{
		CmdStr5[k] = CmdStr5[k]-7;
	}

	for(k=0;k<8;k++)
	{
		CmdStr6[k] = CmdStr6[k]-8;
	}

	for(k=0;k<5;k++)
	{
		CmdStr7[k] = CmdStr7[k]-9;
	}

	for(k=0;k<5;k++)
	{
		CmdStr8[k] = CmdStr8[k]-10;
	}

	for(k=0;k<16;k++)
	{
		CmdStr9[k] = CmdStr9[k]-11;
	}

	for(k=0;k<4;k++)
	{
		CmdStr10[k] = CmdStr10[k]-12;
	}

	for(k=0;k<8;k++)
	{
		CmdStr11[k] = CmdStr11[k]-2;
	}

	for(k=0;k<36;k++)
	{
		CmdStr12[k] = CmdStr12[k]-1;
	}

	for(k=0;k<3;k++)
	{
		CmdStr13[k] = CmdStr13[k]-13;
	}
	// If the command is too long, WOOF!
	if((CommandStringLength > MAXIMUM_SIZE_OF_NON_MENU_COMMAND) || (CommandStringLength <2))
	{
		xputstring("\025");
		return;
	}
	// Get rid of the N,
	// We'd better find a comma to mark the end of the terminal ID (even if there was none).
	// Process the rest of the command if we haven't experienced an error.
	while(CommandStringLength>0 )
	{
		CommandStringLength--;
		char c = *CommandString++;
		if (c==',')
		{
			break;
		}
	}
	if(CommandStringLength)
	{
		if(CheckForNonMenuCommand(&CommandString, &CommandStringLength, CmdStr1))
		{	// Technically, DEVICETYPE no longer exists, but apps use the first characters to determine if they are talking to app or boot.  To be safe, we'll return a number as well--we'll use 0 to ensure no Gen5 app thinks it can download to us.
			xputstring(CmdStr2);
		}
		// Turn on skip header check mode.
		else if(CheckForNonMenuCommand(&CommandString, &CommandStringLength, CmdStr3))
		{
			SkipHeaderCheckOnNextDownloadMode = true;
		}
		// New application
		else if(CheckForNonMenuCommand(&CommandString, &CommandStringLength, CmdStr4))
		{
			DownloadApplicationCode((SkipHeaderCheckOnNextDownloadMode > 0) ? true : false);
			SkipHeaderCheckOnNextDownloadMode = 0;
		}
		else
		{
			SkipHeaderCheckOnNextDownloadMode=false; // these commands reset skip header check
			// Reset to boot code.
			if(CheckForNonMenuCommand(&CommandString, &CommandStringLength, CmdStr5))
			{
				reset_to_boot_code_hard();
			}
			// Dump version information.
			else if(CheckForNonMenuCommand(&CommandString, &CommandStringLength, CmdStr6))
			{
				show_system();
			}

			// Test baud rate.
			else if(CheckForNonMenuCommand(&CommandString, &CommandStringLength, CmdStr7))
			{
				unsigned int rate = 0;
				while (*CommandString>='0' && *CommandString<='9')
				{
					rate = rate*10 + (*CommandString++) - '0';
				}
				m_BaudRate = rate;
				SetBaudRate(rate);
			}
#ifdef SAFE_FFS_USED
			else if(CheckForNonMenuCommand(&CommandString, &CommandStringLength, CmdStr8))
			{
				//F_FIND find;
   				// char buf[60];
   				// F_SPACE space;
				xputstring(CmdStr9);
   				 //if (!f_getfreespace(f_getdrive(),&space))
   				 //{
        				//sprintf(buf,"%d total,%d free,%d used\r\n",space.total,space.free,space.used);
        				//xputstring(buf);//Ken TODO
    				//}
			}
			//Send the command twice to Erase All Flash
			else if(CheckForNonMenuCommand(&CommandString, &CommandStringLength, CmdStr10))
			{
				if(EraseAllFlashQuestion)
				{
					SpiEraseAllFlash();
					xputstring(CmdStr11);
				}
				else
				{
					EraseAllFlashQuestion = 1;
					xputstring(CmdStr12);
				}
			}
#endif			
			// Dump a memory block.
			else if(CheckForNonMenuCommand(&CommandString, &CommandStringLength, CmdStr13))
			{
				if((CommandStringLength > 0) && (CommandStringLength < 9))
				{
					DisplayMemoryBlockBaseAddress = 0;
					while(CommandStringLength)
					{
						DisplayMemoryBlockBaseAddress <<=4;
						int temp = (*CommandString++) &0xdf;
						DisplayMemoryBlockBaseAddress +=((temp >= 'A') && (temp <= 'F')) ?
						(temp- 'A' + 10) : (*CommandString - '0');
						CommandString++;
						CommandStringLength--;
					}
				}
				DisplayMemoryBlockBaseAddress &=0xfffffff0;		// start at /16 address
				for (i = 0;    i < 256 ; i++)
				{
					if ((((int) DisplayMemoryBlockBaseAddress) % 16) == 0)
					{
						xputstring("\r\n");
						xputhex(((int) DisplayMemoryBlockBaseAddress), 8);
						xputstring(" -");
					}
					xputstring(" ");
					xputhex(*((char *)DisplayMemoryBlockBaseAddress), 2);
					++DisplayMemoryBlockBaseAddress;
				}
				xputstring ("\r\n") ;
			}
			// Goto app running 232 mode if the app is recent enough to recognize the request.
			else if(CheckForNonMenuCommand(&CommandString, &CommandStringLength, "232"))
			{
				BOOT_TO_APP_REQUEST = REQUEST_232;

				RESET_WATCHDOG_TIMER();   /*One final kick before we run application */
			}

			// Unrecognized non-menu command; WOOF!!
			else
			{
				xputstring("\025");
			}
		}	//endof else for commands reset skip header check
	}	//endof if(Success)
	return;
      } // ProcessNonMenuCommands



