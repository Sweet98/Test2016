/* ==============================================================================
   ST3800 Data Editor Module Header
   ==============================================================================

   $RCSfile: Gen6FoundationApp/FoundationApp/dataedit.h $
   $Revision: 1.11 $
   $Date: 2011/08/24 11:28:17EDT $

   ==============================================================================

   ============================================================================== */

#ifndef _DATAEDIT_H_
#define _DATAEDIT_H_

#include "stdInclude.h"

/*	Defines */
//#define PREFIX	((unsigned char) 1)				/* Specifies prefix mode to function add_prefix_suffix() */
//#define SUFFIX	((unsigned char) !PREFIX)		/* Specifies suffix mode to function add_prefix_suffix() */
#define FORMAT_ESCAPE_CHARACTER		'\\'		/* Format string escape character is a backslash */
#define SUB_WA_ID								0x80	/* Substitute Welch Allyn Code ID */
#define SUB_AIM_ID							0x81	/* Substitute AIM Code ID */
#define SUB_PORT_ID							0x82	/* Substitute Port ID */
#define SUB_CMD_KEY_NUMBER					0xB5	/* Substitute Key number */
#define SUB_CMD_DELAY_TIME					0xB6	/* Substitute delay time in mSec */
#define SUB_CMD_INSERT_UNICODE			0xB7	/* Insert unicode char */
#define UNIVERSAL_WA_ID						0x99	/* The key to match any Code ID is 0x99 hex */
#define UNIVERSAL_CODE_LENGTH				9999	// The key to match any message length
#define UNIVERSAL_TERMINAL_ID				0x63	/* The key to match any TerminalID is 99 decimal (0x63 hex) */
#define SIZEOF_SUPPRESS_BUFR				15		/* Size of buffer to hold suppression characters */
#define SIZEOF_REPLACE_BUFR				30		/* Size of buffer to hold replacement characters */

#define DELAY_ESCAPE_CHARACTER			0xEF01   // is in the private use area

#define WEDGE_ESC_SEQUENCES_SUPPORTED 1  // I wait for the final implementation util we have unicode  (Dieter)
#ifdef WEDGE_ESC_SEQUENCES_SUPPORTED
   #define WEDGE_ESCAPE_CHARACTER         0xF001   // is in the private use area
   #define SUB_KEY_NUMBER                 0xF003   // Key number
   #define SUB_KEY_NUMBER_MAKE            0xF004   // Key number (make key)
   #define SUB_KEY_NUMBER_BREAK           0xF005   // key number (break key)
   #define SUB_SCANCODE                   0xF006   // scancode (direct byte)
   #define SUB_DELAY_TIME                 0xF007   // delay time in mSec
   #define SUB_CTRLASCII_MODE                    0xF008   // KBDCAS3 support for Ctrl+AscII prefix/suffix
   #define ENDOF_FIX       0X01   // KBDCAS3 support for prefix/suffix to turn Ctrl+AscII translation ON
   #define STARTOF_FIX     0x02   // KBDCAS3 support for prefix/suffix to turn Ctrl+AscII translation ON
   #define TEST_DELAY   0x07D0
#endif

/*	Structures */
struct data_edit_options_t
{
	unsigned int	RemoveFunctionCodes : 1;
	unsigned int	AddPrefix :1;
	unsigned int	AddSuffix :1;
    unsigned int    AddFactorySuffix :1;
	unsigned int	ApplyDataFormat :1;
	unsigned int	RequireDataFormat :1;

	unsigned int 	NoMatchError :1;
	unsigned int	ParticularSymbol :1;
	unsigned int	SpecifiedDefaultKey;

};

/* A structure to hold the keys when searching for parameter strings */
//enum key_type {PREFIX_SUFFIX_STRING, DATA_FORMAT_STRING};

struct KEYS
{
	int alternate_format;
	int term_id;
	int code_id;
	int code_length;
};

typedef struct _CODE_NAMES
{
	bchar_t 		code_type_id;
	const char	*barcode_name;
	int 			count;
}CODE_NAMES_T;


class PREFIX_SUFFIX;

class CBarcodeData;
class HSTRING;

class CDataEdit
{
public:
	CDataEdit(void);
	static void CreateDataEditor(void);

	bool DataEdit(CBarcodeData *&pLabel);
	void GetDataEditSettingsFromMenu(void);

	bool SetPrimaryFormat(void);
	bool SetAlternateFormatOne(void);
	bool SetAlternateFormatTwo(void);
	bool SetAlternateFormatThree(void);

#ifdef RFBASE
	void RemoveEscapeSequenceOfCommandEF(CBarcodeData *pLabel);
#endif

protected:
// other functions
	void Base64Encode(CBarcodeData *pLabel);

	void Base64Decode(CBarcodeData *pLabel);

	void RemoveFunctionCodes(CBarcodeData *pLabel);

// pre/suffix
	void AddPrefix(CBarcodeData *&pLabel);
	void AddSuffix(CBarcodeData *&pLabel);
   PREFIX_SUFFIX * DetermineSuffix(void);
// helpers for pre/suffix
	// Get the size and the editstring
	size_t GetSizeAndEditString(PREFIX_SUFFIX *pPreSuffix, int CodeId, HSTRING *&pEditHString);
	// write the pre/suffix to it's destination
	void AddXfix(CBarcodeData *pLabel, tichar_t *pDestination, size_t XFixSize, HSTRING *pEditHString );

// formatter
	bool DataFormat( CBarcodeData *&pLabel );

// helpers for formatter
	HSTRING *FindFormatterString(KEYS *key, int &Position);
	bool FindFormatAndTry(KEYS *key, CBarcodeData *&pLabel);
	void TryThisDataFormat(HSTRING *pFormat, CBarcodeData *&pLabel);

// common
	bool CheckDestinationSize(void);
	int IncludeCharacter(void);
	tichar_t CopyCharacter(bchar_t *pSource);
	void CopyCharacter(tichar_t *pDestination, bchar_t *pSource);
	void CopyString(tichar_t *pDestination, bchar_t *pSource, size_t size);
	bool CompareString(tichar_t *pString1, tichar_t *pString2, size_t size);
	UINT GetDecimalLengthFromFormat(unsigned char rawlength);
	UINT GetDecimalLengthFromFormat(unsigned char rawlength100, unsigned char rawlength);

protected:
// data members
	data_edit_options_t m_Options;
	int m_TermID;
	bool m_good_format;                			/* True if the format was applied successfully to input string */
	bool m_abort_all_formats;
	bool m_no_format_match;						/*0, no match; 1, match*/
	int m_ucAlternateFormat;						/*	0 = Primary Format Key; N = N Format Key	*/
	size_t m_number_of_suppression_characters;	/* Count of the characters used in the suppression filter */
	size_t m_number_of_replacement_characters;	/* Count of the characters used in the replacement filter */
	tichar_t m_suppression_characters[SIZEOF_SUPPRESS_BUFR];
	tichar_t m_replacement_characters[SIZEOF_REPLACE_BUFR];

	tichar_t *m_pucCursor;                		/* pointer to the next character in the input string */
	tichar_t *m_pNewPtr;								/* pointer to next empty location in the output string */
	UINT m_NumSourceChars;          				/* Number of characters in the source buffer */
	UINT m_chars_left;          					/* Number of characters between the cursor and the end of buffer */

	tichar_t *m_pFormattedSymbol;					// new buffer
	size_t m_DestinationSize;						// size of new buffer
};

extern   CDataEdit  	 	*g_pDataEditor;
#define  theDataEditor	(*g_pDataEditor)

// compatibility with old code
// This might be removed after the old code has been fixed
//#define DATA_EDIT_OPTIONS data_edit_options_t

bool SetPrimaryFormat(void);
bool SetAlternateFormatOne(void);
bool SetAlternateFormatTwo(void);
bool SetAlternateFormatThree(void);

#endif




