//======================================================================================
// PluginInterface.h
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Plugin/PluginInterface.h $
// $Revision: 1.7 $
// $Date: 2011/07/03 23:02:26EDT $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Module Interface Description:
//    This file contains the interfaces between plugins and Matrix OS.
//
//---------------------------------------------------------------------------


#ifndef _PLUGIN_INTERFACE_H
#define _PLUGIN_INTERFACE_H

#define PLUGIN_STRING_LEN	(64)
#define PLUGIN_GUID_LEN		(256)

enum HONPluginClassType
{
	HON_PLUGIN_TYPE_UNKNOW = 0,
	HON_PLUGIN_FORMAT,
	HON_PLUGIN_IMAGE,
	HON_PLUGIN_DECODE
};


typedef struct{
	// Raw name in plugin binary
	char PluginRawName[PLUGIN_STRING_LEN];

	// Raw Class Type in plguin binary
	HONPluginClassType PluginRawClassType;
		
	// Startup function.
	int (*PluginInitRoutine)(void *Info);

	// Destruction function.
	void (*PluginExitRoutine)(void);

	/* Plugin APIs. This should be in this structure in order that the plugin could assign APIs' address here */
	void *PluginApis;

	/* This field is the identifier assigned from Hoenywell. Menu codes with the identifier prefix will be passed to 
	 * the corresponding plug-in */
	 int MenuIdentifier;

	/* Other plugin infos */
	char CompanyName[PLUGIN_STRING_LEN];
	char MajorVersionNumber[PLUGIN_STRING_LEN];
	char MinorVersionNumber[PLUGIN_STRING_LEN];
	char BuildNumber[PLUGIN_STRING_LEN];
	char CertificateNumber[PLUGIN_STRING_LEN];
	char CertificateTime[PLUGIN_STRING_LEN];
	char GUID[PLUGIN_GUID_LEN];
	char PluginFileName[PLUGIN_STRING_LEN];
		
} HONPluginRawInfo;


/* Plugin API stuff */
#define VERSIONED_FUNCTIONS		/* We used versioned function approach */
#define MAX_DECODED_DATA_LEN		4096
#define MAX_SUPPL_DATA_LEN		128
#define MAX_SYMID_STRING_LEN		32


typedef struct {
	int RevisionNumber;			// Revision number
	unsigned char *message; 		// Output Data String
	int length;      				// Number of Data Characters
	int CharSize;					//Character size (1 for byte, 2 for word)
	char HHPcodeID;   			// HHP internal Code (Symbology) ID
	char AIMcodeLetter;  			// AIM/FACT/ISO "Symbology Identifier"
	char AIMcodeModifier; 		// ... and "Modifier" character
} DataEditParam;



typedef struct {
	/* Revision number */
	int RevisionNumber;

	/* TODO: Implement image plugin api parameters here */
	
} ImageParam;


typedef struct {
	/* Revision number */
	int RevisionNumber;

	/* TODO: Implement decode plugin api parameters here */
	
} DecodeParam;


typedef struct {
	int RevisionNumber;	/* Revision Number */
	char *GUID;			/* GUID of the plug-in */
	char *PluginName;	 	/* Plug-in name */
	char *CompanyName;	/* Company name of the plug-in */
	int MajorVersion;		/* Major version number */
	int MinorVersion;		/* Minor version number */
	int BuildNumber;		/* Build number of the plug-in version */
	char *CertificateNumber; 	/* Certificate number of the plug-in version */
	char *CertificateTime;		/* Certificate time (yyyy/mm/dd hh:mm:ss) */
} VersionInfo;


typedef struct
{
	int RevisionNumber;			/* Revision Number */
	int (*DataEdit)(DataEditParam *format_param);	/* Format API callback */
	int (*ProcessingBarcode)(char *pMenuData, int DataLength);	/* Plug-in Menuing API callback */
	int (*CheckLicense)(char *SN);		/* Check license API callback */
	int (*GetVersion)(VersionInfo *Info);		/* Get version API callback */
	void (*GPIO_Plugins)(void);

} DataEditApi;


typedef struct
{
	int RevisionNumber;			/* Revision Number */
	/* TODO: Define image plugin apis hooks here */

} ImageApi;


typedef struct
{
	/// Revision Number
	int RevisionNumber;			
	/// Format API callback
	int (*Decode)(unsigned char *pBuffer, int width, int height);
	/// Set Decoder Menu
	int (*SetDecoderMenu)(void *DecoderSetting);
	/// Plug-in Menuing API callback 
	int (*ProcessingBarcode)(char *pMenuData, int DataLength);	
	/// Check license API callback
	int (*CheckLicense)(char *SN);		
	/// Get version API callback
	int (*GetVersion)(VersionInfo *pInfo);
	void (*GPIO_Plugins)(void);
	int (*GetDecodeDataType)(void);	// 0 - mono, 1 - color, 2 - raw
} DecodeApi;


extern int wrapper_isatty(int fd);
extern int wrapper_stat(char *file, FS_STAT *fstat);
extern int wrapper_fstat(int file, FS_STAT *fstat);
extern int wrapper_lseek(int file, int offset, int whence);
extern int wrapper_close (int fd);
extern int wrapper_read(int fd, void *buf, int count);
extern int wrapper_write(int fd, void *buf, int count);
extern int wrapper_open(const char *filename, int flags, int mode);
extern int wrapper_rename(char *oldname, char *newname);
extern int wrapper_delete(char *file) ;
extern int wrapper_sbrk(int size);
extern int wrapper_ver();


#endif

//=================================================================================
//                       File Modification History
//===================================================================================

