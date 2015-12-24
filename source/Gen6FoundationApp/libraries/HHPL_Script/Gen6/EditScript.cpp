/*=============================================================================
 *

 *=============================================================================
 * $RCSfile: Gen6/EditScript.cpp $
 * $Revision: 1.7 $
 * $Date: 2011/01/10 15:32:59EST $
 *============================================================================*/

#include <stddef.h>
#include "stdInclude.h"
//#include	"FlashScript.h"
#include "BarcodeFifo.h"
#include "BarcodeData.h"
#include "EditScript.h"
#include "ioman.h"
#include "menu.h"
#include "hstring.h"
#include "dataedit.h"
#include "TransCodeEx.h"
#include "productconfiguration.h"
#include "readerconfiguration.h"


#ifdef PLUGIN_DEV
#include "db_chrio.h"
//#include "elf_plugin.h"


bool CEditScriptProc::DoPluginFormat(CBarcodeData *&pCurrentMsg, CBarcodeFifo *pOutFifo)
{
	/* Invoke all plugins to process the output content */
	HONPluginRawInfo *PluginInfo;
	HONPlugin *Plugin, *FirstCallPlugin = NULL;
	DataEditApi *FormatApis;
	CBarcodeData *pDestinationLabel;
	/* The default return value is false, so for the first plugin, 
	    if it was configured as chain on success, it will not be called */
	volatile int ret = -1, RetVal = -1;
	UINT NumSourceChars, DestinationSize, TempBufferSize;
	tichar_t *pFormattedSymbol;				//buffers
	DataEditParam Param;
	int CharSize = sizeof(tichar_t);


	// Get the raw buffer and size of the current message
	tichar_t *pucMessage, *ptmpMessage;
	if (PASS != pCurrentMsg->GetContentsBuffer(pucMessage, NumSourceChars))
		return false;

	TempBufferSize = NumSourceChars;
	DestinationSize = NumSourceChars + 500;

	// Allocate temp memory to hold the temporary formatted data.
	ptmpMessage = (tichar_t*)malloc(DestinationSize*CharSize);
	if(!ptmpMessage)
		return false;

	/* Plugin not configured or no plugins are configured as before called, return */
	if(g_pFormattingPluginConfiguration && g_pFormattingPluginConfiguration->GetFirstPlugin())
	{
		pDestinationLabel = CBarcodeData::GetSaveNewBuffer(pFormattedSymbol, DestinationSize);
		pFormattedSymbol[0] = '\0';	// ensure a 0 termination
		memcpy(pFormattedSymbol, (const char*)pucMessage, NumSourceChars*CharSize);
		// copy the original input data to the temp buffer
		memcpy(ptmpMessage, (const char*)pucMessage, NumSourceChars*CharSize);

		Param.AIMcodeLetter = pCurrentMsg->GetAimId();
		Param.AIMcodeModifier = pCurrentMsg->GetAimModifier();
		Param.HHPcodeID = pCurrentMsg->GetHHPId();
		Param.length = NumSourceChars;
		Param.CharSize = CharSize;
		Param.message = (unsigned char*)pFormattedSymbol;

		for(Plugin = g_pFormattingPluginConfiguration->GetFirstPlugin(); 
			Plugin && Plugin != g_pFormattingPluginConfiguration->GetSystemDummy(); 
			Plugin = Plugin->GetNextPlugin())
		{
			PluginInfo = Plugin->GetRawInfo();
			FormatApis = (DataEditApi*)PluginInfo->PluginApis;

			if(!PluginInfo || !FormatApis || !FormatApis->DataEdit)
				continue;

			if(FirstCallPlugin == NULL)
				FirstCallPlugin = Plugin;

			if(Plugin->GetChainOnExit() == HON_PLUGIN_CHAIN_ON_EXIT_CHAINNEVER)
				continue;

			// If this is the first plugin, ignore chain failure or chain succeed. The first must be called.
			if(Plugin != FirstCallPlugin){
				if(!ret && Plugin->GetChainOnExit() == HON_PLUGIN_CHAIN_ON_EXIT_CHAINIFFAILURE)
					continue;

				if(ret && Plugin->GetChainOnExit() == HON_PLUGIN_CHAIN_ON_EXIT_CHAINIFSUCCESS)
					continue;
			}

			/* Entry data state must be original if chianed on failure in configuration file */
			if(Plugin->GetEntryDataState() == HON_PLUGIN_ENTRY_DATA_STATE_ORIGINAL){
				memcpy(pFormattedSymbol, (const char*)pucMessage, NumSourceChars*CharSize);
				Param.length = NumSourceChars;
			}else if(Plugin->GetEntryDataState() == HON_PLUGIN_ENTRY_DATA_STATE_MODIFIED){
				memcpy(pFormattedSymbol, (const char*)ptmpMessage, TempBufferSize*CharSize);
				Param.length = TempBufferSize;


			}

			ret = FormatApis->DataEdit(&Param);



			if(!ret){
				// Copy the result to temp buffer
				if(DestinationSize >= Param.length){
					memcpy(ptmpMessage, (const char*)Param.message, Param.length*CharSize);
					TempBufferSize = Param.length;
				}
				// Once we get one plugin or system routine parsed, return true.
				RetVal = 0;
			}
				

		}


		/* Then System routine handler */
		if(Plugin->GetChainOnExit() == HON_PLUGIN_CHAIN_ON_EXIT_CHAINALWAYS
			|| (Plugin->GetChainOnExit() == HON_PLUGIN_CHAIN_ON_EXIT_CHAINIFFAILURE && ret)
			||(Plugin->GetChainOnExit() == HON_PLUGIN_CHAIN_ON_EXIT_CHAINIFSUCCESS && !ret))
		{
			if(Plugin->GetEntryDataState() == HON_PLUGIN_ENTRY_DATA_STATE_ORIGINAL){
				memcpy(pFormattedSymbol, (const char*)pucMessage, NumSourceChars*CharSize);
				*((tichar_t *)Param.message + NumSourceChars)= 0;
				pDestinationLabel->CopyTypeInfo(pCurrentMsg);
				pDestinationLabel->FinalizeBuffer(NumSourceChars);	// ensure the lenght info is correct
			}else if(Plugin->GetEntryDataState() == HON_PLUGIN_ENTRY_DATA_STATE_MODIFIED){
				memcpy(pFormattedSymbol, (const char*)ptmpMessage, TempBufferSize*CharSize);
				*((tichar_t *)Param.message + TempBufferSize)= 0;
				pDestinationLabel->CopyTypeInfo(pCurrentMsg);
				pDestinationLabel->FinalizeBuffer(TempBufferSize);	// ensure the lenght info is correct
			}

			theDataEditor.GetDataEditSettingsFromMenu() ;
			bool RVal = theDataEditor.DataEdit(pDestinationLabel);
			ret = RVal?0:(-1);

			if(!ret){
				if (PASS != pDestinationLabel->GetContentsBuffer(pFormattedSymbol, TempBufferSize)){
					pDestinationLabel->Free();
					if(ptmpMessage)
						free(ptmpMessage);
					return false;
				}
				// Copy the result to temp buffer
				if(DestinationSize >= TempBufferSize){
					memcpy(ptmpMessage, (const char*)pFormattedSymbol, TempBufferSize*CharSize);
				}

				// Once we get one plugin or system routine parsed, return true.
				RetVal = 0;
			}
			
		}

		// !!!!Buffer is update, assgine to Param.message again
		Param.message = (unsigned char*)pFormattedSymbol;
		Param.length = TempBufferSize;

		/* After routines */
		for(Plugin = Plugin->GetNextPlugin(); Plugin;Plugin = Plugin->GetNextPlugin())
		{
			PluginInfo = Plugin->GetRawInfo();
			FormatApis = (DataEditApi*)PluginInfo->PluginApis;

			if(!PluginInfo || !FormatApis || !FormatApis->DataEdit)
				continue;

			if(FirstCallPlugin == NULL)
				FirstCallPlugin = Plugin;

			if(Plugin->GetChainOnExit() == HON_PLUGIN_CHAIN_ON_EXIT_CHAINNEVER)
				continue;

			// If this is the first plugin, ignore chain failure or chain succeed. The first must be called.
			if(Plugin != FirstCallPlugin){
				if(!ret && Plugin->GetChainOnExit() == HON_PLUGIN_CHAIN_ON_EXIT_CHAINIFFAILURE)
					continue;

				if(ret && Plugin->GetChainOnExit() == HON_PLUGIN_CHAIN_ON_EXIT_CHAINIFSUCCESS)
					continue;
			}

			/* Entry data state must be original if chianed on failure in configuration file */
			if(Plugin->GetEntryDataState() == HON_PLUGIN_ENTRY_DATA_STATE_ORIGINAL){
				memcpy(pFormattedSymbol, (const char*)pucMessage, NumSourceChars*CharSize);
				Param.length = NumSourceChars;
			}else if(Plugin->GetEntryDataState() == HON_PLUGIN_ENTRY_DATA_STATE_MODIFIED){
				memcpy(pFormattedSymbol, (const char*)ptmpMessage, TempBufferSize*CharSize);
				Param.length = TempBufferSize;
			}

			ret = FormatApis->DataEdit(&Param);

			if(!ret){
				// Copy the result to temp buffer
				if(DestinationSize >= Param.length){
					memcpy(ptmpMessage, (const char*)Param.message, Param.length*CharSize);
					TempBufferSize = Param.length;
				}

				// Once we get one plugin or system routine parsed, return true.
				RetVal = 0;
			}
			

		}

		if(!RetVal){
			*((tichar_t *)Param.message + TempBufferSize)= 0;	                  // add a terminating 0 (not really required, but good for debugging)
				// now exchange the objects (this avoids a copy action)
				pDestinationLabel->CopyTypeInfo(pCurrentMsg);
				pCurrentMsg->Free();  	// this is the "old" source message which gets replaced by the formatted message
				pCurrentMsg = pDestinationLabel;			// do the replace action
				pCurrentMsg->FinalizeBuffer(TempBufferSize);	// ensure the lenght info is correct

		}else{
			pDestinationLabel->Free();
		}

	}
	else
	{
		theDataEditor.GetDataEditSettingsFromMenu() ;
		bool RVal = theDataEditor.DataEdit(pCurrentMsg);
		RetVal = RVal?0:(-1);
	}

	if(ptmpMessage)
		free(ptmpMessage);

	if(!RetVal){
		pOutFifo->Write(pCurrentMsg);	      // Formatter was happy, send it.
		return true;
	}else{
		pCurrentMsg->Free();
		return false;
	}

}

#endif

// Here we either call the IQEdit script or the good old DataEdit (Formatter).
// Note: The IQEdit script can call the DataEdit by its "send" function (ends up in SendToHost). See there for details.
bool CEditScriptProc::DoDataEditOrScript(CBarcodeData *&pCurrentMsg, CBarcodeFifo *pOutFifo)
{
	ASSERT(pOutFifo!=NULL);
	bool RetVal=true;
	
	if (IOCRET_NO!=theStdInterface.DeviceIoControl(IOCTRL_IS_DATA_EDITING_ALLOWED, (LPARAM)pCurrentMsg)) // if default or yes do edit
	{
		theTransCode.AutoTranslate(pCurrentMsg, IO_DATATYPE_UNICODE);

#ifdef PLUGIN_DEV
		RetVal = CEditScriptProc::DoPluginFormat(pCurrentMsg, pOutFifo);
#else
			RetVal = CEditScriptProc::CallIQEditScript(pCurrentMsg, pOutFifo);
			if(!RetVal)
			{
				RetVal = CEditScriptProc::CallClassicDataEdit(pCurrentMsg, pOutFifo);
			}
#endif
	}
	else
	{
		RetVal=(pOutFifo->Write(pCurrentMsg)>0); // no data formatting allowed just copy to output if no room say error
	}

	return RetVal;
}

// a helper to call the script
// We check if scripting is allowed,
// read the script from the flash file,
// instantiate the interpreter and run the script.
inline bool CEditScriptProc::CallIQEditScript(CBarcodeData *&pCurrentMsg, CBarcodeFifo *pOutFifo)
{
	bool RetVal=false;
	HSTRING EditScript(256,true);
	// Any active script available?
	if (0) //FIXMEReadEditScriptFromFlash(GET_SETTING(ActiveIQEditProScript), &EditScript))
	{
		CEditScriptProc *pScriptInterpreter = new CEditScriptProc(pOutFifo);
		pScriptInterpreter->ProcessHHPLProgram(&EditScript, pCurrentMsg);

		if( !pScriptInterpreter->HHPLProgramTerminatedBecauseScriptingIsNotAvailable )
		{
			RetVal = true;
		// the interpretter did send the data to the pOutFifo, so we free the old obsolete object.
			pCurrentMsg->Free();
		}

		delete pScriptInterpreter;
	}
	return RetVal;
}

// a helper to call the formatter
inline bool CEditScriptProc::CallClassicDataEdit(CBarcodeData *&pCurrentMsg, CBarcodeFifo *pOutFifo)
{
#if !defined( RFBASE ) && !defined( RFSCAN )
	pCurrentMsg->SetControlMask(HHPL_SEND_TO_HOST_DEFAULT);
#endif
	theDataEditor.GetDataEditSettingsFromMenu() ;
	bool RetVal = theDataEditor.DataEdit(pCurrentMsg);
	if(RetVal)
	{
		pOutFifo->Write(pCurrentMsg);	      // Formatter was happy, send it.
	}
	else
	{
		pCurrentMsg->Free();	// Formatter failed to find a valid format and was set to "required". We do not send it.
	}
	return RetVal;
}

CEditScriptProc::CEditScriptProc(CBarcodeFifo *pOutFifo)
{
	// store the fifo pointer so we can use it in the callback from the script interpreter
	m_pOutFifo = pOutFifo;
}

void CEditScriptProc::ProcessHHPLProgram(HSTRING *pCode, CBarcodeData *&pCurrentMsg)
{
	// prepare the input data into a form the interpreter can use.
	const tichar_t *pInputData;
	size_t InputDataSize;
	pCurrentMsg->GetTotalData(pInputData, InputDataSize);

	const int IDSize=5;
	tichar_t DataSourceID[IDSize+1];
	pCurrentMsg->FillDataSourceInfo(DataSourceID);

	HHPL_PROCESSOR::ProcessHHPLProgram(pCode->Char, pCode->Size, pInputData, InputDataSize, DataSourceID, IDSize, pCurrentMsg->GetControlMask() );
}




//
//	IsScriptingAvailable
//		Overload for HHPL_PROCESSOR function to tell the processor whether or not scripting is enabled by product configuration.
//
bool CEditScriptProc::IsScriptingAvailable( void )
{
	return GET_CONFIG_SETTING( IQEditingProAvailable );
}




// this is he callback that gets called from the interpreter if it finds a "send" command in the script.
bool	CEditScriptProc::SendToHost( SEND_TO_HOST_PARAMETERS *SendParameters )
{
	bool ret=true;
#ifdef UNICODE_FOR_INTERFACES
	const IoDataType_t DataType = IO_DATATYPE_UNICODE;
	const wchar_t *String = SendParameters->String;
	int LengthOfString = SendParameters->LengthOfString;
#else
//////// For now we must translate back to byte characters
	const IoDataType_t DataType = IO_DATATYPE_CP_SYS;
	HSTRING	wchar_tData( (UCHAR *) SendParameters->String, SendParameters->LengthOfString * 2, true);
	wchar_tData.UniCodeToSimpleAsciiFormat();
	const tchar_t *String = wchar_tData.Char;
	int LengthOfString = wchar_tData.Size;
////////
#endif

	CBarcodeData *pCurrentMsg = CBarcodeData::StoreToNewBuffer(String, LengthOfString);
	pCurrentMsg->CopyIDsFromDataSourceInfo(SendParameters->DataSourceInformation);
	pCurrentMsg->SetControlMask(SendParameters->ControlFlags);
	pCurrentMsg->SetAdditionalInterMessageDelay(SendParameters->DelayInMicroseconds);
	pCurrentMsg->StoreDataType(DataType);

	if(SendParameters->ControlMask & HHPL_SEND_TO_HOST_RUN_THROUGH_FORMATTER_BEFORE_OUTPUTTING)
	{
		// do old style data edit
		theDataEditor.GetDataEditSettingsFromMenu();
		ret = theDataEditor.DataEdit(pCurrentMsg);
	}

	ASSERT(m_pOutFifo!=NULL);
	m_pOutFifo->Write(pCurrentMsg);

	return ret;
}



