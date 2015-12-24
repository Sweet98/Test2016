/*=============================================================================
 * 4683 definitions

 *=============================================================================
 * $RCSfile: interfaces/def4683.h $
 * $Revision: 2.4.1.5.1.3 $
 * $Date: 2010/08/20 13:10:29EDT $
 *============================================================================*/

#ifndef FILE_DEF4683_H__INCLUDED_
#define FILE_DEF4683_H__INCLUDED_

#define TRUE								1
#define FALSE								0
#define OFF									0
#define ON									1

#define PORT_9B							0x4b
#define PORT_5B							0x4b
#define PORT_17							0x4a
#define PORT_64                     0x64  //Left slot
#define PORT_65                     0x65  //Right slot
#define PORT_68                     0x68  //Left slot
#define PORT_69                     0x69  //Right slot
#define BROADCAST_ADDRESS				0x7a

#define PORT_5B_ID						0
#define PORT_9B1_ID						1
#define PORT_9B2_ID					  	2
#define PORT_17_ID						3
#define PORT_68_ID                  4
#define PORT_69_ID                  5
#define PORT_64_ID                  7
#define PORT_65_ID                  8


#define NO_MESSAGE			  				0
#define MESSAGE_NOT_READY					1
#define MESSAGE_READY						2

// misc
#define CONTROL_CHAR						1
#define FRAME_END_FLAG					0x7e

//Timer stuff
#define TIME_TO_ANSWER_POLL			1150									  			/* time to answer a poll in us*/
#define POLL_TIME_OUT					TIME_TO_ANSWER_POLL
#define TIME_BEFORE_ANSWERING_POLL	200									  			/* time to wait before answering a poll in us*/
#define POLL_RESPONSE_DELAY			TIME_BEFORE_ANSWERING_POLL

//mode select (databyte1)
#define TEST_MODE							0x00
#define UPC_EAN_CODABAR					0x10
#define UPC_EAN_UPC_D3					0x20
#define UPC_EAN_CODE_39					0x30
#define UPC_EAN_ITF						0x40
#define UPC_EAN__CODE_128				0x50
#define UPC_EAN_CODE_93					0x60
#define UPC_EAN_SUPP_2_5				0x70
#define STANDARD_2_OF_5					0x80
#define INTERLEAVED_2_OF_5				0x90
#define CODE_128							0xa0
#define CODE_93							0xb0
#define SUPP_2_5							0xc0

/* for rx case statement */
#define FIRST_CHAR						0
#define LOOKING_FOR_END_CHAR			1
#define LOOKING_FOR_POLL				2
#define WAITING_UNTIL_PROCESSED		3

/* Frame commands */
#define SEQUENCED_TYPE					0x00
#define SEQUENCED_MASK					0x11
#define SUPERVISORY_TYPE				0x01
#define SUPERVISORY_MASK				0x1f
#define NON_SEQUENCED_TYPE				0x03
#define NON_SEQUENCED_MASK				0x13
#define ADDRESS_TYPE						0x80
#define ADDRESS_MASK						0x80

/* Rx commands */
#define SEND_NORMAL_RESPONSE_MODE	0x83

//used for switch (ProcessState)
#define WAIT_POLL_OR_NS_FRAME			0
#define SEND_NSA_FRAME					1
#define SEND_NS_RESPONSE		 		2
#define WAIT_FOR_NSA_FRAME				3
#define COMMAND_REJECT_MODE			4

#define NORMAL_RESPONSE_MODE			5
#define SEND_I_FRAME						6
#define WAIT_POLL_SEND_NS_RESPONSE	7
#define WAIT_FOR_ACK						8
#define NORM_WAIT_FOR_NSA_FRAME		9
#define SEND_STATUS						10

// 5B commands
/* byte 1 */
#define SYSTEM_COMMAND					0x00
#define SCANNER_ENABLE              0x11
#define SCANNER_DISABLE					0x12
#define BEEPER_ENABLE					0x14
#define BEEPER_DISABLE					0x18
#define BEEPER2_ENABLE 					0x20
#define BEEPER2_DISABLE					0x21
#define P2_2_ON							0x22
#define P2_2_OFF							0x24
#define P2_0_ON							0x28
#define P2_0_OFF							0x30
#define FLASH_30_COMMAND                0x30
#define STATUS_OUT						0x31
#define RESET_SCANNER					0x32
#define LONG_DIRECT_IO_COMMAND			0x35

#define TEST								0x10
#define STATUS_REQUEST					0x20
#define POR									0x40
#define EC_READOUT						0x80

#define CONFIGURE_SCANNER				0x20
#define MODE_SELECT						0x20
#define LENGTH_LIMIT						0x21
#define READ_MODE_STATUS				0x31
#define SCANNER_INIT						0x32

#define SYSTEM_CHAR1						2
#define SYSTEM_CHAR2						3
#define SYSTEM_CHAR3						4
#define SYSTEM_CHAR4                        5
#define SYSTEM_CHAR5					6
#define DATA_BYTE_1						3

//Tx/Rx commands
#define NON_SEQUENCED_ACK				0x63

//Tx commands
#define REQUEST_ON_LINE					0x0f
#define COMMAND_REJECT					0x87
#define END_OF_POLL						0x5a

//ec_readout_byte
#define EC_READOUT_BYTE					0x03
#define EC_READOUT_BYTE_17				0x40

#define ACK									0x06
#define _EOF_									-1

#define GOOD_COMMAND							0
#define BAD_COMMAND						 	1
#define BAD_COMMAND_BROADCAST_ADDRESS	2

// Download Project
// Async adapter command byte 1
#define IBM_SYSTEM_COMMAND				0x00
#define IBM_SETUP						0x01
#define IBM_CHANNEL_CONTROL			    0x02
#define IBM_READ_STATUS					0x04
#define IBM_TRANSMIT					0x08			// Data from 4680 to TT8800
#define IBM_RECEIVE_BUFFER				0x10			// Data from TT8800 to 4680
#define IBM_WRAP						0x40

// Async adapter command byte 2 for system_commands
#define IBM_TEST						0x10
#define IBM_STATUS_REQUEST				0x20
#define IBM_RESET						0x40
#define IBM_EC_LEVEL_REQUEST			0x80

// Async Adapter status byte 1 bits definitions
#define RX_BUFFER_2_FULL				0x01
#define RX_BUFFER_1_FULL				0x02
#define OVERRUN							0x04
#define FLASH_IN_PROGRESS				0x08
#define RX_BUFFER_2_N_EMPTY			    0x10
#define RX_BUFFER_1_N_EMPTY			    0x20
#define TIMER_OVERFLOW					0x40
#define BREAK_IN_PROGRESS				0x80

// Async Adapter status byte 2 bits definitions
#define RX_BUFFER_ERROR					0x01
#define DTR								0x02
#define RTS								0x04
#define SOFT_ERROR						0x08
#define EC_LEVEL_FOLLOWS				0x10
#define COMMAND_ERROR					0x20
#define INITIALIZED						0x40
#define TX_BUFFER_N_EMPTY				0x80

// The EC level we will report
#define	EC_LEVEL						0xff

// Definition to interpret the IBM_CHANNEL_CONTROL command
#define RCV_ENABLE						0x04

#endif

