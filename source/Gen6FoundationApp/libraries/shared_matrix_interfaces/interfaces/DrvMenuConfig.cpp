/*=============================================================================
 * This file contains functions that implement menu configuration for drivers
 *

 *=============================================================================
 * $RCSfile: interfaces/DrvMenuConfig.cpp $
 * $Revision: 1.1.2.2 $
 * $Date: 2010/07/20 15:22:27EDT $
 *============================================================================*/

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "DrvMenuConfig.h"
#include "ParseHelper.h"
#include "stdInclude.h"
#include "interfacetypedefs.h"
#include "barcodedata.h"
#include "uchar.h"

#define _U(x)	(const bchar_t*)(x)

DrvMenuConfig::DrvMenuConfig(void)
{
	m_CurrentMenuIndex = 0;
	m_pMenuResponse = NULL;
}

DrvMenuConfig::~DrvMenuConfig(void)
{
}

void DrvMenuConfig::FreeMenuResponse(void)
{
	m_pMenuResponse->Free();	                  // Avoid a memory leak
	m_pMenuResponse = NULL;
}

void DrvMenuConfig::SetMenuResponse(CBarcodeData *pMenuResponse)
{
	m_pMenuResponse = pMenuResponse;
}

void DrvMenuConfig::AppendToMenuBuffer(const bchar_t *pFormat, ...)
{
	va_list	pArgs;
	int written=0;
	size_t BufferSize = DRV_MENU_BUFFER_SIZE-m_CurrentMenuIndex;
	va_start( pArgs, pFormat );
	written = vsnprintf((char *)m_MenuBuffer+m_CurrentMenuIndex, BufferSize, (char *)pFormat, pArgs );
	if(written>0)
		m_CurrentMenuIndex += written;

	va_end(pArgs);
}

void DrvMenuConfig::AppendCmdToMenuBuffer(const bchar_t *pCommand, const bchar_t *pAction)
{
	AppendToMenuBuffer(_U("%s%s"), pCommand, pAction);
}

void DrvMenuConfig::AppendQueryCmdToMenuBuffer(const bchar_t *pCommand)
{
	AppendCmdToMenuBuffer(pCommand, _U("?;"));
}

////////////////////////////////////////////////////
// Some helpers that parse parts of a menu response.
// Store the resultig bool as a bit into the supplied paramter "Byte" at the place marked by "Mask".
bool DrvMenuConfig::ParseMenuResponse_bool(const tchar_t *pResponse, const tchar_t *pName, UCHAR &Byte, UCHAR Mask)
{
	bool ret;
	bool temp=0;
	ret = CParseHelper::Extract(pResponse, pName, temp);
	if(ret)
	{
		Byte &= ~Mask;
		Byte |= temp ? Mask : 0;
	}
	return ret;
}

// We use the following function to translate multiple settings in the reader configuration into a single setting in
// the SurePos configuration.
// Rules:
// 1.	All settings must in the the response, otherwise we don't even touch the SurePos side.
// 2. All settings in the list must be set, to set the SurePos side (logical AND function)
bool DrvMenuConfig::ParseMenuResponse_bool(const tchar_t *pResponse, const tchar_t *pName[], int count, UCHAR &Byte, UCHAR Mask)
{
	bool ret=true;
	bool temp = 0;
	int NumOnes=0;
	for(int i=0; i<count; i++)
	{
		ret &= CParseHelper::Extract(pResponse, pName[i], temp);
		if(ret && temp)
		{
			NumOnes++;
		}
	}

	if(ret)
	{
		Byte &= ~Mask;
		if(NumOnes == count)
		{
			Byte |= temp ? Mask : 0;
		}
	}

	return ret;
}

bool DrvMenuConfig::ParseMenuResponse_int(const tchar_t *pResponse, const tchar_t *pName, UCHAR &Byte, UCHAR Mask)
{
	bool ret;
	int temp;

	// we must find how often we need to shift
	int shift;
	temp = Mask;
	for(shift=0; shift<8; shift++)
	{
		if(temp & 0x01)
		{
			break;
		}
		temp = temp>>1;
	}

	temp=0;
	ret = CParseHelper::Extract(pResponse, pName, temp);  // read the setting
	if(ret)
	{
		Byte &= ~Mask;
		Byte |= (temp<<shift)&Mask;     // shift into right position and store it into SurePos config
	}
	return ret;
}


