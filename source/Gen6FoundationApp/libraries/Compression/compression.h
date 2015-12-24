//======================================================================================
// Compression.h
//======================================================================================
// $RCSfile: compression.h $
// $Revision: 3776 $
// $Date: 2012-09-26 23:15:21 +0800 (Wed, 26 Sep 2012) $
//======================================================================================

//======================================================================================








#ifndef COMPRESSIONH
#define COMPRESSIONH
//---------------------------------------------------------------------------
#include "HString.h"



//---------------------------------------------------------------------------





    // Compression flag settings
    enum { //COMPRESSION_FLAGS {
        NO_COMPRESSION=0,
        LEMPEL_ZIV_VERSION_1_0,
        HUFFMAN_VERSION_1_0,
        JPEG_LOSSY_1_0,
		JPEG_LOSSY_FAST_1_0,

    // Floating point is not supported at this time; remapped to JPEG_LOSSY_1_0
        JPEG_LOSSY_1_0_FP,

        LEMPEL_ZIV_VERSION_2_0,
        HUFFMAN_VERSION_2_0,

		LZO,

        INVALID_COMPRESSION_MODE
    };




// Create a running difference for the input data sent by the caller.
void Diff(HSTRING *Data, int Interval);

// Convert a running difference, sent by the caller, back to original data.
void Undiff(HSTRING *Data, int Interval);

// Compress data, sent by the caller, via a compression technique that mimics adaptive Huffman.
bool HuffmanEncode(HSTRING *Data, int Interval);

// Decompress data, sent by the caller, via a compression technique that mimics adaptive Huffman.
bool HuffmanDecode(HSTRING *Data);


// Compress data sent by the caller according to the mode specified by the caller.
//      (version #1, which takes only data and compression mode as arguments).
bool CompressData(HSTRING *Data, int CompressionMode);


// Compress data sent by the caller according to the mode specified by the caller.
//      (version #2, which takes data, compression mode, and Huffman difference interval as arguments).
bool CompressData(HSTRING *Data, int CompressionMode, int DifferenceInterval);


// Compress data sent by the caller according to the mode specified by the caller.
//      (version #3, which takes data, compression mode, Huffman difference interval, rows, columns, JPEG quality
//		factor, and JPEG compression smoothing factor as arguments.
bool CompressData(HSTRING *Data, int CompressionMode, int DifferenceInterval, int XDimension, int YDimension,
                            int QualityFactor, int SmoothingFactor, int colored);


// DecompressData (version #1, which returns image dimensions if applicable and takes JPEG decompression
//		block smoothing as an argument)
bool DecompressData(HSTRING *Data, int CompressionMode, int *XDimension, int *YDimension, bool BlockSmoothing,
							int UncompressedSize = 0);


// DecompressData (version #2, which takes JPEG decompression block smoothing as an argument)
bool DecompressData(HSTRING *Data, int CompressionMode, bool BlockSmoothing = true, int UncompressedSize = 0);


/******************************************************************************/
/*                             End of COMPRESSION.h                              */
/******************************************************************************/
#endif





