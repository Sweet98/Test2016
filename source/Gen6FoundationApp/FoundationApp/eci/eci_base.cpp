/* ==============================================================================
   ECI Base Class
   ==============================================================================

   $RCSfile: Gen6FoundationApp/FoundationApp/eci/eci_base.cpp $
   $Revision: 1.1 $
   $Date: 2009/03/05 14:45:51EST $

   ==============================================================================

   ============================================================================== */
//! \file
// Base class for the ECI classes

#include "stdInclude.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uchar.h"
#include "Eci_Base.h"

CEciBase::CEciBase(void)
{
	m_ModeFlags = ECIMO_None;
	m_Contents = empty;
}


// A helper that Searches for a terminating character
size_t CEciBase::ParseEndOfECI(const bchar_t *pIn, size_t size, bchar_t Endmark)
{
	size_t SkipSize=1;
	while(*pIn++ != Endmark)
	{
		SkipSize++;
		if(SkipSize >= size)
			break;
	}
	return SkipSize;
}


