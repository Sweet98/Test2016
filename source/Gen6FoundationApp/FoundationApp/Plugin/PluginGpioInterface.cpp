/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  Date:       10/20/2010
//  Tools:      RVCT Build 862
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Plugin/PluginGpioInterface.cpp $
// $Revision: 1.5 $
// $Date: 2010/12/08 19:15:50EST $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// File: PluginGpioInterface.cpp
//
//This file contains functions that allow the Plug-in access to the 12 pin 
// flex connector GPIO.  A class is provided that interfaces to the plug-in 
// to control specific GPIO.  Another Class access the specific GPIO lines 
// related to the board.  Access functions are also provided which allow the 
// system to determine if the plug-in already has control of the pins.  Plug-ins 
// take president over the system, except the communication lines.
//------------------------------------------------------------------------------
#ifdef PLUGIN_DEV
#include <assert.h>

#include "PluginGpioInterface.h"
#include "HardwareToken.h"
#include "iMX25_GPIO.h"
#include "iMX25_IOMUX.h"
#include "HardwareMacros.h"
#include "language.h"
#include "ioman.h"
#include "ReaderConfiguration.h"

#define   	MOD_BIT(a)	((unsigned long)0x00000001 << (a))

#define		OEM_NUMBER_OF_GPIO_SUPPORTED	9
#define		PLUGIN_GPIO_PAD_REG_SETTING		0x000000E0

CPluginGpio *pCPluginGpio = NULL;
COemPluginGpio *pCOemPluginGpio = NULL;

// ===  Function  ======================================================================
//
//  Name:  CreatePluginGpio
//
//  Description:  Creates the Plugin GPIO Class.
//      
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:   
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void CPluginGpio::CreatePluginGpio(void)
{
    if (!pCPluginGpio)
    {
        pCPluginGpio = new CPluginGpio();        
    }
    ASSERT(pCPluginGpio != NULL);

	if(pCPluginGpio)
	{
		if(HardwareToken.IAmOEM() && !(HardwareToken.IAmOEMPrototype()))
		{
    		COemPluginGpio::CreateOemPluginGpio();
		}
	}
}

// ===  Function  ======================================================================
//
//  Name:  DestroyPluginGpio
//
//  Description: Destroys the Plugin GPIO class if needed.
//      
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:   
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void CPluginGpio::DestroyPluginGpio(void)
{
	if(pCPluginGpio)
	{
    	delete pCPluginGpio;
    	pCPluginGpio = NULL;
	}
}

// ===  Function  ======================================================================
//
//  Name:  CPluginGpio
//
//  Description: Initialize Plugin GPIO member variables
//      
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:   
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
CPluginGpio::CPluginGpio()
{
	m_PluginGpioControlStatus = 0;
};


// ===  Function  ======================================================================
//
//         Name:  GpioInit
//
//  Description: Function is used to take control of GPIO pins.  Inputs are bit fields 
//		used to take control of pins and specify input or output functionality.  
//		Functionality of GPIO pins can also be returned to the system.
//
//	Example: GpioInit(0x40, 0x40); will give plug-in control of  GPIO6 as an output
//  Example: GpioInit(0x00, 0x00); will return all GPIO pins to the system
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:   int				GpioPins		Specific pins which need to be controlled.
//				int				ReadWrite		set Read/Write Status 1 = Output; 0 = Input
//    in-out:
//       out:
//    return:	int								-1 is returned when a failure occurs 
//												 0 is returned if successful 
// -------------------------------------------------------------------------------------
int CPluginGpio::GpioInit(int GpioPins, int ReadWrite)
{
	int GpioNumber;
	int NumberOfGpioSupported = 0;

	if(HardwareToken.IAmOEM())
	{
		NumberOfGpioSupported = OemPluginGpio.ReportNumberofGpio();
	}
	else
	{
		return -1;
	}
	
	for( GpioNumber=0; GpioNumber < NumberOfGpioSupported; GpioNumber++)
	{
		switch(GpioNumber)
		{
			case 0:
				if(GpioPins & MOD_BIT(GpioNumber))
				{
					OemPluginGpio.TakeControlofGPIO0(ReadWrite & MOD_BIT(GpioNumber));
				}
				else
				{
					if(true == PluginHasGpioControl(GpioNumber))
					{
						OemPluginGpio.ReturnControlofGPIO0();
					}
				}
				break;
			case 1:
				if(GpioPins & MOD_BIT(GpioNumber))
				{
					OemPluginGpio.TakeControlofGPIO1(ReadWrite & MOD_BIT(GpioNumber));
				}
				else
				{
					if(true == PluginHasGpioControl(GpioNumber))
					{
						OemPluginGpio.ReturnControlofGPIO1();
					}
				}			
				break;
			case 2:
				if(GpioPins & MOD_BIT(GpioNumber))
				{
					OemPluginGpio.TakeControlofGPIO2(ReadWrite & MOD_BIT(GpioNumber));
				}
				else
				{
					if(true == PluginHasGpioControl(GpioNumber))
					{
						OemPluginGpio.ReturnControlofGPIO2();
					}
				}
				break;
			case 3:
				if(GpioPins & MOD_BIT(GpioNumber))
				{
					OemPluginGpio.TakeControlofGPIO3(ReadWrite & MOD_BIT(GpioNumber));
				}
				else
				{
					if(true == PluginHasGpioControl(GpioNumber))
					{
						OemPluginGpio.ReturnControlofGPIO3();
					}
				}
				break;
			case 4:
				if(GpioPins & MOD_BIT(GpioNumber))
				{
					OemPluginGpio.TakeControlofGPIO4(ReadWrite & MOD_BIT(GpioNumber));
				}
				else
				{
					if(true == PluginHasGpioControl(GpioNumber))
					{
						OemPluginGpio.ReturnControlofGPIO4();
					}
				}
				break;
			case 5:
				if(GpioPins & MOD_BIT(GpioNumber))
				{
					OemPluginGpio.TakeControlofGPIO5(ReadWrite & MOD_BIT(GpioNumber));
				}
				else
				{
					if(true == PluginHasGpioControl(GpioNumber))
					{
						OemPluginGpio.ReturnControlofGPIO5();
					}
				}
				break;
			case 6:
				if(GpioPins & MOD_BIT(GpioNumber))
				{
					OemPluginGpio.TakeControlofGPIO6(ReadWrite & MOD_BIT(GpioNumber));
				}
				else
				{
					if(true == PluginHasGpioControl(GpioNumber))
					{
						OemPluginGpio.ReturnControlofGPIO6();
					}
				}
				break;
			case 7:
				if(GpioPins & MOD_BIT(GpioNumber))
				{
					OemPluginGpio.TakeControlofGPIO7(ReadWrite & MOD_BIT(GpioNumber));
				}
				else
				{
					if(true == PluginHasGpioControl(GpioNumber))
					{
						OemPluginGpio.ReturnControlofGPIO7();
					}
				}
				break;
			case 8:
				if(GpioPins & MOD_BIT(GpioNumber))
				{
					OemPluginGpio.TakeControlofGPIO8(ReadWrite & MOD_BIT(GpioNumber));
				}
				else
				{
					if(true == PluginHasGpioControl(GpioNumber))
					{
						OemPluginGpio.ReturnControlofGPIO8();
					}
				}
				break;
			default:
				break;
		}
	}
	m_PluginGpioControlStatus = GpioPins;
	return 0;
} //endof GPIOInit

// ===  Function  ======================================================================
//
//         Name:  GpioRead
//
//  Description: Function used to read the GPIO pins.  A bit field is returned containing 
//		the read values.  Output pins and pins which are not initialized by the plug-in 
//		will be set as zeros.
//
//	Example: 0x100, GPIO8 is high.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return: int								Bit field containing the GPIO Pin Status
//												-1 is returned when a failure occurs 		
// -------------------------------------------------------------------------------------
int CPluginGpio::GpioRead(void)
{
	int GpioNumber = 0;
	int NumberOfGpioSupported = 0;
	int ReadData = 0;
	
	if(HardwareToken.IAmOEM())
	{
		NumberOfGpioSupported = OemPluginGpio.ReportNumberofGpio();
	}
	else
	{
		return -1;
	}
	
	for( GpioNumber=0; GpioNumber < NumberOfGpioSupported; GpioNumber++)
	{
		if(PluginHasGpioControl(GpioNumber))
		{
			switch(GpioNumber)
			{
				case 0:
					ReadData |= (OemPluginGpio.ReadGPIO0() << GpioNumber);					
					break;
				case 1:
					ReadData |= (OemPluginGpio.ReadGPIO1() << GpioNumber);					
					break;
				case 2:
					ReadData |= (OemPluginGpio.ReadGPIO2() << GpioNumber);					
					break;
				case 3:
					ReadData |= (OemPluginGpio.ReadGPIO3() << GpioNumber);				
					break;
				case 4:
					ReadData |= (OemPluginGpio.ReadGPIO4() << GpioNumber);	
					break;
				case 5:
					ReadData |= (OemPluginGpio.ReadGPIO5() << GpioNumber);					
					break;
				case 6:
					ReadData |= (OemPluginGpio.ReadGPIO6() << GpioNumber);					
					break;
				case 7:
					ReadData |= (OemPluginGpio.ReadGPIO7() << GpioNumber);					
					break;
				case 8:
					ReadData |= (OemPluginGpio.ReadGPIO8() << GpioNumber);					
					break;
				default:
					break;				
			}
		}
	}
	return ReadData;
} //endof GpioRead

// ===  Function  ======================================================================
//
//         Name:  GpioWrite
//
//  Description: Function used to write the GPIO Pins.  The pins to write and values 
//		are specified as inputs.  Pins that are not initialized by the plug-in and 
//		input pins are ignored.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	int				GpioPins		Specific pins which need to be controlled.
//				int				WriteData		1 = high, 0 = low
//    in-out:
//       out:
//    return:	int								-1 is returned when a failure occurs 
//												 0 is returned if successful
// -------------------------------------------------------------------------------------
int CPluginGpio::GpioWrite(int GpioPins, int WriteData)
{
	int GpioNumber = 0;
	int NumberOfGpioSupported = 0;
	
	if(HardwareToken.IAmOEM())
	{
		NumberOfGpioSupported = OemPluginGpio.ReportNumberofGpio();
	}
	else
	{
		return -1;
	}
	
	for( GpioNumber=0; GpioNumber < NumberOfGpioSupported; GpioNumber++)
	{
		if((GpioPins & MOD_BIT(GpioNumber)) && PluginHasGpioControl(GpioNumber))
		{
			switch(GpioNumber)
			{
				case 0:
					OemPluginGpio.WriteGPIO0(WriteData & MOD_BIT(GpioNumber));					
					break;
				case 1:
					OemPluginGpio.WriteGPIO1(WriteData & MOD_BIT(GpioNumber));					
					break;
				case 2:
					OemPluginGpio.WriteGPIO2(WriteData & MOD_BIT(GpioNumber));					
					break;
				case 3:
					OemPluginGpio.WriteGPIO3(WriteData & MOD_BIT(GpioNumber));					
					break;
				case 4:
					OemPluginGpio.WriteGPIO4(WriteData & MOD_BIT(GpioNumber));					
					break;
				case 5:
					OemPluginGpio.WriteGPIO5(WriteData & MOD_BIT(GpioNumber));				
					break;
				case 6:
					OemPluginGpio.WriteGPIO6(WriteData & MOD_BIT(GpioNumber));					
					break;
				case 7:
					OemPluginGpio.WriteGPIO7(WriteData & MOD_BIT(GpioNumber));					
					break;
				case 8:
					OemPluginGpio.WriteGPIO8(WriteData & MOD_BIT(GpioNumber));				
					break;
				default:
					break;			
			}
		}
	}
	return 0;
} //endof GpioWrite

// ===  Function  ======================================================================
//
//         Name:  PluginHasGpioControl
//
//  Description: Internal function used by the GPIO class to determine if the Plug-in 
//		already has control of a specific pin
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  int				GpioPin			Bit field representing pin number
//    in-out:
//       out:
//    return: bool								TRUE - Plug-in has control of PIN
//												FALSE - System has control of PIN
// -------------------------------------------------------------------------------------
bool CPluginGpio::PluginHasGpioControl(int GpioPin)
{
	return (m_PluginGpioControlStatus & MOD_BIT(GpioPin));
} //endof PluginHasGpioControl

// ===  Function  ======================================================================
//
//  Name:  CreateOemPluginGpio
//
//  Description:  Creates the OEM Plugin GPIO Class.
//      
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:   
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void COemPluginGpio::CreateOemPluginGpio(void)
{
    if (!pCOemPluginGpio)
    {
        pCOemPluginGpio = new COemPluginGpio();     
        ASSERT(pCOemPluginGpio != NULL);   
    }
}

// ===  Function  ======================================================================
//
//  Name:  DestroyOemPluginGpio
//
//  Description: Destroys the OEM Plugin GPIO class if needed.
//      
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:   
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void COemPluginGpio::DestroyOemPluginGpio(void)
{
	if(pCOemPluginGpio)
	{
    	delete pCOemPluginGpio;
    	pCOemPluginGpio = NULL;
	}
}

// ===  Function  ======================================================================
//
//  Name:  CPluginGpio
//
//  Description: Initialize Plugin GPIO member variables
//      
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:   
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
COemPluginGpio::COemPluginGpio()
{
	m_OEMNumberOfGpioSupported = OEM_NUMBER_OF_GPIO_SUPPORTED;

	m_SavedMuxGPIO0 = 0x0;
	m_SavedMuxGPIO1 = 0x0;
	m_SavedMuxGPIO2 = 0x0;
	m_SavedMuxGPIO3 = 0x0;
	m_SavedMuxGPIO4 = 0x0;
	m_SavedMuxGPIO5 = 0x0;
	m_SavedMuxGPIO6 = 0x0;
	m_SavedMuxGPIO7 = 0x0;
	m_SavedMuxGPIO8Out = 0x0;
	m_SavedMuxGPIO8In = 0x0;
	
	m_SavedPadGPIO0 = 0x0;
	m_SavedPadGPIO1 = 0x0;
	m_SavedPadGPIO2 = 0x0;
	m_SavedPadGPIO3 = 0x0;
	m_SavedPadGPIO4 = 0x0;
	m_SavedPadGPIO5 = 0x0;
	m_SavedPadGPIO6 = 0x0;
	m_SavedPadGPIO7 = 0x0;
	m_SavedPadGPIO8Out = 0x0;
	m_SavedPadGPIO8In = 0x0;
	
	m_SavedDirGPIO0 = 0x0;
	m_SavedDirGPIO1 = 0x0;
	m_SavedDirGPIO2 = 0x0;
	m_SavedDirGPIO3 = 0x0;
	m_SavedDirGPIO4 = 0x0;
	m_SavedDirGPIO5 = 0x0;
	m_SavedDirGPIO6 = 0x0;
	m_SavedDirGPIO7 = 0x0;
	m_SavedDirGPIO8Out = 0x0;
	m_SavedDirGPIO8In = 0x0;
	
	m_SavedDatGPIO0 = 0x0;
	m_SavedDatGPIO1 = 0x0;
	m_SavedDatGPIO2 = 0x0;
	m_SavedDatGPIO3 = 0x0;
	m_SavedDatGPIO4 = 0x0;
	m_SavedDatGPIO5 = 0x0;
	m_SavedDatGPIO6 = 0x0;
	m_SavedDatGPIO7 = 0x0;
	m_SavedDatGPIO8Out = 0x0;
	m_SavedDatGPIO8In = 0x0;
	
	m_PluginHasControlofUART1_RXD = 0;
	m_PluginHasControlofUART1_TXD = 0;
	m_PluginHasControlofUART1_RTS = 0;
	m_PluginHasControlofUART1_CTS = 0;
	m_PluginHasControlofNFRB = 0;
	m_PluginHasControlofCSPI1_SS0 = 0;
	m_PluginHasControlofCSPI1_SS1 = 0;
	m_PluginHasControlofKPP_ROW1 = 0;
	m_PluginHasControlofKPP_COL2 = 0;
	m_PluginHasControlofKPP_ROW0 = 0;
	
};


// ===  Function  ======================================================================
//
//         Name:  TakeControlofGPIO (0-8)
//
//  Description: Saves Existing configuration and initializes the pin as GPIO for plug-in use.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in: Bool				InputOutput		Pin Direction 1 = Output; 0 = Input
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void COemPluginGpio::TakeControlofGPIO0(bool InputOutput)
{
	DISABLE_INTERRUPTS();
	
	if(theStdInterface.GetTerminalID() != TRM_ASYNC_ASCII)
	{
		if(m_PluginHasControlofUART1_RXD == false)	//Don't save the data if you already have control of the pin
		{		
			m_SavedMuxGPIO0 = READ_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_RXD);
			m_SavedPadGPIO0 = READ_REG(IOMUXC_SW_PAD_CTL_PAD_UART1_RXD);
			m_SavedDirGPIO0 =  READ_FIELD(GPIO4_GDIR0,GPIO_BIT22_LSH,GPIO_BIT22_WID);
			m_SavedDatGPIO0 =  READ_FIELD(GPIO4_DR0,GPIO_BIT22_LSH,GPIO_BIT22_WID);
		
			m_PluginHasControlofUART1_RXD = true;
		}
		
		//Allow Plug-in to change output setting.
		
		//GPIO4, pin 22, Function: 232_RxD / nTerm_Clk_In   Pin name: UART1_RXD    GPIO4_22
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT22_LSH,GPIO_BIT22_WID,InputOutput);		//set direction 1 = Output; 0 = Input
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART1_RXD,PLUGIN_GPIO_PAD_REG_SETTING);		//Disable Pull-up
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_RXD,MUX_MODE_5);		//0x05 to select mux mode 5 GPIO
	}	
	RESTORE_INTERRUPTS();
	
} //endof TakeControlofGPIO0

void COemPluginGpio::TakeControlofGPIO1(bool InputOutput)
{
	if(theStdInterface.GetTerminalID() != TRM_ASYNC_ASCII)
	{
		DISABLE_INTERRUPTS();
		
		if(m_PluginHasControlofUART1_TXD == false)
		{
			m_SavedMuxGPIO1 = READ_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_TXD);
			m_SavedPadGPIO1 = READ_REG(IOMUXC_SW_PAD_CTL_PAD_UART1_TXD);
			m_SavedDirGPIO1 =  READ_FIELD(GPIO4_GDIR0,GPIO_BIT23_LSH,GPIO_BIT23_WID);
			m_SavedDatGPIO1 =  READ_FIELD(GPIO4_DR0,GPIO_BIT23_LSH,GPIO_BIT23_WID);
			
			m_PluginHasControlofUART1_TXD = true; 
		}
		
		//Function: 232_TxD / Term_Data_Out   Pin name: UART1_TXD    GPIO4_23
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT23_LSH,GPIO_BIT23_WID,InputOutput);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART1_TXD,PLUGIN_GPIO_PAD_REG_SETTING);
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_TXD,MUX_MODE_5);
		
		RESTORE_INTERRUPTS();
	}		
		
} //endof TakeControlofGPIO1

void COemPluginGpio::TakeControlofGPIO2(bool InputOutput)
{
	if(theStdInterface.GetTerminalID() != TRM_ASYNC_ASCII)
	{
		DISABLE_INTERRUPTS();
		
		if(m_PluginHasControlofUART1_CTS == false)
		{
			m_SavedMuxGPIO2 = READ_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_CTS);
			m_SavedPadGPIO2 = READ_REG(IOMUXC_SW_PAD_CTL_PAD_UART1_CTS);
			m_SavedDirGPIO2 =  READ_FIELD(GPIO4_GDIR0,GPIO_BIT25_LSH,GPIO_BIT25_WID);
			m_SavedDatGPIO2 =  READ_FIELD(GPIO4_DR0,GPIO_BIT25_LSH,GPIO_BIT25_WID);
			
			m_PluginHasControlofUART1_CTS = true;
		}
		
		//Function: 232_RTS / Key_Data_Out   Pin name: UART1_CTS    GPIO4_25	
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT25_LSH,GPIO_BIT25_WID,InputOutput);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART1_CTS,PLUGIN_GPIO_PAD_REG_SETTING);
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_CTS,MUX_MODE_5);
		
		RESTORE_INTERRUPTS();
	}		
	
} //endof TakeControlofGPIO2

void COemPluginGpio::TakeControlofGPIO3(bool InputOutput)
{
	if(theStdInterface.GetTerminalID() != TRM_ASYNC_ASCII)
	{
		DISABLE_INTERRUPTS();
		
		if(m_PluginHasControlofUART1_RTS == false)
		{
			m_SavedMuxGPIO3 = READ_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_RTS);
			m_SavedPadGPIO3 = READ_REG(IOMUXC_SW_PAD_CTL_PAD_UART1_RTS);
			m_SavedDirGPIO3 =  READ_FIELD(GPIO4_GDIR0,GPIO_BIT24_LSH,GPIO_BIT24_WID);
			m_SavedDatGPIO3 =  READ_FIELD(GPIO4_DR0,GPIO_BIT24_LSH,GPIO_BIT24_WID);
			
			m_PluginHasControlofUART1_RTS = true;		
		}
		
		//GPIO4, pin 25, Function: 232_RTS / nKey_Clk_In   Pin name: UART1_RTS    GPIO4_24
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT24_LSH,GPIO_BIT24_WID,InputOutput);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART1_RTS,PLUGIN_GPIO_PAD_REG_SETTING);
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_RTS,MUX_MODE_5);

		RESTORE_INTERRUPTS();	
	}
	
} //endof TakeControlofGPIO3

void COemPluginGpio::TakeControlofGPIO4(bool InputOutput)
{
	DISABLE_INTERRUPTS();
	
	if(m_PluginHasControlofNFRB == false)
	{
		m_SavedMuxGPIO4 = READ_REG(IOMUXC_SW_MUX_CTL_PAD_NFRB);
		m_SavedPadGPIO4 = READ_REG(IOMUXC_SW_PAD_CTL_PAD_NFRB);
		m_SavedDirGPIO4 =  READ_FIELD(GPIO3_GDIR0,GPIO_BIT31_LSH,GPIO_BIT31_WID);
		m_SavedDatGPIO4 =  READ_FIELD(GPIO3_DR0,GPIO_BIT31_LSH,GPIO_BIT31_WID);
	
		m_PluginHasControlofNFRB = true;
	}
	
	//GPIO - NFRB - Trace 2  - PWRDWN - GPIO3_31
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT31_LSH,GPIO_BIT31_WID,InputOutput);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_NFRB,PLUGIN_GPIO_PAD_REG_SETTING);
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_NFRB,MUX_MODE_5);

	RESTORE_INTERRUPTS();

} //endof TakeControlofGPIO4

void COemPluginGpio::TakeControlofGPIO5(bool InputOutput)
{
	DISABLE_INTERRUPTS();
	
	if(m_PluginHasControlofCSPI1_SS0 == false)
	{
		m_SavedMuxGPIO5 = READ_REG(IOMUXC_SW_MUX_CTL_PAD_CSPI1_SS0);
		m_SavedPadGPIO5 = READ_REG(IOMUXC_SW_PAD_CTL_PAD_CSPI1_SS0);
		m_SavedDirGPIO5 =  READ_FIELD(GPIO1_GDIR0,GPIO_BIT16_LSH,GPIO_BIT16_WID);
		m_SavedDatGPIO5 =  READ_FIELD(GPIO1_DR0,GPIO_BIT16_LSH,GPIO_BIT16_WID);
	
		m_PluginHasControlofCSPI1_SS0 = true;
	}
	
	//GPIO - CSPI1_SS0 - nBeeper (Trace 6) - GPIO1_16
	HAZARD_WRITE_FIELD_TO_REG(GPIO1_GDIR0,GPIO_BIT16_LSH,GPIO_BIT16_WID,InputOutput);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_CSPI1_SS0,PLUGIN_GPIO_PAD_REG_SETTING);
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_CSPI1_SS0,MUX_MODE_5);

	RESTORE_INTERRUPTS();

} //endof TakeControlofGPIO5

void COemPluginGpio::TakeControlofGPIO6(bool InputOutput)
{
	DISABLE_INTERRUPTS();
	
	if(m_PluginHasControlofCSPI1_SS1 == false)
	{
		m_SavedMuxGPIO6 = READ_REG(IOMUXC_SW_MUX_CTL_PAD_CSPI1_SS1);
		m_SavedPadGPIO6 = READ_REG(IOMUXC_SW_PAD_CTL_PAD_CSPI1_SS1);
		m_SavedDirGPIO6 =  READ_FIELD(GPIO1_GDIR0,GPIO_BIT17_LSH,GPIO_BIT17_WID);
		m_SavedDatGPIO6 =  READ_FIELD(GPIO1_DR0,GPIO_BIT17_LSH,GPIO_BIT17_WID);
	
		m_PluginHasControlofCSPI1_SS1 = true; 
	}
	
	////GPIO - CSPI1_SS1 - nGRLED - Port1_17
	HAZARD_WRITE_FIELD_TO_REG(GPIO1_GDIR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,InputOutput);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_CSPI1_SS1,PLUGIN_GPIO_PAD_REG_SETTING);
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_CSPI1_SS1,MUX_MODE_5);
			
	RESTORE_INTERRUPTS();

} //endof TakeControlofGPIO6

void COemPluginGpio::TakeControlofGPIO7(bool InputOutput)
{
	DISABLE_INTERRUPTS();
	
	if(m_PluginHasControlofKPP_ROW1 == false)
	{ 
		m_SavedMuxGPIO7 = READ_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_ROW1);
		m_SavedPadGPIO7 = READ_REG(IOMUXC_SW_PAD_CTL_PAD_KPP_ROW1);
		m_SavedDirGPIO7 =  READ_FIELD(GPIO2_GDIR0,GPIO_BIT30_LSH,GPIO_BIT30_WID);
		m_SavedDatGPIO7 =  READ_FIELD(GPIO2_DR0,GPIO_BIT30_LSH,GPIO_BIT30_WID);
	
		m_PluginHasControlofKPP_ROW1 = true;	
	}
	
	//GPIO - KPP_ROW1 - AIM_IN - GPIO2_30
	HAZARD_WRITE_FIELD_TO_REG(GPIO2_GDIR0,GPIO_BIT30_LSH,GPIO_BIT30_WID,InputOutput);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_KPP_ROW1,0x000000A0);
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_ROW1,MUX_MODE_5);

	RESTORE_INTERRUPTS();

} //endof TakeControlofGPIO7

void COemPluginGpio::TakeControlofGPIO8(bool InputOutput)
{
	DISABLE_INTERRUPTS();
	
	//GPIO Pin 8 has different pins for input and output capability
	//OUTPUT PIN
	if(m_PluginHasControlofKPP_ROW0 == false)
	{
		m_SavedMuxGPIO8Out = READ_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_ROW0);
		m_SavedPadGPIO8Out = READ_REG(IOMUXC_SW_PAD_CTL_PAD_KPP_ROW0);
		m_SavedDirGPIO8Out =  READ_FIELD(GPIO2_GDIR0,GPIO_BIT29_LSH,GPIO_BIT29_WID);
		m_SavedDatGPIO8Out =  READ_FIELD(GPIO2_DR0,GPIO_BIT29_LSH,GPIO_BIT29_WID);
		
		m_PluginHasControlofKPP_ROW0 = true;
	}
	
	//GPIO2, pin 29, Function: nSW_TRIG   Pin name: KPP_ROW0    GPIO2_29
	HAZARD_WRITE_FIELD_TO_REG(GPIO2_GDIR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,InputOutput);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_KPP_ROW0,PLUGIN_GPIO_PAD_REG_SETTING);
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_ROW0,MUX_MODE_5);

	//INPUT PIN
	if(	m_PluginHasControlofKPP_COL2 == false)
	{
		m_SavedMuxGPIO8In = READ_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_COL2);
		m_SavedPadGPIO8In = READ_REG(IOMUXC_SW_PAD_CTL_PAD_KPP_COL2);
		m_SavedDirGPIO8In =  READ_FIELD(GPIO3_GDIR0,GPIO_BIT3_LSH,GPIO_BIT3_WID);
		m_SavedDatGPIO8In =  READ_FIELD(GPIO3_DR0,GPIO_BIT3_LSH,GPIO_BIT3_WID);
		
		m_PluginHasControlofKPP_COL2 = true;
	}
	//GPIO3, pin 3, Function: nKEY_DATA_IN   Pin name: KPP_COL2    GPIO3_3
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT3_LSH,GPIO_BIT3_WID,InputOutput);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_KPP_COL2,PLUGIN_GPIO_PAD_REG_SETTING);
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_COL2,MUX_MODE_5);

	RESTORE_INTERRUPTS();

} //endof TakeControlofGPIO8


// ===  Function  ======================================================================
//
//         Name:  ReturnControlofGPIO
//
//  Description: Gives control of pin back to the system, recalls saved pin 
//		configuration from TakeControl function
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void COemPluginGpio::ReturnControlofGPIO0(void)
{
	if(m_PluginHasControlofUART1_RXD == true)
	{
		DISABLE_INTERRUPTS();
	
		//GPIO4, pin 22, Function: 232_RxD / nTerm_Clk_In   Pin name: UART1_RXD    GPIO4_22
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT22_LSH,GPIO_BIT22_WID,m_SavedDatGPIO0);		//set direction 1 = Output; 0 = Input
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT22_LSH,GPIO_BIT22_WID,m_SavedDirGPIO0);		//set direction 1 = Output; 0 = Input
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART1_RXD,m_SavedPadGPIO0);		//Disable Pull-up
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_RXD,m_SavedMuxGPIO0);		//0x05 to select mux mode 5 GPIO
	
		m_PluginHasControlofUART1_RXD = false;
		
		RESTORE_INTERRUPTS();
	}
		
} //endof ReturnControlofGPIO0

void COemPluginGpio::ReturnControlofGPIO1(void)
{
	if(m_PluginHasControlofUART1_TXD == true)
	{
		DISABLE_INTERRUPTS();
		
		//Function: 232_TxD / Term_Data_Out   Pin name: UART1_TXD    GPIO4_23
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT23_LSH,GPIO_BIT23_WID,m_SavedDatGPIO1);
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT23_LSH,GPIO_BIT23_WID,m_SavedDirGPIO1);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART1_TXD,m_SavedPadGPIO1);
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_TXD,m_SavedMuxGPIO1);
	
		m_PluginHasControlofUART1_TXD = false;
				
		RESTORE_INTERRUPTS();
	}
	
} //endof ReturnControlofGPIO1

void COemPluginGpio::ReturnControlofGPIO2(void)
{
	if(m_PluginHasControlofUART1_CTS == true)
	{
		DISABLE_INTERRUPTS();

		//Function: 232_RTS / Key_Data_Out   Pin name: UART1_CTS    GPIO4_25
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT25_LSH,GPIO_BIT25_WID,m_SavedDatGPIO2);
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT25_LSH,GPIO_BIT25_WID,m_SavedDirGPIO2);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART1_CTS,m_SavedPadGPIO2);
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_CTS,m_SavedMuxGPIO2);
	
		m_PluginHasControlofUART1_CTS = false;
		
		RESTORE_INTERRUPTS();
	}

} //endof ReturnControlofGPIO2

void COemPluginGpio::ReturnControlofGPIO3(void)
{
	if(m_PluginHasControlofUART1_RTS == true)
	{
		DISABLE_INTERRUPTS();
		
		//GPIO4, pin 24, Function: 232_CTS / nKey_Clk_In   Pin name: UART1_RTS    GPIO4_24
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT24_LSH,GPIO_BIT24_WID,m_SavedDatGPIO3);
		HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT24_LSH,GPIO_BIT24_WID,m_SavedDirGPIO3);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART1_RTS,m_SavedPadGPIO3);
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_UART1_RTS,m_SavedMuxGPIO3);
	
		m_PluginHasControlofUART1_CTS = false;
	
		RESTORE_INTERRUPTS();
	}
} //endof ReturnControlofGPIO3

void COemPluginGpio::ReturnControlofGPIO4(void)
{
	if(m_PluginHasControlofNFRB == true)
	{
		DISABLE_INTERRUPTS();

		//GPIO - NFRB - Trace 2  - PWRDWN - GPIO3_31
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT31_LSH,GPIO_BIT31_WID,m_SavedDatGPIO4);
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT31_LSH,GPIO_BIT31_WID,m_SavedDirGPIO4);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_NFRB,m_SavedPadGPIO4);
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_NFRB,m_SavedMuxGPIO4);

		m_PluginHasControlofNFRB = false;

		RESTORE_INTERRUPTS();
	}

} //endof ReturnControlofGPIO4

void COemPluginGpio::ReturnControlofGPIO5(void)
{
	if(m_PluginHasControlofCSPI1_SS0 == true)
	{
		DISABLE_INTERRUPTS();
	
		//GPIO - CSPI1_SS0 - nBeeper (Trace 6) - GPIO1_16
		HAZARD_WRITE_FIELD_TO_REG(GPIO1_DR0,GPIO_BIT16_LSH,GPIO_BIT16_WID,m_SavedDatGPIO5);
		HAZARD_WRITE_FIELD_TO_REG(GPIO1_GDIR0,GPIO_BIT16_LSH,GPIO_BIT16_WID,m_SavedDirGPIO5);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_CSPI1_SS0,m_SavedPadGPIO5);
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_CSPI1_SS0,m_SavedMuxGPIO5);

		m_PluginHasControlofCSPI1_SS0 = false;

		RESTORE_INTERRUPTS();
	}

} //endof ReturnControlofGPIO5

void COemPluginGpio::ReturnControlofGPIO6(void)
{
	if(m_PluginHasControlofCSPI1_SS1 == true)
	{
		DISABLE_INTERRUPTS();
	
		//GPIO - CSPI1_SS1 - nGRLED - Port1_17
		HAZARD_WRITE_FIELD_TO_REG(GPIO1_DR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,m_SavedDatGPIO6);
		HAZARD_WRITE_FIELD_TO_REG(GPIO1_GDIR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,m_SavedDirGPIO6);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_CSPI1_SS1,m_SavedPadGPIO6);
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_CSPI1_SS1,m_SavedMuxGPIO6);
	
		m_PluginHasControlofCSPI1_SS1 = false;
			
		RESTORE_INTERRUPTS();
	}
	
} //endof ReturnControlofGPIO6

void COemPluginGpio::ReturnControlofGPIO7(void)
{
	if(m_PluginHasControlofKPP_ROW1 == true)
	{
		DISABLE_INTERRUPTS();

		//GPIO - KPP_ROW1 - AIM_IN - GPIO2_30
		HAZARD_WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT30_LSH,GPIO_BIT30_WID,m_SavedDatGPIO7);
		HAZARD_WRITE_FIELD_TO_REG(GPIO2_GDIR0,GPIO_BIT30_LSH,GPIO_BIT30_WID,m_SavedDirGPIO7);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_KPP_ROW1,m_SavedPadGPIO7);
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_ROW1,m_SavedMuxGPIO7);

		m_PluginHasControlofKPP_ROW1 = false;

		RESTORE_INTERRUPTS();
	}

} //endof ReturnControlofGPIO7

void COemPluginGpio::ReturnControlofGPIO8(void)
{
	if((m_PluginHasControlofKPP_COL2 == true) && (m_PluginHasControlofKPP_ROW0 == true))
	{
		DISABLE_INTERRUPTS();

		//GPIO Pin 8 has different pins for input and output capability
		//OUTPUT PIN
		//GPIO2, pin 29, Function: nSW_TRIG   Pin name: KPP_ROW0    GPIO2_29
		HAZARD_WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,m_SavedDatGPIO8Out);
		HAZARD_WRITE_FIELD_TO_REG(GPIO2_GDIR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,m_SavedDirGPIO8Out);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_KPP_ROW0,m_SavedPadGPIO8Out);
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_ROW0,m_SavedMuxGPIO8Out);

		//INPUT PIN
		//GPIO3, pin 3, Function: nKEY_DATA_IN   Pin name: KPP_COL2    GPIO3_3
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT3_LSH,GPIO_BIT3_WID,m_SavedDatGPIO8In);
		HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT3_LSH,GPIO_BIT3_WID,m_SavedDirGPIO8In);
		HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_KPP_COL2,m_SavedPadGPIO8In);
		HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_KPP_COL2,m_SavedMuxGPIO8In);

		m_PluginHasControlofKPP_COL2 = false;
		m_PluginHasControlofKPP_ROW0 = false;
	
		RESTORE_INTERRUPTS();
	}

} //endof ReturnControlofGPIO8

// ===  Function  ======================================================================
//
//         Name:  ReadGPIO
//
//  Description: Reads specific OEM pins.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return: bool								Pin State 1 = High, 0 = Low
// -------------------------------------------------------------------------------------
bool COemPluginGpio::ReadGPIO0(void)
{
	//Function: 232_RxD / nTerm_Clk_In   Pin name: UART1_RXD    GPIO4_22
	return READ_FIELD(GPIO4_PSR0,GPIO_BIT22_LSH,GPIO_BIT22_WID);
} //endof ReadGPIO0

bool COemPluginGpio::ReadGPIO1(void)
{
	//Function: 232_TXD/Term_Data_Out   Pin name: UART1_TXD    GPIO4_23
	return READ_FIELD(GPIO4_PSR0,GPIO_BIT23_LSH,GPIO_BIT23_WID);
} //endof ReadGPIO1

bool COemPluginGpio::ReadGPIO2(void)
{
	//Function: 232_RTS / Key_Data_Out   Pin name: UART1_CTS    GPIO4_25
	return READ_FIELD(GPIO4_PSR0,GPIO_BIT25_LSH,GPIO_BIT25_WID);
} //endof ReadGPIO2

bool COemPluginGpio::ReadGPIO3(void)
{
	//GPIO4, pin 24, Function: 232_CTS / nKey_Clk_In   Pin name: UART1_RTS    GPIO4_24
	return READ_FIELD(GPIO4_PSR0,GPIO_BIT24_LSH,GPIO_BIT24_WID);
} //endof ReadGPIO3

bool COemPluginGpio::ReadGPIO4(void)
{
	//GPIO - NFRB - Trace 2  - PWRDWN - GPIO3_31
	return READ_FIELD(GPIO3_PSR0,GPIO_BIT31_LSH,GPIO_BIT31_WID);
} //endof ReadGPIO4

bool COemPluginGpio::ReadGPIO5(void)
{
	//GPIO - CSPI1_SS0 - nBeeper (Trace 6) - GPIO1_16
	return READ_FIELD(GPIO1_PSR0,GPIO_BIT16_LSH,GPIO_BIT16_WID);
} //endof ReadGPIO5

bool COemPluginGpio::ReadGPIO6(void)
{
	//GPIO - CSPI1_SS1 - nGRLED - Port1_17
	return READ_FIELD(GPIO1_PSR0,GPIO_BIT17_LSH,GPIO_BIT17_WID);
} //endof ReadGPIO6

bool COemPluginGpio::ReadGPIO7(void)
{
	//GPIO - AIM_IN - KPP_ROW1 - GPIO2_30
	return READ_FIELD(GPIO2_PSR0,GPIO_BIT30_LSH,GPIO_BIT30_WID);
} //endof ReadGPIO7

bool COemPluginGpio::ReadGPIO8(void)
{
	//GPIO3, pin 3, Function: nKEY_DATA_IN   Pin name: KPP_COL2    GPIO3_3
	return READ_FIELD(GPIO3_PSR0,GPIO_BIT3_LSH,GPIO_BIT3_WID);
} //endof ReadGPIO8

// ===  Function  ======================================================================
//
//         Name:  WriteGPIO
//
//  Description: Writes specific OEM pins.  Input only pins are ignored
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	bool			DataToWrite		Value of GPIO Pin to set 1 = High, 0 = Low (at chip output)
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void COemPluginGpio::WriteGPIO0(bool DataToWrite)
{
	//This is an Input Pin Only
	//Function: 232_RxD / nTerm_Clk_In   Pin name: UART1_RXD    GPIO4_22
	//WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT22_LSH,GPIO_BIT22_WID,DataToWrite);
} //endof WriteGPIO0

void COemPluginGpio::WriteGPIO1(bool DataToWrite)
{
	//Function: 232_TXD/Term_Data_Out   Pin name: UART1_TXD    GPIO4_23
	WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT23_LSH,GPIO_BIT23_WID,DataToWrite);
} //endof WriteGPIO1

void COemPluginGpio::WriteGPIO2(bool DataToWrite)
{
	//This is an Input Pin Only
	//Function: 232_RTS / Key_Data_Out   Pin name: UART1_CTS    GPIO4_25
	//WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT25_LSH,GPIO_BIT25_WID,DataToWrite);
} //endof WriteGPIO2

void COemPluginGpio::WriteGPIO3(bool DataToWrite)
{
	//GPIO4, pin 24, Function: 232_CTS / nKey_Clk_In   Pin name: UART1_RTS    GPIO4_24
	WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT24_LSH,GPIO_BIT24_WID,DataToWrite);
} //endof WriteGPIO3

void COemPluginGpio::WriteGPIO4(bool DataToWrite)
{
	//GPIO - NFRB - Trace 2  - PWRDWN - GPIO3_31
	WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT31_LSH,GPIO_BIT31_WID,DataToWrite);
} //endof WriteGPIO4

void COemPluginGpio::WriteGPIO5(bool DataToWrite)
{
	//GPIO - CSPI1_SS0 - nBeeper (Trace 6) - GPIO1_16
	WRITE_FIELD_TO_REG(GPIO1_DR0,GPIO_BIT16_LSH,GPIO_BIT16_WID,DataToWrite);				
} //endof WriteGPIO5

void COemPluginGpio::WriteGPIO6(bool DataToWrite)
{
	//GPIO - CSPI1_SS1 - nGRLED - Port1_17
	WRITE_FIELD_TO_REG(GPIO1_DR0,GPIO_BIT17_LSH,GPIO_BIT17_WID,DataToWrite);
} //endof WriteGPIO6

void COemPluginGpio::WriteGPIO7(bool DataToWrite)
{
	//This is an Input Pin Only
	//GPIO - KPP_ROW1 - AIM_IN - GPIO2_30
	//WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT30_LSH,GPIO_BIT30_WID,DataToWrite);
} //endof WriteGPIO7

void COemPluginGpio::WriteGPIO8(bool DataToWrite)
{
	//GPIO2, pin 29, Function: nSW_TRIG   Pin name: KPP_ROW0    GPIO2_29
	WRITE_FIELD_TO_REG(GPIO2_DR0,GPIO_BIT29_LSH,GPIO_BIT29_WID,DataToWrite);
} //endof WriteGPIO8

// ===  Function  ======================================================================
//
//  Name:  ReportNumberofGpio
//
//  Description: Different boards could have different a different number of GPIO Pins
//		 supported.  This function returns the number specifically supported by the OEM board.
//      
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:   
//    in-out:
//       out:
//    return: int								Number of GPIO the Plug-in can control
// -------------------------------------------------------------------------------------
int COemPluginGpio::ReportNumberofGpio(void)
{
	return m_OEMNumberOfGpioSupported;
}

#endif //endof PLUGIN_DEV

// ===  Function  ======================================================================
//
//  Name:  DoesPluginHaveControlof...
//
//  Description: Functions used by the system to determine who has control of the pin
//      
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:   
//    in-out:
//       out:
//    return: bool								True = Plug-in has control
//												False = System can access pin
// -------------------------------------------------------------------------------------
bool DoesPluginHaveControlofUART1_RXD(void)
{
	bool status = false;
	
	if(HardwareToken.IAmOEM())
	{
		if(pCOemPluginGpio == NULL)
		{
			status = false;
		}
		else
		{
			status = OemPluginGpio.m_PluginHasControlofUART1_RXD;
		}
	}
	else
	{
		status = false;
	}
	return status;
}

bool DoesPluginHaveControlofUART1_TXD(void)
{
	bool status = false;
	
	if(HardwareToken.IAmOEM())
	{
		if(pCOemPluginGpio == NULL)
		{
			status = false;
		}
		else
		{
			status = OemPluginGpio.m_PluginHasControlofUART1_TXD;
		}
	}
	else
	{
		status = false;
	}
	return status;
}

bool DoesPluginHaveControlofUART1_RTS(void)
{
	bool status = false;
	
	if(HardwareToken.IAmOEM())
	{
		if(pCOemPluginGpio == NULL)
		{
			status = false;
		}
		else
		{
			status = OemPluginGpio.m_PluginHasControlofUART1_RTS;
		}
	}
	else
	{
		status = false;
	}
	return status;
}

bool DoesPluginHaveControlofUART1_CTS(void)
{
	bool status = false;
	
	if(HardwareToken.IAmOEM())
	{
		if(pCOemPluginGpio == NULL)
		{
			status = false;
		}
		else
		{
			status = OemPluginGpio.m_PluginHasControlofUART1_CTS;
		}
	}
	else
	{
		status = false;
	}
	return status;
}

bool DoesPluginHaveControlofNFRB(void)
{
	bool status = false;
	
	if(HardwareToken.IAmOEM())
	{
		if(pCOemPluginGpio == NULL)
		{
			status = false;
		}
		else
		{
			status = OemPluginGpio.m_PluginHasControlofNFRB;
		}
	}
	else
	{
		status = false;
	}
	return status;
}

bool DoesPluginHaveControlofCSPI1_SS0(void)
{
	bool status = false;
	
	if(HardwareToken.IAmOEM())
	{
		if(pCOemPluginGpio == NULL)
		{
			status = false;
		}
		else
		{
			status = OemPluginGpio.m_PluginHasControlofCSPI1_SS0;
		}
	}
	else
	{
		status = false;
	}
	return status;
}

bool DoesPluginHaveControlofCSPI1_SS1(void)
{
	bool status = false;
	
	if(HardwareToken.IAmOEM())
	{
		if(pCOemPluginGpio == NULL)
		{
			status = false;
		}
		else
		{
			status = OemPluginGpio.m_PluginHasControlofCSPI1_SS1;
		}
	}
	else
	{
		status = false;
	}
	return status;
}

bool DoesPluginHaveControlofKPP_ROW1(void)
{
	bool status = false;
	
	if(HardwareToken.IAmOEM())
	{
		if(pCOemPluginGpio == NULL)
		{
			status = false;
		}
		else
		{
			status = OemPluginGpio.m_PluginHasControlofKPP_ROW1;
		}
	}
	else
	{
		status = false;
	}
	return status;
}

bool DoesPluginHaveControlofKPP_COL2(void)
{
	bool status = false;
	
	if(HardwareToken.IAmOEM())
	{
		if(pCOemPluginGpio == NULL)
		{
			status = false;
		}
		else
		{
			status = OemPluginGpio.m_PluginHasControlofKPP_COL2;
		}
	}
	else
	{
		status = false;
	}
	return status;
}

bool DoesPluginHaveControlofKPP_ROW0(void)
{
	bool status = false;
	
	if(HardwareToken.IAmOEM())
	{
		if(pCOemPluginGpio == NULL)
		{
			status = false;
		}
		else
		{
			status = OemPluginGpio.m_PluginHasControlofKPP_ROW0;
		}
	}
	else
	{
		status = false;
	}
	return status;
}


