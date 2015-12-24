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
// $RCSfile: db_shared/db_crc.c $
// $Revision: 1.1.2.3 $
// $Date: 2009/02/06 18:22:03EST $
//======================================================================================
/************************************************************************/
/************************************************************************/
/* IBM SDLC uses the CCITT-16 CRC. The CRC is initialized to 0xFFFF     */
/* prior to any calculations. On transmit the CRC includes all bytes    */
/* except the end-of-frame. The CRC is inverted and transmitted LSB     */
/* first. On reception the CRC bytes are included in the calculation.   */
/* This results in a constant value (0xF0B8) when no errors are found.  */
/************************************************************************/
/*|=======================================================================
  |           MODIFICATION HISTORY
  |=======================================================================
 *************************************************************************/
//#include "db_sys.h"     /* system definitions                           */
#include "db_crc.h"     /* function to calculate CCITT-16 CRC           */
#include "db_chrio.h"

/* why is this not in the header file ???? */

#define BIG_ENDIAN  0   /* Motorola processors are little endian    */

#if BIG_ENDIAN
  #define CRCLOW    *crcp       /* IBM-PC is big endian         */
  #define CRCHIGH   *(crcp+1)
#else
  #define CRCLOW    *(crcp+1)   /* Motorola is little endian    */
  #define CRCHIGH   *crcp
#endif

/* Should be able to do this differently with a 32 bit bus */

// unsigned short calc_crc (BYTE *buf, int numchars)
// {
//     unsigned short  crc ;       /* full word    */
//     BYTE            *crcp ;     /* crc BYTE ptr */
//     BYTE            t ;
//     unsigned short  u, v ;
//
//     crc = 0xFFFF ;          /* for IBM SDLC the CRC is initialized to 0xFFFF */
//     crcp = (BYTE *) &crc ;          /* point to CRC storage */
//
//     while (numchars--)
//     {
//         t = CRCLOW ^ *buf++ ;       /* generate 16 bit intermediates */
//         CRCLOW = CRCHIGH ;
//         CRCHIGH = t ;
//
//         u = (unsigned short) t << 3 ;
//         v = crc ;
//         crc = (crc >> 1) & 0x0780 ;
//
//         CRCHIGH |= (t << 4) ;
//
//         crc ^= u ^ v ^ (t & 0x0F) ^ (t >> 4) ;  /* combine all intermediates    */
//     }
//
//     return (crc) ;
//
// }   /*  calc_crc    */


unsigned short calc_crc(unsigned char *ptr, int count)
{
	int	crc =0;
	while(--count >= 0)
	{
		crc = crc ^ (int)*ptr++ << 8;
		poll_getchar_from_rx_fifo();
		for(int i = 8; i > 0; i--)
		{
			crc <<= 1;	
			if(crc & 0x10000)
				crc ^= 0x1021;
		}
	}

	return (((unsigned short) (crc & 0xFFFF)));
}   /*  calc_crc    */

