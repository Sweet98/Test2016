/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  HARDWARE DEFINITION FILE
//  Date:       9/12/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: db_shared/Hardware_Header_Files/iMX25/iMX25_UART.h $
// $Revision: 1.6 $
// $Date: 2011/06/28 13:26:42EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx25_uart.h
//
// Provides definitions for the 6 general-purpose timers (GPT)
// Refer to Chapter 42 of the iMX25 Reference Manual
//
//-----------------------------------------------------------------------------
#ifndef IMX25_UART_H_
#define IMX25_UART_H_

#include "Hardware_Width_Defines.h"		//Used for the Mask Width
#define UART_CLK_REF 44333333
//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------
#define CSP_BASE_REG_PA_UART1           (0x43F90000U)
#define CSP_BASE_REG_PA_UART2           (0x43F94000U)
#define CSP_BASE_REG_PA_UART3           (0x5000C000U)
#define CSP_BASE_REG_PA_UART4           (0x50008000U)
#define CSP_BASE_REG_PA_UART5           (0x5002C000U)
#ifndef BIT
#define BIT(aU) (1<<a)
#endif
typedef struct 
{
	volatile unsigned int Urxd[0x40/sizeof(unsigned int)];		// 32bit receiver reg
	volatile unsigned int Utxd[0x40/sizeof(unsigned int)];		// 32bit transmitter reg
	volatile unsigned int Ucr1;				// 32bit control 1 reg
	volatile unsigned int Ucr2;				// 32bit control 2 reg
	volatile unsigned int Ucr3;				// 32bit control 3 reg
	volatile unsigned int Ucr4;				// 32bit control 4 reg
	volatile unsigned int Ufcr;				// 32bit fifo control reg
	volatile unsigned int Usr1;				// 32bit status 1 reg
	volatile unsigned int Usr2;				// 32bit status 2 reg
	volatile unsigned int UESC;            	// 32bit uart1 escape char reg
	volatile unsigned int UTIM;            	// 32bit uart1 escape timer reg
	volatile unsigned int Ubir;            	// 32bit uart1 BRM incremental reg
	volatile unsigned int Ubmr;            	// 32bit uart1 BRM modulator reg
	volatile unsigned int UBRC;       		// 32bit uart1 baud rate count reg
	volatile unsigned int ONEMS;           	// 32bit uart1 one ms reg
	volatile unsigned int UTS;             	// 32bit uart1 test reg
}
Uart_T;
//#define pUart1 (Uart_Reg_t *)CSP_BASE_REG_PA_UART1           
Uart_T * const pUart1 ((Uart_T *)CSP_BASE_REG_PA_UART1);           
Uart_T * const pUart2 ((Uart_T *)CSP_BASE_REG_PA_UART2);           
Uart_T * const pUart3 ((Uart_T *)CSP_BASE_REG_PA_UART3);           
Uart_T * const pUart4 ((Uart_T *)CSP_BASE_REG_PA_UART4);           
Uart_T * const pUart5 ((Uart_T *)CSP_BASE_REG_PA_UART5);           


//------------------------------------------------------------------------------
// REGISTER OFFSETS
//------------------------------------------------------------------------------

// offsets relative to base address
#define UART_URXD            (0x0000U)	// 32bit uart1 receiver reg
#define UART_UTXD            (0x0040U)	// 32bit uart1 transmitter reg
#define UART_UCR1            (0x0080U)	// 32bit uart1 control 1 reg
#define UART_UCR2            (0x0084U)	// 32bit uart1 control 2 reg
#define UART_UCR3            (0x0088U)	// 32bit uart1 control 3 reg
#define UART_UCR4            (0x008CU)	// 32bit uart1 control 4 reg
#define UART_UFCR            (0x0090U)	// 32bit uart1 fifo control reg
#define UART_USR1            (0x0094U)	// 32bit uart1 status 1 reg
#define UART_USR2            (0x0098U)	// 32bit uart1 status 2 reg
#define UART_UESC            (0x009CU)	// 32bit uart1 escape char reg
#define UART_UTIM            (0x00A0U)	// 32bit uart1 escape timer reg
#define UART_UBIR            (0x00A4U)	// 32bit uart1 BRM incremental reg
#define UART_UBMR            (0x00A8U)	// 32bit uart1 BRM modulator reg
#define UART_UBRC            (0x00ACU)	// 32bit uart1 baud rate count reg
#define UART_ONEMS           (0x00B0U)	// 32bit uart1 one ms reg
#define UART_UTS             (0x00B4U)	// 32bit uart1 test reg

//UART 1:
#define UART1_URXD            (CSP_BASE_REG_PA_UART1+0x0000U)	// 32bit uart1 receiver reg
#define UART1_UTXD            (CSP_BASE_REG_PA_UART1+0x0040U)	// 32bit uart1 transmitter reg
#define UART1_UCR1            (CSP_BASE_REG_PA_UART1+0x0080U)	// 32bit uart1 control 1 reg
#define UART1_UCR2            (CSP_BASE_REG_PA_UART1+0x0084U)	// 32bit uart1 control 2 reg
#define UART1_UCR3            (CSP_BASE_REG_PA_UART1+0x0088U)	// 32bit uart1 control 3 reg
#define UART1_UCR4            (CSP_BASE_REG_PA_UART1+0x008CU)	// 32bit uart1 control 4 reg
#define UART1_UFCR            (CSP_BASE_REG_PA_UART1+0x0090U)	// 32bit uart1 fifo control reg
#define UART1_USR1            (CSP_BASE_REG_PA_UART1+0x0094U)	// 32bit uart1 status 1 reg
#define UART1_USR2            (CSP_BASE_REG_PA_UART1+0x0098U)	// 32bit uart1 status 2 reg
#define UART1_UESC            (CSP_BASE_REG_PA_UART1+0x009CU)	// 32bit uart1 escape char reg
#define UART1_UTIM            (CSP_BASE_REG_PA_UART1+0x00A0U)	// 32bit uart1 escape timer reg
#define UART1_UBIR            (CSP_BASE_REG_PA_UART1+0x00A4U)	// 32bit uart1 BRM incremental reg
#define UART1_UBMR            (CSP_BASE_REG_PA_UART1+0x00A8U)	// 32bit uart1 BRM modulator reg
#define UART1_UBRC            (CSP_BASE_REG_PA_UART1+0x00ACU)	// 32bit uart1 baud rate count reg
#define UART1_ONEMS           (CSP_BASE_REG_PA_UART1+0x00B0U)	// 32bit uart1 one ms reg
#define UART1_UTS             (CSP_BASE_REG_PA_UART1+0x00B4U)	// 32bit uart1 test reg

//UART 2:
#define UART2_URXD            (CSP_BASE_REG_PA_UART2+0x0000U)
#define UART2_UTXD            (CSP_BASE_REG_PA_UART2+0x0040U)
#define UART2_UCR1            (CSP_BASE_REG_PA_UART2+0x0080U)
#define UART2_UCR2            (CSP_BASE_REG_PA_UART2+0x0084U)
#define UART2_UCR3            (CSP_BASE_REG_PA_UART2+0x0088U)
#define UART2_UCR4            (CSP_BASE_REG_PA_UART2+0x008CU)
#define UART2_UFCR            (CSP_BASE_REG_PA_UART2+0x0090U)
#define UART2_USR1            (CSP_BASE_REG_PA_UART2+0x0094U)
#define UART2_USR2            (CSP_BASE_REG_PA_UART2+0x0098U)
#define UART2_UESC            (CSP_BASE_REG_PA_UART2+0x009CU)
#define UART2_UTIM            (CSP_BASE_REG_PA_UART2+0x00A0U)
#define UART2_UBIR            (CSP_BASE_REG_PA_UART2+0x00A4U)
#define UART2_UBMR            (CSP_BASE_REG_PA_UART2+0x00A8U)
#define UART2_UBRC            (CSP_BASE_REG_PA_UART2+0x00ACU)
#define UART2_ONEMS           (CSP_BASE_REG_PA_UART2+0x00B0U)
#define UART2_UTS             (CSP_BASE_REG_PA_UART2+0x00B4U)

//UART 3:
#define UART3_URXD            (CSP_BASE_REG_PA_UART3+0x0000U)
#define UART3_UTXD            (CSP_BASE_REG_PA_UART3+0x0040U)
#define UART3_UCR1            (CSP_BASE_REG_PA_UART3+0x0080U)
#define UART3_UCR2            (CSP_BASE_REG_PA_UART3+0x0084U)
#define UART3_UCR3            (CSP_BASE_REG_PA_UART3+0x0088U)
#define UART3_UCR4            (CSP_BASE_REG_PA_UART3+0x008CU)
#define UART3_UFCR            (CSP_BASE_REG_PA_UART3+0x0090U)
#define UART3_USR1            (CSP_BASE_REG_PA_UART3+0x0094U)
#define UART3_USR2            (CSP_BASE_REG_PA_UART3+0x0098U)
#define UART3_UESC            (CSP_BASE_REG_PA_UART3+0x009CU)
#define UART3_UTIM            (CSP_BASE_REG_PA_UART3+0x00A0U)
#define UART3_UBIR            (CSP_BASE_REG_PA_UART3+0x00A4U)
#define UART3_UBMR            (CSP_BASE_REG_PA_UART3+0x00A8U)
#define UART3_UBRC            (CSP_BASE_REG_PA_UART3+0x00ACU)
#define UART3_ONEMS           (CSP_BASE_REG_PA_UART3+0x00B0U)
#define UART3_UTS             (CSP_BASE_REG_PA_UART3+0x00B4U)

//UART 4:
#define UART4_URXD            (CSP_BASE_REG_PA_UART4+0x0000U)
#define UART4_UTXD            (CSP_BASE_REG_PA_UART4+0x0040U)
#define UART4_UCR1            (CSP_BASE_REG_PA_UART4+0x0080U)
#define UART4_UCR2            (CSP_BASE_REG_PA_UART4+0x0084U)
#define UART4_UCR3            (CSP_BASE_REG_PA_UART4+0x0088U)
#define UART4_UCR4            (CSP_BASE_REG_PA_UART4+0x008CU)
#define UART4_UFCR            (CSP_BASE_REG_PA_UART4+0x0090U)
#define UART4_USR1            (CSP_BASE_REG_PA_UART4+0x0094U)
#define UART4_USR2            (CSP_BASE_REG_PA_UART4+0x0098U)
#define UART4_UESC            (CSP_BASE_REG_PA_UART4+0x009CU)
#define UART4_UTIM            (CSP_BASE_REG_PA_UART4+0x00A0U)
#define UART4_UBIR            (CSP_BASE_REG_PA_UART4+0x00A4U)
#define UART4_UBMR            (CSP_BASE_REG_PA_UART4+0x00A8U)
#define UART4_UBRC            (CSP_BASE_REG_PA_UART4+0x00ACU)
#define UART4_ONEMS           (CSP_BASE_REG_PA_UART4+0x00B0U)
#define UART4_UTS             (CSP_BASE_REG_PA_UART4+0x00B4U)

//UART 5:
#define UART5_URXD            (CSP_BASE_REG_PA_UART5+0x0000U)
#define UART5_UTXD            (CSP_BASE_REG_PA_UART5+0x0040U)
#define UART5_UCR1            (CSP_BASE_REG_PA_UART5+0x0080U)
#define UART5_UCR2            (CSP_BASE_REG_PA_UART5+0x0084U)
#define UART5_UCR3            (CSP_BASE_REG_PA_UART5+0x0088U)
#define UART5_UCR4            (CSP_BASE_REG_PA_UART5+0x008CU)
#define UART5_UFCR            (CSP_BASE_REG_PA_UART5+0x0090U)
#define UART5_USR1            (CSP_BASE_REG_PA_UART5+0x0094U)
#define UART5_USR2            (CSP_BASE_REG_PA_UART5+0x0098U)
#define UART5_UESC            (CSP_BASE_REG_PA_UART5+0x009CU)
#define UART5_UTIM            (CSP_BASE_REG_PA_UART5+0x00A0U)
#define UART5_UBIR            (CSP_BASE_REG_PA_UART5+0x00A4U)
#define UART5_UBMR            (CSP_BASE_REG_PA_UART5+0x00A8U)
#define UART5_UBRC            (CSP_BASE_REG_PA_UART5+0x00ACU)
#define UART5_ONEMS           (CSP_BASE_REG_PA_UART5+0x00B0U)
#define UART5_UTS             (CSP_BASE_REG_PA_UART5+0x00B4U)

//------------------------------------------------------------------------------
// REGISTER BIT FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------

//UART Receiver Register (URXD)
#define UART_URXD_RX_DATA_LSH       0
#define UART_URXD_PRERR_LSH         10
#define UART_URXD_BRK_LSH           11
#define UART_URXD_FRMERR_LSH        12
#define UART_URXD_OVRRUN_LSH        13
#define UART_URXD_ERR_LSH           14
#define UART_URXD_CHARRDY_LSH       15

//UART Transmitter Register (UTXD)
#define UART_UTXD_TX_DATA_LSH       0

//UART Control Register 1 (UCR1)
#define UART_UCR1_UARTEN_LSH        0
#define UART_UCR1_DOZE_LSH          1
#define UART_UCR1_ATDMAEN_LSH       2
#define UART_UCR1_TDMAEN_LSH        3
#define UART_UCR1_SNDBRK_LSH        4
#define UART_UCR1_RTSDEN_LSH        5
#define UART_UCR1_TXMPTYEN_LSH      6
#define UART_UCR1_IREN_LSH          7
#define UART_UCR1_RDMAEN_LSH        8
#define UART_UCR1_RRDYEN_LSH        9
#define UART_UCR1_ICD_LSH           10
#define UART_UCR1_IDEN_LSH          12
#define UART_UCR1_TRDYEN_LSH        13
#define UART_UCR1_ADBR_LSH          14
#define UART_UCR1_ADEN_LSH          15

//UART Control Register 2 (UCR2)
#define UART_UCR2_SRST_LSH          0
#define UART_UCR2_RXEN_LSH          1
#define UART_UCR2_TXEN_LSH          2
#define UART_UCR2_ATEN_LSH          3
#define UART_UCR2_RTSEN_LSH         4
#define UART_UCR2_WS_LSH            5
#define UART_UCR2_STPB_LSH          6
#define UART_UCR2_PROE_LSH          7
#define UART_UCR2_PREN_LSH          8
#define UART_UCR2_RTEC_LSH          9
#define UART_UCR2_ESCEN_LSH         11
#define UART_UCR2_CTS_LSH           12
#define UART_UCR2_CTSC_LSH          13
#define UART_UCR2_IRTS_LSH          14
#define UART_UCR2_ESCI_LSH          15

//UART Control Register 3 (UCR3)
#define UART_UCR3_ACIEN_LSH         0
#define UART_UCR3_INVT_LSH          1
#define UART_UCR3_RXDMUXSEL_LSH     2
#define UART_UCR3_DTRDEN_LSH	    3
#define UART_UCR3_AWAKEN_LSH        4
#define UART_UCR3_AIRINTEN_LSH      5
#define UART_UCR3_RXDSEN_LSH        6
#define UART_UCR3_ADNIMP_LSH        7
#define UART_UCR3_RI_LSH	        8
#define UART_UCR3_DCD_LSH	        9
#define UART_UCR3_DSR_LSH	        10
#define UART_UCR3_FRAERREN_LSH      11
#define UART_UCR3_PARERREN_LSH      12
#define UART_UCR3_DTREN_LSH		    13
#define UART_UCR3_DPEC_LSH		    14

//UART Control Register 4 (UCR4)
#define UART_UCR4_DREN_LSH          0
#define UART_UCR4_OREN_LSH          1
#define UART_UCR4_BKEN_LSH          2
#define UART_UCR4_TCEN_LSH          3
#define UART_UCR4_LPBYP_LSH         4
#define UART_UCR4_IRSC_LSH          5
#define UART_UCR4_IDDMAEN_LSH       6
#define UART_UCR4_WKEN_LSH          7
#define UART_UCR4_ENIRI_LSH         8
#define UART_UCR4_INVR_LSH          9
#define UART_UCR4_CTSTL_LSH         10

//UART FIFO Control Register (UFCR)
#define UART_UFCR_RXTL_LSH          0
#define UART_UFCR_DCEDTE_LSH        6
#define UART_UFCR_RFDIV_LSH         7
#define UART_UFCR_TXTL_LSH          10

//UART Status Register 1 (USR1)
#define UART_USR1_AWAKE_LSH         4
#define UART_USR1_AIRINT_LSH        5
#define UART_USR1_RXDS_LSH          6
#define UART_USR1_DTRD_LSH          7
#define UART_USR1_AGTIM_LSH         8
#define UART_USR1_RRDY_LSH          9
#define UART_USR1_FRAMERR_LSH       10
#define UART_USR1_ESCF_LSH          11
#define UART_USR1_RTSD_LSH          12
#define UART_USR1_TRDY_LSH          13
#define UART_USR1_RTSS_LSH          14
#define UART_USR1_PARITYERR_LSH     15

//UART Status Register 2 (USR2)
#define UART_USR2_RDR_LSH           0
#define UART_USR2_ORE_LSH           1
#define UART_USR2_BRCD_LSH          2
#define UART_USR2_TXDC_LSH          3
#define UART_USR2_RTSF_LSH          4
#define UART_USR2_DCDIN_LSH         5
#define UART_USR2_DCDDELT_LSH       6
#define UART_USR2_WAKE_LSH          7
#define UART_USR2_IRINT_LSH         8
#define UART_USR2_RIIN_LSH          9
#define UART_USR2_RIDELT_LSH        10
#define UART_USR2_ACST_LSH          11
#define UART_USR2_IDLE_LSH          12
#define UART_USR2_DTRF_LSH          13
#define UART_USR2_TXFE_LSH          14
#define UART_USR2_ADET_LSH          15

//UART Escape Character Register (UESC)
#define UART_UESC_ESC_CHAR_LSH      0

//UART Escape Timer Register (UTIM)
#define UART_UTIM_TIM_LSH           0

//UART BRM Incremental Register (UBIR)
#define UART_UBIR_INC_LSH           0

//UART BRM Modulator Register (UBMR)
#define UART_UBMR_MOD_LSH           0

//UART Baud Rate Count Register (UBRC)
#define UART_UBRC_BCNT_LSH          0

//UART One Millisecond Register (ONEMS)
#define UART_ONEMS_ONEMS_LSH        0

//UART Test Register (UTS)
#define UART_UTS_SOFTRST_LSH        0
#define UART_UTS_RXFULL_LSH         3
#define UART_UTS_TXFULL_LSH         4
#define UART_UTS_RXEMPTY_LSH        5
#define UART_UTS_TXEMPTY_LSH        6
#define UART_UTS_RXDBG_LSH          9
#define UART_UTS_LOOPIR_LSH         10
#define UART_UTS_DBGEN_LSH          11
#define UART_UTS_LOOP_LSH           12
#define UART_UTS_FRCPERR_LSH        13



//------------------------------------------------------------------------------
// REGISTER BIT FIELD WIDTHS
//------------------------------------------------------------------------------
//UART Receiver Register (URXD)
#define UART_URXD_RX_DATA_WID       Width_8
#define UART_URXD_PRERR_WID         Width_1
#define UART_URXD_BRK_WID           Width_1
#define UART_URXD_FRMERR_WID        Width_1
#define UART_URXD_OVRRUN_WID        Width_1
#define UART_URXD_ERR_WID           Width_1
#define UART_URXD_CHARRDY_WID       Width_1

//UART Transmitter Register (UTXD)
#define UART_UTXD_TX_DATA_WID       Width_8

//UART Control Register 1 (UCR1)
#define UART_UCR1_UARTEN_WID        Width_1
#define UART_UCR1_DOZE_WID          Width_1
#define UART_UCR1_ATDMAEN_WID       Width_1
#define UART_UCR1_TDMAEN_WID        Width_1
#define UART_UCR1_SNDBRK_WID        Width_1
#define UART_UCR1_RTSDEN_WID        Width_1
#define UART_UCR1_TXMPTYEN_WID      Width_1
#define UART_UCR1_IREN_WID          Width_1
#define UART_UCR1_RDMAEN_WID        Width_1
#define UART_UCR1_RRDYEN_WID        Width_1
#define UART_UCR1_ICD_WID           Width_2
#define UART_UCR1_IDEN_WID          Width_1
#define UART_UCR1_TRDYEN_WID        Width_1
#define UART_UCR1_ADBR_WID          Width_1
#define UART_UCR1_ADEN_WID          Width_1

//UART Control Register 2 (UCR2)
#define UART_UCR2_SRST_WID          Width_1
#define UART_UCR2_RXEN_WID          Width_1
#define UART_UCR2_TXEN_WID          Width_1
#define UART_UCR2_ATEN_WID          Width_1
#define UART_UCR2_RTSEN_WID         Width_1
#define UART_UCR2_WS_WID            Width_1
#define UART_UCR2_STPB_WID          Width_1
#define UART_UCR2_PROE_WID          Width_1
#define UART_UCR2_PREN_WID          Width_1
#define UART_UCR2_RTEC_WID          Width_2
#define UART_UCR2_ESCEN_WID         Width_1
#define UART_UCR2_CTS_WID           Width_1
#define UART_UCR2_CTSC_WID          Width_1
#define UART_UCR2_IRTS_WID          Width_1
#define UART_UCR2_ESCI_WID          Width_1

//UART Control Register 3 (UCR3)
#define UART_UCR3_ACIEN_WID         Width_1
#define UART_UCR3_INVT_WID          Width_1
#define UART_UCR3_RXDMUXSEL_WID     Width_1
#define UART_UCR3_DTRDEN_WID	    Width_1
#define UART_UCR3_AWAKEN_WID        Width_1
#define UART_UCR3_AIRINTEN_WID      Width_1
#define UART_UCR3_RXDSEN_WID        Width_1
#define UART_UCR3_ADNIMP_WID        Width_1
#define UART_UCR3_RI_WID	        Width_1
#define UART_UCR3_DCD_WID	        Width_1
#define UART_UCR3_DSR_WID	        Width_1
#define UART_UCR3_FRAERREN_WID      Width_1
#define UART_UCR3_PARERREN_WID      Width_1
#define UART_UCR3_DTREN_WID		    Width_1
#define UART_UCR3_DPEC_WID		    Width_2

//UART Control Register 4 (UCR4)
#define UART_UCR4_DREN_WID          Width_1
#define UART_UCR4_OREN_WID          Width_1
#define UART_UCR4_BKEN_WID          Width_1
#define UART_UCR4_TCEN_WID          Width_1
#define UART_UCR4_LPBYP_WID         Width_1
#define UART_UCR4_IRSC_WID          Width_1
#define UART_UCR4_IDDMAEN_WID       Width_1
#define UART_UCR4_WKEN_WID          Width_1
#define UART_UCR4_ENIRI_WID         Width_1
#define UART_UCR4_INVR_WID          Width_1
#define UART_UCR4_CTSTL_WID         Width_6

//UART FIFO Control Register (UFCR)
#define UART_UFCR_RXTL_WID          Width_6
#define UART_UFCR_DCEDTE_WID        Width_1
#define UART_UFCR_RFDIV_WID         Width_3
#define UART_UFCR_TXTL_WID          Width_6

//UART Status Register 1 (USR1)
#define UART_USR1_AWAKE_WID         Width_1
#define UART_USR1_AIRINT_WID        Width_1
#define UART_USR1_RXDS_WID          Width_1
#define UART_USR1_DTRD_WID          Width_1
#define UART_USR1_AGTIM_WID         Width_1
#define UART_USR1_RRDY_WID          Width_1
#define UART_USR1_FRAMERR_WID       Width_1
#define UART_USR1_ESCF_WID          Width_1
#define UART_USR1_RTSD_WID          Width_1
#define UART_USR1_TRDY_WID          Width_1
#define UART_USR1_RTSS_WID          Width_1
#define UART_USR1_PARITYERR_WID     Width_1

//UART Status Register 2 (USR2)
#define UART_USR2_RDR_WID           Width_1
#define UART_USR2_ORE_WID           Width_1
#define UART_USR2_BRCD_WID          Width_1
#define UART_USR2_TXDC_WID          Width_1
#define UART_USR2_RTSF_WID          Width_1
#define UART_USR2_DCDIN_WID         Width_1
#define UART_USR2_DCDDELT_WID       Width_1
#define UART_USR2_WAKE_WID          Width_1
#define UART_USR2_IRINT_WID         Width_1
#define UART_USR2_RIIN_WID          Width_1
#define UART_USR2_RIDELT_WID        Width_1
#define UART_USR2_ACST_WID          Width_1
#define UART_USR2_IDLE_WID          Width_1
#define UART_USR2_DTRF_WID          Width_1
#define UART_USR2_TXFE_WID          Width_1
#define UART_USR2_ADET_WID          Width_1

//UART Escape Character Register (UESC)
#define UART_UESC_ESC_CHAR_WID      Width_8

//UART Escape Timer Register (UTIM)
#define UART_UTIM_TIM_WID           Width_12

//UART BRM Incremental Register (UBIR)
#define UART_UBIR_INC_WID           Width_16

//UART BRM Modulator Register (UBMR)
#define UART_UBMR_MOD_WID           Width_16

//UART Baud Rate Count Register (UBRC)
#define UART_UBRC_BCNT_WID          Width_16

//UART One Millisecond Register (ONEMS)
#define UART_ONEMS_ONEMS_WID        Width_16

//UART Test Register (UTS)
#define UART_UTS_SOFTRST_WID        Width_1
#define UART_UTS_RXFULL_WID         Width_1
#define UART_UTS_TXFULL_WID         Width_1
#define UART_UTS_RXEMPTY_WID        Width_1
#define UART_UTS_TXEMPTY_WID        Width_1
#define UART_UTS_RXDBG_WID          Width_1
#define UART_UTS_LOOPIR_WID         Width_1
#define UART_UTS_DBGEN_WID          Width_1
#define UART_UTS_LOOP_WID           Width_1
#define UART_UTS_FRCPERR_WID        Width_1


//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------

// URXD
#define UART_URXD_PRERR_NOERROR     0
#define UART_URXD_PRERR_ERROR       1

#define UART_URXD_BRK_NOBREAK       0
#define UART_URXD_BRK_BREAK         1

#define UART_URXD_FRMERR_NOERROR    0
#define UART_URXD_FRMERR_ERROR      1

#define UART_URXD_OVRRUN_NOOVRRUN   0
#define UART_URXD_OVRRUN_OVRRUN     1

#define UART_URXD_ERR_NOERROR       0
#define UART_URXD_ERR_ERROR         1


// UCR1
#define UART_UCR1_UARTEN_DISABLE    0
#define UART_UCR1_UARTEN_ENABLE     1

#define UART_UCR1_DOZE_ENABLE       0
#define UART_UCR1_DOZE_DISABLE      1

#define UART_UCR1_TXDMAEN_DISABLE   0
#define UART_UCR1_TXDMAEN_ENABLE    1

#define UART_UCR1_SNDBRK_NOBREAK    0
#define UART_UCR1_SNDBRK_BREAK      1

#define UART_UCR1_RTSDEN_DISABLE    0
#define UART_UCR1_RTSDEN_ENABLE     1

#define UART_UCR1_TXMPTYEN_DISABLE  0
#define UART_UCR1_TXMPTYEN_ENABLE   1

#define UART_UCR1_TXMPTYEN_DISABLE  0
#define UART_UCR1_TXMPTYEN_ENABLE   1

#define UART_UCR1_IREN_DISABLE      0
#define UART_UCR1_IREN_ENABLE       1

#define UART_UCR1_RXDMAEN_DISABLE   0
#define UART_UCR1_RXDMAEN_ENABLE    1

#define UART_UCR1_RRDYEN_DISABLE    0
#define UART_UCR1_RRDYEN_ENABLE     1

#define UART_UCR1_ICD_4FRAMES       0
#define UART_UCR1_ICD_8FRAMES       1
#define UART_UCR1_ICD_16FRAMES      2
#define UART_UCR1_ICD_32FRAMES      3

#define UART_UCR1_IDEN_DISABLE      0
#define UART_UCR1_IDEN_ENABLE       1

#define UART_UCR1_TRDYEN_DISABLE    0
#define UART_UCR1_TRDYEN_ENABLE     1

#define UART_UCR1_ADBR_DISABLE      0
#define UART_UCR1_ADBR_ENABLE       1

#define UART_UCR1_ADEN_DISABLE      0
#define UART_UCR1_ADEN_ENABLE       1


// UCR2
#define UART_UCR2_SRST_RESET        0
#define UART_UCR2_SRST_NORESET      1

#define UART_UCR2_RXEN_DISABLE      0
#define UART_UCR2_RXEN_ENABLE       1

#define UART_UCR2_TXEN_DISABLE      0
#define UART_UCR2_TXEN_ENABLE       1

#define UART_UCR2_ATEN_DISABLE      0
#define UART_UCR2_ATEN_ENABLE       1

#define UART_UCR2_RTSEN_DISABLE     0
#define UART_UCR2_RTSEN_ENABLE      1

#define UART_UCR2_WS_7BIT           0
#define UART_UCR2_WS_8BIT           1

#define UART_UCR2_STPB_1STOP        0
#define UART_UCR2_STPB_2STOP        1

#define UART_UCR2_PROE_EVEN         0
#define UART_UCR2_PROE_ODD          1

#define UART_UCR2_PREN_DISBLE       0
#define UART_UCR2_PREN_ENABLE       1

#define UART_UCR2_RTEC_RISEDGE      0
#define UART_UCR2_RTEC_FALLEDGE     1
#define UART_UCR2_RTEC_ANYEDGE      2

#define UART_UCR2_ESCEN_DISABLE     0
#define UART_UCR2_ESCEN_ENABLE      1

#define UART_UCR2_CTS_HIGH          0
#define UART_UCR2_CTS_LOW           1

#define UART_UCR2_CTSC_BITCTRL      0
#define UART_UCR2_CTSC_RXCTRL       1

#define UART_UCR2_IRTS_USERTS       0
#define UART_UCR2_IRTS_IGNORERTS    1

#define UART_UCR2_ESCI_DISABLE      0
#define UART_UCR2_ESCI_ENABLE       1


// UCR3
#define UART_UCR3_ACIEN_DISABLE     0
#define UART_UCR3_ACIEN_ENABLE      1

#define UART_UCR3_INVT_ACTIVELOW    0
#define UART_UCR3_INVT_ACTIVEHIGH   1

#define UART_UCR3_RXDMUXSEL_NOTMUX  0
#define UART_UCR3_RXDMUXSEL_MUX     1

#define UART_UCR3_AWAKEN_DISABLE    0
#define UART_UCR3_AWAKEN_ENABLE     1

#define UART_UCR3_AIRINTEN_DISABLE  0
#define UART_UCR3_AIRINTEN_ENABLE   1

#define UART_UCR3_RXDSEN_DISABLE    0
#define UART_UCR3_RXDSEN_ENABLE     1

#define UART_UCR3_ADNIMP_NEW        0
#define UART_UCR3_ADNIMP_OLD        1

#define UART_UCR3_FRAERREN_DISABLE  0
#define UART_UCR3_FRAERREN_ENABLE   1

#define UART_UCR3_PARERREN_DISABLE  0
#define UART_UCR3_PARERREN_ENABLE   1


// UCR4
#define UART_UCR4_DREN_DISABLE      0
#define UART_UCR4_DREN_ENABLE       1

#define UART_UCR4_OREN_DISABLE      0
#define UART_UCR4_OREN_ENABLE       1

#define UART_UCR4_BKEN_DISABLE      0
#define UART_UCR4_BKEN_ENABLE       1

#define UART_UCR4_TCEN_DISABLE      0
#define UART_UCR4_TCEN_ENABLE       1

#define UART_UCR4_LPBYP_DISABLE     0
#define UART_UCR4_LPBYP_ENABLE      1

#define UART_UCR4_IRSC_SAMPCLK      0
#define UART_UCR4_IRSC_REFCLK       1

#define UART_UCR4_WKEN_DISABLE      0
#define UART_UCR4_WKEN_ENABLE       1

#define UART_UCR4_ENIRI_DISABLE     0
#define UART_UCR4_ENIRI_ENABLE      1

#define UART_UCR4_INVR_ACTIVELOW    0
#define UART_UCR4_INVR_ACTIVEHIGH   1


// UFCR
#define UART_UFCR_DCEDTE_DCE        0
#define UART_UFCR_DCEDTE_DTE        1

#define UART_UFCR_RFDIV_DIV6        0
#define UART_UFCR_RFDIV_DIV5        1
#define UART_UFCR_RFDIV_DIV4        2
#define UART_UFCR_RFDIV_DIV3        3
#define UART_UFCR_RFDIV_DIV2        4
#define UART_UFCR_RFDIV_DIV1        5
#define UART_UFCR_RFDIV_DIV7        6


// USR1
#define UART_USR1_AWAKE_SET         1
#define UART_USR1_AIRINT_SET        1
#define UART_USR1_RXDS_SET          1
#define UART_USR1_DTRD_SET          1
#define UART_USR1_AGTIM_SET         1
#define UART_USR1_RRDY_SET          1
#define UART_USR1_FRAMERR_SET       1
#define UART_USR1_ESCF_SET          1
#define UART_USR1_RTSD_SET          1
#define UART_USR1_TRDY_SET          1
#define UART_USR1_RTSS_SET          1
#define UART_USR1_PARITYERR_SET     1

// USR2
#define UART_USR2_RDR_SET           1
#define UART_USR2_ORE_SET           1
#define UART_USR2_BRCD_SET          1
#define UART_USR2_TXDC_SET          1
#define UART_USR2_RTSF_SET          1
#define UART_USR2_DCDIN_SET         1
#define UART_USR2_DCDDELT_SET       1
#define UART_USR2_WAKE_SET          1
#define UART_USR2_IRINT_SET         1
#define UART_USR2_RIIN_SET          1
#define UART_USR2_RIDELT_SET        1
#define UART_USR2_ACST_SET          1
#define UART_USR2_IDLE_SET          1
#define UART_USR2_DTRF_SET          1
#define UART_USR2_TXFE_SET          1
#define UART_USR2_ADET_SET          1

// UTS
#define UART_UTS_SOFTRST_INACTIVE	0
#define UART_UTS_SOFTRST_ACTIVE		1

#define UART_UTS_RXFULL_NOTFULL		0
#define UART_UTS_RXFULL_FULL		1

#define UART_UTS_TXFULL_NOTFUL		0
#define UART_UTS_TXFULL_FULL		1

#define UART_UTS_RXEMPTY_NOTEMPTY	0
#define UART_UTS_RXEMPTY_EMPTY		1

#define UART_UTS_TXEMPTY_NOTEMPTY	0
#define UART_UTS_TXEMPTY_EMPTY		1

#define UART_UTS_RXDBG_NOINCREMENT  0
#define UART_UTS_RXDBG_INCREMENT    1

#define UART_UTS_LOOPIR_NOLOOP      0
#define UART_UTS_LOOPIR_LOOP        1

#define UART_UTS_DBGEN_DEBUG        0
#define UART_UTS_DBGEN_NODEBUG      1

#define UART_UTS_LOOP_NOLOOP        0
#define UART_UTS_LOOP_LOOP          1

#define UART_UTS_FRCPERR_NOERROR    0
#define UART_UTS_FRCPERR_ERROR      1


#endif /*IMX25_UART_H_*/


