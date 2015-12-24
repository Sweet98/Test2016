/*=============================================================================
 * <One line description>

 *=============================================================================
 * $RCSfile: interfaces/4683pro.h $
 * $Revision: 1.2 $
 * $Date: 2005/09/26 14:13:13EDT $
 *============================================================================*/

#ifndef FILE_4683Pro_H__INCLUDED_
#define FILE_4683Pro_H__INCLUDED_

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
//#define P2_0_OFF                            0x30
#define FLASH_30_COMMAND                0x30
#define STATUS_OUT						0x31
#define RESET_SCANNER					0x32

/* byte 2 part of system_command*/
#define FLASH_COMMAND                   0x1D
#define FLASH_DATA                      0x1B

#define ENTER_FLASH_MODE                0x00
#define REQUEST_PART_NUMBER             0x01
#define REQUEST_PERIPHERAL_ID           0x02
#define SELECT_FLASH_SECTION            0x03
#define GET_CRC_16                      0x04
#define ERASE_SELECTED_SECTION          0x05
#define SELECT_FLASH_OFFSET             0x06
#define UPDATE_DEVICE                   0x07
#define RESET_DEVICE                    0x09
#define GET_CRC_32                      0x0A

#define DIO_4683                        0x41
#define SPECIAL_4683                    0x46
#define CONFIG_GET                      0xA3
#define CONFIG_SET                      0xA2

#define FULL_UNIT_ID                    0x30
#define FW_NEW_VERSION_REPORT           0x34
#define FW_VERSION_REPORT               0x36
#define SERIAL_NUMBER_REPORT            0x35
#define BEEP_4683                       0x32
// Add changes to Def4683.h too

#define TEST                                0x10
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

#define GOOD_COMMAND							0
#define BAD_COMMAND						 	1
#define BAD_COMMAND_BROADCAST_ADDRESS	2

#endif

