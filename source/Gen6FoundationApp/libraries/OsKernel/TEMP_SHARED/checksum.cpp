/************************************************************************/
//
// HHP 4480 embedded firmware
// Checksum calculation
//
//  Date:       3/20/02
//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: TEMP_SHARED/checksum.cpp $
// $Revision: 1.2 $
// $Date: 2008/07/28 10:00:34EDT $
//======================================================================================


#include "CheckSum.h"		// function to calculate checksum

#include "db_chrio.h" 		//TEST ~DV

//
//	CalculateChecksum
//		Compute an alternating checksum (alternately adding and subtracting).
//		Do this on longwords (32 bits) until there aren't enough longwords
//		to do the addition and subtraction; add the remaining data to the checksum
//		one byte at a time.
//
//		Note: we check to see if the input data is longword aligned and if it isn't,
//		we'll realign the data for checksum calculation purposes.
//
unsigned int CalculateChecksum(unsigned char *Data, unsigned int Size)
{
	unsigned int	Checksum;
	unsigned int	*LongData;

	union {
		unsigned int			lval;
		unsigned char			cval[4];
	};

	Checksum = 0;
	LongData = ((unsigned int *) Data);

	if((((unsigned int) Data) % 4) != 0)
	{
		while(Size >= 8)	{
			cval[0] = *Data++;
			cval[1] = *Data++;
			cval[2] = *Data++;
			cval[3] = *Data++;
			Checksum += lval;
			cval[0] = *Data++;
			cval[1] = *Data++;
			cval[2] = *Data++;
			cval[3] = *Data++;
			Checksum -= lval;
			Size -= 8;
		}
	}
	else
	{
		while(Size >= 8)	{
			Checksum += *LongData++;
			Checksum -= *LongData++;
			Size -= 8;
		}

		Data = ((unsigned char *) LongData);
	}

	while(Size > 0)	{
		Checksum += ((unsigned int) *Data++);
		Size--;
	}

	return (Checksum);
}   //  CalculateChecksum



