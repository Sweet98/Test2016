/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  Date:       9/17/2009
//  Tools:      RVCT Build 836
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: interfaces_usb2_hw/usb2resu.h $
// $Revision: 1.1 $
// $Date: 2009/09/17 15:52:36EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// HEADER:  usb2resu.h
//
// USB Functions Run, Suspend, and Resume are found here outside the USB Class 
//  This allows the USB Modules on the chip to be put into suspend mode without the
//  USB Class or interface
//------------------------------------------------------------------------------
#ifndef USB2RESU_H_
#define USB2RESU_H_

void MX25USB2_run(void);
void MX25USB2_resume(void);
void MX25USB2_suspend(void);


#endif /*USB2RESU_H_*/

