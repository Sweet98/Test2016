/*===================================================================
MocfParser neeeds to compile on multiple OSs, and needs to "know"
what a 32 bit unsigned int is.
=====================================================================
   $Source: IntSizes.h $
   $Date: 2010/02/01 16:08:27EST $
   $Revision: 1.2 $


===================================================================*/
#ifndef _IntSizes_h_
#define _IntSizes_h_

#if defined(_WIN32)
#include "windows.h"
#elif defined(BUILD_LINUX)
typedef unsigned int DWORD;
#else
// assume Gen6 firmware
#include "language.h"
#endif // endof #if defined(_WIN32)






#endif // endof #ifndef _IntSizes_h_



