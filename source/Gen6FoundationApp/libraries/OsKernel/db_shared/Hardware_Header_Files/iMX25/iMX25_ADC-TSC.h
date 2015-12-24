/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  HARDWARE DEFINITION FILE 
//  Date:       9/16/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: db_shared/Hardware_Header_Files/iMX25/iMX25_ADC-TSC.h $
// $Revision: 1.3 $
// $Date: 2011/06/28 13:26:54EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx25_adc-tsc.h
//
// Provides definitions for The Touch Screen controller and 
//	associated Analog to Digital Converter (ADC)
// Refer to Chapter 62 of the iMX25 Reference Manual
//
//-----------------------------------------------------------------------------

#ifndef IMX25_ADCTSC_H_
#define IMX25_ADCTSC_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------
// #########################################
// # TSC                               #
// #########################################
#define ADC_BASE_ADDR              	0x50030000U
#define ADC_TGCR	               	(ADC_BASE_ADDR+0x000U) //TSC General Config Register (TGCR)
#define ADC_TGSR	               	(ADC_BASE_ADDR+0x004U) //TSC General Status Register (TGSR)
#define ADC_TCQFIFO	               	(ADC_BASE_ADDR+0x400U) //Queue FIFO(TCQFIFO)
#define ADC_GCQFIFO	               	(ADC_BASE_ADDR+0x800U) //Queue FIFO(GCQFIFO)
#define ADC_TCQCR	               	(ADC_BASE_ADDR+0x404U) //Queue FIFO(TCQFIFO)
#define ADC_GCQCR	               	(ADC_BASE_ADDR+0x804U) //Queue FIFO(GCQFIFO)
#define ADC_TCQSR	               	(ADC_BASE_ADDR+0x408U) //Queue Status Register(TCQSR)
#define ADC_GCQSR	               	(ADC_BASE_ADDR+0x808U) //Queue Status Register(GCQSR)
#define ADC_TCQMR	               	(ADC_BASE_ADDR+0x40CU) //Queue Mask Register(TCQMR)
#define ADC_GCQMR	               	(ADC_BASE_ADDR+0x80CU) //Queue Mask Register(GCQMR)
	
#define ADC_TCQITEM_7_0	           	(ADC_BASE_ADDR+0x420U) //TCQ ITEM Register(TCQITEM_7_0)
#define ADC_TCQITEM_15_8           	(ADC_BASE_ADDR+0x424U) //TCQ ITEM Register(TCQITEM_15_8)

#define ADC_GCQITEM_7_0	           	(ADC_BASE_ADDR+0x820U) //GCQ ITEM Register(GCQITEM_7_0)
#define ADC_GCQITEM_15_8           	(ADC_BASE_ADDR+0x824U) //GCQ ITEM Register(GCQITEM_15_8)

#define ADC_TICR					(ADC_BASE_ADDR+0x008U) //Convert Config (TICR/TCC0~TCC7/GCC0~GCC7)
#define ADC_TCC0					(ADC_BASE_ADDR+0x440U) //Convert Config (TICR/TCC0~TCC7/GCC0~GCC7)
#define ADC_TCC1					(ADC_BASE_ADDR+0x444U) //Convert Config (TICR/TCC0~TCC7/GCC0~GCC7)
#define ADC_TCC2					(ADC_BASE_ADDR+0x448U) //Convert Config (TICR/TCC0~TCC7/GCC0~GCC7)
#define ADC_TCC3					(ADC_BASE_ADDR+0x44CU) //Convert Config (TICR/TCC0~TCC7/GCC0~GCC7)
#define ADC_TCC4					(ADC_BASE_ADDR+0x450U) //Convert Config (TICR/TCC0~TCC7/GCC0~GCC7)
#define ADC_TCC5					(ADC_BASE_ADDR+0x454U) //Convert Config (TICR/TCC0~TCC7/GCC0~GCC7)
#define ADC_TCC6					(ADC_BASE_ADDR+0x458U) //Convert Config (TICR/TCC0~TCC7/GCC0~GCC7)
#define ADC_TCC7					(ADC_BASE_ADDR+0x45CU) //Convert Config (TICR/TCC0~TCC7/GCC0~GCC7)

#define ADC_GCC0					(ADC_BASE_ADDR+0x840U) //Convert Config (TICR/TCC0~TCC7/GCC0~GCC7)
#define ADC_GCC1					(ADC_BASE_ADDR+0x844U) //Convert Config (TICR/TCC0~TCC7/GCC0~GCC7)
#define ADC_GCC2					(ADC_BASE_ADDR+0x848U) //Convert Config (TICR/TCC0~TCC7/GCC0~GCC7)
#define ADC_GCC3					(ADC_BASE_ADDR+0x84CU) //Convert Config (TICR/TCC0~TCC7/GCC0~GCC7)
#define ADC_GCC4					(ADC_BASE_ADDR+0x850U) //Convert Config (TICR/TCC0~TCC7/GCC0~GCC7)
#define ADC_GCC5					(ADC_BASE_ADDR+0x854U) //Convert Config (TICR/TCC0~TCC7/GCC0~GCC7)
#define ADC_GCC6					(ADC_BASE_ADDR+0x858U) //Convert Config (TICR/TCC0~TCC7/GCC0~GCC7)
#define ADC_GCC7					(ADC_BASE_ADDR+0x85CU) //Convert Config (TICR/TCC0~TCC7/GCC0~GCC7)

//------------------------------------------------------------------------------
// REGISTER FIELD POSITIONS (LEFT SHIFT)
// The name CQ was used inplace of TCQ or GCQ, to reduce number of defines
//------------------------------------------------------------------------------
//TSC General Config Register (TGCR)
#define ADC_TGCR_IPG_CLK_EN_LSH		0			
#define ADC_TGCR_TSC_RST_LSH		1
#define ADC_TGCR_FUNC_RST_LSH		2
#define ADC_TGCR_SLPC_LSH			4
#define ADC_TGCR_STLC_LSH			5
#define ADC_TGCR_HSYNCEN_LSH		6
#define ADC_TGCR_HSYNCPOL_LSH		7
#define ADC_TGCR_POWERMODE_LSH		8
#define ADC_TGCR_INTREFEN_LSH		10
#define ADC_TGCR_ADCCLKCFG_LSH		16
#define ADC_TGCR_PDEN_LSH			23
#define ADC_TGCR_PDBEN_LSH			24
#define ADC_TGCR_PDBTIME_LSH		25

//TSC General Status Register (TGSR)
#define ADC_TGSR_TCQ_INT_LSH		0
#define ADC_TGSR_GCQ_INT_LSH		1
#define ADC_TGSR_SLP_INT_LSH		2
#define ADC_TGSR_TCQ_DMA_LSH		16
#define ADC_TGSR_GCQ_DMA_LSH		17

//Queue FIFO(TCQFIFO/GCQFIFO)
#define ADC_CQFIFO_ITEM_ID_LSH		0
#define ADC_CQFIFO_ADCDATAOUT_LSH	4

//Queue Control Register(TCQCR/GCQCR)
#define ADC_CQCR_QSM_LSH			0
#define ADC_CQCR_FQS_LSH			2
#define ADC_CQCR_RPT_LSH			3
#define ADC_CQCR_LAST_ITEM_ID_LSH	4
#define ADC_CQCR_FIFOWATERMARK_LSH	8
#define ADC_CQCR_REPEATWAIT_LSH		12
#define ADC_CQCR_QRST_LSH			16
#define ADC_CQCR_FRST_LSH			17
#define ADC_CQCR_PDMSK_LSH			18
#define ADC_CQCR_PDCFG_LSH			19

//Queue Status Register(TCQSR/GCQSR)
#define ADC_CQSR_PD_LSH				0
#define ADC_CQSR_EOQ_LSH			1
#define ADC_CQSR_FOR_LSH			4
#define ADC_CQSR_FUR_LSH			5
#define ADC_CQSR_FER_LSH			6
#define ADC_CQSR_FDN_LSH			8
#define ADC_CQSR_EMPT_LSH			13
#define ADC_CQSR_FULL_LSH			14
#define ADC_CQSR_FDRY_LSH			15
#define ADC_CQSR_FRPTR_LSH			16
#define ADC_CQSR_FWPTR_LSH			21

//Queue Mask Register(TCQMR/GCQMR)
#define ADC_CQMR_PD_IRQ_MASK_LSH	0
#define ADC_CQMR_EOQ_IRQ_MASK_LSH	1
#define ADC_CQMR_FOR_IRQ_MASK_LSH	4
#define ADC_CQMR_FUR_IRQ_MASK_LSH	5
#define ADC_CQMR_FER_IRQ_MASK_LSH	6
#define ADC_CQMR_FDRY_IRQ_MASK_LSH	15
#define ADC_CQMR_PD_DMA_MASK_LSH	16
#define ADC_CQMR_EOQ_DMA_MASK_LSH	17
#define ADC_CQMR_FOR_DMA_MASK_LSH	20
#define ADC_CQMR_FUR_DMA_MASK_LSH	21
#define ADC_CQMR_FER_DMA_MASK_LSH	22
#define ADC_CQMR_FDRY_DMA_MASK_LSH	31

//TCQ ITEM Register(TCQITEM_7_0/TCQITEM_15_8)
#define ADC_CQITEM_ITEM0_LSH		0
#define ADC_CQITEM_ITEM1_LSH		4
#define ADC_CQITEM_ITEM2_LSH		8
#define ADC_CQITEM_ITEM3_LSH		12
#define ADC_CQITEM_ITEM4_LSH		16
#define ADC_CQITEM_ITEM5_LSH		20
#define ADC_CQITEM_ITEM6_LSH		24
#define ADC_CQITEM_ITEM7_LSH		28

//Convert Config (TICR/TCC0~TCC7/GCC0~GCC7)
//xCCx was used in place of TICR, TCC0-7 and GCC0-7
#define ADC_xCCx_PENIACK_LSH		1
#define ADC_xCCx_SELREFN_LSH		2
#define ADC_xCCx_SELIN_LSH			4
#define ADC_xCCx_SELREFP_LSH		7
#define ADC_xCCx_XPULSW_LSH			9
#define ADC_xCCx_XNURSW_LSH			10
#define ADC_xCCx_YPLLSW_LSH			12
#define ADC_xCCx_YNLRSW_LSH			14
#define ADC_xCCx_WIPERSW_LSH		15
#define ADC_xCCx_NOS_LSH			16
#define ADC_xCCx_IGS_LSH			20
#define ADC_xCCx_SETTLING_TIME_LSH	24

//------------------------------------------------------------------------------
// REGISTER FIELD WIDTHS
// The name CQ was used inplace of TCQ or GCQ, to reduce number of defines
//------------------------------------------------------------------------------
//TSC General Config Register (TGCR)
#define ADC_TGCR_IPG_CLK_EN_WID		Width_1			
#define ADC_TGCR_TSC_RST_WID		Width_1
#define ADC_TGCR_FUNC_RST_WID		Width_1
#define ADC_TGCR_SLPC_WID			Width_1
#define ADC_TGCR_STLC_WID			Width_1
#define ADC_TGCR_HSYNCEN_WID		Width_1
#define ADC_TGCR_HSYNCPOL_WID		Width_1
#define ADC_TGCR_POWERMODE_WID		Width_2
#define ADC_TGCR_INTREFEN_WID		Width_1
#define ADC_TGCR_ADCCLKCFG_WID		Width_5
#define ADC_TGCR_PDEN_WID			Width_1
#define ADC_TGCR_PDBEN_WID			Width_1
#define ADC_TGCR_PDBTIME_WID		Width_7

//TSC General Status Register (TGSR)
#define ADC_TGSR_TCQ_INT_WID		Width_1
#define ADC_TGSR_GCQ_INT_WID		Width_1
#define ADC_TGSR_SLP_INT_WID		Width_1
#define ADC_TGSR_TCQ_DMA_WID		Width_1
#define ADC_TGSR_GCQ_DMA_WID		Width_1

//Queue FIFO(TCQFIFO/GCQFIFO)
#define ADC_CQFIFO_ITEM_ID_WID		Width_4
#define ADC_CQFIFO_ADCDATAOUT_WID	Width_12

//Queue Control Register(TCQCR/GCQCR)
#define ADC_CQCR_QSM_WID			Width_2
#define ADC_CQCR_FQS_WID			Width_1
#define ADC_CQCR_RPT_WID			Width_1
#define ADC_CQCR_LAST_ITEM_ID_WID	Width_4
#define ADC_CQCR_FIFOWATERMARK_WID	Width_4
#define ADC_CQCR_REPEATWAIT_WID		Width_4
#define ADC_CQCR_QRST_WID			Width_1
#define ADC_CQCR_FRST_WID			Width_1
#define ADC_CQCR_PDMSK_WID			Width_1
#define ADC_CQCR_PDCFG_WID			Width_1

//Queue Status Register(TCQSR/GCQSR)
#define ADC_CQSR_PD_WID				Width_1
#define ADC_CQSR_EOQ_WID			Width_1
#define ADC_CQSR_FOR_WID			Width_1
#define ADC_CQSR_FUR_WID			Width_1
#define ADC_CQSR_FER_WID			Width_1
#define ADC_CQSR_FDN_WID			Width_5
#define ADC_CQSR_EMPT_WID			Width_1
#define ADC_CQSR_FULL_WID			Width_1
#define ADC_CQSR_FDRY_WID			Width_1
#define ADC_CQSR_FRPTR_WID			Width_5
#define ADC_CQSR_FWPTR_WID			Width_5

//Queue Mask Register(TCQMR/GCQMR)
#define ADC_CQMR_PD_IRQ_MASK_WID	Width_1
#define ADC_CQMR_EOQ_IRQ_MASK_WID	Width_1
#define ADC_CQMR_FOR_IRQ_MASK_WID	Width_1
#define ADC_CQMR_FUR_IRQ_MASK_WID	Width_1
#define ADC_CQMR_FER_IRQ_MASK_WID	Width_1
#define ADC_CQMR_FDRY_IRQ_MASK_WID	Width_1
#define ADC_CQMR_PD_DMA_MASK_WID	Width_1
#define ADC_CQMR_EOQ_DMA_MASK_WID	Width_1
#define ADC_CQMR_FOR_DMA_MASK_WID	Width_1
#define ADC_CQMR_FUR_DMA_MASK_WID	Width_1
#define ADC_CQMR_FER_DMA_MASK_WID	Width_1
#define ADC_CQMR_FDRY_DMA_MASK_WID	Width_1

//TCQ ITEM Register(TCQITEM_7_0/TCQITEM_15_8)
#define ADC_CQITEM_ITEM0_WID		Width_4
#define ADC_CQITEM_ITEM1_WID		Width_4
#define ADC_CQITEM_ITEM2_WID		Width_4
#define ADC_CQITEM_ITEM3_WID		Width_4
#define ADC_CQITEM_ITEM4_WID		Width_4
#define ADC_CQITEM_ITEM5_WID		Width_4
#define ADC_CQITEM_ITEM6_WID		Width_4
#define ADC_CQITEM_ITEM7_WID		Width_4

//Convert Config (TICR/TCC0~TCC7/GCC0~GCC7)
//xCCx was used in place of TICR, TCC0-7 and GCC0-7
#define ADC_xCCx_PENIACK_WID		Width_1
#define ADC_xCCx_SELREFN_WID		Width_2
#define ADC_xCCx_SELIN_WID			Width_3
#define ADC_xCCx_SELREFP_WID		Width_2
#define ADC_xCCx_XPULSW_WID			Width_1
#define ADC_xCCx_XNURSW_WID			Width_2
#define ADC_xCCx_YPLLSW_WID			Width_2
#define ADC_xCCx_YNLRSW_WID			Width_1
#define ADC_xCCx_WIPERSW_WID		Width_1
#define ADC_xCCx_NOS_WID			Width_4
#define ADC_xCCx_IGS_WID			Width_1
#define ADC_xCCx_SETTLING_TIME_WID	Width_8


//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------


#endif /*IMX25_ADCTSC_H_*/


