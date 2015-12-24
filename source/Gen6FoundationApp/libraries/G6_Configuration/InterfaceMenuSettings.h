//======================================================================================
// InterfaceMenuSettings.h
//======================================================================================
// $RCSfile: InterfaceMenuSettings.h $
// $Revision: 1.18 $
// $Date: 2011/11/01 01:30:59EDT $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Module Interface Description:
//      This file contains the interface definition to a routine that creates
//		interface menu settings as part of the overall reader configuration for
//		a Matrix device.  It also contains the active value declarations of
//		interface settings.
//
//---------------------------------------------------------------------------


#ifndef InterfaceMenuSettingsH
#define InterfaceMenuSettingsH
//---------------------------------------------------------------------------


#include	"language.h"					// system global definitions

bool	bEngSettings;

int CblSelNumber;
#define   CBL_RS232          0
#define   CBL_KBW             1
#define   CBL_USB              2
#define   CBL_RS485           3
#define   CBL_ALL              9999
#define 	TRM_ASYNC_ASCII		0
#define 	AUX_RS232		0
#define	TRM_IBM_PC_XT   		1
#define	TRM_OLIVETTI_M19	 	1
#define	TRM_IBM_PS2			2
#define	TRM_IBM_PC_AT		 	3
#define	TRM_DG_PC			3
#define	TRM_OLIVETTI_M240		3
#define 	TRM_DEC_VT420		4
#define 	TRM_DEC_VT510		5
#define	TRM_IBM_102KEY		 	6
#define	TRM_IBM_122KEY   		7
#define	TRM_DDC_IS386	    		7
#define	TRM_IBM_197KEY		 	8
#define 	TRM_HARRIS		9
#define	TRM_IBM_3180_1		 	24
#define	TRM_SIEMENS			34
#define	TRM_BULL_BDS_7   		35
#define	TRM_FALCO_5220   		47
#define	TRM_NCR_7052	     		48
#define	TRM_APPLE_ADB			49
#define	TRM_RS232			50
#define	TRM_IBM_4683			51
#define	TRM_OCIA			52
#define	TRM_WAND_EMUL		 	61
#define	TRM_CODE_39_EMUL 		61
#define	TRM_NATIVE_EMUL			64
#define	TRM_ADI_1496	     		72
#define	TRM_HHLC			89
#define	TRM_HHLC_RAW			88
#define	TRM_HHLC_C39			87
#define	TRM_DEC500_LK411		104
#define  TRM_USB_iMAC			124          /* 38/3900i USB Dongle (US keyboards) */
#define	TRM_USB_KBD_PC			124
#define  TRM_USB_KBD_MAC		125       /* use for Mac international keyboards (also works for US) */
#define	TRM_USB_FACTORY_TEST		127		 /* this is same as the KBD, but the EZUSB does not shut us off */
#define	TRM_IBM_SUREPOS_HH		128
#define	TRM_IBM_SUREPOS_TT		129
#define	TRM_USB_COMEMU			130
#define	TRM_USB_HIDPOS			131
#define	TRM_USB_JAP_KBD			134
#define	TRM_RF				135
#define	TRM_BT_HCI_IF			902
#define	TRM_BT_SPP_IF			903
#define	TRM_USB_REM				906
#define TRM_BT_HID_IF			907

int		TerminalID;
int          Term232ID;
int		TermKBWID;
int		TermUSBID;
int 		Term485ID;
int		UsbTabSelNum;
int   		RS232TabSelNum;
int           KBDTabSelNum;
int           RS485TabSelNum;
int		CBLTestMenu;

int		InterfaceType;

#if defined(SM4100)
bool IRorTrigger;
int  IRResetTime;
#endif

#if defined(IR_TRIGGER)
#define IR_TRG_OFF		0
#define IR_TRG_ON_STD	1
#define IR_TRG_ON_NEAR	2
#define IR_TRG_ON_FAR	3
int IrTriggerEnable;
#endif

#define	RCFG_BAUD_300			0
#define	RCFG_BAUD_600			1
#define	RCFG_BAUD_1200			2
#define	RCFG_BAUD_2400			3
#define	RCFG_BAUD_4800			4
#define	RCFG_BAUD_9600			5
#define	RCFG_BAUD_19200			6
#define	RCFG_BAUD_38400			7
#define	RCFG_BAUD_57600			8
#define	RCFG_BAUD_115200		9
#define	RCFG_BAUD_230400		10
#define	RCFG_BAUD_460800		11
#define	RCFG_BAUD_921600		12

int		RS232BaudRate;

#define	SER_WORD_N71		0
#define	SER_WORD_N72		1
#define	SER_WORD_N81		2
#define	SER_WORD_E71		3
#define	SER_WORD_E72		4
#define	SER_WORD_E81		5
#define	SER_WORD_O71		6
#define	SER_WORD_O72		7
#define	SER_WORD_O81		8
#define	SER_WORD_S71		9
#define	SER_WORD_S72		10
#define	SER_WORD_S81		11
#define	SER_WORD_M71		12
#define	SER_WORD_M72		13
#define	SER_WORD_M81		14

#define CTSOFF	0			//cts off
#define HNDSHK	1			// old style rts/cts handshaking we set rts when we have data send w/cts true (CTS is hardware controled)
#define FLOW	2			// rts cts do flow control in both directions
#define CTSMAN	3			// old style rts/cts handshaking we set rts when we have data send w/cts true (CTS is GPIO and software controled)
//ewr-02787
#define CTSFST	4			// old style rts/cts handshaking , check CTS first, send data if set, and if not set, we set rts when we have data send w/cts true (CTS is GPIO and software controled)
//cts control scanning RTS pulse
#define SCANCTRL 5
#define CTSPLS	 6	
//end CTS control and RTS pulse

#define PKTOFF 0			//Turns Mmmmmlllll serial packet mode off
#define PKTONE 1        //Mmmmmlllll serial packet mode one. not currently supported
#define PKTTWO 2        //Mmmmmlllll serial packet mode two, the only one we currently support
//

//add a command to ignore the Interface data, by Chuck
	bool	OpenInterfaceEnable;



int		RS232WordFormat;

	int	RS232CtsEnable;
bool	RS232XonEnable;
bool	RS232AckEnable;
int	RS232LowPowerTimeOut;
int	RS232CtsDelay;

// NGOEM-199
bool RS232Unlock;

//ewr-02787
int	RS232PacketMode;
bool	RS232PacketModeNakEnable;
int   RS232AckNakDelay;
//
//ewr-05579
bool RS232SerialDisableEnable;

bool	UsbCtsEnable;
bool	UsbAckEnable;
bool	UsbFifoErrEnable;
bool    UsbHighspEnable; 

#define	USB_SERNUM_OFF		0
#define	USB_SERNUM_ON  		1
//#define	USB_SERNUM_USER   2	         // perhaps later we add this?
int	UsbSerialNumber;



#define	TO_P1_AND_P2		0
#define	TO_P1			1
#define	TO_P2			2

int		SerWdgConfig;



#define	CTY_USA				0
#define	CTY_BELGIUM			1
#define	CTY_FINLAND			2
#define	CTY_FRANCE			3
#define	CTY_GERMANY			4
#define	CTY_ITALY			5
#define	CTY_SWITZ			6
#define	CTY_UK				7
#define	CTY_DENMARK			8
#define	CTY_NORWAY			9
#define	CTY_SPAIN			10
#define	CTY_DUTCH			11
#define	CTY_HEBREW			12
#define	CTY_PORTUGAL			13
#define	CTY_LATIN_AMERICA		14
#define	CTY_CZECH			15
#define	CTY_BRAZIL			16
#define	CTY_GREEK			17
#define	CTY_FRENCH_CANADA		18
#define	CTY_HUNGARY			19
#define	CTY_POLAND			20
#define	CTY_SCS				21
#define	CTY_SLOVAK			22
#define	CTY_SWEDEN			23
#define	CTY_TURKEY_Q			24
#define	CTY_ROMANIA			25
#define	CTY_RUSSIA			26
#define	CTY_TURKEY_F			27
#define	CTY_TAIWAN_BIG5      		28
#define	CTY_SWITZERLAND_FRENCH		29
#define	CTY_USA_INTERNATIONAL          30
#define	CTY_SLOVENIA                   31
#define	CTY_CROATIA                    32
#define	CTY_BOSNIA                     33
#define	CTY_MACEDONIA                  34
#define	CTY_ALBANIA                    35
#define	CTY_SERBIA_LATIN               36
#define	CTY_SERBIA_CYRILLIC            37
#define	CTY_CZECH_QWERTZ               38
#define	CTY_CZECH_QWERTY               39
#define	CTY_CZECH_PROGRAMMERS          40
#define	CTY_ESTONIA                    41
#define	CTY_LATVIA                     42
#define	CTY_LATVIA_QWERTY              43
#define	CTY_LITHUANIA                  44
#define	CTY_LITHUANIAN_IBM             45
#define	CTY_JAPAN_ASCII      	       46
#define	CTY_CHINA_GB2312			   47
#define	CTY_SLOVAK_QWERTZ              48
#define	CTY_SLOVAK_QWERTY              49
#define	CTY_HUNGARIAN_101_KEY          50
#define	CTY_SPANISH_VARIATION          51
#define	CTY_BULGARIA_CYRILLIC          52
#define	CTY_BULGARIA_LATIN	       53
#define	CTY_CANADIAN_FRENCH_LEGACY     54
#define	CTY_CANADIAN_MULTILINGUAL      55
#define	CTY_ITALIAN_142                56
#define	CTY_POLISH_214                 57
#define	CTY_POLISH_PROGRAMMERS         58
#define	CTY_BRAZIL_MS                  59
#define	CTY_GREEK_POLYTONIC            60
#define	CTY_GREEK_220                  61
#define	CTY_GREEK_319                  62
#define	CTY_GREEK_LATIN                63
#define	CTY_GREEK_220_LATIN            64
#define	CTY_GREEK_319_LATIN            65
#define	CTY_GREEK_MS                   66
#define	CTY_RUSSIAN_MS                 67
#define	CTY_RUSSIAN_TYPEWRITER         68
#define	CTY_Reserved1		       69
#define	CTY_Reserved2		       70
#define	CTY_Reserved3		       71
#define	CTY_Reserved4		       72
#define	CTY_IRISH                      73
#define	CTY_MALTA                      74
#define	CTY_ICELAND                    75
#define	CTY_UKRAINIAN                  76
#define	CTY_UZBEK_CYRILLIC             77
#define	CTY_KAZAKH                     78
#define	CTY_KYRGYZ_CYRILLIC            79
#define	CTY_AZERI_LATIN                80
#define	CTY_AZERI_CYRILLIC             81
#define	CTY_BELARUSIAN                 82
#define	CTY_FAEROESE                   83
#define	CTY_GAELIC                     84
#define	CTY_TATAR                      85
#define	CTY_MONGOLIAN_CYRILLIC         86
#define	CTY_USA_DVORAK                 87
#define	CTY_USA_DVORAK_LEFT            88
#define	CTY_USA_DVORAK_RIGHT           89
#define	CTY_MAX_COUNTRY	CTY_USA_DVORAK_RIGHT


int		KBD_CtryCode;



#define	STY_STANDARD					0
#define	STY_CAPS_LOCK					1
#define	STY_SHIFT_LOCK					2
#define	STY_CTRL_ASCII					3
#define	STY_RESERVED					4
#define	STY_EMULATE_EXT			  		5
#define	STY_AUTOCAPS_LOCK				6
#define	STY_AUTOCAPS_LOCK_VIA_NUMLOCK			7
#define	MOD_STANDARD				0
#define	MOD_UPPER				1
#define	MOD_LOWER				2
int		KBD_Mode;


int		KBD_Style;

bool	KBD_AutoDirCon;
bool	KBD_TurboMode;
bool	KBD_NumKeypad;
bool	KBD_TransDeadKey;
bool	KBD_NonPrint;

#define  CNTRL_ASCII_OFF	0
#define  CNTRL_ASCII_DOS	1
#define  CNTRL_ASCII_WIN	2
#define  CNTRL_ASCII_ESC	3

int		KBD_CtrlASCII;

#define	ALT_OFF				0
#define	ALT_WINDOWS			1
#define	ALT_DOS				2
#define	ALT_UNICODE			3
#define	ALT_WINDOWS_SYS_CP 		4
#define	ALT_DOS_SYS_CP			5

int		KBD_TransAltNum;

bool	KBD_TransHtml;
bool	KBD_TransHex;



#define	WND_SPD_10			0
#define	WND_SPD_25			1
#define	WND_SPD_40			2
#define	WND_SPD_80			3
#define	WND_SPD_120			4
#define	WND_SPD_150			5
#define	WND_SPD_200			6
#define	WND_SPD_300			7

int		WandEmRate;

#define	POL_BLACK_HI		0
#define	POL_WHITE_HI		1

int		WandPolarity;

#define	IDLE_LO				0
#define	IDLE_HI				1

int		WandEmIdleState;


#define	WANDEM_BLOCKSIZE_20		0
#define	WANDEM_BLOCKSIZE_40		1
#define	WANDEM_BLOCKSIZE_60		2
#define	WANDEM_BLOCKSIZE_80		3

int		WandEmBlockSize;


#define	WANDEM_BLOCKDELAY_5		0
#define	WANDEM_BLOCKDELAY_50		1
#define	WANDEM_BLOCKDELAY_150	2
#define	WANDEM_BLOCKDELAY_500	3

int		WandEmBlockDelay;


bool	WandEmChecksum;

bool	UsbSurePosIgnCfg;
bool	UsbSurePosIgnDis;
bool	UsbSurePosIgnBep;           //ignore configuration of beeper by the register
bool	UsbSurePosSelectOCR;        //select type of OCR to enable in SurePos, 0==OCR A, 1==OCR B
bool	UsbSurePosUseBCD;
int	UsbSurePosGenNum;           //IBM gereration number in surepos (equiv to our revision's base number) - test setting
int	UsbSurePosLevNum;           //IBM Level number in surepos  (equiv to our revisions's dash number) - test setting

int	UsbReEnumerationTimeout;
bool IBM_4683_IgnoreFirstDisable;
bool RetailScannerEnable;

bool RemIfaceEnable;
int RemRouting;

#define	IBM_4683_PORT_5B		  		0
#define	IBM_4683_PORT_9B_HHBCR1		1
#define	IBM_4683_PORT_9B_HHBCR2		2
#define	IBM_4683_PORT_17		  		3
#define	IBM_4683_PORT_23_ADDR68		4
#define	IBM_4683_PORT_23_ADDR69		5
#define	IBM_4683_PORT_24		  		6
#define	IBM_4683_PORT_25_ADDR64		7
#define	IBM_4683_PORT_25_ADDR65		8
#define	IBM_4683_PORT_26				9
#define	IBM_4683_SurePos				10
#define IBM_4683_PORT_NULL				11

int		IBM_4683_Protocol;
int		IBM_4683_Serial;        // Auxilary 4683 port - Port 6x for flashing over the tailgate

#define	NCR_SHORT		0
#define	NCR_LONG		1
#define	NIXDORF			2
#define	SPECTRA_PHYSICS  	3

#if WALMART_DEBUG
#ifndef RFSCAN
bool AllowWalMartFlash;
bool EnableWalmartDebug;
#endif
#endif

int		OCIA_Protocol;


// HHLC Output Settings
#define	HHLC_36			0
#define	HHLC_100		1

int		HHLCRate;
int		HHLCPolarity;
int		HHLCIdleState;
int		HHLCMinModuleSize;
int		HHLCStretchFactor;
int		HHLCZoomFactor;
int		HHLCVotingTimeout;
int		HHLCLaserEnablePolarity;
#define HHLC_LAS_ENABLE_POLARITY_AUTO	0
#define HHLC_LAS_ENABLE_POLARITY_LOW	1
#define HHLC_LAS_ENABLE_POLARITY_HIGH	2


// Secondary Interface Settings
#define SEC_WAND_SAME       	64
#define SEC_WAND_CODE39 	61
#define	SEC_PSEUDO_232		0
#define	SEC_HHLC		89
#define	SEC_HHLC_RAW		88
#define	SEC_HHLC_C39		87

int		SecondTriggerMode;
int		SecondLowPowerTimeOut;



bool		GoodReadLedInvert;
int		BeeperInvert;
int		RS232InvEnable;



#define 	ECI_TRANSLATE_OFF		0
#define 	ECI_TRANSLATE_ON		1
#define 	ECI_TRANSLATE_ASK_DRIVER	2

int		EciTranslateMode;
int		EciSystemCodepage;	         // see codepage.h for values

enum
{
	ECI_OFF =			0,
	ECI_ON =			0x01,
	ECI_BUFFER_MACROS = 		0x02,
	ECI_DIAGNOSTIC	=		0x04
};
int		EciEnable;

enum
{
	ECI_OUTPUT_BINARY = 0,
	ECI_OUTPUT_SYSCP,
	ECI_OUTPUT_UTF8,
	ECI_OUTPUT_UTF16BE,
	ECI_OUTPUT_UTF16LE
};
int		EciOutputFormat;



int		NumberOfUSBRestarts;


// This setting is actually used by USB to uniquely identify the device.  If the product configuration SERNUM is set to something other
// than default, this setting will be equal to SERNUM.  Otherwise, this setting will be updated with the image engine serial number.
HSTRING		*USBSerNum;



void CreateInterfaceMenuSettings( int ControlMask );


int HModemDownAppReceiveTimeOut;


#endif // InterfaceMenuSettingsH
//----------------------------------------------------------------------------




