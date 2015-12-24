#ifndef _BYD_HRDWR_H_
#define _BYD_HRDWR_H_

#if PLATFORM_CURRENT == imx25

	#include <stdlib.h>               //included for malloc definition
	#include "HardwareMacros.h"		//Provides Macros for setting registers
	#include "Hardware_Width_Defines.h"
	#include "imx25_I2C.h"
	#include "iMX25_IOMUX.h"
	#include "imx25_GPIO.h"
	#include "Imx25_csi.h"

	#include "interruptMacros.h"
	#include "interrupts.h"

	/* Bit field flags */
	#define   MOD_BIT(a)				  ((unsigned long)0x00000001 << (a))

	//set the default Mux Mode for CSI
	#define DATA_MUX_SETTING								((unsigned long)0x00000000)
	#define SET_CSI_PORT_MUX_MODE			\
											do{	\
												*((volatile unsigned long *)(IOMUXC_SW_MUX_CTL_PAD_CSI_D2)) = DATA_MUX_SETTING;	\
												*((volatile unsigned long *)(IOMUXC_SW_MUX_CTL_PAD_CSI_D3)) = DATA_MUX_SETTING;	\
												*((volatile unsigned long *)(IOMUXC_SW_MUX_CTL_PAD_CSI_D4)) = DATA_MUX_SETTING;	\
												*((volatile unsigned long *)(IOMUXC_SW_MUX_CTL_PAD_CSI_D5)) = DATA_MUX_SETTING;	\
												*((volatile unsigned long *)(IOMUXC_SW_MUX_CTL_PAD_CSI_D6)) = DATA_MUX_SETTING;	\
												*((volatile unsigned long *)(IOMUXC_SW_MUX_CTL_PAD_CSI_D7)) = DATA_MUX_SETTING;	\
												*((volatile unsigned long *)(IOMUXC_SW_MUX_CTL_PAD_CSI_D8)) = DATA_MUX_SETTING;	\
												*((volatile unsigned long *)(IOMUXC_SW_MUX_CTL_PAD_CSI_D9)) = DATA_MUX_SETTING;	\
												*((volatile unsigned long *)(IOMUXC_SW_MUX_CTL_PAD_CSI_MCLK)) = DATA_MUX_SETTING;	\
												*((volatile unsigned long *)(IOMUXC_SW_MUX_CTL_PAD_CSI_VSYNC)) = DATA_MUX_SETTING;	\
												*((volatile unsigned long *)(IOMUXC_SW_MUX_CTL_PAD_CSI_HSYNC)) = DATA_MUX_SETTING;	\
												*((volatile unsigned long *)(IOMUXC_SW_MUX_CTL_PAD_CSI_PIXCLK)) = DATA_MUX_SETTING;	\
											}while(0)

	//CSI Control Register address defines *********************************************************

	#define CSI_CONTROL_REG_ONE									 (*((volatile unsigned long *)(CSI_CSICR1)))

	#define CSI_CONTROL_REG_THREE									 (*((volatile unsigned long *)(CSI_CSICR3)))

	#define CSI_STATUS_REG											 (*((volatile unsigned long *)(CSI_CSISR)))

	#define CSI_CONTROL_REG_TWO									 (*((volatile unsigned long *)(CSI_CSICR2)))

	#define GET_CSI_CONTROL_REG_THREE							 (*((volatile unsigned long *)(CSI_CSICR3)))

	#define SET_CSI_CONTROL_REG_THREE							 (*((volatile unsigned long *)(CSI_CSICR3)))

	//CsiRxFIFO, source address to rx fifo
	#define RX_FIFO_REGISTER_ADDRESS								((int)CSI_CSIRFIFO)

	//CsiStatFIFO, deref source address to STAT FIFO  for reading data
	#define GET_STATISTICS_FIFO_REGISTER_DATA					((int)(*((volatile unsigned long *)(CSI_CSISTATFIFO))))

	//CsiStatFIFO, deref source address to STAT FIFO DMA Transfer Size
	#define STATISTICS_FIFO_TRANSFER_SIZE_REG					(*((volatile unsigned long *)(CSI_CSIDMATS_STATFIFO)))

	//CsiRxCountRegister, deref source address to RX COUNT REGISTER Transfer Size
	#define RX_COUNT_TRANSFER_SIZE_REG							(*((volatile unsigned long *)(CSI_CSIRXCNT)))

	//CsiStatFIFO, deref source address to STAT FIFO DMA Start Address
	#define STATISTICS_FIFO_DMA_START_ADDR_REG				(*((volatile unsigned long *)(CSI_CSIDMASA_STATFIFO)))

	//CsiRxFIFO, deref source address to RX FIFO for reading data
	#define GET_RX_FIFO_REGISTER_DATA							((int)(*((volatile unsigned long *)(CSI_CSIRFIFO))))

	#define CSI_FRAME_BUFFER_ONE_REG								(*((volatile unsigned long *)(CSI_CSIDMASA_FB1)))

	#define CSI_FRAME_BUFFER_TWO_REG								(*((volatile unsigned long *)(CSI_CSIDMASA_FB2)))

	#define CSI_IMAGE_SIZE_REG										(*((volatile unsigned long *)(CSI_CSIIMAG_PARA)))

	#define CSI_IMAGE_STRIDE_REG									(*((volatile unsigned long *)(CSI_CSIFBUF_PARA)))

	//FRMCNT, get dma frame count
	#define GET_CSI_CONTROL_REG_THREE_FRAME_COUNT					(((*((volatile unsigned long *)(CSI_CSICR3))) & 0xffff0000)>>16)



	//CSI Control Register CR1 and CR3 Macros *********************************************************

	//SOF_INTEN
	#define IS_SOF_INTERUPT_ENABLED								 ((CSI_CONTROL_REG_ONE & ((unsigned long)MOD_BIT(16))) != FALSE)

	#define DISABLE_SOF_INTERUPT								 (CSI_CONTROL_REG_ONE &= ~((unsigned long)MOD_BIT(16)))

	#define ENABLE_SOF_INTERUPT									 (CSI_CONTROL_REG_ONE |= ((unsigned long)MOD_BIT(16)))

	//EOF_INTEN
	#define IS_EOF_INTERUPT_ENABLED								 ((CSI_CONTROL_REG_ONE & ((unsigned long)MOD_BIT(29))) != FALSE)

	#define DISABLE_EOF_INTERUPT								 (CSI_CONTROL_REG_ONE &= ~((unsigned long)MOD_BIT(29)))

	#define ENABLE_EOF_INTERUPT									 (CSI_CONTROL_REG_ONE |= ((unsigned long)MOD_BIT(29)))

	//RX_FIFO_FULL_INTEN, control for the rx fifo full interupt, not used if DMAing rx
	#define IS_RX_FIFO_FULL_INTERUPT_ENABLED					 ((CSI_CONTROL_REG_ONE & ((unsigned long)MOD_BIT(18))) != FALSE)

	#define DISABLE_RX_FIFO_FULL_INTERUPT						 (CSI_CONTROL_REG_ONE &= ~((unsigned long)MOD_BIT(18)))

	#define ENABLE_RX_FIFO_FULL_INTERUPT						 (CSI_CONTROL_REG_ONE |= ((unsigned long)MOD_BIT(18)))

	//NOP
	#define DISABLE_CSI

	//NOP
	#define ENABLE_CSI

	//MCLKDIV - set  MCLK to imager, it operates at 48 MHz, divide by 1 (240mhz usbpll source for perclk0, using / by 5 on perclk for 48 mhz) - DEFAULT - OVERLOAD ONE
	//#define SET_MCLK_DIV										(CSI_CONTROL_REG_ONE &= ~((unsigned long)(MOD_BIT(15)| MOD_BIT(14)| MOD_BIT(13)| MOD_BIT(12))))

	//HI2D 24 MHz
	//MCLKDIV - set  MCLK to imager, it operates at 48 MHz, divide by 1 (240mhz usbpll source for perclk0, using / by 5 on perclk for 48 mhz) - DEFAULT - OVERLOAD ONE
	#define SET_MCLK_DIV									 (CSI_CONTROL_REG_ONE = ((CSI_CONTROL_REG_ONE &(~((unsigned long)(MOD_BIT(15)| MOD_BIT(14)| MOD_BIT(13)| MOD_BIT(12))))) | MOD_BIT(12)))

	//MCLKDIV - set  MCLK to imager, it operates at 24 MHz, divide by 2 (240mhz usbpll source for perclk0, using / by 5 on perclk for 48 mhz) - OVERLOAD TWO
	//#define SET_MCLK_DIV										(CSI_CONTROL_REG_ONE = ((CSI_CONTROL_REG_ONE &(~((unsigned long)(MOD_BIT(15)| MOD_BIT(14)| MOD_BIT(13)| MOD_BIT(12))))) | MOD_BIT(12)))

	//MCLKDIV - set  MCLK to imager, it operates at 12 MHz, divide by 4 (240mhz usbpll source for perclk0, using / by 5 on perclk for 48 mhz) - OVERLOAD THREE
	//#define SET_MCLK_DIV										(CSI_CONTROL_REG_ONE = ( (CSI_CONTROL_REG_ONE &(~((unsigned long)(MOD_BIT(15)| MOD_BIT(14)| MOD_BIT(13)| MOD_BIT(12))))) | MOD_BIT(13) ) )

	//MCLKEN
	#define ENABLE_MCLK												(CSI_CONTROL_REG_ONE |= ((unsigned long)MOD_BIT(9)))

	#define DISABLE_MCLK											(CSI_CONTROL_REG_ONE &= ~((unsigned long)MOD_BIT(9)))

	//SOF_POLARITY_FALLING - SOF POLARITY OVERLOAD 1
	//Use this for BYD if you want SOF to trigger on EOF
	#define SET_SOF_POLARITY_FALLING								(CSI_CONTROL_REG_ONE &= ~((unsigned long)MOD_BIT(17)))

	//SOF_POLARITY_RISING - SOF POLARITY OVERLOAD 2
	//Use this for BYD if you want SOF to trigger on the actual SOF (end of vert. blank)
	#define SET_SOF_POLARITY_RISING									(CSI_CONTROL_REG_ONE |= ((unsigned long)MOD_BIT(17)))

	//STATFF_INTEN, control for the stat fifo full interupt, not used if DMAing stat
	#define IS_STAT_FIFO_FULL_INTERUPT_ENABLED						((CSI_CONTROL_REG_ONE & ((unsigned long)MOD_BIT(21))) != FALSE)

	#define DISABLE_STAT_FIFO_INT									(CSI_CONTROL_REG_ONE &= ~((unsigned long)MOD_BIT(21)))

	#define ENABLE_STAT_FIFO_INT									(CSI_CONTROL_REG_ONE |= ((unsigned long)MOD_BIT(21)))

	//CLR_STATFIFO, clear the STAT FIFO, then waits for next SOF to restart
	#define CLEAR_STAT_FIFO											(CSI_CONTROL_REG_ONE |= ((unsigned long)MOD_BIT(6)))

	//CLR_RXFIFO, clear the CSI RX FIFO, then restarts immediately
	#define CLEAR_CSI_FIFO											(CSI_CONTROL_REG_ONE |= ((unsigned long)MOD_BIT(5)))

	//FCC ENABLE
	//Enable syncronous fifo clearing, otherwise it must be done asyncronously (cleared manually in every vertical using CLR_xxxx_FIFO)
	#define ENABLE_SYNC_FCC											(CSI_CONTROL_REG_ONE |= ((unsigned long)MOD_BIT(8)))

	#define DISABLE_SYNC_FCC										(CSI_CONTROL_REG_ONE &= ~((unsigned long)MOD_BIT(8)))

	//GCLK_MODE
	#define ENABLE_GATED_CLK_MODE									(CSI_CONTROL_REG_ONE |= ((unsigned long)MOD_BIT(4)))

	//REDGE, rise is correct for mx25
	#define SELECT_VALID_PIXEL_CLK_EDGE_RISE					(CSI_CONTROL_REG_ONE |= ((unsigned long)MOD_BIT(1)))

	#define SELECT_VALID_PIXEL_CLK_EDGE_FALL					(CSI_CONTROL_REG_ONE &= ~((unsigned long)MOD_BIT(1)))

	//HSYNC_POL, hsync is active high
	#define SELECT_ACTIVE_HSYNC_POL								(CSI_CONTROL_REG_ONE |= ((unsigned long)MOD_BIT(11)))

	//PACK_DIR, lsb is correct for mx25
	#define SELECT_PACK_DIR_LSB									(CSI_CONTROL_REG_ONE &= ~((unsigned long)MOD_BIT(7)))

	#define SELECT_PACK_DIR_MSB									(CSI_CONTROL_REG_ONE |= ((unsigned long)MOD_BIT(7)))

	//PIXEL_BIT, 8 bits per pixel is correct for mx25
	#define SELECT_PIXEL_BIT_EIGHT								(CSI_CONTROL_REG_ONE &= ~((unsigned long)MOD_BIT(0)))

	#define SELECT_PIXEL_BIT_TEN								(CSI_CONTROL_REG_ONE |= ((unsigned long)MOD_BIT(0)))

	//TWO_EIGHT_BIT_SENSORS, one 8 bit sensor used is correct for mx25
	#define SELECT_TWO_EIGHT_BIT_SENSOR_ONE_USED				(CSI_CONTROL_REG_THREE &= ~((unsigned long)MOD_BIT(3)))

	#define SELECT_TWO_EIGHT_BIT_SENSOR_TWO_USED				(CSI_CONTROL_REG_THREE |= ((unsigned long)MOD_BIT(3)))

	//STAFF_LEVEL, set to 16 words(011), for stat fifo full level - DEFAULT - overload 1
	#define SET_STAT_FIFO_FULL_LEV								(CSI_CONTROL_REG_THREE = ((CSI_CONTROL_REG_THREE & ~((unsigned long)MOD_BIT(10))) | ((unsigned long)(MOD_BIT(9)|MOD_BIT(8)))))

	//STAFF_LEVEL, set to 8 words(001), for stat fifo full level - overload 2
	//#define SET_STAT_FIFO_FULL_LEV								(CSI_CONTROL_REG_THREE = ((CSI_CONTROL_REG_THREE & ~((unsigned long)(MOD_BIT(10)|MOD_BIT(9)))) | ((unsigned long)MOD_BIT(8))))

	//RXFF_LEVEL, set to 32 words(100) - overload 1
	//#define SET_RX_FIFO_FULL_LEV									(CSI_CONTROL_REG_THREE = ((CSI_CONTROL_REG_THREE & ~((unsigned long)(MOD_BIT(5)|MOD_BIT(4)))) | ((unsigned long)MOD_BIT(6))))

	//RXFF_LEVEL, set to 16 words(010) - overload 2
	//#define SET_RX_FIFO_FULL_LEV									(CSI_CONTROL_REG_THREE = ((CSI_CONTROL_REG_THREE & ~((unsigned long)(MOD_BIT(6)|MOD_BIT(4)))) | ((unsigned long)MOD_BIT(5))))

	//RXFF_LEVEL, set to 8 words(001) - DEFAULT - overload 3
	#define SET_RX_FIFO_FULL_LEV									(CSI_CONTROL_REG_THREE = ((CSI_CONTROL_REG_THREE & ~((unsigned long)(MOD_BIT(6)|MOD_BIT(5)))) | ((unsigned long)MOD_BIT(4))))

	//RXFF_LEVEL, set to 64 words(110) - overload 4
	//#define SET_RX_FIFO_FULL_LEV									(CSI_CONTROL_REG_THREE = ((CSI_CONTROL_REG_THREE & ~((unsigned long)(MOD_BIT(4)))) | ((unsigned long)(MOD_BIT(6)|MOD_BIT(5)))))

	//RXFF_LEVEL, set to 96 words(111) - overload 5
	//#define SET_RX_FIFO_FULL_LEV									(CSI_CONTROL_REG_THREE = (CSI_CONTROL_REG_THREE | ((unsigned long)(MOD_BIT(6)|MOD_BIT(5)|MOD_BIT(4)))))


	//Status register Macros *********************************************************

	//SOF_INT
	#define IS_SOF_INT_SET											((CSI_STATUS_REG & ((unsigned long)MOD_BIT(16))) !=FALSE)

	#define CLEAR_SOF_INT											(CSI_STATUS_REG = ((unsigned long)MOD_BIT(16)))

	//EOF_INT
	#define IS_EOF_INT_SET											((CSI_STATUS_REG & ((unsigned long)MOD_BIT(17))) !=FALSE)

	#define CLEAR_EOF_INT											(CSI_STATUS_REG = ((unsigned long)MOD_BIT(17)))

	//RX_FIFO_FULL_INT, status reg rx fifo full indicator
	#define IS_RX_FIFO_FULL_INT_SET								((CSI_STATUS_REG & ((unsigned long)MOD_BIT(18))) !=FALSE)

	//STATFF_INT_FULL_INT, status reg stat fifo full indicator
	#define IS_STAT_FIFO_FULL_INT_SET							((CSI_STATUS_REG & ((unsigned long)MOD_BIT(21))) !=FALSE)

	//RX_FIFO_OVERRUN_INT set
	#define IS_RX_FIFO_OVERRUN_INT_SET							((CSI_STATUS_REG & ((unsigned long)MOD_BIT(24))) !=FALSE)

	//STATFF_OVERRUN_INT set
	#define IS_STAT_FIFO_OVERRUN_INT_SET						((CSI_STATUS_REG & ((unsigned long)MOD_BIT(25))) !=FALSE)

	//RX_FIFO_OVERRUN_INT clear
	#define CLEAR_RX_FIFO_OVERRUN_INT							(CSI_STATUS_REG = ((unsigned long)MOD_BIT(24)))

	//STATFF_OVERRUN_INT clear
	#define CLEAR_STAT_FIFO_OVERRUN_INT							(CSI_STATUS_REG = ((unsigned long)MOD_BIT(25)))

	//HRESP_ERR_INT, indicates a dma error
	#define IS_HRESP_ERR_INT_SET								((CSI_STATUS_REG & ((unsigned long)MOD_BIT(7))) !=FALSE)

	#define CLEAR_HRESP_ERR_INT									(CSI_STATUS_REG = ((unsigned long)MOD_BIT(7)))

	//CLEAR ALL INTERRUPT STATUS, including clearing start of frame interrupt, rx fifo overrun inteRrupt status, and statfifo overrun inteRrupt status, plus new end of frame status(mx25 - bit 17)
	#define CLEAR_ALL_INTERUPT_STATUS							(CSI_STATUS_REG = ((unsigned long)(MOD_BIT(25)|MOD_BIT(24)|MOD_BIT(22)|MOD_BIT(20)|MOD_BIT(19)|MOD_BIT(17)|MOD_BIT(16)|MOD_BIT(13)|MOD_BIT(7)|MOD_BIT(1))))

	//Statistics in Control Register 2 Macros *********************************************************

	//DRM, controls the size of the statitics grid
	#define SET_DOUBLE_RESOLUTION_MODE							(CSI_CONTROL_REG_TWO |= ((unsigned long)MOD_BIT(26)))

	//SCE, enables stat fifo skip count
	#define ENABLE_SKIP_COUNT									(CSI_CONTROL_REG_TWO |= ((unsigned long)MOD_BIT(23)))

	//LVRM, live view resolution mode, 512x384
	#define SET_LIVE_VIEW_RES_MODE								(CSI_CONTROL_REG_TWO &= ~((unsigned long)(MOD_BIT(18)|MOD_BIT(17)|MOD_BIT(16))))

	//VSC, set vertical skip count
	#define SET_VERTICAL_SKIP_COUNT_TO(a)						(CSI_CONTROL_REG_TWO = ((CSI_CONTROL_REG_TWO & 0xffff00ff) | (((a) & 0x000000ff) << 8)))

	//HSC, set horizontal skip count
	#define SET_HORIZONTAL_SKIP_COUNT_TO(a)					(CSI_CONTROL_REG_TWO = ((CSI_CONTROL_REG_TWO & 0xffffff00) | ((a) & 0x000000ff)))


	//Control Register 3 Macros *********************************************************

	//HRESP_ERROR_EN, disable HRESP error interupt
	#define DISABLE_HRESP_ERROR_INT								(CSI_CONTROL_REG_THREE &= ~((unsigned long)MOD_BIT(7)))

	//HRESP_ERROR_EN, enable HRESP error interupt
	#define ENABLE_HRESP_ERROR_INT								(CSI_CONTROL_REG_THREE |= ((unsigned long)MOD_BIT(7)))

	//FRMCNT_RST, clear frame counter
	#define CLEAR_CSI_CONTROL_REG_THREE_FRAME_COUNT				(CSI_CONTROL_REG_THREE |= ((unsigned long)MOD_BIT(15)))


	//DMA Macros *********************************************************

	//DMA_REQ_EN_RFF, disable dma request from Rx Fifo
	#define DISABLE_RX_FIFO_DMA									(CSI_CONTROL_REG_THREE &= ~((unsigned long)MOD_BIT(12)))

	//DMA_REQ_EN_RFF, enable dma request from Rx Fifo
	#define ENABLE_RX_FIFO_DMA									(CSI_CONTROL_REG_THREE |= ((unsigned long)MOD_BIT(12)))

	//DMA_REQ_EN_RFF, check if dma from Rx Fifo is disabled
	#define IS_RX_FIFO_DMA_DISABLED								((CSI_CONTROL_REG_THREE & ((unsigned long)MOD_BIT(12))) == false)

	//DMA_REFLASH_RFF, clear and restart dma for Rx Fifo
	#define REFRESH_RX_FIFO_DMA									(CSI_CONTROL_REG_THREE |= ((unsigned long)MOD_BIT(14)))

	//DMA_REQ_EN_SFF, disable dma request from Stat Fifo
	#define DISABLE_STAT_FIFO_DMA								(CSI_CONTROL_REG_THREE &= ~((unsigned long)MOD_BIT(11)))

	//DMA_REQ_EN_SFF, enable dma request from Stat Fifo
	#define ENABLE_STAT_FIFO_DMA								(CSI_CONTROL_REG_THREE |= ((unsigned long)MOD_BIT(11)))

	//DMA_REQ_EN_SFF, check if dma from Stat Fifo is disabled
	#define IS_STAT_FIFO_DMA_DISABLED							((CSI_CONTROL_REG_THREE & ((unsigned long)MOD_BIT(11))) == false)

	//DMA_REFLASH_SFF, clear and restart dma for Stat Fifo
	#define REFRESH_STAT_FIFO_DMA								(CSI_CONTROL_REG_THREE |= ((unsigned long)MOD_BIT(13)))

	//set DMA destination address from my buffer address
	//NOP for mx25, since we have two buffers
	#define SET_DMA_DESTINATION_ADDRESS(a)

	//set DMA destination address for dma buffer one, from my buffer address
	#define SET_DMA_BUF_ONE_DESTINATION_ADDRESS(a)			(CSI_FRAME_BUFFER_ONE_REG=(a))

	//set DMA destination address for dma buffer two, from my buffer address
	#define SET_DMA_BUF_TWO_DESTINATION_ADDRESS(a)			(CSI_FRAME_BUFFER_TWO_REG=(a))

	//set DMA destination address for stat fifo, from a buffer address
	#define SET_DMA_STAT_FIFO_DESTINATION_ADDRESS(a)		(STATISTICS_FIFO_DMA_START_ADDR_REG=(a))

	//set DMA transfer size for stat fifo, to match fill size
	#define SET_DMA_STAT_FIFO_TRANSFER_SIZE(a)				(STATISTICS_FIFO_TRANSFER_SIZE_REG=(a))

	//set DMA EOF RX Count Register size for RX fifo, to match DMA word transfer size (number of pixels /4 to get number of words)
	#define SET_DMA_EOF_RX_COUNT_TRANSFER_SIZE(a)			(RX_COUNT_TRANSFER_SIZE_REG=(a))

	//SET DMA image width
	#define SET_DMA_IMAGE_WIDTH_TO(a)						(CSI_IMAGE_SIZE_REG = ((CSI_IMAGE_SIZE_REG & 0x0000ffff) | (((a) & 0x0000ffff) << 16)))

	//SET DMA image height
	#define SET_DMA_IMAGE_HEIGHT_TO(a)						(CSI_IMAGE_SIZE_REG = ((CSI_IMAGE_SIZE_REG & 0xffff0000) | ((a) & 0x0000ffff)))

	//SET DMA image frame buffer stride
	#define SET_DMA_IMAGE_FRAME_BUFFER_STRIDE_TO(a)		(CSI_IMAGE_STRIDE_REG = ((CSI_IMAGE_STRIDE_REG & 0xffff0000) | ((a) & 0x0000ffff)))

	//SET dma burst type of the rx fifo to INCR8 (00) - DEFAULT - overload 1
	#define SET_RX_DMA_BURST_TYPE									(CSI_CONTROL_REG_TWO &= ~((unsigned long)(MOD_BIT(31) | MOD_BIT(30))))

	//SET dma burst type of the rx fifo to INCR16 (11) - overload 2
	//#define SET_RX_DMA_BURST_TYPE									(CSI_CONTROL_REG_TWO |= ((unsigned long)(MOD_BIT(31) | MOD_BIT(30))))

	//SET dma burst type of the stat fifo to INCR8 (00) - DEFAULT - overload 1
	#define SET_STAT_DMA_BURST_TYPE									(CSI_CONTROL_REG_TWO &= ~((unsigned long)(MOD_BIT(29) | MOD_BIT(28))))

	//SET dma burst type of the stat fifo to INCR16 (11) - overload 2
	//#define SET_STAT_DMA_BURST_TYPE									(CSI_CONTROL_REG_TWO |= ((unsigned long)(MOD_BIT(29) | MOD_BIT(28))))

	//STAT_FIFO_OVERRUN_INTERUPT, enable interupt for statistics fifo overrun interupt
	#define ENABLE_STAT_FIFO_OVERRUN_INT							(CSI_CONTROL_REG_ONE |= ((unsigned long)MOD_BIT(25)))

	//STAT_FIFO_OVERRUN_INTERUPT, disable interupt for statistics fifo overrun interupt
	#define DISABLE_STAT_FIFO_OVERRUN_INT							(CSI_CONTROL_REG_ONE &= ~((unsigned long)MOD_BIT(25)))

	//RX_FIFO_OVERRUN_INTERUPT, enable interupt for rx fifo overrun interupt
	#define ENABLE_RX_FIFO_OVERRUN_INT								(CSI_CONTROL_REG_ONE |= ((unsigned long)MOD_BIT(24)))

	//RX_FIFO_OVERRUN_INTERUPT, disable interupt for rx fifo overrun interupt
	#define DISABLE_RX_FIFO_OVERRUN_INT								(CSI_CONTROL_REG_ONE &= ~((unsigned long)MOD_BIT(24)))

	//FB2_DMA_DONE_INTEN, disable dma request for dma frame two done
	#define DISABLE_BUFFER_TWO_DMA_DONE								(CSI_CONTROL_REG_ONE &= ~((unsigned long)MOD_BIT(20)))

	//FB2_DMA_DONE_INTEN, enable dma request for dma frame two done
	#define ENABLE_BUFFER_TWO_DMA_DONE								(CSI_CONTROL_REG_ONE |= ((unsigned long)MOD_BIT(20)))

	//FB1_DMA_DONE_INTEN, disable dma request for dma frame one done
	#define DISABLE_BUFFER_ONE_DMA_DONE								(CSI_CONTROL_REG_ONE &= ~((unsigned long)MOD_BIT(19)))

	//FB1_DMA_DONE_INTEN, enable dma request for dma frame one done
	#define ENABLE_BUFFER_ONE_DMA_DONE								(CSI_CONTROL_REG_ONE |= ((unsigned long)MOD_BIT(19)))

	//DMA_TSF_DONE_FB1 and FB2, is dma buffer one or two done
	#define IS_BUFFER_DMA_DONE_SET									((CSI_STATUS_REG & ((unsigned long)(MOD_BIT(19)|MOD_BIT(20)))) !=FALSE)

	//DMA_TSF_DONE_FB2, is dma buffer two done
	#define IS_BUFFER_TWO_DMA_DONE_SET								((CSI_STATUS_REG & ((unsigned long)MOD_BIT(20))) !=FALSE)

	//DMA_TSF_DONE_FB2, clear dma buffer two done
	#define CLEAR_BUFFER_TWO_DMA_DONE								(CSI_STATUS_REG = ((unsigned long)MOD_BIT(20)))

	//DMA_TSF_DONE_FB1, is dma buffer one done
	#define IS_BUFFER_ONE_DMA_DONE_SET								((CSI_STATUS_REG & ((unsigned long)MOD_BIT(19))) !=FALSE)

	//DMA_TSF_DONE_FB1, clear dma buffer one done
	#define CLEAR_BUFFER_ONE_DMA_DONE								(CSI_STATUS_REG = ((unsigned long)MOD_BIT(19)))

	//DMA_TSF_DONE_STAT_FIFO, is dma of stat fifo done
	#define IS_STAT_FIFO_DMA_DONE_SET								((CSI_STATUS_REG & ((unsigned long)MOD_BIT(22))) !=FALSE)

	//DMA_TSF_DONE_STAT_FIFO, clear dma of stat fifo done
	#define CLEAR_STAT_FIFO_DMA_DONE									(CSI_STATUS_REG = ((unsigned long)MOD_BIT(22)))

	//STAT_FIFO_DMA_DONE_INTEN disable, disable dma request for dma stat fifo done interupt
	#define DISABLE_STAT_FIFO_DMA_DONE								(CSI_CONTROL_REG_ONE &= ~((unsigned long)MOD_BIT(22)))

	//STAT_FIFO_DMA_DONE_INTEN enable, enable dma request for dma stat fifo done interupt
	#define ENABLE_STAT_FIFO_DMA_DONE								(CSI_CONTROL_REG_ONE |= ((unsigned long)MOD_BIT(22)))

	#define IS_ENABLED_STAT_FIFO_DMA_DONE								(CSI_CONTROL_REG_ONE |= ((unsigned long)MOD_BIT(22)))


	//HAL Macros *********************************************************

	//BYD power, set pad to cmos and no 100k pull down
	#define CONFIG_BYD_POWER_PAD_TO_CMOS_W_PD					*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_KPP_ROW2)) = (unsigned long)( MOD_BIT(13) )

	//BYD power, set pad to gpio
	#define CONFIG_BYD_POWER_PAD_TO_GPIO						*((volatile unsigned long *)(IOMUXC_SW_MUX_CTL_PAD_KPP_ROW2)) = 0x00000005

	//set BYD power data register to 1 to turn on power
	#define SET_BYD_POWER_DR_REG_HIGH							(*((volatile unsigned long *)(GPIO2_DR0))) |= ((unsigned long)(MOD_BIT(31)))

	//set BYD power data register to 0 to turn off power
	#define SET_BYD_POWER_DR_REG_LOW							(*((volatile unsigned long *)(GPIO2_DR0))) &= ~((unsigned long)(MOD_BIT(31)))

	//set BYD power direction register to output
	#define SET_BYD_POWER_DATA_DIR_OUTPUT						(*((volatile unsigned long *)(GPIO2_GDIR0))) |= ((unsigned long)(MOD_BIT(31)))


	//BYD INTERFACE, set all pads below to 1.8 VOLTAGE LEVELS
	#define CONFIG_BYD_CSI_D2_PAD				*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_CSI_D2)) =  (unsigned long)( MOD_BIT(13) )

	#define CONFIG_BYD_CSI_D3_PAD				*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_CSI_D3)) =  (unsigned long)( MOD_BIT(13) )

	#define CONFIG_BYD_CSI_D4_PAD				*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_CSI_D4)) =  (unsigned long)( MOD_BIT(13) )

	#define CONFIG_BYD_CSI_D5_PAD				*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_CSI_D5)) =  (unsigned long)( MOD_BIT(13) )

	#define CONFIG_BYD_CSI_D6_PAD				*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_CSI_D6)) =  (unsigned long)( MOD_BIT(13) )

	#define CONFIG_BYD_CSI_D7_PAD				*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_CSI_D7)) =  (unsigned long)( MOD_BIT(13) )

	#define CONFIG_BYD_CSI_D8_PAD				*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_CSI_D8)) =  (unsigned long)( MOD_BIT(13) )

	#define CONFIG_BYD_CSI_D9_PAD				*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_CSI_D9)) =  (unsigned long)( MOD_BIT(13) )


	//OVERLOAD 1 - MCLOCK pad is set up to 1.8 v (bit 13), drive strength default (bit 2 - 1), slew is default (bit 0), rise time 4.98ns
	//#define CONFIG_BYD_CSI_MCLK_PAD			*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_CSI_MCLK)) =  (unsigned long)( MOD_BIT(13) )

	//Standard Product Configuration setting
	//OVERLOAD 2 - MCLOCK pad is set up to 1.8 v (bit 13), drive strength default (bit 2 - 1), slew is high (bit 0), rise time 2.02ns
	#define CONFIG_BYD_CSI_MCLK_PAD_old			*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_CSI_MCLK)) =  (unsigned long)( MOD_BIT(13) | 0x00000001 )

	//OVERLOAD 3 - MCLOCK pad is set up to 1.8 v (bit 13), drive strength max (bit 2 - 1), slew is low (bit 0), rise time 1.76ns
	//#define CONFIG_BYD_CSI_MCLK_PAD			*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_CSI_MCLK)) =  (unsigned long)( MOD_BIT(13) | MOD_BIT(2) )

	//OVERLOAD 4 - MCLOCK pad is set up to 1.8 v (bit 13), drive strength high (bit 2 - 1), slew is high (bit 0), rise time 1.35ns
	//#define CONFIG_BYD_CSI_MCLK_PAD			*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_CSI_MCLK)) =  (unsigned long)( MOD_BIT(13) | MOD_BIT(1) | 0x00000001 )

	//OVERLOAD 5 - MCLOCK pad is set up to 1.8 v (bit 13), drive strength max (bit 2 - 1), slew is high (bit 0), rise time 1.26ns
	//#define CONFIG_BYD_CSI_MCLK_PAD			*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_CSI_MCLK)) =  (unsigned long)( MOD_BIT(13) | MOD_BIT(2) | 0x00000001 )
	//for HI2D
	#define CONFIG_BYD_CSI_MCLK_PAD			*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_CSI_MCLK)) =  (unsigned long)( MOD_BIT(7) | 0x00000001 )


	#define CONFIG_BYD_CSI_VSYNC_PAD			*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_CSI_VSYNC)) =  (unsigned long)( MOD_BIT(13) )

	#define CONFIG_BYD_CSI_HSYNC_PAD			*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_CSI_HSYNC)) =  (unsigned long)( MOD_BIT(13) )

	#define CONFIG_BYD_CSI_PIXCLK_PAD			*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_CSI_PIXCLK)) =  (unsigned long)( MOD_BIT(13) )

//#if defined(OEM_ENGINE)
	#define	SET_ILL_CON_HIGH			       HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT15_LSH,GPIO_BIT15_WID,0x1)
	
	#define	SET_ILL_CON_LOW				HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT15_LSH,GPIO_BIT15_WID,0x0)
	
	#define	SET_AIM_CON_HIGH			       HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT12_LSH,GPIO_BIT12_WID,0x1)
	
	#define	SET_AIM_CON_LOW				HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT12_LSH,GPIO_BIT12_WID,0x0)
	
	//#define	ENG_PSOC_XRES_HIGH			HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT0_LSH,GPIO_BIT0_WID,0x1)
	#define	ENG_PSOC_XRES_HIGH			HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT0_LSH,GPIO_BIT0_WID,0x0)//for HI2D

//*****************************************************************************************************************************
//*****************************************************************************************************************************
#else
	#error VALID PLATFORM NOT DEFINED
#endif //endof PLATFORM Check
//*****************************************************************************************************************************
//*****************************************************************************************************************************



//*****************************************************************************************************************************
//*****************************************************************************************************************************
//lmc  Stuff for debug ********************************************************************************************************

//Define below if you want to allow the standard debugging output			//lmc
//#define DEBUG_ON					1

//#define DMA_RX_FIFO_CAPTURE_LIMIT 1		//lmc   debug   define if you want to limit the number of frames captured

//#define THREAD_PROGRESS_INDICATOR	1    //lmc   debug   define if you want to output debug for line and vert progress

//#define FIFO_OVERRUN_TEST 1			//lmc   debug   define if you want to do a FIFO over run test

//#define THREAD_TIME_TEST 1			//lmc   debug   define if you want to do a test of interupt and thread event time

//#define THREAD_TIME_TEST_VERT 1			//lmc   debug   define if you want to do a test of interupt and thread event time in Vert int

//#define THREAD_TIME_TEST_LINE 1			//lmc   debug   define if you want to do a test of interupt and thread event time in Line int

//#define ENABLE_DMA_COMPLETE_INTERUPT_DEBUG 1         //lmc   debug   define if you want to run the dma complete interupt debug output

//#define ENABLE_FUNCTION_CALL_DEBUG 1         //lmc   debug   define if you want to run the function call order debug

//#define ENABLE_I2C_READ_DEBUG 1         //lmc   debug   define if you want to run the I2C read debug

//#define ENABLE_I2C_WRITE_DEBUG 1         //lmc   debug   define if you want to run the I2C write debug

#define SCAN_DRIVER_DEBUG_TEST	0					//lmc   debug   define to one to output mem location of dma buffer

//lmc  end stuff for debug ****************************************************************************************************
//*****************************************************************************************************************************
//*****************************************************************************************************************************



/*==================================================================================================
 * Configuration tables for the BYD imager
 *==================================================================================================*/

#ifdef BYD_SENSOR


#define DMA_STAT_TRANSFER_SIZE		((unsigned long)(16))
#define DMA_STAT_TRANSFER_LOCATION	((volatile unsigned long*)(0x78000000)) 							//dma stat fifo to internal ram



#define BYD_OUTPUT_PROCESS_RAW			0

#define NATIVE_IMAGE_HEIGHT				480
#define NATIVE_IMAGE_WIDTH				640

#define BYD_FRAME_RATE				30

#define BYD_GAIN_FIXED				0x18
#define BYD_INT_FIXED				0x30

#define BYD_GAIN_MAX				0x3F	//0x30
#define BYD_GAIN_MIN				0x01


#define BYD_INT_MAX					0x80	//0x4C
#define BYD_INT_MIN					0x06


#define	BYD_REG_GAIN				0x87
#define	BYD_REG_TINT_H				0x8C
#define	BYD_REG_TINT_L				0x8D

#define BYD_IIC_READ_ADDRESS 		0xDD		// base I2C address
#define BYD_IIC_WRITE_ADDRESS 		0xDC		// base I2C wrtie address
#define BYD_IIC_ADDRESS 			0xDC

struct BYDRegisterEntry_t
{
	unsigned char nRegister;
	unsigned char nValue;
} ;



static BYDRegisterEntry_t BYDRegEntriesForHHPExposureControl_3703[] = 
{	
/*
	
	{0x15,0x2, },
	{0x9,0x0,  },
	{0x1e,0x0, },
	{0x13,0x0, },
	{0x1,0x12, },
	{0x2,0x22, },
	{0x8c,0x2, },
	{0x8d,0x64,},
	{0x87,0x18,},
	{0x13,0x7, },
	{0x5,0x1f, },
	{0x6,0x60, },
	{0x14,0x1f,},
	{0x6,0xe0, },
	{0x11,0x80,},
	{0x2b,0x20,},
	{0x92,0x40,},
	{0x9d,0x93,},
	{0xeb,0x30,},
	{0xbb,0x20,},
	{0xf5,0x21,},
	{0xe1,0x3c,},
	{0x16,0x3, },
	{0x2f,0x66,},
	{0x35,0x80,},
	{0x65,0x83,},
	{0x66,0x80,},
	{0x36,0x5, },
	{0x37,0xf6,},
	{0x38,0x46,},
	{0x9b,0xff,},
	{0x9c,0x46,},
	{0xbc,0x11,},
	{0xbd,0x10,},
	{0xbe,0x46,},
	{0x82,0x14,},
	{0x83,0x23,},
	{0x9a,0x23,},
	{0x84,0x1a,},
	{0x85,0x20,},
	{0x89,0x3, },
	{0x8a,0x64,},
	{0x86,0x28,},
	{0x96,0xa6,},
	{0x97,0xc, },
	{0x98,0x18,},
	{0x70,0x6f,},
	{0x72,0x6f,},
	{0x73,0x2f,},
	{0x74,0x27,},
	{0x7a,0x86,},
	{0x7b,0x28,},
	{0x24,0x85,},
	{0x25,0x75,},
	{0x94,0xa, },
	{0x1f,0x20,},
	{0x22,0x20,},
	{0x26,0x20,},
	{0x56,0x40,},
	{0x61,0xd3,},
	{0x79,0x48,},
	{0x3b,0x60,},
	{0x3c,0x20,},
	{0x39,0x80,},
	{0x3f,0xb0,},
	{0x40,0x9b,},
	{0x41,0x88,},
	{0x42,0x6e,},
	{0x43,0x59,},
	{0x44,0x4d,},
	{0x45,0x45,},
	{0x46,0x3e,},
	{0x47,0x39,},
	{0x48,0x35,},
	{0x49,0x31,},
	{0x4b,0x2e,},
	{0x4c,0x2b,},
	{0x4e,0x26,},
	{0x4f,0x23,},
	{0x50,0x21,},
	{0x51,0x5, },
	{0x52,0x18,},
	{0x53,0x13,},
	{0x54,0x15,},
	{0x57,0x9e,},
	{0x58,0x89,},
	{0x59,0x5f,},
	{0x5a,0x89,},
	{0x5b,0x2a,},
	{0x5d,0x95,},
	{0x5c,0xe, },
	{0x60,0x20,},
	{0x6a,0x81,},
	{0x23,0x66,},
	{0xa0,0x3, },
	{0x6,0xe0, },
	{0xa1,0x51,},
	{0xa2,0xe, },
	{0xa3,0x26,},
	{0xa4,0xd, },
	{0xa5,0x23,},
	{0xa6,0x3, },
	{0xa7,0x80,},
	{0xa8,0x7e,},
	{0xa9,0x26,},
	{0xaa,0x29,},
	{0xab,0x26,},
	{0xac,0x3c,},
	{0xad,0xf0,},
	{0xc8,0x15,},
	{0xc9,0x20,},
	{0xca,0x14,},
	{0xcb,0x1f,},
	{0xaf,0x0, },
	{0xc5,0x16,},
	{0xc6,0x0, },
	{0xc7,0x20,},
	{0xae,0x83,},
	{0xcd,0x90,},
	{0xee,0x4c,},
	{0x80,0x55,},
	{0xb0,0xd0,},
	{0xb1,0xc0,},
	{0xb2,0xa0,},
	{0xb3,0x8a,},
	{0x8e,0x7, },
	{0x8f,0x79,},
	{0x20,0x40,},
	{0x9,0x44, },
	*/

	
//	{0x09,0x01},  	//0x09[4]=1,Enable Soft Sleep;  0x09[4]=0,Disable Soft Sleep;  
	{0x09,0x01},
#if BYD_OUTPUT_PROCESS_RAW
	{0x12,0x05},
	{0x0C,0x80},
#else
	{0x12,0x00}, 	//Output DATA MODE; 0x12[1:0]=00,YUV;  0x12[1:0]=01,Raw Data;  0x12[1:0]=10,RGB;  0x12[1:0]=11,Processed Raw Data;
#endif
	{0x15,0x02},	//Vsync active high

//	{0x3a,0x02},
	{0x3a,0x00},
	{0x17,0x00},
	{0x18,0xa0},
	{0x19,0x00},
	{0x1a,0x78},
	{0x03,0x00},	//image size 640X480

	{0x0b,0x00},	//skip the first 0 frames

	{0xE3,0x10},	//dummy lines after active lines
	{0x92,0x20},  	//dummy line before active lines

	{0x1e,0x30},  	//flip

	{0xf1,0x6A},  	//disable color correction ,disable saturation
	{0x6a,0x00},  	//green gain
	{0x23,0x00},  	//green gain
	{0x13,0x00},  	//disable AWB(0x13[1]=0) and AE(0x13[2]=0&0x13[2]=0).
	{0x01,0x00},  
	{0x02,0x00}, 
	{0x8c,((BYD_INT_FIXED >> 8) & 0xFF)},  	//manual exposure time
	{0x8d,(BYD_INT_FIXED & 0xFF)},  		//manual exposure time({0x8c,0x8d}) when 0x13[2]=0.
	{0x87,BYD_GAIN_FIXED},  				//manual exposure gain (0x87) when 0x13[2]=0.

	{0x11,0x80},
	{0x2b,0x00},  	//0 dummy pixel
	
	{0x80,0x55},  	//0x80[0]=1, 50Hz banding;  0x80[0]=0, 60Hz banding;
	{0x9d,0x99},  	//50hz banding
	{0x9e,0x7f},  	//60hz banding

	{0xeb,0x30},  	//
	{0xbb,0x20},
	{0xf5,0x21},
	{0xe1,0x3c},
	{0x16,0x03},
	{0x2f,0xf6}, 
	{0x1F,0x20},
	{0x22,0x20},
	{0x26,0x20},
	{0x61,0xd3},
	{0x79,0x48},  
	{0xa0,0x03},
	{0x06,0xe0}, 	//analog register,the value are recommended.

	{0x33,0x20},  	//
	{0x34,0x08},
	{0x35,0x56},
	{0x65,0x58},
	{0x66,0x52},
	{0x36,0x05},
	{0x37,0xf6},
	{0x38,0x46},
	{0x9b,0xf6},
	{0x9c,0x46},
	{0xbc,0x01},
	{0xbd,0xf6},
	{0xbe,0x46}, 	//lens shading correction

	{0x70,0x64}, 	//Disable low pass filter, Bit[0]=0; turn off bad pixel correction, Bit[1] = 0.
	{0x72,0x2f},
	{0x73,0x2f},
	{0x74,0x29},
	{0x77,0x90},
	{0x7a,0x4e},
	{0x7b,0x30}, 	//denoise

	{0x82,BYD_GAIN_MIN}, 				//global gain minimum 0x01
	{0x86,BYD_GAIN_MAX},				//global gain maximum 0x3F
	
	{0x8b,BYD_INT_MIN}, 				//global int minimum 0x06
	{0x8e,((BYD_INT_MAX >> 8) & 0xFF)},	//global int maximum 
	{0x8f,(BYD_INT_MAX & 0xFF)},		//global int maximum 0x4C
	
	{0x83,0x23},
	{0x9a,0x23},
	{0x84,0x1a},
	{0x85,0x20},
	{0x89,0x02},
	{0x8a,0x64},

	{0x96,0xa6},
	{0x97,0x0c},
	{0x98,0x18},
	{0x80,0x54},
	{0x24,0x88},
	{0x25,0x78}, 
	{0x94,0x0a}, 
	{0x8e,0x01},
	{0x8f,0xFE},  	//{0x8E,0x8F} intergration time maximum 0x1FE (510*784*2/(24*10^6) = 33.3ms) 
 
	{0x69,0x00},

	{0x3b,0x60}, 	//
	{0x3c,0x28},
	{0x39,0x80},
	{0x3f,0xc0},
	{0x40,0x60},
	{0x41,0x60},
	{0x42,0x66},
	{0x43,0x57},
	{0x44,0x4c},
	{0x45,0x43},
	{0x46,0x3c},
	{0x47,0x37},
	{0x48,0x33},
	{0x49,0x2f},
	{0x4b,0x2c},
	{0x4c,0x29},
	{0x4e,0x25},
	{0x4f,0x22},
	{0x50,0x20},  	//gamma 

	{0x56,0x40}  	//Contrast

};

#else

#error "INVALID SENSOR"

#endif	//end #ifdef BYD_SENSOR

#endif	//end #ifndef _BYD_HRDWR_H_

