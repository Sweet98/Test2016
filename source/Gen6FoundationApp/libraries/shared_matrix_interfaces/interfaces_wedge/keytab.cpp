/* ==============================================================================
   All terminal property and ASCII to Key translation tables Source Module  (Hand made)
   ==============================================================================

   $RCSfile: interfaces_wedge/keytab.cpp $
   $Revision: 2.5 $
   $Date: 2009/06/29 05:02:52EDT $

   ==============================================================================

   ============================================================================== */


/* ==============================================================================
   Use the Windows ANSI code page (CP1252) to view this file!
 	Place all hand made key translation tables into this file.
   ============================================================================== */

#define KEYBOARD_TABLE_VERSION 100	/* This version gets changed when table structure changes */
/* Format version history: */
/* 5 Added capslock and numlock key to property table */
/* 4 Initial release */


#include "stdInclude.h"
/* This single include file allows for adding you special requirements */
#include "termtabl.h"

/* Here we test whether the format versions match */
#if (KEYBOARD_PRG_VERSION != KEYBOARD_TABLE_VERSION)
#error "Keyboard tables do have the wrong version"
#endif

/******************************************************/
/* Keynum to scancode translation tables              */
/******************************************************/

/*	CTOSUNISYS_KB100 * --------------------------------------------------*/
const CScancodes CTOSUNISYS_KB100_SCAN =
{
	/* no sequence info		*/	NULL,
	/* max used key		*/	143,
	/* extension			*/	0x00,
	/* illegal scancode	*/	0x00, 0,
	/*   1 					*/	0x00, 0,										/*'*/
	/*   2 					*/	0xFF, 0,										/*1*/
	/*   3 					*/	0xFB, 0,										/*2*/
	/*   4 					*/	0x8B, 0,										/*3*/
	/*   5 					*/	0x83, 0,										/*4*/
	/*   6 					*/	0xC2, 0,										/*5*/
	/*   7 					*/	0xD4, 0,	                        	/*6*/
	/*   8 					*/	0xBA, 0,	                        	/*7*/
	/*   9 					*/	0x94, 0,	                        	/*8*/
	/*  10 					*/	0xAE, 0,	                        	/*9*/
	/*  11 					*/	0xE0, 0,		                        /*0*/
	/*  12 					*/	0x00, 0,		                        /*-*/
	/*  13 					*/	0x00, 0,		                        /*=*/
	/*  14 Empty			*/	0x00, 0,
	/*  15 					*/	0x00, 0,		                        /*BS*/
	/*  16 					*/	0x00, 0,		                        /*tab*/
	/*  17 					*/	0x00, 0,		                        /*Q*/
	/*  18 					*/	0x00, 0,		                        /*W*/
	/*  19 					*/	0x00, 0,		                        /*E*/
	/*  20 					*/	0x00, 0,      	                     /*R*/
	/*  21 					*/	0x00, 0,		                        /*T*/
	/*  22 					*/	0x00, 0,										/*Y*/
	/*  23 					*/	0x00, 0,		                        /*U*/
	/*  24 					*/	0x00, 0,		                        /*I*/
	/*  25 					*/	0x00, 0,		                        /*O*/
	/*  26 					*/	0x00, 0,		                        /*P*/
	/*  27 					*/	0x00, 0,		                        /*[*/
	/*  28 					*/	0x00, 0,		                        /*]*/
	/*  29 					*/	0x00, 0,		                        /*\*/
	/*  30 					*/	0x00, 0,						        		/*CAPS*/
	/*  31 					*/	0x00, 0,						        		/*A*/
	/*  32 					*/	0x00, 0,						        		/*S*/
	/*  33 					*/	0x00, 0,						        		/*D*/
	/*  34 					*/	0x00, 0,						        		/*F*/
	/*  35 					*/	0x00, 0,						        		/*G*/
	/*  36 					*/	0x00, 0,						        		/*H*/
	/*  37 					*/	0x00, 0,						        		/*J*/
	/*  38 					*/	0x00, 0,						        		/*K*/
	/*  39 					*/	0x00, 0,						        		/*L*/
	/*  40 					*/	0x00, 0,						        		/*;*/
	/*  41 					*/	0x00, 0,						        		/*'*/
	/*  42 					*/	0x00, 0,
	/*  43 					*/	0xC8, 0,						        		/*ENTER*/
	/*  44 					*/	0x00, 0,               					//LEFT SHIFT
	/*  45 					*/	0x00, 0,
	/*  46 					*/	0x00, 0,                            /*Z*/
	/*  47 					*/	0x00, 0,						        		/*X*/
	/*  48 					*/	0x00, 0,						        		/*C*/
	/*  49 					*/	0x00, 0,                            /*V*/
	/*  50 					*/	0x00, 0,                            /*B*/
	/*  51 					*/	0x00, 0,                            /*N*/
	/*  52 					*/	0x00, 0,                            /*M*/
	/*  53 					*/	0x00, 0,                            /*,*/
	/*  54 					*/	0x00, 0,                            /*.*/
	/*  55 					*/	0x00, 0,                            /*'/'*/
	/*  56 					*/	0x00, 0,
	/*  57 					*/	0x00, 0,               					/*RIGHT SHIFT*/
	/*  58 					*/	0x00, 0,               					/*LFT CTRL*/
	/*  59 					*/	0x00, 0,
	/*  60 					*/	0x00, 0,                            /*LFT ALT*/
	/*  61 					*/	0x00, 0,                            /*SPACE*/
	/*  62 					*/	0x00, 0,                      		/*RT ALT*/
	/*  63 					*/	0x00, 0,
	/*  64 					*/	0x00, 0, 									/*RT CTRL*/
	/*  65 					*/	0x00, 0,
	/*  66 					*/	0x00, 0,
	/*  67 					*/	0x00, 0,
	/*  68 					*/	0x00, 0,
	/*  69 					*/	0x00, 0,
	/*  70 					*/	0x00, 0,
	/*  71 					*/	0x00, 0,
	/*  72 					*/	0x00, 0,
	/*  73 					*/	0x00, 0,
	/*  74 					*/	0x00, 0,
	/*  75 					*/	0xCA, 0,                      		/*INS*/
	/*  76 					*/	0x00, 0,										/*DEL*/
	/*  77 Empty			*/	0x00, 0,
	/*  78 					*/	0x00, 0,
	/*  79 					*/	0x9B, 0,                      		/*LFT ARROW*/
	/*  80 					*/	0x00, 0,                      		/*HOME*/
	/*  81 					*/	0x00, 0,                      		/*END*/
	/*  82 					*/	0x00, 0,
	/*  83 					*/	0xC6, 0,                      		/*UP ARROW*/
	/*  84 					*/	0xC3, 0,                      		/*DOWN ARROW*/
	/*  85 					*/	0x00, 0,                      		/*PGUP*/
	/*  86 					*/	0x00, 0,                      		/*PGDN*/
	/*  87 Empty			*/	0x00, 0,
	/*  88 					*/	0x00, 0,
	/*  89 					*/	0x8A, 0,                      		/*RT ARROW*/
	/*  90 					*/	0x00, 0,               					/*NUM LOCK*/
	/*  91 					*/	0x00, 0,                            /*NUMPAD 7*/
	/*  92 					*/	0x00, 0,                            /*NUMPAD 4*/
	/*  93 					*/	0x00, 0,                           	/*NUMPAD 1*/
	/*  94 					*/	0x00, 0,
	/*  95 					*/	0x00, 0,                      		/*NUMPAD /  */
	/*  96 					*/	0x00, 0,                            /*NUMPAD 8*/
	/*  97 					*/	0x00, 0,                            /*NUMPAD 5*/
	/*  98 					*/	0x00, 0,                            /*NUMPAD 2*/
	/*  99 					*/	0x00, 0,                            /*NUMPAD 0*/
	/* 100 					*/	0x00, 0,                            /*NUMPAD **/
	/* 101 					*/	0x00, 0,                            /*NUMPAD 9*/
	/* 102 					*/	0x00, 0,                            /*NUMPAD 6*/
	/* 103 					*/	0x00, 0,                            /*NUMPAD 3*/
	/* 104 					*/	0x00, 0,                            /*NUMPAD DEL*/
	/* 105 					*/	0x00, 0,                            /*NUMPAD -*/
	/* 106 					*/	0x00, 0,                            /*NUMPAD +*/
	/* 107 					*/	0x00, 0,
	/* 108 					*/	0x00, 0,                      		/*NUMPAD ENTER*/
	/* 109 					*/	0x00, 0,
	/* 110 					*/	0x00, 0,                            /*ESC*/
	/* 111 Empty			*/	0x00, 0,
	/* 112 					*/	0x00, 0,                            /*F1*/
	/* 113 					*/	0x00, 0,                            /*F2*/
	/* 114 					*/	0x00, 0,                            /*F3*/
	/* 115 					*/	0x00, 0,                            /*F4*/
	/* 116 					*/	0x00, 0,                            /*F5*/
	/* 117 					*/	0x00, 0,                            /*F6*/
	/* 118 					*/	0x00, 0,                            /*F7*/
	/* 119 					*/	0x00, 0,                            /*F8*/
	/* 120 					*/	0x00, 0,                            /*F9*/
	/* 121 					*/	0x00, 0,                            /*F10*/
	/* 122 					*/	0x00, 0,                            /*F11*/
	/* 123 					*/	0x00, 0,                            /*F12*/
	/* 124 Sequence		*/	0x00, 0,                          	/*PRNT SCRN*/
	/* 125 					*/	0x00, 0,                            /*SCROLL LOCK*/
	/* 126 Sequence		*/	0x00, 0,                          	/*BREAK*/
	/* 127 					*/	0x00, 0,
	/* 128 					*/	0x00, 0,
	/* 129 					*/	0x00, 0,
	/* 130 					*/	0x00, 0,
	/* 131 					*/	0x00, 0,
	/* 132 					*/	0x00, 0,
	/* 133 					*/	0x00, 0,
	/* 134 					*/	0x00, 0,
	/* 135 					*/	0x00, 0,
	/* 136 					*/	0x00, 0,
	/* 137 Empty			*/	0x00, 0,
	/* 138 Empty			*/	0x00, 0,
	/* 139 Empty			*/	0x00, 0,
	/* 140 					*/	0x00, 0,
	/* 141 					*/	0x00, 0,
	/* 142 					*/	0x00, 0,
	/* 143 					*/	0x00, 0
};

/*	CITOS-UNISYS K5 --------------------------------------------------*/
const CScancodes CTOSUNISYS_K5_SCAN =
{
	/* sequence info		*/	NULL,
	/* max used key		*/	143,
	/* extension			*/	0x00,
	/* illegal scancode	*/	0x00, KN_PARITYVARIATION,
	/*   1 					*/	0x00, KN_PARITYVARIATION,                            /*'*/
	/*   2 					*/	0xE0, KN_PARITYVARIATION,                            /*1*/
	/*   3 					*/	0xC6, KN_PARITYVARIATION,                            /*2*/
	/*   4 					*/	0xC7, KN_PARITYVARIATION,                            /*3*/
	/*   5 					*/	0xFB, KN_PARITYVARIATION,                            /*4*/
	/*   6 					*/	0xFD, KN_PARITYVARIATION,                            /*5*/
	/*   7 					*/	0xC1, KN_PARITYVARIATION,                            /*6*/
	/*   8 					*/	0xDC, KN_PARITYVARIATION,                            /*7*/
	/*   9 					*/	0xFC, KN_PARITYVARIATION,                            /*8*/
	/*  10 					*/	0xA1, KN_PARITYVARIATION,                            /*9*/
	/*  11 					*/	0xCA, KN_PARITYVARIATION,                            /*0*/
	/*  12 					*/	0x00, KN_PARITYVARIATION,                            /*-*/
	/*  13 					*/	0x00, KN_PARITYVARIATION,                            /*=*/
	/*  14 					*/	0x00, KN_PARITYVARIATION,
	/*  15 					*/	0xFF, KN_PARITYVARIATION,                            /*BS*/
	/*  16 					*/	0x00, KN_PARITYVARIATION,                            /*tab*/
	/*  17 					*/	0x00, KN_PARITYVARIATION,                            /*Q*/
	/*  18 					*/	0x00, KN_PARITYVARIATION,                            /*W*/
	/*  19 					*/	0x00, KN_PARITYVARIATION,                            /*E*/
	/*  20 					*/	0x00, KN_PARITYVARIATION,                            /*R*/
	/*  21 					*/	0x00, KN_PARITYVARIATION,                            /*T*/
	/*  22 					*/	0x00, KN_PARITYVARIATION,                            /*Y*/
	/*  23 					*/	0x00, KN_PARITYVARIATION,                            /*U*/
	/*  24 					*/	0x00, KN_PARITYVARIATION,                            /*I*/
	/*  25 					*/	0x00, KN_PARITYVARIATION,                            /*O*/
	/*  26 					*/	0x00, KN_PARITYVARIATION,                            /*P*/
	/*  27 					*/	0x00, KN_PARITYVARIATION,                            /*[*/
	/*  28 					*/	0x00, KN_PARITYVARIATION,                            /*]*/
	/*  29 					*/	0x00, KN_PARITYVARIATION,                            /*\*/
	/*  30 					*/	0x00, KN_PARITYVARIATION,                            /*CAPS*/
	/*  31 					*/	0x00, KN_PARITYVARIATION,                            /*A*/
	/*  32 					*/	0x00, KN_PARITYVARIATION,                            /*S*/
	/*  33 					*/	0x00, KN_PARITYVARIATION,                            /*D*/
	/*  34 					*/	0x00, KN_PARITYVARIATION,                            /*F*/
	/*  35 					*/	0x00, KN_PARITYVARIATION,                            /*G*/
	/*  36 					*/	0x00, KN_PARITYVARIATION,                            /*H*/
	/*  37 					*/	0x00, KN_PARITYVARIATION,                            /*J*/
	/*  38 					*/	0x00, KN_PARITYVARIATION,                            /*K*/
	/*  39 					*/	0x00, KN_PARITYVARIATION,                            /*L*/
	/*  40 					*/	0x00, KN_PARITYVARIATION,                            /*;*/
	/*  41 					*/	0x00, KN_PARITYVARIATION,                            /*'*/
	/*  42 					*/	0x00, KN_PARITYVARIATION,
	/*  43 					*/	0x9B, KN_PARITYVARIATION,                            /*ENTER*/
	/*  44 					*/	0x00, KN_PARITYVARIATION,                            /*LEFT SHIFT*/
	/*  45 					*/	0x00, KN_PARITYVARIATION,
	/*  46 					*/	0x00, KN_PARITYVARIATION,                            /*Z*/
	/*  47 					*/	0x00, KN_PARITYVARIATION,                            /*X*/
	/*  48 					*/	0x00, KN_PARITYVARIATION,                            /*C*/
	/*  49 					*/	0x00, KN_PARITYVARIATION,                            /*V*/
	/*  50 					*/	0x00, KN_PARITYVARIATION,                            /*B*/
	/*  51 					*/	0x00, KN_PARITYVARIATION,                            /*N*/
	/*  52 					*/	0x00, KN_PARITYVARIATION,                            /*M*/
	/*  53 					*/	0x00, KN_PARITYVARIATION,                            /*,*/
	/*  54 					*/	0x00, KN_PARITYVARIATION,                            /*.*/
	/*  55 					*/	0x00, KN_PARITYVARIATION,                            /*'/'*/
	/*  56 					*/	0x00, KN_PARITYVARIATION,
	/*  57 					*/	0x00, KN_PARITYVARIATION,                            /*RIGHT SHIFT*/
	/*  58 					*/	0x00, KN_PARITYVARIATION,                            /*LFT CTRL*/
	/*  59 					*/	0x00, KN_PARITYVARIATION,
	/*  60 					*/	0x00, KN_PARITYVARIATION,                            /*LFT ALT*/
	/*  61 					*/	0x00, KN_PARITYVARIATION,                            /*SPACE*/
	/*  62 					*/	0x00, KN_PARITYVARIATION,                            /*RT ALT*/
	/*  63 					*/	0x00, KN_PARITYVARIATION,
	/*  64 					*/	0x00, KN_PARITYVARIATION,                            /*RT CTRL*/
	/*  65 					*/	0x00, KN_PARITYVARIATION,
	/*  66 					*/	0x00, KN_PARITYVARIATION,
	/*  67 					*/	0x00, KN_PARITYVARIATION,
	/*  68 					*/	0x00, KN_PARITYVARIATION,
	/*  69 					*/	0x00, KN_PARITYVARIATION,
	/*  70 					*/	0x00, KN_PARITYVARIATION,
	/*  71 					*/	0x00, KN_PARITYVARIATION,
	/*  72 					*/	0x00, KN_PARITYVARIATION,
	/*  73 					*/	0x00, KN_PARITYVARIATION,
	/*  74 					*/	0x00, KN_PARITYVARIATION,
	/*  75 					*/	0x9B, KN_PARITYVARIATION,                            /*INS*/
	/*  76 					*/	0x00, KN_PARITYVARIATION,                            /*DEL*/
	/*  77 					*/	0x00, KN_PARITYVARIATION,
	/*  78 					*/	0x00, KN_PARITYVARIATION,
	/*  79 					*/	0x8E, KN_PARITYVARIATION,                            /*LFT ARROW*/
	/*  80 					*/	0x00, KN_PARITYVARIATION,                            /*HOME*/
	/*  81 					*/	0x00, KN_PARITYVARIATION,                            /*END*/
	/*  82 					*/	0x00, KN_PARITYVARIATION,
	/*  83 					*/	0x81, KN_PARITYVARIATION,                            /*UP ARROW*/
	/*  84 					*/	0x8D, KN_PARITYVARIATION,                            /*DOWN ARROW*/
	/*  85 					*/	0x00, KN_PARITYVARIATION,                            /*PGUP*/
	/*  86 					*/	0x00, KN_PARITYVARIATION,                            /*PGDN*/
	/*  87 					*/	0x00, KN_PARITYVARIATION,
	/*  88 					*/	0x00, KN_PARITYVARIATION,
	/*  89 					*/	0x8F, KN_PARITYVARIATION,                            /*RT ARROW*/
	/*  90 					*/	0x00, KN_PARITYVARIATION,                            /*NUM LOCK*/
	/*  91 					*/	0x00, KN_PARITYVARIATION,                            /*NUMPAD 7*/
	/*  92 					*/	0x00, KN_PARITYVARIATION,                            /*NUMPAD 4*/
	/*  93 					*/	0x00, KN_PARITYVARIATION,                            /*NUMPAD 1*/
	/*  94 					*/	0x00, KN_PARITYVARIATION,
	/*  95 					*/	0x00, KN_PARITYVARIATION,                            /*NUMPAD /  */
	/*  96 					*/	0x00, KN_PARITYVARIATION,                            /*NUMPAD 8*/
	/*  97 					*/	0x00, KN_PARITYVARIATION,                            /*NUMPAD 5*/
	/*  98 					*/	0x00, KN_PARITYVARIATION,                            /*NUMPAD 2*/
	/*  99 					*/	0x00, KN_PARITYVARIATION,                            /*NUMPAD 0*/
	/* 100 					*/	0x00, KN_PARITYVARIATION,                            /*NUMPAD **/
	/* 101 					*/	0x00, KN_PARITYVARIATION,                            /*NUMPAD 9*/
	/* 102 					*/	0x00, KN_PARITYVARIATION,                            /*NUMPAD 6*/
	/* 103 					*/	0x00, KN_PARITYVARIATION,                            /*NUMPAD 3*/
	/* 104 					*/	0x00, KN_PARITYVARIATION,                            /*NUMPAD DEL*/
	/* 105 					*/	0x00, KN_PARITYVARIATION,                            /*NUMPAD -*/
	/* 106 					*/	0x00, KN_PARITYVARIATION,                            /*NUMPAD +*/
	/* 107 					*/	0x00, KN_PARITYVARIATION,
	/* 108 					*/	0x00, KN_PARITYVARIATION,                            /*NUMPAD ENTER*/
	/* 109 					*/	0x00, KN_PARITYVARIATION,
	/* 110 					*/	0x00, KN_PARITYVARIATION,                            /*ESC*/
	/* 111 					*/	0x00, KN_PARITYVARIATION,
	/* 112 					*/	0x95, KN_PARITYVARIATION,                            /*F1*/
	/* 113 					*/	0x96, KN_PARITYVARIATION,                            /*F2*/
	/* 114 					*/	0x97, KN_PARITYVARIATION,                            /*F3*/
	/* 115 					*/	0x98, KN_PARITYVARIATION,                            /*F4*/
	/* 116 					*/	0x99, KN_PARITYVARIATION,                            /*F5*/
	/* 117 					*/	0x9A, KN_PARITYVARIATION,                            /*F6*/
	/* 118 					*/	0x9C, KN_PARITYVARIATION,                            /*F7*/
	/* 119 					*/	0x9D, KN_PARITYVARIATION,                            /*F8*/
	/* 120 					*/	0x9E, KN_PARITYVARIATION,                            /*F9*/
	/* 121 					*/	0x9F, KN_PARITYVARIATION,                            /*F10*/
	/* 122 					*/	0x00, KN_PARITYVARIATION,                            /*F11*/
	/* 123 					*/	0x00, KN_PARITYVARIATION,                            /*F12*/
	/* 124 		  			*/	0x00, KN_PARITYVARIATION,                            /*PRNT SCRN*/
	/* 125 					*/	0x00, KN_PARITYVARIATION,                            /*SCROLL LOCK*/
	/* 126 					*/	0x00, KN_PARITYVARIATION,                            /*BREAK*/
	/* 127 					*/	0x00, KN_PARITYVARIATION,
	/* 128 					*/	0x00, KN_PARITYVARIATION,
	/* 129 					*/	0x00, KN_PARITYVARIATION,
	/* 130 					*/	0x00, KN_PARITYVARIATION,
	/* 131 					*/	0x00, KN_PARITYVARIATION,
	/* 132 					*/	0x00, KN_PARITYVARIATION,
	/* 133 					*/	0x00, KN_PARITYVARIATION,
	/* 134 					*/	0x00, KN_PARITYVARIATION,
	/* 135 					*/	0x00, KN_PARITYVARIATION,
	/* 136 					*/	0x00, KN_PARITYVARIATION,
	/* 137		 			*/	0x00, KN_PARITYVARIATION,
	/* 138 	 				*/	0x00, KN_PARITYVARIATION,
	/* 139 	  				*/	0x00, KN_PARITYVARIATION,
	/* 140 					*/	0x00, KN_PARITYVARIATION,
	/* 141 					*/	0x00, KN_PARITYVARIATION,
	/* 142 					*/	0x00, KN_PARITYVARIATION,
	/* 143 					*/	0x00, KN_PARITYVARIATION
};

/*	CITOS-UNISYS K4 --------------------------------------------------*/
const CScancodes CTOSUNISYS_K4_SCAN =
{
	/* sequence info		*/	NULL,
	/* max used key		*/	143,
	/* extension			*/	0x00,
	/* illegal scancode	*/	0x00, 0,
	/*   1 					*/	0x00, 0,                            /*'*/
	/*   2 					*/	0xE0, 0,                            /*1*/
	/*   3 					*/	0xC6, 0,                            /*2*/
	/*   4 					*/	0xC7, 0,                            /*3*/
	/*   5 					*/	0xFB, 0,                            /*4*/
	/*   6 					*/	0xFD, 0,                            /*5*/
	/*   7 					*/	0xC1, 0,                            /*6*/
	/*   8 					*/	0xDC, 0,                            /*7*/
	/*   9 					*/	0xFC, 0,                            /*8*/
	/*  10 					*/	0xA1, 0,                            /*9*/
	/*  11 					*/	0xCA, 0,                            /*0*/
	/*  12 					*/	0x00, 0,                            /*-*/
	/*  13 					*/	0x00, 0,                            /*=*/
	/*  14 					*/	0x00, 0,
	/*  15 					*/	0x00, 0,                            /*BS*/
	/*  16 					*/	0x00, 0,                            /*tab*/
	/*  17 					*/	0x00, 0,                            /*Q*/
	/*  18 					*/	0x00, 0,                            /*W*/
	/*  19 					*/	0x00, 0,                            /*E*/
	/*  20 					*/	0x00, 0,                            /*R*/
	/*  21 					*/	0x00, 0,                            /*T*/
	/*  22 					*/	0x00, 0,                            /*Y*/
	/*  23 					*/	0x00, 0,                            /*U*/
	/*  24 					*/	0x00, 0,                            /*I*/
	/*  25 					*/	0x00, 0,                            /*O*/
	/*  26 					*/	0x00, 0,                            /*P*/
	/*  27 					*/	0x00, 0,                            /*[*/
	/*  28 					*/	0x00, 0,                            /*]*/
	/*  29 					*/	0x00, 0,                            /*\*/
	/*  30 					*/	0x00, 0,                            /*CAPS*/
	/*  31 					*/	0x00, 0,                            /*A*/
	/*  32 					*/	0x00, 0,                            /*S*/
	/*  33 					*/	0x00, 0,                            /*D*/
	/*  34 					*/	0x00, 0,                            /*F*/
	/*  35 					*/	0x00, 0,                            /*G*/
	/*  36 					*/	0x00, 0,                            /*H*/
	/*  37 					*/	0x00, 0,                            /*J*/
	/*  38 					*/	0x00, 0,                            /*K*/
	/*  39 					*/	0x00, 0,                            /*L*/
	/*  40 					*/	0x00, 0,                            /*;*/
	/*  41 					*/	0x00, 0,                            /*'*/
	/*  42 					*/	0x00, 0,
	/*  43 					*/	0x8D, 0,                            /*ENTER*/
	/*  44 					*/	0x00, 0,                            /*LEFT SHIFT*/
	/*  45 					*/	0x00, 0,
	/*  46 					*/	0x00, 0,                            /*Z*/
	/*  47 					*/	0x00, 0,                            /*X*/
	/*  48 					*/	0x00, 0,                            /*C*/
	/*  49 					*/	0x00, 0,                            /*V*/
	/*  50 					*/	0x00, 0,                            /*B*/
	/*  51 					*/	0x00, 0,                            /*N*/
	/*  52 					*/	0x00, 0,                            /*M*/
	/*  53 					*/	0x00, 0,                            /*,*/
	/*  54 					*/	0xAE, 0,                            /*.*/
	/*  55 					*/	0x00, 0,                            /*'/'*/
	/*  56 					*/	0x00, 0,
	/*  57 					*/	0x00, 0,                            /*RIGHT SHIFT*/
	/*  58 					*/	0x00, 0,                            /*LFT CTRL*/
	/*  59 					*/	0x00, 0,
	/*  60 					*/	0x00, 0,                            /*LFT ALT*/
	/*  61 					*/	0x8E, 0,                            /*SPACE*/
	/*  62 					*/	0x00, 0,                            /*RT ALT*/
	/*  63 					*/	0x00, 0,
	/*  64 					*/	0x00, 0,                            /*RT CTRL*/
	/*  65 					*/	0x00, 0,
	/*  66 					*/	0x00, 0,
	/*  67 					*/	0x00, 0,
	/*  68 					*/	0x00, 0,
	/*  69 					*/	0x00, 0,
	/*  70 					*/	0x00, 0,
	/*  71 					*/	0x00, 0,
	/*  72 					*/	0x00, 0,
	/*  73 					*/	0x00, 0,
	/*  74 					*/	0x00, 0,
	/*  75 					*/	0x88, 0,                            /*INS*/
	/*  76 					*/	0x00, 0,                            /*DEL*/
	/*  77 					*/	0x00, 0,
	/*  78 					*/	0x00, 0,
	/*  79 					*/	0x83, 0,                            /*LFT ARROW*/
	/*  80 					*/	0x00, 0,                            /*HOME*/
	/*  81 					*/	0x00, 0,                            /*END*/
	/*  82 					*/	0x00, 0,
	/*  83 					*/	0x8B, 0,                            /*UP ARROW*/
	/*  84 					*/	0x81, 0,                            /*DOWN ARROW*/
	/*  85 					*/	0x00, 0,                            /*PGUP*/
	/*  86 					*/	0x00, 0,                            /*PGDN*/
	/*  87 					*/	0x00, 0,
	/*  88 					*/	0x00, 0,
	/*  89 					*/	0xD4, 0,                            /*RT ARROW*/
	/*  90 					*/	0x00, 0,                            /*NUM LOCK*/
	/*  91 					*/	0x00, 0,                            /*NUMPAD 7*/
	/*  92 					*/	0x00, 0,                            /*NUMPAD 4*/
	/*  93 					*/	0x00, 0,                            /*NUMPAD 1*/
	/*  94 					*/	0x00, 0,
	/*  95 					*/	0x00, 0,                            /*NUMPAD /  */
	/*  96 					*/	0x00, 0,                            /*NUMPAD 8*/
	/*  97 					*/	0x00, 0,                            /*NUMPAD 5*/
	/*  98 					*/	0x00, 0,                            /*NUMPAD 2*/
	/*  99 					*/	0x00, 0,                            /*NUMPAD 0*/
	/* 100 					*/	0x00, 0,                            /*NUMPAD **/
	/* 101 					*/	0x00, 0,                            /*NUMPAD 9*/
	/* 102 					*/	0x00, 0,                            /*NUMPAD 6*/
	/* 103 					*/	0x00, 0,                            /*NUMPAD 3*/
	/* 104 					*/	0x00, 0,                            /*NUMPAD DEL*/
	/* 105 					*/	0x00, 0,                            /*NUMPAD -*/
	/* 106 					*/	0x00, 0,                            /*NUMPAD +*/
	/* 107 					*/	0x00, 0,
	/* 108 					*/	0x00, 0,                            /*NUMPAD ENTER*/
	/* 109 					*/	0x00, 0,
	/* 110 					*/	0x00, 0,                            /*ESC*/
	/* 111 					*/	0x00, 0,
	/* 112 					*/	0x00, 0,                            /*F1*/
	/* 113 					*/	0x00, 0,                            /*F2*/
	/* 114 					*/	0x00, 0,                            /*F3*/
	/* 115 					*/	0x00, 0,                            /*F4*/
	/* 116 					*/	0x00, 0,                            /*F5*/
	/* 117 					*/	0x00, 0,                            /*F6*/
	/* 118 					*/	0x00, 0,                            /*F7*/
	/* 119 					*/	0x00, 0,                            /*F8*/
	/* 120 					*/	0x00, 0,                            /*F9*/
	/* 121 					*/	0x00, 0,                            /*F10*/
	/* 122 					*/	0x00, 0,                            /*F11*/
	/* 123 					*/	0x00, 0,                            /*F12*/
	/* 124 		  			*/	0x00, 0,                            /*PRNT SCRN*/
	/* 125 					*/	0x00, 0,                            /*SCROLL LOCK*/
	/* 126 					*/	0x00, 0,                            /*BREAK*/
	/* 127 					*/	0x00, 0,
	/* 128 					*/	0x00, 0,
	/* 129 					*/	0x00, 0,
	/* 130 					*/	0x00, 0,
	/* 131 					*/	0x00, 0,
	/* 132 					*/	0x00, 0,
	/* 133 					*/	0x00, 0,
	/* 134 					*/	0x00, 0,
	/* 135 					*/	0x00, 0,
	/* 136 					*/	0x00, 0,
	/* 137		 			*/	0x00, 0,
	/* 138 	 				*/	0x00, 0,
	/* 139 	  				*/	0x00, 0,
	/* 140 					*/	0x00, 0,
	/* 141 					*/	0x00, 0,
	/* 142 					*/	0x00, 0,
	/* 143 					*/	0x00, 0
};
static const CScancodeSequence seq_jap_106_SCAN[] =
{
/*    30 */		  6,	 30, { 0X12, 0x58, 0xF0, 0x58, 0xF0, 0x12 },
/*   124 */		 10, 	124, { 0xE0, 0x12, 0xE0, 0x7C, 0xE0, 0xF0, 0x7C, 0xE0, 0xF0, 0x12 },
/*   126 */		  8, 	126, { 0xE1, 0x14, 0x77, 0xE1, 0xF0, 0x14, 0xF0, 0x77 },
	0
};
const CScancodes JAP_106_SCAN =
{
	/* sequence info		*/	seq_jap_106_SCAN,
	/* max used key		*/	143,
	/* extension			*/	0xe0,
	/* illegal scancode	*/	0x00, 0,
	/*   1 					*/	0x0E, 0,
	/*   2 					*/	0x16, 0,
	/*   3 					*/	0x1E, 0,
	/*   4 					*/	0x26, 0,
	/*   5 					*/	0x25, 0,
	/*   6 					*/	0x2E, 0,
	/*   7 					*/	0x36, 0,
	/*   8 					*/	0x3D, 0,
	/*   9 					*/	0x3E, 0,
	/*  10 					*/	0x46, 0,
	/*  11 					*/	0x45, 0,
	/*  12 					*/	0x4E, 0,
	/*  13 					*/	0x55, 0,
	/*  14 Empty			*/	0x6A, 0,
	/*  15 					*/	0x66, 0,
	/*  16 					*/	0x0D, 0,
	/*  17 					*/	0x15, 0,
	/*  18 					*/	0x1D, 0,
	/*  19 					*/	0x24, 0,
	/*  20 					*/	0x2D, 0,
	/*  21 					*/	0x2C, 0,
	/*  22 					*/	0x35, 0,
	/*  23 					*/	0x3C, 0,
	/*  24 					*/	0x43, 0,
	/*  25 					*/	0x44, 0,
	/*  26 					*/	0x4D, 0,
	/*  27 					*/	0x54, 0,
	/*  28 					*/	0x5B, 0,
	/*  29 					*/	0x5D, 0,
	/*  30 					*/	0xE0, 0|KN_COMMUNICATION|KN_SEQUENCE,	/*MpS*/
	/*  31 					*/	0x1C, 0,
	/*  32 					*/	0x1B, 0,
	/*  33 					*/	0x23, 0,
	/*  34 					*/	0x2B, 0,
	/*  35 					*/	0x34, 0,
	/*  36 					*/	0x33, 0,
	/*  37 					*/	0x3B, 0,
	/*  38 					*/	0x42, 0,
	/*  39 					*/	0x4B, 0,
	/*  40 					*/	0x4C, 0,
	/*  41 					*/	0x52, 0,
	/*  42 					*/	0x5D, 0,
	/*  43 					*/	0x5A, 0,
	/*  44 					*/	0x12, 0|KN_COMMUNICATION,
	/*  45 					*/	0x61, 0,
	/*  46 					*/	0x1A, 0,
	/*  47 					*/	0x22, 0,
	/*  48 					*/	0x21, 0,
	/*  49 					*/	0x2A, 0,
	/*  50 					*/	0x32, 0,
	/*  51 					*/	0x31, 0,
	/*  52 					*/	0x3A, 0,
	/*  53 					*/	0x41, 0,
	/*  54 					*/	0x49, 0,
	/*  55 					*/	0x4A, 0,
	/*  56 					*/	0x2F, KN_EXTENDED,
	/*  57 					*/	0x59, 0|KN_COMMUNICATION,
	/*  58 					*/	0x14, 0|KN_COMMUNICATION,
	/*  59 					*/	0x1F, KN_EXTENDED,
	/*  60 					*/	0x11, 0,
	/*  61 					*/	0x29, 0,
	/*  62 					*/	0x11, KN_EXTENDED,
	/*  63 					*/	0x27, KN_EXTENDED,
	/*  64 					*/	0x14, KN_EXTENDED|KN_COMMUNICATION,
	/*  65 					*/	0x06, 0,
	/*  66 					*/	0x0C, 0,
	/*  67 					*/	0x0B, 0,
	/*  68 					*/	0x0A, 0,
	/*  69 					*/	0x09, 0,
	/*  70 					*/	0x05, 0,
	/*  71 					*/	0x04, 0,
	/*  72 					*/	0x03, 0,
	/*  73 					*/	0x83, 0,
	/*  74 					*/	0x01, 0,
	/*  75 					*/	0x70, KN_EXTENDED,
	/*  76 					*/	0x71, KN_EXTENDED,
	/*  77 Empty			*/	0x51, 0,
	/*  78 					*/	0x6B, KN_EXTENDED,
	/*  79 					*/	0x6B, KN_EXTENDED,
	/*  80 					*/	0x6C, KN_EXTENDED,
	/*  81 					*/	0x69, KN_EXTENDED,
	/*  82 					*/	0x75, KN_EXTENDED,
	/*  83 					*/	0x75, KN_EXTENDED,
	/*  84 					*/	0x72, KN_EXTENDED,
	/*  85 					*/	0x7D, KN_EXTENDED,
	/*  86 					*/	0x7A, KN_EXTENDED,
	/*  87 Empty			*/	0x00, 0,
	/*  88 					*/	0x74, KN_EXTENDED,
	/*  89 					*/	0x74, KN_EXTENDED,
	/*  90 					*/	0x77, 0|KN_COMMUNICATION,
	/*  91 					*/	0x6C, 0,
	/*  92 					*/	0x6B, 0,
	/*  93 					*/	0x69, 0,
	/*  94 					*/	0x68, 0,
	/*  95 					*/	0x4A, KN_EXTENDED,
	/*  96 					*/	0x75, 0,
	/*  97 					*/	0x73, 0,
	/*  98 					*/	0x72, 0,
	/*  99 					*/	0x70, 0,
	/* 100 					*/	0x7C, 0,
	/* 101 					*/	0x7D, 0,
	/* 102 					*/	0x74, 0,
	/* 103 					*/	0x7A, 0,
	/* 104 					*/	0x71, 0,
	/* 105 					*/	0x7B, 0,
	/* 106 					*/	0x79, 0,
	/* 107 					*/	0x7B, 0,
	/* 108 					*/	0x5A, KN_EXTENDED,
	/* 109 					*/	0x78, 0,
	/* 110 					*/	0x76, 0,
	/* 111 Empty			*/	0x00, 0,
	/* 112 					*/	0x05, 0,
	/* 113 					*/	0x06, 0,
	/* 114 					*/	0x04, 0,
	/* 115 					*/	0x0C, 0,
	/* 116 					*/	0x03, 0,
	/* 117 					*/	0x0B, 0,
	/* 118 					*/	0x83, 0,
	/* 119 					*/	0x0A, 0,
	/* 120 					*/	0x01, 0,
	/* 121 					*/	0x09, 0,
	/* 122 					*/	0x78, 0,
	/* 123 					*/	0x07, 0,
	/* 124 Sequence		*/	0xE0, KN_SEQUENCE,
	/* 125 					*/	0x7E, 0,
	/* 126 Sequence		*/	0xE0, KN_SEQUENCE,
	/* 127 					*/	0x08, 0,
	/* 128 					*/	0x10, 0,
	/* 129 					*/	0x18, 0,
	/* 130 					*/	0x20, 0,
	/* 131 					*/	0x28, 0,
	/* 132 					*/	0x30, 0,
	/* 133 					*/	0x38, 0,
	/* 134 					*/	0x40, 0,
	/* 135 					*/	0x48, 0,
	/* 136 					*/	0x50, 0,
	/* 137 Empty			*/	0x00, 0,
	/* 138 Empty			*/	0x00, 0,
	/* 139 Empty			*/	0x00, 0,
	/* 140 					*/	0x11, 0,
	/* 141 					*/	0x11, 0,
	/* 142 					*/	0x14, 0,
	/* 143 					*/	0x14, 0
};

static const CScancodeSequence seq_NEC_98_SCAN[] =
{
/*    30 */		  6,	 30, { 0X12, 0x58, 0xF0, 0x58, 0xF0, 0x12 },
/*   124 */		 10, 	124, { 0xE0, 0x12, 0xE0, 0x7C, 0xE0, 0xF0, 0x7C, 0xE0, 0xF0, 0x12 },
/*   126 */		  8, 	126, { 0xE1, 0x14, 0x77, 0xE1, 0xF0, 0x14, 0xF0, 0x77 },
	0
};
const CScancodes NEC_98_SCAN =
{
	/* sequence info		*/	seq_NEC_98_SCAN,
	/* max used key		*/	143,
	/* extension			*/	0xe0,
	/* illegal scancode	*/	0x00, 0,
	/*   1 					*/	0x00, 0,
	/*   2 					*/	0xF2, 0|KN_COMMUNICATION,
	/*   3 					*/	0x72, 0|KN_COMMUNICATION,
	/*   4 					*/	0x00, 0,
	/*   5 					*/	0x00, 0,
	/*   6 					*/	0x00, 0,
	/*   7 					*/	0x00, 0,
	/*   8 					*/	0x0E, 0,
	/*   9 					*/	0x0F, 0,
	/*  10 					*/	0x00, 0,
	/*  11 					*/	0x00, 0,
	/*  12 					*/	0x00, 0,
	/*  13 					*/	0x1C, 0,
	/*  14 					*/	0x00, 0,
	/*  15 					*/	0x00, 0,
	/*  16 					*/	0x00, 0,
	/*  17 					*/	0x00, 0,
	/*  18 					*/	0x00, 0,
	/*  19 					*/	0x00, 0,
	/*  20 					*/	0x70, 0,
	/*  21 					*/	0x00, 0,
	/*  22 					*/	0x62, 0,
	/*  23 					*/	0x63, 0,
	/*  24 					*/	0x64, 0,
	/*  25 					*/	0x65, 0,
	/*  26 					*/	0x66, 0,
	/*  27 					*/	0x67, 0,
	/*  28 					*/	0x68, 0,
	/*  29 					*/	0x69, 0,
	/*  30 					*/	0x6A, 0,
	/*  31 					*/	0x6B, 0,
   /*  32 					*/	0x34, 0,
	/*  33 					*/	0x01, 0,
	/*  34 					*/	0x02, 0,
	/*  35 					*/	0x03, 0,
	/*  36 					*/	0x04, 0,
	/*  37 					*/	0x05, 0,
	/*  38 					*/	0x06, 0,
	/*  39 					*/	0x07, 0,
	/*  40 					*/	0x08, 0,
	/*  41 					*/	0x09, 0,
	/*  42 					*/	0x27, 0,
	/*  43 					*/	0x26, 0,
	/*  44 					*/	0x30, 0,
	/*  45 					*/	0x0B, 0,
	/*  46 					*/	0x31, 0,
	/*  47 					*/	0x32, 0,
	/*  48 					*/	0x0A, 0,
	/*  49 					*/	0x01, 0,
	/*  50 					*/	0x02, 0,
	/*  51 					*/	0x03, 0,
	/*  52 					*/	0x04, 0,
	/*  53 					*/	0x05, 0,
	/*  54 					*/	0x06, 0,
	/*  55 					*/	0x07, 0,
	/*  56 					*/	0x08, 0,
	/*  57 					*/	0x09, 0,
	/*  58 					*/	0x27, 0,
	/*  59 					*/	0x26, 0,
	/*  60 					*/	0x30, 0,
	/*  61 					*/	0x0B, 0,
	/*  62 					*/	0x31, 0,
	/*  63 					*/	0x32, 0,
	/*  64 					*/	0x1A, 0,
	/*  65 					*/	0x1D, 0,
	/*  66 					*/	0x2D, 0,
	/*  67 					*/	0x2B, 0,
	/*  68 					*/	0x1F, 0,
	/*  69 					*/	0x12, 0,
	/*  70 					*/	0x20, 0,
	/*  71 					*/	0x21, 0,
	/*  72 					*/	0x22, 0,
	/*  73 					*/	0x17, 0,
	/*  74 					*/	0x23, 0,
	/*  75 					*/	0x24, 0,
	/*  76 					*/	0x25, 0,
	/*  77 					*/	0x2F, 0,
	/*  78 					*/	0x2E, 0,
	/*  79 					*/	0x18, 0,
	/*  80 					*/	0x19, 0,
	/*  81 					*/	0x10, 0,
	/*  82 					*/	0x13, 0,
	/*  83 					*/	0x1E, 0,
	/*  84 					*/	0x14, 0,
	/*  85 					*/	0x16, 0,
	/*  86 					*/	0x2C, 0,
	/*  87 					*/	0x11, 0,
	/*  88 					*/	0x2A, 0,
	/*  89 					*/	0x15, 0,
	/*  90 					*/	0x29, 0,
	/*  91 					*/	0x1B, 0,
	/*  92 					*/	0x0D, 0,
	/*  93 					*/	0x28, 0,
	/*  94 					*/	0x0C, 0,
	/*  95 					*/	0x33, 0,
	/*  96 					*/	0x0C, 0,
	/*  97 					*/	0x1D, 0,
	/*  98 					*/	0x2D, 0,
	/*  99 					*/	0x2B, 0,
	/* 100 					*/	0x1F, 0,
	/* 101 					*/	0x12, 0,
	/* 102 					*/	0x20, 0,
	/* 103 					*/	0x21, 0,
	/* 104 					*/	0x22, 0,
	/* 105 					*/	0x17, 0,
	/* 106 					*/	0x23, 0,
	/* 107 					*/	0x24, 0,
	/* 108 					*/	0x25, 0,
	/* 109 					*/	0x2F, 0,
	/* 110 					*/	0x2E, 0,
	/* 111 					*/	0x18, 0,
	/* 112 					*/	0x19, 0,
	/* 113 					*/	0x10, 0,
	/* 114 					*/	0x13, 0,
	/* 115 					*/	0x1E, 0,
	/* 116 					*/	0x14, 0,
	/* 117 					*/	0x16, 0,
	/* 118 					*/	0x2C, 0,
	/* 119 					*/	0x11, 0,
	/* 120 					*/	0x2A, 0,
	/* 121 					*/	0x15, 0,
	/* 122 					*/	0x29, 0,
	/* 123 					*/	0x1B, 0,
	/* 124 					*/	0x0D, 0,
	/* 125 					*/	0x28, 0,
	/* 126 					*/	0x1A, 0,
	/* 127 					*/	0x00, 0,
	/* 128 					*/	0x00, 0,
	/* 129 					*/	0x00, 0,
	/* 130 					*/	0x00, 0,
	/* 131 					*/	0x00, 0,
	/* 132 					*/	0x00, 0,
	/* 133 					*/	0x00, 0,
	/* 134 					*/	0x00, 0,
	/* 135 					*/	0x00, 0,
	/* 136 					*/	0x00, 0,
	/* 137 					*/	0x00, 0,
	/* 138 					*/	0x00, 0,
	/* 139 					*/	0x00, 0,
	/* 140 					*/	0x00, 0,
	/* 141 					*/	0x00, 0,
	/* 142 					*/	0x00, 0,
	/* 143 					*/	0x00, 0
};

/*	BURROUGHS B25 K4 keyboard --------------------------------------------------*/
const CScancodes BURROUGHS_K4_SCAN =
{
	/* sequence info		*/	NULL,
	/* max used key		*/	143,
	/* extension			*/ 0x00,
	/* illegal scancode	*/ 0x00, 0,
	/*   1 					*/ 0x00, 0,
	/*   2 					*/ 0x00, 0,
	/*   3 					*/ 0x00, 0,
	/*   4 					*/ 0x00, 0,
	/*   5 					*/ 0x00, 0,
	/*   6 					*/ 0x00, 0,
	/*   7 					*/ 0x00, 0,
	/*   8 					*/ 0x00, 0,
	/*   9 					*/ 0x00, 0,
	/*  10 					*/ 0x27, 0,
	/*  11 					*/ 0x00, 0,
	/*  12 					*/ 0x00, 0,
	/*  13 					*/ 0x27, 0,
	/*  14 					*/ 0x00, 0,
	/*  15 					*/ 0x00, 0,
	/*  16 					*/ 0x00, 0,
	/*  17 					*/ 0x00, 0,
	/*  18 					*/ 0x00, 0,
	/*  19 					*/ 0x00, 0,
	/*  20 					*/ 0x00, 0,
	/*  21 					*/ 0x00, 0,
	/*  22 					*/ 0x00, 0,
	/*  23 					*/ 0x00, 0,
	/*  24 					*/ 0x00, 0,
	/*  25 					*/ 0x00, 0,
	/*  26 					*/ 0x00, 0,
	/*  27 					*/ 0x77, 0,
	/*  28 					*/ 0x00, 0,
	/*  29 					*/ 0x00, 0,
	/*  30 					*/ 0x00, 0,
	/*  31 					*/ 0x00, 0,
	/*  32 					*/ 0x47, 0,
	/*  33 					*/ 0x7C, 0,
	/*  34 					*/ 0x07, 0,
	/*  35 					*/ 0x0E, 0,
	/*  36 					*/ 0x10, 0,
	/*  37 					*/ 0x71, 0,
	/*  38 					*/ 0x62, 0,
	/*  39 					*/ 0x00, 0,
	/*  40 					*/ 0x3D, 0,
	/*  41 					*/ 0x56, 0,
	/*  42 					*/ 0x31, 0,
	/*  43 					*/ 0x13, 0,
	/*  44 					*/ 0x57, 0,
	/*  45 					*/ 0x25, 0,
	/*  46 					*/ 0x45, 0,
	/*  47 					*/ 0x79, 0,
	/*  48 					*/ 0x56, 0,
	/*  49 					*/ 0x7C, 0,
	/*  50 					*/ 0x4E, 0,
	/*  51 					*/ 0x0E, 0,
	/*  52 					*/ 0x10, 0,
	/*  53 					*/ 0x20, 0,
	/*  54 					*/ 0x3E, 0,
	/*  55 					*/ 0x62, 0,
	/*  56 					*/ 0x60, 0,
	/*  57 					*/ 0x3D, 0,
	/*  58 					*/ 0x51, 0,
	/*  59 					*/ 0x09, 0,
	/*  60 					*/ 0x0F, 0,
	/*  61 					*/ 0x5E, 0,
	/*  62 					*/ 0x7F, 0,
	/*  63 					*/ 0x76, 0,
	/*  64 					*/ 0x4E, 0,
	/*  65 					*/ 0x2B, 0,
	/*  66 					*/ 0x4B, 0,
	/*  67 					*/ 0x0B, 0,
	/*  68 					*/ 0x73, 0,
	/*  69 					*/ 0x33, 0,
	/*  70 					*/ 0x53, 0,
	/*  71 					*/ 0x63, 0,
	/*  72 					*/ 0x23, 0,
	/*  73 					*/ 0x43, 0,
	/*  74 					*/ 0x03, 0,
	/*  75 					*/ 0x2F, 0,
	/*  76 					*/ 0x3B, 0,
	/*  77 					*/ 0x42, 0,
	/*  78 					*/ 0x39, 0,
	/*  79 					*/ 0x59, 0,
	/*  80 					*/ 0x19, 0,
	/*  81 					*/ 0x69, 0,
	/*  82 					*/ 0x29, 0,
	/*  83 					*/ 0x49, 0,
	/*  84 					*/ 0x09, 0,
	/*  85 					*/ 0x1B, 0,
	/*  86 					*/ 0x71, 0,
	/*  87 					*/ 0x31, 0,
	/*  88 					*/ 0x79, 0,
	/*  89 					*/ 0x25, 0,
	/*  90 					*/ 0x67, 0,
	/*  91 					*/ 0x13, 0,
	/*  92 					*/ 0x00, 0,
	/*  93 					*/ 0x5E, 0,
	/*  94 					*/ 0x00, 0,
	/*  95 					*/ 0x00, 0,
	/*  96 					*/ 0x00, 0,
	/*  97 					*/ 0x00, 0,
	/*  98 					*/ 0x00, 0,
	/*  99 					*/ 0x00, 0,
	/* 100 					*/ 0x00, 0,
	/* 101 					*/ 0x00, 0,
	/* 102 					*/ 0x00, 0,
	/* 103 					*/ 0x00, 0,
	/* 104 					*/ 0x00, 0,
	/* 105 					*/ 0x00, 0,
	/* 106 					*/ 0x00, 0,
	/* 107 					*/ 0x00, 0,
	/* 108 					*/ 0x00, 0,
	/* 109 					*/ 0x00, 0,
	/* 110 					*/ 0x00, 0,
	/* 111 					*/ 0x00, 0,
	/* 112 					*/ 0x00, 0,
	/* 113 					*/ 0x00, 0,
	/* 114 					*/ 0x00, 0,
	/* 115 					*/ 0x00, 0,
	/* 116 					*/ 0x00, 0,
	/* 117 					*/ 0x00, 0,
	/* 118 					*/ 0x00, 0,
	/* 119 					*/ 0x00, 0,
	/* 120 					*/ 0x00, 0,
	/* 121 					*/ 0x00, 0,
	/* 122 					*/ 0x00, 0,
	/* 123 					*/ 0x00, 0,
	/* 124 		  			*/ 0x00, 0,
	/* 125 					*/ 0x00, 0,
	/* 126 					*/ 0x60, 0,
	/* 127 					*/ 0x00, 0,
	/* 128 					*/ 0x15, KN_NO_DELAY,             //left shift key
	/* 129 					*/ 0x6F, KN_NO_DELAY,             //right shift key
	/* 130 					*/    0, 0,
	/* 131 					*/    0, 0,
	/* 132 					*/    0, 0,
	/* 133 					*/    0, 0,
	/* 134 					*/    0, 0,
	/* 135 					*/    0, 0,
	/* 136 					*/    0, 0,
	/* 137		 			*/    0, 0,
	/* 138 	 				*/    0, 0,
	/* 139 	  				*/    0, 0,
	/* 140 					*/    0, 0,
	/* 141 					*/    0, 0,
	/* 142 					*/    0, 0,
	/* 143 					*/    0, 0
};

/*	BURROUGHS B25 K4 keyboard --------------------------------------------------*/
const CScancodes BURROUGHS_K5_SCAN =
{
	/* sequence info		*/	NULL,
	/* max used key			*/	143,
	/* extension			*/ 0x00,
	/* illegal scancode		*/ 0x00, 0,
	/*   1 					*/ 0x57, 0,
	/*   2 					*/ 0x11, 0,
	/*   3 					*/ 0x0D, 0,
	/*   4 					*/ 0x22, 0,
	/*   5 					*/ 0x12, 0,
	/*   6 					*/ 0x12, 0,
	/*   7 					*/ 0x0D, 0,
	/*   8 					*/ 0x77, 0,
	/*   9 					*/ 0x37, 0,
	/*  10 					*/ 0x11, 0,
	/*  11 					*/ 0x57, 0,
	/*  12 					*/ 0x00, 0,
	/*  13 					*/ 0x57, 0,
	/*  14 					*/ 0x00, 0,
	/*  15 					*/ 0x3F, 0,
	/*  16 					*/ 0x47, 0,
	/*  17 					*/ 0x17, 0,
	/*  18 					*/ 0x07, 0,
	/*  19 					*/ 0x00, 0,
	/*  20 					*/ 0x27, 0,
	/*  21 					*/ 0x13, 0,
	/*  22 					*/ 0x2B, 0,
	/*  23 					*/ 0x4B, 0,
	/*  24 					*/ 0x0B, 0,
	/*  25 					*/ 0x73, 0,
	/*  26 					*/ 0x33, 0,
	/*  27 					*/ 0x53, 0,
	/*  28 					*/ 0x63, 0,
	/*  29 					*/ 0x23, 0,
	/*  30 					*/ 0x43, 0,
	/*  31 					*/ 0x03, 0,
	/*  32 					*/ 0x7D, 0,
	/*  33 					*/ 0x39, 0,
	/*  34 					*/ 0x59, 0,
	/*  35 					*/ 0x32, 0,
	/*  36 					*/ 0x69, 0,
	/*  37 					*/ 0x29, 0,
	/*  38 					*/ 0x49, 0,
	/*  39 					*/ 0x42, 0,
	/*  40 					*/ 0x71, 0,
	/*  41 					*/ 0x31, 0,
	/*  42 					*/ 0x19, 0,
	/*  43 					*/ 0x25, 0,
	/*  44 					*/ 0x65, 0,
	/*  45 					*/ 0x05, 0,
	/*  46 					*/ 0x45, 0,
	/*  47 					*/ 0x09, 0,
	/*  48 					*/ 0x79, 0,
	/*  49 					*/ 0x39, 0,
	/*  50 					*/ 0x59, 0,
	/*  51 					*/ 0x19, 0,
	/*  52 					*/ 0x69, 0,
	/*  53 					*/ 0x29, 0,
	/*  54 					*/ 0x49, 0,
	/*  55 					*/ 0x09, 0,
	/*  56 					*/ 0x71, 0,
	/*  57 					*/ 0x31, 0,
	/*  58 					*/ 0x45, 0,
	/*  59 					*/ 0x65, 0,
	/*  60 					*/ 0x7C, 0,
	/*  61 					*/ 0x79, 0,
	/*  62 					*/ 0x4E, 0,
	/*  63 					*/ 0x25, 0,
	/*  64 					*/ 0x4F, 0,
	/*  65 					*/ 0x2B, 0,
	/*  66 					*/ 0x4B, 0,
	/*  67 					*/ 0x0B, 0,
	/*  68 					*/ 0x73, 0,
	/*  69 					*/ 0x33, 0,
	/*  70 					*/ 0x53, 0,
	/*  71 					*/ 0x63, 0,
	/*  72 					*/ 0x23, 0,
	/*  73 					*/ 0x43, 0,
	/*  74 					*/ 0x03, 0,
	/*  75 					*/ 0x2F, 0,
	/*  76 					*/ 0x3B, 0,
	/*  77 					*/ 0x42, 0,
	/*  78 					*/ 0x39, 0,
	/*  79 					*/ 0x59, 0,
	/*  80 					*/ 0x19, 0,
	/*  81 					*/ 0x69, 0,
	/*  82 					*/ 0x29, 0,
	/*  83 					*/ 0x49, 0,
	/*  84 					*/ 0x5B, 0,
	/*  85 					*/ 0x1B, 0,
	/*  86 					*/ 0x71, 0,
	/*  87 					*/ 0x31, 0,
	/*  88 					*/ 0x79, 0,
	/*  89 					*/ 0x25, 0,
	/*  90 					*/ 0x67, 0,
	/*  91 					*/ 0x62, 0,
	/*  92 					*/ 0x16, 0,
	/*  93 					*/ 0x60, 0,
	/*  94 					*/ 0x3D, 0,
	/*  95 					*/ 0x05, 0,
	/*  96 					*/ 0x15, 0,
	/*  97 					*/ 0x3C, 0,
	/*  98 					*/ 0x5C, 0,
	/*  99 					*/ 0x1C, 0,
	/* 100 					*/ 0x6C, 0,
	/* 101 					*/ 0x2C, 0,
	/* 102 					*/ 0x4C, 0,
	/* 103 					*/ 0x0C, 0,
	/* 104 					*/ 0x74, 0,
	/* 105 					*/ 0x34, 0,
	/* 106 					*/ 0x54, 0,
	/* 107 					*/ 0x14, 0,
	/* 108 					*/ 0x64, 0,
	/* 109 					*/ 0x24, 0,
	/* 110 					*/ 0x44, 0,
	/* 111 					*/ 0x04, 0,
	/* 112 					*/ 0x78, 0,
	/* 113 					*/ 0x38, 0,
	/* 114 					*/ 0x58, 0,
	/* 115 					*/ 0x18, 0,
	/* 116 					*/ 0x68, 0,
	/* 117 					*/ 0x28, 0,
	/* 118 					*/ 0x48, 0,
	/* 119 					*/ 0x08, 0,
	/* 120 					*/ 0x70, 0,
	/* 121 					*/ 0x30, 0,
	/* 122 					*/ 0x50, 0,
	/* 123 					*/ 0x10, 0,
	/* 124 		  			*/ 0x00, 0,
	/* 125 					*/ 0x20, 0,
	/* 126 					*/ 0x3E, 0,
	/* 127 					*/ 0x00, 0,
	/* 128 					*/ 0x76, KN_NO_DELAY,	/*burroughs left shift for lack of a better place*/
	/* 129 					*/    0, 0,
	/* 130 					*/    0, 0,
	/* 131 					*/    0, 0,
	/* 132 					*/    0, 0,
	/* 133 					*/    0, 0,
	/* 134 					*/    0, 0,
	/* 135 					*/    0, 0,
	/* 136 					*/    0, 0,
	/* 137		 			*/    0, 0,
	/* 138 	 				*/    0, 0,
	/* 139 	  				*/    0, 0,
	/* 140 					*/    0, 0,
	/* 141 					*/    0, 0,
	/* 142 					*/    0, 0,
	/* 143 					*/    0, 0
};

/******************************************************/
/* Make Break tables                                  */
/******************************************************/

static const CMakeBreaks CTOSUNISYS_KB100_MKBR =
{
	/*			pre byte, type */
	/*   0 */	0xF0, BR_All,
	/*   1 */	0xF0, BR_All,
	/*   2 */	0xF0, BR_All,
	/*   3 */	0xF0, BR_All,
	/*   4 */	0xF0, BR_All,
	/*   5 */	0xF0, BR_All,
	/*   6 */	0xF0, BR_All,
	/*   7 */	0xF0, BR_All,
	/*   8 */	0xF0, BR_All,
	/*   9 */	0xF0, BR_All,
	/*  10 */	0xF0, BR_All,
	/*  11 */	0xF0, BR_All,
	/*  12 */	0xF0, BR_All,
	/*  13 */	0xF0, BR_All,
	/*  14 */	0xF0, BR_All,
	/*  15 */	0xF0, BR_All,
	/*  16 */	0xF0, BR_All,
	/*  17 */	0xF0, BR_All,
	/*  18 */	0xF0, BR_All,
	/*  19 */	0xF0, BR_All,
	/*  20 */	0xF0, BR_All,
	/*  21 */	0xF0, BR_All,
	/*  22 */	0xF0, BR_All,
	/*  23 */	0xF0, BR_All,
	/*  24 */	0xF0, BR_All,
	/*  25 */	0xF0, BR_All,
	/*  26 */	0xF0, BR_All,
	/*  27 */	0xF0, BR_All,
	/*  28 */	0xF0, BR_All,
	/*  29 */	0xF0, BR_All,
	/*  30 */	0xF0, BR_All,
	/*  31 */	0xF0, BR_All,
	/*  32 */	0xF0, BR_All,
	/*  33 */	0xF0, BR_All,
	/*  34 */	0xF0, BR_All,
	/*  35 */	0xF0, BR_All,
	/*  36 */	0xF0, BR_All,
	/*  37 */	0xF0, BR_All,
	/*  38 */	0xF0, BR_All,
	/*  39 */	0xF0, BR_All,
	/*  40 */	0xF0, BR_All,
	/*  41 */	0xF0, BR_All,
	/*  42 */	0xF0, BR_All,
	/*  44 */	0xF0, BR_All,
	/*  45 */	0xF0, BR_All,
	/*  46 */	0xF0, BR_All,
	/*  47 */	0xF0, BR_All,
	/*  48 */	0xF0, BR_All,
	/*  49 */	0xF0, BR_All,
	/*  50 */	0xF0, BR_All,
	/*  51 */	0xF0, BR_All,
	/*  52 */	0xF0, BR_All,
	/*  53 */	0xF0, BR_All,
	/*  54 */	0xF0, BR_All,
	/*  55 */	0xF0, BR_All,
	/*  56 */	0xF0, BR_All,
	/*  57 */	0xF0, BR_All,
	/*  58 */	0xF0, BR_All,
	/*  59 */	0xF0, BR_All,
	/*  60 */	0xF0, BR_All,
	/*  61 */	0xF0, BR_All,
	/*  62 */	0xF0, BR_All,
	/*  63 */	0xF0, BR_All,
	/*  64 */	0xF0, BR_All,
	/*  65 */	0xF0, BR_All,
	/*  66 */	0xF0, BR_All,
	/*  67 */	0xF0, BR_All,
	/*  68 */	0xF0, BR_All,
	/*  69 */	0xF0, BR_All,
	/*  70 */	0xF0, BR_All,
	/*  71 */	0xF0, BR_All,
	/*  72 */	0xF0, BR_All,
	/*  73 */	0xF0, BR_All,
	/*  74 */	0xF0, BR_All,
	/*  75 */	0xF0, BR_All,
	/*  76 */	0xF0, BR_All,
	/*  77 */	0xF0, BR_All,
	/*  78 */	0xF0, BR_All,
	/*  79 */	0xF0, BR_All,
	/*  80 */	0xF0, BR_All,
	/*  81 */	0xF0, BR_All,
	/*  82 */	0xF0, BR_All,
	/*  83 */	0xF0, BR_All,
	/*  84 */	0xF0, BR_All,
	/*  85 */	0xF0, BR_All,
	/*  86 */	0xF0, BR_All,
	/*  87 */	0xF0, BR_All,
	/*  88 */	0xF0, BR_All,
	/*  89 */	0xF0, BR_All,
	/*  90 */	0xF0, BR_All,
	/*  91 */	0xF0, BR_All,
	/*  92 */	0xF0, BR_All,
	/*  93 */	0xF0, BR_All,
	/*  94 */	0xF0, BR_All,
	/*  95 */	0xF0, BR_All,
	/*  96 */	0xF0, BR_All,
	/*  97 */	0xF0, BR_All,
	/*  98 */	0xF0, BR_All,
	/*  99 */	0xF0, BR_All,
	/* 100 */	0xF0, BR_All,
	/* 101 */	0xF0, BR_All,
	/* 102 */	0xF0, BR_All,
	/* 103 */	0xF0, BR_All,
	/* 104 */	0xF0, BR_All,
	/* 105 */	0xF0, BR_All,
	/* 106 */	0xF0, BR_All,
	/* 107 */	0xF0, BR_All,
	/* 108 */	0xF0, BR_All,
	/* 109 */	0xF0, BR_All,
	/* 110 */	0xF0, BR_All,
	/* 111 */	0xF0, BR_All,
	/* 112 */	0xF0, BR_All,
	/* 113 */	0xF0, BR_All,
	/* 114 */	0xF0, BR_All,
	/* 115 */	0xF0, BR_All,
	/* 116 */	0xF0, BR_All,
	/* 117 */	0xF0, BR_All,
	/* 118 */	0xF0, BR_All,
	/* 119 */	0xF0, BR_All,
	/* 120 */	0xF0, BR_All,
	/* 121 */	0xF0, BR_All,
	/* 122 */	0xF0, BR_All,
	/* 123 */	0xF0, BR_All,
	/* 124 */	0xF0, BR_All,
	/* 125 */	0xF0, BR_All,
	/* 126 */	0xF0, BR_All,
	/* 127 */	0xF0, BR_All,
	/* 128 */	0xF0, BR_All,
	/* 129 */	0xF0, BR_All,
	/* 130 */	0xF0, BR_All,
	/* 131 */	0xF0, BR_All,
	/* 132 */	0xF0, BR_All,
	/* 133 */	0xF0, BR_All,
	/* 134 */	0xF0, BR_All,
	/* 135 */	0xF0, BR_All,
	/* 136 */	0xF0, BR_All,
	/* 137 */	0xF0, BR_All,
	/* 138 */	0xF0, BR_All,
	/* 139 */	0xF0, BR_All,
	/* 140 */	0xF0, BR_All,
	/* 141 */	0xF0, BR_All,
	/* 142 */	0xF0, BR_All,
	/* 143 */	0xF0, BR_All,
	/* 144 */	0xF0, BR_All,
	/* 145 */	0xF0, BR_All,
	/* 146 */	0xF0, BR_All,
	/* 147 */	0xF0, BR_All,
	/* 148 */	0xF0, BR_All,
	/* 149 */	0xF0, BR_All
};


static const CMakeBreaks CTOSUNISYS_K5_MKBR =
{
	/*			pre byte, type */
	/*   0 */	0xE0, BR_SpecialParity,
	/*   1 */	0xE0, BR_SpecialParity,
	/*   2 */	0xE0, BR_SpecialParity,
	/*   3 */	0xE0, BR_SpecialParity,
	/*   4 */	0xE0, BR_SpecialParity,
	/*   5 */	0xE0, BR_SpecialParity,
	/*   6 */	0xE0, BR_SpecialParity,
	/*   7 */	0xE0, BR_SpecialParity,
	/*   8 */	0xE0, BR_SpecialParity,
	/*   9 */	0xE0, BR_SpecialParity,
	/*  10 */	0xE0, BR_SpecialParity,
	/*  11 */	0xE0, BR_SpecialParity,
	/*  12 */	0xE0, BR_SpecialParity,
	/*  13 */	0xE0, BR_SpecialParity,
	/*  14 */	0xE0, BR_SpecialParity,
	/*  15 */	0xE0, BR_SpecialParity,
	/*  16 */	0xE0, BR_SpecialParity,
	/*  17 */	0xE0, BR_SpecialParity,
	/*  18 */	0xE0, BR_SpecialParity,
	/*  19 */	0xE0, BR_SpecialParity,
	/*  20 */	0xE0, BR_SpecialParity,
	/*  21 */	0xE0, BR_SpecialParity,
	/*  22 */	0xE0, BR_SpecialParity,
	/*  23 */	0xE0, BR_SpecialParity,
	/*  24 */	0xE0, BR_SpecialParity,
	/*  25 */	0xE0, BR_SpecialParity,
	/*  26 */	0xE0, BR_SpecialParity,
	/*  27 */	0xE0, BR_SpecialParity,
	/*  28 */	0xE0, BR_SpecialParity,
	/*  29 */	0xE0, BR_SpecialParity,
	/*  30 */	0xE0, BR_SpecialParity,
	/*  31 */	0xE0, BR_SpecialParity,
	/*  32 */	0xE0, BR_SpecialParity,
	/*  33 */	0xE0, BR_SpecialParity,
	/*  34 */	0xE0, BR_SpecialParity,
	/*  35 */	0xE0, BR_SpecialParity,
	/*  36 */	0xE0, BR_SpecialParity,
	/*  37 */	0xE0, BR_SpecialParity,
	/*  38 */	0xE0, BR_SpecialParity,
	/*  39 */	0xE0, BR_SpecialParity,
	/*  40 */	0xE0, BR_SpecialParity,
	/*  41 */	0xE0, BR_SpecialParity,
	/*  42 */	0xE0, BR_SpecialParity,
	/*  44 */	0xE0, BR_SpecialParity,
	/*  45 */	0xE0, BR_SpecialParity,
	/*  46 */	0xE0, BR_SpecialParity,
	/*  47 */	0xE0, BR_SpecialParity,
	/*  48 */	0xE0, BR_SpecialParity,
	/*  49 */	0xE0, BR_SpecialParity,
	/*  50 */	0xE0, BR_SpecialParity,
	/*  51 */	0xE0, BR_SpecialParity,
	/*  52 */	0xE0, BR_SpecialParity,
	/*  53 */	0xE0, BR_SpecialParity,
	/*  54 */	0xE0, BR_SpecialParity,
	/*  55 */	0xE0, BR_SpecialParity,
	/*  56 */	0xE0, BR_SpecialParity,
	/*  57 */	0xE0, BR_SpecialParity,
	/*  58 */	0xE0, BR_SpecialParity,
	/*  59 */	0xE0, BR_SpecialParity,
	/*  60 */	0xE0, BR_SpecialParity,
	/*  61 */	0xE0, BR_SpecialParity,
	/*  62 */	0xE0, BR_SpecialParity,
	/*  63 */	0xE0, BR_SpecialParity,
	/*  64 */	0xE0, BR_SpecialParity,
	/*  65 */	0xE0, BR_SpecialParity,
	/*  66 */	0xE0, BR_SpecialParity,
	/*  67 */	0xE0, BR_SpecialParity,
	/*  68 */	0xE0, BR_SpecialParity,
	/*  69 */	0xE0, BR_SpecialParity,
	/*  70 */	0xE0, BR_SpecialParity,
	/*  71 */	0xE0, BR_SpecialParity,
	/*  72 */	0xE0, BR_SpecialParity,
	/*  73 */	0xE0, BR_SpecialParity,
	/*  74 */	0xE0, BR_SpecialParity,
	/*  75 */	0xE0, BR_SpecialParity,
	/*  76 */	0xE0, BR_SpecialParity,
	/*  77 */	0xE0, BR_SpecialParity,
	/*  78 */	0xE0, BR_SpecialParity,
	/*  79 */	0xE0, BR_SpecialParity,
	/*  80 */	0xE0, BR_SpecialParity,
	/*  81 */	0xE0, BR_SpecialParity,
	/*  82 */	0xE0, BR_SpecialParity,
	/*  83 */	0xE0, BR_SpecialParity,
	/*  84 */	0xE0, BR_SpecialParity,
	/*  85 */	0xE0, BR_SpecialParity,
	/*  86 */	0xE0, BR_SpecialParity,
	/*  87 */	0xE0, BR_SpecialParity,
	/*  88 */	0xE0, BR_SpecialParity,
	/*  89 */	0xE0, BR_SpecialParity,
	/*  90 */	0xE0, BR_SpecialParity,
	/*  91 */	0xE0, BR_SpecialParity,
	/*  92 */	0xE0, BR_SpecialParity,
	/*  93 */	0xE0, BR_SpecialParity,
	/*  94 */	0xE0, BR_SpecialParity,
	/*  95 */	0xE0, BR_SpecialParity,
	/*  96 */	0xE0, BR_SpecialParity,
	/*  97 */	0xE0, BR_SpecialParity,
	/*  98 */	0xE0, BR_SpecialParity,
	/*  99 */	0xE0, BR_SpecialParity,
	/* 100 */	0xE0, BR_SpecialParity,
	/* 101 */	0xE0, BR_SpecialParity,
	/* 102 */	0xE0, BR_SpecialParity,
	/* 103 */	0xE0, BR_SpecialParity,
	/* 104 */	0xE0, BR_SpecialParity,
	/* 105 */	0xE0, BR_SpecialParity,
	/* 106 */	0xE0, BR_SpecialParity,
	/* 107 */	0xE0, BR_SpecialParity,
	/* 108 */	0xE0, BR_SpecialParity,
	/* 109 */	0xE0, BR_SpecialParity,
	/* 110 */	0xE0, BR_SpecialParity,
	/* 111 */	0xE0, BR_SpecialParity,
	/* 112 */	0xE0, BR_SpecialParity,
	/* 113 */	0xE0, BR_SpecialParity,
	/* 114 */	0xE0, BR_SpecialParity,
	/* 115 */	0xE0, BR_SpecialParity,
	/* 116 */	0xE0, BR_SpecialParity,
	/* 117 */	0xE0, BR_SpecialParity,
	/* 118 */	0xE0, BR_SpecialParity,
	/* 119 */	0xE0, BR_SpecialParity,
	/* 120 */	0xE0, BR_SpecialParity,
	/* 121 */	0xE0, BR_SpecialParity,
	/* 122 */	0xE0, BR_SpecialParity,
	/* 123 */	0xE0, BR_SpecialParity,
	/* 124 */	0xE0, BR_SpecialParity,
	/* 125 */	0xE0, BR_SpecialParity,
	/* 126 */	0xE0, BR_SpecialParity,
	/* 127 */	0xE0, BR_SpecialParity,
	/* 128 */	0xE0, BR_SpecialParity,
	/* 129 */	0xE0, BR_SpecialParity,
	/* 130 */	0xE0, BR_SpecialParity,
	/* 131 */	0xE0, BR_SpecialParity,
	/* 132 */	0xE0, BR_SpecialParity,
	/* 133 */	0xE0, BR_SpecialParity,
	/* 134 */	0xE0, BR_SpecialParity,
	/* 135 */	0xE0, BR_SpecialParity,
	/* 136 */	0xE0, BR_SpecialParity,
	/* 137 */	0xE0, BR_SpecialParity,
	/* 138 */	0xE0, BR_SpecialParity,
	/* 139 */	0xE0, BR_SpecialParity,
	/* 140 */	0xE0, BR_SpecialParity,
	/* 141 */	0xE0, BR_SpecialParity,
	/* 142 */	0xE0, BR_SpecialParity,
	/* 143 */	0xE0, BR_SpecialParity,
	/* 144 */	0xE0, BR_SpecialParity,
	/* 145 */	0xE0, BR_SpecialParity,
	/* 146 */	0xE0, BR_SpecialParity,
	/* 147 */	0xE0, BR_SpecialParity,
	/* 148 */	0xE0, BR_SpecialParity,
	/* 149 */	0xE0, BR_SpecialParity
};

static const CMakeBreaks NEC_98_MKBR =
{
	/*			pre byte, type */
	/*   0 */	0x00, BR_HighBit,
	/*   1 */	0x00, BR_HighBit,
	/*   2 */	0x00, BR_MakeOnly,
	/*   3 */	0x00, BR_MakeOnly,
	/*   4 */	0x00, BR_HighBit,
	/*   5 */	0x00, BR_HighBit,
	/*   6 */	0x00, BR_HighBit,
	/*   7 */	0x00, BR_HighBit,
	/*   8 */	0x00, BR_HighBit,
	/*   9 */	0x00, BR_HighBit,
	/*  10 */	0x00, BR_HighBit,
	/*  11 */	0x00, BR_HighBit,
	/*  12 */	0x00, BR_HighBit,
	/*  13 */	0x00, BR_HighBit,
	/*  14 */	0x00, BR_HighBit,
	/*  15 */	0x00, BR_HighBit,
	/*  16 */	0x00, BR_HighBit,
	/*  17 */	0x00, BR_HighBit,
	/*  18 */	0x00, BR_HighBit,
	/*  19 */	0x00, BR_HighBit,
	/*  20 */	0x00, BR_HighBit,
	/*  21 */	0x00, BR_HighBit,
	/*  22 */	0x00, BR_HighBit,
	/*  23 */	0x00, BR_HighBit,
	/*  24 */	0x00, BR_HighBit,
	/*  25 */	0x00, BR_HighBit,
	/*  26 */	0x00, BR_HighBit,
	/*  27 */	0x00, BR_HighBit,
	/*  28 */	0x00, BR_HighBit,
	/*  29 */	0x00, BR_HighBit,
	/*  30 */	0x00, BR_HighBit,
	/*  31 */	0x00, BR_HighBit,
	/*  32 */	0x00, BR_HighBit,
	/*  33 */	0x00, BR_HighBit,
	/*  34 */	0x00, BR_HighBit,
	/*  35 */	0x00, BR_HighBit,
	/*  36 */	0x00, BR_HighBit,
	/*  37 */	0x00, BR_HighBit,
	/*  38 */	0x00, BR_HighBit,
	/*  39 */	0x00, BR_HighBit,
	/*  40 */	0x00, BR_HighBit,
	/*  41 */	0x00, BR_HighBit,
	/*  42 */	0x00, BR_HighBit,
	/*  43 */	0x00, BR_HighBit,
	/*  44 */	0x00, BR_HighBit,
	/*  45 */	0x00, BR_HighBit,
	/*  46 */	0x00, BR_HighBit,
	/*  47 */	0x00, BR_HighBit,
	/*  48 */	0x00, BR_HighBit,
	/*  49 */	0x00, BR_HighBit,
	/*  50 */	0x00, BR_HighBit,
	/*  51 */	0x00, BR_HighBit,
	/*  52 */	0x00, BR_HighBit,
	/*  53 */	0x00, BR_HighBit,
	/*  54 */	0x00, BR_HighBit,
	/*  55 */	0x00, BR_HighBit,
	/*  56 */	0x00, BR_HighBit,
	/*  57 */	0x00, BR_HighBit,
	/*  58 */	0x00, BR_HighBit,
	/*  59 */	0x00, BR_HighBit,
	/*  60 */	0x00, BR_HighBit,
	/*  61 */	0x00, BR_HighBit,
	/*  62 */	0x00, BR_HighBit,
	/*  63 */	0x00, BR_HighBit,
	/*  64 */	0x00, BR_HighBit,
	/*  65 */	0x00, BR_HighBit,
	/*  66 */	0x00, BR_HighBit,
	/*  67 */	0x00, BR_HighBit,
	/*  68 */	0x00, BR_HighBit,
	/*  69 */	0x00, BR_HighBit,
	/*  70 */	0x00, BR_HighBit,
	/*  71 */	0x00, BR_HighBit,
	/*  72 */	0x00, BR_HighBit,
	/*  73 */	0x00, BR_HighBit,
	/*  74 */	0x00, BR_HighBit,
	/*  75 */	0x00, BR_HighBit,
	/*  76 */	0x00, BR_HighBit,
	/*  77 */	0x00, BR_HighBit,
	/*  78 */	0x00, BR_HighBit,
	/*  79 */	0x00, BR_HighBit,
	/*  80 */	0x00, BR_HighBit,
	/*  81 */	0x00, BR_HighBit,
	/*  82 */	0x00, BR_HighBit,
	/*  83 */	0x00, BR_HighBit,
	/*  84 */	0x00, BR_HighBit,
	/*  85 */	0x00, BR_HighBit,
	/*  86 */	0x00, BR_HighBit,
	/*  87 */	0x00, BR_HighBit,
	/*  88 */	0x00, BR_HighBit,
	/*  89 */	0x00, BR_HighBit,
	/*  90 */	0x00, BR_HighBit,
	/*  91 */	0x00, BR_HighBit,
	/*  92 */	0x00, BR_HighBit,
	/*  93 */	0x00, BR_HighBit,
	/*  94 */	0x00, BR_HighBit,
	/*  95 */	0x00, BR_HighBit,
	/*  96 */	0x00, BR_HighBit,
	/*  97 */	0x00, BR_HighBit,
	/*  98 */	0x00, BR_HighBit,
	/*  99 */	0x00, BR_HighBit,
	/* 100 */	0x00, BR_HighBit,
	/* 101 */	0x00, BR_HighBit,
	/* 102 */	0x00, BR_HighBit,
	/* 103 */	0x00, BR_HighBit,
	/* 104 */	0x00, BR_HighBit,
	/* 105 */	0x00, BR_HighBit,
	/* 106 */	0x00, BR_HighBit,
	/* 107 */	0x00, BR_HighBit,
	/* 108 */	0x00, BR_HighBit,
	/* 109 */	0x00, BR_HighBit,
	/* 110 */	0x00, BR_HighBit,
	/* 111 */	0x00, BR_HighBit,
	/* 112 */	0x00, BR_HighBit,
	/* 113 */	0x00, BR_HighBit,
	/* 114 */	0x00, BR_HighBit,
	/* 115 */	0x00, BR_HighBit,
	/* 116 */	0x00, BR_HighBit,
	/* 117 */	0x00, BR_HighBit,
	/* 118 */	0x00, BR_HighBit,
	/* 119 */	0x00, BR_HighBit,
	/* 120 */	0x00, BR_HighBit,
	/* 121 */	0x00, BR_HighBit,
	/* 122 */	0x00, BR_HighBit,
	/* 123 */	0x00, BR_HighBit,
	/* 124 */	0x00, BR_HighBit,
	/* 125 */	0x00, BR_HighBit,
	/* 126 */	0x00, BR_HighBit,
	/* 127 */	0x00, BR_HighBit,
	/* 128 */	0x00, BR_HighBit,
	/* 129 */	0x00, BR_HighBit,
	/* 130 */	0x00, BR_HighBit,
	/* 131 */	0x00, BR_HighBit,
	/* 132 */	0x00, BR_HighBit,
	/* 133 */	0x00, BR_HighBit,
	/* 134 */	0x00, BR_HighBit,
	/* 135 */	0x00, BR_HighBit,
	/* 136 */	0x00, BR_HighBit,
	/* 137 */	0x00, BR_HighBit,
	/* 138 */	0x00, BR_HighBit,
	/* 139 */	0x00, BR_HighBit,
	/* 140 */	0x00, BR_HighBit,
	/* 141 */	0x00, BR_HighBit,
	/* 142 */	0x00, BR_HighBit,
	/* 143 */	0x00, BR_HighBit,
	/* 144 */	0x00, BR_HighBit,
	/* 145 */	0x00, BR_HighBit,
	/* 146 */	0x00, BR_HighBit,
	/* 147 */	0x00, BR_HighBit,
	/* 148 */	0x00, BR_HighBit,
	/* 149 */	0x00, BR_HighBit
};

static const CMakeBreaks BURROUGHS_K4_MKBR =
{
	/*			pre byte, type */
	/*   0 */	0x00, BR_MakeOnly,
	/*   1 */	0x00, BR_MakeOnly,
	/*   2 */	0x00, BR_MakeOnly,
	/*   3 */	0x00, BR_MakeOnly,
	/*   4 */	0x00, BR_MakeOnly,
	/*   5 */	0x00, BR_MakeOnly,
	/*   6 */	0x00, BR_MakeOnly,
	/*   7 */	0x00, BR_MakeOnly,
	/*   8 */	0x00, BR_MakeOnly,
	/*   9 */	0x00, BR_MakeOnly,
	/*  10 */	0x00, BR_MakeOnly,
	/*  11 */	0x00, BR_MakeOnly,
	/*  12 */	0x00, BR_MakeOnly,
	/*  13 */	0x00, BR_MakeOnly,
	/*  14 */	0x00, BR_MakeOnly,
	/*  15 */	0x00, BR_MakeOnly,
	/*  16 */	0x00, BR_MakeOnly,
	/*  17 */	0x00, BR_MakeOnly,
	/*  18 */	0x00, BR_MakeOnly,
	/*  19 */	0x00, BR_MakeOnly,
	/*  20 */	0x00, BR_MakeOnly,
	/*  21 */	0x00, BR_MakeOnly,
	/*  22 */	0x00, BR_MakeOnly,
	/*  23 */	0x00, BR_MakeOnly,
	/*  24 */	0x00, BR_MakeOnly,
	/*  25 */	0x00, BR_MakeOnly,
	/*  26 */	0x00, BR_MakeOnly,
	/*  27 */	0x00, BR_MakeOnly,
	/*  28 */	0x00, BR_MakeOnly,
	/*  29 */	0x00, BR_MakeOnly,
	/*  30 */	0x00, BR_MakeOnly,
	/*  31 */	0x00, BR_MakeOnly,
	/*  32 */	0x00, BR_MakeOnly,
	/*  33 */	0x00, BR_MakeOnly,
	/*  34 */	0x00, BR_MakeOnly,
	/*  35 */	0x00, BR_MakeOnly,
	/*  36 */	0x00, BR_MakeOnly,
	/*  37 */	0x00, BR_MakeOnly,
	/*  38 */	0x00, BR_MakeOnly,
	/*  39 */	0x00, BR_MakeOnly,
	/*  40 */	0x00, BR_MakeOnly,
	/*  41 */	0x00, BR_MakeOnly,
	/*  42 */	0x00, BR_MakeOnly,
	/*  43 */	0x00, BR_MakeOnly,
	/*  44 */	0xb3, BR_MakeOnly,
	/*  45 */	0x00, BR_MakeOnly,
	/*  46 */	0x00, BR_MakeOnly,
	/*  47 */	0x00, BR_MakeOnly,
	/*  48 */	0x00, BR_MakeOnly,
	/*  49 */	0x00, BR_MakeOnly,
	/*  50 */	0x00, BR_MakeOnly,
	/*  51 */	0x00, BR_MakeOnly,
	/*  52 */	0x00, BR_MakeOnly,
	/*  53 */	0x00, BR_MakeOnly,
	/*  54 */	0x00, BR_MakeOnly,
	/*  55 */	0x00, BR_MakeOnly,
	/*  56 */	0x00, BR_MakeOnly,
	/*  57 */	0xb3, BR_MakeOnly,
	/*  58 */	0xb3, BR_MakeOnly,
	/*  59 */	0xb3, BR_MakeOnly,
	/*  60 */	0x00, BR_MakeOnly,
	/*  61 */	0x00, BR_MakeOnly,
	/*  62 */	0x00, BR_MakeOnly,
	/*  63 */	0x00, BR_MakeOnly,
	/*  64 */	0x00, BR_MakeOnly,
	/*  65 */	0x00, BR_MakeOnly,
	/*  66 */	0x00, BR_MakeOnly,
	/*  67 */	0x00, BR_MakeOnly,
	/*  68 */	0x00, BR_MakeOnly,
	/*  69 */	0x00, BR_MakeOnly,
	/*  70 */	0x00, BR_MakeOnly,
	/*  71 */	0x00, BR_MakeOnly,
	/*  72 */	0x00, BR_MakeOnly,
	/*  73 */	0x00, BR_MakeOnly,
	/*  74 */	0x00, BR_MakeOnly,
	/*  75 */	0x00, BR_MakeOnly,
	/*  76 */	0x00, BR_MakeOnly,
	/*  77 */	0x00, BR_MakeOnly,
	/*  78 */	0x00, BR_MakeOnly,
	/*  79 */	0x00, BR_MakeOnly,
	/*  80 */	0x00, BR_MakeOnly,
	/*  81 */	0x00, BR_MakeOnly,
	/*  82 */	0x00, BR_MakeOnly,
	/*  83 */	0x00, BR_MakeOnly,
	/*  84 */	0x00, BR_MakeOnly,
	/*  85 */	0x00, BR_MakeOnly,
	/*  86 */	0x00, BR_MakeOnly,
	/*  87 */	0x00, BR_MakeOnly,
	/*  88 */	0x00, BR_MakeOnly,
	/*  89 */	0x00, BR_MakeOnly,
	/*  90 */	0x00, BR_MakeOnly,
	/*  91 */	0x00, BR_MakeOnly,
	/*  92 */	0x00, BR_MakeOnly,
	/*  93 */	0x00, BR_MakeOnly,
	/*  94 */	0x00, BR_MakeOnly,
	/*  95 */	0x00, BR_MakeOnly,
	/*  96 */	0x00, BR_MakeOnly,
	/*  97 */	0x00, BR_MakeOnly,
	/*  98 */	0x00, BR_MakeOnly,
	/*  99 */	0x00, BR_MakeOnly,
	/* 100 */	0x00, BR_MakeOnly,
	/* 101 */	0x00, BR_MakeOnly,
	/* 102 */	0x00, BR_MakeOnly,
	/* 103 */	0x00, BR_MakeOnly,
	/* 104 */	0x00, BR_MakeOnly,
	/* 105 */	0x00, BR_MakeOnly,
	/* 106 */	0x00, BR_MakeOnly,
	/* 107 */	0x00, BR_MakeOnly,
	/* 108 */	0x00, BR_MakeOnly,
	/* 109 */	0x00, BR_MakeOnly,
	/* 110 */	0x00, BR_MakeOnly,
	/* 111 */	0x00, BR_MakeOnly,
	/* 112 */	0x00, BR_MakeOnly,
	/* 113 */	0x00, BR_MakeOnly,
	/* 114 */	0x00, BR_MakeOnly,
	/* 115 */	0x00, BR_MakeOnly,
	/* 116 */	0x00, BR_MakeOnly,
	/* 117 */	0x00, BR_MakeOnly,
	/* 118 */	0x00, BR_MakeOnly,
	/* 119 */	0x00, BR_MakeOnly,
	/* 120 */	0x00, BR_MakeOnly,
	/* 121 */	0x00, BR_MakeOnly,
	/* 122 */	0x00, BR_MakeOnly,
	/* 123 */	0x00, BR_MakeOnly,
	/* 124 */	0x00, BR_MakeOnly,
	/* 125 */	0x00, BR_MakeOnly,
	/* 126 */	0x00, BR_MakeOnly,
	/* 127 */	0x00, BR_MakeOnly,
	/* 128 */	0x00, BR_MakeOnly,
	/* 129 */	0x00, BR_MakeOnly,
	/* 130 */	0x00, BR_MakeOnly,
	/* 131 */	0x00, BR_MakeOnly,
	/* 132 */	0x00, BR_MakeOnly,
	/* 133 */	0x00, BR_MakeOnly,
	/* 134 */	0x00, BR_MakeOnly,
	/* 135 */	0x00, BR_MakeOnly,
	/* 136 */	0x00, BR_MakeOnly,
	/* 137 */	0x00, BR_MakeOnly,
	/* 138 */	0x00, BR_MakeOnly,
	/* 139 */	0x00, BR_MakeOnly,
	/* 140 */	0x00, BR_MakeOnly,
	/* 141 */	0x00, BR_MakeOnly,
	/* 142 */	0x00, BR_MakeOnly,
	/* 143 */	0x00, BR_MakeOnly,
	/* 144 */	0x00, BR_MakeOnly,
	/* 145 */	0x00, BR_MakeOnly,
	/* 146 */	0x00, BR_MakeOnly,
	/* 147 */	0x00, BR_MakeOnly,
	/* 148 */	0x00, BR_MakeOnly,
	/* 149 */	0x00, BR_MakeOnly
};

static const CMakeBreaks BURROUGHS_K5_MKBR =
{
	/*			pre byte, type */
	/*   0 */	0x00, BR_MakeOnly,
	/*   1 */	0x00, BR_MakeOnly,
	/*   2 */	0x00, BR_MakeOnly,
	/*   3 */	0x00, BR_MakeOnly,
	/*   4 */	0x00, BR_MakeOnly,
	/*   5 */	0x00, BR_MakeOnly,
	/*   6 */	0x00, BR_MakeOnly,
	/*   7 */	0x00, BR_MakeOnly,
	/*   8 */	0x00, BR_MakeOnly,
	/*   9 */	0x00, BR_MakeOnly,
	/*  10 */	0x00, BR_MakeOnly,
	/*  11 */	0x00, BR_MakeOnly,
	/*  12 */	0x00, BR_MakeOnly,
	/*  13 */	0x00, BR_MakeOnly,
	/*  14 */	0x00, BR_MakeOnly,
	/*  15 */	0x00, BR_MakeOnly,
	/*  16 */	0x00, BR_MakeOnly,
	/*  17 */	0x00, BR_MakeOnly,
	/*  18 */	0x00, BR_MakeOnly,
	/*  19 */	0x00, BR_MakeOnly,
	/*  20 */	0x00, BR_MakeOnly,
	/*  21 */	0x00, BR_MakeOnly,
	/*  22 */	0x00, BR_MakeOnly,
	/*  23 */	0x00, BR_MakeOnly,
	/*  24 */	0x00, BR_MakeOnly,
	/*  25 */	0x00, BR_MakeOnly,
	/*  26 */	0x00, BR_MakeOnly,
	/*  27 */	0x00, BR_MakeOnly,
	/*  28 */	0x00, BR_MakeOnly,
	/*  29 */	0x00, BR_MakeOnly,
	/*  30 */	0x00, BR_MakeOnly,
	/*  31 */	0x00, BR_MakeOnly,
	/*  32 */	0x00, BR_MakeOnly,
	/*  33 */	0x00, BR_MakeOnly,
	/*  34 */	0x00, BR_MakeOnly,
	/*  35 */	0x00, BR_MakeOnly,
	/*  36 */	0x00, BR_MakeOnly,
	/*  37 */	0x00, BR_MakeOnly,
	/*  38 */	0x00, BR_MakeOnly,
	/*  39 */	0x00, BR_MakeOnly,
	/*  40 */	0x00, BR_MakeOnly,
	/*  41 */	0x00, BR_MakeOnly,
	/*  42 */	0x00, BR_MakeOnly,
	/*  43 */	0x00, BR_MakeOnly,
	/*  44 */	0xb3, BR_MakeOnly,
	/*  45 */	0x00, BR_MakeOnly,
	/*  46 */	0x00, BR_MakeOnly,
	/*  47 */	0x00, BR_MakeOnly,
	/*  48 */	0x00, BR_MakeOnly,
	/*  49 */	0x00, BR_MakeOnly,
	/*  50 */	0x00, BR_MakeOnly,
	/*  51 */	0x00, BR_MakeOnly,
	/*  52 */	0x00, BR_MakeOnly,
	/*  53 */	0x00, BR_MakeOnly,
	/*  54 */	0x00, BR_MakeOnly,
	/*  55 */	0x00, BR_MakeOnly,
	/*  56 */	0x00, BR_MakeOnly,
	/*  57 */	0xb3, BR_MakeOnly,
	/*  58 */	0xb3, BR_MakeOnly,
	/*  59 */	0xb3, BR_MakeOnly,
	/*  60 */	0x00, BR_MakeOnly,
	/*  61 */	0x00, BR_MakeOnly,
	/*  62 */	0x00, BR_MakeOnly,
	/*  63 */	0x00, BR_MakeOnly,
	/*  64 */	0x00, BR_MakeOnly,
	/*  65 */	0x00, BR_MakeOnly,
	/*  66 */	0x00, BR_MakeOnly,
	/*  67 */	0x00, BR_MakeOnly,
	/*  68 */	0x00, BR_MakeOnly,
	/*  69 */	0x00, BR_MakeOnly,
	/*  70 */	0x00, BR_MakeOnly,
	/*  71 */	0x00, BR_MakeOnly,
	/*  72 */	0x00, BR_MakeOnly,
	/*  73 */	0x00, BR_MakeOnly,
	/*  74 */	0x00, BR_MakeOnly,
	/*  75 */	0x00, BR_MakeOnly,
	/*  76 */	0x00, BR_MakeOnly,
	/*  77 */	0x00, BR_MakeOnly,
	/*  78 */	0x00, BR_MakeOnly,
	/*  79 */	0x00, BR_MakeOnly,
	/*  80 */	0x00, BR_MakeOnly,
	/*  81 */	0x00, BR_MakeOnly,
	/*  82 */	0x00, BR_MakeOnly,
	/*  83 */	0x00, BR_MakeOnly,
	/*  84 */	0x00, BR_MakeOnly,
	/*  85 */	0x00, BR_MakeOnly,
	/*  86 */	0x00, BR_MakeOnly,
	/*  87 */	0x00, BR_MakeOnly,
	/*  88 */	0x00, BR_MakeOnly,
	/*  89 */	0x00, BR_MakeOnly,
	/*  90 */	0x00, BR_MakeOnly,
	/*  91 */	0x00, BR_MakeOnly,
	/*  92 */	0x00, BR_MakeOnly,
	/*  93 */	0x00, BR_MakeOnly,
	/*  94 */	0x00, BR_MakeOnly,
	/*  95 */	0x00, BR_MakeOnly,
	/*  96 */	0x00, BR_MakeOnly,
	/*  97 */	0x00, BR_MakeOnly,
	/*  98 */	0x00, BR_MakeOnly,
	/*  99 */	0x00, BR_MakeOnly,
	/* 100 */	0x00, BR_MakeOnly,
	/* 101 */	0x00, BR_MakeOnly,
	/* 102 */	0x00, BR_MakeOnly,
	/* 103 */	0x00, BR_MakeOnly,
	/* 104 */	0x00, BR_MakeOnly,
	/* 105 */	0x00, BR_MakeOnly,
	/* 106 */	0x00, BR_MakeOnly,
	/* 107 */	0x00, BR_MakeOnly,
	/* 108 */	0x00, BR_MakeOnly,
	/* 109 */	0x00, BR_MakeOnly,
	/* 110 */	0x00, BR_MakeOnly,
	/* 111 */	0x00, BR_MakeOnly,
	/* 112 */	0x00, BR_MakeOnly,
	/* 113 */	0x00, BR_MakeOnly,
	/* 114 */	0x00, BR_MakeOnly,
	/* 115 */	0x00, BR_MakeOnly,
	/* 116 */	0x00, BR_MakeOnly,
	/* 117 */	0x00, BR_MakeOnly,
	/* 118 */	0x00, BR_MakeOnly,
	/* 119 */	0x00, BR_MakeOnly,
	/* 120 */	0x00, BR_MakeOnly,
	/* 121 */	0x00, BR_MakeOnly,
	/* 122 */	0x00, BR_MakeOnly,
	/* 123 */	0x00, BR_MakeOnly,
	/* 124 */	0x00, BR_MakeOnly,
	/* 125 */	0x00, BR_MakeOnly,
	/* 126 */	0x00, BR_MakeOnly,
	/* 127 */	0x00, BR_MakeOnly,
	/* 128 */	0x00, BR_MakeOnly,
	/* 129 */	0x00, BR_MakeOnly,
	/* 130 */	0x00, BR_MakeOnly,
	/* 131 */	0x00, BR_MakeOnly,
	/* 132 */	0x00, BR_MakeOnly,
	/* 133 */	0x00, BR_MakeOnly,
	/* 134 */	0x00, BR_MakeOnly,
	/* 135 */	0x00, BR_MakeOnly,
	/* 136 */	0x00, BR_MakeOnly,
	/* 137 */	0x00, BR_MakeOnly,
	/* 138 */	0x00, BR_MakeOnly,
	/* 139 */	0x00, BR_MakeOnly,
	/* 140 */	0x00, BR_MakeOnly,
	/* 141 */	0x00, BR_MakeOnly,
	/* 142 */	0x00, BR_MakeOnly,
	/* 143 */	0x00, BR_MakeOnly,
	/* 144 */	0x00, BR_MakeOnly,
	/* 145 */	0x00, BR_MakeOnly,
	/* 146 */	0x00, BR_MakeOnly,
	/* 147 */	0x00, BR_MakeOnly,
	/* 148 */	0x00, BR_MakeOnly,
	/* 149 */	0x00, BR_MakeOnly
};


/******************************************************/
/* ASCII to Keynum translation tables                 */
/******************************************************/
extern const keynum_t AT_NumPad[];
extern const keynum_t DE_NumPad[];

static const CChar2Key CTOSUNISYS_KB100_Layout =
{
	/* NumPad   */	AT_NumPad,
	/* Max ANSI */	127,

	/*       	  KN, MODIFIER */
	/* 0x00 */	   0, 0,
	/* 0x01 */	   0, 0,
	/* 0x02 */	   0, 0,
	/* 0x03 */	   0, 0,
	/* 0x04 */	   0, 0,
	/* 0x05 */	   0, 0,
	/* 0x06 */	   0, 0,
	/* 0x07 */	   0, 0,
	/* 0x08 */	   0, 0,
	/* 0x09 */	   0, 0,
	/* 0x0a */	   0, 0,
	/* 0x0b */	   0, 0,
	/* 0x0c */	   0, 0,
	/* 0x0d */	  43, 0,
	/* 0x0e */	  75, 0,
	/* 0x0f */	  79, 0,
	/* 0x10 */	  89, 0,
	/* 0x11 */	  84, 0,
	/* 0x12 */	  83, 0,
	/* 0x13 */	   0, 0,
	/* 0x14 */	   0, 0,
	/* 0x15 */	   0, 0,
	/* 0x16 */	   0, 0,
	/* 0x17 */	   0, 0,
	/* 0x18 */	   0, 0,
	/* 0x19 */	   0, 0,
	/* 0x1a */	   0, 0,
	/* 0x1b */	   0, 0,
	/* 0x1c */	   0, 0,
	/* 0x1d */	   0, 0,
	/* 0x1e */	   0, 0,
	/* 0x1f */	   0, 0,
	/* ' '  */	   0, 0,
	/* '!'  */	   0, 0,
	/* '"'  */	   0, 0,
	/* '#'  */	   0, 0,
	/* '$'  */	   0, 0,
	/* '%'  */	   0, 0,
	/* '&'  */	   0, 0,
	/* '''  */	   0, 0,
	/* '('  */	   0, 0,
	/* ')'  */	   0, 0,
	/* '*'  */	   0, 0,
	/* '+'  */	   0, 0,
	/* ','  */	   0, 0,
	/* '-'  */	   0, 0,
	/* '.'  */	   0, 0,
	/* '/'  */	   0, 0,
	/* '0'  */	  11, 0,
	/* '1'  */	   2, 0,
	/* '2'  */	   3, 0,
	/* '3'  */	   4, 0,
	/* '4'  */	   5, 0,
	/* '5'  */	   6, 0,
	/* '6'  */	   7, 0,
	/* '7'  */	   8, 0,
	/* '8'  */	   9, 0,
	/* '9'  */	  10, 0,
	/* ':'  */	   0, 0,
	/* ';'  */	   0, 0,
	/* '<'  */	   0, 0,
	/* '='  */	   0, 0,
	/* '>'  */	   0, 0,
	/* '?'  */	   0, 0,
	/* '@'  */	   0, 0,
	/* 'A'  */	   0, 0,
	/* 'B'  */	   0, 0,
	/* 'C'  */	   0, 0,
	/* 'D'  */	   0, 0,
	/* 'E'  */	   0, 0,
	/* 'F'  */	   0, 0,
	/* 'G'  */	   0, 0,
	/* 'H'  */	   0, 0,
	/* 'I'  */	   0, 0,
	/* 'J'  */	   0, 0,
	/* 'K'  */	   0, 0,
	/* 'L'  */	   0, 0,
	/* 'M'  */	   0, 0,
	/* 'N'  */	   0, 0,
	/* 'O'  */	   0, 0,
	/* 'P'  */	   0, 0,
	/* 'Q'  */	   0, 0,
	/* 'R'  */	   0, 0,
	/* 'S'  */	   0, 0,
	/* 'T'  */	   0, 0,
	/* 'U'  */	   0, 0,
	/* 'V'  */	   0, 0,
	/* 'W'  */	   0, 0,
	/* 'X'  */	   0, 0,
	/* 'Y'  */	   0, 0,
	/* 'Z'  */	   0, 0,
	/* '['  */	   0, 0,
	/* '\'  */	   0, 0,
	/* ']'  */	   0, 0,
	/* '^'  */	   0, 0,
	/* '_'  */	   0, 0,
	/* '`'  */	   0, 0,
	/* 'a'  */	   0, 0,
	/* 'b'  */	   0, 0,
	/* 'c'  */	   0, 0,
	/* 'd'  */	   0, 0,
	/* 'e'  */	   0, 0,
	/* 'f'  */	   0, 0,
	/* 'g'  */	   0, 0,
	/* 'h'  */	   0, 0,
	/* 'i'  */	   0, 0,
	/* 'j'  */	   0, 0,
	/* 'k'  */	   0, 0,
	/* 'l'  */	   0, 0,
	/* 'm'  */	   0, 0,
	/* 'n'  */	   0, 0,
	/* 'o'  */	   0, 0,
	/* 'p'  */	   0, 0,
	/* 'q'  */	   0, 0,
	/* 'r'  */	   0, 0,
	/* 's'  */	   0, 0,
	/* 't'  */	   0, 0,
	/* 'u'  */	   0, 0,
	/* 'v'  */	   0, 0,
	/* 'w'  */	   0, 0,
	/* 'x'  */	   0, 0,
	/* 'y'  */	   0, 0,
	/* 'z'  */	   0, 0,
	/* '{'  */	   0, 0,
	/* '|'  */	   0, 0,
	/* '}'  */	   0, 0,
	/* '~'  */	   0, 0,
	/* 0x7f */	   0, 0
};

static const CChar2Key CTOSUNISYS_K4_Layout =
{
	/* NumPad   */	AT_NumPad,
	/* Max ANSI */	127,

	/*       	  KN, MODIFIER */
	/* 0x00 */	   0, 0,
	/* 0x01 */	   0, 0,
	/* 0x02 */	   0, 0,
	/* 0x03 */	   0, 0,
	/* 0x04 */	   0, 0,
	/* 0x05 */	   0, 0,
	/* 0x06 */	   0, 0,
	/* 0x07 */	   0, 0,
	/* 0x08 */	   0, 0,
	/* 0x09 */	   0, 0,
	/* 0x0a */	   0, 0,
	/* 0x0b */	   0, 0,
	/* 0x0c */	   0, 0,
	/* 0x0d */	   43, 0,
	/* 0x0e */	   75, 0,
	/* 0x0f */	   79, 0,
	/* 0x10 */	   89, 0,
	/* 0x11 */	   84, 0,
	/* 0x12 */	   83, 0,
	/* 0x13 */	   0, 0,
	/* 0x14 */	   0, 0,
	/* 0x15 */	   0, 0,
	/* 0x16 */	   0, 0,
	/* 0x17 */	   0, 0,
	/* 0x18 */	   0, 0,
	/* 0x19 */	   0, 0,
	/* 0x1a */	   0, 0,
	/* 0x1b */	   0, 0,
	/* 0x1c */	   0, 0,
	/* 0x1d */	   0, 0,
	/* 0x1e */	   0, 0,
	/* 0x1f */	   0, 0,
	/* ' '  */	  61, 0,
	/* '!'  */	   0, 0,
	/* '"'  */	   0, 0,
	/* '#'  */	   0, 0,
	/* '$'  */	   0, 0,
	/* '%'  */	   0, 0,
	/* '&'  */	   0, 0,
	/* '''  */	   0, 0,
	/* '('  */	   0, 0,
	/* ')'  */	   0, 0,
	/* '*'  */	   0, 0,
	/* '+'  */	   0, 0,
	/* ','  */	   0, 0,
	/* '-'  */	   0, 0,
	/* '.'  */	  54, 0,
	/* '/'  */	   0, 0,
	/* '0'  */	  11, 0,
	/* '1'  */	   2, 0,
	/* '2'  */	   3, 0,
	/* '3'  */	   4, 0,
	/* '4'  */	   5, 0,
	/* '5'  */	   6, 0,
	/* '6'  */	   7, 0,
	/* '7'  */	   8, 0,
	/* '8'  */	   9, 0,
	/* '9'  */	  10, 0,
	/* ':'  */	   0, 0,
	/* ';'  */	   0, 0,
	/* '<'  */	   0, 0,
	/* '='  */	   0, 0,
	/* '>'  */	   0, 0,
	/* '?'  */	   0, 0,
	/* '@'  */	   0, 0,
	/* 'A'  */	   0, 0,
	/* 'B'  */	   0, 0,
	/* 'C'  */	   0, 0,
	/* 'D'  */	   0, 0,
	/* 'E'  */	   0, 0,
	/* 'F'  */	   0, 0,
	/* 'G'  */	   0, 0,
	/* 'H'  */	   0, 0,
	/* 'I'  */	   0, 0,
	/* 'J'  */	   0, 0,
	/* 'K'  */	   0, 0,
	/* 'L'  */	   0, 0,
	/* 'M'  */	   0, 0,
	/* 'N'  */	   0, 0,
	/* 'O'  */	   0, 0,
	/* 'P'  */	   0, 0,
	/* 'Q'  */	   0, 0,
	/* 'R'  */	   0, 0,
	/* 'S'  */	   0, 0,
	/* 'T'  */	   0, 0,
	/* 'U'  */	   0, 0,
	/* 'V'  */	   0, 0,
	/* 'W'  */	   0, 0,
	/* 'X'  */	   0, 0,
	/* 'Y'  */	   0, 0,
	/* 'Z'  */	   0, 0,
	/* '['  */	   0, 0,
	/* '\'  */	   0, 0,
	/* ']'  */	   0, 0,
	/* '^'  */	   0, 0,
	/* '_'  */	   0, 0,
	/* '`'  */	   0, 0,
	/* 'a'  */	   0, 0,
	/* 'b'  */	   0, 0,
	/* 'c'  */	   0, 0,
	/* 'd'  */	   0, 0,
	/* 'e'  */	   0, 0,
	/* 'f'  */	   0, 0,
	/* 'g'  */	   0, 0,
	/* 'h'  */	   0, 0,
	/* 'i'  */	   0, 0,
	/* 'j'  */	   0, 0,
	/* 'k'  */	   0, 0,
	/* 'l'  */	   0, 0,
	/* 'm'  */	   0, 0,
	/* 'n'  */	   0, 0,
	/* 'o'  */	   0, 0,
	/* 'p'  */	   0, 0,
	/* 'q'  */	   0, 0,
	/* 'r'  */	   0, 0,
	/* 's'  */	   0, 0,
	/* 't'  */	   0, 0,
	/* 'u'  */	   0, 0,
	/* 'v'  */	   0, 0,
	/* 'w'  */	   0, 0,
	/* 'x'  */	   0, 0,
	/* 'y'  */	   0, 0,
	/* 'z'  */	   0, 0,
	/* '{'  */	   0, 0,
	/* '|'  */	   0, 0,
	/* '}'  */	   0, 0,
	/* '~'  */	   0, 0,
	/* 0x7f */	   0, 0
};

static const CChar2Key CTOSUNISYS_K5_Layout =
{
	/* NumPad   */	AT_NumPad,
	/* Max ANSI */	127,

	/*       	  KN, MODIFIER */
	/* 0x00 */	   0, 0,
	/* 0x01 */	   0, 0,
	/* 0x02 */	   0, 0,
	/* 0x03 */	   0, 0,
	/* 0x04 */	   0, 0,
	/* 0x05 */	   0, 0,
	/* 0x06 */	   0, 0,
	/* 0x07 */	   0, 0,
	/* 0x08 */	   0, 0,
	/* 0x09 */	   0, 0,
	/* 0x0a */	   0, 0,
	/* 0x0b */	   0, 0,
	/* 0x0c */	   0, 0,
	/* 0x0d */	  43, 0,
	/* 0x0e */	  75, 0,
	/* 0x0f */	  79, 0,
	/* 0x10 */	  89, 0,
	/* 0x11 */	  84, 0,
	/* 0x12 */	  83, 0,
	/* 0x13 */	   0, 0,
	/* 0x14 */	   0, 0,
	/* 0x15 */	   0, 0,
	/* 0x16 */	 112, 0,
	/* 0x17 */	 113, 0,
	/* 0x18 */	 114, 0,
	/* 0x19 */	 115, 0,
	/* 0x1a */	 116, 0,
	/* 0x1b */	 117, 0,
	/* 0x1c */	 118, 0,
	/* 0x1d */	 119, 0,
	/* 0x1e */	 120, 0,
	/* 0x1f */	 121, 0,
	/* ' '  */	   0, 0,
	/* '!'  */	   0, 0,
	/* '"'  */	   0, 0,
	/* '#'  */	   0, 0,
	/* '$'  */	   0, 0,
	/* '%'  */	   0, 0,
	/* '&'  */	   0, 0,
	/* '''  */	   0, 0,
	/* '('  */	   0, 0,
	/* ')'  */	   0, 0,
	/* '*'  */	   0, 0,
	/* '+'  */	   0, 0,
	/* ','  */	   0, 0,
	/* '-'  */	   0, 0,
	/* '.'  */	   0, 0,
	/* '/'  */	   0, 0,
	/* '0'  */	  11, 0,
	/* '1'  */	   2, 0,
	/* '2'  */	   3, 0,
	/* '3'  */	   4, 0,
	/* '4'  */	   5, 0,
	/* '5'  */	   6, 0,
	/* '6'  */	   7, 0,
	/* '7'  */	   8, 0,
	/* '8'  */	   9, 0,
	/* '9'  */	  10, 0,
	/* ':'  */	   0, 0,
	/* ';'  */	   0, 0,
	/* '<'  */	   0, 0,
	/* '='  */	   0, 0,
	/* '>'  */	   0, 0,
	/* '?'  */	   0, 0,
	/* '@'  */	   0, 0,
	/* 'A'  */	   0, 0,
	/* 'B'  */	   0, 0,
	/* 'C'  */	   0, 0,
	/* 'D'  */	   0, 0,
	/* 'E'  */	   0, 0,
	/* 'F'  */	   0, 0,
	/* 'G'  */	   0, 0,
	/* 'H'  */	   0, 0,
	/* 'I'  */	   0, 0,
	/* 'J'  */	   0, 0,
	/* 'K'  */	   0, 0,
	/* 'L'  */	   0, 0,
	/* 'M'  */	   0, 0,
	/* 'N'  */	   0, 0,
	/* 'O'  */	   0, 0,
	/* 'P'  */	   0, 0,
	/* 'Q'  */	   0, 0,
	/* 'R'  */	   0, 0,
	/* 'S'  */	   0, 0,
	/* 'T'  */	   0, 0,
	/* 'U'  */	   0, 0,
	/* 'V'  */	   0, 0,
	/* 'W'  */	   0, 0,
	/* 'X'  */	   0, 0,
	/* 'Y'  */	   0, 0,
	/* 'Z'  */	   0, 0,
	/* '['  */	   0, 0,
	/* '\'  */	   0, 0,
	/* ']'  */	   0, 0,
	/* '^'  */	   0, 0,
	/* '_'  */	   0, 0,
	/* '`'  */	   0, 0,
	/* 'a'  */	   0, 0,
	/* 'b'  */	   0, 0,
	/* 'c'  */	   0, 0,
	/* 'd'  */	   0, 0,
	/* 'e'  */	   0, 0,
	/* 'f'  */	   0, 0,
	/* 'g'  */	   0, 0,
	/* 'h'  */	   0, 0,
	/* 'i'  */	   0, 0,
	/* 'j'  */	   0, 0,
	/* 'k'  */	   0, 0,
	/* 'l'  */	   0, 0,
	/* 'm'  */	   0, 0,
	/* 'n'  */	   0, 0,
	/* 'o'  */	   0, 0,
	/* 'p'  */	   0, 0,
	/* 'q'  */	   0, 0,
	/* 'r'  */	   0, 0,
	/* 's'  */	   0, 0,
	/* 't'  */	   0, 0,
	/* 'u'  */	   0, 0,
	/* 'v'  */	   0, 0,
	/* 'w'  */	   0, 0,
	/* 'x'  */	   0, 0,
	/* 'y'  */	   0, 0,
	/* 'z'  */	   0, 0,
	/* '{'  */	   0, 0,
	/* '|'  */	   0, 0,
	/* '}'  */	   0, 0,
	/* '~'  */	   0, 0,
	/* 0x7f */	   0, 0
};

static const CChar2Key JAP_106_COLL_Layout =
{
	/* NumPad   */	AT_NumPad,
	/* Max ANSI */	127,

	/*       	  KN, MODIFIER */
	/* 0x00 */	   0, 0,
	/* 0x01 */	 108, 0,
	/* 0x02 */	  30, 0,
	/* 0x03 */	 140, 0,
	/* 0x04 */	 141, 0,
	/* 0x05 */	 142, 0,
	/* 0x06 */	 143, 0,
	/* 0x07 */	  43, 0,
	/* 0x08 */	   0, 0,
	/* 0x09 */	  16, 0,
	/* 0x0a */	   0, 0,
	/* 0x0b */	  16, 0,
	/* 0x0c */	  76, 0,
	/* 0x0d */	  43, 0,
	/* 0x0e */	  75, 0,
	/* 0x0f */	 110, 0,
	/* 0x10 */	 122, 0,
	/* 0x11 */	  80, 0,
	/* 0x12 */	 124, 0,
	/* 0x13 */	  15, 0,
	/* 0x14 */	  16, MO_ShiftLeft,
	/* 0x15 */	 123, 0,
	/* 0x16 */	 112, 0,
	/* 0x17 */	 113, 0,
	/* 0x18 */	 114, 0,
	/* 0x19 */	 115, 0,
	/* 0x1a */	 116, 0,
	/* 0x1b */	 117, 0,
	/* 0x1c */	 118, 0,
	/* 0x1d */	 119, 0,
	/* 0x1e */	 120, 0,
	/* 0x1f */	 121, 0,
	/* ' '  */	  61, 0,
	/* '!'  */	   2, MO_ShiftLeft,
	/* '"'  */	   3, MO_ShiftLeft,
	/* '#'  */	   4, MO_ShiftLeft,
	/* '$'  */	   5, MO_ShiftLeft,
	/* '%'  */	   6, MO_ShiftLeft,
	/* '&'  */	   7, MO_ShiftLeft,
	/* '''  */	   8, MO_ShiftLeft,
	/* '('  */	   9, MO_ShiftLeft,
	/* ')'  */	  10, MO_ShiftLeft,
	/* '*'  */	  41, MO_ShiftLeft,
	/* '+'  */	  40, MO_ShiftLeft,
	/* ','  */	  53, 0,
	/* '-'  */	  12, 0,
	/* '.'  */	  54, 0,
	/* '/'  */	  55, 0,
	/* '0'  */	  11, 0,
	/* '1'  */	   2, 0,
	/* '2'  */	   3, 0,
	/* '3'  */	   4, 0,
	/* '4'  */	   5, 0,
	/* '5'  */	   6, 0,
	/* '6'  */	   7, 0,
	/* '7'  */	   8, 0,
	/* '8'  */	   9, 0,
	/* '9'  */	  10, 0,
	/* ':'  */	  41, 0,
	/* ';'  */	  40, 0,
	/* '<'  */	  53, MO_ShiftLeft,
	/* '='  */	  12, MO_ShiftLeft,
	/* '>'  */	  54, MO_ShiftLeft,
	/* '?'  */	  55, MO_ShiftLeft,
	/* '@'  */	  27, 0,
	/* 'A'  */	  31, MO_ShiftLeft,
	/* 'B'  */	  50, MO_ShiftLeft,
	/* 'C'  */	  48, MO_ShiftLeft,
	/* 'D'  */	  33, MO_ShiftLeft,
	/* 'E'  */	  19, MO_ShiftLeft,
	/* 'F'  */	  34, MO_ShiftLeft,
	/* 'G'  */	  35, MO_ShiftLeft,
	/* 'H'  */	  36, MO_ShiftLeft,
	/* 'I'  */	  24, MO_ShiftLeft,
	/* 'J'  */	  37, MO_ShiftLeft,
	/* 'K'  */	  38, MO_ShiftLeft,
	/* 'L'  */	  39, MO_ShiftLeft,
	/* 'M'  */	  52, MO_ShiftLeft,
	/* 'N'  */	  51, MO_ShiftLeft,
	/* 'O'  */	  25, MO_ShiftLeft,
	/* 'P'  */	  26, MO_ShiftLeft,
	/* 'Q'  */	  17, MO_ShiftLeft,
	/* 'R'  */	  20, MO_ShiftLeft,
	/* 'S'  */	  32, MO_ShiftLeft,
	/* 'T'  */	  21, MO_ShiftLeft,
	/* 'U'  */	  23, MO_ShiftLeft,
	/* 'V'  */	  49, MO_ShiftLeft,
	/* 'W'  */	  18, MO_ShiftLeft,
	/* 'X'  */	  47, MO_ShiftLeft,
	/* 'Y'  */	  22, MO_ShiftLeft,
	/* 'Z'  */	  46, MO_ShiftLeft,
	/* '['  */	  28, 0,
	/* '\'  */	  77, 0,
	/* ']'  */	  42, 0,
	/* '^'  */	  13, 0,
	/* '_'  */	  77, MO_ShiftLeft,
	/* '`'  */	  27, MO_ShiftLeft,
	/* 'a'  */	  31, 0,
	/* 'b'  */	  50, 0,
	/* 'c'  */	  48, 0,
	/* 'd'  */	  33, 0,
	/* 'e'  */	  19, 0,
	/* 'f'  */	  34, 0,
	/* 'g'  */	  35, 0,
	/* 'h'  */	  36, 0,
	/* 'i'  */	  24, 0,
	/* 'j'  */	  37, 0,
	/* 'k'  */	  38, 0,
	/* 'l'  */	  39, 0,
	/* 'm'  */	  52, 0,
	/* 'n'  */	  51, 0,
	/* 'o'  */	  25, 0,
	/* 'p'  */	  26, 0,
	/* 'q'  */	  17, 0,
	/* 'r'  */	  20, 0,
	/* 's'  */	  32, 0,
	/* 't'  */	  21, 0,
	/* 'u'  */	  23, 0,
	/* 'v'  */	  49, 0,
	/* 'w'  */	  18, 0,
	/* 'x'  */	  47, 0,
	/* 'y'  */	  22, 0,
	/* 'z'  */	  46, 0,
	/* '{'  */	  28, MO_ShiftLeft,
	/* '|'  */	  14, MO_ShiftLeft,
	/* '}'  */	  42, MO_ShiftLeft,
	/* '~'  */	  13, MO_ShiftLeft,
	/* 0x7f */	 104, 0
};

static const CChar2Key JAP_106_USB_Layout =
{
	/* NumPad   */	AT_NumPad,
	/* Max ANSI */	127,

	/*       	  KN, MODIFIER */
	/* 0x00 */	   0, 0           ,	/*                 */
	/* 0x01 */	 108, 0           ,	/* Enter           */
   /* 0x02 */    30, 0           ,  /* CAPS            */
	/* 0x03 */	 140, 0           ,	/* V_ALTmake       */
	/* 0x04 */	 141, 0           ,	/* V_ALTbreak      */
	/* 0x05 */	 142, 0           ,	/* V_CNTRLmake     */
	/* 0x06 */	 143, 0           ,	/* V_CNTRLbreak    */
	/* 0x07 */	  43, 0           ,	/* RETURN          */
	/* 0x08 */	   0, 0           ,	/* LEFT            */
	/* 0x09 */	  16, 0           ,	/* TAB             */
	/* 0x0a */	   0, 0           ,	/* DOWN            */
	/* 0x0b */	  16, 0           ,	/* TAB             */
	/* 0x0c */	  76, 0           ,	/* Del             */
	/* 0x0d */	  43, 0           ,	/* RETURN          */
	/* 0x0e */	  75, 0           ,	/* Ins             */
	/* 0x0f */	 110, 0           ,	/* ESC             */
	/* 0x10 */	 122, 0           ,	/* F11             */
	/* 0x11 */	  80, 0           ,	/* Home            */
	/* 0x12 */	 124, 0           ,	/* Prnt Scrn       */
	/* 0x13 */	  15, 0           ,	/* BACKSPACE       */
	/* 0x14 */	  16, MO_ShiftLeft,	/* TAB             */
	/* 0x15 */	 123, 0           ,	/* F12             */
	/* 0x16 */	 112, 0           ,	/* F1              */
	/* 0x17 */	 113, 0           ,	/* F2              */
	/* 0x18 */	 114, 0           ,	/* F3              */
	/* 0x19 */	 115, 0           ,	/* F4              */
	/* 0x1a */	 116, 0           ,	/* F5              */
	/* 0x1b */	 117, 0           ,	/* F6              */
	/* 0x1c */	 118, 0           ,	/* F7              */
	/* 0x1d */	 119, 0           ,	/* F8              */
	/* 0x1e */	 120, 0           ,	/* F9              */
	/* 0x1f */	 121, 0           ,	/* F10             */
	/* ' '  */	  61, 0,
	/* '!'  */	   2, MO_ShiftLeft,
	/* '"'  */	   3, MO_ShiftLeft,
	/* '#'  */	   4, MO_ShiftLeft,
	/* '$'  */	   5, MO_ShiftLeft,
	/* '%'  */	   6, MO_ShiftLeft,
	/* '&'  */	   7, MO_ShiftLeft,
	/* '''  */	   8, MO_ShiftLeft,
	/* '('  */	   9, MO_ShiftLeft,
	/* ')'  */	  10, MO_ShiftLeft,
	/* '*'  */	  41, MO_ShiftLeft,
	/* '+'  */	  40, MO_ShiftLeft,
	/* ','  */	  53, 0,
	/* '-'  */	  12, 0,
	/* '.'  */	  54, 0,
	/* '/'  */	  55, 0,
	/* '0'  */	  11, 0,
	/* '1'  */	   2, 0,
	/* '2'  */	   3, 0,
	/* '3'  */	   4, 0,
	/* '4'  */	   5, 0,
	/* '5'  */	   6, 0,
	/* '6'  */	   7, 0,
	/* '7'  */	   8, 0,
	/* '8'  */	   9, 0,
	/* '9'  */	  10, 0,
	/* ':'  */	  41, 0,
	/* ';'  */	  40, 0,
	/* '<'  */	  53, MO_ShiftLeft,
	/* '='  */	  12, MO_ShiftLeft,
	/* '>'  */	  54, MO_ShiftLeft,
	/* '?'  */	  55, MO_ShiftLeft,
	/* '@'  */	  27, 0,
	/* 'A'  */	  31, MO_ShiftLeft,
	/* 'B'  */	  50, MO_ShiftLeft,
	/* 'C'  */	  48, MO_ShiftLeft,
	/* 'D'  */	  33, MO_ShiftLeft,
	/* 'E'  */	  19, MO_ShiftLeft,
	/* 'F'  */	  34, MO_ShiftLeft,
	/* 'G'  */	  35, MO_ShiftLeft,
	/* 'H'  */	  36, MO_ShiftLeft,
	/* 'I'  */	  24, MO_ShiftLeft,
	/* 'J'  */	  37, MO_ShiftLeft,
	/* 'K'  */	  38, MO_ShiftLeft,
	/* 'L'  */	  39, MO_ShiftLeft,
	/* 'M'  */	  52, MO_ShiftLeft,
	/* 'N'  */	  51, MO_ShiftLeft,
	/* 'O'  */	  25, MO_ShiftLeft,
	/* 'P'  */	  26, MO_ShiftLeft,
	/* 'Q'  */	  17, MO_ShiftLeft,
	/* 'R'  */	  20, MO_ShiftLeft,
	/* 'S'  */	  32, MO_ShiftLeft,
	/* 'T'  */	  21, MO_ShiftLeft,
	/* 'U'  */	  23, MO_ShiftLeft,
	/* 'V'  */	  49, MO_ShiftLeft,
	/* 'W'  */	  18, MO_ShiftLeft,
	/* 'X'  */	  47, MO_ShiftLeft,
	/* 'Y'  */	  22, MO_ShiftLeft,
	/* 'Z'  */	  46, MO_ShiftLeft,
	/* '['  */	  28, 0,
	/* '\'  */	  56, 0,
	/* ']'  */	  42, 0,
	/* '^'  */	  13, 0,
	/* '_'  */	  56, MO_ShiftLeft,
	/* '`'  */	  27, MO_ShiftLeft,
	/* 'a'  */	  31, 0,
	/* 'b'  */	  50, 0,
	/* 'c'  */	  48, 0,
	/* 'd'  */	  33, 0,
	/* 'e'  */	  19, 0,
	/* 'f'  */	  34, 0,
	/* 'g'  */	  35, 0,
	/* 'h'  */	  36, 0,
	/* 'i'  */	  24, 0,
	/* 'j'  */	  37, 0,
	/* 'k'  */	  38, 0,
	/* 'l'  */	  39, 0,
	/* 'm'  */	  52, 0,
	/* 'n'  */	  51, 0,
	/* 'o'  */	  25, 0,
	/* 'p'  */	  26, 0,
	/* 'q'  */	  17, 0,
	/* 'r'  */	  20, 0,
	/* 's'  */	  32, 0,
	/* 't'  */	  21, 0,
	/* 'u'  */	  23, 0,
	/* 'v'  */	  49, 0,
	/* 'w'  */	  18, 0,
	/* 'x'  */	  47, 0,
	/* 'y'  */	  22, 0,
	/* 'z'  */	  46, 0,
	/* '{'  */	  28, MO_ShiftLeft,
	/* '|'  */	  14, MO_ShiftLeft,
	/* '}'  */	  42, MO_ShiftLeft,
	/* '~'  */	  13, MO_ShiftLeft,
	/* 0x7f */	 104, 0
	};

/*	 Id: USB.KLI 1.2 1999/09/15 12:22:40 D_F Exp  	*/
extern const CScancodeSequence seq_JAP_106_USB_KEYBOARD_SCAN[] =
{
/*    30 */      6,   30, { 0xE1, 0x39, 0xF0, 0x39, 0xF0, 0xE1 },
/*   138 */		  1, 	138, { 0xE3 },
/*   139 */		  2, 	139, { 0xF0, 0xE3 },
/*   140 */		  1, 	140, { 0xE2 },
/*   141 */		  2, 	141, { 0xF0, 0xE2 },
/*   142 */		  1, 	142, { 0xE0 },
/*   143 */		  2, 	143, { 0xF0, 0xE0 },
	0
};

/*	 Id: USB.KLI 1.2 1999/09/15 12:22:40 D_F Exp  	*/
extern const CScancodes JAP_106_USB_KEYBOARD_SCAN =
{
	/* sequence info		*/	seq_JAP_106_USB_KEYBOARD_SCAN,
	/* max used key		*/	149,
	/* extension			*/	0x00,
	/* illegal scancode	*/	0x00, 0,
	/*   1 					*/	0x35, 0,
	/*   2 					*/	0x1E, 0,
	/*   3 					*/	0x1F, 0,
	/*   4 					*/	0x20, 0,
	/*   5 					*/	0x21, 0,
	/*   6 					*/	0x22, 0,
	/*   7 					*/	0x23, 0,
	/*   8 					*/	0x24, 0,
	/*   9 					*/	0x25, 0,
	/*  10 					*/	0x26, 0,
	/*  11 					*/	0x27, 0,
	/*  12 					*/	0x2D, 0,
	/*  13 					*/	0x2E, 0,
	/*  14 					*/	0x89, 0,
	/*  15 					*/	0x2A, 0,
	/*  16 					*/	0x2B, 0,
	/*  17 					*/	0x14, 0,
	/*  18 					*/	0x1A, 0,
	/*  19 					*/	0x08, 0,
	/*  20 					*/	0x15, 0,
	/*  21 					*/	0x17, 0,
	/*  22 					*/	0x1C, 0,
	/*  23 					*/	0x18, 0,
	/*  24 					*/	0x0C, 0,
	/*  25 					*/	0x12, 0,
	/*  26 					*/	0x13, 0,
	/*  27 					*/	0x2F, 0,
	/*  28 					*/	0x30, 0,
	/*  29 					*/	0x31, 0,
   /*  30               */ 0x39, 0|KN_COMMUNICATION|KN_SEQUENCE,
	/*  31 					*/	0x04, 0,
	/*  32 					*/	0x16, 0,
	/*  33 					*/	0x07, 0,
	/*  34 					*/	0x09, 0,
	/*  35 					*/	0x0A, 0,
	/*  36 					*/	0x0B, 0,
	/*  37 					*/	0x0D, 0,
	/*  38 					*/	0x0E, 0,
	/*  39 					*/	0x0F, 0,
	/*  40 					*/	0x33, 0,
	/*  41 					*/	0x34, 0,
	/*  42 					*/	0x32, 0,
	/*  43 					*/	0x28, 0,
	/*  44 					*/	0xE1, KN_COMMUNICATION,
	/*  45 					*/	0x64, 0,
	/*  46 					*/	0x1D, 0,
	/*  47 					*/	0x1B, 0,
	/*  48 					*/	0x06, 0,
	/*  49 					*/	0x19, 0,
	/*  50 					*/	0x05, 0,
	/*  51 					*/	0x11, 0,
	/*  52 					*/	0x10, 0,
	/*  53 					*/	0x36, 0,
	/*  54 					*/	0x37, 0,
	/*  55 					*/	0x38, 0,
	/*  56 					*/	0x87, 0,
	/*  57 					*/	0xE5, KN_COMMUNICATION,
	/*  58 					*/	0xE0, KN_COMMUNICATION,
	/*  59 					*/	0xE3, 0,
	/*  60 					*/	0xE2, 0,
	/*  61 					*/	0x2C, 0,
	/*  62 					*/	0xE6, 0,
	/*  63 					*/	0xE7, 0,
	/*  64 					*/	0xE4, KN_COMMUNICATION,
	/*  65 Empty			*/	0x00, 0,
	/*  66 Empty			*/	0x00, 0,
	/*  67 Empty			*/	0x00, 0,
	/*  68 Empty			*/	0x00, 0,
	/*  69 Empty			*/	0x00, 0,
	/*  70 Empty			*/	0x00, 0,
	/*  71 Empty			*/	0x00, 0,
	/*  72 Empty			*/	0x00, 0,
	/*  73 Empty			*/	0x00, 0,
	/*  74 Empty			*/	0x00, 0,
	/*  75 					*/	0x49, 0,
	/*  76 					*/	0x4C, 0,
	/*  77 Empty			*/	0x00, 0,
	/*  78 Empty			*/	0x00, 0,
	/*  79 					*/	0x50, 0,
	/*  80 					*/	0x4A, 0,
	/*  81 					*/	0x4D, 0,
	/*  82 Empty			*/	0x00, 0,
	/*  83 					*/	0x52, 0,
	/*  84 					*/	0x51, 0,
	/*  85 					*/	0x4B, 0,
	/*  86 					*/	0x4E, 0,
	/*  87 Empty			*/	0x00, 0,
	/*  88 Empty			*/	0x00, 0,
	/*  89 					*/	0x4F, 0,
	/*  90 					*/	0x53, KN_COMMUNICATION,
	/*  91 					*/	0x5F, 0,
	/*  92 					*/	0x5C, 0,
	/*  93 					*/	0x59, 0,
	/*  94 Empty			*/	0x00, 0,
	/*  95 					*/	0x54, 0,
	/*  96 					*/	0x60, 0,
	/*  97 					*/	0x5D, 0,
	/*  98 					*/	0x5A, 0,
	/*  99 					*/	0x62, 0,
	/* 100 					*/	0x55, 0,
	/* 101 					*/	0x61, 0,
	/* 102 					*/	0x5E, 0,
	/* 103 					*/	0x5B, 0,
	/* 104 					*/	0x63, 0,
	/* 105 					*/	0x56, 0,
	/* 106 					*/	0x57, 0,
	/* 107 					*/	0x85, 0,
	/* 108 					*/	0x58, 0,
	/* 109 					*/	0x67, 0,
	/* 110 					*/	0x29, 0,
	/* 111 Empty			*/	0x00, 0,
	/* 112 					*/	0x3A, 0,
	/* 113 					*/	0x3B, 0,
	/* 114 					*/	0x3C, 0,
	/* 115 					*/	0x3D, 0,
	/* 116 					*/	0x3E, 0,
	/* 117 					*/	0x3F, 0,
	/* 118 					*/	0x40, 0,
	/* 119 					*/	0x41, 0,
	/* 120 					*/	0x42, 0,
	/* 121 					*/	0x43, 0,
	/* 122 					*/	0x44, 0,
	/* 123 					*/	0x45, 0,
	/* 124 					*/	0x46, 0,
	/* 125 					*/	0x47, 0,
	/* 126 					*/	0x48, 0,
	/* 127 Empty			*/	0x00, 0,
	/* 128 Empty			*/	0x00, 0,
	/* 129 					*/	0x65, 0,
	/* 130 					*/	0x68, 0,
	/* 131 					*/	0x69, 0,
	/* 132 					*/	0x6A, 0,
	/* 133 Empty			*/	0x00, 0,
	/* 134 Empty			*/	0x00, 0,
	/* 135 Empty			*/	0x00, 0,
	/* 136 Empty			*/	0x00, 0,
	/* 137 Empty			*/	0x00, 0,
	/* 138 Sequence		*/	0x00, KN_SEQUENCE,
	/* 139 Sequence		*/	0x00, KN_SEQUENCE,
	/* 140 Sequence		*/	0x00, KN_SEQUENCE,
	/* 141 Sequence		*/	0x00, KN_SEQUENCE,
	/* 142 Sequence		*/	0x00, KN_SEQUENCE,
	/* 143 Sequence		*/	0x00, KN_SEQUENCE,
	/* 144 Empty			*/	0x00, 0,
	/* 145 Empty			*/	0x00, 0,
	/* 146 					*/	0x00, 0,
	/* 147 					*/	0x00, 0,
	/* 148 					*/	0x00, 0,
	/* 149 					*/	0x00, 0,
};


static const CChar2Key NEC_98_COLL_Layout =
{
	/* NumPad   */	AT_NumPad,
	/* Max ANSI */	127,

	/*       	  KN, MODIFIER */
	/* 0x00 */	   0, 0,
	/* 0x01 */	   1, 0,
	/* 0x02 */	   2, 0,
	/* 0x03 */	   3, 0,
	/* 0x04 */	   4, 0,
	/* 0x05 */	   5, 0,
	/* 0x06 */	   6, 0,
	/* 0x07 */	   7, 0,
	/* 0x08 */	   8, 0,
	/* 0x09 */	   9, 0,
	/* 0x0a */	  10, 0,
	/* 0x0b */	  11, 0,
	/* 0x0c */	  12, 0,
	/* 0x0d */	  13, 0,
	/* 0x0e */	  14, 0,
	/* 0x0f */	  15, 0,
	/* 0x10 */	  16, 0,
	/* 0x11 */	  17, 0,
	/* 0x12 */	  18, 0,
	/* 0x13 */	  19, 0,
	/* 0x14 */	  20, 0,
	/* 0x15 */	  21, 0,
	/* 0x16 */	  22, 0,
	/* 0x17 */	  23, 0,
	/* 0x18 */	  24, 0,
	/* 0x19 */	  25, 0,
	/* 0x1a */	  26, 0,
	/* 0x1b */	  27, 0,
	/* 0x1c */	  28, 0,
	/* 0x1d */	  29, 0,
	/* 0x1e */	  30, 0,
	/* 0x1f */	  31, 0,
	/* ' '  */	  32, 0,
	/* '!'  */	  33, MO_ShiftLeft,
	/* '"'  */	  34, MO_ShiftLeft,
	/* '#'  */	  35, MO_ShiftLeft,
	/* '$'  */	  36, MO_ShiftLeft,
	/* '%'  */	  37, MO_ShiftLeft,
	/* '&'  */	  38, MO_ShiftLeft,
	/* '''  */	  39, MO_ShiftLeft,
	/* '('  */	  40, MO_ShiftLeft,
	/* ')'  */	  41, MO_ShiftLeft,
	/* '*'  */	  42, MO_ShiftLeft,
	/* '+'  */	  43, MO_ShiftLeft,
	/* ','  */	  44, 0,
	/* '-'  */	  45, 0,
	/* '.'  */	  46, 0,
	/* '/'  */	  47, 0,
	/* '0'  */	  48, 0,
	/* '1'  */	  49, 0,
	/* '2'  */	  50, 0,
	/* '3'  */	  51, 0,
	/* '4'  */	  52, 0,
	/* '5'  */	  53, 0,
	/* '6'  */	  54, 0,
	/* '7'  */	  55, 0,
	/* '8'  */	  56, 0,
	/* '9'  */	  57, 0,
	/* ':'  */	  58, 0,
	/* ';'  */	  59, 0,
	/* '<'  */	  60, MO_ShiftLeft,
	/* '='  */	  61, MO_ShiftLeft,
	/* '>'  */	  62, MO_ShiftLeft,
	/* '?'  */	  63, MO_ShiftLeft,
	/* '@'  */	  64, 0,
	/* 'A'  */	  65, MO_ShiftLeft,
	/* 'B'  */	  66, MO_ShiftLeft,
	/* 'C'  */	  67, MO_ShiftLeft,
	/* 'D'  */	  68, MO_ShiftLeft,
	/* 'E'  */	  69, MO_ShiftLeft,
	/* 'F'  */	  70, MO_ShiftLeft,
	/* 'G'  */	  71, MO_ShiftLeft,
	/* 'H'  */	  72, MO_ShiftLeft,
	/* 'I'  */	  73, MO_ShiftLeft,
	/* 'J'  */	  74, MO_ShiftLeft,
	/* 'K'  */	  75, MO_ShiftLeft,
	/* 'L'  */	  76, MO_ShiftLeft,
	/* 'M'  */	  77, MO_ShiftLeft,
	/* 'N'  */	  78, MO_ShiftLeft,
	/* 'O'  */	  79, MO_ShiftLeft,
	/* 'P'  */	  80, MO_ShiftLeft,
	/* 'Q'  */	  81, MO_ShiftLeft,
	/* 'R'  */	  82, MO_ShiftLeft,
	/* 'S'  */	  83, MO_ShiftLeft,
	/* 'T'  */	  84, MO_ShiftLeft,
	/* 'U'  */	  85, MO_ShiftLeft,
	/* 'V'  */	  86, MO_ShiftLeft,
	/* 'W'  */	  87, MO_ShiftLeft,
	/* 'X'  */	  88, MO_ShiftLeft,
	/* 'Y'  */	  89, MO_ShiftLeft,
	/* 'Z'  */	  90, MO_ShiftLeft,
	/* '['  */	  91, 0,
	/* '\'  */	  92, 0,
	/* ']'  */	  93, 0,
	/* '^'  */	  94, 0,
	/* '_'  */	  95, MO_ShiftLeft,
	/* '`'  */	  96, MO_ShiftLeft,
	/* 'a'  */	  97, 0,
	/* 'b'  */	  98, 0,
	/* 'c'  */	  99, 0,
	/* 'd'  */	 100, 0,
	/* 'e'  */	 101, 0,
	/* 'f'  */	 102, 0,
	/* 'g'  */	 103, 0,
	/* 'h'  */	 104, 0,
	/* 'i'  */	 105, 0,
	/* 'j'  */	 106, 0,
	/* 'k'  */	 107, 0,
	/* 'l'  */	 108, 0,
	/* 'm'  */	 109, 0,
	/* 'n'  */	 110, 0,
	/* 'o'  */	 111, 0,
	/* 'p'  */	 112, 0,
	/* 'q'  */	 113, 0,
	/* 'r'  */	 114, 0,
	/* 's'  */	 115, 0,
	/* 't'  */	 116, 0,
	/* 'u'  */	 117, 0,
	/* 'v'  */	 118, 0,
	/* 'w'  */	 119, 0,
	/* 'x'  */	 120, 0,
	/* 'y'  */	 121, 0,
	/* 'z'  */	 122, 0,
	/* '{'  */	 123, MO_ShiftLeft,
	/* '|'  */	 124, MO_ShiftLeft,
	/* '}'  */	 125, MO_ShiftLeft,
	/* '~'  */	 126, MO_ShiftLeft,
	/* 0x7f */	 127, 0
};

// -----------------------------------------------------------------------
//this is mapped "straight through"  the ansi number and the keynumber are the same.
// The implementaiton in the 3400 this is taken from does not
// allow any lower case letters to be transmitted
// this is why the lower case letters are all shifted.
// 4/9/99
// have found that the mapping from 3400 was not fully tested; only numbers were verified
// I think the shift is wrong, and some other keys are still wrong.
// The are no lower case keys available on the keyboard as far as I can tell
// Will need to do furhter tests or get scancodes somehow to wrap this up
// -----------------------------------------------------------------------
static const CChar2Key BURROUGHS_K4_Layout =
{
	/* NumPad   */	AT_NumPad,
	/* Max ANSI */	127,

	/*       	  KN, MODIFIER */			//scancode
	/* 0x00 */    0, 0,		            //0x00
	/* 0x01 */	  0, 0,		            //0x00
	/* 0x02 */	  0, 0,		            //0x00
	/* 0x03 */	  0, 0,		            //0x00
	/* 0x04 */	  0, 0,		            //0x00
	/* 0x05 */	  0, 0,		            //0x00
	/* 0x06 */	  0, 0,		            //0x00
	/* 0x07 */	  0, 0,		            //0x00
	/* 0x08 */	  0, 0,		            //0x00
	/* 0x09 */	  0, 0,		            //0x00
	/* 0x0a */	 10, 0,		            //0x27
	/* 0x0b */	  0, 0,		            //0x00
	/* 0x0c */	  0, 0,		            //0x00
	/* 0x0d */	 13, 0,		            //0x27
	/* 0x0e */	 14, 0,		            //0x00
	/* 0x0f */	  0, 0,		            //0x00
	/* 0x10 */	  0, 0,		            //0x00
	/* 0x11 */	  0, 0,		            //0x00
	/* 0x12 */	  0, 0,		            //0x00
	/* 0x13 */	  0, 0,		            //0x00
  	/* 0x14 */	  0, 0,		            //0x00
  	/* 0x15 */	  0, 0,		            //0x00
  	/* 0x16 */	  0, 0,		            //0x00
	/* 0x17 */	  0, 0,		            //0x00
	/* 0x18 */	  0, 0,		            //0x00
	/* 0x19 */	  0, 0,		            //0x00
	/* 0x1a */	  0, 0,		            //0x00
	/* 0x1b */	 27, 0,		            //0x77
	/* 0x1c */	  0, 0,		            //0x00
	/* 0x1d */	  0, 0,		            //0x00
	/* 0x1e */	  0, 0,		            //0x00
	/* 0x1f */	  0, 0,		            //0x00
	/* ' '  */	 32, 0,		            //0x47
	/* '!'  */	  0, 0,
	/* '"'  */	  0, 0,
	/* '#'  */	  0, 0,
	/* '$'  */	  0, 0,
	/* '%'  */	  0, 0,
	/* '&'  */	  0, 0,
 	/* '''  */	  0, 0,
	/* '('  */	  0, 0,
	/* ')'  */	  0, 0,
	/* '*'  */	 83, MO_ShiftLeft,
	/* '+'  */	 84, MO_ShiftLeft,
	/* ','  */    0, 0,
	/* '-'  */	 85, MO_ShiftLeft,
	/* '.'  */	 46, 0,		            //0x45
	/* '/'  */	 86, MO_ShiftLeft,
	/* '0'  */	 48, 0,		            //0x56
	/* '1'  */	 49, 0,		            //0x7C
	/* '2'  */	 50, 0,		            //0x4E
	/* '3'  */	 51, 0,		            //0x0E
	/* '4'  */	 52, 0,		            //0x10
	/* '5'  */	 53, 0,		            //0x20
	/* '6'  */	 54, 0,		            //0x3E
	/* '7'  */	 55, 0,		            //0x62
 	/* '8'  */	 56, 0,		            //0x60
	/* '9'  */	 57, 0,		            //0x3D
	/* ':'  */	 58, 0,		            //0xD1
	/* ';'  */	  0, 0,
	/* '<'  */	  0, 0,
	/* '='  */	  0, 0,
	/* '>'  */	  0, 0,
	/* '?'  */	  0, 0,
	/* '@'  */	  0, 0,
	/* 'A'  */	 65, 0,					//0x2B        	A
	/* 'B'  */	 66, 0,					//0x4B         B
	/* 'C'  */	 67, 0,					//0x0B        	C
	/* 'D'  */   68, 0,					//0x73        	D
	/* 'E'  */	 69, 0,					//0x33        	E
	/* 'F'  */	 70, 0,					//0x53        	F
	/* 'G'  */	 71, 0,					//0x63        	G
	/* 'H'  */	 72, 0,					//0x23        	H
 	/* 'I'  */	 73, 0,					//0x43        	I
	/* 'J'  */	 74, 0,					//0x03        	J
	/* 'K'  */	 75, 0,					//0x2F        	K
	/* 'L'  */	 76, 0,					//0x3B        	L
	/* 'M'  */	 77, 0,					//0x42        	M
	/* 'N'  */	 78, 0,					//0x39        	N
	/* 'O'  */	 79, 0,					//0x59        	O
	/* 'P'  */	 80, 0,					//0x19        	P
	/* 'Q'  */	 81, 0,					//0x69        	Q
	/* 'R'  */	 82, 0,					//0x29        	R
	/* 'S'  */	 83, 0,					//0x49        	S
	/* 'T'  */	 84, 0,					//0x09			T
	/* 'U'  */	 85, 0,					//0x1B        	U
	/* 'V'  */	 86, 0,					//0x71        	V
	/* 'W'  */	 87, 0,					//0x31      	W
	/* 'X'  */	 88, 0,					//0x79   	   X
	/* 'Y'  */	 89, 0,					//0x25 	      Y
 	/* 'Z'  */	 85, MO_ShiftLeft,	//0x1B      	Z (shifted U)
	/* '['  */	  0, 0,
	/* '\'  */	  0, 0,
	/* ']'  */	  0, 0,
	/* '^'  */	  0, 0,
	/* '_'  */	  0, 0,
	/* '`'  */	  0, 0,
	/* 'a'  */	 65, 0,					//0x2B        	A
	/* 'b'  */	 66, 0,					//0x4B         B
	/* 'c'  */	 67, 0,					//0x0B        	C
	/* 'd'  */	 68, 0,					//0x73        	D
	/* 'e'  */	 69, 0,					//0x33        	E
	/* 'f'  */	 70, 0,					//0x53        	F
	/* 'g'  */	 71, 0,					//0x63        	G
	/* 'h'  */	 72, 0,					//0x23        	H
	/* 'i'  */	 73, 0,					//0x43        	I
	/* 'j'  */	 74, 0,					//0x03        	J
 	/* 'k'  */	 75, 0,					//0x2F        	K
	/* 'l'  */	 76, 0,					//0x3B        	L
	/* 'm'  */	 77, 0,					//0x42        	M
	/* 'n'  */	 78, 0,					//0x39        	N
	/* 'o'  */	 79, 0,					//0x59        	O
	/* 'p'  */	 80, 0,					//0x19        	P
	/* 'q'  */	 81, 0,					//0x69        	Q
	/* 'r'  */	 82, 0,					//0x29        	R
	/* 's'  */	 83, 0,					//0x49        	S
	/* 't'  */	 84, 0,					//0x33        	?
	/* 'u'  */	 85, 0,					//0x1B        	U
	/* 'v'  */	 86, 0,					//0x71        	V
	/* 'w'  */	 87, 0,					//0x31      	W
	/* 'x'  */	 88, 0,					//0x79   	   X
	/* 'y'  */	 89, 0,					//0x25 	      Y
	/* 'z'  */	 85, MO_ShiftLeft,	//0x1B     		Z shifted U
	/* '{'  */	  0, 0,
	/* '|'  */	  0, 0,
	/* '}'  */	  0, 0,
	/* '~'  */	126, MO_ShiftRight,			//0xE0
	/* 0x7f */	127, 0,						//0x00
};


//this is mapped "straight through", i.e. the ansi number
// and the keynumber are the same.
// The implementaiton in the 3400 this is taken from does not
// allow any lower case letters to be transmitted
// this is why the lower case letters are all shifted
static const CChar2Key BURROUGHS_K5_Layout =
{
	/* NumPad   */	AT_NumPad,
	/* Max ANSI */	127,

	/*       	  KN, MODIFIER */       //scancode			with MSB stripeed
	/* 0x00 */    0, 0,						//0x00            00
	/* 0x01 */	  1, 0,						//0x57            57
	/* 0x02 */	  2, MO_ShiftLeft,		//0x91            11
	/* 0x03 */	  3, MO_ShiftLeft,		//0x8D            0D
	/* 0x04 */	  4, 0,						//0x22            22
	/* 0x05 */	  5, MO_ShiftLeft,		//0x92            12
	/* 0x06 */	  6, 0,						//0x12            12
	/* 0x07 */	  7, 0,						//0x0D            0D
	/* 0x08 */	  8, 0,						//0x77            77
	/* 0x09 */	  9, 0,						//0x37            37
	/* 0x0a */	 10, 0,						//0x11            11
	/* 0x0b */	 11, 0,						//0x57            57
	/* 0x0c */	 12, 0,						//0x00            00
	/* 0x0d */	 13, 0,		  	 	  		//0x57            57
	/* 0x0e */	 14, 0,		  	 	  		//0x00            00
	/* 0x0f */	 15, 0,		  	 	  		//0x3F            3F
	/* 0x10 */	 16, 0,		  	 		  	//0x47            47
	/* 0x11 */	 17, 0,		  	 		  	//0x17            17
	/* 0x12 */	 18, 0,		  	 		  	//0x07            07
	/* 0x13 */	 19, 0,		  	 		  	//0x00            00
   /* 0x14 */	 20, 0,		  	 		  	//0x27            27
   /* 0x15 */	 21, 0,		  				//0x13            13
  	/* 0x16 */	 22, 0,		  				//0x2B            2B
	/* 0x17 */	 23, 0,		  				//0x4B            4B
	/* 0x18 */	 24, 0,		  				//0x0B            0B
	/* 0x19 */	 25, 0,		  				//0x73            73
	/* 0x1a */	 26, 0,		 			 	//0x33            33
	/* 0x1b */	 27, 0,		  				//0x53            53
	/* 0x1c */	 28, 0,		  				//0x63            63
	/* 0x1d */	 29, 0,		 				//0x23            23
	/* 0x1e */	 30, 0,		 				//0x43            43
	/* 0x1f */	 31, 0,		 				//0x03            03
	/* ' '  */	 32, 0,		 				//0x7D            7D
	/* '!'  */	 33, MO_ShiftLeft,		//0xB9            39
	/* '"'  */	 34, MO_ShiftLeft,		//0xD9            59
	/* '#'  */	 35, MO_ShiftLeft,		//0xB2            32
	/* '$'  */	 36, MO_ShiftLeft,	 	//0xE9            69
	/* '%'  */	 37, MO_ShiftLeft,		//0xA9            29
	/* '&'  */	 38, MO_ShiftLeft,		//0xC9            49
 	/* '''  */	 39, 0,						//0x42            42
	/* '('  */	 40, MO_ShiftLeft,		//0xF1            71
	/* ')'  */	 41, MO_ShiftLeft,		//0xB1            31
	/* '*'  */	 42, MO_ShiftLeft,		//0x99            19
	/* '+'  */	 43, 0,						//0x25            25
	/* ','  */	 44, 0,		 				//0x65            65
	/* '-'  */	 45, 0,		 				//0x05            05
	/* '.'  */	 46, 0,					 	//0x45            45
	/* '/'  */	 47, MO_ShiftLeft,		//0x89            09
	/* '0'  */	 48, 0,		 				//0x79            79
	/* '1'  */	 49, 0,						//0x39            39
	/* '2'  */	 50, 0,						//0x59            59
	/* '3'  */	 51, 0,						//0x19            19
	/* '4'  */	 52, 0,						//0x69            69
	/* '5'  */	 53, 0,						//0x29            29
	/* '6'  */	 54, 0,		 				//0x49            49
	/* '7'  */	 55, 0,		 				//0x09            09
 	/* '8'  */	 56, 0,					 	//0x71            71
	/* '9'  */	 57, 0,		 				//0x31            31
	/* ':'  */	 58, MO_ShiftLeft,		//0xC5            45
	/* ';'  */	 59, MO_ShiftLeft,		//0xE5            65
	/* '<'  */	 60, MO_ShiftLeft,		//0xFC            7C
	/* '='  */	 61, MO_ShiftLeft,		//0xF9            79
	/* '>'  */	 62, MO_ShiftLeft,		//0xCE            4E
	/* '?'  */	 63, MO_ShiftLeft,		//0xA5            25
	/* '@'  */	 64, 0,	   				//0x4F            4F
	/* 'A'  */	 97, MO_ShiftLeft,		//0xBC            3C
	/* 'B'  */	 98, MO_ShiftLeft,		//0xDC            5C
	/* 'C'  */	 99, MO_ShiftLeft,		//0x9C            1C
	/* 'D'  */  100, MO_ShiftLeft,		//0xEC            6C
	/* 'E'  */	101, MO_ShiftLeft,  	  	//0xAC            2C
	/* 'F'  */	102, MO_ShiftLeft,  	   //0xCC            4C
	/* 'G'  */	103, MO_ShiftLeft,  	   //0x8C            0C
	/* 'H'  */	104, MO_ShiftLeft,  	   //0xF4            74
 	/* 'I'  */	105, MO_ShiftLeft,  	   //0xB4            34
	/* 'J'  */	106, MO_ShiftLeft,  	   //0xD4            54
	/* 'K'  */	107, MO_ShiftLeft,  	  	//0x94            14
	/* 'L'  */	108, MO_ShiftLeft,  	  	//0xE4            64
	/* 'M'  */	109, MO_ShiftLeft,  	  	//0xA4            24
	/* 'N'  */	110, MO_ShiftLeft,  	  	//0xC4            44
	/* 'O'  */	111, MO_ShiftLeft,  	  	//0x84            04
	/* 'P'  */	112, MO_ShiftLeft,  	  	//0xF8            78
	/* 'Q'  */	113, MO_ShiftLeft,  	  	//0xB8            38
	/* 'R'  */	114, MO_ShiftLeft,  	  	//0xD8            58
	/* 'S'  */	115, MO_ShiftLeft,  	  	//0x98            18
	/* 'T'  */	116, MO_ShiftLeft,  	  	//0xE8            68
	/* 'U'  */	117, MO_ShiftLeft,  	  	//0xA8            28
	/* 'V'  */	118, MO_ShiftLeft,  	  	//0xC8            48
	/* 'W'  */	119, MO_ShiftLeft,  	  	//0x88            08
	/* 'X'  */	120, MO_ShiftLeft,  	  	//0xF0            70
	/* 'Y'  */	121, MO_ShiftLeft,  	  	//0xB0            30
 	/* 'Z'  */	122, MO_ShiftLeft,  	  	//0xD0            50
	/* '['  */	 91, MO_ShiftLeft, 	  	//0xE2            62
	/* '\'  */	 92, MO_ShiftLeft, 	  	//0x96            16
	/* ']'  */	 93, MO_ShiftLeft, 	  	//0xE0            60
	/* '^'  */	 94, MO_ShiftLeft, 	  	//0xBD            3D
	/* '_'  */	 95, MO_ShiftLeft, 	  	//0x85            05
	/* '`'  */	 96, MO_ShiftLeft, 	  	//0x95            15
	/* 'a'  */	 97, 0, 	  					//0x3C            3C
	/* 'b'  */	 98, 0, 	  					//0x5C            5C
	/* 'c'  */	 99, 0, 	  					//0x1C            1C
	/* 'd'  */	100, 0, 	  					//0x6C            6C
	/* 'e'  */	101, 0, 	  					//0x2C            2C
	/* 'f'  */	102, 0, 	  					//0x4C            4C
	/* 'g'  */	103, 0, 	  					//0x0C            0C
	/* 'h'  */	104, 0, 	  					//0x74            74
	/* 'i'  */	105, 0, 	  					//0x34            34
	/* 'j'  */	106, 0, 	  					//0x54            54
 	/* 'k'  */	107, 0, 	  					//0x14            14
	/* 'l'  */	108, 0, 	  					//0x64            64
	/* 'm'  */	109, 0, 	  					//0x24            24
	/* 'n'  */	110, 0, 	  					//0x44            44
	/* 'o'  */	111, 0, 	  					//0x04            04
	/* 'p'  */	112, 0, 	  					//0x78            78
	/* 'q'  */	113, 0, 	  					//0x38            38
	/* 'r'  */	114, 0, 	  					//0x58            58
  	/* 's'  */	115, 0, 	  					//0x18            18
	/* 't'  */	116, 0, 	  					//0x68            68
	/* 'u'  */	117, 0, 	   				//0x28            28
	/* 'v'  */	118, 0, 	   				//0x48            48
	/* 'w'  */	119, 0, 	   				//0x08            08
	/* 'x'  */	120, 0, 	   				//0x70            70
	/* 'y'  */	121, 0, 	   				//0x30            30
	/* 'z'  */	122, 0, 	   				//0x50            50
	/* '{'  */	123, MO_ShiftLeft, 	   //0x90            10
	/* '|'  */	124, 0,			    		//0x00            00
	/* '}'  */	125, MO_ShiftLeft, 		//0xA0            20
	/* '~'  */	126, MO_ShiftLeft, 		//0xBE            3E
	/* 0x7f */	127, 0, 						//0x80            00
};

/******************************************************/
/* Terminal properties                                */
/******************************************************/
extern const CWedgeTerminal JAP_106_TERM;
extern const CWedgeTerminal USB_JAP_106_TERM;
extern const CWedgeTerminal NEC_98_TERM;
extern const CWedgeTerminal BURROUGHS_K5_TERM;
extern const CWedgeTerminal BURROUGHS_K4_TERM;
extern const CWedgeTerminal CTOSUNISYS_K4_TERM;
extern const CWedgeTerminal CTOSUNISYS_K5_TERM;
extern const CWedgeTerminal CTOSUNISYS_KB100_TERM;


const CWedgeTerminal CTOSUNISYS_KB100_TERM =
{
	/* table format version    */		4,
	/* capabilities            */ 0,
	/* ANSI to keynumber       */	&CTOSUNISYS_KB100_Layout,
	/* international info      */	&USA_Only_FixupPtr,
	/* keynumber to scancode   */	&CTOSUNISYS_KB100_SCAN,
	/* make/break info         */	&CTOSUNISYS_KB100_MKBR,
	/* short delay      (uSec) */	  500,
	/* medium delay     (uSec) */	 5000,
	/* long delay       (uSec) */	30000,
	/* interchar delay  (uSec) */	 5000,
	/* lshift  	  (key number) */	0,
	/* rshift  	  (key number) */	0,
	/* lcntrl  	  (key number) */	0,
	/* rcntrl  	  (key number) */	0,
	/* lalt    	  (key number) */	0,
	/* ralt    	  (key number) */	0,
	/* lcompose	  (key number) */	0,
	/* rcompose	  (key number) */	0,
	/* capslock	  (key number) */	0,
	/* numlock 	  (key number) */	0
};

const CWedgeTerminal CTOSUNISYS_K4_TERM =
{
	/* table format version    */	    4,
	/* capabilities            */	0,
	/* ANSI to keynumber       */	&CTOSUNISYS_K4_Layout,
	/* international info      */	&USA_Only_FixupPtr,
	/* keynumber to scancode   */	&CTOSUNISYS_K4_SCAN,
	/* make/break info         */	&CTOSUNISYS_KB100_MKBR,
	/* short delay      (uSec) */	  500,
	/* medium delay     (uSec) */	 5000,
	/* long delay       (uSec) */	30000,
	/* interchar delay  (uSec) */	 5000,
	/* lshift  	  (key number) */	0,
	/* rshift  	  (key number) */	57,
	/* lcntrl  	  (key number) */	58,
	/* rcntrl  	  (key number) */	64,
	/* lalt    	  (key number) */	60,
	/* ralt    	  (key number) */	62,
	/* lcompose	  (key number) */	0,
	/* rcompose	  (key number) */	0,
	/* capslock	  (key number) */	0,
	/* numlock 	  (key number) */	0
};

const CWedgeTerminal CTOSUNISYS_K5_TERM =
{
	/* table format version    */	    4,
	/* capabilities            */	0,
	/* ANSI to keynumber       */	&CTOSUNISYS_K5_Layout,
	///* ANSI to keynumber       */	&CTOSUNISYS_K4_Layout,
	/* international info      */	&USA_Only_FixupPtr,
	/* keynumber to scancode   */	&CTOSUNISYS_K5_SCAN,
	///* keynumber to scancode   */	&CTOSUNISYS_K4_SCAN,
	//* make/break info         */ CTOSUNISYS_KB100_MKBR,
	/* make/break info         */ &CTOSUNISYS_K5_MKBR,
	/* short delay      (uSec) */	  500,
	/* medium delay     (uSec) */	 5000,
	/* long delay       (uSec) */	30000,
	/* interchar delay  (uSec) */	 5000,
	/* lshift  	  (key number) */	0,
	/* rshift  	  (key number) */	57,
	/* lcntrl  	  (key number) */	58,
	/* rcntrl  	  (key number) */	64,
	/* lalt    	  (key number) */	60,
	/* ralt    	  (key number) */	62,
	/* lcompose	  (key number) */	0,
	/* rcompose	  (key number) */	0,
	/* capslock	  (key number) */	0,
	/* numlock 	  (key number) */	0
};

const CWedgeTerminal BURROUGHS_K4_TERM =
{
	/* table format version    */	    4,
	/* capabilities            */	0,
	/* ANSI to keynumber       */	&BURROUGHS_K4_Layout,
	/* international info      */	&USA_Only_FixupPtr,
	/* keynumber to scancode   */	&BURROUGHS_K4_SCAN,
	/* make/break info         */ &BURROUGHS_K4_MKBR,

	/* short delay      (uSec) */	  500,
	/* medium delay     (uSec) */	 5000,
	/* long delay       (uSec) */	30000,
	/* interchar delay  (uSec) */	 5000,

	/* lshift  	  (key number) */	128,	 /* the normal shift used.  */
	/* rshift  	  (key number) */	129,	 /* the shift that is used for the ~ key  5/3/99 actual left shift scan code is 36H, not used though becasue if this codigo thing*/
	/* lcntrl  	  (key number) */	0,
	/* rcntrl  	  (key number) */	0,
	/* lalt    	  (key number) */	0,
	/* ralt    	  (key number) */	0,
	/* lcompose	  (key number) */	0,
	/* rcompose	  (key number) */	0,
	/* capslock	  (key number) */	0,
	/* numlock 	  (key number) */	0
};

const CWedgeTerminal BURROUGHS_K5_TERM =
{
	/* table format version    */	    4,
	/* capabilities            */	0,
	/* ANSI to keynumber       */	&BURROUGHS_K5_Layout,
	/* international info      */	&USA_Only_FixupPtr,
	/* keynumber to scancode   */	&BURROUGHS_K5_SCAN,
	/* make/break info         */ &BURROUGHS_K5_MKBR,

	/* short delay      (uSec) */	  500,
	/* medium delay     (uSec) */	 5000,
	/* long delay       (uSec) */	30000,
	/* interchar delay  (uSec) */	 5000,

	/* lshift  	  (key number) */	128,
	/* rshift  	  (key number) */	0,
	/* lcntrl  	  (key number) */	0,
	/* rcntrl  	  (key number) */	0,
	/* lalt    	  (key number) */	0,
	/* ralt    	  (key number) */	0,
	/* lcompose	  (key number) */	0,
	/* rcompose	  (key number) */	0,
	/* capslock	  (key number) */	0,
	/* numlock 	  (key number) */	0
};

extern const CMakeBreaks IBM_AT_MKBR;

const CWedgeTerminal JAP_106_TERM =
{
	/* table format version    */	    4,
	/* capabilities            */	0|CAP_CNTRL_MSDOS|CAP_ALT_NUMPAD_DOS|CAP_ALT_NUMPAD_WIN|CAP_SUPPORT_DIRCON,
	/* ANSI to keynumber       */	&JAP_106_COLL_Layout,
	/* international info      */	&IBM_AT_FixupPtr,
	/* keynumber to scancode   */	&JAP_106_SCAN,
	/* make/break info         */	&IBM_AT_MKBR,
	/* short delay      (uSec) */	  500,
	/* medium delay     (uSec) */	 5000,
	/* long delay       (uSec) */	30000,
	/* interchar delay  (uSec) */	 5000,
	/* lshift  	  (key number) */	44,
	/* rshift  	  (key number) */	57,
	/* lcntrl  	  (key number) */	58,
	/* rcntrl  	  (key number) */	64,
	/* lalt    	  (key number) */	60,
	/* ralt    	  (key number) */	62,
	/* lcompose	  (key number) */	0,
	/* rcompose	  (key number) */	0,
	/* capslock	  (key number) */	30,
	/* numlock 	  (key number) */	90
};

/*	 Id: USB */
/*	 Id: USB_DOS_V */
extern const CWedgeTerminal USB_JAP_106_TERM =
{
	/* table format version    */	    4,
	/* capabilities            */	CAP_CNTRL_MSDOS|CAP_ALT_NUMPAD_DOS|CAP_ALT_NUMPAD_WIN,
	/* ANSI to keynumber       */	&JAP_106_USB_Layout,
	/* international info      */	&JAP_106_USB_IBM_AT_FixupPtr,
	/* keynumber to scancode   */	&JAP_106_USB_KEYBOARD_SCAN,
	/* make/break info         */	&USB_KEYBOARD_MKBR,
	/* short delay      (uSec) */	  200,
	/* medium delay     (uSec) */	  200,
	/* long delay       (uSec) */	  200,
	/* interchar delay  (uSec) */	 2000,
	/* lshift  	  (key number) */	44,
	/* rshift  	  (key number) */	57,
	/* lcntrl  	  (key number) */	58,
	/* rcntrl  	  (key number) */	64,
	/* lalt    	  (key number) */	60,
	/* ralt    	  (key number) */	62,
	/* lcompose	  (key number) */	0,
	/* rcompose	  (key number) */	0,
	/* capslock	  (key number) */	30,
	/* numlock 	  (key number) */	90,
};

const CWedgeTerminal NEC_98_TERM =
{
	/* table format version    */	    4,
	/* capabilities            */	0,
	/* ANSI to keynumber       */	&NEC_98_COLL_Layout,
	/* international info      */	&IBM_AT_FixupPtr,
	/* keynumber to scancode   */	&NEC_98_SCAN,
	/* make/break info         */	&NEC_98_MKBR,
	/* short delay      (uSec) */	  500,
	/* medium delay     (uSec) */	 5000,
	/* long delay       (uSec) */	30000,
	/* interchar delay  (uSec) */	 5000,
	/* lshift  	  (key number) */	20,
	/* rshift  	  (key number) */	0,
	/* lcntrl  	  (key number) */	0,
	/* rcntrl  	  (key number) */	0,
	/* lalt    	  (key number) */	0,
	/* ralt    	  (key number) */	0,
	/* lcompose	  (key number) */	0,
	/* rcompose	  (key number) */	0,
	/* capslock	  (key number) */	30,
	/* numlock 	  (key number) */	90
};


