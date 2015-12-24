/*===================================================================
  See the cpp file for information. 

=====================================================================
   $Source: Gen6FoundationApp/FoundationApp/Peripheral_Interfaces/IndicatorTfb.h $
   $Date: 2011/11/18 16:59:30EST $
   $Revision: 1.1 $


===================================================================*/
#ifndef _IndicatorTfb_h_
#define _IndicatorTfb_h_


class IndicatorTfb
{
public:
	static void CondInit(void);
	static void CondOn(void);
	static void CondOff(void);

protected:
	static unsigned int m_StartTimeTicks;
};



#endif // endof #ifndef _IndicatorTfb_h_





