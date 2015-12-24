/*=================================================================================
  class: CIoMan

//=================================================================================
   $RCSfile: interfaces/ioman.cpp $
   $Date: 2011/03/31 14:27:03EDT $
   $Revision: 1.80.1.7.1.8 $


//=================================================================================*/
#include <stdarg.h>
#include <stdio.h>

#include "stdInclude.h"
#include "CommonDriver.h"
#include "IoMan.h"
#include "TermId.h"
#include "MenuGlueDriver.h"
#include "BarcodeDataMan.h"
#include "If_table.h"
#include "hstring.h"
#include "timeout.h"
//#include "power.h"
#include "thread.h"

#include "db_chrio.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if (1)
#define ResetPowerTimeOuts()
#define KEY_WEDGE_ON()
#define KEY_WEDGE_OFF()
#else
#define KEY_WEDGE_ON() theHal.KeyWedgeOn()
#define KEY_WEDGE_OFF() theHal.KeyWedgeOff()
#endif
void xputhex(unsigned int hex, int width); // removeme

/** \class CIoMan
	\brief The CIoMan is the only official way to talk to an interface driver from the application level.

	It does all common work. Each CIoMan object supports a interface at a given time.
	There are these predefined interface names:
	Two CIoMan objects are predifined here (theStdInterface and theDebugInterface),
 	so we can open two interfaces at the same time.
	It is the application writers responsibility not to open two drivers which share hardware resources.

	CIoMan does not know details about the varius drivers. It rather uses the interface defined by CDriverBase
	to communicate with the drivers. (All drivers must be derived from CDriverBase).
	For an example how to write a driver see the DriverExample.cpp.

	The CInterfaceTable is used to maintain the list of interface drivers and their terminal IDs.

	CIoMan is the replacement for the 3800_CPI (if you know the IT3800).

 	Application use:
	/verbatim
 	theStdInterface.DoSomething();
	(theStdInterface is a macro that uses the pointer g_pStdInterface)
	/endverbatim

	CIoMan objects can even be called when there is no interface open yet.
*/

//int CIoMan::ms_SystemCodePage = CP1252;

CIoMan *g_pStdInterface=NULL;
CIoMan *g_pDebugInterface=NULL;
CIoMan *g_pRfInterface=NULL;
CIoMan *g_pCurrentInterface = NULL;
CIoMan *g_pBToothSPP_Interfaces[MAX_SPP_PORTS + 1];
CIoMan *g_pCurrentClientSPP_Interface = NULL;
CIoMan *g_pOWInterface = NULL;
CIoMan *g_pRemInterface = NULL;

/// Call this static member to create all CIoMan objects.
void CIoMan::CreateStdInterfaces(void)
{	
	if (g_pStdInterface == NULL)
	{
		g_pStdInterface = new CIoMan;
	}
	ASSERT(g_pStdInterface != NULL);
	if (g_pOWInterface == NULL)
	{
		g_pOWInterface = new CIoMan;
	}
	ASSERT(g_pOWInterface != NULL);
	if (g_pDebugInterface == NULL)
	{
		g_pDebugInterface = new CIoMan;
	}
	ASSERT(g_pDebugInterface != NULL);
#ifdef theRfInterface

	if (g_pRfInterface == NULL)
	{
		g_pRfInterface = new CIoMan;
	}
	ASSERT(g_pRfInterface != NULL);
#endif

	g_pCurrentInterface = g_pStdInterface;
	ASSERT(g_pCurrentInterface != NULL);


	for (int i=1;i<= MAX_SPP_PORTS; i++)
	{
		g_pBToothSPP_Interfaces[i] = new CIoMan;
		ASSERT(g_pBToothSPP_Interfaces[i] != NULL);
	}
#if (MAX_SPP_PORTS)
	g_pCurrentClientSPP_Interface = g_pBToothSPP_Interfaces[ 1 ];
		ASSERT(g_pCurrentClientSPP_Interface != NULL);
#endif
	g_pBToothSPP_Interfaces[ 0 ] = g_pStdInterface;
	ASSERT(g_pBToothSPP_Interfaces[ 0 ] != NULL);

	g_pRemInterface = new CIoMan;
}

/// Call this static member to destroy all CIoMan objects.
void CIoMan::DeleteStdInterfaces(void)
{
	delete g_pStdInterface;
	g_pStdInterface = NULL;

	delete g_pRemInterface;
	g_pRemInterface = NULL;

	delete g_pOWInterface;
	g_pOWInterface = NULL;

	delete g_pDebugInterface;
	g_pDebugInterface = NULL;

#ifdef theRfInterface

	delete g_pRfInterface;
	g_pRfInterface = NULL;
#endif


	for (int i=1;i<= MAX_SPP_PORTS; i++)
	{
		if (g_pBToothSPP_Interfaces[i] != NULL)
		{
			delete g_pBToothSPP_Interfaces[i];
			g_pBToothSPP_Interfaces[i] = NULL;
		}
	}
}

/// Constructor
CIoMan::CIoMan()
{
	m_pPort = NULL;
	SetTerminalID(EMPTY_TERM_ID);
	m_pCollector = NULL;
	m_LowPowerModeAllowed = false;
	m_LastUsedParam[0] = 0;	               // ensure we can call Open correctly
	m_LastSequenceNumber = 0;
}

/// Destructor
CIoMan::~CIoMan()
{
	Close();
}

int		CIoMan::GetLastSequenceNumber(void)
{
	return m_LastSequenceNumber;
}

void	CIoMan::SetLastSequenceNumber(int newSequenceNumber)
{
	m_LastSequenceNumber = newSequenceNumber;
}

bool	CIoMan::QueryInterface(interface_id_t termid, model_t model)
{
	return (CTermialID::QueryInterface(termid, model) != NULL);
}

const model_t DummyModel = 0xffff;	// model info is already protected in menuvects

MatrixError_t  CIoMan::Open(interface_id_t nId)
{
	return InternalOpen(nId, NULL);
}

MatrixError_t  CIoMan::Open(interface_id_t nId, const bchar_t *pParam)
{
#ifdef UNICODE_FOR_INTERFACES
	// translate to UNICODE
	wchar_t Buffer[ParameterSize];    // using 'new' would cause heap fragmentation
	if (mbstowcs(Buffer,(char *)pParam,ParameterSize)== (size_t)(-1))
	{
		DEBUG_puts("convert to wide failed \r\n");
		return ERR_FAIL;
		//MatrixError_t RetVal = theTransCode.SzSystemCPStringToUnicode(pParam, Buffer, DestinationSize);
		//	if(!IsOK(RetVal))
		//		return ERR_FAIL;
	}
	else
		return InternalOpen(nId, Buffer);
#else

	return InternalOpen(nId, pParam);
#endif
}

#ifndef PRODUCT_CONFIGURATION_APPLICATION  	// must save code size
MatrixError_t  CIoMan::Open(interface_id_t nId, const wchar_t *pParam)
{
#ifdef UNICODE_FOR_INTERFACES
	return InternalOpen(nId, pParam);
#else
	// translate to byte characters
	size_t DestinationSize=ParameterSize;
	bchar_t Buffer[ParameterSize];    // using 'new' would cause heap fragmentation
	int ret = wcstombs((char *)Buffer,pParam,ParameterSize);
	//	MatrixError_t RetVal = theTransCode.UnicodeToSystemCPString(pParam, wcslen(pParam), Buffer, DestinationSize);
	//	if(!IsOK(RetVal))
	if (ret<0)
		return ERR_FAIL;
	return InternalOpen(nId, Buffer);
#endif
}
#endif

MatrixError_t  CIoMan::InternalOpen(interface_id_t nId, const tichar_t *pParam)
{

	MatrixError_t  RetVal=ERR_PASS;
	if(GetTerminalID() != nId)	           // close if a new terminal ID is requested
	{
		RetVal = Close();
		//DEBUG_puts("InternalOpen 1\r\n");//edited by Chuck
		if ( RetVal==ERR_NOT_ALLOWED)
			return ERR_PASS;
	}

	else if ( nId == 51)				 /* We definitely do not want 4683 to change ANYTHING! */
		// This is bad practice. We need to work and remove it. Such things must be done inside the driver.
	{
		return ERR_PASS;
	}
	//DEBUG_puts("InternalOpen 2\r\n");//edited by Chuck
	if (IsOK(RetVal))
	{
		USHORT usType;
		CDriverBase *(*pClassFactory)(void);
		tichar_t Buffer[ParameterSize];    // using 'new' would cause heap fragmentation
		RetVal =  CTermialID::GetDetails(nId, DummyModel, usType, &pClassFactory);
		//DEBUG_puts("InternalOpen 3\r\n");//edited by Chuck
		if (IsOK(RetVal))
		{
			if(pParam==NULL)		           // create parameter string from menu settings if not passed
			{
				RetVal =  CMenuGlueDriver::GetSetting(usType, Buffer, ParameterSize);
				//DEBUG_puts("InternalOpen 4\r\n");//edited by Chuck
				pParam = Buffer;
			}
			if (IsOK(RetVal))						  // create driver if not done previously
			{
				if(m_pPort == NULL)
				{
					m_LastUsedParam[0] = 0;	         // allow for next call because termid was changed
					m_pPort = pClassFactory();
				}
				SetTerminalID(nId);
				//DEBUG_puts("InternalOpen 5\r\n");//edited by Chuck
				RetVal = OpenOnlyIfChanged(nId, pParam);  // initialize driver with parameters
				//DEBUG_puts("InternalOpen 6\r\n");//edited by Chuck
				if (IsOK(RetVal))
				{
					// we cache the low power mode for speed
					if (DeviceIoControl(IOCTRL_IS_LOW_POWER_ALLOWED) == IOCRET_YES)
					{
						m_LowPowerModeAllowed = true;
					}
					else
					{
						m_LowPowerModeAllowed = false;
					}
				}
				//DEBUG_puts("InternalOpen 7\r\n");//edited by Chuck
			}
		}
	}
	return RetVal;
}

///////////////////////////////////////////////////////////////////////////////
//! Calls the open of the driver only if it is really required.
/*! That means the pParam string has changed.
 We add this function to avoid potential problems if Open gets called by non driver
 related changes from the menu system.
 \param nId	Terminal ID
 \param *pParam  parameter string
 \return MatrixError_t
*/
MatrixError_t  CIoMan::OpenOnlyIfChanged(interface_id_t nId, const wchar_t *pParam)
{
	MatrixError_t RetVal=ERR_PASS;
	if(0 != wcsncmp(m_LastUsedParam, pParam, ParameterSize))  // params changed?
	{
		WaitUntilSent();
		//DEBUG_puts("OpenOnlyIfChanged 1\r\n");//edited by Chuck
		RetVal = m_pPort->Open(nId, pParam);  // initialize driver with parameters
		//DEBUG_puts("OpenOnlyIfChanged 2\r\n");//edited by Chuck
		if (IsOK(RetVal))
		{
			wcsncpy(m_LastUsedParam, (wchar_t *)pParam, ParameterSize);  // store the params for next call
			//DEBUG_puts("OpenOnlyIfChanged 3\r\n");//edited by Chuck
			switch(DeviceIoControl(IOCTRL_IS_ANA_SWITCH_ON_REQUIRED))
			{
			case IOCRET_YES:
				KEY_WEDGE_ON();
				break;

			case IOCRET_DEFAULT:
				KEY_WEDGE_OFF();
				break;
			case IOCRET_NO:
			default:
				break;
			}
		}
		else
		{
			m_LastUsedParam[0] = 0;	         // allow for next call because we failed
		}
	}
	//DEBUG_puts("OpenOnlyIfChanged 4\r\n");//edited by Chuck
	return RetVal;
}

MatrixError_t CIoMan::Open(interface_id_t nId, const tichar_t *pParam, CDriverBase *(*pClassFactory)(void))
{
	MatrixError_t  ret=ERR_PASS;
	if(m_pPort == NULL)
	{
		m_pPort = pClassFactory();
	}
	ret = OpenOnlyIfChanged(nId, pParam);  // initialize driver with parameters
	SetTerminalID(nId);
	return ret;
}

MatrixError_t  CIoMan::Close(void)
{
	MatrixError_t  ret=ERR_PASS;
	if(IsPortAvailable())
	{
		WaitUntilSent();
		ret = m_pPort->Close();									// first step of closing (flushes TxFifo)
		if(ERR_PASS==ret) 							// (dmh - for 4683)  // I had another idea in place for this (Dieter)
		{
			delete m_pPort;									// final step of closing
			m_pPort = NULL;
			m_TermId = EMPTY_TERM_ID;
			m_LastUsedParam[0] = 0;	               // ensure we can call Open correctly nect time
		}
	}
	return ret;
}

///////////////////////////////////////////////////////////////////////////////
//! The typical universal interace to device drivers.
/*! Use for non standard communication from you application to the driver.
  ! Note this routine passes commands to lower level objects first
  ! if cmd is not handled by lower level objects it must be handled here or this routine returns
  ! ERR_FAIL this differs from lower level routins which return IOCRET_DEFAULT for unhandled commands
 \param cmd A command as type IoControl_t
 \param lparam Universal parameter
 \param wparam Universal parameter
 \return MatrixError_t
*/
MatrixError_t  CIoMan::DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam)
{
	MatrixError_t RetVal = ERR_FAIL;
	if(IsPortAvailable())
	{
		RetVal = m_pPort->DeviceIoControl(cmd, lparam, wparam);

		// Here we check specific commands at a central place
		// Mostly it's the clean up for unhandled commands, so the caller gets nicer return values.
		switch(RetVal)
		{
		case IOCRET_DEFAULT:	            // driver does not know the command
			switch(cmd)
			{
				case IOCTRL_GET_PREFERED_DATATYPES:
				case IOCTRL_GET_SUPPORTED_DATATYPES:
					if(lparam!=NULL)
					{
						IoDataType_t *pTypes = (IoDataType_t*)lparam;
						*pTypes = IO_DATATYPE_TEXT;
					}
					RetVal = ERR_PASS;
					break;
//ewr-02787
				//Here are the default ERR_FAIL responses
//											
				case IOCTRL_REQUEST_HHLC_SHUT_DOWN:
//ewr-02787
				case IOCTRL_CHECK_IF_MESSAGE_NOT_SENT_IN_232_CTS_MAN:
				case IOCTRL_ACK_BACK_TO_IO_THAT_MESSAGE_NOT_SENT_IN_232_CTS_MAN:
//										
					RetVal = ERR_FAIL;
					break;
						
//ewr-02787
				//Here are the default ERR_PASS responses
//
				case IOCTRL_AM_I_ENUMERATED:
				case IOCTRL_PREPARE_FORCED_CLOSE:
					RetVal = ERR_PASS;
				break;					
//ewr-02787
				//Here are the default IOCRET_NO responses
//										
				case IOCTRL_IS_CLOSE_PROHIBITED:
				case IOCTRL_IS_BAUDRATE_AVAILABLE:
				case IOCTRL_IS_INTERPRET_DATA:
//ewr-02787
				case IOCTRL_CHECK_IF_WE_ARE_IN_232_CTS_MAN:
//					
					RetVal = IOCRET_NO;
				break;					
//ewr-02787
				//Here are the default IOCRET_YES responses
//					
				case IOCTRL_SHALL_I_BEEP_ON_POWERUP:
				case IOCTRL_SHALL_I_BEEP_ON_GOOD_READ:
				case IOCTRL_SHALL_I_BEEP_ON_MENU:
//					case IOCTRL_SHALL_I_UNTRIGGER:
//					case IOCTRL_SHALL_I_TRIGGER:
				case IOCTRL_IS_REMOTE_WAKEUP_EN:
				case IOCTRL_GET_ERROR_MSG:
						RetVal = IOCRET_YES;
				break;
//ewr-02787
					//Here are the default, unique to individual IOCTRL, responses
//
				case IOCTRL_GET_INTERCHARACTER_RECEIVE_TIMEOUT:
				{
					unsigned int		*DefaultIntercharacterReceiveTimeout = (unsigned int *) lparam;
					*DefaultIntercharacterReceiveTimeout = DEFAULT_INTERCHARACTER_RECEIVE_TIMEOUT;
					RetVal = ERR_PASS;
				}
				break;	

				case IOCTRL_GET_INTERFACE_OPEN_STRING:
				{

					if(wparam >= ParameterSize)
						wcsncpy((wchar_t *)lparam, m_LastUsedParam, ParameterSize );  //copy wide char to array passed, width 300 max
					else
						wcsncpy((wchar_t *)lparam,L"", 1 );  //copy NUL, passed string not large enough for max possible size

					RetVal = ERR_PASS;
				}
				break;

				case IOCTRL_IS_ANA_SWITCH_ON_REQUIRED:
					break;

				case IOCTRL_IS_SPP_PORT_SET:
					RetVal = IOCRET_NO;
					break;

				default:
					RetVal = ERR_FAIL;
					break;
			}		//end switch(cmd) 
			break;
		default:
			break;
		}		//end switch(RetVal)
	}
	return RetVal;
}

///////////////////////////////////////////////////////////////////////////////
//! Read bytes from the receive fifo.
/*!
 \param *pBin Point to a buffer that receives the data
 \param nSize Size of the Buffer
 \return Size of the data read
*/
size_t CIoMan::Read (bchar_t *pBin, size_t nSize)
{
	if(IsPortAvailable())
	{
		size_t RetVal=m_pPort->Read(pBin, nSize);
#if !(defined(PRODUCT_CONFIGURATION_APPLICATION) || defined(EMERALD_VERIFIER))

		if (RetVal!=0)
			ResetPowerTimeOuts();
#endif

		return RetVal;
	}
	else
		return 0;
}

///////////////////////////////////////////////////////////////////////////////
//! Checks if there is any data in the receive fifo.
/*!
 \return Amount of data in the receive fifo
*/
size_t CIoMan::IsAnyRxData (void)
{
	if(IsPortAvailable())
		return m_pPort->IsAnyRxData();
	else
		return 0;
}


///////////////////////////////////////////////////////////////////////////////
//! Read bytes from the receive fifo but does not increment fifo pointer.
/*!
 \param *pBin Point to a buffer that receives the data
 \param offset is the offset to read forwards into rx fifo (size of number of charaters wanted minus one)
 \return Size of the data read (size of offset plus one)
*/
size_t CIoMan::Peek( bchar_t *pBin, size_t offset )
{
	if( IsPortAvailable() )
	{
		size_t RetVal = m_pPort->Peek( pBin, offset );

#if !(defined(PRODUCT_CONFIGURATION_APPLICATION) || defined(EMERALD_VERIFIER))

		if( RetVal != 0 )
			ResetPowerTimeOuts();
#endif

		return RetVal;
	}
	else
	{
		return 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
//! Wait until all data have been transmitted.
/*! We now have a timeout for this call. A value is 0 which means no timeout.
 But be warned! You might stuck there if we can not send the data. This happens if the wrong cable is used!
 (Download cable rather the correct cable for the interface.)
 
 The default timeout is set to 1 Second
 
\param timeout in uSecs
 \return MatrixError_t
*/
MatrixError_t CIoMan::WaitUntilSent(timer_uSec_t timeout, bool bFlushData)
{
	MatrixError_t RetVal=ERR_PASS;
	if(IsPortAvailable())
	{
		if (m_pPort->HadAnyGoodTransfersYet())
			timeout *= 8;		               // use a larger timeout if the cable could be correct
		CTimeout timer(timeout);
		while(!IsAllSent())
		{
			if( (timeout > 0) && timer.HasExpired() )
			{
				if(bFlushData)
				{
					TxFlush();
					RetVal = ERR_FAIL;
				}

				break;
			}

			MT_Delay(1);
		}
	}
	return RetVal;
}

///////////////////////////////////////////////////////////////////////////////
// Now various overloads of the Write functions

///////////////////////////////////////////////////////////////////////////////
//! Write data to the interface.
/*! All overloads of Write end up calling this one.
 
 \param *pLabel Point to a CBarcodeData object
 \return 1 for success, 0 for not sent
*/
size_t CIoMan::Write(CBarcodeData *pLabel)
{
	if(IsPortAvailable())
	{
#ifndef EMERALD_VERIFIER
		ResetPowerTimeOuts();
#endif

		return m_pPort->Write(pLabel);
	}
	else
		return 0;
}

///////////////////////////////////////////////////////////////////////////////
//! Write data to the interface.
/*! Zero terminated wide character string.
 
 \param *pText Point to a zero terminated unicode string
 \return 1 for success, 0 for not sent
*/
size_t CIoMan::Write(const wchar_t *pText)
{
	size_t size = wcslen(pText);
	return Write(pText, size);
}

///////////////////////////////////////////////////////////////////////////////
//! Write data to the interface.
/*! Zero terminated byte character string.
 
 \param *pText Point to a zero terminated byte string
 \return 1 for success, 0 for not sent
*/
size_t CIoMan::Write(const bchar_t *pText)
{
	size_t size = strlen((char *)pText);
	return Write(pText, size);
}

///////////////////////////////////////////////////////////////////////////////
//! Write data to the interface.
/*! For Barry's string class.
 
 \param *HSTRING Point to an HSTRING object
 \return 1 for success, 0 for not sent
*/
size_t CIoMan::Write(const HSTRING *pString)
{
	size_t size = pString->Size;
	return Write((const bchar_t*)pString->Char, size);
}

///////////////////////////////////////////////////////////////////////////////
//! Write data to the interface.
/*! Byte character string with length (binary data wellcome).
 
 \param *pBin Point to a byte string
 \param nSize Amount of bytes to be sent
 \return 1 for success, 0 for not sent
*/
size_t CIoMan::Write(const bchar_t *pBin, size_t nSize)
{
	if(IsPortAvailable())
	{
#ifndef EMERALD_VERIFIER
		ResetPowerTimeOuts();
#endif

		return m_pPort->Write(pBin, nSize);
	}
	else
		return 0;
}

///////////////////////////////////////////////////////////////////////////////
//! Write data to the interface.
/*! Wide character string with separate length.
 
 \param *pText Point to a unicode string
 \param nSize Amount of unicode characters to be sent
 \return 1 for success, 0 for not sent
*/

///////////////////////////////////////////////////////////////////////////////
size_t CIoMan::Write(const wchar_t *pText, size_t nSize)
{
	if(IsPortAvailable())
	{
#ifndef EMERALD_VERIFIER
		ResetPowerTimeOuts();
#endif

		return m_pPort->Write(pText, nSize);
	}
	else
		return 0;
}

//! Similar to the ::printf()
/*! This is one of several overload for the convenient printf.
 
	If the buffersize is too small, we cut off the last part.
 
 \param *psz Format string as bchar_t
 \param ...
 \return Amount of characters written.
*/
#define	RETRIES_WHEN_NO_BARCODEDATA_AVAILABLE	1

size_t CIoMan::printf(const bchar_t *psz, ...)
{
	size_t ret=0;
	bchar_t * pBuf;
	va_list	pArgs;
	size_t BufSize = (strlen((char *)psz) * 5) + 200;
	int written;
	size_t stored=0;

	int iRetryCount = 0;
	CBarcodeData *pLabel;
	do
	{
		pLabel = theBarcodeBufferManager.GetNewBuffer(pBuf, BufSize);
		if(pLabel == NULL)
		{
			WaitUntilSent(3*1000*1000, false);  // 3 Second timeout
		}
		iRetryCount++;
	}
	while( (pLabel == NULL) && (iRetryCount <= RETRIES_WHEN_NO_BARCODEDATA_AVAILABLE) );

	if(pLabel != NULL)
	{
		va_start( pArgs, psz );
		written = _vsnprintf((char *) pBuf, BufSize-1, (char *)psz, pArgs );
		if(written < 0)
			stored = BufSize-1;
		else
			stored = written;
		pLabel->StoreNonBarcodeIds();		   						// Set AIM and HHP IDs
		pLabel->FinalizeBuffer(stored, IO_DATATYPE_CP_SYS);	// hand over the responsibility to the CBarcode object
		ret = Write(pLabel);
		if (ret==0)
			pLabel->Free();
		va_end(pArgs);
	}
	return stored;
}

///////////////////////////////////////////////////////////////////////////////
//! Similar to the ::printf()
/*! This is one of several overload for the convenient printf.
 
	If the buffersize is too small, we cut off the last part.
 
 \param *psz Format string as wchar_t
 \param ...
 \return Amount of characters written.
*/
size_t CIoMan::printf(const wchar_t *psz, ...)
{
	size_t ret=0;

	wchar_t * pBuf;
	va_list	pArgs;
	size_t BufSize = (wcslen(psz) * 5) + 200;
	int written;
	size_t stored=0;

	int iRetryCount = 0;
	CBarcodeData *pLabel;
	do
	{
		pLabel = theBarcodeBufferManager.GetNewBuffer(pBuf, BufSize);
		if(pLabel == NULL)
		{
			WaitUntilSent(3*1000*1000, false);  // 3 Second timeout
		}
		iRetryCount++;
	}
	while( (pLabel == NULL) && (iRetryCount <= RETRIES_WHEN_NO_BARCODEDATA_AVAILABLE) );

	if(pLabel != NULL)
	{
		va_start( pArgs, psz );
		written = vswprintf( pBuf, BufSize-1, psz, pArgs );
		if(written < 0)
			stored = BufSize-1;
		else
			stored = written;
		pLabel->StoreNonBarcodeIds();		   						// Set AIM and HHP IDs
		pLabel->FinalizeBuffer(stored, IO_DATATYPE_UNICODE);	// hand over the responsibility to the CBarcode object
		ret = Write(pLabel);
		if (ret==0)
			pLabel->Free();
		va_end(pArgs);
	}
	return stored;
}

///////////////////////////////////////////////////////////////////////////////
//! Similar to the ::printf()
/*! This is one of several overload for the convenient printf.
 
	If the buffersize is too small, we cut off the last part.
 
 \param *psz Format string as char
 \param ...
 \return Amount of characters written.
*/
size_t CIoMan::printf(const char *psz, ...)
{
	size_t ret=0;

	schar_t * pBuf;
	va_list	pArgs;
	size_t BufSize = (strlen(psz) * 5) + 200;
	int written;
	size_t stored = 0;

	int iRetryCount = 0;
	CBarcodeData *pLabel;
	do
	{
		pLabel = theBarcodeBufferManager.GetNewBuffer(pBuf, BufSize);
		if(pLabel == NULL)
		{
			WaitUntilSent(3*1000*1000, false);  // 3 Second timeout
		}
		iRetryCount++;
	}
	while( (pLabel == NULL) && (iRetryCount <= RETRIES_WHEN_NO_BARCODEDATA_AVAILABLE) );

	if(pLabel != NULL)
	{
		va_start( pArgs, psz );
		written = vsnprintf( pBuf, BufSize-1, psz, pArgs );
		if(written < 0)
			stored = BufSize-1;
		else
			stored = written;
		pLabel->StoreNonBarcodeIds();		   						// Set AIM and HHP IDs
		pLabel->FinalizeBuffer(stored, IO_DATATYPE_CP_SYS);	// hand over the responsibility to the CBarcode object
		ret = Write(pLabel);
		if (ret==0)
			pLabel->Free();
		va_end(pArgs);
	}
	return stored;
}

///////////////////////////////////////////////////////////////////////////////
// These functions collect the text and send it if either the buffer is full,
// or when you call SendCollected()

size_t CIoMan::Collect (const bchar_t *pText, size_t nSize)
{
	const size_t SendLimit = 150;
	// must be same type as before!
	if(m_pCollector!=NULL)
	{
		if(!m_pCollector->Is8BitData())
		{
			SendCollected();
		}
	}

	if(m_pCollector==NULL)
	{
		size_t Buffersize=200-1;
		bchar_t *pDummy;
		m_pCollector = CBarcodeData::GetSaveNewBuffer(pDummy, Buffersize);
	}
	size_t Added = m_pCollector->AddText(pText, nSize);
	if(m_pCollector->GetLabelSize() > SendLimit)
	{
		SendCollected();
	}
	return Added;
}

size_t CIoMan::Collect (const wchar_t *pText, size_t nSize)
{
	const size_t SendLimit = 150;
	// must be same type as before!
	if(m_pCollector!=NULL)
	{
		if(!m_pCollector->Is16BitData())
		{
			SendCollected();
		}
	}

	if(m_pCollector==NULL)
	{
		size_t Buffersize=200-1;
		bchar_t *pDummy;
		m_pCollector = CBarcodeData::GetSaveNewBuffer(pDummy, Buffersize);
	}
	size_t Added = m_pCollector->AddText(pText, nSize);
	if(m_pCollector->GetLabelSize() > SendLimit)
	{
		SendCollected();
	}
	return Added;
}

size_t CIoMan::Collect (const bchar_t *pText)
{
	size_t nSize = strlen((char *)pText);
	return Collect(pText, nSize);
}

size_t CIoMan::Collect (const wchar_t *pText)
{
	size_t nSize = wcslen(pText);
	return Collect(pText, nSize);
}

size_t CIoMan::SendCollected (void)
{
	size_t bytes_written =0;
	if(m_pCollector!=NULL)
	{
		bytes_written =Write(m_pCollector);
		m_pCollector = NULL;
	}
	return bytes_written;
}

///////////////////////////////////////////////////////////////////////////////
//! Stuff data into the receive buffer.
/*! This can be used to simulate received data.
 \param *pBin Point to binary data.
 \param nSize Amount of characters
 \return Amount of characters written.
*/
size_t CIoMan::StuffRxBuff(const bchar_t *pBin, size_t nSize)
{
	return m_pPort->StuffRxBuff(pBin, nSize);
}
bool	CIoMan::IsPortAvailable(void)
{
	return (m_pPort != NULL);
}


bool IsInterfaceRunning(void)
{
	return ( (g_pCurrentInterface!=NULL) ? (g_pCurrentInterface->IsPortAvailable()) : false);
}


bool DoesInterfaceDoSimpleIO(void)
{
	return ( (g_pCurrentInterface==g_pStdInterface));
}


unsigned int IntercharacterReceiveTimeoutForThisInterface( void )
{
	unsigned int	IntercharacterReceiveTimer = CIoMan::DEFAULT_INTERCHARACTER_RECEIVE_TIMEOUT;


	if( IsInterfaceRunning() )
	{
		theCurrentInterface.DeviceIoControl( IOCTRL_GET_INTERCHARACTER_RECEIVE_TIMEOUT, (LPARAM) &IntercharacterReceiveTimer );
	}

	return IntercharacterReceiveTimer;
}
// writes directly not using cbarcode etc most if dont support returns-1
// if not open -2 if not supported
int CIoMan::DirectWrite(int length,unsigned char * buffer)
{

	if(IsPortAvailable())
	{
		return m_pPort->DirectWrite(length,buffer);
	}
	else
		return -1;
}

///////////////////////////////////////////////////////////////////////////////
//! Flush data in the txBuffer.
/*! if the interface buffers fata waiting to send this routine flushes that buffer
*/

void CIoMan::TxFlush(void)
{
	m_pPort->FlushTxFifo();
}

///////////////////////////////////////////////////////////////////////////////
//! GetTxFifoFillLevel .
/*! if the interface buffers fata waiting to send this routine returns number of msgs inthat buffer
\return this routine returns the number of messages in the tx buffer returns -1 if IFace not open or no buffer used
*/

int CIoMan::GetTxFifoFillLevel(void)
{
	if (m_pPort)
		return m_pPort->GetTxFifoFillLevel();
	else
		return -1;
}

//check for debug interface

bool IsInterfaceRunning_2(void)                          //lmc-debug
{
	return ( (g_pDebugInterface!=NULL) ? (g_pDebugInterface->IsPortAvailable()) : false);
}






