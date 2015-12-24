/************************************************************************/
//
// HHP 4480 embedded firmware
//
//  Display routines
//  Date:       2/24/02
//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: db_boot/db_show.c $
// $Revision: 1.1.2.8 $
// $Date: 2011/04/18 15:23:55EDT $
//======================================================================================
/************************************************************************/
/*|=======================================================================
  |			MODIFICATION HISTORY
  |=======================================================================
*************************************************************************/


#include "db_chrio.h"		// Character input / output routine definitions
#include "boot_main.h"


#if ((PLATFORM_CURRENT == imx27) || (PLATFORM_CURRENT == imx25))
#define NEW_PLATFORM_MX27	1
#endif

#include "db_sharedtable.h"

extern "C" const char Boot_msg[];


/************************************************************************/
/*	 Very stripped down versions of show_system included below			*/
/* such that the memory needs could be satisfied.  Intent was to keep	*/
/* same modular routine calling approach as op_code.					*/
/************************************************************************/
void show_system (void)
{
	xputstring(Boot_msg);
	xputstring((char *) BOOT_CODE_VERSION_STRING);
	xputstring ("\r\n");
	xputstring(g_DateStamp) ;
	xputchar (' ') ;
	xputstring(g_TimeStamp) ;
#ifdef ARM2_5
	xputstring(" SDT  ");
#else
	#ifdef NEW_PLATFORM_MX27
		//xputstring(" RVDS rev: ") ;
		xputint(__ARMCC_VERSION,6);
	#else
     	xputstring(" CWADS");
	#endif
#endif
	xputstring("\r\n") ;

}

void internal_reg_dump (void)
{
     // dump internal registers
}



