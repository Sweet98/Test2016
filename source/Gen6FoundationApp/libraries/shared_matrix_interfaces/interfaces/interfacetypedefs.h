/*=================================================================================
  Declarations for interface drivers.

//=================================================================================
   $Source: interfaces/interfacetypedefs.h $
   $Date: 2005/03/02 07:39:00EST $
   $Revision: 1.10 $


//=================================================================================*/
//! \file

#ifndef INTERFACE_TYPES_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define INTERFACE_TYPES_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

class CInterfaceTable;
class CDriverBase;
class CTermialID;
class CBarcodeData;
class CMenuGlue;
class CDriverBase;
class CIoMan;
class CThreadRealTime;

typedef short interface_id_t;						//!< Interface ID.
typedef USHORT interface_type_t;					//!< Type of interface (wedge, serial, ..).
typedef unsigned long model_t;					//!< Model info (4600, 4080, ...).

//! Signals to wakeup a waiting CThreadRealTime.
enum
{
	Sig_BarcodeSent		=0x01,
	Sig_BarcodeSent2		=0x02,
	Sig_NewBarcodeToSend	=0x04,
	Sig_Resend				=0x08,
	Sig_ByteSent			=0x10,
	Sig_ByteReceived		=0x20,
	Sig_BigBangDone		=0x40,
	Sig_Configured			=0x80,
	Sig_CustomEvent1		=0x100,
	Sig_4683Event			=Sig_CustomEvent1,
	Sig_CustomEvent2		=0x200,
	Sig_CustomEvent3		=0x400,
	Sig_CustomEvent4		=0x800,
	Sig_MenuResponse		=0x1000
};


#endif // INTERFACE_TYPES_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_




