//======================================================================================
// ProductConfiguration.h
//======================================================================================
// $RCSfile: productconfiguration.h $
// $Revision: 1.23 $
// $Date: 2011/11/18 17:15:09EST $
//======================================================================================

//======================================================================================




// Process for inserting new configuration entries:
//

//---------------------------------------------------------------------------
//
//  Class definitions for ProductConfiguration
//
//---------------------------------------------------------------------------
#ifndef ProductConfigurationH
#define ProductConfigurationH
//---------------------------------------------------------------------------


#include	"language.h"					// system global definitions
#include	"ReaderConfigurationStorage.h"
#include	"hstring.h"




//////////////////////////////////
//	some debug stuff
//#define _TCRDBGXPUT
#ifdef _TCRDBGXPUT
	extern char buff[127];
#endif

#ifdef _TCRDBGXPUT
	#define	DBGXPUTSTR(a) xputstring(a);
	#define	DBGXPUTSTR1(a,b) sprintf(buff,a,b); xputstring(buff);
	#define	DBGXPUTSTR2(a,b,c) sprintf(buff,a,b,c); xputstring(buff);
	#define	DBGXPUTSTR3(a,b,c,d) sprintf(buff,a,b,c,d); xputstring(buff);
	#define	DBGXPUTSTR4(a,b,c,d,e) sprintf(buff,a,b,c,d,e); xputstring(buff);
#else
	#define	DBGXPUTSTR(a)
	#define	DBGXPUTSTR1(a,b)
	#define	DBGXPUTSTR2(a,b,c)
	#define	DBGXPUTSTR3(a,b,c,d)
	#define	DBGXPUTSTR4(a,b,c,d,e)
#endif
//
////////////////////////////////////////////////////

//////////////////////////////////////////////////////
//
//	Definitions of compiler flags used by product configuration.
//
//		DEFAULT_SETTING_CHANGE_STATE -					Determines whether or not a particular build is allowed to change any product configuration
//														settings.
//		FULL_VISIBILITY_MODIFIABLE_BY_APP -				A setting marked with this flag will behave as follows:
//															 1) Product configuration - modifiable / fully visible.
//															 2) Application - modifiable / visible by tag.
//															 3) Manufacturing - modifiable / visible by tag.
//		FULL_VISIBILITY_MODIFIABLE_BY_MFG_APP -			A setting marked with this flag will behave as follows:
//															 1) Product configuration - modifiable / fully visible.
//															 2) Application - invisible.
//															 3) Manufacturing - modifiable / visible by tag.
//		FULL_VISIBILITY -								A setting marked with this flag will behave as follows:
//															 1) Product configuration - modifiable / fully visible.
//															 2) Application - fixed / visible by tag.
//															 3) Manufacturing - fixed / visible by tag.
//		LIMITED_VISIBILITY									SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG
//															 1) Product configuration - modifiable / fully visible.
//															 2) Application - fixed / invisible.
//															 3) Manufacturing - fixed / invisible.
//		LIMITED_VISIBILITY2									SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG
//															 1) Product configuration - modifiable / visible by sub-tag.
//															 2) Application - fixed /  visible by sub-tag.
//															 3) Manufacturing - fixed /  visible by sub-tag.
//		LIMITED_VISIBILITY_BY_DEVICE						SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG
//															 1) Product configuration - modifiable / visible by sub-tag.
//															 2) Application - fixed /  visible by tag.
//															 3) Manufacturing - fixed /  visible by tag.
//		LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP		SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG
//															 1) Product configuration - modifiable / fully visible.
//															 2) Application - modifiable / visible by tag.
//															 3) Manufacturing - modifiable / visible by tag.
//
//////////////////////////////////////////////////////
#if		defined(PRODUCT_CONFIGURATION_APPLICATION_EXTERNAL)
	#define	DEFAULT_SETTING_CHANGE_STATE						true
	#define	FULL_VISIBILITY_MODIFIABLE_BY_APP					SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG
	#define	FULL_VISIBILITY_MODIFIABLE_BY_MFG_APP				SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG
	#define	FULL_VISIBILITY										SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG
	#define	LIMITED_VISIBILITY									SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG
	#define	LIMITED_VISIBILITY2									SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG
	#define	LIMITED_VISIBILITY_BY_DEVICE						SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG
	#define	LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP		SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG
#elif		defined(PRODUCT_CONFIGURATION_APPLICATION)
	#define	DEFAULT_SETTING_CHANGE_STATE						true
	#define	FULL_VISIBILITY_MODIFIABLE_BY_APP					SUB_TAG_TABLE::FULLY_VISIBLE
	#define	FULL_VISIBILITY_MODIFIABLE_BY_MFG_APP				SUB_TAG_TABLE::FULLY_VISIBLE
	#define	FULL_VISIBILITY										SUB_TAG_TABLE::FULLY_VISIBLE
	#define	LIMITED_VISIBILITY									SUB_TAG_TABLE::FULLY_VISIBLE
	#define	LIMITED_VISIBILITY2									SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG
	#define	LIMITED_VISIBILITY_BY_DEVICE						SUB_TAG_TABLE::FULLY_VISIBLE
	#define	LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP		SUB_TAG_TABLE::FULLY_VISIBLE
#elif defined( MFG_TEST )
	#define	DEFAULT_SETTING_CHANGE_STATE						false
	#define	FULL_VISIBILITY_MODIFIABLE_BY_APP					SUB_TAG_TABLE::VISIBLE_BY_TAG
	#define	FULL_VISIBILITY_MODIFIABLE_BY_MFG_APP				SUB_TAG_TABLE::VISIBLE_BY_TAG
	#define	FULL_VISIBILITY										SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_TAG
	#define	LIMITED_VISIBILITY									SUB_TAG_TABLE::FIXED
	#define	LIMITED_VISIBILITY2									SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG
	#define	LIMITED_VISIBILITY_BY_DEVICE						SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_TAG
	#define	LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP		SUB_TAG_TABLE::VISIBLE_BY_TAG
#else
	#define	DEFAULT_SETTING_CHANGE_STATE						false
	#define	FULL_VISIBILITY_MODIFIABLE_BY_APP					SUB_TAG_TABLE::VISIBLE_BY_TAG
	#define	FULL_VISIBILITY_MODIFIABLE_BY_MFG_APP				SUB_TAG_TABLE::FIXED
	#define	FULL_VISIBILITY										SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_TAG
	#define	LIMITED_VISIBILITY									SUB_TAG_TABLE::FIXED
	#define	LIMITED_VISIBILITY2									SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG
	#define	LIMITED_VISIBILITY_BY_DEVICE						SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_TAG
	#ifdef CF2D
		#define	LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP	SUB_TAG_TABLE::FIXED | SUB_TAG_TABLE::VISIBLE_BY_TAG
	#else
		#define	LIMITED_VISIBILITY_BY_DEVICE_MODIFIABLE_BY_APP	SUB_TAG_TABLE::VISIBLE_BY_TAG
	#endif
#endif
//
///////////////////////////////////////////////////

#define MAKECHAR(a) 0x30+a


typedef struct imgCtrData_tag{
	int ImageHeight;
	int ImageWidth;
	int AimerCenterY;
	int AimerCenterX;
}imgCtrData_t;

typedef enum aimerType_tag{
	UNKNOWN,
	LED,
	LASER
}aimer_t;

///////////////////////////////////////////////////
// prototypes
bool FindCenters();
aimer_t FindAimerCenter(imgCtrData_t & imgData);
bool Find_Laser_Aimer(imgCtrData_t & imgData);
bool Find_Led_Aimer(imgCtrData_t & imgData);
void Display_Aimer_Center(imgCtrData_t & imgData);




//////////////////////////////////////////////////////
//
//
class PRODUCT_CONFIGURATION : public READER_CONFIGURATION_STORAGE
{
public:
	bool	MatrixSymbologies;
	bool	PostalSymbologies;
	bool	StackedLinearSymbologies;
	bool	LinearSymbologies;
	bool	CodeZSymbology;
	bool	VericodeSymbology;
	bool	BC412Symbology;
	bool	SecureCodeSymbology;
	bool	OCRAvailable;
	bool	ImagingAvailable;
	bool 	ImageShippingAvailable;
	bool	IQEditingProAvailable;
	bool 	iScriptingAvailable;
	bool	PDFSymbology;          //linear ONLY
	bool 	DpcCirrusSymbology;
	bool 	GmxMatrixSymbology;
	int 	SecureCodeVendorKey;
	bool	HeatMitigationFixedSettings;
	bool	HasStandSW;	// set by manufacturing to indicate presense of reed switch used in magnetic stand

#ifdef VUQUEST3310
	bool	ExtTriggerAndIllumAvailable;
#endif

#ifdef PLUGIN_DEV
	bool PluginEnable;
#endif

#if defined(IR_TRIGGER)
	int IrPwmDuty;
	int IrPwmDutyHysteresis;
	int IrPwmFreq;
	int	IrOutDebounceTimeMs;
#define IR_MAX_DUTY		100
#define IR_DEFAULT_DUTY	38
#define IR_MAX_FREQ		100000
#define IR_DEFAULT_FREQ	20000
#endif

	// Color Fusion product config
	bool bColorFusion;

#define SIZE_OF_PRODUCT_NAME	64
	HSTRING * ProductName ;//= new HSTRING(SIZE_OF_PRODUCT_NAME+1, false);

// If true, tells the application code to not append a suffix.
	bool	ProductNameNoSuffix;

// Feature revision is a setting that defines a particular feature revision for a unit.  Default value of 0 means
// minimal features.
	typedef enum  {
		FEATURE_REVISION__MINIMAL_FEATURE_SET = 0,
		FEATURE_REVISION__3800G_FEATURE_SET_1		// Aggressive digitizer, multiframe exposure, standard symbology set.
	} FEATURE_REVISION;

	int		FeatureRevision;

// Flash image part number is a setting that defines the part number assigned to a manufacturing flash image as it is created.
#define SIZE_OF_FLASH_IMAGE_PART_NUMBER		64
	HSTRING	*FlashImagePartNumber;




#define SIZE_OF_VENDOR_NAME	64
	HSTRING * VendorName ;//= new HSTRING(SIZE_OF_VENDOR_NAME+1, false);
#ifdef YJ2D_BASED_ON_HI2D
#define VENDOR_NAME ""
#else
#define VENDOR_NAME "SuperMax Imaging"
#endif

#ifdef YJ2D_BASED_ON_HI2D
#define USB_VID			0x23D0
#else
#define USB_VID			0x0202	/* HIM - Thank you Metro (legacy)*/
#endif


	int	iUsbVid;

#define	USB_PID_1900	0x0900			// 1900 Corded Scanner PID
#define USB_PID_1400 	0x0B80 			//Voyager 2D 1400

#define USB_PID_8100	0x0000
#define USB_PID_5100	0x0020
#define USB_PID_4100	0x0040
#define USB_PID_3100	0x0060
#define USB_PID_3110	0x0080
#define USB_PID_8213	0x00A0			// GongJin Customize version

	int	iUsbPid;



#define USB_GEN			0x00
	int	iUsbGen;                      //Part of EC # in IBM spec, Usb surepos firmware generation, upper byte
	int	DefaultUsbGEN;
#define USB_LEV			0x00
	int	iUsbLev;                      //Part of EC # in IBM spec, Usb surepos firmware level, lower byte
	int	DefaultUsbLEV;

#define SIZE_OF_SER_NUM	64
	HSTRING * SerNum;// = new HSTRING(SIZE_OF_SER_NUM+1, false);
	HSTRING	* CustomSerialNumber;
	HSTRING	* UniqueUniversalIdentifier;
#define SER_NUM "XXXXXX"
//#define SER_NUM "----------" //Define this as default for QUAL entry
#define	SIZE_OF_MFG_DATE	20
	HSTRING * VerifierOriginalManufacturingDate;
	HSTRING	* VerifierLastServiceDate;

#define SIZE_OF_FLASH_IMAGE_MD5_CHECK_SUM	32
	char	FlashImageMD5CheckSum[ SIZE_OF_FLASH_IMAGE_MD5_CHECK_SUM + 1 ];

int TimesProdCfgFlashed;
/////////////////////////
int	iInterfacePopulation;
#define PSEUDO_KBW_4683_USB	  	0
#define RS232_ONLY					1
#define HHLC_LP						2

//IT4080 without the USB
// Adding this macro to help distinguish the 40104/4080 IF_POP options returned in WhichInterfaceAmI()
// Case: Product cfg revs 1.13 or later with New App rev 1.153 or later
//		Prod Cfg defaults IF_POP2 and now the 4010/4080 without the usb chio will return the correct set of term id's
//		for the board.  Same will be true for the 4010/4080 with USB.
//	Case: Product cfg rev 1.13 or later with APP older then -051
//		Product config defaults IF_POP2 which means that the IT4010/4080 with and without the usb chip will return
//		the wrong set of allowed terminal ID's.  This is the pre bug fix condition and this fix can't correct
//		legacy apps beacuse to have created a new IF_POP constant to describe the unique IT4010 with and without models
//		would have 'broken' and the legacy apps
#define IT4080_RS232_HHLC			HHLC_LP

#define FULL_ASYNC_SERIAL_ONLY 	3		// CF2D - DTR and DSR used
#define RS232_USB_HHLC				4		// IT4080 that handles RS232,HHLC and USB
#define PSEUDO232_USB	  			5     // 4800DR supports USB and 232 only

#define	SELECTED_INTERFACE		PSEUDO_KBW_4683_USB

bool m_TfbPop;
/////////////////////
int	iOpticsPopulation;
#define ONE_D_NO_LASER		0
#define ONE_D_LASER			1
#define TWO_D				2
#define ALR_LASER			3
#define TWO_D_LASER			4

#ifndef VERIFIER
#define SELECTED_OPTICS	TWO_D
#else
#define SELECTED_OPTICS	ONE_D_NO_LASER
#endif


///////////////////////////
int iEngineMountPosition;
#define ENGINE_RIGHT_SIDE_UP	0
#define ENGINE_ROTATED_RIGHT	1
#define ENGINE_UP_SIDE_DOWN		2
#define ENGINE_ROTATED_LEFT		3

int iMaxProccesorFreq;

///////////////////////////
int iEngineType;
#define NOENGINE	0	// ONE D, DEFAULT
#define IT4000 	1
#define IT4000RG	2
#define IT4100 	3		//LED, Bright aimer
#define IT4200 	4
#define IT4300 	5		//LASER


////////////////////////
int	iWindowType;
#define WINDOW_CLEAR	0
#define WINDOW_RED		1

int iLenFilterType;
#define LEN_HYBRID			0
#define LEN_GLASS_TRIBAND	1
#define LEN_GLASS_IR_CUT	2

int	iImageYCenter;		// Use for factory tuning of Y-axis center, aimer to imager
int	iImageXCenter;		// Use for factory tuning of X-axis center, (laser) aimer to imager

int	ImageCenterOffset;	// Use for factory tuning of Verifier center.

//--- iMX25 Illumination Control Engine Replacement --- 
bool ProcessorControlledIllumination;

int	AimerType;
#define 	AIMER_NONE 					0	//No AIMER
#define 	AIMER_GREEN_LED 			1	//Two Green LEDs (5000) 
#define 	AIMER_BRIGHT_GREEN_LED		2	//Two Bright Green LEDs (5100)
#define 	AIMER_PSOC_LASER 			3	//PSOC Controlled Laser
#define 	AIMER_LASER 				4	//iMX25 Controlled Laser
#define 	AIMER_RED_LED 				5	//Two Red LEDs (Gen6)

int	PrimaryIlluminationType;
int	SecondaryIlluminationType;
#define 	ILLUMINATION_NONE 			0	//No Illumination
#define 	ILLUMINATION_RED_LED 		1	//4-6 RED LEDs near the Imager (5000 design)
#define 	ILLUMINATION_RED_LIGHT_BAR 	2	//6 RED LEDs on the Light Bar
#define 	ILLUMINATION_WHITE 			3	//1 White LED on the Light Bar
#define 	ILLUMINATION_GREEN 			4	//4-6 Green LEDs near the Imager, used in 5000 designs as Secondary Illumination

int	AimerMAXCurrent;
int	PrimaryIlluminationMAXCurrent;
int	SecondaryIlluminationMAXCurrent; 

int	ImageSensor;
#define Micron_MT9V022_BW 				0
#define Micron_MT9V022_Color 			1
#define STMicro_602_BW	 				2
#define STMicro_602_Color 				3
#define E2V_EV76C454_Jade 				4
#define E2V_EV76C454_Jade_Monocolor	5

int	FocusSetting;
#define FS_HD 		0
#define FS_SF_ER 	1
#define FS_SR 		2
#define FS_VHD	 	3
#define FS_DR 		4
#define FS_WA 		5
#define FS_XR 		6

int	PixelClockFrequency;
#define PIXCLK_MHz_24 	0
#define PIXCLK_MHz_26 	1
#define PIXCLK_MHz_47 	2
#define PIXCLK_MHz_48 	3
//--- endof iMX25 Illumination Control Engine Replacement --- 

int iFinalTestTracking; //Used in the Reconfigure station by manufacturing to confirm that the unit has gone through Final Test.

#if defined(RFBASE) || defined(RFSCAN) 
#define BT_ADD_LEN	6
	HSTRING * pBtHwAddress;
	int BtAnaFTrim;  // crystal frequency trim
#endif



//RTLM//////////////////
#define UNIQ_ID_LEN	64
HSTRING * UniqueIdentifier;

int iGroupIdentifier;

#define P_ID_LEN 64	
HSTRING * ProductNameIdentifier;

#define SYM_KEY_LEN	6
HSTRING * SymmetricKey;

#define PVT_KEY_LEN	6
HSTRING * PvtKey;

#define SIZE_OF_MAX_KEY_USAGE	400
HSTRING * ActivationKeyMaxUsage;

#define SIZE_OF_MAX_TRANS_ID	400
HSTRING * ActivationKeyTransactionIDs;

#define SIZE_OF_MAX_KEY_DATE	400
HSTRING * ActivationKeyDate;
	
//end RTLM//////////////////


public:		// User declarations
	void SetUpCenterTags(DWORD dwContext);




#ifdef PRODUCT_CONFIGURATION_APPLICATION
	// to keep the size of std build - these functions are only needed for config app build
	virtual void MenuDefaults();
	virtual void MenuCurrentConfiguration();
#endif

	PRODUCT_CONFIGURATION(void);
	~PRODUCT_CONFIGURATION(void);


};  // Class PRODUCT_CONFIGURATION


//////////////////////////////////////////
//
bool BuyFish(void);
typedef enum lock_tag{LOCK,UNLOCK}lock_t;
const int NUM_KEYS = 31;
const int LEN_KEYSTR = 6;

////////////////////////////////////////////////////////////////////////////
//	The ProductConfiguration class for external use
//	This is the class used for the app build for the aplication we give out to customers
//	who have been given permission to chagne the configuration
//
//
class PRODUCT_CONFIGURATION_EX: public PRODUCT_CONFIGURATION
{

public:
	PRODUCT_CONFIGURATION_EX();
	virtual ~PRODUCT_CONFIGURATION_EX();


private:
	bool 	BeQuiet;				// indicating whether we announce locking or not, really a debugging aid.

	// --------------------
	//	KeyTable class
	//	contains a hash number and a function ptr
	// User enters a string containing 6 characters which are hashed to a number.
	//	The number is used to look up an Unlock function
	//	---------------------
	class KeyTable
	{
		public:
		unsigned int KeyNum;
		bool (PRODUCT_CONFIGURATION_EX::*fnlock)(lock_t);
	}KeyTables[NUM_KEYS];


public:
	virtual void MenuDefaults();
	virtual void MenuCurrentConfiguration();
	void Report(const char* str);
	unsigned int HashKey(unsigned char* pCh);

	//void ParseKeys();
	bool FindKey(unsigned int keynum);
	bool FindDelimiter(unsigned char ch, unsigned char** ppWalkr, unsigned char* pEnd);
	bool Unlock(const char *);
	bool Lock(const char *);
	bool LUL_ProductName(lock_t unlock);
	bool LUL_VendorName(lock_t unlock);
	bool LUL_USBVendorID(lock_t unlock);
	bool LUL_USBProductID(lock_t unlock);
	bool LUL_USBPower(lock_t unlock);
	bool LUL_USBPowerSource(lock_t unlock);
	bool LUL_SerialNum(lock_t unlock);
	bool LUL_InterfacePopulation(lock_t unlock);
	bool LUL_OpticsPopulation(lock_t unlock);
	bool LUL_IndustrialPopulation(lock_t unlock);
	bool LUL_StackedLinears(lock_t unlock);
	bool LUL_Linears(lock_t unlock);
	bool LUL_Matrix(lock_t unlock);
	bool LUL_PostalCodes(lock_t);
	bool LUL_CodeZ(lock_t unlock);
	bool LUL_Vericode(lock_t unlock);
	bool LUL_BC412(lock_t unlock);
	bool LUL_SecureCode(lock_t unlock);
	bool LUL_SecureCodeVendorKey(lock_t unlock);
	bool LUL_IMGCTR(lock_t unlock);
	bool LUL_IMGCTX(lock_t unlock);
	bool LUL_DPCCirrus(lock_t unlock);
	bool LUL_OCR(lock_t unlock);
	bool LUL_ImagingAvailable(lock_t unlock);
	bool LUL_ImageShippingAvailable(lock_t unlock);
	bool LUL_IQEditingProAvailable(lock_t unlock);
	bool LUL_IScriptingAvailable(lock_t unlock);
	bool LUL_PDFSymbology(lock_t unlock);
	bool LUL_EngineMountPosition(lock_t lock);
	bool LUL_EngineType(lock_t lock);
	bool SetLocks(lock_t lock);

#ifdef _TCRDEBUG
	struct ExternalKey_tag* ExKeys;
#endif

};  // Class PRODUCT_CONFIGURATION_EX

#ifdef PRODUCT_CONFIGURATION_APPLICATION_EXTERNAL
	extern PRODUCT_CONFIGURATION_EX		*ProductConfiguration;
#else
	extern PRODUCT_CONFIGURATION	*ProductConfiguration;
#endif

#endif // ProductConfigurationH
//----------------------------------------------------------------------------




