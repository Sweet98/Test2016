/************************************************************************/
//
// HHP Matrix embedded firmware
//
//  ESC command handler
//  Date:       6/22/04
//  Tools:      ADS 1.2
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: ESCCommandParse.cpp $
// $Revision: 1.1 $
// $Date: 2009/04/06 14:42:44EDT $
//======================================================================================
/************************************************************************/
/* This module is artwork independent and processor dependent.          */
/************************************************************************/

#include "beep.h"
#include "ESCCommandParse.h"		// ESC command handler definitions
#include "menu.h"
#include "IoMan.h"
#include "ReaderConfiguration.h"
#include "HString.h"
//#include "Language.h"


/* ========================================================================
	ServiceHighLevelInput()
 
	This function implements a state machine to parse incoming high level
	data that may contain commands for the scanner. Usually, this data
	will only come from the serial port. However, future designs may also
	accept input from other sources (i.e. keyboard)
	======================================================================== */

#define INPUT_RESET__WAITING_FOR_WORKGROUP						0
#define INPUT_WORKGROUP_RECEIVED___WAITING_FOR_ESC				1
#define INPUT_ESC_RECEIVED__WAITING_FOR_COMMAND					2
#define INPUT_COMMAND_RECEIVED__WAITING_FOR_COMMA_OR_ESC		3

// Constructor
ESC_COMMAND_PARSE::ESC_COMMAND_PARSE( int BufferSize )
{
	hstrBuffer = new HSTRING( BufferSize, true );
	Command = new HSTRING( 20, true );
	uiState = INPUT_RESET__WAITING_FOR_WORKGROUP ;
	return;
}




// Destructor
ESC_COMMAND_PARSE::~ESC_COMMAND_PARSE( void )
{
	delete hstrBuffer;
	delete Command;
	return;
}




bool ESC_COMMAND_PARSE::ServiceESCCommand( UCHAR ucData )
{
	bool		ReceivedByteWasPartOfAnESCCommand = false;


	/* ========================================================================
		State INPUT_RESET__WAITING_FOR_WORKGROUP :

		Nothing valid has been received - We are waiting for the first byte of
		a workgroup number
		======================================================================== */

	if( INPUT_RESET__WAITING_FOR_WORKGROUP==uiState )
	{
		if ( ucData == ESC )
		{
			// Process all workgroups
			Workgroup = -1;
			uiState = INPUT_ESC_RECEIVED__WAITING_FOR_COMMAND;
		}
		else
		{
			hstrBuffer->CopyChar( ucData );
			if ( hstrBuffer->RangeCheck( '0', '9' ))
			{
				uiState=INPUT_WORKGROUP_RECEIVED___WAITING_FOR_ESC ;
				ReceivedByteWasPartOfAnESCCommand = true;
			}
		}

	}



	/* ========================================================================
		State INPUT_WORKGROUP_RECEIVED___WAITING_FOR_ESC :

		We've received the workgroup - We are waiting for an ESC until we get
		an ESC character or we get a non-numeric workgroup number
		======================================================================== */

	else if( INPUT_WORKGROUP_RECEIVED___WAITING_FOR_ESC==uiState )
	{
		if( ucData == ESC )
		{
			hstrBuffer->GetNumber( &Workgroup );
			if(( Workgroup >= 0 ) && ( Workgroup < ReaderConfiguration->NumberOfAvailableWorkgroups() ))
			{
				uiState = INPUT_ESC_RECEIVED__WAITING_FOR_COMMAND;
			}
			else
			{
				uiState = INPUT_RESET__WAITING_FOR_WORKGROUP;

			}
		}
		else
		{
			hstrBuffer->AppendChar( ucData );
			if ( !hstrBuffer->RangeCheck( '0', '9' ))
			{
				uiState=INPUT_RESET__WAITING_FOR_WORKGROUP ;

			}
		}

		ReceivedByteWasPartOfAnESCCommand = true;
	}



	/* ========================================================================
		State INPUT_ESC_RECEIVED__WAITING_FOR_COMMAND :

		We've received the workgroup and ESC - We are waiting for the first command
		byte.
		======================================================================== */

	else if( INPUT_ESC_RECEIVED__WAITING_FOR_COMMAND==uiState )
	{
		if(( ucData != ',' ) && ( ucData != ESC ))
		{
			Command->CopyChar( ucData );
			uiState=INPUT_COMMAND_RECEIVED__WAITING_FOR_COMMA_OR_ESC ;
		}
		else
		{
			// Needed at least one command byte; WOOF.
			uiState=INPUT_RESET__WAITING_FOR_WORKGROUP ;

		}

		ReceivedByteWasPartOfAnESCCommand = true;
	}



	/* ========================================================================
		State INPUT_COMMAND_RECEIVED__WAITING_FOR_COMMA_OR_ESC :

		We've received at least one command byte - We are waiting for the comma
		to terminate the command.
		======================================================================== */

	else if( INPUT_COMMAND_RECEIVED__WAITING_FOR_COMMA_OR_ESC==uiState )
	{
		if((( ucData == ',' ) || ( ucData == ESC )))
		{

			ExecuteHostAckBeep(Command->Char[0]); // executes host ack beeps a,b, and 1-8 returns false if not one of these
			// If this command was terminated with a comma, quit.  Otherwise, it must have been terminated with an ESC, so we need to go get
			// the next command.
			if( ucData == ',' )
			{
				uiState=INPUT_RESET__WAITING_FOR_WORKGROUP ;
			}
			else
			{
				uiState = INPUT_ESC_RECEIVED__WAITING_FOR_COMMAND;
			}
		}
		else
		{
			Command->AppendChar( ucData );
		}

		ReceivedByteWasPartOfAnESCCommand = true;
	}
	return(ReceivedByteWasPartOfAnESCCommand) ;
}


ESC_COMMAND_PARSE *ESCCommandParser=NULL;




