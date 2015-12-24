/*=================================================================================
  class:

//=================================================================================
   $Source: interfaces_usb2/base_usb2/usb2base.h $
   $Revision: 1.31 $
   $Date: 2011/11/14 13:33:49EST $

//=================================================================================

//=================================================================================*/

#ifndef USB2_BASE_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define USB2_BASE_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "usb2.h"
#include "usbdesc.h"
#include "DescriptorQueues.h"
#include "hstring.h"
#include "ThreadRealtime.h"


#define MAX_PACKET_SIZE	64  // Unfortunately, there was no macro defined for this early on, so you'll see it hardcoded in lots of places


class CRxFifo;
class CThreadRealTime;
class CBarcodeData;
class CThreadedDriver;

class CUsb2Base
{
public:
	CUsb2Base(CRxFifo *pRxFifo, CThreadRealTime *pThread);
	virtual ~CUsb2Base();
	
	virtual MatrixError_t Close(void) { return ERR_PASS;	};

	UCHAR IsConfigured(void) { return m_BusState >= USB_CONFIGURED;	}
	void WaitUntilWeAreConfigured(void);
	virtual bool IsTxComplete(void) {return true;} // overridden in cdcacm etc
	virtual bool IsRemTxComplete(void) {return true;}
	virtual bool IsCtsInActive(void) {return true;}
	virtual bool IsEnabled_CTS(void) {return false;}
	virtual usb_err_t Ep0DataHandler(void)=0;

	virtual UINT GetEPnum(void) 
	{
		return termid_eps;		
	}
	
//datapump part
	virtual int SendLabel(CBarcodeData *pLabel) = 0;
	virtual void SendToRem(const unsigned char * pData, int length){};  // drvUsbRem will call this when it wants to send data.  This must be derived from a class higher than CUsb2CHip so it can pass the pointer to it (see CUsbRem::SendData()). 
	void SetRemRxFifo(CRxFifo *pRxFifo) {m_pRemRxFifo = pRxFifo;} // drvUsbRem will call this when it opens ReM interface.
	virtual MatrixError_t ExtractParameters(const tichar_t *pParameters );
	virtual MatrixError_t SetupHardware(void) = 0; 
	virtual void disconnect_USB(void) {}
	virtual void connect_USB(void) {}
	virtual void Reconnect(void) {}
	virtual void TimeoutHack(UCHAR /*EpNum*/, UINT /*toutNum*/, bool /*ReCon*/){}
	void RemoteWakeUp(void);
	bool IsRemoteWakeUpEn(void);
	bool IsBeepEnabled(void) { return m_BeepEnabled&m_TxEnabled;}
	bool IsTxEnabled(void) { return m_TxEnabled;}
	void SetDeferedPowerupBeep(void) { m_usb2DeferedPowerUpBeep = true;}
	void StoreMenuResponse(CBarcodeData *pMenuResponse);  // see m_pMenuResponse	
	int mUsbRecoveryTimeout;
//datapump part end
	void SetParentDriver( CThreadedDriver *parentDriver );
	virtual bool GetScannerDisableStatus(void) { return false; } // dummy response by default
	
protected:
	CThreadedDriver *m_ParentDriver;
	volatile USBD_QH_T *usbd_qh_basemem;
	volatile USBD_QH_T *usbd_qh_base;
	volatile CONTROL_XFER ControlData;
	EPPFLAGS volatile bEPPflags;
	volatile UCHAR	m_BusState;       // USB status
	volatile bool m_BandwithSaveMode;	
	volatile bool m_cts_active;
	volatile bool m_SpdHighEnabled;
	volatile bool m_dataBufCtrl;
			
	volatile UINT ucControlState;	
	volatile UINT ucControlStateLine;	
	volatile UCHAR *dataFromEP;  
	volatile UCHAR RemOutEndpoint[MAX_PACKET_SIZE];  
	volatile UCHAR *dataFromEPsec;  
	volatile UCHAR *dataToEP;  
	volatile UINT datalenEP;
	volatile UINT maxTxdata;

	//For kbd
	volatile bool m_datakbdBufCtrl;	
	volatile UCHAR *datakbdFromEPsec;  
	volatile UCHAR *datakbdFromEP; 			
	
	volatile UINT termid_eps;
	volatile UINT rxMaxLenIDs;
	volatile UINT m_ConTimes;	
			
	volatile UINT pxProductStrDesLEN;
	volatile UINT pxSerialStrDesLEN;	
	const UCHAR *pxInterfaceStringDescriptor[2];
	volatile UINT pxInterfaceStrDesLEN[2];
		
	const USB_DEVICE_DESCRIPTOR *DeviceDescr;
	const HS_DEVICE_QUALIFIER *HS_Device_Qualifier;
	
	UCHAR *usb_descr_FS;
	volatile UINT usb_descrLEN_FS;
	UCHAR *usb_descr_HS;
	volatile UINT usb_descrLEN_HS;
	UCHAR *usb_othersp_descr;	
	volatile UINT usb_othersp_descrLEN;

	CDescriptorQueue m_HidDescriptors;
	CDescriptorQueue m_ReportDescriptors;
	// TODO:  add CDescriptorQueue m_StringDescriptors;  (need to handle CustomizeUsbStrings())
	
	MT_Event_t MenuResponseEvent;  

	bool m_bRemIfaceEnabled;
	bool m_bSimpleInterfaceEnabled;

	virtual UCHAR GetSizeOfConfigDesc(void) {return 0;}  // this includes ReM
	virtual UCHAR GetMaxNumIfaces(void) {return 0;}  // this includes ReM
	virtual UCHAR GetPidOffset(bool) {return 0;}

	void AttachDevDescriptors(const USB_DEVICE_DESCRIPTOR *pChain, const HS_DEVICE_QUALIFIER *pChainQ);	
	void AttachConDescriptors(UCHAR *pChain, UINT index);
	void AttachStringDescriptors(const UCHAR *pChain, UINT index);

	void AdjustAllConfigDescriptorsForRem(void);
	void AdjustThisConfigDescriptorForRem(ConfigDescriptor_t * pConfigDescr, volatile UINT * pDescrLength);

	// These functions will be overridden by interfaces that need to remove secondary interfaces
	virtual void AdjustAllDescriptorsForSimpleInterface(void);
	virtual void AdjustThisConfigDescriptorsForSimpleInterface(ConfigDescriptor_t * pConfigDescr, volatile UINT * pDescrLength) {return;}
	
	void EnableUsbSerialNumber(USB_DEVICE_DESCRIPTOR *pChain, bool enable);
	void PatchDeviceDesc(USB_DEVICE_DESCRIPTOR *pDeviceDesc);
	
	void PatchConfigDesc(USB_CONFIG_DESCRIPTOR *pConfigDesc);
		
	void PatchString(UCHAR *pDest, HSTRING *pSource);	
	void CustomizeUsbStrings(void);
	void SetQueuesToNonCached(void);

	// some overloads to stroe a string into the input buffer (fifo). This has the same effect as received data.
	size_t StuffInputBuffer( const UCHAR* pcInputData, size_t size );
	size_t StuffInputBuffer( const UCHAR* pcInputData );
	size_t StuffInputBuffer( const char* pcInputData )
	{
		return StuffInputBuffer( (const UCHAR*)pcInputData );
	}

	// sleep and wake the thread
	void MT_Sleep(timer_uSec_t uSec)						{ m_pThread->Sleep(uSec);								}
	void PrepareWaitForSignal(ULONG SignalMask)		{ m_pThread->PrepareWaitForSignal(SignalMask); }
	ULONG WaitForAnySignal(ULONG SignalMask)			{ return m_pThread->WaitForAnySignal(SignalMask);}
	ULONG WaitForAnySignal(ULONG SignalMask, ULONG uSec)	{ return m_pThread->WaitForAnySignal(SignalMask, uSec);	}
	void Signal(ULONG SignalMask)							{ m_pThread->Signal(SignalMask);						}

	// Allow drivers to control certain features
	void EnableBeeper(void)									{ m_BeepEnabled=true; }
	void DisableBeeper(void)								{ m_BeepEnabled=false; }
	void EnableTx(void)									{ m_TxEnabled=true; }
	void DisableTx(void)									{ m_TxEnabled=false; }										
	bool m_usb2DeferedPowerUpBeep;
	CBarcodeData *GetMenuResponse(void)	{ return m_pMenuResponse; } // see m_pMenuResponse
	void FreeMenuResponse(void); 	// see m_pMenuResponse
	void StuffInputBufferAndWaitForMenuResponse(const bchar_t *pMenuString);
	CThreadRealTime *m_pThread;
	CRxFifo *m_pRemRxFifo;

private:
	CRxFifo *m_pRxFifo;
	bool m_BeepEnabled;
	bool m_TxEnabled;
	CBarcodeData *m_pMenuResponse;	// used by some interfaces (SurePos) to feed the menu response directly to a dispatcher

	CUsb2Base (const CUsb2Base&);            // declared private, but not defined, to prevent copying         
	CUsb2Base& operator=(const CUsb2Base&);  // declared private, but not defined, to prevent copying 
	
//datapump part	end
};
	
//POS
class CHidInterface
{
	enum
	{
		HID_IDLE = 0x01
	};

public:
	CHidInterface()
	{
		m_supports_idle_requests = false;
		m_ucProtocol = 0;
		m_ucIdle = 0;
		m_sIdleCounter = 0;
	}

	usb_err_t SetIdle(UCHAR bReportID, UCHAR idle)
	{
		if(IsIdleSupported(bReportID))
		{
			m_ucIdle = idle;
			return RET_ACK;
		}
		else
		{
			return RET_STALL;
		}
	}

	void SetIdleSupported(UCHAR /* bReportID */)	{ m_supports_idle_requests = true;	}
	bool IsIdleSupported(UCHAR /* bReportID */)	{ return m_supports_idle_requests;	}

	UCHAR	GetIdle(UCHAR /* bReportID */)	 		{ return m_ucIdle;		}
	UCHAR	*GetIdleAddr(UCHAR /* bReportID */)		{ return &m_ucIdle;		}

	void	SetProtocol(UCHAR idle)						{ m_ucProtocol = idle;	}
	UCHAR	GetProtocol(void)								{ return m_ucProtocol;	}
	UCHAR	*GetProtocolAddr(void)						{ return &m_ucProtocol;	}

protected:

	bool m_supports_idle_requests;
	UCHAR m_ucProtocol;				// store for HID Set/Get Protocol
	UCHAR m_ucIdle;  					// idle rate reload (HID, SetIdle)
	short m_sIdleCounter;  			// idle cocunter (HID, SetIdle)
};
	
#endif /* USB2_BASE_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */

   
