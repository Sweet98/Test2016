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
// $RCSfile: db_shared/Hardware_Header_Files/iMX25/iMX25_CSI.h $
// $Revision: 1.4 $
// $Date: 2011/06/28 13:26:43EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx25_csi.h
//
// Provides definitions for the CMOS Sensor Interface (CSI)
// Refer to Chapter 56 of the iMX25 Reference Manual
//
//------------------------------------------------------------------------------
#ifndef IMX25_CSI_H_
#define IMX25_CSI_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------
// #########################################
// # CSI                                   #
// #########################################
#define CSI_BASE_ADDR              	0x53FF8000U
#define CSI_CSICR1               	(CSI_BASE_ADDR+0x00U)       // 32bit CSI control 1 reg
#define CSI_CSICR2               	(CSI_BASE_ADDR+0x04U)       // 32bit CSI control 2 reg
#define CSI_CSICR3               	(CSI_BASE_ADDR+0x08U)       // 32bit CSI control 3 reg
#define CSI_CSISTATFIFO            	(CSI_BASE_ADDR+0x0CU)       // 32bit CSI StatFIFO Register
#define CSI_CSIRFIFO	            (CSI_BASE_ADDR+0x10U)       // 32bit CSI RxFIFO Register
#define CSI_CSIRXCNT             	(CSI_BASE_ADDR+0x14U)       // 32bit CSI receive count reg
#define CSI_CSISR                	(CSI_BASE_ADDR+0x18U)       // 32bit CSI status reg
#define CSI_CSIDBG       	       	(CSI_BASE_ADDR+0x1CU)       // 32bit CSI debug reg
#define CSI_CSIDMASA_STATFIFO      	(CSI_BASE_ADDR+0x20U)       // 32bit CSI STATFIFO DMA Start Address Register
#define CSI_CSIDMATS_STATFIFO      	(CSI_BASE_ADDR+0x24U)       // 32bit CSI STATFIFO DMA Transfer Size Register
#define CSI_CSIDMASA_FB1	      	(CSI_BASE_ADDR+0x28U)       // 32bit CSI Frame Buffer1 DMA Start Address Register
#define CSI_CSIDMASA_FB2	      	(CSI_BASE_ADDR+0x2CU)       // 32bit CSI Frame Buffer2 DMA Start Address Register
#define CSI_CSIFBUF_PARA	      	(CSI_BASE_ADDR+0x30U)       // 32bit CSI Frame Buffer Parameter Register
#define CSI_CSIIMAG_PARA	      	(CSI_BASE_ADDR+0x34U)       // 32bit CSI Image Parameter Register


//------------------------------------------------------------------------------
// REGISTER FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
// CSICR1 : CSI Control Register 1
#define CSI_CSICR1_PIXEL_BIT_LSH					0
#define CSI_CSICR1_REDGE_LSH						1
#define CSI_CSICR1_INV_PCLK_LSH					2
#define CSI_CSICR1_INV_DATA_LSH		 			3
#define CSI_CSICR1_GCLK_MODE_LSH					4
#define CSI_CSICR1_CLR_RXFIFO_LSH				5
#define CSI_CSICR1_CLR_STATFIFO_LSH				6
#define CSI_CSICR1_PACK_DIR_LSH					7
#define CSI_CSICR1_FCC_LSH							8
#define CSI_CSICR1_MCLKEN_LSH						9
#define CSI_CSICR1_CCIR_EN_LSH					10
#define CSI_CSICR1_HSYNC_POL_LSH					11
#define CSI_CSICR1_MCLKDIV_LSH					12
#define CSI_CSICR1_SOF_INTEN_LSH					16
#define CSI_CSICR1_SOF_POL_LSH					17
#define CSI_CSICR1_RXFF_INTEN_LSH				18
#define CSI_CSICR1_FB1_DMA_DONE_INTEN_LSH		19
#define CSI_CSICR1_FB2_DMA_DONE_INTEN_LSH		20
#define CSI_CSICR1_STATFF_INTEN_LSH				21
#define CSI_CSICR1_SFF_DMA_DONE_INTEN_LSH		22
#define CSI_CSICR1_RF_OR_INTEN_LSH				24
#define CSI_CSICR1_SF_OR_INTEN_LSH				25
#define CSI_CSICR1_COF_INT_EN_LSH				26
#define CSI_CSICR1_CCIR_MODE_LSH					27
#define CSI_CSICR1_PRP_IF_EN_LSH					28
#define CSI_CSICR1_EOF_INT_EN_LSH				29
#define CSI_CSICR1_EXT_VSYNC_LSH					30
#define CSI_CSICR1_SWAP16_EN_LSH					31

//CSICR2 : CSI Control Register 2
#define CSI_CSICR2_LVRM_LSH						16
#define CSI_CSICR2_HSC_LSH							0
#define CSI_CSICR2_VSC_LSH							8
#define CSI_CSICR2_BTS_LSH							19
#define CSI_CSICR2_SCE_LSH							23
#define CSI_CSICR2_AFS_LSH							24
#define CSI_CSICR2_DRM_LSH							26
#define CSI_CSICR2_DMA_BURST_TYPE_SFF_LSH		28
#define CSI_CSICR2_DMA_BURST_TYPE_RFF_LSH		30

//CSICR3 : CSI Control Register 3
#define CSI_CSICR3_ECC_AUTO_EN_LSH			0
#define CSI_CSICR3_ECC_INT_EN_LSH			1
#define CSI_CSICR3_ZERO_PACK_EN_LSH			2

#define CSI_CSICR3_TWO_8BIT_SENSOR_LSH		3
#define CSI_CSICR3_RxFF_LEVEL_LSH			4
#define CSI_CSICR3_HRESP_ERR_EN_LSH			7
#define CSI_CSICR3_STATFF_LEVEL_LSH			8
#define CSI_CSICR3_DMA_REQ_EN_SFF_LSH		11
#define CSI_CSICR3_DMA_REQ_EN_RFF_LSH		12
#define CSI_CSICR3_DMA_REFLASH_SFF_LSH		13
#define CSI_CSICR3_DMA_REFLASH_RFF_LSH		14

#define CSI_CSICR3_FRMCNT_RST_LSH			15
#define CSI_CSICR3_FRMCNT_LSH				16

//CSISTATFIFO : CSI Statistic FIFO Register
#define CSI_CSISTATFIFO_STAT_LSH				0

//CSIRXFIFO : CSI RxFIFO Register
#define CSI_CSIRXFIFO_IMAGE_LSH				0

//CSIRXCNT :  CSI RxFIFO Count Register
#define CSI_CSIRXCNT_RXCNT_LSH				0

//CSISR :  CSI Status Register
#define CSI_CSISR_DRDY_LSH						0
#define CSI_CSISR_ECC_INT_LSH					1
#define CSI_CSISR_HRESP_ERR_INT_LSH			7
#define CSI_CSISR_COF_INT_LSH					13
#define CSI_CSISR_F1_INT_LSH					14
#define CSI_CSISR_F2_INT_LSH					15
#define CSI_CSISR_SOF_INT_LSH					16
#define CSI_CSISR_EOF_INT_LSH					17
#define CSI_CSISR_RXFF_INT_LSH				18
#define CSI_CSISR_DMA_TSF_DONE_FB1_LSH		19
#define CSI_CSISR_DMA_TSF_DONE_FB2_LSH		20
#define CSI_CSISR_STATFF_INT_LSH				21
#define CSI_CSISR_DMA_TSF_DONE_SFF_LSH		22
#define CSI_CSISR_RF_OR_INT_LSH				24
#define CSI_CSISR_SF_OR_INT_LSH				25

//CSIDBGREG : CSI Debug Register
#define CSI_CSIDBGREG_DEBUG_SIGNAL_EN_LSH	0

//CSI STATFIFO DMA Start Address Register (CSIDMASA-STATFIFO)
#define CSI_CSIDMASA_STATFIFO_DMA_START_ADDR_SFF_LSH	2

//CSI STATFIFO DMA Transfer Size Register (CSIDMATS-STATFIFO)

//CSI Frame Buffer1 DMA Start Address Register (CSIDMASA-FB1)
#define CSI_CSIDMASA_FB1_DMA_START_ADDR_FB1_LSH			2

//CSI Frame Buffer2 DMA Start Address Register (CSIDMASA-FB2)
#define CSI_CSIDMASA_FB2_DMA_START_ADDR_FB2_LSH			2

//CSI Frame Buffer Parameter Register (CSIFBUF_PARA)
#define CSI_CSIFBUF_PARA_FBUF_STRIDE_LSH					0

//CSI Image Parameter Register (CSIIMAG_PARA)

//------------------------------------------------------------------------------
// REGISTER BIT FIELD WIDTHS
//------------------------------------------------------------------------------
// CSICR1 : CSI Control Register 1
#define CSI_CSICR1_PIXEL_BITWID					Width_1
#define CSI_CSICR1_REDGE_WID						Width_1
#define CSI_CSICR1_INV_PCLK_WID					Width_1
#define CSI_CSICR1_INV_DATA_WID		 			Width_1
#define CSI_CSICR1_GCLK_MODE_WID					Width_1
#define CSI_CSICR1_CLR_RXFIFO_WID				Width_1
#define CSI_CSICR1_CLR_STATFIFO_WID				Width_1
#define CSI_CSICR1_PACK_DIR_WID					Width_1
#define CSI_CSICR1_FCC_WID							Width_1
#define CSI_CSICR1_MCLKEN_WID						Width_1
#define CSI_CSICR1_CCIR_EN_WID					Width_1
#define CSI_CSICR1_HSYNC_POL_WID					Width_1
#define CSI_CSICR1_MCLKDIV_WID					Width_4
#define CSI_CSICR1_SOF_INTEN_WID					Width_1
#define CSI_CSICR1_SOF_POL_WID					Width_1
#define CSI_CSICR1_RXFF_INTEN_WID				Width_1
#define CSI_CSICR1_FB1_DMA_DONE_INTEN_WID		Width_1
#define CSI_CSICR1_FB2_DMA_DONE_INTEN_WID		Width_1
#define CSI_CSICR1_STATFF_INTEN_WID				Width_1
#define CSI_CSICR1_SFF_DMA_DONE_INTEN_WID		Width_1
#define CSI_CSICR1_RF_OR_INTEN_WID				Width_1
#define CSI_CSICR1_SF_OR_INTEN_WID				Width_1
#define CSI_CSICR1_COF_INT_EN_WID				Width_1
#define CSI_CSICR1_CCIR_MODE_WID					Width_1
#define CSI_CSICR1_PRP_IF_EN_WID					Width_1
#define CSI_CSICR1_EOF_INT_EN_WID				Width_1
#define CSI_CSICR1_EXT_VSYNC_WID					Width_1
#define CSI_CSICR1_SWAP16_EN_WID					Width_1

//CSICR2 : CSI Control Register 2
#define CSI_CSICR2_HSC_WID							Width_8
#define CSI_CSICR2_VSC_WID							Width_8
#define CSI_CSICR2_LVRM_WID						Width_3
#define CSI_CSICR2_BTS_WID							Width_2
#define CSI_CSICR2_SCE_WID							Width_1
#define CSI_CSICR2_AFS_WID							Width_2
#define CSI_CSICR2_DRM_WID							Width_1
#define CSI_CSICR2_DMA_BURST_TYPE_SFF_WID		Width_2
#define CSI_CSICR2_DMA_BURST_TYPE_RFF_WID		Width_2

//CSICR3 : CSI Control Register 3
#define CSI_CSICR3_ECC_AUTO_EN_WID			Width_1
#define CSI_CSICR3_ECC_INT_EN_WID			Width_1
#define CSI_CSICR3_ZERO_PACK_EN_WID			Width_1
#define CSI_CSICR3_TWO_8BIT_SENSOR_WID		Width_1
#define CSI_CSICR3_RxFF_LEVEL_WID			Width_3
#define CSI_CSICR3_HRESP_ERR_EN_WID			Width_1
#define CSI_CSICR3_STATFF_LEVEL_WID			Width_3
#define CSI_CSICR3_DMA_REQ_EN_SFF_WID		Width_1
#define CSI_CSICR3_DMA_REQ_EN_RFF_WID		Width_1
#define CSI_CSICR3_DMA_REFLASH_SFF_WID		Width_1
#define CSI_CSICR3_DMA_REFLASH_RFF_WID		Width_1
#define CSI_CSICR3_FRMCNT_RST_WID			Width_1
#define CSI_CSICR3_FRMCNT_WID					Width_16

//CSISTATFIFO : CSI Statistic FIFO Register
#define CSI_CSISTATFIFO_STAT_WID				Width_32

//CSIRXFIFO : CSI RxFIFO Register
#define CSI_CSIRXFIFO_IMAGE_WID				Width_32

//CSIRXCNT :  CSI RxFIFO Count Register
#define CSI_CSIRXCNT_RXCNT_WID				Width_22

//CSISR :  CSI Status Register
#define CSI_CSISR_DRDY_WID						Width_1
#define CSI_CSISR_ECC_INT_WID					Width_1
#define CSI_CSISR_HRESP_ERR_INT_WID			Width_1
#define CSI_CSISR_COF_INT_WID					Width_1
#define CSI_CSISR_F1_INT_WID					Width_1
#define CSI_CSISR_F2_INT_WID					Width_1
#define CSI_CSISR_SOF_INT_WID					Width_1
#define CSI_CSISR_EOF_INT_WID					Width_1
#define CSI_CSISR_RXFF_INT_WID				Width_1
#define CSI_CSISR_DMA_TSF_DONE_FB1_WID		Width_1
#define CSI_CSISR_DMA_TSF_DONE_FB2_WID		Width_1
#define CSI_CSISR_STATFF_INT_WID				Width_1
#define CSI_CSISR_DMA_TSF_DONE_SFF_WID		Width_1
#define CSI_CSISR_RF_OR_INT_WID				Width_1
#define CSI_CSISR_SF_OR_INT_WID				Width_1

//CSIDBGREG : CSI Debug Register
#define CSI_CSIDBGREG_DEBUG_SIGNAL_EN_WID	Width_1

//CSI STATFIFO DMA Start Address Register (CSIDMASA-STATFIFO)
#define CSI_CSIDMASA_STATFIFO_DMA_START_ADDR_SFF_WID	Width_30

//CSI STATFIFO DMA Transfer Size Register (CSIDMATS-STATFIFO)

//CSI Frame Buffer1 DMA Start Address Register (CSIDMASA-FB1)
#define CSI_CSIDMASA_FB1_DMA_START_ADDR_FB1_WID			Width_30

//CSI Frame Buffer2 DMA Start Address Register (CSIDMASA-FB2)
#define CSI_CSIDMASA_FB2_DMA_START_ADDR_FB2_WID			Width_30

//CSI Frame Buffer Parameter Register (CSIFBUF_PARA)
#define CSI_CSIFBUF_PARA_FBUF_STRIDE_WID				Width_30

//CSI Image Parameter Register (CSIIMAG_PARA)

//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------
// CSICR1 : CSI Control Register 1
#define CSI_CSICR1_REDGE_RISING			1
#define CSI_CSICR1_REDGE_FALLING		0

#define CSI_CSICR1_INV_PCLK_DIRECT		0
#define CSI_CSICR1_INV_PCLK_INVERT		1

#define CSI_CSICR1_INV_DATA_DIRECT		0
#define CSI_CSICR1_INV_DATA_INVERT		1

#define CSI_CSICR1_GCLK_MODE_NONGATED	0
#define CSI_CSICR1_GCLK_MODE_GATED		1

#define CSI_CSICR1_CLR_RXFIFO_CLEAR		1

#define CSI_CSICR1_CLR_STATFIFO_CLEAR	1

#define CSI_CSICR1_PACK_DIR_LSB			0
#define CSI_CSICR1_PACK_DIR_MSB			1

#define CSI_CSICR1_FCC_ASYNC			0
#define CSI_CSICR1_FCC_SYNC				1

#define CSI_CSICR1_MCLKEN_DISABLE		0
#define CSI_CSICR1_MCLKEN_ENABLE		1

#define CSI_CSICR1_CCIR_EN_TRADITIONAL	0
#define CSI_CSICR1_CCIR_EN_CCIR			1

#define CSI_CSICR1_HSYNC_POL_LOW		0
#define CSI_CSICR1_HSYNC_POL_HIGN		1

#define CSI_CSICR1_MCLKDIV_VALUE(X)		((X)/2-1)

#define CSI_CSICR1_SOF_INTEN_DISABLE	0
#define CSI_CSICR1_SOF_INTEN_ENABLE		1

#define CSI_CSICR1_SOF_POL_FALLING		0
#define CSI_CSICR1_SOF_POL_RISING		1

#define CSI_CSICR1_RXFF_INTEN_DISABLE	0
#define CSI_CSICR1_RXFF_INTEN_ENABLE	1

#define CSI_CSICR1_RXFF_LEVEL_4WORDS	0
#define CSI_CSICR1_RXFF_LEVEL_8WORDS	1
#define CSI_CSICR1_RXFF_LEVEL_16WORDS	2
#define CSI_CSICR1_RXFF_LEVEL_24WORDS	3

#define CSI_CSICR1_STATFF_INTEN_DISABLE	0
#define CSI_CSICR1_STATFF_INTEN_ENABLE	1

#define CSI_CSICR1_STATFF_LEVEL_4WORDS	0
#define CSI_CSICR1_STATFF_LEVEL_8WORDS	1
#define CSI_CSICR1_STATFF_LEVEL_12WORDS	2
#define CSI_CSICR1_STATFF_LEVEL_16WORDS	3

#define CSI_CSICR1_RF_OR_INTEN_DISABLE	0
#define CSI_CSICR1_RF_OR_INTEN_ENABLE	1

#define CSI_CSICR1_SF_OR_INTEN_DISABLE	0
#define CSI_CSICR1_SF_OR_INTEN_ENABLE	1

#define CSI_CSICR1_COF_INT_EN_DISABLE	0
#define CSI_CSICR1_COF_INT_EN_ENABLE	1

#define CSI_CSICR1_CCIR_MODE_PROGRESSIVE 0
#define CSI_CSICR1_CCIR_MODE_INTERLACE	 1

#define CSI_CSICR1_PRP_IF_EN_DISABLE	0
#define CSI_CSICR1_PRP_IF_EN_ENABLE		1

#define CSI_CSICR1_EOF_INT_EN_DISABLE	0
#define CSI_CSICR1_EOF_INT_EN_ENABLE	1

#define CSI_CSICR1_EXT_VSYNC_INTERNAL	0
#define CSI_CSICR1_EXT_VSYNC_EXTERNAL	1

#define CSI_CSICR1_SWAP16_EN_DISABLE	0
#define CSI_CSICR1_SWAP16_EN_ENABLE		1


//CSICR2 : CSI Control Register 2
#define CSI_CSICR2_HSC_VALUE(X)		(X)
#define CSI_CSICR2_VSC_VALUE(X)		(X)
#define CSI_CSICR2_LVRM_512X384		0
#define CSI_CSICR2_LVRM_448X336		1
#define CSI_CSICR2_LVRM_384X288		2
#define CSI_CSICR2_LVRM_384X256		3
#define CSI_CSICR2_LVRM_320X240		4
#define CSI_CSICR2_LVRM_288X216		5
#define CSI_CSICR2_LVRM_400X300		6

#define CSI_CSICR2_BTS_GR			0
#define CSI_CSICR2_BTS_RG			1
#define CSI_CSICR2_BTS_BG			2
#define CSI_CSICR2_BTS_GB			3

#define CSI_CSICR2_SCE_DISABLE		0
#define CSI_CSICR2_SCE_ENABLE		1

#define CSI_CSICR2_AFS_CONSECUTIVE	0
#define CSI_CSICR2_AFS_EVERYOTHER	1
#define CSI_CSICR2_AFS_EVERYFOUR	2 // 3 also OK

#define CSI_CSICR2_DRM_8X6			0
#define CSI_CSICR2_DRM_8X12			1


//CSISR :  CSI Status Register
#define CSI_CSISR_ECC_INT_W1L		1
#define CSI_CSISR_COF_INT_W1L		1
#define CSI_CSISR_SOF_INT_W1L		1
#define CSI_CSISR_EOF_INT_W1L		1
#define CSI_CSISR_RF_OR_INT_W1L		1
#define CSI_CSISR_SF_OR_INT_W1L		1

//CSISTATFIFO : CSI Statistic FIFO Register

//CSIRXFIFO : CSI RxFIFO Register

//CSIRXCNT :  CSI RxFIFO Count Register
#define CSI_CSIRXCNT_RXCNT_VALUE(X)		(X)

//CSIDBGREG : CSI Debug Register
#define CSI_CSIDBGREG_DEBUG_SIGNAL_EN_DISABLE	0
#define CSI_CSIDBGREG_DEBUG_SIGNAL_EN_ENABLE	1

//CSICR3 : CSI Control Register 3
#define CSI_CSICR3_ECC_AUTO_EN_DISABLE	0
#define CSI_CSICR3_ECC_AUTO_EN_ENABLE	1

#define CSI_CSICR3_ECC_INT_EN_DISABLE	0
#define CSI_CSICR3_ECC_INT_EN_ENABLE	1

#define CSI_CSICR3_CSI_SVR_ANYMODE		0
#define CSI_CSICR3_CSI_SVR_SVRMODE		1

#define CSI_CSICR3_ZERO_PACK_EN_DISABLE	0
#define CSI_CSICR3_ZERO_PACK_EN_ENABLE	1

#define CSI_CSICR3_FRMCNT_RST_RESET		1
#define CSI_CSICR3_FRMCNT_RST_UNRESET	0

#define CSI_CSICR3_FRMCNT_VALUE(X)	(X)


#endif /*IMX25_CSI_H_*/



