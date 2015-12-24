/**************************************************************************************/
//
// HIM 48X0G GENERATION 6 SOFTWARE
//
//  Date:       10/31/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: app_kernel/Abort.h $
// $Revision: 1.1 $
// $Date: 2008/11/03 13:37:48EST $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// HEADER:  Abort.h
//
//	This file contains reset and abort handling functions
// 
//------------------------------------------------------------------------------
#ifndef ABORT_H_
#define ABORT_H_


// Data abort memory check routines
void EnableMemoryCheck( void );
void DisableMemoryCheck( void );
int DataAbortCount( void );
void ResetDataAbortCount( void );


void DumpMemory(const unsigned int * base, unsigned int word_count);
void DumpRegisters(const unsigned int * save_base, const char * line_indent);

#endif /*ABORT_H_*/


