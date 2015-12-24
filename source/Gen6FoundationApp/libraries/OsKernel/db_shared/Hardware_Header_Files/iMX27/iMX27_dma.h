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
// $RCSfile: db_shared/Hardware_Header_Files/iMX27/iMX27_dma.h $
// $Revision: 1.1 $
// $Date: 2008/09/22 16:35:06EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx27_dma.h
//
// Provides definitions for the The Direct Memory Access Controller (DMAC)
// Address will be referenced with DMA and not DMAC
// Refer to Chapter 37 Page 1307 of the iMX27 Reference Manual
//
//-----------------------------------------------------------------------------

#ifndef IMX27_DMA_H_
#define IMX27_DMA_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------

// #########################################
// # DMA                                   #
// # $1000_1000 to $1000_1FFF              #
// #########################################
#define DMA_BASE_ADDR            0x10001000
#define DMA_SYS_BASE             (DMA_BASE_ADDR+0x000)      // base location for system
#define DMA_M2D_BASE             (DMA_BASE_ADDR+0x040)      // base location for 2D memory reg
#define DMA_CH0_BASE             (DMA_BASE_ADDR+0x080)      // base location for channel 0
#define DMA_CH1_BASE             (DMA_BASE_ADDR+0x0C0)      // base location for channel 1
#define DMA_CH2_BASE             (DMA_BASE_ADDR+0x100)      // base location for channel 2
#define DMA_CH3_BASE             (DMA_BASE_ADDR+0x140)      // base location for channel 3
#define DMA_CH4_BASE             (DMA_BASE_ADDR+0x180)      // base location for channel 4
#define DMA_CH5_BASE             (DMA_BASE_ADDR+0x1C0)      // base location for channel 5
#define DMA_CH6_BASE             (DMA_BASE_ADDR+0x200)      // base location for channel 6
#define DMA_CH7_BASE             (DMA_BASE_ADDR+0x240)      // base location for channel 7
#define DMA_CH8_BASE             (DMA_BASE_ADDR+0x280)      // base location for channel 8
#define DMA_CH9_BASE             (DMA_BASE_ADDR+0x2C0)      // base location for channel 9
#define DMA_CH10_BASE            (DMA_BASE_ADDR+0x300)      // base location for channel 10
#define DMA_CH11_BASE            (DMA_BASE_ADDR+0x340)      // base location for channel 11
#define DMA_CH12_BASE            (DMA_BASE_ADDR+0x380)      // base location for channel 12
#define DMA_CH13_BASE            (DMA_BASE_ADDR+0x3C0)      // base location for channel 13
#define DMA_CH14_BASE            (DMA_BASE_ADDR+0x400)      // base location for channel 14
#define DMA_CH15_BASE            (DMA_BASE_ADDR+0x440)      // base location for channel 15
#define DMA_TEST_BASE            (DMA_BASE_ADDR+0x480)      // base location for test registers

#define DMA_DCR                  (DMA_SYS_BASE+0x000)       // 32bit dma control reg
#define DMA_DISR                 (DMA_SYS_BASE+0x004)       // 32bit dma interrupt status reg
#define DMA_DIMR                 (DMA_SYS_BASE+0x008)       // 32bit dma interrupt mask reg
#define DMA_DBTOSR               (DMA_SYS_BASE+0x00C)       // 32bit dma burst timeout stat reg
#define DMA_DRTOSR               (DMA_SYS_BASE+0x010)       // 32bit dma req timeout status reg
#define DMA_DSESR                (DMA_SYS_BASE+0x014)       // 32bit dma transfer err status reg
#define DMA_DBOSR                (DMA_SYS_BASE+0x018)       // 32bit dma buffer overflow stat reg
#define DMA_DBTOCR               (DMA_SYS_BASE+0x01C)       // 32bit dma burst timeout ctrl reg

#define DMA_WSRA                 (DMA_M2D_BASE+0x000)       // 32bit dma W-size A reg
#define DMA_XSRA                 (DMA_M2D_BASE+0x004)       // 32bit dma X-size A reg
#define DMA_YSRA                 (DMA_M2D_BASE+0x008)       // 32bit dma Y-size A reg
#define DMA_WSRB                 (DMA_M2D_BASE+0x00C)       // 32bit dma W-size B reg
#define DMA_XSRB                 (DMA_M2D_BASE+0x010)       // 32bit dma X-size B reg
#define DMA_YSRB                 (DMA_M2D_BASE+0x014)       // 32bit dma Y-size B reg

#define DMA_SAR0                 (DMA_CH0_BASE+0x000)       // 32bit dma ch0 source addr reg
#define DMA_DAR0                 (DMA_CH0_BASE+0x004)       // 32bit dma ch0 dest addr reg
#define DMA_CNTR0                (DMA_CH0_BASE+0x008)       // 32bit dma ch0 count reg
#define DMA_CCR0                 (DMA_CH0_BASE+0x00C)       // 32bit dma ch0 control reg
#define DMA_RSSR0                (DMA_CH0_BASE+0x010)       // 32bit dma ch0 req source sel reg
#define DMA_BLR0                 (DMA_CH0_BASE+0x014)       // 32bit dma ch0 burst lenght reg
#define DMA_RTOR0                (DMA_CH0_BASE+0x018)       // 32bit dma ch0 req time out reg
#define DMA_BUCR0                (DMA_CH0_BASE+0x018)       // 32bit dma ch0 bus utilization reg
#define DMA_CCNR0                (DMA_CH0_BASE+0x01C)       // 32bit dma ch0 counter reg

#define DMA_SAR1                 (DMA_CH1_BASE+0x000)       // 32bit dma ch1 source addr reg
#define DMA_DAR1                 (DMA_CH1_BASE+0x004)       // 32bit dma ch1 dest addr reg
#define DMA_CNTR1                (DMA_CH1_BASE+0x008)       // 32bit dma ch1 count reg
#define DMA_CCR1                 (DMA_CH1_BASE+0x00C)       // 32bit dma ch1 control reg
#define DMA_RSSR1                (DMA_CH1_BASE+0x010)       // 32bit dma ch1 req source sel reg
#define DMA_BLR1                 (DMA_CH1_BASE+0x014)       // 32bit dma ch1 burst lenght reg
#define DMA_RTOR1                (DMA_CH1_BASE+0x018)       // 32bit dma ch1 req time out reg
#define DMA_BUCR1                (DMA_CH1_BASE+0x018)       // 32bit dma ch1 bus utilization reg
#define DMA_CCNR1                (DMA_CH1_BASE+0x01C)       // 32bit dma ch1 counter reg

#define DMA_SAR2                 (DMA_CH2_BASE+0x000)       // 32bit dma ch2 source addr reg
#define DMA_DAR2                 (DMA_CH2_BASE+0x004)       // 32bit dma ch2 dest addr reg
#define DMA_CNTR2                (DMA_CH2_BASE+0x008)       // 32bit dma ch2 count reg
#define DMA_CCR2                 (DMA_CH2_BASE+0x00C)       // 32bit dma ch2 control reg
#define DMA_RSSR2                (DMA_CH2_BASE+0x010)       // 32bit dma ch2 req source sel reg
#define DMA_BLR2                 (DMA_CH2_BASE+0x014)       // 32bit dma ch2 burst lenght reg
#define DMA_RTOR2                (DMA_CH2_BASE+0x018)       // 32bit dma ch2 req time out reg
#define DMA_BUCR2                (DMA_CH2_BASE+0x018)       // 32bit dma ch2 bus utilization reg
#define DMA_CCNR2                (DMA_CH2_BASE+0x01C)       // 32bit dma ch2 counter reg

#define DMA_SAR3                 (DMA_CH3_BASE+0x000)       // 32bit dma ch3 source addr reg
#define DMA_DAR3                 (DMA_CH3_BASE+0x004)       // 32bit dma ch3 dest addr reg
#define DMA_CNTR3                (DMA_CH3_BASE+0x008)       // 32bit dma ch3 count reg
#define DMA_CCR3                 (DMA_CH3_BASE+0x00C)       // 32bit dma ch3 control reg
#define DMA_RSSR3                (DMA_CH3_BASE+0x010)       // 32bit dma ch3 req source sel reg
#define DMA_BLR3                 (DMA_CH3_BASE+0x014)       // 32bit dma ch3 burst lenght reg
#define DMA_RTOR3                (DMA_CH3_BASE+0x018)       // 32bit dma ch3 req time out reg
#define DMA_BUCR3                (DMA_CH3_BASE+0x018)       // 32bit dma ch3 bus utilization reg
#define DMA_CCNR3                (DMA_CH3_BASE+0x01C)       // 32bit dma ch3 counter reg

#define DMA_SAR4                 (DMA_CH4_BASE+0x000)       // 32bit dma ch4 source addr reg
#define DMA_DAR4                 (DMA_CH4_BASE+0x004)       // 32bit dma ch4 dest addr reg
#define DMA_CNTR4                (DMA_CH4_BASE+0x008)       // 32bit dma ch4 count reg
#define DMA_CCR4                 (DMA_CH4_BASE+0x00C)       // 32bit dma ch4 control reg
#define DMA_RSSR4                (DMA_CH4_BASE+0x010)       // 32bit dma ch4 req source sel reg
#define DMA_BLR4                 (DMA_CH4_BASE+0x014)       // 32bit dma ch4 burst lenght reg
#define DMA_RTOR4                (DMA_CH4_BASE+0x018)       // 32bit dma ch4 req time out reg
#define DMA_BUCR4                (DMA_CH4_BASE+0x018)       // 32bit dma ch4 bus utilization reg
#define DMA_CCNR4                (DMA_CH4_BASE+0x01C)       // 32bit dma ch4 counter reg

#define DMA_SAR5                 (DMA_CH5_BASE+0x000)       // 32bit dma ch5 source addr reg
#define DMA_DAR5                 (DMA_CH5_BASE+0x004)       // 32bit dma ch5 dest addr reg
#define DMA_CNTR5                (DMA_CH5_BASE+0x008)       // 32bit dma ch5 count reg
#define DMA_CCR5                 (DMA_CH5_BASE+0x00C)       // 32bit dma ch5 control reg
#define DMA_RSSR5                (DMA_CH5_BASE+0x010)       // 32bit dma ch5 req source sel reg
#define DMA_BLR5                 (DMA_CH5_BASE+0x014)       // 32bit dma ch5 burst lenght reg
#define DMA_RTOR5                (DMA_CH5_BASE+0x018)       // 32bit dma ch5 req time out reg
#define DMA_BUCR5                (DMA_CH5_BASE+0x018)       // 32bit dma ch5 bus utilization reg
#define DMA_CCNR5                (DMA_CH5_BASE+0x01C)       // 32bit dma ch5 counter reg

#define DMA_SAR6                 (DMA_CH6_BASE+0x000)       // 32bit dma ch6 source addr reg
#define DMA_DAR6                 (DMA_CH6_BASE+0x004)       // 32bit dma ch6 dest addr reg
#define DMA_CNTR6                (DMA_CH6_BASE+0x008)       // 32bit dma ch6 count reg
#define DMA_CCR6                 (DMA_CH6_BASE+0x00C)       // 32bit dma ch6 control reg
#define DMA_RSSR6                (DMA_CH6_BASE+0x010)       // 32bit dma ch6 req source sel reg
#define DMA_BLR6                 (DMA_CH6_BASE+0x014)       // 32bit dma ch6 burst lenght reg
#define DMA_RTOR6                (DMA_CH6_BASE+0x018)       // 32bit dma ch6 req time out reg
#define DMA_BUCR6                (DMA_CH6_BASE+0x018)       // 32bit dma ch6 bus utilization reg
#define DMA_CCNR6                (DMA_CH6_BASE+0x01C)       // 32bit dma ch6 counter reg

#define DMA_SAR7                 (DMA_CH7_BASE+0x000)       // 32bit dma ch7 source addr reg
#define DMA_DAR7                 (DMA_CH7_BASE+0x004)       // 32bit dma ch7 dest addr reg
#define DMA_CNTR7                (DMA_CH7_BASE+0x008)       // 32bit dma ch7 count reg
#define DMA_CCR7                 (DMA_CH7_BASE+0x00C)       // 32bit dma ch7 control reg
#define DMA_RSSR7                (DMA_CH7_BASE+0x010)       // 32bit dma ch7 req source sel reg
#define DMA_BLR7                 (DMA_CH7_BASE+0x014)       // 32bit dma ch7 burst lenght reg
#define DMA_RTOR7                (DMA_CH7_BASE+0x018)       // 32bit dma ch7 req time out reg
#define DMA_BUCR7                (DMA_CH7_BASE+0x018)       // 32bit dma ch7 bus utilization reg
#define DMA_CCNR7                (DMA_CH7_BASE+0x01C)       // 32bit dma ch7 counter reg

#define DMA_SAR8                 (DMA_CH8_BASE+0x000)       // 32bit dma ch8 source addr reg
#define DMA_DAR8                 (DMA_CH8_BASE+0x004)       // 32bit dma ch8 dest addr reg
#define DMA_CNTR8                (DMA_CH8_BASE+0x008)       // 32bit dma ch8 count reg
#define DMA_CCR8                 (DMA_CH8_BASE+0x00C)       // 32bit dma ch8 control reg
#define DMA_RSSR8                (DMA_CH8_BASE+0x010)       // 32bit dma ch8 req source sel reg
#define DMA_BLR8                 (DMA_CH8_BASE+0x014)       // 32bit dma ch8 burst lenght reg
#define DMA_RTOR8                (DMA_CH8_BASE+0x018)       // 32bit dma ch8 req time out reg
#define DMA_BUCR8                (DMA_CH8_BASE+0x018)       // 32bit dma ch8 bus utilization reg
#define DMA_CCNR8                (DMA_CH8_BASE+0x01C)       // 32bit dma ch8 counter reg

#define DMA_SAR9                 (DMA_CH9_BASE+0x000)       // 32bit dma ch9 source addr reg
#define DMA_DAR9                 (DMA_CH9_BASE+0x004)       // 32bit dma ch9 dest addr reg
#define DMA_CNTR9                (DMA_CH9_BASE+0x008)       // 32bit dma ch9 count reg
#define DMA_CCR9                 (DMA_CH9_BASE+0x00C)       // 32bit dma ch9 control reg
#define DMA_RSSR9                (DMA_CH9_BASE+0x010)       // 32bit dma ch9 req source sel reg
#define DMA_BLR9                 (DMA_CH9_BASE+0x014)       // 32bit dma ch9 burst lenght reg
#define DMA_RTOR9                (DMA_CH9_BASE+0x018)       // 32bit dma ch9 req time out reg
#define DMA_BUCR9                (DMA_CH9_BASE+0x018)       // 32bit dma ch9 bus utilization reg
#define DMA_CCNR9                (DMA_CH9_BASE+0x01C)       // 32bit dma ch9 counter reg

#define DMA_SAR10                (DMA_CH10_BASE+0x000)      // 32bit dma ch10 source addr reg
#define DMA_DAR10                (DMA_CH10_BASE+0x004)      // 32bit dma ch10 dest addr reg
#define DMA_CNTR10               (DMA_CH10_BASE+0x008)      // 32bit dma ch10 count reg
#define DMA_CCR10                (DMA_CH10_BASE+0x00C)      // 32bit dma ch10 control reg
#define DMA_RSSR10               (DMA_CH10_BASE+0x010)      // 32bit dma ch10 req source sel reg
#define DMA_BLR10                (DMA_CH10_BASE+0x014)      // 32bit dma ch10 burst lenght reg
#define DMA_RTOR10               (DMA_CH10_BASE+0x018)      // 32bit dma ch10 req time out reg
#define DMA_BUCR10               (DMA_CH10_BASE+0x018)      // 32bit dma ch10 bus utilization reg
#define DMA_CCNR10               (DMA_CH10_BASE+0x01C)      // 32bit dma ch10 counter reg

#define DMA_SAR11                (DMA_CH11_BASE+0x000)      // 32bit dma ch11 source addr reg
#define DMA_DAR11                (DMA_CH11_BASE+0x004)      // 32bit dma ch11 dest addr reg
#define DMA_CNTR11               (DMA_CH11_BASE+0x008)      // 32bit dma ch11 count reg
#define DMA_CCR11                (DMA_CH11_BASE+0x00C)      // 32bit dma ch11 control reg
#define DMA_RSSR11               (DMA_CH11_BASE+0x010)      // 32bit dma ch11 req source sel reg
#define DMA_BLR11                (DMA_CH11_BASE+0x014)      // 32bit dma ch11 burst lenght reg
#define DMA_RTOR11               (DMA_CH11_BASE+0x018)      // 32bit dma ch11 req time out reg
#define DMA_BUCR11               (DMA_CH11_BASE+0x018)      // 32bit dma ch11 bus utilization reg
#define DMA_CCNR11               (DMA_CH11_BASE+0x01C)      // 32bit dma ch11 counter reg

#define DMA_SAR12                (DMA_CH12_BASE+0x000)      // 32bit dma ch12 source addr reg
#define DMA_DAR12                (DMA_CH12_BASE+0x004)      // 32bit dma ch12 dest addr reg
#define DMA_CNTR12               (DMA_CH12_BASE+0x008)      // 32bit dma ch12 count reg
#define DMA_CCR12                (DMA_CH12_BASE+0x00C)      // 32bit dma ch12 control reg
#define DMA_RSSR12               (DMA_CH12_BASE+0x010)      // 32bit dma ch12 req source sel reg
#define DMA_BLR12                (DMA_CH12_BASE+0x014)      // 32bit dma ch12 burst lenght reg
#define DMA_RTOR12               (DMA_CH12_BASE+0x018)      // 32bit dma ch12 req time out reg
#define DMA_BUCR12               (DMA_CH12_BASE+0x018)      // 32bit dma ch12 bus utilization reg
#define DMA_CCNR12               (DMA_CH12_BASE+0x01C)      // 32bit dma ch12 counter reg

#define DMA_SAR13                (DMA_CH13_BASE+0x000)      // 32bit dma ch13 source addr reg
#define DMA_DAR13                (DMA_CH13_BASE+0x004)      // 32bit dma ch13 dest addr reg
#define DMA_CNTR13               (DMA_CH13_BASE+0x008)      // 32bit dma ch13 count reg
#define DMA_CCR13                (DMA_CH13_BASE+0x00C)      // 32bit dma ch13 control reg
#define DMA_RSSR13               (DMA_CH13_BASE+0x010)      // 32bit dma ch13 req source sel reg
#define DMA_BLR13                (DMA_CH13_BASE+0x014)      // 32bit dma ch13 burst lenght reg
#define DMA_RTOR13               (DMA_CH13_BASE+0x018)      // 32bit dma ch13 req time out reg
#define DMA_BUCR13               (DMA_CH13_BASE+0x018)      // 32bit dma ch13 bus utilization reg
#define DMA_CCNR13               (DMA_CH13_BASE+0x01C)      // 32bit dma ch13 counter reg

#define DMA_SAR14                (DMA_CH14_BASE+0x000)      // 32bit dma ch14 source addr reg
#define DMA_DAR14                (DMA_CH14_BASE+0x004)      // 32bit dma ch14 dest addr reg
#define DMA_CNTR14               (DMA_CH14_BASE+0x008)      // 32bit dma ch14 count reg
#define DMA_CCR14                (DMA_CH14_BASE+0x00C)      // 32bit dma ch14 control reg
#define DMA_RSSR14               (DMA_CH14_BASE+0x010)      // 32bit dma ch14 req source sel reg
#define DMA_BLR14                (DMA_CH14_BASE+0x014)      // 32bit dma ch14 burst lenght reg
#define DMA_RTOR14               (DMA_CH14_BASE+0x018)      // 32bit dma ch14 req time out reg
#define DMA_BUCR14               (DMA_CH14_BASE+0x018)      // 32bit dma ch14 bus utilization reg
#define DMA_CCNR14               (DMA_CH14_BASE+0x01C)      // 32bit dma ch14 counter reg

#define DMA_SAR15                (DMA_CH15_BASE+0x000)      // 32bit dma ch15 source addr reg
#define DMA_DAR15                (DMA_CH15_BASE+0x004)      // 32bit dma ch15 dest addr reg
#define DMA_CNTR15               (DMA_CH15_BASE+0x008)      // 32bit dma ch15 count reg
#define DMA_CCR15                (DMA_CH15_BASE+0x00C)      // 32bit dma ch15 control reg
#define DMA_RSSR15               (DMA_CH15_BASE+0x010)      // 32bit dma ch15 req source sel reg
#define DMA_BLR15                (DMA_CH15_BASE+0x014)      // 32bit dma ch15 burst lenght reg
#define DMA_RTOR15               (DMA_CH15_BASE+0x018)      // 32bit dma ch15 req time out reg
#define DMA_BUCR15               (DMA_CH15_BASE+0x018)      // 32bit dma ch15 bus utilization reg
#define DMA_CCNR15               (DMA_CH15_BASE+0x01C)      // 32bit dma ch15 counter reg

#define DMA_TCR                  (DMA_TEST_BASE+0x000)      // 32bit dma test control reg
#define DMA_TFIFOAR              (DMA_TEST_BASE+0x004)      // 32bit dma test fifo A reg
#define DMA_TDIPR                (DMA_TEST_BASE+0x00C)      // 32bit dma test in progress reg
#define DMA_TFIFOBR              (DMA_TEST_BASE+0x010)      // 32bit dma test fifo B reg
#define DMA_TDRR_L               (DMA_TEST_BASE+0x018)      // 64bit dma test request reg low 32bit
#define DMA_TDRR_H               (DMA_TEST_BASE+0x01C)      // 64bit dma test request reg high 32bit


//------------------------------------------------------------------------------
// REGISTER BIT FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
// DCR
#define DMA_DCR_DEN_LSH                0
#define DMA_DCR_DRST_LSH               1
#define DMA_DCR_DAM_LSH                2

// DBTOCR
#define DMA_DBTOCR_CNT_LSH             0
#define DMA_DBTOCR_EN_LSH              15

// WSRx
#define DMA_WSR_WS_LSH                 0

// XSRx
#define DMA_XSR_XS_LSH                 0

// YSRx
#define DMA_YSR_YS_LSH                 0

// Channel registers
// SAR
#define DMA_SAR_SA_LSH                 0

// DAR
#define DMA_DAR_DA_LSH                 0

// CNTR
#define DMA_CNTR_CNT_LSH               0

// CCR
#define DMA_CCR_CEN_LSH                0
#define DMA_CCR_FRC_LSH                1
#define DMA_CCR_RPT_LSH                2
#define DMA_CCR_REN_LSH                3
#define DMA_CCR_SSIZ_LSH               4
#define DMA_CCR_DSIZ_LSH               6
#define DMA_CCR_MSEL_LSH               8
#define DMA_CCR_MDIR_LSH               9
#define DMA_CCR_SMOD_LSH               10
#define DMA_CCR_DMOD_LSH               12
#define DMA_CCR_ACRPT_LSH              14

// RSSR
#define DMA_RSSR_RSS_LSH               0

// BLR
#define DMA_BLR_BL_LSH                 0

// RTOR
#define DMA_RTOR_CNT_LSH               0
#define DMA_RTOR_PSC_LSH               13
#define DMA_RTOR_CLK_LSH               14
#define DMA_RTOR_EN_LSH                15

// BUCR
#define DMA_BUCR_BU_CNT_LSH            0

// CCNR
#define DMA_CCNR_CCNR_LSH              0

//------------------------------------------------------------------------------
// REGISTER BIT FIELD WIDTHS
//------------------------------------------------------------------------------
// DCR
#define DMA_DCR_DEN_WID                Width_1
#define DMA_DCR_DRST_WID               Width_1
#define DMA_DCR_DAM_WID                Width_1

// DBTOCR
#define DMA_DBTOCR_CNT_WID             Width_15
#define DMA_DBTOCR_EN_WID              Width_1

// WSRx
#define DMA_WSR_WS_WID                 Width_16

// XSRx
#define DMA_XSR_XS_WID                 Width_16

// YSRx
#define DMA_YSR_YS_WID                 Width_16

// Channel registers
// SAR
#define DMA_SAR_SA_WID                 Width_32

// DAR
#define DMA_DAR_DA_WID                 Width_32

// CNTR
#define DMA_CNTR_CNT_WID               Width_24

// CCR
#define DMA_CCR_CEN_WID                Width_1
#define DMA_CCR_FRC_WID                Width_1
#define DMA_CCR_RPT_WID                Width_1
#define DMA_CCR_REN_WID                Width_1
#define DMA_CCR_SSIZ_WID               Width_2
#define DMA_CCR_DSIZ_WID               Width_2
#define DMA_CCR_MSEL_WID               Width_1
#define DMA_CCR_MDIR_WID               Width_1
#define DMA_CCR_SMOD_WID               Width_2
#define DMA_CCR_DMOD_WID               Width_2
#define DMA_CCR_ACRPT_WID              Width_1

// RSSR
#define DMA_RSSR_RSS_WID               Width_6

// BLR
#define DMA_BLR_BL_WID                 Width_6

// RTOR
#define DMA_RTOR_CNT_WID               Width_13
#define DMA_RTOR_PSC_WID               Width_1
#define DMA_RTOR_CLK_WID               Width_1
#define DMA_RTOR_EN_WID                Width_1

// BUCR
#define DMA_BUCR_BU_CNT_WID            Width_16

// CCNR
#define DMA_CCNR_CCNR_WID              Width_24


//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------
// DCR
#define DMA_DCR_DEN_DISABLE            0
#define DMA_DCR_DEN_ENABLE             1
#define DMA_DCR_DRST_NOEFFECT          0
#define DMA_DCR_DRST_RESET             1
#define DMA_DCR_DAM_PRIVILEGED         0
#define DMA_DCR_DAM_USER               1

// DBTOCR
#define DMA_DBTOCR_EN_DISABLE          0
#define DMA_DBTOCR_EN_ENABLE           1

// Channel registers
// CCR
#define DMA_CCR_CEN_DISABLE            0
#define DMA_CCR_CEN_ENABLE             1
#define DMA_CCR_FRC_NOEFFECT           0
#define DMA_CCR_FRC_FORCE_DMA_CYCLE    1
#define DMA_CCR_RPT_DISABLE            0
#define DMA_CCR_RPT_ENABLE             1
#define DMA_CCR_REN_DISABLE            0
#define DMA_CCR_REN_ENABLE             1
#define DMA_CCR_SSIZ_32BIT             0 
#define DMA_CCR_SSIZ_8BIT              1
#define DMA_CCR_SSIZ_16BIT             2
#define DMA_CCR_DSIZ_32BIT             0 
#define DMA_CCR_DSIZ_8BIT              1
#define DMA_CCR_DSIZ_16BIT             2
#define DMA_CCR_MSEL_2DMEM_SETA        0
#define DMA_CCR_MSEL_2DMEM_SETB        1
#define DMA_CCR_MDIR_INCREMENT         0
#define DMA_CCR_MDIR_DECREMENT         1
#define DMA_CCR_SMOD_LINEAR            0
#define DMA_CCR_SMOD_2DMEM             1
#define DMA_CCR_SMOD_FIFO              2
#define DMA_CCR_DMOD_LINEAR            0
#define DMA_CCR_DMOD_2DMEM             1
#define DMA_CCR_DMOD_FIFO              2
#define DMA_CCR_ACRPT_DISABLE          0
#define DMA_CCR_ACRPT_AUTOCLEAR_RPT    1

// BLR
#define DMA_BLR_BL_64_BYTES            0

// RTOR
#define DMA_RTOR_PSC_DIV1              0
#define DMA_RTOR_PSC_DIV256            1
#define DMA_RTOR_CLK_HCLK              0
#define DMA_RTOR_CLK_32KHZ_REF         1
#define DMA_RTOR_EN_DISABLE            0
#define DMA_RTOR_EN_ENABLE             1


#endif /*IMX27_DMA_H_*/


