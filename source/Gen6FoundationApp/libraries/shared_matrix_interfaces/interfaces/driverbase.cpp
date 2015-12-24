/*=================================================================================
  class: CDriverBase

  Note:  if you don't override ExtractParameters(), you must pass a minimum number
  of parameters to your Open() method, either via if_table.cpp or via the Open()
  call itself.  If you don't, this class' ExtractParameters() will fail if it doesn't
  find all of them and Open() will fail.  TODO:  is this REALLY the way we want it
  to work?  I'd say if it can't find it, it should use a default value.
 
  Note:  there's some issue with passing an empty string in Open()--it won't open.
//=================================================================================
   $RCSfile: interfaces/driverbase.cpp $
   $Date: 2011/03/31 12:41:46EDT $
   $Revision: 1.26.2.5 $
 

//=================================================================================*/
#include "stdInclude.h"
#include "CommonDriver.h"
#include "rxfifo.h"
#include "BarcodeData.h"
#include "BarcodeFifo.h"
#include "ParseHelper.h"

#ifndef PRODUCT_CONFIGURATION_APPLICATION  // must save code size
#include "TransCodeEx.h"
#endif

#include "db_chrio.h" //edited by Chuck

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/** \class CDriverBase
	\brief This is the base class for all interface drivers.
	It defines the interface between the CIoMan and the drivers.
	Most functions are defined as virtual and will be overwritten by the derived driver class.
*/

/// size of the receive fifo (bytes)

/// size of the tx fifo (number of CBarcodeData object pointers)

/// The Constructor
CDriverBase::CDriverBase(size_t txbuffersize,size_t rxbuffersize )
{
	m_pRxFifo=NULL;
	m_pBarcodeFifo=NULL;
	if (rxbuffersize)
		m_pRxFifo = new CRxFifo(rxbuffersize);
	if (txbuffersize)
		m_pBarcodeFifo = new CBarcodeFifo(txbuffersize);
	m_DelayRawInterMessage = 0;
	m_DelayInterMessage = 0;
	m_DelayInterFunction = 0;
	m_DelayInterCharacter = 0;
	m_DelayCustomInterCharacter = 0;
	m_DelayCharacter = 0;
	m_TermId=0;
	m_pCurrentBarcodeToSend = NULL;
	m_LastError = ERR_PASS;
	m_HadAtLeastOneGoodTransfer = false;	   // helps with wrong cables
	m_OutputFormat = IO_DATATYPE_BINARY;
	m_DriverSupportsProgrammableOutputFormats = false;
}

// The Destructor
CDriverBase::~CDriverBase()
{
	if( m_pRxFifo != NULL )
	{
		delete m_pRxFifo;
		m_pRxFifo = NULL;
	}
	if( m_pBarcodeFifo != NULL )
	{
		delete m_pBarcodeFifo;
		m_pBarcodeFifo = NULL;
	}
}


/// Call to parse the parameters and init the hardware
MatrixError_t CDriverBase::Open(interface_id_t nId, const tichar_t *pParameter)
{
	//DEBUG_puts("Open 1\r\n");//edited by Chuck
	m_HadAtLeastOneGoodTransfer = false;	   // helps with wrong cables
	m_TermId = nId;
	//DEBUG_puts("Open 2\r\n");//edited by Chuck	
	return ExtractParameters(pParameter);
}

/// Call to close the interface
MatrixError_t CDriverBase::Close(void)
{
	return FlushTxFifo();
}

/// Only used for the BT stack. Do not use for other purposes!
int CDriverBase::DirectWrite(int,unsigned char *)
{
	return -2;
}

///////////////////////////////////////////////////////////////////////////////
//! Write data to the interface.
/*! Byte character string with length (binary data wellcome).
 
 \param *pBin Point to a byte string
 \param nSize Amount of bytes to be sent
 \return 1 for success, 0 for not sent
*/
size_t CDriverBase::Write(const bchar_t *pBin, size_t nSize)
{
	size_t ret=0;
	if (nSize)
	{
		CBarcodeData *pLabel = CBarcodeData::StoreToNewBuffer(pBin, nSize);
		if (pLabel)
		{
			ret = Write(pLabel);
			if (ret==0)
				pLabel->Free();
			else
				ret=nSize;
		}
	}
	return ret;

}


size_t CDriverBase::Write(const wchar_t *pText, size_t nSize)
{
	size_t ret=0;
	if (nSize)
	{
		CBarcodeData *pLabel = CBarcodeData::StoreToNewBuffer(pText, nSize);
		if (!pLabel)
			return 0;
		ret = Write(pLabel);
		if (ret==0)
			pLabel->Free();
	}
	return ret;
}


/// This default implementation writes to a fifo
size_t CDriverBase::Write(CBarcodeData  *pLabel)
{
	AutoTranslate(pLabel);
	size_t ret = m_pBarcodeFifo->Write(pLabel);
	return ret;
}

/// This default implementation reads from a fifo
size_t CDriverBase::Read(bchar_t *pDestination, size_t size)
{
	return m_pRxFifo->Read(pDestination, size);
}

/// This default implementation reads from a fifo
size_t CDriverBase::IsAnyRxData(void)
{
	return m_pRxFifo->AnyData();
}

// This default implementation reads from a fifo, but does not increment fifo pointer
size_t CDriverBase::Peek( bchar_t *pDestination, size_t offset )
{
	return m_pRxFifo->Peek(pDestination, offset);
}

/// Remove all CBarcodeData from the TX fifo and mark them as sent.
/* Be carefull when calling this function.
 Ensure that your driver is not in the middle of a transmition because we use the m_pCurrentBarcodeToSend !!
 So disable your interrupts etc.
 */
MatrixError_t CDriverBase::FlushTxFifo(void)
{
	DISABLE_INTERRUPTS();
	bool bTemp = m_HadAtLeastOneGoodTransfer;  // save the "wrong cable" helper
	SentBarcode();	// ensure the current object gets freed
	while (AnyDataToSend() > 0)
	{
		GetBarcodeToSend();
		SentBarcode();
	}
	m_HadAtLeastOneGoodTransfer = bTemp;
	RESTORE_INTERRUPTS();
	return ERR_PASS;
}

///////////////////////////////////////////////////////////////////////////////
//! The typical universal interace to device drivers.
/*! Use for non standard communication from you application to the driver.
 \param cmd A command as type IoControl_t
 \param lparam Universal parameter
 \param wparam Universal parameter
 \return MatrixError_t
*/

MatrixError_t CDriverBase::DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM )
{
	MatrixError_t RetVal=IOCRET_DEFAULT;
	switch(cmd)
	{
	case IOCTRL_IS_DONE:
		if (m_pBarcodeFifo != NULL && (m_pBarcodeFifo->AnyData() || m_pCurrentBarcodeToSend!=NULL))
			RetVal = IOCRET_NO;
		else
			RetVal = IOCRET_YES;
		break;
	case IOCTRL_GET_LAST_ERROR:
		RetVal = ERR_FAIL;
		if(lparam!=NULL)
		{
			MatrixError_t *pError = (MatrixError_t*)lparam;
			*pError = m_LastError;	// return the error to the caller and clear it
			m_LastError = ERR_PASS;
			RetVal = ERR_PASS;
		}
		break;
	default:
		break;
	}
	return RetVal;
}

///////////////////////////////////////////////////////////////////////////////
//! A helper to extract the standart parameters.
/*! Interface drivers are configured via special string commands. /sa CMenuGlueDriver
 \param *pParameters A tichar_t string (made by MenuGlue)
 \return MatrixError_t
*/
MatrixError_t CDriverBase::ExtractParameters(const tichar_t *pParameters )
{
	bool ret=true;
	//DEBUG_puts("ExtractParameters 1\r\n");//edited by Chuck
	ret &= CParseHelper::Extract(pParameters, _TI("IMDLY"), m_DelayRawInterMessage);
	ret &= CParseHelper::Extract(pParameters, _TI("IFDLY"), m_DelayInterFunction);
	ret &= CParseHelper::Extract(pParameters, _TI("ICDLY"), m_DelayInterCharacter);
	ret &= CParseHelper::Extract(pParameters, _TI("IC_X_DLY"), m_DelayCustomInterCharacter);
	ret &= CParseHelper::Extract(pParameters, _TI("DelayChar"), m_DelayCharacter);
	//DEBUG_puts("ExtractParameters 2\r\n");//edited by Chuck
	if(m_DriverSupportsProgrammableOutputFormats)
	{
		ULONG temp;
		ret &= CParseHelper::Extract(pParameters, _TI("EciOut"), temp);
		//DEBUG_puts("ExtractParameters 3\r\n");//edited by Chuck
		m_OutputFormat = (IoDataType_t) temp;
	}
	//DEBUG_puts("ExtractParameters 4\r\n");//edited by Chuck
	// now ensure some minimum values
	if(m_DelayInterCharacter > m_DelayInterFunction)
		m_DelayInterFunction = m_DelayInterCharacter;
	if(m_DelayInterCharacter > m_DelayRawInterMessage)
		m_DelayRawInterMessage = m_DelayInterCharacter;

	const size_t TimeSteps=5000;			// we've got values in 5 mSec steps
	m_DelayRawInterMessage *= TimeSteps;// make uSeconds
	m_DelayInterFunction *= TimeSteps;
	m_DelayInterCharacter *= TimeSteps;
	m_DelayCustomInterCharacter *= TimeSteps;

	m_DelayInterMessage = m_DelayRawInterMessage;  // ensure both are at least the ame value
	//DEBUG_puts("ExtractParameters 5\r\n");//edited by Chuck
	return ret ? ERR_PASS : ERR_BAD_DELAY_PARAMS;
}

/// A helper to find the appropriate delay time after a character has been sent
unsigned long CDriverBase::GetInterCharDelay(tichar_t ansi)
{
	if ( (m_DelayCustomInterCharacter > 0) && (ansi == m_DelayCharacter) )
		return m_DelayCustomInterCharacter;

	if (ansi < ' ')
		return m_DelayInterFunction;
	else
		return m_DelayInterCharacter;
}

/// Checks if there is anything to send
size_t CDriverBase::AnyDataToSend(void) const
{
	if ((this > (void *)0x100) && m_pBarcodeFifo) // TODO: JAW fix this is a cheat to prevent compier optimizing this out 
		return m_pBarcodeFifo->AnyData();
	else
		return 0;
}

/// Reads the next Barcode out of the fifo (for sending)
CBarcodeData *CDriverBase::GetBarcodeToSend(void)
{
	// only get a new one if the old one has been sent (pointer is NULL)
	if (m_pCurrentBarcodeToSend == NULL)
	{
		m_pCurrentBarcodeToSend = m_pBarcodeFifo->Read();
	}

	if (m_pCurrentBarcodeToSend != NULL)
	{
		m_DelayInterMessage = m_DelayRawInterMessage + m_pCurrentBarcodeToSend->GetAdditionalInterMessageDelay();
	}
	return m_pCurrentBarcodeToSend;
}

/// Call after your driver sent the barcode. This frees the CBarcode object for future use.
void CDriverBase::SentBarcode(void)
{
	m_pCurrentBarcodeToSend->Free();
	m_pCurrentBarcodeToSend = NULL;
	m_HadAtLeastOneGoodTransfer = true;	   // helps with wrong cables
}

/// Chacks if there are any delays between characters required.
bool CDriverBase::IsAnyCharacterDelays(void)
{
	return (	GetInterCharDelay()
	         || GetInterFunctionDelay()
	         || GetCustomCharDelay() ) ? true : false;
}

/// return number of messages buffered for sending. return -1 if no fifo is used
int CDriverBase::GetTxFifoFillLevel(void)
{
	if (m_pBarcodeFifo)
		return m_pBarcodeFifo->AnyData();
	else
		return -1;
}

/// Try to fix wrong formats in the label
void CDriverBase::AutoTranslate(CBarcodeData  *pLabel)
{
#ifndef PRODUCT_CONFIGURATION_APPLICATION  // must save code size
	theTransCode.AutoTranslate(pLabel, m_OutputFormat);
#endif
}





