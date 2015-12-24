/*=================================================================================
  class: CUsb2ComEmu

//=================================================================================
   $Source: interfaces_usb2/cdcacm_usb2/drvusb2comemu.cpp $
   $Revision: 1.6 $
   $Date: 2009/07/06 23:17:26EDT $

//=================================================================================

//=================================================================================*/

#include <stdio.h>
#include "stdInclude.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "product.h"
#include "CommonDriver.h"
#include "Usb2CdcAcm.h"
#include "BarcodeData.h"

#include "drvusb2comemu.h"

// This is the class factory for this driver
CDriverBase *CreateUsbComEmu(void)
{
	return (new CUsb2ComEmu);
}

CUsb2ComEmu::CUsb2ComEmu(void): CDriverUsb2Base(DEFAULT_BUFFER_SIZE_TX, 40*1024)
{
	bool UseAlternativPID = false;  

	m_pUsb = new CUsb2CdcAcm(m_pRxFifo, NULL, UseAlternativPID);	// create the USB component
	ASSERT(m_pUsb != NULL);
	m_DriverSupportsProgrammableOutputFormats = true;
}

// Desctructor
CUsb2ComEmu::~CUsb2ComEmu(void)
{
	delete m_pUsb;
}

   
