
/*----------------------------------------------------------------------*
 *	       This file conatins the plugin decoding processing		*
 *	       routines.									*
 *----------------------------------------------------------------------*/

//=================================================================================
//   $RCSfile: Gen6FoundationApp/FoundationApp/Plugin/PluginProcessDecoding.cpp $
//   $Date: 2011/08/11 22:14:40EDT $
//   $Revision: 1.6 $
//
//=================================================================================*

//=================================================================================

#include "PluginProcessDecoding.h"
#include "DecodeIFace.h"
#include "Menu.h"
#include "ReaderConfiguration.h"
#include "readerconfigurationstorage.h"
#include "ioman.h"
#include "scandrivermatrixinterface.h"
#include "db_chrio.h"
#include "ImageCore.h"
#include "productconfiguration.h"
#include "ScanEnginePSOC.h"
#include "HardwareToken.h"

BOOL bPluginHalted = 1;
extern BOOL bHalted;

#define STOP_DECODER bHalted = 1;
#define START_DECODER bHalted = 0;
#define STOP_DECODER_FLAG	bHalted

/************************************************************************/
//Global variables
/************************************************************************/
volatile HONPluginDecoderState  PluginDecoderState = HON_PLUGIN_DECODER_STATE_INIT; // Plugin decoder state variable
//static int PluginDecodeTry = 0;

#ifdef PLUGIN_DECODER
#if !defined( RFBASE )
static unsigned char* FissionImageData(unsigned char *pBuffer, unsigned long *Width, unsigned long *Height) 
{
	int NewWid = *Width;
	int NewHgt = *Height;
	ImageDesc Src, Dst;
	Src.ImageX = NewWid;
	Src.ImageY = NewHgt;
	Src.ImageBlueX = 6; //will make it flexible  			
	Src.ImageBlueY = 2; //will make it flexible
	Src.Color = MONOCOLOR;
	Src.ImageDepth = 8;
	Src.ImageData = (unsigned char *)pBuffer;
	Dst.ImageData=0;

	if(!pBuffer)
		return NULL;

	if (0 == ToGray(&Src, &Dst)) {
		*Width = Dst.ImageX; 
		*Height = Dst.ImageY;
		return (unsigned char *)Dst.ImageData;
	}

	return NULL;

}


static unsigned char* FusionImageData(unsigned char *pBuffer, unsigned long *Width, unsigned long *Height)
{
	ImageDesc Src, Dst;
	Src.ImageX = *Width;
	Src.ImageY = *Height;
	Src.ImageBlueX = 6; //will make it flexible  			
	Src.ImageBlueY = 2; //will make it flexible
	Src.Color = MONOCOLOR;
	Src.ImageDepth = 8;
	Src.ImageData = (unsigned char *)pBuffer;
	Dst.ImageData=0;

	if(!pBuffer)
		return NULL;

	if (0 == ToRGB(&Src, &Dst)) {
		int i;
		unsigned char *src = (unsigned char*)Dst.ImageData;
		unsigned char *dst = (unsigned char*)malloc(Dst.ImageX*Dst.ImageY*3);

		if(!dst){
			if(Dst.ImageData)
				free(Dst.ImageData);
			return NULL;
		}
		
		//!!!Convert to RGB ... the output of ToRGB follows format of R-block, G-block, B-block.
		for(i = 0; i < Dst.ImageX*Dst.ImageY; i++)
		{
			dst[3*i] = src[i];
			dst[3*i+1] = src[i+Dst.ImageX*Dst.ImageY];
			dst[3*i+2] = src[i+Dst.ImageX*Dst.ImageY*2];
		}
		
		*Width = Dst.ImageX; 
		*Height=Dst.ImageY;

		if(Dst.ImageData)
			free(Dst.ImageData);
		
		return (unsigned char *)dst;
	}

	return NULL;
}
#endif

/*----------------------------------------------------------------------*/
/*	This routine is called when decode plugins are configured to work with 	*/
/*	the system decoder together. A variable is used to describe the state 	*/
/*	of the decoder chain --- PluginDecoderState:						*/
/*	HON_PLUGIN_DECODER_STATE_INIT						 		*/
/*		Decoder calling should start from the begginning of the chain	 	*/
/*		we should first call system decoder for menu IDs. If the code	 	*/
/*		is menu state won't change.	If not a menu, state change to 	 	*/
/*		HON_PLUGIN_DECODER_STATE_NOTMENU.					 	*/
/*	HON_PLUGIN_DECODER_STATE_NOTMENU						 	*/
/*		Indicate that the code is not a menu, then call decoder chain	 	*/
/*		according to the order defined in configuration file. If decode fails, 	*/
/*		state change to HON_PLUGIN_DECODER_STATE_DECODEFAIL, else 	*/
/*		the state will change to HON_PLUGIN_DECODER_STATE_INIT.	 	*/
/*	HON_PLUGIN_DECODER_STATE_DECODEFAIL					 	*/
/*		In this state, we will try next decoder in the chain. If we reached 	*/
/*		the last decoder in the chain, then we will roll over to the 	 	*/
/*		begginning of the decoder chain and try the decoders in the chain 	*/
/*		again and again until a "stop" event (timeout or trigger released) 	*/
/*		occured.												 	*/
/*															 	*/
/*	The decode plugin will call PluginDecoderMayContinue() to check if it	*/
/*	should continue decoding or not. After get a result, decode plugin will	*/
/*	PluginDecoderProcessResult(), which will call ProcessResult() directly	*/
/*	to process decode result.										*/
/*----------------------------------------------------------------------*/
void DecodeWithPlugins (unsigned char *img)
{
#if !defined( RFBASE )
	static HONPlugin *Plugin = NULL;
	HONPluginRawInfo *PluginInfo;
	DecodeApi *DecodeApis;
	DWORD Stuff[] = {HHPSD_IMAGE_STAT_ILLUM_MAXIMUM,0,HHPSD_IMAGE_STAT_ILLUM_CLIP,0,HHPSD_IMAGE_STAT_HEIGHT,0,HHPSD_IMAGE_STAT_WIDTH,0,HHPSD_IMAGE_STAT_ILLUM_VALUE,0,HHPSD_IMAGE_STAT_IMAGE_NUMBER,0};
	DWORD nSize;
	unsigned char *ImagePtr = img;
	unsigned char *ImagePtr1 = NULL;
	unsigned long Wid, Hght;

	PLUGIN_TRACE(("Plugin decode process\r\n"))
				
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_IMAGE_ATTRIBUTES,(UCHAR *)&P_SCANBUFFER,0,(PBYTE)Stuff,sizeof(Stuff)/sizeof(DWORD),&nSize);
	Wid = Stuff[7];
	Hght = Stuff[5];
	
	if(!g_pDecodingPluginConfiguration || !g_pDecodingPluginConfiguration->GetFirstPlugin()
		|| !GET_SETTING(EnableDecodePlugins)){
		PLUGIN_TRACE(("No decode plugin chained, call system decoder\r\n"))
		decode(img);
		return;
	}

	switch(PluginDecoderState){
		// We should first check if the scanned img is menu code.
		case HON_PLUGIN_DECODER_STATE_INIT:
			PLUGIN_TRACE(("Call system decoder for menuing\r\n"))
			STOP_PLUGIN_DECODER;	// stop plugin decoder
			DecodeForMenuing(img);
			/* Set next plugin the header of the chain */
			PluginDecoderState = HON_PLUGIN_DECODER_STATE_NOTMENU;
			Plugin = g_pDecodingPluginConfiguration->GetFirstPlugin();
			return;
			
		case HON_PLUGIN_DECODER_STATE_NOTMENU:// default to call plugin one by one
		// Call next plugin.
			if(!Plugin){	// if it is the end of the plugin, turn over to the start of the plugin chain
				Plugin = g_pDecodingPluginConfiguration->GetFirstPlugin();
			}

			
			if(Plugin && Plugin == g_pDecodingPluginConfiguration->GetSystemDummy()){
				if(Plugin->GetChainOnExit() == HON_PLUGIN_CHAIN_ON_EXIT_CHAINALWAYS
					|| Plugin->GetChainOnExit() == HON_PLUGIN_CHAIN_ON_EXIT_CHAINIFFAILURE){
					// HON_PLUGIN_CHAIN_ON_EXIT_CHAINALWAYS and HON_PLUGIN_CHAIN_ON_EXIT_CHAINNEVER
					// would never get here
					// Use system decoder first.
					STOP_PLUGIN_DECODER;	// stop plugin decoder
					PLUGIN_TRACE(("Next decoder is system decoder\r\n"))
					decode(img);
				}
			}
			else if(Plugin){
				PluginInfo = Plugin->GetRawInfo();
				DecodeApis = (DecodeApi*)PluginInfo->PluginApis;
			
				if(PluginInfo && DecodeApis && DecodeApis->Decode
					&& (Plugin->GetChainOnExit() == HON_PLUGIN_CHAIN_ON_EXIT_CHAINIFFAILURE
					|| Plugin->GetChainOnExit() == HON_PLUGIN_CHAIN_ON_EXIT_CHAINALWAYS)){
					volatile int DecodeDataType = 0;	// default force to send mono data to decode plug-in
					volatile int Rev;
					PSoC_5100_REGISTER_TABLE Registers;
					
					STOP_DECODER;	// stop system decoder first
					START_PLUGIN_DECODER; // always clear bHalted!!
					PLUGIN_TRACE(("Next decoder is a plugin\r\n"))
						
					Rev = DecodeApis->RevisionNumber;
					if(Rev>2)
					{
						DecodeDataType = DecodeApis->GetDecodeDataType();
					}

					DecodeDataType = DecodeApis->GetDecodeDataType();
					if(DecodeDataType == 0){		// mono data
						if(HardwareToken.IAmOEMJade()) {
							if(Psoc5100GetRegisters( &Registers )){
								if(PSOC_5600_AIMER_IMAGER_JADE_MONOCOLOR == Registers.EngineID.b.Imager)
									ImagePtr1 = FissionImageData(ImagePtr, &Wid, &Hght);
							}else{
								PLUGIN_TRACE(("Failed to read PSOC regs\r\n"));
								return;
							}
						}else if(GET_CONFIG_SETTING(ImageSensor) == E2V_EV76C454_Jade_Monocolor)
							ImagePtr1 = FissionImageData(ImagePtr, &Wid, &Hght);
					}else if(DecodeDataType == 1){	// color data
						if(HardwareToken.IAmOEMJade()) {
							if(Psoc5100GetRegisters( &Registers )){
								if(PSOC_5600_AIMER_IMAGER_JADE_MONOCOLOR == Registers.EngineID.b.Imager)
									ImagePtr1 = FusionImageData(ImagePtr, &Wid, &Hght);
								else
									return;		// request color, we are mono sensor, just return
							}else{
								PLUGIN_TRACE(("Failed to read PSOC regs\r\n"));
								return;
							}
						}else if((GET_CONFIG_SETTING(ImageSensor) == E2V_EV76C454_Jade_Monocolor) &&
								(GET_CONFIG_SETTING(bColorFusion))){
							ImagePtr1 = FusionImageData(ImagePtr, &Wid, &Hght);
						}else {//if(GET_CONFIG_SETTING(ImageSensor) == E2V_EV76C454_Jade){
							return;			// request color, we are mono sensor, just return
						}
					}


					if(ImagePtr1 && Wid && Hght)
						DecodeApis->Decode(ImagePtr1, Wid, Hght);


					if(ImagePtr1)      
						free(ImagePtr1);    
				}
			}

			Plugin = Plugin->GetNextPlugin();
			return;

		default:
			return;
	}
#else
	return;
#endif
}

#endif



