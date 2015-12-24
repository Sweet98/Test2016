/*-------------------------------------------------------------------

 *-------------------------------------------------------------------
 * ScanDriverEngine.c -- This file contains engine specific code for
 *	communicating with the engine PSOC controller -- and intends to cover diagnostic/direct
 * commands to the I2C of the sensor if some interaction is needed by the Scan Driver layer --
 * which will be part of several -- first 5000 VGA and 5000 MP -- engines.
 *-------------------------------------------------------------------
 * $RCSfile: ScanDriverEngine.c $
 * $Revision: 1.32 $
 * $Date: 2011/09/09 14:52:42EDT $
 *-------------------------------------------------------------------*/
#include <stdarg.h>
#include <stdio.h>
#include <string.h>            //added for testflex

#if defined(_WIN32_WCE) || defined(PALMOS) || defined(_MSC_VER)
#include <windows.h>
#else // DBALL
#include "language.h"
#include "platform.h"
#endif

#include "ScandriverDebug.h"
#include "ScanDriverEngine.h"
#include "ScanDriverScannerSelect.h"
#include "ScanDriverhwLayer.h"

#ifdef SD_NAMESPACE
   namespace SD_NAMESPACE {
#endif


volatile BOOL bPsocPresent = FALSE;
volatile BOOL bPsocDiagnosticMode = FALSE;
volatile BOOL bSensorDiagnosticMode = FALSE;

static BYTE PsocRegistersMem[64];  // BYTE array keeps original read for posterity in normal BYTE order
static HHPScanDriverPsocRegisterTable_t g_PsocRegisters; // Tracks same original settings, but big/endian swapped where appropriate

#ifndef PALMOS
volatile HHPScanDriverPsocRegisterTable_t *gp_PsocRegisters = &g_PsocRegisters;

#else
volatile HHPScanDriverPsocRegisterTable_t *gp_PsocRegisters;

void InitScanDriverEngine(void)
{
	gp_PsocRegisters = &g_PsocRegisters;
}
#endif

static int ImagerWasAlreadyPoweredUp = FALSE;


// This function doesn't appear until later on, but EngineReset needs it defined now, hence this declaration.
BOOL NoLatencyIssue(void);

void PsocLoadStructure(unsigned char *Registers);   //added for testflex



//
// Reset Engine
//
BOOL EngineReset(void)
{
	BOOL bStatus = FALSE;
	BYTE data=PSOC_CMD_RESET_ENGINE;

//	if (UpdateVersion() && NoLatencyIssue())
	if (NoLatencyIssue())
		bStatus =PsocWriteBytes(&data,1);

	if (bStatus)
		HWLAYER.WaitMilliseconds(30);

	return bStatus;
}


//
// Engine Setup Diagnostic Mode -- this will set the flag and reset the PSOC, effectively returning EEPROM config values into RAM
//
BOOL PsocSetupDiagnosticMode (BOOL state)
{
	BOOL bStatus = FALSE;
	if (bPsocPresent) {
		if (bPsocDiagnosticMode != state) {
			bPsocDiagnosticMode = state;
			bStatus = TRUE; // pull EngineReset for now, problem on certain versions!! EngineReset();
		}
		else {
			bStatus = TRUE ; // already set in this mode, do not change and/or reset;
		}

	}
	return bStatus;
}


//
//	PsocPreCommunication
//		Perform pre communication setup, at first to make sure Psoc is actually powered to communicate with
//		 **** Note, Scan Driver invoked Psoc communication will be done *inside* update processing which will call here
//		 **** with engine already or currently powered, so these functions should do nothing.  These functions are
//		 **** really here to support diagnostic and direct comm invoked to PSOC through Scan Driver Ioctls.
//
/*
  Note:  this has a very large delay "waiting" for the PSOC to come out of reset (it's reset on bootup).  There are
  times (manual trigger mode) when the PSOC has already finished booted when this gets called and we unnecessarily
  delay (see Jira NG2D-916).  We need the PSOC controlling/writing code to be in a class and that can use a timer that
  gets started every time the PSOC gets reset.  That way, we can minimize or eliminate unecessary delaying.

  TODO:  we are turning the imager on & off here and it has nothing to do with the PSOC.  We should consider removing this.  If
  some other part of the app code needs to do this, then it probably should be done there, not here.
*/
void PsocPreCommunication(void)
{
	if (bPsocPresent) 
	{
		ImagerWasAlreadyPoweredUp = HWLAYER.EngineCurrentlyPowered();

		if (!ImagerWasAlreadyPoweredUp)
		{
			if(HWLAYER.IsPsocSupported())
			{
				unsigned char nOffset = 0x01;
				unsigned char nRegValue[2] = {0x01,0xC2};

				HWLAYER.WaitMilliseconds(30);           //wait for psoc reset, remove if our final decision is not to reset
			
				HWLAYER.ImagerPowerUp();				
		
				HWLAYER.WaitMilliseconds(8);	//1ms for psoc, 1 for jade, 5 for clock to become stable, 1 for luck, wait for jade 

				HWLAYER_JADE.WriteIIC_SpreadSpec_w_ADD(nOffset,(const unsigned char*) &nRegValue,2);

				HWLAYER.WaitMilliseconds(12);   //wait for spread spectrum before talking to imager, need 20ms, try 12 since we already have 8 above
			}
		}
	}
}


//
//	PsocPostCommunication
//		Perform cleanup following base communication setup, at first to return Psoc to same power state as started
//		 **** Note, Scan Driver invoked Psoc communication will be done *inside* update processing which will call here
//		 **** with engine already or currently powered, so these functions should do nothing.  These functions are
//		 **** really here to support diagnostic and direct comm invoked to PSOC through Scan Driver Ioctls.
//
void PsocPostCommunication(void)
{
	if (bPsocPresent && !ImagerWasAlreadyPoweredUp)
	{
		if (HWLAYER.SynchronizedPowerDown)
		{
			DBG_OUTPUT ("SyncPD\r\n");
			HWLAYER.SynchronizedPowerDown();
		}
		else
		{
			DBG_OUTPUT ("NOSyncPD\r\n");
			HWLAYER.ImagerPowerDown();
		}

	}
}

//
// NoLatencyIssue is attempt to understand if the PSOC has been improved to a level where there is no main loop (polling)
//		latency to the I2C communication
//
//
BOOL NoLatencyIssue(void)
{
	BOOL bStatus = FALSE;  // For now latency issue exists , apply 5ms between all read/write transactions
// Versions of PSOC when I2C interrupt control was done is the detail that qualifies no need for latency.  1.26 was first
// version of 5100, 2.01 first version of 5300, and then some arbitrary selection to avoid 0x3030 as unset case and yet give us
// some level of forward compatibility.
	if (bPsocPresent)   // code flow 'weirdness' intentional for quickest pass through without having to have multiple returns
	{
		if (gp_PsocRegisters->MajorRevision == 0x01) {
			if (gp_PsocRegisters->MinorRevision > 25)
				bStatus = TRUE;
		}
		else if (gp_PsocRegisters->MajorRevision == 0x02) {
		 	if (gp_PsocRegisters->MinorRevision > 0x00)
				bStatus = TRUE;
		}
		else if (gp_PsocRegisters->MajorRevision > 0x02 && gp_PsocRegisters->MajorRevision < 0x09)
			bStatus = TRUE;
	}

	return bStatus;
}


/*
	*********
	PSOC 'general functionality' has two current exceptions:
	1) Synchronized Power down versus Power Down (async)
	2) PSOC Write Reg Safe transaction versus historical two step which was prone to errors.

	Several techniques were considered to handle these exceptions:
	(a) valid / invalid functions
	(b) global 'single' engine type flag
	(c) use/trust the MfgId in the first location of the PSOC
	(d) use PSOC firmware version

	** as the difference is between Mpx (no sync power down and Psoc write safe transaction) versus 5000 VGA / MT9 (sync power down and historical two step PSOC write transaction) **

	Would have preferred to chose (c) but couldn't trust -- for now -- MPx load of MfgId, so (a) was chosen over (d) for no real good reason other than it seemed the better to do from code point of view.
	*********
*/


//*************************************************
// ONLY 4 Functions that should be calling the PSOC write/read I2C

//
// PsocWriteBytes
//		Base function to write request byte(s) to Psoc
//		Required to send non-register based commands
BOOL PsocWriteBytes (BYTE *data, BYTE cnt)
{
	BOOL bStatus = FALSE;
	DBG_OUTPUT2 ("WB %x cnt %d\r\n",data[0], cnt);
	if (HWLAYER.IsPsocSupported()) {
		PsocPreCommunication();
		bStatus = HWLAYER.WriteIIC_PSOC(data,cnt);
		PsocPostCommunication();
	}

	return bStatus;
 }

//
// PsocReadBytes
//		Base function to read request byte(s) from Psoc
//		Required to read non-register data
BOOL PsocReadBytes (BYTE *data, BYTE cnt)
{
	BOOL bStatus = FALSE;

	if (HWLAYER.IsPsocSupported()) {
		PsocPreCommunication();
		bStatus = HWLAYER.ReadIIC_PSOC(data,cnt);
		PsocPostCommunication();
	}
	DBG_OUTPUT2 ("RB %x cnt %d\r\n",data[0], cnt);

	return bStatus;
}

static BOOL dummyRead (void)
{
	BOOL bStatus = FALSE;
	int i =0;
	BYTE data;
	BYTE cnt = 1;

	DBG_OUTPUT ("dR\r\n");
	do {
		bStatus = HWLAYER.ReadIIC_PSOC(&data,cnt);
		DBG_OUTPUT1 ("%d",i);
	} while ((i++<10) && !bStatus);

	DBG_OUTPUT1 ("b%d\r\n",bStatus);
	return bStatus;
}

/* REPLACE the original retry band-aid for weak protocol by putting the retries and dummy read 'more appropriately'
 * around the source  -- Effective change was that the retries of individual reads and writes were replaced
 * with "transaction" based retries, still with the dummyRead should things have not worked on a given attempt.
 */
/*
static BOOL WritePsoc (BYTE *data, BYTE cnt)  {
	int i=0,bStatus=0;
	do {
		bStatus = HWLAYER.WriteIIC_PSOC(data,cnt);
	} while ((i++<10)&&(bStatus == 0));
	if (!bStatus)
		dummyRead();   // added protection to "clear" the weak protocol!!  Note, it cannot be done on every retry because that would by nature get us out of sync !!
	return bStatus;
}


static BOOL ReadPsoc (BYTE *data, BYTE cnt) {
	int i=0,bStatus=0;
	do {
		bStatus = HWLAYER.ReadIIC_PSOC(data,cnt);
	} while ((i++<10)&&(bStatus == 0));
	if (!bStatus)
		dummyRead();   // added protection to "clear" the weak protocol!!  Note, it cannot be done on every retry because that would by nature get us out of sync !!
	return bStatus;
}
*/

//
// PsocWriteRegisters
//		Write data byte(s) to consecutive memory/offset registers in the PSoc starting at offset address
BOOL PsocWriteRegisters (DWORD offset, const BYTE * data, BYTE cnt )   //changed from: (DWORD offset, BYTE * data, BYTE cnt ) before testflex changes
{
	BYTE boffset = offset;
	BOOL bStatus = FALSE;
	BOOL dmStatus = TRUE; // dummy read status, all currently is O'tay
	int i = 0;

	if (HWLAYER.IsPsocSupported())
	{
		if (HWLAYER.WriteIIC_PSOC_w_ADD) // new and improved
		{
			DBG_OUTPUT ("PsocWrSingleEvent\r\n");
			PsocPreCommunication();
			if (HWLAYER.WriteIIC_PSOC_w_ADD(boffset,data,cnt))
			{
				bStatus = TRUE;
				DBG_OUTPUT3 ("WriteWAdd PASSED off %d, data0 %d, cnt %d\r\n",boffset, data[0], cnt);
			}
			else
			{
				DBG_OUTPUT3 ("WriteWAdd FAILED off %d, data0 %d, cnt %d\r\n",boffset, data[0], cnt);
			}
			PsocPostCommunication();
		}
		else
		{
			DBG_OUTPUT ("PsocWrTwoEvents\r\n");

			PsocPreCommunication();
			if (NoLatencyIssue()) {
				do {
					if (HWLAYER.WriteIIC_PSOC(&boffset,1) && HWLAYER.WriteIIC_PSOC(data,cnt))
						bStatus = TRUE;
					else
						dmStatus = dummyRead();
					if (i)
						DBG_OUTPUT1 ("PRR i %d\r\n",i);
				} while ((i++<10) && (!bStatus) && dmStatus);

			}
			else {
				do {
					if (HWLAYER.WriteIIC_PSOC(&boffset,1)) {
						HWLAYER.WaitMilliseconds(5);
						bStatus = HWLAYER.WriteIIC_PSOC(data,cnt);
						HWLAYER.WaitMilliseconds(5);
					}
					if (!bStatus)
						dmStatus = dummyRead();
					if (i)
						DBG_OUTPUT1 ("PRR i %d\r\n",i);
				} while ((i++<10) && (!bStatus) && dmStatus);
			}
			PsocPostCommunication();

		}
	}

	return bStatus;
}

//
// PsocReadRegisters
//		Read data byte(s) from consecutive memory/offset registers in the PSoc starting at offset address
BOOL PsocReadRegisters (DWORD offset, BYTE * data, BYTE cnt)
{
	BOOL bStatus = FALSE;
	BYTE boffset = offset;
	BOOL dmStatus = TRUE; // dummy read status, all currently is O'tay
	int i = 0;

	if (HWLAYER.IsPsocSupported())	{
		PsocPreCommunication();
		if (NoLatencyIssue()) {
			do {
				if (HWLAYER.WriteIIC_PSOC(&boffset,1) && HWLAYER.ReadIIC_PSOC(data,cnt))
					bStatus = TRUE;
				else
					dmStatus = dummyRead();
				if (i)
					DBG_OUTPUT1 ("PRR i %d\r\n",i);
			} while ((i++<10) && (!bStatus) && dmStatus);
		}
		else {
			do {
				if (HWLAYER.WriteIIC_PSOC(&boffset,1)) {
					HWLAYER.WaitMilliseconds(5);
					bStatus = HWLAYER.ReadIIC_PSOC(data,cnt);
					HWLAYER.WaitMilliseconds(5);
				}
				if (!bStatus)
					dmStatus = dummyRead();
				if (i)
					DBG_OUTPUT1 ("PRR i %d\r\n",i);

			} while ((i++<10) && (!bStatus) && dmStatus);
		}
		PsocPostCommunication();
	}

	return bStatus;

}


//#if defined(OEM_ENGINE)
//SpreadReadRegisters
BOOL SpreadReadRegisters (DWORD offset, BYTE * data, BYTE cnt)
{
	BOOL bStatus = FALSE;
	BOOL dmStatus = TRUE; // dummy read status, all currently is O'tay
	int i = 0;

	if (HWLAYER.IsPsocSupported())	
	{
		PsocPreCommunication();

		if (NoLatencyIssue()) 
		{
			do {
				if (HWLAYER_JADE.ReadIIC_SpreadSpec(data, offset, cnt))
					bStatus = TRUE;
				else
					dmStatus = dummyRead();
				if (i)
					DBG_OUTPUT1 ("PRR i %d\r\n",i);
			} while ((i++<10) && (!bStatus) && dmStatus);
		}
		PsocPostCommunication();
	}

	return bStatus;
}


//SpreadWriteRegister
BOOL SpreadWriteRegisters (DWORD offset, const BYTE * data, BYTE cnt )
{
	BYTE boffset = offset;
	BOOL bStatus = FALSE;
	BOOL dmStatus = TRUE; // dummy read status, all currently is O'tay
	int i = 0;

	if (HWLAYER.IsPsocSupported())
	{
		if (HWLAYER_JADE.WriteIIC_SpreadSpec_w_ADD) // new and improved
		{
			dbg_output ("SpreadSpecWrSingleEvent\r\n");
			PsocPreCommunication();

			if (HWLAYER_JADE.WriteIIC_SpreadSpec_w_ADD(boffset,data,cnt))
			{
				bStatus = TRUE;
				DBG_OUTPUT3 ("WriteWAdd Spread Spectrum PASSED off %d, data0 %d, cnt %d\r\n",boffset, data[0], cnt);
			}
			else
			{
				DBG_OUTPUT3 ("WriteWAdd Spread Spectrum FAILED off %d, data0 %d, cnt %d\r\n",boffset, data[0], cnt);
			}

			PsocPostCommunication();
		}
		else
		{
			DBG_OUTPUT ("SpreadWrTwoEvents\r\n");
			PsocPreCommunication();

			if (NoLatencyIssue()) 
			{
				do {
					if (HWLAYER_JADE.WriteIIC_SpreadSpec(&boffset,1) && HWLAYER_JADE.WriteIIC_SpreadSpec(data,cnt))
						bStatus = TRUE;
					else
						dmStatus = dummyRead();
					if (i)
						DBG_OUTPUT1 ("PRR i %d\r\n",i);
				} while ((i++<10) && (!bStatus) && dmStatus);
			}
			else 
			{
				do {
					if (HWLAYER_JADE.WriteIIC_SpreadSpec(&boffset,1)) {
						HWLAYER.WaitMilliseconds(5);
						bStatus = HWLAYER_JADE.WriteIIC_SpreadSpec(data,cnt);
						HWLAYER.WaitMilliseconds(5);
					}
					if (!bStatus)
						dmStatus = dummyRead();
					if (i)
						DBG_OUTPUT1 ("PRR i %d\r\n",i);
				} while ((i++<10) && (!bStatus) && dmStatus);
			}
			PsocPostCommunication();

		}
	}

	return bStatus;
}

//#endif		//end if defined(OEM_ENGINE)



//**************************Testflex*******************************
// Writes to the PSOC registers, and also updates the local copies of the PSOC register data
BOOL PsocUpdateRegisters(DWORD RegOffset, const BYTE * pData, BYTE DataLength)
{
	if ( RegOffset + DataLength > sizeof PsocRegistersMem ) return FALSE;

	if ( PsocWriteRegisters(RegOffset, pData, DataLength) )
	{	// Update local copies of PSOC register data
		memcpy(PsocRegistersMem + RegOffset, pData, DataLength);
		// Adjust checksum
		BYTE Checksum = 0;
		BYTE * pData = PsocRegistersMem;
		while ( pData < PsocRegistersMem + PSOC_OFFSET_REG_CHECKSUM )
		{
			Checksum += *pData;
			pData++;
		}
		*pData = Checksum; // pData now points to the checksum byte--update it
		PsocLoadStructure(PsocRegistersMem); // Now load the data structure that mirrors PsocRegistersMem[]
	}
	else return FALSE;

	return TRUE;
}
//*****************************************************************

// ... End of
// ONLY 4 Functions that should be calling the PSOC write/read I2C
//*************************************************


//
// PsocInternalWriteRegisters
//		Internal Scan Driver real time calls  need to be exception handled by diagnostic over-ride
//
BOOL PsocInternalWriteRegisters(BYTE offset, BYTE * data, BYTE cnt)
{
	BOOL bStatus = FALSE;

	if (!bPsocDiagnosticMode)
	{
		bStatus = PsocWriteRegisters(offset, data, cnt);
	}

	return bStatus;
}


//
// PsocInternalReadRegisters
//		Internal Scan Driver real time calls need to be exception handled by diagnostic over-ride
//
BOOL PsocInternalReadRegisters(BYTE offset, BYTE *data, BYTE cnt)
{
	BOOL bStatus = FALSE;

	if (!bPsocDiagnosticMode)
	{
		bStatus = PsocReadRegisters(offset,data,cnt);
	}

	return bStatus;
}

//
// PsocInternalWriteBytes
//		Internal Scan Driver real time calls need to be exception handled by diagnostic over-ride
//
BOOL PsocInternalWriteBytes(BYTE * data, BYTE cnt)
{
	BOOL bStatus = FALSE;

	if (!bPsocDiagnosticMode)
		bStatus = PsocWriteBytes(data, cnt);

	return bStatus;
}


//
// PsocInternalReadBytes
//		Internal Scan Driver real time calls need to be exception handled by diagnostic over-ride
//
BOOL PsocInternalReadBytes(BYTE *data, BYTE cnt)
{
	BOOL bStatus = FALSE;

	if (!bPsocDiagnosticMode)
		bStatus = PsocReadBytes(data,cnt);

	return bStatus;
}



void PsocLoadStructure(unsigned char *Registers)
{
	unsigned char *ptr = (unsigned char *)gp_PsocRegisters; // = (HHPScanDriverPsocRegisterTable_t *)Registers;
	unsigned short temp;
	int i;

// copy original to Psoc Structure -- heretofor, we have both original in PsocRegistersMem and the usuable, where
// necessary big/little endian shifted gp_PsocRegister
	for (i=0;i<sizeof(HHPScanDriverPsocRegisterTable_t);i++)
{		ptr[i]=Registers[i];

	//	printf ("PLS i%d 0x%2x 0x%2x\r\n",i,gp_PsocRegisters[i],ptr[i]);
}

	//Engine Id
	temp = UINT_TO_BE(gp_PsocRegisters->EngineID);
	gp_PsocRegisters->EngineID = temp;

	//Aimer X
	temp = USHORT_TO_BE(gp_PsocRegisters->AimerCenterX);
	gp_PsocRegisters->AimerCenterX = temp;

	//Aimer Y
	temp = USHORT_TO_BE(gp_PsocRegisters->AimerCenterY);
	gp_PsocRegisters->AimerCenterY = temp;

	//Firmware Checksum
	temp = USHORT_TO_BE(gp_PsocRegisters->FirmwareChecksum);
	gp_PsocRegisters->FirmwareChecksum = temp;


	DBG_OUTPUT4 ("EngineId %4x, AimerX %4d, AimerY %4d, FirmwareChecksum %4x\r\n",gp_PsocRegisters->EngineID, gp_PsocRegisters->AimerCenterX, gp_PsocRegisters->AimerCenterY, gp_PsocRegisters->FirmwareChecksum);
}

void PsocDumpConfigArea (void)
{
	unsigned char *ptr;
	int i;
//	PsocReadConfigArea(); // seems to STILL be some ridiculous sensitivity to other things going on that makes this fail

	ptr = (unsigned char *) PsocRegistersMem;
	dbg_output (("*******************\r\nPSOC Actual Registers 0x0 through 0x3f"));
	for (i=0;i<64;i++) {
		if (i%16 == 0)
			dbg_output ("\r\n%02x - ",i);
		dbg_output ("%02x  ", ptr[i]);
	}
	dbg_output (("\r\n*******************\r\n"));

	ptr = (unsigned char *)gp_PsocRegisters;
	dbg_output (("*******************\r\nPSOC Structure Adjusted"));
	for (i=0;i<64;i++) {
		if (i%16 == 0)
			dbg_output ("\r\n%02x - ",i);
		dbg_output ("%02x  ", ptr[i]);
	}
	dbg_output (("\r\n*******************\r\n"));

}



//
// PsocRegisterChecksumVerify
// 	Function takes a PSOC register memory and verifies internal checksum of memory
//		is valid
BOOL PsocRegisterChecksumVerify (unsigned char *Registers)
{
	HHPScanDriverPsocRegisterTable_t *RegStruct = (HHPScanDriverPsocRegisterTable_t *)Registers;
	BOOL ChecksumStatus = FALSE;

	unsigned char	Checksum = 0;
	unsigned char	*ChecksumPtr = (unsigned char *) Registers;
	while( ChecksumPtr < (unsigned char *) &( RegStruct->RegisterChecksum ) )
	{
		Checksum += *ChecksumPtr++;
	}
	if( Checksum == RegStruct->RegisterChecksum )
		ChecksumStatus = TRUE;

	DBG_OUTPUT3 ("ChecksumStatus %d Calc %x Actual %x\r\n",ChecksumStatus, Checksum, RegStruct->RegisterChecksum);

	return ChecksumStatus;

}


//
//	PsocReadConfigArea
//		Base, initialization singularly called function to get current Psoc memory state
BOOL PsocReadConfigArea (void)
{
	BOOL bStatus=TRUE;
	BOOL ReadSuccessful=FALSE;
	unsigned char offset=0;
	int retry=10;

	DBG_OUTPUT1 ("ReadPsocConfigArea size %d\r\n",sizeof(PsocRegistersMem));

	while (bStatus && !ReadSuccessful)
	{
		bStatus = PsocReadRegisters(offset,PsocRegistersMem,sizeof(PsocRegistersMem)); //** call base function as this is before CS is setup
		ReadSuccessful = PsocRegisterChecksumVerify(PsocRegistersMem);
		if (!ReadSuccessful && (--retry==0))
			bStatus = FALSE;
	}


	if (bStatus) {
		PsocLoadStructure(PsocRegistersMem);
		bPsocPresent = TRUE;
	}


#if ESHELL_DEBUG || DBALL_DEBUG // over do this, because don't want to waste the for loop if not necessary
	PsocDumpConfigArea();
#endif

	return bStatus;

}

BOOL ReportImagerProperties(  const DWORD * pBufIn, DWORD * pBufOut,
							  DWORD dwLenOut)
{
	BOOL bStatus = FALSE;
	unsigned short us;
	DWORD len,tag;
	unsigned int i;


	len = dwLenOut/sizeof(DWORD);
	for (i = 0; i < len; i+= 2)
	{
		bool hwlayer_get_properties = false;
		tag = pBufIn[i];
		pBufOut[i] = tag;
		DWORD & value = pBufOut[i + 1];

		switch(tag)
		{
		case IP_AIMER_X_OFFSET:
			if (!bPsocPresent)
				hwlayer_get_properties = true;
			else
			{
				us = gp_PsocRegisters->AimerCenterX;

				// Sanity check changed to just make sure it is without
				// overall bounds of dimension
				if ((us > 0) && (us < (HWLAYER.GetScanWidth() - 1)))
					value = us;
				else
					hwlayer_get_properties = true;
			}
			break;
		case IP_AIMER_Y_OFFSET:
			if (!bPsocPresent)
				hwlayer_get_properties = true;
			else
			{
				us = gp_PsocRegisters->AimerCenterY;
				// Sanity check changed to just make sure it is without
				// overall bounds of dimension
				if ((us > 0) && (us < (HWLAYER.GetScanHeight() - 1)))
					value = us;
				else
					hwlayer_get_properties = true;
			}
			break;
		case IP_PSOC_MINOR_REV:
			if (bPsocPresent)
				value = gp_PsocRegisters->MajorRevision;
			else
				value = (DWORD)-1;
			break;
		case IP_PSOC_MAJOR_REV:
			if (bPsocPresent)
				value = gp_PsocRegisters->MinorRevision;
			else
				value = (DWORD)-1;
			break;
		default:
			hwlayer_get_properties = true;
			break;
		}

		if (hwlayer_get_properties)
		{
			bStatus = HWLAYER.GetImagerProperties(  pBufIn + i, pBufOut + i,
													sizeof(DWORD) * 2  );
			if (bStatus == FALSE)
				return bStatus;
		}
	}

	return TRUE;
}

#ifdef SD_NAMESPACE
   } // namespace SD_NAMESPACE {
#endif
