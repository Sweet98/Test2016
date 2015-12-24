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
// $RCSfile: db_shared/Hardware_Header_Files/iMX27/iMX27.h $
// $Revision: 1.1 $
// $Date: 2008/09/22 16:35:04EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  iMX27.h
//
// This Files includes all of the Hardware Definition files for the iMX27 Hardware
// Refer to ds005_Hardware_Definitions.doc in the Gen 6 documentation for more info
//
//------------------------------------------------------------------------------

#ifndef IMX27_H_
#define IMX27_H_

#include "HardwareMacros.h"		//Provides Macros for setting registers
#include "iMX27_aitc.h"			//ARM926EJ-S Interrupt Controller Definitions
#include "iMX27_csi.h"			//CMOS Sensor Interface Definitions
#include "iMX27_cspi.h"			//Configurable Serial Peripheral Interface Definitions
#include "iMX27_dma.h"			//Direct Memory Access Controller Definitions
#include "imx27_esdramc.h"		//Enhanced SDRAM Controller Definitions
#include "iMX27_extmem.h"		//External Memory Definitions
#include "iMX27_GPIO.h"			//General-Purpose I/O Definitions
#include "iMX27_i2c.h"			//I2C Definitions
#include "iMX27_intervec.h"		//Provides definitions of the Interrupt Vectors
#include "iMX27_pllcrc.h"		//PLL, Clock & Reset controller Definitions
#include "iMX27_pwm.h"			//Pulse-Width Modulator Definitions
#include "imx27_rtc.h"			//Real Time Clock Definitions
#include "iMX27_sysctrl.h"		//System Control Definitions
#include "iMX27_timers.h"		//General Purpose Timer (GPT) Definitions
#include "iMX27_uart.h"			//UART Definitions, Offsets for UARTs 1-6
#include "iMX27_usbotg.h"		//USB On The GO Definitions
#include "iMX27_wdog.h"			//WatchDog Timer (WDOG) Definitions


#endif /*IMX27_H_*/


