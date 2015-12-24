/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  Date:       4/14/2010
//  Tools:      RVCT Build 862
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: db_shared/product.cpp $
// $Revision: 1.4 $
// $Date: 2010/10/22 16:49:17EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// File:  product.cpp
//		
//		File Contains functions related to defining the product
//
//------------------------------------------------------------------------------
#include "product.h"

#if PLATFORM_CURRENT == imx25
#include "HardwareMacros.h"
#include "iMX25_IOMUX.h"
#include "iMX25_GPIO.h"
#endif

#include "HardwareToken.h"


// ===  Function  ======================================================================
//
//  Name:  InputPowerStatus
//
//  Description: 
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	
//    in-out:
//       out:
//    return: BASE_POWER_OPTIONS				CABLE_POWERED - Power is coming from the 10 pin jack
//												AUX_POWERED - The AC Adapter is supplying power
//												BOTH_POWERED - Power is coming from both inputs
// -------------------------------------------------------------------------------------
BASE_POWER_OPTIONS InputPowerStatus(void)
{
	return CABLE_POWERED;
}	//endof InputPowerStatus	


