/*=============================================================================
 * This file contains functions that implement menu configuration for drivers
 *

 *=============================================================================
 * $RCSfile: interfaces/DrvMenuConfig.h $
 * $Revision: 1.1.2.2 $
 * $Date: 2010/07/20 15:22:27EDT $
 *============================================================================*/

#ifndef DRVMENUCONFIG_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define DRVMENUCONFIG_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "stdInclude.h"
#include "barcodedata.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#define DRV_MENU_BUFFER_SIZE 512

class DrvMenuConfig
{
protected:
	size_t m_CurrentMenuIndex;
	bchar_t m_MenuBuffer[DRV_MENU_BUFFER_SIZE+1];
	CBarcodeData *m_pMenuResponse;
	
public:
	DrvMenuConfig(void);
	~DrvMenuConfig(void);
	
	CBarcodeData *GetMenuResponse(void)	{ return m_pMenuResponse; }
	bchar_t *GetMenuBuffer(void)		{ return m_MenuBuffer; }
	int GetMenuLength(void)				{ return m_CurrentMenuIndex; }
	void ResetMenuBuffer(void)			{ m_CurrentMenuIndex = 0; }
	void FreeMenuResponse(void);
	void SetMenuResponse(CBarcodeData *pMenuResponse);
	void AppendToMenuBuffer(const bchar_t *pFormat, ...);
	void AppendCmdToMenuBuffer(const bchar_t *pCommand, const bchar_t *pAction);
	void AppendQueryCmdToMenuBuffer(const bchar_t *pCommand);
	static bool ParseMenuResponse_bool(const bchar_t *pResponse, const bchar_t *pName, UCHAR &Byte, UCHAR Mask);
	static bool ParseMenuResponse_bool(const bchar_t *pResponse, const bchar_t *pName[], int count, UCHAR &Byte, UCHAR Mask);
	static bool ParseMenuResponse_int(const bchar_t *pResponse, const bchar_t *pName, UCHAR &Byte, UCHAR Mask);

	void DebugSend(const bchar_t *pBin, size_t Size);  // a debug helper
	
};

#endif /* DRVMENUCONFIG_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */


