//======================================================================================
// HexFile.cpp
//======================================================================================
// $RCSfile: HexFile.cpp $
// $Revision: 1.1 $
// $Date: 2009/03/08 16:03:39EDT $
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




#include "HexFile.h"






//
//	CompressHexData
//		Convert hex file data from ASCII to compressed binary.
//
bool CompressHexData( HSTRING *InputUncompressedHexData, HSTRING *OutputCompressedHexData )
{
	bool		Status = true;				// No problems yet.
	int			i;
	HSTRING		hstrTemp( 20, true );



	OutputCompressedHexData->Copy( "" );

	i = 0;
	while(( i < (int) InputUncompressedHexData->Size ) && ( Status ))
	{
		switch( InputUncompressedHexData->Char[ i ] )
		{
			case 0x3A:
			{
			// Keep the colons
				Status = OutputCompressedHexData->AppendChar( (int) InputUncompressedHexData->Char[ i ] );
				++i;
				break;
			}

			case 0x0A:
			case 0x0D:
			{
			// Do nothing, we do not need to transmit carriage returns
				++i;
				break;
			}

			default:
			{
			// Convert hex data to binary.
				hstrTemp.Copy( InputUncompressedHexData, i, i + 1 );
				Status = hstrTemp.HexToBinary();
				if( Status )
				{
					i += 2;
					Status = OutputCompressedHexData->Append( &hstrTemp );
				}
				break;
			}				
		}
	}

	return Status;
} // CompressHexData







//
//	DecompressHexData
//		Convert hex file data from compressed binary to ASCII.
//
#define	RECORD_SIZE_MINUS_DATA		6
bool DecompressHexData( HSTRING *InputCompressedHexData, HSTRING *OutputUncompressedHexData )
{
	bool			Status = true;				// No problems yet.
	int				BeginningOfNextRecord;
	unsigned int	NextRecordLength;
	unsigned int	NextDataLength;
	HSTRING			hstrTemp( 20, true );



	OutputUncompressedHexData->Copy( "" );

	BeginningOfNextRecord = 0;
	while(( ( BeginningOfNextRecord + RECORD_SIZE_MINUS_DATA ) <= (int) InputCompressedHexData->Size ) && ( Status ))
	{
	// Colon should be first.
		if( InputCompressedHexData->Char[ BeginningOfNextRecord ] == ':' )
		{
		// Get data length and compute record length.  They'd better be within the amount of data remaining to be processed.
			NextDataLength = (unsigned int) InputCompressedHexData->Char[ BeginningOfNextRecord + 1 ];
			NextRecordLength = NextDataLength + RECORD_SIZE_MINUS_DATA;
			if(( BeginningOfNextRecord + NextRecordLength ) <= (int) InputCompressedHexData->Size )
			{
			// Append the next formatted record, followed by <CR><LF>.
				OutputUncompressedHexData->Append( &InputCompressedHexData->Char[ BeginningOfNextRecord ], 1 );
				hstrTemp.Copy( InputCompressedHexData, BeginningOfNextRecord + 1, BeginningOfNextRecord + NextRecordLength - 1 );
				Status = hstrTemp.BinaryToHex();
				if( Status )
				{
					OutputUncompressedHexData->Append( &hstrTemp );
					OutputUncompressedHexData->Append( "\r\n" );
					BeginningOfNextRecord += NextRecordLength;		// Go to next record.
				}
			}
			else
			{
			// Next record extends beyond the end of the data remaining to be processed; WOOF!
				Status = false;
			}
		}
		else
		{
		// Next record didn't start with colon; WOOF!
			Status = false;
		}
	}

// If we succeeded, make sure that we consumed all the data.
	if(( Status ) && ( BeginningOfNextRecord < (int) InputCompressedHexData->Size ))
		Status = false;


	return Status;
} // DecompressHexData







