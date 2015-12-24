/* ==============================================================================
   USB POS Module
//=================================================================================
   $Source: interfaces_usb2/hid_usb2/pos_usb2/drvusb2pos.cpp $
   $Date: 2010/10/15 14:54:32EDT $
   $Revision: 1.4 $

//=================================================================================

//=================================================================================*/

#include "stdInclude.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "product.h"
#include "CommonDriver.h"
#include "DrvUsb2Pos.h"
#include "usb2hidpos.h"
#include "BarcodeData.h"

// This is the class factory for this driver
CDriverBase *CreateUsbPos(void)
{
	return (new CDriverUsb2Pos);
}

// Constructor
CDriverUsb2Pos::CDriverUsb2Pos()
{
	m_pUsb = new CUsb2HidPos(m_pRxFifo, NULL);	// create the USB component
	ASSERT(m_pUsb != NULL);
	m_DriverSupportsProgrammableOutputFormats = true;
}

// Desstructor
CDriverUsb2Pos::~CDriverUsb2Pos()
{
	delete m_pUsb;
}

IoControlRet_t CDriverUsb2Pos::DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam)
{
	IoControlRet_t RetVal=baseCDriverUsb2Pos::DeviceIoControl(cmd, lparam, wparam);
	switch(cmd)
	{
		case IOCTRL_EXPLICITLY_ENABLE_SCANNER:		// Called by reconfigure interface to enable
			if( m_pUsb->GetScannerDisableStatus() )	// the scanner on startup in USB HID
			{
				RetVal = IOCRET_NO;		// Scanner should be disabled, do not enable it
			}
			else
			{
				RetVal = IOCRET_YES;	// Scanner should be enabled
			}
			break;
		default:
			break;
	}

	return RetVal;
}





