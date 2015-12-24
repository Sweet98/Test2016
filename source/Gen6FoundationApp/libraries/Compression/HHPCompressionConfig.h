//======================================================================================
// HHPCompressionConfig.h - Definitions required by Compression library
//======================================================================================
// $RCSfile: HHPCompressionConfig.h $
// $Revision: 3776 $
// $Date: 2012-09-26 23:15:21 +0800 (Wed, 26 Sep 2012) $
//======================================================================================

//======================================================================================








#ifndef HHP_COMPRESSION_CONFIGH
#define HHP_COMPRESSION_CONFIGH


// If doing an HHP Strong ARM build, include platform.h to get watchdog definitions.  Otherwise, it is up to the integrator to
// define these functions, which do nothing by default.
#ifdef ARM
	#ifndef _WIN32_WCE	// The following is for HHP device apps NOT Windows OS devices (Pocket PC).
		#include	"Platform.h"
	#else				// The following is for Windows OS devices (Pocket PC).
		#define		RESET_WATCHDOG_TIMER()
	#endif

#else

#define		RESET_WATCHDOG_TIMER()

#endif

#endif





