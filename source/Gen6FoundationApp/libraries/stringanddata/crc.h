//======================================================================================
// CRC.h
//======================================================================================
// $RCSfile: crc.h $
// $Revision: 1.3 $
// $Date: 2002/08/06 07:53:31EDT $
//======================================================================================

//======================================================================================



/************************************************************************/
/* IBM SDLC uses the CCITT-16 CRC. The CRC is initialized to 0xFFFF     */
/* prior to any calculations. On transmit the CRC includes all bytes    */
/* except the end-of-frame. The CRC is inverted and transmitted LSB     */
/* first. On reception the CRC bytes are included in the calculation.   */
/* This results in a constant value (0xF0B8) when no errors are found.  */
/************************************************************************/


#ifndef __CRC_H          /*  prevent multiple definitions    */
#define __CRC_H  1





#define CRCOK       0xF0B8      /* received frame, valid CRC    */

unsigned short calc_crc (unsigned char *, int) ;

#endif      /*  __CRC_H  */



