/*=================================================================================

//=================================================================================
   $RCSfile: interfaces_usb2_hw/usb2resu.cpp $
   $Revision: 1.2 $
   $Date: 2009/09/17 15:54:12EDT $

//=================================================================================

//=================================================================================*/
//! \file

#include "usb2resu.h"
#include "usb2.h"

#if PLATFORM_CURRENT == imx25
#include "HardwareMacros.h"
#include "iMX25_GPIO.h"
#include "usb2_mx25.h"

#else
#error No Platform Defined!
#endif //endof PLATFORM_CURRENT

// Enable DR irq reg and Set Dr controller Run
void MX25USB2_run(void)
{
	UINT reg_val;

	// Enable DR irq reg
	reg_val = USB_INTR_INT_EN | USB_INTR_ERR_INT_EN | USB_INTR_PTC_DETECT_EN | USB_INTR_RESET_EN |USB_INTR_DEVICE_SUSPEND | USB_INTR_SYS_ERR_EN;
	UOG_USBINTR = reg_val;
	
	// Set controller to Run
	reg_val = UOG_USBCMD;
	reg_val |= USB_CMD_RUN_STOP;
	UOG_USBCMD = reg_val;

	return;
}

/*-----------------------------------------------------------------------------
 * @Name:
 *   MX25USB2_resume
 *
 * @Description:
 *    MX25USB2 resume handler
 *
 * @Paramters:
 *   None.
 *
 * @Return values:
 *   
 *-----------------------------------------------------------------------------
 */	
void MX25USB2_resume(void)
{		
	UINT reg_val;
		
	//Start usb clk
	//Pls add codes
 
/*
	//Put Phy into no suspend
	reg_val = USB_PHY_CTRL_FUNC;
	reg_val |= 1 << 26;
	USB_PHY_CTRL_FUNC = reg_val;	
*/
	
	reg_val = UOG_PORTSC1;
	if(reg_val & 0x800000)  //PHCD
	{
		reg_val &= ~PORTSC_PHCD; //PHCD
		UOG_PORTSC1 = reg_val;
		
		// Force resume
		reg_val |= PORTSC_FPR;  //FPR
		UOG_PORTSC1 = reg_val;
	}	
	reg_val = USB_CTRL;
	if(reg_val & 0x80000000)  //OWIR
	{
		reg_val &= ~(1 << 27);	
		USB_CTRL = reg_val;
	}

	MX25USB2_run();
}

/*-----------------------------------------------------------------------------
 * @Name:
 *   MX25USB2_suspend
 *
 * @Description:
 *    MX25USB2 suspend handler
 *
 * @Paramters:
 *   None.
 *
 * @Return values:
 *   
 *-----------------------------------------------------------------------------
 */	
void MX25USB2_suspend(void)
{	
	UINT reg_val;
		
//	MX25USB2_stop();
	
	reg_val = UOG_USBSTS;
	UOG_USBSTS = reg_val;
	
	reg_val = UOG_PORTSC1;
	reg_val |= PORTSC_WKCN | PORTSC_WKDS |PORTSC_WKOC;  
	UOG_PORTSC1 = reg_val;
	
	reg_val = USB_CTRL;
	reg_val |= (1<<27)|(1<<28);  //OWIE,OUIE
	USB_CTRL = reg_val;
	
	reg_val = UOG_PORTSC1;
	reg_val |= PORTSC_PHCD;    //PHCD(Phy clock disable)
	UOG_PORTSC1 = reg_val;

/*	
	//Put Phy into suspend
	reg_val = USB_PHY_CTRL_FUNC;
	reg_val &= ~(1 << 26);
	USB_PHY_CTRL_FUNC = reg_val;
*/	

	//Stop usb clk
	//Pls add codes
}


