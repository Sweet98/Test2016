//======================================================================================
// NonMenuCommands.cpp
//======================================================================================
// $RCSfile: NonMenucommands.cpp $
// $Revision: 1.65 $
// $Date: 2011/11/10 20:32:03EST $
//======================================================================================

//======================================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db_time.h"
#include "db_chrio.h"
#include "db_hdw.h"
#include "db_sharedtable.h"
#include "BarcodeData.h"
#include "db_NonMenuCommands.h"
#include "ioman.h"
#include "thread.h"
#include "imaging.h"
#include "MatrixHeader.h"
#include "HSTRING.h"
#include "HModem.h"
#include "SynCommandParser.h"
#include "readerconfiguration.h"
#include "productconfiguration.h"
#include "Menu.h"
#include "Interface_Support_Functions.h"
#include "ScanDriverImagingInterface.h"
#include "ScanDriverMatrixInterface.h"
#include "PowerManagement.h"
#include "beep.h"
#include "timeout.h"
#include "StartupFlashDisk.h"
#include "iMX25_IIM.h"
#include "IndicatorLEDs.h"
#include "iMX25_GPIO.h"
#include "iMX25_IOMUX.h"
#include "usb2resu.h"
#include "NonMenuCommands.h"
#include "iMX25_USBOTG.h"
#include "usb2_mx25.h"
#include "mx25spi.h"

#if defined(IR_TRIGGER)
#include "IrTrigger.h"
#endif


#ifdef FLASH_LEDS
#include "Flash_Illumination_Control.h"
#else
#include "iMX25_Illumination_Control.h"
#endif //endof FLASH_LEDS


#if FIXME
#include "db_flash.h"
#include "db_show.h"     // configuration display message prototypes
#include "FlashApplicationCode.h"
#include "FlashBootCode.h"
#include "video.h"
#endif


#ifdef EMERALD_VERIFIER
#include "hal_emerald.h"
#endif

#ifndef  RFBASE
#include "DecodeIFace.h"
#include "mainhhplproc.h"
#include "dataedit.h"
#include "i2c_drvr.h"
//#include "ScanEnginePSOC.h"                                    //lmc  not needed at this time
#ifndef RFBASE // defined(DIRT_TEST)
#include "Api.h"
#endif
#define	TRIGGER_5_5		1
#ifdef TRIGGER_5_5
#include "trigger_5_5.h"                                         //lmc   fixme
#else
#include "trigger.h"
#endif	//end #ifdef TRIGGER_5_5

#else


#endif		//end #ifndef  RFBASE

#if defined(SHOW_RESET_TO_BOOT) && !defined(MATRIX_BOOT_CODE)
#define reset_to_boot_code_hard() _reset_to_boot_code_hard(__LINE__)
void _reset_to_boot_code_hard(int);
#else
void reset_to_boot_code_hard(void);
#endif

//******************************************************************************************************************
//Defines
#define BUSTYPEMASK	0xc0000000
#define FASTBUS		0x0
#define ASYNC        BUSTYPEMASK
#define OUTPUT_DEBUG_MESSAGE theCurrentInterface.printf

//******************************************************************************************************************
//Externs
extern bool ShowFreeHeapSpace(void);
extern "C"	void MMU_CleanAndInvalidate(void);
extern int	ComputeUntouchedHeapSpace(void);
extern	bool	SendRevisionInfo(void);
extern "C" void Fast_memcpy(void *Destination, void *Source, int CopySize);
extern UINT ReadCableSelADCVoltage(void);


static void DumpGpio(void);
static bool DumpSPIFlash(void);
//******************************************************************************************************************
//Declarations
//bool DecodeCurrentImage(void);                                      //lmc  fixme
//bool RedecodeSavedImage(void);                                    //lmc  fixme

//locally called functions declaration
#ifndef RFBASE
static void SendImageData(void);
static void SendImageDataSynFe(void);
static void SendImageData_AVG_CAL(void);
static void SendImageData_Delimited(void);
static void ship_nyb(int value, int *outchr);
static void ship_kim_img(UCHAR *img, int x0, int y0, int x1, int y1, int skip, int topx);
static void SendKimData(void);
static bool CheckSpikes_D_CAL(int pos);
static bool ImgJadeWrite( void );
static bool ImgJadeRead( void );
#endif
//static void Delay(int dely);                                                                                    //lmc fixme - currently not used
//static bool CheckForNonMenuCommand(char **CommandString, UINT *CommandStringLength, const char *CommandToCheckFor);     //lmc fixme - currently not used


//Table function declarations
static bool DeviceType( void );
static bool SkipHeaderCheck( void );
static bool NewApp( void );
static bool SkipBootCodeHeaderCheck( void );
static bool NewBoot( void );
static bool SetBootCodeDownLoadMode( void );
static bool HeapUsage( void );
static bool UnUsedHeap( void );
static bool FlashDiskDirectory( void );
static bool FlashDiskDirectory( void );
static bool ReadFlashDiskFile( void );
static bool DeleteFlashDiskFile( void );
static bool Reset( void );
static bool DumpMemoryBl( void );
static bool DumpMemoryBl( void );
static bool Peek( void );
static bool Poke( void );
static bool Baud( void );
static bool GetBaud( void );
static bool SysInfo( void );
static bool Version( void );
static bool Chg_Stat( void );
static bool Id_Oid( void );
static bool DmpGpio( void );
static bool DumpMt9( void );
static bool RdOnlyI2c( void );
static bool PsocBurn( void );
static bool Psoc5100DnLd( void );
static bool PsocVerify( void );
static bool WrI2c( void );
static bool RdI2c( void );
static bool ReadI2c( void );
static bool WriteI2c( void );
static bool MxId( void );
static bool DecodeCurImg( void );
static bool ReDecode( void );
static bool TriggerSer( void );
static bool UnTriggerSer( void );
static bool TestStack( void );
static bool ConfigUpdated( void );
static bool ForceConfigUpdate( void );
static bool TestDataFormat( void );
static bool TestValParComm( void );
static bool TestKcode( void );
static bool TestFlashStatus( void );
static bool EraseFlashDisk( void );
static bool EraseAllFlash( void );
static bool Psoc_Cmd( void );
static bool Bat_Lvl( void );
static bool Bat_Temp( void );
static bool Sys_LVL( void );
static bool ManufactForceScanOn( void );
static bool ManufactLedCurrentOn( void );
static bool ManufactLedCurrentOn( void );
static bool ManufactForceScanOff( void );
static bool Passed( void );
static bool DumpImageWithCr( void );
static bool TestInject( void );
static bool Zf1( void );
static bool Zf3( void );
static bool Zf10( void );
static bool Zf11( void );
static bool Zf12( void );
static bool Zy( void );
static bool Zz( void );
static bool Zq( void );
static bool Zp( void );
static bool Vv( void );
static bool Lg( void );
static bool BepGrx( void );
static bool NMSeqAck( void );
static bool Adr2Scn( void );
static bool RadRst( void );
static bool RadioSetReset( void );
static bool RfDlTst( void );
static bool LastMethod( void );
#ifndef XENON_LITE_1500
static bool ImgSnpWParam( void );
static bool ImgShpWParam( void );
#endif
static bool GoInToStopMode( void );
static bool EnterFlashMode( void );
static bool ExitFlashMode( void );
static bool Stop_Boost( void );
static bool Start_Boost( void );
static bool GoodReadLEDCommand( void );
static bool ErrorLEDCommand( void );
static bool WatchDogTest ( void );
bool AutoCableADCReading ( void );
static bool TestJ( void );
static bool TestK( void );


#if ! defined(RFBASE)
static bool ImgStartStopTest(void);
static bool ImgStartScannningTest(void);
static bool ImgStopScannningTest(void);
static bool ImgSmearTest(void);
static bool ImgProductionTest(void);
static bool ImgSignalToNoiseTest(void);
static bool ImgRowNoiseTest(void);
static bool ImgFlatnessTest(void);
static bool ImgDirtTest(void);
static bool ImgDirtTestREV(void);
#endif

#if defined(IR_TRIGGER)
static bool NCmdIrConfig (void);
static bool NCmdIrRead (void);
#endif


//The following is the prototype of the end method in the table.
//It is used to terminate the table and must remain at the end
bool LastMethod( void );


//******************************************************************************************************************
//Variables
static int PsocTemp;

static char **pp_CommandString = 0;

static unsigned int *p_CommandStringLength = 0;

extern int PSOCBlankdownload=0; 

#ifndef RFBASE
// extra, perhaps temp material associated with PSOC / 5000VGA diagnostics
static unsigned int uiData;
static unsigned int ucAdd;
static unsigned char ucSub;
static unsigned char ucData;
static unsigned short usData[256];
static unsigned char byteData[256];
static char val[3];
static int bStatus;


static char DumpSPI[4096]={0};

#define PSOC_I2C_ADDRESS 0x80
#endif


// Keep track of whether or not we're in skip application code header check mode.
int			SkipHeaderCheckOnNextDownloadMode = 0;


// Keep track of whether or not we're in skip boot code header check mode.
int			SkipBootCodeHeaderCheckOnNextBootCodeDownloadMode = 0;


// Keep track of last address query.
char	*DisplayMemoryBlockBaseAddress = 0;


// Keep track of whether or not we're in boot code download mode.
int	BootCodeDownloadMode = 0;


//******************************************************************************************************************
//Table support


/* The following type definition represents the generic function     */
   /* pointer to be used by all commands that can be executed by the    */
   /* test program.                                                     */
typedef bool (*MenuCommandFunction_t)( void );


//function declaration of this type
static MenuCommandFunction_t FindMenuCommand( char **ppCommandString, UINT *pCommandStringLength );


//last table function
bool LastMethod( void ){return true;}


   /* The following type definition represents the structure which holds*/
   /* information used in the interpretation and execution of commmands */
typedef struct _tagMenuCommandTable_t
{
   char		              *pMenuCommandName;
   MenuCommandFunction_t  MenuCommandFunction;
} MenuCommandTable_t;



static MenuCommandTable_t MenuCommandTable[] =
{
	{"DSF",				DumpSPIFlash},
	{"EFL",				EraseAllFlash},
	{"DEVICETYPE", 		DeviceType},
	{"SLEEP",			GoInToStopMode},
#if defined(IR_TRIGGER)
	{"IRCFG",			NCmdIrConfig},
	{"IRREAD",			NCmdIrRead},
#endif
};



//****************************************************************************************************************************
//****************************************************************************************************************************
//
//Table selected Methods for non menu commands
//
//Function pointers selected from MenuCommandTable[] above.
//****************************************************************************************************************************
//****************************************************************************************************************************
bool DumpSPIFlash(void)
{
	int ret=0;
	int i=1024;
	unsigned int j=0;
	//DumpSPI = {0};
	while(i--)
	{
		memset(DumpSPI,0,4096);
		SpiMemRead(j*0x1000,0x1000,DumpSPI);

		ret = theStdInterface.Write(DumpSPI,0x1000);
		MT_Delay(1);
		wait_til_tx_is_empty();
		j++;
	}
}

		// If we received a run-time script command, get the remainder of the command and execute it.
		// Device type query.
		//Function for Non Menu Command:  "DEVICETYPE"
		bool DeviceType( void )
		{
			// If we're in skip header check mode, increment the counter.  Tools may use this command prior to a download, so we
			// want this flag to still be set when the "NEWAPP" is issued.
			if(SkipHeaderCheckOnNextDownloadMode == 1)
			{
				++SkipHeaderCheckOnNextDownloadMode;
			}

			// If we're in skip boot code header check mode, increment the counter.  Tools may use this command prior to a download, so we
			// want this flag to still be set when the "NEWBOOT" is issued.
			if(SkipBootCodeHeaderCheckOnNextBootCodeDownloadMode == 1)
			{
				++SkipBootCodeHeaderCheckOnNextBootCodeDownloadMode;
			}

			xputstring("APP,0\r");
			return true;
		}


		// Turn on skip header check mode.
		//Function for Non Menu Command:  "SKIPHEADERCHECK!@#$%^&*"
		bool SkipHeaderCheck( void )
		{
			SkipHeaderCheckOnNextDownloadMode = 2;

			return true;
		}


		// New application
		//Function for Non Menu Command:  "NEWAPP"
		bool NewApp( void )
		{
			unsigned int CommandStringLength = *p_CommandStringLength;
			char *CommandString = *pp_CommandString;

			ForceScanDriverOff();
			HSTRING		CommandRemainder( CommandStringLength, true);
			CommandRemainder.Copy(CommandString, CommandStringLength);

			if(CommandRemainder.Size == 0)
			{
				// NEWAPP - write to flash.
				//theHal.KeyWedgeOff();	               // ensure the ANA_SW is off, it could kill communication    //lmc fixme
				DownloadApplicationCode((SkipHeaderCheckOnNextDownloadMode > 0) ? true : false,
				                        (SkipHeaderCheckOnNextDownloadMode > 0) ? true : false, true);
			}
			else if (CommandRemainder.Compare("RAM", false) == 0)
			{
				// NEWAPPRAM - don't write to flash.
				DownloadApplicationCode((SkipHeaderCheckOnNextDownloadMode > 0) ? true : false,
				                        (SkipHeaderCheckOnNextDownloadMode > 0) ? true : false, false);
			}
			else
			{
				// Unrecognized command qualifier; WOOF!
				xputchar(NAK);
			}

			SkipHeaderCheckOnNextDownloadMode = 0;

			return true;
		}


		// Turn on boot code skip header check mode.
		//Function for Non Menu Command:  "SKIPBOOTCODEHEADERCHECK!@#$%^&*"
		bool SkipBootCodeHeaderCheck( void )
		{
			SkipBootCodeHeaderCheckOnNextBootCodeDownloadMode = 3;

			return true;
		}


		// New boot
		//Function for Non Menu Command:  "NEWBOOT"
		bool NewBoot( void )
		{
			ForceScanDriverOff();
			DownloadBootCode( (SkipBootCodeHeaderCheckOnNextBootCodeDownloadMode > 0) ? true : false );
			BootCodeDownloadMode = 0;
			SkipBootCodeHeaderCheckOnNextBootCodeDownloadMode = 0;

			return true;
		}


		// Turn on boot code download mode.
		//Function for Non Menu Command:  "SETBOOTCODEDOWNLOADMODE"
		bool SetBootCodeDownLoadMode( void )
		{
			xputstring("SETBOOTCODEDOWNLOADMODE\015");
			BootCodeDownloadMode = 2;

			return true;
		}


bool TestJ( void )
{
	UINT reg_val;
	UINT timeout;
	if(GET_SETTING(TerminalID)==0)
	{
		reg_val = USB_CTRL;
		reg_val |= UCTRL_OWIE | UCTRL_OPM;
		USB_CTRL = reg_val;      //Init_MX25USB2Otg

		reg_val = UOG_PORTSC1;
		reg_val &= ~PORTSC_PTS_MASK;	//set UTMI xcvr
		reg_val |= PORTSC_PTS_UTMI;
		UOG_PORTSC1 = reg_val;

		//set UTMI PHY
		reg_val = USB_PHY_CTRL_FUNC;
		reg_val |= UTMI_USBEN ;
		// Set EVDO at suggestion of Freescale to fix NG2D-1134
		reg_val |= UTMI_EVDO; 
		reg_val &= ~(UTMI_HOSTPORT|UTMI_LSFE|UTMI_VCONTROLLOADM);
		USB_PHY_CTRL_FUNC = reg_val;

		reg_val = UOG_USBCMD;
		reg_val &= ~USB_CMD_RUN_STOP;
		UOG_USBCMD = reg_val;

		reg_val = UOG_USBCMD;
		reg_val |= USB_CMD_CTRL_RESET;
		UOG_USBCMD = reg_val;

		/* Wait for reset to complete */
		timeout = 10000000;
		while ((UOG_USBCMD & USB_CMD_CTRL_RESET) && --timeout) 
		{
			continue;
		}
		if (timeout == 0) 
		{
			return false;
		}

		//Set the controller as device mode
		reg_val = UOG_USBMODE;
		reg_val |= USB_MODE_CTRL_MODE_DEVICE;
		reg_val |= USB_MODE_SETUP_LOCK_OFF;
		reg_val |= USB_MODE_STREAM_DISABLE;
		UOG_USBMODE = reg_val;

		//Clear the setup status
		UOG_USBSTS = 0;

		reg_val = UOG_PORTSC1;
		reg_val &= ~PORTSCX_PHY_TYPE_SEL;
		reg_val |= PORTSC_PTS_UTMI;
		reg_val |= 0x1000000;
		UOG_PORTSC1 = reg_val;

		// Set the controller as device mode
		reg_val = UOG_USBMODE;
		reg_val |= USB_MODE_CTRL_MODE_DEVICE;
		UOG_USBMODE = reg_val;

		reg_val = USB_INTR_INT_EN | USB_INTR_ERR_INT_EN | USB_INTR_PTC_DETECT_EN | USB_INTR_RESET_EN |USB_INTR_DEVICE_SUSPEND | USB_INTR_SYS_ERR_EN;
		UOG_USBINTR = reg_val;

		// Set controller to Run
		reg_val = UOG_USBCMD;
		reg_val |= USB_CMD_RUN_STOP;
		UOG_USBCMD = reg_val;
	}							
	reg_val = UOG_PORTSC1;
	reg_val |= 0x01 <<16;
	UOG_PORTSC1 = reg_val;	
	return true;	
}

bool TestK( void )
{
	UINT reg_val;
	UINT timeout;
	if(GET_SETTING(TerminalID)==0)
	{
		reg_val = USB_CTRL;
		reg_val |= UCTRL_OWIE | UCTRL_OPM;
		USB_CTRL = reg_val;      //Init_MX25USB2Otg

		reg_val = UOG_PORTSC1;
		reg_val &= ~PORTSC_PTS_MASK;	//set UTMI xcvr
		reg_val |= PORTSC_PTS_UTMI;
		UOG_PORTSC1 = reg_val;

		//set UTMI PHY
		reg_val = USB_PHY_CTRL_FUNC;
		reg_val |= UTMI_USBEN ;
		// Set EVDO at suggestion of Freescale to fix NG2D-1134
		reg_val |= UTMI_EVDO; 
		reg_val &= ~(UTMI_HOSTPORT|UTMI_LSFE|UTMI_VCONTROLLOADM);
		USB_PHY_CTRL_FUNC = reg_val;

		reg_val = UOG_USBCMD;
		reg_val &= ~USB_CMD_RUN_STOP;
		UOG_USBCMD = reg_val;

		reg_val = UOG_USBCMD;
		reg_val |= USB_CMD_CTRL_RESET;
		UOG_USBCMD = reg_val;

		/* Wait for reset to complete */
		timeout = 10000000;
		while ((UOG_USBCMD & USB_CMD_CTRL_RESET) && --timeout) 
		{
			continue;
		}
		if (timeout == 0) 
		{
			return false;
		}

		//Set the controller as device mode
		reg_val = UOG_USBMODE;
		reg_val |= USB_MODE_CTRL_MODE_DEVICE;
		reg_val |= USB_MODE_SETUP_LOCK_OFF;
		reg_val |= USB_MODE_STREAM_DISABLE;
		UOG_USBMODE = reg_val;

		//Clear the setup status
		UOG_USBSTS = 0;

		reg_val = UOG_PORTSC1;
		reg_val &= ~PORTSCX_PHY_TYPE_SEL;
		reg_val |= PORTSC_PTS_UTMI;
		reg_val |= 0x1000000;
		UOG_PORTSC1 = reg_val;

		// Set the controller as device mode
		reg_val = UOG_USBMODE;
		reg_val |= USB_MODE_CTRL_MODE_DEVICE;
		UOG_USBMODE = reg_val;

		reg_val = USB_INTR_INT_EN | USB_INTR_ERR_INT_EN | USB_INTR_PTC_DETECT_EN | USB_INTR_RESET_EN |USB_INTR_DEVICE_SUSPEND | USB_INTR_SYS_ERR_EN;
		UOG_USBINTR = reg_val;

		// Set controller to Run
		reg_val = UOG_USBCMD;
		reg_val |= USB_CMD_RUN_STOP;
		UOG_USBCMD = reg_val;
	}				
	reg_val = UOG_PORTSC1;
	reg_val |= 0x02 <<16;
	UOG_PORTSC1 = reg_val;		
	return true;	
}


//		
//MFG COMMAND BOOST COMMENT:		
//		THE FOLLOWING COMMANDS ARE USED IN MFG. 
//		THEY NEED TO KNOW IF WE CHANGE THE OPERATION OF THESE COMMANDS
	//Enter and Exit LED Flash Mode
		bool EnterFlashMode ( void )
		{
#ifdef FLASH_LEDS
			SetILL_FlashMode(0x1);
			xputstring("Flash Mode - High Current\r\n");
#endif
			return true;
		}
		bool ExitFlashMode ( void )
		{
#ifdef FLASH_LEDS
			SetILL_FlashMode(0x0);
			xputstring("Normal Mode - Low Current\r\n");
#endif
			return true;
		}
		bool Stop_Boost ( void )
		{
#ifdef FLASH_LEDS
			Disable_Boost();
			xputstring("Disable Boost\r\n");
#endif
			return true;
		}
		bool Start_Boost ( void )
		{
#ifdef FLASH_LEDS
			Enable_Boost();
			xputstring("Enable Boost\r\n");
#endif
			return true;
		}
//		
// END MFG COMMAND BOOST COMMENT		
//		


		
		bool GoodReadLEDCommand ( void )
		{
			if(**pp_CommandString == '1')
			{
				GoodReadLedOn();
				xputstring("Good Read LED ON\r\n");
			}
			else if(**pp_CommandString == '0')
			{
				GoodReadLedOff();
				xputstring("Good Read LED OFF\r\n");
			}
			return true;
		}
		
		bool ErrorLEDCommand ( void )
		{
			if(**pp_CommandString == '1')
			{
				ErrorLedOn();
				xputstring("Error LED ON\r\n");
			}
			else if(**pp_CommandString == '0')
			{
				ErrorLedOff();
				xputstring("Error LED OFF\r\n");
			}
			return true;
		}
		
		//Call PowerManagment to enter Stop Mode
		bool GoInToStopMode( void )
		{
			xputstring("Now Entering Stop Mode\r\n");
			wait_til_tx_is_empty();
		
			MX25USB2_suspend();
			EnterPowerState(STOP);
			MX25USB2_resume();
	
			return true;
		}
		
		//Test the watchdog
		bool WatchDogTest( void )
		{
//			xputstring("Wait for it\r\n");
//			wait_til_tx_is_empty();
//			while(1);

			return true;
		}
		
		bool AutoCableADCReading ( void )
		{
			xputstring("ADC Value - ");
			xputhex(ReadCableSelADCVoltage(),4);
			xputstring("\r\n");
			
			return true;
		}

		// Display heap usage.
		//Function for Non Menu Command:  "HEAPUSAGE"
		bool HeapUsage( void )
		{
			ShowFreeHeapSpace();

			return true;
		}


		// Display unused heap.
		//Function for Non Menu Command:  "UNUSEDHEAP"
		bool UnUsedHeap( void )
		{
			char	ResultString[160];

			sprintf( ResultString, "Unused heap space = %d\r\n", ComputeUntouchedHeapSpace() );
			xputstring( ResultString );

			return true;
		}


		// Display flash disk directory .
		//Function for Non Menu Command:  "FLASHDISKDIRECTORY"
		bool FlashDiskDirectory( void )
		{
			HSTRING	Report(300, true);
			//FlashDisk->CreateFormattedDirectoryReport(&Report);               //lmc fixme
			xputstring((char *) Report.Char, Report.Size);

			return true;
		}


		// Read flash disk file.
		//Function for Non Menu Command:  "READFLASHDISKFILE"
		bool ReadFlashDiskFile( void )
		{
			HSTRING	FileContents(20, true);
			int		FileIndex = -1;
			char	ResultString[160];
			int i;

			//FileIndex = FlashDisk->Read(*pp_CommandString, &FileContents);                  //lmc fixme

			if(FileIndex < 0)
			{
				// Report any errors.
				sprintf(ResultString, "Error reading file %s : index = %d\r\n", *pp_CommandString, FileIndex);
				xputstring(ResultString);
			}

			// If we had no errors or the file is corrupt (mismatched CRC), show it.
			if(0)//((FileIndex >= 0) || (FileIndex == MATRIX_FLASH_DISK::FD_CORRUPT_FILE))			//lmc fixme
			{
				i = 0;
				while(i < FileContents.Size)
				{
					// Write out 80 characters per line.
					if((i + 80) < FileContents.Size)
					{
						xputstring(((char *) &FileContents.Char[i]), 80);
					}
					else
					{
						xputstring(((char *) &FileContents.Char[i]), FileContents.Size - i);
					}

					i += 80;
					//					xputstring("\r\n");
				}
			}

			return true;
		}


		// Delete flash disk file.
		//Function for Non Menu Command:  "DELETEFLASHDISKFILE"
		bool DeleteFlashDiskFile( void )
		{
			int		FileIndex = -1;
			char	ResultString[160];

			//FileIndex = FlashDisk->RemoveFile( *pp_CommandString );               //lmc fixme

			if(FileIndex < 0)
			{
				// Couldn't delete file.
				sprintf(ResultString, "Couldn't delete file %s : index = %d\r\n", *pp_CommandString, FileIndex);
				xputstring(ResultString);
			}

			return true;
		}


		// Reset to boot code.
		//Function for Non Menu Command:  "RESET"
		bool Reset( void )
		{
			reset_to_boot_code_hard();

			return true;
		}


		// Dump a memory block.
		//Function for Non Menu Command:  "DUMPMEMORY"
		bool DumpMemoryBl( void )
		{
			char	ResultString[160];
			int i = 0;

			if( *p_CommandStringLength > 0 )
			{
				sscanf (*pp_CommandString, "%X", &i);		/* parse the offset */
				DisplayMemoryBlockBaseAddress = ((char *) i);
			}

			if ((((int) DisplayMemoryBlockBaseAddress) % 16) != 0)
			{
				sprintf (ResultString,"\r\n%08X -", ((int) DisplayMemoryBlockBaseAddress)) ;
				xputstring(ResultString);
			}

			for (i = 0;    i < 256 ; i++)
			{
				if ((((int) DisplayMemoryBlockBaseAddress) % 16) == 0)
				{
					sprintf (ResultString,"\r\n%08X -", ((int) DisplayMemoryBlockBaseAddress)) ;
					xputstring(ResultString);
				}

				sprintf (ResultString," %02X", ((int) *DisplayMemoryBlockBaseAddress)) ;
				xputstring (ResultString);
				++DisplayMemoryBlockBaseAddress;
			}

			xputstring ("\r\n") ;

			return true;
		}

		// Peek memory.
		//Function for Non Menu Command:  "PEEK"
		bool Peek( void )
		{
			char	ResultString[160];
			int i = 0;

			if(*p_CommandStringLength > 0)
			{
				sscanf( *pp_CommandString, "%X", &i );		/* parse the offset */
				DisplayMemoryBlockBaseAddress = ((char *) i);
				sprintf (ResultString,"Peek %08X -", ((int) DisplayMemoryBlockBaseAddress)) ;
				xputstring (ResultString);
				sprintf (ResultString," %08X", (*(int *)DisplayMemoryBlockBaseAddress)) ;
				xputstring (ResultString);
				xputstring ("\r\n") ;
			}

			return true;
		}


		// Poke memory.
		//Function for Non Menu Command:  "POKE"
		bool Poke( void )
		{
			int j;
			char	ResultString[160];
			int i = 0;

			if( *p_CommandStringLength > 0 )
			{
				if( sscanf( *pp_CommandString, "%X %X", &i, &j)==2 );		/* parse the offset */

				(*(int *)i)=j;
				DisplayMemoryBlockBaseAddress = ((char *) i);
				sprintf (ResultString,"Poke %08X -", ((int) DisplayMemoryBlockBaseAddress)) ;
				xputstring (ResultString);
				sprintf (ResultString," %08X readback", (*(int *)DisplayMemoryBlockBaseAddress)) ;
				xputstring (ResultString);
				xputstring ("\r\n") ;
			}

			return true;
		}


		// Test baud rate.
		//Function for Non Menu Command:  "BAUD"
		bool Baud( void )
		{
			SetBaudRate(atoi(*pp_CommandString));

			return true;
		}


		// Get baud rate.
		//Function for Non Menu Command:  "GETBAUD"
		bool GetBaud( void )
		{
			char	ResultString[160];

			sprintf(ResultString, "Baud rate = %d\r\n", GetBaudRate());
			xputstring(ResultString);

			return true;
		}


		// Dump system information.
		//Function for Non Menu Command:  "SYSINFO"
		bool SysInfo( void )
		{
			//show_system();			//lmc fixme

			return true;
		}


		// Dump version information.
		//Function for Non Menu Command:  "VERSION"
		bool Version( void )
		{
			//show_system();				//lmc fixme

			return true;
		}


		//Dump GPIO
		//Function for Non Menu Command:  "DUMPGPIO"
		bool DmpGpio( void )
		{
			DumpGpio();

			return true;
		}


		// report MX25 chip id
		//Function for Non Menu Command:  "MXID"
		bool MxId( void )
		{
			char MsgBuf[100];
			int    revision;

			revision = READ_REG(IIM_SREV);
			sprintf (MsgBuf, "MX25 Silicon Revision Reg (SREV) = 0x%02x\r\n", revision);
			xputstring (MsgBuf);

			return true;
		}


		// Test stack usage.
		//Function for Non Menu Command:  "TESTSTACK"
		bool TestStack( void )
		{
			char	ResultString[160];
			int i = 0;

			UINT	SvcStackSize = (UINT) SVC_STACK - (UINT) ABT_STACK;
			UINT	SvcStackBase = (UINT) SVC_STACK - SvcStackSize;
			UINT	AbtStackSize = (UINT) ABT_STACK - (UINT) UNDEF_STACK;
			UINT	AbtStackBase = (UINT) ABT_STACK - AbtStackSize;
			UINT	UndStackSize = (UINT) UNDEF_STACK - (UINT) IRQ_STACK;
			UINT	UndStackBase = (UINT) UNDEF_STACK - UndStackSize;
			UINT	IrqStackSize = (UINT) IRQ_STACK - (UINT) FIQ_STACK;
			UINT	IrqStackBase = (UINT) IRQ_STACK - IrqStackSize;
			UINT	FiqStackSize = (UINT) FIQ_STACK - (UINT) SYS_STACK;
			UINT	FiqStackBase = (UINT) FIQ_STACK - FiqStackSize;
			UINT	UsrStackSize = (UINT) SYS_STACK - (UINT) HEAP_END;
			UINT	UsrStackBase = (UINT) SYS_STACK - UsrStackSize;
			UINT	SysStackSize = UsrStackSize;
			UINT	SysStackBase = UsrStackBase;

			char	*StackPtr;

			// Test the system stack.
			StackPtr = ((char *) SysStackBase);

			for(i = 0;    i < SysStackSize;    ++i)
			{
				if(StackPtr[i] != 0xFF)
					break;
			}

			sprintf(ResultString, "Unused Sys Stack (%08X) = %d bytes\r\n", SysStackBase, i);
			xputstring(ResultString);

			// Test the user stack.
			StackPtr = ((char *) UsrStackBase);

			for(i = 0;    i < UsrStackSize;    ++i)
			{
				if(StackPtr[i] != 0xFF)
					break;
			}

			sprintf(ResultString, "Unused Usr Stack (%08X) = %d bytes\r\n", UsrStackBase, i);
			xputstring(ResultString);

			// Test the FIQ stack.
			StackPtr = ((char *) FiqStackBase);

			for(i = 0;    i < FiqStackSize;    ++i)
			{
				if(StackPtr[i] != 0xFF)
					break;
			}

			sprintf(ResultString, "Unused Fiq Stack (%08X) = %d bytes\r\n", FiqStackBase, i);
			xputstring(ResultString);

			// Test the IRQ stack.
			StackPtr = ((char *) IrqStackBase);

			for(i = 0;    i < IrqStackSize;    ++i)
			{
				if(StackPtr[i] != 0xFF)
					break;
			}

			sprintf(ResultString, "Unused Irq Stack (%08X) = %d bytes\r\n", IrqStackBase, i);
			xputstring(ResultString);

			// Test the Und stack.
			StackPtr = ((char *) UndStackBase);

			for(i = 0;    i < UndStackSize;    ++i)
			{
				if(StackPtr[i] != 0xFF)
					break;
			}

			sprintf(ResultString, "Unused Und Stack (%08X) = %d bytes\r\n", UndStackBase, i);
			xputstring(ResultString);

			// Test the Abt stack.
			StackPtr = ((char *) AbtStackBase);

			for(i = 0;    i < AbtStackSize;    ++i)
			{
				if(StackPtr[i] != 0xFF)
					break;
			}

			sprintf(ResultString, "Unused Abt Stack (%08X) = %d bytes\r\n", AbtStackBase, i);
			xputstring(ResultString);

			// Test the Svc stack.
			StackPtr = ((char *) SvcStackBase);

			for(i = 0;    i < SvcStackSize;    ++i)
			{
				if(StackPtr[i] != 0xFF)
					break;
			}

			sprintf(ResultString, "Unused Svc Stack (%08X) = %d bytes\r\n", SvcStackBase, i);
			xputstring(ResultString);

			for( threadhandle_t hndl=1; hndl<MAXTHREADS;hndl++ )
			{
				if( IsThreadOpen( hndl ) )
				{
					sprintf(ResultString, "Thread %ld:Unused Driver Thread Stack = %d bytes\r\n",hndl,ReportDriverThreadUnusedStack(hndl));
					xputstring(ResultString);
				}
			}

			return true;
		}


		// Display configuration update information.
		//Function for Non Menu Command:  "CONFIGUPDATED"
		bool ConfigUpdated( void )
		{
			if (ReaderConfiguration->CustomerDefaultsUpdated())
			{
				xputstring("Customer defaults updated:  YES\r\n");
			}
			else
			{
				xputstring("Customer defaults updated:  NO\r\n");
			}

			if(ReaderConfiguration->WorkingValuesUpdated())
			{
				xputstring("Working values updated:     YES\r\n");
			}
			else
			{
				xputstring("Working values updated:     NO\r\n");
			}

			if (ReaderConfiguration->ActiveValuesUpdated())
			{
				xputstring("Active values updated:      YES\r\n");
			}
			else
			{
				xputstring("Active values updated:      NO\r\n");
			}

			return true;
		}


		// Force configuration to updated state.
		//Function for Non Menu Command:  "FORCECONFIGUPDATE"
		bool ForceConfigUpdate( void )
		{
			ReaderConfiguration->CustomerDefaultsUpToDate();
			ReaderConfiguration->WorkingValuesUpToDate();
			ReaderConfiguration->ActiveValuesUpToDate();

			return true;
		}


		// Test a data format.
		//Function for Non Menu Command:  "TESTDATAFORMAT"
		bool TestDataFormat( void )
		{
			HSTRING	FormattedCommandString(*p_CommandStringLength, true);

			FormattedCommandString.Copy(*pp_CommandString, *p_CommandStringLength);
			FormattedCommandString.UnprintableFormatsToCharacters();

			if(FormattedCommandString.Size > 3)
			{
				CBarcodeData *pBarcode;
				pBarcode = CBarcodeData::StoreToNewBuffer(&(FormattedCommandString.Char[3]), FormattedCommandString.Size - 3);
				pBarcode->StoreHHP(FormattedCommandString.Char[0], 0);
				pBarcode->StoreAim(FormattedCommandString.Char[1], FormattedCommandString.Char[2]);

				//theDataEditor.GetDataEditSettingsFromMenu() ;				//lmc fixme

				if(0)// ( theDataEditor.DataEdit(pBarcode) )                 //lmc fixme
				{
					pBarcode->GetText(&FormattedCommandString);
					pBarcode->Free();
					pBarcode = NULL;

					xputstring("Result = ");
					xputstring(FormattedCommandString.Char, FormattedCommandString.Size);
					xputstring("\r\n");
				}
				else
				{
					xputstring("Format failed\r\n");
				}
			}
			else
			{
				xputstring("Input string too short; must include HHP ID, AIM ID, and AIM mod followed by data\r\n");
			}

			return true;
		}


		// Test ValidatePartialCommand.
		//Function for Non Menu Command:  "TESTVPC"
		bool TestValParComm( void )
		{
			// Create the configuration command.
			HSTRING	Command(*p_CommandStringLength, true);
			Command.Copy(*pp_CommandString, *p_CommandStringLength);

			// Validate the partial command.
			bool	MaximumCommandLengthLimitReached = 0;

			if(0)//(ReaderConfiguration->ValidatePartialCommand((char *) Command.Char, &MaximumCommandLengthLimitReached))			//lmc fixme
			{
				xputstring("Partial command is valid:      ");
			}
			else
			{
				xputstring("Partial command is invalid:    ");
			}

			if(MaximumCommandLengthLimitReached)
			{
				xputstring("maximum length");
			}
			else
			{
				xputstring("not maximum length");
			}

			xputstring("\r\n");

			return true;
		}


		// Test KCODE.
		//Function for Non Menu Command:  "TESTKC"
		bool TestKcode( void )
		{
			char	ResultString[160];

			int Status =EnterScannedMenu(p_CommandStringLength, (UCHAR *) *pp_CommandString);
			sprintf(ResultString, "TESTKC status = %d\r\n", Status);
			xputstring(ResultString);

			return true;
		}


		// Command to test psoc stuff
		//Function for Non Menu Command:  "PSOC_CMD"
		bool Psoc_Cmd( void )
		{
			char MsgBuf[100];

			PsocTemp = atoi (*pp_CommandString);

			if(0)//(GET_CONFIG_SETTING(iOpticsPopulation)==TWO_D) //Are we 1D??                           //lmc fixme
			{
				sprintf (MsgBuf, "Psoc command sent = %d\r\n", PsocTemp);
				xputstring (MsgBuf);
/*                                                                                   //lmc fixme
				switch ( PsocTemp )
				{
				case PSOC_CMD_KEY_WEDGE_ON :
					theHal.KeyWedgeOn();
					break;

				case PSOC_CMD_KEY_WEDGE_OFF :
					theHal.KeyWedgeOff();
					break;
				case PSOC_CMD_START_LED_PWM :
					theHal.Start_Scanning();
					break;
				case PSOC_CMD_STOP_LED_PWM :
					theHal.Stop_Scanning();
					break;

#ifndef NEW_EXPOSURE

				case PSOC_CMD_LED_INTERNAL :
					theHal.Set_LED_Clock_Internal();
					break;
				case PSOC_CMD_LED_EXTERNAL :
					theHal.Set_LED_Clock_External();
					break;

				case PSOC_CMD_LED_100:
				case PSOC_CMD_LED_0:
				case PSOC_CMD_LED_1:
				case PSOC_CMD_LED_2:
				case PSOC_CMD_LED_3:
				case PSOC_CMD_LED_4:
				case PSOC_CMD_LED_5:
				case PSOC_CMD_LED_6:
				case PSOC_CMD_LED_7:
				case PSOC_CMD_LED_8:
				case PSOC_CMD_LED_9:
				case PSOC_CMD_LED_10:
				case PSOC_CMD_LED_11:
				case PSOC_CMD_LED_12:
				case PSOC_CMD_LED_13:
				case PSOC_CMD_LED_14:
				case PSOC_CMD_LED_15:
				case PSOC_CMD_LED_16:
				case PSOC_CMD_LED_17:
				case PSOC_CMD_LED_18:
				case PSOC_CMD_LED_19:
				case PSOC_CMD_LED_20:
				case PSOC_CMD_LED_21:
				case PSOC_CMD_LED_22:
				case PSOC_CMD_LED_23:
				case PSOC_CMD_LED_24:
				case PSOC_CMD_LED_25:
				case PSOC_CMD_LED_26:
				case PSOC_CMD_LED_27:
				case PSOC_CMD_LED_28:
				case PSOC_CMD_LED_29:
				case PSOC_CMD_LED_30:
				case PSOC_CMD_LED_31:
				case PSOC_CMD_LED_32:
				case PSOC_CMD_LED_33:
				case PSOC_CMD_LED_34:
				case PSOC_CMD_LED_35:
				case PSOC_CMD_LED_36:
				case PSOC_CMD_LED_37:
				case PSOC_CMD_LED_38:
				case PSOC_CMD_LED_39:
				case PSOC_CMD_LED_40:
				case PSOC_CMD_LED_41:
				case PSOC_CMD_LED_42:
				case PSOC_CMD_LED_43:
				case PSOC_CMD_LED_44:
				case PSOC_CMD_LED_45:
				case PSOC_CMD_LED_46:
				case PSOC_CMD_LED_47:
				case PSOC_CMD_LED_48:
				case PSOC_CMD_LED_49:
				case PSOC_CMD_LED_50:
				case PSOC_CMD_LED_51:
				case PSOC_CMD_LED_52:
				case PSOC_CMD_LED_53:
				case PSOC_CMD_LED_54:
				case PSOC_CMD_LED_55:
				case PSOC_CMD_LED_56:
				case PSOC_CMD_LED_57:
				case PSOC_CMD_LED_58:
				case PSOC_CMD_LED_59:
				case PSOC_CMD_LED_60:
				case PSOC_CMD_LED_61:
				case PSOC_CMD_LED_62:
				case PSOC_CMD_LED_63:
				case PSOC_CMD_LED_64:
				case PSOC_CMD_LED_65:
				case PSOC_CMD_LED_66:
				case PSOC_CMD_LED_67:
				case PSOC_CMD_LED_68:
				case PSOC_CMD_LED_69:
				case PSOC_CMD_LED_70:
				case PSOC_CMD_LED_71:
				case PSOC_CMD_LED_72:
				case PSOC_CMD_LED_73:
				case PSOC_CMD_LED_74:
				case PSOC_CMD_LED_75:
				case PSOC_CMD_LED_76:
				case PSOC_CMD_LED_77:
				case PSOC_CMD_LED_78:
				case PSOC_CMD_LED_79:
				case PSOC_CMD_LED_80:
				case PSOC_CMD_LED_81:
				case PSOC_CMD_LED_82:
				case PSOC_CMD_LED_83:
				case PSOC_CMD_LED_84:
				case PSOC_CMD_LED_85:
				case PSOC_CMD_LED_86:
				case PSOC_CMD_LED_87:
				case PSOC_CMD_LED_88:
				case PSOC_CMD_LED_89:
				case PSOC_CMD_LED_90:
				case PSOC_CMD_LED_91:
				case PSOC_CMD_LED_92:
				case PSOC_CMD_LED_93:
				case PSOC_CMD_LED_94:
				case PSOC_CMD_LED_95:
				case PSOC_CMD_LED_96:
				case PSOC_CMD_LED_97:
				case PSOC_CMD_LED_98:
				case PSOC_CMD_LED_99:
					theHal.Set_LED_Current(PsocTemp-PSOC_CMD_LED_0);
					break;
#endif

				default:
					break;
				}         */                     //lmc   fixme
			}
			else		//if 1D
			{
				//	Psoc_Send_Byte((unsigned char)PsocTemp);
				//	sprintf (MsgBuf, "Psoc command sent = %d\r\n", PsocTemp);
				//	xputstring (MsgBuf);
				sprintf (MsgBuf, "Psoc command sent = %d\r\n", PsocTemp);
				xputstring (MsgBuf);
/*                                                                         //lmc fixme
				switch ( PsocTemp )
				{
				case PSOC_CMD_KEY_WEDGE_ON :
					theHal.KeyWedgeOn();
					break;

				case PSOC_CMD_KEY_WEDGE_OFF :
					theHal.KeyWedgeOff();
					break;

#ifndef NEW_EXPOSURE

				case PSOC_CMD_SH_0:
				case PSOC_CMD_SH_1:
				case PSOC_CMD_SH_2:
				case PSOC_CMD_SH_3:
				case PSOC_CMD_SH_4:
				case PSOC_CMD_SH_5:
				case PSOC_CMD_SH_6:
				case PSOC_CMD_SH_7:
				case PSOC_CMD_SH_8:
				case PSOC_CMD_SH_9:
				case PSOC_CMD_SH_10:
				case PSOC_CMD_SH_11:

#ifdef EXP_ADJUST
					theHal.Set_Exposure(PsocTemp+EXP_ADJUST);
#else
					theHal.Set_Exposure(PsocTemp);
#endif

					break;
#endif

				case PSOC_CMD_START_SCAN:
					theHal.Start_Scanning();
					break;
				case PSOC_CMD_STOP_SCAN:
					theHal.Stop_Scanning();
					break;
				case PSOC_CMD_LED_SET_CURRENT_LVL:				//For the 3800G this is a command to change the DC_Offset voltage.
					PsocTemp = atoi (&*pp_CommandString[3]);
					theHal.Set_DC_Offset(PsocTemp);
					printf ("DCO to %d\r\n",PsocTemp);
					break;

#if defined(RFBASE) || defined(RFSCAN)

				case PSOC_CMD_CLK_5_KHZ:

					theHal.Set_Sampling_5Khz();
					break;
				case PSOC_CMD_CLK_1_MHZ:
					theHal.Set_Sampling_1Mhz();
					break;
#endif

				default:
					break;
				}                              */                    //lmc fixme
			}

			return true;
		}




//****************************************************************************************************************************
#ifdef EMERALD_VERIFIER

		//Charging state
		//Function for Non Menu Command:  "CHG_STAT"
		bool Chg_Stat( void )
		{
			fprintf(stderr,"Charging State = %x\r\n",theHal.IsChargeStat());

			return true;
		}


		// Check external Device Type
		//Function for Non Menu Command:  "ID_OID"
		bool Id_Oid( void )
		{
			char	ResultString[160];

			switch(theHal.ID_ExternalOID(FALSE))
			{
			case OID_TEST_BUSY:
				sprintf (ResultString,"External Device: OID_TEST_BUSY") ;
				break;
			case NoScannerAttached:
				sprintf (ResultString,"External Device: NoScannerAttached") ;
				break;
			case _660nm_20mil_wand:
				sprintf (ResultString,"External Device: _660nm_20mil_wand") ;
				break;
			case _660nm_10mil_wand:
				sprintf (ResultString,"External Device: _660nm_10mil_wand") ;
				break;
			case _660nm_3_40mil_wand:
				sprintf (ResultString,"External Device: _660nm_3_40mil_wand") ;
				break;
			case _660nm_5mil_wand:
				sprintf (ResultString,"External Device: _660nm_5mil_wand") ;
				break;
			case _660nm_8mil_wand:
				sprintf (ResultString,"External Device: _660nm_8mil_wand") ;
				break;
			case _660nm_6mil_wand:
				sprintf (ResultString,"External Device: _660nm_6mil_wand") ;
				break;
			case _660nm_3mil_wand:
				sprintf (ResultString,"External Device: _660nm_3mil_wand") ;
				break;
			case Special_scanner:
				sprintf (ResultString,"External Device: Special_scanner") ;
				break;
			case _940nm_20mil_wand:
				sprintf (ResultString,"External Device: _940nm_20mil_wand") ;
				break;
			case _940nm_10mil_wand:
				sprintf (ResultString,"External Device: _940nm_10mil_wand") ;
				break;
			case _940nm_3_40mil_wand:
				sprintf (ResultString,"External Device: _940nm_3_40mil_wand") ;
				break;
			case _940nm_5mil_wand:
				sprintf (ResultString,"External Device: _940nm_5mil_wand") ;
				break;
			case _940nm_8mil_wand:
				sprintf (ResultString,"External Device: _940nm_8mil_wand") ;
				break;
			case _940nm_6mil_wand:
				sprintf (ResultString,"External Device: _940nm_6mil_wand") ;
				break;
			case _940nm_3mil_wand:
				sprintf (ResultString,"External Device: _940nm_3mil_wand") ;
				break;
			case _LASER:
				sprintf (ResultString,"External Device: _LASER") ;
				break;
			case _3800V:
				sprintf (ResultString,"External Device: _3800V") ;
				break;
			}
			xputstring (ResultString);
			xputstring ("\r\n") ;

			return true;
		}
#elif defined( RFBASE )

#else
		bool Chg_Stat( void ){return false;}
		bool Id_Oid( void ){return false;}
#endif



//****************************************************************************************************************************
#ifndef RFBASE

		// extra, perhaps temp material associated with PSOC / 5000VGA diagnostics
		//Function for Non Menu Command:  "DumpMT9"
		bool DumpMt9( void )
		{
			int i;
			char MsgBuf[100];

			//Psoc5100PowerUpForRegisterProcessing();				//lmc fixme

			//bStatus =ReadIIC_MT9V022 ((unsigned char)0,usData,(unsigned int)256);			//lmc fixme

			for (i=0; i<256; i++)
			{
				sprintf (MsgBuf, "0x%02x/0x%04x/%d\r\n",i, usData[i], usData[i]);
				xputstring (MsgBuf);
			}

			//Psoc5100PowerDownAfterRegisterProcessing();				//lmc fixme

			return true;
		}


		//Read PSOC I2C
		//Function for Non Menu Command:  "RdOnlyI2C"
		bool RdOnlyI2c( void )
		{
			char MsgBuf[100];

			//Psoc5100PowerUpForRegisterProcessing();				//lmc fixme

			sscanf (*pp_CommandString,"%X",&uiData);

			if( *p_CommandStringLength == 2 )
			{
				ucAdd = PSOC_I2C_ADDRESS;
				ucSub = (unsigned char)uiData;
			}
			else
			{
				ucAdd = PSOC_I2C_ADDRESS;
				ucSub = 0;
			}

			sprintf (MsgBuf, "Read Only -- Skip Setup of Address -- I2C address 0x%x (%d), subaddress 0x%x (%d)\r\n",ucAdd,ucAdd, ucSub,ucSub);
			xputstring (MsgBuf);

			bStatus = 0; // changed to be good, bStatus is good for now, bit fields set for each offending case below

			if (bStatus)
			{}
			else if(0)// (pI2C_Drvr->Read_Request(ucAdd+1)==FALSE)			//lmc fixme
			{
				bStatus |= 1<<3;//= FALSE;
			}
			else
			{
				//uiData = pI2C_Drvr->Get_Byte(TRUE);                   //lmc fixme
				ucData = (unsigned char)uiData;
			}

			//pI2C_Drvr->Generate_Stop();				//lmc fixme
			sprintf (MsgBuf, "Read 0x%02x, Status 0x%02x\r\n",ucData,bStatus);
			xputstring (MsgBuf);

			//Psoc5100PowerDownAfterRegisterProcessing();				//lmc fixme

			return true;
		}


		//Flash Psoc Firmware
		//Function for Non Menu Command:  "PSOCBURN"
		bool PsocBurn( void )
		{
			//Psoc5100FlashNewFirmware( true, false, false);			//lmc fixme
			PSOCBlankdownload=1;
			xputstring ("Enable Flash Blank PSoC.\r\n");
			return true;
		}


		//Download PSOC firmware
		//Function for Non Menu Command:  "PSoC5100Dnld"
		bool Psoc5100DnLd( void )
		{
			//Psoc5100SerialDownloadFirmware();                                                      //lmc   fixme

			return true;
		}


		//Verify PSOC firmware version
		//Function for Non Menu Command:  "PSOCVERIFY"
		bool PsocVerify( void )
		{
			//Psoc5100VerifyFirmwareVersion();                                                            //lmc   fixme

			return true;
		}


		//Write i2c incrementing data without register control, PSOC forced as device address
		//Function for Non Menu Command:  "WrI2C"
		bool WrI2c( void )
		{
			int i;
			char MsgBuf[100];

			if(0)//( GET_SETTING( EngineCommandPrivilege ) )                                         //lmc fixme       no menuing
			{

#if 0	// new way to go through the scan driver

				for (i=0; i<*p_CommandStringLength; i= i+2)
				{
					val[0] = *pp_CommandString[i];
					val[1] = *pp_CommandString[i+1];
					val[2] = 0;
					sscanf (val,"%x",&uiData);
					byteData[i] = (unsigned char)uiData;
					printf ("%x",byteData[i]);
				}

				// byteData array of bytes to write, no subaddress explicitly 'implied'
				if (!SCN_IOControl(nOpenCntxt, HHPSD_IOCTL_WRITE_PSOC, (PBYTE)byteData, (DWORD)*p_CommandStringLength/2,NULL, NULL, NULL))
					printf ("Write I2C Failed\r\n");
				else
					printf ("\r\n");

#else

				//Psoc5100PowerUpForRegisterProcessing();                          //lmc fixme

				//pI2C_Drvr->Generate_Start();                                     //lmc fixme
				//pI2C_Drvr->Send_Byte(PSOC_I2C_ADDRESS);                           //lmc fixme

				for (i=0; i<*p_CommandStringLength; i=i+2)
				{
					val[0] = *pp_CommandString[i];
					val[1] = *pp_CommandString[i+1];
					val[2] = 0;

					sscanf (val,"%x",&uiData);
					ucData = (unsigned char)uiData;
					//pI2C_Drvr->Send_Byte(ucData);                                   //lmc   fixme
					sprintf (MsgBuf,"%x",ucData);
					xputstring (MsgBuf);
				}

				//pI2C_Drvr->Generate_Stop();                                         //lmc   fixme
				//Psoc5100DelayInMilliSec(PSOC_CMD_DELAY);		// this is here since we can send save to eeprom raw       //lmc   fixme

				//Psoc5100PowerDownAfterRegisterProcessing();                                           //lmc   fixme

				xputstring ("\r\n");
#endif
			}

			return true;
		}


		// read i2c up to the offset implied in the form:
		// RdI2C<i2caddress><max offset>
		// RdI2C<i2caddress> 	~~ I2C address assumes
		// RdI2C 				~~ I2C address assumes Psoc, maxoffset will
		//Function for Non Menu Command:  "RdI2C"
		bool RdI2c( void )
		{
			int i;
			bStatus = TRUE; //for now
			sscanf (*pp_CommandString, "%X",&uiData);

			if( *p_CommandStringLength == 4 )
			{
				ucAdd = (unsigned char)(uiData>>8);
				ucSub = (unsigned char)(uiData);
			}
			else if(*p_CommandStringLength == 2 )
			{
				ucAdd = (unsigned char)(uiData);
				ucSub = 255;
			}
			else //if( *pp_CommandString ==0 )
			{
				ucAdd = PSOC_I2C_ADDRESS;
				ucSub = 255;
			}

#if 0		//Use scandriver
			if (!SCN_IOControl(nOpenCntxt, HHPSD_IOCTL_READ_PSOC_REG, (PBYTE )&ucAdd, 1,(PBYTE )byteData, (DWORD)ucSub,NULL))
				bStatus = FALSE; //Status = false;

#else		//Not using scan driver

			//Psoc5100PowerUpForRegisterProcessing();                                        //lmc fixme

			if(0)// (pI2C_Drvr->Read_Request(ucAdd+1)==FALSE)                                //lmc fixme
			{
				bStatus |= 1<<3;//= FALSE;
			}
#if 0    // investigation of i2c last write is that we actually read an extra byte every time, if I fix this in I2C class, this path cna be used
			else
			{
				for (i=0;i<ucSub;i++)
				{
					//if (i==(ucSub-2))
					//byteData[i] = pI2C_Drvr->Get_Byte_2(1);
					//else

					if (i==(ucSub-1))
					{
						//byteData[i] = pI2C_Drvr->Get_Byte_2(2);                                       //lmc   fixme
					}
					else
					{
						//byteData[i] = pI2C_Drvr->Get_Byte_2(0);                                       //lmc   fixme
					}
				}
			}
			//pI2C_Drvr->Generate_Stop();
#else // old way
			else
			{
				for (i=0;i<ucSub;i++)
				{
					//uiData = pI2C_Drvr->Get_Byte(FALSE);                                          //lmc fixme
					byteData[i]/*ucData*/ = (unsigned char)uiData;
				}
			}

			//uiData = pI2C_Drvr->Get_Byte(TRUE);                                                  //lmc fixme
			byteData[i]/*ucData*/ = (unsigned char)uiData;

			//pI2C_Drvr->Generate_Stop();                                                           //lmc fixme
#endif

			//Psoc5100PowerDownAfterRegisterProcessing();                                             //lmc fixme

#endif	//End Not using scan driver

			if (bStatus)
			{
				for (i=0;i<ucSub;i++)
				{
					printf ("Offset=%d, Data= %02x\r\n", i, byteData[i]);
				}
			}

			return true;
		}


		//Read I2c
		//Function for Non Menu Command:  "ReadI2C"
		bool ReadI2c( void )
		{
			char MsgBuf[100];

			//Psoc5100PowerUpForRegisterProcessing();                                   //lmc   fixme

			sscanf (*pp_CommandString,"%X",&uiData);

			if( *p_CommandStringLength == 4 )
			{
				ucAdd = (unsigned char)(uiData>>8);
				ucSub = (unsigned char)(uiData);
			}
			else if( *p_CommandStringLength == 2 )
			{
				ucAdd = PSOC_I2C_ADDRESS;
				ucSub = (unsigned char)uiData;
			}
			else
			{
				ucAdd = PSOC_I2C_ADDRESS;
				ucSub = 0;
			}

			sprintf (MsgBuf, "Read from I2C address 0x%x (%d), subaddress 0x%x (%d) of PSOC \r\n",ucAdd,ucAdd, ucSub,ucSub);
			xputstring (MsgBuf);

			bStatus = 0; // changed to be good, bStatus is good for now, bit fields set for each offending case below

			// switch to use bStatus = ReadIIC_Generic(ucAdd,ucSub);

			//pI2C_Drvr->Generate_Start();                                         //lmc fixme

			if(0)// (pI2C_Drvr->Send_Byte(ucAdd) == FALSE)                         //lmc fixme
			{
				bStatus |= 1;
			}
			else if(0)// (pI2C_Drvr->Send_Byte(ucSub) == FALSE)                    //lmc fixme
			{
				bStatus |= 1<<2;
			}

			//pI2C_Drvr->Generate_Stop();                                            //lmc fixme

			if (bStatus)
			{}
			else if(0)// (pI2C_Drvr->Read_Request(ucAdd+1)==FALSE)                  //lmc fixme
			{
				bStatus |= 1<<3;//= FALSE;
			}
			else
			{
				//uiData = pI2C_Drvr->Get_Byte(TRUE);                                 //lmc fixme
				ucData = (unsigned char)uiData;
			}

			//pI2C_Drvr->Generate_Stop();                                            //lmc fixme

			sprintf (MsgBuf, "Read 0x%02x, Status 0x%02x\r\n",ucData,bStatus);
			xputstring (MsgBuf);

			//Psoc5100PowerDownAfterRegisterProcessing();                             //lmc fixme

			return true;
		}


		//Write I2C
		//Function for Non Menu Command:  "WriteI2C"
		bool WriteI2c( void )
		{
			char MsgBuf[100];

			if(0)//( GET_SETTING( EngineCommandPrivilege ) )                                    //lmc fixme
			{
				//Psoc5100PowerUpForRegisterProcessing();                                        //lmc fixme

				sscanf( *pp_CommandString,"%x",&uiData );

				if( *p_CommandStringLength == 6 )
				{
					ucAdd = (unsigned char)(uiData>>16);
					ucSub = (unsigned char)(uiData>>8);
					ucData = (unsigned char)(uiData);
				}
				else if( *p_CommandStringLength == 4 )
				{
					ucAdd = PSOC_I2C_ADDRESS;
					ucSub = (unsigned char)(uiData>>8);
					ucData = (unsigned char)(uiData);
				}
				else
				{
					ucAdd = 0;
					ucSub = 0;
					ucData = 0;
				}

				bStatus = 0; // changed to be good.  bStatus is good for now, bit fields set for each offending case below

				sprintf (MsgBuf,"Writing to I2C add %02x, subaddress %02x, data %02x\r\n",ucAdd,ucSub,ucData);
				xputstring (MsgBuf);

				//pI2C_Drvr->Generate_Start();                                       //lmc fixme

				if(0)// (pI2C_Drvr->Send_Byte(ucAdd) == FALSE)                        //lmc fixme
				{
					bStatus |= 1; //= FALSE;
				}
				else if(0)// (pI2C_Drvr->Send_Byte(ucSub) == FALSE)                   //lmc fixme
				{
					bStatus |= 1<<2;//FALSE;
				}
				else if(0)// (pI2C_Drvr->Send_Byte(ucData) == FALSE)                  //lmc fixme
				{
					bStatus |= 1<<3;//FALSE;
				}

				//pI2C_Drvr->Generate_Stop();                                         //lmc fixme

				sprintf (MsgBuf, "bStatus 0x%02x\r\n",bStatus);
				xputstring (MsgBuf);

				//Psoc5100PowerDownAfterRegisterProcessing();                         //lmc fixme
			}

			return true;
		}


		// Serial Trigger
		//Function for Non Menu Command:  "TRIGGER"
		bool TriggerSer( void )
		{
			pTrigger->TriggerSerial();

			return true;
		}

		// Un Serial Trigger
	  //Function for Non Menu Command:  "UNTRIGGER"
		bool UnTriggerSer( void )
		{
			pTrigger->UnTriggerSerial();

			return true;
		}


		//*****************************************************************************************************************
		//*****************************************************************************************************************
		// Special commands for test floor
		/*****************************************************************************************************************
		 * This code is used by final test only.  It is used to measure led current.  Need to send  MANUFACTFORCESCANON
		 * then measure current.  Then send MANUFACTLEDCURRENTON then measure current again.  The difference between the
		 * two currents will be actual led current.  MANUFACTFORCESCANOFF will undo anything that the other functions have
		 * done. */

		//Force scanning on
		//Function for Non Menu Command:  "MANUFACTFORCESCANON"
		 bool ManufactForceScanOn( void )
		{
			if(0)//(GET_CONFIG_SETTING(iOpticsPopulation)==TWO_D) //Are we 2D??                   //lmc fixme
			{
				SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_START_SCANNING,NULL,0,NULL,0,NULL);
			}
			else
			{
				//theHal.Start_Scanning();	       // start scan                     //lmc fixme
			}

			return true;
		}


		//Set Current On
		//Function for Non Menu Command:  "MANUFACTLEDCURRENTON"
		bool ManufactLedCurrentOn( void )
		{
			int Is3800G = 0;

			//Is3800G = (CHal::WhatBoardAmI() == IT3800G_1_TOKEN);                              //lmc   fixme

			if(0)//(GET_CONFIG_SETTING(iOpticsPopulation)==TWO_D) //Are we 2D??                //lmc   fixme
			{
				//SetLights(true,100,true,100);                                                   //lmc   fixme
			}
			else
			{
				if(Is3800G)
				{
					//pDR_C->b.Bit7=0;					//	enable led's from MX1                         //lmc   fixme
				}
				else
				{
					//pDR_C->b.Bit7=1;					//	enable led's from MX1                         //lmc   fixme
				}
			}

			return true;
		}


		//Force scanning off
		//Function for Non Menu Command:  "MANUFACTFORCESCANOFF"
		bool ManufactForceScanOff( void )
		{
			int Is3800G = 0;

			//Is3800G = (CHal::WhatBoardAmI() == IT3800G_1_TOKEN);                       //lmc   fixme

			if(0)//(GET_CONFIG_SETTING(iOpticsPopulation)==TWO_D) //Are we 2D??               //lmc   fixme
			{
				//SetLights(false,0,false,0);                                                    //lmc   fixme
				SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_STOP_SCANNING,NULL,0,NULL,0,NULL);
				//pTrigger->SignalScanningFinished();                                             //lmc   fixme
			}
			else
			{
				if(Is3800G)
				{
					//pDR_C->b.Bit7=1;					//	enable led's from MX1                          //lmc   fixme
				}
				else
				{
					//pDR_C->b.Bit7=0;					//	enable led's from MX1                          //lmc   fixme
				}

				//theHal.Stop_Scanning();	       // stop scan                                     //lmc   fixme
			}

			return true;
		}


		//Passed
		//Function for Non Menu Command:  "PASSED"
		bool Passed( void )
		{
			int temp = atoi (*pp_CommandString);

			if(0 ==temp)
			{
				xputstring("\x06");

				while(1)
				{
					//theHal.GoodReadLedOn();				// else leds on                               //lmc   fixme
					MT_Delay(100);
					//theHal.GoodReadLedOff();				// else leds off                               //lmc   fixme
					MT_Delay(100);
				}
			}
			else if(1==temp)
			{
				xputstring("\x06");
				//theHal.GoodReadLedOn();				// else leds on                                    //lmc   fixme

				//while(!theHal.IsTriggerDebounced());                                                  //lmc   fixme

				//while(!theHal.IsTriggerPressed())                                                     //lmc   fixme
					RESET_WATCHDOG_TIMER();

				//theHal.GoodReadLedOff();				// else leds off                                    //lmc   fixme
				SoundStandardBeep(5);                                                                 //lmc   fixme

				char pucBuffer[256];
				HSTRING TempString(50,true);
				strcpy(&pucBuffer[strlen(pucBuffer)], "UUID:");
				TempString.Copy(GET_CONFIG_SETTING(UniqueUniversalIdentifier));
				TempString.UniCodeToSimpleAsciiFormat();
				strcat(&pucBuffer[strlen(pucBuffer)],(char *)TempString.Char);
				strcat(&pucBuffer[strlen(pucBuffer)], "\r");

				strcat(&pucBuffer[strlen(pucBuffer)],"SERNUM:");
				TempString.Copy(GET_CONFIG_SETTING(SerNum));
				TempString.UniCodeToSimpleAsciiFormat();
				strcat(&pucBuffer[strlen(pucBuffer)],(char *)TempString.Char);
				strcat(&pucBuffer[strlen(pucBuffer)], "\r");

				strcat(&pucBuffer[strlen(pucBuffer)], "SERCUS:");
				TempString.Copy(GET_CONFIG_SETTING(CustomSerialNumber));
				TempString.UniCodeToSimpleAsciiFormat();
				strcat(&pucBuffer[strlen(pucBuffer)],(char *)TempString.Char);
				strcat(&pucBuffer[strlen(pucBuffer)], "\r");

				strcat(&pucBuffer[strlen(pucBuffer)], "REV_WA:");
				strcat(&pucBuffer[strlen(pucBuffer)], GET_SETTING(ShowWARev) + 2);
				strcat(&pucBuffer[strlen(pucBuffer)], "\r");

				strcat(&pucBuffer[strlen(pucBuffer)], "FIMPNM:");
				TempString.Copy(GET_CONFIG_SETTING(FlashImagePartNumber));
				strcat(&pucBuffer[strlen(pucBuffer)],(char *)TempString.Char);
				strcat(&pucBuffer[strlen(pucBuffer)], "\r");

				xputstring(pucBuffer);
			}
			else
				xputstring("\x15");

			return true;
		}

		//*****************************************************************************************************************
		//*****************************************************************************************************************
		// End the Special commands for test floor
		//*****************************************************************************************************************
		//*****************************************************************************************************************



		//Dump image with CR
		//Function for Non Menu Command:  "DUMPIMAGEWITHCR"
		bool DumpImageWithCr( void )
		{
			int i;
			char MsgBuf[8];
			xputchar(0x0);
			xputchar(0x0);
			xputchar(0xe);
			xputchar(0x40);

			for(i=0;i<3645;i++)
			{
				sprintf((char *)MsgBuf,"%04d\r",(((int)P_SCANBUFFER[i])));
				xputstring(MsgBuf);
				wait_til_tx_is_empty();
			}

			return true;
		}


		// simulate a barcode read for translation tests
		//Function for Non Menu Command:  "TESTINJECT"
		bool TestInject( void )
		{
			// format: TESTINJECT,a]bcd,1234\r
			// 1234 = lenght of barcode (so we can even send a 0) (Datasource not included)
			// DataSourceInfo:
			// a = HHP ID
			// ]
			// b = AIM ID
			// c = AIM modifier
			// d = HHP modifier (ignored for now)
			// barcode is received here in this place to avoid size problems and allow for all characters
			// barcode = simulated barcode contents
			size_t BarcodeLength;
			const int DataSourceOffset = 1;
			const int HHPModifierOffset = 5;
			char dummy;

			if( 6 == sscanf( *pp_CommandString, ",%c%c%c%c%c,%d", &dummy, &dummy, &dummy, &dummy, &dummy, &BarcodeLength ) )
			{
				xputstring("\x06");	         // acknowledge it and wait for more data
				(*pp_CommandString)[HHPModifierOffset] = 0;  // ignore this becasue the rest of the code has trouble with it.
				size_t BufferLenght=BarcodeLength;
				bchar_t *pBuffer;
				CBarcodeData *pBarcode;
				pBarcode = CBarcodeData::GetSaveNewBuffer(pBuffer, BufferLenght);
				pBarcode->CopyIDsFromDataSourceInfo( *pp_CommandString + DataSourceOffset );
				int RestSize=BarcodeLength;
				int Received=0;
				while (RestSize > 0)
				{
					Received += theCurrentInterface.Read(pBuffer+Received, RestSize);
					RestSize = BarcodeLength - Received;
					RESET_WATCHDOG_TIMER();
				}
				pBarcode->FinalizeBuffer(Received);
				theScriptInterpreter.InjectSimulatedBarcode(pBarcode);                            //lmc   fixme
			}
			else
			{
				xputstring("\x15");
			}

			return true;
		}


#ifndef XENON_LITE_1500
		// snap image
		//Function for Non Menu Command:  "IMGSNP"
		bool ImgSnpWParam( void )
		{
			//xputstring(" GotSnap ");                                            //lmc   debug
			ImageSnapWithParameters( pp_CommandString, p_CommandStringLength );

			return true;
		}



		// ship image
		//Function for Non Menu Command:  "IMGSHP"
		bool ImgShpWParam( void )
		{
			//xputstring(" GotShip ");                                             //lmc   debug
			ImageShipSimple(1);

			return true;
		}
#endif
		#ifndef HI2D_T

		/* ******** e2V Jade - Sensor - I2C Routines ******** */
		#define Jade_IIC_ADDRESS 0x30		// base I2C address
		#define Jade_IIC_REGISTER_OFFSET_SWITCH_COUNT	0x7f		//were register size switches form one byte to two bytes
		#else
		#define Jade_IIC_ADDRESS 0xB8		// base I2C address for HI2D
		#define Jade_IIC_REGISTER_OFFSET_SWITCH_COUNT	0x100		//were register size switches form one byte to two bytes
		#endif
		//#define ENABLE_I2C_NON_MENU_DEBUG                      //define for debug of jade register writes and reads


		//Write jade imager
		//Function for Non Menu Command:  "IMGWRI"
		//format:  IMGWRI0XAABBBB
		//where '0X' means hex input, which is only form allowed, 'AA' is the register address in hex, 'BBBB' is data being sent in hex.
		//no return data
		bool ImgJadeWrite( void )
		{
			BOOL bStatus = TRUE;
			char cTheZero = 0;
			char cTheX = 0;
			unsigned int iTheRegister = 0;
			unsigned int iTheData;

			unsigned char subAddress = 0;
			unsigned short uiBuffer[4];
			unsigned int nCount = 1;

			//RAM_SET_ACTUAL(ImagerIdlePowerLevel,1);			//set power down to standby, do not allow to power down        //lmc   fixme

			if( g_pI2C_Drvr == NULL )		bStatus = FALSE;

			if( (*p_CommandStringLength >= 8) && bStatus )
			{
				sscanf( *pp_CommandString,"%c%c%2X%4X",&cTheZero,&cTheX,&iTheRegister,&iTheData );

#ifdef ENABLE_I2C_NON_MENU_DEBUG
				char MsgBuf[100];
				xputchar(cTheZero);
				xputchar(cTheX);
				sprintf( MsgBuf," %d %d %X %X ", iTheRegister, iTheData, iTheRegister, iTheData );
				xputstring( MsgBuf );
#endif
				subAddress = (unsigned char)(iTheRegister);
				uiBuffer[0] = (unsigned short)(iTheData);

				bStatus = g_pI2C_Drvr->Send_Data_W_Short_W_Dual_Reg_Size_I2C( Jade_IIC_ADDRESS, subAddress, uiBuffer, nCount, Jade_IIC_REGISTER_OFFSET_SWITCH_COUNT );

#ifdef ENABLE_I2C_NON_MENU_DEBUG
				if( bStatus ) theStdInterface.printf("\r\n WriteIIC pass, add 0X %x val 0X %x cnt %x \r\n",subAddress, uiBuffer[0],nCount);    //lmc   debug
				if( !bStatus ) theStdInterface.printf("\r\n WriteIIC fail, add 0X %x val %x cnt 0X %x \r\n",subAddress, uiBuffer[0],nCount);   //lmc   debug
#endif
			}

			return bStatus;
		}



#define RUN_COMMON_READ_THEN_WRITE_I2C_METHOD_FOR_COMPLETE_MUTEX	1

		//Read jade imager
		//Function for Non Menu Command:  "IMGRED"
		//format:  IMGWRI0XAA
		//where '0X' means hex input, which is only form allowed, 'AA' is the register address in hex.
		//data is returned on interface as "IMGRED0XBBBB", where 'BBBB' is the data read in hex.
		bool ImgJadeRead( void )
		{
			char MsgBuf[100];
			BOOL bStatus = TRUE;
			char cTheZero = 0;
			char cTheX = 0;
			unsigned int iTheRegister = 0;
			unsigned int iTheData;

			unsigned char subAddress = 0;
			unsigned short uiBuffer[4];
			unsigned int nCount = 1;

			RAM_SET_ACTUAL(ImagerIdlePowerLevel,1);		//set power down to standby, do not allow to power down

			if( g_pI2C_Drvr == NULL )		bStatus = FALSE;

			if( (*p_CommandStringLength >= 4) && bStatus )
			{
				sscanf( *pp_CommandString,"%c%c%2X",&cTheZero,&cTheX,&iTheRegister );

#ifdef ENABLE_I2C_NON_MENU_DEBUG
				xputchar(cTheZero);
				xputchar(cTheX);
				sprintf( MsgBuf," %d %X ", iTheRegister, iTheRegister );
				xputstring( MsgBuf );
#endif
				subAddress = (unsigned char)(iTheRegister);

#if RUN_COMMON_READ_THEN_WRITE_I2C_METHOD_FOR_COMPLETE_MUTEX

				bStatus = g_pI2C_Drvr->Write_Setup_Then_Receive_Data_W_Short_W_Dual_Reg_Size_I2C( (Jade_IIC_ADDRESS+1), subAddress, uiBuffer, nCount, Jade_IIC_REGISTER_OFFSET_SWITCH_COUNT );

				if( bStatus )
				{
					iTheData = (unsigned int)(uiBuffer[0]);
					sprintf( MsgBuf," 0408150X%04x ", iTheData );
					xputstring( MsgBuf );

	#ifdef ENABLE_I2C_NON_MENU_DEBUG
					xputstring("\r\n write setup of read address passed \r\n");        //lmc   remove debug
					theStdInterface.printf(" read of address 0X %x passed \r\n",subAddress);       //lmc   remove debug
				}
				else theStdInterface.printf(" read of address 0X %x failed \r\n",subAddress);               //lmc   remove debug
	#else
				}
	#endif
#else

				bStatus = g_pI2C_Drvr->Send_Addr_And_Sub_I2C( Jade_IIC_ADDRESS, subAddress, TRUE, FALSE );

				if( bStatus )
				{
					bStatus = g_pI2C_Drvr->Receive_Data_W_Short_W_Dual_Reg_Size_I2C( (Jade_IIC_ADDRESS+1), subAddress, uiBuffer, nCount, Jade_IIC_REGISTER_OFFSET_SWITCH_COUNT );

					iTheData = (unsigned int)(uiBuffer[0]);
					sprintf( MsgBuf," 0408150X%04x ", iTheData );
					xputstring( MsgBuf );

	#ifdef ENABLE_I2C_NON_MENU_DEBUG
					xputstring("\r\n write of address passed \r\n");        //lmc   remove debug

					if( bStatus ) theStdInterface.printf(" read of address 0X %x passed \r\n",subAddress);       //lmc   remove debug
					else theStdInterface.printf(" read of address 0X %x failed \r\n",subAddress);               //lmc   remove debug
				}
				else theStdInterface.printf(" write of address 0X %x failed \r\n",subAddress);                //lmc   remove debug
	#else
				}
	#endif

#endif //end if RUN_COMMON_READ_THEN_WRITE_I2C_METHOD_FOR_COMPLETE_MUTEX
			}

			return bStatus;
		}



#else	//if defined RFBASE
		bool DumpMt9( void ){return false;}
		bool RdOnlyI2c( void ){return false;}
		bool PsocBurn( void ){return false;}
		bool Psoc5100DnLd( void ){return false;}
		bool PsocVerify( void ){return false;}
		bool WrI2c( void ){return false;}
		bool RdI2c( void ){return false;}
		bool ReadI2c( void ){return false;}
		bool WriteI2c( void ){return false;}
		bool TriggerSer( void ){return false;}
		bool UnTriggerSer( void ){return false;}
		bool ManufactForceScanOn( void ){return false;}
		bool ManufactLedCurrentOn( void ){return false;}
		bool ManufactForceScanOff( void ){return false;}
	    bool Passed( void ){return false;}
		bool DumpImageWithCr( void ){return false;}
		bool ImgSnpWParam( void ){return false;}
		bool ImgShpWParam( void ){return false;}


		// simulate a barcode read for translation tests
		//Function for Non Menu Command:  "TESTINJECT"
		bool TestInject( void )
		{
			// format: TESTINJECT,a]bcd,1234\r
			// 1234 = lenght of barcode (so we can even send a 0) (Datasource not included)
			// DataSourceInfo:
			// a = HHP ID
			// ]
			// b = AIM ID
			// c = AIM modifier
			// d = HHP modifier (ignored for now)
			// barcode is received here in this place to avoid size problems and allow for all characters
			// barcode = simulated barcode contents
			size_t BarcodeLength;
			const int DataSourceOffset = 1;
			const int HHPModifierOffset = 5;
			char dummy;
			unsigned char bcodechar;
			HSTRING barcodestring(20,true);

			if( 6 == sscanf( *pp_CommandString, ",%c%c%c%c%c,%d", &dummy, &dummy, &dummy, &dummy, &dummy, &BarcodeLength ) )
			{
				xputstring("\x06");	         // acknowledge it and wait for more data
		
				(*pp_CommandString)[HHPModifierOffset] = '0';  // ignore this becasue the rest of the code has trouble with it. and yes this needs to be
															// '0' not 0 like scanner implementation of this...might not even need this line but I was not 
															// brave enough to take out.
				int Received=0;
				char scannercommand[64];
				sprintf(scannercommand,"\x02N,TESTINJECT,%c%c%c%c%c,%d\r", (*pp_CommandString)[DataSourceOffset],
				        (*pp_CommandString)[DataSourceOffset+1],
				        (*pp_CommandString)[DataSourceOffset+2],
				        (*pp_CommandString)[DataSourceOffset+3],
				        (*pp_CommandString)[DataSourceOffset+4],
				        BarcodeLength);
				barcodestring.Copy(scannercommand);

				while (Received <BarcodeLength )
				{
					if (theStdInterface.Read(&bcodechar,1))
					{
						barcodestring.Append(&bcodechar,1);
						Received ++;
					}
					RESET_WATCHDOG_TIMER();
				}

				for ( int i=1;i<=MAX_SPP_PORTS;i++)
				{
					if(theSPP_Ports(i).IsPortAvailable())
					{
						theSPP_Ports(i).Write(barcodestring);
					}
				}
			}
			else
			{
				xputstring("\x15");
			}

			return true;
		}

#endif	// #ifndef RFBASE



//****************************************************************************************************************************
#ifdef RFBASE

#else
		bool Adr2Scn( void ){return false;}
		bool RfDlTst( void ){return false;}
#endif



//****************************************************************************************************************************
#if !defined( RFBASE ) && !defined( RFSCAN )

		// Force a decode of the current image
		//Function for Non Menu Command:  "DECODE"
		bool DecodeCurImg( void )
		{
			DecodeCurrentImage();

			return true;
		}

		// Force a decode of the current image
		//Function for Non Menu Command:  "REDECODE"
		bool ReDecode( void )
		{
			RedecodeSavedImage();

			return true;
		}

#else
#endif		//end !defined( RFBASE ) && !defined( RFSCAN )



//****************************************************************************************************************************
#if defined(RFBASE) || defined(RFSCAN)
#else
		bool Bat_Lvl( void ){return false;}
		bool Sys_LVL( void ){return false;}
		bool RadRst( void ){return false;}
		bool RadioSetReset( void ){return false;}
		bool Bat_Temp( void ){return false;}


#endif		//end defined(RFBASE) || defined(RFSCAN)



//****************************************************************************************************************************
#if defined( DEVELOPMENT_SETTINGS )

		// Test flash.
		//Function for Non Menu Command:  "TESTFLASHFTLEASSTH"
		bool TestFlashStatus( void )
		{
			char	ResultString[160];

			int Status = test_flash();
			sprintf(ResultString, "TESTFLASH status = %d\r\n", Status);
			xputstring(ResultString);

			return true;
		}


		// Erase flash disk.
		//Function for Non Menu Command:  "EFD"
		bool EraseFlashDisk( void )
		{
			if(0)//( (GET_SETTING(UltimatePrivilegeLevel) & FLASH_TEST_PRIVILEGE) || (GET_SETTING(FlashTestPrivilege)) )      //lmc   fixme
			{
				//program_flash((BYTE *) FLASH_DISK_BASE, ((BYTE *) (&RAM_RESET)), FLASH_DISK_SECTOR_SIZE, NORMAL_MODE, false);           //lmc   fixme
				//program_flash((BYTE *) FLASH_DEFAULT_CONFIGURATION_BASE, ((BYTE *) (&RAM_RESET)), FLASH_MAX_CONFIGURATION_LENGTH, NORMAL_MODE, false);     //lmc   fixme
			}

			return true;
		}


		// Erase all flash but boot code.
		//Function for Non Menu Command:  "EFL"
		bool EraseAllFlash( void )
		{
			if(0)//( (GET_SETTING(UltimatePrivilegeLevel) & FLASH_TEST_PRIVILEGE) || (GET_SETTING(FlashTestPrivilege)) )       //lmc   fixme
			{
				UINT	PatternByte = 0;

				if( *p_CommandStringLength > 0 )
				{
					sscanf (*pp_CommandString, "%X", &PatternByte);		/* parse the pattern byte */
				}

				BYTE	*ErasePattern;
				ErasePattern = new BYTE[FLASH_SIZE];

				for(i = 0;    i < FLASH_SIZE;    ++i)
				{
					ErasePattern[i] = (BYTE) PatternByte;
				}

				program_flash((BYTE *) (FLASH_BOOT_CODE_BASE + FLASH_MAX_BOOT_CODE_LENGTH), ErasePattern,
			              	(ULONG) FLASH_SIZE - FLASH_MAX_BOOT_CODE_LENGTH, MANUFACTURING_CONFIGURATION_MODE, false);

				delete ErasePattern;
			}

			return true;
		}

#else
		 bool TestFlashStatus( void ){return false;}
		 bool EraseFlashDisk( void ){return false;}
		 bool EraseAllFlash( void )
		 {
		 return EraseEntireFlash()==0;
		 }
#endif		//end DEVELOPMENT_SETTINGS



//****************************************************************************************************************************
#ifdef TESTFUNCS

		// Replacement for PSC ZF11 to reset PSOC.
		//Function for Non Menu Command:  "ZF11"
		bool Zf11( void )
		{
			TestLoop();

			return true;
		}

#else
		bool Zf11( void ){return false;}
#endif



		// Replacement for PSC ZF10 to reset PSOC.
		//Function for Non Menu Command:  "ZF10"   // note these have to be tested before zf1
		bool Zf10( void )
		{
			xputstring("flashing psoc\r\n");


			//theHal.flash_psoc();                                               //lmc   fixme


			return true;
		}


		//ZF12
		//Function for Non Menu Command:  "ZF12"		// note these have to be tested before zf1
		bool Zf12( void )
		{
			xputstring("erasing and flashing psoc\r\n");


			//theHal.ErasePsocFlash();                                       //lmc   fixme

			//	theHal.flash_psoc();


			return true;
		}


		// Replacement for PSC ZF1 to test good read beep sequence.
		//Function for Non Menu Command:  "ZF1"
		bool Zf1( void )
		{
			SoundGoodReadBeep();	
			return true;
		}


		// Replacement for PSC ZF3 to test standard beep.
		//Function for Non Menu Command:  "ZF3"
		bool Zf3( void )
		{
			SoundStandardBeep(4);;
			return true;
		}


		// Replacement for PSC ZY to send KIM data to the host
		//Function for Non Menu Command:  "ZY"
		bool Zy( void )
		{
			#ifndef RFBASE
				SendKimData();
			#endif

			return true;
		}


		// Replacement for PSC ZZ to send grayscale data to the host
		//Function for Non Menu Command:  "ZZ"
		bool Zz( void )
		{
			#ifndef RFBASE
				SendImageData();
			#endif

			return true;
		}


		// Replacement for PSC ZZ to send grayscale data to the host for Acuity
		//Function for Non Menu Command:  "ZQ"
		bool Zq( void )
		{
			#ifndef RFBASE
				SendImageDataSynFe();
			#endif

			return true;
		}


		// IMGSNP substitute
		//Function for Non Menu Command:  "ZP", takes a decoder type image.
		bool Zp( void )
		{
			#ifndef RFBASE
				Take1dImage();
				SendImageDataSynFe();
			#else // addcode here to forward this command to scanner
			#endif

			return true;
		}


		// Virginias spreadsheet format to send grayscale data to the host
		//Function for Non Menu Command:  "VV"
		bool Vv( void )
		{
			#ifndef RFBASE
				SendImageData_Delimited();
			#endif

			return true;
		}


		// Average white value skipping CAL code for VRF
		//Function for Non Menu Command:  "LG"
		bool Lg( void )
		{
			#ifndef RFBASE
				SendImageData_AVG_CAL();
			#endif

			return true;
		}


		// Do good read indicator sequence
		//Function for Non Menu Command:  "BEPGRX"
		bool BepGrx( void )
		{

			if( (*p_CommandStringLength==0) || (*pp_CommandString[0]=='1') )
			{
				SoundGoodReadBeep();                                                                       //lmc   fixme
			}

			return true;
		}
		
		// Handle processing a sequenced ACK (used in batch mode)
		bool NMSeqAck( void )
		{
			return true;
		}


//****************************************************************************************************************************
//****************************************************************************************************************************
//End Methods for non menu commands
//****************************************************************************************************************************
//****************************************************************************************************************************




//****************************************************************************************************************************
//****************************************************************************************************************************
//
//Functional Processing Methods
//
//****************************************************************************************************************************
//****************************************************************************************************************************



void ForceScanDriverOff(void)
{
#ifndef RFBASE
	StopImaging();
	SetLights(0, -1, 0, -1);  // Jira NG2D-916:  if the Scan Illumination value doesn't match the last set value, we will write the PSOC even though the Illumination is off and we're trying to turn it off.
#endif
}



#ifndef RFBASE
// Ported from db_cmds.c
// *----------------------------------------------------------------------*
// * ship_nyb - does KIM format compression on a nibble					  *
// *----------------------------------------------------------------------*
void ship_nyb(int value, int *outchr)
{
	*outchr = ((*outchr)<<4) + (value&0x0f);
	if (*outchr > 255)
	{
		xputchar((unsigned char)(*outchr&0xff));
		*outchr = 1;
	}
}
// Ported from db_cmds.c
void ship_kim_img(UCHAR *img, int x0, int y0, int x1, int y1, int skip, int topx)
{
	int x, y, yy, pixel, last, outchr;
	char s[55];

	sprintf(s,"]!!)8bT(%04d%04d",(x1-x0+skip-1)/skip,(y1-y0+skip-1)/skip);
	xputstring(s);
	sprintf(s,"%04d%04d%04d%04d%04d",x0,y0,x1,y1,skip);
	xputstring(s);

	last = -10;
	outchr = 1;
	for (y=y0; y<y1; y+=skip)
	{
		yy = y * topx;
		for (x=x0; x<x1; x+=skip)
		{
			pixel = img[x+yy];
			if ((pixel > (last-8)) && (pixel < (last+8)))
			{
				ship_nyb(pixel-last,&outchr);
			}
			else
			{
				ship_nyb(8,&outchr);
				ship_nyb(pixel>>4,&outchr);
				ship_nyb(pixel,&outchr);
			}
			last = pixel;
		}
	}
	if (outchr != 1)
		ship_nyb(8,&outchr);
	xputchar(CR);
	xputchar(LF);
}



// Ported from db_cmds.c
// ZY
void SendKimData(void)
{
	ship_kim_img(P_SCANBUFFER,0,0,640,480,1,640);
	return;
}

// Ported from db_cmds.c
// ZZ
void SendImageData(void)
{
	int i, value;
	char MsgBuf[8];

	xputchar(0x0);
	xputchar(0x0);
	xputchar(0xe);
	xputchar(0x3d);

	for(i=0; i<3645; i++)
	{
		value = (int)P_SCANBUFFER[i];
		sprintf((char *)MsgBuf,"%04d",value);
		xputstring(MsgBuf);
		wait_til_tx_is_empty();
	}

	return;
}

// ZQ as syn fe msg
void SendImageDataSynFe(void)
{
	char MsgBuf[17];
	int Count = 3645;
	int size = Count+4+6;

	MsgBuf[0] = 0x02;
	MsgBuf[1] = 0xfe;
	MsgBuf[2] = size&0xff;
	MsgBuf[3] = (size>>8)&0xff;
	MsgBuf[4] = (size>>16)&0xff;
	MsgBuf[5] = (size>>24)&0xff;
	MsgBuf[6] = '\r';
	MsgBuf[7] = 'I';
	MsgBuf[8] = 'M';
	MsgBuf[9] = 'G';
	MsgBuf[10] = 'L';
	MsgBuf[11] = 'I';
	MsgBuf[12] = 'N';
	MsgBuf[13] = (Count>>24)&0xff;
	MsgBuf[14] = (Count>>16)&0xff;
	MsgBuf[15] = (Count>>8)&0xff;
	MsgBuf[16] = Count&0xff;
	theStdInterface.Write(MsgBuf,17);
	theStdInterface.Write(P_SCANBUFFER,Count);
	return;
}

//VV for Virginia - delimited and no header
void SendImageData_Delimited(void)
{
	int i;
	char MsgBuf[8];

	for(i=0; i<3645; i++)
	{
		sprintf((char *)MsgBuf,"%04d\r",(((int)P_SCANBUFFER[i])));
		xputstring(MsgBuf);
		wait_til_tx_is_empty();
	}

	return;
}



bool CheckSpikes_D_CAL(int pos)
{
	#define SPIKE 6
	#define SPIKE_WIDTH 12 //support ssb-width of 12 pixels
	bool bPassFail=true;

	if ((P_SCANBUFFER[pos+SPIKE_WIDTH]-P_SCANBUFFER[pos]) && (P_SCANBUFFER[pos+SPIKE_WIDTH]>(P_SCANBUFFER[pos]+SPIKE )))
		bPassFail = false;
	else if (P_SCANBUFFER[pos]-P_SCANBUFFER[pos+SPIKE_WIDTH] && ((P_SCANBUFFER[pos+SPIKE_WIDTH])+SPIKE<(P_SCANBUFFER[pos])))
		bPassFail = false;


	return bPassFail;
}

//LG - delimited, no header, ships pixels around CAL target
void SendImageData_AVG_CAL(void)
{
	int i, iSum;
	bool bPassFail;
	char MsgBuf[15];
	iSum=0;
	bPassFail=true;

	for(i=400; i<1000; i++)
	{
		iSum=iSum+P_SCANBUFFER[i];

		if (CheckSpikes_D_CAL(i)==false)
			bPassFail = false;
	}

	for(i=2648; i<3248; i++)
	{
		iSum=iSum+P_SCANBUFFER[i];

		if (CheckSpikes_D_CAL(i)==false)
			bPassFail = false;
	}

	iSum = iSum / 1200;

	if (bPassFail)
		sprintf((char *)MsgBuf,"\x06%03d\r",iSum);
	else
		sprintf((char *)MsgBuf,"\x15%03d\r",iSum);

	xputstring(MsgBuf);
	wait_til_tx_is_empty();

	return;
}

#endif //RFBASE


/*                                            //lmc fixme - currently not used
void Delay(int dely)
{
	static volatile int tempint=1;
	volatile int del_cnt;

	for (del_cnt=0; del_cnt<dely; del_cnt++)
	{
		tempint = tempint*4;
	}
}
*/                                            //lmc fixme


//
//	CheckForNonMenuCommand
//		Check the command string for the presence of the non-menu command sent by the caller.
//		If present, adjust the command string pointer and length past the non-menu command
//		string.  The comparison is done in a case sensitive manner.
//
/*                                                                                                  //lmc fixme - not currently used
bool CheckForNonMenuCommand(char **CommandString, UINT *CommandStringLength,
                            const char *CommandToCheckFor)
{
	UINT	LengthOfCommandToCheckFor;
	UINT	i = 0;
	char	Char1, Char2;				// Next characters to be compared, which will be converted to upper case if necessary.
	bool	CompareStatus = true;		// Assume a good compare until we know otherwise.


	LengthOfCommandToCheckFor = strlen(CommandToCheckFor);

	// If the command to check for is longer than the command string, then the check operation fails.
	if(LengthOfCommandToCheckFor <= *CommandStringLength)
	{
		// If we find the non-menu command in the command string, adjust the command string pointer
		// and length.
		i = 0;
		while((i < LengthOfCommandToCheckFor) && (CompareStatus))
		{
			Char1 = (*CommandString)[i];
			Char2 = CommandToCheckFor[i];
			if((Char1 >= 'a') && (Char1 <= 'z'))
				Char1 = ((char) (Char1 & 0xDF));
			if((Char2 >= 'a') && (Char2 <= 'z'))
				Char2 = ((char) (Char2 & 0xDF));
			if(Char1 == Char2)
			{
				++i;
			}
			else
			{
				CompareStatus = false;
			}
		}
		if(CompareStatus)
		{
			*CommandString += LengthOfCommandToCheckFor;
			*CommandStringLength -= LengthOfCommandToCheckFor;
		}
	}
	else
	{
		CompareStatus = false;
	}
	return(CompareStatus);
} // CheckForNonMenuCommand
*/                                                             //lmc fixme


//****************************************************************************************************************************
//****************************************************************************************************************************
//
//Main non menu command Processing Method
//
//****************************************************************************************************************************
//****************************************************************************************************************************



//
//	ProcessNonMenuCommands
//
//		Format:		<SYN>N,cmd_string<CR>
//
//		We've received a <SYN>Ncmd<CR> to indicate that we're supposed to process a
//		non-menu system command.  The following is a list of non-menu commands:
//
//			DEVICETYPE
//				Return the device type flag bytes in the beginning of the code image.
//			NEWAPP
//				Start downloading new application code to the reader.
//			NEWBOOT
//				Start downloading new boot code to the reader.  Note that SETBOOTCODEDOWNLOADMODE
//				must have been sent first.
//			SETBOOTCODEDOWNLOADMODE
//				Enables boot code download mode so that the next code download may be boot code.
//
void ProcessNonMenuCommands(char *CommandString, UINT CommandStringLength)
{
	char	*TerminalID;
	int		TerminalIDLength;
	bool	Success = true;		// We haven't failed yet.

	// If the command is too long, WOOF!
	if(CommandStringLength > MAXIMUM_SIZE_OF_NON_MENU_COMMAND)
	{
		xputstring("\025");
	}

	// Get rid of the N.
	if(CommandStringLength > 1)
	{
		CommandString += 1;
		CommandStringLength -= 1;
	}
	else
	{
		// We don't even have enough characters for a command; WOOF!
		xputstring("\025");
	}

	// Check to see if we received a terminal ID.
	TerminalID = CommandString;
	TerminalIDLength = 0;

	while((TerminalIDLength < CommandStringLength) &&
	        (TerminalID[TerminalIDLength] != ','))
	{
		++TerminalIDLength;
	}

	// We'd better find a comma to mark the end of the terminal ID (even if there was none).
	if((TerminalIDLength < CommandStringLength) &&
	        (CommandString[TerminalIDLength] == ','))
	{
		// We have a valid terminal ID;  move past it!
		CommandString += TerminalIDLength + 1;
		CommandStringLength -= (TerminalIDLength + 1);
	}
	else
	{
		Success = false;
	}

	// If this is an RFBASE or RFSCAN, check to see if there is a device name and if there is, send the command to that device.  The only valid device
	// name for RFSCAN builds is the wildcard.


	// Process the rest of the command if we haven't experienced an error.
	if(Success)
	{
		// If we're in skip header check mode, decrement the counter.  We set it at two when instructed to do so by the host via
		// the "SKIPHEADERCHECK" command.  If the command immediately following is "NEWAPP", then the counter will still be
		// non-zero.  The intent is to force a "NEWAPP" to immediately follow "SKIPHEADERCHECK" for safety reasons (in other
		// words, the system will never be inadvertently left in skip header check mode).
		if(SkipHeaderCheckOnNextDownloadMode > 0)
		{
			--SkipHeaderCheckOnNextDownloadMode;
		}

		// If we're in skip boot code header check mode, decrement the counter.  We set it at three when instructed to do so by the host via
		// the "SKIPBOOTCODEHEADERCHECK" command.  If the commands immediately following are "SETBOOTCODEDOWNLOADMODE" and "NEWBOOT", then
		// the counter will still be non-zero.  The intent is to force a "SETBOOTCODEDOWNLOADMODE" to immediately follow "SKIPBOOTCODEHEADERCHECK"
		// for safety reasons (in other words, the system will never be inadvertently left in skip boot code header check mode).
		if(SkipBootCodeHeaderCheckOnNextBootCodeDownloadMode > 0)
		{
			--SkipBootCodeHeaderCheckOnNextBootCodeDownloadMode;
		}

		// If we're in boot code download mode, decrement the counter.  We set it at two when instructed to do so by the host via
		// the "SETBOOTCODEDOWNLOADMODE" command.  If the command immediately following is "NEWBOOT", then the counter will still be
		// non-zero.  The intent is to force a "NEWBOOT" to immediately follow "SETBOOTCODEDOWNLOADMODE" for safety reasons (in other
		// words, the system will never be inadvertently left in boot download mode).
		if(BootCodeDownloadMode > 0)
		{
			--BootCodeDownloadMode;
		}

//****************************************************************************************************************************
//Find and run non menu command
//****************************************************************************************************************************
		MenuCommandFunction_t pMenuCommandFunctionPointer;

		//set globals for table functions that need them
		pp_CommandString = &CommandString;

		p_CommandStringLength = &CommandStringLength;

		//call function to find pointer to function and call
		pMenuCommandFunctionPointer = FindMenuCommand( &CommandString, &CommandStringLength );

		if( pMenuCommandFunctionPointer != NULL )   //if found command , run it
		{
			pMenuCommandFunctionPointer();
		}

		// Unrecognized non-menu command; WOOF!!
		else
		{
			xputstring("\025");
		}
	}

	return;
} // ProcessNonMenuCommands



//
//	FindNonMenuCommandMethod
//		Check the command string for the presence of the non-menu command sent by the caller.
//		If a valid one is present, adjust the command string pointer and length past the non-menu command
//		string.  The comparison is done in a case sensitive manner.
//returns:
//		The new char pointer is returned on success, Null on failure
//modifies:
//		The passed in length is reduced by the size of the menu command if it is found.
//
char * FindNonMenuCommandMethod( char *pCommandString, unsigned int *pCommandStringLength, char *pCommandToCheckFor )
{
	unsigned int	LengthOfCommandToCheckFor;
	char * pNewCommandString;

	pNewCommandString = NULL;			//if string not found, return a NULL

	LengthOfCommandToCheckFor = strlen( pCommandToCheckFor );		//Find length of the string holding the command to look for

	// If the command to check for is longer than the command string, then the check operation fails.
	if(LengthOfCommandToCheckFor <= *pCommandStringLength)
	{
		// If we find the non-menu command in the command string, adjust the new command string pointer
		// and passed in length.
		if( strncmp( pCommandString, pCommandToCheckFor, LengthOfCommandToCheckFor ) == 0 )
		{
			pNewCommandString = pCommandString + LengthOfCommandToCheckFor;       //set new pointer to location after command
			*pCommandStringLength -= LengthOfCommandToCheckFor;                    //reduce passed string length by size of command
		}
	}

	return( pNewCommandString );
} // FindNonMenuCommandMethod



/* FindMenuCommand() */
/* The following function searches the Command Table for the	         */
   /* specified Command passed in by CommandString.  If the Command		*/
	/* is found, this function returns 												*/
   /* a NON-NULL Command Function Pointer.  If the command is not found */
   /* this function returns NULL.                                       */
MenuCommandFunction_t FindMenuCommand( char **ppCommandString, UINT *pCommandStringLength )
{
	char * pTableCommand;
	char * pFoundTableCommand;
   unsigned int uiIndex;
   MenuCommandFunction_t pMenuCommandFunctionPointer;
	unsigned int	LengthOfTableCommandToCheckFor;
	bool bDone = false;

   /* First, make sure that the command pointer specified is semi-valid.        */
   if( *ppCommandString )
   {
      /* Now loop through each element in the table to see if there is  */
      /* a match.                                                       */
      for( uiIndex=0; !bDone; uiIndex++ )
      {
			pMenuCommandFunctionPointer = NULL;
			pFoundTableCommand = NULL;

			pTableCommand = MenuCommandTable[uiIndex].pMenuCommandName;           //get command name from table
			LengthOfTableCommandToCheckFor = strlen( pTableCommand );		//Find length of the string holding the command to look for

			if( strncmp( "EndMethod", pTableCommand, LengthOfTableCommandToCheckFor ) == 0 )     //if end of table, we are finished
			{
				bDone = true;
			}
			else
			{
				pFoundTableCommand = FindNonMenuCommandMethod( *ppCommandString, pCommandStringLength, pTableCommand );   //look for match, passed length will be modified on success
			}

			if( (pFoundTableCommand) != NULL )           //found a match
			{
            pMenuCommandFunctionPointer = MenuCommandTable[uiIndex].MenuCommandFunction;    //set return value
				*ppCommandString = pFoundTableCommand;			//found, so set pointer to new location after command
				bDone = true;
			}
      }		//end for()
   }
   else
	{
      pMenuCommandFunctionPointer = NULL;
	}

   return( pMenuCommandFunctionPointer );
}

#ifndef RFBASE
bool ImgStartStopTest(void)
{
	CTimeoutmS	waitDelay(500);          //500 ms
	int iLightsOn = 0;

	//Variables for image snap with default settings
	DWORD FrameRate = 30;
	DWORD ExpoValue = 200;	             //200     //7874;;
	//DWORD TargetWhiteValue = 125;
	DWORD GainValue = 1;

	//int AcceptanceDelta = 25;

	int NumberOfTries = 6;
	//int TargetSetPoint = 50;
	int ConformMe = 0;

	DWORD DataToSend[12];
	DWORD ModeToSend;

	ModeToSend = HHPSD_EXPOSURE_FIXED;
	//ModeToSend = HHPSD_AUTOEXPOSURE_USE_HHP;

	//---- things are sent in groups of 2s, the first is the tag, and the second is the value----
	DataToSend[0] = ES_FIXED_FRAME_RATE;
	DataToSend[1] = FrameRate;
	DataToSend[2] = ES_FIXED_EXP;
	DataToSend[3] = ExpoValue;
	DataToSend[4] = ES_FIXED_GAIN;
	DataToSend[5] = GainValue;
	DataToSend[6] = ES_FRAME_RATE;
	DataToSend[7] = FrameRate;
	DataToSend[8] = ES_CONFORM_IMAGE;
	DataToSend[9] = ConformMe;
	DataToSend[10] = ES_CONFORM_TRIES;
	DataToSend[11] = NumberOfTries;

	//MT_Delay(500);

	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_MODE,(PBYTE)&ModeToSend,1*sizeof(DWORD),NULL,0,NULL);
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)DataToSend,sizeof(DataToSend),NULL,0,NULL);
	//SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_START_SCANNING,NULL,0,NULL,0,NULL);
	//SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_NEW_SCAN,(UCHAR *)&P_SCANBUFFER,0,NULL,0,NULL);
	int loopCount = 100;

	while(loopCount>0)
	{
		RESET_WATCHDOG_TIMER();

		if( (!waitDelay.HasExpired()) || (loopCount == 0) )
		{
			//xputstring("LP\r\n");
			//wait_til_tx_is_empty();
			MT_Delay(50);
			ReSynchWorkingAndActive();
			ProcessInterfaceData();
			//Main Loop TEST
		}
		else
		{
		 	//waitDelay.SetExpiration(500);

			if( iLightsOn == 0 )             //turn on
			{
				SetLights( 1, 100, 1, 100 );
				ExpoValue = GET_SETTING(ImageSnapExposure);
				DataToSend[3] = ExpoValue;
				xputstring("SS\r\n");
				wait_til_tx_is_empty();
				SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_MODE,(PBYTE)&ModeToSend,1*sizeof(DWORD),NULL,0,NULL);
				SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)DataToSend,sizeof(DataToSend),NULL,0,NULL);
				SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_START_SCANNING,NULL,0,NULL,0,NULL);
				//SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_NEW_SCAN,(UCHAR *)&P_SCANBUFFER,0,NULL,0,NULL);
				//SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SINGLE_FRAME_CAPTURE,(UCHAR *) &P_SCANBUFFER,0,NULL,0,NULL);
				//MT_Delay(200);
				iLightsOn = 1;
			}
			else
			{
				SetLights( 0, 100, 0, 100 );
				iLightsOn = 0;
				//MT_Delay(200);
				SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_STOP_SCANNING,NULL,0,NULL,0,NULL);
				xputstring("1234\r\n");
				wait_til_tx_is_empty();
				loopCount--;
			}

			waitDelay.SetExpiration(500);
		}
	}
	return true;
}

bool ImgStartScannningTest(void)
{

	//Variables for image snap with default settings
	DWORD FrameRate = 30;
	DWORD ExpoValue = 200;	             //200     //7874;;
	//DWORD TargetWhiteValue = 125;
	DWORD GainValue = 1;

	//int AcceptanceDelta = 25;

	int NumberOfTries = 6;
	//int TargetSetPoint = 50;
	int ConformMe = 1;

	SetLights( 1, 100, 1, 100 );

	DWORD DataToSend[12];
	DWORD ModeToSend;

	//ModeToSend = HHPSD_EXPOSURE_FIXED;
	ModeToSend = HHPSD_AUTOEXPOSURE_USE_HHP;

	//---- things are sent in groups of 2s, the first is the tag, and the second is the value----
	DataToSend[0] = ES_FIXED_FRAME_RATE;
	DataToSend[1] = FrameRate;
	DataToSend[2] = ES_FIXED_EXP;
	DataToSend[3] = ExpoValue;
	DataToSend[4] = ES_FIXED_GAIN;
	DataToSend[5] = GainValue;
	DataToSend[6] = ES_FRAME_RATE;
	DataToSend[7] = FrameRate;
	DataToSend[8] = ES_CONFORM_IMAGE;
	DataToSend[9] = ConformMe;
	DataToSend[10] = ES_CONFORM_TRIES;
	DataToSend[11] = NumberOfTries;
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_MODE,(PBYTE)&ModeToSend,1*sizeof(DWORD),NULL,0,NULL);
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)DataToSend,sizeof(DataToSend),NULL,0,NULL);
	ExpoValue = GET_SETTING(ImageSnapExposure);
	DataToSend[3] = ExpoValue;
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_MODE,(PBYTE)&ModeToSend,1*sizeof(DWORD),NULL,0,NULL);
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_SET_EXPOSURE_SETTINGS,(PBYTE)DataToSend,sizeof(DataToSend),NULL,0,NULL);
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_START_SCANNING,NULL,0,NULL,0,NULL);
	return true;
}


bool ImgStopScannningTest(void)
{
	SetLights( 0, 100, 0, 100 );
	SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_STOP_SCANNING,NULL,0,NULL,0,NULL);
	return true;
}

#ifndef RFBASE  //#if defined(DIRT_TEST)
bool ImgSmearTest(void)
{
	if(P_SCANBUFFER != NULL)
	{
	// Get image height and width from scan driver.
		DWORD dwHeight, dwWidth;
		char s[10];
		int Score;
		int value;
		int X,Y,xpoint,ypoint;
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&dwHeight,4,NULL);
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&dwWidth,4,NULL);
		value=sscanf( *pp_CommandString, ",%d,%d,%d,%d", &xpoint,&ypoint,&X,&Y);		/* parse the offset */
		if(value!=4){ 
			xpoint=0;
			ypoint=0;
			X=dwWidth;
			Y=dwHeight;
			xputstring("using defalt parameter.\r\n");
		}
		if(((X+xpoint)>dwWidth)||((Y+ypoint)>dwHeight)){
			xputstring("input size too large");
			return true;
			}	
		UCHAR *ptr;
  		UCHAR *imgCopy = (UCHAR *)malloc(X*Y);
 		ptr=P_SCANBUFFER+xpoint*dwWidth+ypoint;		
     		  for (int i=0; i<Y; i++)
		 {
         		 Fast_memcpy(imgCopy,ptr,X);
          		 ptr += dwWidth;
          		 imgCopy += X;
      		 }
		imgCopy -= (X*Y);
		Score=SmearTest(imgCopy, X, Y);
		free(imgCopy);
		xputstring("SmearTest Result = ");
		sprintf(s,"%04d",Score);
		xputstring(s);
		xputstring("\r\n");
	}
	else{
		  xputstring("No image in buffer\r\n");
		}
	return true;
		
}
//ProductionTest
bool ImgProductionTest(void)
{
if(P_SCANBUFFER != NULL)
	{
	// Get image height and width from scan driver.
		DWORD dwHeight, dwWidth;
		int Score;
		char s[100];
		int value;
		int X,Y,xpoint,ypoint;
		int conf, maxdis, ndis, xdis[2000], ydis[2000],maxbad, nbad,xbad[2000],ybad[2000],maxmaj, nmajor, xmajor[2000], ymajor[2000], maxmin, nminor, xminor[2000], yminor[2000], outofspec;
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&dwHeight,4,NULL);
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&dwWidth,4,NULL);
		value=sscanf( *pp_CommandString, ",%d,%d,%d,%d,%d,%d,%d,%d,%d", &xpoint,&ypoint,&X,&Y,&conf,&maxdis, &maxbad, &maxmaj, &maxmin );		/* parse the offset */
		if(value!=9){ 
			xpoint=0;
			ypoint=0;
			X=dwWidth;
			Y=dwHeight;
			conf=0;
			maxdis=maxmaj=maxbad=maxmin=1000;
			xputstring("using defalt parameter.\r\n");
		}
		if(((X+xpoint)>dwWidth)||((Y+ypoint)>dwHeight)){
			xputstring("input size too large");
			return true;
			}	
		UCHAR *ptr;
  		UCHAR *imgCopy = (UCHAR *)malloc(X*Y);
		if( NULL == imgCopy )
		return FAIL;
		*imgCopy = '\0';
 		ptr=P_SCANBUFFER+xpoint*dwWidth+ypoint;		
       for (int i=0; i<Y; i++)
		 {
         	 Fast_memcpy(imgCopy,ptr,X);
          	 ptr += dwWidth;
          	 imgCopy += X;
      		 }
		imgCopy -= (X*Y);
		Score=ProductionTest(imgCopy, X,Y, conf, 
                                                    maxdis, &ndis, xdis, ydis,
                                                    maxbad, &nbad, xbad, ybad,
                                                    maxmaj, &nmajor, xmajor, ymajor,
                                                	maxmin,  &nminor, xminor, yminor, &outofspec);
		RESET_WATCHDOG_TIMER();
		for(int p=0;p<nbad;p++){
			if(xbad[p]!=-1)
			xbad[p]+=xpoint;
			if(ybad[p]!=-1)
			ybad[p]+=ypoint;
		}
		for(int p=0;p<ndis;p++){
			if(xdis[p]!=-1)
			xdis[p]+=xpoint;
			if(ydis[p]!=-1)
			ydis[p]+=ypoint;
		}
		for(int p=0;p<nmajor;p++){
			xmajor[p]+=xpoint;
			ymajor[p]+=ypoint;
		}
		for(int p=0;p<nminor;p++){
			xminor[p]+=xpoint;
			yminor[p]+=ypoint;
		}	
			xputstring("the value of nbad=");
			sprintf(s,"%04d \r\n",nbad);
			xputstring(s);
			if( nbad >0){
				xputstring("the coordinate of bad point:\r\n");
				for(int p=0;p<nbad;p++){
					sprintf(s,"(%04d, %04d) ",xbad[p],ybad[p]);
					xputstring(s);
					if(p%100==0)
						RESET_WATCHDOG_TIMER();
				}
				xputstring("\r\n");
			}
			xputstring("the value of ndis=");
			sprintf(s,"%04d \r\n",ndis);
			xputstring(s);
			xputstring("the value of nmajor=");
			sprintf(s,"%04d \r\n",nmajor);
			xputstring(s);
			if(nmajor > 0){
				xputstring("the coordinate of major blemishes:\r\n");
				for(int p=0;p<nmajor;p++){
					sprintf(s,"(%04d, %04d) ",xmajor[p],ymajor[p]);
					xputstring(s);
				}
				xputstring("\r\n");
			}
			xputstring("the value of nminor=");
			sprintf(s,"%04d \r\n",nminor);
			xputstring(s);
			if(nminor > 0){
				xputstring("the coordinate of minor blemishes:\r\n");
				for(int p=0;p<nminor;p++){
					sprintf(s,"(%04d, %04d) ",xminor[p],yminor[p]);
					xputstring(s);
				}
				RESET_WATCHDOG_TIMER();
				xputstring("\r\n");
			}
			xputstring("the value of outofspec=");
			sprintf(s,"%04d \r\n",outofspec);
			xputstring(s);
			if(Score)
				xputstring("ProductionTest successed ");
			else
				xputstring("ProductionTest failed\r\n");
			
	//		SendChasteImage(X, Y, imgCopy); 
			free(imgCopy);
		RESET_WATCHDOG_TIMER();
		xputstring("\r\n");
	}
	else{
		xputstring("No image in buffer\r\n");
		}
		
	return true;

}

// SignalToNoiseTest
bool ImgSignalToNoiseTest(void)
{
	if(P_SCANBUFFER != NULL)
	{
	// Get image height and width from scan driver.
		DWORD dwHeight, dwWidth;
		char s[200];
		int value;
		int X,Y,xpoint,ypoint;
		double PixelAverage, PixelStandardDeviation, NoiseAverage, RMSNoise, SignalToNoise;
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&dwHeight,4,NULL);
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&dwWidth,4,NULL);
		value=sscanf( *pp_CommandString, ",%d,%d,%d,%d", &xpoint,&ypoint,&X,&Y);		/* parse the offset */
		if(value!=4){ 
			xpoint=0;
			ypoint=0;
			X=dwWidth;
			Y=dwHeight;
			xputstring("using defalt parameter.\r\n");
		}
		if(((X+xpoint)>dwWidth)||((Y+ypoint)>dwHeight)){
			xputstring("input size too large");
			return true;
			}	
		UCHAR *ptr;
  		UCHAR *imgCopy = (UCHAR *)malloc(X*Y);
 		ptr=P_SCANBUFFER+xpoint*dwWidth+ypoint;		
     		  for (int i=0; i<Y; i++)
		 {
         		 Fast_memcpy(imgCopy,ptr,X);
          		 ptr += dwWidth;
          		 imgCopy += X;
      		 }
		imgCopy -= (X*Y);
		ComputeSNR(imgCopy, X, Y, &PixelAverage, &PixelStandardDeviation, &NoiseAverage, &RMSNoise, &SignalToNoise);
		free(imgCopy);
		sprintf(s,"PixelAverage=%lf  PixelStandardDeviation=%lf  NoiseAverage=%lf  RMSNoise=%lf  SignalToNoise=%lf",PixelAverage, PixelStandardDeviation, NoiseAverage, RMSNoise, SignalToNoise);
		xputstring(s);
		xputstring("\r\n");
	}
	else{
		xputstring("No image in buffer\r\n");
	}
	return true;
}

//RowNoiseTest
bool ImgRowNoiseTest(void)
{
if(P_SCANBUFFER != NULL)
	{
	// Get image height and width from scan driver.
		DWORD dwHeight, dwWidth;
		char s[100];
		int value;
		int X,Y,xpoint,ypoint;
		int topRatio, bottomRatio;
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&dwHeight,4,NULL);
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&dwWidth,4,NULL);
		value=sscanf( *pp_CommandString, ",%d,%d,%d,%d", &xpoint,&ypoint,&X,&Y);		/* parse the offset */
		if(value!=4){ 
			xpoint=0;
			ypoint=0;
			X=dwWidth;
			Y=dwHeight;
			xputstring("using defalt parameter.\r\n");
		}
		if(((X+xpoint)>dwWidth)||((Y+ypoint)>dwHeight)){
			xputstring("input parameters too large");
			return true;
			}	
		UCHAR *ptr;
  		UCHAR *imgCopy = (UCHAR *)malloc(X*Y);
 		ptr=P_SCANBUFFER+xpoint*dwWidth+ypoint;		
     		  for (int i=0; i<Y; i++)
		 {
         		 Fast_memcpy(imgCopy,ptr,X);
          		 ptr += dwWidth;
          		 imgCopy += X;
      		 }
		imgCopy -= (X*Y);
		CalculateRowNoiseParameters (imgCopy, X, Y, &topRatio, &bottomRatio);
		sprintf(s,"TopRatio=%d BottomRatio=%d",topRatio,bottomRatio);
		xputstring(s);
		xputstring("\r\n");
		free(imgCopy);
	}
	else{
		xputstring("No image in buffer\r\n");
	}	
	return true;
}

//FlatnessTest
bool ImgFlatnessTest(void)
{
	if(P_SCANBUFFER != NULL)
	{
	// Get image height and width from scan driver.
		DWORD dwHeight, dwWidth;
		char s[100];
		int value;
		int X,Y,xpoint,ypoint;
		int cx,cy,ul,uc,ur,ml,mr,ll,lc,lr;
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&dwHeight,4,NULL);
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&dwWidth,4,NULL);
		value=sscanf( *pp_CommandString, ",%d,%d,%d,%d", &xpoint,&ypoint,&X,&Y);		/* parse the offset */
		if(value!=4){ 
			xputstring("using defalt parameter.\r\n");
			CalculateOpticalFlatness (P_SCANBUFFER,dwWidth, dwHeight, &cx, &cy, &ul, &uc, &ur, &ml, &mr, &ll, &lc, &lr);
		}
		else{
			if(((X+xpoint)>dwWidth)||((Y+ypoint)>dwHeight)){
				xputstring("input parameters too large");
				return true;
			}	
			UCHAR *ptr;
  			UCHAR *imgCopy = (UCHAR *)malloc(X*Y);
 			ptr=P_SCANBUFFER+xpoint*dwWidth+ypoint;		
     			  for (int i=0; i<Y; i++)
			 {
         			 Fast_memcpy(imgCopy,ptr,X);
          			 ptr += dwWidth;
          			 imgCopy += X;
      			 }
			imgCopy -= (X*Y);
			CalculateOpticalFlatness (imgCopy,X, Y, &cx, &cy, &ul, &uc, &ur, &ml, &mr, &ll, &lc, &lr);
			free(imgCopy);
			RESET_WATCHDOG_TIMER();
			}
		xputstring("The test result of cx, cy ,ul, uc, ur, ml, mr, ll, lc, lr are listed below:\r\n");			
		sprintf(s,"%04d, %04d, %04d, %04d, %04d, %04d, %04d, %04d, %04d, %04d",cx,cy,ul,uc,ur,ml,mr,ll,lc,lr);
		xputstring(s);
		xputstring("\r\n");
	}
	else{
		xputstring("No image in buffer\r\n");
	}	
	return true;
}

//DirtTest
bool ImgDirtTest(void)
{
	if(P_SCANBUFFER != NULL)
	{
	// Get image height and width from scan driver.
		DWORD dwHeight, dwWidth;
		int Score,X,Y,xpoint,ypoint,value;
		char s[15];
		int numDirtSpots;
		DirtSpotInfo di[1280];
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_HEIGHT,NULL,0,(PBYTE)&dwHeight,4,NULL);
		SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_GET_SCAN_WIDTH,NULL,0,(PBYTE)&dwWidth,4,NULL);
		value=sscanf( *pp_CommandString, ",%d,%d,%d,%d", &xpoint,&ypoint,&X,&Y);		/* parse the offset */
		if(value!=4){ 
			xpoint=0;
			ypoint=0;
			X=dwWidth;
			Y=dwHeight;
			xputstring("using defalt parameter.\r\n");
		}
		if(((X+xpoint)>dwWidth)||((Y+ypoint)>dwHeight)){
			xputstring("input size too large");
			return true;
			}	
		UCHAR *ptr;
  		UCHAR *imgCopy = (UCHAR *)malloc(X*Y);
 		ptr=P_SCANBUFFER+xpoint*dwWidth+ypoint;		
     		  for (int i=0; i<Y; i++)
		 {
         		 Fast_memcpy(imgCopy,ptr,X);
          		 ptr += dwWidth;
          		 imgCopy += X;
      		 }
		imgCopy -= (X*Y);
		Score=DirtTestImage(imgCopy, X, Y, &numDirtSpots, di);
		for(int p=0;p<numDirtSpots;p++){
			di[p].x+=xpoint;
			di[p].y+=ypoint;
		}
		xputstring("The number of dirt spot is: ");
		sprintf(s,"%04d \r\n",numDirtSpots);
		xputstring(s);
		xputstring("the coordinate of dirt spot is:\r\n");
			for(int p=0;p<numDirtSpots;p++){
				sprintf(s,"(%04d, %04d) ",di[p].x,di[p].y);
				xputstring(s);
			}
			xputstring("\r\n");
		free(imgCopy);
		if(Score)
			xputstring("DirtTest successed ");
		else	
			xputstring("DirtTest failed");
			
		//sprintf(s,"%04d",Score);
		//xputstring(s);
		xputstring("\r\n");
	}
	else{
		xputstring("No image in buffer\r\n");
		xputstring("\r\n");
	}
	return true;
}
bool ImgDirtTestREV(void)
{
	char s[50];
	GetDIRTVersion(s);
	xputstring(s);
	return true;
}

#else

#endif
#endif // endof #ifndef  RFBASE

static void DumpGpio(void)
{
	int BaseAddress = 0;
	int	CurrentAddress = 0;
	theStdInterface.printf("iMX25 GPIO Dump:\r\n");
	
	theStdInterface.printf("IOMUX:\r\n");
	BaseAddress = IOMUXC_BASE_ADDR;
	for(int i=0;i<0x584;i+=4)
	{
		CurrentAddress = BaseAddress + i;
		theStdInterface.printf("Address: %X Value: %X\r\n", CurrentAddress, READ_REG(CurrentAddress));
	}
	
	theStdInterface.printf("\r\n\r\nGPIO Values:\r\n");
	theStdInterface.printf("GPIO 1 Data Address: %X Value: %X\r\n", GPIO1_DR0, READ_REG(GPIO1_DR0));
	theStdInterface.printf("GPIO 1 Direction Address: %X Value: %X\r\n", GPIO1_GDIR0, READ_REG(GPIO1_GDIR0));
	theStdInterface.printf("GPIO 2 Data Address: %X Value: %X\r\n", GPIO2_DR0, READ_REG(GPIO2_DR0));
	theStdInterface.printf("GPIO 2 Direction Address: %X Value: %X\r\n", GPIO2_GDIR0, READ_REG(GPIO2_GDIR0));
	theStdInterface.printf("GPIO 3 Data Address: %X Value: %X\r\n", GPIO3_DR0, READ_REG(GPIO3_DR0));
	theStdInterface.printf("GPIO 3 Direction Address: %X Value: %X\r\n", GPIO3_GDIR0, READ_REG(GPIO3_GDIR0));
	theStdInterface.printf("GPIO 4 Data Address: %X Value: %X\r\n", GPIO4_DR0, READ_REG(GPIO4_DR0));
	theStdInterface.printf("GPIO 4 Direction Address: %X Value: %X\r\n", GPIO4_GDIR0, READ_REG(GPIO4_GDIR0));
		
}

#if defined(IR_TRIGGER)
/**
 * Command: "IRCFG par1, par2"
 * Configure the IR emitter power(IR_DAC value) according with the require IR 
 * detection distance. This configure process is run in manufacturing fixture, 
 * before run this funtion, need set a white pad at required distance from scan 
 * window.
 *
 * Parameter: 
 * par1: start value of IR DAC
 * par2: max value of IR DAC
 */
static bool NCmdIrConfig (void)
{
	int config_result = 0;
	int start_value, max_value;

	int NumVarsRead = sscanf( *pp_CommandString, " %d, %d", &start_value, &max_value);

	if(NumVarsRead == 2)
	{
		theStdInterface.printf("waiting for ir configuration...\r\n", config_result);
		config_result = IrConfig(start_value, max_value);
		theStdInterface.printf("dac:%d\r\n", config_result);
	}
	else
	{
		xputstring("\x15");
	}
	return true;
}

static bool NCmdIrRead (void)
{
	bool result = true;
	
	theStdInterface.printf("IR: %d\r\n", IrCheck());

	return result;
}
#endif
