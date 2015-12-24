// "MxAPI.h" -- General Application Interface for the "Matrix" Decoder


#ifndef _INC_MXAPI
 #define _INC_MXAPI

#ifndef BCB
#if (TCC)
 #define PCHUGE huge
#else
 #define PCHUGE
#endif
#else
 #define PCHUGE
#endif
typedef unsigned char MxPixel;

#if defined(HHP_DLL) /* this is for Win32 DLLs */
 #ifdef __cplusplus
  #define EXTRA_STUFF extern "C" __declspec(dllexport)
 #else
  #define EXTRA_STUFF __declspec(dllexport)
 #endif
#elif defined(CONFIG_MX_BUILD_LO) /* this is for any ELF object */
 #if defined(__GNUG__)
  #define EXTRA_STUFF extern "C" __attribute__ ((visibility("default")))
 #elif defined(__GNUC__)
  #define EXTRA_STUFF __attribute__ ((visibility("default")))
 #endif
#endif

#ifndef EXTRA_STUFF
  #define EXTRA_STUFF
#endif

#define TRUE 1
#define FALSE 0

#if defined(GEN6_DEVELOPMENT)
 #define ZAP_ENABLED FALSE
 //#define ZAP_DEBUG
 #if defined (ZAP_DEBUG)
  void ZapDebugLog (const char *filename, unsigned line);
 #endif
#else
 #define ZAP_ENABLED FALSE
#endif
#if ZAP_ENABLED||TCC
  #define ZAPPABILITY_INDEX TRUE
#else
  #define ZAPPABILITY_INDEX FALSE
#endif

#if ZAP_ENABLED  ||  TCC  ||  defined(_WIN_THREADING)
 #define COLD 2    // Just searching for symbols
 #define LUKEWARM 5   // Evidence of a particular symbol has been found
 #define WARM 8    // The type of symbol has been confirmed; Data is being collected
 #define HOT 16    // Data capture has been successful at some level, with more to go
 #define COOKED 48 //  A fully valid read has occurred; putting on finishing touches
#endif

/***********************************************************************/
EXTRA_STUFF void* HHP_EstablishDecoder (int width, int height); // Establishes & Initializes a Decoding
// 	Workspace for grayscale images up to "width" x "height", returning a Pointer to it!
//		NB: height = 0 indicates TimerCount Inputs, height = 1 indicates ScanProfile Inputs

/***********************************************************************/
EXTRA_STUFF void HHP_FreeDecoder (void* decoder);	// Frees a Decoding Workspace, if ever needed

/***********************************************************************/
EXTRA_STUFF void HHP_ReportDecoderVersion(char *RevisionString); // Copies Revision String to supplied buffer. Allow for string len of 70 bytes.

/***********************************************************************/

typedef enum {
	DECODE_TYPE_FAST = 0,
	DECODE_TYPE_FULL = 1
} DecodeType_t;
// Corner points of the bar code
typedef struct {
   signed int x;
   signed int y;
} Corner_t;
typedef struct {
	Corner_t Corners[4];
	int D_Corner;
	int PdfRows;
	int PdfCols;
} IQImgInfo_t;


typedef struct {
	unsigned char *message;	// Output Data String
	int length;	 				// Number of Data Characters
	char menuFlag;				// Boolean: is this for Menuing Purposes
	char mustStop;				// Boolean: signals UnStructured Append result
	char lastRecord;			// Boolean: is this the final "Result" from the current decode?
	char HHPcodeID;			// HHP internal Code (Symbology) ID
	char AIMcodeLetter;		// AIM/FACT/ISO "Symbology Identifier"
	char AIMcodeModifier;	// ... and "Modifier" character
	DecodeType_t DecodeType;
	IQImgInfo_t IQImgInfo;
} DecodeResult;

typedef struct {
	void (*ProcessResult)(DecodeResult*);	// void ProcessResult(DecodeResult* result) handles decoded results
										// (when length = -1, simply "Halt Scanning")
#if !ZAP_ENABLED
   int (*MayContinue)(void);  // int MayContinue(void) checks whether decoding should continue
		// 0 = MUST Return (trigger removed), 1 = MAY return (timed out: NO MORE searching), & 2 = Keep Going!
#else
   void (*Chilling)(void);
#endif
	void (*FeedTheDog)(void);	// void FeedTheDog(void) tickles the watchdog
	unsigned int (*MilliSecondTimer)(void); // returns Millisecond Duration since last Reset
	// if returns non-zero, the Exposure and Gain of the image are stored in the first and second pointer locations respectively
	int (*GetExposureGain)(MxPixel PCHUGE *,unsigned int *,unsigned int *);
	int (*ReadFile)(const char *filename, const char **, int *len);
	int (*WriteFile)(const char *filename, char *, int len);
} Callbacks;

#if ZAP_ENABLED
  EXTRA_STUFF void HHP_WriteZappedFlag(void* decoder, int Temperature);
#endif
#if ZAP_ENABLED  ||  defined(_WIN_THREADING)
  EXTRA_STUFF int HHP_GetTemperature(void* decoder);
#endif
EXTRA_STUFF int HHP_GetEnergy(void* decoder);
EXTRA_STUFF int HHP_GetMotion(void* decoder);
#if CONCURRENT || defined(_WIN_THREADING)
EXTRA_STUFF int HHP_GetFastCouponData( void* decoder, unsigned char *buffer );
EXTRA_STUFF int HHP_InstallFastCouponData( void* decoder, unsigned char *buffer, int length, unsigned int imgNum);
EXTRA_STUFF int HHP_FastCouponDecode( void* decoder );
EXTRA_STUFF void HHP_ResetCouponData( void* decoder );
#endif//CONCURRENT||_WIN_THREADING

EXTRA_STUFF void HHP_InstallCallbacks (void* decoder, Callbacks* callbacks);	// Installs a Decoder's Callback Pointers

EXTRA_STUFF void HHP_InstallImagerFeedback(void* decoder, void (*)(void *));
/***********************************************************************/
//	Table of Menuing Parameter Tags, with Default Settings shown inside {}
//
// Once a tag has been defined here, never use a value for anything else.
// This avoids synchronization errors between mxapi.h (if used in an
// external app) and any library that includes the decoder.
//

#define SCAN_ZONE 		0x010	// 0 to 100(%) {20}

#define PRINTWEIGHT  	0x011
#define PRINT_LOCAL		0x012
#define DDDIAG       	0x013	// {FALSE}
//#define VIDEOREVERSE 	0x014 // Implement via "whiteHigh" Boolean in Image structures
#define DECODEMODE   	0x015
#define DECODEWIDTH  	0x016
#define TOPBOTTOM    	0x017	// "Center" position (0 - 8, 4 = Nominal)
#define LFTRGT       	0x018

#define STACKED_CAPABLE	0x019	// basic Reader (i.e. Menuing) Capabilities
#define MATRIX_CAPABLE	0x01a
#define ADDENDA_DELAY   0x01b	// {120 ms}
#define CONCAT_DELAY		0x01c	// {300 ms}
#define UPC_COMP_DELAY	0x01d	// {300 ms}
#define SLIT_HEIGHT     0x01e // ALD Aperature height
#define FIXED_MOUNT     0x01f // Fixed Mount mode (not center weighted)

#define DECODE_WINDOW	0x020	// Check that decoded symbols invade a window centered on the (1) Aimer or (2) FOV
#define WINDOW_TOP      0x021
#define WINDOW_BOT      0x022
#define WINDOW_LFT      0x023	// that Window's widths & heights
#define WINDOW_RGT      0x024
#define DECODE_ZOOM     0x025
#define DECODE_VIGOR    0x026
#define DECODE_MLD      0x027
#define DECODE_REFLECT  0x028
#define DECODE_BOX		0x029	// Decode only within a sub-image centered on the (1) Aimer or (2) FOV
#define DECODE_BOX_HGT	0x02a
#define DECODE_BOX_WID	0x02b	// Box Height & Width as percent of image height & width

#define CBQAMODE        0x030
//#define REFSCALE        0x031
//#define REFWHITE        0x032
//#define REFBLACK        0x033
//#define REFEXPOSURE     0x034
//#define REFSYMSKEW      0x035
#define AUTO_ADAPT		0x036
#define PRIORITIZE		0x037	// a string of Code IDs which are -always- top priority

#define DOA_SIZE		0x38
#define DOACODE			0x39

#define COUPON_REFRESH_MIN  0x3a
#define COUPON_REFRESH_MAX  0x3b
#define DECODE_SECURITY_LEVEL	0x3c //Security Level

// defines Threshold determining region for CYTYC uses only, benign to other decoders...
#define MARGIN_LEFT		0x040	// {0}
#define MARGIN_RIGHT		0x041	// {0}
#define MARGIN_TOP		0x042	// {0}
#define MARGIN_BOTTOM	0x043	// {0}

#define CBR_ENABLED	0x100	// Codabar enabling {FALSE}
#define CBR_SS_XMIT	0x101	//	{FALSE}
#define CBR_CHK_CHR	0x102	// 0 = None, 1 = Checked, 2 = Checked & Transmitted {0}
#define CBR_CONCAT	0x103	// 0 = Disabled, 1 = Enabled, 2 = Required {1}
#define CBR_MIN_LEN	0x104	// {4}
#define CBR_MAX_LEN	0x105	// {60}

#define C39_ENABLED	0x110	// Code 39 enabling {FALSE}
#define C39_SS_XMIT	0x111	// {FALSE}
#define C39_CHK_CHR	0x112	// 0 = None, 1 = Checked, 2 = Checked & Transmitted {0}
#define C39_MIN_LEN	0x113	// {1}
#define C39_MAX_LEN	0x114	// {48}
#define C39_APPEND	0x115	// {FALSE}
#define C39_BASE_32	0x116	// {FALSE} a.k.a PARAF or Italian Pharmacies Interpretation
#define C39_FULL_ASC	0x117	// {TRUE}

#define I25_ENABLED	0x120	// Interleaved 2/5 enabling (FALSE}
#define I25_CHK_DIG	0x121	// 0 = None, 1 = Checked, 2 = Checked & Transmitted {0}
#define I25_MIN_LEN	0x122	// {4}
#define I25_MAX_LEN	0x123	// {80}

#define C93_ENABLED	0x130	// Code 93 enabling {FALSE}
#define C93_MIN_LEN	0x131	// {1}
#define C93_MAX_LEN	0x132	// {80}

#define S25_ENABLED	0x140	// Straight 2/5 (3-bar S/S) enabling {FALSE}
#define S25_MIN_LEN	0x141	// {4}
#define S25_MAX_LEN	0x142	// {48}

#define IATA_ENABLED	0x150	// IATA 2/5 (2-bar S/S) enabling {FALSE}
#define IATA_MIN_LEN	0x151	// {4}
#define IATA_MAX_LEN	0x152	// {48}

#define MX25_ENABLED	0x160	// Matrix 2/5 enabling {FALSE}
#define MX25_MIN_LEN	0x161	// {4}
#define MX25_MAX_LEN	0x162	//	{80}

#define C11_ENABLED	0x170	// Code 11 enabling {FALSE}
#define C11_DBL_CHK	0x171	//	{TRUE}
#define C11_MIN_LEN	0x172	//	{4}
#define C11_MAX_LEN	0x173	// {80}

#define C128_ENABLED	0x180	// Code 128 enabling {FALSE}
#define C128_MIN_LEN	0x181	// {1}
#define C128_MAX_LEN	0x182	// {80}
#define ISBT_ENABLED	0x183 // {FALSE}
#define FNC1_SUBSTITUTE 0x184	// {29} (i.e., <GS>)
#define C128_SHOW_FNCS 0x185	// {FALSE}
#define C128_USE_MLD	  0x186
#define GS1_128_ENABLED 0x187
#define GS1_128_MIN_LEN	0x188
#define GS1_128_MAX_LEN	0x189
//#define GS1_FNC1_SUBSTITUTE 0x18a	// {29} (i.e., <GS>)
//#define GS1_128_SHOW_FNCS 0x18b	// {FALSE}

#define TPEN_ENABLED	0x190	// Telepen enabling {FALSE}
#define TPEN_OLD_STY 0x191 // {FALSE}
#define TPEN_MIN_LEN	0x192	// {1}
#define TPEN_MAX_LEN	0x193	// {60}

#define UPCA_ENABLED	0x1a0 // UPC-A enabling {FALSE}
#define UPCA_CHK_XMIT 0x1a1 // {TRUE}
#define UPCA_NUM_SYS_XMIT 0x1a2 // {TRUE}
#define UPCA_ADDON_2	0x1a3 // {FALSE}
#define UPCA_ADDON_5	0x1a4 // {FALSE}
#define UPCA_ADDON_REQ 0x1a5 // {FALSE}
#define UPCA_ADDON_SEP 0x1a6 // {TRUE}

#define UPCE0_ENABLED 0x1b0 // UPC-E0 enabling {FALSE}
#define UPCE1_ENABLED 0x1b1 // UPC-E1 enabling {FALSE}
#define UPCE0_EXPAND	0x1b2 // {FALSE}
#define UPCE0_CHK_XMIT 0x1b3 // {TRUE}
#define UPCE0_NUM_SYS_XMIT 0x1b4 // {TRUE}
#define UPCE_ADDON_2	0x1b5 // {FALSE}
#define UPCE_ADDON_5	0x1b6 // {FALSE}
#define UPCE_ADDON_REQ 0x1b7 // {FALSE}
#define UPCE_ADDON_SEP 0x1b8 // {TRUE}

#define EAN13_ENABLED 0x1c0 // EAN-13 enabling {FALSE}
#define EAN13_CHK_XMIT 0x1c1 // {TRUE}
#define EAN13_ADDON_2 0x1c2 // {FALSE}
#define EAN13_ADDON_5 0x1c3 // {FALSE}
#define EAN13_ADDON_REQ 0x1c4 // {FALSE}
#define EAN13_ADDON_SEP 0x1c5 // {TRUE}
#define EAN13_ISBN_EXPAND 0x1c6 // {FALSE}

#define EAN8_ENABLED 0x1d0 // EAN-8 enabling {FALSE}
#define EAN8_CHK_XMIT 0x1d1 // {TRUE}
#define EAN8_ADDON_2 0x1d2 // {FALSE}
#define EAN8_ADDON_5 0x1d3 // {FALSE}
#define EAN8_ADDON_REQ 0x1d4 // {FALSE}
#define EAN8_ADDON_SEP 0x1d5 // {TRUE}

#define COUPON_CODES 0x1d8	// {FALSE}
#define UPC_USE_MLD 0x1d9	// {FALSE}

#define EANUCC_EMULATION 0xd9 // 0 = Off, 1 = UCC/EAN-128 ("]C1"), 2 = RSS/Composite ("]e0") {0}

#define MSI_ENABLED	0x1e0	// MSI Code enabling {FALSE}
#define MSI_CHK_XMIT	0x1e1	// {FALSE}
#define MSI_MIN_LEN	0x1e2	// {4}
#define MSI_MAX_LEN	0x1e3	// {48}

#define PLSY_ENABLED	0x1f0	// Plessey Code enabling {FALSE}
#define PLSY_MIN_LEN	0x1f1	// {4}
#define PLSY_MAX_LEN	0x1f2	// {48}
#define PLSY_CHK_XMIT	0x1f3	// {FALSE}

#define RSS14_ENABLED	0x200	// RSS-14 enabling {FALSE}
#define RSSLIM_ENABLED	0x204	// RSS Limited enabling {FALSE}
#define RSSEXP_ENABLED	0x208	// RSS Expanded enabling {FALSE}
#define RSSEXP_MIN_LEN	0x209	// {4}
#define RSSEXP_MAX_LEN	0x20a	// {74}

#define CHINA_ENABLED	0x210	// China Post 2/5 enabling {FALSE}
#define CHINA_MIN_LEN	0x211	// {4}
#define CHINA_MAX_LEN	0x212	// {80}

#define KOREA_ENABLED	0x220	// Korea Post 3/5 enabling {FALSE}
#define KOREA_MIN_LEN	0x221	// {4}
#define KOREA_MAX_LEN	0x222	// {48}

#define POSI_ENABLED	0x230	// PosiCode enabling {FALSE}
#define POSI_LIMITED 0x231	// '0' = disabled, '1' = A, '2' = B {0}
#define POSI_MIN_LEN	0x232	// {4}
#define POSI_MAX_LEN	0x233	// {48}

#define TRI_ENABLED	0x240	// TriOptic Code enabling {FALSE}

#define LBL_ENABLED	0x250	// Labelcode (Follett) enabling

#define BC412_ENABLED	0x260	// BC412 enabling {FALSE}
#define BC412_MIN_LEN	0x261	// {4}
#define BC412_MAX_LEN	0x262	// {48}

#define ZED_ENABLED		0x270	// Code Z enabling {FALSE}
#define ZED_MIN_LEN		0x271
#define ZED_MAX_LEN		0x272

#define CBF_ENABLED	0x280	// Codablock-F enabling {FALSE}
#define CBF_MIN_LEN	0x281	// {1}
#define CBF_MAX_LEN	0x282	// {2048}

#define C16K_ENABLED	0x290	// Code 16K enabling {FALSE}
#define C16K_MIN_LEN	0x291 // {1}
#define C16K_MAX_LEN	0x292	// {160}

#define C49_ENABLED	0x2a0	// Code 49 enabling {FALSE}
#define C49_MIN_LEN	0x2a1 // {1}
#define C49_MAX_LEN	0x2a2	// {81}

#define PDF_ENABLED	0x2b0	// PDF417 enabling {FALSE}
#define PDF_MIN_LEN	0x2b1 // {1}
#define PDF_MAX_LEN	0x2b2	// {2048}
#define PDF_SHW_GLI	0x2b3 // {FALSE}
#define PDF_TICKING	0x2b4 // {FALSE}

#define MPD_ENABLED	0x2c0	// Micro PDF417 enabling {FALSE}
#define MPD_MIN_LEN	0x2c1 // {1}
#define MPD_MAX_LEN	0x2c2	// {250}

#define COM_ENABLED	0x2d0	// Composites enabling {FALSE}
#define COM_ON_UPC	0x2d1 // {FALSE}
#define COM_MIN_LEN	0x2d2 // {1}
#define COM_MAX_LEN	0x2d3	// {250}

#define TLC_ENABLED	0x2e0	// TLC39 enabling {FALSE}

#define AZTEC_ENABLED 0x300	// Aztec Code enabling {FALSE}
#define AZTEC_RUNES	0x301		// Aztec Runes enabled {FALSE}
#define AZTEC_MIN_LEN 0x302	// {1}
#define AZTEC_MAX_LEN 0x303	// {3832}

#define I25_MESA_ENABLED	0x310	// Aztec Mesas enabling {FALSE}
#define C128_MESA_ENABLED	0x311 // {FALSE}
#define C39_MESA_ENABLED	0x312 // {FALSE}
#define C93_MESA_ENABLED	0x313	// {FALSE}
#define UPCA_MESA_ENABLED	0x314 // {FALSE}
#define EAN13_MESA_ENABLED	0x315 // {FALSE}

#define MAXI_ENABLED	0x320	// Maxicode enabling {FALSE}
#define MAXI_MIN_LEN	0x321	// {1}
#define MAXI_MAX_LEN	0x322	// {150}

#define REFTARGET_ENABLED	0x32c	// Decoding of Area Reference Target {FALSE}

#define DATAMX_ENABLED	0x330	// Data Matrix enabling {FALSE}
#define DATAMX_MIN_LEN	0x331	// {1}
#define DATAMX_MAX_LEN	0x332	// {3116}

#define QR_ENABLED	0x340	// QR Code enabling {FALSE}
#define QR_MIN_LEN	0x341	// {1}
#define QR_MAX_LEN	0x342	// {7089}

#define GMX_ENABLED	0x348	// Grid Matrix Code enabling {FALSE}
#define GMX_MIN_LEN	0x349	// {1}
#define GMX_MAX_LEN	0x34a	// {2751}

#define HX_ENABLED	0x34c	// Han Xin Code enabling {FALSE}
#define HX_MIN_LEN	0x34d	// {1}
#define HX_MAX_LEN	0x34e	// {6000}

#define VERI_ENABLED	0x350	// Vericode enabling {FALSE}
#define VERI_MIN_LEN	0x351
#define VERI_MAX_LEN	0x352
#define VERI_SIZE_A	0x353
#define VERI_SIZE_B	0x354
#define VERI_SIZE_C	0x355
#define VERI_SIZE_D	0x356
#define VERI_SIZE_E	0x357

#define NET_ENABLED	0x360	// PostNet enabling {FALSE}
#define NET_CHK_XMIT	0x361
#define PLANET_ENABLED 0x362	// Planet Code enabling {FALSE}
#define PLANET_CHK_XMIT 0x363
#define BPO_ENABLED	0x364	// British Post Office 4-State enabling {FALSE}
#define JAP_ENABLED	0x365	// Japanese 4-State enabling {FALSE}
#define CAN_ENABLED	0x366	// Canadian 4-State enabling {FALSE}
#define KIX_ENABLED	0x367	// KIX (Netherlands) 4-State enabling {FALSE}
#define USPS4CB_ENABLED	0x368	// USPS 4-State Customer Barcode {FALSE}
#define IDTAG_ENABLED	0x369	// UPU 4-State ID-Tag {FALSE}
#define AUS_ENABLED	0x36a	// Australian 4-State enabling {FALSE}
#define AUS_BAR_OUTPUT 0x36b
#define AUS_CUST_INTRPT	0x36c	// 0 = Bar Seq, 1 = N Table, 2 = C Table, 3 = Combo: N on 52 bar symbols, C on 67) {0}
#define BNB_ENABLED 0x36d	// USPS's Fluorescent Bar / No Bar {0}  -- normally NOT supported!
#define BNB_HHR_FORMAT 0x36e //BNB half hour format, either hhr or the half hour
#define BNB_HEX_FORMAT 0x36f

#define OCR_ENABLED	0x370	// OCR enabling (1 = OCR-A, 2 = OCR-B, 3 = Currency, 4 = E13B MICR, 5 = SEMI) {FALSE}
 #define OCR_A 1
 #define OCR_B 2
 #define CURRENCY 3
 #define MICR_E13B 4
 #define SEMI_FONT 5
#define OCR_TEMPLATE	0x371	// 'd' = Digit, 'l' = Letter, 'a' = Alphanumeric, 'e' = Everything, 'g' & 'h' = Group char,
									// 'c' = Check char, 'r' = new Row, 't' = new Template, other = Literal {""}
#define OCR_GROUPG	0x372	// {""}
#define OCR_GROUPH	0x373	// {""}
#define OCR_CHECKSET	0x374	// {""}

#define SUDOKU_ENABLED 0x378	// {FALSE}

#define SECURE_ENABLED 0x380	// Secure Code enabling & options
#define SECURE_MIN_LEN 0x381	// {1}
#define SECURE_MAX_LEN 0x382	// {4000}
#define APPLICATION_KEY 0x383	// {"KEY"}
#define VENDOR_KEY	0x384	// {0}

#define DOT_ENABLED	0x388	// DotCode enabling {FALSE}
#define DOT_MIN_LEN	0x389	// {0}
#define DOT_MAX_LEN	0x38a	// {1024}

#define CIRRUS_ENABLED 0x390	// {0} Available in Special Configurations only!

#define ADD_DEBUG_INFO 0x3A0  // Adds some debug info to the output message

#define UNREC_ENABLED 0x3B0 // Verifier Unrecognized symbology enabling {FALSE}

#define ALD_START_POINT_X 0x3c0 // ALD (fixed mount) Start point x coordinate
#define ALD_START_POINT_Y 0x3c1 // ALD (fixed mount) Start point y coordinate
#define ALD_STOP_POINT_X 0x3c2 // ALD (fixed mount) Stop point x coordinate
#define ALD_STOP_POINT_Y 0x3c3 // ALD (fixed mount) Stop point y coordinate
#define ALD_SPACING 0x3c4 // ALD (fixed mount) gap size between scan lines
#define ALD_ATTEMPTS 0x3c5 // ALD (fixed mount) Number of scan lines to attempt
#define ALD_VOTE 0x3c6 // ALD (fixed mount) if RESULT == MORE_VOTES do we do voting scans?

#define SKIP_LINEARS 0x3d0 // don't do LineScans and LinearPriority.

// Installs a Parameter Value, returns FALSE if not possible
EXTRA_STUFF int HHP_InstallMenuParameter (void *decoder, int menuTag, int value);
// Retrieves a Parameter Value, returns FALSE if not possible
EXTRA_STUFF int HHP_RetrieveMenuParameter (void *decoder, int menuTag, int *value);
// Installs a Parameter String, returns FALSE if not possible
EXTRA_STUFF int HHP_InstallMenuString (void *decoder, int menuTag, char *string);
// Retrieves a Parameter String, returns FALSE if not possible
EXTRA_STUFF int HHP_RetrieveMenuString (void *decoder, int menuTag, char *string, int maxLen);

/***********************************************************************/
EXTRA_STUFF void HHP_ClearDecoder (void* decoder);	// Flushes Decoding Workspace, eliminating Partial Results
// This is done upon each Good Read (?!) and each new Trigger Pull

/***********************************************************************/

#define DPX_LIN_IMG_3800G			-1

typedef struct {	//Image Type #1 -- a 2D Grayscale Image // OBSOLETE (but Still Supported)... See Type #4
	int imageType;	// MUST be 1!
	int height, width; // Size of grayscale Image
	int arrayWidth;	// Width of the Image array, including any dead pixels
	int verticalCenter;	// line number of Aimer's center (about height/2);
	char whiteHigh;	// Boolean: 1/TRUE = White high, 0/FALSE = Black high
	char mirrorImage;	// 0 = Normal, 1 = Left-to-Right Mirror Inverse, 2 = Top-to-Bottom Mirror Inverse
	char imageRotation; // 0 = normal, 1 = 90* Right, 2 = 180* (Upside Down), 3 = 270* Right
	char color;			// Boolean: is this a Color Image?
	int WhiteMax;	// Maximum white value
	int WhiteClip;
	MxPixel PCHUGE *array;	// Start of the Pixel array
} GrayscaleImage;

typedef struct {  // Image Type #2 -- a 1D Grayscale Image (i.e. ScanProfile !!)
	int imageType;	// MUST be 2!
	int length;		// Length of linear Image
	char whiteHigh;	// Boolean: 1/TRUE = White high, 0/FALSE = Black high
	MxPixel PCHUGE *array;	// Start of the Pixel array
} ScanProfile;

typedef struct {	// Image Type #3 -- a 1D Binary Image (i.e. ascending TimerCounts !!)
	int imageType;	// MUST be 3!
	int length;		// Length of the TimerCount list
	int *array;		// Start of the TimerCount list (Start of a Space is FIRST entry!!)
} TimerCounts;

typedef struct {	//Image Type #4 -- a 2D Grayscale Image
	int imageType;	// MUST be 4!
	int height, width; // Size of grayscale Image
	int arrayWidth;	// Width of the Image array, including any dead pixels
	int verticalCenter;	// line number of Aimer's center (about height/2);
	int horizontalCenter;	// column number of Aimer's center (about width/2);
	char whiteHigh;	// Boolean: 1/TRUE = White high, 0/FALSE = Black high
	char mirrorImage;	// 0 = Normal, 1 = Left-to-Right Mirror Inverse, 2 = Top-to-Bottom Mirror Inverse
	char imageRotation; // 0 = normal, 1 = 90* Right, 2 = 180* (Upside Down), 3 = 270* Right
	char color;			// Boolean: is this a Color Image?
	int WhiteMax;	// Maximum white value
	int WhiteClip;
	int imageNumber; //number since trigger pull
	MxPixel PCHUGE *array;	// Start of the Pixel array
} GrayscaleImage2;

typedef struct {  // Image Type #5 -- a 1D Grayscale Image (i.e. ScanProfile !!)
	int imageType;	// MUST be 5!
	int DPX;			// a positive number indicates a Dots Per X setting, and negative numbers can be used for engine type
	int length;		// Length of linear Image
	char whiteHigh;	// Boolean: 1/TRUE = White high, 0/FALSE = Black high
	MxPixel PCHUGE *array;	// Start of the Pixel array
} ScanProfile2;


EXTRA_STUFF void HHP_Decode (void* decoder, void* image);	// Performs Decoding on Provided Image

/***********************************************************************/

#endif //_INC_MXAPI

/*----------------------------------------------------------------------*
 * $RCSfile: mxapi.h $
 * $Revision: 4.70 $
 * $Date: 2010/01/14 18:06:41EST $
 *
 *----------------------------------------------------------------------*/
