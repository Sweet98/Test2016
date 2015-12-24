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
// $RCSfile: db_boot/db_syncommandparse.c $
// $Revision: 1.5.3.12 $
// $Date: 2011/11/10 20:43:09EST $
//======================================================================================
/************************************************************************/
/* This module is artwork independent and processor dependent.          */
/************************************************************************/
#include <ctype.h>		/* character typing functions				*/
#include <string.h>

#include "db_SYNCommandParse.h"		// SYN command handler definitions
#include "db_NonMenuCommands.h"
#include "db_chrio.h"
#include "db_sharedtable.h"
#if defined(HI2D_T)
#include "stdio.h"
#endif



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

#define CHUCK 1
bool ServiceSYNCommand( UCHAR ucData )
{
	static char pucBuffer[SIZEOF_HIGHLEVEL_PARSE_BUFFER] ;
	static unsigned int ui=0, uiState=INPUT_RESET__WAITING_FOR_SYN ;
	bool		ReceivedByteWasPartOfASynCommand = false;
	int k=0;

	char		CmdStr1[9]={'M'+3,'A'+3,'T'+3,'C'+3,'M'+3,'D'+3,'?'+3,'.'+3,0+3};
	char		CmdStr2[9]={'M'+4,'A'+4,'T'+4,'C'+4,'M'+4,'D'+4,'\x06'+4,'.'+4,0+4};
	char		CmdStr3[9]={'R'+5,'E'+5,'V'+5,'_'+5,'B'+5,'T'+5,'?'+5,'.'+5,0+5};
	char		CmdStr4[9]={'C'+6,'O'+6,'M'+6,'P'+6,'I'+6,'D'+6,'?'+6,'.'+6,0+6};
	char		CmdStr5[7]={'C'+7,'O'+7,'M'+7,'P'+7,'I'+7,'D'+7,0+7};

	for(k=0;k<9;k++)
	{
		CmdStr1[k] = CmdStr1[k]-3;
	}

	for(k=0;k<9;k++)
	{
		CmdStr2[k] = CmdStr2[k]-4;
	}

	for(k=0;k<9;k++)
	{
		CmdStr3[k] = CmdStr3[k]-5;
	}

	for(k=0;k<9;k++)
	{
		CmdStr4[k] = CmdStr4[k]-6;
	}

	for(k=0;k<7;k++)
	{
		CmdStr5[k] = CmdStr5[k]-7;
	}

	
	ucData =((ucData >= 'a') && (ucData <= 'z')) ? (ucData - 32) : ucData;
	/* ========================================================================
		State INPUT_RESET__WAITING_FOR_SYN :

		Nothing valid has been received - We are waiting for [SYN]
		======================================================================== */

	if( INPUT_RESET__WAITING_FOR_SYN==uiState )
	{
		if ( STX==ucData )
		{
			uiState=INPUT_SYN_RECEIVED__WAITING_FOR_COMMAND_BYTE ;
			ReceivedByteWasPartOfASynCommand = true;
		}
	}

	/* ========================================================================
		State INPUT_SYN_RECEIVED__WAITING_FOR_COMMAND_BYTE :

		A SYN character has been received - Waiting for a valid command byte
		======================================================================== */

	else if( INPUT_SYN_RECEIVED__WAITING_FOR_COMMAND_BYTE==uiState )
	{
		ui=0 ;

		switch(ucData )
		{

		case 'C' :                	/* Auto-baud detection request */
		case 'M' :           	  	/* Menuing symbol to follow */
		case 'N' :                 /* Process non-menu command */
		case BEL :                 /* Beep */
			pucBuffer[ui++]=ucData ;
			uiState=INPUT_COMMAND_BYTE_RECEIVED__WAITING_FOR_COMMAND_DATA ;
		case STX :
			break ;
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
		if( CR!=ucData )
		{
			if( ui==SIZEOF_HIGHLEVEL_PARSE_BUFFER )
			{
				uiState=INPUT_RESET__WAITING_FOR_SYN ;
			}

			else if ( STX==ucData )
				uiState=INPUT_SYN_RECEIVED__WAITING_FOR_COMMAND_BYTE ;

			else
				pucBuffer[ui++]=ucData ;
		}
		else
		{
			uiState=INPUT_RESET__WAITING_FOR_SYN ;
			switch( ((pucBuffer[0] >= 'a') && (pucBuffer[0] <= 'z')) ? (pucBuffer[0] - 32) : pucBuffer[0] )//toupper(pucBuffer[0]) )
			{
			case 'C' :
				{
					// Respond with ACK NAK DEh DFh
					xputstring("\006\025\336\337");
					break;
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
					break ;
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
			uiState=INPUT_RESET__WAITING_FOR_SYN ;
		}
		else if ( STX==ucData )
		{
			uiState=INPUT_SYN_RECEIVED__WAITING_FOR_COMMAND_BYTE ;
		}
		else
		{
			pucBuffer[ui++]=ucData ;
			if( '!'==ucData || '.'==ucData || '&' == ucData)
			{
				pucBuffer[ui] = 0;
				#if CHUCK
				if ( (ui == 8) && (strncmp(CmdStr1,pucBuffer,8)==0) )
				{
					xputstring(CmdStr2);
				}
				else if( (ui == 8) && (strncmp(CmdStr3,pucBuffer,8)==0) )
				{
					xputstring((char *) BOOT_CODE_VERSION_STRING);
					xputstring("\006.");
				}
				else if( (ui == 8) && (strncmp(CmdStr4,pucBuffer,8)==0) )
				{
					xputstring(CmdStr5);
#if defined(HI2D_T)
					char CompId[6];
					sprintf(CompId,"%d\x06.",COMPAT_PROD_ID);
					xputstring(CompId);
#endif
				}
				else
				{
				#endif
					// We received a command, but we don't understand it.  We're not going to do a real parse, but we will add an ENQ character before the
					// table character and remove a query character if one is present.  Note that this won't work for block commands.
					if((pucBuffer[ui - 2] == '?') || (pucBuffer[ui - 2] == '$'))
					{
						pucBuffer[ui - 2] = ENQ;
					}
					else
					{
						pucBuffer[ui - 1] = ENQ;
						pucBuffer[ui++]=ucData ;
						pucBuffer[ui] = 0;
					}
					xputstring(pucBuffer);
				#if CHUCK
				}
				#endif
				uiState=INPUT_RESET__WAITING_FOR_SYN ;
			}
		}

		ReceivedByteWasPartOfASynCommand = true;
	}
	return(ReceivedByteWasPartOfASynCommand) ;
}







