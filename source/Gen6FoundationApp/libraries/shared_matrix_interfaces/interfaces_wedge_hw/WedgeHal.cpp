/*=================================================================================

//=================================================================================
   $RCSfile: interfaces_wedge_hw/WedgeHal.cpp $
   $Revision: 1.23 $
   $Date: 2011/11/18 02:38:29EST $

//=================================================================================

//=================================================================================*/

/*-----------------------------------------------------------------------------------------------
 **************************class CWedgeHal***************************
-----------------------------------------------------------------------------------------------*/

#include "iMX25_GPIO.h"
#include "iMX25_IOMUX.h"
#include "HardwareMacros.h"
#include "readerconfiguration.h"
#include "iMX25_INTVEC.h"
#include "interrupts.h"
#include "WedgeHal.h"
#include "stdio.h"
#include "stdInclude.h"
#include "Product.h"
#include "string.h"
#include "db_chrio.h"
#include "HardwareToken.h"

CWedgeHal *g_pWedgeHal=NULL;
////////////////////////////////////////////////////////////////////////////////////////////////

CWedgeHal::CWedgeHal(void)
	: m_Port1((GpioPort_t*) GPIO1_BASE_ADDR),
	  m_Port2((GpioPort_t*) GPIO2_BASE_ADDR),
	  m_Port3((GpioPort_t*) GPIO3_BASE_ADDR),
	  m_Port4((GpioPort_t*) GPIO4_BASE_ADDR)
{
	m_mTimerInterruptForceBitmask=0x01<<(INTERFACE_TIMER_VECTOR&0x1F);
	m_pTimerInteruptForceRegister=(GpioPort_t *)ASIC_INTFRCH;
	m_InterfaceTimer=new TInterfaceTimer();
#ifndef PROTO_HW
	SetupPort(TERM_DATA_IN,	m_Port3,BitNum1,INVERTED);

	SetupPort(TERM_DATA_OUT,m_Port4,BitNum23,INVERTED);

	SetupPort(TERM_DATA_DIR,m_Port4,BitNum20,INVERTED);

	SetupPort(KBD_DATA_IN,	m_Port3,BitNum3,INVERTED);
	SetupPort(KBD_DATA_OUT,m_Port4,BitNum25,INVERTED);
	SetupPort(KBD_DATA_DIR,	m_Port4,BitNum19,INVERTED);

	SetupPort(KBD_CLK_IN,	m_Port4,BitNum24,INVERTED);
	SetupPort(KBD_CLK_OUT,	m_Port3,BitNum4,NOT_INVERTED);
	SetupPort(KBD_CLK_DIR,	m_Port4,BitNum18,INVERTED);

	SetupPort(TERM_CLK_IN,	m_Port4,BitNum22,INVERTED);

	SetupPort(TERM_CLK_OUT,	m_Port3,BitNum2,NOT_INVERTED);
	SetupPort(TERM_CLK_DIR,	m_Port4,BitNum17,INVERTED);


	SetupPort(KBD_ENABLE,	m_Port2,BitNum12,NOT_INVERTED);

	WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_EB0,MUX_MODE_5);	//KBD_SW_CNTRL
	HAZARD_WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT12_LSH,GPIO_BIT12_WID,GPIO_HIGH);


#else

	SetupPort(TERM_DATA_IN,	m_Port3,BitNum1,INVERTED);

	SetupPort(TERM_DATA_OUT,m_Port4,BitNum23,INVERTED);

	SetupPort(TERM_DATA_DIR,m_Port3,BitNum16,INVERTED);

	SetupPort(KBD_DATA_IN,	m_Port3,BitNum3,INVERTED);
	SetupPort(KBD_DATA_OUT,m_Port4,BitNum25,INVERTED);
	SetupPort(KBD_DATA_DIR,	m_Port3,BitNum17,INVERTED);

	SetupPort(KBD_CLK_IN,	m_Port4,BitNum24,INVERTED);
	SetupPort(KBD_CLK_OUT,	m_Port3,BitNum4,NOT_INVERTED);
	SetupPort(KBD_CLK_DIR,	m_Port3,BitNum18,INVERTED);

	SetupPort(TERM_CLK_IN,	m_Port4,BitNum22,INVERTED);

	SetupPort(TERM_CLK_OUT,	m_Port3,BitNum2,NOT_INVERTED);
	SetupPort(TERM_CLK_DIR,	m_Port3,BitNum19,INVERTED);

	SetupPort(KBD_ENABLE,	m_Port1,BitNum24,NOT_INVERTED);

	WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_LSCLK,MUX_MODE_5);	//KBD_SW_CNTRL
	HAZARD_WRITE_FIELD_TO_REG(GPIO1_DR0,GPIO_BIT24_LSH,GPIO_BIT24_WID,GPIO_HIGH);

#endif //endof PROTO_HW
}

CWedgeHal::~CWedgeHal(void)
{
	delete m_InterfaceTimer;

}


void CWedgeHal::CreateWedgeHal(void)
{
	g_pWedgeHal=new CWedgeHal();
}

void CWedgeHal::SetupPort(Ports port, CGpio &PortAddr, bit_number_t bitnumber, bool inverted)
{
	ASSERT(port < MAX_PORT_ID);
	if (port < MAX_PORT_ID)
	{
		CPortDetails::PortType type=CPortDetails::INPUT;
		if(port > LAST_IN_PORT_ID)
		{
			type = CPortDetails::BIDIR;
		}
		GetPort(port)->SetupPort(PortAddr, bitnumber, inverted, type);
	}
}

void CWedgeHal::MakeInterfaceIoBenign(void)
{
	DISABLE_INTERRUPTS();
#ifndef PROTO_HW
	if(!(HardwareToken.IAmOEM()))
	{
		//GPIO2, pin 12, Function: KBD_SW_CNTRL		Pin name: EB0   GPIO2_12
		HAZARD_WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT12_LSH,GPIO_BIT12_WID,GPIO_HIGH);	//set data reg to high, to enable analog switches
		HAZARD_WRITE_FIELD_TO_REG(GPIO2_GDIR0,GPIO_BIT12_LSH,GPIO_BIT12_WID,GPIO_GDIR_OUTPUT);	//set direction to output
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_EB0,0x00002042);		//1.8V, Pull disabled, keeper not selected, cmos high drive and low slew
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_EB0,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO

		//GPIO4, pin 20, Function: n232 / KBD_OE1   Pin name: D0   GPIO4_20
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT20_LSH,GPIO_BIT20_WID,0x1);			//set data reg to high, to disable output gate
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT20_LSH,GPIO_BIT20_WID,GPIO_GDIR_OUTPUT);	//set direction to output
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_D0,0x00000047);		//Pull disabled, keeper not selected, cmos max drive and high slew
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_D0,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO

	//GPIO4, pin 19, Function: n232 / KBD_OE2   Pin name: D1   GPIO4_19
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT19_LSH,GPIO_BIT19_WID,0x1);		//set data reg to high, to disable output gate
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT19_LSH,GPIO_BIT19_WID,GPIO_GDIR_OUTPUT);	//set direction to output
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_D1,0x00000047);		//Pull disabled, keeper not selected, cmos max drive and high slew
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_D1,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO

	//GPIO4, pin 18, Function: n232 / KBD_OE3   Pin name: D2    GPIO4_18
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT18_LSH,GPIO_BIT18_WID,0x1);		//set data reg to high, to disable output gate
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT18_LSH,GPIO_BIT18_WID,GPIO_GDIR_OUTPUT);	//set direction to output
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_D2,0x00000047);		//Pull disabled, keeper not selected, cmos max drive and high slew
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_D2,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO

		//GPIO4, pin 17, Function: n232 / KBD_OE4   Pin name: D3    GPIO4_17
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,0x1);		//set data reg to high, to disable output gate
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,GPIO_GDIR_OUTPUT);	//set direction to output
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_D3,0x00000047);		//Pull disabled, keeper not selected, cmos max drive and high slew
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_D3,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO
	}
#else
	//KBD_SW_CNTRL
	HAZARD_WRITE_FIELD_TO_REG(GPIO1_DR0,GPIO_BIT24_LSH,GPIO_BIT24_WID,0);
	HAZARD_WRITE_FIELD_TO_REG(GPIO1_GDIR0,GPIO_BIT24_LSH,GPIO_BIT24_WID,GPIO_GDIR_OUTPUT);
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_LSCLK,MUX_MODE_5);

	//Function: n232/KBD_OE1   Pin: UPLL_BYPCLK   GPIO3_16
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UPLL_BYPCLK,MUX_MODE_5);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT16_LSH,GPIO_BIT16_WID,GPIO_GDIR_OUTPUT);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT16_LSH,GPIO_BIT16_WID,0x1);

	//Function: n232/KBD_OE2   Pin: VSTBY_REQ   GPIO3_17
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_VSTBY_REQ,MUX_MODE_5);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,GPIO_GDIR_OUTPUT);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,0x1);

	//Function: n232/KBD_OE3   Pin: VSTBY_ACK    GPIO3_18
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_VSTBY_ACK,MUX_MODE_5);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT18_LSH,GPIO_BIT18_WID,GPIO_GDIR_OUTPUT);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT18_LSH,GPIO_BIT18_WID,0x1);

	//Function: n232/KBD_OE4   Pin: POWER_FAIL    GPIO3_19
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_POWER_FAIL,MUX_MODE_5);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT19_LSH,GPIO_BIT19_WID,GPIO_GDIR_OUTPUT);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT19_LSH,GPIO_BIT19_WID,0x1);
#endif //endof PROTO_HW


	//GPIO4, pin 22, Function: 232_RxD / nTerm_Clk_In   Pin name: UART1_RXD    GPIO4_22
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT22_LSH,GPIO_BIT22_WID,GPIO_GDIR_INPUT);		//set direction to input
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART1_RXD,0x00000000);		//Disable Pull-up
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_RXD,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO

	//GPIO4, pin 24, Function: 232_CTS / nKey_Clk_In   Pin name: UART1_RTS    GPIO4_24
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT24_LSH,GPIO_BIT24_WID,GPIO_GDIR_INPUT);		//set direction to input
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART1_RTS,0x00000000);		//Disable Pull-up
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_RTS,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO
	
	if(!(HardwareToken.IAmOEM()))
	{
		//GPIO3, pin 1, Function: nTERM_DATA_IN   Pin name: KPP_COL0    GPIO3_1
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT1_LSH,GPIO_BIT1_WID,GPIO_GDIR_INPUT);		//set direction to input
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_KPP_COL0,0x00000000);		//Disable Pull-up
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_COL0,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO

		//GPIO3, pin 3, Function: nKEY_DATA_IN   Pin name: KPP_COL2    GPIO3_3
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT3_LSH,GPIO_BIT3_WID,GPIO_GDIR_INPUT);		//set direction to input
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_KPP_COL2,0x00000000);		//Disable Pull-up
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_COL2,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO
	}

	RESTORE_INTERRUPTS();
}

//#define USE_IRQ_FOR_WEDGE	1

void CWedgeHal::PrepareIoForWedge(void)
{
	MakeInterfaceIoBenign();

	DISABLE_INTERRUPTS();

	//GPIO 4, pin 23, Function: 232_TxD / Term_Data_Out   Pin name: UART1_TXD    GPIO4_23
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_TXD,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT23_LSH,GPIO_BIT23_WID,GPIO_GDIR_OUTPUT);	//set direction to output

	//GPIO 4, pin 25, Function: 232_RTS / Key_Data_Out   Pin name: UART1_CTS    GPIO4_25
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_CTS,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT25_LSH,GPIO_BIT25_WID,GPIO_GDIR_OUTPUT);	//set direction to output
	
	if(!(HardwareToken.IAmOEM()))
	{
		//GPIO 3, pin 2, Function: TERM_CLK_OUT   Pin name: KPP_COL1    GPIO3_2
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_COL1,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT2_LSH,GPIO_BIT2_WID,GPIO_GDIR_OUTPUT);	//set direction to output

		//GPIO 3, pin 4, Function: KEY_CLK_OUT   Pin name: KPP_COL3    GPIO3_4
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_COL3,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT4_LSH,GPIO_BIT4_WID,GPIO_GDIR_OUTPUT);	//set direction to output
	}

	RESTORE_INTERRUPTS();

	KeyboardEnable();

#ifdef USE_IRQ_FOR_WEDGE
	PrepareIoIrqForWedge();
#endif
}


#ifdef USE_IRQ_FOR_WEDGE
void CWedgeHal::PrepareIoIrqForWedge(void)
{
	DisableInterruptNumber(VEC_GPIO3_INT);
	DisableInterruptNumber(VEC_GPIO4_INT);

	DISABLE_INTERRUPTS();

	//GPIO 4, pin 24, cts input /  key clk in
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_IMR,GPIO_BIT24_LSH,GPIO_BIT24_WID,0x0);		//Enable Interupt mask bit 24
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_ICR2,GPIO_BIT17_LSH,GPIO_BIT17_WID,0x0);    //set 0x10, interrupt rising edge
	HAZARD_WRITE_REG(GPIO4_ISR,(0x01<<GPIO_BIT24_LSH));                         //clear interupt status bit 24
	
	if(!(HardwareToken.IAmOEM()))
	{
		//GPIO 3, pin 3,  key data in
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_IMR,GPIO_BIT3_LSH,GPIO_BIT3_WID,0x0);		//Enable Interupt mask bit 3
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_ICR1,GPIO_BIT7_LSH,GPIO_BIT7_WID,0x0);		//set 0x10, interrupt rising edge
		HAZARD_WRITE_REG(GPIO3_ISR,(0x01<<GPIO_BIT3_LSH));							//clear interupt status bit 3
		
		//GPIO 3, pin 1,  term data in
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_IMR,GPIO_BIT1_LSH,GPIO_BIT1_WID,0x0);		//Enable Interupt mask bit 1
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_ICR1,GPIO_BIT3_LSH,GPIO_BIT3_WID,0x0);		//set 0x10, interrupt rising edge
		HAZARD_WRITE_REG(GPIO3_ISR,(0x01<<GPIO_BIT1_LSH));                          //clear interupt status bit 1
	}
	
	//GPIO 4, pin 22, RxD input /  term clk in
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_IMR,GPIO_BIT22_LSH,GPIO_BIT22_WID,0x0);		//Enable Interupt mask bit 22
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_ICR2,GPIO_BIT13_LSH,GPIO_BIT13_WID,0x0);    //set 0x10, interrupt rising edge
	HAZARD_WRITE_REG(GPIO4_ISR,(0x01<<GPIO_BIT22_LSH));                         //clear interupt status bit 22
	
	RESTORE_INTERRUPTS();

	EnableInterruptNumber(VEC_GPIO4_INT);
	EnableInterruptNumber(VEC_GPIO3_INT);
}
#endif


bool CWedgeHal::BitInfo(Ports port, bool &inverted)
{
	ASSERT(port < MAX_PORT_ID);
	if (port < MAX_PORT_ID)
	{
		return GetPort(port)->BitInfo(inverted);
	}
	return FAIL;
}

bool CWedgeHal::ReadBit(Ports port)
{
	CPortDetails *pPort;
	GetPort(port, pPort);
	return pPort->IsBitSet();
}

void CWedgeHal::KeyboardEnable (void)
{
	DoSetPort(KBD_ENABLE);
}

void CWedgeHal::KeyboardDisable (void)
{
	DoClrPort(KBD_ENABLE);
}

void CWedgeHal::DoClrPort(Ports port_id)
{
	DISABLE_INTERRUPTS();
	DoClrPortFast(port_id);
	RESTORE_INTERRUPTS();
}

void CWedgeHal::DoSetPort(Ports port_id)
{
	DISABLE_INTERRUPTS();
	DoSetPortFast(port_id);
	RESTORE_INTERRUPTS();
}

void CWedgeHal::DoClrPortFast(Ports port_id)
{
	GetPort(port_id)->DoClrPort();
}

void CWedgeHal::DoSetPortFast(Ports port_id)
{
	GetPort(port_id)->DoSetPort();
}


// ===  Function  ======================================================================
//
//         Name:  PrepareCableInterfaceforUSBPower
//
//  Description:  Disables the Output Enables and 232 Communication pins.  This is done
//					to draw the least amount of power in USB Suspend mode.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void CWedgeHal::PrepareCableInterfaceforUSBPower(void)
{
#ifndef PROTO_HW

	//Drive UART1_CTS High,   Function: 232_RTS / Key_Data_Out   Pin name: UART1_CTS    GPIO4_25
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_CTS,MUX_MODE_5);
	if(HardwareToken.IAmOEMJade())	// !!!! Set to input since the RTS and CTS pin of 232 are reversed.   //NGOEM208
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT25_LSH,GPIO_BIT25_WID,GPIO_GDIR_INPUT);
	else
		{
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT25_LSH,GPIO_BIT25_WID,GPIO_HIGH);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT25_LSH,GPIO_BIT25_WID,GPIO_GDIR_OUTPUT);
		}

	if(!(HardwareToken.IAmOEM()))	
	{
		//Set KEY_CLK_OUT - KPP_COL3/PORT3_4 Low,   Function: KEY_CLK_OUT   Pin name: KPP_COL3    GPIO3_4
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_COL3,MUX_MODE_5);
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT4_LSH,GPIO_BIT4_WID,GPIO_LOW);
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT4_LSH,GPIO_BIT4_WID,GPIO_GDIR_OUTPUT);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_KPP_COL3,0x00000000);
		//Set TERM_CLK_OUT - KPP_COL1/PORT3_2,   Function: TERM_CLK_OUT   Pin name: KPP_COL1    GPIO3_2
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_COL1,MUX_MODE_5);
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT2_LSH,GPIO_BIT2_WID,GPIO_HIGH);
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT2_LSH,GPIO_BIT2_WID,GPIO_GDIR_OUTPUT);

		//n232/KBD_OE1 - GPIO4_20 - LOW,   Function: n232 / KBD_OE1   Pin name: D0   GPIO4_20
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_D0,MUX_MODE_5);
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT20_LSH,GPIO_BIT20_WID,GPIO_LOW);
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT20_LSH,GPIO_BIT20_WID,GPIO_GDIR_OUTPUT);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_D0,0x00000047); 		//keeper off, slow slew and normal drive

		//n232/KBD_OE2 - GPIO4_19 - LOW,   Function: n232 / KBD_OE2   Pin name: D1   GPIO4_19
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_D1,MUX_MODE_5);
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT19_LSH,GPIO_BIT19_WID,GPIO_LOW);
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT19_LSH,GPIO_BIT19_WID,GPIO_GDIR_OUTPUT);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_D1,0x000000E0);

		//n232/KBD_OE3 - GPIO4_18 - LOW,   Function: n232 / KBD_OE3   Pin name: D2    GPIO4_18
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_D2,MUX_MODE_5);
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT18_LSH,GPIO_BIT18_WID,GPIO_LOW);
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT18_LSH,GPIO_BIT18_WID,GPIO_GDIR_OUTPUT);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_D2,0x000000E0);

		//n232/KBD_OE4 - GPIO4_17 - LOW,   Function: n232 / KBD_OE4   Pin name: D3    GPIO4_17
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_D3,MUX_MODE_5);
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,GPIO_HIGH);
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,GPIO_GDIR_OUTPUT);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_D3,0x000000E0);

		//HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART1_RXD,0x00000000);	//Disable Pull-up for UART1_RXD

		//GPIO2, pin 12, Function: KBD_SW_CNTRL		Pin name: EB0   GPIO2_12
		HAZARD_WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT12_LSH,GPIO_BIT12_WID,GPIO_HIGH);	//set data reg to high, to enable analog switches
		HAZARD_WRITE_FIELD_TO_REG(GPIO2_GDIR0,GPIO_BIT12_LSH,GPIO_BIT12_WID,GPIO_GDIR_OUTPUT);	//set direction to output
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_EB0,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_EB0,0x00002040);		//1.8V, Pull disabled, keeper not selected, cmos normal drive and low slew
		
		//GPIO3, pin 1, Function: nTERM_DATA_IN   Pin name: KPP_COL0    GPIO3_1
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT1_LSH,GPIO_BIT1_WID,GPIO_GDIR_INPUT);		//set direction to input
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_KPP_COL0,0x00000000);		//Disable Pull-up
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_COL0,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO

		//GPIO3, pin 3, Function: nKEY_DATA_IN   Pin name: KPP_COL2    GPIO3_3
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT3_LSH,GPIO_BIT3_WID,GPIO_GDIR_INPUT);		//set direction to input
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_KPP_COL2,0x00000000);		//Disable Pull-up
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_COL2,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO
	}

	//Set 232_TXD - UART1_TXD/PORT4_23 Low,   Function: 232_TxD / Term_Data_Out   Pin name: UART1_TXD    GPIO4_23
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_TXD,MUX_MODE_5);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT23_LSH,GPIO_BIT23_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT23_LSH,GPIO_BIT23_WID,GPIO_GDIR_OUTPUT);
	
	//GPIO4, pin 22, Function: 232_RxD / nTerm_Clk_In   Pin name: UART1_RXD    GPIO4_22
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT22_LSH,GPIO_BIT22_WID,GPIO_GDIR_INPUT);		//set direction to input
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART1_RXD,0x00000000);		//Disable Pull-up
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_RXD,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO

	//GPIO4, pin 24, Function: 232_CTS / nKey_Clk_In   Pin name: UART1_RTS    GPIO4_24
	if(HardwareToken.IAmOEMJade())    // !!!! Set to output since the RTS and CTS pin of 232 are reversed.
	{	
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT24_LSH,GPIO_BIT24_WID,GPIO_HIGH);
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT24_LSH,GPIO_BIT24_WID,GPIO_GDIR_OUTPUT);		//set direction to output
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART1_RTS,0x000000E0);		//enable Pull-up 100k
	}
	else
	{
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT24_LSH,GPIO_BIT24_WID,GPIO_GDIR_INPUT);		//set direction to input
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART1_RTS,0x00000000);		//Disable Pull-up
	}
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_RTS,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO

#else
	//Turn Off Output Enable, only needed for Corded, but will not affect other boards
	//Pin has already been configured for GPIO, GPIO3_16 = n232/KBD_OE1
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT16_LSH,GPIO_BIT16_WID,GPIO_HIGH);

	//Drive UART1_CTS High
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_CTS,MUX_MODE_5);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT25_LSH,GPIO_BIT25_WID,GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT25_LSH,GPIO_BIT25_WID,GPIO_GDIR_OUTPUT);
	
	if(!(HardwareToken.IAmOEM()))
	{
		//Set KEY_CLK_OUT - KPP_COL3/PORT3_4 Low
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_COL3,MUX_MODE_5);
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT4_LSH,GPIO_BIT4_WID,GPIO_LOW);
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT4_LSH,GPIO_BIT4_WID,GPIO_GDIR_OUTPUT);

		//Set TERM_CLK_OUT - KPP_COL1/PORT3_2 Low
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_COL1,MUX_MODE_5);
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT2_LSH,GPIO_BIT2_WID,GPIO_LOW);
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT2_LSH,GPIO_BIT2_WID,GPIO_GDIR_OUTPUT);

		//n232/KBD_OE2 - GPIO3_17 - LOW
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_D1,MUX_MODE_5);
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,GPIO_LOW);
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,GPIO_GDIR_OUTPUT);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_D1,0x000000E0);

		//n232/KBD_OE3 - GPIO3_18 - LOW
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_D2,MUX_MODE_5);
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT18_LSH,GPIO_BIT18_WID,GPIO_LOW);
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT18_LSH,GPIO_BIT18_WID,GPIO_GDIR_OUTPUT);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_D2,0x000000E0);
	}
#endif //endof PROTO_HW
}	//endof PrepareCableInterfaceforUSBPower





