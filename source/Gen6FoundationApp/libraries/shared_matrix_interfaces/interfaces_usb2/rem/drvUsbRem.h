/*===================================================================
  See .cpp file for description 
=====================================================================
   $Source: interfaces_usb2/rem/drvUsbRem.h $
   $Date: 2011/03/31 14:14:26EDT $
   $Revision: 1.1 $


===================================================================*/
#ifndef _drvUsbRem_h_
#define _drvUsbRem_h_

#include <stddef.h>
#include "stdInclude.h"
#include "threadeddriver.h"
#include "usb2base.h"


class CDriverUsbRem : public CThreadedDriver
{
public:
	CDriverUsbRem(void){m_pSharedUsb = NULL;}
	virtual MatrixError_t Open(interface_id_t nId, const tichar_t *pParameter);
	virtual MatrixError_t Close(void);
	virtual MatrixError_t DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam);
	
protected:
	CUsb2Base * m_pSharedUsb;
	virtual MatrixError_t ExtractParameters(const tichar_t *pParameters ) {return ERR_PASS;}
	virtual int StartSending(void);
};


#endif // endof #ifndef _drvUsbRem_h_



