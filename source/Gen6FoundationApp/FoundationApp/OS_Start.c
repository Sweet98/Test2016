/**************************************************************************************/
//
// HIM 48X0G GENERATION 6 SOFTWARE
//
//  MAIN
//  Date:       6/23/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX27
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/OS_Start.c $
// $Revision: 1.186 $
// $Date: 2011/11/18 17:51:55EST $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// File:  OS_Start.c
//
// C Application Code, used to start the OS and go into a Main Loop
//
//------------------------------------------------------------------------------
#define RUN_SCAN_DRIVER 1				//Define to '1' if you want to run the Scan Driver
#define SCAN_DRIVER_CLOSE_TEST 0					//lmc   debug
#include <fenv.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <rt_fp.h>
#include "db_sharedtable.h"
#include "db_chrio.h"
#include "db_time.h"
#include "OsTimer.h"
#include "db_Heap.h"
#include "interrupts.h"
#include "PowerManagement.h"
#include "thread.h"
#include "mx25spi.h"
#include "beep.h"
#include "StartupFlashDisk.h"
#include "ReportStartupTiming.h"
#include "barcodedataman.h"
#include "ioman.h"
#include "Matrixhcomm.h"
#include "MatrixHTransferStatus.h"
#include "SynCommandParser.h"
#include "ESCCommandParse.h"
#include "MatrixSynFECommandHandler.h"
#include "menu.h"
#include "MenuFlash.h"
#include "ReaderConfiguration.h"
#include "ProductConfiguration.h"
#include "Interface_support_functions.h"
#include "i2c_drvr.h"
//ken@2012.02.14 make sure hi704a.h clash with jade.h?????
#include "Hi704A.h"
#include "scandrivermatrixinterface.h"
#include "imaging.h"
#include "Interface_support_functions.h"
#include "DecodeIFace.h"
#include "mainhhplproc.h"
#include "dataedit.h"
#include "eci.h"
#include "WedgeHal.h"
#include "HardwareToken.h"
#include "HWPageButton.h"
#include "AutoCableSelect.h"
#include "Timer3_Driver.h"
#include "usage_statistics.h"
#include "flashapplicationcode.h"
#include "RemRouter.h"
#include "IndicatorTfb.h"
#include "ScanEnginePSOC.h"

#define	TRIGGER_5_5		1
#ifdef TRIGGER_5_5
#include "trigger_5_5.h"                                         //lmc   fixme
#else
#include "TriggerHandler.h"
#endif	//end #ifdef TRIGGER_5_5

#include "HWTrigger.h"
#ifdef IR_TRIGGER
#include "IRTrigger.h"
#endif

#ifdef PLUGIN_DEV
#include "HONPluginConfiguration.h"

#endif


// TODO:  these should be in a header file
extern "C" void MMU_init(void);
extern "C" void MMU_reset(void);
extern "C" int MMU_IsEnabled( void );
extern "C" void MMU_CleanAndInvalidateBothCaches(void);
extern "C" void Move_Vector_Table_To_Zero(void);

#ifdef FLASH_LEDS
#include "Flash_Illumination_Control.h"
#else
#include "iMX25_Illumination_Control.h"
#endif //endof FLASH_LEDS


bool PlugAndPlayFactoryTest(void);

#if defined(SM3110) || defined(SM3100)|| defined(SM5130)
bool bTriggerAsync;
bool bAimer;
#endif
extern bool	bFactoryTestMode;
extern void UpdatePowerDownPin(void);


/*----------------------------------------------------------------------------------------
SetupReaderConfiguration

  Creates the reader config object & inits it with compile defined and Product
  Config options.

  Parameters:  none

  Returns:  nothing
----------------------------------------------------------------------------------------*/
void SetupReaderConfiguration(void)
{
    int ReaderConfigurationMask=0;
#if defined( DEVELOPMENT_SETTINGS ) || defined( MANUFACTURING_IMAGER_TEST )   

    ReaderConfigurationMask |= MAKE_LINEAR_SYMBOLOGIES_AVAILABLE;
    //ReaderConfigurationMask |= MAKE_CODE_Z_SYMBOLOGY_AVAILABLE;              
    //ReaderConfigurationMask |= MAKE_VERICODE_SYMBOLOGY_AVAILABLE;            
    ReaderConfigurationMask |= MAKE_BC_412_SYMBOLOGY_AVAILABLE;
    //ReaderConfigurationMask |= MAKE_SECURECODE_SYMBOLOGY_AVAILABLE;          

    ReaderConfigurationMask |= MAKE_2D_COMMANDS_AVAILABLE;
    ReaderConfigurationMask |= MAKE_IMAGE_SHIPPING_AVAILABLE;
    ReaderConfigurationMask |= MAKE_SCRIPTING_AVAILABLE;
    //ReaderConfigurationMask |= MAKE_DPC_CIRRUS_SYMBOLOGY_AVAILABLE;          
    ReaderConfigurationMask |= MAKE_GRID_MATRIX_SYMBOLOGY_AVAILABLE;

    ReaderConfigurationMask |= MAKE_HEAT_MITIGATION_AVAILABLE;

#else		//if not development settings

#ifdef RFBASE

#else
  
    ReaderConfigurationMask |= (ProductConfiguration->LinearSymbologies) ? MAKE_LINEAR_SYMBOLOGIES_AVAILABLE : 0;
    ReaderConfigurationMask |= (ProductConfiguration->CodeZSymbology) ? MAKE_CODE_Z_SYMBOLOGY_AVAILABLE : 0;
    ReaderConfigurationMask |= (ProductConfiguration->VericodeSymbology) ? MAKE_VERICODE_SYMBOLOGY_AVAILABLE : 0;
    ReaderConfigurationMask |= (ProductConfiguration->BC412Symbology) ? MAKE_BC_412_SYMBOLOGY_AVAILABLE : 0;
    ReaderConfigurationMask |= (ProductConfiguration->SecureCodeSymbology) ? MAKE_SECURECODE_SYMBOLOGY_AVAILABLE : 0;
    ReaderConfigurationMask |= (ProductConfiguration->iOpticsPopulation==TWO_D) ? MAKE_2D_COMMANDS_AVAILABLE :0;
    ReaderConfigurationMask |= (ProductConfiguration->ImageShippingAvailable) ? MAKE_IMAGE_SHIPPING_AVAILABLE :0;
    ReaderConfigurationMask |= (ProductConfiguration->DpcCirrusSymbology) ? MAKE_DPC_CIRRUS_SYMBOLOGY_AVAILABLE : 0;
    ReaderConfigurationMask |= (ProductConfiguration->GmxMatrixSymbology) ? MAKE_GRID_MATRIX_SYMBOLOGY_AVAILABLE : 0;

    ReaderConfigurationMask |= (ProductConfiguration->HeatMitigationFixedSettings) ? MAKE_HEAT_MITIGATION_FIXED_SETTINGS_FIXED : 0;
    ReaderConfigurationMask |= (ProductConfiguration->HeatMitigationFixedSettings) ? MAKE_HEAT_MITIGATION_AVAILABLE : 0;

    ReaderConfigurationMask |= ((GET_CONFIG_SETTING(iScriptingAvailable)) || (GET_CONFIG_SETTING(IQEditingProAvailable)))
                               ? MAKE_SCRIPTING_AVAILABLE :0;
    ReaderConfigurationMask |= (ProductConfiguration->DpcCirrusSymbology) ? MAKE_DPC_CIRRUS_SYMBOLOGY_AVAILABLE : 0;
#endif 	// end else from if RFBASE
#endif		//end "ELSE" if not development settings

    ReaderConfigurationMask |= MAKE_AIMER_COMMANDS_AVAILABLE;
    ReaderConfiguration = new READER_CONFIGURATION(ReaderConfigurationMask);
	ReaderConfiguration->ConfigureLicensableCommands();
}


/*----------------------------------------------------------------------------------------
SetupProductConfiguration

  Creates the Product Config object & inits it with stored data.

  Parameters:
  				ConfigurationStorage[in/out]:  an HSTRING used for temporary storage.  We
  				reuse this area so we don't have to repeatedly call new/delete.

  Returns:  nothing
----------------------------------------------------------------------------------------*/
void SetupProductConfiguration(HSTRING *ConfigurationStorage)
{
    ProductConfiguration = new PRODUCT_CONFIGURATION();
    //	being read from or written to flash.
    //	if (ReadProductConfigurationFromFlash(ProductConfigurationStorage))
    //xputstring("SetupProductConfiguration--ReadAndCloseFile--Before\r\n");
    int sz = ReadAndCloseFile("ProductConfig",FLASH_MAX_CONFIGURATION_LENGTH * 2,&ConfigurationStorage->Char);
	//xputstring("SetupProductConfiguration--ReadAndCloseFile--ProductConfig\r\n");
    if (sz>0)
    {
        // If the product configuration is valid, load it to ProductConfiguration.
        ConfigurationStorage->Size=sz;
        ProductConfiguration->Menu(ConfigurationStorage, true);
    }
    ProductConfiguration->WorkingValuesUpToDate();
    ProductConfiguration->ActiveValuesUpToDate();
}


/*----------------------------------------------------------------------------------------
ReadWorkingAndDefaultConfigurations

  Reads the Working Config stored data.

  Parameters:
  				ConfigurationStorage[in/out]:  an HSTRING used for temporary storage.  We
  				reuse this area so we don't have to repeatedly call new/delete.

  Returns:  nothing
----------------------------------------------------------------------------------------*/
void ReadWorkingAndDefaultConfigurations(HSTRING *ConfigurationStorage,HSTRING*ConfigurationStorage1)
{
    // Check the working and default configurations in flash.  If a working copies are available, load them.
    //	NOTE:
    //		We try to load the working configuration first.  The reason we do that is in case there are any customer defaults dependent upon
    //		working values.  The one example at this point is "232BAD".  If the customer default is "232BAD11", then "HIBAUD1" must be set in
    //		the working table, else "232BAD11" will fail.  If we don't find a working configuration in flash, then after we load customer defaults,
    //		we'll copy the customer defaults to the working and active tables.
    //	being read from or written to flash.
#ifdef RFSCAN
#else

    int PositionTableSettingWasFound;
    int ConfigTableNum=0;
    HSTRING ConfigCabSel(20,false);
    if(PresentInterface==Interface_232_Group)
    {
        ConfigCabSel.Copy("060302");
        ReaderConfiguration->UpdateNumericListWorkingDefaltValue("060300",0);
    }
    else if(PresentInterface==Interface_485_Group)
    {
        ConfigCabSel.Copy("060304");
         ReaderConfiguration->UpdateNumericListWorkingDefaltValue("060300",3);
    }
    else if(PresentInterface==Interface_KBDWDG_Group)
    {
        ConfigCabSel.Copy("060303");
         ReaderConfiguration->UpdateNumericListWorkingDefaltValue("060300",1);
    }
    else if((PresentInterface==Interface_USB_Group)||(PresentInterface==Interface_Unknow))
    {
        ConfigCabSel.Copy("060301");
         ReaderConfiguration->UpdateNumericListWorkingDefaltValue("060300",2);
    }
    int sz0 = ReadAndCloseFile(FILENAME_WORKING_CONFIG_0,FLASH_MAX_CONFIGURATION_LENGTH * 2,&ConfigurationStorage->Char);
    if(sz0>0)
    {
        ReaderConfiguration->FoundValidWorkingConfigurationInFlash = true;
        ReaderConfiguration->FirstTimeLoadWorkingConfigurationInFlash=true;
        MarkTime(__LINE__,"ReadWorkingConfig0FormFlashDisk:");
        ConfigurationStorage->Size=sz0;
        ConfigurationStorage->Char[ConfigurationStorage->Size] = 0;		// Probably unnecessary, since ReadWorkingConfigurationFromFlash should do this.
        ReaderConfiguration->FoundValidWorkingConfiguration0InFlash=true;
        ConfigTableNum=0;
        int sz1 = ReadAndCloseFile(FILENAME_WORKING_CONFIG_1,FLASH_MAX_CONFIGURATION_LENGTH * 2,&ConfigurationStorage1->Char);
        if(sz1>0)
        {
            ReaderConfiguration->FoundValidWorkingConfiguration1InFlash=true;
            ConfigurationStorage1->Size=sz1;
            ConfigurationStorage1->Char[ConfigurationStorage1->Size]=0;

		PositionTableSettingWasFound = AutoCable_StrIndex(ConfigurationStorage->Char, ConfigCabSel.Char);

		if(PresentInterface!=Interface_Fac232_Group && PositionTableSettingWasFound >= 0) // if factory then use table 0 else use the table in menu
		{
			PositionTableSettingWasFound += 6;
			ConfigTableNum=ConfigurationStorage->Char[PositionTableSettingWasFound]-'0';
		}

        }
        // If the working configuration is valid, load it to ReaderConfiguration.
        if(ConfigTableNum )
        {
            Menu(&ConfigurationStorage1->Size,ConfigurationStorage1->Char);
        }
        else
        {
            Menu(&ConfigurationStorage->Size, ConfigurationStorage->Char);
        }
        ReaderConfiguration->SetWorkTable(ConfigTableNum);
        ReaderConfiguration->SetActiveWorkTable(ConfigTableNum);
        ReaderConfiguration->SetCurrentTableForScannedCommand(ConfigTableNum);


    }
    ReaderConfiguration->FirstTimeLoadWorkingConfigurationInFlash=false;
    MarkTime(__LINE__,"ReadWorkingConfigFormFlashSetupTime:");
#endif

    int sz2 = ReadAndCloseFile("DefaultConfig",FLASH_MAX_CONFIGURATION_LENGTH * 2,&ConfigurationStorage->Char);
    if(sz2>0)
    {
        // If the default configuration is valid, load it to ReaderConfiguration.
        ConfigurationStorage->Size=sz2;
        ReaderConfiguration->Menu(ConfigurationStorage, false, true);
    }
    MarkTime(__LINE__,"ReadDefaultConfigFormFlashSetupTime:");
}
/*----------------------------------------------------------------------------------------
UpdateAfterReadWorkingAndDefaultConfiguration

  This looks to see if either of the Working Config files are missing and creates them
  from the default settings.  We have to be careful here because ReaderConfiguration
  may be set up (have new Menu Settings and possibly Custom Defaults) for the active
  table.  We don't want to inadvertently reset ReaderConfiguration (set it to its defaulst).
  So, we check to see what WorkingTable (Working Config table) is active before we
  reset ReaderConfiguration (and write the defaults to the file).

  We used to write the defaults to both Working Config files if either was missing.  This
  isn't very robust, but more importantly, the System Update MOCF will have created ONLY
  a single Working Config and it's Menu Settings must be retained for Test Engineering.
  However, we need to ensure both Working Config files exist.  So, we do create the ones
  that are missing and then fix things up later.


  Parameters:
		ConfigurationStorage[in, out]:  scratchpad HSTRING pre-allocated with hopefully
										enough space.  I believe we do this to save time
										because we need to boot fast to meet USB spec.

  Returns:  nothing
----------------------------------------------------------------------------------------*/
void UpdateAfterReadWorkingAndDefaultConfiguration(HSTRING *ConfigurationStorage)
{
#ifdef RFSCAN
#else
	if( (! ReaderConfiguration->FoundValidWorkingConfiguration1InFlash)|| (! ReaderConfiguration->FoundValidWorkingConfiguration0InFlash) )
	{
		int RestoreTable = -1;
		// If the working configuration was not valid, create a working configuration from the user defaults in ReaderConfiguration.
		if ( ! ReaderConfiguration->FoundValidWorkingConfiguration0InFlash )
		{	// See RFSCAN code above for a description of how the following lines work.
			ReaderConfiguration->CopyDefaultsToWorkingAndActive();
			ConfigurationStorage->Copy("$.");
			ReaderConfiguration->Menu(ConfigurationStorage);
			EncodeBuffer(FILENAME_WORKING_CONFIG_0,ConfigurationStorage->Char,ConfigurationStorage->Size);
			WriteAndCloseFile(FILENAME_WORKING_CONFIG_0,ConfigurationStorage->Char,ConfigurationStorage->Size);
			if ( ReaderConfiguration->FoundValidWorkingConfiguration1InFlash && (ReaderConfiguration->GetActiveWorktableNumber() == 1) )
			{	// We just defaulted ReaderConfiguration and the active table is 1 and there's a valid WorkingConfig file--we need to restore table 1 Menu Settings.
				RestoreTable = 1;
			}
		}
		if ( ! ReaderConfiguration->FoundValidWorkingConfiguration1InFlash )
		{
			ReaderConfiguration->CopyDefaultsToWorkingAndActive();
			ConfigurationStorage->Copy("$.");
			ReaderConfiguration->Menu(ConfigurationStorage);
			EncodeBuffer(FILENAME_WORKING_CONFIG_1,ConfigurationStorage->Char,ConfigurationStorage->Size);
			WriteAndCloseFile(FILENAME_WORKING_CONFIG_1,ConfigurationStorage->Char,ConfigurationStorage->Size);
			if ( ReaderConfiguration->FoundValidWorkingConfiguration0InFlash && (ReaderConfiguration->GetActiveWorktableNumber() == 0) )
			{	// We just defaulted ReaderConfiguration and the active table is 0 and there's a valid WorkingConfig file--we need to restore table 0 Menu Settings.
				RestoreTable = 0;
			}
		}

		if ( RestoreTable == 0 )
		{	// Read the settings from WorkingConfig0 and Menu().
			HSTRING MenuSettings(FLASH_MAX_CONFIGURATION_LENGTH * 2 + 1, true);
		    int length = ReadAndCloseFile(FILENAME_WORKING_CONFIG_0, FLASH_MAX_CONFIGURATION_LENGTH * 2, &MenuSettings.Char);
			if ( length > 0 )
		    {
				MenuSettings.Size = length;
				MenuSettings.Char[MenuSettings.Size] = 0;
				Menu(&MenuSettings.Size, MenuSettings.Char);
		    }
		}
		else if ( RestoreTable == 1 )
		{
			HSTRING MenuSettings(FLASH_MAX_CONFIGURATION_LENGTH * 2 + 1, true);
		    int length = ReadAndCloseFile(FILENAME_WORKING_CONFIG_1, FLASH_MAX_CONFIGURATION_LENGTH * 2, &MenuSettings.Char);
			if ( length > 0 )
		    {
				MenuSettings.Size = length;
				MenuSettings.Char[MenuSettings.Size] = 0;
				Menu(&MenuSettings.Size, MenuSettings.Char);
		    }
		}

		if(PresentInterface==Interface_232_Group)
		{
			ReaderConfiguration->UpdateNumericListWorkingDefaltValue("060300",0);
		}
		else if(PresentInterface==Interface_485_Group)
		{
			ReaderConfiguration->UpdateNumericListWorkingDefaltValue("060300",3);
		}
		else if(PresentInterface==Interface_KBDWDG_Group)
		{
			ReaderConfiguration->UpdateNumericListWorkingDefaltValue("060300",1);
		}
		else
		{
			ReaderConfiguration->UpdateNumericListWorkingDefaltValue("060300",2);
		}
	}
#endif
}





/*----------------------------------------------------------------------------------------
ProcessUpdateInformation

  If fw was downloaded, a temporary file will be placed in the filesytem
  to help us restore Menu Settings to what they were before we rebooted.  We read the file and
  restore the settings.


  Parameters:  none

  Returns:  true if an Update Information File was found, false otherwise
----------------------------------------------------------------------------------------*/
bool ProcessUpdateInformation(int * pDownloadIface)
{
	char FlashDiskData[150];
	int NumReadFromFile;
	bool bFileFound = false;


	BYTE * pData = (BYTE *)FlashDiskData;
	NumReadFromFile = ReadAndCloseFile(FILE_FW_UPDATE, sizeof FlashDiskData - 1, &pData);
	if ( NumReadFromFile >= 0 )
	{	// A file was found and read successfully
		bool bIsHiBaud=GET_SETTING(bEngSettings);
		FlashDiskData[NumReadFromFile] = 0;  // terminate string
		if ( NumReadFromFile > 2 ) //is there any data?
		{
			if ( ! bIsHiBaud )
			{
				RAM_SET_ACTUAL(bEngSettings,1); //allow higher baud rates
				ExpandBaudRates();
			}
			HSTRING HStrFlashData(FlashDiskData);
			ReaderConfiguration->Menu(&HStrFlashData);	//restore setting we used to flash at
			ReconfigureInterface(); //set the IF to this setting
			*pDownloadIface = GET_SETTING(DownloadMedium); // save this before it gets overwritten by the following CopyWorkingSettingsOverActiveSettings()
			ReaderConfiguration->CopyWorkingSettingsOverActiveSettings();  //allow next reconfigure IF to take defaults
	 	}
		EraseFile(FILE_FW_UPDATE);
		if ( GET_SETTING(GeneralDebugOn) ) xputstring("Main application has been updated\r\n");
		if ( ! bIsHiBaud )
		{
			ReaderConfiguration->ChangeNumericList("060702", RCFG_BAUD_300, RCFG_BAUD_600, RCFG_BAUD_1200, RCFG_BAUD_2400,
													RCFG_BAUD_4800,RCFG_BAUD_9600, RCFG_BAUD_19200, RCFG_BAUD_38400, RCFG_BAUD_57600, RCFG_BAUD_115200);
			RAM_SET_ACTUAL(bEngSettings,0); //Get higher baud rates off the list
		}
		bFileFound = true;
	}

	return bFileFound;
}




/*----------------------------------------------------------------------------------------
ProcessAnyFirmwareUpdate

  A fw update will leave a file containing Menu Settings that need to be updated.  This
  allows us to restore (interface related) Menu Settings to what they were before we
  rebooted.  I *think* this is necessary because of our applications.  For some reason, they
  may temporarily change the interface for download (TODO:  find out if this is so and why the
  app can't make it permanent and then change it back after the download.  Maybe we can remove
  this klunky code).

  This checks for the file and proceses the Menu Settings it contains.

  Note 1:  We used to unconditionally send the ACK out via xputchar().  This meant if a
  download happened over RF, the ACK would go out the RS232 (cradle) interface which is wrong.
  Since the RF is obviously not waiting for the ACK, now that we've fixed this, we have to
  decide if we should send the ACK--we don't.  See NG2D-931 & NG2D932.


  Parameters:  none

  Returns:  nothing
----------------------------------------------------------------------------------------*/
void ProcessAnyFirmwareUpdate(void)
{
	int DownloadIface;
	if ( ProcessUpdateInformation(&DownloadIface) )
	{	// Since we must've just downloaded code, send the ACK out the same interface the download came in on
		if ( DownloadIface == DOWNLOAD_MEDIUM_IS_CORD_REM )
		{
			theRemInterface.Write((unsigned char)ACK);
			// theRemInterface.WaitUntilSent();  // I'm not sure if we REALLY need to wait here.
		}
		else if ( DownloadIface == DOWNLOAD_MEDIUM_IS_CORD )
		{
			xputchar(ACK);
			wait_til_tx_is_empty();
		}
		// else (it's DOWNLOAD_MEDIUM_IS_RADIO):  don't send anything if it came in on RF (see Note 1 above)
		RESET_WATCHDOG_TIMER();
	}
}



bool IsOkToProcessBackroundTasks(void)
{
#if ! defined(RFBASE)
    return theScriptInterpreter.ScriptIsInBackgroundOperationsState();

#else

    return true;	// RF base
#endif
}

void ReturnToForgroundTask()
{
#if ! defined(RFBASE)
    theScriptInterpreter.SriptRunSome();
#endif

}
#if ! defined(RFBASE)

/*----------------------------------------------------------------------------------------
OpenScanDriver

  Opens the scan driver and registers global image buffers.  Also creates necessary objects
  for starting the scan driver.

  Parameters:  none

  Returns:  nothing
----------------------------------------------------------------------------------------*/
void OpenScanDriver(void)
{
    //Scan Driver - csi - i2c ***********************************************************************
#if RUN_SCAN_DRIVER		//enable scan driver
#ifdef HI2D_T//How to remove the macro???
#ifdef BYD_SENSOR
	pfSetForCachedImaging = UseCachedMemoryBYD;
#else
    pfSetForCachedImaging = UseCachedMemoryHi704A;
#endif	//end #ifdef BYD_SENSOR
#else
	pfSetForCachedImaging = UseCachedMemoryJade;
#endif	//end #ifdef HI2D_T
    RESET_WATCHDOG_TIMER();
    wait_til_tx_is_empty();
    //theStdInterface.Open(901,(unsigned char  *)"115200,N81,XON0,ACK0,CTS0,IC_X_DLY00,DelayChar00,ICDLY00,IFDLY00,IMDLY00,EciOut4097");

    if(GET_CONFIG_SETTING(ProcessorControlledIllumination))
    {
    	if (!(HardwareToken.IAmOEM()))
		{
        	Ill_Ctl_Sys_Init_Thread();  //Start the Illumination System and run the Thread, must be done before I2C and scan driver
        }
		else  
		{
			UpdatePowerDownPin();
		}
	}

    I2C_Drvr::Create_I2C_Drvr();

    /* NOTE: This must happen before the interface can be configured!!!
        * The IF open sends the PSOC a command. It will die if the PSOC
        * class is not instantiated yet. */
    if(1)//( GET_SETTING(FactoryTestMode) == 0 )
    {
        ScanDriverContextResult = SCN_Init(0);
    }
    else
    {	// Only allow ENGDIA to work when FACTON is non zero
        //ScanDriverContextResult = SCN_Init(GET_SETTING(EngineFirmwareDiagnosticMode));
    }

    nOpenCntxt = SCN_Open(ScanDriverContextResult,0,0);
	deferred_reset_state = DEFERRED_RESET_NOT_ALLOWED;
    SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_REGISTER_SCAN_BUFFER,(UCHAR *)&pGlobalImage1,0,NULL,0,NULL);
    ppCurrentScanBuffer = &pGlobalImage1;
    SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_REGISTER_SCAN_BUFFER,(UCHAR *)&pGlobalImage2,0,NULL,0,NULL);
    ppIQHoldScanBuffer = &pGlobalImage2;

#ifdef GEN6_DEVELOPMENT

    SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_REGISTER_SCAN_BUFFER,(UCHAR *)&pGlobalImage3,0,NULL,0,NULL);
    ppRecentScanBuffer = &pGlobalImage3;
#endif

#if defined(GEN6_DEVELOPMENT) || CONCURRENT

    SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_REGISTER_SCAN_BUFFER,(UCHAR *)&pGlobalImage4,0,NULL,0,NULL);
    ppFastDecodeBuffer = &pGlobalImage4;
#endif

    pfSetForCachedImaging(TRUE);

#endif
}


/*----------------------------------------------------------------------------------------
CloseScanDriver

  Closes the scan driver and unregisters global image buffers.

  Note:  It does NOT destroy any objects created in OpenScanDriver().

  Parameters:  none

  Returns:  nothing
----------------------------------------------------------------------------------------*/
void CloseScanDriver(void)
{
#if RUN_SCAN_DRIVER		//enable scan driver

#if(SCAN_DRIVER_CLOSE_TEST)

#if(SCAN_DRIVER_DEBUG_TEST == 0)

    xputstring(" scan driver is closing \r\n");
#endif

    BOOL bResult;
    ppIQHoldScanBuffer = ppCurrentScanBuffer = NULL;
    SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_RETURN_SCAN,(UCHAR *)&pGlobalImage1,0,NULL,0,NULL);
    SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_RETURN_SCAN,(UCHAR *)&pGlobalImage2,0,NULL,0,NULL);

#ifdef GEN6_DEVELOPMENT

    ppRecentScanBuffer = NULL;
    SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_RETURN_SCAN,(UCHAR *)&pGlobalImage3,0,NULL,0,NULL);
#endif

#if defined(GEN6_DEVELOPMENT) || CONCURRENT

    ppFastDecodeBuffer = NULL;
    SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_RETURN_SCAN,(UCHAR *)&pGlobalImage4,0,NULL,0,NULL);
#endif

    pfSetForCachedImaging(FALSE);
    SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_UNREGISTER_SCAN_BUFFER,(UCHAR *)&pGlobalImage1,0,NULL,0,NULL);
    SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_UNREGISTER_SCAN_BUFFER,(UCHAR *)&pGlobalImage2,0,NULL,0,NULL);

#ifdef GEN6_DEVELOPMENT

    SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_UNREGISTER_SCAN_BUFFER,(UCHAR *)&pGlobalImage3,0,NULL,0,NULL);
#endif

#if defined(GEN6_DEVELOPMENT) || CONCURRENT

    SCN_IOControl(nOpenCntxt,HHPSD_IOCTL_UNREGISTER_SCAN_BUFFER,(UCHAR *)&pGlobalImage4,0,NULL,0,NULL);
#endif

    bResult = SCN_Close(nOpenCntxt);
    nOpenCntxt = 0x00;
    SCN_Deinit(0);

    if(bResult)
    {
        xputstring(" scan driver now closed \r\n");
    }
#endif		//end #if(SCAN_DRIVER_CLOSE_TEST)
#endif    //end #if RUN_SCAN_DRIVER		//enable scan driver
}


//------------------------------------------------
// NOT a Base - OpenScanDriver_ProdSpecific
//------------------------------------------------
void OpenScanDriver_ProdSpecific(void)
{
    OpenScanDriver();
}

//------------------------------------------------
// NOT a Base - CloseScanDriver_ProdSpecific
//------------------------------------------------
void CloseScanDriver_ProdSpecific(void)
{
    CloseScanDriver();
}

//------------------------------------------------
// NOT a Base - CreateScriptInterpreter_ProdSpecific
//------------------------------------------------
void CreateScriptInterpreter_ProdSpecific(void)
{
    MAIN_HHPL_PROC::CreateScriptInterpreter();
}

//------------------------------------------------
// NOT a Base - CreateObjectsThatMenuingUses_ProdSpecific
//------------------------------------------------
void CreateObjectsThatMenuingUses_ProdSpecific(void)
{}


#endif // endof #if ! defined(RFBASE)


#if defined(RFBASE) || defined(RFSCAN)
#else // elseof #if defined(RFSCAN || RFBASE)

// put corded functions here

extern void PSOC_Port_Init();
extern void reset_psoc();

//------------------------------------------------
// Corded - PrepareForStdInterfaces_ProdSpecific
//------------------------------------------------
void PrepareForStdInterfaces_ProdSpecific(void)
{
    CWedgeHal::CreateWedgeHal();
}

void CreateAutoCableSelect_ProdSpecific(void)
{
    AutoCable_Ctl_Sys_Initialization();
}

#if defined(DISABLE_AUTO_CBL)
void CreateManualCableSelect_ProdSpecific(void)
{
	ManualCable_Ctl_Sys_Initialization();

	if(PresentInterface==Interface_232_Group)
	{
		ReaderConfiguration->UpdateNumericListWorkingDefaltValue("060300",0);
	}
	else if(PresentInterface==Interface_485_Group)
	{
		ReaderConfiguration->UpdateNumericListWorkingDefaltValue("060300",3);
	}
	else if(PresentInterface==Interface_KBDWDG_Group)
	{
		ReaderConfiguration->UpdateNumericListWorkingDefaltValue("060300",1);
	}
	else
	{
		ReaderConfiguration->UpdateNumericListWorkingDefaltValue("060300",2);
	}
}
#endif

//------------------------------------------------
// Corded - Init_ProdSpecific
//------------------------------------------------
void Init_ProdSpecific(void)
{
    DecoderPowerupInit(); //Go get the decoder ready.
    MarkTime(__LINE__,"DecodePwrTime:");
	DecodeIFaceOnChangedConfiguration(); //go do decoder menu updates
    FillDecoderMenu(); //go do decoder menu changes
    MarkTime(__LINE__,"DecodeMenuFillTime:");

#ifdef TRIGGER_5_5

    pTrigger = new CTrigger();           // create an instanse of the triggger class to manage auto and manual trigger
#else

    g_pTriggerHandler =new CTriggerHandler();           // create an instanse of the triggger class to manage auto and manual trigger
#endif	//end #ifdef TRIGGER_5_5

#if defined(IR_TRIGGER)
	IrHardwareInit();
#endif

    CHWTrigger::CreateHWTrigger();
    theScriptInterpreter.OnChangedConfiguration();	// the interpreter need to update it's internal state
    MarkTime(__LINE__,"ScriptChangeConfigTime:");
    theEci.OnChangedConfiguration();						// the ECI need to update it's internal state
    MarkTime(__LINE__,"ECIChangeConfigTime:");
#ifdef TRIGGER_5_5

    pTrigger->OnChangedConfiguration();
#else

    g_pTriggerHandler->OnChangedConfiguration();
#endif	//end #ifdef TRIGGER_5_5

    MarkTime(__LINE__,"TriggerChangeConfigTime:");
    RESET_WATCHDOG_TIMER();
    StartScript();
}


//------------------------------------------------
// Corded - DoPoll_ProdSpecific
//------------------------------------------------
void DoPoll_ProdSpecific(void)
{
#ifdef TRIGGER_5_5
    pTrigger->UpdateTriggerStatus();  // handle auto trigger serial trigger
#else

    g_pTriggerHandler->Update();  // handle auto trigger serial trigger
#endif	//end #ifdef TRIGGER_5_5
	
    theScriptInterpreter.ServiceMainScriptTimers();
	Restore_System_from_Suspend();
    IndicateCustomDefaultProgrammingMode();
}

#endif // endof #if defined(RFBASE) || defined(RFSCAN) #else


// ===  Function  ======================================================================
//
//  Name:  C_Entry
//
//  Description: The Start of (C/C++ based) Application Code.
//				All system initialization and startup code is run from here.
//				Once initialization is finished, the Main Loop is entered which
//				updates all background tasks when thread 0 is allowed to run.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
extern "C" int C_Entry (void)
{
    unsigned int SizeOfAreaToExcludeFromHeap=0;
    MarkTime(__LINE__,"BootTime:");
    bFactoryTestMode=false;

   	UsageStatistics.StartPowerTime=current_time();
	InitUsageStatistics();

    init_hardware();  //Initializes or reinitializes the Clocks, timer, and Wdog.

    RESET_WATCHDOG_TIMER();
    //Check the MMU***************************************************
    if( !MMU_IsEnabled() )		//Check to make sure the MMU is Enabled
    {
        MMU_init();				//If Not Enable it
    }

    Move_Vector_Table_To_Zero();
    MMU_CleanAndInvalidateBothCaches();
		
    //Determine what kind of App we are*************************************
#if !defined( ETM )

    if (SIZE_OF_OP_CODE_TO_STORE)
    {
        if(SIZE_OF_OP_CODE_TO_STORE == COMPRESSOR_AND_COMPRESSED_APPLICATION_SIZE)			//Check to see if this is a compressed App
        {
            if(COMPRESSED_APPLICATION_LOCATION == &APPLICATION_COMPRESSOR_LOCATION[COMPRESSOR_AND_COMPRESSED_APPLICATION_SIZE - COMPRESSED_APPLICATION_SIZE] &&
                    (COMPRESSOR_AND_COMPRESSED_APPLICATION_SIZE != 0))
            {
                SizeOfAreaToExcludeFromHeap = ( COMPRESSOR_AND_COMPRESSED_APPLICATION_SIZE);
                SizeOfAreaToExcludeFromHeap = ( SizeOfAreaToExcludeFromHeap + 7 ) & 0xFFFFFFF8;	// Add 256 bytes to keep the heap well out of the way, and
            }																						//	make sure that the size is word (4 byte) aligned.
        }
        else	//If it is not a compressed App we still need to make a copy of it for flashing.
        {
            COMPRESSED_APPLICATION_SIZE = (APP_IMAGE_ZI_BASE - APP_IMAGE_RO_BASE);
            SizeOfAreaToExcludeFromHeap = ((COMPRESSED_APPLICATION_SIZE +7) & 0xFFFFFFF8);
            COMPRESSED_APPLICATION_LOCATION = HEAP_END-SizeOfAreaToExcludeFromHeap;
            int tempTimeInfoIndex=TimeInfoIndex;
            // reset all global variables used to their starting state so when app is flashed they have the correct value
            TimeInfoIndex=0;
            memcpy((char *)COMPRESSED_APPLICATION_LOCATION,(char *)APP_IMAGE_RO_BASE, COMPRESSED_APPLICATION_SIZE);
            TimeInfoIndex=tempTimeInfoIndex;
            //We make a copy of the Application and the RW section.  This copy allows us to flash the application into ROM
            //without changing the RW sections.  if the RW section changes the Checksum that Flash Disk stores and the
            //Checksum that Boot calculates will be different and boot will not load the app.
            //Keep this new copy away from the Heap.  we add on a little padding and make sure its word (4 byte) aligned.
        }
    }
#endif
    RESET_WATCHDOG_TIMER();

    //Init the HEAP********************************************************************
    HHPHeapInit(HEAP_START,(UINT) ((HEAP_END - SizeOfAreaToExcludeFromHeap)- HEAP_START));
    // c library etc since there is no main()
    setlocale(LC_ALL,"C"); // this is the default but since we do no pre main stuff make sure its set
    _fp_init();
    __ieee_status(FE_IEEE_MASK_ALL_EXCEPT,FE_IEEE_MASK_ALL_EXCEPT);
    __ieee_status(FE_IEEE_MASK_INEXACT,0);
    //DEBUG_puts("APP-- DATE:"__DATE__" Time: "__TIME__"\r\n");

    //Interrupts and Threads************************************************************
    InterruptInit();
    InitOsTimer();
    InitializeTheadSystem(NULL);
    EnableFastAndNormalInterrupts();    
    InitSpi(2,NULL);		// init spi 2 with default values

    //FLASH DISK***********************************************************************
    initflashdisk(true);	// call with false to skip flashing app downloaded from boot
    //Give the Copy of App, if there is one, back to the Heap***************************
    if (SizeOfAreaToExcludeFromHeap != 0)
    {
        BYTE * AppStart=(APPLICATION_COMPRESSOR_LOCATION)?APPLICATION_COMPRESSOR_LOCATION: (BYTE *) COMPRESSED_APPLICATION_LOCATION;
        HHP_AddMemoryToHeap(AppStart,SizeOfAreaToExcludeFromHeap);
    }
    RESET_WATCHDOG_TIMER();
    
    //Device Specific *********************************************************************
	CHardwareToken::CreateHardwareToken();
	
	Init_PowerPins();	//Initializes PSMODE and uP_Wake GPIO Pins
	BeepInit();		// indicators
	IndicatorTfb::CondInit();
	 
    if( HardwareToken.IAmOEMJade() )
    {
		PSOC_Port_Init();
		reset_psoc();		   
	}

    Timer3_Init();

    // interface initialization -- helpers and base io manager
    CBarcodeManager::CreateBarcodeManager();
    PrepareForStdInterfaces_ProdSpecific(); // TODO:  we have this because of CWedgeHal::CreateWedgeHal().  Look to see if this can be done when the kyboard wedge interface is created
    CIoMan::CreateStdInterfaces();
    wait_til_tx_is_empty();
    RESET_WATCHDOG_TIMER();
	CreateAutoCableSelect_ProdSpecific();	

    HSTRING *ConfigurationStorage = new HSTRING(FLASH_MAX_CONFIGURATION_LENGTH * 2 + 1, true);	// Temporary storage for configuration data
    HSTRING *ConfigurationStorage1 = new HSTRING(FLASH_MAX_CONFIGURATION_LENGTH * 2 + 1, true);
    CreateObjectsThatMenuingUses_ProdSpecific();
    SetupProductConfiguration(ConfigurationStorage);
    SetupReaderConfiguration();

    CEci::CreateEci();
    UpdateDynamicMenuSettings();			// TermID, codepages, baudrates and more

    CreateScriptInterpreter_ProdSpecific();
    ReadWorkingAndDefaultConfigurations(ConfigurationStorage,ConfigurationStorage1);
    UpdateAfterReadWorkingAndDefaultConfiguration(ConfigurationStorage);
    LoadUsageStatistics();  //just after Readerconfig creation.    

    //		***** If you need to default the whole menu, then uncomment the next 2 lines.
    //		ConfigurationStorage->Copy("DEFALT.");
    //		ReaderConfiguration->Menu(ConfigurationStorage);

    // **** Because we reversed IMGBOT and IMGTOP, which could result in both being set to zero, for the time being we're checking to see if they're
    // **** equal and if so, we're setting IMGBOT to 479.
    if(ReaderConfiguration->TransmitImageWindowTop == ReaderConfiguration->TransmitImageWindowBottom)
    {
        ConfigurationStorage->Copy("040825479.");
        ReaderConfiguration->Menu(ConfigurationStorage);
        ConfigurationStorage->Copy("$.");
        ReaderConfiguration->Menu(ConfigurationStorage);
		EncodeBuffer("WorkingConfig",ConfigurationStorage->Char,ConfigurationStorage->Size);
        WriteAndCloseFile("WorkingConfig",ConfigurationStorage->Char,ConfigurationStorage->Size);
    }

#if defined( NO_FLASH )
    ReaderConfiguration->FlashingEnabled = false;
#endif

    // Mark all menu settings in the default and working tables as being up to date.
    ReaderConfiguration->CustomerDefaultsUpToDate();
    ReaderConfiguration->WorkingValuesUpToDate();
    ReaderConfiguration->ActiveValuesUpToDate();
    delete ConfigurationStorage;
    delete ConfigurationStorage1;

	/* NGOEM-199: Remove TERMID0 for NGOEM USB (-213/-215) in if RS232 is locked */
	if(HardwareToken.IAmOEM() && (RS232_ONLY != GET_CONFIG_SETTING(iInterfacePopulation))){
		if(!GET_SETTING(RS232Unlock)){
			int piTempTerms[400];
			int iNumberOfTempTerms;

			iNumberOfTempTerms=CollectInterfaceUSBTerminalIds(piTempTerms);	//query the interface table	
			ReaderConfiguration->ChangeNumericList("060500", piTempTerms, iNumberOfTempTerms, 3);	//update the menu
		}
	}

    //WedgeHal.MakeInterfaceIoBenign();
    //	"FACTON" is used by manufacturing to put a unit in 'Semi-permanent' factory test mode...
    //	 when enabled on power-up, PlugAndPlayFactoryTest() is called during initialization.

    if (BOOT_TO_APP_REQUEST == REQUEST_232 ||GET_SETTING(FactoryTestMode))
    {
        PlugAndPlayFactoryTest();
    }


    if(!(HardwareToken.IAmOEM()))
	{
   	 	RAM_SET_ACTUAL( UsbHighspEnable, 0);
	}

    ProcessAnyFirmwareUpdate();

#if defined(DISABLE_AUTO_CBL)
	CreateManualCableSelect_ProdSpecific();
#endif

    //open the interface with the menued parameters	
	UpdateOpenInterfaceID();
    ReconfigureInterface();

    // Perform communication and transfer status monitoring abstraction for HMODEM and other classes that require it.
    ComDevice = new MatrixHComm();
    TransferStatusMonitor = new MatrixHTransferStatus();
//    SYNCommandParser = new SYN_COMMAND_PARSE * [ MAX_SPP_PORTS + 1 ];
//    for( int NextInterfaceToCheck = 0; NextInterfaceToCheck < ( MAX_SPP_PORTS + 1 ); ++NextInterfaceToCheck )
    {
//        SYNCommandParser[ NextInterfaceToCheck ] = new SYN_COMMAND_PARSE( SIZEOF_HIGHLEVEL_PARSE_BUFFER );
    }
//    SYNFECommandHandler = new MATRIX_SYN_FE_COMMAND_HANDLER(ComDevice, TransferStatusMonitor);

//    ESCCommandParser = new ESC_COMMAND_PARSE( SIZEOF_HIGHLEVEL_PARSE_BUFFER );
//    OWSYNCommandParser = new SYN_COMMAND_PARSE( SIZEOF_HIGHLEVEL_PARSE_BUFFER );
//	g_pRemSynCommandParser = new SYN_COMMAND_PARSE();

    STXCommandParser = new STX_COMMAND_PARSE * [ MAX_SPP_PORTS + 1 ];
    for( int NextInterfaceToCheck = 0; NextInterfaceToCheck < ( MAX_SPP_PORTS + 1 ); ++NextInterfaceToCheck )
    {
        STXCommandParser[ NextInterfaceToCheck ] = new STX_COMMAND_PARSE( SIZEOF_HIGHLEVEL_PARSE_BUFFER );
    }
    OWSTXCommandParser = new STX_COMMAND_PARSE( SIZEOF_HIGHLEVEL_PARSE_BUFFER );
	g_pRemStxCommandParser = new STX_COMMAND_PARSE();
	
	CRemRouterHandler::CreateRouter();  // a null one will be created for RFSCAN

    MarkTime(__LINE__,"NewApplicationUpdateTime:");

    OpenScanDriver_ProdSpecific();

    // Reconcile menu setting defaults and ranges for settings dependent upon hardware configuration after hardware has been initialized.
    ReaderConfiguration->ReconcileSettingsAfterHardwareInitialization();
    RESET_WATCHDOG_TIMER();

    CDataEdit::CreateDataEditor();		// create the single edit object (theDataEditor)
    RESET_WATCHDOG_TIMER();
    CloseScanDriver_ProdSpecific();
    RESET_WATCHDOG_TIMER();
    wait_til_tx_is_empty();
    Init_ProdSpecific();
	if(HardwareToken.IAmOEM())
	{
#ifndef RFBASE
		if(!(pTrigger->IsHWorSerialTrig())) SoundPowerupBeep(); //NGOEM129
#endif
	}
	else SoundPowerupBeep();

#ifdef PLUGIN_DEV
#ifndef RFBASE
	HONPluginConfiguration::Create();
	HONPluginConfiguration::LoadAllPlugins();
#endif //endof #ifndef RFBASE
#endif //endof #ifdef PLUGIN_DEV
	// Check to see if boot firmware just stored this application firmware (in manufacturing).  We want to let the manufacturing app know
	// the update is done.  We don't send this string in boot because the fixture shuts off power once it sees the string.  This
	// firmware could be in the middle of updating a file (Working Config, for example) when power is shut off.  Even though the file system should handle this,
	// we want to be safe, so we send this string after all updates to flash are done.  However, the manufacturing application expects
	// the string at whatever baud rate it used to download the app firmware.  Also, the test fixture did not set the cable select pins
	// so we can't use theStdInterface methods because I'm told the interface won't default to RS232.  Because interfaces are running, we can't use xputstring() either.
	if ( GetFileLength(FILENAME_BOOT_UPDATE) >= 0 )
	{	// This app was just loaded via the boot firmware.
		DWORD BaudRate;
		unsigned char * pBaudRate = (unsigned char *)&BaudRate;

		// Boot stores its baud rate in the following file before it runs the new app firmware.  It's just a DWORD.
		if ( ReadAndCloseFile(FILENAME_BOOT_UPDATE, sizeof BaudRate, &pBaudRate) == sizeof BaudRate )
		{
			SetBaudRate(BaudRate);
			//DEBUG_puts("Done!!\r\n\x06");
		}
		EraseFile(FILENAME_BOOT_UPDATE);
	}

    //---------------------------------
    // Main Loop - Thread 0
    //---------------------------------
#if defined(SM3110) || defined(SM3100)|| defined(SM5130)
    bTriggerAsync = GET_SETTING(bTriggerAimerSync);
	bAimer = GET_SETTING(bAimerIndependOn);
#endif
    while(1)
    {
        RESET_WATCHDOG_TIMER();
        if (IsOkToProcessBackroundTasks())
        {
            FlashAndReSynchCustomerDefaults();
			if (deferred_reset_state == DEFERRED_RESET_PENDING)
			{
				FlashWorkingChanges();
				wait_til_tx_is_empty();

				while(IsBeepInProgress())
				{
//                  RESET_WATCHDOG_TIMER();
				}
 
 
				DISABLE_INTERRUPTS();   // to be safe, make sure I stay here and go nowhere else... after all, I want to reset!!
 
				CTimeout        DelayedResetTimer( 10000000 );
				while( !DelayedResetTimer.HasExpired() )
				{
//                  RESET_WATCHDOG_TIMER();
				}

				reset_to_boot_code_hard();
			}
            FlashWorkingChanges();
			AutoCable_Ctl_Sys_Control();
            ReSynchWorkingAndActive();

#if defined(SM3110) || defined(SM3100)|| defined(SM5130)
			if((GET_SETTING(bTriggerAimerSync) == FALSE) && (GET_SETTING(bAimerIndependOn) == TRUE))
			{
				do{WRITE_FIELD_TO_REG(0x53F9C000,29,1,1);}while(0);//Aimer always on
			}
			else if((GET_SETTING(bTriggerAimerSync) == TRUE) && (bTriggerAsync == FALSE))
			{
				do{WRITE_FIELD_TO_REG(0x53F9C000,29,1,0);}while(0);//Aimer not always off
			}
			else if((GET_SETTING(bAimerIndependOn) == FALSE) && (bAimer == TRUE))
			{
				do{WRITE_FIELD_TO_REG(0x53F9C000,29,1,0);}while(0);//Aimer not always off
			}

			bTriggerAsync = GET_SETTING(bTriggerAimerSync);
			bAimer = GET_SETTING(bAimerIndependOn);
#endif

#if defined(SM4100)
			if (GET_SETTING(IRorTrigger)==1)
			{
				HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT18_LSH,GPIO_BIT18_WID,0x1);
			}
			else
			{
				HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT18_LSH,GPIO_BIT18_WID,0x0);
			}
#endif
			ProcessEngineData();
            DoPoll_ProdSpecific();
			ManageUsageStatistics();
            ReturnToForgroundTask();
	    	PluginsAPIExtend();
        }
        ManageSystemPower();
    }
}	//endof C_Entry



