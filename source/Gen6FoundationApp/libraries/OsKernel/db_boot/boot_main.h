/*===================================================================
 Common functions
=====================================================================
   $Source: db_boot/boot_main.h $
   $Date: 2011/04/18 15:23:54EDT $
   $Revision: 1.2 $


===================================================================*/
#ifndef _boot_main_h_
#define _boot_main_h_

extern "C"		void Run_application_code( void *AddressToRelocateApplicationToBeforeExecuting, void *AddressOfApplication, int SizeOfApplication );

int ReadApplicationCodeFromSerialFlash(unsigned char **DestinationLocation);

extern const char g_TimeStamp[];
extern const char g_DateStamp[];


#endif // endof #ifndef _boot_main_h_



