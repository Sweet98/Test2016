//======================================================================================
// HHPReaderConfiguration_Config.h - Definitions required by reader configuration library
//======================================================================================
// $RCSfile: HHPReaderConfiguration_Config.h $
// $Revision: 1.1 $
// $Date: 2009/03/03 08:54:39EST $
//======================================================================================

//======================================================================================








#ifndef HHP_READER_CONFIGURATION_CONFIGH
#define HHP_READER_CONFIGURATION_CONFIGH



// If doing an ARM build, include platform.h to get compiler environment definitions.  Otherwise, it is up to the
// integrator to define these functions, which do nothing by default.
#ifdef ARM

#include	"Language.h"
#include	"Platform.h"

#else

#define	ENQ	0x05
#define	ACK	0x06
#define	NAK	0x15

#endif



#endif





