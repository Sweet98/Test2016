//======================================================================================
// PlugAndPlayMenuSettings.cpp
//======================================================================================
// $RCSfile: PlugAndPlayMenuSettings.cpp $
// $Revision: 1.91 $
// $Date: 2011/11/04 02:39:41EDT $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Module Description:
//      This file contains a routine that creates plug and play menu settings as part
//		of the overall reader configuration for a Matrix device.
//
//---------------------------------------------------------------------------
#include <string.h>
#include <stdlib.h>

#include "stdInclude.h"
#include "menu.h"
#include "ioman.h"
#include "dataedit.h"
#include "beep.h"
#include "readerconfiguration.h"
#include "db_sharedtable.h"
#include "db_chrio.h"
#include "productconfiguration.h"
#include "interface_support_functions.h"
#include "HardwareToken.h"
#include "MenuFlash.h"
#include "AutoCableSelect.h"

#ifndef RFBASE
//#include "trigger.h"
#include "scandrivermatrixinterface.h"
extern "C" {
#include "mxapi.h" // decoder revision
}
#if PLATFORM_CURRENT == imx25
#include "HardwareMacros.h"
#include "iMX25_GPIO.h"	//GPIO Defines
#include "iMX25_IOMUX.h"	//IO MUX Defines
#else
#error No Platform Defined!
#endif //endof PLATFORM_CURRENT
#endif // endof RFBASE

#include "BarcodeID.h"
#include "barcodedata.h"



#if !defined( RFBASE ) && !defined( VERIFIER )
#include	"ScanEnginePsoc.h"
#include "ScanDriverImagingInterface.h"
#endif

extern bool SendDataFormatCommands(void);
// fixme not needed?? extern bool SetupKioskDecodeMode(void); // go allow kiosk decode mode




bool	PlugAndPlayGetBufferedMessage(void);
bool	PlugAndPlayVisualMenu(void);
bool	PlugAndPlayVisualMenu2(void);
bool	PlugAndPlayVisualMenu3(void);
bool	PlugAndPlayFactoryTest(void);
bool	PlugAndPlay232(void);
bool	PlugAndPlayIBMAT(void);
bool	PlugAndPlayIBMPS2(void);
bool	PlugAndPlayEriaIBMPS2(void);
bool	PlugAndPlayLapTopDirect(void);
bool	PlugAndPlayCloneSlave(void);
bool	PlugAndPlayOPOS(void);
bool	SendRevisionInfo(void);
bool	SetSuffixCR(void);
bool	SendSize(void);
bool	SendSuffixBlockSize(void);
bool	SendDataFormatBlockSize(void);
bool	SetPrefixCodeID(void);
void	SendStringToInterface(const char *pucBuffer);
void	SendStringToInterface( const char *pucBuffer, int SizeOfBuffer);
bool	VisualMenu(int VisMnuType);

bool 	SendDecoderAllRevisions(void);
bool 	SendDecoderRevision(void);
bool 	SendScanDriverRevision(void);

#ifndef RFSCAN
extern void LoadWorkingConfigurationOne();
extern void LoadWorkingConfigurationZero();
#endif

extern int ParseNumberData();
extern bool ReCopyROMSettingsToRAM(int);


void SetPrefixNoneSuffixCRLF(void);
void SetPrefixNoneSuffixNone(void);
void SetPrefixNoneSuffixCR(void);
bool SetPrefixSubstitute(UCHAR Substitute);

/*-----------------6/24/2002 2:17PM-----------------
 * STUB FUNCTIONS
 * --------------------------------------------------*/
bool	PlugAndPlayCloneSlave()
{
	return FALSE;
}
bool PlugAndPlayOCIA_NCRShort()
{
	return FALSE;
}
bool PlugAndPlayOCIA_NCRLong()
{
	return FALSE;
}
bool PlugAndPlayOCIA_Nixdorf()
{
	return FALSE;
}

bool PlugAndPlay_Symbol_3300_HHLC()
{
	return FALSE;
}



/*----------------------------------------------------------------------------------------
PAP_BeeprLevelMax

  Sets all applicable Menu Settings to provide the user with the loudest beep.

  Parameters:  none

  Returns:  true if no errors, false otherwise
----------------------------------------------------------------------------------------*/
bool PAP_BeeprLevelMax(void)
{
	HSTRING MenuString("05021D$;05020D$&", true); // query default values & omit the <ACK>

	if ( ReaderConfiguration->Menu(&MenuString) != MENU_QUERY ) return false;
	MenuString.Remove(MenuString.Size - 1, 1);// Menu String now has the string for the defaults which should give us max beep volume--remove trailing '&'

	if ( ReaderConfiguration->CurrentTable == '@' ) MenuString.Append("@");
	else MenuString.Append(".");

	return ( ReaderConfiguration->Menu(&MenuString) == MENU_PASSED );
}





#if (defined(RFBASE) || defined(RFSCAN))
#else
bool AmIDirectLinkToHost(void)
{
	return false;
}
#endif


// Menu string for UPC settings in with retail interfaces
static const char RetailUPC[] = "0211011;0211021;0211031;0211040;0211050;0212011;0212020;0212030;0212041;0212051;0212060;0212070";
// Menu string for EAN settings in with retail interfaces
static const char RetailEAN[] = "0213011;0213021;0213030;0213040;0213070;0214011;0214021;0214030;0214040";
// Menu string for Addenda settings in with retail interfaces
static const char AddendaSeparator[] = "0211071;0212091;0214061;0213061";
// Menu string to clear pre and suffix
static const char PrefixNoneSuffixNone[] = "080404;080507";

//Standard Vismnu string for connection details
extern "C" const char VismnuDefaults[] = "0607029;0607032;0607070;0607040;0607050;0607060;080B000;080B010;080B020;080B030;0E01000;0605000!";
//Vismnu string for 40x0 (includes 232 INV)
extern "C" const char VismnuDefaults2[] = "0607029;0607032;0607070;0607040;0607050;0607060;06070D0;080B000;080B010;080B020;080B030;0E01000;0605000!";
//Vismnu string for USB Com emulation with serial number reporting on
extern "C" const char VismnuDefaults3[] = "080B000;080B010;080B020;080B030;0E01000;060A000;060A010;060A051;060500130!";
//Vismnu string for USB Com emulation with serial number reporting off
extern "C" const char VismnuDefaults4[] = "080B000;080B010;080B020;080B030;0E01000;060A000;060A010;060A050;060500130!";
const char * VisMnuPtr="";
bool interfaceController=false;
// ported formats (from the IT3800)
#define Max_Data_Format_Length 32

/*	Pre-coded prefix formats	*/
typedef struct
{
	unsigned short	Length;
	unsigned char	Data[Max_Data_Format_Length];
}
FormatString_t;
#ifdef INCLUDE_OCIA
/*	When defining formats count the number of bytes used by the keys and format commands and manually put it into the length	*/
/*	{Format Length, {Primary Key, Terminal Key, Code ID Key, Code Length MSB Key, Code Length LSB Key, Format Commands}}	*/
/*The Format Length may not exceed 'Max_Data_Format_Length' without changing its definition. */

static const FormatString_t NCR_Short_OCIA_EAN8  =
    {
        10, {0, TRM_OCIA, 'D', 0,  8, 0xF4, 0x0F, 0x02, 0xF1, 0x00}
    };
static const FormatString_t NCR_Short_OCIA_EAN13 =
    {
        10, {0, TRM_OCIA, 'd', 0, 13, 0xF4, 0x0F, 0x01, 0xF1, 0x00}
    };
static const FormatString_t NCR_Short_OCIA_UPCA  =
    {
        10, {0, TRM_OCIA, 'c', 0, 12, 0xF4, 0x0A, 0x01, 0xF1, 0x00}
    };
static const FormatString_t NCR_Short_OCIA_UPCE  =
    {
        11, {0, TRM_OCIA, 'E', 0,  8, 0xF4, 0x0E, 0x01, 0xF2, 0x07, 0x00}
    };

static const FormatString_t NCR_Long_OCIA_EAN8  =
    {
        10, {0, TRM_OCIA, 'D', 0,  8, 0xF4, 0x46, 0x02, 0xF1, 0x00}
    };
static const FormatString_t NCR_Long_OCIA_EAN13 =
    {
        10, {0, TRM_OCIA, 'd', 0, 13, 0xF4, 0x46, 0x01, 0xF1, 0x00}
    };
static const FormatString_t NCR_Long_OCIA_UPCA  =
    {
        10, {0, TRM_OCIA, 'c', 0, 12, 0xF4, 0x41, 0x01, 0xF1, 0x00}
    };
static const FormatString_t NCR_Long_OCIA_UPCE  =
    {
        11, {0, TRM_OCIA, 'E', 0,  8, 0xF4, 0x45, 0x01, 0xF2, 0x07, 0x00}
    };
static const FormatString_t NCR_Long_OCIA_C39   =
    {
        13, {0, TRM_OCIA, 'b', 0x27, 0x0F, 0xF4, 0x42, 0x01, 0xF4, 0x31, 0x01, 0xF1, 0x00}
    };
static const FormatString_t NCR_Long_OCIA_I25   =
    {
        13, {0, TRM_OCIA, 'e', 0x27, 0x0F, 0xF4, 0x42, 0x01, 0xF4, 0x32, 0x01, 0xF1, 0x00}
    };
static const FormatString_t NCR_Long_OCIA_128   =
    {
        13, {0, TRM_OCIA, 'j', 0x27, 0x0F, 0xF4, 0x42, 0x01, 0xF4, 0x33, 0x01, 0xF1, 0x00}
    };

static const FormatString_t Nixdorf_OCIA_EAN8  =
    {
        13, {0, TRM_OCIA, 'D', 0,  8, 0xF4, 0x44, 0x01, 0xF4, 0x4B, 0x01, 0xF1, 0x00}
    };
static const FormatString_t Nixdorf_OCIA_EAN13 =
    {
        13, {0, TRM_OCIA, 'd', 0, 13, 0xF4, 0x44, 0x01, 0xF4, 0x4B, 0x01, 0xF1, 0x00}
    };
static const FormatString_t Nixdorf_OCIA_UPCA  =
    {
        13, {0, TRM_OCIA, 'c', 0, 12, 0xF4, 0x44, 0x01, 0xF4, 0x4B, 0x01, 0xF1, 0x00}
    };
static const FormatString_t Nixdorf_OCIA_UPCE  =
    {
        14, {0, TRM_OCIA, 'E', 0,  8, 0xF4, 0x44, 0x01, 0xF4, 0x4B, 0x01, 0xF2, 0x07, 0x00}
    };
static const FormatString_t Nixdorf_OCIA_C39   =
    {
        13, {0, TRM_OCIA, 'b', 0x27, 0x0F, 0xF4, 0x44, 0x01, 0xF4, 0x49, 0x01, 0xF1, 0x00}
    };
static const FormatString_t Nixdorf_OCIA_I25   =
    {
        13, {0, TRM_OCIA, 'e', 0x27, 0x0F, 0xF4, 0x44, 0x01, 0xF4, 0x48, 0x01, 0xF1, 0x00}
    };
static const FormatString_t Nixdorf_OCIA_C25   =
    {
        13, {0, TRM_OCIA, 'f', 0x27, 0x0F, 0xF4, 0x44, 0x01, 0xF4, 0x47, 0x01, 0xF1, 0x00}
    };
static const FormatString_t Nixdorf_OCIA_128   =
    {
        13, {0, TRM_OCIA, 'j', 0x27, 0x0F, 0xF4, 0x44, 0x01, 0xF4, 0x4A, 0x01, 0xF1, 0x00}
    };

static const FormatString_t Spectra_Physics_OCIA_EAN8  =
    {
        10, {0, TRM_OCIA, 'D', 0,  8, 0xF4, 0x06, 0x02, 0xF1, 0x00}
    };
static const FormatString_t Spectra_Physics_OCIA_EAN13 =
    {
        10, {0, TRM_OCIA, 'd', 0, 13, 0xF4, 0x06, 0x01, 0xF1, 0x00}
    };
static const FormatString_t Spectra_Physics_OCIA_UPCA  =
    {
        10, {0, TRM_OCIA, 'c', 0, 12, 0xF4, 0x01, 0x01, 0xF1, 0x00}
    };
static const FormatString_t Spectra_Physics_OCIA_UPCE  =
    {
        11, {0, TRM_OCIA, 'E', 0,  8, 0xF4, 0x05, 0x01, 0xF2, 0x07, 0x00}
    };
#endif

static const FormatString_t IBM_4683_128_1 =
    {
        16, {0,99, 'j', 0x27, 0x0F, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x0A, 0x01, 0xF4, 0x0B, 0x01}
    };
static const FormatString_t IBM_4683_128_2 =
    {
        16, {0,99, 'j', 0x27, 0x0F, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x18, 0x01, 0xF4, 0x0B, 0x01}
    };

static const FormatString_t IBM_4683_UCC128_1 =
    {
        16, {0,99, 'I', 0x27, 0x0F, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x25, 0x01, 0xF4, 0x0B, 0x01}
    };
static const FormatString_t IBM_4683_UCC128_2 =
    {
        16, {0,99, 'I', 0x27, 0x0F, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x25, 0x01, 0xF4, 0x0B, 0x01}
    };

static const FormatString_t IBM_4683_Formats[] =
    {
        /* EAN8	   */	{7,  {0, 99, 'D',    0,    8, 0xF1, 0x0C}},
        /* EAN8_2  */	{19, {0, 99, 'D',    0,   11, 0xFB, 0x01, 0x20, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x17, 0x01, 0xF4, 0x0B, 0x01}},
        /* EAN8_5  */	{19, {0, 99, 'D',    0,   14, 0xFB, 0x01, 0x20, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x1D, 0x01, 0xF4, 0x0B, 0x01}},
        /* EAN13   */	{7,  {0, 99, 'd',    0,   13, 0xF1, 0x16}},
        /* EAN13_2 */	{19, {0, 99, 'd',    0,   16, 0xFB, 0x01, 0x20, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x13, 0x01, 0xF4, 0x0B, 0x01}},
        /* EAN13_5 */	{19, {0, 99, 'd',    0,   19, 0xFB, 0x01, 0x20, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x15, 0x01, 0xF4, 0x0B, 0x01}},
        /* UPCA	   */	{7,  {0, 99, 'c',    0,   12, 0xF1, 0x0D}},
        /* UPCA_2  */	{19, {0, 99, 'c',    0,   15, 0xFB, 0x01, 0x20, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x16, 0x01, 0xF4, 0x0B, 0x01}},
        /* UPCA_5  */	{19, {0, 99, 'c',    0,   18, 0xFB, 0x01, 0x20, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x11, 0x01, 0xF4, 0x0B, 0x01}},
        /* UPCE	   */	{7,  {0, 99, 'E',    0,    8, 0xF1, 0x0A}},
        /* UPCE_2  */	{19, {0, 99, 'E',    0,   11, 0xFB, 0x01, 0x20, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x12, 0x01, 0xF4, 0x0B, 0x01}},
        /* UPCE_5  */	{19, {0, 99, 'E',    0,   14, 0xFB, 0x01, 0x20, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x14, 0x01, 0xF4, 0x0B, 0x01}},
        /* UPCE Expanded    */  {7,  {0, 99, 'E',    0,   12, 0xF1, 0x0D}},
        /* UPCE Expanded    */  {19, {0, 99, 'E',    0,   15, 0xFB, 0x01, 0x20, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x16, 0x01, 0xF4, 0x0B, 0x01}},
        /* UPCE Expanded    */  {19, {0, 99, 'E',    0,   18, 0xFB, 0x01, 0x20, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x11, 0x01, 0xF4, 0x0B, 0x01}},
        /* CBAR	   */	{16, {0, 99, 'a', 0x27, 0x0F, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x0E, 0x01, 0xF4, 0x0B, 0x01}},
        /* C39	   */	{16, {0, 99, 'b', 0x27, 0x0F, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x0A, 0x01, 0xF4, 0x0B, 0x01}},
        /* Std2of5 */	{16, {0, 99, 'f', 0x27, 0x0F, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x0C, 0x01, 0xF4, 0x0B, 0x01}},
        /* I25	   */	{16, {0, 99, 'e', 0x27, 0x0F, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x0D, 0x01, 0xF4, 0x0B, 0x01}},
        /* C93	   */	{16, {0, 99, 'i', 0x27, 0x0F, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x19, 0x01, 0xF4, 0x0B, 0x01}},
        /* GS1 Databar  */{16, {0, 99, 'y',    0,   16, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x2A, 0x01, 0xF4, 0x0B, 0x01}},
        /* GS1 Limited	*/{16, {0, 99, '{', 0x27, 0x0F, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x2A, 0x01, 0xF4, 0x0B, 0x01}},
        /* GS1 Expanded	*/{16, {0, 99, '}', 0x27, 0x0F, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x2B, 0x01, 0xF4, 0x0B, 0x01}},
        /* PDF         	*/{16, {0, 99, 'r', 0x27, 0x0F, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x2E, 0x01, 0xF4, 0x0B, 0x01}},
        /* Maxicode    	*/{16, {0, 99, 'x', 0x27, 0x0F, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x2F, 0x01, 0xF4, 0x0B, 0x01}},
        /* Data Matrix 	*/{16, {0, 99, 'w', 0x27, 0x0F, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x32, 0x01, 0xF4, 0x0B, 0x01}},
        /* QR Code    	*/{16, {0, 99, 's', 0x27, 0x0F, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x33, 0x01, 0xF4, 0x0B, 0x01}},
        /* Aztec    	*/{16, {0, 99, 'z', 0x27, 0x0F, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x34, 0x01, 0xF4, 0x0B, 0x01}},
        /* Code 49    	*/{16, {0, 99, 'l', 0x27, 0x0F, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0x35, 0x01, 0xF4, 0x0B, 0x01}},
        /* All others	*/{16, {0, 99, 0x99,  0x27, 0x0F, 0xF1, 0x00, 0xF4, 0x00, 0x01, 0xF4, 0xFF, 0x01, 0xF4, 0x0B, 0x01}},
        /* end	  */  {0,	{0,0,0}},
    };

// a helper to translate the old style format data into the new for ReaderConfiguration
static int CreateFormatCommandString(const FormatString_t *OldFormat, USHORT TermId, char *CommandString, size_t BufferLenght)
{
	int i;
	int CurrentSize;

	CurrentSize = _snprintf(CommandString, BufferLenght, "%01u%03u%02X%04u",
	                        (unsigned int) OldFormat->Data[0],
	                        (unsigned int) TermId,
	                        //	 										(unsigned int) OldFormat->Data[1],                           /*uncomment to test in 232 mode*/
	                        (unsigned int) OldFormat->Data[2],
	                        ((unsigned int) OldFormat->Data[3]) * 256 + ((unsigned int) OldFormat->Data[4]));

	if (CurrentSize >= 0)
	{
		for(i = 5;    i < OldFormat->Length;    ++i)
		{
			CurrentSize += _snprintf(CommandString+CurrentSize, BufferLenght-CurrentSize, "%02X", ((unsigned int) OldFormat->Data[i]));
		}
	}
	return CurrentSize;
}

// a helper to create the menu string
static int CreateRetailTermIDString(int TermId, int Protocol, int TrigMod, int ReReadDelay, char *CommandString, size_t BufferLenght)
{
	return _snprintf(CommandString, BufferLenght, "060500%d;060F00%d;0E0100%d;080B06%d", TermId, Protocol, TrigMod, ReReadDelay);
}

static int CreateGoodReadDelayString(int GoodReadDelay, char *CommandString, size_t BufferLenght)
{
	return _snprintf(CommandString, BufferLenght, "080B08%d", GoodReadDelay);
}

// Common function suits all 4683 and SurePos interfaces
bool PlugAndPlayIBM_4683_SurePos( int Protocol, int TrigMod, int TermId )
{
	if(ValidateTermId( TermId )==FALSE)
		return(FALSE);
	const FormatString_t *pFormat;
	const size_t BufferSize = 80;
	char CommandString[BufferSize]="";

	HSTRING MenuString0("080304", true);
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		MenuString0.Append( "@" );
	}
	else
	{
		MenuString0.Append( "." );
	}

	HSTRING MenuString("080300", true);


	ReaderConfiguration->Menu(&MenuString0);  /*clear all DFM's now */


	// load the format commands
	pFormat = IBM_4683_Formats;
	while(pFormat->Length > 0)
	{
		CreateFormatCommandString(pFormat, TermId, CommandString, BufferSize);
		MenuString.Append(CommandString);
		pFormat++;
		MenuString.Append("|");
	}

	switch(Protocol)
	{
	case IBM_4683_PORT_9B_HHBCR2:
	case IBM_4683_SurePos:
		{
			CreateFormatCommandString(&IBM_4683_128_2, TermId, CommandString, BufferSize);
			MenuString.Append(CommandString);
			MenuString.Append("|");
			CreateFormatCommandString(&IBM_4683_UCC128_2, TermId, CommandString, BufferSize);
			break;
		}
	case IBM_4683_PORT_5B:
	case IBM_4683_PORT_17:
	case IBM_4683_PORT_9B_HHBCR1:
		{
			CreateFormatCommandString(&IBM_4683_128_1, TermId, CommandString, BufferSize);
			MenuString.Append(CommandString);
			MenuString.Append("|");
			CreateFormatCommandString(&IBM_4683_UCC128_1, TermId, CommandString, BufferSize);
			break;
		}
	case IBM_4683_PORT_23_ADDR68:
	case IBM_4683_PORT_23_ADDR69:
	case IBM_4683_PORT_24:
	case IBM_4683_PORT_25_ADDR64:
	case IBM_4683_PORT_25_ADDR65:
	case IBM_4683_PORT_26:
	default:
		break;
	}

	MenuString.Append(CommandString);
	MenuString.Append(";");

	MenuString.Append(PrefixNoneSuffixNone); //clear Pre- and suffixes
	MenuString.Append(";");

	/*	Set up UPC and EAN 	*/
	MenuString.Append(RetailUPC);
	MenuString.Append(";");
	MenuString.Append(RetailEAN);
	MenuString.Append(";");
	MenuString.Append(AddendaSeparator);
	MenuString.Append(";");

	/*	Set TERMID	*/
	/*	Set AutoTrigger */
	/* Set Reread Delay to Medium */
	CreateRetailTermIDString(TermId, Protocol, TrigMod, 450, CommandString, BufferSize);
	MenuString.Append(CommandString);
	MenuString.Append(";");

	/*Set Good Read Delay to Short so the cash register keeps up with us.*/
	CreateGoodReadDelayString(500, CommandString, BufferSize);
	MenuString.Append(CommandString);


	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		MenuString.Append( "@" );
	}
	else
	{
		MenuString.Append( "." );
	}

	//	theStdInterface.Write(&MenuString);		// debug
	//	return PASS;
	return(ReaderConfiguration->Menu(&MenuString)==MENU_PASSED);
}

/*------------------------------------------------------------------------------
PlugAndPlayIBM_4683_Common()
------------------------------------------------------------------------------*/
static bool PlugAndPlayIBM_4683_Common( int Protocol)
{
#if defined (RFSCAN) || defined (RFBASE)
#else

	if(GET_CONFIG_SETTING(iOpticsPopulation)==TWO_D)
	{
		return PlugAndPlayIBM_4683_SurePos(Protocol, TRGMOD_MANUAL, TRM_IBM_4683);
	}
	else
	{
		return PlugAndPlayIBM_4683_SurePos(Protocol, TRGMOD_AUTO, TRM_IBM_4683);
	}
#endif
}

/*------------------------------------------------------------------------------
PlugAndPlayIBM_4683Port5B()
------------------------------------------------------------------------------*/
bool PlugAndPlayIBM_4683Port5B()
{
	return PlugAndPlayIBM_4683_Common(IBM_4683_PORT_5B);
}


/*------------------------------------------------------------------------------
PlugAndPlayIBM_4683Port9B_HHBCR1()
------------------------------------------------------------------------------*/
bool PlugAndPlayIBM_4683Port9B_HHBCR1()
{
	return PlugAndPlayIBM_4683_Common(IBM_4683_PORT_9B_HHBCR1);
}


/*------------------------------------------------------------------------------
PlugAndPlayIBM_4683Port9B_HHBCR2()
------------------------------------------------------------------------------*/
bool PlugAndPlayIBM_4683Port9B_HHBCR2()
{
	return PlugAndPlayIBM_4683_Common(IBM_4683_PORT_9B_HHBCR2);
}


/*------------------------------------------------------------------------------
PlugAndPlayIBM_4683Port17()
------------------------------------------------------------------------------*/
bool PlugAndPlayIBM_4683Port17()
{
	return PlugAndPlayIBM_4683_Common(IBM_4683_PORT_17);
}


/*------------------------------------------------------------------------------
PlugAndPlayIBM_4683Port23Addr68()
------------------------------------------------------------------------------*/
bool PlugAndPlayIBM_4683Port23Addr68()
{
	return PlugAndPlayIBM_4683_Common(IBM_4683_PORT_23_ADDR68);
}


/*------------------------------------------------------------------------------
PlugAndPlayIBM_4683Port23Addr69()
------------------------------------------------------------------------------*/
bool PlugAndPlayIBM_4683Port23Addr69()
{
	return PlugAndPlayIBM_4683_Common(IBM_4683_PORT_23_ADDR69);
}


/*------------------------------------------------------------------------------
PlugAndPlayIBM_4683Port24()
------------------------------------------------------------------------------*/
bool PlugAndPlayIBM_4683Port24()
{
	return PlugAndPlayIBM_4683_Common(IBM_4683_PORT_24);
}


/*------------------------------------------------------------------------------
PlugAndPlayIBM_4683Port25Addr64()
------------------------------------------------------------------------------*/
bool PlugAndPlayIBM_4683Port25Addr64()
{
	return PlugAndPlayIBM_4683_Common(IBM_4683_PORT_25_ADDR64);
}


/*------------------------------------------------------------------------------
PlugAndPlayIBM_4683Port25Addr65()
------------------------------------------------------------------------------*/
bool PlugAndPlayIBM_4683Port25Addr65()
{
	return PlugAndPlayIBM_4683_Common(IBM_4683_PORT_25_ADDR65);
}


/*------------------------------------------------------------------------------
PlugAndPlayIBM_4683Port26()
------------------------------------------------------------------------------*/
bool PlugAndPlayIBM_4683Port26()
{
	return PlugAndPlayIBM_4683_Common(IBM_4683_PORT_26);
}

/*------------------------------------------------------------------------------
PlugAndPlayIBM_USB_SURE_POS_HandHeld()
------------------------------------------------------------------------------*/
bool PlugAndPlayIBM_USB_SURE_POS_HandHeld(void)
{
	return PlugAndPlayIBM_4683_SurePos(IBM_4683_SurePos, TRGMOD_MANUAL, TRM_IBM_SUREPOS_HH);
}
/*------------------------------------------------------------------------------
PlugAndPlayIBM_USB_SURE_POS_TableTop()
------------------------------------------------------------------------------*/
bool PlugAndPlayIBM_USB_SURE_POS_TableTop(void)
{
	return PlugAndPlayIBM_4683_SurePos(IBM_4683_SurePos, TRGMOD_MANUAL, TRM_IBM_SUREPOS_TT);
}


// Menu string for Addenda settings in with retail interfaces
static const char HidPosString[] = ";060500131";

bool PlugAndPlayUsbPos(void)
{
	if(ValidateTermId( 131 )==FALSE)
		return(FALSE);

	HSTRING	MenuString(PrefixNoneSuffixNone, true);	//clear Pre- and suffixes
	MenuString.Append(HidPosString);

	if( ReaderConfiguration->CurrentTable == '@' )
	{
		MenuString.Append( "@" );
	}
	else
	{
		MenuString.Append( "." );
	}

	return( ReaderConfiguration->Menu(&MenuString)==MENU_PASSED);
}

/*------------------------------------------------------------------------------
SetPrefixCodeID()
------------------------------------------------------------------------------*/
bool SetPrefixCodeID(void)
{
	return SetPrefixSubstitute(SUB_WA_ID); /* Code ID */
}

/*------------------------------------------------------------------------------
SetPrefixCodeID()
------------------------------------------------------------------------------*/
bool SetPrefixAimID(void)
{
	return SetPrefixSubstitute(SUB_AIM_ID); /* Aim ID */
}

/*------------------------------------------------------------------------------
SetPrefixSubstitute()
------------------------------------------------------------------------------*/
bool SetPrefixSubstitute(UCHAR Substitute)
{
	char pcTempString[4];
	HSTRING	Temp("080404;080400995C", true); //change Prefix for all symbologies
	sprintf(pcTempString,"%02X",Substitute);
	Temp.Append(pcTempString); //byte to be entered

	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}

	return(ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

/*------------------------------------------------------------------------------
SetSuffixCR()
------------------------------------------------------------------------------*/
bool SetSuffixCR(void)
{
	HSTRING	Temp("080507;080500990D", true); //change Suffix for all symbologies to CR

	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}

	return(ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}


/*------------------------------------------------------------------------------
PlugAndPlay232()
------------------------------------------------------------------------------*/
bool PlugAndPlay232(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);

	HSTRING	Temp("",true);

	/*	Set Prefix and Suffix to canned values	*/
#ifndef VERIFIER

	SetPrefixNoneSuffixCRLF();
#endif
	/*	Set Serial Port Parameters */
#ifndef RFBASE
	// fixme everything 115200 n81  ?
	//if ((CHal::WhatBoardAmI() >= 0x20) && (CHal::WhatBoardAmI() < 0x30)) //40x0 goes to 115,200
	Temp.Append("0605000;080B000;0E01000;0607029;0607032;0607040;0607050;0607060");
	//
	//	else if( (CHal::WhatBoardAmI() == IT4800DR_1_TOKEN) )       //4800dr support
	//		Temp.Append("TERMID0;DLYCHR0;TRGMOD0;232BAD9,WRD2,CTS0,XON0,ACK0");
	//	else
	//		Temp.Append("TERMID0;DLYCHR0;TRGMOD0;232BAD7,WRD2,CTS0,XON0,ACK0");
#else

#endif

	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

/*------------------------------------------------------------------------------
PlugAndPlayKbdWedge()
------------------------------------------------------------------------------*/
bool PlugAndPlayKbdWedge(int termid, int country, int style)
{
	if(ValidateTermId( termid )==FALSE)
		return(FALSE);

	char pcTempString[84];
	/*	Set Prefix and Suffix to canned values	*/
	SetPrefixNoneSuffixCR();
	//change keyboard wedge settings
	sprintf(pcTempString,"060D030;060D070;060D040;060D050;060E00%d;060D01%d;0E0100%d;060500%d;080B000",country,style,TRGMOD_MANUAL,termid);
	HSTRING	Temp(pcTempString, true);

	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}

	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

// both USB keyboard flavors
bool PlugAndPlayUsbKbdPc(void)
{
	return PlugAndPlayKbdWedge(TRM_USB_KBD_PC, CTY_USA, STY_STANDARD);
}

bool PlugAndPlayUsbKbdMac(void)
{
	return PlugAndPlayKbdWedge(TRM_USB_KBD_MAC, CTY_USA, STY_STANDARD);
}

/*------------------------------------------------------------------------------
PlugAndPlayIBMAT()
------------------------------------------------------------------------------*/
bool PlugAndPlayIBMAT(void)
{
	return PlugAndPlayKbdWedge(TRM_IBM_PC_AT, CTY_USA, STY_STANDARD);
}


/*------------------------------------------------------------------------------
PlugAndPlayIBMPS2()
------------------------------------------------------------------------------*/
bool PlugAndPlayIBMPS2(void)
{
	return PlugAndPlayKbdWedge(TRM_IBM_PS2, CTY_USA, STY_STANDARD);
}


/*------------------------------------------------------------------------------
PlugAndPlayEriaIBMPS2()
------------------------------------------------------------------------------*/
bool PlugAndPlayEriaIBMPS2(void)
{
	return PlugAndPlayKbdWedge(TRM_IBM_PS2, CTY_FRANCE, STY_STANDARD);
}


/*------------------------------------------------------------------------------
PlugAndPlayLapTopDirect()
------------------------------------------------------------------------------*/
bool PlugAndPlayLapTopDirect(void)
{
	return PlugAndPlayKbdWedge(TRM_IBM_PC_AT, CTY_USA, STY_EMULATE_EXT);
}

/*------------------------------------------------------------------------------
PlugAndPlayOPOS()
------------------------------------------------------------------------------*/
bool PlugAndPlayOPOS(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);

	/* Set Prefix and Suffix to canned values */
	SetPrefixNoneSuffixCRLF();
	/* Set Serial Port Parameters 38400, N81, RTS */
	HSTRING  Temp("0605000;080B000;0E01000;0607027;0607032;0607041;0607050;0607060;", true);
	/* Set Prefix Code ID and AIM ID*/
	Temp.Append("080404;080400995C805C81;");
	/* Set Suffix CR */
	Temp.Append("080507;080500990D;");
	/* Enable UPCA, check digit ON and number system ON */
	Temp.Append("0211011;0211021;0211031;");
	/* Enable UPCE0, check digit ON */
	Temp.Append("0212011;0212041;");
	/* Enable EAN8, check digit ON */
	Temp.Append("0214011;0214021;");
	/* Enable EAN13, check digit ON */
	Temp.Append("0213011;0213021;");
	/* Enable OPOS */
	Temp.Append("080C001;");
	/* Auto Disable OFF */
	Temp.Append("080C010");

	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}

	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}


bool	bFactoryTestMode;	// Inital Value set in db_main
/*------------------------------------------------------------------------------
PlugAndPlayFactoryTest()
------------------------------------------------------------------------------*/
bool PlugAndPlayFactoryTest(void)
{
	bFactoryTestMode = TRUE;	// This will get cleared on reset (which is how we exit Factory test mode).
	USHORT TermID = TRM_ASYNC_ASCII;
	switch( GET_SETTING(FactoryTestMode) )
	{
	case 2:
		TermID = TRM_USB_COMEMU;
		break;
	default:
		break;
	}

	/* NGOEM-199: Add back the Removed TERMID0 for NGOEM USB (-213/-215) in factory mode */
	if(HardwareToken.IAmOEM() && (RS232_ONLY != GET_CONFIG_SETTING(iInterfacePopulation))){
		if(GET_SETTING(FactoryTestMode) && !GET_SETTING(RS232Unlock)){
			int piTempTerms[400];
			int iNumberOfTempTerms;

			iNumberOfTempTerms=CollectAllInterfaceTerminalIds(sizeof piTempTerms/sizeof(int), piTempTerms);	//query the interface table	
			ReaderConfiguration->ChangeNumericList("060500", piTempTerms, iNumberOfTempTerms, 3);	//update the menu

			/* We should temorarily set the 232ULK1 so that we can always come up with 232 when FACTON1 */
			RAM_SET_ACTUAL( RS232Unlock, 1);
		}
	}

	if(HardwareToken.IAmVoyager1400())//open imaging function in factory mode
	{
		if(GET_SETTING(FactoryTestMode)){
			if (GET_CONFIG_SETTING(HasStandSW))
				ReaderConfiguration->AddBoolean("0E0116", SUB_TAG_TABLE::FULLY_VISIBLE | WORKGROUP , NULL, NULL, &(ReaderConfiguration->ScanStandSwitchEn), true, true);
			ReaderConfiguration->ChangeTypeFlags("040806", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, true);
			ReaderConfiguration->ChangeTypeFlags("040807", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, true);
			ReaderConfiguration->ChangeTypeFlags("040809", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG | LOCAL_SETTING | SUB_TAG_TABLE::RETAIN_EXTRANEOUS_TEXT_AFTER_COMMAND, true);

			ReaderConfiguration->ChangeTypeFlags("04081E", SUB_TAG_TABLE::FULLY_VISIBLE, true);
			ReaderConfiguration->ChangeTypeFlags("04081F", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("040820", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("040821", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("040822", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("040823", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("040824", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("040825", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("040826", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("040827", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("04082A", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("04082B", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("04082D", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("04082E", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("04082F", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("040830", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("040837", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("040838", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("040839", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("04083B", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("04083D", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);

			ReaderConfiguration->ChangeTypeFlags("040B03", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("040B04", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("040B05", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("040B06", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("040B07", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("040B08", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("040B09", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("040B0A", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
			ReaderConfiguration->ChangeTypeFlags("040B0B", SUB_TAG_TABLE::FULLY_VISIBLE  | WORKGROUP , true);
		}
	}
	
	/* Set Prefix and Suffix to none */
	//SetPrefixNoneSuffixNone();
	HSTRING	Temp("080404;080507!", true); //clear Prefixes, put in suffix
	ReaderConfiguration->Menu(&Temp);

	/*	Manual Trigger, No Scan Stand Switch, Disable Data Format, Beep High Volume, Beeper Pitch Medium(2700HZ)  */
	RAM_SET_ACTUAL( TriggerMode, TRGMOD_MANUAL);
	RAM_SET_ACTUAL( ScanStandSwitchEn, false);
	RAM_SET_ACTUAL( DataFormatEnable, DFM_DISABLE);
	RAM_SET_ACTUAL( BeepVolume, BEEP_HI);
	RAM_SET_ACTUAL( BASE_SCANNER_SELECT_GoodReadFrequency, 2700);

	/* Disable Codegate, set Scandriver timeout = 1 */
	RAM_SET_ACTUAL( bAimerOutOfStandCodeGate, false);
	RAM_SET_ACTUAL( ScanDriverTimeout, 1);

	/*	Setup Code 39	*/
	RAM_SET_ACTUAL( C39Enable, 1);
	RAM_SET_ACTUAL( C39SSXmit, 0);
	RAM_SET_ACTUAL( C39ChkChar, CHK_NONE);
	RAM_SET_ACTUAL( C39MinLength, (UINT) 1);
	RAM_SET_ACTUAL( C39MaxLength, (UINT) 48);

	/*	Setup UPC-A	*/
	RAM_SET_ACTUAL( UPCAEnable, 1);
	RAM_SET_ACTUAL( UPCAChkXmit, 1);
	RAM_SET_ACTUAL( UPCANumXmit, 1);

	/* Setup Maxicode */
	RAM_SET_ACTUAL( maxEnable, 1);
	RAM_SET_ACTUAL( maxMinLength, 1);
	RAM_SET_ACTUAL( maxMaxLength, 150);

	/* Setup PDF */
	RAM_SET_ACTUAL( PDFEnable, 1);
	RAM_SET_ACTUAL( PDFMinLength, 1);
	RAM_SET_ACTUAL( PDFMaxLength, 2750);

	/* Setup Data Matrix*/
	RAM_SET_ACTUAL( idmEnable, 1);
	RAM_SET_ACTUAL( idmMinLength, 1);
	RAM_SET_ACTUAL( idmMaxLength, 3116);

	/* Setup Aztec */
	RAM_SET_ACTUAL( aztEnable, 1);
	RAM_SET_ACTUAL( aztMinLength, 1);
	RAM_SET_ACTUAL( aztMaxLength, 3832);

#if defined( RFSCAN )

#else
	//	if( CHal::WhatBoardAmI() == IT3800G_1_TOKEN )
	//		RAM_SET_ACTUAL( RS232BaudRate, BAUD_460800);		Removed as requested by Mfg. 1/30/02 (dmh)
	//	else
	RAM_SET_ACTUAL( RS232BaudRate, BAUD_115200);
	RAM_SET_ACTUAL( RS232WordFormat, SER_WORD_N81);
	RAM_SET_ACTUAL( RS232CtsEnable, 0);
	RAM_SET_ACTUAL( RS232XonEnable, 0);
	RAM_SET_ACTUAL( RS232AckEnable, 0);
	RAM_SET_ACTUAL( RS232CtsDelay, 0);

	RAM_SET_ACTUAL(RemIfaceEnable, false);  // Just in case we're making some special that has REMIFC enabled, make sure it stays off so apps don't see a COM port number change caused by the changing USB PID
#endif
	//fixme oem special stuff
	if (HardwareToken.IAmOEM())
	{
		RAM_SET_ACTUAL( RS232InvEnable, 0);
		RAM_SET_ACTUAL( BeeperInvert, 0);
		RAM_SET_ACTUAL( GoodReadLedInvert, 0);
		}
	RAM_SET_ACTUAL( IntCharDelay, 0);
	RAM_SET_ACTUAL( IntFuncDelay, 0);
	RAM_SET_ACTUAL( IntMsgDelay, 0);
	RAM_SET_ACTUAL( IntCharXDelay, 0);

	/*	Ask interface for term id (all but USB uses TRM_ASYNC_ASCII. TermID is not touched if the driver does not know this ifIoControl */
	//	ifIoControl((IoControlLParam_t)&TermID, 0, IOCTRL_GET_FACTORY_INTERFACE, g_pvInterface);

	/*	Set TERMID	*/
	RAM_SET_ACTUAL( TerminalID, TermID);

	/* If 4683 is OPENED, this is the only way we can close it! */
	if (g_pStdInterface)
		theStdInterface.DeviceIoControl(IOCTRL_PREPARE_FORCED_CLOSE);


	/*	Disable Buffered Message Mode */
	RAM_SET_ACTUAL( BufferedOutput, 0);

	/*	Disable Decode Header */
	RAM_SET_ACTUAL( DecodeHeader, false);

	/*	Disable allignment mode image block in 4800dr */
//	RAM_SET_ACTUAL( BlockImagingEnabledWhenNotAlligned, 0 );

	/* Make sure scanner is Enabled */

	RAM_SET_ACTUAL( bOposMenuEnable, 0);
	RAM_SET_ACTUAL( bOposAutoDisable, 0);

	/*	Factory Test settings are temporary, don't save them to ROM	*/
#ifdef ENCRYPT_FIPS

	RAM_SET_ACTUAL(EnableFIPS, 0);
	RAM_SET_ACTUAL(FIPSIndicator, 0);
#endif

	/* Disable Plugin Loading */
#ifdef PLUGIN_DEV
	RAM_SET_ACTUAL(EnableFormatPlugins, 0);
	RAM_SET_ACTUAL(EnableDecodePlugins, 0);
	RAM_SET_ACTUAL(EnableImagePlugins, 0);
#endif

	return PASS;
}


/*------------------------------------------------------------------------------
PlugAndPlayGetBufferedMessage()
------------------------------------------------------------------------------*/
bool PlugAndPlayGetBufferedMessage(void)
{
	return PASS ;
}

/*------------------------------------------------------------------------------
 * Plug and Play Visual Menu Settings
 *------------------------------------------------------------------------------*/
bool PlugAndPlayVisualMenu(void)
{
	if((PresentInterface==Interface_485_Group)||(PresentInterface==Interface_KBDWDG_Group))
	{
		SoundErrorBeep();
		return FAIL;
	}
	else if(PresentInterface==Interface_USB_Group)
	{
		VisualMenu(1);
		return PASS;
	}
	else
	{
		VisualMenu(0);
		return PASS;
	}
}
bool PlugAndPlayVisualMenu2(void)
{
	if(PresentInterface==Interface_USB_Group || PresentInterface == Interface_Unknow )
	{
		VisualMenu(1);
		return PASS;
	}
	else  
	{
		SoundErrorBeep();
		return FAIL;
	}
}
bool PlugAndPlayVisualMenu3(void)
{
	if(PresentInterface==Interface_USB_Group || PresentInterface == Interface_Unknow )
	{
		VisualMenu(2);
		return PASS;
	}
	else   
	{
		SoundErrorBeep();
		return FAIL;
	}
}

bool VisualMenu(int VisMnuType)
{
	/*	Set Prefix and Suffix to none	*/
	HSTRING	Temp("080404;080507;", true); //clear Prefixes, put in suffix

	switch (VisMnuType)
	{
	case 0:
		if (0) // fixme CHal::AmIOem())
			VisMnuPtr=VismnuDefaults2;
		else
			VisMnuPtr=VismnuDefaults;
		break;
	case 1:
		VisMnuPtr = VismnuDefaults3;
		break;
	case 2:
		VisMnuPtr = VismnuDefaults4;
		break;
	default:
		return FAIL;
	}

	/*	Set Serial Port Parameters 38400,N,8,1 and options disabled */
	Temp.Append(VisMnuPtr);
	ReaderConfiguration->Menu(&Temp);

	/* If 4683 is OPENED, this is the only way we can close it! */

	theStdInterface.DeviceIoControl(IOCTRL_PREPARE_FORCED_CLOSE);
	ReSynchWorkingAndActive(TRUE); //Go do setup, including forcing manual trigger and IF open.
	bVisMnuOverride = TRUE;
	/*	Visual Menu settings are temporary, don't save them to ROM	*/
	return PASS;
}

void SendStringToInterface( const char *pucBuffer, int SizeOfBuffer, bool /* DoNotUseIndicators */ )
{
	RESET_WATCHDOG_TIMER();

	if(IsInterfaceRunning())
		theStdInterface.Write( pucBuffer, SizeOfBuffer );
}

void SendStringToInterface( const char *pucBuffer, int SizeOfBuffer)
{
	SendStringToInterface( pucBuffer, SizeOfBuffer, false );
	RESET_WATCHDOG_TIMER();
}

void SendStringToInterface(const char *pucBuffer)
{
	SendStringToInterface( pucBuffer, strlen(pucBuffer ) );
}

extern int gScanDriverCellPhone;

bool PlugAndPlaySPNormal(void)
{
	int menuPassed = MENU_PASSED;                //indicate passed
	int menuTwoPassed = MENU_PASSED;                //indicate passed
	HSTRING	Temp("",true);
	HSTRING	TempTwo("",true);

	if(HardwareToken.IAmOEM())
	{
		TempTwo.Append("0410000;0403050.");                //turn off cell before changing trig mode
		menuTwoPassed = ReaderConfiguration->Menu(&TempTwo);

		if( menuTwoPassed==MENU_PASSED )
		{

#if !defined( RFBASE ) && !defined( VERIFIER )
			gScanDriverCellPhone = 0;	//make sure cell is off in scan driver before we do a set lights
			SetLights(0,-1,-1,-1);	//since cell controls lights internal to scan driver and may be off, toggle set lights so next light mode written takes effect
#endif
		}

		Temp.Append("0E01008;0402110;0403062;0403074;0403031;024E020;024E0D150;024E0E200.");
	}
	else
	{
		Temp.Append("0E01008;0402110;0403050;0403062;0403074;0403031;024E020;024E0D150;024E0E200.");
	}

	menuPassed = ReaderConfiguration->Menu(&Temp);

	return( (menuPassed==MENU_PASSED) && (menuTwoPassed==MENU_PASSED) );
}

bool PlugAndPlaySPEnhanced(void)
{
	int menuPassed = MENU_PASSED;                //indicate passed
	int menuTwoPassed = MENU_PASSED;                //indicate passed
	HSTRING	Temp("",true);
	HSTRING	TempTwo("",true);

	if(HardwareToken.IAmOEM())
	{
		TempTwo.Append("0410000;0403050.");                 //turn off cell before changing trig mode
		menuTwoPassed = ReaderConfiguration->Menu(&TempTwo);

		if( menuTwoPassed==MENU_PASSED )
		{
#if !defined( RFBASE ) && !defined( VERIFIER )
			gScanDriverCellPhone = 0;	//make sure cell is off in scan driver before we do a set lights
			SetLights(0,-1,-1,-1);	//since cell controls lights internal to scan driver and may be off, toggle set lights so next light mode written takes effect
#endif
		}

		Temp.Append("0E01008;0402110;0403061;0403074;0403031;024E020;024E0D150;024E0E200.");
	}
	else
	{
		Temp.Append("0E01008;0402110;0403050;0403061;0403074;0403031;024E020;024E0D150;024E0E200.");
	}

	menuPassed = ReaderConfiguration->Menu(&Temp);

	return( (menuPassed==MENU_PASSED) && (menuTwoPassed==MENU_PASSED) );
}

bool PlugAndPlaySPCell(void)
{
	HSTRING	Temp("",true);

	if(HardwareToken.IAmOEM())
	{
#if !defined( RFBASE ) && !defined( VERIFIER )
			SetLights(0,-1,-1,-1);	//since cell controls lights internal to scan driver and may be off, toggle set lights so next light mode written takes effect
#endif
		Temp.Append("0E01008;0402115;0410000;0403051;0403062;0403074;0403031;024E020;024E0D300;024E0E500.");
	}
	else
	{
		Temp.Append("0E01008;0402115;0403051;0403063;0403031;024E020;024E0D300;024E0E500.");
	}

	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayHHSnappy(void)
{
	HSTRING	Temp("0E01000;0402100;0410000;0403050;0408401;024D0D1;024D21100;024D22300.", true);
	//HSTRING	Temp("TRGMOD0;EXPMOD4;SDRCEL0;PDCCEL0;EXPTAR120;EXPEST4;EXPEMX4;EXPGST4;EXPGMX4;IMGIQM1;DECAUX1;DECTMN100;DECTMX300.", true);
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayHHFull(void)
{
#if defined(IR_TRIGGER) || defined(SM4100)
	HSTRING	Temp("0E01000;0402104;0410000;0403050;04020790;0408401;024D0D1;024D21400;024D22800;0616000.", true);
#else
	HSTRING	Temp("0E01000;0402104;0410000;0403050;04020790;0408401;024D0D1;024D21400;024D22800.", true);
#endif
	//HSTRING	Temp("TRGMOD0;EXPMOD4;SDRCEL0;PDCCEL0;EXPTAR120;EXPEST4;EXPEMX100;EXPGST4;EXPGMX4;IMGIQM1;DECAUX1;DECTMN400;DECTMX800.", true);
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayHi2dPresentaion(void)
{
#if defined(IR_TRIGGER) || defined(SM4100)
	HSTRING	Temp("0E01003;0402104;0410000;0403050;0408401;024D0D1;024D21300;024D22500;0616000.", true);
#else
	HSTRING	Temp("0E01003;0402104;0410000;0403050;0408401;024D0D1;024D21300;024D22500.", true);
#endif
	//HSTRING	Temp("TRGMOD3;EXPMOD5;SDRCEL0;PDCCEL0;EXPFEX4;IMGIQM1;DECAUX1;DECTMN300;DECTMX500.", true);
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayHHCell(void)
{
	HSTRING	Temp("0E01000;0402106;0410001;0403050;0408401;024D0D1;024D21300;024D22500.", true);
	//HSTRING	Temp("TRGMOD0;EXPMOD5;SDRCEL1;PDCCEL0;EXPFEX4;IMGIQM1;DECAUX1;DECTMN300;DECTMX500.", true);
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}


bool PlugAndPlayHi2dPresentaionCell(void)
{
	HSTRING	Temp("0E01003;0402106;0410000;0403051;0408401;024D0D1;024D21300;024D22500.", true);
	//HSTRING	Temp("TRGMOD3;EXPMOD5;SDRCEL1;PDCCEL1;EXPFEX4;IMGIQM1;DECAUX1;DECTMN300;DECTMX500.", true);
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}


/*------------------------------------------------------------------------------
SendDecoderAllRevisions()
------------------------------------------------------------------------------*/
#ifndef RFBASE
#include "DecoderInterface.h"
#include "DecoderLibRevision.h"
#endif

bool SendDecoderAllRevisions(void)
{
#if defined(RFBASE)
#else
	char *pucBuffer;
	pucBuffer = (char *)malloc(300*4);
	if ( NULL == pucBuffer)
		return FAIL;
	*pucBuffer = '\0';

	int stringOffset = 0;
	sprintf(pucBuffer,"\r\nDecoder Library Version: %s",&DecoderLibVersion[18]);
	stringOffset = strlen(pucBuffer);
	pucBuffer[stringOffset-2] = ' ';
	pucBuffer[stringOffset-1] = '\r';
	pucBuffer[stringOffset] = '\n';
	pucBuffer[stringOffset+1] = '\0';

	int revType = DEC_REVISION_CONTROL_LOGIC;
#if (DECODER_ID) 
	revType += DEC_REVISION_ID;
#endif
	
#if (DECODER_AD)
	revType += DEC_REVISION_AD;
#endif

#if (DECODER_FLD)
	revType += DEC_REVISION_FLD;
#endif

#if (DECODER_ULD)
	revType += DEC_REVISION_ULD;
#endif

	ReportDecoderVersion(revType, (char *)&pucBuffer[strlen(pucBuffer)]);
	ReaderConfiguration->MenuCommandResponse->Append((UCHAR *)" ");

	ReaderConfiguration->MenuCommandResponse->Append((UCHAR *)pucBuffer);

	ReaderConfiguration->OneOrMoreQueryCommandsWereProcessed = true;

	free (pucBuffer);
	return PASS;
#endif //endof defined(RFBASE)
}

/*------------------------------------------------------------------------------
SendDecoderRevision()
------------------------------------------------------------------------------*/
bool SendDecoderRevision(void)
{
#if defined(RFBASE)
#else
	char *pucBuffer;
//	RESET_WATCHDOG_TIMER();
	pucBuffer = (char *)malloc(300);
	if ( NULL == pucBuffer)
		return FAIL;
	*pucBuffer = '\0';

	int revType = 0;
#if (DECODER_ID)
	revType += DEC_REVISION_ID;
#endif
#if (DECODER_AD)
	revType += DEC_REVISION_AD;
#endif
	ReportDecoderVersion(revType, pucBuffer);
	ReaderConfiguration->MenuCommandResponse->Append((UCHAR *)" ");

	ReaderConfiguration->MenuCommandResponse->Append((UCHAR *)pucBuffer);

	ReaderConfiguration->OneOrMoreQueryCommandsWereProcessed = true;

	free (pucBuffer);
	return PASS;
#endif //endof defined(RFBASE)
}
/*------------------------------------------------------------------------------
SendScanDriverRevision()
------------------------------------------------------------------------------*/
bool SendScanDriverRevision(void)
{
#if defined(RFBASE)
#else
	char *pucBuffer;
//	RESET_WATCHDOG_TIMER();
	pucBuffer = (char *)malloc(80);
	if ( NULL == pucBuffer)
		return FAIL;
	*pucBuffer = '\0';

	if (SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_REVISION_STRING,NULL,0,(unsigned char *)pucBuffer,0,NULL))
	{
		ReaderConfiguration->MenuCommandResponse->Append((UCHAR *)pucBuffer);
		ReaderConfiguration->OneOrMoreQueryCommandsWereProcessed = true;
	}
	else
		return FAIL;

	free (pucBuffer);
	return PASS;
#endif //endof defined(RFBASE)
}

/*------------------------------------------------------------------------------
SendRevisionInfo()
------------------------------------------------------------------------------*/
#define RevBufferSize	600

bool SendRevisionInfo(void)
{
	char *pucBuffer;

//	RESET_WATCHDOG_TIMER();

	/*	Create a temporary buffer	*/
	pucBuffer = (char *) malloc(RevBufferSize);
	if( NULL == pucBuffer )
		return FAIL;
	*pucBuffer = '\0';
	strcpy(pucBuffer,"Product Name: ");
	strcpy(&pucBuffer[strlen(pucBuffer)],GET_SETTING(ShowProductName));
	/*	Send the Micro's Boot Code Revision	*/
	strcpy(&pucBuffer[strlen(pucBuffer)], "\r\nBoot Revision: ");


	if(BOOT_CODE_MAJOR_VERSION != 0xFF)
	{
		sprintf(&pucBuffer[strlen(pucBuffer)],"%02d.%02d ", (BYTE) BOOT_CODE_MAJOR_VERSION, (BYTE) BOOT_CODE_MINOR_VERSION);
	}
	else
	{
		sprintf(&pucBuffer[strlen(pucBuffer)],"%s", BOOT_CODE_VERSION_STRING);
	}

	/*	Send the Software Number	*/
	strcpy(&pucBuffer[strlen(pucBuffer)], "\r\nApp Revision");
	strcat(&pucBuffer[strlen(pucBuffer)],GET_SETTING(ShowWARev));

	/*	Send the Project Revision	*/
	strcpy(&pucBuffer[strlen(pucBuffer)], "\r\nProject Revision");
	strcat(&pucBuffer[strlen(pucBuffer)],GET_SETTING(ShowSwRev));

	/*	Send the Serial Number	*/
	strcpy(&pucBuffer[strlen(pucBuffer)], "\r\nSerial Number: ");
	HSTRING TempString(50,true);
	TempString.Copy(GET_CONFIG_SETTING(SerNum));
	TempString.UniCodeToSimpleAsciiFormat();
	strcat(&pucBuffer[strlen(pucBuffer)],(char *)TempString.Char);

	/*-----------------1/8/2003 3:34PM------------------
	 * Send the supported interface population.
	 * --------------------------------------------------*/
	strcpy(&pucBuffer[strlen(pucBuffer)], "\r\nSupported IF: ");
	//	#ifdef RFBASE
#ifndef RFSCAN

	//fixme UINT bdToken = CHal::WhatBoardAmI();
	int ifs= GET_CONFIG_SETTING(iInterfacePopulation);
	switch (ifs)
	{
	case PSEUDO_KBW_4683_USB :
		sprintf(&pucBuffer[strlen(pucBuffer)], "Standard");
		break;
	case RS232_ONLY :
		if (HardwareToken.IAmOEM())
		sprintf(&pucBuffer[strlen(pucBuffer)], "RS/232");
		else
			sprintf(&pucBuffer[strlen(pucBuffer)], "True232");
		break;
		//case IT4080_RS232_HHLC : same as below
	case HHLC_LP :
		if (0)//fixme bdToken >= IT4080_1_TOKEN  && bdToken < IT4080_LAST_TOKEN)
			sprintf(&pucBuffer[strlen(pucBuffer)], "RS232/HHLC");
		else
			sprintf(&pucBuffer[strlen(pucBuffer)], "HHLC");
		break;
	case RS232_USB_HHLC :
		sprintf(&pucBuffer[strlen(pucBuffer)], "RS232/USB/HHLC");
		break;
	case PSEUDO232_USB :
		sprintf(&pucBuffer[strlen(pucBuffer)], "RS232/USB");
		break;
	default :
		sprintf(&pucBuffer[strlen(pucBuffer)], "Not Detected %x",ifs);
	}
#elif defined(EMERALD_VERIFIER)
	sprintf(&pucBuffer[strlen(pucBuffer)], "RS232/USB/Bluetooth"); //Report bluetooth, 232, USB  for QC900
#else

	sprintf(&pucBuffer[strlen(pucBuffer)], "Bluetooth"); //Report bluetooth only for scanner
#endif

	/*	Send the 6 bit PCB Assembly ID	*/
	strcpy(&pucBuffer[strlen(pucBuffer)], "\r\nPCB Assembly ID: ");
	UCHAR ucGarbage=0; //fixmeCHal::WhatBoardAmI();
	HSTRING BSString(8,true);
	for(int i=0;i<6;i++,ucGarbage/=2)
		BSString.Insert(((ucGarbage%2)?"1":"0"),0);
	sprintf(&pucBuffer[strlen(pucBuffer)],"%s\r\n",(char *)BSString.Char);

	// Attach the engine information if this is not an RFBASE or a Verifier.
#if !defined( RFBASE ) && !defined( VERIFIER )
	if(HardwareToken.IAmOEM())
	{
		if( ImageEngineHasPsoc() )
		{
			PSoC_5100_REGISTER_TABLE	Registers;
			bool	Status = Psoc5100GetRegisters( &Registers );

		if( Status )
			{
				HSTRING		Temp( 20, true );
				Temp.Copy( Registers.SweetcornSerialNumber, sizeof( Registers.SweetcornSerialNumber ) );
				sprintf( &pucBuffer[strlen(pucBuffer)], "Engine Firmware Type: %d   Revision: %d   Serial Number: %s   Checksum: %X\r\n",
			         (int) Registers.FirmwareType,
		        	 (int) Registers.FirmwareRevision,
			         (char *) Temp.Char,
			         (int) Registers.Checksum );
			}
		else
			strcpy( &pucBuffer[strlen(pucBuffer)], "Engine Firmware Type: Unknown   Revision: Unknown   Serial Number: Unknown   Checksum: Unknown\r\n" );
		}
	}
	else
		strcpy( &pucBuffer[strlen(pucBuffer)], "Engine Firmware Type: N/A   Revision: N/A   Serial Number: N/A   Checksum: N/A\r\n" );
#endif

	SendStringToInterface(pucBuffer);
	//#ifdef RFSCAN
	//if (theCurrentClientSPP_Interface.IsPortAvailable())
	//		theCurrentClientSPP_Interface.printf("\x16M\rREVINF.");
	//#endif

	free(pucBuffer);
	return PASS;
}
bool SendSettingForAllCable()
{
	bool rtn=PASS;
	#ifndef RFSCAN
	char* pucBuffer1;
	char* pucBuffer2=NULL;

	//	RESET_WATCHDOG_TIMER();
	pucBuffer1 = (char *) malloc(FLASH_MAX_CONFIGURATION_LENGTH);
	if(NULL!=pucBuffer1)
	{
		pucBuffer2 = (char *) malloc(FLASH_MAX_CONFIGURATION_LENGTH);
		if(NULL!=pucBuffer2)
		{
				*pucBuffer1 = '\0';
				*pucBuffer2 = '\0';

			#ifdef RFBASE
				sprintf (pucBuffer1, "=at:ae;?.");
				sprintf (pucBuffer2, "=at:ae;?.");
			#else
				sprintf (pucBuffer1, "?.");
				sprintf (pucBuffer2, "?.");

			#endif

				unsigned int MenuSize1=strlen(pucBuffer1);
				unsigned int MenuSize2=strlen(pucBuffer2);
				ReaderConfiguration->Menu(&MenuSize1,(unsigned char*)pucBuffer1);
			if(ReaderConfiguration->CurrentWorkTable()){
				SaveSettingChangeToTableOne();
				LoadWorkingConfigurationZero();
				ReaderConfiguration->Menu(&MenuSize2,(unsigned char*)pucBuffer2);
				LoadWorkingConfigurationOne();
				}
			else
			{
				SaveSettingChangeToTableZero();
				LoadWorkingConfigurationOne();
				ReaderConfiguration->Menu(&MenuSize2,(unsigned char*)pucBuffer2);
				LoadWorkingConfigurationZero();
			}
			RESET_WATCHDOG_TIMER();
			if(GET_SETTING(RS232TabSelNum))
				SendStringToInterface(pucBuffer1);
			else
				SendStringToInterface(pucBuffer2);
			if(GET_SETTING(KBDTabSelNum))
				SendStringToInterface(pucBuffer1);
			else
				SendStringToInterface(pucBuffer2);
			if(GET_SETTING(UsbTabSelNum))
				SendStringToInterface(pucBuffer1);
			else
				SendStringToInterface(pucBuffer2);
			if(GET_SETTING(RS485TabSelNum))
				SendStringToInterface(pucBuffer1);
			else
			SendStringToInterface(pucBuffer2);
			free(pucBuffer2);
		}
		else
		{
			rtn=FAIL;
		}
		free(pucBuffer1);
	}
	else
	{
		rtn=FAIL;
	}

	#endif
	return rtn;
}
/*------------------------------------------------------------------------------
SendHardwareToken()
------------------------------------------------------------------------------*/
bool SendHardwareToken(void)
{
	char *pucBuffer;

//	RESET_WATCHDOG_TIMER();

	pucBuffer = (char *) malloc (20);
	if (NULL == pucBuffer)
		return FAIL;

//	sprintf (pucBuffer, "0x%02x", 0); //fixme CHal::WhatBoardAmI());
	sprintf (pucBuffer, "0x%02x", HardwareToken.GetHardwareToken());

	ReaderConfiguration->MenuCommandResponse->Append((UCHAR *)pucBuffer);

	free(pucBuffer);
	return PASS;
}

/*------------------------------------------------------------------------------
SendBootRevision()
------------------------------------------------------------------------------*/
bool SendBootRevision(void)
{
	char *pucBuffer;

//	RESET_WATCHDOG_TIMER();

	if(BOOT_CODE_MAJOR_VERSION != 0xFF)
	{
		/*	Create a temporary buffer	*/
		pucBuffer = (char *) malloc(80);
		if( NULL == pucBuffer )
			return FAIL;
		*pucBuffer = '\0';

		sprintf (pucBuffer, "\rBoot version %02d.%02d", (BYTE) BOOT_CODE_MAJOR_VERSION, (BYTE) BOOT_CODE_MINOR_VERSION);
	}
	else
	{
		// If the version string length is greater than 80 characters, WOOF! (it probably isn't properly initialized)
		if( strlen((char *) BOOT_CODE_VERSION_STRING) > 80 )
			return FAIL;

		/*	Create a temporary buffer	*/
		pucBuffer = (char *) malloc(strlen((char *) BOOT_CODE_VERSION_STRING) + 40);
		if( NULL == pucBuffer )
			return FAIL;
		*pucBuffer = '\0';

		sprintf (pucBuffer, "\rBoot %s", BOOT_CODE_VERSION_STRING);
	}
	ReaderConfiguration->MenuCommandResponse->Append((UCHAR *)pucBuffer);

	free(pucBuffer);
	return PASS;
}



/*------------------------------------------------------------------------------
SendPrefixBlockSize()
------------------------------------------------------------------------------*/
bool SendPrefixBlockSize(void)
{
	char cBuffer[8] = {'\0'};

//	RESET_WATCHDOG_TIMER();

	/*	If their is no interface open then we can't send	*/
	//	if( NULL == pInterface )
	//		return FAIL;;

	sprintf(cBuffer, "%u\r\n", PREFIX_BLOCKSIZE-PREFIX_SUFFIX_OVERHEAD);
	SendStringToInterface(cBuffer);

	return PASS;
}


/*------------------------------------------------------------------------------
SendSuffixBlockSize()
------------------------------------------------------------------------------*/
bool SendSuffixBlockSize(void)
{
	char cBuffer[8] = {'\0'};

//	RESET_WATCHDOG_TIMER();

	/*	If their is no interface open then we can't send	*/
	//	if( NULL == pInterface )
	//		return FAIL;;

	sprintf(cBuffer, "%u\r\n", SUFFIX_BLOCKSIZE-PREFIX_SUFFIX_OVERHEAD);
	SendStringToInterface(cBuffer);

	return PASS;
}


/*------------------------------------------------------------------------------
SendDataFormatBlockSize()
------------------------------------------------------------------------------*/
bool SendDataFormatBlockSize(void)
{
	char cBuffer[8] = {'\0'};

//	RESET_WATCHDOG_TIMER();

	/*	If their is no interface open then we can't send	*/
	//	if( NULL == pInterface )
	//		return FAIL;;

	sprintf(cBuffer, "%u\r\n", DATAFORMAT_BLOCKSIZE);
	SendStringToInterface(cBuffer);

	return PASS;
}


/*------------------------------------------------------------------------------
SetPrefixNoneSuffixCRLF() is a utility to set the prefix to none and suffix to CR LF
------------------------------------------------------------------------------*/
void SetPrefixNoneSuffixCRLF(void)
{
	HSTRING	Temp("080404;080507;080500990D0A", true); //clear Prefixes, put in suffix

	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}

	ReaderConfiguration->Menu(&Temp);
	return;
}


/*------------------------------------------------------------------------------
SetPrefixNoneSuffixNone() is a utility to set the prefix to none and suffix to CR LF
------------------------------------------------------------------------------*/
void SetPrefixNoneSuffixNone(void)
{
	HSTRING	Temp("080404;080507", true); //clear Prefixes, put in suffix

	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}

	ReaderConfiguration->Menu(&Temp);
	return;
}


/*------------------------------------------------------------------------------
SetPrefixNoneSuffixCR() is a utility to set the prefix to none and suffix to CR
------------------------------------------------------------------------------*/
void SetPrefixNoneSuffixCR(void)
{
	HSTRING	Temp("080404;080507;080500990D", true); //clear Prefixes, put in suffix

	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}

	ReaderConfiguration->Menu(&Temp);
	return;
}


bool PlugAndPlayRuby(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);

	  /* Set Prefix and Suffix to canned values */
//	SetPrefixNoneSuffixCRLF();
	  /* Set Serial Port Parameters 38400, N81, RTS */
	HSTRING	Temp("0211021;0211031;0608000;0607022;0607041;06070314;0212031;080500990A;", true);
	  /* Set Prefix Code ID and AIM ID*/
	Temp.Append("0803000099639999f44101f100;");
	Temp.Append("0803000099459999f44101f100;");
	Temp.Append("0803000099649999f44601f100;");
	Temp.Append("0803000099449999f44602f100;");
//	Temp.Append("DFMBK30099629999f44201f100;");
//	Temp.Append("DFMBK300996a9999f44401f100;");
//	Temp.Append("DFMBK30099499999f44b01f100;");
//	Temp.Append("DFMBK30099729999f45801f100;");

	if(HardwareToken.IAmVoyager1400())
	{
		Temp.Append("0E01003");
	}
	else
	{
		Temp.Append("0E01008");
	}

	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}

	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);

//	return true;
}

bool PlugAndPlayGilbarco(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);

	  /* Set Prefix and Suffix to canned values */
//	SetPrefixNoneSuffixCRLF();
	  /* Set Serial Port Parameters 38400, N81, RTS */
	HSTRING	Temp("0211021;0211031;0212041;0608000;0607023;0607041;0607034;0203010;0204010;0202010;020A010;020D010;080500990D;", true);
	  /* Set Prefix Code ID and AIM ID*/
	Temp.Append("0803000099639999f44101f100;");
	Temp.Append("0803000099459999f44501f43001f100;");
	Temp.Append("0803000099649999f44601f100;");
	Temp.Append("0803000099449999f44602f100");

	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}

	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);

//	return true;
}

// this plug&play is not completed now, need to add in the future
bool PlugAndPlayBioptiAuxPort(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);

	  /* Set Prefix and Suffix to canned values */
//	SetPrefixNoneSuffixCRLF();

	  /* Set Serial Port Parameters 38400, N81, RTS */
	HSTRING	Temp("060500000;0607027;0607032;", true);
	Temp.Append("0607044;0607092;0607083100;06070A1;06070B5100");

	/* Turn off beeper */
	Temp.Append(";0502100");

	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}

	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);

//	return true;
}

bool PlugAndPlayMagellan(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);

	HSTRING	Temp("",true);
	Temp.Append("0605000;080B000;0E01000;0607029;0607032;0607040;0607050;0607060;");	//PAP232 Settings
	Temp.Append("0607025;0607032;0607041;");
	Temp.Append("0203051;0202021;080507;");
	Temp.Append("080400795234;0804006341;0804004545;080400444646;0804006446;080400622A;0804006569;0804006125;0804006A23;0804006740;0804006926;0804007250");

	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}

	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);

//	return true;
}

bool PlugAndPlayNCR(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);

	  /* Set Prefix and Suffix to canned values */
	SetPrefixNoneSuffixCRLF();
	  /* Set Serial Port Parameters 9600 baud , E71, CTS */
	HSTRING	Temp("0607025;0607033;0607043;", true);
	  /* Set Prefix Code ID and AIM ID*/
    Temp.Append("0803000099459999f44501f100;"); //UPC-E Prefix-> E
    Temp.Append("0803000099639999f44101f100;"); //UPC-A Prefix-> A
	Temp.Append("0803000099449999f44602f100;"); //ENA-8 Prefix-> FF
	Temp.Append("0803000099649999f44601f100;"); //ENA-13 Prefix-> F
	Temp.Append("0803000099629999f46101f100;"); //C39 Prefix-> a
	Temp.Append("0803000099659999f46201f100;");	//Interleaved 2 of 5 Prefix-> b
	Temp.Append("08030000996A9999f46601f100");  //C128 Prefix-> f


	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}

	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
//	return true;
}

bool PlugAndPlayWinNix(void)
{
	if(ValidateTermId( 0 )==FALSE)
			return(FALSE);

	  /* Set Prefix and Suffix to canned values */
//	SetPrefixNoneSuffixCRLF();
	  /* Set Serial Port Parameters 38400, N81, RTS */
	HSTRING	Temp("0211021;0211031;060500000;0607025;0607041;0607032;", true);
	  /* Set Prefix Code ID and AIM ID*/
	Temp.Append("0804020;0202010;020D010;");
	Temp.Append("020A011;0203011;0213011;0214011;0211011;0212011;0212021;");
	Temp.Append("080500990D");

	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}

	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);

//	return true;
}

bool PlugAndPlayBeatle(void)
{
	if(ValidateTermId( 0 )==FALSE)
			return(FALSE);

	  /* Set Prefix and Suffix to canned values */
//	SetPrefixNoneSuffixCRLF();
	  /* Set Serial Port Parameters 38400, N81, RTS */
	HSTRING	Temp("",true);
	  /* Set Prefix Code ID and AIM ID*/
	Temp.Append("0803000099459999f44301f100;");
	Temp.Append("0803000099449999f44201f100;");
	Temp.Append("0803000099639999f44101f43001f100;");
	Temp.Append("0803000099649999f44101f100;");
	Temp.Append("0803000099799999f44501f100;");
	Temp.Append("0803000099669999f44801f100;");
	Temp.Append("0803000099659999f44901f100;");
	Temp.Append("08030000996A9999f44B01f100;");
	Temp.Append("0803000099699999f44C01f100;");
	Temp.Append("0803000099619999f44E01f100;");
	Temp.Append("0803000099499999f45001f100;");
	Temp.Append("0803000099729999f45101f100;"); //PDF417
	Temp.Append("0803000099779999f45201f100;"); //DATAMATRIX	
	Temp.Append("0803000099529999f45301f100;"); //MICROPDF417	
	Temp.Append("0803000099789999f45401f100;"); //MAXI CODE
	Temp.Append("0803000099739999f45501f100;"); //QR CODE
	Temp.Append("08030000997A9999f45601f100;"); //AZTEC
	Temp.Append("0803000099999999f44D01f100");

	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}

	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);

//	return true;
}

bool PlugAndPlaySetToDefault(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0D0100;0601000", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayAddCR(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("080500990D", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}
bool PlugAndPlayAddTab(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0805009909", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayAddLF(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("080500990A", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayAddCRLF(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("080500990D0A", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}

	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}


bool PlugAndPlayUSB232(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("060802130", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayUSBKBW(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("060802124", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayImageRevert(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("024B002", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayCRReplaceSemicolon(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0803000099999999E4023B0DF100", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayCRReplaceComma(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0803000099999999E4022C0DF100", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayBurningTest(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0305001;0E01050", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

//232 baud rate
bool PlugAndPlayBaudRate300(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0607020", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayBaudRate600(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0607021", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayBaudRate1200(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0607022", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayBaudRate2400(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0607023", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayBaudRate4800(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0607024", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayBaudRate9600(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0607025", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayBaudRate19200(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0607026", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayBaudRate38400(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0607027", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayBaudRate57600(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0607028", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayBaudRate115200(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0607020", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}


//232 word format
bool PlugAndPlayWordFormatN71(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0607030", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}


bool PlugAndPlayWordFormatN72(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0607031", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}


bool PlugAndPlayWordFormatN81(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0607032", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}


bool PlugAndPlayWordFormatE71(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0607033", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}


bool PlugAndPlayWordFormatE72(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0607034", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}


bool PlugAndPlayWordFormatE81(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0607035", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}


bool PlugAndPlayWordFormatO71(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0607036", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}


bool PlugAndPlayWordFormatO72(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0607037", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}


bool PlugAndPlayWordFormatO81(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0607038", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}


bool PlugAndPlayWordFormatM81(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("06070314", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayPresentation(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0E01003", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayAddPrefixAsterisk(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("080400992A", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayAddSuffixAsteriskCR(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("080500992A0D", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayAddSuffixAsterisk(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("080500992A", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayClearPrefix(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING Temp("080404", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayClearSuffix(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING Temp("080507", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayC39CheckNone(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING Temp("0203040", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayC39CheckRequest(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING Temp("0203041", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayC39CheckRequestXmit(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING Temp("0203042", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlay232Group(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);

	ReaderConfiguration->ChangeDefault( "060600", 0, false, true);
	HSTRING Temp("0606000", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayUSBGroup(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);

	ReaderConfiguration->ChangeDefault( "060600", 1, false, true);
	HSTRING Temp("0606001", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

#if defined(SM4100)
bool PlugAndPlayManualTRG(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING Temp("0E01000;0616000", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT18_LSH,GPIO_BIT18_WID,0x0);
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}

bool PlugAndPlayIR(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING	Temp("0E01000;0616001", true);
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}

	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT18_LSH,GPIO_BIT18_WID,0x1);
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}
#elif defined(IR_TRIGGER)
bool PlugAndPlayIR(void)
{
	HSTRING	Temp("0E01000;0402104;0410000;0403050;04020790;0408401;024D0D1;024D21400;024D22800;0616001.", true);
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}
#endif

#if defined(SM3110)
bool PlugAndPlayBY_DFT(void)
{
	if(ValidateTermId( 0 )==FALSE)
		return(FALSE);
	HSTRING Temp("0D0100;", true);
	Temp.Append("0606000;0607029;06070730;080500990D0A");
	
	// Terminate block entry
	if( ReaderConfiguration->CurrentTable == '@' )
	{
		Temp.Append( "@" );
	}
	else
	{
		Temp.Append( "." );
	}
	return( ReaderConfiguration->Menu(&Temp)==MENU_PASSED);
}
#endif


void READER_CONFIGURATION::CreatePlugAndPlayMenuSettings( int /* ControlMask */ )
{
//added by Chuck
#if defined(SM3110)
	AddCommand("BY_DFT", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayBY_DFT,NULL);//Baiyang Default Settings
#endif
	AddCommand("090100", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlaySetToDefault,NULL);
	AddCommand("090200", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayAddCR,NULL);
	AddCommand("090201", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayAddTab,NULL);
	AddCommand("090202", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayAddLF,NULL);
	AddCommand("090300", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayAddCRLF,NULL);
	AddCommand("090400", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayUSB232,NULL);
	AddCommand("090500", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayUSBKBW,NULL);
	AddCommand("090600", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayImageRevert,NULL);
	AddCommand("090700", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayCRReplaceSemicolon,NULL);
	AddCommand("090701", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayCRReplaceComma,NULL);
	AddCommand("090800", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayBurningTest,NULL);
	AddCommand("090900", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayAddPrefixAsterisk,NULL);
	AddCommand("090A00", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayAddSuffixAsterisk,NULL);
	AddCommand("090B00", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayAddSuffixAsteriskCR,NULL);
	AddCommand("090C00", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayClearPrefix,NULL);
	AddCommand("090C01", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayClearSuffix,NULL);

	//232 baud rate
	AddCommand("090D00", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayBaudRate300,NULL);
	AddCommand("090D01", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayBaudRate600,NULL);
	AddCommand("090D02", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayBaudRate1200,NULL);
	AddCommand("090D03", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayBaudRate2400,NULL);
	AddCommand("090D04", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayBaudRate4800,NULL);
	AddCommand("090D05", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayBaudRate9600,NULL);
	AddCommand("090D06", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayBaudRate19200,NULL);
	AddCommand("090D07", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayBaudRate38400,NULL);
	AddCommand("090D08", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayBaudRate57600,NULL);
	AddCommand("090D09", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayBaudRate115200,NULL);

	//232 word format
	AddCommand("090E00", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayWordFormatN71,NULL);
	AddCommand("090E01", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayWordFormatN72,NULL);
	AddCommand("090E02", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayWordFormatN81,NULL);
	AddCommand("090E03", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayWordFormatE71,NULL);
	AddCommand("090E04", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayWordFormatE72,NULL);
	AddCommand("090E05", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayWordFormatE81,NULL);
	AddCommand("090E06", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayWordFormatO71,NULL);
	AddCommand("090E07", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayWordFormatO72,NULL);
	AddCommand("090E08", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayWordFormatO81,NULL);
	AddCommand("090E09", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayWordFormatM81,NULL);

#if defined(SM4100)
	AddCommand("090F00", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayManualTRG,NULL);
#endif
#if defined(SM4100) || defined(IR_TRIGGER)
	AddCommand("092100", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayIR,NULL);
#endif
	AddCommand("090401", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlay232Group,NULL);
	AddCommand("091000", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayUSBGroup,NULL);
//ended by Chuck

	AddCommand("091100", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayC39CheckNone,NULL);
	AddCommand("091101", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayC39CheckRequest,NULL);
	AddCommand("091102", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayC39CheckRequestXmit,NULL);


	// Plug n Play and Other Vector Settings
	AddCommand("091200", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayGetBufferedMessage,NULL);
#if (!defined(XENON_LITE_1500)) && (!defined(HI2D_T))	/* Below commands are removed for Xenon Lite(1500) and HI2D*/
	AddCommand("091300", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &SetPrimaryFormat,NULL);//Added by Jerry
	AddCommand("091301", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &SetAlternateFormatOne,NULL);
	AddCommand("091302", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &SetAlternateFormatTwo,NULL);
	AddCommand("091303", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &SetAlternateFormatThree,NULL);
#endif
	AddCommand("091400", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &SetSuffixCR,NULL);
	AddCommand("091500", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &SetPrefixCodeID,NULL);
	AddCommand("091600", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &SendRevisionInfo,NULL);
	AddCommand("091700",SUB_TAG_TABLE::FULLY_VISIBLE,(bool(*)(void))&SendSettingForAllCable,NULL);
	AddCommand("091601", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &SendBootRevision,NULL);

	AddCommand("091602", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING, (bool (*) (void)) &SendDecoderAllRevisions,NULL);
	AddCommand("091603", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING, (bool (*) (void)) &SendDecoderRevision,NULL);
	AddCommand("091604", SUB_TAG_TABLE::FULLY_VISIBLE | LOCAL_SETTING, (bool (*) (void)) &SendScanDriverRevision,NULL);

	//PresMode plug-n-plays
#if (!defined(XENON_LITE_1500) && !defined(HI2D_T))	/* Manual (enhanced) and Streaming Pres (normal, enhanced, mobile) modes are removed for Xenon Lite(1500) */
//SP Normal :  	See command for settings.
	AddCommand("091800", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlaySPNormal,NULL);
//SP Enhanced :  	See command for settings.
	AddCommand("091801", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlaySPEnhanced,NULL);
//SP Cell Phone :  	See command for settings.
	AddCommand("091802", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlaySPCell,NULL);
//HH Snappy :  	See command for settings.
	AddCommand("091803", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayHHSnappy,NULL);
#endif
//HH Snappy :  	See command for settings.
	AddCommand("091900", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayHHSnappy,NULL);
//HH Full DOF :  	See command for settings.
	AddCommand("091A00", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayHHFull,NULL);
//HH Cell Phone :  	See command for settings.
	AddCommand("091B00", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayHHCell,NULL);
#if defined(HI2D_T)
//HI2D Presentation :  	See command for settings.
	AddCommand("090901", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayHi2dPresentaion,NULL);
	AddCommand("090902", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayHi2dPresentaionCell,NULL);
#endif

	AddCommand("091C00", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayVisualMenu,NULL);
	AddCommand("091C01", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayVisualMenu2,NULL);
	AddCommand("091C02", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayVisualMenu3,NULL);
	AddCommand("091D00", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &SendHardwareToken, NULL);
	AddCommand("091E00", SUB_TAG_TABLE::FULLY_VISIBLE, &SendDataFormatCommands,NULL);
	AddCommand("090903", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &SendPrefixBlockSize,NULL);
	AddCommand("090A01", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &SendSuffixBlockSize,NULL);
	AddCommand("091E01", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &SendDataFormatBlockSize,NULL);

	AddCommand("091F00", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayFactoryTest,NULL);
	AddCommand("091804", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlay232,NULL);
	AddCommand("091805", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayIBMAT,NULL);
//	AddCommand("PAP_XT", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayIBMXT,NULL);
	AddCommand("091806", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayIBMPS2,NULL);
//	AddCommand("PAPWM1", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayWandEmC39,NULL);
//	AddCommand("PAPWM2", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayWandEmSame,NULL);
	AddCommand("091807", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayLapTopDirect,NULL);
	AddCommand("091808", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayCloneSlave,NULL);
	AddCommand("092000", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayEriaIBMPS2,NULL);
	AddCommand("091809", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayUsbKbdPc,NULL);	//DFUSB
	AddCommand("09180A", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayUsbKbdMac,NULL);	//DFUSB

//	AddCommand("PAPHLC", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayHHLC,NULL);
//	AddCommand("PAP087", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayHHLC_C39,NULL);
//	if(GET_CONFIG_SETTING(iOpticsPopulation)==TWO_D)
//		AddCommand("PAP088", NULL, (bool (*) (void)) &PlugAndPlayHHLC_RAW,NULL);
//	else
//		AddCommand("PAP088", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayHHLC_RAW,NULL);
	if ( HardwareToken.IAmIndustrial() ) AddCommand("09180B", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PAP_BeeprLevelMax,NULL);
	AddCommand("09180C", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayOCIA_NCRShort,NULL);
	AddCommand("09180D", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayOCIA_NCRLong,NULL);
	AddCommand("09180E", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayOCIA_Nixdorf,NULL);
//	AddCommand("PAP_SP", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayOCIA_SpectrPhysics,NULL);
	if ( ! HardwareToken.IAmIndustrial() ) // Industrial won't support 4683 cable/interface
	{
		AddCommand("09180F", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayIBM_4683Port5B,NULL);
		AddCommand("091810", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayIBM_4683Port9B_HHBCR1,NULL);
		AddCommand("091811", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayIBM_4683Port9B_HHBCR2,NULL);
		AddCommand("091812", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayIBM_4683Port17,NULL);
	}
	AddCommand("091813", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlay_Symbol_3300_HHLC,NULL);
	AddCommand("091814", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayUsbPos,NULL);		//DFUSB
	AddCommand("091815", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayIBM_USB_SURE_POS_HandHeld,NULL);	//DFUSB
	AddCommand("091816", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayIBM_USB_SURE_POS_TableTop,NULL);	//DFUSB
//fixme not needed??	AddBoolean("PAPKSK", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG,NULL, &SetupKioskDecodeMode, &bKioskMode, false, false);
	AddCommand("091817", SUB_TAG_TABLE::VISIBLE_BY_SUB_TAG, (bool (*) (void)) &PlugAndPlayOPOS,NULL);

//	AddCommand("PAPOPP", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &SetPassportOCR,NULL);
	AddCommand("091818", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayRuby,NULL);
	AddCommand("091819", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayGilbarco,NULL);
	AddCommand("09181A", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayBioptiAuxPort,NULL);
	AddCommand("09181B", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayMagellan,NULL);
	AddCommand("09181C", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayNCR,NULL);
	AddCommand("09181D", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayWinNix,NULL);
	AddCommand("09181E", SUB_TAG_TABLE::FULLY_VISIBLE, (bool (*) (void)) &PlugAndPlayBeatle,NULL);

	return;
}



