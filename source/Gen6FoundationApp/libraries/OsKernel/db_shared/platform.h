/*----------------------------------------------------------------------*
 *	"PLATFORM.H" -- Platform specific header
 *----------------------------------------------------------------------*
 * $RCSfile: db_shared/platform.h $
 * $Revision: 1.5.1.10 $
 * $Date: 2009/02/25 13:47:06EST $
 *
 *
 ************************************************************************/

#ifndef _INC_PLATFORM
  #define _INC_PLATFORM

#include "compiler.h"


/*==============================================================
 * ARM specific Macros
 *==============================================================*/
#if defined (ARM)
extern bool EnableWDog;

#if PLATFORM_CURRENT == imx25
#include "HardwareMacros.h"		//Provides Macros for setting registers
#include "iMX25_WDOG.h"			//WatchDog Definitions
#elif PLATFORM_CURRENT	== imx27
#include "HardwareMacros.h"		//Provides Macros for setting registers
#include "iMX27_wdog.h"			//WatchDog Definitions
#elif PLATFORM_CURRENT	== mxl
#include "db_mx1.h"
#else
#error No PLATFORM_CURRENT Defined
#endif

// some platform specific defines
#define FAR
#define PC_HUGE
#define LONG			long
#define DBYTE			short
#define LNGLNG			long

// image platform specifics
//#define  TOPX        ttopx       // Width of image
//#define  TOPY        ttopy       // Height of image
#define  IMGX        iimgx       // Screen coord for upper-left corner
#define  IMGY        iimgy

extern   int         ttopx;
extern   int         ttopy;
extern   int         iimgx;
extern   int         iimgy;

// dynamic mem specifics
#define  MALLOC(t)   malloc(t)
#define  FREE(t)     free(t)

// watchdog specifics
#if (PLATFORM_CURRENT == imx27) | (PLATFORM_CURRENT == imx25)
	#define 	RESET_WATCHDOG_TIMER() do {HAZARD_WRITE_16BIT_REG(WDOG_WSR,WDOG_WSR_WSR_RELOAD1); HAZARD_WRITE_16BIT_REG(WDOG_WSR,WDOG_WSR_WSR_RELOAD2);} while (0)			//Feed the Dog
#elif PLATFORM_CURRENT	== mxl
	#define 	RESET_WATCHDOG_TIMER() do {WATCHDOG.Wsr.f = 0x5555; WATCHDOG.Wsr.f = 0xAAAA;} while (0)
#if (defined(ETM)) || (defined(PRODUCT_CONFIGURATION_APPLICATION))
	#define 	ENABLE_WATCHDOG() WATCHDOG.Wcr.b.WDE=FALSE						//Enable watchdog
#else //ETM - PRODUCT_CONFIGURATION_APPLICATION
	#define 	ENABLE_WATCHDOG() WATCHDOG.Wcr.b.WDE=EnableWDog						//Enable watchdog
#endif //endof ETM - PRODUCT_CONFIGURATION_APPLICATION
	#define 	DISABLE_WATCHDOG() WATCHDOG.Wcr.b.WDE=FALSE						//Disable watchdog
#endif //endof PLATFORM_CURRENT

#define	SPECIAL_CLASS_DEFINITION_MODIFIERS


#define	NEED_STRNICMP_AND_STRICMP	1

//extern "C" void Fast_memcpy(void *Destination, void *Source, int CopySize);


#define WOOF RESET_WATCHDOG_TIMER()

/*==============================================================
 * Power PC Gen 2 specific MACROS
 *==============================================================*/
#elif defined (PPC)

#define  SWSR *(volatile unsigned short *) 0xff00000e
#define  RESET()     _ASM("ba		0x0100")
#define  FPU         0

// some platform specific defines
#define  FAR
#define  PC_HUGE
#define  LONG        int
#define	DBYTE			short
#define	LNGLNG		long long

// dynamic mem specifics
#include "hhutils.h"				// Gives access to MemAlloc & MemFree
#define  MALLOC(t)   MemAlloc(t)
#define  FREE(t)     MemFree(t)

// image platform specifics
#ifdef IT4200
	#define  TOPX        640      // Width of image
	#define  TOPY        480      // Height of image
#else
	#define  TOPX        659      // Width of image
	#define  TOPY        494      // Height of image
#endif

#define  IMGX        20       // Screen coord for upper-left corner
#define  IMGY        60

#define	INVERSE_VIDEO	0		/* IT4400 images are 256-grayscale: 0=black, 255=white */

// watchdog specifics
#define  WOOF        do { SWSR = 0X556C; SWSR = 0xAA39; } while (0)


/*==============================================================
 * Turbo C  PCIMG specific MACROS
 *==============================================================*/
#elif defined (TCC)

#include <alloc.h>
#include <graphics.h>

#define  FPU         0
#define  RESET()

// some platform specific defines
#define  FAR         far
#define  PC_HUGE     huge
#define  LONG        long
#define	DBYTE			int
#define	LNGLNG		float

// dynamic mem specifics
#define  MALLOC(t)   malloc(t)
#define  FREE(t)     free(t)

// image platform specifics
#define  TOPX        ttopx       // Width of image
#define  TOPY        ttopy       // Height of image
#define  IMGX        iimgx       // Screen coord for upper-left corner
#define  IMGY        iimgy

extern   int         ttopx;
extern   int         ttopy;
extern   int         iimgx;
extern   int         iimgy;

extern int lmar, rmar, prx, pry;

#define	INVERSE_VIDEO	0		/* PC images are 256-grayscale: 0=black, 255=white */

// watchdog specifics
#define  WOOF

/*==============================================================
 * Windows DLL specific Macros
 *==============================================================*/
#elif defined (DLL)
//#if (DLL > 0)  			// we are using 2 different

#define  FPU         0
#define	IS_TRIG_PULLED()	1
#define  RESET()

// some platform specific defines
#define	PC_HUGE
#define	LONG		long
#define	DBYTE		int
#define	LNGLNG	float

#define	VOID		void
#define	SHORT		short
#define	CHAR		char
#undef	DEBUG

// dynamic mem specifics
void *   MemoryAllocate( unsigned LONG size);
void     MemoryFree( void *p );
#define  MALLOC(size)	MemoryAllocate((ULONG)(size))
#define  FREE(p)     	MemoryFree(p)

// image platform specifics
#define  TOPX  ttopx // Width of image
#define  TOPY  ttopy // Height of image
#define  IMGX  0     // Screen coord for upper-left corner
#define  IMGY  0

extern   int   ttopx;
extern   int   ttopy;

#define	INVERSE_VIDEO	0		/* PC images are 256-grayscale: 0=black, 255=white */

// watchdog specifics
#define  WOOF


#define	FastMemoryCopy(Destination, Source, AmountOfDataToCopy)		memcpy(Destination, Source, AmountOfDataToCopy);




/*==============================================================
 * Windows specific Macros (used for simulation)
 *==============================================================*/
#elif defined (WIN32)

#ifdef WINSIM	                           // a simulated debug environement with MSVC6
// image platform specifics
#define  TOPX        ttopx       // Width of image
#define  TOPY        ttopy       // Height of image
#define  IMGX        iimgx       // Screen coord for upper-left corner
#define  IMGY        iimgy

extern   int         ttopx;
extern   int         ttopy;
extern   int         iimgx;
extern   int         iimgy;

#define	SPECIAL_CLASS_DEFINITION_MODIFIERS
inline void RESET_WATCHDOG_TIMER(void)	{};
#ifndef LONG
typedef long LONG;
#endif
#define PC_HUGE
#endif

#endif   // if / elif for platform types


#endif  /*      _INC_PLATFORM */



