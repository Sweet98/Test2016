//======================================================================================
// db_NonMenuHModem.c
//======================================================================================
// $RCSfile: Matrix/MatrixNonMenuHModem.cpp $
// $Revision: 1.1 $
// $Date: 2009/03/08 15:58:07EDT $
//======================================================================================

//======================================================================================




#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>



#include "MatrixNonMenuHModem.h"
#include "db_chrio.h"
#include "db_hdw.h"
#include "MatrixHcomm.h"
#include "Matrixhtransferstatus.h"









//
// In order to use HModem, several members of the HModem class must be overridden.  The NonMenuHModem class accomplishes this.
//



//
//	Constructor
//		Nothing to do.
//
NonMenuHModem::NonMenuHModem(void)
	    : HMODEM(ComDevice, TransferStatusMonitor, this), HCOMMAND_HANDLER()
{
	return;
} // NonMenuHModem constructor



//
//	Destructor
//		Nothing to do.
//
NonMenuHModem::~NonMenuHModem(void)
{
	return;
} // NonMenuHModem destructor



