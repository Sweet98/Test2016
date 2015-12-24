/*=================================================================================
  class: CSerialBase
 
//=================================================================================
   $Source: interfaces/serialbase.cpp $
   $Date: 2011/01/21 16:35:50EST $
   $Revision: 1.34.1.16 $
 

//=================================================================================*/
#include "stdInclude.h"
#include "CommonDriver.h"
#include "SerialBase.h"
#include "BarcodeData.h"
#include "rxfifo.h"
#include "ParseHelper.h"
#include "timeout.h"



#ifdef ADDTRACE
extern int ReturnAddr;
#include <stdio.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#if PLATFORM_CURRENT == imx25

#include "HardwareMacros.h"	// see  gpio configure below should not be here permanently
#include "iMX25_GPIO.h"
#include "iMX25_IOMUX.h"
struct UartDetails
{
	Uart_T *pBaseAddr;
	vector_number_t VectorNumber;

};

static const UartDetails Uarts[2] =
    {
        {
            pUart1,
            VEC_UART1_INT,
        },
        {
            pUart2,
            VEC_UART2_INT,
        }
    };
// Constructor overload 1
CSerialBase::CSerialBase(UartNumber_t UartNumber)
		: m_Port(Uarts[UartNumber].pBaseAddr)
{
	m_VectorNumber = Uarts[UartNumber].VectorNumber;
	// precaution
	DisableInterruptNumber(m_VectorNumber);

	m_TxThreshold = 2;
	m_RestSize = 0;
	m_OutputStopped = false;
	m_DriverSupportsProgrammableOutputFormats = true;
	sendABResponse=false;
	m_MessageThrownOutNotSent = false;

}


MatrixError_t CSerialBase::Open(interface_id_t nId, const tichar_t *pParameter)
{
	DisableInterruptNumber(m_VectorNumber);		// safety
	MatrixError_t ret = CThreadedDriver::Open(nId, pParameter);
	if(IsOK(ret))
	{
		ret = m_Port.ExtractParameters(pParameter);
		m_Port.SetIoPins();
		if (nId != 905)	// do not set the GPIOs if the interface is OneWire
			Set_OE_ForRxTxRtsCts(0);
		
		if(IsOK(ret))
			SetIrqVectors();

	}
	return ret;
}

MatrixError_t CSerialBase::Close()
{
	// precaution
	DisableInterruptNumber(m_VectorNumber);

	m_Port.Close();
	return CThreadedDriver::Close();
}

void CSerialBase::SetIrqVectors(void)
{
	SetVector(&CSerialBase::SimpleRxTxISR, m_VectorNumber);
}

#else
#error VALID PLATFORM NOT DEFINED
#endif
// Destructor
CSerialBase::~CSerialBase()
{}


threadhandle_t CSerialBase::SimpleRxTxISR(void)
{
	handleISRs();
	return false;		// no thread switch
}

/// Use for non standard communication from you application to the driver
IoControlRet_t CSerialBase::DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam)
{
	// first call base class
	IoControlRet_t RetVal=CThreadedDriver::DeviceIoControl(cmd, lparam, wparam);
	switch(cmd)
	{
	case IOCTRL_IS_DONE:
		if(RetVal == IOCRET_YES)			// if base class said yes, then
		{
			if (!m_Port.IsTxComplete())	      // if uart fifo is not empty, ...
				RetVal = IOCRET_NO;
		}
		//Add for delay issue
		else if(!m_Port.IsCtsInActive() && ((IsRtsCts()==HNDSHK) || (IsRtsCts()==CTSMAN)))  
		{
                	RetVal = IOCRET_YES;                         
		}                      
		
		break;
	case IOCTRL_IS_LOW_POWER_ALLOWED:
		RetVal = IOCRET_YES;
		break;
	case IOCTRL_PREPARE_FOR_LOW_POWER_STATE:
		RetVal = PrepareForLowPowerSleep();
		break;
	case IOCTRL_RETURN_TO_NORMAL_POWER_STATE:
		RetVal = ReturnFromLowPowerSleep();
		break;
	case IOCTRL_IS_BAUDRATE_AVAILABLE:
		{
			if (lparam != 0)
			{
				ULONG BaudRate=0;
				UCHAR *pParameters = (UCHAR*) lparam;
				if (CParseHelper::Extract(pParameters, 0, BaudRate))
				{
					RetVal = (BaudRate < 1000000) ?  IOCRET_YES : IOCRET_NO;
				}
			}
		}
		break;

//ewr-02787		
		case IOCTRL_CHECK_IF_MESSAGE_NOT_SENT_IN_232_CTS_MAN:
//		
			RetVal = ERR_FAIL;

		if(m_MessageThrownOutNotSent)
		{
			RetVal = ERR_PASS;
		}
		break;

//ewr-02787		
		case IOCTRL_ACK_BACK_TO_IO_THAT_MESSAGE_NOT_SENT_IN_232_CTS_MAN:
//
		
			RetVal = ERR_PASS;
			m_MessageThrownOutNotSent = false;
		break;
		
//ewr-02787
		case IOCTRL_CHECK_IF_WE_ARE_IN_232_CTS_MAN:
			RetVal = IOCRET_NO;
			//If we are using one of the manusl control CTS modes, indicate it
			if( (IsRtsCts() == CTSMAN) || (IsRtsCts() == CTSFST) )
			{
				RetVal = IOCRET_YES;
			}
		break;
//
		
		case IOCTRL_GOT_AUTOBAUD:
			RetVal=IOCRET_NO;
			if (sendABResponse)
			{
				RetVal=IOCRET_YES;
				DISABLE_INTERRUPTS();
				sendABResponse=false;
				RESTORE_INTERRUPTS();
			}
		default:
		break;
	}
	return RetVal;
}

// This function gets called from the driver ThreadLoop()
int CSerialBase::StartSending(void)
{
	ASSERT(AnyDataToSend());
	int signal = Sig_Resend;

	//Add for ¡°Partial Data Send¡± issue
	if(HardwareToken.IAmOEM())
	{
		while(!m_Port.IsCtsInActive() && ((IsRtsCts()==HNDSHK) || (IsRtsCts()==FLOW) || (IsRtsCts()==CTSMAN)))  
		{
			MT_Delay(2);			
		}
	}
	else
	{
		while(!m_Port.IsCtsInActive() && ((IsRtsCts()==HNDSHK) || (IsRtsCts()==CTSMAN)))  
		{
			MT_Delay(2); 			
		}
	}
	while(signal & Sig_Resend)
	{
		GetBarcodeToSend()->GetTotalData(m_pCurrentText, m_RestSize);
		PrepareWaitForSignal(Sig_BarcodeSent|Sig_Resend);			// prepare the wait for done

		while(true)								// is output on hold by the other side?
		{
			DISABLE_INTERRUPTS();
			if (!m_OutputStopped)
			{
				m_Port.TxReadyIrqEnable();
				RESTORE_INTERRUPTS();
				break;
			}
			RESTORE_INTERRUPTS();
			MT_Delay(2);			         			// check every 2000 uSec
		}
		signal = WaitForAnySignal(Sig_BarcodeSent|Sig_Resend);	// wait until the ISR sent them
	}
	// if we are here, we know that all bytes have been sent
	return 0;
}

// This function gets called from the driver ThreadLoop() before the StartSending.
void CSerialBase::PreSendAction(void)
{
	m_Port.SetRtsOutActive();
}

// This function gets called from the driver ThreadLoop() after the StartSending.
void CSerialBase::PostSendAction(void)
{
	m_Port.SetRtsOutPassive();
	SentBarcode();
}

void CSerialBase::SimpleSendISR(void)
{
	size_t block = CUart::TX_FIFO_SIZE - m_TxThreshold;
	while(block-- > 0)
	{
		if(m_RestSize-- > 0)
		{
			m_Port.WriteFifo(*m_pCurrentText++);
		}
		else
		{
			m_RestSize++;							// keep it at 0
			m_Port.TxReadyIrqDisable();      // done with filling the FIFO
			m_Port.TxCompleteIrqEnable();		// now wait until all bytes have been sent
			if (m_Port.IsTxComplete())
			{
				m_Port.TxCompleteIrqDisable();// done with waiting for the last byte sent
				Signal(Sig_BarcodeSent);		// wake the driver thread
			}
			break;									// done
		}
	}
}
void DEBUG_puts(const char *c);
#ifdef ADDTRACE2
void PrintAbortMessageandReset(unsigned int InstructionAddress,const char * str )
#endif

void CSerialBase::SimpleReceiveISR(void)
{
	m_Port.ClearORunErr();
	m_Port.ClearRxIdleIrq();
	m_Port.ClearRxTimeout();
	unsigned long received = m_Port.ReadFifo();

	while (received & BIT(UART_URXD_CHARRDY_LSH))
	{
		if(!(received & BIT(UART_URXD_ERR_LSH)))
		{
			char ch=received & 0xff;
			m_pRxFifo->Write(ch);

			switch(ch &0x7f)
			{
			case 0x02:
				m_autobaudstate=1;
				break;

			case 'C':
			case 'c':
				if (m_autobaudstate==1)
					m_autobaudstate=2;
				else
					m_autobaudstate=0;
			case 0x0d:
				if (m_autobaudstate==2)
					sendABResponse=true;
			default:
				m_autobaudstate=0;

			}

#ifdef ADDTRACE
			static int state=0;
			switch(state)
			{
			case 0:
				if(ch == 'L')
					state++;
				break;
			case 1:
				if(ch == 'o')
					state++;
				else
					state = 0;
				break;
			case 2:
				if(ch == 'c')
					state++;
				else
					state = 0;
				break;
			case 3:
				if(ch == 'k')
					state++;
				else
					state = 0;
				break;
			case 4:
				if(ch == 'U')
					state++;
				else
					state = 0;
				break;
			case 5:
				if(ch == 'p')
				{
					m_Port.TxReadyIrqDisable();
					char s[50];
					sprintf(s,"Address = %08X \r\n",ReturnAddr);
					DEBUG_puts(s);
#ifdef ADDTRACE2

					PrintAbortMessageandReset(ReturnAddr,"LockUp Generated Debugging: " )
#endif
					m_Port.TxReadyIrqEnable();
				}
				state = 0;
				break;
			}
#endif

		}
		else
		{
			// what to do??
		}
		received = m_Port.ReadFifo();	// read next fifo entry
	}
}

// below is the power managment stuff

void CSerialBase::CtsISR(void)    	      // Mx1 calls this RTS!
{
	m_Port.AckCtsChangeIrq();
	// do not activate the drivers thread
}

void CSerialBase::PfISR(void)    	      // Mx1 calls this RTS!
{
	do
	{
		m_Port.ClearFrameErr();
	}
	while (m_Port.IsFrameErrIrq());
	// do not activate the drivers thread
}


MatrixError_t CSerialBase::PrepareForLowPowerSleep(void)
{
	// wake on RXD falling edge
	m_Port.ClearAwake();		// clear status
	m_Port.SetAwakeOn();
	// wakes on rising or falling edge of RTS
	m_Port.ClearRtsDelta();
	m_Port.EnableRtsDelta();
	return ERR_PASS;
}

MatrixError_t CSerialBase::ReturnFromLowPowerSleep(void)
{
	m_Port.SetAwakeOff();
	m_Port.ClearAwake();		// clear status

	m_Port.ClearRtsDelta();
	m_Port.DisableRtsDelta();
	return ERR_PASS;
}

int CSerialBase::Set_OE_ForRxTxRtsCts(int portnum)
{
	DISABLE_INTERRUPTS();
	
	
	
#ifndef PROTO_HW
	//Function: n232/KBD_OE1   Pin: D0   GPIO4_20 
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT20_LSH,GPIO_BIT20_WID,GPIO_LOW);
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_D0,MUX_MODE_5);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_D0,0x00000047);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT20_LSH,GPIO_BIT20_WID,GPIO_GDIR_OUTPUT);
	
	//Function: n232/KBD_OE2   Pin: D1   GPIO4_19 
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT19_LSH,GPIO_BIT19_WID,GPIO_LOW);
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_D1,MUX_MODE_5);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_D1,0x00000047);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT19_LSH,GPIO_BIT19_WID,GPIO_GDIR_OUTPUT);
	
	//Function: n232/KBD_OE3   Pin: D2    GPIO4_18
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT18_LSH,GPIO_BIT18_WID,GPIO_HIGH);
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_D2,MUX_MODE_5);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_D2,0x00000047);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT18_LSH,GPIO_BIT18_WID,GPIO_GDIR_OUTPUT);
	
	//Function: n232/KBD_OE4   Pin: D3    GPIO4_17
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,GPIO_HIGH);	
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_D3,MUX_MODE_5);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_D3,0x00000047);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,GPIO_GDIR_OUTPUT);
		
	//KBD_SW_CNTRL
	HAZARD_WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT12_LSH,GPIO_BIT12_WID,GPIO_LOW);
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_EB0,MUX_MODE_5);
	HAZARD_WRITE_FIELD_TO_REG(GPIO2_GDIR0,GPIO_BIT12_LSH,GPIO_BIT12_WID,GPIO_GDIR_OUTPUT);

#else	
	//n232/KBD_OE1
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT16_LSH,GPIO_BIT16_WID,GPIO_LOW);
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UPLL_BYPCLK,MUX_MODE_5);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT16_LSH,GPIO_BIT16_WID,GPIO_GDIR_OUTPUT);
	
	//n232/KBD_OE2
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,GPIO_LOW);
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_VSTBY_REQ,MUX_MODE_5);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,GPIO_GDIR_OUTPUT);
	
	//n232/KBD_OE3
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT18_LSH,GPIO_BIT18_WID,GPIO_HIGH);	
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_VSTBY_ACK,MUX_MODE_5);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT18_LSH,GPIO_BIT18_WID,GPIO_GDIR_OUTPUT);
	
	//n232/KBD_OE4
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT19_LSH,GPIO_BIT19_WID,GPIO_HIGH);
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_POWER_FAIL,MUX_MODE_5);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT19_LSH,GPIO_BIT19_WID,GPIO_GDIR_OUTPUT);
	
	//KBD_SW_CNTRL
	HAZARD_WRITE_FIELD_TO_REG(GPIO1_DR0,GPIO_BIT24_LSH,GPIO_BIT24_WID,GPIO_LOW);
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_LSCLK,MUX_MODE_5);
	HAZARD_WRITE_FIELD_TO_REG(GPIO1_GDIR0,GPIO_BIT24_LSH,GPIO_BIT24_WID,GPIO_GDIR_OUTPUT);


#endif //endof PROTO_HW
	RESTORE_INTERRUPTS();
	return 0;
}




