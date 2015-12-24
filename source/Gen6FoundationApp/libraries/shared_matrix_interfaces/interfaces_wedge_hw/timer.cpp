/*=================================================================================
  class:

//=================================================================================
   $Source: interfaces_wedge_hw/timer.cpp $
   $Date: 2009/06/29 05:11:42EDT $
   $Revision:


//=================================================================================*/

#include "stdInclude.h"
#include "product.h"
#include "timer.h"

static volatile int tempint;


void ShortDelay(int loops)
{
	volatile int del_cnt;
	for (del_cnt=0; del_cnt<loops; del_cnt++)
	{
		tempint = del_cnt;
	}
}


void GpTimerSoftwareReset(GPTimers_t *pTimer)
{
	pTimer->Tctl.u.b.EN = 1;
	pTimer->Tctl.u.b.SWR = 1;
	ShortDelay(5);	                 // minimum 3 cycle delay here
	pTimer->Tctl.u.b.EN = 0;
}
  /*============================================================================== */


