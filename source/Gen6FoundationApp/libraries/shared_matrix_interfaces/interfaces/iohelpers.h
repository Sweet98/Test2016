/*=================================================================================

//=================================================================================
   $Source: interfaces/iohelpers.h $
   $Date: 2003/10/22 08:44:20EDT $
   $Revision:


//=================================================================================*/

#ifndef IO_HELPERS_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define IO_HELPERS_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#include "product.h"
#include "IoMan.h"

inline void Puts(const char *pText)
{
	theStdInterface.Write(pText);
}

inline void Puts(const bchar_t *pText)
{
	theStdInterface.Write(pText);
}

inline void Puts(const wchar_t *pText)
{
	theStdInterface.Write(pText);
}

#ifdef _MSC_VER
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#endif

#endif // IO_HELPERS_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_




