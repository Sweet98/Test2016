//======================================================================================
// HTagVerifier.h
//======================================================================================
// $RCSfile: HTagVerifier.h $
// $Revision: 1.2 $
// $Date: 2011/11/09 15:24:22EST $
//======================================================================================

//======================================================================================



//---------------------------------------------------------------------------
#ifndef HTAG_VERIFIER_H
#define HTAG_VERIFIER_H
//---------------------------------------------------------------------------


#include	"HStringArray.h"
#include	"HTag.h"
#include	"HTagData.h"
#include	"HTransferProcessing.h"





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




class HTAG_VERIFIER_CALIBRATION_DATA : public HTAG
{
public:
	#define	HTAG_VERIFIER_CALIBRATION_DATA_MAXIMUM_NUMBER_OF_RECORDS	8


public:
	#define	HTAG_VERIFIER_INTERNAL_DEVICE_CALIBRATION_DATA_COMMAND_NAME		"VRFCAL"
	#define	HTAG_VERIFIER_EXTERNAL_DEVICE_CALIBRATION_DATA_COMMAND_NAME		"VRFCLX"
	#define	HTAG_VERIFIER_CALIBRATION_DATA_DEFAULT_COMMAND_NAME				"VRFCAL"


public:
// Data items with names.
	#define	HTAG_VERIFIER_CALIBRATION_DATA_DATE					"DATE"
	HSTRINGARRAY	*Date;
	int				DateItemNumber;

	#define	HTAG_VERIFIER_CALIBRATION_DATA_TIME					"TIME"
	HSTRINGARRAY	*Time;
	int				TimeItemNumber;

	#define	HTAG_VERIFIER_CALIBRATION_DATA_CALIBRATION_DATA		"CALDATA"
	HSTRINGARRAY	*CalibrationData;
	int				CalibrationDataItemNumber;

	#define	HTAG_VERIFIER_CALIBRATION_DATA_DESCRIPTOR			"DESC"
	HSTRINGARRAY	*Descriptor;
	int				DescriptorItemNumber;


public:
// Pointer to the actual data.
	#define	DEFAULT_HTAG_VERIFIER_CALIBRATION_DATA				40
	HTAG_DATA	*Data;


public:
// Constructor for HTAG_VERIFIER_CALIBRATION_DATA.
    HTAG_VERIFIER_CALIBRATION_DATA( const char *Command = HTAG_VERIFIER_CALIBRATION_DATA_DEFAULT_COMMAND_NAME,
						HCOMM *HCommDevice = 0, HTRANSFER_STATUS *TransferStatus = 0, HCOMMAND_HANDLER *CommandHandler = 0);

// Destructor for HTAG_VERIFIER_CALIBRATION_DATA
    ~HTAG_VERIFIER_CALIBRATION_DATA(void);

// Set Verifier calibration data parameters.
	bool SetVerifierCalibrationDataParameters( int Protocol, char *CalibrationData, int SizeOfCalibrationData,
						char *Descriptor, int SizeOfDescriptor, char *Date, char *Time );

// Send Verifier calibration data parameters.
	static bool SendVerifierCalibrationDataParameters( const char *CommandName, char *Result, int Protocol,
				char *CalibrationData, int SizeOfCalibrationData, char *Descriptor, int SizeOfDescriptor, char *Date,
				char *Time, HCOMM *Device = 0, HTRANSFER_STATUS *TransferStatus = 0, HCOMMAND_HANDLER *CommandHandler = 0 );

// Receive Verifier HTAG commands.
	static bool ReceiveVerifierCommand( char *Result, int *Status, HSTRING *CommandName,
				HSTRING *ReceivedData, int CommandLength, HCOMM *Device = 0, HTRANSFER_STATUS *TransferStatus = 0,
				HCOMMAND_HANDLER *CommandHandler = 0, HTAG_POST_TRANSFER_PROCESSOR *PostTransferProcessor = 0 );
	
private:
	HTAG_VERIFIER_CALIBRATION_DATA (const HTAG_VERIFIER_CALIBRATION_DATA&);            // declared private, but not defined, to prevent copying         
	HTAG_VERIFIER_CALIBRATION_DATA& operator=(const HTAG_VERIFIER_CALIBRATION_DATA&);  // declared private, but not defined, to prevent copying 

};
//----------------------------------------------------------------------------




#endif // HTAG_VERIFIER_H



