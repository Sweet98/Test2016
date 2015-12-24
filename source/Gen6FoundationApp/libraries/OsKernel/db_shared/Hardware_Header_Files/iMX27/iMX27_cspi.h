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
// $RCSfile: db_shared/Hardware_Header_Files/iMX27/iMX27_cspi.h $
// $Revision: 1.1 $
// $Date: 2008/09/22 16:35:06EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx27_cspi.h
//
// Provides definitions for the Configurable Serial Peripheral Interface (CSPI)
// Refer to Chapter 23 Page 768 of the iMX27 Reference Manual
//
//------------------------------------------------------------------------------
#ifndef IMX27_CSPI_H_
#define IMX27_CSPI_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------
// #########################################
// # CSPI1                                 #
// # $1000_E000 to $1000_EFFF              #
// #########################################
#define CSPI1_BASE_ADDR          0x1000E000
#define CSPI1_RXDATAREG1         (CSPI1_BASE_ADDR+0x00)     // 32bit cspi1 receive data reg
#define CSPI1_TXDATAREG1         (CSPI1_BASE_ADDR+0x04)     // 32bit cspi1 transmit data reg
#define CSPI1_CONTROLREG1        (CSPI1_BASE_ADDR+0x08)     // 32bit cspi1 control reg
#define CSPI1_INTREG1            (CSPI1_BASE_ADDR+0x0C)     // 32bit cspi1 interrupt stat/ctr reg
#define CSPI1_TESTREG1           (CSPI1_BASE_ADDR+0x10)     // 32bit cspi1 test reg
#define CSPI1_PERIODREG1         (CSPI1_BASE_ADDR+0x14)     // 32bit cspi1 sample period ctrl reg
#define CSPI1_DMAREG1            (CSPI1_BASE_ADDR+0x18)     // 32bit cspi1 dma ctrl reg
#define CSPI1_RESETREG1          (CSPI1_BASE_ADDR+0x1C)     // 32bit cspi1 soft reset reg

// #########################################
// # CSPI2                                 #
// # $1000_F000 to $1000_FFFF              #
// #########################################
#define CSPI2_BASE_ADDR          0x1000F000
#define CSPI2_RXDATAREG1         (CSPI2_BASE_ADDR+0x00)     // 32bit cspi2 receive data reg
#define CSPI2_TXDATAREG1         (CSPI2_BASE_ADDR+0x04)     // 32bit cspi2 transmit data reg
#define CSPI2_CONTROLREG1        (CSPI2_BASE_ADDR+0x08)     // 32bit cspi2 control reg
#define CSPI2_INTREG1            (CSPI2_BASE_ADDR+0x0C)     // 32bit cspi2 interrupt stat/ctr reg
#define CSPI2_TESTREG1           (CSPI2_BASE_ADDR+0x10)     // 32bit cspi2 test reg
#define CSPI2_PERIODREG1         (CSPI2_BASE_ADDR+0x14)     // 32bit cspi2 sample period ctrl reg
#define CSPI2_DMAREG1            (CSPI2_BASE_ADDR+0x18)     // 32bit cspi2 dma ctrl reg
#define CSPI2_RESETREG1          (CSPI2_BASE_ADDR+0x1C)     // 32bit cspi2 soft reset reg

// #########################################
// # CSPI3                                 #
// # $1001_7000 to $1001_7FFF              #
// #########################################
#define CSPI3_BASE_ADDR          0x10017000
#define CSPI3_RXDATAREG1         (CSPI3_BASE_ADDR+0x00)     // 32bit cspi3 receive data reg
#define CSPI3_TXDATAREG1         (CSPI3_BASE_ADDR+0x04)     // 32bit cspi3 transmit data reg
#define CSPI3_CONTROLREG1        (CSPI3_BASE_ADDR+0x08)     // 32bit cspi3 control reg
#define CSPI3_INTREG1            (CSPI3_BASE_ADDR+0x0C)     // 32bit cspi3 interrupt stat/ctr reg
#define CSPI3_TESTREG1           (CSPI3_BASE_ADDR+0x10)     // 32bit cspi3 test reg
#define CSPI3_PERIODREG1         (CSPI3_BASE_ADDR+0x14)     // 32bit cspi3 sample period ctrl reg
#define CSPI3_DMAREG1            (CSPI3_BASE_ADDR+0x18)     // 32bit cspi3 dma ctrl reg
#define CSPI3_RESETREG1          (CSPI3_BASE_ADDR+0x1C)     // 32bit cspi3 soft reset reg


//------------------------------------------------------------------------------
// REGISTER FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
#define CSPI_RXDATA_LSH                     0

#define CSPI_TXDATA_LSH                     0

#define CSPI_CONTROLREG_BITCOUNT_LSH        0
#define CSPI_CONTROLREG_POL_LSH             5
#define CSPI_CONTROLREG_PHA_LSH             6
#define CSPI_CONTROLREG_SSCTL_LSH           7
#define CSPI_CONTROLREG_SSPOL_LSH           8
#define CSPI_CONTROLREG_XCH_LSH             9
#define CSPI_CONTROLREG_SPIEN_LSH           10
#define CSPI_CONTROLREG_MODE_LSH            11
#define CSPI_CONTROLREG_DRCTL_LSH           12
#define CSPI_CONTROLREG_DATARATE_LSH        14
#define CSPI_CONTROLREG_CS_LSH              19
#define CSPI_CONTROLREG_SWAP_LSH            21
#define CSPI_CONTROLREG_SDHC_SPIEN_LSH      22
#define CSPI_CONTROLREG_BURST_LSH           23

#define CSPI_INT_TE_LSH                     0
#define CSPI_INT_TH_LSH                     1
#define CSPI_INT_TF_LSH                     2
#define CSPI_INT_TSHFE_LSH                  3
#define CSPI_INT_RR_LSH                     4
#define CSPI_INT_RH_LSH                     5
#define CSPI_INT_RF_LSH                     6
#define CSPI_INT_RO_LSH                     7
#define CSPI_INT_BO_LSH                     8
#define CSPI_INT_TEEN_LSH                   9
#define CSPI_INT_THEN_LSH                   10
#define CSPI_INT_TFEN_LSH                   11
#define CSPI_INT_TSHFEEN_LSH                12
#define CSPI_INT_RREN_LSH                   13
#define CSPI_INT_RHEN_LSH                   14
#define CSPI_INT_RFEN_LSH                   15
#define CSPI_INT_ROEN_LSH                   16
#define CSPI_INT_BOEN_LSH                   17

#define CSPI_TEST_TXCNT_LSH                 0
#define CSPI_TEST_RXCNT_LSH                 4
#define CSPI_TEST_SSSTATUS_LSH              8
#define CSPI_TEST_SS_ASSERT_LSH             12
#define CSPI_TEST_INIT_LSH                  13
#define CSPI_TEST_LBC_LSH                   14

#define CSPI_PERIOD_WAIT_LSH                0
#define CSPI_PERIOD_CSRC_LSH                15

#define CSPI_DMA_RHDMA_LSH                  4
#define CSPI_DMA_RFDMA_LSH                  5
#define CSPI_DMA_TEDMA_LSH                  6
#define CSPI_DMA_THDMA_LSH                  7
#define CSPI_DMA_RHDEN_LSH                  12
#define CSPI_DMA_TRDEN_LSH                  13
#define CSPI_DMA_TEDEN_LSH                  14
#define CSPI_DMA_THDEN_LSH                  15

#define CSPI_RESET_START_LSH                0


//------------------------------------------------------------------------------
// REGISTER FIELD WIDTHS
//------------------------------------------------------------------------------
#define CSPI_RXDATA_WID                     Width_32

#define CSPI_TXDATA_WID                     Width_32

#define CSPI_CONTROLREG_BITCOUNT_WID        Width_5
#define CSPI_CONTROLREG_POL_WID             Width_1
#define CSPI_CONTROLREG_PHA_WID             Width_1
#define CSPI_CONTROLREG_SSCTL_WID           Width_1
#define CSPI_CONTROLREG_SSPOL_WID           Width_1
#define CSPI_CONTROLREG_XCH_WID             Width_1
#define CSPI_CONTROLREG_SPIEN_WID           Width_1
#define CSPI_CONTROLREG_MODE_WID            Width_1
#define CSPI_CONTROLREG_DRCTL_WID           Width_2
#define CSPI_CONTROLREG_DATARATE_WID        Width_5
#define CSPI_CONTROLREG_CS_WID              Width_2
#define CSPI_CONTROLREG_SWAP_WID            Width_1
#define CSPI_CONTROLREG_SDHC_SPIEN_WID      Width_1
#define CSPI_CONTROLREG_BURST_WID           Width_1

#define CSPI_INT_TE_WID                     Width_1
#define CSPI_INT_TH_WID                     Width_1
#define CSPI_INT_TF_WID                     Width_1
#define CSPI_INT_TSHFE_WID                  Width_1
#define CSPI_INT_RR_WID                     Width_1
#define CSPI_INT_RH_WID                     Width_1
#define CSPI_INT_RF_WID                     Width_1
#define CSPI_INT_RO_WID                     Width_1
#define CSPI_INT_BO_WID                     Width_1
#define CSPI_INT_TEEN_WID                   Width_1
#define CSPI_INT_THEN_WID                   Width_1
#define CSPI_INT_TFEN_WID                   Width_1
#define CSPI_INT_TSHFEEN_WID                Width_1
#define CSPI_INT_RREN_WID                   Width_1
#define CSPI_INT_RHEN_WID                   Width_1
#define CSPI_INT_RFEN_WID                   Width_1
#define CSPI_INT_ROEN_WID                   Width_1
#define CSPI_INT_BOEN_WID                   Width_1

#define CSPI_TEST_TXCNT_WID                 Width_4
#define CSPI_TEST_RXCNT_WID                 Width_4
#define CSPI_TEST_SSSTATUS_WID              Width_4
#define CSPI_TEST_SS_ASSERT_WID             Width_2
#define CSPI_TEST_INIT_WID                  Width_1
#define CSPI_TEST_LBC_WID                   Width_1

#define CSPI_PERIOD_WAIT_WID                Width_14
#define CSPI_PERIOD_CSRC_WID                Width_1

#define CSPI_DMA_RHDMA_WID                  Width_1
#define CSPI_DMA_RFDMA_WID                  Width_1
#define CSPI_DMA_TEDMA_WID                  Width_1
#define CSPI_DMA_RHDMA_WID                  Width_1
#define CSPI_DMA_TEDEN_WID                  Width_1
#define CSPI_DMA_THDEN_WID                  Width_1
#define CSPI_DMA_RHDEN_WID                  Width_1
#define CSPI_DMA_RFDEN_WID                  Width_1

#define CSPI_RESET_START_WID                Width_1

//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------
//
// CONTROLREG
#define CSPI_CONTROLREG_BITCOUNT_1BIT       0x00    // 1-bit transfer
#define CSPI_CONTROLREG_BITCOUNT_8BIT       0x07    // 8-bit transfer
#define CSPI_CONTROLREG_BITCOUNT_16BIT      0x0F    // 16-bit transfer
#define CSPI_CONTROLREG_BITCOUNT_32BIT      0x1F    // 32-bit transfer

#define CSPI_CONTROLREG_POL_ACTIVE_HIGH     0x0     // Active high polarity
#define CSPI_CONTROLREG_POL_ACTIVE_LOW      0x1     // Active low polarity

#define CSPI_CONTROLREG_PHA0                0x0     // Phase 0 operation
#define CSPI_CONTROLREG_PHA1                0x1     // Phase 1 operation

#define CSPI_CONTROLREG_SSCTL_ASSERT        0x0     // MS: SS stays aserted
#define CSPI_CONTROLREG_SSCTL_PULSE         0x1     // MS: Insert pulse

#define CSPI_CONTROLREG_SSPOL_ACTIVE_LOW    0x0     // Active low
#define CSPI_CONTROLREG_SSPOL_ACTIVE_HIGH   0x1     // Active high

#define CSPI_CONTROLREG_XCH_IDLE            0x0     // Idle
#define CSPI_CONTROLREG_XCH_EN              0x1     // Initiates exchange

#define CSPI_CONTROLREG_SPIEN_DISABLE       0x0     // SPI disabled
#define CSPI_CONTROLREG_SPIEN_ENABLE        0x1     // SPI enabled

#define CSPI_CONTROLREG_MODE_SLAVE          0x0     // Slave mode
#define CSPI_CONTROLREG_MODE_MASTER         0x1     // Master mode

#define CSPI_CONTROLREG_DRCTL_DONTCARE      0x0     // Ignored
#define CSPI_CONTROLREG_DRCTL_FALLING_EDGE  0x1     // Falling edge trigger
#define CSPI_CONTROLREG_DRCTL_ACTIVE_LOW    0x2     // Active low trigger

// For even states other than 0, div = 2 * (2 ^ (n/2))
// For odd states other than 0, div = 3 * (2 ^ ((n-1)/2))
#define CSPI_CONTROLREG_DATARATE_DIV3       0x01    // Only available if SDHC_SPIEN set
#define CSPI_CONTROLREG_DATARATE_DIV4       0x02
#define CSPI_CONTROLREG_DATARATE_DIV6       0x03
#define CSPI_CONTROLREG_DATARATE_DIV8       0x04
#define CSPI_CONTROLREG_DATARATE_DIV12      0x05
#define CSPI_CONTROLREG_DATARATE_DIV16      0x06
#define CSPI_CONTROLREG_DATARATE_DIV24      0x07
#define CSPI_CONTROLREG_DATARATE_DIV32      0x08
#define CSPI_CONTROLREG_DATARATE_DIV48      0x09
#define CSPI_CONTROLREG_DATARATE_DIV64      0x0A
#define CSPI_CONTROLREG_DATARATE_DIV96      0x0B
#define CSPI_CONTROLREG_DATARATE_DIV128     0x0C
#define CSPI_CONTROLREG_DATARATE_DIV192     0x0D
#define CSPI_CONTROLREG_DATARATE_DIV256     0x0E
#define CSPI_CONTROLREG_DATARATE_DIV384     0x0F
#define CSPI_CONTROLREG_DATARATE_DIV512     0x10
// Min and max divisor values
#define CSPI_CONTROLREG_DATARATE_DIV_MIN    3
#define CSPI_CONTROLREG_DATARATE_DIV_MAX    512

#define CSPI_CONTROLREG_CS_SS0              0x0     // SS0 will be asserted
#define CSPI_CONTROLREG_CS_SS1              0x1     // SS1 will be asserted
#define CSPI_CONTROLREG_CS_SS2              0x2     // SS2 will be asserted
#define CSPI_CONTROLREG_CS_SS3              0x3     // SS3 will be asserted

#define CSPI_CONTROLREG_SWAP_NOSWAP         0x0     // No swapping
#define CSPI_CONTROLREG_SWAP_SWAPDATA       0x1     // Byte swapping for TxFIFO & RxFIFO

#define CSPI_CONTROLREG_SDHC_SPIEN_DISABLE  0x0     // SDHC SPI mode disabled
#define CSPI_CONTROLREG_SDHC_SPIEN_ENABLE   0x1     // SDHC SPI mode enabled

#define CSPI_CONTROLREG_BURST_INSERTIDLE    0x0     // Idle time inserted between transfers
#define CSPI_CONTROLREG_BURST_NOIDLE        0x1     // No idle time between consecutive data transfers

// INTREG
#define CSPI_INT_TH_NOT_HALF_EMPTY          0       // Less than 4 empty slots in TxFIFO
#define CSPI_INT_TH_HALF_EMPTY              1       // 4 or more empty slots in TxFIFO

#define CSPI_INT_TF_NOT_FULL                0       // Less than 9 data words in TxFIFO
#define CSPI_INT_TF_FULL                    1       // 9 data words in buffer

#define CSPI_INT_TSHFE_NOT_EMPTY            0       // At least one data word is in TxFIFO or at least one data bit is in Tx Shift Register.
#define CSPI_INT_TSHFE_EMPTY                1       // TxFIFO and Tx Shift Register are empty

#define CSPI_INT_RR_EMPTY                   0       // RxFIFO is empty
#define CSPI_INT_RR_NOT_EMPTY               1       // At least one data word in RxFIFO

#define CSPI_INT_RH_NOT_HALF_FULL           0       // Less than 4 data words in RxFIFO
#define CSPI_INT_RH_HALF_FULL               1       // More than or equal to 4 data in RxFIFO

#define CSPI_INT_RF_NOT_FULL                0       // Less than 8 data words in RxFIFO
#define CSPI_INT_RF_FULL                    1       // 8 data words in RxFIFO.

#define CSPI_INT_RO_NOT_OVERFLOW            0       // RxFIFO is not overflow
#define CSPI_INT_RO_OVERFLOW                1       // RxFIFO is overflow

#define CSPI_INT_BO_NOT_OVERFLOW            0       // Bit count overflow error
#define CSPI_INT_BO_OVERFLOW                1       // Bit count overflow

#define CSPI_INT_TEEN_DISABLE               0       // TxFIFO empty interrupt
#define CSPI_INT_TEEN_ENABLE                1       // TxFIFO empty interrupt

#define CSPI_INT_THEN_DISABLE               0       // TxFIFO half interrupt
#define CSPI_INT_THEN_ENABLE                1       // TxFIFO half interrupt

#define CSPI_INT_TFEN_DISABLE               0       // TxFIFO full interrupt
#define CSPI_INT_TFEN_ENABLE                1       // TxFIFO full interrupt

#define CSPI_INT_TSHFEEN_DISABLE            0       // Transfer shift register empty interrupt
#define CSPI_INT_TSHFEEN_ENABLE             1       // Transfer shift register empty interrupt

#define CSPI_INT_RREN_DISABLE               0       // RxFIFO data ready interrupt
#define CSPI_INT_RREN_ENABLE                1       // RxFIFO data ready interrupt

#define CSPI_INT_RHEN_DISABLE               0       // RxFIFO half interrupt
#define CSPI_INT_RHEN_ENABLE                1       // RxFIFO half interrupt

#define CSPI_INT_RFEN_DISABLE               0       // RxFIFO full interrupt
#define CSPI_INT_RFEN_ENABLE                1       // RxFIFO full interrupt

#define CSPI_INT_ROEN_DISABLE               0       // RxFIFO overflow interrupt
#define CSPI_INT_ROEN_ENABLE                1       // RxFIFO overflow interrupt

#define CSPI_INT_BOEN_DISABLE               0       // Bit count overflow interrupt
#define CSPI_INT_BOEN_ENABLE                1       // Bit count overflow interrupt

// TEST
#define CSPI_TEST_INIT_NOINIT               0x0     // No initialization
#define CSPI_TEST_INIT_INITSTATE            0x1     // Init state machine

#define CSPI_TEST_LBC_NOCONN                0x0     // Not connected
#define CSPI_TEST_LBC_CONN                  0x1     // Internally connected

#define CSPI_TEST_SS_ASSERT_DISABLE         0x0     // SSn asserted as per control reg
#define CSPI_TEST_SS_ASSERT_ENABLE          0x1     // SSn remains asserted after TsSHFD set

// PERIOD
#define CSPI_PERIOD_CSRC_SPI                0x0     // SPI clock
#define CSPI_PERIOD_CSRC_CKIL               0x1     // 32.768kHz

// DMA
#define CSPI_DMA_RFDMA_NOT_HALF             0       // RxFIFO has less than 4 slots
#define CSPI_DMA_RFDMA_HALF                 1       // RxFIFO has 4 or more slots

#define CSPI_DMA_RHDMA_NOT_FULL             0       // RxFIFO not full
#define CSPI_DMA_RHDMA_FULL                 1       // RxFIFO is full

#define CSPI_DMA_THDMA_NOT_EMPTY            0       // TxFIFO not empty
#define CSPI_DMA_THDMA_EMPTY                1       // TxFIFO is empty

#define CSPI_DMA_TEDMA_NOT_HALF             0       // TxFIFO has less than 4 empty slots
#define CSPI_DMA_TEDMA_HALF                 1       // TxFIFO has 4 or more empty slots

#define CSPI_DMA_RHDEN_DISABLE              0       // RxFIFO half full DMA req
#define CSPI_DMA_RHDEN_ENABLE               1       // RxFIFO half full DMA req

#define CSPI_DMA_RFDEN_DISABLE              0       // RxFIFO full DMA req
#define CSPI_DMA_RFDEN_ENABLE               1       // RxFIFO full DMA req

#define CSPI_DMA_TEDEN_DISABLE              0       // TxFIFO empty DMA req
#define CSPI_DMA_TEDEN_ENABLE               1       // TxFIFO empty DMA req

#define CSPI_DMA_THDEN_DISABLE              0       // TxFIFO half empty DMA req
#define CSPI_DMA_THDEN_ENABLE               1       // TxFIFO half empty DMA req

// RESET
#define CSPI_RESET_START_NORESET            0       // No soft reset
#define CSPI_RESET_START_SWRESET            1       // Generate soft reset



#endif /*IMX27_CSPI_H_*/


