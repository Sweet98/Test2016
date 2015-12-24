/*=================================================================================
  IBM Family bitbanger

//=================================================================================
   $Source: interfaces_wedge_hw/bbibm.cpp $
   $Date: 2010/02/03 02:43:17EST $
   $Revision: 1.10 $


//=================================================================================*/

#include "stdInclude.h"
#include "product.h"
#include "wdgtype.h"
#include "BbIbm.h"
#include "WedgeHal.h"
#include "stdio.h"
#include "thread.h"
#include "db_chrio.h"
//#include "HwTrace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBitBangIBM::CBitBangIBM(void) : CBitBanger(100)
{
}

void CBitBangIBM::PulseClockTerm(void)
{
	TermClk_CheckHigh(10-4);
	TermClk_Low(0);
	TermClk_Output(35);
	TermClk_HighOc(20-6);							// 'open collector' with edge inprovement !
}

MatrixError_t CBitBangIBM::BitBangOutTerm(unsigned int Data, unsigned int extrainfo)
{
	unsigned char ucParityBit;
	unsigned char ucBitMask;

	ResetCommands();							  // empty the command buffer


	TermClk_WaitHigh(500);						// Give it a chance to proceed before giving up

	PrepareWaitForSilence(140);				// wait for clock and data to be high for 150 uSec.
	TermDat_CheckHigh(10);						// ensure that the PC does not want to talk
	CheckForSilence(5);							// possibly abort if no silence

	DisableKeyboard(10);
	KbdClk_Low(5);									// prevent the keyboard from sending anything
	KbdClk_Output(5);
	TermDat_Low(0);								// issue the START bit
	TermDat_Output(10);

	PulseClockTerm();

	// Send 8 bits of data, LSB first
	ucParityBit = 0;
	for ( ucBitMask = 0x01; ucBitMask; ucBitMask <<= 1 )
	{
		if ( Data & ucBitMask )				// Send 8 DATA bits - one at a time!
		{
			TermDat_High(10);
			ucParityBit++;
		}
		else
		{
			TermDat_Low(10);
		}
		PulseClockTerm();
	}

	// Send the parity bit
	if (ucParityBit&1)
	{
		TermDat_Low(10);
	}
	else
	{
		TermDat_High(10);
	}
	PulseClockTerm();

	// Send the stop bit without test for abort
	TermDat_High(0);
	TermClk_Low(15);			// We cannot use the PulseClock, because aborts are not allowed here
	TermClk_Output(35);

	OnAbortResumeHere();

	TermClk_HighZ(0);			// Important: just let the Clock float high !!
	TermDat_HighZ(10);

	if(!(extrainfo & EX_KEYEMU))
	{
		EnableKeyboard(90);
	}

	KbdClk_HighZ(5);			// allow the keyboard to send again.
									// It might still be disabled if we are in kbd emulation mode

	return RunBigBanger();	// start Bitbanger
}


MatrixError_t CBitBangIBM::BitBangInTerm(UINT &uiReturnData)
{
	int iCount;
	MatrixError_t ret = ERR_PASS;
	ResetCommands();

	/* Do a fast test first. This is used for polling via the "CheckDirCon" function */
	if (1== WedgeHal.ReadBit(CWedgeHal::TERM_DATA_IN))
		return ERR_BB_READ_NoData;

	/* Wait max. 30mSec in case we are connected to the download cable */
	/* This delay only occurs if we are connected to a download cable */
	TermClk_WaitHigh(30000);

	/* Ensure to keep a potential keyboard quiet */
	KbdClk_Low(5);									// prevent the keyboard from sending anything
	KbdClk_Output(5);

	/* now ensure the start condition */
	TermClk_CheckHigh(10);				      // time not critical
	TermDat_CheckLow(0);

	/* Read 9 bits, LSB first, parity last */
	for (iCount = 0; iCount < 9; iCount++)
	{
		PulseClockTerm();
		TermDat_ReadShiftRight(10);
	}

	// Read stop bit
	PulseClockTerm();
	WhilePortLow(CWedgeHal::TERM_DATA_IN);
	{
		PulseClockTerm();
		SetAbort();
	}
	EndWhile();

	// Send the acknowledge pulse
	TermDat_Low(0);
	TermDat_Output(10);
	PulseClockTerm();

	OnAbortResumeHere();

	TermClk_HighZ(0);			// Important: just let the Clock float high !!
	TermDat_HighZ(10);

	KbdClk_HighZ(5);			// allow the keyboard to send again.
									// It might still be disabled if we are in kbd emulation mode

	ret = RunBigBanger();	// start Bitbanger

	if(IsOK(ret))
	{
		ULONG tempdata = GetReceivedPortData(CWedgeHal::TERM_DATA_IN) >>22;
		if (!CheckOddParity(tempdata, 9))	// these are the received bits
			ret = ERR_BB_READ_ERROR;

		uiReturnData = (UINT) tempdata&0xff;
	}

	return ret;
}

MatrixError_t CBitBangIBM::BitBangOutKbd(unsigned int Data, unsigned int /* extrainfo */)
{
	unsigned char ucParityBit;
	unsigned char ucBitMask;

	ResetCommands();							// empty the command buffer

	KbdClk_Low(0);								// stop the keyboard
	KbdClk_Output(100);

	KbdDat_Low(0);								// issue the START bit
	KbdDat_Output(10);
	KbdClk_HighZ(2);							// allow keyboard to receive

	// Send 8 bits of data, LSB first
	ucParityBit = 0;
	for ( ucBitMask = 0x01; ucBitMask; ucBitMask <<= 1 )
	{
		KbdClk_WaitLow(100);
		if ( Data & ucBitMask )				// Send 8 DATA bits - one at a time!
		{
			KbdDat_High(10);
			ucParityBit++;
		}
		else
		{
			KbdDat_Low(10);
		}
		KbdClk_WaitHigh(100);
	}

	// Send the parity bit
	KbdClk_WaitLow(100);
	if (ucParityBit&1)
	{
		KbdDat_Low(10);
	}
	else
	{
		KbdDat_High(10);
	}
	KbdClk_WaitHigh(100);

	// Send the stop bit
	KbdClk_WaitLow(100);
	KbdDat_HighOc(10);
	KbdClk_WaitHigh(100);

	KbdClk_WaitLow(100);
	KbdDat_CheckLow(10);
	KbdClk_WaitHigh(100);

	// issue the ack pulse
	KbdClk_Low(0);
	KbdClk_Output(35);
	KbdClk_HighZ(20-6);

	OnAbortResumeHere();

	KbdClk_HighZ(0);			// Important: just let the Clock float high !!
	KbdDat_HighZ(10);

	return RunBigBanger();	// start Bitbanger
}

MatrixError_t CBitBangIBM::BitBangInKbd(UINT &uiReturnData)
{
	int iCount;
	MatrixError_t ret = ERR_PASS;
	ResetCommands();

	/* Read 9 bits, LSB first, then parity, then stop bit at last */
	for (iCount = 0; iCount < 10; iCount++)
	{
		KbdClk_WaitLow(100);
		KbdDat_ReadShiftRight(10);
		KbdClk_WaitHigh(100);
	}

	KbdClk_Low(0);
	KbdDat_Output(10);

	OnAbortResumeHere();

	KbdClk_HighZ(0);			// Important: just let the Clock float high !!
	KbdDat_HighZ(10);

	ret = RunBigBanger();	// start Bitbanger

	if(IsOK(ret))
	{
		ULONG tempdata = GetReceivedPortData(CWedgeHal::TERM_DATA_IN) >>21;
		if (!CheckOddParity(tempdata, 9))	// these are the received bits
			ret = ERR_BB_READ_ERROR;
		if (!(tempdata & 0x20))	            // check the stop bit
			ret = ERR_BB_READ_ERROR;

		uiReturnData = (UINT)tempdata&0xff;
	}

	return ret;
}

/*****************************************************************************
*
Description: (Emulates IBM-PC keyboard.)
Two way: PC to keyboard and keyboard to PC.
Serial bit stream synchronized to simultaneous clock stream as below:
The data transfer is 11 bits;  start (LO); 8 data; 1 odd parity; stop (HI).

Bit #	Function
1	Start bit (always 0)
2	Data bit 1 (least-significant)
3 	Data bit 2
4 	Data bit 3
5 	Data bit 4
6 	Data bit 5
7 	Data bit 6
8 	Data bit 7
9	Data bit 8 (most-significant)
10	Parity bit (always odd)
11	Stop bit (always 1)

	Communication Protocol:

Clock (CLK) and Data (DAT) lines must begin as inputs.
The CLK and DAT lines must be idle (HI) to begin, for 150 uSec minimum. (idle)
	If DAT is held low after we have checked for activity, this
   ..indicates the terminal is trying to send information to the kbd (abort).
Make DAT an output LO (start bit).
Delay 5 uSecs. (min)
Make CLK an output LO.
Wait 40 uSec.
Raise CLK.
Make CLK an input.
Check for CLK to remain HI for 40uSec. else abort.
Beginning with bit 0 thru bit 8 then parity bit send data same as start bit.
End with stop bit but hold CLK Lo for 9mSec and start again.

NOTE:
Send Data (10*80uSec ~1mSec)
Hold CLK Lo for (9 mSec)
Leave CLK & DAT idle (9 mSec)
Send 0xF0 (~1mSec)
Hold CLK Lo for (1 mSec)
Leave CLK & DAT idle (3 mSec)
Send Data (~1mSec)
Hold CLK Lo for (9 mSec)
Leave lines idle
*

WRITE:
			    start bit    bit 0 ................~.  parity bit     stop bit  (terminal might hold the clk down)
		 -------+      +------+      +------+     ~+------+      +------+      +--------
clock:	     +------+      +------+      +-----~+      +------+      +------+-------- (up to many milli seconds)
			 | 20|  40  |  40  |  40  |  40  |  40 ~|  40  |  40  |  40  |  40  |
      ----+             ++------------++--------~---++------------+-----------------
data :    +-------------++------------++--------~---++------------+


READ:
			           start bit       bit 0 ..................~.  parity bit     stop bit  			(terminal might hold the clk down)
		 --+             +----------+      +------+      +-----~+      +------+      +------+      +-------------
clock:	+-------------+          +------+      +------+     ~+------+      +------+      +------+-------------
			                         |  40  |  40  |  40  |  40 ~|  40  |  40  |  40  |  40  |  40  |  40  |
			  |(can be milliseconds) |5|  |5|  40  |5|  |5|  40 ~|5|  |5|  40  |5|  |5|  40  |5|  |5|  40  |
      -----+                        ++++----------++++-------~--++++----------++++----------++++         ++++--
data :     +------------------------++++----------++++-------~--++++----------++++          ++++---------++++



*****************************************************************************/





