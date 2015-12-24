/************************************************************************/
//
// HHP 4480 embedded firmware
//
//  Character I/O header
//  Date:       2/24/02
//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: db_shared/db_chrio.h $
// $Revision: 1.14.1.4 $
// $Date: 2010/03/04 12:50:08EST $
//======================================================================================
/************************************************************************/
/*|=======================================================================
  |           MODIFICATION HISTORY
  |=======================================================================
*************************************************************************/
#ifndef __DB_CHRIO_H
#define __DB_CHRIO_H 1

//#include "language.h"


//#define   SCRATCH_BUFFER_SIZE      1024 //2048

#ifndef LARRY_TEST
//#define LARRY_TEST                          //uncomment if you wish to use UART2 for debug,   lmc-debug
#endif

//extern char	MsgBuf [SCRATCH_BUFFER_SIZE] ;

void FlushSimpleIoQueues(void);
void clear_tx_queue(void);
void clear_rx_queue(void);




// Compute the baud rate and set the baud rate registers.  Note that we're assuming the ratio is such that 1.0 is equivalent to
// 1000000 baud.
void SetBaudRate(int BaudRate);

// Sample the baud rate registers and compute the baud rate.  Note that we're assuming the ratio is such that 1.0 is equivalent to
// 1000000 baud.
int GetBaudRate(void);
void wait_til_tx_is_empty();
void DEBUG_puts(const char *c); // send string out serial port use for LOW LEVEL or USB debugging

void xputstring(const unsigned char *string, int StringLength);
void xputstring(const unsigned char *string);
void xputstring(const char *string, int StringLength);
void xputstring(const char *string);

void xputchar(unsigned char Ch);
void xputhex(unsigned int hex, int width);
void xputint(unsigned int num, int width);

//add command 'D' and 'E'
bool IsCtsHigh(void);
void toggleCtsline(void);
//

//****************************************************************
//****************************************************************
#ifdef LARRY_TEST               //lmc-debug
void xPutString2(const unsigned char *string, int StringLength);
void xPutString2(const unsigned char *string);
inline void xPutString2(const char *string, int StringLength)
{
	xPutString2((const unsigned char *)string, StringLength);
}
inline void xPutString2(const char *string)
{
	xPutString2((const unsigned char*) string);
}

void xputstring1(const unsigned char *string, int StringLength);
void xputstring1(const unsigned char *string);
inline void xputstring1(const char *string, int StringLength)
{
	xputstring1((const unsigned char *)string, StringLength);
}
inline void xputstring1(const char *string)
{
	xputstring1((const unsigned char*) string);
}

void DEBUG_wait_til_tx_is_empty();

int get_peeks(unsigned char * resultChar, unsigned int offset);
#endif
//****************************************************************
//****************************************************************

unsigned char poll_getchar_from_rx_fifo (void);
int kbhit (void);
char get_char(void);
char get_peek(void);

// Send a string of caller specified length to the host, even if interrupts are off.
void xputstring_w_null(unsigned char *String, int Length);
class CIoMan;
extern CIoMan * g_pStdInterface;
extern CIoMan * g_pCurrentInterface;
#if !defined( MATRIX_BOOT_CODE ) && !defined( SIMPLE_IO_ONLY )
class CIoMan;
CIoMan *GetCurrentInterface( void );
void SetCurrentInterface( CIoMan *NewCurrentInterfacePointer );
#endif

#endif      /*  __DB_CHRIO_H */



