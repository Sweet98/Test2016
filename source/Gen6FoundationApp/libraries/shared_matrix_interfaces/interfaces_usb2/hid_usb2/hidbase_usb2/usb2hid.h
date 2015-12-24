/* ==============================================================================
   USB HID declarations
   ==============================================================================

   $RCSfile: interfaces_usb2/hid_usb2/hidbase_usb2/usb2hid.h $
   $Revision: 1.10 $
   $Date: 2011/03/31 14:38:47EDT $

//=================================================================================

//=================================================================================*/
//! \file

#ifndef _USB2HID_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define _USB2HID_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "usb2enum.h"

//HID bRequest Types
#define USB_HID_GET_REPORT      0x01
#define USB_HID_GET_IDLE        0x02
#define USB_HID_GET_PROTOCOL	0x03
#define USB_HID_SET_REPORT	0x09
#define USB_HID_SET_IDLE	0x0A
#define USB_HID_SET_PROTOCOL    0x0B

// NOTE:  be careful where you use the following.  In some instances, DeviceRequest.wValue does NOT have USB_DIRECTION_MASK logically ORd with value.  For example, in ReM, I see wValue set to 0x01, not 0x81
#define USB_HID_REPORT_IN				(0x01|USB_DIRECTION_MASK)
#define USB_HID_REPORT_OUT				0x02
#define USB_HID_REPORT_FEATURE_IN	(0x03|USB_DIRECTION_MASK)
#define USB_HID_REPORT_FEATURE_OUT	0x03

typedef struct tag_USB_HidScanner_Status
{                               	
		UINT ScannerDisabled	        :1;
		UINT MenuResponse		:1;
}USB_HidScanner_Status_t;

#define USB_HID_REPORT_TABLE_END		0x00

template <class T>
class THidReportTable
{
public:
	UCHAR ucType;  					// IN, OUT or Feature
	UCHAR ucReportId;  				// HID report ID 
	USHORT ReportSize;  				// HID report size 
	usb_err_t (T::*ReportHandler)(void);


	int GetReportSize(void)	const { return (int) ReportSize;	}

	bool IsThisReportEntry(UCHAR bReporttype, UCHAR bReportID) const
	{
		if(bReporttype == ucType)
		{
			if(bReportID == ucReportId)
			{
				return true;
			}
		}
		return false;
	}

	static void FindReportEntry(const THidReportTable<T> *&pTable, UCHAR bReporttype, UCHAR bReportID)
	{
		if(pTable != NULL)
		{
			while(pTable->ucType != USB_HID_REPORT_TABLE_END)
			{
				if(pTable->IsThisReportEntry(bReporttype, bReportID))
				{
					return;	// done
				}
				pTable++;	// try next entry
			};
		}
		pTable = NULL;	// not found
	}

};

class CUsb2Hid : public CUsb2Enum
{
public:
	CUsb2Hid(CRxFifo *pRxFifo, CThreadRealTime *pThread, size_t InSize=256, size_t OutSize=256);
	virtual ~CUsb2Hid(void);

	// connect this class to the USB setup dispatcher
	virtual void OnClassRequest(void);

	BOOL IsIdleActionRequired(int nInterface);

	// A helper to search trough the report table and call the required handler
	virtual usb_err_t ReportHandler(void)=0;

	// A helper to search trough the report table and see if we have a handler for this report
	virtual int GetReportSize(DEVICE_REQUEST *pRequest)=0;

	template <typename T>
	usb_err_t FindAndCallReportHandler(const THidReportTable<T> *pTable)
	{
		usb_err_t bRetVal = RET_STALL;					// assume a failure
		UCHAR bReporttype = MSB(ControlData.DeviceRequest.wValue);     //High;
		UCHAR bReportID = ControlData.DeviceRequest.wValue & 0xff;     //Low;			
		bReporttype |= ControlData.DeviceRequest.bmRequestType & USB_DIRECTION_MASK;	// distinguish feature in/out 

		THidReportTable<T>::FindReportEntry(pTable, bReporttype, bReportID);
		if(pTable != NULL)
		{
			usb_err_t (T::*pFunction)(void);				// a helper to call the member function
			pFunction = pTable->ReportHandler;			// get the member function
			bRetVal = (((T*)this)->*pFunction)();				// call the member function of CUsbHidPos
		}
		else
		{
		}
		return bRetVal;
	}

	// A helper to search trough the report table and call the required handler
	// It gets called by the HID framework for any get/set report request
	template <typename T>
	int GetReportSize(const THidReportTable<T> *pTable)
	{
		int size = -1;					// assume a failure
		UCHAR bReporttype = MSB(ControlData.DeviceRequest.wValue);     //High;
		UCHAR bReportID = ControlData.DeviceRequest.wValue & 0xff;     //Low;			
		bReporttype |= ControlData.DeviceRequest.bmRequestType & USB_DIRECTION_MASK;	// distinguish feature in/out 

		THidReportTable<T>::FindReportEntry(pTable, bReporttype, bReportID);
		if(pTable != NULL)
		{
			size = pTable->GetReportSize();			// get the size
		}
		return size;
	}
	
	virtual bool GetScannerDisableStatus(void);
	virtual void SendToRem(const unsigned char * pData, int length);
	virtual bool IsRemTxComplete(void);

protected:
	enum
	{
		NUM_HID_INTERFACES = 2
	};

	void OnSetClassInterfaceReq(void);
	void OnGetClassInterfaceReq(void);
	void SetIdle(void);
	void GetIdle(void);
	void SetProtocol(void);
	void GetProtocol(void);
	usb_err_t GetReport(void);	

	void SetReport(void);
	
	usb_err_t SendReportWithAck(UCHAR EpNum, int size, UCHAR *buffer);
	void SendReportWithTest(int nInterface, int size, UCHAR *buffer);

	CHidInterface *GetInterface(int nInterface)
	{
		ASSERT(nInterface < NUM_HID_INTERFACES);
		return m_pInterface[nInterface];
	}

	int GetDestinationInterface(void)			{ return m_ucDestinationInterface;	}

protected:	
	UCHAR m_ucNumInterfaces;			// number of interfaces
	UCHAR m_ucDestinationInterface;	// for redirecting to different interfaces

	volatile USB_HidScanner_Status_t	m_ScannerStatus;
	
	CHidInterface *m_pInterface[NUM_HID_INTERFACES];
	volatile UCHAR dataForHandler[327];  //263+64 	
};

#endif	/* _USB2HID_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */


