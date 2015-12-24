/* ==============================================================================
   Common base class for USB drivers
//=================================================================================
   $Source: interfaces_usb2/base_usb2/usb2driverbase.h $
   $Revision: 1.8 $
   $Date: 2011/11/14 13:33:50EST $

//=================================================================================

//=================================================================================*/
//! \file

#ifndef _USB2_BASE_DRIVER_H_CDD7933B_4209_4291_B69D_1788BB71CCDC__INCLUDED
#define _USB2_BASE_DRIVER_H_CDD7933B_4209_4291_B69D_1788BB71CCDC__INCLUDED

#include "ThreadedDriver.h"
#include "Usb2base.h"
#include "timeout.h"

class CBarcodeData;

#define baseCDriverUsb2Base CThreadedDriver

class CDriverUsb2Base : public baseCDriverUsb2Base
{
public:
	CDriverUsb2Base(size_t BufferSizeTx = DEFAULT_BUFFER_SIZE_TX, size_t BufferSizeRx = DEFAULT_BUFFER_SIZE_RX);
	virtual ~CDriverUsb2Base();

	virtual MatrixError_t Open(interface_id_t nId, const tichar_t *pParameter);
	virtual MatrixError_t Close();
	virtual IoControlRet_t DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam);

	virtual void ThreadLoop(void);
	virtual size_t Write(CBarcodeData  *pLabel);	//default writes to a fifo
	virtual size_t Write(const bchar_t *pBin, size_t nSize) // add these to remove warnings
	{
		return CDriverBase::Write(pBin,nSize);
	};
	virtual size_t Write(const wchar_t *pText, size_t nSize)
	{
		return CDriverBase::Write(pText,nSize);
	};

protected:
	virtual void InitInstance(void);
	virtual int StartSending(void);
	virtual void SetInterfaceError(int Err);

	virtual void PrepareCableInterfaceforUSBPowerSave(void);


	CUsb2Base	*m_pUsb;
	static CTimeout ms_EnsureReconnectTime;
	static bool ms_NeedReconnectDelay;
	static bool ms_TimeoutInitialized;
	int mErrorUsb;

	bool m_FirstTime;

private:
	CDriverUsb2Base (const CDriverUsb2Base&);            // declared private, but not defined, to prevent copying         
	CDriverUsb2Base& operator=(const CDriverUsb2Base&);  // declared private, but not defined, to prevent copying 

public:
};

#endif	// _USB2_BASE_DRIVER_H_CDD7933B_4209_4291_B69D_1788BB71CCDC__INCLUDED

