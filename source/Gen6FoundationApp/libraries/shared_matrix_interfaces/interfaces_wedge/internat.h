/*=================================================================================
	An include file to collect all required includes for the keyintf.cpp.

//=================================================================================
   $Source: interfaces_wedge/internat.h $
   $Date: 2009/06/29 05:02:26EDT $
   $Revision: 2.1 $



//=================================================================================*/

#ifndef WEDGE_TERMINAL_TABLE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define WEDGE_TERMINAL_TABLE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#include "CommonDriver.h"
#include "WdgType.h"
#include "WedgeKey.h"
#include "WedgeFixup.h"

// Protection agains wrong versions.m (keyintf.cpp is a computer generated file (forkey)
#ifdef UNICODE_FOR_INTERFACES
#define INTERNATIONAL_PRG_VERSION 201	/* This version gets changed when table structure changes */
#else
#define INTERNATIONAL_PRG_VERSION 101	/* This version gets changed when table structure changes */
#endif

#endif // WEDGE_TERMINAL_TABLE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_




