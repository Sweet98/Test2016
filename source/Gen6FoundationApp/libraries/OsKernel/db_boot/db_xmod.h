/************************************************************************/
//
// HHP 4480 embedded firmware
//
//  Xmodem header for boot code
//  Date:       2/24/02
//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: db_boot/db_xmod.h $
// $Revision: 1.2 $
// $Date: 2009/02/24 14:45:50EST $
//======================================================================================
/************************************************************************/
/*|=======================================================================
  |			MODIFICATION HISTORY
  |=======================================================================
*************************************************************************/
#ifndef __DB_XMOD_H         /*  prevent multiple definitions    */
#define __DB_XMOD_H     1

#define WAIT_PERIOD 4000

#define BADNEWS					-1
#define GOODNEWS				0

#define TIMEOUT_ERROR			1
#define CRC_ERROR				2
#define INVBLOCK_ERROR			3
#define NO_SOHSTX_ERROR			4
#define MISSED_BLOCK_ERROR		5
#define SPURIOUSCHAR_ERROR		6
#define RETRY_ERROR				7
#define	XMOD_ESC_RECEIVED		8
#define	DOWNLOAD_BUFFER_OVERRUN	9

extern int xmodem(unsigned char*, int *, int);

//int start_xmodem (int, char *);
int start_xmodem (unsigned char *, int *, int);

extern int xmodem_transmit(unsigned char*, int);

#endif  /*  __DB_XMOD_H */
/*  END */


