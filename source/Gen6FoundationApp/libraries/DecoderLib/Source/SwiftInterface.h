//======================================================================================
// SwiftInterface.h
//======================================================================================
// $Source: Source/SwiftInterface.h $
// $Revision: 1.9 $
// $Date: 2011/07/07 13:54:43EDT $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Module Interface Description:
//      This file contains the interface definitions to create and interact with 
//		the Swift decoder.
//
//---------------------------------------------------------------------------



#ifndef SWIFT_INTERFACE_H
#define SWIFT_INTERFACE_H

#include "sd.h"
#include "sd_op.h"

/* Preamble */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Decoder Functions
 *	These are the functions used to communicate to the decoder.
 */

int SI_InitDecoder( int width, int height, int wrap );
void SI_DestroyDecoder( int decoder );

void SI_ReportDecoderVersion( int decoder, char *reportString );

int SI_InstallCallback( int decoder, /*CallbackTag*/ unsigned int tag, void *function );

int SI_Decode( int decoder, void *image );
int SI_StopDecode( int decoder );
int SI_ContinueDecode( int decoder );
int SI_PauseDecode( int decoder );

int SI_DecodeGet( int decoder, unsigned int tag, void *data );
int SI_DecodeSet( int decoder, unsigned int tag, void *data );
void SI_DecodeReset( int decoder );

int SI_GetLastDecoderError( void );

/* Postamble */
#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif //SWIFT_INTERFACE_H

