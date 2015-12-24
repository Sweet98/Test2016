#ifndef UsageStatisticsH
#define UsageStatisticsH


void InitUsageStatistics();
void LoadUsageStatistics();
void UpdateUsageStatistics();
void StoreUsageStatistics();
void ManageUsageStatistics();
bool DumpUsageInfo();
bool ResetUsageInfo();

#define SIZE_OF_USE_TEXT	60


class CStatTimeSec_MsInc
{
public:
	void clear(void){m_TotalTime = 0;m_IncrementalTime = 0;} // Note:  UsageStatistics is a global object and we don't have an adedquate C++ environment to ensure a contstructor gets called in this instance, so user MUST call clear() before first use.
	void AddTime(int TimeMs){m_IncrementalTime += TimeMs;}
	void UpdateTime(void);

	int m_TotalTime;  // we make this public so Menu system can access it

protected:
	unsigned int m_IncrementalTime;
};

#ifndef UsageStat
extern
#endif
struct{
	// The highest number we can keep is 0x7fffFFFF = 2,147,483,647
	// The times are in seconds therefore we can accumulate up to 2,147,483,647/86400/365 = 68 years
	int PowerOnCycles;
	int PowerOnTime; 			// Includes low power time
	int PowerDownCycles;
	int DozeTime;
	int DozeLowTime;
	int StopCycles;
	int StopTime;

	int HardwareTriggers;
	int SerialTriggers;
	int TriggerTime;
	int TriggerTimeouts;
	
	int NumberDecodes1D;
	int NumberDecodes2D;
	int NumberDecodesPDF;

	// The decode times are in 100ths of seconds therefore we can accumulate up to 2,147,483,647/100/86400/365 = 0.68 years
	// of constant decoding
	int DecodeTime1D;
	int DecodeTime2D;
	int DecodeTimePDF;
	
	int TimesAppFlashed;
	int TimesBootFlashed;
	int TimesWorkingFlashed;
	int TimesDefaultFlashed;
	int TimesProdCfgFlashed;

	int IlluminationOnTime;
	int AimOnTime;
	CStatTimeSec_MsInc m_TfbOnTime;
	
	int CradleInsertions;	
	int EsdHibernations;
	int WatchDogResets;

	int NumberTimesStored;
	int SaveInterval;
	bool UsageStatsStorageEnabled;
	
	// The following temporary variables used to help calculate the above
	int StartPowerTime;
	int StartAimTime;
	int StartIllumTime;
	int StartDozeTime;
	int StartDozeLowTime;
	int StartTriggerTime;

	// We need these so we can accumulate in uS and add to the above in seconds
	// The highest number we can keep is 0xffffFFFF = 4,294,967,295
	// The therefore we can accumulate up to 4,294,967,295/1,000,000/3600 = 1.19 hours
	UINT AccumTrigTime;
	UINT AccumAimTime;
	UINT AccumIllumTime;
	UINT AccumDozeTime;
	UINT AccumDozeLowTime;

	// We need these so we can accumulate in mS and add to the above in 100ths of seconds
	// The highest number we can keep is 0xffffFFFF = 4,294,967,295
	// The therefore we can accumulate up to 4,294,967,295/1,000/3600/24 = 49.7 days
	UINT Accum1DTime;
	UINT Accum2DTime;
	UINT AccumPDFTime;

	// General purpose information string intended for storing reader friendly info about the scanner
	// prior to sending out to a customer, could include a date, customer name, etc.
	char Text[SIZE_OF_USE_TEXT+1];
	
} UsageStatistics;


#endif
