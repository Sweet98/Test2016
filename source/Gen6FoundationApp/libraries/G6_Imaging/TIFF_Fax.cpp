//======================================================================================
// TIFF_Fax.cpp
//======================================================================================
// $RCSfile: TIFF_Fax.cpp $
// $Revision: 1.2 $
// $Date: 2009/04/09 14:40:54EDT $
//======================================================================================

//======================================================================================
//
//	This file is part of a TIFF compression / decompression package that Tim Meier picked
//	up from .  This package was ported from the CxImage library to be a standalone
//	compression / decompression library for TIFF images.
//
//======================================================================================



//ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
//º
//º	The code herein is a direct cut from LibTIFF by Sam Leffler.  The CCITT
//º	G3/G4 routines are in turn adapted from algorithms by Frank Cringle.
//º
//º	My whole purpose in hacking this code was to produce codecs which could
//º	be used outside of any TIFF library.
//º
//º	Jon Saxton, September 1998
//º
//ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ

//ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//³ CCITT Group 3 (T.4) and Group 4 (T.6) Compression Support.
//³
//³ This file contains support for decoding and encoding TIFF
//³ compression algorithms 2, 3, 4, and 32771.
//³
//³ Decoder support is derived, with permission, from the code
//³ in Frank Cringle's viewfax program;
//³      Copyright (C) 1990, 1995  Frank D. Cringle.
//ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

// THINGS I NEED TO DO
//
// Figure out when to initialise csb->row.  It seems that it should start off
// at -1 even though it is an unsigned int.
//
// Write an equivalent for Fax3SetupState() or figure out how and when to
// do it before invoking the codec
//
// Determine the sequence of calls necessary to encode or decode an image.

#include <stdio.h>
//#include "ints.h"
#include "TIFF_CSB.h"
#include "TIFF_Fax.h"
#include "TIFF_FaxPriv.h"
#define	G3CODES
#include "TIFF_G3States.h"
#include "TIFF_Swab.h"
#include "TIFF_T4.h"
#include "TIFF_TiffCon.hpp"
#ifndef _WIN32_WCE
    #include <assert.h>
#endif
#include <stdarg.h>
#include <string.h>

static int Fax4PostEncode(CSB *csb);

#define	is2DEncoding(sp) \
	(sp->b.groupoptions & TIFF_Group3Options::G3_2DENCODING)

#define	isAligned(p,t)	((((uint32)(p)) & (sizeof (t)-1)) == 0)

// Group 3 and Group 4 Decoding.

// These macros glue the TIFF library state to the state expected by
// Frank's decoder.
//
// Note that formal parameter 'tif' was changed to 'csb'.

#define	DECLARE_STATE(csb, sp, mod)										\
    static const char module[] = mod;									\
    Fax3DecodeState *sp = DecoderState(csb);							\
    int a0;								/* reference element */			\
    int lastx = sp->b.rowpixels;		/* last element in row */		\
    uint32 BitAcc;						/* bit accumulator */			\
    int BitsAvail;						/* # valid bits in BitAcc */	\
    int RunLength;						/* length of current run */		\
    uchar* cp;							/* next byte of input data */	\
    uchar* ep;							/* end of input data */			\
    uint16* pa;							/* place to stuff next run */	\
    uint16* thisrun;					/* current row's run array */	\
    int EOLcnt;							/* # EOL codes recognized */	\
    const uchar* bitmap = sp->bitmap;	/* input data bit reverser */	\
    const TIFFFaxTabEnt* TabEnt

#define	DECLARE_STATE_2D(csb, sp, mod)									\
    DECLARE_STATE(csb, sp, mod);										\
    int b1;								/* next change on prev line */	\
    uint16* pb							/* next run in reference line */\

// Many of the following definitions had to be changed because they referred
// to fields in a TIFF which I had moved into the codec state block.

// Load any state that may be changed during decoding.
//
//#define	CACHE_STATE(tif, sp)								\
//	do															\
//	{															\
//		BitAcc = sp->data;										\
//    	BitsAvail = sp->bit;									\
//		EOLcnt = sp->EOLcnt;									\
//		cp = (unsigned char*) tif->tif_rawcp;					\
//		ep = cp + tif->tif_rawcc;								\
//	} while (0)

#define	CACHE_STATE(sp)											\
	do															\
	{															\
    	BitAcc = sp->data;										\
    	BitsAvail = sp->bit;									\
    	EOLcnt = sp->EOLcnt;									\
    	cp = (unsigned char*) sp->b.rawcp;						\
    	ep = cp + sp->b.rawcc;									\
	} while (0)

// Save state possibly changed during decoding.
//
//#define	UNCACHE_STATE(tif, sp)								\
//	do															\
//	{															\
//	    sp->bit = BitsAvail;									\
//	    sp->data = BitAcc;										\
//	    sp->EOLcnt = EOLcnt;									\
//	    tif->tif_rawcc -= (uchar *) cp - tif->tif_rawcp;		\
//	    tif->tif_rawcp = (uchar *) cp;							\
//	} while (0)

#define	UNCACHE_STATE(sp)										\
	do															\
	{															\
	    sp->bit = BitsAvail;									\
	    sp->data = BitAcc;										\
	    sp->EOLcnt = EOLcnt;									\
	    sp->b.rawcc -= (uchar *) cp - sp->b.rawcp;				\
		sp->b.rawcp = (uchar *) cp;								\
	} while (0)


//ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//³
//³ Procedure:      Fax3PreDecode
//³
//³ Purpose:		Setup state for decoding a strip
//³
//³ Inputs:         None
//³
//³ Outputs:        None
//³
//³ Returns:        Nothing
//³
//³ Side-effects:   None
//³
//³ Notes:			I'm initialising row to -1 here.
//³
//³
//ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

//static int Fax3PreDecode(TIFF *tif, tsample_t s)
// Altered to remove TIFF dependence.  Also, parameter 's' was not used
// except in the statement "(void) s;" which does nothing.
//
// Now the fill order has to be passed in as a parameter instead.

void Fax3PreDecode(CSB *csb, uint16 td_fillorder)
{
	Fax3DecodeState *sp = DecoderState(csb);

	//	(void) s;
#ifndef _WIN32_WCE

	assert(sp != NULL);
#endif

	csb->bs.codec = Decompressor;
	csb->bs.row = -1;
	sp->bit = 0;				/* force initial read */
	sp->data = 0;
	sp->EOLcnt = 0;				/* force initial scan for EOL */

	// Decoder assumes lsb-to-msb bit order.  Note that we select this here
	// rather than in Fax3SetupState so that viewers can hold the image open,
	// fiddle with the FillOrder tag value, and then re-decode the image.
	// Otherwise they'd need to close and open the image to get the state
	// reset.
	//
	// I'm not sure that is true now.  (jrs)

	sp->bitmap =
	    TIFFGetBitRevTable(td_fillorder != TIFF_FillOrder::LSB2MSB);

	if (sp->refruns)
	{							/* init reference line to white */
		sp->refruns[0] = (uint16) sp->b.rowpixels;
		sp->refruns[1] = 0;
	}
}

//ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//³
//³ Procedure:      codecError
//³
//³ Purpose:		Report errors encountered during image compression or
//³					decompression.
//³
//³ Inputs:         Output file, message string and other parameters.
//³
//³ Outputs:        None
//³
//³ Returns:        Nothing
//³
//³ Side-effects:	Writes to stderr.
//³
//³ Notes:			This routine may be replaced at some stage.
//³
//³
//ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

void codecError(const char *module, const char *format, ...)
{
	va_list		args;
	if (module != NULL)
		fprintf(stderr, "%s: ", module);
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
}

//ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//³ Routine for handling various errors/conditions.
//³ Note how they are "glued into the decoder" by
//³ overriding the definitions used by the decoder.
//ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

static void Fax3Unexpected(const char *module, CSB *csb, uint32 a0)
{
	codecError(module, "%s: Bad code word at scanline %d (x %lu)\n",
	           csb->bs.row, a0);
}

#define	unexpected(table, a0)	Fax3Unexpected(table, csb, a0)

static void Fax3Extension(const char *module, CSB *csb, uint32 a0)
{
	codecError(module,
	           "Uncompressed data (not supported) at scanline %d (x %lu)\n",
	           csb->bs.row, a0);
}

#define	extension(a0)	Fax3Extension(module, csb, a0)

static void Fax3BadLength(const char *module, CSB *csb, uint32 a0, uint32 lastx)
{
	codecError(module, "%s at scanline %d (got %lu, expected %lu)\n",
	           a0 < lastx ? "Premature EOL" : "Line length mismatch",
	           csb->bs.row, a0, lastx);
}

#define	badlength(a0,lastx)	Fax3BadLength(module, csb, a0, lastx)

static void Fax3PrematureEOF(const char *module, CSB *csb, uint32 a0)
{
	codecError(module, "Premature EOF at scanline %d (x %lu)\n", csb->bs.row, a0);
}

#define	prematureEOF(a0)	Fax3PrematureEOF(module, csb, a0)

#define	Nop

//ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//³
//³ Procedure:      Fax3Decode1D
//³
//³ Purpose:		Decode the requested amount of G3 1D-encoded data.
//³
//³ Inputs:         Codec state block
//³
//³ Outputs:        buf
//³
//³ Returns:        Nothing
//³
//³ Side-effects:   None
//³
//³ Notes:
//³
//³
//ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
#define	SWAP(t,a,b)	{ t x; x = (a); (a) = (b); (b) = x; }

//ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//³
//³ Procedure:      Fax3Decode2D
//³
//³ Purpose:		Decode the requested amount of G3 2D-encoded data.
//³
//³ Inputs:         None
//³
//³ Outputs:        None
//³
//³ Returns:        Nothing
//³
//³ Side-effects:   None
//³
//³ Notes:
//³
//³
//ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
#undef SWAP

// The ZERO & FILL macros must handle spans < 2*sizeof(long) bytes.
// For machines with 64-bit longs this is <16 bytes; otherwise
// this is <8 bytes.  We optimize the code here to reflect the
// machine characteristics.

#if defined(__alpha) || _MIPS_SZLONG == 64
#define FILL(n, cp)							    \
    switch (n) {							    \
    case 15:(cp)[14] = 0xff; case 14:(cp)[13] = 0xff; case 13: (cp)[12] = 0xff;\
    case 12:(cp)[11] = 0xff; case 11:(cp)[10] = 0xff; case 10: (cp)[9] = 0xff;\
    case  9: (cp)[8] = 0xff; case  8: (cp)[7] = 0xff; case  7: (cp)[6] = 0xff;\
    case  6: (cp)[5] = 0xff; case  5: (cp)[4] = 0xff; case  4: (cp)[3] = 0xff;\
    case  3: (cp)[2] = 0xff; case  2: (cp)[1] = 0xff;			      \
    case  1: (cp)[0] = 0xff; (cp) += (n); case 0:  ;			      \
    }
#define ZERO(n, cp)							\
    switch (n) {							\
    case 15:(cp)[14] = 0; case 14:(cp)[13] = 0; case 13: (cp)[12] = 0;	\
    case 12:(cp)[11] = 0; case 11:(cp)[10] = 0; case 10: (cp)[9] = 0;	\
    case  9: (cp)[8] = 0; case  8: (cp)[7] = 0; case  7: (cp)[6] = 0;	\
    case  6: (cp)[5] = 0; case  5: (cp)[4] = 0; case  4: (cp)[3] = 0;	\
    case  3: (cp)[2] = 0; case  2: (cp)[1] = 0;			      	\
    case  1: (cp)[0] = 0; (cp) += (n); case 0:  ;			\
    }
#else
#define FILL(n, cp)							    \
    switch (n) {							    \
    case 7: (cp)[6] = 0xff; case 6: (cp)[5] = 0xff; case 5: (cp)[4] = 0xff; \
    case 4: (cp)[3] = 0xff; case 3: (cp)[2] = 0xff; case 2: (cp)[1] = 0xff; \
    case 1: (cp)[0] = 0xff; (cp) += (n); case 0:  ;			    \
    }
#define ZERO(n, cp)	\
    switch (n)	\
	{ \
    case 7: (cp)[6] = 0; case 6: (cp)[5] = 0; case 5: (cp)[4] = 0;	\
    case 4: (cp)[3] = 0; case 3: (cp)[2] = 0; case 2: (cp)[1] = 0;	\
    case 1: (cp)[0] = 0; (cp) += (n); case 0:  ;			\
    }
#endif /* defined(__alpha) ... */

//ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//³
//³ Procedure:      _TIFFFax3fillruns
//³
//³ Purpose:		Bit-fill a row according to the white/black runs generated
//³					during G3/G4 decoding.
//³
//³ Inputs:         None
//³
//³ Outputs:        Buffer
//³
//³ Returns:        Nothing
//³
//³ Side-effects:   None
//³
//³ Notes:
//³
//³
//ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

void _TIFFFax3fillruns(uchar *buf, uint16 *runs, uint16 *erun, uint32 lastx)
{

	static const unsigned char _fillmasks[] =
	    {
	        0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff
	    };
	uchar *cp = 0;
	uint32 x, bx, run;
	int32 n, nw;
	long *lp;
	bool  bInc = false;
	uchar uchTmp = 0;

	if ((erun - runs) & 1)
		*erun++ = 0;
	x = 0;
	for (; runs < erun; runs += 2)
	{
		run = runs[0];
		if (x + run > lastx)
		{
			run = lastx - x;
			runs[0] = (uint16) run;
		}
		if (run)
		{
			cp = buf + (x >> 3);
			bx = x & 7;
			if (run > 8 - bx)
			{
				bInc = false;
				uchTmp = *cp;
				if( bx > 0 )
				{
					bInc = true;
					// align to byte boundary
					uchTmp &= (uchar)(0xff << (8 - (uchar)bx));
					run -= (8 - bx);
				}
				if( bInc )
					*cp++ = uchTmp;
				n = run >> 3;
				if (n)				// multiple bytes to fill
				{
					if ((n / sizeof(long)) > 1)
					{
						// Align to longword boundary and fill.
						for (; n && !isAligned(cp, long); n--)
							*cp++ = 0x00;
						lp = (long *) cp;
						nw = (int32) (n / sizeof(long));
						n -= nw * sizeof(long);
						do
						{
							*lp++ = 0L;
						}
						while (--nw);
						cp = (uchar *) lp;
					}
					ZERO(n, cp);
					run &= 7;
				}

#ifdef PURIFY
				if (run)
					cp[0] &= 0xff >> run;
#else

				cp[0] &= 0xff >> run;
#endif // PURIFY

			}
			else
				cp[0] &= ~(_fillmasks[run] >> bx);
			x += runs[0];
		}
		run = runs[1];
		if (x + run > lastx)
		{
			run = lastx - x;
			runs[1] = (uint16) run;
		}

		if (run)
		{
			cp = buf + (x >> 3);
			bx = x & 7;
			if (run > 8 - bx)
			{
				if (bx)				// align to byte boundary
				{
					*cp++ |= 0xff >> bx;
					run -= 8 - bx;
				}
				n = run >> 3;
				if (n)				// multiple bytes to fill
				{
					if ((n / sizeof(long)) > 1)
					{
						//
						//   Align to longword boundary and fill.
						//
						for (; n && !isAligned(cp, long); n--)
							*cp++ = 0xff;
						lp = (long *) cp;
						nw = (int32) (n / sizeof(long));
						n -= nw * sizeof(long);
						do
						{
							*lp++ = -1L;
						}
						while (--nw);
						cp = (uchar *) lp;
					}
					FILL(n, cp);
					run &= 7;
				}
#ifdef PURIFY
				if (run)
					cp[0] |= 0xff00 >> run;
#else

				cp[0] |= 0xff00 >> run;
#endif // PURIFY

			}
			else
				cp[0] |= _fillmasks[run] >> bx;
			x += runs[1];
		}

	}
#ifndef _WIN32_WCE
	assert(x == lastx);
#endif
}

#undef	ZERO
#undef	FILL

// CCITT Group 3 FAX Encoding.

// Some significant changes here.  I never deal with tiled or striped images
// and I am not writing to a file!

//#define Fax3FlushBits(tif, sp)							\
//	{														\
//		if ((tif)->rawcc >= (tif)->rawdatasize)				\
//			(void) TIFFFlushData1(tif);						\
//		*(tif)->tif_rawcp++ = (sp)->data;					\
//		(tif)->tif_rawcc++;									\
//		(sp)->data = 0, (sp)->bit = 8;						\
//	}

#define	Fax3FlushBits(csb, sp)								\
	{														\
		*(csb)->bs.rawcp++ = (sp)->data;					\
		(csb)->bs.rawcc++;									\
		(sp)->data = 0, (sp)->bit = 8;						\
	}

//#define _FlushBits(tif)									\
//	{														\
//		if ((tif)->tif_rawcc >= (tif)->tif_rawdatasize)		\
//			(void) TIFFFlushData1(tif);						\
//		*(tif)->tif_rawcp++ = data;							\
//		(tif)->tif_rawcc++;									\
//		data = 0, bit = 8;									\
//	}

#define	_FlushBits(csb)										\
	{														\
		*(csb)->bs.rawcp++ = data;							\
		(csb)->bs.rawcc++;									\
		data = 0, bit = 8;									\
	}

static const int _msbmask[9] =
    {
        0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff
    };

// No change to this one

#define	_PutBits(csb, bits, length)								\
	{															\
		while ( (int) length > bit)									\
		{														\
			data |= bits >> (length - bit);						\
			length -= bit;										\
			_FlushBits(csb);									\
		}														\
		data |= (bits & _msbmask[length]) << (bit - length);	\
		bit -= length;											\
		if (bit == 0)											\
			_FlushBits(csb);									\
	}

//ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//³
//³ Procedure:      Fax3PutBits
//³
//³ Purpose:		Write a variable-length bit value to the output stream.
//³
//³ Inputs:         None
//³
//³ Outputs:        None
//³
//³ Returns:        Nothing
//³
//³ Side-effects:   None
//³
//³ Notes:			At most 16 bits will be written.
//³					"Written" is the wrong word.  It really means "transferred
//³					to the output buffer" since no I/O occurs here.
//³
//³
//ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

static void Fax3PutBits(CSB *csb, uint32 bits, uint32 length)
{
	Fax3EncodeState *sp = EncoderState(csb);
	int bit = sp->bit;
	int data = sp->data;

	_PutBits(csb, bits, length);

	sp->data = data;
	sp->bit = bit;
}


// Write a code to the output stream.

#define putcode(tif, te)	Fax3PutBits(tif, (te)->code, (te)->length)

#ifdef FAX3_DEBUG
#define	DEBUG_COLOR(w) (tab == TIFFFaxWhiteCodes ? w "W" : w "B")
#define	DEBUG_PRINT(what,len) {						\
    int t;								\
    printf("%08X/%-2d: %s%5d\t", data, bit, DEBUG_COLOR(what), len);	\
    for (t = length-1; t >= 0; t--)					\
	putchar(code & (1<<t) ? '1' : '0');				\
    putchar('\n');							\
}
#endif /* FAX3_DEBUG */

//ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//³
//³ Procedure:      putspan()
//³
//³ Purpose:		Emit the sequence of codes that describes the span of
//³					zeros or ones.
//³
//³
//³
//³ Inputs:         Codec state block
//³					Length of span
//³					Table holding the make-up and terminating codes.
//³
//³
//³ Outputs:        None
//³
//³ Returns:        Nothing
//³
//³ Side-effects:   None
//³
//³ Notes:			Output is "written" to memory block addressed by
//³
//³
//ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

static void putspan(CSB *tif, int32 span, const tableentry *tab)
{
	Fax3EncodeState *sp = EncoderState(tif);
	int bit = sp->bit;
	int data = sp->data;
	uint32 code, length;

	while (span >= 2624)
	{
		const tableentry *te = &tab[63 + (2560 >> 6)];

		code = te->code, length = te->length;
#ifdef FAX3_DEBUG

		DEBUG_PRINT("MakeUp", te->runlen);
#endif

		_PutBits(tif, code, length);
		span -= te->runlen;
	}
	if (span >= 64)
	{
		const tableentry *te = &tab[63 + (span >> 6)];

#ifndef _WIN32_WCE

		assert(te->runlen == 64 * (span >> 6));
#endif

		code = te->code, length = te->length;
#ifdef FAX3_DEBUG

		DEBUG_PRINT("MakeUp", te->runlen);
#endif

		_PutBits(tif, code, length);
		span -= te->runlen;
	}
	code = tab[span].code, length = tab[span].length;
#ifdef FAX3_DEBUG

	DEBUG_PRINT("  Term", tab[span].runlen);
#endif

	_PutBits(tif, code, length);

	sp->data = data;
	sp->bit = bit;
}

//ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//³
//³ Procedure:      Fax3PutEOL
//³
//³ Purpose:		Emit an EOL code onto the output stream.
//³
//³ Inputs:         Codec state block.
//³
//³ Outputs:        None
//³
//³ Returns:        Nothing
//³
//³ Side-effects:   None
//³
//³ Notes:			The zero-fill logic for byte-aligning encoded scanlines is
//³					handled here.  We also handle emission of the tag bit for
//³					the next scanline when doing 2-D encoding.
//³
//³					This function is called during compression.
//³
//ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//³
//³ Procedure:      Fax3PreEncode
//³
//³ Purpose:		Reset encoding state at the start of a strip.
//³
//³ Inputs:         Codec state block, dummy.
//³
//³ Outputs:        None
//³
//³ Returns:        Nothing
//³
//³ Side-effects:   None
//³
//³ Notes:
//³
//³
//ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

static int Fax3PreEncode(CSB *csb) //, tsample_t s)
{
	Fax3EncodeState *sp = EncoderState(csb);

	//	(void) s;
#ifndef _WIN32_WCE

	assert(sp != NULL);
#endif

	csb->bs.codec = Compressor;
	csb->bs.row = -1;
	sp->bit = 8;
	sp->data = 0;
	sp->tag = EncoderMode::G3_1D;

	// This is necessary for Group 4; otherwise it isn't needed because the
	// first scanline of each strip ends up being copied into the refline.

	if (sp->refline)
		memset(sp->refline, 0x00, sp->b.rowbytes);
	if (is2DEncoding(sp))
	{
		float res = csb->bs.yresolution;

		// The CCITT spec says that when doing 2d encoding, you should only do
		// it on K consecutive scanlines, where K depends on the resolution of
		// the image being encoded (2 for <= 200 lpi, 4 for > 200 lpi).  Since
		// the directory code initializes td_yresolution to 0, this code will
		// select a K of 2 unless the YResolution tag is set appropriately.
		// (Note also that we fudge a little here and use 150 lpi to avoid
		// problems with units conversion.)

		if (csb->bs.resolutionunit == TIFF_ResolutionUnit::CENTIMETER)
			res *= 2.54f;		/* convert to inches */
		sp->maxk = (res > 150 ? 4 : 2);
		sp->k = sp->maxk - 1;
	}
	else
		sp->k = sp->maxk = 0;
	return 1;
}

static const uchar zeroruns[256] =
    {
        8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,	/* 0x00 - 0x0f */
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,	/* 0x10 - 0x1f */
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,	/* 0x20 - 0x2f */
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,	/* 0x30 - 0x3f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x40 - 0x4f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x50 - 0x5f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x60 - 0x6f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x70 - 0x7f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x80 - 0x8f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x90 - 0x9f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xa0 - 0xaf */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xb0 - 0xbf */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xc0 - 0xcf */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xd0 - 0xdf */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xe0 - 0xef */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xf0 - 0xff */
    };
static const uchar oneruns[256] =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x00 - 0x0f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x10 - 0x1f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x20 - 0x2f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x30 - 0x3f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x40 - 0x4f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x50 - 0x5f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x60 - 0x6f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x70 - 0x7f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x80 - 0x8f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x90 - 0x9f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0xa0 - 0xaf */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0xb0 - 0xbf */
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,	/* 0xc0 - 0xcf */
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,	/* 0xd0 - 0xdf */
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,	/* 0xe0 - 0xef */
        4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 7, 8,	/* 0xf0 - 0xff */
    };

/*
 * On certain systems it pays to inline
 * the routines that find pixel spans.
 */
#ifdef VAXC
static int32 find0span(uchar *, int32, int32);
static int32 find1span(uchar *, int32, int32);

#pragma inline(find0span,find1span)
#endif /* VAXC */

/*
 * Find a span of ones or zeros using the supplied
 * table.  The "base" of the bit string is supplied
 * along with the start+end bit indices.
 */

static int32 find0span(uchar *bp, int32 bs, int32 be)
{
	int32 bits = be - bs;
	int32 n;
	int32 span=0;

	bp += bs >> 3;

	// Check partial byte on lhs.

	if (bits > 0 )
	{
		n = (bs & 7);
		if (n)
		{
			span = zeroruns[(*bp << n) & 0xff];
			if (span > 8 - n)		// table value too generous
				span = 8 - n;
			if (span > bits)		// constrain span to bit range
				span = bits;
			if (n + span < 8)		// doesn't extend to edge of byte
				return (span);
			bits -= span;
			bp++;
		}
	}
	if (bits >= 2 * 8 * sizeof(long))
	{
		long *lp;

		// Align to longword boundary and check longwords.

		while (!isAligned(bp, long))
		{
			if (*bp != 0x00)
				return (span + zeroruns[*bp]);
			span += 8, bits -= 8;
			bp++;
		}
		lp = (long *) bp;
		while (bits >= 8 * sizeof(long) && *lp == 0)
		{
			span += 8 * sizeof(long), bits -= 8 * sizeof(long);
			lp++;
		}
		bp = (uchar *) lp;
	}

	// Scan full bytes for all 0's.

	while (bits >= 8)
	{
		if (*bp != 0x00)		// end of run
			return (span + zeroruns[*bp]);
		span += 8, bits -= 8;
		bp++;
	}

	// Check partial byte on rhs.

	if (bits > 0)
	{
		n = zeroruns[*bp];
		span += (n > bits ? bits : n);
	}
	return (span);
}

static int32 find1span(uchar *bp, int32 bs, int32 be)
{
	int32 bits = be - bs;
	int32 n;
	int32 span=0;

	bp += bs >> 3;

	// Check partial byte on lhs.

	if (bits > 0)
	{
		n = (bs & 7);
		if (n)
		{
			span = oneruns[(*bp << n) & 0xff];
			if (span > 8 - n)		// table value too generous
				span = 8 - n;
			if (span > bits)		// constrain span to bit range
				span = bits;
			if (n + span < 8)		// doesn't extend to edge of byte
				return (span);
			bits -= span;
			bp++;
		}
	}
	if (bits >= 2 * 8 * sizeof(long))
	{
		long *lp;

		// Align to longword boundary and check longwords.

		while (!isAligned(bp, long))
		{
			if (*bp != 0xff)
				return (span + oneruns[*bp]);
			span += 8, bits -= 8;
			bp++;
		}
		lp = (long *) bp;
		while (bits >= 8 * sizeof(long) && *lp == ~0)
		{
			span += 8 * sizeof(long), bits -= 8 * sizeof(long);
			lp++;
		}
		bp = (uchar *) lp;
	}

	// Scan full bytes for all 1's.

	while (bits >= 8)
	{
		if (*bp != 0xff)		// end of run
			return (span + oneruns[*bp]);
		span += 8, bits -= 8;
		bp++;
	}

	// Check partial byte on rhs.

	if (bits > 0)
	{
		n = oneruns[*bp];
		span += (n > bits ? bits : n);
	}
	return span;
}

// Return the offset of the next bit in the range
// [bs..be] that is different from the specified
// color.  The end, be, is returned if no such bit
// exists.

#define	finddiff(_cp, _bs, _be, _color)	\
	(_bs + (_color ? find1span(_cp,_bs,_be) : find0span(_cp,_bs,_be)))

// Like finddiff, but also check the starting bit
// against the end in case start > end.

#define	finddiff2(_cp, _bs, _be, _color) \
	(_bs < _be ? finddiff(_cp,_bs,_be,_color) : _be)

//ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//³
//³ Procedure:      Fax3Encode1DRow()
//³
//³ Purpose:		1D-encode a row of pixels.
//³
//³ Inputs:         Codec state block,
//³					(Pointer to) input bit stream
//³					Number of bits.
//³
//³ Outputs:        Encoded bit stream in CSB
//³
//³ Returns:        Nothing
//³
//³ Side-effects:   None
//³
//³ Notes:			The encoding is a sequence of all-white or all-black
//³					spans of pixels encoded with Huffman codes.
//³
//ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
static const tableentry horizcode =
    {
        3, 0x1
    }
    ;						/* 001 */
static const tableentry passcode =
    {
        4, 0x1
    }
    ;						/* 0001 */
static const tableentry vcodes[7] =
    {
        {
            7, 0x03
        }
        ,					/* 0000 011 */
        {6, 0x03},					/* 0000 11 */
        {3, 0x03},					/* 011 */
        {1, 0x1},					/* 1 */
        {3, 0x2},					/* 010 */
        {6, 0x02},					/* 0000 10 */
        {7, 0x02}					/* 0000 010 */
    };

//ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//³
//³ Procedure:      Fax3Encode2DRow
//³
//³ Purpose:		2D-encode a row of pixels.
//³
//³ Inputs:         None
//³
//³ Outputs:        None
//³
//³ Returns:        Nothing
//³
//³ Side-effects:   None
//³
//³ Notes:			For algorithm, see CCITT documentation.
//³
//³
//ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

static int Fax3Encode2DRow(CSB *tif, uchar *bp, uchar *rp, uint32 bits)
{
#define	PIXEL(buf,ix)	((((buf)[(ix)>>3]) >> (7-((ix)&7))) & 1)
	int32 a0 = 0;
	int32 a1 = (PIXEL(bp, 0) != 0 ? 0 : finddiff(bp, 0, bits, 0));
	int32 b1 = (PIXEL(rp, 0) != 0 ? 0 : finddiff(rp, 0, bits, 0));
	int32 a2, b2;

	for (;;)
	{
		b2 = finddiff2(rp, (uint32) b1, bits, PIXEL(rp, b1));
		if (b2 >= a1)
		{
			int32 d = b1 - a1;

			if (!(-3 <= d && d <= 3))
			{					/* horizontal mode */
				a2 = finddiff2(bp, (uint32) a1, bits, PIXEL(bp, a1));
				putcode(tif, &horizcode);
				if (a0 + a1 == 0 || PIXEL(bp, a0) == 0)
				{
					putspan(tif, a1 - a0, TIFFFaxWhiteCodes);
					putspan(tif, a2 - a1, TIFFFaxBlackCodes);
				}
				else
				{
					putspan(tif, a1 - a0, TIFFFaxBlackCodes);
					putspan(tif, a2 - a1, TIFFFaxWhiteCodes);
				}
				a0 = a2;
			}
			else
			{					/* vertical mode */
				putcode(tif, &vcodes[d + 3]);
				a0 = a1;
			}
		}
		else
		{						/* pass mode */
			putcode(tif, &passcode);
			a0 = b2;
		}
		if ( (uint32) a0 >= bits)
			break;
		a1 = finddiff(bp, a0, bits, PIXEL(bp, a0));
		b1 = finddiff(rp, a0, bits, !PIXEL(bp, a0));
		b1 = finddiff(rp, b1, bits, PIXEL(bp, a0));
	}
	return (1);
#undef PIXEL
}

//ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//³
//³ Procedure:      Fax3Encode()
//³
//³ Purpose:		Encode an entire buffer of pixels.
//³
//³ Inputs:
//³
//³ Outputs:
//³
//³ Returns:
//³
//³ Side-effects:
//³
//³ Notes:
//³
//³
//ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
/*
 * CCITT Group 4 (T.6) Facsimile-compatible
 * Compression Scheme Support.
 */

#define	SWAP(t,a,b)	{ t x; x = (a); (a) = (b); (b) = x; }
/*
 * Decode the requested amount of G4-encoded data.
 */

int Fax4Decode(CSB *csb, uchar *buf, tsize_t occ) //, tsample_t s
{
	DECLARE_STATE_2D(csb, sp, "Fax4Decode");

	//	(void) s;
	CACHE_STATE(sp);
	sp->fill = _TIFFFax3fillruns;
	while ((long) occ > 0)
	{
		a0 = 0;
		RunLength = 0;
		pa = thisrun = sp->curruns;
		pb = sp->refruns;
		b1 = *pb++;
#ifdef FAX3_DEBUG

		printf("\nBitAcc=%08X, BitsAvail = %d\n", BitAcc, BitsAvail);
		printf("-------------------- %d\n", csb->bs.row);
		fflush(stdout);
#endif

		EXPAND2D(EOFG4);
		(*sp->fill) (buf, thisrun, pa, lastx);
		SETVAL(0);				/* imaginary change for reference */
		SWAP(uint16 *, sp->curruns, sp->refruns);
		buf += sp->b.rowbytes;
		occ -= sp->b.rowbytes;
		if (occ != 0)
			csb->bs.row++;
		continue;
EOFG4:
		(*sp->fill) (buf, thisrun, pa, lastx);
		UNCACHE_STATE(sp);
		return -1;
	}
	UNCACHE_STATE(sp);
	return 1;
}

#undef	SWAP

/*
 * Encode the requested amount of data.
 */

int Fax4Encode(CSB *csb, uchar *bp, tsize_t cc) //, tsample_t s)
{
	Fax3EncodeState *sp = EncoderState(csb);

	//	(void) s;

	Fax3PreEncode(csb); //, s);

	while ((long) cc > 0)
	{
		if (!Fax3Encode2DRow(csb, bp, sp->refline, sp->b.rowpixels))
			return 0;
		memcpy(sp->refline, bp, sp->b.rowbytes);
		bp += sp->b.rowbytes;
		cc -= sp->b.rowbytes;
		if (cc != 0)
			csb->bs.row++;
	}
	return Fax4PostEncode(csb);
}

static int Fax4PostEncode(CSB *csb)
{
	Fax3EncodeState *sp = EncoderState(csb);

	/* terminate strip w/ EOFB */
	Fax3PutBits(csb, EOL, 12);
	Fax3PutBits(csb, EOL, 12);
	if (sp->bit != 8)
		Fax3FlushBits(csb, sp);
	return 1;
}



