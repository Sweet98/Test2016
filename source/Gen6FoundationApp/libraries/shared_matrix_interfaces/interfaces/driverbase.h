/*=================================================================================
  class: CDriverBase
 
//=================================================================================
   $RCSfile: interfaces/driverbase.h $
   $Date: 2011/11/14 13:33:50EST $
   $Revision: 1.24.1.4 $
 

//=================================================================================*/
//! \file

#ifndef DRIVER_BASE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define DRIVER_BASE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#include "IoControl.h"
#include "BarcodeData.h"
#include "rxfifo.h"
#include "interfacetypedefs.h"


class CBarcodeData;
class CRxFifo;
class CBarcodeFifo;

class CDriverBase
{
	friend class CWedgeLabel;
public:
	enum {DEFAULT_BUFFER_SIZE_TX = 1000, DEFAULT_BUFFER_SIZE_RX = 2100};

	CDriverBase(size_t txbuffersize=DEFAULT_BUFFER_SIZE_TX,size_t rxbuffersize=DEFAULT_BUFFER_SIZE_RX);
	virtual ~CDriverBase();

	virtual size_t Write(CBarcodeData  *pLabel);	//default writes to a fifo
	virtual size_t Write(const bchar_t *pBin, size_t nSize);
	virtual size_t Write(const wchar_t *pText, size_t nSize);

	/// Only used for the BT stack. Do not use for other purposes!
	virtual int DirectWrite(int,unsigned char *);

	virtual size_t Read(bchar_t *pDestination, size_t size);  //defaults reads from a fifo
	virtual size_t Peek( bchar_t *pDestination, size_t offset = 0 ); //defaults reads from a fifo but does not increment fifo pointer
	virtual size_t IsAnyRxData(void);	            // returns the amount of RX data
	virtual MatrixError_t Open(interface_id_t nId, const tichar_t *pParameter)=0;
	virtual MatrixError_t Close();
	virtual MatrixError_t DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam);
	virtual int GetTxFifoFillLevel(void);

	///////////////////////////////////////////////////////////////////////////////
	//! Write received data to the receive fifo.
	/*! This is an inline function for speed.
	 \param *pBin Point to the buffer with bytes to write
	 \param nSize Amount of bytes to write
	 \return Amount of bytes written
	*/
	size_t StuffRxBuff(const bchar_t *pBin, size_t nSize)
	{
		return m_pRxFifo->Write((const bchar_t *)pBin,nSize);
	}
	size_t StuffRxBuff(char *pBin, size_t nSize)
	{
		return m_pRxFifo->Write((const bchar_t *)pBin,nSize);
	}

	// more as we need it

	// Check if we have a barcode to send in the TxFifo
	size_t AnyDataToSend(void) const;
	// Retrieves the next available barcode in the TxFifo.
	// Uses the m_BarcodeDummy if empty. (This should never happen if your code is correct)
	CBarcodeData *GetBarcodeToSend(void);
	// Call this after you are done with the barcode (frees the buffer etc.)
	void SentBarcode(void);

	// delay helpers
	void SetInterCharDelay(ULONG delay)
	{
		m_DelayInterCharacter = delay;
	}
	void SetInterFunctionDelay(ULONG delay)
	{
		m_DelayInterFunction = delay;
	}
	void SetInterMessageDelay(ULONG delay)
	{
		m_DelayRawInterMessage = delay;
		m_DelayInterMessage = delay;
	}

	ULONG GetInterCharDelay(tichar_t ansi);
	ULONG GetInterCharDelay(void)
	{
		return m_DelayInterCharacter;
	}
	ULONG GetCustomCharDelay(void)
	{
		return m_DelayCustomInterCharacter;
	}
	ULONG GetInterFunctionDelay(void)
	{
		return m_DelayInterFunction;
	}
	ULONG GetInterMessageDelay(void)
	{
		return m_DelayInterMessage;
	}

	bool IsAnyCharacterDelays(void);

	interface_id_t GetTerminalID(void)
	{
		return m_TermId;
	}

	bool HadAnyGoodTransfersYet(void)
	{
		return m_HadAtLeastOneGoodTransfer;
	}

	// Remove all CBarcodeData from the TX fifo and mark them as sent
	MatrixError_t FlushTxFifo(void);                                    //lmc   make public

protected:
	/// Sets the LastError variable
	void SetLastError(MatrixError_t LastError)
	{
		m_LastError = LastError;
	}

	// A helper function to extract standard parameters from the string
	virtual MatrixError_t ExtractParameters(const tichar_t *pParameters );

	// try to fix wrong formats in the label
	void AutoTranslate(CBarcodeData  *pLabel);

	/// The receive fifo
	CRxFifo *m_pRxFifo;
	/// The transmit fifo
	CBarcodeFifo *m_pBarcodeFifo;
	/// The barcode that we currently try to send
	CBarcodeData *m_pCurrentBarcodeToSend;

	IoDataType_t m_OutputFormat;				//!< We can send in binary, UTF8, UTF16BE, UTF16LE, systemcodepage
	bool m_DriverSupportsProgrammableOutputFormats;	//!< Show whether a driver supports output formats (UTF8, ..)

	// delay values (uSecs)
	ULONG m_DelayRawInterMessage; 	      //!< global value from the menu system
	ULONG m_DelayInterMessage;					//!< value = m_DelayRawInterMessage + GetAdditionalInterMessageDelay()
	ULONG m_DelayInterFunction;				//!< Delay between control characters if the interface supports it
	ULONG m_DelayInterCharacter;				//!< Delay between characters if the interface supports it
	ULONG m_DelayCustomInterCharacter;		//!< Delay after a special character if the interface supports it
	int m_DelayCharacter;						//!< Character for the above delay

	interface_id_t m_TermId;					//!< in case we need it later

	/// This holds the last error until it get read by the application main loop.
	MatrixError_t m_LastError;

	/// this helps to make the code more robust in error situations
	//CBarcodeData m_BarcodeDummy;

	bool m_HadAtLeastOneGoodTransfer;      //!< helps with wrong cables

private:
	CDriverBase (const CDriverBase&);            // declared private, but not defined, to prevent copying         
	CDriverBase& operator=(const CDriverBase&);  // declared private, but not defined, to prevent copying 
	
};

#endif // DRIVER_BASE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_





