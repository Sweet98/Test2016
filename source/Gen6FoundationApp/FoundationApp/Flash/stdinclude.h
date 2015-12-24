/*=================================================================================
  class:

//=================================================================================
   $Source: Gen6FoundationApp/FoundationApp/Flash/stdinclude.h $
   $Date: 2008/12/17 13:39:00EST $
   $Revision: 1.2 $


//=================================================================================*/

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#ifndef AFX_STDAFX_H__40335AE0_886E_4CCE_9CA1_08AC0CD226B7__INCLUDED_
#define AFX_STDAFX_H__40335AE0_886E_4CCE_9CA1_08AC0CD226B7__INCLUDED_

#ifdef WIN32

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "Product.h"

#ifdef WINSIM
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

// ensure that we use our version rathter the files coming with the MS-Compiler
#include ".\uchar.h"
#include ".\wchar.h"
#include "..\..\..\libraries\OsKernel\db_shared\tchar.h"
#include "HW_simulation.h"
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#endif

#elif defined(__arm)
#include <stdlib.h>
//#include <stdcomp.h> TEST ~DV
#include <stdarg.h>
#include <string.h>

#include "Product.h"
#include <wchar.h>
#include "tchar.h"
#endif



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // AFX_STDAFX_H__40335AE0_886E_4CCE_9CA1_08AC0CD226B7__INCLUDED_




