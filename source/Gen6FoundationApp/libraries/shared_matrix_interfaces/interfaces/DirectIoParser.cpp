/*=============================================================================
 * This file contains functions that implement direct IO commands for 4683
 *

 *=============================================================================
 * $RCSfile: interfaces/4683DirectIO.cpp $
 * $Revision: 1.3.2.3.1.12 $
 * $Date: 2010/11/08 14:02:00EST $
 *============================================================================*/

#include "beep.h"
#include "DirectIoParser.h"
#include "SYNFECommandHandler.h"
#include "ParseHelper.h"
#include "DrvMenuConfig.h"
#include "RxFifo.h"
#include "menu.h"
#include "ReaderConfiguration.h"
#include "ProductConfiguration.h"
#include "SystemMenuSettings.h"
#include "RetailAppFlash.h"
#include "db_hdw.h" 		// for reset to boot code
#include "FileHandler.h"	// mocf 
#include "StartupFlashDisk.h"	// Append and Close File
#include "InterfaceMenuSettings.h"

// Declare the m_flash structure
volatile flash4683_t	m_flash4683;
volatile tfData_t		m_tfData;

#if WALMART_DEBUG
#define WALMART_DEBUG_FILE "WmtDbg"
void AddStringToLogFile( const char* msg, int length )
{
	AppendAndCloseFile(WALMART_DEBUG_FILE, (void *)msg, length);
}
#else
void AddStringToLogFile( const char *, int)
{}
#endif

DirectIoParser::DirectIoParser(CRxFifo *fifo)
{
	DioMessage[0] = NULL;
	DioDataLength = 0;
	configHelper = new DrvMenuConfig();
	DirectIOResponseEvent = MT_CreateEvent(false);
	m_flash4683.flashBufferLength = 0;
	m_flash4683.flashStatus = NOT_FLASH_MODE;
	m_flash4683.flashBuffer = NULL;
	m_flash4683.tempFlashPtr = NULL;
	m_flash4683.endFlashPtr = NULL;
	DioStatus = DIO_UNDEFINED;
	m_ExpectingMenuResponse = false;
	RxFifo = fifo;
}

DirectIoParser::~DirectIoParser(void)
{
	if( configHelper != NULL )
	{
		delete configHelper;
		configHelper = NULL;
	}
	MT_CloseEvent(DirectIOResponseEvent);
}

// The first byte SYSTEM_CHAR1 is a 0x30 or a 0x35
// The Length argument should always be used, since there are now
// short and long direct IO commands.
// Return value is set by SetDioStatus()
char DirectIoParser::ExecuteDirectIOCommand(unsigned char * Message, int Length)
{
	m_flash4683.flashTimeout = TIMER_RESET_VALUE;
	// Handle possible sub commands with SYSTEM_CHAR2
	// Including:	0x30 - Non flash commands
	// 				0x1B - Flash Data
	// 				0x1D - Flash Commands
	switch( Message[DIO_CHAR2] )
	{
		case NON_FLASH_COMMAND:							// 0x30 0x30
			if ( Message[DIO_CHAR3] == DIO_A_SERIES )// 0x30 0x30 0x41
			{
				switch ( Message[DIO_CHAR4] ) 
				{
					case FULL_UNIT_ID:			// 0x30
						DIO_FullUnitID();
						break;
					case REPORT_CONFIGURATION:	// 0x33
						DIO_ReportConfiguration();
						break;
					case FW_NEW_VERSION_REPORT:	// 0x34
						DIO_FWNewVersionReport();
						break;
					case FW_VERSION_REPORT:		// 0x36
						DIO_FWVersionReport();
						break;
					case SERIAL_NUMBER_REPORT:	// 0x35
						DIO_SerialNumberReport();
						break;
					default:
						SetDioStatus(DIO_UNDEFINED);
						break;
				}
			}
			else if( Message[DIO_CHAR3] == DIO_H_SERIES &&	// 0x30 0x30 0x48
					 Message[DIO_CHAR4] == DIO_0H0_COMMAND )	// 0x30
			{
				DIO_0H0Command();
			}
			else if ( Message[DIO_CHAR3] == DIO_F_SERIES && 	// 0x30 0x30 0x46
					  Message[DIO_CHAR4] == BEEP_4683 ) 		// 0x32
			{
				DIO_Beep4683();
			}
			else if( Message[DIO_CHAR3] == DIO_t_SERIES )	// 0x30 0x74
			{
				switch( Message[DIO_CHAR4] )
				{
				case ENTER_TRICKLE_FEED:		// 0x01
					TF_EnterTrickleFeed();
					break;
				case GET_DOWNLOAD_OFFSET:		// 0x02
					TF_GetDownloadOffset();
					break;
				case TRICKLE_FEED_COMPLETE:		//0x04
					TF_TrickleFeedComplete();
					break;
				case PAUSE_TRICKLE_FEED:		// 0x05
					TF_PauseTrickleFeed();
					break;
				case TERMINATE_TRICKLE_FEED:	// 0x06
					TF_TerminateTrickleFeed();
					break;
				case ACTIVATE_NEW_FIRMWARE:		// 0x07
					TF_ActivateNewFirmware();
					break;
				default:
					SetDioStatus(DIO_UNDEFINED);
					break;
				}
			}
			else
			{
				SetDioStatus(DIO_UNDEFINED);
			}		
			break;
		case FLASH_COMMAND:		// 0x30 0x1D
			switch( Message[DIO_CHAR3] )
			{
				case ENTER_FLASH_MODE:			// 0x00
					DIO_EnterFlashMode();
					break;
				case REQUEST_PART_NUMBER:		// 0x01
					DIO_RequestPartNumber();
					break;
				case REQUEST_PERIPHERAL_ID:		// 0x02
					DIO_RequestPeripheralID();
					break;
				case SELECT_FLASH_SECTION:		// 0x03
					DIO_SelectFlashSection(Message);
					break;
				case GET_CRC_16:				// 0x04
					DIO_GetCRC16();
					break;
				case GET_CRC_32:				// 0x0A
					DIO_GetCRC32();
					break;
				case ERASE_SELECTED_SECTION:	// 0x05
					DIO_EraseSelectedSection();
					break;
				case SELECT_FLASH_OFFSET:		// 0x06
					DIO_SelectFlashOffset(Message);
					break;
				case UPDATE_DEVICE:				// 0x07
					DIO_UpdateDevice();
					break;
				case RESET_DEVICE:				// 0x09
					DIO_ResetDevice();
					break;
				case REQUEST_SCANNER_FIRMWARE_VERSION:	// 0x0C
					DIO_RequestScannerFirmwareVersion();
					break;
				case INCREMENT_FIRMWARE_FLASH_COUNTER:	// 0x0B
					DIO_IncrementFirmwareFlashCounter();
					break;
				default:
					SetDioStatus(DIO_UNDEFINED);
					break;
			}
			break;
		case FLASH_DATA:		// 0x30 0x1B
			DIO_FlashData( Message, Length );
			break;
		case DIO_A_SERIES:		// 0x30 0x41
			if( Message[DIO_CHAR3] == SEND_CONFIGURATION &&
				Message[DIO_CHAR4] == SEND_CONFIGURATION_A )
			{
				DIO_SendConfigurationA(Message, Length);
			}
			else if( Message[DIO_CHAR3] == SEND_CONFIGURATION &&
				Message[DIO_CHAR4] == SEND_CONFIGURATION_B )
			{
				DIO_SendConfigurationB(Message, Length);
			}
			else
			{
				SetDioStatus(DIO_UNDEFINED);
			}
			break;
		case IBM_DIO_RESERVED:	// 0xFF
			switch( Message[DIO_CHAR3] )
			{
			case IBM_DIO_CONFIG_SYM:	// 0xFF
				IbmEnableAdditionalSymbologies(Message);
				break;
			case IBM_DIO_DEV_INFO:		// 0xFE
				IbmGetDeviceInformaiton();
				break;
			default:
				SetDioStatus(DIO_UNDEFINED);
				break;
			}
			break;
		default:
			SetDioStatus(DIO_UNDEFINED);
			break;
	}
	return DioStatus;
}

// Set the DioStatus to accepted, undefined, or not allowed
// this will be returned to the calling function
void DirectIoParser::SetDioStatus( int stat )
{
	DioStatus = stat;
}

int DirectIoParser::GetResponse( unsigned char **result )
{
	*result = DioMessage;
	return DioDataLength;
}

void DirectIoParser::IbmEnableAdditionalSymbologies( unsigned char *message )
{
	SetDioStatus(DIO_ACCEPTED);
	unsigned char config;
	int temp;

	configHelper->ResetMenuBuffer();
	// cmd header
	configHelper->AppendToMenuBuffer(_U("\x02M\r"));
	// Sym 0 - GS1 Databar, GS1 Exp, GS1 Composite, PDF417, MaxiCode, OCR
	config =  message[SYM_0];
	temp = (config & 0x01) ? 1 : 0;
	configHelper->AppendToMenuBuffer(_U("021A01%i;021901%i;"), temp, temp);
	configHelper->AppendToMenuBuffer(_U("021B01%i;") , (config & 0x02) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("022101%i;") , (config & 0x04) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("021F01%i;") , (config & 0x08) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("023401%i;") , (config & 0x10) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("024501%i;") , (config & 0x20) ? 1 : 0);
	// Sym 1 - Data Matrix, Aztec, QR Code, Code 49 (unsupported)
	configHelper->AppendToMenuBuffer(_U("023601%i;") , (config & 0x01) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("023301%i;") , (config & 0x02) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("023701%i;") , (config & 0x04) ? 1 : 0);
	// Set temporary flag
	configHelper->AppendToMenuBuffer(_U("!"));
	// Send to menu
	TalkToReaderConfig(1);
}

void DirectIoParser::IbmGetDeviceInformaiton(void)
{
	SetDioStatus(DIO_ACCEPTED_WITH_DATA);
	// set continuation to zero
	DioMessage[0] = 0x00;
	// set vendor ID to 0x0007
	DioMessage[1] = 0x00;
	DioMessage[2] = 0x07;
	DioDataLength = 3;
}

char DirectIoParser::DIO_EnterFlashMode()
{
	SetDioStatus(DIO_ACCEPTED_WITH_DATA);
	//add fourth status byte (length set at the bottom)
	DioMessage[0] = 0x00;
	//determine if its in flash mode to set 5th bit
	if ( m_flash4683.flashStatus == NOT_FLASH_MODE )
	{
		m_flash4683.flashStatus = FLASH_MODE;
		DioMessage[1] = 0x05;
		DioMessage[2] = 0x00;
	} 
	else if ( m_flash4683.flashStatus == FLASH_MODE ) 
	{
		//Initialize Scanner Buffer
		if ( m_flash4683.flashBuffer != NULL ) 
		{
			delete [] m_flash4683.flashBuffer;
		}
		m_flash4683.flashBuffer = new unsigned char [WALMART_BUFFER];
		memset ( m_flash4683.flashBuffer, 0x00, WALMART_BUFFER );
		m_flash4683.tempFlashPtr = m_flash4683.flashBuffer;
		m_flash4683.endFlashPtr = m_flash4683.flashBuffer;
		m_flash4683.currentSection = 0;
		m_flash4683.sectionLength = 0;
		m_flash4683.bytesReceived = 0;
		m_flash4683.bytesToExpect = 0;
		m_flash4683.offset = 0;
		m_flash4683.flashBufferLength = 0;
		for ( int i = 0; i < 35; i++ ) 
		{
			m_flash4683.sections[i] = 0;
		}
		m_flash4683.scanLength = 0;
		m_flash4683.baseLength = 0;
		m_flash4683.fileLength = 0;
		m_flash4683.scanFlashed = false;
		m_flash4683.baseFlashed = false;
		//Set complete bit
		DioMessage[1] = 0x06;
	}
	else if ( m_flash4683.flashStatus == READY_TO_FLASH_SCANNER ||
			  m_flash4683.flashStatus == READY_TO_FLASH_BASE ) 
	{
		// Do not interrupt flashing - an application error has occurred
		DioMessage[1] = 0x05;
		DioMessage[2] = 0x01;
	}
	else
	{
		// Unexpected input - most likely an application error
		// This will reset the entire flashing process
		m_flash4683.flashStatus = NOT_FLASH_MODE;
		DioMessage[1] = 0x05;
		DioMessage[2] = 0x02;
	}
	DioDataLength = 2;	// Length is 2 no matter what
	return 1;
}

char DirectIoParser::DIO_RequestPartNumber()
{
	SetDioStatus(DIO_ACCEPTED_WITH_DATA);
	//add 4th status byte
	DioMessage[0] = 0x00;
	DioDataLength = 1;

	GenerateFirmwareVersion((char *)m_flash4683.baseFirmwareNum);
	for(int i = 1; i < 12; i++)	// fill in 1 through 11
    {
    	DioMessage[i] = m_flash4683.baseFirmwareNum[i-1];
    	DioDataLength ++;
	}
	//add trailing zero
	DioMessage[12] = 0x00;
	DioDataLength = 13;
	return 1;
}

// 
// Request Peripheral ID (x30 x1D x02)
// Output format:
//	[xx]		Flash map number
//	[xx]		EEPROM map number
//	[xx] 		Flash version number
//	[xx]		EEPROM version number
//	[xx]		Reserved Bit 7 = 1 means CRC-32 supported
//
char DirectIoParser::DIO_RequestPeripheralID()
{
	SetDioStatus(DIO_ACCEPTED_WITH_DATA);
	//Add 4th Status Byte
	DioMessage[0] = 0x00;
	//Set data bytes
#if SPOOF_GEN_5
	DioMessage[1] = 0x09; //flash map number [FlashID09] Gen 5 is 09
	DioMessage[2] = 0x09; //EEPROM map number [EEPROMID09] Gen 5 is 09
#else
	DioMessage[1] = 0x19; //flash map number [FlashID09] Gen 5 is 09
	DioMessage[2] = 0x02; //EEPROM map number [EEPROMID09] Gen 5 is 09
#endif

	if( m_flash4683.flashStatus == FLASH_MODE || m_tfData.flashStatus == TF_FLASH_MODE )
		DioMessage[3] = 0x03;	//Set bits 0-2 for non-flash mode, and supports long direct IO
	else
		DioMessage[3] = 0x07;	//Set bits 0,1 for flash mode, and supports long direct IO

	DioMessage[4] = 0x00; //EEPROM Version Number
	DioMessage[5] = 0x80; //CRC32 not supported (80 = supported)
	DioDataLength = 6;
	return 1;
}

char DirectIoParser::DIO_SelectFlashSection(unsigned char * Message)
{
	if( m_flash4683.flashStatus == NOT_FLASH_MODE )
	{
		SetDioStatus(DIO_UNDEFINED);
		return 1;
	}
	SetDioStatus(DIO_ACCEPTED);
	m_flash4683.currentSection = Message[DIO_CHAR4];
	m_flash4683.bytesReceived = 0;
	m_flash4683.offset = 0;
	m_flash4683.bytesToExpect = 0;
	m_flash4683.sectionLength = m_flash4683.sections[m_flash4683.currentSection];
	DioDataLength = 0;
	return 1;
}

char DirectIoParser::DIO_GetCRC16()
{
	SetDioStatus(DIO_ACCEPTED_WITH_DATA);
	//Add 4th Status Byte
	DioMessage[0] = 0x00;
						
	if( m_flash4683.flashStatus == FLASH_MODE ) 
	{
		DioMessage[1] = 0x05;
		DioDataLength = 2;
		m_flash4683.flashStatus = WAIT_FOR_CRC;
	} 
	else if ( m_flash4683.flashStatus == WAIT_FOR_CRC ) 
	{
		if ( ( m_flash4683.endFlashPtr - m_flash4683.sectionLength ) >= m_flash4683.flashBuffer ) 
		{
			m_flash4683.tempFlashPtr = (m_flash4683.endFlashPtr - m_flash4683.sectionLength);
		} 
		else 
		{
			m_flash4683.tempFlashPtr = m_flash4683.flashBuffer;
		}
		m_flash4683.sectionLength = m_flash4683.sections[m_flash4683.currentSection];
		DioMessage[1] = 0x06;

		m_flash4683.uiCRC = Generate_CRC16( 0, m_flash4683.tempFlashPtr, m_flash4683.sectionLength );
		DioMessage[3] = (unsigned char)m_flash4683.uiCRC;
		DioMessage[2] = (unsigned char)(m_flash4683.uiCRC >> 8);
		DioDataLength = 4;
		
		m_flash4683.flashStatus = FLASH_MODE;
	}
	else
	{
		// Most likely application error - set waiting bit
		// Either application will timeout waiting, or our previous
		// task completes and we can calculate the CRC before the timeout
		SetDioStatus(DIO_UNDEFINED);
	}
	return 1;
}

char DirectIoParser::DIO_GetCRC32()
{
	SetDioStatus(DIO_ACCEPTED_WITH_DATA);
	//Add 4th Status Byte
	DioMessage[0] = 0x00;
						
	if( m_flash4683.flashStatus == FLASH_MODE ) 
	{
		DioMessage[1] = 0x05;
		DioDataLength = 2;
		m_flash4683.flashStatus = WAIT_FOR_CRC;
	} 
	else if ( m_flash4683.flashStatus == WAIT_FOR_CRC ) 
	{
		if ( ( m_flash4683.endFlashPtr - m_flash4683.sectionLength ) >= m_flash4683.flashBuffer ) 
		{
			m_flash4683.tempFlashPtr = (m_flash4683.endFlashPtr - m_flash4683.sectionLength);
		} 
		else 
		{
			m_flash4683.tempFlashPtr = m_flash4683.flashBuffer;
		}
		m_flash4683.sectionLength = m_flash4683.sections[m_flash4683.currentSection];
		DioMessage[1] = 0x06;
		
		m_flash4683.ulCRC = Generate_CRC32( 0, m_flash4683.tempFlashPtr, m_flash4683.sectionLength );
		DioMessage[5] = (unsigned char)m_flash4683.ulCRC;
		DioMessage[4] = (unsigned char)(m_flash4683.ulCRC >> 8);
		DioMessage[3] = (unsigned char)(m_flash4683.ulCRC >> 16);
		DioMessage[2] = (unsigned char)(m_flash4683.ulCRC >> 24);
		DioDataLength = 6;
		
		m_flash4683.flashStatus = FLASH_MODE;
	}
	else
	{
		// Most likely application error - set waiting bit
		// Either application will timeout waiting, or our previous
		// task completes and we can calculate the CRC before the timeout
		SetDioStatus(DIO_UNDEFINED);
	}
	return 1;
}

char DirectIoParser::DIO_EraseSelectedSection()
{
	SetDioStatus(DIO_ACCEPTED_WITH_DATA);
	//Add 4th Status Byte
	DioMessage[0] = 0x00;
	if( m_flash4683.flashStatus == FLASH_MODE ) 
	{
		m_flash4683.endFlashPtr -= m_flash4683.sectionLength;
		m_flash4683.flashBufferLength -= m_flash4683.sectionLength;
		m_flash4683.bytesReceived = 0;
		m_flash4683.offset = 0;
		m_flash4683.bytesToExpect = 0;
		m_flash4683.sectionLength = 0;
		m_flash4683.flashStatus = WAIT_FOR_ERASE;
		DioMessage[1] = 0x05;
		DioDataLength = 2;
	} 
	else if( m_flash4683.flashStatus == WAIT_FOR_ERASE ) 
	{
		// Make sure everything erased correctly
		if( m_flash4683.bytesReceived != 0 || m_flash4683.offset != 0 ||
			m_flash4683.bytesToExpect != 0 || m_flash4683.sectionLength != 0 )
		{
			m_flash4683.flashStatus = ERROR_WHILE_FLASHING;
			DioMessage[1] = 0x07;
		}
		else
		{
			m_flash4683.flashStatus = FLASH_MODE;
			DioMessage[1] = 0x06;
		}
		DioDataLength = 2;
	}
	else
	{
		// Not in flash mode or currently erasing - send error code
		SetDioStatus(DIO_UNDEFINED);
	}
	return 1;
}

/*
 * Input: 30h 1Dh 06h n1 n2 n3 n4
 * n1: Lower byte of offset word
 * n2: Upper byte of offset word
 * n3: Upper value of number of data bytes to expect
 * n4: Lower value of number of data bytes to expect
 */
char DirectIoParser::DIO_SelectFlashOffset(unsigned char * Message)
{
	if( m_flash4683.flashStatus == NOT_FLASH_MODE )
	{
		SetDioStatus(DIO_UNDEFINED);
		return 1;
	}
	SetDioStatus(DIO_ACCEPTED);
	m_flash4683.offset = ( Message[4] * 256 ) + Message[3];	
	m_flash4683.bytesToExpect = ( Message[5] * 256 ) + Message[6];
	m_flash4683.bytesReceived = 0;
	return 1;
}

char DirectIoParser::DIO_UpdateDevice()
{
	SetDioStatus(DIO_ACCEPTED_WITH_DATA);
	//Add 4th Status Byte
	DioMessage[0] = 0x00;
	DioDataLength = 2;
	if ( m_flash4683.flashStatus == FLASH_MODE ) 
	{
		DioMessage[1] = 0x05;
		m_flash4683.flashStatus = WAIT_FOR_UPDATE;
		m_flash4683.sections[m_flash4683.currentSection] = m_flash4683.sectionLength;
		// If its the first section then save the file length and delete the first section
		if( m_flash4683.currentSection == 1 ) 
		{
			// Store the length of scanner code
			m_flash4683.scanLength = TF_GetIntFromBuffer(m_flash4683.flashBuffer, 0);
			// Store the length of base code
			m_flash4683.baseLength = TF_GetIntFromBuffer(m_flash4683.flashBuffer, 4);
			// Store whole file length
			m_flash4683.fileLength = m_flash4683.baseLength + m_flash4683.scanLength + SECTIONLEN_1;
			char dbgbuf[64];
			sprintf(dbgbuf, "Scan %d, base %d, total %d\r\n",m_flash4683.scanLength, m_flash4683.baseLength, m_flash4683.fileLength);
			AddStringToLogFile(dbgbuf, strlen(dbgbuf));
		}
		// If we've received all of the scanner data then flash the scanner
		if( m_flash4683.scanLength > 0 && m_flash4683.scanFlashed == false &&
			m_flash4683.flashBufferLength >= (m_flash4683.scanLength + 16) ) 
		{
			// Include 16 for offset, compiler won't allow SECTIONLEN_1...
			m_flash4683.tempFlashPtr = m_flash4683.flashBuffer;
			m_flash4683.tempFlashPtr += SECTIONLEN_1;
			m_flash4683.flashStatus = READY_TO_FLASH_SCANNER;
		}
		// If we've received the whole file then flash the base
		if( ( m_flash4683.baseLength > 0 ) &&
			( m_flash4683.flashBufferLength >= m_flash4683.fileLength ) ) 
		{
			m_flash4683.tempFlashPtr = m_flash4683.flashBuffer;
			m_flash4683.tempFlashPtr += m_flash4683.scanLength;
			m_flash4683.tempFlashPtr += SECTIONLEN_1;
			m_flash4683.flashStatus = READY_TO_FLASH_BASE;
		}
	} 
	else if ( m_flash4683.flashStatus == READY_TO_FLASH_SCANNER ||
			  m_flash4683.flashStatus == READY_TO_FLASH_BASE ) 
	{
		// Standard Output
		DioMessage[1] = 0x05;
	} 
	else if ( m_flash4683.flashStatus == SCANNER_FLASHED ) 
	{
		m_flash4683.scanFlashed = true;
		// The scanner has been flashed, erase scanner firmware from buffer
		// Set temp pointer to the beginning of this section
		m_flash4683.tempFlashPtr = m_flash4683.endFlashPtr - m_flash4683.sectionLength;
		// Copy the entire section to the front of the buffer
		// But after Section 1 with the lengths, so the base can flash correctly
		memcpy( (m_flash4683.flashBuffer + 16), m_flash4683.tempFlashPtr, m_flash4683.sectionLength);
		// Reset the end flash pointer
		m_flash4683.endFlashPtr = m_flash4683.flashBuffer + m_flash4683.sectionLength + SECTIONLEN_1;
		// Base Length so far = Flash Buffer Length - Scanner Length
		// New scanner length = Section Length - Base Length so far
		m_flash4683.scanLength = m_flash4683.sectionLength -
								(m_flash4683.flashBufferLength - (m_flash4683.scanLength + 16));  // 16 for SECTIONLEN_1
		DioMessage[1] = 0x05;
		m_flash4683.flashStatus = WAIT_FOR_UPDATE;
	} 
	else if ( m_flash4683.flashStatus == WAIT_FOR_UPDATE ||
			  m_flash4683.flashStatus == BASE_FLASHED ) 
	{
		m_flash4683.flashStatus = FLASH_MODE;
		DioMessage[1] = 0x06;
	}
	else if ( m_flash4683.flashStatus == ERROR_WHILE_FLASHING )
	{
		m_flash4683.flashStatus = ERROR_WHILE_FLASHING;
		DioMessage[1] = 0x07;
#if WALMART_DEBUG
		if( GET_SETTING(EnableWalmartDebug))
		{
			DioMessage[2] = m_flash4683.errorCondition;
			DioDataLength = 3;
		}
#endif
	}
	else
	{
		// Most likely an application error, either we will recover or 
		// the application will time-out
		SetDioStatus(DIO_UNDEFINED);
	}
	return 1;
}

char DirectIoParser::DIO_ResetDevice()
{
	if( m_flash4683.flashStatus == NOT_FLASH_MODE )
	{
		SetDioStatus(DIO_UNDEFINED);
		return 1;
	}
	SetDioStatus(DIO_ACCEPTED);
	m_flash4683.flashStatus = READY_TO_RESET;
	return 1;
}

char DirectIoParser::DIO_RequestScannerFirmwareVersion()
{
	SetDioStatus(DIO_ACCEPTED_WITH_DATA);
	DioMessage[0] = 0x00;
	DioDataLength = 1;
	// Add scanner's firmware number
	GenerateScannerVersionInformation();
	for( int i = 0; i < 11; i++ )
	{
		DioMessage[i+1] = m_flash4683.scannerFirmwareNum[i];
		DioDataLength ++;
	}
	// Add terminating zero
	DioMessage[DioDataLength++] = 0x00;
	return 1;
}

char DirectIoParser::DIO_IncrementFirmwareFlashCounter()
{
	SetDioStatus(DIO_ACCEPTED);
	return 1;
}

char DirectIoParser::DIO_FlashData(unsigned char * Message, int Length)
{
	SetDioStatus(DIO_ACCEPTED);
	if( m_flash4683.flashStatus == FLASH_MODE )			// Flash Scan
	{
		// If data is being sent to the scanner, store it in flashBuffer
		// Update section length and flashBufferLength
		for( int i = DIO_CHAR3; i < Length; i++ ) 
		{
			*m_flash4683.endFlashPtr = Message[i];
			m_flash4683.endFlashPtr++;
			m_flash4683.sectionLength ++;
			m_flash4683.flashBufferLength ++;
		}
	}
	else if( m_tfData.flashStatus == TF_FLASH_MODE )	// Trickle Feed
	{
		// Store data and increment offset
		for( int i = DIO_CHAR3; i < Length; i++ )
		{
			m_tfData.flashBuffer[m_tfData.offset] = Message[i];
			m_tfData.offset++;
		}
	}
	else
	{
		SetDioStatus(DIO_UNDEFINED);
	}
	return 1;
}

char DirectIoParser::DIO_SendConfigurationA(unsigned char * Message, int Length)
{
	// This command contains status bytes a,b,c,d,e,f,g
	// Check for any invalid values
	if( Length < 9 )
	{
		SetDioStatus(DIO_NOT_ALLOWED);
		return 0;
	}
	SetDioStatus(DIO_ACCEPTED);
	unsigned char config;
	int temp;

	configHelper->ResetMenuBuffer();
	// cmd header
	configHelper->AppendToMenuBuffer(_U("\x02M\r"));
	// Byte 0(A) - Trigger Mode, Linking Mode, Charge Rate, Good Read Beep and Beeper Volume
	config =  Message[BYTE_A];
	temp = config & 0x03;
	if( temp == 0x01 ) temp = TRGMOD_PRESENT;
	else if( temp == 0x02 ) temp = TRGMOD_AUTO;
	else if( temp == 0x03 ) temp = TRGMOD_MANUAL;
	configHelper->AppendToMenuBuffer(_U("0E0100%i;") , temp);
	configHelper->AppendToMenuBuffer(_U("BASCON%i;") , (config & 0x04) ? 1 : 0);
	temp = config & 0x18;
	if( temp == 0x00 ) temp = 3;
	else if( temp == 0x08 ) temp = 1;
	else if( temp == 0x10 ) temp = 4;
	else if( temp == 0x18 ) temp = 2;
	configHelper->AppendToMenuBuffer(_U("BASCHG%i;") , temp);
	configHelper->AppendToMenuBuffer(_U("050210%i;") , (config & 0x20) ? 1 : 0);
	temp = config & 0xC0;
	if( temp == 0x00 ) temp = 0;
	else if( temp == 0x40 ) temp = 1;
	else if( temp == 0x80 ) temp = 2;
	else if( temp == 0xC0 ) temp = 3;
	configHelper->AppendToMenuBuffer(_U("05021D%i;") , temp);
	// Byte 1(B) - Good Read Beep, Programming Barcodes, Beeper Pitch, Good Read LED, Good Read Delay
	config = Message[BYTE_B];
	configHelper->AppendToMenuBuffer(_U("050216%i;") , (config & 0x01) ? 0 : 1);	// 0 and 1 have to be switched
	configHelper->AppendToMenuBuffer(_U("060F03%i;") , (config & 0x02) ? 0 : 1);	// 0 and 1 have to be switched
	temp = config & 0x18;
	if( temp == 0x08 ) temp = 1600;
	else if( temp == 0x10 ) temp = 3250;
	else if( temp == 0x18 ) temp = 4200;
	configHelper->AppendToMenuBuffer(_U("05020D%i;") , temp);
	configHelper->AppendToMenuBuffer(_U("05020F%i;") , (config & 0x20) ? 1 : 0);
	temp = config & 0xC0;
	if( temp == 0x00 ) temp = 0;
	else if( temp == 0x40 ) temp = 500;
	else if( temp == 0x80 ) temp = 1000;
	else if( temp == 0xC0 ) temp = 1500;
	configHelper->AppendToMenuBuffer(_U("080B08%i;") , temp);
	// Byte 2(C) - Reread Delay, Addenda Read Speed, Data Accumulation, Expand UPCE to UPCA
	config = Message[BYTE_C];
	temp = config & 0x07;
	if( temp == 0 ) temp = 100;
	else if( temp == 0x01 ) temp = 500;
	else if( temp == 0x02 ) temp = 750;
	else if( temp == 0x03 ) temp = 1000;
	else if( temp == 0x04 ) temp = 2000;
	configHelper->AppendToMenuBuffer(_U("080B06%i;") , temp);
	temp = config & 0x30;
	if( temp == 0x30 ) temp = 10;
	else if( temp == 0x20 ) temp = 120;
	else if( temp == 0x10 ) temp = 250;
	else if( temp == 0x00 ) temp = 500;
	configHelper->AppendToMenuBuffer(_U("025100%i;") , temp);
	configHelper->AppendToMenuBuffer(_U("BATENA%i;") , (config & 0x40) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("021203%i;") , (config & 0x80) ? 1 : 0);
	// Byte 3(D) - Base out of range alarm, Scanner out of range alarm
	config = Message[BYTE_D];
	temp = (config & 0x0F) * 200;
	configHelper->AppendToMenuBuffer(_U("BASORD%i;"), temp);
	temp = ((config & 0xF0)>>4) * 200;
	configHelper->AppendToMenuBuffer(_U("BT_ORD%i;"), temp);
	// Byte 4(E) - UPCA and UPCE
	config = Message[BYTE_E];
	configHelper->AppendToMenuBuffer(_U("021101%i;"), (config & 0x01) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("021104%i;"), (config & 0x02) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("021105%i;"), (config & 0x04) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("021106%i;"), (config & 0x08) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("021201%i;"), (config & 0x10) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("021206%i;"), (config & 0x20) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("021207%i;"), (config & 0x40) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("021208%i;"), (config & 0x80) ? 1 : 0);
	// Byte 5(F) - EAN/JAN 8 and 13
	config = Message[BYTE_F];
	configHelper->AppendToMenuBuffer(_U("021401%i;"), (config & 0x01) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("021403%i;"), (config & 0x02) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("021404%i;"), (config & 0x04) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("021405%i;"), (config & 0x08) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("021301%i;"), (config & 0x10) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("021303%i;"), (config & 0x20) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("021304%i;"), (config & 0x40) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("021305%i;"), (config & 0x80) ? 1 : 0);
	// Byte 6(G) - RSS, Code 128, Code 39 and Code 93
	config = Message[BYTE_G];
	configHelper->AppendToMenuBuffer(_U("021901%i;"), (config & 0x01) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("021B01%i;"), (config & 0x02) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("020A01%i;"), (config & 0x04) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("020301%i;"), (config & 0x08) ? 1 : 0);
	temp = config & 0x30;
	if( temp == 0x00 ) temp = 0;
	else if( temp == 0x10 ) temp = 1;
	else if( temp == 0x20 ) temp = 2;
	configHelper->AppendToMenuBuffer(_U("020304%i;"), temp);
	configHelper->AppendToMenuBuffer(_U("020302%i;"), (config & 0x40) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("020D01%i!"), (config & 0x80) ? 1 : 0);

	return TalkToReaderConfig(1);
}

char DirectIoParser::DIO_SendConfigurationB(unsigned char * Message, int Length)
{
	// This command contains status bytes h,i,j,k,l,m
	if( Length < 7 )
	{
		SetDioStatus(DIO_NOT_ALLOWED);
		return 0;
	}
	SetDioStatus(DIO_ACCEPTED);
	unsigned char config;
	int temp;

	configHelper->ResetMenuBuffer();
	// cmd header
	configHelper->AppendToMenuBuffer(_U("\x02M\r"));
	// Byte 7(H) - Misc symbologies
	config = Message[BYTE_H];
	configHelper->AppendToMenuBuffer(_U("020201%i;"), (config & 0x01) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("021801%i;"), (config & 0x04) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("022401%i;"), (config & 0x08) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("020601%i;"), (config & 0x10) ? 1 : 0);
#if (!defined(XENON_LITE_1500)) && (!defined(HI2D_T))	/* Code 11 is removed for Xenon Lite(1500) and HI2D*/
	configHelper->AppendToMenuBuffer(_U("020901%i;"), (config & 0x20) ? 1 : 0);
#endif
	configHelper->AppendToMenuBuffer(_U("022501%i;"), (config & 0x40) ? 1 : 0);
#if (!defined(XENON_LITE_1500)) && (!defined(HI2D_T))	/* Code 32 is removed for Xenon Lite(1500) and HI2D*/
	configHelper->AppendToMenuBuffer(_U("020306%i;"), (config & 0x80) ? 1 : 0);
#endif
	// Byte 8(I) - Misc symbologies
	config = Message[BYTE_I];
	configHelper->AppendToMenuBuffer(_U("022601%i;"), (config & 0x01) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("020701%i;"), (config & 0x02) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("020401%i;"), (config & 0x04) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("023901%i;"), (config & 0x08) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("020801%i;"), (config & 0x10) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("021500%i;"), (config & 0x20) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("020E01%i;"), (config & 0x40) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("020F01%i;"), (config & 0x80) ? 1 : 0);
	// Byte 9(J) - Misc symbologies, Imager Power Timeout
	config = Message[BYTE_J];
	configHelper->AppendToMenuBuffer(_U("021D01%i;"), (config & 0x01) ? 1 : 0);
#if (!defined(XENON_LITE_1500)) && (!defined(HI2D_T))	/* Telepen/Trioptic is removed for Xenon Lite(1500) and HI2D*/
	configHelper->AppendToMenuBuffer(_U("021001%i;"), (config & 0x02) ? 1 : 0);
	configHelper->AppendToMenuBuffer(_U("021E00%i;"), (config & 0x04) ? 1 : 0);
#endif
	temp = config & 0x38;
	if(temp == 0x00)		temp = 0;
	else if(temp == 0x08)	temp = 200;
	else if(temp == 0x10)	temp = 400;
	else if(temp == 0x18)	temp = 900;
	else if(temp == 0x20)	temp = 3600;
	else if(temp == 0x28)	temp = 7200;
	configHelper->AppendToMenuBuffer(_U("BT_LPT%i;"), temp);
	// Byte 10(K) - ITF Length
	// Byte 11(L) - ITF Length
	config = Message[BYTE_K];
	temp = config & I25_LENGTH_MASK;
	if(config & 0x80)
	{
		config = Message[BYTE_L];
		temp += (config & I25_LENGTH_MASK);
	}
	configHelper->AppendToMenuBuffer(_U("020404%i!"), temp);

	return TalkToReaderConfig(2);
}

char DirectIoParser::DIO_ReportConfiguration()
{
	SetDioStatus(DIO_ACCEPTED_WITH_DATA);
	DioMessage[0] = 0x00;
	DioMessage[1] = '0';
	DioMessage[2] = 'A';
	DioMessage[3] = '3';
	DioDataLength = 4;
	
	QueryScannerConfig();
	for(int i=0;i<=DIO_CONFIGURATION_LENGTH;i++) 
	{
		DioMessage[i+4] = m_flash4683.DIOconfiguration[i];
		DioDataLength ++;
	}
	return 1;
}

// Full Unit Identification (0A0)
// Output of this command must be (specific to RF base or corded scanner):
//	0A10, 			- command received
//	xxxx,			- 4-digit class number 
//	xxxx,,			- 4-digit model number with extra comma 
//	xxxxxxxxxxx,	- 11-digit serial number 
//	xxxxxxxxxxx,	- 11-digit firmware part number
//	xxx				- 3-digit firmware version number
//
char DirectIoParser::DIO_FullUnitID()
{
	char dioOutput[256];
	dioOutput[0] = 0;
	SetDioStatus(DIO_ACCEPTED_WITH_DATA);
	DioMessage[0] = 0x00;
	DioDataLength = 1;
	
	int fwLength = GenerateFirmwareVersion((char *)m_flash4683.baseFirmwareNum);
	GenerateSerialNumber((char *)m_flash4683.baseSerialNum);
	GenerateProductName((char *)m_flash4683.baseProductName);	// Reply hard coded to 3030 for Xenon right now

	// Gen 5 should report 3000 instead of 3500
#if SPOOF_GEN_5
	sprintf( dioOutput, "0A10,3030,3000,,%11s,%11s,%3s", 
			 m_flash4683.baseSerialNum, 
			 m_flash4683.baseFirmwareNum, 
			 m_flash4683.baseFirmwareNum+fwLength-4  );
#else
	sprintf( dioOutput, "0A10,3030,3500,,%11s,%11s,%3s", 
			 m_flash4683.baseSerialNum, 
			 m_flash4683.baseFirmwareNum, 
			 m_flash4683.baseFirmwareNum+fwLength-4  );
#endif
	for( int i = 0; i <= strlen( dioOutput ); i++ )
	{
		DioMessage[i + 1] = dioOutput[i];
		DioDataLength ++;
	}
	
	return 1;
}

//
// Send Firmware Part Number and New Version SA-ONLY (0A4)
// Output:
// 	0A4,			- Received command
//	xxxxxxxxxx,		- 11-digit firmware part number
//	ww.xx.yy.zz,	- Version number: major HW ver, major SW ver, minor SW ver, patch num
//
char DirectIoParser::DIO_FWNewVersionReport()
{
	char dioOutput[256];
	dioOutput[0] = 0;
	SetDioStatus(DIO_ACCEPTED_WITH_DATA);
	DioMessage[0] = 0x00;
	DioDataLength = 1;
	
	GenerateFirmwareVersion((char *)m_flash4683.baseFirmwareNum);

	sprintf( dioOutput, "0A4,%11s,01.01.01.01", m_flash4683.baseFirmwareNum );
	
	for( int i = 0; i <= strlen( dioOutput ); i++ )
	{
		DioMessage[i + 1] = dioOutput[i];
		DioDataLength ++;
	}
	return 1;
}

//
// Send Firmware Part Number and Version (0A6)
// Output Format:
//	0A6,			- Received command
//	xxxxxxxxxxx,	- Firmware part number
//  xxx				- Version number
//
char DirectIoParser::DIO_FWVersionReport()
{
	char dioOutput[256];
	dioOutput[0] = 0;
	SetDioStatus(DIO_ACCEPTED_WITH_DATA);
	DioMessage[0] = 0x00;
	DioDataLength = 1;
	
	int fwLength = GenerateFirmwareVersion((char *)m_flash4683.baseFirmwareNum);

	sprintf( dioOutput, "0A6,%11s,%3s", 
			 m_flash4683.baseFirmwareNum,
			 m_flash4683.baseFirmwareNum+fwLength-3 );
	
	for( int i = 0; i <= strlen( dioOutput ); i++ )
	{
		DioMessage[i + 1] = dioOutput[i];
		DioDataLength ++;
	}
	return 1;
}

//
// Send Serial Number (0A5)
// Output Format:
//	0A5, 			- Received Command
//	xxxxxxxxxxx,	- Unit (base or corded scanner) serial number
//
char DirectIoParser::DIO_SerialNumberReport()
{
	char dioOutput[256];
	dioOutput[0] = 0;
	SetDioStatus(DIO_ACCEPTED_WITH_DATA);
	DioMessage[0] = 0x00;
	DioDataLength = 1;
	
	sprintf( dioOutput, "0A5,%s,", m_flash4683.baseSerialNum );
	
	for( int i = 0; i <= strlen( dioOutput ); i++ )
	{
		DioMessage[i + 1] = dioOutput[i];
		DioDataLength ++;
	}
	return 1;
}

//
// Request HH Full Unit ID
// Output Format:
//	0H				- Received command
//	01,				- Command response version
//	xxx, 			- Length in decimal
//	xxxx,			- Base class number (p-name)
//	xxxxxxxxxxx,	- Base serial number (11 digits)
//	xxxxxxxxxxx,	- Base software number (11 digits)
//	xxxx,			- Scanner class number (p-name)
//	xxxxxxxxxxx,	- Scanner serial number (11 digits)
//	xxxxxxxxxxx,	- Scanner software number (11 digits)
//	xxxx			- Peripheral ID (same as x30 x1D x02)
//
char DirectIoParser::DIO_0H0Command()
{
	char dioOutput[256];
	dioOutput[0] = 0;
	SetDioStatus(DIO_ACCEPTED_WITH_DATA);
	DioMessage[0] = 0x00;
	DioDataLength = 1;
	// do the 0h0 stuff
	GenerateScannerVersionInformation();
	GenerateFirmwareVersion((char *)m_flash4683.baseFirmwareNum);
	GenerateSerialNumber((char *)m_flash4683.baseSerialNum);
	
	// Gen 5 should report 0909 instead of 1902
	// FIXME: Base product name hard coded to 3030 per NCR request
	// FIXME: Scanner product name hard coded to 6620 per NCR request
#if SPOOF_GEN_5
	sprintf( dioOutput, "0H01,070,2020,%11s,%11s,5620,%11s,%11s,0909",
			 m_flash4683.baseSerialNum, m_flash4683.baseFirmwareNum,
			 m_flash4683.scannerSerialNum, m_flash4683.scannerFirmwareNum);
#else
	sprintf( dioOutput, "0H01,070,3030,%11s,%11s,6620,%11s,%11s,1902",
			 m_flash4683.baseSerialNum, m_flash4683.baseFirmwareNum,
			 m_flash4683.scannerSerialNum, m_flash4683.scannerFirmwareNum);
#endif
	for( int i = 0; i <= strlen( dioOutput ); i++ )
	{
		DioMessage[i + 1] = dioOutput[i];
		DioDataLength ++;
	}
	return 1;
}

char DirectIoParser::DIO_Beep4683()
{
	SetDioStatus(DIO_ACCEPTED);
	SoundStandardBeep(3);
	return 1;
}

// Trickle feed commands...
/****************************************************************************
 *
 *  Function:   TF_EnterTrickleFeed
 *				This function will put the base into 'trickle feed mode' by 
 *				changing the state of the state machine.  If it is a new 
 *				download, or if the buffer has been corrupted a new buffer will
 *				be created and all values will be reset.  
 *				This command is sent at least twice which gives the base extra
 *				time to configure everything.  
 *
 * **************************************************************************/
char DirectIoParser::TF_EnterTrickleFeed()
{
	SetDioStatus(DIO_ACCEPTED_WITH_DATA);
	// send a 0x05 or 0x06
	DioMessage[0] = 0x00;
	DioMessage[1] = 0x05;
	DioDataLength = 2;
	// Check current state
	if( (m_tfData.flashStatus == TF_PAUSED) || (m_tfData.flashStatus == TF_COMPLETE) )
	{
		// check CRC
		if( m_tfData.ulCRC != Generate_CRC32(0, m_tfData.flashBuffer, WALMART_BUFFER) )
		{
			// If buffer is corrupt then reset values
			TF_ResetValues();
			m_tfData.flashStatus = TF_FLASH_MODE;
			if( m_tfData.flashBuffer != NULL )
				delete [] m_tfData.flashBuffer;
			m_tfData.flashBuffer = new unsigned char [WALMART_BUFFER];
			memset( m_tfData.flashBuffer, 0x00, WALMART_BUFFER );
		}
		// Set the correct state
		if( m_tfData.flashStatus == TF_COMPLETE )
		{
			m_tfData.flashStatus = TF_FLASH_TRANSFER_COMPLETE;
		}
		else 
		{
			m_tfData.flashStatus = TF_FLASH_MODE;
		}
	}
	else if( m_tfData.flashStatus == TF_FLASH_MODE || 
			 m_tfData.flashStatus == TF_FLASH_TRANSFER_COMPLETE )
	{
		// send 0x06
		DioMessage[1] = 0x06;
	}
	else //if( m_tfData.flashStatus == NOT_FLASH_MODE )
	{
		TF_ResetValues();
		m_tfData.flashStatus = TF_FLASH_MODE;
		if( m_tfData.flashBuffer != NULL )
			delete [] m_tfData.flashBuffer;
		m_tfData.flashBuffer = new unsigned char [WALMART_BUFFER];
		memset ( m_tfData.flashBuffer, 0x00, WALMART_BUFFER );
	}
	return 1;
}

/****************************************************************************
 *
 *  Function:   TF_GetDownloadOffset
 *				This function will send the current offset that the base is 
 *				expecting to receive data from.
 *
 * **************************************************************************/
char DirectIoParser::TF_GetDownloadOffset()
{
	if( m_tfData.flashStatus == TF_FLASH_MODE || 
		m_tfData.flashStatus == TF_FLASH_TRANSFER_COMPLETE )
	{
		SetDioStatus(DIO_ACCEPTED_WITH_DATA);
		DioMessage[4] = (unsigned char) m_tfData.offset;
		DioMessage[3] = (unsigned char)(m_tfData.offset >> 8);
		DioMessage[2] = (unsigned char)(m_tfData.offset >> 16);
		DioMessage[1] = (unsigned char)(m_tfData.offset >> 24);
		DioDataLength = 5;
	}
	else
	{
		SetDioStatus(DIO_UNDEFINED);
	}
	return 1;
}

#define OFFSETS_LOC	0x0240
/****************************************************************************
 *
 *  Function:   TF_TrickleFeedComplete
 *				This function indicates that the download has completed.  The
 *				lengths of the header, scanner, and base firmware will all be
 *				extracted from the firmware image.  The total file lenght will
 *				be stored and the whole-file CRC will be extracted from the 
 *				end of the file.  The CRC isn't checked until the Activate 
 *				command is sent in case the buffer changes. 
 *
 * **************************************************************************/
char DirectIoParser::TF_TrickleFeedComplete()
{
	if( m_tfData.flashStatus == TF_FLASH_MODE ||
		m_tfData.flashStatus == TF_FLASH_TRANSFER_COMPLETE )
	{
		SetDioStatus(DIO_ACCEPTED);
		m_tfData.ulCRC = Generate_CRC32( 0, m_tfData.flashBuffer, WALMART_BUFFER );
		m_tfData.flashStatus = TF_FLASH_TRANSFER_COMPLETE;

		// **** SINGLE FILE SOLUTION *****
		// Skip over the flash scan header (should be 0x3FF bytes)
		// That actual information is stored in the header, offsets are stored at 0x0240
		// each offset is stored as 4 hex byte.
		// Section 0 = FlashScan header			(OFFSETS_LOC + 0) - ignore
		// Section 1 = Hand held header			(OFFSETS_LOC + 4) - get scan/base lengths
		// Section 2 = Beginning of firmware	(OFFSETS_LOC + 8) - where FW starts
		//int fsHeaderLength = 0;		// Length of flash scan header (to be ignored)
		//fsHeaderLength = TF_GetIntFromBuffer( m_tfData.flashBuffer, OFFSETS_LOC + 4 );
		//m_tfData.headerLength = TF_GetIntFromBuffer( m_tfData.flashBuffer, OFFSETS_LOC + 8 );
		//m_tfData.scanLength = TF_GetIntFromBuffer( m_tfData.flashBuffer, fsHeaderLength );
		//m_tfData.baseLength = TF_GetIntFromBuffer( m_tfData.flashBuffer, fsHeaderLength + 4 );

		// **** CURRENT DUAL FILE SOLUTION ****
		// Store the length of the header
		m_tfData.headerLength = TF_GetIntFromBuffer( m_tfData.flashBuffer, 0 );
		// Store the length of scanner code
		m_tfData.scanLength = TF_GetIntFromBuffer( m_tfData.flashBuffer, 4 );
		// Store the length of base code
		m_tfData.baseLength = TF_GetIntFromBuffer( m_tfData.flashBuffer, 8 );
		// **** END SINGLE/DUAL FILE DIFFERENCES ****

		// Store whole file length
		m_tfData.fileLength = m_tfData.headerLength + m_tfData.baseLength + m_tfData.scanLength;
		// Store the whole file CRC
		m_tfData.fileCRC = TF_GetIntFromBuffer( m_tfData.flashBuffer, m_tfData.fileLength );
#if WALMART_DEBUG
		if( GET_SETTING(EnableWalmartDebug) )
		{
			char debugBuf[128];
			sprintf( debugBuf, "HHHead = %X, ScanLen = %X, BaseLen = %X, FileLen =%X \r\n", 
					 m_tfData.headerLength, m_tfData.scanLength, m_tfData.baseLength, m_tfData.fileLength );
			AddStringToLogFile( debugBuf, strlen(debugBuf) );
		}
#endif
	}
	else
	{
		SetDioStatus(DIO_UNDEFINED);
	}
	return 1;
}

/****************************************************************************
 *
 *  Function:   TF_PauseTrickleFeed
 *				This function will pause the download.  It will calculate and
 *				store the CRC of the current buffer. 
 *
 * **************************************************************************/
char DirectIoParser::TF_PauseTrickleFeed()
{
	if( m_tfData.flashStatus == TF_FLASH_MODE )
	{
		SetDioStatus(DIO_ACCEPTED);
		m_tfData.ulCRC = Generate_CRC32( 0, m_tfData.flashBuffer, WALMART_BUFFER );
		m_tfData.flashStatus = TF_PAUSED;
	}
	else if( m_tfData.flashStatus == TF_FLASH_TRANSFER_COMPLETE )
	{
		SetDioStatus(DIO_ACCEPTED);
		m_tfData.ulCRC = Generate_CRC32( 0, m_tfData.flashBuffer, WALMART_BUFFER );
		m_tfData.flashStatus = TF_COMPLETE;
	}
	else
	{
		SetDioStatus(DIO_UNDEFINED);
	}
	return 1;
}

/****************************************************************************
 *
 *  Function:   TF_TerminateTrickleFeed
 *				This function will terminate the download and reset the state
 *				machine and all variables. 
 *
 * **************************************************************************/
char DirectIoParser::TF_TerminateTrickleFeed()
{
	if( m_tfData.flashStatus != NOT_FLASH_MODE )
	{
		SetDioStatus(DIO_ACCEPTED);
		TF_ResetValues();
	}
	else
	{
		SetDioStatus(DIO_UNDEFINED);
	}
	return 1;
}

/****************************************************************************
 *
 *  Function:   TF_ActivateNewFirmware
 *				This function will setup the state machine to allow the scanner
 *				then base to flash.  If the download was completed then the 
 *				CRC embedded in the file is checked.  After that the scanner
 *				is flashed first then the base.  After flashing both the base
 *				is reset.
 *				This command will be sent repetedly until the scanner and base
 *				have been flashed.  A final ACK (0x06) will signify that 
 *				everythign completed successfully. 
 *
 * **************************************************************************/
char DirectIoParser::TF_ActivateNewFirmware()
{
	SetDioStatus(DIO_ACCEPTED_WITH_DATA);
	DioMessage[0] = 0x00;
	DioMessage[1] = 0x05;
	DioDataLength = 2;

	unsigned int fileCRC = 0;

	switch( m_tfData.flashStatus )
	{
	case TF_FLASH_TRANSFER_COMPLETE:
		// Check file CRC
		fileCRC = Generate_CRC32(0,m_tfData.flashBuffer,m_tfData.fileLength);
		if( m_tfData.fileCRC != fileCRC )
		{
#if WALMART_DEBUG
			if( GET_SETTING(EnableWalmartDebug) )
			{
				char debugBuf[64];
				sprintf( debugBuf, "TF CRC Mismatch - File: %X, Calc: %X\r\n", m_tfData.fileCRC, m_tfData.calcFileCRC );
				AddStringToLogFile( debugBuf, strlen(debugBuf) );
			}
#endif
			TF_ResetValues();
			m_tfData.flashStatus = ERROR_WHILE_FLASHING;
		}
		else if( m_tfData.scanLength > 0 )
		{
			// Flash scanner
			m_tfData.flashStatus = READY_TO_FLASH_SCANNER;
		}
		else if( m_tfData.baseLength > 0 )
		{
			// Flash base
			m_tfData.flashStatus = READY_TO_FLASH_BASE;
		}
		break;
	case READY_TO_FLASH_SCANNER:
	case READY_TO_FLASH_BASE:
		break;
	case SCANNER_FLASHED:
		// Flash the base now
		if( m_tfData.baseLength > 0 )
		{
			// Flash base
			m_tfData.flashStatus = READY_TO_FLASH_BASE;
		}
		else
		{
			m_tfData.flashStatus = READY_TO_RESET;
			DioMessage[1] = 0x06;
		}
		break;
	case BASE_FLASHED:
	case READY_TO_RESET:
		// ready to reset now
		m_tfData.flashStatus = READY_TO_RESET;
		DioMessage[1] = 0x06;
		break;
	case ERROR_WHILE_FLASHING:
		DioMessage[1] = 0x00;		// Send recoverable error
#if WALMART_DEBUG
		if( GET_SETTING(EnableWalmartDebug))
		{
			DioMessage[2] = m_tfData.errorCondition;
			DioDataLength = 3;
		}
#endif
		TF_ResetValues();
		break;
	default:
		SetDioStatus(DIO_UNDEFINED);
		DioDataLength = 0;
		break;
	}
	
	return 1;
}

/****************************************************************************
 *
 *  Function:   TF_ResetValues
 *				This will reset all values used in the trickle feed protocol
 *
 * **************************************************************************/
void TF_ResetValues()
{
	// erase everything!
	if( m_tfData.flashBuffer != NULL )
		delete [] m_tfData.flashBuffer;
	m_tfData.flashBuffer = NULL;
	m_tfData.flashStatus = NOT_FLASH_MODE;
	m_tfData.offset = 0;
	m_tfData.ulCRC = 0;
	m_tfData.fileCRC = 0;
	m_tfData.headerLength = 0;
	m_tfData.scanLength = 0;
	m_tfData.baseLength = 0;
	m_tfData.fileLength = 0;
}

/****************************************************************************
 *
 *	Function:	TF_GetIntFromBufer
 *				Returns an int that was stored backwards in the buffer
 *	Arguments:	Buffer		pointer to the beginning of the buffer
 *				offset		the offset where the number is stored
 *	Returns:	The int created from the buffer
 *
 * **************************************************************************/
unsigned int TF_GetIntFromBuffer( unsigned char * Buffer, int offset )
{
	unsigned char *pBuffer = Buffer;
	int foundInt = 0;

	foundInt = pBuffer[offset + 3];
	foundInt *= 256;
	foundInt += pBuffer[offset + 2];
	foundInt *= 256;
	foundInt += pBuffer[offset + 1];
	foundInt *= 256;
	foundInt += pBuffer[offset + 0];

	return foundInt;
}

/****************************************************************************
 *
 *	Function:	ProcessDioDownload
 *				Parses and flashes the application code received
 *	Arguments:	none
 *
 *	Returns:	none
 *
 * **************************************************************************/
//static bool ProcessMocf(const unsigned char * pMocf, int MocfLength, bool bRunAppFirmwareFromRam = false, bool bDoCompatibleCheck = true);
void ProcessDioDownload(void)
{
	char tmp[32];

	if( m_tfData.flashStatus == READY_TO_FLASH_SCANNER )
	{
		int scannerOffset = m_tfData.headerLength;
		if(CFileHandler::ProcessMocf(m_tfData.flashBuffer+scannerOffset, m_tfData.scanLength, false, true, false))
		{
			m_tfData.flashStatus = SCANNER_FLASHED;
			sprintf(tmp, "Scan Flashed\r\n");
			AddStringToLogFile(tmp, strlen(tmp));
		}
		else
		{
			m_tfData.flashStatus = ERROR_WHILE_FLASHING;
			sprintf(tmp, "Error Flashing TF Scan\r\n");
			AddStringToLogFile(tmp, strlen(tmp));
		}
	}
	else if( m_tfData.flashStatus == READY_TO_FLASH_BASE )
	{
		int baseOffset = m_tfData.headerLength + m_tfData.scanLength;
		if(CFileHandler::ProcessMocf(m_tfData.flashBuffer+baseOffset, m_tfData.baseLength, false, true, false))
		{
			m_tfData.flashStatus = BASE_FLASHED;
			sprintf(tmp, "Base Flashed\r\n");
			AddStringToLogFile(tmp, strlen(tmp));
		}
		else
		{
			m_tfData.flashStatus = ERROR_WHILE_FLASHING;		
			sprintf(tmp, "Error Flashing TF Base\r\n");
			AddStringToLogFile(tmp, strlen(tmp));
		}
	}
	else if( m_flash4683.flashStatus == READY_TO_FLASH_SCANNER )
	{
		if(CFileHandler::ProcessMocf(m_flash4683.tempFlashPtr, m_flash4683.scanLength))
		{
			m_flash4683.flashStatus = SCANNER_FLASHED;
			m_flash4683.scanFlashed = true;
			sprintf(tmp, "Scan Flashed\r\n");
			AddStringToLogFile(tmp, strlen(tmp));
		}
		else
		{
			m_flash4683.flashStatus = ERROR_WHILE_FLASHING;
			m_flash4683.scanFlashed = false;
			sprintf(tmp, "Error Flashing FS Scan\r\n");
			AddStringToLogFile(tmp, strlen(tmp));
		}
	}
	else if( m_flash4683.flashStatus == READY_TO_FLASH_BASE )
	{
		if(CFileHandler::ProcessMocf(m_flash4683.tempFlashPtr, m_flash4683.baseLength, false, true, false))
		{
			m_flash4683.flashStatus = WAIT_FOR_UPDATE;
			m_flash4683.baseFlashed = true;
			sprintf(tmp, "Base Flashed\r\n");
			AddStringToLogFile(tmp, strlen(tmp));
		}
		else
		{
			m_flash4683.flashStatus = ERROR_WHILE_FLASHING;
			sprintf(tmp, "Error Flashing FS Base\r\n");
			AddStringToLogFile(tmp, strlen(tmp));
		}
	}
	else if( m_tfData.flashStatus == READY_TO_RESET || m_flash4683.flashStatus == READY_TO_RESET )
	{
		sprintf(tmp, "Reset %d downloaded\r\n", m_flash4683.flashBufferLength);
		AddStringToLogFile(tmp, strlen(tmp));
		MT_Delay(1000);
		reset_to_boot_code_hard();
	}
	return;
}

/****************************************************************************
 *
 *	Function:	ParseMenuResponse
 *				Parses the configuration response from the menu
 *	Arguments:	*pConfig	A pointer to the configuration buffer (will be modified)
 *				
 *	Returns:	True/False depending on if the configuration is successful
 *
 * **************************************************************************/

bool DirectIoParser::ParseMenuResponse(bchar_t *pConfig)
{
	bool ret = true;
	ret &= ParseMenuResponseA( pConfig );
	ret &= ParseMenuResponseB( pConfig );
	return ret;
}

bool DirectIoParser::ParseMenuResponseA(bchar_t *pConfig)
{
	bool ret = PASS;
	size_t Size = 0;
	int temp = 0;
	const bchar_t *pResponse = NULL;
	CBarcodeData *pMenuResponse = configHelper->GetMenuResponse();
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
	// Byte 0(A) - Trigger Mode, Linking Mode, Charge Rate, Good Read Beep and Beeper Volume
	pConfig[0] = 0x00;
	ret &= CParseHelper::Extract(pResponse, _U("0E0100"), temp);
	if(temp == 0)		pConfig[0] |= 0x03;
	else if( temp == 1)	pConfig[0] |= 0x02;
	else if( temp == 3)	pConfig[0] |= 0x01;
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("050210"), pConfig[0],0x20);
	ret &= CParseHelper::Extract(pResponse, _U("05021D"), temp);
	if(temp == 0)		pConfig[0] |= 0x00;
	else if(temp == 1)	pConfig[0] |= 0x40;
	else if(temp == 2)	pConfig[0] |= 0x80;
	else if(temp == 3)	pConfig[0] |= 0xC0;
	// Byte 1(B) - Good Read Beep, Programming Barcodes, Beeper Pitch, Good Read LED, Good Read Delay
	pConfig[1] = 0x00;
	ret &= CParseHelper::Extract(pResponse, _U("050216"), temp);
	if(temp == 1)							pConfig[1] |= 0x00;
	else if(temp == 0)						pConfig[1] |= 0x01;
	ret &= CParseHelper::Extract(pResponse, _U("060F03"), temp);
	if(temp == 1)							pConfig[1] |= 0x00;
	else if(temp == 0)						pConfig[1] |= 0x02;
	ret &= CParseHelper::Extract(pResponse, _U("05020D"), temp);
	if(temp <= 2425)						pConfig[1] |= 0x08;
	else if(temp >= 2526 && temp <= 3725)	pConfig[1] |= 0x10;
	else if(temp >= 3726)					pConfig[1] |= 0x18;
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("05020F"), pConfig[1],0x20);
	ret &= CParseHelper::Extract(pResponse, _U("080B08"), temp);
	if(temp == 0)							pConfig[1] |= 0x00;
	else if(temp > 0 && temp <= 750)		pConfig[1] |= 0x40;
	else if(temp > 750 && temp <=1250)		pConfig[1] |= 0x80;
	else if(temp > 1250)					pConfig[1] |= 0xC0;
	// Byte 2(C) - Reread Delay, Addenda Read Speed, Data Accumulation, Expand UPCE to UPCA
	pConfig[2] = 0x00;
	ret &= CParseHelper::Extract(pResponse, _U("080B06"), temp);
	if(temp < 500)							pConfig[2] |= 0x00;
	else if(temp >= 500 && temp < 750)		pConfig[2] |= 0x01;
	else if(temp >= 750 && temp < 1000)		pConfig[2] |= 0x02;
	else if(temp >= 1000 && temp < 2000)	pConfig[2] |= 0x03;
	else if(temp > 1000)					pConfig[2] |= 0x04;
	ret &= CParseHelper::Extract(pResponse, _U("025100"), temp);
	if(temp <= 10)							pConfig[2] |= 0x30;
	else if(10 < temp && temp <= 120)		pConfig[2] |= 0x20;
	else if(120 < temp && temp <= 250)		pConfig[2] |= 0x10;
	else if(temp > 250)						pConfig[2] |= 0x00;
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("BATENA"), pConfig[2],0x40);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("021203"), pConfig[2],0x80);
	// Byte 3(D) - Base out of range alarm, Scanner out of range alarm
	pConfig[3] = 0x00;
	temp = 0;
	// Byte 4(E) - UPCA and UPCE
	pConfig[4] = 0x00;
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("021101"), pConfig[4],0x01);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("021104"), pConfig[4],0x02);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("021105"), pConfig[4],0x04);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("021106"), pConfig[4],0x08);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("021201"), pConfig[4],0x10);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("021206"), pConfig[4],0x20);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("021207"), pConfig[4],0x40);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("021208"), pConfig[4],0x80);
	// Byte 5(F) - EAN/JAN 8 and 13
	pConfig[5] = 0x00;
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("021401"), pConfig[5],0x01);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("021403"), pConfig[5],0x02);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("021404"), pConfig[5],0x04);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("021405"), pConfig[5],0x08);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("021301"), pConfig[5],0x10);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("021303"), pConfig[5],0x20);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("021304"), pConfig[5],0x40);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("021305"), pConfig[5],0x80);
	// Byte 6(G) - RSS, Code 128, Code 39 and Code 93
	pConfig[6] = 0x00;
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("021901"), pConfig[6],0x01);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("021B01"), pConfig[6],0x02);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("020A01"), pConfig[6],0x04);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("020301"), pConfig[6],0x08);
	ret &= CParseHelper::Extract(pResponse, _U("020304"), temp);
	if(temp == 0)		pConfig[6] |= 0x00;
	else if(temp == 1)	pConfig[6] |= 0x10;
	else if(temp == 2)	pConfig[6] |= 0x20;
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("020302"), pConfig[6],0x40);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("020D01"), pConfig[6],0x80);

	return ret;
}

bool DirectIoParser::ParseMenuResponseB(bchar_t *pConfig)
{
	bool ret = PASS;
	size_t Size = 0;
	int temp = 0;
	const bchar_t *pResponse = NULL;
	CBarcodeData *pMenuResponse = configHelper->GetMenuResponse();
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
	// Byte 7(H) - Misc symbologies
	pConfig[7] = 0x00;
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("020201"), pConfig[7],0x01);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("021801"), pConfig[7],0x04);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("022401"), pConfig[7],0x08);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("020601"), pConfig[7],0x10);
#if (!defined(XENON_LITE_1500)) && (!defined(HI2D_T))	/* Code 11 is removed for Xenon Lite(1500) and HI2D*/
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("020901"), pConfig[7],0x20);
#endif
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("022501"), pConfig[7],0x40);
#if (!defined(XENON_LITE_1500)) && (!defined(HI2D_T))	/* Code 32 is removed for Xenon Lite(1500) and HI2D*/
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("020306"), pConfig[7],0x80);
#endif
	// Byte 8(I) - Misc symbologies
	pConfig[8] = 0x00;
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("022601"), pConfig[8],0x01);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("020701"), pConfig[8],0x02);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("020401"), pConfig[8],0x04);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("023901"), pConfig[8],0x08);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("020801"), pConfig[8],0x10);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("021500"), pConfig[8],0x20);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("020E01"), pConfig[8],0x40);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("020F01"), pConfig[8],0x80);
	// Byte 9(J) - Misc symbologies, Imager Power Timeout
	pConfig[9] = 0x00;
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("021D01"), pConfig[9],0x01);
#if (!defined(XENON_LITE_1500)) && (!defined(HI2D_T))	/* Telepen/Trioptic is removed for Xenon Lite(1500) and HI2D*/
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("021001"), pConfig[9],0x02);
	ret &= DrvMenuConfig::ParseMenuResponse_bool(pResponse, _U("021E00"), pConfig[9],0x04);
#endif
	// Byte 10(K) - ITF Length
	// Byte 11(L) - ITF Length
	pConfig[10] = 0x00;
	pConfig[11] = 0x00;
	ret &= CParseHelper::Extract(pResponse, _U("020404"), temp);
	if(temp > 63)
	{
		pConfig[10] = 0xbf;	// 0x80 + 0x3f (63d).
		pConfig[11] = (temp - 63);
	}
	else
	{
		pConfig[10] = temp;
		pConfig[11] = 0;
	}
	// Byte 12(M) - Scanner In Cradle
	pConfig[12] = 0x00;
	return ret;
}

bool DirectIoParser::ParseMenuResponseBattry(bchar_t *pConfig)
{
	// Response comes in the form BATLVL#.##V[ACK]
	// Extract the numbers, compare them to predetermined 80% and 30% 
	// battery levels.  Set configuration accordingly
	bool ret = FAIL;
	size_t Size = 0;
	const bchar_t *pResponse = NULL;
	CBarcodeData *pMenuResponse = configHelper->GetMenuResponse();
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
	pConfig[12] = 0x00;
	return ret;
}

void DirectIoParser::QueryScannerConfig(void)
{
	configHelper->ResetMenuBuffer();
	//cmd header  
	configHelper->AppendToMenuBuffer((const bchar_t*)"\x02M\r"
	// Command A
	// Byte 0(A) - Trigger Mode, Linking Mode, Charge Rate, Good Read Beep and Beeper Volume
	"0E0100?;050210?;05021D?;"
	// Byte 1(B) - Good Read Beep, Programming Barcodes, Beeper Pitch, Good Read LED, Good Read Delay
	"050216?;060F03?;05020D?;05020F?;080B08?;"
	// Byte 2(C) - Reread Delay, Addenda Read Speed, Data Accumulation, Expand UPCE to UPCA
	"080B06?;025100?;BATENA?;021203?;"
	// Byte 4(E) - UPCA and UPCE
	"021101?;021104?;021105?;021106?;021201?;021206?;021207?;021208?;"
	// Byte 5(F) - EAN/JAN 8 and 13
	"021401?;021403?;021404?;021405?;021301?;021303?;021304?;021305?;"
	// Byte 6(G) - RSS, Code 128, Code 39 and Code 93
	"021901?;021B01?;020A01?;020301?;020304?;020302?;020D01?;"
	// Command B
	// Byte 7(H) - Misc symbologies
#if (!defined(XENON_LITE_1500)) && (!defined(HI2D_T))	/* Code 11/Code 32 is removed for Xenon Lite(1500) and HI2D*/
	"020201?;021801?;022401?;020601?;020901?;022501?;020306?;"
#else
	"020201?;021801?;022401?;020601?;022501?;"
#endif
	// Byte 8(I) - Misc symbologies
	"022601?;020701?;020401?;023901?;020801?;021500?;020E01?;020F01?;"
	// Byte 9(J) - Misc symbologies, Imager Power Timeout
#if (!defined(XENON_LITE_1500)) && (!defined(HI2D_T))	/* Telepen/Trioptic is removed for Xenon Lite(1500) and HI2D*/
	"021D01?;021001?;021E00?;"
#else
	"021D01?;"
#endif
	// Byte 10(K) - ITF Length
	// Byte 11(L) - ITF Length
	"020404?;"
	// Byte 12(M) - Battery Level, Scanner present
	"!");
	TalkToReaderConfig(0);
	
	configHelper->ResetMenuBuffer();
}

// Talk to reader config types:
// 0 - Expecting menu response from 'get config' request
// 1 - Not expecting a menu response
// 2 - Not expecting a menu response
// 3 - Expecting menu response form 'get battery level' request
bool DirectIoParser::TalkToReaderConfig(int type)
{
	bool ret = true;
	if (configHelper->GetMenuLength() >= (DRV_MENU_BUFFER_SIZE-1))  //  check for buffer overflow
	{
		ret = false;
	}
	else
	{
		switch( type )
		{
			case 0:
				m_ExpectingMenuResponse = true;
				StuffInputBufferAndWaitForMenuResponse( configHelper->GetMenuBuffer() );
				ret = ParseMenuResponse((unsigned char *)m_flash4683.DIOconfiguration);
				break;
			case 1:
			case 2:
				StuffInputBuffer( (const char *)configHelper->GetMenuBuffer() );
				break;
			case 3:
				m_ExpectingMenuResponse = true;
				StuffInputBufferAndWaitForMenuResponse( configHelper->GetMenuBuffer() );
				ret = ParseMenuResponseBattry((unsigned char *)m_flash4683.DIOconfiguration);
				break;
			default:
				ret = false;
				break;
		}
		m_ExpectingMenuResponse = false;
		configHelper->FreeMenuResponse();
	}
	return ret;
}

// **** Helper functions from USBDataPump.cpp ****
// some helpes to access the menu system
// We stuff the string into the input buffer, so the main loop can read it as it would have been received serially.
// Must only be called from the context of a CThreadRealTime
void DirectIoParser::StuffInputBufferAndWaitForMenuResponse(const bchar_t *pMenuString)
{
	MT_ResetEvent(DirectIOResponseEvent);
#if DIO_DEBUG
	if( GET_SETTING(EnableWalmartDebug) )
	{
		AddStringToLogFile( (const char *)pMenuString, strlen((char *)pMenuString) );
	}
#endif	//#if DIO_DEBUG
	StuffInputBuffer( (const char *)pMenuString );
	MT_WaitEvent(DirectIOResponseEvent,INFINITE);
	MT_ResetEvent(DirectIOResponseEvent);
}

// Can be called in the write finction of a driver to bypass the TxFifo.
// See SurePos driver for an example.
void DirectIoParser::StoreMenuResponse(CBarcodeData *pMenuResponse)
{
	// Here we must ensure that there are no old responses. This could happen if we get an unsolicided menu response.
	// The Free() can be savely called with a NULL pointer!
	configHelper->FreeMenuResponse();	            // Avoid a memory leak
	configHelper->SetMenuResponse(pMenuResponse);
#if DIO_DEBUG
	if( GET_SETTING(EnableWalmartDebug) )
	{
		const bchar_t *pMenuString;
		size_t pMenuSize;
		char title[32];
		sprintf( title, "\r\nResponse\r\n");
		AddStringToLogFile( title, strlen(title) );
		configHelper->GetMenuResponse()->GetTotalData( pMenuString, pMenuSize );
		AddStringToLogFile( (const char *)pMenuString, pMenuSize );
	}
#endif	//#if DIO_DEBUG
	MT_SetEvent(DirectIOResponseEvent);        // wakeup our thread (see TalkToReaderConfig())
}

/****************************************************************************
 *
 *  Function:   GetExpectingMenuResponse
 *              Returns the m_ExpectingMenuResponse variable value
 *
 *  Returns:    bool    Value of m_ExpectingMenuResponse
 *
 * **************************************************************************/
bool DirectIoParser::GetExpectingMenuResponse(void)
{
	return m_ExpectingMenuResponse;
}

void DirectIoParser::DebugSend(const bchar_t *, size_t )
{
}

void DirectIoParser::StuffInputBuffer( const char* pcInputData )
{
	int size = strlen( pcInputData );
	RxFifo->Write(pcInputData, size);
}

/****************************************************************************
 *
 *  Function:   GenerateFirmwareVersion
 *  Arguments:  char    *tempString - modify to add firmware version
 *  Returns:    int                 - lenght of firmware number
 *
 * **************************************************************************/

int DirectIoParser::GenerateFirmwareVersion( char *tempString ) {
    int i = 0;
    const char *		firmwareID = GET_SETTING(ShowWARev);		// ": BIxxxxxxAAC" (example)
#if SPOOF_GEN_5
	tempString[0] = '0';  // 0
	tempString[1] = '0';  // 0
	tempString[2] = '0';  // 0
	tempString[3] = '0';  // 0
	tempString[4] = firmwareID[3];  // 5
	tempString[5] = firmwareID[4];  // 4
	tempString[6] = firmwareID[5];  // 2
	tempString[7] = firmwareID[6];  // 3
	tempString[8] = firmwareID[7]; // x (1)
	tempString[9] = firmwareID[8]; // x (2)
	tempString[10] = firmwareID[9]; // x (3)
	tempString[11] = 0x00;
	i = 12;
#else
    tempString[i++] = '0';  // pad to skip A
    tempString[i++] = '0';  // pad to skip A
    tempString[i++] = '0';  // pad to skip C
    tempString[i++] = firmwareID[2];  // B
    tempString[i++] = firmwareID[3];  // I/K/L
    tempString[i++] = firmwareID[4];  // x
    tempString[i++] = firmwareID[5];  // x
    tempString[i++] = firmwareID[6];  // x
    tempString[i++] = firmwareID[7];  // x
    tempString[i++] = firmwareID[8];  // x
    tempString[i++] = firmwareID[9];  // x
    tempString[i++] = 0x00;
#endif
    return i;
}

 /****************************************************************************
 *
 *  Function:   GenerateScannerVersionInformation
 *  			Fills in m_flash4683.scannerSerialNum
 *                       m_flash4683.scannerFirmwareNum
 *
 * **************************************************************************/
void DirectIoParser::GenerateScannerVersionInformation() 
{
}

/****************************************************************************
 *
 *  Function:   GenerateSerialNumber
 *  Arguments:  char    *tempString - storage for returning the serial number
 *  Returns:    int                 - lenght of serial number
 *
 * **************************************************************************/

int DirectIoParser::GenerateSerialNumber( char *tempString ) {
    int serialLength = 0;
    HSTRING serialNumber(15,true);
    serialNumber.Copy(GET_CONFIG_SETTING(SerNum));
    serialNumber.UniCodeToSimpleAsciiFormat();
    serialNumber.InsertFillChar( '0', (11 - serialNumber.Size) );
    memcpy( tempString, (char *)serialNumber.Char, 11 );
    tempString[11] = 0x00;
    serialLength = strlen( tempString );

    return serialLength;
}

/****************************************************************************
 *
 *  Function:   GenerateProductName
 *  Arguments:  char    *tempString - storage for returning the P_NAME
 *  Returns:    int                 - lenght of P_NAME
 *
 * **************************************************************************/

int DirectIoParser::GenerateProductName( char *tempString )
{
    int ProductNameLength = 0;
    HSTRING ProductName(15,true);
    ProductName.Copy(GET_CONFIG_SETTING(ProductName));
    ProductName.UniCodeToSimpleAsciiFormat();
    ProductName.Move( 2, 0, 4);
    memcpy( tempString, (char *)ProductName.Char, 4 );
    tempString[4] = 0x00;
    ProductNameLength = strlen( tempString );

    return ProductNameLength;
}

/******************************************************************************
 *
 * Function:    Generate_CRC16
 * Arguments:   unsigned int    uiInitValue     -   initial value
 *              unsigned char   *puchData       -   data
 *              unsigned long   ulLength        -   data length
 *
 * Returns:     unsigned int    -   Calculated CRC 16
 *
 *****************************************************************************/

USHORT DirectIoParser::Generate_CRC16(USHORT uiInitValue,UCHAR * puchData,ULONG ulLength) {

    const USHORT uiCRC16Table[] =
    {
        0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
        0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
        0x1231,0x2100,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
        0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
        0x2462,0x3443,0x4200,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
        0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
        0x3653,0x2672,0x1611,0x6300,0x76d7,0x66f6,0x5695,0x46b4,
        0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
        0x48c4,0x58e5,0x6886,0x78a7,0x8400,0x1861,0x2802,0x3823,
        0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
        0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0xa500,0x3a33,0x2a12,
        0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
        0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0xc600,0x1c41,
        0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
        0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0xe700,
        0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
        0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
        0x1080,0xa100,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
        0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
        0x2b10,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
        0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
        0x34e2,0x24c3,0x14a0,0x4810,0x7466,0x6447,0x5424,0x4405,
        0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
        0x26d3,0x36f2,0x6910,0x16b0,0x6657,0x7676,0x4615,0x5634,
        0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
        0x5844,0x4865,0x7806,0x6827,0x18c0,0x8e10,0x3882,0x28a3,
        0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
        0x4a75,0x5a54,0x6a37,0x7a16,0xaf10,0x1ad0,0x2ab3,0x3a92,
        0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
        0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0xcc10,
        0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
        0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0xed1,0x1ef0,
        0x6944
    };

   ULONG i;
   USHORT uiCrc = uiInitValue;

   for (i = 0; i < ulLength; i++) {
	   uiCrc = uiCRC16Table[ ( ( (uiCrc >> 8) ^ puchData[i] ) & 0xff) ] ^ (uiCrc << 8);
   }
   return(uiCrc);
}

/******************************************************************************
 *
 * Function:    Generate_CRC32
 * Arguments:   unsigned int    initval     -   initial value
 *              unsigned char   *data       -   data
 *              unsigned long   length      -   data length
 *
 * Returns:     unsigned long               -   Calculated CRC 32
 *
 *****************************************************************************/
ULONG DirectIoParser::Generate_CRC32(ULONG initval, UCHAR * data, ULONG length)
{
    const ULONG ulCRC32Table[] =
    {
        0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
        0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
        0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
        0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
        0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
        0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
        0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
        0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
        0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
        0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
        0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
        0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
        0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
        0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
        0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
        0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
        0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
        0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
        0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
        0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
        0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
        0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
        0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
        0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
        0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
        0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
        0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
        0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
        0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
        0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
        0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
        0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
        0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
        0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
        0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
        0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
        0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
        0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
        0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
        0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
        0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
        0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
        0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
    };

    ULONG i;
    ULONG ulCRC = initval;

    for (i = 0; i < length; i++)
    {
       ulCRC = ulCRC32Table[(ulCRC^(data[i]))&0xff]^(ulCRC >> 8);
    }
    return (ulCRC);
}


