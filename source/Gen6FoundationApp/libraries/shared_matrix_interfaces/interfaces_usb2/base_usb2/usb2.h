/* ==============================================================================
   USB communication declarations
   ==============================================================================

   $RCSfile: interfaces_usb2/base_usb2/usb2.h $
   $Revision: 1.26 $
   $Date: 2011/11/10 20:53:31EST $

//=================================================================================

//=================================================================================*/

#ifndef _USB2_DECLARATIONS_H_
#define _USB2_DECLARATIONS_H_
 
#include "product.h"
#include <stdio.h>
#include <stdarg.h>
     
//
//	USB2_TRACE is a macro that can be used to send debug output to a debug port.  If not already
//	defined, it is defined here to do nothing.  It can be overridden via StdInclude.h by adding
//	a working USB2_TRACE macro to platform.h or one of the other platform specific files included
//	via StdInclude.h
//
extern void TraceOutput( const char *TraceInformation );

#ifndef	USB2_TRACE
#define	USB2_TRACE(n)	TraceOutput( n )
#endif
  
#define USB2_BASE_PID	         0x0500 
#define USB2_CDC_ACM_PID	 USB2_BASE_PID + 0x66 	
 
#define USB2DBG			 0		                //debug infos switch,1-open,0-close
#define DATABSIZE                2		                //DATABSIZEx16k data block size
#define RXMAXSIZE                1536		                //Max size 1029 for code download from QV
#define TXMAXSIZE                320                            //Buffer for Pos

#define MSB(x)    (((x) >> 8) & 0xFF)

#define FULL_SPEED              0
#define HIGH_SPEED		1
#define LOW_SPEED		3

#define MAX_ENDPOINTS_MASK      (UCHAR)0x07
#define USB_REQUEST_DIR_MASK     (UCHAR)0x80

#define USB_IDLE           0
#define USB_TRANSMIT       1
#define USB_RECEIVE        2

#define MAX_CONTROLDATA_SIZE	264   //surePos max size is 263 bytes

/* Bit field flags */
#define   BIT0				  (0x00000001 << 0)
#define   BIT1          	          (0x00000001 << 1)

#define USB_SIZE_STRING_MANUFACTURER 1+1+(32*2)
#define USB_SIZE_STRING_PRODUCT 1+1+(32*2)
#define USB_SIZE_STRING_SERIAL 1+1+(32*2)
#define USB_SIZE_STRING_CONFIG 1+1+(7*2)

typedef union _epp_flags
{
	struct _flags
	{			
		UINT configuration	: 1;			
		UINT remote_wakeup	: 1;	      
		UINT usb_mode		: 1;																						
		UINT control_state	: 2;
		UINT testmode		: 3;																							 				       	
 	} bits;
	
	UCHAR value;
	
} EPPFLAGS;

typedef struct _device_request
{
	UCHAR bmRequestType;
	UCHAR bRequest;
	USHORT wValue;
	USHORT wIndex;
	USHORT wLength;
} DEVICE_REQUEST;

typedef struct _control_xfer
{
	DEVICE_REQUEST DeviceRequest;
	USHORT wLength;
	USHORT wCount;
	UCHAR * pData;
	UCHAR * dataBuffer;
} CONTROL_XFER;

typedef struct _USB_TESTPACKET {
	UCHAR  TESTPATTERN[56];     //TESTPATTERN[56];
} USB_TESTPACKET,*PUSB_TESTPACKET;

typedef struct _USB_DEVICE_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType;
    UCHAR bcdUSB0;
    UCHAR bcdUSB1;
    UCHAR bDeviceClass;
    UCHAR bDeviceSubClass;
    UCHAR bDeviceProtocol;
    UCHAR bMaxPacketSize0;
    UCHAR idVendor0;
    UCHAR idVendor1;
    UCHAR idProduct0;
    UCHAR idProduct1;
    UCHAR bcdDevice0;
    UCHAR bcdDevice1;
    UCHAR iManufacturer;
    UCHAR iProduct;
    UCHAR iSerialNumber;
    UCHAR bNumConfigurations;
} USB_DEVICE_DESCRIPTOR, *PUSB_DEVICE_DESCRIPTOR;

typedef struct _USB_CONFIG_DESCRIPTOR
{
	UCHAR bLength;					// size of descr in bytes
	UCHAR bDescriptorType;		// USB_CONFIG_DESCR
	UCHAR wTotalLengthLow;		// total length of data returned
	UCHAR wTotalLengthHigh;		// total length of data returned
	UCHAR bNumInterfaces;		// number of interfaces supported
	UCHAR bConfigurationValue;	// Set Configuration selection
	UCHAR iConfiguration;		// index string: configuration
	UCHAR bmAttributes;			// configuration characteristics
	UCHAR MaxPower;				// maximum power consumption
} USB_CONFIG_DESCRIPTOR, *PUSB_CONFIG_DESCRIPTOR;

typedef struct HS_DEVICE_QUALIFIER
{
	UCHAR	bLength;		//length of HS Device Descriptor
	UCHAR	bQualifier; 		//HS Device Qualifier Type
	UCHAR	wVersion0;		// USB 2.0 version
	UCHAR	wVersion1;
	UCHAR	bDeviceClass;		//Device class
	UCHAR	bDeviceSubClasss;	//Device SubClass
	UCHAR	bProtocol;	        //Device Protocol Code
	UCHAR	MaxPktSize;		//Maximum Packet SIze for other speed
	UCHAR	bOther_Config;		//Number of Other speed configurations
	UCHAR	Reserved;		//Reserved
}HS_DEVICE_QUALIFIER;

#define USB_RECIPIENT            (UCHAR)0x1F
#define USB_RECIPIENT_DEVICE     (UCHAR)0x00
#define USB_RECIPIENT_INTERFACE  (UCHAR)0x01
#define USB_RECIPIENT_ENDPOINT   (UCHAR)0x02

#define USB_REQUEST_TYPE_MASK    (UCHAR)0x60
#define USB_STANDARD_REQUEST     (UCHAR)0x00
#define USB_CLASS_REQUEST        (UCHAR)0x20
#define USB_VENDOR_REQUEST       (UCHAR)0x40

#define USB_REQUEST_MASK         (UCHAR)0x0F
#define USB_ENDPOINT_DIRECTION_MASK               0x80

#define USB_DEVICE_DESCRIPTOR_TYPE                0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE         0x02
#define USB_STRING_DESCRIPTOR_TYPE                0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE             0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE              0x05
#define USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE      0x06 // usb 2.0
#define USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE    0x07 // usb 2.0
#define USB_POWER_DESCRIPTOR_TYPE                 0x08 // usb 2.0
// HID Class descriptors
#define USB_HID_DESCRIPTOR_TYPE                   0x21
#define USB_REPORT_DESCRIPTOR_TYPE                0x22

#define USB_FEATURE_ENDPOINT_STALL          0x0000

#define USB_FEATURE_REMOTE_WAKEUP           0x0001
#define USB_FEATURE_TEST_MODE		    0x0002

#define USB_FEATURE_TEST_J		    0x01
#define USB_FEATURE_TEST_K		    0x02
#define USB_FEATURE_TEST_SE0_NAK	    0x03
#define USB_FEATURE_TEST_Packet		    0x04
#define USB_FEATURE_TEST_Force_Enable	    0x05    // for USB2.0 HUB downstream port only


// Custom String Length variables (max 32 unicode)
const int CUSTOMIZABLE_STRING_LENGTH = 32; 	      // in Unicode chars

//following is old
#ifdef __C51__	                                  // KEIL C8051 Compiler
#define CONST code const
typedef UCHAR endpoint_t;
typedef UCHAR interface_t;
typedef UCHAR feature_t;
typedef short usb_size_t;
typedef bit usb_err_t;
#else															 // All other compilers
#define idata
#define pdata
#define xdata
#define code
#define CONST const
typedef USHORT endpoint_t;
typedef USHORT interface_num_t;
typedef USHORT feature_t;
typedef long usb_size_t;
#define RET_WAIT		FAIL
#define RET_STALL		FAIL
#define RET_ACK		PASS
#define RET_NOTHING	PASS+1
//#ifdef MatrixError_t
typedef MatrixError_t usb_err_t;
//#else
//typedef USHORT usb_err_t;
//#endif
#endif

struct tag_USB_Variables_t;			    // forward declaration to make the GNU compiler happy

#define USB_WORD16(x)	((x) & 0xFF), (((x) >> 8) & 0x00FF)
#define UNI(x)				USB_WORD16(x)

#define NULL_DATA_PACKET		0

#ifndef USB_BASE_PRODUCT_ID
#error "You need to define USB_BASE_PRODUCT_ID in product.h"
#endif

// Use the BASE number for device global functions Firmware flash etc. 
#define USB_KBD_PC_PID_OFFSET		0x0001
#define USB_KBD_MAC_PID_OFFSET		0x0002
#define USB_SUREPOS_HH_PID_OFFSET	0x0003
#define USB_SUREPOS_TT_PID_OFFSET	0x0004
#define USB_SER_PID_OFFSET			0x0005
#define USB_FTDI_SER_PID_OFFSET		0x0006
#define USB_POS_PID_OFFSET			0x0007
#define USB_OPOS_PID_OFFSET			0x0008
#define USB_SUREPOS_SIO_PID_OFFSET	0x0009
#define USB_MODEM_ACM_PID_OFFSET	0x000A
#define USB_IRDA_PID_OFFSET			0x000B
#define USB_STILL_VIDEO_PID_OFFSET	0x000C
#define USB_MOVIE_VIDEO_PID_OFFSET	0x000D
#define USB_MASS_STORAGE_PID_OFFSET	0x000E
// ReM PIDs
#define USB_KBD_PC_REM_PID_OFFSET		0x000F
#define USB_KBD_MAC_REM_PID_OFFSET		0x0010
#define USB_SUREPOS_HH_REM_PID_OFFSET	0x0011
#define USB_SUREPOS_TT_REM_PID_OFFSET	0x0012
#define USB_POS_REM_PID_OFFSET			0x0013
#define USB_MODEM_ACM_REM_PID_OFFSET	0x0014

#define USB_KBD_PC_PRODUCT_ID		(USB_BASE_PRODUCT_ID + USB_KBD_PC_PID_OFFSET)
#define USB_KBD_MAC_PRODUCT_ID		(USB_BASE_PRODUCT_ID + USB_KBD_MAC_PID_OFFSET)
#define USB_SUREPOS_HH_PRODUCT_ID	(USB_BASE_PRODUCT_ID + USB_SUREPOS_HH_PID_OFFSET)
#define USB_SUREPOS_TT_PRODUCT_ID	(USB_BASE_PRODUCT_ID + USB_SUREPOS_TT_PID_OFFSET)
#define USB_SER_PRODUCT_ID			(USB_BASE_PRODUCT_ID + USB_SER_PID_OFFSET)
#define USB_FTDI_SER_PRODUCT_ID		(USB_BASE_PRODUCT_ID + USB_FTDI_SER_PID_OFFSET)
#define USB_POS_PRODUCT_ID			(USB_BASE_PRODUCT_ID + USB_POS_PID_OFFSET)
#define USB_OPOS_PRODUCT_ID			(USB_BASE_PRODUCT_ID + USB_OPOS_PID_OFFSET)
#define USB_SUREPOS_SIO_PRODUCT_ID	(USB_BASE_PRODUCT_ID + USB_SUREPOS_SIO_PID_OFFSET)
#define USB_MODEM_ACM_PRODUCT_ID	(USB_BASE_PRODUCT_ID + USB_MODEM_ACM_PID_OFFSET)
#define USB_IRDA_PRODUCT_ID			(USB_BASE_PRODUCT_ID + USB_IRDA_PID_OFFSET)
#define USB_STILL_VIDEO_PRODUCT_ID	(USB_BASE_PRODUCT_ID + USB_STILL_VIDEO_PID_OFFSET)
#define USB_MOVIE_VIDEO_PRODUCT_ID	(USB_BASE_PRODUCT_ID + USB_MOVIE_VIDEO_PID_OFFSET)
#define USB_MASS_STORAGE_PRODUCT_ID	(USB_BASE_PRODUCT_ID + USB_MASS_STORAGE_PID_OFFSET)

#define USB_DEVICE_ID					REVISION_BCD	

#define USB_SPECIFICATION				0x0110	

#define USB_REQUEST_SIZE 8				// All USB requests are 8 bytes in size (+data), see chapter 9 

#define	USB_GET_CLASS_INTERFACE		0xa1
#define	USB_GET_VENDOR_INTERFACE	0xc1

#define	USB_SET_CLASS_INTERFACE		0x21

/*
 * USB states
 */
enum UsbBusState
{
	USB_UNPLUGGED=0,
	USB_DEFAULT,		// plugged in, but No descriptor read
	USB_ADDR_ASSIGNED,	// The host has given us an address but we did not set it in hardware yet
	USB_ADDR_SET,		// We have set the address in the device
	USB_CONFIGURED,
	USB_BUS_STATE_MASK=0x07,
	USB_ADDRESS_ASSIGNED = USB_ADDR_ASSIGNED	/* for older sources */
};

//iMX25 USB OTG
typedef struct _ep_qh {
	UINT max_pkt_length;	// Mult(31-30) , Zlt(29) , Max Pkt len and IOS(15)
	UINT curr_dtd_ptr;	// Current dTD Pointer(31-5) 
	UINT next_dtd_ptr;	// Next dTD Pointer(31-5), T(0) 
	UINT size_ioc_int_sts;	// Total bytes (30-16), IOC (15),
				        // MultO(11-10), STS (7-0)  
	UINT buff_ptr0;		// Buffer pointer Page 0 (31-12) 
	UINT buff_ptr1;		// Buffer pointer Page 1 (31-12) 
	UINT buff_ptr2;		// Buffer pointer Page 2 (31-12) 
	UINT buff_ptr3;		// Buffer pointer Page 3 (31-12) 
	UINT buff_ptr4;		// Buffer pointer Page 4 (31-12) 
	UINT res1;
	UCHAR  setup_buffer[8];		// Setup data 8 bytes 
	UINT res2[4];			// pad out to 64 bytes 
} EP_QUEUE_HEAD;

struct ep_tdt_queue {
	UINT next_td_ptr;	        // Next TD pointer(31-5), T(0) set indicate invalid 
	UINT size_ioc_sts;	        // Total bytes (30-16), IOC (15),MultO(11-10), STS (7-0)
	UINT buff_ptr0;		// Buffer pointer Page 0 (31-12)
	UINT buff_ptr1;		// Buffer pointer Page 1 (31-12)
	UINT buff_ptr2;		// Buffer pointer Page 2 (31-12)
	UINT buff_ptr3;		// Buffer pointer Page 3 (31-12)
	UINT buff_ptr4;		// Buffer pointer Page 4 (31-12)
	UINT res;		        // make it an even 8 words 
};

typedef struct ep_tdt_queue USBD_dTD_T;

typedef struct _USBD_QH
{
    EP_QUEUE_HEAD qh[16];
    USBD_dTD_T  dtd[2];
    USBD_dTD_T dtd_nxt[14][DATABSIZE];
} USBD_QH_T;

// Endpoint Transfer Descriptor
#define  DTD_NEXT_TERMINATE                   (0x00000001)
#define  DTD_IOC                              (0x00008000)
#define  DTD_STATUS_ACTIVE                    (0x00000080)
#define  DTD_STATUS_HALTED                    (0x00000040)
#define  DTD_STATUS_DATA_BUFF_ERR             (0x00000020)
#define  DTD_STATUS_TRANSACTION_ERR           (0x00000008)
#define  DTD_RESERVED_FIELDS                  (0x80007300)
#define  DTD_ADDR_MASK                        (0xFFFFFFE0)
#define  DTD_PACKET_SIZE                      (0x7FFF0000)
#define  DTD_LENGTH_BIT_POS                   (16)
#define  DTD_ERROR_MASK                       (DTD_STATUS_HALTED | \
                                               DTD_STATUS_DATA_BUFF_ERR | \
                                               DTD_STATUS_TRANSACTION_ERR)

// Endpoints
#define USB_ENDPOINT_NUMBER_MASK	0x0f	/* in bEndpointAddress */
#define USB_ENDPOINT_DIR_MASK		0x80

#define USB_ENDPOINT_XFERTYPE_MASK	0x03	/* in bmAttributes */
#define USB_ENDPOINT_XFER_CONTROL	0
#define USB_ENDPOINT_XFER_ISOC		1
#define USB_ENDPOINT_XFER_BULK		2
#define USB_ENDPOINT_XFER_INT		3
#define USB_ENDPOINT_MAX_ADJUSTABLE	0x80

// ENDPOINTCTRLx  Register Bit Masks
#define  EPCTRL_TX_ENABLE                     (0x00800000)
#define  EPCTRL_TX_DATA_TOGGLE_RST            (0x00400000)	/* Not EP0 */
#define  EPCTRL_TX_DATA_TOGGLE_INH            (0x00200000)	/* Not EP0 */
#define  EPCTRL_TX_TYPE                       (0x000C0000)
#define  EPCTRL_TX_DATA_SOURCE                (0x00020000)	/* Not EP0 */
#define  EPCTRL_TX_EP_STALL                   (0x00010000)
#define  EPCTRL_RX_ENABLE                     (0x00000080)
#define  EPCTRL_RX_DATA_TOGGLE_RST            (0x00000040)	/* Not EP0 */
#define  EPCTRL_RX_DATA_TOGGLE_INH            (0x00000020)	/* Not EP0 */
#define  EPCTRL_RX_TYPE                       (0x0000000C)
#define  EPCTRL_RX_DATA_SINK                  (0x00000002)	/* Not EP0 */
#define  EPCTRL_RX_EP_STALL                   (0x00000001)

#define USB_RECV	(0)	/* OUT EP */
#define USB_SEND	(1)	/* IN EP */

// bit 19-18 and 3-2 are endpoint type
#define  EPCTRL_EP_TYPE_CONTROL               (0)
#define  EPCTRL_EP_TYPE_ISO                   (1)
#define  EPCTRL_EP_TYPE_BULK                  (2)
#define  EPCTRL_EP_TYPE_INTERRUPT             (3)
#define  EPCTRL_TX_EP_TYPE_SHIFT              (18)
#define  EPCTRL_RX_EP_TYPE_SHIFT              (2)

// Endpoint Queue Head
#define  EP_QUEUE_HEAD_MULT_POS               (30)
#define  EP_QUEUE_HEAD_ZLT_SEL                (0x20000000)
#define  EP_QUEUE_HEAD_MAX_PKT_LEN_POS        (16)
#define  EP_QUEUE_HEAD_MAX_PKT_LEN(ep_info)   (((ep_info)>>16)&0x07ff)
#define  EP_QUEUE_HEAD_IOS                    (0x00008000)
#define  EP_QUEUE_HEAD_NEXT_TERMINATE         (0x00000001)
#define  EP_QUEUE_HEAD_IOC                    (0x00008000)
#define  EP_QUEUE_HEAD_MULTO                  (0x00000C00)
#define  EP_QUEUE_HEAD_STATUS_HALT	      (0x00000040)
#define  EP_QUEUE_HEAD_STATUS_ACTIVE          (0x00000080)
#define  EP_QUEUE_CURRENT_OFFSET_MASK         (0x00000FFF)
#define  EP_QUEUE_HEAD_NEXT_POINTER_MASK      (0xFFFFFFE0)
#define  EP_QUEUE_FRINDEX_MASK                (0x000007FF)
#define  EP_MAX_LENGTH_TRANSFER               (0x4000 - 1)  // To avoid ZLP

// Device Address bit masks 
#define  USB_DEVICE_ADDRESS_MASK              (0xFE000000)
#define  USB_DEVICE_ADDRESS_BIT_POS           (25)

#define EP_DIR_IN	1
#define EP_DIR_OUT	0
//iMX25 USB OTG end

#define	USB_DIRECTION_MASK			0x80


#if(0)
typedef pdata struct tag_USB_Variables_t
{
	volatile UCHAR		USBState;					/* USB Driver status */
	volatile UCHAR		BusState;   				/* USB status */
	volatile UCHAR		DeviceStatus[2];        /* for device status requests */
#ifndef AUTOMATIC_USB_ADDRESS
	volatile UCHAR		Address;             	/* USB device address */
	volatile UCHAR		NewAddress;          	/* used to regognize a change in address */
#endif
	volatile UCHAR		CurrentConfiguration;	/* set by the SetConfiguration request */
	volatile UINT	NumInterfaces;
	volatile enum UsbTransactionStage Stage;	/* stages for cntrl transfers */
#ifndef USE_ONCHIP_USB_BUFFERS
	PhysicalDeviceRequest_t UsbCmd;				/* request in USB form (little endian) */
#endif

//#ifdef BIG_ENDIAN
	DeviceRequest_t Cmd;								/* request translated into native proccessor form (big or little endian) */
//#endif

#ifndef NO_DESCRIPTOR_HANDLER
	const struct tag_USB_DescriptorChain_t code *pDescriptorChain;
#endif

#ifndef USE_FIXED_CONFIG_HANDLER
	BOOL (code* NewConfig)(struct tag_USB_Variables_t *const pUsb);
#endif

#ifdef CHIP_SPECIFIC_DATA
	CHIP_SPECIFIC_DATA;
#endif
} USB_Variables_t;

// interface to hardware chip
//void BeginnDataIn(USB_Variables_t *pUsb, UCHAR endpoint, const UCHAR *pSource, size_t size);

BOOL freopen_Usb(void);
BOOL fclose_Usb(void);

#ifndef USE_FIXED_CONFIG_HANDLER
BOOL fopen_Usb(const struct tag_USB_DescriptorChain_t code *pChain,
						BOOL (code *NewConfig)(struct tag_USB_Variables_t * const pUsb)
					);
#else
BOOL fopen_Usb(void);
#endif

BOOL USBWaitUntilWeAreConfigured(void);
void USBReconnect(USB_Variables_t *pUsb);
void USBReset(USB_Variables_t *pUsb);
void USBSendResume(void);

#ifdef USE_STATIC_LINKED_USB_VARS
extern USB_Variables_t gUsbVars;
#endif

#ifdef _WIN32
#define sleep()
#endif
#endif

//For surePos
inline int _vsnprintf( bchar_t *buffer, size_t count, const bchar_t *format, va_list argptr )
{
	return _vsnprintf( (char *) buffer, count, (const char *)format, argptr );
}

// same, but no '_'
#ifndef WIN32
inline int vsnprintf( bchar_t *buffer, size_t count, const bchar_t *format, va_list argptr )
{
	return _vsnprintf( (char *) buffer, count, (const char *)format, argptr );
}
#endif

int vswprintf(wchar_t *pDestination, const wchar_t *pFormat, va_list);
int vsnwprintf(wchar_t *pDestination, size_t BufferSize, const wchar_t *pFormat, va_list);
#define _vsnwprintf vsnwprintf

#endif

   

