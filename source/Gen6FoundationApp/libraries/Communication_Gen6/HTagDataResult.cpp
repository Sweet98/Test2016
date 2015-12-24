//======================================================================================
// HTagDataResult.cpp
//======================================================================================
// $RCSfile: HTagDataResult.cpp $
// $Revision: 1.4 $
// $Date: 2011/06/07 13:46:49EDT $
//======================================================================================

//======================================================================================


#include	"HHPCommunicationConfig.h"

#include	"HTagDataResult.h"		// HTagDataResult interface definition

#include	<stdlib.h>		/* standard library functions				*/
#include	<stdio.h>
#include	<string.h>

//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//
//  Class Name:     HTAG_DECODE_RESULT
//
//  Class Description:
//		The HTAG_DECODE_RESULT class provides a mechanism to transfer decode result messages.  Since all fields are tagged,
//		if the receiver does not yet speak the new version of HTAG_DECODE_RESULT, it will ignore tags that it doesn't
//		understand.
//
//---------------------------------------------------------------------------







//
// Constructor for HTAG_DECODE_RESULT
//
HTAG_DECODE_RESULT::HTAG_DECODE_RESULT(const char *Command, HCOMM *HCommDevice, HTRANSFER_STATUS *TransferStatus,
								HCOMMAND_HANDLER *CommandHandler)
		: HTAG( HCommDevice, TransferStatus, CommandHandler, false )
{
	Data = new BITSTRING(DEFAULT_DECODE_RESULT_DATA_ALLOCATION, true);
	HTAG::Data = Data;				// Give the pointer to the base class so it can operate on the data as well.
	CommandName->Copy(Command);


// Parameters with names.
	HHPCodeID = new HSTRING(20, true);
	AddHStringParameter( HTAG_DECODE_RESULT_CODE_ID, HHPCodeID, "", 0, false );

	AIMID = new HSTRING(20, true);
	AddHStringParameter( HTAG_DECODE_RESULT_AIM_ID, AIMID, "", 0, false );

	AddNumericParameter( HTAG_DECODE_RESULT_HOST_TRANSMIT_ACKNOWLEDGE, &HostTransmitAcknowledge,
					HTAG_DECODE_RESULT_HOST_TRANSMIT_ACKNOWLEDGE_OFF,
					HTAG_DECODE_RESULT_HOST_TRANSMIT_ACKNOWLEDGE_OFF, HTAG_DECODE_RESULT_HOST_TRANSMIT_ACKNOWLEDGE_ON, false );

	AddNumericParameter( HTAG_DECODE_RESULT_WIDE_CHARACTER_FORMAT, &WideCharacterFormat, 0, 0, 1, true );

	AddNumericParameter( HTAG_DECODE_RESULT_INDICATOR_REQUIRED, &IndicatorRequired,
					HTAG_DECODE_RESULT_INDICATOR_REQUIRED_DEFAULT, 0, 1, false );

	AddNumericParameter( HTAG_DECODE_RESULT_SEQUENCE_NUMBER, &SequenceNumber, HTAG_DECODE_RESULT_SEQUENCE_NUMBER_DEFAULT, false );

	AddNumericParameter( HTAG_DECODE_RESULT_INTERMESSAGE_DELAY, &IntermessageDelay,
					HTAG_DECODE_RESULT_INTERMESSAGE_DELAY_DEFAULT, false );

	AddNumericParameter( HTAG_DECODE_RESULT_GOOD_READ_TIMEOUT, &GoodReadTimeout,
					HTAG_DECODE_RESULT_GOOD_READ_TIMEOUT_DEFAULT, false );

    return;
}
//---------------------------------------------------------------------------





//
// Destructor for HTAG_DECODE_RESULT
//      If memory has been allocated, release it.
//
HTAG_DECODE_RESULT::~HTAG_DECODE_RESULT(void)
{
	delete Data;
	delete HHPCodeID;
	delete AIMID;

    return;
}





//
//	SetDecodeResultParameters
//		Set decode result parameters.
//
bool HTAG_DECODE_RESULT::SetDecodeResultParameters( int Protocol, const char *DecodeResultData,
			int SizeOfDecodeResultData, const char *HHPCodeID, const char *AIMID, bool WaitForResponse,
			bool ReturnDecodeResultHostTransmitAcknowledge, bool WideCharacterFormat, int IndicatorRequired,
			int SequenceNumber, int IntermessageDelay, int GoodReadTimeout )
{
	bool		Status = true;			// Assume success for now.


	Data->Copy( DecodeResultData, SizeOfDecodeResultData );

	UpdateParameter( HTAG_PROTOCOL_PARAMETER_NAME, Protocol );
	UpdateParameter( HTAG_DECODE_RESULT_CODE_ID, HHPCodeID );
	UpdateParameter( HTAG_DECODE_RESULT_AIM_ID, AIMID );
	UpdateParameter( HTAG_RESPONSE_PARAMETER_NAME, WaitForResponse ? 1 : 0 );
	UpdateParameter( HTAG_DECODE_RESULT_HOST_TRANSMIT_ACKNOWLEDGE, ReturnDecodeResultHostTransmitAcknowledge ? 
						HTAG_DECODE_RESULT::HTAG_DECODE_RESULT_HOST_TRANSMIT_ACKNOWLEDGE_ON :
						HTAG_DECODE_RESULT::HTAG_DECODE_RESULT_HOST_TRANSMIT_ACKNOWLEDGE_OFF );
	UpdateParameter( HTAG_DECODE_RESULT_WIDE_CHARACTER_FORMAT, WideCharacterFormat ? 1 : 0 );
	UpdateParameter( HTAG_DECODE_RESULT_INDICATOR_REQUIRED, IndicatorRequired );
	UpdateParameter( HTAG_DECODE_RESULT_SEQUENCE_NUMBER, SequenceNumber );
	UpdateParameter( HTAG_DECODE_RESULT_INTERMESSAGE_DELAY, IntermessageDelay );
	UpdateParameter( HTAG_DECODE_RESULT_GOOD_READ_TIMEOUT, GoodReadTimeout );

	return(Status);
} // SetDecodeResultParameters





//
//	SetWideDecodeResultParameters
//		Set wchar_t decode result parameters.
//
bool HTAG_DECODE_RESULT::SetWideDecodeResultParameters( int Protocol, const wchar_t *DecodeResultData,
			int SizeOfDecodeResultData, const char *HHPCodeID, const char *AIMID, bool WaitForResponse,
			bool ReturnDecodeResultHostTransmitAcknowledge, int IndicatorRequired, int SequenceNumber,
			int IntermessageDelay, int GoodReadTimeout )
{
	HWSTRING	hwstrDecodeResult( SizeOfDecodeResultData, true );
	HSTRING		hstrDecodeResult( SizeOfDecodeResultData * 2, true );
	hwstrDecodeResult.Copy( DecodeResultData, SizeOfDecodeResultData );
	hwstrDecodeResult.CopyToHString( &hstrDecodeResult );

	return SetDecodeResultParameters( Protocol, (char *) hstrDecodeResult.Char, hstrDecodeResult.Size, HHPCodeID, AIMID,
			WaitForResponse, ReturnDecodeResultHostTransmitAcknowledge, true, IndicatorRequired, SequenceNumber,
			IntermessageDelay, GoodReadTimeout );
} // SetWideDecodeResultParameters





//
//	SetDecodeResultHostTransmitAcknowledgeParameters
//		Set host transmit acknowledge for decode result parameters.
//
bool HTAG_DECODE_RESULT::SetDecodeResultHostTransmitAcknowledgeParameters( int Protocol, bool WaitForResponse,
			int IndicatorRequired, int SequenceNumber )
{
	bool		Status = true;			// Assume success for now.


	UpdateParameter( HTAG_PROTOCOL_PARAMETER_NAME, Protocol );
	UpdateParameter( HTAG_RESPONSE_PARAMETER_NAME, WaitForResponse ? 1 : 0 );
	UpdateParameter( HTAG_DECODE_RESULT_INDICATOR_REQUIRED, IndicatorRequired );
	UpdateParameter( HTAG_DECODE_RESULT_SEQUENCE_NUMBER, SequenceNumber );

	return(Status);
} // SetDecodeResultHostTransmitAcknowledgeParameters
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
//
//  Class Name:     HTAG_SYSTEM_INFORMATION
//
//  Class Description:
//		The HTAG_SYSTEM_INFORMATION class provides a mechanism to obtain information about a scanner or other device.  Since all
//		fields are tagged, if the receiver does not yet speak the new version of HTAG_SYSTEM_INFORMATION, it will ignore tags
//		that it doesn't understand.
//
//---------------------------------------------------------------------------







//
// Constructor for HTAG_SYSTEM_INFORMATION
//
HTAG_SYSTEM_INFORMATION::HTAG_SYSTEM_INFORMATION(const char *Command, HCOMM *HCommDevice, HTRANSFER_STATUS *TransferStatus,
								HCOMMAND_HANDLER *CommandHandler)
		: HTAG( HCommDevice, TransferStatus, CommandHandler, false )
{
	Data = new BITSTRING( DEFAULT_DECODE_RESULT_DATA_ALLOCATION, true );
	HTAG::Data = Data;				// Give the pointer to the base class so it can operate on the data as well.
	CommandName->Copy(Command);

	AddNumericParameter( HTAG_SYSTEM_INFORMATION_WORKGROUP_SELECTION, &WorkgroupSelection, 0, 0, 0x7FFFFFFF, false );

	AddNumericParameter( HTAG_SYSTEM_INFORMATION_FORCE_CONNECTION, &ForceConnection, 0, 0, 3, false );

	AddNumericParameter( HTAG_SYSTEM_INFORMATION_BLUE_TOOTH_MODE, &BlueToothMode, -1, false );

	AddNumericParameter( HTAG_SYSTEM_INFORMATION_IMAGER_X_DIMENSION, &ImagerXDimension, -1, false );

	AddNumericParameter( HTAG_SYSTEM_INFORMATION_IMAGER_Y_DIMENSION, &ImagerYDimension, -1, false );

	AddNumericParameter( HTAG_SYSTEM_INFORMATION_IMAGER_DEPTH, &ImagerDepth, -1, false );

	DeviceModel = new HSTRING(20, true);
	AddHStringParameter( HTAG_SYSTEM_INFORMATION_DEVICE_MODEL, DeviceModel, "", 0, false );

	AddHStringParameter( HTAG_SYSTEM_INFORMATION_PNAME_PROD_CONFIG, &m_Pname_ProdConfig, "", 0, false );

	DeviceSerialNumber = new HSTRING(20, true);
	AddHStringParameter( HTAG_SYSTEM_INFORMATION_DEVICE_SERIAL_NUMBER, DeviceSerialNumber, "", 0, false );

	FirmwarePartNumber = new HSTRING(20, true);
	AddHStringParameter( HTAG_SYSTEM_INFORMATION_FIRMWARE_PART_NUMBER, FirmwarePartNumber, "", 0, false );

	FirmwareRevisionNumber = new HSTRING(20, true);
	AddHStringParameter( HTAG_SYSTEM_INFORMATION_FIRMWARE_REVISION_NUMBER, FirmwareRevisionNumber, "", 0, false );

	AddNumericParameter( HTAG_SYSTEM_INFORMATION_IMAGE_ENGINE_FAMILY, &ImageEngineFamily, 0, false );

	AddNumericParameter( HTAG_SYSTEM_INFORMATION_IMAGE_ENGINE_5000_FAMILY_MODEL, &ImageEngine5000FamilyModel, 0, false );

	AddNumericParameter( HTAG_SYSTEM_INFORMATION_IMAGE_ENGINE_5000_FAMILY_FIRMWARE_REVISION,
					&ImageEngine5000FamilyFirmwareRevision, 0, false );

	ImageEngine5000FamilySerialNumber = new HSTRING(20, true);
	AddHStringParameter( HTAG_SYSTEM_INFORMATION_IMAGE_ENGINE_5000_FAMILY_SERIAL_NUMBER,
					ImageEngine5000FamilySerialNumber, "", 0, false );

    return;
}
//---------------------------------------------------------------------------





//
// Destructor for HTAG_SYSTEM_INFORMATION
//      If memory has been allocated, release it.
//
HTAG_SYSTEM_INFORMATION::~HTAG_SYSTEM_INFORMATION(void)
{
	delete Data;
	delete DeviceModel;
	delete DeviceSerialNumber;
	delete FirmwarePartNumber;
	delete FirmwareRevisionNumber;
	delete ImageEngine5000FamilySerialNumber;

    return;
}





//
//	SetSystemInformationRequestParameters
//		Set system information request parameters.
//
bool HTAG_SYSTEM_INFORMATION::SetSystemInformationRequestParameters( int Protocol, const char *DeviceAddress,
			int SizeOfDeviceAddress, const char *DeviceName, int SizeOfDeviceName, bool WaitForResponse )
{
	bool		Status = true;			// Assume success for now.


	UpdateParameter( HTAG_PROTOCOL_PARAMETER_NAME, Protocol );
	UpdateParameter( HTAG_DEVICE_ADDRESS_PARAMETER_NAME, DeviceAddress, SizeOfDeviceAddress );
	UpdateParameter( HTAG_DEVICE_NAME_PARAMETER_NAME, DeviceName, SizeOfDeviceName );
	UpdateParameter( HTAG_RESPONSE_PARAMETER_NAME, WaitForResponse ? 1 : 0 );

	return(Status);
} // SetSystemInformationRequestParameters





//
//	SetSystemInformationParameters
//		Set system information parameters.
//
bool HTAG_SYSTEM_INFORMATION::SetSystemInformationParameters( int Protocol, const char *DeviceAddress,
			int SizeOfDeviceAddress, const char *DeviceName, int SizeOfDeviceName, bool WaitForResponse, int WorkgroupSelection,
			int ImagerXDimension, int ImagerYDimension, int ImagerDepth, const char *Model, const char *SerialNumber,
			const char *PartNumber, const char *RevisionNumber, int ImageEngineFamily, int ImageEngine5000FamilyModel,
			int ImageEngine5000FamilyFirmwareRevision, const char *ImageEngine5000FamilySerialNumber )
{
	bool		Status = true;			// Assume success for now.


	UpdateParameter( HTAG_PROTOCOL_PARAMETER_NAME, Protocol );
	UpdateParameter( HTAG_DEVICE_ADDRESS_PARAMETER_NAME, DeviceAddress, SizeOfDeviceAddress );
	UpdateParameter( HTAG_DEVICE_NAME_PARAMETER_NAME, DeviceName, SizeOfDeviceName );
	UpdateParameter( HTAG_RESPONSE_PARAMETER_NAME, WaitForResponse ? 1 : 0 );
	UpdateParameter( HTAG_SYSTEM_INFORMATION_WORKGROUP_SELECTION, WorkgroupSelection );
	UpdateParameter( HTAG_SYSTEM_INFORMATION_IMAGER_X_DIMENSION, ImagerXDimension );
	UpdateParameter( HTAG_SYSTEM_INFORMATION_IMAGER_Y_DIMENSION, ImagerYDimension );
	UpdateParameter( HTAG_SYSTEM_INFORMATION_IMAGER_DEPTH, ImagerDepth );
	UpdateParameter( HTAG_SYSTEM_INFORMATION_DEVICE_MODEL, Model );
	UpdateParameter( HTAG_SYSTEM_INFORMATION_DEVICE_SERIAL_NUMBER, SerialNumber );
	UpdateParameter( HTAG_SYSTEM_INFORMATION_FIRMWARE_PART_NUMBER, PartNumber );
	UpdateParameter( HTAG_SYSTEM_INFORMATION_FIRMWARE_REVISION_NUMBER, RevisionNumber );
	UpdateParameter( HTAG_SYSTEM_INFORMATION_IMAGE_ENGINE_FAMILY, ImageEngineFamily );
	UpdateParameter( HTAG_SYSTEM_INFORMATION_IMAGE_ENGINE_5000_FAMILY_MODEL, ImageEngine5000FamilyModel );
	UpdateParameter( HTAG_SYSTEM_INFORMATION_IMAGE_ENGINE_5000_FAMILY_FIRMWARE_REVISION, ImageEngine5000FamilyFirmwareRevision );
	UpdateParameter( HTAG_SYSTEM_INFORMATION_IMAGE_ENGINE_5000_FAMILY_SERIAL_NUMBER, ImageEngine5000FamilySerialNumber );

	return(Status);
} // SetSystemInformationParameters
//---------------------------------------------------------------------------









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
//---------------------------------------------------------------------------







//
// Constructor for HTAG_MENU_COMMAND
//
HTAG_MENU_COMMAND::HTAG_MENU_COMMAND(const char *Command, HCOMM *HCommDevice, HTRANSFER_STATUS *TransferStatus,
								HCOMMAND_HANDLER *CommandHandler)
		: HTAG( HCommDevice, TransferStatus, CommandHandler, false )
{
	Data = new BITSTRING( DEFAULT_MENU_COMMAND_DATA_ALLOCATION, true );
	HTAG::Data = Data;				// Give the pointer to the base class so it can operate on the data as well.
	CommandName->Copy(Command);

	// Klocwork will warn about unitialized variables getting used, but these methods first  store the address of the variable and then actually do init them (using that pointer and with the default value).
	AddNumericParameter( HTAG_MENU_COMMAND_RESPONSE_REQUIRED, &CommandResponseRequired, 0, 0, 1, false );
	AddNumericParameter( HTAG_MENU_COMMAND_INDICATOR_REQUIRED, &IndicatorRequired, 0, 0, 1, false );
	AddNumericParameter( HTAG_PARAM_ORIG_INTERFACE, &m_OriginatingInterface, 0, false );

    return;
}
//---------------------------------------------------------------------------





//
// Destructor for HTAG_MENU_COMMAND
//      If memory has been allocated, release it.
//
HTAG_MENU_COMMAND::~HTAG_MENU_COMMAND(void)
{
	delete Data;

    return;
}





//
//	SetMenuCommandParameters
//		Set menu command parameters.
//
bool HTAG_MENU_COMMAND::SetMenuCommandParameters( int Protocol, const char *DeviceName, int SizeOfDeviceName,
			const char *MenuCommand, int SizeOfMenuCommand, bool CommandResponseRequired, bool WaitForResponse,
			bool IndicatorRequired, const CIoMan *  pOrigInterface /*= g_pStdInterface*/ )
{
	bool		Status = true;			// Assume success for now.


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

	UpdateParameter( HTAG_PROTOCOL_PARAMETER_NAME, Protocol );
	UpdateParameter( HTAG_DEVICE_NAME_PARAMETER_NAME, DeviceName, SizeOfDeviceName );
	UpdateParameter( HTAG_RESPONSE_PARAMETER_NAME, WaitForResponse ? 1 : 0 );
	UpdateParameter( HTAG_MENU_COMMAND_RESPONSE_REQUIRED, CommandResponseRequired ? 1 : 0 );
	UpdateParameter( HTAG_MENU_COMMAND_INDICATOR_REQUIRED, IndicatorRequired ? 1 : 0 );
	UpdateParameter( HTAG_PARAM_ORIG_INTERFACE, GetIfaceNum(pOrigInterface) );

	Data->Copy( MenuCommand, SizeOfMenuCommand );

	return(Status);
} // SetMenuCommandParameters



/*----------------------------------------------------------------------------------------
GetIfaceNum

  This returns a number represetning the interface.
  

  Parameters:  
				pIface[in]:  the interface the Menu Command came over (Originating Interface)

  Returns:  the number representing the interface.
----------------------------------------------------------------------------------------*/
int HTAG_MENU_COMMAND::GetIfaceNum(const CIoMan * pIface)
{
	if ( pIface == g_pStdInterface ) return ifStd;
	else return ifRem;
}




//
//	SetMenuCommandResponseParameters
//		Set menu command response parameters.
//
bool HTAG_MENU_COMMAND::SetMenuCommandResponseParameters( int Protocol, const char *DeviceName, int SizeOfDeviceName,
			const char *MenuCommandResponse, int SizeOfMenuCommandResponse, bool WaitForResponse )
{
	bool		Status = true;			// Assume success for now.


	UpdateParameter( HTAG_PROTOCOL_PARAMETER_NAME, Protocol );
	UpdateParameter( HTAG_DEVICE_NAME_PARAMETER_NAME, DeviceName, SizeOfDeviceName );
	UpdateParameter( HTAG_RESPONSE_PARAMETER_NAME, WaitForResponse ? 1 : 0 );

	Data->Copy( MenuCommandResponse, SizeOfMenuCommandResponse );

	return(Status);
} // SetMenuCommandResponseParameters





//
//	SetNonMenuCommandParameters
//		Set non-menu command parameters.
//
bool HTAG_MENU_COMMAND::SetNonMenuCommandParameters( int Protocol, const char *DeviceName, int SizeOfDeviceName,
			const char *NonMenuCommand, int SizeOfNonMenuCommand, bool CommandResponseRequired, bool WaitForResponse )
{
	bool		Status = true;			// Assume success for now.


	UpdateParameter( HTAG_PROTOCOL_PARAMETER_NAME, Protocol );
	UpdateParameter( HTAG_DEVICE_NAME_PARAMETER_NAME, DeviceName, SizeOfDeviceName );
	UpdateParameter( HTAG_RESPONSE_PARAMETER_NAME, WaitForResponse ? 1 : 0 );
	UpdateParameter( HTAG_MENU_COMMAND_RESPONSE_REQUIRED, CommandResponseRequired ? 1 : 0 );

	Data->Copy( NonMenuCommand, SizeOfNonMenuCommand );

	return(Status);
} // SetNonMenuCommandParameters





//
//	SetNonMenuCommandResponseParameters
//		Set non-menu command response parameters.
//
bool HTAG_MENU_COMMAND::SetNonMenuCommandResponseParameters( int Protocol, const char *DeviceName,
			int SizeOfDeviceName, const char *NonMenuCommandResponse, int SizeOfNonMenuCommandResponse, bool WaitForResponse )
{
	bool		Status = true;			// Assume success for now.


	UpdateParameter( HTAG_PROTOCOL_PARAMETER_NAME, Protocol );
	UpdateParameter( HTAG_DEVICE_NAME_PARAMETER_NAME, DeviceName, SizeOfDeviceName );
	UpdateParameter( HTAG_RESPONSE_PARAMETER_NAME, WaitForResponse ? 1 : 0 );

	Data->Copy( NonMenuCommandResponse, SizeOfNonMenuCommandResponse );

	return(Status);
} // SetNonMenuCommandResponseParameters
//---------------------------------------------------------------------------





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










//
// Constructor for HTAG_ACTIVITY_STATUS
//
HTAG_ACTIVITY_STATUS::HTAG_ACTIVITY_STATUS(const char *Command, HCOMM *HCommDevice, HTRANSFER_STATUS *TransferStatus,
								HCOMMAND_HANDLER *CommandHandler)
		: HTAG( HCommDevice, TransferStatus, CommandHandler, false )
{
	Data = new BITSTRING( DEFAULT_ACTIVITY_STATUS_DATA_ALLOCATION, true );
	HTAG::Data = Data;				// Give the pointer to the base class so it can operate on the data as well.
	CommandName->Copy(Command);

	AddNumericParameter( HTAG_ACTIVITY_STATUS_TYPE, &ActivityType, HTAG_ACTIVITY_STATUS_FLASHING, 0,
								HTAG_ACTIVITY_STATUS_NUMBER_OF_TYPES - 1, true );
	AddNumericParameter( HTAG_ACTIVITY_STATUS_AMOUNT_OF_DATA_PROCESSED, &DataProcessed, 0, true );
	AddNumericParameter( HTAG_ACTIVITY_STATUS_TOTAL_AMOUNT_OF_DATA_TO_BE_PROCESSED, &TotalDataToBeProcessed, 0, true );

	RFScanner = new HSTRING(20, true);
	AddHStringParameter( HTAG_ACTIVITY_STATUS_RF_SCANNER, RFScanner, "", 0, false );

    return;
}
//---------------------------------------------------------------------------





//
// Destructor for HTAG_ACTIVITY_STATUS
//      If memory has been allocated, release it.
//
HTAG_ACTIVITY_STATUS::~HTAG_ACTIVITY_STATUS(void)
{
	delete Data;
	delete RFScanner;

    return;
}





//
//	SetActivityStatusParameters
//		Set activity status parameters.
//
bool HTAG_ACTIVITY_STATUS::SetActivityStatusParameters( int Protocol, int ActivityType, int DataProcessed,
												int TotalDataToBeProcessed, const char *RFScanner )
{
	bool		Status = true;			// Assume success for now.


	UpdateParameter( HTAG_PROTOCOL_PARAMETER_NAME, Protocol );
	UpdateParameter( HTAG_ACTIVITY_STATUS_TYPE, ActivityType );
	UpdateParameter( HTAG_ACTIVITY_STATUS_AMOUNT_OF_DATA_PROCESSED, DataProcessed );
	UpdateParameter( HTAG_ACTIVITY_STATUS_TOTAL_AMOUNT_OF_DATA_TO_BE_PROCESSED, TotalDataToBeProcessed );
	UpdateParameter( HTAG_ACTIVITY_STATUS_RF_SCANNER, RFScanner );

	return(Status);
} // SendActivityStatus





//
//	SendActivityStatus
//		Send activity status report.
//
bool HTAG_ACTIVITY_STATUS::SendActivityStatus( char *Result, int Protocol, int ActivityType, int DataProcessed,
						int TotalDataToBeProcessed, const char *RFScanner, HCOMM *Device,
						HTRANSFER_STATUS *TransferStatus, HCOMMAND_HANDLER *CommandHandler )
{
	bool		Status = true;			// Assume success for now.


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

// Create the activity status command.
	HTAG_ACTIVITY_STATUS	HTAGActivityStatusCommand( HTAG_ACTIVITY_STATUS_DEFAULT_COMMAND_NAME, Device, TransferStatus,
						CommandHandler );

	HTAGActivityStatusCommand.SetActivityStatusParameters( Protocol, ActivityType, DataProcessed, TotalDataToBeProcessed,
						RFScanner );

	int		StartingTransferType = TransferStatus->TransferType;
	TransferStatus->TransferType = HTRANSFER_STATUS::SENDING_ACTIVITY_STATUS;
	TransferStatus->PrepareDownloadStatus();

// Write the result.
	Status = ( HTAGActivityStatusCommand.WriteTaggedImage( Result ) == HTAG::HTAG_OK) ? true : false;

	TransferStatus->CloseDownloadStatus();
	TransferStatus->TransferType = StartingTransferType;

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return(Status);
} // SendActivityStatus
//---------------------------------------------------------------------------






