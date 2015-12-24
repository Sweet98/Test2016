#include <stdio.h>                             //lmc   was included by menu
#include "language.h"
#include "Hi704A.h"
#include "thread.h"
#include "db_sharedtable.h"
#include "i2c_drvr.h"
#include "scandrivermatrixinterface.h"
#include "ScanDriverDebug.h"
#include "menu.h"
#include "readerconfiguration.h"
#include "ProductConfiguration.h"
#include "db_chrio.h"                          			// added to output debug using xputstring()
#include "ioman.h"                            //lmc   add to use debug theStdInterface.printf()
#include "timeout.h"

//#if defined(OEM_ENGINE)
#include "HardwareToken.h"
#include "Timer3_Driver.h"
#include "trigger_5_5.h"
//#endif

#include "ScanDriverHWLayer.h"

#ifdef FLASH_LEDS
#include "Flash_Illumination_Control.h"
#else
#include "iMX25_Illumination_Control.h"
#endif //endof FLASH_LEDS

#if PLATFORM_CURRENT == imx25
#include "iMX25_M3IF.h"
#endif

#define TRUE_WINDOWING			0			

//If the BYD sensor is colorful so it can not use fld.
#define USE_THIRD_THREAD		1					//added here for testing addition of third thread for linear decoder
#define USE_SHUT_DOWN_THREAD	1					//added here for testing addition of fourth thread for imager abort shutdown

#define USE_ABORTS_BYD			1         			//added here for testing addition of passing aborts to the fourth thread for imager abort shutdown


/*==================================================================================================
 * Hardware layer stuff for BYD imager
 *==================================================================================================*/

static void BYD_poweron_gpio_init()
{
	//CSP type
	//init D11 gpio4_9 to control VDD
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_D11,MUX_MODE_5);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT9_LSH,GPIO_BIT9_WID,GPIO_LOW);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_D11,0x00000000);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT9_LSH,GPIO_BIT9_WID,GPIO_GDIR_OUTPUT);

	//initD10 gpio4_10 to control CHIP_ENB
		
	HAZARD_WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_D10,MUX_MODE_5);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT10_LSH,GPIO_BIT10_WID,GPIO_LOW);
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_D10,0x00000000);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT10_LSH,GPIO_BIT10_WID,GPIO_GDIR_OUTPUT);	
}

static void BYD_init_poweron()
{
	DISABLE_INTERRUPTS();
	BYD_poweron_gpio_init();
	//start
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT9_LSH,GPIO_BIT9_WID,GPIO_HIGH);//pull the VDD
	RESTORE_INTERRUPTS();

	
	//open MCLK
	SET_MCLK_DIV;								//MCLK operates at 24 MHz
	ENABLE_MCLK;//Turn on MCLK!!

	MT_Delay(1);
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT10_LSH,GPIO_BIT10_WID,GPIO_HIGH);//pull the CHIP_ENB

	MT_Delay(3);
									
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT10_LSH,GPIO_BIT10_WID,GPIO_LOW);//down the CHIP_ENB

}
 static void BYDPowerOn(void)
 {
	BYD_init_poweron();
	//MT_Delay(200);//delay 200ms before configure the registers remove it take a try
 }

 static void BYDPowerOff(void)
 {

	BYD_poweron_gpio_init();
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT9_LSH,GPIO_BIT9_WID,GPIO_LOW);//down the VDD
}


static void ConfigureBYDPins(void)
 {
 	CONFIG_BYD_CSI_D2_PAD;
	CONFIG_BYD_CSI_D3_PAD;
	CONFIG_BYD_CSI_D4_PAD;
	CONFIG_BYD_CSI_D5_PAD;
	CONFIG_BYD_CSI_D6_PAD;
	CONFIG_BYD_CSI_D7_PAD;
	CONFIG_BYD_CSI_D8_PAD;
	CONFIG_BYD_CSI_D9_PAD;

	//select slew rate and drive for mclk (currently set to normal drive and high slew)
	CONFIG_BYD_CSI_MCLK_PAD;

	CONFIG_BYD_CSI_VSYNC_PAD;
	CONFIG_BYD_CSI_HSYNC_PAD;
	CONFIG_BYD_CSI_PIXCLK_PAD;

	//configure the group pins for csi and
	//make csi master high priority
	*(volatile unsigned int *)IOMUXC_SW_PAD_CTL_GRP_DVS_CSI= 1U<<13;
	*(volatile unsigned int *)M3IF_CTL= 1U<<7; // csi master 7
 }



 /*==================================================================================================
 * Declarations
 *==================================================================================================*/

#define BYD_ID_REG 	0xff
#define BYD_REVISION 	0x0700
#define BYD_REVISION_2	0x0701
//static unsigned short BYDPartNumber = 0;
extern unsigned short nSensorRev;


static void (*pfVsyncNotify)(void);
static void (*pfDataNotify)(void);
static volatile DWORD nInterruptCount = 0;
static volatile BOOL bVsyncProcessingEnabled = FALSE;
static volatile BOOL bImageLineDataEnabled = FALSE;
static threadhandle_t vsyncntfy_thread = 0;
static threadhandle_t LineProcThread = 0;
static MT_Event_t vertSyncSofEvent;
static MT_Event_t statFifoFullEvent;
BOOL WriteRegisterBYD(void);
BOOL ReadRegisterBYD(void);

#ifdef USE_THIRD_THREAD
static threadhandle_t fastlinear_thread = 0;
static MT_Event_t StartFastDecode = NULL;

#define VSYNC_STACK_SIZE 5*1024
#define LINE_STACK_SIZE 5000
#define FASTDECODE_STACK_SIZE 32*1024
#else
#define VSYNC_STACK_SIZE (32*1024)
#define LINE_STACK_SIZE 5000
#endif

#ifdef USE_SHUT_DOWN_THREAD//spear
static threadhandle_t shut_down_thread = 0;
static MT_Event_t StartShutDown = NULL;
#define SHUTDOWN_STACK_SIZE 5*1024
#endif


//dma use indicator, for keeping track of next dma buffer pointer to be used
static volatile bool dmaBufferOneIsNextToLoad = FALSE;

//dma complete indicators, for checking for rx dma completion
static volatile bool dmaDoneOne = FALSE;
static volatile bool dmaDoneTwo = FALSE;
static volatile bool dmaNotDoneSof = FALSE;
volatile bool g_dmaNotDoneBeforeSof = FALSE;          //global variable for DMA not complete

//dma complete recovery indicators used by SOF to do error recovery
static volatile bool dmaComplete = FALSE;

//thread status indicator, used by vert thread to indicate state of bVsyncProcessingEnabled, set in vert interupt
static volatile bool bVertThreadEnabled = FALSE;

//dma pointer received failure indicator, did not get new pointer, debug only
static volatile bool bVertThreadDmaPoinerNotSent = FALSE;

//dma pointer received failure indicator, did not get new pointer int time to do dma flip in line interupt, debug only
static volatile bool bVertThreadDmaPoinerNotFlipedInThread = FALSE;

//Allow thread event to call Scan Driver indicator
static volatile bool bAllowThreadToCallScanDriv = FALSE;

//dma line error count, for storing line count during interupt, for use in threads
static volatile int iDmaErrorCount = 0;
static volatile int iDmaLineErrorCount = 0;

//fifo overrun indicators, debug only
static volatile bool rxFifoOver = FALSE;
static volatile bool statFifoOver = FALSE;
static volatile bool hrespError = FALSE;

#ifdef THREAD_TIME_TEST
static CTimeout	testWaitDelay;                    //lmc  test thread time
static volatile ULONG testTickStart = 0;                  //lmc   thread time debug
static volatile ULONG testTickInt = 0;                  //lmc   thread time debug
//For line thread test only:
static volatile ULONG testTickCallThread = 0;                  //lmc   thread time debug
static volatile ULONG testTickInThread = 0;                  //lmc   thread time debug
static volatile ULONG testTickSDstart = 0;                  //lmc   thread time debug
static volatile ULONG testTickSDend = 0;                  //lmc   thread time debug
static volatile ULONG testTickTwelve = 0;                  //lmc   thread time debug
static volatile ULONG testTickDmaComp = 0;                  //lmc   thread time debug
static volatile int iDmaLineErrorCountTimeTest = 0;
#endif

#ifdef THREAD_TIME_TEST_LINE
//extern int iErrorStallCount;			//lmc debug remove
static int iErrorStallCount = 0;			//lmc debug remove
static volatile int iErrorStallCountSaved;			//lmc debug remove
#endif

//Stat DMA buffer
static volatile unsigned long ulStatFifoDmaBuffer[64];		//dma buffer for the stat fifo, max size is 64 words, must be word alligned
static volatile unsigned long * pStatFifoDmaBuffer;

//dma count
static volatile int iDmaCount = 0;                      //lmc debug remove


/*==================================================================================================
 * Configuration of the imager
 *==================================================================================================*/



/** min_max_align_size is used to insure the height and width of the ROI is
    correctly divisible by a power of two.

	For instance, the height of an image should be divisible by 8.  This 
	function will take the top row and bottom row and adjust them so that
    the height will be divisible by 8.  

	@param[in,out] small      The column or row of the left most of top most 
	                          pixels.
	@param[in,out] large      The column or row of the right most or bottom 
                              most pixel.
    @param[in]     max        The maximum column or row number
	@param[in]     alignment  An alignment for the width or height of the 
                              image.  The width or height must be a multiple
                              of alighment
*/


static void align_size(  unsigned * small, unsigned * large, 
						 unsigned max, unsigned alignment      )
{
	// max must be aligned correctly.  If not, it must be reduced 
	// to align it.

	max = (max / alignment) * alignment;

	unsigned size;  // Size is width or height of the image.
	
	if (*large < *small)         // Insure *large >= *small
	{
		 unsigned tmp = *large;
		 *large = *small;
		 *small = tmp;
	 }

	 // Calculate requested size
	 size = *large - *small + 1; 

	 unsigned mod = size % alignment;
	 if (mod == 0)  // If it is already aligned, return 
		 return;    // immediately.

	 // How much to increase the size to get alignment
	 unsigned increase = alignment - mod; 

	 *large += increase;   // Actually increase the size.
	 if (*large < max)    // *large is less than max, return
		 return;          

	 // If we get here, increasing *large caused it to go over max.
	 // So, need to decrease *small too!
	 *large = max - 1;

	 // Calculate a new size 
	 size = *large - *small + 1; 
	 mod = size % alignment;

	 unsigned decrease = alignment - mod;
	 
	 // If there is not enough room to decrease by the requested 
	 // amount, then set *small to zero
	 if (*small < decrease)
	 {
		 *small = 0;
		 return;
	 }
	 
	 *small -= decrease;
}

bool CanChangeROI()
{
#if TRUE_WINDOWING
	if (! GET_SETTING(ImageROIEnabled))
		return false;
	return true;
#else
	return false;
#endif
}

bool IsROIChanged()
{
	if (!CanChangeROI())
	{
		return false;
	}

	unsigned left, right, top, bottom;
	ROILeftRight(&left, &right);
	ROITopBottom(&top,  &bottom);

	/// @todo De-magicfy these constants
	if (  left != 0    || right != (NATIVE_IMAGE_WIDTH - 1) 
		  || top  != 0 || bottom  != (NATIVE_IMAGE_HEIGHT - 1)  )
	{
		return true;
	}
	return false;
}

inline unsigned ROIImageHeight()
{
	unsigned top, height;
	ROITopHeight(&top, &height);
	return height;
}	

inline unsigned ROIImageWidth()
{
	unsigned left, width;
	ROILeftWidth(&left, &width);
	return width;
}

inline unsigned ROIImageSize()
{
	return ROIImageHeight() * ROIImageWidth();
}

inline unsigned ROIPreEndOfFrameRows()
{
	/*unsigned gap = 22;
	if (IsROIChanged())
		gap = 40;

	return ROIImageHeight() - gap; for HI2D*/
	return ROIImageHeight();
}

inline unsigned ROIPreEndOfFrameWords()
{
	return ROIImageWidth() * ROIPreEndOfFrameRows() / 4;
}

inline void ROILeftRight(unsigned * left, unsigned * right)
{
	if (! CanChangeROI())
	{
		*left = 0;
	    *right = NATIVE_IMAGE_WIDTH - 1;
		return;
	}

	*left  = GET_SETTING(ImageROILeft);
	*right = GET_SETTING(ImageROIRight);

	align_size(left, right, NATIVE_IMAGE_WIDTH, 4);
}

inline void ROITopBottom(unsigned * top, unsigned * bottom)
{
	if (! CanChangeROI())
	{
		*top = 0;
	    *bottom = NATIVE_IMAGE_HEIGHT - 1;
		return;
	}

	*top    = GET_SETTING(ImageROITop);
	*bottom = GET_SETTING(ImageROIBottom);

	align_size(top, bottom, NATIVE_IMAGE_HEIGHT, 8);
}

inline void ROILeftWidth(unsigned * left, unsigned * width)
{
	unsigned right;
	ROILeftRight(left, &right);
	*width = right - *left + 1;
}

inline void ROITopHeight(unsigned * top, unsigned * height)
{
	unsigned bottom;
	ROITopBottom(top, &bottom);
	*height = bottom - *top + 1;
}


DWORD GetRegisterSettingsForHHPExposureControlBYD(const BYDRegisterEntry_t ** ppRegisterEntries)
{
	int size;

	*ppRegisterEntries = BYDRegEntriesForHHPExposureControl_3703;
	size = sizeof(BYDRegEntriesForHHPExposureControl_3703)/sizeof(BYDRegisterEntry_t);

	return size;
}


DWORD GetScanWidthBYD(void)
{
	return ROIImageWidth();  // placeholder for windowing, although not sure this architecture will support
}


DWORD GetScanHeightBYD(void)
{
	return ROIImageHeight();
}


/*==================================================================================================
 * Memory Control
 *==================================================================================================*/
static volatile BOOL bUseCachedMemory = FALSE;

void UseCachedMemoryBYD(bool bValue)
{
	bUseCachedMemory = bValue;
}


DWORD AllocatePhysicalScanSpaceBYD(void)
{
	unsigned char *pImageBuffer;

	pImageBuffer = (unsigned char *)malloc(NATIVE_IMAGE_WIDTH*NATIVE_IMAGE_HEIGHT);
	pImageBuffer += (DWORD) ( NON_CACHED_MEMORY_BASE - CACHED_MEMORY_BASE ); // 0x01000000;
	return (DWORD)pImageBuffer;
}


DWORD FreePhysicalScanSpaceBYD(DWORD hImage)
{
	unsigned char *pImageBuffer;
	pImageBuffer = (unsigned char *)hImage;
	pImageBuffer -= (DWORD) ( NON_CACHED_MEMORY_BASE - CACHED_MEMORY_BASE ); // 0x01000000;

	free((void *)pImageBuffer);

	return 1;
}


DWORD BindPhysicalMemoryToVirtualMemoryBYD(DWORD hPhysical,void *pVirtual)
{
	if (bUseCachedMemory)
	{
		*((DWORD **)(pVirtual)) = (DWORD *)(hPhysical- (DWORD) ( NON_CACHED_MEMORY_BASE - CACHED_MEMORY_BASE ));    // 0x1000000);
		MMU_CleanAndInvalidate();
	}
	else
	{
		*((DWORD **)(pVirtual)) = (DWORD *)hPhysical;
	}

	return 1;
}


DWORD UnbindPhysicalMemoryFromVirtualMemoryBYD(DWORD hPhysical,void *pVirtual)
{
	hPhysical = hPhysical + 0;
	*((DWORD **)(pVirtual)) = (DWORD *)0;
	return 1;
}


/*==================================================================================================
 * I2C Functionality
 * =================================================================================================*/

BOOL WriteIIC_BYD( unsigned char subaddress, const unsigned short * uiBuffer, unsigned int nCount )
{
 	BOOL bStatus = TRUE;
	
	if( g_pI2C_Drvr == NULL )		
		return FALSE;

	if( nCount == 0 )		
		return TRUE;

	bStatus = g_pI2C_Drvr->Send_Data_W_Short_W_Dual_Reg_Size_I2C( BYD_IIC_WRITE_ADDRESS, subaddress, uiBuffer, nCount, 0x100 );

	return bStatus;
}


BOOL ReadIIC_BYD( unsigned char subAddress, unsigned short * uiBuffer, unsigned int nCount )
{
	BOOL bStatus = TRUE;
	
	if( g_pI2C_Drvr == NULL )	
		return FALSE;
	
	if( nCount == 0 )	
		return TRUE;

	bStatus = g_pI2C_Drvr->Write_Setup_Then_Receive_Data_W_Short_W_Dual_Reg_Size_I2C( BYD_IIC_READ_ADDRESS, subAddress, uiBuffer, nCount, 0x100 );

	return bStatus;
}


BOOL IsPsocInterfaceSupportedBYD(void)
{
	return FALSE;	//don't support psoc
}

/*==================================================================================================
 * Power and Transfer Functionality
 * =================================================================================================*/
#define INTERRUPT_COUNT_MAX	13                        //line count is one less then placed here

//Indicates that a new pointer has been passed from the scan driver for dma's memory location
static volatile BOOL bInitScan = FALSE;

//Indicates that the new pointer has been used in the line interupt, and does not to have dma set up in the vertical interupt
static volatile int dInitScanInLineIntCompleted = 0;						//0 == flip has not occured in the line interupt, no InitCap() response from S.D., starting condition;
 																			//1 == new pointer not passed, scan driver did not have one so use old one and then let Vert turn off DMA's before use;
																			//2 == new pointer passed in #11 line, now it needs to be flipped into DMA buffer;
																			//3 == new pointer has now been flipped into DMA buffer;

//Indicates that we are configuring dma for first frame
static volatile BOOL bFirstFrame = FALSE;

//Indicates that we want image progress size for the EOF instead of stat progress
static volatile BOOL bEOF = FALSE;

static volatile BOOL bImagerIsOn = FALSE;

//variable for doing sync power down state for image stop
//0 == shut down not detected, default state
//1 == shut down detected
//2 == SOF has occured, so send i2c command to untrigger
//3 == i2c command has been sent, so wait for dma to be disabled at end of frame
//4 == dma has been disabled
static volatile int iDoSynchronizedPowerDownBYD = 0;
//variable for doing imediate sync power down state, for single frame capture which does a same frame shutdown
//0 == shut down not detected for single frame capture, default state
//1 == shut down request detected
static volatile int iDoSynchronizedPowerDownBYDImmediate = 0;

//variable for doing sync power down state with abort, for shutdown on current line
//0 == shut down with abort not detected , default state
//1 == shut down with abort request detected
static volatile int iDoSynchronizedPowerDownBYDAbort = 0;

//variable for doing sync power down state with abort, for shutdown on current line
//0 == shut down with BYD power down not detected , default state
//1 == shut down with BYD power down request detected
static volatile int iDoSynchronizedPowerDownBYDPowerOff = 0;

//variables for doing sync power down of imager with passed i2c untrigger command
static unsigned char subaddressForSynchronizedPowerDownBYD = 0;
#ifndef HI2D_T
static unsigned short uiBufferForSynchronizedPowerDownBYD = 0;
static unsigned int nCountForSynchronizedPowerDownBYD = 0;
#endif
//variable for doing abort of imager with passed i2c untrigger command
static unsigned char subaddressForAbortBYD = 0;
static unsigned short uiBufferForAbortBYD = 0;
static unsigned int nCountForAbortBYD = 0;

static volatile BOOL bWaitForVsyncBYD = FALSE;
static volatile unsigned int uiHoldCsicr1 = 0, uiHoldCsicr2 = 0, uiHoldCsicr3 = 0;
static volatile unsigned char *pScanLocation;


DWORD GetFrequencyPerFrameOfLineDataCallbackBYD(void)
{
	DWORD result = INTERRUPT_COUNT_MAX;
	result -= 1;
	return result;
}


DWORD ImagerIdlePowerLevelBYD(void)
{
	return GET_SETTING(ImagerIdlePowerLevel);               //0 == power off, 1 == standby, 2 == running but not triggered
}


DWORD	ImagerIsPoweredBYD(void)
{
	return bImagerIsOn;
}


void ResetTransferHardwareBYD(void)
{
	BOOL bSofWasEnabled;

	// Here we disable the DMA, we depend on another source to enable it which
	// will be synchronized with the VSYNC
	bSofWasEnabled = (BOOL)IS_SOF_INTERUPT_ENABLED;

	if (bSofWasEnabled)
	{
		DisableInterruptNumber(VEC_CSI_INT);
		DISABLE_SOF_INTERUPT;
	}

	SET_MCLK_DIV;								//MCLK operates at 48 MHz
	ENABLE_MCLK;								//Turn on MCLK!!

	if (bSofWasEnabled)
	{
		//Recall vsync initializtion
		CSI_CONTROL_REG_ONE = uiHoldCsicr1;  /*note when this is saved the CSI enable is 1 so we are not disabling */
		CSI_CONTROL_REG_TWO = uiHoldCsicr2;
		SET_CSI_CONTROL_REG_THREE = uiHoldCsicr3;

		DISABLE_EOF_INTERUPT;
		DISABLE_RX_FIFO_FULL_INTERUPT;
		DISABLE_RX_FIFO_DMA;                          // Now we disable DMA
		DISABLE_BUFFER_ONE_DMA_DONE;
		DISABLE_BUFFER_TWO_DMA_DONE;
		DISABLE_RX_FIFO_OVERRUN_INT;
		SET_RX_DMA_BURST_TYPE;											//set burst to incr8

		DISABLE_STAT_FIFO_INT;
		DISABLE_STAT_FIFO_DMA;			//disable stat fifo dma
		DISABLE_STAT_FIFO_DMA_DONE;
		DISABLE_STAT_FIFO_OVERRUN_INT;

		DISABLE_HRESP_ERROR_INT;
		CLEAR_CSI_CONTROL_REG_THREE_FRAME_COUNT;

		// Clear the pending bits for the overruns, fills of FIFO, and start of frame
		CLEAR_ALL_INTERUPT_STATUS;

		ENABLE_SOF_INTERUPT;
		EnableInterruptNumber(VEC_CSI_INT);
	}

	bInitScan = FALSE;
	dInitScanInLineIntCompleted = 0;

	//reset variables for sync shut down
	subaddressForSynchronizedPowerDownBYD = 0;        //clear to indicate no register has been selected
	iDoSynchronizedPowerDownBYD = 0;                //clear to indicate that we are not shutting down yet on a stop imaging command
	iDoSynchronizedPowerDownBYDImmediate = 0;       //clear to indicate that we are not shutting down yet on a single frame capture
	iDoSynchronizedPowerDownBYDAbort = 0;			//clear to indicate that we are not shutting down yet on a imager abort
	iDoSynchronizedPowerDownBYDPowerOff = 0;		//clear to indicate that we are not shutting down yet on a power down

	//variable for doing abort of imager with passed i2c untrigger command
	subaddressForAbortBYD = 0;				//clear to indicate no register has been selected

}


BOOL ImagerPowerUpBYD(void)
{
	I2C_Drvr::Create_I2C_Drvr();

	DISABLE_INTERRUPTS();

	if( uiHoldCsicr1 == 0 ) //These variables have not been configured yet, so set them to mx 25 default for each control register
	{
		SET_CSI_PORT_MUX_MODE;
		ConfigureBYDPins();

		uiHoldCsicr1 = 0x40000800;
		uiHoldCsicr2 = 0x00000000;
		uiHoldCsicr3 = 0x00000000;
	}

	RESTORE_INTERRUPTS();

	ResetTransferHardwareBYD();

	DisableInterruptNumber(VEC_CSI_INT);

	//Recall vsync initializtion
	CSI_CONTROL_REG_ONE = uiHoldCsicr1;  /*note when this is saved the CSI enable is 1 so we are not disabling */
	CSI_CONTROL_REG_TWO = uiHoldCsicr2;
	SET_CSI_CONTROL_REG_THREE = uiHoldCsicr3;


	BYDPowerOn();				

	MT_Delay(2);

	bImagerIsOn = TRUE;       	//indicate imager is powered

	EnableInterruptNumber(VEC_CSI_INT);

	return 1;
}


BOOL ImagerPowerDownBYD(void)
{
	DISABLE_INTERRUPTS();

	BYDPowerOff();

	RESTORE_INTERRUPTS();

	bImagerIsOn = FALSE;

	return TRUE;
}


void WaitMillisecondsBYD(DWORD nTime)
{
	MT_Delay(nTime);                    //ntime is in milliseconds
}

void DoPointerFlipRxDma( void )
{
	if( !bFirstFrame )
	{
		if( bInitScan && (dInitScanInLineIntCompleted == 2) )		//got the new pointer, so set in this line (#11 or #12), so do the flip
		{
			dInitScanInLineIntCompleted = 3;		//set up dma here, so tell dma complete this, so it does not do it again

			//Set the new pointer for the next auto buffer flip on the rx fifo dma
			if( dmaBufferOneIsNextToLoad )	//If buffer one just completed, then it is the next buffer address to be loaded
			{
				SET_DMA_BUF_ONE_DESTINATION_ADDRESS((unsigned long)pScanLocation);  	//set destination address, for buffer one, from my buffer address
				SET_DMA_BUF_TWO_DESTINATION_ADDRESS((unsigned long)NULL);
			}
			else		//else buffer two just completed, then it is the next buffer address to be loaded
			{
				SET_DMA_BUF_TWO_DESTINATION_ADDRESS((unsigned long)pScanLocation);  	//set destination address, for buffer two, from my buffer address
				SET_DMA_BUF_ONE_DESTINATION_ADDRESS((unsigned long)NULL);
			}
		}
	}
}


//InitCapture
//Passes a new pointer to the rx dma
//
//Definition of dPointerPassed;
//a 2 indicates that a new pointer is available and was passed
//a 1 indicates that no new pointer was available, so load old pointer
//a 0 is a failure, or if this function was not called on purpose,
//like when single from capture is done, then zero is also the default value
//indicating that the scan driver is done, and did not pass a pointer.
//This will be indcated when bVertThreadEnabled == false, the default value, and bInitScan == TRUE, indicating a new pointer pass
//kid problem notice  ken@2011.11.11
DWORD InitCaptureBYD( DWORD hPhysical, DWORD dPointerPassed )
{
	DWORD returnStatus = 0;

	DisableInterruptNumber(VEC_CSI_INT);

	if( dPointerPassed == 2 )            //got a pointer
	{
		if (bInitScan != TRUE)
		{
			pScanLocation = (UCHAR *)hPhysical;
			bInitScan = TRUE;
			dInitScanInLineIntCompleted = 2;
			DoPointerFlipRxDma();

#if(SCAN_DRIVER_DEBUG_TEST)
			theStdInterface.printf("MEM %x \r\n",hPhysical);                        //lmc   debug
#endif

			returnStatus = 2;
		}
		else            //bInitScan has not been cleared
		{
			dInitScanInLineIntCompleted = 0;
		}
	}
	else if( dPointerPassed == 1 )                //scan driver did not have a pointer available
	{
		bInitScan = FALSE;
		dInitScanInLineIntCompleted = 1;
		returnStatus = 1;
	}
	else                                        //error
	{
		bInitScan = FALSE;
		dInitScanInLineIntCompleted = 0;
	}

	EnableInterruptNumber(VEC_CSI_INT);

#ifdef ENABLE_FUNCTION_CALL_DEBUG
	xputstring(" InitCap ");        //lmc    debug
#endif

	return returnStatus;
}


BOOL LoadCaptureInfoBYD(void)
{
	BOOL bSofWasEnabled;

	DISABLE_INTERRUPTS();
	DisableInterruptNumber(VEC_CSI_INT);

// Initialize the same kickoff of capture as the Vsync would normally do, recognizing that this sensor first frame is good, Tint precedes!
	DISABLE_STAT_FIFO_INT;    //disable stat fifo interupt
	DISABLE_STAT_FIFO_DMA;			//disable stat fifo dma
	DISABLE_STAT_FIFO_DMA_DONE;  //disable stat fifo dma done interupt
	DISABLE_STAT_FIFO_OVERRUN_INT;
	CLEAR_STAT_FIFO_DMA_DONE;				//clear stat fifo dma done
	CLEAR_STAT_FIFO_OVERRUN_INT;

	bAllowThreadToCallScanDriv = TRUE;                  //set the allow the threads to call scan driver flag to run threads
	nInterruptCount = 0;
	dmaComplete = TRUE;		//If tri stated, might get a SOF when imager enabled, so do not allow anything to occur in SOF during first frame
	rxFifoOver = FALSE;
	statFifoOver = FALSE;
	hrespError = FALSE;

	DISABLE_EOF_INTERUPT;	 				// Disable EOF interupt which handles stat #12
	DISABLE_RX_FIFO_DMA;                 //disable dma
	DISABLE_RX_FIFO_FULL_INTERUPT;
	DISABLE_BUFFER_ONE_DMA_DONE;
	DISABLE_BUFFER_TWO_DMA_DONE;
	DISABLE_RX_FIFO_OVERRUN_INT;
	CLEAR_EOF_INT;                  //clear interupt status
	CLEAR_BUFFER_TWO_DMA_DONE;      //clear interupt status
	CLEAR_BUFFER_ONE_DMA_DONE;      //clear interupt status
	CLEAR_RX_FIFO_OVERRUN_INT;

	DISABLE_HRESP_ERROR_INT;
	CLEAR_HRESP_ERR_INT;
	CLEAR_CSI_CONTROL_REG_THREE_FRAME_COUNT;

	DISABLE_SYNC_FCC;						//disable sync clearing to do it manually
	CLEAR_CSI_FIFO;
	CLEAR_STAT_FIFO;		// clear the STAT FIFO, then waits for next SOF to restart		//LMC  REMOVE SINCE WE CAN NOT WAIT FOR NEXT VERTICAL ON FIRST FRAME due to timing
	//ENABLE_SYNC_FCC;						//enable sync fifo clearing


	SET_DMA_BUF_ONE_DESTINATION_ADDRESS((unsigned long)pScanLocation);  	//set destination address, for buffer one, from my buffer address, on mx25
	SET_DMA_BUF_TWO_DESTINATION_ADDRESS((unsigned long)NULL);  	//set destination address, for buffer two, from my buffer address, on mx25
	
    SET_DMA_IMAGE_WIDTH_TO(ROIImageWidth());						          //set image width for HI2D KENTODO need to use function to get image details.
	SET_DMA_IMAGE_HEIGHT_TO(ROIImageHeight());						          //set image height for HI2D
	SET_DMA_IMAGE_FRAME_BUFFER_STRIDE_TO( 0 );           //set dma stride
	SET_DMA_EOF_RX_COUNT_TRANSFER_SIZE(ROIPreEndOfFrameWords());//HI2D

	pStatFifoDmaBuffer = DMA_STAT_TRANSFER_LOCATION;
	SET_DMA_STAT_FIFO_DESTINATION_ADDRESS( (unsigned long)pStatFifoDmaBuffer );
	SET_DMA_STAT_FIFO_TRANSFER_SIZE( DMA_STAT_TRANSFER_SIZE );             //set stat fifo transfer size in words


	//reset variables for dma pointer flip
	bFirstFrame = FALSE;                       //now allow dma initialization in Init capture during the line thread
	bInitScan = FALSE;                         //set to low to allow Init capture to pass a new pointer
	dInitScanInLineIntCompleted = 0;           //indicates the state of pointer passing from Init capture
	dmaBufferOneIsNextToLoad = FALSE;	//Starts with buffer one being the first buffer, so false, since buffer two is next to be loaded with new buffer pointer address

	//reset variables for sync shut down
	subaddressForSynchronizedPowerDownBYD = 0;        //clear to indicate no register has been selected
	iDoSynchronizedPowerDownBYD = 0;                //clear to indicate that we are not shutting down yet on a stop imaging command
	iDoSynchronizedPowerDownBYDImmediate = 0;       //clear to indicate that we are not shutting down yet on a single frame capture
	iDoSynchronizedPowerDownBYDAbort = 0;			//clear to indicate that we are not shutting down yet on a imager abort
	iDoSynchronizedPowerDownBYDPowerOff = 0;		//clear to indicate that we are not shutting down yet on a power down

	//variable for doing abort of imager with passed i2c untrigger command
	subaddressForAbortBYD = 0;				//clear to indicate no register has been selected

	// Here we disable the SOF, we depend on Enable Vsync Processing to enable it, which
	// will then be synchronized with enabling VSYNC processing
	bSofWasEnabled = (BOOL)IS_SOF_INTERUPT_ENABLED;

	if( bSofWasEnabled )
	{
		DISABLE_SOF_INTERUPT;
		CLEAR_SOF_INT;
	}

	// Here we will also move these to Enable Vsync Processing, which will keep interupts synchronized with
	// enabling VSYNC processing, and triggered state occurs is set by scan driver after Enable Vsync Processing.
	//ENABLE_STAT_FIFO_INT;                                 //enable line interupt
	//ENABLE_STAT_FIFO_DMA;											//enable stat fifo dma
	//ENABLE_STAT_FIFO_DMA_DONE;                             //enable dma complete line interupt
	//ENABLE_STAT_FIFO_OVERRUN_INT;		//not needed, except to cause error detection;
	//REFRESH_STAT_FIFO_DMA;										//refresh the stat dma
	//ENABLE_RX_FIFO_DMA;                                   //enable dma
	//ENABLE_BUFFER_ONE_DMA_DONE;      //turn on rx fifo dma done interupt
	//ENABLE_BUFFER_TWO_DMA_DONE;      //turn on rx fifo dma done interupt
	//ENABLE_RX_FIFO_OVERRUN_INT;		//not needed, except to cause a trigger for trace debugging, once a frame;    lmc debug remove
	//REFRESH_RX_FIFO_DMA;                                  //refresh dma
	//ENABLE_EOF_INTERUPT;												//enable EOF interupt
	//ENABLE_HRESP_ERROR_INT;			//not needed, except to cause error detection;

	EnableInterruptNumber(VEC_CSI_INT);
	RESTORE_INTERRUPTS();

#ifdef ENABLE_FUNCTION_CALL_DEBUG
	xputstring(" LoadCapInfo ");					//lmc   debug
#endif

	return TRUE;
}



//======================================================================================
// RequestSynchronizedPowerDownBYD  -- BYD did this the way 'we' would have done it, they have
// a standby request bit in their control register which says stop imaging at end of next frame.
// so instead of the weird handshaking / balancing act... we will do this directly from 'above' in the Scan Driver.
//   Functionality:
//   Arguments:
//   Returns:
//======================================================================================
void RequestSynchronizedPowerDownBYD( unsigned char subaddress, const unsigned short * uiBuffer, unsigned int nCount )
{
	if( !iDoSynchronizedPowerDownBYDPowerOff && !iDoSynchronizedPowerDownBYDImmediate )
	{
		if( (subaddress == 0x09) )
		{
			subaddressForAbortBYD = subaddress;
			uiBufferForAbortBYD = *uiBuffer;
			nCountForAbortBYD = nCount;
		}
		else
		{
			subaddressForSynchronizedPowerDownBYD = 0;
			subaddressForAbortBYD = 0;
		}
		if( subaddressForAbortBYD == 0x09 )
		{
			iDoSynchronizedPowerDownBYDAbort = 1;
		}
	}
}


//======================================================================================
// SynchronizedPowerDownBYD
//   Functionality:
//   Arguments:
//   Returns:
//======================================================================================
BOOL SynchronizedPowerDownBYD(void)
{
	//Not used with this imager
	return 1;
}


//======================================================================================
// WaitForVsyncBYD
//   Functionality:
//   Arguments:
//   Returns:
//======================================================================================
BOOL WaitForVsyncBYD(void)
{
	bWaitForVsyncBYD = TRUE;
	while (bWaitForVsyncBYD == TRUE);
	return 1;
}



/*======================================================================================
 * Light Control
 *======================================================================================*/
static volatile BOOL bAimerIsOn = FALSE, bIllumIsOn = FALSE;

BOOL  LedControlStateBYD(UINT State)
{
	SetLedControlState(State);

	return 1;
}


BOOL AimIsOnBYD(void)
{
	return bAimerIsOn;
}


DWORD AimOnBYD(void)
{
	SetAIM_ONState(ILL_ON);

	bAimerIsOn = TRUE;

	return 1;
} 


DWORD AimOffBYD(void)
{
	SetAIM_ONState(ILL_OFF);

	bAimerIsOn = FALSE;

	return 1;
}


DWORD AimConfigBYD(DWORD nLevel)
{
	return 1;
}


BOOL IllumIsOnBYD(void)
{
	return bIllumIsOn;
}


DWORD IllumOnBYD(void)
{
	SetILL_ONState(ILL_ON);

	bIllumIsOn = TRUE;

	return 0;
}


DWORD IllumOffBYD(void)
{
	SetILL_ONState(ILL_OFF);

	bIllumIsOn = FALSE;

	return 1;
}


DWORD IllumConfigBYD(DWORD nLevel)
{
	return 1;
}


DWORD LightsLowPowerModeBYD(void)
{
	return (DWORD)(GET_SETTING(ScanAimerMode)==AIM_ALT);
}


DWORD IllumOffTimerBYD( void )
{
	return 1;
}

/*======================================================================================
 * Interrupt Functionality
 *======================================================================================*/
#if CONCURRENT
volatile static unsigned char *pCurrentStart,*pLastStart;
#endif


//clear all of the indicators used by dma and csi
void ClearDmaIndicators(void)
{
	//initialize dma done state indicators before we check dma state
	dmaDoneTwo = FALSE;						//indicate that dma two has completed
	dmaDoneOne = FALSE;						//indicate that dma one has completed
	bVertThreadEnabled = FALSE;				//indicate that scan driver is currently enabled
	bVertThreadDmaPoinerNotSent = FALSE;		//indicate failure mode, that new dma buffer was not passed from scan driver
	bVertThreadDmaPoinerNotFlipedInThread = FALSE;		//indicates that buffer was not fliped in line interupt, had to be done in interrupt complete instead

	//Check this to see if we are still enabled
	bVertThreadEnabled = bVsyncProcessingEnabled;             //This is the first flag set in the shut down process!!!!

	bAllowThreadToCallScanDriv = TRUE;                  //set the allow the threads to call scan driver flag
}


//Check for the completion of the RX dma, called in interupt only, and only from the rx dma complete service
void CheckDmaComplete(void)
{
	if( IS_BUFFER_ONE_DMA_DONE_SET )
	{
		dmaDoneOne = TRUE;
		dmaBufferOneIsNextToLoad = TRUE;		//If buffer one just completed, then it is the next buffer address to be loaded, so true

		// Clear the interrupt flag
  		CLEAR_BUFFER_ONE_DMA_DONE;      //clear interupt status
	}
	else if( IS_BUFFER_TWO_DMA_DONE_SET )
	{
		dmaDoneTwo = TRUE;
		dmaBufferOneIsNextToLoad = FALSE;		//If buffer two just completed, then it is the next buffer address to be loaded, so false to buffer one next

		// Clear the interrupt flag
		CLEAR_BUFFER_TWO_DMA_DONE;      //clear interupt status
	}

	//Either dmaDoneOne or dmaDoneTwo must be true here, because we are in rx dma complete service, so no need to check below for normal operation
	//Check for normal DMA completion, then if not, handle failure or shutdown condition
	if( !(bVertThreadEnabled && (bInitScan || (iDmaErrorCount >= 11))) )	//normal condition, should always complete DMA,
	{                                                    						//be enabled, and have a new pointer passed, or if single cap,
		                                                						//has reached stat line 11, if working correctly.

		if( (bVertThreadEnabled == FALSE) )      //Do not allow vert thread to call scan driver, since we are shutting down
		{
			bAllowThreadToCallScanDriv = FALSE;         //also set the allow the line thread to call scan driver flag to false, too!
		}
	}
}


//Disable the Rx fifo DMA, called in interupt only
void DisableRxDma(void)
{
	//If done with dma or not, its time to reset dma

	//disable RX fifo dma
	DISABLE_RX_FIFO_DMA;                       // turn off the DMA
	DISABLE_BUFFER_ONE_DMA_DONE;          //turn off interupt
	DISABLE_BUFFER_TWO_DMA_DONE;        //turn off interupt
	DISABLE_RX_FIFO_OVERRUN_INT;        //turn off over run error interupt
	DISABLE_HRESP_ERROR_INT;            //turn off hresp error interupt

	//DISABLE_SYNC_FCC;						//disable sync clearing to do it manually
	//CLEAR_CSI_FIFO;			// clear the CSI RX FIFO, then restarts immediately  //comment out since we are now using auto clear function at SOF
	//ENABLE_SYNC_FCC;						//enable sync fifo clearing

	CLEAR_BUFFER_ONE_DMA_DONE;      //clear interupt status
	CLEAR_BUFFER_TWO_DMA_DONE;      //clear interupt status
	CLEAR_RX_FIFO_OVERRUN_INT;      //clear polled status
	CLEAR_HRESP_ERR_INT;            //clear polled status
}


//Disable the Stat fifo DMA, called in interupt only
void DisableStatDma(void)
{
	//If done with dma or not, its time to reset dma

	//DISABLE_SYNC_FCC;						//disable sync clearing to do it manually
	//CLEAR_STAT_FIFO;		// clear the STAT FIFO, then waits for next SOF to restart		//LMC  REMOVE SINCE WE CAN NOT WAIT FOR NEXT VERTICALON FIRST FRAME
	//ENABLE_SYNC_FCC;						//enable sync fifo clearing

	// disable the STAT FIFO interrupt, we only want it enabled when valid frame data is coming in
	//DISABLE_STAT_FIFO_INT;                            //not needed
	DISABLE_STAT_FIFO_DMA;			//disable stat fifo dma
	DISABLE_STAT_FIFO_DMA_DONE;		  //disable stat fifo dma done interupt
	DISABLE_STAT_FIFO_OVERRUN_INT;		//disable stat fifo over run interupt
	CLEAR_STAT_FIFO_DMA_DONE;				//clear stat fifo dma done interupt status
	CLEAR_STAT_FIFO_OVERRUN_INT;        //clear polled status

	DISABLE_EOF_INTERUPT;			// Disable EOF interupt which handles stat #12
}


//Prepare for the auto pointer flip on the Rx fifo DMA, called in interupt only
void PrepareForPointerFlipRxDma( void )
{
	if( dmaBufferOneIsNextToLoad )	//If buffer one just completed, then it is the next buffer address to be loaded
	{
		SET_DMA_BUF_ONE_DESTINATION_ADDRESS((unsigned long)pScanLocation);  	//set destination address, for buffer one, from my buffer address
	}
	else		//else buffer two just completed, then it is the next buffer address to be loaded
	{
		SET_DMA_BUF_TWO_DESTINATION_ADDRESS((unsigned long)pScanLocation);  	//set destination address, for buffer two, from my buffer address
	}
}


//Completed the auto pointer flip on the Rx fifo DMA, called in interupt only
void CompletedPointerFlipRxDma( void )
{
	if( bInitScan == TRUE )
	{
#if CONCURRENT
		pLastStart = pCurrentStart;
		pCurrentStart = pScanLocation;
#endif

		bInitScan = FALSE;                 //Indicate that we have initialized DMA with new pointer
	}
}


//Restart the Stat fifo DMA, called in interupt only
void RestartStatDma( void )
{
	CLEAR_STAT_FIFO_DMA_DONE;				//clear stat fifo dma done interupt status
	CLEAR_STAT_FIFO_OVERRUN_INT;        //clear polled status
	REFRESH_STAT_FIFO_DMA;					//refresh the stat dma
}


//register scandriver function called by vertical thread
DWORD RegisterVsyncNotificationBYD(void(*fVsyncNotification)(void))
{
	pfVsyncNotify = fVsyncNotification;
	return 1;
}


//======================================================================================
// VsyncService
//   Functionality:
//   Arguments:
//   Returns:
//======================================================================================
 // rev 1.0 MX1 has a real nice synchronous clear of CSI rx fifo with automatic hold off until VSYNC goes active
 //so the Vsync handling is real easy here we use an and gate to simulate this


static threadhandle_t VsyncServiceBYD(void *)
{

#ifdef THREAD_TIME_TEST_VERT
	testWaitDelay.SetExpiration(48000);                            //lmc  test thread time
	testTickStart = testWaitDelay.GetRestTicks();                  //lmc   thread time debug
#endif

	unsigned int iEventSet = 0;             //event has not been set

	if( IS_HRESP_ERR_INT_SET )
	{
		CLEAR_HRESP_ERR_INT;
		hrespError = TRUE;
	}


	if( IS_STAT_FIFO_OVERRUN_INT_SET )
	{
		CLEAR_STAT_FIFO_OVERRUN_INT;

		//Stat fifo stops when overrun occurs, so clear it hear so it will be reset by next SOF
		if( iDoSynchronizedPowerDownBYD < 2 )        //if we have not got to our last SOF yet, then allow a clear to be set here
		{
			CLEAR_STAT_FIFO;		// clear the STAT FIFO, then waits for next SOF to restart
		}

		statFifoOver = TRUE;
	}


	if( IS_RX_FIFO_OVERRUN_INT_SET )
	{
		CLEAR_RX_FIFO_OVERRUN_INT;
		//DISABLE_RX_FIFO_OVERRUN_INT;           //not needed, except to trigger here for trace debugging;

		//No clear needed here, Dma will continue to run and will count the over run data , only the over run data will be lost from image (per frescale)
		//CLEAR_CSI_FIFO;								// clears the CSI RX FIFO, then restarts immediately
		rxFifoOver = TRUE;
	}


	//On the stat dma done line progress interupt
	if( IS_STAT_FIFO_DMA_DONE_SET )
	{
		CLEAR_STAT_FIFO_DMA_DONE;				//clear stat fifo dma done

		if( (nInterruptCount < (INTERRUPT_COUNT_MAX - 2)) )            //if it is event 1 through 11
		{
			// indicate a line interrupt has come through
			nInterruptCount++;
			iDmaLineErrorCount = nInterruptCount;
			bEOF = FALSE;

			// if we're not already spawning the Vsync thread, and the line interrupt is enabled, spawn the line interrupt thread.
			if( (bImageLineDataEnabled == TRUE) )
			{
				iEventSet = 1;			//lowest priority thread (line thread)
			}

#ifdef THREAD_TIME_TEST_LINE
			iDmaLineErrorCountTimeTest = nInterruptCount;
#endif
		}

		REFRESH_STAT_FIFO_DMA;					//refresh the stat dma
	}


	//On a end of frame interrupt
	if( IS_EOF_INT_SET )
	{
		// Clear the interrupt flag
		CLEAR_EOF_INT;

		bEOF = TRUE;         //report correct image progress size to scan driver for EOF, so not based on nInterruptCount

		// indicate the final line interrupt has come through, should be event #12
		nInterruptCount++;

		if( nInterruptCount < 12 )         //should be at 12 here
		{
			iDmaLineErrorCount = 13;			// 13 is error indicator
		}
		else
		{
			iDmaLineErrorCount = nInterruptCount;
		}

		// if we're not already spawning the Vsync thread, and the line interrupt is enabled, spawn the line interrupt thread.
		if( (bImageLineDataEnabled == TRUE) )
		{
			iEventSet = 1;			//lowest priority thread (line thread)
		}

#ifdef THREAD_TIME_TEST_LINE
		ULONG testTicks = testWaitDelay.GetRestTicks();                  //lmc   thread time debug
		testTickTwelve = testTickStart - testTicks;
#endif
	}


	//On the DMA complete interrupt
	if( IS_BUFFER_DMA_DONE_SET )
	{
		iDmaErrorCount = nInterruptCount;

		dmaComplete = TRUE;

		ClearDmaIndicators();
		CheckDmaComplete();                 //clear interupt flags and indicate next buffer pointer to load

		if( dInitScanInLineIntCompleted == 3 )		//RX dma already set up in line interupt, so do not touch it here
		{
			//ENABLE_RX_FIFO_OVERRUN_INT;		//not needed, except to cause a trigger for trace debugging, once a frame;
			CompletedPointerFlipRxDma();
		}
		else if( dInitScanInLineIntCompleted == 2 )		//RX dma not set up in line interupt, but new pointer passed, so show error here and dma writes to NULL
		{
			bVertThreadDmaPoinerNotFlipedInThread = TRUE;	//NOTE: image pointers will be out of sink with scan driver for one frame in this error mode
		}
		else if( dInitScanInLineIntCompleted == 1 )		//RX dma not set up in line interupt, and no new pointer passed, not available,  so show error here and dma writes to NULL
		{
			bVertThreadDmaPoinerNotSent = TRUE;		//NOTE: image pointers will be out of sink with scan driver for one frame in this error mode
		}

		if( dInitScanInLineIntCompleted < 3 )		//line interupt did not set up rx dma, so handle it here by just letting it dma to NULL pointer
		{
			bInitScan = FALSE;                 //Indicate that we have initialized DMA with new pointer, even though we did not
		}

		dInitScanInLineIntCompleted = 0;             //set it back to default
		nInterruptCount = 0;                         //set stat counter for next frame
		RestartStatDma();                            //refresh stat dma just in case we had a overflow, before SOF causes clear (if set in overflow) and we run again

		//Do shut down sequence if needed
		if( ( (!bImageLineDataEnabled) && (!bVertThreadEnabled) && (iDoSynchronizedPowerDownBYD == 0)) || (iDoSynchronizedPowerDownBYDImmediate == 1) )	//power down and not enabled, disable all
		{                             //bVertThreadEnabled == false so we not allow vert thread to call scandriver, to output debug only
			bAllowThreadToCallScanDriv = false;   //do not allow line thread to call scandriver, to output debug only
			iDoSynchronizedPowerDownBYD = 1;

			if( iDoSynchronizedPowerDownBYDAbort == 1 )
			{
				iDoSynchronizedPowerDownBYD = 4;
				MT_SetEvent(StartShutDown);        //Run the shut down thread
			}
		}
		else if(iDoSynchronizedPowerDownBYD == 3)
		{
			DisableStatDma();
			DisableRxDma();
		}

		// if Vsync processing is enabled, we'll spawn the thread to call the Vsync processing in the driver
		//if( bVsyncProcessingEnabled == TRUE )
  		iEventSet = 2;			//highest priority thread (vert thread)

		// if anybody is waiting, signal them the Vsync has come
		bWaitForVsyncBYD = FALSE;

#ifdef THREAD_TIME_TEST_LINE
		ULONG testTicks = testWaitDelay.GetRestTicks();                  //lmc   thread time debug
		testTickDmaComp = testTickStart - testTicks;
#endif
	}


	//On a start of frame interrupt
	if( IS_SOF_INT_SET )
	{
	  bool bRunVertThreadInSof = FALSE;
	  bool bIsRxFifoDmaEnabled = !IS_RX_FIFO_DMA_DISABLED;



		// Clear the interrupt flag
		CLEAR_SOF_INT;

		//Check this to see if we are still enabled
		bVertThreadEnabled = bVsyncProcessingEnabled;             //This is the first flag set in the shut down process, save it!!!!

		if( !dmaComplete && bIsRxFifoDmaEnabled && bVertThreadEnabled )        //Only an error condition if dma is enabled, and dma did not happen, plus we are not trying to shut down
		{
			iDmaErrorCount = nInterruptCount;
			bRunVertThreadInSof = TRUE;

			dmaNotDoneSof = TRUE;                           //set error flag condition for dma not complete
			g_dmaNotDoneBeforeSof = TRUE;                     //set error flag condition for dma not complete and needs to be reset

			RestartStatDma();
			bInitScan = FALSE;                 //Indicate that we do not want to initialize DMA with new pointer
			dInitScanInLineIntCompleted = 0;    //set it back to default
			nInterruptCount = 0;                //line thread count
		}
		else
		{
			dmaNotDoneSof = FALSE;                  //reset error state for dma not completed
		}

		if( bIsRxFifoDmaEnabled )        //Only if dma is enabled do we need to reset this flag for next frame capture
		{
			dmaComplete = FALSE;                     //reset dma complete flag on SOF (dma complete occurs before SOF)
		}

		//Do shut down sequence if needed
		if( (!bImageLineDataEnabled) && (!bVertThreadEnabled) && (bIsRxFifoDmaEnabled) && ((iDoSynchronizedPowerDownBYD == 0) || (iDoSynchronizedPowerDownBYD == 1)) )	//power down and not enabled, disable all callbacks
		{                             //so, do not allow vert thread to call scandriver, only to output debug only
			bAllowThreadToCallScanDriv = false;   //do not allow line thread to call scandriver, to output debug only
			iDoSynchronizedPowerDownBYD = 2;

			if( iDoSynchronizedPowerDownBYDAbort == 1 )
			{
				MT_SetEvent(StartShutDown);        //Run the shut down thread			 
			}
		}

		if( bRunVertThreadInSof )
		{
			// if Vsync processing is enabled, we'll spawn the thread to call the Vsync processing in the driver
			//if( bVsyncProcessingEnabled == TRUE )
	  		iEventSet = 2;			//highest priority thread (vert thread)

			// if anybody is waiting, signal them the Vsync has come
			bWaitForVsyncBYD = FALSE;
		}


#ifdef THREAD_TIME_TEST_LINE
		testWaitDelay.SetExpiration(48000);                            //lmc  test thread time
		testTickStart = testWaitDelay.GetRestTicks();                  //lmc   thread time debug
		testTickCallThread=0; testTickInThread=0; testTickSDstart=0; testTickSDend=0, testTickTwelve=0, testTickDmaComp=0;
#endif
	}		//end if( IS_SOF_INT_SET )


	//Now set the event to run one of the threads if needed
	if( iEventSet == 2)			//highest priority imaging thread
	{
		// if Vsync processing is enabled, we'll spawn the thread to call the Vsync processing in the driver
		MT_SetEvent(vertSyncSofEvent);		//Ret=vsyncntfy_thread;      //replaced with newer model   lmc
	}
	else if( iEventSet == 1)			//lowest priority imaging thread
	{
		// if we're not already spawning the Vsync thread, and the line interrupt is enabled, spawn the line interrupt thread.
		MT_SetEvent(statFifoFullEvent);			//Ret=LineProcThread;      /replaced with newer model   lmc
	}

#ifdef THREAD_TIME_TEST
	testTickInt = testWaitDelay.GetRestTicks();                  //lmc   thread time debug
#endif

	return (threadhandle_t) 0;             //do not run a thread
}



#if CONCURRENT
void ProcessLinearScan(unsigned char *PointerToImage);
#endif



static void Do2ndThreadVsync(void *)
{
	while(TRUE)
	{
		MT_WaitEvent(vertSyncSofEvent,INFINITE);
		//MT_ResetEvent(vertSyncSofEvent);            //lmc   removed to test auto reset type above
		#if HI2D
		xputstring("vsync interrupt\r\n");
		#endif

		//---------------------------------------------------------------------------------------
#ifdef THREAD_TIME_TEST_VERT
		ULONG testTicks = testWaitDelay.GetRestTicks();
		char	ResultString[80];
		//testWaitDelay.Reset();                                         //lmc  test thread time
		//testTickStart = testWaitDelay.GetRestTicks();                  //lmc   thread time debug
		sprintf(ResultString, " TI-%d ", (testTickStart - testTickInt));
		xputstring(ResultString);
		sprintf(ResultString, " TB-%d ", (testTickStart - testTicks));
		xputstring(ResultString);
#endif//---------------------------------------------------------------------------------------

		//---------------------------------------------------------------------------------------
#ifdef THREAD_TIME_TEST_LINE
		//char	ResultString[80];
		//sprintf(ResultString, " L-%d TI-%d TB-%d TT-%d TS-%d TW-%d TD-%d ", iDmaLineErrorCountTimeTest, testTickCallThread, testTickInThread, testTickSDstart, testTickSDend, testTickTwelve, testTickDmaComp);
		//xputstring(ResultString);
#endif//---------------------------------------------------------------------------------------

		//---------------------------------------------------------------------------------------
#ifdef FIFO_OVERRUN_TEST
		if( bVertThreadDmaPoinerNotFlipedInThread && bVertThreadEnabled && GET_SETTING(DebugScanDriver) )    //indicate thread did not flip new dma pointer in the line interupt, but did it in dma complete instead
		{
			xputstring("_F_\r\n");        //lmc    debug  remove-it
		//---------------------------------------------------------------------------------------
#ifdef THREAD_TIME_TEST_LINE
			char	ResultString[80];
			sprintf(ResultString, " L-%d TI-%d TB-%d TT-%d TS-%d TW-%d TD-%d ST-%d ", iDmaLineErrorCountTimeTest, testTickCallThread, testTickInThread, testTickSDstart, testTickSDend, testTickTwelve, testTickDmaComp, iErrorStallCountSaved);
			xputstring(ResultString);
#endif//---------------------------------------------------------------------------------------
		}

		if( bVertThreadDmaPoinerNotSent && bVertThreadEnabled && GET_SETTING(DebugScanDriver) )    //indicate thread did not pass new dma pointer
		{
			xputstring("_P_\r\n");        //lmc    debug  remove-it
		}
#endif//---------------------------------------------------------------------------------------

		//---------------------------------------------------------------------------------------
#ifdef ENABLE_DMA_COMPLETE_INTERUPT_DEBUG
		if(dmaDoneTwo)	                                                   //lmc   debug
		{
			xputstring( " DMAy2 " );
		}
		if(dmaDoneOne)	                                                   //lmc   debug
		{
			xputstring( " DMAy1 " );
		}
		if(!dmaNotDoneSof)                                                   //lmc   debug
		{
			xputstring( " DMAnotSof " );
		}
#endif//---------------------------------------------------------------------------------------

		//---------------------------------------------------------------------------------------
#ifdef FIFO_OVERRUN_TEST
		if( statFifoOver && bVertThreadEnabled && GET_SETTING(DebugScanDriver) )    //indicate that the stat fifo overflowed
		{
			xputstring("_OS_");        //lmc    debug
		}
		if( rxFifoOver && bVertThreadEnabled && GET_SETTING(DebugScanDriver) )   //indicate that the rx fifo overflowed
		{
			xputstring("_OR_");        //lmc    debug
		}
		if( hrespError && bVertThreadEnabled && GET_SETTING(DebugScanDriver) )   //indicate that the rx dma errored
		{
			xputstring("_OE_");        //lmc    debug
		}

		rxFifoOver = FALSE;
		statFifoOver = FALSE;
		hrespError = FALSE;
#endif//---------------------------------------------------------------------------------------



		if( (!dmaDoneTwo && !dmaDoneOne) || dmaNotDoneSof )
		{

#ifdef ENABLE_DMA_COMPLETE_INTERUPT_DEBUG
			xputstring( " DMAno " );
#else
			if( bVertThreadEnabled && GET_SETTING(DebugScanDriver) )
			{
				//xputstring("-D-");        //lmc    debug  remove-it
		  		xputstring( " DMAno " );                  //error, did not complete dma before SOF
			}

			if( GET_SETTING(DebugScanDriver) )      //lmc   debug
			{
				char	ResultString[80];                                                 //lmc   debug
				sprintf(ResultString, "-VL%d-", iDmaErrorCount );           //lmc   debug
				xputstring(ResultString);                       								   //lmc   debug
			}
#endif

			dmaNotDoneSof = FALSE;
		}



		//---------------------------------------------------------------------------------------
#ifdef THREAD_TIME_TEST_VERT
		testTicks = testWaitDelay.GetRestTicks();                  //lmc   thread time debug
		sprintf(ResultString, " TT-%d ", (testTickStart - testTicks));
		xputstring(ResultString);
#endif//---------------------------------------------------------------------------------------



		if( bVertThreadEnabled )
		{

#ifdef THREAD_PROGRESS_INDICATOR
			xputstring("-V-");        //lmc    debug  remove-it

			if( GET_SETTING(DebugScanDriver) )      //lmc   debug
			{
				unsigned int iFrameCount = (unsigned int)GET_CSI_CONTROL_REG_THREE_FRAME_COUNT;
				char	ResultString[80];                                                 //lmc   debug
				sprintf(ResultString, "-VN%d-", iFrameCount );           //lmc   debug
				xputstring(ResultString);                                                         //lmc   debug
			}
#endif

			pfVsyncNotify();                        //lets notify scan driver

#ifdef USE_THIRD_THREAD
			MT_SetEvent(StartFastDecode);
#endif
		} 	//end  if( bVertThreadEnabled )


		//---------------------------------------------------------------------------------------
#ifdef THREAD_PROGRESS_INDICATOR
		else
		{
			if( GET_SETTING(DebugScanDriver) )      //lmc   debug
			{
				xputstring("-OV-\r\n");        //lmc    debug  remove-it
				//char	ResultString[80];                                                 //lmc   debug - remove it
				//sprintf(ResultString, " CLOSE_STATE-%d- ", iDoSynchronizedPowerDownBYD );           //lmc   debug - remove it
				//xputstring(ResultString);                       								   //lmc   debug - remove it
			}
		}
#endif//---------------------------------------------------------------------------------------

		//---------------------------------------------------------------------------------------
#ifdef THREAD_TIME_TEST_VERT
	   	testTicks = testWaitDelay.GetRestTicks();                  //lmc   thread time debug
		sprintf(ResultString, " TS-%d ", (testTickStart - testTicks));
		xputstring(ResultString);
#endif//---------------------------------------------------------------------------------------



#ifndef USE_THIRD_THREAD
#if CONCURRENT
		/* CALL YOUR FUNCTION HERE WITH
		 *  "pLastStart" AS THE POINTER TO
		 * THE LAST IMAGE */
		if( pLastStart > (unsigned char *)NON_CACHED_MEMORY_BASE )
		{
			 pLastStart = (unsigned char *)( (int)pLastStart - (int)(NON_CACHED_MEMORY_BASE - CACHED_MEMORY_BASE) );
		}
		MMU_CleanAndInvalidate();
		ProcessLinearScan( (unsigned char *)pLastStart );
#endif
#endif


		//SleepDriverThread();			//removed old thread model   lmc
	}	//end while(1)
}



static void Do2ndThreadLineSvc(void *)
{
	while(TRUE)
	{
		MT_WaitEvent(statFifoFullEvent,INFINITE);
		//xputstring("lineing...");
		//MT_ResetEvent(statFifoFullEvent);                 //lmc   removed to test auto reset type


		//---------------------------------------------------------------------------------------
#ifdef THREAD_TIME_TEST_LINE
		//char	ResultString[80];
		ULONG testTicks = testWaitDelay.GetRestTicks();
		if( (iDmaLineErrorCountTimeTest == (INTERRUPT_COUNT_MAX - 2)) )                     //lmc   debug
		{
			testTickCallThread = testTickStart - testTickInt;
			testTickInThread = testTickStart - testTicks;
			//testWaitDelay.Reset();                                         //lmc  test thread time
			//testTickStart = testWaitDelay.GetRestTicks();                  //lmc   thread time debug
			//sprintf(ResultString, " TI-%d ", (testTickStart - testTickInt));
			//xputstring(ResultString);
			//sprintf(ResultString, " TB-%d ", (testTickStart - testTicks));
			//xputstring(ResultString);
		}
#endif//---------------------------------------------------------------------------------------

		//---------------------------------------------------------------------------------------
#ifdef FIFO_OVERRUN_TEST
      	if( statFifoOver && bVsyncProcessingEnabled && GET_SETTING(DebugScanDriver) )    //indicate that the stat fifo overflowed
		{
			xputstring("_OS_");        //lmc    debug
		}
		if( rxFifoOver && bVsyncProcessingEnabled && GET_SETTING(DebugScanDriver) )     //indicate that the rx fifo overflowed
		{
			xputstring("_OR_");        //lmc    debug
		}
		if( hrespError && bVsyncProcessingEnabled && GET_SETTING(DebugScanDriver) )   //indicate that the rx dma errored
		{
			xputstring("_OE_");        //lmc    debug
		}

		rxFifoOver = FALSE;
		statFifoOver = FALSE;
		hrespError = FALSE;

		if( (GET_SETTING(DebugScanDriver)) && (iDmaLineErrorCount == 13) && bVsyncProcessingEnabled )      //lmc   debug
		{                                    													//if line count is out of wack, send out debug
			char	ResultString[80];                                                 //lmc   debug
			sprintf(ResultString, "-LF%d_%d-", iDmaLineErrorCount, nInterruptCount );           //lmc   debug
			xputstring(ResultString);                       								   //lmc   debug
		}
#endif//---------------------------------------------------------------------------------------

		//---------------------------------------------------------------------------------------
#ifdef THREAD_PROGRESS_INDICATOR
		if( (iDmaLineErrorCount >= (INTERRUPT_COUNT_MAX - 2)) && GET_SETTING(DebugScanDriver) )      //lmc   debug
		{
			char	ResultString[80];                                                 //lmc   debug
			sprintf(ResultString, "-L%d-", iDmaLineErrorCount );           //lmc   debug
			xputstring(ResultString);                       								   //lmc   debug

			if(iDmaLineErrorCount == 13)                 //error indicator
			{
				sprintf(ResultString, "-LA%d-", nInterruptCount );           //lmc   debug
				xputstring(ResultString);                       								   //lmc   debug
			}
		}
		else if( !bVsyncProcessingEnabled && (GET_SETTING(DebugScanDriver)) )
		{
			//xputstring("L");        //lmc    debug  remove-it
			char	ResultString[80];                                                 //lmc   debug
			sprintf(ResultString, "-L%d-", iDmaLineErrorCount );           //lmc   debug
			xputstring(ResultString);
		}
#endif//---------------------------------------------------------------------------------------

		//---------------------------------------------------------------------------------------
#ifdef THREAD_TIME_TEST_LINE
		if( (iDmaLineErrorCountTimeTest == (INTERRUPT_COUNT_MAX - 2)) )                     //lmc   debug
		{
			testTicks = testWaitDelay.GetRestTicks();                  //lmc   thread time debug
			testTickSDstart = testTickStart - testTicks;
			//sprintf(ResultString, " TT-%d ", (testTickStart - testTicks));
			//xputstring(ResultString);
		}
#endif//---------------------------------------------------------------------------------------



		if( bAllowThreadToCallScanDriv )
		{
			pfDataNotify();                                     //lets notify scan driver
		}



		//---------------------------------------------------------------------------------------
#ifdef THREAD_TIME_TEST_LINE
		if( (iDmaLineErrorCountTimeTest == (INTERRUPT_COUNT_MAX - 2)) )                     //lmc   debug
		{
			testTicks = testWaitDelay.GetRestTicks();                  //lmc   thread time debug
			testTickSDend = testTickStart - testTicks;
			iDmaLineErrorCountTimeTest++;
			//sprintf(ResultString, " TS-%d ", (testTickStart - testTicks));
			//xputstring(ResultString);
			iErrorStallCountSaved = iErrorStallCount;
			//sprintf(ResultString, " STALL-%d ", iErrorStallCount);
			//xputstring(ResultString);
		}
#endif//---------------------------------------------------------------------------------------


		//SleepDriverThread();
	}	//end while(1)
}



#ifdef USE_THIRD_THREAD
//static void FastLinearThread(void *)
//This function handles the thread used for the fast linear decoder
static void FastLinearThread(void *)
{
	while (true)
	{
		if (MT_WaitEvent(StartFastDecode,INFINITE))
		{
			/* CALL YOUR FUNCTION HERE WITH
			 *  "pLastStart" AS THE POINTER TO
			 * THE LAST IMAGE */
      	if (pLastStart>(unsigned char *)NON_CACHED_MEMORY_BASE)
			{
      		 pLastStart = (unsigned char *)((int)pLastStart - (int)(NON_CACHED_MEMORY_BASE - CACHED_MEMORY_BASE));
			}
			//xputstring("preprocess image now\r\n");
			MMU_CleanAndInvalidate();
			ProcessLinearScan((unsigned char *)pLastStart);
		}
	}
}

#endif		//ifdef USE_THIRD_THREAD



//static void DisableCsiInteruptsOnshutDown( void )
//disable the finale csi interupts when shut down completes, only call from shutdown routine
static void DisableCsiInteruptsOnshutDown( void )
{
	DISABLE_INTERRUPTS();

	DISABLE_STAT_FIFO_DMA;			//disable stat fifo dma
	DISABLE_STAT_FIFO_DMA_DONE;  //disable stat fifo dma done interupt

	DISABLE_RX_FIFO_DMA;          //Disable dma
	DISABLE_BUFFER_ONE_DMA_DONE;    //disable rx fifo dma done interupt
	DISABLE_BUFFER_TWO_DMA_DONE;
	DISABLE_EOF_INTERUPT;			// Disable EOF interupt which handles stat #12

	RESTORE_INTERRUPTS();
}



#ifdef USE_SHUT_DOWN_THREAD
//static void ShutDownThread(void *)
//This function handles the shutdown of the BYD imager.
static void ShutDownThread(void *)
{
	while( true )
	{//xputstring("before\r\n");
		if (MT_WaitEvent(StartShutDown,INFINITE))
		{//xputstring("after\r\n");
			int iDisableDelayCount = 40;              //40 * 30ms gets us a total wait of > 1 second for really long exposures
			//CTimeoutmS	disableDelay(25);             //25 ms wait for vert interupt to shut dma down, frame rate is < 20 ms on BYD
			CTimeoutmS	disableDelay(50);             //25 ms wait for vert interupt to shut dma down, frame rate is < 45 ms on BYD
			while( !IS_RX_FIFO_DMA_DISABLED )        //Lets wait for imager to complete last frame to shut down
			{
				if( (iDoSynchronizedPowerDownBYDPowerOff == 1) ) //imager power down command has already been sent,
				{														//doing a power down, no I2C, so shut down with no delay
					iDisableDelayCount = 0;
					DisableCsiInteruptsOnshutDown();

					//theStdInterface.printf(" SyncIIC Power Down , add %x val %x cnt %x \r\n",subaddressForSynchronizedPowerDownBYD, uiBufferForSynchronizedPowerDownBYD,nCountForSynchronizedPowerDownBYD);    //lmc   debug
					//xputstring(" DMA_CLOSE_WITH_POWER_DOWN ");					//lmc   debug - remove it

					break;
				}

				if( (iDoSynchronizedPowerDownBYD == 1) && (iDoSynchronizedPowerDownBYDImmediate == 1) ) //single frame capture has already sent standby after good frame,
				{																									//doing a power down, no I2C, so shut down current frame when dma has been completed.
					iDisableDelayCount = 0;
	                		DisableCsiInteruptsOnshutDown();

					//theStdInterface.printf(" SyncIIC DMA Completed , add %x val %x cnt %x \r\n",subaddressForSynchronizedPowerDownBYD, uiBufferForSynchronizedPowerDownBYD,nCountForSynchronizedPowerDownBYD);    //lmc   debug
					//xputstring(" DMA_CLOSE_SINGLE_FRAME ");					//lmc   debug - remove it

					break;
				}

				if( ((iDoSynchronizedPowerDownBYD == 2) || (iDoSynchronizedPowerDownBYD == 4)) && (iDoSynchronizedPowerDownBYDAbort == 1) ) //do an abort for quick BYD shutdown on current line,
				{                                                          							  		//so shut down when you can send the I2C abort command.
					if( subaddressForAbortBYD == 0x09 )
					{
					//Kyle???TODO
						unsigned char reg = 0x11;
						unsigned short value = 0xA0;

						WriteIIC_BYD( subaddressForAbortBYD, &uiBufferForAbortBYD, nCountForAbortBYD );
						WriteIIC_BYD(reg,&value,1);
					}
					if( iDoSynchronizedPowerDownBYD == 4 )
					{
						//MT_Delay(1);            //wait for line to complete in BYD
	                			DisableCsiInteruptsOnshutDown();
						//xputstring(" DMA_CLOSE_W_ABORT ");					//lmc   debug - remove it
						break;
					}
					else
					{
						disableDelay.Reset();                   //wait another 45 ms for next DMA complete to turn off RX_FIFO_DMA
						iDoSynchronizedPowerDownBYD = 3;
						//xputstring(" DMA_CLOSE_TLF_ABORT !!!!!!!!!!!!!!!!!!!!!!!!!!!");//ken here notice					//lmc   debug - remove it
					}
				}

				if( (iDoSynchronizedPowerDownBYD == 2) && (iDoSynchronizedPowerDownBYDAbort == 0) )
				{
					if( subaddressForSynchronizedPowerDownBYD == 0x09 )
					{
						
						WriteIIC_BYD( subaddressForAbortBYD, &uiBufferForAbortBYD, nCountForAbortBYD );
						unsigned char reg = 0x11;
						unsigned short value = 0xA0;
						WriteIIC_BYD(reg,&value,1);
						
						disableDelay.Reset();
						iDoSynchronizedPowerDownBYD = 3;
					}
				}

				if( disableDelay.HasExpired() )        //reset dma if it was not done by synchronous shutdown in the vert interupt, Sync with DMA complete to shut down
				{
					if( (iDoSynchronizedPowerDownBYD < 3) && (iDisableDelayCount > 0) )		//if we are in the active vert, then wait for DMA complete to reset nInterruptCount
					{
						iDisableDelayCount--;                   //wait up to a total of 1 sec for long exposures
						disableDelay.Reset();                   //wait another 25 ms for next DMA complete to turn off RX_FIFO_DMA

						//char	ResultString[80];                                                 //lmc   debug - remove it
						//sprintf(ResultString, " DMA_CLOSE-%d,%d- ", iDisableDelayCount, iDoSynchronizedPowerDownBYD );           //lmc   debug - remove it
						//xputstring(ResultString);                       								   //lmc   debug - remove it
						//theStdInterface.printf(" SyncIIC , add %x val %x cnt %x \r\n",subaddressForSynchronizedPowerDownBYD, uiBufferForSynchronizedPowerDownBYD,nCountForSynchronizedPowerDownBYD);    //lmc   debug
					}
					else
					{
				         DisableCsiInteruptsOnshutDown();

						//xputstring(" DMA_CLOSE ");					//lmc   debug - remove it

						break;
					}
				}

				//theStdInterface.printf(" CurrentState %d, AbortState %d\r\n", iDoSynchronizedPowerDownBYD, iDoSynchronizedPowerDownBYDAbort );     //lmc   debug - remove it

				MT_Delay(1);
			}				//end while( !IS_RX_FIFO_DMA_DISABLED)

			bInitScan = FALSE;
			nInterruptCount = 0;          //reset line count
			subaddressForSynchronizedPowerDownBYD = 0;           //clear to indicate no register has been selected
			subaddressForAbortBYD = 0;						//clear to indicate no register has been selected
			iDoSynchronizedPowerDownBYD = 0;                //clear to indicate that shut down is complete
			iDoSynchronizedPowerDownBYDImmediate = 0;       //clear to indicate that shut down is complete
			iDoSynchronizedPowerDownBYDAbort = 0;			//clear to indicate that shut down is complete
			iDoSynchronizedPowerDownBYDPowerOff = 0;			//clear to indicate that shut down is complete

			//xputstring(" DMA_DONE ");	//spear				//lmc   debug - remove it

		}			//end if (MT_WaitEvent(StartShutDown,INFINITE))
	}			//end while( true )
}

#endif		//end ifdef USE_SHUT_DOWN_THREAD


DWORD RegisterSofPsocWriteNotificationBYD(void(*fSofNotification)(void))
{
	return 1;
}


DWORD InitializeVsyncProcessingBYD(void)
{
	fImageRegisterRead = ReadRegisterBYD;
	fImageRegisterWrite = WriteRegisterBYD;
	pfSetForCachedImaging = UseCachedMemoryBYD;

	//Set up Threads
	if( vsyncntfy_thread == 0 )
	{
		vsyncntfy_thread = MT_OpenDriverThread(SCAN_PRIORITY+2,VSYNC_STACK_SIZE,&Do2ndThreadVsync,NULL);
		vertSyncSofEvent = MT_CreateEvent_Ex(false,true);
		WakeDriverThread(vsyncntfy_thread);
	}

#ifdef USE_THIRD_THREAD
	if (fastlinear_thread == 0)
	{
		fastlinear_thread = MT_OpenDriverThread(FASTDECODE_PRIORITY,FASTDECODE_STACK_SIZE,&FastLinearThread,NULL);
		StartFastDecode = MT_CreateEvent_Ex(false,true);
		WakeDriverThread( fastlinear_thread );
	}
#endif

#ifdef USE_SHUT_DOWN_THREAD
	if( shut_down_thread == 0 )
	{
		shut_down_thread = MT_OpenDriverThread(SCAN_PRIORITY+3,SHUTDOWN_STACK_SIZE,&ShutDownThread,NULL);
		StartShutDown = MT_CreateEvent_Ex(false,true);
		WakeDriverThread(shut_down_thread);
	}
#endif

	// Here we disable all the low level interrupts.
	DISABLE_INTERRUPTS();

	//NEED configure BYD
	SET_CSI_PORT_MUX_MODE;
	ConfigureBYDPins();
	//SET_MCLK_DIV;								//MCLK operates at 24 MHz
	//ENABLE_MCLK;								//Turn on MCLK!!
	uiHoldCsicr1 = 0;                     		//force pin configuration to be clear
	uiHoldCsicr2 = 0;
	uiHoldCsicr3 = 0;

	SELECT_PACK_DIR_LSB;              			//LITTLE ENDIAN
	SELECT_PIXEL_BIT_EIGHT;						//BYD is eight bits wide
	SELECT_TWO_EIGHT_BIT_SENSOR_ONE_USED;		//BYD is a single 8bit imager


	// Configure the CSI Control register, this does every bit as per the manual.
	// Now we disable the low level interrupt for overruns of the receive FIFO
	SET_STAT_FIFO_FULL_LEV;
	SET_STAT_DMA_BURST_TYPE;                              //set burst to incr16
	SET_RX_FIFO_FULL_LEV;
	SET_RX_DMA_BURST_TYPE;											//set burst to incr8

	ENABLE_GATED_CLK_MODE;			/* Gate pixel clock with HSync*/
	SELECT_VALID_PIXEL_CLK_EDGE_RISE;	// Pixel Data output on falling edge, and latches on rising edge
//	SELECT_VALID_PIXEL_CLK_EDGE_FALL;
	SELECT_ACTIVE_HSYNC_POL;				//HSYNC is valid when high (valid pixels)

// statistics fifo set up
	SET_DOUBLE_RESOLUTION_MODE;
	ENABLE_SKIP_COUNT;
	SET_LIVE_VIEW_RES_MODE;

	//Kyle???TODO: how to calculate it???
	SET_VERTICAL_SKIP_COUNT_TO((unsigned long)(7));                //7 rows
	SET_HORIZONTAL_SKIP_COUNT_TO((unsigned long)(15));                //15 pixels

	// Set up the Vsync function as our ISR for the CSI interrupt.

	SetVector(VsyncServiceBYD,VEC_CSI_INT);

	//As a test, use a fast interupt
	//UseFastInterrupt(VEC_CSI_INT);                               //lmc  fast interrupt test

	//As a test, use a priority on normal interupt
	SetNormalInterruptPriority(VEC_CSI_INT,7);                     //lmc  high priority normal interupt test

	// Enable interrupts from the CSI to interrupt the core
	EnableInterruptNumber(VEC_CSI_INT);

	// Clear the pending bits for the overruns, fills of FIFO, and start of frame
	CLEAR_ALL_INTERUPT_STATUS;
	DISABLE_SYNC_FCC;						//disable sync clearing to do it manually
	CLEAR_CSI_FIFO;
	//CLEAR_STAT_FIFO;           // clear the STAT FIFO, then waits for next SOF to restart        /*will hold FIFO in reset until next SOF */
	//ENABLE_SYNC_FCC;						//enable sync fifo clearing

	// Enable the start of frame interrupt
	SET_SOF_POLARITY_RISING;                 //use overload if you want SOF to fire at SOF (end of vert. blanking)
	//SET_SOF_POLARITY_FALLING;                //use this overload if you want SOF to fire on actual EOF
	dmaComplete = TRUE;		//If tri stated, might get a SOF when imager enabled, so do not allow anything to occur in SOF during first frame
	ENABLE_SOF_INTERUPT;

	DISABLE_EOF_INTERUPT;	 				// Disable EOF interupt which handles stat #12

	DISABLE_HRESP_ERROR_INT;            //turn off hresp error interupt

// Here we DISABLE the RX Fifo DMA and interrupt, because we're going to let the VSYNC throttle when it
// gets enabled.
	DISABLE_RX_FIFO_DMA;                 //disable dma
	DISABLE_RX_FIFO_FULL_INTERUPT;
	DISABLE_BUFFER_ONE_DMA_DONE;        //disable dma buffer one done
	DISABLE_BUFFER_TWO_DMA_DONE;        //disable dma buffer two done
	DISABLE_RX_FIFO_OVERRUN_INT;

// Here we DISABLE the Stat Fifo DMA and interrupt, because we're going to let the VSYNC throttle when it
// gets enabled.
	DISABLE_STAT_FIFO_INT;        //disable stat fifo full interupt
	DISABLE_STAT_FIFO_DMA;			//disable stat fifo dma
	DISABLE_STAT_FIFO_DMA_DONE;  //disable stat fifo dma done interupt
	DISABLE_STAT_FIFO_OVERRUN_INT;

// 3 lines added for power considerations -- similar to toshibahw.cpp
	uiHoldCsicr1  = CSI_CONTROL_REG_ONE;   /*save the int of these regs so we can quickly restore them after turning on the CSI */
	uiHoldCsicr2  = CSI_CONTROL_REG_TWO;
	uiHoldCsicr3  = GET_CSI_CONTROL_REG_THREE;
	DISABLE_CSI;             /*Turn off CSI to save power */               //ONLY WORKS ON mxl platform

	RESTORE_INTERRUPTS();

	//Wait 1 ms per spec to init imager after power and clock before talking or imaging
	MT_Delay(2);

	return 1;
}


DWORD DeinitializeVsyncProcessingBYD(void)
{
	DISABLE_INTERRUPTS();
	// Enable interrupts from the CSI to interrupt the core
	DisableInterruptNumber(VEC_CSI_INT);
	// Here we are disabling the low level start of frame interrupt.
	DISABLE_SOF_INTERUPT;

	// We clear the status for the start of frame interrupt
	CLEAR_SOF_INT;

	DISABLE_MCLK;                     //Turn off MCLK!!

	MT_CloseEvent(vertSyncSofEvent);

#ifdef USE_THIRD_THREAD
	MT_CloseEvent(StartFastDecode);
#endif

#ifdef USE_SHUT_DOWN_THREAD
	MT_CloseEvent(StartShutDown);
#endif

	RESTORE_INTERRUPTS();

	MT_CloseDriverThread(vsyncntfy_thread);
	vsyncntfy_thread = 0;

#ifdef USE_THIRD_THREAD
	MT_CloseDriverThread(fastlinear_thread);
	fastlinear_thread = 0;
#endif

#ifdef USE_SHUT_DOWN_THREAD
	MT_CloseDriverThread(shut_down_thread);
	shut_down_thread = 0;
#endif


	return 1;
}


//======================================================================================
DWORD RegisterImageLineDataNotificationBYD(void(*fImageNotification)(void))
{
	pfDataNotify = fImageNotification;
	return 1;
}


DWORD InitializeImageLineDataProcessingBYD(void)
{
	if (LineProcThread==0)
	{
		LineProcThread = MT_OpenDriverThread(SCAN_PRIORITY+1,LINE_STACK_SIZE,Do2ndThreadLineSvc,NULL);
		statFifoFullEvent = MT_CreateEvent_Ex(false,true);
		WakeDriverThread(LineProcThread);
	}

	return 1;
}


DWORD DeinitializeImageLineDataProcessingBYD(void)
{
	DISABLE_INTERRUPTS();

//	// Unload the vector function from the ISR table
	MT_CloseEvent(statFifoFullEvent);

	RESTORE_INTERRUPTS();

	MT_CloseDriverThread(LineProcThread);
	LineProcThread=0;

	return 1;
}


DWORD EnableImageLineDataProcessingBYD(void)
{

	bool bEnableDelayHasNotExpired = true;
	CTimeoutmS	enableDelay(100);		// wait for shut dma to complete, typical shut down is less than 30 ms, 
										// (This delay was 1 sec, some logcial error of sensor power down will lead this 1 sec deday happenned, 
										// when first trigger if use USB cable).

	while( (iDoSynchronizedPowerDownBYD || iDoSynchronizedPowerDownBYDImmediate || iDoSynchronizedPowerDownBYDAbort || iDoSynchronizedPowerDownBYDPowerOff) && bEnableDelayHasNotExpired )
	{
		MT_Delay(1);            //wait for shut down to complete in BYD before we start to image again
		bEnableDelayHasNotExpired = !enableDelay.HasExpired();
	}

	if( !bEnableDelayHasNotExpired )		//if we have timed out, shut down has failed, so lets Slam Chit Down, make sure imager is off before we start again
	{														//make sure imager is off before we start again
		//we don't need judge bImagerIsOn in case of it's wrong
//		if( bImagerIsOn )		// if imager is on, lets restart things
		{
			//Kyle???TODO: do we need to put sesor into standby mode?
			ImagerPowerDownBYD();	//lets start from scratch
			MT_Delay(2);
		}

	    DISABLE_INTERRUPTS();
		DISABLE_STAT_FIFO_DMA;			//disable stat fifo dma
		DISABLE_STAT_FIFO_DMA_DONE;  //disable stat fifo dma done interupt
		DISABLE_RX_FIFO_DMA;          //Disable dma
		DISABLE_BUFFER_ONE_DMA_DONE;    //disable rx fifo dma done interupt
		DISABLE_BUFFER_TWO_DMA_DONE;
		DISABLE_EOF_INTERUPT;			// Disable EOF interupt which handles stat #12
		RESTORE_INTERRUPTS();

		bInitScan = FALSE;
		nInterruptCount = 0;          //reset line count
		subaddressForSynchronizedPowerDownBYD = 0;           //clear to indicate no register has been selected
		subaddressForAbortBYD = 0;						//clear to indicate no register has been selected
		iDoSynchronizedPowerDownBYD = 0;                //clear to indicate that shut down is complete
		iDoSynchronizedPowerDownBYDImmediate = 0;       //clear to indicate that shut down is complete
		iDoSynchronizedPowerDownBYDAbort = 0;			//clear to indicate that shut down is complete
		iDoSynchronizedPowerDownBYDPowerOff = 0;			//clear to indicate that shut down is complete

		if( GET_SETTING(DebugScanDriver) )
		{
			xputstring(" SLAM_CHIT ");        //lmc    debug
		}
	}


	DisableInterruptNumber(VEC_CSI_INT);


	//NEED TO ADD DMA CLOCK ENABLE                 //LMC

	DISABLE_HRESP_ERROR_INT;            //turn off hresp error interupt

	DISABLE_EOF_INTERUPT;	 				// Disable EOF interupt which handles stat #12
	DISABLE_RX_FIFO_DMA;		                            //DISABLE DMA CHANNEL
	DISABLE_RX_FIFO_FULL_INTERUPT;
	DISABLE_BUFFER_ONE_DMA_DONE;
	DISABLE_BUFFER_TWO_DMA_DONE;
	SET_RX_DMA_BURST_TYPE;											//set burst to incr8
	DISABLE_RX_FIFO_OVERRUN_INT;

	DISABLE_STAT_FIFO_INT;						    //disable stat fifo interupt
	DISABLE_STAT_FIFO_DMA;				//disable stat fifo dma
	DISABLE_STAT_FIFO_DMA_DONE;  		//disable stat fifo dma done interupt
	SET_STAT_DMA_BURST_TYPE;                              //set burst to incr16
	DISABLE_STAT_FIFO_OVERRUN_INT;

	bFirstFrame = TRUE;                     //enable the dma initialization of dma in load capture info only, not in init capture

	bImageLineDataEnabled = TRUE;

	EnableInterruptNumber(VEC_CSI_INT);

#ifdef ENABLE_FUNCTION_CALL_DEBUG
	xputstring(" EnaImgLine2 ");        //lmc    debug
#endif

	return 1;
}


DWORD DisableImageLineDataProcessingBYD(void)
{
	DISABLE_INTERRUPTS();

	DISABLE_STAT_FIFO_INT;    //disable stat fifo interupt
	//DISABLE_STAT_FIFO_DMA;			//disable stat fifo dma                     //lmc,  lets allow last vert interupt to do this to sync it
	//DISABLE_STAT_FIFO_DMA_DONE;  //disable stat fifo dma done interupt         //lmc,  lets allow last vert interupt to do this to sync it
	DISABLE_STAT_FIFO_OVERRUN_INT;     //turn off over run error interupt
	//nInterruptCount = 0;                                                       //lmc,  lets allow last vert interupt to do this to sync it

	//DISABLE_EOF_INTERUPT;		// Disable EOF interupt which handles stat #12	  //lmc,  lets allow last vert interupt to do this to sync it
	//DISABLE_RX_FIFO_DMA;                  //Disable dma                        //lmc,  lets allow last vert interupt to do this to sync it
	DISABLE_RX_FIFO_FULL_INTERUPT;         //lmc   debug
	//DISABLE_BUFFER_ONE_DMA_DONE;                                                //lmc,  lets allow last vert interupt to do this to sync it
	//DISABLE_BUFFER_TWO_DMA_DONE;
	DISABLE_RX_FIFO_OVERRUN_INT;          //turn off over run error interupt

	DISABLE_HRESP_ERROR_INT;            //turn off hresp error interupt

	bImageLineDataEnabled = FALSE;		//shuts down the line thread while we close down in while loop below


	RESTORE_INTERRUPTS();

#ifdef USE_ABORTS_BYD
	//xputstring("here\r\n");
	if( (iDoSynchronizedPowerDownBYDAbort != 1) && ((iDoSynchronizedPowerDownBYDPowerOff == 1) || (iDoSynchronizedPowerDownBYDImmediate == 1)) )
	{	//run if not an abort shutdown and we are shutting down because power down or imediate after single frame capture, the abort shut down event is set in the interupt
		//xputstring("sss\r\n");
		MT_SetEvent(StartShutDown);        //Run the shut down thread
	}

#else

	MT_SetEvent(StartShutDown);        //Run the shut down thread

#endif	//end ifdef USE_ABORTS_BYD


#ifdef ENABLE_FUNCTION_CALL_DEBUG
	xputstring(" DisImgLine2 ");        //lmc    debug
#endif

	return 1;
}


DWORD EnableVsyncProcessingBYD(void)
{
	DisableInterruptNumber(VEC_CSI_INT);

#if CONCURRENT
   pCurrentStart = pLastStart = NULL;
#endif

	SET_MCLK_DIV; 							//MCLK operates at 48 MHz
	ENABLE_MCLK;							//Turn on MCLK!!

	if( !bInitScan )          //if this is first frame, then we need to restart SOF here, was turned off in LoadCaptureInfoBYD()
	{
		CLEAR_ALL_INTERUPT_STATUS;
		ENABLE_SOF_INTERUPT;
		ENABLE_EOF_INTERUPT;				//enable EOF interupt

		//Also Enable these at the same time, was done, was done in LoadCaptureInfoBYD()
		ENABLE_RX_FIFO_DMA;                                   //enable dma
		ENABLE_BUFFER_ONE_DMA_DONE;      //turn on rx fifo dma done one interupt
		ENABLE_BUFFER_TWO_DMA_DONE;       //turn on rx fifo dma done two interupt

#ifdef FIFO_OVERRUN_TEST
		ENABLE_RX_FIFO_OVERRUN_INT;		//not needed, except to cause a trigger for trace debugging, once a frame, or error detection;
#endif

		REFRESH_RX_FIFO_DMA;                                  //refresh dma

		//ENABLE_STAT_FIFO_INT;                                 //enable line interupt   	//lmc not currently being used
		
		if (IsROIChanged())
		{
			DISABLE_STAT_FIFO_DMA;								//disable stat fifo dma
			DISABLE_STAT_FIFO_DMA_DONE;                            //disable dma complete line interupt
			DISABLE_STAT_FIFO_OVERRUN_INT;		  //not needed, except to cause error detection;
		}
		else
		{
			ENABLE_STAT_FIFO_DMA;								//enable stat fifo dma
			ENABLE_STAT_FIFO_DMA_DONE;                            //enable dma complete line interupt
			//xputstring("enable stat isr\r\n");
			#ifdef FIFO_OVERRUN_TEST
				ENABLE_STAT_FIFO_OVERRUN_INT;		//not needed, except to cause error detection;
			#endif

			REFRESH_STAT_FIFO_DMA;										//refresh the stat dma
 		}

#ifdef FIFO_OVERRUN_TEST
		ENABLE_HRESP_ERROR_INT;			//not needed, except to cause error detection;
#endif

		iDmaCount = 0;                    //lmc debug remove

		//MT_Delay(2);                    //time is in milliseconds to wait before we trigger imager to capture first image
	}

   	bVsyncProcessingEnabled = TRUE;

	g_dmaNotDoneBeforeSof = FALSE;          //global variable for DMA not complete

	EnableInterruptNumber(VEC_CSI_INT);

	MT_Delay(1);                    //time is in milliseconds to wait before we trigger imager to capture first image

#ifdef ENABLE_FUNCTION_CALL_DEBUG
	xputstring(" EnaVProc2 ");        //lmc    debug
#endif

	return 1;
}

DWORD DisableVsyncProcessingBYD(void)
{
	DISABLE_INTERRUPTS();

	bVsyncProcessingEnabled = FALSE;

	RESTORE_INTERRUPTS();

#ifdef ENABLE_FUNCTION_CALL_DEBUG
	xputstring(" DisVProc2 ");        //lmc    debug
#endif

	return 1;
}


DWORD GetNumberOfValidRowsBYD(void)
{
	DWORD ValidRows = 0;

	if (bEOF)
	{
		//ValidRows = NUMBER_OF_ROWS_VALID_EOF;
		ValidRows = ROIPreEndOfFrameRows();
		bEOF = FALSE;
	}
	else
	{
		//ValidRows = ((nInterruptCount==0)?0:(((nInterruptCount-1)*((unsigned long)((NATIVE_IMAGE_HEIGHT-4)/12)))+32));  // -4 was a test case to even division
		ValidRows = ((nInterruptCount==0)?0:(((nInterruptCount-1)*((unsigned long)((NATIVE_IMAGE_HEIGHT)/12))) + 40));  // ken@2011.11.15 for HI2D to improve performance
	}

	//theStdInterface.printf(" IntCount %d ValidRows %d ",nInterruptCount,ValidRows);                     //lmc   debug

	return ValidRows;
}


/*======================================================================================
 * Diagnostic functions
 *======================================================================================*/


// these are replacing the historical Product Config pass up values for a number of reasons --> mostly because the PSOC
// will be the intended solution and these only want to be backups in case they are "invalid" (but also because
// we are trying to get away from using Product Config and if nothing done, Product Config could have Icmedia center
// coordinates and that would be bad.

BOOL GetImagerPropertiesBYD( const DWORD * pBuffIn, DWORD * pBuffOut, DWORD dwLen )
{
	for (unsigned i = 0; i < dwLen / sizeof(DWORD); i += 2)
	{
		DWORD tag = *pBuffIn;
		pBuffIn += 2;
		*pBuffOut++ = tag;

		DWORD value = (DWORD)-1;

		switch(tag)
		{
		case IP_SIZE:              
			value = 0;                            
			break;
		case IP_ENGINE_ID:         
			value = 0;      
			break;
		case IP_NUM_ROWS:          
			value = GetScanHeightBYD();       
			break;
		case IP_NUM_COLS:          
			value = GetScanWidthBYD();        
			break;
		case IP_BITS_PER_PIXEL:    
			value = 8;                           
			break;
		case IP_AIMER_X_OFFSET:
			value = NATIVE_IMAGE_WIDTH >> 1;
			break;
		case IP_AIMER_Y_OFFSET:
			value = NATIVE_IMAGE_HEIGHT >> 1;
			break;
		case IP_YDEPTH:            
			value = 0;    
			break;
		case IP_ROTATION:
			value = 0;
			break;
        case IP_PIXEL_CLOCK:
            value = 0;// 1 for 24MHz, 0 for 12MHz.
            break;
		default:
			DBG_OUTPUT1("GetImagerPropertiesBYD: Unknown property %d "
					   "requested\r\n", tag);
			break;
		}
		*pBuffOut++ = value;
	}
	return TRUE;
}


//ConvertHexCharacters()
//Pass in a hex char to convert
//return value of 0x00 to 0x0f if passes
//return value of 0x3f ('?' or 63) if a failure
//
unsigned char ConvertHexCharacters( unsigned char ucTempChar )
{
	if( (ucTempChar >= '0') && (ucTempChar <= '9') )
	{
		ucTempChar -= 0x30;
	}
	else if( ucTempChar > '9' )
	{
		if( (ucTempChar >= 'A') && (ucTempChar <= 'F') )
		{
			ucTempChar -= 0x37;
		}
		else if( (ucTempChar >= 'a') && (ucTempChar <= 'f') )
		{
			ucTempChar -= 0x57;
		}
		else
		{
			xputstring("ILLEGAL CHARACTER!!!\r\n");
			ucTempChar = 0x3f;			//error
		}
	}
	else
	{
		xputstring("ILLEGAL CHARACTER!!!\r\n");
		ucTempChar = 0x3f;			//error
	}

	return ucTempChar;
}


BOOL WriteRegisterBYD(void)
{
	UINT nSizeOfString,i=0,nIndex;
	unsigned char ch[5],OutputString[40];
	unsigned char *pInput;
	unsigned char nAddress;
	unsigned short nValues[100];

	pInput = ch;                                                        //temp to get rid or warning till we add menuing   lmc
	nSizeOfString = 0;                                                  //temp to get rid or warning till we add menuing   lmc
	//Get Data
	pInput = ReaderConfiguration->ExtraneousTextAfterCommand->Char;
	nSizeOfString = ReaderConfiguration->ExtraneousTextAfterCommand->Size;

	if (nSizeOfString < 7) return 0; // Make sure there's atleast a reg and write val for that reg.

	if ((pInput[0] == '0')&&((pInput[1] == 'x')||(pInput[1] == 'X')))      //is it a hex value?
	{
		if (nSizeOfString < 6) return 0;
		//Get third character
		i = 2;

		//Convert first Hex character (third input char)
		ch[0] = pInput[i++];
		ch[0] = ConvertHexCharacters( ch[0] );
		if( ch[0] == 0x3f )	return 0;	//error

		//Convert second Hex character (fourth input char)
		ch[1] = pInput[i++];
		ch[1] = ConvertHexCharacters( ch[1] );
		if( ch[1] == 0x3f )	return 0;	//error

		//Set address byte
		nAddress = (ch[0]<<4)+(ch[1]);
	}
	else //if not an hex address, convert decimal address
	{
		ch[0] = pInput[i++];		ch[1] = pInput[i++];		ch[2] = pInput[i++];

		if( (ch[0] >= '0') && (ch[0] <= '9') && (ch[1] >= '0') && (ch[1] <= '9') && (ch[2] >= '0') && (ch[2] <= '9') )
		{
			nIndex = (((UINT)(ch[0]-0x30))*100) + (((UINT)(ch[1]-0x30))*10) + ((UINT)(ch[2]-0x30));

			if( nIndex < 256 )
			{
				nAddress = (unsigned char)(nIndex&0xff);
			}
			else
			{
				xputstring("OUT OF RANGE!!!\r\n");
				return 0;
			}
		}
		else
		{
			xputstring("NON DECIMAL CHARACTER!!!\r\n");
			return 0;
		}
	}

	//Now lets process each two byte hex data value following the address
	for (nIndex=0; i<nSizeOfString; i+=4)
	{
		//get most significant nibble and convert hex characters
		ch[0] = pInput[i];
		ch[0] = ConvertHexCharacters( ch[0] );
		if( ch[0] == 0x3f )	return 0;	//error

		//get second nibble and convert
		ch[1] = pInput[i+1];
		ch[1] = ConvertHexCharacters( ch[1] );
		if( ch[1] == 0x3f )	return 0;	//error

		//get third nibble and convert
		ch[2] = pInput[i+2];
		ch[2] = ConvertHexCharacters( ch[2] );
		if( ch[2] == 0x3f )	return 0;	//error

		//get least significant nibble and convert hex characters
		ch[3] = pInput[i+3];
		ch[3] = ConvertHexCharacters( ch[3] );
		if( ch[3] == 0x3f )	return 0;	//error

		//store the data
		nValues[nIndex++] = (((unsigned short)ch[0])<<12) + (((unsigned short)ch[1])<<8) + (((unsigned short)ch[2])<<4) + ((unsigned short)ch[3]);
	}

	DWORD	ImagerWasAlreadyPoweredUp = ImagerIsPoweredBYD();
	if( !ImagerWasAlreadyPoweredUp )
	{
		ImagerPowerUpBYD();
		WaitMillisecondsBYD(5);
	}

	if (WriteIIC_BYD(nAddress,&nValues[0],nIndex))
	{
		for (i=1; i<nIndex; i++)
		{
			sprintf((char *)OutputString,"Reg:%3d (0x%02x), Wrote:0x%04x\r\n",(UINT)nAddress,(UINT)nAddress,nValues[i]);
			xputstring((const char *)OutputString);
		}

		if( !ImagerWasAlreadyPoweredUp )		ImagerPowerDownBYD();

		return(TRUE);
	}
	else
	{
		xputstring((const char *)"I2C WRITE FAILED\r\n");
	}

	if( !ImagerWasAlreadyPoweredUp )		ImagerPowerDownBYD();

	return 0;
}


BOOL ReadRegisterBYD(void)
{
	UINT nSizeOfString,i=0,nIndex;
	unsigned char ch[3],ReadReg,OutputString[50];
	unsigned short nReadVal;
	unsigned char *pInput;
	BOOL bStatus=TRUE;

	pInput = ch;                                                        //temp to get rid or warning till we add menuing   lmc
	nSizeOfString = 0;                                                  //temp to get rid or warning till we add menuing   lmc
	pInput =  ReaderConfiguration->ExtraneousTextAfterCommand->Char;
	nSizeOfString = ReaderConfiguration->ExtraneousTextAfterCommand->Size;

	if( nSizeOfString < 3 )		return 0;	// Make sure it's non-zero

	if( (pInput[0] == '0') && ((pInput[1] == 'X') || (pInput[1] == 'x')) )
	{
		if (nSizeOfString < 4)	return 0;

		i = 2;

		for( nIndex=0; i<nSizeOfString; i+=2 )
		{
			//get most significant nibble and convert hex characters
			ch[0] = pInput[i];
			ch[0] = ConvertHexCharacters( ch[0] );
			if( ch[0] == 0x3f )	return 0;	//error

			//get least significant nibble and convert hex characters
			ch[1] = pInput[i+1];
			ch[1] = ConvertHexCharacters( ch[1] );
			if( ch[1] == 0x3f )	return 0;	//error

			//address to read
			ReadReg = (ch[0]<<4)+(ch[1]);

			DWORD	ImagerWasAlreadyPoweredUp = ImagerIsPoweredBYD();
			if( !ImagerWasAlreadyPoweredUp )
			{
				ImagerPowerUpBYD();
				WaitMillisecondsBYD(5);
			}

			if (ReadIIC_BYD(ReadReg,&nReadVal,1))
			{
				sprintf((char *)OutputString,"Reg:%3d (0x%02x), ReadBack:0x%04x\r\n",(UINT)ReadReg,(UINT)ReadReg,(UINT)nReadVal);
			}
			else
			{
				sprintf((char *)OutputString,"I2C READ FAILED\r\n");
				bStatus = FALSE;
			}

			if( !ImagerWasAlreadyPoweredUp )		ImagerPowerDownBYD();

			SendStringToInterface( (const char *)OutputString, strlen(  (const char *)OutputString ) );
		}		//end for()
	}
	else	//if not an hex address, convert decimal address
	{
		if( nSizeOfString % 3 )		return 0;

		while( i < nSizeOfString )
		{
			ch[0] = pInput[i++];		ch[1] = pInput[i++];		ch[2] = pInput[i++];

			if( (ch[0] >= '0') && (ch[0] <= '9') && (ch[1] >= '0') && (ch[1] <= '9') && (ch[2] >= '0') && (ch[2] <= '9') )
			{
				nIndex = (((UINT)(ch[0]-0x30))*100) + (((UINT)(ch[1]-0x30))*10) + ((UINT)(ch[2]-0x30));

				if (nIndex > 255)
				{
					sprintf((char *)OutputString,"OUT OF RANGE!!!\r\n");
					SendStringToInterface( (const char *)OutputString, strlen(  (const char *)OutputString ) );
					return 0;
				}

				//address to read
				ReadReg = (unsigned char)(nIndex&0xff);

				DWORD	ImagerWasAlreadyPoweredUp = ImagerIsPoweredBYD();
				if( !ImagerWasAlreadyPoweredUp )
				{
					ImagerPowerUpBYD();
					WaitMillisecondsBYD(5);
				}

				if (ReadIIC_BYD(ReadReg,&nReadVal,1))
				{
					sprintf((char *)OutputString,"Reg:%3d (0x%02x), ReadBack:0x%04x\r\n",(UINT)ReadReg,(UINT)ReadReg,(UINT)nReadVal);
				}
				else
				{
					sprintf((char *)OutputString,"I2C READ FAILED\r\n");
					bStatus = FALSE;
				}

				if( !ImagerWasAlreadyPoweredUp )		ImagerPowerDownBYD();

				SendStringToInterface( (const char *)OutputString, strlen(  (const char *)OutputString ) );
			}
			else
			{
				sprintf((char *)OutputString,"NON DECIMAL CHARACTER : %c, %c, %c \r\n",ch[0],ch[1],ch[2]);
				SendStringToInterface( (const char *)OutputString, strlen( (const char *)OutputString ) );
				return 0;
			}
		}		//end while()
	}
	return(bStatus);
}


//*********************** Testflex***********************************************
// Code for placing imager in and out of grayscale test mode

/*----------------------------------------------------------------------------------------
SetImagerModeToGrayscaleBYD

  Sets the imager to always send a ditigal test pattern consisting of a grayscale ramp.


  Parameters:  none

  Returns:  SDHW_ERR_NONE if successful, SDHW_ERR_MISC otherwise.
----------------------------------------------------------------------------------------*/
DWORD SetImagerModeToGrayscaleBYD(void)
{
	return SDHW_ERR_NONE;
}

/*----------------------------------------------------------------------------------------
UnSetImagerModeToGrayscaleBYD

  Unsets the grayscale test mode of the imager (restores it to normal operation).


  Parameters:  none

  Returns:  SDHW_ERR_NONE if successful, SDHW_ERR_MISC otherwise.
----------------------------------------------------------------------------------------*/
DWORD UnSetImagerModeToGrayscaleBYD(void)
{
	return SDHW_ERR_NONE;
}
//******************************************End Test Flex***********************************

