/*=================================================================================
 $Source: LZWCompressor.cpp $
 
  Class implementation for Tiff LZW compression.
===================================================================================
 $Date: 2011/06/17 04:35:36EDT $
 $Revision: 1.1 $

===================================================================================*/


#include "LZWCompressor.h"


CLZWCompressor::CLZWCompressor()
{
	output_block_count = 0;
	output_bit_count = 0;
	output_bit_buffer = 0L;
}

CLZWCompressor::~CLZWCompressor()
{
	;
}

bool CLZWCompressor::TiffCompress(BITSTRING *TiffImage, BITSTRING *InputImage, unsigned long InputBytes)
{
	unsigned int next_code;
	unsigned int character;
	unsigned int string_code;
	unsigned int index;
	int i;
	int n_bits = 9;
	unsigned char *Input = InputImage->Char;


	// Initialize string table
	code_value=new int[TABLE_SIZE*sizeof(int)];
	prefix_code=new unsigned int[TABLE_SIZE*sizeof(unsigned int)];
	append_character=new unsigned char [TABLE_SIZE*sizeof(unsigned char)];

	if (code_value==NULL || prefix_code==NULL || append_character==NULL)
	{
		return false;
	}

	// Clear code
	output_bits(TiffImage, CLEAR_CODE, n_bits);

	next_code=FIRST;              // Next code is the next available string code
	for (i=0;i<TABLE_SIZE;i++)  // Clear out the string table before starting
		code_value[i]=-1;

	string_code=*Input;    // Get the first code
	Input++;

	// Main loop
	while (Input < (InputImage->Char + InputBytes))
	{
		
		character=*Input;
		Input++;

		// If the string is in the table, get the code value else try to add it.
		index=FindMatch(string_code,character, n_bits);
		if (code_value[index] != -1)            
			string_code=code_value[index];        
		else                                    
		{                                       
			code_value[index]=next_code;
			next_code++;
			prefix_code[index]=string_code;
			append_character[index]=character;

			output_bits(TiffImage,string_code,n_bits); 
			string_code=character;            

			if (next_code == (1 << n_bits))
				n_bits++;

			if (next_code==MAX_CODE)
			{
				// Clear code
				output_bits(TiffImage, CLEAR_CODE, n_bits);
				//Reset table
				n_bits = 9;
				next_code = FIRST;
				for (i=0;i<TABLE_SIZE;i++)  // Clear out the string table before starting
					code_value[i]=-1;
				// Output as first code
				character=*Input;
				Input++;
				code_value[index]=next_code;
				next_code++;
				prefix_code[index]=string_code;
				append_character[index]=character;
				output_bits(TiffImage,string_code,n_bits);  
				string_code=character;           
			}
		}                                   
	}                                 
	
	// End of the main loop.

	output_bits(TiffImage,string_code,n_bits)	;			// Output the last code
	output_bits(TiffImage,END_OF_INFORMATION, n_bits);			// Output the end of information



	delete [] code_value;
	delete [] prefix_code;
	delete [] append_character;

	return true;
}


/* Find if the string is already in the string table */
int CLZWCompressor::FindMatch(int HashPrefix,unsigned int HashCharacter, int HashBits)
{
	int Index;
	int Offset;

	Index = (HashCharacter << HASHING_SHIFT) ^ HashPrefix;
	if (Index == 0)
		Offset = 1;
	else
		Offset = TABLE_SIZE - Index;
	while (1)
	{
		if (code_value[Index] == -1)
			return(Index);
		
		if (prefix_code[Index] == HashPrefix && 
			append_character[Index] == HashCharacter)
			return(Index);

		Index -= Offset;
		if (Index < 0)
			Index += TABLE_SIZE;
	}
}



void CLZWCompressor::output_bits(BITSTRING *output,unsigned short string_code,short current_code_bits)
{
	output_bit_buffer |= (unsigned long) string_code << (32-current_code_bits-output_bit_count);
	output_bit_count += current_code_bits;
	while (output_bit_count >= 8)
	{
		add_to_output_block(output_bit_buffer >> 24, output, 0);
		output_bit_buffer <<= 8;
		output_bit_count -= 8;
	}

	if(string_code == END_OF_INFORMATION){
		/* get the remaining bits in the buffer up to one byte*/
		add_to_output_block(output_bit_buffer >> 24, output, 1);
	}

}

void CLZWCompressor::add_to_output_block(unsigned char byte, BITSTRING *TIFFImage, int force_flush)
{
	//static unsigned char output_block[256];
	//static int	output_block_count = 0;
	
	output_block[output_block_count] = byte;
	output_block_count++;
	
	if ( output_block_count == 256 ){
		flush_output_block(TIFFImage);
		output_block_count = 0;
	}

	if(force_flush){
		flush_output_block(TIFFImage);
	}

}


void CLZWCompressor::flush_output_block(BITSTRING *TIFFImage)
{
	if ( output_block_count )
	{
		TIFFImage->Append(output_block, output_block_count);

	}
}



