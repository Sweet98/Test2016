/************************************************************************/
//
// HHP 4480 embedded firmware
//
//  Xmodem C code for boot code
//  Date:       2/24/02
//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: db_boot/db_xmod.c $
// $Revision: 1.2 $
// $Date: 2009/02/24 14:45:49EST $
//======================================================================================
/************************************************************************/
/*
  | THIS MODULE IS STILL NEEDED BY BOOT CODE!!
  |
  |			MODIFICATION HISTORY
  |=======================================================================
*************************************************************************/
/*
	Front end for xmodem
*/
//#include "db_mx1.h"
#include "db_xmod.h"
#include "language.h"
#include "db_chrio.h"       // Character input / output routine definitions



//typedef int (*FUNC)(int, char**) ;
typedef int (*FUNC)();

int start_xmodem (BYTE *TransferMemoryLocation, int *NumberOfBytesTransferred, int MaximumBufferSize)
//int start_xmodem (int argc, char *argv[])
{
int		 retval = 0;


	xputstring ("will load at address 0x");

	xputhex((unsigned int)TransferMemoryLocation, 8) ;
	xputstring(".....\n") ;
	if (NULL != TransferMemoryLocation) {
		retval = xmodem(TransferMemoryLocation, NumberOfBytesTransferred, MaximumBufferSize) ;

		xputstring("\nHeader:\n") ;

	}

	return retval ;
}



