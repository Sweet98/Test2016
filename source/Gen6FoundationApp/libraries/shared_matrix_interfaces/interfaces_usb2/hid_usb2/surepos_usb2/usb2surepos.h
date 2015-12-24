/* ==============================================================================
   USB IBM SurePos communication declarations
   ==============================================================================

   $RCSfile: interfaces_usb2/hid_usb2/surepos_usb2/usb2surepos.h $
   $Revision: 1.7.1.2 $
   $Date: 2010/09/01 11:13:27EDT $

//=================================================================================

//=================================================================================*/
//! \file

#ifndef _USB2_SUREPOS_H_COMMUNICATION_
#define _USB2_SUREPOS_H_COMMUNICATION_

extern int IBM4690SurePosMaxPacketSize;

#define SYSTEM_COMMAND					0x00                			//Flash Commands
#define START_FLASH						0x01
#define WRITE_FLASH_RECORD				0x02
#define SCANNER_ENABLE              0x11                      //Scanner Commands
#define SCANNER_DISABLE					0x12
#define BEEPER_ENABLE					0x14
#define BEEPER_DISABLE					0x18
#define CONFIGURE_SCANNER 				0x20
#define REPORT_CONFIGURATION			0x21
#define CONFIGURE_TWO_LABEL			0x23
#define REPORT_TWO_LABEL				0x34
#define DIRECT_IO							0x30

#define NO_COMMAND						0xff	// used as illegal command

															//Command Set
/* byte 2 part of system_command*/           //Command Byte #2
#define SCANNER_COMMANDS            0x00                       //Scanner Commands
#define TEST								0x10                       //Flash and Scanner Commands
#define STATUS_REQUEST					0x20
#define RESET_SCANNER					0x40

															//Response Format Set  -  order:  Bits 0 to 7  (least significant to most)
/* status */                                 //Status Byte 0
#define OST0_FLASH_IN_PROGRESS			0x01
#define OST0_CONFIG_RESPONSE				0x02
#define OST0_TWO_LABEL_RESPONSE			0x04
#define OST0_RESERVED						0x08
#define OST0_GOOD_READ_BEEP_ENABLED		0x10
#define OST0_HARDWARE_ERROR				0x20
#define OST0_DIRECT_IO_RESPONSE			0x40
#define OST0_NOT_READY						0x80
															//Status Byte 1
#define OST1_SCANNER_ALIVE					0x01
#define OST1_SCANNER_ENABLED				0x02
#define OST1_LENGHT_CHECKDIGIT_ERROR	0x04
#define OST1_RESERVED1						0x08
#define OST1_RESERVED2						0x10
#define OST1_RESERVED3						0x20
#define OST1_RESERVED4						0x40
#define OST1_COMMAND_REJECT				0x80
															//Status Byte 2
#define OST2_CONFIG_SUCCESFUL				0x01
#define OST2_CONFIG_COERCED				0x02
#define OST2_TWO_LABEL_SUCCESFUL			0x04
#define OST2_DIRECT_IO_ACCEPTED			0x08
#define OST2_DIRECT_IO_NOT_ALLOWED		0x10
#define OST2_DIRECT_IO_UNDEFINED			0x20
#define OST2_RESERVED1						0x40
#define OST2_RESERVED2						0x80

															//Firmware Flash Status Report  -  Order  Bit 0 to 7
/* Flash status */                           //status byte #1
#define FST0_DEFAULT							0x00
#define FST0_COMMAND_COMPLETE				0x01              //bit 0
#define FST0_FLASH_IN_PROGRESS			0x02
#define FST0_RESERVED1						0x04
#define FST0_RESERVED2						0x08
#define FST0_RESERVED3						0x10
#define FST0_RESERVED4						0x20
#define FST0_OPERATION_IN_ERROR			0x40
#define FST0_COMMAND_REJECT				0x80              //bit 7

#define HID_SURE_POS_IN_REPORT_SIZE   64
#define HID_SURE_POS_LABEL_SIZE       HID_SURE_POS_IN_REPORT_SIZE-4
#define HID_SURE_POS_REPORT_UCLABEL_SIZE 256-4

struct SurePosInReportBuffer_t
{
public:
	void Clear(void)						{ memset(GetArray(), 0, GetBufferSize());	}
	void ClearData(void)					{ memset(u.report.ucLabel, 0, (IBM4690SurePosMaxPacketSize-4));	}
	void CopyData(const UCHAR *pSrc, size_t Length)		{ memcpy(u.report.ucLabel, pSrc, Length);	}

	UCHAR	*GetDataPointer(void)		{ return u.report.ucLabel;						}
	void SetLength(UCHAR size)			{ u.report.ucLength = size;					}
	UCHAR GetLength(void) const		{ return u.report.ucLength;					}

	void AddByte(UCHAR nByte)
	{
		u.report.ucLabel[u.report.ucLength] = nByte;
		u.report.ucLength++;
	}

	UCHAR	*GetArray(void)				{ return u.bytes.ucArray;						}
	size_t GetBufferSize(void)	const	{ return IBM4690SurePosMaxPacketSize;		}

	union
	{
		struct
		{
			UCHAR ucArray[256];
		}bytes;
		struct
		{
			UCHAR ucLength;  		/* Label length */
			UCHAR ucStatus0;		/* Status byte */
			UCHAR ucStatus1;		/* Status byte */
			UCHAR ucStatus2;		/* Status byte */
			UCHAR ucLabel[HID_SURE_POS_REPORT_UCLABEL_SIZE];  	/* Label contents */
		}report;
	}u;
};

#define HID_SURE_POS_OUT_REPORT_SIZE 11

struct SurePosOutReportBuffer_t                        //Scanner Command from Host
{
	union
	{
		struct
		{
			UCHAR ucArray[HID_SURE_POS_OUT_REPORT_SIZE];
		}bytes;
		struct
		{
			UCHAR Cmd1;		  		/* */
			UCHAR Cmd2;		  		/* */
			UCHAR ucData[HID_SURE_POS_OUT_REPORT_SIZE-2];  	/* Response contents */
		}report;
	}u;
};

struct SurePosResponseReportBuffer_t
{
	union
	{
		struct
		{
			UCHAR ucArray[HID_SURE_POS_OUT_REPORT_SIZE];
		}bytes;
		struct
		{
			UCHAR ucLength;  		/* Response length */
			UCHAR ucStatus0;		/* Status byte */
			UCHAR ucStatus1;		/* Status byte */
			UCHAR ucStatus2;		/* Status byte */
			UCHAR ucData[HID_SURE_POS_OUT_REPORT_SIZE-4];  	/* Response contents */
		}report;
	}u;
};

#define HID_SURE_POS_FLASH_OUT_REPORT_SIZE 263
#define HID_SURE_POS_FLASH_OUT_DATA_SIZE   255

struct SurePosFlashOutReportBuffer_t
{
	union
	{
		struct
		{
			UCHAR ucArray[HID_SURE_POS_FLASH_OUT_REPORT_SIZE];
		}bytes;
		struct
		{
			UCHAR Dummy1;	  		/* */
			UCHAR Dummy2;	  		/* */
			UCHAR Cmd1;		  		/* */
			UCHAR Cmd2;		  		/* */
			UCHAR DataCount;		/* Data length */
			UCHAR FixedField[3];	/* */
			UCHAR Data[HID_SURE_POS_FLASH_OUT_DATA_SIZE];  	/* Response contents */
		}report;
	}u;
};

#define HID_SURE_POS_FLASH_IN_REPORT_SIZE 2

struct SurePosFlashResponseReportBuffer_t
{
	union
	{
		struct
		{
			UCHAR ucArray[HID_SURE_POS_FLASH_IN_REPORT_SIZE];
		}bytes;
		struct
		{
			UCHAR ucStatus0;		/* Status byte */
			UCHAR ucStatus1;		/* Status byte */
		}report;
	}u;
};


#define ConfigSize 9
#define DirectIoConfigSize	1
#define TWO_LABEL_CONFIG_SIZE 8

#define VendorId1		0
#define VendorId2		7                //HHP vendor ID for IBM SurePos spec ver 2.0

#define MORE_DATA_CONTINUATION_BYTE		0x10       //used by label data to indicate if there is more than one data packet  (zero byte, see comment below)
#define LAST_DATA_CONTINUATION_BYTE		0x00

#define PDF_FIRST_DATA_CONTINUATION_BYTE		0x00
#define ID_BYTE_ONE_PDF_NEW4690OS			0x0A

#define ID_BYTE_ONE_PDF						0x2E       //symbology label type identifier per IBM Spec.  (3 bytes total including continuation byte)

#define ID_BYTE_ONE_MAXICODE				0x2F

#define ID_BYTE_ONE_RSS_EXPANDED			0x2B

#define ID_BYTE_ONE_UNKNOWN					0xFF

#define ID_BYTE_TWO_ALL						0x0B

#if(0)
#define SurePosTempBufferSize 500

struct SurePos_Scanner_Status_t
{                               	/*  Bit  Access */
//		UINT BufferScans		:1;	/*    Bit 7     */
		UINT MenuResponse		:1;	/*    Bit 6     */
//		UINT Waiting4DataAck	:1;	/*    Bit 5     */
//		UINT ScannerDisabled	:1;	/*    Bit 4     */
//		UINT BeeperEnabled		:1;	/*    Bit 4     */
//		UCHAR 						:1;	/*    Bit 3     */
//		UCHAR 						:1;	/*    Bit 2     */
//		UCHAR 						:1;	/*    Bit 1     */
//		UCHAR 						:1;	/*    Bit 0     */
};
#endif

#define MAX_SUREPOS_OUT_REPORT_SIZE (HID_SURE_POS_FLASH_OUT_REPORT_SIZE+1)
#define MAX_SUREPOS_IN_REPORT_SIZE  (256+1)

#endif // _USB2_SUREPOS_H_COMMUNICATION_


