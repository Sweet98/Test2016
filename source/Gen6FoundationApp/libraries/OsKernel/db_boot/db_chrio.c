
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
// $RCSfile: db_boot/db_chrio.c $
// $Revision: 1.4.1.11 $
// $Date: 2009/12/17 18:45:50EST $
//======================================================================================
/************************************************************************/
/************************************************************************/
/*  This module contains serial port character I/O functionions		*/
/************************************************************************/
/*|=======================================================================
  |			MODIFICATION HISTORY
  |=======================================================================
*************************************************************************/
#include <string.h>
#include "db_hdw.h"          // Artwork dependant hardware definitions
#include "db_chrio.h"        // Character I/O header prototypes
#include "db_queue.h"

#if PLATFORM_CURRENT == imx25
#include "HardwareMacros.h"
#include "iMX25_UART.h"
#endif


#ifndef SCI_RBSIZE
#define SCI_RBSIZE 3*1024  /* size of the SCI receive queue   */
#endif
#ifndef SCI_TBSIZE
#define SCI_TBSIZE 3*1024  /* size of the SCI transmit queue */
#endif

static BYTE     Rcvbfr   [SCI_RBSIZE] ;      /* receive data buffer   */
static BYTE     Xmtbfr   [SCI_TBSIZE] ;      /* transmit data buffer  */

QUEUE   Rcvq ;					/* serial receive queue             */
QUEUE   Trnq ;					/* serial transmit queue            */



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

/* return receive queue status  */
int kbhit (void)
{
	return (que_has_data (&Rcvq)) ;
}	/*  kbhit	*/

/* return next character from the queue or false if the queue is empty  */
/* note format of Standard C library returns an int ... seems awfully messy */
/* so I have made my own get_char routine that really gets a "char" */
/* what's the point of having a standard C library that is not standard?? */
/* when a char is not a char..., and a define is not sacred... you know */
/* you are using DEC, or sorry, Intel processor */
char get_char(void)
{
	return (get_from_que (&Rcvq)) ;
} 	/*  get_char */

extern "C"	void SetBaudRateRegisters(int, int);
extern "C"	void GetBaudRateRegisters(int *, int *);





//
//	SetBaudRate
//		Compute the baud rate and set the baud rate registers.  Note that we're assuming the ratio is such that 1.0 is equivalent to
//		1024K baud.

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
//		Sample the baud rate registers and compute the baud rate.  Note that we're assuming the ratio is such that 1.0 is equivalent to
//		1024K baud. for mxl note 
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


unsigned char poll_getchar_from_rx_fifo (void)
{
#if ((PLATFORM_CURRENT == imx27) || (PLATFORM_CURRENT == imx25))
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
#endif
}


void xputchar(unsigned char Ch)
{
	int Status;
#if ((PLATFORM_CURRENT == imx27) || (PLATFORM_CURRENT == imx25))

	do
	{
		Status = READ_FIELD(UART1_UTS,UART_UTS_TXFULL_LSH,UART_UTS_TXFULL_WID);          // if tx fifo full, wait til it's not, for this primitive of a function, I got no protocol, lockup should never happen!!
	}
	while (Status);

	HAZARD_WRITE_FIELD_TO_REG(UART1_UTXD,UART_UTXD_TX_DATA_LSH,UART_UTXD_TX_DATA_WID, Ch);                        // send the char
#endif
}

void xputhex(unsigned int hex, int width)
{
	unsigned char c ;
	int i ;

	for (i = width - 1 ; i >= 0; i--)
	{
		c = ((UCHAR) ((hex >> (i * 4)) & 0xf)) ;
		if (c <= 9)
		{
			c = ((UCHAR) (c + '0')) ;
		}
		else
		{
			c = ((UCHAR) (c - 10 + 'A')) ;
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
		xputchar(((UCHAR) (c + '0'))) ;
	}
}

//
// Send a string of caller specified length to the host, even if interrupts are off.
//


void xputstring(const unsigned char *string, int StringLength)
{
	while (StringLength > 0)
	{
		xputchar(*string) ;
		string++ ;
		--StringLength;
	}
}


void xputstring(const unsigned char *string)
{
	xputstring(string, strlen(((char *) string)));
}


void xputstring(const char *string, int StringLength)
{
	xputstring((const unsigned char *)string, StringLength);
}


void xputstring(const char *string)
{
	xputstring((const unsigned char *)string, strlen(string));
}


// function to wait until the tx fifo and tx queue has been emptied
// note efficiency says you never want to do this because it is basically just deadlooping waiting for que to empty
// and txitter to be empty... note also for now, as I don't have interrupts on, this CANNOT wait for que to be empty...
// BECAUSE you actaully have to leave here for the queue to get emptied!!
//
void wait_til_tx_is_empty()
{
	int Status;

	do
	{
#if ((PLATFORM_CURRENT == imx27) || (PLATFORM_CURRENT == imx25))
		Status = (READ_FIELD(UART1_UTS,UART_UTS_TXEMPTY_LSH, UART_UTS_TXEMPTY_WID));
#endif //endof PLATFORM_CURRENT

	}
	while (!Status) ;                          // wait until the fifo is empty to leave

}



