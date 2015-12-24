/* ==============================================================================
   ST3800 Wedge interfaces keyboard style CAPSLOCK support module source
   ==============================================================================

   $RCSfile: interfaces_wedge/wedgecapslock.cpp $
   $Revision: 1.4 $
   $Date: 2009/08/05 02:52:27EDT $

	==============================================================================

	============================================================================== */

#include "stdInclude.h"
#include <stdlib.h>
#include <string.h>
#include "CommonDriver.h"
#include "wdgtype.h"
#include "WedgeBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
We do all the ctype stuff local here since we can not (yet) relay on the compilers CRT !
Most compilers support ASCII only by default. For full ANSI characters you must reconfigure
the library. (So does the IAR compiler).
*/


/* set bit masks for the possible character types */
#ifndef WIN32
#define _UPPER          0x1     /* upper case letter */
#define _LOWER          0x2     /* lower case letter */
#define _DIGIT          0x4     /* digit[0-9] */
#define _SPACE          0x8     /* tab, carriage return, newline, */
                                /* vertical tab or form feed */
#define _PUNCT          0x10    /* punctuation character */
#define _CONTROL        0x20    /* control character */
#define _BLANK          0x40    /* space char */
#define _HEX            0x80    /* hexadecimal digit */
#define _ALPHA          (_UPPER|_LOWER)  /* alphabetic character */
#endif

static const unsigned char _ctype_x[256] =
{
        _CONTROL,               /* 00 (NUL) */
        _CONTROL,               /* 01 (SOH) */
        _CONTROL,               /* 02 (STX) */
        _CONTROL,               /* 03 (ETX) */
        _CONTROL,               /* 04 (EOT) */
        _CONTROL,               /* 05 (ENQ) */
        _CONTROL,               /* 06 (ACK) */
        _CONTROL,               /* 07 (BEL) */
        _CONTROL,               /* 08 (BS)  */
        _SPACE+_CONTROL,        /* 09 (HT)  */
        _SPACE+_CONTROL,        /* 0A (LF)  */
        _SPACE+_CONTROL,        /* 0B (VT)  */
        _SPACE+_CONTROL,        /* 0C (FF)  */
        _SPACE+_CONTROL,        /* 0D (CR)  */
        _CONTROL,               /* 0E (SI)  */
        _CONTROL,               /* 0F (SO)  */
        _CONTROL,               /* 10 (DLE) */
        _CONTROL,               /* 11 (DC1) */
        _CONTROL,               /* 12 (DC2) */
        _CONTROL,               /* 13 (DC3) */
        _CONTROL,               /* 14 (DC4) */
        _CONTROL,               /* 15 (NAK) */
        _CONTROL,               /* 16 (SYN) */
        _CONTROL,               /* 17 (ETB) */
        _CONTROL,               /* 18 (CAN) */
        _CONTROL,               /* 19 (EM)  */
        _CONTROL,               /* 1A (SUB) */
        _CONTROL,               /* 1B (ESC) */
        _CONTROL,               /* 1C (FS)  */
        _CONTROL,               /* 1D (GS)  */
        _CONTROL,               /* 1E (RS)  */
        _CONTROL,               /* 1F (US)  */
        _SPACE+_BLANK,          /* 20 SPACE */
        _PUNCT,                 /* 21 !     */
        _PUNCT,                 /* 22 "     */
        _PUNCT,                 /* 23 #     */
        _PUNCT,                 /* 24 $     */
        _PUNCT,                 /* 25 %     */
        _PUNCT,                 /* 26 &     */
        _PUNCT,                 /* 27 '     */
        _PUNCT,                 /* 28 (     */
        _PUNCT,                 /* 29 )     */
        _PUNCT,                 /* 2A *     */
        _PUNCT,                 /* 2B +     */
        _PUNCT,                 /* 2C ,     */
        _PUNCT,                 /* 2D -     */
        _PUNCT,                 /* 2E .     */
        _PUNCT,                 /* 2F /     */
        _DIGIT+_HEX,            /* 30 0     */
        _DIGIT+_HEX,            /* 31 1     */
        _DIGIT+_HEX,            /* 32 2     */
        _DIGIT+_HEX,            /* 33 3     */
        _DIGIT+_HEX,            /* 34 4     */
        _DIGIT+_HEX,            /* 35 5     */
        _DIGIT+_HEX,            /* 36 6     */
        _DIGIT+_HEX,            /* 37 7     */
        _DIGIT+_HEX,            /* 38 8     */
        _DIGIT+_HEX,            /* 39 9     */
        _PUNCT,                 /* 3A :     */
        _PUNCT,                 /* 3B ;     */
        _PUNCT,                 /* 3C <     */
        _PUNCT,                 /* 3D =     */
        _PUNCT,                 /* 3E >     */
        _PUNCT,                 /* 3F ?     */
        _PUNCT,                 /* 40 @     */
        _UPPER+_HEX,            /* 41 A     */
        _UPPER+_HEX,            /* 42 B     */
        _UPPER+_HEX,            /* 43 C     */
        _UPPER+_HEX,            /* 44 D     */
        _UPPER+_HEX,            /* 45 E     */
        _UPPER+_HEX,            /* 46 F     */
        _UPPER,                 /* 47 G     */
        _UPPER,                 /* 48 H     */
        _UPPER,                 /* 49 I     */
        _UPPER,                 /* 4A J     */
        _UPPER,                 /* 4B K     */
        _UPPER,                 /* 4C L     */
        _UPPER,                 /* 4D M     */
        _UPPER,                 /* 4E N     */
        _UPPER,                 /* 4F O     */
        _UPPER,                 /* 50 P     */
        _UPPER,                 /* 51 Q     */
        _UPPER,                 /* 52 R     */
        _UPPER,                 /* 53 S     */
        _UPPER,                 /* 54 T     */
        _UPPER,                 /* 55 U     */
        _UPPER,                 /* 56 V     */
        _UPPER,                 /* 57 W     */
        _UPPER,                 /* 58 X     */
        _UPPER,                 /* 59 Y     */
        _UPPER,                 /* 5A Z     */
        _PUNCT,                 /* 5B [     */
        _PUNCT,                 /* 5C \     */
        _PUNCT,                 /* 5D ]     */
        _PUNCT,                 /* 5E ^     */
        _PUNCT,                 /* 5F _     */
        _PUNCT,                 /* 60 `     */
        _LOWER+_HEX,            /* 61 a     */
        _LOWER+_HEX,            /* 62 b     */
        _LOWER+_HEX,            /* 63 c     */
        _LOWER+_HEX,            /* 64 d     */
        _LOWER+_HEX,            /* 65 e     */
        _LOWER+_HEX,            /* 66 f     */
        _LOWER,                 /* 67 g     */
        _LOWER,                 /* 68 h     */
        _LOWER,                 /* 69 i     */
        _LOWER,                 /* 6A j     */
        _LOWER,                 /* 6B k     */
        _LOWER,                 /* 6C l     */
        _LOWER,                 /* 6D m     */
        _LOWER,                 /* 6E n     */
        _LOWER,                 /* 6F o     */
        _LOWER,                 /* 70 p     */
        _LOWER,                 /* 71 q     */
        _LOWER,                 /* 72 r     */
        _LOWER,                 /* 73 s     */
        _LOWER,                 /* 74 t     */
        _LOWER,                 /* 75 u     */
        _LOWER,                 /* 76 v     */
        _LOWER,                 /* 77 w     */
        _LOWER,                 /* 78 x     */
        _LOWER,                 /* 79 y     */
        _LOWER,                 /* 7A z     */
        _PUNCT,                 /* 7B {     */
        _PUNCT,                 /* 7C |     */
        _PUNCT,                 /* 7D }     */
        _PUNCT,                 /* 7E ~     */
        _CONTROL,               /* 7F (DEL) */
        _PUNCT,					  /* 80 EURO  */
        _PUNCT,                 /* 81 Å     */
        _PUNCT,                 /* 82 Ç     */
        _PUNCT,                 /* 83 É     */
        _PUNCT,                 /* 84 Ñ     */
        _PUNCT,                 /* 85 Ö     */
        _PUNCT,                 /* 86 Ü     */
        _PUNCT,                 /* 87 á     */
        _PUNCT,                 /* 88 à     */
        _PUNCT,			        /* 89 â     */
        _UPPER,        		     /* 8A ä     */
        _PUNCT,			        /* 8B ã     */
        _PUNCT,			        /* 8C å     */
        _PUNCT,			        /* 8D ç     */
        _PUNCT,                 /* 8E é     */
        _PUNCT,                 /* 8F è     */
        _PUNCT,                 /* 90 ê     */
        _PUNCT,                 /* 91 ë     */
        _PUNCT,                 /* 92 í     */
        _PUNCT,                 /* 93 ì     */
        _PUNCT,                 /* 94 î     */
        _PUNCT,                 /* 95 ï     */
        _PUNCT,                 /* 96 ñ     */
        _PUNCT,                 /* 97 ó     */
        _PUNCT,                 /* 98 ò     */
        _PUNCT,                 /* 99 ô     */
        _LOWER,                 /* 9A ö     */
        _PUNCT,                 /* 9B õ     */
        _PUNCT,                 /* 9C ú     */
        _PUNCT,                 /* 9D ù     */
        _PUNCT,                 /* 9E û     */
        _UPPER,                 /* 9F ü     */
        _SPACE,		           /* A0 NBSP  */
        _PUNCT,                 /* A1 °     */
        _PUNCT,                 /* A2 ¢     */
        _PUNCT,                 /* A3 £     */
        _PUNCT,                 /* A4 §     */
        _PUNCT,                 /* A5 •     */
        _PUNCT,                 /* A6 ¶     */
        _PUNCT,                 /* A7 ß     */
        _PUNCT,                 /* A8 ®     */
        _PUNCT,                 /* A9 ©     */
        _PUNCT,                 /* AA ™     */
        _PUNCT,                 /* AB ´     */
        _PUNCT,                 /* AC ¨     */
        _PUNCT,                 /* AD ≠     */
        _PUNCT,                 /* AE Æ     */
        _PUNCT,                 /* AF Ø     */
        _PUNCT,            	  /* B0 ∞     */
        _PUNCT,            	  /* B1 ±     */
        _PUNCT,            	  /* B2 ≤     */
        _PUNCT,            	  /* B3 ≥     */
        _PUNCT,            	  /* B4 ¥     */
        _PUNCT,            	  /* B5 µ     */
        _PUNCT,            	  /* B6 ∂     */
        _PUNCT,            	  /* B7 ∑     */
        _PUNCT,            	  /* B8 ∏     */
        _PUNCT,            	  /* B9 π     */
        _PUNCT,                 /* BA ∫     */
        _PUNCT,                 /* BB ª     */
        _PUNCT,                 /* BC º     */
        _PUNCT,                 /* BD Ω     */
        _PUNCT,                 /* BE æ     */
        _PUNCT,                 /* BF ø     */
        _PUNCT,                 /* C0 ¿     */
        _UPPER,	              /* C1 ¡     */
        _UPPER,	              /* C2 ¬     */
        _UPPER,	              /* C3 √     */
        _UPPER,   	           /* C4 ƒ     */
        _UPPER,	              /* C5 ≈     */
        _UPPER,	              /* C6 ∆     */
        _UPPER,                 /* C7 «     */
        _UPPER,                 /* C8 »     */
        _UPPER,                 /* C9 …     */
        _UPPER,                 /* CA       */
        _UPPER,                 /* CB À     */
        _UPPER,                 /* CC Ã     */
        _UPPER,                 /* CD Õ     */
        _UPPER,                 /* CE Œ     */
        _UPPER,                 /* CF œ     */
        _UPPER,                 /* D0 –     */
        _UPPER,                 /* D1 —     */
        _UPPER,                 /* D2 “     */
        _UPPER,                 /* D3 ”     */
        _UPPER,                 /* D4 ‘     */
        _UPPER,                 /* D5 ’     */
        _UPPER,                 /* D6 ÷     */
        _PUNCT,                 /* D7 ◊     */
        _UPPER,                 /* D8 ÿ     */
        _UPPER,                 /* D9 Ÿ     */
        _UPPER,                 /* DA ⁄     */
        _UPPER,                 /* DB €     */
        _UPPER,                 /* DC ‹     */
        _UPPER,                 /* DD ›     */
        _UPPER,                 /* DE ﬁ     */
        _PUNCT,                 /* DF ﬂ     */
        _LOWER,                 /* E0 ‡     */
        _LOWER,	              /* E1 ·     */
        _LOWER,	              /* E2 ‚     */
        _LOWER,	              /* E3 „     */
        _LOWER,	              /* E4 ‰     */
        _LOWER,	              /* E5 Â     */
        _LOWER,	              /* E6 Ê     */
        _LOWER,                 /* E7 Á     */
        _LOWER,                 /* E8 Ë     */
        _LOWER,                 /* E9 È     */
        _LOWER,                 /* EA Í     */
        _LOWER,                 /* EB Î     */
        _LOWER,                 /* EC Ï     */
        _LOWER,                 /* ED Ì     */
        _LOWER,                 /* EE Ó     */
        _LOWER,                 /* EF Ô     */
        _LOWER,                 /* F0      */
        _LOWER,                 /* F1 Ò     */
        _LOWER,                 /* F2 Ú     */
        _LOWER,                 /* F3 Û     */
        _LOWER,                 /* F4 Ù     */
        _LOWER,                 /* F5 ı     */
        _LOWER,                 /* F6 ˆ     */
        _PUNCT,                 /* F7 ˜     */
        _LOWER,                 /* F8 ¯     */
        _LOWER,                 /* F9 ˘     */
        _LOWER,                 /* FA ˙     */
        _LOWER,                 /* FB ˚     */
        _LOWER,                 /* FC ¸     */
        _LOWER,                 /* FD ˝     */
        _LOWER,                 /* FE ˛     */
        _LOWER,                 /* FF ˇ     */
};

#define isalpha_x(_c)     ( _ctype_x[_c] & (_UPPER|_LOWER) )
#define isupper_x(_c)     ( _ctype_x[_c] & _UPPER )
#define islower_x(_c)     ( _ctype_x[_c] & _LOWER )
#define isdigit_x(_c)     ( _ctype_x[_c] & _DIGIT )
#define isxdigit_x(_c)    ( _ctype_x[_c] & _HEX )
#define isspace_x(_c)     ( _ctype_x[_c] & _SPACE )
#define ispunct_x(_c)     ( _ctype_x[_c] & _PUNCT )
#define isalnum_x(_c)     ( _ctype_x[_c] & (_UPPER|_LOWER|_DIGIT) )
#define isprint_x(_c)     ( _ctype_x[_c] & (_BLANK|_PUNCT|_UPPER|_LOWER|_DIGIT) )
#define isgraph_x(_c)     ( _ctype_x[_c] & (_PUNCT|_UPPER|_LOWER|_DIGIT) )
#define iscntrl_x(_c)     ( _ctype_x[_c] & _CONTROL )


void CWedgeBase::HandleCapsLock(void)
{
	int	ansi;
	CKey 	*key;

	if (IsKbdStyleCapsLock())
	{
		for(ansi=0; ansi<=255; ansi++)
		{
			if (isalpha_x(ansi))
			{
                key = m_KeyNumLookup.GetKey((tichar_t)ansi);
				if (key->m_keynum != 0)
				{
					if (isupper_x(ansi))
					{
						key->m_modifier &= ~MO_ShiftLeft;
					}
					if (islower_x(ansi))
					{
						key->m_modifier |= MO_ShiftLeft;
					}
                    m_KeyNumLookup.StoreKey((tichar_t)ansi, key->m_keynum, key->m_modifier);
				}
			}
		}
	}
}

void CWedgeBase::HandleKbdMode(void)
{
			int	ansi;
			CKey 	*key;

			if (GetMode()==1)
			{
			      for(ansi=0; ansi<=255; ansi++)
			     {
				if (isalpha_x(ansi))
				{
              			     key = m_KeyNumLookup.GetKey((tichar_t)ansi);
				       if (key->m_keynum != 0)
				      {
					if (islower_x(ansi))
					{
					key->m_modifier |= MO_ShiftLeft;
				                  }
                   		 m_KeyNumLookup.StoreKey((tichar_t)ansi, key->m_keynum, key->m_modifier);
				       }
			                  }
		                      }
	             		 }
			else if (GetMode()==2)
			{
			      for(ansi=0; ansi<=255; ansi++)
			     {
			          if (isalpha_x(ansi))
			         {
                                                       key = m_KeyNumLookup.GetKey((tichar_t)ansi);
				if (key->m_keynum != 0)
				{
				       if (isupper_x(ansi))
				          {
					key->m_modifier &= ~MO_ShiftLeft;
				          }
                                                   m_KeyNumLookup.StoreKey((tichar_t)ansi, key->m_keynum, key->m_modifier);
				}
			           }
		                        }
	                                     }
                                     }



