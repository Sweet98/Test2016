//======================================================================================
// HHPImagingConfig.h - Definitions required by Imaging library
//======================================================================================
// $RCSfile: HHPImagingConfig.h $
// $Revision: 1.2 $
// $Date: 2009/04/03 18:21:32EDT $
//======================================================================================

//======================================================================================



#ifndef HHP_IMAGING_CONFIGH
#define HHP_IMAGING_CONFIGH



// If doing an HHP Strong ARM build, include platform.h to get watchdog definitions.  Otherwise, it is up to the integrator to
// define these functions, which do nothing by default.
#ifdef ARM
	#ifndef _WIN32_WCE	// The following is for HHP device apps NOT Windows OS devices (Pocket PC).

		#include	"Language.h"
		#include	"Platform.h"            // Abbreviated type definitions
		#define		FastMemoryCopy(Destination, Source, AmountOfDataToCopy)		memcpy(Destination, Source, AmountOfDataToCopy);

	#else				// The following is for Windows OS devices (Pocket PC).

		#define		RESET_WATCHDOG_TIMER()
		#define		FastMemoryCopy(Destination, Source, AmountOfDataToCopy)		memcpy(Destination, Source, AmountOfDataToCopy);
		#define		TRACE( TraceMessage )
		#define		MALLOC(size)	malloc((unsigned long)(size))
		#define		FREE(p)     	free(p)

	#endif

#else			//not ARM

#define		RESET_WATCHDOG_TIMER()

#define		FastMemoryCopy(Destination, Source, AmountOfDataToCopy)		memcpy(Destination, Source, AmountOfDataToCopy);

#define		TRACE( TraceMessage )

#define		MALLOC(size)	malloc((unsigned long)(size))
#define		FREE(p)     	free(p)

#endif


// Include TIFF decompression if this isn't a Matrix firmware build.
#ifndef	MATRIX
	#define	TIFF_DECOMPRESSION	1
#endif


#endif



