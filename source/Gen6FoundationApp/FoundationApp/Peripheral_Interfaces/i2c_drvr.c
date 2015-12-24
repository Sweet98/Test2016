/* ==============================================================================
	Matrix I2C Driver Functions Module Source
   ==============================================================================

   $RCSfile: Gen6FoundationApp/FoundationApp/Peripheral_Interfaces/i2c_drvr.c $
   $Revision: 1.15 $
   $Date: 2010/08/05 07:30:04EDT $

   ==============================================================================

   ============================================================================== */

#include <assert.h>
#include <stdio.h>                             //lmc   was included by menu before, lib includes sprintf()

#include "db_chrio.h"
#include "i2c_drvr.h"
#include "i2c_drvr_hrdwr.h"
#include "timeout.h"
#include "interrupts.h"
#include "menu.h"
#include "readerconfiguration.h"
#include "thread.h"
#include "PowerManagement.h"

#include "ioman.h"                            //lmc   add to use debug theStdInterface.printf()







//*****************************************************************************************************************
//		   Threads and Interupts and Mutexes
//*****************************************************************************************************************


static MT_Event_t transferCompleteEvent;
static MT_Mutex_t accessI2Cmtx;

#if I2C_INTERUPTS
static bool g_bIsTransferNotCompleted = false;
static bool g_bIsMasterModeNotSet = false;
#endif


static threadhandle_t I2CService(void *)
{
	threadhandle_t Ret = 0;             //do not run a thread

#if I2C_INTERUPTS

	g_bIsTransferNotCompleted = false;
	g_bIsMasterModeNotSet = false;

	if( IS_TRANSFER_NOT_COMPLETED )
	{
		g_bIsTransferNotCompleted = true;		//set true if transfer has not been completed, false for completed transaction
	}

	if( IS_ARBITRATION_LOST )
	{
		g_bIsMasterModeNotSet = true;  //Arbitration lost, so Master Mode is not set
	}

#endif

	#ifdef I2C_TEST_DEBUG_ON
	//xputstring("-I-");      //lmc   debug
	#endif

	DISABLE_I2C_INTERUPT;	//Disable the I2C interupt for detecting transfer complete
	CLEAR_STATUS_REG_I2C;	//CLEAR STATUS REG. - IAL (arbitration lost==1, cleared by writing a 0) and IIF (I2C interupt, have interupt==1, cleared by writing 0)
	ENABLE_I2C_INTERUPT;     //enable the I2C interupt for detecting transfer complete

	MT_SetEvent(transferCompleteEvent);

	return Ret;
}


//*****************************************************************************************************************
//		   I2C_Drvr class
//*****************************************************************************************************************

I2C_Drvr * g_pI2C_Drvr = NULL;	//Declare global pointer to class

I2C_Drvr * I2C_Drvr::Create_I2C_Drvr(void)                    //class factory
{
	if (g_pI2C_Drvr == NULL)
	{
		g_pI2C_Drvr = new I2C_Drvr;
		g_pI2C_Drvr->InitI2C();
	}
	ASSERT(g_pI2C_Drvr != NULL);

	return g_pI2C_Drvr;
}


void I2C_Drvr::Destroy_I2C_Drvr(void)
{
	delete g_pI2C_Drvr;
	g_pI2C_Drvr = NULL;
}


// Constructor
I2C_Drvr::I2C_Drvr()
{
	addr = -1;
    m_CheckForAck = 1;
	ImagerSetup();
}


// Destructor
I2C_Drvr::~I2C_Drvr()
{
	UnInitI2C();
}


// Routine Name: ImagerSetup
// Effects: This routine sets up the I2C port for 2D imager
// Inputs: none
// Outputs: none
void I2C_Drvr::ImagerSetup()
{
	DISABLE_INTERRUPTS();
	ENABLE_ALL_OUTPUT_RESISTORS;	//enable output pull-up resistors
	SET_CLK_DATA_DIR_INPUT;			//turn off CLK and data as an output, set them to be inputs, to be pulled high
	SET_DATA_DR_REG_LOW;				//Set data to low state
	SET_CLOCK_DR_REG_LOW;			//Set clock to low state
	SET_I2C_CLK_DAISY;				//set pad daisy chain for i2c3 only
	SET_I2C_DAT_DAISY;           //set pad daisy chain for i2c3 only
	SET_CLK_DATA_PORT_MODE_I2C;	//Set GUIS or pad to be special function (I2C), not using GPIO here, then set GPR to special function mode 0 (I2C)
	DISABLE_I2C;						//I2C disable
	SET_DIVIDER;						//set up I2C clock divider to approx. 400kb/sec,  so Tlow >1.3 uS per i2c spec
	SET_ADDRESS;						//set I2C ADDRESS REGISTER
	ENABLE_I2C;							//I2C enable
	addr = -1;

	RESTORE_INTERRUPTS();
}


// Routine Name: InitI2C
// Effects: This routine sets up the I2C port for interupts
// Inputs: none
// Outputs: none
void I2C_Drvr::InitI2C()
{
	DISABLE_INTERRUPTS();
	DISABLE_I2C_INTERUPT;	//Disable the I2C interupt for detecting transfer complete

	// Set up the I2CService function as our ISR for the I2C interrupt.
	SetVector(I2CService,VEC_I2C_INT);
	// Enable interrupts from the I2C to interrupt the core
	EnableInterruptNumber(VEC_I2C_INT);
	//Create an event to indicate transfer completed, set signal to false
	transferCompleteEvent = MT_CreateEvent(false);
	//Create an mutex to block other thread access
	accessI2Cmtx = MT_CreateMutex(false);

	CLEAR_STATUS_REG_I2C;	//CLEAR STATUS REG. - IAL (arbitration lost==1, cleared by writing a 0) and IIF (I2C interupt, have interupt==1, cleared by writing 0)

	RESTORE_INTERRUPTS();
}


// Routine Name: UnInitI2C
// Effects: This routine unsets the I2C port for interupts
// Inputs: none
// Outputs: none
void I2C_Drvr::UnInitI2C()
{
	DISABLE_INTERRUPTS();
	DISABLE_I2C_INTERUPT;
	MT_CloseEvent(transferCompleteEvent);
	MT_CloseMutex(accessI2Cmtx);
	DisableInterruptNumber(VEC_I2C_INT);
	RESTORE_INTERRUPTS();
}


//***************************************************************************************************************
#if I2C_INTERUPTS
//Interupt driven I2C methods
//***************************************************************************************************************


// Routine Name: Is_Transfer_Complete
// Effects: This routine checks to see if transfer is complete
// Inputs: bool bRunRecover to run recovery on failure, defaulted false.
// Outputs: True if transfer complete, false if condition fails
bool I2C_Drvr::Is_Transfer_Complete( bool bRunRecover )
{
	bool result = true;
	bool bIsTransferNotCompleted = false;
	bool bIsMasterModeNotSet = false;

//	RESET_WATCHDOG_TIMER();

#ifdef I2C_INTERUPTS_ON

	if( !MT_WaitEvent(transferCompleteEvent,I2C_TIMEOUT_MS) )
	{
		bIsTransferNotCompleted = IS_TRANSFER_NOT_COMPLETED;
		bIsMasterModeNotSet = IS_MASTER_MODE_NOT_SET;

		#ifdef I2C_TEST_DEBUG_ON
		xputstring(" Fail Wait Event - IsTransComp! \r\n");      //lmc   debug
		#endif
	}
	else
	{
		bIsTransferNotCompleted = g_bIsTransferNotCompleted;
		bIsMasterModeNotSet = g_bIsMasterModeNotSet;
	}

	MT_ResetEvent(transferCompleteEvent);

#else

	CTimeout testtimeout(I2C_TIMEOUT);        //Wait while poleing I2C interupt flag - Set when one byte transfer completed on ninth clock,
															// or if arbitration is lost
	while( IS_INTERUPT_STATUS_CLEAR && (!testtimeout.HasExpired()) );

	bIsTransferNotCompleted = g_bIsTransferNotCompleted;
	bIsMasterModeNotSet = g_bIsMasterModeNotSet;

	bIsTransferNotCompleted = IS_TRANSFER_NOT_COMPLETED;
	bIsMasterModeNotSet = IS_MASTER_MODE_NOT_SET;

	CLEAR_STATUS_REG_I2C;	//CLEAR STATUS REG. - IAL (arbitration lost==1, cleared by writing a 0) and IIF (I2C interupt, have interupt==1, cleared by writing 0)

#endif

	if( bIsTransferNotCompleted )  //If transaction didn't complete, arbitration was lost, fail (ICF, byte transfer in progress==0, transfer complete==1, set at ninth clock of byte transfer)
	{
		if( bIsMasterModeNotSet )     //Are we in control of the bus?  (MSTA, master==1/slave==0 mode select bit)
		{
			#ifdef I2C_TEST_DEBUG_ON
			xputstring(" Lost Master - IsTransComp! \r\n");      //lmc   debug
			#endif
		}
		else
		{
			#ifdef I2C_TEST_DEBUG_ON
			xputstring(" Trans Not Complete - IsTransComp! \r\n");      //lmc   debug
			#endif
		}

		if( bRunRecover )       //run recovery here, or it must be done in calling function (to aid in debugging with __LINE__).
		{
			RecoverIIC(__LINE__);
		}

		result = false;
	}

	return result;
}


// Routine Name: Dummy_Read
// Effects: This routine sets up the first read transaction. This generates the clocks by doing
//          a "dummy read" , which starts the read clocks.
// Inputs: set true if only receiving one byte, false sends TX ACK for every byte received,
//				last byte is not ACK'ed so input should be set to true
// Outputs: True if slave responded AND data was read, false if condition fails
bool I2C_Drvr::Dummy_Read( bool bLastByte )
{
	bool result;
	int data;
	bool bSkipTransferComplete = true;

	if( IS_MASTER_MODE_NOT_SET )
	{
		result = false;    //Are we in control of the bus?  (MSTA, master==1/slave==0 mode select bit)

		#ifdef I2C_TEST_DEBUG_ON
		xputstring(" Lost Master - DummyRead! \r\n");      //lmc   debug
		#endif
	}
	else
	{
		data = Get_Byte( bLastByte, bSkipTransferComplete );		//Dummy Read
		result = (data >= 0);
	}

	return result;
}


// Routine Name: Read_Request
// Effects: This routine sets up a read transaction. This generates a start, sends the address of the
//          slave to be read from.
// Inputs: address of slave to be read from
// Outputs: True if slave responded, false if condition fails
bool I2C_Drvr::Read_Request(unsigned char ucSlaveAddress)
{
	bool result;

	result = Generate_Start();

	if (result)
	{
        result = Send_Byte(ucSlaveAddress, m_CheckForAck);     //Send Read_Address (Address of device to be read from)
	}
	else
	{
		#ifdef I2C_TEST_DEBUG_ON
		xputstring(" Start Failed - ReadReq! \r\n");      //lmc   debug
		#endif
	}

	if (result)
	{
		SET_MASTER_RECEIVE_MODE_I2C;		//SET REG. - BIT7=IEN (Enable I2C) , BIT5=MSTA (master/slave mode select bit), set in control register

		DISABLE_INTERRUPTS();
		DISABLE_I2C_INTERUPT;	//Disable the I2C interupt for detecting transfer complete
		MT_ResetEvent(transferCompleteEvent);
		CLEAR_STATUS_REG_I2C;	//CLEAR STATUS REG. - IAL (arbitration lost==1, cleared by writing a 0) and IIF (I2C interupt, have interupt==1, cleared by writing 0)

		#ifdef I2C_INTERUPTS_ON
		ENABLE_I2C_INTERUPT;              //enable the I2C interupt for detecting transfer complete
		#endif

		RESTORE_INTERRUPTS();

		if( IS_MASTER_MODE_NOT_SET )
		{
			result = false;    //Are we in control of the bus?  (MSTA, master==1/slave==0 mode select bit)

			#ifdef I2C_TEST_DEBUG_ON
			xputstring(" Lost Master - ReadReq! \r\n");      //lmc   debug
			#endif
		}
	}
	else
	{
		#ifdef I2C_TEST_DEBUG_ON
		xputstring(" Send Failed - ReadReq! \r\n");      //lmc   debug
		#endif
	}

	return result;
}


// Routine Name: Read_Request _w_Repeated_Start
// Effects: This routine sets up a read transaction. This generates a repeat start, sends the address of the
//          slave to be read from.
//				Must be done before we send a stop in our last transaction, so we still must be in master mode.
// Inputs: address of slave to be read from
// Outputs: True if still in master mode, slave responded, false if any condition fails
bool I2C_Drvr::Read_Request_w_Repeated_Start( unsigned char ucSlaveAddress )
{
	bool result;

	result = Generate_Repeated_Start();

	if (result)
	{
        result = Send_Byte(ucSlaveAddress, m_CheckForAck);     //Send Read_Address (Address of device to be read from)
	}
	else
	{
		#ifdef I2C_TEST_DEBUG_ON
		xputstring(" ReStart Failed - ReadReqWRepeat! \r\n");      //lmc   debug
		#endif
	}

	if (result)
	{
		SET_MASTER_RECEIVE_MODE_I2C;	//SET REG. - BIT7=IEN (Enable I2C) , BIT5=MSTA (master/slave mode select bit), set in control register

      DISABLE_INTERRUPTS();
		DISABLE_I2C_INTERUPT;	//Disable the I2C interupt for detecting transfer complete
		MT_ResetEvent(transferCompleteEvent);
		CLEAR_STATUS_REG_I2C;	//CLEAR STATUS REG. - IAL (arbitration lost==1, cleared by writing a 0) and IIF (I2C interupt, have interupt==1, cleared by writing 0)

		#ifdef I2C_INTERUPTS_ON
		ENABLE_I2C_INTERUPT;              //enable the I2C interupt for detecting transfer complete
		#endif

		RESTORE_INTERRUPTS();

		if( IS_MASTER_MODE_NOT_SET )
		{
			result = false;    //Are we in control of the bus?  (MSTA, master==1/slave==0 mode select bit)

			#ifdef I2C_TEST_DEBUG_ON
			xputstring(" Lost Master - ReadReqWRepeat! \r\n");      //lmc   debug
			#endif
		}
	}
	else
	{
		#ifdef I2C_TEST_DEBUG_ON
		xputstring(" Send Failed - ReadReqWRepeat! \r\n");      //lmc   debug
		#endif
	}

	return result;
}


// Routine Name: Get_Byte
// Effects: This routine gets a byte from the I2C port
// Inputs: bool bLastByte false sends TX ACK for every byte received, last byte is not ACK'ed so input should be set to true,
//				bool bSkipTransferComplete true will skip the wait event, defaulted to false.
// Outputs: byte value
int I2C_Drvr::Get_Byte( bool bLastByte, bool bSkipTransferComplete )
{
	if( IS_MASTER_MODE_NOT_SET && (!bSkipTransferComplete) )     //Are we in control of the bus?  (MSTA, master==1/slave==0 mode select bit)
	{

		#ifdef DEBUG_ON
		if ((GET_SETTING(GeneralDebugOn)) || GET_SETTING( ImagerDebugOn ))
		{
			xputstring("Arbitration Lost get!\r\n");
		}
		#endif

		#ifdef I2C_TEST_DEBUG_ON
		xputstring(" Arbitration Lost - getbyte! \r\n");      //lmc   debug
		#endif

		return -1;
	}

	if( !bSkipTransferComplete )
	{
		bool bTransferCompleted = Is_Transfer_Complete(false);      //check if byte transfer was completed

		if( !bTransferCompleted )  //If transaction didn't complete, arbitration was lost, fail (ICF, byte transfer in progress==0, transfer complete==1, set at ninth clock of byte transfer)
		{
			#ifdef I2C_TEST_DEBUG_ON
			xputstring(" Trans Not Complete - getbyte! \r\n");      //lmc   debug
			#endif

			RecoverIIC(__LINE__);
			return -1;
		}
	}

	if(bLastByte) DISABLE_TRANSMIT_ACK;    //must be done when reading second to last byte, so last byte clocks do not include a ACK, indicating end of read
	else ENABLE_TRANSMIT_ACK;	//TXAK==0, send a TX ACK when in receive mode, will not transmit ACK until ninth clock, (TXAK==1, no ACK sent) (on last byte)

	int iData = READ_DATA_REGISTER_BYTE;	   //get byte from I2C data register
//	RESET_WATCHDOG_TIMER();

	return (iData&0xff);
}


// Routine Name: Send_Byte
// Effects: This routine sends a byte out the I2C port
// Inputs:  Byte to send, if ack check is required
// Outputs: True if ACK received, false otherwise
bool I2C_Drvr::Send_Byte( unsigned char ucByteToSend, int iCheckForAck )
{
	if( addr < 0 )
	{
		addr = ucByteToSend;
	}

	if( IS_MASTER_MODE_NOT_SET )		//Did we lose arbitration? (MSTA, master==1/slave==0 mode select bit)
	{
		#ifdef DEBUG_ON
		if ((GET_SETTING(GeneralDebugOn)) || GET_SETTING( ImagerDebugOn ))
		{
			xputstring("Arbitration Lost send!\r\n");
		}
		#endif

		#ifdef I2C_TEST_DEBUG_ON
		xputstring(" Arbitration Lost - sendbyte! \r\n");      //lmc   debug
		#endif

		return false;
	}

	WRITE_DATA_REGISTER_BYTE = ucByteToSend;	//write byte to I2C data register
//	RESET_WATCHDOG_TIMER();

	bool bTransferCompleted = Is_Transfer_Complete(false);     //check if byte transfer was completed

	if( !bTransferCompleted )  //If transaction didn't complete, arbitration was lost, fail (ICF, byte transfer in progress==0, transfer complete==1, set at ninth clock of byte transfer)
	{
		#ifdef I2C_TEST_DEBUG_ON
		xputstring(" Transfer Not Completed - sendbyte! \r\n");         //lmc   debug
		#endif

		RecoverIIC(__LINE__);
		return(false); //Return failure, transaction not completed
	}

    if( DID_NOT_RECEIVE_ACK && (iCheckForAck > 0) )       //Did we get an ACK? (RXAK, detect an RX ACK while in transmit mode, ACK received at ninth clock==0, no ACK==1)
	{
		RecoverIIC(__LINE__);
		return(false); //Return failure, no ACK
	}
	else
	{
		return(true); //got ACK
	}
}


// Routine Name: Generate_Start
// Effects: This routine generates I2C start condition
// Inputs: none
// Outputs: none
bool I2C_Drvr::Generate_Start(void)
{
	bool done = false;

	DISABLE_INTERRUPTS();
	ENABLE_I2C;							//I2C enable
	DISABLE_I2C_INTERUPT;	//Disable the I2C interupt for detecting transfer complete
	RESTORE_INTERRUPTS();

//	RESET_WATCHDOG_TIMER();

	CTimeout testtimeout(I2C_TIMEOUT);
	while(!testtimeout.HasExpired() && !done)
	{
		DISABLE_INTERRUPTS();
	 	if( IS_BUS_IDLE )	//Is the bus busy or idle? (IBB - bus idle==0, cleared by a stop, bus busy==1, set by a start)
		{
			SET_MASTER_TRANSMIT_MODE_I2C;		//Set Enable, MSTA, MTX - (BIT7=IEN (Enable I2C) , BIT5=MSTA (master/slave mode select bit), BIT4=MTX (transmit/receive mode select bit))

			MT_ResetEvent(transferCompleteEvent);
			CLEAR_STATUS_REG_I2C;	//CLEAR STATUS REG. - IAL (arbitration lost==1, cleared by writing a 0) and IIF (I2C interupt, have interupt==1, cleared by writing 0)

			#ifdef I2C_INTERUPTS_ON
			ENABLE_I2C_INTERUPT;              //enable the I2C interupt for detecting transfer complete
			#endif

			done = true;                   // Setting MSTA causes a start condition
		}
		RESTORE_INTERRUPTS();
	}			//end while()

	if( !done )
	{
		#ifdef I2C_TEST_DEBUG_ON
		xputstring(" Generate start fail \r\n");                //lmc   debug
		#endif

		RecoverIIC(__LINE__);	//Was the bus busy through out timeout?

		Generate_Stop();		//we are finished if we failed start
	}

#ifdef I2C_TEST_DEBUG_ON
	//else	xputstring(" Generate start pass \r\n");           //lmc   debug
#endif

#if SHOW_ERRORS_AND_STOP
	if( addr != -1 ) xputstring("start w/o stop ?\r\n");
#endif

	return done;
}


// Routine Name: Generate_Stop
// Effects: This routine generates I2C stop condition
// Inputs: none
// Outputs: none
void I2C_Drvr::Generate_Stop(void)
{
	 DISABLE_INTERRUPTS();
	 DISABLE_I2C_INTERUPT;	//Disable the I2C interupt for detecting transfer complete
	 MT_ResetEvent(transferCompleteEvent);
	 CLEAR_STATUS_REG_I2C;	//CLEAR STATUS REG. - IAL (arbitration lost==1, cleared by writing a 0) and IIF (I2C interupt, have interupt==1, cleared by writing 0)
	 SET_SLAVE_RECEIVE_MODE_I2C;		// set device to slave, if it was master, this will generate a stop
	 addr = -1;					//Indicate that a stop has occured by setting addr to -1
	 RESTORE_INTERRUPTS();
}


// Routine Name: Generate_Repeated_Start
// Effects: This routine generates I2C repeat start condition.
//				Must be done only when a bus master, or arbitration is lost
// Inputs: none
// Outputs: a bool for pass/fail, a pass if we are still master
bool I2C_Drvr::Generate_Repeated_Start(void)
{
	bool done = true;

	DISABLE_INTERRUPTS();
	DISABLE_I2C_INTERUPT;	//Disable the I2C interupt for detecting transfer complete
	MT_ResetEvent(transferCompleteEvent);
	CLEAR_STATUS_REG_I2C;	//CLEAR STATUS REG. - IAL (arbitration lost==1, cleared by writing a 0) and IIF (I2C interupt, have interupt==1, cleared by writing 0)

	#ifdef I2C_INTERUPTS_ON
	ENABLE_I2C_INTERUPT;              //enable the I2C interupt for detecting transfer complete
	#endif

	RESTORE_INTERRUPTS();

//	RESET_WATCHDOG_TIMER();

	if( IS_MASTER_MODE_NOT_SET )		//Did we lose arbitration? (MSTA, master==1/slave==0 mode select bit)
	{
		#ifdef DEBUG_ON
		if ((GET_SETTING(GeneralDebugOn)) || GET_SETTING( ImagerDebugOn ))
		{
			xputstring("Arbitration Lost - Repeated Start!\r\n");
		}
		#endif

		#ifdef I2C_TEST_DEBUG_ON
		xputstring("Arbitration Lost - Repeated Start!\r\n");
		#endif

		done = false;
	}
	else
	{
		DISABLE_INTERRUPTS();
		SET_REPEAT_START;		//RSTA - repeat start, 1== enabled, it is self clearing!
		RESTORE_INTERRUPTS();
	}

	return done;
}


//***************************************************************************************************************
#else
//Poled I2C methods
//***************************************************************************************************************



// Routine Name: Read_Request
// Effects: This routine sets up a read transaction. This generates a start, sends the address of the
//          slave to be read from, and does a "dummy read" in order to start the read clocks
// Inputs: address of slave to be read from
// Outputs: True if slave responded AND data was read, false if either condition fails
bool I2C_Drvr::Read_Request(unsigned char ucSlaveAddress)
{
	bool result;
	int data;

	Generate_Start();
    result = Send_Byte(ucSlaveAddress, m_CheckForAck);     //Send Read_Address (Address of device to be read from)

	if (result)
	{
		SET_MASTER_RECEIVE_MODE_I2C;		//SET REG. - BIT7=IEN (Enable I2C) , BIT5=MSTA (master/slave mode select bit), set in control register
		data = Get_Byte(false);		//Dummy Read
		result = (data >= 0);
	}

	return result;
}


// Routine Name: Read_Request _w_Repeated_Start
// Effects: This routine sets up a read transaction. This generates a repeat start, sends the address of the
//          slave to be read from, and does a "dummy read" in order to start the read clocks.
//				Must be done before we send a stop in our last transaction, so we still must be in master mode.
// Inputs: address of slave to be read from
// Outputs: True if still in master mode, slave responded AND data was read, false if any condition fails
bool I2C_Drvr::Read_Request_w_Repeated_Start( unsigned char ucSlaveAddress )
{
	bool result;
	int data;

	result = Generate_Repeated_Start();

	if (result)
	{
        result = Send_Byte(ucSlaveAddress, m_CheckForAck);     //Send Read_Address (Address of device to be read from)
	}

	if (result)
	{
		SET_MASTER_RECEIVE_MODE_I2C;	//SET REG. - BIT7=IEN (Enable I2C) , BIT5=MSTA (master/slave mode select bit), set in control register
		data = Get_Byte(false);		//Dummy Read
		result = (data >= 0);
	}

	return result;
}


// Routine Name: Get_Byte
// Effects: This routine gets a byte from the I2C port
// Inputs: false sends TX ACK for every byte received, last byte is not ACK'ed so input should be set to true
// Outputs: byte value
int I2C_Drvr::Get_Byte(bool bLastByte)
{
	CLEAR_INTERUPT_I2C;				// Clear the I2C Interrupt Flag.

	if( IS_MASTER_MODE_NOT_SET ) return -1;    //Are we in control of the bus?  (MSTA, master==1/slave==0 mode select bit)

	if(bLastByte) DISABLE_TRANSMIT_ACK;
	else ENABLE_TRANSMIT_ACK;	//TXAK==0, send a TX ACK when in receive mode, will not transmit ACK until ninth clock, (TXAK==1, no ACK sent) (on last byte)

	int iData = READ_DATA_REGISTER_BYTE;	   //get byte from I2C data register
//	RESET_WATCHDOG_TIMER();

	CTimeout testtimeout(I2C_TIMEOUT);
  	while( IS_INTERUPT_STATUS_CLEAR && (!testtimeout.HasExpired()) );

	if( IS_TRANSFER_NOT_COMPLETED )  //If transaction didn't complete, arbitration was lost, fail (ICF, byte transfer in progress==0, transfer complete==1, set at ninth clock of byte transfer)
	{
		RecoverIIC(__LINE__);
		return -1;
	}

	return (iData&0xff);
}


// Routine Name: Send_Byte
// Effects: This routine sends a byte out the I2C port
// Inputs:  Byte to send, if ack check is required
// Outputs: True if ACK received, false otherwise
bool I2C_Drvr::Send_Byte( unsigned char ucByteToSend, int iCheckForAck )
{
	if( addr < 0 )
	{
		addr = ucByteToSend;
	}

	CLEAR_INTERUPT_I2C;	// Clear the I2C Interrupt Flag.

	if( IS_MASTER_MODE_NOT_SET )		//Did we lose arbitration? (MSTA, master==1/slave==0 mode select bit)
	{
#ifdef DEBUG_ON
		if ((GET_SETTING(GeneralDebugOn)) || GET_SETTING( ImagerDebugOn ))
		{
			xputstring("Arbitration Lost send!\r\n");
		}
#endif

#ifdef I2C_TEST_DEBUG_ON
		xputstring(" Arbitration Lost - sendbyte! \r\n");      //lmc   debug
#endif

		return false;
	}

	WRITE_DATA_REGISTER_BYTE = ucByteToSend;	//write byte to I2C data register
//	RESET_WATCHDOG_TIMER();

	CTimeout testtimeout(I2C_TIMEOUT);
	while( IS_INTERUPT_STATUS_CLEAR && !testtimeout.HasExpired() ); //Wait while no I2C interupt - Set when one byte transfer completed on ninth clock,
																						// or if arbitration is lost
	if( IS_TRANSFER_NOT_COMPLETED )  //If transaction didn't complete, arbitration was lost, fail (ICF, byte transfer in progress==0, transfer complete==1, set at ninth clock of byte transfer)
	{

#ifdef I2C_TEST_DEBUG_ON
		xputstring(" Transfer Not Completed - sendbyte! \r\n");         //lmc   debug
#endif

		RecoverIIC(__LINE__);
		return(false); //Return failure, transaction not completed
	}

    if( DID_NOT_RECEIVE_ACK && (iCheckForAck > 0) )       //Did we get an ACK? (RXAK, detect an RX ACK while in transmit mode, ACK received at ninth clock==0, no ACK==1)
	{
		RecoverIIC(__LINE__);
		return(false); //Return failure, no ACK
	}
	else
	{
		return(true); //got ACK
	}
}


// Routine Name: Generate_Start
// Effects: This routine generates I2C start condition
// Inputs: none
// Outputs: none
bool I2C_Drvr::Generate_Start(void)
{
	bool done = false;
	ENABLE_I2C;							//I2C enable
	CLEAR_STATUS_REG_I2C;	//CLEAR STATUS REG. - IAL (arbitration lost==1, cleared by writing a 0) and IIF (I2C interupt, have interupt==1, cleared by writing 0)
//	RESET_WATCHDOG_TIMER();

	CTimeout testtimeout(I2C_TIMEOUT);
	while(!testtimeout.HasExpired() && !done)
	{
		DISABLE_INTERRUPTS();
	 	if( IS_BUS_IDLE )	//Is the bus busy or idle? (IBB - bus idle==0, cleared by a stop, bus busy==1, set by a start)
		{
			SET_MASTER_TRANSMIT_MODE_I2C;		//Set Enable, MSTA, MTX - (BIT7=IEN (Enable I2C) , BIT5=MSTA (master/slave mode select bit), BIT4=MTX (transmit/receive mode select bit))
     		done = true;                   // Setting MSTA causes a start condition
		}
		RESTORE_INTERRUPTS();
	}

	if( !done )
	{
		#ifdef I2C_TEST_DEBUG_ON
		xputstring(" Generate start fail \r\n");                //lmc   debug
		#endif

		RecoverIIC(__LINE__);	//Was the bus busy through out timeout?
	}

#ifdef I2C_TEST_DEBUG_ON
	//else	xputstring(" Generate start pass \r\n");           //lmc   debug
#endif

#if SHOW_ERRORS_AND_STOP
	if( addr != -1 ) xputstring("start w/o stop ?\r\n");
#endif

	return done;
}


// Routine Name: Generate_Stop
// Effects: This routine generates I2C stop condition
// Inputs: none
// Outputs: none
void I2C_Drvr::Generate_Stop(void)
{
	 DISABLE_INTERRUPTS();
	 CLEAR_STATUS_REG_I2C;	//CLEAR STATUS REG. - IAL (arbitration lost==1, cleared by writing a 0) and IIF (I2C interupt, have interupt==1, cleared by writing 0)
	 SET_SLAVE_RECEIVE_MODE_I2C;		// set device to slave, if it was master, this will generate a stop
	 addr = -1;					//Indicate that a stop has occured by setting addr to -1
	 RESTORE_INTERRUPTS();
}


// Routine Name: Generate_Repeated_Start
// Effects: This routine generates I2C repeat start condition.
//				Must be done only when a bus master, or arbitration is lost
// Inputs: none
// Outputs: a bool for pass/fail, a pass if we are still master
bool I2C_Drvr::Generate_Repeated_Start(void)
{
	bool done = true;

//	RESET_WATCHDOG_TIMER();

	if( IS_MASTER_MODE_NOT_SET )		//Did we lose arbitration? (MSTA, master==1/slave==0 mode select bit)
	{
		#ifdef DEBUG_ON
		if ((GET_SETTING(GeneralDebugOn)) || GET_SETTING( ImagerDebugOn ))
		{
			xputstring("Arbitration Lost - Repeated Start!\r\n");
		}
		#endif

		#ifdef I2C_TEST_DEBUG_ON
		xputstring("Arbitration Lost - Repeated Start!\r\n");
		#endif

		done = false;
	}
	else
	{
		DISABLE_INTERRUPTS();
		SET_REPEAT_START;		//RSTA - repeat start, 1== enabled, it is self clearing!
		RESTORE_INTERRUPTS();
	}

	return done;
}


//***************************************************************************************************************
#endif
//End poled I2C methods
//***************************************************************************************************************



//***************************************************************************************************************
//Common I2C methods
//***************************************************************************************************************


#define IIC_REC_BIT_TIME 4 		//recovery timeout in uS


// Routine Name: RecoverIIC
// Effects: This routine takes over SCLK , toggles it, then sets up the I2C port for 2D imager
//			The intent is to re-start I2C communication when imager communication has failed
// Inputs: line this routine was called from
// Outputs: none
void I2C_Drvr::RecoverIIC(unsigned int line)
{
	int	failure = TRUE;
	char	errmsg[100];
	int	temp;
	int	i = -1;

	temp = addr;

	// DEBUG_puts( "Recovery Attempted\r\n" );
	#ifdef I2C_TEST_DEBUG_ON
   xputstring( " Recovery Attempted \r\n" );            //lmc   debug
	#endif

	for (int j=0; j<3; j++)              //Try recovery three times before giving up
	{
        // CLK and DATA pin config for GPIO
        DISABLE_INTERRUPTS();
		SET_CLK_DATA_DIR_INPUT;	//turn on data and clock as GPIO inputs to read
		SET_CLOCK_DR_REG_LOW;	//set clock's output data register to zero to make clock pin low, toggle by switching input/output, clock is open drain
        SELECT_CLK_GPIO_OUTPUT_SOURCE;  //turn on clock pin's GPIO output source as using data register
		SET_CLK_DATA_PORT_MODE_GPIO;	//set up clock and data ports for GPIO
		RESTORE_INTERRUPTS();

		CTimeout js_timeout(IIC_REC_BIT_TIME);

		for (i=1;i<10;i++)		      //Clock in up to 9 bits to clear imager state
		{
//			RESET_WATCHDOG_TIMER();

			js_timeout.SetExpiration(I2C_TIMEOUT);
			while( IS_CLK_NOT_HIGH && !js_timeout.HasExpired() );

			if( IS_CLK_NOT_HIGH )
			{
				#ifdef I2C_TEST_DEBUG_ON
				xputstring( " Recovery Fail-Clock not high \r\n" );       //lmc   debug
				#endif

				break;			  			//clock did not go high, fail
			}
			else
			{
				if( IS_DATA_HIGH )       	//Check to see if data line went HIGH
				{
	          		failure = FALSE;         //The I2C was jumpstarted correctly
	               break;                   //exit this loop
	      	}
				//xputstring( " Toggle Clock \r\n" );             //lmc   debug
			}

			DISABLE_INTERRUPTS();
			SET_CLOCK_DIR_OUTPUT;				//turn on CLK as an output to set clock to a low state
			js_timeout.SetExpiration(IIC_REC_BIT_TIME);
			while(!js_timeout.HasExpired());
			SET_CLOCK_DIR_INPUT;				//turn on CLK as an input, because it is open drain, it is set to a high state by pull up
			js_timeout.SetExpiration(IIC_REC_BIT_TIME);
			while(!js_timeout.HasExpired());
			RESTORE_INTERRUPTS();
		}

		js_timeout.SetExpiration(I2C_TIMEOUT);
		while( IS_CLK_NOT_HIGH && !js_timeout.HasExpired() );		//wait for clock to go high again

		// braces below not part of while loop, it is for disable enable irqs
		//here the start /stop will end a slave transaction
		{
			DISABLE_INTERRUPTS();
			SET_DATA_DR_REG_LOW;		        			//set DATA LINE's output Data register to zero (open drain pulls low)
			SET_DATA_DIR_OUTPUT;		        		//set DATA LINE's data direction to output, for a START (data line high to low)
			js_timeout.SetExpiration(IIC_REC_BIT_TIME);
			while(!js_timeout.HasExpired());
			SET_DATA_DIR_INPUT;						//set DATA LINE's data direction to input, for a STOP  (data line low to high)
			js_timeout.SetExpiration(IIC_REC_BIT_TIME);
			while(!js_timeout.HasExpired());
			RESTORE_INTERRUPTS();
		}

		ImagerSetup();   // set clock and data pins back to IIC config
		MT_Delay(2);

#ifdef DEBUG_ON
		if((GET_SETTING(GeneralDebugOn)) || GET_SETTING( ImagerDebugOn ) || SHOW_ERRORS_AND_STOP)
        {
            sprintf(errmsg,"Jumpstart at line %d DevAddr %X bits %d status  %s\r\n",line,(unsigned int)temp,i,failure?"F":"P");
            xputstring(errmsg);
        }
#else
		if( SHOW_ERRORS_AND_STOP )
        {
            sprintf(errmsg,"Jumpstart at line %d DevAddr %X bits %d status  %s\r\n",line,(unsigned int)temp,i,failure?"F":"P");
            xputstring(errmsg);
        }
#endif

#ifdef I2C_TEST_DEBUG_ON
        sprintf(errmsg,"Jumpstart at line %d DevAddr %X bits %d status  %s\r\n",line,(unsigned int)temp,i,failure?"F":"P");
        xputstring(errmsg);
#endif

        if (!failure)       //if we jump started correctly, we are finished
        {
            break;
		}
	}								//end for() retry loop

#if (defined(RFSCAN) && !defined(VERIFIER) || SHOW_ERRORS_AND_STOP)
	// If JumpStart did not succeed then the whole unit must power down to recover I2C
	if( failure )
	{
#if (SHOW_ERRORS_AND_STOP)
		SoundErrorBeep();
		while( true )
#endif
//      RESET_WATCHDOG_TIMER();
       EnterPowerState(POWERDOWN);
	}
#endif

	return;
}



//***************************************************************************************************************
//***************************************************************************************************************
//
//Encapsulated Privite-Common Read and Write I2C methods
//
//***************************************************************************************************************
//***************************************************************************************************************
#if I2C_INTERUPTS
//Interupt Driven Versions of I2C methods
//***************************************************************************************************************


// Routine Name: Send_Message_I2C
// Effects: This routine is a common method to send data to imager or psoc.
//				Sub address being sent is optional by setting the bool iSendSub parameter.
//				Sending data is optional by setting parameter nCount to zero and/or parameter ucBuffer/uiBuffer to NULL.
// Inputs: deviceAddress - I2C device address
//				subAddress - the register # in imager communication, offset to register in PSOC communication (length -one byte, char) .
//								(NOTE: a read from the PSOC resets the register offset to '0',  see dummyRead() )
//				uiSubAddress - the register # in imager communication, offset to register in PSOC communication  (two byte, short).
//          	ucBuffer - pointer to char data buffer (NULL means no data sent).
//          	uiBuffer - pointer to short data buffer (NULL means no data sent). Must chose one buffer or the other if data is being sent.
//				nCount - number of bytes of data to send ('0' means no data sent ).
//				iSendSub - selection for sending sub address (1 means send char sub address, 2 means send short sub address).
//				iUseBuffer - selection for using char buffer, '0' for char data buffer, '1' for using short data buffer, '2' for
//									using short data buffer with an address offset to switch register size writes (8 or 16 bit).
//				iOffsetCount - address offset were we switch from writing 8 bit registers to 16 bit registers
//				bSendStop - selection for sending stop at end of write, no stop needed if using repeat start in a read request next.
// Outputs: bool - true indicates success
// Packet Format:
//				PSOC - start, write_address, sub address, stop, start, write_address, send_data, stop
//				Imager- start, write_address, sub address, send_data, stop

bool I2C_Drvr::Send_Message_I2C( unsigned char deviceAddress, unsigned char subAddress, unsigned short uiSubAddress, const unsigned char * ucBuffer, const unsigned short * uiBuffer, unsigned int nCount, int iSendSub, int iUseBuffer, int iOffsetCount, bool bSendStop )
{
	//Check for MUTEX
	MT_WaitMutex(accessI2Cmtx,INFINITE);

	int iCount;
	unsigned char ucChar;
	bool bStatus = TRUE;

	bStatus = Generate_Start();

	if( bStatus )
	{
        if( !Send_Byte(deviceAddress, m_CheckForAck) )           //send the device address
		{
			bStatus = FALSE;

			#ifdef I2C_TEST_DEBUG_ON
			xputstring("\r\n Failed Send address ");          //lmc   debug
			xputchar(deviceAddress);                          //lmc   debug
			xputstring(" \r\n ");                             //lmc   debug
			#endif
		}
	}

	if( bStatus && (iSendSub > 0) )               //if enabled, send the sub address
	{
		if( iSendSub == 1 )            //send one byte (char)
		{
            if( !Send_Byte(subAddress, m_CheckForAck) )
			{
				bStatus = FALSE;

				#ifdef I2C_TEST_DEBUG_ON
				xputstring(" failed send sub-address \r\n");         //lmc   debug
				#endif
			}
		}
		else if( iSendSub == 2 )		//send two bytes (short)
		{
			ucChar = (unsigned char)(((uiSubAddress)>>8)&0xff);

            if( !Send_Byte(ucChar, m_CheckForAck) )
			{
				bStatus = FALSE;

				#ifdef I2C_TEST_DEBUG_ON
				xputstring(" failed send most signif sub-address \r\n");         //lmc   debug
				#endif
			}

			ucChar = (unsigned char)((uiSubAddress)&0xff);

            if( !Send_Byte(ucChar, m_CheckForAck) )
			{
				bStatus = FALSE;

				#ifdef I2C_TEST_DEBUG_ON
				xputstring(" failed send least signif sub-address \r\n");         //lmc   debug
				#endif
			}
		}
	}

	if( bStatus && (nCount > 0) && (uiBuffer != NULL) && (iSendSub == 1) && (iUseBuffer > 0) )		//iUseBuffer == to 1 or 2, and using uiBuffer (short)
	{
		iCount = 0;

		while(nCount--)
		{
			if( (((subAddress + iCount) > iOffsetCount) && (iUseBuffer==2)) || (iUseBuffer==1) )    //iOffsetCount ==  to 0x7f for JADE
			{                                                                           //switches from one byte to two byte register writes
				ucChar = (unsigned char)(((*uiBuffer)>>8)&0xff);

                if( !Send_Byte(ucChar, m_CheckForAck) )
				{
					bStatus = FALSE;
					break;
				}

				ucChar = (unsigned char)((*uiBuffer)&0xff);

                if( !Send_Byte(ucChar, m_CheckForAck) )
				{
					bStatus = FALSE;
					break;
				}
			}
			else if( iUseBuffer == 2 )
			{
				ucChar = (unsigned char)(((*uiBuffer))&0xff);

                if( !Send_Byte(ucChar, m_CheckForAck) )
				{
					bStatus = FALSE;
					break;
				}
			}
			else		//error
			{
				bStatus = FALSE;
				break;
			}
			uiBuffer++;
			iCount++;
		}				//end while()
	}
	else if( iUseBuffer == 0 )			//iUseBuffer == 0, and using ucBuffer (char)
	{
		if( bStatus && (nCount > 0) && (ucBuffer != NULL) )         //send the bytes
		{
			for(iCount=0; iCount < nCount; iCount++)
			{
                if( !Send_Byte(ucBuffer[iCount], m_CheckForAck) )
				{
					bStatus = FALSE;

					#ifdef I2C_TEST_DEBUG_ON
					xputstring(" failed to send bytes \r\n");          //lmc   debug
					#endif

					break;
				}
			}		//end for()
		}
	}

	if( bSendStop || !bStatus )
	{
		Generate_Stop();
	}

	//Release the mutex
	MT_ReleaseMutex(accessI2Cmtx);

	return bStatus;
}


// Routine Name: Receive_Message_I2C
// Effects: This routine is a common method to get data from imager or psoc.
// Inputs: deviceAddress - I2C device address to read from (up to user to add on  '1'  to address to change from write to read mode.
//				subAddress - the register # in imager communication, offset to register in PSOC communication, set by a previous write .
//              cSendCount - If not null, will send receive count to device.
//              ucBuffer - pointer to (char) data buffer .
//				uiBuffer - pointer to (short) data buffer . Must chose one buffer or the other for the read.
//				nCount - number of bytes of data to read .
//				iUseBuffer - selection for using char buffer, '0' for char data buffer, '1' for using short data buffer, '2' for
//									using short data buffer with an address offset to switch register size writes (8 or 16 bit).
//				iOffsetCount - address offset were we switch from reading 8 bit registers to 16 bit registers
//              iUseSendCount - 0 is do not send cSendCount, 1 is send it with ack required, 2 send it with no ack required.
//              bSendRepeatedStart - selection for sending repeated start at start of read, no stop sent is required in previous write to use this.
// Outputs: bool - true indicates success
// Packet Format:
//				PSOC - start, write_address, sub address, stop, start, read_address, get_data, stop
//				Imager- start, write_address, sub address, stop, start, read_address, get_data, stop
//							(NOTE: Read_Request == start, read_address )
//				JADE Imager- start, write_address, sub address, set_repeat_start, read_address, get_data, stop
//							(NOTE: Read_Request_w_Repeated_Start == set_repeat_start, read_address )

bool I2C_Drvr::Receive_Message_I2C( unsigned char deviceAddress, unsigned char subAddress, unsigned char cSendCount, unsigned char * ucBuffer, unsigned short * uiBuffer, unsigned int nCount, int iUseBuffer, int iOffsetCount, int iUseSendCount, bool bSendRepeatedStart )
{
	//Check for MUTEX
	MT_WaitMutex(accessI2Cmtx,INFINITE);

	int iCount;
	unsigned char ucChar;
	int i;
	int temp = -1;                     //initialize to an error state
	bool bStatus = TRUE;
	bool bDoDummyRead = TRUE;
	bool bDoStop = TRUE;

	if( bSendRepeatedStart )
	{
		bStatus = Read_Request_w_Repeated_Start(deviceAddress);
	}
	else
	{
		bStatus = Read_Request(deviceAddress);
	}

    if( bStatus && (cSendCount != 0) && (iUseSendCount != 0) )
	{
        int iCheckForAck = 1;       //if iUseSendCount == 1, then indicate need to wiat for ack

        if(iUseSendCount != 1)      //if 2, do not wait for ack
        {
            iCheckForAck = 0;       //indicate not to wait for ack
        }

        if( !Send_Byte(cSendCount, iCheckForAck) )           //send the device address
		{
			bStatus = FALSE;

			#ifdef I2C_TEST_DEBUG_ON
            xputstring("\r\n Failed Send count ");          //lmc   debug
            xputchar( cSendCount );                          //lmc   debug
			xputstring(" \r\n ");                             //lmc   debug
			#endif
		}
    }


	if( bStatus && (nCount > 0) && (iUseBuffer > 0) )		//iUseBuffer == to 1 or 2, and using uiBuffer (short)
	{
		iCount = 0;

		while (nCount--)
		{
			// everything is 2 bytes, the FALSE indicates the first byte can't be the last
			// FIXME:  do some exception handling for 16 bit versus 8 bit registers
			if( (((subAddress + iCount) > iOffsetCount) && (iUseBuffer==2)) || (iUseBuffer==1) )	//iOffsetCount ==  to 0x7f for JADE
			{                                                                                      //switches from one byte to two byte register writes
				//Start read clocks with a dummy read to get first read data
				if( bDoDummyRead )                //indicates we have not started the read clocks yet
				{
					Dummy_Read( FALSE );           //never the last byte because we are reading two here
					bDoDummyRead = FALSE;
				}

				//Get first byte
				temp = Get_Byte( nCount == 0, FALSE );       //if second to last byte, indicate we will not ACK last byte

				if( temp == -1 )		//error
				{
					ucChar = (unsigned char)(temp & 0xff);
					*uiBuffer = (((unsigned short)ucChar))&0x00ff;
					bStatus = FALSE;
					break;
				}
				else
				{
					ucChar = (unsigned char)temp;
					*uiBuffer = (((unsigned short)ucChar)<<8)&0xff00;
				}

				if( nCount == 0 )                    //if we are last byte, we must do a stop first
				{
					bool bTransferCompleted = Is_Transfer_Complete( TRUE );      //if transfer complete fails, indicate to run recover I2C

					if( bTransferCompleted )
					{
						if( bDoStop )
						{
							Generate_Stop();
							bDoStop = FALSE;
						}
						//Get second byte
						temp = Get_Byte( FALSE, TRUE );      //Indicate that we do not want to wait for TransferComplete with a TRUE in second parameter
					}
					else
					{
						temp = -1;		//error, transfer not completed
					}
				}
				else
				{
					//Get second byte
					temp = Get_Byte( FALSE );
				}

				if( temp == -1 )		//error
				{
					ucChar = (unsigned char)(temp & 0xff);
					*uiBuffer = (((unsigned short)ucChar))&0x00ff;
					bStatus = FALSE;
					break;
				}
				else
				{
					ucChar = (unsigned char)temp;
					*uiBuffer += (((unsigned short)ucChar))&0x00ff;
				}
			}
			else if( iUseBuffer == 2 )		//addressing a one byte per register
			{
				//Start read clocks with a dummy read to get first read data
				if( bDoDummyRead )               //indicates we have not started the read clocks yet
				{
					if( nCount == 0 )
					{
						Dummy_Read( TRUE );       //if only reading one byte, thus the last byte, so indicate no TX ACK
					}
					else
					{
						Dummy_Read( FALSE );  	//not the last byte, reading more than one byte
					}

					bDoDummyRead = FALSE;
				}

				if( nCount == 1 )      //second to the last byte, must indicate that last byte has no TX ACK
				{
					//store only one byte
					temp = Get_Byte( TRUE );
				}
				else if( nCount == 0 )     //last byte to send, must do a stop first
				{
					bool bTransferCompleted = Is_Transfer_Complete( TRUE );    //if transfer complete fails, indicate to run recover I2C

					if( bTransferCompleted )
					{
						if( bDoStop )
						{
							Generate_Stop();
							bDoStop = FALSE;
						}
						//store only one byte
						temp = Get_Byte( FALSE, TRUE );      //Indicate that we do not want to wait for TransferComplete with a TRUE in second parameter
					}
					else
					{
						temp = -1;		//error, transfer not completed
					}
				}
				else
				{
					//store only one byte
					temp = Get_Byte( FALSE );
				}

				if( temp == -1 )		//error
				{
					ucChar = (unsigned char)(temp & 0xff);
					*uiBuffer = (((unsigned short)ucChar))&0x00ff;
					bStatus = FALSE;
					break;
				}
				else
				{
					ucChar = (unsigned char)temp;
					*uiBuffer = (((unsigned short)ucChar))&0x00ff;
				}
			}
			else		//error
			{
				bStatus = FALSE;
				break;
			}
			uiBuffer++;
			iCount++;
		}		//end while()
	}
	else if( iUseBuffer == 0 )			//iUseBuffer == 0, and using ucBuffer (char)
	{
		if( bStatus && (nCount > 0) )         //get the bytes
		{
			//Start read clocks with a dummy read to get first read data
			if( bDoDummyRead )               //indicates we have not started the read clocks yet
			{
				if( nCount == 1 )
				{
					Dummy_Read( TRUE );       //if only reading one byte, thus the last byte, so indicate no TX ACK
				}
				else
				{
					Dummy_Read( FALSE );  	//not the last byte, reading more than one byte
				}

				bDoDummyRead = FALSE;
			}

			for( i=0; (i < nCount) && bStatus; i++ )
			{
				if( i==(nCount-2) )
				{
					temp = Get_Byte( TRUE );		//If second to last byte, set to true which means no TX ACK for last byte
				}
				else if( i==(nCount-1) )           //If last byte, must do a stop first before read
				{
					bool bTransferCompleted = Is_Transfer_Complete( TRUE );    //if transfer complete fails, indicate to run recover I2C

					if( bTransferCompleted )
					{
						if( bDoStop )
						{
							Generate_Stop();
							bDoStop = FALSE;
						}
						//store only one byte
						temp = Get_Byte( FALSE, TRUE );      //Indicate that we do not want to wait for TransferComplete with a TRUE in second parameter
					}
					else
					{
						temp = -1;		//error, transfer not completed
					}
				}
				else
				{
					temp = Get_Byte( FALSE );		//false means send a TX ACK
				}

				if( temp == -1 )
				{
					ucBuffer[i] = (unsigned char)(temp & 0xff);
					bStatus = FALSE;
				}
				else
				{
					ucBuffer[i] = (unsigned char)temp;
				}
			}	//end for()
		}
	}

	if( bDoStop || !bStatus )
	{
		Generate_Stop();
	}

	//Release the mutex
	MT_ReleaseMutex(accessI2Cmtx);

	return bStatus;
}



//***************************************************************************************************************
//Encapsulated Privite-Common Read and Write I2C methods
#else
//Poled Versions of the I2C methods
//***************************************************************************************************************



// Routine Name: Send_Message_I2C
// Effects: This routine is a common method to send data to imager or psoc.
//				Sub address being sent is optional by setting the bool iSendSub parameter.
//				Sending data is optional by setting parameter nCount to zero and/or parameter ucBuffer/uiBuffer to NULL.
// Inputs: deviceAddress - I2C device address
//				subAddress - the register # in imager communication, offset to register in PSOC communication (length -one byte, char) .
//								(NOTE: a read from the PSOC resets the register offset to '0',  see dummyRead() )
//				uiSubAddress - the register # in imager communication, offset to register in PSOC communication  (two byte, short).
//          	ucBuffer - pointer to char data buffer (NULL means no data sent).
//          	uiBuffer - pointer to short data buffer (NULL means no data sent). Must chose one buffer or the other if data is being sent.
//				nCount - number of bytes of data to send ('0' means no data sent ).
//				iSendSub - selection for sending sub address (1 means send char sub address, 2 means send short sub address).
//				iUseBuffer - selection for using char buffer, '0' for char data buffer, '1' for using short data buffer, '2' for
//									using short data buffer with an address offset to switch register size writes (8 or 16 bit).
//				iOffsetCount - address offset were we switch from writing 8 bit registers to 16 bit registers
//				bSendStop - selection for sending stop at end of write, no stop needed if using repeat start in a read request next.
// Outputs: bool - true indicates success
// Packet Format:
//				PSOC - start, write_address, sub address, stop, start, write_address, send_data, stop
//				Imager- start, write_address, sub address, send_data, stop

bool I2C_Drvr::Send_Message_I2C( unsigned char deviceAddress, unsigned char subAddress, unsigned short uiSubAddress, const unsigned char * ucBuffer, const unsigned short * uiBuffer, unsigned int nCount, int iSendSub, int iUseBuffer, int iOffsetCount, bool bSendStop )
{
	int iCount;
	unsigned char ucChar;
	bool bStatus = TRUE;

	Generate_Start();

    if( !Send_Byte(deviceAddress, m_CheckForAck) )          //send the device address
	{
		bStatus = FALSE;

#ifdef I2C_TEST_DEBUG_ON
		xputstring("\r\n Failed Send address ");          //lmc   debug
		xputchar(deviceAddress);                          //lmc   debug
		xputstring(" \r\n ");                             //lmc   debug
#endif
	}

	if( bStatus && (iSendSub > 0) )               //if enabled, send the sub address
	{
		if( iSendSub == 1 )            //send one byte (char)
		{
            if( !Send_Byte(subAddress, m_CheckForAck) )
			{
				bStatus = FALSE;

				#ifdef I2C_TEST_DEBUG_ON
				xputstring(" failed send sub-address \r\n");         //lmc   debug
				#endif
			}
		}
		else if( iSendSub == 2 )		//send two bytes (short)
		{
			ucChar = (unsigned char)(((uiSubAddress)>>8)&0xff);

            if( !Send_Byte(ucChar, m_CheckForAck) )
			{
				bStatus = FALSE;

				#ifdef I2C_TEST_DEBUG_ON
				xputstring(" failed send most signif sub-address \r\n");         //lmc   debug
				#endif
			}

			ucChar = (unsigned char)((uiSubAddress)&0xff);

            if( !Send_Byte(ucChar, m_CheckForAck) )
			{
				bStatus = FALSE;

				#ifdef I2C_TEST_DEBUG_ON
				xputstring(" failed send least signif sub-address \r\n");         //lmc   debug
				#endif
			}
		}
	}

	if( bStatus && (nCount > 0) && (uiBuffer != NULL) && (iSendSub == 1) && (iUseBuffer > 0) )		//iUseBuffer == to 1 or 2, and using uiBuffer (short)
	{
		iCount = 0;

		while(nCount--)
		{
			if( (((subAddress + iCount) > iOffsetCount) && (iUseBuffer==2)) || (iUseBuffer==1) )    //iOffsetCount ==  to 0x7f for JADE
			{                                                                           //switches from one byte to two byte register writes
				ucChar = (unsigned char)(((*uiBuffer)>>8)&0xff);

                if( !Send_Byte(ucChar, m_CheckForAck) )
				{
					bStatus = FALSE;
					break;
				}

				ucChar = (unsigned char)((*uiBuffer)&0xff);

                if( !Send_Byte(ucChar, m_CheckForAck) )
				{
					bStatus = FALSE;
					break;
				}
			}
			else if( iUseBuffer == 2 )
			{
				ucChar = (unsigned char)(((*uiBuffer))&0xff);

                if( !Send_Byte(ucChar, m_CheckForAck) )
				{
					bStatus = FALSE;
					break;
				}
			}
			else		//error
			{
				bStatus = FALSE;
				break;
			}
			uiBuffer++;
			iCount++;
		}				//end while()
	}
	else if( iUseBuffer == 0 )			//iUseBuffer == 0, and using ucBuffer (char)
	{
		if( bStatus && (nCount > 0) && (ucBuffer != NULL) )         //send the bytes
		{
			for(iCount=0; iCount < nCount; iCount++)
			{
                if( !Send_Byte(ucBuffer[iCount], m_CheckForAck) )
				{
					bStatus = FALSE;

#ifdef I2C_TEST_DEBUG_ON
					xputstring(" failed to send bytes \r\n");          //lmc   debug
#endif
					break;
				}
			}		//end for()
		}
	}

	if( bSendStop )
	{
		Generate_Stop();
	}

	return bStatus;
}


// Routine Name: Receive_Message_I2C
// Effects: This routine is a common method to get data from imager or psoc.
// Inputs: deviceAddress - I2C device address to read from (up to user to add on  '1'  to address to change from write to read mode.
//				subAddress - the register # in imager communication, offset to register in PSOC communication, set by a previous write .
//              cSendCount - If not null, will send receive count to device.
//              ucBuffer - pointer to (char) data buffer .
//				uiBuffer - pointer to (short) data buffer . Must chose one buffer or the other for the read.
//				nCount - number of bytes of data to read .
//				iUseBuffer - selection for using char buffer, '0' for char data buffer, '1' for using short data buffer, '2' for
//									using short data buffer with an address offset to switch register size writes (8 or 16 bit).
//				iOffsetCount - address offset were we switch from reading 8 bit registers to 16 bit registers
//              iUseSendCount - 0 is do not send cSendCount, 1 is send it with ack required, 2 send it with no ack required.
//              bSendRepeatedStart - selection for sending repeated start at start of read, no stop sent is required in previous write to use this.
// Outputs: bool - true indicates success
// Packet Format:
//				PSOC - start, write_address, sub address, stop, start, read_address, get_data, stop
//				Imager- start, write_address, sub address, stop, start, read_address, get_data, stop
//							(NOTE: Read_Request == start, read_address )
//				JADE Imager- start, write_address, sub address, set_repeat_start, read_address, get_data, stop
//							(NOTE: Read_Request_w_Repeated_Start == set_repeat_start, read_address )

bool I2C_Drvr::Receive_Message_I2C( unsigned char deviceAddress, unsigned char subAddress, unsigned char cSendCount, unsigned char * ucBuffer, unsigned short * uiBuffer, unsigned int nCount, int iUseBuffer, int iOffsetCount, int iUseSendCount, bool bSendRepeatedStart )
{
	int iCount;
	unsigned char ucChar;
	int i;
	int temp;
	bool bStatus = TRUE;

	if( bSendRepeatedStart )
	{
		bStatus = Read_Request_w_Repeated_Start(deviceAddress);
	}
	else
	{
		bStatus = Read_Request(deviceAddress);
	}

    if( bStatus && (cSendCount != 0) && (iUseSendCount != 0) )
	{
        int iCheckForAck = 1;       //if iUseSendCount == 1, then indicate need to wiat for ack

        if(iUseSendCount != 1)      //if 2, do not wait for ack
        {
            iCheckForAck = 0;       //indicate not to wait for ack
        }

        if( !Send_Byte( cSendCount, iCheckForAck ) )           //send the device address
		{
			bStatus = FALSE;

			#ifdef I2C_TEST_DEBUG_ON
            xputstring("\r\n Failed Send count ");          //lmc   debug
            xputchar( cSendCount );                          //lmc   debug
			xputstring(" \r\n ");                             //lmc   debug
			#endif
		}
    }

	if( bStatus && (nCount > 0) && (iUseBuffer > 0) )		//iUseBuffer == to 1 or 2, and using uiBuffer (short)
	{
		iCount = 0;

		while (nCount--)
		{
			// everything is 2 bytes, the FALSE indicates the first byte can't be the last
			// FIXME:  do some exception handling for 16 bit versus 8 bit registers
			if( (((subAddress + iCount) > iOffsetCount) && (iUseBuffer==2)) || (iUseBuffer==1) )	//iOffsetCount ==  to 0x7f for JADE
			{                                                                                    //switches from one byte to two byte register writes
				//Get first byte
				temp = Get_Byte(FALSE);

				if( temp == -1 )		//error
				{
					ucChar = (unsigned char)(temp & 0xff);
					*uiBuffer = (((unsigned short)ucChar))&0x00ff;
					bStatus = FALSE;
					break;
				}
				else
				{
					ucChar = (unsigned char)temp;
					*uiBuffer = (((unsigned short)ucChar)<<8)&0xff00;
				}

				//Get second byte
				temp = Get_Byte( nCount == 0 );

				if( temp == -1 )		//error
				{
					ucChar = (unsigned char)(temp & 0xff);
					*uiBuffer = (((unsigned short)ucChar))&0x00ff;
					bStatus = FALSE;
					break;
				}
				else
				{
					ucChar = (unsigned char)temp;
					*uiBuffer += (((unsigned short)ucChar))&0x00ff;
				}
			}
			else if( iUseBuffer == 2 )
			{
				//store only one byte
				temp = Get_Byte( nCount == 0 );

				if( temp == -1 )		//error
				{
					ucChar = (unsigned char)(temp & 0xff);
					*uiBuffer = (((unsigned short)ucChar))&0x00ff;
					bStatus = FALSE;
					break;
				}
				else
				{
					ucChar = (unsigned char)temp;
					*uiBuffer = (((unsigned short)ucChar))&0x00ff;
				}
			}
			else		//error
			{
				bStatus = FALSE;
				break;
			}
			uiBuffer++;
			iCount++;
		}		//end while()
	}
	else if( iUseBuffer == 0 )			//iUseBuffer == 0, and using ucBuffer (char)
	{
		if( bStatus && (nCount > 0) )         //get the bytes
		{
			for( i=0; (i < nCount) && bStatus; i++ )
			{
				if( i==(nCount-1) )
				{
					temp = Get_Byte(TRUE);		//true means no TX ACK
				}
				else
				{
					temp = Get_Byte(FALSE);		//false means send a TX ACK
				}

				if( temp == -1 )
				{
					ucBuffer[i] = (unsigned char)(temp & 0xff);
					bStatus = FALSE;
				}
				else
				{
					ucBuffer[i] = (unsigned char)temp;
				}
			}	//end for()
		}
	}

	Generate_Stop();
	return bStatus;
}


//***************************************************************************************************************
//End Encapsulated Privite-Common Read and Write I2C methods
#endif
//End of the poled versions of the I2C methods
//***************************************************************************************************************



//****************************************************************************************************
// Public Generic Methods to Write I2C
//****************************************************************************************************


// Routine Name: Send_Data_W_Short_W_Dual_Reg_Size_I2C
// Effects: This routine is a common method to send data to imager or psoc.
//				Designed for the use mainly by JADE sensor.
//				Sub address being sent is required for this method.
//				Sending data is optional by setting parameter nCount to zero and/or parameter uiBuffer to NULL.
// Inputs: deviceAddress - I2C device address
//				subAddress - the register # in imager communication, offset to register in PSOC communication (length -one byte, char) .
//								(NOTE: a read from the PSOC resets the register offset to '0',  see dummyRead() )
//          	uiBuffer - pointer to short data buffer (NULL means no data sent).
//				nCount - number of bytes of data to send ('0' means no data sent ).
//				iOffsetCount - offset were we switch from writing 8 bit registers to 16 bit registers
// Outputs: bool - true indicates success
// Packet Format:
//				PSOC - start, write_address, sub address, stop, start, write_address, send_data, stop
//				Imager- start, write_address, sub address, send_data, stop

bool I2C_Drvr::Send_Data_W_Short_W_Dual_Reg_Size_I2C( unsigned char deviceAddress, unsigned char subAddress, const unsigned short * uiBuffer, unsigned int nCount, int iOffsetCount )
{
	bool bStatus = TRUE;
	const unsigned char * ucBuffer = NULL;
	unsigned short uiSubAddress = NULL;
	int iSendSub = 1;                   //send sub address as a char
	int iUseBuffer = 2;
	bool bSendStop = TRUE;

	bStatus = Send_Message_I2C( deviceAddress, subAddress, uiSubAddress, ucBuffer, uiBuffer, nCount, iSendSub, iUseBuffer, iOffsetCount, bSendStop );
	return bStatus;
}


// Routine Name: Send_Data_W_Short_W_Single_Reg_Size_I2C
// Effects: This routine is a common method to send data to imager or psoc.
//				Designed for the use mainly by MT9V sensor.
//				Sub address being sent is required for this method.
//				Sending data is optional by setting parameter nCount to zero and/or parameter uiBuffer to NULL.
// Inputs: deviceAddress - I2C device address
//				subAddress - the register # in imager communication, offset to register in PSOC communication .
//								(NOTE: a read from the PSOC resets the register offset to '0',  see dummyRead() )
//          uiBuffer - pointer to short data buffer (NULL means no data sent).
//				nCount - number of bytes of data to send ('0' means no data sent ).
// Outputs: bool - true indicates success
// Packet Format:
//				PSOC - start, write_address, sub address, stop, start, write_address, send_data, stop
//				Imager- start, write_address, sub address, send_data, stop

bool I2C_Drvr::Send_Data_W_Short_W_Single_Reg_Size_I2C( unsigned char deviceAddress, unsigned char subAddress, const unsigned short * uiBuffer, unsigned int nCount )
{
	bool bStatus = TRUE;
	const unsigned char * ucBuffer = NULL;
	unsigned short uiSubAddress = NULL;
	int iSendSub = 1;                   //send sub address as a char
	int iUseBuffer = 1;
	int iOffsetCount = 0x00;
	bool bSendStop = TRUE;

	bStatus = Send_Message_I2C( deviceAddress, subAddress, uiSubAddress, ucBuffer, uiBuffer, nCount, iSendSub, iUseBuffer, iOffsetCount, bSendStop );
	return bStatus;
}


// Routine Name: Send_Data_As_Char_I2C
// Effects: This routine is a common method to send data to imager or psoc.
//				Sub address being sent is optional by setting the bool iSendSub parameter
//						(1 means send char sub address, 2 means send short sub address).
//				Sending data is optional by setting parameter nCount to zero and/or parameter ucBuffer to NULL.
// Inputs: deviceAddress - I2C device address
//				subAddress - the register # in imager communication, offset to register in PSOC communication (length -one byte, char) .
//								(NOTE: a read from the PSOC resets the register offset to '0',  see dummyRead() )
//				uiSubAddress - the register # in imager communication, offset to register in PSOC communication  (two byte, short).
//          	ucBuffer - pointer to char data buffer (NULL means no data sent).
//				nCount - number of bytes of data to send ('0' means no data sent ).
//				iSendSub - selection for sending sub address (1 means send char sub address, 2 means send short sub address).
//              iCheckForAck - selection for checking for ack on transmit (1 means check it, the I2C spec, 0 means do not check), 1 is default.
// Outputs: bool - true indicates success
// Packet Format:
//				PSOC - start, write_address, sub address, stop, start, write_address, send_data, stop
//				Imager- start, write_address, sub address, send_data, stop
//
//                          (NOTE: If iCheckForAck == 0, then no check for 9th bit slave's ACK of a byte transmit, for omnivision 5650)

bool I2C_Drvr::Send_Data_As_Char_I2C( unsigned char deviceAddress, unsigned char subAddress, unsigned short uiSubAddress, const unsigned char * ucBuffer,  unsigned int nCount, int iSendSub, int iCheckForAck )
{
	bool bStatus = TRUE;
	const unsigned short * uiBuffer = NULL;
	int iUseBuffer = 0;             //use char buffer
	int iOffsetCount = 0x00;
	bool bSendStop = TRUE;

    m_CheckForAck = iCheckForAck;                          //do not check for ack on transmit if set to 0, default value is 1

	bStatus = Send_Message_I2C( deviceAddress, subAddress, uiSubAddress, ucBuffer, uiBuffer, nCount, iSendSub, iUseBuffer, iOffsetCount, bSendStop );

    m_CheckForAck = 1;                          //return to check for ack on transmit, because this is part of I2C spec

    return bStatus;
}


// Routine Name: Send_Addr_And_Sub_I2C
// Effects: This routine is a common method to send data to imager or psoc.
//				Sub address being sent is optional by setting the bool bSendSub parameter.
//				Stop being sent is optional by setting the bool bSendStop parameter.
// Inputs: deviceAddress - I2C device address
//				subAddress - the register # in imager communication, offset to register in PSOC communication (length -one byte, char) .
//								(NOTE: a read from the PSOC resets the register offset to '0',  see dummyRead() )
//				uiSubAddress - the register # in imager communication, offset to register in PSOC communication  (two byte, short).
//				iSendSub - selection for sending sub address (1 means send char sub address, 2 means send short sub address).
//				bSendStop - selection for sending stop at end of write, no stop needed if using repeat start in next read request .
// Outputs: bool - true indicates success
// Packet Format:
//				Imager- start, write_address, sub address, stop

bool I2C_Drvr::Send_Addr_And_Sub_I2C( unsigned char deviceAddress, unsigned char subAddress, unsigned short uiSubAddress, int iSendSub, bool bSendStop )
{
	bool bStatus = TRUE;
	const unsigned short * uiBuffer = NULL;      //no data to send
	int iUseBuffer = 0;                          //use char buffer
	int iOffsetCount = 0x00;
	const unsigned char * ucBuffer = NULL;       //no data to send
	unsigned int nCount = 0;                     //no data if 0

	bStatus = Send_Message_I2C( deviceAddress, subAddress, uiSubAddress, ucBuffer, uiBuffer, nCount, iSendSub, iUseBuffer, iOffsetCount, bSendStop );
	return bStatus;
}


//****************************************************************************************************
// Public Generic Methods to Read I2C
//****************************************************************************************************


// Routine Name: Receive_Data_W_Short_W_Dual_Reg_Size_I2C
// Effects: This routine is a common method to get data from imager or psoc.
//				This is designed to be used with the JADE for repeated starts.
//				Previous write that begins this must not have a stop.
// Inputs: deviceAddress - I2C device address to read from (up to user to add on  '1'  to address to change from write to read mode.
//				subAddress - the register # in imager communication, offset to register in PSOC communication, set by a previous write .
//				uiBuffer - pointer to (short) data buffer .
//				nCount - number of words of data to read .
//				iOffsetCount - address offset were we switch from reading 8 bit registers to 16 bit registers
// Outputs: bool - true indicates success
// Packet Format:
//				PSOC - start, write_address, sub address, stop, start, read_address, get_data, stop
//				Imager- start, write_address, sub address, stop, start, read_address, get_data, stop
//							(NOTE: Read_Request == start, read_address )
//				JADE Imager- start, write_address, sub address, set_repeat_start, read_address, get_data, stop
//							(NOTE: Read_Request_w_Repeated_Start == set_repeat_start, read_address )

bool I2C_Drvr::Receive_Data_W_Short_W_Dual_Reg_Size_I2C( unsigned char deviceAddress, unsigned char subAddress, unsigned short * uiBuffer, unsigned int nCount, int iOffsetCount )
{
	bool bStatus = TRUE;
	unsigned char * ucBuffer = NULL;
	int iUseBuffer = 2;	//use the uiBuffer (short), also using a "iOffsetCount + subAddress", to judge when to switch between 8 and 16 bit registers in imager.
	bool bSendRepeatedStart = TRUE;	//Do a Repeated start, this is selection for sending repeated start at start of read, no stop sent is required in previous write to use this.
    unsigned char cSendCount = 0;   //do not send a receive count
    int iUseSendCount = 0;         //use CSendCount is off

    bStatus = Receive_Message_I2C( deviceAddress, subAddress, cSendCount, ucBuffer, uiBuffer, nCount, iUseBuffer, iOffsetCount, iUseSendCount, bSendRepeatedStart );
	return bStatus;
}


// Routine Name: Receive_Data_Bytes_W_Char_Buffer_I2C
// Effects: This routine is a common method to get data from imager or psoc.
//				This is designed to be used with standard stop/start method and also needs a character buffer for return.
//				Previous write that begins this must have a stop.
// Inputs: deviceAddress - I2C device address to read from (up to user to add on  '1'  to address to change from write to read mode.
//				subAddress - the register # in imager communication, offset to register in PSOC communication, set by a previous write .
//				ucBuffer - pointer to (char) data buffer .
//				nCount - number of bytes of data to read .
// Outputs: bool - true indicates success
// Packet Format:
//				PSOC - start, write_address, sub address, stop, start, read_address, get_data, stop
//				Imager- start, write_address, sub address, stop, start, read_address, get_data, stop
//							(NOTE: Read_Request == start, read_address )
//				JADE Imager- start, write_address, sub address, set_repeat_start, read_address, get_data, stop
//							(NOTE: Read_Request_w_Repeated_Start == set_repeat_start, read_address )

bool I2C_Drvr::Receive_Data_Bytes_W_Char_Buffer_I2C( unsigned char deviceAddress, unsigned char * ucBuffer, unsigned int nCount )
{
	bool bStatus = TRUE;
	unsigned char subAddress = 0;
	unsigned short * uiBuffer = NULL;
	int iUseBuffer = 0;	//use the ucBuffer (char)
	bool bSendRepeatedStart = FALSE;	//Do not do a Repeated start, this is selection for not sending repeated start at start of read, a stop sent is required in previous write to use this.
	int iOffsetCount = 0x00;
    unsigned char cSendCount = 0;   //do not send a receive count
    int iUseSendCount = 0;         //use CSendCount is off

    bStatus = Receive_Message_I2C( deviceAddress, subAddress, cSendCount, ucBuffer, uiBuffer, nCount, iUseBuffer, iOffsetCount, iUseSendCount, bSendRepeatedStart );
	return bStatus;
}


#if I2C_INTERUPTS

//****************************************************************************************************
// Public Generic Methods to do Write Setup and then Read I2C
//****************************************************************************************************


// Routine Name: Write_Setup_Then_Receive_Data_W_Short_W_Dual_Reg_Size_I2C
// Effects: This routine is a common method to get data from imager or psoc.
//				This is designed to be used with the JADE for repeated starts.
//				Also sends a write first that sets up this read, and write does not have a stop.
// Inputs:
//				deviceAddress - I2C device address to read from (up to user to add on  '1'  to address to change from write to read mode.
//									NOTE: Write setup used below will subtract '1' from deviceAddress passed here (so user must add '1' !!!)
//				subAddress - the register # in imager communication, offset to register in PSOC communication, set by a previous write .
//				uiBuffer - pointer to (short) data buffer .
//				nCount - number of words of data to read .
//				iOffsetCount - address offset were we switch from reading 8 bit registers to 16 bit registers
// Outputs: bool - true indicates success
// Packet Format:
//				JADE Imager- start, write_address, sub address, set_repeat_start, read_address, get_data, stop
//							(NOTE: Read_Request_w_Repeated_Start == set_repeat_start, read_address )

bool I2C_Drvr::Write_Setup_Then_Receive_Data_W_Short_W_Dual_Reg_Size_I2C( unsigned char deviceAddress, unsigned char subAddress, unsigned short * uiBuffer, unsigned int nCount, int iOffsetCount )
{
	bool bStatus = TRUE;

	//Check for MUTEX
	MT_WaitMutex(accessI2Cmtx,INFINITE);

	unsigned short uiSubAddress = NULL;
	const unsigned short * uiBufferWr = NULL;
	int iUseBufferWr = 0;
	int iOffsetCountWr = 0x00;
	const unsigned char * ucBufferWr = NULL;
	unsigned int nCountWr = 0;
	int iSendSub = 1;
	bool bSendStop = FALSE;

	bStatus = Send_Message_I2C( (deviceAddress-1), subAddress, uiSubAddress, ucBufferWr, uiBufferWr, nCountWr, iSendSub, iUseBufferWr, iOffsetCountWr, bSendStop );

	if( bStatus )
	{
		unsigned char * ucBuffer = NULL;
		int iUseBuffer = 2;	//use the uiBuffer (short), also using a "iOffsetCount + subAddress", to judge when to switch between 8 and 16 bit registers in imager.
		bool bSendRepeatedStart = TRUE;	//Do a Repeated start, this is selection for sending repeated start at start of read, no stop sent is required in previous write to use this.
        unsigned char cSendCount = 0;   //do not send a receive count
        int iUseSendCount = 0;         //use CSendCount is off

        bStatus = Receive_Message_I2C( deviceAddress, subAddress, cSendCount, ucBuffer, uiBuffer, nCount, iUseBuffer, iOffsetCount, iUseSendCount, bSendRepeatedStart );
	}

	//Release the mutex
	MT_ReleaseMutex(accessI2Cmtx);

	return bStatus;
}



// Routine Name: Write_Setup_Then_Receive_Data_W_Char_I2C
// Effects: This routine is a common method to get data from imager or psoc.
//				This is designed to be used with the JADE for repeated starts.
//				Also sends a write first that sets up this read, and write does not have a stop.
// Inputs:
//				deviceAddress - I2C device address to read from (up to user to add on  '1'  to address to change from write to read mode.
//									NOTE: Write setup used below will subtract '1' from deviceAddress passed here (so user must add '1' !!!)
//				subAddress - the register # in imager communication, offset to register in PSOC communication, set by a previous write (length -one byte, char) .
//				uiSubAddress - same as above, except length is two byte, a short.
//				ucBuffer - pointer to (char) data buffer .
//				nCount - number of words of data to read .
//				iSendSub - selection for sending sub address (1 means send char sub address, 2 means send short sub address).
//              iCheckForAck - selection for checking for ack on transmit (1 means check it, the I2C spec, 0 means do not check), 1 is default.
// Outputs: bool - true indicates success
// Packet Format:
//              Omni Vision Imager- start, write_address, sub address, set_repeat_start, read_address, get_data, stop
//                          (NOTE: Read_Request_w_Repeated_Start == set_repeat_start, read_address )
//
//							(NOTE: iSendSub must be 2 for Omni Vision to send short length subaddress,
//								a 0 (do not send subaddress) for iSend Sub can not be used here!)
//
//                          (NOTE2: If iCheckForAck == 0, then no check for 9th bit slave's ACK of a byte transmit, for omnivision 5650)



bool I2C_Drvr::Write_Setup_Then_Receive_Data_W_Char_I2C( unsigned char deviceAddress, unsigned char subAddress, unsigned short uiSubAddress, unsigned char * ucBuffer, unsigned int nCount, int iSendSub, int iCheckForAck )
{
	bool bStatus = TRUE;

	//Check for MUTEX
	MT_WaitMutex(accessI2Cmtx,INFINITE);

	const unsigned short * uiBufferWr = NULL;    //no data to send
	int iUseBufferWr = 0;                        //use char buffer
	int iOffsetCountWr = 0x00;
	const unsigned char * ucBufferWr = NULL;    //no write data to send
	unsigned int nCountWr = 0;	                //no data for write if 0
    bool bSendStop = FALSE;                     //we are going to do a restart next

    m_CheckForAck = iCheckForAck;                          //do not check for ack on transmit if set to 0, default value is 1

	bStatus = Send_Message_I2C( (deviceAddress-1), subAddress, uiSubAddress, ucBufferWr, uiBufferWr, nCountWr, iSendSub, iUseBufferWr, iOffsetCountWr, bSendStop );

	if( bStatus )
	{
		unsigned short * uiBuffer = NULL;
		int iOffsetCount = 0x00;
		int iUseBuffer = 0;	//use the ucBuffer (char)
        bool bSendRepeatedStart = TRUE; //Do a Repeated start, this is selection for sending repeated start at start of read, no stop sent is required in previous write to use this.
        unsigned char cSendCount = 0;   //do not send a receive count
        int iUseSendCount = 0;         //use CSendCount is off

        subAddress = 0;     //not needed here in read, since we do not send it and we are not checking for register offset (iOffsetCount).

        bStatus = Receive_Message_I2C( deviceAddress, subAddress, cSendCount, ucBuffer, uiBuffer, nCount, iUseBuffer, iOffsetCount, iUseSendCount, bSendRepeatedStart );
	}

    m_CheckForAck = 1;                          //return to check for ack on transmit, because this is part of I2C spec

	//Release the mutex
	MT_ReleaseMutex(accessI2Cmtx);

	return bStatus;
}



// Routine Name: Write_Setup_Then_Receive_Data_W_Char_I2C
// Effects: This routine is a common method to get data from imager or psoc.
//				This is designed to be used with the JADE for repeated starts.
//				Also sends a write first that sets up this read, and write does not have a stop.
// Inputs:
//				deviceAddress - I2C device address to read from (up to user to add on  '1'  to address to change from write to read mode.
//									NOTE: Write setup used below will subtract '1' from deviceAddress passed here (so user must add '1' !!!)
//				subAddress - the register # in imager communication, offset to register in PSOC communication, set by a previous write (length -one byte, char) .
//				uiSubAddress - same as above, except length is two byte, a short.
//              cSendCount - If not null, will send receive count to device.
//				ucBuffer - pointer to (char) data buffer .
//				nCount - number of words of data to read .
//				iSendSub - selection for sending sub address (1 means send char sub address, 2 means send short sub address).
//              iUseSendCount - 0 is do not send cSendCount, 1 is send it with ack required, 2 send it with no ack required.
// Outputs: bool - true indicates success
// Packet Format:
//              - start, write_address, sub address, set_repeat_start, read_address, send count, get_data, stop
//                          (NOTE: Read_Request_w_Repeated_Start == set_repeat_start, read_address )
//
//                          (NOTE: iSendSub must be 2 to send short length subaddress,
//								a 0 (do not send subaddress) for iSend Sub can not be used here!)
//
//                          (NOTE: for spread spectrum, want to use iUseSendCount of 2, so no ack required

bool I2C_Drvr::Write_Setup_Then_Receive_Data_W_Char_I2C_Plus_Send_Count( unsigned char deviceAddress, unsigned char subAddress, unsigned short uiSubAddress, unsigned char cSendCount, unsigned char * ucBuffer, unsigned int nCount, int iSendSub, int iUseSendCount )
{
	bool bStatus = TRUE;

	//Check for MUTEX
	MT_WaitMutex(accessI2Cmtx,INFINITE);

	const unsigned short * uiBufferWr = NULL;    //no data to send
	int iUseBufferWr = 0;                        //use char buffer
	int iOffsetCountWr = 0x00;
	const unsigned char * ucBufferWr = NULL;    //no write data to send
	unsigned int nCountWr = 0;	                //no data for write if 0
    bool bSendStop = FALSE;                     //we are going to do a restart next

	bStatus = Send_Message_I2C( (deviceAddress-1), subAddress, uiSubAddress, ucBufferWr, uiBufferWr, nCountWr, iSendSub, iUseBufferWr, iOffsetCountWr, bSendStop );

	if( bStatus )
	{
		unsigned short * uiBuffer = NULL;
		int iOffsetCount = 0x00;
		int iUseBuffer = 0;	//use the ucBuffer (char)
        bool bSendRepeatedStart = TRUE; //Do a Repeated start, this is selection for sending repeated start at start of read, no stop sent is required in previous write to use this.

        subAddress = 0;     //not needed here in read, since we do not send it and we are not checking for register offset (iOffsetCount).

        bStatus = Receive_Message_I2C( deviceAddress, subAddress, cSendCount, ucBuffer, uiBuffer, nCount, iUseBuffer, iOffsetCount, iUseSendCount, bSendRepeatedStart );
	}

	//Release the mutex
	MT_ReleaseMutex(accessI2Cmtx);

	return bStatus;
}

#endif



