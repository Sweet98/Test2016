/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  HARDWARE DEFINITION FILE
//  Date:       7/24/2009
//  Tools:      RVCT Build 731
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: db_shared/Hardware_Header_Files/iMX25/iMX25_DRYICE.h $
// $Revision: 1.2 $
// $Date: 2011/06/28 13:26:48EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  iMX25_DRYICE.h
//
// Provides definitions of the DryIce Module including the Real Time Clock (RTC)
// Refer to "MCIMX25_SecurityRefManual" and "AN3874_iMX25 RTC" for more information
//
//------------------------------------------------------------------------------
#ifndef IMX25_DRYICE_H_
#define IMX25_DRYICE_H_

#include "Hardware_Width_Defines.h"		//Used for the Mask Width 
//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------
// #########################################
// # DRYICE                                #
// #########################################
#define DRYICE_BASE_ADDR		 0x53FFC000U
#define DRYICE_DTCMR 			(DRYICE_BASE_ADDR+0x00U) //Time-Counter MSB Register (DTCMR)
#define DRYICE_DTCLR 			(DRYICE_BASE_ADDR+0x04U) //Time-Counter LSB Register (DTCLR)
#define DRYICE_DCAMR 			(DRYICE_BASE_ADDR+0x08U) //Clock-Alarm MSB Register (DCAMR)
#define DRYICE_DCALR 			(DRYICE_BASE_ADDR+0x0CU) //Clock-Alarm LSB Register (DCALR)
#define DRYICE_DCR 				(DRYICE_BASE_ADDR+0x10U) //Control Register (DCR)
#define DRYICE_DSR 				(DRYICE_BASE_ADDR+0x14U) //Status Register (DSR)
#define DRYICE_DIER 			(DRYICE_BASE_ADDR+0x18U) //Interrupt-Enable Register (DIER)
#define DRYICE_DMCR 			(DRYICE_BASE_ADDR+0x1CU) //Monotonic-Counter Register (DMCR)
#define DRYICE_DKSR 			(DRYICE_BASE_ADDR+0x20U) //Key-Select Register (DKSR)
#define DRYICE_DKCR 			(DRYICE_BASE_ADDR+0x24U) //Key-Control Register (DKCR)
#define DRYICE_DTCR 			(DRYICE_BASE_ADDR+0x28U) //Tamper Configuration Register (DTCR)
#define DRYICE_DACR 			(DRYICE_BASE_ADDR+0x2CU) //Analog Configuration Register (DACR)
#define DRYICE_DGPR 			(DRYICE_BASE_ADDR+0x3CU) //General-Purpose Register (DGPR)
#define DRYICE_DPKR0 			(DRYICE_BASE_ADDR+0x40U) //Programmed-Key Register (DPKR0 to DPKR7)
#define DRYICE_DPKR1 			(DRYICE_BASE_ADDR+0x44U)
#define DRYICE_DPKR2 			(DRYICE_BASE_ADDR+0x48U)
#define DRYICE_DPKR3 			(DRYICE_BASE_ADDR+0x4CU)
#define DRYICE_DPKR4 			(DRYICE_BASE_ADDR+0x50U)
#define DRYICE_DPKR5 			(DRYICE_BASE_ADDR+0x54U)
#define DRYICE_DPKR6 			(DRYICE_BASE_ADDR+0x58U)
#define DRYICE_DPKR7 			(DRYICE_BASE_ADDR+0x5CU)
#define DRYICE_DRKR0 			(DRYICE_BASE_ADDR+0x60U) //Random-Key Register (DRKR0 to DRKR5)
#define DRYICE_DRKR1 			(DRYICE_BASE_ADDR+0x64U)
#define DRYICE_DRKR2 			(DRYICE_BASE_ADDR+0x68U)
#define DRYICE_DRKR3 			(DRYICE_BASE_ADDR+0x6CU)
#define DRYICE_DRKR4 			(DRYICE_BASE_ADDR+0x70U)
#define DRYICE_DRKR5 			(DRYICE_BASE_ADDR+0x74U)

//------------------------------------------------------------------------------
// REGISTER BIT FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
//Time-Counter MSB Register (DTCMR)
#define DRYICE_DTCMR_DTC_LSH 	0

//Time-Counter LSB Register (DTCLR)
#define DRYICE_DTCLR_DTC_LSH 	17

//Clock-Alarm MSB Register (DCAMR)
#define DRYICE_DCAMR_DCA_LSH 	0

//Clock-Alarm LSB Register (DCALR)
#define DRYICE_DCALR_DCA_LSH 	17

//Control Register (DCR)
#define DRYICE_DCR_SWR_LSH 		0
#define DRYICE_DCR_MCE_LSH 		2
#define DRYICE_DCR_TCE_LSH 		3
#define DRYICE_DCR_APE_LSH 		4
#define DRYICE_DCR_OSCB_LSH 	14
#define DRYICE_DCR_NSA_LSH 		15
#define DRYICE_DCR_FSHL_LSH 	16
#define DRYICE_DCR_TCSL_LSH 	17
#define DRYICE_DCR_TCHL_LSH 	18
#define DRYICE_DCR_MCSL_LSH 	19
#define DRYICE_DCR_MCHL_LSH 	20
#define DRYICE_DCR_PKWSL_LSH 	21
#define DRYICE_DCR_PKWHL_LSH 	22
#define DRYICE_DCR_PKRSL_LSH 	23
#define DRYICE_DCR_PKRHL_LSH 	24
#define DRYICE_DCR_RKSL_LSH 	25
#define DRYICE_DCR_RKHL_LSH 	26
#define DRYICE_DCR_KSSL_LSH 	27
#define DRYICE_DCR_KSHL_LSH 	28
#define DRYICE_DCR_TDCSL_LSH 	29
#define DRYICE_DCR_TDCHL_LSH 	30

//Status Register (DSR)
#define DRYICE_DSR_SVF_LSH 		0
#define DRYICE_DSR_NVF_LSH 		1
#define DRYICE_DSR_TCO_LSH 		2
#define DRYICE_DSR_MCO_LSH 		3
#define DRYICE_DSR_CAF_LSH 		4
#define DRYICE_DSR_RKV_LSH 		5
#define DRYICE_DSR_RKE_LSH 		6
#define DRYICE_DSR_WEF_LSH 		7
#define DRYICE_DSR_WCF_LSH 		8
#define DRYICE_DSR_WNF_LSH 		9
#define DRYICE_DSR_WBF_LSH 		10
#define DRYICE_DSR_KCB_LSH 		11
#define DRYICE_DSR_VTD_LSH 		16
#define DRYICE_DSR_CTD_LSH 		17
#define DRYICE_DSR_TTD_LSH 		18
#define DRYICE_DSR_SAD_LSH 		19
#define DRYICE_DSR_EBD_LSH 		20
#define DRYICE_DSR_ETAD_LSH 	21
#define DRYICE_DSR_ETBD_LSH 	22
#define DRYICE_DSR_WTD_LSH 		23

//Interrupt-Enable Register (DIER)
#define DRYICE_DIER_SVIE_LSH 	0
#define DRYICE_DIER_TOIE_LSH 	2
#define DRYICE_DIER_MOIE_LSH 	3
#define DRYICE_DIER_CAIE_LSH 	4
#define DRYICE_DIER_RKIE_LSH 	5
#define DRYICE_DIER_WEIE_LSH 	7
#define DRYICE_DIER_WCIE_LSH 	8
#define DRYICE_DIER_WNIE_LSH 	9

//Monotonic-Counter Register (DMCR)
#define DRYICE_DMCR_DMC_LSH 	0

//Key-Select Register (DKSR)
#define DRYICE_DKSR_SKS_LSH 	0

//Key-Control Register (DKCR)
#define DRYICE_DKCR_LRK_LSH 	0

//Tamper Configuration Register (DTCR)
#define DRYICE_DTCR_VTE_LSH 	0
#define DRYICE_DTCR_CTE_LSH 	1
#define DRYICE_DTCR_TTE_LSH 	2
#define DRYICE_DTCR_SAIE_LSH 	3
#define DRYICE_DTCR_EBE_LSH 	4
#define DRYICE_DTCR_ETAE_LSH 	5
#define DRYICE_DTCR_ETBE_LSH 	6
#define DRYICE_DTCR_WTE_LSH 	7
#define DRYICE_DTCR_TOE_LSH 	8
#define DRYICE_DTCR_MOE_LSH 	9
#define DRYICE_DTCR_SAOE_LSH 	15
#define DRYICE_DTCR_WGFE_LSH 	16
#define DRYICE_DTCR_WTGF_LSH 	17
#define DRYICE_DTCR_ETGFA_LSH 	22
#define DRYICE_DTCR_ETGFB_LSH 	27

//Analog Configuration Register (DACR)
#define DRYICE_DACR_LTDC_LSH 	0
#define DRYICE_DACR_HTDC_LSH 	3
#define DRYICE_DACR_VRC_LSH 	6

//------------------------------------------------------------------------------
// REGISTER FIELD WIDTHS
//------------------------------------------------------------------------------
//Time-Counter MSB Register (DTCMR)
#define DRYICE_DTCMR_DTC_WID 	Width_32

//Time-Counter LSB Register (DTCLR)
#define DRYICE_DTCLR_DTC_WID 	Width_15

//Clock-Alarm MSB Register (DCAMR)
#define DRYICE_DCAMR_DCA_WID 	Width_32

//Clock-Alarm LSB Register (DCALR)
#define DRYICE_DCALR_DCA_WID 	Width_15

//Control Register (DCR)
#define DRYICE_DCR_SWR_WID 		Width_1
#define DRYICE_DCR_MCE_WID 		Width_1
#define DRYICE_DCR_TCE_WID 		Width_1
#define DRYICE_DCR_APE_WID 		Width_1
#define DRYICE_DCR_OSCB_WID 	Width_1
#define DRYICE_DCR_NSA_WID 		Width_1
#define DRYICE_DCR_FSHL_WID 	Width_1
#define DRYICE_DCR_TCSL_WID 	Width_1
#define DRYICE_DCR_TCHL_WID 	Width_1
#define DRYICE_DCR_MCSL_WID 	Width_1
#define DRYICE_DCR_MCHL_WID 	Width_1
#define DRYICE_DCR_PKWSL_WID 	Width_1
#define DRYICE_DCR_PKWHL_WID 	Width_1
#define DRYICE_DCR_PKRSL_WID 	Width_1
#define DRYICE_DCR_PKRHL_WID 	Width_1
#define DRYICE_DCR_RKSL_WID 	Width_1
#define DRYICE_DCR_RKHL_WID 	Width_1
#define DRYICE_DCR_KSSL_WID 	Width_1
#define DRYICE_DCR_KSHL_WID 	Width_1
#define DRYICE_DCR_TDCSL_WID 	Width_1
#define DRYICE_DCR_TDCHL_WID 	Width_1

//Status Register (DSR)
#define DRYICE_DSR_SVF_WID 		Width_1
#define DRYICE_DSR_NVF_WID 		Width_1
#define DRYICE_DSR_TCO_WID 		Width_1
#define DRYICE_DSR_MCO_WID 		Width_1
#define DRYICE_DSR_CAF_WID 		Width_1
#define DRYICE_DSR_RKV_WID 		Width_1
#define DRYICE_DSR_RKE_WID 		Width_1
#define DRYICE_DSR_WEF_WID 		Width_1
#define DRYICE_DSR_WCF_WID 		Width_1
#define DRYICE_DSR_WNF_WID 		Width_1
#define DRYICE_DSR_WBF_WID 		Width_1
#define DRYICE_DSR_KCB_WID 		Width_1
#define DRYICE_DSR_VTD_WID 		Width_1
#define DRYICE_DSR_CTD_WID 		Width_1
#define DRYICE_DSR_TTD_WID 		Width_1
#define DRYICE_DSR_SAD_WID 		Width_1
#define DRYICE_DSR_EBD_WID 		Width_1
#define DRYICE_DSR_ETAD_WID 	Width_1
#define DRYICE_DSR_ETBD_WID 	Width_1
#define DRYICE_DSR_WTD_WID 		Width_1

//Interrupt-Enable Register (DIER)
#define DRYICE_DIER_SVIE_WID 	Width_1
#define DRYICE_DIER_TOIE_WID 	Width_1
#define DRYICE_DIER_MOIE_WID 	Width_1
#define DRYICE_DIER_CAIE_WID 	Width_1
#define DRYICE_DIER_RKIE_WID 	Width_1
#define DRYICE_DIER_WEIE_WID 	Width_1
#define DRYICE_DIER_WCIE_WID 	Width_1
#define DRYICE_DIER_WNIE_WID 	Width_1

//Monotonic-Counter Register (DMCR)
#define DRYICE_DMCR_DMC_WID 	Width_32

//Key-Select Register (DKSR)
#define DRYICE_DKSR_SKS_WID 	Width_3

//Key-Control Register (DKCR)
#define DRYICE_DKCR_LRK_WID 	Width_1

//Tamper Configuration Register (DTCR)
#define DRYICE_DTCR_VTE_WID 	Width_1
#define DRYICE_DTCR_CTE_WID 	Width_1
#define DRYICE_DTCR_TTE_WID 	Width_1
#define DRYICE_DTCR_SAIE_WID 	Width_1
#define DRYICE_DTCR_EBE_WID 	Width_1
#define DRYICE_DTCR_ETAE_WID 	Width_1
#define DRYICE_DTCR_ETBE_WID 	Width_1
#define DRYICE_DTCR_WTE_WID 	Width_1
#define DRYICE_DTCR_TOE_WID 	Width_1
#define DRYICE_DTCR_MOE_WID 	Width_1
#define DRYICE_DTCR_SAOE_WID 	Width_1
#define DRYICE_DTCR_WGFE_WID 	Width_1
#define DRYICE_DTCR_WTGF_WID 	Width_5
#define DRYICE_DTCR_ETGFA_WID 	Width_5
#define DRYICE_DTCR_ETGFB_WID 	Width_5

//Analog Configuration Register (DACR)
#define DRYICE_DACR_LTDC_WID 	Width_3
#define DRYICE_DACR_HTDC_WID 	Width_3
#define DRYICE_DACR_VRC_WID 	Width_3

//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------
#define Time_Counter_Enable		0x1
#define Time_Counter_Disable	0x0

#endif /*IMX25_DRYICE_H_*/


