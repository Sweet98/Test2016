/* ==============================================================================
   USB HID for HID POS Module header
   ==============================================================================

   $RCSfile: interfaces_usb2/hid_usb2/pos_usb2/usb2pos.h $
   $Revision: 1.5 $
   $Date: 2011/03/31 14:39:47EDT $

//=================================================================================

//=================================================================================*/
#include <string.h>


#ifndef USB2_POS_DEFINITIONS_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define USB2_POS_DEFINITIONS_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

/* HidPos Report IDs also used in comon stuff */
#define HID_POS_FEATURE_EEPROM_REPORT_ID	255
#define HID_POS_REDIRECT_REPORT_ID			254  // redirect the scanners output
#define HID_POS_OUT_1252_REPORT_ID			253  // sending data to the scanner
#define HID_POS_OUT_TRIGGER					4

#define HID_POS_FEATURE_REDIRECT_SIZE 2
#define HID_POS_OUT_REPORT_SIZE 64

#define EZUSB_CONFIG_SIZE	32
#define EZUSB_CONFIG_REPORT_SIZE	EZUSB_CONFIG_SIZE+4+1

#define HID_OUT_1252_SIZE	(HID_POS_OUT_REPORT_SIZE-1-1)

struct UsbCommonHidReportBuffer_t
{
public:
	void Clear(void)					{ memset(GetArray(), 0, sizeof(UsbCommonHidReportBuffer_t));	}
	UCHAR	*GetArray(void)			{ return u.bytes.ucArray;		}

	UCHAR	GetReportId(void) const	{ return u.id.ucReportId;		}
	void SetReportId(UCHAR id)		{ u.id.ucReportId = id;			}

	UCHAR	GetTriggerCmd(void) const	{ return u.triggerreport.ucFlags;				}
	UCHAR	GetInterface(void)  const	{ return u.feature_redirect.ucInterface;		}
	void	SetInterface(UCHAR value)	{ u.feature_redirect.ucInterface = value;		}
	size_t GetFeatureRedirectSize(void) const	{ return HID_POS_FEATURE_REDIRECT_SIZE;	}
	UCHAR	*GetContentBuffer(void)		{ return u.outreport.ucContent;		}
	UCHAR	GetContentLength(void) 		{ return u.outreport.ucLength;		}
protected:

	union
	{
		struct
		{
			UCHAR ucArray[HID_POS_OUT_REPORT_SIZE];
		}bytes;
		struct
		{
			UCHAR ucReportId;  									/* HID ID */
		}id;
		struct
		{
			UCHAR ucReportId;  									/* HID ID */
			UCHAR ucFlags; 											/* Response contents */
		}triggerreport;
		struct
		{
			UCHAR ucReportId;  									/* HID ID */
			UCHAR ucInterface;										/* interface for redirection */
		}feature_redirect;
		struct
		{
			UCHAR ucReportId;  									/* HID ID */
			UCHAR ucLength;  										/* HID size */
			UCHAR ucContent[HID_OUT_1252_SIZE];				/* contents */
		}outreport;
	}u;
};


#define HID_USBPOS_IN_REPORT_ID					2

#define HID_USBPOS_IN_REPORT_SIZE 64
#define HID_USBPOS_LABEL_SIZE (HID_USBPOS_IN_REPORT_SIZE-1-1-1-3-2)

#define HID_USBPOS_FEATURE_EEPROM_REPORT_SIZE EZUSB_CONFIG_SIZE+1

struct UsbPosInReportBuffer_t
{
public:
	void Clear(void)						{ memset(GetArray(), 0, GetBufferSize());	}

	UCHAR	GetReportId(void) const		{ return u.id.ucReportId;		}
	void SetReportId(UCHAR id)			{ u.id.ucReportId = id;			}

	void SetSize(UCHAR size)			{ u.report.ucDataSize = size;	}

	void SetAimStart(UCHAR val)		{ u.report.ucAimStart = val;		}
	void SetAimId(UCHAR val)			{ u.report.ucAimID = val;			}
	void SetAimModifier(UCHAR val)	{ u.report.ucAimModifier = val;	}
	void SetCodeID(UCHAR val)			{ u.report.ucCodeID = val;			}
	void SetCodeModifier(UCHAR val)	{ u.report.ucCodeModifier = val;	}
	void SetPadding(UCHAR val)			{ u.report.ucPadding = val;		}

	UCHAR	*GetArray(void)				{ return u.bytes.ucArray;				}
	size_t GetBufferSize(void)	const	{ return HID_USBPOS_IN_REPORT_SIZE;	}

	UCHAR	*GetContentBuffer(void)		{ return u.report.ucLabel;				}

protected:
	union
	{
		struct
		{
			UCHAR ucArray[HID_USBPOS_IN_REPORT_SIZE];
		}bytes;
		struct
		{
			UCHAR ucReportId;  									/* HID ID */
		}id;
		struct
		{
			UCHAR ucReportId;  									/* HID ID */
			UCHAR ucDataSize;
			UCHAR ucAimStart;
			UCHAR ucAimID;
			UCHAR ucAimModifier;
			UCHAR ucLabel[HID_USBPOS_LABEL_SIZE];  			/* Label contents */
			UCHAR ucCodeID;
			UCHAR ucCodeModifier;
			UCHAR ucPadding;
		}report;
		struct
		{
			UCHAR ucReportId;  									/* HID ID */
			UCHAR ucContents[EZUSB_CONFIG_REPORT_SIZE];	/* EEPROM */
		}feature_read_ee;
	}u;

};


#define HID_USBPOS_OUT_REPORT_SIZE 4

#define HID_USBPOS_OUT_ARRAY_SIZE HID_USBPOS_OUT_REPORT_SIZE
struct UsbPosOutReportBuffer_t
{
	union
	{
		struct
		{
			UCHAR ucArray[HID_USBPOS_OUT_REPORT_SIZE];
		}bytes;
		struct
		{
			UCHAR ucReportId;  									/* HID ID */
		}id;
		struct
		{
			UCHAR ucReportId;  									/* HID ID */
			UCHAR ucData[HID_USBPOS_OUT_REPORT_SIZE-1];  	/* Response contents */
		}report;
		struct
		{
			UCHAR ucReportId;  									/* HID ID */
			UCHAR ucFlags; 											/* Response contents */
		}triggerreport;
		struct
		{
			UCHAR ucReportId;  									/* HID ID */
			UCHAR ucContents[EZUSB_CONFIG_REPORT_SIZE];	/* EEPROM */
		}feature_read_ee;
	}u;
};


#endif	/* USB2_POS_DEFINITIONS_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */





