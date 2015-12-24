/**************************************************************************************/
//
// HIM 48X0G GENERATION 6 SOFTWARE
//
//  ReportStartupTiming.cpp 
//  Date:       7/11/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX27
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Foundation_Temp/ReportStartupTiming.cpp $
// $Revision: 1.2 $
// $Date: 2008/11/26 17:00:02EST $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// File:  ReportStartupTiming.cpp
//
// This File contains the Class and Function to report the Startup Timming.
//
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "ReportStartupTiming.h"
#include "platform.h"
#include "db_chrio.h"
#include "db_sharedtable.h"
#include "db_time.h"


typedef struct TimeInfo_t
			{
				unsigned int time;
				int linenum;
				const char * comment;
			}TimeInfo_tag;

TimeInfo_t TimeInfo[32];
int TimeInfoIndex=0;

void MarkTime(int linenumber,const char * comment)
{
	TimeInfo[TimeInfoIndex].time=current_time();
	TimeInfo[TimeInfoIndex].linenum=linenumber;
	TimeInfo[TimeInfoIndex].comment=comment;
	TimeInfoIndex++;
}

////////////////////////////////////////////////////////////////////////////////
//	ReportStartupTiming
//		Report startup timing information.
//

class CReportStartTiming
{
public:
	CReportStartTiming();
	size_t PrintTime(char *pBuffer, unsigned int Time);
	void SendTimeWithText(const char *pText, unsigned int Time);

protected:
	int m_LastTime;
};

CReportStartTiming::CReportStartTiming()
{
	m_LastTime = 0;
	// print a title line
	xputstring("\r\nDescription,                      absolute time,        delta time\r\n");
}

size_t CReportStartTiming::PrintTime(char *pBuffer, unsigned int Time)
{
	return sprintf(pBuffer, "%8u.%03u mSec", translate_ticks_to_ms(Time), Time % 1000);
}

void CReportStartTiming::SendTimeWithText(const char *pText,unsigned  int Time)
{
	char buffer[128];
	size_t StringSize =0;
	int Delta = Time - m_LastTime;
	if(Delta < 0)
	{
		m_LastTime = 0;
		Delta = 0;
	}
	else
	{
		m_LastTime = Time;
	}

	StringSize		+=snprintf(buffer,sizeof(buffer), "%-30s%8u.%03u mSec, %8u.%03u mSec\r\n", pText,translate_ticks_to_ms(Time), Time % 1000,translate_ticks_to_ms(Delta), Delta % 1000);
	buffer[sizeof(buffer)-1]=0;
	xputstring(buffer);
}

bool ReportStartupTiming( void )
{

	CReportStartTiming xlate;
	// here we add the details about the boot time (when we have these)

// Only report boot code times if they are sane (less than the total boot time measured by this application.
//	if( ( BootTime > BOOT_TOTAL_APPLICATION_CODE_LOAD_TIME ) && ( BOOT_TOTAL_APPLICATION_CODE_LOAD_TIME >= 0 ) )
	{
		xlate.SendTimeWithText ("Boot code main entry time:", BOOT_MAIN_ENTRY_TIME);
		xlate.SendTimeWithText ("Boot code hardware setup time:", BOOT_HARDWARE_SETUP_TIME);
		xlate.SendTimeWithText ("Boot code memory check time:", BOOT_MEMORY_CHECK_TIME);
		xlate.SendTimeWithText ("Boot code entry point time:", BOOT_ENTRY_POINT_INFORMATION_VALIDATION_TIME);
		xlate.SendTimeWithText ("Boot code app copy time:", BOOT_APPLICATION_COPY_TIME);
		xlate.SendTimeWithText ("Boot code checksum time:", BOOT_CHECKSUM_VALIDATION_TIME);
		xlate.SendTimeWithText ("Boot code CRC time:", BOOT_CRC_VALIDATION_TIME);
		xlate.SendTimeWithText ("Boot code total app load time:", BOOT_TOTAL_APPLICATION_CODE_LOAD_TIME);
	}
	for (int i=0;i<TimeInfoIndex;i++)
	{
		xlate.SendTimeWithText (TimeInfo[i].comment,TimeInfo[i].time);
	}
	return true;

} // ReportStartupTiming


