/* ==============================================================================
   USB communication declarations
   ==============================================================================

   $RCSfile: interfaces_usb2/base_usb2/usbdesc.h $
   $Revision: 2.4.4.4 $
   $Date: 2011/03/31 14:35:11EDT $

   ==============================================================================

   ============================================================================== */
#ifndef _USB_DESCRIPTOR_DECLARATIONS_H_
#define _USB_DESCRIPTOR_DECLARATIONS_H_

#include <stdlib.h>
#include "language.h"


/*
*	Standard Descriptor Types
*/
#define USB_DEVICE_DESCR            0x01
#define USB_CONFIG_DESCR            0x02
#define USB_STRING_DESCR            0x03
#define USB_INTERFACE_DESCR         0x04
#define USB_ENDPOINT_DESCR          0x05
#define USB_END_OF_DESCR          	0xFF

/*
*	Class specific descriptor types
*/
#define USB_HID_DESCR               0x21
#define USB_ENTITY_DESCR            0x22
#define USB_REPORT_DESCR            USB_ENTITY_DESCR
#define USB_PHYSICAL_DESCR				0x23
#define USB_HUB_DESCR               0x09
#define USB_CDC_CS_INTERFACE			0x24
#define USB_CDC_CS_ENDPOINT			0x25

/*
*	Feature Selectors
*/
#define USB_ENDPOINT_HALT				0x00
#define USB_DEVICE_REMOTE_WAKEUP		0x01

/*Lengths of various descriptor segments*/
#define USB_DEV_LENGTH                  18      /*length of device desc.  */
#define USB_CFG_LENGTH                  9       /*length of cfg. desc. seg*/
#define USB_INT_LENGTH                  9       /*length of int. desc. seg*/
#define USB_HID_LENGTH                  9       /*length of HID  desc. seg*/
#define USB_END_LENGTH                  7       /*length of end. desc. seg*/

// IAD values
#define IAD_DEV_CLASS		0xef
#define IAD_DEV_SUBCLASS	0x02
#define IAD_DEV_PROTOCOL	0x01

/* Class codes */
#define USB_CDC_CLASS						0x02
#define USB_HID_CLASS						0x03
#define USB_HIDCLASS							USB_HID_CLASS  // old sources

// HID Subclass codes
#define USB_NOSUBCLASS                  0x00
#define USB_HIDBOOTSUBCLASS             0x01
#define USB_VENDORSPEC                  0xFF

// HID Protokoll codes
#define USB_NOPROTOCOL                  0x00
#define USB_BOOTKEYBOARD					 0x01

// CDC Subclass codes
#define USB_CDC_DIRECT_LINE		0x01 // Direct Line Control Model
#define USB_CDC_ACM					0x02 // Abstract Control Model
#define USB_CDC_TELEPHONE			0x03 // Telephone Control Model
#define USB_CDC_MULTI_CHANNEL		0x04 // Multi-Channel Control Model
#define USB_CDC_CAPI					0x05 // CAPI Control Model
#define USB_CDC_ETHERNET			0x06 // Ethernet Networking Control Model
#define USB_CDC_ATM					0x07 // ATM Networking Control Model
#define USB_CDC_FIRST_VENDOR		0x80 // RESERVED (vendor specific) (up to 0xFE)

// CDC Interface class codes
#define USB_CDC_COMMUNICATION_INTERFACE	0x02
#define USB_CDC_DATA_INTERFACE				0x0A

// CDC Protokoll codes
/*
A communication control protocol is used by the USB host to control communication functions in the device or on the
network. This specification defines code values for certain standard control protocols. It also reserves codes for
additional standard or vendor-specific control protocols. If the Communication Class control model does not require a
specific protocol, the value of 00h should be used.
*/
#define USB_CDC_PROT_V25			0x01 // V.25ter Common AT commands (also known as “Hayes™ compatible”)
#define USB_CDC_PROT_VENDOR		0xFF // Vendor-specific


// bDescriptor SubType in Functional Descriptors
#define USB_CDC_SUB_HEADER		0x00 // Header Functional Descriptor, marks the beginning of the set of functional descriptors.
#define USB_CDC_SUB_CALL		0x01 // Call Management Functional Descriptor.
#define USB_CDC_SUB_ACM			0x02 // Abstract Control Management


/*
 *	Valid Configuration Characteristics:
 */
#define USB_CONFIG_POWERSOURCE_MASK	0xc0
#define USB_CONFIG_BUS_POWERED		0x80
#define USB_CONFIG_SELF_POWERED		0xc0
#define USB_CONFIG_REMOTE_WAKEUP		0x20

/* provide some nice automatic helpers */
#if(USB_POWER > 0)
	#define USB_CONFIG_POWER	USB_CONFIG_BUS_POWERED
#else
	#define USB_CONFIG_POWER	USB_CONFIG_SELF_POWERED
#endif

#if(USB_SUPPORTS_REMOTE_WAKEUP == TRUE)
	#define USB_CONFIG_WAKEUP	USB_CONFIG_REMOTE_WAKEUP
#else
	#define USB_CONFIG_WAKEUP	0
#endif

/*
 *	Endpoint Transfer Type (bit 0-1 in bmAttributes
 */
enum TransferType
{
	USB_TT_CONTROL = 0,
	USB_TT_ISOCHRONOUS,
	USB_TT_BULK,
	USB_TT_INTERRUPT
};

#define EP_IN(address)	((address & 0x7) | 0x80)
#define EP_OUT(address)	(address & 0x7)
#define MAXPWR_mA(mA)	((mA >> 1) & 0xff)
//#define PWRON2PWRGOOD(ms) ((ms >> 1) & 0xff)

// Descriptor header
struct Descriptor_t
{
	UCHAR bLength;					// size of descr in bytes
	UCHAR bDescriptorType;		//
};

//	USB Device Descriptor structure
struct DeviceDescriptor_t
{
	UCHAR bLength;					// size of descr in bytes
	UCHAR bDescriptorType;		// USB_DEVICE_DESCR
	UCHAR bcdUSBLow;				// USB spec release number (BCD)
	UCHAR bcdUSBHigh;				// USB spec release number (BCD)
	UCHAR bDeviceClass;			// device class (assigned by USB)
	UCHAR bDeviceSubClass;		// device subclass (assigned by USB)
	UCHAR bDeviceProtocol;		// protocol code (assigned by USB)
	UCHAR wMaxPacketSize0;		// maximum packet size (endpoint zero)
	UCHAR idVendorLow;			// vendor id (USB: 0x0628 for TASKING)
	UCHAR idVendorHigh;			// vendor id (USB: 0x0628 for TASKING)
	UCHAR idProductLow;			// your (manufacturer) product id
	UCHAR idProductHigh;			// your (manufacturer) product id
	UCHAR bcdDeviceLow;			// device release number (BCD format)
	UCHAR bcdDeviceHigh;			// device release number (BCD format)
	UCHAR iManufacturer;			// index string: name of your company
	UCHAR iProduct;				// index string: name of your product
	UCHAR iSerialNumber;			// index string: device serial number
	UCHAR bNumConfigurations;	// number of possible configurations
};

// Configuration Descriptor structure
struct ConfigDescriptor_t
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
};

// Interface Descriptor structure
struct InterfaceDescriptor_t
{
	UCHAR bLength;	   			// size of the descriptor in bytes
	UCHAR bDescriptorType;		// USB_INTERFACE_DESCR
	UCHAR bInterfaceNumber;		// number of the interface
	UCHAR bAlternateSetting;	// select alternate setting
	UCHAR bNumEndpoints;			// number of endpoints used
	UCHAR bInterfaceClass;		// class code (assigned by USB)
	UCHAR bInterfaceSubClass;	// subclass code (assigned by USB)
	UCHAR bInterfaceProtocol;	// protocol code (assigned by USB)
	UCHAR iInterface;				// index string: name of interface
};

// End Point Descriptor structure
struct EndpointDescriptor_t
{
	UCHAR bLength;					// size of the descriptor in bytes
	UCHAR bDescriptorType;		// USB_ENDPOINT_DESCR
	UCHAR bEndpointAddress;		// endpoint address
	UCHAR bmAtrributes;			// configuration characteristics
	UCHAR wMaxPacketSizeLow;	// Maximum packet size
	UCHAR wMaxPacketSizeHigh;	// Maximum packet size
	UCHAR bInterval;				// polling interval
};

// HID.  See USB HID spec version 1.11, section 6.2.1
struct HIDDescriptor1_t
{
	UCHAR bLength;					// size of the descriptor in bytes
	UCHAR bDescriptorType;		// USB_HID_DESCR
	UCHAR bcdHIDLow;				// HID spec release number (BCD)
	UCHAR bcdHIDHigh;				// HID spec release number (BCD)
	UCHAR bCountryCode;			// if the device supports countries
	UCHAR bNumDescriptors;		// Descriptors to follow
	UCHAR bDescriptorType1;		// Types: 21=HID, 22=Report, 23=Physical
	UCHAR bDescriptorLengthLow;	// Report Descriptor length
	UCHAR bDescriptorLengthHigh;	//
	// ...... todo
};

struct IadDescriptor_t
{
	UCHAR bLength;
	UCHAR bDescriptorType;
	UCHAR bFirstInterface;
	UCHAR bInterfaceCount;
	UCHAR bFunctionClass;
	UCHAR bFunctionSubClass;
	UCHAR bFunctionProtocol;
	UCHAR iFunction;
};  // Interface Association Descriptor

// CDC
struct CDC_HeaderDescriptor_t
{
	UCHAR bLength;					// size of the descriptor in bytes
	UCHAR bDescriptorType;		//
	UCHAR bDescriptorSubType;	//
	UCHAR bcdHIDLow;				// CDC spec release number (BCD)
	UCHAR bcdHIDHigh;				// CDC spec release number (BCD)
};

struct CDC_CallManagmentDescriptor_t
{
	UCHAR bLength;					// size of the descriptor in bytes
	UCHAR bDescriptorType;		//
	UCHAR bDescriptorSubType;	//
	UCHAR bmCapabilities;		//
	UCHAR bDataInterface; 		//
};

struct CDC_AC_ManagmentDescriptor_t // Abstract Control Management Functional Descriptor
{
	UCHAR bLength;					// size of the descriptor in bytes
	UCHAR bDescriptorType;		//
	UCHAR bDescriptorSubType;	//
	UCHAR bmCapabilities;		//
};

struct CDC_UnionFunctionalDescriptor_t
{
	UCHAR bLength;					// size of the descriptor in bytes
	UCHAR bDescriptorType;		//
	UCHAR bDescriptorSubType;	//
	UCHAR bMasterInterface;		//
	UCHAR bSlaveInterface0;
};

// Strings
struct USB_StringDescriptorDetails_t
{
	unsigned char ucIndex;
	const unsigned char  *pStringDescriptor;
};

struct USB_LanguageStrings_t
{
	unsigned short usLanguageID;
	const USB_StringDescriptorDetails_t *pStringDetails;
};

struct Usb_DescriptorDetails_t
{
	const UCHAR	*pDescriptor;
	size_t DescriptorSize;
};

struct Usb_DescriptorTable_t
{
	const Usb_DescriptorDetails_t *pDetail;
	int NumberOfDescriptors;
};

struct USB_DescriptorChain_t
{
  	const USB_LanguageStrings_t *pStrings;
	Usb_DescriptorTable_t DeviceDescriptor;
	Usb_DescriptorTable_t ConfigDescriptor;
	Usb_DescriptorTable_t ClassDescriptors;
	Usb_DescriptorTable_t ReportDescriptors;
};

/*
 *	String indexes
 */
enum StringIndex
{
	LANGUAGE_IDX = 0,
	MANUFACTURER_IDX,
	PRODUCT_IDX,
	CONFIGURATION_IDX,
	INTERFACE1_IDX,
	INTERFACE2_IDX,
	REM_INTERFACE_IDX,
	INTERFACE3_IDX,
	SERIAL_IDX,
	INTERFACE_TRIGGER_IDX,	               // the HidPos interface
	FIRST_CUSTOM_INDEX,	// use this as a base for more indexes in your USB personality
	EOT_IDX=0xff
};

#ifdef ARM
#define USBDATA	__align(4)	// ARM needs 4 byte alligned tables for simpler and faster code
#else
#define USBDATA
#endif

void CustomizeUsbDescriptors(const USB_DescriptorChain_t *pChain);
bool AreWeSelfPowered(const USB_DescriptorChain_t *pChain);
void EnableUsbSerialNumber(const USB_DescriptorChain_t *pChain, bool enable);

#endif

/*
 Some usefule struct declarations for dealing with USB descriptors.
 String indexes and constant values are also included.
*/

