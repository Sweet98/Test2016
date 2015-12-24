
//
// HHP 4480 embedded firmware
//
//  Character I/O functions and some timing
//======================================================================================

//======================================================================================
//  Date:       2/24/02
//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: app_kernel/db_chrio.c $
// $Revision: 1.13.3.19 $
// $Date: 2010/04/09 15:33:45EDT $
//======================================================================================
/************************************************************************/
/************************************************************************/
/*  This module contains serial port character I/O functionions		*/
/************************************************************************/
/*|=======================================================================
  |			MODIFICATION HISTORY
  |=======================================================================
*************************************************************************/

#ifdef MATRIX_BOOT_CODE
#undef SIMPLE_IO_ONLY
#define SIMPLE_IO_ONLY 1
#endif

#include <string.h>
#include <stdio.h>

#include "db_hdw.h"          // Artwork dependant hardware definitions
#include "db_chrio.h"        // Character I/O header prototypes
#include "db_queue.h"

#if PLATFORM_CURRENT == imx25
#include "HardwareMacros.h"
#include "iMX25_UART.h"
#endif	//endof PLATFORM_CURRENT

/***** Private variables *****/
#ifndef SCI_RBSIZE
#define SCI_RBSIZE 10*1024  /* size of the SCI receive queue   */
#endif
#ifndef SCI_TBSIZE
#define SCI_TBSIZE 10*1024  /* size of the SCI transmit queue */
#endif

static BYTE     Rcvbfr   [SCI_RBSIZE] ;      /* receive data buffer   */
static BYTE     Xmtbfr   [SCI_TBSIZE] ;      /* transmit data buffer  */

QUEUE   Rcvq ;					/* serial receive queue             */
QUEUE   Trnq ;					/* serial transmit queue            */

extern "C"	void SetBaudRateRegisters(int, int);
extern "C"	void GetBaudRateRegisters(int *, int *);

#ifdef SIMPLE_IO_ONLY

#define IsInterfaceRunning() 0
CIoMan *g_pStdInterface = NULL;
CIoMan*g_pCurrentInterface = NULL;
#define HI_LEVEL_READ(a,b)
#define HI_LEVEL_WAIT()
#define HI_LEVEL_QHAS_DATA()	FALSE
#define HI_LEVEL_WRITE_CHARACTER( character )			1
#define HI_LEVEL_WRITE_WITH_LENGTH( string, length )

#else

#include "ioman.h"
#define HI_LEVEL_WRITE_CHARACTER( character )			theCurrentInterface.Write( character )
#define HI_LEVEL_WRITE_WITH_LENGTH( string, length )	theCurrentInterface.Write( string, length )
#define HI_LEVEL_WRITE_STRING(s) theCurrentInterface.Write(s)
#define HI_LEVEL_READ(a,b)  	theCurrentInterface.Read(a,b)
#define HI_LEVEL_WAIT()		theCurrentInterface. WaitUntilSent()
#define HI_LEVEL_QHAS_DATA()	theCurrentInterface.IsAnyRxData()
#endif

void FlushSimpleIoQueues(void)
{
    Rcvq.inptr = Rcvq.outptr = Rcvq.head = Rcvbfr ;
    Rcvq.tail  = Rcvbfr + SCI_RBSIZE ;

    Trnq.inptr = Trnq.outptr = Trnq.head = Xmtbfr ;
    Trnq.tail  = Xmtbfr + SCI_TBSIZE ;

}
void clear_tx_queue(void)
{
    clear_que(&Trnq);
}

void clear_rx_queue(void)
{
    clear_que(&Rcvq);
}

//
//	SetBaudRate
//		Compute the baud rate and set the baud rate registers.  Note that we're assuming the ratio is such that 1.0 is equivalent to
//		1024K baud.
//
void SetBaudRate(int BaudRate)         /*hardcoded numbers for best accuracy at this point */
{
	const int den = 30000;
	
	switch(BaudRate)
	{
	case 115200:
	case 230400:
	case 460800:
	case 921600:
	{
		unsigned int num =(16*BaudRate)/(UART_CLK_REF/den);
		SetBaudRateRegisters(num-1,den-1);
	}
	default:
		break;
	}
	return;
}



//
//	GetBaudRate
//		Sample the baud rate registers and compute the baud rate. Assumes Uart ref freq is actually UART_CLK_REF
//
//

int GetBaudRate(void)
{
	unsigned int num;
	unsigned int den;
	GetBaudRateRegisters((int *)(&num),(int *)(&den));
// below we try to limit effects of overflow by evaluating in a certain order
	num+=1;
	den+=1;
	unsigned long temp= UART_CLK_REF/den;
	temp *=num;
	temp/=16;
	return temp;
} // GetBaudRate
//
//	If this application supports CIoMan, then implement these functions that get or set the value of g_pCurrentInterface,
//	checking to see if we're running in the main thread or not.
//
#if !defined( MATRIX_BOOT_CODE ) && !defined( SIMPLE_IO_ONLY )
CIoMan *GetCurrentInterface( void )
{

    return g_pCurrentInterface;
}


void SetCurrentInterface( CIoMan *NewCurrentInterfacePointer )
{
    g_pCurrentInterface = NewCurrentInterfacePointer;
    return;
}
#endif


unsigned char poll_getchar_from_rx_fifo (void)
{
    if ( IsInterfaceRunning())
        return FALSE;
    else
    {

#if PLATFORM_CURRENT == imx25
        unsigned char  rx_char;
        unsigned char  status = false;
        unsigned long rx_data_reg;
        if(!(READ_FIELD(UART1_UTS,UART_UTS_RXEMPTY_LSH,UART_UTS_RXEMPTY_WID)))
        {
            rx_data_reg = READ_REG(UART1_URXD);
            if(!(READ_FIELD((unsigned long)&rx_data_reg,UART_URXD_ERR_LSH,UART_URXD_ERR_WID)))
            {
                rx_char = (unsigned char) rx_data_reg;
                put_in_que (&Rcvq, rx_char);
                status = TRUE;
            }
        }
        return status;

#endif	//endof PLATFORM_CURRENT

    } //Else IsInterfaceRunning()
}//poll_getchar_from_rx_fifo

int kbhit (void)
{
    if ( IsInterfaceRunning())
        return HI_LEVEL_QHAS_DATA();
    else
    {
        int i;
        i = que_has_data (&Rcvq);
        return (i) ; // que_has_data (&Rcvq)) ;
    }
}	/*  kbhit	*/


/* return next character from the queue or false if the queue is empty  */
/* note format of Standard C library returns an int ... seems awfully messy */
/* so I have made my own get_char routine that really gets a "char" */
/* what's the point of having a standard C library that is not standard?? */
/* when a char is not a char..., and a define is not sacred... you know */
/* you are using DEC, or sorry, Intel processor */
char get_char(void)
{
//    RESET_WATCHDOG_TIMER();
    if ( IsInterfaceRunning())
    {
        bchar_t Readchar;
        HI_LEVEL_READ(&Readchar,1);
        return (char) Readchar;
    }
    else
        return (get_from_que (&Rcvq)) ;
} 	/*  get_char */



void xputchar(unsigned char Ch)
{
#if defined(SM5130)
{
        int Status;
#if ((PLATFORM_CURRENT == imx25) || (PLATFORM_CURRENT == imx27))

        do
        {
            Status = READ_FIELD(UART1_UTS,UART_UTS_TXFULL_LSH,UART_UTS_TXFULL_WID);          // if tx fifo full, wait til it's not, for this primitive of a function, I got no protocol, lockup should never happen!!
        }
        while (Status);

        HAZARD_WRITE_FIELD_TO_REG(UART1_UTXD,UART_UTXD_TX_DATA_LSH,UART_UTXD_TX_DATA_WID, Ch);                        // send the char

#endif //endof PLATFORM_CURRENT

//        RESET_WATCHDOG_TIMER();
    }

#else
    if ( IsInterfaceRunning())
        do
        { }
        while ( HI_LEVEL_WRITE_CHARACTER(Ch)==0 );
    else
    {
        int Status;
#if ((PLATFORM_CURRENT == imx25) || (PLATFORM_CURRENT == imx27))

        do
        {
            Status = READ_FIELD(UART1_UTS,UART_UTS_TXFULL_LSH,UART_UTS_TXFULL_WID);          // if tx fifo full, wait til it's not, for this primitive of a function, I got no protocol, lockup should never happen!!
        }
        while (Status);

        HAZARD_WRITE_FIELD_TO_REG(UART1_UTXD,UART_UTXD_TX_DATA_LSH,UART_UTXD_TX_DATA_WID, Ch);                        // send the char

#endif //endof PLATFORM_CURRENT

//        RESET_WATCHDOG_TIMER();
    }	//else if ( IsInterfaceRunning())
#endif    
} //xputchar


void xputhex(unsigned int hex, int width)
{
    unsigned char c ;
    int i ;

    for (i = width - 1 ; i >= 0; i--)
    {
        c = ((unsigned char) ((hex >> (i * 4)) & 0xf));
        //	if ((c >= 0) && (c <= 9)) {		// First comparison is not needed for unsigned.
        if (c <= 9)
        {
            c = ((unsigned char) (c + '0'));
        }
        else
        {
            c = ((unsigned char) (c - 10 + 'A'));
        }
        xputchar(c) ;
    }
}


void xputint(unsigned int num, int width)
{

    unsigned char c ;
    int i ;
    int		DigitMultiplier=1;


    // Compute the multiplier for the highest digit.
    for(i = 1;    i < width;    ++i)
    {
        DigitMultiplier *= 10;
    }


    for (i = width - 1 ; i >= 0; i--)
    {
        c = ((unsigned char) ((num / DigitMultiplier) % 10));
        DigitMultiplier /= 10;
        xputchar(((BYTE) (c + '0'))) ;
    }
}

//
// Send a string of caller specified length to the host, even if interrupts are off.
//


void xputstring(const unsigned char *string, int StringLength)
{
    if ( IsInterfaceRunning())
    {
        HI_LEVEL_WRITE_WITH_LENGTH((char *)string,StringLength );
        return;
    }
    while (StringLength > 0)
    {
        xputchar(*string) ;
        string++ ;
        --StringLength;
    }
}


void xputstring(const unsigned char *string)
{
    if ( IsInterfaceRunning())
    {
        HI_LEVEL_WRITE_STRING((char *)string);
        return;
    }
    while (*string)
    {
        xputchar(*string) ;
        string++ ;
    }
}



void xputstring(const char *string, int StringLength)
{
    xputstring((const unsigned char *)string, StringLength);
}


void xputstring(const char *string)
{
    xputstring((const unsigned char*) string);
}


// function to wait until the tx fifo and tx queue has been emptied
// note efficiency says you never want to do this because it is basically just deadlooping waiting for que to empty
// and txitter to be empty... note also for now, as I don't have interrupts on, this CANNOT wait for que to be empty...
// BECAUSE you actaully have to leave here for the queue to get emptied!!
//
void wait_til_tx_is_empty()
{
    if ( IsInterfaceRunning())
    {
        HI_LEVEL_WAIT();
    }
    else
    {
        int Status;

        // in the long term, when irqs are emtying the queue, this is really what I want to do here
        //while (que_has_data(&Trnq) || tx_fifo_has_data())
        //{}        // wait til both of the above are not true

        do
        {

#if ((PLATFORM_CURRENT == imx25) || (PLATFORM_CURRENT == imx27))
            Status = (READ_FIELD(UART1_UTS,UART_UTS_TXEMPTY_LSH,UART_UTS_TXEMPTY_WID));
#endif	//endof PLATFORM_CURRENT

        }
        while (!Status) ;                          // wait until the fifo is empty to leave
    }
}

// add only for debug
void TraceOutput( const char *TraceInformation )
{
    DEBUG_puts(TraceInformation);
}

void DEBUG_puts(const char *c)
{
    RESET_WATCHDOG_TIMER();
#if ((PLATFORM_CURRENT == imx25) || (PLATFORM_CURRENT == imx27))

    for (;*c;c++)
    {
        while (READ_FIELD(UART1_UTS,UART_UTS_TXFULL_LSH,UART_UTS_TXFULL_WID))
        {}
        ; // if tx fifo full, wait til it's not
        HAZARD_WRITE_FIELD_TO_REG(UART1_UTXD,UART_UTXD_TX_DATA_LSH,UART_UTXD_TX_DATA_WID,*c);                        // send the char
    }

#endif
}//DEBUG_puts

//add command 'D' and 'E'
bool IsCtsHigh(void)
{
	if((pUart1->Usr1)&BIT(UART_USR1_RTSS_LSH))
	{
		return true;
	}
	else
	{
		return false;
	}
} 

void toggleCtsline(void)
{
	if((pUart1->Ucr2)&BIT(UART_UCR2_CTS_LSH))
	{
		(pUart1->Ucr2) &= (~BIT(UART_UCR2_CTS_LSH));
	}
	else
	{
		(pUart1->Ucr2) |= BIT(UART_UCR2_CTS_LSH);
	}
}
//

//****************************************************************
//****************************************************************
//Use these to send debug output to UART 2
#ifdef LARRY_TEST                                 //lmc-debug

//  Debug2 outputs to the second uart

void DEBUG_puts2(const char *c)
{
    RESET_WATCHDOG_TIMER();

    while (*c)
    {
        Uart2.Utxd[0].f = *c;                        // send the char
        c++;
        do
        {}
        while ( Uart2.Uts.b.TXFULL);
    }
}

//  xPutChar2 outputs to the second uart

void xPutChar2(unsigned char Ch)
{
    int Status;

    if(IsInterfaceRunning_2())
    {
        do
        {
            Status = Uart2.Uts.b.TXFULL;  // if tx fifo full, wait til it's not, for this primitive of a function, I got no protocol, lockup should never happen!!
        }
        while (Status);

        Uart2.Utxd[0].f = Ch;                        // send the char
    }

//    RESET_WATCHDOG_TIMER();

}

//  xPutString2 outputs to the second uart

void xPutString2(const BYTE *string, int StringLength)
{
    while (StringLength > 0)
    {
        xPutChar2(*string) ;
        string++ ;
        --StringLength;
    }
}

//  xPutString2 outputs to the second uart

void xPutString2(const BYTE *string)
{
    xPutString2(string, strlen(((char *) string)));
}

// Put to Debug UART 2 if Debug interface active, or current interface or Uart 1 if not active

void xputstring1(const BYTE *string, int StringLength)
{
    if( IsInterfaceRunning_2() )                                                //if UART 2 running
    {
        xPutString2(string, StringLength);                           //lets send it to uart 2 for debug out
        return;
    }

    if ( IsInterfaceRunning())
    {
        HI_LEVEL_WRITE_STRING(char *)string,StringLength );
        return;
    }
    while (StringLength > 0)
    {
        xputchar(*string) ;
        string++ ;
        --StringLength;
    }
}

void xputstring1(const BYTE *string)
{
    xputstring1(string, strlen(((char *) string)));
}


void DEBUG_wait_til_tx_is_empty()
{
    int Status;

    // in the long term, when irqs are emtying the queue, this is really what I want to do here
    //while (que_has_data(&Trnq) || tx_fifo_has_data())
    //{}        // wait til both of the above are not true

    do
    {
        Status = (Uart1.Uts.b.TXEMPTY && Uart1.Usr2.b.TXDC);   // fifo empty and TRULY complete ... and I am outta here
    }
    while (!Status) ;                          // wait until the fifo is empty to leave

}
#endif
//****************************************************************
//****************************************************************

