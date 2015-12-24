/*=================================================================================
  class: CInterfaceTable

//=================================================================================
   $Source: interfaces/if_table.cpp $
   $Date: 2011/07/21 16:20:17EDT $
   $Revision: 1.49.7.5.1.8.1.5 $


//=================================================================================*/
//! \file

#include "stdInclude.h"
#include "product.h"
#include "TermId.h"
#define	IF_TABLE_MACROS		// enable table generation macro definitions
#include "if_table.h"


/**
	The CInterfaceTable is used to maintain the list of interface drivers and their terminal IDs.
	There are two tables:
	- the standard table	(this file)		(never add custom entries here, see next line)
	- the custom table	(if_custom.cpp)(used to customize a device with special drivers etc.)

  The CTermialID class knows both tables and searches for a driver in the custom table first.

	Each table entry contains:
	- Terminal ID
	- Pointer to the create function for the driver
	- Type info (used to extract the correct parameter from the menu system)
	- Model info (used to avoid the conditional compiling for the various models)
*/

// Declare the Create functions here:
CDriverBase *CreateExample(void);

CDriverBase *CreateNullDriver(void);


CDriverBase *CreateSerialSimple(void);
CDriverBase *CreateSerialSimpleNoThread(void);
CDriverBase *CreateSerialDriver(void);


CDriverBase *CreateIBM_XT(void);
CDriverBase *CreateIBM_PS2(void);
CDriverBase *CreateIBM_AT(void);
CDriverBase *CreateIBM_Thinkpad_PS2(void);
CDriverBase *CreateIBM_Thinkpad_AT(void);
CDriverBase *CreateDEC_VT510_PC(void);
CDriverBase *CreateIBM_3486_102(void);
CDriverBase *CreateIBM_3191_122(void);
CDriverBase *CreateIBM_3486_122(void);
CDriverBase *CreateIBM_3180(void);
CDriverBase *CreateTelex_88(void);
CDriverBase *CreateTelex_88_DataEntry(void);
CDriverBase *CreateTelex_102(void);
CDriverBase *CreateTelex_122(void);
CDriverBase *CreateDDC_3496_102(void);
CDriverBase *CreateDDC_3496_122(void);
CDriverBase *CreateIBM_3180_DATA_ENTRY(void);
CDriverBase *CreateDEC_VT510_LK411(void);
CDriverBase *CreateJap106(void);
CDriverBase *CreateNec98(void);
CDriverBase *CreateCitos_K4(void);
CDriverBase *CreateCitos_K5(void);

CDriverBase *CreateSerialWedge(void);

CDriverBase *CreateSerial4683(void);
CDriverBase *CreateOcia(void);
CDriverBase *CreateHHLC(void);
CDriverBase *CreateHHLCEncode(void);
CDriverBase *CreateHHLCEncodeC39(void);
CDriverBase *CreateLECEncode(void);
CDriverBase *CreateLECEncodeC39(void);

CDriverBase *CreateUsbPos(void);
CDriverBase *CreateUsbKbdPc(void);
CDriverBase *CreateUsbKbdMac(void);
CDriverBase *CreateUsbJapKbd(void);

CDriverBase *CreateUsbSurePosHH(void);
CDriverBase *CreateUsbSurePosTT(void);

CDriverBase *CreateUsbComEmu(void);
CDriverBase *CreateUsbRem(void);


CDriverBase *CreateWandEmSame(void);
CDriverBase *CreateWandEmC39(void);
CDriverBase *CreateWandEmC128(void);

CDriverBase	*CreateSerialRf(void);
CDriverBase *CreateSPPSerialPort(void);
CDriverBase *CreateBluetoothHid(void);


// nobody except CTermID is allowed to access this table!


//! Table of interface drivers
/*!
 * The Terminals[] table below is created with a macro.  In order to reduce
 * duplication in the entries in this table, the common part of Interface
 * names is ommitted, as well as the class specifications on the Type and
 * Model enums used here.  The macro adds these prefixes.
 * Note that the entries do not use tabs as field separators.
 *
 * If "Example" is specified as the Interface, this is really a reference
 * to "CreateExample".  The Class prefix for the type and Model fields
 * is "CInterfaceTable::"
   The Models field sets m_Models and is used in QueryInterface().
 *
 * Build entry selector parameter/column definitions:
 * 	D	-  entry selector for Default (Matrix) firmware.
 * 	B	-	entry selector for Matrix Cordless Base firmware.
 * 	S	-	entry selector for Matrix Cordless Scanner firmware.
 * 	C	-	entry selector for CF2D firmware.
 * 	E	-	entry selector for Emerald Verifier firmware.
 *
 * (Specify "Y" to include; "ND" to include, using driver "NullDriver"; "N" to exclude)
 *
 * Note that if a selector is omitted, the build defaults to no entry (N).  An Entry with 'N'
 * in all selector columns is equivalent to commenting out (disabling) the line.  This
 * allows a more consistant table layout.
 *
 * If a different driver is needed for different firmware types, use two lines for that
 * interface, only one of which may have a Y or a ND for a selector parameter.
 * (See ID=135 for an example.)
 */

CInterfaceTable Terminals[] =
{
	//                                                               Build Entry Selectors
	//     ID   Interface             Type         Models            D  B  S   C  E
//	DRIVER(999, Example,              MinParams,   All,              Y, Y, N,  Y, Y )  // example
//	DRIVER(900, SerialSimple,         Serial,      All,              Y, Y, N,  Y, Y )  // RS232    //for now at 900
	DRIVER(901, SerialSimpleNoThread, Serial,      All,              Y, Y, Y,  Y, Y )  // RS232    //for now at 901
	DRIVER(902, SerialRf,             Serial,      All,              N, N, Y,  N, Y )  // RS232 second uart (TRM_BT_HCI_IF)
#if PLATFORM_CURRENT == imx25
	DRIVER(902, SerialRf,             Serial,      All,              N, Y, N,  N, N )  // RS232 second uart (different driver in base) (TRM_BT_HCI_IF)
#endif
	DRIVER(903, SPPSerialPort,        Serial,      All,              N, Y, Y,  N, Y )  // RS232 second uart (TRM_BT_SPP_IF)
	DRIVER(904, SerialSimpleNoThread2,Serial,		  All,				  N, Y, N,  N, N )  // RS232    //for now at 904, base MX second UART for debug  //lmc-debug
	DRIVER(906, UsbRem,               MinParams,   All,              Y, Y, ND, N, Y ) // ReM USB interface. I think Models is used when searching ifaces. (no CF2D--it's legacy and I don't really know what it is)
	DRIVER(907, BluetoothHid,		  Wedge,       All,              N, N, Y,  N, N )  // RS232 second uart (TRM_BT_HID_IF)

	DRIVER(  0, SerialDriver,         Serial,      All,              Y, Y, Y,  Y, Y )  // RS232
	DRIVER(  3, IBM_AT,               Wedge,       Standard,         Y, Y, ND, N, N )  // IBM AT
	DRIVER( 51, Serial4683,           Ibm4683,     IBM_4683,         Y, Y, ND, N, N )  // IBM 4680

	DRIVER(124, UsbKbdPc,             Wedge,       Standard_IT4080,  Y, Y, ND, N, N )  // USB HID Keyboard for PC
	DRIVER(125, UsbKbdMac,            Wedge,       Standard_IT4080,  Y, Y, ND, N, N )  // USB HID Keyboard for MAC


	DRIVER(128, UsbSurePosHH,         IbmSurePos,  Standard_IT4080,  Y, Y, ND, N, N )  // IBM USB OEM POS Handheld (SurePos)
	DRIVER(129, UsbSurePosTT,         IbmSurePos,  Standard_IT4080,  Y, Y, ND, N, N )  // IBM USB OEM POS Tabletop (SurePos)
	DRIVER(130, UsbComEmu,            UsbComPort,  Standard_IT4080,  Y, Y, ND, N, Y )  // USB COM port emulation with the CDC ACM model of the USB
	DRIVER(131, UsbPos,               UspHidPos,   Standard_IT4080,  Y, Y, ND, N, N )  // USB HID POS usage page


	DRIVER(134, UsbJapKbd,            Wedge,       Standard_IT4080,  Y, Y, ND, N, N )  // USB HID Keyboard for Japan 106 key

// Add new interface above ^^^^^^
	INTERFACE_TABLE_END
};




