//======================================================================================
// PlugAndPlayMenuSettings.h
//======================================================================================
// $RCSfile: PlugAndPlayMenuSettings.h $
// $Revision: 1.3 $
// $Date: 2009/06/17 09:38:52EDT $
//======================================================================================

//======================================================================================




//---------------------------------------------------------------------------
//
//  Module Interface Description:
//      This file contains the interface definition to a routine that creates
//	plug and play menu settings as part of the overall reader configuration for
//	a Matrix device.  It also contains the active value declarations of
//	plug and play settings.
//
//---------------------------------------------------------------------------


#ifndef PlugAndPlayMenuSettingsH
#define PlugAndPlayMenuSettingsH
//---------------------------------------------------------------------------





bool	bKioskMode;// This bit enabled will go allow re-population of the DECMOD to allow DECMOD4, kiosk mode.



void CreatePlugAndPlayMenuSettings( int ControlMask );


#endif // PlugAndPlayMenuSettingsH
//----------------------------------------------------------------------------




