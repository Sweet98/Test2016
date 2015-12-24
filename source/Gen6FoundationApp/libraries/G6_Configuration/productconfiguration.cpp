//======================================================================================
// ProductConfiguration.cpp
//======================================================================================
// $RCSfile: productconfiguration.cpp $
// $Revision: 1.53 $
// $Date: 2011/11/18 17:15:09EST $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Class Name:     PRODUCT_CONFIGURATION
//
//  Class Description:
//      PRODUCT_CONFIGURATION contains the data fields and methods needed to manage
//      HHP product configuration data.
//
//---------------------------------------------------------------------------
#define IT4800DR_1_TOKEN 0
#define GEN6_1_BRD_1_TOKEN 1



#include <stdio.h>		/* standard I/O functions					*/
#include <string.h>		/* string functions							*/

#include "stdInclude.h"
#include "ProductConfiguration.h"			// PRODUCT_CONFIGURATION interface definition
#include "SystemMenuSettings.h"
#include "db_hdw.h"
#include "menu.h"
#include "db_chrio.h"
#include "HardwareToken.h"
#include "ReaderConfiguration.h"

#ifndef RFBASE
#include "scandrivermatrixinterface.h"
#endif


#ifdef PRODUCT_CONFIGURATION_APPLICATION_EXTERNAL
PRODUCT_CONFIGURATION_EX		*ProductConfiguration;
#else

PRODUCT_CONFIGURATION	*ProductConfiguration;
#endif


/***** System Wide Globals for application code  *****/

extern volatile unsigned nOpenCntxt;		             // "open context" for the scan driver" opened in main called elswhere
extern UCHAR **ppCurrentScanBuffer;
extern UCHAR *pGlobalImage1;

#define P_SCANBUFFER (*ppCurrentScanBuffer)



#ifdef RFBASE
#else
bool RFBase = false;
#endif

#ifdef RFSCAN
#else
bool RFScanner = false;
#endif


#if		defined(PRODUCT_CONFIGURATION_APPLICATION)	// (mfg or ext)
  #include	"ProjectRevision_PrdCfg.h"
#else
  #define	PROD_CFG_PROJECT_VERSION	": undefined "
  #define	PROD_CFG_SOFTWARE_VERSION	": undefined "
#endif


char ShowProdCfgSwRev[SIZE_OF_PROJECT_VERSION + 1];
char ShowProdCfgWARev[SIZE_OF_SOFTWARE_VERSION + 1];

const char * GetProductName(void);
int GetUSB_PID(void);


// use with caution especially if in a base
#if defined(CFG_DBUG) && !defined(RFBASE)

bool TestIFPopulation()
{
	char cEngine;
	char cInterface;
	cEngine=theHal.WhichEngineAmI();
	cInterface=theHal.WhichInterfaceAmI();
	// Test population v. expected population
	xputint(cEngine,1);
	xputint(cInterface,1);

	if((cInterface==ProductConfiguration->iInterfacePopulation) && (cEngine==ProductConfiguration->iOpticsPopulation))
		xputstring(" PASS ");
	else
		xputstring(" FAIL ");

	return true;
}
#endif

const char * GetProductName(void)
{
#if defined(SM5100)			// 5100
	return "5100";
#elif defined(SM4100)		// 4100
	return "4100";
#elif defined(SM3100)		// 3100
	return "3100";
#elif defined(SM3110)		// 3110
	return "3110";
#elif defined(SM8213)		// 8123, GongJin Customize version, child of 8100
	return "8213";
#else						// 8100
	return "8100";
#endif
}

int GetUSB_PID(void)
{
#if defined(SM5100)			// 5100
	return USB_PID_5100;
#elif defined(SM4100)		// 4100
	return USB_PID_4100;
#elif defined(SM3100)		// 3100
	return USB_PID_3100;
#elif defined(SM3110)		// 3110
	return USB_PID_3110;
#elif defined(SM8213)		// 8123, GongJin Customize version, child of 8100
	return USB_PID_8213;
#else						// 8100
	return USB_PID_8100;
#endif
}

#if !defined(RFBASE)
/////////////////////////////////////////////////////////
//	ReportAimerCenter()
//	Find the aimer center for the LED or LASER type aimer report the
//	center data
//	If required then ship a kim image
//	Receive: nothing
//	Return: true if found
//
////////////////////////////   NOTE 	//////////////////////////////////////////
/*	This function only works in the product cfg app.  In the main app
	this does not.  The debug to date show the problem to be somewhere in the
	FindAimerCenter() function.  Here's some notes from Tim Meier on his debug efforts

	1) All of the commands set with the existing Open Context come back reporting success.
	2) We are indeed taking an image - if you do an IMGSHP after you call this command - you'll see the image you just took
	3) TSTAON is a menu command that will light the aimer, however after this RPTCTR command is called, it no longer works.

	I see the command to the HAL telling the aimer to come on , but I don't know why it isn't working.

	Another thing that I noticed  ....when I power up and do a TSTAON, the aimer comes on like it should,
	when I do TSTAOF the aimer goes off like it should, but after I do the RPTCTR command,
	the TSTAON doesn't work any more.  If I do a TSTLON after that, BOTH the aimer and leds come on,
	and if I do a TSTLOF they both go off.  Then if I pull the trigger, where the commands from the
	scan driver I see says the aimer should be flickering, it's not flickering anymore.
	Somehow, something is getting screwed up by this call, and I'm not sure what it is.
	However, I will say that everything I'm seeing is saying that the scan driver is working appropriately.
	I don't know what to tell you beyond that, because I really don't know what could cause the aimer not to come
	on even if we're specifically telling the HAL to turn it on.
	Your file that you gave me should work just fine, and does according to the scan driver.
	I just don't know what's happening after that.

	- Tim

	No more effort at this point to get it working.  tcr 1/11/05
*/

bool ReportAimerCenter()
{
	bool bRetVal = false;
#ifdef FIXME

	aimer_t aimType  = UNKNOWN;
	imgCtrData_t imgData = {0,0,0,0};
	if (!RFBase)		// not valid for RFBase
	{
		aimType  = FindAimerCenter(imgData);
		if (LASER == aimType)
			sprintf(MsgBuf,": 0F1A00%d;0F1A01%d.",imgData.AimerCenterY,imgData.AimerCenterX);
		else if (LED == aimType)
			sprintf(MsgBuf,": 0F1A00%d.",imgData.AimerCenterY);
		else
			sprintf(MsgBuf," \025: UNKNOWN AIMER\r\n");

		ProductConfiguration->MenuCommandResponse->Append(MsgBuf);
	}


	if (UNKNOWN != aimType)
	{
		// check for ship kimimage suffix D
		int pos = 0xff;
		bool bFound= false;
		bFound =  ProductConfiguration->MenuCommandResponse->FindCharacterForward('d', 0, &pos);
		bFound |=  ProductConfiguration->MenuCommandResponse->FindCharacterForward('D', 0, &pos);
		if (bFound /*&& 0 == pos*/)
			Display_Aimer_Center(imgData);

		bRetVal = true;
	}
#endif

	return bRetVal;
}
#endif


//---------------------------------------------------------------------------
//
// Constructor for PRODUCT_CONFIGURATION
//      Create / initialize product configuration settings.
//
PRODUCT_CONFIGURATION::PRODUCT_CONFIGURATION(void)
		: READER_CONFIGURATION_STORAGE()
{
	m_TfbPop = false;
	// Set up defaults.
	int			DefaultUsbPid;
	const char		*DefaultProductName;
	int			DefaultProductNameModifier = LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP;
	int			DefaultInterfacePopulation = SELECTED_INTERFACE;
	int			DefaultImageXCenter = 432; //for Jade
	int			DefaultImageXCenterModifier = FULL_VISIBILITY_MODIFIABLE_BY_APP;
	int			DefaultImageYCenter = 320; //for Jade
	int			DefaultImageYCenterModifier = FULL_VISIBILITY_MODIFIABLE_BY_APP;
	bool		DefaultOCRAvailable = false;
#ifdef XENON_LITE_1500
	bool		DefaultImagingAvailable = false;
	bool		DefaultImageShippingAvailable = false;
#else
	bool		DefaultImagingAvailable = true;
	bool		DefaultImageShippingAvailable = true;
#endif
	int			DefaultImagingAvailableModifier = FULL_VISIBILITY;
	int			DefaultImageShippingAvailableModifier = FULL_VISIBILITY;
	bool		DefaultPDFSymbologyEnabled = true;
	int			DefaultEngineMount = ENGINE_UP_SIDE_DOWN;
	int			DefaultEngineType = NOENGINE;
	int			DefaultWindowType = WINDOW_RED;
	int			DefaultProccesorFreq = 150;
	bool		DefaultHeatMitigationFixedSettings = false;
	int		DefaultLenFilterType = LEN_HYBRID;

#if !defined(RFBASE) 
 	int			DefaultOpticsPopulation = SELECTED_OPTICS;
	int			DefaultOpticsPopulationModifier = LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP;
 	bool		DefaultStackedLinearSymbologiesEnabled = true;
	bool		DefaultLinearSymbologiesEnabled = true;
	bool		DefaultMatrixSymbologiesEnabled = true;
	bool		DefaultPostalSymbologiesEnabled = true;
	
	DefaultOpticsPopulation = TWO_D;
#ifdef XENON_LITE_1500
	bool		bDefaultHasSwitch = false;
#else
	bool		bDefaultHasSwitch = true;
#endif // endof #ifdef XENON_LITE_1500
#endif

	DefaultUsbGEN = USB_GEN;
	DefaultUsbLEV = USB_LEV;
	if(HardwareToken.IAmVoyager1400())//also for Youjie 2D
	{
		DefaultImageXCenter = 320;//for HI2D 640x480 image
		DefaultImageYCenter = 240;
	}
	if(HardwareToken.IAmChargeOnlyBaseType())
	{
		DefaultInterfacePopulation = RS232_ONLY;
	}
	else if(HardwareToken.IAmOEM())
	{
		if((OEM_TOKEN_FULLSPEED == HardwareToken.InterfaceToken()) ||(OEM_TOKEN_HISPEED == HardwareToken.InterfaceToken() || HardwareToken.IAmOEMPrototype()))
		{
			DefaultInterfacePopulation = PSEUDO232_USB; 
		}
		else
		{
			DefaultInterfacePopulation = RS232_ONLY;
		}
	}
	else
	{
		DefaultInterfacePopulation = PSEUDO_KBW_4683_USB; //All interfaces are supported
	}
	
	DefaultProccesorFreq = 200;
	DefaultHeatMitigationFixedSettings = false;

	EnableSettingChanges( DEFAULT_SETTING_CHANGE_STATE );
	
	DefaultProductName = GetProductName();
	DefaultUsbPid = GetUSB_PID();
	
	AddUnicodeString("0F0100", DefaultProductNameModifier, NULL, NULL, &ProductName,
	                 SIZE_OF_PRODUCT_NAME, DefaultProductName, DefaultProductName );

	// If true, tells the application code to not append a suffix.
	AddBoolean("0F0200", DefaultProductNameModifier, NULL, NULL, &ProductNameNoSuffix, false, false );

	AddNumber("0F0300", FULL_VISIBILITY, NULL, NULL, &FeatureRevision, FEATURE_REVISION__MINIMAL_FEATURE_SET, FEATURE_REVISION__MINIMAL_FEATURE_SET,
	          0, 0x7FFFFFFF);

	AddASCIIString( "0F0400", FULL_VISIBILITY_MODIFIABLE_BY_APP, NULL, NULL, &FlashImagePartNumber, SIZE_OF_MFG_DATE, "", "" );

	AddUnicodeString("0F0500", LIMITED_VISIBILITY_BY_DEVICE, NULL, NULL, &VendorName,
	                 SIZE_OF_VENDOR_NAME, VENDOR_NAME, VENDOR_NAME);

	AddNumber("0F0600", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG, NULL, NULL, &TimesProdCfgFlashed, 0, 0, 0, 0x7FFFFFFF);

	AddNumber("0F0700", LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP, NULL, NULL, &iUsbVid, USB_VID, USB_VID, 0, 0xFFFF);

	AddNumber("0F0701", LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP, NULL, NULL, &iUsbPid, DefaultUsbPid, DefaultUsbPid, 0, 0xFFFF);

	AddNumber("0F0702", LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP, NULL, NULL, &iUsbGen, DefaultUsbGEN, DefaultUsbGEN, 0, 0xFF);
	AddNumber("0F0703", LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP, NULL, NULL, &iUsbLev, DefaultUsbLEV, DefaultUsbLEV, 0, 0xFF);

	AddUnicodeString("0F0800", FULL_VISIBILITY_MODIFIABLE_BY_APP, NULL, NULL, &SerNum,SIZE_OF_SER_NUM, SER_NUM, SER_NUM);
	AddUnicodeString("0F0801", FULL_VISIBILITY_MODIFIABLE_BY_APP, NULL, NULL, &CustomSerialNumber,SIZE_OF_SER_NUM, SER_NUM, SER_NUM);
	AddUnicodeString("0F0802", FULL_VISIBILITY_MODIFIABLE_BY_APP, NULL, NULL, &UniqueUniversalIdentifier, SIZE_OF_SER_NUM, SER_NUM, SER_NUM);

	AddUnicodeString("0F0803", FULL_VISIBILITY, NULL, NULL, &UniqueIdentifier,UNIQ_ID_LEN, SER_NUM, SER_NUM);
	AddNumber("0F0804", FULL_VISIBILITY, NULL, NULL, &iGroupIdentifier, 0, 0, 0, 0xFFFF);
	AddUnicodeString("0F0805", DefaultProductNameModifier, NULL, NULL, &ProductNameIdentifier,
				P_ID_LEN, DefaultProductName, DefaultProductName );
	AddHexString("0F0806", NULL, NULL, NULL, &SymmetricKey, SYM_KEY_LEN, "453F1A", "453F1A");
	AddHexString("0F0807", NULL, NULL, NULL, &PvtKey, PVT_KEY_LEN, "453F1A", "453F1A");
	AddUnicodeString("0F0900", NULL, NULL, NULL, &ActivationKeyMaxUsage, SIZE_OF_MAX_KEY_USAGE, "", "" );
	AddUnicodeString("0F0901", NULL, NULL, NULL, &ActivationKeyTransactionIDs, SIZE_OF_MAX_TRANS_ID, "", "" );
	AddUnicodeString("0F0902", NULL, NULL, NULL, &ActivationKeyDate, SIZE_OF_MAX_KEY_DATE, "", "" );		


	AddString("0F0A00", FULL_VISIBILITY_MODIFIABLE_BY_APP, NULL, NULL, FlashImageMD5CheckSum, SIZE_OF_FLASH_IMAGE_MD5_CHECK_SUM, "", "" );

	AddASCIIString("0F0B00", FULL_VISIBILITY_MODIFIABLE_BY_APP, NULL, NULL, &VerifierOriginalManufacturingDate, SIZE_OF_MFG_DATE, "", "" );
	AddASCIIString("0F0B01", FULL_VISIBILITY_MODIFIABLE_BY_APP, NULL, NULL, &VerifierLastServiceDate, SIZE_OF_MFG_DATE, "", "" );
	AddNumber("0F0C00", LIMITED_VISIBILITY_BY_DEVICE, NULL, NULL, &iInterfacePopulation, DefaultInterfacePopulation, DefaultInterfacePopulation, 0, 0xFFFF);

#if !defined(RFBASE)

	AddNumber("0F0D00", DefaultOpticsPopulationModifier, NULL, NULL, &iOpticsPopulation, DefaultOpticsPopulation, DefaultOpticsPopulation, 0, 0xFFFF);
	if ( HardwareToken.IAmIndustrial() ) bDefaultHasSwitch = false;
	AddBoolean("0F0E00", FULL_VISIBILITY_MODIFIABLE_BY_APP, NULL, NULL, &HasStandSW, bDefaultHasSwitch, bDefaultHasSwitch);
	AddBoolean("0F0F00", FULL_VISIBILITY, NULL, NULL, &StackedLinearSymbologies, DefaultStackedLinearSymbologiesEnabled, DefaultStackedLinearSymbologiesEnabled );
	AddBoolean("0F1000", FULL_VISIBILITY, NULL, NULL, &LinearSymbologies, DefaultLinearSymbologiesEnabled, DefaultLinearSymbologiesEnabled );
	AddBoolean("0F1100", FULL_VISIBILITY, NULL, NULL, &MatrixSymbologies, DefaultMatrixSymbologiesEnabled, DefaultMatrixSymbologiesEnabled );
	AddBoolean("0F1200", FULL_VISIBILITY, NULL, NULL, &PostalSymbologies, DefaultPostalSymbologiesEnabled, DefaultPostalSymbologiesEnabled );
	if ( HardwareToken.IAmIndustrial() )
	{
		AddBoolean("0F1300", LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP, NULL, NULL, &m_TfbPop, false, false);
	}


	//proprietary codes
	AddBoolean("0F1400", FULL_VISIBILITY, NULL, NULL, &CodeZSymbology, false, false);
	AddBoolean("0F1500", FULL_VISIBILITY, NULL, NULL, &VericodeSymbology, false, false);
	AddBoolean("0F1600", FULL_VISIBILITY, NULL, NULL, &BC412Symbology, false, false);
	AddBoolean("0F1700", FULL_VISIBILITY, NULL, NULL, &SecureCodeSymbology, false, false);
	AddNumber("0F1701", LIMITED_VISIBILITY2, NULL, NULL, &SecureCodeVendorKey, 1, 1, 0, 255);

	// Report Aimer Centers  // not ready for app yet - not working
	AddCommand("0F1800", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG|SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND,&ReportAimerCenter,NULL);
#endif // endof #if !defined(RFBASE)

	// use with caution especially if in a base
#if defined(CFG_DBUG) && !defined(RFBASE)
	AddCommand("0F1900", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG, &TestIFPopulation,NULL);
#endif

#ifndef PRODUCT_CONFIGURATION_APPLICATION
	////////////////////////	!!!!!!!!!  NOTE  !!!!!!!!!!!!!!!!!!//////////////////////////////
	//	Initialization of image centering parameters with product configuration application MUST be done
	//	after the class has been instanced.  The method SCN_Open() and subsequent funciton calls wind up using the
	//	GET_SETTING macro which is an error if used prior to ReaderCfg class creation
	///////////////////////////////////////////////////////////////////////////////////////////////
	//	Here's what's going on with the centering parameters
	//	For the applicaiton:
	//	If we open up the ranges to a large image size this prevents the setting in product config
	//	that should have been set up in production from being dropped down to fit inside the range...
	//	If the unit did not have product config run on it it will use the VGA center coordinates and
	//	the range will be adjusted to VGA at runtime.
	//	If engine is MP, will use VGA center coodinates and use expand range to actual image size.
	//	This is the default mode and we should not run into it: Units should be product config'd.
	//
	//	For the product config part refer to the NOTE above and see SetUpCenterTags()
	AddNumber("0F1A00", DefaultImageYCenterModifier, NULL, NULL, &iImageYCenter, DefaultImageYCenter, DefaultImageYCenter, 0, 4000);
	AddNumber("0F1A01", DefaultImageXCenterModifier, NULL, NULL, &iImageXCenter, DefaultImageXCenter, DefaultImageXCenter, 0, 4000);
#endif


	AddNumber("0F0B02", FULL_VISIBILITY_MODIFIABLE_BY_APP, NULL, NULL, &ImageCenterOffset, 182, 182, 0, 364);


	AddBoolean("0F1B00", FULL_VISIBILITY, NULL, NULL, &DpcCirrusSymbology, false, false);
	AddBoolean("0F1C00", FULL_VISIBILITY, NULL, NULL, &GmxMatrixSymbology, false, false);

	AddBoolean("0F1D00", FULL_VISIBILITY, NULL, NULL, &OCRAvailable, DefaultOCRAvailable, DefaultOCRAvailable );
	AddBoolean("0F1A02", DefaultImagingAvailableModifier, NULL, NULL, &ImagingAvailable, DefaultImagingAvailable, DefaultImagingAvailable );
	AddBoolean("0F1A03", DefaultImageShippingAvailableModifier, NULL, NULL, &ImageShippingAvailable, DefaultImageShippingAvailable, DefaultImageShippingAvailable );

	AddBoolean("0F1E00", FULL_VISIBILITY_MODIFIABLE_BY_APP, NULL, NULL, &HeatMitigationFixedSettings, DefaultHeatMitigationFixedSettings, DefaultHeatMitigationFixedSettings);

	AddBoolean("0F1F00", LIMITED_VISIBILITY_BY_DEVICE, NULL, NULL, &IQEditingProAvailable, false, false);
	AddBoolean("0F2000", LIMITED_VISIBILITY_BY_DEVICE, NULL, NULL, &iScriptingAvailable, false, false);


	AddBoolean("0F2100", FULL_VISIBILITY, NULL, NULL, &PDFSymbology, DefaultPDFSymbologyEnabled, DefaultPDFSymbologyEnabled );  // for linear only
	AddNumericList("0F2200", FULL_VISIBILITY, NULL, NULL, &iEngineMountPosition, DefaultEngineMount, DefaultEngineMount, ENGINE_RIGHT_SIDE_UP, ENGINE_ROTATED_RIGHT, ENGINE_UP_SIDE_DOWN, ENGINE_ROTATED_LEFT);
	AddNumericList("0F2201", FULL_VISIBILITY, NULL, NULL, &iEngineType, DefaultEngineType, DefaultEngineType
	               ,NOENGINE
	               ,IT4000
	               ,IT4000RG
	               ,IT4100
	               ,IT4200
	               ,IT4300);

	AddNumber("0F2300", FULL_VISIBILITY, NULL, NULL, &iMaxProccesorFreq, DefaultProccesorFreq, DefaultProccesorFreq, 0, 1000);    //LMC    NORMALLY 150 FOR DEFAULT PROC FREQ
	AddString("0F2400",  SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::FULLY_VISIBLE, NULL, NULL, ShowProdCfgSwRev,SIZE_OF_PROJECT_VERSION,
	          PROD_CFG_PROJECT_VERSION, PROD_CFG_PROJECT_VERSION);
	AddString("0F2401",  SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::FULLY_VISIBLE, NULL, NULL, ShowProdCfgWARev,SIZE_OF_SOFTWARE_VERSION,
	          PROD_CFG_SOFTWARE_VERSION, PROD_CFG_SOFTWARE_VERSION);


	AddBoolean("0F2600", LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP, NULL, NULL, &ProcessorControlledIllumination, true, true );
	int DefaultAimerType = AIMER_RED_LED;
	if ( HardwareToken.IAmIndustrial() ) DefaultAimerType = AIMER_PSOC_LASER;
	AddNumber ("0F2700", LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP, NULL, NULL, &AimerType, DefaultAimerType, DefaultAimerType, AIMER_NONE, AIMER_RED_LED);
	AddNumber ("0F2800", LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP, NULL, NULL, &PrimaryIlluminationType, 0x2, 0x2, 0x0, 0x4);
	AddNumber ("0F2801", LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP, NULL, NULL, &SecondaryIlluminationType, 0x0, 0x0, 0x0, 0x4);
#ifndef PROTO_HW
	AddNumber ("0F2701", LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP, NULL, NULL, &AimerMAXCurrent, 0x135, 0x135, 0x0, 0x200);
	AddNumber ("0F2802", LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP, NULL, NULL, &PrimaryIlluminationMAXCurrent, 0x135, 0x135, 0x0, 0x200);
	AddNumber ("0F2803", LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP, NULL, NULL, &SecondaryIlluminationMAXCurrent, 0x0, 0x0, 0x0, 0x200);
#else
	AddNumber ("0F2701", LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP, NULL, NULL, &AimerMAXCurrent, 0x44, 0x44, 0x0, 0x100);
	AddNumber ("0F2802", LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP, NULL, NULL, &PrimaryIlluminationMAXCurrent, 0x44, 0x44, 0x0, 0x100);
	AddNumber ("0F2803", LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP, NULL, NULL, &SecondaryIlluminationMAXCurrent, 0x0, 0x0, 0x0, 0x100);
#endif //endof PROTO_HW
	/* 	
		Imager type: Micron_MT9V022_BW 0, Micron_MT9V022_Color 1, STMicro_602_BW 2,
		STMicro_602_Color 3, E2V_EV76C454_Jade 4, E2V_EV76C454_Jade_Monocolor 5
		Please refer to productconfiguration.h line 366 - 371 for the definitions
	*/
	AddNumber ("0F1A04", LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP, NULL, NULL, &ImageSensor, 0x4, 0x4, 0x0, 0x5);
	AddNumber ("0F2900", LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP, NULL, NULL, &FocusSetting, 0x2, 0x2, 0x0, 0x6);
	AddNumber ("0F2A00", LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP, NULL, NULL, &PixelClockFrequency, 0x3, 0x3, 0x0, 0x3);

	// Window type
	if(HardwareToken.IAmOEMJade())	// For OEM, clear window
		DefaultWindowType = WINDOW_CLEAR;
	AddNumericList("0F2B00", LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP, NULL, NULL, &iWindowType, DefaultWindowType, DefaultWindowType, WINDOW_CLEAR, WINDOW_RED);		

	// LEN filter
	AddNumericList("0F2901", LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP, NULL, NULL, &iLenFilterType, DefaultLenFilterType, DefaultLenFilterType, LEN_HYBRID, LEN_GLASS_TRIBAND, LEN_GLASS_IR_CUT);
	// Color Fusion product setting
	if(HardwareToken.IAmOEMJade())	// For OEM, enable as default
		AddBoolean("0F2C00", FULL_VISIBILITY, NULL, NULL, &bColorFusion, true, true);
	else
		AddBoolean("0F2C00", FULL_VISIBILITY, NULL, NULL, &bColorFusion, false, false);
		
#ifdef PLUGIN_DEV
	AddBoolean("0F2D00", FULL_VISIBILITY, NULL, NULL, &PluginEnable, true, true );
#endif

    AddNumber ("0F1901", LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP, NULL, NULL, &iFinalTestTracking, 0x0, 0x0, 0x0, 0xFF);

#if defined(IR_TRIGGER)
// TODO: update documents and fixture application
	AddNumber("0F2E01", FULL_VISIBILITY_MODIFIABLE_BY_APP, NULL, NULL, &IrPwmDuty, IR_DEFAULT_DUTY, IR_DEFAULT_DUTY, 0, IR_MAX_DUTY);
	AddNumber("0F2E02", FULL_VISIBILITY_MODIFIABLE_BY_APP, NULL, NULL, &IrPwmDutyHysteresis, IR_MAX_DUTY/10, IR_MAX_DUTY/10, 0, IR_MAX_DUTY);
	AddNumber("0F2E03", FULL_VISIBILITY_MODIFIABLE_BY_APP, NULL, NULL, &IrPwmFreq, IR_DEFAULT_FREQ, IR_DEFAULT_FREQ, 0, IR_MAX_FREQ);
	AddNumber("0F2E04", FULL_VISIBILITY_MODIFIABLE_BY_APP, NULL, NULL, &IrOutDebounceTimeMs, 100, 100, 5, 3000);
#endif

	return;
} // Constructor


//
// Destructor for PRODUCT_CONFIGURATION
//      Nothing to do at this point.
//
PRODUCT_CONFIGURATION::~PRODUCT_CONFIGURATION(void)
{
	return;
} // Destructor




//////////////////////////////////////////
// MenuCurrentConfiguration()
//
// If the product configuration is valid, load it to ProductConfiguration.
// but keep the current serial number
// Receive: nothing
// Return: nothing
#ifdef PRODUCT_CONFIGURATION_APPLICATION
void PRODUCT_CONFIGURATION::MenuCurrentConfiguration()
{
	char MsgBuf[128];
	HSTRING		*ProductConfigurationStorage = new HSTRING(FLASH_MAX_CONFIGURATION_LENGTH * 2, true);	// Temporary storage for configuration data
	if (ReadProductConfigurationFromFlash(ProductConfigurationStorage))
	{
		Menu(ProductConfigurationStorage, true);
		ProductConfigurationStorage->Copy(SerNum); //Get the serial number
		ProductConfigurationStorage->UniCodeToSimpleAsciiFormat();
		ProductConfigurationStorage->Insert(";0F0800", strlen(";0F0800"), 0); 	//Do a Prepend of TAG

		sprintf(MsgBuf,"%d",GET_CONFIG_SETTING(TimesProdCfgFlashed));
		ProductConfigurationStorage->Insert(MsgBuf, strlen(MsgBuf), 0); 	//Do a Prepend of TAG
		ProductConfigurationStorage->Insert("0F0600", strlen("0F0600"), 0); 	//Do a Prepend of TAG
		ProductConfigurationStorage->Append(".");										//We're going to store the serial number

		CopyDefaultsToWorkingAndActive('&');  // reset to the defaults

		Menu(ProductConfigurationStorage, true); //restore the serial number
	}
	else
	{
		StoreProductConfigurationToFlash();
	}

	WorkingValuesUpToDate();
	ActiveValuesUpToDate();

	delete ProductConfigurationStorage;
}
#endif




///////////////////////////////////////////////////////////////////
//	SetCenterValues()
// 	Set up of tag table centering parameters with product configuration application MUST be done
//	after the class has been instanced.  The SCN_IOControl calls need a valid nOpenCntxt variable
//	and the calls to get it use GET_SETTINGS macro calls which need the reader cfg instanced -- whihc needs the
//	product cfg class instanced.  So it must happen afterwards
//
//	We will go find the imager and set the defalts to yield well on the floor. In other words we
//	default to value that gives us the highest yield for our tolernaces.  ON the floor they can run a special
///	command which will go find the center of aimer to a precise location to make our yield 100% */
//	Receive: none
//	Return: nothing
//
#if defined(PRODUCT_CONFIGURATION_APPLICATION)
void PRODUCT_CONFIGURATION::SetUpCenterTags(DWORD dwContext)
{
	int nBuf;
	DWORD nHeight,nWidth;

	SCN_IOControl(dwContext,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&nHeight,4,NULL);
	SCN_IOControl(dwContext,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&nWidth,4,NULL);
	SCN_IOControl(dwContext,HHPSD_IOCTL_GET_ENGINE_TYPE,NULL,0,(PBYTE)&nBuf,0,NULL);

	/*if no engine plugged in or we have linear the engine type will come back linear. We can't autosense linear so that is best scandriver can do
	so these parms will be set to linear values in that case */
	switch(nBuf)
	{
	case HHPSD_ENGINE_IMAGER_ICMEDIA:
	case HHPSD_ENGINE_IMAGER_ALD_ICMEDIA:
		/*Scale down the range, experience shows mechanical tolerances point us toward 230.   LED aimer only.  Don't know yields on laser aimer
		and what best defalts are for that.  We can't tell laser aimer in SW so this architecture not best for it.  When we get to handle laser aimer we
		will have to either send the defalts in on a config file called on the BOM or make sure we start out of the gate using Rod's sw that allows the floor
		to find this locations for both LED and Laser versions. Knowing what we know now, we would not have put anything in here for defalt besides half.
		a different value should have been put in the config file*/
		AddNumber("0F1A00", FULL_VISIBILITY, NULL, NULL, &iImageYCenter, 230, 230, 0, nHeight);

		break;

	default:
		AddNumber("0F1A00", FULL_VISIBILITY, NULL, NULL, &iImageYCenter, nHeight/2,nHeight/2 , 0, nHeight);
		break;
	}
	AddNumber("0F1A01", FULL_VISIBILITY, NULL, NULL, &iImageXCenter, nWidth/2,nWidth/2 , 0, nWidth);
	ChangeNumericRange("0F1A00", 0, nHeight);
	ChangeNumericRange("0F1A01", 0, nWidth);
}

#endif



