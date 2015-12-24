/*=================================================================================
  class:

//=================================================================================
   $RCSfile: interfaces/iocontrol.h $
   $Date: 2011/08/24 11:02:14EDT $
   $Revision: 1.32 $


//=================================================================================*/
//! \file

#ifndef IO_CONTROL_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define IO_CONTROL_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

//! Commands for the interfaces drivers IoControl function.
enum IoControl_t
{
	IOCTRL_NOOP=0,										//!< More or less a dummy value

	////////////////////////////////////////////////////////////////////////////
	// Beeper control
	IOCTRL_SHALL_I_BEEP_ON_POWERUP,				//!< The framework asks the driver whether it should beep
	IOCTRL_SHALL_I_BEEP_ON_GOOD_READ,         //!< The framework asks the driver whether it should beep
	IOCTRL_SHALL_I_BEEP_ON_MENU,              //!< The framework asks the driver whether it should beep
	IOCTRL_SET_DEFAULT_BEEP_LEVEL,			//!< Set the default beeper level for interfaces that enable/disable beeper

	////////////////////////////////////////////////////////////////////////////
	// Data flow and close preparation
	IOCTRL_IS_DONE,									//!< All data sent?

	IOCTRL_PAUSE_THREAD,								//!< Pauses the interface thread
	IOCTRL_UNPAUSE_THREAD,							//!< Runs the interface thread again

	IOCTRL_IS_CLOSE_PROHIBITED,					//!< used for 4683
	IOCTRL_PREPARE_FORCED_CLOSE,					//<  used for 4683 and HHLC

	////////////////////////////////////////////////////////////////////////////
	// Power managment
	//! Returns IOCRET_YES if allowed, else IOCRET_NO
	IOCTRL_IS_LOW_POWER_ALLOWED,
	//! Returns IOCRET_PASS when prepared, else IOCRET_FAIL if not possible
	IOCTRL_PREPARE_FOR_LOW_POWER_STATE,
	//! The power managment tells the driver to remove any special conditions.
	IOCTRL_RETURN_TO_NORMAL_POWER_STATE,
	IOCTRL_IS_REMOTE_WAKEUP_EN,

	////////////////////////////////////////////////////////////////////////////
	// Data types
	IOCTRL_GET_SUPPORTED_DATATYPES,				//!< Return a bitmask (IoDataType_t) to the caller supplied pointer
	IOCTRL_GET_PREFERED_DATATYPES,				//!< Return a bitmask (IoDataType_t) to the caller supplied pointer

	////////////////////////////////////////////////////////////////////////////
	// Error handling
	// Returns the last error (MatrixError_t) to the caller supplied pointer
	// The lasterror variable in the driver gets cleared after sending the error.
	// Note: Drivers can have asynchronos error events, so they can not be returned
	// 	by the Write() function.

	IOCTRL_GET_LAST_ERROR,							//!< Returns the last error (MatrixError_t) to the caller supplied pointer
	IOCTRL_GET_ERROR_MSG,							//!< In the case of HHLC return a requested error msg


	////////////////////////////////////////////////////////////////////////////
	// Misc.
	// returns IOCRET_YES or IOCRET_NO
	IOCTRL_IS_BAUDRATE_AVAILABLE,					//!< is that baudrate available by this driver?

	// returns IOCRET_YES or IOCRET_NO
	IOCTRL_IS_INTERPRET_DATA,						//!< Does the driver interpret the data? (yes for keyboards)
	IOCTRL_TRIGGERED,									//!< We just went into triggered state (used for HHLC)
	IOCTRL_UNTRIGGERED,								//!< We just went into untriggered state (used for HHLC)
	IOCTRL_SHALL_I_TRIGGER,							//!< Used for HHLC
	IOCTRL_SHALL_I_UNTRIGGER,						//!< Used for HHLC
	IOCTRL_SHALL_I_BEEP,								//!< Used for HHLC and others
	IOCTRL_REQUEST_HHLC_SHUT_DOWN,				//!< Used for HHLC
	IOCTRL_IS_DATA_EDITING_ALLOWED,				//!< Used for HHLC

	//! Used to find out whether the interface has an error free connection (USB)
	IOCTRL_IS_FAILSAVE_INTERFACE,

	//! Used to overcome a hardware problem. Our powerup time is too slow for the keyboard wedge.
	//! Therefore the PSOC can help us, but we need to tell the required state.
	IOCTRL_IS_ANA_SWITCH_ON_REQUIRED,
	//! Bluetooth needs a callback so we register it by sending in lparam
	IOCTRL_REG_CALLBACK,
	IOCTRL_SET_SPP_PORT,				//!< A method for now to det the spp port in the driver, should be done in open later
	IOCTRL_IS_SPP_PORT_SET,				//!< lets us check if spp port has been set, to indicate completion of link
	IOCTRL_GET_RSSI,					//!< get bluetooth RSSI value for this connection
	IOCTRL_GET_SPP_FLOW,				//!< get bluetooth SPP emulated rts/cts and dtr/dsr states
	IOCTRL_SET_SPP_FLOW,				//!< get bluetooth SPP emulated rts/cts and dtr/dsr states
	//! Keyboard drivers do not support all countires. Use this to ask which one is supported.
	//! Returns IOCRET_YES if supported, all other returns should be treaded as no
	//! pass the country to lparam
	IOCTRL_IS_COUNTRY_SUPPORTED,

	////////////////////////////////////////////////////////////////////////////
	// Old or experimental
	// returns IOCRET_YES or IOCRET_NO
//	IOCTRL_GET_VIS_MENU_INTERFACE,	      // we might not be able to use these (Dieter)
//	IOCTRL_GET_FACTORY_INTERFACE

	//! HComm needs an intercharacter receive timeout that is interface dependent
	IOCTRL_GET_INTERCHARACTER_RECEIVE_TIMEOUT,
	IOCTRL_GOT_AUTOBAUD,			//!< Returns true if serial port sees syn c cr (ignores parity and framing)

    ////////////////////////////////////////////////////////////////////////////
    // These are used in 4683 for using dual ports
    IOCTRL_SET_PRIMARY_PORT,
    IOCTRL_SET_AUX_PORT,

	IOCTRL_AM_I_ENUMERATED,         //!< The framework asks the driver whether it is enumerated

//ewr-02787
   //message has been thrown out instead of being sent, in standard serial interface only, with CTSMAN or CTSFST handshaking enabled
	IOCTRL_CHECK_IF_MESSAGE_NOT_SENT_IN_232_CTS_MAN,
	IOCTRL_ACK_BACK_TO_IO_THAT_MESSAGE_NOT_SENT_IN_232_CTS_MAN,       //ack that this transaction is complete and clear variable in IO when using a manual CTS mode
	IOCTRL_CHECK_IF_WE_ARE_IN_232_CTS_MAN, 	//Check if in standard serial interface only, using a manual controled CTS, by having CTSMAN or CTSFST handshaking enabled
//
	IOCTRL_EXPLICITLY_ENABLE_SCANNER,	// Used in USB HID interface to enable scanner by default which is different than other HID intefaces

	IOCTRL_GET_INTERFACE_OPEN_STRING,				//!<copies last parameter string into buffer poined to by LPARAM length wpaaram

	IOCTRL_SHALL_I_DELAY,		//!<insert interface delay
	IOCTRL_REM_SET_USB_DRV,		// Sets the pointer in the ReM USB driver to the CUsb2Base class of the interface

	//Used if control+ASCII translation not needed for prefix or suffix
	IOCTRL_NEED_PRESUFFIX_ESC,

	_IOCTRL_LAST_					//!< Just a dummy
};

//! Old historic return value.
#define IoControlRet_t MatrixError_t

/*! \enum IoControl_t
 Some examples how to use it:
 \code
	MatrixError_t CIoMan::DeviceIoControl  (IoControl_t cmd, LPARAM lparam=0, WPARAM wparam=0);

 	As you can see the LPARAM and WPARAM are optional parameters. They are used in specific manners for each command.
	Most commands do not need them, other use the LPARAM as a pointer to return a value. See further below.

	The return value can be the ERR_PASS, ERR_FAIL plus a few specifc values:
	IOCRET_DEFAULT	: means nobody did know that command and it was not handled in any way.
	IOCRET_YES		: a positive return value
	IOCRET_NO		: a negative return value

 	A simple uses:
	----------------------------------------------------------------------------
	MatrixError_t RetVal;
	RetVal = theStdInterface.DeviceIoControl(IOCTRL_PREPARE_FOR_LOW_POWER_STATE);
 	if(RetVal == ERR_PASS)
	{
		ChangePowerState();........
 	}

	----------------------------------------------------------------------------
	MatrixError_t RetVal;
	RetVal = theStdInterface.DeviceIoControl(IOCTRL_IS_BEEP_ON_GOOD_READ);
 	if(RetVal == IOCRET_YES)
	{
		Beep();........
 	}

	----------------------------------------------------------------------------
	Now more complex uses:
	----------------------------------------------------------------------------
	MatrixError_t RetVal;
 	UINT DataTypes =0;
	// Pass the addrss of DataTypes, so the driver can fill it with the correct value
	RetVal = theStdInterface.DeviceIoControl(IOCTRL_GET_SUPPORTED_DATATYPES, (LPARAM) &DataTypes);
 	if(RetVal == ERR_PASS)
	{
		if(DataTypes & IO_DATATYPE_CP_SYS)
 		{
			SendCharacterData();
 		}
		else if(DataTypes & IO_DATATYPE_HHLC)
 		{
			SendHHLCData();
 		}
		else                                // we don't know !!!
 		{
			ERROR!
 		}
 	}

	Note: The datatypes are declared in BarcodeData.h (IoDataType_t)
	----------------------------------------------------------------------------
	----------------------------------------------------------------------------
 	Now a few examples for the writers of drivers. Inside a driver there is a switch statement to dispatch the commands.
	If you don't handle a command in your driver, you MUST call your base class and pass the parameters to it.
	In some cases you also call to base class and further enhance the result of the base class (see IOCTRL_IS_DONE below).

	----------------------------------------------------------------------------
	IoControlRet_t CSerialBase::DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam)
	{
		// first call base class
		IoControlRet_t RetVal=baseCSerialBase::DeviceIoControl(cmd, lparam, wparam);
		switch(cmd)
		{
			case IOCTRL_IS_DONE:
				if(RetVal == IOCRET_YES)			// if base class said yes, then
				{
					if (!m_Port.IsTxComplete())	      // if uart fifo is not empty, ...
					RetVal = IOCRET_NO;
				}
			break;
			case IOCTRL_IS_LOW_POWER_ALLOWED:
				RetVal = IOCRET_YES;
			break;
			case IOCTRL_PREPARE_FOR_LOW_POWER_STATE:
				RetVal = PrepareForLowPowerSleep();
			break;
			case IOCTRL_RETURN_TO_NORMAL_POWER_STATE:
				RetVal = ReturnFromLowPowerSleep();
			break;
			case IOCTRL_IS_BAUDRATE_AVAILABLE:
			{
				if (lparam != 0)
				{
					ULONG BaudRate=0;
					UCHAR *pParameters = (UCHAR*) lparam;
					if (CParseHelper::Extract(pParameters, 0, BaudRate))
					{
						RetVal = (BaudRate < 1000000) ?  IOCRET_YES : IOCRET_NO;
					}
				}
			}
			break;
			default:
			break;
		}
		return RetVal;
	}

	----------------------------------------------------------------------------
	IoControlRet_t CHHLC::DeviceIoControl(IoControl_t cmd, LPARAM lparam, WPARAM wparam)
	{
		// first call base class
		IoControlRet_t RetVal=baseCSerialBase::DeviceIoControl(cmd, lparam, wparam);
		switch(cmd)
		{
			case IOCTRL_IS_LOW_POWER_ALLOWED:
				RetVal = IOCRET_YES;
			break;
			case IOCTRL_PREPARE_FOR_LOW_POWER_STATE:
				RetVal = PrepareForLowPowerSleep();
			break;
			case IOCTRL_RETURN_TO_NORMAL_POWER_STATE:
				RetVal = ReturnFromLowPowerSleep();
			break;
			case IOCTRL_SHALL_I_TRIGGER:
				RetVal = m_trigger ? IOCRET_YES : IOCRET_NO;
			break;
			case IOCTRL_SHALL_I_UNTRIGGER:
				RetVal = m_trigger ? IOCRET_NO : IOCRET_YES;
			break;
			case IOCTRL_GET_PREFERED_DATATYPES:
			case IOCTRL_GET_SUPPORTED_DATATYPES:
				if(lparam!=NULL)
				{
					IoDataType_t *pTypes = (IoDataType_t*)lparam;
					*pTypes = IO_DATATYPE_HHLC;
				}
				RetVal = ERR_PASS;
			break;
			default:
			break;
		}
		return RetVal;
	}
\endcode

	Todo: add another usefull example. Succestions?

	\remark A last note:
	The IoControl is not an extremely fast mechanism.
 	So if you go for extreme speed you need another solution. You could prepare a fast communication channel between the
	App andd the driver with an IoCOntrol and thenm use the fast channel.

	The nice thing with the IoCOntrol is that the commands find it's way to the right places where the answer is known
	automatically.

 */
#endif /* IO_CONTROL_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */





