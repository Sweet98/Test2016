/*=================================================================================
  class: CWedgeEngineIBM

//=================================================================================
   $Source: interfaces_wedge/wedgeengineibm.cpp $
   $Date: 2010/04/13 17:24:46EDT $
   $Revision: 1.11 $



//=================================================================================*/

#include "stdInclude.h"
#include "CommonDriver.h"
#include "WedgeEngineIBM.h"
#include "Bitbanger.h"
#include "ThreadRealtime.h"
#include "Thread.h"
#include "db_chrio.h"
#include "WedgeHal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Constructor
CWedgeEngineIBM::CWedgeEngineIBM(CThreadedDriver *pDriver, const CWedgeTerminal *pTerminal, UINT ScancodeSet, CBitBanger *pBitBanger)
	:baseCWedgeEngineIBM(pDriver, pTerminal, pBitBanger)
{
	m_ScancodeSet = ScancodeSet;
}

// Destructor
CWedgeEngineIBM::~CWedgeEngineIBM()
{
}

//MatrixError_t CWedgeEngineIBM::BitBangOutKbd(unsigned int data, unsigned int extrainfo)
MatrixError_t CWedgeEngineIBM::BitBangOutKbd(unsigned int , unsigned int )
{
	ASSERT(m_pBitBang!=NULL);
//	return m_pBitBang->BitBangOutKbd(data, extrainfo); // for later when we implement full automatic
	return ERR_PASS;
}

//MatrixError_t CWedgeEngineIBM::BitBangInKbd(unsigned int &uiReturnData)
MatrixError_t CWedgeEngineIBM::BitBangInKbd(unsigned int &)
{
	ASSERT(m_pBitBang!=NULL);

//	return m_pBitBang->BitBangInKbd(uiReturnData);  // for later when we implement full automatic
	return ERR_BB_READ_NoData;
}


MatrixError_t CWedgeEngineIBM::OpenKeyboardEmulation(void)
{
	if (WdgIsKeyEmu())
	{
//		MT_Sleep(475*1000);
		if (WaitForClkDataHigh(1400))		/* try for 1400 mS */
			Write(0xAA);
	}
	return ERR_PASS;
}


MatrixError_t CWedgeEngineIBM::CheckKeyboardEmulation(void)
{
	unsigned int data;
	MatrixError_t ret;

	//XWTRACE2_H();
	if( 0== WedgeHal.ReadBit(CWedgeHal::TERM_CLK_IN))
	{
		ret =ERR_BB_READ_NoData;
		return ret;
	}

		if( 1== WedgeHal.ReadBit(CWedgeHal::TERM_DATA_IN))
	{
		ret =ERR_BB_READ_NoData;
		return ret;
	}

	ret = BitBangIn(data);
	//XWTRACE2_L();

	if (ERR_BB_READ_NoData == ret)
		{
			return ret;
		}

	if (ERR_BB_READ_ERROR == ret)
	{
		DoAskForResend();
		return ERR_BB_READ_ERROR;
	}
	else if(IsOK(ret))
	{
		if (data < 0xED)
		{
			DoAskForResend();
			ret = ERR_BB_READ_ERROR;
		}
		else
		{
			BitBangOutKbd(data, 0);
			switch(data)
			{
			case 0xFF:
				DoReset();
				break;
			case 0xFE:
				DoResend();
				break;

			case 0xF4:
				DoEnable();
				break;
			case 0xF3:
				DoTypematic();
				break;
			case 0xF2:
				DoId();
				break;
			case 0xF1:
			case 0xEF:
					DoAskForResend();
					ret = ERR_BB_READ_ERROR;
					break;
			case 0xF0:
				DoScancodeSet();
				break;
			case 0xEE:
				DoEcho();
				break;
			case 0xED:
				DoStatusIndicators();
				break;


			/* ^^^ add new commands here ^^^ */
			default:
				DoSendAck();
				break;
			}
		}
	}
	return ret;
}

bool CWedgeEngineIBM::WaitForClkDataHigh(UINT mSec)
{
	int count = mSec*10; //jw
	int bounce=0;
	do
	{
		if( 1== WedgeHal.ReadBit(CWedgeHal::TERM_CLK_IN))
		{
			if ( 1== WedgeHal.ReadBit(CWedgeHal::TERM_DATA_IN))
				bounce++;
			else return false;
		}
		else bounce=0;
		MT_Sleep(100);                  //dangerous  Jess
		//MT_Delay(100/1000);
	}while ( bounce<5 && (count-- != 0) );

	return bounce>4;
}

/*****************************************************************************/

MatrixError_t CWedgeEngineIBM::Write(unsigned int scancode)
{
	if (!WaitForClkDataHigh(1500))		/* try for 1400 mS */
			return ERR_FAIL;
	return BitBangOut(scancode, EX_KEYEMU);
//	MT_Sleep(75);
}

void CWedgeEngineIBM::DoAskForResend(void)
{
	Write(0xFE);
}

MatrixError_t CWedgeEngineIBM::GetNextByte(unsigned char *Data )

{
	unsigned int data=0;
	MatrixError_t ret=ERR_FAIL;

	while (true)
	{
		if( 1== WedgeHal.ReadBit(CWedgeHal::TERM_CLK_IN) &&WedgeHal.ReadBit(CWedgeHal::TERM_DATA_IN)==0 )
		{
			ret = BitBangIn(data);
			if(IsOK(ret))
			{
				break;
			}

			if (ERR_BB_READ_ERROR == ret)
				DoAskForResend();
		}
		MT_Sleep(100);
	}
	if(IsOK(ret))
	{
		*Data = data;
		BitBangOutKbd(data, 0);
	}
	return ret;
}


inline void CWedgeEngineIBM::DoSendAck(void)
{
	Write(0xFA);
}
inline void CWedgeEngineIBM::DoEcho(void)
{
	Write(0xEE);
}
inline void CWedgeEngineIBM::DoResend(void)
{
	Write(m_LastSend);
}

void CWedgeEngineIBM::DoEnable(void)
{
	WdgStoreEnabled(1);
	DoSendAck();
}

void CWedgeEngineIBM::DoReset(void)
{
	WdgStoreEnabled(0);
	m_LastSend = 0;
	DoSendAck();
	CWedgeEngineIBM::OpenKeyboardEmulation();
}

void CWedgeEngineIBM::DoTypematic(void)
{
	unsigned char data;
	DoSendAck();
	GetNextByte(&data);
	DoSendAck();
}

extern const CScancodes IBM_SET1_SCAN;
extern const CScancodes IBM_SET2_SCAN;
extern const CScancodes IBM_SET3_SCAN;

void CWedgeEngineIBM::DoScancodeSet(void)
{
	unsigned char data;
	DoSendAck();
	GetNextByte(&data);
	if (data == 0)			/* Is it a query scancode set command ? */
	{
		DoSendAck();
		Write(GetScancodeSet());	/* Then send it */
	}
	else
	{
		SetScancodeSet(data);			/* Else store it */
		switch(data)		/* Auto switch between the 3 different IBM scancode sets (only works in KeyEmu) */
		{
		case 1:
			SetScancodeTable(&IBM_SET1_SCAN);
			break;
		case 2:
			SetScancodeTable(&IBM_SET2_SCAN);
			break;
		case 3:
			SetScancodeTable(&IBM_SET3_SCAN);
			break;
		default:
			/* unknow requested scancode set, so keep the old one */
			break;
		}
		DoSendAck();
	}
}

void CWedgeEngineIBM::DoId(void)
{
	// This function has some timing requirements. This implies that we can not use our nice functions.
	// We can not use the Write function bacuse we must meet a 500uS max. time
	// We must call the low level function directly
	WdgStoreEnabled(0);
	do {
		if (!IsOK(Write(0xFA)))		/* try for 1400 mS */
			break;
		if (!IsOK(Write(0xAB)))		/* try for 1400 mS */
			break;
		Write(0x83);// Send the ACK byte. DoSendAck() can not be used to meet the timing.
		}while (0);
	WdgStoreEnabled(1);
}

void CWedgeEngineIBM::DoStatusIndicators(void)
{
	UCHAR data;
	DoSendAck();
	GetNextByte(&data);
	if (data < 0x07)
	{
		WdgStoreCapsLock(0);
		WdgStoreNumLock(0);
		WdgStoreScrollLock(0);
		if (data & 0x04)
			WdgStoreCapsLock(1);
		if (data & 0x02)
			WdgStoreNumLock(1);
		if (data & 0x01)
			WdgStoreScrollLock(1);
		WdgStoreLED_Valid(true);
	}
	DoSendAck();
}


void CWedgeEngineIBM::PrepareIoForWedgeHardware(void)
{
	WedgeHal.PrepareIoForWedge();
}

void CWedgeEngineIBM::EnableKeyboard (void)
{
	if (!WdgIsKeyEmu())
		WedgeHal.KeyboardEnable();
}

void CWedgeEngineIBM::DisableKeyboard (void)
{
	WedgeHal.KeyboardDisable();
}




