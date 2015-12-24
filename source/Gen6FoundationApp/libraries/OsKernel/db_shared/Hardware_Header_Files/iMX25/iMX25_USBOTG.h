/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  HARDWARE DEFINITION FILE 
//  Date:       9/18/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: db_shared/Hardware_Header_Files/iMX25/iMX25_USBOTG.h $
// $Revision: 1.2 $
// $Date: 2011/06/28 13:26:53EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx25_usbotg.h
//
// Provides definitions for the USB On The Go (OTG) module based on MX25. 
// The Host definitions are also definded Here.
// Refer to Chapter 43 of the iMX25 Reference Manual 
//
//------------------------------------------------------------------------------

#ifndef IMX25_USBOTG_H_
#define IMX25_USBOTG_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// REGISTERS
//------------------------------------------------------------------------------
// #########################################
// # USB                                   #
// #########################################
#define USB_MODULE_BASE_ADDR       0x53FF4000U
#define USB_OTG_BASE_ADDR          (USB_MODULE_BASE_ADDR+0x000U)
#define USB_H1_BASE_ADDR           (USB_MODULE_BASE_ADDR+0x200U)
#define USB_H2_BASE_ADDR           (USB_MODULE_BASE_ADDR+0x400U)
#define USB_CONTROL_REG            (USB_MODULE_BASE_ADDR+0x600U)
#define USB_OTG_MIRROR_REG         (USB_MODULE_BASE_ADDR+0x604U)
#define USB_OTG_UTMI_PHY_CTRL_FUNC (USB_MODULE_BASE_ADDR+0x608U)
#define USB_OTG_UTMI_PHY_CTRL_TEST (USB_MODULE_BASE_ADDR+0x60CU)

#define USB_H2_ID                  (USB_H2_BASE_ADDR+0x000U)   // Identification Register
#define USB_H2_HWGENERAL           (USB_H2_BASE_ADDR+0x004U)   // General Hardware Parameters
#define USB_H2_HWHOST              (USB_H2_BASE_ADDR+0x008U)   // Host Hardware Parameters
#define USB_H2_HWTXBUF             (USB_H2_BASE_ADDR+0x010U)   // TX Buffer Hardware Parameters
#define USB_H2_HWRXBUF             (USB_H2_BASE_ADDR+0x014U)   // RX Buffer Hardware Parameters
#define USB_H2_GPTIMER0LD          (USB_H2_BASE_ADDR+0x080U)  
#define USB_H2_GPTIMER0CTRL        (USB_H2_BASE_ADDR+0x084U)  
#define USB_H2_GPTIMER1LD          (USB_H2_BASE_ADDR+0x088U)  
#define USB_H2_GPTIMER1CTRL        (USB_H2_BASE_ADDR+0x08CU)  
#define USB_H2_CAPLENGTH           (USB_H2_BASE_ADDR+0x100U)   // Capability Register Length
#define USB_H2_HCIVERSION          (USB_H2_BASE_ADDR+0x102U)   // Host Interface Version Number
#define USB_H2_HCSPARAMS           (USB_H2_BASE_ADDR+0x104U)   // Host Ctrl. Structural Parameters
#define USB_H2_HCCPARAMS           (USB_H2_BASE_ADDR+0x108U)   // Host Ctrl. Capability Parameters
#define USB_H2_USBCMD              (USB_H2_BASE_ADDR+0x140U)   // USB Command
#define USB_H2_USBSTS              (USB_H2_BASE_ADDR+0x144U)   // USB Status
#define USB_H2_USBINTR             (USB_H2_BASE_ADDR+0x148U)   // USB Interrupt Enable
#define USB_H2_FRINDEX             (USB_H2_BASE_ADDR+0x14CU)   // USB Frame Index
#define USB_H2_PERIODICLISTBASE    (USB_H2_BASE_ADDR+0x154U)   // Frame List Base Address
#define USB_H2_ASYNCLISTADDR       (USB_H2_BASE_ADDR+0x158U)   // Next Asynchronous List Address
#define USB_H2_BURSTSIZE           (USB_H2_BASE_ADDR+0x160U)   // Programmable Burst Size
#define USB_H2_TXFILLTUNING        (USB_H2_BASE_ADDR+0x164U)   // Host Transmit Pre-Buffer Packet Tuning
#define USB_H2_CONFIGFLAG          (USB_H2_BASE_ADDR+0x180U)   // Configured Flag Register
#define USB_H2_PORTSC1             (USB_H2_BASE_ADDR+0x184U)   // Port Status/Control
#define USB_H2_USBMODE             (USB_H2_BASE_ADDR+0x1A8U)   // USB Device Mode

#define USB_OTG_ID                 (USB_OTG_BASE_ADDR+0x000U)  // Identification Register
#define USB_OTG_HWGENERAL          (USB_OTG_BASE_ADDR+0x004U)  // General Hardware Parameters
#define USB_OTG_HWHOST             (USB_OTG_BASE_ADDR+0x008U)  // Host Hardware Parameters
#define USB_OTG_HWDEVICE           (USB_OTG_BASE_ADDR+0x00CU)  // Device Hardware Parameters
#define USB_OTG_HWTXBUF            (USB_OTG_BASE_ADDR+0x010U)  // TX Buffer Hardware Parameters
#define USB_OTG_HWRXBUF            (USB_OTG_BASE_ADDR+0x014U)  // RX Buffer Hardware Parameters
#define USB_OTG_GPTIMER0LD         (USB_OTG_BASE_ADDR+0x080U) 
#define USB_OTG_GPTIMER0CTRL       (USB_OTG_BASE_ADDR+0x084U) 
#define USB_OTG_GPTIMER1LD         (USB_OTG_BASE_ADDR+0x088U) 
#define USB_OTG_GPTIMER1CTRL       (USB_OTG_BASE_ADDR+0x08CU) 
#define USB_OTG_CAPLENGTH          (USB_OTG_BASE_ADDR+0x100U)  // Capability Register Length
#define USB_OTG_HCIVERSION         (USB_OTG_BASE_ADDR+0x102U)  // Host Interface Version Number
#define USB_OTG_HCSPARAMS          (USB_OTG_BASE_ADDR+0x104U)  // Host Ctrl. Structural Parameters
#define USB_OTG_HCCPARAMS          (USB_OTG_BASE_ADDR+0x108U)  // Host Ctrl. Capability Parameters
#define USB_OTG_DCIVERSION         (USB_OTG_BASE_ADDR+0x120U)  // Dev. Interface Version Number
#define USB_OTG_DCCPARAMS          (USB_OTG_BASE_ADDR+0x124U)  // Device Ctrl. Capability Parameters
#define USB_OTG_USBCMD             (USB_OTG_BASE_ADDR+0x140U)  // USB Command
#define USB_OTG_USBSTS             (USB_OTG_BASE_ADDR+0x144U)  // USB Status
#define USB_OTG_USBINTR            (USB_OTG_BASE_ADDR+0x148U)  // USB Interrupt Enable
#define USB_OTG_FRINDEX            (USB_OTG_BASE_ADDR+0x14CU)  // USB Frame Index
#define USB_OTG_PERIODICLISTBASE   (USB_OTG_BASE_ADDR+0x154U)  // Frame List Base Address
#define USB_OTG_ASYNCLISTADDR      (USB_OTG_BASE_ADDR+0x158U)  // Next Asynchronous List Address
#define USB_OTG_BURSTSIZE          (USB_OTG_BASE_ADDR+0x160U)  // Programmable Burst Size
#define USB_OTG_TXFILLTUNING       (USB_OTG_BASE_ADDR+0x164U)  // Host Transmit Pre-Buffer Packet Tuning
#define USB_OTG_CONFIGFLAG         (USB_OTG_BASE_ADDR+0x180U)  // Configured Flag Register
#define USB_OTG_PORTSC1            (USB_OTG_BASE_ADDR+0x184U)  // Port Status/Control
#define USB_OTG_OTGSC              (USB_OTG_BASE_ADDR+0x1A4U)  // On-The-Go (OTGU) Status and Control
#define USB_OTG_USBMODE            (USB_OTG_BASE_ADDR+0x1A8U)  // USB Device Mode
#define USB_OTG_ENPDTSETUPSTAT     (USB_OTG_BASE_ADDR+0x1ACU)  // Endpoint Setup Status
#define USB_OTG_ENDPTPRIME         (USB_OTG_BASE_ADDR+0x1B0U)  // Endpoint Initialization
#define USB_OTG_ENDPTFLUSH         (USB_OTG_BASE_ADDR+0x1B4U)  // Endpoint De-Initialize
#define USB_OTG_ENDPTSTATUS        (USB_OTG_BASE_ADDR+0x1B8U)  // Endpoint Status
#define USB_OTG_ENDPTCOMPLETE      (USB_OTG_BASE_ADDR+0x1BCU)  // Endpoint Complete
#define USB_OTG_ENDPTCTRL0         (USB_OTG_BASE_ADDR+0x1C0U)  // Endpoint Control 0
#define USB_OTG_ENDPTCTRL1         (USB_OTG_BASE_ADDR+0x1C4U)  // Endpoint Control 1
#define USB_OTG_ENDPTCTRL2         (USB_OTG_BASE_ADDR+0x1C8U)  // Endpoint Control 2
#define USB_OTG_ENDPTCTRL3         (USB_OTG_BASE_ADDR+0x1CCU)  // Endpoint Control 3
#define USB_OTG_ENDPTCTRL4         (USB_OTG_BASE_ADDR+0x1D0U)  // Endpoint Control 4
#define USB_OTG_ENDPTCTRL5         (USB_OTG_BASE_ADDR+0x1D4U)  // Endpoint Control 5
#define USB_OTG_ENDPTCTRL6         (USB_OTG_BASE_ADDR+0x1D8U)  // Endpoint Control 6
#define USB_OTG_ENDPTCTRL7         (USB_OTG_BASE_ADDR+0x1DCU)  // Endpoint Control 7

//------------------------------------------------------------------------------
// REGISTER FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// REGISTER FIELD WIDTHS
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------

#endif /*IMX25_USBOTG_H_*/


