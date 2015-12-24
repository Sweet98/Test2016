/*=================================================================================
  class: CUsb2Chip

//=================================================================================
   $RCSfile: interfaces_usb2_hw/usb2chip.cpp $
   $Revision: 1.101 $
   $Date: 2011/11/11 00:42:17EST $

//=================================================================================

//=================================================================================*/
#include <stdio.h>
#include "stdInclude.h"

//#include "db_chrio.h"
#include "interrupts.h"
#include "platform.h"
#include "usb2chip.h"
#include "thread.h"
#include "usb2.h"
#include "usb2pos.h"
#include "UsbRem.h"
#include "timeout.h"
#include "beep.h"
#include "usb2resu.h"
#include "db_sharedtable.h"
#include "product.h"
#include "Interface_Support_Functions.h"
#include "Timer3_Driver.h" 

#if PLATFORM_CURRENT == imx25
#include "HardwareMacros.h"
#include "iMX25_GPIO.h"
#include "usb2_mx25.h"
#include "PowerManagement.h"
#else
#error No Platform Defined!
#endif //endof PLATFORM_CURRENT

#include "db_hdw.h"          /* artwork dependant hardware definitions    */

#if defined(SHOW_RESET_TO_BOOT) && !defined(MATRIX_BOOT_CODE)
#define reset_to_boot_code_hard() _reset_to_boot_code_hard(__LINE__)
void _reset_to_boot_code_hard(int);
#else
void reset_to_boot_code_hard(void);
#endif


// Uncomment USING_DEBUGGER if you're using the debugger.  This prevents the device from going into USB suspend which prevents JTAG interface from working.
// If you have app loaded in flash, you should update it with this new version.
//#define USING_DEBUGGER


extern "C"  void MMU_CleanAndInvalidate(void);

CUsb2Chip::CUsb2Chip(CRxFifo *pRxFifo, CThreadRealTime *pThread)
: CUsb2Base(pRxFifo, pThread)
{
	VEC_USB2_INT = VEC_USB_OTG_INT;

	DisableInterruptNumber(VEC_USB2_INT);		// safety
	SetVector(&CUsb2Chip::Usb_Exception,VEC_USB2_INT);

	m_UsbAddress = 0;
	mSuspended = false;
	DataStageEn = false;
	totalDEP = 0;
	Usb2BeepDownEvent = MT_CreateEvent(false);
	Usb2EP2RxIrqEvent = MT_CreateEvent(false);
	Usb2EP4TxIrqEvent = MT_CreateEvent(false);
	Usb2EP0setupIrqEvent=MT_CreateEvent(false);
	SuspResEvent = MT_CreateEvent(false);

	TCallBackTranslator<CUsb2Chip> CallBackSusp(this, &CUsb2Chip::Susp_Resume_Thread);
	mTheadSusp.PrepareRun(IFACE_PRIORITY+5,CallBackSusp);
	WakeDriverThread(mTheadSusp.GetThreadHandle());
}

CUsb2Chip::~CUsb2Chip(void)
{
	// precaution
	DisableInterruptNumber(VEC_USB2_INT);
    SystemSuspendState(0);
    
	MT_CloseEvent(Usb2BeepDownEvent);
	MT_CloseEvent(Usb2EP2RxIrqEvent);
	MT_CloseEvent(Usb2EP4TxIrqEvent);
	MT_CloseEvent(Usb2EP0setupIrqEvent);
	MT_CloseEvent(SuspResEvent);

	disconnect_USB();
}

MatrixError_t CUsb2Chip::Close(void)
{
	DisableInterruptNumber(VEC_USB2_INT);

	MatrixError_t RetVal = ERR_PASS;
	return RetVal;
}

#if defined(VUQUEST3310) || defined (HI2D_T)
// add for USB ESD issue
static int start_susp_irq = 0;
static int end_imager_time = 0;
static int resume_irq_count = 0;
static int suspend_timeout = 0;
/*-----------------------------------------------------------------------------
 * @Name:
 *   reset_res_count_and_timout
 *
 * @Description:
 *    Reset the resume irq count and suspend timeout count.
 *
 * @Paramters:
 *
 *
 * @Return values:
 *
 *-----------------------------------------------------------------------------
 */
void reset_res_count_and_timout()
{
	resume_irq_count = 0;
	suspend_timeout = 0;
}
/*-----------------------------------------------------------------------------
 * @Name:
 *   is_ignore_suspend
 *
 * @Description:
 *   Whether need to ignore the current suspend processing
 *
 * @Paramters:
 *
 *
 * @Return values: ture, false
 *
 *-----------------------------------------------------------------------------
 */
bool is_ignore_suspend()
{
	return ((resume_irq_count > 0)&& suspend_timeout);
}
#endif	

/*-----------------------------------------------------------------------------
 * @Name:
 *   Usb_Exception
 *
 * @Description:
 *    Interrupt service handler function(ISR) for MX25USB2
 *
 * @Paramters:
 *
 *
 * @Return values:
 *
 *-----------------------------------------------------------------------------
 */
threadhandle_t CUsb2Chip::Usb_Exception(void)
{
	UINT irq_src;
	UINT reg_val;

    //NOTE: This interrupt runs before the system has been restored after a
    //suspend.  This function restores the clocks to the USB module so
    //the registers can be accessed.  The system will crash if the clocks
    //are not enabled before then.
    if(!(READ_FIELD(CRM_CGR0,CRM_CGCR0_HCLK_AHB_USBOTG_LSH,CRM_CGCR0_HCLK_AHB_USBOTG_WID)))
    {
    	usbclk_enable();
    }

    irq_src = UOG_USBSTS & UOG_USBINTR;
	mSuspended=false;
#ifndef USING_DEBUGGER
	if (irq_src == USB_STS_RESUME)
	{
		#if defined(VUQUEST3310) || defined (HI2D_T)
		resume_irq_count++;
		#endif
		MX25USB2_resume();
	}
#endif
	//Clear notification bits
	reg_val = UOG_USBSTS;
	reg_val &= irq_src;
	UOG_USBSTS = reg_val;

	// USB Interrupt
	if (irq_src & USB_STS_INT)
	{
		// Setup packet
		reg_val = UOG_ENDPTSETUPSTAT;
		if (reg_val & EP_SETUP_STATUS_EP0)
		{
			MX25USB2_EP0_Rx();
			EP0_setup();
		}
		reg_val = UOG_ENDPTCOMPLETE;
		if (reg_val)
		{
			UOG_ENDPTCOMPLETE = reg_val;
			// endpoint 7 for rx
			if(reg_val & 0x80)
			{
				EPRxHandleReceivedData(7);  //endpoint 7
			}
			// endpoint 5 for rx
			if(reg_val & 0x20)
			{
				ReceiveKbdData(5);  //endpoint 5
			}
			// OUT endpoint 3 for rx ReM
			if ( reg_val & (1 << CUsbRem::MyOutEndpoint) )
			{
				CUsbRem rem;  // there's no stack space used by this object.
				UINT NumRead = MX25USB2_ReadEndpoint_Data(CUsbRem::MyOutEndpoint, sizeof RemOutEndpoint, false, RemOutEndpoint);
				rem.HandleRxData((const unsigned char *)RemOutEndpoint, NumRead, m_pRemRxFifo);
			}
			// endpoint 2 for tx
			if(reg_val & 0x40000)
			{
			}
			// IN endpoint 6 for tx (ReM)
			if ( reg_val & (1 << (16+CUsbRem::MyInEndpoint)) )
			{
			}
			// endpoint 4 for tx
			if(reg_val & 0x100000)
			{
			}
			// endpoint 0 for tx
			if(reg_val & 0x10000)
			{
				// Received an interrupt on complete sending a packet over EP0
				// Address cannot be set until the handshake phase is complete, we cannot set our
				// address before receiving the final ACK or we'll miss it and not always become addressed
				if(m_BusState == USB_ADDR_ASSIGNED)	
				{
					UOG_PERIODICLISTBASE = m_UsbAddress << 25;	// set address
					m_BusState = USB_ADDR_SET;
				}
			}
			// endpoint 0 for rx
			if(reg_val & 0x01)
			{
				if(bEPPflags.bits.control_state == USB_RECEIVE)
				{
					MX25USB2_qu2ep_zlp(1);
					EP0_rxdone();
				}
			}
			// need to send zero length packet over endpoint 0 for handshake
			if(DataStageEn)
			{
				DataStageEn = false;
				MX25USB2_qu2ep_zlp(0);
			}
		}
	}
	// Port Change
	if (irq_src & USB_STS_PORT_CHANGE)
	{
		PortChange_isr();
	}
	// Reset Received
	if (irq_src & USB_STS_RESET)
	{
		MX25USB2_busreset();
	}
	// Sleep Enable (Suspend)
#ifndef USING_DEBUGGER
	if (irq_src & USB_STS_SUSPEND)
	{
		MX25USB2_suspend();
		mSuspended=true;
		#if defined(VUQUEST3310) || defined (HI2D_T)
		start_susp_irq=current_time();
		#endif
		MT_SetEvent(SuspResEvent);
	}
#endif
	if (irq_src & USB_STS_ERR)	// bit 0x02 - USB Error Interrupt
	{
		// A dTD had an error reading - read endpoint data should check status
	}
	if (irq_src & USB_STS_SYS_ERR)	// bit 0x10 - Unrecoverable System Error
	{
		// Per IMX25RM.pdf 47.5.7.6.3:
		// Unrecoverable error. Immediate Reset of core; free transfers buffers in
		// progress and restart the DCD.
		USB2_TRACE("Err IRQ\r\n");
		Reconnect();
		mSuspended=false;
	}

	return 0;
}

/*-----------------------------------------------------------------------------
 * @Name:
 *   //Reconnect
 *
 * @Description:
 *    Reconnect usb handler
 *
 * @Paramters:
 *   None.
 *
 * @Return values:
 *
 *-----------------------------------------------------------------------------
 */
void CUsb2Chip::Reconnect(void)
{
	disconnect_USB();
        // Need to delay
	CTimeout wait(500);
	while (!wait.HasExpired());

	connect_USBHw();
}

/*-----------------------------------------------------------------------------
 * @Name:
 *   connect_USBHw
 *
 * @Description:
 *    Connect usb handler
 *
 * @Paramters:
 *   None.
 *
 * @Return values:
 *
 *-----------------------------------------------------------------------------
 */
void CUsb2Chip::connect_USBHw(void)
{
	disconnect_USB();
       	// Need to delay when switch between usbspd0 and usbspd1
	CTimeout wait(500000);
	while (!wait.HasExpired());

	DISABLE_INTERRUPTS();
	Init_MX25USB2();
	RESTORE_INTERRUPTS();
}

/*-----------------------------------------------------------------------------
 * @Name:
 *   connect_USB
 *
 * @Description:
 *    Connect usb handler
 *
 * @Paramters:
 *   None.
 *
 * @Return values:
 *
 *-----------------------------------------------------------------------------
 */
void CUsb2Chip::connect_USB(void)
{
	if(m_SpdHighEnabled != m_SpdHighEnabledS)
		connect_USBHw();
}

/*-----------------------------------------------------------------------------
 * @Name:
 *   disconnect_USB
 *
 * @Description:
 *    Disconnect usb handler
 *
 * @Paramters:
 *   None.
 *
 * @Return values:
 *
 *-----------------------------------------------------------------------------
 */
void CUsb2Chip::disconnect_USB(void)
{
	DISABLE_INTERRUPTS();
	m_BusState =USB_UNPLUGGED;
	m_ConTimes = 0;

	// Write 1s to the Flush register
	// Erratta BID2053 about flush issue
	MX25USB2_FlushAllBuffers(false);

	if(usbd_qh_basemem != NULL)
	{
		FREE((void *)usbd_qh_basemem);
		usbd_qh_basemem = NULL;
	}

	MX25USB2_stop();

	bEPPflags.bits.usb_mode = FULL_SPEED;
	RESTORE_INTERRUPTS();
}

void CUsb2Chip::Susp_Resume_Thread(void)
{
	while(true)
	{
		if (MT_WaitEvent(SuspResEvent,INFINITE))
		{
			MT_ResetEvent(SuspResEvent);
			while( m_BusState < USB_DEFAULT || 		// Wait until the hardware is initialized
			  	(!IsBeepQueEmpty() && m_BusState < USB_CONFIGURED) )	// Or we're changing interfaces, so allow the beeps
			{
				MT_Delay(1);
			}	
			if (mSuspended==true)
			{	
				SystemSuspendState(1);
				Prepare_System_for_Suspend();
				if(InputPowerStatus() == CABLE_POWERED) //Corded and HEP always returns Cable Powered
				{
					#if defined(VUQUEST3310) || defined (HI2D_T)
					end_imager_time=current_time();
					if((end_imager_time - start_susp_irq) > 10000)//shut down imager is more than 10ms
					{
						suspend_timeout = 1;
					}
					#endif
					EnterStopMode();
					mSuspended = false;
				}
				else
				{
					while(mSuspended==true)
					{
						MT_Delay(1);  //Allow the rest of the system to contine to run. 	
						if (mSuspended && IsHwTriggerPulled() && (bEPPflags.bits.remote_wakeup == 1)) // unless remote wakeup is enabled and the trigger is pressed
						{
							mSuspended = false;
							break;
						}
					}
				}
				SystemSuspendState(0);
				MX25USB2_resume();			
			}
		}
	}
}

//iMX25 USB OTG
/*-----------------------------------------------------------------------------
 * @Name:
 *   MX25USB2_busreset
 *
 * @Description:
 *    MX25USB2 bus reset handler
 *
 * @Paramters:
 *   None.
 *
 * @Return values:
 *
 *-----------------------------------------------------------------------------
 */
void CUsb2Chip::MX25USB2_busreset(void)
{
	UINT reg_val;

	bEPPflags.bits.remote_wakeup = 0;
	m_BusState= USB_DEFAULT;
	mSuspended=false;

	// Clear the device address
	reg_val = UOG_PERIODICLISTBASE;
	reg_val &= ~USB_DEVICE_ADDRESS_MASK;
	UOG_PERIODICLISTBASE = reg_val;

	// Clear all the setup token semaphores
	reg_val = UOG_ENDPTSETUPSTAT;
	UOG_ENDPTSETUPSTAT = reg_val;

	// Clear all the endpoint complete status bits
	reg_val = UOG_ENDPTCOMPLETE;
	UOG_ENDPTCOMPLETE = reg_val;

	// Wait until all endptprime bits cleared
	while (UOG_ENDPTPRIME) {
//		RESET_WATCHDOG_TIMER();
		USB2_TRACE("reset_isr bus reset loop\r\n");
	}

	MX25USB2_FlushAllBuffers(true);

	m_ConTimes = 0;

	if (UOG_PORTSC1 & PORTSCX_PORT_RESET)
	{
		// Reset all the queues, include XD, dTD, EP queue head and TR Queue
	}
	else
	{
		Reconnect();
	}
}

// It is necessary to flush or de-prime endpoints on a USB device reset
// This function follows the instructions from the IMX25RM 46.5.7.5.5
// The bool argument to this function will setup buffers per errata BID2053
// This should be done during setup, not needed during a disconnect
void CUsb2Chip::MX25USB2_FlushAllBuffers(bool EndpointSetupFix)
{
	//DEBUG_puts("USB Flush buffers\r\n");
	bool done = false;
	do
	{
		// Write 1s to the Flush register
		UOG_ENDPTFLUSH = 0xFFFFFFFF;
		// Wait until all bits in the flush register are zero
		while( UOG_ENDPTFLUSH );

		// Make sure ENDPT setup status register for all endpoints is zero,
		// else start over
		if( !UOG_ENDPTSETUPSTAT )
			done = true;
	} while( !done );

	// Erratta BID2053 about flush issue:
	// Endpoints are in pairs odd endpoints receive, even endpoints send
	// If you want to be able to send short packets you need prime the
	// complimentary receive to receive 2 bytes.
	// Added by JimR:  This looks like the BID2108 workaround (see Step 2 in "USB BID2108 1 byte CRC error/2 bytes long packet error issue").
	// ReM IN endpoint will always send max packet size, so we don't need to worry about this issue.
	if( EndpointSetupFix )
	{
		MX25USB2_WriteEPs2bytes(5);
		MX25USB2_WriteEPs2bytes(1);
		MX25USB2_WriteEPs2bytes(3);
	}
}

void CUsb2Chip::MX25USB2_EP0_Rx(void)
{
	UINT reg_val;
	volatile EP_QUEUE_HEAD *p_QH;
	UCHAR req[8];

	memset(req, 0, 8);

	p_QH = &usbd_qh_base->qh[2 * 0 + EP_DIR_OUT];

	// Clear bit in ENDPTSETUPSTAT
	UOG_ENDPTSETUPSTAT |= 1;

	// while a hazard exists when setup package arrives
	while(1)
	{
		// Set Setup
		reg_val = USB_CMD_SUTW;
		UOG_USBCMD |= reg_val;

		// Copy the setup packet to local buffer
		memcpy(req, (UCHAR *) p_QH->setup_buffer, 8);

		if(UOG_USBCMD & USB_CMD_SUTW)
			break;
	}
	// Clear Setup
	reg_val = UOG_USBCMD;
	reg_val &= ~USB_CMD_SUTW;
	UOG_USBCMD = reg_val;

	// big/little endian
	ControlData.DeviceRequest.bmRequestType = req[0];
	ControlData.DeviceRequest.bRequest = req[1];
	ControlData.DeviceRequest.wValue = req[3] * 256 + req[2];
	ControlData.DeviceRequest.wIndex = req[5] * 256 + req[4];
	ControlData.DeviceRequest.wLength = req[7] * 256 + req[6];
}

void CUsb2Chip::MX25USB2_filldtd(UCHAR EpNum, UCHAR Epdtd, UINT len, UCHAR dir, volatile const UCHAR *pBuf)
{
	UINT tmp_val;
	UINT temp;
	UINT dtd_count = 0, offset = 0;
	unsigned char *buf = (unsigned char *)pBuf;

	// Any data buffers being sent to this function should be using the non-cached address
	// we need to set these pointers to the physical (cached) address before sending the
	// address to the peripheral.
	if( buf > NON_CACHED_MEMORY_BASE )
	{
		buf -= ( NON_CACHED_MEMORY_BASE - CACHED_MEMORY_BASE );
	}
	else	// if we receive a cached/virtual address flush the cache
	{
		MMU_CleanAndInvalidate();
	}

	while((len - offset) > 0)
	{
		// Init all of buffer page pointers
		tmp_val =  (UINT)buf + offset;

		usbd_qh_base->dtd_nxt[Epdtd-2][dtd_count].buff_ptr0 = tmp_val;
		usbd_qh_base->dtd_nxt[Epdtd-2][dtd_count].buff_ptr1 = tmp_val + 0x1000;
		usbd_qh_base->dtd_nxt[Epdtd-2][dtd_count].buff_ptr2 = tmp_val + 0x2000;
		usbd_qh_base->dtd_nxt[Epdtd-2][dtd_count].buff_ptr3 = tmp_val + 0x3000;
		usbd_qh_base->dtd_nxt[Epdtd-2][dtd_count].buff_ptr4 = tmp_val + 0x4000;

		if(dtd_count >= 1)
			usbd_qh_base->dtd_nxt[Epdtd-2][dtd_count-1].next_td_ptr = (UINT)&usbd_qh_base->dtd_nxt[Epdtd-2][dtd_count];

		if((len - offset) > EP_MAX_LENGTH_TRANSFER)
		{
			// Fill in the transfer size; set interrupt on every dtd;
			// set active bit
			tmp_val = ((EP_MAX_LENGTH_TRANSFER << DTD_LENGTH_BIT_POS) | DTD_IOC | DTD_STATUS_ACTIVE);
			offset += EP_MAX_LENGTH_TRANSFER;
		}
		else
		{
			// Fill in the transfer size; set interrupt on every dtd;
			// set active bit
			tmp_val = (((len - offset) << DTD_LENGTH_BIT_POS) | DTD_IOC | DTD_STATUS_ACTIVE);
			usbd_qh_base->dtd_nxt[Epdtd-2][dtd_count].next_td_ptr = DTD_NEXT_TERMINATE;
			offset += (len - offset);
		}

		usbd_qh_base->dtd_nxt[Epdtd-2][dtd_count].size_ioc_sts = (UINT)tmp_val;

		// Clear reserved field
		tmp_val = (UINT)usbd_qh_base->dtd_nxt[Epdtd-2][dtd_count].size_ioc_sts;
		tmp_val &= ~DTD_RESERVED_FIELDS;
		usbd_qh_base->dtd_nxt[Epdtd-2][dtd_count].size_ioc_sts = (UINT)tmp_val;

		dtd_count += 1;
	}  //while end

	usbd_qh_base->qh[Epdtd].curr_dtd_ptr = 0;

	// Write dQH next pointer and terminate bit to 0
	temp = (UINT)&usbd_qh_base->dtd_nxt[Epdtd-2][0] & EP_QUEUE_HEAD_NEXT_POINTER_MASK;
	usbd_qh_base->qh[Epdtd].next_dtd_ptr = temp;

	// Clear active and halt bit
	temp = ~(EP_QUEUE_HEAD_STATUS_ACTIVE | EP_QUEUE_HEAD_STATUS_HALT);
	usbd_qh_base->qh[Epdtd].size_ioc_int_sts &= temp;

	// Prime endpoint by writing 1 to ENDPTPRIME
	temp = dir ? (1 << (EpNum + 16)) : (1 << EpNum);

	UOG_ENDPTPRIME |= temp;

	while(UOG_ENDPTPRIME)
	{
	}
}

void CUsb2Chip::MX25USB2_filldtd_ctrl(UCHAR Epdtd, UINT len, UCHAR dir, volatile const UCHAR *pBuf)
{
	UINT length;
	UINT tmp_val;
	UINT temp;
	unsigned char *buf = (unsigned char *)pBuf;

	if(len > EP_MAX_LENGTH_TRANSFER)
		length = EP_MAX_LENGTH_TRANSFER;
	else
		length = len;

	// Any data buffers being sent to this function should be using the non-cached address
	// we need to set these pointers to the physical (cached) address before sending the
	// address to the peripheral.
	if( buf > NON_CACHED_MEMORY_BASE )
	{
		buf -= ( NON_CACHED_MEMORY_BASE - CACHED_MEMORY_BASE );
	}
	else
	{
		MMU_CleanAndInvalidate();
	}

	// Fill in the transfer size; set interrupt on every dtd;
	// set active bit
	tmp_val = ((length << DTD_LENGTH_BIT_POS) | DTD_IOC | DTD_STATUS_ACTIVE);

	usbd_qh_base->dtd[Epdtd].size_ioc_sts = (UINT)tmp_val;

	// Clear reserved field
	tmp_val = (UINT)usbd_qh_base->dtd[Epdtd].size_ioc_sts;
	tmp_val &= ~DTD_RESERVED_FIELDS;
	usbd_qh_base->dtd[Epdtd].size_ioc_sts = (UINT)tmp_val;

	// Init all of buffer page pointers
	usbd_qh_base->dtd[Epdtd].buff_ptr0 = (UINT)buf;
	usbd_qh_base->dtd[Epdtd].buff_ptr1 = (UINT)buf + 0x1000;
	usbd_qh_base->dtd[Epdtd].buff_ptr2 = (UINT)buf + 0x2000;
	usbd_qh_base->dtd[Epdtd].buff_ptr3 = (UINT)buf + 0x3000;
	usbd_qh_base->dtd[Epdtd].buff_ptr4 = (UINT)buf + 0x4000;

	usbd_qh_base->dtd[Epdtd].next_td_ptr = DTD_NEXT_TERMINATE;

	usbd_qh_base->qh[Epdtd].curr_dtd_ptr = 0;

	// Write dQH next pointer and terminate bit to 0
	temp = (UINT)&usbd_qh_base->dtd[Epdtd] & EP_QUEUE_HEAD_NEXT_POINTER_MASK;
	usbd_qh_base->qh[Epdtd].next_dtd_ptr = temp;

	// Clear active and halt bit
	temp = ~(EP_QUEUE_HEAD_STATUS_ACTIVE | EP_QUEUE_HEAD_STATUS_HALT);
	usbd_qh_base->qh[Epdtd].size_ioc_int_sts &= temp;

	// Prime endpoint by writing 1 to ENDPTPRIME
	temp = dir ? (1 << 16) : (1 << 0);

	UOG_ENDPTPRIME |= temp;

	while( UOG_ENDPTPRIME)
	{
	}
}

// Tx zero-length package request to control endpoint
void CUsb2Chip::MX25USB2_qu2ep_zlp(UCHAR bIN)
{
	MX25USB2_qu2ep_ctrl(bIN, 0, bIN, dataToEP);
}


// Tx request to endpoint
void CUsb2Chip::MX25USB2_qu2ep(UCHAR EpNum, UCHAR Epdtd, UINT len, UCHAR dir, volatile const UCHAR *buf)
{
	DISABLE_INTERRUPTS();

	// Fill the endpoint dtds
	MX25USB2_filldtd(EpNum, Epdtd, len, dir, buf);

	RESTORE_INTERRUPTS();
}

void CUsb2Chip::MX25USB2_qu2ep_ctrl(UCHAR Epdtd, UINT len, UCHAR dir, volatile const UCHAR *buf)
{
	DISABLE_INTERRUPTS();

	// Fill the endpoint dtds
	MX25USB2_filldtd_ctrl(Epdtd, len, dir, buf);

	RESTORE_INTERRUPTS();
}

void CUsb2Chip::PortChange_isr(void)
{
	UINT speed;

	// Bus resetting is finished
	if (!(UOG_PORTSC1 & PORTSCX_PORT_RESET))
	{
		// Get the speed
		speed = (UOG_PORTSC1 & PORTSCX_PORT_SPEED_MASK);
		switch (speed)
		{
		case PORTSCX_PORT_SPEED_HIGH:
			bEPPflags.bits.usb_mode = HIGH_SPEED;
			break;
		case PORTSCX_PORT_SPEED_FULL:
			bEPPflags.bits.usb_mode = FULL_SPEED;
			break;
		case PORTSCX_PORT_SPEED_LOW:
			bEPPflags.bits.usb_mode = LOW_SPEED;
			break;
		default:
			break;
		}
		MX25USB2_eps_setup(false);
	}
}

void CUsb2Chip::usbclk_enable(void)
{
	UINT reg_val;

	reg_val = CLK_CCTL;
	reg_val |= 0x3 << 16;  //60MHZ(UPLL=240MHZ:pd=2,mfd=1,mfi=10,mfn=0))
	CLK_CCTL = reg_val;

	reg_val = CLK_CGCR0;
	reg_val |= 1 << 28;   //hclk_MX25USB2
	CLK_CGCR0 = reg_val;
}

void CUsb2Chip::MX25USB2_setxcvr(void)
{
	UINT reg_val;

	reg_val = UOG_PORTSC1;
	reg_val &= ~PORTSC_PTS_MASK;	//set UTMI xcvr
	reg_val |= PORTSC_PTS_UTMI;
	UOG_PORTSC1 = reg_val;

	//set UTMI PHY
	reg_val = USB_PHY_CTRL_FUNC;
	//enable the USB UTMI PHY,the clk is within the spec,device side
	reg_val |= UTMI_USBEN ;
	// Set EVDO at suggestion of Freescale to fix NG2D-1134
	reg_val |= UTMI_EVDO; 
	reg_val &= ~(UTMI_HOSTPORT|UTMI_LSFE|UTMI_VCONTROLLOADM);
	USB_PHY_CTRL_FUNC = reg_val;
}

/*-----------------------------------------------------------------------------
 * @Name:
 *   Init_MX25USB2Otg
 *
 * @Description:
 *    Confige GPIOs for initialize MX25USB2's ports
 *
 * @Paramters:
 *
 *
 * @Return values:
 *
 *-----------------------------------------------------------------------------
 */
void CUsb2Chip::Init_MX25USB2Otg(void)
{
	UINT reg_val;

//	usbclk_enable();

	reg_val = USB_CTRL;
	reg_val |= UCTRL_OWIE | UCTRL_OPM;
	USB_CTRL = reg_val;

	MX25USB2_setxcvr();

}


/*-----------------------------------------------------------------------------
 * @Name:
 *   Rst_MX25USB2
 *
 * @Description:
 *    Set GPIOs to reset MX25USB2
 *
 * @Paramters:
 *
 *
 * @Return values:
 *
 *-----------------------------------------------------------------------------
 */
void CUsb2Chip::Rst_MX25USB2(void)
{

}

void CUsb2Chip::MX25USB2_dQHsetup(void)
{
	// initialized QHs, take care the 2K align
	usbd_qh_basemem = (USBD_QH_T *)MALLOC(sizeof(USBD_QH_T) + 2 * 1024);

	if (usbd_qh_basemem == NULL)
	{
		USB2_TRACE("usbd_qh_basemem malloc fail\r\n");
		return ;
	}

	// Use (((x)+(a)-1)&~((a)-1)) to align
	usbd_qh_base = (USBD_QH_T *)((((UINT)usbd_qh_basemem)+(2*1024)-1)&~((2*1024)-1));
	// Ensure that qh_base points to non-cached memory to properly communicate with the peripheral
	unsigned char * pTempQh = (unsigned char *)usbd_qh_base;
	pTempQh += ( NON_CACHED_MEMORY_BASE - CACHED_MEMORY_BASE );
	usbd_qh_base = (USBD_QH_T *)pTempQh;

	memset((void *)usbd_qh_base, 0, sizeof(USBD_QH_T));
}

void CUsb2Chip::MX25USB2_stop(void)
{
	UINT reg_val;

	// disable all INTR
	UOG_USBINTR = 0;

	// set controller to Stop
	reg_val = UOG_USBCMD;
	reg_val &= ~USB_CMD_RUN_STOP;
	UOG_USBCMD = reg_val;
}

void CUsb2Chip::MX25USB2_Setup(void)
{
	UINT reg_val = 0;
	UINT timeout;

	reg_val = UOG_USBCMD;
	reg_val &= ~USB_CMD_RUN_STOP;
	UOG_USBCMD = reg_val;

	reg_val = UOG_USBCMD;
	reg_val |= USB_CMD_CTRL_RESET;
	UOG_USBCMD = reg_val;

	/* Wait for reset to complete */
	timeout = 10000000;
	while ((UOG_USBCMD & USB_CMD_CTRL_RESET) && --timeout) {
//		RESET_WATCHDOG_TIMER();
		continue;
	}
	if (timeout == 0) {
		USB2_TRACE("MX25USB2_Setup:timeout\r\n");
		return ;
	}

	//Set the controller as device mode
	reg_val = UOG_USBMODE;
	reg_val |= USB_MODE_CTRL_MODE_DEVICE;
	//Disable Setup Lockout
	reg_val |= USB_MODE_SETUP_LOCK_OFF;
	//Enable Stream Disable (SDIS) mode (disable streaming)
	reg_val |= USB_MODE_STREAM_DISABLE;
	UOG_USBMODE = reg_val;

	//Clear the setup status
	UOG_USBSTS = 0;

	UOG_ASYNCLISTADDR = (UINT)usbd_qh_base;

	reg_val = UOG_PORTSC1;
	reg_val &= ~PORTSCX_PHY_TYPE_SEL;
	reg_val |= PORTSC_PTS_UTMI;

#ifdef DEBUG_FORCE_FS
	reg_val |= 0x1000000;
#endif
	UOG_PORTSC1 = reg_val;

	// Set the controller as device mode
	reg_val = UOG_USBMODE;
	reg_val |= USB_MODE_CTRL_MODE_DEVICE;
	UOG_USBMODE = reg_val;

	//add set_vbus_power
}

/*-----------------------------------------------------------------------------
 * @Name:
 *   MX25USB2_ep_qh_setup
 *
 * @Description:
 *    set the Endpoint Capabilites field of QH
 *
 * @Paramters:
 *   zlt: Zero Length Termination Select (0=enabled, 1=disabled)
 *   mult: Mult field
 *
 * @Return values:
 *
 *-----------------------------------------------------------------------------
 */
void CUsb2Chip::MX25USB2_ep_qh_setup(UCHAR ep_num, UCHAR dir, UCHAR ep_type, UINT max_pkt_len, UINT zlt, UCHAR mult)
{
	volatile EP_QUEUE_HEAD *p_QH = NULL;
	UINT i, tmp = 0;

	i = 2 * ep_num + dir;

	if(i > 15)
		return ;

	p_QH = &usbd_qh_base->qh[i];

	switch (ep_type) {
	case USB_ENDPOINT_XFER_CONTROL:
		//Interrupt On Setup (IOS). for control ep, Zlt=1
		tmp = (max_pkt_len << EP_QUEUE_HEAD_MAX_PKT_LEN_POS) | EP_QUEUE_HEAD_IOS | (1<<29);
		//Set ioc xxx-complete after receive setup
		p_QH->next_dtd_ptr = DTD_NEXT_TERMINATE;
		p_QH->size_ioc_int_sts = ~(EP_QUEUE_HEAD_STATUS_ACTIVE | EP_QUEUE_HEAD_STATUS_HALT) | (1<<15);
		break;
	case USB_ENDPOINT_XFER_BULK:
	case USB_ENDPOINT_XFER_INT:
		tmp = max_pkt_len << EP_QUEUE_HEAD_MAX_PKT_LEN_POS;
		// For ZLT 0 is enabled, 1 is disabled
		if (!zlt)
		{
			tmp |= EP_QUEUE_HEAD_ZLT_SEL;
		}
		break;
	default:
		return;
	}
	p_QH->max_pkt_length = tmp;
}

void CUsb2Chip::MX25USB2_dr_ep_setup(UCHAR ep_num, UCHAR dir, UCHAR ep_type)
{
	UINT tmp_epctrl = 0;

	switch (ep_num) {
	case 0:
		tmp_epctrl = UOG_ENDPTCTRL0;
		break;
	case 1:
		tmp_epctrl = UOG_ENDPTCTRL1;
		break;
	case 2:
		tmp_epctrl = UOG_ENDPTCTRL2;
		break;
	case 3:
		tmp_epctrl = UOG_ENDPTCTRL3;
		break;
	case 4:
		tmp_epctrl = UOG_ENDPTCTRL4;
		break;
	case 5:
		tmp_epctrl = UOG_ENDPTCTRL5;
		break;
	case 6:
		tmp_epctrl = UOG_ENDPTCTRL6;
		break;
	case 7:
		tmp_epctrl = UOG_ENDPTCTRL7;
		break;
	default:
		return ;
	}
	if (dir) {
		if (ep_num)
		{
			tmp_epctrl |= EPCTRL_TX_DATA_TOGGLE_RST;
		}
		tmp_epctrl |= EPCTRL_TX_ENABLE;
		tmp_epctrl |= ((UINT)(ep_type) << EPCTRL_TX_EP_TYPE_SHIFT);
	} else {
		if (ep_num)
		{
			tmp_epctrl |= EPCTRL_RX_DATA_TOGGLE_RST;
		}
		tmp_epctrl |= EPCTRL_RX_ENABLE;
		tmp_epctrl |= ((UINT)(ep_type) << EPCTRL_RX_EP_TYPE_SHIFT);
	}

	switch (ep_num) {
	case 0:
		UOG_ENDPTCTRL0 = tmp_epctrl;
		break;
	case 1:
		UOG_ENDPTCTRL1 = tmp_epctrl;
		break;
	case 2:
		UOG_ENDPTCTRL2 = tmp_epctrl;
		break;
	case 3:
		UOG_ENDPTCTRL3 = tmp_epctrl;
		break;
	case 4:
		UOG_ENDPTCTRL4 = tmp_epctrl;
		break;
	case 5:
		UOG_ENDPTCTRL5 = tmp_epctrl;
		break;
	case 6:
		UOG_ENDPTCTRL6 = tmp_epctrl;
		break;
	case 7:
		UOG_ENDPTCTRL7 = tmp_epctrl;
		break;
	default:
		return ;
	}

	// wait for the write reg to finish
	
	for(int i=0; i<10; i++); ///TODO:  This is hardly robust.  In fact, the compiler's optimizer probably takes it out.
}


void CUsb2Chip::MX25USB2_ep0_setup(void)
{
	MX25USB2_ep_qh_setup(0, USB_RECV, USB_ENDPOINT_XFER_CONTROL, USB_MAX_CTRL_PAYLOAD, 0, 0);
	MX25USB2_ep_qh_setup(0, USB_SEND, USB_ENDPOINT_XFER_CONTROL, USB_MAX_CTRL_PAYLOAD, 0, 0);
	MX25USB2_dr_ep_setup(0, USB_RECV, USB_ENDPOINT_XFER_CONTROL);
	MX25USB2_dr_ep_setup(0, USB_SEND, USB_ENDPOINT_XFER_CONTROL);
}


// In the keyboard code, it looks odd to see OUT endpoint 5 getting set to an IN when bool First param is true.  I think we get away with this because there will
// always be a bus reset when we boot, and that handler calls this method with the First param set to false which then sets it up correctlly.
//
// HID POS, Keyboard and Keyboard HID POS Helper never sends 1 or 2 byte packets, so I doubt we need BD2108 workaround for them, but it can't hurt.  I don't know about Sure POS.
void CUsb2Chip::MX25USB2_eps_setup(bool First)
{
	// BID2108 issue
	if(First)
		MX25USB2_ep_qh_setup(1, USB_SEND, USB_ENDPOINT_XFER_INT, 64, 0, 0);

	switch(termid_eps)
	{
	case 130:	// USB COM
		//endpint 4
		//Set it to interrupt endpoint-Int IN
		MX25USB2_ep_qh_setup(4, USB_SEND, USB_ENDPOINT_XFER_INT, 64, 1, 0);
		MX25USB2_dr_ep_setup(4, USB_SEND, USB_ENDPOINT_XFER_INT);
		// BID2108 issue
		if(First)
			MX25USB2_ep_qh_setup(5, USB_SEND, USB_ENDPOINT_XFER_INT, 64, 0, 0);

		if(bEPPflags.bits.usb_mode == HIGH_SPEED)
		{
			//endpint 7
			//Set it to bulk endpoint-Bulk OUT
			MX25USB2_ep_qh_setup(7, USB_RECV, USB_ENDPOINT_XFER_BULK, 512, 0, 0);
			MX25USB2_dr_ep_setup(7, USB_RECV, USB_ENDPOINT_XFER_BULK);

			//endpint 2
			//Set it to bulk endpoint-Bulk IN
			MX25USB2_ep_qh_setup(2, USB_SEND, USB_ENDPOINT_XFER_BULK, 512, 1, 0);
			MX25USB2_dr_ep_setup(2, USB_SEND, USB_ENDPOINT_XFER_BULK);
		}
		else
		{
			//endpint 7
			//Set it to bulk endpoint-Bulk OUT
			MX25USB2_ep_qh_setup(7, USB_RECV, USB_ENDPOINT_XFER_BULK, 64, 0, 0);
			MX25USB2_dr_ep_setup(7, USB_RECV, USB_ENDPOINT_XFER_BULK);

			//endpint 2
			//Set it to bulk endpoint-Bulk IN
			MX25USB2_ep_qh_setup(2, USB_SEND, USB_ENDPOINT_XFER_BULK, 64, 1, 0);
			MX25USB2_dr_ep_setup(2, USB_SEND, USB_ENDPOINT_XFER_BULK);
		}
		// BID2108 issue
		if(First)
			MX25USB2_ep_qh_setup(3, USB_SEND, USB_ENDPOINT_XFER_INT, 64, 0, 0);
		break;
	case 131:	// USB HID POS
		//endpint 2
		//Set it to interrupt endpoint-Int IN
		MX25USB2_ep_qh_setup(2, USB_SEND, USB_ENDPOINT_XFER_INT, 64, 0, 0);
		MX25USB2_dr_ep_setup(2, USB_SEND, USB_ENDPOINT_XFER_INT);
		// BID2108 issue
		if(First)
			MX25USB2_ep_qh_setup(3, USB_SEND, USB_ENDPOINT_XFER_INT, 64, 0, 0);

		//endpint 7
		//Set it to interrupt endpoint-int OUT
		MX25USB2_ep_qh_setup(7, USB_RECV, USB_ENDPOINT_XFER_INT, 64, 0, 0);
		MX25USB2_dr_ep_setup(7, USB_RECV, USB_ENDPOINT_XFER_INT);

		break;
	case 128:	// USB SurePOS Hand Held
	case 129:	// USB SurePOS Table Top
		//endpint 4
		//Set it to interrupt endpoint-Int IN
		MX25USB2_ep_qh_setup(4, USB_SEND, USB_ENDPOINT_XFER_INT, 64, 0, 0);
		MX25USB2_dr_ep_setup(4, USB_SEND, USB_ENDPOINT_XFER_INT);
		// BID2108 issue
		if(First)
			MX25USB2_ep_qh_setup(5, USB_SEND, USB_ENDPOINT_XFER_INT, 64, 0, 0);

		//endpint 2
		//Set it to interrupt endpoint-Int IN
		MX25USB2_ep_qh_setup(2, USB_SEND, USB_ENDPOINT_XFER_INT, 64, 0, 0);
		MX25USB2_dr_ep_setup(2, USB_SEND, USB_ENDPOINT_XFER_INT);
		// BID2108 issue
		if(First)
			MX25USB2_ep_qh_setup(3, USB_SEND, USB_ENDPOINT_XFER_INT, 64, 0, 0);

		//endpint 7
		//Set it to interrupt endpoint-int OUT
		MX25USB2_ep_qh_setup(7, USB_RECV, USB_ENDPOINT_XFER_INT, 64, 0, 0);
		MX25USB2_dr_ep_setup(7, USB_RECV, USB_ENDPOINT_XFER_INT);

		break;
	case 124:	// USB Keyboard PC
	case 125:	// USB Keyboard Mac
	case 134:	// USB Keyboard Japanese (PC)
		//endpint 4
		//Set it to interrupt endpoint-Int IN
		MX25USB2_ep_qh_setup(4, USB_SEND, USB_ENDPOINT_XFER_INT, 64, 0, 0);
		MX25USB2_dr_ep_setup(4, USB_SEND, USB_ENDPOINT_XFER_INT);
		// BID2108 issue
		if(First)
			MX25USB2_ep_qh_setup(5, USB_SEND, USB_ENDPOINT_XFER_INT, 64, 0, 0);
		else
		{
			MX25USB2_ep_qh_setup(5, USB_RECV, USB_ENDPOINT_XFER_INT, 64, 0, 0);
			MX25USB2_dr_ep_setup(5, USB_RECV, USB_ENDPOINT_XFER_INT);
		}

		//endpint 2
		//Set it to interrupt endpoint-Int IN
		MX25USB2_ep_qh_setup(2, USB_SEND, USB_ENDPOINT_XFER_INT, 64, 0, 0);
		MX25USB2_dr_ep_setup(2, USB_SEND, USB_ENDPOINT_XFER_INT);
		// BID2108 issue
		if(First)
			MX25USB2_ep_qh_setup(3, USB_SEND, USB_ENDPOINT_XFER_INT, 64, 0, 0);

		//endpint 7
		//Set it to interrupt endpoint-int OUT
		MX25USB2_ep_qh_setup(7, USB_RECV, USB_ENDPOINT_XFER_INT, 64, 0, 0);
		MX25USB2_dr_ep_setup(7, USB_RECV, USB_ENDPOINT_XFER_INT);

		break;
	default:
		break;
	}
	if ( ! First && m_bRemIfaceEnabled )
	{
		MX25USB2_ep_qh_setup(CUsbRem::MyOutEndpoint, USB_RECV, USB_ENDPOINT_XFER_INT, MAX_PACKET_SIZE, 0, 0);
		MX25USB2_dr_ep_setup(CUsbRem::MyOutEndpoint, USB_RECV, USB_ENDPOINT_XFER_INT);
		// Note:  we always send max packet size packets, so we don't need to worry about BD2108
		MX25USB2_ep_qh_setup(CUsbRem::MyInEndpoint, USB_SEND, USB_ENDPOINT_XFER_INT, MAX_PACKET_SIZE, 0, 0);
		MX25USB2_dr_ep_setup(CUsbRem::MyInEndpoint, USB_SEND, USB_ENDPOINT_XFER_INT);
		MX25USB2_ReadEndpoint(CUsbRem::MyOutEndpoint, sizeof RemOutEndpoint, false, RemOutEndpoint);  // prime the endpoint for reading
	}
}

void CUsb2Chip::MX25USB2_eps_dis(UCHAR ep_num)
{
	UINT tmp_epctrl = 0;

	switch (ep_num) {
	case 0:
		tmp_epctrl = UOG_ENDPTCTRL0;
		break;
	case 1:
		tmp_epctrl = UOG_ENDPTCTRL1;
		break;
	case 2:
		tmp_epctrl = UOG_ENDPTCTRL2;
		break;
	case 3:
		tmp_epctrl = UOG_ENDPTCTRL3;
		break;
	case 4:
		tmp_epctrl = UOG_ENDPTCTRL4;
		break;
	case 5:
		tmp_epctrl = UOG_ENDPTCTRL5;
		break;
	case 6:
		tmp_epctrl = UOG_ENDPTCTRL6;
		break;
	case 7:
		tmp_epctrl = UOG_ENDPTCTRL7;
		break;
	default:
		return ;
	}

	tmp_epctrl &= ~(1 << 7);   //Rx disable
	tmp_epctrl &= ~(1 << 23);  //Tx disable

	switch (ep_num) {
	case 0:
		UOG_ENDPTCTRL0 = tmp_epctrl;
		break;
	case 1:
		UOG_ENDPTCTRL1 = tmp_epctrl;
		break;
	case 2:
		UOG_ENDPTCTRL2 = tmp_epctrl;
		break;
	case 3:
		UOG_ENDPTCTRL3 = tmp_epctrl;
		break;
	case 4:
		UOG_ENDPTCTRL4 = tmp_epctrl;
		break;
	case 5:
		UOG_ENDPTCTRL5 = tmp_epctrl;
		break;
	case 6:
		UOG_ENDPTCTRL6 = tmp_epctrl;
		break;
	case 7:
		UOG_ENDPTCTRL7 = tmp_epctrl;
		break;
	default:
		return ;
	}

	// wait for the write reg to finish
	for(int i=0; i<10; i++);
}

/*-----------------------------------------------------------------------------
 * @Name:
 *   Init_MX25USB2
 *
 * @Description:
 *
 *
 * @Paramters:
 *   None.
 *
 * @Return values:
 *
 *-----------------------------------------------------------------------------
 */
void CUsb2Chip::Init_MX25USB2(void)
{
	bEPPflags.value = 0;

	bEPPflags.bits.usb_mode = FULL_SPEED;

        // OTG init,interrupt install
	Init_MX25USB2Otg();

	//Initialize the QH structure member
	MX25USB2_dQHsetup();

	bEPPflags.bits.control_state = USB_IDLE;

	MX25USB2_Setup();

	//endpoint setup
	//ep0 setup
	MX25USB2_ep0_setup();
	//other endpoints setup
	MX25USB2_eps_setup(true);

	m_SpdHighEnabledS = m_SpdHighEnabled;
	//Force to Full speed by USBSPD0 menu
	if(!m_SpdHighEnabled)
		UOG_PORTSC1 |= PORTSC_PFSC;

	MX25USB2_run();

	// For BID2108
	MX25USB2_WriteEPs2bytes(5);
	MX25USB2_WriteEPs2bytes(1);
	MX25USB2_WriteEPs2bytes(3);
	m_BusState = USB_DEFAULT;
}
//iMX25 USB OTG init end
void CUsb2Chip::DataStageEnable(void)
{
	DataStageEn = true;
}

void CUsb2Chip::StatusAcknowledge(UCHAR bIN)
{
	MX25USB2_qu2ep_zlp(bIN);
}

/*-----------------------------------------------------------------------------
 * @Name:
 *   MX25USB2_ReadEndpoint
 *
 * This really doesn't read the endpoint.  This primes it for reading.
 *
 *
 * @Paramters:
 *    bEndp  --
 *    len    -- data length for receiving data.
 *
 *
 * @Return values:
 *
 *-----------------------------------------------------------------------------
 */
void CUsb2Chip::MX25USB2_ReadEndpoint(UCHAR bEndp, UINT len, bool ctrl, volatile UCHAR *buf)
{
	UCHAR Epdtd = bEndp * 2;

	if(bEndp == 0)
		MX25USB2_qu2ep_ctrl(Epdtd, len, 0, buf);
	else
		MX25USB2_qu2ep(bEndp, Epdtd, len, 0, buf);
}


UINT CUsb2Chip::MX25USB2_ReadEndpoint_Data(UCHAR bEndp, UINT maxlen, bool ctrl, volatile UCHAR *buf)
{
	UINT len;
	//bool failed = false;
	if(bEndp > 7)
		return 0;

	len = usbd_qh_base->qh[2*bEndp].size_ioc_int_sts >> 16;	// padding size

	if(!ctrl)
		MX25USB2_ReadEndpoint(bEndp, maxlen, false, buf);   // Prepare Rx for next package, max len RXMAXSIZE for code dowload

	//Max 1032 bytes buffer for code dowload
	if(ctrl)
		len = 64 - len;	// return length = maxsize - padding
	else
		len = maxlen - len;

	return len;
}

// For BID2108
void CUsb2Chip::MX25USB2_WriteEPs2bytes(UCHAR EpNum)
{
	UCHAR Epdtd = EpNum * 2 + 1;

	dataToEP[0] = 0x55;
	dataToEP[1] = 0xaa;
	MX25USB2_qu2ep(EpNum, Epdtd, 2, 1, dataToEP);
}

/*-----------------------------------------------------------------------------
 * @Name:
 *   MX25USB2_WriteEndpoint
 *
 * @Description:
 *    Write len length data that in buf into buffer of ISP1582's endpoint
 *
 * @Paramters:
 *    bEndp  -- endpoint num equal to (bEndp<<1)+1.
 *    len    -- data length for transfering.
 *    buf    -- data in the buf is for transfering.
 *
 *
 * @Return values:
 *
 *-----------------------------------------------------------------------------
 */
UINT CUsb2Chip::MX25USB2_WriteEndpoint(UCHAR bEndp, UINT len, const UCHAR * buf)
{
	UINT temp, Offset=0;
	UCHAR Epdtd = bEndp * 2 + 1;

	temp = 1 << (bEndp + 16);

	while(len - Offset != 0)
	{
		UINT size = (len-Offset >= maxTxdata)?maxTxdata:(len-Offset);

  		// Wait until we're able to send and the last prime is finished
		while (!IsConfigured() || (UOG_ENDPTSTAT & temp))
		{
			Timer3_MicroSecondDelay(100);
		}
		MX25USB2_qu2ep(bEndp, Epdtd, size, 1, buf+Offset);
		Offset+=size;
	}

	return len;
}


/*-----------------------------------------------------------------------------
 * @Name:
 *   EpHasData
 *
 * @Description:
 *   Check to see if data in EP fifo
 *
 * @Paramters:
 *    end point number
 *
 * @Return values:
 *   true data in fifo false otherwise
 *-----------------------------------------------------------------------------
 */
bool CUsb2Chip::EpHasData(UCHAR EpNum)
{
	bool ret = false;
	UINT temp, reg_val;

	DISABLE_INTERRUPTS();

	temp = 1 << (EpNum + 16) ;
	reg_val = UOG_ENDPTSTAT;
	reg_val &= temp;

	if(reg_val)
		ret =  true;

	RESTORE_INTERRUPTS();

	return ret;
}

void CUsb2Chip::MX25USB2_SetEndpointStatus(UCHAR bEndp,UCHAR bStalled)
{
	stall_eps(bEndp, bStalled);
}

UCHAR CUsb2Chip::MX25USB2_GetEndpointStatus(UCHAR bEndp)
{
	UCHAR status = 0;
	UINT reg_val;

	// a protocol stall
	if(bEndp == 0)
		reg_val = UOG_ENDPTCTRL0;
	else if(bEndp == 1)
		reg_val = UOG_ENDPTCTRL1;
	else if(bEndp == 2)
		reg_val = UOG_ENDPTCTRL2;
	else if(bEndp == 3)
		reg_val = UOG_ENDPTCTRL3;
	else if(bEndp == 4)
		reg_val = UOG_ENDPTCTRL4;
	else if(bEndp == 5)
		reg_val = UOG_ENDPTCTRL5;
	else if(bEndp == 6)
		reg_val = UOG_ENDPTCTRL6;
	else if(bEndp == 7)
		reg_val = UOG_ENDPTCTRL7;
	else
		return status;

	status = (reg_val & EPCTRL_RX_EP_STALL) || (reg_val & EPCTRL_TX_EP_STALL);

	return status;
}

inline void CUsb2Chip::EP0_setup(void)
{
	ControlData.wLength = 0;
	ControlData.wCount = 0;

	ControlData.wLength = ControlData.DeviceRequest.wLength;

	if (ControlData.DeviceRequest.bmRequestType & (UCHAR)USB_ENDPOINT_DIRECTION_MASK)
	{
		DISABLE_INTERRUPTS();
		control_handler();
		RESTORE_INTERRUPTS();

		bEPPflags.bits.control_state = USB_TRANSMIT;
	}
	else
	{
		if (ControlData.DeviceRequest.wLength == 0)
		{
			DISABLE_INTERRUPTS();
			control_handler();
			RESTORE_INTERRUPTS();

			bEPPflags.bits.control_state = USB_IDLE;
		}
		else
		{
			if (ControlData.DeviceRequest.wLength > MAX_CONTROLDATA_SIZE)
			{

				bEPPflags.bits.control_state = USB_IDLE;
				MX25USB2_SetEndpointStatus(0, 1);
			}
			else
			{
				MX25USB2_ReadEndpoint(0, 64, true, ControlData.dataBuffer);
		 		bEPPflags.bits.control_state = USB_RECEIVE;
		 		totalDEP = 0;
			}
		}// set command with data
	}// else set command
}

inline void CUsb2Chip::EP0_rxdone(void)
{
	if (bEPPflags.bits.control_state == USB_RECEIVE)
	{
		datalenEP = MX25USB2_ReadEndpoint_Data(0, 64, true, ControlData.dataBuffer);	// only gets last-read length returned when 'true' is in there
		totalDEP += datalenEP;	// increment received size
		if((totalDEP < 64) ||(totalDEP >= ControlData.DeviceRequest.wLength))		// less than 64 or complete
		{
			Ep0DataHandler();
			bEPPflags.bits.control_state = USB_IDLE;
			totalDEP = 0;
		}
		else if( totalDEP < ControlData.DeviceRequest.wLength )	// not yet complete receiving
		{
			// read from EP at buffer + total read
			MX25USB2_ReadEndpoint(0, 64, true, ControlData.dataBuffer+totalDEP);
			bEPPflags.bits.control_state = USB_RECEIVE;
		}
		else	// totalDEP length = 0
		{
			bEPPflags.bits.control_state = USB_IDLE;
		}
	}
	else
	{
		bEPPflags.bits.control_state = USB_IDLE;
	}
}


MatrixError_t CUsb2Chip::SetupHardware(void)
{
	//safety
	DISABLE_INTERRUPTS();
	MatrixError_t RetVal = SetupHardwareAfterReset();
	RESTORE_INTERRUPTS();
	return RetVal;
}


MatrixError_t CUsb2Chip::SetupHardwareAfterReset(void)
{
	DisableInterruptNumber(VEC_USB2_INT);
	Rst_MX25USB2();		//reset MX25USB2

	Init_MX25USB2();

	EnableInterruptNumber(VEC_USB2_INT);

	return ERR_PASS;
}

void CUsb2Chip::ResetDevice (void)
{
	reset_to_boot_code_hard();
}

/*-----------------------------------------------------------------------------
 * @Name:
 *   TimeoutHack
 *
 * @Description:
 *    This is a hack fix for loss of communication when the D- line is broken
 *
 * @Paramters:
 *   EpNum    -- endpoint num.
 *   toutNum  -- Max time(second) for timeout
 *
 * @Return values:
 *
 *-----------------------------------------------------------------------------
 */
void CUsb2Chip::TimeoutHack(UCHAR EpNum, UINT toutNum, bool ReCon)
{

	CTimeout timer(RECONNECT_TEST_MS * MilliSec);   //100ms timeout
	UINT timeout = 0;

	while(EpHasData(EpNum))
	{
//		RESET_WATCHDOG_TIMER();
		MT_Delay(1);
		if( timer.HasExpired() )
		{
			timeout++;
			if(timeout > toutNum)  //toutNum s don't ACK, we'll reconnect
			{
				if(ReCon)
					Reconnect();
				else
					break;
			}
			MT_Delay(1);               //Delay 1ms
			timer.Reset();
		}
	}
}


