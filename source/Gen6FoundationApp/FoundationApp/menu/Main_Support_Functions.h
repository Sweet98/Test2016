
/************************************************************************/
//
//  Main Support Functions
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/menu/Main_Support_Functions.h $
//======================================================================================
/************************************************************************/
#ifndef _MAIN_SUPPORT_H_
#define _MAIN_SUPPORT_H_


#include	"ErrorDefs.h"

//void ProcessInterfaceData(void);                                    //lmc   removed and placed in SynCommand Parser
void ServiceTimeouts(void);
//MatrixError_t ReconfigureInterface();                         //lmc   removed and placed in interface_support_functions
//bool SecondaryInterfaceDetected( bool );
BOOL OpenSecondaryInterface( void );
//void UpdateTermIDs( void );                                       //lmc   removed and placed in interface_support_functions
void UpdateCodePages( void );
//void UpdateDynamicMenuSettings( void );                              //lmc   removed and placed in interface_support_functions
//void UpdateImageTransferSettings( void );                            //lmc   removed and placed in interface_support_functions
void FlashProductConfigurationChanges( void );
void FlashAndReSynchCustomerDefaults( void );
void FlashWorkingChanges( void );
//void ReSynchWorkingAndActive( bool = false );                         //lmc   removed and placed in interface_support_functions
//void AlertDriverOfSettingsChange( void );                             //lmc   removed and placed in interface_support_functions
bool	SendRevisionInfo(void);
//int CollectAllInterfaceTerminalIds( int * pTagBufferData);             //lmc   removed and placed in interface_support_functions
//BOOL ValidateTermId( int iTermId );                                       //lmc   removed and placed in interface_support_functions
void CheckForScannerInCradle(void);
//void InitializeGlobalTimers(void);                                      //lmc   removed and placed in interface_support_functions
//int GetInterfaceClass(void);                                            //lmc   removed and placed in interface_support_functions
void CheckForRadioBaudUpdate(void);
//void DoRemoveAddr(void);                                                 //lmc   removed and placed in interface_support_functions
//void reset_to_boot_code(void);
void RemovePowerOnError(void);

#if defined(SHOW_RESET_TO_BOOT) && !defined(MATRIX_BOOT_CODE)
#define reset_to_boot_code() _reset_to_boot_code(__LINE__)
void _reset_to_boot_code(int);
#else
void reset_to_boot_code(void);
#endif

#define USB_INTERFACE_CLASS 1
#define SERIAL_INTERFACE_CLASS 2
#define BARCODE_INTERFACE_CLASS 3
#define TERMINAL_INTERFACE_CLASS 4

#endif


