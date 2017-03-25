// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

//#define _MFC_VER 0x0601
//#define _AFX_NO_DEBUG_CRT
#define NO_WARN_MBCS_MFC_DEPRECATION
#define _CRT_SECURE_NO_WARNINGS

//#pragma warning(disable : 4200)
//#pragma warning(disable : 4244)
//#pragma warning(disable : 4838)

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0502
#endif

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions (including VB)
#include <afxtempl.h>

//#include <afxdao.h>         // MFC DAO database classes
//#include <afxcmn.h>         // MFC support for Windows 95 Common Controls
//#include <afxdisp.h>        // MFC OLE automation classes
//#include "ids.h"
//#include "util.h"

//#define STRICT
#define _ATL_APARTMENT_THREADED

#define CELEM(a) (sizeof(a)/sizeof(a[0])) 

#include <atlbase.h>
//#include <atlcom.h>
#include <comdef.h>
#include <atldef.h>
#include <afxcmn.h>
#include <limits.h>
