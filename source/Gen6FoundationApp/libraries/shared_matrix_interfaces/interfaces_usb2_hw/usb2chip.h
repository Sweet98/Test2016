/*=================================================================================
  class: CUsb2Chip
	// hardware specific file

//=================================================================================
   $RCSfile: interfaces_usb2_hw/usb2chip.h $
   $Revision: 1.33 $
   $Date: 2011/10/11 11:10:23EDT $

//=================================================================================

//=================================================================================*/
//! \file

#ifndef USB2_CHIP_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define USB2_CHIP_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "Usb2Base.h"
#include "Usb2_mx25.h"
#include "interrupts.h"

#include "threadsimple.h"

#define RECONNECT_TEST_MS 100

class CUsb2Chip : public CUsb2Base
{	
public:
	CUsb2Chip(CRxFifo *pRxFifo, CThreadRealTime *pThread);
	virtual ~CUsb2Chip(void);

	virtual MatrixError_t Close(void);
	virtual MatrixError_t SetupHardware(void);
	
	CREATE_SETVECTOR_MEMBER(CUsb2Chip);

	virtual void Reconnect(void);      
	virtual void connect_USB(void);
	virtual void disconnect_USB(void);
	virtual void TimeoutHack(UCHAR EpNum, UINT toutNum, bool ReCon);
	void ResetDevice (void);	
	UINT MX25USB2_WriteEndpoint(UCHAR bEndp, UINT len, const UCHAR * buf);
		
protected:
	void connect_USBHw(void);
	void MX25USB2_busreset(void);
	void MX25USB2_FlushAllBuffers(bool EndpointSetupFix);
	void DataStageEnable(void);
	void StatusAcknowledge(UCHAR EpNum);

	void EP0_setup(void);
	void EP0_rxdone(void);
	
	void MX25USB2_filldtd(UCHAR EpNum, UCHAR Epdtd, UINT len, UCHAR dir, volatile const UCHAR *pBuf);
	void MX25USB2_filldtd_ctrl(UCHAR Epdtd, UINT len, UCHAR dir, volatile const UCHAR *pBuf);
	void MX25USB2_qu2ep_zlp(UCHAR bIN);
	void MX25USB2_qu2ep(UCHAR EpNum, UCHAR Epdtd, UINT len, UCHAR dir, volatile const UCHAR *buf);
	void MX25USB2_qu2ep_ctrl(UCHAR Epdtd, UINT len, UCHAR dir, volatile const UCHAR *buf);
	void PortChange_isr(void);	
	void usbclk_enable(void);
	void MX25USB2_setxcvr(void);
	void Init_MX25USB2Otg(void);
	void Rst_MX25USB2(void);
	void MX25USB2_dQHsetup(void);
	void MX25USB2_stop(void);
	void MX25USB2_Setup(void);
	void MX25USB2_ep_qh_setup(UCHAR ep_num, UCHAR dir, UCHAR ep_type, UINT max_pkt_len, UINT zlt, UCHAR mult);
	void MX25USB2_dr_ep_setup(UCHAR ep_num, UCHAR dir, UCHAR ep_type);
	void MX25USB2_ep0_setup(void);
	void MX25USB2_eps_setup(bool First);
	void MX25USB2_eps_dis(UCHAR ep_num);
	void Init_MX25USB2(void);

	void MX25USB2_EP0_Rx(void);		
	virtual void EPRxHandleReceivedData(UINT){};
	virtual void ReceiveKbdData(UINT){};

	virtual	void stall_ep0(void){};
	virtual	void stall_eps(UCHAR, UCHAR){};

protected:	
	void MX25USB2_ReadEndpoint(UCHAR bEndp, UINT len, bool ctrl, volatile UCHAR *buf);
	UINT MX25USB2_ReadEndpoint_Data(UCHAR bEndp, UINT maxlen, bool ctrl, volatile UCHAR *buf);
	void MX25USB2_WriteEPs2bytes(UCHAR EpNum);
	void MX25USB2_SetEndpointStatus(UCHAR bEndp,UCHAR bStalled);
	UCHAR MX25USB2_GetEndpointStatus(UCHAR bEndp);

	threadhandle_t Usb_Exception(void);

	MatrixError_t SetupHardwareAfterReset(void);
	MatrixError_t PrepareForHid(void);

	void Susp_Resume_Thread(void);
	bool EpHasData(UCHAR EpNum);
	void Ep2TxCompletehandler(void);		
		
protected:	
	UINT m_UsbAddress;
	vector_number_t VEC_USB2_INT;
	MT_Event_t Usb2BeepDownEvent;
	MT_Event_t Usb2EP2RxIrqEvent;
	MT_Event_t Usb2EP4TxIrqEvent;
	MT_Event_t Usb2EP0setupIrqEvent;
	MT_Event_t SuspResEvent;
	volatile bool m_SpdHighEnabledS;
		

private:
	CThreadSimple mTheadSusp;	
	volatile bool mSuspended;
	volatile UINT totalDEP;
	
// For chapter 9
protected:
	virtual void control_handler(void){};
	volatile bool DataStageEn;  	
};

#endif /* USB2_CHIP_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */


