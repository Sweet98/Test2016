//======================================================================================
// AdaptusSettings.h
//======================================================================================
// $Source: AdaptusSettings.h $
// $Revision: 1.13 $
// $Date: 2011/07/07 13:22:41EDT $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Module Interface Description:
//      This file contains the configurable settings that apply only when the Adaptus decoder
//		is used.
//
//---------------------------------------------------------------------------



#ifndef ADAPTUS_SETTINGS_H
#define ADAPTUS_SETTINGS_H

/* Preamble */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Decoder Menu Types 
 *	These are the expected type for each command. They are embedded in the menu tag so 
 *	the user knows what type of information to pass into the menu function
 */
#define MENU_TYPE_INT		0x10000000
#define MENU_TYPE_STRING	0x20000000
#define MENU_TYPE_BITFIELD	0x40000000
#define MENU_TYPE_MASK		0xF0000000


/* System Control Settings
 *	These settings are general decoder control. 
 */
 
// Basic
#define DEC_PRINT_WEIGHT						(MENU_TYPE_INT + 0x0C001001)
#define DEC_PRINT_LOCALIZATION					(MENU_TYPE_INT + 0x0C001002)
#define DEC_FIXED_MOUNT_MODE					(MENU_TYPE_INT + 0x0C001003)
#define DEC_SCAN_ZONE							(MENU_TYPE_INT + 0x0C001004)
#define DEC_DECODE_BOX							(MENU_TYPE_INT + 0x0C00100A)
#define DEC_DECODE_BOX_HEIGHT					(MENU_TYPE_INT + 0x0C00100B)
#define DEC_DECODE_BOX_WIDTH					(MENU_TYPE_INT + 0x0C00100C)
#define DEC_DECODE_WIDTH						(MENU_TYPE_INT + 0x0C00100D)
#define DEC_STACKED_CAPABLE						(MENU_TYPE_INT + 0x0C00100E)
#define DEC_MATRIX_CAPABLE						(MENU_TYPE_INT + 0x0C00100F)
#define DEC_IMAGE_COLOR							(MENU_TYPE_INT + 0x0C001014)
#define DEC_IMAGE_MAX_WHITE_VALUE				(MENU_TYPE_INT + 0x0C001015)
#define DEC_IMAGE_WHITE_CLIP_VALUE				(MENU_TYPE_INT + 0x0C001016)

// Advanced
#define DEC_DECODE_SEARCH_MODE					(MENU_TYPE_INT + 0x0C001017)
#define DEC_ALD_SLIT_HEIGHT						(MENU_TYPE_INT + 0x0C001018)
#define DEC_ALD_START_POINT_X					(MENU_TYPE_INT + 0x0C001019)
#define DEC_ALD_START_POINT_Y					(MENU_TYPE_INT + 0x0C00101B)
#define DEC_ALD_STOP_POINT_X					(MENU_TYPE_INT + 0x0C00101C)
#define DEC_ALD_STOP_POINT_Y					(MENU_TYPE_INT + 0x0C00101D)
#define DEC_ALD_SPACING							(MENU_TYPE_INT + 0x0C00101E)
#define DEC_ALD_ATTEMPTS						(MENU_TYPE_INT + 0x0C00101F)
#define DEC_ALD_VOTE							(MENU_TYPE_INT + 0x0C001020)
#define DEC_ADAPTIVE_DECODE_ENABLED				(MENU_TYPE_INT + 0x0C001021)
#define DEC_PRIORITY_SYMBOLOGIES				(MENU_TYPE_STRING + 0x0C001022)
#define DEC_ENGINE_ORIENTATION					(MENU_TYPE_INT + 0x0C001023)
#define DEC_REFLECTIONS_ENABLED					(MENU_TYPE_INT + 0x0C001024)
#define DEC_ZOOM								(MENU_TYPE_INT + 0x0C001025)

// Other Modes
#define DEC_PRINT_QUAL_ASSESS_MODE				(MENU_TYPE_INT + 0x0C001026)
#define DEC_SUDOKU_ENABLED						(MENU_TYPE_INT + 0x0C001027)

// Customer Specific
#define DEC_AUS_POST_QUAL_ASSESS_MODE			(MENU_TYPE_INT + 0x0C001028)

//Wait for EWR to add functionality
//#define DEC_CYTYC_MODE							(MENU_TYPE_INT + 0x0C0)

/* Symbology Specific Settings
 *	These settings are specific for each symbology.
 */

/* Linear Symbologies */

//Label Code 4 & 5
#define DEC_LABEL_CODE_ENABLED					(MENU_TYPE_INT + 0x0C032001)

//Posicode
#define DEC_POSICODE_ENABLED					(MENU_TYPE_INT + 0x0C033001)
#define DEC_POSICODE_MIN_LENGTH					(MENU_TYPE_INT + 0x0C033002)
#define DEC_POSICODE_MAX_LENGTH					(MENU_TYPE_INT + 0x0C033003)
#define DEC_POSICODE_LIMITED_MODE				(MENU_TYPE_INT + 0x0C033004)

/* Stacked Linear Symbologies */

//Code 16K
#define DEC_CODE16_ENABLED						(MENU_TYPE_INT + 0x0C034001)
#define DEC_CODE16_MIN_LENGTH					(MENU_TYPE_INT + 0x0C034002)
#define DEC_CODE16_MAX_LENGTH					(MENU_TYPE_INT + 0x0C034003)

//Code 49
#define DEC_CODE49_ENABLED						(MENU_TYPE_INT + 0x0C035001)
#define DEC_CODE49_MIN_LENGTH					(MENU_TYPE_INT + 0x0C035002)
#define DEC_CODE49_MAX_LENGTH					(MENU_TYPE_INT + 0x0C035003)

#define DEC_PDF417_CODEWORD_TICK				(MENU_TYPE_INT + 0x0C024001)

//Disabled Commands
#define DEC_PDF417_LEARN_MODE_ENABLED			(MENU_TYPE_INT + 0x0C024002)

/* Matrix 2D Symbologies */

//Aztec Code
#define DEC_AZTEC_RUNES_ENABLED					(MENU_TYPE_INT + 0x0C027001)
#define DEC_AZTEC_MESA_CODE128_ENABLED			(MENU_TYPE_INT + 0x0C027002)
#define DEC_AZTEC_MESA_CODE39_ENABLED			(MENU_TYPE_INT + 0x0C027003)
#define DEC_AZTEC_MESA_CODE93_ENABLED			(MENU_TYPE_INT + 0x0C027004)
#define DEC_AZTEC_MESA_UPCA_ENABLED				(MENU_TYPE_INT + 0x0C027005)
#define DEC_AZTEC_MESA_EAN13_ENABLED			(MENU_TYPE_INT + 0x0C027006)
#define DEC_AZTEC_MESA_I25_ENABLED				(MENU_TYPE_INT + 0x0C027007)

//Maxicode
#define DEC_MAXICODE_REF_TARGET_ENABLED			(MENU_TYPE_INT + 0x0C028001)

//Grid Matrix
#define DEC_GM_ENABLED							(MENU_TYPE_INT + 0x0C036001)
#define DEC_GM_MIN_LENGTH						(MENU_TYPE_INT + 0x0C036002)
#define DEC_GM_MAX_LENGTH						(MENU_TYPE_INT + 0x0C036003)
#define DEC_GM_APPEND_ENABLED					(MENU_TYPE_INT + 0x0C036004)

//Vericode
#define DEC_VERICODE_ENABLED					(MENU_TYPE_INT + 0x0C037001)
#define DEC_VERICODE_MIN_LENGTH					(MENU_TYPE_INT + 0x0C037002)
#define DEC_VERICODE_MAX_LENGTH					(MENU_TYPE_INT + 0x0C037003)
#define DEC_VERICODE_SIZE_A						(MENU_TYPE_INT + 0x0C037004)
#define DEC_VERICODE_SIZE_B						(MENU_TYPE_INT + 0x0C037005)
#define DEC_VERICODE_SIZE_C						(MENU_TYPE_INT + 0x0C037006)
#define DEC_VERICODE_SIZE_D						(MENU_TYPE_INT + 0x0C037007)
#define DEC_VERICODE_SIZE_E						(MENU_TYPE_INT + 0x0C037008)


/* Postal Symbologies */

//BNB-62 (S18C)
#define DEC_BNB_ENABLED							(MENU_TYPE_INT + 0x0C038001)
#define DEC_BNB_HALF_HOUR_FORMAT				(MENU_TYPE_INT + 0x0C038002)
#define DEC_BNB_MACHINE_ID_FORMAT				(MENU_TYPE_INT + 0x0C038003)

//OCR
#define DEC_OCR_GROUP_G							(MENU_TYPE_STRING + 0x0C02D001)
#define DEC_OCR_GROUP_H							(MENU_TYPE_STRING + 0x0C02D002)
#define DEC_OCR_CHECK_SET						(MENU_TYPE_STRING + 0x0C02D003)

//BC412
#define DEC_BC412_ENABLED						(MENU_TYPE_INT + 0x0C039001)
#define DEC_BC412_MIN_LENGTH					(MENU_TYPE_INT + 0x0C039002)
#define DEC_BC412_MAX_LENGTH					(MENU_TYPE_INT + 0x0C039003)

//Code Z (GTech)
#define DEC_CODEZ_ENABLED						(MENU_TYPE_INT + 0x0C03A001)
#define DEC_CODEZ_MIN_LENGTH					(MENU_TYPE_INT + 0x0C03A002)
#define DEC_CODEZ_MAX_LENGTH					(MENU_TYPE_INT + 0x0C03A003)

//Secure Code
#define DEC_SECURE_ENABLED						(MENU_TYPE_INT + 0x0C03B001)
#define DEC_SECURE_MIN_LENGTH					(MENU_TYPE_INT + 0x0C03B002)
#define DEC_SECURE_MAX_LENGTH					(MENU_TYPE_INT + 0x0C03B003)
#define DEC_SECURE_APPLICATION_KEY				(MENU_TYPE_STRING + 0x0C03B004)
#define DEC_SECURE_VENDOR_KEY					(MENU_TYPE_STRING + 0x0C03B005)

//Dot Code
#define DEC_DOT_ENABLED							(MENU_TYPE_INT + 0x0C03C001)
#define DEC_DOT_MIN_LENGTH						(MENU_TYPE_INT + 0x0C03C002)
#define DEC_DOT_MAX_LENGTH						(MENU_TYPE_INT + 0x0C03C003)

//DPC Cirrus
#define DEC_DPC_CIRRUS_ENABLED					(MENU_TYPE_INT + 0x0C03D001)

//Plessey Code
#define DEC_PLESSEY_ENABLED						(MENU_TYPE_INT + 0x0C03E001)
#define DEC_PLESSEY_MIN_LENGTH					(MENU_TYPE_INT + 0x0C03E002)
#define DEC_PLESSEY_MAX_LENGTH					(MENU_TYPE_INT + 0x0C03E003)
#define DEC_PLESSEY_CHECK_DIGIT_ENABLED			(MENU_TYPE_INT + 0x0C03E004)


/* Postamble */
#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif //ADAPTUS_SETTINGS_H

