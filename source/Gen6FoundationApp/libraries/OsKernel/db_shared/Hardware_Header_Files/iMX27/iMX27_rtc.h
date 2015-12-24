/**************************************************************************************/
//
// HIM 48X0G GENERATION 6 SOFTWARE
//
//  HARDWARE DEFINITION FILE 
//  Date:       5/13/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX27
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: db_shared/Hardware_Header_Files/iMX27/iMX27_rtc.h $
// $Revision: 1.1 $
// $Date: 2008/09/22 16:35:09EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  imx27_rtc.h
//
// Provides definitions for the Real Time Clock (RTC)
// Refer to Chapter 33 Page 1233 of the iMX27 Reference Manual
//
// Address Range: $1000_7000 to $1000_7FFF   
//
//------------------------------------------------------------------------------
#ifndef IMX27_RTC_H_
#define IMX27_RTC_H_
#include "Hardware_Width_Defines.h"		//Used for the Mask Width
//------------------------------------------------------------------------------
// Registers
//------------------------------------------------------------------------------
#define RTC_BASE_ADDR            0x10007000
#define RTC_HOURMIN              (RTC_BASE_ADDR+0x00)       // 32bit rtc hour/min counter reg
#define RTC_SECOND               (RTC_BASE_ADDR+0x04)       // 32bit rtc seconds counter reg
#define RTC_ALRM_HM              (RTC_BASE_ADDR+0x08)       // 32bit rtc alarm hour/min reg
#define RTC_ALRM_SEC             (RTC_BASE_ADDR+0x0C)       // 32bit rtc alarm seconds reg
#define RTC_RTCCTL               (RTC_BASE_ADDR+0x10)       // 32bit rtc control reg
#define RTC_RTCISR               (RTC_BASE_ADDR+0x14)       // 32bit rtc interrupt status reg
#define RTC_RTCIENR              (RTC_BASE_ADDR+0x18)       // 32bit rtc interrupt enable reg
#define RTC_STPWCH               (RTC_BASE_ADDR+0x1C)       // 32bit rtc stopwatch min reg
#define RTC_DAYR                 (RTC_BASE_ADDR+0x20)       // 32bit rtc days counter reg
#define RTC_DAYALARM             (RTC_BASE_ADDR+0x24)       // 32bit rtc day alarm reg
#define RTC_TEST1                (RTC_BASE_ADDR+0x28)       // 32bit rtc test reg 1
#define RTC_TEST2                (RTC_BASE_ADDR+0x2C)       // 32bit rtc test reg 2
#define RTC_TEST3                (RTC_BASE_ADDR+0x30)       // 32bit rtc test reg 3

//------------------------------------------------------------------------------
// REGISTER FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
#define RTC_HOURMIN_MINUTES_LSH			0
#define RTC_HOURMIN_HOUR_LSH			8

#define RTC_SECOND_SECONDS_LSH			0

#define RTC_ALRM_HM_MINUTES_LSH			0
#define RTC_ALRM_HM_HOUR_LSH			8

#define RTC_ALRM_SEC_SECONDS_LSH		0

#define RTC_RTCCTL_SWR_LSH				0
#define RTC_RTCCTL_GEN_LSH				1
#define RTC_RTCCTL_XTL_LSH				5
#define RTC_RTCCTL_EN_LSH				7

#define RTC_RTCISR_SW_LSH				0
#define RTC_RTCISR_MIN_LSH				1
#define RTC_RTCISR_ALM_LSH				2
#define RTC_RTCISR_DAY_LSH				3
#define RTC_RTCISR_1HZ_LSH				4
#define RTC_RTCISR_HR_LSH				5
#define RTC_RTCISR_2HZ_LSH				7
#define RTC_RTCISR_SAM0_LSH				8
#define RTC_RTCISR_SAM1_LSH				9
#define RTC_RTCISR_SAM2_LSH				10
#define RTC_RTCISR_SAM3_LSH				11
#define RTC_RTCISR_SAM4_LSH				12
#define RTC_RTCISR_SAM5_LSH				13
#define RTC_RTCISR_SAM6_LSH				14
#define RTC_RTCISR_SAM7_LSH				15

#define RTC_RTCIENR_SW_LSH				0
#define RTC_RTCIENR_MIN_LSH				1
#define RTC_RTCIENR_ALM_LSH				2
#define RTC_RTCIENR_DAY_LSH				3
#define RTC_RTCIENR_1HZ_LSH				4
#define RTC_RTCIENR_HR_LSH				5
#define RTC_RTCIENR_2HZ_LSH				7
#define RTC_RTCIENR_SAM0_LSH			8
#define RTC_RTCIENR_SAM1_LSH			9
#define RTC_RTCIENR_SAM2_LSH			10
#define RTC_RTCIENR_SAM3_LSH			11
#define RTC_RTCIENR_SAM4_LSH			12
#define RTC_RTCIENR_SAM5_LSH			13
#define RTC_RTCIENR_SAM6_LSH			14
#define RTC_RTCIENR_SAM7_LSH			15

#define RTC_STPWCH_CNT_LSH				0

#define RTC_DAYR_DAYS_LSH				0

#define RTC_DAYALARM_DAYSAL_LSH			0
//------------------------------------------------------------------------------
// REGISTER FIELD WIDTHS
//------------------------------------------------------------------------------
#define RTC_HOURMIN_MINUTES_WID			Width_6
#define RTC_HOURMIN_HOUR_WID			Width_5

#define RTC_SECOND_SECONDS_WID			Width_6

#define RTC_ALRM_HM_MINUTES_WID			Width_6
#define RTC_ALRM_HM_HOUR_WID			Width_5

#define RTC_ALRM_SEC_SECONDS_WID		Width_6

#define RTC_RTCCTL_SWR_WID				Width_1
#define RTC_RTCCTL_GEN_WID				Width_1
#define RTC_RTCCTL_XTL_WID				Width_2
#define RTC_RTCCTL_EN_WID				Width_1

#define RTC_RTCISR_SW_WID				Width_1
#define RTC_RTCISR_MIN_WID				Width_1
#define RTC_RTCISR_ALM_WID				Width_1
#define RTC_RTCISR_DAY_WID				Width_1
#define RTC_RTCISR_1HZ_WID				Width_1
#define RTC_RTCISR_HR_WID				Width_1
#define RTC_RTCISR_2HZ_WID				Width_1
#define RTC_RTCISR_SAM0_WID				Width_1
#define RTC_RTCISR_SAM1_WID				Width_1
#define RTC_RTCISR_SAM2_WID				Width_1
#define RTC_RTCISR_SAM3_WID				Width_1
#define RTC_RTCISR_SAM4_WID				Width_1
#define RTC_RTCISR_SAM5_WID				Width_1
#define RTC_RTCISR_SAM6_WID				Width_1
#define RTC_RTCISR_SAM7_WID				Width_1

#define RTC_RTCIENR_SW_WID				Width_1
#define RTC_RTCIENR_MIN_WID				Width_1
#define RTC_RTCIENR_ALM_WID				Width_1
#define RTC_RTCIENR_DAY_WID				Width_1
#define RTC_RTCIENR_1HZ_WID				Width_1
#define RTC_RTCIENR_HR_WID				Width_1
#define RTC_RTCIENR_2HZ_WID				Width_1
#define RTC_RTCIENR_SAM0_WID			Width_1
#define RTC_RTCIENR_SAM1_WID			Width_1
#define RTC_RTCIENR_SAM2_WID			Width_1
#define RTC_RTCIENR_SAM3_WID			Width_1
#define RTC_RTCIENR_SAM4_WID			Width_1
#define RTC_RTCIENR_SAM5_WID			Width_1
#define RTC_RTCIENR_SAM6_WID			Width_1
#define RTC_RTCIENR_SAM7_WID			Width_1

#define RTC_STPWCH_CNT_WID				Width_6

#define RTC_DAYR_DAYS_WID				Width_15

#define RTC_DAYALARM_DAYSAL_WID			Width_15


//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------
#define RTC_ENABLED			1
#define RTC_DISABLED		0


#endif /*IMX27_RTC_H_*/


