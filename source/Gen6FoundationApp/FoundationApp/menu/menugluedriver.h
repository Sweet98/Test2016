/*=================================================================================
  class: CMenuGlue

//=================================================================================
   $Source: Gen6FoundationApp/FoundationApp/menu/menugluedriver.h $
   $Date: 2010/05/04 11:45:14EDT $
   $Revision: 1.3 $


//=================================================================================*/

#ifndef MENU_GLUE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_
#define MENU_GLUE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_

class HSTRING;

class CMenuGlueDriver
{
public:
	static MatrixError_t GetSetting(USHORT usType, tichar_t *pBuffer, size_t BufferSize);

	// These are helpers to isolate the USB interfaces from the menu system
	static HSTRING *GetVendorName(void);
	static HSTRING *GetProductName(void);
	static HSTRING *GetSerNumString(void);

	static USHORT GetUsbPID(void);
	static USHORT GetUsbVID(void);
	static USHORT GetUsbGen(void);
	static USHORT GetUsbLev(void);
	static int GetPartNumFromMenu(void);
};


#endif // MENU_GLUE_H__20DB49DD_8F9C_4ADD_9830_CF227654ADFA__INCLUDED_




