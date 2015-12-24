/*=============================================================================
 * This file contains functions that implement direct IO commands for 4683
 *

 *=============================================================================
 * $RCSfile: interfaces/DirectIoParser.h $
 * $Revision: 1.3 $
 * $Date: 2010/10/05 14:09:14EDT $
 *============================================================================*/

#ifndef DIRECTIOPARSER_H_
#define DIRECTIOPARSER_H_

#include <stddef.h>
#include "language.h"
#include "thread.h"

#define _U(x)	(const bchar_t*)(x)
#define DIO_RESPONSE_SIZE 264

enum
{
	DIO_UNDEFINED = 0,
	DIO_ACCEPTED,
	DIO_ACCEPTED_WITH_DATA,
	DIO_NOT_ALLOWED
};

unsigned int TF_GetIntFromBuffer( unsigned char * Buffer, int offset );
void TF_ResetValues(void);
void ProcessDioDownload(void);

// Spoof Gen 5 P_name and other responses for testing with legacy applications
#define SPOOF_GEN_5 0
// Enable/disable direct IO configuration command debugging
#define DIO_DEBUG 0
// Enable/disable trickle feed/flashscan debugging
#define WALMART_DEBUG 0

#if WALMART_DEBUG
void AddStringToLogFile( const char* msg, int length );
#else
void AddStringToLogfile( const char *, int);
#endif


class DrvMenuConfig;
class CBarcodeData;
class CRxFifo;

class DirectIoParser
{
public:
	DirectIoParser(CRxFifo *fifo);
	virtual ~DirectIoParser();
	
	void StoreMenuResponse(CBarcodeData *pMenuResponse);
	char ExecuteDirectIOCommand(unsigned char * Message, int Length);
	bool GetExpectingMenuResponse(void);
	int GetResponse( unsigned char **result );

private:
	// Private copy constructor to fix klockwork bugs
	DirectIoParser (const DirectIoParser&);            // declared private, but not defined, to prevent copying         
	DirectIoParser& operator=(const DirectIoParser&);  // declared private, but not defined, to prevent copying 

	int GenerateFirmwareVersion( char *tempString );
	int GenerateSerialNumber( char *tempString );
	int GenerateProductName( char *tempString );
	void GenerateScannerVersionInformation();
	USHORT Generate_CRC16(unsigned short uiInitValue,unsigned char * puchData,unsigned long ulLength);
	ULONG  Generate_CRC32(unsigned long initval, unsigned char * data, unsigned long length);
	void SetDioStatus( int stat );
	// IBM Reserved Direct IO Commands:
	void IbmEnableAdditionalSymbologies( unsigned char *message );
	void IbmGetDeviceInformaiton(void);
	// Direct IO Commands for Wal-Mart:
	char DIO_EnterFlashMode();
	char DIO_RequestPartNumber();
	char DIO_RequestPeripheralID();
	char DIO_SelectFlashSection(unsigned char * Message);
	char DIO_GetCRC16();
	char DIO_GetCRC32();
	char DIO_EraseSelectedSection();
	char DIO_SelectFlashOffset(unsigned char * Message);
	char DIO_UpdateDevice();
	char DIO_ResetDevice();
	char DIO_RequestScannerFirmwareVersion();
	char DIO_IncrementFirmwareFlashCounter();
	char DIO_FlashData(unsigned char * Message, int Length);
	char DIO_SendConfigurationA(unsigned char * Message, int Length);
	char DIO_SendConfigurationB(unsigned char * Message, int Length);
	char DIO_ReportConfiguration();
	char DIO_FullUnitID();
	char DIO_FWNewVersionReport();
	char DIO_FWVersionReport();
	char DIO_SerialNumberReport();
	char DIO_0H0Command();
	char DIO_Beep4683();
	char TF_EnterTrickleFeed();
	char TF_GetDownloadOffset();
	char TF_TrickleFeedComplete();
	char TF_PauseTrickleFeed();
	char TF_TerminateTrickleFeed();
	char TF_ActivateNewFirmware();
	virtual bool ParseMenuResponse(bchar_t *pConfig);
	virtual bool TalkToReaderConfig(int type);
	virtual void QueryScannerConfig(void);
	bool ParseMenuResponseA(bchar_t *pConfig);
	bool ParseMenuResponseB(bchar_t *pConfig);
	bool ParseMenuResponseBattry(bchar_t *pConfig);
	void StuffInputBufferAndWaitForMenuResponse(const bchar_t *pMenuString);
	void StuffInputBuffer( const char* pcInputData );
	void DebugSend(const bchar_t *pBin, size_t Size);  // a debug helper

protected:
	unsigned char	DioMessage[DIO_RESPONSE_SIZE];	// pointer to response message 
	volatile int	DioDataLength;			// length of the response message sent
	DrvMenuConfig 	*configHelper; 			// Config helper class
	MT_Event_t		DirectIOResponseEvent;
	int				DioStatus;				// Current status of
	bool 			m_ExpectingMenuResponse;
	CRxFifo			*RxFifo;  
};

#endif /*DIRECTIOPARSER_H_*/


