/*=================================================================================
  class: CInterfaceTable

//=================================================================================
   $Source: interfaces/if_table.h $
   $Date: 2006/03/22 18:22:58EST $
   $Revision: 1.16 $


//=================================================================================*/
//! \file

#ifndef INTERFACE_ID_TABLE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define INTERFACE_ID_TABLE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

class CDriverBase;


///////////////////////////////////////////////////////////////////////////////
//! Forms an entry in the list of interface drivers.
/*!
*/
class CInterfaceTable
{
public:		// unfortunatelly we need public to allow for the initializers in C style
	interface_id_t m_termid;						//!< The ID of the interface.
	USHORT	m_Type;									//!< The type of the interface (wedge, serial, ...).
//	USHORT	m_Capabilities;
	model_t	m_Models;								//!< Device models (4600, 4080,..) supported by this interface
	CDriverBase *(*pClassFactory)(void);		//!< Function pointer to a creator function that instanciates the driver.
//	int placeholder;
	enum
	{
		End=0x7fff
	};
//	enum capabilities
//	{	// binary coded
//		// future expansion, would be wasted ba alignment anyway
//	};

	//! Interface types.
	enum type
	{ // numbered
		Unknown = 0,
		MinParams,
		Serial,
		SerialWedge,
		Wedge,
		Ibm4683,
		IbmSurePos,
		Ocia,
		WandEm,
		HHLC,
		PicoNet,
		BlueTooth,
		Emulate3700,
		UsbComPort,
		UspHidPos
	};
	//! Device models as a bitmask.
	enum models
	{	// binary coded
	  	None					=0x00,
		Standard	  			=0x01,
		True232	  				=0x02,
		Standard_True232		=0x03,
		Hhlc					=0x04,
		IT4080					=0x08,
		Standard_IT4080			=0x09,
		Hhlc_IT4080				=0x0C,
		IT4080woUSB				=0x10,
		Standard_IT4080woUSB	=0x11,
		IT4080_IT4080woUSB		=0x18,
		Hhlc_IT4080_IT4080woUSB	=0x1C,
		IBM_4683  				=0x20,
		Standard_4683			=0x21,

		//LiteOnly				=0x80,

		All						=0xFFFFFFFF
		//AllNonLite			=All&~LiteOnly
	};

};


//! Instantiate a dummy driver.
CDriverBase *CreateNullDriver(void);


/*		Macros for Interface table generation:
 * 
 * 	The definition of the "DRIVER" macro function is based on the target firmware
 * 	being built.  Each firmware target uses a unique macro parameter to select
 * 	whether the table entry is included.
 * 
 * 	The parameters to the DRIVER macro are:
 * 		Id		-	the Interface ID number.
 * 		Nm		-	the Entry point name of the driver open function (with the "Create"
 * 					prefix omitted).
 * 		T		-	the interface type from enum type in class CInterfaceTable.
 * 		M		-	the valid models bitmask from enum models in class CInterfaceTable.
 * 		D		-  entry selector for Default (Matrix) firmware.
 * 		B		-	entry selector for Matrix Cordless Base firmware.
 * 		S		-	entry selector for Matrix Cordless Scanner firmware.
 * 		C		-	entry selector for CF2D firmware.
 * 		E		-	entry selector for Emerald Verifier firmware.
 * 
 * 	The entry selector fields specify the interfaces to include in the table for a 
 * 	specific firmware build image.  They must be one of the following:
 * 		Y	-	include this interface
 * 		ND -	include this interface, but substitute "CreateNullDriver" for the 
 * 				specified driver.
 * 		N	-	do not include this interface in the list.
 */

#if	defined(IF_TABLE_MACROS)		// these macros are only available where this is defined...

  #if	defined(CF2D)
    #define DRIVER(Id, Nm, T, M, D, B, S, C, E)		DRVR_##C(Id, Nm, T, M)

  #elif	defined(EMERALD_VERIFIER)
    #define DRIVER(Id, Nm, T, M, D, B, S, C, E)		DRVR_##E(Id, Nm, T, M)

  #elif	defined(RFBASE)
    #define DRIVER(Id, Nm, T, M, D, B, S, C, E)		DRVR_##B(Id, Nm, T, M)

  #elif	defined(RFSCAN)
    #define DRIVER(Id, Nm, T, M, D, B, S, C, E)		DRVR_##S(Id, Nm, T, M)

  #else											// the default
    #define DRIVER(Id, Nm, T, M, D, B, S, C, E)		DRVR_##D(Id, Nm, T, M)
  #endif

  #define	DRVR_Y(Id, Nm, T, M)	{ Id, CInterfaceTable::##T, CInterfaceTable::##M, Create##Nm },
  #define	DRVR_ND(Id, Nm, T, M)	{ Id, CInterfaceTable::##T, CInterfaceTable::##M, CreateNullDriver },
  #define	DRVR_N(Id, Nm, T, M)
  #define	DRVR_(Id, Nm, T, M)

  // Define end of table (flag) entry
  #define	INTERFACE_TABLE_END		DRVR_Y(CInterfaceTable::End, NullDriver, Unknown, All)

#endif	/* IF_TABLE_MACROS */

#endif // INTERFACE_ID_TABLE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_





