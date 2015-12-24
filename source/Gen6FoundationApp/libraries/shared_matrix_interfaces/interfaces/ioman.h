/*=================================================================================
  class: CIoMan
 
//=================================================================================
   $Source: interfaces/ioman.h $
   $Date: 2011/03/31 14:27:03EDT $
   $Revision: 1.37.1.3.1.2.1.5 $
 

//=================================================================================*/
//! \file


#ifndef IO_MAN_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define IO_MAN_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

#include <stddef.h>
#include "ErrorDefs.h"
#include "IoControl.h"
#include "InterfaceTypedefs.h"
class HSTRING;

// for a transition periode I need a separate character type for the ioman.
// The interfaces can be compiled as a component in UNICODE, while the rest is still byte characters.
//#ifdef UNICODE_COMPONENT
//typedef wchar_t ichar_t;
//#else
//#ifdef UNICODE
//typedef wchar_t ichar_t;
//#else
//typedef unsigned char ichar_t;
//#endif
//#endif
class CIoMan
{
public:
	CIoMan();
	~CIoMan();
	static void CreateStdInterfaces(void);  // helper to instantiate
	static void DeleteStdInterfaces(void);  // helper to delete

	// interface
	// various overloads for the Write function
	size_t Write (const bchar_t *pText);
	size_t Write (const wchar_t *pText);
	size_t Write (const char *pText)
	{
		return Write((const bchar_t *)pText);
	}

	size_t Write (const wchar_t *pBin, size_t nSize);
	size_t Write (const bchar_t *pBin, size_t nSize);
	size_t Write (const char *pBin, size_t nSize)
	{
		return Write((const bchar_t *)pBin, nSize);
	}

	size_t Write (bchar_t nChar)
	{
		return Write(&nChar, 1);
	}
	size_t Write (wchar_t nChar)
	{
		return Write(&nChar, 1);
	}

	size_t Write (CBarcodeData *pLabel);
	size_t Write (const HSTRING *pString);
	size_t Write (const HSTRING &String)
	{
		return Write (&String);
	}

	// and even a printf lookalike
	size_t printf(const bchar_t *psz, ... );
	size_t printf(const wchar_t *psz, ... );
	size_t printf(const char *psz, ... );

	size_t Collect (const bchar_t *pText);
	size_t Collect (const bchar_t *pText, size_t nSize);
	size_t Collect (const wchar_t *pText);
	size_t Collect (const wchar_t *pText, size_t nSize);
	size_t SendCollected (void);
	int DirectWrite(int length,unsigned char * buffer);

	int GetTxFifoFillLevel(void);

	size_t Read (bchar_t *pBin, size_t nSize);
	size_t IsAnyRxData (void);	            // returns the amount of RX data
	size_t Peek( bchar_t *pBin, size_t offset = 0 );
	void TxFlush(void);
	MatrixError_t DeviceIoControl  (IoControl_t cmd, LPARAM lparam=0, WPARAM wparam=0);
	MatrixError_t WaitUntilSent(timer_uSec_t timeout = 4*1000*1000, bool bFlushData = true);  // 4 Second default timeout
	MatrixError_t IsAllSent(void)
	{
		return (DeviceIoControl(IOCTRL_IS_DONE) == IOCRET_YES);
	}

	MatrixError_t Open(interface_id_t nId);
	MatrixError_t Open(interface_id_t nId, const bchar_t *pParam);
	MatrixError_t Open(interface_id_t nId, const wchar_t *pParam);
	MatrixError_t Open(interface_id_t nId, const tichar_t *pParam, CDriverBase *(*pClassFactory)(void)); // only for testing

	MatrixError_t Close(void);

	static bool	QueryInterface(interface_id_t termid, model_t model);

	//	static int GetSystemCodepage(void)					{ return ms_SystemCodePage;	}

	bool	IsPortAvailable(void);

	interface_id_t GetTerminalID(void)
	{
		return m_TermId;
	}
	bool IsLowPowerModeAllowed(void)
	{
		return m_LowPowerModeAllowed;
	}

	size_t StuffRxBuff(const bchar_t *pBin, size_t nSize);

	int		GetLastSequenceNumber(void);
	void	SetLastSequenceNumber(int newSequenceNumber);

protected:
	// for transition to UNICODE
	MatrixError_t InternalOpen(interface_id_t nId, const tichar_t *pParam);
	MatrixError_t OpenOnlyIfChanged(interface_id_t nId, const tichar_t *pParam);

	enum
	{
	    ParameterSize = 300
	};
	tichar_t m_LastUsedParam[ParameterSize];

	void SetTerminalID(interface_id_t id)
	{
		m_TermId = id;
	}

	enum
	{
	    EMPTY_TERM_ID = -1
	};
	CDriverBase	*m_pPort;
	interface_id_t m_TermId;
	bool m_LowPowerModeAllowed;

	CBarcodeData *m_pCollector;
	int	m_LastSequenceNumber;

public:
	enum
	{
	    DEFAULT_INTERCHARACTER_RECEIVE_TIMEOUT = 10		// 10 ms is the default
	};
};


extern	CIoMan *g_pStdInterface;
extern  CIoMan *g_pDebugInterface;
extern	CIoMan *g_pRfInterface;
extern	CIoMan *g_pCurrentInterface;
extern	CIoMan *g_pBToothSPP_Interfaces[];  // This is a bad name.  It actually holds theStdinterface as well.
extern 	CIoMan *g_pCurrentClientSPP_Interface;
extern	CIoMan *g_pOWInterface;
extern	CIoMan *g_pRemInterface;

#define theStdInterface (*g_pStdInterface)
#define theDebugInterface (*g_pDebugInterface)
#define theRfInterface (*g_pRfInterface)
#define theRemInterface (*g_pRemInterface)
#define theCurrentInterface (*g_pCurrentInterface)
#if defined(RFBASE) || defined(RFSCAN)
#define theOWInterface (*g_pOWInterface)
#endif
#if defined(RFBASE)
#define MAX_SPP_PORTS 7
#elif defined(RFSCAN)
#define MAX_SPP_PORTS 7
#else
#define MAX_SPP_PORTS 0
#endif

/*
 TODO:  When I was asked to add ReM, a code inspection revealed we maintain an array of theStdInterface PLUS all of the SPP interfaces 
(g_pBToothSPP_Interfaces or theSPP_Ports).  Oddly, there is only one place where we make use of this array and index 0 (theStdInterface).
Unfortunately, there were no macros indicating the start and end of this array--it's ALL hard coded.  Thus, adding ReM to this array would
necessitate hundreds of changes.  If we are going to touch all those places, we might as well extract theStdInterface from this array and
just keep the RF interfaces in it--it's a lot cleaner.

This will be done in two phases.  The first phase will remove the one and only place where we make use of index 0.  The Rem interface will
NOT be put in the array.  The second phase will be the tedious one--adjusting all the loops' starting and ending points.
*/
// These macros were created before I realized there were so many hard coded indeces.  I left them in for documentation purposes.
#define IOMAN_ARRAY_STD_INDX	0  // theStdInterface
#define IOMAN_ARRAY_BT_INDX		1  // This is the starting index for the bluetooth interfaces

#define IOMAN_ARRAY_LEN	(1 + MAX_SPP_PORTS)


#define theSPP_Ports(n) (*(g_pBToothSPP_Interfaces[n]))
#define theCurrentClientSPP_Interface (*g_pCurrentClientSPP_Interface)

bool IsInterfaceRunning(void);
bool DoesInterfaceDoSimpleIO(void);
unsigned int IntercharacterReceiveTimeoutForThisInterface( void );
bool IsInterfaceRunning_2(void);                //lmc-debug


#endif // IO_MAN_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_



