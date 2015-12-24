
//************************************************************************
//
//  Interface Support Functions
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/menu/Interface_Support_Functions.h $
//
// $Date: 2011/03/10 15:05:46EST $
// $Revision: 1.11 $
//======================================================================================
//************************************************************************

#ifndef _INTERFACE_SUPPORT_H_
#define _INTERFACE_SUPPORT_H_


#define MAXIMUM_TERMINAL_USB_ID 134
#define MINIMUN_TERMINAL_USB_ID 124


extern bool bVisMnuOverride;
extern const char * VisMnuPtr;


MatrixError_t ReconfigureInterface();
void UpdateOpenInterfaceID(void);
void UpdateTermIDs( void );
void UpdateImageTransferSettings( void );
void UpdateDynamicMenuSettings( void );
void UpdateImageTransferSettings( void );
void ReSynchWorkingAndActive( bool = false );
void AlertDriverOfSettingsChange( void );
int CollectAllInterfaceTerminalIds(int MaxNumItems, int * pTagBufferData);
int CollectInterfaceKBWTerminalIds(int MaxNumItems, int * pTagBufferData);
int CollectInterfaceUSBTerminalIds(int * pTagBufferData);
void UpdateCodePages(void);	             // go add CodePages to the menu
BOOL ValidateTermId( int iTermId );
void InitializeGlobalTimers(void);
int GetInterfaceClass(void);
void DoPageButtonDetecting(void);
bool ExpandBaudRates(void);  // go allow higher baud rates for engineers.
bool IsHwTriggerPulled(void);


#define USB_INTERFACE_CLASS 1
#define SERIAL_INTERFACE_CLASS 2
#define BARCODE_INTERFACE_CLASS 3
#define TERMINAL_INTERFACE_CLASS 4

#endif


