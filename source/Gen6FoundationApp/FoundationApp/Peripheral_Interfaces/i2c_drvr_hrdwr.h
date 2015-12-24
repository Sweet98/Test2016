/* ==============================================================================
	Matrix I2C Driver Functions Header File
   ==============================================================================

   $RCSfile: Gen6FoundationApp/FoundationApp/Peripheral_Interfaces/i2c_drvr_hrdwr.h $
   $Revision: 1.4 $
   $Date: 2009/05/01 17:09:36EDT $

   These macros provide hardware definitions for the I2C bus functions.

   ==============================================================================

   ============================================================================== */


/////////////////////////////////////////////////////////////////////////////

#ifndef I2C_DRVR_HRDWR_H
#define I2C_DRVR_HRDWR_H


//*******************************************************************************************************************
//PLATFORM SPECIFIC INCLUDES AND MACROS
//*******************************************************************************************************************
//#define imx27	7
//#define imx25	5
//#define mxl		1
//#define PLATFORM_CURRENT	mxl



/***************************************************************************************************************************/
/***************************************************************************************************************************/

#if PLATFORM_CURRENT == imx25

	#include "HardwareMacros.h"		//Provides Macros for setting registers
	#include "Hardware_Width_Defines.h"
	#include "imx25_I2C.h"
	#include "iMX25_IOMUX.h"
	#include "imx25_GPIO.h"

	/* Bit field flags */
	#define   MOD_BIT(a)				  ((unsigned long)0x00000001 << (a))


	//Macros to change between I2C channels
	#if 0                     					//if you want to use I2C ONE (daugher board)

	#define IOMUXC_SW_MUX_CTL_PAD_I2C_CLK			IOMUXC_SW_MUX_CTL_PAD_I2C1_CLK

	#define IOMUXC_SW_MUX_CTL_PAD_I2C_DAT			IOMUXC_SW_MUX_CTL_PAD_I2C1_DAT

	#define IOMUXC_SW_PAD_CTL_PAD_I2C_CLK			IOMUXC_SW_PAD_CTL_PAD_I2C1_CLK

	#define IOMUXC_SW_PAD_CTL_PAD_I2C_DAT			IOMUXC_SW_PAD_CTL_PAD_I2C1_DAT

	#define I2C_MUX_SETTING								((unsigned long)0x00000000)

	#define GPIO_MUX_SETTING							((unsigned long)0x00000005)

	#define CLK_BIT										12

	#define DAT_BIT										13

	//control register
	#define I2C_I2CR										I2C1_I2CR

	//frequency divider
	#define I2C_IFDR										I2C1_IFDR

	//address register
	#define I2C_IADR										I2C1_IADR

	//status
	#define I2C_I2SR										I2C1_I2SR

	//data i/o
	#define I2C_I2DR										I2C1_I2DR

	//select port daisy chain for i2c1 clock
	#define SET_I2C_CLK_DAISY							SET_I2C1_CLK_DAISY
	//NOP
	#define SET_I2C1_CLK_DAISY

	//select port daisy chain for i2c1 data
	#define SET_I2C_DAT_DAISY							SET_I2C1_DAT_DAISY
	//NOP
	#define SET_I2C1_DAT_DAISY

	//Select interupt vector
	#define VEC_I2C_INT 									VEC_I2C1_INT


	#else												//by default use I2C THREE (cordless board)


	#define IOMUXC_SW_MUX_CTL_PAD_I2C_CLK			IOMUXC_SW_MUX_CTL_PAD_GPIO_A

	#define IOMUXC_SW_MUX_CTL_PAD_I2C_DAT			IOMUXC_SW_MUX_CTL_PAD_GPIO_B

	#define IOMUXC_SW_PAD_CTL_PAD_I2C_CLK			IOMUXC_SW_PAD_CTL_PAD_GPIO_A

	#define IOMUXC_SW_PAD_CTL_PAD_I2C_DAT			IOMUXC_SW_PAD_CTL_PAD_GPIO_B

	#define I2C_MUX_SETTING								((unsigned long)0x00000004)

	#define GPIO_MUX_SETTING							((unsigned long)0x00000000)

	#define CLK_BIT										0

	#define DAT_BIT										1

	//control register
	#define I2C_I2CR										I2C3_I2CR

	//frequency divider
	#define I2C_IFDR										I2C3_IFDR

	//address register
	#define I2C_IADR										I2C3_IADR

	//status
	#define I2C_I2SR										I2C3_I2SR

	//data i/o
	#define I2C_I2DR										I2C3_I2DR

	//select port daisy chain for i2c3 clock
	#define SET_I2C_CLK_DAISY							SET_I2C3_CLK_DAISY

	#define SET_I2C3_CLK_DAISY							*((volatile unsigned long *)(IOMUXC_I2C3_IPP_SCL_IN_SELECT_INPUT)) = ((unsigned long)0x00000001)

	//select port daisy chain for i2c3 data
	#define SET_I2C_DAT_DAISY							SET_I2C3_DAT_DAISY

	#define SET_I2C3_DAT_DAISY							*((volatile unsigned long *)(IOMUXC_I2C3_IPP_SDA_IN_SELECT_INPUT)) = ((unsigned long)0x00000002)

	//Select interupt vector
	#define VEC_I2C_INT 									VEC_I2C3_INT


	#endif		//End the I2C port selection


	//set the default Mux Mode
	#define SET_CLK_DATA_PORT_MODE_I2C	\
											do{	\
											*((volatile unsigned long *)(IOMUXC_SW_MUX_CTL_PAD_I2C_CLK)) = I2C_MUX_SETTING;	\
	   									*((volatile unsigned long *)(IOMUXC_SW_MUX_CTL_PAD_I2C_DAT)) = I2C_MUX_SETTING;	\
											}while(0)
	//set clock open drain with 47k pull up
	#define ENABLE_CLOCK_RESISTOR		*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_I2C_CLK)) = (unsigned long)( MOD_BIT(13) | MOD_BIT(7) | MOD_BIT(6) | MOD_BIT(4) | MOD_BIT(3) )
	//set data open drain with 47k pull up
	#define ENABLE_DATA_RESISTOR 		*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_I2C_DAT)) = (unsigned long)( MOD_BIT(13) | MOD_BIT(7) | MOD_BIT(6) | MOD_BIT(4) | MOD_BIT(3) )
	//set All open drain with 47k pull up
	#define ENABLE_ALL_OUTPUT_RESISTORS	\
											do{	\
											*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_I2C_CLK)) = (unsigned long)( MOD_BIT(13) | MOD_BIT(7) | MOD_BIT(6) | MOD_BIT(4) | MOD_BIT(3) );		\
	 										*((volatile unsigned long *)(IOMUXC_SW_PAD_CTL_PAD_I2C_DAT)) = (unsigned long)( MOD_BIT(13) | MOD_BIT(7) | MOD_BIT(6) | MOD_BIT(4) | MOD_BIT(3) );		\
											}while(0)
	//set clock direction to input
	#define SET_CLOCK_DIR_INPUT			(*((volatile unsigned long *)(GPIO1_GDIR0))) &= ~((unsigned long)(MOD_BIT(CLK_BIT)))
	//set data direction to input
	#define SET_DATA_DIR_INPUT				(*((volatile unsigned long *)(GPIO1_GDIR0))) &= ~((unsigned long)(MOD_BIT(DAT_BIT)))
	//set clock and data direction to input
	#define SET_CLK_DATA_DIR_INPUT		(*((volatile unsigned long *)(GPIO1_GDIR0))) &= ~((unsigned long)(MOD_BIT(CLK_BIT) | MOD_BIT(DAT_BIT)))
	//Set_Clock_Data_Reg_LOW
	#define SET_CLOCK_DR_REG_LOW			(*((volatile unsigned long *)(GPIO1_DR0))) &= ~((unsigned long)(MOD_BIT(CLK_BIT)))
	//Set_Clock_Data_Reg_HIGH
	#define SET_CLOCK_DR_REG_HIGH			(*((volatile unsigned long *)(GPIO1_DR0))) |= ((unsigned long)(MOD_BIT(CLK_BIT)))
	//Select_Clock_Output_Source
	//NOTE:  a NOP for the mx25
	#define SELECT_CLK_GPIO_OUTPUT_SOURCE
	//Set_Clock_and_Data_Port_for_GPIO
	#define SET_CLK_DATA_PORT_MODE_GPIO		\
											do{		\
											*((volatile unsigned long *)(IOMUXC_SW_MUX_CTL_PAD_I2C_CLK)) = GPIO_MUX_SETTING;	\
	   									*((volatile unsigned long *)(IOMUXC_SW_MUX_CTL_PAD_I2C_DAT)) = GPIO_MUX_SETTING;	\
											}while(0)
	//Is_Clock_Not_High
	#define IS_CLK_NOT_HIGH					(( (*((volatile unsigned long *)(GPIO1_PSR0))) & ((unsigned long)(MOD_BIT(CLK_BIT))) )==FALSE)
	//Is_Data_High
	#define IS_DATA_HIGH						(( (*((volatile unsigned long *)(GPIO1_PSR0))) & ((unsigned long)(MOD_BIT(DAT_BIT))) )!=FALSE)
	//set clock direction to output
	#define SET_CLOCK_DIR_OUTPUT			(*((volatile unsigned long *)(GPIO1_GDIR0))) |= ((unsigned long)(MOD_BIT(CLK_BIT)))
	//set data direction to output
	#define SET_DATA_DIR_OUTPUT			(*((volatile unsigned long *)(GPIO1_GDIR0))) |= ((unsigned long)(MOD_BIT(DAT_BIT)))
	//Set_Data_Data_Reg_Low
	#define SET_DATA_DR_REG_LOW			(*((volatile unsigned long *)(GPIO1_DR0))) &= ~((unsigned long)(MOD_BIT(DAT_BIT)))
	//Set_Data_Data_Reg_HIGH
	#define SET_DATA_DR_REG_HIGH			(*((volatile unsigned long *)(GPIO1_DR0))) |= ((unsigned long)(MOD_BIT(DAT_BIT)))


	//Disable I2C
	#define DISABLE_I2C						*((volatile unsigned short *)(I2C_I2CR)) = 0x0000
	//set up divider
	//set up I2C clock divider to approx. 400kb/sec (133mhz/384), value 0x12, 346kb/sec
	#define SET_DIVIDER						*((volatile unsigned short *)(I2C_IFDR)) = 0x0012
	//set up address
	#define SET_ADDRESS 						*((volatile unsigned short *)(I2C_IADR)) = 0x0000
	//Enable_I2C
	#define ENABLE_I2C						*((volatile unsigned short *)(I2C_I2CR)) = 0x0080
	//Set_Master_Receive_Mode
	#define SET_MASTER_RECEIVE_MODE_I2C		*((volatile unsigned short *)(I2C_I2CR)) = ((unsigned short)( MOD_BIT(7) | MOD_BIT(5) ))
	//Clear_Interupt_I2C
	#define CLEAR_INTERUPT_I2C				(*((volatile unsigned short *)(I2C_I2SR))) &= ~((unsigned short)(MOD_BIT(1)))
	//Is_Interupt_Status_Clear
	#define IS_INTERUPT_STATUS_CLEAR		(( (*((volatile unsigned short *)(I2C_I2SR))) & ((unsigned short)(MOD_BIT(1))) )==FALSE)
	//Enable_Interupt
	#define ENABLE_I2C_INTERUPT			(*((volatile unsigned short *)(I2C_I2CR))) |= ((unsigned short)(MOD_BIT(6)))
	//Disable_Interupt
	#define DISABLE_I2C_INTERUPT			(*((volatile unsigned short *)(I2C_I2CR))) &= ~((unsigned short)(MOD_BIT(6)))
	//Is_Master_Mode_Not_Set
	#define IS_MASTER_MODE_NOT_SET		(( (*((volatile unsigned short *)(I2C_I2CR))) & ((unsigned short)(MOD_BIT(5))) )==FALSE)
	//Enable_Transmit_Ack
	#define ENABLE_TRANSMIT_ACK			(*((volatile unsigned short *)(I2C_I2CR))) &= ~((unsigned short)(MOD_BIT(3)))
	//Disable_Transmit_Ack
	#define DISABLE_TRANSMIT_ACK			(*((volatile unsigned short *)(I2C_I2CR))) |= ((unsigned short)(MOD_BIT(3)))
	//read_data_register_byte
	#define READ_DATA_REGISTER_BYTE		( (*((volatile unsigned short *)(I2C_I2DR))) & 0x00FF )
	//IS_TRANSFER_NOT_COMPLETED
	#define IS_TRANSFER_NOT_COMPLETED	(( (*((volatile unsigned short *)(I2C_I2SR))) & ((unsigned short)(MOD_BIT(7))) )==FALSE)
	//IS_TRANSFER_COMPLETED
	#define IS_TRANSFER_COMPLETED			(( (*((volatile unsigned short *)(I2C_I2SR))) & ((unsigned short)(MOD_BIT(7))) )!=FALSE)
	//IS_ARBITRATION_LOST
	#define IS_ARBITRATION_LOST			(( (*((volatile unsigned short *)(I2C_I2SR))) & ((unsigned short)(MOD_BIT(4))) )!=FALSE)
	//write_data_register_byte
	#define WRITE_DATA_REGISTER_BYTE		(*((volatile unsigned short *)(I2C_I2DR)))
	//Did_Not_Receive_Ack
	#define DID_NOT_RECEIVE_ACK			(( (*((volatile unsigned short *)(I2C_I2SR))) & ((unsigned short)(MOD_BIT(0))) )!=FALSE)
	//CLEAR_STATUS_REG_I2C
	#define CLEAR_STATUS_REG_I2C			*((volatile unsigned short *)(I2C_I2SR)) = ((unsigned short)0x0000)
	//Is_Bus_Idle
	#define IS_BUS_IDLE						(( (*((volatile unsigned short *)(I2C_I2SR))) & ((unsigned short)(MOD_BIT(5))) )==FALSE)
	//Set_Master_Transmit_Mode
	#define SET_MASTER_TRANSMIT_MODE_I2C	*((volatile unsigned short *)(I2C_I2CR)) = (unsigned short)( MOD_BIT(7) | MOD_BIT(5) | MOD_BIT(4) )
	//Set_Repeat_Start
	#define SET_REPEAT_START				(*((volatile unsigned short *)(I2C_I2CR))) |= ((unsigned short)(MOD_BIT(2)))
	//Set_Slave_Receive_Mode
	#define SET_SLAVE_RECEIVE_MODE_I2C	*((volatile unsigned short *)(I2C_I2CR)) = (unsigned short)( MOD_BIT(7) )



/***************************************************************************************************************************/
/***************************************************************************************************************************/


#elif	PLATFORM_CURRENT == mxl

	#include "db_mx1.h"

	/**********************************************************************/
	//I2CR, I2C Control Register
	//Bits 31-8= RESERVED
	//Bit 7(IEN)= I2C enable (0=disable) (RW)
	//Bit 6(IIEN)= I2C enable Interrupt (0=disable) (RW)
	//Bit 5(MSTA)= Master/Slave (0=slave) (RW)
	//Bit 4(MTX)= Transmit/Receive Mode (0=RX) (RW)
	//Bit 3(TXAK)= Transmit ACK bit? (0=send) (RW)
	//Bit 2(RSTA)= Set for Repeated Start (RW)
	//Bit 1= RESERVED
	//Bit 0 = RESERVED
	/**********************************************************************/
	//I2CSR, I2C Status Register
	//Bits 31-8 = RESERVED
	//Bit 7(ICF)= Data xfer (0=in progress) (R)
	//Bit 6(IAAS)= I2C Slave addressed (0=not addressed) (R)
	//Bit 5(IBB)= I2C bus busy (0=idle) (R)
	//Bit 4(IAL)= Arbitration Lost (gets set under error condition) (RW)
	//Bit 3= RESERVED
	//Bit 2(SRW)= Slave Read/Write (0=RX) (R)
	//Bit 1(IIF)= I2C interrupt pending (0= no interrupt pending) (RW)
	//Bit 0(RXAK)= Received ACK (0=ACK received) (R)
	/**********************************************************************/
	//Bit 16(SCL)= I2C Clock Pin
	//Bit 15(SDA)= I2C Data Pin
	/**********************************************************************/


	//NOP
	#define SET_I2C_CLK_DAISY
	//NOP
	#define SET_I2C_DAT_DAISY

	//set the default Mux Mode
	//first set GUIS special function (I2C), not using GPIO here, sets mode for both clock and data
	//next set GPR special function mode 0, sets mode for both clock and data, like above
	#define SET_CLK_DATA_PORT_MODE_I2C				\
														do{	\
														pGIUS_A->f &= ~(BIT16|BIT15);	\
														pGPR_A->f &= ~(BIT16|BIT15);	\
														}while(0)
	//enable clock pull up
	//enable clock output pull-up resistors
	#define ENABLE_CLOCK_RESISTOR				pPUEN_A->f |= (BIT16)
	//enable data pull up
	//enable data output pull-up resistors
	#define ENABLE_DATA_RESISTOR				pPUEN_A->f |= (BIT15)
	//set All open drain with pull up
	//enable output pull-up resistors
	#define ENABLE_ALL_OUTPUT_RESISTORS		pPUEN_A->f |= (BIT16|BIT15)
	//set clock direction to input
	//turn off CLK as an output, set them to be GPIO inputs
	#define SET_CLOCK_DIR_INPUT				pDDIR_A->f &= ~(BIT16)
	//set data direction to input
	//turn off data as an output, set them to be GPIO inputs
	#define SET_DATA_DIR_INPUT					pDDIR_A->f &= ~(BIT15)
	//set clock and data direction to input
	//turn off CLK and DATA as an output, now set them to be GPIO inputs for a read
	#define SET_CLK_DATA_DIR_INPUT			pDDIR_A->f &= ~(BIT16|BIT15)
	//Set_Clock_Data_Reg_Low
	//set clock's output data register to zero to make clock pin low, toggle by switching input/output, clock is open drain
	#define SET_CLOCK_DR_REG_LOW				pDR_A->b.Bit16 = 0
	//Set_Clock_Data_Reg_HIGH
	//set clock's output data register to one to make clock pin high, toggle by switching input/output, clock is open drain
	#define SET_CLOCK_DR_REG_HIGH				pDR_A->b.Bit16 = 1
	//Select_Clock_Output_Source
	//turn on clock pin's GPIO output source as using data register
	#define SELECT_CLK_GPIO_OUTPUT_SOURCE		pOCR2_A->f |= (BIT1|BIT0)
	//Set_Clock_and_Data_Port_for_GPIO
	//set up clock and data for GPIO mode
	#define SET_CLK_DATA_PORT_MODE_GPIO			pGIUS_A->f |= (BIT16|BIT15)
	//Is_Clock_Not_High
	//Is clock not high state
	#define IS_CLK_NOT_HIGH							(pSSR_A->b.Bit16 != 1)
	//Is_Data_High
	//Check to see if data line went HIGH
	#define IS_DATA_HIGH								(pSSR_A->b.Bit15 == 1)
	//set clock direction to output
	//turn off CLK as an input, set them to be GPIO outputs
	#define SET_CLOCK_DIR_OUTPUT					pDDIR_A->f |= (BIT16)
	//set data direction to output
	//turn off data as an input, set them to be GPIO outputs
	#define SET_DATA_DIR_OUTPUT					pDDIR_A->f |= (BIT15)
	//Set_Data_Data_Reg_Low
	//set data's output data register to zero to make data pin low, toggle by switching input/output, data is open drain
	#define SET_DATA_DR_REG_LOW				pDR_A->b.Bit15 = 0
	//Set_Data_Data_Reg_HIGH
	//set data's output data register to one to make data pin high, toggle by switching input/output, data is open drain
	#define SET_DATA_DR_REG_HIGH				pDR_A->b.Bit15 = 1


	//Disable_I2C
	//I2C is disabled
	#define DISABLE_I2C							I2CR.f = 0x00000000UL
	//set up divider
	//set up I2C clock divider to 400kb/sec (96mhz/240), original value 0x0F,
	//changed this so Tlow >1.3 uS per i2c spec (96mhz/288), new value 0x10, for 333.3kb/sec
	#define SET_DIVIDER							IFDR.f = 0x00000010
	//set_up_address
	//I2C ADDRESS REGISTER
	#define SET_ADDRESS 							IADR.f = 0x00000000
	//Enable_I2C
	//I2C is enabled only, all other bits defaulted
	#define ENABLE_I2C							I2CR.f = 0x00000080UL
	//Set_Master_receive_Mode
	//SET REG. - BIT7=IEN (Enable I2C) , BIT5=MSTA (master/slave mode select bit), set in control register
	#define SET_MASTER_RECEIVE_MODE_I2C		I2CR.f = (BIT7|BIT5)
	//Clear_Interupt_I2C
	// Clear the I2C Interrupt Flag by writing a 0 to IIF bit.
	#define CLEAR_INTERUPT_I2C					I2CSR.b.IIF = FALSE
	//Is_Interupt_Status_Clear
	// Get the I2C Interrupt Flag Status by reading the IIF bit, and seeing if it is FALSE (0).
	#define IS_INTERUPT_STATUS_CLEAR			((I2CSR.b.IIF)==FALSE)
	//Enable_Interupt
	//NOP
	#define ENABLE_I2C_INTERUPT
	//Disable_Interupt
	//NOP
	#define DISABLE_I2C_INTERUPT
	//Is_Master_Mode_Not_Set
	//Are we in control of the bus?  (MSTA, master==1/slave==0 mode select bit)
	#define IS_MASTER_MODE_NOT_SET			(!I2CR.b.MSTA)
	//Enable_Transmit_Ack
	//TXAK==0, send a TX ACK when in receive mode, will not transmit ACK until ninth clock,  (TXAK==1, no ACK sent) (on last byte)
	#define ENABLE_TRANSMIT_ACK		 		I2CR.b.TXAK = 0
	//Disable_Transmit_Ack
	//TXAK==0, send a TX ACK when in receive mode, will not transmit ACK until ninth clock,  (TXAK==1, no ACK sent) (on last byte)
	#define DISABLE_TRANSMIT_ACK		 		I2CR.b.TXAK = 1
	//read_data_register_byte
	//get byte from I2C data register
	#define READ_DATA_REGISTER_BYTE			I2DR.b.D
	//IS_TRANSFER_NOT_COMPLETED
	//If transaction didn't complete, arbitration was lost, fail (ICF, byte transfer in progress==0, transfer complete==1, set at ninth clock of byte transfer)
	#define IS_TRANSFER_NOT_COMPLETED		(I2CSR.b.ICF==FALSE)
	//IS_TRANSFER_COMPLETED
	#define IS_TRANSFER_COMPLETED				(I2CSR.b.ICF!=FALSE)
	//IS_ARBITRATION_LOST
	//NOP
	#define IS_ARBITRATION_LOST
	//write_data_register_byte
	//write byte to I2C data register
	#define WRITE_DATA_REGISTER_BYTE			I2DR.b.D
	//Did_Not_Receive_Ack
	//Did we get an ACK? (RXAK, detect an RX ACK while in transmit mode, ACK received at ninth clock==0, no ACK==1)
	#define DID_NOT_RECEIVE_ACK				I2CSR.b.RXAK
	//CLEAR_STATUS_REG_I2C
	//CLEAR STATUS REGISTER - IAL (arbitration lost==1, cleared by writing a 0) and IIF (I2C interupt, have interupt==1, cleared by writing 0)
	#define CLEAR_STATUS_REG_I2C				I2CSR.f = 0
	//Is_Bus_Idle
	//Is the bus busy or idle? (IBB - bus idle==0, cleared by a stop, bus busy==1, set by a start)
	#define IS_BUS_IDLE							(!I2CSR.b.IBB)
	//Set_Master_Transmit_Mode
	//Set Enable, MSTA, MTX - (BIT7=IEN (Enable I2C) , BIT5=MSTA (master/slave mode select bit), BIT4=MTX (transmit/receive mode select bit))
	#define SET_MASTER_TRANSMIT_MODE_I2C		I2CR.f = (BIT7|BIT5|BIT4)
	//Set_Repeat_Start
	//RSTA - repeat start, 1== enabled, it is self clearing!
	#define SET_REPEAT_START					I2CR.b.RSTA = true
	//Set_Slave_Receive_Mode
	// set device to slave by setting MSTA (bit5) to 0, receive by setting MTX (bit4) to 0, if it was master, this will generate a stop
	#define SET_SLAVE_RECEIVE_MODE_I2C		I2CR.f = (BIT7)


#else
	#error VALID PLATFORM NOT DEFINED
#endif //endof PLATFORM Check
//*******************************************************************************************************************
//*******************************************************************************************************************


//Defines and debugs ************************************************************************************************

void SoundErrorBeep(void);   // debugging beep when jumpstart fails

#define I2C_TIMEOUT 10000    // general IIC timeout, 10ms

#define I2C_TIMEOUT_MS			( I2C_TIMEOUT / 1000 )    // general IIC timeout, 10ms

#define SHOW_ERRORS_AND_STOP 0       // in this line, define as 1 to report jumpstart errors and halt on faiures

//Define below if you want to allow standard debugging			//lmc
#define DEBUG_ON					1

//Define below if you want to allow I2C testing debugging			//lmc
//#define I2C_TEST_DEBUG_ON					1

//Define below if you want to allow I2C interupts to be enabled, the standard is interupts on			//lmc
#define I2C_INTERUPTS_ON					1

//End Defines and debugs ************************************************************************************************


#endif

/////////////////////////////////////////////////////////////////////////////


