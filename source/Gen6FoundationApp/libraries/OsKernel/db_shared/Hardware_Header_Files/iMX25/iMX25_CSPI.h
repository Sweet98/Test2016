/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  HARDWARE DEFINITION FILE 
//  Date:       9/17/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: db_shared/Hardware_Header_Files/iMX25/iMX25_CSPI.h $
// $Revision: 1.2 $
// $Date: 2011/06/28 13:26:50EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx25_cspi.h
//
// Provides definitions for the Configurable Serial Peripheral Interface (CSPI)
// Refer to Chapter 34 of the iMX25 Reference Manual
//
//------------------------------------------------------------------------------
#ifndef IMX25_CSPI_H_
#define IMX25_CSPI_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------
// #########################################
// # CSPI1                                 #
// #########################################
#define CSPI1_BASE_ADDR            0x43FA4000U
#define CSPI1_RXDATA               (CSPI1_BASE_ADDR+0x00U)     // 32bit cspi1 receive data reg
#define CSPI1_TXDATA               (CSPI1_BASE_ADDR+0x04U)     // 32bit cspi1 transmit data reg
#define CSPI1_CONREG               (CSPI1_BASE_ADDR+0x08U)     // 32bit cspi1 control reg
#define CSPI1_INTREG               (CSPI1_BASE_ADDR+0x0CU)     // 32bit cspi1 interrupt stat/ctr reg
#define CSPI1_DMAREG               (CSPI1_BASE_ADDR+0x10U)     // 32bit cspi1 test reg
#define CSPI1_STATREG              (CSPI1_BASE_ADDR+0x14U)     // 32bit cspi1 sample period ctrl reg
#define CSPI1_PERIODREG            (CSPI1_BASE_ADDR+0x18U)     // 32bit cspi1 dma ctrl reg
#define CSPI1_TESTREG              (CSPI1_BASE_ADDR+0x1CU)     // 32bit cspi1 soft reset reg

// #########################################
// # CSPI2                                 #
// #########################################
#define CSPI2_BASE_ADDR            0x50010000U
#define CSPI2_RXDATA               (CSPI2_BASE_ADDR+0x00U)     // 32bit cspi2 receive data reg
#define CSPI2_TXDATA               (CSPI2_BASE_ADDR+0x04U)     // 32bit cspi2 transmit data reg
#define CSPI2_CONREG               (CSPI2_BASE_ADDR+0x08U)     // 32bit cspi2 control reg
#define CSPI2_INTREG               (CSPI2_BASE_ADDR+0x0CU)     // 32bit cspi2 interrupt stat/ctr reg
#define CSPI2_DMAREG               (CSPI2_BASE_ADDR+0x10U)     // 32bit cspi2 test reg
#define CSPI2_STATREG              (CSPI2_BASE_ADDR+0x14U)     // 32bit cspi2 sample period ctrl reg
#define CSPI2_PERIODREG            (CSPI2_BASE_ADDR+0x18U)     // 32bit cspi2 dma ctrl reg
#define CSPI2_TESTREG              (CSPI2_BASE_ADDR+0x1CU)     // 32bit cspi2 soft reset reg

// #########################################
// # CSPI3                                 #
// #########################################
#define CSPI3_BASE_ADDR            0x50004000U
#define CSPI3_RXDATA               (CSPI3_BASE_ADDR+0x00U)     // 32bit cspi3 receive data reg
#define CSPI3_TXDATA               (CSPI3_BASE_ADDR+0x04U)     // 32bit cspi3 transmit data reg
#define CSPI3_CONREG               (CSPI3_BASE_ADDR+0x08U)     // 32bit cspi3 control reg
#define CSPI3_INTREG               (CSPI3_BASE_ADDR+0x0CU)     // 32bit cspi3 interrupt stat/ctr reg
#define CSPI3_DMAREG               (CSPI3_BASE_ADDR+0x10U)     // 32bit cspi3 test reg
#define CSPI3_STATREG              (CSPI3_BASE_ADDR+0x14U)     // 32bit cspi3 sample period ctrl reg
#define CSPI3_PERIODREG            (CSPI3_BASE_ADDR+0x18U)     // 32bit cspi3 dma ctrl reg
#define CSPI3_TESTREG              (CSPI3_BASE_ADDR+0x1CU)     // 32bit cspi3 soft reset reg

//------------------------------------------------------------------------------
// REGISTER FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
//Receive Data Register (RXDATA)
#define CSPI_RXDATA_LSH                 	0

//Transmit Data Register (TXDATA)
#define CSPI_TXDATA_LSH                 	0

//Control Register (CONREG)
#define CSPI_CONREG_EN_LSH					0
#define CSPI_CONREG_MODE_LSH            	1
#define CSPI_CONREG_XCH_LSH             	2
#define CSPI_CONREG_SMC_LSH             	3
#define CSPI_CONREG_POL_LSH             	4
#define CSPI_CONREG_PHA_LSH             	5
#define CSPI_CONREG_SSCTL_LSH           	6
#define CSPI_CONREG_SSPOL_LSH           	7
#define CSPI_CONREG_DRCTL_LSH           	8
#define CSPI_CONREG_CHIPSELECT_LSH      	12
#define CSPI_CONREG_DATARATE_LSH        	16
#define CSPI_CONREG_BURST_LSH           	20

//Interrupt Control Register (INTREG)
#define CSPI_INTREG_TEEN_LSH                0
#define CSPI_INTREG_THEN_LSH                1
#define CSPI_INTREG_TFEN_LSH                2
#define CSPI_INTREG_RREN_LSH                3
#define CSPI_INTREG_RHEN_LSH                4
#define CSPI_INTREG_RFEN_LSH                5
#define CSPI_INTREG_ROEN_LSH                6
#define CSPI_INTREG_TCEN_LSH                7

//DMA Control Register (DMAREG)
#define CSPI_DMAREG_TEDEN_LSH               0
#define CSPI_DMAREG_THDEN_LSH               1
#define CSPI_DMAREG_RHDEN_LSH               4
#define CSPI_DMAREG_RFDEN_LSH               5

//Status Register (STATREG)
#define CSPI_STATREG_TE_LSH	                0
#define CSPI_STATREG_TH_LSH	                1
#define CSPI_STATREG_TF_LSH	                2
#define CSPI_STATREG_RR_LSH	                3
#define CSPI_STATREG_RH_LSH	                4
#define CSPI_STATREG_RF_LSH	                5
#define CSPI_STATREG_RO_LSH	                6
#define CSPI_STATREG_TC_LSH	                7

//Sample Period Control Register (PERIODREG)
#define CSPI_PERIODREG_SAMPLE_PERIOD_LSH    0
#define CSPI_PERIODREG_CSRC_LSH      		15

//Test Control Register (TESTREG)
#define CSPI_TESTREG_TXCNT_LSH              0
#define CSPI_TESTREG_RXCNT_LSH              4
#define CSPI_TESTREG_SMSTATUS_LSH           8
#define CSPI_TESTREG_LBC_LSH                14
#define CSPI_TESTREG_SWAP_LSH               15


//------------------------------------------------------------------------------
// REGISTER FIELD WIDTHS
//------------------------------------------------------------------------------
//Receive Data Register (RXDATA)
#define CSPI_RXDATA_WID                     Width_32

//Transmit Data Register (TXDATA)
#define CSPI_TXDATA_WID                     Width_32

//Control Register (CONREG)
#define CSPI_CONREG_EN_WID					Width_1
#define CSPI_CONREG_MODE_WID            	Width_1
#define CSPI_CONREG_XCH_WID             	Width_1
#define CSPI_CONREG_SMC_WID             	Width_1
#define CSPI_CONREG_POL_WID             	Width_1
#define CSPI_CONREG_PHA_WID             	Width_1
#define CSPI_CONREG_SSCTL_WID           	Width_1
#define CSPI_CONREG_SSPOL_WID           	Width_1
#define CSPI_CONREG_DRCTL_WID           	Width_2
#define CSPI_CONREG_CHIPSELECT_WID      	Width_2
#define CSPI_CONREG_DATARATE_WID        	Width_3
#define CSPI_CONREG_BURST_WID           	Width_12

//Interrupt Control Register (INTREG)
#define CSPI_INTREG_TEEN_WID	           	Width_1
#define CSPI_INTREG_THEN_WID    	       	Width_1
#define CSPI_INTREG_TFEN_WID	           	Width_1
#define CSPI_INTREG_RREN_WID	           	Width_1
#define CSPI_INTREG_RHEN_WID	           	Width_1
#define CSPI_INTREG_RFEN_WID	           	Width_1
#define CSPI_INTREG_ROEN_WID	           	Width_1
#define CSPI_INTREG_TCEN_WID	           	Width_1

//DMAREG Control Register (DMAREGREG)
#define CSPI_DMAREG_TEDEN_WID	          	Width_1
#define CSPI_DMAREG_THDEN_WID	          	Width_1
#define CSPI_DMAREG_RHDEN_WID	          	Width_1
#define CSPI_DMAREG_RFDEN_WID	          	Width_1

//Status Register (STATREG)
#define CSPI_STATREG_TE_WID	           		Width_1
#define CSPI_STATREG_TH_WID	           		Width_1
#define CSPI_STATREG_TF_WID	           		Width_1
#define CSPI_STATREG_RR_WID	           		Width_1
#define CSPI_STATREG_RH_WID	           		Width_1
#define CSPI_STATREG_RF_WID	           		Width_1
#define CSPI_STATREG_RO_WID	           		Width_1
#define CSPI_STATREG_TC_WID	           		Width_1

//Sample Period Control Register (PERIODREG)
#define CSPI_PERIODREG_SAMPLE_PERIOD_WID	Width_15
#define CSPI_PERIODREG_CSRC_WID	           	Width_1

//Test Control Register (TESTREG)
#define CSPI_TESTREG_TXCNT_WID	           	Width_4
#define CSPI_TESTREG_RXCNT_WID	           	Width_4
#define CSPI_TESTREG_SMSTATUS_WID	        Width_4
#define CSPI_TESTREG_LBC_WID	           	Width_1
#define CSPI_TESTREG_SWAP_WID	           	Width_1


//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------
//
// CONREG
#define CSPI_CONREG_POL_ACTIVE_HIGH     		0x0     // Active high polarity
#define CSPI_CONREG_POL_ACTIVE_LOW      		0x1     // Active low polarity

#define CSPI_CONREG_PHA0                		0x0     // Phase 0 operation
#define CSPI_CONREG_PHA1                		0x1     // Phase 1 operation

#define CSPI_CONREG_SSCTL_ASSERT       		 	0x0     // MS: SS stays aserted
#define CSPI_CONREG_SSCTL_PULSE         		0x1     // MS: Insert pulse

#define CSPI_CONREG_SSPOL_ACTIVE_LOW    		0x0     // Active low
#define CSPI_CONREG_SSPOL_ACTIVE_HIGH   		0x1     // Active high

#define CSPI_CONREG_XCH_IDLE            		0x0     // Idle
#define CSPI_CONREG_XCH_EN              		0x1     // Initiates exchange

#define CSPI_CONREG_SPIEN_DISABLE       		0x0     // SPI disabled
#define CSPI_CONREG_SPIEN_ENABLE        		0x1     // SPI enabled

#define CSPI_CONREG_MODE_SLAVE          		0x0     // Slave mode
#define CSPI_CONREG_MODE_MASTER         		0x1     // Master mode

#define CSPI_CONREG_DRCTL_DONTCARE      		0x0     // Ignored
#define CSPI_CONREG_DRCTL_FALLING_EDGE  		0x1     // Falling edge trigger
#define CSPI_CONREG_DRCTL_ACTIVE_LOW    		0x2     // Active low trigger

// For even states other than 0, div = 2 * (2 ^ (n/2))
#define CSPI_CONREG_DATARATE_DIV4       		0x00
#define CSPI_CONREG_DATARATE_DIV8      		 	0x01
#define CSPI_CONREG_DATARATE_DIV16      		0x02
#define CSPI_CONREG_DATARATE_DIV32      		0x03
#define CSPI_CONREG_DATARATE_DIV64      		0x04
#define CSPI_CONREG_DATARATE_DIV128     		0x05
#define CSPI_CONREG_DATARATE_DIV256     		0x06
#define CSPI_CONREG_DATARATE_DIV512     		0x07

#define CSPI_CONREG_CS_SS0              		0x0     // SS0 will be asserted
#define CSPI_CONREG_CS_SS1              		0x1     // SS1 will be asserted
#define CSPI_CONREG_CS_SS2              		0x2     // SS2 will be asserted
#define CSPI_CONREG_CS_SS3              		0x3     // SS3 will be asserted

#define CSPI_CONREG_SWAP_NOSWAP         		0x0     // No swapping
#define CSPI_CONREG_SWAP_SWAPDATA       		0x1     // Byte swapping for TxFIFO & RxFIFO

#define CSPI_CONREG_SDHC_SPIEN_DISABLE  		0x0     // SDHC SPI mode disabled
#define CSPI_CONREG_SDHC_SPIEN_ENABLE   		0x1     // SDHC SPI mode enabled

#define CSPI_CONREG_BURST_INSERTIDLE    		0x0     // Idle time inserted between transfers
#define CSPI_CONREG_BURST_NOIDLE        		0x1     // No idle time between consecutive data transfers

// INTREG
#define CSPI_INT_TEEN_DISABLE               	0       // TxFIFO empty interrupt
#define CSPI_INT_TEEN_ENABLE                	1       // TxFIFO empty interrupt

#define CSPI_INT_THEN_DISABLE               	0       // TxFIFO half interrupt
#define CSPI_INT_THEN_ENABLE                	1       // TxFIFO half interrupt

#define CSPI_INT_TFEN_DISABLE               	0       // TxFIFO full interrupt
#define CSPI_INT_TFEN_ENABLE                	1       // TxFIFO full interrupt

#define CSPI_INT_RREN_DISABLE               	0       // RxFIFO data ready interrupt
#define CSPI_INT_RREN_ENABLE                	1       // RxFIFO data ready interrupt

#define CSPI_INT_RHEN_DISABLE               	0       // RxFIFO half interrupt
#define CSPI_INT_RHEN_ENABLE                	1       // RxFIFO half interrupt

#define CSPI_INT_RFEN_DISABLE               	0       // RxFIFO full interrupt
#define CSPI_INT_RFEN_ENABLE                	1       // RxFIFO full interrupt

#define CSPI_INT_ROEN_DISABLE               	0       // RxFIFO overflow interrupt
#define CSPI_INT_ROEN_ENABLE                	1       // RxFIFO overflow interrupt
	
#define CSPI_INT_TCEN_DISABLE               	0       // Transfer Completed Interrupt
#define CSPI_INT_TCEN_ENABLE                	1       // Transfer Completed Interrupt


// DMAREG
#define CSPI_DMAREG_TEDEN_DISABLE              	0       // TxFIFO empty DMAREG req
#define CSPI_DMAREG_TEDEN_ENABLE                1       // TxFIFO empty DMAREG req

#define CSPI_DMAREG_THDEN_DISABLE              	0       // TxFIFO half empty DMAREG req
#define CSPI_DMAREG_THDEN_ENABLE               	1       // TxFIFO half empty DMAREG req

#define CSPI_DMAREG_RHDEN_DISABLE              	0       // RXFIFO Half Full DMA Request Enable
#define CSPI_DMAREG_RHDEN_ENABLE               	1       // RXFIFO Half Full DMA Request Enable

#define CSPI_DMAREG_RFDEN_DISABLE              	0       // RXFIFO Full DMA Request Enable
#define CSPI_DMAREG_RFDEN_ENABLE               	1       // RXFIFO Full DMA Request Enable


// TESTREG
#define CSPI_TESTREG_LBC_NOCONN                0x0     // Not connected
#define CSPI_TESTREG_LBC_CONN                  0x1     // Internally connected


#endif /*IMX25_CSPI_H_*/


