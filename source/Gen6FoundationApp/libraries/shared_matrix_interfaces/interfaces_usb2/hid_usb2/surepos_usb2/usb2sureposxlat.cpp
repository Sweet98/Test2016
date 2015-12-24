/* ==============================================================================
ST3800 IBM-4680 Cash Register Network Command To Scanner Command Translator Source
=================================================================================

   $RCSfile: interfaces_usb2/hid_usb2/surepos_usb2/usb2sureposxlat.cpp $
   $Revision: 1.9.1.5 $
   $Date: 2010/10/07 14:01:19EDT $

//=================================================================================

//=================================================================================*/

/**********************************************************
*
*	 This module is the Interface between
*	 the network commands and the scanner commands.  It
*	 translates the commands from the network into menu
*	 commands the scanner will understand.
*
**********************************************************/

#include "stdInclude.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "interfacetypedefs.h"
#include "barcodedata.h"
#include "usb2.h"
#include "usb2chip.h"
#include "usb2SurePos.h"
#include "usb2hidsurepos.h"
#include "ParseHelper.h"
#include "uchar.h"
#include "usb2pos.h"
#include "DirectIoParser.h"

#define _U(x)	(const bchar_t*)(x)

extern bool IBM4690RetailPDFEnable;
extern int IBM4690SurePosMaxPacketSize;

void CUsb2HidSurePos::SurePosSendStatusReport (SurePosInReportBuffer_t *pInReport)
{	
	pInReport->ClearData();
	pInReport->SetLength(4);
	SendReportWithTest (DATA_INTERFACE, HID_USBPOS_IN_REPORT_SIZE, (UCHAR *)pInReport);
}

void  CUsb2HidSurePos::SurePosFillAndSendStatusReport(SurePosInReportBuffer_t *pInReport)
{
	pInReport->u.report.ucStatus0 = m_ucStatus0;
	pInReport->u.report.ucStatus1 = m_ucStatus1;
	pInReport->u.report.ucStatus2 = 0;
	SurePosSendStatusReport (pInReport);
}

/*
		A quick mention on all things cfg byte related -- specifically those pesky cfg0 through cfg8 which are
		associated with the Configure Scanner and Report Scanner Configuration commands in the SurePos environment.

		The issues are several.  Basically this interface, other than being inflexible and old style, is very cranky
		about reporting and keeping in sync.  Bottom line, these 9 configuration bytes have settings that 'may' associate
		to settings in the reader.  Previous approach was to treat a lot as don't cares and report all was well even when
		we didn't support the functionality.  IBM said this was unacceptable, so the following general rules are now
		implemented in several members below:

		Config bit(s) fall into 1 of 3 categories:
		(1) Supported
 		Nothing else to say about these, other than we are changing the solution from original to always
		read the current product settings when being asked to report, so as to be the most accurate.

		(2) Unsupported
 		If attempted to be set in a way as to indicate something is selected that is not available, these will be
		corrected.  On Configure Scanner, along with being returned to a value that does not indicate this, they will
		result in Config Successful, but Configuration Coerced in Status flag 2.  The devil is always in the details --
		so one of 2 things will happen here.  In the easier case these will be forced to some fixed value that will
		always report unsupported (feature disabled, for example).  In more complicated cases, where some of the settings
		are supported for a given field but not all, this will have to pick a good correction technique (return to what
		it was before, for example).  For the most part, unless unacceptable to IBM these will be 'faked' as best as
		possible so they act -- at least with respect to reporting to the host -- as if they are supported.  Per Bob
		Cato's memo, those fitting the unsupported but able to be treated/faked as supported include those partial
		supported fields like beeper duration, beeper loudness, good read duration.

		(3) Don't cares
 		These relate to functionalities not supported -- or at least not connected between SurePos
		cfg0-8 and product configuration -- and they are for the most part left alone.
*/

void CUsb2HidSurePos::SurePosSendCommandNotSupported (SurePosInReportBuffer_t *pInReport)
{
	pInReport->u.report.ucStatus0 = m_ucStatus0;
	pInReport->u.report.ucStatus1 = m_ucStatus1;
	pInReport->u.report.ucStatus2 = OST2_CONFIG_SUCCESFUL | OST2_CONFIG_COERCED;
	SurePosSendStatusReport (pInReport);
}

void CUsb2HidSurePos::SurePosSendCommandRejectReport (SurePosInReportBuffer_t *pInReport)
{
	pInReport->u.report.ucStatus0 = m_ucStatus0;
	pInReport->u.report.ucStatus1 = m_ucStatus1 | OST1_COMMAND_REJECT;
	pInReport->u.report.ucStatus2 = 0;
	SurePosSendStatusReport (pInReport);
}

void CUsb2HidSurePos::TranslateSurePosSystemCommand(SurePosInReportBuffer_t *pInReport, SurePosOutReportBuffer_t *pOutReport)
{
	switch (pOutReport->u.report.Cmd2)		/* Sort the scanner system command */
	{
		case TEST:
		{
			pInReport->u.report.ucStatus0 = m_ucStatus0 & 0x61;  // also expose the flash status
			pInReport->u.report.ucStatus1 = m_ucStatus1 & 0x03;
			pInReport->u.report.ucStatus2 = 0;
	  		SurePosSendStatusReport (pInReport);
	  		break;
		}
		case STATUS_REQUEST:
		{
	  		SurePosFillAndSendStatusReport (pInReport);
			break;
		}
		case RESET_SCANNER:
		{
			if( !((m_ucFlashStatus0 & FST0_FLASH_IN_PROGRESS) && m_bFlashResetStatus0) )      //We reset anyway , so do not reset during flash   lmc
			{
				ResetDevice();
			}
			break;
		}
		default:
			//SurePosCommandNotSupported (pInReport); // spd changed per EWR
			SurePosSendCommandRejectReport (pInReport);  // leave unknown system commands the way they were -- reject
			break;
	}
}

void CUsb2HidSurePos::TranslateSurePosDirectIoCommand(SurePosInReportBuffer_t *pInReport, SurePosOutReportBuffer_t *pOutReport)
{
   // The Direct I/O command refers to the first 0x30
   // The sub command refers to the next two bytes - dio 0 and dio 1
	int dioResult = dioParser->ExecuteDirectIOCommand(pOutReport->u.bytes.ucArray, HID_SURE_POS_OUT_REPORT_SIZE);
	if( dioResult == DIO_ACCEPTED )
	{
		DirectIOCommandAccepted(pInReport, false); // no data to return
	}
	else if( dioResult == DIO_ACCEPTED_WITH_DATA )
	{
		pInReport->ClearData();
		unsigned char *returnMsg;
		int msgLength = dioParser->GetResponse( &returnMsg );
		// Make sure msgLength does not go beyond the report size. 
		msgLength = (msgLength > HID_SURE_POS_REPORT_UCLABEL_SIZE ? HID_SURE_POS_REPORT_UCLABEL_SIZE : msgLength);
		for(int i = 0; i < msgLength; i++)
		{
			pInReport->u.report.ucLabel[i] = returnMsg[i];
		}
		pInReport->u.report.ucLength = msgLength + 4; // message length plus status bytes and length
		DirectIOCommandAccepted(pInReport, true); // has data to return
	}
	else if( dioResult == DIO_NOT_ALLOWED )
	{
		DirectIOCommandNotAllowed(pInReport);
	}
	else
	{
		DirectIOCommandUndefined(pInReport);
	}
}

void CUsb2HidSurePos::DirectIOCommandUndefined(SurePosInReportBuffer_t *pInReport)
{
	pInReport->u.report.ucStatus0 = m_ucStatus0 & 0x30;
	pInReport->u.report.ucStatus1 = m_ucStatus1 & 0x03;
	pInReport->u.report.ucStatus2 = OST2_DIRECT_IO_UNDEFINED;
	SurePosSendStatusReport (pInReport);
}

void CUsb2HidSurePos::DirectIOCommandAccepted(SurePosInReportBuffer_t *pInReport, bool withData)
{
	pInReport->u.report.ucStatus1 = m_ucStatus1 & 0x03;
	pInReport->u.report.ucStatus2 = OST2_DIRECT_IO_ACCEPTED;
	if( withData )
	{
		pInReport->u.report.ucStatus0 = ((m_ucStatus0 | OST0_DIRECT_IO_RESPONSE) & 0x70);
		int lenToSend = pInReport->u.report.ucLength;
		int sent = 0;
		while( (lenToSend - sent) > 0 )
		{
			// If length left to send is greater than a single packet set length and continuation bytes properly
			if( pInReport->u.report.ucLength > HID_SURE_POS_IN_REPORT_SIZE )
			{
				// Set size and continuation byte
				pInReport->u.report.ucLength = HID_SURE_POS_IN_REPORT_SIZE;
				pInReport->u.report.ucLabel[0] = 0x01;
			}
			else	
			{	// Clear the continuation byte
				pInReport->u.report.ucLabel[0] = 0x00;
			} 
			SendReportWithTest (DATA_INTERFACE, HID_USBPOS_IN_REPORT_SIZE, (UCHAR *)pInReport);
			if( lenToSend - sent > HID_SURE_POS_LABEL_SIZE )
			{
				sent += HID_SURE_POS_LABEL_SIZE;
				// Start at ucLabel + 1 because direct IO commands include the continuation byte (5th byte)
				memcpy(pInReport->u.report.ucLabel+1, pInReport->u.report.ucLabel + sent, HID_SURE_POS_REPORT_UCLABEL_SIZE-sent);
				pInReport->u.report.ucLength = lenToSend - sent;	// update the length
			}
			else
			{
				sent += HID_SURE_POS_LABEL_SIZE;
			}
		}
	}
	else
	{
		pInReport->u.report.ucStatus0 = m_ucStatus0 & 0x30;
		SurePosSendStatusReport (pInReport);
	}
	
}

void CUsb2HidSurePos::DirectIOCommandNotAllowed(SurePosInReportBuffer_t *pInReport)
{
	pInReport->u.report.ucStatus0 = m_ucStatus0 & 0x30;
	pInReport->u.report.ucStatus1 = m_ucStatus1 & 0x03;
	pInReport->u.report.ucStatus2 = OST2_DIRECT_IO_NOT_ALLOWED;
	SurePosSendStatusReport (pInReport);
}

void CUsb2HidSurePos::SurePosConfigTwoLabelFlags(SurePosInReportBuffer_t *pInReport) /* , SurePosOutReportBuffer_t *pOutReport)*/
{
//	UCHAR config = pucCommandString[2+0];
//	if (config & 0x80)
//
//	if (config & 0x40)
// later.....

//spd changed per EWR request by IBM (Bob Cato)
//	PRINT("Two Label Command");
// do not copy the flag config as we don't support
//	memcpy(m_ucTwoLabelFlagConfig, &pOutReport->u.report.ucData[0], TWO_LABEL_CONFIG_SIZE);
	pInReport->u.report.ucStatus0 = m_ucStatus0 | OST0_TWO_LABEL_RESPONSE; //leave as is
	pInReport->u.report.ucStatus1 = m_ucStatus1;
//	pInReport->u.report.ucStatus2 = OST2_TWO_LABEL_SUCCESFUL; // IBM says no we should not success, we should say config success and config coerced but not set the bit
	pInReport->u.report.ucStatus2 = OST2_CONFIG_SUCCESFUL | OST2_CONFIG_COERCED;
	SurePosSendStatusReport (pInReport);
}

void CUsb2HidSurePos::TranslateSurePosCommand(SurePosOutReportBuffer_t *pOutReport)
{
	bool status = FALSE;
	bool btmp=m_IgnoreConfigFlag;

	SurePosInReportBuffer_t *pInReport = (SurePosInReportBuffer_t *)dataToEP;

	switch ( pOutReport->u.report.Cmd1 )		/* Sort the scanner command */
	{
		case SCANNER_ENABLE :			/* Enable Scanner */
		{
	  		m_ucStatus1 |= OST1_SCANNER_ENABLED;
			EnableTx();
			SurePosFillAndSendStatusReport (pInReport);
			m_IgnoreConfigFlag=false;
			StuffInputBuffer("\x02m\r=at:ae;060F0B1!");
			m_IgnoreConfigFlag=btmp;
	  		break;
		}
		case SCANNER_DISABLE :			/* Disable Scanner */
  	  	{
  	  		m_ucStatus1 &= ~OST1_SCANNER_ENABLED;
			DisableTx();
			m_IgnoreConfigFlag=false;
			StuffInputBuffer("\x02m\r=at:ae;060F0B0!");
			m_IgnoreConfigFlag=btmp;
			SurePosFillAndSendStatusReport (pInReport);
  	  		break;
  	  	}
  	  	case BEEPER_ENABLE :				/* Enable Beeper */
  	  	{  	  		
	  		m_ucStatus0 |= OST0_GOOD_READ_BEEP_ENABLED;
			EnableBeeper();
			SurePosFillAndSendStatusReport (pInReport);
  	  		break;
  	  	}
  	  	case BEEPER_DISABLE :			/* Disable Beeper */
  	  	{  	  		
	  		m_ucStatus0 &= ~OST0_GOOD_READ_BEEP_ENABLED;
			DisableBeeper();
			SurePosFillAndSendStatusReport (pInReport);
  	  		break;
  	  	}
  	  	case CONFIGURE_SCANNER:   
  	  	{	  		
			if(m_IgnoreConfigFlag == false)	// allow USB to configure the scanner
  	  		{
				status =  SurePosConfigValidateAndCorrect( pOutReport); // if it came in
				status &= SurePosConfigScanner(pOutReport);
  	  		}
  	  		else	// do not allow, fake out response
  	  		{
  	  			status = true;
  	  		}

			memcpy(m_CurrentConfig, &pOutReport->u.report.ucData[0], ConfigSize);
       			//pInReport->u.report.ucStatus0 = m_ucStatus0 | OST0_CONFIG_RESPONSE;            
  			pInReport->u.report.ucStatus0 = m_ucStatus0 & 0x30;  // Only send OST0_CONFIG_RESPONSE when reporting configuration - JSL  //Status Byte '0' should be all '0' for a sucessful configuration, except bits 4 and 5 which are don't cares
			pInReport->u.report.ucStatus1 = m_ucStatus1 & 0x03;  //Status Byte '1' should be all '0' for a sucessful configuration, except bits 0 and 1 which are don't cares
			pInReport->u.report.ucStatus2 = OST2_CONFIG_SUCCESFUL;
			if (status == FALSE)
			{
				// tell them one of the bits received was not welcome!!  ** those bit(s) drove NO change in the config of the product**
				pInReport->u.report.ucStatus2 |= OST2_CONFIG_COERCED;
			}
			SurePosSendStatusReport (pInReport);			
  	  		break;
  	  	}
  	  	case REPORT_CONFIGURATION:   
  	  	{
			QueryScannerConfig();

			memset(pInReport->u.report.ucLabel, 0, IBM4690SurePosMaxPacketSize-4);
			memcpy(pInReport->u.report.ucLabel, m_CurrentConfig, ConfigSize);
			pInReport->u.report.ucLength = 4+ConfigSize;
			pInReport->u.report.ucStatus0 = m_ucStatus0 | OST0_CONFIG_RESPONSE;
			pInReport->u.report.ucStatus1 = m_ucStatus1;
			pInReport->u.report.ucStatus2 = 0;
			SendReportWithTest (DATA_INTERFACE, HID_USBPOS_IN_REPORT_SIZE, (UCHAR *)pInReport);
  	  		break;
  	  	}
  	  	case CONFIGURE_TWO_LABEL:
  	  	{
			SurePosConfigTwoLabelFlags( pInReport); /*, pOutReport );*/  	  			// get rid of errors because pOutReport not used
		 	break;
  	  	}
  	  	case REPORT_TWO_LABEL:
  	  	{
			memset(pInReport->u.report.ucLabel, 0, IBM4690SurePosMaxPacketSize-4);
			memcpy(pInReport->u.report.ucLabel, m_ucTwoLabelFlagConfig, TWO_LABEL_CONFIG_SIZE);
			pInReport->u.report.ucLength = 4+TWO_LABEL_CONFIG_SIZE;
			pInReport->u.report.ucStatus0 = m_ucStatus0 | OST0_TWO_LABEL_RESPONSE;
			pInReport->u.report.ucStatus1 = m_ucStatus1;
			pInReport->u.report.ucStatus2 = 0;
			SendReportWithTest (DATA_INTERFACE, HID_USBPOS_IN_REPORT_SIZE, (UCHAR *)pInReport);
			break;
  	  	}
  	  	case SYSTEM_COMMAND:
  	  	{
			TranslateSurePosSystemCommand( pInReport, pOutReport );
  	  		break;
  	  	}

		// We now support DIRECT_IO, so we can enable additional symbologies, including 2D and PDF
  	  	case DIRECT_IO:  
  	  	{
			TranslateSurePosDirectIoCommand( pInReport, pOutReport );
			break;
  	  	}

		default:
			SurePosSendCommandNotSupported (pInReport); // spd changed per EWR
//			SurePosSendCommandRejectReport (pInReport);
			break;
	}		/* Command handling complete! */
	return;
}

bool CUsb2HidSurePos::SurePosConfigValidateAndCorrect (SurePosOutReportBuffer_t *pOutReport)
{
	UCHAR config, config2;
	bool status=true;

// cfg0 , only check for 0, some question if we should check for UPC-D, but for now just clear/set based on input
	config = pOutReport->u.report.ucData[0];

	// 00 is not valid for this config byte
	if (config == 0)
	{
		status = false;
		pOutReport->u.report.ucData[0] = 0xFD; // nah! just force 'em all on
	}

	// C128 and EAN128 must both be set to the same value (bits 6 and 7).             //removed for cvs   LMC
	config2 = (config<<1) & 0x80;
	if ((config&0x80) ^ config2)
	{
		status = false;
		pOutReport->u.report.ucData[0] &= ~0xC0;
		if((config&0x80) || (config&0x40))
			pOutReport->u.report.ucData[0] |= 0xC0;  // if one bit is on, correct to "both on".
	}

	// UPC-D not supported
	if (config & 0x02)
	{
		status = false;
		pOutReport->u.report.ucData[0] &= ~0x02;
	}

// cfg1, no check, all bits are valid

// cfg2,
	config = pOutReport->u.report.ucData[2];
	if (config & 0x37)
	{
		status = FALSE;
		pOutReport->u.report.ucData[2] &= ~0x37; 		// No need to remember previous state... just make it so!
	}


// cfg3
	config = pOutReport->u.report.ucData[3];
	if (config & 0x40)	// 0x10 and 0x11 is not supported -- long, longest duration
	{
		status = FALSE;
		pOutReport->u.report.ucData[3] &= ~0x60;
		pOutReport->u.report.ucData[3] |= 0x20;  // set to medium (= bib-mode off)
	}

// cfg4
// completely not supported - Laser/motor control
	config = pOutReport->u.report.ucData[4];
	if (config)
	{
		status = FALSE;
		pOutReport->u.report.ucData[4] = 0;
	}


// cfg5
// completely not supported - Security level
	config = pOutReport->u.report.ucData[5];
	if (config)
	{
		status = FALSE;
		pOutReport->u.report.ucData[5] = 0;
	}

// cfg6
	config = pOutReport->u.report.ucData[6];
	config2 = pOutReport->u.report.ucData[7]; // these travel together
	if ((config & 0xc0) == 0x40) // the non supported is 2 lengths, discrete lengths, we don't support
	{
		status = FALSE;
		pOutReport->u.report.ucData[6] |= 0x80;	// let the wanting to set 2 lengths rule here... this was a toss up
	}

// cfg7 (or really crossover of cfg6 and cfg7)
	if ((config & 0x40) == 0)	// if bit 7 clear, 1 length , so make sure the 2nd length is 0
	{
		if ((config2&0x3f) != 0)
		{
			status = FALSE;
			pOutReport->u.report.ucData[7] &= ~0x3f;
		}
	}


// cfg8
//	config = pOutReport->u.report.ucData[8];

//bit 3 and 4 real don't cares... (or don't knows)... no errors generated and they are left alone by me - Laser/volume switch enable/disable
//bit 2 is legite - Barcode menuing Enable/disable
//bits 0 and 1 we 'fake' but they are don't cares  - LED good read duration

	return status;
}

void CUsb2HidSurePos::AppendToMenuBuffer(const bchar_t *pFormat, ...)
{
	va_list	pArgs;
	int written=0;
	
	size_t BufferSize = MENU_BUFFER_SIZE-m_CurrentMenuIndex;
	va_start( pArgs, pFormat );
	written = vsnprintf(m_MenuBuffer+m_CurrentMenuIndex, BufferSize, pFormat, pArgs );
	if(written>0)
		m_CurrentMenuIndex += written;

	va_end(pArgs);	
}

void CUsb2HidSurePos::AppendCmdToMenuBuffer(const bchar_t *pCommand, const bchar_t *pAction)
{
	AppendToMenuBuffer(_U("%s%s"), pCommand, pAction);
}

void CUsb2HidSurePos::AppendQueryCmdToMenuBuffer(const bchar_t *pCommand)
{
	AppendCmdToMenuBuffer(pCommand, _U("?;"));
}


// connect cfg settings with reader settings
// note, there is a member above to validate and correct "Unsupported" cfg fields
// note 2, while the validate and correct member above handles "Unsupported", this member handles "Don't cares" and "Fake" as the latter
// 		requires that a configuration setting be set to something, and that only happens in this member.
bool CUsb2HidSurePos::SurePosConfigScanner(SurePosOutReportBuffer_t *pOutReport)
{
	UCHAR config;
	int temp;

	ResetMenuBuffer();

	// cmd header
	AppendToMenuBuffer(_U("\x02M\r"));

	// ----- cfg0 ------------------------------------------------------------------------------------------------------
	config =  pOutReport->u.report.ucData[0];
	// Ean 128 is NOT configurable independed of C128. So we use both bits here.
	//AppendToMenuBuffer(_U("GS1ENA%i;") , (config & 0x80) ? 1 : 0);                     //added for cvs   LMC
	//AppendToMenuBuffer(_U("128ENA%i;") , (config & 0x40) ? 1 : 0);                     //added for cvs   LMC
	AppendToMenuBuffer(_U("020A01%i;") , (config & 0xC0) ? 1 : 0);           //standard product combines 128's, bits 6 and 7
	AppendToMenuBuffer(_U("020D01%i;") , (config & 0x20) ? 1 : 0);
	AppendToMenuBuffer(_U("020201%i;") , (config & 0x10) ? 1 : 0);
	AppendToMenuBuffer(_U("020401%i;") , (config & 0x08) ? 1 : 0);
	AppendToMenuBuffer(_U("020301%i;") , (config & 0x04) ? 1 : 0);
	//	No support for UPC-D in this scanner							  (config & 0x02) ? 1 : 0);		// but it's a don't care	
	temp = (config & 0x01) ? 1 : 0;
	AppendToMenuBuffer(_U("021101%i;021201%i;"), temp, temp);
	AppendToMenuBuffer(_U("021401%i;021301%i;"), temp, temp);

	// ----- cfg1 ------------------------------------------------------------------------------------------------------
	config = pOutReport->u.report.ucData[1];
	AppendToMenuBuffer(_U("020601%i;") , (config & 0x80) ? 1 : 0);          //Added support for standard 2of5   LMC
	AppendToMenuBuffer(_U("020402%i;") , (config & 0x40) ? 2 : 0);
	AppendToMenuBuffer(_U("020304%i;") , (config & 0x20) ? 2 : 0);
	AppendToMenuBuffer(_U("021204%i;") , (config & 0x10) ? 1 : 0);
	AppendToMenuBuffer(_U("021102%i;") , (config & 0x08) ? 1 : 0);
	AppendToMenuBuffer(_U("021500%i;") , (config & 0x04) ? 1 : 0); //C128 supplements -- I think this is Coupon Codes
	temp = (config & 0x02) ? 1 : 0;
	AppendToMenuBuffer(_U("021304%i;021404%i;021207%i;021105%i;"), temp, temp, temp, temp);
	temp = (config & 0x01) ? 1 : 0;
	AppendToMenuBuffer(_U("021303%i;021403%i;021206%i;021104%i;"), temp, temp, temp, temp);

	// ----- cfg2 ------------------------------------------------------------------------------------------------------
	/* gets ignored  ... it shouldn't */
	config = pOutReport->u.report.ucData[2];		// if unsupported bits are written to, say coerced when all is said and done
	// bit 7 and 6 are reserved // don't care 0x80, 0x40
	// bits 5, 4, 2, 1, 0 were handled in the correct function, should all be 0s
	// bit 3
 	AppendToMenuBuffer(_U("021203%i;"),(config & 0x08) ? 1 : 0);


	// ----- cfg3 ------------------------------------------------------------------------------------------------------
	config = pOutReport->u.report.ucData[3];
	// bit 7 , reserved, don't care
	// bits 5,6 are supported or 'faked'

	//if (config & 0x60)	// we only have short and medium
	//	temp = 0;             							// medium
	//else
	//	temp = 1;	          							// short
	temp = ((config & 0x20) >> 5);			//lmc - only support short and medium, long and longest are converted to medium in SurePosConfigValidateAndCorrect
	if(temp == 0)  temp = 1;                 		// short    (== bib-mode on)
	else 				temp = 0;                    	// medium  (== bib-mode off)

	if(!m_IgnoreBeepFlag) AppendToMenuBuffer(_U("050216%i;") , temp);


// bits 3,4 full supported
	temp = ((config & 0x18) >> 3);
	switch (temp)
	{
		case (0):      //lowest freq by spec
			temp = 800;		  //lowest (error) beep freq
			//temp = 3250;        //lmc - 800 is used in standard product, changed to medium for CVS custom
			break;
		case (1):      //low freq by spec
			temp = 1600;        //lowest supported on our good read pitch menu
			//temp = 3250;            //lmc - 1600 is used in standard product, changed to medium for CVS custom
			break;
		case (2):      //med freq by spec
			temp = 3250;
			break;
		case (3):      //high freq by spec
			temp = 4200;   	//highest supported on our good read pitch menu
			break;
		default:
			temp = 3250;   //med freq
			break;
	}
	if(!m_IgnoreBeepFlag) AppendToMenuBuffer(_U("05020D%i;"), temp);

	// bits 1,2 are supported or 'faked'      //lmc - 0==off, 1==low, 2==med, 3==high, but by spec, '0' should be very low - not supported
	temp = ((config & 0x06) >> 1);
	if(!m_IgnoreBeepFlag) AppendToMenuBuffer(_U("05021D%i;") , temp); // yes 0 (Lowest) will be Off .. and accomplish the same as bit 0!!

	// bit 0
//   AppendToMenuBuffer(_U("BEPDEC%i;") , (config & 0x01) ? 1 : 0);
   // Disabled BEPDEC configuration as this beep setting was causing the scanner to beep while Disabled.
   // This change was tested in Mark's Retail lab to verify for no adverse effects.
   // A future project will evaluate the scanner against the SurePos spec in detail which has never been done.
   AppendToMenuBuffer(_U("050214%i;") , (config & 0x01) ? 0 : 0);

	// ----- cfg4 ------------------------------------------------------------------------------------------------------
	// Unsupported ALL handled elsewhere
	//	temp = (config & 0x60) >> 5;
	//	AppendToMenuBuffer(_U("RRDDLY%i;") , temp); // wrong command ... it is now DLYRRD, and it only applies to triggering modes not implied in the spec
	//	PRINT(m_TempBuffer);

	// ----- cfg5 ------------------------------------------------------------------------------------------------------
	// Unsupported ALL handled elsewhere

	// ----- cfg6 ------------------------------------------------------------------------------------------------------
	int i25min, i25max;
	i25min = pOutReport->u.report.ucData[6] & 0x3F;
	i25max = pOutReport->u.report.ucData[7] & 0x3F;

	config = pOutReport->u.report.ucData[6];
	temp	 = pOutReport->u.report.ucData[7] & 0x3F;

	temp = pOutReport->u.report.ucData[6]>>6;
	temp &= 0x3;
	switch (temp)
	{

		case (0): // 00xxxxxx discrete, 1 length -- msb is meaningless, 1 length rules
		case (2): // 10xxxxxx range, 1 length	-- msb is meaningless, 1 length rules
				i25max = i25min;
				break;
		case (1): // 01xxxxxx discrete, 2 length -- should have been corrected above
				break;
		case (3): // 11xxxxxx range, 2 length
		default:
				if (i25min>i25max) // adjust for which one is bigger
				{
 					temp = i25max;
 					i25max = i25min;
 					i25min = temp;
				}
				break;

	}
	AppendToMenuBuffer(_U("020403%i;") , i25min);
	AppendToMenuBuffer(_U("020404%i;") , i25max);

	// ----- cfg8 ------------------------------------------------------------------------------------------------------
	config = pOutReport->u.report.ucData[8];
	// bits 3 and 4 left alone for now
 	AppendToMenuBuffer(_U("0D0602%i;!"), (config & 0x04) ? 1 : 0 ); 			// this could be dangerous
 	// bit 0 is NOT supported after all, the BEPLED command beeps in place of the LED?!?!?!?!
 	// AppendToMenuBuffer(_U("BEPLED%i!", ((config & 0x03) == 0) ? 1 : 0 ); 	// anything other than 0 sets good read led

	// m_MenuBuffer is modified by Append to menu buffer throughout the function
	StuffInputBuffer(m_MenuBuffer);
	// Return true, we've removed any values that could cause us to return a coerced configuration
	return true;
}


// Get the current menu setting.
void CUsb2HidSurePos::QueryScannerConfig(void)
{
	ResetMenuBuffer();

//cmd header
	AppendToMenuBuffer((const bchar_t*)"\x02M\r"
	/* cfg0 */
	// Ean 128 is always enabled, it is NOT configurable  - this is changing
	"020A01?;020D01?;020201?;020401?;020301?;021101?;021201?;021401?;021301?;"            //standard product does not program 128's separately
	//"GS1ENA?;128ENA?;C93ENA?;CBRENA?;I25ENA?;C39ENA?;UPAENA?;UPEEN0?;EA8ENA?;E13ENA?;"           //added for cvs - gs1ena   LMC
	/* cfg1 */
	"020601?;020402?;020304?;021204?;021102?;021500?;021304?;021404?;021207?;021105?;021303?;021403?;021206?;021104?;"
	/* cfg2 */
 	"021203?;"
	/* cfg3 */
	"050216?;05020D?;05021D?;050214?;"
	/* cfg4 */
	/* cfg5 */
	/* cfg6 */
	"020403?;020404?;"
	// cfg8
 	"0D0602?;"
	// DirectIO - cfg9
#if (!defined(XENON_LITE_1500)) && (!defined(HI2D_T))	/* Maxicode is removed for Xenon Lite(1500) and HI2D*/
	"024501?;023401?;021F01?;022101?;021B01?;021A01?;021901?;"
#else
	"024501?;021F01?;022101?;021B01?;021A01?;021901?;"
#endif
	"!");

	TalkToReaderConfig();
}


const bchar_t *RssEnableUSB2[] =
{
	_U("021A01"),
	_U("021901"),
};


const bchar_t *UpcEanEnableUSB2[] =
{
	_U("021101"),
	_U("021201"),
	_U("021401"),
	_U("021301"),
};

const bchar_t *Adendum5EnableUSB2[] =
{
	_U("021304"),
	_U("021404"),
	_U("021207"),
	_U("021105"),
};

const bchar_t *Adendum2EnableUSB2[] =
{
	_U("021303"),
	_U("021403"),
	_U("021206"),
	_U("021104"),
};


// Here we parse the resoponse from the menu system
// after we either queried the current settings or changed the settings.
// The result of the parsing will be stored in the supplied pointer to a SurePos config buffer.
// All bits in the config that are not corrosponding to our menusetting are left alone.
bool CUsb2HidSurePos::ParseMenuResponse(bchar_t *pConfig)
{
	ULONG temp;
	bool ret=PASS;

	size_t Size=0;
	const bchar_t *pResponse = NULL;  
	CBarcodeData *pMenuResponse = GetMenuResponse();
	if(pMenuResponse==NULL)
	{
		return FAIL;
	}
	pMenuResponse->GetTotalData(pResponse, Size);
	DebugSend(pResponse, Size);

	if(Size==0)
	{
		return FAIL;
	}

	// cfg0 
	// Ean 128 is same as C128
	//ret &= ParseMenuResponse_bool(pResponse, _U("GS1ENA"), pConfig[0],0x80);              //added for cvs   LMC
	//ret &= ParseMenuResponse_bool(pResponse, _U("128ENA"), pConfig[0],0x40);              //added for cvs   LMC
	ret &= ParseMenuResponse_bool(pResponse, _U("020A01"), pConfig[0],0xC0);       //standard product combines 128's, bits 6 and 7
	ret &= ParseMenuResponse_bool(pResponse, _U("020D01"), pConfig[0],0x20);
	ret &= ParseMenuResponse_bool(pResponse, _U("020201"), pConfig[0],0x10);
	ret &= ParseMenuResponse_bool(pResponse, _U("020401"), pConfig[0],0x08);
	ret &= ParseMenuResponse_bool(pResponse, _U("020301"), pConfig[0],0x04);
//	No support for UPC-D in this scanner  (bit 2, 0x20)
	temp=0;
	ret &= ParseMenuResponse_bool(pResponse, UpcEanEnableUSB2, 4, pConfig[0],0x01);

	// cfg1 
	temp=0;
	ret &= CParseHelper::Extract(pResponse, _U("020402"), temp);
	pConfig[1] &= ~0x40;
	if(temp==2)
	{
		pConfig[1] |= 0x40;
	}
	temp=0;
	ret &= CParseHelper::Extract(pResponse, _U("020304"), temp);
	pConfig[1] &= ~0x20;
	if(temp==2)
	{
		pConfig[1] |= 0x20;
	}
	ret &= ParseMenuResponse_bool(pResponse, _U("021204"), pConfig[1],0x10);
	ret &= ParseMenuResponse_bool(pResponse, _U("021102"), pConfig[1],0x08);
	ret &= ParseMenuResponse_bool(pResponse, _U("021500"), pConfig[1],0x04); //C128 supplements -- I think this is Coupon Codes

	ret &= ParseMenuResponse_bool(pResponse, Adendum5EnableUSB2, 4, pConfig[1], 0x02);
	ret &= ParseMenuResponse_bool(pResponse, Adendum2EnableUSB2, 4, pConfig[1], 0x01);

	// cfg2 
 	ret &= CParseHelper::Extract(pResponse, _U("021203"), temp);
// what about the other bits???

	// cfg3 
	temp = 0;
	ret &= CParseHelper::Extract(pResponse, _U("050216"), temp);
	pConfig[3] &= ~0x60;
	if(temp == 0)
	{
		pConfig[3] |= 0x20;
	}

	temp = 0;
	ret &= CParseHelper::Extract(pResponse, _U("05020D"), temp);
	pConfig[3] &= ~0x18;
	if(temp > 3500)
	{
		pConfig[3] |= 0x18;					// highest
	}
	else if(temp > 2500)
	{
		pConfig[3] |= 0x10;					// medium
	}
	else if(temp > 1000)
	{
		pConfig[3] |= 0x08;					// low
	}

	ret &= ParseMenuResponse_int(pResponse, _U("05021D"), pConfig[3], 0x06);
	ret &= ParseMenuResponse_bool(pResponse, _U("050214"), pConfig[3], 0x01);
	if(pConfig[3] & 0x01)	               // beeper flag copy to status
		m_ucStatus0 |= OST0_GOOD_READ_BEEP_ENABLED;
	else
		m_ucStatus0 &= ~OST0_GOOD_READ_BEEP_ENABLED;

	// cfg4
//	AppendToMenuBuffer(_U("RRDDLY%i;") , temp); // wrong command ... it is now DLYRRD, and it only applies to triggering modes not implied in the spec
	// cfg5 
// Unsupported ALL handled elsewhere

	// cfg 6 and 7 
	ret &= ParseMenuResponse_int(pResponse, _U("020403"), pConfig[6], 0x3f);
	ret &= ParseMenuResponse_int(pResponse, _U("020404"), pConfig[7], 0x3f);

	// cfg8 
 	ret &= ParseMenuResponse_bool(pResponse, _U("0D0602"), pConfig[8], 0x04);

	// direct io cfg 9 
	temp=0;
	//pConfig[0],0x80);          Bit 7 reserved
	//pConfig[0],0x40);          Bit 6 reserved
	ret &= CParseHelper::Extract(pResponse, _U("024501"), temp);         //Bit 5
	pConfig[9] &= ~0x20;
	if(temp>0)                     //if any OCRs are enabled, then indicate config enabled
	{
		pConfig[9] |= 0x20;
	}

#if (!defined(XENON_LITE_1500)) && (!defined(HI2D_T))	/* Maxicode is removed for Xenon Lite(1500) and HI2D*/
	ret &= ParseMenuResponse_bool(pResponse, _U("023401"), pConfig[9],0x10);
#endif
	ret &= ParseMenuResponse_bool(pResponse, _U("021F01"), pConfig[9],0x08);
	ret &= ParseMenuResponse_bool(pResponse, _U("022101"), pConfig[9],0x04);
	ret &= ParseMenuResponse_bool(pResponse, _U("021B01"), pConfig[9],0x02);
	temp=0;
	ret &= ParseMenuResponse_bool(pResponse, RssEnableUSB2, 2, pConfig[9],0x01);

	return ret;
}

// Send the m_MenuBuffer to the menu via the InputBuffer and wait for the response.
// Then Parse the response.
bool CUsb2HidSurePos::TalkToReaderConfig(void)
{
	bool RetVal=true;

	if(m_IgnoreConfigFlag == FALSE)
	{		
		if (m_CurrentMenuIndex >= (MENU_BUFFER_SIZE-1))  //  check for buffer overflow
		{
			m_ucStatus0 |= OST0_HARDWARE_ERROR;				// this can only be fixed by increasing buffer and recompile
			RetVal=false;
		}
 		else
 		{
			DebugSend(m_MenuBuffer, m_CurrentMenuIndex);
			StuffInputBufferAndWaitForMenuResponse( m_MenuBuffer );
			RetVal = ParseMenuResponse(m_CurrentConfig);
			FreeMenuResponse();					
		}
	}
	return RetVal;
}

////////////////////////////////////////////////////
// Some helpers that parse parts of a menu response.
// Store the resultig bool as a bit into the supplied paramter "Byte" at the place marked by "Mask".
bool CUsb2HidSurePos::ParseMenuResponse_bool(const tchar_t *pResponse, const tchar_t *pName, UCHAR &Byte, UCHAR Mask)
{
	bool ret;
	bool temp=0;

	ret = CParseHelper::Extract(pResponse, pName, temp);
	if(ret)
	{
		Byte &= ~Mask;
		Byte |= temp ? Mask : 0;
	}
	return ret;
}

// We use the following function to translate multiple settings in the reader configuration into a single setting in
// the SurePos configuration.
// Rules:
// 1.	All settings must in the the response, otherwise we don't even touch the SurePos side.
// 2. All settings in the list must be set, to set the SurePos side (logical AND function)
bool CUsb2HidSurePos::ParseMenuResponse_bool(const tchar_t *pResponse, const tchar_t *pName[], int count, UCHAR &Byte, UCHAR Mask)
{
	bool ret=true;
	bool temp = 0;
	int NumOnes=0;

	for(int i=0; i<count; i++)
	{
		ret &= CParseHelper::Extract(pResponse, pName[i], temp);
		if(ret && temp)
		{
			NumOnes++;
		}
	}

	if(ret)
	{
		Byte &= ~Mask;
		if(NumOnes == count)
		{
			Byte |= temp ? Mask : 0;
		}
	}

	return ret;
}

bool CUsb2HidSurePos::ParseMenuResponse_int(const tchar_t *pResponse, const tchar_t *pName, UCHAR &Byte, UCHAR Mask)
{
	bool ret;
	int temp;

	// we must find how often we need to shift
	int shift;
	temp = Mask;

	for(shift=0; shift<8; shift++)
	{
		if(temp & 0x01)
		{
			break;
		}
		temp = temp>>1;
	}

	temp=0;
	ret = CParseHelper::Extract(pResponse, pName, temp);  // read the setting
	if(ret)
	{
		Byte &= ~Mask;
		Byte |= (temp<<shift)&Mask;     // shift into right position and store it into SurePos config
	}
	return ret;
}

//#define SUREPOS_MENU_DEBUG
#ifdef SUREPOS_MENU_DEBUG

void CUsb2HidSurePos::DebugSend(const bchar_t *pBin, size_t Size)
{
	SurePosInReportBuffer_t *pInReport = GetInReportBuffer();
	int rest=Size;
	const int POSTFIX_SIZE=3;
	
	int BlockSize=HID_SURE_POS_LABEL_SIZE-POSTFIX_SIZE;
	while(rest >0)
	{
		if(rest < BlockSize)
			BlockSize = rest;

		pInReport->Clear();
		pInReport->CopyData(pBin, BlockSize);
		pInReport->SetLength(BlockSize+4);
		pInReport->AddByte(0x00);
		pInReport->AddByte(0x18);
		pInReport->AddByte(0x0B);
		SendReportWithTest (DATA_INTERFACE, HID_SURE_POS_IN_REPORT_SIZE, (UCHAR *)pInReport);
		rest -= BlockSize;
		pBin += BlockSize;
		LockInBuffer(DATA_INTERFACE);		// wait until buffer is available
	}
}
#else
void CUsb2HidSurePos::DebugSend(const bchar_t *, size_t )
{
}
#endif

#ifdef RFBASE
#else
void CUsb2HidSurePos::SendOneMenu(const bchar_t *){}
#endif


