/*===================================================================
  See .cpp file for description 

=====================================================================
   $Source: PinfoCreator.h $
   $Date: 2011/06/06 08:54:37EDT $
   $Revision: 1.5 $


===================================================================*/
#ifndef _PinfoCreator_h_
#define _PinfoCreator_h_

#include "language.h"
#include "HstringArray.h"


#define PINFO_NAME_MODE			"mode"
#define PINFO_NAME_APP_PRES		"app-present"
#define PINFO_NAME_BOOT_PRES	"boot-present"
#define PINFO_NAME_SN		"hw-sn"
#define PINFO_NAME_HW_REV	"hw-rev"
#define PINFO_NAME_CBL		"cbl"
#define PINFO_NAME_APP_REV	"app-sw-rev"
#define PINFO_NAME_BOOT_REV	"boot-sw-rev"
#define PINFO_NAME_FLASH	"flash"
#define PINFO_NAME_APP_PNAME_MENU	"app-m_name"
#define PINFO_NAME_BOOT_PNAME_MENU	"boot-m_name"
#define PINFO_NAME_APP_PNAME_CONFIG	"app-p_name"
#define PINFO_NAME_BOOT_PNAME_CONFIG	"boot-p_name"
#define PINFO_NAME_APP_SW_TIME	"app-time"
#define PINFO_NAME_BOOT_SW_TIME	"boot-time"
#define PINFO_NAME_APP_SW_DATE	"app-date"
#define PINFO_NAME_BOOT_SW_DATE	"boot-date"
#define PINFO_NAME_HUB_NUM	"hub-connections"
#define PINFO_NAME_HUB_DEV_ADD	"dev-addr" // a number must be inserted
#define PINFO_NAME_HUB_DEV_NAME	"dev-name" // a number must be inserted
#define PINFO_NAME_HUB_DEV_TYPE	"dev-type" // a number must be inserted

#define PINFO_CABLE_UNKNOWN	1
#define PINFO_CABLE_RS232	2
#define PINFO_CABLE_USB		4
#define PINFO_CABLE_RETAIL	8
#define PINFO_CABLE_KBD_WDG	16
#define PINFO_CABLE_BLUETOOTH	32

#define PINFO_STR_VAL_APP	"app"
#define PINFO_STR_VAL_UNKNOWN	"unknown"
#define PINFO_STR_VAL_EXT	"external"

class PinfoCreator
{
public:
	PinfoCreator(void);
	void AddItem(const char * pName, const char * pValue);
	void AddItem(const char * pName, int value);
	void AddItem(const char * pName, bool value);
	void AddItemHex(const char * pName, unsigned int value);
	void AddSubItem(const char * pName, int NameNum, const char * pValue);
	void CreateStr(HSTRING * pPinfoStr);

protected:
	HSTRINGARRAY m_Items;
	
	void BreakUpLine(HSTRING * pLine);
	WORD CalculateChecksum(void);
};

#endif // endof #ifndef _PinfoCreator_h_



