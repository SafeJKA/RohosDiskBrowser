// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__37351E32_B9A4_47AE_A64C_CB5BE587A197__INCLUDED_)
#define AFX_STDAFX_H__37351E32_B9A4_47AE_A64C_CB5BE587A197__INCLUDED_

#define _WIN32_WINNT  0x500

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

#include "common1.h"
#include "TraceMaker.h"
//#include "HtmlHelp.h"
#include "strings.h"

#include "shlwapi.h"
#include "pub.h"

BOOL IsFileUseSteganos(LPCTSTR FileName);
BOOL GetSteganosOffset(LPCTSTR FileName, LPDWORD begin_offset_mb,  LPDWORD ending_offset_mb, LPDWORD file_size_mb=NULL);


#define MAX_DISK_SIZEMB_MINIDRIVE	1024*1024*2200

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__37351E32_B9A4_47AE_A64C_CB5BE587A197__INCLUDED_)
