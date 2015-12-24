//======================================================================================
// DecoderMenuSettings.h
//======================================================================================
// $RCSfile: DecoderMenuSettings.h $
// $Revision: 1.41 $
// $Date: 2011/09/22 11:16:20EDT $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Module Interface Description:
//      This file contains the interface definition to a routine that creates
//		decoder menu settings as part of the overall reader configuration for
//		a Matrix device.  It also contains the active value declarations of
//		decoder settings.
//
//---------------------------------------------------------------------------


#ifndef DecoderMenuSettingsH
#define DecoderMenuSettingsH
//---------------------------------------------------------------------------


#include	"language.h"					// system global definitions



//	Global Decoder Settings
//	bool	AccelerationTest;
//	int		FirstReadRateSetting;


#define	CHK_NONE			0
#define CHK_REQ				1
#define	CHK_REQ_XMIT			2
#define	CHK_REQ_16			3
#define	CHK_REQ_XMIT_16			4

#define	CHK_REQ_W49M10			3	// for special I2/5 applications
#define	CHK_REQ_XMIT_W49M10		4

#define	MSI_CHK_10				0
#define	MSI_CHK_10_XMIT			1
#define	MSI_CHK_10_10			2
#define	MSI_CHK_10_10_XMIT		3
#define	MSI_CHK_11_10			4
#define	MSI_CHK_11_10_XMIT		5
#define	MSI_CHK_NONE			6

#define	CONCAT_DISABLED			0
#define	CONCAT_ENABLED			1
#define	CONCAT_REQUIRED			2


//	Codabar Settings
	bool	CbrEnable;
	bool	CbrSSXmit;
	int		CbrChkChar;
	int		CbrConCat;
	int		CbrMinLength;
	int		CbrMaxLength;

//	Code 39 Settings
	bool	C39Enable;
	bool	C39SSXmit;
	bool	C39FullAscii;
	bool	C39Append;
	int		C39ChkChar;
	bool	C39Base32;
	int		C39MinLength;
	int		C39MaxLength;
	int		C39DfltCodePage;


//	Interleaved 2 of 5 Settings
	bool	I25Enable;
	int		I25ChkChar;
	bool	I25StrictDecode;
	int		I25MinLength;
	int		I25MaxLength;

	//	NEC 2 of 5 Settings
	bool	N25Enable;
	int		N25ChkChar;
	int		N25MinLength;
	int		N25MaxLength;


//	Code 2 of 5 Settings
	bool	R25Enable;
	int		R25MinLength;
	int		R25MaxLength;


//	IATA Code 2 of 5 Settings
	bool	A25Enable;
	int		A25MinLength;
	int		A25MaxLength;

//	Matrix 2 of 5 Settings
	bool	X25Enable;
	int		X25MinLength;
	int		X25MaxLength;

//	Code 11 Settings
	bool	C11Enable;
// ****	bool	C11ChkChar;
	int		C11MinLength;
	int		C11MaxLength;
	bool	C11DoubleCheck;


//	Code 128 Settings
	bool	C128Enable;
	int		C128MinLength;
	int		C128MaxLength;
	bool	C128ISBTEnabled;
	bool	C128ShowFNCs;
	int		C128CharSubFunc1;
	int		C128DfltCodePage;
	bool  C128FarReading;  // 128 MLD
	bool  G128Enable;  // UCC/EAN-128
	int		G128MinLength;
	int		G128MaxLength;
	bool	C128AppendEnable;

//	Code 93 Settings
	bool	C93Enable;
	int		C93MinLength;
	int		C93MaxLength;
	int		C93DfltCodePage;
	bool	C93AppendEnable;


// MSI
	bool	MSIEnable;
	int		MSIChkChar;
	int		MSIMinLength;
	int		MSIMaxLength;

//	Plessey Settings
	bool	PlsyEnable;
	bool		PlsyChkChar;
	int		PlsyMinLength;
	int		PlsyMaxLength;

//	Telepen Settings
	bool	TELEnable;
	int		TELOldStyle;
	int		TELMinLength;
	int		TELMaxLength;

//	UPC-A Settings
	bool	UPCAEnable;
	bool	UPCAChkXmit;
	bool	UPCANumXmit;
	bool	UPCAAddenda2;
	bool	UPCAAddenda5;
	bool	UPCAAddendaReq;
	bool	UPCAAddendaSep;


//	UPC-E Settings
	bool	UPCE0Enable;
	bool	UPCE1Enable;
	bool	UPCEExpand;
	bool	UPCEChkXmit;
	bool	UPCENumXmit;
	bool	UPCEAddenda2;
	bool	UPCEAddenda5;
	bool	UPCEAddendaReq;
	bool	UPCEAddendaSep;


//	EAN-13 Settings
	bool	EAN13Enable;
	bool	EAN13ChkXmit;
	bool	EAN13Addenda2;
	bool	EAN13Addenda5;
	bool	EAN13AddendaReq;
	bool	EAN13AddendaSep;
	bool	EAN13ISBN;

//	EAN-8 Settings
	bool	EAN8Enable;
	bool	EAN8ChkXmit;
	bool	EAN8Addenda2;
	bool	EAN8Addenda5;
	bool	EAN8AddendaReq;
	bool	EAN8AddendaSep;

//Coupon Code enable
 	int CouponCodes;
	bool CouponSecondaryOutput;

//EAN.UCC emulation
	int EanUccEmulation;
 // UPC MLD
	bool UpcFarReading;

//	China Postal Code Settings
	bool	ChinaEnable;
	int		ChinaMinLength;
	int		ChinaMaxLength;


//	RSS-14 Code Settings
	bool	RSS14Enable;


//	RSS-Limited Code Settings
	bool	RSSLimEnable;


//	RSS-Expanded Code Settings
	bool	RSSExEnable;
	int		RSSExMinLength;
	int		RSSExMaxLength;


// PDF Settings and Decoder Options
	bool	PDFEnable;
	bool	PDFDiagnostics;
	bool	PDFShowGLI;
	bool	PDFMacroEnabled;
	bool	PDFShowMacro;
	bool	PDFTicks;
	int		PDFMinLength;
	int		PDFMaxLength;
	int		PDFDfltCodePage;
	bool	PDFLearnMode;

// MicroPDF Settings
	bool	MPDEnable;
	int		MPDMinLength;
	int		MPDMaxLength;
	int		MPDDfltCodePage;

// Composites Settings
	bool	CompositeEnable;
	bool	CompsOnUpcEan;
	int	CompMinLength;
	int	CompMaxLength;
	int	COMDfltCodePage;

	bool	tlc39Enable;

//CodaBlock F
	bool	cblkfEnable;
	int		cblkfMinLength;
	int		cblkfMaxLength;

//CodaBlock A
	bool	cblkaEnable;
	int		cblkaMinLength;
	int		cblkaMaxLength;

//	Code 16 K Settings
	bool	c16KEnable;
	int		c16KMinLength;
	int		c16KMaxLength;



// BC-412 Settings
	bool	bc412Enable;
	int		bc412MinLength;
	int		bc412MaxLength;

//SecureCode Settings
	#define SIZE_OF_SECURECODE_APP_KEY 11
	bool	SecureCodeEnable;
	int		SecureCodeMinLength;
	int		SecureCodeMaxLength;
//	int 	SecureCodeVendorKey; MOVED TO PRODUCTCONFIG FOR MATRIX, CF2D COMPILE SWITCHED ABOVE
	char	SecureCodeKey[SIZE_OF_SECURECODE_APP_KEY + 1];

// Code 49 Settings
	bool		c49Enable;
	int		c49MinLength;
	int		c49MaxLength;

//posicode
	bool		posiEnable;
	int		posiMinLength;
	int		posiMaxLength;

#define POSILIMITED_OFF	0
#define POSILIMITED_A	1
#define POSILIMITED_B	2

	int		posiLimited;

//trioptic
	bool		triopticEnable;

//korean post
	bool		koreanPostEnable;
	int		koreanPostMinLength;
	int		koreanPostMaxLength;
	bool 	koreanPostCheckTransmit;
	bool 	koreanPostReverse;


	bool	i25MesaEnable;
	bool	c128MesaEnable;
	bool	c39MesaEnable;
	bool	c93MesaEnable;
	bool	upcaMesaEnable;
	bool	ean13MesaEnable;


	bool	aztEnable;
	bool	aztRunes;
	int		aztMinLength;
	int		aztMaxLength;
	int		aztDfltCodePage;
	bool    aztSmallSymbolSize;
	bool    PresenationDecodeAztecSymbolSize;
	bool	aztAppendEnable;
	bool	aztAppendStripInfo;


	bool	maxEnable;
	int		maxMinLength;
	int		maxMaxLength;
	bool    maxSmallSymbolSize;
	bool    PresenationDecodeMaxicodeSymbolSize;

	bool 		gmxEnable;
	int 		gmxMinLength;
	int 		gmxMaxLength;
	bool		gmxAppendEnable;

	bool	refTargetEnable;


	bool	idmEnable;
	int		idmMinLength;
	int		idmMaxLength;
	int		idmDfltCodePage;
	bool    idmNonSquareModules;
	bool	idmShiftedTiles;
	int		idmLowContrast;
	int		PresenationDecodeDataMatrixLowContrast;
	int		idmSmallSymbolSize;
	int		PresenationDecodeDataMatrixSymbolSize;
	int		idmSearchTimeOut;
	bool	idmAppendEnable;


	bool	qrEnable;
	int		qrMinLength;
	int		qrMaxLength;
	int		qrDfltCodePage;
	bool	qrcNonSquareModules;
	int		qrcSmallSymbolSize;
	int		PresenationDecodeQRCodeSymbolSize;
	bool	qrcAppendEnable;

	bool 	hxEnable;
	int 	hxMinLength;
	int 	hxMaxLength;

	bool	netEnable;
	bool	netChkXmit;
	bool	plnEnable;
	bool	plnChkXmit;
	bool	bpoEnable;
	bool	japEnable;
	int		canEnable;
	bool	kixEnable;
	int		postalEnable;
	int     royalMailMinLengthNoFormatCheck;


	bool	ausEnable;
	bool	ausBarOutput;
	int		ausCustIntrpt;
	bool	ausFCCenable;

//uSps specific 4-state codes
	bool 	uspsS18TagEnable;
	bool 	usps4cbEnable;

	bool    BarNoBarEnable;
	bool    BarNoBarHhrFormat;
	bool	BarNoBarHexFormat;

	int	CBQAMode;

	int		DecoderECIHandling;
	bool	CellPhoneMode;
	bool	ImproveBounds;
	bool	LowApsectRatioLinearFinding;
	bool	UseCyclingFinder;
	bool	PresenationDecodeCyclingFinder;
	int		LowSymbolContrast;
	int		PresenationDecodeLowConstrastImprovements;
	bool    ForcePresentationStyleDecode;

	#define LSC_NONE 0
	#define LSC_LINEARS_PDF_NOTI25 1
	#define LSC_I25 2
	#define LSC_ALL 3


//OCR settings
	#define		SIZE_OF_OCR_TEMPLATE		500
	#define		SIZE_OF_OCR_GROUP_G		50
	#define		SIZE_OF_OCR_GROUP_H		50
	#define		SIZE_OF_OCR_CHECKSET		50

	#define OCR_OFF		0
	#define OCR_A          	1
	#define OCR_B         	2
	#define OCR_CURRENCY   	3
	#define E13B_MICR       4
	#define OCR_SEMI        5
	int  	ocrEnable;

	#define OCR_TMP_NONE		0
	#define OCR_TMP_USER		1
	#define OCR_TMP_PASSPORT	2
	#define OCR_TMP_ISBN		4
	#define OCR_TMP_U_I			(OCR_TMP_USER+OCR_TMP_ISBN) //5
	#define OCR_TMP_PRICE_FIELD	8
	#define OCR_TMP_PF_U		(OCR_TMP_PRICE_FIELD+OCR_TMP_USER) //9
	#define OCR_TMP_PF_I		(OCR_TMP_PRICE_FIELD+OCR_TMP_ISBN) //12
	#define OCR_TMP_PF_U_I		(OCR_TMP_PRICE_FIELD+OCR_TMP_USER+OCR_TMP_ISBN) //13
	#define OCR_TMP_MICR		16
	#define OCR_TMP_M_U			(OCR_TMP_MICR+OCR_TMP_USER) //17
	#define OCR_TMP_M_I			(OCR_TMP_MICR+OCR_TMP_ISBN) //20
	#define OCR_TMP_M_U_I		(OCR_TMP_MICR+OCR_TMP_USER+OCR_TMP_ISBN) //21
	#define OCR_TMP_M_PF		(OCR_TMP_MICR+OCR_TMP_PRICE_FIELD) //24
	#define OCR_TMP_M_U_PF		(OCR_TMP_MICR+OCR_TMP_USER+OCR_TMP_PRICE_FIELD) //25
	#define OCR_TMP_M_I_PF		(OCR_TMP_MICR+OCR_TMP_ISBN+OCR_TMP_PRICE_FIELD) //28
	#define OCR_TMP_M_U_I_PF	(OCR_TMP_MICR+OCR_TMP_USER+OCR_TMP_ISBN+OCR_TMP_PRICE_FIELD) //29

	int  	ocrActiveTemplates;
	bool 	ocrBusyBackground;

	char	ocrTemplate[SIZE_OF_OCR_TEMPLATE + 1];
	char	ocrGroupg[SIZE_OF_OCR_GROUP_G + 1];
	char	ocrGrouph[SIZE_OF_OCR_GROUP_H + 1];
	char	ocrCheckSet[SIZE_OF_OCR_CHECKSET + 1];


	bool	verEnable;
	int		verMinLength;
	int		verMaxLength;
	int		verSizeA;
	int		verSizeB;
	int		verSizeC;
	int		verSizeD;
	int		verSizeE;


// Code Z Settings
	bool	codezEnable;
	int		codezMinLength;
	int		codezMaxLength;

// DPC Cirrus Settings
	bool	DpcCirrusEnable;


//Label Code (follett) enable
 	bool LabelCodeEnable;


	int	DecodeInWindow;
	int	WindowTopY;
	int	WindowBotY;
	int	WindowTopX;
	int	WindowBotX;
	int ShowDecodeWindow;
//ewr04791 Codegate: Window Centering settings presentation type trigger.
	int	PresentationDecodeInWindow;
	int	PresentationWindowTopY;
	int	PresentationWindowBotY;
	int	PresentationWindowTopX;
	int	PresentationWindowBotX;

	bool	DecodeZoom;
	int	DecodeVigor;
	bool	DecodeMLD;
	bool	DecodeReflections;
	int   DecodeControl;
	int	DecodeBox, DecodeBoxWidth, DecodeBoxHeight;
	int LinearShortQuietZones;


	int PresentationDecodeVigor;
	int PresentationDecodeControl;
	int PresentationDecodeMinTimeout;
	int PresentationDecodeMaxTimeout;
	int PresentationCouponRefreshMinTimeout;
	int PresentationCouponRefreshMaxTimeout;

	int GeneralDecodeImprovements;
	int Shotgun;
	int		PrintWeight;
	bool 	PrintLocalization;
	int		DDDiag;
	bool	ShowMenuSymbols;

	int			ScanDriverTimeout;

	int	VideoReverse;

	bool	AutoAdaptive;
	#define SIZE_OF_PRIORITY_IDS 11
	char	PriorityCodeIds[SIZE_OF_PRIORITY_IDS+1];


	#define IMAGER_UPRIGHT			0
	#define IMAGER_RIGHT_SIDE_DOWN		1
	#define IMAGER_UPSIDE_DOWN		2
	#define IMAGER_LEFT_SIDE_DOWN		3
	int		iEngineRotation;

#if (DECODER_ID) || defined(RFBASE)
	bool UndoMirroredImage;
#else
	#define	NO_MIRROR	0
	#define	LEFT_TO_RIGHT	1
	#define	TOP_TO_BOTTOM	2
	int		UndoMirroredImage;							//!< undo a mirrored image
#endif

	#define NON_OMNI_ALD	  	2
	#define OMNI_LINEAR		3
	#define QUICK_OMNI		1
	#define FULLSCREEN_OMNI		0
	#define KIOSK_MODE		4
	int		DecodeMode;
	int		DecodeWidth;
	int		DecodeSlitHeight;
	int		DecodeMinTimeout;
	int		DecodeMaxTimeout;

#ifdef MULTIPLE_DECODE_CORES
	int		DecodeMinTimeout2;
	int		DecodeMaxTimeout2;
	bool	AlternateImages;
#endif

	int		DecodeCouponRefreshMinTimeout;
	int		DecodeCouponRefreshMaxTimeout;
	int		DecodeAdditionalTimeout;
	int		DecodeAddendaTimeout;
	int		DecodeConCatTimeout;
	int		DecodeUPC_CompTimeout;

#ifdef MULTIPLE_DECODE_CORES
	int		DecoderCore1;
	int		DecoderCore2;

#define DEC_OPTIMIZE_NONE	0
#define	DEC_OPTIMIZE_DM		1
	int		DecoderCoreOptimize1;
	int		DecoderCoreOptimize2;
#endif

   int     AuxiliaryDecoder;
   int     PresentationAuxiliaryDecoder;

	bool    ShowSnappyTime;
	bool	ShowNoRead;
	
	bool	DecodePassThrough;
	bool	PresenationDecodePassThrough;
	bool	DecodeSubpixelFinder;
	bool	PresenationDecodeSubpixelFinder;
	bool	DecodeEdgeDetector;
	bool	PresenationDecodeEdgeDetector;

#if defined( DEVELOPMENT_BUILD )
   bool DecodeInjectedImage; // Used to force decoder to read an injected image rather than one from scan driver.
   bool DisableDecoder; // used to run system without actually executing the decoder
#endif
   
	#define  SEQUENCE_DISABLED		    0
	#define 	SEQUENCE_ENABLED	    1
	#define  SEQUENCE_REQUIRED		    2
	#define  SIZE_OF_SEQUENCE_BLOCK 	200
	int		SequenceEnabled;
	HSTRING * SequenceBlock;
	bool     PrioritySequence;
	int 		PrioritySequenceTimeValue;
	#define	SIZE_OF_NON_PREFERRED_BLOCK		20
	HSTRING * NonPreferredBlock;
	#define	SIZE_OF_CODE_PREFERRED_BLOCK		20
	HSTRING * CodePreferredBlock;

   int      SudokuEnabled;

void CreateDecoderMenuSettings( int ControlMask, UINT BC412SymbologyCommandModifier, UINT CodeZSymbologyCommandModifier,
		UINT DpcCirrusSymbologyCommandModifier,	UINT SecureCodeSymbologyCommandModifier, UINT Two_DCommandModifier,
		UINT VericodeSymbologyCommandModifier, UINT GmxMatrixSymbologyCommandModifier, UINT LinearSymbologyCommandModifier );

	bool DecoderExtraInfo;

	int PrintResults;
	int DecodeSecurityLevel;
	int SwiftDebug;

   int DecodersToUse;
   bool skipLinear;

   //int DecodeMonocolorInterpolation;
   int DecodeMonocolorForceInterpolation;  // Change DECMCI to DECMFI
#endif // DecoderMenuSettingsH
//----------------------------------------------------------------------------




