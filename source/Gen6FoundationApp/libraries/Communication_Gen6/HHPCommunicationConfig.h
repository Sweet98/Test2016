//======================================================================================
// HHPCommunicationConfig.h - Definitions required by Communication library
//======================================================================================
// $RCSfile: HHPCommunicationConfig.h $
// $Revision: 1.1 $
// $Date: 2009/03/08 16:03:41EDT $
//======================================================================================

//======================================================================================








#ifndef HHP_COMMUNICATION_CONFIGH
#define HHP_COMMUNICATION_CONFIGH




// Only include strnicmp and stricmp for non-PC code.
#ifdef NEED_STRNICMP_AND_STRICMP	    // defined in the Win IDE project settings.
	#include "stricmp.h"
	#include "strnicmp.h"
#endif // NEED_STRNICMP_AND_STRICMP


// If doing an HHP Strong ARM build, include platform.h to get watchdog definitions.  Otherwise, it is up to the integrator to
// define these functions, which do nothing by default.
#ifdef ARM
	#ifndef _WIN32_WCE	// The following is for HHP device apps NOT Windows OS devices (Pocket PC).
		#include	"Language.h"
		#include	"Platform.h"
	#else				// The following is for Windows OS devices (Pocket PC).
		#define		RESET_WATCHDOG_TIMER()
		#define	EOT	0x04
		#define	ENQ	0x05
		#define	ACK	0x06
		#define	NAK	0x15
		#define	SYN	0x16
		#define	ESC	0x1B
		#define	GS	0x1D
		#define	STX	0x02
	#endif

#else

#define		RESET_WATCHDOG_TIMER()

#define	EOT	0x04
#define	ENQ	0x05
#define	ACK	0x06
#define	NAK	0x15
#define	SYN	0x16
#define	ESC	0x1B
#define	GS	0x1D
#define	STX	0x02

#endif

#endif





