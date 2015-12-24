/*===================================================================
  This is the CIoMan interface for USB ReM.  To avoid peppering code with if statements
  to see if there's a valid interface present, this interface will always be present--no 
  matter what the main interface is or device we are.  
  
  This shares the CUsb2Base class the main USB interface uses.  Thus, the main USB interface must tell 
  this interface the value of its CUsb2Base pointer.  We accomplish this in CDriverUsb2Base's Open() and
  Close() methods.  If there's an active USB interface, m_pSharedUsb will be set to it.  If there isn't,
  then m_pSharedUsb is NULL and this class effectively becomes a dummy interface.  As a necessary
  side effect, this interface never closes when the main interface changes.  We just adjust m_pSharedUsb.
  This is necessary because the firmware architecture lacks an entity that is responsible for opening
  and closing interfaces--it's done inside each CIoMan interface, so no one really knows when the main interface
  changes.
  
  In iftable.cpp, we set the interface type to MinParams.  However, we don't want to use any standard parameters
  because they are related to the main interface.  We avoid any future issues by supplying the parameter string
  to theRemInterface.Open().  We don't pass NULL because I saw something odd (unfortunately, I didn't write it
  down and now I can't remember what it was).
    
  If we're in RS232, m_pSharedUsb will be NULL.
  
  If we're RFSCAN, the null (dummy) interface is created instead of this one (see if_table.cpp).  We probably
  could've used this class, but it would involve including some USB files--the null interface is cleaner.  
  
  Note:  if you don't override ExtractParameters(), you must pass a minimum number of parameters to the Open() 
  method, either via if_table.cpp or via the Open() call itself.  If you don't, the base class' ExtractParameters()
  will fail if it doesn't find all of them and Open() will fail and the thread loop won't run.  Since all variables
  are initialized in CDriverBase's constrructor, we don't need to bother passing the string.
=====================================================================
   $Source: interfaces_usb2/rem/drvUsbRem.cpp $
   $Date: 2011/03/31 14:14:26EDT $
   $Revision: 1.1 $


===================================================================*/
#include <assert.h>
#include "menu.h"
#include "ReaderConfiguration.h"
#include "drvUsbRem.h"
#include "ioman.h"


/*----------------------------------------------------------------------------------------
CreateUsbRem

  This is called by the big if_table.cpp macro to create this interface.

  Parameters:  none

  Returns:  a pointer to this instance of this class.
----------------------------------------------------------------------------------------*/
CDriverBase * CreateUsbRem(void)
{
	return (new CDriverUsbRem);
}




/*----------------------------------------------------------------------------------------
Open

  Called to open the interface.  
  
  To ensure this doesn't get repeatedly called when REMIFC changes, we use the small default
  parameters during the Open() (see if_tablle.cpp & 
  
  Parameters:  
  			nId[in]:  TERMID

  			pParameters[in]:  parameters specified in the Open() call (not used).

  Returns:  nothing.
----------------------------------------------------------------------------------------*/
MatrixError_t CDriverUsbRem::Open(interface_id_t nId, const tichar_t *pParameters)
{
	MatrixError_t OpenStatus = CThreadedDriver::Open(nId, pParameters);
	if ( OpenStatus == ERR_PASS ) RunThread();
	
	return OpenStatus;
}



/*----------------------------------------------------------------------------------------
Close

  This doesn't do anything.  I left it in because it's a great place to set a breakpoint
  for debugging.

  Note:  CIoMan only calls Close() when it sees the TERMID change.  We probably could
  change the stored value when IOCTRL_REM_SET_USB_DRV sets m_pSharedUsb to NULL to force
  it to call Close(), but repeatedly calling Open() with no Close() has no side 
  affects--the code was written to allow it.

  Parameters:  none

  Returns:  nothing.
----------------------------------------------------------------------------------------*/
MatrixError_t CDriverUsbRem::Close(void)
{
	return CThreadedDriver::Close();
}


/*----------------------------------------------------------------------------------------
DeviceIoControl

  Handles necessary IOCTs.  
  
  Note:  the ReM spec doesn't cover what we should do when the customer does something that 
  shuts down the USB port.  We could delay until all the ReM data is sent, but that could 
  affect the customer.  We could flush the TX buffers or wait and if we come back as USB, 
  we could try to finish sending the data.  We flush the TX data.

  Parameters:  
  			nId[in]:  TERMID

  			pParameters[in]:  parameters specified in the Open() call (not used).

  Returns:  nothing.
----------------------------------------------------------------------------------------*/
IoControlRet_t CDriverUsbRem::DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam)
{
	IoControlRet_t IoRet = CThreadedDriver::DeviceIoControl(cmd, lparam, wparam);

	switch(cmd)
	{
		case IOCTRL_REM_SET_USB_DRV:
		{	// a NULL value is sent when the port is closing.
			m_pSharedUsb = (CUsb2Base *)wparam;
			if ( m_pSharedUsb ) m_pSharedUsb->SetRemRxFifo(m_pRxFifo); // this could be a new usb iface, so set its rx fifo pointer
			else FlushTxFifo();  // There shouldn't be any data in it, but to be safe, lets clear it out in case the main interface is changing USB ports (we don't want to send "old stuff" on the new ReM USB interface that's created
			IoRet = IOCRET_YES;
		}
		break;
		
		case IOCTRL_IS_DONE:
			if ( IoRet == IOCRET_YES )
			{
				if ( m_pSharedUsb )
				{
					if ( ! m_pSharedUsb->IsRemTxComplete() ) IoRet = IOCRET_NO;
				}
			}
		break;
	}
	return IoRet;
}



/*----------------------------------------------------------------------------------------
StartSending

  This is the overload that actually sends data.  We need to be careful and free all
  necessary resources (see comments below).

  Parameters:  none

  Returns:  0.
----------------------------------------------------------------------------------------*/
int CDriverUsbRem::StartSending(void)
{
	CBarcodeData * pBarcodeData = GetBarcodeToSend(); // you need to call this to remove the "barcode" from the fifo.  NOTE:  a copy of this pointer is also made in CDriverBase.
	assert(pBarcodeData);  // The thread should never be calling us unless there's data
	if ( GET_SETTING(RemIfaceEnable) && m_pSharedUsb )
	{
		const unsigned char * pData;
		size_t DataLength;
		pBarcodeData->GetTotalData(pData, DataLength);
		m_pSharedUsb->SendToRem(pData, DataLength);
	}
	
	SentBarcode(); // you need to call this to free the buffer associated with pBarcodeData (CBarcodeData)
	return 0; // almost all classes (except for HidPosHelper) return 0 (no error code)
}



