//======================================================================================
// DecoderConfig.h
//======================================================================================
// $Source: DecoderConfig.h $
// $Revision: 1.4 $
// $Date: 2011/08/18 16:25:39EDT $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Module Interface Description:
//      This file contains the default configuration of the components compiled into
//		the control logic.
//
//---------------------------------------------------------------------------



#ifndef DECODER_CONFIG_H
#define DECODER_CONFIG_H


/* Preamble */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//Decoders
#define OVERRIDE_BUILD	0	//For the decoders only, this is meant to be an alternative method for specifying which decoders should be included. 
							//The primary, and suggested method is to use the make file to specify the type of decoders in the compile defines in whatever
							//build system being used.

#if (OVERRIDE_BUILD)
#undef DECDOER_ID
#undef DECODER_AD
#undef DECODER_FLD
#undef DECODER_ULD
#endif

/* Integrated Decoder
	Description:
*/
#ifndef DECODER_ID
#define DECODER_ID	1
#endif

/* Adaptus Decoder
	Description:
*/
#ifndef DECODER_AD
#define DECODER_AD	0
#endif

/* Fast Linear Decoder
	Description:
*/
#ifndef DECODER_FLD
#define DECODER_FLD	1
#endif

/* Unified Laser Decoder
	Description:
*/
#ifndef DECODER_ULD
#define DECODER_ULD	0
#endif


//Other Components

#define APPEND_MODES	1
#define WINDOWING		1
#define WINDOWING_DEBUG	1
#define VIDEO_REVERSE	1
#define OCR_SEPARATE	1
#define MONOCOLORING	1


#define OLD_STRUCTURE	1
/* Postamble */
#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif // DECODER_CONFIG_H

