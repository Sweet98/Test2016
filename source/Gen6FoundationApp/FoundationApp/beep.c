/* ==============================================================================
   Matrix Beeper Module Source
   ==============================================================================

   $RCSfile: Gen6FoundationApp/FoundationApp/beep.c $
   $Revision: 1.61 $
   $Date: 2011/11/18 17:02:38EST $

   ==============================================================================

   ============================================================================== */

#define _BEEP_C_
#include <stdio.h>		/* standard I/O functions					*/
#include <string.h>		/* string functions							*/
#include <ctype.h>		/* character typing functions				*/
#include <stdlib.h>		/* standard library functions				*/

#include "beep.h"
#include "menu.h"
#include "ioman.h"
#include "readerconfiguration.h"
#include "PWM_Driver.h"
#include "OsTimer.h"
#include "IndicatorLEDs.h"
#include "IndicatorTfb.h"
#include "thread.h"
#include "interrupts.h"
#include "product.h"
#include "MainHHPLProc.h"
#include "HardwareToken.h"
#include "PluginGpioInterface.h"
#include "iMX25_PWM.h"
#include "ProductConfiguration.h"
#ifdef FLASH_LEDS
#include "Flash_Illumination_Control.h"
#endif
#include "HardwareMacros.h"
#include "iMX25_GPIO.h"	//GPIO Defines
#include "iMX25_IOMUX.h"


static threadhandle_t PWM1_Service_IRQ(void *);
static threadhandle_t PWM2_Service_IRQ(void *);
static void PWM_Interrupt_Init(void);

#define BEEP_PWM_DIV 240
#define PWM_CLOCK (240000000/BEEP_PWM_DIV)

// Allow a sequence to be repeated by putting REPEAT_SEQUENCE in the duration position.  Everything following will be repeated indefinitely
// until outside intervention causes sequence processing to terminate.  StartOfRepeatSequence keeps track of the beginning of the section to
// be repeated.
#define	REPEAT_SEQUENCE			0xFFFFFFFF
#define END_OF_SEQ	0

//LED/TFB Defines
#define NO_LED 0	            // These have to be Even numbers!  The odd value is used to select OFF timing. dmh
#define GOOD_READ_LED 2
#define ERROR_LED 4
#define TFB_IND 6

#define CLICKS_PER_QUEUE 4
#define TFB_IDX_DUR	1

unsigned int *StartOfRepeatSequence = 0;

bool bResetBeeper=false;
unsigned int cSynchLED=false;
bool bMenuOverRide=false;
bool bShowRxLights = true;
bool AllowPowerUpBeep = true;
bool PWM_Signal_State = false;

const unsigned int volume_map[]=
    {
        0,5,30,100
    };

// The beeper sequences
//	MemList 						glMemHead; 	/* head of memory free list */
// TODO:  the BeepBuffer array size is 98, but we always logically AND the read and write index with 0x3f (63)--fix this
unsigned int *BeepBuffer[98]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                              0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                              0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned int WriteIndex=0, ReadIndex=0;
unsigned int *pIntoBeepSeq=0;
unsigned int beep_duration=0;
//The GOODREAD and ERROR LED control is now maintained by the beeper
//A frequency 100 or above will be an audible sound (provided the beeper can create the sound)
//A frequency 100 or above will use the LED specified at the first char of the sequence
//A for each silent pause, use one of the LED defines as the frequency.
// unsigned int ExampleSeq[] = {audible LED sync (LED_DEFINE), duration of next freq (mS), audible freq (Hz),
//								duration (mS), silent freq (LED_DEFINE), end of string (0x00)};
// unsigned int StandardClickSeq[] = {NO_LED, 10, 200, 10, NO_LED, 0x00};
// This sequence can be read as 10mS sound at 200Hz with no led, then 10mS silence with no led

// NOTE the beeper duration has a resolution of 10ms.  The duration is controlled by the heartbeat interrupt which is 10ms
//	unsigned int StandardBeepSeq[]	={LED Synchronized?,mS (duration),frequency hz,end of string (0x00)};
const unsigned int StandardBeepSeq[]=
    {
        NO_LED,160,2720,0x00
    };
 unsigned int StandardTfbSeq[]=
    {
        NO_LED, 200, TFB_IND, END_OF_SEQ
    };
const unsigned int BeepBipSeq[]=
    {
        NO_LED,60,2720,0x00
    };
const unsigned int StandardClickSeq[]=
    {
        NO_LED,10,200,10,NO_LED,0x00
    };
//const unsigned int DeadBeepSeq[]=	{NO_LED,50,NO_LED,50,NO_LED,0x00};
//unsigned int GoodReadBeepSeq[]=	{GOOD_READ_LED,80,3250,60,NO_LED,0x00}; // no longer used--see GoodReadBeepSeq_NoDelay.  I left this in (commented out) for those used to seeing this one to help demonstrate what changed.
unsigned int GoodReadBeepSeq_NoDelay[]=	{GOOD_READ_LED,80,3250,0,NO_LED,0x00}; // no intra sequence delay at end
unsigned int GoodReadBeepSeq__IntraSeqDelay[]=	{NO_LED, 60, 0, END_OF_SEQ}; // the delay, if needed, between successive beeps
unsigned int GoodReadTfbSeq_NoDelay[]= {NO_LED, 200, TFB_IND, 0, NO_LED, END_OF_SEQ}; // no intra sequence delay at end
unsigned int TfbSeq__IntraSeqDelay[]= {NO_LED, 200, 0, END_OF_SEQ}; // the delay, if needed, between successive TFBs
const unsigned int BatchReadBeepSeq[]=
    {
        GOOD_READ_LED,80,3250,60,GOOD_READ_LED,20,100,10,NO_LED,0x00
    };
const unsigned int BatchUploadedBeepSeq[]=
    {
        GOOD_READ_LED,200,1800,0x00
    };
#ifdef STREAM_CAPTURE
const unsigned int ChargeBeepSeq[]=
	{
		GOOD_READ_LED,100,659,10,0,100,880,10,0,100,1174,10,0,100,1396,160,0,200,1174,10,0,700,1396,NO_LED,0x00
	};
#endif


// GOODREAD     --------------
// BEEPER      |    80 mS     |_____60 mS_____|
//
// LED          --------------
//             |    80 mS     |_____60 mS_____|
//
const unsigned int MenuReadBeepSeq[] =
    {
        GOOD_READ_LED,80,1000,80,1600,0x00
    };
 unsigned int MenuReadTfbSeq_NoDelay[] =
    {
        NO_LED, 200, TFB_IND, 100, NO_LED, 400, TFB_IND, END_OF_SEQ
    }; // no intra sequence delay at end
const unsigned int MenuDifferentBeepSeq[]=
    {
    	GOOD_READ_LED,400,900,400,1200,0x00
    };
const unsigned int BadReadBeepSeq[] =
    {
        ERROR_LED,160,1250,160,NO_LED,0x00
    };
const unsigned int ResetBeepSeq[] =
    {
        NO_LED,
300,142,
300,142,300,147,300,143,300,147,300,158,300,167,300,177,300,161,300,162,300,145,300,150,300,147,300,149,300,143,300,134,300,135,300,141,300,132,300,138,300,124,300,129,300,131,300,127,
300,128,300,136,300,130,300,121,300,113,300,126,300,110,300,113,300,105,300,121,300,111,300,111,300,96,300,99,300,86,300,93,300,54,300,79,300,42,300,63,300,56,300,68,300,82,
300,92,300,107,300,104,300,109,300,104,300,110,300,115,300,130,300,135,300,134,300,127,300,121,300,129,300,141,300,167,300,186,300,187,300,163,300,148,300,142,300,156,300,171,300,180,
300,181,300,173,300,158,300,140,300,149,300,142,300,157,300,157,300,160,300,149,300,147,300,149,300,140,300,141,300,142,300,137,300,138,300,138,300,128,300,120,300,113,300,112,300,105,
300,91,300,84,300,60,300,64,300,56,300,65,300,75,300,103,300,117,300,138,300,130,300,114,300,126,300,127,300,131,300,140,300,148,300,151,300,153,300,148,300,155,300,140,300,158,
300,160,300,167,300,166,300,184,300,185,300,183,300,169,300,163,300,161,300,179,300,191,300,192,300,175,300,163,300,148,300,142,300,149,300,158,300,163,300,159,300,144,300,141,300,134,
300,128,300,128,300,127,300,130,300,116,300,100,300,89,300,71,300,71,300,48,300,39,300,15,300,17,300,17,300,39,300,68,300,115,300,128,300,134,300,122,300,101,300,87,300,83,
300,104,300,127,300,142,300,141,300,136,300,126,300,123,300,119,300,126,300,135,300,142,300,147,300,140,300,141,300,145,300,152,300,163,300,161,300,162,300,157,300,169,300,162,300,153,
300,148,300,141,300,149,300,147,300,154,300,137,300,132,300,126,300,128,300,127,300,127,300,127,300,118,300,113,300,95,300,68,300,55,300,34,300,9,300,4,300,4,300,2,300,27,
300,93,300,144,300,189,300,172,300,126,300,73,300,55,300,67,300,114,300,158,300,177,300,170,300,148,300,118,300,111,300,122,300,136,300,158,300,150,300,156,300,133,300,144,300,139,
300,140,300,152,300,164,300,195,300,205,300,204,300,172,300,147,300,136,300,158,300,183,300,217,300,221,300,200,300,158,300,132,300,129,300,137,300,164,300,162,300,167,300,128,300,121,
300,88,300,78,300,42,300,21,300,0,300,8,300,0,300,24,300,95,300,221,300,251,300,214,300,166,300,46,300,44,300,25,300,114,300,161,300,209,300,190,300,151,300,111,300,100,
300,106,300,137,300,150,300,154,300,141,300,126,300,112,300,115,300,111,300,129,300,138,300,154,300,165,300,156,300,175,300,153,300,178,300,145,300,165,300,146,300,180,300,174,300,193,
300,176,300,172,300,140,300,140,300,129,300,140,300,138,300,128,300,115,300,88,300,64,300,38,300,11,300,1,300,7,300,0,300,10,300,37,300,242,300,245,300,250,300,182,300,54,
300,0,300,1,300,53,300,145,300,231,300,206,300,162,300,85,300,65,300,79,300,115,300,140,300,147,300,124,300,118,300,99,300,105,300,104,300,118,300,126,300,134,300,128,300,132,
300,117,300,121,300,126,300,162,300,196,300,226,300,230,300,203,300,154,300,133,300,131,300,167,300,195,300,218,300,206,300,175,300,131,300,106,300,91,300,104,300,106,300,107,300,73,
300,33,300,0,300,6,300,3,300,1,300,72,300,248,300,246,300,255,300,186,300,8,300,9,300,0,300,71,300,215,300,250,300,246,300,158,300,88,300,77,300,118,300,155,300,154,
300,146,300,102,300,110,300,117,300,140,300,151,300,140,300,143,300,118,300,120,300,111,300,114,300,130,300,133,300,150,300,162,300,190,300,181,300,211,300,199,300,213,300,178,300,181,
300,149,300,179,300,173,300,206,300,198,300,192,300,152,300,126,300,101,300,106,300,106,300,114,300,100,300,66,300,9,300,6,300,0,300,7,300,8,300,168,300,255,300,232,300,255,
300,68,300,0,300,3,300,10,300,99,300,240,300,243,300,223,300,120,300,62,300,83,300,113,300,121,300,126,300,79,300,85,300,87,300,132,300,148,300,149,300,123,300,110,300,84,
300,90,300,95,300,111,300,113,300,120,300,120,300,129,300,126,300,130,300,119,300,111,300,115,300,119,300,143,300,183,300,228,300,218,300,213,300,151,300,108,300,77,300,116,300,152,
300,206,300,220,300,205,300,154,300,99,300,73,300,63,300,82,300,83,300,53,300,2,300,10,300,0,300,9,300,42,300,255,300,237,300,255,300,217,300,49,300,0,300,7,300,7,
300,177,300,255,300,247,300,234,300,123,300,85,300,105,300,141,300,151,300,133,300,91,300,86,300,111,300,147,300,189,300,191,300,172,300,132,300,87,300,82,300,86,300,113,300,132,
300,155,300,154,300,155,300,144,300,140,300,124,300,124,300,128,300,134,300,148,300,148,300,148,300,147,300,155,300,179,300,220,300,232,300,216,300,167,300,127,300,120,300,140,300,181,
300,224,300,217,300,204,300,147,300,112,300,86,300,87,300,99,300,92,300,77,300,7,300,6,300,1,300,4,300,24,300,196,300,255,300,240,300,254,300,46,300,5,300,6,300,0,
300,102,300,245,300,252,300,238,300,169,300,76,300,91,300,96,300,131,300,121,300,94,300,85,300,93,300,128,300,171,300,186,300,169,300,132,300,81,300,50,300,50,300,71,300,121,
300,133,300,157,300,145,300,131,300,113,300,99,300,86,300,101,300,103,300,118,300,128,300,137,300,132,300,136,300,127,300,123,300,130,300,136,300,188,300,171,300,199,300,139,300,144,
300,104,300,137,300,147,300,191,300,180,300,179,300,134,300,128,300,95,300,113,300,114,300,120,300,98,300,60,300,4,300,5,300,4,300,0,300,43,300,209,300,255,300,240,300,255,
300,61,300,6,300,2,300,1,300,74,300,231,300,254,300,245,300,189,300,107,300,107,300,113,300,133,300,134,300,98,300,86,300,78,300,120,300,162,300,203,300,190,300,162,300,105,
300,73,300,57,300,76,300,120,300,152,300,176,300,172,300,160,300,137,300,120,300,113,300,122,300,129,300,140,300,155,300,149,300,145,300,132,300,136,300,135,300,127,300,161,300,144,
300,228,300,181,300,220,300,140,300,153,300,98,300,142,300,134,300,203,300,190,300,208,300,144,300,148,300,98,300,120,300,129,300,143,300,143,300,116,300,85,300,22,300,5,300,0,
300,11,300,8,300,216,300,252,300,254,300,247,300,168,300,0,300,15,300,0,300,36,300,152,300,248,300,250,300,204,300,125,300,95,300,89,300,103,300,106,300,101,300,70,300,72,
300,76,300,129,300,159,300,184,300,160,300,130,300,75,300,49,300,45,300,68,300,104,300,135,300,153,300,149,300,134,300,116,300,98,300,100,300,101,300,113,300,112,300,120,300,113,
300,120,300,120,300,131,300,122,300,140,300,144,300,186,300,160,300,190,300,133,300,138,300,106,300,115,300,129,300,168,300,176,300,189,300,157,300,141,300,109,300,106,300,110,300,119,
300,120,300,112,300,96,300,58,300,13,300,5,300,0,300,7,300,64,300,253,300,245,300,251,300,250,300,94,300,0,300,10,300,1,300,80,300,200,300,252,300,249,300,194,300,125,
300,106,300,96,300,128,300,128,300,121,300,97,300,92,300,97,300,136,300,170,300,204,300,189,300,157,300,99,300,69,300,45,300,84,300,114,300,162,300,177,300,181,300,153,300,133,
300,108,300,111,300,119,300,133,300,141,300,141,300,140,300,143,300,136,300,159,300,169,300,199,300,181,300,187,300,141,300,131,300,116,300,133,300,156,300,181,300,192,300,179,300,155,
300,127,300,112,300,105,300,111,300,126,300,134,300,126,300,108,300,81,300,34,300,5,300,4,300,4,300,9,300,132,300,255,300,243,300,255,300,194,300,27,300,0,300,4,300,7,
300,92,300,210,300,245,300,234,300,162,300,117,300,79,300,88,300,99,300,111,300,94,300,86,300,69,300,78,300,111,300,143,300,171,300,165,300,134,300,94,300,54,300,35,300,54,
300,93,300,139,300,157,300,163,300,140,300,114,300,88,300,85,300,91,300,117,300,127,300,130,300,131,300,112,300,137,300,144,300,201,300,176,300,196,300,135,300,119,300,101,300,106,
300,149,300,166,300,202,300,182,300,166,300,129,300,107,300,105,300,117,300,127,300,135,300,133,300,124,300,105,300,72,300,34,300,3,300,6,300,3,300,44,300,151,300,255,300,243,
300,255,300,202,300,45,300,5,300,1,300,13,300,94,300,214,300,247,300,253,300,183,300,141,300,94,300,99,300,114,300,130,300,134514000,300,-16121856,300,0,300,0,300,0,300,0,300,0,
300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,
300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,
300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,
300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,
300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,
300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,
300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,
300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,
300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,
300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,
300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,
300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,
300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,
300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,
300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,
300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,
300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,
300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,
300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,
300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,
300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,
300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,
300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,300,0,
300,0,300,0,300,0,300,0,0x00
    };//NO_LED,80,1632,80,2176,80,2720,80,3264,80,3808,0x00
const unsigned int ResetTestModeBeepSeq[] =
    {
        NO_LED,160,1632,160,2720,160,1800,240,3250,0x00
    };
const unsigned int GoodBeepSeq[]=
    {
        GOOD_READ_LED,100,4200,0x00
    };
const unsigned int BadBeepSeq[] =
    {
        GOOD_READ_LED, 100,1600,100,800,100,0x00,0x00
    };
#if (PLATFORM_CURRENT == imx25) || defined(RFSCAN)
//unsigned int ErrorBeepSeq[]	=     {ERROR_LED,300,800,300,NO_LED,0x00};
unsigned int ErrorBeepSeq_NoDelay[]	= {ERROR_LED,400,800,0,NO_LED,0x00};
unsigned int ErrorBeepSeq_IntraSeqDelay[]=	{NO_LED, 400, 0, END_OF_SEQ}; // the delay, if needed, between successive beeps
#endif
unsigned int ErrorTfbSeq_NoDelay[]	= {NO_LED, 400, TFB_IND, 400, NO_LED, END_OF_SEQ};


const unsigned int OoRWarn5Seq[] =
    {
        ERROR_LED,50,3000,30,NO_LED,30,3000,400,NO_LED,50,3000,1500,NO_LED,0x00
    };
const unsigned int OoRWarn4Seq[] =
    {
        ERROR_LED,50,3000,20,NO_LED,30,1500,1500,NO_LED,0x00
    };
const unsigned int OoRWarn1Seq[] =
    {
        ERROR_LED,500,6000,500,NO_LED,500,6000,500,NO_LED,500,6000,1500,NO_LED,0x00
    };
const unsigned int OoRWarn2Seq[] =
    {
        ERROR_LED,80,3000,30,NO_LED,80,3000,30,NO_LED,80,3000,30,NO_LED,80,3000,30,NO_LED,80,3000,1500,NO_LED,0x00
    };
const unsigned int OoRWarn7Seq[] =
    {
        ERROR_LED,50,6000,30,NO_LED,30,6000,400,NO_LED,50,6000,1500,NO_LED,0x00
    };
const unsigned int OoRWarn6Seq[] =
    {
        ERROR_LED,50,6000,20,NO_LED,30,4500,1500,NO_LED,0x00
    };
const unsigned int OoRWarn0Seq[] =
    {
        ERROR_LED,500,1500,500,NO_LED,500,1500,500,NO_LED,500,1500,1500,NO_LED,0x00
    };
const unsigned int OoRWarn3Seq[] =
    {
        ERROR_LED,60,6000,30,NO_LED,60,6000,30,NO_LED,60,6000,30,NO_LED,60,6000,30,NO_LED,60,6000,1500,NO_LED,0x00
    };
const unsigned int * OoRWarnSequences[] =
    {
        OoRWarn0Seq,OoRWarn1Seq,OoRWarn2Seq,OoRWarn3Seq,OoRWarn4Seq,OoRWarn5Seq,OoRWarn6Seq,OoRWarn7Seq
    };

const unsigned int OposBeepSeq[]   	=
    {
        ERROR_LED,160,1632,160,1250,160,800,0x00
    };
const unsigned int BTConnectBeep[]	=
    {
        GOOD_READ_LED,30,1200,30,1800,0x00
    };
const unsigned int CustomDefaultProgrammingIndicator[] =
	{	// just good read LED for 250ms then off
		GOOD_READ_LED,250,GOOD_READ_LED,0x00
	};

//unsigned int SpaceShip[] = {GOOD_READ_LED,1,110,1,120,1,130,1,140,1,150,1,160,1,170,1,180,1,190,1,200,1,210,1,220,
//	1,230,1,240,1,250,1,260,1,270,1,280,1,290,1,300,1,310,1,320,1,330,1,340,1,350,1,360,1,370,1,380,1,390,1,400,1,
//	410,1,420,1,430,1,440,1,450,1,460,1,470,1,480,1,490,1,500,1,510,1,520,1,530,1,540,1,550,1,560,1,570,1,580,1,590,
//	1,600,1,610,1,620,1,620,1,630,1,640,1,650,1,660,1,670,1,680,1,690,1,700,0x00};

//unsigned int MassaWorkin[] ={GOOD_READ_LED,200,1632,100,0,100,1632,50,0,100,1632,50,0,100,1632,300,2176,300,2720,200,0,
//								200,2176,100,0,100,2176,50,0,100,2176,50,0,100,2176,50,0,300,1904,300,1632,0x00};
//MASTA GOT ME WORKIN'
unsigned int IllumGreen135mS_Seq[] = {GOOD_READ_LED,130,GOOD_READ_LED,70,NO_LED,0x00};  //Host ACK [ESC] 1
unsigned int IllumGreen2S_Seq[] = {GOOD_READ_LED,1500,GOOD_READ_LED,500,NO_LED,0x00};   //Host ACK [ESC] 2
unsigned int IllumGreen5S_Seq[] = {GOOD_READ_LED,4500,GOOD_READ_LED,500,NO_LED,0x00};   //Host ACK [ESC] 3
unsigned int BeepLow_Seq[] = {NO_LED,160,1600,0x00}; //Host ACK [ESC] 4
unsigned int BeepMed_Seq[] = {NO_LED,160,3250,0x00};   //Host ACK [ESC] 5
unsigned int BeepHi_Seq[] = {NO_LED,160,4200,0x00};   //Host ACK [ESC] 6
unsigned int YouCantLinkSeq[] = {NO_LED,50,1800,50,1200,200,NO_LED,500,1200,0x00};
unsigned int PsucNoAquire[]  ={NO_LED,160,1000,160,700,160,1000,160,700,160,1000,0x00};
unsigned int ShowRXLeds[]	= {TRUE,30,1,30,0,0x00};
unsigned int DiagErrorSeq[]	= {TRUE,1000,1,250,0,0x00};


unsigned int FlashImageDownloadBeepSeq[] = { GOOD_READ_LED, REPEAT_SEQUENCE, 200, GOOD_READ_LED, 1950, NO_LED, 0x00 };
unsigned int FlashImageDownloadCompleteBeepSeq[] = { GOOD_READ_LED, REPEAT_SEQUENCE, 500, GOOD_READ_LED, 500, NO_LED, 0x00 };

void BeepOn(unsigned int beepfreq, unsigned int volume);

void BeepInit(void)
{
	DISABLE_INTERRUPTS();
	{
        //Configure the HWT0 (PORT3_5)
	    HAZARD_WRITE_FIELD_TO_REG(IOMUXC_SW_MUX_CTL_PAD_FEC_MDC, IOMUXC_SW_MUX_CTL_MUX_MODE_LSH, IOMUXC_SW_MUX_CTL_MUX_MODE_WID, MUX_MODE_5);	//MUX To GPIO
	    HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_FEC_MDC, 0x00000007);	//PAD Regiser, 100K ohm Pullup
        HAZARD_WRITE_FIELD_TO_REG(GPIO3_GDIR0, GPIO_BIT5_LSH, GPIO_BIT5_WID, GPIO_GDIR_OUTPUT);	//Direction Register
	    HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT5_LSH,GPIO_BIT5_WID,GPIO_HIGH); //Set High to keep the unit from shutting down
	}
	if(HardwareToken.IAmOEM() && !(HardwareToken.IAmOEMPrototype()))
	{
		if(false == DoesPluginHaveControlofCSPI1_SS0())
		{
			PWM_Init(PWM2,CSPI1_SS0,BEEP_PWM_DIV-1);
		}
	}
	else
	{
		PWM_Init(PWM1,PWM,BEEP_PWM_DIV-1);
	}

	beep_duration=0;
	pIntoBeepSeq=NULL;
	WriteIndex=0;
	ReadIndex=0;
	pIntoBeepSeq=0;
	StartOfRepeatSequence = 0;
	bMenuOverRide=false;
	bShowRxLights = true;
	AllowPowerUpBeep = true;

	Init_IndicatorLEDs();
	PWM_Interrupt_Init();

	RESTORE_INTERRUPTS();
	UnRegisterOneMsTask(ServiceBeeper);
	RegisterOneMsTask(ServiceBeeper);

}
void ConfigurePWMtoGPIOForBeepConvert(void)
{
	if(HardwareToken.IAmOEM() && !(HardwareToken.IAmOEMPrototype()))
	{
		if(false == DoesPluginHaveControlofCSPI1_SS0())
		{
			if(GET_SETTING(BeeperInvert))
			{
				HAZARD_WRITE_FIELD_TO_REG(IOMUXC_SW_MUX_CTL_PAD_CSPI1_SS0,IOMUXC_SW_MUX_CTL_MUX_MODE_LSH, IOMUXC_SW_MUX_CTL_MUX_MODE_WID, MUX_MODE_5);	//MUX To GPIO
				WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_CSPI1_SS0,0x00000007);   //Set Pad Control, MAX Drive, Fast Slew Rate
				HAZARD_WRITE_FIELD_TO_REG(GPIO1_GDIR0, GPIO_BIT16_LSH, GPIO_BIT16_WID, GPIO_GDIR_OUTPUT);	//Direction Register
				HAZARD_WRITE_FIELD_TO_REG(GPIO1_DR0,GPIO_BIT16_LSH,GPIO_BIT16_WID,GPIO_LOW); //Set High to keep the unit from shutting down
			}
			else
			{
				HAZARD_WRITE_FIELD_TO_REG(IOMUXC_SW_MUX_CTL_PAD_CSPI1_SS0,IOMUXC_SW_MUX_CTL_MUX_MODE_LSH, IOMUXC_SW_MUX_CTL_MUX_MODE_WID, MUX_MODE_5);	//MUX To GPIO
				WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_CSPI1_SS0,0x00000007);   //Set Pad Control, MAX Drive, Fast Slew Rate
				HAZARD_WRITE_FIELD_TO_REG(GPIO1_GDIR0, GPIO_BIT16_LSH, GPIO_BIT16_WID, GPIO_GDIR_OUTPUT);	//Direction Register
				HAZARD_WRITE_FIELD_TO_REG(GPIO1_DR0,GPIO_BIT16_LSH,GPIO_BIT16_WID,GPIO_HIGH); //Set High to keep the unit from shutt
			}

		}
	}
	else if(HardwareToken.IAmOEMPrototype())
	{
		if(GET_SETTING(BeeperInvert))
		{
			WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_PWM,MUX_MODE_5);	//Set Pad Mux for PWM
			WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_PWM,0x00000007);    //Set Pad Control, MAX Drive, Fast Slew Rate
			HAZARD_WRITE_FIELD_TO_REG(GPIO1_GDIR0, GPIO_BIT26_LSH, GPIO_BIT26_WID, GPIO_GDIR_OUTPUT);	//Direction Register
			HAZARD_WRITE_FIELD_TO_REG(GPIO1_DR0,GPIO_BIT26_LSH,GPIO_BIT26_WID,GPIO_LOW); //Set High to keep the unit from shutting down
		}
		else
		{
			WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_PWM,MUX_MODE_5);	//Set Pad Mux for PWM
			WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_PWM,0x00000007);    //Set Pad Control, MAX Drive, Fast Slew Rate
			HAZARD_WRITE_FIELD_TO_REG(GPIO1_GDIR0, GPIO_BIT26_LSH, GPIO_BIT26_WID, GPIO_GDIR_OUTPUT);	//Direction Register
			HAZARD_WRITE_FIELD_TO_REG(GPIO1_DR0,GPIO_BIT26_LSH,GPIO_BIT26_WID,GPIO_HIGH); //Set High to keep the unit from shutting down
		}
	}

}
void BeepOff(void)
{
	PWM_Signal_State = 0;
	ConfigurePWMtoGPIOForBeepConvert();
	if(HardwareToken.IAmOEM() && !(HardwareToken.IAmOEMPrototype()))
	{
		if(false == DoesPluginHaveControlofCSPI1_SS0())
		{
			PWM_Stop(PWM2);
		}
	}
	else
	{
		PWM_Stop(PWM1);
	}
}


void BeepOn(unsigned int Frequency, unsigned int Volume)
{
	unsigned int Period =(PWM_CLOCK/Frequency) - 2;
	if(HardwareToken.IAmOEM() && !(HardwareToken.IAmOEMPrototype()))
	{
		if(false == DoesPluginHaveControlofCSPI1_SS0())
		{
			PWM_Init(PWM2,CSPI1_SS0,BEEP_PWM_DIV-1);
			PWM_Interrupt_Init();
			PWM_Period(PWM2,Period);
			if(GET_SETTING(BeeperInvert))
			{
				PWM_Sample(PWM2,Period -Volume * Period / 200);
			}
			else
			{
				PWM_Sample(PWM2, (Volume * Period/200));
			}

		}
	}
	else
	{
		if(HardwareToken.IAmOEMPrototype())
		{
	    	PWM_Init(PWM1,PWM,BEEP_PWM_DIV-1);
			PWM_Interrupt_Init();
		}
		PWM_Period(PWM1,Period);
		if(HardwareToken.IAmOEMPrototype())
		{
			if(GET_SETTING(BeeperInvert))
			{
				PWM_Sample(PWM1,Period -Volume * Period / 200);
			}
			else
			{
				PWM_Sample(PWM1,(Volume * Period / 200));
			}
		}
		else
		{
			PWM_Sample(PWM1,(Volume * Period / 200));
		}
	}
	if (Frequency>0)
	{
		PWM_Signal_State = 1;
		Force_Green_LED(0);
		Force_Red_LED(0);
		if(HardwareToken.IAmOEM() && !(HardwareToken.IAmOEMPrototype()))
		{
			if(false == DoesPluginHaveControlofCSPI1_SS0())
			{
				PWM_Start(PWM2);
			}
		}
		else
		{
			HAZARD_WRITE_FIELD_TO_REG(PWM1_PWMCR,PWM_CR_POUTC_LSH,PWM_CR_POUTC_WID,0x1);
			PWM_Start(PWM1);
		}

	}
}



/*----------------------------------------------------------------------*
 * Beep - Sounds the beeper 'Count' times.
 *----------------------------------------------------------------------*/
void SoundStandardBeep(unsigned int Count)
{
	if (Count>0 && Count<10)
	{
		while (Count-- > 0)
			QueueBeepSequence(StandardBeepSeq);
	}
}

bool Beep(bool ind,unsigned int freq,unsigned int dur)
{
	static unsigned int scriptbeep[4];
	scriptbeep[0]=ind?1:0;			//No LED flash
	scriptbeep[1]=dur;
	scriptbeep[2]=freq;
	scriptbeep[3]=0;
	if (NULL == pIntoBeepSeq)				// if no sequence is playing
	{
		QueueBeepSequence(scriptbeep);
		return true;
	}
	else
		return false;
}


/*----------------------------------------------------------------------*
 * Beep - Queues a beeper sequence
 *----------------------------------------------------------------------*/
//limit the number of clicks added to the queue to provide
//better feedback to the end user when scanning a stacked
//code with a linear scanner

void QueueBeepSequence(unsigned int const* pBeepSeq)
{
	if (pBeepSeq != NULL)
	{
		DISABLE_INTERRUPTS();
		if(bResetBeeper)
		{
			WriteIndex=ReadIndex;	//Set the tail pointer to the head pointer
		}
		if ((pBeepSeq != StandardClickSeq) || ((WriteIndex-ReadIndex)& 0x3f)< CLICKS_PER_QUEUE)
		{
			BeepBuffer[WriteIndex++ & 0x3F] = (unsigned int *)pBeepSeq;
			if ((WriteIndex & 0x3F) == (ReadIndex & 0x3F))
			{
				ReadIndex++; // if we are overunning the buffer discard the oldest sequence
			}
		}
		bResetBeeper=false;
		RESTORE_INTERRUPTS();
	}
}


//--------------------------------------------------------------------------
//	 Service the beeper timer.
// The Beep_Buffer is a circular buffer of pointers to beep sequences.
// The Odd entries of the sequence are duration in heartbeats.
// The even entries are the frequency (0 is a rest).
// The end of the script is denoted by a duration of 0.
// The ReadIndex is always set to the sequence being played.
// The WriteIndex is always set to the next slot to be filled.
// When the indexes are equal there are no more sequences to be played.
// There is no protection for overunning the buffer, oldest sequences are simply overwritten.
//---------------------------------------------------------------------------
void ServiceBeeper(void)
{

	if (beep_duration != 0)						// if a beep is in progress
	{
		beep_duration--;						    // decrement counter and exit construct
	}
	else if (ReadIndex!=WriteIndex)				// if the buffer is not empty ... else exit construct
	{
		if (NULL == pIntoBeepSeq)				// if no sequence is playing
		{
			bResetBeeper=false;
			pIntoBeepSeq = BeepBuffer[ReadIndex & 0x3F];      // set pIntoBeepSeq to the next sequence
			cSynchLED = *pIntoBeepSeq++;
		}
		if (*pIntoBeepSeq == 0x00)				// if this is the end of a sequence?
		{
			BeepOff();
			BeeperUnLockLEDS();
			GoodReadLedOff();
			IndicatorTfb::CondOff();

			ErrorLedOff();
#ifdef FLASH_LEDS
			if(!(HardwareToken.IAmOEM()) && !(HardwareToken.IAmVoyager1400()))
			{
				Ill_HW_Enable_Aimer(FUNC_BEEPER);
				Ill_Ctl_Sys_LED_Cir_EN_CTRL(FUNC_BEEPER, 1);
			}
#endif
			// If we're supposed to restart the sequence, do it now.
			if( StartOfRepeatSequence != 0 )
			{
				pIntoBeepSeq = StartOfRepeatSequence;
			}
			else
			{
				bMenuOverRide = false;
				cSynchLED = NO_LED;
				pIntoBeepSeq = 0;
				if (++ReadIndex == WriteIndex)	  	// index and check to see if the buffer is empty
				{

					return;
				}
				pIntoBeepSeq = BeepBuffer[ReadIndex & 0x3F];		// set pIntoBeepSeq to the next sequence
				cSynchLED = *pIntoBeepSeq++;
			}
		} // endof if (*pIntoBeepSeq == 0x00).  We're done processing end of the sequence.

		if( *pIntoBeepSeq == REPEAT_SEQUENCE )
		{
			++pIntoBeepSeq;
			StartOfRepeatSequence = pIntoBeepSeq;
		}
		else
		{
			BeepOff();
#ifdef FLASH_LEDS
		if(!(HardwareToken.IAmOEM()) && !(HardwareToken.IAmVoyager1400()))
		{
			Ill_HW_Disable_Aimer(FUNC_BEEPER);
			Ill_Ctl_Sys_LED_Cir_EN_CTRL(FUNC_BEEPER, 1);
		}
#endif

			beep_duration = (*pIntoBeepSeq++);// set up and play next note divide by 10 because heardbeat interrupt is 10mS
			// set up and play next note divide by 10 because heardbeat interrupt is 10mS
			beep_duration=(GET_SETTING(BeepBipMode))?beep_duration/3:beep_duration* (GET_SETTING(BeepDuration))/100;
			int beepfreq= *pIntoBeepSeq++;
			if (beepfreq>=100)							// if this note is NOT silent (<100 Hz denotes a silent, used for LED maintenance)
			{
					unsigned int volume =volume_map[bMenuOverRide?3:GET_SETTING(BeepVolume)];
					if(volume != 0x0)
					{
#ifdef FLASH_LEDS
						if(!(HardwareToken.IAmOEM()) && !(HardwareToken.IAmVoyager1400()))
						{
							Ill_Ctl_Sys_LED_Cir_EN_CTRL(FUNC_BEEPER, 0);
						}
#endif
						BeeperLockLEDS();
						BeepOn(beepfreq, volume);
					}

					beepfreq=cSynchLED;

			}


			switch(beepfreq)
			{
			case GOOD_READ_LED:
				ErrorLedOff();
				IndicatorTfb::CondOff();
				GoodReadLedOn();
				break;
			case ERROR_LED:
				GoodReadLedOff();
				IndicatorTfb::CondOff();
				ErrorLedOn();
				break;
			case TFB_IND:
				GoodReadLedOff();
				ErrorLedOff();
				IndicatorTfb::CondOn();
				break;
			case NO_LED:
				GoodReadLedOff();
				ErrorLedOff();
				IndicatorTfb::CondOff();
				break;
			}
		}
	}
}


/*----------------------------------------------------------------------------------------
IsThisLastInQueue

  This checks to see if the provided beep sequence is the last in the beep queue.

  Parameters:
		pBeepSeq[in]:  beep sequence

  Returns:  true if pBeepSeq is the last in the beep queue, false otherwise
----------------------------------------------------------------------------------------*/
static bool IsThisLastInQueue(const unsigned int * pBeepSeq)
{
	if ( IsBeepQueEmpty() ) return false;
	int LastIndex = (WriteIndex - 1) & 0x3f;
	return (BeepBuffer[LastIndex] == pBeepSeq);
}



/*----------------------------------------------------------------------------------------
QueueEfficientSequence

  This efficiently handles the trailing delay at the end of beep sequences so that we
  only have delays when necessary.  This results in a "snappier" system.  See
  SoundGoodReadBeep() for more information.

  Parameters:
		pSeq_NoDelay[in]:  beep sequence with no delay at end

		pIntraSeqDelay[in]:  the beep sequence that becomes the delay in between beep sequences.

		NumTimes[in]:  number of times to queue the sequence

  Returns:  nothing
----------------------------------------------------------------------------------------*/
static void QueueEfficientSequence(const unsigned int * pSeq_NoDelay, const unsigned int * pIntraSeqDelay, int NumTimes)
{
	if ( NumTimes == 0 ) return;
	if ( IsThisLastInQueue(pSeq_NoDelay) )
	{	// There's already a sequence at the end of the queue with no delay--queue a delay first so this next sequence doesn't bump into it
		QueueBeepSequence(pIntraSeqDelay);
	}
	for (int i = 1; i < NumTimes; i++ )  // we want to do 1 less than the NumTimes--we do one more after this loop (we queue a sequence with no delay at the end)
	{
		QueueBeepSequence(pSeq_NoDelay);
		QueueBeepSequence(pIntraSeqDelay);
	}
	QueueBeepSequence(pSeq_NoDelay);  // we don't want an unnecessary delay at the end
}




/*----------------------------------------------------------------------------------------
CondQueueTfbSequence

  This conditionally queues the TFB (beep) sequence.  We must have a TFB  and we must be
  enumerated (relevant to USB only) in order to queue the sequence.  We queue sequences
  like the good read beep--we use a separate delay sequence to minimize the sequence time
  so the aimer can come on faster.

  We need to account for multiple TFBs (we need a delay in between), but we also don't
  want a trailing delay.  We handle these requirements like SoundGoodReadBeep()--see
  the function description for more information./

  Parameters:
		pTfbSeq[in]:  TFB sequence

		NumTimes[in]:  Number of times to queue the sequence

  Returns:  nothing
----------------------------------------------------------------------------------------*/
static void CondQueueTfbSequence(const unsigned int * pTfbSeq, int NumTimes)
{
#ifndef RFBASE
	if ( GET_CONFIG_SETTING(m_TfbPop) && (theStdInterface.DeviceIoControl(IOCTRL_AM_I_ENUMERATED) == ERR_PASS) )
	{
		QueueEfficientSequence(pTfbSeq, TfbSeq__IntraSeqDelay, NumTimes);
	}
#endif
}


/*----------------------------------------------------------------------*
 * Good Read Beep - Sounds the beeper (X) times, X is determined from a menu read.

 The following was information was gained via reverse engineering code
 and a conversation with Joe W.:

 This gets called before the data formatter and sequencing stuff.
 We have two instances where we beep on a successful barcode read.  One is
 controlled by BEPBEP, the data formatter, and sequencing (see SoundGoodReadBeep()).
 The other is not (see DecodeBeep()).  Thus, if you have a data formatter
 that throws out a barcode (and SoundGoodReadBeep() won't be called), the
 user can still be "told" a barcode was decoded.

 We need to have a delay at the end of each sequence so if GET_SETTING(BeepOnRead)
 is greater than 1, they don't bump into each other and become a single long
 beep.  However, the last sequence's delay is not necessary and unnecessarily
 keeps the aimer from coming on.  Furhtermore, for Industrial with a TFB, we
 want to turn on the TFB as soon as possible to give the user the illusion the
 beep and TFB happen simultaneously (we can't actually have them on at the same
 time becuase of the USB 500 mA current limit).  So, we queue a separate delay
 in between each sequence and leave the delay off for the last.

 However, it's possible to queue 2 or more sequences by scanning linear
 barcodes quickly.  If the last sequence now lacks the delay, the queuing of
 the next one will bump into the last.  So, to account for this, we look to
 see if the last sequence in the buffer is a GoodRead sequence.  If it is,
 we insert a delay before the next sequence.
 *----------------------------------------------------------------------*/
void	SoundGoodReadBeep(void)
{
//HAP don't need sound beep
		GoodReadBeepSeq_NoDelay[0] = GET_SETTING(GoodReadLEDMode)?GOOD_READ_LED:NO_LED;
		GoodReadBeepSeq_NoDelay[2] = GET_SETTING(GoodReadBeepMode)? GET_SETTING(GoodReadFrequency):GoodReadBeepSeq_NoDelay[0]; //get the selected frequency

		QueueEfficientSequence(GoodReadBeepSeq_NoDelay, GoodReadBeepSeq__IntraSeqDelay, GET_SETTING(BeepOnRead));
	// The TFB functions and methods are designed to handle RFBASE correctly (do nothing), but since this #ifdef was already here, I just used it.
	if ( GET_SETTING(m_bEnableGoodReadTfb) )
	{
		GoodReadTfbSeq_NoDelay[TFB_IDX_DUR] = GET_SETTING(m_TfbDuration);
		CondQueueTfbSequence(GoodReadTfbSeq_NoDelay, GET_SETTING(BeepOnRead));
	}
}



void SoundBatchReadBeep(void)
{
	QueueBeepSequence(BatchReadBeepSeq);
}

void SoundBatchUploadedBeep(void)
{
	QueueBeepSequence(BatchUploadedBeepSeq);
}

/*----------------------------------------------------------------------*
 * Error Beep - Sounds the beeper (X) times, X is determined from a menu read.
 *----------------------------------------------------------------------*/
void SoundErrorBeep(bool bIsMenuReadError/* = false*/)
{

//HAP don't need sound beep
		ErrorBeepSeq_NoDelay[2] = GET_SETTING(ErrorFrequency); //get the selected frequency
		QueueEfficientSequence(ErrorBeepSeq_NoDelay, ErrorBeepSeq_IntraSeqDelay, GET_SETTING(BeepOnError));
	if ( bIsMenuReadError ) CondQueueTfbSequence(ErrorTfbSeq_NoDelay, GET_SETTING(BeepOnError));
}

void SoundMenuBeep(void)
{
	if(GET_SETTING(BeepVolume) == 0)
		bMenuOverRide = true;
	QueueBeepSequence(MenuReadBeepSeq);
	CondQueueTfbSequence(MenuReadTfbSeq_NoDelay, 1);
}

void SoundDifferentMenuBeep(void)
{
	if(GET_SETTING(BeepVolume)==0)
		bMenuOverRide=true;
	QueueBeepSequence(MenuDifferentBeepSeq);
	CondQueueTfbSequence(MenuReadTfbSeq_NoDelay, 1);
}
/*----------------------------------------------------------------------*
 * Decode Beep - Sounds the beeper (X) times, X is determined from a menu read.

 The following was information was gained via reverse engineering code
 and a conversation with Joe W.:

 This gets called before the data formatter and sequencing stuff.
 We have two instances where we beep on a successful barcode read.  One is
 controlled by BEPBEP, the data formatter, and sequencing (see SoundGoodReadBeep()).
 The other is not (see DecodeBeep()).  Thus, if you have a data formatter
 that throws out a barcode (and SoundGoodReadBeep() won't be called), the
 user can still be "told" a barcode was decoded.
 *----------------------------------------------------------------------*/
void DecodeBeep(void)
{

//HAP don't need sound beep
		for (int Count=GET_SETTING(BeepOnDecode);Count>0;Count--)
		{
			QueueBeepSequence(StandardBeepSeq); // Note:  this sequence has no delay at the end, so we don't have to do what we did for ghe good read beep to eliminate it so the TFB starts immediately after the beep
		}
		CondQueueTfbSequence(StandardTfbSeq, GET_SETTING(BeepOnDecode));
}

/*----------------------------------------------------------------------*
 * OPOS Disable Beep - Sounds the beeper (x) times.
 *----------------------------------------------------------------------*/
void SoundOposBeep(void)
{
	QueueBeepSequence(OposBeepSeq);
}


/*----------------------------------------------------------------------*
 * NoDecode Beep - Sounds the beeper (X) times, X is determined from a menu read.
 *----------------------------------------------------------------------*/
void NoDecodeBeep(void)
{
//HAP don't need sound beep
		for (int Count=GET_SETTING(BeepOnNoDecode);Count>0;Count--)
		{
			QueueBeepSequence(BadReadBeepSeq);
		}

}

/*-----------------10/25/1999 5:06PM----------------
 * Click
 * Gives you a click sound
 * --------------------------------------------------*/
void Click(void)
{
	QueueBeepSequence( (unsigned int *)StandardClickSeq );
}

void TrigClick(void)
{
	if(GET_SETTING(bTrigBeepEnable))
		QueueBeepSequence( (unsigned int *)StandardClickSeq );
}

/*-----------------10/25/1999 5:06PM----------------
 * Beep Bip
 * --------------------------------------------------*/
void BeepBip(void)
{
	QueueBeepSequence( (unsigned int *)BeepBipSeq );
}


void SoundBTConnectBeep(void)
{
	QueueBeepSequence( (unsigned int *)BTConnectBeep );
}


//----------------------------------------------------------------
//	 Clears Beeper Buffer
//
//----------------------------------------------------------------
void ResetBeeper()
{
	bResetBeeper=true;
	StartOfRepeatSequence = 0;
}

//----------------------------------------------------------------
// Return TRUE if we have any beeps in que
//
//----------------------------------------------------------------
bool IsBeepQueEmpty()
{
	return(ReadIndex == WriteIndex);
}


void SoundPowerupBeep(void)
{
#ifndef PRODUCT_CONFIGURATION_APPLICATION
	if (IOCRET_NO!=theStdInterface.DeviceIoControl(IOCTRL_SHALL_I_BEEP_ON_POWERUP))
#endif

	{
		if(AllowPowerUpBeep)
		{
			if(GET_SETTING(bPowerUpBeepEnable)){
				if(GET_SETTING(FactoryTestMode))
					QueueBeepSequence(ResetTestModeBeepSeq);
				else
					QueueBeepSequence(ResetBeepSeq);
			AllowPowerUpBeep = false;
				}
		}
	}
}

void SoundPowerupBeepNoTest(void)
{
	if(AllowPowerUpBeep)
	{
		if(GET_SETTING(bPowerUpBeepEnable)){
			if(GET_SETTING(FactoryTestMode))
			{
				QueueBeepSequence(ResetTestModeBeepSeq);
			}
			else
			{
			QueueBeepSequence(ResetBeepSeq);
			}

		AllowPowerUpBeep = false;
			}
	}
}


void SoundBadPsocCommunication(void)
{
	if(!(HardwareToken.IAmVoyager1400()))
		QueueBeepSequence(PsucNoAquire);
}

bool IsBeepInProgress(void)
{
	return (WriteIndex&0x000f)!=(ReadIndex&0x000f);
}

void EnableRxLights(void)
{
	bShowRxLights = true;
}
void DisableRxLights(void)
{
	bShowRxLights = false;
}

void ShowRxDataLights(void)
{
	if( bShowRxLights )
	{
			ErrorLedOff();      // turn off the power indication

		if ( IsBeepQueEmpty() )
		{

				QueueBeepSequence( ShowRXLeds ); // show that we got data (RF & Serial)
		}
	}
}

void ShowDiagnosticError(void)
{
	GoodReadLedOff();		// turn off the power indication
	while( IsBeepQueEmpty() )
		QueueBeepSequence( DiagErrorSeq );	// show error condition
}

bool  SoundDifferentScanMenuBeep(void)
{
	if(GET_SETTING(BeepVolume)==0)
		bMenuOverRide=true;
	QueueBeepSequence(MenuDifferentBeepSeq);
	return true;
}
bool ScannerCantLink(void)
{
	QueueBeepSequence(YouCantLinkSeq);
	return true;
}

bool IllumGreen135mS(void)	//Host ACK [ESC] 1
{
	return ExecuteHostAckBeep('1');
}

bool IllumGreen2S(void)		//Host ACK [ESC] 2
{
	return ExecuteHostAckBeep('2');
}

bool IllumGreen5S(void)		//Host ACK [ESC] 3
{
	return ExecuteHostAckBeep('3');
}

bool BeepLow(void)         	//Host ACK [ESC] 4
{
	return ExecuteHostAckBeep('4');
}

bool BeepMed(void)         	//Host ACK [ESC] 5
{
	return ExecuteHostAckBeep('5');
}

bool BeepHi(void)        	//Host ACK [ESC] 6
{
	return ExecuteHostAckBeep('5');
}

void BlinkCustomDefaultIndicator(void)
{
	QueueBeepSequence(CustomDefaultProgrammingIndicator);
}

void DebugBeep(void)
{
	QueueBeepSequence(GoodBeepSeq);
}

void DebugBadBeep(void)
{
	QueueBeepSequence(BadBeepSeq);
}

/*-------------------------------------------------------------------------
 * ExecuteGoodReadBeepSequence
 *-------------------------------------------------------------------------*/
bool ExecuteGoodReadBeepSequence(void)
{
	return ExecuteHostAckBeep('7');
}

/*-------------------------------------------------------------------------
 * ExecuteNoReadBeepSequence
 *-------------------------------------------------------------------------*/
bool ExecuteNoReadBeepSequence(void)
{
	return ExecuteHostAckBeep('8');
}

/*-------------------------------------------------------------------------
 * GoodMenuBeep
 *-------------------------------------------------------------------------*/
bool GoodMenuBeep(void)
{
	return ExecuteHostAckBeep('a');
}

/*-------------------------------------------------------------------------
 * ErrorMenuBeep
 *-------------------------------------------------------------------------*/
bool ErrorMenuBeep(void)
{
	return ExecuteHostAckBeep('b');
}

//
//	ExecuteHostAckBeep
//		Inputs
//			BeepType - host ACK command character send from host
//
//		Returns true if command sent by host (BeepType) is recognized, false if it is not.
//
bool ExecuteHostAckBeep(unsigned char BeepType)
{
	switch (BeepType)
	{
	case 'a':
		SoundMenuBeep();
		break;
	case 'b':
		SoundErrorBeep();
		break;
	case '1':
		QueueBeepSequence(IllumGreen135mS_Seq );
		break;
	case '2':
		QueueBeepSequence(IllumGreen2S_Seq );
		break;
	case '3':
		QueueBeepSequence(IllumGreen5S_Seq );
		break;
	case '4':
		QueueBeepSequence(BeepLow_Seq );
		break;
	case '5':
		QueueBeepSequence(BeepMed_Seq );
		break;
	case '6':
		QueueBeepSequence(BeepHi_Seq);
		break;
	case '7':
		SoundGoodReadBeep();
		break;
	case '8':
#if !defined(RFBASE)
		theScriptInterpreter.HostAcknowledgedLastDataTransmission( false );
#endif

		SoundErrorBeep(); // changed to soud error beep to beep correctly no decode is controlled
		return true;
	default:
		return false;

	}
#if !defined(RFBASE)
	theScriptInterpreter.HostAcknowledgedLastDataTransmission( true );
#endif

	return true;
}


//
//	FlashImageDownloadBeep
//		Indicate that flash image download is in progress.  Always flush the beeper queue first.
//
void FlashImageDownloadBeep(void)
{
	ResetBeeper();
	DisableRxLights();
	pIntoBeepSeq = 0;
	QueueBeepSequence( FlashImageDownloadBeepSeq );
} // FlashImageDownloadBeep


//
//	FlashImageDownloadCompleteBeep
//		Indicate that flash image download is completed.  Always flush the beeper queue first.
//
void FlashImageDownloadCompleteBeep(void)
{
	ResetBeeper();
	pIntoBeepSeq = 0;
	DisableRxLights();
	QueueBeepSequence( FlashImageDownloadCompleteBeepSeq );
} // FlashImageDownloadCompleteBeep

#ifdef STREAM_CAPTURE
void SoundOOMBeep(void)
{
	QueueBeepSequence( ChargeBeepSeq );
}
#endif //STRAEM_CAPTURE

// ===  Function  ======================================================================
//
//  Name:  PWM_Interrupt_Init
//
//  Description:  Initial Setup of the Roll-Over and Compaire PWM Interrupts
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
static void PWM_Interrupt_Init(void)
{

	if(HardwareToken.IAmOEM() && !(HardwareToken.IAmOEMPrototype()))
	{
		if(false == DoesPluginHaveControlofCSPI1_SS0())
		{
			SetVector(PWM2_Service_IRQ,VEC_PWM2_INT);
			Clear_ROV_Interrupt_Signal(PWM2);
			ROV_Interrupt_Enable(PWM2, 0x1);

			Clear_CMP_Interrupt_Signal(PWM2);
			CMP_Interrupt_Enable(PWM2, 0x1);

			EnableInterruptNumber(VEC_PWM2_INT);	// Enable interrupts on PWM-2
		}
	}
	else
	{
		SetVector(PWM1_Service_IRQ,VEC_PWM_INT);
		Clear_ROV_Interrupt_Signal(PWM1);
		ROV_Interrupt_Enable(PWM1, 0x1);

		Clear_CMP_Interrupt_Signal(PWM1);
		CMP_Interrupt_Enable(PWM1, 0x1);

		EnableInterruptNumber(VEC_PWM_INT);	// Enable interrupts on PWM-1
	}

}	//endof Ill_HW_Flash_Interrupt_Init

// ===  Function  ======================================================================
//
//         Name:  PWM_Service_IRQ
//
//  Description:  PWM1 is setup to Set the output on a Rollover and Clear it on Compare,
//			This is configurable with the POUTC bits.  This function will need to be
//			updated if POUTC is changed.  Otherwise USB spec failures will occur.
//			This (PWM_Service_IRQ) function interlaces the indicator LEDs with the beeper.
//			When the PWM signal goes high the LEDs turn off.  When the PWM signal goes low
//			the LEDs turn on.  The two are interlaced to keep the current below 500mA (This
//			comes from the USB Spec) while the beeper is beeping.  When the LEDs are
//			turned on a flag is set in the Indicator LED code, this tells the interrupt
//			whether its ok to turn the LED on or not.  The Boost Enable Pin is also interlaced
//			here
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
static threadhandle_t PWM1_Service_IRQ(void *)
{
#if PLATFORM_CURRENT == imx25

	if(Read_ROV_Interrupt_Signal(PWM1))  //Check if the Roll-Over Interrupt has been triggered
	{
		PWM_Signal_State = 1;
#ifdef FLASH_LEDS
		if(!(HardwareToken.IAmOEM()) && !(HardwareToken.IAmVoyager1400()))
			Ill_Ctl_Sys_LED_Cir_EN_CTRL(FUNC_BEEPER, 0);
#endif
		Force_Green_LED(0);
		Force_Red_LED(0);
		Clear_ROV_Interrupt_Signal(PWM1);
	}
	if(Read_CMP_Interrupt_Signal(PWM1))  //Check if the Compare Interrupt has been triggered
	{
		PWM_Signal_State = 0;
#ifdef FLASH_LEDS
		if(!(HardwareToken.IAmOEM()) && !(HardwareToken.IAmVoyager1400()))
			Ill_Ctl_Sys_LED_Cir_EN_CTRL(FUNC_BEEPER, 1);
#endif
		if(IsGoodReadOn())
		{
			Force_Green_LED(1);
		}
		if(IsErrorLedOn())
		{
			Force_Red_LED(1);
		}
		Clear_CMP_Interrupt_Signal(PWM1);
	}
	return NULL;

#endif
}//endof PWM1_Service_IRQ

static threadhandle_t PWM2_Service_IRQ(void *)
{
#if PLATFORM_CURRENT == imx25

	if(Read_ROV_Interrupt_Signal(PWM2))  //Check if the Roll-Over Interrupt has been triggered
	{
		PWM_Signal_State = 1;
#ifdef FLASH_LEDS
		if(!(HardwareToken.IAmOEM()) && !(HardwareToken.IAmVoyager1400()))
			Ill_Ctl_Sys_LED_Cir_EN_CTRL(FUNC_BEEPER, 0);
#endif
		Force_Green_LED(0);
		Force_Red_LED(0);
		Clear_ROV_Interrupt_Signal(PWM2);
	}
	if(Read_CMP_Interrupt_Signal(PWM2))  //Check if the Compare Interrupt has been triggered
	{
		PWM_Signal_State = 0;
#ifdef FLASH_LEDS
		if(!(HardwareToken.IAmOEM()) && !(HardwareToken.IAmVoyager1400()))
			Ill_Ctl_Sys_LED_Cir_EN_CTRL(FUNC_BEEPER, 1);
#endif
		if(IsGoodReadOn())
		{
			Force_Green_LED(1);
		}
		if(IsErrorLedOn())
		{
			Force_Red_LED(1);
		}
		Clear_CMP_Interrupt_Signal(PWM2);
	}
	return NULL;

#endif
}//endof PWM1_Service_IRQ

// ===  Function  ======================================================================
//
//  Name:  Is_PWM_Signal_State_High
//
//  Description:  Returns the state of the PWM signal
//				1 = the pin is High, 0 = the pin is Low.
//				This prevents the illumination from turning on the boost circuit while
//				the beeper is beeping
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:	bool			PWM_Signal_State	Physical state of the PWM beeper line
// -------------------------------------------------------------------------------------
bool Is_PWM_Signal_State_High(void)
{
	return PWM_Signal_State;
}	//endof Is_PWM_Signal_State_High




