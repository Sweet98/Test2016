/*=================================================================================
  typename:

//=================================================================================
   $Source: interfaces_wedge_hw/Gpio.cpp $
   $Date: 2009/06/29 05:11:01EDT $
   $Revision:


//=================================================================================*/

#include "stdInclude.h"
#include "product.h"
#include "gpio.h"
#include "string.h"


CGpio::CGpio(GpioPort_t *pPort)
: m_pPort(pPort)
{

}

#ifndef USE_GPIO_INLINE_FUNCTIONS            //Done by Jess

///////////////////////////////////////////////////////////////////////////////////////////////////////
void CGpio::MakeOutput(ULONG bitmask)				{ SetPort(bitmask, AddrDDIR());	}
void CGpio::MakeInput(ULONG bitmask)				{ ClrPort(bitmask, AddrDDIR());	}

void CGpio::SetPort(ULONG bitmask)					{ SetPort(bitmask, AddrDR());		}
void CGpio::ClrPort(ULONG bitmask)					{ ClrPort(bitmask, AddrDR());		}

void CGpio::EnableIrq(ULONG bitmask)				{ SetPort(bitmask, AddrIMR());	}
void CGpio::DisableIrq(ULONG bitmask)				{ ClrPort(bitmask, AddrIMR());	}


#endif           

// The next functions help with the 2 bit configuration registers.
// Motorola could have done much easier with two registers for level and polarity, but they have choosen the worst.

// The "val" is translated into a bitmask based on the bitnumber
ULONG CGpio::Get2BitMaskFromNumber(bit_number_t Bitnumber, int val)
{
	ULONG index = (ULONG) Bitnumber & (BitNumBorder-1);  // limit range from 0..15
	ULONG mask = (val&1) ? GetMaskFromNumber((bit_number_t)(2*index)) : 0;
	mask 		 |= (val&2) ? GetMaskFromNumber((bit_number_t)(2*index+1)) : 0;
	return mask;
}

// Calculates the bitmask and config register address depending on bitnumber and val
// I could have used a __int64 based bit field, but I was afraid of big/little enduian influences
// 	if we ever change this mode. Also bit field arrays are illegal, so indexing would not work.
GpioPort_t *CGpio::GetIrqConfigAddr(bit_number_t Bitnumber)
{
	if(Bitnumber < BitNumBorder)
	{
		return AddrICR1();
	}
	else
	{
		return AddrICR2();
	}
}


