/* ==============================================================================
	Matrix I2C Driver Functions Header File
   ==============================================================================

   $RCSfile: Gen6FoundationApp/FoundationApp/Peripheral_Interfaces/i2c_drvr.h $
   $Revision: 1.7 $
   $Date: 2010/08/05 07:23:33EDT $

   These routines provide access to the I2C bus functions.

   ==============================================================================

   ============================================================================== */


/////////////////////////////////////////////////////////////////////////////

#ifndef I2C_DRVR_H
#define I2C_DRVR_H



#define I2C_INTERUPTS	1                //define to one for interrupt driven code, 0 for old polling mode code



// I2C object class definition
class I2C_Drvr
{
public:

	static I2C_Drvr * Create_I2C_Drvr(void);   //class factory
	static void Destroy_I2C_Drvr(void);


private:
	I2C_Drvr(void);          // constructor, hide it since we use a class factory

	~I2C_Drvr(void);                   // destructor


private:


	// Routine Name: RecoverIIC
	// Effects: This routine takes over SCLK , toggles it, then sets up the I2C port for 2D imager
	//			The intent is to re-start I2C communication when imager communication has failed
	// Inputs: none
	// Outputs: none
	void RecoverIIC(unsigned int line=0);


	// Routine Name: InitI2C
	// Effects: This routine sets up the I2C port for interupts
	// Inputs: none
	// Outputs: none
	void InitI2C();


	// Routine Name: UnInitI2C
	// Effects: This routine unsets the I2C port for interupts
	// Inputs: none
	// Outputs: none
	void UnInitI2C();


	// Routine Name: ImagerSetup
	// Effects: This routine sets up the I2C port for 2D imager
	// Inputs: none
	// Outputs: none
	void ImagerSetup(void);                       	/* Initialization routines for I2C */


	// Routine Name: Read_Request
	// Effects: This routine does a start, sends read address, and a dummy read, to set up for doing a byte read
	// Inputs: slave read address
	// Outputs: bool for pass/fail
	bool Read_Request(unsigned char);	/* Sends a Read request to the specified slave address */


	// Routine Name: Read_Request_w_Repeated_Start
	// Effects: This routine does a repeat start, sends read address, and a dummy read, to set up for doing another byte read without a stop
	// Inputs: slave read address
	// Outputs: bool for pass/fail
	bool Read_Request_w_Repeated_Start(unsigned char);


#if I2C_INTERUPTS

	// Routine Name: Is_Transfer_Complete
	// Effects: This routine checks to see if transfer is complete
	// Inputs: bool bRunRecover to run recovery on failure, defaulted false.
	// Outputs: True if transfer complete, false if condition fails
	bool Is_Transfer_Complete(  bool bRunRecover = false );


	// Routine Name: Dummy_Read
	// Effects: This routine sets up the first read transaction. This generates the clocks by doing
	//          a "dummy read" , which starts the read clocks.
	// Inputs: set true if only receiving one byte, false sends TX ACK for every byte received,
	//				last byte is not ACK'ed so input should be set to true
	// Outputs: True if slave responded AND data was read, false if condition fails
	bool Dummy_Read( bool bLastByte );


	// Routine Name: Get_Byte
	// Effects: This routine gets a byte from the I2C port
	// Inputs: bLastByte indicates last byte, bSkipTransferComplete indicates this is a dummy read (no real data available yet), defaulted false.
	// Outputs: byte value, negative if none available
	int Get_Byte( bool bLastByte, bool bSkipTransferComplete = false );

#else


	// Routine Name: Get_Byte
	// Effects: This routine gets a byte from the I2C port
	// Inputs: bLastByte indicates last byte
	// Outputs: byte value, negative if none available
	int  Get_Byte(bool bLastByte);

#endif


// Routine Name: Send_Byte
// Effects: This routine sends a byte out the I2C port
// Inputs:  Byte to send
// Outputs: True if ACK received, false otherwise
    bool Send_Byte( unsigned char ucByteToSend, int iCheckForAck = 1 );


// Routine Name: Generate_Start
// Effects: This routine generates I2C start condition
// Inputs: none
// Outputs: bool as pass/fail
	bool Generate_Start(void);


// Routine Name: Generate_Repeated_Start
// Effects: This routine generates I2C repeat start condition
// Inputs: none
// Outputs: bool as pass/fail
	bool Generate_Repeated_Start(void);


// Routine Name: Generate_Stop
// Effects: This routine generates I2C stop condition
// Inputs: none
// Outputs: none
	void Generate_Stop(void);


// Routine Name: Send_Message_I2C
// Effects: This routine sends a set # of bytes out the I2C port
// Inputs:  device address, sub address, two types of pointers to data to send, data count, is sub address needed,
// 								type of data buffer use, offset for register size change, is stop needed.
// Outputs: True if ACK received, false otherwise
   bool Send_Message_I2C( unsigned char deviceAddress, unsigned char subAddress, unsigned short uiSubAddress, const unsigned char * ucBuffer, const unsigned short * uiBuffer, unsigned int nCount, int iSendSub, int iUseBuffer, int iOffsetCount, bool bSendStop );


// Routine Name: Receive_Message_I2C
// Effects: This routine gets a set # of bytes from the I2C port
// Inputs:  device address, sub address, two types of pointers to Bytes to get, data count, type of data buffer use,
//									offset for register size change, is repeated start needed.
// Outputs: True if ACK received, false otherwise
    bool Receive_Message_I2C( unsigned char deviceAddress, unsigned char subAddress, unsigned char cSendCount, unsigned char * ucBuffer, unsigned short * uiBuffer, unsigned int nCount, int iUseBuffer, int iOffsetCount, int iUseSendCount, bool bSendRepeatedStart );


public:


// Routine Name: Send_Data_W_Short_W_Dual_Reg_Size_I2C
// Effects: This routine sends a set # of bytes out the I2C port.
//				Mostly used by JADE sensor.
// Inputs:  device address, sub address, pointer to (short) data to send, data count, offset for register size change.
// Outputs: True if ACK received, false otherwise
	bool Send_Data_W_Short_W_Dual_Reg_Size_I2C( unsigned char deviceAddress, unsigned char subAddress, const unsigned short * uiBuffer, unsigned int nCount, int iOffsetCount );


// Routine Name: Send_Data_W_Short_W_Single_Reg_Size_I2C
// Effects: This routine sends a set # of bytes out the I2C port.
//				Mostly used by JADE sensor.
// Inputs:  device address, sub address, pointer to (short) data to send, data count.
// Outputs: True if ACK received, false otherwise
	bool Send_Data_W_Short_W_Single_Reg_Size_I2C( unsigned char deviceAddress, unsigned char subAddress, const unsigned short * uiBuffer, unsigned int nCount );


// Routine Name: Send_Data_As_Char_I2C
// Effects: This routine sends a set # of bytes out the I2C port
// Inputs:  device address, sub address, pointers to (char) data to send, data count, is sub address needed.
// Outputs: True if ACK received, false otherwise
    bool Send_Data_As_Char_I2C( unsigned char deviceAddress, unsigned char subAddress, unsigned short uiSubAddress, const unsigned char * ucBuffer,  unsigned int nCount, int iSendSub, int iCheckForAck = 1 );


// Routine Name: Send_Addr_And_Sub_I2C
// Effects: This routine sends address and sub address out the I2C port, sub addr and stop optional.
// Inputs:  device address, sub address, is sub address needed, is stop needed.
// Outputs: True if ACK received, false otherwise
	bool Send_Addr_And_Sub_I2C( unsigned char deviceAddress, unsigned char subAddress, unsigned short uiSubAddress, int iSendSub, bool bSendStop );


// Routine Name: Receive_Data_W_Short_W_Dual_Reg_Size_I2C
// Effects: This routine gets a set # of short words from the I2C port
// Inputs:  device address, sub address, (short) data pointer to short word to get, data count,
//									offset for register size change.
// Outputs: True if ACK received, false otherwise
	bool Receive_Data_W_Short_W_Dual_Reg_Size_I2C( unsigned char deviceAddress, unsigned char subAddress, unsigned short * uiBuffer, unsigned int nCount, int iOffsetCount );


// Routine Name: Receive_Data_Bytes_W_Char_Buffer_I2C
// Effects: This routine gets a set # of bytes from the I2C port
// Inputs:  device address, sub address, (char) data pointer to Bytes to get, data count.
// Outputs: True if ACK received, false otherwise
	bool Receive_Data_Bytes_W_Char_Buffer_I2C( unsigned char deviceAddress, unsigned char * ucBuffer, unsigned int nCount );


#if I2C_INTERUPTS

// Routine Name: Write_Setup_Then_Receive_Data_W_Short_W_Dual_Reg_Size_I2C
// Effects: This routine gets a set # of short words from the I2C port.
//				Send out write first as set up for read and repeat start.
// Inputs:  device address, sub address, (short) data pointer to short word to get, data count,
//									offset for register size change.
// Outputs: True if ACK received, false otherwise
	bool Write_Setup_Then_Receive_Data_W_Short_W_Dual_Reg_Size_I2C( unsigned char deviceAddress, unsigned char subAddress, unsigned short * uiBuffer, unsigned int nCount, int iOffsetCount );



// Routine Name: I2C_Drvr::Write_Setup_Then_Receive_Data_W_Char_I2C
// Effects: This routine gets a set # of char length data from the I2C port.
//				Send out write first as set up for read , with stop.
// Inputs:  device address, sub address, uiSubAddress, (char) data pointer to char length data to get, data count,
//                                  and sendsub, which sets the size of subaddress (1 is char size, 2 is short size), plus check for ack (on transmit).
// Outputs: True if ACK received, false otherwise
    bool Write_Setup_Then_Receive_Data_W_Char_I2C( unsigned char deviceAddress, unsigned char subAddress, unsigned short uiSubAddress, unsigned char * ucBuffer, unsigned int nCount, int iSendSub, int iCheckForAck = 1 );


 // Routine Name: I2C_Drvr::Write_Setup_Then_Receive_Data_W_Char_I2C
// Effects: This routine gets a set # of char length data from the I2C port.
//				Send out write first as set up for read , with stop.
// Inputs:  device address, sub address, uiSubAddress, sendCount for receive count to send to device, (char) data pointer to char length data to get, data count,
//                                  and sendsub, which sets the size of subaddress (1 is char size, 2 is short size), plus use sendCount for sending receive size.
// Outputs: True if ACK received, false otherwise
    bool Write_Setup_Then_Receive_Data_W_Char_I2C_Plus_Send_Count( unsigned char deviceAddress, unsigned char subAddress, unsigned short uiSubAddress, unsigned char cSendCount, unsigned char * ucBuffer, unsigned int nCount, int iSendSub, int iUseSendCount );

#endif


private:

	int addr;         // used to track address of the device being communicated to in case of failure

    int m_CheckForAck;    //used to indicate if 9th bit on transmit is checked for the ack state from slave (==0)

};

//Global pointer to class factory instance
extern I2C_Drvr *g_pI2C_Drvr;
#define The_I2C_Drvr	(*g_pI2C_Drvr)

#endif

/////////////////////////////////////////////////////////////////////////////


