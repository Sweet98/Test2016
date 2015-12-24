//======================================================================================
// UnifiedLaserSettings.h
//======================================================================================
// $Source: UnifiedLaserSettings.h $
// $Revision: 1.3 $
// $Date: 2011/07/07 13:22:41EDT $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Module Interface Description:
//      This file contains the configurable settings that apply only when the Unified Laser
//		decoder is used. 
//
//---------------------------------------------------------------------------



#ifndef UNIFIED_LASER_SETTINGS_H
#define UNIFIED_LASER_SETTINGS_H

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
#define DEC_INPUT_SATURATION					(MENU_TYPE_INT + 0x0D001001)
#define DEC_INPUT_TYPE							(MENU_TYPE_INT + 0x0D001002)
#define DEC_INPUT_TIME_MS						(MENU_TYPE_INT + 0x0D001003)
#define DEC_INPUT_CHANNEL						(MENU_TYPE_INT + 0x0D001004)
#define DEC_MISC_TAG							(MENU_TYPE_INT + 0x0D001005)
#define DEC_INPUT_MLDD_POSITION					(MENU_TYPE_INT + 0x0D001006)
#define DEC_INPUT_MLDD_EDGE_STRENGTH			(MENU_TYPE_INT + 0x0D001007)
#define DEC_INPUT_DMLDD_POINTER					(MENU_TYPE_INT + 0x0D001008)
#define DEC_INPUT_DMLDD_FRAME_NUMBER			(MENU_TYPE_INT + 0x0D001009)
#define DEC_PARSE_BUFFER						(MENU_TYPE_INT + 0x0D001010)
#define DEC_INPUT_PARSE_THRESHOLD				(MENU_TYPE_INT + 0x0D001011)
#define DEC_PARSE_NUM_RUNLENGTHS				(MENU_TYPE_INT + 0x0D001012)
#define DEC_ULD_IMAGE_POINTER					(MENU_TYPE_INT + 0x0D001013)
#define DEC_ULD_EXPANDED_VERSION_ENABLED		(MENU_TYPE_INT + 0x0D001014)

//Additional Version Info
#define DEC_VERSION_COMPONENTS_AUTHORIZED		(MENU_TYPE_INT + 0x0D002001)
#define DEC_VERSION_COMPONENTS_PRESENT			(MENU_TYPE_INT + 0x0D002002)
#define DEC_VERSION_CONFIGURATION				(MENU_TYPE_INT + 0x0D002003)
#define DEC_VERSION_COPYRIGHTS					(MENU_TYPE_INT + 0x0D002004)
#define DEC_VERSION_TRADEMARKS					(MENU_TYPE_INT + 0x0D002005)
#define DEC_VERSION_COMMENTS					(MENU_TYPE_INT + 0x0D002006)
#define DEC_VERSION_COMPANY_NAME				(MENU_TYPE_INT + 0x0D002007)
#define DEC_VERSION_PRODUCT_NAME				(MENU_TYPE_INT + 0x0D002008)
#define DEC_VERSION_TIMESTAMP					(MENU_TYPE_INT + 0x0D002009)
	
/* Symbology Specific Settings
 *	These settings are specific for each symbology.
 */

//Codabar
#define DEC_CODABAR_MISC						(MENU_TYPE_INT + 0x0D01F001)

//Code 128
#define DEC_CODE128_MISC						(MENU_TYPE_INT + 0x0D014001)
#define DEC_CODE128_COLLAPSE_VOIDS				(MENU_TYPE_INT + 0x0D014002)
#define DEC_CODE128_STITCHING_ENABLED			(MENU_TYPE_INT + 0x0D014003)
#define DEC_CODE128_STITCHING_TIMEOUT			(MENU_TYPE_INT + 0x0D014004)

//Code 39
#define DEC_CODE39_MISC							(MENU_TYPE_INT + 0x0D016001)
#define DEC_CODE39_COLLAPSE_VOIDS				(MENU_TYPE_INT + 0x0D016002)

//Interleaved 2 of 5
#define DEC_I25_COLLAPSE_VOIDS					(MENU_TYPE_INT + 0x0D019001)
#define DEC_I25_ALLOW_ODD_DIGIT					(MENU_TYPE_INT + 0x0D019002)
	
//UPC Misc
#define DEC_UPC_STITCH_OVERLAP					(MENU_TYPE_INT + 0x0D006001)
#define DEC_UPC_STITCH_EXTENDED_OVERLAP			(MENU_TYPE_INT + 0x0D006002)
#define DEC_UPC_UNCERTAINTY_ROUNDING			(MENU_TYPE_INT + 0x0D006003)
#define DEC_UPC_COLLAPSE_VOIDS					(MENU_TYPE_INT + 0x0D006004)

//UPC-A
#define DEC_UPCA_STITCHING_ENABLED				(MENU_TYPE_INT + 0x0D010001)
	
//UPC-E
#define DEC_UPCE_STITCHING_ENABLED				(MENU_TYPE_INT + 0x0D011001)
	
//EAN-8
#define DEC_EAN8_STITCHING_ENABLED				(MENU_TYPE_INT + 0x0D012001)
	
//EAN-13
#define DEC_EAN13_STITCHING_ENABLED				(MENU_TYPE_INT + 0x0D013001)
	
//RSS
#define DEC_RSS_14_STITCHING_TIMEOUT			(MENU_TYPE_INT + 0x0D022001)
#define DEC_RSS_EXPANDED_STITCHING_TIMEOUT		(MENU_TYPE_INT + 0x0D022002)
#define DEC_RSS_CODEWORD_VOTING					(MENU_TYPE_INT + 0x0D022003)
#define DEC_RSS_MISC							(MENU_TYPE_INT + 0x0D022004)

//S 2 of 5
#define DEC_S25_MISC							(MENU_TYPE_INT + 0x0D01A001)

//Korea Post
#define DEC_KOREA_POST_MISC						(MENU_TYPE_INT + 0x0D100001)

//Telepen
#define DEC_TELEPEN_MISC						(MENU_TYPE_INT + 0x0D020001)

/* Postamble */
#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif //UNIFIED_LASER_SETTINGS_H

