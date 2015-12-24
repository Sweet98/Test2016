#include "scandriverdebug.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interrupts.h"

#if defined(_WIN32_WCE) || defined(_MSC_VER)
#  include <windows.h>
#  if defined(_WIN32_WCE)
#     include <dbgapi.h>
#  endif
#  if defined(_MSC_VER)
#     include <tchar.h>
#     define NKDbgPrintfW _tprintf
#  endif
#else
#  include "db_chrio.h"
#endif


#ifdef SD_NAMESPACE
   namespace SD_NAMESPACE {
#endif

void dbg_output(const char * str, ...)
{
	char output_string[257];
	va_list arglist;
	va_start(arglist, str);

	_vsnprintf(output_string, sizeof(output_string), str, arglist);
	output_string[sizeof(output_string) - 1] = 0;

#if defined(_WIN32_WCE) || defined(_MSC_VER)
	TCHAR output_string2[257];
	_stprintf(output_string2, TEXT("%hs"), output_string);
	NKDbgPrintfW(TEXT("%s"), output_string2);
#else
	xputstring(output_string);
#endif


	va_end(arglist);
}

void sd_assert_function(const char * cond, const char * file, unsigned line)
{
	dbg_output("\r\n****************************************\r\n"
			   "Assertion: %s @ %s line @%u\r\n\r\n", cond, file, line);
 #ifdef _MSC_VER
	abort();
#endif
}
static const unsigned sz_log_buffer = 256 * 1024;
volatile char * volatile log_buffer = 0;
volatile char * volatile wp_log_buffer = 0; // write pointer into log_buffer

static void dbg_log_init()
{
	int _SavedIrqMask_;

	DisableInterrupts(_SavedIrqMask_);
	bool init = (log_buffer != 0);
	RestoreInterrupts(_SavedIrqMask_);

	if (init)
		return;

	char * log_buffer2 = (char *)malloc(sz_log_buffer + 1);

	DisableInterrupts(_SavedIrqMask_);
	init = (log_buffer != 0);
	if (!init)
	{
		log_buffer    = log_buffer2;
		wp_log_buffer = log_buffer2;
	}
	RestoreInterrupts(_SavedIrqMask_);

	if (init)
		free(log_buffer2);
}

template <class T>
void swap(T & t1, T & t2)
{
	T tmp = t1;
	t1 = t2;
	t2 = tmp;
}

void dbg_log_dump()
{
	int _SavedIrqMask_;
	DisableInterrupts(_SavedIrqMask_);
	bool empty = (log_buffer == wp_log_buffer || log_buffer == 0);
	RestoreInterrupts(_SavedIrqMask_);
	if (empty)
		return;


	char * log_buffer2 = (char *)malloc(sz_log_buffer + 1);
	char * wp_log_buffer2 = log_buffer2;
	char * tmp;

	DisableInterrupts(_SavedIrqMask_);

	tmp = log_buffer2;
	log_buffer2 = (char *)log_buffer;
	log_buffer = tmp;

	tmp = wp_log_buffer2;
	wp_log_buffer2 = (char *)wp_log_buffer;
	wp_log_buffer = tmp;

	RestoreInterrupts(_SavedIrqMask_);

	char buffer[129];
	char * rp_log_buffer = log_buffer2;

	while(rp_log_buffer < wp_log_buffer2)
	{
		unsigned chunk = wp_log_buffer2 - rp_log_buffer;
		if (chunk > 128)
			chunk = 128;
		
		memcpy(buffer, rp_log_buffer, chunk);
		buffer[chunk] = 0;
		xputstring(buffer);

		rp_log_buffer += chunk;
	}

	free(log_buffer2);
}

void dbg_log(const char * str, ...)
{
	dbg_log_init();

	char output_string[257];
	va_list arglist;
	va_start(arglist, str);

	_vsnprintf(output_string, sizeof(output_string), str, arglist);
	output_string[sizeof(output_string) - 1] = 0;

	unsigned slen = strlen(output_string);

	DISABLE_INTERRUPTS();
	unsigned clen = sz_log_buffer - (wp_log_buffer - log_buffer);
	if (clen > slen)
		clen = slen;

	memcpy((char *)wp_log_buffer, output_string, clen);
	wp_log_buffer += clen;
	RESTORE_INTERRUPTS();
	

	va_end(arglist);
}

#ifdef SD_NAMESPACE
   } // namespace SD_NAMESPACE {
#endif
