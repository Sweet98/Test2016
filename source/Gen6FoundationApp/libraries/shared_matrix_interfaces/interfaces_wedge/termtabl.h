/*=================================================================================


//=================================================================================
   $Source: interfaces_wedge/termtabl.h $
   $Date: 2009/06/29 05:03:01EDT $
   $Revision:


//=================================================================================*/

#ifndef WEDGE_TERMINAL_TABLE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define WEDGE_TERMINAL_TABLE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#include "product.h"
#include "WdgType.h"
#include "WedgeScancode.h"
#include "WedgeMakeBreak.h"
#include "WedgeKey.h"
#include "WedgeFixup.h"
#include "WedgeKeyNumber.h"
#include "WedgeChar2Key.h"
#include "WedgeTerminal.h"

/* Forward declarations for pointer tables */
extern const CKeyFixups IBM_XT_FixupPtr;
extern const CKeyFixups IBM_AT_FixupPtr;
extern const CKeyFixups JAP_106_USB_IBM_AT_FixupPtr;
extern const CKeyFixups JAP_106_USB_DEC_VT510_PC_FixupPtr;
#ifndef UNICODE_FOR_INTERFACES
extern const CKeyFixups JAP_106_USB_DEC_VT510_LK411_FixupPtr;
#endif
extern const CKeyFixups DEC_VT420_FixupPtr;
extern const CKeyFixups DEC_VT510_PC_FixupPtr;
extern const CKeyFixups IBM_3486_102_FixupPtr;
extern const CKeyFixups IBM_3191_122_FixupPtr;
extern const CKeyFixups IBM_3486_122_FixupPtr;
extern const CKeyFixups Telex_88_FixupPtr;
extern const CKeyFixups BDS_7_FixupPtr;
extern const CKeyFixups APPLE_ADB_FixupPtr;
extern const CKeyFixups DEC_VT510_LK411_FixupPtr;
extern const CKeyFixups Telex_88_DATA_ENTRY_FixupPtr;
extern const CKeyFixups USA_Only_FixupPtr;
extern const CScancodes USB_KEYBOARD_SCAN;
extern const CScancodes JAP_106_USB_KEYBOARD_SCAN;
extern const CMakeBreaks USB_KEYBOARD_MKBR;

#define KEYBOARD_PRG_VERSION 100	/* This version gets changed when table structure changes */

#endif // WEDGE_TERMINAL_TABLE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_




