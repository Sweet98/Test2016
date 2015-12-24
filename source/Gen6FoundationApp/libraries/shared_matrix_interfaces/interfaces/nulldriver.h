/*=================================================================================
  class: CNullDriver
 
//=================================================================================
   $Source: interfaces/nulldriver.h $
   $Date: 2011/06/06 15:19:17EDT $
   $Revision:
 

//=================================================================================*/

#ifndef NULL_DRIVER_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define NULL_DRIVER_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

class CBarcodeData;

class CNullDriver : public CDriverBase
{
public:
	CNullDriver(void) : CDriverBase(2, 2){}; // don't bother creating large TX & RX buffers, but DO create them.  There's so many methods that aren't overloaded, one of them might reference the pointers and they better be valid.  I used 2 instead of 1 because I didn't want to "try my luck" to see if the fifo classes handled it.
	virtual MatrixError_t Open(interface_id_t nId, const tichar_t *pParameter) {return ERR_PASS;}
	virtual MatrixError_t Close() {return ERR_PASS;}

	// We don't want to call into the base's Write() because it would either fail once we filled up a fifo, and/or create a new buffer (CBarcodeData::StoreToNewBuffer()).
	virtual size_t Write(CBarcodeData  *pLabel) { return 1;} // Do we need to free any data in pLabel here?  Right now, no--I don't think it's used in our code.
	// Some code returns the size of the data, some returns 1 (I guess that means "true" or "ok").  Most seems to return 1, so we will here as well.
	virtual size_t Write(const bchar_t *pBin, size_t nSize) { return 1;}
	virtual size_t Write(const wchar_t *pText, size_t nSize) { return 1;}

	virtual size_t Read(bchar_t *pDestination, size_t size) {return 0;}
	virtual size_t Peek( bchar_t *pDestination, size_t offset = 0 ) {return 0;}
	virtual size_t IsAnyRxData(void) {return 0;}

	virtual IoControlRet_t DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam);
	virtual int GetTxFifoFillLevel(void) {return 0;}
protected:
};

#endif // NULL_DRIVER_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_




