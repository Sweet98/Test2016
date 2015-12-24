//======================================================================================
// CRC.cpp
//======================================================================================
// $RCSfile: crc.cpp $
// $Revision: 1.2 $
// $Date: 2002/08/06 07:53:14EDT $
//======================================================================================

//======================================================================================


/************************************************************************/
/* IBM SDLC uses the CCITT-16 CRC. The CRC is initialized to 0xFFFF     */
/* prior to any calculations. On transmit the CRC includes all bytes    */
/* except the end-of-frame. The CRC is inverted and transmitted LSB     */
/* first. On reception the CRC bytes are included in the calculation.   */
/* This results in a constant value (0xF0B8) when no errors are found.  */
/************************************************************************/

#include "CRC.h"     /* function to calculate CCITT-16 CRC           */


#define BIG_ENDIAN  0   /* Intel processors are little endian    */

#if BIG_ENDIAN
  #define CRCLOW    *crcp       /* IBM-PC is big endian         */
  #define CRCHIGH   *(crcp+1)
#else
  #define CRCLOW    *(crcp+1)   /* Motorola is little endian    */
  #define CRCHIGH   *crcp
#endif



unsigned short calc_crc(unsigned char *ptr, int count)
{
int	crc, i;

	crc = 0;
	while(--count >= 0)	{
		crc = crc ^ (int)*ptr++ << 8;
		for(i = 0; i < 8; ++i)
			if(crc & 0x8000)
				crc = crc << 1 ^ 0x1021;
			else
				crc = crc << 1;
	}

	return (((unsigned short) (crc & 0xFFFF)));
}   /*  calc_crc    */



