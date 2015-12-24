/*=============================================================================
 * This file contains a function that will calculate a CRC for a given character
 * string.

 *=============================================================================
 * $RCSfile: interfaces/crc4683.cpp $
 * $Revision: 2.0.1.2 $
 * $Date: 2009/02/18 17:14:03EST $
 *============================================================================*/

#include "language.h"
#include "product.h"
#include "stdInclude.h"
#include "CommonDriver.h"
//#include "Timer.h"
#include "Drvserial4683.h"

/***********************************************************************************************
 * calc_crc - Calculates the crc's (cyclic redundancy check characters) for a given array.
 *          - crc's are calculated using a X16+X12+X5+1 polynomial.
 *          - Requires a pointer to an unsigned character array and the array size.
 *          - Returns a type crc (2 unsigned char's).
***********************************************************************************************/

CSerial4683::crc_t CSerial4683::CalcCrc( unsigned char *cmd_str, int lenght )
{
   crc_t crc_out;
   unsigned char crch = 0xff;
	unsigned char crcl;
	unsigned char temp = 0xff;
   int count, c, ibit;
   unsigned int temp_word;

   for (count=0; count<lenght; count++)         /*The main calculation loop.*/
      {
      crcl = cmd_str[count];                 /*fetch the next unsigned char in the array.*/
      crcl ^= temp;                       /*begin the calculatioon.*/

       for (ibit=0; ibit<8; ibit++)             /*the individual ibit operations loop.*/
         {
         c=0;
         if ((crcl % 2) != 0)
            {
            c=1;
            crch^=8;
            crcl^=16;
            }
         temp_word = (( crch * 256 ) + crcl );
         temp_word >>= 1;
         crcl = temp_word - ( crch * 256 );
         crch = temp_word >> 8;
         if ( c==1 )
            crch |= 0x80;
         }                             /*the end of the bit manipulation loop.*/

       temp = crcl;
      }                                /*the end of the main calculation loop.*/

   crc_out.crclo = (temp ^ 0xff);               /*result - the low order byte.*/
   crc_out.crchi = (crch ^ 0xff);               /*result - the high order byte.*/

   return crc_out;
}


