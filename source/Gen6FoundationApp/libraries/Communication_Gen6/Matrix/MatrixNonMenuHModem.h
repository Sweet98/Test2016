//======================================================================================
// db_NonMenuXModem.h
//======================================================================================
// $RCSfile: Matrix/MatrixNonMenuHModem.h $
// $Revision: 1.1 $
// $Date: 2009/03/08 15:58:08EDT $
//======================================================================================

//======================================================================================




#ifndef	_NON_MENU_HMODEM_H
#define	_NON_MENU_HMODEM_H

#include	<stdlib.h>
#include	<string.h>
#include	"language.h"
#include	"HModem.h"
#include	"HComm.h"
#include	"HCommandHandler.h"


//
// In order to use HModem, several members of the HModem class must be overridden.  The NonMenuHModem class accomplishes this.
//

class NonMenuHModem : public HMODEM, public HCOMMAND_HANDLER
{
public:
	NonMenuHModem(void);
	~NonMenuHModem(void);
};


#endif



