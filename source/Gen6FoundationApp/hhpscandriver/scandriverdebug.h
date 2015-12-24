#ifndef SCANDRIVERDEBUG_H_F12E6156_B5A4_4C5A_AED9_B1D93AC01AAC
#define SCANDRIVERDEBUG_H_F12E6156_B5A4_4C5A_AED9_B1D93AC01AAC

#if defined(_WIN32_WCE) || defined(PALMOS)
#include <windows.h>
#else // DBALL
extern "C" {
	int abs(int);
}
#include "language.h"
#include "platform.h"

// #include <math.h>

// #include "db_time.h"
// #include "menu.h" //FIXME TODO definitely do not want this to stay
// #include "ReaderConfiguration.h" //FIXME TODO definitely do not want this to stay



#endif


#include <stdio.h>
#include <string.h>


#ifndef ESHELL_DEBUG
#define ESHELL_DEBUG 0
#endif

#ifndef DBALL_DEBUG
#define DBALL_DEBUG 0
#endif

#ifndef ASSERT_FLAG
#define ASSERT_FLAG 0
#endif

#if ESHELL_DEBUG || DBALL_DEBUG

#define DBG_OUTPUT  dbg_output
#define DBG_OUTPUT1 dbg_output
#define DBG_OUTPUT2 dbg_output
#define DBG_OUTPUT3 dbg_output
#define DBG_OUTPUT4 dbg_output
#define DBG_OUTPUT5 dbg_output
#define DBG_OUTPUT6 dbg_output
#define DBG_OUTPUT7 dbg_output



#else // #if ESHELL_DEBUG || DBALL_DEBUG

#define DBG_OUTPUT(a);
#define DBG_OUTPUT1(a,b);
#define DBG_OUTPUT2(a,b,c);
#define DBG_OUTPUT3(a,b,c,d);
#define DBG_OUTPUT4(a,b,c,d,e);
#define DBG_OUTPUT5(a,b,c,d,e,f);
#define DBG_OUTPUT6(a,b,c,d,e,f,g);
#define DBG_OUTPUT7(a,b,c,d,e,f,g,h);

#endif // #if ESHELL_DEBUG || DBALL_DEBUG

// ERR_MODE:
// 0 - No logging
// 1 - Call NKDbgPrintfW for WinCE , do nothing for firmware
#ifndef ERR_MODE
	#ifdef _WIN32_WCE
		#define ERR_MODE 1
	#else
		#define	ERR_MODE 0
	#endif
#endif

#if ERR_MODE
	#define	ERRMSG		dbg_output
	#define ERRMSG1		dbg_output
	#define ERRMSG2		dbg_output
#else
	#define ERRMSG(a);
	#define ERRMSG1(a,b);
	#define ERRMSG2(a,b,c);
#endif


#ifdef SD_NAMESPACE
   namespace SD_NAMESPACE {
#else
#  ifdef __cplusplus
      extern "C" {
#  endif
#endif

void dbg_output(const char * str, ...);
void dbg_log(const char * str, ...);
void dbg_log_dump();

#ifdef SD_NAMESPACE
  } // namespace SD_NAMESPACE {
#else
#  ifdef __cplusplus
      } // extern "C" {
#  endif
#endif

void sd_assert_function(const char * cond, const char * file, unsigned line);

template <class T>
struct to_str
{
};

template <>
struct to_str<unsigned>
{
	const char * operator()(unsigned v)
		{
			char number[40];
			
			sprintf(number, "%u", v);
			char * ret = new char[strlen(number)+1];
			strcpy(ret, number);
			return ret;
		}
};

template <class T>
struct to_str<T *>
{
	const char * operator()(T * v)
		{
			char number[40];
			
			sprintf(number, "0x%X", v);
			char * ret = new char[strlen(number)+1];
			strcpy(ret, number);
			return ret;
		}
};

template <>
struct to_str<DWORD>
{
	const char * operator()(unsigned v)
		{
			char number[40];
			
			sprintf(number, "%u", v);
			char * ret = new char[strlen(number)+1];
			strcpy(ret, number);
			return ret;
		}
};

template<class T1, class T2>
void sd_assert_eq_function(  const char * value_name, const T1 &value,  
							 const T2 & expect, const char * file, 
							 unsigned line)
{
	to_str<T1> ts1;
	to_str<T2> ts2;
	const char * v_str = ts1(value);
	const char * e_str = ts2(expect);
	dbg_output("\r\n****************************************\r\n"
			   "Assertion: %s expected %s got %s @ %s line %u\r\n\r\n", 
			   value_name, e_str, v_str, file, line);
	delete[] v_str;
	delete[] e_str;

#ifdef _MSC_VER
	abort();
#endif
}

#if ASSERT_FLAG

#define SDASSERT_EQ(expect,value)										\
	if ((expect) != (value))										\
		sd_assert_eq_function(#value, (value), (expect), __FILE__, __LINE__)


#define SDASSERT(cond) \
   ((cond) ? ((void)0) : sd_assert_function(#cond, __FILE__, __LINE__))

#else

#define SDASSERT(cond) ((void)0)
#define SDASSERT_EQ(expect,value) ((void)0)

#endif

#include "menu.h" //FIXME TODO definitely do not want this to stay
#include "ReaderConfiguration.h" //FIXME TODO definitely do not want this to stay
#define DBG(compare) if ((GET_SETTING(DebugScanDriver)) compare)
#define DBGM(mask,compare) if (  ((GET_SETTING(DebugScanDriver)) == 3) ||   \
								 (  ((GET_SETTING(DebugScanDriver)) & mask) \
									compare  ))



#endif // #ifndef SCANDRIVERDEBUG_H_F12E6156_B5A4_4C5A_AED9_B1D93AC01AAC


