#ifndef COMMANDHANDLER4683_H_
#define COMMANDHANDLER4683_H_

#include "def4683.h"
#include "Timeout.h"
//#include "BarcodeFifo.h"
#include "BarcodeData.h"
#include "rxfifo.h"

#define SIZEOF_RETAIL_BUF				500
#define	FRAME_SIZE						264
#define RX_BUFFER_SIZE                  2100
#define BARCODE_FIFO_SIZE               100

class DirectIoParser;
class CBarcodeFifo;

class CommandHandler4683
{
public:
// *** Functions in CommandHandler4683.cpp: ***
    CommandHandler4683(char port);				// Constructor
    ~CommandHandler4683();                      // Destructor
    void Initialize();                          // Initialize variables
    void InitStatusBytes();                     // Initialize status specifically based on the port
	MatrixError_t ExtractParameters(const tichar_t *pParameters ); // Extract menu parameters
    char GetAddress();                          // Returns our address character - 0x4A/0x4B/0x6X
    int GetTxFrameLength();                     // Returns the TxFrameLength
    unsigned char* GetTxMessage();              // Returns a pointer to the TxBuffer
    CRxFifo * GetRxFifo();                      // Returns a pointer to the RxFifo
    bool AmIPort6x(void);                       // Checks retailPort
    bool IsBeepingEnabled();                    // Check if beeping is enabled
    void SetProhibitClose( bool prohibit );     // Set prohibit close
    bool GetProhibitClose();                    // Get prohibit close
    CBarcodeFifo * GetBarcodeFifo();            // Returns pointer to the barcode fifo
    CBarcodeData * GetBarcodeToSend(void);
    void SentBarcode(void);
    bool GetExpectingMenuResponse(void);
    void StoreMenuResponse(CBarcodeData *pMenuResponse);
    void DirectIOCommandAccepted(bool dataReturned);
	void DirectIOCommandUndefined();
	void DirectIOCommandNotAllowed();
// *** Functions in 4683pro.cpp: ***
    void GotPolled();                           // Handles a poll, returns a pointer to data if there is any to send
    void ProcessRetailMessage( unsigned char* buffer, int length ); // Starts the command handling functionality, handle command, prepare response frame
    int SimulateScannerReset(void);             // Simulates a scanner reset to the host
    void ClearTxBuffer(void);                   // Sets TxFrame and TxData Lengths to zero
    bool CheckRetailFrame(void);                // Checks the incoming frame type and send and receive counts
    void ResetSendReceiveCounts(void);          // This will clear the send receive counts
    void SendEOP(void);                         // Send specific response frames
    void SendROL(void);
    void SendNsaFrame(void);
    void SendRRFrame(void);
    void SendMessage(void);                                     // Sends TxMessage to SendRetailFrame
    char ExecuteCommand(unsigned char * Message, int Length);   // Executes incoming message
    char AcceptCommand(unsigned char * Message);                // Accepts unknown commands
    bool HasStatusChanged();                                    // Check and update status bits based on current status
    bool CanITransmit6x(void);                                  // Checks if transmitting over Port6x is allowed
// *** Functions in 4683xlat.cpp: ***
    void Translate4680Command(unsigned char *pucCommandString); // Translate 4683 command into menu commands 
    void EnableScanner4683(void);                               // Enable the scanner
    void DisableScanner4683(void);                              // Disable the scanner
// ***Functions in 4683tx.cpp: ***
    void SendRetailFrame(unsigned char * Message );             // Construct a retail frame with address, control, status, and data
    void ClearStatusBits(void);                                 // Clear the status bits - set all to zero
    bool CanYouSendThisBarcode( CBarcodeData *pBarcode );       // Is the port capable of sending this barcode?

// Member structs and variables
bool startofsegment;
int PDFleftToSend;

typedef struct
{                               	    /*  Bit  Access */
	unsigned char BufferScans		:1;	/*    Bit 7     */
	unsigned char MenuResponse		:1;	/*    Bit 6     */  // Only used in USB
	unsigned char Waiting4DataAck	:1;	/*    Bit 5     */  // Not used either
	unsigned char ScannerDisabled	:1;	/*    Bit 4     */
	unsigned char IgnoreDisable     :1;	/*    Bit 3     */
	unsigned char 					:1;	/*    Bit 2     */
	unsigned char 					:1;	/*    Bit 1     */
	unsigned char 					:1;	/*    Bit 0     */
}IBM4680_Scanner_Status_t;
typedef struct RETAIL_RX_MESSAGE
{
	union
	{
		unsigned char	All		: 8;

		struct
		{
			unsigned char	bPoll           : 1;
			unsigned char	bAck_Frame      : 1;
			unsigned char	bSI_Frame       : 1;
			unsigned char	bNSI_Frame      : 1;
			unsigned char	bSNRM_Frame     : 1;
			unsigned char	bRR_Frame       : 1;
			unsigned char	bErrorFrame     : 1;
			unsigned char	bUnused         : 1;
		}Bits;
	}Char;
} RetailRxMessage_t;
struct FrameChar_t                                      /* The FRAME_CHAR structure provides 2 ways to      */
{                                                       /* read & write a buffered frame character:         */
    union CHAR                                          /*      8        7      6   5   4   3   2   1   0   */
    {                                                   /*+----------+-------------------------------------+*/
        unsigned char       Raw         : 8;            /*|bMP(Tx/Rx)|          R A W    C h a r           |*/
                                                        /*+----------+-------+-----------------------------+*/
        struct FLAG_POLL                                /*|bMP(Tx/Rx)| bType |           C h               |*/
		{                                               /*+----------+-------+-----------------------------+*/
            unsigned char   Ch          : 7;
            unsigned char   bType       : 1;            // 1 = Poll char., 0 = Addr/Flag
        } FlagPoll;

        struct
        {
            unsigned char   NotUsed     : 1;
            unsigned char   SendNum     : 3;
            unsigned char   AlwaysZero  : 1;
            unsigned char   RecNum      : 3;
        }SeqInfo;
    } Char;

    union
    {
        unsigned char       Raw         : 8;

        struct BITS
        {
            unsigned char   bMpTx       : 1;        /* 1 = Frame char., 0 = Data char. (R/W) */
            unsigned char   bMpRx       : 1;        /* 1 = Frame char., 0 = Data char. (R-O) */
            unsigned char   bTxEnd      : 1;        /* (R-O) */
            unsigned char   bParityErr  : 1;        /* (R/[W only 0]) */
            unsigned char   bFrameErr   : 1;        /* (R/[W only 0]) */
            unsigned char   bOverrunErr : 1;        /* (R/[W only 0]) */
            unsigned char   bRxFull     : 1;        /* (R/[W only 0]) */
            unsigned char   bTxEmpty    : 1;        /* (R/[W only 0]) */
        } Bits;
    }Byte1;
};

typedef struct STATUS_CONFIG_6X
{
    struct STATUS_6X
	{
        struct PORT_6X_BYTE_1
		{
            unsigned int         bRxBuffer2Full                   : 1;  /* LSB Bit 0 */ 
            unsigned int         bRxBuffer1Full                   : 1;
            unsigned int         bBufferOverrun                   : 1;
            unsigned int         bInternalAdapter                 : 1;
            unsigned int         bRxBuffer2Empty                  : 1;
            unsigned int         bRxBuffer1Empty                  : 1;
            unsigned int         bTimerOverflow                   : 1;
            unsigned int         bBreakInProgress                 : 1;  /* MSB Bit 7 */
		} Byte1;
        
        struct PORT_6X_BYTE_2
		{
            unsigned int         bRxBufferError                   : 1;
            unsigned int         bDTR                             : 1;
            unsigned int         bRTS                             : 1;
            unsigned int         bSoftErrorOnBringUp              : 1;
            unsigned int         bECLevelFollows                  : 1;
            unsigned int         bCommandError                    : 1;
            unsigned int         bInitialized                     : 1;
            unsigned int         bTxBufferEmpty                   : 1;
        } Byte2;

	} Status;
} StatusConfig6X_t;
typedef struct STATUS_CONFIG_5B
{
	struct CONFIG_5B
	{
		struct PORT_5B_STATUS_OUT
		{
			unsigned int			nI25HalfMinimumLength       : 4;
			unsigned int			bInterleaved2of5Enabled		: 1;
			unsigned int			bCode39Enabled				: 1;
			unsigned int			bUPCDEnabled				: 1;
			unsigned int			bUPCAandEANEnabled			: 1;
		} StatusOut;
	} Config;

	struct STATUS_5B
	{
		struct PORT_5B_BYTE_1
		{
			unsigned int			bECLevelFollows				: 1;
			unsigned int			bRASTestRanTwice			: 1;
			unsigned int			bUnused2					: 1;	/* Always zero */
			unsigned int			bScannerHWResetAck			: 1;
			unsigned int			bScannerMainBeepEnabled		: 1;
			unsigned int			bScannerHeadBeepDisabled	: 1;
			unsigned int			bPort2Bit2					: 1;
			unsigned int			bPort2Bit0					: 1;
		} Byte1;

		struct PORT_5B_BYTE_2
		{
			unsigned int			bScannerIsAlive				: 1;
			unsigned int			bScannerEnabled				: 1;
			unsigned int			bPort0WriteAck				: 1;
            unsigned int			bDeviceNotReady				: 1;
			unsigned int			bScannerCommTimeout			: 1;
			unsigned int			bScannerBufferOverflow		: 1;
			unsigned int			bHardwareError				: 1;
			unsigned int			bCommandReject				: 1;
		} Byte2;

		unsigned int	Byte3;											/* Always zero */
	} Status;
} StatusConfig5B_t;
typedef struct STATUS_CONFIG_9B
{
	struct CONFIG_9B
	{
		union
		{
			struct PORT_9B_MODE_PARAMETER
			{
				unsigned int				bAlwaysZero			: 2;
				unsigned int				bCheckByte			: 1;
				unsigned int				bDoubleTouch		: 1;
				unsigned int				nModeType			: 4;
			} ModeParameter;

			unsigned int		ModeByte;
		}Char;
		unsigned int		nLengthLimit;
	} Config;

	struct STATUS_9B
	{
		struct PORT_9B_BYTE_1
		{
			unsigned int				bECReadoutResponse		: 1;
			unsigned int				bTestEndResponse		: 1;
			unsigned int				bUnused2				: 1;
			unsigned int				bStatusResponse			: 1;
			unsigned int				bAckResponse			: 1;
            unsigned int				bReadDataInResponse		: 1;
			unsigned int				bModeDataInResponse		: 1;
			unsigned int				bUnused7				: 1;
		} Byte1;

		struct PORT_9B_BYTE_2
		{
			unsigned int				bReaderEnable				: 1;
			unsigned int				bBeeperEnable				: 1;
			unsigned int				bDataLengthError			: 1;
			unsigned int				bLabelTypeError				: 1;
			unsigned int				bReadError					: 1;
			unsigned int				bNotLastDataFor2_3BlockRead	: 1;
			unsigned int				bHardwareErrorInReaderBAT	: 1;
			unsigned int				bCommandRejected			: 1;
		} Byte2;
	} Status;
} StatusConfig9B_t;
typedef struct STATUS_CONFIG_17
{
	union
	{
		int	ConfigAll[8];

		struct CONFIG_17
		{
			struct PORT_17_CONFIG_BYTE_0
			{
				unsigned int				bUPCEAN							: 1;
				unsigned int				bUPCEANandUPCD					: 1;
				unsigned int				bUPCEANwith2DigitPeriodical		: 1;
				unsigned int				bUPCEANwith5DigitPeriodical		: 1;
				unsigned int				bUPCEANwith2and5DigitPeriodical	: 1;
				unsigned int				bCode39							: 1;
				unsigned int				bInterleaved2of5				: 1;
				unsigned int				bCodabar						: 1;
			} Byte0;

			struct PORT_17_CONFIG_BYTE_1
			{
				unsigned int				bCode93							: 1;
				unsigned int				bCode128						: 1;
				unsigned int				bAlwaysZero						: 6;
			} Byte1;

			struct PORT_17_CONFIG_BYTE_2
			{
				unsigned int				nInterleaved2of5Length			: 6;
				unsigned int				bAlwaysZero						: 2;
			} Byte2;

			struct PORT_17_CONFIG_BYTE_3
			{
				unsigned int					bBeeperEanbled				: 1;
				unsigned int					nBeeperDuration				: 2;
				unsigned int					nBeeperVolume				: 3;
				unsigned int					nBeeperFreq					: 2;
			} Byte3;

			unsigned int			Byte4;											/* Always zero */
			unsigned int			Byte5;											/* Always zero */

			struct PORT_17_CONFIG_BYTE_6
			{
				unsigned int						nMotorTimeout			: 3;
				unsigned int						nLaserTimeout			: 2;
				unsigned int						nDoubleReadTimeout		: 2;
				unsigned int						bAlwaysZero				: 1;
			} Byte6;

			struct PORT_17_CONFIG_BYTE_7
			{
				unsigned int						nLedBlink				: 2;
				unsigned int						nScansPerRead			: 2;
				unsigned int						bAlwaysZero4			: 1;
				unsigned int						bAttachmentsDecode		: 1;
				unsigned int						bOnOffSwitchEnabled		: 1;
				unsigned int						bAlwaysZero7			: 1;
			} Byte7;
		} Char;
	}Config;

	struct STATUS_17
	{
		struct PORT_17_STATUS_BYTE_0
		{
			unsigned int						bECLevelFollows				: 1;
			unsigned int						bUnused1_3					: 3;	/* Always zero */
			unsigned int						bScannerBeepEnabled			: 1;
			unsigned int						bUnused5_7					: 3;	/* Always zero */
		} Byte0;

		struct PORT_17_STATUS_BYTE_1
		{
			unsigned int						bAlwaysOne					: 1;	/* !! ALWAYS ONE !! */
			unsigned int						bScannerEnabledAndLaserOn   : 1;
			unsigned int						bUnused2_7					: 5;	/* Always zero */
			unsigned int						bCommandReject				: 1;	/* command reject this is not in the spec but all of our other products do it */
		} Byte1;

		struct PORT_17_STATUS_BYTE_2
		{
			unsigned int						bConfigureScannerCommandAccepted    : 1;
			unsigned int						bUnused1_7							: 7;
		} Byte2;
	} Status;
} StatusConfig17_t;
typedef struct
{
    union
    {
        unsigned char   All :8;
        struct
        {                           /*  Bit  Access */
            unsigned char DTR6x :1; /*      Bit 7   */
            unsigned char CTS   :1; /*      Bit 6   */
            unsigned char RCV   :1; /*      Bit 5   */
            unsigned char BRK   :1; /*      Bit 4   */
            unsigned char       :1; /*      Bit 3   */
            unsigned char       :1; /*      Bit 2   */
            unsigned char DSR   :1; /*      Bit 1   */
            unsigned char       :1; /*      Bit 0   */
        }Bits;
    }Char;
}Port_6x_Channel_Status_t;

protected:
    char                        retailAddress;          // Actual address 4A, 4B, or 6x
    char                        retailPort;             // Protocol ID 5B, 9B, 17, or 6x
    int                         retailState;            // Internal state machine
    IBM4680_Scanner_Status_t    scannerStatus;          // Random status variables
    unsigned char               outputBuffer[259];      // Temporary output buffer
    char						outputState;            // No output or Message Ready 
	char						dataWasSent;            // Data was sent variable
    FrameChar_t                 S_R_Count;              // Send and receive counts
    //volatile int                outputCount;            // Output count for transmitting
    unsigned int                *statusPtr;             // A pointer to the status bytes
    union
    {
        StatusConfig9B_t        Port9BControl;          // port 9b control bytes 
        StatusConfig17_t        Port17Control;          // port 17 control bytes 
        StatusConfig5B_t        Port5BControl;          // port 5b control bytes 
        StatusConfig6X_t        Port6XControl;          // port 6x control bytes 
    }Status;
    CRxFifo                     *RxFifo;                // RxFifo for stuffing menu data
    bool                        sendStatus;             // Send the scanner's status only on the next poll
    char						controlChar;            // Send/Receive count, NSA, ROL, Command Reject character
    volatile int                TxFrameLength;                  // tx length 
	unsigned char               RetailTxBuf[FRAME_SIZE];        // Tx buffer 
	volatile int                TxDataLength;                   // length of the bar code message sent 
	unsigned char               TxMessage[FRAME_SIZE];          // pointer to bar code message 
	unsigned char               RetailRxBuf[SIZEOF_RETAIL_BUF+4];   // receive buffer 
    volatile int                RxFrameLength;          // Length of RetailRxBuffer
    CBarcodeFifo                *BarcodeFifo;           // CBarcodeFifo for transmitting barcodes
    CBarcodeData                *pCurrentBarcode;       // The current barcode to send
    CBarcodeData                m_BarcodeDummy;         // Dummy barcode used as a saftey against crashes
    RetailRxMessage_t           ControlType;            // Output message/frame type (poll, RR, SI)
    RetailRxMessage_t			RetailRxType;           // Input message/frame type
    bool                        prohibitClose;          // set by the VISMNU PnP code via IoControl
    int							mBeeperVolume; 
    bool						m_IgnoreConfigFlag;	// makes the setup easier if the SurePos is configured wrong
    // Port 6x Variables
    CTimeoutmS                  port6xTimer;        
    int                         port6xTimeout;
    Port_6x_Channel_Status_t    port6xChannelStatus;
    const bchar_t               *tempBarcodePtr;        // This pointer is used in sending menu responses greater than 244 bytes over 6x

    
private:
	CommandHandler4683 (const CommandHandler4683&);            // declared private, but not defined, to prevent copying         
	CommandHandler4683& operator=(const CommandHandler4683&);  // declared private, but not defined, to prevent copying 

    DirectIoParser				*dioParser;
};

#endif /*COMMANDHANDLER4683_H_*/


