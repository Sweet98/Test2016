/*=================================================================================
  class: CFlatBedParser

//=================================================================================
   $Source: interfaces/MLFlatbedSerialParser.h $
   $Date: 2009/12/17 02:32:49EST $
   $Revision: 1.1 $


//=================================================================================*/

#ifndef FLAT_BED_PARSER_
#define FLAT_BED_PARSER_

class CBarcodeData;

class CFlatBedParser
{
public:
	~CFlatBedParser();

	static void CreateFlatBedParser(void);   //class factory
	static void DestroyFlatBedParser(void);

	bool FormatBarcodeData( CBarcodeData *&pLabel, unsigned int uiPacketModeType );

	bchar_t GetTermPacketChar( void );

// internal methods
protected:

	CFlatBedParser();                 // protected constructor to force people to use the create class factory function.

	void ParseMultiPacket(void);
	void GetBarcodeId( CBarcodeData *&pLabel );
	void AddPacketPrefix( CBarcodeData *&pLabel );
	void AddPacketSuffix( CBarcodeData *&pLabel );
	void AddEscapeCodes( CBarcodeData *&pLabel );
	bool SelectAuxMode( void );
	unsigned int CalcLrc( bchar_t *pData, size_t uiDataLength, size_t uiStartCheck, size_t uiFinishCheck );

//internal variables
private:
	//flatbed packet info
	int m_PacketSuffixSize;                 //packet suffix size
	int m_PacketPrefixSize;                 //suffix prefix size
	int m_LrcMinDataSize;                 // prefix + data min size
	int m_MinDataSize;                    // min data alone size
	int m_PacketMode;                     //packet type

	//valid packet mode check
	bool	m_bFoundValidPacketMode;

	//min char check
	bool	m_bFoundMinCharSize;

	//barcode id
	bchar_t	m_ucPacketBarcodeId;          //barcode ID for the packet

	//actual number of charaters in packet not including escapes
	unsigned int m_uiPacketActualDataCount;

	//the LRC calculation
	unsigned int m_LRC;

	//starting data sizes
	size_t	m_uiPreEscapeLength;
	size_t	m_uiPostEscapeLength;

	//barcode data source info
	unsigned int m_uiNumSourceChars;   				// Number of characters in the source barcode data buffer
	bchar_t *m_pucSourceData;					      // data pointer of source barcode data buffer

	unsigned int m_uiCharsLeft;          							// Number of characters left that have not been placed in a packet
	bchar_t *m_pucSourceCursor;                					// pointer to the next character in the input string


	//barcode data destination info
	size_t m_uiDestinationSize;					  // size of new barcode data destination buffer
	bchar_t *m_pucDestinationData;					  // data pointer of new barcode data destination buffer

	bchar_t *m_pucDestinationCursor;								// pointer to next empty location in the output string
	size_t m_uiTotalBarcodeDataDestinationSize;           //the final size of the new barcode data

};

//Global pointer to class factory instance
extern CFlatBedParser *g_pFlatBedParser;
#define theFlatBedParser	(*g_pFlatBedParser)

#endif // FLAT_BED_PARSER_




