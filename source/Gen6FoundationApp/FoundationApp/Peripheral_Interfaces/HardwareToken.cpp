/*=================================================================================

//=================================================================================
   $RCSfile: Gen6FoundationApp/FoundationApp/Peripheral_Interfaces/HardwareToken.cpp $
   $Revision: 1.30 $
   $Date: 2011/11/18 17:13:02EST $

//=================================================================================

//=================================================================================*/
//------------------------------------------------------------------------------
// File:  HardwareToken.cpp
//		
//	File contains Hardware Token functions used to differentiate product and 
//		hardware differences within build targets.  See Hardware Token Table 
//		Doc 500005086 for more information on token tables.
//
//------------------------------------------------------------------------------

#include <assert.h>

#include "HardwareToken.h"

#include "language.h"

#if PLATFORM_CURRENT == imx25
#include "HardwareMacros.h"
#include "iMX25_GPIO.h"	//GPIO Defines
#include "iMX25_IOMUX.h"	//IO MUX Defines
#else
#error No Platform Defined!
#endif //endof PLATFORM_CURRENT

//Token Defines
#define CCB_TOKEN_START 0x00
#define CCB_CCB01		0x00
#define CCB_COB01		0x01
#define CCB_CCB02		0x02
#define CCB_COB02		0x03
#define CCB_TOKEN_END	0x1F

#define HEAP_TOKEN_START 0x20
#define HEAP_TOKEN_END	 0x3F

#define OEM_TOKEN_START			0x20
#define FIVE_MEG_PIX_TOKEN		0x20
#define JADE_TOKEN				0x21
//#define OEM_TOKEN_RS232_ONLY	0x22
//#define OEM_TOKEN_HISPEED		0x23
//#define OEM_TOKEN_FULLSPEED	0x24
#define OEM_TOKEN_END			0x2f

// Corded  and rfscan tokens have the same ID, unless noted otherwise
#define CORDED_RFSCAN_2LVL_ONLY	0x00 // corded and rf scanner with 2 level illumination only (no 3 level capability)
#define CORDED_RFSCAN_2LVL		0x01 // corded and rf scanner with 3 level capability, but populated for 2 level illumination
#define CORDED_RFSCAN_3LVL		0x02 // corded and rf scanner with 3 level capability, populated for 3 level illumination.  NOTE:  at this time, everything after this will be 3 level illumination.
#define CORDED_RFSCAN_INDUST	0x03 // Industrial corded (1910i) and rf (1911i) scanner

#define VUQUEST3310_TOKEN		0x10 // This used to be called the 4980 (that's what's in the HW Token Table doc).

//Voyager series
#define VOYAGER_TOKEN_START 0x00
#define YJ4600_TOKEN		0x10	//Youjie2D 4600 is base on HI2D	1400
#define VOYAGER_TOKEN_END 0x1F

#define SUPPORTS_TWO_LEVEL_ILLUMINATION 0x00 //HW token used on first released board which only supports two levels of illumination and not 3




CHardwareToken *pCHardwareToken = NULL;

// ===  Function  ======================================================================
//
//  Name:  CreateHardwareToken
//
//  Description:  Creates the Hardware Token Class and calls the initialize function.
//      
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:   
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void CHardwareToken::CreateHardwareToken(void)
{
    if (!pCHardwareToken)
    {
        pCHardwareToken = new CHardwareToken();
		ASSERT(pCHardwareToken != NULL);

		if(pCHardwareToken)
		{
           pCHardwareToken->InitHardwareToken();
		}	        
    }
}

// ===  Function  ======================================================================
//
//  Name:  DestroyHardwareToken
//
//  Description: Destroys the hardware token class if needed.
//      
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:   
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void CHardwareToken::DestroyHardwareToken(void)
{
	if(pCHardwareToken)
	{
    	delete pCHardwareToken;
    	pCHardwareToken = NULL;
	}
}

// ===  Function  ======================================================================
//
//  Name:  CHardwareToken
//
//  Description: Create the Hardware Token Class and Initialize member variables
//      
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:   
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
CHardwareToken::CHardwareToken()
{
	m_HWTValue = 0x3F;
};

// ===  Function  ======================================================================
//
//  Name:  InitHardwareToken
//
//  Description:  Initializes the hardware token pins so they can be read.  Then reads 
//		the pin value to the member variable.  After we read the value, we configure the
//		pins as outputs, we write the token value back to the port and then we disable 
//		the use of the internal pullup resistors.  We do this because
//		the external hw token resistors only pull the value down to ground.  For a high,
//		we rely on the internal pullups of the micro.  Having them as outputs that match
//		the read values ensures we don't have any floating inputs and there's no current
//		flowing through the gpio line.  The actual hardware value is only read once.
//      
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:   
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void CHardwareToken::InitHardwareToken(void)
{

	DISABLE_INTERRUPTS();
	
#if PLATFORM_CURRENT == imx25

	//Configure the HWT1 (PORT3_6)
	HAZARD_WRITE_FIELD_TO_REG(IOMUXC_SW_MUX_CTL_PAD_FEC_MDIO, IOMUXC_SW_MUX_CTL_MUX_MODE_LSH, IOMUXC_SW_MUX_CTL_MUX_MODE_WID, MUX_MODE_5);	//MUX To GPIO
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_FEC_MDIO, 0x000000E0);	//PAD Regiser, 100K ohm Pullups
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0, GPIO_BIT6_LSH, GPIO_BIT6_WID, GPIO_GDIR_INPUT);	//Direction Register

	//Configure the HWT2 (PORT3_7)
	HAZARD_WRITE_FIELD_TO_REG(IOMUXC_SW_MUX_CTL_PAD_FEC_TDATA0, IOMUXC_SW_MUX_CTL_MUX_MODE_LSH, IOMUXC_SW_MUX_CTL_MUX_MODE_WID, MUX_MODE_5);	//MUX To GPIO
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_FEC_TDATA0, 0x000000E0);	//PAD Regiser, 100K ohm Pullups
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0, GPIO_BIT7_LSH, GPIO_BIT7_WID, GPIO_GDIR_INPUT);	//Direction Register

	//Configure the HWT3 (PORT3_8)
	HAZARD_WRITE_FIELD_TO_REG(IOMUXC_SW_MUX_CTL_PAD_FEC_TDATA1, IOMUXC_SW_MUX_CTL_MUX_MODE_LSH, IOMUXC_SW_MUX_CTL_MUX_MODE_WID, MUX_MODE_5);	//MUX To GPIO
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_FEC_TDATA1, 0x000000E0);	//PAD Regiser, 100K ohm Pullups
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0, GPIO_BIT8_LSH, GPIO_BIT8_WID, GPIO_GDIR_INPUT);	//Direction Register

	//Configure the HWT4 (PORT3_9)
	HAZARD_WRITE_FIELD_TO_REG(IOMUXC_SW_MUX_CTL_PAD_FEC_TX_EN, IOMUXC_SW_MUX_CTL_MUX_MODE_LSH, IOMUXC_SW_MUX_CTL_MUX_MODE_WID, MUX_MODE_5);	//MUX To GPIO
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_FEC_TX_EN, 0x000000E0);	//PAD Regiser, 100K ohm Pullups
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0, GPIO_BIT9_LSH, GPIO_BIT9_WID, GPIO_GDIR_INPUT);	//Direction Register

	//Configure the HWT5 (PORT3_10)
	HAZARD_WRITE_FIELD_TO_REG(IOMUXC_SW_MUX_CTL_PAD_FEC_RDATA0, IOMUXC_SW_MUX_CTL_MUX_MODE_LSH, IOMUXC_SW_MUX_CTL_MUX_MODE_WID, MUX_MODE_5);	//MUX To GPIO
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_FEC_RDATA0, 0x000000E0);	//PAD Regiser, 100K ohm Pullups
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0, GPIO_BIT10_LSH, GPIO_BIT10_WID, GPIO_GDIR_INPUT);	//Direction Register

	//Read HWT
	m_HWTValue = 0x01;//READ_FIELD(GPIO3_PSR0,GPIO_BIT5_LSH,Width_6);	//Read HWT Value

	// Write HWT value back as outputs
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT5_LSH,Width_6,0); //Write values to Outputs
	//HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0,GPIO_BIT5_LSH,Width_6,0x3F); //Set HWT to outputs
	
	//PAD Regiser No PullUps for HWT
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_FEC_RDATA0,0x00000000);	//HWT5
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_FEC_TX_EN,0x00000000);	//HWT4
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_FEC_TDATA1,0x00000000);	//HWT3
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_FEC_TDATA0,0x00000000);	//HWT2
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_FEC_MDIO,0x00000000);	//HWT1
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_FEC_MDC,0x00000000);	//HWT0		
#endif //endof PLATFORM_CURRENT == imx25

	RESTORE_INTERRUPTS();

} //endof InitHardwareToken


// - Am_I? ------------------------------------------------
//  HAP (Access Point)
// -------------------------------------------------------
bool CHardwareToken::IAmHAP(void)
{
	return false;
}
	
// - Am_I? ------------------------------------------------
// CCB (Charge Communication Base)
// -------------------------------------------------------
bool CHardwareToken::IAmCCB(void)
{
	return false;
}

bool CHardwareToken::IAmCCBType(void)
{
	return false;
}

// - Am_I? ------------------------------------------------
// COB (Charge Only Base)
// -------------------------------------------------------
bool CHardwareToken::IAmChargeOnlyBaseType(void)
{
	return false;
}



// - Am_I? ------------------------------------------------
// a  3-level illumination Xenon board    
// TODO:  This is always called with one of the IAmOEM methods as getting OR'd with this.
//        Maybe we can replace everything with a simpler IHave3LevelIllumination() method
//        or better yet remove this entirely and just use the IAmTwoLevelIllumination() method.
// -------------------------------------------------------
bool CHardwareToken::IAmThreeLevelIllumination_NonOem(void)
{
	return false;
}



// - Am_I? ------------------------------------------------
// an OEM Board
// -------------------------------------------------------
bool CHardwareToken::IAmOEM(void)
{
	return ((OEM_TOKEN_START <= m_HWTValue) && (m_HWTValue <= OEM_TOKEN_END));
}

// - Am_I? ------------------------------------------------
// an OEM board with a Jade Imager
// The Engine should be read to determine Imager Type
// -------------------------------------------------------
bool CHardwareToken::IAmOEMJade(void)
{
	if(FIVE_MEG_PIX_TOKEN != m_HWTValue)
	{
		return ( IAmOEM() );
	}
	else
	{
		return false;
	}
}

// - Am_I? ------------------------------------------------
// an OEM Prototype
// Use this function for pinchanges only
// -------------------------------------------------------
bool CHardwareToken::IAmOEMPrototype(void)
{
	return ((FIVE_MEG_PIX_TOKEN == m_HWTValue) ||(JADE_TOKEN == m_HWTValue));
}

// - Am_I? ------------------------------------------------
// Vuquest 3310
// -------------------------------------------------------
bool CHardwareToken::IAmVuquest3310(void)
{
	return FALSE;
}

// - Am_I? ------------------------------------------------
// Voyager 2D
// -------------------------------------------------------
bool CHardwareToken::IAmVoyager1400(void)
{
#if defined(YJ2D_BASED_ON_HI2D)
	return (YJ4600_TOKEN == m_HWTValue);
#else
	return (0x1 == m_HWTValue);
#endif
}

// - Am_I? ------------------------------------------------
// Voyager 2D
// -------------------------------------------------------
bool CHardwareToken::IAmYoujie4600(void)
{
#if defined(YJ2D_BASED_ON_HI2D)
	return (YJ4600_TOKEN == m_HWTValue);
#else
	return false;
#endif
}
// - Do_I? ------------------------------------------------
// Returns the token so the interface can determine the 
//	interface configuration
// -------------------------------------------------------
int CHardwareToken::InterfaceToken(void)
{
	return  m_HWTValue;
}

// - Am_I? ------------------------------------------------
// Only Support the Low power Table
// -------------------------------------------------------
bool CHardwareToken::IAmLowXmitPowerScanner(void)
{
	return true;	//there is no plan to use the High power table as of this time
}

// - Am_I? ------------------------------------------------
// Only Support 2 Illumination Settings
// -------------------------------------------------------
bool CHardwareToken::IAmTwoLevelIllumination(void)
{
	return true;//HI2D two level current
}



/*----------------------------------------------------------------------------------------
IAmIndustrial

  Checks HW token to see if we're an Industrial.

  Parameters:  none

  Returns:  true if token says we're Industrial.
----------------------------------------------------------------------------------------*/
bool CHardwareToken::IAmIndustrial(void)
{
	return false;
}





