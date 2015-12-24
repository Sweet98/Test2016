/*=================================================================================
 $Source: LZWCompressor.h $
 
  Class implementation for Tiff LZW compression.
===================================================================================
 $Date: 2011/06/17 04:35:30EDT $
 $Revision: 1.1 $

===================================================================================*/


#ifndef _LZWCompressor_h_
#define _LZWCompressor_h_

#include "stdio.h"
#include "stdlib.h"
#include "BitString.h"


#define BITS 12									/* Setting the number of bits to 12, 13*/
#define HASHING_SHIFT (BITS-8)					/* or 14 affects several constants.    */
#define MAX_VALUE (1 << BITS) - 1					/* Note that MS-DOS machines need to   */
#define MAX_CODE MAX_VALUE - 1					/* compile their code in large model if*/
												/* 14 bits are selected.               */

#define CLEAR_CODE					(256)				/* Clear code */
#define FIRST						(CLEAR_CODE + 2)		/* Firs code */
#define END_OF_INFORMATION		(CLEAR_CODE + 1)		/* End of information */

#if BITS == 14
  #define TABLE_SIZE 18041						/* The string table size needs to be a */
#endif											/* prime number that is somewhat larger*/
#if BITS == 13									/* than 2**BITS.                       */
  #define TABLE_SIZE 9029
#endif
#if BITS <= 12
  #define TABLE_SIZE 5021
#endif


class CLZWCompressor
{
public:

	// Constructor
	CLZWCompressor();

	// Destructor
	~CLZWCompressor(void);

	// Search string table
	int FindMatch(int hash_prefix,unsigned int hash_character, int HashBits);

	// Output code to bit stream
	void output_bits(BITSTRING *output,unsigned short string_code,short current_code_bits);

	// We have output block of 256 length. if the output block buffer is full, flush it
	void flush_output_block(BITSTRING *TIFFImage);

	
	void add_to_output_block(unsigned char byte, BITSTRING *TIFFImage, int force_flush);

	
	bool TiffCompress(BITSTRING *TiffImage, BITSTRING *InputImage, unsigned long InputBytes);


private:
	
	int *code_value;						/* This is the code value array        */
	unsigned int *prefix_code;				/* This array holds the prefix codes   */
	unsigned char *append_character;		/* This array holds the appended chars */

	unsigned char output_block[256];
	int	output_block_count;

	int output_bit_count;
	unsigned long output_bit_buffer;

};













#endif // #ifndef _LZWCompressor_h_

