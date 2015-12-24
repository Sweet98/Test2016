/*===================================================================

=====================================================================
   $Source: Gen6FoundationApp/FoundationApp/ProjectRevision.c $
   $Date: 2011/04/18 15:13:55EDT $
   $Revision: 1.4 $


===================================================================*/
#include <stdio.h>
#include "ProjectRevision.h"
const char pSW_GEN[] = FIRMWARE_GENERATION;
extern const char g_TimeStamp[] = __TIME__;
extern const char g_DateStamp[] = __DATE__;
const char TimeDateRevisionString[] =  __TIME__ " " __DATE__ " " RevProject;


const char* ReportProjectGenString(void)
{
	return pSW_GEN;
}



//TODO:  This macro is a re-definition of the one in SystemMenuSettings.cpp.  If I try to move it to SystemMenuSettings.h
// and include that file, I get multiple defined sysmbol errors.
// I can't move it to ProjectRevision.h because I get errors as well.  This is caused by the fact all Config related header files
// get #included in one big ReaderConfig header file, so you are limited.  The only quick way around this is to put that single
// macro in its own header file.  What we need to implement is a way of "exporting" stuff in Menu header files to outside modules.
// One way to do this is have ReaderConfig define a macro called "FOR_READER_CONFIG before it #includes the supporting Menu
// header files. 
#define SIZE_OF_SOFTWARE_VERSION	(40)


#include "flashdrv.h"
#include "MocfTags.h"
#include "TlvContainerParser.h"
#include "flashapplicationcode.h"


/*----------------------------------------------------------------------------------------
SkipToNextRecord

  This moves the file pointer over to the next Record.


  Parameters: 
		pTlvReocrd[in]:  pointer to an Object Reocrd.

		pFile[in]:  F_FILE pointer.


  Returns:  nothing.
----------------------------------------------------------------------------------------*/
void SkipToNextRecord(const CTlv::Header_t * pTlvReocrd, F_FILE * pFile)
{
	DWORD DataAndPaddingLength = pTlvReocrd->Length + CTlvContainerParser::CalcNumPaddingBytes(pTlvReocrd);
	f_seek(pFile, DataAndPaddingLength, F_SEEK_CUR);  // skip the Length & padding bytes and point to the next Record
}



/*----------------------------------------------------------------------------------------
GetSwVersion

  This retrieves the Software Version string from the Application Firmware Object Record
  in the file system.


  Parameters: 
		pFile[in]:  F_FILE pointer.

		pSoftwareVersion[out]:  pointer to char array to hold the string.
		
		MaxLength[in]:  max length of string


  Returns:  true if the Record was found and read OK, false otherwise.
----------------------------------------------------------------------------------------*/
bool GetSwVersion(F_FILE * pFile, char * pSoftwareVersion, int MaxLength)
{
	bool bIsOk = false;
	CTlv::Header_t header;
	
	f_read(&header, 1, sizeof header, pFile); // this should be Gen6::tagCompatProd
	SkipToNextRecord(&header, pFile);
	
	f_read(&header, 1, sizeof header, pFile); // this should be Gen6::tagSwVersionStr
	if ( (header.Tag == Gen6::tagSwVersionStr) && (header.Length <= MaxLength) )
	{
		f_read(pSoftwareVersion, 1, header.Length, pFile);  // that Record contains a terminating 0
		bIsOk = true;
	}
	
	return bIsOk;
}

/*----------------------------------------------------------------------------------------
ReadSoftwareVersion

  This retrieves the Software Version string from the Application Firmware Object Record
  in the file system.  If one is not found, a fake string is constructed.


  Parameters: 
		pSoftwareVersion[out]:  pointer to char array to hold the string.
		
		MaxLength[in]:  max length of string


  Returns:  true if the Record was found and read OK, false otherwise.
----------------------------------------------------------------------------------------*/
void ReadSoftwareVersion(char * pSoftwareVersion, int MaxLength)
{
	CTlv::Header_t header;
	char SoftwareVersion[SIZE_OF_SOFTWARE_VERSION + 1];
	F_FILE * pAppFwObjectFile;
	
	pAppFwObjectFile = f_open(FILENAME_MAIN_APP, "r");
	if ( pAppFwObjectFile )
	{
		f_read(&header, 1, sizeof header, pAppFwObjectFile);
		if ( header.Tag == Gen6::tagAppFirmware )  // We test this because for a while, someone may inadvertently load a binary file & we should be ready for it
		{	
			if ( GetSwVersion(pAppFwObjectFile, SoftwareVersion, sizeof SoftwareVersion) )
			{
				sprintf(pSoftwareVersion, "%s", SoftwareVersion);
			}
			else sprintf(pSoftwareVersion, "ZZ000001ZZC"); // should never happen
		}
		else sprintf(pSoftwareVersion, "ZZ000002ZZC"); // could happen while migrating
	}
	else sprintf(pSoftwareVersion, "ZZ000003ZZC"); // should never happen
	f_close(pAppFwObjectFile);
}

