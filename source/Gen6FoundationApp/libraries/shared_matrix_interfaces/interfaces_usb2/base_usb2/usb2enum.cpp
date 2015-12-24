/* ==============================================================================
   USB Enumeration Module Source
   ==============================================================================

   $RCSfile: interfaces_usb2/base_usb2/usb2enum.cpp $
   $Revision: 1.26 $
   $Date: 2011/03/08 15:23:08EST $

//=================================================================================

//=================================================================================*/

#include <stdio.h>
#include "stdInclude.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h> 
#include "product.h"
#include "usb2enum.h"
#include "timeout.h"
#include "beep.h"
#include "UsbRem.h"

extern const UCHAR pxLanguageStringDescriptorA[4];
extern const UCHAR pxManufacturerStringDescriptorA[USB_SIZE_STRING_MANUFACTURER];
extern const UCHAR pxProductStringDescriptorA[USB_SIZE_STRING_PRODUCT];
extern const UCHAR pxSerialStringDescriptorA[USB_SIZE_STRING_SERIAL];
extern const UCHAR pxConfigurationStringDescriptorA[USB_SIZE_STRING_CONFIG];


CUsb2Enum::CUsb2Enum(CRxFifo *pRxFifo, CThreadRealTime *pThread)
: CUsb2Chip(pRxFifo, pThread)
{
}


CUsb2Enum::~CUsb2Enum( void )
{
}

//Chapter 9 start
USB_TESTPACKET bTestPacket =
{
	0xc3,
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0xaa,
	0xaa, 0xaa,
	0xaa, 0xaa,
	0xaa, 0xaa,
	0xaa, 0xee,  //aa*4
	0xee, 0xee,
	0xee, 0xee,
	0xee, 0xee,
	0xee, 0xfe,  //ee*4
	0xff, 0xff,
	0xff, 0xff,
	0xff, 0xff,
	0xff, 0xff,
	0xff, 0xff,  //FF*11
	0xff, 0x7f,
	0xbf, 0xdf,
	0xef, 0xf7,
	0xfb, 0xfd,
	0xfc, 0x7e,
	0xbf, 0xdf,
	0xef, 0xf7,
	0xfb, 0xfd,
	0x7e,
	0xb6, 0xce,  // crc
};

inline void CUsb2Enum::stall_ep0(void)
{
	stall_eps(0, 1);
}

inline void CUsb2Enum::stall_eps(UCHAR bEndp, UCHAR bStalled)
{
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
		return ;

	if(bStalled == 1)		
		reg_val |= EPCTRL_TX_EP_STALL | EPCTRL_RX_EP_STALL;
	else
	{
		reg_val &= ~EPCTRL_TX_EP_STALL;
		reg_val &= ~EPCTRL_RX_EP_STALL;
	}
		
	if(bEndp == 0)	
		UOG_ENDPTCTRL0 = reg_val;
	else if(bEndp == 1)	
		UOG_ENDPTCTRL1 = reg_val;
	else if(bEndp == 2)	
		UOG_ENDPTCTRL2 = reg_val;
	else if(bEndp == 3)	
		UOG_ENDPTCTRL3 = reg_val;
	else if(bEndp == 4)	
		UOG_ENDPTCTRL4 = reg_val;
	else if(bEndp == 5)	
		UOG_ENDPTCTRL5 = reg_val;
	else if(bEndp == 6)	
		UOG_ENDPTCTRL6 = reg_val;
	else if(bEndp == 7)	
		UOG_ENDPTCTRL7 = reg_val;	
}

// NOTE:  it appears code_transmit() does not send a 0 length packet if necessary to terminate the transfer.  I don't see any methods that use this check for this condition.
// See CUsbRem::OnClassRequest() for additional information.
void CUsb2Enum::code_transmit(const UCHAR *pRomData, USHORT len)
{
	DataStageEnable();
	MX25USB2_qu2ep_ctrl(1, len, 1, pRomData);	
}

inline void CUsb2Enum::get_status(void)
{
	UCHAR endp, txdat[2], MX25USB2_STALL;
	UCHAR bRecipient = ControlData.DeviceRequest.bmRequestType & USB_RECIPIENT;  

	if(ControlData.DeviceRequest.wValue == 0 && ControlData.DeviceRequest.wLength == 2 ){
		switch(bRecipient)
		{
		case USB_RECIPIENT_DEVICE:
			// Bit 0 is Self Powered: 0=false, 1=true
			// Bit 1 is Remote Wakeup: 0=not set, 1=set
			if(InputPowerStatus() == CABLE_POWERED)
			{
				//Bus Powered
				if(bEPPflags.bits.remote_wakeup == 1)	
				{
					txdat[0] = 0x02;
				}								
				else
				{
					txdat[0] = 0x00;
				}	
			}
			else
			{
				//Self Powered
				if(bEPPflags.bits.remote_wakeup == 1)	
				{
					txdat[0] = 0x03;
				}								
				else
				{
					txdat[0] = 0x01;
				}	
			}
														
			txdat[1]=0x00;										
			code_transmit(txdat, 2);
			
			break;			
		case USB_RECIPIENT_INTERFACE:		
			txdat[0]=0;
			txdat[1]=0;
			code_transmit(txdat, 2);	
			
			break;					

		case USB_RECIPIENT_ENDPOINT:
			endp = (ControlData.DeviceRequest.wIndex& MAX_ENDPOINTS_MASK);

			MX25USB2_STALL = MX25USB2_GetEndpointStatus(endp);		
			
			txdat[0] = MX25USB2_STALL&0x01;
			txdat[1] = 0;
			code_transmit(txdat, 2);			
						
			break;		
			
		default:
			stall_ep0();
			break;
			
		}									
	} 
	else
		stall_ep0();							
}

inline void CUsb2Enum::clear_feature(void)
{
	UCHAR endp;								
	UCHAR bRecipient = ControlData.DeviceRequest.bmRequestType & USB_RECIPIENT;

	if( ControlData.DeviceRequest.wLength == 0 ){
		switch(bRecipient)
		{
		case USB_RECIPIENT_DEVICE:															     
		if (ControlData.DeviceRequest.wValue == USB_FEATURE_REMOTE_WAKEUP) 
		{		
			bEPPflags.bits.remote_wakeup = 0;	
			StatusAcknowledge(1);								
		}
		else
		{
			stall_ep0();
		}		
		break;
	
		case USB_RECIPIENT_ENDPOINT:	
	        if (ControlData.DeviceRequest.wValue == USB_FEATURE_ENDPOINT_STALL) 
		{	
			endp = (UCHAR)(ControlData.DeviceRequest.wIndex & MAX_ENDPOINTS_MASK);				
					
			MX25USB2_SetEndpointStatus(endp, 0);				
					    	
			StatusAcknowledge(1);		
		}
		else
		{
			stall_ep0();
		}
		break;
		
		default:
			stall_ep0();
			break;
		}							
	} 
	else
		stall_ep0();		
}

inline void CUsb2Enum::set_feature(void)
{
	UCHAR endp;									
	UCHAR bRecipient = ControlData.DeviceRequest.bmRequestType & USB_RECIPIENT;
	UINT reg_val;

	if( ControlData.DeviceRequest.wLength == 0 )
	{        
		bEPPflags.bits.testmode = (UINT)(ControlData.DeviceRequest.wIndex >> 8);
		
		switch(bRecipient)
		{
		case USB_RECIPIENT_DEVICE:
			if (ControlData.DeviceRequest.wValue == USB_FEATURE_REMOTE_WAKEUP)
			{						
				bEPPflags.bits.remote_wakeup = 1;	
				StatusAcknowledge(1);	
			}	
			// added for highspeed;
			else if((ControlData.DeviceRequest.wValue == USB_FEATURE_TEST_MODE) &&\
					((ControlData.DeviceRequest.wIndex & 0xf8ff) == 0) &&\
					(bEPPflags.bits.usb_mode == HIGH_SPEED) &&\
					((bEPPflags.bits.testmode < 5) && (bEPPflags.bits.testmode > 0)))
			{
				StatusAcknowledge(1);	
				reg_val = UOG_PORTSC1;
		
				switch(bEPPflags.bits.testmode)
				{
					case USB_FEATURE_TEST_J: // force high speed j state
					{	
						reg_val |= 0x01 <<16;
						break;
					}
					case USB_FEATURE_TEST_K: // force high speed k state
					{	
						reg_val |= 0x02 <<16;
						break;
					}
					case USB_FEATURE_TEST_SE0_NAK:
					{	
						reg_val |= 0x03 <<16;
						break;
					}
					case USB_FEATURE_TEST_Packet:
					{	
						reg_val |= 0x04 <<16;
						break;
					}
				}
				UOG_PORTSC1 = reg_val;										
			}
			else
			{
				stall_ep0();		
			}
			break;
			
		case USB_RECIPIENT_ENDPOINT:			
			endp = (ControlData.DeviceRequest.wIndex & MAX_ENDPOINTS_MASK);
								
			if (ControlData.DeviceRequest.wValue == USB_FEATURE_ENDPOINT_STALL) 
			{
				MX25USB2_SetEndpointStatus(endp, 1);					
			   			
				StatusAcknowledge(1);				
			}
			else
			{
				MX25USB2_SetEndpointStatus(endp, 0);			
				StatusAcknowledge(1);	
			}
			
			break;
		default:		
			stall_ep0();		
			break;
		}	
	}else
	{
		stall_ep0();	
	} 		
}

inline void CUsb2Enum::set_address(void)
{
	StatusAcknowledge(1); 
	m_BusState = USB_ADDR_ASSIGNED;	// fall back to addressed state	
	m_UsbAddress = ControlData.DeviceRequest.wValue; // Save address to be set in hardware	
}

inline void CUsb2Enum::get_descriptor(void)
{
	UCHAR bDescriptor = MSB(ControlData.DeviceRequest.wValue);
	
	switch(bDescriptor)
	{
		case USB_DEVICE_DESCRIPTOR_TYPE:
			{
				if (ControlData.DeviceRequest.wLength > sizeof(USB_DEVICE_DESCRIPTOR))
					ControlData.DeviceRequest.wLength = sizeof(USB_DEVICE_DESCRIPTOR); //sizeof(DeviceDescr);

				code_transmit((UCHAR *)DeviceDescr, ControlData.DeviceRequest.wLength);	

				break;	
			}
		case USB_CONFIGURATION_DESCRIPTOR_TYPE:
			{
				if((bEPPflags.bits.usb_mode == FULL_SPEED))	
				{
					if (ControlData.DeviceRequest.wLength > usb_descrLEN_FS) ControlData.DeviceRequest.wLength = usb_descrLEN_FS;
					code_transmit((UCHAR *)(usb_descr_FS), ControlData.DeviceRequest.wLength);								
                                }
				else		
				{
					if (ControlData.DeviceRequest.wLength > usb_descrLEN_HS) ControlData.DeviceRequest.wLength = usb_descrLEN_HS;
					code_transmit((UCHAR *)(usb_descr_HS), ControlData.DeviceRequest.wLength);
				}
				break;	
			}
			
		//0x03 string descriptor
		// TODO:  we don't handle this as well as we can--for example, some interfaces will not have INTERFACE2_IDX and we should send a STALL packet. Consider using CDescriptorQueue.
		case USB_STRING_DESCRIPTOR_TYPE:
			/* The index to the string descriptor is the lower byte. */
			switch( (ControlData.DeviceRequest.wValue) & 0xff )
			{		
				case LANGUAGE_IDX:				        
        				if (ControlData.DeviceRequest.wLength > sizeof(pxLanguageStringDescriptorA))
        					ControlData.DeviceRequest.wLength = sizeof(pxLanguageStringDescriptorA);
        				code_transmit((UCHAR *)pxLanguageStringDescriptorA, ControlData.DeviceRequest.wLength);	
				                                                
					break;

				case MANUFACTURER_IDX:
				        if (ControlData.DeviceRequest.wLength > sizeof(pxManufacturerStringDescriptorA))
				        	ControlData.DeviceRequest.wLength = sizeof(pxManufacturerStringDescriptorA);
				        code_transmit((UCHAR *)pxManufacturerStringDescriptorA, ControlData.DeviceRequest.wLength);
				            	                                
	                                break;

				case PRODUCT_IDX:
				        if (ControlData.DeviceRequest.wLength > pxProductStrDesLEN)
				        	ControlData.DeviceRequest.wLength = pxProductStrDesLEN;
				        code_transmit((UCHAR *)pxProductStringDescriptorA, ControlData.DeviceRequest.wLength);								    				        

					break;

				case CONFIGURATION_IDX:
				        if (ControlData.DeviceRequest.wLength > sizeof(pxConfigurationStringDescriptorA))
         				    ControlData.DeviceRequest.wLength = sizeof(pxConfigurationStringDescriptorA);
                                        code_transmit((UCHAR *)pxConfigurationStringDescriptorA, ControlData.DeviceRequest.wLength);

					break;

				case INTERFACE1_IDX:
				        if (ControlData.DeviceRequest.wLength > pxInterfaceStrDesLEN[0])
         				    ControlData.DeviceRequest.wLength = pxInterfaceStrDesLEN[0];
				        code_transmit((UCHAR *)pxInterfaceStringDescriptor[0], ControlData.DeviceRequest.wLength);

					break;
					
				case INTERFACE2_IDX:
				        if (ControlData.DeviceRequest.wLength > pxInterfaceStrDesLEN[1])
         				    ControlData.DeviceRequest.wLength = pxInterfaceStrDesLEN[1];
				        code_transmit((UCHAR *)pxInterfaceStringDescriptor[1], ControlData.DeviceRequest.wLength);

					break;
					
				case REM_INTERFACE_IDX:
					if ( m_bRemIfaceEnabled )
					{
				        if (ControlData.DeviceRequest.wLength > CUsbRem::m_RemInterfaceStrDesc.bLength) ControlData.DeviceRequest.wLength = CUsbRem::m_RemInterfaceStrDesc.bLength;
				        code_transmit((const UCHAR *)&CUsbRem::m_RemInterfaceStrDesc, ControlData.DeviceRequest.wLength);
					}
					else stall_ep0();
					break;
					
				case SERIAL_IDX:		// Serial Number
					if (ControlData.DeviceRequest.wLength > pxSerialStrDesLEN)
						ControlData.DeviceRequest.wLength = pxSerialStrDesLEN;
				        code_transmit((UCHAR *)pxSerialStringDescriptorA, ControlData.DeviceRequest.wLength);
					break;

				default:
			               stall_ep0();					

			               break;
			}
			break;
			
		//0x06			
		case USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE:

			if (ControlData.DeviceRequest.wLength > sizeof(HS_DEVICE_QUALIFIER))
				ControlData.DeviceRequest.wLength = sizeof(HS_DEVICE_QUALIFIER); //sizeof(DeviceDescr);
		
			code_transmit((UCHAR *)HS_Device_Qualifier, ControlData.DeviceRequest.wLength);	
			break;

		//other speed discriptor
		case USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE: 
			if (ControlData.DeviceRequest.wLength > usb_othersp_descrLEN)
				ControlData.DeviceRequest.wLength = usb_othersp_descrLEN;

			code_transmit((UCHAR *)(usb_othersp_descr), ControlData.DeviceRequest.wLength);		

			break;
		
		//0x21
		case USB_HID_DESCRIPTOR_TYPE:
		{
			CDescriptorQueue::Descriptor_t descriptor;
			int WhichInterface = ControlData.DeviceRequest.wIndex;
			if ( m_HidDescriptors.Find(WhichInterface, &descriptor) ) code_transmit(descriptor.pDescData, descriptor.DescLength);
			else stall_ep0();

			break;
		}

		//0x22
		case USB_REPORT_DESCRIPTOR_TYPE:
		{
			CDescriptorQueue::Descriptor_t descriptor;
			int WhichInterface = ControlData.DeviceRequest.wIndex;
			if ( m_ReportDescriptors.Find(WhichInterface, &descriptor) ) code_transmit(descriptor.pDescData, descriptor.DescLength);
			else stall_ep0();

			break;
		}
			
		case USB_INTERFACE_DESCRIPTOR_TYPE:
		case USB_ENDPOINT_DESCRIPTOR_TYPE:
//		case USB_POWER_DESCRIPTOR_TYPE:			
		default:
			stall_ep0();	
			break;						
	}

}

inline void CUsb2Enum::get_configuration(void)
{
	UCHAR c = bEPPflags.bits.configuration;			
	
	if (ControlData.DeviceRequest.wValue == 0 && ControlData.DeviceRequest.wIndex == 0 && ControlData.DeviceRequest.wLength == 1)
	{
		code_transmit(&c, 1);	
	}
	else
	{
		stall_ep0();
	}				
}

inline void CUsb2Enum::set_configuration(void)
{
	if (ControlData.DeviceRequest.wValue == 0) 
	{												
		bEPPflags.bits.configuration = 0;
		bInterface_num = 0;				

		StatusAcknowledge(1);				

		m_BusState = USB_ADDR_SET;	// fall back to addressed state								
	} 
	
	else if (ControlData.DeviceRequest.wValue == 1)
	{	
		bEPPflags.bits.configuration = 1;	
		bInterface_num = 0;	
		
		StatusAcknowledge(1);			

		m_BusState = USB_CONFIGURED;	
		SoundPowerupBeep();		        //Power up sound
		if((m_ConTimes == 0)&&(termid_eps == 130))
		{
			m_ConTimes += 1;
			m_dataBufCtrl = true;
			MX25USB2_ReadEndpoint(7, rxMaxLenIDs, false, dataFromEP);  //Rx data from endpoint 7,max len RXMAXSIZE for code download			
		}			
	} else
		stall_ep0();	
}

inline void CUsb2Enum::get_interface(void)
{
	if (ControlData.DeviceRequest.wValue == 0 && ControlData.DeviceRequest.wLength == 1)
		code_transmit(&bInterface_num, 1);	
	else
		stall_ep0();			
}

inline void CUsb2Enum::set_interface(void)
{
	if (ControlData.DeviceRequest.wLength == 0)
	{
		bInterface_num = (UCHAR)(ControlData.DeviceRequest.wValue);

		StatusAcknowledge(1);					
	}	
	else		
		stall_ep0();						
}

inline void CUsb2Enum::control_handler(void)
{
	UCHAR type, req, reqtype;

	reqtype = ControlData.DeviceRequest.bmRequestType;
	type = reqtype & USB_REQUEST_TYPE_MASK;
												
	req = ControlData.DeviceRequest.bRequest & USB_REQUEST_MASK;

	if ((type == USB_STANDARD_REQUEST) && (req < 0xd))
    {
		switch( req )
		{
			case 0x00:
				get_status();
				break;
				
			case 0x01:
				clear_feature();
				break;
				
			case 0x03:
				set_feature();
				break;
				
			case 0x05:
				set_address();
				break;
				
			case 0x06:
				get_descriptor();
				break;
				
			case 0x08:
				get_configuration();
				break;
				
			case 0x09:
				set_configuration();
				break;
				
			case 0x0a:
				get_interface();
				break;
				
			case 0x0b:
				set_interface();
				break;
			
			case 0x02:	
			case 0x04:
			case 0x07:
			case 0x0c:
			case 0x0d:
			case 0x0e:
			case 0x0f:
			default:
				stall_ep0();
				break;
		}
	}
	else if (type == USB_CLASS_REQUEST) 
	{	// This might be a HID class request meant for the ReM interface.
		unsigned char ReqRecipient = reqtype & USB_RECIPIENT;
		if ( m_bRemIfaceEnabled && (ReqRecipient == USB_RECIPIENT_INTERFACE) && (ControlData.DeviceRequest.wIndex == (GetMaxNumIfaces() - 1)) )  // ReM is always the last interface
		{	// This is a request to the interface, see if ReM wants to handle it.
			CUsbRem rem;
			unsigned char HidPacket[HID_USBPOS_IN_REPORT_SIZE];
			int NumToSend;
			bool bSendPacket;

			if ( rem.OnClassRequest(&ControlData.DeviceRequest, HidPacket, &NumToSend, &bSendPacket) )
			{
				if ( bSendPacket )
				{
					code_transmit(HidPacket, NumToSend);
				}
			}
			else stall_ep0();
		}
		else OnClassRequest();		
	}
	else if ((type == USB_VENDOR_REQUEST) && (req < 0x0f))
	{
		OnVendorRequest();
	}				
	else
	{
		stall_ep0();
	}
}




