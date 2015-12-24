//======================================================================================
// RapidScanInterface.h
//======================================================================================
// $Source: Source/RapidScanInterface.h $
// $Revision: 1.4 $
// $Date: 2011/07/07 13:54:50EDT $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Module Interface Description:
//      This file contains the interface definitions to create and interact with 
//		the RapidScan decoder.
//
//---------------------------------------------------------------------------



#ifndef RAPIDSCAN_INTERFACE_H
#define RAPIDSCAN_INTERFACE_H


/* Preamble */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Decoder Functions
 *	These are the functions used to communicate to the decoder.
 */

int RSI_InitDecoder( int width, int height, int wrap );
void RSI_DestroyDecoder(void);

void RSI_ReportDecoderVersion( char *reportString );

int RSI_InstallCallback( unsigned int tag, void *function );

int RSI_Decode( void *image, int decodeImage );
int RSI_StopDecode(void);

int RSI_DecodeGet( unsigned int tag, void *data );
int RSI_DecodeSet( unsigned int tag, void *data );
int RSI_DecodeReset(void);

int RSI_GetLastDecoderError( void );

/* Postamble */
#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif //RAPIDSCAN_INTERFACE_H

