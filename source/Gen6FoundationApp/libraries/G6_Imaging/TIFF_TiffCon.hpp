//======================================================================================
// TIFF_TiffCon.hpp
//======================================================================================
// $RCSfile: TIFF_TiffCon.hpp $
// $Revision: 1.1 $
// $Date: 2009/04/03 15:08:47EDT $
//======================================================================================
//
//======================================================================================
//
//	This file is part of a TIFF compression / decompression package that Tim Meier picked
//	up from .  This package was ported from the CxImage library to be a standalone
//	compression / decompression library for TIFF images.
//
//======================================================================================



//ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//บ
//บ	Constant values used in the TIFF++ library.  Nearly all of these are
//บ taken from tifflib.h
//บ
//บ	Jon Saxton
//บ	November 1997
//บ
//ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#ifndef TIFFCON_HPP
#define TIFFCON_HPP

struct JPEGConversion
{
	enum
	{
		NO_CHANGE,
		NEW,
		OLD,
		FAKE_OLD,
		AS_IS
	};
};

// Everything from here to ^^^ was copied from tifflib

// NB: In the comments below,
//  - items marked with a + are obsoleted by revision 5.0,
//  - items marked with a ! are introduced in revision 6.0.
//  - items marked with a % are introduced post revision 6.0.
//  - items marked with a $ are obsoleted by revision 6.0.

#ifndef _TIFF_DATA_TYPEDEFS_
  #define _TIFF_DATA_TYPEDEFS_

	// Intrinsic data types required by the file format:
	//
	//	8-bit quantities	int8/uint8
	// 16-bit quantities	int16/uint16
	// 32-bit quantities	int32/uint32
	// strings				unsigned char*

#ifdef __STDC__
	typedef signed char int8;	/* NB: non-ANSI compilers may not grok */
  #else
	typedef char int8;
  #endif

	typedef unsigned char uint8;
	typedef unsigned char uchar;
	typedef short int16;
	typedef unsigned short uint16;	/* sizeof (uint16) must == 2 */

  #if defined(__alpha) || (defined(_MIPS_SZLONG) && _MIPS_SZLONG == 64)
	typedef int int32;
	typedef unsigned int uint32;	/* sizeof (uint32) must == 4 */
  #else
	typedef long int32;
	typedef unsigned long uint32;	/* sizeof (uint32) must == 4 */
  #endif

#endif /* _TIFF_DATA_TYPEDEFS_ */

// Tag data type information.
// Note: RATIONALs are the ratio of two 32-bit integer values.

enum TIFFDataType
{
	TIFF_NOTYPE 	= 0,	// placeholder
	TIFF_BYTE		= 1,	// 8-bit unsigned integer
	TIFF_ASCII		= 2,	// 8-bit bytes w/ last byte null
	TIFF_SHORT		= 3,	// 16-bit unsigned integer
	TIFF_LONG		= 4,	// 32-bit unsigned integer
	TIFF_RATIONAL	= 5,	// 64-bit unsigned fraction
	TIFF_SBYTE		= 6,	// !8-bit signed integer
	TIFF_UNDEFINED	= 7,	// !8-bit untyped data
	TIFF_SSHORT 	= 8,	// !16-bit signed integer
	TIFF_SLONG		= 9,	// !32-bit signed integer
	TIFF_SRATIONAL	= 10,	// !64-bit signed fraction
	TIFF_FLOAT		= 11,	// !32-bit IEEE floating point
	TIFF_DOUBLE 	= 12	// !64-bit IEEE floating point
};

// The following enums are enclosed inside structs to separate the namespaces.
// Using the namespace keyword would achieve the same thing if the VAC++
// compiler supported it.  Either way each use of an enumerated value has to
// be qualified.  Ordinarily that is tedious but in this library I consider it
// to be a GOOD thing because it adds clarity to the usage of each enum value.

// TIFF Tag Definitions.  (In the original these were #defines but I changed
// them so the compiler would see the names.)

struct TIFF_Filetype
{
	enum
	{
		REDUCEDIMAGE		=	1,		// reduced resolution version
		PAGE				=	2,		// one page of many
		MASK				=	4 		// transparency mask
	};
};

struct TIFF_OldFiletype
{
	enum
	{
		IMAGE				=	1,		// full resolution image data
		REDUCEDIMAGE		=	2,		// reduced size image data
		PAGE				=	3		// one page of many
	};
};

struct TIFF_Compression
{
	enum
	{
		NONE				=	1,		// dump mode
		CCITTRLE			=	2,		// CCITT modified Huffman RLE
		CCITTFAX3			=	3,		// CCITT Group 3 fax encoding
		CCITTFAX4			=	4,		// CCITT Group 4 fax encoding
		LZW 				=	5,		// Lempel-Ziv  & Welch
		OJPEG				=	6,		// !6.0 JPEG
		JPEG				=	7,		// %JPEG DCT compression
		ABIC				=	32010,	// ABIC (not fully supported)
		NEXT				=	32766,	// NeXT 2-bit RLE
		CCITTRLEW			=	32771,	// #1 w/ word alignment
		PACKBITS			=	32773,	// Macintosh RLE
		THUNDERSCAN 		=	32809,	// ThunderScan RLE
// codes 32895-32898 are reserved for ANSI IT8 TIFF/IT <dkelly@etsinc.com)
		IT8CTPAD			=	32895,	// IT8 CT w/padding
		IT8LW				=	32896,	// IT8 Linework RLE
		IT8MP				=	32897,	// IT8 Monochrome picture
		IT8BL				=	32898,	// IT8 Binary line art
// compression codes 32908-32911 are reserved for Pixar
		PIXARFILM			=	32908,	// Pixar companded 10bit LZW
		PIXARLOG			=	32909,	// Pixar companded 11bit ZIP
		DEFLATE 			=	32946,	// Deflate compression
// compression code 32947 is reserved for Oceana Matrix <dev@oceana.com>
		DCS 				=	32947,	// Kodak DCS encoding
		JBIG				=	34661	// ISO JBIG
	};
};

struct TIFF_Photometric
{
	enum
	{
		MINISWHITE			=	0,		// min value is white
		MINISBLACK			=	1,		// min value is black
		RGB 				=	2,		// RGB color model
		PALETTE 			=	3,		// color map indexed
		MASK				=	4,		// $holdout mask
		SEPARATED			=	5,		// !color separations
		YCBCR				=	6,		// !CCIR 601
		CIELAB				=	8		// !1976 CIE L*a*b*
	};
};

struct TIFF_Threshold
{
	enum
	{
		BILEVEL 			=	1,		// b&w art scan
		HALFTONE			=	2,		// or dithered scan
		ERRORDIFFUSE		=	3		// usually floyd-steinberg
	};
};

struct TIFF_FillOrder
{
	enum
	{
		MSB2LSB 		=	1,		// most significant -> least
		LSB2MSB 		=	2		// least significant -> most
	};
};

struct TIFF_Orientation
{
	enum
	{
		TOPLEFT 		=	1,		// row 0 top, col 0 lhs
		TOPRIGHT		=	2,		// row 0 top, col 0 rhs
		BOTRIGHT		=	3,		// row 0 bottom, col 0 rhs
		BOTLEFT 		=	4,		// row 0 bottom, col 0 lhs
		LEFTTOP 		=	5,		// row 0 lhs, col 0 top
		RIGHTTOP		=	6,		// row 0 rhs, col 0 top
		RIGHTBOT		=	7,		// row 0 rhs, col 0 bottom
		LEFTBOT 		=	8		// row 0 lhs, col 0 bottom
	};
};

struct TIFF_PlanarConfig
{
	enum
	{
		CONTIG			=	1,		// single image plane
		SEPARATE		=	2		// separate planes of data
	};
};

struct TIFF_ResponseUnit
{
	enum
	{
		UNIT_10S		=	1,		// tenths of a unit
		UNIT_100S		=	2,		// hundredths of a unit
		UNIT_1000S		=	3,		// thousandths of a unit
		UNIT_10000S 	=	4,		// ten-thousandths of a unit
		UNIT_100000S	=	5		// hundred-thousandths
	};
};

struct TIFF_Group3Options
{
	enum
	{
		G3_2DENCODING		=	0x1,	// 2-dimensional coding
		G3_UNCOMPRESSED 	=	0x2,	// data not compressed
		G3_FILLBITS 		=	0x4		// fill to byte boundary
	};
};

struct TIFF_Group4Options
{
	enum
	{
		G4_UNCOMPRESSED 	=	0x2 	// data not compressed
	};
};

struct TIFF_ResolutionUnit
{
	enum
	{
		NONE				=	1,		// no meaningful units
		INCH				=	2,		// english
		CENTIMETER			=	3,		// metric
		CENTIMETRE			=	3
	};
};

struct TIFF_CleanFaxData
{
	enum
	{
		CLEAN				=	0,		// no errors detected
		REGENERATED 		=	1,		// receiver regenerated lines
		UNCLEAN 			=	2		// uncorrected errors exist
	};
};

struct TIFF_InkSet
{
	enum
	{
		CMYK				=	1		// !cyan-magenta-yellow-black
	};
};

struct TIFF_ExtraSamples
{
	enum
	{
		UNSPECIFIED 		=	0,		// !unspecified data
		ASSOCALPHA			=	1,		// !associated alpha data
		UNASSALPHA			=	2		// !unassociated alpha data
	};
};

struct TIFF_SampleFormat
{
	// I added the SF_ prefix because the preprocessor got hold of INT
	// and VOID with predictable but nasty consequences.

	enum
	{
		SF_UINT				=	1,		// !unsigned integer data
		SF_INT 				=	2,		// !signed integer data
		SF_IEEEFP			=	3,		// !IEEE floating point data
		SF_VOID				=	4		// !untyped data
	};
};

struct TIFF_JPEGProc
{
	enum
	{
		BASELINE			=	1,		// !baseline sequential
		LOSSLESS			=	14		// !Huffman coded lossless
	};
};

struct TIFF_YCBCRPosition
{
	enum
	{
		CENTERED			=	1,		// !as in PostScript Level 2
		COSITED 			=	2		// !as in CCIR 601-1
	};
};

struct TIFF_FaxMode
{
	enum
	{
		CLASSIC				=	0x0000,			// default, include RTC
		NORTC				=	0x0001,			// no RTC at end of data
		NOEOL				=	0x0002,			// no EOL code at end of row
		BYTEALIGN			=	0x0004,			// byte align row
		WORDALIGN			=	0x0008,			// word align row
		FAXMODE_CLASSF		=	NORTC			// TIFF Class F
	};
};

// English spellings
#define GREYRESPONSEUNIT GRAYRESPONSEUNIT
#define GREYRESPONSECURVE GRAYRESPONSECURVE

struct TIFF_Tag
{
	enum
	{
		SUBFILETYPE 			= 254,		// subfile data descriptor
		OSUBFILETYPE			= 255,		// +kind of data in subfile
		IMAGEWIDTH				= 256,		// image width in pixels
		IMAGELENGTH 			= 257,		// image height in pixels
		BITSPERSAMPLE			= 258,		// bits per channel (sample)
		COMPRESSION 			= 259,		// data compression technique
		PHOTOMETRIC 			= 262,		// photometric interpretation
		THRESHHOLDING			= 263,		// +thresholding used on data
		CELLWIDTH				= 264,		// +dithering matrix width
		CELLLENGTH				= 265,		// +dithering matrix height
		FILLORDER				= 266,		// data order within a byte
		DOCUMENTNAME			= 269,		// name of doc. image is from
		IMAGEDESCRIPTION		= 270,		// info about image
		MAKE					= 271,		// scanner manufacturer name
		MODEL					= 272,		// scanner model name/number
		STRIPOFFSETS			= 273,		// offsets to data strips
		ORIENTATION 			= 274,		// +image orientation
		SAMPLESPERPIXEL 		= 277,		// samples per pixel
		ROWSPERSTRIP			= 278,		// rows per strip of data
		STRIPBYTECOUNTS 		= 279,		// bytes counts for strips
		MINSAMPLEVALUE			= 280,		// +minimum sample value
		MAXSAMPLEVALUE			= 281,		// +maximum sample value
		XRESOLUTION 			= 282,		// pixels/resolution in x
		YRESOLUTION 			= 283,		// pixels/resolution in y
		PLANARCONFIG			= 284,		// storage organization
		PAGENAME				= 285,		// page name image is from
		XPOSITION				= 286,		// x page offset of image lhs
		YPOSITION				= 287,		// y page offset of image lhs
		FREEOFFSETS 			= 288,		// +byte offset to free block
		FREEBYTECOUNTS			= 289,		// +sizes of free blocks
		GRAYRESPONSEUNIT		= 290,		// $gray scale curve accuracy
		GRAYRESPONSECURVE		= 291,		// $gray scale response curve
		GROUP3OPTIONS			= 292,		// 32 flag bits
		GROUP4OPTIONS			= 293,		// 32 flag bits
		RESOLUTIONUNIT			= 296,		// units of resolutions
		PAGENUMBER				= 297,		// page numbers of multi-page
		COLORRESPONSEUNIT		= 300,		// $color curve accuracy
		TRANSFERFUNCTION		= 301,		// !colorimetry info
		SOFTWARE				= 305,		// name & release
		DATETIME				= 306,		// creation date and time
		ARTIST					= 315,		// creator of image
		HOSTCOMPUTER			= 316,		// machine where created
		PREDICTOR				= 317,		// prediction scheme w/ LZW
		WHITEPOINT				= 318,		// image white point
		PRIMARYCHROMATICITIES	= 319,		// !primary chromaticities
		COLORMAP				= 320,		// RGB map for pallette image
		HALFTONEHINTS			= 321,		// !highlight+shadow info
		TILEWIDTH				= 322,		// !rows/data tile
		TILELENGTH				= 323,		// !cols/data tile
		TILEOFFSETS 			= 324,		// !offsets to data tiles
		TILEBYTECOUNTS			= 325,		// !byte counts for tiles
		BADFAXLINES 			= 326,		// lines w/ wrong pixel count
		CLEANFAXDATA			= 327,		// regenerated line info
		CONSECUTIVEBADFAXLINES	= 328,		// max consecutive bad lines
		SUBIFD					= 330,		// subimage descriptors
		INKSET					= 332,		// !inks in separated image
		INKNAMES				= 333,		// !ascii names of inks
		NUMBEROFINKS			= 334,		// !number of inks
		DOTRANGE				= 336,		// !0% and 100% dot codes
		TARGETPRINTER			= 337,		// !separation target
		EXTRASAMPLES			= 338,		// !info about extra samples
		SAMPLEFORMAT			= 339,		// !data sample format
		SMINSAMPLEVALUE 		= 340,		// !variable MinSampleValue
		SMAXSAMPLEVALUE 		= 341,		// !variable MaxSampleValue
		JPEGTABLES				= 347,		// %JPEG table stream

// Tags 512-521 are obsoleted by Technical Note #2
// which specifies a revised JPEG-in-TIFF scheme.

		JPEGPROC				= 512,		// !JPEG processing algorithm
		JPEGIFOFFSET			= 513,		// !pointer to SOI marker
		JPEGIFBYTECOUNT 		= 514,		// !JFIF stream length
		JPEGRESTARTINTERVAL 	= 515,		// !restart interval length
		JPEGLOSSLESSPREDICTORS	= 517,		// !lossless proc predictor
		JPEGPOINTTRANSFORM		= 518,		// !lossless point transform
		JPEGQTABLES 			= 519,		// !Q matrice offsets
		JPEGDCTABLES			= 520,		// !DCT table offsets
		JPEGACTABLES			= 521,		// !AC coefficient offsets

		YCBCRCOEFFICIENTS		= 529,		// !RGB -> YCbCr transform
		YCBCRSUBSAMPLING		= 530,		// !YCbCr subsampling factors
		YCBCRPOSITIONING		= 531,		// !subsample positioning
		REFERENCEBLACKWHITE 	= 532,		// !colorimetry info
// tags 32952-32956 are private tags registered to Island Graphics
		REFPTS					= 32953,	// image reference points
		REGIONTACKPOINT 		= 32954,	// region-xform tack point
		REGIONWARPCORNERS		= 32955,	// warp quadrilateral
		REGIONAFFINE			= 32956,	// affine transformation mat
// tags 32995-32999 are private tags registered to SGI
		MATTEING				= 32995,	// $use ExtraSamples
		DATATYPE				= 32996,	// $use SampleFormat
		IMAGEDEPTH				= 32997,	// z depth of image
		TILEDEPTH				= 32998,	// z depth/data tile
// tags 33300-33309 are private tags registered to Pixar
/*
 * PIXAR_IMAGEFULLWIDTH and PIXAR_IMAGEFULLLENGTH
 * are set when an image has been cropped out of a larger image.
 * They reflect the size of the original uncropped image.
 * The XPOSITION and YPOSITION can be used
 * to determine the position of the smaller image in the larger one.
 */
		PIXAR_IMAGEFULLWIDTH	= 33300,		// full image size in x
		PIXAR_IMAGEFULLLENGTH	= 33301,		// full image size in y
// tag 33405 is a private tag registered to Eastman Kodak
		WRITERSERIALNUMBER		= 33405,		// device serial number
// tag 33432 is listed in the 6.0 spec w/ unknown ownership
		COPYRIGHT				= 33432,		// copyright string
// 34016-34029 are reserved for ANSI IT8 TIFF/IT <dkelly@etsinc.com)
		IT8SITE 				= 34016,		// site name
		IT8COLORSEQUENCE		= 34017,		// color seq. [RGB,CMYK,etc]
		IT8HEADER				= 34018,		// DDES Header
		IT8RASTERPADDING		= 34019,		// raster scanline padding
		IT8BITSPERRUNLENGTH 	= 34020,		// # of bits in short run
		IT8BITSPEREXTENDEDRUNLENGTH = 34021,	// # of bits in long run
		IT8COLORTABLE			= 34022,		// LW colortable
		IT8IMAGECOLORINDICATOR	= 34023,		// BP/BL image color switch
		IT8BKGCOLORINDICATOR	= 34024,		// BP/BL bg color switch
		IT8IMAGECOLORVALUE		= 34025,		// BP/BL image color value
		IT8BKGCOLORVALUE		= 34026,		// BP/BL bg color value
		IT8PIXELINTENSITYRANGE	= 34027,		// MP pixel intensity value
		IT8TRANSPARENCYINDICATOR= 34028,		// HC transparency switch
		IT8COLORCHARACTERIZATION= 34029,		// color character. table
// tags 34232-34236 are private tags registered to Texas Instruments
		FRAMECOUNT				= 34232,		// Sequence Frame Count
// tag 34750 is a private tag registered to Adobe?
		ICCPROFILE				= 34675,		// ICC profile data
// tag 34750 is a private tag registered to Pixel Magic
		JBIGOPTIONS 			= 34750,		// JBIG options
// tags 34908-34914 are private tags registered to SGI
		FAXRECVPARAMS			= 34908,		// encoded Class 2 ses. parms
		FAXSUBADDRESS			= 34909,		// received SubAddr string
		FAXRECVTIME 			= 34910,		// receive time (secs)
// tag 65535 is an undefined tag used by Eastman Kodak
		DCSHUESHIFTVALUES		= 65535,		// hue shift correction data

// The following are "pseudo tags" that can be used to control codec-specific
// functionality.  These tags are not written to file.	Note that these values
// start at 0xffff+1 so that they'll never collide with Aldus-assigned tags.

		JPEGQUALITY 			= 65537,	// Compression quality level
// Note: quality level is on the IJG 0-100 scale.  Default value is 75
		JPEGCOLORMODE			= 65538,	// Auto RGB<=>YCbCr convert?
//			JPEGCOLORMODE_RAW=	0x0000			// no conversion (default)
//			JPEGCOLORMODE_RGB=	0x0001			// do auto conversion
		JPEGTABLESMODE			= 65539,	// What to put in JPEGTables
//			JPEGTABLESMODE_QUANT=0x0001 		// include quantization tbls
//			JPEGTABLESMODE_HUFF= 0x0002 		// include Huffman tbls
// Note: default is JPEGTABLESMODE_QUANT | JPEGTABLESMODE_HUFF

// ^^^ Everything above here was copied from tifflib
//	   Everything below is mine.

// Chase Manhattan Bank
		CHEQUE_HISTORY=0x8200,	// 33280 Index of an earlier version
		CHEQUE_MEDIUM,			// 33281 Delivery medium
								//		dlt
								//		midas
								//		cims
		CHEQUE_ORIGIN,			// 33282 Originating system (image capture)
								//		MIDAS systems
								//			tx	Texas
								//			de	Delaware
								//			ny	New York (Syracuse)
								//		FRB
								//			dal	Dallas
								//			phi	Philadelphia
								//			uti	Utica
								//		CIMS
								//			syr	Syracuse
								//		7780
								//			ncr	Syracuse
		CHEQUE_PROC_CTRL,		// 33283 Process Control field
		CHEQUE_SEQUENCE,		// 33284 Sequence number (sometimes available)
		CHEQUE_MICR,			// 33285 MICR line as captured by MICR reader
		CHEQUE_SUBACCOUNT,		// 33286 AFS only (at the moment)
		CHEQUE_CUSTOMER,		// 33287 Client name (abbreviated)
		CHEQUE_ACCOUNT,			// 33288 Client's account
		CHEQUE_NUMBER,			// 33289 Serial number
		CHEQUE_AMOUNT,			// 33290 Value of cheque
	 	CHEQUE_DATE,			// 33291 Processing date
		CHEQUE_SOURCE,			// 33292 Image source (file, system, computer)
								//		7780	<computer name>
								//		CIMS	<batch name>
								//		MIDAS	midas
								//		tape	(see below)
								//	For tape sources, the image source string
								//  comprises a six-character tape volume
								//	label, an underscore, and the dataset
								//	name (e.g. ut0036_uti00106.tif)
		CHEQUE_CAPTURE_TIME,	// 33293 Time at which image was captured
		CHEQUE_REPAIR_ID,		// 33294 Repair workstation (if any)
		CHEQUE_REPAIR_TIME,		// 33295 Time at which image was repaired
		CHEQUE_RTN,				// 33296 Routing transit number
		CHEQUE_WORKFLOW,		// 33297 Destination & repair flags
		CHEQUE_OCR,		 		// 33298 MICR line as captured by OCR reader
		CHEQUE_REPAIR_FLAGS,	// 33299 Bit mask of fields needing repair
		// NOTE: Do not create tag 33300.  That one and those just above
		//		 it are already allocated to others.  It is safe to use
		//		 lower numbers.

// FRB Syracuse et al.  Unisys embedded IFD tags

		FRB_DATA = 33881		// Unisys Data Parameters (embedded IFD)
	};
};

struct FRB_Tag
{
	enum
	{
		DIN = 1,				// Document identification number (not used)
		CheckSerialNum,			//
		ProcessingDate,			// YYYYMMDD
		CheckAmount,			//
		TransactionCode,		//
		AccountNumber,			//
		RoutingTransit,			//
		AuxiliaryOnUs,			//
		Position44,				// Unused
		UserArea = 11			// Another embedded IFD
	};
};

struct CBIS_Tag
{
	enum
	{
		UserID,					// byte[4], "CBIS" exactly.
		SiteID,					// short
		CaptureStartTime,		// time_t
		CapturePID,				// long
		DocumentNum,			// long
		MigrationDate,			// byte[8]
		MicroFilmSeq,			// byte[8]
		ItemSeq,				// byte[9]
		TracerNumber,			// byte[15]
		SideID,					// byte (0 = front, 1 = back)
		ZoneList,				// byte
		NLIFD_Offset			// byte
	};
};

struct Workflow
{
	enum
	{
		WORKFLOW_DESTINATION	=	   7,	// Bit mask (3 bits)
		WORKFLOW_REPAIR			=	   8,	// Bit mask (1 bit)
		WORKFLOW_SOURCE			=	0xF0,	// Bit mask (4 bits)
		WORKFLOW_SIDE			=	0x100	// Bit mask (1 bit)
	};
};

struct WorkflowDestination
{
	enum
	{
		archive,
		midas,
		afs,
		chemex,
		hcr
	};
};

struct WorkflowSource
{
	enum
	{
		SYR_7780				=	0x00,
		SYR_3890				=	0x10,
		HCR						=	0x20,
		FRB_UTICA				=	0x30
	};
};

struct RepairFlags
{
	enum
	{
		REPAIR_AMOUNT			=	0x01,
		REPAIR_ACCOUNT			=	0x02,
		REPAIR_RTN				=	0x04,
		REPAIR_NUMBER			=	0x08,
		REPAIR_SUBACCOUNT		=	0x10
	};
};

#endif



