//======================================================================================
// OutputMenuSettings.h
//======================================================================================
// $RCSfile: OutputMenuSettings.h $
// $Revision: 1.8 $
// $Date: 2010/07/03 16:36:22EDT $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Module Interface Description:
//      This file contains the interface definition to a routine that creates
//		output menu settings as part of the overall reader configuration for
//		a Matrix device.  It also contains the active value declarations of
//		output settings.
//
//---------------------------------------------------------------------------


#ifndef OutputMenuSettingsH
#define OutputMenuSettingsH
//---------------------------------------------------------------------------


#include	"language.h"		// system global definitions
#include	"Readerconfigurationstorage.h"



#define	MAX_2D_BARCODE_MSG		4096
int			MaxBcodeMsgSize;


// Prefix / suffix and data format definitions.
#define PREFIX_BLOCKSIZE      200
#define SUFFIX_BLOCKSIZE      200
#define PREFIX_SUFFIX_OVERHEAD 5  // for reporting actual Prefix / Suffix programmable size
#define DATAFORMAT_BLOCKSIZE	2000


#define	DFM_DISABLE	0
#define	DFM_ENABLE	1
#define	DFM_REQUIRED	2
//add particular configuration
#define DFM_ENABKE_P		3
#define DFM_REQUIRED_P		4
//end adding particular configuration

//add specifing the default key
#define PrimaryKey			0
#define AlternateKey1		1
#define AlternateKey2		2
#define AlternateKey3		3

DATA_FORMAT		*DataFormatObj;
PREFIX_SUFFIX	*PrefixObj;
PREFIX_SUFFIX	*SuffixObj;
PREFIX_SUFFIX   *KeyboardSuffixObj; //Default Suffix when in Keyboard Mode
PREFIX_SUFFIX   *ComSuffixObj;  //Default Suffix with in Com or Com Emulation Mode

int				DataFormatEnable;
bool			PrefixEnable;
bool			SuffixEnable;
bool            SuffixFactoryEnable;
bool			RemoveFncCodes;
bool 			TimeoutTransmitSequenceEnable;

bool			base64enable;

bool 			NoMatchError;
int			SpecifiedDefaultKey;


int				iCodeIdNew;


int		IntCharDelay;
int		IntFuncDelay;
int		IntMsgDelay;
int		IntCharXDelay;
int		DelayChar;

#define	GRDLY_NONE	0
#define	GRDLY_500 	1
#define	GRDLY_1000	2
#define	GRDLY_1500	3

int		GoodReadDly;


#define	RRDLY_SHORT	0
#define	RRDLY_MEDIUM 	1
#define	RRDLY_LONG	2
#define	RRDLY_EXTRALONG	3

int		ReReadDly;
int     ReReadDly2D;
int		SkipDecodingTimeout;


bool	bOposMenuEnable;
bool	bOposAutoDisable;


bool	BufferedOutput;


bool 	DecodeHeader;


bool    RetailPDFEnable;
int 	SurePosMaxPacketSize;

int		DataRedundant;

void CreateOutputMenuSettings( int ControlMask );


#endif // OutputMenuSettingsH
//----------------------------------------------------------------------------




