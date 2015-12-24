/* ==============================================================================
   USB Strings
   ==============================================================================

   $RCSfile: interfaces_usb2/base_usb2/usb2stringdesc.cpp $
   $Revision: 1.6 $
   $Date: 2010/02/02 15:08:47EST $

//=================================================================================

//=================================================================================*/
//! \file

#include "stdInclude.h"
#include <stddef.h>
#include "product.h"
#include "usb2.h"
#include "hstring.h"

extern const UCHAR pxLanguageStringDescriptorA[] =
{
	4,
	0x03,
	0x09, 0x04
};

#ifdef YJ2D_BASED_ON_HI2D

extern const UCHAR pxManufacturerStringDescriptorA[USB_SIZE_STRING_MANUFACTURER] = 
{
	USB_SIZE_STRING_MANUFACTURER,
	0x03,
	
	UNI(' '), UNI(' '), UNI(' '), UNI(' '),	UNI(' '), UNI(' '), UNI(' '), UNI(' '),	UNI(' '), UNI(' '),
	UNI(' '), UNI(' '), UNI(' '), UNI(' '),	UNI(' '), UNI(' '), UNI(' '), UNI(' '),	UNI(' '), UNI(' '),
	UNI(' '), UNI(' '), UNI(' '), UNI(' '), UNI(' '), UNI(' '), UNI(' '), UNI(' '), UNI(' '), UNI(' '),
	UNI(' '), UNI(' '),
};

#else

extern const UCHAR pxManufacturerStringDescriptorA[USB_SIZE_STRING_MANUFACTURER] = 
{
	USB_SIZE_STRING_MANUFACTURER,
	0x03,
	
	UNI('S'), UNI('u'), UNI('p'), UNI('e'),	UNI('r'), UNI('M'), UNI('a'), UNI('x'),	UNI(' '), UNI(' '),
	UNI('I'), UNI('m'), UNI('a'), UNI('g'),	UNI('i'), UNI('n'), UNI('g'), UNI(' '),	UNI(' '), UNI(' '),
	UNI(' '), UNI(' '), UNI(' '), UNI(' '), UNI(' '), UNI(' '), UNI(' '), UNI(' '), UNI(' '), UNI(' '),
	UNI(' '), UNI(' '),
};

#endif

extern const UCHAR pxProductStringDescriptorA[USB_SIZE_STRING_PRODUCT] = 
{
	USB_SIZE_STRING_PRODUCT,
	0x03,

	UNI('2'), UNI('D'), UNI(' '), UNI('B'),	UNI('a'), UNI('r'), UNI('c'), UNI('o'),	UNI('d'), UNI('e'),
	UNI(' '), UNI('I'), UNI('m'), UNI('a'),	UNI('g'), UNI('e'), UNI('r'), UNI(' '),	UNI(' '), UNI(' '),
	UNI(' '), UNI(' '), UNI(' '), UNI(' '), UNI(' '), UNI(' '), UNI(' '), UNI(' '), UNI(' '), UNI(' '),
	UNI(' '), UNI(' '), 
};

extern const UCHAR pxSerialStringDescriptorA[USB_SIZE_STRING_SERIAL] =
{
	USB_SIZE_STRING_SERIAL,
	0x03,

	UNI('0'), UNI('0'), UNI('0'), UNI('0'),	UNI('0'), UNI('0'), UNI('0'), UNI('0'), UNI('0'), UNI('0'), 
	UNI('0'), UNI('0'), UNI('0'), UNI('0'),	UNI('0'), UNI('0'), UNI('0'), UNI('0'), UNI('0'), UNI('0'), 
	UNI('0'), UNI('0'), UNI('0'), UNI('0'),	UNI('0'), UNI('0'), UNI('0'), UNI('0'), UNI('0'), UNI('0'), 
	UNI('0'), UNI('1'), 
};

extern const UCHAR pxConfigurationStringDescriptorA[USB_SIZE_STRING_CONFIG] = 
{
	USB_SIZE_STRING_CONFIG,
	0x03,

	UNI('D'), UNI('e'), UNI('f'), UNI('a'), UNI('u'), UNI('l'), UNI('t'),
};

   
