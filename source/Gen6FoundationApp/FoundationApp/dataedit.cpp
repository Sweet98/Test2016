/* ==============================================================================
   ST3800 Data Editor Module Source
   ==============================================================================

   $RCSfile: Gen6FoundationApp/FoundationApp/dataedit.cpp $
   $Revision: 1.35 $
   $Date: 2011/08/24 11:53:18EDT $

   ==============================================================================

   ============================================================================== */

/*#define _DEBUG 1*/

#include "stdInclude.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "dataedit.h"
#include "menu.h"
#include "BarcodeDataMan.h"
#include "IoMan.h"
#include "HWString.h"
#include "TransCodeEx.h"
#include "thread.h"
#include "readerconfiguration.h"
#include "if_table.h"
#include "wedgelabel.h"
#include "TermId.h"

#if defined(_DEBUG)
// #include <debug.c>
#endif


#if(0)
#define XTRACE(x)							theStdInterface.Write(x);
#define XTRACE1(a,b)						theStdInterface.printf(a,b);
#define XTRACE4(x,a1,a2,a3,a4)		theStdInterface.printf(x,a1,a2,a3,a4);
#define XTRACE5(x,a1,a2,a3,a4,a5)	theStdInterface.printf(x,a1,a2,a3,a4,a5);

#define YTRACE(x)							theStdInterface.Write(x);
#define YTRACE1(a,b)						theStdInterface.printf(a,b);
#define YTRACE4(x,a1,a2,a3,a4)		theStdInterface.printf(x,a1,a2,a3,a4);
#define YTRACE5(x,a1,a2,a3,a4,a5)	theStdInterface.printf(x,a1,a2,a3,a4,a5);
#else
#define XTRACE(x)
#define XTRACE1(a,b)
#define XTRACE4(x,a1,a2,a3,a4)
#define XTRACE5(x,a1,a2,a3,a4,a5)

#define YTRACE(x)
#define YTRACE1(a,b)
#define YTRACE4(x,a1,a2,a3,a4)
#define YTRACE5(x,a1,a2,a3,a4,a5)
#endif
CODE_NAMES_T g_barcode_name_table[] =
{
	{0x41, "Australian Post", 15},
	{0x7A, "Aztec Code", 10},
	{0x5A, "Aztec Mesas", 11},
	{0x47, "BC412", 5},
	{0x42, "British Post", 12},
	{0x43, "Canadian Post", 13},
	{0x70, "Channel Code", 12},
	{0x51, "China Post", 10},
	{0x48, "China Sensible Code", 19},
	{0x61, "Codabar", 7},
	{0x71, "Codablock F", 11},
	{0x68, "Code 11", 7},
	{0x6A, "Code 128", 8},
	{0x49, "GS1-128", 7},
	{0x6F, "Code 16K", 8},
	{0x3C, "Code 32 Pharmaceutical", 22},
	{0x62, "Code 39", 7},
	{0x6C, "Code 49", 7},
	{0x69, "Code 93", 7},
	{0x31, "Code One", 8},
	{0x75, "Code Z",6},
	{0x77, "Data Matrix", 11},
	{0x2E, "DotCode", 7},
	{0x64, "EAN-13", 6},
	{0x44, "EAN-8", 5},
	{0x58, "Grid Matrix Code", 16},
	{0x79, "GS1 DataBar Omni", 16},
	{0x7b, "GS1 DataBar Limited", 19},
	{0x7d, "GS1 DataBar Expanded", 20},
	{0x4E, "ID-tag", 6},
	{0x65, "Interleaved 2 of 5", 18},
	{0x4A, "Japanese Post", 13},
	{0x4B, "KIX (Netherlands) Post", 22},
	{0x3F, "Korea Post", 10},
	{0x46, "Label Code", 10},
	{0x6D, "Matrix 2 of 5", 13},
	{0x78, "MaxiCode", 8},
	{0x52, "MicroPDF417", 11},
	{0x67, "MSI", 3},
	{0x4F, "OCR", 3},
	{0x72, "PDF417", 6},
	{0x4C, "Planet Code", 11},
	{0x6E, "Plessey Code", 12},
	{0x57, "PosiCode", 8},
	{0x50, "Postnet", 7},
	{0x73, "QR Code", 7},
	{0x53, "SecureCode", 10},
	{0x66, "Straight 2 of 5", 15},
	{0x54, "TCIF Linked Code 39", 19},
	{0x74, "Telepen", 7},
	{0x3D, "Trioptic Code", 13},
	{0x55, "Ultracode", 9},
	{0x63, "UPC-A", 5},
	{0x45, "UPC-E", 5},
	{0x76, "Vericode", 8},
	{0x99, "UNKNOWN", 7}
};

extern const model_t DummyModel = 0xffff ;
CDataEdit::CDataEdit(void)
{
// to do
	m_ucAlternateFormat = -1;		/*	0 = Primary Format Key; N = N Format Key; -1 = Use Menu Command	*/
	m_number_of_suppression_characters = 0;
	m_number_of_replacement_characters = 0;
	m_abort_all_formats=false;
}

//base64 decode code
const char base[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
//char* base64_encode(const char* data, int data_len);
char *base64_decode(const char* data, int data_len);
static char find_pos(char ch);
/* */
#if 1
char *base64_encode(const char* data, int data_len)
{
//int data_len = strlen(data);
int prepare = 0;
int ret_len;
int temp = 0;
char *ret = NULL;
char *f = NULL;
int tmp = 0;
char changed[4];
int i = 0;
ret_len = data_len / 3;
temp = data_len % 3;
if (temp > 0)
{
   ret_len += 1;
}
ret_len = ret_len*4 + 1;
ret = (char *)malloc(ret_len);

if ( ret == NULL)
{
   printf("No enough memory.\n");
   exit(0);
}
memset(ret, 0, ret_len);
f = ret;
while (tmp < data_len)
{
   temp = 0;
   prepare = 0;
   memset(changed, '\0', 4);
   while (temp < 3)
   {
    //printf("tmp = %d\n", tmp);
    if (tmp >= data_len)
    {
     break;
    }
    prepare = ((prepare << 8) | (data[tmp] & 0xFF));
    tmp++;
    temp++;
   }
   prepare = (prepare<<((3-temp)*8));
   //printf("before for : temp = %d, prepare = %d\n", temp, prepare);
   for (i = 0; i < 4 ;i++ )
   {
    if (temp < i)
    {
     changed[i] = 0x40;
    }
    else
    {
     changed[i] = (prepare>>((3-i)*6)) & 0x3F;
    }
    *f = base[changed[i]];
    //printf("%.2X", changed[i]);
    f++;
   }
}
*f = '\0';

return ret;

}
#endif
/* */
static char find_pos(char ch) 
{
char *ptr = (char*)strrchr(base, ch);//the last position (the only) in base[]
return (ptr - base);
}
/* */
char *base64_decode(const char *data, int data_len, int *length)
{
int ret_len = (data_len / 4) * 3;
int equal_count = 0;
char *ret = NULL;
char *f = NULL;
int tmp = 0;
int temp = 0;
char need[3];
int prepare = 0;
int i = 0;
if (*(data + data_len - 1) == '=')
{
   equal_count += 1;
}
if (*(data + data_len - 2) == '=')
{
   equal_count += 1;
}
if (*(data + data_len - 3) == '=')
{//seems impossible
   equal_count += 1;
}
switch (equal_count)
{
case 0:
   ret_len += 4;//3 + 1 [1 for NULL]
   break;
case 1:
   ret_len += 4;//Ceil((6*3)/8)+1
   break;
case 2:
   ret_len += 3;//Ceil((6*2)/8)+1
   break;
case 3:
   ret_len += 2;//Ceil((6*1)/8)+1
   break;
}
*length = ret_len;
ret = (char *)malloc(ret_len);
if (ret == NULL)
{
   printf("No enough memory.\n");
   exit(0);
}
memset(ret, 0, ret_len);
f = ret;
while (tmp < (data_len - equal_count))
{
   temp = 0;
   prepare = 0;
   memset(need, 0, 4);
   while (temp < 4)
   {
    if (tmp >= (data_len - equal_count))
    {
     break;
	 }
	 prepare = (prepare << 6) | (find_pos(data[tmp]));
	 temp++;
	 tmp++;
	}
	prepare = prepare << ((4-temp) * 6);
	for (i=0; i<3 ;i++ )
	{
	 if (i == temp)
	 {
	  break;
	 }
	 *f = (char)((prepare>>((2-i)*8)) & 0xFF);
	 f++;
	}
 }
 *f = '\0';
 XTRACE1("Decode:%s ***\r\n",f);
 
 return ret;
 }
 //end of base64 decode code

#if defined(SM5130)
void BTTrans(CBarcodeData *&pLabel)
{
	// Get the raw buffer and size of the current message
	tichar_t *pucMessage;
	tichar_t *pMessage;
	UINT NumSourceChars;
	UINT DestinationSize;
	UINT i=0;
	if (PASS != pLabel->GetContentsBuffer(pucMessage, NumSourceChars))
		return;

	CBarcodeData *pDestinationLabel;

	DestinationSize = NumSourceChars + 4;
	pDestinationLabel = CBarcodeData::GetSaveNewBuffer(pMessage, DestinationSize);
	
//theStdInterface.printf("1   Num of source characters, %d, 0x%x\r\n",NumSourceChars,(NumSourceChars & 0xFF));

	pMessage[0] = 0x02;
	pMessage[1] = NumSourceChars>>8;
	pMessage[2] = NumSourceChars & 0xFF;
	//theStdInterface.printf("2   Num of source characters, %d, 0x%x, 0x%x\r\n",NumSourceChars,(NumSourceChars & 0xFF),pMessage[2]);
	for(i=0; i<NumSourceChars; i++)
	{
		pMessage[i+3] = pucMessage[i];
	}
	pMessage[NumSourceChars+3] = 0x03;
	pMessage[NumSourceChars+4] = 0x0;

	
	pDestinationLabel->CopyTypeInfo(pLabel);
	pLabel->Free(); 	// this is the "old" source message which gets replaced by the formatted message
	pLabel = pDestinationLabel; 		// do the replace action
	pLabel->FinalizeBuffer(NumSourceChars+4);	// ensure the lenght info is correct
}
#endif

/* -----------------------------------------------------

 	The DataEdit() function.  Includes RemoveFunctionCodes(),
 	DataFormat() and add_prefix_suffix() processing.

	DataEdit()
	This function calls the RemoveFunctionCodes(), DataFormat() and add_prefix_suffix() editing functions,
 	if enabled, in a prescribed sequence.  The sequence is important since the presence of function codes and
 	prefix/suffix characters in the message string effect the outcome of data formatting.

	Inputs:
	Just the CBarcodeData *pLabel object pointer
	The rest is handled internally.

	Outputs:
 	Modifys the CBarcodeData object.
 	Returns TRUE if Data Formatting was disabled or was successful or was not required.

	Typical Useage:

		result = theDataEditor.DataEdit(gpCurrentDecoded);
 		if ( result )
		{
			GoodReadBeep();
			SendMessage();
		}
		else
		{
			ErrorBeep();
			DiscardMessage();
		}
  --------------------------------------------------*/
bool CDataEdit::DataEdit(CBarcodeData *&pLabel)
{
	bool result=false;
	m_no_format_match = 0;
	if (pLabel->GetHHPId()==WA_CODELETTER_MENU_RESPONSE)
		return true;
	GetDataEditSettingsFromMenu();
	
	if(GET_SETTING(base64enable)==TRUE)
		Base64Encode(pLabel);

	/*	Sequence Step 1: Remove the function codes	*/
	if ( m_Options.RemoveFunctionCodes == 1 )
	{
		RemoveFunctionCodes(pLabel);
	}

	/*	Sequence Step 2: Apply data formatting	*/
	/*	Always return TRUE if data formatting is disabled or not required.	*/
	if ( m_Options.ApplyDataFormat == 1 )
	{
		result = DataFormat(pLabel);
		if ( result && m_Options.ParticularSymbol == 1 )
		{
			m_Options.AddPrefix = 0;
			m_Options.AddSuffix = 0;

		}
		if ( m_Options.RequireDataFormat == 0 )
			result = true;
	}
	else
	{
		result = true;
	}

	/*	Sequence Step 3: Add prefix	*/
	if ( m_Options.AddPrefix == 1 )
	{
		AddPrefix(pLabel);
	}

	/*	Sequence Step 4: Add suffix	*/
	if (( m_Options.AddSuffix == 1 ) || ( m_Options.AddFactorySuffix == 1 ))
	{
		AddSuffix(pLabel);
	}
/*	if((m_no_format_match == 0)&&(GET_SETTING(NoMatchError) == 0)&&((GET_SETTING(DataFormatEnable) == 2)||(GET_SETTING(DataFormatEnable) == 4)))
	{
		result = false;
	}
//	XTRACE1("DATA EDIT Result is : %d\r\n",result);*/
#if defined(SM5130)
	if(GET_SETTING(TerminalID)==0)
	{
		BTTrans(pLabel);
	}
#endif

	return (result);
}

/* -------------------------------------------------
	Base64Encode()
	Function removes all characters in the range of 0x00 to 0x1f and 0x7f (ASCII
 	function characters) from the data message.

	Inputs:
 	*pLabel points to a barcode object.

	Outputs:
 	Modifys the barcode object
 --------------------------------------------------*/
void CDataEdit::Base64Encode( CBarcodeData *pLabel )
{
	tichar_t	*pSrc;
	char		*pDst;
	char		*pStr;
	size_t	n, uiNewLength;
	int *length = NULL;
	int i = 0;
	if ( PASS == pLabel->GetContentsBuffer(pSrc, n) )
	{
		uiNewLength = 0;

		pStr = (char *)malloc(n+1);
		memset(pStr, 0, n+1);
		for(i=0;i<n;i++)
		{
			pStr[i] = pSrc[i];
		}

		pDst = base64_encode((const char *)pStr,n);
		uiNewLength = strlen((const char *)pDst);
		for(i=0;i<uiNewLength;i++)
		{
			*pSrc = *pDst;
			pSrc++;
			pDst++;
		}
		*pSrc = '\0';

		pLabel->FinalizeBuffer(uiNewLength);
	}
}

/* -------------------------------------------------
	Base64Decode()
	Function removes all characters in the range of 0x00 to 0x1f and 0x7f (ASCII
 	function characters) from the data message.

	Inputs:
 	*pLabel points to a barcode object.

	Outputs:
 	Modifys the barcode object
 --------------------------------------------------*/
void CDataEdit::Base64Decode( CBarcodeData *pLabel )
{
	tichar_t	*pSrc, *pDst;
	size_t	n, uiNewLength;
	int *length = NULL;
	int i = 0;
	if ( PASS == pLabel->GetContentsBuffer(pSrc, n) )
	{
		pDst = pSrc;
		uiNewLength = 0;

		XTRACE("\r\npSrc:");
		for(i=0;i<n;i++)
		{
			XTRACE1("%c",pSrc[i]);
		}
		XTRACE("\r\n");
		pDst = (tichar_t *)base64_decode((const char *)pSrc,n,length);
		XTRACE1("pDst:%s\r\n",pDst);
		uiNewLength = *length;
		pLabel->FinalizeBuffer(uiNewLength);
	}
}

/* -------------------------------------------------
	RemoveFunctionCodes()
	Function removes all characters in the range of 0x00 to 0x1f and 0x7f (ASCII
 	function characters) from the data message.

	Inputs:
 	*pLabel points to a barcode object.

	Outputs:
 	Modifys the barcode object
 --------------------------------------------------*/
void CDataEdit::RemoveFunctionCodes( CBarcodeData *pLabel )
{
	tichar_t	*pSrc, *pDst;
	size_t	n, uiNewLength;

	if ( PASS == pLabel->GetContentsBuffer(pSrc, n) )
	{
		pDst = pSrc;
		uiNewLength = 0;

		for (/* no init */; n > 0; --n, ++pSrc )
		{
   		if ( !iscntrl( *pSrc ) )
   		{
      		*pDst++ = *pSrc;
      		uiNewLength++;
   		}
		}
		pLabel->FinalizeBuffer(uiNewLength);
	}
}

void CDataEdit::CopyCharacter(tichar_t *pDestination, bchar_t *pSource)
{
#ifdef UNICODE_FOR_INTERFACES
	*pDestination = theTransCode.SystemCPCharacterToUnicode(*pSource);
#else
	*pDestination = *pSource;
#endif
}

tichar_t CDataEdit::CopyCharacter(bchar_t *pSource)
{
#ifdef UNICODE_FOR_INTERFACES
	return theTransCode.SystemCPCharacterToUnicode(*pSource);
#else
	return *pSource;
#endif
}

void CDataEdit::CopyString(tichar_t *pDestination, bchar_t *pSource, size_t size)
{
#ifdef UNICODE_FOR_INTERFACES
	size_t DummySize=size;
	theTransCode.SystemCPStringToUnicode(pSource, size, pDestination, DummySize);
#else
	memcpy(pDestination, pSource, size);
#endif
}
bool CDataEdit::CompareString(tichar_t *pString1, tichar_t *pString2, size_t size)
{
	UINT i =0;
	for(i=0; i<size; i++)
	{
		if(*(pString1+i)!=*(pString2+i))
		{
			return FALSE;
		}
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//! Retrieve the length in decimal (2 digits)
/*!
 \param rawlength 2 digits from formatter data
 \return decimal equivalent of the 2 digits in the format
*/
UINT CDataEdit::GetDecimalLengthFromFormat(unsigned char rawlength)
{
	UINT digit1  = rawlength&0x0f;	 // this is not correct, but in use since ages, so for now I keep it.
	UINT digit10 = (rawlength>>4)*10;
	return digit10+digit1;
}

///////////////////////////////////////////////////////////////////////////////
//! Retrieve the length in decimal (4 digits)
//!
/*!
 \param rawlength100 2 digits from formatter data
 \param rawlength 2 digits from formatter data
 \return decimal equivalent of the 4 digits in the format
*/
UINT CDataEdit::GetDecimalLengthFromFormat(unsigned char rawlength100, unsigned char rawlength)
{
	return GetDecimalLengthFromFormat(rawlength100)*100 + GetDecimalLengthFromFormat(rawlength);
}

/*
	This function searches the pre od suffix string based on the HHP code ID or the universal code ID.
	Then it finds the correct lenght after any expansion.
	Returns that lenght and the pointer to the pre/suffix.

	The FORMAT_ESCAPE_CHARACTER followed by command byte in the parameter string causes special
 	handling, such as substituting Code ID into the prefix or suffix.
 */
size_t CDataEdit::GetSizeAndEditString(PREFIX_SUFFIX *pPreSuffix, int CodeId, HSTRING *&pEditHString)
{
	size_t XFixSize = 0;

	/*	Look through the parameter block for a match with the specifc Code ID.  If one is not
	found then look again for a match with the universal Code ID.  */
	pEditHString = pPreSuffix->FindCommandString (CodeId);

	if( NULL == pEditHString )	         // not found, try universal
	{
		pEditHString = pPreSuffix->FindCommandString (UNIVERSAL_WA_ID);
	}

	/*	If a match was found...  */
	if (NULL != pEditHString)
	{
		/*  Look through the parameter string for FORMAT ESCAPE CHARACTERS and determine the number
			of characters that will be added to the message, keeping in mind that some characters,
			such as AIM ID, expand to multiple characters when added.  */
		size_t	i;
		XFixSize = pEditHString->Size;
		bchar_t *pucEditString =  pEditHString->Char;
		bchar_t *pucTemp = pucEditString;

		for ( i = XFixSize; i > 0; i-- )
		{
			if ( *pucTemp++ == FORMAT_ESCAPE_CHARACTER )
			{
				XFixSize--;				/* The FORMAT_ESCAPE_CHARACTER itself does not get added to the message */
				i--;

				/*	This function assumes that each escape sequence will result in a single character being
					included in the final prefix or suffix.  For any escape sequence that does not, a case
					statement must be placed here to adjust EditSize accordingly.	*/
				switch ( *pucTemp++ )
				{
					case SUB_AIM_ID:
						XFixSize += 2;	/* Aim ID will expand to three in the message */
						break;
					case SUB_CMD_KEY_NUMBER:
						{
							size_t count = GetDecimalLengthFromFormat(*pucTemp++); // keystroke inserts do contain a count info
							size_t inputsize = 1+1+(count*2);    	// amount in bytes (cmd,count, keystrokes)
							size_t outputsize = (1+1+1) * count;	// amount in Unicode (esc, cmd, keystroke) * count
							pucTemp += count*2;
							if(pucTemp <= (pEditHString->Char + pEditHString->Size)) // user might have entered a size which is too big
							{
								XFixSize += outputsize-inputsize;
								i-=(inputsize-1);	                   // -1 because the for loop already subs one
							}
							else
							{
								i=0;			// Abort the loop
							}
						}
						break;
					case SUB_DELAY_TIME:
						XFixSize += 3-1-1;								// time command will be 3 unicode chars
						pucTemp++;											// skip the time parameter
						i--;
						break;
					case SUB_CMD_INSERT_UNICODE:
						XFixSize -= 3-1;									// only one unicode char in output (less than XFix size)
						pucTemp+=2;											// skip the 2 bytes for the unicode
						i-=2;
						break;
					default:
						break;
				}
				if(i == 0)
					break;
 			}
			/* KBDCAS3 - make room for the 'WEDGE_ESCAPE_CHARACTERs' support to turn off/on
			 * Ctrl+AscII translation in wedge interfaces*/
			if (IOCRET_YES == theStdInterface.DeviceIoControl(IOCTRL_NEED_PRESUFFIX_ESC))
			{
			   XFixSize += 6;    // adjust for the expansion
			}
		}
	}
	return XFixSize;
}

// Add the pre/suffix
// XFixSize is already the size of the final expanded XFix.
//	The FORMAT_ESCAPE_CHARACTER followed by command byte in the parameter string causes special
// 	handling, such as substituting Code ID into the prefix or suffix.

void CDataEdit::AddXfix(CBarcodeData *pLabel, tichar_t *pDestination, size_t XFixSize, HSTRING *pEditHString )
{
	bchar_t *pEditString = pEditHString->Char;
   bool flag_ctrlascii = false;
	/*	Add the new characters, processing escape sequences as they are encountered. */
	while (XFixSize-- > 0)
	{
		if ( *pEditString == FORMAT_ESCAPE_CHARACTER )
		{
 			pEditString++;
			switch ( *pEditString )
			{
				case SUB_WA_ID:
					*pDestination = pLabel->GetHHPId();
					break;
				case SUB_AIM_ID:
					*pDestination++ = ']';		/*	AIM ID starts with ']'	*/
					*pDestination++ =  pLabel->GetAimId();
					*pDestination	 =  pLabel->GetAimModifier();
					XFixSize -= 2;					/*	adjust for the expansion	*/
					break;
				case SUB_CMD_KEY_NUMBER:
					{
						pEditString++;
						size_t count = GetDecimalLengthFromFormat(*pEditString++);	// the keystroke inserts do contain a count info
						int rest = pEditHString->Size - (pEditString - pEditHString->Char);
						if((count*2) <= rest)  // user might have entered a size which is too big
						{
							for(int i=0; i<count; i++)
							{
								*pDestination++ = WEDGE_ESCAPE_CHARACTER;
								*pDestination++ = SUB_KEY_NUMBER;
								wchar_t Key;
								Key  = *pEditString++ <<8;
								Key |= *pEditString++ &0xff;
								*pDestination++ = Key;
								XFixSize -= 3;								//	adjust for the expansion
							}
							pEditString--;	                	// we must point to the last byte of the current command
							pDestination--;
							XFixSize++;
						}
						else
						{
							// We print just some amount of '?', so the user might get a clue.
							// This is better than just aborting the xfix.
							// Abort any further processing of xfix.
							XFixSize++;
							while (XFixSize-- > 0)
							{
								*pDestination++ = '?';
							}
							XFixSize=0;
						}
					}
					break;
				case SUB_CMD_DELAY_TIME:
					*pDestination++ = WEDGE_ESCAPE_CHARACTER;	// The escape for the wedge drivers
					*pDestination++ = SUB_DELAY_TIME;
					pEditString++;
					*pDestination = GetDecimalLengthFromFormat(*pEditString);
					XFixSize -= 3-1-1;								//	adjust for the expansion
					break;
				case SUB_CMD_INSERT_UNICODE:
					{
						wchar_t uni;
						pEditString++;
						uni  = *pEditString++ <<8;
						uni |= *pEditString &0xff;
						*pDestination = uni;
					}
					break;
				default:
					CopyCharacter(pDestination, pEditString);
					break;
			}
		}
      /* KBDCAS3 - insert the 'WEDGE_ESCAPE_CHARACTERs' support to turn off/on
       * Ctrl+AscII translation in wedge interfaces*/
      else if (IOCRET_YES == theStdInterface.DeviceIoControl(IOCTRL_NEED_PRESUFFIX_ESC))
      {
         /* First time through add the 'ESC' chars to turn OFF Ctrl+AscII translation
            and add the first prefix/suffix character*/
         if(flag_ctrlascii == false)
         {
            *pDestination++ = WEDGE_ESCAPE_CHARACTER; // The escape for the wedge drivers
            *pDestination++ = SUB_CTRLASCII_MODE;     // Set Ctrl+ASCII mode
            *pDestination++ = STARTOF_FIX;            // Turn OFF Ctrl+ASCII translation
            XFixSize -= 3;                            // adjust for the expansion
            CopyCharacter(pDestination, pEditString);
            flag_ctrlascii = true;
         }
         /* After all prefix/suffix characters are added, add the 'ESC' chars to turn ON Ctrl+AscII translation */
         else if(XFixSize == 2)
         {
            *pDestination++ = WEDGE_ESCAPE_CHARACTER; // The escape for the wedge drivers
            *pDestination++ = SUB_CTRLASCII_MODE;     // Set Ctrl+ASCII mode
            *pDestination++ = ENDOF_FIX;              // Turn ON Ctrl+ASCII translation
            XFixSize -= 2;                            // adjust for the expansion
         }
         /* Add the second through last prefix/suffix characters */
         else
         {
            CopyCharacter(pDestination, pEditString);
         }
      }  // End KBDCAS3 section.

		else
		{
			CopyCharacter(pDestination, pEditString);
		}
		pEditString++;
		pDestination++;
	}
}

// This simple function adds the Prefix
void CDataEdit::AddPrefix(CBarcodeData *&pLabel)
{
    HSTRING *pEditString;
    size_t uiEditSize = GetSizeAndEditString(GET_SETTING(PrefixObj), pLabel->GetHHPId(), pEditString);

    if(uiEditSize > 0)		               // only if there is something to add
    {
	   // check buffer size and enlarge it if required
	   tichar_t *pReturnedInsertPos;
	   if ( pLabel->MakeSpace(pReturnedInsertPos, 0, uiEditSize) )
	   {
			AddXfix(pLabel, pReturnedInsertPos, uiEditSize, pEditString );
	   }
    }
}

// This simple function adds the Suffix
void CDataEdit::AddSuffix(CBarcodeData *&pLabel)
{
    // get size and details what to do
    HSTRING *pEditString;
    size_t uiEditSize;

    uiEditSize = GetSizeAndEditString(CDataEdit::DetermineSuffix(), pLabel->GetHHPId(), pEditString);

    if(uiEditSize > 0)		               // only if there is something to add
    {
        // check buffer size and enlarge it if required

        tichar_t *pReturnedInsertPos;
        if ( pLabel->CanAddElements(uiEditSize) )
        {
            if (pLabel->GetAddrOfElement(pReturnedInsertPos, UINT_MAX))  // get the next free spot
            {
                AddXfix(pLabel, pReturnedInsertPos, uiEditSize, pEditString );
                pLabel->FinalizeAddingElements(uiEditSize);	// tell how many we added
            }
        }
    }
}

/* -------------------------------------------------
    DetermineSuffix()
    This function sets the the Suffix to be used
    SUFKEY is the default for Keyboard interfaces when SUFFAC is enabled
    SUFCOM is the default for Com interfaces when SUFFAC is enabled
    SUFBK2 is used when the defaults are not enabled

    Inputs:
    none

    Outputs:
    none

    Returns pointer to the Suffix which should be used.
 --------------------------------------------------*/
PREFIX_SUFFIX * CDataEdit::DetermineSuffix(void)
{
   PREFIX_SUFFIX *SuffixObj_ToBeAdded = GET_SETTING(SuffixObj);

    if(m_Options.AddFactorySuffix == 1)
    {
        if((GET_SETTING(CblSelNumber)==CBL_USB))
        {
            if(GET_SETTING(TermUSBID) == TRM_USB_KBD_PC)
            {
                SuffixObj_ToBeAdded = GET_SETTING(KeyboardSuffixObj);
            }
            else if(GET_SETTING(TermUSBID) == TRM_USB_COMEMU)
            {
                SuffixObj_ToBeAdded = GET_SETTING(ComSuffixObj);
            }
        }
        else if(GET_SETTING(CblSelNumber)==CBL_KBW)
        {
            if(GET_SETTING(TermKBWID) == TRM_IBM_PC_AT)
            {
                SuffixObj_ToBeAdded = GET_SETTING(KeyboardSuffixObj);
            }
        }
        else if(GET_SETTING(CblSelNumber)==CBL_RS232)
        {
            if(GET_SETTING(Term232ID) == AUX_RS232)
            {
                SuffixObj_ToBeAdded = GET_SETTING(ComSuffixObj);
            }
        }
    }
    else
    {
       SuffixObj_ToBeAdded = GET_SETTING(SuffixObj);
    }

    return  SuffixObj_ToBeAdded;
}

/* -------------------------------------------------
	DataFormat()
 	This function applies data formatting to the given decoded string.  The priority order in which
 	formats will be applied is:
		1) Actual Term ID, Actual Code ID, Actual Length
		2) Actual Term ID, Actual Code ID, Universal Length
		3) Actual Term ID, Universal Code ID, Actual Length
		4) Actual Term ID, Universal Code ID, Universal Length
		5) Universal Term ID, Actual Code ID, Actual Length
		6) Universal Term ID, Actual Code ID, Universal Length
		7) Universal Term ID, Universal Code ID, Actual Length
		8) Universal Term ID, Universal Code ID, Universal Length

	Inputs:
 	*pLabel points to a barcode object.

	Outputs:
 	Modifys the barcode object

	Returns TRUE if format was succesfully applied.
 --------------------------------------------------*/
bool CDataEdit::DataFormat(CBarcodeData *&pLabel)
{
	KEYS		key;							/* Table holding the search keys (term id, code id, length) */

	XTRACE("CDataEdit::DataFormat\r\n")
	m_good_format = FALSE;
	m_abort_all_formats = false;


	/*	Search the format command strings in the prescribed order for satisfied keys.  Execute each
	 * command string that is found until a good format (no format abort) is attained.	*/

	// -------------------------------------------------------------------
	XTRACE("CDataEdit 1: Actual Term ID, Actual Code ID, Actual Length\r\n");
	if(m_ucAlternateFormat == -1)
	{
		key.alternate_format = GET_SETTING(SpecifiedDefaultKey); //Get Format from Menu Command
	}
	else
	{
		key.alternate_format = m_ucAlternateFormat;	//Use temporary format from VSAF commands
	}

	key.term_id = m_TermID;
	key.code_id = pLabel->GetHHPId();
	key.code_length = pLabel->GetLabelSize();

	if (FindFormatAndTry(&key, pLabel))
		return (m_good_format);

	// -------------------------------------------------------------------
	XTRACE("CDataEdit 2: Actual Term ID, Actual Code ID, Universal Length\r\n");
	key.code_length = UNIVERSAL_CODE_LENGTH;

	if (FindFormatAndTry(&key, pLabel))
		return (m_good_format);

	// -------------------------------------------------------------------
	XTRACE("CDataEdit 3: Actual Term ID, Universal Code ID, Actual Length\r\n");
	key.code_id = UNIVERSAL_WA_ID;
	key.code_length = pLabel->GetLabelSize();

	if (FindFormatAndTry(&key, pLabel))
		return (m_good_format);

	// -------------------------------------------------------------------
	XTRACE("CDataEdit 4: Actual Term ID, Universal Code ID, Universal Length\r\n");
	key.code_length = UNIVERSAL_CODE_LENGTH;

	if (FindFormatAndTry(&key, pLabel))
		return (m_good_format);

	// -------------------------------------------------------------------
	XTRACE("CDataEdit 5: Universal Term ID, Actual Code ID, Actual Length\r\n");
	key.term_id = UNIVERSAL_TERMINAL_ID;
	key.code_id = pLabel->GetHHPId();
	key.code_length = pLabel->GetLabelSize();

	if (FindFormatAndTry(&key, pLabel))
		return (m_good_format);

	// -------------------------------------------------------------------
	XTRACE("CDataEdit 6: Universal Term ID, Actual Code ID, Universal Length\r\n");
	key.code_length = UNIVERSAL_CODE_LENGTH;

	if (FindFormatAndTry(&key, pLabel))
		return (m_good_format);

	// -------------------------------------------------------------------
	XTRACE("CDataEdit 7: Universal Term ID, Universal Code ID, Actual Length\r\n");
	key.code_id = UNIVERSAL_WA_ID;
	key.code_length = pLabel->GetLabelSize();

	if (FindFormatAndTry(&key, pLabel))
		return (m_good_format);

	// -------------------------------------------------------------------
	XTRACE("CDataEdit 8: Universal Term ID, Universal Code ID, Universal Length\r\n");
	key.code_length = UNIVERSAL_CODE_LENGTH;

	if (FindFormatAndTry(&key, pLabel))
		return (m_good_format);

	/*	Automatically clear AlternateFormat so next call will use primary formats	*/
	m_ucAlternateFormat = -1;
	return ( m_good_format );
}

// A helper to make the code more readable
// We search a format command string and try to use it for the current label.
// If it fails, we search for the next command string (same key == multiple formats at same key!!!) and try this one.
// We do this until there are no command strings anymore, or we've got a succesfully format
bool CDataEdit::FindFormatAndTry(KEYS *key, CBarcodeData *&pLabel)
{
	int Position = -1;                     // start with the very first command string (if there are more than one)
	do
	{
		Position++;	                        // try next position
		HSTRING *pFormat = FindFormatterString(key, Position);  // Position is a reference here !!
		if( NULL != pFormat)
		{
			m_no_format_match = 1;
			TryThisDataFormat(pFormat, pLabel);
		}
	}
	while ((Position >= 0) && !m_good_format && !m_abort_all_formats);

	if(m_good_format)
	{
		m_ucAlternateFormat = -1;
	}
	return (m_good_format || m_abort_all_formats);
}

// call the menu system to help with the details.
HSTRING *CDataEdit::FindFormatterString(KEYS *key, int &Position)
{
	DATA_FORMAT	*pFormat = GET_SETTING(DataFormatObj);
	HSTRING *pReturn	 = pFormat->FindCommandString (key->alternate_format, key->term_id, key->code_id, key->code_length, Position);

	if (pReturn == NULL)
	{
		XTRACE("CDataEdit::FindNextDataEditString: none found\r\n")
	}
	return (pReturn);
}

/*****************************************************************************/
void CDataEdit::TryThisDataFormat(HSTRING *pFormat, CBarcodeData *&pLabel)
{
	tichar_t character;
	UINT count = 0;

	UINT count1 = 0;
	UINT barcode_length = 0;
	UINT k = 0;

	bool format_not_done = TRUE;

	bool bCompare;

	tichar_t character_to_match;

	tichar_t *pSearchStr;
	tichar_t BarcodeLen[4];


	XTRACE("Entering formatter\r\n")
	m_good_format = TRUE;

	bchar_t *pucEditString = pFormat->Char;
	bchar_t *pucFormatEnd = &pucEditString[pFormat->Size];

	// Get the raw buffer and size of the current message
	tichar_t *pucMessage;
	if (PASS != pLabel->GetContentsBuffer(pucMessage, m_NumSourceChars))
		return;

	m_chars_left = m_NumSourceChars;

	barcode_length = m_NumSourceChars;

	// Get a destination barcode object with a suitable size
	m_DestinationSize = m_NumSourceChars+500;	// get a destination buffer with some head room
	CBarcodeData *pDestinationLabel;
	pDestinationLabel = CBarcodeData::GetSaveNewBuffer(m_pFormattedSymbol, m_DestinationSize);

	m_number_of_suppression_characters = 0;
	m_number_of_replacement_characters = 0;
	m_pFormattedSymbol[0] = '\0';	// ensure a 0 termination

	m_pNewPtr = m_pFormattedSymbol;
	m_pucCursor = pucMessage;			      // start at the beginning

	while ( TRUE == m_good_format && TRUE == format_not_done && false == m_abort_all_formats )
	{
		switch (*pucEditString++)
		{
			case 0xf1:		/* F1XX  --  Include all characters from cursor followed by XX */

				/*	Include the characters	*/
				while (m_chars_left && m_good_format)
				{
					if ( FAIL == IncludeCharacter() )
					{
						m_good_format = FALSE;
						XTRACE("Err: F1a\r\n")
					}
				}

				/*	Add the XX character, if not 0x00 and there is room	*/
				if (*pucEditString)
				{
					if (FAIL == CheckDestinationSize())
					{
						XTRACE("Err: F1b\r\n")
					}
					else
					{
						CopyCharacter(m_pNewPtr, pucEditString);
						m_pNewPtr++;
					}
				}

				pucEditString++;
				break;

			case 0xf2:		/* F2NNXX --  Include NN characters from cursor followed by XX */

				count = GetDecimalLengthFromFormat(*pucEditString++);

				/*	Check for NN characters available	*/
				if (count > m_chars_left)
				{
					m_good_format = FALSE;
					XTRACE("Err: F2a\r\n")
				}
				else
				{
					/*	Include the characters	*/
					while (count-- && m_good_format)
					{
						if ( FAIL == IncludeCharacter() )
						{
							m_good_format = FALSE;
							XTRACE("Err: F2b\r\n")
						}
					}

					/*	Include XX if not equal to 0x00 and their is room	*/
					if (*pucEditString)
					{
						if (FAIL == CheckDestinationSize())
						{
							XTRACE("Err: F2c\r\n")
						}
						else
						{
							CopyCharacter(m_pNewPtr, pucEditString);
							m_pNewPtr++;
						}
					}
				}

				pucEditString++;
				break;

			case 0xf3:		/* F3SSXX  --  Include from cursor up to but not including SS followed by XX */

				character_to_match = CopyCharacter(pucEditString);
				pucEditString++;

				/*	Include the characters	*/
				while (character_to_match != *m_pucCursor && m_chars_left && m_good_format)
				{
					if ( FAIL == IncludeCharacter() )
					{
						m_good_format = FALSE;
						XTRACE("Err: F3a\r\n")
					}
				}

				if (m_chars_left == 0)
				{
					m_good_format = FALSE;
					XTRACE("Err: F3b\r\n")
				}

				/*	Add the XX character, if not 0x00 and there is room	*/
				if (*pucEditString)
				{
					if (FAIL == CheckDestinationSize())
					{
						XTRACE("Err: F3c\r\n")
					}
					else
					{
						CopyCharacter(m_pNewPtr, pucEditString);
						m_pNewPtr++;
					}
				}

				pucEditString++;
				break;

			case 0xe9:		/* E9NN --  Include all but last NN characters from cursor */

				count = GetDecimalLengthFromFormat(*pucEditString++);

				/*	Check for NN+1 characters available	*/
				if (count >= m_chars_left)
				{
					m_good_format = FALSE;
					XTRACE("Err: E9a\r\n")
				}
				else
				{
					/*	Include the characters	*/
					count = m_chars_left - count;
					while (count-- && m_good_format)
					{
						if ( FAIL == IncludeCharacter() )
						{
							m_good_format = FALSE;
							XTRACE("Err: E9b\r\n")
						}
					}
				}
				break;

			case 0xf4:		/* F4XXNN  --  Insert XX character NN times */

				character = CopyCharacter(pucEditString);
				pucEditString++;
				count = GetDecimalLengthFromFormat(*pucEditString++);

				/*	Insert the characters	*/
				while (count && m_good_format)
				{
					if (FAIL == CheckDestinationSize())
					{
						XTRACE("Err: F4\r\n")
					}
					else
					{
						*m_pNewPtr++ = character;
						count--;
					}
				}
				break;

			case 0xf5:		/* F5NN  --  Move cursor ahead NN characters */

				count = GetDecimalLengthFromFormat(*pucEditString++);

				if (count<=m_chars_left)
				{
					m_pucCursor += count;
					m_chars_left -= count;
				}
				else
				{
					m_good_format = FALSE;
					XTRACE("Err: F5\r\n")
				}
				break;

			case 0xf6:		/* F6NN  --  Move cursor back NN charaters */

				count = GetDecimalLengthFromFormat(*pucEditString++);

				m_pucCursor -= count;
				m_chars_left += count;

				if (FAIL == CheckDestinationSize())
				{
					XTRACE("Err: F6\r\n")
				}
				break;

			case 0xf7:		/* F7 --  Move cursor to the start of the input message */

				m_pucCursor = pucMessage;
				m_chars_left = m_NumSourceChars;
				break;

			case 0xea:		/* EA --  Move cursor to the end of the input message */

				m_pucCursor = pucMessage + m_NumSourceChars - 1;
				m_chars_left = 1;
				break;

			case 0xf8:		/* F8XX  --  Move cursor ahead to XX character */

				character = CopyCharacter(pucEditString);
				while (m_chars_left && *m_pucCursor != character)
				{
					m_pucCursor++;
					m_chars_left--;
				}

				if (m_chars_left == 0)
				{
					m_good_format = FALSE;
					XTRACE("Err: F8\r\n")
				}

				pucEditString++;
				break;

			case 0xf9:		/* F9XX  --  Move cursor back to XX character */

				character = CopyCharacter(pucEditString);
				while ( (m_chars_left <= m_NumSourceChars) && (*m_pucCursor != character) )
				{
					m_pucCursor--;
					m_chars_left++;
				}

				if (FAIL == CheckDestinationSize())
				{
					XTRACE("Err: F9\r\n")
				}

				pucEditString++;
				break;

			case 0xe6:		/* E6XX  --  Move cursor ahead to non-XX character */

				character = CopyCharacter(pucEditString);
				while (m_chars_left && *m_pucCursor == character)
				{
					m_pucCursor++;
					m_chars_left--;
				}

				if (m_chars_left == 0)
				{
					m_good_format = FALSE;
					XTRACE("Err: E6\r\n")
				}

				pucEditString++;
				break;

			case 0xe7:		/* E7XX  --  Move cursor back to non-XX character */

				character = CopyCharacter(pucEditString);
				while ( (m_chars_left <= m_NumSourceChars) && (*m_pucCursor == character) )
				{
					m_pucCursor--;
					m_chars_left++;
				}

				if (FAIL == CheckDestinationSize())
				{
					XTRACE("Err: E7\r\n")
				}

				pucEditString++;
				break;

			case 0xfb:		/* FBNNXXYY..ZZ Suppress up to 15 different chars until FC is entered */

				count = 0;
				m_number_of_suppression_characters = *pucEditString++;
				m_number_of_suppression_characters = ((m_number_of_suppression_characters >> 4) * 10) + (m_number_of_suppression_characters & 0x0f);

				if ( m_number_of_suppression_characters <= sizeof(m_suppression_characters) )
				{
					CopyString(m_suppression_characters, pucEditString, m_number_of_suppression_characters);
					pucEditString += m_number_of_suppression_characters;
//					while ( count < (UINT) m_number_of_suppression_characters )
//					{
//						CopyCharacter(&m_suppression_characters[count++], pucEditString);
//						pucEditString++;
//					}
				}

				break;

			case 0xfc:		/* FC  --  Disable character suppression */

				m_number_of_suppression_characters = 0;
				break;

			case 0xe4:		/* E4NNXXxxYYyy..ZZzz Replace up to 15 different chars until E2 is entered */

				count = 0;
				m_number_of_replacement_characters = *pucEditString++ ;
				m_number_of_replacement_characters = ((m_number_of_replacement_characters >> 4) * 10) + (m_number_of_replacement_characters & 0x0f);

				if ( m_number_of_replacement_characters <= sizeof(m_replacement_characters) )
				{
					CopyString(m_replacement_characters, pucEditString, m_number_of_replacement_characters);
					pucEditString += m_number_of_replacement_characters;
//					while ( count < (UINT) m_number_of_replacement_characters )
//					{
//						m_replacement_characters[count++] = *pucEditString++;
//						m_replacement_characters[count++] = *pucEditString++;
//					}
				}

				break;

			case 0xe5:		/* E5  --  Disable character replacement */

				m_number_of_replacement_characters = 0;
				break;

			case 0xfe:		/* FEXX  --  Compare character at cursor to XX, increment cursor */

				character = CopyCharacter(pucEditString);
				if ( *m_pucCursor == character )
				{
					m_pucCursor++;
					m_chars_left--;
				}
				else
				{
					m_good_format = FALSE;
					XTRACE("Err: FE\r\n")
					XTRACE1("Src ch: %c\r\n",*m_pucCursor)
					XTRACE1("Edit ch: %c\r\n",*pucEditString)
				}

				pucEditString++;
				break;

			case 0xec:		/* EC  --  Compare character at cursor to Numeric */

				if ( !isdigit(*m_pucCursor) )
				{
					m_good_format = FALSE;
					XTRACE("Err: EC\r\n")
				}

				break;

			case 0xed:		/* ED  --  Compare character at cursor to Non-Numeric */

				if ( isdigit(*m_pucCursor) )
				{
					m_good_format = FALSE;
					XTRACE("Err: ED\r\n")
				}

				break;

			case 0xef:		/* EFNNNN  --  Delay before sending any more output */
				/* Get delay counter  */
				count = GetDecimalLengthFromFormat(*pucEditString++, *pucEditString++);
				count *= 5;

				/* Insert the delay Esc characters   */
#if !defined( RFBASE ) && !defined( RFSCAN )
				if (IOCRET_YES == theStdInterface.DeviceIoControl(IOCTRL_SHALL_I_DELAY))
#endif
#ifdef RFSCAN
				if (!AmIDirectLinkToHost())
#endif
				{
					if (FAIL == CheckDestinationSize())
					{
						m_good_format = FALSE;
						XTRACE("Err: EFa\r\n")
					}
					else
					{
						*m_pNewPtr++ = DELAY_ESCAPE_CHARACTER; // The escape for interface delay
					}

					if (FAIL == CheckDestinationSize())
					{
						m_good_format = FALSE;
						XTRACE("Err: EFb\r\n")
					}
					else
					{
						*m_pNewPtr++ = (tichar_t)count;     // Set delay
					}
				}
				break;

#if 0	/* Commands E0, E1, E2 and E3 are not yet part of the 3800 */
			case 0xe0:		/* E0  --  insert Key-Code 1 character */

				*m_pNewPtr++ = Current_Param_Table.key_code_1_assignment;
				break;

			case 0xe1:		/* E1  --  insert Key-Code 2 character */

				*m_pNewPtr++ = Current_Param_Table.key_code_2_assignment;
				break;

			case 0xe2:		/* E2  --  insert Key-Code 3 character */

				*m_pNewPtr++ = Current_Param_Table.key_code_3_assignment;
				break;

			case 0xe3:		/* E3  --  insert Key-Code 4 character */

				*m_pNewPtr++ = Current_Param_Table.key_code_4_assignment;
				break;
#endif	/* Commands E0, E1, E2 and E3 are not yet part of the 3800 */

#if 0 //Cntrl+ASCII
		 case 0xee:     /* EE  --  Stop Control+ASCII translation (for wedge KBDCAS3 mode)  */

            if (IOCRET_YES == theStdInterface.DeviceIoControl(IOCTRL_NEED_PRESUFFIX_ESC))
            {
               /* Insert the wedge Esc characters   */
               if (FAIL == CheckDestinationSize())
               {
                  XTRACE("Err: F4\r\n")
               }
               else
               {
                  *m_pNewPtr++ = WEDGE_ESCAPE_CHARACTER; // The escape for the wedge drivers
                  *m_pNewPtr++ = SUB_CTRLASCII_MODE;     // Set Ctrl+ASCII mode
                  *m_pNewPtr++ = STARTOF_FIX;            // Turn OFF Ctrl+ASCII translation
               }
            }
            break;

         case 0xef:     /* EF  --  Start Control+ASCII translation (for wedge KBDCAS3 mode)  */

            if (IOCRET_YES == theStdInterface.DeviceIoControl(IOCTRL_NEED_PRESUFFIX_ESC))
            {
               /* Insert the wedge Esc characters   */
               if (FAIL == CheckDestinationSize())
               {
                  XTRACE("Err: F4\r\n")
               }
               else
               {
                  *m_pNewPtr++ = WEDGE_ESCAPE_CHARACTER; // The escape for the wedge drivers
                  *m_pNewPtr++ = SUB_CTRLASCII_MODE;     // Set Ctrl+ASCII mode
                  *m_pNewPtr++ = ENDOF_FIX;              // Turn ON Ctrl+ASCII translation
               }
            }
            break;
#endif //Cntrl+ASCII


			case 0xb0:				/*  B0NNNNS -- Search forward for matching string */
				count = GetDecimalLengthFromFormat(*pucEditString++, *pucEditString++);

				pSearchStr = (tichar_t *)MALLOC(count);
				CopyString(pSearchStr, pucEditString, count);
				bCompare = CompareString(pSearchStr,m_pucCursor,count);
				while ((m_chars_left>=count) && !bCompare)
				{
					m_pucCursor++;
					m_chars_left--;
					bCompare = CompareString(pSearchStr,m_pucCursor,count);
					XTRACE1("\r\n Compare Result is : %d\r\n",bCompare);
				}

				if (m_chars_left < count)
				{
					m_good_format = FALSE;
					XTRACE("Err: B0\r\n")
				}

				pucEditString+=count;
				FREE(pSearchStr);
				pSearchStr = NULL;
				break;

			case 0xb1:				/* B1NNNNS -- Search backward for matching string */
				count = GetDecimalLengthFromFormat(*pucEditString++, *pucEditString++);

				pSearchStr = (tichar_t *)MALLOC(count);
				CopyString(pSearchStr, pucEditString, count);
				while ((m_chars_left <= barcode_length) && !(CompareString(pSearchStr,m_pucCursor,count)))
				{
					m_pucCursor--;
					m_chars_left++;
				}

				if (m_chars_left > barcode_length)
				{
					m_good_format = FALSE;
					XTRACE("Err: B1\r\n")
				}

				pucEditString+=count;
				FREE(pSearchStr);
				pSearchStr = NULL;
				break;

			case 0xb2:				/* B2NNNNS -- Compare string */
				count = GetDecimalLengthFromFormat(*pucEditString++, *pucEditString++);

				pSearchStr = (tichar_t *)MALLOC(count);
				CopyString(pSearchStr, pucEditString, count);
				pucEditString += count;
				if(CompareString(pSearchStr,m_pucCursor,count))
				{
					m_pucCursor += count;
					m_chars_left -= count;
				}
				else
				{
					m_good_format = FALSE;
					XTRACE("Err: B2\r\n");
				}
				FREE(pSearchStr);
				pSearchStr = NULL;
				break;

			case 0xb3:				/* B3 -- Insert the symbology name in the output message */
				pLabel->ReportID();
				for(k=0; k<53;k++)
				{
					if(pLabel->m_Code_ID == g_barcode_name_table[k].code_type_id)
					{
						count = g_barcode_name_table[k].count;
						while (count&& m_good_format)
						{
							if (FAIL == CheckDestinationSize())
							{
								m_good_format = FALSE;
								XTRACE("Err: B3\r\n");
							}
							else
							{
								*m_pNewPtr++ = *(g_barcode_name_table[k].barcode_name+g_barcode_name_table[k].count-count);
								count--;
							}
						}
						break;
					}
				}

				if(k==53)
				{
					count = g_barcode_name_table[53].count;
					while (count&& m_good_format)
					{
						if (FAIL == CheckDestinationSize())
						{
							m_good_format = FALSE;
							XTRACE("Err: B3\r\n");
						}
						else
						{
							*m_pNewPtr++ = *(g_barcode_name_table[53].barcode_name+g_barcode_name_table[53].count-count);
							count--;
						}
					}
				}
				break;

			case 0xb4:				/* B4 -- Insert barcode length */
				if(barcode_length >= 10000)
				{
					XTRACE("Error\r\n");
				}
				else if(barcode_length >=1000)
				{
					count = 4;
					BarcodeLen[0] = barcode_length/1000 + 0x30;
					BarcodeLen[1] = (barcode_length%1000)/100 + 0x30;
					BarcodeLen[2] = (barcode_length%100)/10 + 0x30;
					BarcodeLen[3] = barcode_length%10 + 0x30;
				}
				else if(barcode_length >=100)
				{
					count = 3;
					BarcodeLen[0] = barcode_length/100 + 0x30;
					BarcodeLen[1] = (barcode_length%100)/10 + 0x30;
					BarcodeLen[2] = barcode_length%10 + 0x30;
				}
				else if(barcode_length >=10)
				{
					count = 2;
					BarcodeLen[0] = barcode_length/10 + 0x30;
					BarcodeLen[1] = barcode_length%10 + 0x30;
				}
				else
				{
					count = 1;
					BarcodeLen[0] = barcode_length + 0x30;
				}
				count1 = count;

				while (count && m_good_format)
				{
					if (FAIL == CheckDestinationSize())
					{
						m_good_format = FALSE;
						XTRACE("Err: B4\r\n")
					}
					else
					{
						*m_pNewPtr++ = BarcodeLen[count1-count];
						count--;
					}
				}

				break;

			case 0xb5:		/* B5NNMMKK... MM=modifier, KK=keynumber  --  insert keyboard sequences */
				count = GetDecimalLengthFromFormat(*pucEditString++);
				// check the destination size
				if((m_pNewPtr +count*(1+1+1)) <= (m_pFormattedSymbol + m_DestinationSize))
				{
					if(pucEditString+ count*(1+1) <= pucFormatEnd)
					{
						for(int i=0; i<count; i++)
						{
							*m_pNewPtr++ = WEDGE_ESCAPE_CHARACTER;
							*m_pNewPtr++ = SUB_KEY_NUMBER;
							character  = *pucEditString++ <<8;
							character |= *pucEditString++ &0xff;
							*m_pNewPtr++ = character;
						}
					}
				}
				else
				{
					XTRACE("CheckDestinationSize in B5: (m_pNewPtr >= ...)\r")
					m_good_format = FAIL;
				}
				break;


			case 0xb6:		/* B6  --  insert keyboard delay */
				// check the destination size
				if((m_pNewPtr +(1+1+1)) <= (m_pFormattedSymbol + m_DestinationSize))
				{
					if(pucEditString+ (1+1) <= pucFormatEnd)
					{
						*m_pNewPtr++ = WEDGE_ESCAPE_CHARACTER;
						*m_pNewPtr++ = SUB_DELAY_TIME;
						*m_pNewPtr++ = GetDecimalLengthFromFormat(*pucEditString++);
					}
				}
				else
				{
					XTRACE("CheckDestinationSize in B6: (m_pNewPtr >= ...)\r")
					m_good_format = FAIL;
				}
				break;
			case 0xb7:		/* B7  --  insert unicode char */
				// check the destination size
				if((m_pNewPtr +(1)) <= (m_pFormattedSymbol + m_DestinationSize))
				{
					if(pucEditString+ (1+1) <= pucFormatEnd)
					{
						character  = *pucEditString++ <<8;
						character |= *pucEditString++ &0xff;
						*m_pNewPtr++ = character;
					}
				}
				else
				{
					XTRACE("CheckDestinationSize in B7: (m_pNewPtr >= ...)\r")
					m_good_format = FAIL;
				}
				break;

			case 0xb8:  /* B8 - discard format */
				m_good_format = FALSE;
				m_abort_all_formats = true;  /*when you hit this token, stop everything, don't wnat to process this format token or any others */
				break;

 /*add commands EF,B0,B1,B2,B3,B4 by Jerry.06/11/2009*/
 /* add commands B5,B6B7 by Dieter fauth 2010.12.02*/
			case 0xff:		/* FF  --  End of Formatting */

				/*	The FF command is no longer needed, it was replaced by a compare of pointers
				*  to determine the end of a format.  This "do nothing" FF command is retained
				*  so that if the command is encountered it won't cause an unknown format error.	*/
				break;

			default:		/* Unknown data formatting command */
				m_good_format = FALSE;
				XTRACE1("Err: UNK %02X\r\n", pucEditString[-1])
				break;
		} /* switch */
		if ( (pucEditString >= pucFormatEnd) && (true == m_good_format) )
		{
   		format_not_done = FALSE;
		}
	} /* while */

	if ( m_good_format )
	{
		*m_pNewPtr = 0;	                  // add a terminating 0 (not really required, but good for debugging)
		// now excange the objects (this avoids a copy action)
		pDestinationLabel->CopyTypeInfo(pLabel);
		pLabel->Free();  	// this is the "old" source message which gets replaced by the formatted message
		pLabel = pDestinationLabel;			// do the replace action
		size_t NewLength = (size_t)(m_pNewPtr - m_pFormattedSymbol);
		pLabel->FinalizeBuffer(NewLength);	// ensure the lenght info is correct
	}
	else
	{
		pDestinationLabel->Free();	// clean up!!
	}
	XTRACE("Leaving formatter\r\n")
}

int CDataEdit::IncludeCharacter(void)
{
	int s = m_number_of_suppression_characters;
	int r = m_number_of_replacement_characters;

	if (FAIL == CheckDestinationSize())
	{
		XTRACE("IncludeCharacter: FAIL == CheckDestinationSize()\r")
		return FAIL;
	}

	/*	Do Replace if their are any replace characters to consider	*/
	if ( r )
	{
		/*	Search the replacement string for a match with the current character	*/
		while ( (r-=2) >= 0 && m_replacement_characters[r] != *m_pucCursor)
			;

		/*	If we found one replace it and exit	*/
		if ( r > -1 )
		{
			*m_pNewPtr++ = m_replacement_characters[r+1];
			m_pucCursor++;
			m_chars_left--;
			return PASS;
		}
	}

	/*	Do Suppress if their are any suppress characters to consider	*/
	if ( s )
	{
		/*	Search the suppression string for a match with the current character	*/
		while ( (--s) >= 0 && m_suppression_characters[s] != *m_pucCursor)
			;

		/*	If we found one just exit	*/
		if ( s > -1 )
		{
			m_pucCursor++;
			m_chars_left--;
			return PASS;
		}
	}

	/*	If we didn't replace it or suppress it then include the character	*/
//	XTRACE1("IncludeCharacter: %X\r", (UINT)*m_pucCursor)
	*m_pNewPtr++ = *m_pucCursor++;
	m_chars_left--;
	return PASS;
}

inline bool CDataEdit::CheckDestinationSize(void)
{
	if (m_pNewPtr >= m_pFormattedSymbol + m_DestinationSize)
	{
		XTRACE("CheckDestinationSize: (m_pNewPtr >= ...)\r")
		m_good_format = FAIL;
	}
	if (m_chars_left > m_DestinationSize)
	{
		XTRACE("CheckDestinationSize: (m_chars_left > m_DestinationSize)\r")
		m_good_format = FAIL;
	}
	return m_good_format;
}

/*------------------------------------------------------------------------------
SetAlternateFormatN()
------------------------------------------------------------------------------*/
bool CDataEdit::SetPrimaryFormat(void)
{
	m_ucAlternateFormat = 0;
	return PASS;
}
bool CDataEdit::SetAlternateFormatOne(void)
{
	m_ucAlternateFormat = 1;
	return PASS;
}

bool CDataEdit::SetAlternateFormatTwo(void)
{
	m_ucAlternateFormat = 2;
	return PASS;
}

bool CDataEdit::SetAlternateFormatThree(void)
{
	m_ucAlternateFormat = 3;
	return PASS;
}

// This is the list of data format commands that we accept

const char *SupportedFormats = "b0|b1|b2|b3|b4|b5|b6|b7|b8|e4|e5|e6|e7|e9|ea|ec|ed|ef|f1|f2|f3|f4|f5|f6|f7|f8|f9|fb|fc|fe|ff" ;

/*----------------------------------------------------------------------------
 * SendDataFormatCommands - This function prints out data format commands
 *   that are supported by this unit
 *----------------------------------------------------------------------------*/
bool SendDataFormatCommands(void)
{
	theStdInterface.Write(SupportedFormats);  // this does not work ok. the order of the response is wrong!! (Dieter)
															// But the IT3800 did do the same.
	return PASS;
}

void CDataEdit::GetDataEditSettingsFromMenu(void)
{
	m_TermID = GET_SETTING(TerminalID);
	m_Options.RemoveFunctionCodes = GET_SETTING(RemoveFncCodes);
	m_Options.AddPrefix = GET_SETTING(PrefixEnable);
	m_Options.AddSuffix = GET_SETTING(SuffixEnable);
    m_Options.AddFactorySuffix = GET_SETTING(SuffixFactoryEnable);
	m_Options.NoMatchError = GET_SETTING(NoMatchError);
	m_Options.SpecifiedDefaultKey = GET_SETTING(SpecifiedDefaultKey);
	switch ( GET_SETTING(DataFormatEnable) )
	{
		case 0 	:
			m_Options.ApplyDataFormat = 0;
			m_Options.RequireDataFormat = 0;
			m_Options.ParticularSymbol = 0;
			break;
		case 1 	:
			m_Options.ApplyDataFormat = 1;
			m_Options.RequireDataFormat = 0;
			m_Options.ParticularSymbol = 0;
			break;
		case 2 	:
			m_Options.ApplyDataFormat = 1;
			m_Options.RequireDataFormat = 1;
			m_Options.ParticularSymbol = 0;
			break;
		case 3 :
			m_Options.ApplyDataFormat = 1;
			m_Options.RequireDataFormat = 0;
			m_Options.ParticularSymbol = 1;
			break;
		case 4 :
			m_Options.ApplyDataFormat = 1;
			m_Options.RequireDataFormat = 1;
			m_Options.ParticularSymbol = 1;
	}
}

CDataEdit *g_pDataEditor = NULL;

// Call this static member function to instantiate the data editor
void CDataEdit::CreateDataEditor(void)
{
	if (g_pDataEditor == NULL)
	{
		g_pDataEditor = new CDataEdit;
	}
	ASSERT(g_pDataEditor != NULL);
}

// compatibility with old code
// This might be removed after the old code has been fixed
bool SetPrimaryFormat(void)
{
	return theDataEditor.SetPrimaryFormat();
}
bool SetAlternateFormatOne(void)
{
	return theDataEditor.SetAlternateFormatOne();
}
bool SetAlternateFormatTwo(void)
{
	return theDataEditor.SetAlternateFormatTwo();
}
bool SetAlternateFormatThree(void)
{
	return theDataEditor.SetAlternateFormatThree();
}



/*  End of File */
