/*===================================================================
  See source file module for description.
=====================================================================
   $Source: Gen6FoundationApp/FoundationApp/Flash/FileHandler.h $
   $Date: 2011/09/23 15:03:41EDT $
   $Revision: 1.4 $


===================================================================*/
#ifndef _FileHandler_h_
#define _FileHandler_h_

#define USER_FOLDER	"/PluginFolder"
#define SYSTEM_FOLDER	"/sys"

class CFileHandler
{
public:
	static bool StoreFile(const char * pFileName, const unsigned char * pData, int DataLength);
	static bool StoreUserFile(const char * pFileName, const unsigned char * pData, int DataLength);
	static bool ProcessSystemUpdateMocf(const unsigned char * pMocf);
	static bool ProcessMocf(const unsigned char * pMocf, int MocfLength, bool bRunAppFirmwareFromRam = false, bool bDoCompatibleCheck = true, bool bSendResponseAfterReboot = true);

protected:
	static bool RunFromRam(const unsigned char * pMocf);
	static void SaveInterfaceMenuSettings(void);

#ifdef RFBASE
	static bool RouteToRfScanners(const unsigned char *pMocf, int MocfLength);
#endif // endof #ifdef RFBASE
};


#endif // endof #ifndef _FileHandler_h_



