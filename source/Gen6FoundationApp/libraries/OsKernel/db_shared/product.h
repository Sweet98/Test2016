/* ==============================================================================
   Product Specific Definitions Module
   ==============================================================================

   $RCSfile: db_shared/product.h $
   $Revision: 1.3 $
   $Date: 2010/05/04 11:40:57EDT $

   ==============================================================================


   ============================================================================== */

#ifndef _PRODUCT_INCLUDED
#define _PRODUCT_INCLUDED

#include <limits.h>
#include "language.h"
#include "ErrorDefs.h"
#include <assert.h>

// -----------------------------------------------------------------------
// Product IDs
// -----------------------------------------------------------------------
// to do
// -----------------------------------------------------------------------
// USB engine configuration
// -----------------------------------------------------------------------
#define USB_BASE_PRODUCT_ID					0x0160	/* PID range (0..1f) */

#if defined(RFBASE)	
	#define USB_POWER					500
	#define USB_BM_ATTRIBUTES 			0xE0	//USB_SELF_POWERED, USB_SUPPORTS_REMOTE_WAKEUP
#else
	#define USB_POWER					400
	#define USB_BM_ATTRIBUTES 			0xA0	//USB_BUS_POWERED, USB_SUPPORTS_REMOTE_WAKEUP

#endif //endof else RFBASE

#define REVISION_BCD					0x0000       //lmc   Testing for surepos EC #, revision is used as a hex value in IBM Sure-
													//Pos, upper byte is generation, lower byte is level, as defined by IBM.
	
typedef enum 
{
	CABLE_POWERED,		//Power is coming from the 10 pin jack
	AUX_POWERED,		//The AC Adapter is supplying power
	BOTH_POWERED,		//Power is coming from both inputs
	NONE				//This cannot happen
} BASE_POWER_OPTIONS;

#if defined(RFBASE)
	void UpdateInputPowerStatus(void);
#endif //endof defined(RFBASE)	

BASE_POWER_OPTIONS InputPowerStatus(void);	

											    
// -----------------------------------------------------------------------
// debug helpers etc.
// -----------------------------------------------------------------------
#define  SIZEOF_MAX_MESSAGE 3000

#endif

