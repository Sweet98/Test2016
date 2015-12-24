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
// $RCSfile: db_shared/Hardware_Header_Files/iMX25/iMX25_EPIT.h $
// $Revision: 1.2 $
// $Date: 2011/06/28 13:26:41EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx25_epit.h
//
// Provides definitions Enhanced Periodic Interrupt Timer (EPIT)
// Refer to Chapter 45 of the iMX25 Reference Manual
//
//------------------------------------------------------------------------------

#ifndef IMX25_EPIT_H_
#define IMX25_EPIT_H_

#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// REGISTERS
//------------------------------------------------------------------------------
// #########################################
// # EPIT1                                 #
// #########################################
#define EPIT1_BASE_ADDR            0x53F94000U
#define EPIT1_EPITCR               (EPIT1_BASE_ADDR+0x00U)    
#define EPIT1_EPITSR               (EPIT1_BASE_ADDR+0x04U)    
#define EPIT1_EPITLR               (EPIT1_BASE_ADDR+0x08U)    
#define EPIT1_EPITCMPR             (EPIT1_BASE_ADDR+0x0CU)    
#define EPIT1_EPITCNR              (EPIT1_BASE_ADDR+0x10U)    

// #########################################
// # EPIT2                                 #
// #########################################
#define EPIT2_BASE_ADDR            0x53F98000U
#define EPIT2_EPITCR               (EPIT2_BASE_ADDR+0x00U)    
#define EPIT2_EPITSR               (EPIT2_BASE_ADDR+0x04U)    
#define EPIT2_EPITLR               (EPIT2_BASE_ADDR+0x08U)    
#define EPIT2_EPITCMPR             (EPIT2_BASE_ADDR+0x0CU)    
#define EPIT2_EPITCNR              (EPIT2_BASE_ADDR+0x10U)   




//------------------------------------------------------------------------------
// REGISTER BIT FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
//EPIT Control Register (EPITCR)
#define EPIT_EPITCR_EN_LSH        		0
#define EPIT_EPITCR_ENMOD_LSH        	1
#define EPIT_EPITCR_OCIEN_LSH        	2
#define EPIT_EPITCR_RLD_LSH        		3
#define EPIT_EPITCR_PRESCALAR_LSH       4
#define EPIT_EPITCR_SWR_LSH        		16
#define EPIT_EPITCR_IOVW_LSH        	17
#define EPIT_EPITCR_DBGEN_LSH        	18
#define EPIT_EPITCR_WAITEN_LSH        	19
#define EPIT_EPITCR_DOZEN_LSH        	20
#define EPIT_EPITCR_STOPEN_LSH        	21
#define EPIT_EPITCR_OM_LSH        		22
#define EPIT_EPITCR_CLKSRC_LSH        	24

//EPIT Status Register (EPITSR)
#define EPIT_EPITSR_OCIF_LSH        	0

//EPIT Load Register (EPITLR)
#define EPIT_EPITLR_LOAD_LSH        	0

//EPIT Compare Register (EPITCMPR)
#define EPIT_EPITCMPR_COMPARE_LSH       0

//EPIT Counter Register (EPITCNR)
#define EPIT_EPITCNR_COUNT_LSH       	0



//------------------------------------------------------------------------------
// REGISTER BIT FIELD WIDTHS
//------------------------------------------------------------------------------
//EPIT Control Register (EPITCR)
#define EPIT_EPITCR_EN_WID        		Width_1
#define EPIT_EPITCR_ENMOD_WID        	Width_1
#define EPIT_EPITCR_OCIEN_WID        	Width_1
#define EPIT_EPITCR_RLD_WID        		Width_1
#define EPIT_EPITCR_PRESCALAR_WID       Width_12
#define EPIT_EPITCR_SWR_WID        		Width_1
#define EPIT_EPITCR_IOVW_WID        	Width_1
#define EPIT_EPITCR_DBGEN_WID        	Width_1
#define EPIT_EPITCR_WAITEN_WID        	Width_1
#define EPIT_EPITCR_DOZEN_WID        	Width_1
#define EPIT_EPITCR_STOPEN_WID        	Width_1
#define EPIT_EPITCR_OM_WID        		Width_2
#define EPIT_EPITCR_CLKSRC_WID        	Width_2

//EPIT Status Register (EPITSR)
#define EPIT_EPITSR_OCIF_WID        	Width_1

//EPIT Status Register (EPITSR)
#define EPIT_EPITSR_OCIF_WID        	Width_32

//EPIT Load Register (EPITLR)
#define EPIT_EPITLR_LOAD_WID        	Width_32

//EPIT Compare Register (EPITCMPR)
#define EPIT_EPITCMPR_COMPARE_WID       Width_32

//EPIT Counter Register (EPITCNR)
#define EPIT_EPITCNR_COUNT_WID        	Width_32

//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------




#endif /*IMX25_EPIT_H_*/


