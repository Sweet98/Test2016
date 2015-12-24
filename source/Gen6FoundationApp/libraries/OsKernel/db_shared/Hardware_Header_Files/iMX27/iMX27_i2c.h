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
// $RCSfile: db_shared/Hardware_Header_Files/iMX27/iMX27_i2c.h $
// $Revision: 1.1 $
// $Date: 2008/09/22 16:35:08EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx27_i2c.h
//
// Provides definitions for the Inter-Integrated Circuit (I2C) module
// Refer to Chapter 24 Page 786 of the iMX27 Reference Manual
//
//------------------------------------------------------------------------------

#ifndef IMX27_I2C_H_
#define IMX27_I2C_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// REGISTERS
//------------------------------------------------------------------------------

// #########################################
// # I2C1                                  #
// # $1001_2000 to $1001_2FFF              #
// #########################################
#define I2C1_BASE_ADDR           0x10012000
#define I2C1_IADR                (I2C1_BASE_ADDR+0x00)      // 16bit i2c address reg
#define I2C1_IFDR                (I2C1_BASE_ADDR+0x04)      // 16bit i2c frequency divider reg
#define I2C1_I2CR                (I2C1_BASE_ADDR+0x08)      // 16bit i2c control reg
#define I2C1_I2SR                (I2C1_BASE_ADDR+0x0C)      // 16bit i2c status reg
#define I2C1_I2DR                (I2C1_BASE_ADDR+0x10)      // 16bit i2c data i/o reg


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


#endif /*IMX27_I2C_H_*/


