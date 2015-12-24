/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  Date:       1/14/2011
//  Tools:      RVCT Build 739
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: LicenseControlMenuSettings.cpp $
// $Revision: 1.12 $
// $Date: 2011/11/10 04:44:45EST $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// File:  LicenseControlMenuSettings.cpp
//		
//		File Contains functions and menu settings for enabling licensable 
//			features.  Menu settings which are activated by a license are 
//			configured here, but initialized in there respective files.
//
//------------------------------------------------------------------------------
#include "ReaderConfiguration.h"
#include "ProductConfiguration.h"
#include "LicenseControlMenuSettings.h"
#include "Menu.h"
#include "MenuFlash.h"	// Need this to get access to DeleteDefaultConfigurationFromFlash routine that zeroes out default
#include "HardwareToken.h"



#define RTLM_SUPPORTED 1

#ifdef RTLM_SUPPORTED
#include "AesEncryption.h"
#endif

// Routines which route diagnostic output to the correct interface (from PlugAndPlayMenuSettings.cpp).
extern void SendStringToInterface( const char *pucBuffer, int SizeOfBuffer);



// ===================================================================================
//  Type of Menu Command used in the LicensableCommandTable, Bool, Int, ...      
// -------------------------------------------------------------------------------------
typedef enum
{
  MENUTYPEBOOL,
  MENUTYPEINT
} ProductConfigType;

// ===================================================================================
//  Product Config Setting that controls the license option for each menu setting      
// -------------------------------------------------------------------------------------
typedef enum
{
  STACKEDLINEARSYMBOLOGIES,
  MATRIXSYMBOLOGIES,
  OCRAVAILABLE,
  PDFSYMBOLOGY,
  POSTALSYMBOLOGIES,
  EXTTRIGGERANDILLUMAVAILABLE
} ProductConfigSetting;

// ===================================================================================
//  Structure which configures the LicensableVisibilityCommandTable      
// -------------------------------------------------------------------------------------
typedef bool (*MenuCommandFunction_t)( void );
typedef struct _tagLicensableVisibilityCommandTable_t
{
	ProductConfigSetting	ProductConfigCommandName;
	const char			*pMenuCommandName;
	unsigned int		CommandModifier;
} LicensableVisibilityCommandTable_t;

// ===  Table  ======================================================================
//
//  Name:  LicensableVisibilityCommandTable
//
//  Description: Menu settings in this table are initially setup in other menu 
//		configuration files.  They are setup by default as if they were un-licensed.  
//		Values in this table will be executed when the menu command is licensed. 
//      
// -------------------------------------------------------------------------------------
LicensableVisibilityCommandTable_t LicensableVisibilityCommandTable[] =
{
	  //{Licensable by				Menu Setting,	Visibility Modifiers							}
	{	STACKEDLINEARSYMBOLOGIES,	"021F00" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	PDFSYMBOLOGY,				"021F01" ,		SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP	},
#if (DECODER_AD)
	{	STACKEDLINEARSYMBOLOGIES,	"021F03" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
#endif
	{	STACKEDLINEARSYMBOLOGIES,	"021F05" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"021F06" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"021F07" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022000" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022001" ,		SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022002" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022003" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022004" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022100" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022101" ,		SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022102" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022103" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022104" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022105" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022200" ,		SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP	},
#if (DECODER_ID) || (DECODER_FLD) || defined(RFBASE)	
	{	STACKEDLINEARSYMBOLOGIES,	"022300" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022301" ,		SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022302" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022303" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
#endif
	{	STACKEDLINEARSYMBOLOGIES,	"022400" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022401" ,		SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022402" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022403" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
#if (DECODER_AD)
	{	STACKEDLINEARSYMBOLOGIES,	"022500" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022501" ,		SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022502" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022503" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022600" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022601" ,		SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022602" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	STACKEDLINEARSYMBOLOGIES,	"022603" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"022D00" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"022E00" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"022F00" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023000" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023100" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023200" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
#endif
	{	MATRIXSYMBOLOGIES,			"023300" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023301" ,		SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP	},
#if (DECODER_ID) || defined(RFBASE)
	{	MATRIXSYMBOLOGIES,			"023302" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023303" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
#endif
#if (DECODER_AD)
	{	MATRIXSYMBOLOGIES,			"023304" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
#endif
	{	MATRIXSYMBOLOGIES,			"023305" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023306" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023307" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023400" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023401" ,		SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023402" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023403" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
#if (DECODER_ID) || defined(RFBASE)
	{	MATRIXSYMBOLOGIES,			"023404" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023405" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
#endif
	{	MATRIXSYMBOLOGIES,			"023600" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023601" ,		SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023602" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023603" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023604" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
#if (DECODER_ID) || defined(RFBASE)
	{	MATRIXSYMBOLOGIES,			"023605" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023606" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023607" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023608" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023609" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"02360A" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"02360B" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
#endif
	{	MATRIXSYMBOLOGIES,			"023700" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023701" ,		SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023702" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023703" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023704" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
#if (DECODER_ID) || defined(RFBASE)
	{	MATRIXSYMBOLOGIES,			"023705" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023706" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023707" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
#endif
	{	MATRIXSYMBOLOGIES,			"023800" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023801" ,		SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023802" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	MATRIXSYMBOLOGIES,			"023803" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
#if (DECODER_ID)|| defined(RFBASE)
	{	OCRAVAILABLE,				"024500" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	OCRAVAILABLE,				"024501" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	OCRAVAILABLE,				"024502" ,		SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::STRING_MUST_BE_QUOTED | WORKGROUP	},
	{	OCRAVAILABLE,				"024503" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	OCRAVAILABLE,				"024504" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
#else
	{	OCRAVAILABLE,				"024500" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	OCRAVAILABLE,				"024501" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	OCRAVAILABLE,				"024502" ,		SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::STRING_MUST_BE_QUOTED | WORKGROUP	},
	{	OCRAVAILABLE,				"024505" ,		SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::STRING_MUST_BE_QUOTED | WORKGROUP	},
	{	OCRAVAILABLE,				"024506" ,		SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::STRING_MUST_BE_QUOTED | WORKGROUP	},
	{	OCRAVAILABLE,				"024507" ,		SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::STRING_MUST_BE_QUOTED | WORKGROUP	},
#endif
	{	POSTALSYMBOLOGIES,			"021800" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	POSTALSYMBOLOGIES,			"021801" ,		SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP	},
	{	POSTALSYMBOLOGIES,			"021802" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	POSTALSYMBOLOGIES,			"021803" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	POSTALSYMBOLOGIES,			"023900" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	POSTALSYMBOLOGIES,			"023901" ,	 	SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP	},
	{	POSTALSYMBOLOGIES,			"023902" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	POSTALSYMBOLOGIES,			"023903" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
#if (DECODER_ID) || defined(RFBASE)
	{	POSTALSYMBOLOGIES,			"023904" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	POSTALSYMBOLOGIES,			"023905" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
#endif
	{	POSTALSYMBOLOGIES,			"023A00" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
	{	POSTALSYMBOLOGIES,			"023A01" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	POSTALSYMBOLOGIES,			"023B00" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
	{	POSTALSYMBOLOGIES,			"023B01" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	POSTALSYMBOLOGIES,			"023C00" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
	{	POSTALSYMBOLOGIES,			"023D00" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
	{	POSTALSYMBOLOGIES,			"023E00" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
	{	POSTALSYMBOLOGIES,			"023F00" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
	{	POSTALSYMBOLOGIES,			"024000" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
#if (DECODER_ID) || defined(RFBASE)
	{	POSTALSYMBOLOGIES,			"024001" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
#endif
	{	POSTALSYMBOLOGIES,			"024002" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	POSTALSYMBOLOGIES,			"024003" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	POSTALSYMBOLOGIES,			"024100" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
	{	POSTALSYMBOLOGIES,			"024200" ,		SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | WORKGROUP	},
#if (DECODER_AD)
	{	POSTALSYMBOLOGIES,			"024300" ,		SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP	},
	{	POSTALSYMBOLOGIES,			"024600" ,		SUB_TAG_TABLE::FULLY_VISIBLE | SUB_TAG_TABLE::SYMBOLOGY_ENABLE_COMMAND | WORKGROUP	},
#endif
	{ OCRAVAILABLE, NULL ,NULL} //Last entry in table 
};

// ===  Function  ======================================================================
//
//  Name:  IsLicensed
//
//  Description: Checks to see if the license is enabled
//      
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:   ProductConfigSetting MenuItem	Setting to check if it is licensed
//    in-out:
//       out:
//    return: bool								True = Licensed, False = Un-Licensed
// -------------------------------------------------------------------------------------
bool IsLicensed(ProductConfigSetting MenuItem)
{
#if defined( DEVELOPMENT_SETTINGS ) || defined( RFBASE )

	return true;
	
#elif defined( MANUFACTURING_IMAGER_TEST )
	if(MenuItem == OCRAVAILABLE)
	{
		return false;
	}
	else
	{
		return true;
	}
#else	
	
	bool Licensed = false;
	
	//Check to see if it is Licensed
	if(MenuItem == STACKEDLINEARSYMBOLOGIES)
	{
		Licensed = GET_CONFIG_SETTING(StackedLinearSymbologies);
	}
	else if(MenuItem == MATRIXSYMBOLOGIES)
	{
		Licensed = GET_CONFIG_SETTING(MatrixSymbologies);
	}
	else if(MenuItem == OCRAVAILABLE)
	{
		Licensed = GET_CONFIG_SETTING(OCRAvailable);
	}
	else if(MenuItem == PDFSYMBOLOGY)
	{
		Licensed = GET_CONFIG_SETTING(PDFSymbology);
	}
	else if(MenuItem == POSTALSYMBOLOGIES)
	{
		Licensed = GET_CONFIG_SETTING(PostalSymbologies);
	}
	else
	{
		Licensed = false;
	}
	return Licensed;
	
#endif //End of defined( DEVELOPMENT_SETTINGS ) || defined( RFBASE )
}

// ===================================================================================
//  Structure which configures the LicensableDefaultCommandTable      
// -------------------------------------------------------------------------------------
typedef bool (*MenuCommandFunction_t)( void );
typedef struct _tagLicensableDefaultCommandTable_t
{
	ProductConfigSetting	ProductConfigCommandName;
	ProductConfigType		MenuType;
	const char				*pMenuCommandName;
	int						DefaultValue;
} LicensableDefaultCommandTable_t;

// ===  Table  ======================================================================
//
//  Name:  LicensableDefaultCommandTable
//
//  Description: Menu settings in this table are initially setup in other menu 
//		configuration files.  They are setup by default as if they were un-licensed.  
//		Values in this table will be executed when the menu command is licensed. 
//      
// -------------------------------------------------------------------------------------
LicensableDefaultCommandTable_t LicensableDefaultCommandTable[] =
{
  //{Licensable by				MenuType, 	Menu Setting,	Default Value,		},
	{ PDFSYMBOLOGY, 			MENUTYPEBOOL, 	"021F01" ,	true 	},
#if (DECODER_AD)
	{ STACKEDLINEARSYMBOLOGIES, MENUTYPEBOOL, 	"022601" ,	true	},
#endif
	{ MATRIXSYMBOLOGIES, 		MENUTYPEBOOL, 	"023301" ,	true 	},
#if (DECODER_ID) || defined(RFBASE)
	{ MATRIXSYMBOLOGIES, 		MENUTYPEBOOL, 	"023302" ,	true 	},
#endif
	{ MATRIXSYMBOLOGIES, 		MENUTYPEBOOL, 	"023401" ,	false 	},
#if (DECODER_ID) || defined(RFBASE)	
	{ MATRIXSYMBOLOGIES, 		MENUTYPEBOOL, 	"023404" ,	true 	},
#endif
	{ MATRIXSYMBOLOGIES, 		MENUTYPEBOOL, 	"023601" ,	true 	},
#if (DECODER_ID) || defined(RFBASE)
	{ MATRIXSYMBOLOGIES, 		MENUTYPEINT , 	"023609" ,	2 		},
#endif
	{ MATRIXSYMBOLOGIES, 		MENUTYPEBOOL, 	"023701" ,	true 	},	
#if (DECODER_ID) || defined(RFBASE)
	{ MATRIXSYMBOLOGIES, 		MENUTYPEINT , 	"023706" ,	1		},
#endif
	{ PDFSYMBOLOGY, 			MENUTYPEBOOL, NULL 		,	NULL	} //Last entry in table 
};

// ===  Function  ======================================================================
//
//  Name:  ConfigureLicensableCommands
//
//  Description: Function goes through the LicensableCommandTable to reconfigure the 
//		menu settings based on its license status.  If the menu setting is licensed 
//		the function will change the modifiers and the custom default according to 
//		the table.
//      
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:   
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void READER_CONFIGURATION::ConfigureLicensableCommands(void)
{
	unsigned int uiIndex = 0;
	bool bDone = false;

	//Change the Visibility settings in the table
	for( uiIndex=0; !bDone; uiIndex++ )
    {
    	if(LicensableVisibilityCommandTable[uiIndex].pMenuCommandName == NULL) //Check to see we have made it through the table
    	{
    		bDone = true;
    	}
    	else
    	{	
    		if(IsLicensed(LicensableVisibilityCommandTable[uiIndex].ProductConfigCommandName))
    		{
	    		ReaderConfiguration->ChangeTypeFlags(LicensableVisibilityCommandTable[uiIndex].pMenuCommandName,LicensableVisibilityCommandTable[uiIndex].CommandModifier, true);
    		}
    	}
    } //end for()
	
	uiIndex = 0;
	bDone = false;
	//Change the Default settings in the table
	for( uiIndex=0; !bDone; uiIndex++ )
    {
		if(LicensableDefaultCommandTable[uiIndex].pMenuCommandName == NULL) //Check to see we have made it through the table
		{
			bDone = true;
		}
		else
		{	
			if(IsLicensed(LicensableDefaultCommandTable[uiIndex].ProductConfigCommandName))
			{
				if(LicensableDefaultCommandTable[uiIndex].MenuType == MENUTYPEBOOL) //ChangeDefault gets called differently depending on the input type
				{
					ReaderConfiguration->ChangeDefault( LicensableDefaultCommandTable[uiIndex].pMenuCommandName, (LicensableDefaultCommandTable[uiIndex].DefaultValue)? true : false, false, true);
				}
				else
				{
					ReaderConfiguration->ChangeDefault( LicensableDefaultCommandTable[uiIndex].pMenuCommandName, LicensableDefaultCommandTable[uiIndex].DefaultValue, false, true);
				}
			}
		}
	}		//end for()
#ifndef RFBASE   
	if(!HardwareToken.IAmOEMJade()) 
	{
		if((GET_CONFIG_SETTING(ImageSensor) == E2V_EV76C454_Jade_Monocolor) && (GET_CONFIG_SETTING(bColorFusion)==1))
		{
			ReaderConfiguration->ChangeNumericList("04082A",TRANSMIT_IMAGE_FORMAT_KIM, TRANSMIT_IMAGE_FORMAT_TIF_BINARY, TRANSMIT_IMAGE_FORMAT_TIF_GRP,
			TRANSMIT_IMAGE_FORMAT_TIF_GRAYSCALE, TRANSMIT_IMAGE_FORMAT_BINARY, TRANSMIT_IMAGE_FORMAT_GRAYSCALE,
			TRANSMIT_IMAGE_FORMAT_JPEG, TRANSMIT_IMAGE_FORMAT_OUTLINED_IMAGE, TRANSMIT_IMAGE_FORMAT_BMP, TRANSMIT_IMAGE_FORMAT_LOSSLESS_HUFFMAN,
			TRANSMIT_IMAGE_FORMAT_TIF_COLOR_COMPRESSED, TRANSMIT_IMAGE_FORMAT_TIF_COLOR_UNCOMPRESSED,
			TRANSMIT_IMAGE_FORMAT_JPEG_COLOR, TRANSMIT_IMAGE_FORMAT_BMP_COLOR, TRANSMIT_IMAGE_FORMAT_UNCOMPRESSED_RAW);
		}
		else
		{
			ReaderConfiguration->ChangeNumericList("04082A",TRANSMIT_IMAGE_FORMAT_KIM, TRANSMIT_IMAGE_FORMAT_TIF_BINARY, TRANSMIT_IMAGE_FORMAT_TIF_GRP,
			TRANSMIT_IMAGE_FORMAT_TIF_GRAYSCALE, TRANSMIT_IMAGE_FORMAT_BINARY, TRANSMIT_IMAGE_FORMAT_GRAYSCALE,
			TRANSMIT_IMAGE_FORMAT_JPEG, TRANSMIT_IMAGE_FORMAT_OUTLINED_IMAGE, TRANSMIT_IMAGE_FORMAT_BMP, TRANSMIT_IMAGE_FORMAT_LOSSLESS_HUFFMAN);
		}
	}
#endif
}



#ifdef RTLM_SUPPORTED

#define HHI_MAJOR_RTLM_VERSION 0x32
#define RTLM_ENCRYPTION_SYM_UID 0x31
#define RTLM_ENCRYPTION_SYM_GID 0x32
#define RTLM_HEADER_SIZE 6
#define CRYPTO_STRING_MAX 1024
#define PT_STRING_MAX 1024
#define CRYPTO_BLOCK_SIZE 16
#define PT_BLOCK_SIZE 16


//The following function decrypts the activation string and sends it to the product config

bool DecryptActivationKey(HSTRING * pActString)
{
		bool bTestReturn = false;
//		theStdInterface.printf("Decrypting\r\n");
	if(pActString->Char[3] == HHI_MAJOR_RTLM_VERSION)
	{ //Check the major version, HHI uses version 2.x		Check Encryption type - Symmetric based on unique identifier
//		theStdInterface.printf("Passed Major Ver\r\n");
		CAesEncryption pRTLMcrypt ; //get started with a new encryption class
		HSTRING keystring(16,true); //make a string to hold the encr. key
		HSTRING tempstring(16,true); //make a string to hold the temp PVT key for formatting		

		if (pActString->Char[5] == RTLM_ENCRYPTION_SYM_UID) //serial number (UID)
		{		
//			theStdInterface.printf("Serial Number\r\n");
			keystring.Copy(GET_CONFIG_SETTING(SerNum)); // set a default encryption key	
			keystring.UniCodeToSimpleAsciiFormat();
			tempstring.Copy(GET_CONFIG_SETTING(SymmetricKey)->Char); // get the PVT encryption key
			tempstring.BinaryToHex();		//Convert the hex pairs to binary for the following append
			keystring.Append(&tempstring); // add the UID + PVT KEY for full encryption key
		}
		else //assume Group ID
		{		
			char tempString[20];
//			theStdInterface.printf("Group ID\r\n");
			sprintf(tempString,"%05d",GET_CONFIG_SETTING(iGroupIdentifier));
			keystring.Copy(tempString);		
			tempstring.Copy(GET_CONFIG_SETTING(SymmetricKey)->Char); // get the PVT encryption key						
			tempstring.BinaryToHex();		//Convert the hex pairs to binary for the following append
			keystring.Append("98742"); // add the PVT KEY
			keystring.Append(&tempstring); // add the UID + PVT KEY for full encryption key
		}
//		theStdInterface.printf("Internal Key:\r\n");
//		theStdInterface.printf(keystring.Char, keystring.Size);
//		theStdInterface.printf("\r\n");		

		HSTRING EncodedString(CRYPTO_STRING_MAX,true); //Crypto-string goes in here
		HSTRING DecodedString(PT_STRING_MAX,true); //Decoded activation string goes in here
		bTestReturn = pRTLMcrypt.SetKey(keystring.Char,keystring.Size); //Go try to set the key
	
		if(bTestReturn) //did the setkey pass?
		{
//			theStdInterface.printf("SetKey Passed\r\n");
			EncodedString.Copy(pActString); 
			EncodedString.Remove(0, RTLM_HEADER_SIZE); //Get rid of header data
			EncodedString.HexToBinary(); //Convert crypto message
			unsigned char ucResultData[2048];
			bTestReturn = pRTLMcrypt.EcbDecrypt(EncodedString.Char, EncodedString.Size, ucResultData, PT_STRING_MAX);
			
			DecodedString.Copy(ucResultData);						
		}
		
		if(bTestReturn) //did the decrypt pass?
		{
			DecodedString.RemoveTrailingSpaceCharacters(); //get rid of the NULs, if any, from block padding

			for(int i=0; i<DecodedString.Size; i++)
			{
				if( DecodedString.Char[i] > 127) //Are there unprintable chars? Fail if there are.
				{
					bTestReturn = false;
//					theStdInterface.printf("Non-ASCII found at position: %d\r\n", i);
//				theStdInterface.printf("String: ");
//				theStdInterface.printf(DecodedString.Char);


					break;
				}	
			}
			if(bTestReturn) //Truly ASCII, proceed
			{
						
				DecodedString.Append(".");			
//				theStdInterface.printf("Into Config:");
//				theStdInterface.printf(DecodedString.Char);
				bTestReturn = (ProductConfiguration->Menu(&DecodedString,true,true) != 0); // send the data into ProductConfig
//				theStdInterface.printf("\r\nOut of Config:");
//				theStdInterface.printf(DecodedString.Char);
			
				switch (DecodedString.Char[DecodedString.Size-2])
				{
					case 0x06:		//got an ack from product config return true
						FlashProductConfigurationChanges(true);
//						theStdInterface.printf("\r\nProduct Config ACK\r\n");
						bTestReturn=true;
						break;
					default:			//got something else return false
						bTestReturn =false;
//						theStdInterface.printf("\r\nProduct Config - NO ACK\r\n");					
						break;
				}
			}
		}
	}

	return bTestReturn;
}


#define SCANNER_ACT_KEY '2'
#define BASE_ACT_KEY '3'

// takes product activation key and processes the information, decrypting the key and verifying that the key belongs to this device
bool ProcessActivationKey(void)
{
	bool Ret=false;
	

	HSTRING	DeviceName( "*", true );
	HSTRING	hstrCommandString("070100",true );
//	theStdInterface.printf("Activating\r\n");
	
	if((ReaderConfiguration->ExtraneousTextAfterCommand->Char[0] != 'L') || (ReaderConfiguration->ExtraneousTextAfterCommand->Char[1] != 'K'))
		return Ret; //license key must start with LK

//	theStdInterface.printf("Saw LK\r\n");

	if((ReaderConfiguration->ExtraneousTextAfterCommand->Char[2] == SCANNER_ACT_KEY))
	{	//use direct in corded or send to cordless scanner
//		theStdInterface.printf("FoundScannerKey\r\n");
#ifdef RFBASE
#else
		if(DecryptActivationKey(ReaderConfiguration->ExtraneousTextAfterCommand) && ReaderConfiguration->ExtraneousTextAfterCommand->Size)
			Ret = true;		// Scanner activation key processed internally, successfully
		
#endif
	}
	
	else if((ReaderConfiguration->ExtraneousTextAfterCommand->Char[2] == BASE_ACT_KEY))
	{
#ifdef RFSCAN
	
#else
		if(DecryptActivationKey(ReaderConfiguration->ExtraneousTextAfterCommand) && ReaderConfiguration->ExtraneousTextAfterCommand->Size)
			Ret = true;		
#endif
	}
	ReaderConfiguration->ConfigureLicensableCommands();
	
	return Ret;
}


/* SendActivationKeyStatus reports available RTLM licenses.*/
#define ENABLE_RESPONSE	"Enabled"
#define DISABLE_RESPONSE "Disabled"
#define NEWLINE_RESPONSE ",\n\r"

bool SendActivationKeyStatus(void)
{
	HSTRING cmd("",true);
	
		
	cmd.Append("Stacked Linear Symbologies = "); //PDF decoding
	if(TRUE == GET_CONFIG_SETTING(StackedLinearSymbologies))
	{
		cmd.Append(ENABLE_RESPONSE);
	}
	else
	{
		cmd.Append(DISABLE_RESPONSE);
	}
	cmd.Append(NEWLINE_RESPONSE);
	
	cmd.Append("Matrix Symbologies = "); //2D matrix decoding
	if(TRUE == GET_CONFIG_SETTING(MatrixSymbologies))
	{
		cmd.Append(ENABLE_RESPONSE);
	}
	else
	{
		cmd.Append(DISABLE_RESPONSE);
	}
	cmd.Append(NEWLINE_RESPONSE);
	
	cmd.Append("OCR = "); //OCR font reading
	if(TRUE == GET_CONFIG_SETTING(OCRAvailable))
	{
		cmd.Append(ENABLE_RESPONSE);
	}
	else
	{
		cmd.Append(DISABLE_RESPONSE);
	}
	cmd.Append(NEWLINE_RESPONSE);
	
	cmd.Append("Imaging Available = "); //Image capture
	if(TRUE == GET_CONFIG_SETTING(ImagingAvailable))
	{
		cmd.Append(ENABLE_RESPONSE);
	}
	else
	{
		cmd.Append(DISABLE_RESPONSE);
	}
	cmd.Append(NEWLINE_RESPONSE);
	
	cmd.Append("PDF Symbology = "); //PDF417 license 
	if(TRUE == GET_CONFIG_SETTING(PDFSymbology))
	{
		cmd.Append(ENABLE_RESPONSE);
	}
	else
	{
		cmd.Append(DISABLE_RESPONSE);
	}
	cmd.Append(NEWLINE_RESPONSE);
	
	cmd.Append("Plug-in Enable = "); //Ability to accept loading of TotalFreedom plug-ins 
	if(TRUE == GET_CONFIG_SETTING(PluginEnable))
	{
		cmd.Append(ENABLE_RESPONSE);
	}
	else
	{
		cmd.Append(DISABLE_RESPONSE);
	}
	cmd.Append(NEWLINE_RESPONSE);
	
	cmd.Append("Postal Symbologies = "); //Postal Symbologies decoding
	if(TRUE == GET_CONFIG_SETTING(PostalSymbologies))
	{
		cmd.Append(ENABLE_RESPONSE);
	}
	else
	{
		cmd.Append(DISABLE_RESPONSE);
	}
	cmd.Append(NEWLINE_RESPONSE);
	

//	cmd.Append("FIPS"); //Encryption Level Upgrade - Advanced level of encryption above Bluetooth (FIPS)*/
//	if(TRUE == GET_CONFIG_SETTING(FIPSAvailable))
//	{
//		cmd.Append(ENABLE_RESPONSE);
//	}
//	else
//	{
//		cmd.Append(DISABLE_RESPONSE)
//	}
//	cmd.Append(NEWLINE_RESPONSE);	
	
//o	High-resolution image capture (not yet defined)
//o	Color image capture (not yet defined) 

	SendStringToInterface((char *)cmd.Char,cmd.Size);
//	ReaderConfiguration->MenuCommandResponse->Append(&cmd);
	return true;
}
#endif // RTLM_SUPPORTED

void READER_CONFIGURATION::CreateLicenseMenuSettings( int /* ControlMask */ )
{
#ifdef RTLM_SUPPORTED
	// command for processing an activation key for RTLM
	AddCommand("070100",SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND | LOCAL_SETTING,&ProcessActivationKey,NULL);
	AddCommand("070101",SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING,&SendActivationKeyStatus,NULL);
#endif
}


