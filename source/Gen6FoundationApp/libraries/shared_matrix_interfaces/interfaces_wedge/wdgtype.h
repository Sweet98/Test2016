/* ==============================================================================
   ST3800 Wedge interfaces character to scancode translation module type defs
   ==============================================================================

   $RCSfile: interfaces_wedge/wdgtype.h $
   $Revision: 1.5 $
   $Date: 2009/06/29 05:03:03EDT $

	==============================================================================

	============================================================================== */


#ifndef WDG_TYPE_FH_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define WDG_TYPE_FH_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#define NUMKEYS 150
#define NUMANSI 256

typedef unsigned char scancode_t;
typedef unsigned char keynum_t;
typedef unsigned char char_t;
typedef unsigned char mkbrtype_t;
typedef unsigned char modifiers_t;
typedef unsigned char attributes_t;


#ifndef _T
#define _T(text)	text
#endif

/*
 These are for the extrainfo to the bitbangers. Upper byte is flags, lower byte contains the keynumber if there is one.
*/
#define EX_KEYNUMBER		0x00ff			// mask for accessing the keynumber
#define EX_MAKE			0x8000
#define EX_BREAK			0x4000
#define EX_MODIFIER		0x2000
#define EX_PRE_BREAK		0x1000
#define EX_EXTENDED		0x0800
#define EX_SEQUENCE		0x0400
#define EX_OTHER			0x0200
//#define EX_SEND9BITS		0x0100			   // is is temporay until the command interface supports 32 bits
														// then we will transfer the number of bits
#define EX_NO				0

// bit above the 16 bit range can not be send with the command interface yet
#define EX_KEYEMU			0x10000		   // we are in keyboard emulation mode
#define EX_CLOSE			0x20000		   // it is a clean up call after all characters have been send.
													// Can be used to issue a "All Keys Up" message



#endif
