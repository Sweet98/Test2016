/* ==============================================================================
   Prokey table Module Source
   ==============================================================================
 
   $RCSfile: interfaces_wedge/prokeytb.cpp $
   $Revision: 2.4 $
   $Date: 2009/12/10 17:15:13EST $
 
   ==============================================================================

   ============================================================================== */

#include "stdInclude.h"
#include "ProkeyTb.h"
#include "hstring.h"
#include "startupFlashdisk.h"

/*
	WE NEED A BINARY COMPATIBLE STRUCTURE HERE!
 
	This table is just a dummy so that a PC tool can find it and tell the user about the new method.
	We now use a flash file.
 */

#define DummyTableFormat 116				// Dummy format, we now use a flash file
#define DUMMY_PROKEY_TABLE_SIZE 10		// without header

#define 	PROKEY_TABLE_TOTALSIZE	MAGICSIZE+1+DUMMY_PROKEY_TABLE_SIZE	// total size including header and format
#define  CHECKSUM (DUMMY_PROKEY_TABLE_SIZE+0x0003+0x0003+0x0003)		// manually calculated checksum

const unsigned char DummyProkeyTable[PROKEY_TABLE_TOTALSIZE] =
    {
        'P','r','o','k','e','y','T','a','b','l','e','D','M','F',0,  		// a magic string
        DummyTableFormat,																	// Table format
        (DUMMY_PROKEY_TABLE_SIZE>>8)&0xFF,DUMMY_PROKEY_TABLE_SIZE&0xFF,	// Size
        0x00,0x03,																			// Offset to Sequence table
        0x00,0x03,																			// Offset to Replace table
        0x00,0x03,																			// Offset to user string
        (CHECKSUM>>8)&0xFF,CHECKSUM&0xFF,											// Checksum over directory

        //	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
        //	                                                       0x00,							// Sequences
        //	                                                            0x00,						// Replacements
        //	                                                                 0x00,				// User string
    };

// The ARM linker is so smart, it removes the above table when we not use it. (even marked as external).
// I did not want to mess with pragmas....
// So I changed my plans and the LoadProkeyFile() does return the address of the DummyProkeyTable when there is no
// flash file. Originally I wanted to return NULL.

static const char *ProkeyFileName  = "Prokey";

size_t ReadProkeyFileFromDisk(const UCHAR *&pTable)
{
	static unsigned char *FlashProkeyTbl=NULL;
	pTable = DummyProkeyTable;
	int FileSize=0;	
	FileSize=ReadAndCloseFile(ProkeyFileName,0,&FlashProkeyTbl);
	if (FileSize>=0)
	{
		pTable=FlashProkeyTbl;
	}
//	if(flash_debug_on())
//	{
//		sprintf(ResultString, "Edit script read status = %d\r\n", ReadStatus);
//		xputstring(ResultString);
//	}
	return (FileSize<0)?0:FileSize;
}

const UCHAR *LoadProkeyFile(void)
{
	const UCHAR *pTable=DummyProkeyTable;
	size_t FileSize = ReadProkeyFileFromDisk(pTable);

	// now search the beginning of the table inside the file.
	// We use DummyProkeyTable as the search pattern because it contains the magic string in the first place.
	// Also we want to avoid two magic strings in the firmware.
	HSTRING sTemp(pTable, FileSize);
	int Position;
	if (FileSize && sTemp.Find(DummyProkeyTable, MAGICSIZE, true, 1, &Position))
	{
		pTable += Position;
	}
	return pTable;
}

/*
 * How it works:
 * The table above is empty, that means there are no keyboard remappings.
 * The directory contains offsets to 3 different types of information:
 * Replacements	- Contains keynumbers which will be used for certain characters.
 * Sequences		- Contains sequences of bytes which are sent to the terminal unchanged.
 * User string		- This can be used to store some human readable information. Not read by the scanner.
 *
 * This table can be searched from the outside by a PC program (prokey.exe). It uses the
 * magic string and the checksum over the directory structur to savely find this table inside
 * the hex file.
 * Prokey.exe writes data into this table to remap characters to different keys. There is a document
 * descibing the usage of prokey.exe.
 * */

/*
Format of the table:
	Replacements:
	{
		UCHAR	TermID;
		UCHAR CountryID;
		UCHAR NumKeys;
		NumKeys amount of these:
		{
			tchar_t	character;
			UCHAR		keynumber;
			UCHAR		modifier;
		}
 	}
	TermID==0 terminates the table. Otherwise the next terminal follows.
 
	Sequence:
	{
		UCHAR	TermID;
		UCHAR CountryID;
		{
			UCHAR		NumBytes;               // including NumBytes and character
			tchar_t	character;
			UCHAR		Scancodes[NumBytes-2];
		}
		NumBytes ==0  terminates the sequence
 	}
	TermID==0 terminates the table. Otherwise the next terminal follows.
 */



