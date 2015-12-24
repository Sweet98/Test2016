//======================================================================================
// TIFF_CSB.h
//======================================================================================
// $RCSfile: TIFF_CSB.h $
// $Revision: 1.1 $
// $Date: 2009/04/03 15:08:45EDT $
//======================================================================================

//======================================================================================
//
//	This file is part of a TIFF compression / decompression package that Tim Meier picked
//	up from .  This package was ported from the CxImage library to be a standalone
//	compression / decompression library for TIFF images.
//
//======================================================================================



#ifndef CSB_H
#define CSB_H


#include	"TIFF_TiffCon.hpp"


// The following typedefs define the intrinsic size of
// data types used in the *exported* interfaces.  These
// definitions depend on the proper definition of types
// in tiff.h.  Note also that the varargs interface used
// to pass tag types and values uses the types defined in
// tiff.h directly.
//
// NB: ttag_t is unsigned int and not unsigned short because
//     ANSI C requires that the type before the ellipsis be a
//     promoted type (i.e. one of int, unsigned int, pointer,
//     or double) and because we defined pseudo-tags that are
//     outside the range of legal Aldus-assigned tags.
// NB: tsize_t is int32 and not uint32 because some functions
//     return -1.
// NB: toff_t is not off_t for many reasons; TIFFs max out at
//     32-bit file offsets being the most important

// Not needed, because we're including TIFF_TiffCon.
//#define	uint32	unsigned int
//#define	uint16	unsigned short
//#define	int32	int
//#define	uchar	unsigned char

#define	tsize_t	int32

typedef	uint32			ttag_t;			// directory tag
typedef	uint16			tdir_t;			// directory index
typedef	uint16			tsample_t;		// sample number
typedef	uint32			tstrip_t;		// strip number
typedef uint32			ttile_t;		// tile number
typedef	int32			toff_t;			// file offset

typedef void (*TIFFFaxFillFunc) (unsigned char *, uint16 *, uint16 *, uint32);

enum _codec {Compressor = 6, Decompressor = 13};

// Compression+decompression state blocks are
// derived from this "base state" block.

typedef struct
{
//... Start of stuff migrated from TIFF object
//	tsize_t			scanlinesize;		// Do we need this?
//	tsize_t			scanlineskew;		// Do we need this?
	uchar			*rawdata;			// Raw data buffer (encoded data)
	tsize_t			rawdatasize;		// Number of bytes in raw data buffer
	uchar			*rawcp;				// Current spot in raw data buffer
	tsize_t			rawcc;				// Bytes remaining in raw data buffer
	int				row;				// Current row number
	float			yresolution;		// Vertical resolution
	int				resolutionunit;		// Could be int16 or uint16.
//
//	Notes:
//
//		The rawdata field is not used by the codec and is just for use by
//		client code.  It is intended to hold a pointer to the beginning of
//		the encoded data.  Prior to encoding or decoding, rawcp should
//		be the same as rawdata but rawcp will change and rawdata will not.
//
//		Prior to encoding rawcc should be set to zero; after encoding it
//		will contain the number of bytes of encoded data.  Prior to decoding
//		rawcc should contain the number of bytes of encoded data.
//
//... End of stuff migrated from TIFF object, start of other stuff
	int				codec;				// Poor man's RTTI
	int				rows;				// Number of rows in image
//... End of other stuff
	int				mode;				// operating mode
	uint32			rowbytes;			// bytes in a decoded scanline
	uint32			rowpixels;			// pixels in a scanline
//	uint32			badfaxrun;			// BadFaxRun tag
//	uint32			badfaxlines;		// BadFaxLines tag
	uint32			groupoptions;		// Group 3/4 options tag
//	uint32			recvparams;			// encoded Class 2 session params
//	char 			*subaddress;		// subaddress string
//	uint32			recvtime;			// time spent receiving (secs)
	uint16			cleanfaxdata;		// CleanFaxData tag
}   Fax3BaseState;

//#define	Fax3State(tif)		((Fax3BaseState*) (tif)->tif_data)
//
// That was the original definition for Fax3State.  It assumed a data block
// attached to a TIFF object.  I have two objections to that.
//
// Firstly, a design goal of this hack is to produce code which allows me to
// manipulate image data directly without reference to an enclosing TIFF
// envelope.  I want to construct images in memory and there will be many
// occasions when I want to invoke the G4 codec on an image which is not
// contained in a TIFF.  For me the TIFF packaging needs to be handled as a
// completely separate set of operations.
//
// My second objection is more subtle.  The use of a data area attached to
// and owned by a TIFF object is simply wrong from an object-oriented view-
// point.  A TIFF can contain many images and the data properly belongs to
// a single image.  In my view it should have been part of the IFD rather
// than part of the TIFF object.
//
// Anyway, I have elected to separate the codec data from the TIFF so that
// means all the definitions and functions which address a TIFF object have
// to change.  Fortunately the change is quite simple.  I define an object
// called a codec state block which is a direct replacement for the TIFF
// data block.  The declaration for that appears a little later ...
//
// Now the redefinition of Fax3State, leaving the formal parameter intact:

#define	Fax3State(tif)		((Fax3BaseState*) (tif))

typedef struct
{
	Fax3BaseState		b;
	const uchar			*bitmap;	// bit reversal table
	uint32				data;		// current i/o byte/word
	int					bit;		// current i/o bit in byte
	int					EOLcnt;		// count of EOL codes recognized
	TIFFFaxFillFunc		fill;		// fill routine
	uint16				*runs;		// b&w runs for current/previous row
	uint16				*refruns;	// runs for reference line
	uint16				*curruns;	// runs for current line
}   Fax3DecodeState;

// I can afford to leave this unaltered.  The 'tif' is just a formal parameter.
#define	DecoderState(tif)	((Fax3DecodeState*) Fax3State(tif))

struct EncoderMode
{
	enum
	{
		G3_1D, G3_2D
	};
};

typedef struct
{
	Fax3BaseState	b;
	int				data;			/* current i/o byte */
	int				bit;			/* current i/o bit in byte */
	int				tag;			/* encoding state */
	uchar			*refline;		/* reference line for 2d decoding */
	int				k;				/* #rows left that can be 2d encoded */
	int				maxk;			/* max #rows that can be 2d encoded */
}   Fax3EncodeState;

// I can afford to leave this unaltered.  The 'tif' is just a formal parameter.
#define	EncoderState(tif)	((Fax3EncodeState*) Fax3State(tif))

typedef union _csb
{
	Fax3BaseState		bs;
	Fax3EncodeState		es;
	Fax3DecodeState		ds;
}	CSB;

#endif



