/*=================================================================================
    Helpers to fix tchar_t, bchar_t (signed unsigned char) problems.
//=================================================================================
   $Source: Gen6FoundationApp/FoundationApp/Flash/uchar.h $
   $Date: 2008/09/22 11:15:06EDT $
   $Revision: 1.1 $


//=================================================================================*/

#ifndef UCHAR_HELPER_H
#define UCHAR_HELPER_H

#include <stdio.h>

inline size_t strlen(const bchar_t *pStr)
{
	return strlen((const char*)pStr);
}

inline bchar_t *strcpy( bchar_t *strDestination, const bchar_t *strSource )
{
	return (bchar_t *)strcpy( (char *)strDestination, (const char *)strSource );
}

inline bchar_t *strcpy( bchar_t *strDestination, const char *strSource )
{
	return (bchar_t *)strcpy( (char *)strDestination, (const char *)strSource );
}

inline bchar_t *strncpy( bchar_t *strDestination, const bchar_t *strSource, size_t count  )
{
	return (bchar_t *)strncpy( (char *)strDestination, (const char *)strSource, count );
}

inline bchar_t *strncpy( bchar_t *strDestination, const char *strSource, size_t count  )
{
	return (bchar_t *)strncpy( (char *)strDestination, (const char *)strSource, count );
}

inline bchar_t *strcat( bchar_t *strDestination, const bchar_t *strSource )
{
	return (bchar_t *)strcat( (char *)strDestination, (const char *)strSource );
}

inline bchar_t *strncat( bchar_t *strDestination, const bchar_t *strSource, size_t count  )
{
	return (bchar_t *)strncat( (char *)strDestination, (const char *)strSource, count );
}

inline bchar_t *strchr( const bchar_t *string, int c )
{
	return (bchar_t *)strchr( (const char *)string, c );
}

inline bchar_t *strrchr( const bchar_t *string, int c )
{
	return (bchar_t *)strrchr( (const char *)string, c );
}

inline bchar_t *strstr( const bchar_t *string, const bchar_t *strCharSet )
{
	return (bchar_t *)strstr( (const char *)string, (const char *)strCharSet );
}

inline bchar_t *strstr( const bchar_t *string, const char *strCharSet )
{
	return (bchar_t *)strstr( (const char *)string, (const char *)strCharSet );
}

inline bchar_t *strstr( const char *string, const bchar_t *strCharSet )
{
	return (bchar_t *)strstr( (const char *)string, (const char *)strCharSet );
}

inline int strcmp( const bchar_t *string1, const bchar_t *string2 )
{
	return strcmp( (const char *)string1, (const char *)string2 );
}

inline int strcmp( const bchar_t *string1, const char *string2 )
{
	return strcmp( (const char *)string1, (const char *)string2 );
}

inline int strcmp( const char *string1, const bchar_t *string2 )
{
	return strcmp( (const char *)string1, (const char *)string2 );
}

inline int strncmp( const bchar_t *string1, const bchar_t *string2, size_t count )
{
	return strncmp( (const char *)string1, (const char *)string2, count );
}

/*
inline int stricmp( const bchar_t *string1, const bchar_t *string2 )
{
	return stricmp( (const char *)string1, (const char *)string2 );
}

inline int strnicmp( const bchar_t *string1, const bchar_t *string2, size_t count )
{
	return strnicmp( (const char *)string1, (const char *)string2, count );
}
*/

inline int atoi(const bchar_t *pStr)
{
	return atoi((const char*)pStr);
}

inline long atol(const bchar_t *pStr)
{
	return atol((const char*)pStr);
}

inline double atof(const bchar_t *pStr)
{
	return atof((const char*)pStr);
}

size_t sprintf(bchar_t *pBuffer,const bchar_t *pFormat, ...);
size_t _snprintf(bchar_t *pBuffer, size_t n, const bchar_t *pFormat, ...);

#endif // UCHAR_HELPER_H



