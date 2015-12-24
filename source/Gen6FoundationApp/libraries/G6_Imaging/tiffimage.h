//======================================================================================
// TiffImage.h
//======================================================================================
// $RCSfile: tiffimage.h $
// $Revision: 1.2 $
// $Date: 2011/06/17 04:37:16EDT $
//======================================================================================

//======================================================================================






//---------------------------------------------------------------------------
//
//  Definitions for TiffImage
//
//---------------------------------------------------------------------------
#ifndef TIFF_IMAGE_H
#define TIFF_IMAGE_H
//---------------------------------------------------------------------------




#include "BitString.h"			// BITSTRING interface definition


#include <stdio.h>

#define Largest_code						2560
#define Size_of_make_up_code_increments		64
#define Max_terminating_length				63	/* longest terminating code*/
#define Number_of_different_bytes			256

#define Pixels_per_byte			8
#define Bits_per_byte			8
#define Last_bit_in_a_byte		7		/* assumes bits numbered from 0 - 7 */
#define Last_bit_mask 			1		/* masks the last (low magnitude) bit */
#define Default_width_in_pixels	2560 	/* default width of a scan line */
#define Default_number_of_lines	3300 	/* default length of an image */

#define Invalid				   -1
#define None					0
#define Extra_positions			25		/* ensures extra room in allocations */
#define Not_done_yet			0

#define VL3 				   -3  		/* Vertical Left 3 mode */
#define VL2 				   -2  		/* Vertical Left 2 mode */
#define VL1 				   -1 		/* Vertical Left 1 mode */
#define V0						0		/* Vertical mode */
#define VR1						1 		/* Vertical Right 1 mode */
#define VR2						2  		/* Vertical Right 2 mode */
#define VR3						3  		/* Vertical Right 3 mode */
#define P						4 		/* Pass mode */
#define H						5		/* Horizontal mode */
#define EOFB					6		/* End Of File Buffer */

#define No_offset				0		/* no offset during fseek() */
#define End_of_file				2		/* start at EOF during fseek() */
#define Start_of_file			0		/* start at SOF during fseek() */

/*****************************************************************************************/
/*                         TIFF FILE STUFF                                       */
/*****************************************************************************************/

#define NUMBER_OF_IFD_ENTRIES 13   /* number of tags in the TIFF image file directory (IFD) */
#define IFD_OFFSET 8               /* with respect to the beginning of the file where is the IFD */
#define COMPRESSION_TYPE 4 		  /* This symbolizes CCITT T.6 Group 4 compression */
#define X_RESOLUTION_OFFSET (HEADER_SIZE+6+(NUMBER_OF_IFD_ENTRIES*12))  /* where is the x resolution offset */
#define Y_RESOLUTION_OFFSET (X_RESOLUTION_OFFSET + 8)                   /* where is the y resolution offset */
#define TIFF_STRIP_OFFSET (Y_RESOLUTION_OFFSET + 8)                     /* where is the data */

////////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/
/******************************************************************************************/

struct parameters {
	short previous_color;	/* color of last run of pixels */
	short index;			/* indicates current position in "coding_line" */
	short max_pixel;		/* the number of pixels in a scan line */
	short pixel;			/* pixel number of the last changing element */
	short *reference_line;	/* array of changing elements on reference line */
	short *coding_line;		/* array of changing elements on coding line */
};

struct compressed_descriptor {
	unsigned char  *data;		/* pointer to compressed image */
	short  pixels_per_line;		/* the number of pixels in a scan line */
	short  number_of_lines;		/* the number of scan lines in the image */
	long    length_in_bytes;		/* length of the compressed image in bytes */
};

struct uncompressed_descriptor {
	unsigned char  *data;		/* pointer to uncompressed image */
	short  pixels_per_line;		/* the number of pixels in a scan line */
	short  number_of_lines;		/* the number of scan lines in the image */
};


/*****************************************************************************

	declarations of all the procedures in the group4 compression routines
	follow.  The names of the files that contain the procedures are enclosed
	in comments above the declarations.

******************************************************************************/

/* compress.c */
void control_compression(struct uncompressed_descriptor *uncompressed,struct compressed_descriptor *compressed);   ///////
void set_up_first_and_last_changing_elements_c(struct parameters *params);    /////
void read_uncompressed_file_into_memory(struct uncompressed_descriptor *uncompressed);        /////
void prepare_to_compress(struct uncompressed_descriptor  *uncompressed,struct compressed_descriptor 	*compressed,
		struct parameters *params); /////
void compress_image(struct uncompressed_descriptor *uncompressed,struct compressed_descriptor *compressed,
		struct parameters *params); /////
void make_array_of_changing_elements(struct parameters *params,struct uncompressed_descriptor *uncompressed,
		short line_number); /////
void prepare_to_compress_next_line(struct parameters *params);     /////
void set_up_first_line_c(struct parameters *params);   /////
void crash_c(); /////
void process_char(unsigned char data_byte,struct parameters *params); /////
void compress_line(struct parameters *params);  /////
void initialize_b1(struct parameters *params);  /////
void pass_mode_c(struct parameters *params);  /////
void vertical_mode_c(struct parameters *params);  /////
void horizontal_mode_c(struct parameters *params);  /////
void write_run_length(short length,short color);
void prepare_to_write_bits_c(struct compressed_descriptor *compressed);   /////
void write_bits_c(char const * string_ptr);   /////
unsigned long flush_buffer(); /////
void grp4comp(unsigned char *indata,long inbytes,int width,int height,unsigned char *outdata,long *outbytes);

// Create an 8 or 1 bit per pixel Tiff image from the input image.
bool CreateTiffImage(BITSTRING *TiffImage, BITSTRING *InputImage, int XDimension, int YDimension, bool GrayscaleImage,
					bool CompressedImage, const char *pMessage, bool AppendCaereTag = false, int Threshold = 128);

bool CreateTiffColorImage(BITSTRING *TiffImage, BITSTRING *InputImage, int XDimension, int YDimension,
					bool CompressedImage, const char *pMessage, bool AppendCaereTag = false);

// Decode an 8 or 1 bit per pixel Tiff image into a raw output image.
bool DecodeTiffImage(BITSTRING *TiffImage, BITSTRING *OutputImage, int *XDimension, int *YDimension, int *Depth,
					 int *Compression, HSTRING *Message);




#endif // TIFF_IMAGE_H



