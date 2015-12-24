/*=================================================================================
	Use this table to add or replace custom interfaces.
	It is searched before the standard table
//=================================================================================
   $Source: customization/if_custom.cpp $
   $Date: 2005/04/04 11:46:06EDT $
   $Revision: 1.2 $


//=================================================================================*/

#include "stdInclude.h"
#include "product.h"
#include "TermId.h"
#define	IF_TABLE_MACROS		// enable table generation macro definitions
#include "if_table.h"

// nobody except CTermID is allowed to access this table!

/*
 * The CustomTerminals[] table below is created with a macro.  In order to reduce
 * duplication in the entries in this table, the common part of Interface
 * names is ommitted, as well as the class specifications on the Type and
 * Model enums used here.  The macro adds these prefixes.
 * Note that the entries do not use tabs as field separators.
 *
 * If "Example" is specified as the Interface, this is really a reference
 * to "CreateExample".  The Class prefix for the type and Model fields 
 * is "CInterfaceTable::"
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
 * (See ID=135 in iftable.cpp for an example.)
 */

CInterfaceTable CustomTerminals[] =
{
   //                                                               Build Entry Selectors
	// ID       Interface             Type         Models            D  B  S   C  E
//	DRIVER(999, Example,              MinParams,   All,              Y, Y, N,  Y, Y )  // example

// Add new interface above ^^^^^^
	INTERFACE_TABLE_END
};



