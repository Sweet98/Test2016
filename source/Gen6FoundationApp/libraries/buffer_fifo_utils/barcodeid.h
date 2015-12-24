/*=================================================================================
  Symbology IDs

//=================================================================================
   $Source: barcodeid.h $
   $Date: 2011/03/11 16:21:46EST $
   $Revision:


//=================================================================================*/
//! \file

#ifndef BARCODE_ID_H__03164A21_6A2C_443B_BCD9_B1B934C58593__INCLUDED_
#define BARCODE_ID_H__03164A21_6A2C_443B_BCD9_B1B934C58593__INCLUDED_


/////////////////////////////////////////////////////////////////////////////
// --------------------------------------------------
//
//	These define the Welch Allyn Code ID
//	(not all listed symbologies are or will be
//	implemented)
//	--------------------------------------------------

//  These code IDs are defined in the controlled document 500028821.doc
//  also titled "SYMBOLOGY IDENTIFIER SPECIFICATION 500028821"

	#define WA_CODELETTER_CODABAR 'a'
	#define WA_CODELETTER_CODE39 'b'
	#define WA_CODELETTER_UPC 'c'
	#define WA_CODELETTER_EAN 'd'
	#define WA_CODELETTER_UPCE 'E'
	#define WA_CODELETTER_EAN8 'D'
	#define WA_CODELETTER_INTERLEAVED2OF5 'e'
	#define WA_CODELETTER_CODE2OF5 'f'
	#define WA_CODELETTER_MSI 'g'
	#define WA_CODELETTER_CODE11 'h'
	#define WA_CODELETTER_CODE93 'i'
	#define WA_CODELETTER_CODE128 'j'
	#define WA_CODELETTER_UCC128 'I'
	#define WA_CODELETTER_CODE49 'l'
	#define WA_CODELETTER_MATRIX2OF5 'm'
	#define WA_CODELETTER_PLESSEY 'n'
	#define WA_CODELETTER_CODE16K 'o'
	#define WA_CODELETTER_CHANNELCODE 'p'
	#define WA_CODELETTER_CHINESE_MATRIX2OF5 'q'
	#define WA_CODELETTER_PDF417 'r'
	#define WA_CODELETTER_MICROPDF 'R'
	#define WA_CODELETTER_STACKEDCHANNELCODE 's'
	#define WA_CODELETTER_TELEPEN 't'
	#define WA_CODELETTER_CODEZ 'u'
	#define WA_CODELETTER_CODE39BASE32 'w'
	#define WA_CODELETTER_RSS14 'y'
	#define WA_CODELETTER_RSSLIMITED 'y'
	#define WA_CODELETTER_RSSEXPANDED 'y'
	#define WA_CODELETTER_COMPOSITE 'y'
	#define WA_CODELETTER_AZTEC	'z'
	#define WA_CODELETTER_DATAMATRIX	'w'
	#define WA_CODELETTER_MAXICODE	'x'
	#define WA_CODELETTER_QRCODE	's'

	#define WA_CODELETTER_MENU_RESPONSE '6'
	#define WA_CODELETTER_QUANTITY_STRING '5'
/* the code id we use for the No Read message (I just picked a number TJK) */
	#define	NR_CODEID				156

//***********************************************************
// Define the AIM code letters
//***********************************************************
	#define AIM_CODELETTER_CODABAR					'F'
	#define AIM_CODELETTER_CODE39						'A'
	#define AIM_CODELETTER_UPC							'E'
	#define AIM_CODELETTER_EAN							'E'
	#define AIM_CODELETTER_INTERLEAVED2OF5			'I'
	#define AIM_CODELETTER_CODE2OF5					'S'
	#define AIM_CODELETTER_MSI							'M'
	#define AIM_CODELETTER_CODE11						'H'
	#define AIM_CODELETTER_CODE128					'C'
	#define AIM_CODELETTER_MATRIX2OF5				'X'
	#define AIM_CODELETTER_PLESSEY					'P'
	#define AIM_CODELETTER_CHINESE_MATRIX2OF5		'X'
	#define AIM_CODELETTER_TELEPEN					'B'
	#define AIM_CODELETTER_OTHER						'X'
	#define AIM_CODELETTER_RSS14						'e'
	#define AIM_CODELETTER_RSSLIMITED				'e'
	#define AIM_CODELETTER_RSSEXPANDED				'e'
	#define AIM_CODELETTER_PDF417						'L'
	#define AIM_CODELETTER_NONBARCODE				'Z'
	#define AIM_CODELETTER_MICROPDF					'L'
	#define AIM_CODELETTER_RSS14						'e'
	#define AIM_CODELETTER_AZTEC						'z'
	#define AIM_CODELETTER_DATAMATRIX				'd'
	#define AIM_CODELETTER_MAXICODE					'U'
	#define AIM_CODELETTER_QRCODE						'Q'
	#define AIM_CODELETTER_CODE93						'G'

	#define AIM_CODEMODIFIER_DEFAULT					'0'


/////////////////////////////////////////////////////////////////////////////

#endif // BARCODE_ID_H__03164A21_6A2C_443B_BCD9_B1B934C58593__INCLUDED_


