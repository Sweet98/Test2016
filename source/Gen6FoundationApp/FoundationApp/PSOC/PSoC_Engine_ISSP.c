//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/PSOC/PSoC_Engine_ISSP.c $
// $Revision: 1.8 $
// $Date: 2011/09/09 14:54:57EDT $

/// GENERAL DESCRIPTION
//**************************************************************************************
//
//  ISSP Flash management functions for the Engine PSoC
//
//	   Implements a full reset and reflash of the Cypress CY8C24423A.  Refer to AN2026, AN2026a and AN2015
//		from Cypress MicroSystems for full details.  The port pins used are Px.x, Px,y, and Pz.n.  Note - Interrupts are disabled only
//		during flashing when a stable clock is required.  We control SCLK at all times.  SDATA is only relinquished for the eight
//		clocks to read a byte.  The PSOC seems to have a weak drive when pulling the SDATA line low so we use slowed clocking when
//		reading and for some reason when we are waiting for it to complete a block read.  Reads are clocked on rising edges
//		and writes are clocked on falling edges.
//
//				Flashing proceeds as follows:
//
//           1  Acquire PSOC
//           2  Send Init1, Init2 & Init3 vectors
//           3  Verify chip ID
//           4  Execute bulk erase
//           5  Load 64 byte block in SRAM
//           6  Flash block
//           7  Repeat for all blocks
//           9  Set security level for each block
//           10 Verify checksum
//
//				 - COMMANDS are sent as vector arrays (bitstreams of 22bits each in arrays)
//             		each bitstream is STORED AS a 24bit unsigned int and has the form:
//							0CCC CCCC CCCC CCCC CCCC CCCC CCCC CCC0
//							|\___________________________________/|
//							|					|				  \- 1 additional zero bit padding which is not sent
//							|					|
//							|					\-	22 bits of Command/vector info are sent
//							\-	1 zero bit of padding is not sent
//
//           - WRITES are SENT AS a 22 bit bitstream containing address and data of the form
//					CCC CCAA AAAA DDDD DDDD 000 the trailing zeros are ignored by the PSOC
//
//           - Byte READS are done by sending an 11bit bitstream containing the address, clocking a
//             high Z transition to read, followed by an 8bit read, another high Z transition back to write.
//					CCC CCAA AAAA Z DDDD DDDD Z
//					\___________/  | \________/  \- Transition from read clocked
//							|	  |		|
//							|	  |		\- Read
//							|	  \- Transiton to read clocked
//							\- Write vector command + 6bit address
//
//					high z clock transition to read is low  /^\_/^ to high
//
//					high z clock transition from read is high  \_/^\_ to low
//
//				 -	ID and checksum are read as CCC CCCC CCCC Z DDDD DDDD Z
//
//
//**************************************************************************************************

#include	"db_chrio.h"	// Serial port character output routines
#include	<stdio.h>		/* standard I/O functions					*/
#include	<stdlib.h>		/* standard library functions				*/
#include	<string.h>		/* standard string functions				*/
#include "Timeout.h"
#include "interrupts.h"
#include "Menu.h"
#include "ProductConfiguration.h"
#include "PSoC_Engine_ISSP.h"
#include "PSoC_27xxx_Vectors.h"
#include "PSoC_MPx_Engine_ROM.h"       // PSoC code for 5600's with 26x series PSoC
#include "ReaderConfiguration.h"
#ifndef SCAN_DRIVER_CONTROL
#define SCAN_DRIVER_CONTROL
	#include "scandrivermatrixinterface.h"
	//#include "vars.h"
#endif

#define	DEBUG	IsDebugOn			/* set to one to enable psoc debug statements for now */

#define	ENG_CYCLE_SCLK	ENG_PSOC_SCLK_HIGH; \
							HALF_BIT_DELAY; \
							ENG_PSOC_SCLK_LOW;  \
							HALF_BIT_DELAY

   // For some reason ??? we need this slower clock when waiting for the PSOC to do a Block Read
#define	ENG_CYCLE_SCLK_SLOW	ENG_PSOC_SCLK_HIGH; \
									HALF_BIT_DELAY_IN_READ_MODE; \
									ENG_PSOC_SCLK_LOW;  \
									HALF_BIT_DELAY_IN_READ_MODE

#define  ENG_HIGH_Z_CYCLE_ENTERING_READ	ENG_PSOC_SCLK_HIGH; 	\
											HALF_BIT_DELAY;	\
											ENG_PSOC_SCLK_LOW; 	\
											HALF_BIT_DELAY; 	\
											ENG_PSOC_SCLK_HIGH; 	\
											HALF_BIT_DELAY_IN_READ_MODE

#define  ENG_HIGH_Z_CYCLE_LEAVING_READ	ENG_PSOC_SCLK_LOW; 	\
											HALF_BIT_DELAY_IN_READ_MODE; \
											ENG_PSOC_SCLK_HIGH; 	\
											HALF_BIT_DELAY; 	\
											ENG_PSOC_SCLK_LOW; 	\
											HALF_BIT_DELAY

#define	LOW		0
#define	HIGH	1
#define	READ_PSOC_ID1			0x00f8		// When read function adds prefix b101 we get b101 1111 1000
#define	READ_PSOC_ID2			0x00f9		// When read function adds prefix b101 we get b101 1111 1001
#define	READ_CHECKSUM_BYTE1		0x05f9
#define	READ_CHECKSUM_BYTE2		0x05f8

#define	HALF_BIT_DELAY	asm("nop");asm("nop");asm("nop");asm("nop")
#define	HALF_BIT_DELAY_IN_READ_MODE		DELAY_BIT_TIME(4)	 // When reading the PSOC controls SDATA, for some reason the
														 // falling edge is very slow hence the slower clocking is used
#define	DELAY_BIT_TIME(k)		\
	{						\
		int j = k;				\
		while( j-- > 0 )			\
			{				\
				asm("nop");	\
				asm("nop");	\
				asm("nop");	\
				asm("nop");	\
				asm("nop");	\
				asm("nop");	\
				asm("nop");	\
				asm("nop");	\
				asm("nop");	\
				asm("nop");	\
				asm("nop");	\
				asm("nop");	\
				asm("nop");	\
			}				\
	}

//Protection levels
// 0 - External read enabled, all writes enabled
// 1 - External read disabled, all writes enabled
// 2 - External read disabled, external writes disabled, internal writes enabled
// 3 - External read disabled, all writes disabled

const unsigned int pEng24SecurityArray[]=
/*	    00,    01,    02,   03,   04,    05,    06,    07,   08,    09,    10,    11,    12,   13,   14,   15 */
{
	0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x01,
//	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//No Protection
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};

extern void Setup27xxx(void);
void reset_psoc(void);
unsigned int EngSetComLines(unsigned int);
unsigned int EngAcquirePsoc(void);
unsigned int EngVerifyIdPsoc(void);
unsigned int EngVerify_flash_psoc(void);
unsigned int EngProgramFlashPsoc(void);
unsigned int EngReadIdPsoc(void);
unsigned int EngSecure_psoc(void);
unsigned int EngVerifyFlashChecksumPsoc(void);
void EngResetPsoc(void);
unsigned int EngSendVectorArrayToPsoc(const unsigned int *pVectorArray);
void EngSend22bitStreamToPsoc(unsigned int bitstream);
inline void EngSend11bitStreamToPsoc(unsigned int bitstream);
inline unsigned int EngRead8bitStreamFromPsoc(void);
unsigned int EngWaitForSdataLowPsoc(void);
unsigned int EngWaitForSdataLowPsocSlow(void);	    // For some reason ??? we need this slower clocking when waiting for the PSOC to do a Block Read
unsigned int EngWriteByteToSramPsoc(unsigned int data, unsigned int address);
unsigned int EngReadByteFromSramPsoc(unsigned int address);
unsigned int EngReadFlashChecksumPsoc();
unsigned int EngCalculateFlashChecksumPsoc(void);
const unsigned char * EngGetPsocRamCopy(unsigned int * length);
CTimeout wait(0);
unsigned int FlashEnginePsoc(const unsigned char *pPsocHexFile, unsigned int length);
int EraseEnginePsoc(void);
unsigned int SecureEnginePsoc(const unsigned int * pSecurityArray);

bool IsDebugOn = false;
bool EnginePSoCflash = false;

unsigned int EngHexSize;
const unsigned char* pEngHExInRam;
char * MsgBuf;
//-------------------------------------------------------------------------
//
//		The following pointers/variables are assignments for 26xxx ONLY
//		 - see Setup27xxx() and Setup24423() below for 27xxx and 24xxx
//			assignments respectively.
//
//-------------------------------------------------------------------------
const unsigned int* pInitialize1 = &Initialize1_27x_24x[0];
const unsigned int* pInitialize2 = &Initialize2_27x_24x[0];
const unsigned int* pInitialize3 = &Initialize3_27x_24x[0];
const unsigned int* pIdSetup = &IdSetup_27x_24x[0];
const unsigned int* pEraseAll = &EraseAll_27x_24x[0];
const unsigned int* pProgramBlock = &ProgramBlock_24x[0];
const unsigned int* pVerifySetup = &VerifySetup_27x_24x[0];
const unsigned int* pSecureBlock = &SecureBlock_24x[0];
const unsigned int* pChecksumSetup = &ChecksumSetup_24x[0];
const unsigned int* pEraseBlock = &EraseBlock_A[0];
unsigned int padding = 0x0E;
unsigned int SRAM_Offset = 0;
unsigned int ReadPrefix = 0x0580;
unsigned int WritePrefix = 0x0480000;
unsigned int nNumBlocksInPSoC = BLOCKS_IN_24x_PSOC;
unsigned int nSecureSize = BLOCKS_IN_24x_PSOC - 3; // Do not secure the last 3 blocks... they are used for EEPROM space
//-------------------------------------------------------------------------

#define BLOCK_NUM_61 61
#define HEXFILE_OVERHEAD 6
#define EXTRA_INFO 100
void PSOC_Port_Init()
{
	ENG_MAKE_PSOC_LINES_GPIO;
	CONFIG_POWER_ENA_PAD_TO_GPIO;
	ENG_PSOC_XRES_LOW;              //disable reset
	SET_POWER_ENA_DR_REG_LOW;    	//disable power
	ENG_PSOC_XRES_DIR_OUT;
	SET_POWER_ENA_DATA_DIR_OUTPUT;
	ENG_PSOC_SDATA_DIR_OUT;
	ENG_PSOC_SCLK_DIR_OUT;
}

static void SetAimerAndIllumition()
{
	ENG_PSOC_SDATA_HIGH;
	ENG_PSOC_SCLK_HIGH;
}


// TODO:  this duplicates EngResetPsoc()
void reset_psoc(void)
{
//	if(DEBUG)
//		xputstring ("___Resetting PSOC___\r\n");
	{	// Note:  The IRQ macros create a local variable.  Because we use them twice, we need to localize one set so it compiles (duplicate variable error).
		DISABLE_INTERRUPTS();
		ENG_PSOC_XRES_HIGH;		// Enter reset mode
		RESTORE_INTERRUPTS();
	}
	
	wait.us(100);
	while(!wait.HasExpired());	// Should be at least 10us

	DISABLE_INTERRUPTS();
	ENG_PSOC_XRES_LOW;		// Leave reset mode
	RESTORE_INTERRUPTS();

	wait.us(20);
	while(!wait.HasExpired());	// Should be at least 20us before attempting to aquire
										// Note to aquire psoc, must begin sending the Initialize1 vector within 100us after sending this pulse.
}


static void Setup24423(void)
{
//	From Setup27xxx() in flash_psoc.c
	{
		pInitialize1 = &Initialize1_27x_24x[0];
		pInitialize2 = &Initialize2_27x_24x[0];
		pInitialize3 = &Initialize3_27x_24x[0];
		pIdSetup = &IdSetup_27x_24x[0];
		pEraseAll = &EraseAll_27x_24x[0];
		pProgramBlock = &ProgramBlock_27x[0];
		pVerifySetup = &VerifySetup_27x_24x[0];
		pSecureBlock = &SecureBlock_27x[0];
		pChecksumSetup = &ChecksumSetup_27x[0];
		padding = 0x0007;
		SRAM_Offset = 0;
		ReadPrefix = 0x0580;
		WritePrefix = 0x0480000;
	}
	pProgramBlock = &ProgramBlock_24x[0];
	pSecureBlock = &SecureBlock_24x[0];
	pChecksumSetup = &ChecksumSetup_24x[0];
}


//-------------------------------------------------------------------------
//
//					FLASH ENGINE PSoC
//		Main flashing routine execution for flashing the MPx Engine PSoC
//			begins here and calls down to all other functions in this module.
//
//		The read and write routines, upon entry, assume the clock is at the end
//			 of the LOW period and leave the clock at the end of the LOW period.
//
//		Data is sent to the PSoC valid on the falling edge and received
//			valid on the rising edge.
//
//		Each routine should always leave sdata in the high z state.
//
//-------------------------------------------------------------------------

unsigned int FlashEnginePsoc(const unsigned char *pPsocHexFile, unsigned int length)
{
	unsigned int cPSOC_ID = 0;
	char str[128];
	int product;

	IsDebugOn=GET_SETTING(GeneralDebugOn);

	if(DEBUG)
		xputstring ("Enter FlashEnginePSoC().\r\n");//DMH

	Setup24423();

	if( length >( (nNumBlocksInPSoC*(ENG_PSOC_BLOCK_SIZE + HEXFILE_OVERHEAD) )+ EXTRA_INFO ) )
	{
		if(DEBUG)
		{
			xputstring ("Binary to flash is too large for this device ... \r\n");
			sprintf(str,"length = %d\r\nnNumBlocksInPSoC = %d\r\nPSOC_BLOCK_SIZE = %d\r\n",length,nNumBlocksInPSoC,ENG_PSOC_BLOCK_SIZE);
			xputstring(str);
			product = nNumBlocksInPSoC*ENG_PSOC_BLOCK_SIZE;
			sprintf(str,"PSoC size = %d\r\n",product);
			xputstring(str);
		}
		return 0;
	}

	EnginePSoCflash = TRUE;
	EngHexSize = length;
	pEngHExInRam  = pPsocHexFile;
	if(!EngSetComLines(EngTakeControl))			return	FAIL;
	RESET_WATCHDOG_TIMER();
	if(!EngAcquirePsoc())
		return	FAIL;

	cPSOC_ID = EngVerifyIdPsoc();
	if(DEBUG)
	{
		sprintf (str,"PSoC_ID =  %#06x\r\n", cPSOC_ID );//DMH
		xputstring(str);//DMH
	}
	if(!EraseEnginePsoc())
	{
		EngResetPsoc();
		return	FAIL;
	}
	xputstring( "PROGRAMING...please be patient\r\n" );//DMH

	if(!EngProgramFlashPsoc())			return	FAIL;
	if(!SecureEnginePsoc(&pEng24SecurityArray[0]))
	{
		xputstring( "SecureEnginePsoc() FAILED\r\n");//DMH
		EngResetPsoc();
		return	FAIL;
	}
	if(!EngVerifyFlashChecksumPsoc())
	{
		xputstring( "EngVerifyFlashChecksumPsoc() FAILED\r\n");//DMH
		EngResetPsoc();
		return	FAIL;
	}
	EngResetPsoc();
	EngSetComLines(EngRelease);
	EnginePSoCflash = FALSE;
	xputstring( "PROGRAMMING  COMPLETE  Reset your device!\r\n" );//DMH
	return TRUE;
} //FlashEnginePsoc(const unsigned char *pPsocHexFile, unsigned int length)


//-------------------------------------------------------------------------
//
//				ERASE ENGINE PSoC
//
//-------------------------------------------------------------------------
int EraseEnginePsoc(void)
{
	int RetResult = FAIL;
	const unsigned char *pBinaryInRam;
	const unsigned char *pEndBinaryInRam;
	unsigned int BlockSelectBitstream, length, blocknumber=0;
	pBinaryInRam=EngGetPsocRamCopy(&length);
	pEndBinaryInRam =		pBinaryInRam + length;

	if(!EngAcquirePsoc())
		xputstring("Acquire FAILED while starting to ERASE.\r\n");
	// Erase last block first in case process fails, Good PSoC code is indicated by valid check sum in last Block.
	BlockSelectBitstream = 0x04fa000 + (BLOCK_NUM_61<<4) + padding;	// prefix + blocknumber + padding
	EngSend22bitStreamToPsoc(BlockSelectBitstream);
	EngSendVectorArrayToPsoc(pEraseBlock);
	if (!EngWaitForSdataLowPsoc())			return FAIL;
	RESET_WATCHDOG_TIMER();
	do
	{
		for (unsigned int address = SRAM_Offset ; address < (SRAM_Offset+ENG_PSOC_BLOCK_SIZE) ; )	// send blocks 64 bytes in length
		{
			if (pBinaryInRam  <  pEndBinaryInRam)
				EngWriteByteToSramPsoc(*(pBinaryInRam++), address++);
			else
				EngWriteByteToSramPsoc(0x00, address++); // Fill block with zero's this will keep psoc in rs232 mode after a flash
		}
		BlockSelectBitstream = 0x04fa000 + (blocknumber<<4) + padding;	// prefix + blocknumber + padding
		EngSend22bitStreamToPsoc(BlockSelectBitstream);
		EngSendVectorArrayToPsoc(pEraseBlock);
		if (!EngWaitForSdataLowPsoc())			return FAIL;
		RESET_WATCHDOG_TIMER();
	} while (++blocknumber < (BLOCK_NUM_61 + 1));
	RetResult = PASS;

	return RetResult;
} //EraseEnginePsoc(void)

//-------------------------------------------------------------------------
//
//		INITIALIZE ENGINE PSoC COM LINES
//
//-------------------------------------------------------------------------
unsigned int EngSetComLines(unsigned int action)
{
	static int sdataGIUS,  sclkGUIS;  // Variables to save com lines state
	DISABLE_INTERRUPTS();
	if (action==EngTakeControl)
	{
		ENG_SAVE_COM_LINES_STATE;
		ENG_MAKE_PSOC_LINES_GPIO;
		ENG_PSOC_XRES_LOW;							// Initialize communication lines
		ENG_PSOC_XRES_DIR_OUT;
		ENG_PSOC_SDATA_LOW;
		ENG_PSOC_SDATA_DIR_IN;
		ENG_PSOC_SCLK_LOW;
		ENG_PSOC_SCLK_DIR_OUT;
	}
	else
		ENG_RESTORE_COM_LINES;
	RESTORE_INTERRUPTS();
	return 1;
}

//-------------------------------------------------------------------------
//
//			ACQUIRE ENGINE PSOC
//
//-------------------------------------------------------------------------
unsigned int EngAcquirePsoc(void)
{
	int retval=0;
	while (true)
	{
		DISABLE_INTERRUPTS();
		EngResetPsoc();
		EngSendVectorArrayToPsoc(pInitialize1);	// Must begin in less than 100us after the reset pulse
		RESTORE_INTERRUPTS();
		if (!EngWaitForSdataLowPsoc()){xputstring ("EngAcquirePsoc()__1\r\n"); 	break;}
		retval++;
		EngSendVectorArrayToPsoc(pInitialize2);
		if(!EngWaitForSdataLowPsoc()){xputstring ("EngAcquirePsoc()__2\r\n"); 	break;}
		retval++;
		if(EngSendVectorArrayToPsoc(pInitialize3))
			retval++;
		break;
	}
   return retval==3;
}// EngAcquirePsoc(void)

//-------------------------------------------------------------------------
//
//			RESET ENGINE PSOC
//
//-------------------------------------------------------------------------
void EngResetPsoc(void)
{
	DISABLE_INTERRUPTS();
	ENG_PSOC_XRES_HIGH;		// Enter reset mode
	RESTORE_INTERRUPTS();

	wait.us(30);
	while(!wait.HasExpired());	// Should be at least 10us

	{	// Note:  The IRQ macros create a local variable.  Because we use them twice, we need to localize one set so it compiles (duplicate variable error).
		DISABLE_INTERRUPTS();
		ENG_PSOC_XRES_LOW;		// Exit reset mode
		RESTORE_INTERRUPTS();
	}

	wait.us(30);
	while(!wait.HasExpired());	// Should be at least 20us before attempting to aquire
							// Note to aquire psoc, must begin sending the Initialize1 vector within 100us after sending this pulse.
}

//-------------------------------------------------------------------------
//
//			VERIFY ID ENGINE PSOC
//
//-------------------------------------------------------------------------
unsigned int EngVerifyIdPsoc()
{
	EngSendVectorArrayToPsoc(pIdSetup);
	if (!EngWaitForSdataLowPsoc()) return FAIL;
	return (EngReadIdPsoc());
}

//-------------------------------------------------------------------------
//
//			READ ID ENGINE PSOC
//
//-------------------------------------------------------------------------
unsigned int EngReadIdPsoc(void)
{
	unsigned int b1,b2;
	ReadPrefix = 0x0500;
	b1=EngReadByteFromSramPsoc(READ_PSOC_ID1);
	b2=EngReadByteFromSramPsoc(READ_PSOC_ID2);
	if(DEBUG)
	{
		sprintf (MsgBuf,"PSOC ID = %02X\r\n",(b1<<8)+b2);
		xputstring(MsgBuf);
	}
	return ((b1<<8)+b2);
}

//-------------------------------------------------------------------------
//
//			PROGRAM FLASH ENGINE PSOC
//
//-------------------------------------------------------------------------
unsigned int EngProgramFlashPsoc(void)
{
	const unsigned char *pBinaryInRam;
	const unsigned char *pEndBinaryInRam;
	unsigned int length, blocknumber=0;
	unsigned int BlockSelectBitstream, nhexDataOffset = 0;
	pBinaryInRam=EngGetPsocRamCopy(&length);
	pEndBinaryInRam =		pBinaryInRam + length;
	if( length >( (nNumBlocksInPSoC*(ENG_PSOC_BLOCK_SIZE + HEXFILE_OVERHEAD) )+ EXTRA_INFO ) )
	{
		xputstring ("Binary to flash is too large for this device ... ");
		return FAIL;
	}
	if(DEBUG)
		xputstring ("Programming PSOC ... ");
	nhexDataOffset = 5;	// Data in the Intel Hex File Format Array starts after the first 5 bytes.
	nNumBlocksInPSoC -= 2;	// Don't Prog the last 2 blocks if we are a 24xxx PSoC engine.
	do
	{
		for (unsigned int address = SRAM_Offset ; address < (SRAM_Offset+ENG_PSOC_BLOCK_SIZE) ; )	// send blocks 64 bytes in length
		{
			if (pBinaryInRam  <  pEndBinaryInRam)
				EngWriteByteToSramPsoc(*(nhexDataOffset + pBinaryInRam++), address++);
			else
				EngWriteByteToSramPsoc(0x00, address++); // Fill block with zero's this will keep psoc in rs232 mode after a flash
		}
		pBinaryInRam+=nhexDataOffset+1;		// Skip past the Intel EOL check sum and to the next record of data.
		BlockSelectBitstream = 0x04fa000 + (blocknumber<<4) + padding;	// prefix + blocknumber + padding
		EngSend22bitStreamToPsoc(BlockSelectBitstream);
		EngSendVectorArrayToPsoc(pProgramBlock);
		if (!EngWaitForSdataLowPsoc())			return FAIL;
		RESET_WATCHDOG_TIMER();
	} while (++blocknumber < nNumBlocksInPSoC);
	nNumBlocksInPSoC += 2;	// Restore 24xxx PSoC engine memory size.
	if(DEBUG)
		xputstring ("Pass.\r\n");
	return PASS;
}

//-------------------------------------------------------------------------
//
//			VERIFY FLASH ENGINE PSOC
//
//-------------------------------------------------------------------------
unsigned int EngVerifyFlashPsoc(void)
{
	unsigned int blocknumber = 0;
	unsigned int BlockSelectBitstream;
//	unsigned char PsocFlashArray[BLOCKS_IN_24x_PSOC*ENG_PSOC_BLOCK_SIZE];
	unsigned char MX1FlashArray[BLOCKS_IN_24x_PSOC*ENG_PSOC_BLOCK_SIZE];
	unsigned int j=0;
	unsigned char psocramvalue,romvalue;
	unsigned int returnstatus= PASS;
	unsigned int length;
	const unsigned char * Rom=EngGetPsocRamCopy(&length);

	do
	{
		BlockSelectBitstream = 0x04fa000 + (blocknumber<<4) + 0x0E;	// prefix + blocknumber + zero padding
		EngSend22bitStreamToPsoc(BlockSelectBitstream);
		EngSendVectorArrayToPsoc(pVerifySetup);
		if (!EngWaitForSdataLowPsocSlow()) return 0;	 // For some reason this wait needs to be done at the lower clock speed
		for (unsigned int address = SRAM_Offset ; address < (SRAM_Offset+ENG_PSOC_BLOCK_SIZE) ;address ++)	// read 64 byte block
		{
			psocramvalue=EngReadByteFromSramPsoc(address);
	//		PsocFlashArray[j]=psocramvalue;
			if (j < length)
				romvalue=Rom[j];
			else
				romvalue=0;
			MX1FlashArray[j]=romvalue;

			if (psocramvalue!=MX1FlashArray[j])
			{
				if(DEBUG && blocknumber==0)//DMH && returnstatus
				{
					sprintf(MsgBuf,"block %d  byte %d  expected %#04x  received %#04x\r\n", blocknumber, (address)-SRAM_Offset, (Rom[j]),psocramvalue);
					xputstring(MsgBuf);
				}
					returnstatus=FAIL;
			}
			j++;
		}
		RESET_WATCHDOG_TIMER();
	} while (++blocknumber < nNumBlocksInPSoC);

	if(DEBUG)
	{
	  	if (returnstatus)
			sprintf (MsgBuf,"Pass.\r\n");
	  	else
			sprintf (MsgBuf,"fail.\r\n");
	  xputstring(MsgBuf);
	}
	return returnstatus;
}// verify_flash_psoc(void);

//-------------------------------------------------------------------------
//
//			SECURE ENGINE PSOC
//
//-------------------------------------------------------------------------
unsigned int SecureEnginePsoc(const unsigned int *pSecurityArray)
{
	unsigned int address = SRAM_Offset ;	// 64 byte block
	do{
		EngWriteByteToSramPsoc( pSecurityArray[address], address ); // Set each blocks protection level
	}while(++address < (SRAM_Offset + (BLOCKS_IN_24x_PSOC)) );
	EngSendVectorArrayToPsoc(pSecureBlock);
	return EngWaitForSdataLowPsoc();
}

//-------------------------------------------------------------------------
//
//			VERIFY FLASH CHECKSUM ENGINE PSOC
//
//-------------------------------------------------------------------------
unsigned int EngVerifyFlashChecksumPsoc(void)
{
	char str[128];
	unsigned char cReadByte;
	unsigned int nCalculatedRAMchecksum=0, nPsocAppCheckSum=0, nRegesterChkSum=0;
	unsigned int blocknumber = 62;
	unsigned int BlockSelectBitstream;
	SRAM_Offset = 0x80;
	do
	{
		BlockSelectBitstream = 0x4fa00E + (blocknumber<<4);	// prefix + blocknumber + padding
		if(DEBUG)
		{
			sprintf (str,"BlockSelectBitstream =  %#08x\r\n", BlockSelectBitstream );//DMH
			xputstring(str);//DMH
		}
		EngSend22bitStreamToPsoc(BlockSelectBitstream);
		EngSendVectorArrayToPsoc(pVerifySetup);
		if (!EngWaitForSdataLowPsocSlow()) return 0;	 // For some reason this wait needs to be done at the lower clock speed

		if(DEBUG)
		{
			sprintf (str,"Block %d:\r\n", blocknumber );//DMH
			xputstring(str);//DMH
		}
		for(unsigned int address = SRAM_Offset; address < (SRAM_Offset+ENG_PSOC_BLOCK_SIZE); )	// read 64 byte blocks
		{
			cReadByte = EngReadByteFromSramPsoc(address++);
			if(DEBUG)
			{
				sprintf (str,"%02X ", cReadByte );
				xputstring(str);
			}
			nRegesterChkSum += cReadByte;
		}
		RESET_WATCHDOG_TIMER();
	} while (++blocknumber < nNumBlocksInPSoC);
	SRAM_Offset = 0;

	if(!EngSendVectorArrayToPsoc(pChecksumSetup))
		xputstring("Checksun Setup FAILED!!\r\n");
	if (!EngWaitForSdataLowPsoc())
	{
		xputstring("Checksun Setup FAILED!!\r\n");
		 return 0;
	}

	nPsocAppCheckSum = EngReadFlashChecksumPsoc();

	if(DEBUG)
	{
		sprintf (str,"nPsocAppCheckSum in ROM = %#06X\r\n", nPsocAppCheckSum );//DMH
		xputstring(str);//DMH
		sprintf (str,"nRegesterChkSum = %#06X\r\n", nRegesterChkSum );//DMH
		xputstring(str);//DMH
	}

	nPsocAppCheckSum -= nRegesterChkSum;

	if(DEBUG)
	{
		sprintf (str,"nPsocAppCheckSum in ROM Bk0-> Bk61 = %#06X\r\n", nPsocAppCheckSum );//DMH
		xputstring(str);//DMH
	}
	nCalculatedRAMchecksum = EngCalculateFlashChecksumPsoc();
	if (nPsocAppCheckSum != nCalculatedRAMchecksum)
	{
		if(DEBUG)
		{
			sprintf (str,"Calculated RAM checksum = %#06x\r\nFlash ROM - Reg's =       %#06x\r\n", nCalculatedRAMchecksum, nPsocAppCheckSum );
			xputstring(str);
		}
		return 0;
	}
	if(DEBUG)
		xputstring("verify_flash_checksum_psoc()  PASSED!!!\r\n");
	return 1;
}// EngVerifyFlashChecksumPsoc();

//-------------------------------------------------------------------------
//
//			SEND VECTOR ARRAY TO ENGINE PSOC
//
//-------------------------------------------------------------------------
unsigned int EngSendVectorArrayToPsoc(const unsigned int vectorArray[])
{
	unsigned int i=0;
	while (vectorArray[i]!=1)			// End of array marked by 1
	{
		if (vectorArray[i]>0x7ffffe)	//Is the value in the array illegal (> 22 bits)?
		{
			if(DEBUG)
				xputstring("vector value in the array illegal (> 22 bits)\r\n");
			return 0;					// Vector ERROR!
		}
		EngSend22bitStreamToPsoc(vectorArray[i++]);
	}
	return 1;
}

 //-------------------------------------------------------------------------
//
//			SEND BIT STREAM TO ENGINE PSOC uses mask to change length
//
//-------------------------------------------------------------------------
static void send_bit_stream_to_psoc(unsigned int bitstream,	unsigned int Mask)
{
	DISABLE_INTERRUPTS();
	ENG_PSOC_SDATA_DIR_OUT;
	RESTORE_INTERRUPTS();
	do
	{
		if (bitstream & Mask)	// set sdata
		{
			DISABLE_INTERRUPTS();
			ENG_PSOC_SDATA_HIGH;
			RESTORE_INTERRUPTS();
		}
		else
		{
			DISABLE_INTERRUPTS();
			ENG_PSOC_SDATA_LOW;
			RESTORE_INTERRUPTS();
		}
		DISABLE_INTERRUPTS();
		ENG_PSOC_SCLK_HIGH;			// and clock it (valid on falling edge)
		RESTORE_INTERRUPTS();
		HALF_BIT_DELAY;
		{
			DISABLE_INTERRUPTS();
			ENG_PSOC_SCLK_LOW;
			RESTORE_INTERRUPTS();
		}
		HALF_BIT_DELAY;
	} while ((Mask >>= 1)>1);	// Shift right and skip last padding bit
	{
		DISABLE_INTERRUPTS();
		ENG_PSOC_SDATA_LOW;
		ENG_PSOC_SDATA_DIR_IN;			// always leave sdata in high z state
		RESTORE_INTERRUPTS();
	}
}

//-------------------------------------------------------------------------
//
//			SEND 22BIT STREAM TO ENGINE PSOC
//
//-------------------------------------------------------------------------
void EngSend22bitStreamToPsoc(unsigned int bitstream)
{
	send_bit_stream_to_psoc(bitstream,0x0400000); // Send 22 bits of command and skip the last padding bit, see below
}

//-------------------------------------------------------------------------
//
//			SEND 11 BIT STREAM TO ENGINE PSOC
//	This function is inline to minimize latency between writing and reading the psoc
//
//-------------------------------------------------------------------------
inline void send_11bit_stream_to_psoc(unsigned int bitstream)
{
	int mask=0x0400;
	DISABLE_INTERRUPTS();
	ENG_PSOC_SDATA_DIR_OUT;
	while (mask)
	{
		if (bitstream & mask) 	// set sdata
		{
			ENG_PSOC_SDATA_HIGH;
		}
		else
		{
			ENG_PSOC_SDATA_LOW;
		}
		ENG_PSOC_SCLK_HIGH;			// and clock it (valid on falling edge)
		HALF_BIT_DELAY;
		ENG_PSOC_SCLK_LOW;
		HALF_BIT_DELAY;
		mask>>=1;
	}
	ENG_PSOC_SDATA_LOW;
	ENG_PSOC_SDATA_DIR_IN;		   // always leave sdata in high z state
	RESTORE_INTERRUPTS();
}

//-------------------------------------------------------------------------
//
//			READ 8BIT STREAM FROM ENGINE PSOC
//	This function is inline to minimize latency between writing and reading the psoc
//
//-------------------------------------------------------------------------
inline unsigned int EngRead8bitStreamFromPsoc(void)
{
	unsigned int b=0;
	int count=8;
	DISABLE_INTERRUPTS();
	ENG_HIGH_Z_CYCLE_ENTERING_READ;   // clock does /^\_/^
	RESTORE_INTERRUPTS();
	while (count--)
	{
		DISABLE_INTERRUPTS();
		ENG_PSOC_SCLK_LOW;					//  clock it (valid on rising edge)
		HALF_BIT_DELAY_IN_READ_MODE;
		ENG_PSOC_SCLK_HIGH;			//  clock it (valid on rising edge)
		RESTORE_INTERRUPTS();
		b = ( b << 1 ) + (unsigned int)ENG_PSOC_SDATA_READ;
		HALF_BIT_DELAY_IN_READ_MODE;
	}
	{
		DISABLE_INTERRUPTS();
		ENG_HIGH_Z_CYCLE_LEAVING_READ;	  // clock does \_/^\_
		RESTORE_INTERRUPTS();
	}
	return b;
}

//-------------------------------------------------------------------------
//
//			WAIT FOR SDATA LOW ENGINE PSOC
//
//-------------------------------------------------------------------------
unsigned int EngWaitForSdataLowPsoc()
{
	volatile unsigned long i=0;
	DISABLE_INTERRUPTS();
	ENG_PSOC_SDATA_DIR_IN;	  // should always be left high z but just to be sure
	while (ENG_PSOC_SDATA_READ == LOW)
	{
		DISABLE_INTERRUPTS();
		ENG_CYCLE_SCLK;
		RESTORE_INTERRUPTS();
		if ( i++ > 500 )
		{
			RESTORE_INTERRUPTS();
			return 0;			// Exact number of cycles required unknown
		}
		RESET_WATCHDOG_TIMER();
	}
	RESTORE_INTERRUPTS();
	i=0;
	while (ENG_PSOC_SDATA_READ == HIGH)
	{
		DISABLE_INTERRUPTS();
		ENG_CYCLE_SCLK;
		RESTORE_INTERRUPTS();
		if ( i++ > 1500000 )		// After the vector to generate the checksum the PSOC requires at least 1.5 million cycles
			return 0;
		RESET_WATCHDOG_TIMER();
	}
	for(i=1;i<100;i++)		// Exact number of cycles required unknown
	{								// extra cycles needed for some reason ???
		DISABLE_INTERRUPTS();
		ENG_CYCLE_SCLK;
		RESTORE_INTERRUPTS();
	}
	return 1;
}

//-------------------------------------------------------------------------
//
//			WAIT FOR SDATA LOW ENGINE PSOC SLOW
//
// For some reason ??? we need this slower clocking when waiting for the PSOC to do a Block Read
//
//-------------------------------------------------------------------------
unsigned int EngWaitForSdataLowPsocSlow()
{
	unsigned long i=0;
	ENG_PSOC_SDATA_DIR_IN;	  		// should always be left high z but just to be sure
	while (ENG_PSOC_SDATA_READ == LOW)
	{
		DISABLE_INTERRUPTS();
		ENG_CYCLE_SCLK;
		RESTORE_INTERRUPTS();
		if ( i++ > 50 )
		{
			if(DEBUG)
			{
				sprintf (MsgBuf,"SDATA failed to go high ... ");
				xputstring(MsgBuf);
			}
			return 0;			// Exact number of cycles required unknown
		}
		RESET_WATCHDOG_TIMER();
	}
	i=0;
	while (ENG_PSOC_SDATA_READ == HIGH)
	{
		DISABLE_INTERRUPTS();
		ENG_CYCLE_SCLK_SLOW;
		RESTORE_INTERRUPTS();
		if ( i++ > 30000 )  // Exact number of cycles required unknown
		{
			if(DEBUG)
			{
				sprintf (MsgBuf,"SDATA failed to go low ... ");
				xputstring(MsgBuf);
			}
			return 0;
		}
		RESET_WATCHDOG_TIMER();
	}
	for(i=1;i<100;i++)		// Exact number of cycles required unknown
	{								// extra cycles needed for some reason ???
		DISABLE_INTERRUPTS();
		ENG_CYCLE_SCLK;
		RESTORE_INTERRUPTS();
	}
	return 1;
}

//-------------------------------------------------------------------------
//
//			WRITE BYTE TO SRAM ENGINE PSOC
//
//-------------------------------------------------------------------------
unsigned int EngWriteByteToSramPsoc(unsigned int data, unsigned int address)
{
	unsigned int vector;
	vector = WritePrefix + (address<<12) + (data<<4) + padding;	// write prefix b100 + address + data + zero padding
	EngSend22bitStreamToPsoc(vector);
	return 1;
}

//-------------------------------------------------------------------------
//
//			READ BYTE FROM SRAM ENGINE PSOC
//
//-------------------------------------------------------------------------
unsigned int EngReadByteFromSramPsoc(unsigned int address)
{
	unsigned int bite, vector;
	vector=ReadPrefix + address;	// add read prefix
	send_11bit_stream_to_psoc(vector);
	bite = EngRead8bitStreamFromPsoc();
	return bite;
}

//-------------------------------------------------------------------------
//
//			READ FLASH CHECKSUM ENGINE PSOC
//
//-------------------------------------------------------------------------
unsigned int EngReadFlashChecksumPsoc(void)
{
	unsigned int b1, b2;
	EngSendVectorArrayToPsoc(pChecksumSetup);
	if (!EngWaitForSdataLowPsoc()) return 0;
	send_11bit_stream_to_psoc(READ_CHECKSUM_BYTE1);	 	// send read stream
	b1=EngRead8bitStreamFromPsoc();		  					// read stream
	send_11bit_stream_to_psoc(READ_CHECKSUM_BYTE2);	 	// send read stream
	b2=EngRead8bitStreamFromPsoc();		  					// read stream
	return (b1<<8) + b2;
}

//-------------------------------------------------------------------------
//
//			CALCULATE FLASH CHECKSUM ENGINE PSOC
//
//-------------------------------------------------------------------------
unsigned int EngCalculateFlashChecksumPsoc(void)
{
	unsigned int i, nBlock=0, checksum=0, length, index=0;
	const unsigned char * Rom;
	Rom = EngGetPsocRamCopy(&length);
	while (nBlock++<62)	//Don't include the EEPROM area in the check sum.
	{
		index+=5;	// advance past Intel Hex format (colon, record length, 2 address bytes, and record type)
		for(i=0; i<64; i++)	// Fetch the 64 bytes of app. code in this record.
			checksum += Rom[index++];	// accumulate the checksum!
		index++;	// advance past Intel Record checksum to the next Record.
	}
	checksum = checksum & 0x0000ffff; // Discard upper bytes
	return checksum;
}

//-------------------------------------------------------------------------
//
//			GET ENGINE PSoC RAM COPY
//
//	Returns a pointer to the array in Program RAM that contains ths PSoC code
//		we want to Verify/Flash and it returns the length of the array in a
//		pointer to an int we supply.
//
//-------------------------------------------------------------------------
const unsigned char * EngGetPsocRamCopy(unsigned int * length)
{
	*length = EngHexSize;
	return pEngHExInRam;
}


