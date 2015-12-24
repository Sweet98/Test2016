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
// $RCSfile: db_shared/db_crc.h $
// $Revision: 1.1 $
// $Date: 2003/11/25 17:13:19EST $
//======================================================================================
/************************************************************************/
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
#ifndef __DB_CRC_H          /*  prevent multiple definitions    */
#define __DB_CRC_H  1

#define CRCOK       0xF0B8      /* received frame, valid CRC    */

unsigned short calc_crc (unsigned char *, int) ;

#endif      /*  __DB_CRC_H  */



