/* ==============================================================================
   Prokey table Definition Header
   ==============================================================================

   $RCSfile: interfaces_wedge/prokeytb.h $
   $Revision: 2.1 $
   $Date: 2009/06/29 05:03:00EDT $

	==============================================================================

	============================================================================== */


#ifndef PROKEY_TABLE_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define PROKEY_TABLE_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

/* #define MAGICSIZE sizeof("ProkeyTableDMF") */
#define MAGICSIZE 15

#define PK_FormatOffs		MAGICSIZE+0
#define PK_RepStartOffs		MAGICSIZE+5
#define PK_SeqStartOffs		MAGICSIZE+3


#define UNIVERSAL_COUNTRY 99
#define UNIVERSAL_TERMINAL 99

const UCHAR *LoadProkeyFile(void);

#endif




