/************************************************************************/
//
// HHP Matrix embedded firmware
//
//  SYN Command Parser definitions
//  Date:       3/27/02
//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: db_boot/db_syncommandparse.h $
// $Revision: 1.1.1.8 $
// $Date: 2011/11/10 20:44:26EST $
//======================================================================================
/************************************************************************/
/*|======================================================================
  |		   MODIFICATION HISTORY
  |======================================================================
*/
#ifndef __DB_SYNCOMMANDPARSE_H         /*  prevent redundant definitions   */
#define __DB_SYNCOMMANDPARSE_H  1


#include	"language.h"
#define	SIZEOF_HIGHLEVEL_PARSE_BUFFER	1024*4

#ifdef XENON_LITE_1500
#define COMPAT_PROD_ID	5	// for Xenon Lite, the compatible product ID should be 5
#elif defined(VUQUEST3310)
#define COMPAT_PROD_ID	7
#elif defined(HI2D_T)
#if defined(YJ2D_BASED_ON_HI2D)
#define COMPAT_PROD_ID	0xE0
#else
#define COMPAT_PROD_ID	0xE0
#endif
#else
#define COMPAT_PROD_ID	0
#endif


bool ServiceSYNCommand(UCHAR ucData);


#endif  /*  __DB_SYNCOMMANDPARSE_H  */



