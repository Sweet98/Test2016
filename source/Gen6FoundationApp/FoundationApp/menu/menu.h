/* ==============================================================================
   Matrix Menu Module Header
   ==============================================================================

   $RCSfile: Gen6FoundationApp/FoundationApp/menu/menu.h $
   $Revision: 1.6 $
   $Date: 2011/07/12 13:06:49EDT $

   ==============================================================================
   
   ============================================================================== */

#ifndef _MENU_H_
#define _MENU_H_


#include <stdio.h>
#include "language.h"

//#include	"db_varsc.h"		Needed for ReaderConfiguration
//#define	EXT	extern

#define TCC 					0
#define DLL 					0

#define MAX_LABEL_LENGTH				2800        /* size of the ASCII label buffer  */
#define CMDLEN							512		//MAX_LABEL_LENGTH           /* maximum size of a serial command */
#define DEBOUNCE						2              /* trigger switch debounce delay (10 to 20ms)     */
#ifndef SIZEOF_HIGHLEVEL_PARSE_BUFFER
#define SIZEOF_HIGHLEVEL_PARSE_BUFFER	MAX_LABEL_LENGTH * 3
#endif


#define	IS_MENU_CODE(p)	(p->CodeId == 0x82)
#define	IS_KCODE_MODE()	(bKcodeMode)
#define	KCODE_MODE(x)		(bKcodeMode=(x))

#define ToUpper(p) (((p >= 'a') && (p <= 'z')) ? (p - 32) : p)

//	A gaurd against using the NULL ReaderConfiguration ptr. Only shows up on debug builds
//	and is highly recommended -- at least once in a while.
//	The ternary operator contains an expression using a comma operator - see K&R C book for explanation
// 	THe CFG_BUILD symbol was added to provide debug an d non-denug builds for the product cfg becuase product config USES
//	the DEVELOPMENT_SETTING define.
#ifdef CFG_BUILD
	#ifdef CFG_DBUG
		#define GET_SETTING(p)  ((ReaderConfiguration != NULL) ?  ReaderConfiguration->p : (xputstring(#p": Null Rdr Cfg\r\n"),ReaderConfiguration->p) )
	#else
		#define GET_SETTING(p)  ReaderConfiguration->p
	#endif
#else
	#if defined(DEVELOPMENT_SETTINGS)
	void xputstring(const char *string);
		#define GET_SETTING(p)  ((ReaderConfiguration != NULL) ?  ReaderConfiguration->p : (xputstring("\r\n"#p": Null Rdr Cfg\r\n"),ReaderConfiguration->p) )
	#else
		#define GET_SETTING(p)  ReaderConfiguration->p
	#endif
#endif



#define GET_CONFIG_SETTING(p) ProductConfiguration->p


/*-------------------------------------------------------------------
// some macros
//-------------------------------------------------------------------*/
#define  CONTAINS_KCODE_DATA(pntr)	((pntr[0]=='Y')&&((pntr[2]=='Y')||(pntr[2]=='.')))  // pntr is type DecodedMsg_t
#define  CONTAINS_OCR_DATA(pntr)		(((pntr[0]=='Y')&&((pntr[2]=='Y')||(pntr[2]=='.')))&&((pntr[3]=='Y')&&((pntr[5]=='Y')||(pntr[5]=='.'))))


#define 	NEW_MAJORTAG_CHAR		';'
#define 	NEW_MAJORTAG_STR		";"
#define 	NEW_MINORTAG_CHAR		','
#define 	NEW_MINORTAG_STR		","
#define	QUERY_SETTINGS_WO_ACK_CHAR	'$'       /* active value query w/o ACK     */
#define	QUERY_SETTINGS_WO_ACK_STR	"$"
#define	QUERY_SETTINGS_CHAR	'?'       /* active value query     */
#define	QUERY_SETTINGS_STR	"?"
#define	QUERY_RANGE_CHAR		'*'       /* value range query      */
#define	QUERY_RANGE_STR		"*"
#define	QUERY_DEFAULT_CHAR	'^'       /* customer default / standard default query */
#define	QUERY_DEFAULT_STR		"^"

#define	RANGE_SEPARATOR_STR	"-"
#define	LIST_SEPARATOR_STR	"|"
#define  LIST_SEPARATOR_CHAR	'|'
#define	ROM_TERMINATOR_CHAR	'.'       /* working table / customer default copy terminator */
#define	ROM_TERMINATOR_STR	"."
#define	RAM_TERMINATOR_CHAR	'!'       /* Active (RAM) copy terminator */
#define	RAM_TERMINATOR_STR	"!"
#define	STANDARD_DEFAULTS_TERMINATOR_CHAR	'&'
#define	STANDARD_DEFAULTS_TERMINATOR_STR	"&"
#define	INVALID_TAG_CHAR		ENQ
#define	INVALID_DATA_CHAR		NAK
#define	VALID_TAGDATA_CHAR	ACK
#define	VALID_QUERY				0x03
#define 	BAD_TAGBUFFER			-1
#define 	NO_KCODE_DATA  	 	(UINT)0xffff
#define 	MAJOR_TAGSIZE 			3
#define	MINOR_TAGSIZE 			3
#define	TAG_SIZE					(MAJOR_TAGSIZE+MINOR_TAGSIZE) 	//null terminator space added for CPP compile
#define  MAX_DATA_BLOCK_SIZE	500
#define	DFM_BLOCK_TAG        "080300"
#define BYTE_T char

extern	BOOL			bKcodeMode;

/*----------------------------------------------------------------------
// These MACROS are for accessing the RAM copy of the settings table.
// For TCC and DLL builds we DON'T copy the settings table to 'RAM' so
// we need to redefine where RAM_ENTRY resolves to.
//----------------------------------------------------------------------*/
#define	RAM_SET_ACTUAL(p,n)			ReaderConfiguration->p = n

/*-------------------------------------------------------------------
// Structure of the menu code message passed to the menu processing
//-------------------------------------------------------------------*/
#define MENU_RESPONSE_LEN      2048						/* was 128 */
typedef struct
{
	char     tag[MAJOR_TAGSIZE+MINOR_TAGSIZE+1];    	/* the 'tag' to search for */
	BOOL		bQueryMinor;
	BOOL		bQueryMajor;
	BOOL		bQueryAll;
	char		QueryChar;
	char	   data[MENU_RESPONSE_LEN];						/* the data: a flag or kcode data */
} MenuMsg_t;

/*--------------------------------------------------------------------------------------
//	A Kcode Digit is a number from 0-F, and is entered individually by scanning KxK.
//
//	A Kcode ITEM is a set of kcode digits comprising one entry into the setting's value
//	fields.
//
// For example, code 128 min length has up to 2 kcode digits (0-80), but only
// has ONE entry item (the length value).
//
//	Prefix/Suffix/Data Formatter settings consist of multiple entry items, and since some
// of the items will consist of more than two kcode digits formatted kcode data entry
// should be used in their case.
//
//---------------------------------------------------------------------------------------*/

typedef	struct
{
	UINT	nBase;        						/* 10 or 16 */
	UCHAR	nBuffer[MAX_DATA_BLOCK_SIZE]; /* running storage of incoming kcode values */
}	Kcode_t;


// When entering kcodes we can terminate the kcodes with another menu code.  When that menu code is entered though
// it might be saving to different memory (RAM/ROM) than the last one did.  bMenuRam gets changed the instant that
// a new menu symbol is entered, but since the new menu symbol gets processed before the Kcode data from the previous
// symbol gets saved, we need to know what kind of memory the previous symbol should be saved to.
extern	Kcode_t		Kcode;
extern	MenuMsg_t *	pTagBuffer;

	// When we are entering Kcodes, somebody might scan either a MNUSAV (menu save), or a MNUABT (menu abort), or
	// anything else for that matter.  Well, we don't know what they are going to scan so we need to process their
	// new menu symbol (if it is one) before we decide to save the Kcode data.  This pointer will hold the RAM
   // location of the Entry we need to store the Kcode data to while we process the new menu symbol.

/*-------------------------------------------------------------------
//  Menu Utility Function Prototypes and global var's
//-------------------------------------------------------------------*/
extern unsigned BYTE_T Menu(unsigned int *puiLength, unsigned char *pucMessage);
extern int EnterScannedMenu(unsigned int *puiLength, unsigned char *pucMessage);
bool ForwardPrefixedCommand(bool bIsMenuCommand, const unsigned char *pucBuffer, unsigned int ui, bool * pbCommandWasForwarded);
void IndicateCustomDefaultProgrammingMode( void );
bool CheckSubTagQueryCommand( char *pucMessage, bool RemoveSubQueryCommand);

/*----------------------------------------------------------------------------
// Some BASIC-like string handling macros
//----------------------------------------------------------------------------*/
#define	RIGHT(p,x)			((char*)(p))[strlen((char*)(p))-x]
#define	MID(p,x)				((char*)(p))[x]
#define	APPEND_CHAR(p,ch)	do {							  					\
										int _x = strlen((char*)(p)); 			\
										((char*)(p))[_x] = (UCHAR)ch;			\
										((char*)(p))[_x+1] = 0;					\
									} while(0)




#define	PRO_RECORD			0
#define	PRO_ACKNAK_REC		1
#define	PRO_ACKNAK_BLK		2
#define	PRO_BURST_BLK		3

#define 	PLAIN_TRM_RS232	0
#define	TO_HOST_TERM		1
#define	TO_HOST				2
#define	TO_TERM				3

#define	LAY_NORMAL			0
#define 	LAY_TELE				1
#define	LAY_CALCULATOR		2
#define	LAY_ALPHA			3
#define	LAY_NUMERIC			4

#define	DFM_DISABLE			0
#define	DFM_ENABLE			1
#define	DFM_REQUIRED		2

#ifndef WINSIM	                           // name clash with MS
#define	BAUD_300			0
#define	BAUD_600			1
#define	BAUD_1200			2
#define	BAUD_2400			3
#define	BAUD_4800			4
#define	BAUD_9600			5
#define	BAUD_19200			6
#define	BAUD_38400			7
#define	BAUD_57600			8
#define	BAUD_115200			9
#define	BAUD_230400			10
#define	BAUD_460800			11
#define	BAUD_921600			12
#endif


#endif    /* _MENU_H_ */



  
