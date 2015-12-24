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
// $RCSfile: db_shared/Hardware_Header_Files/iMX27/iMX27_intervec.h $
// $Revision: 1.1 $
// $Date: 2008/09/22 16:35:08EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  iMX27_intervec.h
//
// Provides definitions of the Interrupt Vectors
// Refer to Chapter 10 Page 356 of the iMX27 Reference Manual
//
//------------------------------------------------------------------------------

#ifndef IMX27_INTERVEC_H_
#define IMX27_INTERVEC_H_


//------------------------------------------------------------------------------
// REGISTER BIT FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
// Interrupt Source Register High (INTSRCH)
//Table 10-21. Interrupt Source High (INTSRCH) Assignment

#define INT_DPTC_LSH		31		//Dynamic Process Temperature Compensate (DPTC)
#define INT_IIM_LSH			30		//IC Identify Module (IIM)
#define INT_LCDC_LSH		29		//LCD Controller (LCDC)
#define INT_SLCDC_LSH		28		//Smart LCD Controller (SLCDC)
#define INT_SAHARA_LSH		27		//Symmetric/Asymmetric Hashing and Random Accelerator
#define INT_SCM_LSH			26		//SCC SCM
#define INT_SMN_LSH			25		//SCC SMN
#define INT_USBOTG_LSH		24		//USB OTG
#define INT_USBHS2_LSH		23		//USB HOST2
#define INT_USBHS1_LSH		22		//USB HOST1
#define INT_H264_LSH		21		//H264
#define INT_EMMAPP_LSH		20		//eMMA Post Processor
#define INT_EMMAPRP_LSH		19		//eMMA Pre Processor
#define INT_FEC_LSH			18		//Fast Ethernet Controller
#define INT_UART5_LSH		17		//UART5
#define INT_UART6_LSH		16		//UART6
#define INT_DMACH15_LSH		15		//DMA Channel 15
#define INT_DMACH14_LSH		14		//DMA Channel 14
#define INT_DMACH13_LSH		13		//DMA Channel 13
#define INT_DMACH12_LSH		12		//DMA Channel 12
#define INT_DMACH11_LSH		11		//DMA Channel 11
#define INT_DMACH10_LSH		10		//DMA Channel 10
#define INT_DMACH9_LSH		9		//DMA Channel 9
#define INT_DMACH8_LSH		8		//DMA Channel 8
#define INT_DMACH7_LSH		7		//DMA Channel 7
#define INT_DMACH6_LSH		6		//DMA Channel 6
#define INT_DMACH5_LSH		5		//DMA Channel 5
#define INT_DMACH4_LSH		4		//DMA Channel 4
#define INT_DMACH3_LSH		3		//DMA Channel 3
#define INT_DMACH2_LSH		2		//DMA Channel 2
#define INT_DMACH1_LSH		1		//DMA Channel 1
#define INT_DMACH0_LSH		0		//DMA Channel 0


//Table 10-22. Interrupt Source Low (INTSRCL) Assignment
#define INT_CSI_LSH			31		//CMOS Sensor Interface (CSI)
#define INT_ATA_LSH			30		//Advanced Technology Attachment (ATA) Hard Disk
#define INT_NFC_LSH			29		//NAND Flash Controller (NFC)
#define INT_PCMCIA_LSH		28		//PCMCIA/CF Host Controller (PCMCIA)
#define INT_WDOG_LSH		27		//Watchdog (WDOG)
#define INT_GPT1_LSH		26		//General Purpose Timer (GPT1)
#define INT_GPT2_LSH		25		//General Purpose Timer (GPT2)
#define INT_GPT3_LSH		24		//General Purpose Timer (GPT3)
#define INT_PWM_LSH			23		//Pulse Width Modulator (PWM)
#define INT_RTC_LSH			22		//Real-Time Clock (RTC)
#define INT_KPP_LSH			21		//Key Pad Port (KPP)
#define INT_UART1_LSH		20		//UART1
#define INT_UART2_LSH		19		//UART2
#define INT_UART3_LSH		18		//UART3
#define INT_UART4_LSH		17		//UART4
#define INT_CSPI1_LSH		16		//Configurable SPI (CSPI1)
#define INT_CSPI2_LSH		15		//Configurable SPI (CSPI2)
#define INT_SSI1_LSH		14		//Synchronous Serial Interface (SSI1)
#define INT_SSI2_LSH		13		//Synchronous Serial Interface (SSI2)
#define INT_I2C1_LSH		12		//I2C Bus Controller (I2C1)
#define INT_SDHC1_LSH		11		//Secure Digital Host Controller (SDHC1)
#define INT_SDHC2_LSH		10		//Secure Digital Host Controller (SDHC2)
#define INT_SDHC3_LSH		9		//Secure Digital Host Controller (SDHC3)
#define INT_GPIO_LSH		8		//General Purpose Input/Output (GPIO)
#define INT_MSHC_LSH		7		//Memory Stick Host Controller (MSHC)
#define INT_CSPI3_LSH		6		//Configurable SPI (CSPI3)
#define INT_RTIC_LSH		5		//Real Time Integrity Checker (RTIC)
#define INT_GPT4_LSH		4		//General Purpose Timer (GPT4)
#define INT_GPT5_LSH		3		//General Purpose Timer (GPT5)
#define INT_GPT6_LSH		2		//General Purpose Timer (GPT6)
#define INT_I2C2_LSH		1		//I2C Bus Controller (I2C2)
//Reserved					0		//Reserved

//------------------------------------------------------------------------------
// REGISTER FIELD WIDTHS
//------------------------------------------------------------------------------
#define INT_WID				1		//Width for all Vectors


enum vector_number_t
{
	VEC_DPTC_INT		= 63,		//Dynamic Process Temperature Compensate (DPTC)
	VEC_IIM_INT			= 62,		//IC Identify Module (IIM)
	VEC_LCDC_INT		= 61,		//LCD Controller (LCDC)
	VEC_SLCDC_INT		= 60,		//Smart LCD Controller (SLCDC)
	VEC_SAHARA_INT		= 59,		//Symmetric/Asymmetric Hashing and Random Accelerator
	VEC_SCM_INT			= 58,		//SCC SCM
	VEC_SMN_INT			= 57,		//SCC SMN
	VEC_USBOTG_INT		= 56,		//USB OTG
	VEC_USBHS2_INT		= 55,		//USB HOST2
	VEC_USBHS1_INT		= 54,		//USB HOST1
	VEC_H264_INT		= 53,		//H264
	VEC_EMMAPP_INT		= 52,		//eMMA Post Processor
	VEC_EMMAPRP_INT		= 51,		//eMMA Pre Processor
	VEC_FEC_INT			= 50,		//Fast Ethernet Controller
	VEC_UART5_INT		= 49,		//UART5
	VEC_UART6_INT		= 48,		//UART6
	VEC_DMACH15_INT		= 47,		//DMA Channel 15
	VEC_DMACH14_INT		= 46,		//DMA Channel 14
	VEC_DMACH13_INT		= 45,		//DMA Channel 13
	VEC_DMACH12_INT		= 44,		//DMA Channel 12
	VEC_DMACH11_INT		= 43,		//DMA Channel 11
	VEC_DMACH10_INT		= 42,		//DMA Channel 10
	VEC_DMACH9_INT		= 41,		//DMA Channel 9
	VEC_DMACH8_INT		= 40,		//DMA Channel 8
	VEC_DMACH7_INT		= 39,		//DMA Channel 7
	VEC_DMACH6_INT		= 38,		//DMA Channel 6
	VEC_DMACH5_INT		= 37,		//DMA Channel 5
	VEC_DMACH4_INT		= 36,		//DMA Channel 4
	VEC_DMACH3_INT		= 35,		//DMA Channel 3
	VEC_DMACH2_INT		= 34,		//DMA Channel 2
	VEC_DMACH1_INT		= 33,		//DMA Channel 1
	VEC_DMACH0_INT		= 32,		//DMA Channel 0
	
	//Table 10-22. Interrupt Source Low (INTSRCL) Assignment
	VEC_CSI_INT			= 31,		//CMOS Sensor Interface (CSI)
	VEC_ATA_INT			= 30,		//Advanced Technology Attachment (ATA) Hard Disk
	VEC_NFC_INT			= 29,		//NAND Flash Controller (NFC)
	VEC_PCMCIA_INT		= 28,		//PCMCIA/CF Host Controller (PCMCIA)
	VEC_WDOG_INT		= 27,		//Watchdog (WDOG)
	VEC_GPT1_INT		= 26,		//General Purpose Timer (GPT1)
	VEC_GPT2_INT		= 25,		//General Purpose Timer (GPT2)
	VEC_GPT3_INT		= 24,		//General Purpose Timer (GPT3)
	VEC_PWM_INT			= 23,		//Pulse Width Modulator (PWM)
	VEC_RTC_INT			= 22,		//Real-Time Clock (RTC)
	VEC_KPP_INT			= 21,		//Key Pad Port (KPP)
	VEC_UART1_INT		= 20,		//UART1
	VEC_UART2_INT		= 19,		//UART2
	VEC_UART3_INT		= 18,		//UART3
	VEC_UART4_INT		= 17,		//UART4
	VEC_CSPI1_INT		= 16,		//Configurable SPI (CSPI1)
	VEC_CSPI2_INT		= 15,		//Configurable SPI (CSPI2)
	VEC_SSI1_INT		= 14,		//Synchronous Serial Interface (SSI1)
	VEC_SSI2_INT		= 13,		//Synchronous Serial Interface (SSI2)
	VEC_I2C1_INT		= 12,		//I2C Bus Controller (I2C1)
	VEC_SDHC1_INT		= 11,		//Secure Digital Host Controller (SDHC1)
	VEC_SDHC2_INT		= 10,		//Secure Digital Host Controller (SDHC2)
	VEC_SDHC3_INT		= 9	,		//Secure Digital Host Controller (SDHC3)
	VEC_GPIO_INT		= 8	,		//General Purpose Input/Output (GPIO)
	VEC_MSHC_INT		= 7	,		//Memory Stick Host Controller (MSHC)
	VEC_CSPI3_INT		= 6	,		//Configurable SPI (CSPI3)
	VEC_RTIC_INT		= 5	,		//Real Time Integrity Checker (RTIC)
	VEC_GPT4_INT		= 4	,		//General Purpose Timer (GPT4)
	VEC_GPT5_INT		= 3	,		//General Purpose Timer (GPT5)
	VEC_GPT6_INT		= 2	,		//General Purpose Timer (GPT6)
	VEC_I2C2_INT		= 1	,		//I2C Bus Controller (I2C2)
	RESERVED			= 0			//Reserved
};	
	



#endif /*IMX27_INTERVEC_H_*/



