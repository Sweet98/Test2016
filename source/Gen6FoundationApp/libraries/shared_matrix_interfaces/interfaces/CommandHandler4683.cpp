/*=================================================================================
  class: CommandHandler4683
            This class will work with the CSerial4683 (link layer) by implementing
            the specific protocols.  This class has been designed to allow multiple
            instances of this class to be run simultaneously.

===================================================================================*/
#include "stdInclude.h"
#include "parsehelper.h"
#include "CommandHandler4683.h"
#include "BarcodeData.h"
#include "BarcodeFifo.h"
#include "DirectIoParser.h"


/****************************************************************************
 *
 *  Function:   Constructor
 *              Setup the retailPort, address, and RxFifo
 *
 *  Arguments:  char    port        - Retail PortID 
 *
 * **************************************************************************/
CommandHandler4683::CommandHandler4683(char port)
	:m_BarcodeDummy(10)
{
    retailPort = port;
    switch(retailPort)
    {
        case PORT_5B_ID:
        case PORT_9B1_ID:
        case PORT_9B2_ID:
            retailAddress = PORT_5B;
            break;
        case PORT_17_ID:
            retailAddress = PORT_17;
            break;
        case PORT_68_ID:
            retailAddress = PORT_68;
            break;
        case PORT_69_ID:
            retailAddress = PORT_69;
            break;
        case PORT_64_ID:
            retailAddress = PORT_64;
            break;
        case PORT_65_ID:
            retailAddress = PORT_65;
            break;
        default:
            retailAddress = 0x00;
            break;
    }
    if( retailAddress == 0x00 )
    {
        RxFifo      = NULL;
        BarcodeFifo = NULL;
        dioParser = NULL;
    }
    else
    {
        RxFifo = new CRxFifo(RX_BUFFER_SIZE);
        BarcodeFifo = new CBarcodeFifo(BARCODE_FIFO_SIZE);
        dioParser = new DirectIoParser(RxFifo);
    }
	retailState = WAIT_POLL_OR_NS_FRAME;    // Initialize state machine
    outputState = NO_MESSAGE;
    dataWasSent = FALSE;
    scannerStatus.ScannerDisabled = 1;      // initialize the Scanner to be Disabled on P.O.R.! 
    scannerStatus.BufferScans = 0;
	scannerStatus.MenuResponse = 0;
	scannerStatus.Waiting4DataAck = 0;
	scannerStatus.IgnoreDisable = 0;
    sendStatus = false;
    RxFrameLength = 0;
    ControlType.Char.All = 0;
    RetailRxType.Char.All = 0;
    controlChar = 0;
    // Port 6x Variables
    port6xTimer.SetExpiration(0);
    port6xTimeout = 0;
    port6xChannelStatus.Char.All = 0;
    tempBarcodePtr = NULL;
    prohibitClose = true; 
    statusPtr = NULL;
    TxFrameLength = 0;
    TxDataLength = 0;
    mBeeperVolume = 0;
    //init variables for new feature 260
    startofsegment = true;
    PDFleftToSend = 0;
    m_IgnoreConfigFlag = false;

    pCurrentBarcode = NULL;
}

/****************************************************************************
 *
 *  Function:   Destructor
 *              Only delete the fifos if you are the aux port, the driverbase 
 *              will delete the fifos for the primary port
 *
 * **************************************************************************/
CommandHandler4683::~CommandHandler4683()
{
    if( (retailAddress != PORT_5B) && (retailAddress != PORT_17) )
    {
        if( RxFifo != NULL )
        {
	        delete RxFifo;
            RxFifo = NULL;
        }
        if( BarcodeFifo != NULL )
        {
	        delete BarcodeFifo;
            BarcodeFifo = NULL;
        }
    }
    if( dioParser != NULL )
    {
    	delete dioParser;
    	dioParser = NULL;
    }
}

/****************************************************************************
 *
 *  Function:   Initialize
 *              Initialize states, variables, and status.  Also call
 *              InitStatusBytes to initialize status per retailPort
 *
 * **************************************************************************/
void CommandHandler4683::Initialize()
{
    InitStatusBytes();
    ResetSendReceiveCounts();
    ClearTxBuffer();
    retailState = WAIT_POLL_OR_NS_FRAME;    // Initialize state machine
    outputState = NO_MESSAGE;
    dataWasSent = FALSE;
    scannerStatus.ScannerDisabled = 1;      // initialize the Scanner to be Disabled on P.O.R.! 
    scannerStatus.BufferScans = 0;
    sendStatus = false;
    RxFrameLength = 0;
    ControlType.Char.All = 0;
    RetailRxType.Char.All = 0;
    controlChar = 0;
    // Port 6x Variables
    port6xTimer.SetExpiration(0);
    port6xChannelStatus.Char.All = 0x00;
    tempBarcodePtr = NULL;
    prohibitClose = true; 
    startofsegment = true;
    PDFleftToSend = 0;
}


/****************************************************************************
 *
 *  Function:   InitStatusBytes
 *              Initializes the status bytes based on the retailPort
 *
 * **************************************************************************/
void CommandHandler4683::InitStatusBytes(void)
{
	unsigned int* puchStatus;
	switch(retailPort)
	{
		case PORT_5B_ID:
			retailAddress = PORT_5B;
			statusPtr = (unsigned int *)&Status.Port5BControl.Status.Byte1;
			puchStatus = (unsigned int *)&Status.Port5BControl;
			puchStatus[0] = 0x0;
			puchStatus[1] = 0x10;
			puchStatus[2] = 0x01;
			puchStatus[3] = 0x0;
			break;
		case PORT_9B1_ID:
		case PORT_9B2_ID:
			retailAddress = PORT_9B;
			statusPtr = (unsigned int *)&Status.Port9BControl.Status.Byte1;
			puchStatus = (unsigned int *)&Status.Port9BControl;
			for(int i = 0; i < 4; i++){puchStatus[i] = 0;}      // clear all status bits
			break;
		case PORT_17_ID:
			retailAddress = PORT_17;
			statusPtr = (unsigned int *)&Status.Port17Control.Status.Byte0;
			puchStatus = (unsigned int *)&Status.Port17Control;
			for(int i = 0; i < 11; i++){puchStatus[i] = 0;}     // clear all status bits 
			puchStatus[9] = 0x01;       // then set the default value
			break;
        case PORT_64_ID:
            retailAddress = PORT_64;
            statusPtr = (unsigned int *)&Status.Port6XControl.Status.Byte1;
            puchStatus = (unsigned int *)&Status.Port6XControl;
            puchStatus[0] = 0x0;
            puchStatus[1] = 0x46;    // RTS | DTR | INITIALIZED
			break;
        case PORT_65_ID:
            retailAddress = PORT_65;
            statusPtr = (unsigned int *)&Status.Port6XControl.Status.Byte1;
            puchStatus = (unsigned int *)&Status.Port6XControl;
            puchStatus[0] = 0x0;
            puchStatus[1] = 0x46;    // RTS | DTR | INITIALIZED
			break;
        case PORT_68_ID:
            retailAddress = PORT_68;
            statusPtr = (unsigned int *)&Status.Port6XControl.Status.Byte1;
            puchStatus = (unsigned int *)&Status.Port6XControl;
            puchStatus[0] = 0x0;
            puchStatus[1] = 0x46;    // RTS | DTR | INITIALIZED
			break;
        case PORT_69_ID:
            retailAddress = PORT_69;
            statusPtr = (unsigned int *)&Status.Port6XControl.Status.Byte1;
            puchStatus = (unsigned int *)&Status.Port6XControl;
            puchStatus[0] = 0x0;
            puchStatus[1] = 0x46;    // RTS | DTR | INITIALIZED
			break;
	}    
}

MatrixError_t CommandHandler4683::ExtractParameters(const tichar_t *pParameters )
{
	bool ret=true;
	int tmpIgn = 0;
	ret &= CParseHelper::Extract(pParameters, _TI("060F02"), tmpIgn);
	ret &= CParseHelper::Extract(pParameters, _TI("05021D"), mBeeperVolume);
	ret &= CParseHelper::Extract(pParameters, _TI("060F03"), m_IgnoreConfigFlag);
	scannerStatus.IgnoreDisable = tmpIgn;
	return ret;
}

/****************************************************************************
 *
 *  Function:   GetAddress
 *              Return the retail address as a character
 *
 *  Retrns:     char    The retail address
 *
 * **************************************************************************/
char CommandHandler4683::GetAddress() 
{
    return retailAddress;
}

/****************************************************************************
 *
 *  Function:   GetTxFrameLength
 *              Return the length of the current frame to transmit
 *
 *  Retrns:     int     The length of the frame to transmit
 *
 * **************************************************************************/
int CommandHandler4683::GetTxFrameLength()
{
    return TxFrameLength;
}


/****************************************************************************
 *
 *  Function:   GetTxMessage
 *              Return a pointer to the output buffer
 *
 *  Retrns:     unsigned char *     Pointer to the output buffer to transmit
 *
 * **************************************************************************/
unsigned char* CommandHandler4683::GetTxMessage()
{
    return RetailTxBuf;
}


/****************************************************************************
 *
 *  Function:   GetRxFifo
 *              Return a pointer to our RxFifo that we stuff with menu data -
 *              each CommandHandler object has one so the application knows 
 *              where menu data is coming from
 *
 *  Returns:    Pointer to our RxFifo 
 *
 * **************************************************************************/
CRxFifo * CommandHandler4683::GetRxFifo()
{
    return RxFifo;
}

/****************************************************************************
 *
 *  Function:   IsBeepingEnabled()
 *              Check to see if we're allowed to beep
 *
 *  Returns:    True if we're allowed to beep, false if not
 *
 * **************************************************************************/
bool CommandHandler4683::IsBeepingEnabled()
{
    return (!scannerStatus.ScannerDisabled || scannerStatus.BufferScans || AmIPort6x());
}

/****************************************************************************
 *
 *  Function:   SetProhibitClose
 *              Sets the prohibitClose variable
 *
 *  Arguments:  bool    prohibit    New value of prohibitClose
 *
 * **************************************************************************/
void CommandHandler4683::SetProhibitClose( bool prohibit )
{
    prohibitClose = prohibit;
}

/****************************************************************************
 *
 *  Function:   GetProhibitClose
 *              Returns the prohibitClose variable value
 *
 *  Returns:    bool    Value of prohibit close
 *
 * **************************************************************************/
bool CommandHandler4683::GetProhibitClose()
{
    return prohibitClose;
}

/****************************************************************************
 *
 *  Function:   GetBarcodeFifo
 *              Returns a pointer to the Barcode Fifo
 *
 *  Returns:    CBarcodeFifo *  Pointer to the barcode fifo
 *
 * **************************************************************************/
CBarcodeFifo * CommandHandler4683::GetBarcodeFifo()
{
    return BarcodeFifo;
}

/****************************************************************************
 *
 *  Function:   GetBarcodeToSend
 *              Returns a pointer the next barcode in the Barcode Fifo
 *
 *  Returns:    CBarcodeData *  Pointer to the next barcode in the fifo
 *
 * **************************************************************************/
CBarcodeData * CommandHandler4683::GetBarcodeToSend(void)
{
	// only get a new one if the old one has been sent (pointer is NULL)
	if (pCurrentBarcode == NULL)
	{
		pCurrentBarcode = BarcodeFifo->Read();
	}

	// if still NULL, then there is a programming error. So protect against the crash.
	if (pCurrentBarcode == NULL)
	{
		pCurrentBarcode = &m_BarcodeDummy;
	}

	return pCurrentBarcode;
}

void CommandHandler4683::SentBarcode(void)
{
	pCurrentBarcode->Free();
	pCurrentBarcode = NULL;
}

/****************************************************************************
 *
 *  Function:   GetExpectingMenuResponse
 *              Returns the direct IO parser's expecting menu response value
 *
 *  Returns:    bool    Value of direct IO parser's expecting menu response
 *
 * **************************************************************************/
bool CommandHandler4683::GetExpectingMenuResponse(void)
{
	return dioParser->GetExpectingMenuResponse();
}

// Can be called in the write finction of a driver to bypass the TxFifo.
// See SurePos driver for an example.
void CommandHandler4683::StoreMenuResponse(CBarcodeData *pMenuResponse)
{
	if( dioParser != NULL )
	{
		dioParser->StoreMenuResponse(pMenuResponse);
	}
}

/*********************************************************************
 **	Byte 0:
 *		Bit 0:	EC Level Response
 *		Bit 1:	Test End Response
 *		Bit 2:	Not Used
 *		Bit 3:	Scanner Reset ACK
 *		Bit 4:	Scanner Main Beeper
 *		Bit 5:	Scanner Second Beeper
 *		Bit 6:	Direct IO Data Returned
 *		Bit 7: 	Not Used (always 0)
 **	Byte 1:
 *		Bit 0:	Scanner Alive (always 1)
 *		Bit 1:	Scanner Status (0=disabled, 1=enabled)
 *		Bit 2:	unknown
 *		Bit 3:	unknown
 *		Bit 4:	unknown
 *		Bit 5:	unknown
 *		Bit 6:	Hardware Error in Reader BAT
 *		Bit 7: 	Command Rejected
 **	Byte 2: (no spec states exactly what these mean)
 *		Bit 0:	unknown
 *		Bit 1:	unknown
 *		Bit 2:	unknown
 *		Bit 3:	Direct IO Command accepted
 *		Bit 4:	Direct IO Command not allowed
 *		Bit 5:	Direct IO Command undefined
 *		Bit 6:	unknown
 *		Bit 7: 	unknown
 ************************************************************************/
void CommandHandler4683::DirectIOCommandAccepted(bool dataReturned)
{
	if( dataReturned )
	{
		Status.Port5BControl.Status.Byte1.bPort2Bit2 = 1;
	}
	else
	{
		Status.Port5BControl.Status.Byte1.bPort2Bit2 = 0;
	}
	statusPtr[2] = 0x08;
}

// This response is used when the direct IO command had an error
// ACK the 0x30 (direct IO), but set the 3rd bit stating the subcommand 
// is not allowed
void CommandHandler4683::DirectIOCommandNotAllowed()
{
	Status.Port5BControl.Status.Byte1.bPort2Bit2 = 0;
	statusPtr[2] = 0x10;
	TxDataLength = 0;
}

// This response is also used when the scanner is not in flash
// mode and a flash command was received, and when an unknown 
// direct IO command has been received
// ACK the 0x30 (direct IO), but set the 3rd bit stating the subcommand 
// is undefined
void CommandHandler4683::DirectIOCommandUndefined()
{
	Status.Port5BControl.Status.Byte1.bPort2Bit2 = 0;
	statusPtr[2] = 0x20;
	TxDataLength = 0;
}


