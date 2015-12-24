/* ==============================================================================
   USB SurePos Module
//=================================================================================
   $Source: interfaces_usb2/hid_usb2/surepos_usb2/drvusb2surepos.cpp $
   $Revision: 1.7 $
   $Date: 2010/06/08 15:00:12EDT $

//=================================================================================

//=================================================================================*/
//! \file

#include "stdInclude.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "CommonDriver.h"
#include "DrvUsb2SurePos.h"
#include "Usb2Hidsurepos.h"
#include "BarcodeData.h"

// This is the class factory for the driver (IBM SurePos hand held device)
CDriverBase *CreateUsbSurePosHH(void)
{
	return (new CDriverUsb2SurePos(false));
}

// This is the class factory for the driver (IBM SurePos table top device)
CDriverBase *CreateUsbSurePosTT(void)
{
	return (new CDriverUsb2SurePos(true));
}

CDriverUsb2SurePos::CDriverUsb2SurePos(bool TableTop)
{
	m_pUsb = new CUsb2HidSurePos(m_pRxFifo, NULL, TableTop);	// create the USB component
	m_pUsb->SetParentDriver(this);
	ASSERT(m_pUsb != NULL);
}

CDriverUsb2SurePos::~CDriverUsb2SurePos()
{
	delete m_pUsb;
}

inline CUsb2HidSurePos* CDriverUsb2SurePos::GetSurePosObject(void)
{	
	return (CUsb2HidSurePos*)m_pUsb;
}

// This virtual overload is called as the very first action of the thread
void CDriverUsb2SurePos::InitInstance(void)
{
	baseCDriverUsb2SurePos::InitInstance();
}

void CDriverUsb2SurePos::ThreadLoop(void)
{
	ULONG signal;
	PrepareWaitForSignal(Sig_ByteReceived);
	PrepareWaitForSignal(Sig_FlashCommandReceived);
	while (TRUE)					// This is the drivers output loop
	{
		if (AnyDataToSend())		// Any characters to send?
		{
			StartSending();		// Yes, send them

		}
		else
		{			
			signal = WaitForAnySignal(Sig_NewBarcodeToSend | Sig_ByteReceived | Sig_FlashCommandReceived);	// wait until we have work to do
			if (signal & Sig_NewBarcodeToSend)
			{				
				PrepareWaitForSignal(Sig_NewBarcodeToSend);		// prepare for the next packet
			}
			else if( signal & Sig_ByteReceived )
			{
				PrepareWaitForSignal(Sig_ByteReceived);	
				GetSurePosObject()->TranslateSurePosCommand();	
			}
			else if( signal & Sig_FlashCommandReceived )
			{
				PrepareWaitForSignal(Sig_FlashCommandReceived);
				GetSurePosObject()->FlashCommandDispatcher();
			}
		}
	}
}

size_t CDriverUsb2SurePos::Write(CBarcodeData  *pLabel)
{	
	// If we get a Menu response, then we do not want to send it.
	// It would screw up the cash register. But we might need it in the translation layer or flash interface.
	if (pLabel->GetHHPId()==WA_CODELETTER_MENU_RESPONSE)
	{
		GetSurePosObject()->StoreMenuResponse(pLabel);  // bypass the fifo !
		return 1;                           // Done
	}

	size_t ret = baseCDriverUsb2SurePos::Write(pLabel);
	return ret;
}




