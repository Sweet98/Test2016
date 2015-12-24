/*=================================================================================
  class: CWedgeLabel

//=================================================================================
   $Source: interfaces_wedge/wedgelabel.cpp $
   $Date: 2011/08/30 11:14:04EDT $
   $Revision: 1.28 $



//=================================================================================*/

#include "stdInclude.h"
#include "CommonDriver.h"
#include "WedgeLabel.h"
#include "BarcodeData.h"
#include "barcodefifo.h"
#include "WedgeHal.h"
#include "DataEdit.h"
#include "Thread.h"
#include "stdio.h"
#include "string.h"
#include "db_chrio.h"
#include "ErrorDefs.h"
#include "Timeout.h"
#include "Timer3_Driver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const CKey Lastkey;

CWedgeLabel::CWedgeLabel(CThreadedDriver *pDriver, const CWedgeTerminal *pTerminal)
:baseCWedgeLabel(pDriver, pTerminal)
{
}

void CWedgeLabel::Add_CloseLabelCommands(void)
{
	HandleModifierBreaks(&Lastkey); /* use keynumber 0 to close */
}

void CWedgeLabel::Add_PressCapsKey(void)
{
	keynum_t keynum = GetModifierKeyNumber(CWedgeTerminal::INDEX_CAPS_LOCK);
	DoMake(keynum, EX_NO);
	DoBreak(keynum, EX_NO, GetMediumDelay());
	AddMediumDelayCommand();
}

void CWedgeLabel::Add_PressNumLockKey(void)
{
	keynum_t keynum = GetModifierKeyNumber(CWedgeTerminal::INDEX_NUM_LOCK);
	DoMake(keynum, EX_NO);
	DoBreak(keynum, EX_NO, GetMediumDelay());
	AddMediumDelayCommand();
}

void CWedgeLabel::Add_MakeShiftKey(void)
{
	keynum_t keynum = GetModifierKeyNumber(CWedgeTerminal::INDEX_LEFT_SHIFT);
	if (keynum > 0)
	{
		DoMake(keynum, EX_MODIFIER);
		AddMediumDelayCommand();
		StoreModifierMake(MO_ShiftLeft);			/* Remember later that we pressed the shift key */
	}
}

////////////////////////////////////////////////////////////////


#define REPEATWAIT	25						/* milli seconds */
#define CAPSWAIT	200/REPEATWAIT			/* milli seconds */

void CWedgeLabel::WaitUntilLEDsValid(void)
{
	int wait;
	for(wait=0; wait<CAPSWAIT; wait++)	/* wait until the led info is valid */
	{
		if (WdgIsLED_Valid())
			break;
		MT_Driver_Sleep(REPEATWAIT*1000);
	}
}

void CWedgeLabel::TurnCapsLockOff(void)
{
	Add_PressCapsKey(); 							/* Now get the scancodes for the caps key and send them */
	Add_MakeShiftKey();							// Some European countries clear the lock via the shift key
	SendCommands();								/* so press the shift key as well. */
}

void CWedgeLabel::OpenStyleShiftLock(void)
{
	WdgStoreKeyEmu(1);							// turn keyboard emulation on, so we can capture commands from host

	WdgStoreCapsLockSave(1);					/* Save the current state of the lock (we assume it's on in this style) */
	WdgStoreLED_Valid(0);						/* Invalidate the LED info */
	TurnCapsLockOff();

	WaitUntilLEDsValid();
	if (!IsAutoKeyEmu())                	// turn keyboard emulation off
		WdgStoreKeyEmu(0);
}

//ewr-04771
// OpenKBDALTstyle(): Test if KBDALT mode is enabled.
void CWedgeLabel::OpenKBDALTstyle(void)
{
	if ( IsTransAltNp() )
	{
		MT_Driver_Sleep(3000);			// TODO: Wait for Numlock info is updated. This is a work-around solution
										// for fix the KBDALT mode problem: Auto NumLock not work correctly at fisrt 
										// time after setting KBDALT3.  
		OpenStyleKbdAltNum();
	}
}

// OpenStyleKbdAltNum(): In KBDALT mode ensure Num Lock is ON.
void CWedgeLabel::OpenStyleKbdAltNum(void)
{

	if (!WdgIsNumLock())				/* if Num Lock is now on, it was off before */
	{
		WdgStoreKeyEmu(1);				/* temporarily turn on the keyboard emulation mode */
		WdgStoreLED_Valid(0);     		/* invalidate the led info */
		WdgStoreNumLockSave(0);			/* save OFF */
		Add_PressNumLockKey();	    	/* undo what we've changed */
		SendCommands();
	}
	else
	{
		WdgStoreNumLockSave(1);
	}

	// Now wait until the LED info is valid again.
	// This is necessary to ensure the keyboards LED's are in sync with the
	// internal flags of the BIOS in the PC.
	WaitUntilLEDsValid();

	if (!IsAutoKeyEmu())				/* turn off the keyboard emulation mode */
		WdgStoreKeyEmu(0);
}
//end ewr-04771


void CWedgeLabel::OpenStyleAutoCaps(void)
{
	WdgStoreKeyEmu(1);				/* temporarily turn on the keyboard emulation mode */

	WdgStoreLED_Valid(0);     		/* invalidate the led info */
	Add_PressCapsKey();				/* press the caps key */
	SendCommands();
	WaitUntilLEDsValid();

	if (WdgIsCapsLock())				/* if caps is now on, it was off before */
	{
		WdgStoreLED_Valid(0);		/* invalidate the led info */
		WdgStoreCapsLockSave(0);	/* save OFF */
		Add_PressCapsKey();			/* undo what we've changed */
		SendCommands();
	}
	else
	{
		WdgStoreCapsLockSave(1);
	}

	// Now wait until the LED info is valid again.
	// This is necessary to ensure the keyboards LED's are in sync with the
	// internal flags of the BIOS in the PC.
	WaitUntilLEDsValid();

	if (!IsAutoKeyEmu())				/* turn off the keyboard emulation mode */
		WdgStoreKeyEmu(0);
}

// This style uses the NumLock key to get the CAPS LED info
// Good for European keyboards where the Caps kea does not toggle the lock.
void CWedgeLabel::OpenStyleAutoCapsViaNumLock(void)
{
	WdgStoreKeyEmu(1);					/* temporarily turn on the keyboard emulation mode */

	WdgStoreLED_Valid(0);     			/* invalidate the led info */
	Add_PressNumLockKey();				/* press the NumLock key */
	SendCommands();
	WaitUntilLEDsValid();

	if (WdgIsCapsLock())					/* if caps is now on, store it */
	{
		WdgStoreCapsLockSave(1);		/* save ON */
		Add_PressNumLockKey();			/* get the NumLock key */
		SendCommands();					/* resend the NumLock key to toggle back (still in buffer) */
		WdgStoreLED_Valid(0);     		/* invalidate the led info */
		TurnCapsLockOff();
	}
	else
	{
		WdgStoreCapsLockSave(0);		/* save OFF */
		WdgStoreLED_Valid(0);     		/* invalidate the led info */
		Add_PressNumLockKey();			/* get the NumLock key */
		SendCommands();					/* resend the NumLock key to toggle back (still in buffer) */
	}
	// Now wait until the LED info is valid again.
	// This is necessary to ensure the keyboards LED's are in sync with the
	// internal flags of the BIOS in the PC.
	WaitUntilLEDsValid();

	if (!IsAutoKeyEmu())					/* turn off the keyboard emulation mode */
		WdgStoreKeyEmu(0);
}

void CWedgeLabel::OpenStyleKeyEmu(void)
{
	if ( !(WdgIsLED_Valid()) )						/* Need to get state? */
	{														/* Should only happen after powerup or menuing */
		if (GetModifierKeyNumber(CWedgeTerminal::INDEX_NUM_LOCK))
		{													/* Can only be done if terminal has NumLock */
			Add_PressNumLockKey();					/* press the NumLock key (toggles in all countries) */
			SendCommands();
			MT_Driver_Sleep(GetMediumDelay());	/* some delay between keystrokes */
			Add_PressNumLockKey();					/* press the NumLock key again to toggle back */
			SendCommands();
			WaitUntilLEDsValid();
		}
	}

	if (WdgIsCapsLock())
	{														/* If CapsLock is set, release the lock temporarily */
		WdgStoreCapsLockSave(1);					/* Save the current state of the lock */
		TurnCapsLockOff();
	}
	else
	{
		WdgStoreCapsLockSave(0);
	}
}

void CWedgeLabel::OpenLockStyles(void)
{
	if ( IsKbdStyleAutoCapsLock() )
	{
		OpenStyleAutoCaps();
	}
	else if ( IsKbdStyleAutoCapsLockViaNumLock() )
	{
		OpenStyleAutoCapsViaNumLock();
	}
	else if ( IsKbdStyleShiftLock() )
	{
		OpenStyleShiftLock();
	}
	else if ( IsKbdStyleKeyEmu() )
	{
		OpenStyleKeyEmu();
	}
}


void CWedgeLabel::CloseLockStyles(void)
{
	if (WdgIsCapsLockSave())
	{
		Add_PressCapsKey();
		SendCommands();
	}

	//ewr-04771
	if( IsTransAltNp() && !WdgIsNumLockSave() )
	{
		Add_PressNumLockKey();
		SendCommands();
	}
	//end ewr-04771
}

/* Called after powerup before we send data to the terminal */
void CWedgeLabel::OpenKeyEmu(void)
{
	if (WdgIsKeyEmu())
	{
		OpenKeyboardEmulation();
	}
}


/* Called regularly to check for data comming from the terminal */
bool CWedgeLabel::CheckKeyEmu(void)
{
	if (WdgIsKeyEmu())
	{
		return IsOK(CheckKeyboardEmulation());
	}
	return false;
}


/*
	Calls the terminal specific bitbang function.
	Also handles DirectConnect communication requests.
*/
void CWedgeLabel::BitBangWrite(unsigned int data, unsigned int extrainfo)
{

	MatrixError_t ret=ERR_PASS;
	m_LastSend = data;

	if (WdgIsKeyEmu())
	{
		extrainfo |= EX_KEYEMU;
	}

	do
	{
		CheckKeyEmu();
		if (!IsOK(ret))
		{
			MT_Driver_Sleep(10000);						/* No, just wait with a fixed delay here */
		}				/* Perhaps the terminal wants to talk ?*/
		ret = BitBangOut(data, extrainfo);
	}while(!IsOK(ret));
}

/* These are important simulation tools, do not delete them. */
#ifdef _SIMULATION
#define BIT_BANG_COMMAND(param1, param2)	GeneralBitBang(param1, param2)
#define END_COMMAND()							WriteEndMsg()
#define BAD_COMMAND()							WriteBadMsg()
#define WRITE_ANSI(ansi)						WriteAnsiChar(ansi)
#else
#define BIT_BANG_COMMAND(param1, param2)	BitBangWrite(param1, param2)
#define END_COMMAND()							/* a do nothing here */
#define BAD_COMMAND()							/* a do nothing here */
#define WRITE_ANSI(ansi)						/* a do nothing here */
#endif

void CWedgeLabel::DelayCommand(ULONG uSec)
{
	NotifyOutTermDelay(uSec);
#ifdef _SIMULATION
	WriteDelay(uSec);
#else
	MT_Driver_Sleep(uSec);
#endif
}

/*
	This is the reading side of the command buffer.
	The translation engine translates characters into
	a series of commands. Here we read and dispatch them.
*/
void CWedgeLabel::SendCommands(void)
{
	drvparamw_t param1;
	drvparamw_t param2;
	m_CmdBuffer.AddEndCommand();	         // ensure we'll find an end
	m_CmdBuffer.PrepareRead(); 				// now reset the pointer for the read process
	bool	Done=FALSE;
	while(!Done)
	{
		switch(m_CmdBuffer.GetCommand())
		{
		case CWedgeCmd::CMD_WAITMIRCO:
			DelayCommand(m_CmdBuffer.GetParameterW());
			break;
		case CWedgeCmd::CMD_WAITMIRCO_LONG:
			DelayCommand(m_CmdBuffer.GetParameterL());
			break;
		case CWedgeCmd::CMD_SCANCODE:
			param1 = m_CmdBuffer.GetParameterW();
			param2 = m_CmdBuffer.GetParameterW();
			BIT_BANG_COMMAND(param1, param2);
			break;
		case CWedgeCmd::CMD_NOP:
			break;	/* no parameters */
		case CWedgeCmd::CMD_END:
			Done = TRUE;
			END_COMMAND();
			break;
/*		case CMD_:
			break; */
		/* ^^^ Add new commands here ^^^ */
		default:
			/*  we better abort this, we are out of sync. */
			Done = TRUE;
			BAD_COMMAND();
			break;
		}
		//if(count++>3)
			//break;
	}
	m_CmdBuffer.PrepareWrite();	// be ready for the next character
}

/* Moved WEDGE_ESCAPE_CHARACTER and related #defines to dataedit.h */

/*
 *	This is to handle escape sequences:
 * cmd is the command part of the sequence
 *
 * param1 depends on the command and can be either a keynumber, scancode or a delay time in MSec.
 *
 */
void CWedgeLabel::TranslateEscSequence(tichar_t cmd, tichar_t param1)
{
	keynum_t keynum = param1&0xff;
	modifiers_t modifier = param1>>8;
	const CKey Key(keynum, modifier);

	switch (cmd)
	{
	case SUB_KEY_NUMBER:													// make and break
		HandleModifier(&Key);
		DoMake(keynum, EX_NO);
		DoBreak(keynum, EX_NO, GetMediumDelay());
		AddMediumDelayCommand();
		break;
	case SUB_KEY_NUMBER_MAKE:											// make only
		HandleModifier(&Key);
		DoMake(keynum, EX_NO);
		AddMediumDelayCommand();
		break;
	case SUB_KEY_NUMBER_BREAK:											// break only
		HandleModifier(&Key);
		DoBreak(keynum, EX_NO, GetMediumDelay());
		AddMediumDelayCommand();
		break;
	case SUB_SCANCODE:													// single byte
		HandleModifier(&Key);
		m_CmdBuffer.AddScancode(keynum, EX_OTHER);				// keynum is used to store the byte
		AddMediumDelayCommand();
		break;

	case SUB_DELAY_TIME:													// wait x mSec
		m_CmdBuffer.AddDelayCommand(param1*1000L);
		break;
	case SUB_CTRLASCII_MODE:
		switch(param1)
		{
         case ENDOF_FIX :
            TurnOnControlAscii();
            InitializeCtrlASCIIKeyLookup();
            break;
         case STARTOF_FIX :
            TurnOffControlAscii();
            InitializeCtrlASCIIKeyLookup();
			break;
		default:
			break;
		}
		break;
	default:
		Translate(cmd);		               // no known sequence, threat as regular text
		Translate(param1);
		break;
	}

	m_CmdBuffer.AddEndCommand();
}

/*
 * Here we check for an escape sequence and handle it.
 */
void CWedgeLabel::CheckEscSequence(CBarcodeData *pBarcode, size_t &Position)
{
	tichar_t cmd;
	tichar_t param1;

	// read the sequence
	Position = pBarcode->ReadNextTotalData(Position, cmd);
	if(Position == 0)
	{
		Translate(WEDGE_ESCAPE_CHARACTER);	// threat one escape at the end as a single character
		return;	// done
	}
	if(cmd == WEDGE_ESCAPE_CHARACTER)
	{
		Translate(cmd);		               // threat two escape as a single character
	}
	// now proceed with the escape sequence
	Position = pBarcode->ReadNextTotalData(Position, param1);
	if(Position == 0)
	{
		Translate(cmd);		               // threat aborted sequence as single characters
		return;	// done
	}

	TranslateEscSequence(cmd, param1);
}

void CWedgeLabel::InterfaceDelay(CBarcodeData *pBarcode, size_t &Position)
{
	tichar_t delay;

	// read delay count
	Position = pBarcode->ReadNextTotalData(Position, delay);
	if(Position == 0)
	{
		Translate(DELAY_ESCAPE_CHARACTER);
		return;	// can't read delay counter
	}
	// now proceed delay
	m_CmdBuffer.AddDelayCommand((long)delay*1000L);
	m_CmdBuffer.AddEndCommand();
}

/*
	Here we send the contents of a barcode label.
	Depending on the keyboard styles there are different
	actions before and after the label has been sent.
	The very last action is to produce the Inter-Message delay.
*/
int CWedgeLabel::SendLabel(void)
{
	tichar_t ansi;

	CBarcodeData *pBarcode = m_pDriver->GetBarcodeToSend();
	if (!pBarcode)
		return 0;
	
	size_t Position = pBarcode->BeginnReadingTotalData();
	m_InUse = TRUE;		/* Avoid overrun in reopen, we need to wait there until all data are out. */

	m_CmdBuffer.PrepareWrite();	// be ready for the first character

	/* handle keyboard switch etc. */
	if (IsAutoKeyEmu())
		WdgStoreKeyEmu(1);
	OpenLabel();
	OpenLockStyles();

	//ewr-04771
	OpenKBDALTstyle();

	while (true)
	{	
		Position = pBarcode->ReadNextTotalData(Position, ansi);
		if(Position == 0)
		{
			break;	// done
		}
		else
		{
			WRITE_ANSI(ansi);		/* testing aid */
			#ifdef WEDGE_ESC_SEQUENCES_SUPPORTED
			if (ansi == WEDGE_ESCAPE_CHARACTER)
				CheckEscSequence(pBarcode, Position);
			else
			{
			#endif
			if (ansi == DELAY_ESCAPE_CHARACTER)
				InterfaceDelay(pBarcode, Position);
			else
				Translate(ansi);
			#ifdef WEDGE_ESC_SEQUENCES_SUPPORTED
			}
			#endif
			SendCommands();
		}
	};
	
	Add_CloseLabelCommands();
	SendCommands();

	
	CloseLockStyles();

	/* handle keyboard switch etc. */
	FinalCloseLabel();

	if (IsAutoKeyEmu())
	{
		WdgStoreKeyEmu(0);
		EnableKeyboard ();
	}
	m_InUse = FALSE;
	MT_Driver_Sleep(GetInterMessageDelay());
	m_pDriver->SentBarcode();
	return 0;
}


/*
 This function runs as a second thread of execution.
 It's the wedge drivers output loop.
*/
void CWedgeLabel::SendLabelThread(void)
{
	while(!WdgIsConfigurationValid())
	{
		MT_Driver_Sleep(5000);	/* Wait until the reopen has configured the driver ! */
	}

	OpenKeyEmu();		/* Power up init for keyboard emulation modes */

	while (TRUE)			/* This is the drivers output loop */
	{
		CheckKeyEmu();		/* Does nothing if not keyboard emulation mode */

		if (WdgIsHigLevelOutputDisabled())		/* did somebody turn off the output via the wedge ? */
		{
			MT_Driver_Sleep(3000);							/* Yes, just wait with a fixed delay here */
		}
		else												/* else send the label if any data */
		{
			if (m_pDriver->AnyDataToSend())		/* Any characters to send? */
			{
				SendLabel();					/* Yes, send them */
				MT_Driver_Sleep(10000);						/* No, just wait with a fixed delay here */

			}
			else
			{
					
				MT_Driver_Sleep(10000);						/* No, just wait with a fixed delay here */
			}
		}

	}
}

/*
 * This Delay returns if the is a keyboard communication request
 */
void CWedgeLabel::MT_Driver_Sleep(long uSec)
{
    if(uSec == 0x0)
    {
        return;
    }
    
	long mSec =(uSec+500)/1000;
	if (mSec)
	{
		while (mSec)
		{
			if (CheckKeyEmu())
				return; /* done waiting */
			int dly =(mSec>3)?3:mSec;
			mSec-=dly;
			if (dly)
				MT_Delay(dly);
		}
	}
	else
	{
		  Timer3_MicroSecondDelay(uSec);
	}
}





