/* ==============================================================================
   Project Revision Header
   ==============================================================================
   ##### IMPORTANT #####
   This header file must only be included in SystemMenuSettings.cpp.
   The macros defined here would result in duplicate data in the firmware
   if referenced from other modules.  Because of special builds such as
   the Product Config and Manufacturing Imager Test, the duplicates might not
   be identical.
   ============================================================================== */

#ifndef _PROJECT_REVISION_H_
#define _PROJECT_REVISION_H_

#define RevProject          ": heads/Dev_Speaker-0-gbdfb7d1-dirty"
#define FIRMWARE_GENERATION ": 000"
#define RevCustConfig       FIRMWARE_GENERATION
//Note: Software version is now used for a SurePos firmware level. The dash number must be between 2 and 254 (one byte).
// Zero does not work and one and 255 are reserved.  If this range is exceeded a new base number must be pulled.
//The generation represents a new base number. Increment it if the VID and PID remain the same and a new base
//number is issued.  The valid range for Firmware Generation is Zero to 254 (255 reserved).
//****FIRMWARE_GENERATION is entered as an intger in the form of the string shown below. *****
//The Generation has the format of :  #define  FIRMWARE_GENERATION         ": $CustomRevision: Generation: -000 $"
//The three numbers ("000") following the dash ('-') are the only characters in the string that get modified (-000 to -254).
extern const char pSW_GEN[];
extern const char g_TimeStamp[];
extern const char g_DateStamp[];
extern const char TimeDateRevisionString[];

const char* ReportProjectGenString(void);
void ReadSoftwareVersion(char * pSoftwareVersion, int MaxLength);

#endif    /* _PROJECT_REVISION_H_ */
