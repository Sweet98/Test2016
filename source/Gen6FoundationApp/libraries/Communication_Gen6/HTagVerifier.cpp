//======================================================================================
// HTagVerifier.cpp
//======================================================================================
// $RCSfile: HTagVerifier.cpp $
// $Revision: 1.1 $
// $Date: 2009/03/08 16:04:27EDT $
//======================================================================================

//======================================================================================


//#include	"HHPCommunicationConfig.h"

#include	"HTagVerifier.h"		// HTagDataResult interface definition

//#include	<stdlib.h>		/* standard library functions				*/
//#include	<stdio.h>
#include	<string.h>





//---------------------------------------------------------------------------
//
//  Class Name:     HTAG_VERIFIER_CALIBRATION_DATA
//
//  Class Description:
//
//		The HTAG_VERIFIER_CALIBRATION_DATA class provides a mechanism to transfer / store Verifier calibration data.  Since
//		all fields are tagged, if the receiver does not yet speak the new version of HTAG_VERIFIER_CALIBRATION_DATA, it will
//		ignore tags that it doesn't understand.
//
//---------------------------------------------------------------------------




//
// Constructor for HTAG_VERIFIER_CALIBRATION_DATA
//
HTAG_VERIFIER_CALIBRATION_DATA::HTAG_VERIFIER_CALIBRATION_DATA( const char *Command, HCOMM *HCommDevice,
								HTRANSFER_STATUS *TransferStatus, HCOMMAND_HANDLER *CommandHandler)
		: HTAG( HCommDevice, TransferStatus, CommandHandler, false )
{
	Data = new HTAG_DATA( DEFAULT_HTAG_VERIFIER_CALIBRATION_DATA, true, HTAG_VERIFIER_CALIBRATION_DATA_MAXIMUM_NUMBER_OF_RECORDS );
	HTAG::HTagData = Data;				// Give the pointer to the base class so it can operate on the data as well.
	CommandName->Copy( Command );

// Data items with names.
	Date = new HSTRINGARRAY();
	Date->SetSize( HTAG_VERIFIER_CALIBRATION_DATA_MAXIMUM_NUMBER_OF_RECORDS );
	DateItemNumber = Data->Add( HTAG_VERIFIER_CALIBRATION_DATA_DATE, Date, false );

	Time = new HSTRINGARRAY();
	Time->SetSize( HTAG_VERIFIER_CALIBRATION_DATA_MAXIMUM_NUMBER_OF_RECORDS );
	TimeItemNumber = Data->Add( HTAG_VERIFIER_CALIBRATION_DATA_TIME, Time, false );

	CalibrationData = new HSTRINGARRAY();
	CalibrationData->SetSize( HTAG_VERIFIER_CALIBRATION_DATA_MAXIMUM_NUMBER_OF_RECORDS );
	CalibrationDataItemNumber = Data->Add( HTAG_VERIFIER_CALIBRATION_DATA_CALIBRATION_DATA, CalibrationData, false );

	Descriptor = new HSTRINGARRAY();
	Descriptor->SetSize( HTAG_VERIFIER_CALIBRATION_DATA_MAXIMUM_NUMBER_OF_RECORDS );
	DescriptorItemNumber = Data->Add( HTAG_VERIFIER_CALIBRATION_DATA_DESCRIPTOR, Descriptor, false );

//Make protocol parameter optional
	Parameters[ TransferProtocolParameterNumber ].AlwaysReport = false;

    return;
}
//---------------------------------------------------------------------------





//
// Destructor for HTAG_VERIFIER_CALIBRATION_DATA
//      If memory has been allocated, release it.
//
HTAG_VERIFIER_CALIBRATION_DATA::~HTAG_VERIFIER_CALIBRATION_DATA(void)
{
	delete Date;
	delete Time;
	delete CalibrationData;
	delete Descriptor;
	delete Data;

    return;
}











//
//	SetVerifierCalibrationDataParameters
//		Set Verifier calibration data parameters.
//
bool HTAG_VERIFIER_CALIBRATION_DATA::SetVerifierCalibrationDataParameters( int Protocol, char *CalibrationData,
				int SizeOfCalibrationData, char *Descriptor, int SizeOfDescriptor, char *Date, char *Time )
{
	bool		Status = true;			// Assume success for now.


	UpdateParameter( HTAG_PROTOCOL_PARAMETER_NAME, Protocol );

	HSTRING		hstrCalibrationData( (unsigned char *) CalibrationData, SizeOfCalibrationData, false, true, true );
	Data->Update( HTAG_VERIFIER_CALIBRATION_DATA_CALIBRATION_DATA, &hstrCalibrationData );
	HSTRING		hstrDescriptor( (unsigned char *) Descriptor, SizeOfDescriptor, false, true, true );
	Data->Update( HTAG_VERIFIER_CALIBRATION_DATA_DESCRIPTOR, &hstrDescriptor );
	HSTRING		hstrDate( (unsigned char *) Date, strlen( Date ), false, true, true );
	Data->Update( HTAG_VERIFIER_CALIBRATION_DATA_DATE, &hstrDate );
	HSTRING		hstrTime( (unsigned char *) Time, strlen( Time ), false, true, true );
	Data->Update( HTAG_VERIFIER_CALIBRATION_DATA_TIME, &hstrTime );

	return(Status);
} // SetVerifierCalibrationDataParameters





//
//	SendVerifierCalibrationDataParameters
//		Send Verifier calibration data parameters.
//
bool HTAG_VERIFIER_CALIBRATION_DATA::SendVerifierCalibrationDataParameters( const char *CommandName, char *Result, int Protocol,
				char *CalibrationData, int SizeOfCalibrationData, char *Descriptor, int SizeOfDescriptor, char *Date,
				char *Time, HCOMM *Device, HTRANSFER_STATUS *TransferStatus, HCOMMAND_HANDLER *CommandHandler )
{
	bool		Status = true;			// Assume success for now.


	TransferStatus->IncrementProcessReceivedDataLockSemaphore();

// Create the activity status command.
	HTAG_VERIFIER_CALIBRATION_DATA	VerifierCalibrationData( CommandName, Device, TransferStatus, CommandHandler );

	VerifierCalibrationData.SetVerifierCalibrationDataParameters( Protocol, CalibrationData, SizeOfCalibrationData,
						Descriptor, SizeOfDescriptor, Date, Time );

	int		StartingTransferType = TransferStatus->TransferType;
	TransferStatus->TransferType = HTRANSFER_STATUS::SENDING_VERIFIER_CALIBRATION_DATA;
	TransferStatus->PrepareDownloadStatus();

// Write the result.
	Status = ( VerifierCalibrationData.WriteTaggedImage( Result ) == HTAG::HTAG_OK) ? true : false;

	TransferStatus->CloseDownloadStatus();
	TransferStatus->TransferType = StartingTransferType;

	TransferStatus->DecrementProcessReceivedDataLockSemaphore();

	return(Status);
} // SendVerifierCalibrationDataParameters





//
//	ReceiveVerifierCommand
//		Receive Verifier HTAG commands.
//
bool HTAG_VERIFIER_CALIBRATION_DATA::ReceiveVerifierCommand( char *Result, int *Status, HSTRING *CommandName,
				HSTRING *ReceivedData, int CommandLength, HCOMM *Device, HTRANSFER_STATUS *TransferStatus,
				HCOMMAND_HANDLER *CommandHandler, HTAG_POST_TRANSFER_PROCESSOR *PostTransferProcessor )
{
	int		StartingTransferType = TransferStatus->TransferType;
	bool	CommandFound = true;		// Assume success.


// Is it a command to process Verifier calibration data?
	if(( CommandName->Compare( HTAG_VERIFIER_INTERNAL_DEVICE_CALIBRATION_DATA_COMMAND_NAME, false ) == 0 ) ||
			( CommandName->Compare( HTAG_VERIFIER_EXTERNAL_DEVICE_CALIBRATION_DATA_COMMAND_NAME, false ) == 0 ))
	{
	// If we're going to need more space in ReceivedData, reallocate it.  Note that we allocate 1.5 times as much as is needed
	// in case the protocol is HMODEM compressed and extra space is needed for the decompressed result.
		if( (int) ReceivedData->Size < ( CommandLength - HTAG_COMMAND_NAME_SIZE ) )
		{
			ReceivedData->Reallocate( ( CommandLength - HTAG_COMMAND_NAME_SIZE ) * 3 / 2 + 1024 );
		}

		TransferStatus->TransferType = HTRANSFER_STATUS::RECEIVING_VERIFIER_CALIBRATION_DATA;
		TransferStatus->PrepareDownloadStatus();
		HTAG_VERIFIER_CALIBRATION_DATA	VerifierCalibrationData( (char *) CommandName->Char,
												Device, TransferStatus, CommandHandler );
		*Status = VerifierCalibrationData.ReadTaggedImage( ReceivedData, Result, CommandLength - HTAG_COMMAND_NAME_SIZE);
		if((*Status == HTAG::HTAG_OK) || (*Status == HTAG::HTAG_UNRECOGNIZED_PARAMETER_NAME) || (*Status < 0))
		{
			if( PostTransferProcessor != NULL)  PostTransferProcessor->ProcessHTAGData( Status, Result,
								HTAG_POST_TRANSFER_PROCESSOR::VERIFIER_CALIBRATION_DATA, &VerifierCalibrationData );
		}
		TransferStatus->CloseDownloadStatus();
		TransferStatus->TransferType = StartingTransferType;
	}
	else
	{
		CommandFound = false;
	}

	return CommandFound;
} // ReceiveVerifierCommand






