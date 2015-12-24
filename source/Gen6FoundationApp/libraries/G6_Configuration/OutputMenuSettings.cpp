//======================================================================================
// OutputMenuSettings.cpp
//======================================================================================
// $RCSfile: OutputMenuSettings.cpp $
// $Revision: 1.20 $
// $Date: 2011/09/13 17:17:10EDT $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Module Description:
//      This file contains a routine that creates output menu settings as part
//		of the overall reader configuration for a Matrix device.
//
//---------------------------------------------------------------------------

#include	<stdio.h>

#include "stdInclude.h"
#include "db_chrio.h"
#include "errordefs.h"
#include "ioman.h"
#include "ReaderConfiguration.h"
#include "OutputMenuSettings.h"			// OutputMenuSettings interface definition
#include "menu.h"

extern bool IBM4683RetailPDFEnable;
extern int IBM4683SurePosMaxPacketSize;
extern bool IBM4690RetailPDFEnable;
extern int IBM4690SurePosMaxPacketSize;




//extern void		SendStringToInterface(unsigned char *pucBuffer);


/*------------------------------------------------------------------------------
ReportMaximumMessageSize()
------------------------------------------------------------------------------*/
bool ReportMaximumMessageSize(void)
{
	unsigned int uiSize ;
	unsigned char ucBuffer[8] = {'\0'};

//	RESET_WATCHDOG_TIMER();

	/*	If there is no interface open then we can't send	*/
	if(  IsInterfaceRunning()==FALSE )
		return false;

	uiSize = MAX_2D_BARCODE_MSG ;
	sprintf((char *)ucBuffer, "%u\r\n", uiSize);
//FIXME	SendStringToInterface(ucBuffer);

	return true;
}


bool ExecuteRetailPDF(void)
{
#ifndef RFSCAN 
	//IBM4683 setting
	IBM4683RetailPDFEnable = GET_SETTING(RetailPDFEnable);
	IBM4683SurePosMaxPacketSize = GET_SETTING(SurePosMaxPacketSize);
	
	//IBM4690 setting
	IBM4690RetailPDFEnable = GET_SETTING(RetailPDFEnable);
	IBM4690SurePosMaxPacketSize = GET_SETTING(SurePosMaxPacketSize);
#endif	
	return true;
}

bool TurnOffFactorySuffix(void)
{
	HSTRING MenuCommand("0805030.", true);
	return (ReaderConfiguration->Menu(&MenuCommand))? true:false;
}


void READER_CONFIGURATION::CreateOutputMenuSettings( int /* ControlMask */ )
{
	AddCommand("080100", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING , &ReportMaximumMessageSize,NULL); //INVISIBLE
	AddNumber("080101", SUB_TAG_TABLE::FIXED | LOCAL_SETTING  | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG, NULL, NULL, &MaxBcodeMsgSize,
	          MAX_2D_BARCODE_MSG, MAX_2D_BARCODE_MSG, MAX_2D_BARCODE_MSG, MAX_2D_BARCODE_MSG);

	AddBoolean("080200", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &TimeoutTransmitSequenceEnable,false, false);



	//	Data Editing Settings
	AddDataFormat("080300",	SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &DataFormatObj,
	              DATAFORMAT_BLOCKSIZE,  "", "");
	AddPrefixSuffix("080400",	SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &PrefixObj,
	                PREFIX_BLOCKSIZE, "", "");
#if defined( DEVELOPMENT_SETTINGS )

	AddPrefixSuffix("080500",	SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, &TurnOffFactorySuffix, &SuffixObj,
	                SUFFIX_BLOCKSIZE, "990D0A", "990D0A");
#else		//	Use this suffix for release
	AddPrefixSuffix("080500",	SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, &TurnOffFactorySuffix, &SuffixObj,
	                SUFFIX_BLOCKSIZE, "", "");
#endif
    
    AddPrefixSuffix("080501",   SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP , NULL, NULL, &KeyboardSuffixObj,
                    SUFFIX_BLOCKSIZE, "990D", "990D");
    
    AddPrefixSuffix("080502",   SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP , NULL, NULL, &ComSuffixObj,
                    SUFFIX_BLOCKSIZE, "990D0A", "990D0A");
 
    AddBoolean("080503", NULL | LOCAL_SETTING , NULL, NULL, &SuffixFactoryEnable, false, false); //Setting should not be controlled by a user
    
	AddCommand("080301", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);


	AddNumericList("080302", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &DataFormatEnable, DFM_ENABLE, DFM_ENABLE,
				DFM_DISABLE,DFM_ENABLE,DFM_REQUIRED,DFM_ENABKE_P,DFM_REQUIRED_P);

	AddBoolean("080600", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &base64enable, false, false);


	AddCommand("080303", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP  | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, NULL, NULL, SUB_TAG_TABLE::CLEAR_FORMAT_ENTRY);
	AddCommand("080304", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, SUB_TAG_TABLE::CLEAR_ALL_FORMAT_ENTRIES);
	AddCommand("080401", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);
	AddBoolean("080402", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &PrefixEnable, true, true);
	AddCommand("080403", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP  | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, NULL, NULL, SUB_TAG_TABLE::CLEAR_PREFIX_SUFFIX_ENTRY);
	AddCommand("080404", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, SUB_TAG_TABLE::CLEAR_ALL_PREFIX_SUFFIX_ENTRIES);

	AddCommand("080504", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, SUB_TAG_TABLE::SET_WORKING_AND_ACTIVE_VALUES_TO_DEFAULT_FOR_THIS_TAG);
	AddBoolean("080505", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &SuffixEnable, true, true);
	AddCommand("080506", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP  | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, NULL, &TurnOffFactorySuffix, SUB_TAG_TABLE::CLEAR_PREFIX_SUFFIX_ENTRY);
	AddCommand("080507", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, &TurnOffFactorySuffix, SUB_TAG_TABLE::CLEAR_ALL_PREFIX_SUFFIX_ENTRIES);
	AddBoolean("080700", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &RemoveFncCodes, false, false);
	AddNumber("080800", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &iCodeIdNew, 0, 0, 0, 9);

	AddNumericList("080900", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &SpecifiedDefaultKey, PrimaryKey, PrimaryKey,
				PrimaryKey,AlternateKey1,AlternateKey2,AlternateKey3);

	AddBoolean("080A00", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP  , NULL, NULL, &NoMatchError, false, false);


	//	Output Delay Settings
	AddNumber("080B00", SUB_TAG_TABLE::FULLY_VISIBLE , NULL, NULL, &IntCharDelay, 0, 0, 0, 1000);
	AddNumber("080B01", SUB_TAG_TABLE::FULLY_VISIBLE , NULL, NULL, &IntFuncDelay, 0, 0, 0, 1000);
	AddNumber("080B02", SUB_TAG_TABLE::FULLY_VISIBLE , NULL, NULL, &IntMsgDelay, 0, 0, 0, 1000);
	AddNumber("080B03", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &IntCharXDelay, 0, 0, 0, 1000);
	AddNumber("080B04", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP  | SUB_TAG_TABLE::TWO_DIGIT_HEX, NULL, NULL, &DelayChar, 0, 0, 0, 255);
	AddNumber("080B05", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &SkipDecodingTimeout,0,0,0,30000);
	AddNumber("080B06", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &ReReadDly,750,750,0,30000);
	AddNumber("080B07", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &ReReadDly2D,0,0,0,30000);
	AddNumber("080B08", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &GoodReadDly,0,0,0,30000);


	// OPOS Disable/Enable
	AddBoolean("080C00", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG , NULL, NULL, &bOposMenuEnable, false, false);
	AddBoolean("080C01", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG , NULL, NULL, &bOposAutoDisable, false, false);


	//	SDK Buffered Output
	AddBoolean("080D00", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &BufferedOutput, false, false);
	AddBoolean 	("080E00", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, NULL, NULL, &DecodeHeader, false, false);

	AddNumber("080F00", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &SurePosMaxPacketSize,40,40,20,256);
	AddBoolean("080F01", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP, &ExecuteRetailPDF, NULL, &RetailPDFEnable, false, false);

	AddNumber("080000", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &DataRedundant,0,0,0,3);
	return;
}







