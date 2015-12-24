/*===================================================================
  This module holds two classes (because they're small and related).
  
  The class IndicatorTfb is the application's interface to the
  Tactile Feedback (TFB) device.  The methods all begin with
  "Cond" because they all conditionally call the driver (the TFB
  must be present).  The intent is to help reduce the number of
  "if statements" in the code.  We also wrap the contents of the methods
  with RFBASE macros so we can simply include the code in all builds
  and the compiler will see there's no loc and thus there is no
  additional overhead.
  
  Because of USB current limitations, the TFB can't be on when the
  beeper, the Indicator LEDs or the Illumination LEDs are on.
  
  TODO:  see if it's safe to check for USB enumeration here (these methods
  get called inside an IRQ).  It'll be more robust/future proofed.
   
  
  The class Tfb_Driver is the driver for the TFB.  It is hidden
  from the rest of the application coded.
  
=====================================================================
   $Source: Gen6FoundationApp/FoundationApp/Peripheral_Interfaces/IndicatorTfb.cpp $
   $Date: 2011/11/18 16:59:29EST $
   $Revision: 1.1 $


===================================================================*/
#include "IndicatorTfb.h"
#include "menu.h"
#include "ProductConfiguration.h"
// includes for driver
#include "HardwareMacros.h"		//Provides Macros for setting registers
#include "iMX25_IOMUX.h"
#include "iMX25_GPIO.h"
#include "db_time.h"
#include "usage_statistics.h"


#if 0
#include "OsTimer.h"
#include "db_chrio.h"
#define DBG_TRACE(str)	do {char ValStr[50];sprintf(ValStr, "%d: ", read_ms_clock());DEBUG_puts(ValStr); DEBUG_puts(str);DEBUG_puts("\n");} while (0)
#else
#define DBG_TRACE(str)
#endif // endof #if 0


unsigned int IndicatorTfb::m_StartTimeTicks = 0;

class Tfb_Driver
{
public:
	static void Init(void);
	static void On(void);
	static void Off(void);
};



/*----------------------------------------------------------------------------------------
CondInit

  Contionally calls the driver's Init() method.

  Parameters:  none

  Returns:  nothing
----------------------------------------------------------------------------------------*/
void IndicatorTfb::CondInit(void)
{
#ifndef RFBASE
	if ( GET_CONFIG_SETTING(m_TfbPop) ) Tfb_Driver::Init();
#endif
}


/*----------------------------------------------------------------------------------------
CondInit

  Contionally calls the driver's Init() method.

  Parameters:  none

  Returns:  nothing
----------------------------------------------------------------------------------------*/
void IndicatorTfb::CondOn(void)
{
#ifndef RFBASE
	if ( GET_CONFIG_SETTING(m_TfbPop) )
	{
		m_StartTimeTicks = current_time();
		Tfb_Driver::On();
	}
#endif
}


/*----------------------------------------------------------------------------------------
CondOff

  Contionally calls the driver's Off() method.

  Parameters:  none

  Returns:  nothing
----------------------------------------------------------------------------------------*/
void IndicatorTfb::CondOff(void)
{
#ifndef RFBASE
	if ( GET_CONFIG_SETTING(m_TfbPop) )
	{
		Tfb_Driver::Off();
		unsigned int OnTimeTicks = current_time() - m_StartTimeTicks;
		UsageStatistics.m_TfbOnTime.AddTime(translate_ticks_to_ms(OnTimeTicks));
	}
#endif
}






/*----------------------------------------------------------------------------------------
Init

  Inits the pin for GPIO output.  We use GPIO 3, bit 19.

  Parameters:  none

  Returns:  nothing
----------------------------------------------------------------------------------------*/
void Tfb_Driver::Init(void)
{
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_POWER_FAIL, MUX_MODE_5);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_POWER_FAIL, 0);	// Nominal drive strength, no pullups
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0, GPIO_BIT19_LSH, GPIO_BIT19_WID, GPIO_LOW);
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0, GPIO_BIT19_LSH, GPIO_BIT19_WID, GPIO_GDIR_OUTPUT);
}
	
	

/*----------------------------------------------------------------------------------------
On

  Sets the GPIO pin to turn on the TFB.

  Parameters:  none

  Returns:  nothing
----------------------------------------------------------------------------------------*/
void Tfb_Driver::On(void)
{
	DBG_TRACE("On");
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0, GPIO_BIT19_LSH, GPIO_BIT19_WID, GPIO_HIGH);
}



/*----------------------------------------------------------------------------------------
Off

  Clears the GPIO pin to turn off the TFB.

  Parameters:  none

  Returns:  nothing
----------------------------------------------------------------------------------------*/
void Tfb_Driver::Off(void)
{
	DBG_TRACE("Off");
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0, GPIO_BIT19_LSH, GPIO_BIT19_WID, GPIO_LOW);
}




