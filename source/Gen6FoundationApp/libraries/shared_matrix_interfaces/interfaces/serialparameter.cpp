/*=================================================================================
  class: CSerParm

//=================================================================================
   $Source: interfaces/serialparameter.cpp $
   $Date: 2010/04/28 15:29:58EDT $
   $Revision:


//=================================================================================*/

#include "stdInclude.h"
#include "CommonDriver.h"
#include "SerialParameter.h"
#include "ParseHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
	CSerParm is a helper to parse the ascii parameter strings for the serial interfaces.

*/
CSerParm::CSerParm(void)
{
	m_BaudRate = 38400;
	m_Parity = NOPARITY;
	m_StopBits = ONESTOPBIT;
	m_DataBits = 8;
	m_AckNakEnabled = FALSE;
	m_CtsRtsEnabled = CTSOFF;
	m_XonEnabled = FALSE;
	m_CtsDelay = 0;

//ewr-02787	
	m_PacketModeType = PKTOFF;
	m_PacketNakEnable = FALSE;
//		
}

MatrixError_t CSerParm::ExtractParameters(const tichar_t *pParameters )
{
	bool ret=PASS;
	bool bStat = PASS;
	const tichar_t *pBits=NULL;
	int temp;
	ret &= CParseHelper::Extract(pParameters, 0, m_BaudRate);
	//ret &= CParseHelper::Extract(pParameters, _TI("ACK"), m_AckNakEnabled);
	ret &= CParseHelper::Extract(pParameters, _TI("CTS"), temp);
	m_CtsRtsEnabled=(RtsTypes)temp;
	//ret &= CParseHelper::Extract(pParameters, _TI("XON"), m_XonEnabled);
	ret &= CParseHelper::Extract(pParameters, 1, pBits);

	//These following parameters are not required for all instances created of the serial interface (ex: bluetooth serial port instances),
	//so they are not allowed to fail the creation of an instance of the serial interface

	bStat = CParseHelper::Extract(pParameters, _TI("ACK"), m_AckNakEnabled);
	if(bStat == FALSE)
	{
		m_AckNakEnabled = FALSE;                  //if parameter not there, lets default it off
	}

	bStat = TRUE;
	bStat = CParseHelper::Extract(pParameters, _TI("XON"), m_XonEnabled);
	if(bStat == FALSE)
	{
		m_XonEnabled = FALSE;                  //if parameter not there, lets default it off
	}

	bStat = TRUE;
	bStat = CParseHelper::Extract(pParameters, _TI("DLYCTS"), m_CtsDelay);
	if(bStat == FALSE)
	{
		m_CtsDelay = 0;                        //if parameter not there, lets default it off
	}



//ewr-02787
	bStat = TRUE;
	bStat = CParseHelper::Extract(pParameters, _TI("PKT"), temp);
	if( bStat )
	{
		m_PacketModeType = (PktModeTypes)temp;
	}
	else
	{
		m_PacketModeType = PKTOFF;                        //if parameter not there, lets default it off
	}

	bStat = TRUE;
	bStat = CParseHelper::Extract(pParameters, _TI("NAK"), m_PacketNakEnable);
	if(bStat == FALSE)
	{
		m_PacketNakEnable = FALSE;                        //if parameter not there, lets default it off
	}

   bStat = TRUE;
   bStat = CParseHelper::Extract(pParameters, _TI("DLK"), m_PacketAckNakDelay);
   if(bStat == FALSE)
   {
      m_PacketAckNakDelay = 0;                        //if parameter not there, lets default it off
   }
//


	if(pBits!=NULL)
	{
		switch (pBits[1])
		{
		case '8':
			m_DataBits = 8;
			break;
		case '7':
			m_DataBits = 7;
			break;
		default:
			ret = FAIL;
			break;
		}
		
		switch (pBits[0])
		{
		case 'N':
			m_Parity = NOPARITY;
			break;
		case 'O':
			m_Parity = ODDPARITY;
			break;
		case 'S':
			m_Parity = SPACEPARITY;
			break;
		case 'M':
			m_Parity = NOPARITY;
			m_StopBits = TWOSTOPBITS;
			return ret ? ERR_PASS : ERR_BADPARAM;
		case 'E':
			m_Parity = EVENPARITY;
			break;

		default:
			ret = FAIL;
			break;
		}

		switch (pBits[2])
		{
		case '1':
			m_StopBits = ONESTOPBIT;
			break;
		case '2':
			m_StopBits = TWOSTOPBITS;
			break;
		default:
			ret = FAIL;
			break;
		}
	}
	return ret ? ERR_PASS : ERR_BADPARAM;
}

timer_uSec_t CSerParm::GetCharacterTime(void)
{
	int NumberOfBits = 1+GetDataBits()+1;
	if(m_StopBits != ONESTOPBIT)
		NumberOfBits++;
	if(m_Parity != NOPARITY)
		NumberOfBits++;

	return (NumberOfBits*Second)/GetBaudRate();  // for better delays
}



