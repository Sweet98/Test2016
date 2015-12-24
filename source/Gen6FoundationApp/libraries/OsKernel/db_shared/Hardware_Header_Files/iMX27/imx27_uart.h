/**************************************************************************************/
//
// HIM 48X0G GENERATION 6 SOFTWARE
//
//  HARDWARE DEFINITION FILE 
//  Date:       5/09/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX27
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: db_shared/Hardware_Header_Files/iMX27/imx27_uart.h $
// $Revision: 1.1 $
// $Date: 2008/09/22 16:35:11EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx27_uart.h
//
// Provides definitions for the 6 general-purpose timers (GPT)
// Refer to Chapter 28 Page 892 of the iMX27 Reference Manual
//
//-----------------------------------------------------------------------------

#ifndef IMX27_UART_H_
#define IMX27_UART_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width

//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------
#define CSP_BASE_REG_PA_UART1           (0x1000A000)
#define CSP_BASE_REG_PA_UART2           (0x1000B000)
#define CSP_BASE_REG_PA_UART3           (0x1000C000)
#define CSP_BASE_REG_PA_UART4           (0x1000D000)
#define CSP_BASE_REG_PA_UART5           (0x1001B000)
#define CSP_BASE_REG_PA_UART6           (0x1001C000)

//------------------------------------------------------------------------------
// REGISTER OFFSETS
//------------------------------------------------------------------------------
//UART 1:
#define UART1_URXD            (CSP_BASE_REG_PA_UART1+0x0000)
#define UART1_UTXD            (CSP_BASE_REG_PA_UART1+0x0040)
#define UART1_UCR1            (CSP_BASE_REG_PA_UART1+0x0080)
#define UART1_UCR2            (CSP_BASE_REG_PA_UART1+0x0084)
#define UART1_UCR3            (CSP_BASE_REG_PA_UART1+0x0088)
#define UART1_UCR4            (CSP_BASE_REG_PA_UART1+0x008C)
#define UART1_UFCR            (CSP_BASE_REG_PA_UART1+0x0090)
#define UART1_USR1            (CSP_BASE_REG_PA_UART1+0x0094)
#define UART1_USR2            (CSP_BASE_REG_PA_UART1+0x0098)
#define UART1_UESC            (CSP_BASE_REG_PA_UART1+0x009C)
#define UART1_UTIM            (CSP_BASE_REG_PA_UART1+0x00A0)
#define UART1_UBIR            (CSP_BASE_REG_PA_UART1+0x00A4)
#define UART1_UBMR            (CSP_BASE_REG_PA_UART1+0x00A8)
#define UART1_UBRC            (CSP_BASE_REG_PA_UART1+0x00AC)
#define UART1_ONEMS           (CSP_BASE_REG_PA_UART1+0x00B0)
#define UART1_UTS             (CSP_BASE_REG_PA_UART1+0x00B4)

//UART 2:
#define UART2_URXD            (CSP_BASE_REG_PA_UART2+0x0000)
#define UART2_UTXD            (CSP_BASE_REG_PA_UART2+0x0040)
#define UART2_UCR1            (CSP_BASE_REG_PA_UART2+0x0080)
#define UART2_UCR2            (CSP_BASE_REG_PA_UART2+0x0084)
#define UART2_UCR3            (CSP_BASE_REG_PA_UART2+0x0088)
#define UART2_UCR4            (CSP_BASE_REG_PA_UART2+0x008C)
#define UART2_UFCR            (CSP_BASE_REG_PA_UART2+0x0090)
#define UART2_USR1            (CSP_BASE_REG_PA_UART2+0x0094)
#define UART2_USR2            (CSP_BASE_REG_PA_UART2+0x0098)
#define UART2_UESC            (CSP_BASE_REG_PA_UART2+0x009C)
#define UART2_UTIM            (CSP_BASE_REG_PA_UART2+0x00A0)
#define UART2_UBIR            (CSP_BASE_REG_PA_UART2+0x00A4)
#define UART2_UBMR            (CSP_BASE_REG_PA_UART2+0x00A8)
#define UART2_UBRC            (CSP_BASE_REG_PA_UART2+0x00AC)
#define UART2_ONEMS           (CSP_BASE_REG_PA_UART2+0x00B0)
#define UART2_UTS             (CSP_BASE_REG_PA_UART2+0x00B4)

//UART 3:
#define UART3_URXD            (CSP_BASE_REG_PA_UART3+0x0000)
#define UART3_UTXD            (CSP_BASE_REG_PA_UART3+0x0040)
#define UART3_UCR1            (CSP_BASE_REG_PA_UART3+0x0080)
#define UART3_UCR2            (CSP_BASE_REG_PA_UART3+0x0084)
#define UART3_UCR3            (CSP_BASE_REG_PA_UART3+0x0088)
#define UART3_UCR4            (CSP_BASE_REG_PA_UART3+0x008C)
#define UART3_UFCR            (CSP_BASE_REG_PA_UART3+0x0090)
#define UART3_USR1            (CSP_BASE_REG_PA_UART3+0x0094)
#define UART3_USR2            (CSP_BASE_REG_PA_UART3+0x0098)
#define UART3_UESC            (CSP_BASE_REG_PA_UART3+0x009C)
#define UART3_UTIM            (CSP_BASE_REG_PA_UART3+0x00A0)
#define UART3_UBIR            (CSP_BASE_REG_PA_UART3+0x00A4)
#define UART3_UBMR            (CSP_BASE_REG_PA_UART3+0x00A8)
#define UART3_UBRC            (CSP_BASE_REG_PA_UART3+0x00AC)
#define UART3_ONEMS           (CSP_BASE_REG_PA_UART3+0x00B0)
#define UART3_UTS             (CSP_BASE_REG_PA_UART3+0x00B4)

//UART 4:
#define UART4_URXD            (CSP_BASE_REG_PA_UART4+0x0000)
#define UART4_UTXD            (CSP_BASE_REG_PA_UART4+0x0040)
#define UART4_UCR1            (CSP_BASE_REG_PA_UART4+0x0080)
#define UART4_UCR2            (CSP_BASE_REG_PA_UART4+0x0084)
#define UART4_UCR3            (CSP_BASE_REG_PA_UART4+0x0088)
#define UART4_UCR4            (CSP_BASE_REG_PA_UART4+0x008C)
#define UART4_UFCR            (CSP_BASE_REG_PA_UART4+0x0090)
#define UART4_USR1            (CSP_BASE_REG_PA_UART4+0x0094)
#define UART4_USR2            (CSP_BASE_REG_PA_UART4+0x0098)
#define UART4_UESC            (CSP_BASE_REG_PA_UART4+0x009C)
#define UART4_UTIM            (CSP_BASE_REG_PA_UART4+0x00A0)
#define UART4_UBIR            (CSP_BASE_REG_PA_UART4+0x00A4)
#define UART4_UBMR            (CSP_BASE_REG_PA_UART4+0x00A8)
#define UART4_UBRC            (CSP_BASE_REG_PA_UART4+0x00AC)
#define UART4_ONEMS           (CSP_BASE_REG_PA_UART4+0x00B0)
#define UART4_UTS             (CSP_BASE_REG_PA_UART4+0x00B4)

//UART 5:
#define UART5_URXD            (CSP_BASE_REG_PA_UART5+0x0000)
#define UART5_UTXD            (CSP_BASE_REG_PA_UART5+0x0040)
#define UART5_UCR1            (CSP_BASE_REG_PA_UART5+0x0080)
#define UART5_UCR2            (CSP_BASE_REG_PA_UART5+0x0084)
#define UART5_UCR3            (CSP_BASE_REG_PA_UART5+0x0088)
#define UART5_UCR4            (CSP_BASE_REG_PA_UART5+0x008C)
#define UART5_UFCR            (CSP_BASE_REG_PA_UART5+0x0090)
#define UART5_USR1            (CSP_BASE_REG_PA_UART5+0x0094)
#define UART5_USR2            (CSP_BASE_REG_PA_UART5+0x0098)
#define UART5_UESC            (CSP_BASE_REG_PA_UART5+0x009C)
#define UART5_UTIM            (CSP_BASE_REG_PA_UART5+0x00A0)
#define UART5_UBIR            (CSP_BASE_REG_PA_UART5+0x00A4)
#define UART5_UBMR            (CSP_BASE_REG_PA_UART5+0x00A8)
#define UART5_UBRC            (CSP_BASE_REG_PA_UART5+0x00AC)
#define UART5_ONEMS           (CSP_BASE_REG_PA_UART5+0x00B0)
#define UART5_UTS             (CSP_BASE_REG_PA_UART5+0x00B4)

//UART 6:
#define UART6_URXD            (CSP_BASE_REG_PA_UART6+0x0000)
#define UART6_UTXD            (CSP_BASE_REG_PA_UART6+0x0040)
#define UART6_UCR1            (CSP_BASE_REG_PA_UART6+0x0080)
#define UART6_UCR2            (CSP_BASE_REG_PA_UART6+0x0084)
#define UART6_UCR3            (CSP_BASE_REG_PA_UART6+0x0088)
#define UART6_UCR4            (CSP_BASE_REG_PA_UART6+0x008C)
#define UART6_UFCR            (CSP_BASE_REG_PA_UART6+0x0090)
#define UART6_USR1            (CSP_BASE_REG_PA_UART6+0x0094)
#define UART6_USR2            (CSP_BASE_REG_PA_UART6+0x0098)
#define UART6_UESC            (CSP_BASE_REG_PA_UART6+0x009C)
#define UART6_UTIM            (CSP_BASE_REG_PA_UART6+0x00A0)
#define UART6_UBIR            (CSP_BASE_REG_PA_UART6+0x00A4)
#define UART6_UBMR            (CSP_BASE_REG_PA_UART6+0x00A8)
#define UART6_UBRC            (CSP_BASE_REG_PA_UART6+0x00AC)
#define UART6_ONEMS           (CSP_BASE_REG_PA_UART6+0x00B0)
#define UART6_UTS             (CSP_BASE_REG_PA_UART6+0x00B4)


//------------------------------------------------------------------------------
// REGISTER BIT FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
#define UART_URXD_RX_DATA_LSH       0
#define UART_URXD_PRERR_LSH         10
#define UART_URXD_BRK_LSH           11
#define UART_URXD_FRMERR_LSH        12
#define UART_URXD_OVRRUN_LSH        13
#define UART_URXD_ERR_LSH           14

#define UART_UTXD_TX_DATA_LSH       0

#define UART_UCR1_UARTEN_LSH        0
#define UART_UCR1_DOZE_LSH          1
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

#define UART_UCR3_ACIEN_LSH         0
#define UART_UCR3_INVT_LSH          1
#define UART_UCR3_RXDMUXSEL_LSH     2
#define UART_UCR3_AWAKEN_LSH        4
#define UART_UCR3_AIRINTEN_LSH      5
#define UART_UCR3_RXDSEN_LSH        6
#define UART_UCR3_ADNIMP_LSH        7
#define UART_UCR3_FRAERREN_LSH      11
#define UART_UCR3_PARERREN_LSH      12

#define UART_UCR4_DREN_LSH          0
#define UART_UCR4_OREN_LSH          1
#define UART_UCR4_BKEN_LSH          2
#define UART_UCR4_TCEN_LSH          3
#define UART_UCR4_LPBYP_LSH         4
#define UART_UCR4_IRSC_LSH          5
#define UART_UCR4_WKEN_LSH          7
#define UART_UCR4_ENIRI_LSH         8
#define UART_UCR4_INVR_LSH          9
#define UART_UCR4_CTSTL_LSH         10

#define UART_UFCR_RXTL_LSH          0
#define UART_UFCR_DCEDTE_LSH        6
#define UART_UFCR_RFDIV_LSH         7
#define UART_UFCR_TXTL_LSH          10

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

#define UART_UESC_ESC_CHAR_LSH      0

#define UART_UTIM_TIM_LSH           0

#define UART_UBIR_INC_LSH           0

#define UART_UBMR_MOD_LSH           0

#define UART_UBRC_BCNT_LSH          0

#define UART_ONEMS_ONEMS_LSH        0

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
#define UART_URXD_RX_DATA_WID       Width_8
#define UART_URXD_PRERR_WID         Width_1
#define UART_URXD_BRK_WID           Width_1
#define UART_URXD_FRMERR_WID        Width_1
#define UART_URXD_OVRRUN_WID        Width_1
#define UART_URXD_ERR_WID           Width_1

#define UART_UTXD_TX_DATA_WID       Width_8

#define UART_UCR1_UARTEN_WID        Width_1
#define UART_UCR1_DOZE_WID          Width_1
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

#define UART_UCR3_ACIEN_WID         Width_1
#define UART_UCR3_INVT_WID          Width_1
#define UART_UCR3_RXDMUXSEL_WID     Width_1
#define UART_UCR3_AWAKEN_WID        Width_1
#define UART_UCR3_AIRINTEN_WID      Width_1
#define UART_UCR3_RXDSEN_WID        Width_1
#define UART_UCR3_ADNIMP_WID        Width_1
#define UART_UCR3_FRAERREN_WID      Width_1
#define UART_UCR3_PARERREN_WID      Width_1

#define UART_UCR4_DREN_WID          Width_1
#define UART_UCR4_OREN_WID          Width_1
#define UART_UCR4_BKEN_WID          Width_1
#define UART_UCR4_TCEN_WID          Width_1
#define UART_UCR4_LPBYP_WID         Width_1
#define UART_UCR4_IRSC_WID          Width_1
#define UART_UCR4_WKEN_WID          Width_1
#define UART_UCR4_ENIRI_WID         Width_1
#define UART_UCR4_INVR_WID          Width_1
#define UART_UCR4_CTSTL_WID         Width_6

#define UART_UFCR_RXTL_WID          Width_6
#define UART_UFCR_DCEDTE_WID        Width_1
#define UART_UFCR_RFDIV_WID         Width_3
#define UART_UFCR_TXTL_WID          Width_6

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

#define UART_UESC_ESC_CHAR_WID      Width_8

#define UART_UTIM_TIM_WID           Width_12

#define UART_UBIR_INC_WID           Width_16

#define UART_UBMR_MOD_WID           Width_16

#define UART_UBRC_BCNT_WID          Width_16

#define UART_ONEMS_ONEMS_WID        Width_16

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


#endif /*IMX27_UART_H_*/


