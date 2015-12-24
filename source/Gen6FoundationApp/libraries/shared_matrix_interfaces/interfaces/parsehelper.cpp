/*=================================================================================
  class: CParseHelper
 
//=================================================================================
   $Source: interfaces/parsehelper.cpp $
   $Date: 2008/12/17 13:26:56EST $
   $Revision: 1.7.1.2 $
 

//=================================================================================*/
#include "stdInclude.h"
#include "product.h"
#include "string.h"
#include "stdlib.h"
#include "ParseHelper.h"
// Extract the int from a string with labels
bool CParseHelper::Extract(const wchar_t *pIn, const wchar_t *pSearch, volatile int &Result)
{
	bool RetVal=FALSE;
	const tichar_t *pFound = wcsstr( pIn, pSearch );
	if(pFound != NULL)
	{
		int index = wcslen(pSearch);
		Result = wcstol(&pFound[index],NULL,10);
		RetVal = TRUE;
	}
	return RetVal;
}

#ifdef UNICODE_FOR_INTERFACES
bool CParseHelper::Extract(const bchar_t *pIn, const bchar_t *pSearch, volatile int &Result)
{
	bool RetVal=FALSE;
	const char *pFound = strstr((char *)pIn, (char *)pSearch );
	if(pFound != NULL)
	{
		int index = strlen((char *)pSearch);
		Result = atoi((char *)(&pFound[index]));
		RetVal = TRUE;
	}
	return RetVal;
}
#endif

// Extract the ULONG from a string with labels
bool CParseHelper::Extract(const wchar_t *pIn, const tichar_t *pSearch, volatile ULONG &Result)
{
	bool RetVal=FALSE;
	const wchar_t *pFound = wcsstr( pIn, pSearch );
	if(pFound != NULL)
	{
		int index = wcslen(pSearch);
		Result = wcstol(&pFound[index],NULL,10);
		RetVal = TRUE;
	}
	return RetVal;
}

#ifdef UNICODE_FOR_INTERFACES
// Extract the ULONG from a string with labels
bool CParseHelper::Extract(const bchar_t *pIn, const bchar_t *pSearch, volatile ULONG &Result)
{
	bool RetVal=FALSE;
	const char *pFound = strstr( (char *)pIn, (char *)pSearch );
	if(pFound != NULL)
	{
		int index = strlen((char *)pSearch);
		Result = atol((char *)(&pFound[index]));
		RetVal = TRUE;
	}
	return RetVal;
}
#endif

// Extract the bool from a string with labels
bool  CParseHelper::Extract(const wchar_t *pIn, const wchar_t *pSearch, volatile bool &Result)
{
	int temp=0;
	bool RetVal = Extract(pIn, (const tichar_t*) pSearch, temp);
	if(RetVal)
	{
		Result = temp ? true : false;
	}
	return RetVal;
}

#ifdef UNICODE_FOR_INTERFACES
// Extract the bool from a string with labels
bool  CParseHelper::Extract(const bchar_t *pIn, const bchar_t *pSearch, volatile bool &Result)
{
	ULONG temp=0;
	bool RetVal = Extract(pIn, pSearch, temp);
	if(RetVal)
	{
		Result = temp ? true : false;
	}
	return RetVal;
}
#endif

// Returns a pointer to the nth string.
bool CParseHelper::Extract(const wchar_t *pIn, int delimiter, int index, const tichar_t *&pFound)
{
	bool RetVal=FALSE;
	const tichar_t *pCurrent = pIn;
	const tichar_t *pNew = pIn;

	do
	{
		pNew = wcschr(pCurrent, delimiter);
		if(index-- == 0)
		{
			pFound = pCurrent;
			RetVal = TRUE;
			break;
		}
		if(pNew != NULL)
		{
			pCurrent = &pNew[1];
		}
	}
	while(pNew != NULL);

	return RetVal;
}

bool CParseHelper::Extract(const wchar_t *pIn, int delimiter, int index, volatile int &Result)
{
	bool RetVal=FALSE;
	const tichar_t *pFound = pIn;
	if (Extract(pIn, delimiter, index, pFound))
	{
		Result = wcstol(pFound,NULL,10);
		RetVal = TRUE;
	}

	return RetVal;
}

bool CParseHelper::Extract(const wchar_t *pIn, int index, volatile int &Result)
{
	return Extract(pIn, ',', index, Result);
}

bool CParseHelper::Extract(const wchar_t *pIn, int index, const tichar_t *&pFound)
{
	return Extract(pIn, ',', index, pFound);
}

//#define DEBUG_CParseHelper
#ifdef DEBUG_CParseHelper

void DebugCParseHelper(void)
{
	tichar_t *pString;
	int Result=0;
	bool ret;
	ret = CParseHelper::Extract((const tichar_t*)"000,38400,XON0,ACK0,CTS1,IC_X_DLY5,DelayChar32,ICDLY10,IFDLY12,IMDLY13", (const tichar_t*)"ICDLY", Result);
	ret = CParseHelper::Extract((const tichar_t*)"000,38400,XON0,ACK0,CTS1,IC_X_DLY5,DelayChar32,ICDLY10,IFDLY12,IMDLY13", ',', 0, pString);
	ret = CParseHelper::Extract((const tichar_t*)"000,38400,XON0,ACK0,CTS1,IC_X_DLY5,DelayChar32,ICDLY10,IFDLY12,IMDLY13", ',', 1, pString);
	ret = CParseHelper::Extract((const tichar_t*)"000,38400", ',', 1, pString);
	ret = CParseHelper::Extract((const tichar_t*)"000,38400", ',', 2, pString);

	ret = CParseHelper::Extract((const tichar_t*)"000,38400", ',', 0, Result);
	ret = CParseHelper::Extract((const tichar_t*)"000,57600", ',', 1, Result);
}

#endif


