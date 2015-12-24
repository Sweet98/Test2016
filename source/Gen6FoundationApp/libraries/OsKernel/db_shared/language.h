/*----------------------------------------------------------------------*
 *	"LANGUAGE.H" -- Non compiler specific type definition MACROS
 * Note:  Compiler specific belong in compiler.h and platform specific belong in platform.h
 *----------------------------------------------------------------------*
 * $RCSfile: db_shared/language.h $
 * $Revision: 1.7.1.2 $
 * $Date: 2009/01/26 15:11:37EST $
 *
 *
 *----------------------------------------------------------------------*/
//! \file
#ifndef _INC_LANGUAGE
  #define _INC_LANGUAGE

#include "platform.h"        /* determine which platform we're compiling to */

// Non compiler specific stuff  ... just easier on the typing fingers
#ifndef SINT
typedef  signed int     		SINT;
#endif

#ifndef UINT
typedef  unsigned int   		UINT;
#endif

#ifndef SCHAR
typedef  signed char    		SCHAR;
#endif

#ifndef UCHAR
typedef  unsigned char  		UCHAR;
#endif

typedef  unsigned long  		ULONG;
typedef  unsigned short 		USHORT;

#define VOID void
typedef char CHAR;
typedef short SHORT;
typedef 	unsigned long   		DWORD;
typedef int                 	BOOL;
typedef unsigned char       	BYTE;
typedef unsigned short      	WORD;
typedef 	DWORD * 					PDWORD;

#ifndef WINSIM	// Windows defines this as an u long rather than a u int. We should change it. Meanwhile...
typedef 	unsigned int   		HLOCAL;
typedef 	unsigned int 			HANDLE;
#endif
typedef 	void * 					LPVOID;
typedef 	unsigned char * 		PBYTE;
typedef 	int * 					PBOOL;

typedef unsigned long threadhandle_t;
typedef ULONG timer_ticks_t;						//!< Use this for timer ticks
typedef ULONG timer_uSec_t;						//!< Use this for MicroSec values
typedef ULONG timer_mSec_t;						//!< Use this for MilliSec values

const timer_uSec_t MilliSec = 1000;				//!< A Millisecond with 1 uSec resolution
const timer_uSec_t Second	 =	1000*MilliSec; //!< A Second with 1 uSec resolution

/*-------------------------------------------
 * Convenient declarations
 *-------------------------------------------*/

#ifndef TRUE
#define 	TRUE				1
#define 	FALSE				0
#endif

#define 	YES				TRUE
#define 	NO 				FALSE

#ifndef PASS
#define  PASS				TRUE
#endif

#ifndef FAIL
#define	FAIL				FALSE
#endif

/*-------------------------------------------
 * Handy MACROs
 *-------------------------------------------*/
#define 	BIT(n)				(ULONG)((ULONG)0x00000001 << (ULONG)(n))
#define	MAX(a,b)				((a) >= (b) ?  (a) : (b))
#define	MIN(a,b)				((a) <= (b) ?  (a) : (b))
//#define 	TWIX(a,b,c)			(((a)<=(c))&&((c)<=(b)))
//#define 	DIGIT(c)				TWIX('0','9',(c))
//#define	ALPHA(c)				TWIX('A','Z',(c))
#define  HEX(x)				((x) - (((x) > '9') ? 0x37 : 0x30))

// Some BASIC-like string handling macros
#define	RIGHT(p,x)			((char*)(p))[strlen((char*)(p))-x]
#define	MID(p,x)				((char*)(p))[x]
#define	APPEND_CHAR(p,ch)	do {							  					\
										int _x = strlen((char*)(p)); 			\
										((char*)(p))[_x] = (UCHAR)ch;			\
										((char*)(p))[_x+1] = 0;					\
									} while(0)

/*-------------------------------------------
 * ASCII character definitions
 *-------------------------------------------*/
#ifndef SOH
#define   NUL  0x00
#define	SOH	0x01
#define	STX	0x02
#define	ETX	0x03
#define	EOT	0x04
#define	ENQ	0x05
#define	ACK	0x06
#define	BEL	0x07
#define	BS		0x08
#define	HT		0x09
#define	LF		0x0A
#define	VT		0x0B
#define	FF		0x0C
#define	CR		0x0D
#define	SO		0x0E
#define	SI		0x0F
#define	DLE	0x10
#define	DC1	0x11
#define	DC2	0x12
#define	DC3	0x13
#define	DC4	0x14
#define	NAK	0x15
#define	SYN	0x16
#define	ETB	0x17
#define	CAN	0x18
#define	EM		0x19
#define	SUB	0x1A
#define	ESC	0x1B
#define	FS		0x1C
#define	GS		0x1D
#define	RS		0x1E
#define	US		0x1F
#define	SPC		0x20
#define	DEL	0x7F
#endif

#ifndef XOFF
#define	XOFF	0x13
#define	XON	0x11
#endif

#ifndef NULL
     #define NULL NUL
#endif

// Universal parameter definitions similar to the windows message modell
typedef UINT WPARAM;									//!< Universal UINT parameter for IoControl and others
typedef long LPARAM;                         //!< Universal long parameter for IoControl and others

// silently remove MFC debug helpers
#ifndef ASSERT
#define ASSERT assert
#endif

// silently remove MFC debug helpers
#ifndef TRACE
#ifndef WINSIM
#define TRACE(n)
#define TRACE1(n,a)
#endif
#endif

// silently MS requirments
#ifndef __cdecl
#define __cdecl
#endif


// Unicode and preparation for it
typedef unsigned char bchar_t;					//!< Use this if you want to use a byte size CHARACTER (not just a binary byte)
typedef char  schar_t;								//!< use this if you want a CHARACTER which is signed (not just a binary byte)

#ifdef UNICODE
typedef wchar_t tchar_t;							//!< Use this if you want a byte or word CHARACTER depending on the UNICODE switch
#else
typedef unsigned char tchar_t;               //!< Use this if you want a byte or word CHARACTER depending on the UNICODE switch
#endif

#ifndef TEXT
#ifdef UNICODE
#define TEXT(x)    L ## x                    //!< Use to make an autosize Unicode string depending on the UNICODE switch
#else
#define TEXT(x)    x     //!< Use to make an autosize string depending on the UNICODE switch
#endif
#endif

#ifndef _T
#ifdef UNICODE
#define _T(x)      L ## x							//!< Use to make an autosize Unicode string depending on the UNICODE switch
#define _TS(x)     L ## x                    //!< Use to make an autosize Unicode string depending on the UNICODE switch
#else
#define _T(x)      x		//!< Use to make an autosize string depending on the UNICODE switch
#define _TS(x)     x		//!< Use to make an autosize signed string depending on the UNICODE switch
#endif
#endif

#ifdef WINSIM
#undef _T
#define _T(x)      (const bchar_t*) ## x
#define __irq
typedef unsigned short wchar_t;
#endif

// The following shall make the transition to unicode a little easier.
// It allows for the interfaces to use unicode while most other code still uses byte characters.
// TI stands for "Text for Interfaces"
#ifdef UNICODE_FOR_INTERFACES
typedef wchar_t	tichar_t;		//!< Use this if you want a byte or word CHARACTER depending on the UNICODE_FOR_INTERFACES switch
#define _TI(x)		L ## x         //!< Use to make an autosize Unicode string depending on the UNICODE_FOR_INTERFACES switch
#else
typedef tchar_t	tichar_t;		//!< Use this if you want a byte or word CHARACTER depending on the UNICODE_FOR_INTERFACES switch
#define _TI(x)		(const bchar_t*) ## x  //!< Use to make an autosize Unicode string depending on the UNICODE_FOR_INTERFACES switch
#endif

#endif	/*	_INC_LANGUAGE */




