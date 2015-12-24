//======================================================================================
// HCommandHandler.h
//======================================================================================
// $RCSfile: hcommandhandler.h $
// $Revision: 1.1 $
// $Date: 2009/03/08 16:03:38EDT $
//======================================================================================

//======================================================================================






//---------------------------------------------------------------------------
//
//  Class definitions for HCOMMAND_HANDLER
//
//---------------------------------------------------------------------------
#ifndef HCOMMAND_HANDLERH
#define HCOMMAND_HANDLERH
//---------------------------------------------------------------------------

#include "HString.h"

#ifndef HCOMMAND_HANDLER_API					// Only if it has not been defined yet
#ifdef WIN32
//#if (WIN32 || _WIN32_WCE)
	#ifdef WINAPP
		#define HCOMMAND_HANDLER_API
	#else
		#if (_USRDLL || DLL)
		  #define HCOMMAND_HANDLER_API __declspec(dllexport)
		#else
		  #define HCOMMAND_HANDLER_API __declspec(dllimport)
		#endif
	#endif
#else
	#ifdef _WIN32_WCE
	    #if (_USRDLL || DLL)
    	    #define HCOMMAND_HANDLER_API __declspec(dllexport)
	    #else
    	    #define HCOMMAND_HANDLER_API __declspec(dllimport)
	    #endif
	#else
	    #define HCOMMAND_HANDLER_API
	#endif
#endif
#endif	                                 // ifndef HCOMMAND_HANDLER_API

//---------------------------------------------------------------------------
class HCOMMAND_HANDLER_API HCOMMAND_HANDLER
{
public:
// The following functions are required in order to support HCOMMAND_HANDLER.  It is the responsibility of the
// instantiator to override these, which requires that they use this class as a base class.
	virtual bool WakeupDevice(void) { return(true); }




public:		// User declarations

// Contstructor for HCOMMAND_HANDLER.
//  Prepare for an HCOMMAND_HANDLER I/O.
	HCOMMAND_HANDLER(void);

// Destructor for HCOMMAND_HANDLER.
    ~HCOMMAND_HANDLER(void);
};
//---------------------------------------------------------------------------
#endif





