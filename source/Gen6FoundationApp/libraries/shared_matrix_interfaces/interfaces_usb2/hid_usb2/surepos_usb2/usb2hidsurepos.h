/* ==============================================================================
   USB HID for IBM SurePos Module header
//=================================================================================
   $Source: interfaces_usb2/hid_usb2/surepos_usb2/usb2hidsurepos.h $
   $Revision: 1.6.1.3 $
   $Date: 2010/09/27 14:59:51EDT $

//=================================================================================

//=================================================================================*/
//! \file

#ifndef _USB2HID_SUREPOS_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define _USB2HID_SUREPOS_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "Usb2Hid.h"
#include "usb2SurePos.h"
#include "usbdesc.h"
#include "UsbRem.h"

enum SurePosEventw_t
{
	Sig_FlashCommandReceived = Sig_CustomEvent1
};

class CBarcodeData;
class DirectIoParser;

class CUsb2HidSurePos : public CUsb2Hid
{	
protected:
	enum
	{
		// define the interface metrics 
		DATA_INTERFACE = 0,
		FLASH_INTERFACE= 1,
		MENU_BUFFER_SIZE = 570                          //enlarged due to addition of directIO   lmc
	};	
	const static THidReportTable<CUsb2HidSurePos> ms_DataReports[];
	const static THidReportTable<CUsb2HidSurePos> ms_FlashReports[];

public:
	CUsb2HidSurePos(CRxFifo *pRxFifo, CThreadRealTime *pThread, bool TableTop);
	virtual ~CUsb2HidSurePos(void);
	
	virtual usb_err_t ReportHandler(void);
	virtual usb_err_t Ep0DataHandler(void);
	virtual int GetReportSize(DEVICE_REQUEST *pRequest);

	virtual MatrixError_t ExtractParameters(const tichar_t *pParameters );
	virtual int SendLabel(CBarcodeData *pLabel);
	virtual void StoreMenuResponse(CBarcodeData *pMenuResponse);
	
	int SendLargeLabel(CBarcodeData *pLabel);

	usb_err_t EP0_InEmptyHandler(void);
	usb_err_t OutReportHandler(void);

	SurePosOutReportBuffer_t *GetOutReport(void)
	{		
		const UCHAR *pTemp = (UCHAR *)ControlData.dataBuffer;
		return (SurePosOutReportBuffer_t*) pTemp;
	}

	void TranslateSurePosCommand(void)
	{
		TranslateSurePosCommand(GetOutReport());
	}
	
protected:
	void InitUSBEndpDes(bool TableTop);
	virtual UCHAR GetSizeOfConfigDesc(void) {return sizeof(MyConfigDescriptor_t);}  // this includes ReM
	virtual UCHAR GetMaxNumIfaces(void) {return 3;}  // this includes ReM
	virtual UCHAR GetPidOffset(bool bIsRem);
	virtual void EPRxHandleReceivedData(UINT pEp);

	void TranslateIntoBCD (SurePosInReportBuffer_t *pInReport, CBarcodeData *pLabel);

protected:
	typedef struct
	{
		ConfigDescriptor_t ConfigDesc; 
		InterfaceDescriptor_t ScannerIfaceDesc;
		HIDDescriptor1_t ScannerHidDesc;
		EndpointDescriptor_t ScannerInEndpoint;
		InterfaceDescriptor_t FlashUpdateIfaceDesc;
		HIDDescriptor1_t FlashUpdateHidDesc;
		EndpointDescriptor_t FlashUpdateInEndpoint;
		EndpointDescriptor_t FlashUpdateOutEndpoint;

		CUsbRem::IfaceDescriptor_t RemIfaceDesc;
	} MyConfigDescriptor_t;

	static MyConfigDescriptor_t m_MyConfigDescriptor;
	MyConfigDescriptor_t m_MyOtherSpeedConfigDescriptor;
	bool m_bIsTableTop;

	usb_err_t EP0_FlashInStatusHandler(void);
	usb_err_t EP0_FlashOutReportHandler(void);
		
	void SendFlashStatus(UCHAR FlashStatus, bool RequestCameFromEP0);
	void FlashSystemCommandDispatcher(const SurePosFlashOutReportBuffer_t *pOutReport);

public:
	void FlashCommandDispatcher(void);
	bool IsFlashing(void)			{ return (m_ucStatus0 & OST0_FLASH_IN_PROGRESS) ? true : false;	}

public:
	void InitialUpdateConfig(void)	{ QueryScannerConfig();	}
	
private:
	UCHAR m_ucStatus0;		// Status byte 
	UCHAR m_ucStatus1;		// Status byte 	

	bool	m_IgnoreConfigFlag;	// makes the setup easier if the SurePos is configured wrong
	bool	m_IgnoreDisableFlag;	// makes the setup easier if the SurePos is configured wrong
	bool	m_IgnoreBeepFlag;	// makes the setup easier if the SurePos is configured wrong, ignores register configuration of beeper
	bool	m_SelectOCRFlag;	// makes the setup easier if the SurePos is configured wrong, 0 selects OCR A and 1 selects OCR B
	bool	m_UseBCDFlag;			// some older versions of the OS4690 (< 2.4) need the EAN/UPC as BCD

	UCHAR m_CurrentConfig[ConfigSize + DirectIoConfigSize];         //10 bytes total,  cfg 0-8 first, then direct/io config byte
	UCHAR m_ucTwoLabelFlagConfig[TWO_LABEL_CONFIG_SIZE];

	// helpers to translate the 4690 commands to scanner commands. See SurePosXlate.cpp for the sources.
	void TranslateSurePosCommand(SurePosOutReportBuffer_t *pOutReport);
	void SurePosConfigTwoLabelFlags(SurePosInReportBuffer_t *pInReport); //, SurePosOutReportBuffer_t *pOutReport);
	bool SurePosConfigScanner(SurePosOutReportBuffer_t *pOutReport);
	bool SurePosDirectIoConfigScanner(SurePosOutReportBuffer_t *pOutReport);
	void TranslateSurePosSystemCommand(SurePosInReportBuffer_t *pInReport, SurePosOutReportBuffer_t *pOutReport);
	void TranslateSurePosDirectIoCommand(SurePosInReportBuffer_t *pInReport, SurePosOutReportBuffer_t *pOutReport);
	void SurePosSendCommandNotSupported(SurePosInReportBuffer_t *pInReport);
	void SurePosSendCommandRejectReport (SurePosInReportBuffer_t *pInReport);
	void SurePosFillAndSendStatusReport(SurePosInReportBuffer_t *pInReport);
	bool SurePosConfigValidateAndCorrect (SurePosOutReportBuffer_t *pOutReport);
	void SurePosSendStatusReport (SurePosInReportBuffer_t *pInReport);
	
	void DirectIOCommandUndefined(SurePosInReportBuffer_t *pInReport);
	void DirectIOCommandAccepted(SurePosInReportBuffer_t *pInReport, bool withData);
	void DirectIOCommandNotAllowed(SurePosInReportBuffer_t *pInReport);

	void ResetMenuBuffer(void)		{ m_CurrentMenuIndex = 0;	};
	void AppendToMenuBuffer(const bchar_t *pFormat, ...);
	void AppendCmdToMenuBuffer(const bchar_t *pCommand, const bchar_t *pAction);
	void AppendQueryCmdToMenuBuffer(const bchar_t *pCommand);
	void QueryScannerConfig(void);

	bool CompareMenuResponse(const bchar_t *ResponseBuffer, size_t ResponseSize);
	bool ParseMenuResponse_bool(const bchar_t *pResponse, const bchar_t *pName, UCHAR &Byte, UCHAR Mask);
	bool ParseMenuResponse_bool(const bchar_t *pResponse, const bchar_t *pName[], int count, UCHAR &Byte, UCHAR Mask);
	bool ParseMenuResponse_int(const bchar_t *pResponse, const bchar_t *pName, UCHAR &Byte, UCHAR Mask);
	bool ParseMenuResponse(bchar_t *pConfig);
	bool TalkToReaderConfig(void);
	void DebugSend(const bchar_t *pBin, size_t Size);  // a debug helper
	void SendOneMenu(const bchar_t * str);

	size_t m_CurrentMenuIndex;
	bchar_t m_MenuBuffer[MENU_BUFFER_SIZE+1];

	DirectIoParser *dioParser;
// flash data
	UCHAR m_ucFlashStatus0;		// Status byte 
	bool m_bFlashResetStatus0;		// Flash Finished Reset Status Byte 

	volatile UINT totalDEP;
};

#endif	/* _USB2HID_SUREPOS_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */





