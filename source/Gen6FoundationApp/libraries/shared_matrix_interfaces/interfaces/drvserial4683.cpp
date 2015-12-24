/*=================================================================================
  class: CSerial4683

===================================================================================
   $Source: interfaces/drvserial4683.cpp $
   $Date: 2010/08/20 13:10:59EDT $
   $Revision: 2.12.1.19.1.5 $


==================================================================================*/
#include "stdInclude.h"
#include "CommonDriver.h"
#include "def4683.h"
#include "DrvSerial4683.h"
#include "BarcodeData.h"
#include "BarcodeFifo.h"
#include "menu.h"

//add below for CSerial4683::PrepareIoFor4683
#include "iMX25_GPIO.h"
#include "iMX25_IOMUX.h"
#include "iMX25_UART.h"
#include "readerconfiguration.h"
#include "HardwareToken.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DRIVER_STACK_SIZE 5000


// This is the class factory for this driver
CDriverBase *CreateSerial4683(void)
{
	return (new CSerial4683(pUart1));
}

// Constructor
CSerial4683::CSerial4683(Uart_T *pUart)
: m_Port(pUart),
m_PollResponseDelay4683(TIME_BEFORE_ANSWERING_POLL),
m_PollResponseTimeout4683(TIME_TO_ANSWER_POLL),
Primary4683(GET_SETTING(IBM_4683_Protocol)),
Aux4683(GET_SETTING(IBM_4683_Serial))
{
	m_CParity.BuildParityTable(ODD);

	m_RxVectorNumber = VEC_UART1_INT;
	DisableInterruptNumber(m_RxVectorNumber);

	receivedPoll = false;
	m_RxThreshold = 1;
	m_TxThreshold = 32;
	CurrentRetailAddress = 0;
	PrimaryRetailAddress = 0;
	AuxRetailAddress = 0;
	TxInProgress = false;
	m_ProtocolFormat = 0;
	m_RetailTxBuffer = NULL;
	
    if( m_pRxFifo != NULL )
    {
        delete m_pRxFifo;
    }
    m_pRxFifo = Primary4683.GetRxFifo();
    if( m_pBarcodeFifo != NULL )
    {
        delete m_pBarcodeFifo;
    }
    m_pBarcodeFifo = Primary4683.GetBarcodeFifo();
}

// Desstructor
CSerial4683::~CSerial4683()
{
}

void CSerial4683::PrepareIoFor4683()
{
		DISABLE_INTERRUPTS();
		
		//Setup UART CTS enable 
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_CTS,MUX_MODE_0);		
		m_Port.SetCTSMode(CTSMAN);
		if(!(HardwareToken.IAmOEM()))
		{
		#ifndef PROTO_HW	
		
			//GPIO4_19 to control 74VHCT125
			HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_D1,MUX_MODE_5);
			HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_D1,0x00000047);
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT19_LSH,GPIO_BIT19_WID,GPIO_GDIR_OUTPUT);	//Direction Register PORT3_17
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT19_LSH,GPIO_BIT19_WID,0x0);			//enable converter

			//KBD_OE1->GPIO4_20 to low enable 232
			HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_D0,MUX_MODE_5);
			HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_D0,0x00000047);
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT20_LSH,GPIO_BIT20_WID,GPIO_GDIR_OUTPUT);	//Direction Register PORT3_16
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT20_LSH,GPIO_BIT20_WID,0x0);			//enable converter
		
			//make PORT2_12->KBD_SW_CNTRL->KBD_SW_EN low
			HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_EB0,MUX_MODE_5);
			HAZARD_WRITE_FIELD_TO_REG(GPIO2_GDIR0,GPIO_BIT12_LSH,GPIO_BIT12_WID,GPIO_GDIR_OUTPUT);	//Direction Register PORT2_12
			HAZARD_WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT12_LSH,GPIO_BIT12_WID,0x0);	

			//make n232/KBD_OE4/PORT4_17 high to disable U29D
			HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_D3,MUX_MODE_5);
			HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_D3,0x00000047);
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,GPIO_GDIR_OUTPUT);	//Direction Register PORT3_17
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,0x1);			//enable converter

			//make n232/KBD_OE3 low
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT18_LSH,GPIO_BIT18_WID,GPIO_LOW);
			HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT18_LSH,GPIO_BIT18_WID,GPIO_GDIR_OUTPUT);
			HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_D2,0x00000047);
			HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_D2,MUX_MODE_5);
	
			//make KEY_CLK_OUT - KPP_COL3/PORT3_4 Low
			HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_COL3,MUX_MODE_5);
			HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT4_LSH,GPIO_BIT4_WID,GPIO_LOW);
			HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT4_LSH,GPIO_BIT4_WID,GPIO_GDIR_OUTPUT);
		#else	
			//GPIO3_17 to control 74VHCT125
			HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_VSTBY_REQ,MUX_MODE_5);
			HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,GPIO_GDIR_OUTPUT);	//Direction Register PORT3_17
			HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,0x0);			//enable converter
	
			//KBD_OE1->GPIO3_16 to low enable 232
			HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UPLL_BYPCLK,MUX_MODE_5);
			HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT16_LSH,GPIO_BIT16_WID,GPIO_GDIR_OUTPUT);	//Direction Register PORT3_16
			HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT16_LSH,GPIO_BIT16_WID,0x0);			//enable converter
		
			//make PORT1_24->KBD_SW_CNTRL->KBD_SW_EN low
			HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_LSCLK,MUX_MODE_5);
			HAZARD_WRITE_FIELD_TO_REG(GPIO1_GDIR0,GPIO_BIT24_LSH,GPIO_BIT24_WID,GPIO_GDIR_OUTPUT);	//Direction Register PORT3_17
			HAZARD_WRITE_FIELD_TO_REG(GPIO1_DR0,GPIO_BIT24_LSH,GPIO_BIT24_WID,0x0);			//enable converter

			//make n232/KBD_OE4/PORT3_19 high to disable U29D
			HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_POWER_FAIL,MUX_MODE_5);
			HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT19_LSH,GPIO_BIT19_WID,GPIO_GDIR_OUTPUT);	//Direction Register PORT3_17
			HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT19_LSH,GPIO_BIT19_WID,0x1);			//enable converter

		#endif //endof PROTO_HW
		}
		
		//make sure IBM4683 hardware cannot be borthered by other interface setting
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_RXD,MUX_MODE_0);
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_TXD,MUX_MODE_0);
		
		RESTORE_INTERRUPTS();
}

MatrixError_t CSerial4683::Open(interface_id_t nId, const tichar_t *pParameter)
{
	
	PrepareIoFor4683();
	Rx485Disable();
	MatrixError_t ret = Primary4683.ExtractParameters(pParameter);
	if(IsOK(ret))
		ret &= baseCSerial4683::Open(nId, pParameter);

	if(IsOK(ret))
	{
		m_Port.SetBaudRate(187500);
		ret = m_Port.SetupHardware();		
	}
	
	if(IsOK(ret))
	{
		m_Port.Set8BitWords();
		m_Port.Set2StopBits();
		m_Port.PartyEnable();
		m_Port.SetPartyOdd();

		m_Port.RxMinFifoIrqDisable();
		m_Port.RxIdleIrqDisable();
		m_Port.RxTimeoutIrqDisable();
		m_Port.RxByteByByteIrqDisable();
		m_Port.RxStatusIrqDisable();
		m_Port.CtsIrqDisable();
		m_Port.ParityErrIrqDisable();
		m_Port.TxReadyIrqDisable();
		m_Port.TxEmptyIrqDisable();
		m_Port.TxCompleteIrqDisable();
		FlushTxFifo();
		TxInProgress=false;

	    DISABLE_INTERRUPTS();

        Primary4683.Initialize();                       // Initialize command handler(s)
        Aux4683.Initialize();
        CurrentRetailAddress = Primary4683.GetAddress();
        m_RetailTxBuffer = Primary4683.GetTxMessage();  // Set output buffer to the port's output buffer

        m_RxState = FIRST_CHAR;
        m_EOP_Sent = 0;
        m_OutputCount = 0;

        // test should remove and set to 0
        m_TxFrameLength = 0;
        m_RxFrameLength = 0;

		m_Port.SetTxTriggerLevel(m_TxThreshold);
		m_Port.SetRxTriggerLevel(m_RxThreshold);

		CSerial4683::SetIrqVectors();
		
		Tx485Disable();
		Rx485Enable();

		m_Port.UartEnable();
		m_Port.RxEnable();
		m_Port.IgnoreCtsInput();
		m_Port.TxEnable();
		m_Port.RxByteByByteIrqEnable();			/* allow receiving data from 4683 */

		RunThread();
		RESTORE_INTERRUPTS();	
	}
	return ret;	
}

MatrixError_t CSerial4683::Close(void)
{
	if(Primary4683.GetProhibitClose() == TRUE)
    {    
		return ERR_NOT_ALLOWED;
    }
	else
	{
#ifndef PROTO_HW
		WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT19_LSH,GPIO_BIT19_WID,0x1);			//disable converter
#else
		WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,0x1);			//disable converter
#endif //endof PROTO_HW

		m_Port.Close();
		baseCSerial4683::Close();
		return ERR_PASS;
	}
}

MatrixError_t CSerial4683::IoControlForceClose(void)
{
	Primary4683.SetProhibitClose(false);    // prohibit closing us
	return ERR_PASS;
}

void CSerial4683::SetIrqVectors(void)
{
	DisableInterruptNumber(m_RxVectorNumber);		// safety
	//SetVector(&CSerial4683::Receive4683, m_RxVectorNumber);
	SetVector(&CSerial4683::SimpleRxTxISR, m_RxVectorNumber);
	EnableInterruptNumber(m_RxVectorNumber);
}

threadhandle_t CSerial4683::SimpleRxTxISR(void)
{
	Receive4683();		// read any recieve data

	//here should be if any tx completed pending?
	if (m_Port.IsTxReadyIrq())
	{
		if(TxInProgress)
			Transmit4683(); //handle sending
	}

	return 0;
	//return false;		// no thread switch
}

/// Use for non standard communication from you application to the driver
IoControlRet_t CSerial4683::DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam)
{
		// first call base class
	IoControlRet_t RetVal = baseCSerial4683::DeviceIoControl(cmd, lparam, wparam);
	switch(cmd)
	{
		case IOCTRL_SHALL_I_BEEP_ON_GOOD_READ:      /* Beep if buffered scans is on or scanner is enabled */
            if( Primary4683.IsBeepingEnabled() )
				RetVal = IOCRET_YES;                /* let the Script beep */
			else
				RetVal = IOCRET_NO;
			break;
		case IOCTRL_IS_DONE:
			if(RetVal == IOCRET_YES)                // if base class said yes, then
			{
				if (!m_Port.IsTxComplete())         // if uart fifo is not empty, ...
				RetVal = IOCRET_NO;
			}
			break;
		case IOCTRL_IS_LOW_POWER_ALLOWED:
			RetVal = IOCRET_NO;
			break;
		case 	IOCTRL_IS_CLOSE_PROHIBITED:         // used for 4683
			RetVal = IOCRET_YES;
			break;
		case 	IOCTRL_PREPARE_FORCED_CLOSE:        // used for 4683
            RetVal = IoControlForceClose();
            break;
          
        case    IOCTRL_SET_PRIMARY_PORT:            // used for 4683
            m_pRxFifo = Primary4683.GetRxFifo();
            m_pBarcodeFifo = Primary4683.GetBarcodeFifo();
            RetVal = IOCRET_YES;
            break;           
        case    IOCTRL_SET_AUX_PORT:                // used for 4683
            if( Aux4683.GetAddress() == 0x00 )
            {
                RetVal = IOCRET_NO;
            }
            else 
            {
                m_pRxFifo = Aux4683.GetRxFifo();
                m_pBarcodeFifo = Aux4683.GetBarcodeFifo();
                RetVal = IOCRET_YES;
            }
            break;
        
		default:
			break;
	}
	return RetVal;
}

//	Overloaded Write, checks for barcode type '6' menu response
//	If not '6' it will behave identical to the base Write
size_t CSerial4683::Write(CBarcodeData  *pLabel)
{
	size_t ret = 1;
	AutoTranslate(pLabel);
	if( pLabel ->GetHHPId() == WA_CODELETTER_MENU_RESPONSE && Primary4683.GetExpectingMenuResponse())
	{
		Primary4683.StoreMenuResponse(pLabel);
	}
	else
	{
		ret = m_pBarcodeFifo->Write(pLabel);
	}
	return ret;
}

size_t CSerial4683::Write(const bchar_t *pBin, size_t nSize)
{
	return CDriverBase::Write( pBin, nSize );

}

size_t CSerial4683::Write(const wchar_t *pText, size_t nSize)
{
	return CDriverBase::Write( pText, nSize );
}




