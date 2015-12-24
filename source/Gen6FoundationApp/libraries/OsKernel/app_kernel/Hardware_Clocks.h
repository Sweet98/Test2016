/**************************************************************************************/
//
// HIM 48X0G GENERATION 6 SOFTWARE
//
//  Date:       10/31/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: app_kernel/Hardware_Clocks.h $
// $Revision: 1.2 $
// $Date: 2009/03/30 15:22:01EDT $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// HEADER:  Hardware_Clocks.h
//
//  This module contains a collection of low level functions that control the
//		clocks for power management.
//
//------------------------------------------------------------------------------

#ifndef HARDWARE_CLOCKS_H_
#define HARDWARE_CLOCKS_H_

typedef struct
{
	int MF1;
	int MFN;
	int MFD;
	int PD;
} PLL_VALUES;


#define MCU0MHZ  			0
#define MCU10MHZ	      1
#define MCU20MHZ       	2
#define MCU30MHZ       	3
#define MCU40MHZ       	4
#define MCU50MHZ  		5
#define MCU60MHZ	      6
#define MCU70MHZ       	7
#define MCU80MHZ       	8
#define MCU90MHZ       	9
#define MCU100MHZ       10
#define MCU110MHZ       11
#define MCU120MHZ       12
#define MCU130MHZ       13
#define MCU140MHZ       14
#define MCU150MHZ       15
#define MCU160MHZ       16
#define MCU170MHZ       17
#define MCU180MHZ       18
#define MCU190MHZ       19
#define MCU200MHZ       20
#define MAXMCUFREQ		MCU200MHZ

// For now, simple table with 10MHz delta
//  array index equals the 10s column, so 0 =100MHz, 1 = 110MHz, ... 5 = 150MHz, ...10 = 200MHz
// Ok, I screwed this up the first time, because I took optimized values and forgot to take into account the dynamic
// range of the various entries:
//	MF1 bits 10 -13 so 4 bits, 0 to 15 	(0x0 to 0xf)
// MFN bits 0-9 so 10 bits, 0 to 1023 (0x0 to x3ff)
// MFD bits 16 - 25, so 10 bits, 0 to 1022 (0x0 to 0x3fe, 0x3ff is RESERVED)
// PD bits 26 -29, so 4 bits, 0 to 15
const PLL_VALUES Simple_Pll_Table [] = {
	{0,0,0,0},      //0MHz with 32.768KHz crysal
	{9,336,625,15},      //10MHz with 32.768KHz crysal 	((20/2))  	//leave it at 20 *can't calculate freq < 16MHz in control reg* and set PRESC to div / 2
	{9,336,625,15},      //20MHz with 32.768KHz crysal 	((40/2))
	{7,129,845,7},      //30MHz with 32.768KHz crysal  ((60/2))
	{9,336,625,7},      //40MHz with 32.768KHz crysal  ((80/2))

	{5,583,606,3},      //50MHz with 32.768KHz crysal 	((100/2))
	{7,129,845,3},      //60MHz with 32.768KHz crysal  ((120/2))
	{8,335,971,3},      //70MHz with 32.768KHz crysal  ((140/2))
	{9,336,625,3},      //80MHz with 32.768KHz crysal  ((160/2))
	{5,297,814,1},      //90MHz with 32.768KHz crysal
	{5,583,606,1},      //100MHz with 32.768KHz crysal
	{6,510,916,1}, 		//110
	{7,129,845,1},     	//120
	{7,670,894,1},     	//130
	{8,335,971,1},     	//140
	{8,920,977,1},     	//150 DEFAULT
	{9,336,625,1},     	//160
	{10,130,978,1},     	//170
	{10,551,755,1},     	//180
	{11,205,630,1},     	//190
	{11,757,821,1}     	//200

};

void Initialize_Clocks(void);
void switch_to_async_mode(void);
void switch_to_fastbus_mode(void);
void RestoreCoreClockMode(int);
void set_MCU_PLL(PLL_VALUES *PllValues);
void switch_to_192MHZ_async_mode(void);
void switch_to_150MHZ_async_mode(void);
void crank_up_clocks(int MaxMCUSpeed = 399,int BClckDivisor=3);


#endif /*HARDWARE_CLOCKS_H_*/


