/**************************************************************************************/
//
// HIM 48X0G GENERATION 6 SOFTWARE
//
//  ReportStartupTiming.h
//  Date:       7/11/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX27
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Foundation_Temp/ReportStartupTiming.h $
// $Revision: 1.2 $
// $Date: 2008/11/26 17:00:02EST $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// File:  ReportStartupTiming.h
//
// This File contains the Definitions for Reporting the Startup Timing
//
//------------------------------------------------------------------------------

#ifndef REPORTSTARTUPTIMING_H_
#define REPORTSTARTUPTIMING_H_


bool ReportStartupTiming( void );
void MarkTime(int linenumber,const char * comment);
extern int TimeInfoIndex;

#endif /*REPORTSTARTUPTIMING_H_*/


