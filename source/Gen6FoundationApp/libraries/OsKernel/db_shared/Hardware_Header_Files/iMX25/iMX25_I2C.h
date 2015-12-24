/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  HARDWARE DEFINITION FILE 
//  Date:       9/15/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: db_shared/Hardware_Header_Files/iMX25/iMX25_I2C.h $
// $Revision: 1.2 $
// $Date: 2011/06/28 13:26:54EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx25_I2C.h
//
// Provides definitions for the Inter-Integrated Circuit (I2C) module
// Refer to Chapter 38 of the iMX25 Reference Manual
//
//------------------------------------------------------------------------------

#ifndef IMX25_I2C_H_
#define IMX25_I2C_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// REGISTERS
//------------------------------------------------------------------------------

// #########################################
// # I2C1                                  #
// #########################################
#define I2C1_BASE_ADDR             0x43F80000U
#define I2C1_IADR                  (I2C1_BASE_ADDR+0x00U)      // 16bit i2c address reg
#define I2C1_IFDR                  (I2C1_BASE_ADDR+0x04U)      // 16bit i2c frequency divider reg
#define I2C1_I2CR                  (I2C1_BASE_ADDR+0x08U)      // 16bit i2c control reg
#define I2C1_I2SR                  (I2C1_BASE_ADDR+0x0CU)      // 16bit i2c status reg
#define I2C1_I2DR                  (I2C1_BASE_ADDR+0x10U)      // 16bit i2c data i/o reg

// #########################################
// # I2C2                                  #
// #########################################
#define I2C2_BASE_ADDR             0x43F98000U
#define I2C2_IADR                  (I2C2_BASE_ADDR+0x00U)      // 16bit i2c address reg
#define I2C2_IFDR                  (I2C2_BASE_ADDR+0x04U)      // 16bit i2c frequency divider reg
#define I2C2_I2CR                  (I2C2_BASE_ADDR+0x08U)      // 16bit i2c control reg
#define I2C2_I2SR                  (I2C2_BASE_ADDR+0x0CU)      // 16bit i2c status reg
#define I2C2_I2DR                  (I2C2_BASE_ADDR+0x10U)      // 16bit i2c data i/o reg

// #########################################
// # I2C3                                  #
// #########################################
#define I2C3_BASE_ADDR             0x43F84000U
#define I2C3_IADR                  (I2C3_BASE_ADDR+0x00U)      // 16bit i2c address reg
#define I2C3_IFDR                  (I2C3_BASE_ADDR+0x04U)      // 16bit i2c frequency divider reg
#define I2C3_I2CR                  (I2C3_BASE_ADDR+0x08U)      // 16bit i2c control reg
#define I2C3_I2SR                  (I2C3_BASE_ADDR+0x0CU)      // 16bit i2c status reg
#define I2C3_I2DR                  (I2C3_BASE_ADDR+0x10U)      // 16bit i2c data i/o reg


//------------------------------------------------------------------------------
// REGISTER BIT FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------

#define I2C_IADR_ADR_LSH        1

#define I2C_IFDR_IC_LSH         0

#define I2C_I2CR_RSTA_LSH       2
#define I2C_I2CR_TXAK_LSH       3
#define I2C_I2CR_MTX_LSH        4
#define I2C_I2CR_MSTA_LSH       5
#define I2C_I2CR_IIEN_LSH       6
#define I2C_I2CR_IEN_LSH        7

#define I2C_I2SR_RXAK_LSH       0
#define I2C_I2SR_IIF_LSH        1
#define I2C_I2SR_SRW_LSH        2
#define I2C_I2SR_IAL_LSH        4
#define I2C_I2SR_IBB_LSH        5
#define I2C_I2SR_IAAS_LSH       6
#define I2C_I2SR_ICF_LSH        7

#define I2C_I2DR_D_LSH          0


//------------------------------------------------------------------------------
// REGISTER FIELD WIDTHS
//------------------------------------------------------------------------------

#define I2C_IADR_ADR_WID        Bit16_Width_7

#define I2C_IFDR_IC_WID         Bit16_Width_6

#define I2C_I2CR_RSTA_WID       Bit16_Width_1
#define I2C_I2CR_TXAK_WID       Bit16_Width_1
#define I2C_I2CR_MTX_WID        Bit16_Width_1
#define I2C_I2CR_MSTA_WID       Bit16_Width_1
#define I2C_I2CR_IIEN_WID       Bit16_Width_1
#define I2C_I2CR_IEN_WID        Bit16_Width_1

#define I2C_I2SR_RXAK_WID       Bit16_Width_1
#define I2C_I2SR_IIF_WID        Bit16_Width_1
#define I2C_I2SR_SRW_WID        Bit16_Width_1
#define I2C_I2SR_IAL_WID        Bit16_Width_1
#define I2C_I2SR_IBB_WID        Bit16_Width_1
#define I2C_I2SR_IAAS_WID       Bit16_Width_1
#define I2C_I2SR_ICF_WID        Bit16_Width_1

#define I2C_I2DR_D_WID          Bit16_Width_8

//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------

// I2CR
#define I2C_I2CR_RSTA_GENERATE      1   // Repeat Start will be generated
#define I2C_I2CR_RSTA_WITHHOLD      0   // Otherwise

#define I2C_I2CR_TXAK_NO_ACK_SEND   1   // will not send an acknowledgement
#define I2C_I2CR_TXAK_ACK_SEND      0   // otherwise

#define I2C_I2CR_MTX_TRANSMIT       1   // set to transmit mode
#define I2C_I2CR_MTX_RECEIVE        0   // set to receive mode

#define I2C_I2CR_MSTA_MASTER        1   // set to master mode
#define I2C_I2CR_MSTA_SLAVE         0   // set to slave mode

#define I2C_I2CR_IIEN_ENABLE        1   // enable interrupt
#define I2C_I2CR_IIEN_DISABLE       0   // otherwise

#define I2C_I2CR_IEN_ENABLE         1   // enable I2C module
#define I2C_I2CR_IEN_DISABLE        0   // otherwise

// I2SR
#define I2C_I2SR_RXAK_NO_ACK_DETECT 1   // did not detect an ack during bus cycle
#define I2C_I2SR_RXAK_ACK_DETECT    0   // otherwise

#define I2C_I2SR_IIF_PENDING        1   // interrupt pending
#define I2C_I2SR_IIF_NOT_PENDING    0   // otherwise

#define I2C_I2SR_SRW_TRANSMIT       1   // slave transmitting
#define I2C_I2SR_SRW_RECEIVE        0   // slave receiving

#define I2C_I2SR_IAL_LOST           1   // Arbitration loss
#define I2C_I2SR_IAL_NOT_LOST       0   // otherwise

#define I2C_I2SR_IBB_BUSY           1   // bus busy
#define I2C_I2SR_IBB_IDLE           0   // otherwise

#define I2C_I2SR_IAAS_ADDRESSED     1   // addressed as slave
#define I2C_I2SR_IAAS_NOT_ADDRESSED 0   // otherwise

#define I2C_I2SR_ICF_COMPLETE       1   // transfer complete
#define I2C_I2SR_ICF_IN_PROGRESS    0   // transfer in progress


#endif /*IMX25_I2C_H_*/


