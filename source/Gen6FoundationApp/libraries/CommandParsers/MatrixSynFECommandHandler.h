//======================================================================================
// db_SYNFECommandHandler.h
//======================================================================================
// $RCSfile: MatrixSynFECommandHandler.h $
// $Revision: 1.4 $
// $Date: 2010/03/19 17:33:39EDT $
//======================================================================================

//======================================================================================






//---------------------------------------------------------------------------
//
//  Class definitions for MATRIX_SYN_FE_COMMAND_HANDLER
//
//---------------------------------------------------------------------------
#ifndef MATRIX_SYN_FE_COMMAND_HANDLER_H
#define MATRIX_SYN_FE_COMMAND_HANDLER_H
//---------------------------------------------------------------------------

//
// In order to use SYN_FE_COMMAND_HANDLER, several members of the SYN_FE_COMMAND_HANDLER class must be overridden.  The
// MATRIX_SYN_FE_COMMAND_HANDLER class accomplishes this.
//



#include	"HCommandHandler.h"
#include	"SYNFECommandHandler.h"	// Base class

#ifdef PLUGIN_DEV
#include "HONPluginConfiguration.h"
#endif






	
class MATRIX_SYN_FE_COMMAND_HANDLER : public SYN_FE_COMMAND_HANDLER,
		public HTAG_POST_TRANSFER_PROCESSOR,
		public HTAG_IMAGE_POST_TRANSFER_PROCESSOR,
		public HTAG_SCRIPT_POST_TRANSFER_PROCESSOR,
		public HTAG_SCRIPT_HOST_COMMAND_POST_TRANSFER_PROCESSOR,
		public HTAG_SCRIPT_HOST_DATA_POST_TRANSFER_PROCESSOR,
		public HCOMMAND_HANDLER,
		public HTAG_FILE_POST_TRANSFER_PROCESSOR,
		public HTAG_CODE_POST_TRANSFER_PROCESSOR,
		public READER_DEVICE_EVENT_PROCESSOR

{
public:
	virtual void ProcessHTAGData( int *Status, char *Result, int DataType, void *NewHTAGData );
	virtual void ProcessImage(int *Status, char *Result, HTAG_IMAGE *NewImage);
	virtual void StoreHHPLMainScript(int *Status, char *Result, HTAG_SCRIPT *NewScript);
	virtual void RetrieveHHPLMainScript(int *Status, char *Result, HTAG_SCRIPT *Script);
	virtual void LoadAndRunHHPLMainScript(int *Status, char *Result, HTAG_SCRIPT *NewScript);
	virtual void StoreHHPLEditScript(int *Status, char *Result, HTAG_SCRIPT *NewScript);
	virtual void RetrieveHHPLEditScript(int *Status, char *Result, HTAG_SCRIPT *Script);
	virtual void LoadAndRunHHPLEditScript(int *Status, char *Result, HTAG_SCRIPT *NewScript);
	virtual void HHPLSendHostCommand(int *Status, char *Result, HTAG_SCRIPT_HOST_COMMAND *NewScriptHostCommand);
	virtual void HHPLSendHostData(int *Status, char *Result, HTAG_SCRIPT_HOST_DATA *NewScriptHostData);
	virtual void StoreFile( int *Status, char *Result, HTAG_FILE *NewFile );
	virtual void RetrieveFile( int *Status, char *Result, HTAG_FILE *File );
	virtual void StoreApplicationCode(int *Status, char *Result, HTAG_READER_CODE *NewApplicationCode);
	virtual void StoreBootCode(int *Status, char *Result, HTAG_READER_CODE *NewBootCode);
	virtual void StoreFlashImage(int *Status, char *Result, HTAG_READER_CODE *NewFlashImage );
	virtual void RetrieveFlashImage(int *Status, char *Result, HTAG_READER_CODE *FlashImage );
	virtual void StoreScanEngineFirmware( int *Status, char *Result, HTAG_SCAN_ENGINE_FIRMWARE *NewScanEngineFirmware );

	virtual	void PrepareForFileDownload(void);

#ifdef RFBASE	
	virtual void ProcessDecodeResult(int *Status, char *Result, HTAG_DECODE_RESULT *DecodeResult);
#endif

	virtual void ProcessSystemInformationRequest(int *Status, char *Result, HTAG_SYSTEM_INFORMATION *SystemInformation );
	virtual void ProcessSystemInformation(int *Status, char *Result, HTAG_SYSTEM_INFORMATION *SystemInformation);
	virtual void ProcessMenuCommand(int *Status, char *Result, HTAG_MENU_COMMAND *MenuCommand);
	virtual void ProcessMenuCommandResponse(int *Status, char *Result, HTAG_MENU_COMMAND *MenuCommandResponse);
	virtual void ProcessNonMenuCommand(int *Status, char *Result, HTAG_MENU_COMMAND *NonMenuCommand);
	virtual void ProcessNonMenuCommandResponse(int *Status, char *Result, HTAG_MENU_COMMAND *NonMenuCommandResponse);
	
	virtual void ProcessConfigMenuCommand(int *Status, char *Result, HTAG_MENU_COMMAND *MenuCommand);
	virtual void ProcessConfigMenuCommandResponse(int *Status, char *Result, HTAG_MENU_COMMAND *MenuCommandResponse);
	
#ifdef RFBASE
	virtual void TriggerEvent(int TriggeredState );
#endif

public:
// Constructor for MATRIX_SYN_FE_COMMAND_HANDLER
    MATRIX_SYN_FE_COMMAND_HANDLER(HCOMM *HCommDevice, HTRANSFER_STATUS *InputTransferStatus);

// Destructor for MATRIX_SYN_FE_COMMAND_HANDLER
    ~MATRIX_SYN_FE_COMMAND_HANDLER(void);

};
//----------------------------------------------------------------------------

extern MATRIX_SYN_FE_COMMAND_HANDLER *SYNFECommandHandler;

#endif // SYN_FE_COMMAND_HANDLER_H



