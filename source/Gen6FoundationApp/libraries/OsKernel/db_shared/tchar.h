/*=============================================================================
 *

 *=============================================================================
 * $RCSfile: db_shared/tchar.h $
 * $Revision: 1.3 $
 * $Date: 2006/07/20 10:27:13EDT $
 *============================================================================*/

//! \file
/*!
 * tchar.h
 *
 * Unicode mapping layer for the standard C library. By including this
 * file and using the 't' names for string functions
 * (eg. _tprintf) you can make code which can be easily adapted to both
 * Unicode and non-unicode environments. In a unicode enabled compile define
 * _UNICODE before including tchar.h, otherwise the standard non-unicode
 * library functions will be used.
 *
 * Note that you still need to include string.h or stdlib.h etc. to define
 * the appropriate functions.
 *
 */

#ifndef	_TCHAR_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define _TCHAR_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9


#ifdef	_UNICODE

/*
 * Unicode functions
 */
#define	_tprintf		wprintf
#define	_ftprintf	fwprintf
#define	_stprintf	swprintf
#define	_sntprintf	_snwprintf
#define	_vtprintf	vwprintf
#define	_vftprintf	vfwprintf
#define	_vstprintf	vswprintf
#define	_vsntprintf	_vsnwprintf
#define	_tscanf		wscanf
#define	_ftscanf		fwscanf
#define	_stscanf		swscanf
#define	_fgettc		fgetwc
#define	_fgettchar	_fgetwchar
#define	_fgetts		fgetws
#define	_fputtc		fputwc
#define	_fputtchar	_fputwchar
#define	_fputts		fputws
#define	_gettc		getwc
#define	_getts		getws
#define	_puttc		putwc
#define	_putts		putws
#define	_ungettc		ungetwc
#define	_tcstod		wcstod
#define	_tcstol		wcstol
#define	_tcstoul		wcstoul
#define	_itot	  		_itow
#define	_ltot			_ltow
#define	_ultot		_ultow
#define	_ttoi			_wtoi
#define	_ttol			_wtol
#define	_tcscat		wcscat
#define	_tcschr		wcschr
#define	_tcscmp		wcscmp
#define	_tcscpy		wcscpy
#define	_tcscspn		wcscspn
#define	_tcslen		wcslen
#define	_tcsncat		wcsncat
#define	_tcsncmp		wcsncmp
#define	_tcsncpy		wcsncpy
#define	_tcspbrk		wcspbrk
#define	_tcsrchr		wcsrchr
#define	_tcsspn		wcsspn
#define	_tcsstr		wcsstr
#define	_tcstok		wcstok
#define	_tcsdup		_wcsdup
#define	_tcsicmp		_wcsicmp
#define	_tcsnicmp	_wcsnicmp
#define	_tcsnset		_wcsnset
#define	_tcsrev		_wcsrev
#define	_tcsset		_wcsset
#define	_tcslwr		_wcslwr
#define	_tcsupr		_wcsupr
#define	_tcsxfrm		wcsxfrm
#define	_tcscoll		wcscoll
#define	_tcsicoll	_wcsicoll
#define	_istalpha	iswalpha
#define	_istupper	iswupper
#define	_istlower	iswlower
#define	_istdigit	iswdigit
#define	_istxdigit	iswxdigit
#define	_istspace	iswspace
#define	_istpunct	iswpunct
#define	_istalnum	iswalnum
#define	_istprint	iswprint
#define	_istgraph	iswgraph
#define	_istcntrl	iswcntrl
#define	_istascii	iswascii
#define	_totupper	towupper
#define	_totlower	towlower
#define	_tcsftime	wcsftime

/* Macro functions */
#define	_tcsdec     _wcsdec
#define	_tcsinc     _wcsinc
#define	_tcsnbcnt   _wcsncnt
#define	_tcsnccnt   _wcsncnt
#define	_tcsnextc   _wcsnextc
#define	_tcsninc    _wcsninc
#define	_tcsspnp    _wcsspnp
#define	_wcsdec(_wcs1, _wcs2) ((_wcs1)>=(_wcs2) ? NULL : (_wcs2)-1)
#define	_wcsinc(_wcs)  ((_wcs)+1)
#define	_wcsnextc(_wcs) ((unsigned int) *(_wcs))
#define	_wcsninc(_wcs, _inc) (((_wcs)+(_inc)))
#define	_wcsncnt(_wcs, _cnt) ((wcslen(_wcs)>_cnt) ? _count : wcslen(_wcs))
#define	_wcsspnp(_wcs1, _wcs2) ((*((_wcs1)+wcsspn(_wcs1,_wcs2))) ? ((_wcs1)+wcsspn(_wcs1,_wcs2)) : NULL)


#else	/* Not _UNICODE */

/*
 * Non-unicode (standard) functions
 */

#define	_tprintf		printf
#define	_ftprintf	fprintf
#define	_stprintf	sprintf
#define	_sntprintf	_snprintf
#define	_vtprintf	vprintf
#define	_vftprintf	vfprintf
#define	_vstprintf	vsprintf
#define	_vsntprintf	_vsnprintf
#define	_tscanf		scanf
#define	_ftscanf		fscanf
#define	_stscanf		sscanf
#define	_fgettc		fgetc
#define	_fgettchar	_fgetchar
#define	_fgetts		fgets
#define	_fputtc		fputc
#define	_fputtchar	_fputchar
#define	_fputts		fputs
#define	_gettc		getc
#define	_getts		gets
#define	_puttc		putc
#define	_putts		puts
#define	_ungettc		ungetc
#define	_tcstod		strtod
#define	_tcstol		strtol
#define	_tcstoul		strtoul
#define	_itot			_itoa
#define	_ltot			_ltoa
#define	_ultot		_ultoa
#define	_ttoi			atoi
#define	_ttol			atol
#define	_tcscat		strcat
#define	_tcschr		strchr
#define	_tcscmp		strcmp
#define	_tcscpy		strcpy
#define	_tcscspn		strcspn
#define	_tcslen		strlen
#define	_tcsncat		strncat
#define	_tcsncmp		strncmp
#define	_tcsncpy		strncpy
#define	_tcspbrk		strpbrk
#define	_tcsrchr		strrchr
#define	_tcsspn		strspn
#define	_tcsstr		strstr
#define	_tcstok		strtok
#define	_tcsdup		_strdup
#define	_tcsicmp		_stricmp
#define	_tcsnicmp	_strnicmp
#define	_tcsnset		_strnset
#define	_tcsrev		_strrev
#define	_tcsset		_strset
#define	_tcslwr		_strlwr
#define	_tcsupr		_strupr
#define	_tcsxfrm		strxfrm
#define	_tcscoll		strcoll
#define	_tcsicoll	_stricoll
#define	_istalpha	isalpha
#define	_istupper	isupper
#define	_istlower	islower
#define	_istdigit	isdigit
#define	_istxdigit	isxdigit
#define	_istspace	isspace
#define	_istpunct	ispunct
#define	_istalnum	isalnum
#define	_istprint	isprint
#define	_istgraph	isgraph
#define	_istcntrl	iscntrl
#undef	_istascii				  // removes a warning mit MSVC 2005
#define	_istascii	isascii
#define	_totupper	toupper
#define	_totlower	tolower
#define	_tasctime	asctime
#define	_tctime		ctime
#define	_tstrdate	_strdate
#define	_tstrtime	_strtime
#define	_tutime		_utime
#define	_tcsftime	strftime

/* Macro functions */
#define	_tcsdec     _strdec
#define	_tcsinc     _strinc
#define	_tcsnbcnt   _strncnt
#define	_tcsnccnt   _strncnt
#define	_tcsnextc   _strnextc
#define	_tcsninc    _strninc
#define	_tcsspnp    _strspnp
#define	_strdec(_str1, _str2) ((_str1)>=(_str2) ? NULL : (_str2)-1)
#define	_strinc(_str)  ((_str)+1)
#define	_strnextc(_str) ((unsigned int) *(_str))
#define	_strninc(_str, _inc) (((_str)+(_inc)))
#define	_strncnt(_str, _cnt) ((strlen(_str)>_cnt) ? _count : strlen(_str))
#define	_strspnp(_str1, _str2) ((*((_str1)+strspn(_str1,_str2))) ? ((_str1)+strspn(_str1,_str2)) : NULL)


#endif	/* Not _UNICODE */

#endif	/* Not _TCHAR_H_ */

