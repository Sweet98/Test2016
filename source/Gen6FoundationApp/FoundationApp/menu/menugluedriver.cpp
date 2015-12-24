/*=================================================================================
  class: CMenuGlue
  Handles all the menu glue for the io drivers
//=================================================================================
   $Source: Gen6FoundationApp/FoundationApp/menu/menugluedriver.cpp $
   $Date: 2011/06/30 15:25:50EDT $
   $Revision: 1.19 $
 

//=================================================================================*/

#include <wchar.h>

#include "stdInclude.h"
#include "MenuGlueDriver.h"
#include "IoHelpers.h"
#include "if_table.h"
#include "BarcodeData.h"	               // because of data formats
#include "HSTRING.h"
#include "language.h"

#include "menu.h"
#include "ReaderConfiguration.h"
#include "productconfiguration.h"                                             //lmc   fixme



extern char* ReportProjectGenString(void);


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
	CMenuGlueDriver is the interface between the drivers and the menu system.
	It is smarter than is old IT3800 counterpart.
*/

const  wchar_t *ppucWordFormats[15] =
    {
        _TI("N71"), _TI("N72"), _TI("N81"), _TI("E71"), _TI("E72"), _TI("E81"), _TI("O71"), _TI("O72"), _TI("O81"),
        _TI("S71"), _TI("S72"), _TI("S81"), _TI("M71"), _TI("M72"), _TI("M81")
    };

const  wchar_t *ppucWandEmRates[7] =
    {
        _TI("10"), _TI("25"), _TI("40"), _TI("80"), _TI("120"), _TI("150"), _TI("200")
    } ;
const  wchar_t *ppucPolarity[2] =
    {
        _TI("BLK"), _TI("WHT")
    } ;
const  wchar_t *ppucIdleState[2] =
    {
        _TI("LO"), _TI("HI")
    } ;

// Block Size in characters (only valid when transmitting PDF)
const unsigned int WandEmBlockSizes[4] =
    {
        20, 40, 60, 80
    } ;
// Block delay in "Periodic Interrrupt Durations" (only valid when transmitting PDF)
const unsigned int WandEmBlockDelays[4] =
    {
        5*MilliSec, 50*MilliSec, 150*MilliSec, 500*MilliSec
    } ;

const  int HHLCRates[2] =
    {
        36, 100
    };
const  wchar_t *ppucBaudRates[13] =
    {
        _TI("300"), _TI("600"), _TI("1200"), _TI("2400"), _TI("4800"), _TI("9600"), _TI("19200"), _TI("38400"), _TI("57600"),
        _TI("115200"), _TI("230400"), _TI("460800"), _TI("921600")
    };

const ULONG OutPutFormats[] =
    {
        IO_DATATYPE_BINARY,		// plain binary of unknow structure
        IO_DATATYPE_CP_SYS,		// system codepage
        IO_DATATYPE_UTF8,			// Unicode in UTF8 encoding
        IO_DATATYPE_UTF16BES,	// Unicode big endian as byte stream
        IO_DATATYPE_UTF16LES,	// Unicode little endian as byte stream
    };

static int GetSettingAndCheckBorders(int minval, int maxval, int value)
{
	if(value > maxval)
		value = maxval;
	if(value < minval)
		value = minval;
	return value;
}

//CMenuGlueDriver	theMenuGlue;

// Fill in the Interface Parameter String depending on the interface type
// We use swprintf now instead of _sntprintf to ensure there are no buffer overflows
MatrixError_t CMenuGlueDriver::GetSetting(USHORT usType, tichar_t *pBuffer, size_t BufferSize)
{
	MatrixError_t RetVal = ERR_PASS;
	int nRetSize=0;
	pBuffer[0] = 0;
	memset( pBuffer, 0x00, BufferSize*sizeof(tichar_t) );

	switch(usType)
	{
	case CInterfaceTable::MinParams:
		break;
	case CInterfaceTable::Serial:
#if defined( EMERALD_VERIFIER )

		nRetSize = swprintf( pBuffer, BufferSize,
		                     _TI("%ls,%ls,XON0,ACK0,CTS0"),
		                     ppucBaudRates[GET_SETTING(RS232BaudRate)],
		                     ppucWordFormats[2]);

#elif defined( RFSCAN )

#else

//ewr-02787
		nRetSize = swprintf( pBuffer, BufferSize,
            _TI("%ls,%ls,XON%1d,ACK%1d,CTS%1d,DLYCTS%i,PKT%1d,NAK%1d,DLK%i"),
				ppucBaudRates[GET_SETTING(RS232BaudRate)],
				ppucWordFormats[GET_SETTING(RS232WordFormat)],
 				GET_SETTING(RS232XonEnable),
				GET_SETTING(RS232AckEnable),
				GET_SETTING(RS232CtsEnable),
				GET_SETTING(RS232CtsDelay),
				GET_SETTING(RS232PacketMode),
            GET_SETTING(RS232PacketModeNakEnable),
            GET_SETTING(RS232AckNakDelay) );
            
#endif

		break;
	case CInterfaceTable::UsbComPort:
		nRetSize = swprintf( pBuffer, BufferSize,
		                     _TI("ACK%d,CTS%d,SER%d,RCVYTIM%d,SPD%1d,REMIFC%d"),
		                     GET_SETTING(UsbAckEnable),
		                     GET_SETTING(UsbCtsEnable),
		                     GET_SETTING(UsbSerialNumber),
		                     GET_SETTING(UsbReEnumerationTimeout),
		                     GET_SETTING(UsbHighspEnable),
							 GET_SETTING(RemIfaceEnable));
		break;
	case CInterfaceTable::UspHidPos:
		nRetSize = swprintf( pBuffer, BufferSize,
		                     _TI("RCVYTIM%d,REMIFC%d"),
		                     GET_SETTING(UsbReEnumerationTimeout),
							 GET_SETTING(RemIfaceEnable));
		break;
	case CInterfaceTable::Wedge:
		nRetSize = swprintf( pBuffer, BufferSize,
		                     _TI("Country%02d,Style%1d,Mode%1d,AutoDC%1d,CtrlAsc%1d,Turbo%1d,NonPrt%1d,NumPad%1d,DeadK%1d,AltNp%1d,Html%1d,Hex%1d,RCVYTIM%d,REMIFC%d"),
		                     GET_SETTING(KBD_CtryCode),
		                     GET_SETTING(KBD_Style),
		                     GET_SETTING(KBD_Mode),                                           
		                     GET_SETTING(KBD_AutoDirCon),
		                     GET_SETTING(KBD_CtrlASCII),
		                     GET_SETTING(KBD_TurboMode),
		                       GET_SETTING(KBD_NonPrint),
		                     GET_SETTING(KBD_NumKeypad),
		                     GET_SETTING(KBD_TransDeadKey),
		                     GET_SETTING(KBD_TransAltNum),
		                     GET_SETTING(KBD_TransHtml),
		                     GET_SETTING(KBD_TransHex),
		                     GET_SETTING(UsbReEnumerationTimeout),
							 GET_SETTING(RemIfaceEnable));
		break;
	//ewr-02787		
	case CInterfaceTable::SerialWedge:
		nRetSize = swprintf( pBuffer, BufferSize,
							 _TI("%ls,%ls,XON0,ACK0,CTS0,DLYCTS0,PKT0,NAK0,WedgeMode%1d"),
		                     ppucBaudRates[GET_SETTING(RS232BaudRate)],
		                     ppucWordFormats[GET_SETTING(RS232WordFormat)],
		                     GET_SETTING(SerWdgConfig)) ;
		break;

	case CInterfaceTable::Ibm4683:
		nRetSize = swprintf( pBuffer, BufferSize,
		                     _TI("RTLSCN%1d,RTLSER%1d,RTLIFD%1d,BEPLVL%1d,RTLIGN%i"),
		                     GET_SETTING(IBM_4683_Protocol),
		                     GET_SETTING(IBM_4683_Serial),
		                     GET_SETTING(IBM_4683_IgnoreFirstDisable),
		                     GET_SETTING(BASE_SCANNER_SELECT_BeepVolume),
		                     GET_SETTING(UsbSurePosIgnCfg) ? 1 : 0);
		break;

	case CInterfaceTable::IbmSurePos:
		nRetSize = swprintf( pBuffer, BufferSize,
		                     _TI("RTLOCR%i,RTLBEP%i,RTLIGN%i,RTLBCD%i,RTLIDS%i,RCVYTIM%d,REMIFC%d"),
		                     GET_SETTING(UsbSurePosSelectOCR) ? 1 : 0,
		                     GET_SETTING(UsbSurePosIgnBep) ? 1 : 0,
		                     GET_SETTING(UsbSurePosIgnCfg) ? 1 : 0,
		                     GET_SETTING(UsbSurePosUseBCD) ? 1 : 0,
		                     GET_SETTING(UsbSurePosIgnDis) ? 1 : 0,
		                     GET_SETTING(UsbReEnumerationTimeout),
							 GET_SETTING(RemIfaceEnable));
		break;
	case CInterfaceTable::WandEm:
		nRetSize = swprintf( pBuffer, BufferSize,
		                     _TI("TXRATE%ls,%ls,%ls,C39FULLASC%1d,BLKSIZE%i,BLKDLY%i,BLKSUM%i"),
		                     ppucWandEmRates[GET_SETTING(WandEmRate)],
		                     ppucPolarity[GET_SETTING(WandPolarity)],
		                     ppucIdleState[GET_SETTING(WandEmIdleState)],
		                     GET_SETTING(C39FullAscii),
		                     WandEmBlockSizes[GET_SETTING(WandEmBlockSize)],
		                     WandEmBlockDelays[GET_SETTING(WandEmBlockDelay)],
		                     GET_SETTING(WandEmChecksum)) ;
		break;
	case CInterfaceTable::HHLC:
		{
			HSTRING	Temp("0E01070!", true);	// turn off buffered scans
			ReaderConfiguration->Menu(&Temp);
			nRetSize = swprintf( pBuffer, BufferSize,
			                     _TI("Rate%i,Polarity%i,Idle%i,C39FullAsc%1d,BlkSize%i,BlkDly%i,BlkSum%i,MinMod%i,Stretch%i,Zoom%i,Vote%i,PolLasEna%i"),
			                     HHLCRates[GET_SETTING(HHLCRate)],
			                     GET_SETTING(HHLCPolarity),
			                     GET_SETTING(HHLCIdleState),
			                     GET_SETTING(C39FullAscii),
			                     WandEmBlockSizes[GET_SETTING(WandEmBlockSize)],
			                     WandEmBlockDelays[GET_SETTING(WandEmBlockDelay)],
			                     GET_SETTING(WandEmChecksum),
			                     GET_SETTING(HHLCMinModuleSize),
			                     GET_SETTING(HHLCStretchFactor),
			                     GET_SETTING(HHLCZoomFactor),
			                     GET_SETTING(HHLCVotingTimeout),
			                     GET_SETTING(HHLCLaserEnablePolarity)
			                   );
		}
		break;
	case CInterfaceTable::Ocia:
		nRetSize = swprintf( pBuffer, BufferSize,
		                     _TI("ProtocolFormat%d,W84CLKDLY%02d,TriggerMode%02d"),
		                     GET_SETTING(OCIA_Protocol),
		                     GET_SETTING(IntCharDelay),
		                     GET_SETTING(TriggerMode));
		break;
	case CInterfaceTable::PicoNet:
	case CInterfaceTable::BlueTooth:
	default:
		RetVal = ERR_NO_SUCH_INTERFACE_TYPE;
		break;
	}

	if(nRetSize >= 0)
	{
		// All drivers need this, so we add it at a common place
		nRetSize = swprintf( pBuffer+nRetSize, BufferSize-nRetSize,
		                     _TI(",IC_X_DLY%02d,DelayChar%02d,ICDLY%02d,IFDLY%02d,IMDLY%02d,EciOut%d"),
		                     GET_SETTING(IntCharXDelay),
		                     GET_SETTING(DelayChar),
		                     GET_SETTING(IntCharDelay),
		                     GET_SETTING(IntFuncDelay),
		                     GET_SETTING(IntMsgDelay),
		                     OutPutFormats[GetSettingAndCheckBorders(0, 4, GET_SETTING(EciOutputFormat))]);
	}
	if(nRetSize < 0)
	{
		RetVal = ERR_BADPARAM;
	}
	return RetVal;
}

// Below are helpers to isolate the USB interfaces from the menu system
HSTRING *CMenuGlueDriver::GetVendorName(void)
{
	return GET_CONFIG_SETTING(VendorName);
}

HSTRING *CMenuGlueDriver::GetProductName(void)
{
	return GET_CONFIG_SETTING(ProductName);
}

HSTRING *CMenuGlueDriver::GetSerNumString(void)
{
	return GET_SETTING(USBSerNum);
}

USHORT CMenuGlueDriver::GetUsbPID(void)
{
	return GET_CONFIG_SETTING(iUsbPid);
}

USHORT CMenuGlueDriver::GetUsbVID(void)
{
	return GET_CONFIG_SETTING(iUsbVid);
}

#define CHARACTER_COUNT  6

int CMenuGlueDriver::GetPartNumFromMenu(void)
{
	char cSwTemp[CHARACTER_COUNT+1];
	int  iPartNum = 0;
	char *pSW_GEN_TEMP = GET_SETTING(ShowWARev); //: BI123456AAC, for example
	pSW_GEN_TEMP +=4; //point to beginning digits, skip ": Bx"

	for(int i = 0; i < CHARACTER_COUNT; i++)
	{
		cSwTemp[i] = *pSW_GEN_TEMP;
		pSW_GEN_TEMP++;
	}

	cSwTemp[CHARACTER_COUNT] = '\0';
	iPartNum = atoi(cSwTemp);
	return iPartNum;
}

USHORT CMenuGlueDriver::GetUsbGen(void)
{
	USHORT usUsbGeneration = 0;
	unsigned int uiPartNumFromMenu = 0;
	unsigned int uiUsbGenFromTestSet = 0;

	usUsbGeneration = (USHORT)GET_CONFIG_SETTING(iUsbGen);    //get product config setting
	uiUsbGenFromTestSet = GET_SETTING(UsbSurePosGenNum);      //get back door menu setting

	if(usUsbGeneration == 0)	// no USBGEN set in menu
	{
		uiPartNumFromMenu = GetPartNumFromMenu();
		usUsbGeneration = (USHORT)uiPartNumFromMenu / 256;
	}

	if(uiUsbGenFromTestSet > 0)
	{
		usUsbGeneration = (USHORT)uiUsbGenFromTestSet;
	}
	ProductConfiguration->DefaultUsbGEN = usUsbGeneration;   //set default in product config
	return usUsbGeneration;
}

USHORT CMenuGlueDriver::GetUsbLev(void)
{	
	USHORT usUsbLevel = 0;
	unsigned int uiPartNumFromMenu = 0;
	unsigned int uiUsbLevFromTestSet = 0;

	usUsbLevel = (USHORT)GET_CONFIG_SETTING(iUsbLev);         //get product config setting
	uiUsbLevFromTestSet = GET_SETTING(UsbSurePosLevNum);      //get back door menu setting

	if(usUsbLevel == 0)
	{
		uiPartNumFromMenu = GetPartNumFromMenu();
		usUsbLevel = (USHORT)uiPartNumFromMenu % 256;
	}

	if(uiUsbLevFromTestSet > 0)
	{
		usUsbLevel = (USHORT)uiUsbLevFromTestSet;
	}

	ProductConfiguration->DefaultUsbLEV = usUsbLevel;       //set default in product config
	return usUsbLevel;
}





