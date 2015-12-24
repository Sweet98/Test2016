/*===================================================================
  See .cpp file for description 

=====================================================================
   $Source: interfaces_usb2/rem/UsbRem.h $
   $Date: 2011/03/31 14:41:24EDT $
   $Revision: 1.2 $


===================================================================*/
#ifndef _RemUsbIface_h_
#define _RemUsbIface_h_

#include "usbdesc.h"
#include "usb2pos.h"
#include "usb2chip.h"
#include "rxfifo.h"

class CUsbRem
{
public:
	
	enum {MyInEndpoint = 6, MyOutEndpoint = 3};
	typedef struct
	{
		InterfaceDescriptor_t IfaceDesc;
		HIDDescriptor1_t HidDesc;
		EndpointDescriptor_t InEndpoint;
		EndpointDescriptor_t OutEndpoint;
	} IfaceDescriptor_t;
	
	typedef struct
	{
		unsigned char bLength;
		unsigned char bDescriptorType;
		unsigned char bString[3*2];  // unicode for "REM".  We'll keep it unsigned char instead of WORD so we don't ever need to worry about padding.
	} IfaceStrDescriptor_t;

	static IfaceDescriptor_t m_IfaceDescriptor;
	const static IfaceStrDescriptor_t m_RemInterfaceStrDesc;
	const static HIDDescriptor1_t m_HidDescriptor;
	const static unsigned char m_ReportDescriptor[];

	static void Init(void);
	/*The design of the classes isn't quite right and I didn't want to exacerbate the problem. I didn't want to put this class in the already long chain of derived classes (it would've
	gone right after CUsb2Enum and all "later" classes would be derived from this one).  Unfortunately, it does require the use of some objects.*/
	void SendData(CUsb2Chip * pUsb2Chip, const unsigned char * pData, int length);
	void HandleRxData(const unsigned char * pBuffer, int length, CRxFifo * pRxFifo);
	bool OnClassRequest(const volatile DEVICE_REQUEST * pDevReq, unsigned char * pPacketToSend, int * pNumToSend, bool * pbSendPacket);
protected:
};


#endif // enof #ifndef _RemUsbIface_h_


