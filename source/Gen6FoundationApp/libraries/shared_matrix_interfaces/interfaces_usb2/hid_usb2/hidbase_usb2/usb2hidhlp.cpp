/* ==============================================================================
   USB HID for OEM POS Module Source
   ==============================================================================

   $RCSfile: interfaces_usb2/hid_usb2/hidbase_usb2/usb2hidhlp.cpp $
   $Revision: 1.8 $
   $Date: 2011/03/31 14:38:47EDT $

//=================================================================================

//=================================================================================*/
//! \file


#include "stdInclude.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "product.h"
#include "usb2.h"

#include "USB2HidHlp.h"
#include "BarcodeData.h"

#include "timeout.h"

CUsb2HidPosHelper::CUsb2HidPosHelper(CRxFifo *pRxFifo, CThreadRealTime *pThread)
: CUsb2Hid(pRxFifo, pThread)
{
	totalDEP = 0;
}

CUsb2HidPosHelper::~CUsb2HidPosHelper(void)
{	
}

/* This table lists all known HID reports */
const THidReportTable<CUsb2HidPosHelper> CUsb2HidPosHelper::ms_PosReports[] =
{
	// type,			 report ID,				size,					Handler
	// IN
	USB_HID_REPORT_IN,		 HID_USBPOS_IN_REPORT_ID,		HID_USBPOS_IN_REPORT_SIZE,		&CUsb2HidPosHelper::EP0_PosInEmptyHandler,
	USB_HID_REPORT_FEATURE_IN,	 HID_POS_REDIRECT_REPORT_ID,		HID_POS_FEATURE_REDIRECT_SIZE,		&CUsb2HidPosHelper::FeatureInRedirectHandler,
	// OUT
	USB_HID_REPORT_OUT,		 HID_POS_OUT_1252_REPORT_ID,		HID_POS_OUT_REPORT_SIZE,		&CUsb2HidPosHelper::EP0_Out1252Handler,
	USB_HID_REPORT_OUT,		 HID_POS_OUT_TRIGGER,			2,					&CUsb2HidPosHelper::EP0_OutTriggerHandler,
	USB_HID_REPORT_FEATURE_OUT,	 HID_POS_OUT_TRIGGER,			2,					&CUsb2HidPosHelper::EP0_OutTriggerHandler,
	USB_HID_REPORT_FEATURE_OUT,	 HID_POS_REDIRECT_REPORT_ID,		HID_POS_FEATURE_REDIRECT_SIZE,		&CUsb2HidPosHelper::FeatureOutRedirectHandler,
	// END
	USB_HID_REPORT_TABLE_END, 	 0,					0,					NULL
};

usb_err_t CUsb2HidPosHelper::EP0_PosInEmptyHandler(void)
{
	UsbPosInReportBuffer_t *pReport = (UsbPosInReportBuffer_t*) dataToEP; //ControlData.dataBuffer;
	// send an "all keys up" here
	pReport->Clear();
	pReport->SetReportId(HID_USBPOS_IN_REPORT_ID);
	
	code_transmit((UCHAR *)dataToEP, pReport->GetBufferSize());
		
	return RET_ACK;
}

usb_err_t CUsb2HidPosHelper::FeatureInRedirectHandler(void)
{
	UsbCommonHidReportBuffer_t *pReport = (UsbCommonHidReportBuffer_t *) dataToEP; //ControlData.dataBuffer;
	// send an "feature in" report here 
	pReport->SetReportId(HID_POS_REDIRECT_REPORT_ID);
	pReport->SetInterface(m_ucDestinationInterface);
	
	code_transmit((UCHAR *)dataToEP, pReport->GetFeatureRedirectSize());
		
	return RET_ACK;
}

usb_err_t CUsb2HidPosHelper::FeatureOutRedirectHandler(void)
{
	UsbCommonHidReportBuffer_t *pReport = (UsbCommonHidReportBuffer_t *)ControlData.dataBuffer;//dataToEP;
	if(pReport->GetInterface() < m_ucNumInterfaces)
	{
		m_ucDestinationInterface = pReport->GetInterface();	
	}

	return RET_ACK;		
}

static const UCHAR szMnuTrigger[]  = "\x02T\r";
static const UCHAR szMnuUnTrigger[]= "\x02U\r";
static const UCHAR szMnuStdBeep[]  = "\x02\x07\r";
static const UCHAR szMnuErrorBeep[]  = "\x02\x07\x65\r";

#define HID_USBPOS_OUT_DISABLE					0x02
#define HID_USBPOS_OUT_TRIGGER					0x04
#define HID_USBPOS_OUT_ERROR_BEEP				0x20
#define HID_USBPOS_OUT_GOOD_READ_BEEP				0x40


usb_err_t CUsb2HidPosHelper::OutTriggerHandler(UINT)
{
	
	UsbCommonHidReportBuffer_t *pReport = (UsbCommonHidReportBuffer_t*) ControlData.dataBuffer;
	UCHAR ucCmd = pReport->GetTriggerCmd();
			
	if(ControlData.wCount < 2)
	{
		return RET_STALL;
	}

	if (ucCmd & HID_USBPOS_OUT_DISABLE)	// we ignore other bits if we get disabled 
	{
		m_ScannerStatus.ScannerDisabled = TRUE;
		DisableTx();
	}
	else
	{
		m_ScannerStatus.ScannerDisabled = FALSE;
		EnableTx();
						
		if (ucCmd & HID_USBPOS_OUT_GOOD_READ_BEEP)
		{
			Beep();
		}
		else if (ucCmd & HID_USBPOS_OUT_ERROR_BEEP)
		{
			ErrorBeep();
		}
		if (ucCmd & HID_USBPOS_OUT_TRIGGER)
		{
			StuffInputBuffer( szMnuTrigger, 3 );
		}
		else
		{
			StuffInputBuffer( szMnuUnTrigger, 3 );
		}
	}	
	return RET_ACK;
}

usb_err_t CUsb2HidPosHelper::EP0_OutTriggerHandler(void)
{
	return OutTriggerHandler(0);
}

usb_err_t CUsb2HidPosHelper::Out1252Handler(UINT)
{
	UsbCommonHidReportBuffer_t *pReport = (UsbCommonHidReportBuffer_t*) ControlData.dataBuffer;
	if(ControlData.wCount < 2)
	{
		return RET_STALL;
	}

	StuffInputBuffer( pReport->GetContentBuffer(), pReport->GetContentLength() );
	return RET_ACK;
}

usb_err_t CUsb2HidPosHelper::EP0_Out1252Handler(void)
{
	return Out1252Handler(0);
}

inline void CUsb2HidPosHelper::EPRxHandleReceivedData(UINT pEp)
{			
	if(m_dataBufCtrl)
	{	
		m_dataBufCtrl = false;
		datalenEP = MX25USB2_ReadEndpoint_Data(pEp, rxMaxLenIDs, false, dataFromEPsec);  //endpoint 7	

        	if(datalenEP > 0)  
        	{
			memcpy((UCHAR *)dataForHandler+totalDEP, (UCHAR *)dataFromEP, datalenEP);
			totalDEP += datalenEP;		
		}
	}		
	else
	{
		m_dataBufCtrl = true;
		datalenEP = MX25USB2_ReadEndpoint_Data(pEp, rxMaxLenIDs, false, dataFromEP);  //endpoint 7	

        	if(datalenEP > 0)  
        	{
			memcpy((UCHAR *)dataForHandler+totalDEP, (UCHAR *)dataFromEPsec, datalenEP);
			totalDEP += datalenEP;		
		}
	}
	
	if (((datalenEP <= 2) && (datalenEP > 0)) || (totalDEP >= HID_POS_OUT_REPORT_SIZE))
	{	
		totalDEP = 0;	
					
		UsbCommonHidReportBuffer_t *pReport = (UsbCommonHidReportBuffer_t*) dataForHandler;						
		UCHAR ucCmd = pReport->GetTriggerCmd();
		switch(pReport->GetReportId())
		{
			case HID_POS_OUT_TRIGGER:
				if(datalenEP < 2)
				{
					return ;
				}

				if (ucCmd & HID_USBPOS_OUT_DISABLE)	// we ignore other bits if we get disabled 
				{

					m_ScannerStatus.ScannerDisabled = TRUE;
					DisableTx();
				}
				else
				{
					m_ScannerStatus.ScannerDisabled = FALSE;
					EnableTx();
						
					if (ucCmd & HID_USBPOS_OUT_GOOD_READ_BEEP)
					{
						Beep();
					}
					else if (ucCmd & HID_USBPOS_OUT_ERROR_BEEP)
					{
						ErrorBeep();
					}
					if (ucCmd & HID_USBPOS_OUT_TRIGGER)
					{
						StuffInputBuffer( szMnuTrigger, 3 );
					}
					else
					{
						StuffInputBuffer( szMnuUnTrigger, 3 );
					}
				}

				break;
					
			case HID_POS_OUT_1252_REPORT_ID:
				if(datalenEP < 2)
				{
					return ;
				}
				StuffInputBuffer( pReport->GetContentBuffer(), pReport->GetContentLength() );

				break;
			default:
				break;
		}
	}
}

// Sends a "no data" report 
void CUsb2HidPosHelper::HidPosSend0Label(int)
{
}

int CUsb2HidPosHelper::HidPosSendLabel(int nInterface, CBarcodeData *pLabel)
{
	int ret =0;

	if(IsTxEnabled())
	{
		ret = -1;
		ASSERT(nInterface < NUM_HID_INTERFACES);

		UsbPosInReportBuffer_t *pReport = (UsbPosInReportBuffer_t *)dataToEP;
		UINT size=HID_USBPOS_LABEL_SIZE;
		size_t RestSize=0;
	
		const UCHAR *pCurrentText = NULL;   

		pLabel->GetTotalData(pCurrentText, RestSize);

			while (RestSize>0 )
			{				
				pReport->Clear();
				pReport->SetReportId(HID_USBPOS_IN_REPORT_ID);
				size=RestSize;
				// fill the "more data to follow" flag
				if(size > HID_USBPOS_LABEL_SIZE)
				{
					size=HID_USBPOS_LABEL_SIZE;
					pReport->SetPadding(1);
				}

				// fill size info
				pReport->SetSize(size);
				// fill symbology ID info
				pReport->SetAimStart	(']');
				pReport->SetAimId		(pLabel->GetAimId());
				pReport->SetAimModifier(pLabel->GetAimModifier());
				pReport->SetCodeID	(pLabel->GetHHPId());
				pReport->SetCodeModifier(pLabel->GetHHPModifier());

				// now copy the label data
				memcpy(pReport->GetContentBuffer(), pCurrentText, size);
				SendReportWithTest(nInterface, HID_USBPOS_IN_REPORT_SIZE, (UCHAR *)dataToEP);

				pCurrentText += size;
				RestSize -= size;
			}
			ret= 0;
	}

	return ret;
}

void CUsb2HidPosHelper::Beep(void)
{
	StuffInputBuffer( szMnuStdBeep, 3 );
}

void CUsb2HidPosHelper::ErrorBeep(void)
{
	StuffInputBuffer( szMnuErrorBeep, 4 );
}


