/*=================================================================================
  class: CUart

//=================================================================================
   $Source: interfaces_hw/uart.cpp $
   $Date: 2010/11/09 21:13:23EST $
   $Revision:


//=================================================================================*/
#include <math.h>
#include "stdInclude.h"
#include "CommonDriver.h"
#include "interrupts.h"
#include "Uart.h"

static const vector_number_t UartVector[] =
    {
        (vector_number_t)0,VEC_UART1_INT,VEC_UART2_INT,VEC_UART3_INT,VEC_UART4_INT,VEC_UART5_INT
    };
// Constructor
CUart::CUart(Uart_T *pUart)
{
	m_pUart = pUart;
#if PLATFORM_CURRENT == imx25

	m_RefFrequency = 44.333333E6;
#elif PLATFORM_CURRENT == imx1

	m_RefFrequency = 16E6;	// (16 MHZ )
#else
	//#error (no platform selected)
#endif

	switch ((unsigned long)pUart)
	{
	case  CSP_BASE_REG_PA_UART1:
		mUartnum=1;
		break;
	case  CSP_BASE_REG_PA_UART2:
		mUartnum=2;
		break;
	case  CSP_BASE_REG_PA_UART3:
		mUartnum=3;
		break;
	case  CSP_BASE_REG_PA_UART4:
		mUartnum=4;
		break;
	case  CSP_BASE_REG_PA_UART5:
		mUartnum=5;
		break;
	default:
		m_pUart=NULL;
		mUartnum=0;
	}
}

// Desstructor
CUart::~CUart()
{}
void CUart::SetCTSMode(int mode)
{
	switch (mode)
	{
	case CTSOFF:
	default:
		IgnoreCtsInput();
		RtsManualMode();	// manual RTS control (RTS is the MX1 CTS output). We activate RTS while we send data. CTS is also software controled GPIO port
		ForceRtsOutActive();
		break;
	case HNDSHK:
		if(HardwareToken.IAmOEM())
			IgnoreCtsInput();
		else
			UseCtsInput();
		RtsManualMode();	// manual RTS control (RTS is the MX1 CTS output). We activate RTS while we send data.
		ForceRtsOutPassive();
		break;
	case FLOW:
		ForceRtsOutPassive();
		if(HardwareToken.IAmOEM())
			IgnoreCtsInput();
		else
			UseCtsInput();
		RtsAutolMode();   			// auto RTS control (RTS is the MX1 CTS output).
		SetRtsAutoLevel(24);
		break;
	case CTSMAN:
	//ewr-02787
	case CTSFST:
//
	case SCANCTRL:
	case CTSPLS:
		IgnoreCtsInput();
		RtsManualMode();	// manual RTS control (RTS is the MX1 CTS output). We activate RTS while we send data. CTS is also software controled GPIO port
		ForceRtsOutPassive();
		break;
	}
}


MatrixError_t CUart::ExtractParameters(const tichar_t *pParameters )
{
	MatrixError_t ret;
	ret = m_setting.ExtractParameters(pParameters);
	return ret;
}

MatrixError_t CUart::SetupHardware(void)
{
	MatrixError_t ret = ERR_PASS;

	// Setup the baud rate prescaler
	SetRefDiv(5);			// refeeence / by 1 see spec

	// ensure we've got default values
	m_pUart->Ucr1 = 0;
	m_pUart->Ucr2 = 1;	                 // do not touch the /SRST bit !
	m_pUart->Ucr3 = 4;					 // mux select 1
	m_pUart->Ucr4 = 0;
	//	m_pUart->Ucr4.b.REF16 = 1;			// reference clock is 16Mhz
	//	m_pUart->Ucr1.b.UARTCLKENAB = 1;	// turn on the UART clocks
	UartEnable();							// enable the UART


	switch (GetParity())
	{
	case NOPARITY:
		PartyDisable();
		break;	// nothing to do
	case ODDPARITY:
		SetPartyOdd();
		PartyEnable();
		break;
	case EVENPARITY:
		SetPartyEven();
		PartyEnable();
		break;
	default:
		ret = ERR_BADPARAM;
		break;
	}
	if (ret == ERR_PASS)
	{
		SetTxTriggerLevel(16);
		SetRxTriggerLevel(1);

		// we generate 2 stopbits even if ONE5STOPBITS is specified
		if (GetStopBits() != ONESTOPBIT)
		{
			Set2StopBits();
		}

		if (GetDataBits() == 8)
		{
			Set8BitWords();
		}

		SetCTSMode(IsRtsCts());
		// enable RX and TX
		TxEnable();
		RxEnable();
		return SetupBaudrate();
	}
	else
		return ret;
}

/*
The baudrate is calculated with the following formula:
[(Desired Baud Rate)*16]/(reference frequency)=NUM/DENOM
*/
MatrixError_t CUart::SetupBaudrate(void)
{
	MatrixError_t ret = ERR_PASS;
	double Ratio = (GetBaudRate() * 16.0) / m_RefFrequency;

	//	We calculate the ratio registers in two steps:
	// The step 1 gives us some error (for low baudrates) which we correct in step 2.
	// Step 1:
	double Denom1=30000.;	// we choose a guess value
	double Num = Ratio * Denom1;
	if(Num  < 1)
		Num = 1.0;				// avoid underflow
	if(Num > USHRT_MAX)
		Num = USHRT_MAX-1;	// avoid overflow

	Num = floor(Num+0.5);	// round to nearest interger

	// Step2:
	// Here we correct our first guess value for the denom
	double Denom2 = Num / Ratio ;
	Denom2 = floor(Denom2+0.5);	// round to nearest interger

	// do some sanity checks
	if(Denom2 < 1)
		ret = ERR_BADPARAM;
	if(Denom2 > USHRT_MAX)
		ret = ERR_BADPARAM;

	double Error = ((Num / Denom2) - Ratio) / Ratio;
	if(Error > 0.0015)	// 1.5%
		ret = ERR_BADPARAM;

	// write to the chip
	m_pUart->Ubir = (USHORT)(Num-1);
	m_pUart->Ubmr = (USHORT)(Denom2-1);

	return ret;
}


MatrixError_t CUart::Close()
{
	RxMinFifoIrqDisable();
	RxIdleIrqDisable();
	RxTimeoutIrqDisable();
	RxByteByByteIrqDisable();
	RxStatusIrqDisable();
	CtsIrqDisable();
	ParityErrIrqDisable();

	TxReadyIrqDisable();
	TxEmptyIrqDisable();
	TxCompleteIrqDisable();
	if (mUartnum !=1)  // keep debugging on FIXME
	{
		RxDisable();
		TxDisable();

		// disable the UART
		UartDisable();
	}
	return ERR_PASS;
}
// set up gpioinuse /pad registers for use as uart
void CUart::SetIoPins()
{
	switch(mUartnum)
	{
	case 1:
		*(volatile unsigned int * ) IOMUXC_SW_MUX_CTL_PAD_UART1_RXD =MUX_MODE_0;
		*(volatile unsigned int * ) IOMUXC_SW_MUX_CTL_PAD_UART1_TXD =MUX_MODE_0;
		*(volatile unsigned int * ) IOMUXC_SW_MUX_CTL_PAD_UART1_RTS =MUX_MODE_0;
		*(volatile unsigned int * ) IOMUXC_SW_MUX_CTL_PAD_UART1_CTS =MUX_MODE_0;
		break;
	case 2:	// rfbase cradle uart
		*(volatile unsigned int * ) IOMUXC_SW_MUX_CTL_PAD_UART2_RXD =MUX_MODE_0;
		*(volatile unsigned int * ) IOMUXC_SW_MUX_CTL_PAD_UART2_TXD =MUX_MODE_0;
		*(volatile unsigned int * ) IOMUXC_SW_MUX_CTL_PAD_UART2_RTS =MUX_MODE_0;
		*(volatile unsigned int * ) IOMUXC_SW_MUX_CTL_PAD_UART2_CTS =MUX_MODE_0;
		break;
	case 4:	// bluetooth uart
		*((volatile unsigned int * )IOMUXC_SW_MUX_CTL_PAD_LD8) = MUX_MODE_2;
		*((volatile unsigned int * )IOMUXC_SW_MUX_CTL_PAD_LD9) = MUX_MODE_2;
		*((volatile unsigned int * )IOMUXC_SW_MUX_CTL_PAD_LD10) = MUX_MODE_2;
		*((volatile unsigned int * )IOMUXC_SW_MUX_CTL_PAD_LD11) = MUX_MODE_2;

		// IOMUXC_UART4_IPP_UART_RXD_MUX_SELECT_INPUT default value OK
		// IOMUXC_UART4_IPP_UART_RTS_B_SELECT_INPUT default value OK
		break;

	}
}

// set up gpio in use / pad registers for use in cts manual mode
void CUart::SetCtsIoPinsToGpio()
{
	if(HardwareToken.IAmOEM())
	{
		switch(mUartnum)
		{
		case 1:
			//GPIO4, pin 24, Function: 232_CTS / nKey_Clk_In   Pin name: UART1_RTS    GPIO4_24
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT24_LSH,GPIO_BIT25_WID,GPIO_GDIR_INPUT);		//set direction to input
			HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART1_CTS,0x000000E0);		//enable Pull-up 100k
			HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_CTS,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO

			//*(volatile unsigned int * ) IOMUXC_SW_MUX_CTL_PAD_UART1_RTS = MUX_MODE_5;
			break;
		case 2:	// rfbase cradle uart

			break;
		case 4:	// bluetooth uart
			break;
		}
	}
	else
	{
		switch(mUartnum)
		{
		case 1:
			//GPIO4, pin 24, Function: 232_CTS / nKey_Clk_In   Pin name: UART1_RTS    GPIO4_24
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT24_LSH,GPIO_BIT24_WID,GPIO_GDIR_INPUT);		//set direction to input
			HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART1_RTS,0x000000E0);		//enable Pull-up 100k
			HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_RTS,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO

			//*(volatile unsigned int * ) IOMUXC_SW_MUX_CTL_PAD_UART1_RTS = MUX_MODE_5;
			break;
		case 2:	// rfbase cradle uart

			break;
		case 4:	// bluetooth uart
			break;
		}

	}
}

void CUart::SetRtsCtsIoPinsToGPIO()
{
	switch(mUartnum)
	{
	case 1:
		//GPIO4, pin 25, Function: 232_RTS / nKey_Clk_OUT   Pin name: UART1_CTS    GPIO4_25
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT25_LSH,GPIO_BIT25_WID,GPIO_GDIR_INPUT);		//set direction to input
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART1_CTS,0x000000E0);		//enable Pull-up 100k
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_CTS,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO
		
		//GPIO4, pin 24, Function: 232_CTS/nKey_Clk_in Pin name: UART1_RTS    GPIO4_24
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT24_LSH,GPIO_BIT24_WID,GPIO_GDIR_OUTPUT);		//set direction to OUTput
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART1_RTS,0x000000E0);		//enable Pull-up 100k
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_RTS,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO
		
		//*(volatile unsigned int * ) IOMUXC_SW_MUX_CTL_PAD_UART1_RTS = MUX_MODE_5;
		break;
	case 2:	// rfbase cradle uart

		break;
	case 4:	// bluetooth uart
		break;
	}
}
// set up gpio in use / pad registers for use in CTS uart controled mode
void CUart::SetCtsIoPinsToRts()
{
	switch(mUartnum)
	{
	case 1:
		//GPIO4, pin 24, Function: 232_CTS / nKey_Clk_In   Pin name: UART1_RTS    GPIO4_24
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT24_LSH,GPIO_BIT24_WID,GPIO_GDIR_INPUT);		//set direction to input
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART1_RTS,0x000000E0);		//enable Pull-up 100k
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_RTS,MUX_MODE_0);		//0x00 to select mux mode 0 RTS

		//*(volatile unsigned int * ) IOMUXC_SW_MUX_CTL_PAD_UART1_RTS = MUX_MODE_0;
		break;
	case 2:	// rfbase cradle uart

		break;
	case 4:	// bluetooth uart
		break;

	}
}


	//Read Gpio for cts state
unsigned long CUart::ReadCtsIoPinFromGpio()
{
	unsigned long uiReadCts = 0;

	switch(mUartnum)
	{
	case 1:
		//GPIO4, pin 24, Function: 232_CTS / nKey_Clk_In   Pin name: UART1_RTS    GPIO4_24
	if(HardwareToken.IAmOEM())
		uiReadCts = (unsigned long) READ_FIELD(GPIO4_DR0,GPIO_BIT25_LSH,GPIO_BIT25_WID);		//read CTS
	else
		uiReadCts = (unsigned long) READ_FIELD(GPIO4_DR0,GPIO_BIT24_LSH,GPIO_BIT24_WID);		//read CTS

		break;
	case 2:	// rfbase cradle uart

		break;
	case 4:	// bluetooth uart
		break;
	}

	return uiReadCts;
}


void CUart::EnableIrqAIC(void)
{
	EnableInterruptNumber(UartVector[mUartnum]);
}

void CUart::DisableIrqAIC(void)
{
	DisableInterruptNumber(UartVector[mUartnum]);
}






