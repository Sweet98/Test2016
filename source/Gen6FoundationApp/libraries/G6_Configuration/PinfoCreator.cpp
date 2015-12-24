/*===================================================================
  This class is used to create the ASCII text based data structure
  that is displayed when the P_INFO command is called.
  
  To use, call AddItem() to add an item/value pair.  When done, call
  CreateStr() to create the P_INFO string.
  
  Limitations:
    There is no reset method--it relies on the constructor to set
    things up.  If you want to re-use the same class instantiation,
    you'll need to add a reset() method.
=====================================================================
   $Source: PinfoCreator.cpp $
   $Date: 2011/06/06 08:54:37EDT $
   $Revision: 1.3 $


===================================================================*/
#include <stdio.h>
#include <string.h>
#include "PinfoCreator.h"

#define MAX_LINE_LEN	(80 - 2) // The cr/lf is included in the max line length.  Rather than adjust for it in the code, we adjust for it here.

#define NAME_VAL_SEPARATOR_STR	": "
#define LINE_TERMINATOR_STR	"\r\n"
#define LINE_CONT_STR " "
#define NAME_SUB_NAME_SEPARATOR_CHAR	'-'

#define NAME_FMT		"fmt"
#define NAME_CHECKSUM	"csum"

PinfoCreator::PinfoCreator(void)
{
	AddItem(NAME_FMT, 1);
}



/*----------------------------------------------------------------------------------------
AddItem

  Adds a name/value pair to the P_INFO structure.  

  Parameters:  
  			pName[in]:  name of item

  			pValue[in]:  item data (string)

  Returns:  nothing.
----------------------------------------------------------------------------------------*/
void PinfoCreator::AddItem(const char * pName, const char * pValue)
{
	HSTRING line(pName, true);  // true == auto resize, if necessary
	
	line.Append(NAME_VAL_SEPARATOR_STR);
	line.Append(pValue);
	line.Append(LINE_TERMINATOR_STR);
	BreakUpLine(&line);
	m_Items.Add(&line);
}


/*----------------------------------------------------------------------------------------
AddItem

  Adds a name/value pair to the P_INFO structure.  

  Parameters:  
  			pName[in]:  name of item

  			pValue[in]:  item data (integer)

  Returns:  nothing.
----------------------------------------------------------------------------------------*/
void PinfoCreator::AddItem(const char * pName, int value)
{
	char ValueStr[20]; // has to be big enough for the longest - integer & NULL
	sprintf(ValueStr, "%d", value);
	AddItem(pName, ValueStr);
}


/*----------------------------------------------------------------------------------------
AddItem

  Adds a name/value pair to the P_INFO structure.  

  Parameters:  
  			pName[in]:  name of item

  			pValue[in]:  item data (bool)

  Returns:  nothing.
----------------------------------------------------------------------------------------*/
void PinfoCreator::AddItem(const char * pName, bool value)
{
	AddItem(pName, (int) value);
}


/*----------------------------------------------------------------------------------------
AddItemHex

  Adds a name/value pair to the P_INFO structure.  Creates a hex string for the data

  Parameters:  
  			pName[in]:  name of item

  			pValue[in]:  item data (unsigned integer)

  Returns:  nothing.
----------------------------------------------------------------------------------------*/
void PinfoCreator::AddItemHex(const char * pName, unsigned int value)
{
	char ValueStr[20]; // has to be big enough for "0x" & the longest hex integer & NULL
	sprintf(ValueStr, "%#x", value);
	AddItem(pName, ValueStr);
}




/*----------------------------------------------------------------------------------------
AddSubItem

  Some items are sub items (they are an enumeration of devices).  This builds that item
  by appending the number to the name.

  NOTE:  the P_INFO spec has no formal definition of where the numbers appear.  To date,
  they all appear after the first dash ('-').  This could change some day.

  NOTE:  the format for hubs changed.  This code corresponds to v4 of Aldo's doc, not the
  P_INFO spec in the team room (that rev is dated 5/18/2011).  The format is 
  PrimaryName-n-SubName:  value.  This method inserts the 'n' and appends the value.

  Parameters:  
  			pName[in]:  name of item

  			NameNum[in]:  incremented number of the item's name

  			pValue[in]:  item data (string)

  Returns:  nothing.
----------------------------------------------------------------------------------------*/
void PinfoCreator::AddSubItem(const char * pName, int NameNum, const char * pValue)
{
	char ValueStr[20]; // has to be big enough for the longest integer & NULL
	int DashIndex;  // this is the separator for the number
	HSTRING SubItemName(pName, true);  // true == auto resize, if necessary
	if ( ! SubItemName.FindCharacterForward(NAME_SUB_NAME_SEPARATOR_CHAR, 0, &DashIndex) ) return;  // we should always find the '-', but just in case something goes wrong, we'll just return
	sprintf(ValueStr, "-%d", NameNum);
	SubItemName.Insert(ValueStr, DashIndex);
	AddItem((const char *)SubItemName.Char, pValue);
}




/*----------------------------------------------------------------------------------------
CreateStr

  Creates the string representing the P_INFO structure.  

  Parameters:  
  			pPinfoStr[out]:  p_INFO structure

  Returns:  nothing.
----------------------------------------------------------------------------------------*/
void PinfoCreator::CreateStr(HSTRING * pPinfoStr)
{
	int WhichLine;
	WORD checksum = CalculateChecksum();
	AddItemHex(NAME_CHECKSUM, checksum);
	// Now create one big string
	pPinfoStr->Remove(0, pPinfoStr->Size);  // empty str in case caller didn't
	for (WhichLine = 0; WhichLine < m_Items.GetSize(); WhichLine++)
	{
		pPinfoStr->Append(m_Items.ElementAt(WhichLine));
	}
	pPinfoStr->Append(LINE_TERMINATOR_STR); // Add a blank line which indicates endo of P_INFO data
}







/*----------------------------------------------------------------------------------------
BreakUpLine

  Breaks up the line (if necessary) into two or more lines to ensure there are no long lines.
  The spec calls it "wrapping", but it's more than that as we need to insert a continuation
  character.
  
  Parameters:  
  			pLine[in]:  line to break up

  Returns:  nothing.
----------------------------------------------------------------------------------------*/
void PinfoCreator::BreakUpLine(HSTRING * pLine)
{
	int index = 0;  // pointsw to start of (next) line
	while ( (pLine->Size - index) > MAX_LINE_LEN )
	{	// long line--break it up
		pLine->Insert(LINE_TERMINATOR_STR, index + MAX_LINE_LEN);
		index += MAX_LINE_LEN + strlen(LINE_TERMINATOR_STR);
		pLine->Insert(LINE_CONT_STR, index);
		// don't increment index--we want to count the continuation character in the next line length
	}
}



/*----------------------------------------------------------------------------------------
CreateStr

  Does a simple checksum on the P_INFO structure.  

  Parameters:  none

  Returns:  checksum.
----------------------------------------------------------------------------------------*/
WORD PinfoCreator::CalculateChecksum(void)
{
	int WhichLine;
	int i;
	WORD checksum = 0;
	
	for (WhichLine = 0; WhichLine < m_Items.GetSize(); WhichLine++)
	{
		HSTRING * pLine = m_Items.ElementAt(WhichLine);
		const char * pChar =  (const char *) pLine->Char;
		for (i = 0; i < pLine->Size; i++)  // Note:  HSTRING doesn't include the NULL in Size, but if it did, it wouldn't affect outcome of checksum
		{
			checksum += *pChar;
			pChar++;
		} 
	}
	return checksum;
}









