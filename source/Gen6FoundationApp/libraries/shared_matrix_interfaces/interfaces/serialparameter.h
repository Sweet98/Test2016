/*=================================================================================
  class:

//=================================================================================
   $Source: interfaces/serialparameter.h $
   $Date: 2009/12/17 02:41:12EST $
   $Revision:


//=================================================================================*/

#ifndef SERIAL_PARAMETER_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define SERIAL_PARAMETER_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#define NOPARITY            0
#define ODDPARITY           1
#define EVENPARITY          2
#define MARKPARITY          3
#define SPACEPARITY         4

#define ONESTOPBIT          0
#define ONE5STOPBITS        1
#define TWOSTOPBITS         2

typedef enum
	{
		CTSOFF,
		HNDSHK,
		FLOW,
//ewr-02787		
		CTSMAN,
		CTSFST,
//		
		SCANCTRL,
		CTSPLS
	}RtsTypes;

//ewr-02787
typedef enum
	{
		PKTOFF,			//Turns Mmmmmlllll serial packet mode off
		PKTONE,        //Mmmmmlllll serial packet mode one. not currently supported
		PKTTWO         //Mmmmmlllll serial packet mode two, the only one we currently support
	}PktModeTypes;
//

class CSerParm
{
public:
	CSerParm(void);
	MatrixError_t ExtractParameters(const tichar_t *pParameters );

	ULONG GetBaudRate(void)	{ return m_BaudRate; }
	void SetBaudRate(ULONG n)	{ m_BaudRate = n; }
	RtsTypes IsRtsCts(void)		{ return m_CtsRtsEnabled; }
	BOOL IsXonXoff(void)		{ return m_XonEnabled; }
	BOOL IsAcknak(void)		{ return m_AckNakEnabled; }
	int GetParity(void)		{ return m_Parity; }
	int GetStopBits(void)	{ return m_StopBits; }
	int GetDataBits(void)	{ return m_DataBits; }
	timer_uSec_t GetCharacterTime(void);
	
//ewr-02787	
	ULONG GetCtsDelay(void)					{ return m_CtsDelay; }
	PktModeTypes GetPacketMode(void)		{ return m_PacketModeType; }
	BOOL IsPktNakEnabled(void)				{ return m_PacketNakEnable; }
   ULONG GetPacketAckNakDelay(void) { return m_PacketAckNakDelay*1000; }
//

protected:
	unsigned long m_BaudRate;
	BOOL m_AckNakEnabled;
	RtsTypes m_CtsRtsEnabled;
	BOOL m_XonEnabled;
	int m_Parity;
	int m_StopBits;
	int m_DataBits;
	ULONG m_CtsDelay;

//ewr-02787
	PktModeTypes m_PacketModeType;
	BOOL m_PacketNakEnable;
   ULONG m_PacketAckNakDelay;
//
};

#endif /* SERIAL_PARAMETER_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */



