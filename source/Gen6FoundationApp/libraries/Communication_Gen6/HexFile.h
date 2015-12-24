//======================================================================================
// HexFile.h
//======================================================================================
// $RCSfile: HexFile.h $
// $Revision: 1.1 $
// $Date: 2009/03/08 16:03:40EDT $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Module Name:     HEX_FILE
//
//  Module Description:
//		Provide methods to compress and decompress hex file data.
//
//---------------------------------------------------------------------------
#ifndef HEX_FILEH
#define HEX_FILEH
//---------------------------------------------------------------------------







#include "HString.h"






// Convert hex file data from ASCII to compressed binary.
bool CompressHexData( HSTRING *InputUncompressedHexData, HSTRING *OutputCompressedHexData );

// Convert hex file data from compressed binary to ASCII.
bool DecompressHexData( HSTRING *InputCompressedHexData, HSTRING *OutputUncompressedHexData );



//---------------------------------------------------------------------------
#endif // HEX_FILEH




