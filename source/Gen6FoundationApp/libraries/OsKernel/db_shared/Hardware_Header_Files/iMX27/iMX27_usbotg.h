/**************************************************************************************/
//
// HIM 48X0G GENERATION 6 SOFTWARE
//
//  HARDWARE DEFINITION FILE 
//  Date:       5/12/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX27
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: db_shared/Hardware_Header_Files/iMX27/iMX27_usbotg.h $
// $Revision: 1.1 $
// $Date: 2008/09/22 16:35:11EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  iMX27_usbotg.h
//
// Provides definitions for the USB On The Go (OTG) module based on MX27. 
// Refer to Chapter 30 Page 986 of the iMX27 Reference Manual 
// $1002_4000 to $1002_4FFF
// $1002_5000 to $1002_5FFF  
//
//------------------------------------------------------------------------------

#ifndef IMX27_USBOTG_H_
#define IMX27_USBOTG_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// REGISTERS
//------------------------------------------------------------------------------
#define USB_MODULE_BASE_ADDR     0x10024000
#define USB_OTG_BASE_ADDR        (USB_MODULE_BASE_ADDR+0x000) // 
#define USB_H1_BASE_ADDR         (USB_MODULE_BASE_ADDR+0x200) // 
#define USB_H2_BASE_ADDR         (USB_MODULE_BASE_ADDR+0x400) // 
#define USB_CONTROL_REG          (USB_MODULE_BASE_ADDR+0x600) // 
#define USB_OTG_MIRROR_REG       (USB_MODULE_BASE_ADDR+0x604) // 

#define USB_H1_ID                (USB_H1_BASE_ADDR+0x000)   // Identification Register
#define USB_H1_HWGENERAL         (USB_H1_BASE_ADDR+0x004)   // General Hardware Parameters
#define USB_H1_HWHOST            (USB_H1_BASE_ADDR+0x008)   // Host Hardware Parameters
#define USB_H1_HWTXBUF           (USB_H1_BASE_ADDR+0x010)   // TX Buffer Hardware Parameters
#define USB_H1_HWRXBUF           (USB_H1_BASE_ADDR+0x014)   // RX Buffer Hardware Parameters
#define USB_H1_GPTIMER0LD        (USB_H1_BASE_ADDR+0x080)   // 
#define USB_H1_GPTIMER0CTRL      (USB_H1_BASE_ADDR+0x084)   // 
#define USB_H1_GPTIMER1LD        (USB_H1_BASE_ADDR+0x088)   // 
#define USB_H1_GPTIMER1CTRL      (USB_H1_BASE_ADDR+0x08C)   // 
#define USB_H1_CAPLENGTH         (USB_H1_BASE_ADDR+0x100)   // Capability Register Length
#define USB_H1_HCIVERSION        (USB_H1_BASE_ADDR+0x102)   // Host Interface Version Number
#define USB_H1_HCSPARAMS         (USB_H1_BASE_ADDR+0x104)   // Host Ctrl. Structural Parameters
#define USB_H1_HCCPARAMS         (USB_H1_BASE_ADDR+0x108)   // Host Ctrl. Capability Parameters
#define USB_H1_USBCMD            (USB_H1_BASE_ADDR+0x140)   // USB Command
#define USB_H1_USBSTS            (USB_H1_BASE_ADDR+0x144)   // USB Status
#define USB_H1_USBINTR           (USB_H1_BASE_ADDR+0x148)   // USB Interrupt Enable
#define USB_H1_FRINDEX           (USB_H1_BASE_ADDR+0x14C)   // USB Frame Index
#define USB_H1_PERIODICLISTBASE  (USB_H1_BASE_ADDR+0x154)   // Frame List Base Address
#define USB_H1_ASYNCLISTADDR     (USB_H1_BASE_ADDR+0x158)   // Next Asynchronous List Address
#define USB_H1_BURSTSIZE         (USB_H1_BASE_ADDR+0x160)   // Programmable Burst Size
#define USB_H1_TXFILLTUNING      (USB_H1_BASE_ADDR+0x164)   // Host Transmit Pre-Buffer Packet Tuning
#define USB_H1_CONFIGFLAG        (USB_H1_BASE_ADDR+0x180)   // Configured Flag Register
#define USB_H1_PORTSC1           (USB_H1_BASE_ADDR+0x184)   // Port Status/Control
#define USB_H1_USBMODE           (USB_H1_BASE_ADDR+0x1A8)   // USB Device Mode

#define USB_H2_ID                (USB_H2_BASE_ADDR+0x000)   // Identification Register
#define USB_H2_HWGENERAL         (USB_H2_BASE_ADDR+0x004)   // General Hardware Parameters
#define USB_H2_HWHOST            (USB_H2_BASE_ADDR+0x008)   // Host Hardware Parameters
#define USB_H2_HWTXBUF           (USB_H2_BASE_ADDR+0x010)   // TX Buffer Hardware Parameters
#define USB_H2_HWRXBUF           (USB_H2_BASE_ADDR+0x014)   // RX Buffer Hardware Parameters
#define USB_H2_GPTIMER0LD        (USB_H2_BASE_ADDR+0x080)   // 
#define USB_H2_GPTIMER0CTRL      (USB_H2_BASE_ADDR+0x084)   // 
#define USB_H2_GPTIMER1LD        (USB_H2_BASE_ADDR+0x088)   // 
#define USB_H2_GPTIMER1CTRL      (USB_H2_BASE_ADDR+0x08C)   // 
#define USB_H2_CAPLENGTH         (USB_H2_BASE_ADDR+0x100)   // Capability Register Length
#define USB_H2_HCIVERSION        (USB_H2_BASE_ADDR+0x102)   // Host Interface Version Number
#define USB_H2_HCSPARAMS         (USB_H2_BASE_ADDR+0x104)   // Host Ctrl. Structural Parameters
#define USB_H2_HCCPARAMS         (USB_H2_BASE_ADDR+0x108)   // Host Ctrl. Capability Parameters
#define USB_H2_USBCMD            (USB_H2_BASE_ADDR+0x140)   // USB Command
#define USB_H2_USBSTS            (USB_H2_BASE_ADDR+0x144)   // USB Status
#define USB_H2_USBINTR           (USB_H2_BASE_ADDR+0x148)   // USB Interrupt Enable
#define USB_H2_FRINDEX           (USB_H2_BASE_ADDR+0x14C)   // USB Frame Index
#define USB_H2_PERIODICLISTBASE  (USB_H2_BASE_ADDR+0x154)   // Frame List Base Address
#define USB_H2_ASYNCLISTADDR     (USB_H2_BASE_ADDR+0x158)   // Next Asynchronous List Address
#define USB_H2_BURSTSIZE         (USB_H2_BASE_ADDR+0x160)   // Programmable Burst Size
#define USB_H2_TXFILLTUNING      (USB_H2_BASE_ADDR+0x164)   // Host Transmit Pre-Buffer Packet Tuning
#define USB_H2_CONFIGFLAG        (USB_H2_BASE_ADDR+0x180)   // Configured Flag Register
#define USB_H2_PORTSC1           (USB_H2_BASE_ADDR+0x184)   // Port Status/Control
#define USB_H2_USBMODE           (USB_H2_BASE_ADDR+0x1A8)   // USB Device Mode

#define USB_OTG_ID               (USB_OTG_BASE_ADDR+0x000)  // Identification Register
#define USB_OTG_HWGENERAL        (USB_OTG_BASE_ADDR+0x004)  // General Hardware Parameters
#define USB_OTG_HWHOST           (USB_OTG_BASE_ADDR+0x008)  // Host Hardware Parameters
#define USB_OTG_HWDEVICE         (USB_OTG_BASE_ADDR+0x00C)  // Device Hardware Parameters
#define USB_OTG_HWTXBUF          (USB_OTG_BASE_ADDR+0x010)  // TX Buffer Hardware Parameters
#define USB_OTG_HWRXBUF          (USB_OTG_BASE_ADDR+0x014)  // RX Buffer Hardware Parameters
#define USB_OTG_GPTIMER0LD       (USB_OTG_BASE_ADDR+0x080)  // 
#define USB_OTG_GPTIMER0CTRL     (USB_OTG_BASE_ADDR+0x084)  // 
#define USB_OTG_GPTIMER1LD       (USB_OTG_BASE_ADDR+0x088)  // 
#define USB_OTG_GPTIMER1CTRL     (USB_OTG_BASE_ADDR+0x08C)  // 
#define USB_OTG_CAPLENGTH        (USB_OTG_BASE_ADDR+0x100)  // Capability Register Length
#define USB_OTG_HCIVERSION       (USB_OTG_BASE_ADDR+0x102)  // Host Interface Version Number
#define USB_OTG_HCSPARAMS        (USB_OTG_BASE_ADDR+0x104)  // Host Ctrl. Structural Parameters
#define USB_OTG_HCCPARAMS        (USB_OTG_BASE_ADDR+0x108)  // Host Ctrl. Capability Parameters
#define USB_OTG_DCIVERSION       (USB_OTG_BASE_ADDR+0x120)  // Dev. Interface Version Number
#define USB_OTG_DCCPARAMS        (USB_OTG_BASE_ADDR+0x124)  // Device Ctrl. Capability Parameters
#define USB_OTG_USBCMD           (USB_OTG_BASE_ADDR+0x140)  // USB Command
#define USB_OTG_USBSTS           (USB_OTG_BASE_ADDR+0x144)  // USB Status
#define USB_OTG_USBINTR          (USB_OTG_BASE_ADDR+0x148)  // USB Interrupt Enable
#define USB_OTG_FRINDEX          (USB_OTG_BASE_ADDR+0x14C)  // USB Frame Index
#define USB_OTG_PERIODICLISTBASE (USB_OTG_BASE_ADDR+0x154)  // Frame List Base Address
#define USB_OTG_ASYNCLISTADDR    (USB_OTG_BASE_ADDR+0x158)  // Next Asynchronous List Address
#define USB_OTG_BURSTSIZE        (USB_OTG_BASE_ADDR+0x160)  // Programmable Burst Size
#define USB_OTG_TXFILLTUNING     (USB_OTG_BASE_ADDR+0x164)  // Host Transmit Pre-Buffer Packet Tuning
#define USB_OTG_CONFIGFLAG       (USB_OTG_BASE_ADDR+0x180)  // Configured Flag Register
#define USB_OTG_PORTSC1          (USB_OTG_BASE_ADDR+0x184)  // Port Status/Control
#define USB_OTG_OTGSC            (USB_OTG_BASE_ADDR+0x1A4)  // On-The-Go (OTG) Status and Control
#define USB_OTG_USBMODE          (USB_OTG_BASE_ADDR+0x1A8)  // USB Device Mode
#define USB_OTG_ENPDTSETUPSTAT   (USB_OTG_BASE_ADDR+0x1AC)  // Endpoint Setup Status
#define USB_OTG_ENDPTPRIME       (USB_OTG_BASE_ADDR+0x1B0)  // Endpoint Initialization
#define USB_OTG_ENDPTFLUSH       (USB_OTG_BASE_ADDR+0x1B4)  // Endpoint De-Initialize
#define USB_OTG_ENDPTSTATUS      (USB_OTG_BASE_ADDR+0x1B8)  // Endpoint Status
#define USB_OTG_ENDPTCOMPLETE    (USB_OTG_BASE_ADDR+0x1BC)  // Endpoint Complete
#define USB_OTG_ENDPTCTRL0       (USB_OTG_BASE_ADDR+0x1C0)  // Endpoint Control 0
#define USB_OTG_ENDPTCTRL1       (USB_OTG_BASE_ADDR+0x1C4)  // Endpoint Control 1
#define USB_OTG_ENDPTCTRL2       (USB_OTG_BASE_ADDR+0x1C8)  // Endpoint Control 2
#define USB_OTG_ENDPTCTRL3       (USB_OTG_BASE_ADDR+0x1CC)  // Endpoint Control 3
#define USB_OTG_ENDPTCTRL4       (USB_OTG_BASE_ADDR+0x1D0)  // Endpoint Control 4
#define USB_OTG_ENDPTCTRL5       (USB_OTG_BASE_ADDR+0x1D4)  // Endpoint Control 5
#define USB_OTG_ENDPTCTRL6       (USB_OTG_BASE_ADDR+0x1D8)  // Endpoint Control 6
#define USB_OTG_ENDPTCTRL7       (USB_OTG_BASE_ADDR+0x1DC)  // Endpoint Control 7

//------------------------------------------------------------------------------
// REGISTER FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
#define USB_CONTROL_REG_BPE_LSH			0	//Read Only
#define USB_CONTROL_REG_H1DT_LSH		4
#define USB_CONTROL_REG_H2DT_LSH		5
#define USB_CONTROL_REG_H1PM_LSH		8
#define USB_CONTROL_REG_H1BPVAL_LSH		9
#define USB_CONTROL_REG_H1WIE_LSH		11
#define USB_CONTROL_REG_H1SIC_LSH		13
#define USB_CONTROL_REG_H1WIR_LSH		15	//Read Only
#define USB_CONTROL_REG_H2PM_LSH		16
#define USB_CONTROL_REG_H2WIE_LSH		19
#define USB_CONTROL_REG_H2UIE_LSH		20
#define USB_CONTROL_REG_H2SIC_LSH		21
#define USB_CONTROL_REG_H2WIR_LSH		23	//Read Only
#define USB_CONTROL_REG_OPM_LSH			24
#define USB_CONTROL_REG_OBPVAL_LSH		25
#define USB_CONTROL_REG_OWIE_LSH		27
#define USB_CONTROL_REG_OUIE_LSH		28
#define USB_CONTROL_REG_OSIC_LSH		29
#define USB_CONTROL_REG_OWIR_LSH		31	//Read Only


//------------------------------------------------------------------------------
// REGISTER FIELD WIDTHS
//------------------------------------------------------------------------------
#define USB_CONTROL_REG_BPE_WID			Width_1	//Read Only
#define USB_CONTROL_REG_H1DT_WID		Width_1
#define USB_CONTROL_REG_H2DT_WID		Width_1
#define USB_CONTROL_REG_H1PM_WID		Width_1
#define USB_CONTROL_REG_H1BPVAL_WID		Width_2
#define USB_CONTROL_REG_H1WIE_WID		Width_1
#define USB_CONTROL_REG_H1SIC_WID		Width_2
#define USB_CONTROL_REG_H1WIR_WID		Width_1	//Read Only
#define USB_CONTROL_REG_H2PM_WID		Width_1
#define USB_CONTROL_REG_H2WIE_WID		Width_1
#define USB_CONTROL_REG_H2UIE_WID		Width_1
#define USB_CONTROL_REG_H2SIC_WID		Width_2
#define USB_CONTROL_REG_H2WIR_WID		Width_1	//Read Only
#define USB_CONTROL_REG_OPM_WID			Width_1
#define USB_CONTROL_REG_OBPVAL_WID		Width_2
#define USB_CONTROL_REG_OWIE_WID		Width_1
#define USB_CONTROL_REG_OUIE_WID		Width_1
#define USB_CONTROL_REG_OSIC_WID		Width_2
#define USB_CONTROL_REG_OWIR_WID		Width_1	//Read Only

//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------

#endif /*IMX27_USBOTG_H_*/


