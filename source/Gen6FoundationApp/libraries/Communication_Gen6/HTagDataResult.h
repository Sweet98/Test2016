//======================================================================================
// HTagDataResult.h
//======================================================================================
// $RCSfile: HTagDataResult.h $
// $Revision: 1.6 $
// $Date: 2011/11/08 18:02:46EST $
//======================================================================================

//======================================================================================



//---------------------------------------------------------------------------
#ifndef HTAG_DATA_RESULT_H
#define HTAG_DATA_RESULT_H
//---------------------------------------------------------------------------


#include	"BitString.h"
#include	"HTag.h"
#include	"IoMan.h"





//---------------------------------------------------------------------------
//
//  Class definitions for HTAG_DECODE_RESULT
//
//---------------------------------------------------------------------------

//
//	The HTAG_DECODE_RESULT class provides a mechanism to transfer decode result messages.  Since all fields are tagged, if the
//	receiver does not yet speak the new version of HTAG_DECODE_RESULT, it will ignore tags that it doesn't understand.
//



class HTAG_DECODE_RESULT : public HTAG
{
public:
	#define	HTAG_DECODE_RESULT_DEFAULT_COMMAND_NAME						"DECRES"
	#define	HTAG_DECODE_RESULT_HOST_TRANSMIT_ACKNOWLEDGE_COMMAND_NAME	"DECACK"

// Status codes
	typedef enum {
		HTAG_DECODE_RESULT_BUFFER_ALLOCATION_FAILURE = HTAG::HTAG_USER_DEFINED_STATUS_BEGIN
	} HTAG_DECODE_RESULT_STATUS;



// Host transmit acknowledge modes.
	typedef enum {
		HTAG_DECODE_RESULT_HOST_TRANSMIT_ACKNOWLEDGE_OFF = 0,
		HTAG_DECODE_RESULT_HOST_TRANSMIT_ACKNOWLEDGE_ON
	} HTAG_DECODE_RESULT_HOST_TRANSMIT_ACKNOWLEDGE_MODES;


// Parameters with names.
	#define	HTAG_DECODE_RESULT_CODE_ID							"CID"
	HSTRING		*HHPCodeID;

	#define	HTAG_DECODE_RESULT_AIM_ID							"AID"
	HSTRING		*AIMID;

	#define	HTAG_DECODE_RESULT_HOST_TRANSMIT_ACKNOWLEDGE		"HOSTACK"
	int			HostTransmitAcknowledge;

	#define	HTAG_DECODE_RESULT_WIDE_CHARACTER_FORMAT			"WCHAR"
	int			WideCharacterFormat;

	#define	HTAG_DECODE_RESULT_INDICATOR_REQUIRED				"INDREQ"
	#define	HTAG_DECODE_RESULT_INDICATOR_REQUIRED_DEFAULT		1
	int			IndicatorRequired;

	#define	HTAG_DECODE_RESULT_SEQUENCE_NUMBER					"SEQNUM"
	#define	HTAG_DECODE_RESULT_SEQUENCE_NUMBER_DEFAULT			0
	int			SequenceNumber;

	#define	HTAG_DECODE_RESULT_INTERMESSAGE_DELAY				"IMD"
	#define	HTAG_DECODE_RESULT_INTERMESSAGE_DELAY_DEFAULT		0
	int			IntermessageDelay;

	#define	HTAG_DECODE_RESULT_GOOD_READ_TIMEOUT				"GRT"
	#define	HTAG_DECODE_RESULT_GOOD_READ_TIMEOUT_DEFAULT		3
	int			GoodReadTimeout;


public:


// Pointer to the actual decode result data.
	#define	DEFAULT_DECODE_RESULT_DATA_ALLOCATION				40
	HSTRING		*Data;




// Constructor for HTAG_DECODE_RESULT.
    HTAG_DECODE_RESULT(const char *Command = HTAG_DECODE_RESULT_DEFAULT_COMMAND_NAME, HCOMM *HCommDevice = 0,
						HTRANSFER_STATUS *TransferStatus = 0, HCOMMAND_HANDLER *CommandHandler = 0);

// Destructor for HTAG_DECODE_RESULT
    ~HTAG_DECODE_RESULT(void);


// Set decode result parameters.
	bool SetDecodeResultParameters( int Protocol, const char *DecodeResultData, int SizeOfDecodeResultData,
			const char *HHPCodeID, const char *AIMID, bool WaitForResponse, bool ReturnDecodeResultHostTransmitAcknowledge,
			bool WideCharacterFormat, int IndicatorRequired = HTAG_DECODE_RESULT_INDICATOR_REQUIRED_DEFAULT,
			int SequenceNumber = HTAG_DECODE_RESULT_SEQUENCE_NUMBER_DEFAULT,
			int IntermessageDelay = HTAG_DECODE_RESULT_INTERMESSAGE_DELAY_DEFAULT,
			int GoodReadTimeout = HTAG_DECODE_RESULT_GOOD_READ_TIMEOUT_DEFAULT );


// Set wchar_t decode result parameters.
	bool SetWideDecodeResultParameters( int Protocol, const wchar_t *DecodeResultData, int SizeOfDecodeResultData,
			const char *HHPCodeID, const char *AIMID, bool WaitForResponse, bool ReturnDecodeResultHostTransmitAcknowledge,
			int IndicatorRequired = HTAG_DECODE_RESULT_INDICATOR_REQUIRED_DEFAULT,
			int SequenceNumber = HTAG_DECODE_RESULT_SEQUENCE_NUMBER_DEFAULT,
			int IntermessageDelay = HTAG_DECODE_RESULT_INTERMESSAGE_DELAY_DEFAULT,
			int GoodReadTimeout = HTAG_DECODE_RESULT_GOOD_READ_TIMEOUT_DEFAULT  );


// Set host transmit acknowledge for decode result parameters.
	bool SetDecodeResultHostTransmitAcknowledgeParameters( int Protocol, bool WaitForResponse,
			int IndicatorRequired = HTAG_DECODE_RESULT_INDICATOR_REQUIRED_DEFAULT,
			int SequenceNumber = HTAG_DECODE_RESULT_SEQUENCE_NUMBER_DEFAULT );

private:
	HTAG_DECODE_RESULT (const HTAG_DECODE_RESULT&);            // declared private, but not defined, to prevent copying         
	HTAG_DECODE_RESULT& operator=(const HTAG_DECODE_RESULT&);  // declared private, but not defined, to prevent copying 

};
//----------------------------------------------------------------------------





//---------------------------------------------------------------------------
//
//  Class definitions for HTAG_SYSTEM_INFORMATION
//
//---------------------------------------------------------------------------

//
//	The HTAG_SYSTEM_INFORMATION class provides a mechanism to obtain information about a scanner or other device.  Since all
//	fields are tagged, if the receiver does not yet speak the new version of HTAG_SYSTEM_INFORMATION, it will ignore tags
//	that it doesn't understand.
//



class HTAG_SYSTEM_INFORMATION : public HTAG
{
public:
	#define	HTAG_GET_SYSTEM_INFORMATION_DEFAULT_COMMAND_NAME			"GETSYS"
	#define	HTAG_SYSTEM_INFORMATION_DEFAULT_COMMAND_NAME				"SYSINF"

// Status codes
	typedef enum {
		HTAG_SYSTEM_INFORMATION_UNKNOWN_DEVICE = HTAG::HTAG_USER_DEFINED_STATUS_BEGIN
	} HTAG_SYSTEM_INFORMATION_STATUS;


// Parameters with names.
	#define	HTAG_SYSTEM_INFORMATION_WORKGROUP_SELECTION							"WG"
	int			WorkgroupSelection;

	#define	HTAG_SYSTEM_INFORMATION_FORCE_CONNECTION							"FC"
	int			ForceConnection;

	#define	HTAG_SYSTEM_INFORMATION_BLUE_TOOTH_MODE								"BTM"
	int			BlueToothMode;

	#define	HTAG_SYSTEM_INFORMATION_IMAGER_X_DIMENSION							"IMGX"
	int			ImagerXDimension;

	#define	HTAG_SYSTEM_INFORMATION_IMAGER_Y_DIMENSION							"IMGY"
	int			ImagerYDimension;

	#define	HTAG_SYSTEM_INFORMATION_IMAGER_DEPTH								"IMGD"
	int			ImagerDepth;

	#define	HTAG_SYSTEM_INFORMATION_DEVICE_MODEL								"MODEL"
	HSTRING		*DeviceModel;

	#define	HTAG_SYSTEM_INFORMATION_PNAME_PROD_CONFIG							"P_PNAME"
	HSTRING		m_Pname_ProdConfig;  // I don't think it's necessary for HSTRING variables to be pointers--this class is always declared at runtime on the heap.  It's simpler to not use a pointer.

	#define	HTAG_SYSTEM_INFORMATION_DEVICE_SERIAL_NUMBER						"SERNUM"
	HSTRING		*DeviceSerialNumber;

	#define	HTAG_SYSTEM_INFORMATION_FIRMWARE_PART_NUMBER						"FPNUM"
	HSTRING		*FirmwarePartNumber;

	#define	HTAG_SYSTEM_INFORMATION_FIRMWARE_REVISION_NUMBER					"FREV"
	HSTRING		*FirmwareRevisionNumber;

	#define	HTAG_SYSTEM_INFORMATION_IMAGE_ENGINE_FAMILY							"EFAM"
	int			ImageEngineFamily;

	#define	HTAG_SYSTEM_INFORMATION_IMAGE_ENGINE_5000_FAMILY_MODEL				"E5MOD"
	int			ImageEngine5000FamilyModel;

	#define	HTAG_SYSTEM_INFORMATION_IMAGE_ENGINE_5000_FAMILY_FIRMWARE_REVISION	"E5FREV"
	int			ImageEngine5000FamilyFirmwareRevision;

	#define	HTAG_SYSTEM_INFORMATION_IMAGE_ENGINE_5000_FAMILY_SERIAL_NUMBER		"E5SER"
	HSTRING		*ImageEngine5000FamilySerialNumber;


public:


// Pointer to the actual decode result data.
	#define	DEFAULT_SYSTEM_INFORMATION_DATA_ALLOCATION				40
	HSTRING		*Data;




// Constructor for HTAG_SYSTEM_INFORMATION.
    HTAG_SYSTEM_INFORMATION(const char *Command = HTAG_SYSTEM_INFORMATION_DEFAULT_COMMAND_NAME, HCOMM *HCommDevice = 0,
						HTRANSFER_STATUS *TransferStatus = 0, HCOMMAND_HANDLER *CommandHandler = 0);

// Destructor for HTAG_SYSTEM_INFORMATION
    ~HTAG_SYSTEM_INFORMATION(void);


// Set system information request parameters.
	bool SetSystemInformationRequestParameters( int Protocol, const char *DeviceAddress,
			int SizeOfDeviceAddress, const char *DeviceName, int SizeOfDeviceName, bool WaitForResponse );


// Set system information parameters.
	bool SetSystemInformationParameters( int Protocol, const char *DeviceAddress,
			int SizeOfDeviceAddress, const char *DeviceName, int SizeOfDeviceName, bool WaitForResponse, int WorkgroupSelection,
			int ImagerXDimension = -1, int ImagerYDimension = -1, int ImagerDepth = -1, const char *Model = "",
			const char *SerialNumber = "", const char *PartNumber = "", const char *RevisionNumber = "",
			int ImageEngineFamily = 0, int ImageEngine5000FamilyModel = 0, int ImageEngine5000FamilyFirmwareRevision = 0,
			const char *ImageEngine5000FamilySerialNumber = "" );

private:
	HTAG_SYSTEM_INFORMATION (const HTAG_SYSTEM_INFORMATION&);            // declared private, but not defined, to prevent copying         
	HTAG_SYSTEM_INFORMATION& operator=(const HTAG_SYSTEM_INFORMATION&);  // declared private, but not defined, to prevent copying 

};
//----------------------------------------------------------------------------





//---------------------------------------------------------------------------
//
//  Class definitions for HTAG_MENU_COMMAND
//
//---------------------------------------------------------------------------

//
//	The HTAG_MENU_COMMAND class provides a mechanism to send a menu or non-menu command and to receive a menu or non-menu
//	command result.  Since all fields are tagged, if the receiver does not yet speak the new version of HTAG_SYSTEM_INFORMATION,
//	it will ignore tags that it doesn't understand.
//



class HTAG_MENU_COMMAND : public HTAG
{
public:
	#define	HTAG_MENU_COMMAND_SEND_MENU_COMMAND						"MNUCMD"
	#define	HTAG_MENU_COMMAND_SEND_CONFIG_MENU_COMMAND				"CFGCMD"
	#define	HTAG_MENU_COMMAND_SEND_NON_MENU_COMMAND					"NMNCMD"
	#define	HTAG_MENU_COMMAND_MENU_COMMAND_RESPONSE					"MNURSP"
	#define	HTAG_MENU_COMMAND_CONFIG_MENU_COMMAND_RESPONSE			"CFGRSP"
	#define	HTAG_MENU_COMMAND_NON_MENU_COMMAND_RESPONSE				"NMNRSP"

// Status codes
//	typedef enum {
//		 = HTAG::HTAG_USER_DEFINED_STATUS_BEGIN
//	} HTAG_MENU_COMMAND_STATUS;


// Parameters with names.
	#define	HTAG_MENU_COMMAND_RESPONSE_REQUIRED						"CMDRSPREQ"
	int			CommandResponseRequired;

	#define	HTAG_MENU_COMMAND_INDICATOR_REQUIRED					"INDREQ"
	int			IndicatorRequired;

	typedef enum {
		ifStd = 0,
		ifRem
	} InterfaceType_t;

	#define	HTAG_PARAM_ORIG_INTERFACE						"OI"
	int		m_OriginatingInterface;								// Originating interface.  Used by RF base to route incoming HTAG out to correct interface.  Note:  at this time, we only see ReM using Menu & NonMenu commands, not image ships.  If we have to add images, this Parameter may need to be moved to the HTAG base class.


public:


// Pointer to the actual decode result data.
	#define	DEFAULT_MENU_COMMAND_DATA_ALLOCATION				40
	HSTRING		*Data;




// Constructor for HTAG_MENU_COMMAND.
    HTAG_MENU_COMMAND(const char *Command = HTAG_MENU_COMMAND_SEND_MENU_COMMAND, HCOMM *HCommDevice = 0,
						HTRANSFER_STATUS *TransferStatus = 0, HCOMMAND_HANDLER *CommandHandler = 0);

// Destructor for HTAG_MENU_COMMAND
    ~HTAG_MENU_COMMAND(void);


// Set menu command parameters.
	bool SetMenuCommandParameters( int Protocol, const char *DeviceName, int SizeOfDeviceName, const char *MenuCommand,
			int SizeOfMenuCommand, bool CommandResponseRequired, bool WaitForResponse, bool IndicatorRequired = false, const CIoMan *  pOrigInterface = g_pStdInterface );


// Set menu command response parameters.
	bool SetMenuCommandResponseParameters( int Protocol, const char *DeviceName, int SizeOfDeviceName,
			const char *MenuCommandResponse, int SizeOfMenuCommandResponse, bool WaitForResponse );


// Set non-menu command parameters.
	bool SetNonMenuCommandParameters( int Protocol, const char *DeviceName, int SizeOfDeviceName,
			const char *NonMenuCommand, int SizeOfNonMenuCommand, bool CommandResponseRequired, bool WaitForResponse );


// Set non-menu command response parameters.
	bool SetNonMenuCommandResponseParameters( int Protocol, const char *DeviceName, int SizeOfDeviceName,
			const char *NonMenuCommandResponse, int SizeOfNonMenuCommandResponse, bool WaitForResponse );

protected:
	int GetIfaceNum(const CIoMan * pIface);
	
private:
	HTAG_MENU_COMMAND (const HTAG_MENU_COMMAND&);            // declared private, but not defined, to prevent copying         
	HTAG_MENU_COMMAND& operator=(const HTAG_MENU_COMMAND&);  // declared private, but not defined, to prevent copying 

};
//----------------------------------------------------------------------------





//---------------------------------------------------------------------------
//
//  Class definitions for HTAG_ACTIVITY_STATUS
//
//---------------------------------------------------------------------------

//
//	The HTAG_ACTIVITY_STATUS class provides a mechanism to send status information on some activity, such as flashing or
//	downloading.  Since all fields are tagged, if the receiver does not yet speak the new version of HTAG_ACTIVITY_STATUS,
//	it will ignore tags that it doesn't understand.
//



class HTAG_ACTIVITY_STATUS : public HTAG
{
public:
	#define	HTAG_ACTIVITY_STATUS_DEFAULT_COMMAND_NAME				"070101"

// Status codes
//	typedef enum {
//		 = HTAG::HTAG_USER_DEFINED_STATUS_BEGIN
//	} HTAG_ACTIVITY_STATUS_STATUS;


// Parameters with names.
	typedef enum {
		HTAG_ACTIVITY_STATUS_FLASHING = 0,
		HTAG_ACTIVITY_STATUS_SENDING_CODE_TO_RF_SCANNER,
		HTAG_ACTIVITY_STATUS_NUMBER_OF_TYPES
	} HTAG_ACTIVITY_STATUS_TYPES;

	#define	HTAG_ACTIVITY_STATUS_TYPE									"ACTIVITY"
	int			ActivityType;

	#define	HTAG_ACTIVITY_STATUS_AMOUNT_OF_DATA_PROCESSED				"PROCESSED"
	int			DataProcessed;

	#define	HTAG_ACTIVITY_STATUS_TOTAL_AMOUNT_OF_DATA_TO_BE_PROCESSED	"TOTAL"
	int			TotalDataToBeProcessed;

	#define	HTAG_ACTIVITY_STATUS_RF_SCANNER								"RFSCAN"
	HSTRING		*RFScanner;


public:


// Pointer to the actual decode result data.
	#define	DEFAULT_ACTIVITY_STATUS_DATA_ALLOCATION				40
	HSTRING		*Data;




// Constructor for HTAG_ACTIVITY_STATUS.
    HTAG_ACTIVITY_STATUS(const char *Command = HTAG_ACTIVITY_STATUS_DEFAULT_COMMAND_NAME, HCOMM *HCommDevice = 0,
						HTRANSFER_STATUS *TransferStatus = 0, HCOMMAND_HANDLER *CommandHandler = 0);

// Destructor for HTAG_ACTIVITY_STATUS
    ~HTAG_ACTIVITY_STATUS(void);

// Set activity status parameters.
	bool SetActivityStatusParameters( int Protocol, int ActivityType, int DataProcessed,
												int TotalDataToBeProcessed, const char *RFScanner );

// Send activity status report.
	bool SendActivityStatus( char *Result, int Protocol, int ActivityType, int DataProcessed,
						int TotalDataToBeProcessed, const char *RFScanner, HCOMM *Device = 0,
						HTRANSFER_STATUS *TransferStatus = 0, HCOMMAND_HANDLER *CommandHandler = 0 );
private:
	HTAG_ACTIVITY_STATUS (const HTAG_ACTIVITY_STATUS&);            // declared private, but not defined, to prevent copying         
	HTAG_ACTIVITY_STATUS& operator=(const HTAG_ACTIVITY_STATUS&);  // declared private, but not defined, to prevent copying 

};
//----------------------------------------------------------------------------




#endif // HTAG_DATA_RESULT_H



