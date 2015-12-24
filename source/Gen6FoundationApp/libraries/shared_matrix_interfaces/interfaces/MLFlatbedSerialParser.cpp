/*=================================================================================
  class: CFlatBedParser

//=================================================================================
   $Source: interfaces/MLFlatbedSerialParser.cpp $
   $Date: 2009/12/17 02:32:52EST $
   $Revision: 1.1 $


//=================================================================================*/
#include "stdInclude.h"
#include "BarcodeDataMan.h"
#include "MLFlatbedSerialParser.h"


#define	NO_READ		0x9C				//our decoder does not know what it is.
#define	BAD_TRY		0xAF           //No conversion to ML symbology ID
#define	ID_START_POSITION		60				//our decoder starts with this ID value
#define	ID_END_POSITION		122				//our decoder ends with this ID value

														//HHPid,				DEC#,				CODE
 static const char codeIdConversion[] =
{
	0xAF,											  //<,					60,				code 32 Pharmaceutical
	0x63,											  //=,					61,				Trioptic Code
	0x9C,											  //>,					62,
	0xAF,											  //?,					63,				Korea Post
	0x9C,											  //@,					64,
	0x71,											  //A,					65,				Australian Post
	0x74,											  //B,					66,				British Post
	0xAF,											  //C,					67,				Canandian Post
	0x03,											  //D,					68,				EAN-8
	0x02,											  //E,					69,				UPC-E
	0x9C,											  //F,					70,
	0x9C,											  //G,					71,
	0x9C,											  //H,					72,
	0x77,											  //I,					73,				UCC/EAN-128
	0x72,											  //J,					74,				Japanese Post
	0x73,											  //K,					75,				KIX Post
	0x6F,											  //L,					76,				Planet Code
	0x78,											  //M,					77,				4-CB (4-state customer Barcode)
	0x70,											  //N,					78,				ID-tag (UPU 4-state)
	0xA0,											  //O,					79,				OCR - all types
	0x6E,											  //P,					80,				Postnet
	0xAF,											  //Q,               81,				China Post
	0x69,											  //R,					82,				Micro PDF417
	0x9C,											  //S,					83,
	0xAF,											  //T,					84,				TLC39 (TCIF linked code 39)
	0x9C,											  //U,					85,
	0x9C,											  //V,					86,
	0xAF,											  //W,					87,				Posi Code
	0x9C,											  //X,					88,
	0x9C,											  //Y,					89,
	0x9C,											  //Z,					90,
  	0x9C,											  //[,					91,
	0x9C,											  //\,					92,
	0x9C,											  //],					93,
	0x9C,											  //^,					94,
	0x9C,											  //_,					95,
	0x9C,											  //',					96,
	0x51,											  //a,					97,				Codabar
	0x50,											  //b,					98,				code 39
	0x04,											  //c,					99,				UPC-A, or UPC-A with extended coupon code
	0x05,											  //d,					100,				EAN-13, or EAN-13 with extended coupon code
	0x52,											  //e,					101,				interleaved 2 of 5
	0x62,											  //f,					102,				Straight 2 of 5,  IATA and Indutrial
	0x5B,											  //g,					103,				MSI
	0x5C,											  //h,					104,				code 11
	0x54,											  //i,					105,				code 93 and 93i
	0x53,											  //j,					106,				code 128
	0x9C,											  //k,					107,
	0xAF,											  //l,					108,				code 49
	0x5e,											  //m,					109,				Matrix 2 of 5
	0x60,											  //n,					110,				Plessy Code
	0xAF,											  //o,					111,				code 16k
	0x9C,											  //p,					112,
	0x76,											  //q,					113,				Codablock F
	0x68,											  //r,					114,				PDF417
	0x6C,											  //s,					115,				QR code / Micro QR
	0x5F,											  //t,					116,				Telepen
  	0x9C,											  //u,					117,
	0xAF,											  //v,					118,				VeriCode
	0x6A,											  //w,					119,				Data Matrix
	0x6D,											  //x,					120,				MaxiCode
	0x65,											  //y,					121,				EAN UCC Composite, RSS - all types
	0x6B											  //z,					122,				Aztec
};



CFlatBedParser * g_pFlatBedParser = NULL;

void CFlatBedParser::CreateFlatBedParser(void)
{
	if (g_pFlatBedParser == NULL)
	{
		g_pFlatBedParser = new CFlatBedParser;
	}
	ASSERT(g_pFlatBedParser != NULL);
}

void CFlatBedParser::DestroyFlatBedParser(void)
{
	delete g_pFlatBedParser;
	g_pFlatBedParser = NULL;
}



// Constructor
CFlatBedParser::CFlatBedParser()
{
	//flatbed packet info
	m_PacketSuffixSize = 0;                 //packet suffix size
	m_PacketPrefixSize = 0;                 //suffix prefix size
	m_LrcMinDataSize = 0;                 // prefix + data min size
	m_MinDataSize = 0;                    // min data alone size
	m_PacketMode = 0;                     //packet type

	//valid packet mode check
	m_bFoundValidPacketMode = false;

	//min char check
	m_bFoundMinCharSize = false;

	//barcode id
	m_ucPacketBarcodeId = 0x00;          //barcode ID for the packet

	//actual number of charaters in packet not including escapes
	m_uiPacketActualDataCount = 0;

	//the LRC calculation
	m_LRC = 0;

	//starting data sizes
	m_uiPreEscapeLength = 0;
	m_uiPostEscapeLength = 0;

	//barcode data source info
	m_uiNumSourceChars = 0;   				// Number of characters in the source barcode data buffer

	m_uiCharsLeft = 0;     // Number of characters left that have not been placed in a packet

	//barcode data destination info
	m_uiDestinationSize = 0;					  // size of new barcode data destination buffer
	m_uiTotalBarcodeDataDestinationSize = 0;           //the final size of the new barcode data
}


// Destructor
CFlatBedParser::~CFlatBedParser()
{
}



#define FLAT_BED_AUX_MODE		2			//only packet mode we support so far - make it a menu item in the future, then remove this.

#define  MAX_PACKET_SIZE  57

 /*****************************************************************************/
bool CFlatBedParser::FormatBarcodeData( CBarcodeData *&pLabel, unsigned int uiPacketModeType )
{
	bool bPacketModified = true;

	CBarcodeData *pDestinationLabel;

	m_PacketMode = uiPacketModeType;         //set packet mode type we are using from the packet mode type passed in from menu

	//Set the configuration for packet mode type we are using
	SelectAuxMode();

	//check to see if a valid packet mode has been selected
	if( m_bFoundValidPacketMode == false )
	{
		bPacketModified = false;
		return bPacketModified;
	}

	//convert the barcode ID
	GetBarcodeId( pLabel );

	//add and escapes needed to the data
	AddEscapeCodes( pLabel );

	//check data for min size
	if( m_bFoundMinCharSize == false )
	{
		bPacketModified = false;
		return bPacketModified;
	}

	//if single packet
	if( m_uiPostEscapeLength <= MAX_PACKET_SIZE )
	{
		size_t uiFinishLocation;

		AddPacketPrefix( pLabel );		//put on the prefixes

		// Get the raw buffer and size of the current message
	  	if( PASS != pLabel->GetContentsBuffer(m_pucSourceData, m_uiNumSourceChars) )
		{
			bPacketModified = false;
			return bPacketModified;
		}

		uiFinishLocation = m_uiNumSourceChars - 1;
		m_LRC = CalcLrc( m_pucSourceData, m_uiNumSourceChars, 0, uiFinishLocation );    //calculate LRC
		AddPacketSuffix( pLabel );     //put on the suffixes
	}
	else if( m_PacketMode == 2 )        //if multi packet supported, and must be greater than MAX_PACKET_SIZE
	{

		// Get the raw buffer and size of the current message
		if (PASS != pLabel->GetContentsBuffer(m_pucSourceData, m_uiNumSourceChars))
		{
			bPacketModified = false;
			return bPacketModified;
		}

		m_uiCharsLeft = m_uiNumSourceChars;		//set the number of characters not yet placed in a packet
		m_uiDestinationSize = (m_uiNumSourceChars * 3) + 200 ;		//get a destination buffer with some head room, 3x for escape chars, 200 extra for multi packet info

		if( m_uiDestinationSize > 15000 )      //Pdf max size at this point is 2710, so we should not reach this, but just in case,
		{
			m_uiDestinationSize = 15000;        //limit size per BufferSizes_t BufferList[], which leaves us with 8 barcode buffers available to hold this size.
		}

		pDestinationLabel = CBarcodeData::GetSaveNewBuffer(m_pucDestinationData, m_uiDestinationSize);

		m_pucDestinationCursor = m_pucDestinationData;         // start at the beginning of destination buffer
		m_pucSourceCursor = m_pucSourceData;			      // start at the beginning of source buffer

		//save the barcode type info to the new buffer
		pDestinationLabel->CopyTypeInfo( pLabel );

		//now do packet mode specific additions to destination packet
		ParseMultiPacket();

		// ensure the new lenght info is correct
		pDestinationLabel->FinalizeBuffer( m_uiTotalBarcodeDataDestinationSize );

		// now excange the container objects, saving back to the original barcode data
		pLabel->CopyTypeInfo( pDestinationLabel );
		pLabel->MoveBuffer( pDestinationLabel );

		if( pDestinationLabel != NULL)
		{
			pDestinationLabel->Free();  	// distroy the temporary barcode data
		}
	}
	else
	{
		bPacketModified = false;         //did not fit any packet mode type, error
	}

	return bPacketModified;
}



//this builds the multipacket barcode data packet
void CFlatBedParser::ParseMultiPacket(void)
{
	bool bLastPacket;
	int i;
	size_t uiFinishLocation;
	size_t uiSizeToTest;

	size_t uiCurrentPacketSize;					  // size of data in current packet including escapes
	bchar_t *pucCurrentDataSource;              //location of current source packet data
	size_t uiActualCountPacketSize;             //numbers of char in packet not counting escapes
	bchar_t *pucCurrentPacketStart;             //Marks the start position of the current packet

	m_uiTotalBarcodeDataDestinationSize = 0;   //initialize total count of data in destination buffer

	do
	{
		pucCurrentPacketStart = m_pucDestinationCursor;       //save beginning location of packet
		bLastPacket = false;                                      //indicator for last packet

		//set the current packets data size, which includes escapes
		uiCurrentPacketSize = (unsigned int) MAX_PACKET_SIZE;

		if( m_uiCharsLeft <= MAX_PACKET_SIZE )
		{
			uiCurrentPacketSize = m_uiCharsLeft;
			bLastPacket = true;
		}

		switch( m_PacketMode )	// select parameters for each packet mode
		{
			case 2:
			default:
			{
				pucCurrentDataSource = m_pucSourceCursor;

				if( !bLastPacket )            //do not want to split up escape characters, so shorten data packet if needed, adjust size
				{
					if( (0xfe == pucCurrentDataSource[MAX_PACKET_SIZE-2]) )
					{
						uiCurrentPacketSize = uiCurrentPacketSize - 2;
					}
					else if( (0xfe == pucCurrentDataSource[MAX_PACKET_SIZE-1]) )
					{
						uiCurrentPacketSize = uiCurrentPacketSize - 1;
					}

					m_uiCharsLeft = m_uiCharsLeft - uiCurrentPacketSize;
				}

				uiActualCountPacketSize = 0;               //initialize non escape character data count

				for( i=0; i<uiCurrentPacketSize; i++)      //find actual data count for packet (not counting  the escapes)
				{
					if( (0xfe == pucCurrentDataSource[i]) )
					{
						i = i+2;
					}

					uiActualCountPacketSize++;
				}

				m_uiPacketActualDataCount = uiActualCountPacketSize;         //store the packet data count

				if( !bLastPacket )            //tag it for not last packet by setting upper bit (0x80)
				{
					m_uiPacketActualDataCount = m_uiPacketActualDataCount + 128;
				}

				*m_pucDestinationCursor = m_ucPacketBarcodeId;		//place the code ID as first prefix
				m_pucDestinationCursor++;
				m_uiTotalBarcodeDataDestinationSize++;

				if(m_uiPacketActualDataCount != 141)       //a data count of 13 and not last packet requires an escape (equal to 0x8d)
				{
			 		*m_pucDestinationCursor = m_uiPacketActualDataCount;
				}
				else     //we need to do an 0x8d escape for the data size of 13 and not last packet
				{
					*m_pucDestinationCursor = 0xfe;
					m_pucDestinationCursor++;
					m_uiTotalBarcodeDataDestinationSize++;
					*m_pucDestinationCursor = 0x08;
					m_pucDestinationCursor++;
					m_uiTotalBarcodeDataDestinationSize++;
					*m_pucDestinationCursor = 0x0d;
				}

				m_pucDestinationCursor++;
				m_uiTotalBarcodeDataDestinationSize++;

				for( i=0; i<uiCurrentPacketSize; i++)      //place data in destination buffer
				{
					*m_pucDestinationCursor = *m_pucSourceCursor;
					m_pucDestinationCursor++;
					m_uiTotalBarcodeDataDestinationSize++;
					m_pucSourceCursor++;
				}

				uiFinishLocation = (uiCurrentPacketSize + m_PacketPrefixSize) - 1;
				uiSizeToTest = uiCurrentPacketSize + m_PacketPrefixSize;
				m_LRC = CalcLrc( pucCurrentPacketStart, uiSizeToTest, 0, uiFinishLocation );      //calculate LRC
				*m_pucDestinationCursor = m_LRC;                                //place LRC
				m_pucDestinationCursor++;
				m_uiTotalBarcodeDataDestinationSize++;
				*m_pucDestinationCursor = 0x8d;               //the terminator

				if( !bLastPacket )            //if not the last packet, then update destination pointer
				{
					m_pucDestinationCursor++;
				}

				m_uiTotalBarcodeDataDestinationSize++;    //always update the count

				break;
			}           // end case 2
		}             // end switch
	}while( !bLastPacket );
}



// This simple function gets the barcode ID info for the packet
void CFlatBedParser::GetBarcodeId(CBarcodeData *&pLabel)
{
	bchar_t	ucMessageHhpBarcodeId;

	ucMessageHhpBarcodeId = pLabel->GetHHPId();

	if( (ucMessageHhpBarcodeId >= ID_START_POSITION) && (ucMessageHhpBarcodeId <= ID_END_POSITION) )
	{
		unsigned int barcodeIdPosition = (unsigned int) (ucMessageHhpBarcodeId - ID_START_POSITION);
		m_ucPacketBarcodeId = codeIdConversion[barcodeIdPosition];
	}
	else		//not valid ID
	{
		m_ucPacketBarcodeId = (unsigned char) BAD_TRY;
	}
}



// This simple function adds the Prefix info to the packet
void CFlatBedParser::AddPacketPrefix( CBarcodeData *&pLabel )
{
	// size and data pointer variables
	bchar_t *pReturnedInsertPos;
	size_t uiEditSize;

	bchar_t *pDestinationPointer;

	uiEditSize = m_PacketPrefixSize;

	if(uiEditSize > 0)		               // only if there is something to add
	{
		// check buffer size and enlarge it if required
		if ( pLabel->MakeSpace(pReturnedInsertPos, 0, uiEditSize) )
		{
			pDestinationPointer = pReturnedInsertPos;

			switch( m_PacketMode )	// select parameters for each packet mode
			{
				case 2:
				default:
					*pDestinationPointer = m_ucPacketBarcodeId;
					pDestinationPointer++;
					*pDestinationPointer = (unsigned char) m_uiPreEscapeLength;
					break;
			}
		}
	}
}


// This simple function adds the Suffix info to the packet
void CFlatBedParser::AddPacketSuffix( CBarcodeData *&pLabel )
{
	// size and data pointer variables
	bchar_t *pReturnedInsertPos;    //pointer to location to add data
	size_t uiEditSize;

	bchar_t *pDestinationPointer;

	uiEditSize = m_PacketSuffixSize;

	if(uiEditSize > 0)		               // only if there is something to add
	{
		// check buffer size and enlarge it if required
		if ( pLabel->CanAddElements(uiEditSize) )
		{
			if (pLabel->GetAddrOfElement(pReturnedInsertPos, UINT_MAX))  // get the next free spot
			{
				pDestinationPointer = pReturnedInsertPos;

				switch( m_PacketMode )	// select parameters for each packet mode
				{
					case 2:
					default:
						*pDestinationPointer = m_LRC;
						pDestinationPointer++;
						*pDestinationPointer = 0x8d;               //the terminator
				}

				pLabel->FinalizeAddingElements(uiEditSize);	// tell how many we added
			}
		}
	}
}



/* -------------------------------------------------
	AddEscapeCodes()
	Function removes all characters of value 0x8D (term char) and 0xFE (Escape char) from the data message.

	Inputs:
 	*pLabel points to a barcode object.

	Outputs:
 	Modifys the barcode object with three character substitution of the characters above:

	Oxfe will be replaced by string: 0xfe 0x0f 0x0e
	Ox8d will be replaced by string: 0xfe 0x08 0x0d

	Barcode data size is increased but packet data size is not updated when adding escape char.
 --------------------------------------------------*/
void CFlatBedParser::AddEscapeCodes( CBarcodeData *&pLabel )
{
	bchar_t	*pSrc, *pDst;
	size_t	uiSrcLength, uiDstLength;
	bchar_t *pReturnedInsertPos;
	size_t uiEditSize;

	m_bFoundMinCharSize = false;     //indicates that we found the minimum data size
	uiEditSize = 2;           //always adding 2 more characters when doing substitution
	uiDstLength = 0;          //zero out destination size

	if ( PASS == pLabel->GetContentsBuffer(pSrc, uiSrcLength) )
	{
		pDst = pSrc;
		m_uiPreEscapeLength = uiSrcLength;            //save length before escapes are added

		if( (m_uiPreEscapeLength >= m_MinDataSize) )  // min characters input?
		{
			m_bFoundMinCharSize = true;    //then set true

			for (/* no init */; uiSrcLength > 0; --uiSrcLength, ++pSrc )
			{
   			if( (0x8d != *pSrc) && (0xfe != *pSrc) )
   			{
      			*pDst = *pSrc;
					pDst++;
      			uiDstLength++;
   			}
				else if( (0x8d == *pSrc) )
   			{
      			*pDst = 0xfe;             //replace first byte
      			uiDstLength++;
					pLabel->MakeSpace(pReturnedInsertPos, uiDstLength, uiEditSize);       //make room by moving rest of data
					pDst = pReturnedInsertPos;            //new location to insert data
					pSrc = pDst;
					*pDst = 0x08;           //add second byte
					pDst++;
					pSrc++;
      			uiDstLength++;
					*pDst = 0x0d;           //add third byte
					pDst++;
      			uiDstLength++;
   			}
				else if( (0xfe == *pSrc) )
   			{
      			*pDst = 0xfe;             //replace first byte
      			uiDstLength++;
					pLabel->MakeSpace(pReturnedInsertPos, uiDstLength, uiEditSize);   //make room by moving rest of data
					pDst = pReturnedInsertPos;                 //new location to insert data
					pSrc = pDst;
					*pDst = 0x0f;                     //add second byte
					pDst++;
					pSrc++;
      			uiDstLength++;
					*pDst = 0x0e;              //add third byte
					pDst++;
      			uiDstLength++;
   			}
			}    // end for loop
		}      //end if size check passed

		//pLabel->FinalizeBuffer(uiDstLength);            //set final data size
		m_uiPostEscapeLength = uiDstLength;        //save length after escapes are added
	}	//end if got contents buffer
}




//*****************************************************************
// bool CFlatBedParser:: SelectAuxMode()
//
// Sets min char data size, prefix size, and suffix size of class variables.
//
// IF AUX MODE 2, LRC IS AT AUXBUF+AUX_CHRNO-2, MIN CHAR# =5
// IF AUX MODE 3, LRC IS AT AUXBUF+AUX_CHRNO-2, MIN CHAR# =3
// IF AUX MODE 4, LRC IS AT AUXBUF+AUX_CHRNO-2, MIN CHAR# =6
// IF AUX MODE 5, LRC IS AT AUXBUF+AUX_CHRNO-3, MIN CHAR# =4
// IF AUX MODE 6, LRC IS AT AUXBUF+AUX_CHRNO-4, MIN CHAR# =5
// IF AUX MODE 8, LRC IS AT AUXBUF+AUX_CHRNO-3 (2 byte LRC)
//	OTHER AUX MODES = (no lrc) = PASS!
//
//returns pass/fail
//
// *****************************************************************
bool CFlatBedParser::SelectAuxMode( void )
{
	bool bGotSizes = true;

	m_bFoundValidPacketMode = false;		//indicates that a packet mode type has been implemented and is supported

	switch( m_PacketMode )	// select parameters for each packet mode
	{
		case 2:
			m_PacketSuffixSize = 2;          // packet suffix size = 2, min LRC char size = 5
			m_LrcMinDataSize = 5;
			m_PacketPrefixSize = 2;        //packet prefix size is also 2 in this case, so min barcode data size is actually: (5 - 2 = 3)
			m_bFoundValidPacketMode = true;
			break;
		case 3:
			m_PacketSuffixSize = 2;          // packet suffix size = 2, min LRC char size = 3
			m_LrcMinDataSize = 3;
			m_PacketPrefixSize = 2;        //packet prefix size is also 2 in this case, so min barcode data size is actually: (3 - 2 = 1)
			break;
		case 4:
			m_PacketSuffixSize = 2;          // packet suffix size = 2, min LRC char size = 6
			m_LrcMinDataSize = 6;
			m_PacketPrefixSize = 2;        //packet prefix size is also 2 in this case, so min barcode data size is actually: (6 - 2 = 1)
			break;
		case 5:
			m_PacketSuffixSize = 3;          // packet suffix size = 3, min LRC char size = 4
			m_LrcMinDataSize = 4;
			m_PacketPrefixSize = 2;        //packet prefix size is also 2 in this case, so min barcode data size is actually: (4 - 2 = 2)
			break;
		case 6:
			m_PacketSuffixSize = 4;          // packet suffix size = 4, min LRC char size = 5
			m_LrcMinDataSize = 5;
			m_PacketPrefixSize = 2;        //packet prefix size is also 2 in this case, so min barcode data size is actually: (5 - 2 = 3)
			break;
		default:
			bGotSizes = false;	//failure
	}

	m_MinDataSize = m_LrcMinDataSize - m_PacketPrefixSize;        //min size of data only

	return bGotSizes;
}


//*****************************************************************
// int CFlatBedParser:: CalcLrc( bchar_t	*pData, size_t	uiDataLength, size_t uiStartCheck, size_t uiFinishCheck )
//
//	Calculates the LRC of the packet to be sent.
// This includes barcode data and prefix, which should already be added.
//	The LRC location in packet depends on the ML aux mode used.
// Verifies min char size.
//
// pass in:
// *pData is the pointer to the start of the entire data buffer you wish to do the calc on
// uiDataLength is the number of characters to do the calc on
// uiStartCheck is the location of the first character to modify
// uiFinishCheck is the location of the last character to modify
//
// Returns the LRC
//
// *****************************************************************
unsigned int CFlatBedParser::CalcLrc( bchar_t *pData, size_t uiDataLength, size_t uiStartCheck, size_t uiFinishCheck )
{
	int k;
	unsigned int lrc;
	unsigned int uiDataLengthCheck;
	bchar_t	*pTempData;

	pTempData = &pData[uiStartCheck];       //mark the start
	lrc = 0x00;
	uiDataLengthCheck = (uiFinishCheck - uiStartCheck) + 1;

	if( (uiDataLength >= m_LrcMinDataSize)  && (uiDataLength == uiDataLengthCheck) )  // min characters input?
	{
		for( k=uiStartCheck; k <= uiFinishCheck; k++ )	// define lrc
		{
			if( (0xfe != *pTempData) )
			{
				lrc = lrc ^ *pTempData;	// calculate lrc
				pTempData++;         //move to next char
			}
			else if( (0xfe == *pTempData) )
			{
				if( (pTempData[1] == 0x08) && (pTempData[2] == 0x0d) )       //if a duplicate of the terminating char
				{
					lrc = lrc ^ 0x8d;	// calculate lrc with original character
				}
				else if( (pTempData[1] == 0x0f) && (pTempData[2] == 0x0e) )    //if a duplicate of the escape char
				{
					lrc = lrc ^ 0xfe;	// calculate lrc with original character
				}

				pTempData++;   //move to sub char 1
				pTempData++;   //move to sub char 2
				pTempData++;   //move to next char
			}
		}
	}

	return lrc;
}

//*****************************************************************
// bchar_t CFlatBedParser::GetTermPacketChar( void )
//
//	Gets the packet termination character, dependent on packet mode
//
// pass in:
// nothing
//
// Returns the packet terminating character,
// else returns 0x00 if not set
//
// *****************************************************************
bchar_t CFlatBedParser::GetTermPacketChar( void )
{
	bchar_t ucTheTerminator;

	switch( m_PacketMode )	// select parameters for each packet mode
	{
		case 0:
			ucTheTerminator = 0x00;               //no terminator
			break;
		case 2:
			ucTheTerminator = 0x8d;               //the terminator for packet mode 2
			break;
		default:
			ucTheTerminator = 0x00;               //no terminator
	}

	return ucTheTerminator;
}



