//======================================================================================
// UsageStatistics.cpp
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/usage_statistics.cpp $
// $Revision: 1.8 $
// $Date: 2011/11/18 17:12:18EST $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Module Description:
//    Impliments infrastructure for saving statistics about operation of the scanner.
// 	Most of the statistics are accumulated by statements scattered throughout the code.
//		The Statistics are stored in working menu settings which have been declared exempt from
// 	defaulting.  One statistic 'TimesProdCfgFlashed' is stored in the product config
//	   settings and is, and only can be, modified by product config. Each time the statistics are
// 	saved the scanner will hesitate for one second.
//
//		NOTE - By default Statistics are always accumulated but not stored.  They are only stored
//				 when USEENA is set to 1.
//
//		USEENA - 0 disallow, 1 allow - saving of statistics
//		USEDMP - Display the statistics
//		USERST - Reset the statistics except for USETXT and TimesProdCfgFlashed
//		USETIM - Set the interval in minutes between saves
//		USETXT - User field to store descriptive information about a unit,customer,site,etc.
//
//---------------------------------------------------------------------------

#define UsageStat
#include "product.h"
#include "ProjectRevision.h"
#include "ioman.h"
#include "usage_statistics.h"
#include "db_time.h"
#include <stdio.h>
#include "ReaderConfigurationStorage.h"		// READER_CONFIGURATION_STORAGE interface definition
#include "main_support_functions.h"
#include "menu.h"
#include "productconfiguration.h"
#include "ReaderConfiguration.h"
#include "HardwareMacros.h"
#include "iMX25_WDOG.h"
#include "timeout.h"
#include "db_sharedtable.h"
#include "HSTRING.h"
#include "HardwareToken.h"

CTimeoutmS* pTimer;


void CStatTimeSec_MsInc::UpdateTime(void)
{
	m_TotalTime += m_IncrementalTime/1000;
	m_IncrementalTime = m_IncrementalTime % 1000;
}



// Executed very early in code, before menu table is instantiated
void InitUsageStatistics()
{
	UsageStatistics.m_TfbOnTime.clear();
	UsageStatistics.StartPowerTime=-1;
	UsageStatistics.StartAimTime=-1;
	UsageStatistics.StartIllumTime=-1;
	UsageStatistics.StartDozeTime=-1;
	UsageStatistics.StartDozeLowTime=-1;
	UsageStatistics.StartTriggerTime=-1;

	UsageStatistics.AccumTrigTime=0;
	UsageStatistics.AccumAimTime=0;
	UsageStatistics.AccumIllumTime=0;
	UsageStatistics.AccumDozeTime=0;
	UsageStatistics.AccumDozeLowTime=0;

	UsageStatistics.Accum1DTime=0;
	UsageStatistics.Accum2DTime=0;
	UsageStatistics.AccumPDFTime=0;
}

// Executed after menu table is instantiated
void LoadUsageStatistics()
{
	pTimer = new CTimeoutmS(UsageStatistics.SaveInterval*1000L*60L);
	if(READ_16BIT_FIELD(WDOG_WRSR,WDOG_WRSR_TOUT_LSH,WDOG_WRSR_TOUT_WID))	//iMX25
	{
		UsageStatistics.WatchDogResets++;
	}
	else
		UsageStatistics.PowerOnCycles++;
	StoreUsageStatistics();
}

// Statistics are stored truncated to an integer but the accumulators retain the remainders
void UpdateUsageStatistics()
{
	if ( GET_CONFIG_SETTING(m_TfbPop) ) UsageStatistics.m_TfbOnTime.UpdateTime();
	// Although we accumulate in uS we store in seconds
	UsageStatistics.PowerOnTime        += (current_time()-UsageStatistics.StartPowerTime)/1000000U;
	UsageStatistics.StartPowerTime      = current_time();

	UsageStatistics.TriggerTime        += UsageStatistics.AccumTrigTime    / 1000000U;
	UsageStatistics.AccumTrigTime       = UsageStatistics.AccumTrigTime	  % 1000000U;

	UsageStatistics.IlluminationOnTime += UsageStatistics.AccumIllumTime   / 1000000U;
	UsageStatistics.AccumIllumTime      = UsageStatistics.AccumIllumTime   % 1000000U;
	UsageStatistics.AimOnTime          += UsageStatistics.AccumAimTime     / 1000000U;
	UsageStatistics.AccumAimTime        = UsageStatistics.AccumAimTime     % 1000000U;

	UsageStatistics.DozeTime           += UsageStatistics.AccumDozeTime    / 1000000U;
	UsageStatistics.AccumDozeTime       = UsageStatistics.AccumDozeTime    % 1000000U;
	UsageStatistics.DozeLowTime        += UsageStatistics.AccumDozeLowTime / 1000000U;
	UsageStatistics.AccumDozeLowTime    = UsageStatistics.AccumDozeLowTime % 1000000U;

	// Decode times are accumulated in mS  and stored in 100ths of a second
	UsageStatistics.DecodeTime1D       += UsageStatistics.Accum1DTime  / 10U;
	UsageStatistics.Accum1DTime         = UsageStatistics.Accum1DTime  % 10U;
	UsageStatistics.DecodeTime2D       += UsageStatistics.Accum2DTime  / 10U;
	UsageStatistics.Accum2DTime         = UsageStatistics.Accum2DTime  % 10U;
	UsageStatistics.DecodeTimePDF      += UsageStatistics.AccumPDFTime / 10U;
	UsageStatistics.AccumPDFTime        = UsageStatistics.AccumPDFTime % 10U;
}

void StoreUsageStatistics()
{
	UpdateUsageStatistics();
	if (UsageStatistics.UsageStatsStorageEnabled)
	{
		char MsgBuf[512];
		UsageStatistics.NumberTimesStored++;

		// TODO:  this is error prone.  We should do these one at a time.  One way is to creaet a class that we can use to add each Menu Item and value.  Once done, then we can Menu.
		sprintf
		(
			MsgBuf,
"031200%d;031300%d;031201%d;031301%d;031302%d;031202%d;031303%d;031203%d;031204%d;031304%d;031205%d;\
031206%d;031207%d;031208%d;031305%d;031306%d;031307%d;031209%d;03120A%d;03120B%d;03120C%d;\
031308%d;031309%d;03120D%d;03120E%d;03120F%d;031210%d.",
			UsageStatistics.PowerOnCycles, UsageStatistics.PowerOnTime, UsageStatistics.PowerDownCycles,
			UsageStatistics.DozeTime, UsageStatistics.DozeLowTime, UsageStatistics.StopCycles,
			UsageStatistics.StopTime, UsageStatistics.HardwareTriggers, UsageStatistics.SerialTriggers, 
			UsageStatistics.TriggerTime, UsageStatistics.TriggerTimeouts,UsageStatistics.NumberDecodes1D,
			UsageStatistics.NumberDecodes2D, UsageStatistics.NumberDecodesPDF, UsageStatistics.DecodeTime1D,
			UsageStatistics.DecodeTime2D, UsageStatistics.DecodeTimePDF, UsageStatistics.TimesAppFlashed,
			UsageStatistics.TimesBootFlashed, UsageStatistics.TimesWorkingFlashed, UsageStatistics.TimesDefaultFlashed,
			UsageStatistics.IlluminationOnTime, UsageStatistics.AimOnTime, UsageStatistics.CradleInsertions,
			UsageStatistics.EsdHibernations, UsageStatistics.WatchDogResets, UsageStatistics.NumberTimesStored);

		HSTRING	Temp(MsgBuf,true);
		if ( GET_CONFIG_SETTING(m_TfbPop) )
		{
			char MenuStr[30];
			sprintf(MenuStr, "03130A%d;", UsageStatistics.m_TfbOnTime.m_TotalTime);
			Temp.Insert(MenuStr, 0);
		}
	 	ReaderConfiguration->Menu(&Temp);
		FlashWorkingChanges();

		// just wrote working table so back off count by one
		UsageStatistics.TimesWorkingFlashed--;
		pTimer->Reset();

	}
}

bool DumpUsageInfo()
{
	UpdateUsageStatistics();

	theStdInterface.printf("\r\n");
	theStdInterface.printf("--------- SCANNER INFO -------------------\r\n");
	theStdInterface.printf(" Description           = %.60s\r\n",        UsageStatistics.Text);
	theStdInterface.printf(" Scanner type          = %.20s\r\n",        GET_SETTING(ShowProductName));
	HSTRING TempString(50,true);
	TempString.Copy(GET_CONFIG_SETTING(SerNum));
	TempString.UniCodeToSimpleAsciiFormat();
	theStdInterface.printf(" Serial number         = %.11s\r\n",	     TempString.Char);
	theStdInterface.printf("\r\n");
	theStdInterface.printf(" App code part number  = %.20s\r\n",	     GET_SETTING(ShowWARev)+2);
	theStdInterface.printf(" App code sw rev       = %.10s\r\n",        GET_SETTING(ShowSwRev)+37);
	theStdInterface.printf(" App code compile date = %.11s\r\n",        TimeDateRevisionString+10);
	theStdInterface.printf(" App code compile time = %.8s\r\n" ,        TimeDateRevisionString);
	if(BOOT_CODE_MAJOR_VERSION != 0xFF)
	{
	theStdInterface.printf(" Boot code rev         = %02d.%02d\r\n",	  (BYTE) BOOT_CODE_MAJOR_VERSION, (BYTE) BOOT_CODE_MINOR_VERSION);
	}
	else
	{
	theStdInterface.printf(" Boot code rev         = %.4s\r\n",	        BOOT_CODE_VERSION_STRING+18);
	}
	theStdInterface.printf("\r\n");
	theStdInterface.printf("--------- MAIN STATISTICS -----------------\r\n");
	theStdInterface.printf(" Power on seconds      = %d\r\n",           UsageStatistics.PowerOnTime);
	theStdInterface.printf(" Trigger pulls         = %d\r\n",           UsageStatistics.HardwareTriggers);
	theStdInterface.printf("\r\n");
	theStdInterface.printf(" 1D decodes            = %d\r\n",           UsageStatistics.NumberDecodes1D);
	theStdInterface.printf(" 2D decodes            = %d\r\n",           UsageStatistics.NumberDecodes2D);
	theStdInterface.printf(" PDF decodes           = %d\r\n",           UsageStatistics.NumberDecodesPDF);
	theStdInterface.printf(" 1D decode time Secs   = %d.%02d\r\n",      UsageStatistics.DecodeTime1D/100,  UsageStatistics.DecodeTime1D%100);
	theStdInterface.printf(" 2D decode time Secs   = %d.%02d\r\n",      UsageStatistics.DecodeTime2D/100,  UsageStatistics.DecodeTime2D%100);
	theStdInterface.printf(" PDF decode time Secs  = %d.%02d\r\n",      UsageStatistics.DecodeTimePDF/100, UsageStatistics.DecodeTimePDF%100);
	theStdInterface.printf("\r\n");
	theStdInterface.printf(" Watchdog resets       = %d\r\n",           UsageStatistics.WatchDogResets);
	theStdInterface.printf("\r\n");
	theStdInterface.printf(" Menu setting changes  = %d\r\n",  	        UsageStatistics.TimesWorkingFlashed);
	theStdInterface.printf("\r\n");
	theStdInterface.printf(" Aim on time seconds   = %d\r\n",           UsageStatistics.AimOnTime);
	theStdInterface.printf(" Ill on time seconds   = %d\r\n",           UsageStatistics.IlluminationOnTime);
	theStdInterface.printf("\r\n");
	theStdInterface.printf("--------- ADDITIONAL STATISTICS -----------\r\n");
	if ( GET_CONFIG_SETTING(m_TfbPop) )
	{
	theStdInterface.printf(" Tactile Feedback on time Secs = %d\r\n", UsageStatistics.m_TfbOnTime.m_TotalTime);
	}
	theStdInterface.printf(" Power on cycles       = %d\r\n",           UsageStatistics.PowerOnCycles);
	theStdInterface.printf(" Power down cycles     = %d\r\n",           UsageStatistics.PowerDownCycles);
	theStdInterface.printf(" Doze seconds          = %d\r\n",           UsageStatistics.DozeTime);
	theStdInterface.printf(" Doze low seconds      = %d\r\n",           UsageStatistics.DozeLowTime);
	theStdInterface.printf(" Stop cycles           = %d\r\n",           UsageStatistics.StopCycles);
	theStdInterface.printf(" Stop seconds          = %d\r\n",           UsageStatistics.StopTime);
	theStdInterface.printf("\r\n");
	theStdInterface.printf(" Triggers serial       = %d\r\n",           UsageStatistics.SerialTriggers);
	theStdInterface.printf(" Triggered seconds     = %d\r\n",           UsageStatistics.TriggerTime);
	theStdInterface.printf(" Trigger timeouts      = %d\r\n",           UsageStatistics.TriggerTimeouts);
	theStdInterface.printf("\r\n");
	theStdInterface.printf(" App flashed count     = %d\r\n",           UsageStatistics.TimesAppFlashed);
	theStdInterface.printf(" Boot flashed count    = %d\r\n",           UsageStatistics.TimesBootFlashed);
	theStdInterface.printf(" Default flashed count = %d\r\n",           UsageStatistics.TimesDefaultFlashed);
	theStdInterface.printf(" Product cfg flash cnt = %d\r\n",           GET_CONFIG_SETTING(TimesProdCfgFlashed));
	theStdInterface.printf("\r\n");
	theStdInterface.printf(" Stats stored count    = %d\r\n",           UsageStatistics.NumberTimesStored);
	theStdInterface.printf("--------- END STATISTICS ------------------\r\n");
	theStdInterface.printf("\r\n");
	StoreUsageStatistics();

	return true;
}

bool ResetUsageInfo()
{
	InitUsageStatistics();

	UsageStatistics.StartPowerTime=current_time();
	UsageStatistics.PowerOnCycles=0;
	UsageStatistics.PowerOnTime=0; 			// Includes low power time
	UsageStatistics.PowerDownCycles=0;
	UsageStatistics.DozeTime=0;
	UsageStatistics.DozeLowTime=0;
	UsageStatistics.StopCycles=0;
	UsageStatistics.StopTime=0;

	UsageStatistics.HardwareTriggers=0;
	UsageStatistics.SerialTriggers=0;
	UsageStatistics.TriggerTime=0;
	UsageStatistics.TriggerTimeouts=0;

	UsageStatistics.NumberDecodes1D=0;
	UsageStatistics.NumberDecodes2D=0;
	UsageStatistics.NumberDecodesPDF=0;
	UsageStatistics.DecodeTime1D=0;
	UsageStatistics.DecodeTime2D=0;
	UsageStatistics.DecodeTimePDF=0;

	UsageStatistics.TimesAppFlashed=0;
	UsageStatistics.TimesBootFlashed=0;
	UsageStatistics.TimesWorkingFlashed=0;
	UsageStatistics.TimesDefaultFlashed=0;

	UsageStatistics.IlluminationOnTime=0;
	UsageStatistics.AimOnTime=0;

	UsageStatistics.CradleInsertions=0;
	UsageStatistics.EsdHibernations=0;

	UsageStatistics.WatchDogResets=0;

	UsageStatistics.NumberTimesStored=0;

	StoreUsageStatistics();
	return true;
}

void ManageUsageStatistics()
{
	if (pTimer->HasExpired())
	{
		StoreUsageStatistics();
	}
}


